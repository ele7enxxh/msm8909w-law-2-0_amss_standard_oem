/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     GMM Connection Management (GMMCONN.C)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmmconn.c_v   1.22   18 Jul 2002 11:37:30   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmmconn.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/13   am      Passing internal cause for internal error while RAB reestablishment
10/01/12   am      Back off timer code
01/26/01   jca     Created module
05/25/01   jca     Added detailed code to support service request procedures.
06/08/01   jca     Added support for integrity protection.
06/13/01   jca     Removed processing of RRC_CLOSE_SESSION_CNF primitive.
06/18/01   jca     Now sending GMMSM_ESTABLISH_CNF on RRC_SYNC_IND (INTEGRITY_EST)
08/03/01   jca     Now setting domain_id in RRC_DATA_REQ.
09/12/01   jca     Modified GMMRABM_REESTABLISHMENT_RSP interface and processing.
11/06/01   jca     Now tracking PS session status and integrity status independently.
02/15/02   ks      Added Support for Test Control (TC) entity.
02/23/02   ks      setting the cause of the GMMSM_CON_RELEASE_IND command to
                   SIG_CONNECTION_RELEASE when the RRC connection is released.
03/08/02   ks      Added processing of establish cause RRC_EST_TERM_HI_SIGNALLING
                   and RRC_EST_TERM_LOW_SIGNALLING in gmm_process_rrc_page_ind.
04/22/02   ks      Added nreg_release_pending checking in the REL_IND from
                   RRC, if enabled the NREG_CNF is sent to the REG.
04/29/02   jca     Added new cause code in RRC_SYNC_IND.
04/30/02   ks      Support for RRC_PAGE_CAUSE_UNKNOWN cause. Removed redundant
                   nreg_release_pending check in gmm_handle_connection_management.
05/17/02   jca     Removed conditional processing on GMM SERVICE REJECT message.
05/11/02   mks     Now sending SM signaling message log packets to diag
06/05/02   ks      Added support for Dual mode operation.
07/06/02   vdr     Added new function void gmm_process_abnormal_page_ind() and
                   changed processing of the GMMSM_ESTABLISH_REQ in order to
                   support GMM-LLC interface
07/18/02   vdr     Renamed FEATURE_GPRS to be FEATURE_GSM_GPRS compilation flag
                   Fixed GMM-SM behavior for multi-mode operation MS
07/22/02   ks      Perform Service change or RAT change after connection release
                   based on the Pending flag.
08/02/02   ks      Added support for new reject cause #15 NO_SUITABLE_CELLS_IN_LA
                   Now setting proper cause in mmr_reg_cnf & mmr_service_ind.
09/04/02   vdr     Fixed determination of the CKSN
09/04/02   vdr     Now sending RRC and LLC requests according to the MS current
                   operation mode (GSM or UMTS)
09/30/02   ks      Replaced the GMMSM_ERROR_IND with GMMSM_CON_RELEASE_IND
                   and GMMSM_RELEASE_IND with GMMSM_GPRS_RELEASE_IND.
10/07/02   jca     Changes to support CS/PS combined attach procedures.
11/06/02   jca     Modified code to set GMM serving PLMN status through calls to
                   new function gmm_update_mm_serving_plmn_info().
11/08/02   cd      Added support for access class blocking in
                   gmm_handle_connection_management().
11/12/02   jca     Added support for RRC_CLOSE_SESSION_CNF.
11/14/02   vdr     Added Support for PS SMS connection management
12/06/02   jca     Changes to support CS/PS combined attach for GPRS mode.
12/06/02   mks     Added support for GPRS suspension procedure - Added code
                   to reject CNM/CM establish requests in GPRS SUSPENDED state.
                   Added processing for GPRS resumption information if any, when
                   UE leaves the dedicated mode after GPRS suspension.
12/09/02   ks      Sending the GMMSM_CON_RELEASE_IND to SM after receives a
                   RRC_REL_IND if RABM is the service_request_initiator.
12/16/02   jca     Changes to support CS_ONLY<->CS/PS combined mode switching.
01/10/03   vdr     Changes of CKSN determination
01/15/03   ks      Added support for the GMMSM_CLOSE_SESSION_REQ primitive to
                   CLOSE a PS session upon request from SM.
01/27/03   jca     Changes to support PS_ONLY<->CS/PS combined mode switching.
02/13/03   jca     Now performing a Routing Area Update procedure on PLMN change
                   instead of GPRS Attach procedure.
02/25/03   jca     Modified function gmm_process_rrc_page_ind() as per 24.008
                   spec CR 349.
02/27/03   vdr     Added processing of the GMMSM_RAT_RESUME_CNF by sending
                   MMR_SERVICE_IND if MM is during the UMTS to GSM RAT change
                   and report to REG is required
02/28/03   ks      Sending the RRC establishment cause in function
                   gmm_initiate_service_request for both SM/RABM EST request.
03/06/03   ks      sending the GMMSM_CON_RELEASE_IND to SM in the function
                   gmm_handle_connection_management even if the connection is
                   not initiated by the SM/RABM entities.
03/07/03   vdr     Fixed processing of RR_REL_IND when MM performs post UMTS
                   to GSM CS handover
03/07/03   mks     Now processing GPRS resumption information during service
                   mode changes (to cover CS+PS -> PS transitions).
03/13/03   vdr     Featured out processing of the GMMSM_RAT_RESUME_CNF
                   (FEATURE_GSM_GPRS)
03/18/03   mks     Added a fix to correctly set the "cause" field in
                   MMR_SERVICE_IND while notifying REG about PS data services
                   resumption.
03/25/03   jca     Service Request procedure not performed if no GPRS service available.
03/26/03   jca     Corrected processing for SERVICE REJECT msg with GMM cause #40.
03/27/03   vdr     Fixed PS continuity feature processing when UE is attached to CS
                   domain only or when it attached to PS domain as well but current
                   cell does not provive GPRS services
03/31/03   jca     Now initiate GPRS Detach procedure on release of RRC connection
                   if gmm_gprs_detach_pending flag set to TRUE.
04/11/03   jca     Missed setting GMM state to DEREGISTERED.LIMITED_SERVICE
                   for a SERVICE REJECT gmm_cause case.
04/14/03   jca     Added code to pend RAU if MM state != IDLE and GSM mode active.
04/22/03   mks     Reset mm_serving_plmn.info.ps_data_suspend flag upon resumption of
                   packet data services.
04/29/03   jca     Added support for change of network mode of operation.
05/23/03   jca     Make sure to update mm_serving_plmn_info when GMM deregisters.
05/29/03   mks     Service Request procedure for RAB re-establishment is now performed
                   only in GMM_REGISTERED state.
06/11/03   jca     When operating with ms_op_mode set to CS_PS in network op mode 2
                   send the MMR_REG_CNF upon completion of MM registration and send
                   the PS registration status later in a MMR_SERVICE_IND.
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
06/16/03   jca     Now checking for NULL mmcoord_pending_reg_message before calling
                   function gmm_initiate_gprs_detach().
07/14/03   ks      Removed sending Service_ind after receiving GMMSM_RAT_RESUME_CNF
                   as it was already sent at the completion of RAU procedure.
07/16/03   jca     Added support for new RR_GMM_GPRS_RESUMPTION_IND primitive.
07/18/03   jca     Fixed bug where receipt of GMMSM_CLOSE_SESSION_REQ was causing
                   RR_ABORT_REQ to be generated while operating in GSM mode.
07/29/03   jca     Fixed PS Attach-on-demand problem related to clearing of the
                   mm_serv_change_pending boolean flag.
07/30/03   jca     Clear mm_serv_change_pending flag when changing service domain.
08/12/03   jca     Fixed bug where RAU not performed on MMR_REG_REQ commanded
                   transition from GSM NMO1 to WCDMA NMO2 (same PLMN).
08/15/03   jca     Fixed similar bug where RAU not performed on MMR_REG_REQ commanded
                   transition from WCDMA NMO1 to GSM NMO2 (same PLMN).
09/22/03   mks     Corrected the GMM behavior per TS24.008 to refrain from initiating
                   UMTS service request procedure in GMM_ATTEMPTING_TO_UPDATE substate.
10/09/03   mks     Now rejecting SM/RABM establishment requests in GMM_LIMITED_SERVICE
                   substate.
10/15/03   cd      Corrected start of T3317
11/04/03   jca     Treat ACTIVATION_IND as implicit ABORT_IND when in the
                   process of setting up an RR connection.
11/17/03   mks     Added support for TS24.008 specification CR #359.
01/27/04   jca     Start timer T3317 when gmm_initiate_service_request() is called
                   if a GMM signalling connection already exists (PMM_CONNECTED).
02/27/04   ks      If the SERVICE REQUEST message was sent in PMM_CONNECTED mode,
                   the reception of SERVICE ACCEPT message shall be treated as success
                   completion of the procedure instead of the Integrity being enabled.
03/01/04   ks      Now calling mm_handle_sm_resume_cnf() when SM sends a
                   GMMSM_RAT_RESUME_CNF primitive.
03/25/04   ks      Resetting the service_request_initiator to NULL after sending
                   the connection response to the request initiator.
04/19/04   jca     Check for RAI change against gmm_previous_serving_rai
                   rather than the RAI stored on the SIM.
05/12/04   jca     Set rrc_establishment_cause to RRC_EST_CALL_RE_ESTABLISH when performing
                   initial RA Update procedure after receipt of a RRC_REL_IND with rel_cause
                   set to RRC_DIRECTED_SIGNALLING_REESTABLISHMENT.
05/12/04   cd      Added support for WCDMA multiband feature
05/19/04   cd      Now sending the band_pref in a RRC_PLMN_LIST_REQ if either GSM or WCDMA
                   multiband feature is defined.
05/19/04   jca     Corrected bug where RAU was not occurring on REG-commanded RAT change.
05/26/04   jca     Ensure GMMSM_ESTABLISH_REQ always receives a response when
                   GMM state is set to GMM_REGISTERED.
06/02/04   ks      Added enhancements to support WCDMA multiband feature.
06/18/04   jca     The routing area updating attempt counter shall be reset when a new
                   routing area is entered and the MS is in substate ATTEMPTING-TO-UPDATE
                   (3GPP TS 24.008 section 4.7.5).
                   Always treat RRC_ACTIVATION_IND as a lower layer failure condition when
                   a GPRS Attach, Detach, or Routing Area Updating procedure is active.
07/12/04   cd      Now sending a MMR_REG_CNF at the end of a PS attach procedure resulting
                   from a CS+PS manual registration after the UE failed to register for CS
                   services only.  (CR 43917)
07/12/04   cd      Added check for GMM_REGISTERED_INITIATED when sending a MMR_REG_CNF
                   in the middle of a CS+PS registration.
09/10/04   kvk     Indicating SM about the PMM_CONNECTED with GMMSM_ESTABLISH_IND
                   as response to RRC page for PDP context modification to setup the
                   streaming and conversational RABs
09/22/04   jca     Set mmcoord_mmr_reg_cnf.cause to MMR_CAUSE_FAILURE_REMAIN_ON_PLMN
                   if CS registration has failed and PS registration is still pending
                   when operating in NMO 2.
10/12/04   ks      Fixed the issue where RAU is not being triggered after receiving
                   RRC_REL_IND with cause DIRECTED_SIGNALLING_REESTABLISHMENT.
11/11/04   jca     Ensure that MMR_REG_CNF is not sent before PS registration
                   attempt has completed.
11/16/04   jca     Added delay_sig_con_release flag in RRC_ABORT_REQ.
12/09/04   jca     Ensure that sending the RR_PLMN_SELECT_REQ precedes
                   the initiation of the GPRS Attach procedure when
                   the mm_serv_change_pending flag is set.
12/17/04   sm      Added check to move GMM state to
                   GMM_SERVICE_REQUEST_INITIATED
                   if there is no RAU going on, in case PAGE indication
01/07/04   kvk     Reject the SM establish req with cause RR_ABORT_REDIAL_NOT_ALLOWED
                   to stop silent redial when RRC gives abort indication with  cause
                   RRC_MAX_ACCESS_FAILURE.
01/10/05   abe     In case of RRC REL indication, If gmm is in GMM_DEREGISTERED state
                   and gmm sunstate is GMM_ATTEMPTING_TO_ATTACH then don't initiate
                   another attach immediately, instead wait for the respective timer
                   to be expired to initiate attach.
01/11/05   jca     On release of RRC connection and mm_plmn_list_req_pending flag set
                   to TRUE, do not send the RRC_PLMN_LIST_REQ if mm_rat_change_pending
                   flag also set to TRUE.
01/20/05   jca     Added extra code to ensure GPRS Attach procedure kicked off at
                   proper times when RR_REL_IND is received.
01/21/05   jca     Corrected start of T3330 timer when operating in UMTS mode.
01/26/05   jca     Trigger combined RAU procedure after receiving RRC_REL_IND
                   with cause DIRECTED_SIGNALLING_REESTABLISHMENT with ms_op_mode
                   set to CS+PS and operating in NMO1.
01/26/05   sm      Corrected start of T3310 timer when operating in UMTS mode.
02/02/05   jca     Ensure mm_serving_plmn.info.service_status is set to STATUS_LIMITED
                   rather than STATUS_SRV whenever CS and/or PS service is available
                   but UE not registered for CS or PS service.
03/10/05   jca     Ensure GMM substate set to NORMAL_SERVICE before initiating
                   GPRS Attach procedure.
03/17/05   abe     Stop sending RRC_CLOSE_SERSSION_REQ after receiving
                   GMMSM_CLOSE_SESSION_REQ if any GMM specific procedure running
03/21/05   hj      Handled the NO_SUITABLE_CELLS_IN_LA and NATIONAL_ROAMING_NOT_ALLOWED
                   causes for SERVICE REJECT message for CR# 58591.
04/26/05   up      Updated gmm_handle_connection_management to send the pending
                   PLMN_LIST_REQ only after receiving ABORT_IND/CNF or REL_IND.
04/29/05   jca     Ensure that sending GPRS Detach precedes sending the RR_PLMN_SELECT_REQ
                   when both the mm_serv_change_pending and gmm_gprs_detach_pending flags
                   are set and then an RR_REL_IND is received.
05/05/2005 abe     After inter RAT reselection, don't reject GMMSM_ESTABLISH_REQ
                   based on the ps_data_suspend flag value TRUE. Instead trigger
                   RAU first which will resume the operation and then process
                   the ESTABLISH request.
05/18/05   kvk     When pmm mode is pmm_connected and if the RAB reestablishment is for
                   type streaming/conversational traffic class no need to send the Service
                   request of type=data but send the GMMRABM_REESTABLISH_RSP rightaway.
05/23/05   abe     Set the value of the field auth_failed_bar_cells to FALSE in RR(C)_ABORT_REQ
05/24/05   ss      Sending Status Message when service reject is recieved in wrong state.
05/30/05   abe     Send MMCNM_REL_CNF upon receiving MMCNM_REL_REQ.
06/22/05   iv      modified ‘gmm_handle_connection_management’function, not to send RAU
                   when GMM is in 'GMM_REGISTERED_INITIATED'state after receiving
                   RRC_REL_IND with cause RRC_DIRECTED_SIGNALLING_REESTABLISHMENT.
07/20/05   abe     If WtoG CCO is is progress then do not rest the variables
                   gmm_ps_signalling_connected and gmm_integrity_protection_activated upon
                   receiving RR_ACTIVATION_IND,because if CCO failed then the same signalling
                   connection would be used in RRC.
08/18/05   jca     Ensure MMR_SERVICE_IND is generated on change of service domain.
08/19/05   jca     Generate a GMMRABM_REESTABLISH_REJ if a GMMRABM_REESTABLISH_REQ is
                   received while operating in GSM mode.
08/22/05   kvk     Added notification to SM with the cause in case SR for page response
                   got rejected by the Network.
08/24/05   jca     Set CS Update Status to U3 ROAMING NOT ALLOWED when SERVICE REJECT
                   message received with cause codes #11, #12, #13, and #15.
09/01/05   hs      Perform RAU on RRC_ABORT_CNF with cause RRC_DIRECTED_SIGNALLING_REESTABLISHMENT
09/06/05   abe     Setting the is_emergency flag to FALSE while sending RRC_DATA_REQ. This flag 
                   is introduced for the FEATURE_OUT_OF_SERVICE_ENHANCEMENT
09/08/05   abe     Taking the previous fix out of feature flag FEATURE_OUT_OF_SERVICE_ENHANCEMENT.
11/11/05   ss      Updated for Rel5 changes.
10/06/05   ajt     Invalidate Authentication Cache when upon receiving CM Service Accept/Reject                   
11/21/05   hj      Disable silent redial if GMM state is RAU Initiated, for GCF Test cases.
11/23/05   ajt     Added handling of PDP Context Status IE in GMM Service Accept                   
11/28/05   ss     Changed the cause value in T3317 timer expiry primitive to abort PS procedure.
12/01/05   kvk     New member traffic_class is added in primitive GMMRABM_REESTABLISH_REQ. Now
                   traffic_class is compared rather than rrc_est_cause to see the RAB is 
                   streaming/conversation as rrc_est_cause will now refelect the most demanding 
                   traffic class of all PDPs
12/16/05   ss     Reverted release 5 changes for Service request procedure.
12/20/05   ss     Reverting Service request procedure T3317 timer expiry changes
01/18/06   ss     Handling Service Reject with cause 40 in states other than SERVICE_REQUEST_INITIATED state.
01/18/06   up     Set transaction id while sending the pending PLMN LIST REQ.
02/13/06   ajt    On receiving RR_REL_IND, initiate attach only if gmm_gprs_attach_pending flag is TRUE. Else
                  We may trigger Attach after RR_REL_IND is received on network releasing RR connection after 
                  sending LU Reject with Cause PLMN Not Allowed. As per spec, LU Reject with cause PLMN Not 
                  Allowed should add this plmn into F-PLMN list, and no Attach is allowed on F-PLMN. However, 
                  as per our design, REG adds to F-PLMN list and we send REG_CNF/Service_ind only after receiving
                  RR_REL_IND. Therefore, REG won't get chance to execute till we process RR_REL_IND completely and
                  MM will detect PLMN still as non-forbidden.
02/16/06   ss    Now rejecting GMMSM_ESTABLISH_REQ when W to G interrat in progress.
04/18/06   ss    Added cause value to GMMSM_GPRS_RELEASE_IND
06/07/06   ss    Rejecting GMMSM_ESTABLISH_REQ in Attempting to update substate.
06/13/06   ss    Changing NAS events logging to gmmllcm.c
06/14/06   up    Called new gmm_update_mm_serving_plmn_info for GSTK.
07/14/06   pk     Updated for Feature DSAC.
04/09/07   rd    SMS optimization changes are incorporated
06/19/07   NR    Adding check to reject the pending service request in case of RRC_REL_IND with cause #7
03/06/07   vsm  adding a primitive from RABM to GMM gmmrabm_rab_rel_ind to indicate the release of RAB by AS
                        For an active PDP.
02/07/08    ab    Ignoring PS page with PTMSI if mode is not PMM_IDLE
02/20/08   jbk    Remove GCF NV Item dependence on GCF 12.9.9
04/04/08   ab    Rejecting SM service request in GSM mode when RAU is pending
08/05/08   abhi  Removing assignment of service request initiator to MS_GMM_RABM before sending service request
07/05/11   rajesh  Sending reject for GMMSM_ESTABLISH_REQ to SM if PS data is suspended in DS
07/14/11   rajesh  Made code changes to stop triggering of RAU on receiving RRC_ACTIVATION_IND.
                   NAS should wait for RRC_SERVICE_IND before triggering RAU.
08/03/11   rajesh  Reject the PDP ACTIVATE REQ from SM with cause "PDP_LOWERLAYER_ERROR"
                   when active rat is LTE.
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity 
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "comdef.h"
#include "err.h"
#include "cause.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm_v.h"
#include "mm_umts_v.h"
#include "mm_sim.h"
#include "gmm_v.h"
#include "gmm_int_v.h"
#include "gmm_msg.h"
#include "sms_gmm.h"
#include "sm_gmm.h"
#include "sm_v.h"
#include "rabm_gmm.h"
#include "rabmtask_v.h"
#include "naslog_v.h"
#include "mm_coord.h"
#include "mmrat.h"
#include "ds3gpp_api.h"
#include <stringl/stringl.h>
#ifdef FEATURE_TC
#include "tc_gmm.h"
#endif

#ifdef FEATURE_GSM_GPRS
#include "gmmllcm.h"
#include "gmmsuspend.h"
#include "reg_mm_v.h"
#endif

#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

#ifdef FEATURE_LTE
#include "emm_utility.h"
#include "mm_multimode_common.h"
#include "emm_sim_handler.h"
#endif
#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#endif


#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 


boolean gmm_procedure_rerun_ongoing_sim[MAX_NAS_STACKS];
#define gmm_procedure_rerun_ongoing gmm_procedure_rerun_ongoing_sim[mm_as_id]

#else /* FEATURE_DUAL_SIM  ||  FEATURE_SGLTE*/


boolean gmm_procedure_rerun_ongoing;

#endif /* FEATURE_DUAL_SIM or FEATURE_SGLTE*/

#ifdef FEATURE_WRLF_SYSTEM_SEL
#ifdef FEATURE_MODEM_HEAP
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern serving_plmn_info_T *mm_wrlf_serving_plmn_ptr_sim[MAX_NAS_STACKS];
#define mm_wrlf_serving_plmn_ptr mm_wrlf_serving_plmn_ptr_sim[mm_as_id]
#else
extern serving_plmn_info_T *mm_wrlf_serving_plmn_ptr;
#endif
#else
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern serving_plmn_info_T mm_wrlf_serving_plmn_sim[MAX_NAS_STACKS];
#define mm_wrlf_serving_plmn mm_wrlf_serving_plmn_sim[mm_as_id]
#else
extern serving_plmn_info_T mm_wrlf_serving_plmn;
#endif
#endif
#endif


void mm_send_mmcnm_rel_ind_ps
(
   byte    reject_cause_value,
   boolean reestablishment_possible
);

#ifdef FEATURE_WCDMA
/*===========================================================================

FUNCTION gmm_send_rrc_data_request

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_send_rrc_data_request( byte *nas_msg_p, dword length )
{
    rrc_cmd_type *rrc_cmd = NULL;
#ifdef FEATURE_SEGMENT_LOADING
    interface_t *ptr_W = NULL;     
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL 
    if(mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
    {
#ifdef FEATURE_MODEM_HEAP
     rrc_cmd = mm_get_rrc_cmd_buf(
                             mm_wrlf_serving_plmn_ptr->info.active_rat );
#else
     rrc_cmd = mm_get_rrc_cmd_buf(
                             mm_wrlf_serving_plmn.info.active_rat );
#endif
    }
    else
#endif /* FEATURE_WRLF_SYSTEM_SEL */

    if (is_umts_mode())
    {
      rrc_cmd = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
    }

    if (rrc_cmd == NULL)
    {
      MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
    }
