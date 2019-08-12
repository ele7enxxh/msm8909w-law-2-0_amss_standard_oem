#ifndef SMMUCLOCK_H
#define SMMUCLOCK_H
/*=============================================================================

                       SMMU Clock Internal APIs

FILE:      SMMUClock.h
   System MMU

GENERAL DESCRIPTION
  This file contains internal SMMU clock APIs.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
===============================================================================
Copyright (c) 2014
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/smmu/smmu500/8909/tz/SMMUClock.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/14/14   sc      Added vote for TCU clocks
02/25/14   sc      Initial version
=============================================================================*/
#include "SMMUTypes.h"

typedef enum
{
  SMMU_CLK_APPS_TCU,
  SMMU_CLK_GPU_TCU,
  SMMU_CLK_ALL_TCU, // both GPU_TCU and APPS_TCU
  SMMU_CLK_MAX_TCU,
  SMMU_CLK_UNSUPPORTED_TCU = 0x7FFFFFFF,
} SMMU_ClkTCUVoteType;

SMMUResult_t SMMU_ClockEnable(SMMU_ClkTCUVoteType eTCU);

SMMUResult_t SMMU_ClockDisable(SMMU_ClkTCUVoteType eTCU);

#endif // SMMUCLOCK_H
