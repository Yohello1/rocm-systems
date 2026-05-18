#include "libhsakmt.h"

HSAKMT_STATUS HSAKMTAPI hsaKmtPmcGetCounterProperties(HSAuint32 NodeId,
						      HsaCounterProperties **CounterProperties)
{
	pr_warn_once("not supported\n");
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

HSAKMT_STATUS HSAKMTAPI hsaKmtPmcAcquireTraceAccessCtx(HsaKFDContext* ctx,
						    HSAuint32 NodeId,
						    HSATraceId TraceId)
{
	pr_warn_once("not supported\n");
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

HSAKMT_STATUS HSAKMTAPI hsaKmtPmcAcquireTraceAccess(HSAuint32 NodeId,
						    HSATraceId TraceId)
{
	pr_warn_once("not supported\n");
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

HSAKMT_STATUS HSAKMTAPI hsaKmtPmcReleaseTraceAccess(HSAuint32 NodeId,
						    HSATraceId TraceId)
{
	pr_warn_once("not supported\n");
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

HSAKMT_STATUS HSAKMTAPI hsaKmtPmcStartTrace(HSATraceId TraceId,
					    void *TraceBuffer,
					    HSAuint64 TraceBufferMemorySize)
{
	pr_warn_once("not supported\n");
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

HSAKMT_STATUS HSAKMTAPI hsaKmtPmcQueryTrace(HSATraceId TraceId)
{
	pr_warn_once("not supported\n");
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

HSAKMT_STATUS HSAKMTAPI hsaKmtPmcStopTrace(HSATraceId TraceId)
{
	pr_warn_once("not supported\n");
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

HSAKMT_STATUS HSAKMTAPI hsaKmtPmcRegisterTrace(HSAuint32 NodeId,
					       HSAuint32 NumberOfCounters,
					       HsaCounter *Counters,
					       HsaPmcTraceRoot *TraceRoot)
{
	pr_warn_once("not supported\n");
	return HSAKMT_STATUS_NOT_SUPPORTED;
}

HSAKMT_STATUS HSAKMTAPI hsaKmtPmcUnregisterTrace(HSAuint32 NodeId,
						 HSATraceId TraceId)
{
	pr_warn_once("not supported\n");
	return HSAKMT_STATUS_NOT_SUPPORTED;
}
