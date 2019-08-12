/*! \file
 *  
 *  \brief  
 *   pm_config_registers.c
 *  \details 
 *   This file contains customizable target specific 
 *   driver settings & PMIC registers. This file is generated from database functional
 *   configuration information that is maintained for each of the targets.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Resource Setting Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Software Register Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Processor Allocation Information Version: VU.Please Provide Valid Label - Not Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2010-2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/config/msm8909/pm8916/pm_config_target.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/17/14   rk       Modify XPU permission and Int Ownership for PMIC Peripherals (CR - 666063)
12/19/13   rk      Added spmi channel config setings
10/08/13   rk      Added target specific configurations
07/15/13   aab     Added support for SMBB driver
05/09/13   aab     Updated MPP driver support 
03/26/13   aab     Updated GPIO driver support  
03/07/13   aab     Added VIB driver support
01/18/13   aab     Cleanup
01/09/13   aab     Added support for sleep clk
12/06/12   hw      Rearchitecturing module driver to peripheral driver
08/07/12   hs      Added support for 5V boost.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include"pm_gpio_driver.h"
#include"pm_mpp_driver.h"
#include"pm_clk_driver.h"
#include"pm_vib_driver.h"
#include"pm_smb_driver.h"
#include "pm_pbs_info.h"
#include "SpmiCfg.h"
#include "pm_led.h"
#include "DALDeviceId.h"


 /************************************************ DRIVER DATA *********************************/
/*
 * pm_periph_intr_owner
 *
 * A type to choose the interrupt owner for the peripheral.
 */
typedef enum {
  PM_APPS_HLOS_OWNER = 0,
  PM_APPS_TZ_OWNER,
  PM_MSS_OWNER,
  PM_LPASS_OWNER,
  PM_RPM_OWNER,
  PM_WCONNECT_OWNER
} pm_periph_intr_owner;


//===== LUT ====================================================
pm_led_config_type pm8916_led_array[] =
{   // pm_led_type,   pm_led_channel,          level 
    {PM_LED_ATC, PM_LED_CH_INVALID, 0},
    {PM_LED_KPD, PM_LED_CH_MPP2, PM_MPP__I_SINK__LEVEL_40mA},
//    {PM_LED_RED, PM_LED_CH_RED, PM_RGB_DIM_LEVEL_MID},
//    {PM_LED_GREEN, PM_LED_CH_GREEN, PM_RGB_DIM_LEVEL_MID},
//    {PM_LED_BLUE, PM_LED_CH_BLUE, PM_RGB_DIM_LEVEL_MID},
};

const pm_led_type pm8916_default_led = PM_LED_ATC;

// This array is used form tri-color LED connected to RGB module only.
// For tri-color LED which is connect to led_array, search in led_array
// Do NOT suppose this array size must be 3. Customers may use dual color LED but connect to RGB module
pm_led_rgb_config_type pm8916_rgb_configs[] =
{
    // rgb mask bit          LPG channel         power source
    {PM_RGB_SEGMENT_R, PM_LPG_CHAN_7, PM_RGB_VOLTAGE_SOURCE_VPH},
    {PM_RGB_SEGMENT_G, PM_LPG_CHAN_6, PM_RGB_VOLTAGE_SOURCE_VPH},
    {PM_RGB_SEGMENT_B, PM_LPG_CHAN_5, PM_RGB_VOLTAGE_SOURCE_VPH},
};

pm_led_info_type pm8916_led_info = 
{
    pm8916_led_array,
    pm8916_rgb_configs,
    pm8916_default_led
};

// maybe used in the future if multiple smb chips are use to charge together
pm_smb_index_type pm8916_smb_index_array[] =
{
    SMB_INDEX_1360,
//    SMB_INDEX_358,
    SMB_INDEX_INVALID
};

pm_smb_hw_info_type pm8916_smb_hw_info =
{
    0x14,
    DALDEVICEID_I2C_DEVICE_4,
    400,
    2500,
    SMB_INDEX_1360
};

unsigned pm8916_smps_phase_lut_a[ ]= {0,0,0,0};


pm_mpp_specific_info_type
pm8916_mpp_specific[1] =
{
   {0x9E0, 4}
};

