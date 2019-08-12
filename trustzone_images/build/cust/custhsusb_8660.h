#ifndef CUSTHSUSB_H
#define CUSTHSUSB_H

/*===========================================================================

DESCRIPTION
Configuration for HS-USB. MSM8660

Copyright (c) 2010  by Qualcomm Technologies Incorporated. All Rights Reserved.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custhsusb_8660.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

===========================================================================*/

/* Enables HS-USB for the target 
*/
#define FEATURE_HS_USB

/* For bring-up BMP build, the PMIC APIs are not ready. 
** This feature needs to be removed post bring-up
*/
#define FEATURE_HS_USB_DISABLE_VREGMGR

/* HS-USB PHY is integrated into the MSM
*/
#define FEATURE_HS_USB_INTEGRATED_PHY

/* Enables Mass Storage Function Driver 
*/
#define FEATURE_HS_USB_MS_FD

/* Enables Command Block Wrapper optimizations for higher 
** throughputs with Mass Storage Function Driver 
*/
#define FEATURE_HS_USB_FAST_CSW

/* Buffer size for MS is set to 64KB
*/
#define HS_USB_SCSI_BUFFER_SIZE 0x10000

/* Enables Ethernet Networking Control Model (ECM) Function 
** Driver used for RmNET 
*/
#define FEATURE_HS_USB_ECM

/* Enables HS-USB support for NMEA
*/
#define FEATURE_HS_USB_NMEA

/* Enable SW workaround for HW issue in which cable disconnect can cause 
** PHY to go into unknown state in rare cases. 
*/
#define FEATURE_HS_USB_CORE_RESET_ON_DISC

/* PHY depends on 5V VREG
*/
#define FEATURE_HS_USB_PHY_DEPENDS_ON_VREG_5V

#define HS_USB_SCSI_BUFFER_SIZE 0x10000

/* Enables the OTG feature of the HS-USB stack that may function 
** both as host and as a device
*/
#define FEATURE_HS_USB_OTG

#ifdef FEATURE_STATIC_MTP
    /* Launches MTP background BREW application when 
    ** MTP function driver (FD) gets enabled
    */
   #define FEATURE_USB_LAUNCH_MTP_BACKGROUND_APP
#endif

/* HS-USB uses the new remote charger API. the remote API is implemented 
** by the charger module. 
*/
#define FEATURE_HS_USB_CHG_REMOTE_API

/* PMIC HW support the suspend current of less 2.5mA.  The HS-USB driver will
** call PMIC API that corresponds to drawing suspend current instead of 
** calling API that programs HW to draw no current on USB.
*/
#define FEATURE_HS_USB_CHG_SUSPEND

/* HS-USB Charging feature relies on the PMIC charger flags that may be defined externally 
** in the build configuration. This dependency check is required for ThinUI build to turn 
** off the charging feature externally without modifying this header file.
*/
#if (defined(FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_TASK))
/* For BMP bring-up, disable USB charging regardless of PMIC flag */
/* #define FEATURE_HS_USB_CHG_MGR */
#endif /* FEATURE_BATTERY_CHARGER || FEATURE_CHG_TASK */

/* Enable SW workaround the stuck PHY issue. Whenver the SW detects that the HS-USB
** PHY is stuck, it will restart the HW and the SW state machines.
** 7X30 does not need this workaround.
*/
#undef FEATURE_HS_USB_CORE_RESTART_ON_STUCK_PHY

/* uses HSU LPM which links in hsu_lpm.c for power management 
*/
#define FEATURE_HS_USB_USE_HSU_LPM

/* Enable HS-USB serial layer statistics
 */
#define FEATURE_HS_USB_SER_STATISTICS

/*==============================================================================

                          HS USB HW Configuration Parameters

 ==============================================================================*/
/* PMIC ID base for 8K PMIC.
*/
#define HSU_CFG_PMIC_8K 8000
    
/* USB CI link has usb system clock for protocol engine.
*/
#define HSU_CFG_HAS_USB_SYSTEM_CLK

/* PHY PLL sleeps in suspend mode automatically.
*/
#define HSU_CFG_PHY_PLL_AUTO_SLEEP

/* External LDO is used for 3.3V VDDA on internal HS USB PHY. 
*/
#define HSU_CFG_IS_EXT_LDO_INSTALLED      (FALSE)

/* ID signal is wired to PMIC ID comparator.
*/
#define HSU_CFG_IS_PMIC_ID_PIN_WIRED      (FALSE)

/* VBUS signal is wired to PMIC VBUS comparator.
*/
#define HSU_CFG_IS_PMIC_VBUS_WIRED        (FALSE)

