/*=============================================================================

                       S M M U  D A L  D R I V E R 

GENERAL DESCRIPTION
  This file contains the configuration for the various SMMU instances
  for multimedia hardware blocks in trustzone.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
===============================================================================
Copyright (c) 2010 - 2014
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/smmu/smmu500/8916/tz/SMMUTrustZone.c#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/20/14   sc      Fixed KW issue
02/25/14   sc      Updated with SMMU clock on/off
01/14/14   sc      Updated for S2
12/02/13   sc      Cleaned up code a bit
11/08/13   sc      Updated for 8916
09/04/13   sc      Remove tzbsp_register_isr call
10/09/10   kcr     Initial Version
=============================================================================*/
#include "SMMUTypes.h"
#include "HALSMMU.h"
#include "SMMUConfig.h"
#include "SMMUTranslation.h"
#include "SMMUInternal.h"
#include "SMMUClock.h"
#include "mmu_lpae.h"
#include <stringl/stringl.h>
#include "hyp_target.h"
#include "tzbsp_isr.h"
#include "tzbsp_log.h"
#include "tzbsp.h"

/* ----------------------------------------------------------------------------
** Definitions
** ------------------------------------------------------------------------- */
// fault descriptor
#define SMMU_APPS_SEC_GLOBAL_FAULT_DESC "APPS"
#define SMMU_APPS_NONSEC_GLOBAL_FAULT_DESC "APPS NS"
#define SMMU_GPU_SEC_GLOBAL_FAULT_DESC "GPU"
#define SMMU_GPU_NONSEC_GLOBAL_FAULT_DESC "GPU NS"
#define SMMU_APPS_CB_FAULT_DESC "APPS CB"
#define SMMU_GPU_CB0_FAULT_DESC "GPU CB0"

// fault interrupt numbers
#define SMMU_APPS_SEC_GLOBAL_FAULT_INTR_NUM     70
#define SMMU_APPS_NONSEC_GLOBAL_FAULT_INTR_NUM  73
#define SMMU_GPU_SEC_GLOBAL_FAULT_INTR_NUM      74
#define SMMU_GPU_NONSEC_GLOBAL_FAULT_INTR_NUM   75
#define SMMU_APPS_CB_FAULT_INTR_NUM             105
#define SMMU_GPU_CB0_FAULT_INTR_NUM             272

// max number of global isr context
#define SMMU_GLOBAL_MAX_ISR_CTX 4

// max number of CB isr context
#define SMMU_CB_MAX_ISR_CTX 2

// bit masks for SMR ID field
#define SMMU_SMR_ID_MASK 0x7FFF
#define SMMU_SMR_MASK_MASK 0x7FFF
#define SMMU_TBUID_SHFT 10

/* ----------------------------------------------------------------------------
** External Functions
** ------------------------------------------------------------------------- */
extern void SMMU_CleanupHWRegisters
  (SMMUDeviceId_t smmuDeviceID);

extern SMMUResult_t SMMU_ConfigureCBTranslation
  (SMMUDeviceId_t smmuDeviceID, uint32 ctxBankNumber,
   uint32 *L1PageTableBase, boolean enable);

extern SMMUResult_t SMMU_Unified_ConfigureCBTranslation
  (SMMUDeviceId_t smmuDeviceID,
   uint32 ctxBankNumber, boolean enable);

extern SMMUTBUSSInfo_t smmuTBUSSInfo[];
uint32 smmu_num_max_devices = SMMU_NUM_MAX_DEVICES;
/*-----------------------------------------------------------------------------
 * Type Declarations
 * --------------------------------------------------------------------------*/
// SMMU SS SID2VMID SMR registers for different SS have different layout
typedef struct
{
  uint8 uSIDMaskShft;
  uint8 uVMIDShft;
  uint8 uValidShft;
} SMMU_SID2VmidSMR_ShftType;

/* ----------------------------------------------------------------------------
** Data
** ------------------------------------------------------------------------- */
extern smmu_fault_err_fatal_t smmu_fault_is_err_fatal;

// for global ISR
static uint32 uGlbCtxIdx = 0;
static uint32 aGlbCtx[SMMU_GLOBAL_MAX_ISR_CTX] = {0};

// for CB ISR
static uint32 uCBCtxIdx = 0;
static uint32 aCBCtx[SMMU_CB_MAX_ISR_CTX] = {0};

/*-----------------------------------------------------------------------------
 * Helper Functions
 * --------------------------------------------------------------------------*/