#ifdef FEATURE_SEGMENT_LOADING
    ptr_W = get_wcdma_interface();
#endif
    rrc_cmd->cmd_hdr.cmd_id              = RRC_DATA_REQ;
    rrc_cmd->cmd.data_req.cn_domain_id   = RRC_PS_DOMAIN_CN_ID;
    rrc_cmd->cmd.data_req.priority       = RRC_NAS_MSG_HIGH_PRIO;
    rrc_cmd->cmd.data_req.is_emergency_call = FALSE;
    rrc_cmd->cmd.data_req.nas_msg.length = length;
#ifndef FEATURE_MODEM_HEAP
    rrc_cmd->cmd.data_req.nas_msg.value =
      mm_mem_alloc( rrc_cmd->cmd.data_req.nas_msg.length );
#else
    if (is_wcdma_mode()
#ifdef FEATURE_WRLF_SYSTEM_SEL
         || ((mm_wcdma_rlf_state != WCDMA_RLF_IDLE) &&
             (mm_wrlf_serving_plmn_ptr->info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS))
#endif
        )
    {
      rrc_cmd->cmd.data_req.nas_msg.value =
#ifdef FEATURE_SEGMENT_LOADING
        IWCDMA_rrc_malloc_for_external_cmd(ptr_W, rrc_cmd->cmd.data_req.nas_msg.length );
#else
        rrc_malloc_for_external_cmd( rrc_cmd->cmd.data_req.nas_msg.length );
#endif
    }
#ifdef FEATURE_TDSCDMA
    else if (is_tdscdma_mode()
#ifdef FEATURE_WRLF_SYSTEM_SEL
             || ((mm_wcdma_rlf_state != WCDMA_RLF_IDLE) &&
             (mm_wrlf_serving_plmn_ptr->info.active_rat == SYS_RAT_TDS_RADIO_ACCESS))
#endif
    )
    {
      rrc_cmd->cmd.data_req.nas_msg.value =
        mm_mem_alloc( rrc_cmd->cmd.data_req.nas_msg.length );
    }
#endif /* FEATURE_TDSCDMA */
#endif

    if (rrc_cmd->cmd.data_req.nas_msg.value == NULL)
    {
      MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
    }

    memscpy(rrc_cmd->cmd.data_req.nas_msg.value, rrc_cmd->cmd.data_req.nas_msg.length,
           (void *)(nas_msg_p ),
            rrc_cmd->cmd.data_req.nas_msg.length );

    MSG_HIGH_DS_0(MM_SUB, "=MM= Sending RRC_DATA_REQ ");
#ifdef FEATURE_WRLF_SYSTEM_SEL 
    if (mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
    {
#ifdef FEATURE_MODEM_HEAP
      send_cmd_to_rrc( rrc_cmd, mm_wrlf_serving_plmn_ptr->info.active_rat  );
#else
      send_cmd_to_rrc( rrc_cmd, mm_wrlf_serving_plmn.info.active_rat  );
#endif
    }
    else
#endif
    if(is_umts_mode())
    {
      send_cmd_to_rrc( rrc_cmd, mm_serving_plmn.info.active_rat);
    }

    if( nas_msg_p != NULL)
    {
#ifndef FEATURE_MODEM_HEAP
      gs_free( nas_msg_p);
      nas_msg_p = NULL;
#else
      modem_mem_free( nas_msg_p , MODEM_MEM_CLIENT_NAS);
      nas_msg_p = NULL;
#endif
    }
}
#endif


/*===========================================================================

FUNCTION GMM_INITIATE_SERVICE_REQUEST

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_initiate_service_request
(
  rrc_establish_cause_e_type establish_cause,
  auth_security_context_id security_context,
  gmm_ie_pdp_context_status_T* uplink_data_status
)
{
  
  if ( mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE )
  {
    MSG_HIGH_DS_1( MM_SUB, "=MM= Service request blocked service_state %d",
               mm_serving_plmn.gmm_service_state);
  }
  else
  {
    /* ----------------------------------------------------------------------------
    ** The MS initiates the Service request procedure by sending a SERVICE REQUEST
    ** message.  The timer T3317 shall be started after the SERVICE REQUEST message
    ** has been sent and state GMM-SERVICE-REQUEST-INITIATED is entered.
    ** ---------------------------------------------------------------------------- */
    gmm_send_service_request_message( establish_cause, security_context,
                                      uplink_data_status
                                    );

    /* If RAU is pending, then RAU is sent and this state shall not
     ** in GMM_SERVICE_REQUEST_INITIATED.
    */
    if (gmm_state != GMM_ROUTING_AREA_UPDATING_INITIATED)
    {
      gmm_set_state( GMM_SERVICE_REQUEST_INITIATED );
      mm_stop_timer(TIMER_T3340);
      mm_stop_timer(TIMER_PS_CONN_REL);

      if (ps_session_status == MMRRC_ACTIVE)
      {
        mm_start_timer( TIMER_T3317, DEFAULT_TIMEOUT );
      }
    }
  }
} /* end gmm_initiate_service_request() */


/*===========================================================================

FUNCTION GMM_HANDLE_SERVICE_REQUEST

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_handle_service_request( mm_cmd_type *message )
{
  rabm_cmd_type            *rabm_cmd;
#ifdef FEATURE_WCDMA
  rrc_cmd_type             *rrc_cmd;
#endif /* !defined(FEATURE_NAS_REL6) && defined(FEATURE_WCDMA) */
  sm_cmd_type              *sm_cmd;
  gmm_service_reject_msg_T service_reject_msg;
  gmm_service_accept_msg_T service_accept_msg;
  mmr_service_ind_s_type   mmr_service_ind;
  boolean service_request_retry = FALSE;
#ifdef FEATURE_NAS_REL10
  gmm_t3346_backoff_timer_value = 0;
#endif

  mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW_DS( MM_SUB, "=MM= Calling gmm_handle_service_request()", 0,0,0 );

  switch (message->cmd.hdr.message_set)
  {
    case MS_MM_RRC:
    case MS_MM_TDSRRC:
      switch (message->cmd.hdr.message_id)
      {
        case RRC_SYNC_IND:
          /* --------------------------------------------------------------------
          ** If the SERVICE REQUEST message was sent in PMM-IDLE mode, the
          ** indication from the lower layers that the security mode setting
          ** procedure is completed shall be treated as a sucessful completion of
          ** the procedure.

          ** If the SERVICE REQUEST message was sent in PMM_CONNECTED mode, then
          ** the reception of SERVICE ACCEPT message shall be treated as successful
          ** completion of the procedure. The timer T3317 shall be stopped, and the MS enters
          ** GMM-REGISTERED state and PMM-CONNECTED mode.
          ** -------------------------------------------------------------------- */
          if (((message->cmd.rrc_sync_ind.cause == RRC_INTEGRITY_ESTABLISHED) ||
               (message->cmd.rrc_sync_ind.cause == RRC_INTEGRITY_CIPHERING_ESTABLISHED)) &&
               ((gmm_ps_signalling_connected) && ((pmm_mode == PMM_IDLE)
#ifdef FEATURE_TDSCDMA
                                                  || is_tdscdma_mode()
#endif /* FEATURE_TDSCDMA */
                                                  )))
          {
            pmm_mode = PMM_CONNECTED;
            MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_CONNECTED");

            if (gmm_service_request_initiator == MS_GMM_SM)
            {
              MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_CNF");
              gmm_send_gmmsm_establish_cnf(FALSE);
            }
            else if (gmm_service_request_initiator == MS_GMM_RABM)
            {
              rabm_cmd = rabm_get_cmd_buf();

              if (rabm_cmd == NULL)
              {
                MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
              }

              rabm_cmd->header.message_set = MS_GMM_RABM;
              rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_RSP;
#ifdef FEATURE_DUAL_SIM
              rabm_cmd->cmd.gmm_reestablish_rsp.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

              /*Spec reference: 11.2.2         
                Completion of the Security Mode Control procedure after sending a SERVICE REQUEST with service type "data".*/
              MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_RSP");
              mm_start_timer( TIMER_T3319, gmm_t3319_timer_value );
              rabm_put_cmd( rabm_cmd );
              /*adding message to the debug buffer*/
              mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                              ,(sys_modem_as_id_e_type)mm_as_id
                                            );
              gmm_send_gmmsm_establish_ind(SM_PS_SIG_CONN_AVAIL);	      
            }
            else if (gmm_service_request_initiator == MS_CC_MM)
            {
              /* Send MMCNM_EST_CNF to CNM */
              mm_confirm_mm_connection(MS_CC_MM_PS) ;
            }
#ifdef FEATURE_SECONDARY_PDP
            else if (gmm_service_request_type == GMM_PAGING_RESPONSE)
            {
              mm_start_timer(TIMER_FORBID_TDS_RESELECTION, DEFAULT_TIMEOUT);
              gmm_controls_resel_dec_mt_ps_call = TRUE;
              gmm_send_gmmsm_establish_ind(SM_UNASSIGNED_CAUSE);
              MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_IND");

            }
#endif
            /* Set the Service_request_initiator to NULL after sending the response to Initiator */
            gmm_service_request_initiator = NULL;

            mm_stop_timer( TIMER_T3317 );
            gmm_set_state( GMM_REGISTERED );
            if(gmm_periodic_rau_pending)
            {
              gmm_update_ra_type();
              if (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)
              {
                gmm_initiate_routing_area_update();
              }
            }
            gmm_periodic_rau_pending = FALSE;
          }
          break;

        case RRC_OPEN_SESSION_CNF:
          if (message->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_FAILURE ||
          message->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_FAILURE_CONN_REL_IN_PROG)
          {
            /* -----------------------------------------------------
            ** Lower layer failure - the procedure shall be aborted.
            ** ----------------------------------------------------- */
            if(mm_ps_domain_possible_reest && 
               message->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_FAILURE_CONN_REL_IN_PROG && !gmm_periodic_rau_pending)
            {
              service_request_retry = TRUE;
            }
            gmm_set_state( GMM_REGISTERED );
            mm_stop_timer( TIMER_T3317 );
            if(gmm_periodic_rau_pending)
            {
              mm_ps_domain_possible_reest = FALSE;
              gmm_update_ra_type();
              if (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)
              {
                gmm_initiate_routing_area_update();
              }
            }
            if(!service_request_retry)
            {
              if (gmm_service_request_initiator == MS_GMM_SM)
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ");
                gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
              }
              else if (gmm_service_request_initiator == MS_GMM_RABM)
              {
                rabm_cmd = rabm_get_cmd_buf();

                if (rabm_cmd == NULL)
                {
                  MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                }

                rabm_cmd->header.message_set = MS_GMM_RABM;
                rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
                if(message->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_FAILURE)
                {
                 rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
                }
                else
                {
                  rabm_cmd->cmd.gmm_reestablish_rej.cause = (byte)RAB_RB_DS_L2_CLEANUP_COLLISION;
                }
#ifdef FEATURE_DUAL_SIM
                rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ");

                rabm_put_cmd( rabm_cmd );
                /*adding message to the debug buffer*/
                mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                              ,(sys_modem_as_id_e_type)mm_as_id
                                            );
              }
              else if (gmm_service_request_initiator == MS_CC_MM)
              {
                /* Send MMCNM_EST_REJ to CNM */
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_RRC_OPEN_SESSION_FAILURE, MMCC_SMS_PS);
#else
                mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_RRC_OPEN_SESSION_FAILURE);
#endif
              }

              /* Set the Service_request_initiator to NULL after sending the response to Initiator */
              gmm_service_request_initiator = NULL;
              gmm_controls_resel_dec_mt_ps_call = FALSE;
            }
            service_request_retry = FALSE;
          }
          else if (message->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_SUCCESS)
          {
            /* for tracking the RAT on which last PS signalling information is sent */
            if ( message->cmd.hdr.message_set == MS_MM_TDSRRC
#ifdef FEATURE_SEGMENT_LOADING
               &&get_tdscdma_interface()
#endif
)
            {
              gmm_last_signalling_sent_rat = SYS_RAT_TDS_RADIO_ACCESS;
            }
            else if (TRUE
#ifdef FEATURE_SEGMENT_LOADING
               &&get_wcdma_interface()
#endif
)
            {
              gmm_last_signalling_sent_rat = SYS_RAT_UMTS_RADIO_ACCESS;
            }
            
            if (gmm_integrity_protection_activated)
            {
              /* -------------------------------------------------------------------
              ** An indication from the lower layers that the security mode setting
              ** procedure was completed has already been received.  The timer T3317
              ** shall be stopped, and the MS enters GMM-REGISTERED state and
              ** PMM-CONNECTED mode.
              ** ------------------------------------------------------------------- */
              if (gmm_service_request_initiator == MS_GMM_SM)
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_CNF");
                gmm_send_gmmsm_establish_cnf(FALSE);
              }
              else if (gmm_service_request_initiator == MS_GMM_RABM)
              {
                rabm_cmd = rabm_get_cmd_buf();

                if (rabm_cmd == NULL)
                {
                  MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                }

                rabm_cmd->header.message_set = MS_GMM_RABM;
                rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_RSP;
#ifdef FEATURE_DUAL_SIM
                rabm_cmd->cmd.gmm_reestablish_rsp.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_RSP");

                rabm_put_cmd( rabm_cmd );
                
                /*adding message to the debug buffer*/
                mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                               ,(sys_modem_as_id_e_type)mm_as_id
                                              );
                gmm_send_gmmsm_establish_ind(SM_PS_SIG_CONN_AVAIL);		
              }
              else if (gmm_service_request_initiator == MS_CC_MM)
              {
                /* Send MMCNM_EST_CNF to CNM */
                mm_confirm_mm_connection(MS_CC_MM_PS) ;
              }

              /* Set the Service_request_initiator to NULL after sending the response to Initiator */
              gmm_service_request_initiator = NULL;

              mm_stop_timer( TIMER_T3317 );
              gmm_set_state( GMM_REGISTERED );
              if(gmm_periodic_rau_pending)
              {
                gmm_update_ra_type();
                if (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)
                {
                  gmm_initiate_routing_area_update();
                }
             }  
            }
          }
          break;

        case RRC_CLOSE_SESSION_IND:
        case RRC_CLOSE_SESSION_CNF:
                        
         /* -----------------------------------------------------
          ** Lower layer failure - the procedure shall be aborted.
          ** ----------------------------------------------------- */
          gmm_set_state( GMM_REGISTERED );
          mm_stop_timer( TIMER_T3317 );
    
          if (gmm_periodic_rau_pending)
          {
            /* ----------------------------------------------------------------------
            ** timer T3312 expired when  MS was in state GMM_SERVICE_REQUEST_INITIATED
            ** periodic routing area procedure is performed as soon as MS leaves
            ** the GMM_SERVICE_REQUEST_INITIATED substate.
            ** ---------------------------------------------------------------------*/
            gmm_update_ra_type();
            gmm_initiate_routing_area_update();
          }
          if (gmm_service_request_initiator == MS_GMM_SM)
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ" );
            gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
          }
          else if (gmm_service_request_initiator == MS_GMM_RABM)
          {
            rabm_cmd = rabm_get_cmd_buf();
    
            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
            }
    
            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_0(MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ");
    
            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
          else if (gmm_service_request_initiator == MS_CC_MM)
          {
            /* Send MMCNM_EST_REJ to CNM */
    #ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_PS);
    #else
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
    #endif
          }
    
          /* Set the Service_request_initiator to NULL after sending the response to Initiator */
          gmm_service_request_initiator = NULL;
          gmm_controls_resel_dec_mt_ps_call = FALSE;
    
          break;
        case RRC_REL_IND:
            
          /* -----------------------------------------------------
          ** Lower layer failure - the procedure shall be aborted.
          ** ----------------------------------------------------- */
          gmm_set_state( GMM_REGISTERED );
          mm_stop_timer( TIMER_T3317 );
          if (gmm_periodic_rau_pending)
          {
            /* ----------------------------------------------------------------------
            ** timer T3312 expired when  MS was in state GMM_SERVICE_REQUEST_INITIATED
            ** periodic routing area procedure is performed as soon as MS leaves
            ** the GMM_SERVICE_REQUEST_INITIATED substate.
            ** ---------------------------------------------------------------------*/
            gmm_update_ra_type();
            gmm_initiate_routing_area_update();
          }          
          if (gmm_service_request_initiator == MS_GMM_SM)
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ");
            gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
          }
          else if (gmm_service_request_initiator == MS_GMM_RABM)
          {
            rabm_cmd = rabm_get_cmd_buf();
    
            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
            }
    
            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            
            if (message->cmd.rrc_rel_ind.rel_cause == RRC_REL_CAUSE_CONGESTION)
            {
              rabm_cmd->cmd.gmm_reestablish_rej.cause = CONGESTTION;
            }
            else 
            {
              rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
            }  
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_0(MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ");
    
            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer( rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
          else if (gmm_service_request_initiator == MS_CC_MM)
          {
            /* Send MMCNM_EST_REJ to CNM */
    #ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_PS);
    #else
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
    #endif
          }
    
          /* Set the Service_request_initiator to NULL after sending the response to Initiator */
          gmm_service_request_initiator = NULL;
          gmm_controls_resel_dec_mt_ps_call = FALSE;    
          break;
          
        case RRC_ABORT_CNF:
          /* -----------------------------------------------------
          ** Lower layer failure - the procedure shall be aborted.
          ** ----------------------------------------------------- */
          gmm_set_state( GMM_REGISTERED );
          mm_stop_timer( TIMER_T3317 );
          if (gmm_periodic_rau_pending)
          {
            /* ----------------------------------------------------------------------
            ** timer T3312 expired when  MS was in state GMM_SERVICE_REQUEST_INITIATED
            ** periodic routing area procedure is performed as soon as MS leaves
            ** the GMM_SERVICE_REQUEST_INITIATED substate.
            ** ---------------------------------------------------------------------*/
            gmm_update_ra_type();
            gmm_initiate_routing_area_update();
          }
          if (gmm_service_request_initiator == MS_GMM_SM)
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ");
            gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
          }
          else if (gmm_service_request_initiator == MS_GMM_RABM)
          {
            rabm_cmd = rabm_get_cmd_buf();
    
            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }
    
            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
    
            if (message->cmd.rrc_abort_cnf.conn_rel_cause == RRC_REL_CAUSE_CONGESTION)
            {
              rabm_cmd->cmd.gmm_reestablish_rej.cause = CONGESTTION;
            }
            else
            {
              rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
            }
    
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ" );
    
            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
          else if (gmm_service_request_initiator == MS_CC_MM)
          {
            /* Send MMCNM_EST_REJ to CNM */
    #ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_PS);
    #else
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
    #endif
          }
    
          /* Set the Service_request_initiator to NULL after sending the response to Initiator */
          gmm_service_request_initiator = NULL;
          gmm_controls_resel_dec_mt_ps_call = FALSE;
    
          break;

        case MM_DATA_IND:
          switch (message->cmd.mm_data_ind.L3_data[1])
          {
            case GMM_SERVICE_ACCEPT:
              if (gmm_valid_service_accept_message( message, &service_accept_msg ))
              {
                gmm_send_sm_pdp_mbms_status_ind_if_needed
                (
                   &(service_accept_msg.pdp_context_status),
                   service_accept_msg.pdp_context_status_present,
                   &(service_accept_msg.mbms_context_status),
                   service_accept_msg.mbms_context_status_present
                );
                if ((gmm_state == GMM_SERVICE_REQUEST_INITIATED) &&
                    (gmm_service_request_type == GMM_DATA))
                {
                  mm_stop_timer( TIMER_T3317 );
                  gmm_set_state( GMM_REGISTERED );
                  /* -------------------------------------------------------------
                  ** This primitive is used by the GMM sublayer to inform the RABM
                  ** entity that the reception of the SERVICE REQUEST message has
                  ** been acknowledged by the network
                  ** ------------------------------------------------------------- */
                  rabm_cmd = rabm_get_cmd_buf();

                  if (rabm_cmd == NULL)
                  {
                    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                  }

                  rabm_cmd->header.message_set = MS_GMM_RABM;
                  rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_RSP;
#ifdef FEATURE_DUAL_SIM
                  rabm_cmd->cmd.gmm_reestablish_rsp.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_RSP");
                  /*Spec Ref:4.7.13.3     
                      At successful completion of a service request procedure with Service type "data", the MS shall start timer T3319*/
                  mm_start_timer( TIMER_T3319, gmm_t3319_timer_value );
                  rabm_put_cmd( rabm_cmd );
                  /*adding message to the debug buffer*/
                  mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                                 ,(sys_modem_as_id_e_type)mm_as_id
                                                );
                  gmm_send_gmmsm_establish_ind(SM_PS_SIG_CONN_AVAIL);		  
                }
                else if((gmm_state == GMM_SERVICE_REQUEST_INITIATED)&&
                        (gmm_service_request_initiator == MS_GMM_SM))
                {
                  mm_stop_timer( TIMER_T3317 );
                  gmm_set_state( GMM_REGISTERED );

                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_CNF" );
                  gmm_send_gmmsm_establish_cnf(FALSE);

                  gmm_service_request_initiator = NULL;
                }
                else if((gmm_state == GMM_SERVICE_REQUEST_INITIATED)&&
                        (gmm_service_request_initiator == MS_CC_MM))
                {
                  mm_stop_timer( TIMER_T3317 );
                  gmm_set_state( GMM_REGISTERED );
                  
                  /* Send MMCNM_EST_CNF to CNM */
                  mm_confirm_mm_connection(MS_CC_MM_PS) ;
                  gmm_service_request_initiator = NULL;                  
                }
                auth_reset_cache_memory(FALSE, NO_TIMER_EXPIRED, RRC_PS_DOMAIN_CN_ID);    
                if (gmm_periodic_rau_pending)
                {
                  /* ----------------------------------------------------------------------
                  ** timer T3312 expired when  MS was in state GMM_SERVICE_REQUEST_INITIATED
                  ** periodic routing area procedure is performed as soon as MS leaves
                  ** the GMM_SERVICE_REQUEST_INITIATED substate.
                  ** ---------------------------------------------------------------------*/
                  gmm_update_ra_type();
                  gmm_initiate_routing_area_update();
                }
              } /* end if (gmm_valid_service_accept_message( message ) */
              else
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Invalid SERVICE ACCEPT message");
              }
              break;

            case GMM_SERVICE_REJECT:
              if (gmm_valid_service_reject_message( message, &service_reject_msg ))
              {
                if (gmm_state == GMM_SERVICE_REQUEST_INITIATED)
                {
                  /* --------------------------------------------------------------
                  ** An MS that receives a SERVICE REJECT message stops timer T3317
                  ** -------------------------------------------------------------- */
                  mm_stop_timer( TIMER_T3317 );

                  if (gmm_service_request_initiator == MS_GMM_SM)
                  {
                    MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ");
                    gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(service_reject_msg.gmm_cause));
                  }
                  else if (gmm_service_request_initiator == MS_GMM_RABM)
                  {
                    rabm_cmd = rabm_get_cmd_buf();

                    if (rabm_cmd == NULL)
                    {
                      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                    }

                    rabm_cmd->header.message_set = MS_GMM_RABM;
                    rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
                    rabm_cmd->cmd.gmm_reestablish_rej.cause = service_reject_msg.gmm_cause;
#ifdef FEATURE_DUAL_SIM
                    rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                    MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ");

                    rabm_put_cmd( rabm_cmd );
                    /*adding message to the debug buffer*/
                    mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                                   ,(sys_modem_as_id_e_type)mm_as_id
                                                  );
                  }
                  else if (gmm_service_request_initiator == MS_CC_MM)
                  {
                    /* Send MMCNM_EST_REJ to CNM */
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                    mm_send_mmcnm_est_rej(OTA_REJECT_CAUSE,
                                                service_reject_msg.gmm_cause, MMCC_SMS_PS) ;
#else
                    mm_send_mmcnm_est_rej(OTA_REJECT_CAUSE,
                                                service_reject_msg.gmm_cause) ;
#endif
                  }
                  else if (gmm_service_request_type == GMM_PAGING_RESPONSE)
                  {
                     gmm_send_gmmsm_establish_ind(mm_convert_gmm_to_sm_cause(service_reject_msg.gmm_cause));
                     MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_IND");
                  }

                  /* Set the Service_request_initiator to NULL after sending the response to Initiator */
                  gmm_service_request_initiator = NULL;

