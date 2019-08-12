/*===========================================================================
   Copyright (c) 2010-2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_xpu.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/07/11   mm      Updated log string to include module information
09/28/10   tk      Initial version.
=============================================================================*/

/* This pragma allows anonymous unions that are used by XPU HAL API */
#pragma anon_unions

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <HALhwio.h>
#include <HALxpu2.h>
#include <string.h>

#include "tzbsp.h"
#include "tzbsp_config.h"
#include "tzbsp_errno.h"
#include "tzbsp_log.h"
#include "tzbsp_mem.h"
#include "tzbsp_sys.h"
#include "tzbsp_target.h"
#include "tzbsp_xpu.h"
#include "tzbsp_isr.h"
#include "tzbsp_xpu_def.h"
#include "tzbsp_hwio.h"
#include "tzbsp_xpu_config.h"
#include "tzbsp_vmid_config.h"
#include "tz_syscall_pub.h"
#include "tzbsp_memory_map.h"
#include "tzbsp_mmu_cache.h"
#include "SpmiCfg.h"
#include "tzbsp_smem.h"
#include "pm_access_cfg.h"
#include "SMMUTypes.h"
#include "tzbsp_nand.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/**
 * Convenience macros for RWGE/RWE/ROGE/ROE configuration bits.
 */
#define HAS_RWGE(config, ii) \
      ((TZBSP_RWGE & (config)->rg.rpu[(ii)].flags) ? TRUE: FALSE)
#define HAS_RWE(config, ii) \
      ((TZBSP_RWE & (config)->rg.rpu[(ii)].flags) ? TRUE: FALSE)
#define HAS_ROGE(config, ii) \
      ((TZBSP_ROGE & (config)->rg.rpu[(ii)].flags) ? TRUE: FALSE)
#define HAS_ROE(config, ii) \
      ((TZBSP_ROE & (config)->rg.rpu[(ii)].flags) ? TRUE: FALSE)
#define HAS_MSA_ROE(config, ii) \
      ((TZBSP_MSA_ROE & (config)->rg.rpu[(ii)].flags) ? TRUE: FALSE)
#define HAS_MSA_RWE(config, ii) \
      ((TZBSP_MSA_RWE & (config)->rg.rpu[(ii)].flags) ? TRUE: FALSE)

