/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   P D   M A N A G E R

GENERAL DESCRIPTION
  This module contains functions used by the call manager in processing
  position determination messages.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000 - 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmxpd.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/12   sg      Add required changes to support TDS in Automatic mode
04/11/11   am      Cleaning stubs and runnning qtf SUTE
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
12/23/10   sv      Integrate DSDS feature
11/15/10   rm      Memory optimization changes
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
07/23/10   xs      Remove subscription buffering
06/15/10   aj      support for IRAT measurement support
05/21/10   ak      Mainlining memory optimization changes.
12/08/09   fj      Added lte_band_pref in cmph_force_pref_on_the_fly().
10/14/09   sg      ZI memory reduction. Mmode should call new Heap Manager API to
                   allocate memory dynamically.
08/04/09   np      Added handling pref mode according to LTE system mode addition
09/01/09   sv      Moving from clk API to time API
04/24/09   rm      Including cmlog_v.h
02/23/09   rm      Removing cm_log_v.h
12/15/08   ak      Allow PD session grant in LPM mode.
11/05/08   aj      check to grant PD session if cmregprx is not ACTIVE
10/01/08   sv      Corrected featurization to remove lint errors.
06/12/08   sv      Resolve Klocwork defects
04/16/08   vk      updated code to use pending flags
08/07/07   ic      Lint clean up
02/27/07   pk      Removing uapdms.h
02/12/07   pk      Honouring phone's band preference for HDR systems
02/09/07   pk      Added checks for CDMA subscriptions for GSM builds
01/23/07   pk      Controlling cmph_mo_sms_end under CDMA features
07/07/06   pk      If a PD session is ended while changing the operating mode
                   to ONLINE, the operating mode change command is put on
                   hold for atmost a second, or PD handshake is received
05/08/06   pk      Modified session grant checks for DMOD sessions and HDR
                   call originations
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
12/02/05   pk      If AT is in a CDMA call, grant PD session without checking
                   the last mode forced to SD
11/21/05   ic      Fixed "enumerated type mixed with another type" compiler
                   warning in cmpd_call_start()
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/25/05   pk      Fixed RVCT compiler warnings
10/18/05   pk      Modified the PD session grant proc as per the final design
09/02/05   pk      Added support for GPS session, added support for
                   CM_PD_SESSION_TYPE_WITH_GPS_NORMAL
                   CM_PD_SESSION_TYPE_WITH_GPS_DMOD
08/15/05   pk      Added support for priority Queues for origination
                   parameters
07/15/05   ic      Lint fixes for CDMA + GW targets
02/01/05   dk      Added WLAN Support.
01/24/05   ic      Replaced cmph_is_ph_in_emerg_cb_mode() with
                   cmph_is_in_emergency_cb()
01/04/05   sj      Corrected base lat/long info in BS status response.
11/17/04   sj      Added base station info in the PD event.
08/20/04   sj      Added support for PRL Preference.
08/09/04   ic      Lint clean up for SNAR configuration
08/04/04   ic      CM Lint clean up - cmxpd.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmxpd_dummy() to get rid of ADS 1.2 compiler warning
07/15/04   sj      Changed pref_reason to ORIG_START_CS.
07/14/04   ws      Fixed compiler warnings with the addition of modifying info_structs
                   in event generations.
05/27/04   ic      In cmdbm_client_cmd_proc() call cmdbm_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
04/23/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.28 and MSMSHARED_CM.01.00.10.00.31
04/12/04   sj      Added support for setting DDTM on while doing GPS fix.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
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
11/03/03   ic      Merged in CR fixes over from other targets
10/23/03   ic      Corrected typos, replaced UNAVIALABLE with UNAVAILABLE
04/28/03   AT      Lint cleanup.
04/14/03   ws      Added BSID changes
02/28/03   vt      Added code to block/unblock HDR data call origination.
01/21/03   ws      Updated copyright information for 2003
11/15/02   vt      Fixed a compile error when comparing for valid client.
11/14/02   vt      Merged changes from GW mainline.
                   [ws]Merged in position determination changes
08/30/02   vt      Fixed a typo in an EMERGENCY enum
04/09/02   sh      Added one new argument in cmph_mo_sms_end
04/04/02   hxw     Modified FEATURE_GPSONE_MSBASED.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
02/06/02   sh      Added hybrid 3.0 support
05/07/02   prk     Renamed to cmxpd.c.
01/23/02   ic      Fixes related to running with FEATURE_GPSONE_* enabled
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
12/05/01    ic     Fixed "Warning: C2207W: inventing 'extern int srch_rude_wakeup();'"
11/30/01    hxw    Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                   mechanism.
10/23/01   VT/IC   Baseline for CCS
08/21/01    ic     Main-lined FEATURE_NEWCM
08/01/01    PJ     Made SD2.0 changes for handling sending of DBM msgs.
05/31/01    RC     Corrected compile time error when CM_DEBUG is defined.
01/29/01    HQ     Properly feature-ized NEWCM and FEATURE_GPSONE_DBM.
08/21/00    PJ     Made code review changes.
08/07/00    YLL    Declared the cmpd_mo_msg as static so that it can be
                   referred to get the numBytes and payload information.
06/22/00    PJ     Created.

===========================================================================*/

/*lint -esym(766,customer.h)*/
#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_GPSONE
#include "cm.h"
#include "cm_v.h"
#include "cmi.h"
#include "cmxpd.h"
#include "cmcall.h"
#include "cmdbg.h"
#include "cmcall.h"
#include "cmph.h"
#include "cmclient.h"
#include "cmlog.h"
#include "cmlog_v.h"
#include "cmtaski.h"

#ifdef CM_GW_SUPPORTED
#include "cmregprxi.h"/* Regproxy implementation related declarations. */
#endif /* #if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA) ) */

#if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900) )
#include "time_svc.h"
#endif /* (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900) )*/

#ifdef CM_DEBUG
#error code not present
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                  DEFINES                                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                  GLOBALS                                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-----------------------------------
  Internal PD session state machine
 *-------------------------------- */

#ifdef FEATURE_HDR_HYBRID
typedef struct cmpd_fix_s {
  boolean  status;     /* PD fix status */
} cmpd_fix_s_type;

static cmpd_fix_s_type cmpd_fix;
#endif /* FEATURE_HDR_HYBRID */

static cmpd_session_s_type *cmpd_session[CMPD_SESSION_OBJ_MAX];
  /* The PD session object array */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                               FUNCTION DEFINITIONS                      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

FUNCTION cmpd_init_obj

DESCRIPTION
  Initializing the PD object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpd_init_obj( cmpd_session_s_type *pd_ptr )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pd_ptr->session_state       = CMPD_SESSION_STATE_NONE;
  pd_ptr->session_type        = CM_PD_SESSION_NONE;
  pd_ptr->end_status          = CM_PD_SESSION_STATUS_NONE;
  pd_ptr->dormant_status      = CM_PD_SESSION_DORMANT_NONE;
  pd_ptr->is_mode_forced      = FALSE;
  pd_ptr->hold_session_uptime = CMPD_HOLD_SESSION_TIME_NONE;
}

