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

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/config/msm8956/pm_config_target.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/15   mr      Added support for 8956 target (CR-790476)
05/17/14   rk      Modify XPU permission and Int Ownership for PMIC Peripherals (CR-666063)
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
#include "SpmiCfg.h"
#include "pm_led.h"
#include "pm_smb.h"
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
pm_led_config_type led_array[] =
{   // pm_led_type,   pm_led_channel,          level 
    {PM_LED_ATC, PM_LED_CH_INVALID, 0},
    {PM_LED_KPD, PM_LED_CH_MPP2, PM_MPP__I_SINK__LEVEL_40mA},
//    {PM_LED_RED, PM_LED_CH_RED, PM_RGB_DIM_LEVEL_MID},
//    {PM_LED_GREEN, PM_LED_CH_GREEN, PM_RGB_DIM_LEVEL_MID},
//    {PM_LED_BLUE, PM_LED_CH_BLUE, PM_RGB_DIM_LEVEL_MID},
};

const pm_led_type default_led = PM_LED_ATC;

// This array is used form tri-color LED connected to RGB module only.
// For tri-color LED which is connect to led_array, search in led_array
// Do NOT suppose this array size must be 3. Customers may use dual color LED but connect to RGB module
pm_led_rgb_config_type rgb_configs[] =
{
    // rgb mask bit          LPG channel         power source
    {PM_RGB_SEGMENT_R, PM_LPG_CHAN_7, PM_RGB_VOLTAGE_SOURCE_VPH},
    {PM_RGB_SEGMENT_G, PM_LPG_CHAN_6, PM_RGB_VOLTAGE_SOURCE_VPH},
    {PM_RGB_SEGMENT_B, PM_LPG_CHAN_5, PM_RGB_VOLTAGE_SOURCE_VPH},
};

pm_led_info_type led_info = 
{
    led_array,
    rgb_configs,
    default_led
};

// maybe used in the future if multiple smb chips are use to charge together
pm_smb_index_type smb_index_array[] =
{
    SMB_INDEX_1360,
//    SMB_INDEX_358,
    SMB_INDEX_INVALID
};

pm_smb_hw_info_type smb_hw_info =
{
    0x14,
    DALDEVICEID_I2C_DEVICE_4,
    400,
    2500,
    SMB_INDEX_1360
};

unsigned smps_phase_lut_a[ ]= {0,0,0,0};


pm_mpp_specific_info_type
mpp_specific[1] =
{
   {0x9E0, 4}
};

pm_vib_specific_data_type
vib_specific[1] = 
{
  {1200, 3100}
};

uint16 chg_app_ds[] = 
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

