/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    S E R V I CE   DOMAIN   SELECTION   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager service domain selection Object.

  The  service domain selection Object is responsible for:
  1. Activating/Deactivating 1XCSFB.
  2. Receiving the info related to CSFB from 1XCP and NAs and processing
     the info.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmsds_init() must be called to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1998 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmsds.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/14   xs      hVOLTE SSAC support
03/20/14   xs      Revert 612487 to FR15628 behavior
03/13/14   sm      1xSRVCC support
03/06/14   xs      Per RAT SMS domain sel
02/19/14   xs      Always return TRUE vops in query API when volte call ongoing
12/30/13   xs      Add debug prints
10/25/13   xs      Remove callback checkin in VOLTE call obj detect since IMS
                   Won't register cb for MT call.
10/24/13   xs      Hold VOPS report in query API
09/25/13   xs      Add support to send voice pref on legacy RATs
08/19/13   fj        Add support for DSDX L+G
09/25/13   xs      Hold VOPS indication when moving from VOLTE area to non-VOLTE area
08/19/13   fj        Add support for DSDX L+G
10/05/13   xs      Skip SMS domain sel when LTE is not in full service
08/19/13   fj      Add support for DSDX L+G
08/16/13   xs      Perform SMS domain selection when full srv
08/14/13   sm      Reset VOPS and SMS reg states upon going OOS in hybr1/hybr2
08/08/13   xs      Consider to use hybrid stack for HDR voice/sms pref notification
07/17/13   xs      Take ext IMS emerg into consider of emergnecy call active checking
06/24/13   cl      Ignore domain selection logic in SGLTE mode
06/04/13   xs      Domain selection support for 1x and TDSCDMA
06/17/13   xs      Don't skip sms domain pref sending in SVLTE
03/06/13   qf      FR3255 - Get T3402 value through message router
03/03/13   xs      FR 2972: Enhance QMI to support domain selection preference status
                   updates
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
11/13/12   aj      e911 domain selection
08/17/12   gm      Mainline EFS feature - Remove FEATURE_EFS
05/31/12   vk      FIX to remove LTE from emergency scan list for e9111 call
05/25/12   vk      FIX to set default value of lte_disable_duration to T3402.
05/17/12   vk      FIX to reset CSFB_STATUS after getting DEACT_CNF from 1XCP.
05/16/12   vk      FIX to ignore the redundant CSFB_STATUS messages from 1XCP.
05/14/12   vk      FIX to not disable LTE while processing the buffred LTE
                   SRV IND during LTE to 1X IRAT
05/10/12   vk      FIX to disbale LTE, if CSFB status is negative for CS_PREF
                   voice config.
05/07/12   vk      FIX to not set selected voice domain to NO_VOICE
04/10/12   sk      Fixing the CW and KW warnings
03/23/12   gm      DIME integration fix: TMC deprecation and others.
03/15/12   vk      Addition of domain selection info in CMSS.
03/13/12   vk      FIX to not reset CSFB status during LTE to 1X transition
03/02/12   vk      Provided CSFB support to TDSCDMA
02/23/12   vk      Provided config request support to CMSDS
01/25/12   vk      CMSDS should perform LTE disbale for CS_PREF configuration
01/10/12   gm      Mainlining HDR API change integration
12/20/11   vk      Domain selection related fixes.
12/01/11   vk      PS voice domain selection migration from CNE to CM
11/25/11   vk      Start LTE disable timer only if mode pref contains CDMA.
11/15/11   vk      Setting of default value for voice_domain_pref
11/02/11   gm      Changes for HDR API cleanup
11/01/11   rk      Fix to do CSFB deactivation before disabling LTE
10/31/11   rk      Use CMSS info instead of SD info for Domain selection algorithm
10/08/11   rk      Fix to error case of EFS file reading
09/29/11   xs      Fix HDR tunnel early activate issue
09/28/11   vk      Multimode CSFB activation changes
09/22/11   rk      Handle SYS_LTE_CS_CAPABILITY_LIMITED
09/15/11   vk      New MMOC Transaction "MMOC_TRANS_DEACT_1XCSFB_PROT_CMD" related changes.
09/14/11   rk      NV items N_min_MO_call_soft_retry and N_maxSIB8
09/02/11   rk      S102 less 1XCSFB
08/25/11   xs      Added EPS network feature support definition (PS voice indication)
07/12/11   xs      HDR optimize handoff enhancement
07/05/11   rk      Support for sending voice domain preference to NAS
06/18/11   rk      Fix to reset onexcsfb_status_not_avail_uptime on LTE deact
06/08/11   rk      Correcting the featurization for XCSFB status processing
05/20/11   xs      Add support for LTE->eHRPD optimized handoff
05/19/11   rk      Handling SIB8 failure
05/03/11   rk      Add operating mode check for 1XCSFB activation
04/11/11   am      Cleaning stubs and runnning qtf SUTE
04/08/11   rk      Fix for LTE disable duration 0 issue
04/06/11   rk      Fixing debug statements
04/05/11   rk      Fix lint issues
04/05/11   rk      Fix lint issues
04/05/11   rk      Handling 1XCSFB registation failures
03/31/11   rk      Fix not to do voice domain selection if device is
                   configured as SMS only
03/30/11   rk      Fix not to disable LTE if it is already disabled
03/29/11   rk      Fix for not doing PPCSFB after receiving AttachAccept with
                   SMSOnly set
03/21/11   rk      Fix not to leave LTE if sim state is CS invalid
03/03/11   rk      Fixing lint issues
03/03/11   rk      Fixing featurization issue.
03/02/11   rk      Domain selection phase 2 requirements
02/28/11   rk      Adding support for LTE UE mode preference
02/22/11   rk      SMS only registation
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
12/22/10   rk      Handle CM_LTE_1X_IRAT_F report
12/16/10   am      Including MMODE unit test interceptors
12/10/10   rk      Fixing 1XCSFB MO call issue
10/22/10   rk      Sucessful MO call initial support
10/05/10   rk      Merging 1XCSFB pre-registration code
08/22/10   rk      Removing the SIB8 check for activating 1XCP in CSFB mode
07/31/10   rk      1XCSFB pre-registation initial changes.

===========================================================================*/


/****************************************************************************

 Organization of the file:

    The file is divided into multiple sections.
    You can jump from one sections to the other by searching for / followed
    by 2 *'s. The order of objects defined is as follows:

        includes
        forward declarations
        #defines
        enums
        macros

        functions
        - Internal [Common, 1x only, GW only]
        - External [Common, 1x only, GW only]

****************************************************************************/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/


#include "mmcp_variation.h"
#include "customer.h"
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "sd_v.h"
#include "cmi.h"
#include "cmss.h"      /* Interface to CM Serving System Object */
#include "cmph.h"      /* Interface to Phone Object */
#include "cmaccessctrl.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"       /* System wide definitions */
#include "sys_v.h"
#include "cmtaski.h"   /* Interface to timer definitions */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmlog.h"     /* Interface for Diagnostic Logging */
#include "cmlog_v.h"
#include "cmph.h"      /* Interface to CM phone */
#include "cmaccessctrl.h"
#include "cmsds.h"     /* Interface to CM Serving System Object */
#include "time_svc.h"  /* Interface to clock services */
#include "cmreply.h"
#include "rex.h"       /* Interface to rex services */
#include "cmefs.h"
#include "mmoc.h"
#include "mmoc_v.h"
#include "cmregprxi.h"
#include "cmcall.h"
#include "cmcsfbcall.h"
#include "cmsoa.h"
#include "cmmsc_auto.h"
#include "cmclnup.h"

#ifdef FEATURE_DOMAIN_SELECTION
#include "cm_msgr_msg.h"
#endif
#if defined (FEATURE_LTE ) && defined (FEATURE_DOMAIN_SELECTION)
#include "mm.h"
#endif
#ifdef FEATURE_IP_CALL
#include "esm.h"
#endif
#ifdef CM_DEBUG
#error code not present
#endif
#include "cmemgext.h"
#include "cmmsc.h"
#include "cmsimcoord.h"
#include "policyman.h"
#include "cmipappi.h"
#include "cmpmprx.h"

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/



/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/



/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/

/*===========================================================================

@FUNCTION cmsds_is_ims_active

@DESCRIPTION
Checks whether IMS protocol is currently active.

@DEPENDENCIES

None

@RETURNS
TRUE/FALSE

@SIDE_EFFECT

None

===========================================================================*/
static boolean cmsds_is_ims_active
(
  sys_modem_as_id_e_type sub_id
)
{
  cmsds_s_type  *sds_ptr  = cmsds_ptr();

  if (((sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_LTE] == SYS_IMS_REG_STATE_NEEDS_REG) ||
       (sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_LTE] == SYS_IMS_REG_STATE_REG_IN_PROGRESS) ||
       (sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_LTE] == SYS_IMS_REG_STATE_REGISTERED)) &&
      (cmsds_get_config_ptr(sub_id)->is_ims_enabled))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/*===========================================================================

@FUNCTION cmsds_set_ims_voice_reg_status

@DESCRIPTION
set IMS voice registration status

@DEPENDENCIES

None

@RETURNS
TRUE/FALSE

@SIDE_EFFECT

None

===========================================================================*/

static void cmsds_set_ims_voice_reg_status
(
  sys_ims_reg_state_e_type  reg_status,

  sys_sys_mode_e_type       sys_mode
)
{
  CM_ASSERT(sys_mode != SYS_SYS_MODE_NONE);
  if(SYS_IMS_REG_STATE_UNKNOWN != reg_status)
  {
    CM_MSG_HIGH_2( "IMS voice reg_status %d on mode %d",reg_status, sys_mode);
  }
  cmsds_ptr()->ims_reg_status_voice[sys_mode] = reg_status;
  return;
}

/*===========================================================================

FUNCTION:cmsds_reset_ims_reg_and_voice_avail_status

DESCRIPTION
Reset IMS VOICE and SMS reg status .
Also, reset ims voice avail status.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmsds_reset_ims_reg_and_voice_avail_status
(
  void
)
{
  CM_MSG_MED_0( "Reset IMS SMS/VOICE reg status and lte_voice_avail");
  cmsds_ptr()->ims_reg_status_sms[SYS_SYS_MODE_LTE] = SYS_IMS_REG_STATE_UNKNOWN;
  cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_UNKNOWN, SYS_SYS_MODE_LTE);
  cmsds_ptr()->lte_ims_voice_avail = CMSDS_IMS_VOICE_NONE;
  return;
}

static void cmsds_send_tau_complete(uint8 inst_id)
{
  cm_tau_complete_ind_s_type   tau_complete_ind;
  errno_enum_type msgr_error = E_SUCCESS;

  memset(&tau_complete_ind, 0, sizeof(cm_tau_complete_ind_s_type));
  tau_complete_ind.msg_hdr.inst_id = inst_id;
  msgr_error = cm_msgr_send( MM_CM_TAU_COMPLETE_IND, MSGR_MM_CM,
                     (msgr_hdr_s*)&tau_complete_ind, sizeof(cm_tau_complete_ind_s_type) );

  CM_MSG_HIGH_1( "Sending TAU COMPLTE IND %d", msgr_error);

}


#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)

/*===========================================================================

@FUNCTION cmsds_send_1xsrvcc_activate_req

@DESCRIPTION
  Send SRVCC activate request  to 1xCP.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
static void cmsds_send_1xsrvcc_activate_req
(
  void
)
{
  #ifndef FEATURE_MMODE_REMOVE_1X
  mc_msg_type                    *mc_ptr = NULL;
    /* MC command pointer */
  cmsds_s_type                            *sds_ptr               = cmsds_ptr();


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(!cmph_ptr()->cdma_srvcc_enabled)
 {
   CM_MSG_HIGH_0( "Domsel:1XSRVCC not enabeld from UE");
   return;
 }


  mc_ptr                                = cm_mc_get_buf_else_err_fatal();

  CM_ASSERT( mc_ptr != NULL );
  CM_ASSERT( sds_ptr != NULL );

  mc_ptr->hdr.cmd                       = MC_SRVCC_ACT_F;

  CM_MSG_HIGH_0( "Domsel:Send MC_SRVCC_ACT_F to 1XCP");

  sds_ptr->is_cdma_srvcc_activated = TRUE;

  cm_mc_send_cmd( mc_ptr );
  #endif
} /* cmsds_send_1xsrvcc_activate_req */


/*===========================================================================

@FUNCTION cmsds_send_1xsrvcc_deactivate_req

@DESCRIPTION
  Send SRVCC deactivate request  to 1xCP.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_send_1xsrvcc_deactivate_req (prot_deact_e_type deact_reason)
{

  #ifndef FEATURE_MMODE_REMOVE_1X
  mc_msg_type                    *mc_ptr = NULL;
    /* MC command pointer */
  cmsds_s_type              *sds_ptr = cmsds_ptr();
    /* Domain Selection pointer*/

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (sds_ptr->is_cdma_srvcc_activated == FALSE)
  {
    CM_MSG_LOW_0( "Domsel:1XCP is not in SRVCC mode");
    return;
  }

  /* updating here instead of deact_cnf */
  sds_ptr->is_cdma_srvcc_activated = FALSE;
  mc_ptr                                = cm_mc_get_buf_else_err_fatal();

  CM_ASSERT( mc_ptr != NULL );

  mc_ptr->hdr.cmd                       = MC_SRVCC_DEACT_F;
  mc_ptr->srvcc_deact.deact_reason       = deact_reason;


  CM_MSG_HIGH_0( "Domsel:Send MC_1xSRVCC_DEACT_F to 1XCP");

  cm_mc_send_cmd( mc_ptr );
  #else
  #error code not present
#endif
} /* cmsds_send_1xsrvcc_deactivate_req */

#endif


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) && \
     defined( FEATURE_LTE_TO_1X )


/*===========================================================================

@FUNCTION cmsds_is_mode_supp_1xcsfb

@DESCRIPTION
  Checks whether mode pref supports 1xcsfb or not. This function should be used for
  all domain selection purposes.

@DEPENDENCIES
  None

@RETURNS
  None

@SIDE_EFFECT
  None

===========================================================================*/
static boolean cmsds_is_mode_supp_1xcsfb(
  sys_modem_as_id_e_type    as_id )

{
  cm_mode_pref_e_type      mode_pref = CM_MODE_PREF_NONE;
  sd_ss_mode_pref_e_type pm_mode = SD_SS_MODE_PREF_NONE;
  sd_ss_mode_pref_e_type comm_mode = SD_SS_MODE_PREF_NONE;	
  cmph_s_type                    *ph_ptr        = cmph_ptr();

  mode_pref = CMPH_SS_MODE_PREF(as_id,ph_ptr);
  
  /* If devices's mode pref is EMERGENCY map it to AUTOMATIC for calculation here. 
  ** This is done to avoid recursive calls from map_cm_mode_pref_to_sd_mode_pref->
  ** cmsds_is_LTE_emerg_call_possible->cmsds_get_csfb_type_allowed->cmsds_is_ppcsfb_call_allowed
  ** ->cmsds_is_mode_supp_ppcsfb.
  ** It is done as both AUTOMATIC and EMERGENCY have same set of MODES
  */
  if(mode_pref == CM_MODE_PREF_EMERGENCY)
  {
    mode_pref = CM_MODE_PREF_AUTOMATIC;
  }
  
  pm_mode    =  cmmsc_get_cm_ss_mode_cap(cmph_map_as_id_to_cm_ss(as_id));

  comm_mode = SD_GET_COMMON_MODE(cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref),pm_mode);
  
  CM_MSG_HIGH_2 ("1xcsfb: pm_mode_cap %d,mode_pref =%d ",\
                pm_mode, mode_pref);
  
  if(SD_SS_MODE_PREF_CDMA & comm_mode)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

@FUNCTION cmsds_is_wait_for_1xcsfb_status

@DESCRIPTION


@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
static boolean cmsds_is_wait_for_1xcsfb_status (
  sys_modem_as_id_e_type    as_id
)
{

  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  cmph_s_type    *ph_ptr   = cmph_ptr();
  cmss_s_type    *ss_ptr   = cmss_ptr();
  /* Pointer to serving system object. .*/

  if((sds_ptr->sib8_available == TRUE) &&
     (sds_ptr->csfb_status == CM_1XCSFB_NONE)  &&
     (cmsds_is_ppcsfb_call_allowed(as_id) == FALSE) &&
     (ss_ptr->info.true_srv_status != SYS_SRV_STATUS_LIMITED_REGIONAL) &&
     (sds_ptr->onexcsfb_status_not_avail_uptime == CMSDS_UPTIME_NONE) &&
     (cmsds_get_config_ptr(as_id)->ue_usage_setting == SYS_UE_USAGE_SETTING_VOICE_CENTRIC) &&
      cmsds_is_mode_supp_1xcsfb(as_id) &&
      cmph_is_lte_capable_on_sub(as_id) 
    )
  {
    return TRUE;
  }

  return FALSE;
}


/*===========================================================================

FUNCTION cmsds_update_sib8_status

DESCRIPTION
  Update the (service domain selection)SDS object per the new service indicators.



DEPENDENCIES
  SDS must have already been initialized with
  cmsds_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsds_update_sib8_status(
    cm_ss_e_type              ss,
    
    const sd_si_info_s_type   *si_info_ptr
       /* Pointer to buffer containing new service info information */
)

{

  cmsds_s_type   *sds_ptr  = cmsds_ptr();

  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);

  CM_MSG_MED_2 ("Domsel:Update SIB-8 Status %d rrc_1xcsfb_enabled %d", 
                  si_info_ptr->sib8_available,sds_ptr->rrc_1xcsfb_enabled);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sds_ptr != NULL );
  CM_ASSERT( si_info_ptr != NULL );

  if (as_id != SYS_MODEM_AS_ID_1)
  {
    return;
  }

  if (!cmph_is_lte_capable_on_sub(as_id))
  {
    return;
  }

  if(sds_ptr->rrc_1xcsfb_enabled == FALSE)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( si_info_ptr->sib8_available != SD_SIB8_NO_CHANGE )
  {

    if (si_info_ptr->sib8_available == SD_SIB8_NOT_AVAIL)
    {
      sds_ptr->sib8_available = FALSE;
    }
    else if (si_info_ptr->sib8_available == SD_SIB8_AVAIL)
    {
      sds_ptr->sib8_available = TRUE;
    }

    /* For IMS PREF just start the timer for 1XCSFB status */
    /* SIB8 is available but not got 1XCSFB status from 1XCP
       ** also make sure not in middle of 1XCSFB call then start a timer
       */
    if(cmsds_is_wait_for_1xcsfb_status(as_id) == TRUE)
    {
      sds_ptr->onexcsfb_status_not_avail_uptime = (time_get_uptime_secs()
                                       + sds_ptr->sib8_not_avail_duration);

      CM_MSG_HIGH_1("Domsel:SIB8 avail but CSFB status unknown yet start timer for %d sec ",\
                   sds_ptr->sib8_not_avail_duration );
    }
  }
 } /* cmsds_update_sib8_status */

/*===========================================================================

@FUNCTION cmsds_is_1xcsfb_activation_allowed

@DESCRIPTION
  Checks if 1XCP can be activated in CSFB mode

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

static boolean cmsds_is_1xcsfb_activation_allowed
(
  sys_modem_as_id_e_type  as_id
)
{

  cmph_s_type    *ph_ptr        = cmph_ptr();
    /* Point at phone object */
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
    /* Point at service domain selection  object */
   sd_si_info_s_type    si_info;
    /* Point at SD SI info */

  cm_ss_e_type            cm_ss = cmph_map_as_id_to_cm_ss(as_id);
  sd_ss_e_type            sd_ss = cmph_map_cm_ss_type_to_sd_ss_type(cm_ss);

  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( sds_ptr != NULL );

  if (!cmph_is_lte_capable_on_sub(as_id))
  {
    return FALSE;
  }

  if (as_id != SYS_MODEM_AS_ID_1)
  {
    return FALSE;
  }

  sd_si_info_ptr_get(sd_ss, &si_info);
  /* check if  CSFB available is recived,LTE PS attach success and
  ** CDMA subscription is available
  */
  if( ((sds_ptr->csfb_status == CM_1XCSFB_AVAIL)              ||
       (sds_ptr->csfb_status == CM_1XCSFB_PREREG_SUCCESS))    &&
      (sds_ptr->sib8_available == TRUE)                       &&
      (si_info.srv_status == SYS_SRV_STATUS_SRV)              &&
      (si_info.srv_domain == SYS_SRV_DOMAIN_PS_ONLY)          &&
      (si_info.mode   ==  SYS_SYS_MODE_LTE)                   &&
      (ph_ptr->is_cdma_subscription_available == TRUE)        &&
      (ph_ptr->oprt_mode == SYS_OPRT_MODE_ONLINE)             &&
      (sds_ptr->is_1xcsfb_activated == FALSE)                 &&
      (cmregprx_is_gwl_protocol_active(sd_ss) == TRUE)   &&
      (cmsds_is_mode_supp_1xcsfb(as_id))
    )
  {
    /* 1X CSFB activation can be done */
    return TRUE;

  } else
  {
    /*1X CSFB activation can not  be done */
    return FALSE;
  }
} /* cmsds_is_1xcsfb_activation_allowed */


/*===========================================================================

@FUNCTION cmsds_send_1xcsfb_activate_req

@DESCRIPTION
  Send CSFB activate request  to 1xCP.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
static void cmsds_send_1xcsfb_activate_req
(
  void
)
{
  #ifndef FEATURE_MMODE_REMOVE_1X
  mc_msg_type                    *mc_ptr = NULL;
    /* MC command pointer */
  cmsds_s_type                            *sds_ptr               = cmsds_ptr();


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */




  mc_ptr                                = cm_mc_get_buf_else_err_fatal();

  CM_ASSERT( mc_ptr != NULL );
  CM_ASSERT( sds_ptr != NULL );

  mc_ptr->hdr.cmd                       = MC_CSFB_ACT_F;

  CM_MSG_HIGH_0( "Domsel:Send MC_CSFB_ACT_F to 1XCP");

  sds_ptr->is_1xcsfb_activated = TRUE;

  cm_mc_send_cmd( mc_ptr );
  #endif
} /* cmsds_send_1xcsfb_activate_req */

/*===========================================================================

FUNCTION cmsds_process_sib8_status

DESCRIPTION
  Process the SIB-8 status received from SD.



DEPENDENCIES
  SDS must have already been initialized with
  cmsds_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsds_process_sib8_status(

    const sd_si_info_s_type   *si_info_ptr,
       /* Pointer to buffer containing new service info information */
    
    sys_modem_as_id_e_type     as_id
)

{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( si_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( si_info_ptr->sib8_available != SD_SIB8_NO_CHANGE )
  {
    if ( cmsds_is_1xcsfb_activation_allowed (as_id) == TRUE)
    {
      /* CSFB activation can be done now */
      cmsds_send_1xcsfb_activate_req ();
    }
  }

 } /* cmsds_update_sib8_status */

/*===========================================================================

FUNCTION cmsds_is_csfb_deact_allowed

DESCRIPTION
  This function checks whether CSFB deactivation is allowed or not.

DEPENDENCIES
  none

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmsds_is_csfb_deact_allowed
(
  void
)
{
  cmsds_s_type      *sds_ptr        = cmsds_ptr();
  boolean           is_deact_allowed = FALSE;

  if ((TRUE == sds_ptr->is_1xcsfb_activated) &&
      (FALSE == sds_ptr->is_1xcsfb_deact_in_prog))
  {
    is_deact_allowed = TRUE;
  }

  return is_deact_allowed;
} /* cmsds_is_csfb_deact_allowed*/

/*===========================================================================

FUNCTION cmsds_is_in_1xcsfb_mode

DESCRIPTION
Indicates whether 1XCSFB is activated or not.

DEPENDENCIES
  SDS must have already been initialized with
  cmsds_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmsds_is_in_1xcsfb_mode()
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();

  return sds_ptr->is_1xcsfb_activated;
}

#endif

#ifdef FEATURE_DOMAIN_SELECTION

/*===========================================================================

FUNCTION cmsds_update_voice_support_status_on_lte

DESCRIPTION
  processes the srv indication


DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

========================================================================== */


static void cmsds_update_voice_support_status_on_lte
(
  cm_ss_e_type              ss,

  boolean is_voice_supported
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();

  cmss_s_type              *ss_ptr             = cmss_ptr();
  /* Pointer to serving system object.
 .*/

  CM_ASSERT( sds_ptr != NULL );
  CM_ASSERT( ss_ptr != NULL );

  if(is_voice_supported)
  {
    /* Voice is supported so indicate  VOICE support to
    ** clients if not already done
    */
    if (ss_ptr->info.voice_support_on_lte  == FALSE)
    {
      sds_ptr->voice_support_status = TRUE;
      ss_ptr->info.voice_support_on_lte = TRUE;
      CM_MSG_HIGH_0 ("DOM_SEL: Indicating VOICE support on LTE ");

      if (ss == CM_SS_MAIN)
      {
        ss_ptr->info.changed_fields |= CM_SS_VOICE_SUPPORT_ON_LTE_MASK;
      }
      else if (ss == CM_SS_HYBR_2)
      {
        ss_ptr->info.hybr_gw_changed_fields |= CM_SS_GW_VOICE_SUPPORT_ON_LTE_MASK;
      }
      else if (ss == CM_SS_HYBR_3)
      {
        ss_ptr->info.gw3_changed_fields |= CM_SS_GW_VOICE_SUPPORT_ON_LTE_MASK;
      }
    }
  }
  else
  {
    /* Voice is not supported so indicate NO VOICE support to
    ** clients if not already done
    */
    if ( ss_ptr->info.voice_support_on_lte  == TRUE)
    {
      sds_ptr->voice_support_status = FALSE;
      ss_ptr->info.voice_support_on_lte = FALSE;
      CM_MSG_HIGH_0 ("DOM_SEL: Indicating  NO VOICE support on LTE ");

      if (ss == CM_SS_MAIN)
      {
        ss_ptr->info.changed_fields |= CM_SS_VOICE_SUPPORT_ON_LTE_MASK;
      }
      else if (ss == CM_SS_HYBR_2)
      {
        ss_ptr->info.hybr_gw_changed_fields |= CM_SS_GW_VOICE_SUPPORT_ON_LTE_MASK;
      }
      else if (ss == CM_SS_HYBR_3)
      {
        ss_ptr->info.gw3_changed_fields |= CM_SS_GW_VOICE_SUPPORT_ON_LTE_MASK;
      }
    }
  }
} /* cmsds_update_voice_support_status_on_lte */

/*===========================================================================

FUNCTION cmsds_is_voice_avail_with_mode_pref

DESCRIPTION
  Indicates the possibility of voice services (G/W/T) based on given mode preferance.


DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

========================================================================== */


boolean cmsds_is_voice_avail_with_mode_pref
(
  cm_mode_pref_e_type mode_pref
)
{
  boolean is_voice_avail;

  if((SD_GET_COMMON_MODE(cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref),
      SD_SS_MODE_PREF_CDMA) == SD_SS_MODE_PREF_NONE) &&
      (cmph_ptr()->main_stack_info.pref_info.srv_domain_pref == CM_SRV_DOMAIN_PREF_PS_ONLY))
  {
    /* Serving domain is PS only. So CS voice is not possible and don't disbale LTE as CS RAT is not there */
    is_voice_avail = FALSE;
  }
  else if (SD_GET_COMMON_MODE ( cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref),
                             SD_SS_MODE_PREF_CDMA_HDR_GW ) != SD_SS_MODE_PREF_NONE)
  {
    /* Check for voice support on CDMA HDR GW */
    is_voice_avail = TRUE;
  }
  else if (SD_GET_COMMON_MODE ( cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref),
                                  SD_SS_MODE_PREF_TDS ) != SD_SS_MODE_PREF_NONE)
  {
    /* Check for voice support on TDS*/
    is_voice_avail = TRUE;
  }
  else
  {
    /* Vopice is not supported on give mode_pref */
    is_voice_avail = FALSE;
  }

  return is_voice_avail;
}

/*===========================================================================

@FUNCTION cmsds_is_ps_sms_possible

@DESCRIPTION
Indicates whether SMS Over IMS is possible or not.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

static boolean cmsds_is_ps_sms_possible
(
  sys_sys_mode_e_type  sys_mode,
  sys_modem_as_id_e_type sub_id
)
{

  cmsds_s_type   *sds_ptr            = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  cmsds_sub_config_s_type   *cmsds_config = cmsds_get_config_ptr(sub_id);

  if ((cmsds_config->sms_domain_pref[SYS_SYS_MODE_LTE] == SYS_SMS_DOMAIN_PREF_PS_SMS_NOT_ALLOWED) ||
      (((sds_ptr->ims_reg_status_sms[sys_mode] == SYS_IMS_REG_STATE_DEREGISTERED) ||
         (!cmsds_config->is_ims_enabled)) &&
       (cmsds_config->sms_domain_pref[SYS_SYS_MODE_LTE] == SYS_SMS_DOMAIN_PREF_PS_SMS_PREF))
     )
  {
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

@FUNCTION cmsds_is_cs_sms_possible

@DESCRIPTION
Indicates whether SMS Over SGs/1xcsfb is possible or not.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

static boolean cmsds_is_cs_sms_possible
(
   cm_ss_e_type              ss
)
{

  cmsds_s_type   *sds_ptr            = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  cmph_s_type     *ph_ptr  = cmph_ptr();
  /* Pointer to phone object. .*/

  sd_si_info_s_type   *si_info_ptr;
  sys_modem_as_id_e_type asubs_id = cmph_map_cm_ss_to_subs(ss);

  /* LTE SMS domain selection
  ** Check for SMS over SG's support
  */
  if(sds_ptr->srv_ind_buf_ptr != NULL)
  {
    si_info_ptr = sds_ptr->srv_ind_buf_ptr;
  }
  else
  {
    si_info_ptr = cmss_intl_srv_info_ptr(ss);
  }
  if((si_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS) &&
    (si_info_ptr->mode   ==  SYS_SYS_MODE_LTE) && 
    ((sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_NO_VALUE) ||
     (sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_SMS_ONLY) ||
     (sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_CSFB_NOT_PREFFERED)))
  {
    CM_MSG_HIGH_0("SMS over SGs is possible");
    return TRUE;
  }
  else if(((CM_SRV_DOMAIN_PREF_CS_PS == ph_ptr->main_stack_info.pref_info.srv_domain_pref) ||
           (CM_SRV_DOMAIN_PREF_PS_ONLY == ph_ptr->main_stack_info.pref_info.srv_domain_pref))&&
           (SYS_SRV_DOMAIN_PS_ONLY == si_info_ptr->srv_domain))
  {
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) && \
         defined( FEATURE_LTE_TO_1X )
      if(((cmsds_get_csfb_type_allowed(asubs_id) == CM_CALL_CSFB_TYPE_1XCSFB) ||
            cmsds_is_1xcsfb_activation_allowed(asubs_id)) &&
            (cmsds_get_config_ptr(asubs_id)->is_sms_over_s102_supported == TRUE))
      {
        CM_MSG_HIGH_0("SMS over 1X is possible");
        return TRUE;
      }
    #endif
  }

  return FALSE;
}

/*===========================================================================

@FUNCTION cmsds_process_sms_domain_selection

@DESCRIPTION
Performs SMS domain selection
1. If sms_domain_pref is PS_PREF, send IMS_PREF_IND for SMS servcies to IMS and wait for regsitration status.
2. CM send's PREF_IND for SMS services only at POWER UP or (OOS -> LTE) or (IRAT -> LTE).
3. Do not Send PREF_IND to IMS, once it informs that registration is failed for SMS services. IMS only informs REG_STATUS
   as FAILURE only after performing all retries.
4. If sms_domain_pref is PS_SMS_NOT_ALLOWED, then looks for SMS Over SG's or SMS Over S102.
5. If SMS is mandatory and SMS servcie is not availble, then perform PLMN blocking & DETACH.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

static void cmsds_process_sms_domain_selection
(
  cm_ss_e_type              ss,
  /* stack on which domain selection to be performed.*/
  sys_sys_mode_e_type  sys_mode
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/
  sd_si_info_s_type   *si_info_ptr;

  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);

  cmsds_sub_config_s_type   *cmsds_config = cmsds_get_config_ptr(as_id);

  CM_ASSERT( sds_ptr != NULL );

  CM_MSG_HIGH_2( "SMS Domsel:current SMS domain preferance =%d,sys_mode=%d",
                 cmsds_config->sms_domain_pref[sys_mode], sys_mode);

  if(sds_ptr->srv_ind_buf_ptr != NULL && sys_mode == SYS_SYS_MODE_LTE)
  {
    si_info_ptr = sds_ptr->srv_ind_buf_ptr;
  }
  else
  {
    si_info_ptr = cmss_intl_srv_info_ptr(ss);
  }

  if (si_info_ptr->mode != sys_mode)
  {
    CM_MSG_HIGH_1( "Skip SMS domain selection due to sys mode mismatch =%d", si_info_ptr->mode);
    return;
  }

  /* Report IMS availablity */
  if ((sds_ptr->ims_reg_status_sms[sys_mode] == SYS_IMS_REG_STATE_UNKNOWN)&&
        (cmsds_config->is_ims_enabled))
  {

    /* Generally, CM sends IMS_PREF_IND for SMS servcies while sending PREF_IND for voice servcies itself.
    ** If voice domain selection doesn't result in sending IMS_PREF_IND for voice servcies, then CM has to
    ** send PREF_IND for SMS services alone here */

    if (cmsds_config->sms_domain_pref[sys_mode] == SYS_SMS_DOMAIN_PREF_PS_SMS_PREF)
    {
      sds_ptr->ims_reg_status_sms[sys_mode] = SYS_IMS_REG_STATE_NEEDS_REG;
      cmsds_send_ims_pref_changed_ind(sys_mode, CM_CALL_TYPE_SMS_MASK, CM_CALL_TYPE_SMS_MASK, as_id);
    }
    else
    {
        sds_ptr->ims_reg_status_sms[sys_mode] = SYS_IMS_REG_STATE_DEREGISTERED;
        cmsds_send_ims_pref_changed_ind(sys_mode, CM_CALL_TYPE_NONE_MASK, CM_CALL_TYPE_SMS_MASK, as_id);
    }
  }

  /* Check for CS support if PS SMS is ot possible */
  if (!cmsds_is_ps_sms_possible(sys_mode, as_id))
  {
    /* SMS over IMS is not possible. Check for any CS service that provides SMS */

    cmsds_sms_dom_sel_process_no_ims(ss, sys_mode);

  }

}

#if defined (FEATURE_LTE_TO_1X)

/*===========================================================================

@FUNCTION cmsds_process_1xcsfb_status

@DESCRIPTION


@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/


static void cmsds_process_1xcsfb_status
(
  cm_ss_e_type              ss
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  cmss_s_type    *ss_ptr   = cmss_ptr();

  cm_call_type_mask_type ims_pref_mask = CM_CALL_TYPE_NONE_MASK;

  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);

  cmsds_sub_config_s_type   *cmsds_config = cmsds_get_config_ptr(as_id);

  if (as_id != SYS_MODEM_AS_ID_1 || !cmph_is_lte_capable_on_sub(as_id))
  {
    return;
  }

  if ( sds_ptr->is_dual_rx_1xcsfb == TRUE)
  {
    /* Dual Rx 1XCSFB enabled so no need of checking for CSFB status */
    return;
  }

  #if defined( FEATURE_3GPP_CSFB)
  if ( cmsds_is_ppcsfb_call_allowed(as_id) == TRUE )
  {
    /*  PPCSFB can be used so no need of acting on 1XCSFB status */
    return ;
  }
  #endif

  /* check if 1XCSFB is not possible
  */
  if((sds_ptr->csfb_status == CM_1XCSFB_NOT_AVAIL) ||
     (sds_ptr->csfb_status == CM_1XCSFB_PREREG_FAIL)
    )
  {
    /* As CS voice is not available Perform IMS registration if IMS voice is available */
    if(cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED)
    {

      /*Post IMS PREFERRED IND Message to IMS*/
      if (cmsds_is_volte_possible(ss_ptr->info.lte_ims_voice_avail, as_id) == TRUE)
      {
        cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_NEEDS_REG,SYS_SYS_MODE_LTE);
        ims_pref_mask = CM_CALL_TYPE_VOICE_MASK;

        #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) && \
            defined( FEATURE_LTE_TO_1X )
        cmsds_check_and_deact_csfb_mode ();
        #endif
      }

      /* For CS_PREF configuration, CM will NOT send IMS_PREF_IND for voice services as long as
         CS voice is availble. When CS voice is not availble, then CM will send PREF_IND
         for voice servcies  No need to send PREF_IND for SMS servcie here as it would
         have been sent while processing the CM_SERVCIE_IND.*/
      cmsds_send_ims_pref_changed_ind(SYS_SYS_MODE_LTE, ims_pref_mask,
                                       CM_CALL_TYPE_VOICE_MASK, as_id);

      /*Both IMS and CS voice are not available */
      if (cmsds_is_volte_possible(ss_ptr->info.lte_ims_voice_avail, as_id) == FALSE)
      {
        /* Perform PLMN blocking as per device usage setting */
        cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_NEEDS_DEREG,SYS_SYS_MODE_LTE);
        if (sds_ptr->pending_uemode_cmsds_skipped == FALSE)
        {
           /*Perform PLMN blocking for voice centri devices*/
           cmsds_process_voice_not_avail_ind(ss);
        }
      }
    }

    /* Perform PLMN locking for Voice domain config is "CS_VOICE_ONLY", */
    if (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY
         && sds_ptr->pending_uemode_cmsds_skipped == FALSE)
    {
      /*Perform PLMN blocking for voice centri devices*/
      cmsds_process_voice_not_avail_ind(ss);
    }

    if(cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED)
    {
      /* Block LTE  if IMS voice is not possible */
      if ( (sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_LTE] == SYS_IMS_REG_STATE_DEREGISTERED) ||
           (cmsds_is_volte_possible(ss_ptr->info.lte_ims_voice_avail, as_id) == FALSE)
         )
      {
        if (sds_ptr->pending_uemode_cmsds_skipped == FALSE)
        {
           /*Perform PLMN blocking for voice centri devices*/
           cmsds_process_voice_not_avail_ind(ss);
        }
      }
    }
  }
} /* cmsds_process_1xcsfb_status */

