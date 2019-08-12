/*! \file  pm_config_rpm_npa_pam.c
 *  
 *  \brief  File Contains the PMIC NPA CMI Code
 *  \details  This file contains the needed definition and enum for PMIC NPA layer.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: pmd9635_05_28_2014_v01 - LOCKED

 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2014 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/config/mdm9x45/pm_config_rpm_npa_pam.c#1 $ 
$DateTime: 2015/02/25 02:53:43 $  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/

/*===========================================================================

                        INCLUDE HEADER FILES

===========================================================================*/

#include "pm_npa_device_ldo.h"
#include "pm_npa.h"
#include "pm_rpm_npa.h"
#include "pm_rpm_npa_device.h"
/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/

/* RPM_INIT Client */
static pm_npa_ldo_int_rep
pm_rpm_pam_rpm_init_a_ldo2 [] =
{
   /**< Mode: PMIC_NPA_MODE_ID_RPM_INIT_MODE_0 */
   /**< Comments: Settings needed when we enter sleep.  */
   {
      PM_NPA_GENERIC_ENABLE, /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_SW_MODE_LDO__IPEAK, /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_PIN_CONTROL_ENABLE__NONE, /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_PIN_CONTROL_POWER_MODE__NONE, /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      2, /**< Perpherial Index */
      0, /**< Primary (0) or Secondary (1) PMIC */
      0, /**< If((old sw_en == disable) && (new sw_en == enable) || new pc_en == enable then ldo_en_trans = true else ldo_en_trans = false */
      PM_NPA_BYPASS_ALLOWED, /**< [Allowed (default), Disallowed]*/
      0, /**< reserve 1 - for 32 bit boundary */
      0, /**< [X uV] -> max aggregation. */
      0, /**< [X mA] -> summed aggregation. */
      0, /**< [X uV] -> voltage headroom needed. */
      1200000, /**< [X uV] -> max aggregation. */
   },
   /**< Mode: PMIC_NPA_MODE_ID_RPM_INIT_MODE_1 */
   /**< Comments: Settings needed when we exit sleep.  */
   {
      PM_NPA_GENERIC_ENABLE, /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_SW_MODE_LDO__NPM, /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_PIN_CONTROL_ENABLE__NONE, /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_PIN_CONTROL_POWER_MODE__NONE, /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      2, /**< Perpherial Index */
      0, /**< Primary (0) or Secondary (1) PMIC */
      0, /**< If((old sw_en == disable) && (new sw_en == enable) || new pc_en == enable then ldo_en_trans = true else ldo_en_trans = false */
      PM_NPA_BYPASS_ALLOWED, /**< [Allowed (default), Disallowed]*/
      0, /**< reserve 1 - for 32 bit boundary */
      0, /**< [X uV] -> max aggregation. */
      0, /**< [X mA] -> summed aggregation. */
      0, /**< [X uV] -> voltage headroom needed. */
      1200000, /**< [X uV] -> max aggregation. */
   },
};

pm_npa_pam_client_cfg_type
pm_rpm_pam_rpm_init_rails_info [] =
{
   {
      (void*)pm_rpm_pam_rpm_init_a_ldo2,
      PM_NPA_VREG_LDO
   },
};