#define DUMP_XPU_ERR_FLAG(xx, bit) \
  if((xx) & (bit)) tzbsp_log(TZBSP_MSG_DEBUG, "xpu:  %s", #bit)

#define TZBSP_MAX_MASTERS TZBSP_MMSS_MASTER   ///< Highest numbered bus master

#define TZBSP_XPU_ERR_SEC_CTX               0
#define TZBSP_XPU_ERR_NON_SEC_CTX           1

#define TZBSP_XPU_ERR_INT_STATUS_REG_NUM    1
#define TZBSP_XPU_ERR_NUM_PER_REG           32

/* Maximum value for XPU error status bit position.
 * Error status register is 32 bit so position ranges
 * from 0 to 31
 */
#define TZBSP_XPU_ERR_BIT_POS_MAX   (sizeof(uint32)*8 - 1)

#define TZBSP_XPU_BIMC_MPU_DUMP_SIZE  0x1100
#define TZBSP_XPU_TOTAL_DUMP_SIZE     (2 * TZBSP_XPU_BIMC_MPU_DUMP_SIZE)

/*
 * Whether changing xpu violations behavior as err fatal is allowed via syscall
 */
#define TZBSP_XPU_CFG_VIOLATION_ERR_FATAL_NOTALLOWED 0
#define TZBSP_XPU_CFG_VIOLATION_ERR_FATAL_ALLOWED    1

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
/* Whether disabling xpu violations as err fatal is allowed via syscall */
int tzbsp_xpu_allow_disable_err_fatal = TZBSP_XPU_CFG_VIOLATION_ERR_FATAL_ALLOWED;
/* XPU violations result in error fatal or not */
tz_xpu_violation_err_fatal_t tzbsp_xpu_violation_is_err_fatal = TZ_XPU_VIOLATION_ERR_FATAL_DISABLE;

int xpu_err_sec_ctx = TZBSP_XPU_ERR_SEC_CTX;
int xpu_err_non_sec_ctx = TZBSP_XPU_ERR_NON_SEC_CTX;

/* Whether an xpu init has been done */
uint32 xpu_init_done[HAL_XPU2_COUNT] = {0};

/* Whether this is static configuration run for cold boot */
//static uint8 cold_boot_static_run = 1; not used; commented to remove compiler warning.

int tzbs_xpu_temp_debug = 0;

#ifndef FEATURE_NO_XPU
/**
 * Structure to map the xpu instances corresponding to a particular
 * device.
 */
typedef struct
{
  HAL_xpu2_XPU2Type *xpu;
  uint32            num_xpu;
} tzbsp_device_to_xpu_t;

/* xPUs for Venus */
static HAL_xpu2_XPU2Type tzbsp_venus_xpu_map[] =
                               {HAL_XPU2_MMSS_VENUS};
/* xPUs for Venus */
static HAL_xpu2_XPU2Type tzbsp_vpu_xpu_map[] =
                               {HAL_XPU2_MMSS_VPU_MAPLE, HAL_XPU2_MMSS_VPU_VBIF_APU};
/* xPUs for MDSS at runtime */
static HAL_xpu2_XPU2Type tzbsp_mdss_xpu_map[] =
                                  {HAL_XPU2_MMSS_MDSS_VBIF, HAL_XPU2_MDSS_APU};

/* xPUs for LPASS at runtime */
static HAL_xpu2_XPU2Type tzbsp_lpass_xpu_map[] = {HAL_XPU2_LPASS_COMMON,
                                                  HAL_XPU2_LPASS_CORE_CSR,
                                                  HAL_XPU2_LPASS_LPAIF,
                                                  HAL_XPU2_LPASS_RSMPLR,
                                                  HAL_XPU2_LPASS_LPM};

static HAL_xpu2_XPU2Type tzbsp_usb3_0_xpu_map[] = {HAL_XPU2_USB30};
static HAL_xpu2_XPU2Type tzbsp_usb3_1_xpu_map[] = {HAL_XPU2_USB_HS_SEC};
static HAL_xpu2_XPU2Type tzbsp_pcie_0_xpu_map[] = {HAL_XPU2_PCIE0_APU,
                                                   HAL_XPU2_PCIE0_MPU};
static HAL_xpu2_XPU2Type tzbsp_pcie_1_xpu_map[] = {HAL_XPU2_PCIE1_APU,
                                                   HAL_XPU2_PCIE1_MPU};

/*
 * xPUs for MDSS at boot
 * For MDSS_BOOT scenario, the SMMU global and CB space is kept locked away
 * from HLOS through QRIB of VBIF xPU.
 */
static HAL_xpu2_XPU2Type tzbsp_mdss_boot_xpu_map[] = {HAL_XPU2_MDSS_APU};

#define ARRAYSZ(xx) (sizeof(xx)/sizeof(xx[0]))

static const tzbsp_device_to_xpu_t
tzbsp_device_to_xpu_map[TZ_DEVICE_COUNT] =
{
  {tzbsp_venus_xpu_map, ARRAYSZ(tzbsp_venus_xpu_map)},
  {tzbsp_mdss_xpu_map, ARRAYSZ(tzbsp_mdss_xpu_map)},
  {tzbsp_lpass_xpu_map, ARRAYSZ(tzbsp_lpass_xpu_map)},
  {tzbsp_mdss_boot_xpu_map, ARRAYSZ(tzbsp_mdss_xpu_map)},
  {NULL,  0}, /* usb1 HS */
  {NULL,  0}, /* ocmem */
  {NULL,  0}, /* lpass core smmu */
  {tzbsp_vpu_xpu_map, ARRAYSZ(tzbsp_vpu_xpu_map)},
  {NULL,  0}, /* copss smmu */
  {tzbsp_usb3_0_xpu_map, ARRAYSZ(tzbsp_usb3_0_xpu_map)},
  {tzbsp_usb3_1_xpu_map, ARRAYSZ(tzbsp_usb3_1_xpu_map)},
  {tzbsp_pcie_0_xpu_map, ARRAYSZ(tzbsp_pcie_0_xpu_map)},
  {tzbsp_pcie_1_xpu_map, ARRAYSZ(tzbsp_pcie_1_xpu_map)},
};

#endif

/*
 * Structure to map a bit position in XPU error interrupt status register
 * to corresponding HAL XPU index
 */
typedef struct
{
  /* Bit position in status register */
  uint8 bit_pos;
  /* Corresponding HAL XPU index */
  uint8 xpu;
} xpu_err_pos_to_hal_map_t;

#if 0
static uint8 multi_xpu_errors_per_status_bit_lpass[] = {HAL_XPU2_LPASS_COMMON,
          HAL_XPU2_LPASS_CORE_CSR, HAL_XPU2_LPASS_LCC,
          HAL_XPU2_LPASS_LPAIF, HAL_XPU2_LPASS_LPM, HAL_XPU2_LPASS_QDSP6SS,
          HAL_XPU2_LPASS_RSMPLR, HAL_XPU2_LPASS_SLIMBUS, HAL_XPU2_LPASS_TOP};
#endif

/*
 * Mapping of XPU position in XPU Error interrupt status register
 * to corresponding HAL XPU index
 */
static xpu_err_pos_to_hal_map_t xpu_err_pos_to_hal_map[] =
{
    {0,  HAL_XPU2_MSS},
    {1,  HAL_XPU2_RPM_APU},
    {2,  HAL_XPU2_MMSS_VENUS},
    {3,  HAL_XPU2_CLK_CTL},
    {4,  HAL_XPU2_BIMC_APU},
    {5,  HAL_XPU2_BIMC_MPU0},
    {6,  HAL_XPU2_PMIC_ARB},
    {7,  HAL_XPU2_PRONTO},
    {8,  HAL_XPU2_QPIC_QPIC},
    {9,  HAL_XPU2_TLMM},
    {10, HAL_XPU2_QPIC_QPIC_MPU},
    {11, HAL_XPU2_TCSR_REGS},
    {12, HAL_XPU2_CRYPTO0_BAM},
    {13, HAL_XPU2_BOOT_ROM},
    {14, HAL_XPU2_MPM2},
    {15, HAL_XPU2_DEHR},
    {16, HAL_XPU2_SEC_CTRL_APU},
    {17, HAL_XPU2_IMEM_MPU},
    {18, HAL_XPU2_SNOC_CFG},
    {19, HAL_XPU2_PCNOC_CFG},
    {20, HAL_XPU2_PRNG_CFG},
    {21, HAL_XPU2_RPM_CFG},
    {22, HAL_XPU2_SMMU_500_DIME_MPU},
    {23, HAL_XPU2_SMMU_500_GPS_MPU},
    {24, HAL_XPU2_SMMU_500_QDSP_MPU},
    {25, HAL_XPU2_SMMU_500_TCU_MPU},
    {26, HAL_XPU2_BAM_BLSP1_DMA},
    {27, HAL_XPU2_COUNT},
    {28, HAL_XPU2_COUNT},
    {29, HAL_XPU2_COUNT},
    {30, HAL_XPU2_COUNT},
    {31, HAL_XPU2_COUNT},
};

/*----------------------------------------------------------------------------
 * Global Variable Definitions
 * -------------------------------------------------------------------------*/
/* The cumulative number of XPU errors. */
volatile uint32 g_tzbsp_xpu_err_count;

extern const tzbsp_xpu_cfg_t g_tzbsp_xpu_cfg[];
extern const uint32 g_tzbsp_xpu_cfg_count;
extern uint32 g_tzbsp_dump_xpu_syndrome;

/* SMMU faults result in error fatal or not */
extern smmu_fault_err_fatal_t smmu_fault_is_err_fatal;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

extern void tzbsp_enable_xpu_config_clocks(boolean enable);
extern void tzbsp_enable_xpu_config_clocks_boot(boolean enable);
extern void tzbsp_enable_xpu_isr_clocks(boolean enable, uint32 xpu_err_bitmask);
extern boolean tzbsp_is_nonsecure_device(void);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/* @copydoc tzbsp_vmids_to_perm */
uint32 tzbsp_vmids_to_perm(uint16 read_vmids, uint16 write_vmids)
{
  const uint32 READ = ((uint32)HAL_XPU2_RO_ACCESS) << 30;
  const uint32 WRITE = ((uint32)HAL_XPU2_WO_ACCESS) << 30;
  int i = 1;
  /* Used as a shift register. Access bits are added to the highest two bits
   * and as the register is rotated right, the lowest access bits will
   * eventually end up to the correct location. */
  uint32 ret = 0;

  /* Handle the zeroeth bit as a special case. */
  if(read_vmids & 1)
    ret |= READ;
  if(write_vmids & 1)
    ret |= WRITE;

  do
  {
    read_vmids >>= 1; /* Next read VMID as lowest bit. */
    write_vmids >>= 1; /* Next write VMID as lowest bit. */
    ret >>= 2; /* Shift previous access bits downwards. */

    if(read_vmids & 1)
      ret |= READ;
    if(write_vmids & 1)
      ret |= WRITE;

    i++;
  } while(i < 16);

  return ret;
}

/* Applies an MPU configuration to the actual HW block.
 *
 * @param [in] cfg  MPU configuration data.
 *
 * @param [in] info  Reflection information about the MPU HW.
 */
static int tzbsp_mpu_config(const tzbsp_xpu_cfg_t* cfg,
                             const HAL_xpu2_XPUDeviceParamsType* info)
{
  int i;
  HAL_xpu2_ResourceGroupConfigType perm = {0};

  /* Default configuration. */
  perm.resPerm.bMultiVMID = info->bMultiVMID;

  /* Only the resource groups in our configuration will be enabled. */
  for(i=0; i < cfg->nrg; i++)
  {
  #ifdef FEATURE_NO_XPU
    /* MBA/Modem expect xPUs to be set-up, so only program resource groups
     * marked for MSA use */
    if(!((cfg->rg.mpu[i].flags) & TZBSP_XPU_MODEM_PROT)) continue;
  #endif

    if(info->bMultiVMID)
    {
      perm.resPerm.Vmid_Type.multi.auVMID[0] = 0xffffffff;/*All VMIDs programmed*/
      /* Lower 16 vmids */
      perm.resPerm.Vmid_Type.multi.auVMIDPerm[0] =
        tzbsp_vmids_to_perm(cfg->rg.mpu[i].read_vmid,
                            cfg->rg.mpu[i].write_vmid);
      /* Upper 16 vmids */
      perm.resPerm.Vmid_Type.multi.auVMIDPerm[1] =
        tzbsp_vmids_to_perm((cfg->rg.mpu[i].read_vmid) >> 16,
                            (cfg->rg.mpu[i].write_vmid) >> 16);
    }
    else
    {
      perm.resPerm.Vmid_Type.single.uRWVMID = cfg->rg.mpu[i].write_vmid;
      perm.resPerm.Vmid_Type.single.uROVMID = cfg->rg.mpu[i].read_vmid;
      HAL_xpu2_GetResourceGroupConfig((HAL_xpu2_XPU2Type)cfg->id,
                                      cfg->rg.mpu[i].index, &perm);
      perm.resPerm.bMultiVMID = FALSE;
      perm.resPerm.Vmid_Type.single.bRWGlobalEn =
        (TZBSP_RWGE & cfg->rg.mpu[i].flags) ? TRUE: FALSE;
      perm.resPerm.Vmid_Type.single.bRWVMIDValid =
        (TZBSP_RWE & cfg->rg.mpu[i].flags) ? TRUE: FALSE;
      perm.resPerm.Vmid_Type.single.bROGlobalEn =
        (TZBSP_ROGE & cfg->rg.mpu[i].flags) ? TRUE: FALSE;
      perm.resPerm.Vmid_Type.single.bROVMIDValid =
        (TZBSP_ROE & cfg->rg.mpu[i].flags) ? TRUE: FALSE;

      perm.resPerm.Vmid_Type.single.uRWVMID = cfg->rg.mpu[i].write_vmid;
      perm.resPerm.Vmid_Type.single.uROVMID = cfg->rg.mpu[i].read_vmid;
    }

    if((cfg->rg.mpu[i].flags) & TZBSP_XPU_ENABLE)
    {
      perm.resAddr.uStartLower = cfg->rg.mpu[i].start;
      perm.resAddr.uEndLower = cfg->rg.mpu[i].end;
    }
    else
    {
      perm.resAddr.uStartLower = 0xFFFFFFFF;
      perm.resAddr.uEndLower = 0xFFFFFFFF;
      }

      perm.resAddr.uStartUpper = 0;
      perm.resAddr.uEndUpper = 0;

    if((cfg->rg.mpu[i].flags) & TZBSP_XPU_MODEM_PROT)
      {
        perm.eConfig = HAL_XPU2_CONFIG_MODEM;
      }
    else if((cfg->rg.mpu[i].flags) & TZBSP_XPU_SEC)
      {
        perm.eConfig = HAL_XPU2_CONFIG_SECURE;
        perm.clientPerm.bNonSecureClientROEn = (TZBSP_ROE & cfg->rg.mpu[i].flags) ? TRUE: FALSE;
        perm.clientPerm.bNonSecureClientRWEn = (TZBSP_RWE & cfg->rg.mpu[i].flags) ? TRUE: FALSE;
      }
      else
      {
        perm.eConfig = HAL_XPU2_CONFIG_NON_SECURE;
        perm.clientPerm.bNonSecureClientROEn = FALSE;
        perm.clientPerm.bNonSecureClientRWEn = FALSE;
      }

    if(HAL_XPU2_NO_ERROR != HAL_xpu2_ConfigResourceGroup(
                                                   (HAL_xpu2_XPU2Type)cfg->id,
                                                   cfg->rg.mpu[i].index, &perm))
      {
        return -E_FAILURE;
      }
    }
  return E_SUCCESS;
}

/* Applies an RPU/APU configuration to the actual HW block. Both RPU and APU
 * are handled similarly by this function.
 *
 * @param [in] cfg  RPU/APU configuration data.
 *
 * @param [in] info  Reflection information about the RPU/APU HW.
 */
static int tzbsp_rpu_config(const tzbsp_xpu_cfg_t* cfg,
                             const HAL_xpu2_XPUDeviceParamsType* info)
{
  int i;
  uint32 index = 0;
  HAL_xpu2_ResourceGroupConfigType     rg_cfg = {0};

    /* Apply default access configuration to all resource groups. */
    if(info->bMultiVMID)
    {
      rg_cfg.eConfig = HAL_XPU2_CONFIG_NON_SECURE;
      for(i=0; i < info->uResourceGroupCount; i++)
      {
      HAL_xpu2_GetResourceGroupConfig((HAL_xpu2_XPU2Type)cfg->id, i, &rg_cfg);
        /* Treating all as full permission MPUs. */
        rg_cfg.resPerm.bMultiVMID = TRUE;
        rg_cfg.resPerm.Vmid_Type.multi.auVMID[0] =
          rg_cfg.resPerm.Vmid_Type.multi.auVMIDPerm[0] =
          rg_cfg.resPerm.Vmid_Type.multi.auVMIDPerm[1] = 0xffffffff;

      if(HAL_XPU2_NO_ERROR != HAL_xpu2_ConfigResourceGroup(
                                        (HAL_xpu2_XPU2Type)cfg->id, i, &rg_cfg))
        {
          return -E_FAILURE;
        }
      }
    }
    else /* Single VMID */
    {
      for(i=0; i < info->uResourceGroupCount; i++)
      {
        rg_cfg.resPerm.Vmid_Type.single.uRWVMID = 0;
        rg_cfg.resPerm.Vmid_Type.single.uROVMID = 0;
      HAL_xpu2_GetResourceGroupConfig((HAL_xpu2_XPU2Type)cfg->id, i, &rg_cfg);
        rg_cfg.resPerm.bMultiVMID = FALSE;
        rg_cfg.resPerm.Vmid_Type.single.bRWGlobalEn = TRUE;
        rg_cfg.resPerm.Vmid_Type.single.bRWVMIDValid = 0;
        rg_cfg.resPerm.Vmid_Type.single.bROGlobalEn = TRUE;
        rg_cfg.resPerm.Vmid_Type.single.bROVMIDValid = 0;

      if(HAL_XPU2_NO_ERROR != HAL_xpu2_ConfigResourceGroup(
                                        (HAL_xpu2_XPU2Type)cfg->id, i, &rg_cfg))
        {
          return -E_FAILURE;
        }
      }
    }

  /* Only the resource groups in our configuration will be enabled. */
  if(info->bMultiVMID)
  {
    for(i=0; i < cfg->nrg; i++)
    {
    #ifdef FEATURE_NO_XPU
      /* MBA/Modem expect xPUs to be set-up, so only program resource groups
       * marked for MSA use */
      if(!(TZBSP_XPU_MODEM_PROT & cfg->rg.rpu[i].flags)) continue;
    #endif

      HAL_xpu2_GetResourceGroupConfig((HAL_xpu2_XPU2Type)cfg->id,
                                       cfg->rg.rpu[i].index, &rg_cfg);
      rg_cfg.resPerm.bMultiVMID = TRUE;
      /* All VMIDs will be programmed. */
      rg_cfg.resPerm.Vmid_Type.multi.auVMID[0] = 0xffffffff;
      /* Lower 16 vmids */
      rg_cfg.resPerm.Vmid_Type.multi.auVMIDPerm[0] =
              tzbsp_vmids_to_perm(cfg->rg.rpu[i].read_vmid,
                                  cfg->rg.rpu[i].write_vmid);
      /* Upper 16 vmids */
      rg_cfg.resPerm.Vmid_Type.multi.auVMIDPerm[1] =
              tzbsp_vmids_to_perm((cfg->rg.rpu[i].read_vmid) >> 16,
                                  (cfg->rg.rpu[i].write_vmid) >> 16);

      if(TZBSP_XPU_SEC & cfg->rg.rpu[i].flags)
      {
        rg_cfg.eConfig = HAL_XPU2_CONFIG_SECURE;
        rg_cfg.clientPerm.bNonSecureClientROEn = HAS_ROE(cfg, i);
        rg_cfg.clientPerm.bNonSecureClientRWEn = HAS_RWE(cfg, i);
        rg_cfg.clientPerm.bMSAClientROEn = HAS_MSA_ROE(cfg, i);
        rg_cfg.clientPerm.bMSAClientRWEn = HAS_MSA_RWE(cfg, i);
      }
      else if(TZBSP_XPU_MODEM_PROT & cfg->rg.rpu[i].flags)
      {
        rg_cfg.eConfig = HAL_XPU2_CONFIG_MODEM;
      }
      else
      {
        rg_cfg.eConfig = HAL_XPU2_CONFIG_NON_SECURE;
      }

      if(HAL_XPU2_NO_ERROR != HAL_xpu2_ConfigResourceGroup(
                                               (HAL_xpu2_XPU2Type)cfg->id,
                                               cfg->rg.rpu[i].index, &rg_cfg))
      {
        return -E_FAILURE;
      }
    }
  }
  else /* Single VMID. */
  {

    for(i=0; i < cfg->nrg; i++)
    {
    #ifdef FEATURE_NO_XPU
      /* MBA/Modem expect xPUs to be set-up, so only program resource groups
       * marked for MSA use */
      if(!(TZBSP_XPU_MODEM_PROT & cfg->rg.rpu[i].flags)) continue;
    #endif

      if(HAL_XPU2_PMIC_ARB_SPMI == (HAL_xpu2_XPU2Type)cfg->id)
      {
        /* TODO: temporarily removed SPMI call. */
        /* if(SPMI_CFG_SUCCESS != SpmiCfg_GetLogicalIdFromPpid( */
        /*                                 (uint32)cfg->rg.rpu[i].index, */
        /*                                 &index)) */
        /* { */
        /*   TZBSP_LOG_ERR("PPID to APID mapping error for PPID 0x%X", */
        /*                 cfg->rg.rpu[i].index); */
        /*   return -E_FAILURE; */
        /* } */
      }
      else
      {
        index = cfg->rg.rpu[i].index;
      }
      rg_cfg.resPerm.Vmid_Type.single.uRWVMID = cfg->rg.rpu[i].write_vmid;
      rg_cfg.resPerm.Vmid_Type.single.uROVMID = cfg->rg.rpu[i].read_vmid;
      HAL_xpu2_GetResourceGroupConfig((HAL_xpu2_XPU2Type)cfg->id, index,
                                      &rg_cfg);
      rg_cfg.resPerm.bMultiVMID = FALSE;
      rg_cfg.resPerm.Vmid_Type.single.bRWGlobalEn = HAS_RWGE(cfg, i);
      rg_cfg.resPerm.Vmid_Type.single.bRWVMIDValid = HAS_RWE(cfg, i);
      rg_cfg.resPerm.Vmid_Type.single.bROGlobalEn = HAS_ROGE(cfg, i);
      rg_cfg.resPerm.Vmid_Type.single.bROVMIDValid = HAS_ROE(cfg, i);

      rg_cfg.resPerm.Vmid_Type.single.uRWVMID = cfg->rg.rpu[i].write_vmid;
      rg_cfg.resPerm.Vmid_Type.single.uROVMID = cfg->rg.rpu[i].read_vmid;

      rg_cfg.clientPerm.bSecureClientROEn    = FALSE;
      rg_cfg.clientPerm.bNonSecureClientROEn = FALSE;
      rg_cfg.clientPerm.bMSAClientROEn       = FALSE;
      rg_cfg.clientPerm.bNonSecureClientRWEn = FALSE;
      rg_cfg.clientPerm.bMSAClientRWEn       = FALSE;

      if(TZBSP_XPU_SEC & cfg->rg.rpu[i].flags)
      {
        rg_cfg.eConfig = HAL_XPU2_CONFIG_SECURE;
        rg_cfg.clientPerm.bNonSecureClientROEn = HAS_ROGE(cfg, i);
        rg_cfg.clientPerm.bNonSecureClientRWEn = HAS_RWGE(cfg, i);
        rg_cfg.clientPerm.bMSAClientROEn = HAS_MSA_ROE(cfg, i);
        rg_cfg.clientPerm.bMSAClientRWEn = HAS_MSA_RWE(cfg, i);
      }
      else if(TZBSP_XPU_MODEM_PROT & cfg->rg.rpu[i].flags)
      {
        rg_cfg.eConfig = HAL_XPU2_CONFIG_MODEM;
        rg_cfg.clientPerm.bNonSecureClientROEn = HAS_ROGE(cfg, i);
        rg_cfg.clientPerm.bNonSecureClientRWEn = HAS_RWGE(cfg, i);
      }
      else
      {
        rg_cfg.eConfig = HAL_XPU2_CONFIG_NON_SECURE;
      }

      if(HAL_XPU2_NO_ERROR != HAL_xpu2_ConfigResourceGroup(
                                               (HAL_xpu2_XPU2Type)cfg->id,
                                               index, &rg_cfg))
      {
        return -E_FAILURE;
      }
    }
  }

  return E_SUCCESS;
}

/* Applies the static configuration of an XPU.
 *
 * @param [in] cfg  MPU/APU/RPU configuration data.
 */
int tzbsp_xpu_do_static_config(const tzbsp_xpu_cfg_t* cfg)
{
  /* Options HAL_XPU_OPTION_NS_PROT_EN and HAL_XPU_OPTION_SHARE_EN are only
   * valid for the SMPSS L2 MPU (which is currently not enabled). */
  HAL_xpu2_ErrorOptionsConfigType opt = HAL_XPU2_ERROR_O_CFG_RPT_EN |
                                        HAL_XPU2_ERROR_O_DECODE_EN;

  HAL_xpu2_ResourceGroupPermissionType acc = {0};
  HAL_xpu2_XPUDeviceParamsType info ;
  HAL_xpu2_DefaultxPU2ConfigType xpu_cfg = {0};
  HAL_xpu2_XPU2Type hid = (HAL_xpu2_XPU2Type)cfg->id;

#ifndef TZBSP_WITH_TEST_SVC
  opt |= HAL_XPU2_ERROR_O_CLIENT_RPT_EN;
#endif
  memset((void *)&info ,0 , sizeof(HAL_xpu2_XPUDeviceParamsType));
  /* Enable the interrupts only on those XPUs that are not marked interrupts
   * disabled. */
  if(!(TZBSP_XPU_NO_INTERRUPTS & cfg->flags))
   {
    opt |= HAL_XPU2_ERROR_O_INTERRUPT_EN;
  }

  acc.bMultiVMID = TRUE;
    /* Configure unmapped area access permissions */
    acc.Vmid_Type.multi.auVMID[0] = 0xffffffff; /* All VMID bits are configured.*/
#ifdef FEATURE_NO_XPU
  acc.Vmid_Type.multi.auVMIDPerm[0] = tzbsp_vmids_to_perm((uint16)0xFFFFFFFF, (uint16)0xFFFFFFFF);
  acc.Vmid_Type.multi.auVMIDPerm[1] = tzbsp_vmids_to_perm((uint16)0xFFFFFFFF, (uint16)0xFFFFFFFF);
#else
  acc.Vmid_Type.multi.auVMIDPerm[0] = tzbsp_vmids_to_perm(cfg->unmapped_rvmid,
                                                          cfg->unmapped_wvmid);
  acc.Vmid_Type.multi.auVMIDPerm[1] = tzbsp_vmids_to_perm(cfg->unmapped_rvmid >> 16,
                                                          cfg->unmapped_wvmid >> 16);
#endif

  xpu_cfg.pUnmapRGPerm = &acc;

  xpu_cfg.bEnableVmidCheck = TRUE;
  xpu_cfg.bEnablexPROTNSCheck = TRUE;

#ifdef TZBSP_AMT_MAX_BIT_POS
  /* Enable AMT support for DDR MPU if the chip supports AMT */
  if((HAL_XPU2_BIMC_MPU0 == hid) || (HAL_XPU2_BIMC_MPU1 == hid))
  {
    xpu_cfg.bEnableNonSecureAMT = TRUE;
  }
#endif /* TZBSP_AMT_MAX_BIT_POS */

  if(HAL_XPU2_NO_ERROR != HAL_xpu2_Init(hid, &xpu_cfg, NULL))
  {
    return -E_FAILURE;
  }
  tzbsp_dsb();
  if(HAL_XPU2_NO_ERROR != HAL_xpu2_ConfigErrorReporting(hid,
                                               HAL_XPU2_CONFIG_NON_SECURE, opt))
  {
    return -E_FAILURE;
  }

  /* Since there are two ROTs in the system (TZ and MPSS) controlling the
   * BIMC MPU, but no communication channel between the two, we ignore
   * access violations by TZ on MPSS DDR regions
   */
  if(HAL_XPU2_BIMC_MPU0 == hid || HAL_XPU2_BIMC_MPU1 == hid)
  {
    opt &= ~(HAL_XPU2_ERROR_O_CLIENT_RPT_EN | HAL_XPU2_ERROR_O_INTERRUPT_EN);
  }

  opt |= HAL_XPU2_ERROR_O_NS_RES_CFG_EN;
  if(HAL_XPU2_NO_ERROR != HAL_xpu2_ConfigErrorReporting(hid,
                                                   HAL_XPU2_CONFIG_SECURE, opt))
  {
    return -E_FAILURE;
  }

  /* All XPUs are configurable by SROTs only, identified by NS 0 */
  acc.Vmid_Type.multi.auVMIDPerm[0] = 0;
  acc.Vmid_Type.multi.auVMIDPerm[1] = 0;
  /*
   * RPM is superuser for:
   * PMIC ARB SPMI xPU
   * GCC
   */
  if( (HAL_XPU2_CLK_CTL       == hid) ||
      (HAL_XPU2_PMIC_ARB      == hid))
  {
    acc.Vmid_Type.multi.auVMIDPerm[0] = tzbsp_vmids_to_perm(TZBSP_VMID_RPM_BIT,
                                                            TZBSP_VMID_RPM_BIT);
    acc.Vmid_Type.multi.auVMIDPerm[1] = tzbsp_vmids_to_perm(
                                        (TZBSP_VMID_RPM_BIT) >> 16,
                                        (TZBSP_VMID_RPM_BIT) >> 16);
  }
  HAL_xpu2_ConfigAccessPermissions(hid, &acc);

  HAL_xpu2_GetDeviceParameters(hid, &info);

  if(HAL_XPU2_DEVICE_MPU == info.eXpu2Type)
  {
    if(E_SUCCESS != tzbsp_mpu_config(cfg, &info))
      {
        return -E_FAILURE;
      }
    }
  else /* Either RPU or APU */
  {
    if(E_SUCCESS != tzbsp_rpu_config(cfg, &info))
    {
      return -E_FAILURE;
    }
  }

  if(TZBSP_XPU_MODEM_PROT & cfg->flags)
  {
    HAL_xpu2_EnableMSSSelfAuth(hid);
  }
  tzbsp_dsb();

  if(HAL_XPU2_NO_ERROR != HAL_xpu2_ClearError(hid, HAL_XPU2_CONFIG_SECURE) ||
     HAL_XPU2_NO_ERROR != HAL_xpu2_ClearError(hid, HAL_XPU2_CONFIG_NON_SECURE))
  {
    return -E_FAILURE;
  }
  tzbsp_dsb();
  xpu_init_done[hid] = 1;

  return E_SUCCESS;
}

/*
 * Enable violation interrupts
 */
static void tzbsp_xpu_enable_interrupts(void)
{
  /* Enable the interrupts propagation */
  HWIO_OUT(TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE, 0xFFFFFFFF);
  HWIO_OUT(TCSR_SS_XPU2_SEC_INTR0_ENABLE, 0xFFFFFFFF);
}


static void tzbsp_xpu_config_qrib()
{
#ifndef FEATURE_NO_XPU
  /* Venus & MDP client and config space is under TZ control by default */
  HWIO_OUTF(TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT, MMSS_VENUS0_VBIF_QRIB_XPU2_NSEN_INIT, 1);
  HWIO_OUTF(TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT, MMSS_VENUS0_VBIF_QRIB_XPU2_EN_TZ, 1);

  HWIO_OUTF(TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT, MMSS_VENUS0_WRAPPER_QRIB_XPU2_NSEN_INIT, 1);
  HWIO_OUTF(TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT, MMSS_VENUS0_WRAPPER_QRIB_XPU2_EN_TZ, 1);

#ifndef FIXME_8916
  HWIO_OUTF(TCSR_MMSS_MDP_QRIB_XPU2_INIT, MMSS_MDP_QRIB_XPU2_NSEN_INIT, 1);
  HWIO_OUTF(TCSR_MMSS_MDP_QRIB_XPU2_INIT, MMSS_MDP_QRIB_XPU2_EN_TZ, 1);
#endif

  HWIO_OUTF(TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR, MMSS_VENUS0_VBIF_QRIB_XPU2_ACR, TZBSP_VMID_TZ_BIT);
  HWIO_OUTF(TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT, MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT, 1);
  HWIO_OUTF(TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT, MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_EN_HV, 1);

  HWIO_OUTF(TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR, MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR, TZBSP_VMID_TZ_BIT);
  HWIO_OUTF(TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT, MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT, 1);
  HWIO_OUTF(TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT, MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_EN_HV, 1);

#ifndef FIXME_8916
  HWIO_OUTF(TCSR_MMSS_MDP_QRIB_XPU2_ACR, MMSS_MDP_QRIB_XPU2_ACR, TZBSP_VMID_TZ_BIT);
  HWIO_OUTF(TCSR_MMSS_MDP_QRIB_XPU2_VMIDEN_INIT, MMSS_MDP_QRIB_XPU2_VMIDEN_INIT, 1);
  HWIO_OUTF(TCSR_MMSS_MDP_QRIB_XPU2_VMIDEN_INIT, MMSS_MDP_QRIB_XPU2_VMIDEN_INIT_EN_HV, 1);
#endif
#endif
}

static int tzbsp_config_dehr_apu(void)
{
  extern const tzbsp_xpu_cfg_t g_tzbsp_dehr_msa_enabled_cfg;
  extern const tzbsp_xpu_cfg_t g_tzbsp_dehr_msa_disabled_cfg;
  int err = 0;
#ifndef FIXME_8916
  /* Register FEATURE_CONFIG7 is in Security Control, which is unmapped by
   * default. */
  tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL);

  if(0 == HWIO_INF(FEATURE_CONFIG7, MSA_ENA)) /* MSA is not present. */
  {
    err = tzbsp_xpu_do_static_config(&g_tzbsp_dehr_msa_disabled_cfg);
  }
  else /* MSA is present. */
  {
    err = tzbsp_xpu_do_static_config(&g_tzbsp_dehr_msa_enabled_cfg);
  }

  tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL);