// TODO: move isr handler to unpaged
//extern void *SMMU_TZ_GlobalInterruptHandler(void *pContext);
void *SMMU_TZ_GlobalInterruptHandler(void *pContext)
{
  SMMUResult_t rc = SMMU_ERR_OK;
  uint32 devID = ((*(uint32*)pContext) >> 16);
  uint32 ns = (*(uint32*)pContext) & 0xFFFF;
  SMMUDeviceConfigEntry_t *pSMMUDeviceConfigEntry;
  SMMUAddr_t baseVirtAddr = 0;
  global_fault_regs_dump sgFaultDump = {0};
  SMMU_HAL_GFSR_t gfsr;
  SMMU_HAL_NSGFSR_t nsgfsr;

  // sanity check devID and ns
  if (devID >= SMMU_NUM_MAX_DEVICES)
  {
    TZBSP_LOG_ERR("SMMU GLB ISR: Invalid devID");
    return pContext;
  }
  if (ns != 0 && ns != 1)
  {
    TZBSP_LOG_ERR("SMMU GLB ISR: Invalid NS");
    return pContext;
  }

  // enable SMMU clk(s)
  rc = SMMU_ClockEnable((SMMU_ClkTCUVoteType)devID);
  if (rc != SMMU_ERR_OK)
  {
    TZBSP_LOG_ERR("SMMU GLB ISR: Clk Enable error");
    return pContext;
  }

  pSMMUDeviceConfigEntry = &smmuConfigInfo->SMMUDeviceConfig[devID];

  baseVirtAddr = pSMMUDeviceConfigEntry->baseVirtAddr;

  // global secure?
  if (ns == 0)
  {
    // check global space 0 secure address space fault registers
    SMMUHAL_GetGFSR(baseVirtAddr, &(sgFaultDump.gfsr));
    if (sgFaultDump.gfsr.raw != 0)
    {
      // get GFAR0/1, GFSYNR0/1/2
      SMMUHAL_GetGFAR0(baseVirtAddr, &(sgFaultDump.gfar0));
      SMMUHAL_GetGFAR1(baseVirtAddr, &(sgFaultDump.gfar1));
      SMMUHAL_GetGFSYNR0(baseVirtAddr, &(sgFaultDump.gfsynr0));
      SMMUHAL_GetGFSYNR1(baseVirtAddr, &(sgFaultDump.gfsynr1));
      SMMUHAL_GetGFSYNR2(baseVirtAddr, &(sgFaultDump.gfsynr2));

      // log the fault info
      if (devID == (uint32)SMMU_APPS)
      {
        tzbsp_log(TZBSP_MSG_DEBUG, "SMMU:>> APP Sec Global Fault:");
      }
      else
      {
        tzbsp_log(TZBSP_MSG_DEBUG, "SMMU:>> GPU Sec Global Fault:");
      }
      tzbsp_log(TZBSP_MSG_DEBUG, " sGFSR=0x%08x", sgFaultDump.gfsr.raw);
      tzbsp_log(TZBSP_MSG_DEBUG, " sGFAR0=0x%08x", sgFaultDump.gfar0);
      tzbsp_log(TZBSP_MSG_DEBUG, " sGFAR1=0x%08x", sgFaultDump.gfar1);
      tzbsp_log(TZBSP_MSG_DEBUG, " sGFSYNR0=0x%08x", sgFaultDump.gfsynr0);
      tzbsp_log(TZBSP_MSG_DEBUG, " sGFSYNR1=0x%08x", sgFaultDump.gfsynr1);
      tzbsp_log(TZBSP_MSG_DEBUG, " sGFSYNR2=0x%08x", sgFaultDump.gfsynr2);

      // a flag to enable/disable ERR_FATAL
      if (smmu_fault_is_err_fatal == SMMU_FAULT_ERR_FATAL_ENABLE)
      {
        // never returns and will restart the device
        tzbsp_err_fatal(TZBSP_ERR_FATAL_SMMU_FAULT);
      }
      else
      {
        // clear global secure fault
        gfsr.raw = 0;
        SMMUHAL_SetGFSRRestore(baseVirtAddr, gfsr);
        // clear GFAR0/1, GFSYNR0/1/2
        (void)SMMUHAL_SetGFAR0(baseVirtAddr, 0);
        (void)SMMUHAL_SetGFAR1(baseVirtAddr, 0);
        (void)SMMUHAL_SetGFSYNR0(baseVirtAddr, 0);
        (void)SMMUHAL_SetGFSYNR1(baseVirtAddr, 0);
        (void)SMMUHAL_SetGFSYNR2(baseVirtAddr, 0);
      }
    }
  }
  else // global non-secure
  {
    // check global space 0 non-secure address space fault registers
    SMMUHAL_GetNSGFSR(baseVirtAddr, &(sgFaultDump.nsgfsr));
    if (sgFaultDump.nsgfsr.raw != 0)
    {
      // get NSGFAR0/1, NSGFSYNR0/1/2
      SMMUHAL_GetNSGFAR0(baseVirtAddr, &(sgFaultDump.nsgfar0));
      SMMUHAL_GetNSGFAR1(baseVirtAddr, &(sgFaultDump.nsgfar1));
      SMMUHAL_GetNSGFSYNR0(baseVirtAddr, &(sgFaultDump.nsgfsynr0));
      SMMUHAL_GetNSGFSYNR1(baseVirtAddr, &(sgFaultDump.nsgfsynr1));
      SMMUHAL_GetNSGFSYNR2(baseVirtAddr, &(sgFaultDump.nsgfsynr2));

      // log the fault info
      if (devID == (uint32)SMMU_APPS)
      {
        tzbsp_log(TZBSP_MSG_DEBUG, "SMMU:>> APP NonSec Global Fault:");
      }
      else
      {
        tzbsp_log(TZBSP_MSG_DEBUG, "SMMU:>> GPU NonSec Global Fault:");
      }
      tzbsp_log(TZBSP_MSG_DEBUG, " NSGFSR=0x%08x", sgFaultDump.nsgfsr.raw);
      tzbsp_log(TZBSP_MSG_DEBUG, " NSGFAR0=0x%08x", sgFaultDump.nsgfar0);
      tzbsp_log(TZBSP_MSG_DEBUG, " NSGFAR1=0x%08x", sgFaultDump.nsgfar1);
      tzbsp_log(TZBSP_MSG_DEBUG, " NSGFSYNR0=0x%08x", sgFaultDump.nsgfsynr0);
      tzbsp_log(TZBSP_MSG_DEBUG, " NSGFSYNR1=0x%08x", sgFaultDump.nsgfsynr1);
      tzbsp_log(TZBSP_MSG_DEBUG, " NSGFSYNR2=0x%08x", sgFaultDump.nsgfsynr2);

      // a flag to enable/disable ERR_FATAL
      if (smmu_fault_is_err_fatal == SMMU_FAULT_ERR_FATAL_ENABLE)
      {
        // never returns and will restart the device
        tzbsp_err_fatal(TZBSP_ERR_FATAL_SMMU_FAULT);
      }
      else
      {
        // clear global secure fault
        nsgfsr.raw = 0;
        SMMUHAL_SetNSGFSRRestore(baseVirtAddr, nsgfsr);
        // clear GFAR0/1, GFSYNR0/1/2
        (void)SMMUHAL_SetNSGFAR0(baseVirtAddr, 0);
        (void)SMMUHAL_SetNSGFAR1(baseVirtAddr, 0);
        (void)SMMUHAL_SetNSGFSYNR0(baseVirtAddr, 0);
        (void)SMMUHAL_SetNSGFSYNR1(baseVirtAddr, 0);
        (void)SMMUHAL_SetNSGFSYNR2(baseVirtAddr, 0);
      }
    }
  }

  // disable SMMU clk(s)
  rc = SMMU_ClockDisable((SMMU_ClkTCUVoteType)devID);
  if (rc != SMMU_ERR_OK)
  {
    TZBSP_LOG_ERR("SMMU GLB ISR: Clk Enable error");
    return pContext;
  }

  return pContext;
} // SMMU_TZ_GlobalInterruptHandler

