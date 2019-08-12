
/*===========================================================================
                        S M    U T I L S

DESCRIPTION

   This file contains the Utility functions for SM task. 

EXTERNALIZED FUNCTIONS   
              
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/smutils.c_v   1.5   19 Mar 2002 17:57:32   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/smutils.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/03/12   am      Fixing modem statistics crash in SM
10/01/12   am      Back off timer code
09/27/12  am       Handling of unexpected BCM from Network
04/10/12   us      SM sould always clear up PDP & send either an indication or confirmation to CM on receiving SMREG_PDP_ABORT, irrespective of the PDP state.
02/28/12   us      Removing third party ID references from F3 messages. 
02/08/12   us      Extended Uplink Data rate support added  
11/16/11   am      Fixing the compilation Warning. 
10/10/11   HC      Removed AEEstd.h inclusion as aeestdlib is obsoleted 
                   Replaced the std_strnicmp with C equivalent function strncasecmp
                   Included <stringl/stringl.h>  
09/14/11   rajesh  Made changes to set the ISR status correctly in ESM/SM.
08/04/11   am      NAS Support for HFN Mismatch for QCHAT 
04/10/08  ab      For REL6 Map the Traffic Class 'Subscribed Traffic Class' to the RRC 
                          establishment cause 'Originating Subscribed traffic call' 
01/21/08  ab       removing compiler warning
11/06/07    rk      removed Establish Pending Timer code under gcf_flag
10/16/07   ab       using std_strnicmp in sm_compare_apn so as to take apn length into account 
07/03/07    rk      include AEEstd.h header file to support std_stricmp
08/18/06   rk       Added Extended TI Feature Code
07/17/06   ss       Delaying Local deactivation for 2 secs to avoid RAB setup failure.
06/08/06  rk       Added Release 5 specific logs for QoS
05/02/06  kvk      REL5 QoS validation will be done based on NV item settings rather than the 
                   GCF flag
01/17/06  ss       Locally deactivating Secondary PDP when primary is deactvating and secondary has not yet come up.
12/26/05  ss       Now sending GMMSM_CLOSE Session request in case of local deactivation 
                   SM STATUS message due to cause INVALID TRANS ID.
11/22/05  kvk/hj  GCF 12.9.9: TIMER_ESTABLISH_PENDING timer is extended to 8s for GCF
11/09/05  kvk      Added function sm_force_gmmsm_close_session_req() for RRC
                   to call when RRC stuck in DCH for a long time without any PS Activity
10/13/05  kvk     Now REL5 QoS will be processed only in UMTS mode and GCF flag is enabled
09/29/05  kvk     Now clearing the timer_est_pen_flag when PDP contexts were locally 
                  deactivated to avoid the subsequent call fails when the falg is set and 
                  PDP is locally terminated.
09/28/05  kvk     Removed GCF flag dependency for REL5 QoS
09/21/05  kvk     Now accepting the reserved values in PDP Accept message for
                  Traffic class, Delivery order and Delivery of erroneous sdu  
                  QoS fields in R97 networks.
09/20/05  kvk     Added funcion  sm_get_most_demanding_traffic_class() to get
                   the most demanding traffic class for all PDPs.
 09/13/05  kvk     MO PDP Modify supported. Now handling the Application initiated
                   PDP Modification for QoS and TFT.
09/12/05  kvk      Changed FEATURE_REL5_QOS flag to FEATURE_REL5.
06/20/05   hj      Added support for new events EVENT_SMGMM_REQUEST_SENT, 
                   EVENT_SMGMM_REJECT_RECEIVED.
06/09/05  kvk      Fixed delay class validation.
06/07/05  kvk      Corrected Debug print
05/18/05  kvk      Fixed compilation error and lint errors/warnings
05/11/05  ss        Now checking for Release 99 and Release 5 QoS parameters
                         while validating QoS.
02/28/05  kvk      Now deactivating the Secondary PDP before we actually bring down
                   the Primary PDP contexts during local deactivation.
01/26/04  kvk      Changed the check in sm_perform_loacl_deactivation() to deactivate 
                   PDPs which are not INACTIVE.
12/15/04  kvk      New function sm_check_pdp_collision() added to check the MO and 
                   MT PDP collision
10/18/04  kvk      MT PDP support added. 
10/07/04  kvk      sm_check_pri_duplicate_pdp() modified to allow one static, one dynamic
                   context per PDP type and APN.
09/02/04  kvk      QoS validation functions changed for correct QoS validation.
                   Teardown ind flag is processed for local PDP deactivation.                   
06/08/04  kvk      PDP con state is reset along with PDP state.
06/07/04  kvk      Added sm_perform_local_pdp_deactivation() function to locally 
                   deactivate the specified PDP.
01/28/04   ks      Added sm_log_pdp_event for logging PDP/RABM state info.
11/24/03   ks      Added support for TIMER_ESTABLISH_PENDING for handling the
                   GMM connection before starting the PDP activation request.
08/05/03   mks     Added support for validating the network offered QoS against
                   the minimum QoS requirements of the MS during MO PDP activation
                   and MT PDP modification procedures. If the QoS validation fails,
                   the context deactivation is initiated.
02/24/03   ks      Fixed LINT warnings.
03/15/02   ks      setting the num_ps_connections to 0 if call_info_pkt is not
                   NULL in the sm_log_pdp_info() function.
02/19/02   ks      Changed log packet names from WCDMA_xxxx_PACKET to
                   LOG_UMTS_xxxx_PACKET_C type.
02/19/02   ks      Changed log codes prefix from WCDMA_ to LOG_UMTS_. 
11/27/01   ks      Initializing the num_ps_connections to 0 before logging
                   the Log packet to the QXDM.
11/05/01   ks      Fixed the Log packet contents in the sm_log_qos_info().
08/15/01   ks      Added functions for setting/clear SM timers and also the
                   QXDM log functions.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "mmcp_variation.h"
#include <stringl/stringl.h>
#include "sm_v.h"
#include "naslog_v.h"
#include "rrcmmif.h"
#include "mm_v.h"
#include "sm_cm_msgr_enum_msg.h"

#ifdef FEATURE_GSM_GPRS
#include "gsmsnif.h"
#endif

#include "smtimers.h"
#if defined(FEATURE_LTE)
#include "sm_multimode_common.h"
#endif
#include <stringl/stringl.h>
#include "sys_cnst_v.h"
#include "ds3gpp_api.h"


extern boolean rabm_check_rab_reest_pend(void);
extern boolean rabm_check_serv_req_complete_pending(void);
extern boolean is_ps_gsm_mode( sys_modem_as_id_e_type as_id );
extern boolean is_ps_umts_mode( sys_modem_as_id_e_type as_id );

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern ue_rev_level_indicator_type get_revision_level_indicator_per_nas_stacks
(
  sys_modem_as_id_e_type as_id 
);
#else
extern ue_rev_level_indicator_type get_revision_level_indicator( void );
#endif

#ifdef FEATURE_DUAL_SIM
#define is_umts_mode() is_ps_umts_mode((sys_modem_as_id_e_type)sm_as_id)
#define is_gsm_mode()  is_ps_gsm_mode((sys_modem_as_id_e_type)sm_as_id)
#else
#define is_umts_mode() is_ps_umts_mode(0)
#define is_gsm_mode()  is_ps_gsm_mode(0)
#endif

#if defined(FEATURE_DUAL_DATA)
extern boolean sm_ps_vote_for_reselection_sim[MAX_AS_IDS];
#define sm_ps_vote_for_reselection sm_ps_vote_for_reselection_sim[sm_as_id]

extern sm_blocked_apn_list_T sm_blocked_apn_list_sim[MAX_AS_IDS];
#define sm_blocked_apn_list sm_blocked_apn_list_sim[sm_as_id]

#else
extern sm_blocked_apn_list_T sm_blocked_apn_list;
#endif 



/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

#ifdef FEATURE_DUAL_DATA
byte qos_ie_length_sim[MAX_AS_IDS];
#else
/*Indicates QoS length used identifying post release 97/98 N/Ws*/
byte qos_ie_length;
#endif

extern sm_esm_debug_buffer_s_type sm_esm_debug_buffer[SM_DEBUG_MESSAGE_BUFFER_SIZE];
extern dword sm_esm_debug_buffer_index;

#ifdef FEATURE_GSM_GPRS
/* -----------------------------------------------------------------------
** FUNCTION     : SM_SEND_SNDCP_CMD

** DESCRIPTION  : This function sends command to SNDCP
** ----------------------------------------------------------------------- */
extern void sm_send_sndcp_cmd( pdp_context_info *cur_pdp, gsnsm_prim_type sndcp_cmd);
#endif

extern rex_tcb_type* sm_tcb_ptr;


/*===========================================================================
** FUNCTION DEFINITIONS 
** =========================================================================*/


/*===========================================================================

FUNCTION sm_check_if_no_conn_exists

DESCRIPTION
  This function is called by the SM in order to check whether PDP list is assign with connection id
DEPENDENCIES
  
   
RETURN VALUE

  Returns TRUE if a pdp_list is null , otherwise returns FALSE.

SIDE EFFECTS
  None.
  
===========================================================================*/

boolean sm_check_if_no_conn_exists(byte  connection_id)
{


  if (pdp_list[connection_id] == NULL)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
  
}


/*===========================================================================
FUNCTION SM_CLEAR_PDP_TIMER

DESCRIPTION
  This function clears a specified timer and the associated Signal.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void sm_clear_pdp_timer (sm_timer_T sm_timer_type)
{

  switch (sm_timer_type)
  {
    case TIMER_ESTABLISH_PENDING:
      MSG_MED_DS_0(SM_SUB, "Clearing Timer : TIMER_ESTABLISH_PENDING");
      (void) rex_clr_timer( &gmmsm_establish_timer );
      break;
      
    default:
      MSG_ERROR_DS_1(SM_SUB, "Wrong timer to clear : %d ",sm_timer_type);
      break;
  }

  // Clear the signal associated with this timer.
  (void) rex_clr_sigs( sm_tcb_ptr, (uint32)sm_timer_type );
}

/*===========================================================================
FUNCTION SM_SET_PDP_TIMER

DESCRIPTION
  This function sets a specified PDP timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void sm_set_pdp_timer (sm_timer_T sm_timer_type)
{
  switch (sm_timer_type)
  {
    case TIMER_ESTABLISH_PENDING:
      MSG_MED_DS_0(SM_SUB, "Start Timer : TIMER_ESTABLISH_PENDING");
      (void) rex_set_timer (&gmmsm_establish_timer, ESTABLISH_PENDING_TIME); 
      break;

    default:
      MSG_ERROR_DS_1(SM_SUB, "Wrong timer to set : %d ",sm_timer_type);
      break;
  }
}

/*===========================================================================
FUNCTION SM_LOG_PDP_EVENT

DESCRIPTION
  This function sends the PDP Event packet to DIAG for QXDM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean sm_log_pdp_event(void)
{
  byte pdp_event_pkt[4];
  word i;

  // Pack the Event packet data and commit to the DIAG.
  for (i=0; i<MAX_PDP_NUM; i++ )
  {
    if (pdp_list[i] != NULL)
    {
        pdp_event_pkt[0] = (byte) i;       /* PDP Index */
        pdp_event_pkt[1] = pdp_list[i]->trans_id;        /* Transaction ID */
        pdp_event_pkt[2] = (byte)(((int)pdp_list[i]->rab_state << 4) | (int)pdp_list[i]->pdp_state); /* RABM state | PDP State */
        pdp_event_pkt[3] = pdp_list[i]->current_cause;   /* Identifies current cause */

        event_report_payload(EVENT_SM_PDP_STATE, 4, (void *)pdp_event_pkt);
    }
  }

  return TRUE;
}