#if defined(FEATURE_LTE)
                  /* If LTE is present, update EMM state and parameters.
                     Note: emm_multimode_complete_update must be also called
                     after GMM/MM processing to wrap up EMM update */
                  if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
                  {
                    emm_update_multimode_sr_reject((lte_nas_emm_cause_type)service_reject_msg.gmm_cause);
                  }
#endif

#ifdef FEATURE_NAS_REL10
                  if(service_reject_msg.t3346_value_present)
                  {
                   gmm_t3346_backoff_timer_value = gmm_convert_timer_value_ie_to_usecs(service_reject_msg.t3346_value);
                  }
#endif

                  /* ---------------------------------------------------------
                  ** The MS shall then take different actions depending on the
                  ** received reject cause value:
                  ** --------------------------------------------------------- */
                  switch (service_reject_msg.gmm_cause)
                  {
                    case ILLEGAL_MS:
                    case ILLEGAL_ME:
#ifdef FEATURE_NAS_REL11
                    case GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
#endif
                      /* ----------------------------------------------------------
                      ** The MS shall set the GPRS update status to GU3 ROAMING NOT
                      ** ALLOWED (and shall store it according to section 4.1.3.2)
                      ** and enter the state GMM-DEREGISTERED.  Furthermore, it
                      ** shall delete any P-TMSI, P-TMSI signature, RAI and GPRS
                      ** ciphering key sequence number and shall consider the SIM
                      ** as invalid for GPRS services until switching off or the
                      ** SIM is removed.
                      ** ---------------------------------------------------------- */
                      gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;

                      gmm_delete_ps_location_information();
                      gmm_delete_gprs_cksn();
                      mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                          gmm_stored_ptmsi_signature,
                                                          gmm_stored_rai,
                                                          gmm_update_status );

                      gmm_set_state( GMM_DEREGISTERED );
                      gmm_set_substate( GMM_LIMITED_SERVICE );
                      
                      sm_cmd = sm_get_cmd_buf();

                      if (sm_cmd == NULL)
                      {
                        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                      }

                      sm_cmd->header.message_set = MS_GMM_SM;
                      sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                      sm_cmd->cmd.gmm_gprs_rel_ind.cause = 
                                        mm_convert_gmm_to_sm_cause(service_reject_msg.gmm_cause);
#ifdef FEATURE_DUAL_SIM
                      sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

                      sm_put_cmd( sm_cmd );
                      
                      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
                      {
                        /* ---------------------------------------------------------
                        ** If the MS is IMSI attached, the MS shall in addition set
                        ** the update status to U3 ROAMING NOT ALLOWED, shall delete
                        ** any TMSI, LAI and ciphering key sequence number.
                        ** --------------------------------------------------------- */
                        mm_delete_lu_status( ROAMING_NOT_ALLOWED );

                        /* --------------------------------------------------------
                        ** The SIM shall be considered as invalid also for non-GPRS
                        ** services until switching off or the SIM is removed.
                        ** -------------------------------------------------------- */
                        gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_PS_INVALID,
                                                         SYS_SRV_DOMAIN_NO_SRV,
                                                         TRUE,
                                                         SYS_SRV_STATUS_LIMITED,
                                                         TRUE );
                        mmsim_mark_sim_as_illegal();

                        /* --------------------------------------------------
                        ** The MM state will transition back to IDLE when the
                        ** RR(C) connection is released
                        ** -------------------------------------------------- */
                      }
                      else
                      {
                        gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_PS_INVALID,
                                                         SYS_SRV_DOMAIN_NO_SRV,
                                                         TRUE,
                                                         SYS_SRV_STATUS_LIMITED,
                                                         TRUE );
                      }

                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mmr_service_ind.cause = mm_convert_to_reg_cause(
                                                service_reject_msg.gmm_cause );

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, No Service");
                      mm_send_mmr_service_ind( &mmr_service_ind );
                      break;

                    case GPRS_SERVICES_NOT_ALLOWED:
                      /* ----------------------------------------------------------
                      ** The MS shall set the GPRS update status to GU3 ROAMING NOT
                      ** ALLOWED (and shall store it according to section 4.1.3.2)
                      ** and enter the state GMM-DEREGISTERED.  Furthermore, it
                      ** shall delete any P-TMSI, P-TMSI signature, RAI and GPRS
                      ** ciphering key sequence number and shall consider the SIM
                      ** as invalid for GPRS services until switching off or the
                      ** SIM is removed.
                      ** ---------------------------------------------------------- */
                      gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;

                      gmm_delete_ps_location_information();
                      gmm_delete_gprs_cksn();
                      mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                          gmm_stored_ptmsi_signature,
                                                          gmm_stored_rai,
                                                          gmm_update_status );

                      gmm_set_state( GMM_DEREGISTERED );
                      gmm_set_substate( GMM_LIMITED_SERVICE );

                      sm_cmd = sm_get_cmd_buf();

                      if (sm_cmd == NULL)
                      {
                        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                      }

                      sm_cmd->header.message_set = MS_GMM_SM;
                      sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                      sm_cmd->cmd.gmm_gprs_rel_ind.cause = 
                                   mm_convert_gmm_to_sm_cause(service_reject_msg.gmm_cause);
#ifdef FEATURE_DUAL_SIM
                      sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

                      sm_put_cmd( sm_cmd );

                      if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                          ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)||
                           (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY)))
                      {
#ifdef FEATURE_NAS_REL10
                          if ((!(( mm_location_information.location_update_status == UPDATED ) &&
                                 ( memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0))) 
#ifdef FEATURE_LTE 
                                 ||((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI))

#endif
                              )
                             {
                                 mm_substate_control( MM_STD_UPDATE_ATTEMPTED );
                                 if (mm_state == MM_IDLE)
                                 {
                                   mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                                 }
                                 else
                                 {
                                   mm_pending_lu_request = NORMAL_LU;
                                 }
                             }
                             else
                             {
                                  mm_substate_control(MM_STD_NORMAL_SERVICE);
                                  if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                                  {
                                     if(mm_state == MM_IDLE)
                                     {
                                       mm_initiate_lu(MM_LU_CAUSE_IMSI_ATTACH);
                                     }
                                     else
                                     {
                                       mm_pending_lu_request = IMSI_ATTACH;
                                     }
                                  }
                                  else
                                  {
                                    if(mm_is_per_ms_t3212_present)
                                    {
                                        if ((mm_stored_per_ms_t3212_value != 0 ) &&
                                           (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                                        {
                                          mm_start_timer( TIMER_T3212,
                                          mm_stored_per_ms_t3212_value);
                                        }
                                     }
                                     else if ((mm_system_information.T3212_timer_value != 0 ) &&
                                               (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                                     {
                                        mm_start_timer( TIMER_T3212,
                                        mm_system_information.T3212_timer_value );
                                     }
                                   }
                              }
#else
                              if ((mm_system_information.T3212_timer_value != 0 ) &&
                                   (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                              {
                                 mm_start_timer( TIMER_T3212,
                                 mm_system_information.T3212_timer_value );
                              }
#endif

                      }
                      
                      if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                         (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                         (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
                      {
                        gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_PS_INVALID,
                                                         SYS_SRV_DOMAIN_CS_ONLY,
                                                         FALSE,
                                                         SYS_SRV_STATUS_SRV,
                                                         TRUE );

                      }
                      else
                      {
                        gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_PS_INVALID,
                                                         SYS_SRV_DOMAIN_NO_SRV,
                                                         FALSE,
                                                         SYS_SRV_STATUS_LIMITED,
                                                         TRUE );
                      }
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mmr_service_ind.cause = mm_convert_to_reg_cause(
                                                service_reject_msg.gmm_cause );

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, No Service");
                      mm_send_mmr_service_ind( &mmr_service_ind );
                      break;

                    case MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK:
                      /* ----------------------------------------------------------
                      ** The MS shall set the GPRS update status to GU2 NOT UPDATED
                      ** (and shall store it according to section 4.1.3.2), enter
                      ** the state GMM-DEREGISTERED, and shall delete any P-TMSI,
                      ** P-TMSI signature, RAI and GPRS ciphering key sequence
                      ** number.
                      ** ---------------------------------------------------------- */
                      gmm_update_status = GMM_GU2_NOT_UPDATED;
                      gmm_delete_ps_location_information();
                      gmm_delete_gprs_cksn();
                      mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                          gmm_stored_ptmsi_signature,
                                                          gmm_stored_rai,
                                                          gmm_update_status );

                      gmm_set_state( GMM_DEREGISTERED );

                      sm_cmd = sm_get_cmd_buf();

                      if (sm_cmd == NULL)
                      {
                        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                      }

                      sm_cmd->header.message_set = MS_GMM_SM;
                      sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                      sm_cmd->cmd.gmm_gprs_rel_ind.cause = 
                                 mm_convert_gmm_to_sm_cause(service_reject_msg.gmm_cause);
#ifdef FEATURE_DUAL_SIM
                      sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

                      sm_put_cmd( sm_cmd );
                      
                      if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                         (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                         (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
                      {
                        gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                         SYS_SRV_DOMAIN_CS_ONLY,
                                                         FALSE,
                                                         SYS_SRV_STATUS_SRV,
                                                         TRUE );
                      }
                      else
                      {
                        gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                         SYS_SRV_DOMAIN_NO_SRV,
                                                         FALSE,
                                                         SYS_SRV_STATUS_LIMITED,
                                                         TRUE );
                      }
                      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                                service_reject_msg.gmm_cause,
                                                mm_serving_plmn.info.plmn,
                                                mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                                ,mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                                ,mm_serving_plmn.info.active_rat
                       );

                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service");
                      mm_send_mmr_service_ind( &mmr_service_ind );

                      gmm_attach_request_on_network_reject = TRUE;
                      /* --------------------------------------------------------
                      ** Subsequently, the MS may automatically initiate the GPRS
                      ** attach procedure.
                      ** -------------------------------------------------------- */
                      gmm_set_substate( GMM_NORMAL_SERVICE );
                      break;

                    case IMPLICITLY_DETACHED:
                      /* -----------------------------------------------------------
                      ** The MS shall change to state GMM-DEREGISTERED.NORMAL-
                      ** SERVICE.  The MS shall then perform a new attach procedure.
                      ** ----------------------------------------------------------- */
                      gmm_set_state( GMM_DEREGISTERED );

                      sm_cmd = sm_get_cmd_buf();

                      if (sm_cmd == NULL)
                      {
                        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                      }

                      sm_cmd->header.message_set = MS_GMM_SM;
                      sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                      sm_cmd->cmd.gmm_gprs_rel_ind.cause =
                              mm_convert_gmm_to_sm_cause(service_reject_msg.gmm_cause);
#ifdef FEATURE_DUAL_SIM
                      sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

                      sm_put_cmd( sm_cmd );
                      
                      if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                         (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                         (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
                      {
                        gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                         SYS_SRV_DOMAIN_CS_ONLY,
                                                         FALSE,
                                                         SYS_SRV_STATUS_SRV,
                                                         TRUE );
                      }
                      else
                      {
                        gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                         SYS_SRV_DOMAIN_NO_SRV,
                                                         FALSE,
                                                         SYS_SRV_STATUS_LIMITED,
                                                         TRUE );
                      }
                      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                         service_reject_msg.gmm_cause,
                                         mm_serving_plmn.info.plmn,
                                         mm_serving_plmn.info.lac
                         #ifdef FEATURE_FEMTO_CSG
                                         ,mm_serving_plmn.info.csg_info.csg_id
                         #endif 
                                         ,mm_serving_plmn.info.active_rat
                                           );
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service" );
                      mm_send_mmr_service_ind( &mmr_service_ind );

                      gmm_attach_request_on_network_reject = TRUE;
                      gmm_set_substate( GMM_NORMAL_SERVICE );
                      break;


                    case NO_SUITABLE_CELLS_IN_LA:
                    case NATIONAL_ROAMING_NOT_ALLOWED:
                      /* ---------------------------------------------------------
                      ** The MS shall set GPRS update status to GU3 Roaming not
                      ** allowed and enter the GMM-REGISTERED state with
                      ** LIMITED-SERVICE. Add the current LAI to national the list
                      ** of Forbidden Location Areas.
                      ** --------------------------------------------------------- */

                      gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
                      gmm_set_state( GMM_REGISTERED );
                      gmm_set_substate( GMM_LIMITED_SERVICE );
                      /**************************************************************
                      *24.008 4.7.1.9 The MS shall start the timer T3340  if the MS *
                      * receives any of the reject cause values #11,#12,#13 or #15  *
                      **************************************************************/
                      mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);

                      mm_add_forbidden_national_roaming( &gmm_current_lai );

                      /* If Reject cause is NO_SUITABLE_CELLS_IN_LA (15) add LAI 
                      ** in LAI reject list 
                      */
                      if(service_reject_msg.gmm_cause == NO_SUITABLE_CELLS_IN_LA)
                      {
                        mm_add_lai_in_reject_list(&gmm_current_lai);
                      }
                      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
                      {
                        /* ---------------------------------------------------------
                        ** If the MS is IMSI attached, the MS shall in addition set
                        ** the update status to U3 ROAMING NOT ALLOWED, shall reset
                        ** the location updating counter.
                        ** --------------------------------------------------------- */
                        mm_location_information.location_update_status = ROAMING_NOT_ALLOWED;

                        mm_clear_lu_attempt_counter( );

                        /* --------------------------------
                        ** Copy location information to SIM
                        ** -------------------------------- */
                        mmsim_write_loci();

                        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                        SYS_SRV_DOMAIN_NO_SRV,
                                                        TRUE,
                                                        SYS_SRV_STATUS_LIMITED,
                                                        TRUE );

                        /* --------------------------------------------------
                        ** The MM state will transition back to IDLE when the
                        ** RR(C) connection is released
                        ** -------------------------------------------------- */

                      }

                      /* ---------------------------------------------------------
                      ** Send the appropriate reason in MMR SERVICE IND.
                      ** If  #13 was received, REG shall request PLMN
                      ** selection instead of a cell selection as done for # 15.
                      ** --------------------------------------------------------- */
                      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                       SYS_SRV_DOMAIN_NO_SRV,
                                                       FALSE,
                                                       SYS_SRV_STATUS_LIMITED,
                                                       TRUE );

                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mmr_service_ind.cause = mm_convert_to_reg_cause(
                                                service_reject_msg.gmm_cause );

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service");
                      mm_send_mmr_service_ind( &mmr_service_ind );
                      break;


                    case PLMN_NOT_ALLOWED:
                    case LA_NOT_ALLOWED:
                      /* ---------------------------------------------------------
                      ** The MS shall delete any RAI, P-TMSI, P-TMSI signature and
                      ** GPRS ciphering key sequence number, shall set the GPRS
                      ** update status to GU3 ROAMING NOT ALLOWED and enter the
                      ** state GMM-DEREGISTERED.
                      ** --------------------------------------------------------- */
                      if (service_reject_msg.gmm_cause == PLMN_NOT_ALLOWED)
                      {
                        gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;

                        mm_add_plmn_to_fplmn_list( mm_serving_plmn.info.plmn);
                      }
                      else
                      {
                        gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
                      }

                      gmm_delete_ps_location_information();
                      gmm_delete_gprs_cksn();
                      mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                          gmm_stored_ptmsi_signature,
                                                          gmm_stored_rai,
                                                          gmm_update_status );

                      /* ---------------------------------------------------------
                      ** The MS shall store the LAI or the PLMN identity in the
                      ** appropriate forbidden list, i.e., in the "forbidden PLMN
                      ** list" for cause #11, in the list of "forbidden location
                      ** areas for regional provision of service" for cause #12 or
                      ** in the list of "forbidden location areas for roaming" for
                      ** cause #13.  If #11 #13 or #15 was received, the MS shall
                      ** perform a PLMN selection instead of a cell selection.
                      ** --------------------------------------------------------- */
                      if (service_reject_msg.gmm_cause == LA_NOT_ALLOWED)
                      {
                        mm_add_forbidden_regional_roaming( &gmm_current_lai );
                      }
                      else if ((service_reject_msg.gmm_cause ==
                                 NATIONAL_ROAMING_NOT_ALLOWED) ||
                               (service_reject_msg.gmm_cause == NO_SUITABLE_CELLS_IN_LA))
                      {
                        mm_add_forbidden_national_roaming( &gmm_current_lai );
                      }

                      gmm_set_state( GMM_DEREGISTERED );
                      gmm_set_substate( GMM_LIMITED_SERVICE );
                      /**************************************************************
                      *24.008 4.7.1.9 The MS shall start the timer T3340  if the MS *
                      * receives any of the reject cause values #11,#12,#13 or #15  *
                      **************************************************************/
                      mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);

                      sm_cmd = sm_get_cmd_buf();
                      if (sm_cmd == NULL)
                      {
                        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                      }
                      sm_cmd->header.message_set = MS_GMM_SM;
                      sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                      sm_cmd->cmd.gmm_gprs_rel_ind.cause = 
                            mm_convert_gmm_to_sm_cause(service_reject_msg.gmm_cause);
#ifdef FEATURE_DUAL_SIM
                      sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

                      sm_put_cmd( sm_cmd );

                      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                       SYS_SRV_DOMAIN_NO_SRV,
                                                       FALSE,
                                                       SYS_SRV_STATUS_LIMITED,
                                                       TRUE );

                      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
                      {
                        /* ---------------------------------------------------------
                        ** If the MS is IMSI attached, the MS shall in addition set
                        ** the update status to U3 ROAMING NOT ALLOWED, shall delete
                        ** any TMSI, LAI and ciphering key sequence number and shall
                        ** reset the location update attempt counter.
                        ** --------------------------------------------------------- */
                        mm_delete_lu_status( ROAMING_NOT_ALLOWED );
                        mm_clear_lu_attempt_counter( );

                        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                        SYS_SRV_DOMAIN_NO_SRV,
                                                        TRUE,
                                                        SYS_SRV_STATUS_LIMITED,
                                                        TRUE );

                        /* --------------------------------------------------
                        ** The MM state will transition back to IDLE when the
                        ** RR(C) connection is released
                        ** -------------------------------------------------- */
                      }

                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mmr_service_ind.cause = mm_convert_to_reg_cause(
                                                service_reject_msg.gmm_cause );

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service");
                      mm_send_mmr_service_ind( &mmr_service_ind );
                      break;

                    case NO_PDP_CONTEXT_ACTIVATED:
                      /* -----------------------------------------------------------
                      ** The MS shall deactivate locally all active PDP contexts and
                      ** the MS shall enter the state GMM_REGISTERED.NORMAL_SERVICE.
                      ** ----------------------------------------------------------- */
                      gmm_set_state( GMM_REGISTERED );
                      break;
#ifdef FEATURE_NAS_REL10
                    case CONGESTTION:

                      if(gmm_t3346_backoff_timer_value == 0)
                      {
                         /* ----------------------------------
                                            ** All other causes - abort procedure
                                            ** ---------------------------------- */
                         gmm_set_state( GMM_REGISTERED );
                      }
                      else
                      {
                         /**-----------------------------------------------------------------------------
                                            **Cause #22- The MS shall abort the service request procedure ,T3346 started and enter state GMM-REGISTERED.
                                            **(24.008 section 4.7.13.4) ------------------------------------------------*/
                         gmm_set_state( GMM_REGISTERED );
                         mm_stop_timer( TIMER_T3346 );
                         gmm_is_congestion_due_to_rau = FALSE;
                    
                         mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                                   service_reject_msg.gmm_cause,
                                                   mm_serving_plmn.info.plmn,
                                                   mm_serving_plmn.info.lac
                         #ifdef FEATURE_FEMTO_CSG
                                                  ,mm_serving_plmn.info.csg_info.csg_id
                         #endif 
                                                  ,mm_serving_plmn.info.active_rat
                                                  );

                             /**--------------------------------------------------------------------------------
                       **If the SERVICE REJECT message is integrity protected, the MS shall start timer T3346
                       **with the value provided in the T3346 value IE else start the timer T3346 with a random value in the default range of 15-30 mins. 
                       **(24.008 section 4.7.13.4) ------------------------------------------------*/
                       if (gmm_integrity_protection_activated == TRUE)
                       {
                         mm_start_timer(TIMER_T3346,gmm_t3346_backoff_timer_value);
                       }
                       else
                       {
                        gmm_t3346_backoff_timer_value = ((15 + mm_get_random_value(15)) * 60 * 1000);
                        mm_start_timer(TIMER_T3346,gmm_t3346_backoff_timer_value);
                       }
 
                       /*saves congestion plmn associated with the timer*/
                       memscpy((void *)&gmm_3gpp_t3346_plmn_id, sizeof(sys_plmn_id_s_type),(void *)&(mm_serving_plmn.info.plmn), sizeof(sys_plmn_id_s_type));
                     
                       gmm_t3346_backoff_timer_value = 0;
                       if (gmm_service_request_type == GMM_SIGNALLING)
                       {
                         MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ");
                         gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(service_reject_msg.gmm_cause));
                       }
                       else if (gmm_service_request_type == GMM_DATA)
                       {
                         rabm_cmd = rabm_get_cmd_buf();

                         if (rabm_cmd == NULL)
                         {
                           MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                         }

                         rabm_cmd->header.message_set = MS_GMM_RABM;
                         rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
                         rabm_cmd->cmd.gmm_reestablish_rej.cause = service_reject_msg.gmm_cause;
#ifdef FEATURE_DUAL_SIM
                         rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                         MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ");

                         rabm_put_cmd( rabm_cmd );
                         /*adding message to the debug buffer*/
                         mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FALSE
                                                      ,(sys_modem_as_id_e_type)mm_as_id
                                                     );
                       }

                       mmr_service_ind.service_state = mm_serving_plmn.info;
                       mmr_service_ind.cause         = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                       mm_send_mmr_service_ind( &mmr_service_ind );

                      }

                      break;
#endif

