/**
 * @file tzbsp_sec_core.c
 * @brief Control all target hardware NS aware register settings
 *
 */

/*===========================================================================
 *    Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 ===========================================================================*/

/*===========================================================================
 *
 *                          EDIT HISTORY FOR FILE
 *
 *  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_sec_core.c#1 $
 *  $DateTime: 2016/06/17 14:31:11 $
 *  $Author: pwbldsvc $
 *
 *  when       who      what, where, why
 *  --------   ---      ------------------------------------
 *  10/06/10   cap      Created
 *
 *===========================================================================*/

/*===========================================================================
 *
 *           INCLUDE FILES
 *
 *=============================================================================*/
#include <HALhwio.h>

#include "tzbsp_cp15.h"
#include "tzbsp_target.h"
#include "tzbsp.h"
#include "tzbsp_errno.h"
#include "tzbsp_log.h"
#include "tzbsp_xpu_def.h"
#include "tzbsp_xpu_config.h"
#include "tzbsp_config.h"
#include "tzbsp_chipset.h"
#include "tzbsp_sys.h"
#include "tzbsp_hwio.h"
#include "tzbsp_secboot.h"
#include "tzbsp_peripheral_info.h"
#include "tzbsp_mem.h"

#define RCP15_HACR(reg) MRC15(reg, 4, c1, c1, 7)
#define WCP15_HACR(reg) MCR15(reg, 4, c1, c1, 7)

#define ANSACR_L2REMOTE  (1 << 24)
#define ANSACR_L2VR      (1 << 23)
#define ANSACR_L2PWR     (1 << 21)
#define ANSACR_L2DB      (1 << 19)
#define ANSACR_L2EH      (1 << 18)
#define ANSACR_L2LK      (1 << 17)
#define ANSACR_L2CR      (1 << 16)
#define ANSACR_CPUVR     (1 << 9)
#define ANSACR_CPUPWR    (1 << 8)
#define ANSACR_CPUDB     (1 << 5)
#define ANSACR_CPUEH     (1 << 4)
#define ANSACR_CPULKTLB  (1 << 3)
#define ANSACR_CPULKIC   (1 << 2)
#define ANSACR_CPULKDC   (1 << 1)
#define ANSACR_CPUCR     (1 << 0)

#define APCS_APC_SECURE_TST_NS      (1 << 2)

#define APCS_WDOG_SECURE_NS_ALL     (HWIO_APCS_WDOG_SECURE_RMSK)
#define APCS_APC_SECURE_NS_ALL      (HWIO_APCS_ALIAS0_APC_SECURE_RMSK)
#define APCS_GLB_SECURE_NS_ALL      (HWIO_APCS_GLB_SECURE_RMSK)
#define APCS_L2_SAW2_SECURE_NS_ALL  (HWIO_APCS_L2_SAW2_SECURE_RMSK)
#define APCS_SH_PLL_SECURE_NS_ALL   (HWIO_APCS_SH_PLL_SECURE_RMSK)
#define APCS_SAW2_SECURE_NS_ALL     (HWIO_APCS_ALIAS0_SAW2_SECURE_RMSK)
#define APCS_PLL_SECURE_NS_ALL      (HWIO_APCS_ALIAS0_PLL_SECURE_RMSK)

