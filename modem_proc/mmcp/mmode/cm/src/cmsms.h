#ifndef CMSMS_H
#define CMSMS_H
/*===========================================================================

         C A L L   M A N A G E R   S M S   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMSMS.c

Copyright (c) 1999 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmsms.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/12   jqi     Free MO SMS data buffer correctly.
01/06/12   gm      Data synchronization changes for freefloating platform
03/07/07   jqi     Initial release.


===========================================================================*/


#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*SMS type - holds all the information that is associated
** with a SMS object
*/

typedef struct cmsms_s  {

    cm_init_mask            init_mask;          /* Indicate whether this object was
                                                   properly initialized */

    #ifdef FEATURE_CDSMS

    cm_sms_state_e_type     sms_state;          /* SMS state for MO SMS */

    dword                   max_hold_orig_time; /* maximum allowed time for MO SMS hold
                                                   orig retry */

    dword                   hold_orig_time_s102; /* maximum allowed time for MO SMS hold
                                                   orig retry */

    dword                   max_wait_for_status_time; /* maximum allowed time to wait for
                                                         MO SMS status */

    cm_client_s_type        *sms_client_ptr;    /* SMS client - i.e. requesting client
                                                   for sending MO SMS msg or AWI ACK */

    cm_sms_awi_ack_s_type   awi_ack;            /* field for awi ack */

    boolean                 l2_ack_requested;   /* l2 ack required or not - used by MO SMS*/

    boolean                 bc_enable;

    boolean                 is_ok_to_send;      /* Indicate whether it is OK
                                                   to send MO SMS */
    #endif /* FEATURE_CDSMS */
    
    cm_sms_msg_type         *data;              /* pointer to MO SMS buffer */

    cm_mt_sms_msg_type      *mt_sms_ptr;        /* Pointer to MT SMS buffer */

    cm_sms_mo_status_e_type mo_status;          /* status for MO SMS */

} cmsms_s_type;

/*===========================================================================

FUNCTION cmsms_ptr

DESCRIPTION
  Return a pointer to the one and only SMS object.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmsms_s_type  *cmsms_ptr( void );

/*===========================================================================

FUNCTION cmsms_init

DESCRIPTION
  Initializing the SMS object.

  This function must be called before the SMS object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsms_init( void );


/*===========================================================================

FUNCTION cmsms_client_cmd_proc

DESCRIPTION
  Process clients' SMS commands.

DEPENDENCIES
  Phone object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsms_client_cmd_proc(

    cm_sms_cmd_s_type  *sms_cmd_ptr
        /* pointer to a CM command */
);

/*===========================================================================

FUNCTION cmsms_sms_data_buffer_allocate

DESCRIPTION
 Returns a pointer to the static global location to store outgoing SMS msgs.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to cm_sms_msg_type.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern cm_sms_msg_type *cmsms_sms_data_buffer_allocate( void );

/*===========================================================================

FUNCTION cmxsms_sms_data_buffer_fill

DESCRIPTION
 Fill in the static global location for outgoing SMS msgs.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmsms_sms_data_buffer_fill
(
  cm_sms_msg_type* dest_sms_msg,

  const cm_sms_msg_type*    sms_msg
);


/*===========================================================================

FUNCTION cmsms_event

DESCRIPTION
  Notify the client list of a specified sms event.

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsms_event(

    const cmsms_s_type     *sms_ptr,
        /* pointer to a sms object.  */

    cm_sms_event_e_type    sms_event
        /* notify client list of this sms event */
);

/*===========================================================================
FUNCTION cmsms_dem_is_apps_wakeup_from_suspend_event

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
extern boolean  cmsms_dem_is_apps_wakeup_from_suspend_event(

    cm_sms_event_e_type   sms_event
        /* Call event */
);


#endif /* CMSMS_H */