/* PMIC VREG ID for 3.3 V input in internal HS USB PHY
*/
#define HSU_CFG_PMIC_VREG_USB_ID  (PM_VREG_USB_3P3_ID)

/* PMIC VREG ID for 1.8 V input in internal HS USB PHY
*/
#define HSU_CFG_PMIC_VREG_1P8_ID (PM_VREG_USB1P8_ID)

/* VREG USB voltage
*/
#define HSU_CFG_VREG_USB_VOLT_LVL (3075)

/* PMIC ID. 
*/
#define HSU_CFG_PMIC_ID 8028

/* VREG Lower Bound, lower bound that allows smooth voltage transition */
#define HSU_CFG_VREG_USB_VOLT_LVL_LO_BOUND (3100)
/* VREG Upper Bound */
#define HSU_CFG_VREG_USB_VOLT_LVL_UP_BOUND (3600)

/* Application CPU clock speeds.
*/
#define HSU_CFG_CLKMGR_ACPU_HIGH_SPEED   (0xFFFFFFFF)
#define HSU_CFG_CLKMGR_ACPU_MID_SPEED   (500)
#define HSU_CFG_CLKMGR_ACPU_LOW_SPEED   (0)

/* AHB bus clock speeds.
*/
#define HSU_CFG_CLKMGR_AHB_HIGH_SPEED    (0xFFFFFFFF)  
#define HSU_CFG_CLKMGR_AHB_MID_SPEED    (500)
#define HSU_CFG_CLKMGR_AHB_LOW_SPEED    (0)

/* Main system bus speeds.
*/
#define HSU_CFG_CLKMGR_HBUS_HIGH_SPEED   (0xFFFFFFFF)
#define HSU_CFG_CLKMGR_HBUS_MID_SPEED   (500)
#define HSU_CFG_CLKMGR_HBUS_LOW_SPEED   (0)

/* CLKRGM ID for HS USB peripheral clock. 
*/
#ifdef HSU_CLKMGR_PCLK
#define HSU_CFG_CLKMGR_PCLK HSU_CLKMGR_PCLK  
#else /* HSU_CLKMGR_PCLK */
#define HSU_CFG_CLKMGR_PCLK CLKRGM_USB_HS1_PBDG_CLK  
#endif /* !HSU_CLKMGR_PCLK */

#define HSU_CFG_CLKMGR_SCLK CLKRGM_USB_HS1_CORE_CLK

/* Address where hsu_dload_info_type data structure is stored for DLOAD. 
** TODO: Should be changed to BOOT_DLOAD_ID_ADDR
*/
/* define SHARED_IMEM_USB_BASE ((SHARED_IMEM_BOOT_BASE) + SHARED_IMEM_BOOT_SIZE) in builds file*/
#define HS_USB_DLOAD_INFO_ADDR SHARED_IMEM_USB_BASE 

/* Use charger pin on PMIC to detect VBUS
*/
//#define HSU_CFG_PMIC_CHG_PIN_FOR_VBUS_DETECT (TRUE)

/* Use PMIC secure interrupt API
*/
#ifdef HSU_CFG_PMIC_CHG_PIN_FOR_VBUS_DETECT
   /* 7x30 target, which uses PMIC secure interrupt model,  
   ** should enable HSU_CFG_PMIC_SEC_INTERRUPT 
   */
  #define HSU_CFG_PMIC_SEC_INTERRUPT
#endif /* HSU_CFG_PMIC_CHG_PIN_FOR_VBUS_DETECT */

/*============================================================================

                                                                           QHSUSB defnes

 ============================================================================*/
/* Enable Dead Battery Charging feature in QHSUSB stack */
#define FEATURE_QHSUSB_DEAD_BATTERY_CHARGING

/** The number of all cores (host and peripheral) in all possible
configurations. This constant is used for local array allocation
to be supplied to Jungo on stack startup.
*/
#define HSU_MAX_NUM_OF_CORES 3

/** HS-USB core index. */
#define HSU_CORE_CONFIG_HS_MAIN_IDX   0  
/** UICC core index. */
#define HSU_CORE_CONFIG_UICC_IDX      1
/** HS-USB secondary core index. */
#define HSU_CORE_CONFIG_SECONDARY_IDX 1
/** HSIC core index. */
#define HSU_CORE_CONFIG_HSIC_IDX      2
/** USB3.0 core index. */
#define HSU_CORE_CONFIG_USB3_IDX      4

#ifdef FEATURE_HSIC
  #define FEATURE_HS_USB_HSIC
#endif /* FEATURE_HSIC */

#endif /* CUSTHSUSB_H */