void SMMU_LogCBFault
(
  SMMUDeviceId_t devID,
  uint32 uCBn
)
{
  SMMUDeviceConfigEntry_t *pSMMUDeviceConfigEntry;
  SMMUCtxBankConfigEntry_t *pSMMUCtxBankConfigEntry;
  SMMUAddr_t baseVirtAddr = 0;
  SMMUCtxBankType eCBType;
  fault_regs_dump sCBFaultDump = {0};
  SMMU_HAL_CBn_FSR_t fsr;
  // TODO: temp until ipafar field add to the fault_regs_dump struct
  uint64 ipafar = 0;
  uint32 ipafar0 = 0;
  uint32 ipafar1 = 0;

  pSMMUDeviceConfigEntry = &smmuConfigInfo->SMMUDeviceConfig[devID];
  baseVirtAddr = pSMMUDeviceConfigEntry->baseVirtAddr;
  pSMMUCtxBankConfigEntry = &pSMMUDeviceConfigEntry->ctxBankConfigEntry[uCBn];
  eCBType = pSMMUCtxBankConfigEntry->eCBType;

  SMMUHAL_GetFSR(baseVirtAddr, uCBn, &(sCBFaultDump.fsr));
  // need to skip S2_CTX format fault
  if ((eCBType != SMMU_CB_S2_CTX && sCBFaultDump.fsr.raw !=0) ||
      (eCBType == SMMU_CB_S2_CTX && sCBFaultDump.fsr.raw != SMMU_CB_FSR_FORMAT_SCHEME))
  {
    // get FAR0/1, FSYN0/1
    SMMUHAL_GetFAR0(baseVirtAddr, uCBn, &(sCBFaultDump.far0));
    SMMUHAL_GetFAR1(baseVirtAddr, uCBn, &(sCBFaultDump.far1));

    SMMUHAL_GetFSYN0(baseVirtAddr, uCBn, &(sCBFaultDump.fsyn0));
    SMMUHAL_GetFSYN1(baseVirtAddr, uCBn, &(sCBFaultDump.fsyn1));

    // get IPAFAR
    //SMMUHAL_GetIPAFAR(baseVirtAddr, uCBn, &(sCBFaultDump.ipafar));
    SMMUHAL_GetIPAFAR(baseVirtAddr, uCBn, &ipafar);
    ipafar0 = (uint32)ipafar;
    ipafar1 = ipafar >> 32;

    // get CBFRSYNRA from global space 1
    SMMUHAL_GetCBFRSYNRAn(baseVirtAddr, uCBn, &(sCBFaultDump.cbfrsynra));

    // log CB fault
    if (devID == (uint32)SMMU_APPS)
    {
      tzbsp_log(TZBSP_MSG_DEBUG, "SMMU:>> APP CB%d Fault:", uCBn);
    }
    else
    {
      tzbsp_log(TZBSP_MSG_DEBUG, "SMMU:>> GPU CB%d Fault:", uCBn);
    }
    tzbsp_log(TZBSP_MSG_DEBUG, " FSR=0x%08x", sCBFaultDump.fsr.raw);
    tzbsp_log(TZBSP_MSG_DEBUG, " FAR0=0x%08x", sCBFaultDump.far0);
    tzbsp_log(TZBSP_MSG_DEBUG, " FAR1=0x%08x", sCBFaultDump.far1);
    //tzbsp_log(TZBSP_MSG_DEBUG, " IPAFAR=0x%016llx", sCBFaultDump.ipafar);
    //tzbsp_log(TZBSP_MSG_DEBUG, " IPAFAR=0x%016llx", ipafar);
    tzbsp_log(TZBSP_MSG_DEBUG, " IPAFAR=0x%08x%08x", ipafar1, ipafar0);
    tzbsp_log(TZBSP_MSG_DEBUG, " FSYNR0=0x%08x", sCBFaultDump.fsyn0);
    tzbsp_log(TZBSP_MSG_DEBUG, " FSYNR1=0x%08x", sCBFaultDump.fsyn1);
    tzbsp_log(TZBSP_MSG_DEBUG, " CBFRSYNRA%d=0x%08x", uCBn, sCBFaultDump.cbfrsynra);

    // a flag to enable/disable ERR_FATAL
    // just to be sure no scheme
    if (((eCBType != SMMU_CB_S2_CTX && sCBFaultDump.fsr.raw !=0) ||
        (eCBType == SMMU_CB_S2_CTX && sCBFaultDump.fsr.raw != SMMU_CB_FSR_FORMAT_SCHEME)) &&
        (smmu_fault_is_err_fatal == SMMU_FAULT_ERR_FATAL_ENABLE))
    {
      // never returns and will restart the device
      tzbsp_err_fatal(TZBSP_ERR_FATAL_SMMU_FAULT);
    }
    else
    {
      // clear CB fault
      fsr.raw = 0;
      SMMUHAL_SetFSRRestore(baseVirtAddr, uCBn, fsr);
      // clear CBFRSYNRA, FAR0/1, FSYN0/1, IPAFAR
      SMMUHAL_SetCBFRSYNRAn(baseVirtAddr, uCBn, 0);
      SMMUHAL_SetFAR0(baseVirtAddr, uCBn, 0);
      SMMUHAL_SetFAR1(baseVirtAddr, uCBn, 0);
      SMMUHAL_SetFSYN0(baseVirtAddr, uCBn, 0);
      SMMUHAL_SetFSYN1(baseVirtAddr, uCBn, 0);
      SMMUHAL_SetIPAFAR(baseVirtAddr, uCBn, 0);
    }
  }
} // SMMU_LogCBFault

