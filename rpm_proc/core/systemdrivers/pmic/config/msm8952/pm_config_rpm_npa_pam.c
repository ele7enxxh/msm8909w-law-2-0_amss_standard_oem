/*! \file  pm_config_rpm_npa_pam.c
 *  
 *  \brief  File Contains the PMIC NPA CMI Code
 *  \details  This file contains the needed definition and enum for PMIC NPA layer.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: MSM8952_PM8950_NPA_v1p00_2014_12_15 - LOCKED

 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2014 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/config/msm8952/pm_config_rpm_npa_pam.c#1 $ 
$DateTime: 2015/02/25 02:53:43 $  $Author: pwbldsvc $

when        who      what, where, why
--------    ---      ----------------------------------------------------------
	
===========================================================================*/

/*===========================================================================

                        INCLUDE HEADER FILES

===========================================================================*/
#include "pm_npa.h"

#include "pm_npa_device_clk_buff.h"
#include "pm_npa_device_ldo.h"
#include "pm_npa_device_smps.h"

#include "pm_rpm_npa.h"
#include "pm_rpm_npa_device.h"
/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/

/* LPDDR Client */
static pm_npa_ldo_int_rep
pm_rpm_pam_lpddr_a_ldo2[] =
{
    /**< Mode: PMIC_NPA_MODE_ID_DDR_SLEEP_SPEED*/
{
      PM_NPA_GENERIC_DISABLE, /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_SW_MODE_LDO__IPEAK, /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_PIN_CONTROL_ENABLE__NONE, /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_PIN_CONTROL_POWER_MODE__NONE, /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      2, /**< Perpherial Index */
      0, /**< Primary (0) or Secondary (1) PMIC */
      0, /**< If((old sw_en == disable) && (new sw_en == enable) || new pc_en == enable then ldo_en_trans = true else ldo_en_trans = false */
      PM_NPA_BYPASS_DISALLOWED, /**< [Allowed (default), Disallowed]*/
      0, /**< reserve 1 - for 32 bit boundary */
      0, /**< [X uV] -> max aggregation. */
      0, /**< [X mA] -> summed aggregation. */
      0, /**< [X uV] -> voltage headroom needed. */
      0, /**< [X uV] -> max aggregation. */
      //PM_NPA_CORNER_MODE__NONE, /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    /**< Mode: PMIC_NPA_MODE_ID_DDR_LOW_SPEED*/
   {
      PM_NPA_GENERIC_ENABLE, /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_SW_MODE_LDO__NPM, /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_PIN_CONTROL_ENABLE__NONE, /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_PIN_CONTROL_POWER_MODE__NONE, /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      2, /**< Perpherial Index */
      0, /**< Primary (0) or Secondary (1) PMIC */
      0, /**< If((old sw_en == disable) && (new sw_en == enable) || new pc_en == enable then ldo_en_trans = true else ldo_en_trans = false */
      PM_NPA_BYPASS_DISALLOWED, /**< [Allowed (default), Disallowed]*/
      0, /**< reserve 1 - for 32 bit boundary */
      0, /**< [X uV] -> max aggregation. */
      300, /**< [X mA] -> summed aggregation. */
      62, /**< [X uV] -> voltage headroom needed. */
      1200000, /**< [X uV] -> max aggregation. */
      //PM_NPA_CORNER_MODE__NONE, /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    /**< Mode: PMIC_NPA_MODE_ID_DDR_MID_SPEED*/
   {
      PM_NPA_GENERIC_ENABLE, /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_SW_MODE_LDO__NPM, /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_PIN_CONTROL_ENABLE__NONE, /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_PIN_CONTROL_POWER_MODE__NONE, /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      2, /**< Perpherial Index */
      0, /**< Primary (0) or Secondary (1) PMIC */
      0, /**< If((old sw_en == disable) && (new sw_en == enable) || new pc_en == enable then ldo_en_trans = true else ldo_en_trans = false */
      PM_NPA_BYPASS_DISALLOWED, /**< [Allowed (default), Disallowed]*/
      0, /**< reserve 1 - for 32 bit boundary */
      0, /**< [X uV] -> max aggregation. */
      400, /**< [X mA] -> summed aggregation. */
      100, /**< [X uV] -> voltage headroom needed. */
      1200000, /**< [X uV] -> max aggregation. */
      //PM_NPA_CORNER_MODE__NONE, /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    /**< Mode: PMIC_NPA_MODE_ID_DDR_HIGH_SPEED*/
   {
      PM_NPA_GENERIC_ENABLE, /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_SW_MODE_LDO__NPM, /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_PIN_CONTROL_ENABLE__NONE, /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_PIN_CONTROL_POWER_MODE__NONE, /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      2, /**< Perpherial Index */
      0, /**< Primary (0) or Secondary (1) PMIC */
      0, /**< If((old sw_en == disable) && (new sw_en == enable) || new pc_en == enable then ldo_en_trans = true else ldo_en_trans = false */
      PM_NPA_BYPASS_DISALLOWED, /**< [Allowed (default), Disallowed]*/
      0, /**< reserve 1 - for 32 bit boundary */
      0, /**< [X uV] -> max aggregation. */
      700, /**< [X mA] -> summed aggregation. */
      150, /**< [X uV] -> voltage headroom needed. */
      1225000, /**< [X uV] -> max aggregation. */
      //PM_NPA_CORNER_MODE__NONE, /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
};
static pm_npa_ldo_int_rep
pm_rpm_pam_lpddr_a_ldo3[] =
{
    /**< Mode: PMIC_NPA_MODE_ID_DDR_SLEEP_SPEED*/
{
      PM_NPA_GENERIC_DISABLE, /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_SW_MODE_LDO__IPEAK, /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_PIN_CONTROL_ENABLE__NONE, /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_PIN_CONTROL_POWER_MODE__NONE, /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      3, /**< Perpherial Index */
      0, /**< Primary (0) or Secondary (1) PMIC */
      0, /**< If((old sw_en == disable) && (new sw_en == enable) || new pc_en == enable then ldo_en_trans = true else ldo_en_trans = false */
      PM_NPA_BYPASS_ALLOWED, /**< [Allowed (default), Disallowed]*/
      0, /**< reserve 1 - for 32 bit boundary */
      0, /**< [X uV] -> max aggregation. */
      0, /**< [X mA] -> summed aggregation. */
      0, /**< [X uV] -> voltage headroom needed. */
      0, /**< [X uV] -> max aggregation. */
      //PM_NPA_CORNER_MODE__NONE, /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    /**< Mode: PMIC_NPA_MODE_ID_DDR_LOW_SPEED*/
   {
      PM_NPA_GENERIC_DISABLE, /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_SW_MODE_LDO__IPEAK, /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_PIN_CONTROL_ENABLE__NONE, /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_PIN_CONTROL_POWER_MODE__NONE, /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      3, /**< Perpherial Index */
      0, /**< Primary (0) or Secondary (1) PMIC */
      0, /**< If((old sw_en == disable) && (new sw_en == enable) || new pc_en == enable then ldo_en_trans = true else ldo_en_trans = false */
      PM_NPA_BYPASS_DISALLOWED, /**< [Allowed (default), Disallowed]*/
      0, /**< reserve 1 - for 32 bit boundary */
      0, /**< [X uV] -> max aggregation. */
      0, /**< [X mA] -> summed aggregation. */
      62, /**< [X uV] -> voltage headroom needed. */
      0, /**< [X uV] -> max aggregation. */
      //PM_NPA_CORNER_MODE__NONE, /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    /**< Mode: PMIC_NPA_MODE_ID_DDR_MID_SPEED*/
   {
      PM_NPA_GENERIC_DISABLE, /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_SW_MODE_LDO__IPEAK, /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_PIN_CONTROL_ENABLE__NONE, /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_PIN_CONTROL_POWER_MODE__NONE, /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      3, /**< Perpherial Index */
      0, /**< Primary (0) or Secondary (1) PMIC */
      0, /**< If((old sw_en == disable) && (new sw_en == enable) || new pc_en == enable then ldo_en_trans = true else ldo_en_trans = false */
      PM_NPA_BYPASS_DISALLOWED, /**< [Allowed (default), Disallowed]*/
      0, /**< reserve 1 - for 32 bit boundary */
      0, /**< [X uV] -> max aggregation. */
      0, /**< [X mA] -> summed aggregation. */
      100, /**< [X uV] -> voltage headroom needed. */
      0, /**< [X uV] -> max aggregation. */
      //PM_NPA_CORNER_MODE__NONE, /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    /**< Mode: PMIC_NPA_MODE_ID_DDR_HIGH_SPEED*/
   {
      PM_NPA_GENERIC_DISABLE, /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_SW_MODE_LDO__IPEAK, /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_PIN_CONTROL_ENABLE__NONE, /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_PIN_CONTROL_POWER_MODE__NONE, /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      3, /**< Perpherial Index */
      0, /**< Primary (0) or Secondary (1) PMIC */
      0, /**< If((old sw_en == disable) && (new sw_en == enable) || new pc_en == enable then ldo_en_trans = true else ldo_en_trans = false */
      PM_NPA_BYPASS_DISALLOWED, /**< [Allowed (default), Disallowed]*/
      0, /**< reserve 1 - for 32 bit boundary */
      0, /**< [X uV] -> max aggregation. */
      0, /**< [X mA] -> summed aggregation. */
      150, /**< [X uV] -> voltage headroom needed. */
      0, /**< [X uV] -> max aggregation. */
      //PM_NPA_CORNER_MODE__NONE, /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
};

pm_npa_pam_client_cfg_type
pm_rpm_pam_lpddr_rails_info []=
{
   {
      (void*)pm_rpm_pam_lpddr_a_ldo2,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_rpm_pam_lpddr_a_ldo3,
      PM_NPA_VREG_LDO
   },
};



