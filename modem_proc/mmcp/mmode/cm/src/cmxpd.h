#ifndef CMXPD_H
#define CMXPD_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   P D   M A N A G E R

GENERAL DESCRIPTION
  This module contains functions used by the call manager in processing
  position determination messages.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmsms_init() must be call to initialize this module before any other
  function declared in this module is being called.

Copyright (c) 2000 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmxpd.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/07/06   pk      Added cmpd_process_oprt_mode_change
01/06/06   ic      Lint cleanup  
11/08/05   sk      Separated CM reports and commands.
10/18/05   pk      Added timer support for PD sessions
09/12/05   pk      Added suppport for PD session
06/08/04   ank     Mainlined FEATURE_GPSONE_DBM.
02/06/04   ws      Initial jaguar/mainline merge.
11/20/03   ic      Added support for:
                   CM Commands:
                   - CM_PD_CMD_FIX_START
                   - CM_PD_CMD_FIX_END
                   CM Events:
                   - CM_PD_EVENT_FIX_START
                   - CM_PD_EVENT_FIX_END
                   Log events:
                   - EVENT_CM_BLOCK_HDR_ORIG_DURING_GPS
                   - EVENT_CM_ALLOW_HDR_ORIG_DURING_GPS
02/28/03   vt      Added code to block/unblock HDR data call origination.
01/21/03   ws      Updated copyright information for 2003
04/02/02   hxw     Modified FEATURE_GPSONE_MSBASED.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
02/06/2002 SH      Added hybrid 3.0 support
05/07/02   prk     Renamed to cmxpd.h.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01    VT/IC  Baseline for CCS
07/08/2001  PJ     Added cmpd_timer_proc prototype.
06/22/2000  PJ     Created.

===========================================================================*/

#include "mmcp_variation.h"
#include "cmi.h"
#include "cmcall.h"

#ifdef FEATURE_GPSONE

/* Macro to decide whether a given session type is a supported session type.
*/
#define CMPD_SESSION_TYPE_IS_VALID( session_type ) \
                        ( ((session_type) > CM_PD_SESSION_NONE) && \
                          ((session_type) < CM_PD_SESSION_MAX) )



#define CMPD_SESSION_START_IS_OPRT_MODE_VALID( oprt_mode ) \
            ( ((oprt_mode)== SYS_OPRT_MODE_ONLINE) || \
              ((oprt_mode)== SYS_OPRT_MODE_LPM) || \
              ((oprt_mode)== SYS_OPRT_MODE_FTM) )
            
#define CMPD_HOLD_SESSION_TIME_NONE 0

/* Time until which the session is to be held
*/

#define CMPD_HOLD_SESSION_TIME 1


/*-----------------------------------
  Internal PD session state machine
 *-------------------------------- */
typedef enum cm_pd_session_state_e {
    CMPD_SESSION_STATE_NONE=-1,        /* FOR INTERNAL USE OF CM ONLY! */
    CMPD_SESSION_STATE_INITIATED,      /* Originate a session */
    CMPD_SESSION_STATE_GRANTED,        /* PD session is on */
    CMPD_SESSION_STATE_DENIED,         /* PD session denied */ 
    CMPD_SESSION_STATE_ABORTED         /* PD session is aborted */
} cmpd_session_state_e_type;

typedef struct cmpd_session_s {
  cm_pd_session_type_e_type    session_type;       /* PD session type */
  cmpd_session_state_e_type    session_state;      /* PD session substate */
  cm_pd_session_status_e_type  end_status;         /* PD session end status */
  cm_pd_session_dormant_e_type dormant_status;     /* PD session dormant status */
  boolean                      is_mode_forced;     /* Mode forced to 1X */

  dword                        hold_session_uptime; /* The time to hold the 
                                                    ** session
                                                    */



  #ifdef FEATURE_IS2000_REL_A

  /*---------------------------------
    Following field used for receiving
    Pilot info in BSStatus Request message.
  *------------------------------ */
  cm_pd_bss_rsp_info_s_type    pilot_info;
  /* this contains pilot info received in BSStatusRsp msg*/
    /* HDR data call origination status. TRUE - blocked; FALSE - Unblocked */
  #endif

  #ifdef FEATURE_HDR_HYBRID
   boolean                      gps_fix_status;
  #endif
  
} cmpd_session_s_type;


/* Enumeration of PD objects.
*/
typedef enum {

  CMPD_SESSION_OBJ_NONE = -1, /* FOR INTERNAL USE OF CM ONLY! */

  CMPD_SESSION_OBJ_MAIN,      /* Main session object */
  CMPD_SESSION_OBJ_DMOD,      /* DMOD session object */
  CMPD_SESSION_OBJ_MAX        /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
   , CMPD_SESSION_OBJ_BIG=0x10000000 /* To force enum to 32 bit for windows NT */
  #endif /* FEATURE_RPC */

} cmpd_session_obj_e_type;


