#ifndef CUSTDRIVERS_H
#define CUSTDRIVERS_H
/*=========================================================================== 

                           C U S T D R I V E R S

DESCRIPTION
  Customer definition file for drivers.

  Copyright (c) 2006 by Qualcomm Technologies Incorporated. All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custdrivers.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/25/08   jz      Remvoed clkregim, hw and tramp to custsysdrv_msm76xx.h.
05/30/08   gfr     Only define FEATURE_TIME_TOD_AS_RTC_OFFSET for modem.
04/24/08   gfr     Enable reset logging on WM.
04/15/08    mk     Added FEATURE_SEC_TIME_MU
04/10/08   gfr     Move boot definitions after dependent clock regime ones.
03/18/08   gfr     Move dmov definitions here.  Boot at 528 by default.
03/13/08   jz      Use 98.304MHz PLL3 and 245.76MHz PLL0 for multimode.
02/20/08   gfr     Do not enable DMOV clock gating for the tool chain.
02/01/08   gfr     Support for WinCE.
01/30/08   gfr     Set CLKREGIM_VREG_MSMC1_LEVEL_HIGH to 1.275 volts.
                   Enable mdsp clock switching for CDMA as well.
01/30/08   gfr     Set initial boot-up MSMC1 and MSMC2 voltages.
01/18/08   gfr     Define FEATURE_RF_TX_PWR_LIM_REV whenever WCDMA is defined.
01/10/08   gfr     Enable sleep-at-tcxo and ADM clock gating.  Keep
                   FEATURE_CLKREGIM_CAM_VFE_USE_MPLL off for now.
01/10/08   jz      Enable FEATURE_CLKREGIM_CAM_VFE_USE_MPLL.
12/10/07   gfr     Enable FEATURE_TSSC_ADC_CONTENTION for Linux.
10/31/07   gfr     Re-enable dmov clock optimization.
10/03/07   gfr     Do not enable GPIO sleep code in boot chain.
                   Enable the ADC RF HDET channel for CDMA.
09/26/07   gfr     Enable touchscreen for WinMobile.
09/25/07   gfr     Disable dmov clock optimization until it is fixed.
08/20/07   gfr     Added FEATURE_ADC_VOTE_FOR_SLEEP
08/07/07   gfr     Enable GPIO sleep management.
07/27/07   gfr     Only enable the transmit power limiter for UMTS builds.
07/02/07   gfr     Enable dynamic MDSP and ARM9/AXI clock changing.
06/21/07   gfr     Added FEATURE_RF_TX_PWR_LIM_REV
06/05/07   gfr     Support for split HWIO mappings.
06/01/07   gfr     Added FEATURE_DLOAD_MEM_DEBUG
05/18/07   gfr     Define CLK_REGIME_UP_SLEEP__MCLK_HCLK for both procs.
05/09/07   gfr     Added FEATURE_GPIO_INT_ISSUE_PROTECTION, recover F3 
                   features, and CLK_REGIME_UP_SLEEP__MCLK_HCLK
05/03/07   gfr     Added FEATURE_DMOV_SLEEP
04/24/07   gfr     Remove FEATURE_BIO_SWITCH_SLEEP_OFF and only define
                   MSMHW_MPM_RESET for rev A.
03/16/07   gfr     Use single_mode clock plans.  Added TMC_SET_RAN_SEED.
                   Support for dynamic ARM11 clock switching.
03/09/07   gfr     Added FEATURE_DMOV_GLOBAL_COUNT
03/08/07   gfr     Only define FEATURE_BIO_SWITCH_SLEEP_OFF for UMTS
02/27/07   gfr     Added keypad section.
02/15/07   gfr     Added FEATURE_MDSP_IOCTL.
01/03/07   gfr     Removed CLK_REGIME_DIAG.
12/11/06   gfr     Added HW bug workaround features.
12/05/06   gfr     Add FEATURE_BIO_SWITCH_SLEEP_OFF and moved some clock
                   features to clkregim.h.
11/28/06   gfr     Add FEATURE_MDSP_ATTEMPT_RECOVERY.
11/17/06   gfr     Add MSMHW_MPM_RESET
10/17/06   gfr     Added FEATURE_APPSBL_WARM_BOOT_ON_TCXO
10/09/06   gfr     Sync to 7200 custdrivers.h
09/12/06   gfr     Added FEATURE_HW_INIT and moved some features from
                   msm7600.builds
07/23/06   gfr     Pull in tramp definitions from msm7600.builds.
07/11/06   gfr     Added T_CLKREGIM_MULTI_MODE, FEATURE_CLKREGIM_RM and
                   FEATURE_TRAMP_MUX_INTR
06/08/06   gfr     Changed T_CLKREGIM to 7600
06/06/06   gfr     Add FEATURE_CLKREGIM_RM
04/26/06   gfr     Initial revision.
===========================================================================*/

