#ifndef SMMUCONFIG_H
#define SMMUCONFIG_H
/*=========================================================================

                       S M M U  D A L  D R I V E R 

FILE:      SMMUCONFIG.h

GENERAL DESCRIPTION
  This file contains data structures needed for driver configuration

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 

      Copyright (c) 2010 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/
/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/smmu/smmu500/8909/tz/SMMUConfig.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/06/14   sc      Updated with interim workaround to disable context caching
03/03/14   sc      Updated SID mappings
01/14/14   sc      Added S2 config
12/02/13   sc      Cleaned up a bit the struct
11/08/13   sc      Updated for 8916
09/04/13   sc      Removed interrupt number defines
03/01/12   spa     Initial Version
==========================================================================*/
#include "SMMUTypes.h"
#include "tzbsp_vmid_config.h"

#define SMMU_NO_ACCESS_VMID      TZBSP_VMID_NOACCESS
#define SMMU_RPM_VMID            TZBSP_VMID_RPM
#define SMMU_SROT_VMID           TZBSP_VMID_TZ
#define SMMU_HLOS_VMID           TZBSP_VMID_AP
#define SMMU_APCPU_VMID          TZBSP_VMID_APCPU
#define SMMU_MODEM_VMID          TZBSP_VMID_MSS
#define SMMU_WCNSS_VMID          TZBSP_VMID_WLAN
#define SMMU_CP_VMID             TZBSP_VMID_CP
#define SMMU_VIDEO_VMID          TZBSP_VMID_VIDEO

#define SMMU_NUM_MAX_DEVICES   2 //maximun number of SMMU instances in the system

#define SMMU_NUM_MAX_SIDS             17 
#define SMMU_NUM_MAX_BYPASS_SIDS      3
#define SMMU_NUM_MAX_SID_BITS_IN_MASK 32
#define SMMU_NUM_MAX_CTX_BANKS        32
#define SMMU_MAX_SECURE_MIDS          8 // TODO: this can be changed based on the number of SIDs
#define SMMU_NUM_MAX_BP_ATTR_CTRL     4
#define SMMU_NUM_APPS_TBUS            8
#define SMMU_MAX_TBU_IDS              9

// for bypass attribute control config
#define SMMU_INVALID_VMID             0xFF
#define SMMU_INVALID_BPSID            0xFFFFFFFF

// for SID2VMID config
#define SMMU_INVALID_TBUID            0xFFFFFFFF

// interim workaround to disable context caching
// will need to be removed once HLOS/TZ provide the actual WA
#define SMMU_CB_VENUS_FW 22


/*-----------------------------------------------------------------------------
 * SMMU 500 SS Registers
 * --------------------------------------------------------------------------*/
#ifdef TZBSP_WITH_TBASE
#define SMMU_HAL_SS_LOCAL                  (0x01EF0000 + \
                                            TZBSP_SANDBOX_RELOCATE_OFFSET)
#else
#define SMMU_HAL_SS_LOCAL                   0x01EF0000
#endif

// SID2VMID_SMRn offset
#define SMMU_HAL_SID2VMID_SMR0_JPEG         0x00003000
#define SMMU_HAL_SID2VMID_SMR0_VFE          0x00004000
#define SMMU_HAL_SID2VMID_SMR0_MDP          0x00005000
#define SMMU_HAL_SID2VMID_SMR0_VENUS        0x00006000
#define SMMU_HAL_SID2VMID_SMR0_PRONTO       0x00007000
#define SMMU_HAL_SID2VMID_SMR0_PERIPH       0x00008000
#define SMMU_HAL_SID2VMID_SMR0_DIME         0x00009000
#define SMMU_HAL_SID2VMID_SMR0_GPS          0x0000A000
#define SMMU_HAL_SID2VMID_SMR0_QDSP         0x0000B000
#define SMMU_HAL_SID2VMID_SMR0_GFX          0x0000C000
#define SMMU_HAL_SID2VMID_SMR0_CPP          0x0000E000
#define SMMU_HAL_SID2VMID_SMR0_MDP_RT       0x0000F000

/*------------------------------------------------------------------------------
Local typedef definitions
------------------------------------------------------------------------------*/
typedef enum SMMUDeviceId_t {
  SMMU_APPS,
  SMMU_GPU,
  SMMU_DEVICE_ID_INVALID = -1
} SMMUDeviceId_t;

typedef enum SMMUCtxBankType {
  SMMU_CB_S2_CTX,
  SMMU_CB_S1_CTX_S2_BYPASS,
  SMMU_CB_S1_CTX_S2_FAULT,
  SMMU_CB_S1_CTX_S2_NESTED,
} SMMUCtxBankType;

// SMMU 500: struct to hold TBU SS info
typedef struct SMMUTBUSSInfo_t
{
  SMMUAddr_t uTBUSSVirtAddr;
  uint8      uTBUMaxSID2VmidEntries;
} SMMUTBUSSInfo_t;

typedef struct SMMUCtxBankConfigEntry_t {
  // VMID of the client master that is using this context bank
  uint8                           clientVMID;
  uint8                           interruptIndex;													   
  uint8                           Valid;
  uint8                           secure;
  SMMUCtxBankType                 eCBType;
  uint8                           uS2NestedCBIndex; // ignored if CB type != S2 nested
  // actual number of SIDs configured
  uint16                          uNumCfgSIDs;
  // list of Stream IDs with SMR masked (i.e., exact SMR value) attached to this context bank
  uint32                          uMaskedSIDs[SMMU_NUM_MAX_SIDS];
} SMMUCtxBankConfigEntry_t;

// SMMU 500: for "bypass attribute control" configuration
typedef struct SMMUBPAttrCtrlConfigEntry_t {
  uint8                      clientVMID;
  uint32                     uMaskedSIDs[SMMU_NUM_MAX_BYPASS_SIDS];
} SMMUBPAttrCtrlConfigEntry_t;

typedef struct SMMUDeviceConfigEntry_t {
  SMMUAddr_t                 baseVirtAddr; // base address for context banks, For TZ, this is actually a physical Address 
  uint8                      numNonSecureCBs;
  uint8                      numSecureCBs;
  uint16                     pad;
  uint16                     numSMRGroupOverride;
  uint8                      numCBGroupOverride;
  uint8                      numIRPTOverride;
  uint8                      secureMastersList[SMMU_MAX_SECURE_MIDS];
  // SMMU 500: for "bypass attribute control" context bank
  SMMUBPAttrCtrlConfigEntry_t BPAttrCtrlConfigEntry[SMMU_NUM_MAX_BP_ATTR_CTRL];
  SMMUCtxBankConfigEntry_t    ctxBankConfigEntry[SMMU_NUM_MAX_CTX_BANKS];
} SMMUDeviceConfigEntry_t;

typedef struct SMMUConfig_t {
  uint32                     numDevices;
  SMMUDeviceConfigEntry_t   *SMMUDeviceConfig;
} SMMUConfig_t;

#endif /*SMMUCONFIG_H*/
