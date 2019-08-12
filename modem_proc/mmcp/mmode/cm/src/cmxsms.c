/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R  C D M A  S M S  M O D U L E

GENERAL DESCRIPTION

  This module contains the Call Manager SMS Object.

EXTERNALIZED FUNCTIONS

  cmxsms_is_mo_sms_in_progress - Query the mo sms in progress status

  cmxsms_client_cmd_forward_mc - Forward a client SMS commands to 1X CP

  cmxsms_mo_msg_retry -  Retry mobile originated SMS message

  cmxsms_timer_proc - Process timer events.

  cmsms_mc_rpt_proc - Process MC reports

  cmxsms_mt_msg -Process mobile terminated sms message

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmsms_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1999 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmxsms.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/11   jh      Added feature for dynamic rat acq order change
01/06/12   gm      Data synchronization changes for freefloating platform
04/11/11   am      Cleaning stubs and runnning qtf SUTE
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
09/23/10   xs      Superlint critical/high error fixed
12/08/09   fj      Added lte_band_pref in cmph_force_pref_on_the_fly().
09/01/09   sv      Moving from clk API to time API
02/25/09   sv      Remove cmxll_v.h
02/20/09   rm      Lint clean post CMI changes
12/17/08   jd      Fixed Lint Errors
12/13/07   sk/ak   Added DCM.
08/07/07   ic      Lint clean up
04/16/07   jqi     Added cmxsms_dummy() to get rid of ADS 1.2 compiler
                   warning.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
06/02/06   pk      If a HDR call is in Origination and we get a MO SMS, don't
                   kill the HDR call.
04/19/06   pk      Added meta comments for 7200, and featurization
03/10/06   pk      Added support for high PSIST
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
11/17/05   pk      Added support to unblock data calls because of SMS waiting
                   for confirmation
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/25/05   pk      Fixed RVCT compiler warnings
09/12/05   pk      Modified calls to cmph_force_pref_on_the_fly
08/15/05   pk      Added support for priority Queues for origination
                   parameters
08/02/05   ic      Print value that caused error in default switch case
07/29/05   dk      Kill the HDR call only when it is in orig state
                   while making SMS.
03/24/05   dk      Fix CR 59885 from branch. Kill HDR call during MO SMS.
02/01/05   dk      Added WLAN Support.
01/24/05   ic      Replaced cmph_is_ph_in_emerg_cb_mode() with
                   cmph_is_in_emergency_cb()
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/03/04   ws      Add support for Remote WMS.
11/18/04   dk      Added cmxsms_abort_mo_msg() function.
11/17/04   dk      Fixed SMS cancelation during call orig. Don't cancel SMS.
11/15/04   ic      Lint cleanup with CM_DEBUG on
09/14/04   ws      Remove calls to srch_rude_wakeup.
08/20/04   sj      Added support for PRL Preference.
08/04/04   ic      CM Lint clean up - cmxsms.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmxsms_dummy() to get rid of ADS 1.2 compiler warning
07/15/04   sj      Changed pref_reason to ORIG_START_CS.
06/25/04   dk      Changed cmxcall_is_hdr_call() to cmcall_is_hdr_call().
                   Changed cmxcall_is_hybr_hdr_orig() to cmcall_is_hybr_hdr_orig.
                   Changed cmxcall_is_hybr_hdr_call() to cmcall_is_hybr_hdr_call.
06/21/04   ic      Replaced calls to cmxcall_is_there_a_hdrdata_call() by
                   calls to cmxcall_is_hdr_call()
06/16/04   ic      Removed throwing of CM_SMS_CMD_ERR_NOT_ALLOWED_IN_HDR_S
                   from cmsms_client_cmd_check()
                   Fixed obvious lint errors (-e715 - variable not referenced;
                   bad enum type used)