/*===========================================================================

FUNCTION cmpd_ptr

DESCRIPTION
  Get the pointer to the PD session object

DEPENDENCIES
  pd object must have already been initialized with
  cmpd_init().

RETURN VALUE
  Pointer to the PD session object

SIDE EFFECTS
  none

===========================================================================*/
cmpd_session_s_type  *cmpd_ptr( cmpd_session_obj_e_type pd_session_obj )
{

  if( !BETWEEN(pd_session_obj, CMPD_SESSION_OBJ_NONE, CMPD_SESSION_OBJ_MAX))
  {
    CM_ERR_FATAL_0("pd_session_obj is out-of-bounds");
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( cmpd_session[pd_session_obj] == NULL )
  {
    cmpd_session[pd_session_obj] = (cmpd_session_s_type*)
                                  cm_mem_malloc (sizeof(cmpd_session_s_type));
    cmpd_init_obj(cmpd_session[pd_session_obj]);
  }

  return cmpd_session[pd_session_obj];

} /* cmpd_ptr() */


/*===========================================================================

FUNCTION cmpd_info_get

DESCRIPTION
 Copy the pd info into the pd info buffer.

DEPENDENCIES
  none

RETURN VALUE
 none

SIDE EFFECTS
  none

===========================================================================*/
static void  cmpd_info_get(
  cm_pd_info_s_type *pd_info_ptr,
  /* The info ptr
  */

  const cmpd_session_s_type *pd_ptr
  /* The information to copy
  */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( pd_info_ptr != NULL );
  CM_ASSERT( pd_ptr      != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pd_info_ptr->end_status     = pd_ptr->end_status;
  pd_info_ptr->session_type   = pd_ptr->session_type;
  pd_info_ptr->dormant_status = pd_ptr->dormant_status;

  #ifdef FEATURE_IS2000_REL_A
  {
    int pi_cnt =0;

    pd_info_ptr->pilot_info.num_pilots = pd_ptr->pilot_info.num_pilots;

    for( pi_cnt = 0; pi_cnt < pd_ptr->pilot_info.num_pilots; pi_cnt++ )
    {
      pd_info_ptr->pilot_info.pi_rec[pi_cnt].base_id
                      = pd_ptr->pilot_info.pi_rec[pi_cnt].base_id;
      pd_info_ptr->pilot_info.pi_rec[pi_cnt].nid
                      = pd_ptr->pilot_info.pi_rec[pi_cnt].nid;

      pd_info_ptr->pilot_info.pi_rec[pi_cnt].pilot_pn
                      =  pd_ptr->pilot_info.pi_rec[pi_cnt].pilot_pn;
      pd_info_ptr->pilot_info.pi_rec[pi_cnt].sid
                      = pd_ptr->pilot_info.pi_rec[pi_cnt].sid;
      pd_info_ptr->pilot_info.pi_rec[pi_cnt].base_lat_long_incl
                      = pd_ptr->pilot_info.pi_rec[pi_cnt].base_lat_long_incl;
      pd_info_ptr->pilot_info.pi_rec[pi_cnt].base_lat
                      = pd_ptr->pilot_info.pi_rec[pi_cnt].base_lat;
      pd_info_ptr->pilot_info.pi_rec[pi_cnt].base_long
                      = pd_ptr->pilot_info.pi_rec[pi_cnt].base_long;

    } /* for */
  }
  #endif

  return;

} /* cmpd_info_get */


/*===========================================================================

FUNCTION cmpd_info_alloc

DESCRIPTION
 Allocate a pd info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated pd info buffer.

SIDE EFFECTS
  none

===========================================================================*/
static cm_pd_info_s_type  *cmpd_info_alloc( void )
{
  cm_pd_info_s_type  *pd_info_ptr = NULL;

   /*
  ** Dynamically allocate a sups info object
  */
  pd_info_ptr = (cm_pd_info_s_type *)cm_mem_malloc
                                          (sizeof(cm_pd_info_s_type));

  return pd_info_ptr;
} /* cmpd_info_alloc() */


/*===========================================================================

FUNCTION cmpd_event

DESCRIPTION
  Notify the client list of a specified pd event.

DEPENDENCIES
  pd object must have already been initialized with
  cmpd_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cmpd_event(

  const cmpd_session_s_type *pd_ptr,
    /* pointer to a pd object.  */

  cm_pd_event_e_type         pd_event
    /* notify client list of this pd event */
)
{

  cm_pd_info_s_type  *pd_info_ptr = cmpd_info_alloc();
    /* Allocate pd info buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_ptr      != NULL );
  CM_ASSERT( pd_info_ptr != NULL );
  CM_ASSERT( BETWEEN(pd_event, CM_PD_EVENT_NONE, CM_PD_EVENT_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1( "pd event=%d",pd_event);

  cmpd_info_get( pd_info_ptr, pd_ptr );

  /*---------------------------------
    Now notify the clients of this
    event.
   *------------------------------ */

  cmclient_list_pd_event_ntfy(pd_event, pd_info_ptr);

  /* Deallocate the dynamically allocated memory after sending the pd event. */
  cm_mem_free(pd_info_ptr);

}

/*===========================================================================

FUNCTION cmpd_reset

DESCRIPTION
  Reset the PD object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpd_reset( cmpd_session_obj_e_type pd_session_obj )
{
  /* The init and reset functions are same,
  */
  cm_mem_free(cmpd_session[pd_session_obj]);
  cmpd_session[pd_session_obj] = NULL;
}

/*===========================================================================

FUNCTION cmpd_init

DESCRIPTION
  Initializing the PD object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpd_init( void )
{

  memset(cmpd_session,0,CMPD_SESSION_OBJ_MAX*sizeof(cmpd_session_s_type*));

  #ifdef FEATURE_HDR_HYBRID
  cmpd_fix.status = FALSE;
  #endif /* FEATURE_HDR_HYBRID */
}

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION cmpd_client_reva_cmd_proc

DESCRIPTION
  Processes RevA commands found on the CM command queue.

DEPENDENCIES
  Assumes that the Mobile is on the traffic channel and hence it can send
  the message out anytime.(no need for a rude wake)

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

static void cmpd_client_reva_cmd_proc
(
      cm_pd_cmd_s_type   *reva_cmd_ptr
)
{
  cm_pd_cmd_reva_info_s_type *cmd_info_ptr;

  mc_msg_type *mc_ptr;
  int counter;
  uint16 num_pilots;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  CM_ASSERT(reva_cmd_ptr != NULL);
  CM_ASSERT(reva_cmd_ptr->cmd_type == CM_CMD_TYPE_PD);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr = &reva_cmd_ptr->revAinfo;

  CM_MSG_LOW_1("RevA forwarding message (id %x) to MC",
                reva_cmd_ptr->data_block_ptr);

  mc_ptr = cm_mc_get_buf_else_err_fatal();

  mc_ptr->hdr.cmd             = MC_BSSTATUS_REQ_F;

  /*copy out the pilots into the mc cmd*/
  num_pilots = cmd_info_ptr->num_pilots;

  mc_ptr->bssrequest.num_pilots = num_pilots;

  for( counter=0 ; counter<num_pilots ; counter++ )
  {
      CM_MSG_LOW_1( "PN Offset = %d",cmd_info_ptr->pilot_pn[counter]);
      mc_ptr->bssrequest.pilot_pn[counter] = cmd_info_ptr->pilot_pn[counter];
  }

  cm_mc_send_cmd( mc_ptr );
}
#endif /*FEATURE_IS2000_REL_A*/

/*===========================================================================

FUNCTION cmpd_is_session_granted

DESCRIPTION
  Checks if we already have a compatible session granted

DEPENDENCIES
  cmpd_init() must have been called.

RETURN VALUE
  True if we have a compatible session granted
  False otherwise

SIDE EFFECTS
  none
===========================================================================*/

