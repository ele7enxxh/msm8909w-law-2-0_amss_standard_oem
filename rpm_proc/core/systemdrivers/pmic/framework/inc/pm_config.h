#ifndef PM_CONFIG__H
#define PM_CONFIG__H

/*! \file pm_config.h
*  \n
*  \brief Contains enums and macro definitions used in config data.
*  \n
*  \n &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/framework/inc/pm_config.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/14   kt      Created
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/* Flags used for readibility in config data */

#define PM_NONE             0
#define PM_ALWAYS_ON        1

#define PM_DROOP_DETECT_DIS        0
#define PM_DROOP_DETECT_EN         1

#define PM_SETTLING_ERR_DIS     0
#define PM_SETTLING_ERR_EN      1

#define PM_SETTLING_DIS     0
#define PM_SETTLING_EN      1 

/*! \enum pm_access_allowed_type
 *  \brief PMIC RPM resource AccessAllowed flag type used for
 *         readibility in config data.
 */
typedef enum
{
    PM_ACCESS_DISALLOWED,
    PM_ACCESS_ALLOWED,
    PM_ACCESS_INVALID
}pm_access_allowed_type;

#endif // PM_CONFIG__H


