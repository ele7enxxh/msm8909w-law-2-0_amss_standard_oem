/*! \file
 *
 *  \brief  pm_common_config.c ----This file contains peripheral related settings that are common across PMICs.
 *  \details This file contains peripheral related settings that are common across PMICs.
 *
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Resource Setting Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Software Register Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Processor Allocation Information Version: VU.Please Provide Valid Label - Not Approved
 *    This file contains code for Target specific settings and modes.
 *
 *  &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/config/common/pm_config_common.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/15   mr      Resolved issue with Coincell Driver APIs' failure (CR-801845)
01/29/15   mr      Added support for Coincell Driver (CR-688196)
02/25/14   aab     Updated to the latest PMIC driver architecture
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_gpio_driver.h"
#include "pm_mpp_driver.h"
#include "pm_rgb_driver.h"
#include "pm_lpg_driver.h"
#include "pm_vib_driver.h"
#include "pm_clk_driver.h"
#include "pm_pwr_alg.h"
#include "pm_smps_driver.h"
#include "pm_pbs_client_driver.h"
#include "pm_smbb_driver.h"
#include "pm_lbc_driver.h"
#include "pm_wled_driver.h"
#include "pm_lab_driver.h"
#include "pm_ibb_driver.h"
#include "pm_pwm_driver.h"
#include "pm_config_sbl.h"
#include "pm_coincell_driver.h"

pm_settings_test_flag_reg_info_type settings_test_flag_reg[1] = {0x88F}; //current targetes (8994,9x45,8909) has this same spare register from primary pmic available. If register changes in future then it needs to be moved in pmic specific config.

pm_pwr_range_info_type fts_range[1] =
{
    {0, 1}
};
pm_pwr_volt_info_type fts_volt[2] =
{
    { 0,        1275000,          5000},  //FTS     0 LV  For MV_RANGE = 0: VSET => 0.005V * m, where m = <7:0>
    { 0,        2550000,         10000},  //FTS     1 MV  For MV_RANGE = 1: VSET => 0.010V * m, where m = <7:0>
};
pm_pwr_range_info_type fts2p5_range[1] =
{
    {0, 1}
};
/* Supported LV Range: 0.08V - 1.355V Recommended LV Range: 0.35V - 1.355V
   Supported MV Range: 0.16V - 2.2V   Recommended MV Range: 0.7V - 2.2V */
pm_pwr_volt_info_type fts2p5_volt[2] =
{
    { 80000,        1355000,          5000},  //FTS 2.5  0 LV  For MV_RANGE = 0: VSET => (0.005V * m) + 0.08, where m = <7:0>
    {160000,        2200000,         10000},  //FTS 2.5  1 MV  For MV_RANGE = 1: VSET => (0.010V * m) + 0.16, where m = <7:0>
};

pm_pwr_range_info_type hfs_range[1] =
{
    {0, 1}
};
pm_pwr_volt_info_type hfs_volt[2] =
{
    { 375000,   1562500,         12500},  //HFS     0   Band 1
    {1550000,   3125000,         25000},  //HFS     1   Band 2
};

pm_pwr_range_info_type n1200_range[1] =
{
    {0, 2}
};
pm_pwr_volt_info_type n1200_volt[3] =
{
    { 375000,   1537500,         12500},  //LDO     0 (auto low range)
    { 375000,   1537500,         12500},  //LDO     1 (ultra low range)
    { 750000,   1537500,         12500},  //LDO     2 (low range)
};

pm_pwr_range_info_type n600_range[1] =
{
    {1, 2}
};
pm_pwr_volt_info_type n600_volt[3] =
{
    {      0,         0,             0},  //LDO     //invalid range   -- place holder
    { 375000,   1287500,         12500},  //LDO     1 (ultra low range)
    { 750000,   1537500,         12500},  //LDO     2 (low range)
};