#endif /* FEATURE_LTE_TO_1X */

/*===========================================================================

@FUNCTION cmsds_skip_sds_during_srv_ind

@DESCRIPTION
Checks whether domain selection need to be skipped due to PREF SRV IND
Skip SDS in following cases

1. SD is in NO_SRV state and CM received FULL_SRV_IND on LTE with pref_srv_ind set to TRUE.
2. Skip SDS due to PREF_SRV_IND during silent service lost processing.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

boolean cmsds_skip_sds_during_srv_ind(
    cm_ss_e_type              ss,

    const sd_si_info_s_type   *si_info_ptr
       /* Pointer to buffer containing new service info information */
)
{
  boolean           is_skip_sds = FALSE;
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  sys_modem_as_id_e_type  sub = cmph_map_cm_ss_to_subs(ss);
  /* Take intersection of mode_pref for the sub and mode capability for that stack */
  uint16 intersect_mode = SD_GET_COMMON_MODE(cmph_map_cm_mode_pref_to_sd_mode_pref(CMPH_SS_MODE_PREF(sub, cmph_ptr())),
                                            cmph_map_sys_mode_mask_to_sd_mode_pref(cmmsimc_state_machine_ptr(sub)->stack_common_info.ue_mode_capability));


  /* Skip Domain selection if mode_pref for the sub itself doesnt have LTE */
  if(!sd_misc_is_mode_pref(intersect_mode, SD_SS_MODE_PREF_LTE))
  {
    is_skip_sds = TRUE;
  }
  /* Skip procesing LTE service on MAIN stack if hybr2 is already operational and on hybr2
  ** stack if hybr2 is not on, as it will be a stale LTE service
  */
  else if(cmph_is_subs_feature_mode_1x_sxlte(sub) &&
     si_info_ptr->mode == SYS_SYS_MODE_LTE &&
    ((cmmsc_auto_is_hybr2_on(cmmsimc_state_machine_ptr(sub)->op_mode))?( ss == CM_SS_MAIN):( ss == CM_SS_HYBR_2)) )

  {
    is_skip_sds = TRUE;
    CM_MSG_HIGH_0("Skip processing LTE stale service on MAIN stack as hybr2 is on" );
  }
  else if ((si_info_ptr->is_stable_in_svc == FALSE) ||
      (sds_ptr->is_activation_in_progress == TRUE))

  {
    is_skip_sds = TRUE;
    CM_MSG_MED_2("Skip SDS as SD is NOT in OPR STATE and stable service is not acquired %d %d",\
                  si_info_ptr->is_stable_in_svc, sds_ptr->is_activation_in_progress );
  }

  return is_skip_sds;
}

/*===========================================================================

@FUNCTION cmsds_is_volte_possible

@DESCRIPTION
  Indicates whether VOLTE is possible or not.
  If NV ssac_hvolte is ON, use below logic to decide if VOLTE is possible
  VOLTE area (VA): VOPS && non-p00
  Non-VOLTE area (Non-VA)): non-VOPS or p00

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
boolean cmsds_is_volte_possible
(
  boolean lte_ims_voice_avail,
  sys_modem_as_id_e_type sub_id
)
{
 
  boolean is_ssac_p00 = FALSE;

  /* If NV ssac_hvolte is ON, use below logic to decide if VOLTE is possible
  ** VOLTE area (VA): VOPS && non-p00
  ** Non-VOLTE area (Non-VA)): non-VOPS or p00
  */
  if(cmph_get_volte_sub_info_ptr(sub_id)->is_ssac_hvolte)
  {
    is_ssac_p00 = cmcall_is_ssac_p00();
  }

  if ((cmsds_get_config_ptr(sub_id)->is_ims_enabled) &&
      (lte_ims_voice_avail == TRUE) &&
      !is_ssac_p00)
  {
    CM_MSG_MED_0("VOLTE possible");
    return TRUE;
  }
  CM_MSG_MED_0("VOLTE not possible");
  return FALSE;
}
/*===========================================================================

@FUNCTION cmsds_update_va_flag

@DESCRIPTION
  update VA flag.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

static void cmsds_update_va_flag
(
  boolean p00,
  boolean lte_ims_voice_avail,
  cm_ss_e_type              ss
)
{
  sd_si_info_s_type *cmss_true_info_ptr = NULL;
  cmsds_s_type      *sds_ptr    = cmsds_ptr();
  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();

  cmss_true_info_ptr = cmss_intl_srv_info_ptr(ss);

  if(lte_ims_voice_avail && !p00)
  {
    sds_ptr->va_type.va = CMSDS_VA_TYPE_VOPS;
  }
  else
  {
    sds_ptr->va_type.va = CMSDS_VA_TYPE_NON_VOPS;
  }
   /*if SSAC associated PLMN is same as VOPS associated PLMN update VA associated PLMN */
  if(sys_plmn_id_is_valid(cmss_true_info_ptr->sid.plmn.plmn_id) && sys_plmn_id_is_valid(cmcall_ssac_info->ssac_associated_plmn) &&
    sys_plmn_match(cmss_true_info_ptr->sid.plmn.plmn_id, cmcall_ssac_info->ssac_associated_plmn))
  {
    sds_ptr->va_type.associated_plmn_id = cmss_true_info_ptr->sid.plmn.plmn_id;
  }
  
  CM_MSG_MED_4("is_va:%d, plmn %d, %d %d",sds_ptr->va_type.va, sds_ptr->va_type.associated_plmn_id.identity[0],
  sds_ptr->va_type.associated_plmn_id.identity[1], sds_ptr->va_type.associated_plmn_id.identity[2]);
  return;
}
/*===========================================================================

@FUNCTION cmsds_start_tssac_timer

@DESCRIPTION
  check and start tssac hystersis timer if needed. Start hystersis timer
  Condition: SRLTE mode && previous non-VA && current VA && voice domain != IMS && tssac is not running

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

static void cmsds_start_tssac_timer(
    cm_ss_e_type              ss,

    sys_modem_as_id_e_type          as_id
)
{
  cmsds_s_type      *sds_ptr    = cmsds_ptr();
  cmsds_va_type           prev_va = sds_ptr->va_type.va;
  volte_sub_info_s_type* volte_sub_info_ptr = cmph_get_volte_sub_info_ptr(as_id);
  sys_plmn_id_s_type  old_plmn;

  old_plmn = sds_ptr->va_type.associated_plmn_id;
  
  cmsds_update_va_flag(cmcall_is_ssac_p00(), cmss_ptr()->info.lte_ims_voice_avail,ss);

  CM_MSG_MED_3("prev_va=%d, tssac_hys=%d, voice_reg_status=%d",prev_va, sds_ptr->tssac_hys, sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_LTE]);

  if((prev_va == CMSDS_VA_TYPE_NON_VOPS) &&
     (sds_ptr->va_type.va == CMSDS_VA_TYPE_VOPS)&& (sds_ptr->tssac_hys == 0) &&
    (sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_LTE] != SYS_IMS_REG_STATE_REGISTERED) &&
    (sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_LTE] != SYS_IMS_REG_STATE_REG_IN_PROGRESS)
   )
  {
    /*The reason of not using internal srv status is to avoid switch to CSFB for buffered no srv ind.*/
    if((ss == CM_SS_HYBR_2) && (cmss_ptr()->main_srv_status != SYS_SRV_STATUS_SRV))
    {
      CM_MSG_HIGH_2("Skip t_SSAC timer. main_srv_status %d, t_ssac %d",cmss_ptr()->main_srv_status,
                      volte_sub_info_ptr->t_ssac);
    }
    else if(sys_plmn_match(old_plmn, sds_ptr->va_type.associated_plmn_id))
    {
      sds_ptr->tssac_hys = time_get_uptime_secs() + volte_sub_info_ptr->t_ssac;
      CM_MSG_HIGH_2("t_SSAC timer started uptime=%d, curr_time=%d",sds_ptr->tssac_hys, time_get_uptime_secs());
    }
  }
  return;
}

/*===========================================================================

@FUNCTION cmsds_stop_tssac_timer

@DESCRIPTION
  stop tSSAC timer when UE moves to non-VA or timer expired

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

static void cmsds_stop_tssac_timer(void)
{
  cmsds_s_type      *sds_ptr    = cmsds_ptr();

  sds_ptr->tssac_hys = 0;
  CM_MSG_MED_0("stop tssac timer");
  return;
}

/*===========================================================================

@FUNCTION cmsds_is_reset_lte_cs_capability

@DESCRIPTION
Indicates whetherlte-cs_capability should be reset or not.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
static boolean cmsds_is_reset_lte_cs_capability(
  sys_modem_as_id_e_type sub_id
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  
  cmsds_sub_config_s_type   *cmsds_config = cmsds_get_config_ptr(sub_id);

  if (sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_BARRED)
  {
    /* 3GPP reject cause should be considered for following cases.
           1.  If device supports VOLTE and VOPS is FALSE from NW ' CM ignores the cause #18 and PLMN blocking duration will be T3402/NV value.
           2.  If device supports VOLTE and VOPS is TRUE form NW and IMS informed registration failure ' CM ignores the cause #18 and PLMN blocking duration will be T3402 value.
           3.  If device doesn't support VOLTE and 1XCSFB is also not possible in the NW ' CM will process cause #18 and uses PLMN blocking duration of till power cycle.
           4.  If device & NW supports 1XCSFB and receives 1xcsfb pre-registration failure ' CM ignores cause #18 and PLMN blocking duration will be T3402 value.
           5.  If the device is in SVLTE/SRLTE operation, then CM will not perform PLMN blocking at all as voice can be supported over 1X.
       */
    if ((cmsds_config->is_ims_enabled) &&
        (cmsds_config->voice_domain_pref != SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY))
    {
     /* If SMS is preferred over IMS, it means both Voice and SMS are preferred over IMS.
         ** Ignore 3GPP cause #18
         */
      if (cmsds_config->sms_domain_pref[SYS_SYS_MODE_LTE] == SYS_SMS_DOMAIN_PREF_PS_SMS_PREF)
      {
        CM_MSG_MED_0("Domsel:Reset lte_cs_capability as device prefers voice and SMS over IMS");
        return TRUE;
      }

      /* If SMS is NOT preferred over IMS and sms is not mandatory, it means SMS over SG's(3GPP NW) is used.
          ** voice is given preference and ignore 3GPP reject cause #18.
          ** If SMS is NOT preferred over IMS and sms is  mandatory, it means SMS over SG's(3GPP NW) is used. Don't ignore 3GPP reject casue.
          ** If a 3GPP2 operator is using this PS_SMS_NOT_ALLOWED config, then lte_cs_capability will be reset while procesisng
          ** the csfb_status from 1XCP.
          */

      if ((cmsds_config->sms_domain_pref[SYS_SYS_MODE_LTE] == SYS_SMS_DOMAIN_PREF_PS_SMS_NOT_ALLOWED) &&
          (cmsds_config->is_sms_mandatory == FALSE))
      {
        CM_MSG_MED_0("Domsel:Reset lte_cs_capability as device support VOLTE");
        return TRUE;
      }
    }
    /* Reset lte_cs_capability if CSFB status is received from 1XCP
    If CSFB STATUS is recieved, it means that the NW is 1XCSFb network
    and can ignore the 3GPP reject causes */
    #if defined( FEATURE_LTE_TO_1X ) 
    else if(sds_ptr->csfb_status != CM_1XCSFB_NONE )
    {
      CM_MSG_HIGH_1("Domsel:Reset lte_cs_cap as csfb_status=%d", sds_ptr->csfb_status);
      return TRUE;
    }
    #endif
  }

  return FALSE;
}
/*===========================================================================

@FUNCTION cmsds_reset_srv_ind_buf_ptr

@DESCRIPTION


@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
static void cmsds_reset_srv_ind_buf_ptr(void)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();

  if(sds_ptr->srv_ind_buf_ptr != NULL)
  {
    cm_mem_free(sds_ptr->srv_ind_buf_ptr);
    sds_ptr->srv_ind_buf_ptr = NULL;
    CM_MSG_MED_0("reset srv_ind_buf_ptr");
  }

  return;
}

/*===========================================================================

@FUNCTION cmsds_reset_srv_ind_buf_ptr_per_mode

@DESCRIPTION
 For modes other than LTE, after corresponding buffered service ind
 has been processed, free the srv_ind_buf if srv_ind_buf->mode is same 
 as the one being processed.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
static void cmsds_reset_srv_ind_buf_ptr_per_mode(sys_sys_mode_e_type sys_mode )
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();

  if(sds_ptr->srv_ind_buf_ptr != NULL && sds_ptr->srv_ind_buf_ptr->mode == sys_mode )
  {
    cm_mem_free(sds_ptr->srv_ind_buf_ptr);
    sds_ptr->srv_ind_buf_ptr = NULL;
    CM_MSG_MED_1("reset srv_ind_buf_ptr for mode %d", sys_mode);
  }

  return;
}

/*===========================================================================

@FUNCTION cmsds_process_srv_ind

@DESCRIPTION


@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/


void cmsds_process_srv_ind (
    cm_ss_e_type              ss,
    /* stack on which domain selection to be performed.*/

    const sd_si_info_s_type   *si_info_ptr
       /* Pointer to buffer containing new service info information */
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/
  cm_call_id_type  ims_call_id;
  cmcall_s_type    *call_ptr;  
  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);
  cmsds_sub_config_s_type   *cmsds_config = cmsds_get_config_ptr(as_id);
  
  CM_ASSERT( sds_ptr != NULL );

  CM_MSG_MED_6("Domsel:process_srv_ind, mode=%d, srv=%d, ss=%d,sms_only=%d,ue_usage_setting=%d,lte_disable_duration=%d",
               si_info_ptr->mode,si_info_ptr->srv_status,ss,
               cmsds_config->is_sms_only,cmsds_config->ue_usage_setting,
               cmsds_config->lte_disable_duration);

  CM_MSG_MED_6( "Domsel:curr vocie domain preferance %d ,lte_cs_capability %d, ims_avail %d,sms domain preferance %d ,sms_mandatory %d ,sms_s102_supp %d",\
                cmsds_config->voice_domain_pref, si_info_ptr->lte_cs_capability,si_info_ptr->lte_ims_voice_avail,
               cmsds_config->sms_domain_pref[si_info_ptr->mode],cmsds_config->is_sms_mandatory,cmsds_config->is_sms_over_s102_supported);
  CM_MSG_MED_2("Domsel:extend_srv_info=%d, ims_enabled =%d",si_info_ptr->extend_srv_info, \
                 cmsds_config->is_ims_enabled);

  if (!cmph_is_lte_capable_on_ss(ss))
  {
    return;
  }

  /* Skip processing if hybr2 is not operational and ss == hybr2*/
  if(cmph_is_subs_feature_mode_1x_sxlte(as_id) &&
     ss == CM_SS_HYBR_2   &&
     cmss_ptr()->info.is_hybr_gw_operational == FALSE)
  {
    /* During powerup in SVLTE mode , we will send SRV-REQ on HYBR2 for LTE ,
    ** when GWL-ACQ-SUCCESS is reported ,
    ** SD first sends SD_GENERIC_CB_TYPE_MCC with new MCC and calls cm_si_cb later ,
    ** CMSOA process the new MCC and decided to switch to CSFB .
    ** Ideally by the time cm_si_cb is called CMSOA already triggered a mode switch to CSFB
    ** Hence this SRV-IND is no more valid , hence we can return here by discarding it .
    */
    return;
  }

  /* Do not even buffer SRV IND during ongoing VOLTE call, if CM SDS was anyway
  ** going to skip processing it
  */
  if (cmsds_skip_sds_during_srv_ind(ss,si_info_ptr))
  {
    return;
  }

  ims_call_id= cmcall_is_there_volte_call_obj(CM_CALL_TYPE_VOICE_MASK|CM_CALL_TYPE_VT_MASK|CM_CALL_TYPE_EMERG_MASK);

  if(CM_CALL_ID_INVALID != ims_call_id)
  {
    CM_MSG_HIGH_1( "Domsel: found IMS call %d", ims_call_id);
  }

  call_ptr = cmcall_ptr ( ims_call_id );
  if((call_ptr != NULL)&&
    ((call_ptr->call_type == CM_CALL_TYPE_VOICE) || (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) ||
    (call_ptr->call_type == CM_CALL_TYPE_VT))
   )
  {
    CM_MSG_HIGH_1( "Domsel: hold srv ind due to ongoing IMS call %d", ims_call_id);


    if(sds_ptr->srv_ind_buf_ptr == NULL)
    {
      sds_ptr->srv_ind_buf_ptr = (sd_si_info_s_type *)cm_mem_malloc(sizeof(sd_si_info_s_type));
    }

    if(sds_ptr->srv_ind_buf_ptr != NULL)
    {
      memscpy(sds_ptr->srv_ind_buf_ptr, sizeof(sd_si_info_s_type), si_info_ptr, sizeof(sd_si_info_s_type));
    }

    return;
  }
#if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE))
  if(cmsoa_skip_dom_sel_due_to_pending_ue_mode())
  {
    CM_MSG_HIGH_0("SRLTE: Skip Dom Sel, pending_ue_mode ");
    sds_ptr->pending_uemode_cmsds_skipped = TRUE;
    return;
  }
  else
  {
    sds_ptr->pending_uemode_cmsds_skipped = FALSE;
  }
#endif


  switch(si_info_ptr->mode)
  {
  case SYS_SYS_MODE_GSM:
  case SYS_SYS_MODE_WCDMA:
  case SYS_SYS_MODE_TDS:
    /* always send hard coded value for voice on legacy RATs. SMS is done in below function */
    cmsds_update_and_send_ims_avail(CMSDS_IMS_VOICE_NOT_AVAIL, si_info_ptr->mode, as_id);
    cmsds_update_and_send_sms_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_3GPP, si_info_ptr->mode);
    cmsds_reset_srv_ind_buf_ptr_per_mode(si_info_ptr->mode);
    return;

  case SYS_SYS_MODE_HDR:
    /* always send hard coded value for voice on legacy RATs. SMS is done in below function */
    cmsds_update_and_send_ims_avail(CMSDS_IMS_VOICE_NOT_AVAIL, si_info_ptr->mode, as_id);
    cmsds_reset_srv_ind_buf_ptr_per_mode(si_info_ptr->mode);
    return;

  case SYS_SYS_MODE_CDMA:
    /* always send hard coded value for voice on legacy RATs. SMS is done in below function */
    cmsds_update_and_send_ims_avail(CMSDS_IMS_VOICE_NOT_AVAIL, si_info_ptr->mode, as_id);
    cmsds_update_and_send_sms_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_1X, si_info_ptr->mode);
    cmsds_reset_srv_ind_buf_ptr_per_mode(si_info_ptr->mode);
    return;

  case SYS_SYS_MODE_LTE:
    break;

  default:
    CM_MSG_HIGH_0("dom_sel: mode not support");
    return;
  }


  /*Store LTE CS capability*/
  sds_ptr->lte_cs_capability = si_info_ptr->lte_cs_capability;
 
  if (cmsds_is_reset_lte_cs_capability(as_id))
  {
    sds_ptr->lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
  }

  if (((si_info_ptr->srv_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
       (si_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS)) &&
       (sds_ptr->is_plmn_blocking_in_progress == FALSE))
  {
      /* Check if device is configued as SMS only. if so then no need of
       ** doing voice domain selection.Assumption of current implementation
       ** is SMS is not mandatory.
       */
    if (cmsds_config->is_sms_only == TRUE)
    {
     CM_MSG_MED_0( "Domsel:Configured as SMS only so skip voice domain selection");

     /* Send no voice support indication */

     cmsds_update_and_send_voice_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_NONE,SYS_SYS_MODE_LTE);

     /* Perform SMS domain selection */

     cmsds_process_sms_domain_selection(ss, SYS_SYS_MODE_LTE);

     return;
    }

    if(!cmsds_is_volte_possible(si_info_ptr->lte_ims_voice_avail, as_id))
    {
      sds_ptr->is_temp_ims_reg_fail = FALSE;
      CM_MSG_MED_0( "Domsel:reset is_temp_ims_reg_fail");
    }

    /* hVOLTE SSAC checking
    ** power up in non-VA or moves to non-VA, switch to SRLTE and reset hystersis timer
    */
    if(cmph_get_volte_sub_info_ptr(as_id)->is_ssac_hvolte)
    {
      if(!cmsds_is_volte_possible(si_info_ptr->lte_ims_voice_avail, as_id))
      {
        cmsds_stop_tssac_timer();
        cmsds_update_va_flag(cmcall_is_ssac_p00(), cmss_ptr()->info.lte_ims_voice_avail,ss);
      }
      else if(cmsds_is_volte_possible(si_info_ptr->lte_ims_voice_avail, as_id))
      {
        /** move to VA. Start hystersis timer Condition: SRLTE mode && previous non-VA && current VA && voice domain != IMS*/
        cmsds_start_tssac_timer(ss, as_id);

        /* if tssac_hys is not expired, skip domain sel */
        if(sds_ptr->tssac_hys > time_get_uptime_secs())
        {
          return;
        }
        else
        {
          /* stop SSAC hystersis timer */
          cmsds_stop_tssac_timer();
        }
      }

    }

    /* Update SIB-8 Status */
    #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
        defined( FEATURE_LTE_TO_1X )
    if ((cmsds_config->voice_domain_pref != SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY) &&
       (cmsds_config->voice_domain_pref != SYS_VOICE_DOMAIN_PREF_NONE))
    {
     cmsds_update_sib8_status(ss, si_info_ptr );
    }
    #endif

    switch(cmsds_config->voice_domain_pref)
    {
     case SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY:
     {
       cmsds_process_srv_ind_ims_ps_voice_only(ss, si_info_ptr);
       break;
     }

     case SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY:
     {
       cmsds_process_srv_ind_cs_voice_only(ss, si_info_ptr);
       break;
     }

     case SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED:
     {
       cmsds_process_srv_ind_ims_ps_voice_preferred(ss, si_info_ptr);
       break;
     }

     case SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED:
     {
       cmsds_process_srv_ind_cs_voice_preferred(ss, si_info_ptr);
       break;
     }

     default:
       CM_MSG_HIGH_2("Domsel:Sub %d invalid vocie domain preferance =%d",
                     as_id, cmsds_config->voice_domain_pref);
       break;
    }
  }
  else
  {
    CM_MSG_LOW_2 ("DOM_SEL: Skip Dmain selection srv_domain %d plmn blocking %d", si_info_ptr->srv_domain, \
                                          sds_ptr->is_plmn_blocking_in_progress );
  }

  cmsds_reset_srv_ind_buf_ptr();
  if(sds_ptr->is_tau_needed)
  {
    cmsds_send_tau_complete(SYS_AS_ID_TO_INST_ID(as_id));
    sds_ptr->is_tau_needed = FALSE;
  }
  return;
} /* cmsds_process_srv_ind */

/*===========================================================================

@FUNCTION cmsds_send_domain_sel_chd_ind

@DESCRIPTION
Send DOM_SEL_CHGD_IND to IMS for a specified service.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_send_domain_sel_chd_ind
(
  sys_cm_domain_sel_domain_e_type new_domain,

  cm_call_type_mask_type call_type_mask,

  sys_sys_mode_e_type  sys_mode,

  sys_modem_as_id_e_type  as_id

)
{
  cm_sds_rsp_u_type  *client_request = NULL;

  CM_MSG_HIGH_2("DOM SEL: DOM_SEL_SHGD_IND new_dom %d call_type %d",\
                new_domain, call_type_mask );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  /*           Dynamically allocate memory for  Dom SEL IND message                   */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  client_request = (cm_sds_rsp_u_type *)cm_mem_malloc
                     (sizeof(cm_sds_rsp_u_type));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  client_request->cm_domain_sel_domain_chg_ind.data.domain     = new_domain;
  client_request->cm_domain_sel_domain_chg_ind.data.call_type_mask |= call_type_mask;
  client_request->cm_domain_sel_domain_chg_ind.data.system     = sys_mode;
  client_request->cm_domain_sel_domain_chg_ind.msg_hdr.inst_id= \
        SYS_AS_ID_TO_INST_ID(as_id);
        
  /*---------------------------------------------------------*/

  CM_MSG_HIGH_2("CM->MSGR: SEND: CHANGED_IND, sys_mode=%d, inst_id=%d",
                   sys_mode,
                   client_request->cm_domain_sel_domain_chg_ind.msg_hdr.inst_id);

  (void) cm_msgr_send( MM_DOM_SEL_DOMAIN_SELECTED_CHANGED_IND, MSGR_MM_DOM_SEL,
                       (msgr_hdr_s*)client_request, sizeof(cm_sds_rsp_u_type) );

  /* Free allocated client message buffer */
  cm_mem_free (client_request);

}
/*===========================================================================

@FUNCTION cmsds_is_domain_valid

@DESCRIPTION
Update and send current voice domain selected.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
boolean cmsds_is_domain_valid(sys_cm_domain_sel_domain_e_type domain)
{
  switch(domain)
  {
    case SYS_CM_DOMAIN_SEL_DOMAIN_NONE:
    case SYS_CM_DOMAIN_SEL_DOMAIN_NO_SMS:
    case SYS_CM_DOMAIN_SEL_DOMAIN_NO_VOICE:
    case SYS_CM_DOMAIN_SEL_DOMAIN_MAX:
      return FALSE;
    case SYS_CM_DOMAIN_SEL_DOMAIN_IMS:
    case SYS_CM_DOMAIN_SEL_DOMAIN_1X:
    case SYS_CM_DOMAIN_SEL_DOMAIN_3GPP:
    case SYS_CM_DOMAIN_SEL_HDR_DOMAIN_1X:
      return TRUE;
    default:
      CM_MSG_HIGH_1("invalid domain %d",domain);
      return FALSE;
  }
}
/*===========================================================================

@FUNCTION cmsds_update_and_send_voice_dom_sel_ind

@DESCRIPTION
Update and send current voice domain selected.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_update_and_send_voice_dom_sel_ind
(

  cm_ss_e_type              ss,
  sys_cm_domain_sel_domain_e_type new_domain,
  sys_sys_mode_e_type           sys_mode
)
{
  cmsds_s_type                           *sds_ptr       = cmsds_ptr();
  boolean update_domain = TRUE;
  cmss_s_type  *ss_ptr = cmss_ptr();
  cm_call_csfb_e_type csfb_type = CM_CALL_CSFB_TYPE_NONE;

  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);

  CM_ASSERT( sds_ptr != NULL );


    /*Current Domain  need not to be updated
          if operating in SVLTE mode and Valid
          Voice Domain found by main stack (which is 1X)
          and LTE is not suitable for Voice .
          ex 1 -
          1)got srv on 1x
          2)updated domin to 1x
          3)got srv on lte ,but ims voicenot avaialble
          4)in this case no need to update the new domain

          ex 2 -
          1)got srv on LTE
          2)updated domin to IMS/CSFB
          3)got srv on 1x
          4)in this case no need to update the domain to 1x ,
             since IMS/CSFB is more prefered
    */

    if((ss_ptr->info.is_hybr_gw_operational &&
         cmph_is_subs_feature_mode_1x_sxlte(as_id) == TRUE &&
        sys_srv_status_is_srv(ss_ptr->info.gw_srv_status) &&
        ss_ptr->info.gw_sys_mode == SYS_SYS_MODE_LTE)
        ||
        (ss == CM_SS_HYBR_1
         && sys_mode == SYS_SYS_MODE_HDR))
    {
      /*1) got srv for 1x , updated dom sel
         2) got srv on lte for hybrid2 , updated dom sel to prefer lte
         3) got another update on as IMS/CSFB voice not available ,
             reset dom sel to none
         4) now ideally we should restore the dom sel values computed at step 1
         5) hence we will run domain sel again on the sys_mode of main stack
            based on  recalculate_domain  */

      if(cmsds_is_domain_valid(new_domain) == FALSE
         &&
         (sys_mode == SYS_SYS_MODE_LTE
          || sys_mode == SYS_SYS_MODE_HDR)
          &&
        sys_srv_status_is_srv(ss_ptr->info.srv_status))
      {
        if(sys_mode == SYS_SYS_MODE_LTE)
        {
          cmsds_update_voice_support_status_on_lte(ss, FALSE);
        }
        cmsds_update_and_send_voice_dom_sel_ind(CM_SS_MAIN, SYS_CM_DOMAIN_SEL_DOMAIN_1X,SYS_SYS_MODE_CDMA);
        return;
      }

  /*got LTE srv and updated a valid domain for LTE.
             later got srv on 1X stack(main stack) but domain sent here is 1X
    then we need not to update it ,since LTE (IMS , 1XCSFB , PPCSFB) is prefered over 1X
    Valid domain for LTE will be 1xcsfb/ppcsfb if csfb is possible and IMS if IMS is 
    possible, hence call respective APIs to check whether IMS or any CSFB is possible */
    if(((cmsds_is_cs_voice_avail(&csfb_type, as_id) == TRUE) ||
         cmsds_is_ims_active(as_id))
           && SYS_SYS_MODE_CDMA == sys_mode)
      {
        update_domain = FALSE;
      }
    }

  if (sds_ptr->curr_voice_dom_selected != new_domain)
  {

    if(update_domain == TRUE)
    {
      CM_MSG_MED_3( "DOM SEL: Update current voice domain selected: Old Dom %d New Dom %d sysMode %d",\
                                        sds_ptr->curr_voice_dom_selected, new_domain,
                                        sys_mode);
      /* Update the current domain selected */
      sds_ptr->curr_voice_dom_selected = new_domain;
      /* CM SS based indication to client */
      cmsds_update_selected_voice_domain(ss, new_domain);
    /* Update Voice support on LTE */
    if(sys_mode == SYS_SYS_MODE_LTE)
    {
        if (cmsds_is_domain_valid(new_domain))
      {
        cmsds_update_voice_support_status_on_lte(ss, TRUE);
      }
      else
      {
        cmsds_update_voice_support_status_on_lte(ss, FALSE);
      }
    }

	 #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
       defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
	  if(sds_ptr->curr_voice_dom_selected == SYS_CM_DOMAIN_SEL_DOMAIN_IMS)
	  {
        /* send SRVCC activation to 1x*/
        cmsds_send_1xsrvcc_activate_req ();
		    CM_MSG_HIGH_1( "DOM SEL: send SRVCC ACT to 1x , voice_domain:%d", sds_ptr->curr_voice_dom_selected );
      }
     #endif

    /* Message router based indication to clients */
    cmsds_send_domain_sel_chd_ind (new_domain, CM_CALL_TYPE_VOICE_MASK, sys_mode, as_id);
    }
  }
  else
  {
    CM_MSG_MED_0("VOICE_DOM_SEL: No change in selected voice domain ");
  }
}

/*===========================================================================

@FUNCTION cmsds_update_and_send_sms_dom_sel_ind

@DESCRIPTION
Update and send current SMS domain selected.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

void cmsds_update_and_send_sms_dom_sel_ind
(
  cm_ss_e_type              ss,
  sys_cm_domain_sel_domain_e_type new_domain,


  sys_sys_mode_e_type           sys_mode
)
{
  cmsds_s_type                           *sds_ptr  = cmsds_ptr();
  /* Domain selection object  */
  
  boolean update_domain = TRUE;
  cmss_s_type  *ss_ptr = cmss_ptr();

  cm_call_csfb_e_type csfb_type = CM_CALL_CSFB_TYPE_NONE;
  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);

  CM_ASSERT( sds_ptr != NULL );

    /*Current Domain  need not to be updated
          if operating in SVLTE mode and Valid SMS Domain found by main stack
          and IMS is not suitable for SMS */

    if((ss_ptr->info.is_hybr_gw_operational &&
         cmph_is_subs_feature_mode_1x_sxlte(as_id) == TRUE &&
        sys_srv_status_is_srv(ss_ptr->info.gw_srv_status) &&
        ss_ptr->info.gw_sys_mode == SYS_SYS_MODE_LTE)
        ||
        (ss == CM_SS_HYBR_1
         && sys_mode == SYS_SYS_MODE_HDR))
      {
      /*1) got srv for 1x , updated dom sel
         2) got srv on lte for hybrid2 , updated dom sel to prefer lte
         3) got another update on lte as SMS not avail, reset dom sel to none
         4) now ideally we should restore the dom sel values computed at step 1
         5) hence we will run domain sel again on the sys_mode of main stack
            based on  recalculate_domain  */
      if(cmsds_is_domain_valid(new_domain) == FALSE
         &&
         (sys_mode == SYS_SYS_MODE_LTE
          || sys_mode == SYS_SYS_MODE_HDR)
          &&
        sys_srv_status_is_srv(ss_ptr->info.srv_status))
      {
        cmsds_update_and_send_sms_dom_sel_ind(CM_SS_MAIN, SYS_CM_DOMAIN_SEL_DOMAIN_1X,SYS_SYS_MODE_CDMA);
        return;
      }

      /*got LTE srv and updated a valid domain for LTE
             later got srv on 1X stack(main stack) but domain sent here is 1X
      then we need not to update it ,since LTE (IMS ) is prefered over 1X
      Valid domain for LTE will be 1xcsfb/ppcsfb if csfb is possible and IMS if IMS is 
      possible, hence call respective APIs to check whether IMS or any CSFB is possible */
    if((cmsds_is_cs_sms_possible(ss) ||
        cmsds_is_ps_sms_possible(SYS_SYS_MODE_LTE, as_id))
           && SYS_SYS_MODE_CDMA == sys_mode)
      {
        update_domain = FALSE;
      }
    }

  if (sds_ptr->curr_sms_dom_selected != new_domain)
  {
    CM_MSG_HIGH_2("DOM SEL: SMS_DOM_SEL_SHGD_IND Old %d New %d",\
                sds_ptr->curr_sms_dom_selected, new_domain );
    if(update_domain == TRUE)
    {
      /* Update the current domain selected */
      cmsds_send_domain_sel_chd_ind (new_domain, CM_CALL_TYPE_SMS_MASK, sys_mode, as_id);
      sds_ptr->curr_sms_dom_selected = new_domain;
      /* CM SS based indication to client */
      cmsds_update_selected_sms_domain(ss, new_domain);
    }

  }
  else
  {
    CM_MSG_MED_0("SMS_DOM_SEL: No change in selected sms domain ");
  }

}