void *SMMU_TZ_CBInterruptHandler(void *pContext)
{
  SMMUResult_t rc = SMMU_ERR_OK;
  SMMUDeviceId_t devID = (SMMUDeviceId_t)((*(uint32*)pContext) >> 16);
  uint32 uCBn = (*(uint32*)pContext) & 0xFFFF;
  uint32 i = 0;
  SMMUDeviceConfigEntry_t *pSMMUDeviceConfigEntry = &smmuConfigInfo->SMMUDeviceConfig[devID];
  SMMUCtxBankConfigEntry_t *pSMMUCtxBankConfigEntry;

  // enable SMMU clk(s)
  rc = SMMU_ClockEnable((SMMU_ClkTCUVoteType)devID);
  if (rc != SMMU_ERR_OK)
  {
    TZBSP_LOG_ERR("SMMU CB ISR: Clk Enable error");
    return pContext;
  }

  if (devID == SMMU_APPS)
  {
    for (i = 0; i < SMMU_NUM_MAX_CTX_BANKS; i++)
    {
      pSMMUCtxBankConfigEntry = &pSMMUDeviceConfigEntry->ctxBankConfigEntry[i];

      if (!pSMMUCtxBankConfigEntry->Valid)
      {
        continue;
      }

      // only serviced TZ owned CBs -- S2, WCNSS, RPM and Secure CBs
      if ((pSMMUCtxBankConfigEntry->eCBType == SMMU_CB_S2_CTX) ||
          (pSMMUCtxBankConfigEntry->clientVMID == SMMU_WCNSS_VMID) ||
          (pSMMUCtxBankConfigEntry->clientVMID == SMMU_RPM_VMID) ||
          (pSMMUCtxBankConfigEntry->clientVMID == SMMU_SROT_VMID))
      {
        SMMU_LogCBFault(devID, i);
      }
    }
  }
  else
  {
    SMMU_LogCBFault(devID, uCBn);
  }

  // disable SMMU clk(s)
  rc = SMMU_ClockDisable((SMMU_ClkTCUVoteType)devID);
  if (rc != SMMU_ERR_OK)
  {
    TZBSP_LOG_ERR("SMMU CB ISR: Clk Disable error");
    return pContext;
  }

  return pContext;
} // SMMU_TZ_CBInterruptHandler

