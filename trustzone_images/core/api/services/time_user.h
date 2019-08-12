#ifndef TIME_USER_H
#define TIME_USER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

             S E C U R E   T I M E   H E A D E R    F I L E

GENERAL DESCRIPTION

  This module provides interfaces to set and get the system, user, and
  local time. 

  The system time may only be set by privileged sources of time. 
  The user and local time may be set by various sources. 

  When the system time is requested, the system time is returned 
  unmodified. The system time is the amount of milliseconds since
  the epoch.

  When the user time is requested, an offset is added to the
  system time to create the user time. This user time offset is calculated
  when the user sets the time.

    user_time = system_time + user_time_offset
    
  When the local time is requested, an offset is added to the user
  time to create the local time. This local time offset is set by 
  the user/system.
    
    local_time = user_time + local_time_offset
  
EXTERNALIZED FUNCTIONS

  time_user_get
    This function gets the current user time in milliseconds since the
    epoch. User time is calculated by adding the user time offset to 
    the system time.

  time_user_set
    This function sets the current user time in seconds since the epoch.
    The required user time offset is calculated based on the current 
    system time, and stored in NV.
    

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright(c) 2005 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*=============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_user.h#1 $ 
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
20/23/09   din     Consolidation of Time. Added  FEATURE_SEC_TIME_V2 
04/03/06   ajn     Featurized based on FEATURE_SEC_TIME
04/28/05   ty      Initial revision

=============================================================================*/

#include "comdef.h"
#include "qw.h"



/*=============================================================================

                        DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

FUNCTION     time_user_get

DESCRIPTION
  This function gets the current user time in milliseconds since the
  epoch. User time is calculated by adding the user time offset to 
  the system time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ret_val is filled with the user time.

=============================================================================*/

void time_user_get
(
  uint64                        *ret_val
);


/*=============================================================================

FUNCTION     time_user_set

DESCRIPTION
  This function sets the current user time in seconds since the epoch.
  The required user time offset is calculated based on the current 
  system time. The new user time offset is not stored to NV; a separate 
  call to time_sec_save is required.

  The user time offset is 31 bits long. Therefore, the maximum difference
  between the actual system time and the user time must be +-68 years.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The user time offset is calculated and stored to NV.

=============================================================================*/

void time_user_set
(
  uint32                        new_user_time,

  /* Calling task's TCB pointer */
  rex_tcb_type                  *task_ptr,

  /* Task signal to wait for when reading NV */
  rex_sigs_type                 task_wait_sig,

  /* Task's wait fn to be called when reading NV */
  void                          (*task_wait_handler)(rex_sigs_type)
);



/*=============================================================================

FUNCTION     time_user_set_offset

DESCRIPTION
  This function sets the current user time offset in milliseconds and saves
  it to NV.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  User time offset is saved to NV
=============================================================================*/

void time_user_set_offset
(
  /* New user time offset (in milliseconds) */
  int64                         new_user_offset,

  /* Calling task's TCB pointer */
  rex_tcb_type                  *task_ptr,

  /* Task signal to wait for when writing NV */
  rex_sigs_type                 task_wait_sig,

  /* Task's wait fn to be called when writing NV */
  void                          (*task_wait_handler)(rex_sigs_type)
);



/*=============================================================================

FUNCTION     time_user_get_offset

DESCRIPTION
  This function gets the current user time offset in milliseconds
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_user_get_offset
(
  /* Output: user time offset (in milliseconds) */
  int64                         *user_offset
);


#endif /* TIME_USER_H */