/*===========================================================================
@FUNCTION cmsds_get_call_type_mask_ims

@DESCRIPTION
Returns call_type_mask_according to device configuration

@DEPENDENCIES

None

@RETURNS
  cm_call_type_mask_type

@SIDE_EFFECT

None

===========================================================================*/
cm_call_type_mask_type cmsds_get_call_type_mask_ims
(
    boolean                   is_ims_avail,
    sys_sys_mode_mask_e_type  sys_mode_mask,
    sys_modem_as_id_e_type    as_id
)
{
  cmsds_s_type                         *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/
  cm_call_csfb_e_type                  csfb_type;

  cm_call_type_mask_type               cm_call_type_mask = CM_CALL_TYPE_NONE_MASK;

  cmsds_sub_config_s_type   *cmsds_config = cmsds_get_config_ptr(as_id);
  volte_sub_info_s_type* volte_sub_info_ptr = cmph_get_volte_sub_info_ptr(as_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_MSG_MED_2("get_call_type_mask %d %d",is_ims_avail,sys_mode_mask);
  if(sys_mode_mask & SYS_SYS_MODE_MASK_LTE)
  {
    if (is_ims_avail)
    {
      if ((cmsds_config->is_sms_only == FALSE) &&
           (((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED) &&
             ((cmsds_is_cs_voice_avail(&csfb_type, as_id) == FALSE) || volte_sub_info_ptr->volte_special_pref_setting)) ||
            (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED) ||
            (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY)
           )
         )
      {
        /* Voice over IMS is allowed */
        cm_call_type_mask  |= CM_CALL_TYPE_VOICE_MASK;
        sds_ptr->ims_voice_pref_mask |= SYS_SYS_MODE_MASK_LTE;
      }
    }
    else
    {
      /* Indicate IMS is not available VOICE */
      cm_call_type_mask &= ~ CM_CALL_TYPE_VOICE_MASK;
      sds_ptr->ims_voice_pref_mask &= ~SYS_SYS_MODE_MASK_LTE;
    }

    if (cmsds_config->sms_domain_pref[SYS_SYS_MODE_LTE] == SYS_SMS_DOMAIN_PREF_PS_SMS_PREF)
    {
      /* SMS is preferred over IMS */
      cm_call_type_mask  |= CM_CALL_TYPE_SMS_MASK;
      sds_ptr->ims_sms_pref_mask |= SYS_SYS_MODE_MASK_LTE;

    }
    else
    {
      /* SMS is allowed only over NAS */
      cm_call_type_mask  &= ~CM_CALL_TYPE_SMS_MASK;
      sds_ptr->ims_sms_pref_mask &= ~SYS_SYS_MODE_MASK_LTE;
    }
  }
  /* for eHRPD system, IMS voice always goes to CS. IMS SMS pref depends on NV setting which
     is shared with LTE
   */
  if((sys_mode_mask &  SYS_SYS_MODE_MASK_HDR) &&
         cmss_ptr()->info.hdr_personality == SYS_PERSONALITY_EHRPD)
  {
     /* IMS voice over EHRPD always set to FALSE */
     sds_ptr->ims_voice_pref_mask &= ~SYS_SYS_MODE_MASK_HDR;

    if (cmsds_config->sms_domain_pref[SYS_SYS_MODE_HDR] == SYS_SMS_DOMAIN_PREF_PS_SMS_PREF )
    {
      sds_ptr->ims_sms_pref_mask |= SYS_SYS_MODE_MASK_HDR;
    }
    else
    {
      sds_ptr->ims_sms_pref_mask &= ~SYS_SYS_MODE_MASK_HDR;
    }
  }

  /* Do sms_pref mask calculation for GSM */
  if(sys_mode_mask &  SYS_SYS_MODE_MASK_GSM)
  {
     /* IMS voice over GSM always set to FALSE */
     sds_ptr->ims_voice_pref_mask &= ~SYS_SYS_MODE_MASK_GSM;

    if (cmsds_config->sms_domain_pref[SYS_SYS_MODE_GSM] == SYS_SMS_DOMAIN_PREF_PS_SMS_PREF )
    {
      sds_ptr->ims_sms_pref_mask |= SYS_SYS_MODE_MASK_GSM;
    }
    else
    {
      sds_ptr->ims_sms_pref_mask &= ~SYS_SYS_MODE_MASK_GSM;
    }
  }

  /* Do sms_pref mask calculation for WCDMA */
  if(sys_mode_mask &  SYS_SYS_MODE_MASK_WCDMA)
  {
     /* IMS voice over GSM always set to FALSE */
     sds_ptr->ims_voice_pref_mask &= ~SYS_SYS_MODE_MASK_WCDMA;

    if (cmsds_config->sms_domain_pref[SYS_SYS_MODE_WCDMA] == SYS_SMS_DOMAIN_PREF_PS_SMS_PREF )
    {
      sds_ptr->ims_sms_pref_mask |= SYS_SYS_MODE_MASK_WCDMA;
    }
    else
    {
      sds_ptr->ims_sms_pref_mask &= ~SYS_SYS_MODE_MASK_WCDMA;
    }
  }

  /* Do sms_pref mask calculation for TDSCDMA */
  if(sys_mode_mask &  SYS_SYS_MODE_MASK_TDS)
  {
     /* IMS voice over TDSCDMA always set to FALSE */
     sds_ptr->ims_voice_pref_mask &= ~SYS_SYS_MODE_MASK_TDS;

    if (cmsds_config->sms_domain_pref[SYS_SYS_MODE_TDS] == SYS_SMS_DOMAIN_PREF_PS_SMS_PREF )
    {
      sds_ptr->ims_sms_pref_mask |= SYS_SYS_MODE_MASK_TDS;
    }
    else
    {
      sds_ptr->ims_sms_pref_mask &= ~SYS_SYS_MODE_MASK_TDS;
    }
  }

  /* Do mode mask calculation for 1x */
  if(sys_mode_mask &  SYS_SYS_MODE_MASK_CDMA)
  {
     /* IMS voice over 1X always set to FALSE */
     sds_ptr->ims_voice_pref_mask &= ~SYS_SYS_MODE_MASK_CDMA;
     sds_ptr->ims_sms_pref_mask &= ~SYS_SYS_MODE_MASK_CDMA;
  }

  return cm_call_type_mask;
}

/*===========================================================================
@FUNCTION cmsds_send_ims_pref_changed_ind

@DESCRIPTION
Will update IMS protocol. of IMS preferance for a specified call type.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_send_ims_pref_changed_ind
(
  sys_sys_mode_e_type         system_mode,

  cm_call_type_mask_type      ims_pref_mask,

  cm_call_type_mask_type   call_type_mask,

  sys_modem_as_id_e_type   as_id
)
{
  cm_sds_rsp_u_type  *client_request = NULL;
  cmsds_s_type       *sds_ptr        = cmsds_ptr();
  cm_ss_e_type       lte_ss = cmph_map_sd_ss_type_to_cm_ss_type(cmph_get_sub_with_lte_cap());
  cmss_s_type        *ss_ptr         = cmss_ptr();
  cm_call_csfb_e_type csfb_type = CM_CALL_CSFB_TYPE_NONE;
  cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(as_id);
  sys_ue_mode_e_type ue_mode = state_machine->stack_common_info.ue_mode;
  cmsds_sub_config_s_type *cmsds_config = cmsds_get_config_ptr(as_id);
  cmph_sub_config_s_type *ph_sub_config_ptr = cmph_get_sub_config_ptr(as_id);
  
  CM_MSG_HIGH_3("DOM SEL: IMS_PREF_CHGD_IND ims_pref_mask %d call_type_mask %d, sys_mode=%d", \
               ims_pref_mask, call_type_mask, system_mode);

  if ((system_mode == SYS_SYS_MODE_LTE) &&
      (cmsds_config->is_qip_call_dom_enabled)&&
	  (sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_WLAN] != SYS_IMS_REG_STATE_REGISTERED) &&
      (ims_pref_mask & CM_CALL_TYPE_VOICE_MASK) &&
      ((ue_mode == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED) ||
       (cmsds_is_cs_voice_avail(&csfb_type, as_id)) ||
        (ph_sub_config_ptr->is_cdma_less_device) ) &&
      (cmsds_config->ue_usage_setting == SYS_UE_USAGE_SETTING_VOICE_CENTRIC) &&
      (cmsds_config->ims_reg_status_wait_timer != CMSDS_DISABLE_IMS_REG_STATUS_WAIT_TIMER)
     )
  {
    dword curr_uptime = time_get_uptime_secs();
    if (ims_pref_mask & CM_CALL_TYPE_VOICE_MASK)
    {
      sds_ptr->ims_wait_uptime = curr_uptime + cmsds_config->ims_reg_status_wait_timer;
    }
  }

  CM_MSG_HIGH_4("IMS timer: nv %d ,uptime %d, lte_ims_voice_avail  %d,lte_ims_voice_avail  %d",
    cmsds_config->ims_reg_status_wait_timer,
    sds_ptr->ims_wait_uptime, ss_ptr->info.lte_ims_voice_avail,ss_ptr->info.lte_ims_voice_avail);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  /*           Dynamically allocate memory for  Dom SEL IND message                   */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  client_request = (cm_sds_rsp_u_type *)cm_mem_malloc
                     (sizeof(cm_sds_rsp_u_type));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Is IMS supported */
  client_request->cm_domain_sel_ims_pref_chg_ind.data.ims_pref_mask    = ims_pref_mask;

  client_request->cm_domain_sel_ims_pref_chg_ind.data.call_type_mask = call_type_mask;

  /* Current system */
  client_request->cm_domain_sel_ims_pref_chg_ind.data.system         = system_mode;

  if( (system_mode == SYS_SYS_MODE_LTE) && (ss_ptr->info.lte_ims_voice_avail) )
  {
    client_request->cm_domain_sel_ims_pref_chg_ind.data.lte_ims_voice_avail = TRUE;
  }
  else
  {
    client_request->cm_domain_sel_ims_pref_chg_ind.data.lte_ims_voice_avail = FALSE;
  }

  client_request->cm_domain_sel_ims_pref_chg_ind.msg_hdr.inst_id= \
        SYS_AS_ID_TO_INST_ID(as_id);
        
  (void) cm_msgr_send( MM_DOM_SEL_DOMAIN_SELECTED_IMS_PREF_CHANGED_IND, MSGR_MM_DOM_SEL,
                       (msgr_hdr_s*)client_request, sizeof(cm_sds_rsp_u_type) );
                       
  CM_MSG_HIGH_1("CM->MSGR: SEND: DOMAIN_SELECTED_IMS_PREF_CHANGED_IND, inst_id=%d ",
                  client_request->cm_domain_sel_ims_pref_chg_ind.msg_hdr.inst_id);

  /* Free allocated message buffer */
  cm_mem_free (client_request);
}


/*===========================================================================

@FUNCTION cmsds_map_lte_ims_voice_availability

@DESCRIPTION
Returns IMS VOICE availability based on the service-indication ims_voice
boolean value.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

cm_sds_ims_voice_avail_e_type cmsds_map_lte_ims_voice_availability
(
  boolean is_ims_voice_avail,
  sys_modem_as_id_e_type sub_id
)
{
  cm_sds_ims_voice_avail_e_type ims_voice_avail = CMSDS_IMS_VOICE_NONE;
  boolean is_ssac_p00 = FALSE;

  /* If NV ssac_hvolte is ON, use below logic to decide if VOLTE is possible
  ** VOLTE area (VA): VOPS && non-p00
  ** Non-VOLTE area (Non-VA)): non-VOPS or p00
  */
  if(cmph_get_volte_sub_info_ptr(sub_id)->is_ssac_hvolte)
  {
    is_ssac_p00 = cmcall_is_ssac_p00();
  }

  if (is_ims_voice_avail == TRUE && !is_ssac_p00)
  {
    ims_voice_avail = CMSDS_IMS_VOICE_AVAIL;
  }
  else
  {
    ims_voice_avail = CMSDS_IMS_VOICE_NOT_AVAIL;
  }

  return ims_voice_avail;
}


/*===========================================================================

@FUNCTION cmsds_get_lte_ims_voice_availability

@DESCRIPTION
Returns IMS VOICE availability

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

cm_sds_ims_voice_avail_e_type cmsds_get_lte_ims_voice_availability( )
{
  cmsds_s_type                         *sds_ptr = cmsds_ptr();
  /* Pointer to domain selection object */

  if (sds_ptr == NULL)
  {
    return CMSDS_IMS_VOICE_NONE;
  }

  /* return LTE IMS voice availability */
  return sds_ptr->lte_ims_voice_avail;
}

/*===========================================================================

@FUNCTION cmsds_update_and_send_ims_avail

@DESCRIPTION
Will update IMS protocol. N/W support for IMS over LTE

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

void cmsds_update_and_send_ims_avail
(
  cm_sds_ims_voice_avail_e_type ims_voice_avail,
  sys_sys_mode_e_type           sys_mode,  
  sys_modem_as_id_e_type        as_id
)
{

  cmsds_s_type                         *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  boolean                     is_ims_pref_ind_req = FALSE;
  /* indicates whether IMS PREF_IND is needed or not */

  cm_call_type_mask_type      ims_pref_mask       = CM_CALL_TYPE_NONE_MASK;
  cm_call_type_mask_type      call_type_mask      = CM_CALL_TYPE_NONE_MASK;
  cm_call_csfb_e_type         csfb_type          = CM_CALL_CSFB_TYPE_NONE;
  cmsds_sub_config_s_type     *cmsds_config = cmsds_get_config_ptr(as_id);

  CM_ASSERT( sds_ptr != NULL );

  CM_MSG_MED_4( "Domsel: IMS over mode %d SUPPORT = %d, voice_reg_status = %d, sms_reg_status=%d", 
  				sys_mode, ims_voice_avail,
               sds_ptr->ims_reg_status_voice[sys_mode],
               sds_ptr->ims_reg_status_sms[sys_mode]);


  if (!cmsds_config->is_ims_enabled)
  {
    CM_MSG_MED_1( "Skip IMS_PREF_IND as IMS is not enabled %d", cmsds_config->is_ims_enabled);
    return;
  }

  if(sys_mode == SYS_SYS_MODE_LTE)
  {
  /* Send IMS_PREF_IND for voice services if there is change in VOPS status */
  if ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY) &&
      (sds_ptr->lte_ims_voice_avail == CMSDS_IMS_VOICE_NONE))
  {
    is_ims_pref_ind_req           = TRUE;
    call_type_mask                |= CM_CALL_TYPE_VOICE_MASK;

    /* Donot indicate IMS PREF for voice servcies and don't include voice mask in ims_pref_mask */
    cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_NEEDS_DEREG, SYS_SYS_MODE_LTE);
    sds_ptr->lte_ims_voice_avail  = CMSDS_IMS_VOICE_NOT_AVAIL;
  }
  else if ((sds_ptr->lte_ims_voice_avail != ims_voice_avail) &&
           (cmsds_config->voice_domain_pref != SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY))
  {
    is_ims_pref_ind_req           = TRUE;
    call_type_mask                |= CM_CALL_TYPE_VOICE_MASK;

    if (ims_voice_avail == CMSDS_IMS_VOICE_NOT_AVAIL)
    {
      /* Currently IMS is not available */
      cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_NEEDS_DEREG, SYS_SYS_MODE_LTE);
    }
    else
    {
      /* Currently IMS is available */
      cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_NEEDS_REG, SYS_SYS_MODE_LTE);
      ims_pref_mask |= CM_CALL_TYPE_VOICE_MASK;
    }

    sds_ptr->lte_ims_voice_avail = ims_voice_avail;
  }
  }
  else if (sds_ptr->ims_reg_status_voice[sys_mode] == SYS_IMS_REG_STATE_UNKNOWN)
  {
    cmss_s_type  *ss_ptr = cmss_ptr();
/*
**          1)got srv on LTE
**          2)updated IMS
**          3)got srv on 1x
**          4)in this case no need to update IMS again  */


     if(cmph_is_subs_feature_mode_1x_sxlte(as_id) == FALSE ||
        ss_ptr->info.is_hybr_gw_operational == FALSE ||
        (sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_LTE] != SYS_IMS_REG_STATE_NEEDS_REG &&
         sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_LTE] != SYS_IMS_REG_STATE_REG_IN_PROGRESS &&
         sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_LTE] != SYS_IMS_REG_STATE_REGISTERED))
     {
     is_ims_pref_ind_req           = TRUE;
     call_type_mask                |= CM_CALL_TYPE_VOICE_MASK;
     cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_NEEDS_DEREG, sys_mode);
  }
   }


  /* Will inform IMS of SMS domain pref only during attach. Will not inform SMS domain pref in the subsequent
   ** SERVICE_IND's. "ims_reg_status_sms" will be reset(SYS_IMS_REG_STATE_UNKNOWN) during detach confirmation.
   */
  if (sds_ptr->ims_reg_status_sms[sys_mode] == SYS_IMS_REG_STATE_UNKNOWN)
  {
        cmss_s_type  *ss_ptr = cmss_ptr();
    /*
    **          1)got srv on LTE
    **          2)updated IMS
    **          3)got srv on 1x
    **          4)in this case no need to update IMS again  */
         if(cmph_is_subs_feature_mode_1x_sxlte(as_id) == FALSE ||
            ss_ptr->info.is_hybr_gw_operational == FALSE ||
            sys_mode == SYS_SYS_MODE_LTE ||
            (sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_LTE] != SYS_IMS_REG_STATE_NEEDS_REG &&
             sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_LTE] != SYS_IMS_REG_STATE_REG_IN_PROGRESS &&
             sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_LTE] != SYS_IMS_REG_STATE_REGISTERED))
         {
    is_ims_pref_ind_req           = TRUE;
    call_type_mask                |= CM_CALL_TYPE_SMS_MASK;

    if ((cmsds_config->sms_domain_pref[sys_mode] == SYS_SMS_DOMAIN_PREF_PS_SMS_PREF)&&
       (sys_mode != SYS_SYS_MODE_CDMA))
    {
      sds_ptr->ims_reg_status_sms[sys_mode] = SYS_IMS_REG_STATE_NEEDS_REG;

      ims_pref_mask |= CM_CALL_TYPE_SMS_MASK;
    }
    else
    {
      sds_ptr->ims_reg_status_sms[sys_mode] = SYS_IMS_REG_STATE_DEREGISTERED;
    }
    }
  }

  if (is_ims_pref_ind_req)
  {
    cmsds_send_ims_pref_changed_ind(sys_mode, ims_pref_mask, call_type_mask, as_id);
  }
}

/*===========================================================================

@FUNCTION cmsds_process_no_sms_ind

@DESCRIPTION
Process no sms ind as per is_sms_mandatory configuration.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_process_no_sms_ind
(
  cm_ss_e_type              ss,
  /* Stack on which domain selection to be performed.*/
  sys_sys_mode_e_type sys_mode
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  sys_modem_as_id_e_type  as_id = cmph_map_cm_ss_to_subs(ss);

  cmsds_sub_config_s_type   *cmsds_config = cmsds_get_config_ptr(as_id);

  CM_ASSERT( sds_ptr != NULL );

  CM_MSG_MED_2( "Domsel:Process No SMS Ind: is_sms_mandatory =%d, sys_mode=%d",\
                      cmsds_config->is_sms_mandatory,sys_mode );

  switch(sys_mode)
  {
   case SYS_SYS_MODE_HDR:
   case SYS_SYS_MODE_CDMA:
     sds_ptr->ims_reg_status_sms[sys_mode] = SYS_IMS_REG_STATE_DEREGISTERED;
     cmsds_update_and_send_sms_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_1X,sys_mode);
     return;

   case SYS_SYS_MODE_GSM:
   case SYS_SYS_MODE_WCDMA:
   case SYS_SYS_MODE_TDS:
     sds_ptr->ims_reg_status_sms[sys_mode] = SYS_IMS_REG_STATE_DEREGISTERED;
     cmsds_update_and_send_sms_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_3GPP,sys_mode);
     return;

   default:
     break;
  }

  if ((cmsds_config->is_sms_only == TRUE) ||
      (cmsds_config->is_sms_mandatory == TRUE))
  {

    cmsds_update_and_send_sms_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_NONE,sys_mode);

  }
  else
  {

    cmsds_update_and_send_sms_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_NO_SMS, sys_mode);

  }

  /* Perform PLMN blocking only if SMS is mandatatory or Device is SMS only, and device is not in volte only mode */
  if ((cmsds_config->is_sms_mandatory == TRUE) &&
      (sds_ptr->is_temp_ims_reg_fail == FALSE)&&
       sys_mode == SYS_SYS_MODE_LTE )
  {
    /* Perform PLMN blocking as SMS is not available
       ** PLMN blocking will be performed only when there is no CSFB call
       */
    if((cmcall_are_there_csfb_call_preferences(ss) == FALSE) &&
        (cmcall_is_there_a_call_type(CM_SS_MAX,
                                     CM_CALL_TYPE_EMERGENCY,
                                     CM_CALL_ID_INVALID ) == CM_CALL_ID_INVALID) &&
        (cmph_is_in_emergency_cb() == FALSE)
      )
    {
      cmsds_perform_plmn_blocking(ss, CM_PLMN_BLOCK_REAS_NO_SMS);
    }
    else
    {
      /* Delay the PLMN blocking till CSFB call end */
      sds_ptr->is_plmn_blocking_delayed = TRUE;
      CM_MSG_HIGH_1( "SMS_DOM_SEL: Delay PLMN blocking till CSFB call end =%d",\
                     cmsds_config->is_sms_mandatory );
    }
  }
  else
  {
    CM_MSG_MED_0( "SMS_DOM_SEL: SMS Domsel:Stay on LTE as SMS is not Mandatory");
  }
}

/*===========================================================================

@FUNCTION cmsds_sms_dom_sel_process_no_ims

@DESCRIPTION
SMS over IMS is not possible. Look for SMS over CS domain and perform
PLMN blocking & DETACH if CS SMS is not possible

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_sms_dom_sel_process_no_ims
(

   cm_ss_e_type              ss,
  /* stack on which domain selection to be performed.*/
   sys_sys_mode_e_type       sys_mode
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/
  /*lint -esym(529, sds_ptr) not referenced */

  cmph_s_type     *ph_ptr  = cmph_ptr();
  /* Pointer to phone object. .*/

  sd_si_info_s_type   *si_info_ptr;

  sys_modem_as_id_e_type asubs_id = cmph_map_cm_ss_to_subs(ss);

  cmsds_sub_config_s_type   *cmsds_config = cmsds_get_config_ptr(asubs_id);

  CM_ASSERT( sds_ptr != NULL );

  CM_MSG_HIGH_2( "SMS DOM SEL: No IMS Suppport for SMS.ph srv_dom_pef %d, ss %d", 
                  CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr), ss);

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) && \
       defined( FEATURE_LTE_TO_1X )
  if ((sds_ptr->onexcsfb_status_not_avail_uptime != CMSDS_UPTIME_NONE) &&
      (sds_ptr->is_plmn_blocking_delayed == FALSE))
  {
    CM_MSG_HIGH_1( "SMS DOM SEL: skip 1x SMS domain sel as CSFB STATUS is not yet received %d",\
                  sds_ptr->onexcsfb_status_not_avail_uptime );
    return;
  }
  #endif

  /* SMS domain selection for non-LTE sys */
  if(sys_mode != SYS_SYS_MODE_LTE)
  {
    cmsds_process_no_sms_ind(ss, sys_mode);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* LTE SMS domain selection
  ** Check for SMS over SG's support
  */
  if(sds_ptr->srv_ind_buf_ptr != NULL)
  {
    si_info_ptr = sds_ptr->srv_ind_buf_ptr;
  }
  else
  {
    si_info_ptr = cmss_intl_srv_info_ptr(ss);
  }
  if((si_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS) &&
    (si_info_ptr->mode   ==  SYS_SYS_MODE_LTE))
  {
    if ((sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_NO_VALUE) ||
        (sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_SMS_ONLY) ||
        (sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_CSFB_NOT_PREFFERED))
    {
      /* Indicate SMS over Sg's support for SMS service */
      cmsds_update_and_send_sms_dom_sel_ind (ss, SYS_CM_DOMAIN_SEL_DOMAIN_3GPP, sys_mode);
    }
    else
    {
      /* No CS SMS support. Decide to perform PLMN blocking */
      cmsds_process_no_sms_ind(ss, sys_mode);
    }
  }
  else if((CM_SRV_DOMAIN_PREF_CS_PS == CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr)) &&
          (SYS_SRV_DOMAIN_PS_ONLY == si_info_ptr->srv_domain))
  {
    /* For temporary EMM reject causes on CS domain, do not disable LTE */
    if (SYS_SRV_STATUS_LIMITED_REGIONAL == si_info_ptr->cs_srv_status)
    {
      /* Indicate SMS over Sg's support for SMS service */
      cmsds_update_and_send_sms_dom_sel_ind (ss, SYS_CM_DOMAIN_SEL_DOMAIN_NO_SMS, sys_mode);
    }
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) && \
         defined( FEATURE_LTE_TO_1X )
    else if(((cmsds_get_csfb_type_allowed(asubs_id) == CM_CALL_CSFB_TYPE_1XCSFB) ||
             (cmsds_is_1xcsfb_activation_allowed(asubs_id) == TRUE)) &&
            (cmsds_config->is_sms_over_s102_supported == TRUE))
    {
      /* Indicate SMS over S102s support for SMS service */
      cmsds_update_and_send_sms_dom_sel_ind (ss, SYS_CM_DOMAIN_SEL_DOMAIN_1X,sys_mode);
    }
    #endif
    else
    {
      /* Both SMS over SG's and IMS are not available for  SMS  services */
      cmsds_process_no_sms_ind(ss, sys_mode);
    }
  }
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) && \
     defined( FEATURE_LTE_TO_1X )
  else if((CM_SRV_DOMAIN_PREF_PS_ONLY == CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr)) &&
          (SYS_SRV_DOMAIN_PS_ONLY == si_info_ptr->srv_domain) &&
          (cmsds_config->is_sms_over_s102_supported == TRUE)                      &&
          ((cmsds_get_csfb_type_allowed(asubs_id) == CM_CALL_CSFB_TYPE_1XCSFB) ||
           (cmsds_is_1xcsfb_activation_allowed(asubs_id) == TRUE))
         )
  {
    /* Indicate SMS over S102s support for SMS service */
    cmsds_update_and_send_sms_dom_sel_ind (ss, SYS_CM_DOMAIN_SEL_DOMAIN_1X, sys_mode);
  }
  #endif
  else
  {
    /* Both SMS over SG's and IMS are not available for  SMS  services */
    cmsds_process_no_sms_ind(ss, sys_mode);
  }
}
/*lint +esym(529, sds_ptr) not referenced */
/*lint +esym(529, ss_ptr) not referenced */

/*===========================================================================

@FUNCTION cmsds_sms_dom_sel_process_ims_reg_status

@DESCRIPTION
Performs SMS domain selection based on IMS registration status.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_sms_dom_sel_process_ims_reg_status
(
  sys_sys_mode_e_type                   sys_mode,
  cm_domain_sel_ims_reg_status_s_type  *ims_reg_status,
  sys_modem_as_id_e_type                as_id
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  cmss_s_type    *ss_ptr   = cmss_ptr();
  cm_ss_e_type   ss        = CM_SS_MAIN;

  CM_ASSERT( sds_ptr != NULL );

  if((sys_mode == SYS_SYS_MODE_HDR) && (ss_ptr->info.hdr_hybrid))
  {
    ss = CM_SS_HYBR_1;
  }

  /* For multi-sim targets, update the ss. */
  else if (sys_mode == SYS_SYS_MODE_LTE)
  {
    ss = cmph_determine_ss_per_rat(as_id, SYS_SYS_MODE_LTE);
    CM_MSG_MED_2("LTE_SS, ss=%d, asubs_id=%d",ss, as_id);
  }
  else
  {
    ss = cmph_map_as_id_to_cm_ss(as_id);
  }

  CM_MSG_HIGH_3("SMS DOM SEL: Act on IMS_REG STATUS, is_registered=%d, sys_mode=%d, ss=%d ",ims_reg_status->is_registered, sys_mode,ss);

  if (cmsds_get_config_ptr(as_id)->sms_domain_pref[sys_mode] == SYS_SMS_DOMAIN_PREF_PS_SMS_PREF &&
     sys_mode != SYS_SYS_MODE_CDMA)
  {
    if (ims_reg_status->is_registered == TRUE)
    {
      /* Set the IMS registration status */
      sds_ptr->ims_reg_status_sms[sys_mode] = SYS_IMS_REG_STATE_REGISTERED;
      sds_ptr->is_temp_ims_reg_fail = FALSE;

      cmsds_update_and_send_sms_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_IMS, sys_mode);
    }
    else
    {
      /* Set the IMS registration status */
      sds_ptr->ims_reg_status_sms[sys_mode] = SYS_IMS_REG_STATE_DEREGISTERED;

      if (ims_reg_status->end_cause == SYS_IMS_REG_END_CAUSE_TEMPORARY)
      {
        sds_ptr->is_temp_ims_reg_fail = TRUE;
      }
      else
      {
        sds_ptr->is_temp_ims_reg_fail = FALSE;
      }
      /* Check for SMS support over "SMS over SG's "/1x */
      cmsds_sms_dom_sel_process_no_ims(ss, sys_mode);

      /*  in svlte - we might have skipped IMS-PREF-CHG on 1x ,
        ** since at that time , LTE reg status could be seen as possible .
        ** but know we recieved LTE reg status fail , we should send clients IMS-PREF-CHG on 1x
        */
      if(cmph_is_subs_feature_mode_1x_sxlte(as_id) == TRUE &&
         ss_ptr->info.is_hybr_gw_operational == TRUE &&
         ss_ptr->info.srv_status == SYS_SRV_STATUS_SRV &&
         ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA &&
         sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_CDMA] == SYS_IMS_REG_STATE_UNKNOWN &&
         sys_mode == SYS_SYS_MODE_LTE)
      {
        cmsds_update_and_send_ims_avail(FALSE,SYS_SYS_MODE_CDMA, as_id);
    }
  }
  }
  else
  {
    cmsds_sms_dom_sel_process_no_ims(ss, sys_mode);
  }
}

/*===========================================================================

@FUNCTION cmsds_voice_dom_sel_cs_voice_only

@DESCRIPTION
Perform voice domain selection in CS_VOICE_ONLY configuration

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_voice_dom_sel_cs_voice_only
(
  cm_ss_e_type              ss,

  const sd_si_info_s_type  *si_info_ptr
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  cmph_s_type     *ph_ptr  = cmph_ptr();
  /* Pointer to phone object. .*/

  sd_ss_e_type     sd_ss        = cmph_map_cm_ss_type_to_sd_ss_type(ss);
  sys_modem_as_id_e_type asubs_id = cmph_map_cm_ss_to_subs(ss);

  cmsds_sub_config_s_type   *cmsds_config = cmsds_get_config_ptr(asubs_id);

  CM_ASSERT( sds_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );

  if(((si_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS &&
       si_info_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_CSFB_NOT_PREFFERED) 
          ||
      (si_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS &&
       si_info_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_SMS_ONLY)           
          ||
      ((si_info_ptr->srv_domain == SYS_SRV_DOMAIN_PS_ONLY  ||
        si_info_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_BARRED ) &&
       ((ss == CM_SS_MAIN && ph_ptr->main_stack_info.pref_info.srv_domain_pref== CM_SRV_DOMAIN_PREF_CS_PS)
        || (ss == CM_SS_HYBR_2 && ph_ptr->hybr_2_stack_info.pref_info.srv_domain_pref == CM_SRV_DOMAIN_PREF_CS_PS)
        #if defined(FEATURE_MMODE_SXLTE_G)
        || (ss == CM_SS_HYBR_3 && ph_ptr->hybr_3_stack_info.pref_info.srv_domain_pref== CM_SRV_DOMAIN_PREF_CS_PS)
        #endif
        ))
    ) &&
    cmsds_config->ue_usage_setting == SYS_UE_USAGE_SETTING_VOICE_CENTRIC )
  {

    /* 3GPP spec 24.301 sec 5.5.1.3.4: If the ATTACH ACCEPT message includes the Additional
    ** update result IE with value "SMS only" or "CS Fallback not preferred", a UE operating
    ** in CS/PS mode 1 with "IMS voice not available" shall attempt to select GERAN or UTRAN
    ** radio access technology rather than E-UTRAN for the selected PLMN or equivalent PLMN
    */

    /* 3GPP CSFB is not possible now check if 1XCSFB is available */
    #ifdef FEATURE_LTE_TO_1X
    if ((sds_ptr->csfb_status == CM_1XCSFB_AVAIL ||
         sds_ptr->csfb_status == CM_1XCSFB_PREREG_SUCCESS ) &&
         (sds_ptr->sib8_available == TRUE))
    {
       CM_MSG_MED_3("Domsel:Stay on LTE using 1XCSFB for voice (ue_usage_setting=%d,srv_domain=%d,sim_state=%d)",
                    cmsds_config->ue_usage_setting,\
                si_info_ptr->srv_domain,\
                sd_ss_gw_get_sim_state(sd_ss) );

       /* Update and send voice domain selected to IMS */
       cmsds_update_and_send_voice_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_1X, SYS_SYS_MODE_LTE);
       return;

    }
    #endif /* FEATURE_LTE_TO_1X */

    /* Do not disable LTE for EMM temporary CS registration failures */
    if (si_info_ptr->srv_domain == SYS_SRV_DOMAIN_PS_ONLY &&
        si_info_ptr->cs_srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL &&
        si_info_ptr->mode          == SD_MODE_LTE &&
        sd_ss_gw_get_sim_state(sd_ss) != SYS_SIM_STATE_CS_INVALID)
    {
      CM_MSG_MED_3("Domsel:Stay on LTE (ue_usage_setting=%d,srv_domain=%d cs_srv_status = %d)",
                cmsds_config->ue_usage_setting,\
                si_info_ptr->srv_domain,\
                si_info_ptr->cs_srv_status );
      return;
    }
    /* Waiting for 1XCSFB status update from 1XCP
    ** So skip action now
    */
    #ifdef FEATURE_LTE_TO_1X
    if ((sds_ptr->sib8_available == TRUE) &&
        ((sds_ptr->csfb_status == CM_1XCSFB_NOT_AVAIL) ||
         (sds_ptr->csfb_status == CM_1XCSFB_NONE)))
    {
      if (sds_ptr->onexcsfb_status_not_avail_uptime != CMSDS_UPTIME_NONE)
      {
        CM_MSG_HIGH_1("Domsel:1XCSFB not avail yet waiting upto uptime %d",
                      sds_ptr->onexcsfb_status_not_avail_uptime);
        return;
      }
      else if (cmsds_is_wait_for_1xcsfb_status(asubs_id) == TRUE)
      {
        sds_ptr->onexcsfb_status_not_avail_uptime = (time_get_uptime_secs()
                                         + sds_ptr->sib8_not_avail_duration);

        CM_MSG_HIGH_1("Domsel:1XCSFB not avail yet waiting upto uptime %d",
                             sds_ptr->onexcsfb_status_not_avail_uptime);

        return;
      }
    }
    if (sds_ptr->is_dual_rx_1xcsfb == TRUE)
    {
      /* Dual Rx 1XCSFB enabled so no need of checking for CSFB status */
      return ;
    }
    #endif
    CM_MSG_HIGH_3( "Domsel:Leave LTE for CS RATs (ue_usage_setting=%d,srv_domain=%d,lte_cs_capability=%d)",\
            cmsds_config->ue_usage_setting,si_info_ptr->srv_domain,si_info_ptr->lte_cs_capability);

    /* Disable LTE and send NO_VOICE/SMS_IND to IMS */
    cmsds_process_voice_not_avail_ind (ss);
    return;

  }

  #ifdef FEATURE_LTE_TO_1X
  /* PS only configuration is used for 1XCSFB feature
  ** But SIB8 itself is not available so we can conclude that 1XCSFB
  ** not available and PP CSFB anyways not avail in PS only mode
  ** Since device is voice centric, disable LTE now
          */
  else if (si_info_ptr->srv_domain == SYS_SRV_DOMAIN_PS_ONLY &&
           asubs_id == SYS_MODEM_AS_ID_1 &&
           ph_ptr->main_stack_info.pref_info.srv_domain_pref == CM_SRV_DOMAIN_PREF_PS_ONLY &&
           ((si_info_ptr->sib8_available == SD_SIB8_NOT_AVAIL )||
            (sds_ptr->csfb_status == CM_1XCSFB_NOT_AVAIL)) &&
           cmsds_config->ue_usage_setting == SYS_UE_USAGE_SETTING_VOICE_CENTRIC
          )
        {
           CM_MSG_MED_3("Domsel: Leave LTE, no SIB8, PS only, ue_usage_setting %d sib8_available %d srv_domain_pref %d",
                        cmsds_config->ue_usage_setting,
                        sds_ptr->sib8_available,
                        ph_ptr->main_stack_info.pref_info.srv_domain_pref);

          /* Disable LTE and send NO_VOICE/SMS_IND to IMS */
          cmsds_process_voice_not_avail_ind (ss);
          return;
        }
  #endif
  else if (si_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS &&
           (si_info_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_CSFB_NOT_PREFFERED ||
            si_info_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_NO_VALUE) &&
           cmsds_config->ue_usage_setting == SYS_UE_USAGE_SETTING_DATA_CENTRIC
          )
  {

    /* 3GPP spec 24.301 sec 5.5.1.3.4:If the ATTACH ACCEPT message includes the Additional
    ** update result IE with value "CS Fallback not preferred", this indicates to a UE
    ** operating in CS/PS mode 2 that it is attached for EPS and non-EPS services and that it
    ** can use CS fallback.
    */

    CM_MSG_MED_3( "Domsel:Stay on LTE and use CSFB for VOICE Domsel:ue_usage=%d,srv_domain=%d,lte_cs_capability=%d",\
    			  cmsds_config->ue_usage_setting, si_info_ptr->srv_domain, \
                  si_info_ptr->lte_cs_capability);
  }
  else if (((si_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS &&
             si_info_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_SMS_ONLY) ||
            (si_info_ptr->srv_domain == SYS_SRV_DOMAIN_PS_ONLY ))
                             &&
           cmsds_config->ue_usage_setting == SYS_UE_USAGE_SETTING_DATA_CENTRIC )
  {
    /* 3GPP spec 24.301 sec 5.5.1.3.4: If the ATTACH ACCEPT message includes the Additional update
       ** result IE with value "SMS only", a UE operating in CS/PS mode 2 shall not attempt to use CS
       ** fallback for mobile originating services.
       */
    #ifdef FEATURE_LTE_TO_1X
    if((FALSE == sds_ptr->sib8_available) &&
        (CM_1XCSFB_NONE == sds_ptr->csfb_status))
    {
    #endif

      CM_MSG_LOW_3( "Domsel: Stay on LTE and Indicate NO VOICE support ue_usage=%d, srv_domain=%d, lte_cs_capability=%d",\
                    cmsds_config->ue_usage_setting, si_info_ptr->srv_domain, \
                    si_info_ptr->lte_cs_capability);

      /* Update and send voice domain selected to IMS */
      cmsds_update_and_send_voice_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_NO_VOICE, SYS_SYS_MODE_LTE);
    #ifdef FEATURE_LTE_TO_1X
    }
    #endif

    return;
  }

  /* If CSFB is allowed send domain selection indication as 3GPP */
  if ((cmsds_get_csfb_type_allowed(asubs_id) == CM_CALL_CSFB_TYPE_PPCSFB) ||
      ((sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_CSFB_NOT_PREFFERED) &&
        (cmsds_config->ue_usage_setting == SYS_UE_USAGE_SETTING_DATA_CENTRIC)))
  {
    /* Update and send voice domain selected to IMS */
    cmsds_update_and_send_voice_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_3GPP, SYS_SYS_MODE_LTE);

  }

  /* If CSFB TYPE is 1X, no need to send the voice dom_sel_ind here. because it will be sent at PRE_REG_SUCCESS step*/
}


