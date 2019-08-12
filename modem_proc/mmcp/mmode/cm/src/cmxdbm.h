
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   P D   M A N A G E R

GENERAL DESCRIPTION
  This module contains declarations used by the call manager in processing
  Data Burst messages.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmdbm_init() must be called to initialize this module before any other
  function declared in this module is being called.

Copyright (c) 2002 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmxdbm.h#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
06/14/07    pk/cl   Added support for power suspend
11/08/05    sk      Separated CM reports and commands.
02/06/04    ws      Initial jaguar/mainline merge.
09/18/03    vt      Added functions to update DDTM status.
11/14/02    vt      Added cmdbm_sd_rpt_proc definition.
07/07/02    az      Created.

===========================================================================*/

#include "cmi.h"

        
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                  GLOBALS                                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*  This defines the size of the DBM queue. 
 *  The number of outstanding DBM's buffered by CM.
*/        
#define CM_DBM_CMD_HOLD_QUEUE_SIZE 3


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                               FUNCTION DECLARATIONS                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

FUNCTION cmdbm_init

DESCRIPTION
  Initializing the DBM object.

  This function must be called before the DBM object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbm_init( void );


/*===========================================================================

FUNCTION cmxdbm_is_mo_dbm_in_progress

DESCRIPTION
 Query the mo dbm in progress status

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cmxdbm_is_mo_dbm_in_progress ( void );


/*===========================================================================

FUNCTION cmdbm_client_cmd_proc

DESCRIPTION
  Processes DBM commands found on the CM command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  May force the mode of the phone if necessary.

===========================================================================*/

void cmdbm_client_cmd_proc(
      cm_dbm_cmd_s_type   *dbm_cmd_ptr
);


/*===========================================================================

FUNCTION cmdbm_mc_rpt_proc

DESCRIPTION
  Process MC reports specific to Data Burst messages.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbm_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);



/*===========================================================================

FUNCTION cmdbm_sd_rpt_proc

DESCRIPTION
  Process System determination reports specific to DBM.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbm_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
);


/*===========================================================================

FUNCTION cmdbm_timer_proc

DESCRIPTION

  This function monitors the value of wait time associated with each 
  stored DBM. When the wait time expires, a timeout event is reported
  back to the registered clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmdbm_timer_proc(
    cm_timer_event_type    timer_event
        /* Indicate specific timer event.
        **
        ** Note that currently timer event is not being used */
        /*lint -esym(715,timer_event) */
);

/*===========================================================================
FUNCTION cmxdbm_dem_is_apps_wakeup_from_suspend_event

DESCRIPTION
  Determines if the event should wakeup the processor from the suspend state

DEPENDENCIES
  None.

RETURN VALUE
  True : If the apps processor needs to be woken up
  False: Otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern boolean  cmxdbm_dem_is_apps_wakeup_from_suspend_event(

    cm_dbm_event_e_type   dbm_event
        /* Call event */
);

