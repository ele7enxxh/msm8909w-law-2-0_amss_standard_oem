#ifndef PM_TEST_FRAMEWORK_H
#define PM_TEST_FRAMEWORK_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                PM NPA FRAMEWORK TEST HEADER

GENERAL DESCRIPTION
  This file contains initialization functions for NPA

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2010           by Qualcomm Technologies, Inc.  All Rights Reserved.
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
#include "string.h"

#include "kvp.h"
#include "rpmserver.h"

#include "pm_lib.h"
#include "pm_clk_driver.h"
#include "pm_target_information.h"

#include "hw_module_type.h"
#include "pm_rpm_ldo_trans_apply.h"
#include "pm_rpm_smps_trans_apply.h"


/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/
typedef enum
{
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

    /* LDO */
    PM_LDO_SW_ENABLE,
    PM_LDO_SW_ENABLE_STATUS,
    PM_LDO_SW_MODE,
    PM_LDO_SW_MODE_STATUS,
    PM_LDO_VOLT_LEVEL,
    PM_LDO_VOLT_LEVEL_STATUS,
    PM_LDO_VREG_OK_STATUS,

    /* SMPS */
    PM_SMPS_SW_ENABLE,
    PM_SMPS_SW_ENABLE_STATUS,
    PM_SMPS_SW_MODE,
    PM_SMPS_SW_MODE_STATUS,
    PM_SMPS_VOLT_LEVEL,
    PM_SMPS_VOLT_LEVEL_STATUS,
    PM_SMPS_QUIET_MODE,
    PM_SMPS_SWITCHING_FREQ,
    PM_SMPS_SWITCHING_FREQ_STATUS,
    PM_SMPS_VREG_OK_STATUS,

    /* CLK */
    PM_CLK_DRV_STRENGTH,
    PM_CLK_SW_ENABLE,
} pm_test_apis;

#define PM_TEST_ARRAY_SIZE    8

typedef struct
{
    pm_test_level lvl : 8;
    pm_test_apis api  : 8;
    uint32 rcrs_id    : 8;
    uint32 err_code   : 8;
} pm_test_driver_state;


/*===========================================================================

                EXPORTED GLOBAL VARIABLES DEFINITIONS

===========================================================================*/
extern pm_test_driver_state g_pm_test_status[PM_TEST_ARRAY_SIZE];
extern uint8 g_f_cnt;

extern uint32 g_pm_test_ldo_num;
extern uint32 g_pm_test_smps_num;

extern pm_rpm_ldo_rail_info_type ldo_rail_a[];
extern pm_rpm_smps_rail_info_type smps_rail_a[];

/* For TEST/DEBUG purpose  -  Defined in LDO Driver Test file */
extern pm_sw_mode_type g_sw_mode;
extern pm_on_off_type g_sw_enable;
extern pm_volt_level_type g_volt_lvl, g_prev_volt_lvl;
extern boolean g_vreg_ok;


/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/

inline void pm_test_handle_error(uint8 rcrs, pm_err_flag_type error)
{
    if (g_f_cnt <= PM_TEST_ARRAY_SIZE-1)
    {
        g_pm_test_status[g_f_cnt].rcrs_id = rcrs;
        g_pm_test_status[g_f_cnt].err_code = error;
    }
	++g_f_cnt;
		
	if (g_f_cnt < PM_TEST_ARRAY_SIZE)
	{
        g_pm_test_status[g_f_cnt].lvl = g_pm_test_status[g_f_cnt-1].lvl;
        g_pm_test_status[g_f_cnt].api = g_pm_test_status[g_f_cnt-1].api;
	}    
}

pm_err_flag_type pm_test_driver_ldo_level_0 (void);
pm_err_flag_type pm_test_driver_smps_level_0 (void);
pm_err_flag_type pm_test_driver_clk_level_0 (void);

pm_err_flag_type pm_test_driver_ldo_level_1 (void);
pm_err_flag_type pm_test_driver_smps_level_1 (void);

pm_err_flag_type pm_test_npa_chk_ldo_level_1 (void);
pm_err_flag_type pm_test_npa_chk_smps_level_1 (void);

#endif  /* PM_TEST_FRAMEWORK_H */

