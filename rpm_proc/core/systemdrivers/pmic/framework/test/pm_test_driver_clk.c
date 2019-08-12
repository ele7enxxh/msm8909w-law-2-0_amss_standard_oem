/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             PM CLK DRIVER TEST

GENERAL DESCRIPTION
  This file contains Driver Framework test codes.

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
09/03/14   mr      Added PMIC Framework/Driver Test support. (CR-713705)
===========================================================================*/

/*===========================================================================

                INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_test_framework.h"


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/
 /**
 * @name pm_test_driver_clk_level_0
 *
 * @brief This function tests for APIs' successful return.
 *
 * @param [in] void
 *
 * @param [out] void
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 * @dependencies Should be called after pm_device_init().
 *
 * @sideeffect
 *
 * @note
 *
 */
pm_err_flag_type pm_test_driver_clk_level_0 (void)
{
    uint8 i;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_clk_type m;

    g_pm_test_status[g_f_cnt].api = PM_CLK_DRV_STRENGTH;
    for(m = PM_CLK_SLEEP, i = 0; m < PM_ALL_CLKS;  m++, i++)
    {
        switch (m)
        {
            case PM_CLK_SLEEP:
            case PM_CLK_XO:
            case PM_CLK_RF_1:
            case PM_CLK_RF_2:
            case PM_CLK_BB_1:
            case PM_CLK_BB_2:
            case PM_CLK_DIST:       /* PM_CLK_DIV_1/2/3 don't support change of Drive Strength */
                err_flag = pm_clk_drv_strength(0, m, PM_CLK_DRV_STRENGTH_2X);
                if(PM_ERR_FLAG__SUCCESS != err_flag && PM_ERR_FLAG__FEATURE_NOT_SUPPORTED != err_flag)
                {
                    pm_test_handle_error(i, err_flag);
                }
                break;
            default:
                continue;
        }
    }

    g_pm_test_status[g_f_cnt].api = PM_CLK_SW_ENABLE;
    for(m = PM_CLK_SLEEP, i = 0; m < PM_ALL_CLKS;  m++, i++)
    {
        switch (m)
        {
            case PM_CLK_SLEEP:
            case PM_CLK_XO:
            case PM_CLK_RF_1:
            case PM_CLK_RF_2:
            case PM_CLK_BB_1:
            case PM_CLK_BB_2:
            case PM_CLK_DIV_1:
            case PM_CLK_DIV_2:
            case PM_CLK_DIV_3:
            case PM_CLK_DIST:
                err_flag = pm_clk_sw_enable(0, m, PM_ON);
                if(PM_ERR_FLAG__SUCCESS != err_flag && PM_ERR_FLAG__FEATURE_NOT_SUPPORTED != err_flag)
                {
                    pm_test_handle_error(i, err_flag);
                }
                break;
            default:
                continue;
        }
    }

    return err_flag;
}

