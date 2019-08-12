/*! \file pm_config_target.c
 *
 *  \brief This file contains customizable target specific driver settings & PMIC registers.
 *         This file info is taken from Target Component Resource Specific Settings from PDM
 *         that is maintained for each of the targets separately.
 *
 *  &copy; Copyright 2013 - 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/config/msm8909/pm8909/pm_config_target.c#3 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/29/15   akt     Update LDO7 settings to always in NPM and 1.8V(CR-841406)
03/31/15   akt     Pronto Power optimisation by adding delay in LDO10 settling(CR-815669)
02/17/15   mr      Ignore settling timeout error for specific Buck (CR-791585)
01/16/15   akt     WGR4905 retension for RF rails 
01/16/15   akt     Ignore err_fatal for L12 and L18 settling timeout issue (CR-780093)
12/29/14   mr      Ignore err_fatal for L8 settling timeout issue. (CR-769060)
12/12/14   akt     Removed the CXO cntl in VDD_MIN path as it's done in PBS RAM
11/21/14   akt     Disable settling error for UIM rails (CR-760788)
10/24/14   akt     Disable settling error for UIM rails (CR-745913)
10/24/14   akt     Removed always ON from L10, updated range to 3.3v (CR-746838)
10/24/14   akt     Removed always ON from L14, updated range to 3.3v (CR-743004)
01/17/14   kt      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pmapp_npa.h"
#include "pm_npa.h"
#include "pm_target_information.h"
#include "pm_rpm_ldo_trans_apply.h"
#include "pm_rpm_smps_trans_apply.h"
#include "pm_rpm_clk_buffer_trans_apply.h"
#include "pm_config.h"
#include "pm_sleep_config.h"

uint32   num_of_ldo[]        = {20};
uint32   num_of_smps[]       = {2};

/* LpHpCurrentThreshold, SafetyHeadRoom, IsSawControlled, AccessAllowed, AlwaysOn, MinPwrMode, BypassDisallowed, DroopDetect, MinVoltage, MaxVoltage, MinSettlingTime, SettlingErrorEnabled, SettlingEnabled, Reserved */
pm_rpm_ldo_rail_info_type ldo_rail_a[] =
{
    {5, 63,  0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1000, 1288, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO1   ULT N600_Stepper
    {5, 63,  0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1200, 1300, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO2   ULT N600_Stepper
    {5, 63,  0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS,  750, 1350, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO3   ULT N600_Stepper
    {5, 150, 0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1900, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO4   LDO LV_P600
    {5, 150, 0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO5   LDO LV_P300
    {5, 150, 0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO6   LDO LV_P300
    {5, 150, 0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__NPM, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO7   LDO LV_P300
    {5, 50,  0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 2900, 2900, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO8   ULT P600
    {5, 50,  0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 3000, 3300, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO9   ULT P600
    {5, 50,  0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1250, 1350, 350, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO10  ULT N300_STEPPER
    {5, 50,  0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 2950, 2950, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO11  ULT P600
    {5, 50,  0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 2950, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO12  ULT P150
    {5, 50,  0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 3075, 3075, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO13  ULT P50
    {5, 50,  0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 3300, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO14  ULT P50
    {5, 50,  0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 3300, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO15  ULT P50
    {5, 50,  0, PM_ACCESS_DISALLOWED, PM_NONE,   PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 3300, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO16   P50 - removed in 8909
    {5, 50,  0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 2850, 2850, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO17  ULT P600
    {5, 50,  0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 2700, 2850, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO18  ULT P150
    {5, 50,  0, PM_ACCESS_DISALLOWED, PM_NONE,   PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1740, 2150, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO19  LDO XO LD - - LN_LDO  Ned to check the curent raing
    {5, 50,  0, PM_ACCESS_DISALLOWED, PM_NONE,   PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1740, 2150, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO20  LDO RFCLK LDO - LN_LDO  Ned to check the curent raing
};

/* LpHpCurrentThreshold, IsSawControlled, AccessAllowed, AlwaysOn, MinPwrMode, MinFreq, MaxFreq, MinVoltage, MaxVoltage, MinSettlingTime, SettlingErrorEnabled, SettlingEnabled, Reserved */
pm_rpm_smps_rail_info_type smps_rail_a[] =
{
    {300, 0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_SMPS__NPM, PM_CLK_3p2_MHz, PM_CLK_3p2_MHz, PM_DROOP_DETECT_DIS,  900, 1350, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // ULT BUCK CTL1
    {300, 0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_SMPS__NPM, PM_CLK_1p6_MHz, PM_CLK_1p6_MHz, PM_DROOP_DETECT_DIS, 1850, 2150, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // ULT BUCK CTL4
};

/* AccessAllowed, AlwaysOn */
pm_rpm_clk_info_type clk_info_a[]=
{
    //{PM_ACCESS_DISALLOWED, PM_NONE},   //XO *managed by SBL settings
	{PM_ACCESS_DISALLOWED,PM_ALWAYS_ON}, //BB_CLK1 *PBS puts in PIN control
	{PM_ACCESS_ALLOWED,   PM_NONE},      //BB_CLK2
	{PM_ACCESS_ALLOWED,   PM_NONE},      //EMPTY
	{PM_ACCESS_ALLOWED,   PM_NONE},	     //RF_CLK1
	{PM_ACCESS_ALLOWED,   PM_NONE},      //RF_CLK2
	{PM_ACCESS_ALLOWED,   PM_NONE},      //RF_CLK3
	{PM_ACCESS_ALLOWED,   PM_NONE},      //DIFF_CLK1
	{PM_ACCESS_ALLOWED,   PM_NONE},      //EMPTY
	{PM_ACCESS_DISALLOWED,PM_NONE},	     //CLK_DIST *not supported
	{PM_ACCESS_DISALLOWED,PM_ALWAYS_ON}, //SLEEP_CLK
	{PM_ACCESS_DISALLOWED,PM_ALWAYS_ON}, //DIV_CLK1
	{PM_ACCESS_DISALLOWED,PM_ALWAYS_ON}, //DIV_CLK2
	{PM_ACCESS_DISALLOWED,PM_ALWAYS_ON}  //DIV_CLK3
};

pm_rpm_ldo_rail_info_type* ldo_rail[]=
{
    ldo_rail_a
};

pm_rpm_smps_rail_info_type* smps_rail[]=
{
    smps_rail_a
};

pm_rpm_clk_info_type* clk_info[]=
{
    clk_info_a
};

/* Sleep configuration for enter XO_SHUTDOWN or VDD_MIN. */
static const pm_sleep_reg_type enter_xo_shutdown[] =
{
    {0, 0x5146, 0x01}, // BB_CLK1_EN_CTL: Set BBCLK_NOT_FORCE, and follow Pin
    {0, 0xFFFF, 0x00}, // invalid setting, used to check the end of the array.
};

static const pm_sleep_reg_type enter_vdd_min[] =
{
    {0, 0xFFFF, 0x00}, // invalid setting, used to check the end of the array.
};

const pm_sleep_cfg_type sleep_enter_info[] = {
    {enter_xo_shutdown},
    {enter_vdd_min}
};

static const pm_sleep_reg_type exit_xo_shutdown[] =
{
    {0, 0x5146, 0x81}, // BB_CLK1_EN_CTL: Force BBCLK enable and follow Pin
    {0, 0xFFFF, 0x00}, // invalid setting, used to check the end of the array.
};

static const pm_sleep_reg_type exit_vdd_min[] =
{
    {0xE, 0x540,  0x01},    // Sending resend all command.
    {0xE, 0x614,  0x01},    // HW bug fix, clear SPMI INT LATCHED bit.
    {0x0, 0xFFFF, 0x00},    // check the end of the array.
};

const pm_sleep_cfg_type sleep_exit_info[] = {
    {exit_xo_shutdown},
    {exit_vdd_min}
};



