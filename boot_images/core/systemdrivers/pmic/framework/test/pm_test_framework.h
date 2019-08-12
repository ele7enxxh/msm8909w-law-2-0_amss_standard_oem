#ifndef PM_TEST_FRAMEWORK_H
#define PM_TEST_FRAMEWORK_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                PM TEST FRAMEWORK

GENERAL DESCRIPTION
  PMIC Framework Test header file.

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
#include "string.h"

#include "pm_err_flags.h"
#include "pm_target_information.h"


/*===========================================================================

                 LOCAL MACRO AND STRUCTURE DEFINITIONS

===========================================================================*/
typedef enum
{
    PM_TEST_LVL_NONE,

    /* Driver Test Level */
    PM_TEST_DRIVER_LVL_0,
    PM_TEST_DRIVER_LVL_1,
    PM_TEST_DRIVER_LVL_2,

    /* NPA Framework Test Level */
    PM_TEST_NPA_LVL_0,
    PM_TEST_NPA_LVL_1,
} pm_test_level;

typedef enum
{
    PM_NO_TEST,
    /* MODULE DRIVERs' API */
    /* NPA TEST CALLs */
} pm_test_apis;

#define PM_TEST_ARRAY_SIZE    16

typedef struct
{
    pm_test_level lvl : 8;
    pm_test_apis api  : 8;
    uint32 rcrs_id    : 8;
    pm_err_flag_type err_code   : 8;
} pm_test_driver_state;


/*===========================================================================

                EXPORTED GLOBAL VARIABLES DEFINITIONS

===========================================================================*/
extern pm_test_driver_state g_pm_test_status[PM_TEST_ARRAY_SIZE];
extern uint8 g_f_cnt;

extern uint32 g_pm_test_gpio_num;
extern uint32 g_pm_test_ldo_num;
extern uint32 g_pm_test_lpg_num;
extern uint32 g_pm_test_mpp_num;
extern uint32 g_pm_test_rgb_num;
extern uint32 g_pm_test_smps_num;

/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/
void pm_test_handle_error(pm_test_apis api, uint8 rcrs, pm_err_flag_type error);

/** PMIC Driver Level Test Functions */
// pm_err_flag_type pm_test_driver_gpio_level_0 (void);
// pm_err_flag_type pm_test_driver_gpio_level_1 (void);
// pm_err_flag_type pm_test_driver_gpio_level_2 (void);

void pm_test_unit_testcases (void);

#endif  /* PM_TEST_FRAMEWORK_H */