#include "custcore_systemdrivers_msm76xx.h"

/*---------------------------------------------------------------------------
                           ADC
---------------------------------------------------------------------------*/

/* Enable the transmit power limiter for UMTS builds */
#ifdef FEATURE_WCDMA
  #define FEATURE_RF_TX_PWR_LIM_REV
#endif

/* Put ADC to sleep when not in use - if touchscreen is need then we
   instead enable contention resolution. */
#if defined(FEATURE_APPS_IMAGE_WINMOB) || defined(FEATURE_NATIVELINUX) || defined(FEATURE_TOUCHSCREEN)
  #define FEATURE_TSSC_ADC_CONTENTION
#else
  #define FEATURE_ADC_VOTE_FOR_SLEEP
#endif

/* Enable the RF HDET channel for CDMA */
#ifdef FEATURE_CDMA
  #define FEATURE_ADC_RF_HDET
#endif



/*---------------------------------------------------------------------------
                           BOOT
---------------------------------------------------------------------------*/

/* Define the boot-up value for MSMC1.  Due to a hardware bug on 7600 in
   the ARM11 AHB bridge we need to run at higher voltage when the ARM11
   AHB is > 96 MHz.  */
#define PM_VREG_MSMC1_LEVEL       1275

/* If we are booting at 528 MHz we need to increase the initial MSMC2 from
   the 1.2 volt default. */
#ifdef FEATURE_CLKREGIM_ACPU_BOOT_528_MHZ 
  #define PM_VREG_MSMC2_LEVEL     1325
#endif


/*---------------------------------------------------------------------------
                              DEBUG
---------------------------------------------------------------------------*/

/* Enable saving registers to memory after a reset */
#ifndef FEATURE_WINCE
  #define FEATURE_REX_CORE_DUMP
#endif

/* Enable reset logging */
#define FEATURE_DLOAD_MEM_DEBUG

/* Enable fancy error handler in the boot code */
#if !defined(BOOT_LOADER) && !defined(BUILD_HOSTDL)
  #define FEATURE_ERR_DEBUG_HANDLER
#endif

/* Enable F3 trace feature */
#if !defined(BOOT_LOADER) && !defined(BUILD_HOSTDL) && !defined(FEATURE_WINCE)
  #define FEATURE_SAVE_DEBUG_TRACE
  #define FEATURE_SAVE_TRACE_EXTENDED
  #define FEATURE_OPTIMIZE_TRACE_RECORDS
  #define FEATURE_SAVE_TRACE_ON_BY_DEFAULT
  #define FEATURE_ERR_EXTENDED_STORE
#endif

/* Enable reset logging */
#ifndef FEATURE_WINCE
  #define FEATURE_DLOAD_HW_RESET_DETECT
#endif


/*---------------------------------------------------------------------------
                              DMOV
---------------------------------------------------------------------------*/

