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
09/02/14   mr      Created. (CR-713705)
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "string.h"
#include "comdef.h"

#include "pm_lib_err.h"
#include "pm_pon.h"

// #include "npa.h"
// #include "pm_npa.h"
// #include "pmapp_npa.h"


/*===========================================================================

                 LOCAL MACRO AND STRUCTURE DEFINITIONS

===========================================================================*/
typedef enum
{
	/* Driver Test Level */
    PM_TEST_DRIVER_L0_LVL,
    PM_TEST_DRIVER_L1_LVL,
    PM_TEST_DRIVER_L2_LVL,
	
	/* NPA Framework Test Level */
	PM_TEST_NPA_L0_LVL,
    PM_TEST_NPA_L1_LVL,
} pm_test_level;

typedef enum
{
    PM_NO_TEST,
    /* PON */
    PM_PON_PS_HOLD_CFG,
    /* QFUSE CONFIG APIs */	
    PM_ENABLE_QFUSE_POWER_RAIL,
    PM_DISABLE_QFUSE_POWER_RAIL,
} pm_test_apis;

typedef struct
{
    pm_test_level lvl : 4;
    pm_test_apis api : 4;
    uint32 test_result : 24;
} pm_test_driver_state;


/*===========================================================================

                LOCAL AND EXPORTED VARIABLE DEFINITIONS

===========================================================================*/
extern pm_test_driver_state test_status;


/*===========================================================================

                LOCAL AND EXPORTED FUNCTION PROTOTYPE

===========================================================================*/
/** PMIC GPIO Driver Level Test Functions */
pm_err_flag_type pm_test_driver_pon_l0_level (void);
pm_err_flag_type pm_test_driver_pon_l1_level (void);


/** PMIC NPA Level Test Functions */
// pm_err_flag_type pm_test_npa_chk_l0_level (void);

/** Internal QFuse Power Rail Enable API */
pm_err_flag_type pm_enable_qfuse_power_rail(void);
pm_err_flag_type pm_disable_qfuse_power_rail(void);

#endif  /* PM_TEST_FRAMEWORK_H */



