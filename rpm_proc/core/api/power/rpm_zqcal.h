#ifndef RPM_ZQCAL_H
#define RPM_ZQCAL_H

/*
===========================================================================

FILE:         rpm_zqcal.h

DESCRIPTION:


===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/api/power/rpm_zqcal.h#1 $
$Date: 2015/02/25 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2013 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <stdint.h>

/* -----------------------------------------------------------------------
**                           FUNCTION DECLARATIONS
** ----------------------------------------------------------------------- */

void zqcal_task_register_cb(void *cb);
void zqcal_task_deregister_cb(void);

#endif /* RPM_ZQCAL_H */