#ifdef FEATURE_FEMTO_CSG
                    case CSG_NOT_AUTHORIZED:
                      if(mm_serving_csg_id != SYS_CSG_ID_INVALID)
                      {
                         /* ---------------------------------------------------------
                         ** The MS shall set GPRS update status to GU3 Roaming not
                         ** allowed and enter the GMM-REGISTERED state with
                         ** LIMITED-SERVICE. Add the current LAI to national the list
                         ** of Forbidden Location Areas.
                         ** --------------------------------------------------------- */
   
                         gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
                         gmm_set_state( GMM_REGISTERED );
                         gmm_set_substate( GMM_LIMITED_SERVICE );
                         /**************************************************************
                         *24.008 4.7.1.9 The MS shall start the timer T3340  if the MS *
                         * receives any of the reject cause values #25  *
                         **************************************************************/
                         mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);
   
                         mm_delete_csg_from_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info);
   
                         /* ---------------------------------------------------------
                         ** Send the appropriate reason in MMR SERVICE IND.
                         ** If  #13 was received, REG shall request PLMN
                         ** selection instead of a cell selection as done for # 15.
                         ** --------------------------------------------------------- */
                         gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                          SYS_SRV_DOMAIN_NO_SRV,
                                                          FALSE,
                                                          SYS_SRV_STATUS_LIMITED,
                                                          TRUE );
   
                         mmr_service_ind.service_state = mm_serving_plmn.info;
                         mmr_service_ind.cause = mm_convert_to_reg_cause(
                                                   service_reject_msg.gmm_cause );
   
                         MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service");
                         mm_send_mmr_service_ind( &mmr_service_ind );
                      }
                      else
                      {
                        gmm_set_state( GMM_REGISTERED );
                      }
                      break;
#endif 

                    default:
                      /* ----------------------------------
                      ** All other causes - abort procedure
                      ** ---------------------------------- */
                      gmm_set_state( GMM_REGISTERED );
                      break;
                  } /* end switch (service_reject_msg.gmm_cause) */
#if defined(FEATURE_LTE)
                  if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
                  {
                    emm_multimode_complete_update();
                  }
#endif
                } /* end if (gmm_state == GMM_SERVICE_REQUEST_INITIATED) */
                else
                {
                if (service_reject_msg.gmm_cause == NO_PDP_CONTEXT_ACTIVATED)
                  {
                    if (gmm_service_request_type == GMM_SIGNALLING)
                    {
                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ");
                      gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(service_reject_msg.gmm_cause));
                    }
                    else if (gmm_service_request_type == GMM_DATA)
                    {
                      rabm_cmd = rabm_get_cmd_buf();

                      if (rabm_cmd == NULL)
                      {
                        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                      }

                      rabm_cmd->header.message_set = MS_GMM_RABM;
                      rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
                      rabm_cmd->cmd.gmm_reestablish_rej.cause = service_reject_msg.gmm_cause;
#ifdef FEATURE_DUAL_SIM
                      rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ" );

                      rabm_put_cmd( rabm_cmd );
                      /*adding message to the debug buffer*/
                      mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                                     ,(sys_modem_as_id_e_type)mm_as_id
                                                    );
                    }
                    else if (gmm_service_request_type == GMM_PAGING_RESPONSE)
                    {
                       gmm_send_gmmsm_establish_ind(mm_convert_gmm_to_sm_cause(service_reject_msg.gmm_cause));
                       MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_IND");
                    }
                  }
#ifdef FEATURE_FEMTO_CSG
                  else if( service_reject_msg.gmm_cause == CSG_NOT_AUTHORIZED)
                  {
                    if(mm_serving_csg_id != SYS_CSG_ID_INVALID)
                    {
                       /* ---------------------------------------------------------
                       ** The MS shall set GPRS update status to GU3 Roaming not
                       ** allowed and enter the GMM-REGISTERED state with
                       ** LIMITED-SERVICE. Add the current LAI to national the list
                       ** of Forbidden Location Areas.
                       ** --------------------------------------------------------- */
 
                       gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
                       gmm_set_state( GMM_REGISTERED );
                       gmm_set_substate( GMM_LIMITED_SERVICE );
                       /**************************************************************
                       *24.008 4.7.1.9 The MS shall start the timer T3340  if the MS *
                       * receives any of the reject cause values #25  *
                       **************************************************************/
                       mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);
 
                       mm_delete_csg_from_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info);
 
                       /* ---------------------------------------------------------
                       ** Send the appropriate reason in MMR SERVICE IND.
                       ** If  #13 was received, REG shall request PLMN
                       ** selection instead of a cell selection as done for # 15.
                       ** --------------------------------------------------------- */
                       gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                        SYS_SRV_DOMAIN_NO_SRV,
                                                        FALSE,
                                                        SYS_SRV_STATUS_LIMITED,
                                                        TRUE );
 
                       mmr_service_ind.service_state = mm_serving_plmn.info;
                       mmr_service_ind.cause = mm_convert_to_reg_cause(
                                                 service_reject_msg.gmm_cause );
 
                       MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service");
                       mm_send_mmr_service_ind( &mmr_service_ind );
                    }
                  }
#endif

                else
                {
                  MSG_HIGH_DS_0( MM_SUB, "=MM= SERVICE REJECT message in wrong state" );
                  gmm_send_gmm_status_message(
                    MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE );
                }
                }
                auth_reset_cache_memory(FALSE, NO_TIMER_EXPIRED, RRC_PS_DOMAIN_CN_ID);

                gmm_controls_resel_dec_mt_ps_call = FALSE;

              } /* end if (gmm_valid_service_reject_message() */
              else
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Invalid SERVICE REJECT message");
              }
              break;

            default:
              MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message type %d",
                           message->cmd.mm_data_ind.L3_data[1], 0,0 );
          } /* end switch (gmm_message_type) */
          break;

        case RRC_ABORT_IND:
          /* -----------------------------------------------------
          ** Lower layer failure - the procedure shall be aborted.
          ** ----------------------------------------------------- */
          gmm_set_state( GMM_REGISTERED );
          mm_stop_timer( TIMER_T3317 );
          if((message->cmd.rrc_abort_ind.abort_cause == RRC_AC_NOT_ALLOWED_CS_DOMAIN )||
             (message->cmd.rrc_abort_ind.abort_cause == RRC_AC_NOT_ALLOWED_PS_DOMAIN ))
          {
            gmm_update_cell_status(message->cmd.rrc_abort_ind.abort_cause);
          }
          if (gmm_periodic_rau_pending)
          { 
            /* ----------------------------------------------------------------------
            ** timer T3312 expired when  MS was in state GMM_SERVICE_REQUEST_INITIATED
            ** periodic routing area procedure is performed as soon as MS leaves
            ** the GMM_SERVICE_REQUEST_INITIATED substate.
            ** ---------------------------------------------------------------------*/
            gmm_update_ra_type();
            gmm_initiate_routing_area_update();
          }


          if (gmm_service_request_initiator == MS_GMM_SM)
          {
            sm_cm_status_T local_cause = PPD_UNKNOWN_REASON;
            if (message->cmd.rrc_abort_ind.abort_cause == RRC_MAX_ACCESS_FAILURE)
            {
              local_cause = PDP_LOWERLAYER_ERROR;
            }
#ifdef FEATURE_DUAL_SIM
            else if (message->cmd.rrc_abort_ind.abort_cause == RRC_NO_RESOURCE_AVAILABLE)
            {
              local_cause = SM_NO_RADIO_AVAILABLE;
            }
#endif
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ");
            gmm_send_gmmsm_establish_rej(local_cause);
          }
          else if (gmm_service_request_initiator == MS_GMM_RABM)
          {
            rabm_cmd = rabm_get_cmd_buf();

            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ");

            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
          else if (gmm_service_request_initiator == MS_CC_MM)
          {
            /* Send MMCNM_EST_REJ to CNM */
#ifdef FEATURE_DUAL_SIM
            if (message->cmd.rrc_abort_ind.abort_cause == RRC_NO_RESOURCE_AVAILABLE)
            {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
              mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_ABORT_RADIO_UNAVAILABLE, MMCC_SMS_PS);
#else
              mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_ABORT_RADIO_UNAVAILABLE);
#endif
            }
            else
#endif
            {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
              mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_PS);
#else
              mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
            }

          }

          /* Set the Service_request_initiator to NULL after sending the response to Initiator */
          gmm_service_request_initiator = NULL;
          gmm_controls_resel_dec_mt_ps_call = FALSE;

          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected RRC cmd %d", message->cmd.hdr.message_id, 0,0 );
      } /* end switch (message->cmd.hdr.message_id) */
      break;

    case MS_MM_RR:
      switch (message->cmd.hdr.message_id)
      {
        case RR_ACTIVATION_IND:
          /* -----------------------------------------------------
          ** Lower layer failure - the procedure shall be aborted.
          ** ----------------------------------------------------- */
          gmm_set_state( GMM_REGISTERED );
          mm_stop_timer( TIMER_T3317 );

          if (gmm_service_request_initiator == MS_GMM_SM)
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ");
            gmm_send_gmmsm_establish_rej(PPD_UNKNOWN_REASON);
          }
          else if (gmm_service_request_initiator == MS_GMM_RABM)
          {
            rabm_cmd = rabm_get_cmd_buf();

            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ");

            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
          else if (gmm_service_request_initiator == MS_CC_MM)
          {
            /* Send MMCNM_EST_REJ to CNM */
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_PS);
#else
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
          }

          /* Set the Service_request_initiator to NULL after sending the response to Initiator */
          gmm_service_request_initiator = NULL;
          gmm_controls_resel_dec_mt_ps_call = FALSE;

          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected RR cmd %d", message->cmd.hdr.message_id, 0,0 );
      } /* end switch (message->cmd.hdr.message_id) */
      break;

    case MS_TIMER:
      switch (message->cmd.timer_expiry.timer_id)
      {
        case TIMER_T3317:
          /* ---------------------------------------
          ** The MS shall enter GMM-REGISTERED state
          ** --------------------------------------- */
          gmm_set_state( GMM_REGISTERED );

          if (pmm_mode == PMM_IDLE)
          {
#ifdef FEATURE_WCDMA
            if (is_umts_mode())
            {
              rrc_cmd = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );

              if (rrc_cmd == NULL)
              {
                MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
              }

              rrc_cmd->cmd_hdr.cmd_id = RRC_CLOSE_SESSION_REQ;
              rrc_cmd->cmd.close_sess_req.cn_domain_id = RRC_PS_DOMAIN_CN_ID;

              rrc_cmd->cmd.close_sess_req.close_session_cause = RRC_OTHERS;
              MSG_HIGH_DS_0( MM_SUB, "=MM= Sending RRC_CLOSE_SESSION_REQ" );

              send_cmd_to_rrc( rrc_cmd, mm_serving_plmn.info.active_rat );
            }
#endif /* #ifdef FEATURE_WCDMA */
          }

          if (gmm_service_request_initiator == MS_GMM_SM)
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ");
            gmm_send_gmmsm_establish_rej(PPD_UNKNOWN_REASON);
          }
          else if (gmm_service_request_initiator == MS_GMM_RABM)
          {
            rabm_cmd = rabm_get_cmd_buf();

            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ");

            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
          else if (gmm_service_request_initiator == MS_CC_MM)
          {
            /* Send MMCNM_EST_REJ to CNM */
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_RRC_OPEN_SESSION_FAILURE, MMCC_SMS_PS);
#else
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_RRC_OPEN_SESSION_FAILURE);
#endif
          }

          /* Set the Service_request_initiator to NULL after sending the response to Initiator */
          gmm_service_request_initiator = NULL;
          gmm_controls_resel_dec_mt_ps_call = FALSE;
          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected timer message %d",
                      message->cmd.timer_expiry.timer_id, 0,0 );
          break;
      } /* end switch (message->cmd.timer_expiry.timer_id) */
      break;

    default:
      MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message set %d", message->cmd.hdr.message_set, 0,0 );
  } /* end switch (message->cmd.hdr.message_set) */
} /* end gmm_handle_service_request() */


/*===========================================================================

FUNCTION GMM_PROCESS_ABNORMAL_PAGE_IND

DESCRIPTION
  Function to

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_process_abnormal_page_ind( void )
{
  sm_cmd_type *sm_cmd = sm_get_cmd_buf();

  if (sm_cmd == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
  }

  sm_cmd->header.message_set = MS_GMM_SM;
  sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
  sm_cmd->cmd.gmm_gprs_rel_ind.cause = NO_GPRS_CONTEXT;
#ifdef FEATURE_DUAL_SIM
  sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

  sm_put_cmd( sm_cmd );

#ifdef FEATURE_NAS_REL10
  mm_stop_timer(TIMER_T3346);
  gmm_is_congestion_due_to_rau = FALSE;
#endif

  gmm_update_status = GMM_GU2_NOT_UPDATED;
  gmm_delete_ps_location_information();
  gmm_delete_gprs_cksn();
  mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                      gmm_stored_ptmsi_signature,
                                      gmm_stored_rai,
                                      gmm_update_status );
#ifdef FEATURE_LTE
  if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_LTE_RADIO_ACCESS)==TRUE)
  {  


      emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED, 
                                  SYS_SIM_STATE_AVAILABLE, 
                                  emm_ctrl_data_ptr, FALSE) ;
                           
  }
#endif /* FEATURE_LTE */
  gmm_set_state( GMM_DEREGISTERED );

  gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                   SYS_SRV_DOMAIN_NO_SRV,
                                   FALSE,
                                   SYS_SRV_STATUS_LIMITED,
                                   TRUE );

  /* --------------------------------------------------------------------------
  ** After performing the local detach, the MS shall then perform a GPRS attach
  ** (24.008 4.7.9.1.2)
  ** -------------------------------------------------------------------------- */
  gmm_set_substate( GMM_NORMAL_SERVICE );
} /* end gmm_process_abnormal_page_ind() */

/*===========================================================================

FUNCTION GMM_PROCESS_RRC_PAGE_IND

DESCRIPTION
  Function to

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_WCDMA
void gmm_process_rrc_page_ind( mm_cmd_type *message )
{
  rrc_establish_cause_e_type establish_cause;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ---------------------------------------------------------------------------
  ** If the MS is not GPRS attached when it receives a paging for GPRS services,
  ** the MS shall ignore the paging (24.008 4.7.9.1)
  ** --------------------------------------------------------------------------- */
  if ((gmm_state == GMM_REGISTERED) ||
      (gmm_state == GMM_DEREGISTERED_INITIATED) ||
      (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
      (gmm_state == GMM_SERVICE_REQUEST_INITIATED))
  {
    if (message->cmd.rrc_page_ind.page_ue_id == RRC_PAGE_WITH_PTMSI)
    {
      if((message->cmd.rrc_page_ind.paging_cause != RRC_PAGE_CAUSE_SMS) &&
         (mm_is_mplmn_search_in_progress() == TRUE))
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= Manual search in progress ignore PS page with PTMSI");
      }
      /* -----------------------------------------------------------------------------
      ** If the paging request for GPRS services was received during an ongoing MS
      ** initiated GMM specific procedure, then the MS shall progress the GMM specific
      ** procedure, and the network shall proceed with the GMM specific procedure.
      ** ----------------------------------------------------------------------------- */
      else if (gmm_state == GMM_REGISTERED && pmm_mode == PMM_IDLE)
      {
        /* -------------------------------------------------------------------------
        ** Upon reception of a paging indication, the MS shall respond to the paging
        ** with a SERVICE REQUEST message with service type "paging response"
        ** (24.008 4.7.9.1.1)
        ** ------------------------------------------------------------------------- */
        gmm_service_request_type = GMM_PAGING_RESPONSE;

        if (message->cmd.rrc_page_ind.paging_cause == RRC_PAGE_CAUSE_TERM_CONV_CALL)
        {
          establish_cause = RRC_EST_TERM_CONV_CALL;
        }
        else if (message->cmd.rrc_page_ind.paging_cause == RRC_PAGE_CAUSE_TERM_STREAM_CALL)
        {
          establish_cause = RRC_EST_TERM_STREAM_CALL;
        }
        else if (message->cmd.rrc_page_ind.paging_cause == RRC_PAGE_CAUSE_TERM_INTERACT_CALL)
        {
          establish_cause = RRC_EST_TERM_INTERACT_CALL;
        }
        else if (message->cmd.rrc_page_ind.paging_cause == RRC_PAGE_CAUSE_TERM_BACKGND_CALL)
        {
          establish_cause = RRC_EST_TERM_BACKGND_CALL;
        }
        else if (message->cmd.rrc_page_ind.paging_cause == RRC_PAGE_CAUSE_SMS)
        {
          establish_cause = RRC_EST_LOW_PRI_SIGNALLING;
        }
        else if (message->cmd.rrc_page_ind.paging_cause == RRC_PAGE_CAUSE_HIGH_PRI_SIGNALLING)
        {
          establish_cause = RRC_EST_TERM_HI_SIGNALLING;
        }
        else if (message->cmd.rrc_page_ind.paging_cause == RRC_PAGE_CAUSE_LOW_PRI_SIGNALLING)
        {
          establish_cause = RRC_EST_TERM_LOW_SIGNALLING;
        }
        else if (message->cmd.rrc_page_ind.paging_cause == RRC_PAGE_CAUSE_UNKNOWN)
        {
          establish_cause = RRC_EST_TERM_UNKNOWN;
        }
        else
        {
          MSG_ERROR_DS_1( MM_SUB, "=MM= Unexpected page cause %d", message->cmd.rrc_page_ind.paging_cause );
          establish_cause = RRC_EST_TERM_UNKNOWN;
        }

        if (((gmm_update_status == GMM_GU1_UPDATED) &&
             (!gmm_rai_changed(gmm_stored_rai.location_area_code, gmm_stored_rai.routing_area_code, 
                mm_serving_plmn.lac, mm_serving_plmn.rac)) &&
              (!gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))
            ||
            (gmm_rau_is_required == TRUE)
#ifdef FEATURE_NAS_REL10
            || 
            (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
#endif
           )
        {
          mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_PAGING_RSP);
          gmm_service_request_initiator = NULL;
          gmm_service_request_type      = GMM_PAGING_RESPONSE;

          if ((mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE) ||
              ((gmm_check_if_ps_cell_access_barred(ps_cell_access) == TRUE) &&
#ifdef FEATURE_DSAC              
               (( mm_system_information.dsac_present == FALSE && 
                  rrc_connection_status == MMRRC_IDLE ) ||
                ( mm_system_information.dsac_present == TRUE && 
                  ps_session_status == MMRRC_IDLE ))
#else
               (rrc_connection_status == MMRRC_IDLE) 
#endif                  
              ))
          {
            MSG_HIGH_DS_2( MM_SUB, "=MM= Service request blocked - AC %d or service_state %d",
               ps_cell_access, mm_serving_plmn.gmm_service_state );
          }
#ifdef FEATURE_NAS_REL10
          else if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
                  (gmm_update_status == GMM_GU2_NOT_UPDATED))

          {
            if((gmm_state == GMM_REGISTERED) &&
               (gmm_substate == GMM_ATTEMPTING_TO_UPDATE))
            {
              mm_stop_timer(TIMER_T3346);
              gmm_is_congestion_due_to_rau = FALSE;
              gmm_follow_on_request = TRUE;
              if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
              {
                gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
              }
              else
              {
                gmm_ra_update_type = GMM_RA_UPDATING;
              }
             /**--------------------------------------------------------------------------
                       **The normal routing area updating procedure is initiated due to the reception
                       **of the paging indication using PTMSI while T3346 is running, 
                       **the "follow-on request pending" indication shall be set to 1. 
                       **(24.008 section 4.7.5.1) --------------------------------------------------*/

               gmm_initiate_routing_area_update() ;
            }
            else
            {
              MSG_HIGH_DS_0( MM_SUB, "=MM= Service request blocked -GPRS UPDATE STATUS != GMM_GU1_UPDATED");   
            }
          }
#endif
          else
          {
#ifdef FEATURE_NAS_REL10
            mm_stop_timer(TIMER_T3346);
            gmm_is_congestion_due_to_rau = FALSE;
#endif
            gmm_initiate_service_request( establish_cause, AUTH_PS_GPRS_SECURITY_CONTEXT, NULL);
            /* As per spec[section 4.7.1.7, 4.7.9.1.1], after intersystem change, 
               If paging is received in different access network, RAU should be the response 
               to paging, hence Paging response is being dropped here which is stored in
               gmm_rat_chng_pended_msg_valid variable*/            
            if(gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
            {
              gmm_rat_chng_pended_msg_valid = FALSE;
#ifdef FEATURE_MODEM_HEAP
              if(gmm_rat_chng_pended_msg != NULL)
              {
                modem_mem_free(gmm_rat_chng_pended_msg,MODEM_MEM_CLIENT_NAS);
                gmm_rat_chng_pended_msg = NULL;
              }
#endif /*FEATURE_MODEM_HEAP*/
            } 
            else
            {
               gmm_processing_directed_signalling_reestablishment = FALSE;
            }
          }
        }
        else
        {
          MSG_HIGH_DS_0( MM_SUB, "=MM= Service request blocked -GPRS UPDATE STATUS != GMM_GU1_UPDATED");        
        }
      }
    }
    else if (message->cmd.rrc_page_ind.page_ue_id == RRC_PAGE_WITH_IMSI)
    {
      /* --------------------------------------------------------------------------
      ** Upon reception of a paging indication for GPRS services using IMSI, the MS
      ** shall locally deactivate any active PDP contexts and locally detach from
      ** GPRS.  The local detach includes deleting any RAI, P-TMSI, P-TMSI
      ** signature and GPRS ciphering key sequence number stored, setting the
      ** GPRS update status to GU2 NOT UPDATED and changing state to
      ** GMM-DEREGISTERED.
      ** -------------------------------------------------------------------------- */
      mm_send_sm_unblock_all_apn();
      gmm_process_abnormal_page_ind();
    }
    else if (message->cmd.rrc_page_ind.page_ue_id == RRC_PAGE_WITH_TMSI)
    {
      mm_send_sm_unblock_all_apn();
    }
  }
} /* end gmm_process_rrc_page_ind() */
#endif /*FEATURE_WCDMA*/