#define HWIO_MERGE(var, name, xx) \
  (var) |= ((xx) << HWIO_##name##_SHFT) & (HWIO_##name##_BMSK)

#define TZBSP_RETCHK(xx) if ((xx) != E_SUCCESS) {return (xx);}

// Warm boot reset vector, exported from tzbsp_entry.s
extern void *RST_VEC;
extern uint8 tzbsp_milestone_complete;

// Function to write the CNTHCTL and CNTFRQ registers
#if defined(TZBSP_VIRTIO)
extern void tzbsp_set_cnthctl( void );
#endif
extern void tzbsp_set_cntfrq( uint32 freq );

void tzbsp_set_boot_remap( void );

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/* Debug flag is stored in non-cached memory as L0/L1/L2 dumping features use
 * function \c tzbsp_allow_cache_dumping. */
__attribute__((section ("TZBSP_UNCACHED_ZI_VARS"))) uint32 debug_flag;

static boolean tzbsp_allow_jtag_log       = FALSE;
static boolean tzbsp_allow_jtag_log_init  = FALSE;

static uint32  tzbsp_spiden_disable      = FALSE;
static boolean tzbsp_spiden_disable_init = FALSE;

/** Sets the one-time writable debug OVERRIDE fuses */
static void tzbsp_dbg_fuse_override(uint32 debug)
{
  uint32 override_2 = 0;
  uint32 override_3 = 0;

  if (debug)
  {
    override_2 |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_DAP_DEVICEEN_DISABLE));
    override_2 |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_DAP_NIDEN_DISABLE));
    override_2 |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_DAP_DBGEN_DISABLE));
    override_2 |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_APPS_NIDEN_DISABLE));
    override_2 |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_APPS_DBGEN_DISABLE));
    override_2 |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_SPARE1_DISABLE));
    override_2 |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_SPARE0_DISABLE));
    override_2 |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_VENUS_0_DBGEN_DISABLE));
    override_2 |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_RPM_DAPEN_DISABLE));
    override_2 |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_RPM_WCSS_NIDEN_DISABLE));
    override_2 |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_RPM_DBGEN_DISABLE));
    override_2 |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_WCSS_DBGEN_DISABLE));

    override_3 |= 1 << (HWIO_SHFT(OVERRIDE_3, OVRID_DAP_SPNIDEN_DISABLE));
    override_3 |= 1 << (HWIO_SHFT(OVERRIDE_3, OVRID_DAP_SPIDEN_DISABLE));
    override_3 |= 1 << (HWIO_SHFT(OVERRIDE_3, OVRID_APPS_SPNIDEN_DISABLE));
    override_3 |= 1 << (HWIO_SHFT(OVERRIDE_3, OVRID_APPS_SPIDEN_DISABLE));
    override_3 |= 1 << (HWIO_SHFT(OVERRIDE_3, OVRID_SPDM_SECURE_MODE));

  }

  tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL);

  HWIO_OUT(OVERRIDE_2, override_2);
  HWIO_OUT(OVERRIDE_3, override_3);

  tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL);
}

/* Security Control contains the ACC, QFPROM and SMC (Security Mode Control).
 * SMC contains the SW configuration registers.
 *
 * In SMC FEATURE_CONFIG* contain all of debug disables.
 */
void tzbsp_security_control_config(uint32 debug)
{
  volatile uint32 val;  /* Volatile so compiler doesn't optimize out HWIO_IN/OUT below */

  /* The following writable Security Control registers are currently ignored by
   * TZ:
   *   - OVERRIDE_1
   *   - CAPT_SEC_GPIO
   *   - APP_PROC_CFG, the expectation is that this will come outside TZ and is
   *     hard-coded (i.e. earlier in the boot chain).
   *   - MSS_PROC_CFG TODO How should this be configured?
   *   - QFPROM_CLKCTL, Security Control clock is enabled (reset value). There is
   *     now HW clock gating for Security Control and the clock gating here is
   *     purely SW controlled. As multiple masters in the system need to
   *     independently read fuse values, this clock gating cannot really be used.
   *   - TEST_BUS_SEL, this is a Surf related debug feature.
   *   - BOOT_PARTITION_SELECT
   */

  tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL);

  /* Full chip power collapse isn't supported. Read and write \c CAPT_SEC_GPIO
   * to lock down its one-time writable value. */
  val = HWIO_IN(CAPT_SEC_GPIO);
  HWIO_OUT(CAPT_SEC_GPIO, val);

  /* OVERRIDE_0, all overrides disabled. */
  HWIO_OUT(OVERRIDE_0, 0);

  tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL);

  tzbsp_dbg_fuse_override(debug);

  tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL);

  /* SPDM_DYN_SECURE_MODE, full monitoring allowed only during debug. */
  if(debug)
  {
    /* SECURE_MODE=0, disabled, full monitoring allowed. */
    HWIO_OUTF(SPDM_DYN_SECURE_MODE, SECURE_MODE, 0);
  }
  else
  {
    /* SECURE_MODE=1, enabled, reduced monitoring. */
    HWIO_OUTF(SPDM_DYN_SECURE_MODE, SECURE_MODE, 1);
  }

  tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL);
}

/*
 * Configure the security of the Krait Subsystem (ACPS)
 *
 * The KPSS is responsible for:
 *  - CPU Clock control/distribution (acc).
 *  - Global CPU Clock control/distribution (gcc)
 *  - General Purpose Timer(s) (tmr configured as general purpose)
 *  - Debug Timer (tmr configured for debug)
 *  - Watch Dog timer(s) (wdt)
 *  - System Interrupt Controller (SIC or QGIC or External)
 *  - Test Interface Controller (tic).
 *  - AHB Bus Interface and decoder (ahb).
 *  - Per CPU voltage control (avs)
 *  - Per CPU power control (spm)
 */