#endif

  return err;
}


/* @copydoc tzbsp_xpu_static_config */
int tzbsp_xpu_static_config(void)
{
  int i;

  tzbsp_xpu_config_qrib();
  tzbsp_enable_xpu_config_clocks_boot(TRUE);

  for(i=0; i < g_tzbsp_xpu_cfg_count; i++)
  {
    if(TZBSP_XPU_ENABLE & g_tzbsp_xpu_cfg[i].flags)
    {
      /* Skip QPIC NAND config on eMMC device */
      if(((HAL_XPU2_QPIC_QPIC == g_tzbsp_xpu_cfg[i].id) || (HAL_XPU2_QPIC_QPIC_MPU == g_tzbsp_xpu_cfg[i].id)) &&
        (!tzbsp_check_for_nand_boot()))
        continue;

      if(E_SUCCESS != tzbsp_xpu_do_static_config(&g_tzbsp_xpu_cfg[i]))
        {
          return -E_FAILURE;
        }
      }
    }

  /* DEHR protection needs to be handled as a special case as it is conditional
   * based on MSA_ENA fuse state. */
  if(E_SUCCESS != tzbsp_config_dehr_apu())
  {
    return -E_FAILURE;
  }

  tzbsp_enable_xpu_config_clocks_boot(FALSE);

  tzbsp_xpu_enable_interrupts();

  return E_SUCCESS;
}

