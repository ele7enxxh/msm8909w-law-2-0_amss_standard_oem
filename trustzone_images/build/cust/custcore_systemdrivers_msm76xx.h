#ifndef CUSTSYSDRV_MSM76XX_H
#define CUSTSYSDRV_MSM76XX_H
/*=========================================================================== 

                 C U S T O M E R   D E F I N I T I O N S

                       S Y S T E M   D R I V E R S

                             M S M 7 6 X X

DESCRIPTION
  Customer definition file for system drivers on the MSM76XX branch.

  Copyright (c) 2008 by Qualcomm Technologies Incorporated. All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custcore_systemdrivers_msm76xx.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/09   gfr     Added FEATURE_HWIO_QUARTZ_XML_MAPPINGS
01/28/09   gfr     Added TRAMP mappings for other clients.
11/25/08   gfr     Use FEATURE_TURBOMODE to enable turbo mode.  Do not boot
                   at 528 MHz for UNDP.
11/18/08   gfr     Remove extra turbo mode define.
11/14/08   jz      ROC 2.0.
09/05/08   gfr     Remove need for tramp features, updated clock regime
                   frequency plan definitions for turbo ADSP on Roc.
08/11/08   gfr     Initial revision.
===========================================================================*/


/*-------------------------------------------------------------------------*/
/* CLKREGIM                                                                */
/*-------------------------------------------------------------------------*/

/*
 * Enable the resource manager.
 */
#define FEATURE_CLKREGIM_RM

/*
 * Enable dynamic clock switching on ARM11
 */
#if !defined(FEATURE_WINCE_BOOTLOADER) && !defined(FEATURE_WINCE_OAL)
  #define FEATURE_ACPU_DYNAMIC_CLOCK_VOLTAGE_SCALING
  #define FEATURE_DCVS_APPS
#endif

/*
 * Define this to use 20 us instead of 50 us wait when switching the
 * ARM11 clock.  This only works on FFA though.
 */
#ifdef T_FFA
  #define FEATURE_CLKREGIM_FAST_ACPU_SWITCH_TIME 
#endif

/*
 * Enable dynamic clock switching on ARM9
 */
#define FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING

/*
 * Enable clock regime to do dynamic mDSP clock switching.
 */
#define FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED

/*
 * Leave ARM11 on TCXO after warm boot and let clock regime restore the
 * original speed.  Otherwise the SBL switchs to 384 MHz on GPLL, which
 * will crash if ARM11 was previously running on MPLL.
 */
#define FEATURE_APPSBL_WARM_BOOT_ON_TCXO

/*
 * Define the extended uP sleep API.
 */
#ifndef FEATURE_WINCE
  #define CLK_REGIME_UP_SLEEP__MCLK_HCLK
#endif

/*
 * Ramp down clocks to TCXO during sleep if everyone is off.
 */
#define FEATURE_CLKREGIM_ARM_SLEEP_AT_TCXO

/*
 * Use MPLL for the camera clock when possible.
 */
#define FEATURE_CLKREGIM_CAM_VFE_USE_MPLL

/*
 * Allow ADSP to control the AXI interconnect.
 */
#define FEATURE_CLKREGIM_ADSP_AXI_CONTROL

/*
 * Boot up at 528 MHz unless we are using UNDP which does not support
 * this rate.
 */
#ifndef T_UNDP
  #define FEATURE_CLKREGIM_ACPU_BOOT_528_MHZ
  #define FEATURE_WM_BOOT_ACPU_SPEED_528MHZ
#endif


/*-------------------------------------------------------------------------*/
/* HW                                                                      */
/*-------------------------------------------------------------------------*/

/*
 * Need this for TMC to call hw_init.
 */
#define FEATURE_HW_INIT

/*
 * Support for L4 split hwio mappings.
 */
#ifdef FEATURE_L4
  #define FEATURE_HW_SPLIT_MAPPINGS
  #define FEATURE_HWIO_XML_MAPPINGS
#endif


/*-------------------------------------------------------------------------*/
/* TLMM                                                                    */
/*-------------------------------------------------------------------------*/

/*
 * On FFA we configure GPIOs to low power mode during sleep
 */
#if defined(T_FFA) && !defined(BUILD_BOOT_CHAIN)
  #define FEATURE_MANAGE_GPIOS_FOR_SLEEP
  #define FEATURE_APPS_GPIOS_LOW_POWER
#endif


/*-------------------------------------------------------------------------*/
/* TRAMP                                                                   */
/*-------------------------------------------------------------------------*/


/*
 * Define some target specific mappings.
 */
#define TRAMP_SLEEP_INT1_ISR   TRAMP_SLEEP_1X_IRQ
#define TRAMP_MDSP_EUL_ISR     TRAMP_MDSP_1_IRQ
#define USB_INT                TRAMP_USB_HS_IRQ

#ifdef IMAGE_APPS_PROC
  #define TRAMP_SLP_TICK_ISR   TRAMP_GP_TIMER_IRQ
#else
  #define TRAMP_SLP_TICK_ISR   TRAMP_SLEEP_TIMETICK_IRQ
#endif


#endif /* CUSTSYSDRV_MSM76XX_H */