pm_vib_specific_data_type
pm8916_vib_specific[1] = 
{
  {1200, 3100}
};

uint16 pm8916_chg_app_ds[] = 
{
   3400,  // PM_CHG_FLCB_OS_BOOT_UP_THRESHOLD
   3206,  // PM_CHG_FLCB_WEAK_BATTERY_THRESHOLD, in the unit of mv
      0,  // PM_CHG_FLCB_FAST_CHG_THRESHOLD, not applicable       
   2796,  // PM_CHG_FLCB_DEAD_BATTERY_THRESHOLD, in the unit of mv
      0,  // PM_CHG_DC_CHARGER_IMAX, not applicable
   1440,  // PM_CHG_IMAX_DEFAULT
   1440,  // PM_CHG_USB_IDEV_DCHG
   1440,  // PM_CHG_USB_IDEV_FLOAT
    500,  // PM_CHG_USB_IDEV_SDP
     90,  // PM_CHG_USB_IUNIT
    450,  // PM_CHG_USB_IBAT_ATC_B_NORMAL
   4775,  // PM_CHG_VBAT_MAX_MAX
   4000,  // PM_CHG_VBAT_MAX_MIN
     25,  // PM_CHG_VBAT_MAX_STEP
   1440,  // PM_CHG_IBAT_MAX_MAX
     90,  // PM_CHG_IBAT_MAX_MIN
     90,  // PM_CHG_IBAT_MAX_STEP
   1440,  // PM_CHG_IBAT_SAFE
   4200,  // PM_CHG_VIN_MIN
   2500,  // PM_CHG_VBAT_TRKL_MIN_THRESHOLD, in the unit of mv
   2985,  // PM_CHG_VBAT_TRKL_MAX_THRESHOLD, in the unit of mv
   3000,  // PM_CHG_VBAT_WEAK_MIN_THRESHOLD, in the unit of mv
   3582,  // PM_CHG_VBAT_WEAK_MAX_THRESHOLD, in the unit of mv
   3500,  // PM_CHG_FLCB_OS_BOOTUP_AFTER_USB_SUSPEND_THRESHOLD.  Used when PC is in suspend state to insure that the next boot up can make it to UEFI/LK
      0   // PM_CHG_PMIC_DEVICE_INDEX
};



/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/