05/27/04   ic      In cmss_client_cmd_proc() call cmss_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
05/18/04   ws      Fix problem with hdrdata call check
05/11/04   ws      Do not check for overall call state in MO SMS
04/23/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.28 and MSMSHARED_CM.01.00.10.00.31
03/18/04   sj      Force preferences if only HDR calls are in progress.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
09/18/03   vt      Added functions to update DDTM status.
09/05/03   sj      Updated calls to cmph_force_pref_on_the_fly().
08/14/03   vt      Added DDTM.
04/28/03   AT      Lint cleanup.
02/18/03   vt      Fixed problems to build for HDR.
01/21/03   ws      Updated copyright information for 2003
10/30/02   vt      Fixed compiler warnings.
09/30/02   ws      Added call to srch_rude_wakeup in cmsms_mo_msg_start ()
08/06/02   HQ      Added FEATURE_CDSMS.
04/09/02   sh      Added 20-second delay to unforce the mode after receiving
                   the MO SMS status and added state timer for the wait for
                   status state.
02/06/02   sh      Added hybrid 3.0 support
12/14/01   ic      Fixed CR#19143
05/07/02   PRK     Included cmxsms.h instead of cmsms.h.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
08/21/01   ic      Main-lined FEATURE_NEWCM
05/29/01   RC      Eliminated compiler warning.
02/06/01   RC      Added support for System Determination 2.0
01/29/01   HQ      Properly feature-ized NEWCM and FEATURE_UASMS.
01/23/01   ejv     Mainlined T_SLEEP.
09/15/00   PJ      Changed SMS signal names to more generic DBM.
08/23/00   vt      Added new CM interface changes.
03/16/00   VT      Added BROADCAST SMS support
08/31/99   SH      Added FEATURE_UASMS flags to featurize UASMS
07/30/99   SH      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "comdef.h"    /* Definition for basic types and macros */
#include "cmcall.h"    /* Interface to CM call object */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmph.h"      /* Interface to CM phone object */
#include "cai.h"       /* Over the air messages definitions */
#include "cai_v.h"
#include "mc.h"        /* Interface to MC Task */
#include "mc_v.h"
#include "cmxsms.h"    /* Interface to cmsms.c*/
#include "time_svc.h"

#ifdef FEATURE_HDR_HYBRID
#include "cmxcall.h"
#endif

#ifdef CM_DEBUG
#error code not present
#endif

/*===========================================================================
=============================================================================
============================= INTERNAL FUNCTIONS ============================
=============================================================================
===========================================================================*/

#ifdef FEATURE_CDSMS
/*===========================================================================

FUNCTION cmsms_send_mo_msg

DESCRIPTION
  Send a mobile originated SMS message to MC
DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsms_send_mo_msg(

    const cmsms_s_type    *sms_ptr
      /* pointer to SMS object */
)
{
  mc_msg_type    *mc_ptr  = cm_mc_get_buf_else_err_fatal();
      /* allocate a MC command buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_ptr != NULL );
  CM_ASSERT( mc_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Compose SMS message  */
  mc_ptr->hdr.cmd = MC_MO_DBM_F;

  mc_ptr->mo_dbm.burst_type   = CAI_SHORT_MSG_SERVICES;
  mc_ptr->mo_dbm.ack_req      = sms_ptr->l2_ack_requested;
  mc_ptr->mo_dbm.dbm_buf_ptr  = (void *)sms_ptr->data;

  /* Send command to MC.*/
  cm_mc_send_cmd( mc_ptr );
} /* cmsms_send_mo_msg */