static void tzbsp_set_scss(uint32 debug)
{
  register uint32 val = APCS_APC_SECURE_NS_ALL;

   /* Configuration that need to do only once. This registers are Cx Domain & retain state */
  if(!tzbsp_milestone_complete)
  {
  HWIO_OUT(APCS_WDOG_SECURE, APCS_WDOG_SECURE_NS_ALL);

  if (!debug)
  {
    val &= ~APCS_APC_SECURE_TST_NS;
  }

  HWIO_OUT(APCS_ALIAS0_APC_SECURE, val);
  HWIO_OUT(APCS_ALIAS1_APC_SECURE, val);
  if(g_tzbsp_cpu_count == 4)
  {
    HWIO_OUT(APCS_ALIAS2_APC_SECURE, val);
    HWIO_OUT(APCS_ALIAS3_APC_SECURE, val);
  }

  HWIO_OUT(APCS_GLB_SECURE, APCS_GLB_SECURE_NS_ALL);

  HWIO_OUT(APCS_L2_SAW2_SECURE, APCS_L2_SAW2_SECURE_NS_ALL);
  HWIO_OUT(APCS_SH_PLL_SECURE, APCS_SH_PLL_SECURE_NS_ALL);

  HWIO_OUT(APCS_ALIAS0_SAW2_SECURE, APCS_SAW2_SECURE_NS_ALL);
  HWIO_OUT(APCS_ALIAS1_SAW2_SECURE, APCS_SAW2_SECURE_NS_ALL);
  if(g_tzbsp_cpu_count == 4)
  {
    HWIO_OUT(APCS_ALIAS2_SAW2_SECURE, APCS_SAW2_SECURE_NS_ALL);
    HWIO_OUT(APCS_ALIAS3_SAW2_SECURE, APCS_SAW2_SECURE_NS_ALL);
  }
  /* Krait start address can only be changed by secure side. */
  HWIO_OUTF(APCS_CFG_SECURE, GLB_CTL, 0x0);

  /* Set the start address to boot from trustzone */
  tzbsp_set_boot_remap();
}
}

void tzbsp_set_boot_remap( void )
{
   uint32 val = (uint32)&RST_VEC;
   val |= (HWIO_APCS_START_ADDR_SEC_REMAP_EN_BMSK );
   
   HWIO_OUT(APCS_START_ADDR_SEC, val); 
   

}

void tzbsp_disable_l2_as_tcm( void )
{
   HWIO_OUTF(APCS_TCM_START_ADDR,TCM_REDIRECT_EN_0,0);
}

/* Configure access to, and set-up, the CP15 counter module */
static void tzbsp_timer_security_config( void )
{
  /* Allow access to the PL1 Timer module
   * NOTE: This likely a Virtio requirement since PL2 _shouldn't_
   *       be implemented
   */
#if defined(TZBSP_VIRTIO)
  tzbsp_set_cnthctl();
#endif

  /* CNTFRQ can only be written by Secure PL1; set it here */
  tzbsp_set_cntfrq(HWIO_IN(APCS_QTMR_AC_CNTFRQ));
}

/* Configure access to, and set-up, QTIMER counter module */
void tzbsp_qtimer_reconfig( void )
{
   /* Configuration that need to do only once. This registers are Cx Domain & retain state */
  if(!tzbsp_milestone_complete)
  {
  /* Configure Qtimer Counter Secure Reg to allow QTMR access*/
  HWIO_OUT(APCS_QTMR_AC_CNTSR, 0x7B);

  /* Set Qtimer CNTVOFF Reg to 0 for all 7 frames */
  HWIO_OUTI(APCS_QTMR_AC_CNTACR_n, 0, 0x3F);
  HWIO_OUTI(APCS_QTMR_AC_CNTACR_n, 1, 0x3F);
  HWIO_OUTI(APCS_QTMR_AC_CNTACR_n, 3, 0x3F);
  HWIO_OUTI(APCS_QTMR_AC_CNTACR_n, 4, 0x3F);
  HWIO_OUTI(APCS_QTMR_AC_CNTACR_n, 5, 0x3F);
  HWIO_OUTI(APCS_QTMR_AC_CNTACR_n, 6, 0x3F);

  /* Set Qtimer CNTVOFF Reg to 0 for all 7 frames */
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_LO_n, 0, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_HI_n, 0, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_LO_n, 1, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_HI_n, 1, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_LO_n, 2, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_HI_n, 2, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_LO_n, 3, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_HI_n, 3, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_LO_n, 4, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_HI_n, 4, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_LO_n, 5, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_HI_n, 5, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_LO_n, 6, 0x0);
  HWIO_OUTI(APCS_QTMR_AC_CNTVOFF_HI_n, 6, 0x0);
}
}