/*===========================================================================
FUNCTION SM_LOG_PDP_REQUEST_REJECT_EVENT

DESCRIPTION
  This function logs the PDP Activate Request or Reject and PDP Deactivate 
  Request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void sm_log_pdp_request_reject_event(boolean request, byte message_type,
                                     byte cause)
{

  byte event_payload[2];

  event_payload[0] = message_type;
  event_payload[1] = cause;
  if (request == TRUE)
  {
    event_report_payload(EVENT_SMGMM_REQUEST_SENT,2,(void*)event_payload);
  }
  else
  {
    event_report_payload(EVENT_SMGMM_REJECT_RECEIVED,2,(void*) event_payload);
  }
}
/*===========================================================================
FUNCTION SM_LOG_PDP_INFO

DESCRIPTION
  This function sends the PS info log packet to DIAG for QXDM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean sm_log_pdp_info()
{
  word pkt_size, i;
  LOG_UMTS_PS_CALL_INFO_LOG_PACKET_C_type * call_info_pkt;
#ifdef FEATURE_DUAL_SIM
  LOG_UMTS_DS_PS_CALL_INFO_LOG_PACKET_C_type *ds_log_ptr;
#endif

  // Log PDP Event packet data and sent to DIAG.
  (void)sm_log_pdp_event();

  call_info_pkt = NULL;
  pkt_size = i = 0;

  MSG_LOW_DS_0(SM_SUB,"Logging LOG_UMTS_PS_CALL_INFO_LOG_PACKET_C");

#ifdef FEATURE_DUAL_SIM
  if ( mm_dual_standby_mode )
  {
   /* Calculate log packet to be sent */
   pkt_size = sizeof(LOG_UMTS_DS_PS_CALL_INFO_LOG_PACKET_C_type);

   /* Allocate log packet */
   ds_log_ptr = (LOG_UMTS_DS_PS_CALL_INFO_LOG_PACKET_C_type *)
              log_alloc(LOG_UMTS_DS_PS_CALL_INFO_LOG_PACKET_C, pkt_size);

    if (ds_log_ptr == NULL)
    {
      MSG_LOW_DS_0(SM_SUB,"Unable to get a LOG packet pointer");
      return FALSE;
    }
    else
    {
      ds_log_ptr->num_ps_connections = 0;
    }

    ds_log_ptr->as_id = (uint8) sm_as_id;
    // Pack the Log packet data and commit to the DIAG.
    for (i=0; i<MAX_PDP_NUM; i++ )
    {
      if (pdp_list[i]!= NULL && pdp_list[i]->pdp_state != PDP_INACTIVE)
      {
        ds_log_ptr->num_ps_connections++;
        ds_log_ptr->ps_connection_info[i].connection_id = 
                                              pdp_list[i]->trans_id;
        ds_log_ptr->ps_connection_info[i].nsapi = 
                                              pdp_list[i]->nsapi.nsapi; 
      }
    }
    log_commit(ds_log_ptr);
  }
  else
#endif
  {
  pkt_size = sizeof(LOG_UMTS_PS_CALL_INFO_LOG_PACKET_C_type);
  call_info_pkt = (LOG_UMTS_PS_CALL_INFO_LOG_PACKET_C_type *)
                    log_alloc(LOG_UMTS_PS_CALL_INFO_LOG_PACKET_C, pkt_size);

  if (call_info_pkt == NULL)
  {
    MSG_LOW_DS_0(SM_SUB,"Unable to get a LOG packet pointer");
    return FALSE;
  }
  else
  {
    call_info_pkt->num_ps_connections = 0;
  }
    
  // Pack the Log packet data and commit to the DIAG.
  for (i=0; i<MAX_PDP_NUM; i++ )
  {
    if (pdp_list[i]!= NULL && pdp_list[i]->pdp_state != PDP_INACTIVE)
    {
      call_info_pkt->num_ps_connections++;
      call_info_pkt->ps_connection_info[i].connection_id = 
                                            pdp_list[i]->trans_id;
      call_info_pkt->ps_connection_info[i].nsapi = 
                                            pdp_list[i]->nsapi.nsapi; 
    }
  }
  log_commit(call_info_pkt);
  }
  return TRUE;
}


/*===========================================================================
FUNCTION SM_LOG_QOS_INFO

DESCRIPTION
  This function sends the QOS log packet to DIAG for QXDM and QCAT processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean sm_log_qos_info(pdp_context_info *pdp)
{
  word pkt_size;

  LOG_UMTS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C_type * qos_log_pkt;
#ifdef FEATURE_DUAL_SIM
  LOG_UMTS_DS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C_type *ds_log_ptr;
#endif

  MSG_LOW_DS_0(SM_SUB,"Logging LOG_UMTS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C ");

  qos_log_pkt = NULL;
  pkt_size = 0;

#ifdef FEATURE_DUAL_SIM
  if ( mm_dual_standby_mode )
  {
    pkt_size = sizeof(LOG_UMTS_DS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C_type);

    ds_log_ptr = (LOG_UMTS_DS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C_type *)
               log_alloc(LOG_UMTS_DS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C, pkt_size);

    if (ds_log_ptr == NULL)
    {
      return FALSE;
    }

    ds_log_ptr->as_id = (uint8) sm_as_id;
    // Pack the QOS log packet and commit to DIAG.
    ds_log_ptr->connection_id = pdp->trans_id;
    //memcpy(&ds_log_ptr->delay_class, &pdp->qos.delay_class, sizeof(qos_T));
    ds_log_ptr->delay_class       = pdp->qos.delay_class;
    ds_log_ptr->reliability_class = pdp->qos.reliability_class;
    ds_log_ptr->peak_throughput   = pdp->qos.peak_throughput;
    ds_log_ptr->precedence_class  = pdp->qos.precedence_class;
    ds_log_ptr->mean_throughput   = pdp->qos.mean_throughput;
    ds_log_ptr->traffic_class     = pdp->qos.traffic_class;
    ds_log_ptr->deliver_order     = pdp->qos.deliver_order;
    ds_log_ptr->deliver_err_sdu   = pdp->qos.deliver_err_sdu;
    ds_log_ptr->max_sdu_size      = pdp->qos.max_sdu_size;
    ds_log_ptr->max_bit_rate_uplink = pdp->qos.max_bit_rare_uplink;
    ds_log_ptr->max_bit_rate_downlink =  pdp->qos.max_bit_rare_downlink;
    ds_log_ptr->residual_ber        = pdp->qos.residual_ber;
    ds_log_ptr->sdu_err_ratio       = pdp->qos.sdu_err_ratio;
    ds_log_ptr->tranfer_delay       = pdp->qos.tranfer_delay;
    ds_log_ptr->traffic_priority    = pdp->qos.traffic_priority;
    ds_log_ptr->guarantee_ber_uplink = pdp->qos.guarantee_ber_uplink;
    ds_log_ptr->guarantee_ber_dwnlink = pdp->qos.guarantee_ber_dwnlink;
  #ifdef FEATURE_REL5
    ds_log_ptr->sig_ind = pdp->qos.sig_ind; 
    ds_log_ptr->src_stat_desc  = pdp->qos.src_stat_desc; 
    ds_log_ptr->ext_max_bit_rate_downlink = pdp->qos.ext_max_bit_rate_downlink; 
    ds_log_ptr->ext_gtd_bit_rate_downlink = pdp->qos.ext_gtd_bit_rate_downlink; 
  if(mm_nv_wcdma_hsupa_category >= MM_RRC_WCMDA_DC_HSUPA_CATEGORY)
  {
    ds_log_ptr->ext_max_bit_rate_uplink = pdp->qos.ext_max_bit_rate_uplink; 
    ds_log_ptr->ext_gtd_bit_rate_uplink = pdp->qos.ext_gtd_bit_rate_uplink; 
  }
  #endif

    log_commit(ds_log_ptr);

  }
  else
#endif
  {
  pkt_size = sizeof(LOG_UMTS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C_type);
  qos_log_pkt = (LOG_UMTS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C_type *)
                   log_alloc(LOG_UMTS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C, pkt_size);

  if (qos_log_pkt == NULL)
  {
    return FALSE;
  }
      
  // Pack the QOS log packet and commit to DIAG.
  qos_log_pkt->connection_id = pdp->trans_id;
  //memcpy(&qos_log_pkt->delay_class, &pdp->qos.delay_class, sizeof(qos_T));
  qos_log_pkt->delay_class       = pdp->qos.delay_class;
  qos_log_pkt->reliability_class = pdp->qos.reliability_class;
  qos_log_pkt->peak_throughput   = pdp->qos.peak_throughput;
  qos_log_pkt->precedence_class  = pdp->qos.precedence_class;
  qos_log_pkt->mean_throughput   = pdp->qos.mean_throughput;
  qos_log_pkt->traffic_class     = pdp->qos.traffic_class;
  qos_log_pkt->deliver_order     = pdp->qos.deliver_order;
  qos_log_pkt->deliver_err_sdu   = pdp->qos.deliver_err_sdu;
  qos_log_pkt->max_sdu_size      = pdp->qos.max_sdu_size;
  qos_log_pkt->max_bit_rate_uplink = pdp->qos.max_bit_rare_uplink;
  qos_log_pkt->max_bit_rate_downlink =  pdp->qos.max_bit_rare_downlink;
  qos_log_pkt->residual_ber        = pdp->qos.residual_ber;
  qos_log_pkt->sdu_err_ratio       = pdp->qos.sdu_err_ratio;
  qos_log_pkt->tranfer_delay       = pdp->qos.tranfer_delay;
  qos_log_pkt->traffic_priority    = pdp->qos.traffic_priority;
  qos_log_pkt->guarantee_ber_uplink = pdp->qos.guarantee_ber_uplink;
  qos_log_pkt->guarantee_ber_dwnlink = pdp->qos.guarantee_ber_dwnlink;
#ifdef FEATURE_REL5
  qos_log_pkt->sig_ind = pdp->qos.sig_ind; 
  qos_log_pkt->src_stat_desc  = pdp->qos.src_stat_desc; 
  qos_log_pkt->ext_max_bit_rate_downlink = pdp->qos.ext_max_bit_rate_downlink; 
  qos_log_pkt->ext_gtd_bit_rate_downlink = pdp->qos.ext_gtd_bit_rate_downlink; 
  if(mm_nv_wcdma_hsupa_category >= MM_RRC_WCMDA_DC_HSUPA_CATEGORY)
  {
    qos_log_pkt->ext_max_bit_rate_uplink = pdp->qos.ext_max_bit_rate_uplink; 
    qos_log_pkt->ext_gtd_bit_rate_uplink = pdp->qos.ext_gtd_bit_rate_uplink; 
  }
#endif

  log_commit(qos_log_pkt);
  }

  return TRUE;
}

/*===========================================================================
FUNCTION CONVERT_RABM_RRC_EST_CAUSE 

DESCRIPTION
  This function returns the RRC establish cause based on the Traffic class.  
  Refer to the Annex L. of TS 24.008 for Establish cause descriptions.
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS
===========================================================================*/
rrc_establish_cause_e_type convert_to_rrc_est_cause(byte traffic_class )
{
  rrc_establish_cause_e_type rrc_cause;
  
  ue_rev_level_indicator_type ue_revision;
  ue_revision = sm_get_revision_level_indicator();

  /* Convert the Traffic class to appropriate RRC establish cause */
  switch (traffic_class)
  {
    case CONVERSATION_CLASS:
      MSG_MED_DS_0(SM_SUB,"RRC EST cause = CONV_CALL");
      rrc_cause = RRC_EST_ORIG_CONV_CALL;
      break;

    case STREAMING_CLASS:
      MSG_MED_DS_0(SM_SUB,"RRC EST cause = STREAMING_CLASS");
      rrc_cause = RRC_EST_ORIG_STREAM_CALL;
      break;

    case INTERACTIVE_CLASS:
      MSG_MED_DS_0(SM_SUB,"RRC EST cause = INTERACTIVE_CLASS");
      rrc_cause = RRC_EST_ORIG_INTERACT_CALL;
      break;
      
    case BACKGROUND_CLASS:
      MSG_MED_DS_0(SM_SUB,"RRC EST cause = BACKGROUND_CLASS");
      rrc_cause = RRC_EST_ORIG_BACKGND_CALL;
      break;

#ifdef FEATURE_MBMS
    #error code not present
#endif

    case SUBSCRIBED_CLASS:
      if ((ue_revision == UE_REV_REL99) && (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL6))
      {
        MSG_HIGH_DS_0(SM_SUB,"RRC EST cause = RRC_EST_ORIG_SUBSCRIBED_TRF_CALL");
        rrc_cause = RRC_EST_ORIG_SUBSCRIBED_TRF_CALL;
      }
      else
      {
        MSG_MED_DS_0(SM_SUB,"RRC EST cause = HI_PRI_SIG");
        rrc_cause = RRC_EST_HI_PRI_SIGNALLING;
      }
      break;

    default:
      MSG_MED_DS_0(SM_SUB,"RRC EST cause = HI_PRI_SIG");
      rrc_cause = RRC_EST_HI_PRI_SIGNALLING;
      break;      
  }

  return rrc_cause;
}