/*===========================================================================

FUNCTION cmsds_ims_reg_status_wait_timer_clear

DESCRIPTION
  Clears IMS wait timer

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/

void cmsds_ims_reg_status_wait_timer_clear(cm_call_type_mask_type ims_pref_mask)
{
  cmsds_s_type        *sds_ptr  = cmsds_ptr();

  if (ims_pref_mask & CM_CALL_TYPE_VOICE_MASK)
  {
    sds_ptr->ims_wait_uptime = 0;
  }

}


/*===========================================================================

FUNCTION cmsds_process_ims_reg_status_wlan

DESCRIPTION
  Process IJMS registration status on wlan.

DEPENDENCIES
None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmsds_process_ims_reg_status_wlan(

    const cm_sds_cmd_u_type   *cm_cmd_ptr
      /* Pointer to LL report */
)
{

  if (cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.call_type_mask
         & CM_CALL_TYPE_VOICE_MASK)
  {
    if(cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.is_registered)
    {
      cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_REGISTERED, SYS_SYS_MODE_WLAN);

    }
    else
    {
      cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_DEREGISTERED, SYS_SYS_MODE_WLAN);
    }
  }
}
/*===========================================================================

@FUNCTION cmsds_skip_ims_reg_status_wait_timer_clear

@DESCRIPTION
To evaluate if IMS REG Status wait timer can be cleared.

@DEPENDENCIES
 
None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
static boolean cmsds_skip_ims_reg_status_wait_timer_clear
(
  const cm_domain_sel_ims_reg_status_s_type *reg_status_ptr,

  sys_modem_as_id_e_type asubs_id

)
{
  boolean ret_val = FALSE;
  cmph_sub_config_s_type *ph_sub_config_ptr;
  
  ph_sub_config_ptr = cmph_get_sub_config_ptr(asubs_id);

  if(ph_sub_config_ptr == NULL)
  {
    CM_MSG_HIGH_1("cmph sub config pointer on asubs_id %d is NULL",asubs_id);
    return FALSE;
  }
  
  if(ph_sub_config_ptr->is_cdma_less_device &&
     reg_status_ptr->is_registered == FALSE &&
     reg_status_ptr->end_cause == SYS_IMS_REG_END_CAUSE_TEMPORARY)
  {
    CM_MSG_HIGH_0("Skip ims reg status wait timer clear");
    ret_val = TRUE;
  }
  return ret_val;
}

/*===========================================================================

FUNCTION cmsds_process_ims_reg_status

DESCRIPTION
  Process IJMS registration status.

DEPENDENCIES
None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmsds_process_ims_reg_status(

    const cm_sds_cmd_u_type   *cm_cmd_ptr
      /* Pointer to LL report */
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  cmss_s_type    *ss_ptr   = cmss_ptr();
  /* Serving system object */
  cmph_s_type    *ph_ptr   = cmph_ptr();
  /* Pointer to phone object.*/
  cm_ss_e_type   ss;

  sd_si_info_s_type   *si_info_ptr;
  sys_modem_as_id_e_type asubs_id;

  cmsds_sub_config_s_type   *cmsds_config;

  CM_ASSERT( cm_cmd_ptr != NULL );
  CM_ASSERT( sds_ptr != NULL );
  CM_ASSERT( ss_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );

  asubs_id = cmph_get_as_id_from_ll_ind(
      INST_ID_TO_SYS_AS_ID(cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.msg_hdr.inst_id));

  cmsds_config = cmsds_get_config_ptr(asubs_id);

  CM_MSG_HIGH_3("IMS_REG: RXD: IMS_REG_STATUS_IND, IMS REG status %d call_type_mask%d, sys_mode=%d", \
    cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.is_registered,\
    cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.call_type_mask,
    cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.system);
  
  if(cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.system == SYS_SYS_MODE_WLAN)
  {
	cmsds_process_ims_reg_status_wlan(cm_cmd_ptr);
    return;
  }

  if (asubs_id == SYS_MODEM_AS_ID_1)
  {
    ss = cmph_determine_ss_per_rat(asubs_id, cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.system);
  }
  else
  {
    ss = cmph_map_as_id_to_cm_ss(asubs_id);
  }

  CM_MSG_HIGH_3("IMS_REG: RXD: IMS_REG_STATUS_IND, inst_id=%d, asubs_id=%d, lte as_id=%d", 
                 cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.msg_hdr.inst_id,
                 asubs_id,
                 cmph_get_sub_with_lte_cap());

  if(!(cmsds_skip_ims_reg_status_wait_timer_clear(&(cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data),asubs_id)) )
  {
    cmsds_ims_reg_status_wait_timer_clear(cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.call_type_mask);
  }



  if (ss == CM_SS_MAX)
  {
    CM_MSG_HIGH_0("Ignore IMS REG STATUS as reported system is not part of mode_pref ");
    return;
  }

  /* If UE is not ONLINE - do not process IMS REG status  */
  if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE   ||
      cm_reply_check (&ph_ptr->reply, CM_LPM_OK_F)||
      cm_reply_check (&ph_ptr->reply, CM_RESET_OK_F) ||
      cm_reply_check (&ph_ptr->reply, CM_PWROFF_OK_F) ||
#if defined(FEATURE_IP_CALL)
      cmipapp_is_srv_dom_ps_detach() ||
#endif     
      cmclnup_is_ongoing_subsc(BM(asubs_id)) ||
      ph_ptr->is_oprt_mode_change_ongoing == TRUE
  )
  {
    CM_MSG_HIGH_2("Ignore IMS REG STATUS as reported oprt_mode=%d oprt_mode_ongoing=%d",
                                                    ph_ptr->oprt_mode,
                                                    ph_ptr->is_oprt_mode_change_ongoing);
    return;
  }

  if(!cmph_is_lte_capable_on_sub(asubs_id))
  {
    CM_MSG_HIGH_1("Ignore IMS REG status: asubs_id %d does not have LTE in max_cap",
                  asubs_id);
    return;
  }
  /* Adding a defensive check to not process it if IMS sends REG STATUS IND even 
  ** when subscription for a slot has become un-available .
  ** Ideally, IMS shouldnt sent it in first place
  */
  if(!cmph_is_gw_subsc_avail(asubs_id))
  {
    CM_MSG_HIGH_1("Ignore IMS REG status: asubs_id %d gwl subscription not available",
    			asubs_id);
    return;
  }

  /* Ignore IMS REG STATUS, if mode sent in REG_STATUS
  ** is different from currently camped RAT or CM doesn't have FULL SRV.
  */
  if(sds_ptr->srv_ind_buf_ptr != NULL &&
    cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.system == SYS_SYS_MODE_LTE)
  {
    si_info_ptr = sds_ptr->srv_ind_buf_ptr;
  }
  else
  {
    si_info_ptr = cmss_intl_srv_info_ptr(ss);
  }
  if ((cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.system != si_info_ptr->mode) ||
       (si_info_ptr->srv_status != SYS_SRV_STATUS_SRV)||
      (si_info_ptr->srv_domain == SYS_SRV_DOMAIN_CAMPED)||
       ( si_info_ptr->mode == SYS_SYS_MODE_LTE 
	       && (!( si_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS  ||
              si_info_ptr->srv_domain == SYS_SRV_DOMAIN_PS_ONLY ))
       )                                                               ||
       ( cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.system == SYS_SYS_MODE_LTE
         && cmregprx_get_stack_info_ptr(ss)->substate == CMREGPRX_SUBSTATE_WAIT_PS_DETACH_CNF)

     )
  {
    CM_MSG_HIGH_2("Ignore IMS REG status , lack  FULL SRV or sys_mode mismatch, sys_mode %d srv_status %d",\
    si_info_ptr->mode, si_info_ptr->srv_status );

    return;
  }

  if((sd_ss_get_supp_mode_pref(
                   cmph_map_cm_mode_pref_to_sd_mode_pref(CMPH_SS_MODE_PREF(asubs_id, cmph_ptr())),
                   cmph_map_cm_band_pref_to_sd_band_pref(CMPH_SS_BAND_PREF(asubs_id, cmph_ptr())),
                   CMPH_SS_LTE_BAND_PREF(cmph_map_cm_ss_to_subs(ss), cmph_ptr()),
                   cmph_map_cm_tds_band_pref_to_sd_tds_band_pref(CMPH_SS_TDS_BAND_PREF(asubs_id, cmph_ptr()))) 
                   & SD_SS_MODE_PREF_LTE) == SD_SS_MODE_PREF_NONE)
  {
    CM_MSG_HIGH_1("CM's mode %d and lte_band_pref doesnt support LTE",\
    CMPH_SS_MODE_PREF(cmph_map_cm_ss_to_subs(ss), cmph_ptr()));
    return;
  }

  /* Reset domain selections mask's. While processing IMS Registration status we may need to
        send SS event */

  ss_ptr->info.changed_fields2 = 0;
  ss_ptr->info.hybr_gw_changed_fields =0;
  ss_ptr->info.gw3_changed_fields =0;

  /* Perform VOICE domain selection */
  if(cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.system == SYS_SYS_MODE_LTE)
  {
    if (cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.call_type_mask
         & CM_CALL_TYPE_VOICE_MASK)
    {
      cmsds_voice_dom_sel_process_ims_reg_status
        (&(cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data), asubs_id);
    }

  /* Perform SMS domain selection if LTE is not disabled*/
  if ((sds_ptr->is_plmn_blocking_in_progress == FALSE) &&
      (cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.call_type_mask
      & CM_CALL_TYPE_SMS_MASK) &&
      cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.system == SYS_SYS_MODE_LTE)
    {
      /* If device is voice centric and IMS registration is failed for voice services, then CM should not
         ** perform SMS domain selection as IMS will perform PLMN blocking and PS detach.
         ** If CM performs SMS domain selection, it may result in anoher PLMN blocking command.
         */
      if (((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY) ||
           (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED)) &&
           (sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_LTE] == SYS_IMS_REG_STATE_DEREGISTERED) &&
           (cmsds_config->ue_usage_setting == SYS_UE_USAGE_SETTING_VOICE_CENTRIC)
         )
      {
        CM_MSG_HIGH_0("SMS_DOM_SEL: Skip SMS DOM SEL IMS will perform PLMN blocking");
        return;
      }

      cmsds_sms_dom_sel_process_ims_reg_status
      (SYS_SYS_MODE_LTE,(cm_domain_sel_ims_reg_status_s_type *)&(cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data),
       asubs_id);
    }
  }
   /* Process voice/SMS IMS reg status for other RATs */
  else
  {
    if (cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.call_type_mask
         & CM_CALL_TYPE_VOICE_MASK)
    {
      cmsds_voice_dom_sel_process_ims_reg_status
        (&(cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data), asubs_id);
    }

    if(cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.call_type_mask
          & CM_CALL_TYPE_SMS_MASK)
    {
      cmsds_sms_dom_sel_process_ims_reg_status
        (cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data.system,
        (cm_domain_sel_ims_reg_status_s_type *)&(cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.data),
        asubs_id);
    }
  }

  /* Send SS event if selected voice/SMS domains are changed */
  if ((ss_ptr->info.changed_fields2 & CM_SS_LTE_VOICE_DOMAIN_CHANGED_MASK) ||
      (ss_ptr->info.changed_fields2 & CM_SS_LTE_SMS_DOMAIN_CHANGED_MASK)
     )
  {
    cmss_event(CM_SS_MAIN, CM_SS_EVENT_SRV_CHANGED);
  }

  if ((ss_ptr->info.hybr_gw_changed_fields & CM_SS_GW_LTE_VOICE_DOMAIN_CHANGED_MASK)||
      (ss_ptr->info.hybr_gw_changed_fields & CM_SS_GW_LTE_SMS_DOMAIN_CHANGED_MASK))
  {
    cmss_event(CM_SS_HYBR_2, CM_SS_EVENT_SRV_CHANGED);
  }

  if ((ss_ptr->info.gw3_changed_fields & CM_SS_GW_LTE_VOICE_DOMAIN_CHANGED_MASK)||
      (ss_ptr->info.gw3_changed_fields & CM_SS_GW_LTE_SMS_DOMAIN_CHANGED_MASK))
  {
    cmss_event(CM_SS_HYBR_3, CM_SS_EVENT_SRV_CHANGED);
  }
}

/*===========================================================================

FUNCTION cmcsfbcall_rpt_proc

DESCRIPTION
  Process lower layer reports (i.e. replies to  call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsds_ims_cmd_proc(

  const cm_sds_cmd_u_type   *cm_cmd_ptr
    /* Pointer to LL report */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do command specific processing as follows:
  ** 1. Update appropriate call object fields.
  ** 2. Modify the call state if necessary.
  ** 3. Notifying clients of event if necessary.
  */

  switch( cm_cmd_ptr->msg_hdr.id )
  {

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case MM_DOM_SEL_DOMAIN_SELECTED_GET_REQ:
    {  
      uint8 inst_id = cm_cmd_ptr->cm_domain_sel_domain_get_req_msg.msg_hdr.inst_id;
      CM_MSG_HIGH_1("MSGR->CM: RXD: GET_REQ, inst_id=%d",inst_id);
      cmsds_process_dom_sel_get_req (&(cm_cmd_ptr->cm_domain_sel_domain_get_req_msg.data),
                                     inst_id);
      break;
    }

    case MM_DOM_SEL_DOMAIN_SELECTED_LTE_IMS_PREF_GET_REQ:
    {
      uint8 inst_id = cm_cmd_ptr->cm_domain_sel_ims_pref_get_req.msg_hdr.inst_id;      
      CM_MSG_HIGH_1("MSGR->CM: RXD: LTE_IMS_PREF_GET_REQ, inst_id=%d",inst_id);
      cmsds_process_dom_sel_ims_pref_get_req (inst_id);
      break;
    }

    case MM_DOM_SEL_UPDATE_IMS_REG_STATUS_IND:
    {
      uint8 inst_id = cm_cmd_ptr->cm_domain_sel_update_ims_reg_status_Ind.msg_hdr.inst_id; 
      CM_MSG_HIGH_1("MSGR->CM: RXD: IMS->CM: IMS_REG_STATUS_IND, inst_id=%d",inst_id);
      cmsds_process_ims_reg_status (cm_cmd_ptr);
    }
    break;

    case MM_DOM_SEL_DOMAIN_SELECTED_IMS_PREF_GET_EXT_REQ:
    {
      uint8 inst_id = cm_cmd_ptr->cm_domain_sel_get_ims_pref_req_msg.hdr.inst_id;
      CM_MSG_HIGH_1("MSGR->CM: RXD: IMS_PREF_GET_EXT_REQ, inst_id=%d",inst_id);
      cmsds_process_dom_sel_ims_pref_get_req_ext (inst_id);
      break;
    }

    default:
       CM_MSG_HIGH_1("MSGR->CM: Invalid message %d", cm_cmd_ptr->msg_hdr.id);
       break;
  }
}

/*===========================================================================

FUNCTION cmsds_client_cmd_proc

DESCRIPTION
  Process client commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsds_client_cmd_proc(

  const cm_msgr_cmd_s_type * rpt_ptr
    /* Pointer to command */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL );

  if (CM_MSGR_MSG_CMSDS_IMS(rpt_ptr->cmd.client_cmd.msg_hdr.id))
  {
    cmsds_ims_cmd_proc(&(rpt_ptr->cmd.client_cmd));
  }

  if (CM_MSGR_MSG_CMSDS_CONFIG(rpt_ptr->cmd.client_cmd.msg_hdr.id))
  {
    cmsds_config_cmd_proc(&(rpt_ptr->cmd.client_cmd));
  }

}

/*===========================================================================

FUNCTION cmsds_config_cmd_proc

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsds_config_cmd_proc(

  const cm_sds_cmd_u_type   *cm_cmd_ptr
    /* Pointer to client command*/
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cm_cmd_ptr->msg_hdr.id )
  {
    case MM_DOM_SEL_CONFIG_LTE_VOICE_DOMAIN_PREF_GET_REQ:
    {
      CM_MSG_HIGH_1("MSGR->CM: VOICE_DOMAIN_PREF_GET_REQ, inst_id=%d",
                     cm_cmd_ptr->cm_domain_sel_config_req.hdr.inst_id);
      cmsds_send_voice_domain_pref_cnfg_resp(cm_cmd_ptr);
      break;
    }

    case MM_DOM_SEL_CONFIG_LTE_SMS_DOMAIN_PREF_GET_REQ:
    {      
      CM_MSG_HIGH_1("MSGR->CM: SMS_DOMAIN_PREF_GET_REQ, inst_id=%d",
                     cm_cmd_ptr->cm_domain_sel_config_req.hdr.inst_id);
      cmsds_send_sms_domain_pref_cnfg_resp(cm_cmd_ptr);
      break;
    }

    case MM_DOM_SEL_CONFIG_UPDATE_LTE_SMS_DOMAIN_PREF_REQ:
    {
      cmsds_process_sms_domain_pref_chg_cmd (cm_cmd_ptr);
    }
    break;

    case MM_DOM_SEL_CONFIG_UPDATE_LTE_VOICE_DOMAIN_PREF_REQ:
    default:
       CM_MSG_HIGH_1("Invalid message %d", cm_cmd_ptr->msg_hdr.id);
       break;
  }
}


/*===========================================================================
@FUNCTION cmsds_process_sms_domain_pref_chg_cmd

@DESCRIPTION
Processes SMS pref changed ind from clients.

@DEPENDENCIES
None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_process_sms_domain_pref_chg_cmd (

  const cm_sds_cmd_u_type   *cm_cmd_ptr
    /* Pointer to client command*/
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  sys_sms_domain_pref_e_type curr_pref;
  sys_sms_domain_pref_e_type new_pref;

  cm_ss_e_type               ss       = CM_SS_MAIN;
  sys_modem_as_id_e_type     asubs_id = SYS_MODEM_AS_ID_1;
    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( cm_cmd_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For multi-sim targets, update the ss. */
  asubs_id = cmph_get_as_id_from_ll_ind(INST_ID_TO_SYS_AS_ID(
                      cm_cmd_ptr->cm_domain_sel_config_update_req.hdr.inst_id));
  
  ss = cmph_determine_ss_per_rat(asubs_id, SYS_SYS_MODE_LTE);
  
  CM_MSG_HIGH_3("MSGR->CM: UPDATE_LTE_SMS_DOMAIN_PREF_REQ, LTE_SS, ss=%d, inst_id=%d, asubs_id=%d",
                 ss, 
                 cm_cmd_ptr->cm_domain_sel_config_update_req.hdr.inst_id,
                 asubs_id);

  curr_pref = cmsds_get_config_ptr(asubs_id)->sms_domain_pref[SYS_SYS_MODE_LTE];
  new_pref = cm_cmd_ptr->cm_domain_sel_config_update_req.data.value.lte_sms_dom_pref;

  if (curr_pref == new_pref)
  {
    CM_MSG_HIGH_2("Ignore SMS pref changed command, Current %d new %d ",\
                                              curr_pref, new_pref );
  }
  else
  {
    if (new_pref == SYS_SMS_DOMAIN_PREF_PS_SMS_NOT_ALLOWED)
    {
      sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_LTE] = SYS_IMS_REG_STATE_DEREGISTERED;
      cmsds_send_ims_pref_changed_ind(SYS_SYS_MODE_LTE,CM_CALL_TYPE_NONE_MASK, CM_CALL_TYPE_SMS_MASK,
        asubs_id);

      cmsds_sms_dom_sel_process_no_ims(ss, SYS_SYS_MODE_LTE);

      cmsds_send_sms_domain_pref_cnfg_update_ind (asubs_id);
    }
    else if (new_pref == SYS_SMS_DOMAIN_PREF_PS_SMS_PREF)
    {
      sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_LTE] = SYS_IMS_REG_STATE_NEEDS_REG;
      cmsds_send_ims_pref_changed_ind(SYS_SYS_MODE_LTE, CM_CALL_TYPE_SMS_MASK, CM_CALL_TYPE_SMS_MASK,
        asubs_id);

      cmsds_send_sms_domain_pref_cnfg_update_ind (asubs_id);
    }
    else
    {
      CM_MSG_HIGH_1("Received invalid SMS domain config %d", new_pref);
    }

    cmsds_get_config_ptr(asubs_id)->sms_domain_pref[SYS_SYS_MODE_LTE] = new_pref;
  }
}

/*===========================================================================
@FUNCTION cmsds_send_sms_domain_pref_cnfg_update_ind

@DESCRIPTION
Sends configured sms domain pref to clients

@DEPENDENCIES
None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_send_sms_domain_pref_cnfg_update_ind (
     sys_modem_as_id_e_type               asubs_id
)
{
  cm_sds_rsp_u_type  *resp_primitive = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  /*           Dynamically allocate memory for  Dom SEL IND message                   */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  resp_primitive = (cm_sds_rsp_u_type *)cm_mem_malloc
                     (sizeof(cm_sds_rsp_u_type));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  resp_primitive->cm_domain_sel_config_chg_ind.data.value.lte_sms_dom_pref
                  = cmsds_get_config_ptr(asubs_id)->sms_domain_pref[SYS_SYS_MODE_LTE];
  resp_primitive->cm_domain_sel_config_chg_ind.data.config
                  = CM_DOM_SEL_CONFIG_TYPE_LTE_SMS_DOM_PREF;
  resp_primitive->cm_domain_sel_config_chg_ind.hdr.inst_id= \
        SYS_AS_ID_TO_INST_ID(asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("CM->MSGR: SEND: SMS_DOMAIN_PREF_CHANGED_IND, sms_domain_pref=%d, inst_id=%d",
                   resp_primitive->cm_domain_sel_config_chg_ind.data.value.lte_sms_dom_pref,
                   resp_primitive->cm_domain_sel_config_chg_ind.hdr.inst_id);

  (void) cm_msgr_send( MM_DOM_SEL_CONFIG_LTE_SMS_DOMAIN_PREF_CHANGED_IND, 
                       MSGR_MM_DOM_SEL,
                       (msgr_hdr_s*)resp_primitive, 
                       sizeof(cm_sds_rsp_u_type) );

  /* Free allocated client message buffer */
  cm_mem_free (resp_primitive);

}

/*===========================================================================
@FUNCTION cmsds_send_voice_domain_pref_cnfg_resp

@DESCRIPTION
Sends configured voice domain pref to clients

@DEPENDENCIES
None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_send_voice_domain_pref_cnfg_resp (
  const cm_sds_cmd_u_type   *cm_cmd_ptr
    /* Pointer to client command*/
)
{
  cm_sds_rsp_u_type  *resp_primitive = NULL;
  errno_enum_type    msgr_error = E_FAILURE;

  sys_modem_as_id_e_type     asubs_id = SYS_MODEM_AS_ID_1;
    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( cm_cmd_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  asubs_id = cmph_get_as_id_from_ll_ind(INST_ID_TO_SYS_AS_ID(
                      cm_cmd_ptr->cm_domain_sel_config_req.hdr.inst_id));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  /*           Dynamically allocate memory for  Dom SEL IND message                   */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  resp_primitive = (cm_sds_rsp_u_type *)cm_mem_malloc
                     (sizeof(cm_sds_rsp_u_type));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  resp_primitive->cm_domain_sel_config_resp.status = CM_DOM_SEL_SUCCESS;
  resp_primitive->cm_domain_sel_config_resp.data.value.lte_voice_dom_pref
                  = cmsds_get_config_ptr(asubs_id)->voice_domain_pref;
  resp_primitive->cm_domain_sel_config_resp.data.config
                  = CM_DOM_SEL_CONFIG_TYPE_LTE_VOICE_DOM_PREF;
  resp_primitive->cm_domain_sel_config_resp.hdr.inst_id= \
        SYS_AS_ID_TO_INST_ID(asubs_id);
        
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("CM->MSGR: SEND: VOICE_DOMAIN_PREF_GET_RSP, lte_voice_dom_pref=%d, inst_id=%d",
                   resp_primitive->cm_domain_sel_config_resp.data.value.lte_voice_dom_pref,
                   resp_primitive->cm_domain_sel_config_resp.hdr.inst_id);

  (void) cm_msgr_send( MM_DOM_SEL_CONFIG_LTE_VOICE_DOMAIN_PREF_GET_RSP, 
                       MSGR_MM_DOM_SEL,
                       (msgr_hdr_s*)resp_primitive, 
                       sizeof(cm_sds_rsp_u_type) );

  /* Free allocated client message buffer */
  cm_mem_free (resp_primitive);

  SYS_ARG_NOT_USED(msgr_error)
}

/*===========================================================================
@FUNCTION cmsds_send_sms_domain_pref_cnfg_resp

@DESCRIPTION
Sends configured sms domain pref to clients

@DEPENDENCIES
None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_send_sms_domain_pref_cnfg_resp (

  const cm_sds_cmd_u_type   *cm_cmd_ptr
    /* Pointer to client command*/
)
{
  cm_sds_rsp_u_type  *resp_primitive = NULL;
  errno_enum_type    msgr_error = E_FAILURE;
  sys_modem_as_id_e_type     asubs_id = SYS_MODEM_AS_ID_1;
    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( cm_cmd_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  asubs_id = cmph_get_as_id_from_ll_ind(INST_ID_TO_SYS_AS_ID(
                      cm_cmd_ptr->cm_domain_sel_config_req.hdr.inst_id));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  /*           Dynamically allocate memory for  Dom SEL IND message                   */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  resp_primitive = (cm_sds_rsp_u_type *)cm_mem_malloc
                     (sizeof(cm_sds_rsp_u_type));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  resp_primitive->cm_domain_sel_config_resp.status = CM_DOM_SEL_SUCCESS;
  resp_primitive->cm_domain_sel_config_resp.data.value.lte_sms_dom_pref
                  = cmsds_get_config_ptr(asubs_id)->sms_domain_pref[SYS_SYS_MODE_LTE];
  resp_primitive->cm_domain_sel_config_resp.data.config
                  = CM_DOM_SEL_CONFIG_TYPE_LTE_SMS_DOM_PREF;
  resp_primitive->cm_domain_sel_config_resp.hdr.inst_id= \
        SYS_AS_ID_TO_INST_ID(asubs_id);
        
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("CM->MSGR: SEND: SMS_DOMAIN_PREF_GET_RSP, lte_sms_dom_pref=%d, inst_id=%d",
                   resp_primitive->cm_domain_sel_config_resp.data.value.lte_sms_dom_pref,
                   resp_primitive->cm_domain_sel_config_resp.hdr.inst_id);

  (void) cm_msgr_send( MM_DOM_SEL_CONFIG_LTE_SMS_DOMAIN_PREF_GET_RSP, 
                       MSGR_MM_DOM_SEL,
                       (msgr_hdr_s*)resp_primitive, 
                       sizeof(cm_sds_rsp_u_type) );

  /* Free allocated client message buffer */
  cm_mem_free (resp_primitive);

  SYS_ARG_NOT_USED(msgr_error)

}

/*===========================================================================
@FUNCTION cmsds_send_resp_to_get_imf_pref_req

@DESCRIPTION
Send response to IMS_PREF_GET_REQ


@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_send_resp_to_get_imf_pref_req
(
  cm_call_type_mask_type ims_pref_mask,
  
  uint8                  inst_id
)
{
  cm_sds_rsp_u_type  *client_request = NULL;
  cm_call_type_mask_type call_type_mask = CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_SMS_MASK;
  cmsds_s_type       *sds_ptr        = cmsds_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  /*           Dynamically allocate memory for  Dom SEL IND message                   */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  client_request = (cm_sds_rsp_u_type *)cm_mem_malloc
                     (sizeof(cm_sds_rsp_u_type));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  client_request->cm_domain_sel_ims_pref_get_rsp.data.ims_pref_mask  = ims_pref_mask;

  client_request->cm_domain_sel_ims_pref_get_rsp.data.call_type_mask = call_type_mask;

  client_request->cm_domain_sel_ims_pref_chg_ind.msg_hdr.inst_id= inst_id;

  if(sds_ptr->lte_ims_voice_avail == CMSDS_IMS_VOICE_AVAIL)
  {
    client_request->cm_domain_sel_ims_pref_chg_ind.data.lte_ims_voice_avail = TRUE;
  }
  else
  {
    client_request->cm_domain_sel_ims_pref_chg_ind.data.lte_ims_voice_avail = FALSE;
  }

  /* Current system */
  client_request->cm_domain_sel_ims_pref_get_rsp.data.system         = SYS_SYS_MODE_LTE;

  CM_MSG_HIGH_3("CM->MSGR: SEND: IMS_PREF_GET_RSP, ims_pref_mask=%d, lte_ims_voice_avail=%d, inst_id=%d",
                   ims_pref_mask, 
                   client_request->cm_domain_sel_ims_pref_chg_ind.data.lte_ims_voice_avail,  
                   client_request->cm_domain_sel_ims_pref_chg_ind.msg_hdr.inst_id);
  
  (void) cm_msgr_send( MM_DOM_SEL_DOMAIN_SELECTED_LTE_IMS_PREF_GET_RSP, MSGR_MM_DOM_SEL,
                       (msgr_hdr_s*)client_request, sizeof(cm_sds_rsp_u_type) );

  /* Free allocated message buffer */
  cm_mem_free (client_request);

}
/*===========================================================================
@FUNCTION cmsds_send_resp_to_get_imf_pref_req_ext

@DESCRIPTION
Send response to IMS_PREF_GET_REQ_EXT


@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_send_resp_to_get_imf_pref_req_ext
(
  sys_sys_mode_mask_e_type  sys_mode_mask,
  
  uint8                     inst_id

)
{
  cm_sds_rsp_u_type  *client_request = NULL;
  errno_enum_type    msgr_error = E_FAILURE;

  CM_MSG_HIGH_3("DOM SEL: IMS_PREF_GET_RESP_EXT sys_mode_mask %d, voice_pref_mask = %d, sms_pref_mask=%d",
                  sys_mode_mask,cmsds_ptr()->ims_voice_pref_mask,cmsds_ptr()->ims_sms_pref_mask);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  /*           Dynamically allocate memory for  Dom SEL RSP message                   */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  if( (client_request = (cm_sds_rsp_u_type *)cm_mem_malloc
                     (sizeof(cm_sds_rsp_u_type))) == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  memset(client_request, 0, sizeof(cm_sds_rsp_u_type));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  client_request->cm_domain_sel_ims_pref_get_rsp_ext.sys_mode_mask = sys_mode_mask;
  client_request->cm_domain_sel_ims_pref_get_rsp_ext.ims_voice_pref_mask = cmsds_ptr()->ims_voice_pref_mask;

  client_request->cm_domain_sel_ims_pref_get_rsp_ext.ims_sms_pref_mask = cmsds_ptr()->ims_sms_pref_mask;

  client_request->cm_domain_sel_ims_pref_get_rsp_ext.msg_hdr.inst_id= inst_id;
        
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("CM->MSGR: SEND: IMS_PREF_GET_EXT_RSP, sys_mode_mask=%d, ims_voice_pref_mask=%d, inst_id=%d",
                   sys_mode_mask,
                   client_request->cm_domain_sel_ims_pref_get_rsp_ext.ims_voice_pref_mask,
                   client_request->cm_domain_sel_ims_pref_get_rsp_ext.msg_hdr.inst_id);

  (void) cm_msgr_send( MM_DOM_SEL_DOMAIN_SELECTED_IMS_PREF_GET_EXT_RSP, 
                       MSGR_MM_DOM_SEL,
                       (msgr_hdr_s*)client_request, 
                       sizeof(cm_sds_rsp_u_type) );
  
  /* Free allocated message buffer */
  cm_mem_free (client_request);

  SYS_ARG_NOT_USED(msgr_error)
}

/*===========================================================================
@FUNCTION cmsds_send_resp_to_domain_sel_get_req

@DESCRIPTION
Send response to DOM_SEL_GET_REQ

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None


===========================================================================*/
void cmsds_send_resp_to_domain_sel_get_req
(
  sys_cm_domain_sel_domain_e_type domain_selected,

  cm_call_type_mask_type          call_type_mask,
  
  uint8                           inst_id
)
{
  cm_sds_rsp_u_type  *client_request = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  /*           Dynamically allocate memory for  Dom SEL IND message                   */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - -*/
  client_request = (cm_sds_rsp_u_type *)cm_mem_malloc
                      (sizeof(cm_sds_rsp_u_type));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  client_request->cm_domain_sel_domain_get_rsp.data.domain    = domain_selected;

  client_request->cm_domain_sel_domain_get_rsp.data.call_type_mask = call_type_mask;

  /* Current system */
  client_request->cm_domain_sel_domain_get_rsp.data.system         = SYS_SYS_MODE_LTE;

  client_request->cm_domain_sel_domain_get_rsp.msg_hdr.inst_id= inst_id;

  CM_MSG_HIGH_3("CM->MSGR: SEND: GET_RSP, domain=%d call_type=%d, inst_id=%d",
                   domain_selected,
                   call_type_mask,
                   client_request->cm_domain_sel_domain_get_rsp.msg_hdr.inst_id);
                   
  (void) cm_msgr_send( MM_DOM_SEL_DOMAIN_SELECTED_GET_RSP, MSGR_MM_DOM_SEL,
                       (msgr_hdr_s*)client_request, sizeof(cm_sds_rsp_u_type) );
        
  /* Free allocated message buffer */
  cm_mem_free (client_request);

}

/*===========================================================================
@FUNCTION cmsds_process_dom_sel_ims_pref_get_req

@DESCRIPTION
Handles  IMS_PREF_GET_REQ from IMS

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_process_dom_sel_ims_pref_get_req
(
  uint8 inst_id
)
{
  cmss_s_type    *ss_ptr   = cmss_ptr();
  /* Pointer to serving system object. .*/

  cm_call_type_mask_type    ims_pref_mask = CM_CALL_TYPE_NONE_MASK;

  boolean lte_ims_voice_avail = FALSE;
  boolean skip_ssac_hvolte_check = FALSE;  
  cmsds_s_type  *sds_ptr = cmsds_ptr();
  sys_modem_as_id_e_type as_id = INST_ID_TO_SYS_AS_ID(inst_id);

  cm_call_id_type  ims_call_id;
  cmcall_s_type    *call_ptr = NULL;  

  ims_call_id= cmcall_get_voip_call_id();
  call_ptr = cmcall_ptr ( ims_call_id );  

  CM_MSG_MED_3("lte_ims_voice_avail = %d, srv_buf_ptr=0x%x, tssac=%d ",
                   sds_ptr->lte_ims_voice_avail,
                   sds_ptr->srv_ind_buf_ptr,
                   sds_ptr->tssac_hys);
  if(sds_ptr->srv_ind_buf_ptr != NULL)
  {
    /*ALways return TRUE since IMS should not tear down call regardless of TRUE VOPS status*/
      lte_ims_voice_avail = TRUE;
    }
  else
  {
    lte_ims_voice_avail = ss_ptr->info.lte_ims_voice_avail;
  }

  if((call_ptr) && (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY))
  {
    skip_ssac_hvolte_check = TRUE;
    CM_MSG_HIGH_1("Set skip_ssac_hvolte_check for call-id %d",ims_call_id);
  }

  /* If tssac is running, always report false in query API. wait for timer exp and ims_pref_chg ind*/
  if(cmph_get_volte_sub_info_ptr(as_id)->is_ssac_hvolte && (skip_ssac_hvolte_check == FALSE) &&
	  ((sds_ptr->tssac_hys !=0) || (sds_ptr->va_type.va == CMSDS_VA_TYPE_NON_VOPS)))
  {
    lte_ims_voice_avail = FALSE;
  }


  ims_pref_mask = cmsds_get_call_type_mask_ims(lte_ims_voice_avail,
                                               BM(SYS_SYS_MODE_LTE),as_id);

  cmsds_send_resp_to_get_imf_pref_req(ims_pref_mask, inst_id);
}
/*===========================================================================
@FUNCTION cmsds_process_dom_sel_ims_pref_get_req_ext

@DESCRIPTION
Handles  IMS_PREF_GET_REQ from IMS

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_process_dom_sel_ims_pref_get_req_ext
(
  uint8 inst_id
)
{
  cmss_s_type    *ss_ptr   = cmss_ptr();
  /* Pointer to serving system object. .*/
  sys_sys_mode_mask_e_type  sys_mode_mask = SYS_SYS_MODE_MASK_NONE;

  boolean lte_ims_voice_avail = FALSE;
  cmsds_s_type  *sds_ptr = cmsds_ptr();
  sys_modem_as_id_e_type as_id = INST_ID_TO_SYS_AS_ID(inst_id);

  CM_MSG_MED_3("lte_ims_voice_avail = %d, srv_buf_ptr=0x%x, tssac=%d ",
                   sds_ptr->lte_ims_voice_avail,
                   sds_ptr->srv_ind_buf_ptr,
                   sds_ptr->tssac_hys);

  if (!cmph_is_lte_capable_on_sub(as_id))
  {
    cmsds_send_resp_to_get_imf_pref_req_ext(sys_mode_mask, inst_id);
    return;
  }

  if(sds_ptr->srv_ind_buf_ptr != NULL)
  {
    /*ALways return TRUE since IMS should not tear down call regardless of TRUE VOPS status*/
      lte_ims_voice_avail = TRUE;
  }
  else
  {
    lte_ims_voice_avail = ss_ptr->info.lte_ims_voice_avail;
  }

  /* If tssac is running, always report false in query API. wait for timer exp and ims_pref_chg ind*/
  if(cmph_get_volte_sub_info_ptr(as_id)->is_ssac_hvolte &&
	  ((sds_ptr->tssac_hys !=0) || (sds_ptr->va_type.va == CMSDS_VA_TYPE_NON_VOPS)))
  {
    lte_ims_voice_avail = FALSE;
  }

  /* check current srv sys mode */
  if(ss_ptr->info.srv_status == SYS_SRV_STATUS_SRV && 
     as_id == cmph_map_cm_ss_to_subs(CM_SS_MAIN))
  {
    sys_mode_mask |= BM(ss_ptr->info.sys_mode);
  }
  if(ss_ptr->info.hdr_srv_status == SYS_SRV_STATUS_SRV &&
     as_id == cmph_map_cm_ss_to_subs(CM_SS_MAIN))
  {
    sys_mode_mask |= BM(SYS_SYS_MODE_HDR);
  }
  if(ss_ptr->info.gw_srv_status == SYS_SRV_STATUS_SRV &&
     as_id == cmph_map_cm_ss_to_subs(CM_SS_HYBR_2))
  {
    sys_mode_mask |= BM(ss_ptr->info.gw_sys_mode);
  }
  if(ss_ptr->info.gw3_srv_status == SYS_SRV_STATUS_SRV &&
     as_id == cmph_map_cm_ss_to_subs(CM_SS_HYBR_3))
  {
    sys_mode_mask |= BM(ss_ptr->info.gw3_sys_mode);
  }

  cmsds_get_call_type_mask_ims(lte_ims_voice_avail,
                               sys_mode_mask,as_id);

  cmsds_send_resp_to_get_imf_pref_req_ext(sys_mode_mask, inst_id);
}
/*===========================================================================

FUNCTION cmsds_process_dom_sel_get_req

DESCRIPTION
  Processes Domain selection get request from SDS clients

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

========================================================================== */
void cmsds_process_dom_sel_get_req(
  const cm_domain_sel_domain_info_s_type *dom_sel_get_req,
  uint8 inst_id
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  sys_cm_domain_sel_domain_e_type selected_domain;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( dom_sel_get_req != NULL );
  CM_ASSERT( sds_ptr != NULL );

  CM_MSG_HIGH_3("RXD: GET_REQ, call_type_mask 0x%x,curr_voice_dom=%d,curr_sms_dom=%d",
                 dom_sel_get_req->call_type_mask,
                 sds_ptr->curr_voice_dom_selected,
                 sds_ptr->curr_sms_dom_selected);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (dom_sel_get_req->call_type_mask & CM_CALL_TYPE_VOICE_MASK)
  {
    selected_domain = sds_ptr->curr_voice_dom_selected;
    cmsds_send_resp_to_domain_sel_get_req(selected_domain, CM_CALL_TYPE_VOICE_MASK, inst_id);
  }

  if (dom_sel_get_req->call_type_mask & CM_CALL_TYPE_SMS_MASK)
  {
    selected_domain = sds_ptr->curr_sms_dom_selected;
    cmsds_send_resp_to_domain_sel_get_req(selected_domain, CM_CALL_TYPE_SMS_MASK, inst_id);
  }
}