/*===========================================================================

FUNCTION GMM_CHECK_SERVICE_REJECT_FOR_INTEGRITY_PROTECTION

DESCRIPTION
  Function to perform check for integrity for service reject 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean gmm_check_service_reject_for_integrity_protection(mm_cmd_type *message)
{
  boolean integrity_check_passed = TRUE;
#ifdef FEATURE_FEMTO_CSG
  if(!gmm_integrity_protection_activated &&
       (mm_serving_csg_id != SYS_CSG_ID_INVALID) &&
       (message->cmd.mm_data_ind.L3_data[2] == CSG_NOT_AUTHORIZED))
   {
     integrity_check_passed = FALSE;
   }
#endif 
  return integrity_check_passed;
}
/*===========================================================================

FUNCTION GMM_REESTABLISH_CONNECTION

DESCRIPTION
  This function tries to reestablish connection if required and returns true 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean gmm_reestablish_connection( mm_cmd_type *message )
{
  boolean return_value = TRUE;
  byte pending_gmm_msg_id = 0;

  if (mm_ps_domain_possible_reest &&
      (!(gmm_ra_update_pending &&
         (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) &&
         (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))) &&
      ((message->cmd.hdr.message_id == (int) RRC_ABORT_CNF) ||
       (message->cmd.hdr.message_id == (int) RRC_CLOSE_SESSION_CNF) ||
       ((!(gmm_procedure_rerun_ongoing &&
        ((gmm_state == GMM_SERVICE_REQUEST_INITIATED) ||
        (gmm_state == GMM_DEREGISTERED_INITIATED)))) &&
        (((message->cmd.hdr.message_id == (int) RRC_REL_IND) &&
          (message->cmd.rrc_rel_ind.rel_cause != RRC_DIRECTED_SIGNALLING_REESTABLISHMENT)) ||
         ((message->cmd.hdr.message_id == (int) RRC_ABORT_IND) &&
          (message->cmd.rrc_abort_ind.abort_cause != (rrc_abort_cause_e_type)RRC_DIRECTED_SIGNALLING_REESTABLISHMENT)) ||
         ((message->cmd.hdr.message_set != MS_TDS_RRC) &&
          (message->cmd.hdr.message_id == (int) RRC_CLOSE_SESSION_IND))))))
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= Re-establishing after unintended abort during GMM procedure flag:%d", mm_ps_domain_possible_reest );
    if (pmm_mode == PMM_CONNECTED)
    {
      if ((gmm_state == GMM_DEREGISTERED) ||
         (gmm_state == GMM_REGISTERED_INITIATED))
      {
        pmm_mode = PMM_DETACHED;
        MSG_HIGH_DS_0(MM_SUB, "=MM= pmm_mode = PMM_DETACHED");
      }
      else
      {
        /* ---------------------------------------------------------------------
        ** In UMTS the timer T3312 is reset and started with its initial value
        ** when the MS goes from PMM-CONNECTED to PMM-IDLE mode (24.008 4.7.2.2)
        ** --------------------------------------------------------------------- */
        pmm_mode = PMM_IDLE;
        MSG_HIGH_DS_0(MM_SUB, "=MM= pmm_mode = PMM_IDLE" );
        if (gmm_t3312_timer_value != 0)
        {
          mm_start_timer( TIMER_T3312, gmm_t3312_timer_value );
        }

/*24.008:4.7.7.2 
The RAND and RES values stored in the mobile station shall be deleted and timer T3316, if running, shall be stopped:
-	if the mobile station enters PMM-IDLE mode (Iu mode only).*/
#ifdef FEATURE_NAS_REL11
        if (is_umts_mode())
        {
          auth_reset_cache_memory(FALSE,NO_TIMER_EXPIRED,RRC_PS_DOMAIN_CN_ID) ;
        }
#endif

#ifdef FEATURE_NAS_REL6
        mm_stop_timer( TIMER_T3319 );
#endif
      }

      
    }

    if (pending_gmm_msg != NULL)
    {

      pending_gmm_msg_id = (byte) (((byte *)pending_gmm_msg + sizeof(IMH_T))[1]) ;
 
      switch (pending_gmm_msg_id)
      {
        case GMM_ATTACH_REQUEST :
          mm_stop_timer( TIMER_T3310 );
          gmm_set_state(GMM_REGISTERED_INITIATED);
          break ;
        case GMM_ROUTING_AREA_UPDATE_REQUEST:
          mm_stop_timer( TIMER_T3330 );
          gmm_set_state(GMM_ROUTING_AREA_UPDATING_INITIATED);
          break ;
        case GMM_DETACH_REQUEST :
          mm_stop_timer( TIMER_T3321 );
		  mm_start_timer(TIMER_T3321,DEFAULT_TIMEOUT);
          gmm_set_state(GMM_DEREGISTERED_INITIATED);
          break ;
        case GMM_SERVICE_REQUEST:
          mm_stop_timer(TIMER_T3317);
          gmm_set_state(GMM_SERVICE_REQUEST_INITIATED);
          break ;
        default :
          MSG_HIGH_DS_1(MM_SUB, "=MM= GMM message is not supposed to be resent %d",pending_gmm_msg_id);
          break ;
      }

      mmrrcconn_send_L3_message_to_rrc(
             pending_gmm_msg, pending_gmm_msg_cause, RRC_PS_DOMAIN_CN_ID );

    }
    else
    {
      return_value = FALSE;
    }
    
    mm_ps_domain_possible_reest = FALSE;
  }
  else
  {
    return_value = FALSE;
  }

  return return_value;
}
/*===========================================================================

FUNCTION GMM_HANDLE_CONNECTION_MANAGEMENT_ACTIVATION_IND

DESCRIPTION
  This function handles the processing of the RRC_ACTIVATION_IND and               
  RR_ACTIVATION_IND (which is identical for both primitives).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_handle_connection_management_activation_ind( mm_cmd_type *message )
{
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER

  /* If WtoGCCO is in progress then don't reset these variables, because
     if CCO failed then same RRC session will be used */
  if (WtoGCCO_state == CCO_INACTIVE)
  {
    gmm_ps_signalling_connected = FALSE;
    gmm_integrity_protection_activated = FALSE;
  }
#else
  gmm_ps_signalling_connected = FALSE;
  gmm_integrity_protection_activated = FALSE;
#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */

  /* -------------------------------------------
  ** Treat as indications of lower layer failure
  ** ------------------------------------------- */
  if (gmm_state == GMM_REGISTERED_INITIATED)
  {
    gmm_handle_gprs_attach( message );
  }
  else if (gmm_state == GMM_DEREGISTERED_INITIATED)
  {
    gmm_handle_gprs_detach( message );
  }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
  else if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
  {
    gmm_handle_routing_area_update( message );
  }
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
  else if (gmm_state == GMM_SERVICE_REQUEST_INITIATED)
  {
    gmm_handle_service_request( message );
  }
  else if (gmm_state == GMM_REGISTERED && gmm_substate == GMM_IMSI_DETACH_INITIATED)
  {
    gmm_handle_gprs_detach( message );
  }

  if (mm_serv_change_pending)
  {
    mm_change_service_domain();
    mm_serv_change_pending = FALSE;
  }
  else if (mm_rat_change_pending)
  {
    mm_stop_current_rat();
  }
}


/*===========================================================================

FUNCTION GMM_HANDLE_CONNECTION_MANAGEMENT

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_handle_connection_management( mm_cmd_type *message )
{
  rabm_cmd_type             *rabm_cmd;
  mmr_service_ind_s_type    mmr_service_ind;
  uint16 length;
#ifdef FEATURE_SEGMENT_LOADING
  interface_t*              ptr_W = NULL;
#endif


#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_SEGMENT_LOADING
  ptr_W = get_wcdma_interface();
#endif
  switch (message->cmd.hdr.message_set)
  {
    case MS_CC_MM:
      switch (message->cmd.hdr.message_id)
      {
        case MMCNM_EST_REQ:
          MSG_HIGH_DS_0( MM_SUB, "=MM= Received MMCNM_EST_REQ (PS SMS requested)");

          if (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= MMCNM_EST_REJ due to no GPRS service");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej( MM_REJECT_CAUSE, MM_REJ_NO_SERVICE , MMCC_SMS_PS);
#else
            mm_send_mmcnm_est_rej( MM_REJECT_CAUSE, MM_REJ_NO_SERVICE );
#endif
          }
          else if (gmm_state == GMM_REGISTERED)
          {
#ifdef FEATURE_GSM_GPRS
            if (is_gsm_mode())
            {
              /* ----------------------------------------------------------
              ** MS (GSM only) shall not send any signalling information in
              ** GMM state GMM-REGISTERED.SUSPENDED - CNM establish requests
              ** are rejected in GMM SUSPEDNED substate.
              ** ----------------------------------------------------------*/
              if ((gmm_substate == GMM_SUSPENDED) || (gmm_is_suspended))
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMCNM_EST_REJ due to GPRS suspension" );
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE, MMCC_SMS_PS);
#else
                mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif
              }
/*24.008: 4.7.13.5  m)	Timer T3346 is running
	The MS shall not start the service request procedure unless the MS is accessing the network with access class 11 – 15*/
#ifdef FEATURE_NAS_REL10
              else if ((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
#ifdef FEATURE_NAS_REL11
                       &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
			          )
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMCNM_EST_REJ due to T3346 Running");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE, MMCC_SMS_PS);
#else
                mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
              }
#endif
              /* ----------------------------------------------------------
              ** MS (GSM only) shall not send any signalling information when RAU
              **  is required. The request is postponed so that any emergency call  
              ** will not be blocked in the while. Let CNM retry till RAU is completed.
              ** ----------------------------------------------------------*/
              else if (gmm_rau_is_required == TRUE)
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMCNM_EST_REJ due to RAU pending");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE, MMCC_SMS_PS);
#else
                mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif

                if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                    (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
                {
                  gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
                }
                else
                {
                  gmm_ra_update_type = GMM_RA_UPDATING;
                }
                
                gmm_initiate_routing_area_update();
              }
              else
              {
                /* Send MMCNM_EST_CNF to CNM */
                mm_confirm_mm_connection(MS_CC_MM_PS) ;
                gmm_service_request_initiator = NULL ;
              }
            }
            else
#endif
            {
              if (is_umts_mode())
              {
                if(((gmm_update_status == GMM_GU1_UPDATED) &&
                     (!gmm_rai_changed(gmm_stored_rai.location_area_code, gmm_stored_rai.routing_area_code, 
                        mm_serving_plmn.lac, mm_serving_plmn.rac)) &&
                      (!gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))
#ifdef FEATURE_GSM_GPRS
                    || (gmm_rau_is_required == TRUE)
#endif /*FEATURE_GSM_GPRS*/
                    )
                {
                  if (pmm_mode == PMM_IDLE)
                  {
                    mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_NONE); 
                    /* Initiate setup of the GMM connection */
                    gmm_service_request_initiator = MS_CC_MM;
                    gmm_service_request_type      = GMM_SIGNALLING;

                    /* Check AC blocking restrictions in PMM_IDLE, PS Session will not be there */
                    if ((gmm_check_if_ps_cell_access_barred(ps_cell_access) == TRUE) &&
#ifdef FEATURE_DSAC
                        (( mm_system_information.dsac_present == FALSE && 
                        ( rrc_connection_status == MMRRC_IDLE)) ||
                        ( mm_system_information.dsac_present == TRUE && 
                          ps_session_status == MMRRC_IDLE))
#else                
                        ( rrc_connection_status == MMRRC_IDLE)
#endif                          
                       )
                    {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_ACCESS_CLASS_BLOCKED, MMCC_SMS_PS);
#else
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_ACCESS_CLASS_BLOCKED);
#endif                    
                    }
/*24.008: 4.7.13.5  m)	Timer T3346 is running
	The MS shall not start the service request procedure unless the MS is accessing the network with access class 11 – 15*/
#ifdef FEATURE_NAS_REL10
                    else if ((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
#ifdef FEATURE_NAS_REL11
                              &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
                            )
                    {
                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMCNM_EST_REJ due to T3346 Running");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE, MMCC_SMS_PS);
#else
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
                    }
#endif
                    else
                    {
                      gmm_initiate_service_request( RRC_EST_LOW_PRI_SIGNALLING,
                          mm_get_current_security_context( RRC_PS_DOMAIN_CN_ID ),
                          NULL
                      ) ;
                    }      
                  }
                  else if (pmm_mode == PMM_CONNECTED)
                  {
#ifdef FEATURE_REL6_PS_FOP
                    if ( gmm_ps_fop_resp != PS_FOP_NW_GRANTED)
                    {
                      /* Send MMCNM_EST_REJ to CNM */
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_PS);
#else
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE );
#endif
                    }
/*24.008: 4.7.13.5  m)	Timer T3346 is running
	The MS shall not start the service request procedure unless the MS is accessing the network with access class 11 – 15*/
#ifdef FEATURE_NAS_REL10
                    else if ((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
#ifdef FEATURE_NAS_REL11
                             &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
                            )
                    {
                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMCNM_EST_REJ due to T3346 Running");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE, MMCC_SMS_PS);
#else
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
                    }
#endif
                    else
#endif /*FEATURE_REL6_PS_FOP*/                    
                    if((ps_session_status == MMRRC_RELEASE_IN_PROGRESS) ||
					    (rrc_connection_status == MMRRC_RELEASE_IN_PROGRESS))
                    {
                      
                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMCNM_EST_REJ since PS session status is not active");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE, MMCC_SMS_PS);
#else
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
                    }
                    else
                    { 

                      /* Send MMCNM_EST_CNF to CNM */
                      mm_confirm_mm_connection(MS_CC_MM_PS) ;
                    }
                  }
                }
                else
                {
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Service request blocked -GPRS UPDATE STATUS != GMM_GU1_UPDATED");

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                  mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, OTHER_CAUSE, MMCC_SMS_PS);
#else
                  mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, OTHER_CAUSE);
#endif
                }
              }
              else
              {
                MSG_ERROR_DS_0(MM_SUB, "=MM= Unsupported operation mode") ;
              }
            }
          }
          else
          {
            /* Send MMCNM_EST_REJ to CNM */
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_PS);
#else
            mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE );
#endif
          }

          break;

        case MMCNM_REL_REQ:
          MSG_HIGH_DS_0( MM_SUB, "=MM= Received MMCNM_REL_REQ (Releasing PS SMS required)");
          if(mm_ps_sig_conn_rel_after_ps_sms == TRUE)	  
          {
            if( (is_umts_mode() == FALSE) ||
                (sm_check_pdp_exist() == TRUE) || 
                (gmm_state == GMM_SERVICE_REQUEST_INITIATED) ||
                (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
                (gmm_state == GMM_DEREGISTERED_INITIATED) ||
                (gmm_state == GMM_REGISTERED_INITIATED) )
            {
              MSG_HIGH_DS_1( MM_SUB, "=MM= Not releasing PS conn, gmm_state = %d", gmm_state);
            }
            else
            {
              mm_start_timer(TIMER_PS_CONN_REL, DEFAULT_TIMEOUT);
            }
          }
          mm_send_mmcnm_rel_cnf_ps_sms();
          break;

        default:
          MSG_ERROR_DS_1( MM_SUB, "=MM= Unexpected SMS command %d", message->cmd.hdr.message_id);
          break;
      } /* end switch (message->cmd.hdr.message_id) */
      break;

    case MS_GMM_SM:
      switch (message->cmd.hdr.message_id)
      {
#ifdef FEATURE_GSM_GPRS
        case GMMSM_RAT_RESUME_CNF:
          {
            MSG_MED_DS_0(MM_SUB, "=MM= Received GMMSM_RAT_RESUME_CNF, process it");
            mm_handle_sm_resume_cnf ();
          }
          break ;
#endif /* #ifdef FEATURE_GSM_GPRS */

        case GMMSM_OK_TO_TUNEAWAY_IND:
          mm_send_rrc_tuneaway_allowed_ind(message->cmd.gmmsm_ok_to_tuneaway_ind.ok_to_tuneaway);
          break;

        case GMMSM_ESTABLISH_REQ:
          MSG_HIGH_DS_0( MM_SUB, "=MM= Received GMMSM_ESTABLISH_REQ");

          /* mm_serving_plmn.info.ps_data_suspend falg checking is removed
             here. Lets not reject the request based on the flag = false,
             instead go forward and check if gmm_is_rau_required
             flag is set because of inter RAT. If gmm_is_rau_required is
             set then perform RAU and then process this establish request */



          if (((gmm_state == GMM_REGISTERED) &&
              (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE))
#ifdef FEATURE_WRLF_SYSTEM_SEL
               && (mm_wcdma_rlf_state == WCDMA_RLF_IDLE)
#endif 
             )       
          {
#ifdef FEATURE_GSM_GPRS
            if (is_gsm_mode())
            {
              mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_NONE);             
              /* Check AC blocking restrictions */
              if (gmm_check_if_ps_cell_access_barred(ps_cell_access)==TRUE)
              {
                MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ due to AC blocking %d", ps_cell_access);
                gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(ACCESS_CLASS_DSAC_REJECTION));
              }
              else if (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_NO_SRV ||
                       mm_waiting_for_stop_mode_cnf == TRUE)
              {
                MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ due to No Service or mm_waiting_for_stop_mode_cnf %d",
                            mm_waiting_for_stop_mode_cnf);

                gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(ABORT_SERVICE_NOT_AVAILABLE));
              }              
              /* ----------------------------------------------------------
              ** MS (GSM only) shall not send any signalling information in
              ** GMM state GMM-REGISTERED.SUSPENDED - SM establish requests
              ** are rejected in GMM SUSPEDNED substate.
              ** ----------------------------------------------------------*/
              else if ( gmm_substate == GMM_SUSPENDED || gmm_substate == GMM_LIMITED_SERVICE || 
                          gmm_substate == GMM_ATTEMPTING_TO_UPDATE || gmm_is_suspended || mm_service_ind_blocked_due_to_cs_reg)
              {
                MSG_HIGH_DS_2( MM_SUB, 
                  "=MM= Sending GMMSM_ESTABLISH_REJ : wrong GMM substate %d or mm_service_ind_blocked_due_to_cs_reg %d", 
                  gmm_substate,mm_service_ind_blocked_due_to_cs_reg);

                gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
              }
              else if(mm_interrat_state == MM_UMTS_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS 
#ifdef FEATURE_LTE
                      ||
                      mm_interrat_state == MM_LTE_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS
#endif
 )
              {
                MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ : wrong GMM substate %d", gmm_substate );
                gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
              }
             else if(!(ds3gpp_per_subs_is_service_req_allowed_on_plmn(mm_serving_plmn.info.plmn, (sys_modem_as_id_e_type)mm_sub_id)))
             {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ : due to active DS throttling timer running");
                gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
             }
/*24.008: 4.7.13.5  m)	Timer T3346 is running
	The MS shall not start the service request procedure unless the MS is accessing the network with access class 11 – 15*/
#ifdef FEATURE_NAS_REL10
              else if ((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
#ifdef FEATURE_NAS_REL11
                        &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
                      )
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ : Due to T3346 timer running");
                gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(CONGESTTION));
              }
#endif

              /* --------------------------------------------------------------
              ** MS shall not send any signalling information when RAU is required 
              **The SM service request is rejected and let SM retry after RAU gets completed.
              ** ----------------------------------------------------------*/
              else if (gmm_rau_is_required == TRUE)
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ : Due to RAU pending" );
                gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));

                /*Initiate RAU*/
                if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                    (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
                {
                  gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
                }
                else
                {
                  gmm_ra_update_type = GMM_RA_UPDATING;
                }                

                gmm_initiate_routing_area_update();
                
              }
              else if(!gmm_xid_negotiation_completed )
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ : XID negotiation not completed" );
                gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));

              }
              else if(mm_is_mplmn_search_in_progress() == TRUE)
              {
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ : Due to manual PLMN search is in progress");
                  gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
              }
              else
              {

                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_CNF");
                gmm_send_gmmsm_establish_cnf(FALSE);
                gmm_service_request_initiator = NULL ;
              }
            }
#endif
            if (is_umts_mode())
            {
              /* ---------------------------------------------------------------------
              ** No GMM procedure except Routing Area Update shall be performed in
              ** GMM-REGISTERED.ATTEMPTING-TO-UPDATE
              ** ---------------------------------------------------------------------*/
              if ((gmm_substate == GMM_ATTEMPTING_TO_UPDATE) ||
                  (gmm_substate == GMM_LIMITED_SERVICE) ||
                  (mm_service_ind_blocked_due_to_cs_reg == TRUE))
              {
                MSG_HIGH_DS_2( MM_SUB, 
                     "=MM= Sending GMMSM_ESTABLISH_REJ : wrong GMM substate %d or mm_service_ind_blocked_due_to_cs_reg %d", 
                     gmm_substate,mm_service_ind_blocked_due_to_cs_reg);

                gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
              }
              else if (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_NO_SRV ||
                       mm_waiting_for_stop_mode_cnf == TRUE)
              {
                MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ due to No Service or mm_waiting_for_stop_mode_cnf %d ",
                            mm_waiting_for_stop_mode_cnf);

                gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(ABORT_SERVICE_NOT_AVAILABLE));
              }                            
              else if(mm_interrat_state == MM_GSM_TO_UMTS_INTERRAT_CHANGE_IN_PROGRESS 
#ifdef FEATURE_LTE
|| 
                                    mm_interrat_state == MM_LTE_TO_UMTS_INTERRAT_CHANGE_IN_PROGRESS
#endif
)
              {
                MSG_HIGH_DS( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ : IRAT inprogress %d", mm_interrat_state,0,0 );
                gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
              }
              else if (pmm_mode == PMM_CONNECTED)
              {
                /* Check AC blocking restrictions */
                mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_NONE);
                /***************************************************************
                ** Check AC blocking restrictions 
                ** If DSAC feature is being used, allow SM signalling if already
                ** RRC connection exists.
                ****************************************************************/
                if (gmm_check_if_ps_cell_access_barred(ps_cell_access) == TRUE
#ifdef FEATURE_DSAC
                    &&
                    (ps_session_status == MMRRC_IDLE)
#endif                  
                )
                {
                  MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ due to AC blocking %d", ps_cell_access);
                  gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(ACCESS_CLASS_DSAC_REJECTION));
                }
               else if(!(ds3gpp_per_subs_is_service_req_allowed_on_plmn(mm_serving_plmn.info.plmn, (sys_modem_as_id_e_type)mm_sub_id)))
               {          
                 MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ : due to active DS throttling timer running" );
                 gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
               }
#ifdef FEATURE_REL6_PS_FOP
                else if ( gmm_ps_fop_resp != PS_FOP_NW_GRANTED)
                {
                  MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ due to PS FoP Not Granted %d",
                             gmm_ps_fop_resp );
                  gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
                }
#endif/* FEATURE_REL6_PS_FOP */
/*24.008: 4.7.13.5  m)	Timer T3346 is running
	The MS shall not start the service request procedure unless the MS is accessing the network with access class 11 – 15*/
#ifdef FEATURE_NAS_REL10
                else if ((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
#ifdef FEATURE_NAS_REL11
                           &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
                        )
                {
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ : Due to T3346 timer running");
                  gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(CONGESTTION));
                }