/*===========================================================================
FUNCTION SM_GET_MOST_DEMANDING_TRAFFIC_CLASS

DESCRIPTION
  This function returns most demanding traffic class of the PDP contexts 
exists

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  NOne
===========================================================================*/
byte sm_get_most_demanding_traffic_class(void)
{
  byte traffic_class = (byte)BACKGROUND_CLASS;
  word index = 0;


  for (index = 0; index < MAX_PDP_NUM ; index++)
  {
    if (pdp_list[index] != NULL &&
        (pdp_list[index]->pdp_state == PDP_ACTIVE ||
        pdp_list[index]->pdp_state == PDP_ACTIVE_PENDING ||
        pdp_list[index]->pdp_state == PDP_MODIFY_PENDING) )
    {
      if ((pdp_list[index]->qos.traffic_class != (byte)SUBSCRIBED_CLASS) &&
          (pdp_list[index]->qos.traffic_class < traffic_class))
      {
        traffic_class = pdp_list[index]->qos.traffic_class;
      }
    }        
  }

  return traffic_class;
}


/*===========================================================================
FUNCTION SM_PERFORM_LOCAL_PDP_DEACTIVATION

DESCRIPTION
  This function performs a local deactivation of a particular ACTIVE PDP context.
  Stops all the timers of the context.

DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_perform_local_pdp_deactivation(pdp_context_info *pdp)
{
  cc_cause_T  current_cause;
  /* Saving current cause as it will be modified in sm_send_cm_confirm*/
  current_cause = pdp->current_cause;
  MSG_HIGH_DS_1(SM_SUB,"Locally deactivating PDP NSAPI %d",pdp->nsapi.nsapi);
  
    //Reset the PDP con state
    pdp->pdp_con_state = SIG_CONNECTION_NONE;
    //change RAB state to INACTIVE
    pdp->rab_state = RABM_DEACTIVATED;
    /*Perform local deactivation in case of UE receives SM status message*/
    if (is_umts_mode()) //MBMS_PENDING
    {
      sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, pdp->nsapi.nsapi);
    }
#ifdef FEATURE_MBMS
    #error code not present
#endif
     
    #ifdef FEATURE_GSM_GPRS
    if (pdp->llc_state != LLC_NOT_CONFIRMED)
    {
      sm_send_sndcp_cmd (pdp, GSNSM_DEACTIVATE_IND);
    }
    if(pending_sm_pdu_id == pdp->trans_id)
    {
      pending_sm_pdu_id = INVALID_ID;       
    }
    #endif
      
#ifdef FEATURE_MBMS
    #error code not present
#endif
    
    //stop the timer if any active
    sm_stop_timer(pdp->trans_id);
    
    #ifdef FEATURE_MBMS
    #error code not present
#endif
    if (pdp->pdp_state == PDP_ACTIVE_PENDING)
    {
      //Send the SMREG_PDP_ACTIVATE_REJ to the CM.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm(pdp, SMREG_PDP_ACTIVATE_REJ, SM_NETWORK_CAUSE);
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      /*Send GMMSM_CLOSE_SESSION_REQ when PDP is deactivated due to 
      SM STATUS message due to cause value 81*/
      if (current_cause == (cc_cause_T)SM_INVALID_TRANS_ID)
      {
       send_gmm_close_session_req();
      }
    }
    else if(pdp->pdp_state == PDP_INACTIVE_PENDING)
    {
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
      if (!pdp->teardown_ind.ti_flag && pdp->is_primary_pdp 
        &&sm_promote_sec_pri(pdp))
      {
        sm_send_cm_confirm(pdp, SMREG_PDP_DEACTIVATE_PROMOTE_CNF,
                                                         SM_NETWORK_CAUSE);
      }
      else
      {
#endif
         if (pdp->current_cause == SM_LOCAL_PDP_DEACTIVATION)
         {
           sm_send_cm_confirm(pdp, SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
         }
         else
         {
           sm_send_cm_confirm(pdp, SMREG_PDP_DEACTIVATE_REJ, SM_NETWORK_CAUSE);
         }
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
      }
#endif
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      /*Send GMMSM_CLOSE_SESSION_REQ when PDP is deactivated due to 
      SM STATUS message due to cause value 81*/
      if (current_cause == (cc_cause_T)SM_INVALID_TRANS_ID)
      {
       send_gmm_close_session_req();
      }
    }
    else
    {
      //Send the SMREG_PDP_DEACTIVATE_IND to the CM.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
      if (!pdp->teardown_ind.ti_flag && pdp->is_primary_pdp 
           && sm_promote_sec_pri(pdp))
      {
         sm_send_cm_confirm(pdp, SMREG_PDP_DEACTIVATE_PROMOTE_IND,
                                                       SM_NETWORK_CAUSE);
      }
      else
      {
#endif
        sm_send_cm_confirm(pdp, SMREG_PDP_DEACTIVATE_IND, SM_NETWORK_CAUSE);
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
      }
#endif
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      /*Send GMMSM_CLOSE_SESSION_REQ when PDP is deactivated due to 
      SM STATUS message due to cause value 81*/
      if (current_cause == (cc_cause_T)SM_INVALID_TRANS_ID)
      {
       send_gmm_close_session_req();
      }
    }

  
}

/*===========================================================================
FUNCTION SM_PERFORM_LOCAL_DEACTIVATION

DESCRIPTION
  This function performs a local deactivation of all the ACTIVE PDP contexts.

DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_perform_local_deactivation(byte cause)
{
  word i;
  MSG_HIGH_DS_1(SM_SUB,"Locally Deactivating all PDPs due to GMM/CM reject cause %d", cause);
 /* Clear the establishment timer. */
  sm_clear_pdp_timer(TIMER_ESTABLISH_PENDING);
  //Reset the timer_est_pen flag
  timer_est_pen_flag = FALSE;  
  // Set the Signaling Connection variable as False
  sig_connection_available = FALSE;
  max_est_timer_count = 0;

//TODO"
//#ifdef FEATURE_DUAL_SIM
//#ifndef FEATURE_DUAL_DATA
//  if (!(SM_IS_AS_ID_IN_VALID_RANGE(cmd_ptr->cmd.pdp_abort_req.as_id) == TRUE &&
//       (SM_IS_VALID_AS_ID(cmd_ptr->cmd.pdp_abort_req.as_id) == TRUE)))
//  {
//    MSG_HIGH_DS_2( SM_SUB,"Invalid AS ID in sm_as_id : %d SMREG_PDP_ABORT_REQ : %d", sm_as_id, cmd_ptr->cmd.pdp_abort_req.as_id);
//    return;
//  }
//#endif
//#endif

#ifdef FEATURE_GSM_GPRS      
  pending_sm_pdu_id = INVALID_ID;
#endif

#ifdef FEATURE_MBMS
  #error code not present
#endif
  //Release all the secondary PDP contexts first
  for (i=0; i<MAX_PDP_NUM; i++)
  {
    if (pdp_list[i] == NULL)
        continue ;
    
     
    if( !pdp_list[i]->is_primary_pdp )
        
    {
      MSG_MED_DS_0(SM_SUB, "Changing PDP state to PDP_INACTIVE");
      pdp_list[i]->pdp_state = PDP_INACTIVE;
      //Reset the PDP con state
      pdp_list[i]->pdp_con_state = SIG_CONNECTION_NONE;

      pdp_list[i]->current_cause = cause;

      // Clear the PDP_ACTIVATE_PENDING timer
      sm_stop_timer (pdp_list[i]->trans_id);
      
      MSG_HIGH_DS_1(SM_SUB,"Locally deactivating PDP = %d",pdp_list[i]->trans_id);
      if (is_umts_mode()) // MBMS_PENDING
      {
        sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, pdp_list[i]->nsapi.nsapi);
      }

      #ifdef FEATURE_MBMS
      #error code not present
#endif

      #ifdef FEATURE_GSM_GPRS
      if (pdp_list[i]->llc_state != LLC_NOT_CONFIRMED)
        sm_send_sndcp_cmd (pdp_list[i], GSNSM_DEACTIVATE_IND);
      #endif
      #ifdef FEATURE_MBMS
      #error code not present
#endif
      
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      if (pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
      {
        // Send the SMREG_PDP_DEACTIVATE_CNF to the CM.
        sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
      }
      else if (pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_SEC_REQ)
      {
        sm_send_cm_confirm(pdp_list[i], SMREG_PDP_ACTIVATE_SEC_REJ, 
                                       SM_INTERNAL_CAUSE);
      }
      else
      {
        // Send the SMREG_PDP_DEACTIVATE_IND to the CM.
        sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);
      }
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/

    }
  }
  //Release the primaries now
  for (i=0; i<MAX_PDP_NUM; i++)
  {
    if (pdp_list[i] == NULL)
        continue ;
    
    if ( pdp_list[i]->is_primary_pdp )
    {
      MSG_MED_DS_0(SM_SUB, "Changing PDP state to PDP_INACTIVE");
      pdp_list[i]->pdp_state = PDP_INACTIVE;
      //Reset the PDP con state
      pdp_list[i]->pdp_con_state = SIG_CONNECTION_NONE;

      pdp_list[i]->current_cause = cause;

      // Clear the PDP_ACTIVATE_PENDING timer
      sm_stop_timer (pdp_list[i]->trans_id);
          
      MSG_HIGH_DS_1(SM_SUB,"Locally deactivating PDP = %d",pdp_list[i]->trans_id);
      if (is_umts_mode())
      {
        sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, pdp_list[i]->nsapi.nsapi);
      }
      #ifdef FEATURE_GSM_GPRS
      if (pdp_list[i]->llc_state != LLC_NOT_CONFIRMED)
        sm_send_sndcp_cmd (pdp_list[i], GSNSM_DEACTIVATE_IND);
      #endif

#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      if (pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
      {
        // Send the SMREG_PDP_DEACTIVATE_CNF to the CM.
        sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
      }
       else if(pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_REQ )
       {
          sm_send_cm_confirm(pdp_list[i], SMREG_PDP_ACTIVATE_REJ,
                             SM_INTERNAL_CAUSE);
       }
      else
      {
        // Send the SMREG_PDP_DEACTIVATE_IND to the CM.
        sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);
      }
#endif /*#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
   
    }
  }

}