/*===========================================================================

@FUNCTION cmsds_determine_if_lte_only_mode

@DESCRIPTION
Determined if UE is in LTE only mode.
If UE in LTE only either as per CM modes or SD modes, return TRUE.
Ideally, both should be same. However, they can differ in an event of 
out of sync between CM and SD. 

For eg,

1.
--Mode_pref=GWL, camped on LTE.
--change mode pref to LTE ONLY and CM sends LTE only service req
--Initiate manual PLMN search, CM sends NET LIST REQ
--Change mode pref to AUTO.
--CM sends LTE ONLY service req when manual search aborted.
--NAS sends service cnf with SMS ONLY capabilities.
--CMSDS goes on to block the PLMN since CM was now in AUTO mode.
===>In such cases, we need to consider SD's mode, acc. to which service req was sent.

2.
--Mode_pref=GWL, camp on LTE with 3gpp csfb capability
--Initiate MPLMN search, send NET LIST REQ to NAS
--Change mode pref to LTE only
--CM sends service req with GWL when manual search is aborted.
--NAS loops back with LTE service with lte_cs_Cap=NONE
--CMSDS consideres PPCSFB as not possible since CM's mode is now lte only
--CMSDS blocks PLMN and doesnt skip blocking even though now
    CM is in LTE only mode.
===>In such cases, we need to consider CM's mode .  

@DEPENDENCIES
None

@RETURNS
None

@SIDE_EFFECT
None

===========================================================================*/

static boolean cmsds_determine_if_lte_only_mode
(
  cm_ss_e_type              ss
)
{
  /* SD side modes and bands */
  sd_ss_mode_pref_e_type	sd_mode_pref	  = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type	sd_band_pref	  = SD_SS_BAND_PREF_NONE;
  sys_lte_band_mask_e_type        sd_lte_band_pref  = SYS_LTE_BAND_MASK_CONST_NONE;
  sd_ss_band_pref_e_type	sd_tds_band_pref  = SD_SS_BAND_PREF_NONE;
  
  boolean                   is_lte_only_cm = FALSE, is_lte_only_sd = FALSE; 

  /*1.Determine LTE ONLY as per CM modes */

  /* Take common of UE CAP and CM PREF, to take care of 
  ** case when mode_cap/band_cap  is LTE ONLY 
  */

  is_lte_only_cm = cmph_is_lte_only_sub(ss);
  
  /*2.Determine LTE ONLY as per SD modes */  
  
  /* Fetch mode_pref and band_pref values as per SD for this stack (ss)
  */
  sd_misc_get_mode_band_per_ss(ss, &sd_mode_pref, &sd_band_pref, &sd_lte_band_pref, &sd_tds_band_pref);
  
  is_lte_only_sd = (cmcall_is_lte_only_mode(cmph_map_sd_mode_pref_to_cm_mode_pref(sd_mode_pref), 
     						   cmph_map_sd_band_pref_to_cm_band_pref(sd_band_pref),
     						   sd_lte_band_pref,
     						   cmph_map_sd_tds_band_pref_to_cm_tds_band_pref(sd_tds_band_pref)));

  CM_MSG_HIGH_5("sd_mode_pref:%d, SD:band_pref: CGW = 0x%08x %08x band_pref: TDS = 0x%08x %08x",
                sd_mode_pref,
                QWORD_HIGH(sd_band_pref),
                QWORD_LOW(sd_band_pref),
                QWORD_HIGH(sd_tds_band_pref),
                QWORD_LOW(sd_tds_band_pref));

  cm_print_lte_band_mask(sd_lte_band_pref);
  
  CM_MSG_HIGH_2("is_lte_only_cm:%d, is_lte_only_sd:%d",
  		          is_lte_only_cm, is_lte_only_sd);
  
  /*3.If either CM or SD is in LTE only mode, return TRUE 
  */
  if(is_lte_only_cm || is_lte_only_sd)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

}

/*===========================================================================

@FUNCTION cmsds_perform_plmn_blocking

@DESCRIPTION
Performs PLMN plmn blocking

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

void cmsds_perform_plmn_blocking
(
  cm_ss_e_type              ss,

  cm_plmn_block_reson_e_type  block_reas

)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  cmss_s_type    *ss_ptr  = cmss_ptr();
  /* Pointer to serving system object. .*/

  cmph_s_type    *ph_ptr  = cmph_ptr();

  sd_si_info_s_type si_info;

  cm_plmn_blocking_info_s_type  plmn_blocking_info;
  cmregprx_stack_info_s_type* cmregprx_stack_info_ptr = 
            cmregprx_get_stack_info_ptr(cmph_map_cm_ss_type_to_sd_ss_type(ss));

  cmregprx_info_s_type           *cmregprx_info_ptr = cmregprx_get_info_ptr();

  boolean                 is_lte_only = FALSE; 
  sys_modem_as_id_e_type  asubs_id = cmph_map_cm_ss_to_subs(ss);

  cmsds_sub_config_s_type *cmsds_config = cmsds_get_config_ptr(asubs_id);

  CM_ASSERT( sds_ptr != NULL );

  CM_MSG_HIGH_1("perform_plmn_block: block reason=%d", block_reas);
  sd_si_info_ptr_get(cmph_map_cm_ss_type_to_sd_ss_type(ss), &si_info);

  /*If sim is CS_invalid and we have PS only service , make sure that we skip
  ** PLMN blocking for case where  block_reas is either NO_VOICE or NO_SMS.
  */
  if ((sd_ss_gw_get_sim_state(ss) == SYS_SIM_STATE_CS_INVALID) &&
      (si_info.srv_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
      ((block_reas == CM_PLMN_BLOCK_REAS_NO_VOICE) ||
       (block_reas == CM_PLMN_BLOCK_REAS_NO_SMS ))
     )
  {
    /* Since SIM is invalid for CS calls,Stay on LTE and indicate NO VOICE
    */
    CM_MSG_HIGH_3("DOM_SEL: Skip PLMN blocking .SIM CS Invalid, voice_domain_pref=%d,srv_domain=%d, block_reas=%d",\
                   cmsds_config->voice_domain_pref, si_info.srv_domain, block_reas);


    return;
  }

    /* Do not disable LTE for EMM temporary CS registration failures */
  if (si_info.srv_domain == SYS_SRV_DOMAIN_PS_ONLY &&
      si_info.cs_srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL &&
      si_info.mode          == SD_MODE_LTE &&
      ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED) ||
       (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY) ||
       (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED))
     )
  {
    CM_MSG_MED_2("Domsel:Stay on LTE (srv_domain=%d,cs_srv_status = %d )", 
                  si_info.srv_domain,si_info.cs_srv_status );
    return;
  }

  /*Determine if UE is in LTE only mode either as per CM or SD */
  is_lte_only = cmsds_determine_if_lte_only_mode(ss);

  CM_MSG_HIGH_3("DOM_SEL: LTE_only ss=%d is_lte_only=%d sds_ptr->is_ims_enabled:%d", 
                ss, is_lte_only, cmsds_config->is_ims_enabled);

  if (cmsds_get_config_ptr(asubs_id)->is_plmn_block_req_in_lte_only_mode == FALSE)
  {
    /* Irrespective of VOLTE status, do not block PLMN in LTE only mode as LTE can be used for PS services.
  */  
    if (((block_reas == CM_PLMN_BLOCK_REAS_NO_VOICE) ||
         (block_reas == CM_PLMN_BLOCK_REAS_NO_SMS)) &&
        (is_lte_only == TRUE))
    {
      CM_MSG_HIGH_2("DOM_SEL: LTE_ONLY mode & VOLTE or SMS not possible, Skip PLMN blocking, ims_enabled %d sms_only %d",
          cmsds_config->is_ims_enabled,
        cmregprx_stack_info_ptr->last_srv_req.sms_only);

      return;
    }
  }
  else
  {
    /* Follow the legacy behavior and perform PLMN blocking in LTE only mode as another VOLTE supported
    PLMN can be found */
    if (((block_reas == CM_PLMN_BLOCK_REAS_NO_VOICE) ||
         (block_reas == CM_PLMN_BLOCK_REAS_NO_SMS))  &&
        (is_lte_only == TRUE) &&
        (!cmsds_config->is_ims_enabled)
       )
    {
      CM_MSG_HIGH_2("DOM_SEL: LTE_ONLY mode & VOLTE enabled, Skip PLMN blocking, is_ims_enabled %d, sms_only %d",
          cmsds_config->is_ims_enabled,
          cmregprx_stack_info_ptr->last_srv_req.sms_only);
    
      return;
    }
  }

  if (SD_MODE_CONTAIN(cmph_map_cm_mode_pref_to_sd_mode_pref(
                    CMPH_SS_MODE_PREF(asubs_id, ph_ptr)), 
                    SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE)&&
       !SD_MODE_CONTAIN(cmph_map_cm_mode_pref_to_sd_mode_pref(
                    CMPH_SS_MODE_PREF(asubs_id, ph_ptr)), 
                    SD_SS_MODE_PREF_CDMA_HDR)&&
     (!cmsds_config->is_ims_enabled) &&
     (CMPH_SS_SRV_DOMAIN_PREF(asubs_id, ph_ptr) == CM_SRV_DOMAIN_PREF_PS_ONLY))
  {
    CM_MSG_HIGH_0("DOM_SEL: 3GPP only (with LTE mode pref) & srv_domain is PS_ONLY, Skip PLMN blocking");
    return;
  }

  plmn_blocking_info.request_pref = CM_PLMN_BLOCKING_PREF_PLMN_BLOCKING_ONLY;

  if ((sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_BARRED)
      #if defined FEATURE_LTE_TO_1X
      && (sds_ptr->csfb_status == CM_1XCSFB_NONE)
      #endif
     )
  {
    plmn_blocking_info.blocking_interval = 0;
  }
  else
  {
    if ( (cmsds_config->lte_disable_duration == CMSDS_DISABLE_LTE_DURATION_UPTO_PWR_CYCLE) ||
         (cmsds_config->lte_disable_duration == CMSDS_DISABLE_LTE_DURATION_INVALID))
    {
      /* Disbale LTE till T3402 timer value */
      plmn_blocking_info.blocking_interval = cmsds_get_t3402_value(asubs_id) * 1000;
    }
    else
    {
      plmn_blocking_info.blocking_interval =
        cmsds_config->lte_disable_duration * 1000;
    }
  }
  /* Deactivate 1XCSFB  if it is active */
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
       defined( FEATURE_LTE_TO_1X )
  cmsds_check_and_deact_csfb_mode ();
  #endif

  plmn_blocking_info.plmn.identity[0]  = sds_ptr->plmn_id.identity[0];
  plmn_blocking_info.plmn.identity[1]  = sds_ptr->plmn_id.identity[1];
  plmn_blocking_info.plmn.identity[2]  = sds_ptr->plmn_id.identity[2];

  /* Set PLMN blocking falg to indicate that PLMN blocking is in progress */
  sds_ptr->is_plmn_blocking_in_progress = TRUE;

  /* Set the as_id */
  plmn_blocking_info.asubs_id = asubs_id;

  CM_MSG_MED_5("plmn_blocking_info as_id %d, ss %d, set wait_resp_flag to TRUE, Domsel: PLMN being blocked %d %d %d",
               plmn_blocking_info.asubs_id,ss,
               plmn_blocking_info.plmn.identity[0],
               plmn_blocking_info.plmn.identity[1],
               plmn_blocking_info.plmn.identity[2]);

  (void)cm_ph_cmd_detach_and_plmn_blocking(
        NULL,
        NULL,
        CM_CLIENT_ID_ANONYMOUS,
        plmn_blocking_info);

 /** Set flag indicating that we are waiting for fplmn_list_ind updated with new blocked plmn
 */
 
 cmregprx_info_ptr->is_waiting_for_plmn_block_resp = TRUE;

}
/*===========================================================================

@FUNCTION cmsds_process_voice_not_avail_ind

@DESCRIPTION
Process voice not available ind according to current UE usage setting

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_process_voice_not_avail_ind
(
  cm_ss_e_type              ss
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  /* Pointer to phone object. .*/
  cmss_s_type    *ss_ptr   = cmss_ptr();
  /* Pointer to serving system object. .*/
  
  sys_modem_as_id_e_type  as_id = cmph_map_cm_ss_to_subs(ss);
  
  cmsds_sub_config_s_type *cmsds_config = cmsds_get_config_ptr(as_id);
  
  CM_ASSERT( sds_ptr != NULL );

  if (cmsds_config->ue_usage_setting == SYS_UE_USAGE_SETTING_VOICE_CENTRIC)
  {

    #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
    if(cmsoa_check_and_switch_to_srlte(ss, TRUE) == FALSE)
    #endif
    {
      /*Voice domain selection ind */
      cmsds_update_and_send_voice_dom_sel_ind (ss, SYS_CM_DOMAIN_SEL_DOMAIN_NONE,SYS_SYS_MODE_LTE);
      if (sds_ptr->is_temp_ims_reg_fail == TRUE &&
        cmsds_config->voice_domain_pref != SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY)
      {
        CM_MSG_HIGH_0( "Domsel: Skip IMS REG status processing");
        return;
      }
      /* If HYBR-2 is on, this mean LTE will be on HYBR-2 stack. Hence:
      ** 1. If we're in SVLTE-PLMN, then Domain Selection don't need to remove LTE because
      ** we could do 1X+LTE and using 1X for voice.
      ** 2. If we're in non-SVLTE-PLMN, then SV-Opr Arbitrator will switch to
      ** CSFB mode. AT that time, SDS logic will kicks in. We don't need remove LTE now either.
      */
      #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
      if (cmsoa_skip_dom_sel_due_to_soa())
      {
        CM_MSG_HIGH_0("MMSOA: Skip Dom Sel SOA logic will take care. No LTE removal");
      }
      else
      #endif
      {
        CM_MSG_HIGH_0( "Domsel: Disabling LTE as device is voice centric");
        if (  cmph_is_subs_feature_mode_1x_sxlte(as_id) &&
              (ss_ptr->info.is_hybr_gw_operational == TRUE))
        {
          CM_MSG_HIGH_0( "Domsel: Skip LTE Disbale/PLMN blocking as UE is in SVLTE configuration");
          return;
        }
        //SGLTE NEED CHANGE
        //Domain selection needs to know which ss is mapping to as_id, so that it could retrieve
        //the op_mode. For now, put hard-coded as_id
        if (cmmsimc_state_machine_ptr(as_id)->op_mode == CMMSC_OP_MODE_SGLTE_DUAL_STACK ||
            cmmsimc_state_machine_ptr(as_id)->op_mode == CMMSC_OP_MODE_SGLTE_SINGLE_STACK )
        {
          CM_MSG_HIGH_0( "Domsel: Skip LTE Disbale/PLMN blocking as UE is in SGLTE-Home configuration");
          return;
        }
        cmsds_perform_plmn_blocking(ss, CM_PLMN_BLOCK_REAS_NO_VOICE);

        if (sds_ptr->is_plmn_blocking_in_progress == TRUE)
        {
          /* SMS domain selection ind*/
          cmsds_update_and_send_sms_dom_sel_ind (ss, SYS_CM_DOMAIN_SEL_DOMAIN_NONE, SYS_SYS_MODE_LTE);
        }
      }
    }
    #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
    else
    {
      CM_MSG_MED_0( "Reset IMS SMS/VOICE reg status and lte_voice_avail");
      cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_UNKNOWN, SYS_SYS_MODE_LTE);
      cmsds_ptr()->ims_reg_status_sms[SYS_SYS_MODE_LTE] = SYS_IMS_REG_STATE_UNKNOWN;
    }
    #endif
  }
  else if (cmsds_config->ue_usage_setting == SYS_UE_USAGE_SETTING_DATA_CENTRIC)
  {
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) && \
     defined( FEATURE_LTE_TO_1X )
    /* Deactivate 1XCP from CSFB mode */
    cmsds_check_and_deact_csfb_mode ();
    #endif

    /*Voice domain selection ind */
    if(sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_CSFB_NOT_PREFFERED)
    {
      cmsds_update_and_send_voice_dom_sel_ind (ss, SYS_CM_DOMAIN_SEL_DOMAIN_3GPP, SYS_SYS_MODE_LTE);
    }
    else
    {
    cmsds_update_and_send_voice_dom_sel_ind (ss, SYS_CM_DOMAIN_SEL_DOMAIN_NO_VOICE, SYS_SYS_MODE_LTE);
  }
  }
}

/*===========================================================================

@FUNCTION cmsds_process_ps_voice_preferred_but_no_ps

@DESCRIPTION
Handles PS not available scenario when device is configured as IMS PS PREFERRED and CS VOICE secondary.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_process_ps_voice_preferred_but_no_ps
(
  cm_ss_e_type              ss
)
{

  cm_call_csfb_e_type csfb_type = CM_CALL_CSFB_TYPE_NONE;
  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);

  (void)cmsds_is_cs_voice_avail(&csfb_type, as_id);

  switch (csfb_type)
  {
    case CM_CALL_CSFB_TYPE_PPCSFB:
    {
      /* Voice domain selection ind */
      cmsds_update_and_send_voice_dom_sel_ind (ss, SYS_CM_DOMAIN_SEL_DOMAIN_3GPP,SYS_SYS_MODE_LTE);

      break;
    }

    case CM_CALL_CSFB_TYPE_1XCSFB:
    {
      #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) && \
           defined( FEATURE_LTE_TO_1X )
      /* Decide on stay on/leave LTE based on 1XCSFB status received from 1XCP*/
      cmsds_process_1xcsfb_status(ss);

      /* Perform 1XCP activation as PS voice is not available */
      if (cmsds_is_1xcsfb_activation_allowed (as_id) == TRUE)
      {
        /* CSFB activation can be done now */
        cmsds_send_1xcsfb_activate_req ();
      }

      #endif

      break;
    }

    /* Both CS and PS voice are not available */
    default:
    {
      /* Act on voice not available indication according to device usage setting */
      cmsds_process_voice_not_avail_ind(ss);
    }
  }
}

/*===========================================================================

@FUNCTION cmsds_process_ims_reg_status_cs_pref

@DESCRIPTION
Handles PS not available scenario when device is configured as IMS PS PREFERRED and CS VOICE secondary.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_process_ims_reg_status_cs_pref
(
  cm_ss_e_type              ss
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  cm_call_csfb_e_type csfb_type = CM_CALL_CSFB_TYPE_NONE;
  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);

  if((cmsds_is_cs_voice_avail(&csfb_type, as_id) == FALSE) &&
     (sds_ptr->is_temp_ims_reg_fail == FALSE))
  {
    if (cmcall_is_there_csfb_call(ss,CM_CALL_CSFB_TYPE_1XCSFB,
                                         CM_CALL_ID_INVALID)
                                         != CM_CALL_ID_INVALID)
    {
      CM_MSG_HIGH_0( "Domsel:skip IMS REG status due to CSFB call");
      return;
    }

    cmsds_process_voice_not_avail_ind(ss);
  }
}

/*===========================================================================

@FUNCTION cmsds_process_ims_reg_status_ind

@DESCRIPTION
Process  IMS_REG_STATTUS_IND from IMS

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_voice_dom_sel_process_ims_reg_status
(
  const cm_domain_sel_ims_reg_status_s_type *reg_status_ptr,

  sys_modem_as_id_e_type                     as_id
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  cmss_s_type    *ss_ptr   = cmss_ptr();
  cm_ss_e_type   ss        = CM_SS_MAIN;

  cmsds_sub_config_s_type  *cmsds_config = cmsds_get_config_ptr(as_id);
  volte_sub_info_s_type    *volte_sub_info_ptr = cmph_get_volte_sub_info_ptr(as_id);

  CM_ASSERT( sds_ptr != NULL );

  if((reg_status_ptr->system == SYS_SYS_MODE_HDR) && (ss_ptr->info.hdr_hybrid))
  {
    ss = CM_SS_HYBR_1;
  }

  /* For multi-sim targets, update the ss. */
  else if (reg_status_ptr->system == SYS_SYS_MODE_LTE)
  {
    ss = cmph_determine_ss_per_rat(as_id, SYS_SYS_MODE_LTE);
    CM_MSG_HIGH_2("LTE_SS, ss=%d, asubs_id=%d",ss, as_id);  
  }
  else
  {
    ss = cmph_map_as_id_to_cm_ss(as_id);
  }
  
  CM_MSG_HIGH_3( "Domsel:Process voice IMS_REG_STATUS_IND, sys_mode=%d, status=%d, ss=%d ",
             reg_status_ptr->system, reg_status_ptr->is_registered, ss);
  
  /* process IMS reg status on RATs other than LTE */
  if(reg_status_ptr->system != SYS_SYS_MODE_LTE)
  {
    if (reg_status_ptr->is_registered == TRUE)
    {
      CM_MSG_MED_0(" IMS voice over legacy RAT is not supported ");
    }
    cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_DEREGISTERED, reg_status_ptr->system);

    switch(reg_status_ptr->system)
    {
      case SYS_SYS_MODE_HDR:
      case SYS_SYS_MODE_CDMA:
        cmsds_update_and_send_voice_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_1X,SYS_SYS_MODE_HDR);
        break;

      case SYS_SYS_MODE_GSM:
      case SYS_SYS_MODE_WCDMA:
      case SYS_SYS_MODE_TDS:
        cmsds_update_and_send_voice_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_3GPP,reg_status_ptr->system);
        break;

      default:
        break;
    }

    return;
  }

  /* Process IMS_REG_STATTUS_IND ,if device supports IMS voice */
  if ((cmsds_config->voice_domain_pref != SYS_VOICE_DOMAIN_PREF_NONE) &&
      (cmsds_config->voice_domain_pref != SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY))
  {
    if (reg_status_ptr->is_registered == TRUE)
    {
      cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(as_id);
      CM_MSG_MED_2( "Domsel:IMS registration is success %d %d",
                    state_machine->stack_common_info.ue_mode,
                    cmss_is_hybr2_operational());

#if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE))
      if(cmph_is_oprting_in_csfb_pref_srlte(as_id)
         || cmsoa_is_oprting_in_csfb_pref_srlte(as_id))
      {
        /* reset the ping pong timer ,
        ** since it is not applicable for switch due to VOLTE
        */
        cmsoa_arm_timer(CMSOA_SV_OPRT_MODE_NONE,
                                    CMSOA_SV_SWITCH_DELAY_REAS_NONE);

        if(cmsoa_sv_switch_base(CMSOA_SV_OPRT_MODE_CSFB,CM_SS_MAIN,TRUE))
        {
          cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_UNKNOWN, SYS_SYS_MODE_LTE);
          return;
        }
      }
      else
#endif
      {
      cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_REGISTERED, reg_status_ptr->system);

      sds_ptr->is_temp_ims_reg_fail = FALSE;

      /* Update voice domain selection */
      if (!cmsds_config->is_sms_only)
      {
        /*---------------------------------------------------------------------------------*/
        /* Update domain selected to SYS_CM_DOMAIN_SEL_DOMAIN_IMS, if voice domain pref                 */
        /* 1. SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY or                                                            */
        /* 2. SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED or                                                   */
        /* 3. SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED and CS voice is not available                   */
        /*---------------------------------------------------------------------------------*/
        if (((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED) &&
              (cmsds_get_csfb_type_allowed(as_id) != CM_CALL_CSFB_TYPE_1XCSFB) &&
              (!cmsds_is_ppcsfb_possible(as_id)) && (!volte_sub_info_ptr->volte_special_pref_setting)) ||
            (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED) ||
            (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY))
        {

          cmsds_update_and_send_voice_dom_sel_ind (ss, SYS_CM_DOMAIN_SEL_DOMAIN_IMS, SYS_SYS_MODE_LTE);
        }
      }

      /* Previous IMS REG attempt is failed and received IMS REG success ind when using CS services for voice */
      #if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)
      if ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED) &&
          (sds_ptr->curr_voice_dom_selected == SYS_CM_DOMAIN_SEL_DOMAIN_1X))
      {
        /*Received IMS Registration success indication when UE is using 1XCSFB for voice */
        cmsds_check_and_deact_csfb_mode ();
      }
      #endif
    }
    }
    else
    {
      /* IMS registration failure */
      CM_MSG_MED_1( "Domsel:IMS registration is failed. cause code %d", reg_status_ptr->end_cause);

      /* Set IMS registration status as DEREGISTERED */
      cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_DEREGISTERED, reg_status_ptr->system);

      if (reg_status_ptr->end_cause == SYS_IMS_REG_END_CAUSE_TEMPORARY)
      {
        sds_ptr->is_temp_ims_reg_fail = TRUE;
        CM_MSG_MED_0( "Domsel:Temp IMS reg failure");
      }
      else
      {
        sds_ptr->is_temp_ims_reg_fail = FALSE;
        CM_MSG_MED_0( "Domsel:Permanent IMS reg failure");
      }

      switch(cmsds_config->voice_domain_pref)
      {
        case SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY:
        {
          /* Process voice not available condition according to
          ** device usage configuration  for permanaent IMS registration failures */
          cmsds_process_voice_not_avail_ind(ss);
          break;
        }

        case SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED:
        {
          /* Check for CS service as PS service is not available */
          cmsds_process_ps_voice_preferred_but_no_ps(ss);
          break;
        }

        case SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED:
        {
          /*  If CS voice is not available, then process voice not available
          ** condition according to device usage configuration
          */
          cmsds_process_ims_reg_status_cs_pref(ss);
          break;
        }
        default:
        {
           CM_MSG_MED_0( "Domsel:In valid voice domain preferance");
        }
      }
      /*  in svlte - we might have skipped IMS-PREF-CHG on 1x ,
        ** since at that time , LTE reg status could be seen as possible .
        ** but know we recieved LTE reg status fail , we should send clients IMS-PREF-CHG on 1x
        */
      if(cmph_is_subs_feature_mode_1x_sxlte(as_id) == TRUE &&
         ss_ptr->info.is_hybr_gw_operational == TRUE &&
         ss_ptr->info.srv_status == SYS_SRV_STATUS_SRV &&
         ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA &&
         sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_CDMA] == SYS_IMS_REG_STATE_UNKNOWN)
      {
        cmsds_update_and_send_ims_avail(FALSE,SYS_SYS_MODE_CDMA, as_id);
      }
    }
  }
}

/*===========================================================================

@FUNCTION cmsds_process_srv_ind_ims_ps_voice_only

@DESCRIPTION
Processes service indication according to PS_VOICE_ONLY configuration

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

void cmsds_process_srv_ind_ims_ps_voice_only
(
  cm_ss_e_type              ss,

  const sd_si_info_s_type   *si_info_ptr
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object */
  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);

  CM_ASSERT( sds_ptr != NULL );

  /*Post IMS PREFERRED CHGD IND Message for both VOICE and SMS to IMS*/

    cmsds_update_and_send_ims_avail(cmsds_map_lte_ims_voice_availability
                                      (si_info_ptr->lte_ims_voice_avail, as_id),
                                      SYS_SYS_MODE_LTE,
                                      as_id);

  if (!cmsds_is_ims_active(as_id))
    {
      CM_MSG_HIGH_0( "DOM SEL: IMS PS voice only: IMS PS voice is not available");
    /* Perform PLMN blocking and send "voice &sms" not available ind to IMS
        ** if the device is voice centric */
      cmsds_process_voice_not_avail_ind(ss);
    }

  /* Perform SMS domain selection if PS SMS is not allowed or IMS registrations is failed for SMS */
  if (!cmsds_is_ps_sms_possible(SYS_SYS_MODE_LTE, as_id))
    {

    /* Perform SMS domain selection if  PLMN blocking is not performed.
        */
    if(sds_ptr->is_plmn_blocking_in_progress == FALSE)
      {
        cmsds_sms_dom_sel_process_no_ims(ss, SYS_SYS_MODE_LTE);
      }
    }
  }


/*===========================================================================

@FUNCTION cmsds_process_srv_ind_cs_voice_preferred

@DESCRIPTION
Process the service indication according to CS_VOICE_PREF voice domain configuration

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

void cmsds_process_srv_ind_cs_voice_preferred
(
  cm_ss_e_type              ss,

  const sd_si_info_s_type   *si_info_ptr
)
{
  cmsds_s_type        *sds_ptr  = cmsds_ptr();
  cm_call_csfb_e_type csfb_type = CM_CALL_CSFB_TYPE_NONE;
  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);
  volte_sub_info_s_type* volte_sub_info_ptr = cmph_get_volte_sub_info_ptr(as_id);

  CM_ASSERT( sds_ptr != NULL );

  /* Check if (PP/1X) CSFB is possible */
  if (cmsds_is_cs_voice_avail(&csfb_type, as_id) == FALSE)
  {
    /* CS voice is not available */

    /*Post IMS PREFERRED IND Message to IMS*/

      cmsds_update_and_send_ims_avail(cmsds_map_lte_ims_voice_availability
                                        (si_info_ptr->lte_ims_voice_avail, as_id),
                                        SYS_SYS_MODE_LTE,
                                    as_id);

    /* CS voice not available and IMS voice is also not available*/
    if (!cmsds_is_ims_active(as_id))
    {
      /* Act on No voice condition as per device usage setting */
      cmsds_process_voice_not_avail_ind(ss);
    }

    /* Look for CS SMS support if PS SMS is not allowed or IMS registrations is failed for SMS */
    if (!cmsds_is_ps_sms_possible(SYS_SYS_MODE_LTE, as_id))
    {
      /* Perform SMS domain selectrion if PLMN blocking/LTE disable is not in progress */
      if (sds_ptr->is_plmn_blocking_in_progress == FALSE)
      {
        cmsds_sms_dom_sel_process_no_ims(ss, SYS_SYS_MODE_LTE);

      }
    }
  }
  else
  {
    CM_MSG_HIGH_0( "Domsel:CS PREFERRED: PP or 1X CSFB possible");

    if (csfb_type == CM_CALL_CSFB_TYPE_PPCSFB)
    {
      if(!volte_sub_info_ptr->volte_special_pref_setting)
      {
        if (cmsds_is_ims_active(as_id) == TRUE)
        {
          cmsds_send_ims_pref_changed_ind(SYS_SYS_MODE_LTE, CM_CALL_TYPE_NONE_MASK, CM_CALL_TYPE_VOICE_MASK,
            as_id);
        }
      }
      else
      /*VOLTE special pref handling
      * When the VoLTE Preferred Menu is set to No, the UE shall use LTE CSFB (CS domain) primary and VoLTE (IMS domain) 
      * secondary for voice calls.The UE shall remain IMS registered and perform IMS registration procedures when not registered
      */
      {
        cmsds_update_and_send_ims_avail(cmsds_map_lte_ims_voice_availability
                                        (si_info_ptr->lte_ims_voice_avail, as_id),
                                        SYS_SYS_MODE_LTE,
                                        as_id);
        
      }
      cmsds_update_and_send_voice_dom_sel_ind(ss, SYS_CM_DOMAIN_SEL_DOMAIN_3GPP,SYS_SYS_MODE_LTE);
    }

    /* 1X Domain selection indication will be sent when PRE_REG_SUCCESS ind comes from the 1XCP */
    /* Perform SMS domain selection */
    cmsds_process_sms_domain_selection(ss, SYS_SYS_MODE_LTE);

  }
}


/*===========================================================================

@FUNCTION cmsds_process_srv_ind_ims_ps_voice_preferred

@DESCRIPTION
Handles service indication when the voice domain configuration is PS PREF

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_process_srv_ind_ims_ps_voice_preferred
(
  cm_ss_e_type              ss,

  const sd_si_info_s_type   *si_info_ptr
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);

  CM_ASSERT( sds_ptr != NULL );

  /*Post IMS PREFERRED IND for VOCIE and SMS  to IMS*/
    cmsds_update_and_send_ims_avail(cmsds_map_lte_ims_voice_availability
                                      (si_info_ptr->lte_ims_voice_avail, as_id),
                                      SYS_SYS_MODE_LTE,
                                     as_id);

  /* Check for CS service if IMS registration is failed or N/W doesn't support IMS */
  if (!cmsds_is_ims_active(as_id))
  {
    CM_MSG_HIGH_0( "DOM SEL: IMS PS voice is not available");

    /* Perform voice domain selection */
    cmsds_process_ps_voice_preferred_but_no_ps(ss);
  }

  /* Perform SMS domain selection if PS SMS is not allowed or IMS registrations is failed for SMS */
  if (!cmsds_is_ps_sms_possible(SYS_SYS_MODE_LTE, as_id))
  {
    /* Perform SMS domain selection if PLMN blocking is not in progress */
    if (sds_ptr->is_plmn_blocking_in_progress == FALSE)
    {
      cmsds_sms_dom_sel_process_no_ims(ss, SYS_SYS_MODE_LTE);
    }
  }
}

/*===========================================================================

@FUNCTION cmsds_process_srv_ind_cs_voice_only

@DESCRIPTION
Process the service indication according to CS_VOICE_ONLY voice domain config

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

void cmsds_process_srv_ind_cs_voice_only
(
  cm_ss_e_type              ss,

  const sd_si_info_s_type  *si_info_ptr
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();  
  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);
  
  /* pointer to domain selection object */
  CM_ASSERT( sds_ptr != NULL );


  cmsds_update_and_send_ims_avail(cmsds_map_lte_ims_voice_availability
                                    (si_info_ptr->lte_ims_voice_avail, as_id),
                                    SYS_SYS_MODE_LTE,
                                     as_id);

  /* Perform voice domain selection */
  cmsds_voice_dom_sel_cs_voice_only(ss, si_info_ptr);

  /* Perform SMS domain selection if LTE is not disabled */
  if (!sds_ptr->is_plmn_blocking_in_progress)
  {
    cmsds_process_sms_domain_selection(ss, SYS_SYS_MODE_LTE);
  }
} /* cmsds_process_srv_ind_cs_voice_only */


/*===========================================================================

@FUNCTION cmsds_get_t3402_value

@DESCRIPTION
  Returns the T3402 value from cmsds_ptr

@DEPENDENCIES
None

@RETURNS
  uint32 T3402 value

@SIDE_EFFECT
None

===========================================================================*/
uint32 cmsds_get_t3402_value(
  sys_modem_as_id_e_type  asubs_id
)
{
  return (uint32)(cmsds_get_config_ptr(asubs_id)->t3402_value);
}/*cmsds_get_t3402_value*/

/*===========================================================================

@FUNCTION cmsds_t3402_changed_ind_proc

@DESCRIPTION
  Update the T3402 value based on NAS_EMM_T3402_CHANGED_IND.

@DEPENDENCIES

None

@RETURNS
  None
@SIDE_EFFECT

None
None

===========================================================================*/
void cmsds_t3402_changed_ind_proc (
     emm_t3402_changed_ind_s_type  *t3402_changed_ind
)
{
  sys_modem_as_id_e_type as_id = INST_ID_TO_SYS_AS_ID(t3402_changed_ind->msg_hdr.inst_id);

  CM_MSG_HIGH_2("NAS->CM: rxd T3402_CHANGED_IND T3402 = %d msecs, as_id = %d",
                  t3402_changed_ind->t3402_value, as_id);

  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  cmsds_get_config_ptr(as_id)->t3402_value = (t3402_changed_ind->t3402_value)/1000;
  rex_leave_crit_sect(cmtask_crit_sect_ptr());
}

#endif /* FEATURE_DOMAIN_SELECTION */

/**--------------------------------------------------------------------------
** Functions - external
** --------------------------------------------------------------------------
*/


#if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)

