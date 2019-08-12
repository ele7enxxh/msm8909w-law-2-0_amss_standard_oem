/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             PM SMPS DRIVER TEST

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
pm_smps_switching_freq_type g_switch_freq, g_prev_switch_freq;

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/
/**
 * @name pm_test_driver_smps_level_0
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
pm_err_flag_type pm_test_driver_smps_level_0 (void)
{
    uint8 i = 0;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    g_pm_test_status[g_f_cnt].api = PM_SMPS_SW_ENABLE;
    for(i = 0; i < g_pm_test_smps_num; i++)
    {
        err_flag = pm_smps_sw_enable(0, i, PM_ON);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
    }

    g_pm_test_status[g_f_cnt].api = PM_SMPS_SW_ENABLE_STATUS;
    for(i = 0; i < g_pm_test_smps_num; i++)
    {
        err_flag = pm_smps_sw_enable_status(0, i, &g_sw_enable);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
    }

    g_pm_test_status[g_f_cnt].api = PM_SMPS_SW_MODE;
    for(i = 0; i < g_pm_test_smps_num; i++)
    {
        err_flag = pm_smps_sw_mode(0, i, PM_SW_MODE_NPM);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
    }

    g_pm_test_status[g_f_cnt].api = PM_SMPS_SW_MODE_STATUS;
    for(i = 0; i < g_pm_test_smps_num; i++)
    {
        err_flag = pm_smps_sw_mode_status(0, i, &g_sw_mode);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
    }

    g_pm_test_status[g_f_cnt].api = PM_SMPS_VOLT_LEVEL;
    for(i = 0, g_volt_lvl = 0; i < g_pm_test_smps_num; i++, g_volt_lvl = 0)
    {
        err_flag = pm_smps_volt_level_status(0, i, &g_volt_lvl);
        err_flag = pm_smps_volt_level(0, i, g_volt_lvl);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
    }

    g_pm_test_status[g_f_cnt].api = PM_SMPS_VOLT_LEVEL_STATUS;
    for(i = 0, g_volt_lvl = 0; i < g_pm_test_smps_num; i++, g_volt_lvl = 0)
    {
        err_flag = pm_smps_volt_level_status(0, i, &g_volt_lvl);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
    }

    g_pm_test_status[g_f_cnt].api = PM_SMPS_SWITCHING_FREQ;
    for(i = 0; i < g_pm_test_smps_num; i++)
    {
        err_flag = pm_smps_switching_freq_status(0, i, &g_switch_freq);
        err_flag = pm_smps_switching_freq(0, i, g_switch_freq);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
    }

    g_pm_test_status[g_f_cnt].api = PM_SMPS_SWITCHING_FREQ_STATUS;
    for(i = 0; i < g_pm_test_smps_num; i++, g_switch_freq = PM_SWITCHING_FREQ_FREQ_NONE)
    {
        err_flag = pm_smps_switching_freq_status(0, i, &g_switch_freq);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
    }

    g_pm_test_status[g_f_cnt].api = PM_SMPS_QUIET_MODE;
    for(i = 0; i < g_pm_test_smps_num; i++)
    {
        err_flag = pm_smps_quiet_mode(0, i, PM_QUIET_MODE__QUIET, 500);
        if(PM_ERR_FLAG__SUCCESS != err_flag && PM_ERR_FLAG__FEATURE_NOT_SUPPORTED != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
    }

    g_pm_test_status[g_f_cnt].api = PM_SMPS_VREG_OK_STATUS;
    for(i = 0, g_vreg_ok = PM_OFF; i < g_pm_test_smps_num; i++, g_vreg_ok = PM_OFF)
    {
        err_flag = pm_smps_vreg_ok_status(0, i, &g_vreg_ok);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
		{
            pm_test_handle_error(i, err_flag);
		}
    }

    return err_flag;
}

/**
 * @name pm_test_driver_smps_level_1
 *
 * @brief This function tests for SET-GET APIs' successful return.
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
pm_err_flag_type pm_test_driver_smps_level_1 (void)
{
    uint8 i = 0, do_set = 0;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    g_pm_test_status[g_f_cnt].api = PM_SMPS_SW_ENABLE;
	g_sw_enable = PM_OFF;
    for(i = 0; i < g_pm_test_smps_num; i++)
    {
        err_flag = pm_smps_sw_enable_status(0, i, &g_sw_enable);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
        else
        {
            if (PM_OFF == g_sw_enable)
            {
                do_set = 1;
            }
            err_flag = pm_smps_sw_enable(0, i, PM_ON);
            if(PM_ERR_FLAG__SUCCESS != err_flag)
            {
                pm_test_handle_error(i, err_flag);
            }
            else
            {
                g_sw_enable = PM_OFF;
                err_flag = pm_smps_sw_enable_status(0, i, &g_sw_enable);
                if(PM_ERR_FLAG__SUCCESS != err_flag || PM_ON != g_sw_enable)
                {
                    pm_test_handle_error(i, err_flag);
                }

                if (1 == do_set)
                {
                    err_flag = pm_smps_sw_enable(0, i, PM_OFF);
                    if(PM_ERR_FLAG__SUCCESS != err_flag)
                    {
                        pm_test_handle_error(i, err_flag);
                    }
                }
            }
        }
		g_sw_enable = PM_OFF; do_set = 0;
    }

    g_pm_test_status[g_f_cnt].api = PM_SMPS_SW_MODE;
	g_sw_mode = PM_SW_MODE_LPM;
    for(i = 0; i < g_pm_test_smps_num; i++)
    {
        err_flag = pm_smps_sw_mode_status(0, i, &g_sw_mode);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
        else
        {
            if (PM_SW_MODE_LPM == g_sw_mode)
            {
                do_set = 1;
            }
            err_flag = pm_smps_sw_mode(0, i, PM_SW_MODE_NPM);
            if(PM_ERR_FLAG__SUCCESS != err_flag)
            {
                pm_test_handle_error(i, err_flag);
            }
            else
            {
                g_sw_mode = PM_SW_MODE_LPM;
                err_flag = pm_smps_sw_mode_status(0, i, &g_sw_mode);
                if(PM_ERR_FLAG__SUCCESS != err_flag || PM_SW_MODE_NPM != g_sw_mode)
                {
                    pm_test_handle_error(i, err_flag);
                }

                if (1 == do_set)
                {
                    err_flag = pm_smps_sw_mode(0, i, PM_SW_MODE_LPM);
                    if(PM_ERR_FLAG__SUCCESS != err_flag)
                    {
                        pm_test_handle_error(i, err_flag);
                    }
                }
            }
        }
		g_sw_mode = PM_SW_MODE_LPM; do_set = 0;
    }

    g_pm_test_status[g_f_cnt].api = PM_SMPS_VOLT_LEVEL;
	g_volt_lvl = 0; g_prev_volt_lvl = 0;
    for(i = 0; i < g_pm_test_smps_num; i++)
    {
        err_flag = pm_smps_volt_level_status(0, i, &g_volt_lvl);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
        else
        {
            g_prev_volt_lvl = g_volt_lvl;
            g_volt_lvl += 25000;
            err_flag = pm_smps_volt_level(0, i, g_volt_lvl);
            if(PM_ERR_FLAG__SUCCESS != err_flag)
            {
                pm_test_handle_error(i, err_flag);
            }
            else
            {
                g_volt_lvl = 0;
                err_flag = pm_smps_volt_level_status(0, i, &g_volt_lvl);
                if(PM_ERR_FLAG__SUCCESS != err_flag ||
                   ((g_prev_volt_lvl + 25000) != g_volt_lvl))
                {
                    pm_test_handle_error(i, err_flag);
                }

                err_flag = pm_smps_volt_level(0, i, g_prev_volt_lvl);
                if(PM_ERR_FLAG__SUCCESS != err_flag)
                {
                    pm_test_handle_error(i, err_flag);
                }
            }
        }
		g_volt_lvl = 0; g_prev_volt_lvl = 0;
    }

    g_pm_test_status[g_f_cnt].api = PM_SMPS_SWITCHING_FREQ;
    for(i = 0; i < g_pm_test_smps_num; i++)
    {
        err_flag = pm_smps_switching_freq_status(0, i, &g_switch_freq);
        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            pm_test_handle_error(i, err_flag);
        }
        else
        {
            g_prev_switch_freq = g_switch_freq;
            g_switch_freq += (pm_smps_switching_freq_type)1;
            err_flag = pm_smps_switching_freq(0, i, g_switch_freq);
            if(PM_ERR_FLAG__SUCCESS != err_flag)
            {
                pm_test_handle_error(i, err_flag);
            }
            else
            {
                g_switch_freq = PM_SWITCHING_FREQ_FREQ_NONE;
                err_flag = pm_smps_switching_freq_status(0, i, &g_switch_freq);
                if(PM_ERR_FLAG__SUCCESS != err_flag ||
                   ((g_prev_switch_freq + (pm_smps_switching_freq_type)1) != g_switch_freq))
                {
                    pm_test_handle_error(i, err_flag);
                }

                err_flag = pm_smps_switching_freq(0, i, g_prev_switch_freq);
                if(PM_ERR_FLAG__SUCCESS != err_flag)
                {
                    pm_test_handle_error(i, err_flag);
                }
            }
        }
		g_switch_freq = PM_SWITCHING_FREQ_FREQ_NONE;
		g_prev_switch_freq = PM_SWITCHING_FREQ_FREQ_NONE;
    }

    return err_flag;
}

