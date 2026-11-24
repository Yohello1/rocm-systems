////////////////////////////////////////////////////////////////////////////////
//
// The University of Illinois/NCSA
// Open Source License (NCSA)
//
// Copyright (c) 2026, Advanced Micro Devices, Inc. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Shared contract between topology.c and the per-OS device-info ("dinfo")
 * backends.
 *
 * "dinfo" replaces what used to be called "sysfs" in this file: on Linux
 * the backend still reads /sys, but on FreeBSD there is no sysfs tree, so
 * the backend reads an equivalent sysctl(3) MIB instead. topology.c itself
 * never knows which one it's talking to - it only calls the functions
 * declared below. All node/mem-bank/cache/io-link *parsing* and the
 * gfx-version/marketing-name business logic built on top of that stays in
 * topology.c, shared unchanged by both backends - only the raw byte-fetch
 * primitives are implemented twice.
 *
 * Implementations:
 *   - src/linux/topology_dinfo.cpp   (sysfs-backed, ported verbatim from
 *     the original topology.c code)
 *   - src/freebsd/topology_dinfo.cpp (stub - sysctl-backed implementation
 *     TODO)
 */

#ifndef TOPOLOGY_DINFO_H_INCLUDED
#define TOPOLOGY_DINFO_H_INCLUDED

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* libhsakmt.h has no C++ guards of its own; without this the .cpp
 * backends see its hsakmt_topology_dinfo_get_system_props() declaration
 * with C++ linkage. */
#include "libhsakmt.h"

/* node_props_t / hsa_kfd_topology_context used to be defined privately in
 * topology.c. They moved here because topology_dinfo_map_node_id(),
 * hsakmt_topology_dinfo_get_system_props(), and topology_dinfo_get_iolink_props()
 * all need direct field access to the node-id mapping cache. */
typedef struct {
	HsaNodeProperties node;
	HsaMemoryProperties *mem;     /* node->NumBanks elements */
	HsaCacheProperties *cache;
	HsaIoLinkProperties *link;
} node_props_t;

struct hsa_kfd_topology_context
{
	HsaSystemProperties *system_props;
	node_props_t *node_props;

	/* This array caches dinfo-based node IDs of CPU nodes + all supported
	 * GPU nodes. It will be used to map user-node IDs to dinfo-node IDs.
	 */
	uint32_t *map_user_to_sysfs_node_id;
	uint32_t map_user_to_sysfs_node_id_size;

	uint32_t num_sysfs_nodes;
};

/* Defined in topology.c, shared by both dinfo backends. */
int num_subdirs(char *dirpath, char *prefix);
HSAKMT_STATUS topology_map_sysfs_to_user_node_id(struct hsa_kfd_topology_context *topology_ctx,
						  uint32_t sys_node_id, uint32_t *user_node_id);

/* ---- The dinfo backend contract ---- */

/* Live root (sysfs path on Linux) or the hsakmt_model_topology root when
 * hsakmt_use_model is set. Model mode is plain-file-based on every OS, so
 * each backend still has to honor hsakmt_use_model itself. */
const char *get_dinfo_dir(void);

HSAKMT_STATUS topology_dinfo_get_generation(uint32_t *gen);

HSAKMT_STATUS topology_dinfo_map_node_id(struct hsa_kfd_topology_context *topology_ctx,
					  uint32_t node_id, uint32_t *sys_node_id);

HSAKMT_STATUS topology_dinfo_get_gpu_id(uint32_t sysfs_node_id, uint32_t *gpu_id);

HSAKMT_STATUS topology_dinfo_check_node_supported(HsaKFDContext *ctx,
						   uint32_t sysfs_node_id,
						   bool *is_node_supported);

HSAKMT_STATUS hsakmt_topology_dinfo_get_system_props(HsaKFDContext *ctx,
						      HsaSystemProperties *props);

/* Fetches the raw node "properties" record for @sys_node_id into @buf as a
 * NUL-terminated "key value\n" blob. All parsing of that blob stays in
 * topology.c so platform backends never duplicate the gfx-version-override/
 * marketing-name business logic built on top of it. */
HSAKMT_STATUS topology_dinfo_get_node_props_blob(uint32_t sys_node_id,
						  char *buf, size_t buflen);

HSAKMT_STATUS topology_dinfo_get_mem_props(struct hsa_kfd_topology_context *topology_ctx,
					    uint32_t node_id, uint32_t mem_id,
					    HsaMemoryProperties *props);

HSAKMT_STATUS topology_dinfo_get_cache_props(struct hsa_kfd_topology_context *topology_ctx,
					      uint32_t node_id, uint32_t cache_id,
					      HsaCacheProperties *props);

HSAKMT_STATUS topology_dinfo_get_iolink_props(HsaKFDContext *ctx,
					       uint32_t node_id, uint32_t iolink_id,
					       HsaIoLinkProperties *props, bool p2pLink);

/* UALink fabric-handle capability probe. Used only by
 * topology_get_node_props_from_drm() (libdrm_amdgpu path, otherwise not
 * OS-sensitive) - split out because it's the one raw device-info read in
 * that function. */
bool topology_dinfo_check_fabric_handle_supported(uint32_t drm_render_minor);

#ifdef __cplusplus
}
#endif

#endif /* TOPOLOGY_DINFO_H_INCLUDED */
