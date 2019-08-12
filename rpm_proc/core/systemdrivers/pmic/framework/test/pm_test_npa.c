/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             PM DRIVER FRAMEWORK TEST

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

                GLOBAL VARIABLES DEFINITIONS

===========================================================================*/
kvp_t *kvp;


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/
/**
 * @name pm_test_npa_chk_ldo_level_1
 *
 * @brief This function tests for LDO Proxy Votes.
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
pm_err_flag_type pm_test_npa_chk_ldo_level_1 (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 i = 0, is_same = 0;

    g_pm_test_status[g_f_cnt].api = PM_LDO_VOLT_LEVEL_STATUS;
	g_volt_lvl = 0; g_prev_volt_lvl = 0; 
    for(i = 0; i < g_pm_test_ldo_num; i++)      /* L16 is not present */
    {
        err_flag = pm_ldo_volt_level_status(0, i, &g_volt_lvl);
        if(PM_ERR_FLAG__SUCCESS != err_flag || 0 == g_volt_lvl)
        {
            pm_test_handle_error(i, err_flag);
        }
        else
        {
            g_prev_volt_lvl = g_volt_lvl;

            if (g_volt_lvl != (ldo_rail_a[i].MaxVoltage * 1000))
            {
                g_volt_lvl = ldo_rail_a[i].MaxVoltage * 1000;     /* convert into mV */
            }
            else
            {
                is_same = 1;
            }

            kvp_clear(kvp);
            kvp_put(kvp, 0x7675, sizeof(uint32), &g_volt_lvl);
            rpm_send_init_proxy_vote(RPM_LDO_A_REQ, i+1, 0, kvp);  /* sending vote with Max voltage level */

            g_volt_lvl = 0;
            err_flag = pm_ldo_volt_level_status(0, i, &g_volt_lvl);
            if(PM_ERR_FLAG__SUCCESS != err_flag ||
               (0 == is_same && g_prev_volt_lvl == g_volt_lvl))
            {
                pm_test_handle_error(i, err_flag);
            }
        }
		g_volt_lvl = 0; g_prev_volt_lvl = 0; is_same = 0;
    }

    return err_flag;
}

/**
 * @name pm_test_npa_chk_smps_level_1
 *
 * @brief This function tests for SMPS Proxy Votes.
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
pm_err_flag_type pm_test_npa_chk_smps_level_1 (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 i = 0, is_same = 0;

    g_pm_test_status[g_f_cnt].api = PM_SMPS_VOLT_LEVEL_STATUS;
	g_volt_lvl = 0; g_prev_volt_lvl = 0; 
    for(i = 0; i < g_pm_test_smps_num; i++)
    {
        err_flag = pm_smps_volt_level_status(0, i, &g_volt_lvl);
        if(PM_ERR_FLAG__SUCCESS != err_flag || 0 == g_volt_lvl)
        {
            pm_test_handle_error(i, err_flag);
        }
        else
        {
            g_prev_volt_lvl = g_volt_lvl;

            if (g_volt_lvl != (smps_rail_a[i].MaxVoltage * 1000))
            {
                g_volt_lvl = smps_rail_a[i].MaxVoltage * 1000;        /* Convert into mV */
            }
            else
            {
                is_same = 1;
            }

            kvp_clear(kvp);
            kvp_put(kvp, 0x7675, sizeof(uint32), &g_volt_lvl);  /* sending vote with Max voltage level */
            rpm_send_init_proxy_vote(RPM_SMPS_A_REQ, i+1, 0, kvp);

            g_volt_lvl = 0;
            err_flag = pm_smps_volt_level_status(0, i, &g_volt_lvl);
            if(PM_ERR_FLAG__SUCCESS != err_flag ||
               (0 == is_same && g_prev_volt_lvl == g_volt_lvl))
            {
                pm_test_handle_error(i, err_flag);
            }
        }
		g_volt_lvl = 0; g_prev_volt_lvl = 0; is_same = 0;
    }

    return err_flag;
}


/**
 * LEVEL 0 - NPA Framework Test for successful execution.
 */
/** TODO */

 /**
 * LEVEL 1 - NPA Framework Test for successful execution.
 */
pm_err_flag_type pm_test_npa_level_1 (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    g_pm_test_status[g_f_cnt].lvl = PM_TEST_NPA_LVL_1;

    /* NPA Check */
    err_flag = pm_test_npa_chk_ldo_level_1();

    err_flag = pm_test_npa_chk_smps_level_1();

    return err_flag;
}

pm_err_flag_type pm_test_npa_framework (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    kvp = kvp_create(0);

    err_flag = pm_test_npa_level_1();

    return err_flag;
}