/*===========================================================================
FUNCTION SM_VALIDATE_RELIABILITY_CLASS

DESCRIPTION
  This function validates the offered reliability class value against the 
  minumum reliability class requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_validate_reliability_class ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( offered > (byte)UNACK_GTP_LLC_RLC_UNPROTECTED_DATA_RELIABILITY_CLASS )
  {
    offered = (byte)UNACK_GTP_LLC_ACK_RLC_PROTECTED_DATA_RELIABILITY_CLASS;
  }
  
  if ( minval > (byte)UNACK_GTP_LLC_RLC_UNPROTECTED_DATA_RELIABILITY_CLASS )
  {
    minval = (byte)UNACK_GTP_LLC_ACK_RLC_PROTECTED_DATA_RELIABILITY_CLASS;
  }

  if ( minval != (byte)SUBSCRIBED_RELIABILITY_CLASS && 
      offered != (byte)RESERVED_RELIABILITY_CLASS )
  {  
    valid = (offered <= minval);   
  }
  else if (offered == (byte)RESERVED_RELIABILITY_CLASS)
  {
    valid = FALSE;
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Rel Class validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_DELAY_CLASS

DESCRIPTION
  This function validates the offered delay class value against the 
  minumum delay class requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_validate_delay_class ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( offered > (byte)DELAY_CLASS_BEST_EFFORT )
  {
    offered = (byte)DELAY_CLASS_BEST_EFFORT;
  }
  
  if ( minval > (byte)DELAY_CLASS_BEST_EFFORT )
  {
    minval = (byte)DELAY_CLASS_BEST_EFFORT;
  }

  if ( minval != (byte)SUBSCRIBED_DELAY_CLASS && 
      offered != (byte)RESERVED_DELAY_CLASS )
  {  
    valid = (offered <= minval);   
  }
  else if (offered == (byte)RESERVED_DELAY_CLASS)
  {
    valid = FALSE;
  }
  
  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Del Class validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_PRECEDENCE_CLASS

DESCRIPTION
  This function validates the offered precedence class value against the 
  minumum precedence class requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_validate_precedence_class ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( offered > (byte)LOW_PRIORITY_PRECEDENCE_CLASS )
  {
    offered = (byte)NORMAL_PRIORITY_PRECEDENCE_CLASS;
  }
  
  if ( minval > (byte)LOW_PRIORITY_PRECEDENCE_CLASS )
  {
    minval = (byte)NORMAL_PRIORITY_PRECEDENCE_CLASS;
  }

  if ( minval != (byte)SUBSCRIBED_PRECEDENCE_CLASS && 
      offered != (byte)RESERVED_PRECEDENCE_CLASS )
  {  
    valid = (offered <= minval);   
  }
  else if (offered == (byte)RESERVED_PRECEDENCE_CLASS)
  {
    valid = FALSE;
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Prec Class validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_PEAK_THROUGHPUT

DESCRIPTION
  This function validates the offered peak throughput value against the 
  minumum peak throughput requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/  
boolean sm_validate_peak_throughput ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( offered > (byte)PEAK_THROUGHPUT_UP_TO_256000_OPS )
  {
    offered = (byte)PEAK_THROUGHPUT_UP_TO_1000_OPS;
  }
  
  if ( minval > (byte)PEAK_THROUGHPUT_UP_TO_256000_OPS )
  {
    minval = (byte)PEAK_THROUGHPUT_UP_TO_1000_OPS;
  }

  if ( minval != (byte)SUBSCRIBED_PEAK_THROUGHPUT && 
      offered != (byte)RESERVED_PEAK_THROUGHPUT )
  {  
    valid = (offered >= minval);   
  }
  else if (offered == (byte)RESERVED_PEAK_THROUGHPUT)
  {
    valid = FALSE;
  }
  
  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Pk TP validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_MEAN_THROUGHPUT

DESCRIPTION
  This function validates the mean throughput value against the 
  minumum mean throughput requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/ 
boolean sm_validate_mean_throughput ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( offered > (byte)MEAN_THROUGHPUT_UP_TO_50000000_OPH )
  {
    offered = (byte)BEST_EFFORT_MEAN_THROUGHPUT;
  }
  
  if ( minval > (byte)MEAN_THROUGHPUT_UP_TO_50000000_OPH )
  {
    minval = (byte)BEST_EFFORT_MEAN_THROUGHPUT;
  }

  if ( minval != (byte)SUBSCRIBED_MEAN_THROUGHPUT && 
      offered != (byte)RESERVED_MEAN_THROUGHPUT )
  {  
    valid = (offered >= minval);   
  }
  else if (offered == (byte)RESERVED_MEAN_THROUGHPUT)
  {
    valid = FALSE;
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Mean TP validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_TRAFFIC_CLASS

DESCRIPTION
  This function validates the Traffic Class value against the 
  minumum traffic Class requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/ 
boolean sm_validate_traffic_class ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( minval != (byte)SUBSCRIBED_TRAFFIC_CLASS && 
      offered != (byte)RESERVED_TRAFFIC_CLASS )
  {  
    valid = (offered <= minval);   
  }
  else if (offered == (byte)RESERVED_TRAFFIC_CLASS)
  {
    valid = TRUE;
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Traffic Class validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_DELIVERY_ORDER

DESCRIPTION
  This function validates the delivery order value against the 
  minumum delivery order requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/ 
boolean sm_validate_delivery_order ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( minval != (byte)SUBSCRIBED_DELIVERY_ORDER&& 
      offered != (byte)RESERVED_DELIVERY_ORDER )
  {  
    valid = (offered <= minval);   
  }
  else if (offered == (byte)RESERVED_DELIVERY_ORDER)
  {
    valid = TRUE;
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Delivery Order validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_MAX_BITRATE_FOR_UPLINK

DESCRIPTION
  This function validates the max bitrate for uplink value against the 
  minumum max bitrate for uplink requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/ 
boolean sm_validate_max_bitrate_for_uplink ( byte offered, byte minval )
{
  boolean valid = TRUE;

  /* as per 24.008 sec 105.6.5
  * In network to MS direction:
  * 0 0 0 0 0 0 0 0 Reserved
  */
  if (offered == 0x00)
  {
    valid = FALSE;
  }
  else if ( minval != 0x00 && offered != 0xFF )
  {  
    valid = (offered >= minval);   
  }
  else if ( offered == 0xFF)
  {
    valid = (offered == minval );
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Max Bitrate for Uplink validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_MAX_BITRATE_FOR_DOWNLINK

DESCRIPTION
  This function validates the max bitrate for downlink value against the 
  minumum max bitrate for downlink requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/ 
boolean sm_validate_max_bitrate_for_downlink ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( minval != 0x00 && offered != 0xFF ) /*0xFF is for 0 kbps link*/
  {  
    valid = (offered >= minval);   
  }
  else if ( offered == 0xFF)
  {
    valid = (offered == minval );
  }
  else if (minval == 0xFE)
  {
    valid = (offered == minval );
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Max Bitrate for Downlink failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_RESIDUAL_BER

DESCRIPTION
  This function validates the Residual BER value against the 
  minumum Residual BER requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/ 
boolean sm_validate_residual_ber ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( minval != 0x0 && offered < 0xA ) /*values above 0x9 are reserved */
  {  
    valid = (offered >= minval);   
  }
  else if ( offered >= 0xA)
  {
    valid = FALSE;
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Residual BER failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_SDU_ERROR_RATIO

DESCRIPTION
  This function validates the SDU Error Ratio value against the 
  minumum SDU Error Ratio requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/ 
boolean sm_validate_sdu_error_ratio ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( minval != 0x0 && offered < 0x8 ) /*values above 0x7 are reserved */
  {  
    if ( offered == 0x7 )
    {
      offered = 0x0;
    }
    valid = (offered >= minval);   
  }
  else if ( offered >= 0x8)
  {
    valid = FALSE;
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "SDU error ratio failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_TRANSFER_DELAY

DESCRIPTION
  This function validates the Transfer delay value against the 
  minumum Transfer delay requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/ 
boolean sm_validate_transfer_delay ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( minval != 0x00 && offered != 0x3F )
  {  
    valid = (offered <= minval);   
  }
  else if ( offered == 0x3F)
  {
    valid = FALSE;
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Transfer delay validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_MAX_SDU_SIZE

DESCRIPTION
  This function validates the offered Max SDU size value against the 
  minumum Max SDU size requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_validate_max_sdu_size ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( minval != 0x00 && offered <= 0x99 )/*Values greater than0x99 is reserved*/
  {  
    valid = (offered >= minval);   
  }
  else if ( offered > 0x99)
  {
    valid = FALSE;
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "MAX SDU SIZE validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_TRAFFIC_HANDLING_PRIORITY

DESCRIPTION
  This function validates the offered traffic handling priority value against the 
  minumum traffic handling priority requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_validate_traffic_handling_priority ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( minval != (byte)SUBSCRIBED_TRAFFIC_HANDLING_PRIORITY )
  {  
    valid = (offered <= minval);   
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Traffic Handling Priority validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_DELIVERY_OF_ERRONEOUS_SDU

DESCRIPTION
  This function validates the offered delivery of erroneous sdu value against the 
  minumum delivery of erroneous sdu requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_validate_delivery_of_erroneous_sdus ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( minval != (byte)SUBSCRIBED_DELIVERY_OF_ERRONEOUS_SDUS && 
      offered != (byte)RESERVED_DELIVERY_OF_ERRONEOUS_SDUS)
  {  
    valid = (offered >= minval);   
  }
  else if (offered == (byte)RESERVED_DELIVERY_OF_ERRONEOUS_SDUS)
  {
    valid = TRUE;
  }
  
  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Del of erroneous sdu validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

#ifdef FEATURE_REL5
/*===========================================================================
FUNCTION SM_VALIDATE_SIGNALLING_INDICATION

DESCRIPTION
  This function validates the offered signalling indication value against the 
  minumum signalling indication requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_validate_signalling_indication ( byte offered, byte minval )
{
  boolean valid = TRUE;

    valid = (offered >= minval);   
  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Signalling Indication validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_SOURCE_STATISTICS_DESCRIPTOR

DESCRIPTION
  This function validates the offered source statistics descriptor value against the 
  minumum source statistics descriptor requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_validate_source_statistics_descriptor ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( offered <= (byte) SPEECH)
  {  
    valid = (offered >= minval);   
  }
  else if (offered > (byte) SPEECH)
  {
    valid = FALSE;
  }
  
  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Source Statistics Descriptor validation failed (%d/%d)", offered, minval);
  }
 
  return valid;
}

/*===========================================================================
FUNCTION SM_VALIDATE_EXT_MAX_BITRATE_FOR_DOWNLINK

DESCRIPTION
  This function validates the ext max bitrate for downlink value against the 
  minumum ext max bitrate for downlink requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/ 
boolean sm_validate_ext_max_bitrate_for_downlink ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( minval != 0x00 && offered <= 0xFA ) /*Values greater than0xFA is reserved*/
  {  
    valid = (offered >= minval);   
  }
  else if ( offered > 0xFA)
  {
    /*
      * Not setting Valid Flag to FALSE, as this can cause backward compatibality issues.
      * In future suppose, this field is extended above 0xFA for Rel-X and if NW assigns this value, then 
      * the present UE will always reject the Activation
      */
    MSG_HIGH_DS_1(SM_SUB,"Received Ext Max Bit Rate for DL than allowed for Rel-7: %d", offered);
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB, "Ext Max Bitrate for Downlink failed (%d/%d)", offered, minval);
  }
 
  return valid;
}


/*=========================================================================== 
FUNCTION SM_VALIDATE_EXT_MAX_BITRATE_FOR_UPLINK

DESCRIPTION
  This function validates the ext max bitrate for uplink value against the 
  minumum ext max bitrate for uplink requirements of the UE. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/ 
static boolean sm_validate_ext_max_bitrate_for_uplink ( byte offered, byte minval )
{
  boolean valid = TRUE;

  if ( minval != 0x00 && offered <= 0xFA ) /*Values greater than0xFA is reserved*/
  {  
    valid = (offered >= minval);   
  }
  else if ( offered > 0xFA)
  {
    /*
      * Not setting Valid Flag to FALSE, as this can cause backward compatibality issues.
      * In future suppose, this field is extended above 0xFA for Rel-X and if NW assigns this value, then 
      * the present UE will always reject the Activation
      */
    MSG_HIGH_DS_1(SM_SUB,"Received Ext Max Bit Rate for UL than allowed for Rel-7: %d", offered);
  }

  if ( !valid )
  {
    MSG_ERROR_DS_2(SM_SUB,"Ext Max Bitrate for uplink failed (%d/%d)", offered, minval);
  }
 
  return valid;
}


#endif


/*===========================================================================
FUNCTION SM_VALIDATE_QOS

DESCRIPTION
  This function validates the network offered QoS against the minimum QoS 
  requirements of the UE.

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the validation is successful and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_validate_qos ( qos_T offered_qos, qos_T min_qos )
{
  boolean valid = TRUE;
#ifdef FEATURE_REL5
  ue_rev_level_indicator_type ue_revision;
  ue_revision = sm_get_revision_level_indicator();
#endif
  if ( qos_ie_length <= 3 )
  {
    if ( !sm_validate_reliability_class ( offered_qos.reliability_class, min_qos.reliability_class ) ||
         !sm_validate_delay_class ( offered_qos.delay_class, min_qos.delay_class ) ||
         !sm_validate_precedence_class ( offered_qos.precedence_class, min_qos.precedence_class ) ||
         !sm_validate_peak_throughput ( offered_qos.peak_throughput, min_qos.peak_throughput ) ||
         !sm_validate_mean_throughput ( offered_qos.mean_throughput, min_qos.mean_throughput ) )
    {
      valid = FALSE;
    }
  }
  else /* If the N/W is R99 or later */
  {
    if (!sm_validate_traffic_class ( offered_qos.traffic_class, min_qos.traffic_class ) ||
         !sm_validate_delivery_order ( offered_qos.deliver_order, min_qos.deliver_order ) ||
         !sm_validate_max_bitrate_for_uplink( offered_qos.max_bit_rare_uplink, min_qos.max_bit_rare_uplink ) ||
         !sm_validate_max_bitrate_for_downlink( offered_qos.max_bit_rare_downlink, min_qos.max_bit_rare_downlink ) ||
         !sm_validate_residual_ber( offered_qos.residual_ber, min_qos.residual_ber ) ||
         !sm_validate_sdu_error_ratio( offered_qos.sdu_err_ratio, min_qos.sdu_err_ratio ) ||
         !sm_validate_delivery_of_erroneous_sdus( offered_qos.deliver_err_sdu, min_qos.deliver_err_sdu ) || 
         !sm_validate_max_sdu_size( offered_qos.max_sdu_size, min_qos.max_sdu_size ))
    {
      valid = FALSE;
    }
    if ( offered_qos.traffic_class != (byte)INTERACTIVE_CLASS && 
         offered_qos.traffic_class != (byte)BACKGROUND_CLASS &&
         !sm_validate_transfer_delay( offered_qos.tranfer_delay, min_qos.tranfer_delay))
    {
      valid = FALSE;
    }
  
    if ( offered_qos.traffic_class != (byte)INTERACTIVE_CLASS && 
         offered_qos.traffic_class != (byte)BACKGROUND_CLASS && 
         offered_qos.max_bit_rare_uplink != 0xFF &&
         !sm_validate_max_bitrate_for_uplink( offered_qos.guarantee_ber_uplink, min_qos.guarantee_ber_uplink ) )
    {
      valid = FALSE;
    }
  
    if ( offered_qos.traffic_class != (byte)INTERACTIVE_CLASS && 
         offered_qos.traffic_class != (byte)BACKGROUND_CLASS && 
         offered_qos.max_bit_rare_downlink != 0xFF &&
         !sm_validate_max_bitrate_for_downlink( offered_qos.guarantee_ber_dwnlink, min_qos.guarantee_ber_dwnlink ) )
    {
      valid = FALSE;
    }
  
    if ( (offered_qos.traffic_class == (byte)INTERACTIVE_CLASS ) && 
        !sm_validate_traffic_handling_priority( offered_qos.traffic_priority, min_qos.traffic_priority ))
    {
      valid = FALSE;
    }
  
 #ifdef FEATURE_REL5
    if (((ue_revision == UE_REV_REL99) && (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL5)) &&
        is_umts_mode())
    {
    
      if ( offered_qos.traffic_class != (byte)INTERACTIVE_CLASS && 
           offered_qos.traffic_class != (byte)BACKGROUND_CLASS && 
           offered_qos.max_bit_rare_downlink != 0xFF &&
           !sm_validate_ext_max_bitrate_for_downlink( offered_qos.ext_gtd_bit_rate_downlink, min_qos.ext_gtd_bit_rate_downlink ) )
      {
        valid = FALSE;
      }
  
      if ( offered_qos.traffic_class != (byte)INTERACTIVE_CLASS && 
           offered_qos.traffic_class != (byte)BACKGROUND_CLASS && 
           !sm_validate_source_statistics_descriptor( offered_qos.src_stat_desc, min_qos.src_stat_desc ) )
      {
        valid = FALSE;
      }
  
      if ( offered_qos.traffic_class != (byte)CONVERSATION_CLASS && 
           offered_qos.traffic_class != (byte)STREAMING_CLASS && 
           offered_qos.traffic_class != (byte)BACKGROUND_CLASS &&
           !sm_validate_signalling_indication( offered_qos.sig_ind, min_qos.sig_ind ) )
      {
        valid = FALSE;
      }
  
      if ( !sm_validate_ext_max_bitrate_for_downlink( offered_qos.ext_max_bit_rate_downlink, min_qos.ext_max_bit_rate_downlink ) )
      {
        valid = FALSE;
      }
     if(mm_nv_wcdma_hsupa_category >= MM_RRC_WCMDA_DC_HSUPA_CATEGORY)
     {
      if ( offered_qos.traffic_class != (byte)INTERACTIVE_CLASS && 
           offered_qos.traffic_class != (byte)BACKGROUND_CLASS && 
           offered_qos.max_bit_rare_uplink != 0xFF &&
           !sm_validate_ext_max_bitrate_for_uplink( offered_qos.ext_gtd_bit_rate_uplink, min_qos.ext_gtd_bit_rate_uplink ) )
      {
        valid = FALSE;
      }

      if ( !sm_validate_ext_max_bitrate_for_uplink( offered_qos.ext_max_bit_rate_uplink, min_qos.ext_max_bit_rate_uplink ) )
      {
        valid = FALSE;
      }
     }
      /*
      If QoS offered as 0 kbps (0xFF) for both the Maximum bitrate for downlink and the 
      Maximum bitrate for uplink at the same time. UE shall consider that as a syntactical error.
      */
      if ( ( offered_qos.max_bit_rare_uplink == 0xFF && offered_qos.max_bit_rare_downlink == 0xFF )
        )
      {
	  if(mm_nv_wcdma_hsupa_category >= MM_RRC_WCMDA_DC_HSUPA_CATEGORY)
          {
             if (offered_qos.ext_max_bit_rate_uplink == 0x00 && offered_qos.ext_max_bit_rate_downlink == 0x00 )
             {
                valid = FALSE;
             }  
          }
          else
          {
        valid = FALSE;
      }  
    }
    }
  #endif
  }

  return valid;
}

/*===========================================================================
FUNCTION SM_CHECK_DUPLICATE_PDP

DESCRIPTION
  This function checks whether the requested PDP is already existing or not

DEPENDENCIES
  None.

RETURN VALUE
  Returns con_id if the duplicate exists  and INVALID_ID otherwise.

SIDE EFFECTS

===========================================================================*/
byte sm_check_pri_duplicate_pdp( pdp_context_info *pdp)
{
  word i;
  for ( i = 0; i < MAX_PDP_NUM; i++)
  {
   if (pdp_list[i] != NULL &&
       pdp_list[i]->trans_id != pdp->trans_id &&
        pdp_list[i]->trans_id != INVALID_ID)
    {
      if(sm_compare_apn(&pdp->apn_name, &pdp_list[i]->apn_name))
      {
        if (pdp->pdp_addr.valid && pdp_list[i]->pdp_addr.valid)
        {
           if ((pdp->pdp_addr.pdp_type_org == pdp_list[i]->pdp_addr.pdp_type_org) && 
            (pdp->pdp_addr.pdp_type_num == pdp_list[i]->pdp_addr.pdp_type_num) )
           {
             if( (pdp->pdp_addr_alloc_type == PDP_ADDR_DYNAMIC &&
                 pdp_list[i]->pdp_addr_alloc_type == PDP_ADDR_DYNAMIC) || 
                (pdp->pdp_addr_alloc_type == PDP_ADDR_STATIC &&
                 pdp_list[i]->pdp_addr_alloc_type == PDP_ADDR_STATIC) )
             {
               return pdp_list[i]->trans_id;
             }
           }
        }
      }
    }
  }
 return INVALID_ID;
}

/*===========================================================================
FUNCTION SM_CHECK_PDP_COLLISION

DESCRIPTION
  This function checks whether there is an MO and MT collision.

DEPENDENCIES
  None.

RETURN VALUE
  Returns con_id if the duplicate exists  and INVALID_ID otherwise.

SIDE EFFECTS

===========================================================================*/
byte sm_check_pdp_collision( pdp_context_info *pdp)
{
  word i;
  for ( i = 0; i < MAX_PDP_NUM; i++)
  {
   if (pdp_list[i] != NULL &&
       pdp_list[i]->trans_id != pdp->trans_id &&
        pdp_list[i]->trans_id != INVALID_ID)
    {
      if((!pdp->apn_name.valid && !pdp_list[i]->apn_name.valid) ||
        sm_compare_apn(&pdp->apn_name, &pdp_list[i]->apn_name))
      {
          if (pdp->pdp_addr.valid && pdp_list[i]->pdp_addr.valid)
          {
             if ((pdp->pdp_addr.pdp_type_org == pdp_list[i]->pdp_addr.pdp_type_org) && 
                (pdp->pdp_addr.pdp_type_num == pdp_list[i]->pdp_addr.pdp_type_num) )
             {
               if( (pdp->pdp_addr_alloc_type == PDP_ADDR_DYNAMIC &&
                   pdp_list[i]->pdp_addr_alloc_type == PDP_ADDR_DYNAMIC) || 
                  (pdp->pdp_addr_alloc_type == PDP_ADDR_STATIC &&
                   pdp_list[i]->pdp_addr_alloc_type == PDP_ADDR_STATIC) )
               {
                 return pdp_list[i]->trans_id;
               }
             }
          }
      }
    }
  }
 return INVALID_ID;
}

/*===========================================================================
FUNCTION SM_COMPARE_APN

DESCRIPTION
  This function compares the two APNs

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if match is found FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_compare_apn(apn_T *cur_apn, apn_T *apn_exist)
{
   if ( cur_apn->valid )
   {
     if ((cur_apn->apn_addr_len == apn_exist->apn_addr_len) && 
       !strncasecmp((char *)cur_apn->address, (char *)apn_exist->address, cur_apn->apn_addr_len))
        return TRUE;
   }
   return FALSE;
}

/*===========================================================================
FUNCTION SM_COMPARE_PDPADDR

DESCRIPTION
  This function compares the two PDP address

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if match is found FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_compare_pdpaddr(pdp_address_T *cur_pdpaddr, pdp_address_T *pdpaddr_exist)
{
  if (cur_pdpaddr->valid)
  {
     if ((cur_pdpaddr->pdp_type_org == pdpaddr_exist->pdp_type_org) && 
        (cur_pdpaddr->pdp_type_num == pdpaddr_exist->pdp_type_num) &&
        (cur_pdpaddr->pdp_addr_len != 0) &&
        (cur_pdpaddr->pdp_addr_len == pdpaddr_exist->pdp_addr_len) &&
        ((pdpaddr_exist->pdp_addr_len) <= MAX_PDP_ADR_LEN)&&/*checking size of array before copying*/
        !memcmp(cur_pdpaddr->address, pdpaddr_exist->address, pdpaddr_exist->pdp_addr_len))
     {
        return TRUE;
     }
  }
  return FALSE;
}