#ifdef FEATURE_DMOV

  /* Support for dmov active client registration to enable clock */
  #define FEATURE_DMOV_REGISTRATION
  
  /* Support DMOV sleep vote */
  #ifndef FEATURE_WINCE_BOOTLOADER
    #define FEATURE_DMOV_SLEEP
  #endif
  
  /* ARM11 does not have access to secure DM region */
  #ifdef IMAGE_APPS_PROC
    #define FEATURE_DMOV_NONSECURE_API
  #endif
  
  /* Enable ADM clock gating for the modem processor */
  #ifdef IMAGE_MODEM_PROC
    #define FEATURE_DMOV_XFER_CLK_GATING
  #endif

  #ifdef IMAGE_APPS_PROC
    /* Prevent cache maintenance for the MM_HEAP2 region */
    #if defined(SCL_MM_HEAP2_SIZE) && (SCL_MM_HEAP2_SIZE > 0) &&         defined(SCL_MM_HEAP2_BASE) && !defined(FEATURE_MM_HEAP2_CACHED)
      #define DMOV_REGION_IS_CACHED(addr, len)         ((((uint32)(addr)) < DMOV_HWIO_BASE) &&          !(((uint32)(addr) >= SCL_MM_HEAP2_BASE) &&            ((uint32)(addr) + (uint32)(len)) <= (SCL_MM_HEAP2_BASE+SCL_MM_HEAP2_SIZE)))
    #endif
  #endif

  /* ARM11 ADM clock gating conflicts with L4 demand page loading and is
     not supported on WM. */
  #if defined(IMAGE_APPS_PROC) && !defined(FEATURE_L4_DEMAND_LOAD_BOOT) && \
      !defined(FEATURE_WINCE)
    /*#define FEATURE_DMOV_TIMER_CLK_DISABLE*/
    #define FEATURE_DMOV_TASK
    #define FEATURE_DMOV_SYNC_ISSUE
  #endif
  
#endif  /* FEATURE_DMOV */


/*---------------------------------------------------------------------------
                              HS
---------------------------------------------------------------------------*/

/* Enable headset polling - disabled until build issues are resolved */
#define FEATURE_HS_POLLING_ON_MODEM


/*---------------------------------------------------------------------------
                              KEYPAD
---------------------------------------------------------------------------*/

#ifndef FEATURE_THIN_UI

  /* Enable the keypad unless this is ThinUI */
  #define FEATURE_KEYPAD

#endif

#ifdef FEATURE_KEYPAD

  /* Enable keypad's sleep vote */
  #define FEATURE_KEYPAD_OKTS

  #define FEATURE_KEYPAD_MULTI_KEY
  
  #define FEATURE_KEYPAD_OWNS_KEYPRESS_ISR

#endif


/*---------------------------------------------------------------------------
                              MDSP SERVICES
---------------------------------------------------------------------------*/

/* Define this to attempt recovery instead of error fatal'ing after an
   mDSP failure */
#define FEATURE_MDSP_ATTEMPT_RECOVERY

/* Define this to support the ioctl interface.  This is used by GPS to
   workaround the MODEM_CLK_MISC_CTL_2 bug */
#define FEATURE_MDSP_IOCTL


/*---------------------------------------------------------------------------
                              SDCC
---------------------------------------------------------------------------*/

/* Let SDCC use clock regime to change clock rates */
#define FEATURE_SDCC_USE_CLKREGIM


/*---------------------------------------------------------------------------
                              TIME
---------------------------------------------------------------------------*/

/* Enable VDD minimization support */
#define FEATURE_TIME_POWER_COLLAPSE

#if !defined(BUILD_BOOT_CHAIN) && defined(IMAGE_MODEM_PROC)
  #define FEATURE_TIME_TOD_AS_RTC_OFFSET
#endif

/* Secure Time feature for 7K Targets */
#if !defined(BUILD_BOOT_CHAIN) && !defined(FEATURE_WINCE)
  #define FEATURE_SEC_TIME_MU
#endif



/*---------------------------------------------------------------------------
                              MISC
---------------------------------------------------------------------------*/

/* Enable seeding the random number generator in TMC */
#define TMC_SET_RAN_SEED



#endif /* CUSTDRIVERS_H */