pm_pwr_range_info_type pmos_range[1] =
{
    {2, 4}
};
pm_pwr_volt_info_type pmos_volt[5] =
{
    { 0,              0,             0},  //invalid range   -- place holder
    { 0,              0,             0},  //invalid range   -- place holder
    { 750000,   1537500,         12500},  //LDO     2 (low range)
    {1500000,   3075000,         25000},  //LDO     3 (mid range)
    {1750000,   4900000,         50000},  //LDO     4(high range)
};

pm_pwr_range_info_type ln_range[1] =
{
    {0, 1}
};
pm_pwr_volt_info_type ln_volt[2] =
{
    {1380000,   2220000,        120000},  //LN_LDO  0
    { 690000,   1110000,         60000},  //LN_LDO  1
};

pm_pwr_range_info_type nmos_range[1] =
{
    {2, 2}
};
pm_pwr_volt_info_type nmos_volt[3] =
{
    { 0,              0,            0},   //invalid range   -- place holder
    { 0,              0,            0},   //invalid range   -- place holder
    { 750000,   1537500,         12500},  //LDO     2 (low range)
};

pm_pwr_range_info_type ult_buck_range_1[1] =
{
    {0, 1}
};
pm_pwr_volt_info_type ult_buck_volt_1[2] =
{
    { 375000,   1562500,         12500},  //ULT Buck1,2 3 - Range 0
    { 750000,   1525000,         25000},  //ULT Buck1,2 3    Range 1
};

pm_pwr_range_info_type ult_buck_range_2[1] =
{
    {0, 0}
};
pm_pwr_volt_info_type ult_buck_volt_2[1] =
{
    { 1550000,   3125000,         25000},  //ULT Buck 4
};

pm_pwr_range_info_type ult_pmos_range[1] =
{
    {0, 0}
};
pm_pwr_volt_info_type ult_pmos_volt[1] =
{
    { 1750000,  3337500,         12500},  //same for P50, P150, P300 and P600 PLDO

};

pm_pwr_range_info_type ult_nmos_range[1] =
{
    {0, 0}
};
pm_pwr_volt_info_type ult_nmos_volt[1] =
{
    { 375000,   1537500,         12500},  //same for N300, N600 and N1200 NLDO
};


pm_clk_register_info_type clk_reg[1] =
{
    {0x5000, 0x100, 0x044, 0x046, 0x048}
};

pm_pwr_register_info_type smps_reg[1] =
{
    {0x1400, 0x300, 0x008, 0x045, 0x048, 0x046, 0x040, 0x041, 0x054, 0x060, 0x04A, 0x050, 0x063, 0x04C, 0x051, 0x0D0, 0x005}
};

pm_pwr_register_info_type ldo_reg[1] =
{
    {0x4000, 0x100, 0x008, 0x045, 0x048, 0x046, 0x040, 0x041, 0x000, 0x000, 0x000, 0x000, 0x000, 0x04C, 0x000, 0x0D0, 0x005}
};

pm_pwr_register_info_type vs_reg[1] =
{
    {0x8000, 0x100, 0x008, 0x045, 0x048, 0x046, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x04C, 0x000, 0x0D0, 0x005}
};

pm_pwr_register_info_type boost_reg[1] =
{
    {0xA000, 0x100, 0x008, 0x045, 0x000, 0x046, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x0D0, 0x005}
};

pm_pwm_register_info_type pwm_reg[1] =
{
    {0xBC00, 0x100, 0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x047, 0x0D0, 0x0D8, 0x0D9, 0x0DA, 0x0DB}
};

pm_wled_ctrl_register_info_type wled_ctrl_reg[1]=
{
    {0xD800, 0x01, 0x02, 0x03, 0x04, 0x05, 0x08, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x18, 0x19, 0x1A, 0x1B, 0x44, 0x46, 0x48, 0x49, 0x4B, 0x4C, 0x4D, 0x4E, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5D, 0x5E, 0xD0, 0xE5}
};