#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
/*===========================================================================
FUNCTION SM_PROMOTE_SEC_PRI

DESCRIPTION
  This function picks the best QoS from the secondaries and promote to primary

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if sec is promoted FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_promote_sec_pri(pdp_context_info *cur_pdp)
{
  word index;
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL &&
        pdp_list[index]->trans_id != INVALID_ID && 
      index != cur_pdp->trans_id &&
      cur_pdp->group_id == pdp_list[index]->group_id &&
      !(pdp_list[index]->pdp_state == PDP_ACTIVE_PENDING || pdp_list[index]->pdp_state == PDP_INACTIVE))
    {
      //sec context found promote it to primary
      cur_pdp->linked_ti = pdp_list[index]->trans_id;
      cur_pdp->is_primary_pdp = FALSE;
      //update sec pdp paramets
      pdp_list[index]->linked_ti = pdp_list[index]->trans_id;
      pdp_list[index]->is_primary_pdp = TRUE;

      return TRUE;
    }
    /*Deactivate the Secondary PDP if its in upcoming state and primary pdp gets deactivated.*/
    if (pdp_list[index] != NULL &&
        pdp_list[index]->trans_id != INVALID_ID && 
      index != cur_pdp->trans_id &&
      cur_pdp->group_id == pdp_list[index]->group_id &&
      (pdp_list[index]->pdp_state == PDP_ACTIVE_PENDING || pdp_list[index]->pdp_state == PDP_INACTIVE))
    {
       MSG_HIGH_DS_1(SM_SUB,"Locally deactivating Secondary PDP when upcoming for NSAPI %d",pdp_list[index]->nsapi.nsapi);
       sm_perform_local_pdp_deactivation(pdp_list[index]);
    }
  }
  return FALSE;
}
#endif


