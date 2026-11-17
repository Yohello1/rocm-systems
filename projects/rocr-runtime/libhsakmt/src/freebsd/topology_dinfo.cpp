////////////////////////////////////////////////////////////////////////////////
//
// The University of Illinois/NCSA
// Open Source License (NCSA)
//
// Copyright (c) 2026, Advanced Micro Devices, Inc. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * FreeBSD implementation of the dinfo backend - STUB.
 *
 * FreeBSD has no sysfs tree, so there is nothing at
 * /sys/devices/virtual/kfd/kfd/topology to read. The plan is for the
 * FreeBSD amdkfd kernel module to expose the same information as a
 * sysctl(3) MIB tree mirroring the sysfs layout leaf-for-leaf, e.g.:
 *
 *   hw.amdgpu.kfd.topology.generation_id
 *   hw.amdgpu.kfd.topology.nodes.<N>.gpu_id
 *   hw.amdgpu.kfd.topology.nodes.<N>.properties
 *   hw.amdgpu.kfd.topology.nodes.<N>.mem_banks.<M>.properties
 *   hw.amdgpu.kfd.topology.nodes.<N>.caches.<C>.properties
 *   hw.amdgpu.kfd.topology.nodes.<N>.{io_links,p2p_links}.<L>.properties
 *
 * read via sysctlbyname()/sysctlnametomib(), handing back the SAME
 * "key value\n" text format the Linux sysfs files already contain so none
 * of the parsing logic in topology.c has to change.
 *
 * generation_id is load-bearing: topology_take_snapshot() in topology.c
 * reads it before and after the whole node walk and retries on mismatch.
 * That's the only consistency mechanism across this many non-atomic reads,
 * so whatever populates these sysctl nodes in the kernel MUST bump
 * generation_id on any topology change (hotplug, etc.), or the retry loop
 * can't detect a torn read.
 *
 * Model mode (hsakmt_use_model) is NOT a sysctl concern - it's a plain
 * directory of files (hsakmt_model_topology) on every OS, populated by
 * hsakmtmodel.c. get_dinfo_dir() below still has to honor it before
 * falling into the sysctl path.
 *
 * Every function here currently returns HSAKMT_STATUS_NOT_SUPPORTED (or
 * the equivalent "nothing found" value) as a placeholder - fill in the
 * sysctlbyname() calls per function.
 */

#include <cstring>
#include <sys/sysctl.h>
#include <sys/types.h.>

extern "C" {
#include "fmm.h"
#include "hsakmt/hsakmtmodel.h"
}
#include "topology_dinfo.h"

/* Confirmed against the live kernel module (freebsd-halo, FreeBSD 16.0-CURRENT,
 * amdgpu.ko loaded): the real MIB root is sys.class.kfd.kfd.topology, a literal
 * "/" -> "." translation of the Linux sysfs path /sys/class/kfd/kfd/topology.
 * NOT hw.amdgpu.kfd.* - that was this file's original placeholder guess. */
#define KFD_DINFO_SYSCTL_PREFIX "sys.class.kfd.kfd.topology"

extern "C" const char *get_dinfo_dir(void)
{
	if (hsakmt_use_model)
		return hsakmt_model_topology;
	return KFD_DINFO_SYSCTL_PREFIX;
}

extern "C" HSAKMT_STATUS topology_dinfo_get_generation(uint32_t *gen)
{

	HSAKMT_STATUS ret = HSAKMT_STATUS_SUCCESS;
	size_t len = sizeof(*gen)

	if(sysctlbyname("hw.amdgpu.kfd.topology.generation_id", gen, &len, NULL, 0))
		ret = HSAKMT_STATUS_ERROR;

	return ret;
}

extern "C" HSAKMT_STATUS topology_dinfo_map_node_id(
					struct hsa_kfd_topology_context *topology_ctx,
					uint32_t node_id, uint32_t *sys_node_id)
{
	if ((!topology_ctx->map_user_to_sysfs_node_id) ||
		(node_id >= topology_ctx->map_user_to_sysfs_node_id_size))
		return HSAKMT_STATUS_NOT_SUPPORTED;

	*sys_node_id = topology_ctx->map_user_to_sysfs_node_id[node_id];
	return HSAKMT_STATUS_SUCCESS;
}