pm_wled_sink_register_info_type wled_sink_reg[1]=
{
    {0xD900, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x50, 0x51, 0x52, 0x53, 0x56, 0x57, 0x58, 0x60, 0x61, 0x62, 0x63, 0x66, 0x67, 0x68, 0x70, 0x71, 0x72, 0x73, 0x76, 0x77, 0x78, 0x80, 0x81, 0x82, 0x83, 0x86, 0x87, 0x88, 0xD0}
};

pm_wled_register_info_type wled_reg[1] =
{
    {
      wled_ctrl_reg,
      wled_sink_reg
    }
};

pm_ibb_register_info_type  ibb_reg[1]=
{
    {0xDC00, 0x008, 0x009, 0x00A, 0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x047, 0x048, 0x049, 0x04A, 0x04B, 0x04C, 0x04D, 0x04E, 0x04F, 0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x057, 0x058, 0x059, 0x05A, 0x05B, 0x05C, 0x05D, 0x05E, 0x05F, 0x060, 0x061, 0x0D0}
};

pm_lab_register_info_type  lab_reg[1]=
{
    {0xDE00, 0x08, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x5D, 0x5E, 0x5F, 0x60, 0xD0, 0xEB}
};

pm_clk_sleep_register_info_type clk_sleep_reg[1] =
{
    // 1.smp_ctl1 - 2.cal_rc4
    {  0x048,       0x05B}
};

pm_clk_xo_register_info_type clk_xo_reg[1] =
{
    // 1.model_ctl1 - 2.xo_adj_fine
    {  0x044,         0x05C}
};

pm_clk_type clk_common[PM_MAX_NUM_CLKS] =
{
   PM_CLK_XO,
   PM_CLK_BB_1,
   PM_CLK_BB_2,
   PM_CLK_INVALID,
   PM_CLK_RF_1,
   PM_CLK_RF_2,
   PM_CLK_RF_3,
   PM_CLK_DIFF_1,
   PM_CLK_LN_BB,
   PM_CLK_DIST,
   PM_CLK_SLEEP,
   PM_CLK_DIV_1,
   PM_CLK_DIV_2,
   PM_CLK_DIV_3,
};

pm_gpio_register_info_type gpio_reg[1] =
{
    {0xC000, 0x100, 0x008, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01A, 0x01B, 0x040, 0x042, 0x043, 0x045, 0x041, 0x046}
};

pm_mpp_register_info_type mpp_reg[1] =
{
    {0xA000, 0x100, 0x008, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01A, 0x01B, 0x040, 0x042, 0x043, 0x041, 0x046, 0x04C, 0x04A, 0x048}
};
pm_rgb_register_info_type rgb_reg[1] =
{
    {0xD000, 0x100, 0x045, 0x046, 0x047}
};

pm_lpg_register_info_type lpg_reg[1] =
{
    {0xB000, 0x100, 0x040, 0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x057,0x040,0x041,0x0C8}
};

pm_vib_register_info_type vib_reg[1] =
{
    {0xC000, 0x100, 0x008, 0x041, 0x046}
};

pm_smbb_register_info_type smbb_reg[1] =
{
    {0x1000, 0x100, 0x00B, 0x044, 0x049, 0x04B, 0x04D, 0x050, 0x052, 0x054, 0x064, 0x0D0, 0x0EE, 0x008, 0x09, 0x010,0x043, 0x044, 0x051, 0x00A, 0x042}
};

pm_lbc_register_info_type lbc_reg[1] =
{
    {0x1000, 0x100, 0x008, 0x040, 0x041, 0x044, 0x045, 0x047, 0x049, 0x04D, 0x050, 0x052, 0x055, 0x064, 0x065, 0x0D0, 0x0EE, 0x008, 0x09, 0x047, 0x04E, 0x042}
};

pm_pbs_client_register_info_type pbs_client_reg[1] =
{
    {0x7100, 0x100, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0018, 0x0019, 0x0040, 0x0046}
};

pm_coincell_register_info_type coincell_reg[1] =
{
    {0x2800, 0x100, 0x046, 0x044, 0x045 }
};