/* @copydoc tzbsp_xpu_restore_config */
int tzbsp_xpu_restore_config(tz_device_e_type device)
{
#ifdef FEATURE_NO_XPU
  return E_SUCCESS;
#else
  int i, j;
  int ret = -E_INVALID_ARG;
 
  for (i = 0; i < tzbsp_device_to_xpu_map[device].num_xpu; ++i)
  {
    for (j = 0; (j < g_tzbsp_xpu_cfg_count); ++j)
    {
      if(tzbsp_device_to_xpu_map[device].xpu[i] ==
         (HAL_xpu2_XPU2Type) g_tzbsp_xpu_cfg[j].id)
      {
        ret = tzbsp_xpu_do_static_config(&g_tzbsp_xpu_cfg[j]);
        break;
      }
    }
    if(E_SUCCESS != ret)
    {
      break;
    }
  }
  return ret;
#endif
}

/* Checks non-QRIB controlled settings to determine if an XPU has already
 * been configured
 *
 * @param [in] xpu  XPU HAL index
 *
 * @return TRUE if XPU has been configured, FALSE otherwise
 */
boolean tzbsp_xpu_is_configured(uint32 xpu)
{
#ifdef FEATURE_NO_XPU
  return TRUE;
#else
  HAL_xpu2_ErrorOptionsConfigType err_opts = {0};
  HAL_xpu2_ErrorOptionsConfigType cmp_opts = HAL_XPU2_ERROR_O_CLIENT_RPT_EN |
                                             HAL_XPU2_ERROR_O_CFG_RPT_EN |
                                             HAL_XPU2_ERROR_O_DECODE_EN |
                                             HAL_XPU2_ERROR_O_NS_RES_CFG_EN;

  tzbsp_enable_xpu_config_clocks(TRUE);

  /* Grab the error reporting options for this xPU to determine if
   * its been configured */
  HAL_xpu2_GetErrorReportingConfig((HAL_xpu2_XPU2Type)xpu,
                                   HAL_XPU2_CONFIG_SECURE,
                                   &err_opts);

  tzbsp_enable_xpu_config_clocks(FALSE);

  return ((err_opts & cmp_opts) == cmp_opts);
#endif
}

