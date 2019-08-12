/*! \file pm_config_common.c
 *  
 *  \brief  This file contains peripheral related settings that are common across PMICs.
 *  \n  
 *  &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/config/common/pm_config_common.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/14   akt     Added PM8909/MSM8909 support.
02/22/14   rk      S4 Vmin Change to 1550 mv (CR - 621557)
10/09/13   rk      renaming the ult buck range and volt struct names.
06/11/13   hs      Adding settling time for regulators.
05/08/13   aks     Clk driver code re-factoring 
04/23/13   hs      Fixed the naming convention in \config.
03/04/13   hs      Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pmapp_npa.h"
#include "pm_npa.h"
#include "pm_pwr_alg.h"
#include "pm_clk_driver.h"
#include "pm_rpm_clk_buffer_trans_apply.h"

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

pm_pwr_range_info_type boost_range[1] = 
{
    {0, 0}
};

pm_pwr_volt_info_type boost_volt[1] = 
{
    { 4000000,   5550000,         50000},  //BOOST     0   Band 1
};

pm_pwr_range_info_type boost_byp_range[1] = 
{
    {0, 0}
};

pm_pwr_volt_info_type boost_byp_volt[1] = 
{
    { 2500000, 5650000, 50000},  //BOOST BYPASS    0   Band 1
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

pm_pwr_settling_time_info_type ult_smps_settling_time[1] = 
{
    {500, 10, 12500} // {settling time for off-to-on(uS), settling time per step(uS), voltage size per step(uV)}
};
pm_pwr_settling_time_info_type ult_ldo_settling_time[1] = 
{
    {200, 5, 12500} // {settling time for off-to-on(uS), settling time per step(uS), voltage size per step(uV)}
};

pm_pwr_settling_time_info_type smps_settling_time[1] = 
{
    {500, 1, 1500} // {settling time for off-to-on(uS), settling time per step(uS), voltage size per step(uV)}
};
pm_pwr_settling_time_info_type ldo_settling_time[1] = 
{
    {200, 1, 1500} // {settling time for off-to-on(uS), settling time per step(uS), voltage size per step(uV)}
};
pm_pwr_settling_time_info_type boost_settling_time[1] = 
{
    {200, 0, 0} // {settling time for off-to-on(uS), settling time per step(uS), voltage size per step(uV)}
};
pm_pwr_settling_time_info_type vs_settling_time[1] = 
{
    {1400, 0, 0} // {settling time for off-to-on(uS), settling time per step(uS), voltage size per step(uV)}
};

pm_clk_register_info_type clk_reg[1] = 
{
    {0x5000, 0x100, 0x044, 0x046, 0x048}
};

pm_pwr_register_info_type smps_reg[1] = 
{
    {0x1400, 0x300, 0x008, 0x045, 0x048, 0x046, 0x040, 0x041, 
     0x054, 0x050, 0x063, 0x060, 0x061, 0x051, 0x0D0, 0x000, 
     0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x06C}
};

pm_pwr_register_info_type ldo_reg[1] = 
{
    {0x4000, 0x100, 0x008, 0x045, 0x048, 0x046, 0x040, 0x041, 
     0x000, 0x000, 0x000, 0x04C, 0x000, 0x000, 0x0D0, 0x000, 
     0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x000} 
};

pm_pwr_register_info_type vs_reg[1] = 
{
    {0x8000, 0x100, 0x008, 0x045, 0x048, 0x046, 0x000, 0x000, 
     0x000, 0x000, 0x000, 0x04C, 0x000, 0x000, 0x0D0, 0x000, 
     0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x000}
};

pm_pwr_register_info_type boost_reg[1] = 
{
    {0xA000, 0x100, 0x008, 0x045, 0x000, 0x046, 0x040, 0x041, 
     0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x0D0, 0x000, 
     0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x000}
};

pm_pwr_register_info_type boost_byp_reg[1] = 
{
    {0xA200, 0x100, 0x008, 0x045, 0x000, 0x046, 0x040, 0x041, 
     0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x0D0, 0x042, 
     0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x000}
};

pm_clk_sleep_register_info_type clk_sleep_reg[1] = 
{
//  1.slpsrc_ctl1
    {0x049}
};

pm_clk_xo_register_info_type clk_xo_reg[1] =
{
//	1.timer1
	{0x04C}
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