static int SMMU_RegisterGlobalISRs(SMMUDeviceId_t dev)
{
  int isr_rc = 0;

  if (uGlbCtxIdx >= SMMU_GLOBAL_MAX_ISR_CTX)
  {
    return -1;
  }

  if (dev == SMMU_APPS)
  {
    // context = DEV[31:16] | NS[15:0]
    aGlbCtx[uGlbCtxIdx] = ((uint32)dev << 16) | 0;
    isr_rc |= tzbsp_register_isr(SMMU_APPS_SEC_GLOBAL_FAULT_INTR_NUM,
                                 SMMU_APPS_SEC_GLOBAL_FAULT_DESC,
                                 SMMU_TZ_GlobalInterruptHandler, &aGlbCtx[uGlbCtxIdx],
                                 TZBSP_INTF_TRIGGER_LEVEL, TRUE);

    // context = DEV[31:16] | NS[15:0]
    uGlbCtxIdx++;
    if (uGlbCtxIdx >= SMMU_GLOBAL_MAX_ISR_CTX)
    {
      return -1;
    }
    aGlbCtx[uGlbCtxIdx] = ((uint32)dev << 16) | 1;
    isr_rc |= tzbsp_register_isr(SMMU_APPS_NONSEC_GLOBAL_FAULT_INTR_NUM,
                                 SMMU_APPS_NONSEC_GLOBAL_FAULT_DESC,
                                 SMMU_TZ_GlobalInterruptHandler, &aGlbCtx[uGlbCtxIdx],
                                 TZBSP_INTF_TRIGGER_LEVEL, TRUE);
    uGlbCtxIdx++;
  }
  else
  {
    // SMMU_GPU
    // context = DEV[31:16] | NS[15:0]
    aGlbCtx[uGlbCtxIdx] = ((uint32)dev << 16) | 0;
    isr_rc |= tzbsp_register_isr(SMMU_GPU_SEC_GLOBAL_FAULT_INTR_NUM,
                                 SMMU_GPU_SEC_GLOBAL_FAULT_DESC,
                                 SMMU_TZ_GlobalInterruptHandler, &aGlbCtx[uGlbCtxIdx],
                                 TZBSP_INTF_TRIGGER_LEVEL, TRUE);

    // context = DEV[31:16] | NS[15:0]
    uGlbCtxIdx++;
    if (uGlbCtxIdx >= SMMU_GLOBAL_MAX_ISR_CTX)
    {
      return -1;
    }
    aGlbCtx[uGlbCtxIdx] = ((uint32)dev << 16) | 1;
    isr_rc |= tzbsp_register_isr(SMMU_GPU_NONSEC_GLOBAL_FAULT_INTR_NUM,
                                 SMMU_GPU_NONSEC_GLOBAL_FAULT_DESC,
                                 SMMU_TZ_GlobalInterruptHandler, &aGlbCtx[uGlbCtxIdx],
                                 TZBSP_INTF_TRIGGER_LEVEL, TRUE);
    uGlbCtxIdx++;
  }

  return isr_rc;
} // SMMU_RegisterGlobalISRs

static int SMMU_RegisterCBISRs(SMMUDeviceId_t dev)
{
  SMMUDeviceConfigEntry_t *pSMMUDeviceConfigEntry = &smmuConfigInfo->SMMUDeviceConfig[dev];
  SMMUCtxBankConfigEntry_t *pSMMUCtxBankConfigEntry;
  int isr_rc = 0;
  uint32 i = 0;
  uint32 intr = 0;

  if (uCBCtxIdx >= SMMU_CB_MAX_ISR_CTX)
  {
    return -1;
  }

  if (dev == SMMU_APPS)
  {
    // loop through all CBs
    for (i = 0; i < SMMU_NUM_MAX_CTX_BANKS; i++)
    {
      pSMMUCtxBankConfigEntry = &pSMMUDeviceConfigEntry->ctxBankConfigEntry[i];

      if (!pSMMUCtxBankConfigEntry->Valid)
      {
        continue;
      }

      // only steer the TZ owned CBs -- S2, WCNSS, RPM and Secure CBs
      if ((pSMMUCtxBankConfigEntry->eCBType == SMMU_CB_S2_CTX) ||
          (pSMMUCtxBankConfigEntry->clientVMID == SMMU_WCNSS_VMID) ||
          (pSMMUCtxBankConfigEntry->clientVMID == SMMU_RPM_VMID) ||
          (pSMMUCtxBankConfigEntry->clientVMID == SMMU_SROT_VMID))
      {
        // config interrupt index for APP TCU
        SMMUHAL_GetIntrSelSec(SMMU_HAL_SS_LOCAL, &intr);
        intr |= (1 << i);
        SMMUHAL_SetIntrSelSec(SMMU_HAL_SS_LOCAL, intr);
      }
    }

    // context = DEV[31:16] | CBn[15:0]
    aCBCtx[uCBCtxIdx] = ((uint32)SMMU_APPS << 16) | 0/*aggregated*/;
    isr_rc = tzbsp_register_isr(SMMU_APPS_CB_FAULT_INTR_NUM,
                                SMMU_APPS_CB_FAULT_DESC,
                                SMMU_TZ_CBInterruptHandler, &aCBCtx[uCBCtxIdx],
                                TZBSP_INTF_TRIGGER_LEVEL, TRUE);
    uCBCtxIdx++;
  }
  else // SMMU_GPU
  {
    // GPU CB0 is S2
    // context = DEV[31:16] | CBn[15:0]
    aCBCtx[uCBCtxIdx] = ((uint32)SMMU_GPU << 16) | 0;
    isr_rc = tzbsp_register_isr(SMMU_GPU_CB0_FAULT_INTR_NUM,
                                SMMU_GPU_CB0_FAULT_DESC,
                                SMMU_TZ_CBInterruptHandler, &aCBCtx[uCBCtxIdx],
                                TZBSP_INTF_TRIGGER_LEVEL, TRUE);
    uCBCtxIdx++;
  }

  return isr_rc;
} // SMMU_RegisterCBISRs