/* Dynamically locks an MPU resource group (memory area). Memory area start and
 * end address alignment is dependent on the actual MPU.
 *
 * @param [in] mpu    MPU HAL index.
 * @param [in] index  Resource group index.
 * @param [in] start  Start of the memory area (included in the range).
 * @param [in] end    End of the memory area (excluded from the range).
 * @param [in] vmids  Bitmaps of VMIDs that have full access to the area.
 *
 * @return Zero on success, otherwise an error code.
 */
int tzbsp_mpu_lock_area(uint32 mpu, uint32 index, uint32 start,
                        uint32 end, uint32 rvmids, uint32 wvmids)
{
#ifdef FEATURE_NO_XPU
  return E_SUCCESS;
#else

  HAL_xpu2_ResourceGroupConfigType par = {0};
  HAL_xpu2_Status ret = HAL_XPU2_NO_ERROR;

  HAL_xpu2_XPU2Type hid = (HAL_xpu2_XPU2Type)0;
  tzbsp_xpu_cfg_t xpu_cfg = {0};

  hid = (HAL_xpu2_XPU2Type) mpu;

  if(0 == xpu_init_done[hid])
  {
    xpu_cfg.id = hid;
    xpu_cfg.unmapped_rvmid = 0xFFFFFFFF;
    xpu_cfg.unmapped_wvmid = 0xFFFFFFFF;
    tzbsp_enable_xpu_config_clocks(TRUE);
    if(E_SUCCESS != tzbsp_xpu_do_static_config(&xpu_cfg))
    {
      tzbsp_enable_xpu_config_clocks(FALSE);
      return -E_FAILURE;
    }
    else if(HAL_XPU2_BIMC_MPU0 == hid)
    {
      /* If the request is for DDR MPU, need to init both channels' xPUs */
      xpu_cfg.id = HAL_XPU2_BIMC_MPU1;
      if(E_SUCCESS != tzbsp_xpu_do_static_config(&xpu_cfg))
      {
        tzbsp_enable_xpu_config_clocks(FALSE);
        return -E_FAILURE;
      }
    }
    tzbsp_enable_xpu_config_clocks(FALSE);
  }

  par.resPerm.bMultiVMID = TRUE;
  par.resPerm.Vmid_Type.multi.auVMID[0] = 0xffffffff; /* All VMIDs programmed.*/
  /* Lower 16 vmids */
  par.resPerm.Vmid_Type.multi.auVMIDPerm[0] =
                                            tzbsp_vmids_to_perm(rvmids, wvmids);
  /* Upper 16 vmids */
  par.resPerm.Vmid_Type.multi.auVMIDPerm[1] =
                            tzbsp_vmids_to_perm((rvmids) >> 16, (wvmids) >> 16);
  par.resAddr.uStartLower = start;
  par.resAddr.uEndLower = end;
  par.resAddr.uStartUpper = 0;
  par.resAddr.uEndUpper = 0;
  par.eConfig = HAL_XPU2_CONFIG_NON_SECURE;

  tzbsp_dsb();
  ret = HAL_xpu2_ConfigResourceGroup((HAL_xpu2_XPU2Type)mpu, index, &par);
  tzbsp_dsb();

  if(HAL_XPU2_NO_ERROR == ret)
  {
    return E_SUCCESS;
  }
  else
  {
    return -E_FAILURE;
  }
#endif
}

/* Dynamically unlocks an MPU resource group (memory area). After unlocking the
 * previously protected memory area is accessible by all masters (unless other
 * areas overlap with the unlocked area).
 *
 * @param [in] mpu    MPU HAL index.
 * @param [in] index  Resource group index.
 *
 * @return Zero on success, otherwise an error code.
 */
int tzbsp_mpu_unlock_area(uint32 mpu, uint32 index)
{
#ifdef FEATURE_NO_XPU
  return E_SUCCESS;
#else

  HAL_xpu2_ResourceGroupConfigType par = {0};
  HAL_xpu2_Status ret = HAL_XPU2_NO_ERROR;

  HAL_xpu2_XPU2Type hid = (HAL_xpu2_XPU2Type)0;
  hid = (HAL_xpu2_XPU2Type) mpu;

  if(0 == xpu_init_done[hid])
  {
    /* The xPU is not init'ed so the area cannot be locked in the first place
     * */
    return -E_INVALID_ARG;
  }

  /* TODO: Determine why this causes UEFI to fail during cold boot */
#if 0
  HAL_xpu2_GetResourceGroupConfig(hid, index, &par);
  /* memset to zero before unlocking */
#if !defined(TZBSP_VIRTIO)
  memset((void*)(par.resAddr.uStartLower), 0,
                 par.resAddr.uEndLower - par.resAddr.uStartLower);
#endif
  tzbsp_dcache_flush_region((void*) par.resAddr.uStartLower,
                            par.resAddr.uEndLower - par.resAddr.uStartLower);
#endif

  par.resPerm.bMultiVMID = TRUE;
  par.resPerm.Vmid_Type.multi.auVMID[0] = 0xffffffff; /* All VMIDs programmed.*/
  par.resPerm.Vmid_Type.multi.auVMIDPerm[0] = 0;
  par.resPerm.Vmid_Type.multi.auVMIDPerm[1] = 0;

  /* Configure start/end addresses to disable the partition */
  par.resAddr.uStartLower = 0xFFFFFFFF;
  par.resAddr.uEndLower = 0xFFFFFFFF;
  par.resAddr.uStartUpper = 0;
  par.resAddr.uEndUpper = 0;
  par.eConfig = HAL_XPU2_CONFIG_NON_SECURE;

  if(HAL_XPU2_BIMC_MPU0 == mpu)
  {
    tzbsp_dsb();

    ret = HAL_xpu2_ConfigResourceGroup(HAL_XPU2_BIMC_MPU0, index, &par);

    tzbsp_dsb();
  }
  else /* Other MPU protected memories are not interleaved. */
  {
    tzbsp_dsb();
    ret = HAL_xpu2_ConfigResourceGroup((HAL_xpu2_XPU2Type)mpu, index, &par);
    tzbsp_dsb();
  }
  if(HAL_XPU2_NO_ERROR == ret)
  {
    return E_SUCCESS;
  }
  else
  {
    return -E_FAILURE;
  }
#endif
}


/* @copydoc tzbsp_xpu_lock_area */
int tzbsp_xpu_lock_area(uint32 id, uint32 start, uint32 end, uint32 rvmid,
                        uint32 wvmid)
{
  int err = E_SUCCESS;
  HAL_xpu2_XPU2Type xpu = HAL_XPU2_COUNT;
  uint32 rg = 0;

  switch(id)
  {
#ifndef FIXME_8916
    case TZBSP_BLIST_LPASS:
      xpu = HAL_XPU2_BIMC_MPU0;
      rg  = TZBSP_EBI1_RG_LPASS;
      break;
#endif

    case TZBSP_BLIST_VIDEO:
      xpu = HAL_XPU2_BIMC_MPU0;
      rg  = TZBSP_EBI1_RG_VIDEO;
      break;

    case TZBSP_BLIST_WLAN:
      xpu = HAL_XPU2_BIMC_MPU0;
      rg  = TZBSP_EBI1_RG_WLAN;
      break;

    default:
      err = -E_INVALID_ARG;
      break;
  }

  if(E_SUCCESS == err)
  {
    err = tzbsp_mpu_lock_area(xpu, rg, start, end, rvmid, wvmid);
  }

  return err;
}

/* @copydoc tzbsp_xpu_unlock_area */
int tzbsp_xpu_unlock_area(uint32 id)
{
  int err = E_SUCCESS;

  if(TZBSP_BLIST_LPASS == id)
  {
#ifndef FIXME_8916
    tzbsp_mpu_unlock_area(HAL_XPU2_BIMC_MPU0, TZBSP_EBI1_RG_LPASS);
#endif
  }
  else if(TZBSP_BLIST_VIDEO == id)
  {
    tzbsp_mpu_unlock_area(HAL_XPU2_BIMC_MPU0, TZBSP_EBI1_RG_VIDEO);
  }
  else if(TZBSP_BLIST_WLAN == id)
  {
    tzbsp_mpu_unlock_area(HAL_XPU2_BIMC_MPU0, TZBSP_EBI1_RG_WLAN);
  }
  else
  {
    err = -E_INVALID_ARG;
  }

  return err;
}

