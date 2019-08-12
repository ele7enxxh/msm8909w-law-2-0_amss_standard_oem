#ifndef SMMUINTERNAL_H
#define SMMUINTERNAL_H
/*=============================================================================

                       SMMU Internal Header

FILE:      SMMUInternal.h
   System MMU

GENERAL DESCRIPTION
  This file contains internal SMMU prototypes.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
===============================================================================
Copyright (c) 2013, 2014
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/smmu/smmu500/SMMUInternal.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/02/13   sc      Added macro to extract TBU info
11/08/13   sc      Added bypass define
10/04/13   sc      Cleaned up
=============================================================================*/
#include "SMMUTypes.h"
#include "SMMUConfig.h"

/*-----------------------------------------------------------------------------
 * Externs 
 *---------------------------------------------------------------------------*/
extern SMMUConfig_t smmuConfigInfo[];

/*-----------------------------------------------------------------------------
 * Defines 
 *---------------------------------------------------------------------------*/
#define SMMU_CONFIG_BYPASS_ATTR_CTRL_SIDS 0xFFFFFFFF

// fault to be ignored
#define SMMU_CB_FSR_FORMAT_SCHEME 0x200

/*-----------------------------------------------------------------------------
 * Macros 
 *---------------------------------------------------------------------------*/
#define SMMU_GET_TBUID_FROM_SMRVAL(smr) (((smr) >> 10) & 0x1F)
#define SMMU_GET_TBUSID_FROM_SMRVAL(smr) ((smr) & 0x3FF)
#define SMMU_GET_SMRMASK_FROM_SMRVAL(smr) (((smr) >> 16) & 0x7FFF)
extern uint32 smmu_num_max_devices;
/*-----------------------------------------------------------------------------
 * Helper Functions Prototypes 
 *---------------------------------------------------------------------------*/
// initialize global register space of the SMMU device
SMMUResult_t SMMU_InitDevice
  ( SMMUDeviceId_t smmuDeviceID,
    SMMUAddr_t pageTablePhysBase );

// validate the SMMU device initialization
SMMUResult_t SMMU_ValidateDeviceInitialized
  ( SMMUDeviceConfigEntry_t *pSMMUDeviceConfigEntry );

/*-----------------------------------------------------------------------------
 * Inline Helper Functions 
 *---------------------------------------------------------------------------*/
// inline helper functions
static __inline boolean SMMU_DEVICEID_VALID
(
  SMMUDeviceId_t smmuDeviceID
)
{
  return ((smmuDeviceID >= 0) && (smmuDeviceID < (SMMUDeviceId_t)smmu_num_max_devices));
}

static __inline boolean SMMU_CB_VALID
(
  SMMUDeviceId_t smmuDeviceID,
  uint32 cb
)
{
  return (cb < SMMU_NUM_MAX_CTX_BANKS);
}

static __inline boolean SMMU_DEVICE_CB_VALID
(
  SMMUDeviceId_t smmuDeviceID,
  uint32 cb
)
{
  return (SMMU_DEVICEID_VALID(smmuDeviceID) && SMMU_CB_VALID(smmuDeviceID, cb));
}

#endif /*SMMUINTERNAL_H*/
