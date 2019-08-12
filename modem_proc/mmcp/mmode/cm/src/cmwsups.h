#ifndef CMWSUPS_H
#define CMWSUPS_H

/*===========================================================================

       C A L L   M A N A G E R

              S U P P L E M E N T A R Y   S E R V I C E S

                     H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface with
  the Call Manager Sups module (CMWSUPS.C)

Copyright (c) 2001 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmwsups.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/11   rm      Updating correct sups params with FEATURE_MODEM_HEAP_PHASE_2
12/23/10   sv      Integrate DSDS feature
11/15/10   rm      Memory optimization changes
05/26/09   mp      Updated SUPS event notification in SUSPEND state
11/08/05   sk      Separated CM reports and commands.
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
02/06/04   ws      Initial jaguar/mainline merge.
04/11/03   prk     Removed references to FEATURE_SUPS.
01/21/03   ws      Updated copyright information for 2003
05/07/02   PRK     Renamed to cmwsups.h.
09/25/01   RI      Initial release.

===========================================================================*/
#include "mmcp_variation.h"
#include "cmi.h"

#ifdef FEATURE_IP_CALL
/*===========================================================================
FUNCTION cmipsups_is_ussd_sups_cmd

DESCRIPTION
to check if the command is to be send on USSD interface. 

RETURN VALUE
boolean
===========================================================================*/
boolean cmipsups_is_ussd_sups_cmd( cm_sups_cmd_e_type cmd );
#endif

#ifdef CM_GW_SUPPORTED


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION cmsups_ptr

DESCRIPTION
  Return a pointer to the one and only sups object.

  CM sups object.
  The sups object is responsible for:
  1. Processing clients' sups commands.
  2. Processing MN replies for clients' sups commands.
  3. Processing MN sups related notifications.
  4. Notifying the client list of sups events.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cmsups_s_type  *cmsups_ptr( void );




/*===========================================================================

FUNCTION cmsups_init

DESCRIPTION
  Initializing the sups object.

  This function must be called before the sups object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsups_init( void );


/*===========================================================================

FUNCTION cmsups_nv_refresh

DESCRIPTION
  Reading NV items 


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsups_nv_refresh(

   uint8             refresh_subs_mask 
    /* Subs mask whose NVs need to be refreshed
    */
 
);
/*===========================================================================

FUNCTION cmsups_client_cmd_proc

DESCRIPTION
  Process clients' sups commands.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsups_client_cmd_proc(

    cm_sups_cmd_s_type  *sups_cmd_ptr
        /* pointer to a CM command */
);



/*===========================================================================

FUNCTION cmsups_rpt_proc

DESCRIPTION
  Process Lower Layer sups notifications / sups command replies.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsups_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
);

/*===========================================================================
FUNCTION cmsups_dem_is_apps_wakeup_from_suspend_event

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
boolean  cmsups_dem_is_apps_wakeup_from_suspend_event(

    cm_sups_event_e_type   sups_event
        /* SUPS event */
);

/*===========================================================================

FUNCTION CMWSUPS_MEM_FREE_SUPS_INFO

DESCRIPTION
  De allocates the memory held by sups_info_ptr

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwsups_mem_free_sups_info(

  cmsups_s_type   *sups_ptr
    /* Pointer to sups obj to be freed */
);
/*===========================================================================

FUNCTION cmsups_call_control_complete_cb

DESCRIPTION
  Callback for Number Classification to call when call control is complete.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsups_call_control_complete_cb(

  cm_call_id_type                         call_id,
    /* call id */

  cm_cc_status_e_type                     status,
    /* status of command */

  cm_call_type_e_type                     call_type,
    /* call type */

  const cm_orig_call_control_s_type      *cdma_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on cdma */

  const cm_orig_call_control_s_type      *gw_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on gw */

  const void                                   *user_data
    /* data passed from original call */

);

/*===========================================================================

FUNCTION cmwsups_timer_proc

DESCRIPTION
  Process timer events

DEPENDENCIES
  Sups object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwsups_timer_proc(

  cm_timer_event_type    timer_event
    /* Indicate specific timer event.
    **
    ** Note that currently timer event is not being used
    */
);

/*===========================================================================

FUNCTION cmwsups_sglte_determine_ss_for_sups_cmd

DESCRIPTION
  Process timer events

DEPENDENCIES
  Sups object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_sups_cmd_err_e_type cmwsups_sglte_determine_ss_for_sups_cmd(

   sys_modem_as_id_e_type     asubs_id,
       /* Command Subs ID */

   cm_sups_cmd_e_type         sups_cmd,
       /* Sups Command */

   cm_ss_e_type              *domain_ss
       /* SS to originate the Sups Command */
);

extern void cmsups_create_interrogate_cnf_internal(cm_sups_cmd_info_s_type *sups_cmd_info_ptr);
extern void cmsups_create_activate_cnf_internal(cm_sups_cmd_info_s_type *sups_cmd_info_ptr);
extern void cmsups_create_deactivate_cnf_internal(cm_sups_cmd_info_s_type *sups_cmd_info_ptr);

#endif /* FEATURE_WCDMA || FEATURE_GSM */

#endif /* CMWSUPS_H */