/* @copydoc tzbsp_rpu_reconfigure */
int tzbsp_rpu_reconfigure(uint32 xpu_id, const tzbsp_rpu_rg_t *rpu,
                          boolean sec)
{
#ifdef FEATURE_NO_XPU
  return E_SUCCESS;
#else

  HAL_xpu2_XPUDeviceParamsType info;
  HAL_xpu2_XPU2Type hid = (HAL_xpu2_XPU2Type)0;
  HAL_xpu2_ResourceGroupConfigType cfg = {0};
  HAL_xpu2_Status ret = HAL_XPU2_NO_ERROR;
  tzbsp_xpu_cfg_t xpu_cfg = {0};
  int i = 0;

  memset((void *)&info,0,sizeof(HAL_xpu2_XPUDeviceParamsType));
  if (rpu == NULL)
    return -E_INVALID_ARG;

  if(xpu_id >= HAL_XPU2_COUNT) /* Check for valid XPU id. */
    return -E_INVALID_ARG;

  hid = (HAL_xpu2_XPU2Type) xpu_id;
  /* Skip QPIC NAND config on eMMC device */
  if((HAL_XPU2_QPIC_QPIC == hid) && (!tzbsp_check_for_nand_boot()))
    return E_SUCCESS;

  if(0 == xpu_init_done[hid])
  {
    /*
     * Search through the xpus. If the xpu was supposed to be statically
     * init'ed at cold boot but appears to have been not init'ed now, it must
     * have been left out intentionally for this boot. So, only init with open
     * access and apply current required config. If the xpu was not supposed
     * to be statically init'ed then apply full static config
     */
    for(i = 0; i < g_tzbsp_xpu_cfg_count; ++i)
    {
      if((g_tzbsp_xpu_cfg[i].id == hid) &&
         (TZBSP_XPU_ENABLE & g_tzbsp_xpu_cfg[i].flags))
    {
        /* Found the xPU */
        memcpy(&xpu_cfg, &g_tzbsp_xpu_cfg[i], sizeof(tzbsp_xpu_cfg_t));
        break;
      }
    }
    if(i == g_tzbsp_xpu_cfg_count)
    {
      /*
       * xPU not found in config or found but was supposed to be
       * statically configured, so init with open access
       */
      xpu_cfg.id = hid;
    }
    tzbsp_enable_xpu_config_clocks(TRUE);
    if(E_SUCCESS != tzbsp_xpu_do_static_config(&xpu_cfg))
    {
      tzbsp_enable_xpu_config_clocks(FALSE);
      return -E_FAILURE;
    }
    tzbsp_enable_xpu_config_clocks(FALSE);
  }
  HAL_xpu2_GetDeviceParameters(hid, &info);

  if(HAL_XPU2_DEVICE_RPU != info.eXpu2Type &&
     HAL_XPU2_DEVICE_APU != info.eXpu2Type)
    return -E_INVALID_ARG;

  if(rpu->index >= info.uResourceGroupCount) /* Check for valid rg index. */
    return -E_INVALID_ARG;

  /* We have a valid APU/RPU and a valid resource group, just reprogram the
   * resource group. */
  tzbsp_enable_xpu_config_clocks(TRUE);
  if(info.bMultiVMID)
  {
    HAL_xpu2_GetResourceGroupConfig(hid, rpu->index, &cfg);

    cfg.resPerm.bMultiVMID = TRUE;
    cfg.resPerm.Vmid_Type.multi.auVMID[0] = 0xffffffff;/*All VMIDs programmed.*/
    /* Lower 16 vmids */
    cfg.resPerm.Vmid_Type.multi.auVMIDPerm[0] =
            tzbsp_vmids_to_perm(rpu->read_vmid, rpu->write_vmid);
    /* Upper 16 vmids */
    cfg.resPerm.Vmid_Type.multi.auVMIDPerm[1] =
            tzbsp_vmids_to_perm((rpu->read_vmid) >> 16,
                                (rpu->write_vmid) >> 16);

    /* Set up client permissions for secure/MSA partitions. */
    cfg.clientPerm.bSecureClientROEn    = FALSE;
    cfg.clientPerm.bNonSecureClientROEn = (TZBSP_ROGE & rpu->flags) ? TRUE: FALSE;
    cfg.clientPerm.bMSAClientROEn       = FALSE;
    cfg.clientPerm.bNonSecureClientRWEn = (TZBSP_RWGE & rpu->flags) ? TRUE: FALSE;
    cfg.clientPerm.bMSAClientRWEn       = FALSE;

    cfg.eConfig = (sec) ? HAL_XPU2_CONFIG_SECURE: HAL_XPU2_CONFIG_NON_SECURE;

    ret = HAL_xpu2_ConfigResourceGroup(hid, rpu->index, &cfg);
  }
  else /* Single VMID. */
  {
    HAL_xpu2_GetResourceGroupConfig(hid, rpu->index, &cfg);

    cfg.resPerm.bMultiVMID = FALSE;
    cfg.resPerm.Vmid_Type.single.bRWGlobalEn =
                                        (TZBSP_RWGE & rpu->flags) ? TRUE: FALSE;
    cfg.resPerm.Vmid_Type.single.bRWVMIDValid =
                                         (TZBSP_RWE & rpu->flags) ? TRUE: FALSE;
    cfg.resPerm.Vmid_Type.single.bROGlobalEn =
                                        (TZBSP_ROGE & rpu->flags) ? TRUE: FALSE;
    cfg.resPerm.Vmid_Type.single.bROVMIDValid =
                                         (TZBSP_ROE & rpu->flags) ? TRUE: FALSE;
    cfg.resPerm.Vmid_Type.single.uRWVMID = rpu->write_vmid;
    cfg.resPerm.Vmid_Type.single.uROVMID = rpu->read_vmid;

    cfg.clientPerm.bSecureClientROEn    = FALSE;
    cfg.clientPerm.bNonSecureClientROEn = FALSE;
    cfg.clientPerm.bMSAClientROEn       = FALSE;
    cfg.clientPerm.bNonSecureClientRWEn = FALSE;
    cfg.clientPerm.bMSAClientRWEn       = FALSE;

    cfg.eConfig = (sec) ? HAL_XPU2_CONFIG_SECURE: HAL_XPU2_CONFIG_NON_SECURE;

    ret = HAL_xpu2_ConfigResourceGroup(hid, rpu->index, &cfg);
  }
  tzbsp_enable_xpu_config_clocks(FALSE);

  if(HAL_XPU2_NO_ERROR == ret)
  {
    return E_SUCCESS;
  }
  else
  {
    return -E_FAILURE;
  }
#endif
}

/** @copydoc tzbsp_mpu_lock_dyn_area */
int tzbsp_mpu_lock_dyn_area(uint32 area_id, uint32 start, uint32 end)
{
  int err = -E_NOT_SUPPORTED;

  if(TZBSP_XPU_DYN_AREA_CERT == area_id)
  {
    err = tzbsp_mpu_lock_area(HAL_XPU2_BIMC_MPU0, TZBSP_EBI1_RG_DYN_TMP_1,
                              start, end, TZBSP_VMID_TZ_BIT, TZBSP_VMID_TZ_BIT);
  }

  return err;
}

/** @copydoc tzbsp_mpu_unlock_dyn_area */
int tzbsp_mpu_unlock_dyn_area(uint32 area_id)
{
  int err = -E_NOT_SUPPORTED;

  if(TZBSP_XPU_DYN_AREA_CERT == area_id)
  {
    err = tzbsp_mpu_unlock_area(HAL_XPU2_BIMC_MPU0, TZBSP_EBI1_RG_DYN_TMP_1);
  }

  return err;
}

/* @copydoc tzbsp_xpu_is_multi_vmid */
boolean tzbsp_xpu_is_multi_vmid(uint32 xpu_id)
{
  HAL_xpu2_XPUDeviceParamsType info ;
  memset((void *)&info,0,sizeof(HAL_xpu2_XPUDeviceParamsType));
  
  TZBSP_ALWAYS(xpu_id >= HAL_XPU2_COUNT);

  HAL_xpu2_GetDeviceParameters((HAL_xpu2_XPU2Type) xpu_id, &info);

  return (info.bMultiVMID) ? TRUE: FALSE;
}

static inline uint32 get_next_error_xpu(uint32 bitmask, uint32 *lastval)
{
  uint32 i = *lastval;
  /*
   * Loop over the 32 bits of input bitmask to find the first bit set starting
   * from the bit position pointed to by lastval
   */
  for (; (i <= TZBSP_XPU_ERR_BIT_POS_MAX) && (0 != bitmask); i++)
  {
    if((bitmask >> i) & 0x1)
    {
      break;
    }
  }
  /*
   * Set starting position for next iteration to be 1 more than current
   * set bit found. If no bit found, then it makes lastval as invalid
   */
  *lastval = i + 1;
  return i;
}

/** checking for the MPU partition overlapping
  ** Only in case of non secure and secure XPU violation this check can be done
  ** modem violation is handled by so check will be there.
**/

boolean check_for_mpu_overlapping(HAL_xpu2_XPU2Type xpu, HAL_xpu2_ConfigType type)
{
  HAL_xpu2_ErrorType err ;
  HAL_xpu2_XPUDeviceParamsType info ;
  HAL_xpu2_BusErrorFlagsMaskType bus_err_flag;

  memset((void *)&err,0,sizeof(HAL_xpu2_ErrorType));
  memset((void *)&info,0,sizeof(HAL_xpu2_XPUDeviceParamsType));

  HAL_xpu2_GetDeviceParameters((HAL_xpu2_XPU2Type)xpu, &info);
  if(TRUE == HAL_xpu2_IsError(xpu, type))
  {
     if( HAL_XPU2_DEVICE_MPU ==info.eXpu2Type)
     {
        HAL_xpu2_GetError((HAL_xpu2_XPU2Type)xpu, type, &err);
        bus_err_flag = err.uBusFlags;
        /* Checking for the valid case in which there is no overlapping */
        if(   (((bus_err_flag & HAL_XPU2_BUS_F_MSA_RG_MATCH)!= 0) && ((bus_err_flag & HAL_XPU2_BUS_F_SECURE_RG_MATCH)==0) && ((bus_err_flag & HAL_XPU2_BUS_F_NONSECURE_RG_MATCH)==0))
           || (((bus_err_flag & HAL_XPU2_BUS_F_MSA_RG_MATCH)==0) && ((bus_err_flag & HAL_XPU2_BUS_F_SECURE_RG_MATCH)!=0) && ((bus_err_flag & HAL_XPU2_BUS_F_NONSECURE_RG_MATCH)==0))
           || (((bus_err_flag & HAL_XPU2_BUS_F_MSA_RG_MATCH)==0) && ((bus_err_flag & HAL_XPU2_BUS_F_SECURE_RG_MATCH)==0) && ((bus_err_flag & HAL_XPU2_BUS_F_NONSECURE_RG_MATCH)!=0))
           || (((bus_err_flag & HAL_XPU2_BUS_F_MSA_RG_MATCH)==0) && ((bus_err_flag & HAL_XPU2_BUS_F_SECURE_RG_MATCH)==0) && ((bus_err_flag & HAL_XPU2_BUS_F_NONSECURE_RG_MATCH)==0)))
         {
           return FALSE;
         }
        else
        {
          TZBSP_LOG_ERR("Overlapping in MPU = %s",info.szXpu2Name);
          return TRUE;
        }
     }
  }
  return FALSE;
} 



