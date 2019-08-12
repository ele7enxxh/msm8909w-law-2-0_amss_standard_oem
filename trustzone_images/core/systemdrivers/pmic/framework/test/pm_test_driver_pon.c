/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                PM TEST GPIO DRIVER

GENERAL DESCRIPTION
  This file contains GPIO Driver test codes.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2014-15        by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:
$DateTime:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/02/14   mr      Created. (CR-713705)
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_test_framework.h"


/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/
/* For TEST/DEBUG purpose */
pm_pon_reset_cfg_type pon_cfg;

/*===========================================================================

                LOCAL FUNCTION DEFINITIONS

===========================================================================*/
/**
 * @name pm_test_driver_pon_l0_level
 *
 * @brief This function tests for PON Driver PS_HOLD APIs' successful return
 *        by passing working params.
 *
 * @param [in] void
 *
 * @param [out] void
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 * @dependencies Should be called after pm_tz_device_init().
 *
 * @sideeffect
 *
 * @note Need to read Periph register values after setting configuration.
 *
 */
pm_err_flag_type pm_test_driver_pon_l0_level (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    test_status.api = PM_PON_PS_HOLD_CFG;
    for(pon_cfg = PM_PON_RESET_CFG_WARM_RESET; pon_cfg < PM_PON_RESET_CFG_INVALID; pon_cfg++)
    {
        switch (pon_cfg)
        {
            /* These PON Reset configs are not supported, so need to skip them. */
            case PM_PON_RESET_CFG_WARM_RESET_AND_D_VDD_BATT_REMOVE_SHUTDOWN:
            case PM_PON_RESET_CFG_WARM_RESET_AND_X_VDD_COIN_CELL_REMOVE_SHUTDOWN:
            case PM_PON_RESET_CFG_WARM_RESET_AND_SHUTDOWN:
            case PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET:
            case PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET:
                break;
            default:
            {
                err_flag = pm_pon_ps_hold_cfg (0, pon_cfg);    /** Ex. PM_PON_RESET_CFG_WARM_RESET: 0x085A = 0x01, 0x085B = 0x80 */
                if(PM_ERR_FLAG__SUCCESS != err_flag)
                {
                    test_status.test_result = 1;
                    return err_flag;
                }
            }
        }
    }

    test_status.api = PM_DISABLE_QFUSE_POWER_RAIL;
    err_flag = pm_disable_qfuse_power_rail();
    if(PM_ERR_FLAG__SUCCESS != err_flag)  return err_flag;

    test_status.api = PM_ENABLE_QFUSE_POWER_RAIL;
    err_flag = pm_enable_qfuse_power_rail();

    return err_flag;
}

/**
 * @name pm_test_driver_pon_l1_level
 *
 * @brief 
 *
 * @param [in] void
 *
 * @param [out] void
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 * @dependencies Should be called after pm_tz_device_init().
 *
 * @sideeffect
 *
 * @note
 *
 */
pm_err_flag_type pm_test_driver_pon_l1_level (void)
{
    return PM_ERR_FLAG__SUCCESS;
}

