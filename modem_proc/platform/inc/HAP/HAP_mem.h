#ifndef HAP_MEM_H
#define HAP_MEM_H
/*==============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "AEEStdDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAP_MEM_CONTEXT_DEFAULT (void*)0

int HAP_cx_malloc(void* pctx, uint32 bytes, void** pptr);
int HAP_cx_free(void* pctx, void* ptr);

static inline int HAP_malloc(uint32 bytes, void** pptr)
{
  return HAP_cx_malloc(HAP_MEM_CONTEXT_DEFAULT, bytes, pptr);
}

static inline int HAP_free(void* ptr)
{
  return HAP_cx_free(HAP_MEM_CONTEXT_DEFAULT, ptr);
}

#ifdef __cplusplus
}
#endif

#endif // HAP_MEM_H

