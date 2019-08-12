#ifndef HAP_PERF_H
#define HAP_PERF_H
/*==============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "AEEStdDef.h"

#ifdef __cplusplus
extern "C" {
#endif

uint64 HAP_perf_get_time_us(void);
uint64 HAP_perf_get_pcycles(void);

#ifdef __cplusplus
}
#endif

#endif // HAP_PERF_H
