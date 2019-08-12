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

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/config/mdm9x45/pm_config_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/15   rk      Changed PMIC PS_HOLD config API call for multi-PMIC support.
09/29/14   mr      Updated/Modified PBS Driver (CR-728234)
05/09/13   aab     Updated MPP driver support
03/26/13   aab     Updated GPIO driver support
02/03/13   aab     Added lookup table array SMPS_ILimit_LUT[] to store smps current limit
01/18/13   aab     Cleanup
01/09/13   aab     Added support for sleep clk
12/06/12   hw      Rearchitecturing module driver to peripheral driver
08/07/12   hs      Added support for 5V boost.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_mpp_driver.h"
#include "pm_vib_driver.h"
#include "pm_pbs_info.h"
#include "SpmiCfg.h"


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

/* PPID, channel mapping and ownership.  */
const SpmiCfg_ChannelCfg  pm_spmi_periph_cfg_pmd9645[] =
{
  {0xF, 0x00, 95, PM_APPS_TZ_OWNER}, /* GID            */
  /* Write broadcast channels. */
  {0xE, 0x08, 94, PM_APPS_HLOS_OWNER},     /* PON           */
  {0xE, 0x06, 93, PM_RPM_OWNER},     /* SPMI          */
  {0xE, 0x05, 92, PM_RPM_OWNER},     /* INT           */

  {0x2, 0x06, 91, PM_RPM_OWNER}, /* SPMI_OPTIONS  */
  {0x1, 0x51, 90, PM_RPM_OWNER}, /* LDO_RFCLK     */
  {0x1, 0x50, 89, PM_RPM_OWNER}, /* LDO_XO        */
  {0x1, 0x4F, 88, PM_RPM_OWNER}, /* LDO16         */
  {0x1, 0x4E, 87, PM_RPM_OWNER}, /* LDO15         */
  {0x1, 0x4D, 86, PM_RPM_OWNER}, /* LDO14         */
  {0x1, 0x4C, 85, PM_RPM_OWNER}, /* LDO13         */
  {0x1, 0x4B, 84, PM_RPM_OWNER}, /* LDO12         */
  {0x1, 0x4A, 83, PM_RPM_OWNER}, /* LDO11         */
  {0x1, 0x49, 82, PM_RPM_OWNER}, /* LDO10         */
  {0x1, 0x48, 81, PM_RPM_OWNER}, /* LDO9          */
  {0x1, 0x47, 80, PM_RPM_OWNER}, /* LDO8          */
  {0x1, 0x46, 79, PM_RPM_OWNER}, /* LDO7          */
  {0x1, 0x45, 78, PM_RPM_OWNER}, /* LDO6          */
  {0x1, 0x44, 77, PM_RPM_OWNER}, /* LDO5          */
  {0x1, 0x43, 76, PM_RPM_OWNER}, /* LDO4          */
  {0x1, 0x42, 75, PM_RPM_OWNER}, /* LDO3          */
  {0x1, 0x41, 74, PM_RPM_OWNER}, /* LDO2          */
  {0x1, 0x40, 73, PM_RPM_OWNER}, /* LDO1          */
  {0x1, 0x25, 72, PM_RPM_OWNER}, /* S5_FREQ       */
  {0x1, 0x24, 71, PM_RPM_OWNER}, /* S5_PS         */
  {0x1, 0x23, 70, PM_RPM_OWNER}, /* S5_CTRL       */
  {0x1, 0x22, 69, PM_RPM_OWNER}, /* S4_FREQ       */
  {0x1, 0x21, 68, PM_RPM_OWNER}, /* S4_PS         */
  {0x1, 0x20, 67, PM_RPM_OWNER}, /* S4_CTRL       */
  {0x1, 0x1C, 66, PM_RPM_OWNER}, /* S3_FREQ       */
  {0x1, 0x1B, 65, PM_RPM_OWNER}, /* S3_PS         */
  {0x1, 0x1A, 64, PM_RPM_OWNER}, /* S3_CTRL       */
  {0x1, 0x19, 63, PM_RPM_OWNER}, /* S2_FREQ       */
  {0x1, 0x18, 62, PM_RPM_OWNER}, /* S2_PS         */
  {0x1, 0x17, 61, PM_RPM_OWNER}, /* S2_CTRL       */
  {0x1, 0x16, 60, PM_MSS_OWNER}, /* S1_FREQ       */
  {0x1, 0x15, 59, PM_MSS_OWNER}, /* S1_PS         */
  {0x1, 0x14, 58, PM_MSS_OWNER}, /* S1_CTRL       */
  {0x1, 0x10, 57, PM_RPM_OWNER}, /* BCLK_GEN_MAIN */
  {0x0, 0xFE, 56, PM_RPM_OWNER}, /* TRIM          */
  {0x0, 0xC5, 55, PM_APPS_HLOS_OWNER},     /* GPIO6         */
  {0x0, 0xC4, 54, PM_APPS_HLOS_OWNER},     /* GPIO5         */
  {0x0, 0xC3, 53, PM_APPS_HLOS_OWNER},     /* GPIO4         */
  {0x0, 0xC2, 52, PM_MSS_OWNER},     /* GPIO3         */
  {0x0, 0xC1, 51, PM_MSS_OWNER},     /* GPIO2         */
  {0x0, 0xC0, 50, PM_MSS_OWNER},     /* GPIO1         */
  {0x0, 0xA5, 49, PM_APPS_HLOS_OWNER},     /* MPP6          */
  {0x0, 0xA4, 48, PM_MSS_OWNER},     /* MPP5          */
  {0x0, 0xA3, 47, PM_APPS_HLOS_OWNER},     /* MPP4          */
  {0x0, 0xA2, 46, PM_APPS_HLOS_OWNER},     /* MPP3          */
  {0x0, 0xA1, 45, PM_APPS_HLOS_OWNER},     /* MPP2          */
  {0x0, 0xA0, 44, PM_APPS_HLOS_OWNER},     /* MPP1          */
  {0x0, 0x80, 43, PM_RPM_OWNER},     /* PBS_CLIENT15  */
  {0x0, 0x7F, 42, PM_RPM_OWNER},     /* PBS_CLIENT14  */
  {0x0, 0x7E, 41, PM_RPM_OWNER},     /* PBS_CLIENT13  */
  {0x0, 0x7D, 40, PM_RPM_OWNER},     /* PBS_CLIENT12  */
  {0x0, 0x7C, 39, PM_RPM_OWNER},     /* PBS_CLIENT11  */
  {0x0, 0x7B, 38, PM_RPM_OWNER},     /* PBS_CLIENT10  */
  {0x0, 0x7A, 37, PM_RPM_OWNER},     /* PBS_CLIENT9   */
  {0x0, 0x79, 36, PM_RPM_OWNER},     /* PBS_CLIENT8   */
  {0x0, 0x78, 35, PM_RPM_OWNER},     /* PBS_CLIENT7   */
  {0x0, 0x77, 34, PM_MSS_OWNER},     /* PBS_CLIENT6   */
  {0x0, 0x76, 33, PM_RPM_OWNER},     /* PBS_CLIENT5   */
  {0x0, 0x75, 32, PM_RPM_OWNER},     /* PBS_CLIENT4   */
  {0x0, 0x74, 31, PM_RPM_OWNER},     /* PBS_CLIENT3   */
  {0x0, 0x73, 30, PM_RPM_OWNER},     /* PBS_CLIENT2   */
  {0x0, 0x72, 29, PM_RPM_OWNER},     /* PBS_CLIENT1   */
  {0x0, 0x71, 28, PM_RPM_OWNER},     /* PBS_CLIENT0   */
  {0x0, 0x70, 27, PM_RPM_OWNER},     /* PBS_CORE      */
  {0x0, 0x62, 26, PM_APPS_HLOS_OWNER},     /* RTC_TIMER     */
  {0x0, 0x61, 25, PM_APPS_HLOS_OWNER},     /* RTC_ALARM     */
  {0x0, 0x60, 24, PM_MSS_OWNER},     /* RTC_WR        */
  {0x0, 0x5D, 23, PM_RPM_OWNER},     /* DIV_CLK3      */
  {0x0, 0x5C, 22, PM_RPM_OWNER},     /* DIV_CLK2      */
  {0x0, 0x5B, 21, PM_RPM_OWNER},     /* DIV_CLK1      */
  {0x0, 0x5A, 20, PM_RPM_OWNER},     /* SLEEP_CLK1    */
  {0x0, 0x59, 19, PM_RPM_OWNER},     /* CLK_DIST      */
  {0x0, 0x55, 18, PM_RPM_OWNER},     /* RF_CLK2       */
  {0x0, 0x54, 17, PM_RPM_OWNER},     /* RF_CLK1       */
  {0x0, 0x52, 16, PM_APPS_HLOS_OWNER},     /* LN_BB_CLK     */
  {0x0, 0x51, 15, PM_RPM_OWNER},     /* BB_CLK1       */
  {0x0, 0x50, 14, PM_MSS_OWNER},     /* XO            */
  {0x0, 0x35, 13, PM_MSS_OWNER},     /* VADC3_VBAT    */
  {0x0, 0x34, 12, PM_APPS_HLOS_OWNER},     /* VADC2_BTM     */
  {0x0, 0x32, 11, PM_MSS_OWNER},     /* VADC3_MDM     */
  {0x0, 0x31, 10, PM_APPS_HLOS_OWNER},     /* VADC1_USR     */
  {0x0, 0x2C, 9 , PM_RPM_OWNER},     /* MBG1          */
  {0x0, 0x24, 8 , PM_APPS_HLOS_OWNER},     /* TEMP_ALARM    */
  {0x0, 0x1C, 7 , PM_MSS_OWNER},     /* BUA_4UICC_DUA */
  {0x0, 0x0A, 6 , PM_RPM_OWNER},     /* VREFLPDDR     */
  {0x0, 0x09, 5 , PM_MSS_OWNER},     /* MISC          */
  {0x0, 0x08, 4 , PM_APPS_HLOS_OWNER},     /* PON           */
  {0x0, 0x06, 3 , PM_RPM_OWNER},     /* SPMI          */
  {0x0, 0x05, 2 , PM_RPM_OWNER},     /* INT           */
  {0x0, 0x04, 1 , PM_RPM_OWNER},     /* BUS           */
  {0x0, 0x01, 0 , PM_RPM_OWNER},     /* REVID         */
};

/* Number of peripherals. */
const uint32 pm_num_periph_pmd9645 = sizeof(pm_spmi_periph_cfg_pmd9645) / sizeof(SpmiCfg_ChannelCfg);

pm_pbs_info_data_type pm_pbs_info_data_a =
{
    256,        /* PBS MEMORY Size */
    PM_PBS_INFO_IN_OTP,     /* Place where MFG Info stored */
    0x0900,     /* MISC Periph BASE_ADDRESS - NOT APP for PMD9x35*/
    0xF0,       /* TRIM_NUM  */
    0xF1,       /* TP_REV    */
    0xF2,       /* FAB_ID    */
    0xF3,       /* WAFER_ID  */
    0xF4,       /* X_CO_ORDI */
    0xF5,       /* Y_CO_ORDI */
    0xF6,       /* LOT_ID    */
    0xFF,       /* MFG_ID    */
};

pm_pbs_info_data_type *pm_pbs_info_data[]=
{
    {&pm_pbs_info_data_a},
};