static int SMMU_RegisterISRs(SMMUDeviceId_t dev)
{
  int isr_rc = 0;

  isr_rc |= SMMU_RegisterGlobalISRs(dev);
  isr_rc |= SMMU_RegisterCBISRs(dev);

  return isr_rc;
} // SMMU_RegisterISRs

void SMMU_ConfigAdditionalVenusCbs(void)
{
  /*Nothing to do for 8916. Remove it later*/
}

void SMMU_ConfigStreamToContext
(
  SMMUDeviceId_t smmuDeviceID,
  uint32 entry,
  uint32 s2crFieldInfo,
  uint32 sidType // to config ctx or bypass SIDs
)
{
  SMMU_HAL_S2CRn_t s2cr;
  SMMUAddr_t baseVirtAddr;
  SMMUDeviceConfigEntry_t *pSMMUDeviceConfigEntry;

  pSMMUDeviceConfigEntry = &smmuConfigInfo->SMMUDeviceConfig[smmuDeviceID];
  baseVirtAddr = pSMMUDeviceConfigEntry->baseVirtAddr;

  SMMUHAL_GetS2CRn(baseVirtAddr, entry, &s2cr);
  if (sidType == SMMU_CONFIG_BYPASS_ATTR_CTRL_SIDS)
  {
    s2cr.formatted.TYPE = 1; // bypass mode
    s2cr.formatted.SHCFG = 3; // non-shareable
  }
  else
  {
    s2cr.formatted.TYPE = 0x0; // Translation Context
    s2cr.formatted.TRANSIENTCFG = 0x3;
  }
  s2cr.formatted.FIELD = s2crFieldInfo;
  SMMUHAL_SetS2CRn(baseVirtAddr, entry, s2cr);
}

