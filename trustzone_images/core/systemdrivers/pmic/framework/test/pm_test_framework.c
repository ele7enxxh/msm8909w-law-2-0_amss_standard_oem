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
09/02/14   mr      Created. (CR-713705)
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_test_framework.h"


/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/
pm_test_driver_state test_status;


/*===========================================================================

                LOCAL FUNCTION DEFINITIONS

===========================================================================*/
/**
 * LEVEL 0 - Driver APIs' Test for successful execution.
 */
pm_err_flag_type pm_test_driver_l0_level (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    test_status.lvl = PM_TEST_DRIVER_L0_LVL;

    /* PON Driver */
    err_flag = pm_test_driver_pon_l0_level();

    return err_flag;
}

/**
 * LEVEL 1 - Driver SET-GET APIs' Tests with working params for successful execution.
 */
pm_err_flag_type pm_test_driver_l1_level (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    test_status.lvl = PM_TEST_DRIVER_L1_LVL;

    /* PON Driver */
    err_flag = pm_test_driver_pon_l1_level();

    return err_flag;
}

/**
 * LEVEL 0 - NPA Framework Test for successful execution. - TODO
 */
// pm_err_flag_type pm_test_npa_l0_level (void)
// {
    // pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    // test_status.lvl = PM_TEST_NPA_L0_LVL;

    // /* NPA Check */
    // err_flag = pm_test_npa_chk_l0_level();

    // return err_flag;
// }

/**
 * Main function to call level test functions.
 * This function is exported for explicit tests.
 */
pm_err_flag_type pm_test_framework (void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    memset(&test_status, 0, sizeof(test_status));

    err_flag = pm_test_driver_l0_level();
    if (0 != test_status.test_result)  return err_flag;

#if 0
    err_flag = pm_test_driver_l1_level();
    if (0 != test_status.test_result)  return err_flag;
	
    err_flag = pm_test_npa_l0_level();
#endif

    return err_flag;
}