#endif
                else if(ps_session_status == MMRRC_RELEASE_IN_PROGRESS)
                {
                  MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ as ps session is not active",
                             gmm_ps_fop_resp );
                  gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
                }
                else
                {
                           
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_CNF");
                  gmm_send_gmmsm_establish_cnf(TRUE);
#ifdef FEATURE_NAS_REL12
                  if(mm_optimised_ps_signalling_connection_release)
                  {
                    mm_stop_timer(TIMER_T3340);
                  }
#endif		  

                }
              }
              else
              {
                mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_NONE);              
                /* Check AC blocking restrictions in PMM_IDLE, PS Session will not be there */
                if ((gmm_check_if_ps_cell_access_barred(ps_cell_access) == TRUE) &&
#ifdef FEATURE_DSAC                    
                    (( mm_system_information.dsac_present == FALSE && 
                    ( rrc_connection_status == MMRRC_IDLE)) ||
                    ( mm_system_information.dsac_present == TRUE && 
                    ps_session_status == MMRRC_IDLE ))
#else
                    ( rrc_connection_status == MMRRC_IDLE)
#endif
                    )
                {
                  MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ due to AC blocking %d",ps_cell_access);
                  gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(ACCESS_CLASS_DSAC_REJECTION));
                }
                else if(!(ds3gpp_per_subs_is_service_req_allowed_on_plmn(mm_serving_plmn.info.plmn, (sys_modem_as_id_e_type)mm_sub_id)))
                {
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ : due to active DS throttling timer running" );
                  gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
                }
/*24.008: 4.7.13.5  m)	Timer T3346 is running
	The MS shall not start the service request procedure unless the MS is accessing the network with access class 11 – 15*/
#ifdef FEATURE_NAS_REL10
                else if ((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
#ifdef FEATURE_NAS_REL11
                    &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif              
                        )
                {
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ : Due to T3346 timer running");
                  gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(CONGESTTION));
                }
#endif
                else if(mm_is_mplmn_search_in_progress() == TRUE)
                {
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ : Due to manual PLMN search is in progress" );
                  gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
                }
                else if(((gmm_update_status == GMM_GU1_UPDATED) &&
                     (!gmm_rai_changed(gmm_stored_rai.location_area_code, gmm_stored_rai.routing_area_code, 
                        mm_serving_plmn.lac, mm_serving_plmn.rac)) &&
                      (!gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))
#ifdef FEATURE_GSM_GPRS
                    || (gmm_rau_is_required == TRUE)
#endif /*FEATURE_GSM_GPRS*/
                    )
                {
                  /************************************************************************* 
                  *23.060, 6.1.2.4.1. The UE shall also perform a RAU procedure immediately 
                  *on entering PMM-IDLE state when it has received a RRC Connection Release 
                  *message with cause "Directed Signalling connection re-establishment" even 
                  *if the RA has not changed since the last update.The UE shall perform a
                  *subsequent Service request procedure after successful completion of the
                  *RA Update procedure to re-establish the radio access bearer when it has 
                  *pending user data to send.
                  **************************************************************************/
                  if(gmm_processing_directed_signalling_reestablishment)
                  {
                     MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ due to RAU failed which is started because of Directed Signalling connection Reestablishment");
                     gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
                  }
                  else
                  {
                     gmm_service_request_initiator = MS_GMM_SM;
                     gmm_service_request_type      = GMM_SIGNALLING;

                     gmm_initiate_service_request(
                            message->cmd.gmmsm_establish_req.cause,
                            mm_get_current_security_context( RRC_PS_DOMAIN_CN_ID ),
                            NULL
                           );
                  }
                }
                else
                {
                  MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ due to GMM update status != GMM_GU1_UPDATED, it is %d",gmm_update_status);
                  gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
                }
              }
            }
          }
#ifdef FEATURE_ENHANCED_NW_SELECTION
          else if ((reg_sim_read_ens_flag()) &&
#ifdef FEATURE_DUAL_SIM
                       (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn, (sys_modem_as_id_e_type)mm_sub_id )) 
#else
                       (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn )) 
#endif
                  )
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ as PLMN is GPRS FORBIDDEN");
            gmm_send_gmmsm_establish_rej(PDP_LOWERLAYER_ERROR);
          }
#endif 
#ifdef FEATURE_DUAL_SIM
          else if(((reg_sim_per_subs_plmn_forbidden( mm_serving_plmn.info.plmn ,(sys_modem_as_id_e_type)mm_sub_id )) 
#else
          else if(((reg_sim_plmn_forbidden( mm_serving_plmn.info.plmn )) 
#endif
                      )
                      && (gmm_anite_gcf_flag))
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ as PLMN is FORBIDDEN");
            gmm_send_gmmsm_establish_rej(PDP_LOWERLAYER_ERROR);
          }
#ifdef FEATURE_WRLF_SYSTEM_SEL
          else if ((gmm_state == GMM_REGISTERED) &&
                   (pmm_mode == PMM_CONNECTED) &&
                   (mm_wcdma_rlf_state != WCDMA_RLF_IDLE))
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_CNF");
            gmm_send_gmmsm_establish_cnf(TRUE);
          } 
#endif
#ifdef FEATURE_LTE
          else if (!is_umts_mode() && !is_gsm_mode())
          {
            MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ as active rat is not GW active rat = %d", 
                                mm_serving_plmn.info.active_rat);
            gmm_send_gmmsm_establish_rej(PDP_LOWERLAYER_ERROR);
          }
#endif
          else
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ");
            gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(ABORT_SERVICE_NOT_AVAILABLE));
          }
          break;

        case GMMSM_CLOSE_SESSION_REQ:
          MSG_HIGH_DS_0( MM_SUB, "=MM= Received GMMSM_CLOSE_SESSION_REQ" );

#ifdef FEATURE_DUAL_SIM
          if (message->cmd.gmmsm_session_close_req.cause == SM_DS_PS_SIG_REL_REQ ||
              message->cmd.gmmsm_session_close_req.cause == SM_TS_DS_SIG_REL_REQ)
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= DSDS PS ABORT Initiated");
            if (is_umts_mode())
            {
              if((mm_serving_plmn.info.service_status == SYS_SRV_STATUS_NO_SRV)||
                 (rrc_connection_status == MMRRC_IDLE) )
              {
                gmm_sm_send_con_release_ind(SIG_CONNECTION_RELEASE_UE_INITIATED);
              }
              else
              {
#if defined FEATURE_WCDMA || defined FEATURE_TDSCDMA
                rrc_cmd_type *rrc_cmd_p = NULL;
  
                rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
  
                if (rrc_cmd_p == NULL)
                {
                  MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                }
  
                rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
                rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = FALSE;
                rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
                rrc_cmd_p->cmd.rrc_abort_req.hard_abort = FALSE;                
                rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_DUAL_STANDBY_PS_ABORT;
  #ifdef FEATURE_REL6_PS_FOP              
                if(gmm_ps_fop_resp == PS_FOP_NW_GRANTED)
                {
                  gmm_ps_fop_resp = PS_FOP_UE_NOT_GRANTED_NW_GRANTED_BEFORE_CONN_REL;
                }
                else if(gmm_ps_fop_resp == PS_FOP_NW_NOT_GRANTED)
                {
                  gmm_ps_fop_resp = PS_FOP_UE_NOT_GRANTED_NW_NOT_GRANTED_BEFORE_CONN_REL;
                }
  #endif
                send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
#endif
              }
            }
            else if (is_gsm_mode() &&
                     message->cmd.gmmsm_session_close_req.cause == SM_DS_PS_SIG_REL_REQ)
            {
               rr_ps_abort_req_T    rr_ps_abort_req;
               rr_ps_abort_req.message_header.message_set = MS_MM_RR;
               rr_ps_abort_req.message_header.message_id  = (int) RR_PS_ABORT_REQ;
               PUT_IMH_LEN( sizeof( rr_ps_abort_req_T ) - sizeof(IMH_T),
                            &rr_ps_abort_req.message_header);

               rr_ps_abort_req.abort_cause = MM_RR_DUAL_STANDBY_CS_MO_CALL;

               MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RR_PS_ABORT_REQ");
               mm_send_message( &rr_ps_abort_req.message_header, GS_QUEUE_RR );
            }
            else
            {
              gmm_sm_send_con_release_ind(SIG_CONNECTION_RELEASE_UE_INITIATED);
            }
          }
          else