/*===========================================================================

@FUNCTION cmsds_is_1xcsfb_call_allowed

@DESCRIPTION
  Checks if 1XCP can be activated in CSFB mode

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

boolean cmsds_is_1xcsfb_call_allowed( 
  sys_modem_as_id_e_type  asubs_id

)
{
  cmph_s_type                   *ph_ptr        = cmph_ptr();
    /* Point at phone object */
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
    /* Point at service domain selection  object */

  sd_si_info_s_type             *si_ino_ptr    = NULL;

  cm_ss_e_type  ss = cmph_map_as_id_to_cm_ss(asubs_id);

  if (asubs_id != SYS_MODEM_AS_ID_1)
  {
    return FALSE;
  }

  if(sds_ptr->srv_ind_buf_ptr != NULL)
  {
    si_ino_ptr = sds_ptr->srv_ind_buf_ptr;
  }
  else
  {
    si_ino_ptr = cmss_intl_srv_info_ptr(ss);
  }

  if (sds_ptr->is_dual_rx_1xcsfb == TRUE                                &&
      (si_ino_ptr->mode  ==  SYS_SYS_MODE_LTE)  &&
      (si_ino_ptr->srv_domain == SYS_SRV_DOMAIN_PS_ONLY                ||
       si_ino_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS)                 &&
      ph_ptr->is_cdma_subscription_available == TRUE                   &&
      cmsds_is_mode_supp_1xcsfb(asubs_id)
     )
  {
    /* Dual RX 1XCSFB is enabled so having full service on LTE is enough */
    CM_MSG_LOW_3 ("Domsel:Dual RX 1XCSFB is set so CSFB possible mode = %d,status=%d,domain=%d",\
                  si_ino_ptr->mode, si_ino_ptr->srv_status, \
                  si_ino_ptr->srv_domain);
    return TRUE;
  }

  /* check if  CSFB available is recived,LTE PS attach success and
  ** CDMA subscription is available
  */
  if( sds_ptr->csfb_status != CM_1XCSFB_NOT_AVAIL                  &&
      sds_ptr->csfb_status != CM_1XCSFB_PREREG_FAIL                  &&
      si_ino_ptr->srv_status == SYS_SRV_STATUS_SRV                             &&
      ( si_ino_ptr->srv_domain == SYS_SRV_DOMAIN_PS_ONLY                       ||
        si_ino_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS )                       &&
        si_ino_ptr->mode   ==  SYS_SYS_MODE_LTE                              &&
      (ph_ptr->is_cdma_subscription_available == TRUE)             &&
      (sds_ptr->is_1xcsfb_activated == TRUE )                      &&
       cmsds_is_mode_supp_1xcsfb(asubs_id)
    )
  {
    /* 1XCSFB call is possible */
    return TRUE;

  }
  else
  {
    /* 1XCSFB call is not possible */
    CM_MSG_LOW_3 ("Domsel:1XCSFB not possible mode = %d,status=%d,domain=%d",\
                  si_ino_ptr->mode,si_ino_ptr->srv_status, \
                  si_ino_ptr->srv_domain);

    CM_MSG_LOW_2 ("Domsel:csfb_status = %d,is_1xcsfb_activated = %d",\
                 sds_ptr->csfb_status, sds_ptr->is_1xcsfb_activated );

    return FALSE;
  }
} /* cmsds_is_1xcsfb_call_allowed */



/*===========================================================================

@FUNCTION cmsds_is_dual_rx_1xcsfb_enabled

@DESCRIPTION
  Checks if 1XCP can be activated in CSFB mode

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

 boolean cmsds_is_dual_rx_1xcsfb_enabled( void )
{

 return cmsds_ptr()->is_dual_rx_1xcsfb;
}

/*===========================================================================

@FUNCTION cmsds_is_1xcsfb_avail

@DESCRIPTION
Indicates whether 1XCSFB is posible or not

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

static boolean cmsds_is_1xcsfb_avail
(
  void
)
{
  cmph_s_type                   *ph_ptr        = cmph_ptr();

  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Pointer to domain selection object. .*/

  sd_si_info_s_type *si_info_ptr = NULL;

  if(sds_ptr->srv_ind_buf_ptr != NULL)
  {
    si_info_ptr = sds_ptr->srv_ind_buf_ptr;
  }
  else
  {
    si_info_ptr = cmss_intl_srv_info_ptr(CM_SS_MAIN);
  }

  CM_ASSERT( sds_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );


  if ((si_info_ptr->srv_status == SYS_SRV_STATUS_SRV)         &&

      (si_info_ptr->srv_domain == SYS_SRV_DOMAIN_PS_ONLY    ||
       si_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS)     &&

      si_info_ptr->mode   ==  SYS_SYS_MODE_LTE              &&
      ph_ptr->is_cdma_subscription_available == TRUE        &&
      ph_ptr->oprt_mode == SYS_OPRT_MODE_ONLINE             &&
      sds_ptr->sib8_available == TRUE                   &&
      sds_ptr->csfb_status != CM_1XCSFB_NOT_AVAIL           &&
      sds_ptr->csfb_status != CM_1XCSFB_PREREG_FAIL         &&
      cmsds_is_mode_supp_1xcsfb(SYS_MODEM_AS_ID_1)
     )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

}

#endif /* FEATURE_LTE_TO_1X  */

/*===========================================================================

@FUNCTION cmsds_is_mode_supp_ppcsfb

@DESCRIPTION
  Checks whether mode pref supports PPCSFB or not. This function should be used for
  all domain sekection purposes.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
static boolean cmsds_is_mode_supp_ppcsfb(
  sys_modem_as_id_e_type    as_id
)
{
  cm_mode_pref_e_type      mode_pref = CM_MODE_PREF_NONE;
  sys_sys_mode_mask_e_type pm_mode_mask = SYS_SYS_MODE_MASK_NONE;
  cmmsc_state_machine_s_type   *state_machine_ptr;
  sd_ss_mode_pref_e_type comm_mode = SD_SS_MODE_PREF_NONE;
  cmph_s_type                    *ph_ptr        = cmph_ptr();

  mode_pref = CMPH_SS_MODE_PREF(as_id,ph_ptr);

  /* If devices's mode pref is EMERGENCY map it to AUTOMATIC for calculation here. 
  ** This is done to avoid recursive calls from map_cm_mode_pref_to_sd_mode_pref->
  ** cmsds_is_LTE_emerg_call_possible->cmsds_get_csfb_type_allowed->cmsds_is_ppcsfb_call_allowed
  ** ->cmsds_is_mode_supp_ppcsfb.
  ** It is done as both AUTOMATIC and EMERGENCY have same set of MODES
  */
  if(mode_pref == CM_MODE_PREF_EMERGENCY)
  {
    mode_pref = CM_MODE_PREF_AUTOMATIC;
  }
  
  if((as_id == SYS_MODEM_AS_ID_1) || \
      (as_id == SYS_MODEM_AS_ID_2) || \
      (as_id == SYS_MODEM_AS_ID_3)
     )
  {
    state_machine_ptr = cmmsimc_state_machine_ptr(as_id);
  }
  else
  {
    state_machine_ptr = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);
  }

  if(state_machine_ptr != NULL)
  {
    pm_mode_mask    =  state_machine_ptr->stack_common_info.ue_mode_capability;

    comm_mode = SD_GET_COMMON_MODE(cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref),\
                              cmph_map_sys_mode_mask_to_sd_mode_pref(pm_mode_mask));

  }

  CM_MSG_HIGH_2 ("3gpp csfb: pm_mode_mask %d,mode_pref =%d ",\
                pm_mode_mask, mode_pref);
  if(SD_SS_MODE_PREF_TDS_GSM_WCDMA & comm_mode)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}



/*===========================================================================

@FUNCTION cmsds_is_ppcsfb_call_allowed

@DESCRIPTION
  Checks if 1XCP can be activated in CSFB mode

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

boolean cmsds_is_ppcsfb_call_allowed(
  sys_modem_as_id_e_type    as_id
)
{

  cmsds_s_type                   *sds_ptr  = cmsds_ptr();
     /* Point at service domain selection  object */
     /*lint -esym(529, sds_ptr) not referenced */

  sd_si_info_s_type              *si_info_ptr = NULL;

  cm_ss_e_type                   ss = CM_SS_MAIN;

  cmsds_sub_config_s_type        *cmsds_config = cmsds_get_config_ptr(as_id);

  CM_ASSERT( sds_ptr != NULL );


  #if defined(FEATURE_3GPP_CSFB)

  /* For multi-sim targets, update the si_info_ptr with the multimode-sub info. */
  if (cmph_is_msim())
  {
    ss = cmph_map_as_id_to_cm_ss(as_id);
    CM_MSG_MED_2("LTE_SS, ss=%d, asubs_id=%d",ss, as_id);
  }

  if(sds_ptr->srv_ind_buf_ptr != NULL)
  {
    si_info_ptr = sds_ptr->srv_ind_buf_ptr;
  }
  else
  {
    si_info_ptr = cmss_intl_srv_info_ptr(ss);
  }


  /* Check if  CSFB available is recived,LTE PS attach success and
  ** CDMA subscription is available
  */

  if (si_info_ptr->srv_status == SYS_SRV_STATUS_SRV       &&
      si_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS     &&
      si_info_ptr->mode   ==  SYS_SYS_MODE_LTE            &&
      cmsds_is_mode_supp_ppcsfb(as_id)
   )
  {
    if((cmsds_config->ue_usage_setting == SYS_UE_USAGE_SETTING_DATA_CENTRIC &&
        sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_SMS_ONLY) ||
        (cmsds_config->ue_usage_setting == SYS_UE_USAGE_SETTING_VOICE_CENTRIC &&
        (sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_SMS_ONLY ||
         sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_CSFB_NOT_PREFFERED ||
         sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_BARRED ))
      )
    {
      /* PPCSFB call is not possible */
      CM_MSG_MED_2 ("Domsel:PPCSFB not possible ue_usage = %d,lte_cs_cap =%d ",\
                   cmsds_config->ue_usage_setting, sds_ptr->lte_cs_capability );
      return FALSE;
    } else
    {
      /* PPCSFB call is possible */
      return TRUE;
    }
  } else
  {
    /* PPCSFB call is not possible */
    CM_MSG_MED_3 ("Domsel:PPCSFB not possible mode=%d,status=%d,domain=%d",\
                  si_info_ptr->mode, si_info_ptr->srv_status,\
                  si_info_ptr->srv_domain);

    return FALSE;
  }
  #else
  return FALSE;
  #endif
} /* cmsds_is_ppcsfb_call_allowed */
/*lint +esym(529, ss_ptr) not referenced */
/*lint +esym(529, sds_ptr) not referenced */

/*===========================================================================

@FUNCTION cmsds_is_ppcsfb_possible

@DESCRIPTION
  Checks whether PPCSFB is possible or not. This function should be used for
  all domain sekection purposes.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
 boolean cmsds_is_ppcsfb_possible(
  sys_modem_as_id_e_type    as_id
)

{

  cmsds_s_type                   *sds_ptr  = cmsds_ptr();
     /* Point at service domain selection  object */
     /*lint -esym(529, sds_ptr) not referenced */

  sd_si_info_s_type              *si_info_ptr = NULL;

  cm_ss_e_type                    ss = CM_SS_MAIN;

  CM_ASSERT( sds_ptr != NULL );

  #if defined(FEATURE_3GPP_CSFB)

  if (!cmph_is_lte_capable_on_sub(as_id))
  {
    return FALSE;
  }

  ss = cmph_determine_ss_per_rat(as_id, SYS_SYS_MODE_LTE);
  CM_MSG_MED_2("LTE_SS, ss=%d, as_id=%d",ss, as_id);

  if(sds_ptr->srv_ind_buf_ptr != NULL)
  {
    si_info_ptr = sds_ptr->srv_ind_buf_ptr;
  }
  else
  {
    si_info_ptr = cmss_intl_srv_info_ptr(ss);
  }


  /* Check if  CSFB available is recived,LTE PS attach success and
  ** CDMA subscription is available
  */

  if (si_info_ptr->srv_status == SYS_SRV_STATUS_SRV       &&
      si_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS     &&
      si_info_ptr->mode   ==  SYS_SYS_MODE_LTE            &&
      sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_NO_VALUE &&
      cmsds_is_mode_supp_ppcsfb(as_id)
   )
  {
    CM_MSG_MED_2 ("Domsel:PPCSFB is possible ue_usage = %d,lte_cs_cap =%d ",\
                  cmsds_get_config_ptr(as_id)->ue_usage_setting, sds_ptr->lte_cs_capability );

    /* PPCSFB call is possible */
    return TRUE;
  } else
  {
    /* PPCSFB call is not possible */
    CM_MSG_MED_3 ("Domsel:PPCSFB not possible mode=%d,status=%d,domain=%d",\
                  si_info_ptr->mode, si_info_ptr->srv_status,\
                  si_info_ptr->srv_domain);

    return FALSE;
  }
  #else
  return FALSE;
  #endif
}


#if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X) || defined (FEATURE_DOMAIN_SELECTION)

/*===========================================================================

@FUNCTION cmsds_is_cs_voice_avail

@DESCRIPTION
  Checks if CS domain is available.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
boolean cmsds_is_cs_voice_avail(
  cm_call_csfb_e_type      *csfb_type,  
  sys_modem_as_id_e_type    as_id
)
{
  sd_si_info_s_type  si_info;
  sd_si_info_ptr_get (SD_SS_MAIN, &si_info);

  #if defined (FEATURE_3GPP_CSFB) && !defined (FEATURE_LTE_TO_1X)
  if (cmsds_is_ppcsfb_possible (as_id))
  {
    *csfb_type = CM_CALL_CSFB_TYPE_PPCSFB;
    return TRUE;
  }
  else
  {
    *csfb_type = CM_CALL_CSFB_TYPE_NONE;
    return FALSE;
  }
  #elif  !defined (FEATURE_3GPP_CSFB) && defined (FEATURE_LTE_TO_1X)
  if (cmsds_is_1xcsfb_avail())
  {
    *csfb_type = CM_CALL_CSFB_TYPE_1XCSFB;
    return TRUE;
  }
  else
  {
    *csfb_type = CM_CALL_CSFB_TYPE_NONE;
    return FALSE;
  }
  #elif defined (FEATURE_3GPP_CSFB) && defined (FEATURE_LTE_TO_1X)
  if (cmsds_is_ppcsfb_possible (as_id))
  {
    *csfb_type = CM_CALL_CSFB_TYPE_PPCSFB;
    return TRUE;
  }
  else if (cmsds_is_1xcsfb_avail ())
  {
    *csfb_type = CM_CALL_CSFB_TYPE_1XCSFB;
    return TRUE;
  }
  else
  {
    *csfb_type = CM_CALL_CSFB_TYPE_NONE;
    return FALSE;
  }
  #elif !defined (FEATURE_3GPP_CSFB) && !defined (FEATURE_LTE_TO_1X)
  /* No CSFB support */
  *csfb_type = CM_CALL_CSFB_TYPE_NONE;
  return FALSE;
  #endif

}

/*===========================================================================

@FUNCTION cmsds_dom_sel_get_csfb_type_allowed

@DESCRIPTION
 returns csfb type based on current selected domain.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

cm_call_csfb_e_type cmsds_dom_sel_get_csfb_type_allowed
(
  sys_modem_as_id_e_type  as_id,
      /* the subs ID of the call */
  
  cm_srv_type_e_type      srv_type
      /* the service type of the call */
)
{
  cmsds_s_type   *sds_ptr  = cmsds_ptr();
  /* Point at service domain selection  object */

  cm_call_csfb_e_type csfb_type = CM_CALL_CSFB_TYPE_NONE;
  if(cmsds_get_config_ptr(as_id)->allow_csfb_upon_ims_reg)
  {
    csfb_type = cmsds_get_csfb_type_allowed(as_id);
  }
  else if ((sds_ptr->curr_voice_dom_selected != SYS_CM_DOMAIN_SEL_DOMAIN_IMS) &&
     (sds_ptr->curr_voice_dom_selected != SYS_CM_DOMAIN_SEL_DOMAIN_NONE) &&
     (sds_ptr->curr_voice_dom_selected != SYS_CM_DOMAIN_SEL_DOMAIN_NO_VOICE) )
  {
    csfb_type = cmsds_get_csfb_type_allowed(as_id);
  }
  /* If emergency origination is happening, and current IMS system does
  ** not support emergency, allow call to proceed via CSFB if possible
  */
  else if (sds_ptr->curr_voice_dom_selected == SYS_CM_DOMAIN_SEL_DOMAIN_IMS &&
      !cmsds_is_current_sys_lte_ims_emergency_capable(as_id) &&
       (cmcall_is_there_a_call_type(CM_SS_MAX,CM_CALL_TYPE_EMERGENCY,
                                 CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID )
      )
  {
    CM_MSG_HIGH_1("LTE_911: check csfb cap for as_id = %d", as_id);
    csfb_type = cmsds_get_csfb_type_allowed(as_id);
  }
  /* If there is a call with srv_type CS_ONLY, allow call to proceed via CSFB if possible
  */
  else if( srv_type == CM_SRV_TYPE_CS_ONLY )
  {
    CM_MSG_HIGH_1("call with srv_type CS_ONLY --> check csfb cap for as_id %d", as_id);
    csfb_type = cmsds_get_csfb_type_allowed(as_id);
  }

  return csfb_type;
}

#endif


/*===========================================================================

FUNCTION cmsds_ptr

DESCRIPTION
  Return a pointer to the one and only domain selection object.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmsds_s_type *cmsds_ptr( void )
{
  static cmsds_s_type cmsds;       /* The one and only domain selection object */

  return &cmsds;

} /* *cmsds_ptr() */




/*===========================================================================

  FUNCTION cmsds_sd_rpt_proc

  DESCRIPTION
    Process reports from System Determination.


  DEPENDENCIES

  RETURN VALUE
    none

  SIDE EFFECTS
    none

===========================================================================*/
void cmsds_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
)
{

  /*lint -e{826} */
  const cm_sd_rpt_u_type  *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
    /* Pointer to SD report */

  cmsds_s_type             *sds_ptr        = cmsds_ptr();
  /* Point at service domain selection  object */

  sd_ss_mode_pref_e_type   mode_pref       = SD_SS_MODE_PREF_NONE;
  sys_modem_as_id_e_type   asub_id         = SYS_MODEM_AS_ID_1;
  sys_sys_mode_e_type       i;
  cm_ss_e_type              cm_ss = CM_SS_NONE;

  cmsds_sub_config_s_type  *cmsds_config;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( sds_ptr != NULL );
  CM_ASSERT( sd_rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sd_rpt_ptr->hdr.cmd == CM_SRV_IND_INFO_F)
  {
    mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(cmph_get_ss_mode_pref(cmph_map_sd_ss_type_to_cm_ss_type(sd_rpt_ptr->srv_ind_info.ss)));
    cm_ss     = cmph_map_sd_ss_type_to_cm_ss_type(sd_rpt_ptr->srv_ind_info.ss);
    asub_id   = cmph_map_cm_ss_to_subs(cm_ss);
  }

  cmsds_config = cmsds_get_config_ptr(asub_id);

  /* This is serving system change notifiction
  ** 1. Updating appropriate domain selection object fields.
  */
  switch( sd_rpt_ptr->hdr.cmd )
  {
    /* Service Indicators information is changed. */
    case CM_SRV_IND_INFO_F:
    {
      /* Skip the domain selection process if LTE is not on the ss */
      if (!cmph_is_lte_capable_on_ss(cm_ss))
      {
        break;
      }

       /*stop Tvolte_hys and reset VA type when UE camped on DO/G/W
      ** or when UE camp on 1x while UE is not in SRLTE mode */
      #ifdef FEATURE_DOMAIN_SELECTION
      if(((sd_rpt_ptr->srv_ind_info.si_info.mode == SYS_SYS_MODE_GSM ||
         sd_rpt_ptr->srv_ind_info.si_info.mode == SYS_SYS_MODE_WCDMA ||
         sd_rpt_ptr->srv_ind_info.si_info.mode == SYS_SYS_MODE_HDR)
          ||
         (sd_rpt_ptr->srv_ind_info.si_info.mode == SYS_SYS_MODE_CDMA &&
          (cmph_determine_ss_per_rat(asub_id, SYS_SYS_MODE_LTE) != CM_SS_HYBR_2)))
          &&
          sys_srv_status_is_srv(sd_rpt_ptr->srv_ind_info.si_info.srv_status)
       )
      {
        sds_ptr->va_type.va = CMSDS_VA_TYPE_UNKNOWN;
        cmsds_stop_tssac_timer();
        sys_plmn_undefine_plmn_id(&(sds_ptr->va_type.associated_plmn_id));
      }
      #endif

      switch(sd_rpt_ptr->srv_ind_info.ss)
      {
        case SD_SS_MAIN:
        {
          if ((sd_rpt_ptr->srv_ind_info.si_info.mode == SYS_SYS_MODE_LTE) &&
               (sd_rpt_ptr->srv_ind_info.si_info.is_stable_in_svc == TRUE) &&
               (sd_rpt_ptr->srv_ind_info.si_info.srv_status == SYS_SRV_STATUS_SRV) &&
               ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY) ||
                 (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED) ||
                 ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED) &&
                  (cmsds_is_ims_active(asub_id) == FALSE))
               )
              )
           {

             #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
                 defined( FEATURE_LTE_TO_1X )
             /* Check and  update SIB8 status */
             cmsds_process_sib8_status(&sd_rpt_ptr->srv_ind_info.si_info, asub_id);
             #endif
           }

            if((sd_rpt_ptr->srv_ind_info.si_info.mode != SYS_SYS_MODE_LTE) &&
             (sds_ptr->onexcsfb_status_not_avail_uptime != CMSDS_UPTIME_NONE))
            {
              CM_MSG_HIGH_0("Domsel:reset onexcsfb_status_not_avail_uptime as mode is not LTE");
              sds_ptr->onexcsfb_status_not_avail_uptime = CMSDS_UPTIME_NONE;
            }

           #ifdef FEATURE_DOMAIN_SELECTION
           /* Clear IMS registration status wait timer if SUB feature mode is NORMAL or SGLTE */
           if(((sd_rpt_ptr->srv_ind_info.si_info.mode != SYS_SYS_MODE_LTE) ||
                (sd_rpt_ptr->srv_ind_info.si_info.srv_status != SYS_SRV_STATUS_SRV))&&
               (sd_misc_get_common_mode_pref(mode_pref, SD_SS_MODE_PREF_LTE)) &&
              ((cmph_get_subs_feature_mode(asub_id) == SYS_SUBS_FEATURE_MODE_NORMAL) ||
               (cmph_get_subs_feature_mode(asub_id) == SYS_SUBS_FEATURE_MODE_SGLTE)))
           {
             cmsds_ims_reg_status_wait_timer_clear(CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_SMS_MASK);
           }
           #endif

           /* 1.If lose service(srv_status=NO_SRV), reset domain sel flags on :
                a. LTE if CM_SS_MAIN maps to multimode
                b. GSM WCDMA and CDMA if it doesnt.
              2.If srv_status=SRV and domain=CAMPED (when camped in received after RLF)
                reset domain sel flags only when it is multimode subs*/
           if (sd_rpt_ptr->srv_ind_info.si_info.srv_status != SYS_SRV_STATUS_SRV ||
              (sd_rpt_ptr->srv_ind_info.si_info.srv_status == SYS_SRV_STATUS_SRV &&
               sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_CAMPED ))
           {
             if(cmph_is_lte_capable_on_sub(asub_id))
             {
               if(!(cmss_is_hybr2_operational() && cmph_is_subs_feature_mode_1x_sxlte(asub_id )))
               {
                 cmsds_reset_ims_reg_and_voice_avail_status();
                 #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
                 defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
                 if(!(sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_CAMPED))
                 {
                   /* Deact req will be sent only if Activate sent when IMS was registered on LTE*/
                   cmsds_send_1xsrvcc_deactivate_req(PROT_DEACT_ACQ_OTHERS);
                 }
                 #endif
               }

               if(!(cmss_ptr()->info.hdr_hybrid))
               {
                 cmsds_reset_ims_reg_and_voice_avail_status();
               }
             }

             sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_CDMA] = SYS_IMS_REG_STATE_UNKNOWN;
             sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_GSM] = SYS_IMS_REG_STATE_UNKNOWN;
             sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_WCDMA] = SYS_IMS_REG_STATE_UNKNOWN;
             CM_MSG_HIGH_0("Domsel:Reset IMS reg status upon OOS ");


           }
           else
           {
             /* if acquired full service on one RAT, reset domain sel flags on other RATs */
             for(i=SYS_SYS_MODE_CDMA; i<SYS_SYS_MODE_MAX; i++)
             {
               if(sd_rpt_ptr->srv_ind_info.si_info.mode != i)
               {
                 if(i == SYS_SYS_MODE_LTE)
                 {
                   if((!cmss_is_hybr2_operational() && cmph_map_cm_ss_to_subs(CM_SS_HYBR_2)== asub_id)||
                       cmph_no_hybr2()||
                       cmph_map_cm_ss_to_subs(CM_SS_HYBR_2)!= asub_id
                   )
                   {
                     sds_ptr->lte_ims_voice_avail = CMSDS_IMS_VOICE_NONE;
					           #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
                     defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)

                     /* Deact req will be sent only if Activate sent when IMS was registered on LTE*/

                     cmsds_send_1xsrvcc_deactivate_req(PROT_DEACT_ACQ_OTHERS);

                     #endif
                   }
                   else
                   {
                     continue;
                   }
                 }

                 if(i == SYS_SYS_MODE_HDR && cmss_ptr()->info.hdr_hybrid)
                 {
                   continue;
                 }
                 sds_ptr->ims_reg_status_sms[i] = SYS_IMS_REG_STATE_UNKNOWN;
				 sds_ptr->ims_reg_status_voice[i] = SYS_IMS_REG_STATE_UNKNOWN;
               }
             }

           }
         }
         break;

       case SD_SS_HYBR_1:
         if ((sd_rpt_ptr->srv_ind_info.si_info.mode != SYS_SYS_MODE_HDR) ||
           (sd_rpt_ptr->srv_ind_info.si_info.srv_status != SYS_SRV_STATUS_SRV))
         {
          sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_HDR] =SYS_IMS_REG_STATE_UNKNOWN;
		  sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_HDR] = SYS_IMS_REG_STATE_UNKNOWN;
         }
         break;

       case SD_SS_HYBR_2:

          #if defined(FEATURE_MMODE_SC_SVLTE)||defined(FEATURE_MMODE_DUAL_SIM)

          #ifdef FEATURE_DOMAIN_SELECTION
          /* Clear IMS registration status wait timer if SUB feature mode is NORMAL or SGLTE */
          if(((sd_rpt_ptr->srv_ind_info.si_info.mode != SYS_SYS_MODE_LTE) ||
               (sd_rpt_ptr->srv_ind_info.si_info.srv_status != SYS_SRV_STATUS_SRV))&&
              (sd_misc_get_common_mode_pref(mode_pref, SD_SS_MODE_PREF_LTE)) &&
             ((cmph_get_subs_feature_mode(asub_id) == SYS_SUBS_FEATURE_MODE_NORMAL) ||
              (cmph_get_subs_feature_mode(asub_id) == SYS_SUBS_FEATURE_MODE_SVLTE)))
          {
            cmsds_ims_reg_status_wait_timer_clear(CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_SMS_MASK);
          }
          #endif

          if ((sd_rpt_ptr->srv_ind_info.si_info.srv_status != SYS_SRV_STATUS_SRV ||
               (sd_rpt_ptr->srv_ind_info.si_info.srv_status == SYS_SRV_STATUS_SRV &&
                sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_CAMPED)) &&
                cmph_is_lte_capable_on_ss(CM_SS_HYBR_2) &&
                cmss_is_hybr2_operational())
           {
             cmsds_reset_ims_reg_and_voice_avail_status();
             CM_MSG_HIGH_2("HYBR2 Domsel:Reset VOPS support on LTE %d sms_reg_status %d",\
                sds_ptr->lte_ims_voice_avail,sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_LTE] );

             #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
               defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
               if(!(sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_CAMPED))
               {
               /* Reset va_type to handle following Scenario :
               1)  UE operating in SRLTE mode .
               2)  UE acquired non-VOPS  LTE .
               3)  UE lost LTE service .
               4)  UE acquired VOPS LTE .
               5)  SSAC timer should not to be started ,
               as we already have certain time spent in OOS & Scanning new LTE . */             
               sds_ptr->va_type.va = CMSDS_VA_TYPE_UNKNOWN;                
			    /* Deact req will be sent only if Activate sent when IMS was registered on LTE*/
		    	 cmsds_send_1xsrvcc_deactivate_req(PROT_DEACT_ACQ_OTHERS);
               }
             #endif

           }
          #endif
          break;

       case SD_SS_HYBR_3:
       {
          #if defined (FEATURE_MMODE_SXLTE_G) && defined (FEATURE_DOMAIN_SELECTION)
          /* Clear IMS registration status wait timer if SUB feature mode is NORMAL or SGLTE */
          if(sd_rpt_ptr->srv_ind_info.si_info.mode != SYS_SYS_MODE_LTE ||
             sd_rpt_ptr->srv_ind_info.si_info.srv_status != SYS_SRV_STATUS_SRV
          )
          {
            cmsds_ims_reg_status_wait_timer_clear(CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_SMS_MASK);
          }

          /* If lose service, reset domain sel flags on all RATs */
          if (sd_rpt_ptr->srv_ind_info.si_info.srv_status != SYS_SRV_STATUS_SRV)
          {
            cmsds_reset_ims_reg_and_voice_avail_status();
                    
            sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_CDMA] = SYS_IMS_REG_STATE_UNKNOWN;
            sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_GSM] = SYS_IMS_REG_STATE_UNKNOWN;
            sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_WCDMA] = SYS_IMS_REG_STATE_UNKNOWN;
            sds_ptr->ims_reg_status_sms[SYS_SYS_MODE_TDS] = SYS_IMS_REG_STATE_UNKNOWN;
          }
          else
          {
            /* if acquired full service on one RAT, reset domain sel flags on other RATs */
            for(i=SYS_SYS_MODE_CDMA; i<SYS_SYS_MODE_MAX; i++)
            {
              if(sd_rpt_ptr->srv_ind_info.si_info.mode != i)
              { 
                if (i == SYS_SYS_MODE_LTE)
                {
                  sds_ptr->lte_ims_voice_avail = CMSDS_IMS_VOICE_NONE;
                }
                
                if(i == SYS_SYS_MODE_HDR && cmss_ptr()->info.hdr_hybrid)
                {
                  continue;
                }
                sds_ptr->ims_reg_status_sms[i] = SYS_IMS_REG_STATE_UNKNOWN;
                cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_UNKNOWN, i);
              }
            }
          
          }
          
          #endif
       }
       break;

      default:
        break;

      } /*switch sd_rpt_ptr->srv_ind.info.ss */
    }
    break;

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

    case CM_LTE_1X_IRAT_F:
	{
    
    #ifdef FEATURE_CM_DEBUG_BUFFER
    cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_LTE_1X_IRAT_F, NULL);
    #endif
	  #if defined( FEATURE_LTE_TO_1X )

      CM_MSG_MED_0("RXD: CM_LTE_1X_IRAT_F");

      if (sds_ptr->is_1xcsfb_activated == TRUE)
      {
        /* Reset the CSFB activation ans SIB8 status*/
        CM_MSG_HIGH_1("Domsel:Resetting the 1XCSFB activation. csfb_status %d",\
                     sds_ptr->csfb_status );
        sds_ptr->is_1xcsfb_activated = FALSE;

      } /* sds_ptr->is_1xcsfb_activated */
      #endif

      #if defined(FEATURE_1X_SRVCC)

	  if ( sds_ptr->is_cdma_srvcc_activated == TRUE)
      {
        /* Reset the SRVCC active mode*/
        CM_MSG_HIGH_1("Domsel:Resetting the 1XSRVCC mode during IRAT %d",\
                     sds_ptr->is_cdma_srvcc_activated );
        sds_ptr->is_cdma_srvcc_activated= FALSE;

	  }
      #endif
      #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
      if(cmph_is_subs_feature_mode_srlte(SYS_MODEM_AS_ID_1) &&
         (CM_CALL_ID_INVALID == cmcall_is_there_csfb_call(CM_SS_MAIN,
               CM_CALL_CSFB_TYPE_1XCSFB, CM_CALL_ID_INVALID)))
      {
        cmsds_ptr()->curr_voice_dom_selected = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;
        cmsds_ptr()->curr_sms_dom_selected = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;
        CM_MSG_HIGH_0("Reset voice and SMS domains during the L to 1X IRAT ");
      }
      #endif
	}
     break;
    #endif

    case CM_LTE_DO_IRAT_F:
    {
      if (cmph_is_subs_feature_mode_srlte(SYS_MODEM_AS_ID_1))
      {
        cmsds_ptr()->curr_voice_dom_selected = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;
        cmsds_ptr()->curr_sms_dom_selected = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;
        CM_MSG_HIGH_0("Reset voice and SMS domains during the L to DO IRAT");
      }
    }
    break;
    
    case CM_LPM_OK_F:
    case CM_RESET_OK_F:
    case CM_PWROFF_OK_F:
    {
      #ifdef FEATURE_DOMAIN_SELECTION
      CM_MSG_HIGH_1("Domsel:RXD: SD report [%d] ",sd_rpt_ptr->hdr.cmd);
      /* Reset global variable again to avoid race conditions during oprt mode
      ** change cmd processing
      */
      cmsds_reset();
	 
      /* external VOLTE call needs to be reset separately as 
         global DOMSEL reset is called during PS_DETACH/STOP_MODE */
      sds_ptr->ext_volte_call_active = FALSE;
      sds_ptr->ext_volte_call_asubs_id = SYS_MODEM_AS_ID_NONE;
	  
      #endif

    }
    break;
    default:
      break;
  } /* switch( sd_rpt_ptr->hdr.cmd ) */

  SYS_ARG_NOT_USED (sds_ptr);
  return;
} /* cmsds_sd_rpt_proc() */



#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
     defined( FEATURE_LTE_TO_1X )



/*===========================================================================
@FUNCTION cmsds_reset_1xcsfb_avail_timer

@DESCRIPTION
  Reset 1xcsfb not avail timer when sending stop mode to LTE.

@DEPENDENCIES
  None

@RETURNS
  None

@SIDE_EFFECT
  None

===========================================================================*/
void cmsds_reset_1xcsfb_avail_timer(void)
{

  #ifndef FEATURE_MMODE_REMOVE_1X
  cmsds_s_type              *sds_ptr = cmsds_ptr();
  /* Domain Selection pointer*/

  /* LTE stack is being stopped so reset the timer.
  */
  if( sds_ptr->onexcsfb_status_not_avail_uptime != CMSDS_UPTIME_NONE )
  {
    CM_MSG_HIGH_0("Domsel:Attemping to stop LTE stack, so reset the 1xcsfb not avail timer ");
    sds_ptr->onexcsfb_status_not_avail_uptime = CMSDS_UPTIME_NONE;
  }

  #endif
} /* cmsds_reset_1xcsfb_avail_timer */

/*===========================================================================

@FUNCTION cmsds_send_1xcsfb_deactivate_req

@DESCRIPTION
  Send CSFB deactivate request  to 1xCP.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_send_1xcsfb_deactivate_req (prot_deact_e_type deact_reason)
{

  #ifndef FEATURE_MMODE_REMOVE_1X
  mc_msg_type                    *mc_ptr = NULL;
    /* MC command pointer */
  cmsds_s_type              *sds_ptr = cmsds_ptr();
    /* Domain Selection pointer*/

  /* Domain selection is trying to deativate 1XCP from CSFB mode
  ** so reset the timer.
  */
  if( sds_ptr->onexcsfb_status_not_avail_uptime != CMSDS_UPTIME_NONE )
  {
    CM_MSG_HIGH_0("Domsel:Attemping to deact from 1XCSFB mode so reset the timer ");
    sds_ptr->onexcsfb_status_not_avail_uptime = CMSDS_UPTIME_NONE;
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (sds_ptr->is_1xcsfb_activated == FALSE)
  {
    CM_MSG_LOW_0( "Domsel:1XCP is not in CSFB mode");
    return;
  }

  mc_ptr                                = cm_mc_get_buf_else_err_fatal();

  CM_ASSERT( mc_ptr != NULL );

  mc_ptr->hdr.cmd                       = MC_CSFB_DEACT_F;
  mc_ptr->csfb_deact.deact_reason = deact_reason;
  sds_ptr->is_1xcsfb_deact_in_prog = TRUE;

  CM_MSG_HIGH_0( "Domsel:Send MC_CSFB_DEACT_F to 1XCP");

  cm_mc_send_cmd( mc_ptr );
  #else
  #error code not present
#endif
} /* cmsds_send_1xcsfb_deactivate_req */


/*===========================================================================

FUNCTION cmsds_deactivate_1xcsfb_protocol

DESCRIPTION
  This function handles the deregistation of 1XCSFB protocol.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmsds_deactivate_1xcsfb_protocol
(
  prot_trans_type     mmoc_tans_id,

  prot_deact_e_type   deact_reason
)
/*lint -e{818} cmd_ptr can't be const */
{
  cmsds_s_type              *sds_ptr        = cmsds_ptr();


  if (sds_ptr->is_1xcsfb_activated)
  {
    sds_ptr->mmoc_trans_id = mmoc_tans_id;


    /* Send Deactivation Req to 1x CP */
    cmsds_send_1xcsfb_deactivate_req (deact_reason);

  }
} /* cmsds_deactivate_1xcsfb_protocol*/

/*===========================================================================

FUNCTION cmsds_is_1xcsfb_activated

DESCRIPTION
  This function returns whether 1X is activated in CSFB mode or not..

DEPENDENCIES
  none

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmsds_is_1xcsfb_activated
(
  void
)
{
  cmsds_s_type              *sds_ptr        = cmsds_ptr();

  if (TRUE == sds_ptr->is_1xcsfb_activated)
  {
    return TRUE;
  }

  return FALSE;
} /* cmsds_is_1xcsfb_activated*/