const SpmiCfg_ChannelCfg  pm_spmi_periph_cfg[] =
{
    {0xE, 0x06, 167,  PM_RPM_OWNER},		 /* SPMI   */
    {0xE, 0x06, 162,  PM_RPM_OWNER},		 /* SPMI   */
    {0xE, 0x05, 166,  PM_RPM_OWNER},		 /* INT    */
    {0xE, 0x05, 161,  PM_RPM_OWNER},		 /* INT    */
    
    /* PM8004 Peripherals */
  {0x5, 0x40, 247, PM_RPM_OWNER        },  /*   LDO1            */
  {0x5, 0x22, 246, PM_RPM_OWNER        },  /*   S5_FREQ         */
  {0x5, 0x20, 245, PM_RPM_OWNER        },  /*   S5_CTRL         */
  {0x5, 0x1F, 244, PM_RPM_OWNER        },  /*   S4_FREQ         */
  {0x5, 0x1D, 243, PM_RPM_OWNER        },  /*   S4_CTRL         */
  {0x5, 0x19, 242, PM_RPM_OWNER        },  /*   S2_FREQ         */
  {0x5, 0x17, 241, PM_RPM_OWNER        },  /*   S2_CTRL         */
  {0x5, 0x16, 240, PM_RPM_OWNER        },  /*   S1_FREQ         */
  {0x5, 0x14, 239, PM_RPM_OWNER        },  /*   S1_CTRL         */
  {0x5, 0x10, 238, PM_RPM_OWNER        },  /*   BCLK_GEN_MAIN   */
  {0x4, 0xFE, 237, PM_RPM_OWNER        },  /*   TRIM            */
  {0x4, 0xA3, 236, PM_APPS_HLOS_OWNER  },  /*   MPP4            */
  {0x4, 0xA2, 235, PM_APPS_HLOS_OWNER  },  /*   MPP3            */
  {0x4, 0xA1, 234, PM_APPS_HLOS_OWNER  },  /*   MPP2            */
  {0x4, 0xA0, 233, PM_APPS_HLOS_OWNER  },  /*   MPP1            */
  {0x4, 0x78, 232, PM_RPM_OWNER        },  /*   PBS_CLIENT7     */
  {0x4, 0x77, 231, PM_RPM_OWNER        },  /*   PBS_CLIENT6     */
  {0x4, 0x76, 230, PM_RPM_OWNER        },  /*   PBS_CLIENT5     */
  {0x4, 0x75, 229, PM_RPM_OWNER        },  /*   PBS_CLIENT4     */
  {0x4, 0x74, 228, PM_RPM_OWNER        },  /*   PBS_CLIENT3     */
  {0x4, 0x73, 227, PM_RPM_OWNER        },  /*   PBS_CLIENT2     */
  {0x4, 0x72, 226, PM_RPM_OWNER        },  /*   PBS_CLIENT1     */
  {0x4, 0x71, 225, PM_RPM_OWNER        },  /*   PBS_CLIENT0     */
  {0x4, 0x70, 224, PM_RPM_OWNER        },  /*   PBS_CORE        */
  {0x4, 0x59, 223, PM_RPM_OWNER        },  /*   CLK_DIST        */
  {0x4, 0x2C, 222, PM_RPM_OWNER        },  /*   MBG1            */
  {0x4, 0x24, 221, PM_APPS_HLOS_OWNER  },  /*   TEMP_ALARM      */
  {0x4, 0x09, 220, PM_APPS_HLOS_OWNER  },  /*   MISC            */
  {0x4, 0x08, 219, PM_APPS_HLOS_OWNER  },  /*   PON             */
  {0x4, 0x06, 218, PM_RPM_OWNER        },  /*   SPMI            */
  {0x4, 0x05, 217, PM_RPM_OWNER        },  /*   INT             */
  {0x4, 0x04, 216, PM_RPM_OWNER        },  /*   BUS             */
  {0x4, 0x01, 215, PM_RPM_OWNER        },  /*   REVID           */
	
    /*	PMI8950 Peripherals */
    {0x3, 0xDE, 57,   PM_APPS_HLOS_OWNER},	 /* LAB 		 */
    {0x3, 0xDC, 56,   PM_APPS_HLOS_OWNER},	 /* IBB 		 */
    {0x3, 0xD9, 55,   PM_APPS_HLOS_OWNER},	 /* WLED1_SINK			*/
    {0x3, 0xD8, 54,   PM_APPS_HLOS_OWNER},	 /* WLED1_CTRL		   */
    {0x3, 0xD3, 53,   PM_APPS_HLOS_OWNER},	 /* FLASH1	   */
    {0x3, 0xC0, 52,   PM_APPS_HLOS_OWNER},	 /* HAPTICS    */
    {0x3, 0xB0, 51,   PM_APPS_HLOS_OWNER},	 /* PWM    */
    {0x3, 0x40, 50,   PM_APPS_HLOS_OWNER},	 /* LDO1	*/    
    
    {0x2, 0xFE, 168,  PM_RPM_OWNER},		 /* TRIM	*/
    {0x2, 0xC1, 153,  PM_APPS_HLOS_OWNER},	 /* GPIO2	 */
    {0x2, 0xC0, 152,  PM_APPS_HLOS_OWNER},	 /* GPIO1	 */
    {0x2, 0xA3, 151,  PM_APPS_HLOS_OWNER},	 /* MPP4	*/
    {0x2, 0xA2, 150,  PM_APPS_HLOS_OWNER},	 /* MPP3	*/
    {0x2, 0xA1, 149,  PM_APPS_HLOS_OWNER},	 /* MPP2	*/
    {0x2, 0xA0, 148,  PM_APPS_HLOS_OWNER},	 /* MPP1	*/
    {0x2, 0x79, 116,  PM_RPM_OWNER},		 /* PBS_CLIENT8    */
    {0x2, 0x78, 49,   PM_APPS_HLOS_OWNER},	 /* PBS_CLIENT7    */
    {0x2, 0x77, 48,   PM_APPS_HLOS_OWNER},	 /* PBS_CLIENT6    */
    {0x2, 0x76, 115,  PM_RPM_OWNER},		 /* PBS_CLIENT5    */
    {0x2, 0x75, 47,   PM_APPS_HLOS_OWNER},	 /* PBS_CLIENT4    */
    {0x2, 0x74, 46,   PM_APPS_HLOS_OWNER},	 /* PBS_CLIENT3    */
    {0x2, 0x73, 45,   PM_APPS_HLOS_OWNER},	 /* PBS_CLIENT2    */
    {0x2, 0x72, 44,   PM_APPS_HLOS_OWNER},	 /* PBS_CLIENT1    */
    {0x2, 0x71, 43,   PM_APPS_HLOS_OWNER},	 /* PBS_CLIENT0    */
    {0x2, 0x70, 114,  PM_RPM_OWNER},		 /* PBS_CORE	*/
    {0x2, 0x59, 113,  PM_RPM_OWNER},		 /* CLK_DIST	*/
    {0x2, 0x44, 42,   PM_APPS_HLOS_OWNER},	 /* FG_MEMIF	*/
    {0x2, 0x43, 41,   PM_APPS_HLOS_OWNER},	 /* FG_ADC_MDM	  */
    {0x2, 0x42, 40,   PM_APPS_HLOS_OWNER},	 /* FG_ADC_USR	  */
    {0x2, 0x41, 39,   PM_APPS_HLOS_OWNER},	 /* FG_BATT    */
    {0x2, 0x40, 38,   PM_APPS_HLOS_OWNER},	 /* FG_SOC	  */
    {0x2, 0x31, 37,   PM_APPS_HLOS_OWNER},	 /* VADC1_USR	 */
    {0x2, 0x2C, 112,  PM_RPM_OWNER},		 /* MBG1	*/
    {0x2, 0x1C, 130,  PM_MSS_OWNER},		 /* BUA    */
    {0x2, 0x1B, 36,   PM_APPS_HLOS_OWNER},	 /* BSI    */
    {0x2, 0x16, 35,   PM_APPS_HLOS_OWNER},	 /* SMBCHGL_MISC	*/
    {0x2, 0x14, 34,   PM_APPS_HLOS_OWNER},	 /* SMBCHGL_DC	  */
    {0x2, 0x13, 33,   PM_APPS_HLOS_OWNER},	 /* SMBCHGL_USB    */
    {0x2, 0x12, 32,   PM_APPS_HLOS_OWNER},	 /* SMBCHGL_BAT_IF	  */
    {0x2, 0x11, 31,   PM_APPS_HLOS_OWNER},	 /* SMBCHGL_OTG    */
    {0x2, 0x10, 30,   PM_APPS_HLOS_OWNER},	 /* SMBCHGL_CHGR	*/
    {0x2, 0x09, 29,   PM_RPM_OWNER},		 /* MISC	*/
    {0x2, 0x08, 28,   PM_APPS_HLOS_OWNER},	 /* PON    */
    {0x2, 0x04, 165,  PM_RPM_OWNER},		 /* BUS    */
    {0x2, 0x01, 164,  PM_RPM_OWNER},		 /* REVID	 */    
    
    /* PM8950 Peripherals */
    {0x1, 0xF3, 24,  PM_APPS_HLOS_OWNER},		  /* CDC_NCP_FREQ		   */
    {0x1, 0xF2, 23,  PM_APPS_HLOS_OWNER},		  /* CDC_BOOST_FREQ 		 */
    {0x1, 0xF1, 22,  PM_APPS_HLOS_OWNER},		  /* CDC_A			*/
    {0x1, 0xF0, 21,  PM_APPS_HLOS_OWNER},		  /* CDC_D			*/
    {0x1, 0xBC, 20,  PM_RPM_OWNER}, 		/* PWM			*/
    {0x1, 0x56, 111,  PM_RPM_OWNER},		 /* LDO23		   */
    {0x1, 0x55, 110,  PM_RPM_OWNER},		 /* LDO22		   */
    {0x1, 0x54, 109,  PM_RPM_OWNER},		 /* LDO19		   */
    {0x1, 0x53, 108,  PM_RPM_OWNER},		 /* LDO21		   */
    {0x1, 0x52, 107,  PM_RPM_OWNER},		 /* LDO20		   */
    {0x1, 0x51, 106,  PM_RPM_OWNER},		 /* LDO18		   */
    {0x1, 0x50, 105,  PM_RPM_OWNER},		 /* LDO17		   */
    {0x1, 0x4F, 104,  PM_RPM_OWNER},		 /* LDO16		   */
    {0x1, 0x4E, 103,  PM_RPM_OWNER},		 /* LDO15		   */
    {0x1, 0x4D, 102,  PM_RPM_OWNER},		 /* LDO14		   */
    {0x1, 0x4C, 101,  PM_RPM_OWNER},		 /* LDO13		   */
    {0x1, 0x4B, 100,  PM_RPM_OWNER},		 /* LDO12		   */
    {0x1, 0x4A, 99,  PM_RPM_OWNER}, 		/* LDO11		  */
    {0x1, 0x49, 98,  PM_RPM_OWNER}, 		/* LDO10		  */
    {0x1, 0x48, 97,  PM_RPM_OWNER}, 		/* LDO9 		 */
    {0x1, 0x47, 96,  PM_RPM_OWNER}, 		/* LDO8 		 */
    {0x1, 0x46, 95,  PM_RPM_OWNER}, 		/* LDO7 		 */
    {0x1, 0x45, 94,  PM_RPM_OWNER}, 		/* LDO6 		 */
    {0x1, 0x44, 93,  PM_RPM_OWNER}, 		/* LDO5 		 */
    {0x1, 0x43, 92,  PM_RPM_OWNER}, 		/* LDO4 		 */
    {0x1, 0x42, 91,  PM_RPM_OWNER}, 		/* LDO3 		 */
    {0x1, 0x41, 90,  PM_RPM_OWNER}, 		/* LDO2 		 */
    {0x1, 0x40, 89,  PM_RPM_OWNER}, 		/* LDO1 		 */
    {0x1, 0x25, 19,  PM_APPS_HLOS_OWNER},		  /* S6_FREQ		  */
    {0x1, 0x24, 18,  PM_APPS_HLOS_OWNER},		  /* S6_PS			*/
    {0x1, 0x23, 17,  PM_APPS_HLOS_OWNER},		  /* S6_CTRL		  */
    {0x1, 0x22, 16,  PM_APPS_HLOS_OWNER},		  /* S5_FREQ		  */
    {0x1, 0x21, 15,  PM_APPS_HLOS_OWNER},		  /* S5_PS			*/
    {0x1, 0x20, 14,  PM_APPS_HLOS_OWNER},		  /* S5_CTRL		  */
    {0x1, 0x1F, 88,  PM_RPM_OWNER}, 		/* S4_FREQ			*/
    {0x1, 0x1E, 87,  PM_RPM_OWNER}, 		/* S4_PS		  */
    {0x1, 0x1D, 86,  PM_RPM_OWNER}, 		/* S4_CTRL			*/
    {0x1, 0x1C, 85,  PM_RPM_OWNER}, 		/* S3_FREQ			*/
    {0x1, 0x1B, 84,  PM_RPM_OWNER}, 		/* S3_PS		  */
    {0x1, 0x1A, 83,  PM_RPM_OWNER}, 		/* S3_CTRL			*/
    {0x1, 0x19, 82,  PM_RPM_OWNER}, 		/* S2_FREQ			*/
    {0x1, 0x18, 81,  PM_RPM_OWNER}, 		/* S2_PS		  */
    {0x1, 0x17, 80,  PM_RPM_OWNER}, 		/* S2_CTRL			*/
    {0x1, 0x16, 79,  PM_RPM_OWNER}, 		/* S1_FREQ			*/
    {0x1, 0x15, 78,  PM_RPM_OWNER}, 		/* S1_PS		  */
    {0x1, 0x14, 77,  PM_RPM_OWNER}, 		/* S1_CTRL			*/
    {0x1, 0x10, 76,  PM_RPM_OWNER}, 		/* BCLK_GEN_MAIN		  */    
    
    {0x0, 0xFE, 163,  PM_RPM_OWNER},		 /* TRIM		  */
    {0x0, 0xC7, 147,  PM_RPM_OWNER},		 /* GPIO8		   */
    {0x0, 0xC6, 146,  PM_RPM_OWNER},		 /* GPIO7		   */
    {0x0, 0xC5, 145,  PM_RPM_OWNER},		 /* GPIO6		   */
    {0x0, 0xC4, 144,  PM_RPM_OWNER},		 /* GPIO5		   */
    {0x0, 0xC3, 143,  PM_RPM_OWNER},		 /* GPIO4		   */
    {0x0, 0xC2, 142,  PM_RPM_OWNER},		 /* GPIO3		   */
    {0x0, 0xC1, 141,  PM_RPM_OWNER},		 /* GPIO2		   */
    {0x0, 0xC0, 140,  PM_RPM_OWNER},		 /* GPIO1		   */
    {0x0, 0xA3, 139,  PM_RPM_OWNER},		 /* MPP4		  */
    {0x0, 0xA2, 138,  PM_RPM_OWNER},		 /* MPP3		  */
    {0x0, 0xA1, 137,  PM_RPM_OWNER},		 /* MPP2		  */
    {0x0, 0xA0, 136,  PM_RPM_OWNER},		 /* MPP1		  */
    {0x0, 0x78, 13,  PM_RPM_OWNER}, 		/* PBS_CLIENT7			*/
    {0x0, 0x77, 12,  PM_RPM_OWNER}, 		/* PBS_CLIENT6			*/
    {0x0, 0x76, 11,  PM_RPM_OWNER}, 		/* PBS_CLIENT5			*/
    {0x0, 0x75, 75,  PM_RPM_OWNER},         /* PBS_CLIENT4          */
    {0x0, 0x74, 129,  PM_RPM_OWNER},         /* PBS_CLIENT3          */
    {0x0, 0x73, 10,  PM_RPM_OWNER},         /* PBS_CLIENT2          */
    {0x0, 0x72, 9,  PM_RPM_OWNER},         /* PBS_CLIENT1          */
    {0x0, 0x71, 43,  PM_RPM_OWNER}, 		/* PBS_CLIENT0			*/
    {0x0, 0x70, 73,  PM_RPM_OWNER}, 		/* PBS_CORE 		 */
    {0x0, 0x62, 72,  PM_RPM_OWNER}, 		/* RTC_TIMER		  */
    {0x0, 0x61, 8,  PM_RPM_OWNER}, 		/* RTC_ALARM		  */
    {0x0, 0x60, 60,  PM_RPM_OWNER}, 		/* RTC_RW		   */
    {0x0, 0x5D, 71,  PM_RPM_OWNER}, 		/* DIV_CLK3 		 */
    {0x0, 0x5C, 70,  PM_RPM_OWNER}, 		/* DIV_CLK2 		 */
    {0x0, 0x5B, 69,  PM_RPM_OWNER}, 		/* DIV_CLK1 		 */
    {0x0, 0x5A, 68,  PM_RPM_OWNER}, 		/* SLEEP_CLK1		   */
    {0x0, 0x59, 67,  PM_RPM_OWNER}, 		/* CLK_DIST 		 */
    {0x0, 0x58, 66,  PM_RPM_OWNER}, 		/* LN_BB_CLK		  */
    {0x0, 0x55, 127,  PM_RPM_OWNER},		 /* RF_CLK2 		 */
    {0x0, 0x54, 126,  PM_RPM_OWNER},		 /* RF_CLK1 		 */
    {0x0, 0x52, 65,  PM_RPM_OWNER}, 		/* BB_CLK2			*/
    {0x0, 0x51, 64,  PM_RPM_OWNER}, 		/* BB_CLK1			*/
    {0x0, 0x50, 125,  PM_RPM_OWNER},		 /* XO			*/
    {0x0, 0x35, 7,  PM_RPM_OWNER}, 		/* VADC4_LC_VBAT		  */
    {0x0, 0x34, 6,  PM_RPM_OWNER}, 		/* VADC2_LC_BTM_2		   */
    {0x0, 0x32, 124,  PM_RPM_OWNER},		 /* VADC3_LC_MDM		  */
    {0x0, 0x31, 5,  PM_RPM_OWNER}, 		/* VADC1_LC_USR 		 */
    {0x0, 0x2C, 63,  PM_RPM_OWNER}, 		/* MBG1 		 */
    {0x0, 0x28, 4,  PM_RPM_OWNER}, 		/* COIN 		 */
    {0x0, 0x24, 3,  PM_RPM_OWNER}, 		/* TEMP_ALARM		   */
    {0x0, 0x1C, 123,  PM_RPM_OWNER},		 /* BUA_EXT_CHARGER 		 */
    {0x0, 0x0A, 2,  PM_RPM_OWNER}, 		/* VREFLPDDR		  */
    {0x0, 0x09, 1,  PM_RPM_OWNER}, 		/* MISC 		 */
    {0x0, 0x08, 0,  PM_RPM_OWNER}, 		/* PON			*/
    {0x0, 0x04, 160,  PM_RPM_OWNER},		 /* BUS 		 */
    {0x0, 0x01, 159,  PM_RPM_OWNER},		 /* REVID		   */
};

/* Number of peripherals. */
const uint32 pm_num_periph = sizeof(pm_spmi_periph_cfg) / sizeof(SpmiCfg_ChannelCfg);


