#ifndef HDRSMPKA_H
#define HDRSMPKA_H

/*===========================================================================

            S E S SI O N   M A N A G E M E N T   P R O T O C O L
                      K E E P   A L I V E   M O D U L E
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Session Management Protocol (SMP)
    Keep Alive module

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrsmpka.h_v   1.6   18 Apr 2002 16:01:22   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrsmpka.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/11   cnx     Added hdrsmpka_get_current_ka_interval().
03/27/07   wsh     Added support for new prior session handling
04/15/02   mpa     Modified powerup_init and initialize_timer.
03/27/02   mpa     Added hdrsmpka_save_to_nv.
01/17/02   mpa     Added hdrsmpka_initialize_timer, hdrsmpka_powerup_init.
                   Removed hdrsmpka_set_timer_from_nv.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdrhai.h"
#include "dsm.h"
#include "comdef.h"
#include "hdrerrno.h"
#include "hdrind.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Default Keep Alive Time is 54 hours (number of minutes) */
#define HDRSMPKA_DEFAULT_KEEP_ALIVE_EXPIRATION_MINS 0x0CA8UL

/* keep alive value indicating keep alive is disabled */
#define HDRSMPKA_KEEP_ALIVE_DISABLED 0

typedef enum
{
  HDRSMPKA_TIMER_SIGNAL_RXED,
  HDRSMPKA_ACK_TO_KEEP_ALIVE_REQUEST
} hdrsmpka_update_timer_enum_type;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_DEF_TIMER

DESCRIPTION
  This function defines the timers

DEPENDENCIES
  None

PARAMETERS
  tcb_ptr - pointer to the tcb of the task where the keep alive functionality
            is being run
  sig - sig to set upon timer expiration during definition

RETURN VALUE 
  None

SIDE EFFECTS
  Defines a rex_timer

===========================================================================*/

extern void hdrsmpka_def_timer
(
  rex_tcb_type * tcb_ptr,
  rex_sigs_type sig
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_POWERUP_INIT

DESCRIPTION
  This function commands hdrsmpka to initialize.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrsmpka_powerup_init ( void );

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_INITIALIZE_TIMER

DESCRIPTION
  This function checks for keep alive timer expiration and starts the
  sync time valid timer 

DEPENDENCIES
  There is a dependency that this function only be called upon the first 
  acquisition of an HDR system after powerup.

PARAMETERS
  session_is_open   - whether we have a current session open
  expiration_length - length of the time of expiration (in minutes)

RETURN VALUE
  TRUE  - the keep alive timer is running
  FALSE - the keep alive timer is not running

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrsmpka_initialize_timer
(
  boolean session_is_open,
  uint16  expiration_length
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_UPDATE_TIMER

DESCRIPTION
  This function restarts the timer if there is still time

DEPENDENCIES
  None

PARAMETERS
  update_timer_reason - reason why the timer is being updated.

RETURN VALUE
  TRUE  - the keep alive timer is still running
  FALSE - the keep alive timer is not running

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdrsmpka_update_timer 
( 
  hdrsmpka_update_timer_enum_type update_timer_reason
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_SET_TIMER

DESCRIPTION
  This function sets the timer from the beginning

DEPENDENCIES
  None

PARAMETERS
  expiration_length - length of time of the expiration (in minutes)

RETURN VALUE
  None

SIDE EFFECTS
  This function will set the keep alive timer.

===========================================================================*/

extern void hdrsmpka_set_timer
(
  uint16 expiration_length
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_CLR_TIMER

DESCRIPTION
  This function stops the keep alive timer

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrsmpka_clr_timer ( void );

/*===========================================================================

FUNCTION HDRSMPKA_SAVE_TO_NV

DESCRIPTION
  This function writes KA start and sent times to NV

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrsmpka_save_to_nv( void );

/*===========================================================================

FUNCTION HDRSMPKA_GET_KA_EXPIRE_TIME

DESCRIPTION
  This function returns the expiration time of current session

DEPENDENCIES
  There is a should only be called when current session is valid

PARAMETERS
  expiration_time - returns the expiration time of current session

RETURN VALUE
  None

SIDE EFFECTS
  May set the keep alive timer

===========================================================================*/
extern void hdrsmpka_get_ka_expire_time(qword  expiration_time);

/*===========================================================================

FUNCTION HDRSMPKA_GET_TIME_SOURCE

DESCRIPTION
  This function retrieves an accurate current time.

DEPENDENCIES
  None.

PARAMETERS
  current_system_time - address of the qword in which to return the current
                        system time.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsmpka_get_time_source
(
  qword current_system_time
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_SET_EXPIRE_TIME

DESCRIPTION
  This function sets the keep alive timer to expire at the given time

DEPENDENCIES
  This function must be called after AT has acquired system and has valid
  system time.

PARAMETERS
  time_to_expire - the time keep alive timer will expire.

RETURN VALUE
  True - timer is successfully set
  False - time_to_expire has already passed, timer is not modified.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrsmpka_set_expire_time
(
  qword time_to_expire
);

/*===========================================================================

FUNCTION HDRSMPKA_GET_CURRENT_KA_INTERVAL

DESCRIPTION
  This function returns the interval to the time that Keep Alive
  timer expires.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  The interval to the time that Keep Alive timer expires.

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrsmpka_get_current_ka_interval( void );

#endif               /* HDRSMPKA_H */