/**
 * Log the XPU Errors
 */
void tzbsp_xpu_error_log(HAL_xpu2_XPU2Type xpu, HAL_xpu2_ConfigType type)
{
  HAL_xpu2_ErrorType err ;
  HAL_xpu2_XPUDeviceParamsType info ;

  memset((void *)&err,0,sizeof(HAL_xpu2_ErrorType));
  memset((void *)&info,0,sizeof(HAL_xpu2_XPUDeviceParamsType));

  if(!g_tzbsp_dump_xpu_syndrome)
  {
    return;
  }

  HAL_xpu2_GetDeviceParameters((HAL_xpu2_XPU2Type)xpu, &info);
  if(TRUE == HAL_xpu2_IsError(xpu, type))
  {
    g_tzbsp_xpu_err_count++;
    HAL_xpu2_GetError((HAL_xpu2_XPU2Type)xpu, type, &err);
    tzbsp_log(TZBSP_MSG_DEBUG, "xpu:>>> [%u] XPU error dump, XPU id %u (%s)<<<",
              g_tzbsp_xpu_err_count, xpu, info.szXpu2Name);
    tzbsp_log(TZBSP_MSG_DEBUG, " xpu: uErrorFlags: %08x", err.uErrorFlags);
    DUMP_XPU_ERR_FLAG(err.uErrorFlags, HAL_XPU2_ERROR_F_CONFIG_PORT);
    DUMP_XPU_ERR_FLAG(err.uErrorFlags, HAL_XPU2_ERROR_F_CLIENT_PORT);
    DUMP_XPU_ERR_FLAG(err.uErrorFlags, HAL_XPU2_ERROR_F_MULTIPLE);

    tzbsp_log(TZBSP_MSG_DEBUG, " uBusFlags: %08x", err.uBusFlags);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_ERROR_AC);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_ERROR_DCD);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_ASHARED);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_AINNERSHARED);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_APRIV);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_APROTNS);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_AINST);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_AWRITE);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_AOOO);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_AFULL);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_ABURST);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_BURSTLEN);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_ARDALLOCATE);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_AEXCLUSIVE);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_ARDBEADNDXEN);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_AMSSSELFAUTH);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_SAVERESTORE_IN_PROG);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_MSA_RG_MATCH);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_SECURE_RG_MATCH);
    DUMP_XPU_ERR_FLAG(err.uBusFlags, HAL_XPU2_BUS_F_NONSECURE_RG_MATCH);

    tzbsp_log(TZBSP_MSG_DEBUG, " xpu: uPhysicalAddress: %08x",
              err.uPhysicalAddressLower32);
    tzbsp_log(TZBSP_MSG_DEBUG, " xpu: uMasterId: %08x, uAVMID   : %08x",
              err.uMasterId, err.uAVMID);
    tzbsp_log(TZBSP_MSG_DEBUG, " xpu: uATID    : %08x, uABID    : %08x",
              err.uATID, err.uABID);
    tzbsp_log(TZBSP_MSG_DEBUG, " xpu: uAPID    : %08x, uALen    : %08x",
              err.uAPID, err.uALen);
    tzbsp_log(TZBSP_MSG_DEBUG, " xpu: uASize   : %08x, uAPReqPriority   : %08x",
              err.uASize, err.uAPReqPriority);
    tzbsp_log(TZBSP_MSG_DEBUG, " xpu: uAMemType: %08x", err.uAMemType);
    /* Clear the Error */
    HAL_xpu2_ClearError(xpu, type);
  }
  else
  {
    TZBSP_LOG_ERR("XPU ERROR: XPU %s reported no error!!", info.szXpu2Name);
  }
}

/* @copydoc tzbsp_xpu_isr */
void* tzbsp_xpu_isr(void* ctx)
{
  uint32 err_bitmask = {0};
  uint32 xpu_position = 0;
  uint32 violation_position = 0;
  HAL_xpu2_XPU2Type xpu = HAL_XPU2_COUNT;
  HAL_xpu2_ConfigType err_type = HAL_XPU2_CONFIG_SECURE;
  uint32 i = 0, j = 0;
  uint8 is_partition_overlap =0;

  tzbsp_log(TZBSP_MSG_DEBUG, "xpu: ISR begin");

  switch(*((int *)ctx))
  {
    case TZBSP_XPU_ERR_SEC_CTX:
      {
        TZBSP_LOG_ERR("XPU ERROR: Sec!!");
        violation_position = HWIO_IN(TCSR_SS_XPU2_SEC_INTR0);
        TZBSP_LOG_ERR("XPU ERROR: bit mask: %x", violation_position);
        err_bitmask = HWIO_TCSR_SS_XPU2_SEC_INTR0_IN;
        err_type = HAL_XPU2_CONFIG_SECURE;
        break;
      }

    case TZBSP_XPU_ERR_NON_SEC_CTX:
      {
        TZBSP_LOG_ERR("XPU ERROR: Non Sec!!");
        violation_position = HWIO_IN(TCSR_SS_XPU2_NON_SEC_INTR0);
        TZBSP_LOG_ERR("XPU ERROR: bit mask: %x", violation_position);
        err_bitmask= HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_IN;
        err_type = HAL_XPU2_CONFIG_NON_SECURE;
        break;
      }

    default:
      {
        TZBSP_LOG_ERR("XPU ERROR: Invalid!!");
        break;
      }
  }

  if(0 != err_bitmask)
    {
      i = 0;
      do
      {
      xpu_position = get_next_error_xpu(err_bitmask, &i);
        if(xpu_position <= TZBSP_XPU_ERR_BIT_POS_MAX)
        {
        for (j = 0; j < (sizeof(xpu_err_pos_to_hal_map) /
                           sizeof(xpu_err_pos_to_hal_map_t)); ++j)
          {
            if( xpu_position == xpu_err_pos_to_hal_map[j].bit_pos)
            {
              xpu = (HAL_xpu2_XPU2Type) xpu_err_pos_to_hal_map[j].xpu;
              if(HAL_XPU2_COUNT != xpu)
              {
                tzbsp_enable_xpu_isr_clocks(TRUE, err_bitmask);
                is_partition_overlap = check_for_mpu_overlapping(xpu,err_type);
                if(HAL_XPU2_QPIC_QPIC == xpu)
                {
                  if(xpu_init_done[HAL_XPU2_QPIC_QPIC])
                  {
                    tzbsp_xpu_error_log(HAL_XPU2_QPIC_QPIC, err_type);
                  }
                  if(xpu_init_done[HAL_XPU2_QPIC_QPIC_MPU])
                  {
                    tzbsp_xpu_error_log(HAL_XPU2_QPIC_QPIC_MPU, err_type);
                  }
                }
                else
                {
                  tzbsp_xpu_error_log(xpu, err_type);
                }
                tzbsp_enable_xpu_isr_clocks(FALSE, err_bitmask);
              }
            }
           }
          }
      } while (i < TZBSP_XPU_ERR_BIT_POS_MAX);
    }
  
  if((TRUE == is_partition_overlap) || (TZ_XPU_VIOLATION_ERR_FATAL_ENABLE == tzbsp_xpu_violation_is_err_fatal))
  {
    if(TRUE == is_partition_overlap )
   {
     TZBSP_LOG_ERR("ERROR:MPU Partition overlapping found!");
   }
    tzbsp_err_fatal(TZBSP_ERR_FATAL_XPU_VIOLATION);
  }
  return ctx;
}

/**
 * Add VMIDs (read/write) to any RG
 *
 * @param [in] xpu        XPU Device ID
 * @param [in] index      Resource group index
 * @param [in] rvmid      Bitmask  of read access VMIDs to be added. 0 if no
 *                        change needed in rvmid
 * @param [in] wvmid      Bitmask of write access VMIDs to be added. 0 if no
 *                        change needed in wvmid
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */

int tzbsp_xpu_add_vmid_perms(HAL_xpu2_XPU2Type xpu, uint32 index, uint32 rvmid,
                             uint32 wvmid)
{
  int ret = E_SUCCESS;
  uint32 temp_perm = 0;
  HAL_xpu2_ResourceGroupConfigType rg = {0};

  if(HAL_XPU2_COUNT >= xpu)
  {
    if(tzbsp_xpu_is_multi_vmid(xpu))
    {
      HAL_xpu2_GetResourceGroupConfig(xpu, index, &rg);
      rg.resPerm.Vmid_Type.multi.auVMID[0]=0xFFFFFFFF; /* Program all VMIDs */

      temp_perm = tzbsp_vmids_to_perm(rvmid, wvmid);
      rg.resPerm.Vmid_Type.multi.auVMIDPerm[0] |= temp_perm;
      temp_perm = tzbsp_vmids_to_perm(rvmid >> 16, wvmid >>16);
      rg.resPerm.Vmid_Type.multi.auVMIDPerm[1] |= temp_perm;
      if(HAL_XPU2_NO_ERROR != HAL_xpu2_ConfigResourceGroup(xpu, index, &rg))
      {
        ret = -E_FAILURE;
      }
    }
    else
    {
      /* Adding VMIDs only possible for multi vmid xpus */
      ret = -E_INVALID_ARG;
    }
  }
  else
  {
    ret = -E_INVALID_ARG;
  }

  return ret;
}