/*===========================================================================

FUNCTION cmsms_check_mo_status_timer

DESCRIPTION
  Check the status of the wait for status timer if it the timer
  expired then end the MO SMS


DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsms_check_mo_status_timer( void)
{

  cmsms_s_type            *sms_ptr = cmsms_ptr();
      /* Pointer for sms object */

  dword                   uptime;
      /* get the current uptime */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SMS state is not wait for status, return now.
  */
  if (sms_ptr->sms_state != CM_SMS_STATE_WAIT_FOR_STATUS)
  {
     return;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, SMS state is CM_SMS_STATE_WAIT_FOR_STATUS check
     the wait for status timer is expired or not .
  */

  uptime=time_get_uptime_secs();

  if (uptime > sms_ptr->max_wait_for_status_time)
  {
    /* Do necessary phone's MO SMS end activities.
    */
    cmph_mo_sms_end( (dword) 0, (cm_act_id_type)sms_ptr);

    sms_ptr->mo_status     = CM_SMS_MO_L2_ACK_FAILURE;
    sms_ptr->sms_state     = CM_SMS_STATE_IDLE;
    cmph_update_ddtm_status( cmph_ptr() );        /* update the DDTM satus */
    cmsms_event( sms_ptr, CM_SMS_EVENT_MO_STATUS );
    CM_MSG_HIGH_0("MO SMS-WaitForStatus timeout");
    return;
  }
} /* cmsms_check_mo_status_timer */

#endif /* FEATURE_CDSMS */


/*===========================================================================
=============================================================================
=========================== EXTERNALIZED FUNCTIONS ==========================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION cmxsms_is_mo_sms_in_progress

DESCRIPTION
 Query the mo sms in progress status

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cmxsms_is_mo_sms_in_progress ( void )
{

  cmsms_s_type       *sms_ptr     = cmsms_ptr();
       /* Point at sms object */

  boolean            status= FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_CDSMS
  /* If no DBM is pending then return
  */
  if (sms_ptr->sms_state != CM_SMS_STATE_IDLE)
  {
     status = TRUE;
  }
  #endif /* FEATURE_CDSMS */

  return status;
} /* cmxsms_is_mo_sms_in_progress() */


/*===========================================================================

FUNCTION cmxsms_client_cmd_forward_mc

DESCRIPTION
  Forward a client SMS commands to 1X CP

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  TRUE if CM needs to wait for a replay from the protocal statck before
  continuing processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmxsms_client_cmd_forward_mc(

    const cmsms_s_type  *sms_ptr,
        /* Pointer to a sms object */

    const cm_sms_cmd_s_type *sms_cmd_ptr
        /* Pointer to a CM command */
)
  /*lint -esym(715, sms_ptr, cmd_ptr) */
  /* NOTE -save/-restore do not work with -esym()
  */

{

  const cm_sms_cmd_info_s_type *cmd_info_ptr = NULL;
      /* Point at command information component */

  mc_msg_type              *mc_ptr;
      /* MC command pointer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_cmd_ptr != NULL );
  CM_ASSERT( sms_ptr != NULL );
  CM_ASSERT( sms_cmd_ptr->cmd_type == CM_CMD_TYPE_SMS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr = CMD_INFO_PTR( sms_cmd_ptr );

  switch( sms_cmd_ptr->cmd )
  {
                     /*-------------------------*/
                     /*    Send a MO SMS        */
                     /*-------------------------*/

    case CM_SMS_CMD_MO_MSG:

       /* MO SMS is more elaborated process - we take care of it
      ** in the MC reply function.
      */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*-------------------------*/
                     /*    Alert with Info ACK  */
                     /*-------------------------*/

    case CM_SMS_CMD_AWI_ACK:

      /* Get MC command buffer and indicate the
      ** appropriate MC command.
      */
      mc_ptr = cm_mc_get_buf_else_err_fatal();

      mc_ptr->hdr.cmd               = MC_AWI_SMS_ACK_F;
      mc_ptr->awi_ack.bf            = cmd_info_ptr->awi_ack.bf;
      mc_ptr->awi_ack.error_class   = cmd_info_ptr->awi_ack.error_class;
      mc_ptr->awi_ack.status        = cmd_info_ptr->awi_ack.status;
      mc_ptr->awi_ack.seq_no        = cmd_info_ptr->awi_ack.seq_no;

      /* Send command to MC.
      */
      cm_mc_send_cmd( mc_ptr );

      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                      /*-------------------------*/
                      /* Enable broadcast SMS    */
                      /*-------------------------*/

    case CM_SMS_CMD_BC_ENABLE:

      /* Get MC command buffer and indicate the
      ** appropriate MC command.
      */
      mc_ptr = cm_mc_get_buf_else_err_fatal();

      mc_ptr->hdr.cmd               = MC_BCSMS_ENABLE_F;
      mc_ptr->bcsms_enable.enable   = cmd_info_ptr->bc_enable;

      /* Send command to MC.
      */
      cm_mc_send_cmd( mc_ptr );

      break;

    default:

      CM_ERR_1( "SMS cmd %d",(uint8) sms_cmd_ptr->cmd);


  } /* switch( sms_cmd_ptr->cmd ) */

  return FALSE;

}
/*lint +esym(715, sms_ptr, cmd_ptr) */
/* NOTE -save/-restore do not work with -esym()
*/



