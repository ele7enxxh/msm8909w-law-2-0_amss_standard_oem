#ifndef TIME_ALARM_H
#define TIME_ALARM_H

/*===========================================================================

                     T I M E   A L A R M   S E R V I C E S
  
GENERAL DESCRIPTION 
  This module provides alarm services for other subsystems 
  
  
EXTERNALIZED FUNCTIONS
  time_alarm_set()

  Copyright (c) 2007 - 2009 by Qualcomm Technologies Incorporated. All Rights Reserved.         
===========================================================================*/


                                                                            
/*===========================================================================

                           EDIT HISTORY FOR FILE                             
  This section contains comments describing changes made to the module.      
  Notice that changes are listed in reverse chronological order.             

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_alarm.h#1 $ 
$Author: pwbldsvc $  
$DateTime: 2016/06/17 14:31:11 $  
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/29/09   din     Extended Alarm period by adding time_alarm_set_secs(). 
                   Also added time_alarm_set_ms().
02/23/09   din     Consolidation of Time.
03/07/07   jhs     Style clean up.
02/06/07    MK     Created this file                                         
 
===========================================================================*/



/*===========================================================================
 
                     INCLUDE FILES FOR MODULE
 
===========================================================================*/

#include "target.h"
 


/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items required by this module.

===========================================================================*/



/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/
 
/*=============================================================================

FUNCTION TIME_ALARM_SET

DESCRIPTION
  Set an alarm with the given expiration.  Existing alarm will be canceled.

DEPENDENCIES
  None

RETURN VALUE
  0 - Success
  Other - Failure

SIDE EFFECTS
  None

=============================================================================*/

int32 time_alarm_set
(
  /* Amount of time in milliseconds in which to expire the timer */
  uint32 expiration
);

/*=============================================================================

FUNCTION TIME_ALARM_SET_MS

DESCRIPTION
  Set an alarm with the given expiration.  Existing alarm will be canceled.

DEPENDENCIES
  None

RETURN VALUE
  0 - Success
  Other - Failure

SIDE EFFECTS
  None

=============================================================================*/
int32 time_alarm_set_ms
(
  /* Amount of time in milliseconds in which to expire the timer */
  uint32 expiration
);


/*=============================================================================

FUNCTION TIME_ALARM_SET_SECS

DESCRIPTION
  Set an alarm with the given expiration.  Existing alarm will be canceled.

DEPENDENCIES
  None

RETURN VALUE
  0 - Success
  Other - Failure

SIDE EFFECTS
  None

=============================================================================*/

int32 time_alarm_set_secs
(
  /* Amount of time in seconds in which to expire the timer */
  uint32 expiration
);

#endif /* #ifndef TIME_ALARM_H */

