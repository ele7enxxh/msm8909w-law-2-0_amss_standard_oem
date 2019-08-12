/*! \file
*  \n
*  \brief  pm_spmi_config_pma8916.c
*  \n
*  \n This file contains pmic configuration data specific for SPMI Controller's
      Peripheral for MSM8962 device..
*  \n
*  \n &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/pmic/config/msm8916_pm8916/src/pm_spmi_config_pm8916.c#1 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
08/11/14   mr     PVC Port config for Buck OCP register. (CR-707637)
08/08/13   vtw    Created.
========================================================================== */

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/
//#include "PmicArbCfg.h"
#include "SpmiCfg.h"

/*-------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* ----------------------------------------------------------------------*/

/* Converts a PVC port index to an overall PMIC Arbiter port index */
#define PVC_PORT_TO_PMIC_ARB_PORT(p) (p+1)

/*   Port assignment in 8974(Aragorn)    */
#define KRAIT_L2_PORT   2   /* Krait L2  */
#define MSS_SAW_PORT    3   /* MSS SAW   */

#define MSS_CPR_PORT    6   /* MSS CPR   */
#define OXILI_PORT      7   /* Oxili CPR */
#define TOP_CPR_PORT    8   /* Top CPR   */




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

/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/

/*
 * pm_pvc_L2_addr
 *
 * PVC port addr for Krait.
 */
static SpmiCfg_Ppid pm_pvc_L2_addr[] =
{
  {0x1, 0x1741}, /* PMIC 8916, Periph S2_CTRL, S2_CTRL_VOLTAGE_CTL2 */
  {0x1, 0x176C}, /* PMIC 8916, Periph S2_CTRL, S2_CTRL_OCP */
  {0x1, 0x1745}, /* PMIC 8916, Periph S2_CTRL, S2_CTRL_MODE_CTL */
  {0x1, 0x1746}, /* PMIC 8916, Periph S2_CTRL, S2_CTRL_EN_CTL */
};

const uint8 pm_arb_priorities[] =
{
    0,                                         /* SW port -- highest priority */
    PVC_PORT_TO_PMIC_ARB_PORT( KRAIT_L2_PORT ) /* port 3, next highest priority */
};

/* Number of pm arb priorities. */
const uint32 pm_num_prio = sizeof(pm_arb_priorities) / sizeof(uint8);

/*
 * pm_arb_pvc_cfg_8916
 *
 * PMIC Arbiter PVC ports config.
 */
const SpmiCfg_PvcPortCfg pm_arb_pvc_cfg_8916[] =
{
  {
    /* .pvcPortId           = */ KRAIT_L2_PORT,
    /* .priority = */ SPMI_ACCESS_PRIORITY_LOW,
    /* .ppids       = */ pm_pvc_L2_addr,
    /* .numPpids           = */ sizeof(pm_pvc_L2_addr)/sizeof(SpmiCfg_Ppid)
  },
};

/* Number of pvc ports. */
const uint32 pm_num_pvc = sizeof(pm_arb_pvc_cfg_8916) / sizeof(SpmiCfg_PvcPortCfg);