/*===========================================================================
FUNCTION SM_CHECK_GROUP_PDP_PENDING_DEACTIVATION

DESCRIPTION
  This function checks whether any of the PDP context in the group is in 
    bringing down state

DEPENDENCIES
  None.

RETURN VALUE
  TRUE / FALSE.

SIDE EFFECTS

===========================================================================*/
boolean sm_check_group_pdp_pending_deactivation(pdp_context_info *cur_pdp)
{
  word index;
  /*---------------------------------------------------
    Check the any of the PDP context in the group is in 
    bringing down state
    ---------------------------------------------------*/
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL &&
        (pdp_list[index]->trans_id != INVALID_ID) && 
        (index != cur_pdp->trans_id) &&
        (pdp_list[index]->group_id == cur_pdp->group_id))
    {
      if (pdp_list[index]->pdp_state == PDP_INACTIVE_PENDING ||
          pdp_list[index]->pdp_state == PDP_INACTIVE)
      {
        return TRUE;
      }
    }
  }

  return FALSE;
}

/*===========================================================================
FUNCTION SM_PICK_BEST_QOS

DESCRIPTION
  This function picks the best QoS from the secondaries and promote to primary

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if sec is promoted FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
byte sm_pick_best_qos(
  pdp_context_info* pri_pdp,
  pdp_context_info* sec_pdp)
{
  byte best_qos_index;
  if (pri_pdp->qos.traffic_class == (byte)INTERACTIVE_CLASS || 
          sec_pdp->qos.traffic_class == (byte)INTERACTIVE_CLASS)
  {
    if (pri_pdp->qos.traffic_class == (byte)INTERACTIVE_CLASS && 
              sec_pdp->qos.traffic_class == (byte)INTERACTIVE_CLASS)
    {
      best_qos_index = (pri_pdp->qos.traffic_priority == 
                        sec_pdp->qos.traffic_priority)
                       ?((pri_pdp->qos.max_bit_rare_downlink == 
                           sec_pdp->qos.max_bit_rare_downlink)
                        ?((pri_pdp->qos.max_bit_rare_uplink == 
                               sec_pdp->qos.max_bit_rare_uplink)
                         ?((pri_pdp->nsapi.nsapi > sec_pdp->nsapi.nsapi)
                          ? pri_pdp->trans_id
                          : sec_pdp->trans_id)
                         :((pri_pdp->qos.max_bit_rare_uplink >
                             sec_pdp->qos.max_bit_rare_uplink)
                            ?pri_pdp->trans_id
                            :sec_pdp->trans_id))
                        :((pri_pdp->qos.max_bit_rare_downlink > 
                            sec_pdp->qos.max_bit_rare_downlink)
                          ?pri_pdp->trans_id
                          :sec_pdp->trans_id))
                       :((pri_pdp->qos.traffic_priority < 
                           sec_pdp->qos.traffic_priority)
                        ?pri_pdp->trans_id
                        :sec_pdp->trans_id);
    }
    else if(pri_pdp->qos.traffic_class == (byte)INTERACTIVE_CLASS)
    {
      best_qos_index = (pri_pdp->qos.traffic_priority == 1)
                       ?(pri_pdp->trans_id)
                       :((sec_pdp->qos.traffic_class == (byte)CONVERSATION_CLASS || 
                          sec_pdp->qos.traffic_class == (byte)STREAMING_CLASS)
                         ?sec_pdp->trans_id
                         :(pri_pdp->qos.traffic_priority == 2 ||
                           pri_pdp->qos.traffic_priority == 3)
                          ?pri_pdp->trans_id
                          :sec_pdp->trans_id);
    }
    else
    {
      best_qos_index = (sec_pdp->qos.traffic_priority == 1)
                       ?(sec_pdp->trans_id)
                       :((pri_pdp->qos.traffic_class == (byte)CONVERSATION_CLASS || 
                          pri_pdp->qos.traffic_class == (byte)STREAMING_CLASS)
                         ?pri_pdp->trans_id
                         :((sec_pdp->qos.traffic_priority == 2 ||
                           sec_pdp->qos.traffic_priority == 3)
                          ?sec_pdp->trans_id
                          :pri_pdp->trans_id));
    }
  }
  else if (pri_pdp->qos.traffic_class == (byte)STREAMING_CLASS || 
                sec_pdp->qos.traffic_class == (byte)STREAMING_CLASS)
  {
    if (pri_pdp->qos.traffic_class == (byte)STREAMING_CLASS && 
                  sec_pdp->qos.traffic_class == (byte)STREAMING_CLASS)
    {
      best_qos_index = ((pri_pdp->qos.max_bit_rare_downlink == 
                           sec_pdp->qos.max_bit_rare_downlink)
                        ?((pri_pdp->qos.max_bit_rare_uplink == 
                               sec_pdp->qos.max_bit_rare_uplink)
                         ?((pri_pdp->nsapi.nsapi > sec_pdp->nsapi.nsapi)
                          ? pri_pdp->trans_id
                          : sec_pdp->trans_id)
                         :((pri_pdp->qos.max_bit_rare_uplink >
                             sec_pdp->qos.max_bit_rare_uplink)
                            ?pri_pdp->trans_id
                            :sec_pdp->trans_id))
                        :((pri_pdp->qos.max_bit_rare_downlink > 
                            sec_pdp->qos.max_bit_rare_downlink)
                          ?pri_pdp->trans_id
                          :sec_pdp->trans_id));
    }
    else if(pri_pdp->qos.traffic_class == (byte)STREAMING_CLASS)
    {
      best_qos_index = pri_pdp->trans_id;
    }
    else
    {
      best_qos_index = sec_pdp->trans_id;
    }
  }
  else if (pri_pdp->qos.traffic_class == (byte)CONVERSATION_CLASS || 
                sec_pdp->qos.traffic_class == (byte)CONVERSATION_CLASS)
  {
    if (pri_pdp->qos.traffic_class == (byte)CONVERSATION_CLASS && 
                 sec_pdp->qos.traffic_class == (byte)CONVERSATION_CLASS)
    {
      best_qos_index = ((pri_pdp->qos.max_bit_rare_downlink == 
                           sec_pdp->qos.max_bit_rare_downlink)
                        ?((pri_pdp->qos.max_bit_rare_uplink == 
                               sec_pdp->qos.max_bit_rare_uplink)
                         ?((pri_pdp->nsapi.nsapi > sec_pdp->nsapi.nsapi)
                          ? pri_pdp->trans_id
                          : sec_pdp->trans_id)
                         :((pri_pdp->qos.max_bit_rare_uplink >
                             sec_pdp->qos.max_bit_rare_uplink)
                            ?pri_pdp->trans_id
                            :sec_pdp->trans_id))
                        :((pri_pdp->qos.max_bit_rare_downlink > 
                            sec_pdp->qos.max_bit_rare_downlink)
                          ?pri_pdp->trans_id
                          :sec_pdp->trans_id));
    }
    else if(pri_pdp->qos.traffic_class == (byte)CONVERSATION_CLASS)
    {
      best_qos_index = pri_pdp->trans_id;
    }
    else
    {
      best_qos_index = sec_pdp->trans_id;
    }
  }
  else
  {
    best_qos_index = ((pri_pdp->qos.max_bit_rare_downlink == 
                           sec_pdp->qos.max_bit_rare_downlink)
                        ?((pri_pdp->qos.max_bit_rare_uplink == 
                               sec_pdp->qos.max_bit_rare_uplink)
                         ?((pri_pdp->nsapi.nsapi > sec_pdp->nsapi.nsapi)
                          ? pri_pdp->trans_id
                          : sec_pdp->trans_id)
                         :((pri_pdp->qos.max_bit_rare_uplink >
                             sec_pdp->qos.max_bit_rare_uplink)
                            ?pri_pdp->trans_id
                            :sec_pdp->trans_id))
                        :((pri_pdp->qos.max_bit_rare_downlink > 
                            sec_pdp->qos.max_bit_rare_downlink)
                          ?pri_pdp->trans_id
                          :sec_pdp->trans_id));
  }
  return best_qos_index;

}
// #ifdef FEATURE_INACTIVITY_HANDLING
/*===========================================================================
FUNCTION SM_FORCE_GMMSM_CLOSESESSION_REQ

DESCRIPTION
  This function is called by RRC to trigger GMMSM_CLOSE_SESSION_REQ, if RRC stays
  longer time in DCH with no PS Data Activity.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void sm_force_gmmsm_close_session_req()
{
  mm_cmd_type * gmm_cmd;
  
  if (!sm_check_pdp_procedure_pending())
  {
    MSG_HIGH_DS_0(SM_SUB," Force RRC to go IDLE");
    gmm_cmd = mm_get_cmd_buf();
    if (gmm_cmd != NULL)
    {
       gmm_cmd->cmd.gmmsm_session_close_req.header.message_set     = MS_GMM_SM;
       gmm_cmd->cmd.gmmsm_session_close_req.header.message_id      = GMMSM_CLOSE_SESSION_REQ;
       gmm_cmd->cmd.gmmsm_session_close_req.header.message_len_lsb = 0;
       gmm_cmd->cmd.gmmsm_session_close_req.header.message_len_msb = 0;  
       gmm_cmd->cmd.gmmsm_session_close_req.cause = SM_PS_SIG_REL_REQ;
      MSG_HIGH_DS_0(SM_SUB, "Sending CLOSE Session Req to GMM");
      mm_put_cmd (gmm_cmd);
    }
    else
    {
      MSG_ERROR_DS_0(SM_SUB, "Unable to get a GMM command buffer");
    }  
  }
  else{
    MSG_HIGH_DS_0(SM_SUB,"Ignore RRC CLOSE_SESSION_REQ");
  }
}
// #endif /*FEATURE_INACTIVITY_HANDLING*/
/*===========================================================================
FUNCTION SM_LOCAL_DEACT_ALL_ASSOCIATED_PDP_CTXT

DESCRIPTION
  This function checks whether any of the PDP context is associated with given primary PDP
  and deactivates them locally.

DEPENDENCIES
  None.

RETURN VALUE
  NONE.

SIDE EFFECTS

===========================================================================*/
void sm_local_deact_all_associated_pdp_ctxt(pdp_context_info *cur_pdp)
{
  word index;
  if (cur_pdp == NULL)
    return;
  
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL && 
        (pdp_list[index]->trans_id != INVALID_ID) && 
        (pdp_list[index]->trans_id != cur_pdp->trans_id) &&
        (pdp_list[index]->group_id == cur_pdp->group_id) )
    {
      sm_perform_local_pdp_deactivation(pdp_list[index]);
    }
  }
}
#ifdef FEATURE_MBMS
#error code not present
#endif


#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
FUNCTION CHECK_PDP_INACTIVE_STATE

DESCRIPTION
  This function checks if
  a. Only One PDP exists and is in Deactivation process
  b. All the existing PDP are in Deactivation process
  
DEPENDENCIES
  
  
RETURN VALUE

    return TRUE, if the above condition passes.

SIDE EFFECTS

===========================================================================*/

boolean check_pdp_inactive_state()
{
  word index;
  boolean pdp_inactive_state = TRUE;
  
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if ( pdp_list[index] != NULL && 
         pdp_list[index]->trans_id != INVALID_ID && 
         !(pdp_list[index]->pdp_state == PDP_INACTIVE || 
           pdp_list[index]->pdp_state == PDP_INACTIVE_PENDING) )
    {
      pdp_inactive_state= FALSE;
      break;
    }
  }

  return pdp_inactive_state;  
}

#endif /*FEATURE_GPRS_PS_HANDOVER*/