const SpmiCfg_ChannelCfg  pm8916_pm_spmi_periph_cfg[] =
{
    {0xF, 0x00, 116, PM_APPS_TZ_OWNER},     /* GID            */
    /* Write broadcast channels for INT and SPMI. */
    {0xE, 0x08, 99 , PM_APPS_HLOS_OWNER},   /* PON            */
    {0xE, 0x06, 118, PM_RPM_OWNER},     /* SPMI          */
    {0xE, 0x05, 119, PM_RPM_OWNER},     /* INT           */	
    {0x2, 0x06, 55, PM_RPM_OWNER},         /* SPMI_OPTIONS   */
    {0x1, 0xF3, 24,  PM_APPS_HLOS_OWNER},   /* CDC_NCP_FREQ    */
    {0x1, 0xF2, 23,  PM_APPS_HLOS_OWNER},   /* CDC_BOOST_FREQ    */
    {0x1, 0xF1, 22,  PM_APPS_HLOS_OWNER},   /* CDC_A    */
    {0x1, 0xF0, 21,  PM_APPS_HLOS_OWNER},   /* CDC_D    */
    {0x1, 0xC0, 20,  PM_APPS_HLOS_OWNER},   /* VIB1     */
    {0x1, 0xBC, 19,  PM_APPS_HLOS_OWNER},   /* PWM_3D         */
    {0x1, 0x53, 54, PM_RPM_OWNER},         /* LDO20          */
    {0x1, 0x52, 53, PM_RPM_OWNER},         /* LDO19          */
    {0x1, 0x51, 52, PM_RPM_OWNER},         /* LDO18          */
    {0x1, 0x50, 58, PM_RPM_OWNER},         /* LDO17          */
    {0x1, 0x4F, 86, PM_RPM_OWNER},         /* LDO16          */
    {0x1, 0x4E, 85, PM_RPM_OWNER},         /* LDO15          */
    {0x1, 0x4D, 84, PM_RPM_OWNER},         /* LDO14          */
    {0x1, 0x4C, 57, PM_RPM_OWNER},         /* LDO13          */
    {0x1, 0x4B, 51, PM_RPM_OWNER},         /* LDO12          */
    {0x1, 0x4A, 50, PM_RPM_OWNER},         /* LDO11          */
    {0x1, 0x49, 49, PM_RPM_OWNER},         /* LDO10          */
    {0x1, 0x48, 48, PM_RPM_OWNER},         /* LDO9           */
    {0x1, 0x47, 47, PM_RPM_OWNER},         /* LDO8           */
    {0x1, 0x46, 46, PM_RPM_OWNER},         /* LDO7           */
    {0x1, 0x45, 56, PM_RPM_OWNER},         /* LDO6           */
    {0x1, 0x44, 45, PM_RPM_OWNER},         /* LDO5           */
    {0x1, 0x43, 44, PM_RPM_OWNER},         /* LDO4           */
    {0x1, 0x42, 43, PM_RPM_OWNER},         /* LDO3           */
    {0x1, 0x41, 42, PM_RPM_OWNER},         /* LDO2           */
    {0x1, 0x40, 41, PM_RPM_OWNER},         /* LDO1           */
    {0x1, 0x1F, 64, PM_RPM_OWNER},         /* S4_FREQ        */
    {0x1, 0x1E, 63, PM_RPM_OWNER},         /* S4_PS          */
    {0x1, 0x1D, 62, PM_RPM_OWNER},         /* S4_CTRL        */
    {0x1, 0x1C, 61, PM_RPM_OWNER},         /* S3_FREQ        */
    {0x1, 0x1B, 60, PM_RPM_OWNER},         /* S3_PS          */
    {0x1, 0x1A, 59, PM_RPM_OWNER},         /* S3_CTRL        */
    {0x1, 0x19, 18 , PM_APPS_HLOS_OWNER},         /* S2_FREQ        */
    {0x1, 0x18, 17 , PM_APPS_HLOS_OWNER},         /* S2_PS          */
    {0x1, 0x17, 16 , PM_APPS_HLOS_OWNER},         /* S2_CTRL        */
    {0x1, 0x16, 66,  PM_RPM_OWNER},         /* S1_FREQ        */
    {0x1, 0x15, 65, PM_RPM_OWNER},         /* S1_PS          */
    {0x1, 0x14, 67, PM_RPM_OWNER},         /* S1_CTRL        */
    {0x1, 0x10, 40,  PM_RPM_OWNER},   /* BCLK_GEN_MAIN  */
    {0x0, 0xFE, 115, PM_RPM_OWNER},         /* TRIM           */
    {0x0, 0xC3, 98 , PM_APPS_HLOS_OWNER},   /* GPIO4          */
    {0x0, 0xC2, 97 , PM_APPS_HLOS_OWNER},   /* GPIO3          */
    {0x0, 0xC1, 96,  PM_WCONNECT_OWNER},   /* GPIO2          */
    {0x0, 0xC0, 95, PM_MSS_OWNER},         /* GPIO1          */
    {0x0, 0xA3, 94 , PM_APPS_HLOS_OWNER},   /* MPP4           */
    {0x0, 0xA2, 93 , PM_APPS_HLOS_OWNER},   /* MPP3           */
    {0x0, 0xA1, 92 , PM_APPS_HLOS_OWNER},   /* MPP2           */
    {0x0, 0xA0, 91 , PM_APPS_HLOS_OWNER},   /* MPP1           */
    {0x0, 0x77, 83 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT6    */
    {0x0, 0x76, 82 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT5    */
    {0x0, 0x75, 81 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT4    */
    {0x0, 0x74, 80, PM_MSS_OWNER},         /* PBS_CLIENT3    */
    {0x0, 0x73, 39 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT2    */
    {0x0, 0x72, 38 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT1    */
    {0x0, 0x71, 37 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT0    */
    {0x0, 0x70, 36 , PM_RPM_OWNER},         /* PBS_CORE       */
    {0x0, 0x62, 15 , PM_APPS_HLOS_OWNER},   /* RTC_TIMER      */
    {0x0, 0x61, 14 , PM_APPS_HLOS_OWNER},   /* RTC_ALARM      */
    {0x0, 0x60, 79, PM_MSS_OWNER},         /* RTC_WR         */
    {0x0, 0x5A, 35 , PM_RPM_OWNER},   /* SLEEP_CLK1     */
    {0x0, 0x59, 34 , PM_RPM_OWNER},   /* CLK_DIST       */
    {0x0, 0x55, 75 , PM_RPM_OWNER},   /* RF_CLK2        */
    {0x0, 0x54, 74 , PM_RPM_OWNER},   /* RF_CLK1        */
    {0x0, 0x52, 33 , PM_RPM_OWNER},   /* BB_CLK2        */
    {0x0, 0x51, 32,  PM_RPM_OWNER},   /* BB_CLK1        */
    {0x0, 0x50, 78, PM_MSS_OWNER},   /* XO             */
    {0x0, 0x40, 13 , PM_APPS_HLOS_OWNER},   /* BMS_VM       */
    {0x0, 0x35, 12 , PM_APPS_HLOS_OWNER},   /* VADC4_LC_VBAT        */
    {0x0, 0x34, 11 , PM_APPS_HLOS_OWNER},   /* VADC2_BTM      */
    {0x0, 0x33, 10, PM_MSS_OWNER},         /* VADC3_LC_VBMS      */
    {0x0, 0x32, 77,  PM_RPM_OWNER},         /* VADC3_MDM      */
    {0x0, 0x31, 9 , PM_APPS_HLOS_OWNER},   /* VADC1_USR      */
    {0x0, 0x2C, 31,  PM_RPM_OWNER},   /* MBG1           */
    {0x0, 0x28, 8  , PM_APPS_HLOS_OWNER},   /* COIN           */
    {0x0, 0x24, 7  , PM_APPS_HLOS_OWNER},   /* TEMP_ALARM     */
    {0x0, 0x1C, 76, PM_MSS_OWNER},         /* BUA_EXT_CHARGER*/
    {0x0, 0x16, 6  , PM_APPS_HLOS_OWNER},   /* LBC_MISC      */
    {0x0, 0x13, 5  , PM_APPS_HLOS_OWNER},   /* LBC_USB      */
    {0x0, 0x12, 4  , PM_APPS_HLOS_OWNER},   /* LBC_BAT_IF      */
    {0x0, 0x10, 3  , PM_APPS_HLOS_OWNER},   /* LBC_CHGR      */
    {0x0, 0x0A, 2  , PM_APPS_HLOS_OWNER},   /* VREFLPDDR      */
    {0x0, 0x09, 0,   PM_RPM_OWNER},   /* MISC           */
    {0x0, 0x08, 1  , PM_APPS_HLOS_OWNER},   /* PON            */
    {0x0, 0x06, 114,  PM_RPM_OWNER},         /* SPMI           */
    {0x0, 0x05, 113, PM_RPM_OWNER},         /* INT            */
    {0x0, 0x04, 112, PM_RPM_OWNER},         /* BUS            */
    {0x0, 0x01, 111, PM_RPM_OWNER},         /* REVID          */
};

/* Number of peripherals. */
const uint32 pm8916_pm_num_periph = sizeof(pm8916_pm_spmi_periph_cfg) / sizeof(SpmiCfg_ChannelCfg);

pm_pbs_info_data_type pm8916_pm_pbs_info_data_a =
{
    128,        /* PBS MEMORY Size */
    PM_PBS_INFO_IN_MISC,     /* Place where MFG Info stored */
    0x0900,     /* MISC Periph BASE_ADDRESS */
    0xF0,       /* TRIM_NUM  */
    0xF1,       /* TP_REV    */
    0xF2,       /* FAB_ID    */
    0xF3,       /* WAFER_ID  */
    0xF4,       /* X_CO_ORDI */
    0xF5,       /* Y_CO_ORDI */
    0xF6,       /* LOT_ID    */
    0xFF,       /* MFG_ID    */
};

pm_pbs_info_data_type *pm8916_pm_pbs_info_data[]=
{
    {&pm8916_pm_pbs_info_data_a},
};