static SMMUResult_t SMMU_GetSSRegShftInfo
(
  uint32 uSSOffset,
  SMMU_SID2VmidSMR_ShftType *sSSRegShftInfo
)
{
  if (sSSRegShftInfo == NULL)
  {
    return SMMU_ERR_GENERIC_ERROR;
  }

  // different TBU SS has different layout for the SID2VMID_SMRn
  switch (uSSOffset)
  {
  case SMMU_HAL_SID2VMID_SMR0_JPEG:
  case SMMU_HAL_SID2VMID_SMR0_MDP:
  case SMMU_HAL_SID2VMID_SMR0_DIME:
  case SMMU_HAL_SID2VMID_SMR0_GPS:
    {
      // layout of the SMMU_SID2VMID_*_SMR_n register
      // SID_MATCH : 1
      // SID_MASK : 1
      // VMID : 4
      // VALID : 1
      // reserved : 25
      sSSRegShftInfo->uSIDMaskShft = 1;
      sSSRegShftInfo->uVMIDShft = 2;
      sSSRegShftInfo->uValidShft = 6;
    }
    break;
  case SMMU_HAL_SID2VMID_SMR0_VFE:
    {
      // layout of the SMMU_SID2VMID_*_SMR_n register
      // SID_MATCH : 2
      // SID_MASK : 2
      // VMID : 4
      // VALID : 1
      // reserved : 23
      sSSRegShftInfo->uSIDMaskShft = 2;
      sSSRegShftInfo->uVMIDShft = 4;
      sSSRegShftInfo->uValidShft = 8;
    }
    break;
  case SMMU_HAL_SID2VMID_SMR0_VENUS:
    {
      // layout of the SMMU_SID2VMID_*_SMR_n register
      // SID_MATCH : 8
      // SID_MASK : 8
      // VMID : 4
      // VALID : 1
      // reserved : 11
      sSSRegShftInfo->uSIDMaskShft = 8;
      sSSRegShftInfo->uVMIDShft = 16;
      sSSRegShftInfo->uValidShft = 20;
    }
    break;
  case SMMU_HAL_SID2VMID_SMR0_PRONTO:
    {
      // layout of the SMMU_SID2VMID_*_SMR_n register
      // SID_MATCH : 5
      // SID_MASK : 5
      // VMID : 4
      // VALID : 1
      // reserved : 17
      sSSRegShftInfo->uSIDMaskShft = 5;
      sSSRegShftInfo->uVMIDShft = 10;
      sSSRegShftInfo->uValidShft = 14;
    }
    break;
  case SMMU_HAL_SID2VMID_SMR0_PERIPH:
    {
      // layout of the SMMU_SID2VMID_*_SMR_n register
      // SID_MATCH : 10
      // SID_MASK : 10
      // VMID : 4
      // VALID : 1
      // reserved : 7
      sSSRegShftInfo->uSIDMaskShft = 10;
      sSSRegShftInfo->uVMIDShft = 20;
      sSSRegShftInfo->uValidShft = 24;
    }
    break;
  case SMMU_HAL_SID2VMID_SMR0_QDSP:
  case SMMU_HAL_SID2VMID_SMR0_GFX:
    {
      // layout of the SMMU_SID2VMID_*_SMR_n register
      // SID_MATCH : 1
      // SID_MASK : 1
      // VMID : 5
      // VALID : 1
      // reserved : 24
      sSSRegShftInfo->uSIDMaskShft = 1;
      sSSRegShftInfo->uVMIDShft = 2;
      sSSRegShftInfo->uValidShft = 7;
    }
    break;
  default:
    // invalid SS base addr
    return SMMU_ERR_GENERIC_ERROR;
  }

  return SMMU_ERR_OK;
} // SMMU_GetSSRegShftInfo

SMMUResult_t SMMU_LookupTBUSSInfo
(
  SMMUDeviceId_t smmuDeviceID,
  uint32 uTBUID,
  SMMUTBUSSInfo_t **sTBUSSInfo
)
{
  // TBU ID for GFX needs to be twisted to get the TBU SS info
  if (smmuDeviceID == SMMU_GPU)
  {
    uTBUID += SMMU_NUM_APPS_TBUS;
  }

  if (uTBUID >= SMMU_MAX_TBU_IDS)
  {
    return SMMU_ERR_GENERIC_ERROR;
  }

  *sTBUSSInfo = &smmuTBUSSInfo[uTBUID];

  return SMMU_ERR_OK;
} // SMMU_LookupTBUSSInfo

SMMUResult_t SMMU_GetSID2VmidSMRnAvailEntry
(
  SMMUAddr_t baseSSVirtAddr,
  uint32 maxEntries,
  uint32 *uAvailEntry
)
{
  uint32 i = 0;
  uint32 uSSOffset = 0;
  SMMU_SID2VmidSMR_ShftType sSSRegShftInfo = {0};
  uint32 uSSVal = 0;
  uint32 uStart = 0;

  if (uAvailEntry == NULL)
  {
    return SMMU_ERR_GENERIC_ERROR;
  }

  uSSOffset = baseSSVirtAddr - SMMU_HAL_SS_LOCAL;

  if (SMMU_GetSSRegShftInfo(uSSOffset, &sSSRegShftInfo) != SMMU_ERR_OK)
  {
    return SMMU_ERR_GENERIC_ERROR;
  }

  uStart = *uAvailEntry;

  for (i = uStart; i < maxEntries; i++)
  {
    SMMUHAL_GetSID2VMID_SMRn(baseSSVirtAddr, i, &uSSVal);
    if (((uSSVal >> sSSRegShftInfo.uValidShft) & 1) == 0)
    {
      *uAvailEntry = i;
      break;
    }
  }

  if (i >= maxEntries)
  {
    return SMMU_ERR_GENERIC_ERROR;
  }

  return SMMU_ERR_OK;
} // SMMU_GetSID2VmidSMRnAvailEntry

SMMUResult_t SMMU_SetSID2VmidSMRn
(
  SMMUAddr_t baseSSVirtAddr,
  uint32 uEntry,
  uint32 uVmid,
  uint32 uSMRVal
)
{
  uint32 uSSOffset = 0;
  SMMU_SID2VmidSMR_ShftType sSSRegShftInfo = {0};
  uint32 uSSVal = 0;

  uSSOffset = baseSSVirtAddr - SMMU_HAL_SS_LOCAL;

  if (SMMU_GetSSRegShftInfo(uSSOffset, &sSSRegShftInfo) != SMMU_ERR_OK)
  {
    return SMMU_ERR_GENERIC_ERROR;
  }

  // form the register value
  uSSVal = 1 << sSSRegShftInfo.uValidShft;
  uSSVal |= (uVmid << sSSRegShftInfo.uVMIDShft);
  uSSVal |= (SMMU_GET_SMRMASK_FROM_SMRVAL(uSMRVal) << sSSRegShftInfo.uSIDMaskShft);
  uSSVal |= SMMU_GET_TBUSID_FROM_SMRVAL(uSMRVal);

  SMMUHAL_SetSID2VMID_SMRn(baseSSVirtAddr, uEntry, uSSVal);

  return SMMU_ERR_OK;
} // SMMU_SetSID2VmidSMRn

