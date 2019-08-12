/*! \file pm_config_target.c
 *
 *  \brief This file contains customizable target specific driver settings & PMIC registers.
 *         This file info is taken from Target Component Resource Specific Settings from PDM
 *         that is maintained for each of the targets separately.
 *
 *  &copy; Copyright 2013 - 2015 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/config/mdm9x45/pm_config_target.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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

uint32   num_of_ldo[]        = {18};
uint32   num_of_smps[]       = {5};

/* LpHpCurrentThreshold, SafetyHeadRoom, IsSawControlled, AccessAllowed, AlwaysOn, MinPwrMode, BypassDisallowed, DroopDetect, MinVoltage, MaxVoltage, MinSettlingTime, SettlingErrorEnabled, SettlingEnabled, Reserved */
pm_rpm_ldo_rail_info_type ldo_rail_a[] = 
{
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1200, 1300, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO1 N300
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1200, 1200, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO2 N600
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_ALLOWED,    PM_DROOP_DETECT_DIS, 1000, 1000, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO3 N1200
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_ALLOWED,    PM_DROOP_DETECT_DIS,  950, 1000, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO4 N300
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO5 P150
    { 5,  25, 0, PM_ACCESS_DISALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO6 P600
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO7 P50
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO8 P150 
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS,  600, 1200, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO9 N1200
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_ALLOWED,    PM_DROOP_DETECT_DIS, 3000, 3300, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO10 P50
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_ALLOWED,    PM_DROOP_DETECT_DIS, 1700, 3050, 0, PM_SETTLING_ERR_DIS,PM_SETTLING_EN, 0}, // LDO11 P150
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_ALLOWED,    PM_DROOP_DETECT_DIS, 2700, 2700, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO12 P50
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_ALLOWED,    PM_DROOP_DETECT_DIS, 1700, 3050, 0, PM_SETTLING_ERR_DIS,PM_SETTLING_EN, 0}, // LDO13 P150
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_ALLOWED,    PM_DROOP_DETECT_DIS, 1800, 2850, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO14 P50
    { 5,  25, 0, PM_ACCESS_DISALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO15 P300
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_ALLOWED,    PM_DROOP_DETECT_DIS,  750, 1000, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO16 N300
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_ALLOWED,    PM_DROOP_DETECT_DIS, 1800, 1800, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO17 LN_LDO (XO)
    { 5,  25, 0, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_ALLOWED,    PM_DROOP_DETECT_DIS, 1800, 1800, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO18 LN_LDO (RF)
};

/* LpHpCurrentThreshold, IsSawControlled, AccessAllowed, AlwaysOn, MinPwrMode, MinFreq, MaxFreq, MinVoltage, MaxVoltage, MinSettlingTime, SettlingErrorEnabled, SettlingEnabled, Reserved */
pm_rpm_smps_rail_info_type smps_rail_a[] = 
{
    {350, 0, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_SMPS__AUTO,  PM_CLK_2p13_MHz, PM_CLK_2p13_MHz,PM_DROOP_DETECT_DIS,  650, 1150, 0, PM_SETTLING_ERR_EN, PM_SETTLING_DIS, 0}, //HFS1
    {100, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_SMPS__AUTO,  PM_CLK_1p6_MHz,  PM_CLK_1p6_MHz, PM_DROOP_DETECT_DIS, 1250, 1300, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, //HFS2
    {100, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_SMPS__AUTO,  PM_CLK_3p2_MHz,  PM_CLK_3p2_MHz, PM_DROOP_DETECT_DIS,  950, 1200, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, //HFS3 
    {100, 0, PM_ACCESS_DISALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_SMPS__AUTO,  PM_CLK_1p6_MHz,  PM_CLK_1p6_MHz, PM_DROOP_DETECT_DIS, 1850, 1900, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, //HFS4
    {100, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_SMPS__AUTO,  PM_CLK_3p2_MHz,  PM_CLK_3p2_MHz, PM_DROOP_DETECT_DIS,  500, 1150, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, //HFS5
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
	{PM_ACCESS_ALLOWED,   PM_NONE},      //DIV_CLK1
	{PM_ACCESS_ALLOWED,   PM_NONE},      //DIV_CLK2
	{PM_ACCESS_ALLOWED,   PM_NONE}       //DIV_CLK3
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
    {0, 0x5146, 0x01}, // BB_CLK1_EN_CTL: Set BBCLK_NOT_FORCE, and follow Pin
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
    {0x0, 0x5146, 0x81},    // BB_CLK1_EN_CTL: Force BBCLK enable and follow Pin
    {0xE, 0x540,  0x01},    // Sending resend all command.
    {0xE, 0x614,  0x01},    // HW bug fix, clear SPMI INT LATCHED bit.
    {0x0, 0xFFFF, 0x00},    // check the end of the array.
};

const pm_sleep_cfg_type sleep_exit_info[] = {
    {exit_xo_shutdown},
    {exit_vdd_min}
};