int tzbsp_sec_processor_target(uint32 debug)
{
  debug_flag = debug;

  tzbsp_timer_security_config();

  tzbsp_set_scss(debug);

  return 0;
}

/* Compile these functions for ARM instructions to allow CP15 access from
 * inline assembly. */
#pragma arm

/*
 * Checking if memory can be dumped in case of Modem SSR
 * Allowed if:
 * if JTAG enable is ON in the cert
 * if secboot is disabled
 * if DEBUG FUSE (SPIDEN) is not blown
 *
 * @note This function is called in contexts where cached global variables are
 *       not allowed. Variable \debug_flag is linked into non-cached area.
 */
boolean tzbsp_allow_unlock_xpu(void)
{
  boolean retval = FALSE;

  if (!tzbsp_spiden_disable_init)
  {
    tzbsp_spiden_disable_init = TRUE;

    tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_QFPROM_RAW);
    tzbsp_spiden_disable =
      HWIO_INF(QFPROM_RAW_OEM_CONFIG_ROW0_MSB, APPS_SPIDEN_DISABLE);
    tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_QFPROM_RAW);
  }

  /* By default memory dumping is denied. */
  retval = (debug_flag ||
            (!tzbsp_secboot_hw_is_auth_enabled(1) && !tzbsp_spiden_disable));

  return retval;
}

/**
 * Checks whether cache dumping should be allowed. Currently the rules for this
 * are the same as for XPU unlocking.
 *
 * @note This function is called in contexts where cached global variables are
 *       not allowed.
 */
boolean tzbsp_allow_cache_dumping(void)
{
  return tzbsp_allow_unlock_xpu();
}

/**
 * Checks whether non-secure debug FIQ should be allowed. Currently the rules
 * for this are the same as for XPU unlocking.
 */
boolean tzbsp_allow_ns_debug_fiq(void)
{
  return tzbsp_allow_unlock_xpu();
}

/**
 * Checks if HLOS can affect the behaviour of the secure watchdog; currently
 * the rules for this are the same as XPU unlocking.
 */
boolean tzbsp_allow_sec_wdog_debug(void)
{
  return tzbsp_allow_unlock_xpu();
}

/* @return TRUE if JTAG logging is allowed, FALSE otherwise. */
boolean tzbsp_allow_jtag_logging(void)
{
  if(!tzbsp_allow_jtag_log_init)
  {
    tzbsp_allow_jtag_log_init = TRUE;

    tzbsp_mem_map_device(TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL);

    /* If debug disable fuses have been blown, JTAG logging makes no sense. */
    if(!HWIO_INF(OEM_CONFIG1, ALL_DEBUG_DISABLE) &&
       !HWIO_INF(FEATURE_CONFIG2, QC_APPS_DBGEN_DISABLE) &&
       !HWIO_INF(OEM_CONFIG1, APPS_DBGEN_DISABLE) &&
       !HWIO_INF(FEATURE_CONFIG2, QC_APPS_SPIDEN_DISABLE) &&
       !HWIO_INF(OEM_CONFIG1, APPS_SPIDEN_DISABLE))
    {
      tzbsp_allow_jtag_log = TRUE;
    }

    tzbsp_mem_unmap_device(TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL);
  }

  return tzbsp_allow_jtag_log;
}

/**
 * Wrapper function for tzbsp_dbg_fuse_override() for TZ kernel
 */
int tzbsp_jtag_override(uint32 debug)
{
  tzbsp_dbg_fuse_override(debug);
  return E_SUCCESS;
}

/**
 * Wrapper function to release HLOS HW lock taken by the calling core
 * before making monitor power collapse call. HW Mutex #7 is used to 
 * provide synchronization between TZ and HLOS.
 */
void tzbsp_release_hw_lock(void)
{
  HWIO_OUTI(TCSR_MUTEX_REG_n, 7, 0);
}

/**
 * In 8x26 the bit L2_RST_DIS and PMIC_APC_ON is set by RPM at the boot up as RPM was the boot processor. 
 * But in case of 8909 APSS is the boot processor and this bit is not set by 
 * anyone. BootFSM is not setting this bit to 1. So design team recommends to 
 * set this bit once at the cold boot.
 * 
 */
void tzbsp_disable_l2_rst( void )
{
   HWIO_OUTF(APCS_L2_PWR_CTL, L2_RST_DIS, 1);

   /* PMIC_APC_ON bit  was set by RPM in 8x26 APPS core being the first core on 8909 we are enabling this bit in TZ
    * This BIT is required for the complete SPM sequence to work */

   HWIO_OUTF(APCS_L2_PWR_CTL, PMIC_APC_ON ,1);
}