/*=========================================================================*/



/*===========================================================================

FUNCTION cmpd_ptr

DESCRIPTION
  Get the pointer to the PD session object

DEPENDENCIES
  pd object must have already been initialized with
  cmpd_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cmpd_session_s_type  *cmpd_ptr( cmpd_session_obj_e_type pd_session_obj );



/*===========================================================================

FUNCTION cmpd_init

DESCRIPTION
  Initializing the PD object. Currently we only need to
  initialize the PD session state.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmpd_init( void );

/*===========================================================================

FUNCTION cmpd_call_start

DESCRIPTION
  This function is called to perform corresponding PD operations when
  a call (Incoming/Outgoing) start.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmpd_call_start( 

  cm_call_type_e_type                call_type,
    /* Call type - VOICE, SMS, E911, etc
    */
  cm_ss_e_type       ss,
    /* SS on which the call is attempted
    */
  sys_sys_mode_e_type sys_mode,
    /* Sys mode of the call
    */
  cm_call_mode_info_e_type mode_info,
    /* Mode information of the call
    */
  cmcall_pd_status_e_type *pd_status_ptr
    /* Flag to indicate that the call should wait for confirmation
    ** for PD session end
    */
);

/*===========================================================================

FUNCTION cmpd_client_cmd_proc

DESCRIPTION
  Processes PD commands found on the CM command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cmpd_client_cmd_proc(
      cm_pd_cmd_s_type  *pd_cmd_ptr
      );


/*===========================================================================

FUNCTION cmpd_mt_msg

DESCRIPTION
  This function is to be called by CM whenever a message is received from MC.
  This function will generate a PD event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void cmpd_mt_msg(uint8* payload,
                 uint32 numBytes);

/*===========================================================================

FUNCTION cmpd_mc_rpt_proc

DESCRIPTION
  Process MC reports specific to postiion determination.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpd_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);


/*===========================================================================

FUNCTION cmpd_sd_rpt_proc

DESCRIPTION
  Process System determination reports specific to postiion determination.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpd_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
);

/*===========================================================================

FUNCTION cmpd_timer_proc

DESCRIPTION

  Does any periodic processing for CMPD. The only current use for this
  function is to determine if the HOLD_ORIG timer has expired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cmpd_timer_proc(
    cm_timer_event_type    timer_event
        /* Indicate specific timer event.
        **
        ** Note that currently timer event is not being used 
        */
        /*lint -esym(715,timer_event) */
);

/*===========================================================================

FUNCTION cmpd_abort_mo_dbm

DESCRIPTION
  Abort mobile originated data burst message if it is pending

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpd_abort_mo_dbm( void);

#ifdef FEATURE_HDR_HYBRID
/*===========================================================================

FUNCTION cmpd_get_fix_status

DESCRIPTION
  This function is called to get status of gpsOne fix.
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmpd_get_fix_status( void);

#endif /* FEATURE_HDR_HYBRID */

/*===========================================================================

FUNCTION cmpd_session_grant_proc

DESCRIPTION
  Process the session start command 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/

extern void cmpd_session_grant_proc( cm_pd_session_type_e_type session_type );

/*===========================================================================

FUNCTION cmpd_is_session_active

DESCRIPTION
  Returns the current state of the PD object
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if the state of the PD object is other than NONE or denied
  FALSE Otherwise

SIDE EFFECTS
  none
===========================================================================*/

extern boolean cmpd_is_session_active( const cmpd_session_s_type  *pd_ptr);


/*===========================================================================

FUNCTION cmpd_session_abort

DESCRIPTION
  Abort mobile session if it is on going

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmpd_session_abort(

  cm_pd_session_status_e_type abort_reason,
    /* Abort reason 
    */
  cm_pd_session_type_e_type   session_type,
    /* Session type
    */ 
  dword          duration
    /* duration in seconds to delay unforcing the mode 
    */
);


/*===========================================================================

FUNCTION cmpd_process_oprt_mode_change

DESCRIPTION
  This function is called to perform corresponding PD operations when
  the phone's operating mode is changed.

DEPENDENCIES
  None

RETURN VALUE
 True if a PD session was ended because of the operating mode change
 False otherwise


SIDE EFFECTS
  none

===========================================================================*/

extern boolean cmpd_process_oprt_mode_change( 

  sys_oprt_mode_e_type              oprt_mode,
    /* The new operating mode
    */ 

  cm_mode_pref_e_type               mode_pref
    /* The mode preference the phone is in
    */
);

#endif /* FEATURE_GPSONE */
#endif /* CMXPD_H */
