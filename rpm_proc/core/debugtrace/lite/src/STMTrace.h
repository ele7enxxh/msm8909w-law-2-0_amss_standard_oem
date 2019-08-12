/*=============================================================================	
   Copyright (c) 2011 Qualcomm Technologies Incorporated.	
   All Rights Reserved.
	$Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/lite/src/STMTrace.h#1 $
=============================================================================*/
#ifndef STMTRACE_H
#define STMTRACE_H

#include "HALcomdef.h"

typedef enum
{
   TRACE_DATA_NONETYPE = 0x00,    /* Trace packet not gauranteed, without ts & mark */
   TRACE_DATA_GUARANTEED = 0x80,  /* Trace packet gauranteed */
   TRACE_DATA_MARKED = 0x10,      /* Trace packet marked */
   TRACE_DATA_TIMESTAMPED = 0x08, /* Trace packet timestamped */
   _PLACEHOLDER_STMTraceDataType = 0x7f
} eSTMTraceDataType;

typedef uint8 STMTraceDataType;

typedef enum
{
   TRACE_FLAG_NONETYPE = 0x00,    /* Trace packet not gauranteed, without ts & mark */
   TRACE_FLAG_GUARANTEED = 0x80,  /* Trace packet gauranteed */
   TRACE_FLAG_TIMESTAMPED = 0x08, /* Trace packet timestamped */
   _PLACEHOLDER_STMTraceFlagType = 0x7f
} eSTMTraceFlagType;

typedef uint8 STMTraceFlagType;

typedef enum
{
   TRACE_TRIG_NONETYPE = 0x00,    /* Trace packet not gauranteed, without ts & mark */
   TRACE_TRIG_GUARANTEED = 0x80,  /* Trace packet gauranteed */
   TRACE_TRIG_TIMESTAMPED = 0x08, /* Trace packet timestamped */
   _PLACEHOLDER_STMTraceTriggerType = 0x7f
} eSTMTraceTriggerType;

typedef uint8 STMTraceTriggerType;

int STMTrace_NewPort(uint32 *uPort);
void STMTrace_FreePort(uint32 uPort);
void STMTrace_Data32(uint32 uPort, STMTraceDataType uType, uint32 uData);
void STMTrace_Data16(uint32 uPort, STMTraceDataType uType, uint16 uData);
void STMTrace_Data8(uint32 uPort, STMTraceDataType uType, uint8 uData);
void STMTrace_Flag(uint32 uPort, STMTraceFlagType uType);
void STMTrace_Trigger(uint32 uPort, STMTraceTriggerType uType);

#endif // STMTRACE_H