#ifdef FEATURE_CDSMS
/*===========================================================================

FUNCTION cmxsms_mo_msg_retry

DESCRIPTION
  Retry mobile originated SMS message

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxsms_mo_msg_retry( void)
{

  cmsms_s_type            *sms_ptr = cmsms_ptr();
      /* Pointer for sms object */

  cmss_s_type               *ss_ptr     = cmss_ptr();
      /* Pointer for ss object */

  dword                     uptime;
      /* get the current uptime */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SMS state is not ORIG_RETRY, return now.
  */
  if (sms_ptr->sms_state != CM_SMS_STATE_HOLD_ORIG_RETRY)
  {
     return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, SMS state is ORIG_RETRY. Check whether MO SMS failed.
  */

  if (ss_ptr->info.sys_mode == SYS_SYS_MODE_NO_SRV)
  {
     sms_ptr->mo_status =  CM_SMS_MO_NO_SRV;
  }
  else if (ss_ptr->info.sys_mode == SYS_SYS_MODE_AMPS)
  {
     sms_ptr->mo_status = CM_SMS_MO_NOT_ALLOWED_IN_AMPS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If MO SMS failed (i.e., MO status is other than none), end the
  ** origination process and inform CM clients of MO SMS failure.
  */
  if( sms_ptr->mo_status != CM_SMS_MO_NONE )
  {
     /* Do necessary phone's MO SMS end activities.
     */
     cmph_mo_sms_end( (dword) 0, (cm_act_id_type)sms_ptr );

     sms_ptr->sms_state = CM_SMS_STATE_IDLE;
     cmph_update_ddtm_status( cmph_ptr() );       /* update the DDTM satus */
     cmsms_event( sms_ptr, CM_SMS_EVENT_MO_STATUS );
     return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DEDICATED_C2K_MODE
  if( ss_ptr->is_hdr_session_open )
  {
    CM_MSG_HIGH_0( "DCM: SMS(DBM) is waiting for HDR session close!");
    return;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the SMS origination process is not over yet. If CP is
  ** now ready to process the SMS, send it now. Else keep waiting.
  */

  if( sms_ptr->is_ok_to_send )
  {
    cmsms_send_mo_msg(sms_ptr);
    sms_ptr->sms_state      = CM_SMS_STATE_WAIT_FOR_STATUS;
    sms_ptr->is_ok_to_send  = FALSE;
    sms_ptr->max_wait_for_status_time =
        time_get_uptime_secs() + MAX_SMS_WAIT_FOR_STATUS_TIME;
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, SMS is not sent yet. If we waited for more than
  ** hold_orig max time, end the origination and inform CM clients of MO SMS
  ** failure.
  */
  uptime=time_get_uptime_secs();

  if (uptime > sms_ptr->max_hold_orig_time)
  {
    CM_MSG_MED_2("SMS not sent hence end it uptime:%d sms_retry_period:%d",uptime,sms_ptr->max_hold_orig_time);
    /* Do necessary phone's MO SMS end activities.
    */
    cmph_mo_sms_end( (dword) 0, (cm_act_id_type)sms_ptr);

    sms_ptr->mo_status     = CM_SMS_MO_HOLD_ORIG_RETRY_TIMEOUT;
    sms_ptr->sms_state     = CM_SMS_STATE_IDLE;
    cmph_update_ddtm_status( cmph_ptr() );        /* update the DDTM satus */
    cmsms_event( sms_ptr, CM_SMS_EVENT_MO_STATUS );
    return;
  }
}/* cmxsms_mo_msg_retry */


/*===========================================================================

FUNCTION cmxsms_mo_msg_start

DESCRIPTION
  Kick-start the MO SMS process.

DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxsms_mo_msg_start(

    cmsms_s_type  *sms_ptr
        /* pointer to a sms object */
)
{
  dword                       uptime        = time_get_uptime_secs();
  /* Get the current uptime */

  cm_overall_call_state_e_type overall_call_state;
    /* Overall call state */

  cmss_s_type               *ss_ptr     = cmss_ptr();
    /* Pointer for ss object */

  cm_mode_pref_e_type mode_pref             = CM_MODE_PREF_NO_CHANGE;

  cmph_s_type                 *ph_ptr       = cmph_ptr();

  boolean                     waiting_for_hdr_session_close = FALSE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(sms_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* initialize max allowed hold orig retry time */
  sms_ptr->max_hold_orig_time    = uptime + MAX_SMS_HOLD_ORIG_TIME;

  /* set mo status to none */
  sms_ptr->mo_status                           = CM_SMS_MO_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(ss_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DEDICATED_C2K_MODE
  if( ss_ptr->is_hdr_session_open )
  {
    CM_MSG_HIGH_0("DCM: Closing HDR session for SMS(DBM)");
    cmph_send_hdrmc_session_close_cmd();

    cmxcall_end_call( cmcall_is_hdr_call(),
                      CM_CALL_END_HDR_RELEASED_BY_CM,
                      CAI_REL_NORMAL );

    waiting_for_hdr_session_close = TRUE;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  overall_call_state = cmcall_overall_call_state_per_ss(CM_SS_MAIN);
  CM_MSG_MED_2("Overall_call_state %d s_hybr_hdr_call %d", overall_call_state, cmcall_is_hybr_hdr_call(NULL));

  /* If mobile is not engaged in call related activity, force the origination
  ** preference now and wait for the OK_TO_ORIG response before sending the
  ** MO SMS. Else, send the MO SMS now.
  */
  if(( overall_call_state == CM_OVERALL_CALL_STATE_IDLE
     #ifdef FEATURE_HDR_HYBRID
     || (cmcall_is_hybr_hdr_orig() !=  NULL)
     || cmcall_is_hybr_hdr_call(NULL)
     #endif
    ) && (ss_ptr->info.sys_mode != SYS_SYS_MODE_LTE))
  {

    #ifdef FEATURE_HDR_HYBRID
    mode_pref = cmph_select_dbm_mode_pref();
    #endif
    CM_MSG_HIGH_0("Forcing preferences for SMS to SD");
    (void) cmph_force_pref_on_the_fly(CM_SS_MAIN,
                                      ph_ptr,
                                      SD_SS_PREF_REAS_ORIG_START_CS,
                                      CM_ACT_TYPE_SMS_CALL,
                                      CM_PREF_TERM_CM_MO_SMS,
                                      mode_pref,
                                      CM_BAND_PREF_NO_CHANGE,
                                      SYS_LTE_BAND_MASK_CONST_NO_CHG,
                                      CM_BAND_PREF_NO_CHANGE,
                                      CM_PRL_PREF_NO_CHANGE,
                                      CM_ROAM_PREF_NO_CHANGE,
                                      CM_HYBR_PREF_NO_CHANGE,
                                      ph_ptr->main_stack_info.pref_info.plmn,
                                      CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                      CM_OTASP_ACT_CODE_NONE, 
                                      ph_ptr->main_stack_info.pref_info.acq_order_pref,
                                      CM_NETWORK_SEL_MODE_PREF_NONE,
                                      (cm_act_id_type) sms_ptr,
                                      CM_ACT_UPDATE_REAS_ACT_START,
                                      TRUE,
                                      cmph_get_acq_pri_order_per_sub(CM_SS_MAIN),
                                      CM_DEFAULT_VALUE,
                                      ph_ptr->main_stack_info.pref_info.csg_id,
                                      ph_ptr->main_stack_info.pref_info.csg_rat);

    sms_ptr->sms_state = CM_SMS_STATE_HOLD_ORIG_RETRY;

    /* Wait for ok_to_send indication from CP before sending the MO SMS.
    */
    sms_ptr->is_ok_to_send = FALSE;
  }
  else
  /*lint -e{774} boolean always evaluates to True. */
  if( !waiting_for_hdr_session_close )
  {
    cmsms_send_mo_msg(sms_ptr);
    sms_ptr->sms_state = CM_SMS_STATE_WAIT_FOR_STATUS;
    sms_ptr->max_wait_for_status_time =
        uptime + MAX_SMS_WAIT_FOR_STATUS_TIME;
  }

  cmph_update_ddtm_status( cmph_ptr() );        /* update the DDTM satus */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify client list of MO SMS event.
  */
  cmsms_event( sms_ptr, CM_SMS_EVENT_MO_MSG );

} /* cmxsms_mo_msg_start */


/*===========================================================================

FUNCTION cmxsms_timer_proc

DESCRIPTION
  Process timer events.

DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxsms_timer_proc(

    cm_timer_event_type    timer_event
        /* Indicate specific timer event.
        **
        ** Note that currently timer event is not being used
        */
)
/*lint -esym(715,timer_event) */
{
  cmss_s_type               *ss_ptr     = cmss_ptr();
      /* Pointer for ss object */

  cmsms_s_type            *sms_ptr = cmsms_ptr();

  dword                 uptime  = time_get_uptime_secs() ;

  if((sms_ptr->hold_orig_time_s102 <= uptime) &&
     (sms_ptr->hold_orig_time_s102 < sms_ptr->max_hold_orig_time) &&
     (ss_ptr->info.sys_mode == SYS_SYS_MODE_LTE) &&
     (sms_ptr->sms_state == CM_SMS_STATE_HOLD_ORIG_RETRY))
  {
    sms_ptr->is_ok_to_send = TRUE;
    sms_ptr->hold_orig_time_s102 = CMCALL_UPTIME_NONE;
  }

  /* check whether need to process MO SMS or not */
  cmxsms_mo_msg_retry();
  cmsms_check_mo_status_timer();
}
/*lint +esym(715,timer_event) */

#endif /* FEATURE_CDSMS */


/*===========================================================================

FUNCTION cmsms_mc_rpt_proc

DESCRIPTION
  Process MC reports (i.e. notifications of phone activity or
  replies to clients phone commands).


DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsms_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
)
{
  /*lint -e{826} keep it suppressed for general / specific walks as well */
  const cm_mc_rpt_type    *cm_mc_rpt_ptr = (cm_mc_rpt_type *) rpt_ptr;
    /* Pointer to MC report */

  cmss_s_type               *ss_ptr     = cmss_ptr();
  /* Pointer for ss object */

  dword                     uptime  = time_get_uptime_secs() ;

  #ifndef FEATURE_MMODE_REMOVE_1X
  cmsms_s_type            *sms_ptr       = cmsms_ptr();
      /* Point at sms object */
  cmcall_s_type       *call_ptr;
      /* Point at call object */
  cm_iterator_type    call_itr;
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_mc_rpt_ptr != NULL );
  CM_ASSERT( sms_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do command specific processing as follows:
  ** 1. Updating appropriate sms object fields.
  ** 2. Notifying clients of event if necessary.
  */

  switch( cm_mc_rpt_ptr->hdr.cmd )
  {
   #ifdef FEATURE_CDSMS

    /* MO SMS status
    */

    case CM_MO_DBM_STATUS_F:
      #ifndef FEATURE_MMODE_REMOVE_1X
      CM_MSG_HIGH_3("SMS_STATUS: msg_type:%d, state_:%d, status;%d",cm_mc_rpt_ptr->mo_dbm_status.msg_type,
	                                                      sms_ptr->sms_state ,
	                                                      cm_mc_rpt_ptr->mo_dbm_status.status);
      if(cm_mc_rpt_ptr->mo_dbm_status.msg_type == CAI_SHORT_MSG_SERVICES)
      {
        if (sms_ptr->sms_state == CM_SMS_STATE_WAIT_FOR_STATUS)
        {
          if(cm_mc_rpt_ptr->mo_dbm_status.status == CM_DBM_MO_HOLD_ORIG)
          {
            sms_ptr->sms_state = CM_SMS_STATE_HOLD_ORIG_RETRY;

            /* Reset the ok_to_send indication */
            sms_ptr->is_ok_to_send  = FALSE;

            if (ss_ptr->info.sys_mode == SYS_SYS_MODE_LTE)
            {
              sms_ptr->hold_orig_time_s102 = uptime + CMCALL_SR_PERIOD_TIME;
            }
            /* try to re-send MO SMS */
            cmxsms_mo_msg_retry();
          }
          else
          {

            /* Do necessary phone's MO SMS end activities */
            cmph_mo_sms_end( CMSMS_HOLD_MODE_PREF_TIME, (cm_act_id_type)sms_ptr );

            sms_ptr->sms_state = CM_SMS_STATE_IDLE;

            /*---------------------------------
              Since we have mapped the
              cm_sms_mo_status_e_type to the same
              values as the dbm_status_e_type,
              a cast will suffice. (See def'n of
              cm_sms_mo_status_e_type).
             *------------------------------ */

            sms_ptr->mo_status =
              (cm_sms_mo_status_e_type) (cm_mc_rpt_ptr->mo_dbm_status.status);
            cmph_update_ddtm_status( cmph_ptr() );/* update the DDTM satus */
            cmsms_event( sms_ptr, CM_SMS_EVENT_MO_STATUS );

            cmcall_obj_iterator_init(&call_itr);

            call_ptr = cmcall_obj_get_next(&call_itr);
            
            while(call_ptr != NULL)
            {
              if( ( call_ptr->sms_status == CMCALL_SMS_STATUS_HOLD_ORIG ) &&
                  ( call_ptr->call_state == CM_CALL_STATE_ORIG ) )
              {
                call_ptr->sms_status = CMCALL_SMS_STATUS_NONE;
                cmcall_orig_proc( call_ptr );
              }
              call_ptr = cmcall_obj_get_next(&call_itr);
            }
          }
        }
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /* Else, this command is an error.
        */
        else { CM_ERR_0( "CM_MO_DBM_STATUS_F" " else make no sense" ); }
      }
      #endif /*FEATURE_MMODE_REMOVE_1X */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #else /* FEATURE_CDSMS */

    case CM_MSG_WAITING_F:
      sms_ptr->msgs = cm_mc_rpt_ptr->msgs.msgs;
      cmsms_event( sms_ptr, CM_SMS_EVENT_MSG_WAITING );
      break;

    case CM_N_VOICE_F:
      #ifdef FEATURE_CM_DEPRECATE_CANDIDATE
      
      CM_MSG_HIGH_3( "Deprecated: cmsms_mc_rpt_proc():CM_N_VOICE_F SIZE: sizeof(sms_ptr->data) = %d, sizeof(cm_sms_msg_type) = %d", 
                    sizeof(sms_ptr->data), sizeof(cm_sms_msg_type) );
      memscpy( sms_ptr->data, sizeof(cm_sms_msg_type), cm_mc_rpt_ptr->n_voice.data, 14 );
      cmsms_event( sms_ptr, CM_SMS_EVENT_N_VOICE );
      break;
      #endif

    #endif /* FEATURE_CDSMS */
    default:
      break;
  }
}/* cmsms_mc_rpt_proc */


/*===========================================================================

FUNCTION cmsms_sd_rpt_proc

DESCRIPTION
  Process SD reports


DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsms_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
)
{
  /*lint -e{826} keep it suppressed for general / specific walks as well */
  const cm_sd_rpt_u_type         *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
    /* Pointer to SD report */

  cmsms_s_type             *sms_ptr    = cmsms_ptr();
      /* Point at sms object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sd_rpt_ptr != NULL );
  CM_ASSERT( sms_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do command specific processing as follows:
  ** 1. Updating appropriate sms object fields.
  ** 2. Notifying clients of event if necessary.
  */

  switch( sd_rpt_ptr->hdr.cmd )
  {
    #ifdef FEATURE_CDSMS
    case CM_OK_TO_ORIG_F:

      /* check for ok to orig mode is valid
      */
      if( sd_rpt_ptr->ok_to_orig.ss == SD_SS_MAIN &&
          sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode == SD_MODE_CDMA )
      {
        /* Send MO SMS as necessary.
        */
        sms_ptr->is_ok_to_send = TRUE;
        cmxsms_mo_msg_retry();
      }
      break;
    #endif /* FEATURE_CDSMS */

    default:
      break;
  } /* switch( sd_rpt_ptr->hdr.cmd ) */
} /* cmsms_sd_rpt_proc() */


/*===========================================================================

FUNCTION cmxsms_mt_msg

DESCRIPTION
 Process mobile terminated sms message

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmxsms_mt_msg (

    uint8      format,
         /* MT SMS message format */

    uint16     len,
         /* msg length */

    const uint8 *data
          /* msg buffer pointer */
)
{
    cmsms_s_type                    *sms_ptr     = cmsms_ptr();

    cm_mt_sms_msg_type              mt_sms;
       /* Point at sms object */

    mt_sms.sms.format = (cm_format_e_type)format;

    mt_sms.sms.data_len = memscpy( mt_sms.sms.data,
                                   sizeof(mt_sms.sms.data),
                                   data, len );

    mt_sms.sys_mode = SYS_SYS_MODE_CDMA;

    sms_ptr->mt_sms_ptr = &mt_sms;

    cmsms_event(sms_ptr,CM_SMS_EVENT_MT_MSG);
}/* cmxsms_mt_msg */


/*===========================================================================

FUNCTION cmxsms_abort_mo_msg

DESCRIPTION
  Abort MO SMS.

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxsms_abort_mo_msg( void)
{
  cmsms_s_type            *sms_ptr = cmsms_ptr();
      /* Pointer for sms object */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(sms_ptr != NULL);

  #ifdef FEATURE_CDSMS
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sms_ptr->sms_state == CM_SMS_STATE_IDLE)
  {
    return;
  }

  sms_ptr->mo_status =  CM_SMS_MO_ABORT;
  #endif /* FEATURE_CDSMS */

  /* Do necessary phone's MO SMS end activities.
  */
  cmph_mo_sms_end(  (dword) 0, (cm_act_id_type)sms_ptr );

  #ifdef FEATURE_CDSMS
  sms_ptr->sms_state = CM_SMS_STATE_IDLE;
  #endif /* FEATURE_CDSMS */

  #ifdef FEATURE_DDTM_CNTL
  cmph_update_ddtm_status( cmph_ptr() );       /* update the DDTM satus */
  #endif
  cmsms_event( sms_ptr, CM_SMS_EVENT_MO_STATUS );
  return;
}

#else /* if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

/*===========================================================================
FUNCTION cmxsms_dummy

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
void cmxsms_dummy( void )
/*lint -esym(714,cmxsms_dummy) */
/*lint -esym(765,cmxsms_dummy)
** Can't be static as it would result in compiler warnings
*/
{
}

#endif /* if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */
