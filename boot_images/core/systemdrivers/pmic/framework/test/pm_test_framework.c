/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                PM TEST FRAMEWORKsss

GENERAL DESCRIPTION
  This file contains Framework Driver test codes.

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
03/23/15   mr      Created.
===========================================================================*/

/*===========================================================================

                INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_test_framework.h"


/*===========================================================================

                GLOBAL VARIABLES DEFINITIONS

===========================================================================*/
pm_test_driver_state g_pm_test_status[PM_TEST_ARRAY_SIZE];
uint8 g_f_cnt = 0;

uint32 g_pm_test_gpio_num;
uint32 g_pm_test_lbc_num;
uint32 g_pm_test_ldo_num;
uint32 g_pm_test_lpg_num;
uint32 g_pm_test_mpp_num;
uint32 g_pm_test_rgb_num;
uint32 g_pm_test_smps_num;


/*===========================================================================

                TEST FUNCTION DEFINITIONS

===========================================================================*/
/* Error logging function */
void pm_test_handle_error(pm_test_apis api, uint8 rcrs, pm_err_flag_type error)
{
    if (g_f_cnt <= PM_TEST_ARRAY_SIZE-1)
    {
        g_pm_test_status[g_f_cnt].api = api;
        g_pm_test_status[g_f_cnt].rcrs_id = rcrs;
        g_pm_test_status[g_f_cnt].err_code = error;
    }
    ++g_f_cnt;

    if (g_f_cnt < PM_TEST_ARRAY_SIZE)
    {
        g_pm_test_status[g_f_cnt].lvl = g_pm_test_status[g_f_cnt-1].lvl;
    }
}

void pm_test_get_periph_count_info(void)
{
    g_pm_test_gpio_num = pm_target_information_get_periph_count_info(PM_PROP_GPIOA_NUM, 0);
    g_pm_test_lbc_num = pm_target_information_get_periph_count_info(PM_PROP_LBCA_NUM, 0);
    g_pm_test_ldo_num = pm_target_information_get_periph_count_info(PM_PROP_LDOA_NUM, 0);
    g_pm_test_lpg_num = pm_target_information_get_periph_count_info(PM_PROP_LPGA_NUM, 0);
    g_pm_test_mpp_num = pm_target_information_get_periph_count_info(PM_PROP_MPPA_NUM, 0);
    g_pm_test_rgb_num = pm_target_information_get_periph_count_info(PM_PROP_RGBA_NUM, 0);
    g_pm_test_smps_num = pm_target_information_get_periph_count_info(PM_PROP_SMPSA_NUM, 0);
}

#if 0    /* Functions not implemented in BOOT Test Framework */
/**
 * LEVEL 0 - Driver APIs' Test for successful execution.
 */
pm_err_flag_type pm_test_driver_level_0 (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    g_pm_test_status[g_f_cnt].lvl = PM_TEST_DRIVER_LVL_0;

    /* GPIO Driver */
    err_flag = pm_test_driver_gpio_level_0();

    return err_flag;
}

/**
 * LEVEL 1 - Driver SET-GET APIs' Tests with working params for successful execution.
 */
pm_err_flag_type pm_test_driver_level_1 (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    g_pm_test_status[g_f_cnt].lvl = PM_TEST_DRIVER_LVL_1;

    /* GPIO Driver */
    err_flag = pm_test_driver_gpio_level_1();

    return err_flag;
}

/**
 * LEVEL 2 - Driver SET-GET APIs' Tests with all possible params combination for successful execution.
 */
pm_err_flag_type pm_test_driver_level_2 (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    g_pm_test_status[g_f_cnt].lvl = PM_TEST_DRIVER_LVL_2;

    /* GPIO Driver */
    err_flag = pm_test_driver_gpio_level_2();

    return err_flag;
}


/**
 * LEVEL 0 - NPA Framework Test for successful execution.
 */
pm_err_flag_type pm_test_npa_level_0 (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    g_pm_test_status[g_f_cnt].lvl = PM_TEST_NPA_LVL_0;

    /* NPA Check */
    err_flag = pm_test_npa_chk_level_0();

    return err_flag;
}
#endif

/**
 * Main function to call all three level test functions.
 * This function is exported for explicit tests.
 */
pm_err_flag_type pm_test_framework (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    memset(g_pm_test_status, 0, sizeof(g_pm_test_status));

    pm_test_get_periph_count_info();

    // err_flag |= pm_test_driver_level_0();
    // err_flag |= pm_test_driver_level_1();
    // err_flag |= pm_test_driver_level_2();

    pm_test_unit_testcases();

    return err_flag;
}
