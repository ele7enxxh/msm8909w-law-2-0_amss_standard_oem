/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             PM FRAMEWORK TEST

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

                VARIABLES DEFINITIONS

===========================================================================*/
pm_test_driver_state g_pm_test_status[PM_TEST_ARRAY_SIZE];
uint8 g_f_cnt = 0;

uint32 g_pm_test_ldo_num;
uint32 g_pm_test_smps_num;


/*===========================================================================

                LOCAL FUNCTION DEFINITIONS

===========================================================================*/
/**
 * LEVEL 0 - Driver APIs' Test for successful execution.
 */
pm_err_flag_type pm_test_driver_level_0 (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    g_pm_test_status[g_f_cnt].lvl = PM_TEST_DRIVER_LVL_0;

    err_flag = pm_test_driver_ldo_level_0();

    err_flag = pm_test_driver_smps_level_0();

    err_flag = pm_test_driver_clk_level_0();

    return err_flag;
}

/**
 * LEVEL 1 - Driver SET-GET APIs' Tests with working params for successful execution.
 */
pm_err_flag_type pm_test_driver_level_1 (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    g_pm_test_status[g_f_cnt].lvl = PM_TEST_DRIVER_LVL_1;

    err_flag = pm_test_driver_ldo_level_1();

    err_flag = pm_test_driver_smps_level_1();

    return err_flag;
}

/**
 * Main function to call all three level test functions.
 * This function is exported for explicit tests.
 */
pm_err_flag_type pm_test_framework (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    memset(g_pm_test_status, 0, sizeof(g_pm_test_status));

    g_pm_test_ldo_num = pm_target_information_get_periph_count_info(PM_PROP_LDO_NUM, 0);
    g_pm_test_smps_num = pm_target_information_get_periph_count_info(PM_PROP_SMPS_NUM, 0);

    if(0 == g_pm_test_ldo_num || 0 == g_pm_test_smps_num)
    {
        return PM_ERR_FLAG__INVALID;
    }

    err_flag = pm_test_driver_level_0();

    err_flag = pm_test_driver_level_1();

    // err_flag = pm_test_driver_level_2();

    return err_flag;
}