#endif

          if (is_umts_mode())
          {
           /*If GMM specific procedure is running don't close the session
            */
            if ((gmm_state == GMM_REGISTERED_INITIATED) ||
                (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
                (gmm_state == GMM_DEREGISTERED_INITIATED) ||
                (gmm_state == GMM_SERVICE_REQUEST_INITIATED) ||
                (gmm_substate == GMM_IMSI_DETACH_INITIATED)||
                (mm_sms_is_active))
            {
              if (message->cmd.gmmsm_session_close_req.cause == SM_UE_PS_SIG_REL_REQ)
              {
                gmm_sm_send_con_release_rej();
              }
              MSG_HIGH_DS_3( MM_SUB, "=MM= GMM procedure/PS SMS running - GMMSM_CLOSE_SESSION_REQ rejected GMM state %d,GMM substate %d,PS SMS %d", 
                           gmm_state,gmm_substate,mm_sms_is_active );
            }
            else
            {
              if ((message->cmd.gmmsm_session_close_req.cause == SM_UE_PS_SIG_REL_REQ) &&
                 (pmm_mode != PMM_CONNECTED))
              {
                gmm_sm_send_con_release_ind(SIG_CONNECTION_RELEASE_UE_INITIATED);
              }
#ifdef FEATURE_WCDMA
              else
              {
                rrc_cmd_type *rrc_cmd_p = NULL;
                rrc_mm_req_cause_e_type cause = 
                            (message->cmd.gmmsm_session_close_req.cause == SM_UE_PS_SIG_REL_REQ) ? RRC_UE_INITIATED_DORMANCY: RRC_OTHERS;
#ifdef FEATURE_WRLF_SYSTEM_SEL 
                if (mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
                {
#ifdef FEATURE_MODEM_HEAP
                  rrc_cmd_p = mm_get_rrc_cmd_buf( mm_wrlf_serving_plmn_ptr->info.active_rat );
#else
                  rrc_cmd_p = mm_get_rrc_cmd_buf( mm_wrlf_serving_plmn.info.active_rat );
#endif
                }
                else
#endif
                rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
                if (rrc_cmd_p == NULL)
                {
                  MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                }
            
                MSG_HIGH_DS_1(MM_SUB, "=MM= Signaling Connection close cause: %d",cause);
                if ( (cs_session_status != MMRRC_IDLE)
                      ||
                     (message->cmd.gmmsm_session_close_req.cause == SM_UE_PS_SIG_REL_REQ)
                   )
                {
                  rrc_cmd_p->cmd_hdr.cmd_id = RRC_CLOSE_SESSION_REQ;
                  rrc_cmd_p->cmd.close_sess_req.cn_domain_id = RRC_PS_DOMAIN_CN_ID;
                  rrc_cmd_p->cmd.close_sess_req.close_session_cause = cause;
                }
                else
                {
                  rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
                  rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = TRUE;
                  rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
                  rrc_cmd_p->cmd.rrc_abort_req.hard_abort = FALSE;                
                  rrc_cmd_p->cmd.rrc_abort_req.abort_cause = cause;
                }
#ifdef FEATURE_REL6_PS_FOP              
                if(gmm_ps_fop_resp == PS_FOP_NW_GRANTED)
                {
                  gmm_ps_fop_resp = PS_FOP_UE_NOT_GRANTED_NW_GRANTED_BEFORE_CONN_REL;
                }
                else if(gmm_ps_fop_resp == PS_FOP_NW_NOT_GRANTED)
                {
                  gmm_ps_fop_resp = PS_FOP_UE_NOT_GRANTED_NW_NOT_GRANTED_BEFORE_CONN_REL;
                }
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL 
                if (mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
                {
#ifdef FEATURE_MODEM_HEAP
                  send_cmd_to_rrc( rrc_cmd_p, mm_wrlf_serving_plmn_ptr->info.active_rat  );
#else
                  send_cmd_to_rrc( rrc_cmd_p, mm_wrlf_serving_plmn.info.active_rat  );
#endif
                }
                else
#endif
                send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
              }
#endif /* FEATURE_WCDMA */	      
            }
          }
          else if (is_gsm_mode())
          {
            if (message->cmd.gmmsm_session_close_req.cause == SM_UE_PS_SIG_REL_REQ)
            {
              gmm_sm_send_con_release_ind(SIG_CONNECTION_RELEASE_UE_INITIATED);
            }
          }
          break;

        case GMMSM_FAST_DORMANCY_REQ:
          MSG_HIGH_DS_0(MM_SUB, "=MM= Received GMMSM_FAST_DORMANCY_REQ");

          if (is_umts_mode())
          {
           /*If GMM specific procedure is running don't close the session
            */
            if ((gmm_state == GMM_REGISTERED_INITIATED) ||
                (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
                (gmm_state == GMM_DEREGISTERED_INITIATED) ||
                (gmm_state == GMM_SERVICE_REQUEST_INITIATED) ||
                (gmm_substate == GMM_IMSI_DETACH_INITIATED)||
                (mm_sms_is_active))
            {
              gmm_sm_send_fast_dormancy_cnf (GMMSM_FAST_DORMANCY_FAILURE_APP_RETRY);
              MSG_HIGH_DS_3(MM_SUB, "=MM= GMM procedure/PS SMS running - GMMSM_FAST_DORMANCY_REQ rejected GMM state %d,GMM substate %d PS SMS %d",
                           gmm_state,gmm_substate,mm_sms_is_active );
            }
            else
            {
              if ( pmm_mode != PMM_CONNECTED )
              {
                gmm_sm_send_fast_dormancy_cnf (GMMSM_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);
              }
#ifdef FEATURE_WCDMA	      
              else
              {
                rrc_cmd_type *rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
                if (rrc_cmd_p == NULL)
                {
                  MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
                }
                rrc_cmd_p->cmd_hdr.cmd_id = RRC_FAST_DORMANCY_REQ;
                rrc_cmd_p->cmd.rrc_fast_dormancy_req.cause = RRC_FAST_DORMANCY_INIT;
#ifdef FEATURE_DUAL_SIM
                rrc_cmd_p->cmd.rrc_fast_dormancy_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
              }
#endif /* FEATURE_WCDMA */	      
            }
          }
          else
          {
              gmm_sm_send_fast_dormancy_cnf (GMMSM_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);
          }
          break;

        case GMMSM_UNITDATA_REQ:
          MSG_HIGH_DS_0( MM_SUB, "=MM= Received GMMSM_UNITDATA_REQ" );
          GET_IMH_LEN(length, &message->cmd.gmmsm_unitdata_req.header);
          if(gmm_controls_resel_dec_mt_ps_call == TRUE &&
              message->cmd.gmmsm_unitdata_req.ps_vote_flag == TRUE)
          {
            mm_stop_timer(TIMER_FORBID_TDS_RESELECTION);
            gmm_controls_resel_dec_mt_ps_call = FALSE;
          }

#ifdef FEATURE_WRLF_SYSTEM_SEL
          if((mm_wcdma_rlf_state != WCDMA_RLF_IDLE)&&
             (pmm_mode == PMM_CONNECTED))
          {
#ifdef FEATURE_WCDMA
            /* -------------------------------------
            ** Send SM message to RRC layer directly
            ** ------------------------------------- */
            gmm_send_rrc_data_request(message->cmd.gmmsm_unitdata_req.data,length);
#endif
          }
          else
#endif
          if (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE)
          {
#ifdef FEATURE_WCDMA
            if (is_umts_mode())
            {
              if (pmm_mode == PMM_CONNECTED)
              {
                /* -------------------------------------
                ** Send SM message to RRC layer directly
                ** ------------------------------------- */
                 gmm_send_rrc_data_request(message->cmd.gmmsm_unitdata_req.data,length);              

              }
            }
#endif /* #ifdef FEATURE_WCDMA */

#ifdef FEATURE_GSM_GPRS
            if (is_gsm_mode())
            {
              if (gmm_state == GMM_REGISTERED)
              {
                if (( gmm_substate == GMM_SUSPENDED ) || (gmm_is_suspended))
                {
                  MSG_HIGH_DS_0(MM_SUB, "=MM= Discarding GMMSM_UNITDATA_REQ due to GPRS suspension");
                }
                else
                {
                  /* -------------------------------------
                  ** Send SM message to LLC layer directly
                  ** ------------------------------------- */
                  gmmllc_gmm_send_ota_message(
                    length,
                    message->cmd.gmmsm_unitdata_req.data
#ifdef FEATURE_GPRS_PS_HANDOVER
                    ,message->cmd.gmmsm_unitdata_req.purgeQueueFlag
#endif
                                                                        ) ;

                }
              }
            }
#endif /* #ifdef FEATURE_GSM_GPRS */
          }
          break;

        default:
          MSG_ERROR_DS_1( MM_SUB, "=MM= Unexpected SM command %d", message->cmd.hdr.message_id);
          break;
      } /* end switch (msg_header->cmd.hdr.message_id) */
      break;

#ifdef FEATURE_WCDMA
#ifdef FEATURE_TC
    case MS_GMM_TC:
      if (message->cmd.hdr.message_id == GMMTC_UNITDATA_REQ)
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= Received GMMTC_UNITDATA_REQ" );

        if ((mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE) &&
            (pmm_mode == PMM_CONNECTED))
        {
          /* -------------------------------------
          ** Send TC message to RRC layer directly
          ** ------------------------------------- */
          
          rrc_cmd_type *rrc_cmd = NULL;
          rrc_cmd = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );

          if (rrc_cmd == NULL)
          {
            MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
          }

          rrc_cmd->cmd_hdr.cmd_id              = RRC_DATA_REQ;
          rrc_cmd->cmd.data_req.cn_domain_id   = RRC_PS_DOMAIN_CN_ID;
          rrc_cmd->cmd.data_req.priority       = RRC_NAS_MSG_HIGH_PRIO;
          rrc_cmd->cmd.data_req.is_emergency_call = FALSE;
          rrc_cmd->cmd.data_req.nas_msg.length =
            message->cmd.gmmtc_unitdata_req.header.message_len_lsb;
#ifndef FEATURE_MODEM_HEAP
          rrc_cmd->cmd.data_req.nas_msg.value =
            mm_mem_alloc( rrc_cmd->cmd.data_req.nas_msg.length );
#else
          if (is_wcdma_mode())
          {
            rrc_cmd->cmd.data_req.nas_msg.value =
#ifdef FEATURE_SEGMENT_LOADING
              IWCDMA_rrc_malloc_for_external_cmd(ptr_W, rrc_cmd->cmd.data_req.nas_msg.length );
#else
              rrc_malloc_for_external_cmd( rrc_cmd->cmd.data_req.nas_msg.length );
#endif
          }
#ifdef FEATURE_TDSCDMA
          else if (is_tdscdma_mode())
          {
            rrc_cmd->cmd.data_req.nas_msg.value =
              mm_mem_alloc( rrc_cmd->cmd.data_req.nas_msg.length );
          }
#endif /* FEATURE_TDSCDMA */
#endif
          if (rrc_cmd->cmd.data_req.nas_msg.value == NULL)
          {
            MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
          }

          memscpy( rrc_cmd->cmd.data_req.nas_msg.value,rrc_cmd->cmd.data_req.nas_msg.length,
                  message->cmd.gmmtc_unitdata_req.data,
                  rrc_cmd->cmd.data_req.nas_msg.length );

          MSG_HIGH_DS_0( MM_SUB, "=MM= Sending RRC_DATA_REQ");

          send_cmd_to_rrc( rrc_cmd, mm_serving_plmn.info.active_rat );
        }
      }
      break;
#endif /* FEATURE_TC */
#endif /* FEATURE_WCDMA */

    case MS_GMM_RABM:
    {
      mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_NONE);     
      switch (message->cmd.hdr.message_id)
      {
        case GMMRABM_REESTABLISH_REQ:
          MSG_HIGH_DS_0( MM_SUB, "=MM= Received GMMRABM_REESTABLISH_REQ");
          /* Check AC blocking restrictions */
          if ( mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE ||
               mm_waiting_for_stop_mode_cnf == TRUE)
          {
            rabm_cmd = rabm_get_cmd_buf();

            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
             rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_1( MM_SUB,
                         "=MM= Sending GMMRABM_REESTABLISH_REJ due to no GPRS service or mm_waiting_for_stop_mode_cnf %d",
                         mm_waiting_for_stop_mode_cnf );

            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
          else if (gmm_state != GMM_REGISTERED)
          {
            rabm_cmd = rabm_get_cmd_buf();

            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;

            if ((gmm_state == GMM_REGISTERED_INITIATED) ||
                (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)) 
            {
              rabm_cmd->cmd.gmm_reestablish_rej.cause = REGISTRATION_IN_PROGRESS;
            }
            else
            {
               rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
            }
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ as GMM is in state %d", gmm_state);

            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer( rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
          else if (gmm_substate == GMM_LIMITED_SERVICE ||
                   gmm_substate == GMM_ATTEMPTING_TO_UPDATE)
          {
            rabm_cmd = rabm_get_cmd_buf();

            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ : wrong GMM substate %d", gmm_substate );

            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
          else if ((gmm_check_if_ps_cell_access_barred(ps_cell_access) == TRUE) &&
#ifdef FEATURE_DSAC            
                   (( mm_system_information.dsac_present == FALSE && 
                    ( rrc_connection_status == MMRRC_IDLE)) ||
                    ( mm_system_information.dsac_present == TRUE && 
                      ps_session_status == MMRRC_IDLE )) 
#else
                   ( rrc_connection_status == MMRRC_IDLE)
#endif
                  )
          {
            rabm_cmd = rabm_get_cmd_buf();

            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = ACCESS_CLASS_DSAC_REJECTION;
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_2( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ due to AC blocking %d, cause: %d",
                       ps_cell_access, rabm_cmd->cmd.gmm_reestablish_rej.cause);

            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
#ifdef FEATURE_REL6_PS_FOP
          else if ( gmm_ps_fop_resp != PS_FOP_NW_GRANTED)
          {
            rabm_cmd = rabm_get_cmd_buf();

            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ: FOP not granted");

            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );          
          }
#endif/* FEATURE_REL6_PS_FOP */         
          else if (gmm_update_status != GMM_GU1_UPDATED)
          {
            rabm_cmd = rabm_get_cmd_buf();

            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ due to gprs update status != GMM_GU1_UPDATED, it is %d",gmm_update_status);

            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
#ifdef FEATURE_NAS_REL10
          else if ((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
		  #ifdef FEATURE_NAS_REL11
                    &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
                  )
          {
            rabm_cmd = rabm_get_cmd_buf();
            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }
            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id   = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = CONGESTTION;
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ due to T3346 running ");
            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FALSE
                                           ,(sys_modem_as_id_e_type)mm_as_id
                                           );
          }
#endif
          else if(!(ds3gpp_per_subs_is_service_req_allowed_on_plmn(mm_serving_plmn.info.plmn, (sys_modem_as_id_e_type)mm_sub_id)))
          {
            rabm_cmd = rabm_get_cmd_buf();

            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_0( MM_SUB,
                         "=MM= Sending GMMRABM_REESTABLISH_REJ due to active DS throttling timer ");

            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
        else if(mm_is_mplmn_search_in_progress() == TRUE)
        {
            rabm_cmd = rabm_get_cmd_buf();

            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ:because of manual plmn search is in progress" );

            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                           ,(sys_modem_as_id_e_type)mm_as_id
                                          );

          }
          else if(((gmm_update_status == GMM_GU1_UPDATED) &&
                    (!gmm_rai_changed(gmm_stored_rai.location_area_code, gmm_stored_rai.routing_area_code, 
                        mm_serving_plmn.lac, mm_serving_plmn.rac)) &&
                    (!gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))
#ifdef FEATURE_GSM_GPRS
                   || (gmm_rau_is_required == TRUE)
#endif /*FEATURE_GSM_GPRS*/
              )
          {
            /*If pmm mode is already PMM_CONNECTED and reestablish cause is
              conversational/streaming no need to send Service request(type=data)
              section 9.2.3.5 from 23.060 and 6.1.3.3 from 24.008*/
            if (pmm_mode == PMM_CONNECTED &&
               (message->cmd.gmmrabm_reestablish_req.traffic_class == (byte)CONVERSATION_CLASS ||
                message->cmd.gmmrabm_reestablish_req.traffic_class == (byte)STREAMING_CLASS))
            {
              rabm_cmd = rabm_get_cmd_buf();

              if (rabm_cmd == NULL)
              {
                MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
              }

              rabm_cmd->header.message_set = MS_GMM_RABM;
              rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_RSP;
#ifdef FEATURE_DUAL_SIM
              rabm_cmd->cmd.gmm_reestablish_rsp.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

              MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_RSP");

              rabm_put_cmd( rabm_cmd );
              /*adding message to the debug buffer*/
              mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                             ,(sys_modem_as_id_e_type)mm_as_id
                                             );
              gmm_send_gmmsm_establish_ind(SM_PS_SIG_CONN_AVAIL);	      
            }
            else
            {
              if (is_umts_mode())
              {
                /************************************************************************* 
                *23.060, 6.1.2.4.1. The UE shall also perform a RAU procedure immediately 
                *on entering PMM-IDLE state when it has received a RRC Connection Release 
                *message with cause "Directed Signalling connection re-establishment" even 
                *if the RA has not changed since the last update.The UE shall perform a
                *subsequent Service request procedure after successful completion of the
                *RA Update procedure to re-establish the radio access bearer when it has 
                *pending user data to send.
                **************************************************************************/
                if (gmm_processing_directed_signalling_reestablishment)
                {
                  rabm_cmd = rabm_get_cmd_buf();

                  if (rabm_cmd == NULL)
                  {
                    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                  }

                  rabm_cmd->header.message_set = MS_GMM_RABM;
                  rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
                  rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
                  rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ:RAU started because of Directed Signalling Reestablishment is failed" );

                  rabm_put_cmd( rabm_cmd );
                  /*adding message to the debug buffer*/
                  mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                                 ,(sys_modem_as_id_e_type)mm_as_id
                                                 );
     
                }
                else if(mm_is_mplmn_search_in_progress() == TRUE)
                {
                  rabm_cmd = rabm_get_cmd_buf();

                  if (rabm_cmd == NULL)
                  {
                    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                  }

                  rabm_cmd->header.message_set = MS_GMM_RABM;
                  rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
                  rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
                  rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ:because of manual plmn search is in progress");

                  rabm_put_cmd( rabm_cmd );
                  /*adding message to the debug buffer*/
                  mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                                 ,(sys_modem_as_id_e_type)mm_as_id
                                                 );

                }
                else
                {
                  gmm_service_request_initiator = MS_GMM_RABM;
                  gmm_service_request_type      = GMM_DATA;

                  gmm_initiate_service_request(
                  message->cmd.gmmrabm_reestablish_req.cause,
                  mm_get_current_security_context( RRC_PS_DOMAIN_CN_ID ),
                  &message->cmd.gmmrabm_reestablish_req.uplink_data_status
                  );
                }
              }
              else
              {
                rabm_cmd = rabm_get_cmd_buf();

                if (rabm_cmd == NULL)
                {
                  MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                }

                rabm_cmd->header.message_set = MS_GMM_RABM;
                rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
                rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
                rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ: GSM mode");

                rabm_put_cmd( rabm_cmd );
                /*adding message to the debug buffer*/
                mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                                ,(sys_modem_as_id_e_type)mm_as_id
                                              );
              }
            }
          }
          else
          {
            rabm_cmd = rabm_get_cmd_buf();

            if (rabm_cmd == NULL)
            {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            rabm_cmd->header.message_set = MS_GMM_RABM;
            rabm_cmd->header.cmd_id      = GMMRABM_REESTABLISH_REJ;
            rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
#ifdef FEATURE_DUAL_SIM
            rabm_cmd->cmd.gmm_reestablish_rej.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_1( MM_SUB, "=MM= Sending GMMRABM_REESTABLISH_REJ due to gprs update status != GMM_GU1_UPDATED, it is %d",gmm_update_status );

            rabm_put_cmd( rabm_cmd );
            /*adding message to the debug buffer*/
            mm_add_message_to_debug_buffer(rabm_cmd->header.message_set, (dword)rabm_cmd->header.cmd_id, FROM_MM
                                            ,(sys_modem_as_id_e_type)mm_as_id
                                          );
          }
          break;
        case GMMRABM_RAB_REL_IND:
          MSG_HIGH_DS_0( MM_SUB, "=MM= Received GMMRABM_RAB_REL_IND");
          mm_stop_timer( TIMER_T3319 );
          break;
/**----------------------------------------------------------------------------------------------
      ** spec 24.008 4.7.1.9: 
      
          In case b and c,
          -upon an indication from the lower layers that radio accesss bearer(s) is set up, the MS shall stop timer T3340
            and may send uplink signalling via the existing PS signalling connection or user data via radio access bearer(s) 
**----------------------------------------------------------------------------------------------*/
        case GMMRABM_RAB_EST_IND:
          MSG_HIGH_DS_0( MM_SUB, "=MM= Received GMMRABM_RAB_EST_IND");
          if(mm_timer_status [ TIMER_PS_CONN_REL ] == TIMER_ACTIVE)
          {
            mm_stop_timer(TIMER_PS_CONN_REL);
	  }
          if(mm_timer_status [ TIMER_T3340 ] == TIMER_ACTIVE)
          {
            mm_stop_timer( TIMER_T3340 );
            if(gmm_ps_fop_resp == PS_FOP_NW_NOT_GRANTED)
            {
              gmm_ps_fop_resp = PS_FOP_NW_GRANTED;
            }

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
            if (gmm_gprs_detach_pending)
            {
              if (mmcoord_pending_reg_message.cmd.hdr.message_set != 0)
              {
                gmm_initiate_gprs_detach( &mmcoord_pending_reg_message );
              }
              else
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Unable to proceed with GPRS Detach");
              }
            }
            else 
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/    
  
            if(gmm_gprs_attach_pending)
            {
              gmm_initiate_gprs_attach();
            }
            else if (gmm_ra_update_pending)
            {
              if(gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
              {
                MSG_HIGH_DS_0( MM_SUB, "GMM= Already RAU in progress,RAU should not be initiated again");
              }
              else if ((mm_state != MM_IDLE ) &&
                       ((gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
                        (gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)) &&
                       (gmm_state == GMM_REGISTERED) &&
                       (!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                            gmm_stored_rai.routing_area_code,
                                            mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                          (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
                       (gmm_update_status == GMM_GU1_UPDATED) 
                      )
              {
                 MSG_HIGH_DS_2(MM_SUB,"=MM= combined RAU should not be initiated since mm is not idle, gmm_pending_ra_update_type %d, gmm_update_status %d", 
                            gmm_pending_ra_update_type,gmm_update_status);
              }
              else
              {
                gmm_ra_update_type = gmm_pending_ra_update_type;
                gmm_initiate_routing_area_update();
              }  
            }  
          }
          break;
        default:
          MSG_ERROR_DS_1( MM_SUB, "=MM= Unexpected RABM command %d", message->cmd.hdr.message_id );
          break;
      } /* end switch (message->cmd.hdr.message_id) */
    }     
      break;

    case MS_MM_RRC:
    case MS_MM_TDSRRC:
      switch (message->cmd.hdr.message_id)
      {
        case RRC_SYNC_IND:
          if ((message->cmd.rrc_sync_ind.cause == RRC_INTEGRITY_ESTABLISHED) ||
              (message->cmd.rrc_sync_ind.cause == RRC_INTEGRITY_CIPHERING_ESTABLISHED))
          {
            gmm_integrity_protection_activated = TRUE;
            /* --------------------------------------------------------------------
            ** 24.008 4.7.3.1.5,4.7.4.1.4, 4.7.5.1.5, 4.7.13.5: cases b.1 and b.2
            ** The procedure shall be initiated again, if 
            ** ii)No SECURITY MODE COMMAND message and no NAS messages relating 
            ** to the PS signalling/RRC connection were received after the OTA msg was transmitted
            ** -------------------------------------------------------------------- */
            if(mm_ps_domain_possible_reest)
            {
              
              mm_ps_domain_possible_reest = FALSE;
            }
            if (gmm_ps_signalling_connected)
            {
              /* ----------------------------------------------------------------
              ** The timer T3312 is stopped when the MS enters PMM-CONNECTED mode
              ** (24.008 4.7.2.2)
              ** ---------------------------------------------------------------- */
              if (gmm_state != GMM_SERVICE_REQUEST_INITIATED)
              {
                pmm_mode = PMM_CONNECTED;
                MSG_HIGH_DS_0(MM_SUB, "=MM= pmm_mode = PMM_CONNECTED");

                gmm_periodic_rau_pending = FALSE;
              }

              mm_stop_timer( TIMER_T3312 );
#if defined(FEATURE_LTE)
              mm_stop_timer(TIMER_T3323);
#endif

            }
          }

          if (gmm_state == GMM_SERVICE_REQUEST_INITIATED)
          {
            gmm_handle_service_request( message );
          }
          break;

        case MM_DATA_IND:
          switch (message->cmd.mm_data_ind.L3_data[1])
          {
            case GMM_SERVICE_ACCEPT:
              MSG_HIGH_DS_0( MM_SUB, "=MM= Received SERVICE ACCEPT message");

              if (gmm_integrity_protection_activated
#ifdef FEATURE_TDSCDMA
                  || (!gmm_integrity_protection_activated && is_tdscdma_mode() && !gmm_anite_gcf_flag)
#endif /* FEATURE_TDSCDMA */
                  )
              {
                gmm_handle_service_request( message );
              }
              else
              {
                MSG_ERROR_DS_0( MM_SUB, "=MM= Integrity protection not activated");
              }
              break;

            case GMM_SERVICE_REJECT:
              MSG_HIGH_DS_0( MM_SUB, "=MM= Received SERVICE REJECT message");  
              if(gmm_check_service_reject_for_integrity_protection(message))
              {                
                /* -----------------------------------------------------------------
                ** The SERVICE REJECT message is processsed regardless of whether or
                ** not the security mode control procedure has been activated
                ** ----------------------------------------------------------------- */
                gmm_handle_service_request( message );
              }
              else
              {
                MSG_ERROR_DS_0( MM_SUB, "=MM= Integrity protection not activated" );
              }
              break;

            default:
              MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message type %d",
                          message->cmd.mm_data_ind.L3_data[1], 0,0 );
          } /* end switch (gmm_message_type) */
          break;

        case RRC_OPEN_SESSION_CNF:
          if (message->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_FAILURE ||
              message->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_FAILURE_CONN_REL_IN_PROG)
          {
            /* ------------------------------------------
            ** Treat as indication of lower layer failure
            ** ------------------------------------------ */
            if (gmm_state == GMM_REGISTERED_INITIATED)
            {
              if(message->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_FAILURE_CONN_REL_IN_PROG && mm_ps_domain_possible_reest)
              {
                mm_stop_timer(TIMER_T3310);
                MSG_HIGH_DS_0(MM_SUB,"=MM= wait for conenction release and retry attach after that");
              }
              else
              {
                gmm_handle_gprs_attach( message );
              }
            }
            else if (gmm_state == GMM_DEREGISTERED_INITIATED)
            {
              gmm_handle_gprs_detach( message );
            }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
            else if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
            {
              if(message->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_FAILURE_CONN_REL_IN_PROG && mm_ps_domain_possible_reest)
              {
                mm_stop_timer(TIMER_T3330);
                MSG_HIGH_DS_0(MM_SUB,"=MM= wait for conenction release and retry RAU after that");
              }
              else
              {
                gmm_handle_routing_area_update( message );
              }
            }
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
            else if (gmm_state == GMM_SERVICE_REQUEST_INITIATED)
            {
              gmm_handle_service_request( message );
            }
          }
          else if (message->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_SUCCESS)
          {
            gmm_ps_signalling_connected = TRUE;
            
            /* for tracking the RAT on which last PS signalling information is sent */
            if ( message->cmd.hdr.message_set == MS_MM_TDSRRC
#ifdef FEATURE_SEGMENT_LOADING
               &&get_tdscdma_interface()
#endif
)            {
              gmm_last_signalling_sent_rat = SYS_RAT_TDS_RADIO_ACCESS;
            }
            else if (TRUE
#ifdef FEATURE_SEGMENT_LOADING
               &&get_wcdma_interface()
#endif
)           {
              gmm_last_signalling_sent_rat = SYS_RAT_UMTS_RADIO_ACCESS;
            }
#ifdef FEATURE_LTE
            if((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) && 
               ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) || (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH))&&
                (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_NOT_REQ ||
                 emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_FOR_CSFB_RAU)
               )
             {
                emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
                MSG_HIGH_DS_0(MM_SUB,"=MM= Set SGs assosicaiton flag - On Open Session cnf on RRC for NMO1");
             }
#endif
            if (gmm_integrity_protection_activated)
            {
              /* ----------------------------------------------------------------
              ** The timer T3312 is stopped when the MS enters PMM-CONNECTED mode
              ** (24.008 4.7.2.2)
              ** ---------------------------------------------------------------- */
              pmm_mode = PMM_CONNECTED;

              MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_CONNECTED" );

#if defined(FEATURE_LTE)
              mm_stop_timer(TIMER_T3323);
#endif

              gmm_periodic_rau_pending = FALSE;

              mm_stop_timer( TIMER_T3312 );
            }

            if (gmm_state == GMM_SERVICE_REQUEST_INITIATED)
            {
              mm_start_timer(TIMER_T3317, DEFAULT_TIMEOUT);
              gmm_handle_service_request( message );
            }
            else if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
            {
              mm_start_timer( TIMER_T3330, DEFAULT_TIMEOUT );
            }
            else if (gmm_state == GMM_REGISTERED_INITIATED)
            {
              mm_start_timer( TIMER_T3310, DEFAULT_TIMEOUT );
            }
          }
          break;

        case RRC_FAST_DORMANCY_CNF:
          {
            gmm_sm_send_fast_dormancy_cnf (mm_convert_rrc_to_gmmsm_cause (message->cmd.rrc_fast_dormancy_cnf.status) );
          }
        break;

        case RRC_ABORT_IND:
        case RRC_ABORT_CNF:
        case RRC_CLOSE_SESSION_IND:
        case RRC_CLOSE_SESSION_CNF:
        case RRC_REL_IND:

          /* ------------------------------------------------------------------------
          ** If RRC sends a close cnf/abort cnf indicating OOS, it expresses RRC's inability to release
          ** signaling connection/RRC connection due to OOS. NAS state machine should not change
          ** based on close/abort cnf. Instead, lower layer failure should be communicated to SM,
          ** and inturn to CM.
          ** ------------------------------------------------------------------------*/
#ifdef FEATURE_LTE
          if((mm_nreg_release_pending == TRUE) &&(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF ||
              mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING)) 
          {
           mm_stop_timer(TIMER_MM_PS_DETACH);
           mm_stop_timer(TIMER_DETACH_REL);
           mm_nreg_release_pending = FALSE;
           gmm_gprs_attach_pending = FALSE;
           mm_send_mmr_ps_detach_cnf();
           mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
#ifdef FEATURE_MODEM_HEAP
           if(mm_pending_mmr_ps_detach_req_ptr != NULL)
           {
             modem_mem_free(mm_pending_mmr_ps_detach_req_ptr,MODEM_MEM_CLIENT_NAS);
             mm_pending_mmr_ps_detach_req_ptr = NULL;
           }
#else
           memset(&mm_pending_mmr_ps_detach_req,0,sizeof(mm_cmd_type));
#endif
          }
#endif
          if (((message->cmd.hdr.message_id == (byte)RRC_CLOSE_SESSION_CNF) &&
                (message->cmd.rrc_close_session_cnf.status == RRC_CLOSE_SESS_FAILURE_UE_INIT_DORMANCY_UE_IN_OOS)) ||
              ((message->cmd.hdr.message_id == (byte)RRC_ABORT_CNF) &&
                (message->cmd.rrc_abort_cnf.conn_rel_cause == RRC_UE_INIT_DORMANCY_UE_IN_OOS)))
          {
             gmm_sm_send_con_release_rej();
#ifdef FEATURE_REL6_PS_FOP
             if(gmm_ps_fop_resp == PS_FOP_UE_NOT_GRANTED_NW_GRANTED_BEFORE_CONN_REL)
             {
                gmm_ps_fop_resp = PS_FOP_NW_GRANTED;
             }
             else if(gmm_ps_fop_resp == PS_FOP_UE_NOT_GRANTED_NW_NOT_GRANTED_BEFORE_CONN_REL)
             {
               gmm_ps_fop_resp = PS_FOP_NW_NOT_GRANTED;
             }
#endif
          }
          else
#ifdef FEATURE_DUAL_SIM
          if ((message->cmd.hdr.message_id == (byte)RRC_ABORT_CNF) &&
              (message->cmd.rrc_abort_cnf.conn_rel_cause == RRC_REL_DUAL_STANDBY_PS_ABORT_SUCCESS))
          {
             mm_sms_is_active = FALSE;
#ifdef FEATURE_REL6_PS_FOP
             gmm_ps_fop_resp = PS_FOP_NW_GRANTED;
#endif
             gmm_sm_send_con_release_ind(SIG_CONNECTION_RELEASE_UE_INITIATED);
          }
          else
#endif

          {
              gmm_ps_signalling_connected = FALSE;
              gmm_integrity_protection_activated = FALSE;
              gmm_extend_connection_due_to_mt_csfb = FALSE;

#ifdef FEATURE_REL6_PS_FOP          
              gmm_ps_fop_resp = PS_FOP_NW_GRANTED;
#endif/* FEATURE_REL6_PS_FOP */
              /******************************************************
              *Stop the timer T3340.Other wise when Ps signalling 
              *message has to be sent, there can be a chance that,UE
              *will not send Ps signalling message until T3340 expiry
              ******************************************************/
              mm_stop_timer(TIMER_T3340);
              mm_stop_timer(TIMER_PS_CONN_REL);

              mm_send_cm_cipher_ind(SYS_SRV_DOMAIN_PS_ONLY,FALSE);

              if((pending_gmm_msg_after_release) ||
                  gmm_reestablish_connection(message))              
              {
                /* GMM is trying to re-establish connection */
                pending_gmm_msg_after_release = FALSE;
              }
              else
              {
                 
                mm_ps_domain_possible_reest = FALSE;

                if (pmm_mode == PMM_CONNECTED)
                {
                  if ((gmm_state == GMM_DEREGISTERED) ||
                     (gmm_state == GMM_REGISTERED_INITIATED))
                  {
                    pmm_mode = PMM_DETACHED;
                    MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_DETACHED");
                  }
                  else
                  {
                    /* ---------------------------------------------------------------------
                    ** In UMTS the timer T3312 is reset and started with its initial value
                    ** when the MS goes from PMM-CONNECTED to PMM-IDLE mode (24.008 4.7.2.2)
                    ** --------------------------------------------------------------------- */
                    pmm_mode = PMM_IDLE;
                    MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_IDLE");
/*24.008:4.7.7.2 
The RAND and RES values stored in the mobile station shall be deleted and timer T3316, if running, shall be stopped:
-	if the mobile station enters PMM-IDLE mode (Iu mode only).*/
#ifdef FEATURE_NAS_REL11
                    if (is_umts_mode())
                    {
                      auth_reset_cache_memory(FALSE,NO_TIMER_EXPIRED,RRC_PS_DOMAIN_CN_ID) ;
                    }
#endif

                    if (gmm_t3312_timer_value != 0)
                    {
                      mm_start_timer( TIMER_T3312, gmm_t3312_timer_value );
                    }
                    mm_stop_timer( TIMER_T3319 );
                  }

                  /* --------------------------------------------------------------
                  ** send the GMMSM_CON_RELEASE_IND to SM. SM will coordiante with
                  ** RABM in putting an Active PS RAB in dormant state.
                  ** -------------------------------------------------------------*/
                  {
                    gmm_sm_send_con_release_ind(SIG_CONNECTION_RELEASE);
                  }

                  if (((message->cmd.hdr.message_id == (byte)RRC_REL_IND) ||
                       (message->cmd.hdr.message_id == (byte)RRC_ABORT_IND) ||
                       (message->cmd.hdr.message_id == (byte)RRC_CLOSE_SESSION_IND)) && 
                       (mm_cnm_is_active != TRUE) && (mm_sms_is_active == TRUE))
                  {
                    /* check if ABORT_MSG_RECEIVED will cause WMS to retry */
                    mm_send_mmcnm_rel_ind_ps( ABORT_MSG_RECEIVED, FALSE );
                  }

                }

                mm_sms_is_active = FALSE;

                /* -------------------------------------------------------------------------------
                ** When the MS receives an indication from the lower layers that the RRC conneciton
                ** has been released with cause "Directed signalling connection re-establishment,
                ** then the MS shall enter PMM-IDLE mode and initiate immediately a normal routing
                ** area update procedure (the use of normal or combined procedure depends on the
                ** network operation mode in the current serving cell) regardless whether the routing
                ** area has been changed since the last update or not.
                ** ------------------------------------------------------------------------------- */
                if ((pmm_mode == PMM_IDLE) &&
                    (((message->cmd.hdr.message_id == (int) RRC_REL_IND) &&
                      (message->cmd.rrc_rel_ind.rel_cause == RRC_DIRECTED_SIGNALLING_REESTABLISHMENT)) ||
                     ((message->cmd.hdr.message_id == (int) RRC_ABORT_CNF) &&
                      (message->cmd.rrc_abort_cnf.conn_rel_cause == RRC_DIRECTED_SIGNALLING_REESTABLISHMENT))) &&
                     (gmm_power_down_active == FALSE) &&
                    ((gmm_state !=  GMM_NULL) && (gmm_state !=  GMM_DEREGISTERED))
                   )
                {
                  /* --------------------------------------------------------------------
                  ** 24.008 4.7.4.1.4, 4.7.13.5: cases b.3
                  ** RR release in Iu mode (i.e. RRC connection release) with cause "Directed signalling connection re-establishment"
                  **  The routing area updating procedure shall be initiated followed by a rerun of the service request/detach 
                  **  procedure if the following condition applies:
                  **    i)The service request/detach procedure was not due to a rerun of the procedure due to 
                  **    "Directed signalling connection re-establishment".
                  ** -------------------------------------------------------------------- */
                  if(gmm_procedure_rerun_ongoing &&
                        ((gmm_state == GMM_SERVICE_REQUEST_INITIATED) ||
                         (gmm_state == GMM_DEREGISTERED_INITIATED)))
                  {
                    gmm_procedure_rerun_ongoing = FALSE;
                  }
                  else
                  {
                    if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
                    {
                      gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
                    }
                    else
                    {
                      gmm_ra_update_type = GMM_RA_UPDATING;
                    }

                    gmm_directed_signalling_reestablishment = TRUE;
                    gmm_processing_directed_signalling_reestablishment = TRUE;
                    gmm_procedure_rerun_ongoing = TRUE;

                    if ((gmm_state == GMM_DEREGISTERED_INITIATED) && (mm_timer_status[TIMER_T3321] == TIMER_ACTIVE))
                    {
                      gmm_detach_after_rau_pending = TRUE;
                    }
                 
                    if (gmm_state == GMM_SERVICE_REQUEST_INITIATED)
                    {
                      gmm_handle_service_request( message );
                    }            
                
                    gmm_initiate_routing_area_update();
                    gmm_directed_signalling_reestablishment = FALSE;

                  /* Store directed signalling cause such that NAS should initiate RAU procedure with cause "RRC_EST_CALL_RE_ESTABLISH" 
                  ** after camping on new RAI if current RAU attempt is initiated due to connection release
                  ** with cause directed signalling connection release. */

                    if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
                    {
                      gmm_cause_directed_signalling_reest = TRUE;
                    } 
                  }
                }
                else if(gmm_procedure_rerun_ongoing)
                {
                  gmm_procedure_rerun_ongoing = FALSE;
                }
                /* Check if there is a pending PLMN_LIST_REQ, if so send it.
                ** Send the LIST REQ only after ABORT_IND/CNF or REL_IND
                ** is received.
                */
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
                if ((mm_plmn_list_req_pending) && (!mm_rat_change_pending) &&
                   ((message->cmd.hdr.message_id == (byte) RRC_ABORT_IND) ||
                    (message->cmd.hdr.message_id == (byte) RRC_ABORT_CNF) ||
                    (message->cmd.hdr.message_id == (byte) RRC_REL_IND)))
                {
                  /* Send the PLMN LIST REQUEST Request to RRC */
                  mm_request_list_from_rr(&(mmcoord_pending_reg_list_req_message.cmd.mmr_plmn_search_req));
                }
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

                /* -------------------------------------------
                ** Treat as indications of lower layer failure
                ** ------------------------------------------- */
                if (gmm_state == GMM_REGISTERED_INITIATED)
                {
                  gmm_handle_gprs_attach( message );
                }
                else if (gmm_state == GMM_DEREGISTERED_INITIATED)
                {
                  gmm_handle_gprs_detach( message );
                }
                else if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
                {
                  if ((pmm_mode == PMM_IDLE) &&
                      (((message->cmd.hdr.message_id == (int) RRC_REL_IND) &&
                        (message->cmd.rrc_rel_ind.rel_cause == RRC_DIRECTED_SIGNALLING_REESTABLISHMENT)) ||
                       ((message->cmd.hdr.message_id == (int) RRC_ABORT_CNF) &&
                        (message->cmd.rrc_abort_cnf.conn_rel_cause == RRC_DIRECTED_SIGNALLING_REESTABLISHMENT))))
                  {
                    MSG_HIGH_DS_0(MM_SUB, "=MM= DIRECT SIGNALLING RE_ESTABLISHEMENT - RAU INITIATED");
                  }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
                  else
                  {
                    gmm_handle_routing_area_update( message );
                  }
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
                }
                else if (gmm_state == GMM_SERVICE_REQUEST_INITIATED)
                {
                  gmm_handle_service_request( message );
                }
                else if (gmm_state == GMM_REGISTERED)
                {
                  if (gmm_substate == GMM_IMSI_DETACH_INITIATED)
                  {
                    gmm_handle_gprs_detach( message );
                  }
                  else if (mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf)
                  {
                    if ((mm_reg_waiting_for_reg_cnf) &&
                        (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                       ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                        ||
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
                       )&&
                        (mm_serving_plmn.info.plmn_service_capability ==
                           SYS_SRV_CAPABILITY_CS_PS_SERVICE))
                    {
                      /* ---------------------------------------------------------------
                      ** Immediately send MM registration status to REG and send the GMM
                      ** registration status later in a MMR_SERVICE_IND primitive
                      ** --------------------------------------------------------------- */
                      mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                      mm_send_mmr_reg_cnf();

                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;
                    }

                  }
                }
                else if (gmm_state == GMM_DEREGISTERED)
                {
                  if (mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf)
                  {
                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)   &&
                       ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                        ||
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
                       )&&
                       (mm_serving_plmn.info.plmn_service_capability ==
                            SYS_SRV_CAPABILITY_CS_PS_SERVICE))
                    {
                      /* ---------------------------------------------------------------
                      ** Immediately send MM registration status to REG and send the GMM
                      ** registration status later in a MMR_SERVICE_IND primitive
                      ** --------------------------------------------------------------- */
#ifdef FEATURE_NAS_ECALL
                      if (mm_idle_substate == MM_ECALL_INACTIVE)
                      {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                      }
#endif
                      mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                      mm_send_mmr_reg_cnf();

                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;

                      if ((mmsim_sim_valid( SYS_SRV_DOMAIN_PS_ONLY )) &&
                          (gmm_substate == GMM_NORMAL_SERVICE)
#ifdef FEATURE_NAS_ECALL
                           &&
                          (mm_ecall_mode_type == MMR_ECALL_MODE_NORMAL) 
#endif 
                         )
                      {
                        /* -------------------------------
                        ** Proceed to GMM Attach procedure
                        ** ------------------------------- */
                        t3310_timeout_counter = 0;
                        MSG_HIGH_DS_1( MM_SUB, "=MM= T3310 timeout counter = %d", t3310_timeout_counter );
                        gmm_initiate_gprs_attach();
                      }
                    }
                  }
                }

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)                
                if (gmm_gprs_detach_pending)
                {
                  if (mmcoord_pending_reg_message.cmd.hdr.message_set != 0)
                  {
                    gmm_initiate_gprs_detach( &mmcoord_pending_reg_message );
                  }
                  else
                  {
                    MSG_HIGH_DS_0( MM_SUB, "=MM= Unable to proceed with GPRS Detach" );
                  }
                }
                else
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/                         
                if ((mm_serv_change_pending)
#ifdef FEATURE_SILENT_REDIAL_AFTER_RA_LA_MISMATCH
                     &&(!((gmm_state == GMM_DEREGISTERED_INITIATED)&&(message->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_LAI_RAI_CHANGED)))
#endif
                   )
                {
                  mm_change_service_domain();
                  mm_serv_change_pending = FALSE;
 
                  if (!mm_reg_waiting_for_reg_cnf)
                  {
                    mmr_service_ind.service_state = mm_serving_plmn.info;
                    mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                    MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND" );
                    mm_send_mmr_service_ind( &mmr_service_ind );
                  }
                }  
                else if (mm_rat_change_pending)
                {
                  mm_stop_current_rat();
                }
                else if (gmm_gprs_attach_pending)
                {
                  gmm_initiate_gprs_attach();
                }
                /* if the cause in abort indication is rai/lai changed then we should not start rau
                immediately. RRC will send service indication with lai/rai chaged and rau should be started 
                on service indication */
                else if ( (((message->cmd.hdr.message_id == (byte) RRC_ABORT_IND)
#ifdef FEATURE_SILENT_REDIAL_AFTER_RA_LA_MISMATCH
                             &&(message->cmd.rrc_abort_ind.abort_cause != RRC_ABORT_LAI_RAI_CHANGED)
#endif
                            ) ||
                            (message->cmd.hdr.message_id == (byte) RRC_ABORT_CNF) ||
                            (message->cmd.hdr.message_id == (byte)   RRC_REL_IND) ) &&
                          (gmm_ra_update_pending) )
                {
                  gmm_ra_update_type = gmm_pending_ra_update_type;
                  gmm_initiate_routing_area_update();             
                }
                else if ( ( (message->cmd.hdr.message_id == (byte) RRC_CLOSE_SESSION_IND) ||
                            (message->cmd.hdr.message_id == (byte) RRC_CLOSE_SESSION_CNF) ) &&
                          ( (gmm_ra_update_pending)) )
                {
                /**************************************************
                 If TIMER_T3311 or TIMER_T3302 is running, it means 
                 RAU was initiated and it was not successful.RAU
                 is pended,may be not because of GMM is suspened but
                 because of MM is not MM_IDLE.
                ***************************************************/
 
                  if(((mm_timer_status[TIMER_T3311] == TIMER_ACTIVE) ||
                           (mm_timer_status[TIMER_T3302] == TIMER_ACTIVE)) &&
                          !((gmm_rai_changed( gmm_previous_registration_serving_rai.location_area_code,
                                     gmm_previous_registration_serving_rai.routing_area_code,
                                     mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                            (gmm_plmn_changed(gmm_previous_registration_serving_rai.plmn_id,
                                      mm_serving_plmn.info.plmn)))
                         )
                 {
                   MSG_HIGH_DS_0(MM_SUB, "=MM=  TIMER_T3311 or  TIMER_T3302 is active");
                 }

                 /************************************************************
                  When MM state is not MM_IDLE and gmm_update_status is GMM_GU1_UPDATED,
                  NORMAL RAU might have initiated,got sussessful.Combined RAU is pended 
                  becuase of MM being not IDLE but not because of GMM is suspended.
                  ************************************************************/

                 else if(mm_state != MM_IDLE && 
                         ((gmm_update_status == GMM_GU1_UPDATED) &&
                          (!gmm_rai_changed(gmm_stored_rai.location_area_code, gmm_stored_rai.routing_area_code, 
                                 mm_serving_plmn.lac, mm_serving_plmn.rac)) &&
                          (!gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn))
                         )
#if defined(FEATURE_LTE)
                /* We need to trigger RAU if tin is guti as this is because of interrat
                from LTE to GSM DTM cell */
                         &&(emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI)
#endif
                        )
                        {
                          MSG_HIGH_DS_1(MM_SUB, "=MM=  MM state is %d  No need to intiaite RAU", mm_state);
                        }

                 else  
                 {
                    gmm_ra_update_type = gmm_pending_ra_update_type;
                    gmm_initiate_routing_area_update();
                 }
 
                }
              }
      }
          break;