/* --------------------------------------------------------------------------
FUNCTION SM_GET_BCM_FROM_PCO

DESCRIPTION
  This function extracts BCM mode from PCO received from Network
DEPENDENCIES
  None
RETURN VALUE
  NONE.
SIDE EFFECTS
** ------------------------------------------------------------------------*/
bearer_ctrl_mode_T sm_get_bcm_from_pco(const protocol_cfg_options_T *pco)
{
  bearer_ctrl_mode_T bcm = BCM_MS_ONLY;
  word i;
  
  if(pco->valid)
  {
    for (i = 0; i < pco->num_protocol_ids; i++)
    {
      if (pco->procol_info[i].protocol_id[0] == 0x00 && 
          pco->procol_info[i].protocol_id[1] == 0x05 &&
          pco->procol_info[i].protocol_len == 1)
      {
        if (pco->procol_info[i].protocol_contents[0] == (byte)BCM_MS_NW)
        {
          bcm = (bearer_ctrl_mode_T) pco->procol_info[i].protocol_contents[0];
          break;
        }  
      }
    }  
  }  
  return bcm;
}

/*===========================================================================

FUNCTION SM_SET_BCM_IN_PCO

DESCRIPTION
  
  This function overwrites the BCM value in PCO for a given PDP context.

  
DEPENDENCIES
  None

RETURN VALUE
  TRUE: If successfully overwritten the BCM.
  FALSE: If function is unsuccessful in overwritting the BCM

SIDE EFFECTS
  None

===========================================================================*/
void sm_set_bcm_in_pco(pdp_context_info* cur_pdp, bearer_ctrl_mode_T bcm )
{
  byte i;
  
  if(cur_pdp->config_options.valid)
  {
    for (i = 0; i < cur_pdp->config_options.num_protocol_ids; i++)
    {
      if (cur_pdp->config_options.procol_info[i].protocol_id[0] == 0x00 && 
          cur_pdp->config_options.procol_info[i].protocol_id[1] == 0x05 &&
          cur_pdp->config_options.procol_info[i].protocol_len == 1)
      {
        cur_pdp->config_options.procol_info[i].protocol_contents[0] = (byte)bcm;
        return;
      }
    }  
  }
}

/*===========================================================================
FUNCTION SM_GET_PS_CALL_INFO

DESCRIPTION
   This function fills PDP address,QOS and APN values of activa PDP context
   in MM buffer
DEPENDENCIES
  None
  
RETURN VALUE

     NONE

SIDE EFFECTS

===========================================================================*/
void sm_get_ps_call_info(sys_ps_call_info_s_type *ps_call_info)
{
  rex_enter_crit_sect(&sm_crit_sect);
  if(pdp_list[ps_call_info_ti] != NULL)
  {
    ps_call_info->pdp_address = pdp_list[ps_call_info_ti]->pdp_addr;
    ps_call_info->qos = pdp_list[ps_call_info_ti]->qos;
    ps_call_info->apn = pdp_list[ps_call_info_ti]->apn_name;
  }
  else
  {
    ps_call_info->pdp_address.valid = FALSE;
    ps_call_info->qos.valid = FALSE;
    ps_call_info->apn.valid = FALSE;
  }
  rex_leave_crit_sect(&sm_crit_sect);

}

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION SM_OK_FOR_TUNEAWAY

DESCRIPTION
  This function checks if
  any of the existing PDPs are of CONVERSATION or STREAMING class traffic type.
  PDP that are getting deactivated are exluded as they are going to be cleared.
  This indicates, if the lower layers can tuneaway.
  
DEPENDENCIES
  
  
RETURN VALUE

  return FALSE, if the above condition passes. Implies NOT to Tuneaway

SIDE EFFECTS

===========================================================================*/

boolean sm_ok_for_tuneaway()
{
  uint16 index;

  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL && 
         pdp_list[index]->trans_id != INVALID_ID && 
        ( 
        ( (pdp_list[index]->pdp_state == PDP_ACTIVE) && 
           (pdp_list[index]->qos.traffic_class == (int)CONVERSATION_CLASS || 
              pdp_list[index]->qos.traffic_class == (int)STREAMING_CLASS) ) ||
          (( pdp_list[index]->pdp_state == PDP_MODIFY_PENDING) &&
          (pdp_list[index]->req_qos.traffic_class == (int)CONVERSATION_CLASS || 
             pdp_list[index]->req_qos.traffic_class == (int)STREAMING_CLASS) ) ||
          ( (pdp_list[index]->pdp_state == PDP_ACTIVE_PENDING) && 
             (( is_umts_mode() && (pdp_list[index]->qos.traffic_class == (int)CONVERSATION_CLASS || 
                                  pdp_list[index]->qos.traffic_class == (int)STREAMING_CLASS) )
#ifdef FEATURE_GSM_GPRS
              || (is_gsm_mode() && sm_max_timer_count[index] == 0)
#endif
          ))
         ))
    {
      return FALSE;
    }

  }
  return TRUE;
}

boolean SM_IS_VALID_AS_ID(sys_modem_as_id_e_type cmd_as_id)
{
#ifdef FEATURE_DUAL_DATA
    if (IS_NOT_VALID_SUB_ID(cmd_as_id))
    {
      MSG_ERROR_DS_1( RABM_SUB,"Invalid AS ID recieved, message as_id: %d", cmd_as_id );
      return FALSE;
    }
#else
    if ( (sm_as_id == SYS_MODEM_AS_ID_NONE) ||  (sm_as_id != cmd_as_id) )
    {
      MSG_ERROR_DS_2( RABM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , cmd_as_id);
      return FALSE;
    }
#endif
    return TRUE;
}

#endif /* FEATURE_DUAL_SIM */

/*===========================================================================
FUNCTION SM_GET_AS_ID

DESCRIPTION
  This function will extract SIM_ID from all incoming primitives and enables  processing of  the primitive 
  in the context of extracted SIM_ID
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if AS ID is valid

SIDE EFFECTS
  None
===========================================================================*/
boolean sm_get_as_id(sm_cmd_type *cmd_ptr)
{
  boolean status = TRUE;
  sys_modem_as_id_e_type  as_id = SYS_MODEM_AS_ID_NONE;

  switch (cmd_ptr->header.message_set)
  {
    case MS_RABM_SM:
      switch(cmd_ptr->header.cmd_id)
      {
        case RABMSM_ACTIVATE_RSP:   
          as_id = cmd_ptr->cmd.rabm_activate_rsp.as_id;
          break;
           
        case RABMSM_DEACTIVATE_RSP:
          as_id = cmd_ptr->cmd.rabm_deactivate_rsp.as_id;
          break;

        case RABMSM_MODIFY_RSP:      
          as_id = cmd_ptr->cmd.rabm_modify_rsp.as_id;
          break;

        case RABMSM_STATUS_REQ: 
          as_id = cmd_ptr->cmd.rabm_status_req.as_id;
          break;

        case RABMSM_REESTABLISH_IND:
          as_id = cmd_ptr->cmd.rabm_reest_ind.as_id;
          break;

        default:
          MSG_ERROR_DS_1(SM_SUB, "=SM= Unknown RABM message (%d)",
                cmd_ptr->header.cmd_id);
          break;
      } 
      break;

    case MS_GMM_SM:
      switch (cmd_ptr->header.cmd_id)
      {
        case GMMSM_ESTABLISH_CNF:
          as_id = cmd_ptr->cmd.gmm_establish_cnf.as_id;
          break;

#ifdef FEATURE_REL6_PS_FOP
        case GMMSM_PS_FOP_IND:
          as_id = cmd_ptr->cmd.gmm_ps_fop_ind.as_id;
          break;
#endif/* FEATURE_REL6_PS_FOP */ 

        case GMMSM_ESTABLISH_IND:
          as_id = cmd_ptr->cmd.gmm_establish_ind.as_id;
          break;

        case GMMSM_CON_RELEASE_IND :
          as_id = cmd_ptr->cmd.gmm_con_release_ind.as_id;
          break;

        case GMMSM_FAST_DORMANCY_CNF :
          as_id = cmd_ptr->cmd.gmm_fast_dormancy_cnf.as_id;
          break;

        case GMMSM_ESTABLISH_REJ:
          as_id = cmd_ptr->cmd.gmm_establish_rej.as_id;
          break;

        case GMMSM_GPRS_RELEASE_IND:
          as_id = cmd_ptr->cmd.gmm_gprs_rel_ind.as_id;
          break;

        case GMMSM_UNITDATA_IND:
          as_id = cmd_ptr->cmd.gmm_unitdata_ind.as_id;
          break;

#ifdef FEATURE_GSM_GPRS
        case GMMSM_TLLI_ASSIGNMENT_IND:
          as_id = cmd_ptr->cmd.snsm_tlli_assignment_ind.as_id;
          break;
#endif

        case GMMSM_RAT_CHANGE_REQ:
          as_id = cmd_ptr->cmd.gmm_rat_change_req.as_id;
          break;

        case GMMSM_RAT_RESUME_REQ:
          as_id = cmd_ptr->cmd.gmm_rat_resume_req.as_id;
          break;

        case GMMSM_PDP_MBMS_STATUS_IND:
          as_id = cmd_ptr->cmd.gmm_pdp_mbms_context_status.as_id;
          break;

       case GMMSM_CON_RELEASE_REJ:
         as_id = cmd_ptr->cmd.gmmsm_con_release_rej.as_id;
         break;

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
       case GMMSM_STATUS_IND:
         as_id = cmd_ptr->cmd.gmm_status_ind.as_id;
         break;
#endif /* #ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM */

#if defined(FEATURE_LTE)
       case GMMSM_ISR_STATUS_IND:
          as_id = cmd_ptr->cmd.gmm_isr_status_ind.as_id;
          break;
#endif

       case GMMSM_UNBLOCK_ALL_APN:
          as_id = cmd_ptr->cmd.gmmsm_unblock_all_apn.as_id;
          break; 
#if defined(FEATURE_LTE)
        case GMMSM_MULTIMODE_SUBS_CHGD_REQ:
          as_id = cmd_ptr->cmd.gmmsm_multimode_subs_chgd_req.as_id;
          break; 
#endif
       default:
          MSG_ERROR_DS_1(SM_SUB, "=SM= Unknown GMM message (%d)",
                cmd_ptr->header.cmd_id);
          break;
      }
      break;

#ifdef FEATURE_GSM_GPRS
    case MS_SNDCP_SM:
      switch (cmd_ptr->header.cmd_id)
      {
        case SNSM_ACTIVATE_RESP :
          as_id = cmd_ptr->cmd.sndcp_activate_resp.as_id;
          break;

        case SNSM_DEACTIVATE_RESP:
          as_id = cmd_ptr->cmd.sndcp_deactivate_resp.as_id;
          break;

        case SNSM_MODIFY_RESP:
          as_id = cmd_ptr->cmd.sndcp_modify_resp.as_id;
          break;

        case SNSM_STATUS_REQ:
          as_id = cmd_ptr->cmd.sndcp_status_req.as_id;
          break;

       default:
          MSG_ERROR_DS_1(SM_SUB, "=SM= Unknown SNDCP message (%d)",
                cmd_ptr->header.cmd_id);
          break;
      }
      break;
#endif
#ifdef FEATURE_MBMS
    #error code not present
#endif
    case MS_CM_SM:
      switch (cmd_ptr->header.cmd_id)
      {
        case SMREG_PDP_ACTIVATE_REQ :
          as_id = cmd_ptr->cmd.pdp_activate_req.as_id;
          break;

        case SMREG_PDP_DEACTIVATE_REQ:
          as_id = cmd_ptr->cmd.pdp_deactivate_req.as_id;
          break;

        case SMREG_PDP_MODIFY_REQ:
          as_id = cmd_ptr->cmd.pdp_modify_req.as_id;
          break;

        case SMREG_PDP_ACTIVATE_SEC_REQ:
          as_id = cmd_ptr->cmd.pdp_sec_activate_req.as_id;
          break;

        case SMREG_PDP_ACTIVATE_REJ_RSP:
          as_id = cmd_ptr->cmd.pdp_activate_rej_resp.as_id; 
          break;

        case SMREG_PDP_ABORT_REQ:
         as_id = cmd_ptr->cmd.pdp_abort_req.as_id; 
          break;

        case SMREG_PS_SIGNALING_REL_REQ:
          as_id = cmd_ptr->cmd.ps_signalling_rel_req.as_id; 
          break;

        case SMREG_PDP_GRACEFUL_ABORT_REQ:
          as_id = cmd_ptr->cmd.pdp_graceful_abort_req.as_id; 
          break;

#ifdef FEATURE_MBMS
        #error code not present
#endif
        case SMREG_PDP_NW_MODIFY_CNF:
          as_id = cmd_ptr->cmd.pdp_nw_modify_cnf.as_id; 
          break;
     
        case SMREG_PDP_NW_MODIFY_REJ:
          as_id = cmd_ptr->cmd.pdp_nw_modify_rej.as_id; 
          break;

        case SMREG_BLOCKED_APN_LIST:
          as_id = cmd_ptr->cmd.blocked_apn_list_ind.as_id; 
          break;

       default:
          MSG_ERROR_DS_1(SM_SUB, "=REG= Unknown CM message (%d)",
                cmd_ptr->header.cmd_id);
          break;
      }
      break;
    case MS_TIMER:
#ifdef FEATURE_DUAL_DATA
      as_id = (sys_modem_as_id_e_type)((cmd_ptr->cmd.timer_expiry.timer_id & 0x10)>>4);
#else 
      as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
      break;

    default:
          MSG_ERROR_DS_2(SM_SUB, "=SM= Unknown message set (%d)  id (%d)",
                cmd_ptr->header.message_set, cmd_ptr->header.cmd_id);
          break;
  }

  /*Log incoming messages to SM in sm buffer */
  sm_add_message_to_debug_buffer(cmd_ptr->header.message_set,
                                 cmd_ptr->header.cmd_id,
                                 TO_SM, as_id);
#ifdef FEATURE_DUAL_DATA
   /* Non Dual Data, validation is done as part of processing message */
  sm_as_id = (sm_as_id_type_e)as_id;
  status = SM_IS_VALID_AS_ID(as_id);
#endif
  return status;
}