extern "C" HSAKMT_STATUS topology_dinfo_get_gpu_id(uint32_t sysfs_node_id, uint32_t *gpu_id)
{
	HSAKMT_STATUS ret = HSAKMT_STATUS_SUCCESS;
	size_t len = sizeof(*gpu_id);
	char key[256];
	

	snprintf(key, sizeof(key), "hw.amdgpu.kfd.topology.nodes.%u.gpu_id", sysfs_node_id); 

	if (sysctlbyname(key, gpu_id, &len, NULL, 0))
		ret = HSAKMT_STATUS_ERROR;
	return ret; 
}

extern "C" HSAKMT_STATUS topology_dinfo_check_node_supported(HsaKFDContext *ctx,
					uint32_t sysfs_node_id, bool *is_node_supported)
{
	/* TODO: fetch drm_render_minor from the node's properties sysctl,
	 * then hsakmt_open_drm_render_device(ctx, drm_render_minor) exactly
	 * like the Linux backend does. */
	(void)ctx;
	(void)sysfs_node_id;
	*is_node_supported = false;
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

extern "C" HSAKMT_STATUS hsakmt_topology_dinfo_get_system_props(HsaKFDContext *ctx,
					HsaSystemProperties *props)
{
	/* TODO: sysctlbyname(".system_properties", ...) for
	 * PlatformOem/PlatformId/PlatformRev, then enumerate node count
	 * (likely a dedicated "node count" sysctl rather than num_subdirs(),
	 * since there's no directory to list) and populate
	 * ctx->topology_context->map_user_to_sysfs_node_id the same way the
	 * Linux backend does. */
	(void)ctx;
	(void)props;
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

extern "C" HSAKMT_STATUS topology_dinfo_get_node_props_blob(uint32_t sys_node_id,
					char *buf, size_t buflen)
{
	/* TODO: sysctlbyname("hw.amdgpu.kfd.topology.nodes.<N>.properties",
	 * buf, &buflen, NULL, 0), NUL-terminate. Must return the same
	 * "key value\n" text format as Linux - topology.c's parser is
	 * unchanged and expects exactly that. */
	(void)sys_node_id;
	(void)buf;
	(void)buflen;
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

extern "C" HSAKMT_STATUS topology_dinfo_get_mem_props(
					  struct hsa_kfd_topology_context *topology_ctx,
					  uint32_t node_id,
					  uint32_t mem_id,
					  HsaMemoryProperties *props)
{
	/* TODO: sysctlbyname(".nodes.<N>.mem_banks.<M>.properties", ...) */
	(void)topology_ctx;
	(void)node_id;
	(void)mem_id;
	(void)props;
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

extern "C" HSAKMT_STATUS topology_dinfo_get_cache_props(
					    struct hsa_kfd_topology_context *topology_ctx,
					    uint32_t node_id,
					    uint32_t cache_id,
					    HsaCacheProperties *props)
{
	/* TODO: sysctlbyname(".nodes.<N>.caches.<C>.properties", ...) */
	(void)topology_ctx;
	(void)node_id;
	(void)cache_id;
	(void)props;
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

extern "C" HSAKMT_STATUS topology_dinfo_get_iolink_props(HsaKFDContext *ctx,
					     uint32_t node_id,
					     uint32_t iolink_id,
					     HsaIoLinkProperties *props, bool p2pLink)
{
	/* TODO: sysctlbyname(".nodes.<N>.{io_links,p2p_links}.<L>.properties", ...) */
	(void)ctx;
	(void)node_id;
	(void)iolink_id;
	(void)props;
	(void)p2pLink;
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

extern "C" bool topology_dinfo_check_fabric_handle_supported(uint32_t drm_render_minor)
{
	/* TODO: UALink fabric-handle capability probe. No FreeBSD UALink
	 * sysctl exists yet - returning false (unsupported) is correct until
	 * one does. */
	(void)drm_render_minor;
	return false;
}
