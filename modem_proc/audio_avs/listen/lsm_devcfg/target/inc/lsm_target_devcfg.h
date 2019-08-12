/*========================================================================
   This file contains LSM device configuration functions

  Copyright (c) 2009-2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

 ====================================================================== */
#ifndef LSM_TARGET_DEVCFG_H
#define LSM_TARGET_DEVCFG_H

#include "typedef_drivers.h"


// For MMPM CPP factor denominator
#define LSM_MMPM_COMMON_DENOM       100
// Number of BW index
#define LSM_MMPM_BW_MAX_INDEX       10
// Default CPP factor numerator in case of missing devcfg
#define LSM_MMPM_DEFAULT_CPP_NUM    200

typedef struct
{
   uint32 sw_revision;                /* LSM sw revision version */
   uint32 bw[LSM_MMPM_BW_MAX_INDEX];  /* LSM BW levels*/
   uint32 cpp[LSM_MMPM_BW_MAX_INDEX]; /* LSM CPP factor * 100 */
} lpasshwio_prop_lsm_mmpm_struct_t;

#endif // #ifndef LSM_TARGET_DEVCFG_H
