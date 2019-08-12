#ifndef PM_SLEEP__H
#define PM_SLEEP__H

/** @file pm_sleep.h
*
*  This header file contains functions and variable declarations
*         to support RPM sleep.
*/
/*
*  Copyright(c) 2014 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_sleep.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/14   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
5/20/14   vtw      Created.
========================================================================== */
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINE AND ENUMS

===========================================================================*/

/** @addtogroup pm_sleep
@{ */

 /**
 *  Sleep mode.
 */
typedef enum
{
  PM_SLEEP_MODE_XO_SHUTDOWN,  /**< XO shutdown. */
  PM_SLEEP_MODE_VDD_MIN,      /**< VDD minimum value. */
  PM_SLEEP_MODE_INVALID
} pm_sleep_mode_type;

/*===========================================================================

                        API

===========================================================================*/

/**
 * Calls the driver layer the PMIC calls to set
 * the rails to the state prior to entering sleep.
 *
 * @param[in] sleep_mode Sleep mode. See #pm_sleep_mode_type.
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 *
 * @sideeffects
 *  Overwrites any client configuration requests.
 *  Any cystom additions to the function should be added with caution.
 */
pm_err_flag_type
pm_rpm_enter_sleep (pm_sleep_mode_type sleep_mode);

/**
 * Calls the driver layer the PMIC calls to set
 * the rails back to the state they were in prior to entering sleep.
 *
 * @param[in] sleep_mode Sleep mode. See #pm_sleep_mode_type.
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 *
 * @sideeffects
 *  Restores any client configuration requests.
 *  Any custom additions to the function should be added with caution.
 */
pm_err_flag_type
pm_rpm_exit_sleep (pm_sleep_mode_type sleep_mode);

/** @} */ /* end_addtogroup pm_sleep */

#endif /* PM_SLEEP__H */