#ifdef FEATURE_WCDMA
        case RRC_PAGE_IND:
          gmm_process_rrc_page_ind( message );
          break;
#endif /*FEATURE_WCDMA*/

        case RRC_ACTIVATION_IND:
          gmm_handle_connection_management_activation_ind( message );
          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected RRC cmd %d", message->cmd.hdr.message_id, 0,0 );
      } /* end switch (message->cmd.hdr.message_id) */
      break;

#ifdef FEATURE_GSM_GPRS
    case MS_MM_RR:
      switch (message->cmd.hdr.message_id)
      {
        case RR_REL_IND:
        case RR_ABORT_IND:
        case RR_ABORT_CNF:
          if ((mm_interrat_state == MM_UMTS_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS) &&
              (mm_serving_plmn.info.reg_service_domain !=  SYS_SRV_DOMAIN_CS_ONLY) &&
              (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE))
          {
            gmmllc_reinit(GMM_STANDBY_STATE) ;

            gmm_rau_is_required = TRUE ;

            mm_interrat_state = MM_INTERRAT_CHANGE_STATE_TYPE_LO ;
          }

          if (gmm_state == GMM_REGISTERED)
          {
            if (mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf)
            {
              if ((mm_reg_waiting_for_reg_cnf) &&
                  (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)   &&
                  ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2) 
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                      ||(mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
                  )
                  &&
                  (mm_serving_plmn.info.plmn_service_capability ==
                     SYS_SRV_CAPABILITY_CS_PS_SERVICE))
              {
                /* ---------------------------------------------------------------
                ** Immediately send MM registration status to REG and send the GMM
                ** registration status later in a MMR_SERVICE_IND primitive
                ** --------------------------------------------------------------- */
                if((((gmm_rau_attempt_counter < 5) && (mm_timer_status[TIMER_T3311] == TIMER_ACTIVE)) ||
                     gmm_ra_update_pending ||
                     (mm_timer_status[TIMER_T3330] == TIMER_ACTIVE)) &&
                     (mm_lu_attempt_counter >=4))
                {
                  MSG_HIGH_DS_0(MM_SUB, "=MM= RAU Attempt counter is less than the max value");
                }
                else
                {
                  mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                  mm_send_mmr_reg_cnf();

                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf   = FALSE;
                }
              }

            }
          }
          else if ((gmm_state == GMM_DEREGISTERED) ||
                   (gmm_state == GMM_REGISTERED_INITIATED))
          {
            if ((mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf) ||
                (!mm_reg_waiting_for_reg_cnf))
            {
              if ((mm_reg_waiting_for_reg_cnf) &&
                  (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2) 
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                      ||(mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
                  )
                  &&
                  (mm_serving_plmn.info.plmn_service_capability ==
                     SYS_SRV_CAPABILITY_CS_PS_SERVICE))
              {
                /* ---------------------------------------------------------------
                ** Immediately send MM registration status to REG and send the GMM
                ** registration status later in a MMR_SERVICE_IND primitive
                ** --------------------------------------------------------------- */
                mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;

                if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_NO_SRV) &&
                    (gmm_gprs_attach_pending) &&
                    (mmcoord_mmr_reg_cnf.cause == MMR_CAUSE_NOT_SPECIFIED))
                {
                  mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                }

                if((((gprs_attach_attempt_counter < 5) && (mm_timer_status[TIMER_T3311] == TIMER_ACTIVE)) ||
                    gmm_gprs_attach_pending ||
                    (mm_timer_status[TIMER_T3310] == TIMER_ACTIVE))&&
                    (mm_lu_attempt_counter >=4))
                {
                   MSG_HIGH_DS_0(MM_SUB, "=MM= Attach Attempt counter is less than the max value");
                }
                else
                {
                  mm_send_mmr_reg_cnf();

                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf   = FALSE;
                }
              }

              if ((mmsim_sim_valid( SYS_SRV_DOMAIN_PS_ONLY )) &&
                  (mm_serving_plmn.ms_op_mode != SYS_SRV_DOMAIN_CS_ONLY))
              {
                if ((gmm_state == GMM_DEREGISTERED) &&
                    (gmm_substate == GMM_NORMAL_SERVICE) &&
                    (!mm_serv_change_pending) &&
                    (gmm_gprs_attach_pending))
                {
                  /* -------------------------------
                  ** Proceed to GMM Attach procedure
                  ** ------------------------------- */
                  t3310_timeout_counter = 0;
                  MSG_HIGH_DS_1( MM_SUB, "=MM= T3310 timeout counter = %d", t3310_timeout_counter );
                  gmm_initiate_gprs_attach();
                }
                else if ((gmm_state == GMM_REGISTERED_INITIATED) && 
                         (mm_timer_status[TIMER_T3310] != TIMER_ACTIVE) &&
                         ((gmm_substate == GMM_SUSPENDED) || (gmm_is_suspended))
#ifdef FEATURE_GSM_DTM
                         &&
                         (mm_serving_plmn.dtm_supported == FALSE)
#endif                         
                         )
                {
                  gmm_gprs_attach_pending = TRUE;
                }
              }
              else if (mm_reg_waiting_for_reg_cnf)
              {
                mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                mm_send_mmr_reg_cnf();
                mm_reg_waiting_for_reg_cnf = FALSE;
                mm_ready_to_send_reg_cnf   = FALSE;
              }
            }
          }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
          if (gmm_gprs_detach_pending)
          {
            if (mmcoord_pending_reg_message.cmd.hdr.message_set != 0)
            {
              gmm_initiate_gprs_detach( &mmcoord_pending_reg_message );
            }
            else
            {
              MSG_HIGH_DS_0( MM_SUB, "=MM= Unable to proceed with GPRS Detach" );
            }
          }
          else 
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/            
          if ((gmm_ra_update_pending) && (mm_state == MM_IDLE) && (gmm_detach_after_rau_pending))
          {
            gmm_ra_update_type = gmm_pending_ra_update_type;
            gmm_initiate_routing_area_update();
          }
          else if (mm_serv_change_pending)
          {
            if((gmm_state == GMM_DEREGISTERED_INITIATED && mm_timer_status[TIMER_T3321] == TIMER_ACTIVE) 
						||(gmm_detach_after_rau_pending && gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED ))
            {
              MSG_HIGH_DS_0( MM_SUB, "=MM= Pending service domain change as PS Detach is in progress");
            }
            else
            {
              mm_change_service_domain();
              mm_serv_change_pending = FALSE;

              if (!mm_reg_waiting_for_reg_cnf)
              {
                mmr_service_ind.service_state = mm_serving_plmn.info;
                mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND");
                mm_send_mmr_service_ind( &mmr_service_ind );
              }
            }
		  }
          else if (mm_rat_change_pending)
          {
            mm_stop_current_rat();
          }
          else if (gmm_gprs_attach_pending)
          {
            gmm_initiate_gprs_attach();
          }
          else if ((gmm_ra_update_pending) && (mm_state == MM_IDLE))
          {
 		    if(gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
            {
		      MSG_HIGH_DS_0( MM_SUB, "GMM= Already RAU in progress,RAU should not be initiated again");
            }
            else
			{
			  gmm_ra_update_type = gmm_pending_ra_update_type;
              gmm_initiate_routing_area_update();
			}  
          }

          else if ((gmm_rau_is_required) && (gmm_ps_data_available) &&
                   ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
                    (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)) &&
                   ((gmm_substate != GMM_SUSPENDED ) && (!gmm_is_suspended)) &&
                   (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE))
          {
            /* Initiate RAU */
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
            {
              gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
            }
            else
            {
              gmm_ra_update_type = GMM_RA_UPDATING;
            }

            gmm_initiate_routing_area_update() ;

            gmm_ps_data_available = FALSE ;
          }
          break;

        case RR_ACTIVATION_IND:
            gmm_handle_connection_management_activation_ind( message );
            break;
        
#ifdef FEATURE_DUAL_SIM
        case RR_PS_ABORT_CNF:
#ifdef FEATURE_REL6_PS_FOP
           gmm_ps_fop_resp = PS_FOP_NW_GRANTED;
#endif
           if(is_gsm_mode())
           {
             if (gmm_state == GMM_REGISTERED_INITIATED)
             {
               gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH );
               gmm_set_state( GMM_DEREGISTERED );
               mm_start_timer( TIMER_GMM_RADIO_RETRY, 4000 );
             }
             else if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
             {
               if ((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                        gmm_stored_rai.routing_area_code,
                          mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                      (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
                   (gmm_update_status == GMM_GU1_UPDATED)
 #ifdef FEATURE_LTE
                  && (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI)
#endif 
                )
               {
                 gmm_set_substate( GMM_NORMAL_SERVICE );
               }
               else
               {
                 gmm_update_status = GMM_GU2_NOT_UPDATED;
                 mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                         gmm_stored_ptmsi_signature,
                                                     gmm_stored_rai,
                                                     gmm_update_status );
           
                 if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                    (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                    ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
                    (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)))
                 {
                   gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                  SYS_SRV_DOMAIN_CS_ONLY,
                                                  FALSE,
                                                  SYS_SRV_STATUS_SRV,
                                                  TRUE );
                 }
                 else
                 {
                   gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                               SYS_SRV_DOMAIN_NO_SRV,
                               FALSE,
                               SYS_SRV_STATUS_LIMITED,
                               TRUE );
                 } 
           
                 gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE );
               }
           
               gmm_set_state( GMM_REGISTERED );
               mm_start_timer( TIMER_GMM_RADIO_RETRY, 4000 );
             }
           }
           gmm_sm_send_con_release_ind(SIG_CONNECTION_RELEASE_UE_INITIATED);
          break;
#endif

        default:
          MSG_ERROR_DS_1( MM_SUB, "=MM= Unexpected RR primitive (id=%d)", message->cmd.hdr.message_id );
          break;
      }
      break;
#endif /* #ifdef FEATURE_GSM_GPRS */

    case MS_TIMER:
      switch (message->cmd.timer_expiry.timer_id)
      {
        case TIMER_T3317:
          if (gmm_state == GMM_SERVICE_REQUEST_INITIATED)
          {
            gmm_handle_service_request( message );
          }
          break;

        case TIMER_FORBID_TDS_RESELECTION:
          {
            gmm_controls_resel_dec_mt_ps_call = FALSE;
          }
          break;

        case TIMER_T3319:
          break;
          
        case TIMER_T3340:  
        case TIMER_PS_CONN_REL:
         /**********************************************************
         *24.008 4.7.1.9 Upon expiry of T3340, the MS shall release* 
         *the established PS signalling connection                 *
         **********************************************************/
         if (is_umts_mode())
         {
#if defined FEATURE_WCDMA || defined FEATURE_TDSCDMA
            rrc_cmd_type *rrc_cmd = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );

            if (rrc_cmd == NULL)
            {
              MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
            }

            rrc_cmd->cmd_hdr.cmd_id = RRC_CLOSE_SESSION_REQ;
            rrc_cmd->cmd.close_sess_req.cn_domain_id = RRC_PS_DOMAIN_CN_ID;

            rrc_cmd->cmd.close_sess_req.close_session_cause = RRC_OTHERS;
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending RRC_CLOSE_SESSION_REQ");

            send_cmd_to_rrc( rrc_cmd, mm_serving_plmn.info.active_rat );
#endif
          }
          break;
          
        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected timer message %d",
                      message->cmd.timer_expiry.timer_id, 0,0 );
      }
      break;

    default:
      MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message set %d", message->cmd.hdr.message_set, 0,0 );
  }
} /* end gmm_handle_connection_management() */

/*===========================================================================

FUNCTION MM_SEND_MMCNM_REL_IND_PS

DESCRIPTION
  This function sends a mmcnm_rel_ind message to the cnm for the PS Domain.
  The variable "mm_sms_is_active" is only used for tracking if the SMS sent on PS Domain.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_mmcnm_rel_ind_ps
(
   byte    reject_cause_value,
   boolean reestablishment_possible
)
{
   mmcnm_rel_ind_T mmcnm_rel_ind;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   memset(&mmcnm_rel_ind,0x00,sizeof(mmcnm_rel_ind_T));
   mmcnm_rel_ind.header.message_set = MS_CC_MM_PS;
   mmcnm_rel_ind.header.message_id  = MMCNM_REL_IND;

   mmcnm_rel_ind.rel_cause      = reject_cause_value;
   mmcnm_rel_ind.reest_possible = reestablishment_possible;

   PUT_IMH_LEN( sizeof(mmcnm_rel_ind_T) - sizeof(IMH_T),
                &mmcnm_rel_ind.header );

   MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent MMCNM_REL_IND for PS");
   mm_send_message( (IMH_T *)&mmcnm_rel_ind, GS_QUEUE_MN_CM );

   /* ----------------------------------------------
   ** Flag that the CNM request is no longer pending
   ** ---------------------------------------------- */
   mm_sms_is_active = FALSE;
   mm_held_cnm_service_req.present = FALSE;
}


#ifdef FEATURE_MBMS
#error code not present
#endif

