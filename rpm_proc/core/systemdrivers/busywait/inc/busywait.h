#ifndef BUSYWAIT_H
#define BUSYWAIT_H
/*===========================================================================

                         Busywait Header File

DESCRIPTION
  This header file contains the public API for the busywait component.

      Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/busywait/inc/busywait.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/10   cmd     Replaced busywait_config() with busywait_config_cpu_freq()
                   and busywait_config_pause_timer_freq().
11/09/09   cmd     Added busywait_test() function.
10/22/09   cmd     Initial version.

===========================================================================*/

#include "com_dtypes.h"     /* Common Definitions and Types */


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*==========================================================================

  FUNCTION      BUSYWAIT

  DESCRIPTION   This function pauses the execution of a thread for a
                specified time.

  PARAMETERS    pause_time_us - Time to pause in microseconds

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void busywait (uint32 pause_time_us);


#endif /* BUSYWAIT_H */