/*===========================================================================

FUNCTION cmsds_check_and_deact_csfb_mode

DESCRIPTION
  This function returns deactiavtes CP from CSFB mode.

DEPENDENCIES
  none

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  none

===========================================================================*/

void cmsds_check_and_deact_csfb_mode
(
  void
)
{
  cmsds_s_type      *sds_ptr        = cmsds_ptr();

  if (cmsds_is_csfb_deact_allowed())
  {
    CM_MSG_HIGH_0( "Domsel:Trigger 1xcsfb deactivation");
    mmoc_cmd_deactivate_1xcsfb_prot (PROT_DEACT_ACQ_OTHERS);
    sds_ptr->is_1xcsfb_deact_in_prog = TRUE;
  }

  return;
}

/*===========================================================================

FUNCTION cmsds_mc_rpt_proc

DESCRIPTION
  Process MC reports


DEPENDENCIES
  Serving System object must have already been initialized with
  cmsds_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsds_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
)
{

  /* Pointer to MC report */
   /*lint -e{826} Suspicious pointer-to-pointer conversion (area too small) */
   cm_mc_rpt_type    *cm_mc_rpt_ptr = (cm_mc_rpt_type *) rpt_ptr;

   cmsds_s_type      *sds_ptr       = cmsds_ptr();
       /* Point at service domain selection  object */
   cm_ss_e_type      ss             = CM_SS_MAIN;

   cmss_s_type       *ss_ptr        = cmss_ptr();
  /* Serving system object */

  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;
  
  cmsds_sub_config_s_type  *cmsds_config = cmsds_get_config_ptr(as_id);
  
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   CM_ASSERT( cm_mc_rpt_ptr != NULL );
   CM_ASSERT( sds_ptr != NULL );
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ss_ptr->info.changed_fields &= (~CM_SS_VOICE_SUPPORT_ON_LTE_MASK);

   /* This is a MC notification, so:
   ** 1. Updating appropriate domain selection object fields.
   ** 2. Notifying clients of event.
   */
   switch( cm_mc_rpt_ptr->hdr.cmd )
   {
    /* 1XCSFB  deativation confirmation*/
     case CM_1XCSFB_DEACT_CNF_F:

       CM_MSG_MED_0("RXD: CM_1XCSFB_DEACT_CNF_F");
       /* Set the CSFB activation to FALSE*/
       sds_ptr->is_1xcsfb_activated = FALSE;

       /* Reset CSFB status, AS CM will place the call on native 1X. */
       sds_ptr->csfb_status = CM_1XCSFB_NONE;
       sds_ptr->is_1xcsfb_deact_in_prog = FALSE;

       /* Report CSFB Deactivation CNF to MMoC */
       mmoc_rpt_1xcsfb_prot_deactd_cnf(sds_ptr->mmoc_trans_id);

       /* End the calls in END_WAIT substate */
       cmcsfbcall_end_csfb_calls_with_end_wait_sub_state();

       break;

     case CM_1XCSFB_STATUS_F :
       CM_MSG_MED_3("RXD: CM_1XCSFB_STATUS_F, lte_1xcsfb_result = %d, curr csfb_status = %d, is_esr_supported = %d",
         cm_mc_rpt_ptr->cdma_csfb_status.lte_1xcsfb_result, sds_ptr->csfb_status,
         cm_mc_rpt_ptr->cdma_csfb_status.is_esr_supported);


       sds_ptr->is_esr_supported = cm_mc_rpt_ptr->cdma_csfb_status.is_esr_supported;

       if(sds_ptr->rrc_1xcsfb_enabled == FALSE)
       {
         CM_MSG_HIGH_0( "1xcsfb is disabled in NV");
         return;
       }

       CM_MSG_MED_2("RXD: CM_1XCSFB_STATUS_F,mode = %d srv_domain = %d",\
         cmss_intl_srv_info_ptr(ss)->mode, cmss_intl_srv_info_ptr(ss)->srv_domain);
       
       if((!INRANGE(cm_mc_rpt_ptr->cdma_csfb_status.lte_1xcsfb_result,
                    CM_1XCSFB_NOT_AVAIL, CM_1XCSFB_PREREG_SUCCESS)) ||
          (sds_ptr->is_1xcsfb_deact_in_prog == TRUE))
       {
         CM_MSG_HIGH_2( "Skip csfb_status due to Invalid lte_1xcsfb_result = %d or is_csfb_deact_in_progress = %d",\
                 cm_mc_rpt_ptr->cdma_csfb_status.lte_1xcsfb_result, sds_ptr->is_1xcsfb_deact_in_prog );
         return;
       }

       /* Reset lte_cs_capability if CSFB status is received from 1XCP
          If CSFB STATUS is recieved, it means that the NW is 1XCSFb network
          and can ignore the 3GPP reject causes */
       if (sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_BARRED)
       {
         sds_ptr->lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
         CM_MSG_MED_0("Domsel:Reset lte_cs_capability");
       }

       if ((sds_ptr->csfb_status == cm_mc_rpt_ptr->cdma_csfb_status.lte_1xcsfb_result) ||
            (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY))
       {
         /*No change in SIB8 status: Silently ignore the redundant CSFB_STATUS from 1XCP */
         return;
       }
       sds_ptr->csfb_status = cm_mc_rpt_ptr->cdma_csfb_status.lte_1xcsfb_result;

       if ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY) ||
           ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED) &&
           (cmsds_is_ims_active(as_id) == FALSE)) ||
           (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED))
       {

         #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
         if ((cm_mc_rpt_ptr->cdma_csfb_status.lte_1xcsfb_result == CM_1XCSFB_PREREG_SUCCESS) ||
            (cm_mc_rpt_ptr->cdma_csfb_status.lte_1xcsfb_result == CM_1XCSFB_AVAIL))
         {
           if(cmph_is_oprting_in_csfb_pref_srlte(as_id))
           {
             cmsoa_sv_switch_base(CMSOA_SV_OPRT_MODE_CSFB, CM_SS_MAIN,FALSE);
             break;
           }
         }
         #endif
       }


       /* Added to stop timer for IMS PREF and CS secondary */
       if( sds_ptr->onexcsfb_status_not_avail_uptime != CMSDS_UPTIME_NONE )
       {
         CM_MSG_HIGH_0("Domsel:Got 1xcsfb status so resetting timer ");
         sds_ptr->onexcsfb_status_not_avail_uptime = CMSDS_UPTIME_NONE;
       }

       if(cm_mc_rpt_ptr->cdma_csfb_status.lte_1xcsfb_result == CM_1XCSFB_NOT_AVAIL ||
                  cm_mc_rpt_ptr->cdma_csfb_status.lte_1xcsfb_result == CM_1XCSFB_PREREG_FAIL
                 )
       {
                  CM_MSG_MED_0( "Unforce Call-end preferences, to disable LTE imm. when 1XCSFB not possible ");
                  cmcsfbcall_unforce_hard_failure_preferences();

       }

       if ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY) ||
           ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED) &&
           (cmsds_is_ims_active(as_id) == FALSE)) ||
           (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED))
       {

        #ifdef FEATURE_DOMAIN_SELECTION
         if ((sds_ptr->csfb_status == CM_1XCSFB_PREREG_SUCCESS) ||
             (sds_ptr->csfb_status == CM_1XCSFB_AVAIL))
         {
           CM_MSG_MED_0("Domsel:Stay on LTE  using 1XCSFB for voice");
           if ((cmsds_is_ims_active(as_id) == TRUE) &&
              (cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED) &&
              (cmss_intl_srv_info_ptr(ss)->mode == SD_MODE_LTE) &&
              (cmss_intl_srv_info_ptr(ss)->srv_status == SYS_SRV_STATUS_SRV))
           {
             cmsds_send_ims_pref_changed_ind( SYS_SYS_MODE_LTE,CM_CALL_TYPE_NONE_MASK, CM_CALL_TYPE_VOICE_MASK,
              as_id);
           }
           /*Voice domain selection ind */
           cmsds_update_and_send_voice_dom_sel_ind (ss, SYS_CM_DOMAIN_SEL_DOMAIN_1X, SYS_SYS_MODE_LTE);
         }
         #endif

         if ( cmsds_is_1xcsfb_activation_allowed (as_id) == TRUE)
         {
           /* CSFB activation can be done now */
           cmsds_send_1xcsfb_activate_req ();
         }

         #ifdef FEATURE_DOMAIN_SELECTION
                 /* Decide on stay on/leave LTE based on 1XCSFB status
             Process NO 1XCSFB indication only when LTE FULL SRV is acquired */
         if ((cmss_intl_srv_info_ptr(ss)->mode == SD_MODE_LTE) &&
             (cmss_intl_srv_info_ptr(ss)->srv_status == SYS_SRV_STATUS_SRV) &&
             ((cmss_intl_srv_info_ptr(ss)->srv_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
              (cmss_intl_srv_info_ptr(ss)->srv_domain == SYS_SRV_DOMAIN_CS_PS))
            )
         {
           cmsds_process_1xcsfb_status(ss);

         }
         else if ((sds_ptr->csfb_status == CM_1XCSFB_NOT_AVAIL) ||
                  (sds_ptr->csfb_status == CM_1XCSFB_PREREG_FAIL))
         {
           cmsds_check_and_deact_csfb_mode();
         }
         #endif /* FEATURE_DOMAIN_SELECTION */

       }

       if(!sds_ptr->is_plmn_blocking_in_progress &&
         (cmss_intl_srv_info_ptr(ss)->mode == SD_MODE_LTE) &&
         (cmss_intl_srv_info_ptr(ss)->srv_status == SYS_SRV_STATUS_SRV) &&
         ((cmss_intl_srv_info_ptr(ss)->srv_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
          (cmss_intl_srv_info_ptr(ss)->srv_domain == SYS_SRV_DOMAIN_CS_PS))
         )
       {
         cmsds_process_sms_domain_selection(ss, SYS_SYS_MODE_LTE);
       }

       break;
       default:
       break;
   }
} /* cmsds_mc_rpt_proc */


#endif /*(FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900) && FEATURE_LTE_TO_1X */

/*===========================================================================

FUNCTION:cmsds_reset_during_detach

DESCRIPTION
Reset

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmsds_reset_during_detach
(
  void
)
{
  cmsds_s_type                   *sds_ptr = cmsds_ptr();
  cmph_s_type                    *ph_ptr = cmph_ptr();
  /*lint -esym(529, ph_ptr) not referenced */
  cmss_s_type                    *ss_ptr   = cmss_ptr();
  /* Pointer to serving system object. .*/

  sds_ptr->curr_sms_dom_selected = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;
  sds_ptr->curr_voice_dom_selected = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;
  cmsds_reset_ims_reg_and_voice_avail_status();
  sds_ptr->lte_ims_emerg_avail = SYS_IMS_LTE_EMERG_SUPP_UNKNOWN;
  sds_ptr->is_plmn_blocking_in_progress = FALSE;
  sds_ptr->is_plmn_blocking_delayed = FALSE;
  sds_ptr->is_temp_ims_reg_fail = FALSE;
  sds_ptr->sib8_available = FALSE;
  ss_ptr->info.voice_support_on_lte      = FALSE;
  ss_ptr->info.voice_domain              = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;
  ss_ptr->info.sms_domain                = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;

  #ifdef FEATURE_DOMAIN_SELECTION
  cmsds_ims_reg_status_wait_timer_clear(CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_SMS_MASK);
  #endif

  #ifdef FEATURE_LTE
  sds_ptr->onexcsfb_status_not_avail_uptime = CMSDS_UPTIME_NONE;
  /* Initilise voice support status */
  if (!(ph_ptr->main_stack_info.disable_call_type_mask & CM_CALL_TYPE_VOICE_MASK))
  {
    sds_ptr->voice_support_status = TRUE;
  }
  else
  {
    sds_ptr->voice_support_status = FALSE;
}
  #endif

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
   defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)

     cmsds_send_1xsrvcc_deactivate_req(PROT_DEACT_ACQ_OTHERS);
  #endif

  #ifdef FEATURE_DOMAIN_SELECTION
  cmsds_reset_srv_ind_buf_ptr();
  #endif
  sds_ptr->ims_wait_uptime = 0;
  sds_ptr->tssac_hys = 0;
  sds_ptr->va_type.va = CMSDS_VA_TYPE_UNKNOWN;
  sys_plmn_undefine_plmn_id(&(sds_ptr->va_type.associated_plmn_id));
}

/*===========================================================================

FUNCTION:cmsds_reset_during_stop_mode

DESCRIPTION
Reset

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmsds_reset_during_stop_mode
(
  prot_deact_e_type deact_reason
)
{
  if ((deact_reason == PROT_DEACT_PWR_DOWN)  ||
      (deact_reason == PROT_DEACT_RESET)     ||
      (deact_reason == PROT_DEACT_OFFLINE)   ||
      (deact_reason == PROT_DEACT_LPM)       ||
      (deact_reason == PROT_DEACT_SUBSC_CHGD) ||
      (deact_reason == PROT_DEACT_CAP_CHANGE))
  {
    #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
      defined( FEATURE_LTE_TO_1X )
    if (cmsds_is_csfb_deact_allowed())
    {
      cmsds_send_1xcsfb_deactivate_req (PROT_DEACT_ACQ_OTHERS);
    }
    #endif

    #ifdef FEATURE_DOMAIN_SELECTION
    cmsds_reset_during_detach();
    cmsds_reset();
    #endif
  }
}

/*===========================================================================

FUNCTION cmsds_reset

DESCRIPTION
  Initializes the domain selection object.

  This function must be called before the before domain selection object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmsds_reset
(
  void
)
{
  cmsds_s_type                   *sds_ptr = cmsds_ptr();


  CM_MSG_HIGH_0("DOM_SEL:Reset domain selection globals ");
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
   defined( FEATURE_LTE_TO_1X )
  sds_ptr->csfb_status = CM_1XCSFB_NONE;
  sds_ptr->is_1xcsfb_activated = FALSE;
  sds_ptr->pending_uemode_cmsds_skipped = FALSE;
  #endif


  cmsds_reset_during_detach();
}
/*lint +esym(529, ph_ptr) not referenced */

/*===========================================================================

FUNCTION cmsds_init

DESCRIPTION
  Initializes the domain selection object.

  This function must be called before the before domain selection object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsds_init
(
  boolean           is_update_info,
  uint8             refresh_subs_mask
)
{
cmsds_s_type                      *sds_ptr = cmsds_ptr();
cmph_s_type                       *ph_ptr = cmph_ptr();
int32                              read_size = 0;
uint8 i         = 0;
int32 j         = 0;
cm_policy_config_s_type             *pm_ptr = cmpm_ptr();

#ifdef FEATURE_LTE
lte_3gpp_release_ver_e         lte_rel_version;
#endif

if(is_update_info)
{
#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
     defined( FEATURE_LTE_TO_1X )
  sds_ptr->csfb_status = CM_1XCSFB_NONE;
#endif
  sds_ptr->sib8_available = FALSE;
  sds_ptr->is_1xcsfb_activated = FALSE;
  sds_ptr->is_activation_in_progress = FALSE;

#ifdef FEATURE_DOMAIN_SELECTION
  sds_ptr->curr_sms_dom_selected = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;
  sds_ptr->curr_voice_dom_selected = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;
  memset(sds_ptr->ims_reg_status_sms,SYS_IMS_REG_STATE_UNKNOWN, sizeof(sds_ptr->ims_reg_status_sms));
  memset(sds_ptr->ims_reg_status_voice,SYS_IMS_REG_STATE_UNKNOWN, sizeof(sds_ptr->ims_reg_status_voice));
  sds_ptr->lte_ims_voice_avail = CMSDS_IMS_VOICE_NONE;
  sds_ptr->lte_ims_emerg_avail = SYS_IMS_LTE_EMERG_SUPP_UNKNOWN;
  sds_ptr->is_plmn_blocking_in_progress = FALSE;
  sds_ptr->is_plmn_blocking_delayed = FALSE;
  sds_ptr->pending_uemode_cmsds_skipped = FALSE;
  sds_ptr->is_temp_ims_reg_fail = FALSE;
#endif
  #ifdef FEATURE_LTE
  sds_ptr->onexcsfb_status_not_avail_uptime = CMSDS_UPTIME_NONE;
  /* Initilise voice support status */
  if (!(ph_ptr->main_stack_info.disable_call_type_mask & CM_CALL_TYPE_VOICE_MASK))
  {
    sds_ptr->voice_support_status = TRUE;
  }
  else
  {
    sds_ptr->voice_support_status = FALSE;
  }
    #endif
    #ifdef FEATURE_LTE_TO_HDR_OH
    sds_ptr->hdr_tunnel_status = CM_HDR_TUNNEL_CMD_NONE;
    sds_ptr->is_act_hdr_on_srv_cnf = FALSE;
    #endif
    sds_ptr->sib8_not_avail_duration = (uint32)((sds_ptr->n_maxSIB8 + 1)* 5.12);

    sds_ptr->lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;

    /* Set LTE IMS emergency value */
    sds_ptr->lte_ims_emerg_avail = SYS_IMS_LTE_EMERG_SUPP_UNKNOWN;
    sds_ptr->srv_ind_buf_ptr = NULL;

    sds_ptr->tssac_hys = 0;
    sds_ptr->ims_wait_uptime = 0;
    sds_ptr->va_type.va = CMSDS_VA_TYPE_UNKNOWN;
    sys_plmn_undefine_plmn_id(&(sds_ptr->va_type.associated_plmn_id));
    sds_ptr->ext_volte_call_active = FALSE;
    sds_ptr->ext_volte_call_asubs_id = SYS_MODEM_AS_ID_NONE;

  }
  #ifdef FEATURE_DOMAIN_SELECTION
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
      sds_ptr->config[i].t3402_value = CMSDS_DISABLE_LTE_DURATION_MIN_VALUE;
    }
  }
  #endif

  #ifdef FEATURE_LTE

  read_size = sizeof (boolean);
  /* Read the EFS file sms_only */
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    if ( cmefs_get ( CMEFS_SMS_ONLY,
                     &sds_ptr->config[i].is_sms_only,
                     read_size,
                     (sys_modem_as_id_e_type)i) )
    {
      CM_MSG_HIGH_2 ("Domsel: Sub %d read /nv/item_files/modem/mmode/sms_only = %d",
                     i, sds_ptr->config[i].is_sms_only);
      if (!(ph_ptr->main_stack_info.disable_call_type_mask & CM_CALL_TYPE_VOICE_MASK) &&
            sds_ptr->config[i].is_sms_only)
    {
        CM_ERR_2("Domsel: Wrong config /nv/item_files/modem/mmode/sms_only, disable_call_type_mask %x, is_sms_only %d",\
                 ph_ptr->main_stack_info.disable_call_type_mask, sds_ptr->config[i].is_sms_only);
    }
  }
  else
  {
      CM_MSG_HIGH_1("Domsel: Sub %d read /nv/item_files/modem/mmode/sms_only failed", i);
    /* set default value to FALSE*/
      sds_ptr->config[i].is_sms_only = FALSE;
    }
  }
  }

  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    if ((cmph_get_sub_config_ptr((sys_modem_as_id_e_type)i)->active_ims_stack ==
           SYS_IMS_STACK_STATUS_ENABLED_QCOM) ||
           (cmph_get_sub_config_ptr((sys_modem_as_id_e_type)i)->active_ims_stack ==
             SYS_IMS_STACK_STATUS_ENABLED_THPY))
    {
      sds_ptr->config[i].is_ims_enabled = TRUE;
    }
    else
    {
      sds_ptr->config[i].is_ims_enabled = FALSE;
    }
    }
  }

  read_size = sizeof(uint8);
  /* Read the EFS file qipcall_domain_selection_enable */
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    if ( cmefs_get ( CMEFS_QIPCALL_DOM_SEL_ENABLE,
                       (byte *)&sds_ptr->config[i].is_qip_call_dom_enabled,
                       read_size,
                      (sys_modem_as_id_e_type)i) )
  {
      CM_MSG_HIGH_2 ("Domsel: Sub %d read /nv/item_files/ims/qipcall_domain_selection_enable = %d",
                     i, sds_ptr->config[i].is_qip_call_dom_enabled);
  }
  else
  {
      CM_MSG_HIGH_1("Domsel: Sub %d read /nv/item_files/ims/qipcall_domain_selection_enable failed", i);
    /* set default value to FALSE*/
      sds_ptr->config[i].is_qip_call_dom_enabled = FALSE;
    }
  }
  }

  read_size = sizeof (sys_ue_usage_setting_e_type);
  /* Read the EFS file  ue_usage_setting*/
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    if ( cmefs_get (CMEFS_UE_USAGE_SETTING,
                    (byte*)&sds_ptr->config[i].ue_usage_setting,
                    read_size,
                    (sys_modem_as_id_e_type)i) )
  {
      CM_MSG_HIGH_2("Domsel: Sub %d read /nv/item_files/modem/mmode/ue_usage_setting = %d",\
                    i, sds_ptr->config[i].ue_usage_setting );
  }
  else
  {
      CM_MSG_HIGH_1("Domsel: Sub %d read /nv/item_files/modem/mmode/ue_usage_setting failed", i);
    /* set default value to SYS_UE_USAGE_SETTING_VOICE_CENTRIC */
      sds_ptr->config[i].ue_usage_setting = SYS_UE_USAGE_SETTING_VOICE_CENTRIC;
    }
  }
  }

  read_size = sizeof (boolean);
  if(refresh_subs_mask & BM(SYS_MODEM_AS_ID_1))
  {
  /* Read the EFS file  lte_rrc_1xcsfb_enabled*/
  if ( cmefs_get2 ( "/nv/item_files/modem/lte/rrc/lte_rrc_1xcsfb_enabled",
                    (byte*)&sds_ptr->rrc_1xcsfb_enabled,
                    read_size,
                    SYS_MODEM_AS_ID_1) )
  {
    CM_MSG_HIGH_1 ("Domsel: Read /nv/item_files/modem/lte/rrc/lte_rrc_1xcsfb_enabled = %d",\
                  sds_ptr->rrc_1xcsfb_enabled );
  }
  else
  {
    /* set default value to TRUE */
    sds_ptr->rrc_1xcsfb_enabled = TRUE;

    CM_ERR_0 ("Domsel:Can't read /nv/item_files/modem/lte/rrc/lte_rrc_1xcsfb_enabled");
  }
  }
  /* Update UE mode of operation preference */
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    cmph_update_lte_ue_mode_of_operation((sys_modem_as_id_e_type)i);
  }
  }

  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    if (cmpmprx_get_voice_domain_pref((sys_modem_as_id_e_type)i, &sds_ptr->config[i].voice_domain_pref,
      &sds_ptr->config[i].is_voice_domain_pref_locked))
    {
      CM_MSG_HIGH_2 ("Domsel: Sub %d got voice_domain_pref = %d from PM",\
                     i, sds_ptr->config[i].voice_domain_pref );
    }
    else
    {
      /* Set default value for voice_domain_pref */
      #if (defined(FEATURE_3GPP_CSFB) && !defined (FEATURE_IP_CALL))

      sds_ptr->config[i].voice_domain_pref = SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY;

      #elif (defined(FEATURE_3GPP_CSFB) && defined (FEATURE_IP_CALL) && defined (FEATURE_LTE))

      sds_ptr->config[i].voice_domain_pref = SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED;

      #elif (!defined(FEATURE_3GPP_CSFB) && defined (FEATURE_IP_CALL) && defined (FEATURE_LTE))

      sds_ptr->config[i].voice_domain_pref = SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY;

      #else

      sds_ptr->config[i].voice_domain_pref = SYS_VOICE_DOMAIN_PREF_NONE;

      #endif

      sds_ptr->config[i].is_voice_domain_pref_locked = FALSE;
      CM_MSG_HIGH_2 ("Domsel: Sub %d cmpmprx_get_voice_domain_pref() failed, voice_domain_pref = %d",
                     i, sds_ptr->config[i].voice_domain_pref);
    }
    }
  }

  /* Read the EFS file  sms_mandatory */
  read_size = sizeof (boolean);
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    if ( cmefs_get ( CMEFS_SMS_MANDATORY,
                     (byte*)&sds_ptr->config[i].is_sms_mandatory,
                    read_size,
                     (sys_modem_as_id_e_type)i) )
    {
      CM_MSG_HIGH_2 ("Domsel: Sub %d read /nv/item_files/modem/mmode/sms_mandatory = %d",\
                     i, sds_ptr->config[i].is_sms_mandatory );
    }
    else
    {
      /* set default value to sms_mandatory */
      sds_ptr->config[i].is_sms_mandatory = FALSE;
      CM_MSG_HIGH_1 ("Domsel: Sub %d read /nv/item_files/modem/mmode/sms_mandatory failed", i);
    }
  }
  }

   /* Read sms_domain_pref_list */
  read_size = sizeof (cm_sms_pref_s_type);
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    if ( cmefs_get ( CMEFS_SMS_DOMAIN_PREF_LIST,
                     (byte*)sds_ptr->config[i].sms_domain_pref,
                    read_size,
                     (sys_modem_as_id_e_type)i) )
    {
      CM_MSG_HIGH_1 ("Domsel: Sub %d read /nv/item_files/modem/mmode/sms_domain_pref_list", i);
      for(j=0;j<SYS_SYS_MODE_MAX;j++)
      {
        CM_MSG_HIGH_1 ("%d", sds_ptr->config[i].sms_domain_pref[j]);
      }
    }
    else
    {
      /* If sms_dom_pref_list is not configured or reading fail fallback to legacy NV*/
      sys_sms_domain_pref_e_type lte_sms_pref;

      /* set default value to sms_domain_pref */
      CM_MSG_HIGH_1 ("Domsel: Sub %d read /nv/item_files/modem/mmode/sms_domain_pref_list failed", i);
      memset(sds_ptr->config[i].sms_domain_pref,
             SYS_SMS_DOMAIN_PREF_PS_SMS_NOT_ALLOWED,
             sizeof(sds_ptr->config[i].sms_domain_pref));

     /* Read the EFS file  sms_domain_pref */
      read_size = sizeof (sys_sms_domain_pref_e_type);

      if ( cmefs_get ( CMEFS_SMS_DOMAIN_PREF,
                     (byte*)&lte_sms_pref,
                      read_size,
                       (sys_modem_as_id_e_type)i) )
      {
        CM_MSG_HIGH_2 ("Domsel: Sub %d read lte sms_domain_pref = %d", i, lte_sms_pref);
        for(j=0;j<SYS_SYS_MODE_MAX;j++)
        {
          sds_ptr->config[i].sms_domain_pref[j] = lte_sms_pref;
        }
      }
    }
  }
  }

  #if defined (FEATURE_DOMAIN_SELECTION)
  read_size = sizeof (uint32);
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
  /* Read the EFS file */
    if ( cmefs_get(CMEFS_LTE_DISABLE_DURATION,
                     (byte*)&sds_ptr->config[i].lte_disable_duration_nv,
                    read_size,
                     (sys_modem_as_id_e_type)i) )
    {
      CM_MSG_HIGH_2("Domsel: Sub %d read /nv/item_files/modem/mmode/lte_disable_duration = %d sec",
                      i, sds_ptr->config[i].lte_disable_duration_nv);

      /* If configured LTE disable duration is less then minimum value*/
      if ((sds_ptr->config[i].lte_disable_duration_nv > 0) &&
          (sds_ptr->config[i].lte_disable_duration_nv < CMSDS_DISABLE_LTE_DURATION_MIN_VALUE))
      {
         CM_MSG_MED_2("Domsel: configured value for lte_disable_duration = %d sec is less than min value %d",
                        sds_ptr->config[i].lte_disable_duration_nv,
                        CMSDS_DISABLE_LTE_DURATION_MIN_VALUE);

       /* Overwrite with minimum value*/
         sds_ptr->config[i].lte_disable_duration = CMSDS_DISABLE_LTE_DURATION_MIN_VALUE;
      }
      else
      {
        sds_ptr->config[i].lte_disable_duration = sds_ptr->config[i].lte_disable_duration_nv;
      }
    }
    else
    {
      CM_MSG_HIGH_1("Domsel: Sub %d read /nv/item_files/modem/mmode/lte_disable_duration failed", i);
    /* set default value to till power cycle*/
      sds_ptr->config[i].lte_disable_duration_nv = CMSDS_DISABLE_LTE_DURATION_INVALID;
      sds_ptr->config[i].lte_disable_duration = CMSDS_DISABLE_LTE_DURATION_INVALID;
    }
  }
  }

  #endif /* FEATURE_DOMAIN_SELECTION */
  read_size = sizeof (uint8);
  /* Read the EFS file */
  if(refresh_subs_mask & BM(SYS_MODEM_AS_ID_1))
  {
  if ( cmefs_get ( CMEFS_SUPPORT_DUAL_RX_UE,
                   (byte*)&sds_ptr->supportfordualrxues,
                    read_size,
                    SYS_MODEM_AS_ID_1) )
  {
    CM_MSG_HIGH_1 ("Domsel: Read /nv/item_files/modem/mmode/supportfordualrxues = %d",
                       sds_ptr->supportfordualrxues);
  }
  else
  {
    CM_ERR_0("Domsel: Can't read /nv/item_files/modem/mmode/supportfordualrxues");
    /* set default value to TRUE */
    sds_ptr->supportfordualrxues = TRUE ;
  }
  }
  #if defined (FEATURE_LTE_TO_1X)
  /* Read the EFS file */
  if(refresh_subs_mask & BM(SYS_MODEM_AS_ID_1))
  {
  if ( cmefs_get ( CMEFS_DUAL_RX_1XCSFB,
                   (byte*)&sds_ptr->is_dual_rx_1xcsfb,
                    read_size,
                    SYS_MODEM_AS_ID_1) )
  {
    CM_MSG_HIGH_1 ("Domsel: Read /nv/item_files/modem/lte/rrc/cap/dual_rx_1x_csfb = %d",
                       sds_ptr->is_dual_rx_1xcsfb);
  }
  else
  {
    CM_MSG_MED_0("Domsel: Can't read /nv/item_files/modem/lte/rrc/cap/dual_rx_1x_csfb");
    /* set default value to FALSE */
    sds_ptr->is_dual_rx_1xcsfb = FALSE ;
  }
  }
  #endif

  read_size = sizeof (uint8);
  /* Read the EFS file */
  if(refresh_subs_mask & BM(SYS_MODEM_AS_ID_1))
  {
  if ( cmefs_get ( CMEFS_N_MIN_MO_CALL_SOFT_RETRY,
                   (byte*)&sds_ptr->n_min_MO_call_soft_retry,
                    read_size,
                    SYS_MODEM_AS_ID_1) )
  {
    CM_MSG_HIGH_1 ("Domsel: Read /nv/item_files/modem/mmode/n_min_MO_call_soft_retry = %d",
                      sds_ptr->n_min_MO_call_soft_retry);
  }
  else
  {
    CM_MSG_MED_0("Domsel: Can't read /nv/item_files/modem/mmode/n_min_MO_call_soft_retry ");
    /* set default value */
    sds_ptr->n_min_MO_call_soft_retry = CM_1XCSFB_CALL_MAX_SOFT_FAIL_RETRIES ;
  }
  }

  read_size = sizeof (uint8);
  /* Read the EFS file */
  if(refresh_subs_mask & BM(SYS_MODEM_AS_ID_1))
  {
  if ( cmefs_get ( CMEFS_N_MAX_SIB8_READ_ATTEMPTS,
                   (byte*)&sds_ptr->n_maxSIB8,
                    read_size,
                    SYS_MODEM_AS_ID_1) )
  {
    CM_MSG_HIGH_1 ("Domsel: Read /nv/item_files/modem/mmode/n_maxSIB8 = %d",
                      sds_ptr->n_maxSIB8);
  }
  else
  {
    CM_MSG_MED_0("Domsel: Can't read /nv/item_files/modem/mmode/n_maxSIB8 ");
    /* set default value */
    sds_ptr->n_maxSIB8 = CMSDS_DEFAULT_N_MAX_SIB8 ;
  }
  }

  read_size = sizeof(lte_3gpp_release_ver_e);
  /* Read LTE release version and update if Release 9 or greater is supported
  */
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    if(cmefs_get(CMEFS_LTE_3GPP_RELEASE_VER,
                  (byte*)&lte_rel_version,
                  read_size,
                   (sys_modem_as_id_e_type)i))
    {
      CM_MSG_HIGH_2("Domsel: Sub %d 3GPP_SPEC = %d", i, lte_rel_version);
    }
    else
    {
      CM_MSG_HIGH_1("Domsel: Sub %d read CMEFS_LTE_3GPP_RELEASE_VER failed", i);
      lte_rel_version = LTE_3GPP_DEFAULT_RELEASE_VERSION ;
    }

    if (lte_rel_version >= LTE_3GPP_REL9)
    {
      sds_ptr->config[i].is_lte_release9_supp = TRUE;
    }
    else
    {
      sds_ptr->config[i].is_lte_release9_supp = FALSE;
    }
    CM_MSG_HIGH_2("Domsel: Sub %d is_lte_release9_supp %d", i,
                    sds_ptr->config[i].is_lte_release9_supp);
  }
  }
  #endif /* FEATURE_LTE */

  read_size = sizeof(boolean);
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    /* Read the EFS file */
    if ( cmefs_get ( CMEFS_SMS_OVER_S102,
                     (byte*)&sds_ptr->config[i].is_sms_over_s102_supported,
                     read_size,
                     (sys_modem_as_id_e_type)i) )
    {
      CM_MSG_HIGH_2 ("Domsel: Sub %d read /nv/item_files/modem/mmode/sms_over_s102 = %d",\
                     i, sds_ptr->config[i].is_sms_over_s102_supported );
    }
    else
    {
      /* set default value of sms_over_s102 to FALSE */
      sds_ptr->config[i].is_sms_over_s102_supported = FALSE;
      CM_MSG_HIGH_1 ("Domsel: Sub %d read /nv/item_files/modem/mmode/sms_over_s102 failed", i);
    }
  }
  }

  read_size = sizeof(uint32);
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    /* Read the EFS file */
    if ( cmefs_get ( CMEFS_IMS_REG_STATUS_WAIT_TIMER,
                     (byte*)&sds_ptr->config[i].ims_reg_status_wait_timer,
                     read_size,
                     (sys_modem_as_id_e_type)i) )
    {
      CM_MSG_HIGH_2 ("Domsel: Sub %d read /nv/item_files/modem/mmode/ims_reg_status_wait_timer = %d",\
                     i, sds_ptr->config[i].ims_reg_status_wait_timer );
    }
    else
    {
      /* set default value of ims_reg_status_wait_timer to CMSDS_IMS_REG_STATUS_WAIT_TIMER */
      sds_ptr->config[i].ims_reg_status_wait_timer = CMSDS_IMS_REG_STATUS_WAIT_TIMER;
      CM_MSG_HIGH_1 ("Domsel: Sub %d read /nv/item_files/modem/mmode/ims_reg_status_wait_timer failed", i);
    }

    if ((sds_ptr->config[i].ims_reg_status_wait_timer > 0) &&
        (sds_ptr->config[i].ims_reg_status_wait_timer < CMSDS_IMS_REG_STATUS_WAIT_TIMER))
    {
      sds_ptr->config[i].ims_reg_status_wait_timer = CMSDS_IMS_REG_STATUS_WAIT_TIMER;
    }
  }
  }

  read_size = sizeof(boolean);
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    if(cmefs_get(CMEFS_IS_PLMN_BLOCK_REQ_IN_LTE_ONLY_MODE,
                &sds_ptr->config[i].is_plmn_block_req_in_lte_only_mode,
                read_size,
                (sys_modem_as_id_e_type)i) )
    {
      CM_MSG_HIGH_2("Domsel: Sub %d read CMEFS_IS_PLMN_BLOCK_REQ_IN_LTE_ONLY_MODE = %d",\
                      i, sds_ptr->config[i].is_plmn_block_req_in_lte_only_mode);
    }
    else
    {
      sds_ptr->config[i].is_plmn_block_req_in_lte_only_mode = FALSE;
      CM_MSG_HIGH_1("Domsel: Sub %d read CMEFS_IS_PLMN_BLOCK_REQ_IN_LTE_ONLY_MODE failed", i);
    }
  }
  }

  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    *(CMPH_SS_UE_USAGE_SETTING_PTR((sys_modem_as_id_e_type)i, ph_ptr)) = sds_ptr->config[i].ue_usage_setting;

    for( j=0; j < NV_MAX_NAMS; j++ )
    {
      *(CMPH_PRST_UE_USAGE_SETTING_NAM_PTR(j, (sys_modem_as_id_e_type)i, ph_ptr)) =
                              sds_ptr->config[i].ue_usage_setting;
    }
  }
  }

  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    *(CMPH_SS_VOICE_DOMAIN_PREF_PTR((sys_modem_as_id_e_type)i, ph_ptr)) = sds_ptr->config[i].voice_domain_pref;

    for( j=0; j < NV_MAX_NAMS; j++ )
    {
      *(CMPH_PRST_VOICE_DOMAIN_PREF_NAM_PTR(j, (sys_modem_as_id_e_type)i, ph_ptr)) =
                              sds_ptr->config[i].voice_domain_pref;
    }
  }
  }

  read_size = sizeof(boolean);
  for(i = 0; i < MIN(ARR_SIZE(sds_ptr->config), pm_ptr->device_mode.nSims) ; i++)
  {
    if(refresh_subs_mask & BM(i))
    {
    /* Read the EFS file */
    if ( cmefs_get ( CMEFS_ALLOW_CSFB_UPON_IMS_REG,
                     (byte*)&sds_ptr->config[i].allow_csfb_upon_ims_reg,
                    read_size,
                     (sys_modem_as_id_e_type)i) )
    {
      CM_MSG_HIGH_2 ("Domsel: Sub %d read /nv/item_files/modem/mmode/allow_csfb_upon_ims_reg = %d",\
                     i, sds_ptr->config[i].allow_csfb_upon_ims_reg );
    }
    else
    {
      /* set default value of allow_csfb_upon_ims_reg to FALSE */
      sds_ptr->config[i].allow_csfb_upon_ims_reg = FALSE;
      CM_MSG_HIGH_1 ("Domsel: Sub %d read /nv/item_files/modem/mmode/allow_csfb_upon_ims_reg failed", i);
    }
  }
  }

  /** Read from NV is MT CSFB call redial mechanism supported
  */
  read_size = sizeof(boolean);
  if(refresh_subs_mask & BM(SYS_MODEM_AS_ID_1))
  {
  cmnv_efs_read(CMNV_EFS_ID_MTCSFB_CALL_REDIAL_OVER_1X,
                (byte*)&sds_ptr->is_mtcsfb_call_redial_supported,
                &read_size);
  if ( read_size > 0 )
  {
    CM_MSG_HIGH_1 ("EFS read succ. is_mtcsfb_call_redial_supported= %d",
                   sds_ptr->is_mtcsfb_call_redial_supported);
  }
  }
  SYS_ARG_NOT_USED (read_size);

  /* Mark object as initialized.
  */
  CM_INIT_MARK( sds_ptr->init_mask );


} /* cmsds_init */


