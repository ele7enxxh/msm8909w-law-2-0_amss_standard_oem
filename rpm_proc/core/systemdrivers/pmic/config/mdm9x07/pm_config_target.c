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

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/config/mdm9x07/pm_config_target.c#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/16   ps      Updated L3 to NPM always for ADC (CR-997255)
12/16/15   sv      Updated LDO10 min max voltage range to 1.0V - 1.05V. (CR-951315)
10/19/15   sv      Disabled settling error for LDO's and SMPS's (CR-924032)
10/01/15   sv      Made LDO3 ALWAYS ON. (CR-908379)
09/23/15   sv      S1 is SAW controlled. (CR-912065)
10/01/15   sv      Updated L13 min max voltage limits. (CR-903954)
08/04/15   ps      Fixed S3 clock, S2/L12 max voltages (CR-896758)
07/28/15   ps      Added support for 9x07 target (CR-883336)
02/17/15   mr      Ignore settling timeout error for specific Buck (CR-791585)
02/09/15   akt     PM8916 workaround for LDO11 (CR-686702)
02/04/15   akt     Updated to ignore the rails for PMOS LDOs (CR-787855)
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

uint32   num_of_ldo[]        = {16};
uint32   num_of_smps[]       = {4};

/* LpHpCurrentThreshold, SafetyHeadRoom, IsSawControlled, AccessAllowed, AlwaysOn, MinPwrMode, BypassDisallowed, DroopDetect, MinVoltage, MaxVoltage, MinSettlingTime, SettlingErrorEnabled, SettlingEnabled, Reserved */
pm_rpm_ldo_rail_info_type ldo_rail_a[] =
{
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1250, 1250, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO1   N1200_Stepper
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO2   P50
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO3   LV_P150/P50?
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 3075, 3075, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO4   P50
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1700, 3050, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO5   P150
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1700, 3050, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO6   P150
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1700, 1900, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO7   P150
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO8   P600
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1200, 1250, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO9   N1200_Stepper
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1000, 1050, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO10  N1200_Stepper
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO11  LV_P300
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS,  675, 1350, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO12  N1200_Stepper, fix in TCRSS (1150)
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 2950, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO13  P50
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 2650, 3000, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO14  P50
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO15  P50
    {5, 25,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800, 1800, 0,   PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, // LDO16  P50
};

/* LpHpCurrentThreshold, IsSawControlled, AccessAllowed, AlwaysOn, MinPwrMode, MinFreq, MaxFreq, MinVoltage, MaxVoltage, MinSettlingTime, SettlingErrorEnabled, SettlingEnabled, Reserved */
pm_rpm_smps_rail_info_type smps_rail_a[] =
{
/* S1 = APC rail, SVS min:1050mv, Turbo max:1350mv */
    {100, 1, PM_ACCESS_ALLOWED,    PM_NONE,      PM_NPA_SW_MODE_SMPS__AUTO, PM_CLK_1p6_MHz, PM_CLK_1p6_MHz, PM_DROOP_DETECT_DIS,  900, 1350, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, //HF2 BUCK S1, APC
/* S2 = Low-voltage sub-regulation S2 = L12 (MX) + 50mv (headroom) MX SVS min:900mv, Turbo max:1350mv */
    {100, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_SMPS__AUTO, PM_CLK_1p6_MHz, PM_CLK_1p6_MHz, PM_DROOP_DETECT_DIS,  750, 1400, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, //HF2 BUCK S2, LV sub-regulation
/* S3 = CX rail, SVS min:900mv, Turbo max:1350mv */
    {100, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_SMPS__AUTO, PM_CLK_2p13_MHz, PM_CLK_2p13_MHz, PM_DROOP_DETECT_DIS,  550, 1350, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, //HF2 BUCK S3, CX
/* S4 = High-voltage sub-regulation */
    {100, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_SMPS__AUTO, PM_CLK_1p6_MHz, PM_CLK_1p6_MHz, PM_DROOP_DETECT_DIS, 1800, 1950, 0, PM_SETTLING_ERR_DIS, PM_SETTLING_EN, 0}, //HF2 BUCK S4, HV sub-regulation
};

/* AccessAllowed, AlwaysOn */
pm_rpm_clk_info_type clk_info_a[]=
{
    //{PM_ACCESS_DISALLOWED, PM_NONE},      //XO *managed by SBL settings
    {PM_ACCESS_DISALLOWED,   PM_ALWAYS_ON}, //BB_CLK1 *PBS puts in PIN control
    {PM_ACCESS_DISALLOWED,   PM_NONE},      //BB_CLK2, not supported
    {PM_ACCESS_DISALLOWED,   PM_NONE},      //EMPTY
    {PM_ACCESS_ALLOWED,      PM_NONE},      //RF_CLK1
    {PM_ACCESS_ALLOWED,      PM_NONE},      //RF_CLK2
    {PM_ACCESS_DISALLOWED,   PM_NONE},      //RF_CLK3, not supported
    {PM_ACCESS_ALLOWED,      PM_NONE},      //DIFF_CLK1
    {PM_ACCESS_ALLOWED,      PM_NONE},      //EMPTY
    {PM_ACCESS_DISALLOWED,   PM_NONE},      //CLK_DIST *not supported
    {PM_ACCESS_DISALLOWED,   PM_ALWAYS_ON}, //SLEEP_CLK
    {PM_ACCESS_DISALLOWED,   PM_ALWAYS_ON}, //DIV_CLK1
    {PM_ACCESS_DISALLOWED,   PM_ALWAYS_ON}, //DIV_CLK2
    {PM_ACCESS_DISALLOWED,   PM_ALWAYS_ON}  //DIV_CLK3
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