static boolean cmpd_is_session_granted(

  cm_pd_session_type_e_type session_type
    /* The type of the session requested
    */
)
{
   cmpd_session_s_type *pd_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If session state is not granted, return FALSE
  */

  switch(session_type)
  {
    case CM_PD_SESSION_TYPE_WITH_SYS_TIME:
    case CM_PD_SESSION_TYPE_WITH_GPS_MODE:
    case CM_PD_SESSION_TYPE_WITH_GPS_NORMAL:

      pd_ptr = cmpd_ptr(CMPD_SESSION_OBJ_MAIN);
      if( pd_ptr->session_state == CMPD_SESSION_STATE_GRANTED )
      {
        return TRUE;
      }
      return FALSE;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PD_SESSION_TYPE_WITH_GPS_DMOD:

      pd_ptr = cmpd_ptr(CMPD_SESSION_OBJ_DMOD);
      CM_ASSERT( pd_ptr != NULL );

      if( pd_ptr->session_state == CMPD_SESSION_STATE_GRANTED )
      {
        return TRUE;
      }
      return FALSE;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      return FALSE;
  }/*switch */

}/* cmpd_is_session_granted */


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
void cmpd_session_abort(

  cm_pd_session_status_e_type abort_reason,
    /* Abort reason
    */
  cm_pd_session_type_e_type   session_type,
    /* Session type
    */
  dword          duration
    /* duration in seconds to delay unforcing the mode
    */
)
{
  cmpd_session_obj_e_type session_obj_type = ( session_type == CM_PD_SESSION_TYPE_WITH_GPS_DMOD ?
                                               CMPD_SESSION_OBJ_DMOD  :
                                               CMPD_SESSION_OBJ_MAIN );
  cmpd_session_s_type *pd_ptr = cmpd_ptr(session_obj_type);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_ptr != NULL );
  CM_ASSERT( BETWEEN( abort_reason,CM_PD_SESSION_STATUS_NONE,
                      CM_PD_SESSION_STATUS_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 

  if( pd_ptr->session_state == CMPD_SESSION_STATE_ABORTED &&
      abort_reason == CM_PD_SESSION_STATUS_END )
  {
    /* Reset internal session State machine.
    */
    cmpd_reset(session_obj_type);
    return;
  }


  /* If no PD session then return.
  */
  if( pd_ptr->session_state == CMPD_SESSION_STATE_NONE    ||
      pd_ptr->session_state == CMPD_SESSION_STATE_ABORTED ||
      session_type == CM_PD_SESSION_NONE )
  {
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pd_ptr->session_state = CMPD_SESSION_STATE_ABORTED;

  if( abort_reason == CM_PD_SESSION_STATUS_END &&
      pd_ptr->session_type == CM_PD_SESSION_TYPE_WITH_GPS_MODE )
  {
    /* If session is ended by user,
    ** Just tell PH object to perform End action.
    */
    pd_ptr->end_status = CM_PD_SESSION_STATUS_END;
    #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
    cmph_mo_sms_end( duration, (cm_act_id_type)pd_ptr );
    #endif

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( abort_reason != CM_PD_SESSION_STATUS_ABORT_GPS_MODE_EXIT ||
      pd_ptr->end_status != CM_PD_SESSION_STATUS_END )
  {
    /* Copy the end status and the session type to the pd object
    ** this is used to report correct parameters to the registered clients
    */
    pd_ptr->end_status   = abort_reason;

    if( cmpd_is_session_granted(session_type) )
    {
      pd_ptr->session_type = session_type;
    }

    #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
    /* Tell PH object to perform End action.
    */
    cmph_mo_sms_end( (dword)duration, (cm_act_id_type)pd_ptr );
    #endif

    /* Call cmph_pd_session_end() if the overall state is idle and
    ** We are not in a simultaneous session or we are ending all sessions
    */

    if( cmcall_overall_call_state() == CM_OVERALL_CALL_STATE_IDLE      &&
        cmpd_ptr(CMPD_SESSION_OBJ_MAIN)->end_status != CM_PD_SESSION_STATUS_GRANTED &&
        cmpd_ptr(CMPD_SESSION_OBJ_DMOD)->end_status != CM_PD_SESSION_STATUS_GRANTED )
    {
      cmph_pd_session_end();
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether we need to unforce the system selection preference
  ** that was forced automatically by CM during origination.
  */

  cmph_check_and_unforce_orig_mode( CM_SS_MAIN );
  cmph_check_and_unforce_orig_mode( CM_SS_HDR );

  /* Notify client a PD event.
  */
  CM_MSG_HIGH_1("cmpd_session_abort() Notify PD. Status=%d", pd_ptr->end_status);

  cmpd_event(pd_ptr, CM_PD_EVENT_SESSION_END);

  if( abort_reason == CM_PD_SESSION_STATUS_END )
  {
    /* Reset internal session State machine.
    */
    cmpd_reset(session_obj_type);
  }

  #if (!(defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)))
  SYS_ARG_NOT_USED(duration);
  #endif

} /* cmpd_session_abort */

/*===========================================================================

FUNCTION cmpd_session_grant_status

DESCRIPTION
  Initial check to determine if the GPS session can be granted

DEPENDENCIES
  cmpd_init() must have been called.

RETURN VALUE
  CM_PD_SESSION_STATUS_OK_TO_GRANT if session can be granted in the
  current state of the call/phone, specific cm_pd_session_status_e_type
  otherwise.

SIDE EFFECTS
  none
===========================================================================*/

static cm_pd_session_status_e_type cmpd_session_is_ok_to_grant(
  cm_pd_session_type_e_type session_type
    /* The type of the session requested
    */
)
{

  cm_overall_call_state_e_type cm_call_state    = CM_OVERALL_CALL_STATE_NONE;
  cmcall_s_type           *call_ptr;
  cm_act_orig_s_type      *last_pref_ptr = cmph_get_last_pref_sent(CM_SS_MAIN);
  cmss_s_type             *ss_ptr        = cmss_ptr();
  cmph_s_type             *ph_ptr        = cmph_ptr();
  cm_ph_cmd_e_type        oprt_mode_cmd  = CM_PH_CMD_OPRT_MODE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( last_pref_ptr != NULL );
  CM_ASSERT( ss_ptr        != NULL );
  CM_ASSERT( ph_ptr        != NULL );
  CM_ASSERT( BETWEEN( session_type, CM_PD_SESSION_NONE, CM_PD_SESSION_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If a operating mode change command is pending, deny the session request
  */

  if( cm_hold_cmd_q_exists_cmd( CM_CMD_TYPE_PH,
                                &oprt_mode_cmd ))
  {
    return( CM_PD_SESSION_STATUS_DENY_CMD_ON_HOLD );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If AT is in a 1X call, grant the session, the Queue mechanism will
  ** take care to force the mode for the GPS session
  */

  if( cmcall_is_there_a_cdma_call_in_conv() != CM_CALL_ID_INVALID )
  {
      /* Return ok to grant otherwise
      */
      return( CM_PD_SESSION_STATUS_GRANTED );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #ifdef CM_GW_SUPPORTED
  /* Check if CDMA subscription is available
  */
  if( !ph_ptr->is_cdma_subscription_available )
  {
      return( CM_PD_SESSION_STATUS_DENY_NO_CDMA_SUBSC );
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If overall call state is orig, and the orig_mode is GW only, then
  ** wait until we move out of origination state
  */

  cm_call_state = cmcall_overall_call_state();

  if( cm_call_state == CM_OVERALL_CALL_STATE_ORIG ||
      cm_call_state == CM_OVERALL_CALL_STATE_ORIG_INCOM )
  {

    cm_orig_q_s_type  *ph_orig_ptr = cmtask_orig_para_get_top( CM_SS_MAIN );
    CM_ASSERT( ph_orig_ptr != NULL );

    /*lint -e774 Boolean within 'if' always evaluates to True
    **                 This is due to CM_ASSERT() above (if we get to this line
    **                 then it must be true) but CM_DEBUG may be off on
    **                 some targets so we do want to test against NULL here
    */

    if( ( ph_orig_ptr != NULL ) &&
        ( ph_orig_ptr->orig->act_id != (cm_act_id_type)ph_ptr ) &&
        ( ph_orig_ptr->orig->orig_mode_pref == CM_MODE_PREF_EMERGENCY ) )

    {
      return( CM_PD_SESSION_STATUS_DENY_GSM_CALL );
    }

    if( ( ph_orig_ptr != NULL ) &&
        ( ph_orig_ptr->orig->act_id != (cm_act_id_type)ph_ptr ) &&
        ( cmcall_misc_is_mode_pref( ph_orig_ptr->orig->orig_mode_pref,
                                  CM_MODE_PREF_GSM_WCDMA_ONLY ) )
      )

    /*lint -restore
    */
    {
      return( CM_PD_SESSION_STATUS_DENY_GSM_ORIG );
    }
  }

  call_ptr = cmcall_get_call_obj_in_call_state(CM_CALL_STATE_ORIG);
  if( call_ptr != NULL &&
      call_ptr->favored_mode_pref == CM_MODE_PREF_EMERGENCY )
  {
    return( CM_PD_SESSION_STATUS_DENY_GSM_CALL );
  }

  if( call_ptr != NULL &&
      cmcall_misc_is_mode_pref( call_ptr->favored_mode_pref,
                                CM_MODE_PREF_GSM_WCDMA_ONLY ))
  {
    return( CM_PD_SESSION_STATUS_DENY_GSM_ORIG );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_ptr = cmcall_get_call_obj_in_call_state(CM_CALL_STATE_INCOM);
  if( call_ptr != NULL &&
      call_ptr->favored_mode_pref == CM_MODE_PREF_EMERGENCY )
  {
    return( CM_PD_SESSION_STATUS_DENY_GSM_CALL );
  }

  if( call_ptr != NULL &&
      cmcall_misc_is_mode_pref( call_ptr->favored_mode_pref,
                                CM_MODE_PREF_GSM_WCDMA_ONLY ))
  {
    return( CM_PD_SESSION_STATUS_DENY_GSM_ORIG );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Check if we are in a GSM call
  ** HDR call depending on the session request
  ** If fail, return the end status
  */

  if( ! cmcall_is_no_call_in_gw_cs(CM_SS_MAIN) ||
      ! cmcall_is_no_call_in_gw_ps(CM_SS_MAIN) )
  {
    return( CM_PD_SESSION_STATUS_DENY_GSM_CALL );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if AT is in a HDR call or HDR origination
  */

  call_ptr = cmcall_get_ps_call_obj_in_orig_mode();

  if( session_type == CM_PD_SESSION_TYPE_WITH_GPS_DMOD &&
      ( cmcall_is_hdr_call() != CM_CALL_ID_INVALID ||
         ( call_ptr != NULL && ( call_ptr->ss == CM_SS_HDR ||
             ( call_ptr->ss == CM_SS_MAIN &&
              ss_ptr->info.sys_mode == SYS_SYS_MODE_HDR ) )
         )
       )
    )
  {
    return( CM_PD_SESSION_STATUS_DENY_HDR_CALL );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we forced SD with GW, force SD again and wait one second and run
  ** all the checks again
  */

  /*lint -save -e774 Boolean within 'if' always evaluates to True
  **                 This is due to CM_ASSERT() above (if we get to this line
  **                 then it must be true) but CM_DEBUG may be off on
  **                 some targets so we do want to test against NULL here
  */

  if( ( last_pref_ptr != NULL ) &&
      ( last_pref_ptr->act_priority == CM_ACT_PRIORITY_PH ) &&
        cmcall_misc_is_mode_pref( last_pref_ptr->orig_mode_pref,
                                  CM_MODE_PREF_GSM_WCDMA_ONLY ) &&
      ( ph_ptr->oprt_mode == SYS_OPRT_MODE_ONLINE )
    )
  /*lint -restore
  */
  {
    return( CM_PD_SESSION_STATUS_HOLD_FORCE_MODE );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return ok to grant otherwise
  */
  return( CM_PD_SESSION_STATUS_GRANTED );

} /* cmpd_session_grant_status */


/*===========================================================================

FUNCTION cmpd_session_force_mode

DESCRIPTION
  Force the mode for the session start

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/

static void cmpd_session_force_mode
(
  cm_pd_session_type_e_type session_type,
    /* Session type
    */

  boolean grant_session
    /* Should the session be granted
    */ )
{

  cm_mode_pref_e_type         cm_curr_mode_pref = CM_MODE_PREF_NONE;
  cmph_s_type                *ph_ptr            = cmph_ptr();
  cmpd_session_s_type        *pd_ptr            =
                        ( session_type == CM_PD_SESSION_TYPE_WITH_GPS_DMOD ?
                                          cmpd_ptr(CMPD_SESSION_OBJ_DMOD)  :
                                          cmpd_ptr(CMPD_SESSION_OBJ_MAIN) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the mode is set to HDR only don't add 1x and remove GW
  ** just grant the session
  */

  if( ph_ptr->main_stack_info.pref_info.mode_pref == CM_MODE_PREF_HDR_ONLY )
  {
    if( grant_session && pd_ptr->session_state != CMPD_SESSION_STATE_GRANTED )
    {
      cmph_pd_session_start();
      pd_ptr->session_type  = session_type;
      pd_ptr->session_state = CMPD_SESSION_STATE_GRANTED;
      cmpd_event(pd_ptr, CM_PD_EVENT_SESSION_GRANTED);
    }
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add CDMA to the mode, remove GSM,UMTS,AMPS,LTE from the mode
  */
  cm_curr_mode_pref = cmcall_remove_mode_pref_components(
                                                       ph_ptr->main_stack_info.pref_info.mode_pref,
                                                       2,
                                                       SD_SS_MODE_PREF_AMPS,
                                                       SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE );

  /* If there is only GSM or AMPS component in the mode pref then we
  ** get a CM_MODE_PREF_NONE here and the next function call to add mode
  ** preferences will fail. To protect against that, we set the mode
  ** pref to CM_MODE_PREF_CDMA_ONLY here
  */

  if( cm_curr_mode_pref == CM_MODE_PREF_NONE )
  {
    cm_curr_mode_pref = CM_MODE_PREF_CDMA_ONLY;
  }

  cm_curr_mode_pref = cmcall_add_mode_pref_components(
                                                    cm_curr_mode_pref,
                                                    1,
                                                    SD_SS_MODE_PREF_CDMA );

  /* If we get a valid mode pref and we have not forced the mode yet
  ** force the mode now
  */

  if( cm_curr_mode_pref != CM_MODE_PREF_MAX && !pd_ptr->is_mode_forced )
  {
     (void)cmph_force_pref_on_the_fly
             (
               CM_SS_MAIN,
               ph_ptr,
               SD_SS_PREF_REAS_ORIG_START_PS,
               CM_ACT_TYPE_GPS,
               CM_PREF_TERM_CM_1_CALL,
               cm_curr_mode_pref,
               ph_ptr->main_stack_info.pref_info.band_pref,
               ph_ptr->main_stack_info.pref_info.lte_band_pref,
               ph_ptr->main_stack_info.pref_info.tds_band_pref,
               CM_PRL_PREF_NO_CHANGE,
               CM_ROAM_PREF_NO_CHANGE,
               CM_HYBR_PREF_NO_CHANGE,
               ph_ptr->main_stack_info.pref_info.plmn,
               CM_SRV_DOMAIN_PREF_NO_CHANGE,
               CM_OTASP_ACT_CODE_NONE,
               ph_ptr->main_stack_info.pref_info.acq_order_pref,
               CM_NETWORK_SEL_MODE_PREF_NONE,
               (cm_act_id_type)pd_ptr,
               CM_ACT_UPDATE_REAS_ACT_START,
               TRUE,
               cmph_get_acq_pri_order_per_sub(CM_SS_MAIN),
               CM_DEFAULT_VALUE,
               SYS_CSG_ID_INVALID,
               SYS_RAT_NONE
             );

  }/* if( cm_curr_mode_pref != CM_MODE_PREF_MAX ) */

  pd_ptr->is_mode_forced = TRUE;

  /* Delay the session grant procedure only if GSM is
  ** active
  */

  #ifdef CM_GW_SUPPORTED
  if (cmregprx_get_stack_info_ptr(SD_SS_MAIN)->state    != CMREGPRX_STATE_ACTIVE &&
      cmregprx_get_stack_info_ptr(SD_SS_MAIN)->substate == CMREGPRX_SUBSTATE_NULL)
  #endif
  {
    /* If we have not already granted the session, or we are granting a
    ** Simultaneous session, generate the grant event
    */

    if( grant_session && pd_ptr->session_state != CMPD_SESSION_STATE_GRANTED )
    {
      cmph_pd_session_start();
      pd_ptr->session_type  = session_type;
      pd_ptr->session_state = CMPD_SESSION_STATE_GRANTED;
      cmpd_event(pd_ptr, CM_PD_EVENT_SESSION_GRANTED);
    }

  }

  return;

}/* cmpd_session_force_mode */


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

void cmpd_session_grant_proc( cm_pd_session_type_e_type session_type )
{

  cm_pd_session_status_e_type  cmpd_session_grant_status =
                                                   CM_PD_SESSION_STATUS_NONE;
  cm_call_id_type              call_id          = CM_CALL_ID_INVALID;
  cmpd_session_obj_e_type session_obj_type = ( session_type == CM_PD_SESSION_TYPE_WITH_GPS_DMOD ?
                                               CMPD_SESSION_OBJ_DMOD  :
                                               CMPD_SESSION_OBJ_MAIN );
  cmpd_session_s_type    *pd_ptr           = cmpd_ptr(session_obj_type);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_ptr != NULL );
  CM_ASSERT( BETWEEN( session_type, CM_PD_SESSION_NONE, CM_PD_SESSION_MAX ));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmpd_session_grant_status = cmpd_session_is_ok_to_grant( session_type );


  if( cmpd_session_grant_status == CM_PD_SESSION_STATUS_HOLD_FORCE_MODE )
  {
    /* Force the mode, but don't grant the session
    */

    time_type time_now;
    time_get_ms( time_now );

    /* Lint complains about suspicious cast, time_now is of type qword and
    ** we are casting it to uint64 and then taking the modulus of the
    ** resulting number
    */
    /*lint -save -e740 */

    if( (QW_CVT_Q2N(time_now) % 1000 ) >= 500 )
    {
      pd_ptr->hold_session_uptime =
                         time_get_uptime_secs() + CMPD_HOLD_SESSION_TIME + 1;
    }
    else
    {
      pd_ptr->hold_session_uptime =
                             time_get_uptime_secs() + CMPD_HOLD_SESSION_TIME;
    }

    /*lint -restore */

    cmpd_session_force_mode( session_type, FALSE );
    return;

  }

  /* If we are in origination state and the call might go on GW, then
  ** wait till we move out of origination state
  */

  if( cmpd_session_grant_status == CM_PD_SESSION_STATUS_DENY_GSM_ORIG )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pd_ptr->end_status        = cmpd_session_grant_status;

  /* If the session cannot be granted, then report session deny event to
  ** clients
  */

  if( cmpd_session_grant_status != CM_PD_SESSION_STATUS_GRANTED )
  {
    pd_ptr->session_state = CMPD_SESSION_STATE_NONE;
    pd_ptr->session_type  = session_type;
    cmpd_event( pd_ptr, CM_PD_EVENT_SESSION_DENIED );
    cmpd_reset(session_obj_type);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the dormancy status
  */

  pd_ptr->dormant_status = CM_PD_SESSION_DORMANT_NONE;

  /* Force the mode if applicable and then generate the start event
  */

  /* Check if we are in a 1X call, if we are then don't force the mode
  ** Grant the session, but remember to force the mode when the call ends
  */

  call_id = cmcall_is_there_a_cdma_call_in_conv();

  if( call_id != CM_CALL_ID_INVALID )
  {
    cmph_pd_session_start();
    pd_ptr->is_mode_forced = FALSE;
    pd_ptr->session_state  = CMPD_SESSION_STATE_GRANTED;
    pd_ptr->session_type   = session_type;
    cmpd_event(pd_ptr, CM_PD_EVENT_SESSION_GRANTED);
    return;
  }

  cmpd_session_force_mode( session_type, TRUE );

}/* cmpd_session_grant_proc */


/*===========================================================================

FUNCTION cmpd_update_session_dormant

DESCRIPTION
  Updates the dormant state of the PD object

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the state of the PD object is updated
  FALSE Otherwise

SIDE EFFECTS
  none
===========================================================================*/
static boolean cmpd_update_session_dormant(

  cm_pd_session_type_e_type session_type,
    /* The session type for which the dormancy is to be updated
    */
  cm_pd_session_dormant_e_type dormant_status
    /* New dormancy status
    */
)
{

  cmpd_session_s_type *pd_ptr = ( session_type ==
                                   CM_PD_SESSION_TYPE_WITH_GPS_DMOD ?
                                   cmpd_ptr(CMPD_SESSION_OBJ_DMOD)  :
                                   cmpd_ptr(CMPD_SESSION_OBJ_MAIN) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_ptr != NULL );
  CM_ASSERT( CMPD_SESSION_TYPE_IS_VALID( session_type ) );
  CM_ASSERT( BETWEEN( dormant_status, CM_PD_SESSION_DORMANT_NONE,\
                      CM_PD_SESSION_DORMANT_MAX ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the session is not granted or if the dormant status hasn't changes,
  ** return FALSE
  */

  if( !cmpd_is_session_granted(session_type) ||
      pd_ptr->dormant_status == dormant_status )
  {
    CM_MSG_HIGH_2(" PD session dormancy in wrong state ses type. %d dor. state.%d"
                  ,session_type, dormant_status);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pd_ptr->dormant_status = dormant_status;
  cmpd_event( pd_ptr, CM_PD_EVENT_SESSION_DORMANT_CHGD );

  return TRUE;

} /* cmpd_update_session_dormant */


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

boolean cmpd_is_session_active( const cmpd_session_s_type  *pd_ptr)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(pd_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(pd_ptr->session_state)
  {
    case CMPD_SESSION_STATE_NONE:
    case CMPD_SESSION_STATE_DENIED:
    case CMPD_SESSION_STATE_ABORTED:
      return FALSE;

    case CMPD_SESSION_STATE_INITIATED:
    case CMPD_SESSION_STATE_GRANTED:
      return TRUE;

    default:
        return FALSE;

  }/* switch(pd_ptr->session_state) */
}/* cmpd_is_session_active */


/*===========================================================================

FUNCTION cmpd_timer_proc

DESCRIPTION
  Does the timer handling for the pd object. If the PD session is waiting
  for GSM to shut down, then check if GSM is shut down and grant the session

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none
===========================================================================*/


void cmpd_timer_proc(

  cm_timer_event_type    timer_event
    /* Indicate specific timer event.
    */
)
{

  cmpd_session_s_type *pd_ptr      = cmpd_ptr( CMPD_SESSION_OBJ_MAIN);
  cmpd_session_s_type *pd_dmod_ptr = cmpd_ptr( CMPD_SESSION_OBJ_DMOD);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_ptr      != NULL );
  CM_ASSERT( pd_dmod_ptr != NULL );
  SYS_ARG_NOT_USED( timer_event );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are not waiting for GSM to shut down, return
  */

  if( pd_ptr->session_state      != CMPD_SESSION_STATE_INITIATED &&
      pd_dmod_ptr->session_state != CMPD_SESSION_STATE_INITIATED )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the hold timer hasn't expired return
  */

  if( ( pd_ptr->hold_session_uptime != CMPD_HOLD_SESSION_TIME_NONE &&
        pd_ptr->hold_session_uptime <= time_get_uptime_secs() ) &&
      ( pd_dmod_ptr->hold_session_uptime != CMPD_HOLD_SESSION_TIME_NONE &&
        pd_dmod_ptr->hold_session_uptime <= time_get_uptime_secs() ))
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we have forced the mode and waiting for GSM to shut down, grant the
  ** session now
  */

  if( pd_ptr->session_state == CMPD_SESSION_STATE_INITIATED &&
      pd_ptr->is_mode_forced
      #ifdef CM_GW_SUPPORTED
      && (cmregprx_get_stack_info_ptr(SD_SS_MAIN)->state    != CMREGPRX_STATE_ACTIVE)
      && (cmregprx_get_stack_info_ptr(SD_SS_MAIN)->substate == CMREGPRX_SUBSTATE_NULL )
      #endif
    )
  {
    cmpd_session_grant_proc( pd_ptr->session_type );
  }

  if( pd_dmod_ptr->session_state == CMPD_SESSION_STATE_INITIATED &&
      pd_dmod_ptr->is_mode_forced
      #ifdef CM_GW_SUPPORTED
      && (cmregprx_get_stack_info_ptr(SD_SS_MAIN)->state    != CMREGPRX_STATE_ACTIVE)
      && (cmregprx_get_stack_info_ptr(SD_SS_MAIN)->substate == CMREGPRX_SUBSTATE_NULL )
      #endif
    )
  {
    cmpd_session_grant_proc( pd_dmod_ptr->session_type );
  }

  return;
}

/*===========================================================================

FUNCTION cmpd_client_cmd_check

DESCRIPTION
  Check for PD command parameter errors and whether a specified PD command
  is allowed in the current state of the call/phone.

DEPENDENCIES
  None

RETURN VALUE
  CM_PD_CMD_ERR_NONE if command is allowed in the current state
  of the call/phone, specific cm_pd_cmd_err_e_type otherwise.

SIDE EFFECTS
  none
===========================================================================*/

static cm_pd_cmd_err_e_type cmpd_client_cmd_check(
    cm_pd_cmd_s_type          *pd_cmd_ptr
        /* Pointer to a CM command */
)
{
  cm_pd_cmd_info_s_type  *cmd_info_ptr = NULL;
      /* Point at command information component */
  cmph_s_type            *ph_ptr       = cmph_ptr();
      /* Pointer for phone object */
  cm_pd_cmd_err_e_type   cmd_err       = CM_PD_CMD_ERR_NOERR;
      /* Initialize command error to NO-ERROR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_cmd_ptr != NULL );
  CM_ASSERT( pd_cmd_ptr->cmd_type == CM_CMD_TYPE_PD );
  CM_ASSERT( ph_ptr != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CMD_INFO_PTR( pd_cmd_ptr );
  CM_MSG_HIGH_1( "cmpd_cmd_check, cmd=%d", pd_cmd_ptr->cmd);

  /* check cmd is already in error or not */

  if (cmd_info_ptr->cmd_err != CM_PD_CMD_ERR_NOERR)
  {
      return cmd_info_ptr->cmd_err;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether command is possible.
  */

  if( pd_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
  {
    /* Invalid client ID */
    CM_ERR_0( "CM_PD_CMD_ERR_CLIENT_ID_P");
    return CM_PD_CMD_ERR_CLIENT_ID_P;
  }

  if( (pd_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK )
  {
    /* Invalid client ID */
    CM_ERR_0( "CM_PD_CMD_ERR_CLIENT_ID_P");
    return CM_PD_CMD_ERR_CLIENT_ID_P;
  }

  switch( pd_cmd_ptr->cmd )
  {
    case CM_PD_CMD_SESSION_START:

      if( ! CMPD_SESSION_START_IS_OPRT_MODE_VALID(ph_ptr->oprt_mode))
      {
        cmd_err = CM_PD_CMD_ERR_OFFLINE_S;
        CM_ERR_0( "CM_PD_CMD_ERR_OFFLINE_S");
      }
      else if( ph_ptr->is_in_use )
      {
        /* Phone is already busy in other activity.
        */
        switch(pd_cmd_ptr->info.session_type)
        {
          case CM_PD_SESSION_TYPE_WITH_GPS_NORMAL:
          case CM_PD_SESSION_TYPE_WITH_GPS_DMOD:
            break;

          default:
            cmd_err = CM_PD_CMD_ERR_PHONE_BUSY;
            CM_ERR_0( "CM_PD_CMD_ERR_PHONE_BUSY");
            break;
        }/* switch(pd_cmd_ptr->info.session_type) */
      }
      else if( ph_ptr->main_stack_info.pref_info.mode_pref == CM_MODE_PREF_EMERGENCY )
      {
        /* Current origination mode is EMERG_CB
        */
        switch(pd_cmd_ptr->info.session_type)
        {
          case CM_PD_SESSION_TYPE_WITH_GPS_NORMAL:
          case CM_PD_SESSION_TYPE_WITH_GPS_DMOD:
            break;

          default:
            cmd_err = CM_PD_CMD_ERR_EMERGENCY_MODE_S;
            CM_ERR_0( "CM_PD_CMD_ERR_E911_MODE_S");
            break;
        }/* switch(pd_cmd_ptr->info.session_type) */

      }
      #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
      else if (!CMPD_SESSION_TYPE_IS_VALID(cmd_info_ptr->session_type)) {
        /* Invalid session type.
        */
        cmd_err = CM_PD_CMD_ERR_SESSION_TYPE_INVALID;
        CM_ERR_0( "CM_PD_CMD_ERR_SESSION_TYPE_INVALID");
      }
      break;
      #endif

    case CM_PD_CMD_SESSION_END:
      if (!CMPD_SESSION_TYPE_IS_VALID(cmd_info_ptr->session_type)) {
        /* Invalid session type.
        */
        cmd_err = CM_PD_CMD_ERR_SESSION_TYPE_INVALID;
        CM_ERR_0( "CM_PD_CMD_ERR_SESSION_TYPE_INVALID");
      }

      break;
    #ifdef FEATURE_HDR_HYBRID
    case CM_PD_CMD_FIX_START:

      if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
      {
        cmd_err = CM_PD_CMD_ERR_OFFLINE_S;
        CM_ERR_0( "CM_PD_CMD_ERR_OFFLINE_S");
      }
      else if( cmph_is_in_emergency_cb() )
      {
        /* Current origination mode is EMERG_CB
        */
        /* cmd_err = CM_PD_CMD_ERR_E911_MODE_S; */
        cmd_err = CM_PD_CMD_ERR_EMERGENCY_MODE_S;
        CM_ERR_0( "CM_PD_CMD_ERR_E911_MODE_S");
      }

      break;

    case CM_PD_CMD_FIX_END:
      break;
    #else
    case CM_PD_CMD_FIX_START:
    case CM_PD_CMD_FIX_END:
      CM_ERR_1("Unexpected command %d", pd_cmd_ptr->cmd);
      cmd_err = CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
      break;
    #endif /* FEATURE_HDR_HYBRID */

    case CM_PD_CMD_SESSION_DORMANT:
      break;

    default:
      break;
  }

  return cmd_err;
}

/*===========================================================================

FUNCTION cmpd_client_cmd_err

DESCRIPTION
  Notify clients of a specified pd command error.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpd_client_cmd_err(
    const cm_pd_cmd_s_type       *pd_cmd_ptr,
        /* Pointer to a CM command */
    cm_pd_cmd_err_e_type    cmd_err
        /* Indicate the command error */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_cmd_ptr != NULL );
  CM_ASSERT( pd_cmd_ptr->cmd_type == CM_CMD_TYPE_PD );
  CM_ASSERT( BETWEEN( cmd_err, CM_PD_CMD_ERR_NONE, CM_PD_CMD_ERR_MAX) );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  CM_MSG_LOW_3("START cmpd_client_cmd_err() pd_cmd=%d, cmd_err=%d, client=%p",
                  pd_cmd_ptr->cmd, cmd_err, pd_cmd_ptr->client_ptr );

  /*
  ** If commanding client supplied a callback function,
  ** notify this client of command error status.
  */
  if(pd_cmd_ptr->cmd_cb_func != NULL)
  {
    pd_cmd_ptr->cmd_cb_func(pd_cmd_ptr->data_block_ptr,
                            pd_cmd_ptr->cmd,
                            cmd_err);
  }

  /*---------------------------------
    If error status is other than NO ERROR,
    notify the client list of command error, as well.
   *------------------------------ */

  if(cmd_err != CM_PD_CMD_ERR_NOERR)
  {
    CM_MSG_HIGH_3( "PD cmd err, cmd=%d, err=%d, client=%ld",
            pd_cmd_ptr->cmd, cmd_err, pd_cmd_ptr->client_id );

    cmclient_list_pd_cmd_err_ntfy( cmd_err, pd_cmd_ptr );
  }
}

/*===========================================================================

FUNCTION cmpd_client_cmd_proc

DESCRIPTION
  Processes PD commands found on the CM command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  May force the mode of the phone if necessary.

===========================================================================*/

void cmpd_client_cmd_proc(
      cm_pd_cmd_s_type   *pd_cmd_ptr
)
{

  cm_pd_cmd_err_e_type   cmd_err;
  cm_pd_cmd_info_s_type *cmd_info_ptr;
    /* Point at command information component */
  cmpd_session_s_type   *pd_ptr        = cmpd_ptr(CMPD_SESSION_OBJ_MAIN);
  cmpd_session_s_type   *pd_dmod_ptr   = cmpd_ptr(CMPD_SESSION_OBJ_DMOD);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_ptr != NULL );
  CM_ASSERT( pd_dmod_ptr != NULL );
  CM_ASSERT(pd_cmd_ptr != NULL);
  CM_ASSERT(pd_cmd_ptr->cmd_type == CM_CMD_TYPE_PD);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr = CMD_INFO_PTR( pd_cmd_ptr );/* Point at command information component */

  cmd_err = cmpd_client_cmd_check(pd_cmd_ptr);

  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cmpd_client_cmd_err(pd_cmd_ptr, cmd_err);
  if( cmd_err != CM_PD_CMD_ERR_NOERR )
  {
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(pd_cmd_ptr->cmd)
  {
    case CM_PD_CMD_SESSION_START:

      /* Check if already have the session state granted
      ** If yes then just return granted
      */

      if( cmpd_is_session_granted( cmd_info_ptr->session_type ) )
      {
        /* Notify client a PD event.
        */

        if( cmd_info_ptr->session_type == CM_PD_SESSION_TYPE_WITH_GPS_DMOD )
        {
          cmpd_event( pd_dmod_ptr, CM_PD_EVENT_SESSION_START );
          cmph_pd_session_start();
          cmpd_event( pd_dmod_ptr, CM_PD_EVENT_SESSION_GRANTED );
        }
        else
        {
          cmpd_event( pd_ptr, CM_PD_EVENT_SESSION_START );
          cmph_pd_session_start();
          cmpd_event( pd_ptr, CM_PD_EVENT_SESSION_GRANTED );
        }

        break;
      }

      if(( pd_ptr->session_state != CMPD_SESSION_STATE_NONE &&
           pd_ptr->session_state != CMPD_SESSION_STATE_ABORTED )
           ||
         ( pd_dmod_ptr->session_state != CMPD_SESSION_STATE_NONE &&
           pd_dmod_ptr->session_state != CMPD_SESSION_STATE_ABORTED ))
      {
        CM_MSG_HIGH_0("PD session existing! Start Simultaneous sessions");
      }

      #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
      switch(cmd_info_ptr->session_type)
      {

        case CM_PD_SESSION_TYPE_WITH_SYS_TIME:

          CM_MSG_HIGH_0("Force Mode for PD session with SYS Time!");

          pd_ptr->session_state = CMPD_SESSION_STATE_INITIATED;
          pd_ptr->session_type  = cmd_info_ptr->session_type;
          pd_ptr->end_status    = CM_PD_SESSION_STATUS_NONE;


          /* Force phone preference to make it ready.
          */
          (void) cmph_force_pref_on_the_fly(
                                CM_SS_MAIN,
                                cmph_ptr(),
                                SD_SS_PREF_REAS_ORIG_START_CS,
                                CM_ACT_TYPE_EMERG_CALL,
                                CM_PREF_TERM_CM_MO_SMS,
                                CM_MODE_PREF_CDMA_ONLY,
                                cmph_ptr()->main_stack_info.pref_info.band_pref,
                                cmph_ptr()->main_stack_info.pref_info.lte_band_pref,
                                cmph_ptr()->main_stack_info.pref_info.tds_band_pref,
                                CM_PRL_PREF_NO_CHANGE,
                                CM_ROAM_PREF_NO_CHANGE,
                                CM_HYBR_PREF_NO_CHANGE,
                                cmph_ptr()->main_stack_info.pref_info.plmn,
                                CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                CM_OTASP_ACT_CODE_NONE,
                                cmph_ptr()->main_stack_info.pref_info.acq_order_pref,
                                CM_NETWORK_SEL_MODE_PREF_NONE,
                                (cm_act_id_type)pd_ptr,
                                CM_ACT_UPDATE_REAS_ACT_START,
                                TRUE,
                                cmph_get_acq_pri_order_per_sub(CM_SS_MAIN),
                                CM_DEFAULT_VALUE,
                                SYS_CSG_ID_INVALID,
                                SYS_RAT_NONE
                             );

          /* Notify client a PD event.
          */
          cmpd_event(pd_ptr, CM_PD_EVENT_SESSION_START);
          break;

        case CM_PD_SESSION_TYPE_WITH_GPS_MODE:

          CM_MSG_HIGH_0("Force Mode for PD session with GPS mode!");


          pd_ptr->session_state = CMPD_SESSION_STATE_INITIATED;
          pd_ptr->session_type  = cmd_info_ptr->session_type;
          pd_ptr->end_status    = CM_PD_SESSION_STATUS_NONE;

          /* Force phone preference to make it ready.
          */

           (void) cmph_force_pref_on_the_fly(
                                CM_SS_MAIN,
                                cmph_ptr(),
                                SD_SS_PREF_REAS_USER,
                                CM_ACT_TYPE_SMS_CALL,
                                CM_PREF_TERM_CM_MO_SMS,
                                CM_MODE_PREF_GPS_ONLY,
                                cmph_ptr()->main_stack_info.pref_info.band_pref,
                                cmph_ptr()->main_stack_info.pref_info.lte_band_pref,
                                cmph_ptr()->main_stack_info.pref_info.tds_band_pref,
                                CM_PRL_PREF_NO_CHANGE,
                                CM_ROAM_PREF_NO_CHANGE,
                                CM_HYBR_PREF_NO_CHANGE,
                                cmph_ptr()->main_stack_info.pref_info.plmn,
                                CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                CM_OTASP_ACT_CODE_NONE,
                                cmph_ptr()->main_stack_info.pref_info.acq_order_pref,
                                CM_NETWORK_SEL_MODE_PREF_NONE,
                                (cm_act_id_type)pd_ptr,
                                CM_ACT_UPDATE_REAS_ACT_START,
                                TRUE,
                                cmph_get_acq_pri_order_per_sub(CM_SS_MAIN),
                                CM_DEFAULT_VALUE,
                                SYS_CSG_ID_INVALID,
                                SYS_RAT_NONE
                             );

          /* Notify client a PD event.
          */
          cmpd_event(pd_ptr, CM_PD_EVENT_SESSION_START);
          break;

        case CM_PD_SESSION_TYPE_WITH_GPS_NORMAL:

          CM_MSG_HIGH_0("PD session with GPS Normal!");

          pd_ptr->session_state  = CMPD_SESSION_STATE_INITIATED;
          pd_ptr->session_type   = cmd_info_ptr->session_type;
          pd_ptr->end_status     = CM_PD_SESSION_STATUS_NONE;
          pd_ptr->is_mode_forced = FALSE;
          cmpd_event(pd_ptr, CM_PD_EVENT_SESSION_START);
          cmpd_session_grant_proc( cmd_info_ptr->session_type );
          break;

        case CM_PD_SESSION_TYPE_WITH_GPS_DMOD:

          CM_MSG_HIGH_0("PD session with GPS DMOD!");

          pd_dmod_ptr->session_state  = CMPD_SESSION_STATE_INITIATED;
          pd_dmod_ptr->session_type   = cmd_info_ptr->session_type;
          pd_dmod_ptr->end_status     = CM_PD_SESSION_STATUS_NONE;
          pd_dmod_ptr->is_mode_forced = FALSE;
          cmpd_event(pd_dmod_ptr, CM_PD_EVENT_SESSION_START);
          cmpd_session_grant_proc( cmd_info_ptr->session_type );
          break;

        default:

          break;

      } /* switch(cmd_info_ptr->session_type) */

      break;
    #endif  // #ifdef FEATURE_GPSONE_MSBASED

    case CM_PD_CMD_SESSION_END:
      {
        /*-------------------------
        End the PD session.
        *------------------------*/
        cmpd_session_abort( CM_PD_SESSION_STATUS_END,
                            cmd_info_ptr->session_type,
                            (dword)0 );

          /* The cmd_info_ptr->session_type is ended. Check if the other session
          ** is ended as well.
          */
          if( (cmd_info_ptr->session_type == CM_PD_SESSION_TYPE_WITH_GPS_DMOD ?
                                             pd_ptr->session_state  :
                                             pd_dmod_ptr->session_state)
                                                   ==  CMPD_SESSION_STATE_NONE )
        {
          cm_hold_cmd_handle( FALSE, CM_HOLD_CMD_REAS_GPS );
        }
      }
      break;

#ifdef FEATURE_HDR_HYBRID
    case CM_PD_CMD_FIX_START:

      /* Set internal PD fix status.
      */
      cmpd_fix.status = TRUE;

      /* Notify client a PD event.
      */
      pd_ptr->gps_fix_status = cmpd_fix.status;
      cmpd_event(pd_ptr, CM_PD_EVENT_FIX_START);
      cmlog_block_hdr_orig_during_gps();
      cmph_update_ddtm_status( cmph_ptr() );
      break;

    case CM_PD_CMD_FIX_END:
      cmpd_fix.status = FALSE;
      /* Notify client a PD event.
      */
      pd_ptr->gps_fix_status = cmpd_fix.status;
      cmpd_event(pd_ptr, CM_PD_EVENT_FIX_END);
      cmlog_allow_hdr_orig_during_gps();
      cmph_update_ddtm_status( cmph_ptr() );
      break;

#endif /* FEATURE_HDR_HYBRID */

#ifdef FEATURE_IS2000_REL_A
    case CM_PD_CMD_SEND_REVA:
      /* Ask MC to send Release A message.
      */
      cmpd_client_reva_cmd_proc(pd_cmd_ptr);
      break;
#endif /*FEATURE_IS2000_REL_A*/

    case CM_PD_CMD_SESSION_DORMANT:

      (void) cmpd_update_session_dormant( cmd_info_ptr->session_type,
                                          cmd_info_ptr->session_dormant );
      break;

    default:
      CM_ERR_1("Unknow PD command (%i) passed to CM", pd_cmd_ptr->cmd);
      break;
  }
}

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
)
{

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

  /*lint -e{826} keep it suppressed for general / specific walks as well */
  const cm_mc_rpt_type    *cm_mc_rpt_ptr = (cm_mc_rpt_type *) rpt_ptr;
    /* Pointer to MC report */

  cmpd_session_s_type   *pd_ptr         = NULL;

  #ifdef FEATURE_IS2000_REL_A
  int                    pi_cnt, max_pi_cnt ;
  #endif   /* FEATURE_IS2000_REL_A */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(cm_mc_rpt_ptr->hdr.cmd)
  {
    case CM_GPS_MODE_GRANTED_F:
      pd_ptr = cmpd_ptr(CMPD_SESSION_OBJ_MAIN);
      if ((pd_ptr->session_state == CMPD_SESSION_STATE_INITIATED) &&
          (pd_ptr->session_type == CM_PD_SESSION_TYPE_WITH_GPS_MODE)
         )
      {
        /* PD session with GPS Mode is now successfully granted.
        */
        CM_MSG_MED_0("Session with GPS Mode is granted");
        pd_ptr->session_state = CMPD_SESSION_STATE_GRANTED;

        /* Send a PD event to the client.
        */
        cmpd_event(pd_ptr, CM_PD_EVENT_SESSION_GRANTED);
      }
      break;

    case CM_GPS_MODE_EXIT_F:
      pd_ptr = cmpd_ptr(CMPD_SESSION_OBJ_MAIN);
      if (pd_ptr->session_type == CM_PD_SESSION_TYPE_WITH_GPS_MODE)
      {
        /*-------------------------
        End the PD session.
        *------------------------*/
        cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_GPS_MODE_EXIT,
                            CM_PD_SESSION_TYPE_WITH_GPS_MODE,
                            (dword)0 );
      }
      break;

    #ifdef FEATURE_IS2000_REL_A
    case CM_BS_STATUS_RSP_F:
      /*Received a RevA message send it over to PD
            copy into pd report and send to uapdms*/
      pd_ptr = cmpd_ptr(CMPD_SESSION_OBJ_MAIN);
      max_pi_cnt = cm_mc_rpt_ptr->bss_status_rsp.num_pilot;
      pd_ptr->pilot_info.num_pilots = (uint16)max_pi_cnt;

      CM_MSG_LOW_1("CASE:CMBSSTATUSRSP maxpicnt=%d", max_pi_cnt);
      for( pi_cnt = 0; pi_cnt < max_pi_cnt; pi_cnt++ )
      {
        pd_ptr->pilot_info.pi_rec[pi_cnt].base_id
          = cm_mc_rpt_ptr->bss_status_rsp.pi_rec[pi_cnt].base_id;
        pd_ptr->pilot_info.pi_rec[pi_cnt].nid
          = cm_mc_rpt_ptr->bss_status_rsp.pi_rec[pi_cnt].nid;

        //the +1 is for testing only to ensure the ref_pn we return is
        //not in the active or candidate set
        pd_ptr->pilot_info.pi_rec[pi_cnt].pilot_pn
          = cm_mc_rpt_ptr->bss_status_rsp.pi_rec[pi_cnt].pilot_pn;
        pd_ptr->pilot_info.pi_rec[pi_cnt].sid
          = cm_mc_rpt_ptr->bss_status_rsp.pi_rec[pi_cnt].sid;
        pd_ptr->pilot_info.pi_rec[pi_cnt].base_lat_long_incl
          = cm_mc_rpt_ptr->bss_status_rsp.pi_rec[pi_cnt].base_lat_long_incl;
        pd_ptr->pilot_info.pi_rec[pi_cnt].base_lat
          = cm_mc_rpt_ptr->bss_status_rsp.pi_rec[pi_cnt].base_lat;
        pd_ptr->pilot_info.pi_rec[pi_cnt].base_long
          = cm_mc_rpt_ptr->bss_status_rsp.pi_rec[pi_cnt].base_long;

      }
      cmpd_event( pd_ptr,CM_PD_EVENT_REVA_MSG );
      break;
      #endif /*FEATURE_IS2000_REL_A*/

    default:
      /* do nothing */
      break;
  }
  #else
  SYS_ARG_NOT_USED(rpt_ptr);
  #endif

}



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
)
{
  /*lint -e{826} keep it suppressed for general / specific walks as well */
  const cm_sd_rpt_u_type        *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
    /* Pointer to SD report */

  cmpd_session_s_type     *pd_ptr     = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( sd_rpt_ptr->hdr.cmd )
  {
    case CM_OK_TO_ORIG_F:
      pd_ptr = cmpd_ptr(CMPD_SESSION_OBJ_MAIN);
      if ((pd_ptr->session_state == CMPD_SESSION_STATE_INITIATED) &&
          (pd_ptr->session_type == CM_PD_SESSION_TYPE_WITH_SYS_TIME)
         )
      {
        /* PD session with system time is now successfully granted.
        */
        CM_MSG_MED_0("Session with Sys Time is granted");
        pd_ptr->session_state = CMPD_SESSION_STATE_GRANTED;

        /* Send a PD event to the client.
        */
        cmpd_event(pd_ptr, CM_PD_EVENT_SESSION_GRANTED);
      }
      break;

    case CM_ACQ_FAIL_F:
      pd_ptr = cmpd_ptr(CMPD_SESSION_OBJ_MAIN);
      if (pd_ptr->session_type == CM_PD_SESSION_TYPE_WITH_SYS_TIME)
      {
        /*-------------------------------------------*/
        /*   Acquisition on all systems has failed   */
        /*-------------------------------------------*/
        cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_ACQ_FAIL,
                            CM_PD_SESSION_TYPE_WITH_SYS_TIME,
                            (dword)0 );
      }
      break;

    default:
      /* do nothing */
      break;
  }
} /* cmpd_sd_rpt_proc() */


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

boolean cmpd_process_oprt_mode_change(

  sys_oprt_mode_e_type              oprt_mode,
    /* The new operating mode
    */

  cm_mode_pref_e_type               mode_pref
    /* The mode preference the phone is in
    */
)
{

  switch (oprt_mode)
  {
    case SYS_OPRT_MODE_ONLINE:

      /* If there is a session active and the mode preference has
      ** GSM component, abort the session and return true
      */
      if(cmcall_misc_is_mode_pref(mode_pref, CM_MODE_PREF_GSM_WCDMA_ONLY))
      {
        cmpd_session_s_type  *pd_ptr        = cmpd_ptr(CMPD_SESSION_OBJ_MAIN);
        cmpd_session_s_type  *pd_dmod_ptr   = cmpd_ptr(CMPD_SESSION_OBJ_DMOD);
        boolean               session_ended = FALSE;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        if( pd_ptr->session_state != CMPD_SESSION_STATE_NONE &&
            pd_ptr->session_state != CMPD_SESSION_STATE_ABORTED )
        {
          cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                              pd_ptr->session_type,(dword)0 );
          session_ended = TRUE;
        }

        if( pd_dmod_ptr->session_state != CMPD_SESSION_STATE_NONE &&
            pd_dmod_ptr->session_state != CMPD_SESSION_STATE_ABORTED )
        {
          cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                              pd_dmod_ptr->session_type,(dword)0 );
          session_ended = TRUE;
        }

        if(session_ended)
        {
          /* We are changing the operating mode to GW, update the last
          ** pref sys changed command to reflect this
          */
          cmph_get_last_pref_sent(CM_SS_MAIN)->orig_mode_pref = mode_pref;
          return TRUE;
        }

      } /* Mode has GSM */

      return FALSE;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Currently there is no need to consider other oprt_modes
    */
    default:
      return FALSE;

  } /* switch */
} /* cmpd_process_oprt_mode_change */


/*===========================================================================

FUNCTION cmpd_call_start

DESCRIPTION
  This function is called to perform corresponding PD operations when
  a call (Incoming/Outgoing) start.

DEPENDENCIES
  None

RETURN VALUE
None


SIDE EFFECTS
  none

===========================================================================*/
void cmpd_call_start(

  cm_call_type_e_type      call_type,
    /* Call type - VOICE, SMS, E911, etc
    */
  cm_ss_e_type             ss,
    /* SS on which the call is attempted
    */
  sys_sys_mode_e_type      sys_mode,
    /* Sys mode of the call
    */
  cm_call_mode_info_e_type mode_info,
    /* Mode information of the call
    */
  cmcall_pd_status_e_type *pd_status_ptr
    /* Flag to indicate that the call should wait for confirmation
    ** for PD session end
    */
)
{
  cmpd_session_s_type *pd_ptr = cmpd_ptr( CMPD_SESSION_OBJ_MAIN);
  cmpd_session_s_type *pd_dmod_ptr = cmpd_ptr( CMPD_SESSION_OBJ_DMOD);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_ptr != NULL );
  CM_ASSERT( pd_dmod_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( pd_status_ptr != NULL )
  {
    *pd_status_ptr = CMCALL_PD_STATUS_NONE;
  }

  if( call_type == CM_CALL_TYPE_EMERGENCY )
  {

    if( pd_ptr->session_state != CMPD_SESSION_STATE_NONE &&
        pd_ptr->session_state != CMPD_SESSION_STATE_ABORTED )
    {
      cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                          pd_ptr->session_type,(dword)0 );

      if( pd_status_ptr != NULL )
      {
         *pd_status_ptr = CMCALL_PD_STATUS_HOLD_ORIG;
      }

    }

    if( pd_dmod_ptr->session_state != CMPD_SESSION_STATE_NONE &&
        pd_dmod_ptr->session_state != CMPD_SESSION_STATE_ABORTED )
    {
      cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                          pd_dmod_ptr->session_type, (dword)0 );

      if( pd_status_ptr != NULL )
      {
         *pd_status_ptr = CMCALL_PD_STATUS_HOLD_ORIG;
      }
    }
    return;

  }

  if( pd_ptr->session_state      != CMPD_SESSION_STATE_GRANTED &&
      pd_dmod_ptr->session_state != CMPD_SESSION_STATE_GRANTED )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the session needs to be aborted
  */

  /* Step 1: Check if the current call is on GSM, UMTS or APMS
  ** End the session if it is
  */

  switch( sys_mode )
  {
    case SYS_SYS_MODE_AMPS:
    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_GW:

      if( pd_ptr->session_state == CMPD_SESSION_STATE_GRANTED )
      {
        cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                            pd_ptr->session_type,
                            (dword)0 );
      }

      if( pd_dmod_ptr->session_state == CMPD_SESSION_STATE_GRANTED )
      {
        cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                            pd_dmod_ptr->session_type,
                            (dword)0 );
      }

      if( pd_status_ptr != NULL )
      {
         *pd_status_ptr = CMCALL_PD_STATUS_HOLD_ORIG;
      }
      return;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SYS_SYS_MODE_CDMA:

      /* If we are attempting a OOS call, kill the DMOD session
      */
      if( pd_dmod_ptr->session_state == CMPD_SESSION_STATE_GRANTED &&
          cmcall_check_if_no_srv(TRUE))
      {
        cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                            pd_dmod_ptr->session_type,
                              (dword)0 );
        if( pd_status_ptr != NULL )
        {
           *pd_status_ptr = CMCALL_PD_STATUS_HOLD_ORIG;
        }
        return;
      }
      return;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SYS_SYS_MODE_HDR:

      /* Depending on the session type abort the session
      */
      if( pd_dmod_ptr->session_state == CMPD_SESSION_STATE_GRANTED )
      {
        cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                            pd_dmod_ptr->session_type,
                            (dword)0 );
        if( pd_status_ptr != NULL )
        {
          *pd_status_ptr = CMCALL_PD_STATUS_HOLD_ORIG;
        }
        return;
      }

      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SYS_SYS_MODE_NO_SRV:

      /* This is a outgoing call, check the favoured mode pref
      ** abort the session, if required
      */
      switch( mode_info )
      {
        case CM_CALL_MODE_INFO_GW_CS:
        case CM_CALL_MODE_INFO_GW_PS:
          if( pd_ptr->session_state == CMPD_SESSION_STATE_GRANTED )
          {
            cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                                pd_ptr->session_type,
                                (dword)0 );
          }

          if( pd_dmod_ptr->session_state == CMPD_SESSION_STATE_GRANTED )
          {
            cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                                pd_dmod_ptr->session_type,
                                (dword)0 );
          }

          if( pd_status_ptr != NULL )
          {
            *pd_status_ptr = CMCALL_PD_STATUS_HOLD_ORIG;
          }
          return;
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case CM_CALL_MODE_INFO_CDMA:

          /* If we are attempting a OOS call, kill the DMOD session
          */
          if( pd_dmod_ptr->session_state == CMPD_SESSION_STATE_GRANTED &&
              cmcall_check_if_no_srv(TRUE))
          {
            cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                                pd_dmod_ptr->session_type,
                                (dword)0 );

            if( pd_status_ptr != NULL )
            {
              *pd_status_ptr = CMCALL_PD_STATUS_HOLD_ORIG;
            }
            return;
          }
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          if( call_type == CM_CALL_TYPE_PS_DATA )
          {
            cmss_s_type *ss_ptr = cmss_ptr();

            CM_ASSERT( ss_ptr != NULL );

            /* Check if the mode is set to HDR_ONLY
            ** and ss is main, then depending on the session type
            ** end the session if ss is HDR, then this is a DO call,
            ** if ss is main then, check if we are in HDR only mode
            */

            if( ss == CM_SS_HDR ||
                      ( !ss_ptr->info.hdr_hybrid &&
                         ss_ptr->info.sys_mode == SYS_SYS_MODE_HDR ) )
            {
              /* Depending on the session type abort the session
              */
              if( pd_dmod_ptr->session_state == CMPD_SESSION_STATE_GRANTED )
              {
                cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                                    pd_dmod_ptr->session_type,
                                    (dword)0  );

                if( pd_status_ptr != NULL )
                {
                  *pd_status_ptr = CMCALL_PD_STATUS_HOLD_ORIG;
                }
                return;
              }

            } /* if(ss == CMSS_HDR || !ss_ptr->info.hdr_hybrid )*/

          } /*  if( call_type == CM_CALL_TYPE_PS_DATA ) */

          return;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        default:

          /* For emergency calls, the info is set to none
          */

          cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                              pd_ptr->session_type,
                              (dword)0 );

          cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                              pd_dmod_ptr->session_type,
                              (dword)0 );

          if( pd_status_ptr != NULL )
          {
            *pd_status_ptr = CMCALL_PD_STATUS_HOLD_ORIG;
          }
          return;

       }/* switch( mode_info ) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      CM_ERR_1("sys_mode %d",sys_mode);
      /*
      ** 527: Unreachable
      ** This is because CM_ERR() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -save -e527 */


      cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                          pd_ptr->session_type,
                          (dword)0 );

      cmpd_session_abort( CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
                          pd_dmod_ptr->session_type,
                          (dword)0 );
      if( pd_status_ptr != NULL )
      {
         *pd_status_ptr = CMCALL_PD_STATUS_HOLD_ORIG;
      }
      return;
      /*lint -restore */
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  } /* switch( sys_mode ) */

}/* cmpd_call_start */

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
boolean cmpd_get_fix_status( void)
{
    return cmpd_fix.status;
}
#endif /* FEATURE_HDR_HYBRID */

#else /* FEATURE_GPSONE */

/*===========================================================================
FUNCTION cmxpd_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmxpd_dummy( void )
/*lint -esym(714,cmxpd_dummy) */
/*lint -esym(765,cmxpd_dummy)
** Can't be static as it would result in compiler warnings
*/
{
}

#endif /* FEATURE_GPSONE */