/**
 * Remove VMIDs (read/write) to any RG
 *
 * @param [in] xpu        XPU Device ID
 * @param [in] index      Resource group index
 * @param [in] rvmid      Bitmask  of read access VMIDs to be removed. 0 if no
 *                        change needed in rvmid
 * @param [in] wvmid      Bitmask of write access VMIDs to be removed. 0 if no
 *                        change need in wvmid
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */

int tzbsp_xpu_remove_vmid_perms(HAL_xpu2_XPU2Type xpu, uint32 index,
                                uint32 rvmid, uint32 wvmid)
{
  int ret = E_SUCCESS;
  uint32 temp_perm = 0;
  HAL_xpu2_ResourceGroupConfigType rg = {0};

  if(HAL_XPU2_COUNT >= xpu)
  {
    if(tzbsp_xpu_is_multi_vmid(xpu))
    {
      HAL_xpu2_GetResourceGroupConfig(xpu, index, &rg);
      rg.resPerm.Vmid_Type.multi.auVMID[0]=0xFFFFFFFF; /* Program all VMIDs */

      temp_perm = tzbsp_vmids_to_perm(rvmid, wvmid);
      rg.resPerm.Vmid_Type.multi.auVMIDPerm[0] &= ~temp_perm;
      temp_perm = tzbsp_vmids_to_perm(rvmid >> 16, wvmid >>16);
      rg.resPerm.Vmid_Type.multi.auVMIDPerm[1] &= ~temp_perm;
      if(HAL_XPU2_NO_ERROR != HAL_xpu2_ConfigResourceGroup(xpu, index, &rg))
      {
        ret = -E_FAILURE;
      }
    }
    else
    {
      /* Removing VMIDs only possible for multi vmid xpus */
      ret = -E_INVALID_ARG;
    }
  }
  else
  {
    ret = -E_INVALID_ARG;
  }

  return ret;
}

/**
 * Internal call to dump the contents of key XPUs for debugging purposes.
 * XPUs dumped:
 *  - BIMC MPU 0
 *  - BIMC MPU 1
 *
 * @param[in] buf   Buffer in which to place contents
 * @param[in] size  Size of the input buffer, in bytes
 *
 * @return Number of bytes written into the buffer
 *
 * @warning   Meant to be called when a fatal error occurs and system
 *            will be resetting
 */
uint32 tzbsp_xpu_debug_dump(uint8* buf, uint32 size)
{
  HAL_xpu2_ErrorOptionsConfigType opt = {0};

  if(size < TZBSP_XPU_TOTAL_DUMP_SIZE)
  {
    return 0;
  }

  /* Disable the decode error, since doing block memory copies on the 
   * configuration space will generate them */
  HAL_xpu2_GetErrorReportingConfig(HAL_XPU2_BIMC_MPU0, HAL_XPU2_CONFIG_SECURE, &opt);
  opt = opt & ~HAL_XPU2_ERROR_O_DECODE_EN;
  HAL_xpu2_ConfigErrorReporting(HAL_XPU2_BIMC_MPU0, HAL_XPU2_CONFIG_SECURE, opt);
  HAL_xpu2_ConfigErrorReporting(HAL_XPU2_BIMC_MPU1, HAL_XPU2_CONFIG_SECURE, opt);

  /* BIMC MPU 0 */
  memcpy((void*)buf, (void*)HWIO_BIMC_S_DDR0_XPU_SCR_ADDR, TZBSP_XPU_BIMC_MPU_DUMP_SIZE);
  buf += TZBSP_XPU_BIMC_MPU_DUMP_SIZE;

  return TZBSP_XPU_TOTAL_DUMP_SIZE;
}

/**
 * System call for non-secure side to workaround MMSS security, applied by
 * QRIB, until necessary HLOS changes are put in place.
 *
 * @return E_SUCCESS if the call succeeded, error code otherwise.
 */
int tzbsp_xpu_disable_mmss_qrib(void)
{
  static boolean done = FALSE;

  if(!done)
  {
#ifndef FIXME_8916
    /* Venus & MDP client and config space is under TZ control by default */
    HWIO_OUTF(TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT, MMSS_VENUS0_VBIF_QRIB_XPU2_NSEN_INIT, 0);
    HWIO_OUTF(TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT, MMSS_VENUS0_VBIF_QRIB_XPU2_EN_TZ, 0);

    HWIO_OUTF(TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT, MMSS_VENUS0_WRAPPER_QRIB_XPU2_NSEN_INIT, 0);
    HWIO_OUTF(TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT, MMSS_VENUS0_WRAPPER_QRIB_XPU2_EN_TZ, 0);

    HWIO_OUTF(TCSR_MMSS_MDP_QRIB_XPU2_INIT, MMSS_MDP_QRIB_XPU2_NSEN_INIT, 0);
    HWIO_OUTF(TCSR_MMSS_MDP_QRIB_XPU2_INIT, MMSS_MDP_QRIB_XPU2_EN_TZ, 0);

    HWIO_OUTF(TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT, MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT, 0);
    HWIO_OUTF(TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT, MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_EN_HV, 0);

    HWIO_OUTF(TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT, MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT, 0);
    HWIO_OUTF(TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT, MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_EN_HV, 0);

    HWIO_OUTF(TCSR_MMSS_MDP_QRIB_XPU2_VMIDEN_INIT, MMSS_MDP_QRIB_XPU2_VMIDEN_INIT, 0);
    HWIO_OUTF(TCSR_MMSS_MDP_QRIB_XPU2_VMIDEN_INIT, MMSS_MDP_QRIB_XPU2_VMIDEN_INIT_EN_HV, 0);
#endif

    done = TRUE;
  }

  return E_SUCCESS;
}

/* Syscall API to enable/disable xpu violations becoming error fatals
 *  This can also be used to check if xpu violations are currently error fatals
 *  or not by sending TZ_XPU_VIOLATION_ERR_FATAL_NOOP as the parameter
 *  TZ may choose to force enable/disable xpu violations as error fatals by
 *  default. It may also choose to disallow HLOS from overriding this
 *  configuration. So, HLOS must always check the output parameter (enabled)
 *  to see the current behavior and the return code to see if it was allowed to
 *  carry out the operation (-E_NOT_SUPPORTED is returned if HLOS isn't allowed
 *  to change the behavior).
 *
 * @param [in] config     Enable/Disable/Noop
 * @param [in] spare      Unused
 *
 * @param[out] status     1-> violations are err fatals, 0->violations are not
 *                        err fatals
 *
 * @param [in] buf_size_sz   Size of the response buffer.
 *
 * @return  Zero on success.
 *          Negative error code otherwise.
 */
int tzbsp_xpu_config_violation_err_fatal(
                                   tz_xpu_violation_err_fatal_t  config,
                                   uint32                           spare,
                                   tz_xpu_violation_err_fatal_t* status,
                                   uint32                           buf_size_sz)
{
  if(buf_size_sz < sizeof(uint32))
  {
    return -E_INVALID_ARG;
  }

  if(!tzbsp_is_ns_range(status, sizeof(uint32)))
  {
    return -E_BAD_ADDRESS;
  }

  /*
   * Change configuration only if the config is not NOOP. If it is NOOP, then
   * we just need to report the current status
   */
  if (TZ_XPU_VIOLATION_ERR_FATAL_NOOP != config)
  {
    if (TZBSP_XPU_CFG_VIOLATION_ERR_FATAL_ALLOWED ==
       tzbsp_xpu_allow_disable_err_fatal)
    {
      if ((TZ_XPU_VIOLATION_ERR_FATAL_ENABLE == config) ||
         (TZ_XPU_VIOLATION_ERR_FATAL_DISABLE == config))
      {
        if (TZ_XPU_VIOLATION_ERR_FATAL_DISABLE == config &&
            FALSE == tzbsp_is_nonsecure_device())
        {
          return -E_INVALID_ARG;
        }
        else
        {
          tzbsp_xpu_violation_is_err_fatal = config;
        }

        /* config the SMMU fault as error fatal per the HLOS config */
        if (TZ_XPU_VIOLATION_ERR_FATAL_ENABLE == config)
        {
          smmu_fault_is_err_fatal = SMMU_FAULT_ERR_FATAL_ENABLE;
        }
        else
        {
          smmu_fault_is_err_fatal = SMMU_FAULT_ERR_FATAL_DISABLE;
        }
      }
      else
      {
        return -E_INVALID_ARG;
      }
    }
    else
    {
      return -E_NOT_SUPPORTED;
    }
  }
  *status = tzbsp_xpu_violation_is_err_fatal;
  return E_SUCCESS;
}

int tzbsp_protect_smem_partition(tzbsp_smem_protect_id_t id, uint32 start, uint32 end,
                                uint32 rvmids, uint32 wvmids)
{
  int index = 0;
  int ret = -E_INVALID_ARG;

  switch (id)
  {
    case   TZBSP_MSS_AP_SMEM:
      index = TZBSP_EBI1_RG_MSS_AP_SMD;
      break;

    case   TZBSP_MSS_WCNSS_SMEM:
      index = TZBSP_EBI1_RG_MSS_WCNSS_SMD;
      break;

    case   TZBSP_AP_WCNSS_SMEM:
      index = TZBSP_EBI1_RG_AP_WCNSS_SMD;
      break;

    case   TZBSP_SMEM_TOC:
      index = TZBSP_EBI1_RG_SMD_TOC;
      break;
  }

  if(0 != index)
  {
    ret = tzbsp_mpu_lock_area(HAL_XPU2_BIMC_MPU0, index, start, end, rvmids,
                              wvmids);
  }

  return ret;
}