#if defined(FEATURE_LTE)
/*===========================================================================
FUNCTION sm_check_isr_deact_status_on_interrat

DESCRIPTION
  This function called by GMM to check if PDP activated after exsists.

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
boolean sm_check_isr_deact_status_on_interrat(void)
{
  int index;
  boolean return_value = FALSE;

  if(sm_deact_isr_on_interrat_status_get() == SM_DEACT_ISR_ON_INTERRAT_TO_GW)
  {
    sm_esm_isr_status_set(FALSE);
    return_value = TRUE;
  }
  else
  {
    for (index=0; index<MAX_PDP_NUM; index++)
    {
      if(pdp_list[index] != NULL && pdp_list[index]->pdp_state != PDP_INACTIVE)
      {
        if(pdp_list[index]->isr_pdp_act_status == CTXT_ACT_AFTER_ISR_IN_LTE)
        {
          sm_esm_isr_status_set(FALSE);
          return_value = TRUE;
          break;
        }
      }
    } 
  }
  return return_value;
}



/*===========================================================================
FUNCTION SM_CHECK_IF_PDP_EXISTS

DESCRIPTION
  This function called by GMM to check if any ACTIVE pdp exists

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
boolean sm_check_if_active_pdp_exists(void)
{
  int index;

  for (index=0; index<MAX_PDP_NUM; index++)
  {
    if(pdp_list[index] != NULL && pdp_list[index]->pdp_state != PDP_INACTIVE)
    {
      return TRUE;
    }
  } 
  return FALSE;
}
#endif

#ifdef FEATURE_MMAL
/*===========================================================================
FUNCTION SM_FILL_AND_SEND_PDP_CONTEXT_LOG_INFO

DESCRIPTION
  This function packs the required info and puts in the message router

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
void sm_fill_and_send_pdp_context_log_info(const pdp_context_info *cur_pdp)
{
  sm_pdp_context_request_log_info_ind_T   pdp_context_req_msg;

  /* Init the MSGR header */
  msgr_init_hdr( &pdp_context_req_msg.msg_hdr, MSGR_NAS_SM, NAS_SM_PDP_CONTEXT_REQUEST_LOG_INFO_IND );

  /* Fill the request info */
  pdp_context_req_msg.context_id = cur_pdp->context_id;
  pdp_context_req_msg.assoc_context_id = cur_pdp->assoc_context_id;
  pdp_context_req_msg.is_primary_pdp = cur_pdp->is_primary_pdp;
  pdp_context_req_msg.initiator = cur_pdp->mt_orig_type;
  pdp_context_req_msg.nsapi = cur_pdp->nsapi.nsapi;
  pdp_context_req_msg.sapi = cur_pdp->llc_sapi.sapi;
  memscpy(pdp_context_req_msg.apn, sizeof(pdp_context_req_msg.apn),
          cur_pdp->apn_name.address, cur_pdp->apn_name.apn_addr_len) ;
  MSG_HIGH_DS_1(SM_SUB,"NAS - Filling and sending message for Ctxt id %d",cur_pdp->context_id);

  /* Send the context request info */
  (void)esm_msgr_send( &pdp_context_req_msg.msg_hdr, sizeof(pdp_context_req_msg) );


}
#endif
/*===========================================================================
FUNCTION SM_GET_REVISION_LEVEL_INDICATOR

DESCRIPTION
  This function calls the appropriate gmm revision level indicator function
  based on the device configurations

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
ue_rev_level_indicator_type sm_get_revision_level_indicator(void)
{
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type stack_id_nas;
  sys_modem_as_id_e_type sub_id = (sys_modem_as_id_e_type)sm_as_id;
  ue_rev_level_indicator_type revision_level_ind = UE_REV_REL99;

  stack_id_nas = sub_id;
#ifdef FEATURE_SGLTE
  if(!IS_NOT_VALID_SUB_ID(sub_id) )
  {
    if(MM_SUB_IS_SGLTE_SUB(sub_id))
    {
      stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
    }
  }
  else
  {
    MSG_ERROR_DS_1(SM_SUB,"sm_get_revision_level_indicator - Invalid SUB ID: %d", sub_id);
    return revision_level_ind;
  }
#endif
  return get_revision_level_indicator_per_nas_stacks(stack_id_nas);
#elif defined (FEATURE_SGLTE)
  return get_revision_level_indicator_per_nas_stacks(mm_ps_stack_id);
#else
  return get_revision_level_indicator();
#endif
}




/*===========================================================================
FUNCTION SM_IS_APN_BLOCKED

DESCRIPTION
  This function tells if a particular APN is blocked

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If APN is blocked
  FALSE: If APN is not blocked

SIDE EFFECTS

===========================================================================*/
boolean sm_is_apn_blocked(apn_T apn)
{
  uint16 index;
#ifdef FEATURE_DUAL_SIM
    if (!SM_IS_AS_ID_IN_VALID_RANGE((sys_modem_as_id_e_type)sm_as_id))
    {
      MSG_ERROR_DS_1( SM_SUB,"SM AS ID not valid : %d", sm_as_id );
       return FALSE;
    }
#endif
    for ( index = 0 ; index < sm_blocked_apn_list.length; index++)
    {
      if(sm_compare_apn(&apn, &sm_blocked_apn_list.apn_name[index]))
      return TRUE;
    }
    return FALSE;
}

/*===========================================================================
FUNCTION SM_PS_CALL_VOTE_FOR_RESELCTION

DESCRIPTION
  Function used by RRC to know if PS call establishment is on going and RAB is not established yet

DEPENDENCIES
  None.

RETURN VALUE
  NAS_VOTE_FOR_RESEL_FALSE : If PS call establishment/restablishment is in progress and still waiting for the RAB establishment
  NAS_VOTE_FOR_RESEL_TRUE  : If no PS call OR PS call established with RABs OR PDP is in dormant
SIDE EFFECTS

===========================================================================*/
nas_vote_for_reselection_e_type sm_ps_call_vote_for_reselection(sys_modem_as_id_e_type asid)
{
  nas_vote_for_reselection_e_type  ret_val = NAS_VOTE_FOR_RESEL_TRUE;
  byte is_valid_as_id = TRUE;
  
#if defined (FEATURE_DUAL_SIM)
    if(SM_IS_VALID_AS_ID(asid) == FALSE)
    {
      MSG_HIGH_DS_2( SM_SUB,"SM AS ID Conflict sm_as_id : %d, message as_id: %d", sm_as_id, asid);
      is_valid_as_id = FALSE;
    }
#endif
   if(is_valid_as_id == TRUE)
   {
      if(sm_ps_vote_for_reselection == TRUE)
      {
         ret_val = NAS_VOTE_FOR_RESEL_TRUE;
      }
      else
      {
        ret_val = NAS_VOTE_FOR_RESEL_FALSE;
      }
   }
  
  return ret_val;
}

/*===========================================================================
FUNCTION SM_UNSET_PS_VOTE_FOR_RESELECTION

DESCRIPTION
  This function is used to unset the Reselection/HO allowed status

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
void sm_unset_ps_vote_for_reselection(void)
{
  byte i;
  boolean entry_found = FALSE;

  for (i=0; i<MAX_PDP_NUM; i++)
  {
    if(pdp_list[i] != NULL && ( (pdp_list[i]->pdp_con_state ==  SIG_CONNECTION_PENDING && pdp_list[i]->pdp_state == PDP_INACTIVE) || 
                                    (pdp_list[i]->rab_state ==  RABM_ACTIVE_PENDING) ) )
    {
       entry_found = TRUE;
    }
  }
  
  if( rabm_check_rab_reest_pend() || rabm_check_serv_req_complete_pending() )
  {
    entry_found = TRUE;
  }
  
  if( entry_found == FALSE)
  {
     sm_ps_vote_for_reselection = TRUE;
  }

}

/*===========================================================================
FUNCTION SM_ADD_MESSAGE_TO_DEBUG_BUFFER

DESCRIPTION
  This function adds messages that SM received and sent to the sm_esm_debug_buffer.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

NOTE
  Parameter "sm_esm_message_direction" indicates whether the particular message is
  sent or received by SM
===========================================================================*/
void sm_add_message_to_debug_buffer
(
  byte message_set, 
  dword message_id,
  sm_esm_message_direction_type_T sm_esm_message_direction,
  sys_modem_as_id_e_type as_id
)
{
  if(sm_esm_debug_buffer_index >= SM_DEBUG_MESSAGE_BUFFER_SIZE)
  {
    sm_esm_debug_buffer_index = 0;
  }
  sm_esm_debug_buffer[sm_esm_debug_buffer_index].debug_buffer_type.sm_info_debug_buffer.message_set = message_set;
  sm_esm_debug_buffer[sm_esm_debug_buffer_index].debug_buffer_type.sm_info_debug_buffer.message_id = message_id;
  //sm_esm_debug_buffer[sm_esm_debug_buffer_index].present_gmm_state= gmm_state;
#ifdef FEATURE_LTE
  //sm_esm_debug_buffer[sm_esm_debug_buffer_index].present_emm_state = EMM_GET_STATE();
#endif
  sm_esm_debug_buffer[sm_esm_debug_buffer_index].sm_esm_message_direction = sm_esm_message_direction;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  sm_esm_debug_buffer[sm_esm_debug_buffer_index].as_id = as_id;
#endif
  sm_esm_debug_buffer_index++;
}

/*===========================================================================
FUNCTION ESM_ADD_MESSAGE_TO_DEBUG_BUFFER

DESCRIPTION
  This function adds messages that ESM received and sent to the sm debug buffer.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

NOTE
  Parameter "sm_esm_message_direction" indicates whether the particular message is
  sent or received by ESM.
===========================================================================*/
#ifdef FEATURE_LTE
void esm_add_message_to_debug_buffer
(
  msgr_umid_type    msg_umid,
  sm_esm_message_direction_type_T sm_esm_message_direction,
  sys_modem_as_id_e_type as_id
)
{
  if(sm_esm_debug_buffer_index >= SM_DEBUG_MESSAGE_BUFFER_SIZE)
  {
    sm_esm_debug_buffer_index = 0;
  }
  sm_esm_debug_buffer[sm_esm_debug_buffer_index].debug_buffer_type.esm_info_debug_buffer.msg_umid= msg_umid;
//  sm_esm_debug_buffer[sm_esm_debug_buffer_index].present_gmm_state= gmm_state;
  sm_esm_debug_buffer[sm_esm_debug_buffer_index].sm_esm_message_direction = sm_esm_message_direction;
                                                  
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  sm_esm_debug_buffer[sm_esm_debug_buffer_index].as_id = as_id;
#endif
  sm_esm_debug_buffer_index++;
}
#endif