/*-----------------------------------------------------------------------------
 * Exported APIs
 * --------------------------------------------------------------------------*/
int SMMU_TZ_Init(void)
{
  uint32 i = 0;
  SMMUResult_t rc = SMMU_ERR_OK;
  int mmu_rc = 0;
  int isr_rc = 0;

  // init MMU
  mmu_rc = LPAE_Init((void*)SMMU_PT_POOL_MEMORY_ADDR, SMMU_PT_POOL_MEMORY_SIZE, 0);
  if (mmu_rc != LPAE_OK)
  {
    return mmu_rc;
  }
  // zero out SMMU L1 PT
  secure_memset((void*)SMMU_L1PT_MEMORY_ADDR, 0, SMMU_L1PT_MEMORY_SIZE);

  // enable SMMU clk(s)
  rc = SMMU_ClockEnable(SMMU_CLK_ALL_TCU);
  if (rc != SMMU_ERR_OK)
  {
    return (int)rc;
  }

  // program PTW VMID
  (void)SMMUHAL_SetAppTcuPtwVmidSec(SMMU_HAL_SS_LOCAL, SMMU_APCPU_VMID);
  (void)SMMUHAL_SetGfxTcuPtwVmidSec(SMMU_HAL_SS_LOCAL, SMMU_APCPU_VMID);

  // program GPU and APPS SMMU implicitly
  for (i = 0; i < smmuConfigInfo->numDevices; i++)
  {
    // clean HW regs before setting up ISR
    SMMU_CleanupHWRegisters((SMMUDeviceId_t)i);

    isr_rc = SMMU_RegisterISRs((SMMUDeviceId_t)i);
    if (isr_rc)
    {
      rc = (SMMUResult_t)isr_rc;
      break;
    }

    rc = SMMU_InitDevice((SMMUDeviceId_t)i, SMMU_INVALID_ADDR);
    if (rc != SMMU_ERR_OK)
    {
      break;
    }
  }

  // disable SMMU clk(s)
  rc |= SMMU_ClockDisable(SMMU_CLK_ALL_TCU);

  return (int)rc;
}

SMMUResult_t SMMU_ValidateInstanceInitialized(SMMUDeviceId_t smmuDeviceID)
{
  SMMUResult_t rc = SMMU_ERR_OK;

  if (!SMMU_DEVICEID_VALID(smmuDeviceID))
  {
    return SMMU_ERR_GENERIC_ERROR;
  }

  rc = SMMU_ClockEnable((SMMU_ClkTCUVoteType)smmuDeviceID);
  if (rc != SMMU_ERR_OK)
  {
    return rc;
  }
  rc = SMMU_ValidateDeviceInitialized(&smmuConfigInfo->SMMUDeviceConfig[smmuDeviceID]);
  rc |= SMMU_ClockDisable((SMMU_ClkTCUVoteType)smmuDeviceID);

  return rc;
}

SMMUResult_t SMMU_InstanceConfigureCBTranslation
(
  SMMUDeviceId_t smmuDeviceID,
  uint32 *L1PageTableBase,
  uint32 ctxBankNumber,
  boolean enable
)
{
  SMMUResult_t rc = SMMU_ERR_OK;

  if (!SMMU_DEVICEID_VALID(smmuDeviceID))
  {
    return SMMU_ERR_GENERIC_ERROR;
  }

  rc = SMMU_ClockEnable((SMMU_ClkTCUVoteType)smmuDeviceID);
  if (rc != SMMU_ERR_OK)
  {
    return rc;
  }
  rc = SMMU_ConfigureCBTranslation(smmuDeviceID, ctxBankNumber, L1PageTableBase, enable);
  rc |= SMMU_ClockDisable((SMMU_ClkTCUVoteType)smmuDeviceID);

  return rc;
}

SMMUResult_t SMMU_Unified_InstanceConfigureCBTranslation
(
  SMMUDeviceId_t smmuDeviceID,
  uint32 ctxBankNumber,
  boolean enable
)
{
  SMMUResult_t rc = SMMU_ERR_OK;

  if (!SMMU_DEVICE_CB_VALID(smmuDeviceID, ctxBankNumber))
  {
    return SMMU_ERR_GENERIC_ERROR;
  }

  rc = SMMU_ClockEnable((SMMU_ClkTCUVoteType)smmuDeviceID);
  if (rc != SMMU_ERR_OK)
  {
    return rc;
  }

  rc = SMMU_Unified_ConfigureCBTranslation(smmuDeviceID, ctxBankNumber, enable);
  rc |= SMMU_ClockDisable((SMMU_ClkTCUVoteType)smmuDeviceID);

  return rc;
}
