#ifndef TIME_SEC_H
#define TIME_SEC_H

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

  time_sec_init
    This function initializes the sectime functionality. 
    
  time_sec_save
    This function saves user time offset and local time offset to NV.
    
  time_sec_get
    This function gets the current system time in milliseconds since the
    epoch. 

  time_sec_set_time
    This function sets the current system time in seconds since the epoch.
    The time is stored in the RTC.
    

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright(c) 2005 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*=============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_sec.h#1 $ 
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/05/09   din     Consolidation of Time sources. Added FEATURE_SEC_TIME_V2
04/03/06   ajn     Featurized based on FEATURE_SEC_TIME
04/28/05   ty      Initial revision

=============================================================================*/

#include "comdef.h"
#include "qw.h"


/*=============================================================================

                        DATA DECLARATIONS

=============================================================================*/

/* Multiplier to convert milliseconds to time in time stamp format */
#define MSECS_TO_TIME_STAMP_CONV_FACTOR   8
#define SEC_TO_MS_CONV_FACTOR             1000



/*=============================================================================

FUNCTION    time_sec_init

DESCRIPTION
  This function initializes the sectime functionality.   

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  Local time offset and user time offset are loaded from NV.
  
=============================================================================*/

extern void time_sec_init
(
  /* Calling task's TCB pointer */
  rex_tcb_type                  *task_ptr,

  /* Task signal to wait for when reading NV */
  rex_sigs_type                 wait_sig,

  /* Task's wait fn to be called when reading NV */
  void                          (*wait_handler)(rex_sigs_type)
);



/*=============================================================================

FUNCTION      time_sec_get

DESCRIPTION
  This function gets the current system time in milliseconds since the
  epoch. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ret_val is filled with the system time.

=============================================================================*/

extern void time_sec_get
(
  uint64                        *ret_val
);



/*=============================================================================

FUNCTION     time_sec_set

DESCRIPTION
  This function sets the current system time in seconds since the epoch.
  The time is stored in the RTC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

extern void time_sec_set
(
  /* New system time (in seconds) */
  uint32                        new_sys_time,

  /* Calling task's TCB pointer */
  rex_tcb_type                  *task_ptr,

  /* Task signal to wait for when reading NV */
  rex_sigs_type                 wait_sig,

  /* Task's wait fn to be called when reading NV */
  void                          (*wait_handler)(rex_sigs_type)
);



/*=============================================================================

FUNCTION     time_sec_save

DESCRIPTION
  This function saves user time offset and local time offset to NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_sec_save
(
  /* Calling task's TCB pointer */
  rex_tcb_type                  *task_ptr,

  /* Task signal to wait for when writing NV */
  rex_sigs_type                 wait_sig,

  /* Task's wait fn to be called when writing NV */
  void                          (*wait_handler)(rex_sigs_type)
);

#endif /* TIME_SEC_H */