#ifdef FEATURE_DOMAIN_SELECTION
/*===========================================================================

FUNCTION cmsds_ims_reg_status_wait_timer_expiry

DESCRIPTION: Processes IMS REGISTRATION timer expiry.

DEPENDENCIES  none

RETURN VALUE

NONE

SIDE EFFECTS

none

===========================================================================*/
static void cmsds_ims_reg_status_wait_timer_expiry(dword uptime)
{
  #ifdef FEATURE_DOMAIN_SELECTION
  cm_sds_cmd_u_type   reg_status;

  cmsds_s_type        *sds_ptr = cmsds_ptr();

  cm_call_type_mask_type   call_type_mask = CM_CALL_TYPE_NONE_MASK;

  if(sds_ptr->ims_wait_uptime <= uptime &&
     sds_ptr->ims_wait_uptime > 0)
  {
    call_type_mask |= CM_CALL_TYPE_VOICE_MASK;
    sds_ptr->ims_wait_uptime = 0;
  }

  if (call_type_mask != CM_CALL_TYPE_NONE_MASK)
  {
    CM_MSG_HIGH_1("IMS_REG: WAIT TIMER is expired for %d", call_type_mask);

    reg_status.cm_domain_sel_update_ims_reg_status_Ind.data.system = SYS_SYS_MODE_LTE;

    reg_status.cm_domain_sel_update_ims_reg_status_Ind.data.call_type_mask = call_type_mask;

    reg_status.cm_domain_sel_update_ims_reg_status_Ind.data.is_registered  = FALSE;

    reg_status.cm_domain_sel_update_ims_reg_status_Ind.data.end_cause      = SYS_IMS_REG_END_CAUSE_FAIL;

    reg_status.cm_domain_sel_update_ims_reg_status_Ind.msg_hdr.inst_id 
      = SYS_AS_ID_TO_INST_ID(cmph_get_sub_with_lte_cap());

    cmsds_process_ims_reg_status(&reg_status);
  }
  #endif
}

/*===========================================================================

FUNCTION cmsds_timer_proc

DESCRIPTION
  Process timer events.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsds_timer_proc(

    cm_timer_event_type    timer_event
        /* Indicate specific timer event */
)
{
  dword                 uptime  = time_get_uptime_secs() ;
  cmsds_s_type         *sds_ptr = cmsds_ptr();
  sd_si_info_s_type     si_info;
  sys_modem_as_id_e_type lte_asubs_id = cmph_get_sub_with_lte_cap();
  cm_ss_e_type          lte_ss = cmph_determine_ss_per_rat(lte_asubs_id, SYS_SYS_MODE_LTE);
  cmsds_sub_config_s_type *cmsds_config = cmsds_get_config_ptr(lte_asubs_id);

  #if defined( FEATURE_LTE_TO_1X )
  cmss_s_type    *ss_ptr  = cmss_ptr();
  #endif
  cm_call_type_mask_type cm_call_type_mask = CM_CALL_TYPE_NONE_MASK;
  cm_ss_e_type           ss = CM_SS_MAIN;

  SYS_ARG_NOT_USED( timer_event );
  SYS_ARG_NOT_USED(cm_call_type_mask);

  /* Get the ss with max allowed capability. */
  if (cmph_is_msim())
  {
    ss = cmph_map_subs_to_ss(lte_asubs_id);
    sd_si_info_ptr_get(cmph_map_cm_ss_type_to_sd_ss_type(ss),&si_info);
  }
  else
  {
    sd_si_info_ptr_get(SD_SS_MAIN,&si_info);
  }



  /*Act pended PLMN blocking & detcah command if
   **1. UE acquired LTE after call end &
   **2. service status is FULL service  &
   **3. There are no CSFb call preferences.
   */
  if((si_info.srv_status == SYS_SRV_STATUS_SRV) &&
     (sds_ptr->is_plmn_blocking_delayed == TRUE) &&
     (si_info.mode == SD_MODE_LTE) &&
     (cmcall_are_there_csfb_call_preferences(ss) == FALSE))
  {
    CM_MSG_HIGH_0("DOM_SEL:Act on Pended PLMN blocking command");
    cmsds_perform_plmn_blocking(ss, CM_PLMN_BLOCK_REAS_NO_SMS);
    sds_ptr->is_plmn_blocking_delayed = FALSE;
  }

  #if defined( FEATURE_LTE_TO_1X )
  /* Check if max allowed time for not having 1XCSFB status is expired */
  if ((sds_ptr->onexcsfb_status_not_avail_uptime != CMSDS_UPTIME_NONE) &&
      (sds_ptr->onexcsfb_status_not_avail_uptime <= uptime))
  {
    ss_ptr->info.changed_fields2 = 0;

    /* Reset lte_cs_capability if CSFB status is received from 1XCP
    **If CSFB STATUS is recieved, it means that the NW is 1XCSFb network
    **and can ignore the 3GPP reject causes
    */
    if (sds_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_BARRED)
    {
      sds_ptr->lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
      CM_MSG_MED_0("Domsel:Reset lte_cs_capability");
    }

    if(cmsds_is_ppcsfb_call_allowed(lte_asubs_id) == FALSE )
    {
      /* Maximum timer to wait for 1XCSFB status is over
      ** and PPCSFB also not available/supported so disable LTE now
      */
      CM_MSG_HIGH_0("Domsel:1XCSFB status not avail and PPCSFB not avail");
      sds_ptr->onexcsfb_status_not_avail_uptime = CMSDS_UPTIME_NONE;

      /* If voice domain config is  "CS_PREF", check for IMS voice and send IMS PREF IND
          ** to IMS to perform IMS registration for Voice */
      if ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED) &&
          (cmsds_is_volte_possible(ss_ptr->info.lte_ims_voice_avail, lte_asubs_id) == TRUE))
      {
                /*Post IMS PREFERRED IND Message to IMS for voice services*/
         cmsds_set_ims_voice_reg_status(SYS_IMS_REG_STATE_NEEDS_REG, SYS_SYS_MODE_LTE);
         sds_ptr->csfb_status = CM_1XCSFB_NOT_AVAIL;
         cmsds_send_ims_pref_changed_ind(SYS_SYS_MODE_LTE,CM_CALL_TYPE_VOICE_MASK,
                                         CM_CALL_TYPE_VOICE_MASK,
                                         lte_asubs_id);
      }
      else
      {
        /*---------------------------------------------------------------------------------- *
               * Process voice not avail ind if                                                                                                       *
               * 1. CS vocie only and 1XCSFB status is not received                                                                      *
               * 2. CS_VOICE_PREF and both CS and ims voice are  not supported.                                                *
               * 3. PS_VOCIE_PREF and IMS vocie is not available and 1XCSFB status is not received on time.          *
               * 4. PS_VOCIE_PREF amd IMS registration failed for voice and 1XCSFB status is not received on time  *
               *-----------------------------------------------------------------------------------*/
        if ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY) ||
             ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED) &&
             (cmsds_is_volte_possible(ss_ptr->info.lte_ims_voice_avail, lte_asubs_id) == FALSE)) ||
             ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED) &&
             ((cmsds_is_volte_possible(ss_ptr->info.lte_ims_voice_avail, lte_asubs_id) == FALSE) ||
              (sds_ptr->ims_reg_status_voice[SYS_SYS_MODE_LTE] == SYS_IMS_REG_STATE_DEREGISTERED))
             )
           )
        {
          /*Still on LTE.So disable it now */
          cmsds_process_voice_not_avail_ind(ss);
        }

        /* Set 1XCSFB status to NOT_AVAIL and proceed with PLMN blocking once PS voice is deemed to be not
              ** available  for voice config PS_VOICE_PREF*/
        #if defined(FEATURE_LTE_TO_1X) && defined (FEATURE_CM_LTE)
        if ((cmsds_config->voice_domain_pref == SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED) &&
             (cmsds_is_ims_active(lte_asubs_id)== TRUE))
        {
          sds_ptr->csfb_status = CM_1XCSFB_NOT_AVAIL;
        }
         #endif

        if ((ss == CM_SS_MAIN) && (ss_ptr->info.changed_fields2 != 0))
        {
          cmss_event(CM_SS_MAIN, CM_SS_EVENT_SRV_CHANGED);
        }

        if ((ss == CM_SS_HYBR_2) && (ss_ptr->info.hybr_gw_changed_fields!= 0))
        {
          cmss_event(CM_SS_HYBR_2, CM_SS_EVENT_SRV_CHANGED);
        }

        if ((ss == CM_SS_HYBR_3) && (ss_ptr->info.gw3_changed_fields!= 0))
        {
          cmss_event(CM_SS_HYBR_3, CM_SS_EVENT_SRV_CHANGED);
        }
      }

    }else
    {
      CM_MSG_HIGH_0("Domsel: 1XCSFB status not avail but PPCSFB avail so reset the timer");
      sds_ptr->onexcsfb_status_not_avail_uptime = CMSDS_UPTIME_NONE;
    }
  }
  #endif /* FEATURE_LTE_TO_1X */

  if(sds_ptr->tssac_hys <= uptime &&
    sds_ptr->tssac_hys > 0 &&
    (cmcall_find_active_voice_call() == CM_CALL_ID_INVALID))
  {
     sd_si_info_ptr_get( lte_ss, &si_info);
     cmsds_process_srv_ind(lte_ss, &si_info);
  }
  cmsds_ims_reg_status_wait_timer_expiry(uptime);
} /* cmsds_timer_proc */

#endif /* FEATURE_DOMAIN_SELECTION */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION cmsds_is_hdr_tunnel_activation_allowed

DESCRIPTION
  Checks if HDR tunnel mode activation is allowed

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

========================================================================== */

static boolean cmsds_is_hdr_tunnel_activation_allowed (void)
{

  cmsds_s_type           *sds_ptr = cmsds_ptr();
  cmph_s_type            *ph_ptr = cmph_ptr();

  /* cm mode pref to sd mode pref */
  sd_ss_mode_pref_e_type sd_mode_pref =  cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->main_stack_info.pref_info.mode_pref);
  /* get common mode pref of ph and hdr */
  sd_mode_pref = sd_misc_get_common_mode_pref(sd_mode_pref,SD_SS_MODE_PREF_HDR);

  CM_MSG_HIGH_2(" is_hdr_tunnel_act_allowed:hdr_tunnel_status =%d common mode pref = %d",
                                                         sds_ptr->hdr_tunnel_status,
                                                         sd_mode_pref);

  /* Allow activation only if tunnel mode is not already active and ph mode pref has HDR and */
  if( sds_ptr->hdr_tunnel_status != CM_HDR_TUNNEL_CMD_ACTD &&
      sd_mode_pref == SD_SS_MODE_PREF_HDR &&
      (cmregprx_is_gwl_protocol_active(SD_SS_MAIN) == TRUE
       || cmregprx_is_gwl_protocol_active(SD_SS_HYBR_2) == TRUE)
    )
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

@FUNCTION cmsds_activate_hdr_tunnel_req

@DESCRIPTION
  Send activate tunnel request to HDR.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_activate_hdr_tunnel_req (void)
{
  hdrcp_msg_req_u      *hdrcp_req;
  cmsds_s_type         *sds_ptr = cmsds_ptr();

  if(cmsds_is_hdr_tunnel_activation_allowed() == TRUE)
  {
    hdrcp_req = (hdrcp_msg_req_u*) cm_mem_malloc(sizeof(hdrcp_msg_req_u));

    hdrcp_req->activate_protocol.act_reason = PROT_ACT_LTE_HANDOVER;

    CM_MSG_HIGH_0("CM->MSGR: send HDR_CP_PROT_ACT_REQ");
    (void) cm_msgr_send( HDR_CP_PROT_ACT_REQ, MSGR_MM_CM,
                         (msgr_hdr_struct_type*)hdrcp_req, sizeof(hdrcp_msg_req_u) );

    sds_ptr->hdr_tunnel_status = CM_HDR_TUNNEL_CMD_ACTD;
    sds_ptr->is_act_hdr_on_srv_cnf = FALSE;
    cm_mem_free(hdrcp_req);
  }

  return;
}

/*===========================================================================

@FUNCTION cmsds_deactivate_hdr_tunnel_req

@DESCRIPTION
  Send deactivate tunnel request to HDR.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_deactivate_hdr_tunnel_req (void)
{
  hdrcp_msg_req_u      *hdrcp_req;
  cmsds_s_type         *sds_ptr = cmsds_ptr();

  if(sds_ptr->hdr_tunnel_status == CM_HDR_TUNNEL_CMD_ACTD)
  {
    CM_MSG_HIGH_1("CM->MSGR: deactivate HDR tunnel, status %d", sds_ptr->hdr_tunnel_status);
    hdrcp_req = (hdrcp_msg_req_u*) cm_mem_malloc(sizeof(hdrcp_msg_req_u));

    hdrcp_req->deactivate_protocol.deact_reason = PROT_DEACT_LTE_HANDOVER;
    hdrcp_req->deactivate_protocol.trans_id = PROT_TRANS_ID_RESERVED;

    (void) cm_msgr_send( HDR_CP_PROT_DEACT_REQ, MSGR_MM_CM,
                         (msgr_hdr_struct_type*)hdrcp_req, sizeof(hdrcp_msg_req_u) );

    sds_ptr->hdr_tunnel_status = CM_HDR_TUNNEL_CMD_DEACTD;

    cm_mem_free(hdrcp_req);
  }

  return;
}
#endif

/*===========================================================================

FUNCTION cmsds_update_ims_voice_support_on_lte

DESCRIPTION
  processes ims_avail status from the srv indication. Notify client IMS voice support status on LTE


DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

========================================================================== */

void cmsds_update_ims_voice_support_on_lte (cm_ss_e_type ss, boolean is_ims_voice_avail)
{
  cmss_s_type    *ss_ptr   = cmss_ptr();

  if(ss_ptr->info.lte_ims_voice_avail != is_ims_voice_avail)
  {
    ss_ptr->info.lte_ims_voice_avail = is_ims_voice_avail;

    if (ss == CM_SS_MAIN)
    {
      ss_ptr->info.changed_fields |= CM_SS_LTE_IMS_VOICE_AVAIL_MASK;
    }
    else if (ss == CM_SS_HYBR_2)
    {
      ss_ptr->info.changed_fields |= CM_SS_GW_LTE_IMS_VOICE_AVAIL_MASK;
    }
    else if (ss == CM_SS_HYBR_3)
    {
      ss_ptr->info.gw3_changed_fields |= CM_SS_GW_LTE_IMS_VOICE_AVAIL_MASK;
    }

    CM_MSG_MED_1("Domsel: ims_avail status changed to %d",is_ims_voice_avail);
  }
  return;
}


/*===========================================================================

FUNCTION cmsds_update_ims_emerg_support_on_lte

DESCRIPTION
  processes ims_emerg status from the srv indication.


DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

========================================================================== */

void cmsds_update_ims_emerg_support_on_lte (sys_ims_lte_emerg_supp_e_type ims_emerg_avail)
{
  cmsds_s_type    *sds_ptr   = cmsds_ptr();
  cmss_s_type    *ss_ptr   = cmss_ptr();

  if(sds_ptr->lte_ims_emerg_avail != ims_emerg_avail)
  {
    /* update SDS info with current value of emerg avail status */
    sds_ptr->lte_ims_emerg_avail = ims_emerg_avail;
    CM_MSG_HIGH_1("Domsel: ims_emerg status changed to %d",ims_emerg_avail);
  }
  if(INRANGE(ss_ptr->info.lte_ims_emerg_avail,SYS_IMS_LTE_EMERG_SUPP_NOT_AVAILABLE , SYS_IMS_LTE_EMERG_SUPP_UNKNOWN)
	&& INRANGE(ims_emerg_avail ,SYS_IMS_LTE_EMERG_SUPP_NOT_AVAILABLE , SYS_IMS_LTE_EMERG_SUPP_UNKNOWN) 
	&& ss_ptr->info.lte_ims_emerg_avail != ims_emerg_avail)
	{
	  /* update changed fields 2 with emerg avail status mask*/
      ss_ptr->info.changed_fields2 |= CM_SS_EVT_LTE_IMS_EMERG_AVAIL_MASK;

	  /* update SS with current value of emerg avail status */
	  ss_ptr->info.lte_ims_emerg_avail = ims_emerg_avail;
	}
  return;
}

/*===========================================================================

FUNCTION cmsds_update_selected_voice_domain

DESCRIPTION
  Updates Selected voice domain in LTE.

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

========================================================================== */

void cmsds_update_selected_voice_domain (

    cm_ss_e_type              ss,

    sys_cm_domain_sel_domain_e_type voice_domain
)
{
  cmss_s_type    *ss_ptr   = cmss_ptr();

  if(ss_ptr->info.voice_domain != voice_domain)
  {
    ss_ptr->info.voice_domain = voice_domain;

    if (ss == CM_SS_MAIN)
    {
      ss_ptr->info.changed_fields2 |= CM_SS_LTE_VOICE_DOMAIN_CHANGED_MASK;
    }
    else if (ss == CM_SS_HYBR_2)
    {
      ss_ptr->info.hybr_gw_changed_fields |= CM_SS_GW_LTE_VOICE_DOMAIN_CHANGED_MASK;
    }
    else if (ss == CM_SS_HYBR_3)
    {
      ss_ptr->info.gw3_changed_fields |= CM_SS_GW_LTE_VOICE_DOMAIN_CHANGED_MASK;
    }
  }

  return;
}

/*===========================================================================

FUNCTION cmsds_update_selected_sms_domain

DESCRIPTION
  Updates Selected voice domain in LTE.

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

========================================================================== */

void cmsds_update_selected_sms_domain (

cm_ss_e_type              ss,

sys_cm_domain_sel_domain_e_type sms_domain

)
{
  cmss_s_type    *ss_ptr   = cmss_ptr();

  if(ss_ptr->info.sms_domain != sms_domain)
  {
    ss_ptr->info.sms_domain = sms_domain;

    if (ss == CM_SS_MAIN || ss == CM_SS_HYBR_1)
    {
      ss_ptr->info.changed_fields2 |= CM_SS_LTE_SMS_DOMAIN_CHANGED_MASK;
    }
    else if (ss == CM_SS_HYBR_2)
    {
      ss_ptr->info.hybr_gw_changed_fields |= CM_SS_GW_LTE_SMS_DOMAIN_CHANGED_MASK;
    }
    else if (ss == CM_SS_HYBR_3)
    {
      ss_ptr->info.gw3_changed_fields |= CM_SS_GW_LTE_SMS_DOMAIN_CHANGED_MASK;
    }
  }

  return;
}


/*===========================================================================

FUNCTION cmsds_is_gwt_scan_allowed

DESCRIPTION
  This function returns whether GWT scan allowed during RLF based 
  on VOLTE call and NV enabled

DEPENDENCIES
  none

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  none

===========================================================================*/
boolean cmsds_is_gwt_scan_allowed(sd_ss_e_type ss)
{
  sys_modem_as_id_e_type asubs_id;
  boolean gwt_scan_allowed = TRUE;
  cmph_sub_config_s_type *ph_sub_config_ptr;
  cm_call_id_type volte_call_id = CM_CALL_ID_INVALID;

  asubs_id = cmph_map_cm_ss_to_subs(cmph_map_sd_ss_type_to_cm_ss_type(ss));
  ph_sub_config_ptr = cmph_get_sub_config_ptr(asubs_id);

  if(ph_sub_config_ptr == NULL)
  {
    CM_MSG_HIGH_1("cmph sub config pointer[%d] is NULL",asubs_id);
    return FALSE;
  }

  if(ph_sub_config_ptr->skip_gw_scan_during_rlf)
  {
    if(cmsds_ptr()->ext_volte_call_active && (cmsds_ptr()->ext_volte_call_asubs_id == asubs_id))
    {
      gwt_scan_allowed = FALSE;
    }
    else
    {
      volte_call_id = cmcall_is_there_volte_call_obj(CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_VS_MASK | CM_CALL_TYPE_VT_MASK);
    
      if(volte_call_id != CM_CALL_ID_INVALID)
      {
        cmcall_s_type *call_ptr = cmcall_ptr(volte_call_id);
    
        if(call_ptr && (call_ptr->asubs_id == asubs_id))
        {
          gwt_scan_allowed = FALSE;
        }
      }
    
    }
  }

  CM_MSG_HIGH_5("gwt_scan_allowed: %d, skip_gw: %d, volte_call_id %d, asubs_id %d, ext_volte_call %d",gwt_scan_allowed, 
  ph_sub_config_ptr->skip_gw_scan_during_rlf,volte_call_id,asubs_id,cmsds_ptr()->ext_volte_call_active);

  return gwt_scan_allowed;
}

/*===========================================================================

FUNCTION cmsds_get_config

DESCRIPTION
  Helper function to get a pointer to sds_ptr->config[] for a given sub_id.
  Checks that sub_id is in range of sds_ptr->config[] array.

DEPENDENCIES
  none

RETURN VALUE
  cmsds_sub_config_s_type* config

SIDE EFFECTS
  none

===========================================================================*/
cmsds_sub_config_s_type* cmsds_get_config_ptr(
  sys_modem_as_id_e_type sub_id
)
{
  cmsds_s_type   *sds_ptr = cmsds_ptr();

  if(!(sub_id >= SYS_MODEM_AS_ID_1 && 
       sub_id <  ARR_SIZE(sds_ptr->config) &&
       sub_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    CM_MSG_HIGH_1("cmsds_get_config: invalid sub_id %d", sub_id);
    sub_id = SYS_MODEM_AS_ID_1;
  }

  return &(sds_ptr->config[sub_id]);
}

/*===========================================================================

@FUNCTION cmsds_get_csfb_type_allowed

@DESCRIPTION
  Checks if 1XCP can be activated in CSFB mode

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/


cm_call_csfb_e_type cmsds_get_csfb_type_allowed(
  sys_modem_as_id_e_type  asubs_id
)
{

  #if defined (FEATURE_3GPP_CSFB) && !defined (FEATURE_LTE_TO_1X)

  /* if sub is not lte capable return none */
  if (!cmph_is_lte_capable_on_sub(asubs_id))
  {
    return CM_CALL_CSFB_TYPE_NONE;
  }
  
  if (cmsds_is_ppcsfb_call_allowed (asubs_id))
  {
    return CM_CALL_CSFB_TYPE_PPCSFB;
  }
  else
  {
    return CM_CALL_CSFB_TYPE_NONE;
  }
  #elif  !defined (FEATURE_3GPP_CSFB) && defined (FEATURE_LTE_TO_1X)
  if (cmsds_is_1xcsfb_call_allowed (asubs_id))
  {
    return CM_CALL_CSFB_TYPE_1XCSFB;
  } else
  {
    return CM_CALL_CSFB_TYPE_NONE;
  }
  #elif defined (FEATURE_3GPP_CSFB) && defined (FEATURE_LTE_TO_1X)
  if (cmsds_is_ppcsfb_call_allowed (asubs_id))
  {
    return CM_CALL_CSFB_TYPE_PPCSFB;
  }
  else if (cmsds_is_1xcsfb_call_allowed (asubs_id))
  {
    return CM_CALL_CSFB_TYPE_1XCSFB;
  } else
  {
    return CM_CALL_CSFB_TYPE_NONE;
  }
  #elif !defined (FEATURE_3GPP_CSFB) && !defined (FEATURE_LTE_TO_1X)
  /* No CSFB support */
  return CM_CALL_CSFB_TYPE_NONE;
  #endif

}

/*===========================================================================

@FUNCTION cmsds_is_current_sys_lte_ims_emergency_capable

@DESCRIPTION

Returns whether LTE system acquired supports IMS emergency

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

boolean cmsds_is_current_sys_lte_ims_emergency_capable(
  sys_modem_as_id_e_type  as_id
)
{
  cmsds_s_type                         *sds_ptr = cmsds_ptr();
  /* Pointer to domain selection object */
  cmss_s_type                          *ss_ptr  = cmss_ptr();

  cm_ss_e_type   ss        = CM_SS_MAIN;

  if (!cmph_is_lte_capable_on_sub(as_id))
  {
    return FALSE;
  }

  /* Check LTE release version supported by the device*/
  if (cmsds_get_config_ptr(as_id)->is_lte_release9_supp == FALSE)
  {
    CM_MSG_HIGH_0("LTE rel ver ");
    return FALSE;
  }

  /* Check if this is not a ext IMS E911 and there is no IP APP registered with EMERG cap
  */
  if (cmemg_get_reason_per_sub(as_id) != CM_AC_REASON_THIRD_PARTY_IMS_E911 &&
  	 !cmcall_is_active_ip_app(SYS_SYS_MODE_LTE, CM_CALL_TYPE_EMERGENCY))
  {
    return FALSE;
  }

  /* check if N/W supports IMS emerg per SIB1/attach accept
  */
  if (sds_ptr->lte_ims_emerg_avail != SYS_IMS_LTE_EMERG_SUPP_AVAILABLE)
  {
    return FALSE;
  }

  /* For multi-sim targets, update the ss. */
  ss = cmph_determine_ss_per_rat(as_id, SYS_SYS_MODE_LTE);
  CM_MSG_MED_2("LTE_SS, ss=%d, lte_asubs_id=%d",ss, as_id);

  /* check if SIB2 indicates system is emerg barred
  */
  if (ss == CM_SS_MAIN &&
      ss_ptr->info.cell_info.emerg_access_status != SYS_LTE_EMERG_ACCESS_ALLOWED)
  {
    return FALSE;
  }
  else if (ss == CM_SS_HYBR_2 &&
           ss_ptr->info.gw_cell_info.emerg_access_status != SYS_LTE_EMERG_ACCESS_ALLOWED)
  {
    return FALSE;
  }
  else if (ss == CM_SS_HYBR_3 &&
           ss_ptr->info.gw3_cell_info.emerg_access_status != SYS_LTE_EMERG_ACCESS_ALLOWED)
  {
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

@FUNCTION cmsds_is_LTE_emerg_call_possible

@DESCRIPTION
Returns whether LTE emerg call is possible

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/

boolean cmsds_is_LTE_emerg_call_possible(
  sys_modem_as_id_e_type  as_id
)
{
  cmsds_s_type                         *sds_ptr = cmsds_ptr();
  /* Pointer to domain selection object */

  boolean                              is_lte_emerg_supported = FALSE;

  if (cmemg_get_reason_per_sub(as_id) == CM_AC_REASON_THIRD_PARTY_IMS_E911)
  {
    CM_MSG_HIGH_0("ext LTE  E911 call");

    is_lte_emerg_supported =  TRUE;
  }
  else
  {
    /* If IMS is registed for 911 services over LTE and if current LTE release
    ** version is 9 or greater, allow LTE to be used for emergency call purposes
    */
    if( cmsds_get_config_ptr(as_id)->is_lte_release9_supp &&
      cmcall_is_active_ip_app(SYS_SYS_MODE_LTE,
      CM_CALL_TYPE_EMERGENCY))
    {
      CM_MSG_HIGH_0("LTE_911 over IMS possible");
      is_lte_emerg_supported =  TRUE;
    }
    else if(cmsds_get_csfb_type_allowed(as_id) != CM_CALL_CSFB_TYPE_NONE )
    {
      CM_MSG_HIGH_2("LTE_911 supported ims_emerg%d, curr_voice_dom %d",
        sds_ptr->lte_ims_emerg_avail,sds_ptr->curr_voice_dom_selected);
      is_lte_emerg_supported =  TRUE;
    }
  }
  return is_lte_emerg_supported;
}

/*===========================================================================

@FUNCTION cmsds_emergency_call_dom_selection

@DESCRIPTION
 returns domain for emergency call based on current voice domain and whether
 current domain supports emergency.

 if current voice domain does not support emergency it chooses an alternative domain
 if available.
 Eg:
 1. if curr domain is IMS but it does not support emergency, choose CSFB if it is possible
 2. If curr domain is not chosen, but CSFB is possible, choose CSFB as emerg domain
 3. if curr domain is not chosen, but there is only LTE IMS cap service available, choose IMS as emerg domain


@DEPENDENCIES

None

@RETURNS
  1. ss on which emerg call can be originated
  2. Domain on which emerg call can be originated
  3. csfb_type


@SIDE_EFFECT

None

===========================================================================*/

cm_ss_e_type cmsds_emergency_call_dom_selection
(

  cm_call_csfb_e_type                 *csfb_type_ptr,

  sys_cm_domain_sel_domain_e_type     *dom_sel_ptr,
  
  sys_modem_as_id_e_type               asubs_id
)
{
  cmsds_s_type            *sds_ptr              = cmsds_ptr();
  boolean                 is_lte_emerg_possible = FALSE;
  boolean                 is_lte_sys_emerg_cap  = FALSE;
  cm_ss_e_type            lte_ss                =  CM_SS_MAIN;
  sd_si_info_s_type       *si_info_ptr          = NULL;
  cm_ss_e_type            emerg_ss              =  CM_SS_MAIN;
  cmph_sub_config_s_type *ph_sub_config_ptr;

  /*---------------------------------------------------------------------------------------*/

  emerg_ss = cmph_map_as_id_to_cm_ss(asubs_id);
  *csfb_type_ptr = CM_CALL_CSFB_TYPE_NONE;
  *dom_sel_ptr   = SYS_CM_DOMAIN_SEL_DOMAIN_NONE;
  ph_sub_config_ptr = cmph_get_sub_config_ptr(asubs_id);
  
  /*---------------------------------------------------------------------------------------*/
  if(ph_sub_config_ptr == NULL)
  {
    CM_MSG_HIGH_1("cmph sub config pointer[%d] is NULL",asubs_id);
    return emerg_ss;
  }

  /* if the sub is not lte capable, return now */
  if(!cmph_is_lte_capable_on_sub(asubs_id))
  {
    return emerg_ss;
  }

  /*---------------------------------------------------------------------------------------*/

  /* Determine the lte_ss. */
  lte_ss = cmph_determine_ss_per_rat(asubs_id, SYS_SYS_MODE_LTE);
  CM_MSG_HIGH_3("LTE_911: lte_ss=%d, emerg_ss=%d, asubs_id=%d",
                lte_ss, emerg_ss, asubs_id);
  
  /*---------------------------------------------------------------------------------------*/
  /* check if emerg is possible over LTE - either IMS cap or CSFB supp */
  is_lte_emerg_possible = cmsds_is_LTE_emerg_call_possible(asubs_id);

  if (!is_lte_emerg_possible)
  {
    return emerg_ss;
  }
  /*---------------------------------------------------------------------------------------*/
  /* If we got here, LTE e911 could be possible */
  /* check current domain to see if 911 CSFB is possible*/

  if ((sds_ptr->curr_voice_dom_selected != SYS_CM_DOMAIN_SEL_DOMAIN_IMS) &&
     (sds_ptr->curr_voice_dom_selected != SYS_CM_DOMAIN_SEL_DOMAIN_NONE) &&
     (sds_ptr->curr_voice_dom_selected != SYS_CM_DOMAIN_SEL_DOMAIN_NO_VOICE) )
  {
    *csfb_type_ptr = cmsds_get_csfb_type_allowed(asubs_id);
  }
  /* Looks for CSFB in following cases.
  ** 1. Domain is IMS but acquired LTE doesn't support VOLTE E911.
  ** 2. KDDI device is in romaing. Use CSFB irrespective of VOLTE E911 status.
  */
  is_lte_sys_emerg_cap = cmsds_is_current_sys_lte_ims_emergency_capable(asubs_id);
  if ((sds_ptr->curr_voice_dom_selected == SYS_CM_DOMAIN_SEL_DOMAIN_IMS) &&
      ((!is_lte_sys_emerg_cap) ||
       (cmpmprx_is_special_volte_e911_calls() &&
      cmmsc_get_stack_common_call_mode_per_subs(asubs_id) == SYS_CALL_MODE_NORMAL &&
	  cmss_intl_srv_info_ptr(lte_ss)->mode == SD_MODE_LTE &&
        sys_srv_status_is_srv(cmss_intl_srv_info_ptr(lte_ss)->srv_status))))
   {
    CM_MSG_HIGH_0("LTE_911:IMS without e911 support");
    *csfb_type_ptr = cmsds_get_csfb_type_allowed(asubs_id);
   }

  #if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE)
   /* wwan full srv is supported and LTE sys supports E911 then call shall be attempted over  IP */
  if( ph_sub_config_ptr->wifi_e911_wwan_full_srv == TRUE && 
      is_lte_sys_emerg_cap == TRUE &&
      cmss_intl_srv_info_ptr(lte_ss)->mode == SD_MODE_LTE &&
      sys_srv_status_is_srv(cmss_intl_srv_info_ptr(lte_ss)->srv_status)
    )
  {
    *csfb_type_ptr = CM_CALL_CSFB_TYPE_NONE;
  }
  #endif

  if ( *csfb_type_ptr != CM_CALL_CSFB_TYPE_NONE)
  {
    if (*csfb_type_ptr == CM_CALL_CSFB_TYPE_PPCSFB)
    {
      *dom_sel_ptr = SYS_CM_DOMAIN_SEL_DOMAIN_3GPP;
    }
    else
    {
      *dom_sel_ptr = SYS_CM_DOMAIN_SEL_DOMAIN_1X;
    }
    CM_MSG_HIGH_2("LTE_911:csfb preferred %d %d",*csfb_type_ptr,*dom_sel_ptr);
    return emerg_ss;
  }
  /*---------------------------------------------------------------------------------------*/
  /* If we got here, CSFB is not possible. Check if IMS is the preferred domain
  */
  si_info_ptr = cmss_intl_srv_info_ptr(lte_ss);

  /* if there is no LTE service, or LTE service does not support emergency
  ** return and indicate IMS is not the emerg domain */
  if ( is_lte_sys_emerg_cap == FALSE ||
       !( sys_srv_status_is_srv(si_info_ptr->srv_status)&&
          si_info_ptr->mode   ==  SYS_SYS_MODE_LTE )
     )
  {
    CM_MSG_HIGH_2("LTE_911:no curr ims supp %d %d",is_lte_sys_emerg_cap,si_info_ptr->srv_status);
    return emerg_ss;
  }
  
  /*---------------------------------------------------------------------------------------*/
  /* if a) voice domain is IMS or
        b) there is no service on MAIN,
      choose IMS as emerg domain
  */
  
  si_info_ptr = cmss_intl_srv_info_ptr(emerg_ss);
  if (sds_ptr->curr_voice_dom_selected == SYS_CM_DOMAIN_SEL_DOMAIN_IMS ||
      !sys_srv_status_is_srv(si_info_ptr->srv_status))
  {
    emerg_ss      = lte_ss;
    *dom_sel_ptr  = SYS_CM_DOMAIN_SEL_DOMAIN_IMS;
  }
  return emerg_ss;
}

/*===========================================================================

@FUNCTION cmsds_emergency_call_dom_selection

@DESCRIPTION
      IF PLMN-TAI in TAU complete is same as CM has received last from NAS then it will send TAU complete indication to IMS.
      If PLMN-TAI in TAU complete is different from what CM has received last from NAS then CM will buffer TAU complete. 
      CM will send TAU complete after receiving new service indication from NAS

@DEPENDENCIES

None

@RETURNS

@SIDE_EFFECT

None

===========================================================================*/
void cmsds_process_tau_complete_ind(emm_tau_complete_ind_type *tau_complete_ind)
{
  cmss_s_type  *ss_ptr = cmss_ptr();
  cmsds_s_type   *sds_ptr = cmsds_ptr();
  uint8 inst_id = tau_complete_ind->msg_hdr.inst_id;
  sys_modem_as_id_e_type as_id = INST_ID_TO_SYS_AS_ID(inst_id);
  sd_si_info_s_type  *si_info_ptr = NULL;
  cm_ss_e_type ss = cmph_determine_ss_per_rat (as_id, SYS_SYS_MODE_LTE);
  si_info_ptr = cmss_intl_srv_info_ptr(ss);

  if(sys_plmn_match(tau_complete_ind->plmn, si_info_ptr->sid.plmn.plmn_id) &&
    (tau_complete_ind->tac == ss_ptr->info.tac))
  {
    cmsds_send_tau_complete(tau_complete_ind->msg_hdr.inst_id);
    sds_ptr->is_tau_needed = FALSE;
  }
  else
  {
    sds_ptr->is_tau_needed = TRUE;
  }
  CM_MSG_HIGH_3("ss_info.tac %d tau_complete_ind tac %d is_tau_needed %d",\
       ss_ptr->info.tac, tau_complete_ind->tac, sds_ptr->is_tau_needed);
  return;
}
