/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     GMM Registration Management (GMMREG.C)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmmreg.c_v   1.23   18 Jul 2002 11:40:06   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmmreg.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------       
05/18/15   sn      Using previous LA information if waiting for EST_CNF from RRC and received NO_SERVICE from RRC
11/28/12   am      Fixing compilation warning 
10/01/12   am      Back off timer code
04/17/12   gps     doing local detach if service indication with RA change received during DDS procedure.       
09/12/11   HC      Replacing the #include "sm.h" with #include "sm_v.h" 
                   as a part of CMI activity
12/21/11   gps     FREE FLOAT FEATURE changes
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
11/01/11  rajesh   Setting the mm substates and service status correctly on irat from LTE to GSM.
10/01/11  abhi     resetting mm_csfb_state if it is lu_pending and combined RAU/attach is triggered. 
02/09/11   abhi     After upper layer change from LTE to UMTS if TIN is GUTI then start RAU irrespective of 
                               T3302/T3311 running.
07/13/11   rajesh   Reset the "sg_association_to_be_established" variable whenever combined attach
                             is initiated.
07/13/11   rajesh   Made code changes to start LAU after camping on UMTS when T3302 is running.
07/12/11   sla        When combined RAU is aborted,update the registered service domain as service
                              if GMM update status is updated and stored RAI and camped RAI are same
07/11/11   sla        changes made to change the substate to norml service in nmo1.
06/30/11   rajesh   Changes made for implementing PS detach timer in NAS.
04/13/10   MNK     Minor name changes 
01/26/01   jca     Created module
05/14/01   jca     Added detailed code to functions related to attach procedures
05/23/01   jca     Added code for functions related to RAU and GPRS detach.
05/25/01   jca     Added SM task interface.
06/08/01   jca     Added support for integrity protection.
06/13/01   jca     Added some extra processing when RRC_CLOSE_SESSION_REQ is sent
                   to reflect removal of RRC_CLOSE_SESSION_CNF primitive.
08/03/01   jca     Added code for some missing GPRS Attach requirements.
08/15/01   ab      Renamed CM to CNM.
09/12/01   jca     Added GMMSM_CON_RELEASE_IND and RRC_PAGING_CTRL_REQ processing.
11/06/01   jca     Now tracking PS session status and integrity status independently.
11/16/01   jca     Now setting T3312 timer value to correct field from Attach Accept
                   and Routing Area Update Accept messages.
02/19/02   jca     Modified MMR_SERVICE_IND.
02/20/02   jca     Set GMM state back to REGISTERED after RAU ACCEPT message received.
04/22/02   ks      Stopping TIMER_T3321 after receiving DETACH_Accept message,
                   Added logic to wait until Connection release after receiving
                   Detach Accept.  Added code to send RELEASE_IND to SM task.
04/30/02   ks      Added nreg_release_pending check in gmm_handle_gprs_detach.
05/16/02   jca     Now accounting for case where PLMN search in effect when
                   service restored on RA where mobile is registered.
06/05/02   ks      Added support for Dual mode operation.
07/06/02   vdr     Multiple changes in order to support GMM-LLC interface feature
07/12/02   ks      Setting the default values to Available/Equivalent PLMN list
                   info in mmr_reg_cnf primitive.
07/18/02   vdr     Renamed FEATURE_GPRS to be FEATURE_GSM_GPRS compilation flag
07/22/02   ks      Added support for RAT change procedure upon request from REG.
08/02/02   ks      Added support for new reject cause #15 NO_SUITABLE_CELLS_IN_LA
                   Now setting proper cause in mmr_reg_cnf & mmr_service_ind.
08/10/02   mks     Obsoleted RRC_PAGING_CTRL_REQ as the information conveyed by
                   this primitive is now sent to RRC through RRC_SIM_INSERTED_REQ
                   & RRC_SIM_UPDATE_REQ.
09/06/02   vdr     Changed GMM behavior during processing of the ATTACH ACCEPT and ROUTING
                   AREA UPDATE ACCEPT messages when MS is in GSM mode
09/23/02   ks      Sending the MMR_REG_CNF to the REG instead of a MMR_SERVICE_IND
                   when mm_reg_waiting_for_reg_cnf is TRUE in gmm_process_service_ind()
09/26/02   ks      Setting the mm_service_state in the gmm_process_service_ind().
09/30/02   ks      Replaced the GMMSM_ERROR_IND with GMMSM_CON_RELEASE_IND
                   and GMMSM_RELEASE_IND with GMMSM_GPRS_RELEASE_IND.
10/07/02   jca     Changes to support CS/PS combined attach procedures.
10/09/02   vdr     Now the GMM provides an information in order to retrieve CKSN
                   according to current mode & domain identity
10/09/02   jca     Fixed some bugs related to RR(C)_SERVICE_IND processing.
10/10/02   jca     Added preliminary support for PS/CS combined detach procedures.
11/06/02   jca     Modified code to set GMM serving PLMN status through calls to
                   new function gmm_update_mm_serving_plmn_info().
11/07/02   jca     Changes related to processing ROUTING_AREA_UPDATE_REJECT cause codes.
                   In addition, now setting gmm_previous_service_state on receipt
                   of RR_PLMN_SELECT_CNF.
11/08/02   cd      Added support for AC blocking
11/12/02   jca     Added support for RRC_CLOSE_SESSION_CNF.
12/05/02   jca     In GPRS mode, immediately set GMM state to DEREGISTERED on
                   receipt of DETACH ACCEPT message.
12/10/02   jca     Now sending MMR_SERVICE_IND when MS returns to GPRS coverage
                   while operating in CS/PS mode.
12/12/02   ks      Passing the SYS_SRV_DOMAIN_PS_ONLY in mmsim_sim_valid ()
12/17/02   cd      Setting GMM substate to NORMAL_SERVICE after access class blocking
                   is removed
01/10/03   jca     Changes to support CS<->PS<->CS/PS mode switching.
01/10/03   vdr     Changes of CKSN determination
01/22/03   vdr     Now SMS Radio Priority Level and Cell Notification Indicator are
                   set correctly according to the values are contained in Attach Accept
                   and RAU Accept messages
01/27/03   jca     Changes to support PS_ONLY<->CS/PS combined mode switching.
                   Changes to support IMSI Detach via Combined GPRS Detach procedure.
02/07/03   jca     Implemented Network Initiated GPRS Detach procedure as per 24.008 CR 405.
02/13/03   jca     Now performing a Routing Area Update procedure on PLMN change
                   instead of GPRS Attach procedure.
02/14/03   jca     Now performing a GPRS Attach when moving from a CS-only cell
                   to a cell capable of both CS and PS services.
02/27/03   vdr     Added processing of the UMTS to GSM RAT changes
02/27/03   ks      support for offline-d for automation, SYS_STOP_MODE_REASON_OFFLINE
                   behaves same as SYS_STOP_MODE_REASON_POWER_OFF.
03/04/03   vdr     Changed some code in order to resume LLC Tx after RAU Complete message
                   has been sent
03/13/03   vdr     Featured out PS Continuity RAU initiation (FEATURE_GSM_GPRS)
03/13/03   jca     Fixed problem in process_service_ind() where previous_serving_rai
                   not set correctly after loss of (GMM) service.
03/20/03   jca     Implemented missing requirement from 24.008 section 4.7.2.2.
03/25/03   jca     GPRS Attach and RA Update blocked if no GPRS service available.
03/27/03   cd      Setting the gmm_substate to GMM_UPDATE_NEEDED when access class
                   blocking is detected on a routing area change.
                   Setting GMM substate to NORMAL_SERVICE after a successful RAU.
03/31/03   jca     Added code to delay GPRS Detach if IMSI Detach in-progress.
04/01/03   ks      Added a check fir message_id == MMR_STOP_MODE_REQ before setting
                   the gmm_detach_type in gmm_initiate_gprs_detach() function.
04/03/03   cd      Added support for equivalent PLMN list
04/08/03   jca     Now sending MMR_REG_CNF if DETACH_REQUEST received in
                   REGISTERED_INITIATED state and REG waiting for reg_cnf.
                   Now setting LU type to Periodic Updating when MM LU procedure
                   initiated by expiration of T3312 timer.
04/11/03   jca     Now make sure to carry out periodic RAU processing regardless
                   of the network operation mode.
                   Now making sure to set GMM substate to NORMAL_SERVICE when
                   GPRS service restored and GMM state set to REGISTERED.
04/11/03   jca     Missed setting GMM substate to LIMITED_SERVICE for a couple
                   of DETACH REQUEST gmm_cause cases.
04/11/03   vdr     Modified filtering conditions for GRR’s RR_SERVICE_IND
04/14/03   jca     Added code to pend RAU if MM state != IDLE and GSM mode active.
04/15/03   cd      Now setting LU type to IMSI attach when MM LU procedure is
                   initiated by a combined GPRS_ATTACH procedure rejection with
                   reject_cause GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN
04/22/03   mks     Reset mm_serving_plmn.info.ps_data_suspend flag upon resumption
                   of packet data services.
04/29/03   jca     Added support for change of network mode of operation.
04/30/03   jca     Combined RAU initiated only if MM state is set to MM_IDLE
                   (as per 24.008 CR 477).
05/01/03   jca     Only perform a MM LU update when T3312 expires and GMM service
                   not available if the previous network op mode was NMO1.
05/07/03   jca     Only send Combined RAU if ms_op_mode == SYS_SRV_DOMAIN_CS_PS.
05/13/03   jca     Now setting LU type to "IMSI Attach" when MM registers after
                   combined attach fails due to "GPRS service not available".
05/23/03   jca     Make sure to update mm_serving_plmn_info when GMM deregisters.
                   Added support for PS on demand during CS active connection.
05/29/03   jca     Corrected bugs found when running GPRS test case 44.2.3.2.3.3.2.
05/29/03   mks     Corrected gmm_process_service_ind() to forward loss of service
                   from RR/RRC to REG when ms_op_mode is CS+PS or PS_ONLY and
                   GPRS services have been invalidated.
06/04/03   vdr     Now RR_SERVICE_IND is routed to the GMM-LLC interface only when
                   MM is in IDLE state
06/04/03   ks      Added support for RRC_MODE_CHANGE_CNF for MODE_CHANGE
                   support for UMTS.
06/09/03   jca     Force GRR back to 51 channel mode when ATTACH REJECT indicates
                   only CS allowed on combined GPRS Attach (NMO1).
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
06/16/03   jca     Force GRR back to 51 channel mode when RAU REJECT or DETACH REQUEST
                   message indicates only CS allowed on when operating in NMO1.
06/18/03   jca     When timer T3310 expires do not resend GPRS Attach Request if GMM
                   service is not available.
                   Set detach type to non-combined if only PS side is registered when
                   operating with MS op mode set to CS+PS in network operation mode 1.
                   Added missing requirements from 24.008 section 4.7.3.2.5.
06/19/03   ks      removed the check for cell_change ind while routing RR_SERVICE_IND
                   to GMM-LLC module.
07/09/03   vdr     Now updating glif_ra_changed_rau_completed_ra upon reception of the
                   GPRS Attach Accept message
07/14/03   ks      Removed the rau_failed variable as it is no longer needed. Sending
                   the SERVICE_IND to REG after the RAU is completed.
07/16/03   jca     Added additional checks on blocking restrictions for initiation
                   of GMM procedures including check for GPRS SUSPENDED.
07/16/03   mks     Added support for PS service continuity due to loss of service
                   during Inter RAT changes.
07/17/03   jca     Now unconditionally pass RR_SERVICE_IND to GMM-LLC interface module.
07/18/03   jca     Fixed bug where two MMR_SERVICE_INDs were being generated
                   on the same GMM event.
07/23/03   ks      Modified the earlier changes, so that only one RR_SERVICE_IND
                   is sent but also make sure the Data Suspend flag is handled properly.
07/29/03   jca     Fixed PS Attach-on-demand problem related to clearing of the
                   mm_serv_change_pending boolean flag.
07/30/03   jca     If RA update type is RA_UPDATING do not look for COMBINED_RA_LA
                   accept while operating in NMO1.
07/30/03   jca     Only block GPRS Attach when mm_state != IDLE when in GSM mode.
07/30/03   jca     Clear mm_serv_change_pending flag when changing service domain.
                   Block GPRS Attach in WCDMA mode while MM performing LU procedure.
07/31/03   cd      Corrected processing of ePLMN list when receiving an ATTACH_REJECT
                   or RAU_REJECT msg
08/01/03   jca     Added new substate GMM_NULL_SUBSTATE.
08/01/03   vdr     Featured out GSM GPRS related vars (FEATURE_GSM_GPRS)
08/05/03   mks     Now setting the correct cause in the MMR_REG_CNF when the Network
                   rejects RAU with PLMN_NOT_ALLOWED as the reason.
08/06/03   mks     GPRS detach procedure is now aborted upon expiration of T3321 for
                   5 times.
08/13/03   jca     Now completing Detach procedure after successful RAU if GMM
                   is waiting for Detach Accept when a SERVICE_IND is received
                   which indicates change of RAI (GCF TC 44.2.2.1.8).
08/14/03   mks     Now network initiated detach procedure with detach type IE
                   "reattach not required" is handled for all GMM states.
09/03/03   jca     Added code to reset gmm_ra_update_type if SERVICE_IND indicates
                   RAI change and GMM state equals GMM_ROUTING_AREA_UPDATING_INITIATED.
09/08/03   mks     Now resetting the T3330_timeout_counter upon receiving Routing Area
                   Update accept  message  from the network.  This resolves the RAU
                   issues seen when the network does not respond to the first RAU
                   attempt from MS and eventually accepts the RAU on a subsequent
                   attempt.
09/10/03   vdr     Now using current value of T3302 instead of it's default
09/10/03   jca     Delay sending MMR_REG_CNF on receipt of RAU ACCEPT message
                   if gmm_detach_after_rau_pending is set to TRUE.
09/19/03   ks      Added Debug statements when Attach or RAU reject is received.
09/22/03   mks     Now suspending the data in GPRS mode when Routing Area Update
                   is in progress. Corrected the GMM behavior per the spec so as
                   to suspend data when MS is in GMM-REGISTERED.ATTEMPTING-TO-UPDATE.
09/25/03   ks      Changing GMM substate to Limited upon a REG_REQ requesting Limited
                   Service. Not allowing RAU,Attach,Detach during Limited substate.
09/25/03   jca     Fixed bug where ps_data_suspend was not being reset back
                   to FALSE after completion of RA update procedure.
10/03/03   jca     Added support for network selection mode
                   SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION.
                   Now making sure to change GMM substate from LIMITED to
                   NORMAL following a successful GPRS Attach.
09/26/03   mks     Now sending MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED as the cause
                   to REG in REG_CNF and MMR_SERVICE_IND when attach or routing
                   area update is rejected with cause NO_SUITABLE_CELLS_IN_LA.
10/17/03   jca     Do not overwrite mm_serving_plmn_info.reg_service_domain when
                   camping on a forbidden PLMN/LAI, only the service_status.
10/22/03   jca     Added support for change of network mode of operation to/from NMO III.
10/22/03   cd      Added check for EPLMN when a new PLMN is reported in a service_ind
10/31/03   cd      Corrected MSG_HIGH when a new plmn is detected
11/04/03   jca     Treat ACTIVATION_IND as implicit ABORT_IND when in the
                   process of setting up an RR connection.
11/24/03   jca     Allow LU attempt when camped on a Forbidden PLMN/LAI and
                   the network selection mode is set to MODE_MANUAL as per
                   GCF test case 44.2.1.1.4.3.2.
11/25/03   jca     Ensure timer T3311 is stopped when initiating GPRS Attach
                   or Routing Area Update procedures.
12/03/03   vdr     Now deleting P-TMSI signature upon reception of the Detach
                   Accept message or when MO detach is timed out in case when
                   MO Detach Accept message contained P-TMSI Signature
12/04/03   jca     Fixed bug where MMR_STOP_MODE_REQ was not being processed
                   in GMM_REGISTERED.LIMITED_SERVICE state.
12/11/03   jca     No longer set substate to LIMITED_SERVICE when RR/RRC report
                   change of PLMN in SERVICE_IND.
                   Restore GMM substate to ATTEMPTING_TO_ATTACH when MMR_REG_REQ
                   processing is completed and MS camps on the same cell where it
                   had just previously camped (i.e., MMR_REG_REQ to the same PLMN).
12/16/03   jca     Corrected bugs when user changes service setting from PS_ONLY
                   to CS+PS and the network op mode equals NMO III.
12/23/03   jca     Tightened logic on setting/clearing of ps_data_suspend flag.
01/20/04   jca     Ensure T3212 timer is started when gprs_attach_attempt_counter
                   exceeds 5 and gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH.
01/22/04   vdr     Now changing substate before changing state upon reception of
                   RAU ACCEPT & REJECT messages
01/22/04   jca     Do not initiate GPRS Detach procedure if GPRS service not available.
02/20/04   vdr     Added processing required by READY timer negotiation procedure
03/19/04   jca     Treat SERVICE_IND indicating a loss of (GPRS) service as a lower
                   layer failure when GMM is set to state REGISTERED_INITIATED.
03/22/04   vdr     Now changing substate before changing state upon reception of
                   ATTACH ACCEPT or REJECT message or GMM is timed out when it waits
                   for above messages.
03/24/04   jca     Do not overwrite mm_serving_plmn.info.service_status when a
                   RR(C)_SERVICE_IND is received indicating no GPRS service.
03/25/04   jca     Now calling mm_rat_handle_srv_status_update() from GMM state machine
                   context to help ensure that ps_data_suspend flag is cleared correctly
                   and PDP activation requests succeed when expected.
03/25/04   ks      Now rejecting the pending connection requests from RABM/SM/CC
                   When RA or PLMN has changes as GMM would proceed with RAU.
04/01/04   jca     Now checking if acquired PLMN is on the GPRS Forbidden PLMN list
                   before proceeding with the GPRS Attach procedure.
04/01/04   jca     On getting the 5th consective Combined GPRS Attach Reject,
                   immediately initiate an LAU and try to obtain CS service.
04/02/04   jca     Set state to GMM_REGISTERED.LIMITED_SERVICE (rather than
                   GMM_DEREGISTERED) for RAU reject causes #13 and #15.
04/08/04   vdr     Added follow-on request processing when GMM is sending RA update
                   request message during G2W inter-RAT change and user data is
                   available
04/19/04   jca     Now performing IMSI Attach and GPRS Attach procedures concurrently
                   when operating in NMO2 (UMTS mode only).
04/10/04   kwa     Modified handling of SYS_STOP_MODE_REASON_MODE_CHANGE so
                   it is treated like SYS_STOP_MODE_REASON_POWER_OFF.
04/29/04   jca     Ensure that GPRS update status is set to GU2 NOT UPDATED
                   if a routing area border is crossed when GMM is in state
                   GMM-ROUTING-AREA-UPDATE-INITIATED (as per 24.008 section 4.7.5.1.5).
05/03/04   jca     Treat reception of MMR_PLMN_SEARCH_REQ which causes GMM service state
                   to be reset to SERVICE_NOT_AVAILABLE as a lower layer failure when
                   GMM is currently attempting an Attach, Detach, or RAU procedure.
05/05/04   jca     Do not erase PSLOCI and LOCI SIM data for RAU reject causes #13 and #15.
05/06/04   jca     On getting the 5th consective Combined GPRS Attach Reject,
                   immediately initiate an LAU with LU type "Normal LU"
05/06/04   ks      If RAU reject with cause GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN
                   is received, PS side of SIM is not invalidated anymore.
05/14/04   jca     Before initiating LU procedure check that mm_idle_substate ==
                   MM_NORMAL_SERVICE or MM_ATTEMPTING_TO_UPDATE.
05/17/04   jca     Check for mm_serv_change_pending after processing DETACH ACCEPT
                   message in GSM mode of operation.
05/19/04   jca     Created function to consolidate processing for lower layer
                   failure during GPRS Detach procedure.
05/21/04   jca     Proceed immediately to GMM_DEREGISTERED state if service
                   is lost while gmm_gprs_detach_pending.
05/27/04   jca     Avoid stack lockup by responding to MMR_REG_REQ for PS_ONLY service
                   when registered in CS_ONLY mode on a CS_ONLY capable cell.
06/01/04   jca     Send MMR_SERVICE_IND on receipt of RR(C)_SERVICE_IND if
                   mm_needs_to_send_mmr_service_ind is set to TRUE.
06/07/04   jca     Reset GMM state to DEREGISTERED when MMR_REG_REQ is received
                   asking for PS service when PS deregistration is in-progress
                   so that GMM can then proceed with GPRS (Re)Attach procedure.
06/09/04   jca     Reset RAU attempt counter upon expiry of the T3302 timer
                   regardless of the current GMM state.
06/15/04   jca     Update mm_serving_plmn.info on receipt of DETACH_ACCEPT message.
06/18/04   jca     Ensure gmm_service_state is set to SERVICE_NOT_AVAILABLE when
                   SERVICE_IND is received indicating loss of service and the
                   SIM state is set to PS_INVALID.
06/18/04   jca     The routing area updating attempt counter shall be reset when a new
                   routing area is entered and the MS is in substate ATTEMPTING-TO-UPDATE
                   (3GPP TS 24.008 section 4.7.5).
                   Always treat RRC_ACTIVATION_IND as a lower layer failure condition when
                   a GPRS Attach, Detach, or Routing Area Updating procedure is active.
                   A GPRS MS in MS operation mode A shall perform the normal routing area
                   update procedure during an ongoing circuit-switched transaction
                   (3GPP TS 24.008 section 4.7.5.2.1).
                   A GPRS MS in MS operation mode A that is in an ongoing circuit-switched
                   transaction, shall initiate the combined routing area updating procedure
                   after the circuit-switched transaction has been released
                   (3GPP TS 24.008 section 4.7.5.2.1).
07/22/04   jca     Ensure that gmm_gprs_detach_pending processing takes precedence
                   when processing SERVICE_IND indicating NO_SERVICE_AVAILABLE.
07/23/04   jca     Only a MANUAL mode MMR_PLMN_SEARCH_REQ should affect the current
                   GMM state/substate.
08/30/04   jca     Now set GMM reg_service_domain to NO_SRV when setting GPRS update status to
                   GU2_NOT_UPDATED in gmm_increment_routing_area_update_attempt_counter();
09/05/04   jca     Added code to correctly handle MMR_REG_REQ indicating service change
                   from CS+PS to CS_ONLY when GMM is in state REGISTERED_INITIATED.
09/05/04   jca     Pass T3311/T3302 timer events through GRR event handler so that
                   queued ATTACH REQUEST message will be txed on timer expiration.
09/10/04   npr     Ensure that that gmm_state is set before sending Attach
                   Request in gmm_initiate_gprs_attach(). Reverse previous change
                   are neccessary.
09/11/04   ab      CR39294, decoded the received NPDU list and pass it to SM.
09/17/04   jca     On getting the 5th consective Combined RA Update Reject,
                   immediately initiate an LAU with LU type "Normal LU"
09/21/04   jca     Fixed bug where Attach procedure was being initiated
                   on a forbidden PLMN.
09/22/04   jca     Send MMR_REG_CNF on receipt on ATTACH ACCEPT even if
                   mm_ready_to_send_reg_cnf flag is set to FALSE.
09/28/04   jca     Added code to support missing requirement from 24.008 section 4.7.5.2.3.2.
10/12/04   jca     No longer set GMM substate to PLMN_SEARCH when processing
                   MMR_PLMN_SEARCH_REQ.
10/14/04   ks      No need to suspend Data Services when doing a RAU in GPRS,
                   the UL data is suspended with GMMLLC_SUSPEND_REQ and DS can
                   still receive data on the DL.
10/15/04   ks      Now sending GMMSM_GPRS_RELEASE_IND to SM after receiving
                   DETACH_ACCEPT message as GMM goes to DEREGISTERED State.
10/18/04   jca     If RAU reject with cause GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN
                   do not send a RRC_SERVICE_REQ changing mode to CS_ONLY.
11/01/04   ks      Now initiating Attach procedure in MT Detach Request with Re-attach
                   required whie GMM is in RAU initiated or Service Initiated states.
11/05/04   jca     Ensure that ms_op_mode is set to CS_ONLY after GPRS Attach Reject
                   received with cause #7 while operating in UMTS mode.
11/11/04   jca     Ensure that GMM reports correct service status to REG when a
                   RR(C)_SERVICE_IND is received indicating SERVICE_NOT_AVAILABLE.
11/30/04   abe     CR 44959: Timer T3302 is stopped while initiating RAU
11/30/04   jca     Fixed code so that GMM substate remains GMM_ATTEMPTING_TO_UPDATE_MM
                   when processing ATTACH ACCEPT with result GPRS_ONLY_ATTACHED and
                   gmm_cause as MSC_TEMPORARILY_NOT_REACHABLE.
12/01/04   abe     CR 53481: Fixed code so that IMSI ATTACH will be triggered
                   instead of NORMAL LU for the Combined GPRS attach reject
                   cause #7 & #14
11/16/04   sv      Additions for the handling of the new primitive between
                   RRC and MM - RRC_SERVICE_IND_FROM_OTA_MSG
12/07/04   jca     If a RR(C)_SERVICE_IND indicating a change of LA is received
                   while awaiting a EST_CNF for a CS call, then abort the CS call
                   and initiate a Location Update.
12/08/04   jca     When a RAU REJECT with cause IMPLICITLY_DETACHED is received
                   make sure the cause field in the MMR_REG_CNF/MMR_SERVICE_IND
                   is set to MMR_CAUSE_FAILURE_REMAIN_ON_PLMN so that GMM is able
                   to complete the (re)-ATTACH procedure without REG immediately
                   redirecting MM to another PLMN/RAT.
12/14/04   jca     Modified code to delay GPRS Detach procedure if IMSI Detach
                   is in-progress only if the UE is operating on GSM RAT.
12/23/04   jca     Ensure that gmm_gprs_detach_pending flag is cleared when a
                   MMR_REG_REQ is received activating PS mode.
12/24/04   abe     Added code to reset the rauting area update attempt counter while
                   handling COMBINED GPRS ATTACH REJECT with reject cause PLMN NOT ALLOWED.
12/17/04   sm      Added check to set GMM state to GMM_SERVICE_REQUEST_INITIATED,
                   in case W to G and G to W back RAT changes.
12/27/04   kvk     Fixed When RAU rejected with cause
                   MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES send
                   MMR_REG_CNF with same cause.
01/10/05   jca     Added check in function gmm_initiate_gprs_attach() to block
                   GPRS Attach procedure if operating in CS+PS mode, NMO=3,
                   and CS service is currently available.
01/20/05   jca     Set GMM state to DEREGISTERED (gprs_attach_pending) if T3310
                   expires in REGISTERED_INITIATED state and GPRS Attach procedure
                   is blocked to Access Class restriction or GPRS Service not available.
01/21/05   jca     Corrected start of T3330 timer when operating in UMTS mode.
01/25/05   jca     RAU procedure blocked if MS op mode set to CS_ONLY.
01/26/05   sm      Corrected start of T3310 timer when operating in UMTS mode.
02/02/04   jca     Ensure mm_serving_plmn.info.service_status is set to STATUS_LIMITED
                   rather than STATUS_SRV whenever CS and/or PS service is available
                   but UE not registered for CS or PS service.
02/04/05   jca     Now sending immediate MMR_REG_CNF if REL_IND received in
                   REGISTERED_INITIATED state and REG waiting for reg_cnf
                   and reg_service_domain indicates CS registered.
02/10/05   jca     If TIMER_T3311 expires in SERVICE_REQUEST_INITIATED state initiate a
                   RAU procedure if the GMM Update Status indicates NOT_UPDATED.
02/11/05   jca     Added code to ensure MMR_REG_CNF is sent under all conditions
                   when RRC_SERVICE_CNF/PLMN_SELECT_CNF indicates NO_SERVICE_AVAILABLE.
02/17/05   jca     Send an immediate MMR_REG_CNF when ATTACH REQUEST rejected with
                   cause MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES.
02/25/05   jca     Always set GPRS Detach type to Combined GPRS/IMSI detach when
                   ms_op_mode set to CS+PS and network op mode set to NMO1.
02/25/05   jca     Ensure MM location update timers are disabled when initiating
                   RAU procedure in ms_op_mode CS+PS and network op mode set to NMO1.
03/02/05   jca     Set GMM state to DEREGISTERED (gprs_attach_pending) if GPRS Attach
                   blocked for any reason when calling gmm_initiate_gprs_attach().
03/07/05   abe     MM susbstate is set to NORMAL SERVICE after a successful combined
                   RAU with result GMM_COMBINED_RA_LA_UPDATED
03/09/05   jca     Ensure T3212 timer is started when combined RAU REJECT message
                   is received with cause "GPRS services not allowed".
03/11/05   abe     Invalidating LAI, TMSI, CKSN and EPLMN list incase of incrementing
                   GPRS attach attempt counter for COMBINED attach.
                   Invalidatimg LAI, TMSI, CKSN and EPLMN list incase of incrementing
                   RAU attempt counter for COMBINED RAU.
03/15/05   abe     Pending the GPRS detach request when T3321 expired and
                   GMM is in GMM_SUSPENDED state.                    .
03/25/05   jca     No need to initiate LU if SIM shows CS already registered in
                   current serving cell when Combined RAU accepted for PS only.
03/28/05   sm      Sending the correct reg_srv_domain for RAU reject cause 13
03/28/05   hj      When attach is blocked due to access class restriction, set the
                   gmm substate to ATTACH_NEEDED so that the attach procedure is
                   initiated when RRC_SERVICE_IND is received with proper access class.
03/30/05   sm      Setting the gmm_ra_update_type, gmm_attach_type correctly incase of
                   SIM being invalid for CS services.
04/11/05   sv      NAS not updating after RAU ACCEPT only for GPRS Services
                   - subsequent Detach type incorrect
04/15/05   jca     Check for gmm_detach_after_rau_pending when service is lost
                   while GMM is in the middle of a RAU procedure.
04/20/05   jca     Now block Detach procedure due to access class restrictions.
04/21/05   sm      Checking for Forbidden PLMN incase of in gmm_process_service_ind
                   to ensure that UE stays on the same PLMN.
04/20/05   hj      If rrc_service_ind is received in GMM_REGISTERED-GMM_LIMITED_SERVICE
                   state and if the MMR_REG_REQ did not specify
                   SYS_NETWORK_SELECTION_MODE_LIMITED, GMM_NORMAL_SERVICE is entered
                   to perform a potential RAU.
04/21/05   hj      When an RAU REQUEST does not get a response from the network
                   after 5 successive T3330 timeouts, send
                   MMR_CAUSE_FAILURE_REMAIN_ON_PLMN as cause in the mmr_service_ind
                   to ensure that UE stays on the same PLMN.
04/21/05   hj      If GMM is in GMM_LIMITED_SERVICE (and update status is GU1 UPDATED,
                   RAI stored stored in the USIM is
                   used to compare with the new RAI received in RRC_SERVICE_IND, for
                   detecting change of RAI.
04/21/05   hj      On resumption of GPRS Services after RRC_SERVICE_IND, if GMM is moved
                   to GMM_NORMAL_SERVICE if GMM state is not GMM_NULL.
04/26/05   sm      Setting the gmm_attach_pending, gmm_ra_update_pending incase
                   timer T3211 expiry.
04/28/05   abe     If RAU rejected with cause #9 then inform REG with the failure
                   cause MMR_CAUSE_FAILURE_REMAIN_ON_PLMN so that it won't trigger
                   any PLMN selection immediately. As MM is proceeding with
                   the ATTACH, it can infom once attach is completed and REG can go
                   for plmn selection if needed.
05/05/2005 abe     Abnormal termination of RAU should restrict upper layer to remain
                   on the same PLMN if RAU attempt counter is < 5.
06/14/03   kvk     Forwarding the Location status to GSTK for every cell change indication
06/13/05   jca     Set GMM substate to NORMAL_SERVICE if combined RA/LA RAU is
                   successful and GMM substate set to GMM_ATTEMPTING_TO_UPDATE_MM.
06/17/05   jca     Now calling mm_rat_handle_srv_status_update() when
                   RR(C)_SERVICE_IND indicates transition from
                   NO_GPRS_SERVICE_AVAILABLE to GPRS_SERVICE_AVAILABLE.
06/20/05   hj      Added support for new events EVENT_SMGMM_REQUEST_SENT,
                   EVENT_SMGMM_REJECT_RECEIVED.
07/05/05   hj      Now blocking ATTACH attempt on receiving RRC_SERVICE_IND if
                   the PLMN belongs to GPRS FPLMN list.
07/08/05   iv      Now initiating LAU after receiving Attach reject with cause
                   GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN.
07/18/05   jca     Fixed bug where MMR_STOP_MODE_REQ was not being correctly
                   processed in GMM_DEREGISTERED_INITIATED state.
07/20/05   abe     Added the condition in gmm_process_service_ind for triggering
                   RAU if WtoG CCO in progress even though RAI not changed for the
                   feature FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER.
08/01/05  hj       If network indicates REATTACH is required in a DETACH message,
                   indicate REG to remain on same PLMN to allow the detach to occur.
09/16/05  ajt      Sending emergency number list to REG if Routing area accept or 
                   GMM Attach is properly received.
09/16/05   up      To take care of sending service status change info to GSTK, 
                   gmm_update_mm_serving_plmn_info is used to set service status.
09/08/05  jca      Ensure CS Location Updating procedure is initiated when RAU Reject
                   message received with cause GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN.
09/09/05  up       Send REG_CNF/SERVICE_IND to add PLMN in GPRS FPLMN List
                   when GPRS ATTACH is rejected with cause GPRS_NOT_ALLOWED.
09/22/05  ajt      GPRS RAU Accept/ GPRS Attach Accept for GPRS Only Services with
                   missing gmm_cause should be handled similar to missing gmm_cause way.
09/23/05  up       Blocking RAU/ATTACH if the PLMN is different than the requested
                   PLMN in LIMITED_MANUAL_ACQ mode.
10/06/05  ajt      Invalidate authentication cache upon receiving RAU Accept                  
10/08/05  abe      Added code for the FEATURE_INTERRAT_PCCO_GTOW
10/13/05  ajt      CR 78463, Send RAU Req with RA Update Type as GMM_RA_UPDATING if SIM is
                   invalid for CS Services except when RA Update Type is GMM_RA_PERIODIC.
10/18/05  jca      Differentiate processing between NMO1 and NMO2 if GPRS Forbidden PLMN
                   detected when processing RR/RRC service confirmation.
11/01/05  jca      Ensure MMR_REG_CNF sent immediately upon receipt of ATTACH REJECT
                   with cause code #15 if REG is awaiting REG_CNF.
11/08/05  ajt      If RR(C)_SERVICE_IND from a GPRS Forbidden PLMN in NMO 1, is received,
                   start NORMAL LU.
11/23/05  ajt      Added processing of PDP Context Status IE in RAU Accept.                   
11/23/05  hj       Reverting Fix for CR 74849, now not initiating LU if RA Update fails
                   with Cause # 14.
11/28/05  abe      Guarded the GSTK call with feature flag FEATURE_GSTK
12/02/05  abe      Allow registration in Manual mode even if PLMN is forbidden
12/14/05  abe      Supported changes for FEATURE_GSM_DTM
12/14/05  ajt      If RAU Fails for type GMM_RA_UPDATE, Update upper layers to reflect 
                   Service on CS Only.
12/28/05  abe      Send MMR_SERVICE_IND after CCO failure to update the PLMN info in UI
01/27/06  ajt      Set mm_pending_lu_request to NO_LU if GMM_ATTACH/RAU_ACCEPT is received
                   for Combined RA LA Update. This scenario will arise when we set 
                   mm_pending_lu_request in NMO 2, and move to NMO1, perform RAU/Attach for
                   combined RA/LA update and on RRC_REL_IND, perform LU again on moving to 
                   MM IDLE state.
02/07/06  ajt      After Combined RAU/Attach succeeds in NMO1, MM Idle substate
                   should be set to Normal Service. If this is not done, earlier 
                   substate e.g. No Cell Available, Attempting to Update etc may
                   cause pages to be not responded by MM.
03/22/06  ajt      Update UI Display with No CS Service in NMO 1 network only while
                   marking MM update status as Not Updated.
                   After PLMN Selection, move to GMM-Deregistered.Attach-Needed or 
                   GMM-Registered.Limited-Service. GMM-Registered.Attach-Needed is 
                   invalid combination.
04/19/06  up       If current GMM substate is SUSPENDED and RAI is not changed
                   do not move gmm substate to NORMAL.
04/18/06   ss    Added cause value to GMMSM_GPRS_RELEASE_IND
05/15/06  up       Do not block the RAU/Attach on Forbidden PLMN in Manual mode.
05/15/06  up       Process pending Emergency Call after Inter-RAT redirection
                   when SIM is Invalid/NO SIM.
05/18/06  up       Set reg cause to GPRS_FORBIDDEN_REMAIN_ON_PLMN when
                   Attach is rejected with cause #14 and LU is initiated.
05/22/06  ajt      In gmm state #4 (deregistered initiated), rau should be allowed 
                   even when ms_op_mode variable shows CS_ONLY. This is because MM
                   updates op mode as soon as MMR_REG_REQ is received. Though spec asks
                   to perform RAU on ra change even though detach is in progress, and on 
                   completion of RAU, proceed with Detach.                                                        
06/14/06  up       Called new gmm_update_mm_serving_plmn_info for GSTK.
06/15/06  up       Initiate ATTACH in DEREGISTERED state 
                   and substate is ATTACH_NEEDED.
06/23/06  up       Rememder GS association is done in Combine RAU/ATTACH.
06/09/06  sn       Added processing of SYS_STOP_MODE_RASON_LPM.
07/04/06  up       Perform LU -IMSI_ATTACH when combine RAU is accepted for
                   GPRS service only with cause #16, #17, #22.
07/12/06  up       Pending RAU should not be performed if ATTACH is done.
07/14/06   pk     Updated for Feature DSAC
07/20/06  sn       Updated gmm_handle_plmn_found, send mm_serving_plmn.info
                   in service state when sending reg_cnf.
08/21/06  up       Initiate RAU if CS/PS for bit set due to Call.
08/23/06  sn       Updated gmm_handle_gprs_attach to remove check for
                   mm_ready_to_send_reg_cnf before sending reg_cnf on
                   receiving attach reject with cause 11.
08/28/06  sn       Add LAI in reject list on receiving reject with cause 15
09/06/06  up       Don't initiate registration if forbidden LAI is reselected in Manual mode.
09/20/06  sn       Send cause MMR_CAUSE_FAILURE_MAX_ATTEMPTED when all RAU/
                   Attach attempts are done.
11/17/06 Adding a function gmm_update_ra_type to update the global variable 
                   "gmm_ra_update_type" appropriately.
12/13/06  sn       Set correct ms_op_mode and req_service_domain on receiving
                   reject cause #7.
04/09/07  rd       SMS optimization changes are incorporated                   

04/23/06  NR       putting additional check for stop_mode_reason in case of STOP_MODE_REQ
02/26/08   rk     removed gmm_inter_rat_follow_on_ind as its not used.
04/17/08  jbk     Not sending plmn_select_req after RAU rej or Detach for GPRS in NMO1,
                  else call gets disconnected while in DTM
07/17/08  abhi  Sending RAU after INTER-RAT to same RA when UE is PMM_CONNECTED or 
                        GMM_READY_STATE
07/13/11   rajesh  Handling expiry of T3412 and T3423 when ISR is ON
08/03/11   jbk      Allow DSR initiated RAU on updated RA
08/11/11   sla      Stopping the timers T3310 and t3330 before the Attach/RAU is initiated
08/25/11   jbk     API to check if PLMN ID is valid
09/06/11   rajesh  Get mapped indentities from LTE when UE finds service in GW even when access
                   class is barred.
09/21/11  sla      When UE is waiting stop mode cnf from AS,it will not do
                   registration when retry timers are getting expired.
09/22/11  sla      Send MMR_REG_REJECT_INd to REG when msc tempororily not
                   reachable cause is received             
10/04/11   sla     Dont initiate LU/RAU when LAI/RAI is in foreground search LAI list.
                   Initiate Attach/RAU when foreground search timer expires.
                   Delete RAC from foreground search list when Attach/RAU is accepted                  
11/29/11  sla      GMM substate is changed to normal service after plmn selection if GMM t3311 previous
                   substate is normal service
12/06/11   gps     Remembering DSAC information of the cell where last registration was attempted
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "comdef.h"
#include "err.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm_coord.h"
#include "mm_v.h"
#include "gmm_v.h"
#include "gmm_int_v.h"
#include "gmm_msg.h"
#include "mm_sim.h"
#include "mm_umts_v.h"
#include "cause.h"
#include "rrccmd.h"
#include "sm_v.h"
#include "smtask.h"
#include "smtask_v.h"
#include "sm_v.h"
#include "mmrat.h"
#include "reg_sim_v.h"

#include "reg_state.h"

#include "sys_eplmn_list_v.h"
#include "rabmtask_v.h"
#include "dsumtsps.h"
#include "cm.h"
#include <stringl/stringl.h>
#include "ttl_map.h"
#ifdef FEATURE_GSTK
#include "mm_gstk.h"
#endif
#ifdef FEATURE_GSM_GPRS
#include "gmmllcm.h"
#endif
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#ifdef FEATURE_LTE
#include "mm_multimode_common.h"
#include "emm_utility.h"
#include "emm.h"
#include "emm_rrc_if.h"
#include "sys_v.h"
#endif
#include "ds3gpp_api.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_REL6_PS_FOP
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern boolean data_is_available_sim[MAX_NAS_STACKS];

#define  data_is_available data_is_available_sim[mm_as_id]



#else
extern boolean data_is_available;

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
#endif


extern void mm_send_mmcnm_rel_ind_ps
(
   byte    reject_cause_value,
   boolean reestablishment_possible
);

#ifdef FEATURE_LTE
extern boolean emm_lte_cs_domain_param_match
(
  const sys_lte_cs_domain_param_s_type lte_cs_domain_param_from_reg, 
  const sys_lte_cs_domain_param_s_type lte_cs_domain_param_in_emm
);
#ifdef FEATURE_DUAL_SIM
extern boolean gmm_set_drx_req_pending_sim[MAX_AS_IDS];
#define gmm_set_drx_req_pending gmm_set_drx_req_pending_sim[mm_sub_id]
extern boolean gmm_lte_deact_isr_timer_expired_sim[MAX_AS_IDS];
#define gmm_lte_deact_isr_timer_expired gmm_lte_deact_isr_timer_expired_sim[mm_sub_id]
extern dword gmm_t3323_timer_value_sim[MAX_AS_IDS];
#define gmm_t3323_timer_value gmm_t3323_timer_value_sim[mm_sub_id]
#else
extern boolean gmm_set_drx_req_pending;
/* This variable is set when T3423 expires and TIN is RAT RELATED TMSI
** This is used to start RAU or LA update when UE comes back to service in G or W
*/
extern boolean gmm_lte_deact_isr_timer_expired;
extern dword gmm_t3323_timer_value;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1_sim[MAX_AS_IDS];
#define gmm_drx_cn_coeff_s1 gmm_drx_cn_coeff_s1_sim[mm_sub_id]
extern sys_drx_cn_coefficient_s1_e_type gmm_nv_drx_cn_coeff_s1_sim[MAX_AS_IDS];
#define gmm_nv_drx_cn_coeff_s1 gmm_nv_drx_cn_coeff_s1_sim[mm_sub_id]
#else
extern sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1;
extern sys_drx_cn_coefficient_s1_e_type gmm_nv_drx_cn_coeff_s1;
#endif
#endif/*FEATURE_LTE*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern boolean  mm_cell_changed_to_cs_capable_sim[MAX_NAS_STACKS];


#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a,(sys_modem_as_id_e_type)mm_sub_id,(sys_modem_as_id_e_type)mm_as_id)

extern ue_rev_level_indicator_type get_revision_level_indicator_per_nas_stacks
(
  sys_modem_as_id_e_type as_id 
);
#define get_revision_level_indicator() get_revision_level_indicator_per_nas_stacks(mm_as_id)
#ifdef FEATURE_LTE
extern boolean mm_rau_pending_in_conn_mode_sim[MAX_NAS_STACKS];
#define mm_rau_pending_in_conn_mode mm_rau_pending_in_conn_mode_sim[mm_as_id]
#endif

#else
extern boolean mm_cell_changed_to_cs_capable;
#ifdef FEATURE_LTE
extern boolean mm_rau_pending_in_conn_mode;
#endif
extern ue_rev_level_indicator_type get_revision_level_indicator( void );
#endif
#ifdef FEATURE_DUAL_SIM
boolean gmm_force_local_detach_sim[MAX_NAS_STACKS];
#endif 

#ifndef FEATURE_NAS_FREE_FLOAT
#ifdef FEATURE_GSM_GPRS
extern void glif_grr_send_camp_on_ccch_indication( void );
#endif /*FEATURE_GSM_GPRS*/
#endif
extern void sm_pack_pdp_context_status(byte *ext_buf, sys_modem_as_id_e_type as_id);

void gmm_handle_local_gprs_detach( void );

#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern    byte mm_last_paging_cause_sim[MAX_NAS_STACKS];
#define   mm_last_paging_cause mm_last_paging_cause_sim[mm_as_id]

#else

extern byte mm_last_paging_cause;

#endif

#if defined(FEATURE_DUAL_SIM)||defined(FEATURE_TRIPLE_SIM)|| defined FEATURE_SGLTE
extern byte gmm_irat_lu_pending_sim[MAX_NAS_STACKS];
#define gmm_irat_lu_pending gmm_irat_lu_pending_sim[mm_as_id]
#else
extern byte gmm_irat_lu_pending;
#endif

extern boolean reg_nv_is_imsi_switch_enabled(void);


/*===========================================================================

FUNCTION GMM_PLMN_CHANGED

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - New PLMN is different than the serving PLMN
  FALSE - New PLMN is is the same as the serving PLMN

SIDE EFFECTS
  None

===========================================================================*/

boolean gmm_plmn_changed
(
  sys_plmn_id_s_type previous_plmn,
  sys_plmn_id_s_type new_plmn
)
{
   boolean matched;

   matched = sys_plmn_match (new_plmn, previous_plmn);

   if (!matched)
   {
       MSG_HIGH_DS_6(MM_SUB, "=MM= PLMN mistmatch, Serving PLMN is %X %X %X , Requested PLMN is %X %X %X ",previous_plmn.identity[0],previous_plmn.identity[1],previous_plmn.identity[2],
           new_plmn.identity[0],new_plmn.identity[1],new_plmn.identity[2]);
   }

   return !matched;
} /* end gmm_plmn_changed() */


/*===========================================================================

FUNCTION GMM_RAI_CHANGED

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - New RAI is different than the serving RAI
  FALSE - New RAI is is the same as the serving RAI

SIDE EFFECTS
  None

===========================================================================*/

boolean gmm_rai_changed
(
  location_area_code_T previous_lac,
  routing_area_code_T  previous_rac,
  location_area_code_T new_lac,
  routing_area_code_T  new_rac
)
{
  if ((previous_lac.lac[0] == new_lac.lac[0]) &&
      (previous_lac.lac[1] == new_lac.lac[1]) &&
      (previous_rac == new_rac))
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* end gmm_rai_changed() */


/*===========================================================================

FUNCTION GMM_HANDLE_PLMN_FOUND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_handle_plmn_found( serving_plmn_info_T serving_plmn,
                            boolean             location_update_required )
{
  boolean routine_man_selection_succ = FALSE;
  boolean is_cell_forbidden = TRUE;
  mmr_service_ind_s_type mmr_service_ind;

#ifdef FEATURE_FEMTO_CSG
  boolean is_manual_csg_selection_success = FALSE;
#endif 
  boolean is_ps_attach_allowed   = TRUE;
  
#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* If MM has blocked Attach Failed to Let GMM Decide, Unblock it Now */
  if (mm_mmr_attach_ind_status == ATTACH_FAILED_IND_WAITING_FOR_GMM)                          
  {
     mm_mmr_attach_ind_status = ATTACH_FAILED_IND_REQUIRED;
  }

  /* ------------------------------------------------------------------------
  ** After OOS, if service is required by PLMN Selection but Access Class is
  ** barred in new cell, location_update_required will be TRUE here. This will
  ** prevent us from clearing data suspension marked TRUE due to OOS. Therefore,
  ** clearing data-suspension explicitly in this case by following if block.
  ** ------------------------------------------------------------------------ */

  if ((mmsim_sim_valid( SYS_SRV_DOMAIN_PS_ONLY )) &&
      (!location_update_required) &&
      (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE))
  {
    mm_rat_handle_srv_status_update( SYS_SRV_STATUS_SRV );    
  }

#if defined (FEATURE_LTE) && defined (FEATURE_DUAL_DATA)
  /* Allow PS service only on PLMN present in "Non DDS SUB -VOLTE allowed PLMN list" 
       when other SUB is having DDS (CS+PS, GSM Only) */
       
  is_ps_attach_allowed = mm_is_volte_registration_allowed(mm_serving_plmn.info.plmn, FALSE);
#endif

  if ((!mm_check_forbidden_national_roaming( &gmm_current_lai )) &&
     (!mm_check_forbidden_regional_roaming( &gmm_current_lai )) &&
     (!gmm_check_gprs_forbidden_rai(mm_serving_plmn.info.plmn,mm_serving_plmn.lac,mm_serving_plmn.rac)) &&
  #ifdef FEATURE_DUAL_SIM
     (!reg_sim_per_subs_plmn_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) &&
     (!reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id ))
  #else
    (!reg_sim_plmn_forbidden( mm_serving_plmn.info.plmn )) &&
    (!reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn ))
 #endif
 #ifdef FEATURE_FEMTO_CSG
    && (mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info)) 
#endif
    && (is_ps_attach_allowed)
    )
  {
    is_cell_forbidden = FALSE;
  }

    /* -----------------------------------------
    ** Allow registration if either of folowing TRUE.
    ** - Requested PLMN matched with acquired PLMN in LIM_MANUAL_ACQ mode and manual mode user selection flag is TRUE
    ** - In Manual mode and manual mode user selection flag is TRUE.
    ** - Not forbidden PLMN/LA and mode is not pure Limited.
    ** ----------------------------------------- */
    if (((PLMN_MATCH( mm_serving_lai,
                      mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity )) ||
         (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
          sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))  &&
        (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                     SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION) 
     #ifdef FEATURE_FEMTO_CSG
         &&  (mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
     #endif
     #if defined (FEATURE_LTE) && defined (FEATURE_DUAL_DATA)
         &&  (mm_is_volte_registration_allowed(mm_serving_plmn.info.plmn, FALSE) == TRUE)
     #endif
         )
    {
      routine_man_selection_succ = TRUE;
      MSG_HIGH_DS_0(MM_SUB, "=MM= Same PLMN found during MAN_LIMITED_ACQ");
    }

#ifdef FEATURE_FEMTO_CSG
  /*Manual CSG selection is only successful if UE camps on non forbidden cell */
  if((mmcoord_pending_reg_message.cmd.mmr_reg_req.csg_id != SYS_CSG_ID_INVALID) &&
      (mmcoord_pending_reg_message.cmd.mmr_reg_req.csg_id == mm_serving_plmn.info.csg_info.csg_id) &&
       (sys_plmn_match(mm_serving_plmn.info.plmn, mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn))) 
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= CSG: Manual CSG selection successful");
    is_manual_csg_selection_success = TRUE;
  }
#endif 
  /* -------------------------------------------------------------------
  ** Check if REG has enabled LU and that we are not in a forbidden list
  ** -------------------------------------------------------------------  */
/*24.008: 4.7.13.5  (m)  Timer T3346 is running
    The MS shall not start the service request procedure unless the MS is accessing the network with access class 11 – 15*/ 
 if ((mmsim_sim_valid( SYS_SRV_DOMAIN_PS_ONLY )) &&
      (location_update_required) &&
      (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE)
#ifdef FEATURE_NAS_ECALL
       &&
      (mm_ecall_mode_type == MMR_ECALL_MODE_NORMAL)
#endif
#ifdef FEATURE_NAS_REL10
      &&(!((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
#ifdef FEATURE_NAS_REL11
            (mm_is_rrc_access_class_11_to_15() == FALSE)&&
#endif
            (!is_cell_forbidden || routine_man_selection_succ) &&
            (gmm_is_plmn_equivalent_t3346()) &&
            (
/*4.7.5.1.5 (j)*/
#ifndef FEATURE_NAS_REL11
            (gmmllc_get_glif_state() != GMM_READY_STATE)&&
#endif
             (pmm_mode != PMM_CONNECTED))))
#endif
        )
    {

#if defined(FEATURE_LTE)
        /* If UE has PDP activated in LTE and ISR is activated, set TIN to GUTI*/
    if(((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
         !((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode 
                    ==  SYS_NETWORK_SELECTION_MODE_LIMITED) ||
           ((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                           SYS_NETWORK_SELECTION_MODE_AUTOMATIC )||
         (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                           SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) ||
         ((!gmm_manual_mode_user_selection_procedure)&&
          ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                           SYS_NETWORK_SELECTION_MODE_MANUAL) ||
           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                           SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))))&& 
        ((is_cell_forbidden)
#ifdef FEATURE_FEMTO_CSG
          || (is_manual_csg_selection_success )
#endif 
        )))))
    {
      if(sm_check_isr_deact_status_on_interrat())
      {
        MSG_HIGH_DS_0(MM_SUB,"=MM= Deactivate ISR due to PDP context update while in LTE");
        emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
      }
    }
#endif
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#ifdef FEATURE_GSM_GPRS    
    if (is_umts_mode() && (gmmllc_get_glif_state() == GMM_READY_STATE))
    {
      gmm_is_prev_mode_connected = TRUE;
      MSG_HIGH_DS_1(MM_SUB, "=MM= GMM Previous mode Ready or Connected = %d", gmm_is_prev_mode_connected);
    }
#endif /*FEATURE_GSM_GPRS*/
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
    if ((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                  SYS_NETWORK_SELECTION_MODE_MANUAL) || 
           (routine_man_selection_succ)) &&
        ((gmm_manual_mode_user_selection_procedure == TRUE) ||
          (!is_cell_forbidden))
#ifdef FEATURE_NAS_REL10
         &&
         (mm_timer_status[TIMER_T3346] == TIMER_STOPPED  || !(gmm_is_plmn_equivalent_t3346())
#ifdef FEATURE_NAS_REL11
         ||
         (mm_is_rrc_access_class_11_to_15() == TRUE) 
#endif
         )
#endif
        ) ||
        (((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                 SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) ||
           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                 SYS_NETWORK_SELECTION_MODE_AUTOMATIC)) &&
        (!is_cell_forbidden))
#ifdef FEATURE_FEMTO_CSG
        || ((is_manual_csg_selection_success)
#ifdef FEATURE_NAS_REL10
         &&
         (mm_timer_status[TIMER_T3346] == TIMER_STOPPED  || !(gmm_is_plmn_equivalent_t3346())
#ifdef FEATURE_NAS_REL11
         ||
         (mm_is_rrc_access_class_11_to_15() == TRUE)  
#endif
         )
#endif
)
#endif
       )
    {

      /* If LAI has changed and NMO1 then reset MM attempt counter too */
      if((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
         ((memcmp(&(mm_previous_serving_lai[0]),&(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE) != 0 ) ||
         (memcmp(&(mm_previous_serving_lai[PLMN_SIZE]),&(mm_serving_plmn.lac.lac[0]), LAC_SIZE) != 0)))
      {
        mm_clear_lu_attempt_counter();
      }

      if ((gmm_plmn_changed( gmm_previous_registration_serving_rai.plmn_id, gmm_current_lai.PLMN_id ) ||
            (gmm_rai_changed( gmm_previous_registration_serving_rai.location_area_code,
                              gmm_previous_registration_serving_rai.routing_area_code,
                              mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
            ((gmm_follow_on_request) ||
             (mm_follow_on_request && (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)))) ||
           (is_gsm_mode() &&gmm_multi_slot_ra_update_pending))
      {

        if (gmm_state == GMM_DEREGISTERED ||
            gmm_state == GMM_DEREGISTERED_INITIATED ||
            gmm_state == GMM_REGISTERED_INITIATED)
        {
            /* -------------------------------------------------------------
            ** The GPRS attach attempt counter shall be reset when:
            **  the MS is in substate ATTEMPTING-TO-ATTACH and
            ** a new routing area is entered;or
            ** an attach is triggered by CM sublayer requests.
            ** (3GPP TS 24.008 section 4.7.3).
            ** ------------------------------------------------------------- */
            gprs_attach_attempt_counter = 0;
			t3330_timeout_counter = 0;
            MSG_HIGH_DS_2( MM_SUB, "=MM= GPRS Attach Attempt counter = %d and T3330 timeout counter = %d", gprs_attach_attempt_counter, t3330_timeout_counter);
        }
        else
        {
            /* -------------------------------------------------------------
            ** The routing area updating attempt counter shall be reset when
            ** a new routing area is entered and the MS is in substate
            ** ATTEMPTING-TO-UPDATE (3GPP TS 24.008 section 4.7.5).
            ** ------------------------------------------------------------- */
            gmm_rau_attempt_counter = 0;
            t3330_timeout_counter = 0;
            MSG_HIGH_DS_2( MM_SUB, "=MM= RAU Attempt counter = %d and T3330 timeout counter = %d", gmm_rau_attempt_counter, t3330_timeout_counter );
        }

      }          

      if ((((mm_timer_status[ TIMER_T3311 ] == TIMER_ACTIVE) ||
          (mm_timer_status[ TIMER_T3302 ] == TIMER_ACTIVE))
#ifdef FEATURE_LTE
           && (!((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) &&
                 (emm_ctrl_data_ptr->emm_guti_valid == TRUE) &&
                 (mm_service_loss_on_multimode_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
                 (gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED)))
#endif          
           )
          )
      {
        if ((gmm_plmn_changed( gmm_previous_registration_serving_rai.plmn_id, gmm_current_lai.PLMN_id ) ||
            (gmm_rai_changed( gmm_previous_registration_serving_rai.location_area_code,
                              gmm_previous_registration_serving_rai.routing_area_code,
                              mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
            ((gmm_follow_on_request) ||
             (mm_follow_on_request && (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)))
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
             ||
             gmm_is_prev_mode_connected
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
            || (is_gsm_mode() && gmm_multi_slot_ra_update_pending)))
        {
          gmm_set_substate( GMM_NORMAL_SERVICE );
        }
        else
        {
          if (gmm_previous_T3311_gmm_substate == GMM_ATTEMPTING_TO_ATTACH)
          {
            gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH );
          }
          else if (gmm_previous_T3311_gmm_substate == GMM_ATTEMPTING_TO_UPDATE)
          {
            gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE );          
          }  
          else if (gmm_previous_T3311_gmm_substate == GMM_ATTEMPTING_TO_UPDATE_MM)
          {
            gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE_MM );          
          } 
          else if(gmm_processing_directed_signalling_reestablishment)
          {
            gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE );     
          }
          else if (gmm_previous_T3311_gmm_substate == GMM_NORMAL_SERVICE)
          {
            gmm_set_substate( GMM_NORMAL_SERVICE );          
          } 
          
          if(mm_managed_roaming_enabled && 
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
              (reg_sim_plmn_in_low_priority_plmn_list(mm_serving_plmn.info.plmn, serving_plmn.info.active_rat, (sys_modem_as_id_e_type)mm_as_id)) &&
#else
              (reg_sim_plmn_in_low_priority_plmn_list(mm_serving_plmn.info.plmn, serving_plmn.info.active_rat)) &&
#endif
              (gmm_rau_attempt_counter == 5 ||
               gprs_attach_attempt_counter == 5) &&
              (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
               mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) )
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= GMM HANDLE to MM HANDLE !!");
            mm_handle_plmn_found( mm_serving_plmn, location_update_required );
          }
          else if((mm_location_information.location_update_status != UPDATED) &&
             (mm_timer_status[ TIMER_T3302 ] == TIMER_ACTIVE) &&
             (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
             (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= GMM HANDLE to MM HANDLE to do LAU as T3302 is running");
            mm_handle_plmn_found( mm_serving_plmn, location_update_required );
          }
          else if ( ((gmm_rau_attempt_counter >= 5) ||
                (gprs_attach_attempt_counter >= 5)
#ifdef FEATURE_ENHANCED_NW_SELECTION
                    || (gmm_is_rai_in_foreground_search_lai_rai_timer_list((byte*)&gmm_current_lai,gmm_current_rac))
#endif
                   ) &&
                (mm_reg_waiting_for_reg_cnf && mm_state == MM_IDLE) &&
             !(((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
                (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
                (mm_lu_attempt_counter <4)))
          {
            if ((gmm_rau_attempt_counter >= 5) ||(gprs_attach_attempt_counter >= 5))
          {
            if(mm_managed_roaming_enabled &&
               mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1 &&
               mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
            {
              mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
            }
#ifdef FEATURE_ENHANCED_NW_SELECTION
            if(reg_sim_read_ens_flag() &&
               mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1 &&
               mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
            {
              mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
            }
#endif
            }
            mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
            mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;                      
            mm_send_mmr_reg_cnf();
            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;            
          }
        }
      }
      else
      {
        /* ------------------------------------------------------------------
        ** If a cell supporting GPRS has been found and the PLMN or LA is not
        ** in the forbidden list then the substate shall be NORMAL-SERVICE
        ** (TS 24.008 4.2.4.1.1)
        ** ------------------------------------------------------------------ */
        /*
        ** If GMM substate is SUSPENDED and RAI is not changed, do not move to NORMAL.
        ** i.e. Change the substate to NORMAL is current gmm substate is not SUSPENDED
        ** or RAI has changed.
        */
        if ((((gmm_substate != GMM_SUSPENDED) &&
             (is_umts_mode() || !gmm_is_suspended)) ||
            (gmm_plmn_changed( gmm_previous_serving_rai.plmn_id, gmm_current_lai.PLMN_id ) ||
                (gmm_rai_changed( gmm_previous_serving_rai.location_area_code,
                                  gmm_previous_serving_rai.routing_area_code,
                                  mm_serving_plmn.lac, mm_serving_plmn.rac ))) 
#if defined(FEATURE_LTE)
           || (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI)
           /*
           ** SPEC 24.008 rel 8:
           ** 4.7.5.2.1 Combined routing area updating procedure initiation
           */
           ||(((gmm_lte_deact_isr_timer_expired == TRUE )||
                     (mm_timer_status[TIMER_T3423] == TIMER_ACTIVE &&
                       emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI))
                  &&(mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
                  && (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
#endif
            )

           )
        {
          gmm_set_substate( GMM_NORMAL_SERVICE );
        }
        else if((mm_serving_plmn.info.service_status == SYS_SRV_STATUS_SRV) &&
                ((mm_state == MM_CONNECTION_ACTIVE) ||
                 (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) ||
                 (mm_state == MM_WAIT_FOR_RR_CONNECTION_MM)) &&
                ((gmm_substate == GMM_SUSPENDED) || (gmm_is_suspended)) &&
                ((mm_follow_on_request || gmm_follow_on_request) && 
                (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)))
        {
          if (mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf)
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF" );
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
            mm_send_mmr_reg_cnf();
            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;      
          }
      }
    }
    }
    else  /* LU requested but LAI forbidden */
    {
      if (((gmm_check_gprs_forbidden_rai(mm_serving_plmn.info.plmn,mm_serving_plmn.lac,mm_serving_plmn.rac)) ||
#ifdef FEATURE_DUAL_SIM
          (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id ))
#else
          (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn ))
#endif
#if defined (FEATURE_LTE) && defined (FEATURE_DUAL_DATA)
           || (is_ps_attach_allowed == FALSE)
#endif
         ) &&
          (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
      {
        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES;

        if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
        {
          /* ------------------------------------------------------------------
          ** If the selected cell is in a forbidden LAI then the MS shall enter
          ** the substate LIMITED-SERVICE (TS 24.008 4.2.4.1.1)
          ** ------------------------------------------------------------------ */
          gmm_set_substate( GMM_LIMITED_SERVICE );
          
          mm_handle_plmn_found( mm_serving_plmn, location_update_required );
        }
        else
        {
          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           mm_serving_plmn.info.reg_service_domain,
                                           FALSE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE );

          if (mm_reg_waiting_for_reg_cnf)
          {
            if (mm_ready_to_send_reg_cnf)
            {
              mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
              mm_send_mmr_reg_cnf();
              mm_reg_waiting_for_reg_cnf = FALSE;
              mm_ready_to_send_reg_cnf   = FALSE;
            }
          }
        }
      }
      else
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= PLMN or LAI is forbidden");

        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                         mm_serving_plmn.info.reg_service_domain,
                                         TRUE,
                                         SYS_SRV_STATUS_LIMITED,
                                         TRUE );

            /**------------------------------------------------------------------------
      **The MS shall also start the normal location updating procedure when the network 
      **is operating in network operation mode I, T3346 is running, update status is
      **not U1 UPDATED and the user manually selects the current PLMN.
      **(24.008 section 4.4.1) ------------------------------------------------*/
#ifdef FEATURE_NAS_REL10
        if ((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
#ifdef FEATURE_NAS_REL11
             (mm_is_rrc_access_class_11_to_15() == FALSE)&&
#endif
            (gmm_is_plmn_equivalent_t3346() == TRUE) &&  
            (mm_manual_mode_user_selection_procedure == TRUE) &&
            (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
            (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
        {
           mm_handle_plmn_found( mm_serving_plmn, location_update_required );
        }
        else 
#endif
        {
        if (mm_reg_waiting_for_reg_cnf)
        {
          mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
          mm_send_mmr_reg_cnf();

          mm_reg_waiting_for_reg_cnf = FALSE;
          mm_ready_to_send_reg_cnf   = FALSE;
        }
        }

#ifdef FEATURE_NAS_REL10
       if(gmm_manual_mode_user_selection_procedure &&
          (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
          (gmm_is_plmn_equivalent_t3346()) )
       {
         if(gmm_state == GMM_DEREGISTERED)
         {
           gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH);
         }
         else if(gmm_state == GMM_REGISTERED)
         {
           if(gmm_update_status == GMM_GU1_UPDATED)
           {
             gmm_set_substate( GMM_NORMAL_SERVICE);
           }
           else
           {
             gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE);
           }
         }
       }
       else
#endif
       {

        /* ------------------------------------------------------------------
        ** If the selected cell is in a forbidden LAI then the MS shall enter
        ** the substate LIMITED-SERVICE (TS 24.008 4.2.4.1.1)
        ** ------------------------------------------------------------------ */
        gmm_set_substate( GMM_LIMITED_SERVICE );
        mmr_service_ind.service_state = mm_serving_plmn.info;
        mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
  
        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service");
        mm_send_mmr_service_ind( &mmr_service_ind );

      }
     }
      
      /*****************************************************************************
      *Reset this flag to false as we are not trying PS registration and if PS fop
      *is to be set,SM will be in SIG_CONNECTION_PENDING state.It will take care 
      *of setting PS fop to true
      *****************************************************************************/  
      
      gmm_follow_on_request = FALSE;

    }

    /* ---------------------------------------------------------------------
    ** Handle service status changes so that PS data continuity is ensured
    ** during RAT changes due to loss of service.
    ** --------------------------------------------------------------------- */
    mm_rat_handle_srv_status_update( SYS_SRV_STATUS_SRV );
  }
  else
  {
    mm_as_cell_access_status_T cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);
    
    if (!mmsim_sim_valid( SYS_SRV_DOMAIN_PS_ONLY ))
    {
      gmm_set_substate( GMM_NO_IMSI );

      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                       SYS_SRV_DOMAIN_NO_SRV,
                                       FALSE,
                                       SYS_SRV_STATUS_LIMITED,
                                       TRUE );

      if (mm_reg_waiting_for_reg_cnf)
      {
        if (mm_ready_to_send_reg_cnf)
        {
          mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
          mm_send_mmr_reg_cnf();
          mm_reg_waiting_for_reg_cnf = FALSE;
          mm_ready_to_send_reg_cnf   = FALSE;
        }
      }
    }
    else if (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
    {
      gmm_set_substate( GMM_NO_CELL_AVAILABLE );
    }
#ifdef FEATURE_NAS_ECALL
    else if (mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY)
    {
     
     //gmm_set_substate( GMM_NO_IMSI );

      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                       mm_serving_plmn.info.reg_service_domain,
                                       FALSE,
                                       SYS_SRV_STATUS_LIMITED,
                                       TRUE );
 
      if (mm_reg_waiting_for_reg_cnf)
      {
        if (mm_ready_to_send_reg_cnf)
        {
           mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
           mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
           mm_send_mmr_reg_cnf();
           mm_reg_waiting_for_reg_cnf = FALSE;
           mm_ready_to_send_reg_cnf   = FALSE;
        }
      }

      if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
      {
        mm_handle_plmn_found( mm_serving_plmn, location_update_required );
      }
    }
#endif
#ifdef FEATURE_NAS_REL10
    else if(mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) 
    {
#if defined(FEATURE_LTE)
      /* If UE has PDP activated in LTE and ISR is activated, set TIN to GUTI*/
      if(((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
           !((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode 
                      ==  SYS_NETWORK_SELECTION_MODE_LIMITED) ||
             ((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                             SYS_NETWORK_SELECTION_MODE_AUTOMATIC )||
           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                             SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) ||
           ((!gmm_manual_mode_user_selection_procedure)&&
            ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                             SYS_NETWORK_SELECTION_MODE_MANUAL) ||
             (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                             SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))))&& 
          ((is_cell_forbidden)
#ifdef FEATURE_FEMTO_CSG
            || (is_manual_csg_selection_success )
#endif 
         )))
          )
         )
      {
        if(sm_check_isr_deact_status_on_interrat())
        {
          MSG_HIGH_DS_0(MM_SUB,"=MM= Deactivate ISR due to PDP context update while in LTE");
          emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
        }
      }       
#endif

         /**------------------------------------------------------------------------
       ** The MS shall also start the normal location updating procedure when the network 
       **is operating in network operation mode I, T3346 is running, update status is
       **not U1 UPDATED and the user manually selects the current PLMN.
       **(24.008 section 4.4.1) ------------------------------------------------*/
      if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
      {
         mm_handle_plmn_found( mm_serving_plmn, location_update_required );
      }
      else 
      {
        /** -----------------------------------------------------------------
             **If the stored RAI is different to the RAI of the current serving cell or
             **the TIN indicates "GUTI", the MS shall set the GPRS update status to GU2 NOT UPDATED
             **and change to state GMM-REGISTERED.ATTEMPTING-TO-UPDATE.
             **(24.008 section 4.7.5.1.5) ------------------------------------------------*/
        if (((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                  gmm_stored_rai.routing_area_code,
                                  mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn))) &&
               (gmm_update_status == GMM_GU1_UPDATED)) 
#if defined(FEATURE_LTE)
                    &&
             ((emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI) 
/*4.1.1.7:At an inter-system change from S1 mode to A/Gb mode, if the timer T3346 is running and the TIN indicates "RAT related TMSI", 
the MS shall set the GPRS update status to GU2 NOT UPDATED and enter state GMM-REGISTERED.ATTEMPTING-TO-UPDATE.*/
#ifdef FEATURE_NAS_REL11
                  && (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_RAT_RELATED_TMSI)
#endif
             )
#endif        

            )
           )
        {
          gmm_set_substate( GMM_NORMAL_SERVICE);
          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           mm_serving_plmn.info.reg_service_domain,
                                           FALSE,
                                           SYS_SRV_STATUS_SRV,
                                           TRUE);
        }
        else 
        {
          if(gmm_state == GMM_DEREGISTERED)
          {
            gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH);
          }
          else
          {
            gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE);
          }
          gmm_update_status = GMM_GU2_NOT_UPDATED;
          mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                              gmm_stored_ptmsi_signature,
                                              gmm_stored_rai,
                                              gmm_update_status );
          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           mm_serving_plmn.info.reg_service_domain,
                                           FALSE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE );

          if((gmm_plmn_changed( gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn ))||
              (gmm_rai_changed( gmm_stored_rai.location_area_code,
                                gmm_stored_rai.routing_area_code,
                                mm_serving_plmn.lac, mm_serving_plmn.rac )))
            {
                if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                    (mm_serving_plmn.network_op_mode != gmm_previous_network_op_mode) &&
                    (gmm_previous_network_op_mode != PS_DOMAIN_SYS_INFO_UNAVAILABLE) &&
                    ((mm_timer_status[TIMER_T3246] == TIMER_STOPPED)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                      ||(mm_is_rrc_access_class_11_to_15() == TRUE)
#endif
                     ))
                {
                   gmm_change_of_nmo_during_t3346_active();
                }
            }
        }
        if(mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= LU initiated - T3346 running " );
        }
        else if (mm_reg_waiting_for_reg_cnf)
        {
          if (mm_ready_to_send_reg_cnf)
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_REG_CNF " );
            mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();
            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
          }
        }
        else
        {
             MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_SERVIC_IND " );
             mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
             mmr_service_ind.service_state = mm_serving_plmn.info;
             mm_send_mmr_service_ind( &mmr_service_ind );
        }
      }
    }
#endif
    /* Transition to Limited service substate only if there are no AC restrictions */
    else if (cell_access == MM_AS_ACCESS_ALL_CALLS)
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= LAI changed - but REG said no LU");
      gmm_set_substate( GMM_LIMITED_SERVICE );
    }
    else if (cell_access == MM_AS_ACCESS_NO_CALLS)
    {
      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                       mm_serving_plmn.info.reg_service_domain,
                                       FALSE,
                                       SYS_SRV_STATUS_NO_SRV,
                                       TRUE );

      if (mm_reg_waiting_for_reg_cnf)
      {
        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
        mm_send_mmr_reg_cnf();
        mm_reg_waiting_for_reg_cnf = FALSE;
        mm_ready_to_send_reg_cnf   = FALSE;
      }
    }

    /*****************************************************************************
    *Reset this flag to false as we are not trying PS registration and if PS fop
    *is to be set,SM will be in SIG_CONNECTION_PENDING state.It will take care 
    *of setting PS fop to true
    *****************************************************************************/ 
    
    gmm_follow_on_request = FALSE;

  }

  gmm_previous_serving_rai.plmn_id                   = gmm_current_lai.PLMN_id;
  gmm_previous_serving_rai.location_area_code.lac[0] = gmm_current_lai.location_area_code[0];
  gmm_previous_serving_rai.location_area_code.lac[1] = gmm_current_lai.location_area_code[1];
  gmm_previous_serving_rai.routing_area_code         = gmm_current_rac;

} /* end gmm_handle_plmn_found() */


/*===========================================================================

FUNCTION GMM_INITIATE_GPRS_ATTACH

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_initiate_gprs_attach( void )
{
  mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);
  boolean routine_man_selection_failed = FALSE;
  boolean is_cell_forbidden = FALSE;

#ifdef FEATURE_FEMTO_CSG
  boolean is_manual_csg_selection_success = FALSE;
#endif 

#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

  /* If mode is GSM and Multi Slot RA update is pending, then reset
   * the variable which tracks pending status because ATTACH is being initiated
   * with updated RF capabilities */
  if(is_gsm_mode() && (gmm_multi_slot_ra_update_pending == TRUE))
  {
    gmm_multi_slot_ra_update_pending = FALSE;
  }

  if (!(((PLMN_MATCH( gmm_current_lai.PLMN_id.identity,
                     mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity )) ||
        (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
         sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))
  #ifdef FEATURE_FEMTO_CSG
     && (mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
  #endif
  #if defined (FEATURE_LTE) && defined (FEATURE_DUAL_DATA)
     && (mm_is_volte_registration_allowed(mm_serving_plmn.info.plmn, FALSE) == TRUE)
  #endif
          )  &&
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
              SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  {
      routine_man_selection_failed = TRUE;
      MSG_HIGH_DS_0(MM_SUB, "=MM= Different PLMN found during MAN_LIMITED_ACQ");
  }

  if ((mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
      (mm_check_forbidden_regional_roaming( &gmm_current_lai )) ||
#ifdef FEATURE_DUAL_SIM
      (reg_sim_per_subs_plmn_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
      (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id ))
#else
      (reg_sim_plmn_forbidden( mm_serving_plmn.info.plmn )) ||
      (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn ))
#endif
     )
  {
      is_cell_forbidden = TRUE;
  }

#ifdef FEATURE_FEMTO_CSG
  if((mm_serving_plmn.info.csg_info.csg_id == mmcoord_pending_reg_message.cmd.mmr_reg_req.csg_id) &&
      (sys_plmn_match(mm_serving_plmn.info.plmn, mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn)) &&
      (mm_serving_plmn.info.csg_info.csg_id != SYS_CSG_ID_INVALID))
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= CSG: Manual CSG selection success");
    is_manual_csg_selection_success = TRUE;
  }
#endif

  if (mm_waiting_for_stop_mode_cnf == TRUE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= Waiting for STOP MODE CNF from Lower Layers. Defer Attach.");
  }
  else if ((mmcoord_pending_reg_message.cmd.hdr.message_set == (int) MS_MM_REG) &&
            (mmcoord_pending_reg_message.cmd.hdr.message_id == (int) MMR_STOP_MODE_REQ)&&
            (mm_nreg_req_received))
  {
     MSG_HIGH_DS_0(MM_SUB, "=MM= Power off, Attach trigger ignored");
     mm_stop_timer(TIMER_T3310);
  }
  /* Check if GMM is in Limited Service */
  else if (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
        SYS_NETWORK_SELECTION_MODE_LIMITED)
  {
    MSG_HIGH_DS_0( MM_SUB, "=MM= Network Select Mode Limited - GPRS Attach blocked" );
    gmm_gprs_attach_pending = TRUE;
    gmm_set_state( GMM_DEREGISTERED );
  }
  else if((routine_man_selection_failed)||
          (((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                SYS_NETWORK_SELECTION_MODE_MANUAL)||
              (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                  SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))&&
              (!gmm_manual_mode_user_selection_procedure)) ||
             ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                 SYS_NETWORK_SELECTION_MODE_AUTOMATIC)||
              (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                   SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))) &&
             ((is_cell_forbidden)
#ifdef FEATURE_FEMTO_CSG
              && !(is_manual_csg_selection_success)
#endif 
             )))
  {
    MSG_HIGH_DS_0( MM_SUB, "=MM= Cell is forbidden - GPRS Attach blocked" );
    gmm_set_state( GMM_DEREGISTERED );
  }
  /* Check blocking restrictions */
  else if ((gmm_substate == GMM_SUSPENDED) || (is_gsm_mode() && gmm_is_suspended))
  {
    MSG_HIGH_DS_0( MM_SUB, "=MM= GPRS Attach blocked - GMM_SUSPENDED" );
    gmm_gprs_attach_pending = TRUE;
    gmm_set_state( GMM_DEREGISTERED );
    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
    {
       mm_pending_lu_request = NO_LU;
       MSG_HIGH_DS_0( MM_SUB, "=MM= Pending Normal LU flag cleared here as Combined proc is pended" );
    }    
  }
  else if
  (
    /***********************************************************************************
    ** If DTM is not supported then we must wait for MM Idle before initiating RAU.
    **
    ** If DTM is supported, then we must check to see if MM is establishing an RR connection
    ** for LU, as this will be indicative that a GPRS Suspension message from RR will be queued
    ** and waiting to be processed by GMM. In this case RAU must be held pending GPRS Resumption
    ** on completion of RR connection establishment.
    *************************************************************************************/
#ifdef FEATURE_GSM_DTM
    ((is_gsm_mode())
     &&
     (
      ((mm_serving_plmn.dtm_supported == FALSE) && (mm_state != MM_IDLE)) ||
      ((mm_serving_plmn.dtm_supported == TRUE ) && ((mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) ||
                                                    (((mm_state == MM_WAIT_FOR_RR_CONNECTION_MM) || (mm_state == MM_WAIT_FOR_NETWORK_COMMAND)) && (rrc_connection_status == MMRRC_EST_IN_PROGRESS))
                                                   )
      )
     )
    ) ||
#else
    (is_gsm_mode() && (mm_state != MM_IDLE)) ||
#endif /* FEATURE_GSM_DTM */
    (is_umts_mode() &&
      (
        (mm_state == MM_WAIT_FOR_RR_CONNECTION_IMSI_DETACH) ||
        (mm_state == MM_IMSI_DETACH_INITIATED             )
      )
    )
  )
  {
#ifdef FEATURE_GSM_DTM
    MSG_HIGH_DS_0( MM_SUB, "=MM= GPRS Attach blocked - Either mm_state != IDLE and no DTM OR PS Suspension by RR in progress" );
#else
    MSG_HIGH_DS_0( MM_SUB, "=MM= GPRS Attach blocked - mm_state != IDLE");
#endif /* FEATURE_GSM_DTM */
    gmm_gprs_attach_pending = TRUE;
    gmm_set_state( GMM_DEREGISTERED );
    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
    {
       mm_pending_lu_request = NO_LU;
       MSG_HIGH_DS_0( MM_SUB, "=MM= Pending Normal LU flag cleared here as Combined proc is pended");
    }     
  }
  else if ((gmm_check_if_ps_cell_access_barred(ps_cell_access) == TRUE) ||
           (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE))
  {
    MSG_HIGH_DS_2( MM_SUB, "=MM= GPRS Attach blocked - AC %d or service_state %d",
               ps_cell_access, mm_serving_plmn.gmm_service_state);
    gmm_set_state( GMM_DEREGISTERED );
    /* ---------------------------------------------------------------------
    ** Set the substate to attach needed, to remember to perform
    ** attach procedure when access class will be unbarred.
    ** ---------------------------------------------------------------------*/
    if (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
    {
      gmm_set_substate( GMM_NO_CELL_AVAILABLE );
    }
    else
    {
      gmm_set_substate( GMM_ATTACH_NEEDED );
    }
    
#ifdef FEATURE_DSAC
    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
        (gmm_check_if_ps_cell_access_barred(ps_cell_access) == TRUE))         
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= Initiating LU in NMO1 as CS domain is getting unbarred or LA has changed");
      
      if((mm_location_information.location_update_status == UPDATED) &&
         (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
         (memcmp( &(mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ))
      {   
        if ((mm_possible_imsi_attach) && (mm_system_information.ATT))
        {
          if (mm_state == MM_IDLE)
          {
            mm_initiate_lu( MM_LU_CAUSE_IMSI_ATTACH );
          }          
          else
          {
            mm_pending_lu_request = IMSI_ATTACH;
          }
        }
        else if ((mm_cs_access_barred_to_unbarred) && (mm_pending_lu_request == T3212_EXPIRED))
        {
          if (mm_state == MM_IDLE)
          {
            mm_initiate_lu( MM_LU_CAUSE_PERIODIC );
          }          
          else
          {
            mm_pending_lu_request = T3212_EXPIRED;
          }        
        }
        else if ((mm_cs_access_barred_to_unbarred) && 
                 ((mm_last_procedure_to_update_registration & MM_LU) != MM_LU))
        {
          if (mm_state == MM_IDLE)
          {
            mm_initiate_lu( MM_LU_CAUSE_NORMAL );
          }          
          else
          {
            mm_pending_lu_request = NORMAL_LU;
          }        
        }        
      }
      else
      {   
        if (mm_state == MM_IDLE)
        {
          mm_initiate_lu( MM_LU_CAUSE_NORMAL );
        }          
        else
        {
          mm_pending_lu_request = NORMAL_LU;
        }
      }  
      mm_cs_access_barred_to_unbarred = FALSE;                 
    }               
#endif  
  }
#ifndef FEATURE_CLASS_B_OPERATION_IN_NMO3
  else if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
           (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3) &&
           (mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE))
  {
    MSG_HIGH_DS_0( MM_SUB, "=MM= GPRS Attach blocked - NMO=3");
    gmm_set_state( GMM_DEREGISTERED );
  }
#endif
  else if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)
  {
    MSG_HIGH_DS_0( MM_SUB, "=MM= GPRS Attach blocked - ms_op_mode=CS_ONLY");
    gmm_set_state( GMM_DEREGISTERED );
  }
#ifdef FEATURE_NAS_FREE_FLOAT
  else if((is_gsm_mode()) && 
           gmm_detach_in_progress == TRUE)
  {
    gmm_gprs_attach_pending = TRUE;
    MSG_HIGH_DS_0( MM_SUB, "=MM= GPRS Attach blocked - DEATCH pending" );
  }
#endif
#ifdef FEATURE_REL6_PS_FOP
  else if((mm_timer_status [ TIMER_T3340 ] == TIMER_ACTIVE) && (gmm_ps_fop_resp == PS_FOP_NW_NOT_GRANTED) )
  {
    /**************************************************************
    *24.008 4.7.1.9 The MS shall start the timer T3340  when the  *
    * network indicates "no follow-on proceed" in the             *
    *ROUTING AREA UPDATE ACCEPT or ATTACH ACCEPT message.         *
    *if the MS has signalling pending, then it shall request a    *
    * new PS signalling connection for further signalling         *
    **************************************************************/
    gmm_gprs_attach_pending = TRUE;
    MSG_HIGH_DS_0( MM_SUB,"=MM= GPRS Attach blocked - T3340 is running ");
  }
#endif
  else
  {
    /* ------------------------------------------------------------
    ** Set GPRS attach type based on network and MS operating modes
    ** ------------------------------------------------------------ */
    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) ||
         ( (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2) 
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
          || (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
         )
       )
    {
      gmm_attach_type = GMM_GPRS_ATTACH;
    }
    else
    {
      gmm_attach_type = GMM_COMBINED_GPRS_IMSI_ATTACH;
/*Reset LU flag while doing Attach*/    
#ifdef FEATURE_NAS_REL10
#ifdef FEATURE_LTE 
      mm_is_lu_to_be_initiated_t3346_ltogw = FALSE;
#endif
#endif
    }
#ifdef FEATURE_NAS_REL11
/*Reset PS handover RAU initiation flag*/
      gmm_is_lw_to_g_interrat_in_progress = FALSE;
#endif
    /* If the SIM is nt valid for CS services set the
       gmm_attach_type as GMM_GPRS_ATTACH
    */
    if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
    {
       gmm_attach_type = GMM_GPRS_ATTACH;
    }
    if (mm_state != MM_IDLE && mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
    {    
      gmm_attach_type = GMM_GPRS_ATTACH;
    }
#ifdef FEATURE_DSAC
    {
      mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);

      if (mm_check_if_cs_cell_access_barred(cs_cell_access) == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= Attach Type: GMM_GPRS_ATTACH. CS Cell Access Blocked");
        gmm_attach_type = GMM_GPRS_ATTACH;
        mm_ps_access_barred_to_unbarred = FALSE;        
      }
    }
    
    if (gmm_attach_type == GMM_GPRS_ATTACH &&
        mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1 &&
        mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
    {
      mm_last_procedure_to_update_registration =
         (mm_last_procedure_to_update_registration & (~MM_COMBINED));
      mm_last_procedure_to_update_registration |= MM_RAU;
    }
    else if ( gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH )
    {
      mm_last_procedure_to_update_registration = MM_COMBINED;
    }  
#endif

    mm_stop_timer( TIMER_T3311 );
    mm_stop_timer( TIMER_T3302 );
#ifdef FEATURE_DUAL_SIM
    mm_stop_timer( TIMER_GMM_RADIO_RETRY );
#endif
    mm_stop_timer(TIMER_T3340);
    mm_stop_timer(TIMER_PS_CONN_REL);
    mm_stop_timer( TIMER_T3310);
#ifdef FEATURE_NAS_REL10
    if(mm_timer_status[TIMER_T3346]== TIMER_ACTIVE)
    {
      mm_stop_timer(TIMER_T3346);
      gmm_is_congestion_due_to_rau      = FALSE;
      sys_plmn_undefine_plmn_id(&gmm_3gpp_t3346_plmn_id);
    }
    if (gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH && 
        mm_timer_status[TIMER_T3246] == TIMER_ACTIVE && mm_is_plmn_equivalent_t3246_plmn() == FALSE)
    {
      mm_stop_timer(TIMER_T3246);
      sys_plmn_undefine_plmn_id(&mm_3gpp_t3246_plmn_id);
    }
#endif

    mm_clear_regs_complete_counter();

    if (t3310_timeout_counter == 0)
    {
      gmm_stop_auth_fail_timers();
    }

    gmm_delete_gprs_forbidden_rai();

    if ((is_gsm_mode()) || (ps_session_status == MMRRC_ACTIVE))
    {
       mm_start_timer( TIMER_T3310, DEFAULT_TIMEOUT );
    }
    gmm_set_state( GMM_REGISTERED_INITIATED );

    
#ifdef FEATURE_ENHANCED_NW_SELECTION
    if(reg_sim_read_ens_flag() && (mm_foreground_search_lai_timer_list.length != 0))
    {
      gmm_update_ps_attempt_counter_from_foreground_search_lai_rai_timer_list((byte*)&gmm_current_lai,gmm_current_rac);
    }
#endif

    mm_serv_change_pending  = FALSE;
    gmm_gprs_attach_pending = FALSE;
#ifdef FEATURE_GSM_GPRS
    gmm_rau_is_required     = FALSE;
#endif /*FEATURE_GSM_GPRS*/

    gmm_is_ptmsi_present_in_prev_gmm_msg = FALSE;
    gmm_is_tmsi_present_in_prev_gmm_msg  = FALSE; 

    mm_is_gmm_moved_dereg_due_to_csreg   = FALSE;

#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
    gmm_force_registration = FALSE;
#endif

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
    mm_pending_lu_type = MM_LU_CAUSE_RESERVED;
#endif

    /* Storing the NMO as communication with NW is started on the PS */
    gmm_previous_network_op_mode = mm_serving_plmn.network_op_mode;

    (void) memscpy(&(gmm_previous_registration_serving_rai.plmn_id), 3, &(mm_serving_plmn.info.plmn), 3);
    (void) memscpy(&(gmm_previous_registration_serving_rai.location_area_code), 2, &(mm_serving_plmn.lac), 2);
    gmm_previous_registration_serving_rai.routing_area_code = mm_serving_plmn.rac;
    
    gmm_previous_T3311_gmm_substate = GMM_NULL_SUBSTATE; 

    gmm_foreground_search_pending = FALSE;

    /* ---------------------------------------------
    ** Send an ATTACH REQUEST message to the network
    ** --------------------------------------------- */
    gmm_send_attach_request_message( gmm_attach_type,
                      mm_get_current_security_context( RRC_PS_DOMAIN_CN_ID )) ;

#ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
    mm_save_rat_pri_list_sent_to_nw();
#endif

    if(t3310_timeout_counter == 0)
    {
      mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_START,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
    }
    else
    {
      mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_RESEND,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
    }
    mm_update_last_reg_dsac_info();

#ifdef FEATURE_DUAL_SIM
    if(MM_MODEM_STATISTICS_SUB1)
#endif
    {
    mm_set_signal_for_cm(SYS_NAS_ATTACH_TYPE_MASK);
    }

    /*
    ** Pending RAU should not be performed if ATTACH is done.
    ** RAU was pending when UE goes OOS during GMM SUSPENDED state.
    */
    gmm_ra_update_pending    = FALSE;
    mm_gs_association_established = FALSE;
    if (mm_state != MM_IDLE && mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
    {    
      gmm_pending_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
      gmm_ra_update_pending = TRUE;
    }
    if (gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH)
    {
      (void) memscpy( mm_previous_serving_lai, LAI_SIZE,
                     mm_serving_lai,
                     LAI_SIZE );

      /* Remember that GS association is done in combine procedure */
      MSG_MED_DS_0( MM_SUB, "=MM= GS association is established in combined ATTACH" );
      mm_gs_association_established = TRUE;
      mm_initiate_lu_upon_entry_into_new_cell = FALSE;
      mm_lu_pended_nmo_unknown = FALSE;
      mm_pending_lu_request = NO_LU;
	  mm_previous_idle_substate = mm_idle_substate;
      if((mm_location_information.location_update_status != UPDATED) ||
         ((memcmp(&(mm_location_information.lai[0]),&(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE) != 0 ) ||
          (memcmp(&(mm_location_information.lai[PLMN_SIZE]),&(mm_serving_plmn.lac.lac[0]), LAC_SIZE) != 0)))
      {
        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                        SYS_SRV_DOMAIN_NO_SRV,
                                        FALSE,
                                        SYS_SRV_STATUS_LIMITED,
                                        FALSE );
      }
//    mm_state_control( MM_STD_COMBINED_RA_UPDATE_INITIATED );
#if defined(FEATURE_LTE)  
      emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_NOT_REQ;
      if(mm_csfb_state == MM_CSFB_STATE_LU_PENDING)
      {
        mm_csfb_set_state(MM_CSFB_STATE_NULL);
    }
  
#endif
#ifdef FEATURE_GSTK 
      /* Location Status Event to SIM may be required since LOCI is updated */
      if ((!reg_nv_is_imsi_switch_enabled()))
      {
         mm_gstk_set_location_status_evt (TRUE);
      }
#endif 
    }
  }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
  gmm_is_prev_mode_connected = FALSE;
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

} /* end gmm_initiate_gprs_attach() */


/*===========================================================================

FUNCTION GMM_INCREMENT_GPRS_ATTACH_ATTEMPT_COUNTER

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_increment_gprs_attach_attempt_counter( void )
{
  mmr_service_ind_s_type mmr_service_ind;
  mmr_cause_e_type       mmr_cause;
  byte location_information[LAI_SIZE];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------
  ** Refer to TS 24.008 section 4.7.3.1.5
  ** ------------------------------------ */
  mm_stop_timer( TIMER_T3310 );
  t3310_timeout_counter = 0;
  gprs_attach_attempt_counter++;
  MSG_HIGH_DS_2(MM_SUB, "=MM= T3310 timeout counter = %d and GPRS Attach Attempt counter = %d", t3310_timeout_counter, gprs_attach_attempt_counter);

  gmm_stop_auth_fail_timers();
  
#ifdef FEATURE_ENHANCED_NW_SELECTION 
    if(reg_sim_read_ens_flag() && mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai))
    {
      if (!((gmm_rai_changed( gmm_previous_registration_serving_rai.location_area_code,
                               gmm_previous_registration_serving_rai.routing_area_code,
                               mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
            (gmm_plmn_changed( gmm_previous_registration_serving_rai.plmn_id,
                               mm_serving_plmn.info.plmn))))
      {
        gmm_add_rac_to_foreground_search_lai_rai_timer_list((byte*)&gmm_current_lai,gmm_current_rac);
      }
    }
#endif

  
  if (gprs_attach_attempt_counter < 5)
  {
    mm_start_timer( TIMER_T3311, DEFAULT_TIMEOUT );
    gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH );
    gmm_set_state( GMM_DEREGISTERED );
    gmm_previous_T3311_gmm_substate = gmm_substate; 

    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
        (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY))
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

    /* Making it as per the section 4.7.3.2.5 */

    if (gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH)
    {
       /*
          if the update status is U1 UPDATED, and the stored LAI is equal
          to the one of the current serving cell and the attach attempt counter
          is smaller than 5, then the mobile station shall keep the
          update status to U1 UPDATED, the new MM state is MM IDLE substate NORMAL SERVICE
       */

      location_information[0] = 0xFF;
      location_information[1] = 0xFF;
      location_information[2] = 0xFF;
      location_information[3] = 0x00;
      location_information[4] = 0x00;
      if(memcmp(mm_serving_lai,location_information,PLMN_SIZE) == 0)
       {
         memscpy(location_information,3,&gmm_previous_registration_serving_rai.plmn_id,3);
         memscpy(location_information+3,2,&gmm_previous_registration_serving_rai.location_area_code,2);
       }
       else
       {
         memscpy(location_information,LAI_SIZE,mm_serving_lai,LAI_SIZE);
       }

       if (( mm_location_information.location_update_status == UPDATED ) &&
           ( memcmp( mm_location_information.lai, location_information, LAI_SIZE ) == 0))
       {

          mm_substate_control( MM_STD_NORMAL_SERVICE );
#ifdef FEATURE_NAS_REL10
          if(mm_is_per_ms_t3212_present)
          {
              if ((mm_stored_per_ms_t3212_value != 0 ) &&
                  (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
              {

                mm_start_timer( TIMER_T3212,
                                mm_stored_per_ms_t3212_value);
              }
          }
          else
#endif
          if ((mm_system_information.T3212_timer_value != 0 ) &&
              (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
          {
            /* -----------------------------------------------------------------
            ** If the (T3212) timer is not already started, the timer is started
            ** each time the mobile enters the MM_IDLE substate NORMAL_SERVICE
            ** or ATTEMPTING_TO_UPDATE (24.008 section 4.4.2).
            ** ----------------------------------------------------------------- */
            mm_start_timer( TIMER_T3212,
                            mm_system_information.T3212_timer_value );
          }
       }
       else
       {
          mm_delete_lu_status( NOT_UPDATED );

          /* Delete equivalent PLMN list */
          mm_serving_plmn.update_equiv_plmn_list = TRUE;
          memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));
          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           SYS_SRV_DOMAIN_NO_SRV,
                                           FALSE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE );
       }
    }

    if (mm_reg_waiting_for_reg_cnf)
    {
      if ((mm_ready_to_send_reg_cnf) &&
          (mm_serving_plmn.info.reg_service_domain != SYS_SRV_DOMAIN_NO_SRV))
      {
        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
        mm_send_mmr_reg_cnf();

        mm_reg_waiting_for_reg_cnf = FALSE;
        mm_ready_to_send_reg_cnf   = FALSE;
      }
    }
  }
  else /* gprs_attach_attempt_counter >= 5 */
  {
    gmm_delete_ps_location_information();
    gmm_delete_gprs_cksn();
    gmm_update_status = GMM_GU2_NOT_UPDATED;
    mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                        gmm_stored_ptmsi_signature,
                                        gmm_stored_rai,
                                        gmm_update_status );

#if defined(FEATURE_LTE)
    if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
    {
      emm_set_emm_param_attach_abnormal_failure();
    }
#endif

    mmr_cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;

    /* Reset only PS side flag, since LU procedure will be triggered now for max attempt of combined procedure */
     gmm_manual_mode_user_selection_procedure = FALSE;
    
    /*----------------------------------------------------------
    ** The mobile shall delete any LAI, TMSI, CKSN stored in SIM
    ** in case of Combined Attach procedure, refer 4.7.3.2.5
    **----------------------------------------------------------*/
    if (gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH)
    {
       mm_delete_lu_status( NOT_UPDATED );

#ifdef FEATURE_NAS_REL10
        if(mm_is_per_ms_t3212_present)
        {
            if ((mm_stored_per_ms_t3212_value != 0 ) &&
                (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
            {
              mm_start_timer( TIMER_T3212,
                              mm_stored_per_ms_t3212_value);
            }
        }
        else
#endif
       if ((mm_system_information.T3212_timer_value != 0 ) &&
           (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
       {
         /* -----------------------------------------------------------------
         ** If the (T3212) timer is not already started, the timer is started
         ** each time the mobile enters the MM_IDLE substate NORMAL_SERVICE
         ** or ATTEMPTING_TO_UPDATE (24.008 section 4.4.2).
         ** ----------------------------------------------------------------- */

         mm_start_timer( TIMER_T3212,
                         mm_system_information.T3212_timer_value );
       }

       /* Delete equivalent PLMN list */
       mm_serving_plmn.update_equiv_plmn_list = TRUE;
       memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));

       mmr_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;

       if ((mm_state == MM_WAIT_FOR_RR_CONNECTION_MM) ||
            (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
            (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) ||
            (mm_state == MM_WAIT_FOR_REESTABLISH_DECISION) ||
            (mm_state == MM_WAIT_FOR_RR_CONNECTION_REESTABLISHMENT) ||
            (mm_state == MM_REESTABLISHMENT_INITIATED) ||
            (mm_state == MM_CONNECTION_ACTIVE) ||
            ((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) && 
              (((mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS) &&
                 (mm_last_paging_cause == (byte)CS_PAGING)) ||
                 ((mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS) &&
                  (mm_last_paging_cause != 0xFF)) ||
                 ((mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS) &&
                  (mm_last_paging_cause != 0xFF))))
#ifdef FEATURE_CCBS
                ||
            (mm_state == MM_PROCESS_CM_SERVICE_PROMPT)
#endif /* FEATURE_CCBS */

          )
       {
          gmm_foreground_search_pending = TRUE;
         }
         else
         {
          mmr_cause = MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH;
          gmm_foreground_search_pending = FALSE;
         }
       }
#ifdef FEATURE_NAS_REL10
    if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
       (mm_timer_status[TIMER_T3246]== TIMER_ACTIVE))
       {
        mmr_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
       }
#endif
    if (gmm_t3302_timer_value != 0)
    {
      mm_start_timer( TIMER_T3302, gmm_t3302_timer_value );
    }

    /*----------------------------------------------------------
    ** The Equivalent PLMN List is deleted
    ** (24.008 section 4.7.3.1.5, 4.7.3.2.5)
    **----------------------------------------------------------*/
    mm_serving_plmn.update_equiv_plmn_list = TRUE;
    memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));

    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
        (mm_location_information.location_update_status == UPDATED) &&
        (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0) &&
        (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY))
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

#ifdef FEATURE_GSTK 
    if(mmr_cause == MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH)
    {
      mm_send_gstk_mm_idle_info();
    }
#endif

    if (mm_reg_waiting_for_reg_cnf)
    {
      if (mm_ready_to_send_reg_cnf)
      {
        mmcoord_mmr_reg_cnf.cause         = mmr_cause;
        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
        mm_send_mmr_reg_cnf();

        mm_reg_waiting_for_reg_cnf = FALSE;
        mm_ready_to_send_reg_cnf   = FALSE;
      }
    }
    else
    {
      mmr_service_ind.cause         = mmr_cause;
      mmr_service_ind.service_state = mm_serving_plmn.info;
      mm_send_mmr_service_ind( &mmr_service_ind );
    }

    gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH );
    gmm_set_state( GMM_DEREGISTERED );
    gmm_previous_T3311_gmm_substate = gmm_substate;    
  }
} /* end gmm_increment_gprs_attach_attempt_counter() */


/*===========================================================================

FUNCTION GMM_HANDLE_GPRS_ATTACH

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_handle_gprs_attach( mm_cmd_type *message )
{
  gmm_attach_accept_msg_T* attach_accept_msg;
  gmm_attach_reject_msg_T attach_reject_msg;
  mmr_service_ind_s_type  mmr_service_ind;
  inter_task_lai_T        registered_lai;
  boolean                 allocated_tmsi_present = FALSE;
#ifdef FEATURE_WCDMA
  rrc_cmd_type            *rrc_cmd;
#endif /* #ifdef FEATURE_WCDMA */
  sm_cmd_type             *sm_cmd;
  byte                    i;
  dword                   t3312_timer_value = 0;
  byte                    previous_plmn_identity[PLMN_SIZE];
 #if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = emm_db_get_ctrl_data(); 
  boolean send_lte_reselection_ind = FALSE;
#endif

  mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION); 
  mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION); 

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_NAS_REL10
  gmm_t3346_backoff_timer_value = 0;
#endif


  if (message == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= Unexpected NULL pointer", 0,0,0 );
  }

  switch (message->cmd.hdr.message_set)
  {
    case MS_MM_RR:
      switch (message->cmd.hdr.message_id)
      {
        case RR_ACTIVATION_IND:
          /* -------------------
          ** Lower layer failure
          ** ------------------- */
          gmm_increment_gprs_attach_attempt_counter();


          mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
          /* -------------------
          ** TS 24.008 4.2.4.2.2
          ** ------------------- */
          gprs_attach_upon_entry_to_a_new_cell = TRUE;
          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected RR message %d", message->cmd.hdr.message_id, 0,0 );
      } /* end switch (message->cmd.hdr.message_id) */
      break;

    case MS_MM_RRC:
    case MS_MM_TDSRRC:
      switch (message->cmd.hdr.message_id)
      {
        case RRC_ACTIVATION_IND:
          /* -------------------
          ** Lower layer failure
          ** ------------------- */
          gmm_increment_gprs_attach_attempt_counter();

     
            mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
          /* -------------------
          ** TS 24.008 4.2.4.2.2
          ** ------------------- */
          gprs_attach_upon_entry_to_a_new_cell = TRUE;
          break;

        case RRC_ABORT_IND:
          if (message->cmd.rrc_abort_ind.abort_cause == RRC_AC_NOT_ALLOWED_PS_DOMAIN)
          {
            /* ---------------------------------------------
            ** Access barred because of access class control
            ** --------------------------------------------- */
            gmm_set_substate( GMM_ATTACH_NEEDED );
            gmm_update_cell_status(message->cmd.rrc_abort_ind.abort_cause);
            gmm_set_state( GMM_DEREGISTERED );

            if((mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE)&&
               (gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH)&&
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
            {
              if (mm_state == MM_IDLE)
              {
                MSG_HIGH_DS_1(MM_SUB, "=MM= CS Registration started due to AC blocking for PS domain %d",
                                                                                                       ps_cell_access);
                mm_initiate_lu(MM_LU_CAUSE_NORMAL);
              }
              else
              {
                mm_pending_lu_request = NORMAL_LU;
              }
            }
          }
          else if (message->cmd.rrc_abort_ind.abort_cause == RRC_AC_NOT_ALLOWED_CS_DOMAIN)
          {
            /* ---------------------------------------------
            ** CS Access barred because of access class control
            ** --------------------------------------------- */
            if ((gmm_check_if_ps_cell_access_allowed(ps_cell_access) == TRUE)&&
                 (gmm_attach_type == GMM_GPRS_ATTACH))
            {
              MSG_HIGH_DS_1(MM_SUB, "=MM= PS Registration started due to AC blocking for CS domain %d",
                                                                                                    cs_cell_access);
              gmm_initiate_gprs_attach();
            }
            else
            {
               gmm_set_substate( GMM_ATTACH_NEEDED );
               gmm_set_state( GMM_DEREGISTERED );
            }

          }
#ifdef FEATURE_SILENT_REDIAL_AFTER_RA_LA_MISMATCH
          else if (message->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_LAI_RAI_CHANGED)
          {
            gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH );
            gmm_set_state( GMM_DEREGISTERED );
          }
#endif
#ifdef FEATURE_DUAL_SIM
          else if (message->cmd.rrc_abort_ind.abort_cause == RRC_NO_RESOURCE_AVAILABLE ||
                    message->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_CONN_REJECT_WAIT_TMR)
          {
            gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH );
            gmm_set_state( GMM_DEREGISTERED );
            if(message->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_CONN_REJECT_WAIT_TMR)
            {
              mm_start_timer( TIMER_GMM_RADIO_RETRY, message->cmd.rrc_abort_ind.conn_rej_wait_timer*1000);
            }
            else
            {
              mm_start_timer( TIMER_GMM_RADIO_RETRY, DEFAULT_TIMEOUT );
            }
          }
#endif
          else
          {
            /* -------------------
            ** Lower layer failure
            ** ------------------- */
            gmm_increment_gprs_attach_attempt_counter();

          
            mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
            /* -------------------
            ** TS 24.008 4.2.4.2.2
            ** ------------------- */
            gprs_attach_upon_entry_to_a_new_cell = TRUE;
          }
          break;

        case RRC_OPEN_SESSION_CNF:
        case RRC_CLOSE_SESSION_IND:
        case RRC_CLOSE_SESSION_CNF:
        case RRC_REL_IND:
        case RRC_ABORT_CNF:
          /* -------------------
          ** Lower layer failure
          ** ------------------- */
          gmm_increment_gprs_attach_attempt_counter();
           mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);

          /* -------------------
          ** TS 24.008 4.2.4.2.2
          ** ------------------- */
          gprs_attach_upon_entry_to_a_new_cell = TRUE;
          break;

        case MM_DATA_IND:
          switch (message->cmd.mm_data_ind.L3_data[1])
          {
            case GMM_ATTACH_ACCEPT:
#ifndef FEATURE_MODEM_HEAP
              attach_accept_msg = (gmm_attach_accept_msg_T*)gs_alloc(sizeof(gmm_attach_accept_msg_T));
#else
              attach_accept_msg = (gmm_attach_accept_msg_T*)modem_mem_calloc(1, sizeof(gmm_attach_accept_msg_T), MODEM_MEM_CLIENT_NAS);
#endif
              mm_check_for_null_ptr((void*)attach_accept_msg);
              if (gmm_valid_attach_accept_message( message, attach_accept_msg ))
              {
                TTLBOOTUP(WG_PS_REG_SUCCESS);
                /* ---------------------------------------------------------------
                ** The MS, upon receiving an ATTACH ACCEPT message, stores the
                ** received routing area identification, stops timer T3310,
                ** resets the GPRS attach attempt counter, resets the routing area
                ** update attempt counter, enters state GMM-REGISTERED and sets
                ** the GPRS update status to GU1 UPDATED.
                ** --------------------------------------------------------------- */
                mm_stop_timer( TIMER_T3310 );
                t3310_timeout_counter       = 0;
                MSG_HIGH_DS_1( MM_SUB, "=MM= T3310 timeout counter = %d", t3310_timeout_counter);

                if ((gmm_state == GMM_REGISTERED_INITIATED) && (is_gsm_mode()))
                {
                  mm_start_timer(TIMER_T3350, DEFAULT_TIMEOUT);
                } 
                gmm_update_status = GMM_GU1_UPDATED;
                gmm_processing_directed_signalling_reestablishment = FALSE;
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                if (is_gsm_mode())
                {
                  gmm_transaction_id++;

                  MSG_HIGH_DS_0(MM_SUB,"=MM= Transaction id Incremented Since ATTACH ACCEPT is received ");
                }
#endif

                registered_lai.PLMN_id.identity[0] =
                  attach_accept_msg->routing_area_identification[0];
                registered_lai.PLMN_id.identity[1] =
                  attach_accept_msg->routing_area_identification[1];
                registered_lai.PLMN_id.identity[2] =
                  attach_accept_msg->routing_area_identification[2];
                registered_lai.location_area_code[0] =
                  attach_accept_msg->routing_area_identification[3];
                registered_lai.location_area_code[1] =
                  attach_accept_msg->routing_area_identification[4];

                if((!mm_is_plmn_valid(registered_lai.PLMN_id)) || 
                   ((mm_reg_waiting_for_reg_cnf) && 
                   ((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
                    (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
                     mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)) &&
                   !(sys_plmn_match(mm_serving_plmn.info.plmn , registered_lai.PLMN_id ) ||
                    (sys_eplmn_list_equivalent_plmn(registered_lai.PLMN_id)))))
                {
                  MSG_HIGH_DS_3(MM_SUB, "=MM= MCC/MNC %02x %02x %02x invalid -> retry Attach",
                           registered_lai.PLMN_id.identity[0],
                           registered_lai.PLMN_id.identity[1],
                           registered_lai.PLMN_id.identity[2]);
                  if(gmm_state==GMM_REGISTERED_INITIATED)
                  {
                    gmm_increment_gprs_attach_attempt_counter();
                    mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                  }
#ifndef FEATURE_MODEM_HEAP
                  gs_free(attach_accept_msg);
#else
                  modem_mem_free(attach_accept_msg, MODEM_MEM_CLIENT_NAS);
#endif
                  return;
                }

                /* reset the gmm suspended flag */
                gmm_is_suspended = FALSE;

                if(gmm_state==GMM_REGISTERED)
                {
                   if (attach_accept_msg->ms_identity_present &&
                       ((attach_accept_msg->ms_identity.id_type & 0x7) == 0x4))
                   {
                      allocated_tmsi_present = TRUE;
                   }
                   if (attach_accept_msg->allocated_ptmsi_present || allocated_tmsi_present)
                   {
                    /* ----------------------------------------------
                                  ** Send an ATTACH COMPLETE message to the network
                                  ** ---------------------------------------------- */
                      gmm_send_attach_complete_message();
                      if(attach_accept_msg != NULL)
                      {
#ifndef FEATURE_MODEM_HEAP
                        gs_free(attach_accept_msg);
#else
                        modem_mem_free(attach_accept_msg, MODEM_MEM_CLIENT_NAS);
#endif
                      }
                      return;
                   }
                }

#ifdef FEATURE_DUAL_SIM
                if(MM_MODEM_STATISTICS_SUB1)
#endif
                {
                t3312_timer_value = gmm_t3312_timer_value;
                }

               /*
            ** TS 24.008 4.7.3.1.3 V10.7.0 Extended Routing Area Update Timer
            ** If the ATTACH ACCEPT message contains T3312 extended value IE, 
            ** then the MS shall use the T3312 extended value IE as periodic routing area update timer (T3312). 
            ** If the ATTACH ACCEPT message does not contain T3312 extended value IE, 
            ** then the MS shall use value in T3312 value IE as periodic routing area update timer (T3312).        
            */                
#ifdef FEATURE_NAS_REL10
                if (attach_accept_msg->ext_t3312_value_present)
                {
                    /* ---------------------
                    ** Set T3312 extended timer value
                    ** --------------------- */
                    gmm_t3312_timer_value = gmm_convert_timer3_value_ie_to_msecs(
                                                      attach_accept_msg->ext_t3312_value );
                }
                else
#endif
                {
                /* ----------------------------
                ** Set Periodic RAU timer value
                ** ---------------------------- */
                  gmm_t3312_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                                   attach_accept_msg->periodic_ra_update_timer );
#ifdef FEATURE_NAS_REL11
                   /*
				   ** 24.008: 4.7.2.2
                   ** In Iu mode, if the value of timer T3312 is received in a message without integrity protection 
                   ** and the indicated value is larger than the last received value, or the indicated value is "deactivated" or zero, 
                   **the MS shall use the last received value.
				   */
                  if((is_umts_mode()) &&
                     (gmm_integrity_protection_activated == FALSE) &&
                     ((gmm_t3312_timer_value > t3312_timer_value) ||
                     (gmm_t3312_timer_value == 0)))
                  {
                    gmm_t3312_timer_value = t3312_timer_value;
                  }
#endif
                }

#ifdef FEATURE_DUAL_SIM
                if(MM_MODEM_STATISTICS_SUB1)
#endif
                {
                if (t3312_timer_value != gmm_t3312_timer_value)
                {
                  mm_set_signal_for_cm(SYS_NAS_PERIODIC_TIMER_MASK);
                }
                }
#ifdef FEATURE_GSM_GPRS
                sms_radio_priority    = attach_accept_msg->sms_radio_priority_nw;
                cell_notification_ind = attach_accept_msg->cell_notification_present ;
#endif

                if (attach_accept_msg->t3302_value_present)
                {
                  /* ---------------------
                  ** Set T3302 timer value
                  ** --------------------- */
#ifdef FEATURE_NAS_REL11
                  /*Store PLMN id on which we got T3302 value*/
                  gmm_previous_plmn_with_t3302_value.identity[0] = attach_accept_msg->routing_area_identification[0];
                  gmm_previous_plmn_with_t3302_value.identity[1] = attach_accept_msg->routing_area_identification[1];
                  gmm_previous_plmn_with_t3302_value.identity[2] = attach_accept_msg->routing_area_identification[2];

                  /*24.008:4.7.2.7
                  The default value of this timer is used in Iu mode, if the network provides a value in a non-integrity protected Iu mode GMM message and 
                  the MS is not attaching for emergency services or not attached for emergency services */
                  if ((is_umts_mode ()) &&
                        (gmm_integrity_protection_activated == FALSE))
                  {
                    gmm_t3302_timer_value = 720000;
                  }
                  else
#endif
                  {
                    gmm_t3302_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                            attach_accept_msg->t3302_value );
                  }
                }
#ifdef FEATURE_NAS_REL11
                else
                {
                /*24.008:4.7.2.7
                  The default value of this timer is used if ATTACH ACCEPT message, ROUTING AREA UPDATE ACCEPT message, 
                  ATTACH REJECT message, or ROUTING AREA UPDATE REJECT message is received without a value specified*/
                  gmm_t3302_timer_value = 720000;
                }
#endif
                if (attach_accept_msg->t3319_value_present)
                {
                  /* ---------------------
                  ** Set T3319 timer value
                   ** --------------------- */
                  gmm_t3319_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                            attach_accept_msg->t3319_value );
                } 
                if ((!attach_accept_msg->t3319_value_present ) ||(!gmm_t3319_timer_value)) 
                {
                  gmm_t3319_timer_value = 30000;
                }

#if defined(FEATURE_LTE)
                if (attach_accept_msg->t3323_value_present)
                {
                  /* ---------------------
                  ** Set T3323 timer value
                  ** --------------------- */
                  gmm_t3323_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                            attach_accept_msg->t3323_value );
                }
                /* ------------------------------------------------
                ** If network didnot provide the T3323 value then it should be
                ** set to the same value as T3312.
                ** --------------------- --------------------------*/
                else
                {
                  gmm_t3323_timer_value = gmm_t3312_timer_value;
                }
                gmm_reg_lte_cs_param = 
                    mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param;
                gmm_reg_pended_lte_cs_param =
                    mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param;
#endif

                mm_serving_plmn.update_equiv_plmn_list = TRUE;

                if (attach_accept_msg->equiv_plmn_list_present)
                {
                  /*
                  ** Copy the equivalent PLMN list to the mm_serving_plmn structure
                  */
                  mm_serving_plmn.equiv_plmn_list.length = attach_accept_msg->equiv_plmn_list.length;
                  memscpy(mm_serving_plmn.equiv_plmn_list.plmn, sizeof(sys_plmn_id_s_type)*EQUIVALENT_PLMN_LIST_MAX_LENGTH,
                         attach_accept_msg->equiv_plmn_list.plmn,
                         attach_accept_msg->equiv_plmn_list.length);
                }
                else
                {
                  /* If the msg does not contain an equivalent plmn list, the stored list
                  *should be deleted*/

                  memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));
                }

#ifdef FEATURE_NAS_REL10
                if(attach_accept_msg->nw_ftr_support_present)
                {
                  gmm_stored_nw_feature_support = attach_accept_msg->nw_ftr_support;
                }
#endif

                /* ------------------------------------------------------------
                ** If the LAI or PLMN identity that has been transmitted in the
                ** ATTACH ACCEPT message is a member of any of the "forbidden"
                ** lists, any such entry shall be deleted.
                ** ------------------------------------------------------------ */
                mm_remove_forbidden_national_roaming( &registered_lai );
                mm_remove_forbidden_regional_roaming( &registered_lai );
                (void)memscpy(previous_plmn_identity,PLMN_SIZE, gmm_stored_rai.plmn_id.identity,PLMN_SIZE); 

                /* -------------------------------------
                ** Store the routing area identification
                ** ------------------------------------- */
                gmm_stored_rai.plmn_id.identity[0] =
                  attach_accept_msg->routing_area_identification[0];
                gmm_stored_rai.plmn_id.identity[1] =
                  attach_accept_msg->routing_area_identification[1];
                gmm_stored_rai.plmn_id.identity[2] =
                  attach_accept_msg->routing_area_identification[2];
                gmm_stored_rai.location_area_code.lac[0] =
                  attach_accept_msg->routing_area_identification[3];
                gmm_stored_rai.location_area_code.lac[1] =
                  attach_accept_msg->routing_area_identification[4];
                gmm_stored_rai.routing_area_code =
                  attach_accept_msg->routing_area_identification[5];

                mm_remove_plmn_from_fplmn_list( gmm_stored_rai.plmn_id);

#ifdef FEATURE_FEMTO_CSG
                mm_add_csg_to_white_list(gmm_stored_rai.plmn_id, &mm_serving_plmn.info.csg_info);
                mm_remove_csg_from_rejected_list(gmm_stored_rai.plmn_id,
                                                 mm_serving_plmn.info.csg_info.csg_id);
#endif
#ifdef FEATURE_LTE
                if(
#ifdef FEATURE_DUAL_SIM
                   reg_sim_plmn_gprs_forbidden(gmm_stored_rai.plmn_id,(sys_modem_as_id_e_type)mm_sub_id) &&
#else
                   reg_sim_plmn_gprs_forbidden(gmm_stored_rai.plmn_id) &&
#endif
                   mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS))
                {
                  send_lte_reselection_ind = TRUE;
                }
#endif
                mm_remove_plmn_from_gprs_fplmn_list( gmm_stored_rai.plmn_id);
                
#ifdef FEATURE_LTE
                 if(send_lte_reselection_ind)
                 {
                    if (is_umts_mode())
                    {
                      mm_send_rrc_lte_reselection_status_ind();
                    }
                    else if (is_gsm_mode())
                    {
                      mm_send_rr_lte_reselection_status_ind();
                    } 
                 }
#endif
                /*------------------------------------------------------------
                ** Update currently camped PLMN/LA/RA with the PLMN/LA/RA Id received
                ** in Attach Accept message. This may happen as RAU may have been performed
                ** due to DSAC spec rules, however, RRC didn't inform currently
                ** camped PLMN/LA/RA Id due to RRC spec prohibiting it from informing
                ** so in connected mode.
                **-------------------------------------------------------------*/  
                if ((gmm_plmn_changed( gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn )) ||
                    (gmm_rai_changed ( gmm_stored_rai.location_area_code,
                                       gmm_stored_rai.routing_area_code,
                                       mm_serving_plmn.lac, mm_serving_plmn.rac )))
                {
                  MSG_HIGH_DS_0(MM_SUB, "=MM= RA Id in Attach Accept and the one informed by RRC differ");
                                                                       
                  gmm_previous_serving_rai.plmn_id                   = gmm_current_lai.PLMN_id;
                  gmm_previous_serving_rai.location_area_code.lac[0] = gmm_current_lai.location_area_code[0];
                  gmm_previous_serving_rai.location_area_code.lac[1] = gmm_current_lai.location_area_code[1];
                  gmm_previous_serving_rai.routing_area_code         = gmm_current_rac;
                  
                  gmm_current_lai.PLMN_id.identity[0]   = mm_serving_plmn.info.plmn.identity[0] = gmm_stored_rai.plmn_id.identity[0];
                  gmm_current_lai.PLMN_id.identity[1]   = mm_serving_plmn.info.plmn.identity[1] = gmm_stored_rai.plmn_id.identity[1];
                  gmm_current_lai.PLMN_id.identity[2]   = mm_serving_plmn.info.plmn.identity[2] = gmm_stored_rai.plmn_id.identity[2];
                  gmm_current_lai.location_area_code[0] = mm_serving_plmn.lac.lac[0] =  gmm_stored_rai.location_area_code.lac[0];
                  gmm_current_lai.location_area_code[1] = mm_serving_plmn.lac.lac[1] =  gmm_stored_rai.location_area_code.lac[1];                  
                  gmm_current_rac                       = mm_serving_plmn.rac = gmm_stored_rai.routing_area_code;
                }                  

#ifdef FEATURE_GSM_GPRS
                memscpy((void *)&glif_ra_changed_rau_completed_ra, sizeof(glif_ra_changed_rau_completed_ra),
                           (void *)&gmm_stored_rai,sizeof(routing_area_id_T)) ;
#endif /* #ifdef FEATURE_GSM_GPRS */

                if (attach_accept_msg->allocated_ptmsi_present)
                {
                  /* -------------------------------------------
                  ** Delete the old P-TMSI and store the new one
                  ** ------------------------------------------- */
                  for (i=0; i < PTMSI_SIZE; i++)
                  {
                    gmm_stored_ptmsi.digit[i] =
                      attach_accept_msg->allocated_ptmsi[i];
                  }
                  gmm_is_ptmsi_present_in_prev_gmm_msg = TRUE;
                }

#if defined(FEATURE_LTE)
                emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);  
                /* Set EMM state with flag is attach true */
                emm_set_state_multimode_registration_success(TRUE);
                emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent = NAS_SEND_DRX_ON_GW_RAT;
#endif 

                if (attach_accept_msg->ptmsi_signature_present)
                {
                  /* -----------------------------------------------------
                  ** Delete the old P-TMSI signature and store the new one
                  ** ----------------------------------------------------- */
                  for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
                  {
                    gmm_stored_ptmsi_signature.value[i] =
                      attach_accept_msg->ptmsi_signature[i];
                  }
                }
                else
                {
                  /* -------------------------------
                  ** Delete the old P-TMSI signature
                  ** ------------------------------- */
                  for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
                  {
                    gmm_stored_ptmsi_signature.value[i] = 0xFF;
                  }
                }

                mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                    gmm_stored_ptmsi_signature,
                                                    gmm_stored_rai,
                                                    gmm_update_status );
                
      #ifdef FEATURE_LTE
                emm_update_rplmn_type(MM_PS_RPLMN_TYPE);
                mm_send_rrc_set_drx_ind();  
                emm_send_rrc_drx_update();
      #endif

                if ((gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH) ||
                    (gmm_attach_type == GMM_GPRS_ATTACH_WHILE_IMSI_ATTACHED))
                {
                  if (attach_accept_msg->attach_result == GMM_COMBINED_GPRS_IMSI_ATTACHED)
                  {
                  mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_ACCEPT,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                    /*The MS, receiving an ATTACH ACCEPT message, stores the received routing area identification, stops timer T3310,
                    reset the GPRS attach attempt counter, reset the routing area updating attempt counter, enters state GMMREGISTERED
                    and sets the GPRS update status to GU1 UPDATED*/
                    gmm_set_substate( GMM_NORMAL_SERVICE );
                    gmm_set_state( GMM_REGISTERED );
                    
                    gprs_attach_attempt_counter = 0;
                    MSG_HIGH_DS_1(MM_SUB, "=MM= GPRS Attach attempt counter = %d", gprs_attach_attempt_counter );                    
                    gmm_rau_attempt_counter     = 0;
                    MSG_HIGH_DS_1(MM_SUB, "=MM= GPRS RAU attempt counter = %d", gmm_rau_attempt_counter );
                  
                    mm_pending_lu_request = NO_LU;                  
                    mm_lu_start_reason = NO_LU;
                    gmm_irat_lu_pending = NO_LU;
                    mm_substate_control(MM_STD_NORMAL_SERVICE);
                    gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     SYS_SRV_DOMAIN_CS_PS,
                                                     FALSE,
                                                     SYS_SRV_STATUS_SRV,
                                                     TRUE );
#ifdef FEATURE_GSTK          
                    mm_send_gstk_mm_idle_info();
#endif                    

                    /* ---------------------------------------------------------
                    ** The MS, receiving an ATTACH ACCEPT message, stores the
                    ** received location area identification, stops timer T3310,
                    ** resets the location update attempt counter and sets the
                    ** update status to U1 UPDATED.
                    ** --------------------------------------------------------- */
                    mm_serving_lai[0] = mm_location_information.lai[0] =
                      attach_accept_msg->routing_area_identification[0];
                    mm_serving_lai[1] = mm_location_information.lai[1] =
                      attach_accept_msg->routing_area_identification[1];
                    mm_serving_lai[2] = mm_location_information.lai[2] =
                      attach_accept_msg->routing_area_identification[2];
                    mm_serving_lai[3] = mm_location_information.lai[3] =
                      attach_accept_msg->routing_area_identification[3];
                    mm_serving_lai[4] = mm_location_information.lai[4] =
                      attach_accept_msg->routing_area_identification[4];

                    mm_clear_lu_attempt_counter();

                    mm_location_information.location_update_status = UPDATED;

                    /* Combined attach accept, reset both manual mode user selection flags */
                    gmm_manual_mode_user_selection_procedure = FALSE;
                    mm_manual_mode_user_selection_procedure = FALSE;
#ifdef FEATURE_LTE
                    mm_set_emm_manual_selection_flag(FALSE);
#endif                    
                    /* ----------------------------------------------------------
                    ** If the message contains an IMSI, the mobile station is not
                    ** allocated any TMSI, and shall delete any TMSI accordingly.
                    ** If the message contains a TMSI, the MS shall use this TMSI
                    ** as the new temporary identity.  The MS shall delete its old
                    ** TMSI and shall store the new TMSI.  In this case, an ATTACH
                    ** COMPLETE message is returned to the network.  If neither a
                    ** TMSI nor an IMSI has been included by the network in the
                    ** ATTACH ACCEPT message, the old TMSI, if any available,
                    ** shall be kept.
                    ** ----------------------------------------------------------- */
                    if (attach_accept_msg->ms_identity_present)
                    {
                      if ((attach_accept_msg->ms_identity.id_type & 0x7) == 0x1)
                      {
                        (void)memset( mm_location_information.tmsi, 0xFF, TMSI_SIZE );
                      }
                      else if ((attach_accept_msg->ms_identity.id_type & 0x7) == 0x4)
                      {
                        mm_location_information.tmsi[0] =
                          attach_accept_msg->ms_identity.value[0];
                        mm_location_information.tmsi[1] =
                          attach_accept_msg->ms_identity.value[1];
                        mm_location_information.tmsi[2] =
                          attach_accept_msg->ms_identity.value[2];
                        mm_location_information.tmsi[3] =
                          attach_accept_msg->ms_identity.value[3];

                        allocated_tmsi_present = TRUE;
                        gmm_is_tmsi_present_in_prev_gmm_msg = TRUE;
                      }
                    }

                    /* -----------------------------------------------------------
                    ** Any timer used for triggering the location update procedure
                    ** (e.g., T3211, T3212) shall be stopped if running.
                    ** ----------------------------------------------------------- */
                    mm_stop_timer( TIMER_T3211 );
                    mm_stop_timer( TIMER_T3212 );
                    mm_stop_timer( TIMER_T3213 );
#ifdef FEATURE_DUAL_SIM
                    mm_stop_timer( TIMER_MM_RADIO_RETRY );
#endif

                    mm_idle_transition_timer = NULL;

                    /* --------------------------------
                    ** Copy location information to SIM
                    ** -------------------------------- */
                    mmsim_write_loci();
                    mm_possible_imsi_attach = FALSE;

                    if (mm_managed_roaming_enabled) 
                    {
                      reg_sim_clear_low_priority_plmn_list(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                                           (sys_modem_as_id_e_type)mm_as_id
#endif
                                                          );
                      mm_serving_plmn.info.cs_service_status  = SYS_SRV_STATUS_SRV;
                    }
#ifdef FEATURE_ENHANCED_NW_SELECTION
                    if (reg_sim_read_ens_flag()) 
                    {
                      mm_serving_plmn.info.cs_service_status  = SYS_SRV_STATUS_SRV;
                    }
#endif
                    /* --------------------------------------------------
                    ** The MM state will transition back to IDLE when the
                    ** RR(C) connection is released
                    ** -------------------------------------------------- */
                  }
                  else
                  { 
                    mm_gs_association_established = FALSE;
                    
                    mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_ACCEPT_PS_ONLY,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                    
                    if ( attach_accept_msg->gmm_cause_present)
                    {
                      switch (attach_accept_msg->gmm_cause)
                      {
                        case IMSI_UNKNOWN_IN_HLR:

                          gmm_set_substate( GMM_NORMAL_SERVICE );
                          gmm_set_state( GMM_REGISTERED );
                          gprs_attach_attempt_counter = 0;
                          MSG_HIGH_DS_1(MM_SUB, "=MM= GPRS Attach attempt counter = %d", gprs_attach_attempt_counter);                          
                          /* ----------------------------------------------------
                          ** The MS shall set the update status to U3 ROAMING NOT
                          ** ALLOWED and shall delete any TMSI, LAI and ciphering
                          ** key sequence number.  The SIM shall be considered as
                          ** invalid for non-GPRS services until switching off or
                          ** the SIM is removed.
                          ** ---------------------------------------------------- */
                          mm_delete_lu_status( ROAMING_NOT_ALLOWED );

                          mm_manual_mode_user_selection_procedure = FALSE;

                             /* -----------------------------------------------------------
                           ** Any timer used for triggering the location update procedure
                           ** (e.g., T3211, T3212) shall be stopped if running.
                           ** ----------------------------------------------------------- */

                          mm_stop_timer( TIMER_T3211 );
                          mm_stop_timer( TIMER_T3212 );
                          mm_stop_timer( TIMER_T3213 );

                          /* --------------------------------------------------
                          ** The MM state will transition back to IDLE when the
                          ** RR(C) connection is released
                          ** -------------------------------------------------- */

                          gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_INVALID,
                                                         SYS_SRV_DOMAIN_PS_ONLY,
                                                         FALSE,
                                                         SYS_SRV_STATUS_SRV,
                                                         TRUE );

                          mm_send_mmr_reg_reject_ind(
                                  SYS_SRV_DOMAIN_CS_ONLY,
                                  attach_accept_msg->gmm_cause,
                                  mm_serving_plmn.info.plmn,
                                  mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                  ,mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                  ,mm_serving_plmn.info.active_rat
                            );

#ifdef FEATURE_GSTK          
                          mm_send_gstk_mm_idle_info();
#endif                           
                          break;

#ifndef FEATURE_NAS_REL11
                        case MSC_TEMPORARILY_NOT_REACHABLE:
                        case NETWORK_FAILURE:
                        case CONGESTTION:
#else
/*24.008: 4.7.3.2.3.2
Other GMM cause values and the case that no GMM cause IE was received are considered as abnormal cases. 
The combined attach procedure shall be considered as failed for non-GPRS services. 
The behaviour of the MS in those cases is specified in subclause 4.7.3.2.5.*/
                        default:
#endif
                          /* --------------------------------------------------------
                          ** The MS shall change to state GMM-REGISTERED.ATTEMPTING-
                          ** TO-UPDATE-MM.  The routing area updating attempt counter
                          ** shall be incremented.  The MS shall start timer T3311.
                          ** When timer T3311 expires the combined routing area
                          ** update procedure indicating "combined RA/LA updating
                          ** with IMSI attach" is triggered.
                          ** -------------------------------------------------------- */
                          gmm_set_state( GMM_REGISTERED );
                          gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE_MM );
                          gmm_previous_T3311_gmm_substate = gmm_substate; 
                          
                          gprs_attach_attempt_counter = 0;
                          MSG_HIGH_DS_1(MM_SUB, "=MM= GPRS Attach attempt counter = %d", gprs_attach_attempt_counter );                          
                          gmm_rau_attempt_counter++;
#ifdef FEATURE_NAS_REL11
                          /*24.008: 4.7.3.2.3.2
                          # 22  (Congestion)
                          The MS shall change to state GMM-REGISTERED.ATTEMPTING-TO-UPDATE-MM.The timer T3310 shall be stopped if still running. 
                          The MS shall set the routing area updating attempt counter to 5 and shall start the timer T3302.*/
                          if(attach_accept_msg->gmm_cause == CONGESTTION)
                          {
                            if(mm_timer_status[TIMER_T3310] == TIMER_ACTIVE)
                            {
                               mm_stop_timer(TIMER_T3310);
                            }
                            gmm_rau_attempt_counter = 5;                            
                            mm_start_timer( TIMER_T3302, gmm_t3302_timer_value );                            
                          }
#endif
                          MSG_HIGH_DS_1(MM_SUB, "=MM= RAU Attempt counter = %d", gmm_rau_attempt_counter);

                          if(mm_managed_roaming_enabled && (attach_accept_msg->gmm_cause == NETWORK_FAILURE))
                          {
                            reg_sim_add_to_low_priority_plmn_list(mm_serving_plmn.info.plmn, mm_serving_plmn.info.active_rat
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                                                  ,(sys_modem_as_id_e_type)mm_as_id
#endif
                                                                 );
                          }
                      
                          mm_send_mmr_reg_reject_ind(
                                     SYS_SRV_DOMAIN_CS_ONLY,
                                     attach_accept_msg->gmm_cause,
                                     mm_serving_plmn.info.plmn,
                                     mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                    ,mm_serving_plmn.info.csg_info.csg_id
#endif
                                    ,mm_serving_plmn.info.active_rat
                              );

                          if ((gmm_rau_attempt_counter < 5) &&
                              (!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                                   gmm_stored_rai.routing_area_code,
                                                   mm_serving_plmn.lac,
                                                   mm_serving_plmn.rac )) ||
                                 (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
                              (gmm_update_status == GMM_GU1_UPDATED))
                          {
                            /* ---------------------------------------------------------
                            ** The MS shall keep the GMM update status to GU1 UPDATED.
                            ** The MS shall start timer T3311.  When timer T3311 expires
                            ** the combined routing area updating procedure indicating
                            ** "combined RA/LA updating with IMSI attach" is triggered.
                            ** --------------------------------------------------------- */
                            mm_start_timer( TIMER_T3311, DEFAULT_TIMEOUT );
                            gmm_ra_update_type =
                              GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
                          }
                          else if((gmm_rau_attempt_counter >= 5)
#ifdef FEATURE_NAS_REL11
                                  &&(attach_accept_msg->gmm_cause != CONGESTTION)
#endif
                                 )
                          {
                            mm_start_timer( TIMER_T3302, gmm_t3302_timer_value );
                            gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;

                            /* -----------------------------------------------------
                            ** A GPRS MS operating in MS operation mode A shall then
                            ** proceed with appropriate MM specific procedure. IT can be either IMSI 
                            ** Attach or nothing. 
                            ** ----------------------------------------------------- */
                            if ((mm_state == MM_IDLE) &&
                                ((mm_idle_substate == MM_NORMAL_SERVICE) ||
                                 (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE)||
                                 (mm_idle_substate == MM_LIMITED_SERVICE)||
                                 (mm_idle_substate == MM_LOCATION_UPDATE_NEEDED)))
                            {
                              if ((mm_location_information.location_update_status == UPDATED) &&
                                  (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                                  (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ))
                              {
                                mm_substate_control(MM_STD_NORMAL_SERVICE);
                                if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                                {
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                                   if(is_gsm_mode() && 
                                      (gmm_is_ptmsi_present_in_prev_gmm_msg || gmm_is_tmsi_present_in_prev_gmm_msg))
                                   {
                                     mm_pending_lu_type = MM_LU_CAUSE_IMSI_ATTACH;
                                   }
                                   else
#endif
                                   {
                                     mm_initiate_lu( MM_LU_CAUSE_IMSI_ATTACH );
                                   }
                                }
                                else
                                {
                     
#ifdef FEATURE_NAS_REL10
                                    if(mm_is_per_ms_t3212_present)
                                    {
                                        if ((mm_stored_per_ms_t3212_value != 0 ) &&
                                            (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                                        {
                                          mm_start_timer( TIMER_T3212,
                                                          mm_stored_per_ms_t3212_value);
                                        }
                                    }
                                    else
#endif
                                  if ((mm_system_information.T3212_timer_value != 0) &&
                                       (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                                  {
                                    mm_start_timer( TIMER_T3212,
                                                mm_system_information.T3212_timer_value );
                                  }                               
                                }
                              }
                              else
                              {
                                mm_substate_control(MM_STD_UPDATE_ATTEMPTED);
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                                if (is_gsm_mode() && 
                                   (gmm_is_ptmsi_present_in_prev_gmm_msg || gmm_is_tmsi_present_in_prev_gmm_msg))
                                {
                                  mm_pending_lu_type = MM_LU_CAUSE_NORMAL;
                                }
                                else
#endif
                                {
                                  mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                                }
                              }
                            }
                            else
                            {
                              if ((mm_location_information.location_update_status == UPDATED) &&
                                  (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                                  (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ))
                              {                            
                                if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                                {
                                  mm_pending_lu_request = IMSI_ATTACH; 
                                }
                              }
                              else
                              {
                                mm_pending_lu_request = NORMAL_LU;                               
                              }
                            }
                          }

                          if ((mm_location_information.location_update_status == UPDATED) &&
                              (memcmp( mm_location_information.lai,
                                       mm_serving_lai, LAI_SIZE ) == 0))
                          {
                            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                             SYS_SRV_DOMAIN_CS_PS,
                                                             FALSE,
                                                             SYS_SRV_STATUS_SRV,
                                                             TRUE );
                          }
                          else
                          {
                            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                             SYS_SRV_DOMAIN_PS_ONLY,
                                                             FALSE,
                                                             SYS_SRV_STATUS_SRV,
                                                             TRUE );
                          }

#ifdef FEATURE_GSTK          
                          mm_send_gstk_mm_idle_info();
#endif
#ifndef FEATURE_NAS_REL11
                          break;

                        default:
                          /* -------------------------------------------------------
                          ** Other values are considered as abnormal cases.  The
                          ** combined attach procedure shall be considered as failed
                          ** for GPRS and non-GPRS procedures.  The behavior of the
                          ** MS in those cases is specified in clause 4.7.3.2.5.
                          ** ------------------------------------------------------- */
                          gmm_increment_gprs_attach_attempt_counter();
#endif
                          if ((attach_accept_msg->gmm_cause >=
                               RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MIN_VALUE) &&
                            (attach_accept_msg->gmm_cause <=
                               RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MAX_VALUE))
                          {
                            gprs_attach_upon_entry_to_a_new_cell = TRUE;
                          }
                          
                          break;
                      } /* end switch (attach_accept_msg.gmm_cause) */
                    } /* end GMM Cause Present */
                    else
                    {
/*24.008: The behaviour of the MS in those cases is specified in subclause 4.7.3.2.5.*/
#ifdef FEATURE_NAS_REL11
                          /* --------------------------------------------------------
                          ** The MS shall change to state GMM-REGISTERED.ATTEMPTING-
                          ** TO-UPDATE-MM.  The routing area updating attempt counter
                          ** shall be incremented.  The MS shall start timer T3311.
                          ** When timer T3311 expires the combined routing area
                          ** update procedure indicating "combined RA/LA updating
                          ** with IMSI attach" is triggered.
                          ** -------------------------------------------------------- */
                          gmm_set_state( GMM_REGISTERED );
                          gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE_MM );
                          gmm_previous_T3311_gmm_substate = gmm_substate; 
                          
                          gprs_attach_attempt_counter = 0;
                          MSG_HIGH_DS_1(MM_SUB, "=MM= GPRS Attach attempt counter = %d", gprs_attach_attempt_counter );                          
                          gmm_rau_attempt_counter++;
                          MSG_HIGH_DS_1(MM_SUB, "=MM= RAU Attempt counter = %d", gmm_rau_attempt_counter);

                          mm_send_mmr_reg_reject_ind(
                                     SYS_SRV_DOMAIN_CS_ONLY,
                                     attach_accept_msg->gmm_cause,
                                     mm_serving_plmn.info.plmn,
                                     mm_serving_plmn.info.lac
#ifdef FEATURE_FEMTO_CSG
                                    ,mm_serving_plmn.info.csg_info.csg_id
#endif
                                    ,mm_serving_plmn.info.active_rat
                              );

                          if ((gmm_rau_attempt_counter < 5) &&
                              (!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                                   gmm_stored_rai.routing_area_code,
                                                   mm_serving_plmn.lac,
                                                   mm_serving_plmn.rac )) ||
                                 (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
                              (gmm_update_status == GMM_GU1_UPDATED))
                          {
                            /* ---------------------------------------------------------
                            ** The MS shall keep the GMM update status to GU1 UPDATED.
                            ** The MS shall start timer T3311.  When timer T3311 expires
                            ** the combined routing area updating procedure indicating
                            ** "combined RA/LA updating with IMSI attach" is triggered.
                            ** --------------------------------------------------------- */
                            mm_start_timer( TIMER_T3311, DEFAULT_TIMEOUT );
                            gmm_ra_update_type =
                              GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
                          }
                          else if (gmm_rau_attempt_counter >= 5)
                          {
                            mm_start_timer( TIMER_T3302, gmm_t3302_timer_value );
                            gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;

                            /* -----------------------------------------------------
                            ** A GPRS MS operating in MS operation mode A shall then
                            ** proceed with appropriate MM specific procedure. IT can be either IMSI 
                            ** Attach or nothing. 
                            ** ----------------------------------------------------- */
                            if ((mm_state == MM_IDLE) &&
                                ((mm_idle_substate == MM_NORMAL_SERVICE) ||
                                 (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE)||
                                 (mm_idle_substate == MM_LIMITED_SERVICE)||
                                 (mm_idle_substate == MM_LOCATION_UPDATE_NEEDED)))
                            {
                              if ((mm_location_information.location_update_status == UPDATED) &&
                                  (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                                  (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ))
                              {
                                mm_substate_control(MM_STD_NORMAL_SERVICE);
                                if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                                {
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                                   if(is_gsm_mode() && 
                                      (gmm_is_ptmsi_present_in_prev_gmm_msg || gmm_is_tmsi_present_in_prev_gmm_msg))
                                   {
                                     mm_pending_lu_type = MM_LU_CAUSE_IMSI_ATTACH;
                                   }
                                   else
#endif
                                   {
                                     mm_initiate_lu( MM_LU_CAUSE_IMSI_ATTACH );
                                   }
                                }
                                else
                                {
#ifdef FEATURE_NAS_REL10
                                    if(mm_is_per_ms_t3212_present)
                                    {
                                        if ((mm_stored_per_ms_t3212_value != 0 ) &&
                                            (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                                        {
                                          mm_start_timer( TIMER_T3212,
                                                          mm_stored_per_ms_t3212_value);
                                        }
                                    }
                                    else
#endif
                                  if ((mm_system_information.T3212_timer_value != 0) &&
                                       (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                                  {
                                    mm_start_timer( TIMER_T3212,
                                                mm_system_information.T3212_timer_value );
                                  }                               
                                }
                              }
                              else
                              {
                                mm_substate_control(MM_STD_UPDATE_ATTEMPTED);
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                                if (is_gsm_mode() && 
                                   (gmm_is_ptmsi_present_in_prev_gmm_msg || gmm_is_tmsi_present_in_prev_gmm_msg))
                                {
                                  mm_pending_lu_type = MM_LU_CAUSE_NORMAL;
                                }
                                else
#endif
                                {
                                  mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                                }
                              }
                            }
                            else
                            {
                              if ((mm_location_information.location_update_status == UPDATED) &&
                                  (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                                  (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ))
                              {                            
                                if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                                {
                                  mm_pending_lu_request = IMSI_ATTACH; 
                                }
                              }
                              else
                              {
                                mm_pending_lu_request = NORMAL_LU;                               
                              }
                            }
                          }

                          if ((mm_location_information.location_update_status == UPDATED) &&
                              (memcmp( mm_location_information.lai,
                                       mm_serving_lai, LAI_SIZE ) == 0))
                          {
                            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                             SYS_SRV_DOMAIN_CS_PS,
                                                             FALSE,
                                                             SYS_SRV_STATUS_SRV,
                                                             TRUE );
                          }
                          else
                          {
                            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                             SYS_SRV_DOMAIN_PS_ONLY,
                                                             FALSE,
                                                             SYS_SRV_STATUS_SRV,
                                                             TRUE );
                          }

#ifdef FEATURE_GSTK          
                          mm_send_gstk_mm_idle_info();
#endif
#else
                       /* -------------------------------------------------------
                       ** Other reject cause values and the case that no GMM cause IE was
                       ** received are considered as abnormal cases. The combined attach
                       ** procedure shall be considered as failed for GPRS and non-GPRS
                       ** services. The behaviour of the MS in those cases is specified
                       ** in subclause 4.7.3.2.5.
                       ** ------------------------------------------------------- */
                       gmm_increment_gprs_attach_attempt_counter();
#endif
                    }
                  }
                }
                else
                {
                  /* routing area updating attempt counter and attach attempt counter are reset when GPRS Attach 
                  procedure is successfully completed  */ 
                  gprs_attach_attempt_counter = 0;
                  MSG_HIGH_DS_1(MM_SUB, "=MM= GPRS Attach attempt counter = %d", gprs_attach_attempt_counter );                  
                  gmm_rau_attempt_counter     = 0;
                  MSG_HIGH_DS_1(MM_SUB, "=MM= GPRS RAU attempt counter = %d", gmm_rau_attempt_counter );
 
                  gmm_set_substate( GMM_NORMAL_SERVICE );
                  gmm_set_state( GMM_REGISTERED );                  
                
                  mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_ACCEPT,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                
                  /* Reset Manual mode user selection flag for PS */
                  gmm_manual_mode_user_selection_procedure = FALSE;
#ifdef FEATURE_LTE
                  mm_set_emm_manual_selection_flag(FALSE);
#endif                
#ifdef FEATURE_ENHANCED_NW_SELECTION
                  gmm_invalidate_foreground_search_rai((byte *)(&gmm_current_lai),gmm_current_rac);
#endif
                  if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                      (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                      (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY ||
                       mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
                  {
                    gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     SYS_SRV_DOMAIN_CS_PS,
                                                     FALSE,
                                                     SYS_SRV_STATUS_SRV,
                                                     TRUE );
                  }
                  else
                  {
                    gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     SYS_SRV_DOMAIN_PS_ONLY,
                                                     FALSE,
                                                     SYS_SRV_STATUS_SRV,
                                                     TRUE );
                  }

                }

                if (gmm_state != GMM_DEREGISTERED)
                {
                  if ((gmm_substate != GMM_NORMAL_SERVICE) &&
                      (gmm_substate != GMM_ATTEMPTING_TO_UPDATE_MM))
                  {
                    mm_ready_to_send_reg_cnf = TRUE;
                    gmm_set_substate( GMM_NORMAL_SERVICE );
                  }
                  else
                  {
                    if (mm_reg_waiting_for_reg_cnf)
                    {
                      mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
					  mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                      mm_send_mmr_reg_cnf();

                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;
                    }
                    else
                    {
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                      mm_send_mmr_service_ind( &mmr_service_ind );
                    }
                  }
                }
                if(attach_accept_msg->emergency_number_list_present)
                {
                  mm_update_mmr_emergency_num_list_if_needed(&(attach_accept_msg->emergency_number_list));                
                }
                else
                {
                   mm_nullify_mmr_emergency_num_list_if_needed
                   (
                       &(previous_plmn_identity[0]),
                       &(gmm_current_lai.PLMN_id.identity[0])
                   );
                }
#ifdef FEATURE_MBMS    
                #error code not present
#endif
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
                emm_ctrl_data_ptr->mmit_parameters.utran_ps_voice_parameters_ptr->is_last_utran_voims_supported =
                                                                                                              FALSE;
                if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9) &&
                   ((attach_accept_msg->nw_ftr_support & GMM_VOIMS_SUPPORTED_IN_UMTS)
                                                             == GMM_VOIMS_SUPPORTED_IN_UMTS))
                {
                  emm_ctrl_data_ptr->mmit_parameters.utran_ps_voice_parameters_ptr->is_last_utran_voims_supported =
                                                                                  TRUE;
                }
#endif
#ifdef FEATURE_GSM_GPRS
                if (is_gsm_mode())
                {
                  gmm_trigger_llc_type
                                   gmm_command = GMM_TRIGGER_FORCE_TO_STANDBY ;

                  if ( attach_accept_msg->ready_timer_value_present )
                  {
                    gmm_command |=
                      (attach_accept_msg->allocated_ptmsi_present
                                                   || allocated_tmsi_present) ?
                        GMM_TRIGGER_NEW_READY_TMR_VALUE_WO_UPDATE:
                          GMM_TRIGGER_NEW_READY_TMR_VALUE ;
                  }
                  else
                  {
                    gmm_command |=
                      (attach_accept_msg->allocated_ptmsi_present
                                                   || allocated_tmsi_present) ?
                        GMM_TRIGGER_NO_READY_TMR_VALUE_WO_UPDATE:
                          GMM_TRIGGER_NO_READY_TMR_VALUE ;
                  }

                  gmmllc_gmm_trigger_llc_event(gmm_command,
                        attach_accept_msg->force_to_standby,
                               gmm_convert_timer_value_ie_to_usecs(
                                        attach_accept_msg->ready_timer_value)) ;
#ifdef FEATURE_DUAL_DATA
                if (!attach_accept_msg->allocated_ptmsi_present &&  !allocated_tmsi_present)
                {
                  mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_LOW);
                }
#endif 

                }
#endif
                if (attach_accept_msg->allocated_ptmsi_present || allocated_tmsi_present)
                {
                  /* ----------------------------------------------
                  ** Send an ATTACH COMPLETE message to the network
                  ** ---------------------------------------------- */
                  gmm_send_attach_complete_message();

                }

#ifdef FEATURE_DUAL_SIM
                if(MM_MODEM_STATISTICS_SUB1)
#endif
                {
                  if ((attach_accept_msg->allocated_ptmsi_present )|| 
                    (allocated_tmsi_present) || 
                    (gmm_state != GMM_DEREGISTERED))
                {
                  mm_set_signal_for_cm(SYS_NAS_REGISTRATION_INFO_MASK);
                }
                }

#ifdef FEATURE_REL6_PS_FOP
                if (is_umts_mode() && (gmm_ps_fop_req)) 
                {
                  if (attach_accept_msg->follow_on_proceed_response == 0)
                  {
                    /* process the PS FoP if it is granted by nw */
                    sm_cmd = sm_get_cmd_buf();

                    if (sm_cmd == NULL)
                    {
                      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                    }

                    sm_cmd->header.message_set = MS_GMM_SM;
                    sm_cmd->header.cmd_id      = GMMSM_PS_FOP_IND;
#ifdef FEATURE_DUAL_SIM
                    sm_cmd->cmd.gmm_ps_fop_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                    MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_PS_FOP_GRANTED by NW" );
                    sm_put_cmd( sm_cmd );
                    gmm_ps_fop_resp = PS_FOP_NW_GRANTED;
                  }
                  else
                  {
                    gmm_ps_fop_resp = PS_FOP_NW_NOT_GRANTED;
                    /**************************************************************
                    *24.008 4.7.1.9 The MS shall start the timer T3340  when the  *
                    * network indicates "no follow-on proceed" in the             *
                    *ROUTING AREA UPDATE ACCEPT or ATTACH ACCEPT message.         *
                    **************************************************************/
                    mm_start_timer(TIMER_T3340, DEFAULT_TIMEOUT);
                  }                  
                }
                else if (is_umts_mode())
                {
                  if ( attach_accept_msg->follow_on_proceed_response == 1 )
                  {
                    gmm_ps_fop_resp = PS_FOP_NW_NOT_GRANTED; 
                    /**************************************************************
                    *24.008 4.7.1.9 The MS shall start the timer T3340  when the  *
                    * network indicates "no follow-on proceed" in the             *
                    *ROUTING AREA UPDATE ACCEPT or ATTACH ACCEPT message.         *
                    **************************************************************/
                    mm_start_timer(TIMER_T3340, DEFAULT_TIMEOUT);
                  }
#ifdef FEATURE_DUAL_SIM
                  else if ((cm_get_operator_info((sys_modem_as_id_e_type)mm_sub_id) == OPERATOR_CMCC) || (cm_get_operator_info((sys_modem_as_id_e_type)mm_sub_id) == OPERATOR_CU) || (mm_is_qrd_device) || (mm_optimised_ps_signalling_connection_release))
#else
                  else if ((cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CMCC) || (cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CU) || (mm_is_qrd_device) || (mm_optimised_ps_signalling_connection_release))
#endif
                  { 
                    if (!(gmm_ps_fop_req || gmm_follow_on_request ||
#ifdef FEATURE_DUAL_SIM
                       ( sm_is_pdp_activation_pending((sys_modem_as_id_e_type)mm_sub_id))
#else
                       ( sm_is_pdp_activation_pending()) 
#endif

                        ))
                    {
                    /* there is no follow-on request in GPRS ATTACH REQUEST */
                      mm_start_timer(TIMER_T3340, DEFAULT_TIMEOUT); 
                    }
                  }
                }
                else
#endif/* FEATURE_REL6_PS_FOP */
                if(is_gsm_mode())
                {
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_IND");
                  gmm_send_gmmsm_establish_ind(SM_PS_SIG_CONN_AVAIL);
                }
                if ((mm_held_cnm_service_req.present == TRUE) &&
                     (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)  &&
                     (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))    
                {
                  if (( is_gsm_mode() &&
                       (!attach_accept_msg->allocated_ptmsi_present )&& ( !allocated_tmsi_present))
#ifdef FEATURE_REL6_PS_FOP  
                          ||
                       (is_umts_mode() &&
                       (attach_accept_msg->follow_on_proceed_response == 0))
#endif
                     )
                  {
                    if ((( mm_location_information.location_update_status == UPDATED )&&
                           ( memcmp( mm_serving_lai,mm_location_information.lai, LAI_SIZE ) == 0))
#ifdef FEATURE_NO_EMCALL_UNTIL_COMBINED_REG_SUCCESS
                            ||
                          (mm_held_cnm_service_req.establishment_cause == (int)MM_RR_EMERGENCY_CALL)
#endif
                        )
                    {
                       mm_check_pending_requests();
                    }
                    else
                    {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_CS);
#else
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif
                    }
                  }
                  else
                  {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                    mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_CS);
#else
                    mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif
                  }
                }
                if(is_gsm_mode() && (gmm_multi_slot_ra_update_pending == TRUE))
                {
                  MSG_HIGH_DS_0(MM_SUB,"=MM= Processing the pending RAU request ");
                  gmm_update_ra_type();
                            gmm_initiate_routing_area_update();
                }                
                if((gmm_ra_update_pending == TRUE)&&(mm_state == MM_IDLE)&&
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                   (mm_check_if_cs_cell_access_allowed(mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION)))&&
                   (gmm_check_if_ps_cell_access_allowed(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION))))
                  {
#ifdef FEATURE_REL6_PS_FOP
                    if (is_umts_mode() && (attach_accept_msg->follow_on_proceed_response ==1))
                    {
                      MSG_HIGH_DS_0(MM_SUB,"=MM= combined RAU will be sent after connection release");
                    }
                    else
#endif
                    {
                      MSG_HIGH_DS_0(MM_SUB,"=MM= Processing the pending combined ATTACH request ");
                      gmm_update_ra_type();
                      gmm_initiate_routing_area_update();
                    }
                  }
              } /* end if (gmm_valid_attach_accept_message() */
              else
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Invalid ATTACH ACCEPT");
              }
#ifndef FEATURE_MODEM_HEAP
              gs_free(attach_accept_msg);
#else
              modem_mem_free(attach_accept_msg, MODEM_MEM_CLIENT_NAS);
#endif

              break;

            case GMM_ATTACH_REJECT:
              if (gmm_valid_attach_reject_message( message, &attach_reject_msg ))
              {
              #ifdef FEATURE_GSTK
              // Sending NW REJECT EVENT to GSTK 
              mm_send_gstk_gprs_attach_reject_evt(attach_reject_msg.gmm_cause);
              #endif /*FEATURE_GSTK*/

                mm_stop_timer( TIMER_T3310 );
                t3310_timeout_counter = 0;
                MSG_HIGH_DS_1( MM_SUB, "=MM= T3310 timeout counter = %d", t3310_timeout_counter );

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                if (is_gsm_mode())
                {
                  gmm_transaction_id++;

                  MSG_HIGH_DS_0(MM_SUB,"=MM= Transaction id Incremented Since ATTACH REJECT is received ");
                }
#endif

                mmsend_request_reject_event( FALSE, SMGMM_EVENT_ATTACH_REJECT,
                                             attach_reject_msg.gmm_cause);
#ifdef FEATURE_LTE
                /* If LTE is present, update EMM state and parameters.
                   Note: emm_multimode_complete_update must be also called
                   after GMM/MM processing to wrap up EMM update */
                if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
                {
                  emm_set_state_multimode_attach_failure((lte_nas_emm_cause_type)attach_reject_msg.gmm_cause);
                }
#endif
                gmm_reject_cause = attach_reject_msg.gmm_cause;//FRDIAG move up

#ifdef FEATURE_DUAL_SIM
                if(MM_MODEM_STATISTICS_SUB1)
#endif
                {
                gmm_reject_cause = attach_reject_msg.gmm_cause;
                mm_set_signal_for_cm(SYS_NAS_PS_REJECT_CAUSE_MASK);
                }
                mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_REJECTED,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                if (attach_reject_msg.t3302_value_present)
                {
                   /*Spec_ref:9.4.4.1   
                     In Iu mode, the MS shall ignore the contents of this IE if this message is received without integrity protection.
                     If this IE is not included or if in Iu mode the message is not integrity protected, the MS shall use the default value*/

                  if ((is_umts_mode ()) &&
                        (gmm_integrity_protection_activated == FALSE))
                  { 
                    gmm_t3302_timer_value = 720000;
                  }
                  else
                  {
                  /* ---------------------
                  ** Set T3302 timer value
                  ** --------------------- */
                    gmm_t3302_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                            attach_reject_msg.t3302_value );
                  }
#ifdef FEATURE_NAS_REL11
                  /*Store PLMN-ID for PLMN which provided T3302 value*/
                  gmm_previous_plmn_with_t3302_value.identity[0] = mm_serving_plmn.info.plmn.identity[0];
                  gmm_previous_plmn_with_t3302_value.identity[1] = mm_serving_plmn.info.plmn.identity[1];
                  gmm_previous_plmn_with_t3302_value.identity[2] = mm_serving_plmn.info.plmn.identity[2];
#endif
                }
                else
                {
                  gmm_t3302_timer_value = 720000;
                }

                /*----------------------------------------------------------
                ** The Equivalent PLMN List is deleted for all causes except
                ** #12, 14, 15 (24.008 section 4.7.3.1.4, 4.7.3.2.4)
                **----------------------------------------------------------*/
                if ((attach_reject_msg.gmm_cause != LA_NOT_ALLOWED) &&
                    (attach_reject_msg.gmm_cause != GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN) &&
                    (attach_reject_msg.gmm_cause != NO_SUITABLE_CELLS_IN_LA)
#ifdef FEATURE_NAS_REL10
                   && !((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10) &&
                       attach_reject_msg.gmm_cause == CONGESTTION)
#endif
#ifdef FEATURE_FEMTO_CSG
                    &&(attach_reject_msg.gmm_cause != CSG_NOT_AUTHORIZED)
#endif
                   )
                {
                   mm_serving_plmn.update_equiv_plmn_list = TRUE;
                   memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));
                }

#ifdef FEATURE_NAS_REL10
              if(attach_reject_msg.t3346_value_present && attach_reject_msg.gmm_cause == CONGESTTION)
              {
                gmm_t3346_backoff_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                                                          attach_reject_msg.t3346_value );
              }
#endif

                switch (attach_reject_msg.gmm_cause)
                {
                  case ILLEGAL_MS:
                  case ILLEGAL_ME:
                  case GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
                    /* ---------------------------------------------------
                    ** Delete P-TMSI, P-TMSI Signature, RAI, and GPRS CKSN
                    ** --------------------------------------------------- */
                    gmm_delete_ps_location_information();
                    gmm_delete_gprs_cksn();

                    gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;

                    mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                        gmm_stored_ptmsi_signature,
                                                        gmm_stored_rai,
                                                        gmm_update_status );

                    gmm_set_state( GMM_DEREGISTERED );
                    gmm_set_substate( GMM_LIMITED_SERVICE );

                    if (mm_managed_roaming_enabled)
                    {
                      mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
                    }
#ifdef FEATURE_ENHANCED_NW_SELECTION
                    if (reg_sim_read_ens_flag())
                    {
                      mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
                    }
#endif

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

                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                        (gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH))
                    {
                      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_PS,
                            attach_reject_msg.gmm_cause,
                            mm_serving_plmn.info.plmn,
                            mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                  ,mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                  ,mm_serving_plmn.info.active_rat
                           );
                    } 
                    else
                    {                     
                      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY, 
                                         attach_reject_msg.gmm_cause,
                                         mm_serving_plmn.info.plmn,
                                         mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                         ,mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                         ,mm_serving_plmn.info.active_rat
                           );
                    }   
                    
                    if (mm_reg_waiting_for_reg_cnf)
                    {
                       mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                       mm_send_mmr_reg_cnf();

                       mm_reg_waiting_for_reg_cnf = FALSE;
                       mm_ready_to_send_reg_cnf   = FALSE;
                    }
                    else
                    {
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                      mm_send_mmr_service_ind( &mmr_service_ind );
                    }
                    break;

                  case GPRS_SERVICES_NOT_ALLOWED:
                    /* ---------------------------------------------------
                    ** Delete P-TMSI, P-TMSI Signature, RAI, and GPRS CKSN
                    ** --------------------------------------------------- */
                    gmm_delete_ps_location_information();
                    gmm_delete_gprs_cksn();
                    mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                        gmm_stored_ptmsi_signature,
                                                        gmm_stored_rai,
                                                        gmm_update_status );

                    gmm_set_state( GMM_DEREGISTERED );
                    gmm_set_substate( GMM_LIMITED_SERVICE );

                    gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;

                    gmm_manual_mode_user_selection_procedure = FALSE;

                    mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                       attach_reject_msg.gmm_cause,
                                       mm_serving_plmn.info.plmn,
                                       mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                      , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                      ,mm_serving_plmn.info.active_rat
                                         );

                    /* ---------------------------------
                    ** Set SIM invalid for GPRS services
                    ** ---------------------------------*/
                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                        ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
                        (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)))
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

#ifdef FEATURE_LTE 
                    if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS))
                    {
                      if (is_umts_mode())
                      {
                        mm_send_rrc_lte_reselection_status_ind();
                      }
                      else if (is_gsm_mode())
                      {
                        mm_send_rr_lte_reselection_status_ind();
                      }
                    }
#endif

                    if(mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
                    {
                      mm_serving_plmn.ms_op_mode = SYS_SRV_DOMAIN_CS_ONLY;

                      if((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                         (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
                      {
                        mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain =
                          SYS_SRV_DOMAIN_CS_ONLY;
                      }

                      if ((mm_state != MM_IDLE)||
                          (((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
                             (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
                            (mm_lu_attempt_counter <4)))
                      {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN;
                      }
                      if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
                      {
                        /* -----------------------------------------------------
                        ** A GPRS MS operating in MS operation mode A shall then
                        ** perform an IMSI attach for non-GPRS services by use
                        ** of the MM IMSI attach procedure only if not already attached.
                        ** ----------------------------------------------------- */
                      
#ifdef FEATURE_GSM_GPRS
                        if (is_gsm_mode() && (mm_state == MM_IDLE))
                        {
                          mm_send_service_req( &mmcoord_pending_reg_message,
                                               SYS_RAT_GSM_RADIO_ACCESS );
                        }
                        else
#endif
                        {
                          if ((mm_location_information.location_update_status == UPDATED) &&
                              (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                              (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ))
                          {
                            mm_substate_control(MM_STD_NORMAL_SERVICE);
                            if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                            { 
#ifdef FEATURE_NAS_REL10
                              if(mm_state == MM_IDLE)
                              {
                                mm_initiate_lu(MM_LU_CAUSE_IMSI_ATTACH);
                              }
                              else
#endif
                              {
                              mm_pending_lu_request = IMSI_ATTACH;
                            }
                            }
                            else
                            {
#ifdef FEATURE_NAS_REL10
                              if(mm_is_per_ms_t3212_present)
                              {
                                if ((mm_stored_per_ms_t3212_value != 0 ) &&
                                     (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                                {
                                  mm_start_timer( TIMER_T3212,
                                                  mm_stored_per_ms_t3212_value);
                                }
                              }
                              else
#endif
                              if ((mm_system_information.T3212_timer_value != 0) &&
                                  (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                              {
                                mm_start_timer( TIMER_T3212,
                                                mm_system_information.T3212_timer_value );
                              }                               
                            }
                          }
                          else
                          {
#ifdef FEATURE_NAS_REL10
                            mm_substate_control(MM_STD_UPDATE_ATTEMPTED);
                            if(mm_state == MM_IDLE)
                            {
                              mm_initiate_lu(MM_LU_CAUSE_NORMAL);
                            }
                            else
#endif
                            {
                            mm_pending_lu_request = NORMAL_LU;
                          }

                        }
                        }

                        /* Set cause as REMAIN ON PLMN */
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                        if (mm_reg_waiting_for_reg_cnf)
                        {
                          mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                          mm_send_mmr_reg_cnf();

                          mm_reg_waiting_for_reg_cnf = FALSE;
                          mm_ready_to_send_reg_cnf   = FALSE;
                        }
                        else
                        {
                          mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                          mmr_service_ind.service_state = mm_serving_plmn.info;
                          mm_send_mmr_service_ind( &mmr_service_ind );
                        }
                      }
                      else
                      {
                        if (mm_reg_waiting_for_reg_cnf)
                        {
                          mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                          mm_send_mmr_reg_cnf();

                          mm_reg_waiting_for_reg_cnf = FALSE;
                          mm_ready_to_send_reg_cnf   = FALSE;
                        }
                        else
                        {
                          mmr_service_ind.service_state = mm_serving_plmn.info;
                          if ((mm_state != MM_IDLE)||
                              (((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
                                 (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
                                (mm_lu_attempt_counter <4))) 
                          {
                            mmr_service_ind.cause = MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN;
                          }
                          else
                          {
                            mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                          }
                          mm_send_mmr_service_ind( &mmr_service_ind );
                        }
                      }
                      if(mm_mmr_attach_ind_status != ATTACH_FAILED_IND_NOT_REQUIRED)
                      {
                        mm_send_reg_attach_failed_ind();
                      }
                    }
                    else
                    {
                      if (mm_reg_waiting_for_reg_cnf)
                      {
                        if (mm_ready_to_send_reg_cnf)
                        {
                          mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                          mm_send_mmr_reg_cnf();

                          mm_reg_waiting_for_reg_cnf = FALSE;
                          mm_ready_to_send_reg_cnf   = FALSE;
                        }
                      }
                      else
                      {
                        mmr_service_ind.service_state = mm_serving_plmn.info;
                        mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                        mm_send_mmr_service_ind( &mmr_service_ind );
                      }
                    }
                
                    break;

                  case PLMN_NOT_ALLOWED:
                    /* ----------------------------------------------------------
                    ** The MS shall delete any RAI, P-TMSI, P-TMSI signature, and
                    ** GPRS ciphering key sequence number stored, shall set the
                    ** GPRS update status to GU3 ROAMING NOT ALLOWED...
                    ** ---------------------------------------------------------- */
                    gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;

                    mm_add_plmn_to_fplmn_list( mm_serving_plmn.info.plmn);

                    gmm_delete_ps_location_information();
                    gmm_delete_gprs_cksn();

                    mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                        gmm_stored_ptmsi_signature,
                                                        gmm_stored_rai,
                                                        gmm_update_status );

                    gmm_manual_mode_user_selection_procedure = FALSE;

                    /***************************************************************************
                    **Ideally this flag should be reset in case of NMO1.But in case of NMO2,
                    **network may send PLMN_NOT_ALLOWED for attach and some other
                    **casue for LU.In that case we should not initiate LU also.So reset this falg 
                    **irrespective of NMO
                    ***************************************************************************/

                    mm_manual_mode_user_selection_procedure = FALSE;

#ifdef FEATURE_LTE
                    mm_set_emm_manual_selection_flag(FALSE);
#endif

                    /* ----------------------------------------------
                    ** ...shall reset the GPRS attach attempt counter
                    ** and shall change to state GMM-DEREGISTERED.
                    ** ---------------------------------------------- */
                    gprs_attach_attempt_counter = 0;
                    MSG_HIGH_DS_1(MM_SUB, "=MM= GPRS Attempt counter = %d", gprs_attach_attempt_counter );

                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                        (gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH))
                    {
                       /* shall reset the Rauting area attempt counter incase of
                       combined attach Ref 24.008 V3.19.0 sec 4.7.3.2.4 */

                       gmm_rau_attempt_counter = 0;
                       MSG_HIGH_DS_1( MM_SUB, "=MM= RAU Attempt counter = %d", gmm_rau_attempt_counter );
                       mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_PS,
                                                attach_reject_msg.gmm_cause,
                                                mm_serving_plmn.info.plmn,
                                                mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                                , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                                ,mm_serving_plmn.info.active_rat
                                               );

                    } 
                    else
                    {                     
                       mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                                   attach_reject_msg.gmm_cause,
                                                   mm_serving_plmn.info.plmn,
                                                   mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                                   , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                                  ,mm_serving_plmn.info.active_rat
                                      );
                    }                    

                    gmm_set_state( GMM_DEREGISTERED );
                    gmm_set_substate( GMM_LIMITED_SERVICE );
                    /**************************************************************
                    *24.008 4.7.1.9 The MS shall start the timer T3340  if the MS *
                    * receives any of the reject cause values #11,#12,#13 or #15  *
                    **************************************************************/
                    if (is_umts_mode( ))
                    {
                       mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);
                    }

                    /* -------------------------------------------------------------
                    ** The MS shall store the identity in the "forbidden PLMN list".
                    ** ------------------------------------------------------------- */
                    mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED;

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

                      if (mm_managed_roaming_enabled)
                      {
                        mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
                      }
#ifdef FEATURE_ENHANCED_NW_SELECTION
                      if (reg_sim_read_ens_flag())
                      {
                        mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
                      }
#endif

                      /* --------------------------------------------------
                      ** The MM state will transition back to IDLE when the
                      ** RR(C) connection is released
                      ** -------------------------------------------------- */
                    }                      

                    gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     SYS_SRV_DOMAIN_NO_SRV,
                                                     TRUE,
                                                     SYS_SRV_STATUS_LIMITED,
                                                     TRUE );
                    if (mm_reg_waiting_for_reg_cnf)
                    {
                      mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                      mm_send_mmr_reg_cnf();

                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;
                    }
                    else
                    {
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mmr_service_ind.cause = mmcoord_mmr_reg_cnf.cause;
                      mm_send_mmr_service_ind( &mmr_service_ind );
                    }
                    break;

                  case LA_NOT_ALLOWED:
                  case NATIONAL_ROAMING_NOT_ALLOWED:
                  case NO_SUITABLE_CELLS_IN_LA:
                    /* ---------------------------------------------------
                    ** Delete P-TMSI, P-TMSI Signature, RAI, and GPRS CKSN
                    ** --------------------------------------------------- */
                    gmm_delete_ps_location_information();
                    gmm_delete_gprs_cksn();

                    gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;

                    mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                        gmm_stored_ptmsi_signature,
                                                        gmm_stored_rai,
                                                        gmm_update_status );

                    /* -------------------------------------
                    ** Reset the GPRS attach attempt counter
                    ** ------------------------------------- */
                    gprs_attach_attempt_counter = 0;
                    MSG_HIGH_DS_1( MM_SUB, "=MM= GPRS Attach Attempt counter = %d",
                              gprs_attach_attempt_counter );

                    gmm_set_state( GMM_DEREGISTERED );
                    gmm_set_substate( GMM_LIMITED_SERVICE );
                    /**************************************************************
                    *24.008 4.7.1.9 The MS shall start the timer T3340  if the MS *
                    * receives any of the reject cause values #11,#12,#13 or #15  *
                    **************************************************************/
                    if (is_umts_mode( ))
                    {
                      mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);
                    }

                    if (attach_reject_msg.gmm_cause == LA_NOT_ALLOWED)
                    {
#ifdef FEATURE_FORBID_LAI_UPON_REGIONAL_ROAMING_REJECT
                      /* ---------------------------------------------------------
                      ** This FEATURE was introduced to support a customer request
                      ** for a non-spec compliant PLMN selection on receipt of
                      ** reject cause LA_NOT_ALLOWED
                      ** --------------------------------------------------------- */
                      mm_add_forbidden_national_roaming( &gmm_current_lai );
                      mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED;
#else
                      mm_add_forbidden_regional_roaming( &gmm_current_lai );
                      mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
#endif
                      gmm_manual_mode_user_selection_procedure = FALSE;

                      /*************************************************************
                      **Ideally this flag should be reet in case of NMO1.But in case
                      **of NMO2, network may send LA_NOT_ALLOWED for attach and some
                      **other casue for LU.In that case we should not initiate LU
                      **also.So reset this falg* irrespective of NMO
                      *************************************************************/
                      
                      mm_manual_mode_user_selection_procedure = FALSE;
                    }
                    else if (attach_reject_msg.gmm_cause == NATIONAL_ROAMING_NOT_ALLOWED)
                    {
                      mm_add_forbidden_national_roaming( &gmm_current_lai );
                      mmcoord_mmr_reg_cnf.cause =
                        MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION;
                      gmm_manual_mode_user_selection_procedure = FALSE;


                      /***************************************************************
                      **Ideally this flag should be reet in case of NMO1.But in case
                      **of NMO2, network may send NATIONAL_ROAMING_NOT_ALLOWED for
                      **attach and some other casue for LU.In that case we should not
                      **initiate LU also.So reset this falg* irrespective of NMO
                      ***************************************************************/

                      mm_manual_mode_user_selection_procedure = FALSE;

#ifdef FEATURE_LTE
                      mm_set_emm_manual_selection_flag(FALSE);
#endif
                    }
                    else if (attach_reject_msg.gmm_cause == NO_SUITABLE_CELLS_IN_LA)
                    {
                      mm_add_forbidden_national_roaming( &gmm_current_lai );
                      mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED;
                      /* add LAI in LAI reject list */ 
                      mm_add_lai_in_reject_list(&gmm_current_lai);
                    }
                    if(attach_reject_msg.gmm_cause == LA_NOT_ALLOWED)
                    {
                      if(gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH)
                      {
                        mm_substate_control( MM_STD_LIMITED_SERVICE );
                      }
                    }
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
                      
                      /* Set cs service status to limited if rej. cause is #11, #12, or #13 only in manual mode */
                     if(mm_managed_roaming_enabled && 
                        (attach_reject_msg.gmm_cause == LA_NOT_ALLOWED ||
                         attach_reject_msg.gmm_cause == NATIONAL_ROAMING_NOT_ALLOWED) && 
                         (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                          mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
                     {
                        mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
                     }
#ifdef FEATURE_ENHANCED_NW_SELECTION
                      /* Set cs service status to limited if rej. cause is #11, #12, or #13 only in manual mode */
                     if(reg_sim_read_ens_flag() && 
                        (attach_reject_msg.gmm_cause == LA_NOT_ALLOWED ||
                         attach_reject_msg.gmm_cause == NATIONAL_ROAMING_NOT_ALLOWED) && 
                         (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                          mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
                     {
                        mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
                     }
#endif
  
                      /* --------------------------------------------------
                      ** The MM state will transition back to IDLE when the
                      ** RR(C) connection is released
                      ** -------------------------------------------------- */
                    }  

                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                        (gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH))
                    {
                      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_PS,
                                           attach_reject_msg.gmm_cause,
                                           mm_serving_plmn.info.plmn,
                                           mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                           , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                          , mm_serving_plmn.info.active_rat
                                          );
                    } 
                    else
                    {                     
                      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                      attach_reject_msg.gmm_cause,
                                      mm_serving_plmn.info.plmn,
                                      mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                      , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                      , mm_serving_plmn.info.active_rat
                                                    );
                    }

                   
                    gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     SYS_SRV_DOMAIN_NO_SRV,
                                                     TRUE,
                                                     SYS_SRV_STATUS_LIMITED,
                                                     TRUE );
                    if (mm_reg_waiting_for_reg_cnf)
                    {
                      mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                      mm_send_mmr_reg_cnf();

                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;
                    }
                    else
                    {
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mmr_service_ind.cause = mmcoord_mmr_reg_cnf.cause;
                      mm_send_mmr_service_ind( &mmr_service_ind );
                    }
                    break;

                  case GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN:
                    /* ----------------------------------------------------------
                    ** The MS shall delete any RAI, P-TMSI, P-TMSI signature, and
                    ** GPRS ciphering key sequence number stored, shall set the
                    ** GPRS update status to GU3 ROAMING NOT ALLOWED...
                    ** ---------------------------------------------------------- */
                    gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
                    if (reg_sim_per_subs_is_ehplmn(mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
                    if (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), mm_serving_plmn.info.plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
                    if (reg_sim_is_ehplmn(mm_serving_plmn.info.plmn) )
#else
                    if (sys_plmn_match(reg_sim_read_hplmn(), mm_serving_plmn.info.plmn) )
#endif
#endif
                    {
                      gmm_update_gprs_forbidden_rai(gmm_previous_registration_serving_rai);
                    }
                    else
                    {
                      reg_sim_add_plmn_to_gprs_fplmn_list( mm_serving_plmn.info.plmn
#ifdef FEATURE_DUAL_SIM
                                                         ,(sys_modem_as_id_e_type)mm_sub_id
#endif
                                                       );
                    }                    
#ifdef FEATURE_LTE 
                    if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS))
                    {
                      if (is_umts_mode())
                      {
                        mm_send_rrc_lte_reselection_status_ind();
                      }
                      else if (is_gsm_mode())
                      {
                        mm_send_rr_lte_reselection_status_ind();
                      }
                    }
#endif 
                    
                    gmm_delete_ps_location_information();
                    gmm_delete_gprs_cksn();

                    mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                        gmm_stored_ptmsi_signature,
                                                        gmm_stored_rai,
                                                        gmm_update_status );

                    gmm_manual_mode_user_selection_procedure = FALSE;

                    /* ---------------------------------------------------------------
                    ** ...shall reset the GPRS attach attempt counter and shall change
                    ** to state GMM-DEREGISTERED.  A GPRS MS operating in MS operation
                    ** mode A or B in network operation mode II or III is still IMSI
                    ** attached for CS services in the network.
                    ** --------------------------------------------------------------- */
                    gprs_attach_attempt_counter = 0;
                    MSG_HIGH_DS_1( MM_SUB, "=MM= GPRS Attach Attempt counter = %d",
                               gprs_attach_attempt_counter );

                    gmm_set_state( GMM_DEREGISTERED );
                    gmm_set_substate( GMM_LIMITED_SERVICE );

                    mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                     attach_reject_msg.gmm_cause,
                                      mm_serving_plmn.info.plmn,
                                      mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                     , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                     , mm_serving_plmn.info.active_rat
                             );
                    
                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
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

                    /* -------------------------------------------------------
                    ** The MS shall store the identity in the "forbidden PLMNs
                    ** for GPRS service" list.
                    ** ------------------------------------------------------- */
                    mmcoord_mmr_reg_cnf.cause =
                      MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES;

                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
                    {
                      /*------------------------------------------------------
                      **If the MS does not perform a PLMN selection then a GPRS MS operating in MS operation mode A or B which
                      **is not yet IMSI attached for CS services in the network shall then perform an IMSI attach for non-GPRS services
                      **according to the conditions for the MM IMSI attach procedure (see 4.4.3).
                      **A GPRS MS operating in MS operation mode A or B which is already IMSI attached for CS services in the
                      **network is still IMSI attached for CS services in the network and shall then proceed with the appropriate MM
                      **specific procedure according to the MM service state. Ref :TS 24.008 4.7.5.2.4 V10.7.0 
                      **----------------------------------------------------*/
                      if (!(( mm_location_information.location_update_status == UPDATED ) &&
                            ( memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)))
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

                        mm_substate_control( MM_STD_NORMAL_SERVICE );

                        if ((mm_possible_imsi_attach) && (mm_system_information.ATT))
                        { 
                          if (mm_state == MM_IDLE)
                          {
                            mm_initiate_lu( MM_LU_CAUSE_IMSI_ATTACH );
                          }
                          else
                          {
                            mm_pending_lu_request = IMSI_ATTACH;
                          }
                        }
#ifdef FEATURE_NAS_REL10
                        else if(mm_is_per_ms_t3212_present)
                        {
                           if ((mm_stored_per_ms_t3212_value != 0 ) &&
                               (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                           {
                             mm_start_timer( TIMER_T3212,
                             mm_stored_per_ms_t3212_value);
                           }
                        }
#endif
                        else if ((mm_system_information.T3212_timer_value != 0 ) &&
                                 (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                        {
                          mm_start_timer( TIMER_T3212,
                                          mm_system_information.T3212_timer_value );
                        }
                      }                    
                    }
                    /*
                    ** Set the cause as GPRS_FORBIDDEN_REMAIN_ON_PLMN
                    ** if MM procedure is triggered (MM is not in IDLE sate).
                    */
                    if ((mm_state != MM_IDLE)||
                        (((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
                          (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
                         (mm_lu_attempt_counter <4))
#ifdef FEATURE_NAS_REL10
                           ||
                        (mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
#endif
                       )
                    {
                      mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN;
                    }
                    /*
                    ** Send REG_CNF/SERVICE_IND to add PLMN in GPRS FPLMN List.
                    */
                    if (mm_reg_waiting_for_reg_cnf)
                    {
                      mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                      mm_send_mmr_reg_cnf();

                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;
                    }
                    else
                    {
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mmr_service_ind.cause = mmcoord_mmr_reg_cnf.cause;
                      mm_send_mmr_service_ind( &mmr_service_ind );
                    }
                    break;

#ifdef FEATURE_NAS_REL10
                case CONGESTTION: 

                     if(gmm_t3346_backoff_timer_value == 0)
                     {

                       gmm_increment_gprs_attach_attempt_counter();
                       mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                                  attach_reject_msg.gmm_cause,
                                                  mm_serving_plmn.info.plmn,
                                                  mm_serving_plmn.info.lac
  #ifdef FEATURE_FEMTO_CSG
                                                  , mm_serving_plmn.info.csg_info.csg_id
  #endif 
                                                  , mm_serving_plmn.info.active_rat
                                                 );
                     }

                     else
                     {
                           /**-----------------------------------------------------------------------------
                        **Cause #22-T3346 Started- The MS shall abort the attach procedure, reset the attach attempt counter, 
                        **set the GPRS update status to GU2 NOT UPDATED and enter state GMM-DEREGISTERED.ATTEMPTING-TO-ATTACH
                        **(24.008 section 4.7.3.1.4) ------------------------------------------------*/


                        gprs_attach_attempt_counter = 0;
                        MSG_HIGH_DS_1( MM_SUB, "=MM= GPRS Attach Attempt counter = %d",
                                                                                    gprs_attach_attempt_counter );
                        gmm_update_status = GMM_GU2_NOT_UPDATED;

                        mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                            gmm_stored_ptmsi_signature,
                                                            gmm_stored_rai,
                                                            gmm_update_status );

                        mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                                   attach_reject_msg.gmm_cause,
                                                   mm_serving_plmn.info.plmn,
                                                   mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                                  , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                                  , mm_serving_plmn.info.active_rat
                                                  );

                        gmm_set_state( GMM_DEREGISTERED );
                        gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH);

                        if (mm_managed_roaming_enabled)
                        {
                          mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
                        }
#ifdef FEATURE_ENHANCED_NW_SELECTION
                        if (reg_sim_read_ens_flag())
                        {
                          mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
                        }
#endif
                        if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                           (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                           (mm_location_information.location_update_status == UPDATED) &&
                           (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0) &&
                           ((mm_serving_plmn.info.reg_service_domain  == SYS_SRV_DOMAIN_CS_PS) ||
                            (mm_serving_plmn.info.reg_service_domain  == SYS_SRV_DOMAIN_CS_ONLY)))
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

                            /*****************************************************************************************
                        **If the ATTACH REJECT message is integrity protected, the MS shall start timer T3346 with the 
                        **value provided in the T3346 value IE else start the timer T3346 with a random value in the default range of 15-30 mins. 
                        **(24.008 section 4.7.3.1.4) *******************************************/

                        if (gmm_integrity_protection_activated == TRUE)
                        {
                          mm_start_timer(TIMER_T3346,gmm_t3346_backoff_timer_value);
                        }
                        else
                        {
                          gmm_t3346_backoff_timer_value = ((15 + mm_get_random_value(15)) * 60 * 1000);
                          mm_start_timer(TIMER_T3346,gmm_t3346_backoff_timer_value);
                        }

#ifdef FEATURE_NAS_REL11
                        if(mm_is_rrc_access_class_11_to_15() == TRUE)
                        {
                          MSG_HIGH_DS_0(MM_SUB ,"=MM= Retrying ATTACH in NW Congestion for High AC users");
                          gmm_initiate_gprs_attach();
                        }
#endif
                        /*saves congestion plmn associated with the timer*/
                        memscpy((void *)&gmm_3gpp_t3346_plmn_id,sizeof(sys_plmn_id_s_type),(void *)&(mm_serving_plmn.info.plmn), sizeof(sys_plmn_id_s_type));
                        gmm_t3346_backoff_timer_value = 0;

                         /* For High AC users if ATTACH was re-initiated above, then 
                          * reject the pending call with cause such that re-dial is allowed.
                          * In that case, don't send current Service status to REG, instead
                          * wait for Registration to conclude.
                          */
                        if(gmm_state == GMM_REGISTERED_INITIATED)
                        {
                          if (mm_held_cnm_service_req.present)
                          {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                            mm_send_mmcnm_est_rej(
                                   AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL ,MMCC_SMS_CS);
#else
                            mm_send_mmcnm_est_rej(
                                   AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
                           }
                        }
                        else
                        {
                           mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
   
                           if (mm_reg_waiting_for_reg_cnf)
                           {
                             mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                             mm_send_mmr_reg_cnf();
   
                             mm_reg_waiting_for_reg_cnf = FALSE;
                             mm_ready_to_send_reg_cnf   = FALSE;
                           }
                           else
                           {
                             mmr_service_ind.service_state = mm_serving_plmn.info;
                             mmr_service_ind.cause = mmcoord_mmr_reg_cnf.cause;
                             mm_send_mmr_service_ind( &mmr_service_ind );
                           }
                        }
                     }
                    break;
#endif
#ifdef FEATURE_FEMTO_CSG
                  case CSG_NOT_AUTHORIZED:
                    if(mm_serving_csg_id != SYS_CSG_ID_INVALID)
                    {
                      gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
  
                      mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                          gmm_stored_ptmsi_signature,
                                                          gmm_stored_rai,
                                                          gmm_update_status );
  
                      /* -------------------------------------
                      ** Reset the GPRS attach attempt counter
                      ** ------------------------------------- */
                      gprs_attach_attempt_counter = 0;
                      MSG_HIGH_DS_1( MM_SUB, "=MM= GPRS Attach Attempt counter = %d",
                                gprs_attach_attempt_counter );
  
                      gmm_set_state( GMM_DEREGISTERED );
                      gmm_set_substate( GMM_LIMITED_SERVICE );
                      /**************************************************************
                      *24.008 4.7.1.9 The MS shall start the timer T3340  if the MS *
                      * receives any of the reject cause values #25  *
                      **************************************************************/
                      if (is_umts_mode( ))
                      {
                        mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);
                      }
  
  
                      if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                          (gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH))
                      {
                        mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_PS,
                                      attach_reject_msg.gmm_cause,
                                      mm_serving_plmn.info.plmn,
                                      mm_serving_plmn.info.lac
                                      , mm_serving_plmn.info.csg_info.csg_id,
                                      mm_serving_plmn.info.active_rat
                                 );
                      } 
                      else
                      {                     
                        mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                              attach_reject_msg.gmm_cause,
                                              mm_serving_plmn.info.plmn,
                                              mm_serving_plmn.info.lac,
                                              mm_serving_plmn.info.csg_info.csg_id,
                                              mm_serving_plmn.info.active_rat
                                                  );
                      }
  
                      mm_delete_csg_from_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info);

                      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                       SYS_SRV_DOMAIN_NO_SRV,
                                                       TRUE,
                                                       SYS_SRV_STATUS_LIMITED,
                                                       TRUE );
  
                      mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED;
  
                      if (mm_reg_waiting_for_reg_cnf)
                      {
                        mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                        mm_send_mmr_reg_cnf();
  
                        mm_reg_waiting_for_reg_cnf = FALSE;
                        mm_ready_to_send_reg_cnf   = FALSE;
                      }
                      else
                      {
                        mmr_service_ind.service_state = mm_serving_plmn.info;
                        mmr_service_ind.cause = mmcoord_mmr_reg_cnf.cause;
                        mm_send_mmr_service_ind( &mmr_service_ind );
                      }
                    }
                    else
                    {
                      gmm_increment_gprs_attach_attempt_counter();
                      if ((attach_reject_msg.gmm_cause >=
                             RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MIN_VALUE) &&
                          (attach_reject_msg.gmm_cause <=
                             RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MAX_VALUE))
                      {
                        gprs_attach_upon_entry_to_a_new_cell = TRUE;
                      }
                    }
                    break;
#endif 

                  default:
                    /* ---------------------------------------------
                    ** Abnormal case - handle as in 24.008 4.7.3.1.5
                    ** --------------------------------------------- */

                    /* If rejected with cause #17, add to low priority PLMN list */
                    if(mm_managed_roaming_enabled && (attach_reject_msg.gmm_cause == NETWORK_FAILURE))
                    {
                      reg_sim_add_to_low_priority_plmn_list(mm_serving_plmn.info.plmn, mm_serving_plmn.info.active_rat
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                                            ,(sys_modem_as_id_e_type)mm_as_id
#endif
                                                           );
                    }

                      if(attach_reject_msg.gmm_cause == MM_SEMANTICALLY_INCORRECT_MESSAGE ||
                         attach_reject_msg.gmm_cause == MM_INVALID_MANDATORY_INFORMATION ||
                         attach_reject_msg.gmm_cause == MM_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED ||
                         attach_reject_msg.gmm_cause == MM_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED ||
                         attach_reject_msg.gmm_cause == MM_PROTOCOL_ERROR_UNSPECIFIED)
                      {
                        /* Upon reception of Cause Codes #95 #96 #97 #99 #111, The MS should set the 
                           gprs_attach_attempt_counter to 5 
                           and wait for rel-ind from NW 24.008 rel6 Section 4.7.3.1.5 */           
                        gprs_attach_attempt_counter = (byte) (MAX_ATTEMPT_COUNTER_ATTACH - 1);
                      }
                       
                    gmm_increment_gprs_attach_attempt_counter();

                    if ((attach_reject_msg.gmm_cause == IMSI_UNKNOWN_IN_HLR) ||
                        (attach_reject_msg.gmm_cause == IMSI_UNKNOWN_IN_VLR) ||
                        (attach_reject_msg.gmm_cause == NETWORK_FAILURE) ||
                        (attach_reject_msg.gmm_cause == CONGESTTION) ||
                        (attach_reject_msg.gmm_cause == MSC_TEMPORARILY_NOT_REACHABLE))
                    {
                      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                         attach_reject_msg.gmm_cause,
                                         mm_serving_plmn.info.plmn,
                                         mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                         , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                         , mm_serving_plmn.info.active_rat
                                                   );
                    }

                    if ((attach_reject_msg.gmm_cause >=
                           RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MIN_VALUE) &&
                        (attach_reject_msg.gmm_cause <=
                           RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MAX_VALUE))
                    {
                      gprs_attach_upon_entry_to_a_new_cell = TRUE;
                    }
                    break;
                } /* end switch (attach_reject_msg.gmm_cause) */


#ifdef FEATURE_GSTK
                gmm_send_gstk_idle_event_if_needed(attach_reject_msg.gmm_cause, FALSE);
#endif

#if defined(FEATURE_LTE)
                if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
                {
                  emm_multimode_complete_update();
                }
#endif
              } /* end if (gmm_valid_attach_reject_message() */
              else
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Invalid ATTACH REJECT");
              }
              break;

            default:
              MSG_FATAL_DS( MM_SUB, "=MM= Unexpected L3 msg %d",
                          message->cmd.mm_data_ind.L3_data[1], 0,0 );
          } /* end switch (gmm_message_type) */
          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected RRC message %d", message->cmd.hdr.message_id, 0,0 );
      } /* end switch (message->cmd.hdr.message_id) */
      break;

    case MS_TIMER:
      switch (message->cmd.timer_expiry.timer_id)
      {
        case TIMER_T3310:
          t3310_timeout_counter++;
          MSG_HIGH_DS_1(MM_SUB, "=MM= T3310 timeout counter = %d", t3310_timeout_counter);

          /* one attach attempt done, so mark power on attach as false*/
#ifdef FEATURE_DUAL_SIM
          gmm_power_on_attach_completed = TRUE;
#endif
          /*Cancel any running authentication req*/
          
          if (t3310_timeout_counter < 5)
          {
            /* ---------------------------------------------------------------------
            ** On the first (through 4th) expiry of this timer, the MS shall restart
            ** timer T3310 and shall retransmit the ATTACH REQUEST message
            ** --------------------------------------------------------------------- */
            gmm_initiate_gprs_attach();
          }
          else /* t3310_timeout_counter == 5 */
          {
            /* ------------------------------------------------
            ** The MS shall release the PS signaling connection
            ** (TS 24.008 4.7.3.1.5)
            ** ------------------------------------------------ */
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
              MSG_HIGH_DS_0( MM_SUB, "=MM= Sending RRC_CLOSE_SESSION_REQ");

              send_cmd_to_rrc( rrc_cmd, mm_serving_plmn.info.active_rat );
            }
#endif /* #ifdef FEATURE_WCDMA */

            gmm_ps_signalling_connected = FALSE;

            if (pmm_mode == PMM_CONNECTED)
            {
              /* -------------------------------------------------------------------
              ** In UMTS the timer T3312 is reset and started with its initial value
              ** when the MS goes from PMM-CONNECTED to PMM-IDLE mode
              ** (TS 24.008 4.7.2.2)
              ** ------------------------------------------------------------------- */
              pmm_mode = PMM_IDLE;
              MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_IDLE" );
/*24.008:4.7.7.2 
The RAND and RES values stored in the mobile station shall be deleted and timer T3316, if running, shall be stopped:
-if the mobile station enters PMM-IDLE mode (Iu mode only).*/
#ifdef FEATURE_NAS_REL11
              if (is_umts_mode())
              {
                auth_reset_cache_memory(FALSE,NO_TIMER_EXPIRED,RRC_PS_DOMAIN_CN_ID) ;
              }
#endif

              sm_cmd = sm_get_cmd_buf();

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

              if (gmm_t3312_timer_value != 0)
              {
                mm_start_timer( TIMER_T3312, gmm_t3312_timer_value );
              }
              mm_stop_timer( TIMER_T3319 );
            }

            gmm_increment_gprs_attach_attempt_counter();
            mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_TIME_OUT,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
          }
          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected timer message %d",
                      message->cmd.timer_expiry.timer_id, 0,0 );

      } /* end switch (message->cmd.timer_expiry.timer_id) */
      break;

    default:
      MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message set %d", message->cmd.hdr.message_set, 0,0 );
  } /* end switch (message->cmd.hdr.message_set) */
} /* end gmm_handle_gprs_attach() */


/*===========================================================================

FUNCTION GMM_INITIATE_ROUTING_AREA_UPDATE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_initiate_routing_area_update( void )
{
  mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);
#ifdef FEATURE_DSAC
  mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);
#endif
  boolean routine_man_selection_failed = FALSE ;

#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

  if (!(((PLMN_MATCH( gmm_current_lai.PLMN_id.identity,
                     mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity )) ||
        (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
         sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))
     #ifdef FEATURE_FEMTO_CSG
        && (mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
     #endif
     #if defined (FEATURE_LTE) && defined (FEATURE_DUAL_DATA)
        && (mm_is_volte_registration_allowed(mm_serving_plmn.info.plmn, FALSE) == TRUE)
     #endif
        )  &&
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
              SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  {
    routine_man_selection_failed = TRUE;
    MSG_HIGH_DS_0(MM_SUB, "=MM= Different PLMN found during MAN_LIMITED_ACQ");
  }
  /* Check if GMM is in Limited Service */
  if ((gmm_substate == GMM_LIMITED_SERVICE) &&
      ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode
        != SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION) ||
      ((routine_man_selection_failed)||(!gmm_manual_mode_user_selection_procedure))))
  {
    /* If GMM is in REGISTERED and LIMITED SERVICE RAU not allowed */
    MSG_HIGH_DS_0(MM_SUB, "=MM= GMM substate Limited, RAU not allowed");
  }
  else if ((mmcoord_pending_reg_message.cmd.hdr.message_set == (int) MS_MM_REG) &&
            (mmcoord_pending_reg_message.cmd.hdr.message_id == (int) MMR_STOP_MODE_REQ)&&
            (mm_nreg_req_received))
  {
     MSG_HIGH_DS_0(MM_SUB, "=MM= Power off, RAU trigger ignored");
     mm_stop_timer(TIMER_T3330);
  }
  /* Check blocking restrictions */
  else if ((gmm_substate == GMM_SUSPENDED) || (is_gsm_mode() && gmm_is_suspended))
  {
    MSG_HIGH_DS_0( MM_SUB, "=MM= RA update blocked - GMM_SUSPENDED" );
    gmm_ra_update_pending = TRUE;
    gmm_pending_ra_update_type = gmm_ra_update_type;
    if (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH ||
        gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)
    {
       mm_pending_lu_request = NO_LU;
       MSG_HIGH_DS_0( MM_SUB, "=MM= Pending Normal LU flag cleared here as Combined proc is pended");
    }    
  }
  else if ((gmm_check_if_ps_cell_access_barred(ps_cell_access) == TRUE) ||
           (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE))
  {
    MSG_HIGH_DS_2( MM_SUB, "=MM= RA update blocked - AC %d or service_state %d",
               ps_cell_access, mm_serving_plmn.gmm_service_state);

    gmm_set_state( GMM_REGISTERED );

    if (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
    {
      gmm_set_substate( GMM_NO_CELL_AVAILABLE );
    }
    else
    {
      gmm_set_substate( GMM_UPDATE_NEEDED );
    }

#ifdef FEATURE_DSAC
    /***************************************************************
     * 4.1.1.2.1    GPRS MS operating in mode A or B in a network 
     *       that operates in mode I
     * A GPRS MS operating in mode A or B in a network that operates 
     * in mode I shall perform a normal location updating procedure 
     * (in order to remove the Gs association in the MSC/VLR) when 
     * the following conditions are fulfilled:
     *     -    the GPRS MS has camped on a cell where the PS domain 
                  is barred and the CS domain is unbarred; and
     *     -    T3312 expires.
     ***************************************************************/

    if ((gmm_ra_update_type == GMM_PERIODIC_UPDATING) &&
        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
    {
      if (mm_state == MM_IDLE)
      {
        mm_initiate_lu(MM_LU_CAUSE_NORMAL);
      }
      else
      {
        mm_pending_lu_request = NORMAL_LU;
      }
    }
#endif  
  }
  else if
  (
    /***********************************************************************************
    ** If DTM is not supported then we must wait for MM Idle before initiating RAU.
    **
    ** If DTM is supported, then we must check to see if MM is establishing an RR connection
    ** for LU, as this will be indicative that a GPRS Suspension message from RR will be queued
    ** and waiting to be processed by GMM. In this case RAU must be held pending GPRS Resumption
    ** on completion of RR connection establishment.
    *************************************************************************************/
#ifdef FEATURE_GSM_DTM
    (is_gsm_mode())
    &&
    (
      ((mm_serving_plmn.dtm_supported == FALSE) && (mm_state != MM_IDLE)) ||
      ((mm_serving_plmn.dtm_supported == TRUE ) && ((mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) ||
                                                     (((mm_state == MM_WAIT_FOR_RR_CONNECTION_MM) || (mm_state == MM_WAIT_FOR_NETWORK_COMMAND)) && (rrc_connection_status == MMRRC_EST_IN_PROGRESS))
                                                    )
      )
    )     
#else
    (mm_state != MM_IDLE) && (is_gsm_mode())
#endif /* FEATURE_GSM_DTM */
  )
  {

#ifdef FEATURE_GSM_DTM
    MSG_HIGH_DS_0( MM_SUB, "=MM= RAU blocked - Either mm_state != IDLE and no DTM OR DTM supported, but PS suspension by RR in progress");
#else
    MSG_HIGH_DS_0( MM_SUB, "=MM= RAU blocked - mm_state != IDLE");
#endif /* FEATURE_GSM_DTM */  

    gmm_set_state(GMM_REGISTERED);

    gmm_ra_update_pending = TRUE;
    gmm_pending_ra_update_type = gmm_ra_update_type;
    if (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH ||
        gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)
    {
       mm_pending_lu_request = NO_LU;
       MSG_HIGH_DS_0( MM_SUB, "=MM= Pending Normal LU flag cleared here as Combined proc is pended");
    }    
  }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
  else if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY &&
            !gmm_detach_after_rau_pending && 
            !(gmm_rat_chng_pended_msg_valid &&
#ifdef FEATURE_MODEM_HEAP
              (gmm_rat_chng_pended_msg != NULL) &&
#endif
              (byte) (((byte *) gmm_rat_chng_pended_msg + sizeof(IMH_T))[0] & 0x0F) ==  PD_GMM &&
              (byte) (((byte *) gmm_rat_chng_pended_msg + sizeof(IMH_T))[1]) == GMM_DETACH_REQUEST))
  {
    MSG_HIGH_DS_0( MM_SUB, "=MM= RA Update blocked -> ms_op_mode=CS_ONLY" );
  }
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
#ifdef  FEATURE_REL6_PS_FOP
  else if((mm_timer_status [ TIMER_T3340 ] == TIMER_ACTIVE) && (gmm_ps_fop_resp == PS_FOP_NW_NOT_GRANTED) )
  {
    /**************************************************************
    *24.008 4.7.1.9 The MS shall start the timer T3340  when the  *
    * network indicates "no follow-on proceed" in the             *
    *ROUTING AREA UPDATE ACCEPT or ATTACH ACCEPT message.         *
    *if the MS has signalling pending, then it shall request a    *
    * new PS signalling connection for further signalling         *
    **************************************************************/
    gmm_ra_update_pending = TRUE;
    gmm_pending_ra_update_type = gmm_ra_update_type;
    MSG_HIGH_DS_0( MM_SUB,"=MM= RAU blocked - T3340 is running");
  }
#endif
  else
  {
#ifdef FEATURE_DSAC
    /* ------------------------------------------------------------------------------
    ** TS 24.008: 
    ** section 4.1.1.2.1 GPRS MS operating in mode A or B in a network that .....
    **   
    ** A GPRS MS operating in mode A or B in a network that operates in mode I shall 
    ** perform a combined routing area update procedure indicating 
    ** "combined RA/LA updating with IMSI attach" (in order to establish the Gs
    ** association in the MSC/VLR) when the following conditions are fulfilled: 
    ** -  the GPRS MS detects that the PS domain changes from barred to unbarred;
    ** -  the CS domain is unbarred; and
    ** -  for the last attempt to update the registration of the location area an MM 
    ** specific procedure was performed (see subclause 4.7.5.2.1).
    ** ------------------------------------------------------------------------------ */
     
    if (((mm_ps_access_barred_to_unbarred == TRUE) ||
         (mm_cs_access_barred_to_unbarred == TRUE)) &&
        (((mm_last_procedure_to_update_registration & MM_LU) == MM_LU) ||
         ((mm_last_procedure_to_update_registration & MM_RAU) == MM_RAU)))
    {
       MSG_HIGH_DS_0( MM_SUB, "=MM= Updating RA Type to GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH as PS or CS is unbarred from barred" );
       gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
       mm_ps_access_barred_to_unbarred = FALSE;
       mm_cs_access_barred_to_unbarred = FALSE;
    }

    if ((mm_check_if_cs_cell_access_barred(cs_cell_access) == TRUE) &&
        ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
         (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)))
    {
       if ((gmm_rai_changed( gmm_previous_serving_rai.location_area_code,
                             gmm_previous_serving_rai.routing_area_code,
                             mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
           (gmm_plmn_changed(gmm_previous_serving_rai.plmn_id,
                             mm_serving_plmn.info.plmn)) ||
           (((mm_last_procedure_to_update_registration & MM_RAU) != MM_RAU)
#ifdef FEATURE_LTE
            || (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI)
            ||  gmm_is_lte_cap_changed()
#endif
           ))
       {      
          MSG_HIGH_DS_0( MM_SUB, "=MM= Updating RA Type to GMM_RA_UPDATING as CS is barred");
          gmm_ra_update_type = GMM_RA_UPDATING;
          mm_ps_access_barred_to_unbarred = FALSE;
          mm_cs_access_barred_to_unbarred = FALSE;       
       }
       else if (gmm_periodic_rau_pending)
       {
          MSG_HIGH_DS_0( MM_SUB, "=MM= Updating RA Type to GMM_PERIODIC_UPDATING as CS is barred");
          gmm_ra_update_type = GMM_PERIODIC_UPDATING;
       }       
       else
       {
          MSG_HIGH_DS_0(MM_SUB, "=MM= No need for RAU as RA is already updated and Gs association cannot be established - update the servife status as Service");

          gmm_update_mm_serving_plmn_info(mm_serving_plmn.info.sim_state,
                                         mm_serving_plmn.info.reg_service_domain, 
                                         FALSE, 
                                         SYS_SRV_STATUS_SRV, 
                                         FALSE);

          if (mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf)
          {
             mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
             mm_send_mmr_reg_cnf();
             mm_reg_waiting_for_reg_cnf = FALSE;
             mm_ready_to_send_reg_cnf   = FALSE;
          }
          return;
       }
    }
    
    if(gmm_ra_update_type == GMM_RA_UPDATING &&
       mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1 &&
       mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
    {
       mm_last_procedure_to_update_registration =
         (mm_last_procedure_to_update_registration & (~MM_COMBINED));    
       mm_last_procedure_to_update_registration |= MM_RAU;    
    }
    else if (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH ||
             gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)
    {
       mm_last_procedure_to_update_registration = MM_COMBINED;
/*Reset LU flag while doing Attach*/    
#ifdef FEATURE_NAS_REL10
#ifdef FEATURE_LTE 
      mm_is_lu_to_be_initiated_t3346_ltogw = FALSE;
#endif
#endif
    }         

#endif
#ifdef FEATURE_NAS_REL11
/*Reset PS handover RAU initiation flag*/
    gmm_is_lw_to_g_interrat_in_progress = FALSE;
#endif

    /* -------------------------------------------------------------------------
    ** To initiate the routing area updating procedure, the MS sends the message
    ** ROUTING AREA UPDATE REQUEST to the network, starts timer T3330, and
    ** changes to state GMM-ROUTING-AREA-UPDATING-INITIATED (24.008 4.7.5.1.1)
    ** ------------------------------------------------------------------------- */
    mm_stop_timer( TIMER_T3311 );

    mm_stop_timer( TIMER_T3302 );

    mm_stop_timer( TIMER_T3330);
    mm_stop_timer(TIMER_T3340);
    mm_stop_timer(TIMER_PS_CONN_REL);

#ifdef FEATURE_DUAL_SIM
    mm_stop_timer( TIMER_GMM_RADIO_RETRY );
#endif
#ifdef FEATURE_NAS_REL10
    if(mm_timer_status[TIMER_T3346]== TIMER_ACTIVE)
    {
      mm_stop_timer(TIMER_T3346);
      gmm_is_congestion_due_to_rau = FALSE;
      sys_plmn_undefine_plmn_id(&gmm_3gpp_t3346_plmn_id);
    }
    if ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING || gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) && 
         mm_timer_status[TIMER_T3246] == TIMER_ACTIVE && mm_is_plmn_equivalent_t3246_plmn() == FALSE)
    {
      mm_stop_timer(TIMER_T3246);
      sys_plmn_undefine_plmn_id(&mm_3gpp_t3246_plmn_id);
    }
#endif

    if (t3330_timeout_counter == 0)
    {
      gmm_stop_auth_fail_timers();
    }

    if ((is_gsm_mode()) || (ps_session_status == MMRRC_ACTIVE))
    {
      mm_start_timer( TIMER_T3330, DEFAULT_TIMEOUT );
    }

    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
    {
      mm_idle_transition_timer = NULL;
    }

    if(gmm_gprs_detach_pending)
    {
      gmm_detach_after_rau_pending = TRUE;
      gmm_gprs_detach_pending      = FALSE;
    }

    mm_clear_regs_complete_counter();

    gmm_set_state( GMM_ROUTING_AREA_UPDATING_INITIATED );

#ifdef FEATURE_ENHANCED_NW_SELECTION
    if(reg_sim_read_ens_flag() && (mm_foreground_search_lai_timer_list.length != 0))
    {
      gmm_update_ps_attempt_counter_from_foreground_search_lai_rai_timer_list((byte*)&gmm_current_lai,gmm_current_rac);
    }
#endif

#ifdef FEATURE_GSM_GPRS

    /* Required RAU procedure has already been initiated */
    gmm_rau_is_required = FALSE ;

#endif /* #ifdef FEATURE_GSM_GPRS */

    /* If the SIM is invalid for CS services, set ra_update_type to GMM_RA_UPDATING
     * except if ra_update_type is GMM_PERIODIC_UPDATING 
     */
    if ((!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY)) &&
        (gmm_ra_update_type != GMM_PERIODIC_UPDATING))
    {
       MSG_HIGH_DS_0( MM_SUB, "=MM= Updating RA Type to GMM_RA_UPDATING as SIM is invalid for CS");
       gmm_ra_update_type = GMM_RA_UPDATING;
    }

    /* -------------------------------------------------------------------------
    ** Section 4.7.5: In GSM, user data transmission in the MS shall be suspended
    ** during the routing area updating procedure; data reception shall be possible.
    ** The GMMLLC_SUSPEND_REQ would stop UL data, but DS should not be suspended as
    ** it can still received DL data.

    ** In UMTS, user data transmission and reception in the MS shall not be suspended
    ** during the routing area updating procedure.
    ** ------------------------------------------------------------------ */
    if (((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) &&
         ((mm_gs_association_established == FALSE)
#if defined(FEATURE_LTE)  
          && ((emm_ctrl_data_ptr->srv_domain_registered != SYS_SRV_DOMAIN_CS_PS)
            ||((emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_FOR_CSFB_RAU)||
               (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC)))
            
#endif
         )
         )
         )
    {
      /* ----------------------------------------------------------------
      ** If for the last attempt to update the registration of the location
      ** area a MM specific procedure was performed, the value of the Update type IE in the 
      ** RAU_REQUEST message shall indicate "combined RA/LA updating with IMSI attach".
      ** ---------------------------------------------------------------- */    
      gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;     
      /* Resetting the flag if it is set to CSFB RAU, So that It doesn't remain to CSFB Rau state */
#ifdef FEATURE_LTE
      if(emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_FOR_CSFB_RAU)
      {
        emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_NOT_REQ;
      }
#endif

    }
    if ((gmm_processing_directed_signalling_reestablishment ==TRUE) && 
       ((gmm_rai_changed( gmm_previous_registration_serving_rai.location_area_code,
                          gmm_previous_registration_serving_rai.routing_area_code,
                          mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
        (gmm_plmn_changed(gmm_previous_registration_serving_rai.plmn_id,
                            mm_serving_plmn.info.plmn))))
                             
    {
      gmm_processing_directed_signalling_reestablishment = FALSE;
    }

    gmm_foreground_search_pending = FALSE;
    
    if(is_gsm_mode() && (gmm_multi_slot_ra_update_pending == TRUE))
    {
      gmm_multi_slot_ra_update_pending = FALSE;
    }
    if ((mm_state != MM_IDLE) &&
        ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
         (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)))
    {
      if(mm_pending_lu_request==LTE_IRAT_LU)
      {
        gmm_irat_lu_pending= LTE_IRAT_LU;  
      }
	  if(mm_state == MM_LOCATION_UPDATE_INITIATED || mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
	  {

		mm_last_procedure_to_update_registration = mm_last_procedure_to_update_registration & (~MM_COMBINED);  
    	mm_last_procedure_to_update_registration |= MM_RAU;
		mm_last_procedure_to_update_registration |= MM_LU;
      } 
      mm_pending_lu_request = NO_LU;
      gmm_pending_ra_update_type = gmm_ra_update_type;
      gmm_ra_update_type = GMM_RA_UPDATING;
#ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
      mm_save_rat_pri_list_sent_to_nw();
#endif
      /* ----------------------------------------------------------------
      ** A GPRS MS in MS operation mode A shall perform the normal
      ** routing area update procedure during an ongoing circuit-switched
      ** transaction (3GPP TS 24.008 section 4.7.5.2.1).
      ** ---------------------------------------------------------------- */
      gmm_send_routing_area_update_request_message(
        gmm_ra_update_type, mm_get_current_security_context( RRC_PS_DOMAIN_CN_ID ));

       if(t3330_timeout_counter ==  0)
      {
        mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_START,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
      }
      else
      {
        mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_RESEND,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
      }

      /* ------------------------------------------------------------------------
      ** A GPRS MS in MS operation mode A that is in an ongoing circuit-switched
      ** transaction, shall initiate the combined routing area updating procedure
      ** after the circuit-switched transaction has been released.
      ** (3GPP TS 24.008 section 4.7.5.2.1).
      ** ------------------------------------------------------------------------ */
      gmm_ra_update_pending = TRUE;
#if defined(FEATURE_LTE)
     if(emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_NOT_REQ)
     {
       /* Set this flag only when it is not set, If it is set to any other value, keep the value as it is*/
       emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_FOR_CSFB_RAU;
       MSG_HIGH_DS_0(MM_SUB,"=MM= Set SGs assosicaiton flag - Combined RAU Pending due to CSFB");
     }
     else if(emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_DUE_TO_SRVCC)
     {
       /* Set this flag to CS_OR_CSPS PROC only when it is set to SRVCC*/
       emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
     }
     MSG_HIGH_DS_1(MM_SUB,"=MM= SGs assosicaiton flag - %d",emm_ctrl_data_ptr->sg_association_to_be_established );

     if(mm_csfb_state == MM_CSFB_STATE_LU_PENDING)
     {
       mm_csfb_set_state(MM_CSFB_STATE_NULL);
     }
#endif
    }
    else
    {
#ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
      mm_save_rat_pri_list_sent_to_nw();
#endif
      gmm_send_routing_area_update_request_message(
        gmm_ra_update_type, mm_get_current_security_context( RRC_PS_DOMAIN_CN_ID ));

      if(t3330_timeout_counter ==  0)
      {
        mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_START,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
      }
      else
      {
        mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_RESEND,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
      }
      if((mm_serving_plmn.network_op_mode != NETWORK_OP_MODE_1) ||
         !((mm_state != MM_IDLE) &&
           (gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING ||
            gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)))
      {
        gmm_ra_update_pending = FALSE;
      }
      gmm_periodic_rau_pending = FALSE;
    }
    mm_update_last_reg_dsac_info();
    gmm_is_ptmsi_present_in_prev_gmm_msg = FALSE;
    gmm_is_tmsi_present_in_prev_gmm_msg  = FALSE; 
    mm_is_gmm_moved_dereg_due_to_csreg   = FALSE;

    gmm_delete_gprs_forbidden_rai(); 

#if defined(FEATURE_ENHANCED_NW_SELECTION) || defined (FEATURE_LTE)
    gmm_force_registration = FALSE;
#endif

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
    mm_pending_lu_type = MM_LU_CAUSE_RESERVED;
#endif
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
    gmm_is_prev_mode_connected = FALSE;
    MSG_HIGH_DS_1(MM_SUB, "=MM= GMM Previous mode Ready or Connected = %d", gmm_is_prev_mode_connected);
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
    /* Storing the NMO as communication with NW is started on the PS */
    gmm_previous_network_op_mode = mm_serving_plmn.network_op_mode;
    
    (void) memscpy(&(gmm_previous_registration_serving_rai.plmn_id), 3, &(mm_serving_plmn.info.plmn), 3);
    (void) memscpy(&(gmm_previous_registration_serving_rai.location_area_code), 2, &(mm_serving_plmn.lac), 2);
    gmm_previous_registration_serving_rai.routing_area_code = mm_serving_plmn.rac;

    gmm_previous_T3311_gmm_substate = GMM_NULL_SUBSTATE;

    if ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
        (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH))
    {
      /* Remember that GS association is done in combine procedure */
      MSG_MED_DS_0( MM_SUB, "=MM= GS association is established in Combined RAU" );
      mm_gs_association_established = TRUE;
      (void) memscpy( mm_previous_serving_lai, LAI_SIZE,
                     mm_serving_lai,
                     LAI_SIZE );
      mm_initiate_lu_upon_entry_into_new_cell = FALSE;
      mm_pending_lu_request = NO_LU;
      mm_lu_pended_nmo_unknown = FALSE;
      mm_previous_idle_substate = mm_idle_substate;
      if((mm_location_information.location_update_status != UPDATED) ||
         ((memcmp(&(mm_location_information.lai[0]),&(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE) != 0 ) ||
          (memcmp(&(mm_location_information.lai[PLMN_SIZE]),&(mm_serving_plmn.lac.lac[0]), LAC_SIZE) != 0)))
      {
        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                        SYS_SRV_DOMAIN_NO_SRV,
                                        FALSE,
                                        SYS_SRV_STATUS_LIMITED,
                                        FALSE );
      }
#if defined(FEATURE_LTE)
      gmm_lte_deact_isr_timer_expired = FALSE;
      /* Donot set the sg association flag if we are in CCO. Because if CCO fails and UE
         comes back to LTE, we should not do TAU as LU request is not sent 
         out to the network. */
      if((emm_ctrl_data_ptr->l2gcco_state != EMM_LTOG_CCO_IN_PROGRESS) && (ps_session_status == MMRRC_ACTIVE))
      {
        emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
        MSG_HIGH_DS_0(MM_SUB,"=MM= Set SGs assosicaiton flag - Start Combined RAU");
      }
      if(mm_csfb_state == MM_CSFB_STATE_LU_PENDING)
      {
        mm_csfb_set_state(MM_CSFB_STATE_NULL);
      }
#endif
#ifdef FEATURE_GSTK 
      /* Location Status Event to SIM may be required since LOCI is updated */
      if ((!reg_nv_is_imsi_switch_enabled()))
      {
         mm_gstk_set_location_status_evt (TRUE);
      }
#endif 
    }
    else if (gmm_ra_update_type == GMM_RA_UPDATING)
    {
      /* GS association is not happened in non-combined procedure */
      MSG_MED_DS_0( MM_SUB, "=MM= GS association is removed when RAU is performed");
      mm_gs_association_established = FALSE;
    }
  }

} /* end gmm_initiate_routing_area_update() */


/*===========================================================================

FUNCTION GMM_INCREMENT_ROUTING_AREA_UPDATE_ATTEMPT_COUNTER

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_increment_routing_area_update_attempt_counter
( 
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
boolean npdu_no_list_present
#else
 void 
#endif
)
{
  byte location_information[LAI_SIZE];
  mmr_cause_e_type mmr_cause = MMR_CAUSE_NOT_SPECIFIED;
#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------
  ** Refer to TS 24.008 section 4.7.5.1.5
  ** ------------------------------------ */
  mm_stop_timer( TIMER_T3330 );
  t3330_timeout_counter   = 0;
  MSG_HIGH_DS_1( MM_SUB, "=MM= T3330 timeout counter = %d", t3330_timeout_counter);

  gmm_rau_attempt_counter++;
  MSG_HIGH_DS_1( MM_SUB, "=MM= RAU Attempt counter = %d", gmm_rau_attempt_counter );

  gmm_stop_auth_fail_timers();
  
  gmm_procedure_rerun_ongoing = FALSE;
  gmm_cause_directed_signalling_reest = FALSE;
  gmm_extend_connection_due_to_mt_csfb = FALSE;
 
#ifdef FEATURE_ENHANCED_NW_SELECTION 
     if(reg_sim_read_ens_flag() && mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai))
     {
       if (!((gmm_rai_changed( gmm_previous_registration_serving_rai.location_area_code,
                                gmm_previous_registration_serving_rai.routing_area_code,
                                mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
             (gmm_plmn_changed( gmm_previous_registration_serving_rai.plmn_id,
                                mm_serving_plmn.info.plmn))))
       {
         gmm_add_rac_to_foreground_search_lai_rai_timer_list((byte*)&gmm_current_lai,gmm_current_rac);
       }
     }
#endif

  if (gmm_rau_attempt_counter < 5)
  {
    if (((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                             gmm_stored_rai.routing_area_code,
                             mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
           (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
        (gmm_update_status == GMM_GU1_UPDATED)
#if defined(FEATURE_LTE)
        && (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI)
#endif
                                    )
        )
    {
      /* -----------------------------------------------------------------------
      ** The MS shall keep the GMM update status to GU1 UPDATED and change state
      ** to GMM-REGISTERED.NORMAL-SERVICE.  The MS shall start timer T3311.
      ** When timer T3311 expires the routing area updating procedure is
      ** triggered again.
      ** ----------------------------------------------------------------------- */
      gmm_set_substate( GMM_NORMAL_SERVICE );
      gmm_set_state( GMM_REGISTERED );
      mm_start_timer( TIMER_T3311, DEFAULT_TIMEOUT );
      gmm_previous_T3311_gmm_substate = gmm_substate;
    }
    else
    {
      /* --------------------------------------------------------------------
      ** The stored RAI is different than the RAI of the current serving cell
      ** or the GMM update status is different than GU1 UPDATED...
      ** The MS shall start timer T3311, shall set the GPRS update status to
      ** GU2 NOT UPDATED and change state to GMM-REGISTERED.ATTEMPTING-TO-UPDATE
      ** ----------------------------------------------------------------------- */
      mm_start_timer( TIMER_T3311, DEFAULT_TIMEOUT );
      gmm_update_status = GMM_GU2_NOT_UPDATED;
#if defined(FEATURE_LTE)
      if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
      {
        emm_set_update_status_rau_abnormal_failure(EMM_STATUS_NOT_UPDATED);
      }
#endif
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
      gmm_set_state( GMM_REGISTERED );
      gmm_previous_T3311_gmm_substate = gmm_substate;       
    }

    /* Apply the clause 24.008 4.7.5.2.5 */

    if ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
       (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH))
    {
      /*
      if the update status is U1 UPDATED, and the stored LAI is equal to the
      one of the current serving cell and the routing area updating attempt counter
      is smaller than 5, then the mobile station shall keep the update status
      to U1 UPDATED, the new MM state is MM IDLE substate NORMAL SERVICE;
      */
  
      location_information[0] = 0xFF;
      location_information[1] = 0xFF;
      location_information[2] = 0xFF;
      location_information[3] = 0x00;
      location_information[4] = 0x00;
      if(memcmp(mm_serving_lai,location_information,PLMN_SIZE) == 0)
      {
        memscpy(location_information,3,&gmm_previous_registration_serving_rai.plmn_id,3);
        memscpy(location_information+3,2,&gmm_previous_registration_serving_rai.location_area_code,2);
      }
      else
      {
        memscpy(location_information,LAI_SIZE,mm_serving_lai,LAI_SIZE);
      }   
      if (( mm_location_information.location_update_status == UPDATED ) &&
          ( memcmp( mm_location_information.lai, location_information, LAI_SIZE ) == 0))
      {

         mm_substate_control( MM_STD_NORMAL_SERVICE );
#ifdef FEATURE_NAS_REL10
         if(mm_is_per_ms_t3212_present)
         {
            if ((mm_stored_per_ms_t3212_value != 0 ) &&
                (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
            {

               mm_start_timer( TIMER_T3212,
                               mm_stored_per_ms_t3212_value);
            }
         }
         else
#endif
         if ((mm_system_information.T3212_timer_value != 0 ) &&
             (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
         {
           /* -----------------------------------------------------------------
           ** If the (T3212) timer is not already started, the timer is started
           ** each time the mobile enters the MM_IDLE substate NORMAL_SERVICE
           ** or ATTEMPTING_TO_UPDATE (24.008 section 4.4.2).
           ** ----------------------------------------------------------------- */
           mm_start_timer( TIMER_T3212,
                           mm_system_information.T3212_timer_value );
         }
      }
      else
      {
         mm_delete_lu_status( NOT_UPDATED );

         /* Delete equivalent PLMN list */
         mm_serving_plmn.update_equiv_plmn_list = TRUE;
         memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));
         if ((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                  gmm_stored_rai.routing_area_code,
                                  mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                   (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
             (gmm_update_status == GMM_GU1_UPDATED))
         {
         gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                          SYS_SRV_DOMAIN_PS_ONLY,
                                          FALSE,
                                          SYS_SRV_STATUS_SRV,
                                          TRUE);
         }
         else
         {
           gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                          SYS_SRV_DOMAIN_NO_SRV,
                                          FALSE,
                                          SYS_SRV_STATUS_LIMITED,
                                          TRUE );
      }
    }
    }

    /* The abnormal handling section 4.7.5.1.5 enforces GMM to be in REGISTERED.NORMAL-SERVICE
       or GMM-REGISTERED.ATTEMPTING-TO-UPDATE and start T3311. Further RAU will be triggered once
       the timer is expired.
       During this period REG should not trigger a PLMN selection because of this failure. */

    mmr_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
  }
  else /* routing_area_updating_attempt_counter >= 5 */
  {
    mmr_cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;

    /* Reset only PS side flag, since LU procedure will be triggered now for max attempt of combined procedure */
    gmm_manual_mode_user_selection_procedure = FALSE;

#ifdef FEATURE_NAS_REL11
    /*24.008: 4.7.2.7:
    The default value of timer T3302 is used if a new PLMN which is not in the list of equivalent PLMNs has been entered, 
                                the routing area updating fails and the routing area updating attempt counter is equal to 5*/
    if(gmm_t3302_timer_value != 720000 && (gmm_plmn_changed(gmm_previous_plmn_with_t3302_value, mm_serving_plmn.info.plmn))&& 
                           !(sys_eplmn_list_equivalent_plmn(gmm_previous_plmn_with_t3302_value) && sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))
    {
      gmm_t3302_timer_value = 720000;
      MSG_HIGH_DS_1(MM_SUB,"=MM=The default value of this timer T3302 is used if a new PLMN which is not in the list of equivalent PLMNs has been entered, gmm_t3302_timer_value = %d", gmm_t3302_timer_value);
    }
#endif
    /* -----------------------------------------------------------------------------
    ** The MS shall start timer T3302, shall set the GPRS update status to
    ** GU2 NOT UPDATED and shall change to state GMM-REGISTERED.ATTEMPTING-TO-UPDATE
    ** or optionally to GMM-REGISTERED.PLMN-SEARCH (???)
    ** ----------------------------------------------------------------------------- */
    if (gmm_t3302_timer_value != 0)
    {
      mm_start_timer( TIMER_T3302, gmm_t3302_timer_value );
    }

    gmm_update_status = GMM_GU2_NOT_UPDATED;
    mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                        gmm_stored_ptmsi_signature,
                                        gmm_stored_rai,
                                        gmm_update_status );
#if defined(FEATURE_LTE) 
    emm_set_update_status_rau_abnormal_failure(EMM_STATUS_NOT_UPDATED);
#endif

    gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE );
    gmm_set_state( GMM_REGISTERED );

    gmm_processing_directed_signalling_reestablishment = FALSE;
    gmm_previous_T3311_gmm_substate = gmm_substate; 
    

    MSG_HIGH_DS_0( MM_SUB, "=MM= RAU attempt counter reached its max value >=5)");

    if ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
        (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH))
    {
      mm_delete_lu_status( NOT_UPDATED );


#ifdef FEATURE_NAS_REL10
      if(mm_is_per_ms_t3212_present)
      {
          if ((mm_stored_per_ms_t3212_value != 0 ) &&
              (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
          {

            mm_start_timer( TIMER_T3212,
                            mm_stored_per_ms_t3212_value);
          }
      }
      else
#endif
      if ((mm_system_information.T3212_timer_value != 0 ) &&
          (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
      {
        /* -----------------------------------------------------------------
        ** If the (T3212) timer is not already started, the timer is started
        ** each time the mobile enters the MM_IDLE substate NORMAL_SERVICE
        ** or ATTEMPTING_TO_UPDATE (24.008 section 4.4.2).
        ** ----------------------------------------------------------------- */

        mm_start_timer( TIMER_T3212,
                        mm_system_information.T3212_timer_value );
      }

      /* Delete equivalent PLMN list */
      mm_serving_plmn.update_equiv_plmn_list = TRUE;
      memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));

      mmr_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;

        if ((mm_state == MM_WAIT_FOR_RR_CONNECTION_MM) ||
            (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
            (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) ||
            (mm_state == MM_WAIT_FOR_REESTABLISH_DECISION) ||
            (mm_state == MM_WAIT_FOR_RR_CONNECTION_REESTABLISHMENT) ||
            (mm_state == MM_REESTABLISHMENT_INITIATED) ||
            (mm_state == MM_CONNECTION_ACTIVE) ||
            ((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) && 
                       (((mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS) &&
                          (mm_last_paging_cause == (byte)CS_PAGING)) ||
                        ((mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS) &&
                         (mm_last_paging_cause != 0xFF)) ||
                        ((mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS) &&
                         (mm_last_paging_cause != 0xFF))))
#ifdef FEATURE_CCBS
                                               ||
            (mm_state == MM_PROCESS_CM_SERVICE_PROMPT)
#endif /* FEATURE_CCBS */
            )
      {
           gmm_foreground_search_pending= TRUE;
        }
        else
        {
          mmr_cause = MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH;
          gmm_foreground_search_pending  = FALSE;
      }
    }

    if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
       (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
       (mm_location_information.location_update_status == UPDATED) &&
       (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0) &&
       ((mm_serving_plmn.info.reg_service_domain  == SYS_SRV_DOMAIN_CS_PS) ||
        (mm_serving_plmn.info.reg_service_domain  == SYS_SRV_DOMAIN_CS_ONLY)))
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

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#ifdef FEATURE_GSM_GPRS
    gmm_ps_data_available = FALSE ;
    gmm_rat_chng_pended_msg_valid = FALSE ;
#endif /* #ifdef FEATURE_GSM_GPRS */
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

    /* delete equivalent PLMN list (24.008, 4.7.5.2.5) */
    mm_serving_plmn.update_equiv_plmn_list = TRUE;
    memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));

  }

#ifdef FEATURE_NAS_REL10
  if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
     (mm_timer_status[TIMER_T3246]== TIMER_ACTIVE))
  {
    mmr_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
  } 
#endif

  if ( (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY) &&
       gmm_detach_after_rau_pending)
  {
     gmm_handle_local_gprs_detach();
     gmm_detach_after_rau_pending = FALSE;
     return;
  }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA) 
  if (gmm_rat_chng_pended_msg_valid &&
#ifdef FEATURE_MODEM_HEAP
       (gmm_rat_chng_pended_msg != NULL) &&
#endif
       (byte) (((byte *) gmm_rat_chng_pended_msg + sizeof(IMH_T))[0] & 0x0F) ==  PD_GMM &&
       (byte) (((byte *) gmm_rat_chng_pended_msg + sizeof(IMH_T))[1]) == GMM_DETACH_REQUEST)
  {
     gmm_handle_local_gprs_detach();
     gmm_rat_chng_pended_msg_valid = FALSE;
#ifdef FEATURE_MODEM_HEAP
     if(gmm_rat_chng_pended_msg != NULL)
     {
       modem_mem_free(gmm_rat_chng_pended_msg,MODEM_MEM_CLIENT_NAS);
       gmm_rat_chng_pended_msg = NULL;
     }
#endif
     return;
  }
#ifdef FEATURE_MODEM_HEAP
  if((gmm_rat_chng_pended_msg_valid == FALSE) &&
     (gmm_rat_chng_pended_msg != NULL))
  {
    modem_mem_free(gmm_rat_chng_pended_msg,MODEM_MEM_CLIENT_NAS);
    gmm_rat_chng_pended_msg = NULL;
  }
#endif
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
#ifdef FEATURE_GSM_GPRS
  if (mm_serving_plmn.info.ps_data_suspend)
  {
    if (gmm_substate == GMM_NORMAL_SERVICE)
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= GMM_NORMAL_SERVICE, DATA_SUSPEND=FALSE");
      mm_serving_plmn.info.ps_data_suspend = FALSE;
    }

    if (mm_reg_waiting_for_reg_cnf)
    {
      if((((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
           (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
          (mm_lu_attempt_counter <4)) ||
         ((mm_state == MM_LOCATION_UPDATE_INITIATED) ||
          (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)  ||
          (mm_state == MM_LOCATION_UPDATE_REJECTED)))
      {
         MSG_HIGH_DS_0( MM_SUB, "=MM= LU Attempt counter has not reached the max limit, continue in the same RAT");
      }
      else
      {
      mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
      mmcoord_mmr_reg_cnf.cause         = mmr_cause;
      mm_send_mmr_reg_cnf();
      mm_reg_waiting_for_reg_cnf = FALSE;
      mm_ready_to_send_reg_cnf   = FALSE;
    }
    }
    else
    {
/* Do not send service_ind as Limited_Regional in case of TRM failure as it leads to wrong display on UI */
#ifdef FEATURE_DUAL_SIM
      if((mm_serving_plmn.info.service_status == SYS_SRV_STATUS_NO_SRV) &&
          (mm_serving_plmn.info.acq_status == SYS_ACQ_STATUS_NO_RESOURCE))
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= Will not send limited regional to CM in this case, as it's a TRM failure\n");
      }
      else
#endif
      {
        mmr_service_ind_s_type mmr_service_ind;
        mmr_service_ind.cause = mmr_cause;
        mmr_service_ind.service_state = mm_serving_plmn.info;
        mm_send_mmr_service_ind( &mmr_service_ind );
      }
    }
  }
  else /* !mm_serving_plmn.info.ps_data_suspend */
  {
    /* -------------------------------------------------------------------------------
    ** No data shall be sent or received by MS in GMM-REGISTERED.ATTEMPTING-TO-UPDATE
    ** -------------------------------------------------------------------------------*/
    if (gmm_substate == GMM_ATTEMPTING_TO_UPDATE)
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= Suspending data services");
      mm_serving_plmn.info.ps_data_suspend = TRUE;
    }

    if (mm_reg_waiting_for_reg_cnf)
    {
      if((((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
           (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
           (mm_lu_attempt_counter <4)) ||
          ((mm_state == MM_LOCATION_UPDATE_INITIATED) ||
           (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)  ||
           (mm_state == MM_LOCATION_UPDATE_REJECTED)))
      {
         MSG_HIGH_DS_0( MM_SUB, "=MM= LU Attempt counter has not reached the max limit, continue in the same RAT");
      }
      else
      {
      mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
      mmcoord_mmr_reg_cnf.cause         = mmr_cause;
      mm_send_mmr_reg_cnf();
      mm_reg_waiting_for_reg_cnf = FALSE;
      mm_ready_to_send_reg_cnf   = FALSE;
    }
    }
    else
    {
      mmr_service_ind_s_type mmr_service_ind;
      mmr_service_ind.cause = mmr_cause;
      mmr_service_ind.service_state = mm_serving_plmn.info;
      mm_send_mmr_service_ind( &mmr_service_ind );
    }
  }
#endif /* FEATURE_GSM_GPRS */
} /* end gmm_increment_routing_area_update_attempt_counter() */



/*===========================================================================

FUNCTION GMM_HANDLE_ROUTING_AREA_UPDATE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
void gmm_handle_routing_area_update( mm_cmd_type *message )
{
  gmm_ra_update_accept_msg_T* ra_update_accept_msg;
  gmm_ra_update_reject_msg_T ra_update_reject_msg;
  inter_task_lai_T           registered_lai;
  gmm_npdu_number_list_type  npdu_number_list;
#ifdef FEATURE_WCDMA
  rrc_cmd_type               *rrc_cmd;
#endif /* #ifdef FEATURE_WCDMA */
  byte                    previous_plmn_identity[PLMN_SIZE]; 
  routing_area_id_T        gmm_prev_stored_rai;                     
  sm_cmd_type                *sm_cmd;
  rabm_cmd_type              *rabm_cmd;
  byte                       i;
  mmr_service_ind_s_type     mmr_service_ind;
  sys_plmn_id_s_type         RAU_accept_PLMN;
  boolean                    need_to_send_service_ind = FALSE;
  boolean                    allocated_tmsi_present   = FALSE;
  byte protocol_discriminator = 0;
  byte message_id             = 0;
  dword                      t3312_timer_value = gmm_t3312_timer_value; 
  boolean is_data_available = FALSE;

mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION); 
mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION); 

#ifdef FEATURE_GSTK
  sys_srv_status_e_type prev_service_status, curr_service_status;
  boolean service_status_change = FALSE;
#endif


#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  boolean send_lte_reselection_ind = FALSE;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_NAS_REL10
  gmm_t3346_backoff_timer_value = 0;
#endif

  if (message == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= Unexpected NULL pointer", 0,0,0 );
  }

  switch (message->cmd.hdr.message_set)
  {
    case MS_MM_RR:
      switch (message->cmd.hdr.message_id)
      {
        case RR_ACTIVATION_IND:
          /* ------------------------------------------------------------------
          ** Lower layer failure - the procedure shall be aborted.  Timer T3330
          ** shall be stopped if still running.  The routing area updating
          ** attempt counter shall be incremented.
          ** ------------------------------------------------------------------ */
          gmm_increment_routing_area_update_attempt_counter
          (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
            FALSE
#endif
          );
          mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
#ifdef FEATURE_GPRS_PS_HANDOVER  
          gmm_psho_status = GMM_PSHO_NONE;
#endif          
          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected RR message %d", message->cmd.hdr.message_id, 0,0 );
      } /* end switch (message->cmd.hdr.message_id) */
      break;

    case MS_MM_RRC:
    case MS_MM_TDSRRC:
      switch (message->cmd.hdr.message_id)
      {
        case RRC_ACTIVATION_IND:
          /* ------------------------------------------------------------------
          ** Lower layer failure - the procedure shall be aborted.  Timer T3330
          ** shall be stopped if still running.  The routing area updating
          ** attempt counter shall be incremented.
          ** ------------------------------------------------------------------ */
          gmm_increment_routing_area_update_attempt_counter
          (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
            FALSE
#endif
          );
          mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
#ifdef FEATURE_GPRS_PS_HANDOVER  
          gmm_psho_status = GMM_PSHO_NONE;
#endif          
          break;

        case RRC_ABORT_IND:
          if (message->cmd.rrc_abort_ind.abort_cause == RRC_AC_NOT_ALLOWED_PS_DOMAIN)
          {
            /* ----------------------------------------------------------------------
            ** Access barred because of access class control - The routing area
            ** updating procedure shall not be started.  The MS stays in the current
            ** serving cell and applies the normal cell reselection process.  The
            ** procedure is started as soon as possible and if still necessary, i.e.,
            ** when the barred state is removed or because of a cell change.
            ** ---------------------------------------------------------------------- */
            gmm_set_substate( GMM_UPDATE_NEEDED );
            gmm_update_cell_status(message->cmd.rrc_abort_ind.abort_cause);
            gmm_set_state( GMM_REGISTERED );
            gmm_cause_directed_signalling_reest =FALSE;

            if((mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE)&&
               ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
                (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)||
                (gmm_ra_update_type == GMM_PERIODIC_UPDATING))&&
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
            {
              if (mm_state == MM_IDLE)
              {
                MSG_HIGH_DS_1(MM_SUB, "=MM= CS Registration started due to AC blocking for PS domain %d",
                                                                                                       ps_cell_access);
                mm_initiate_lu(MM_LU_CAUSE_NORMAL);
          }
              else
              {
                mm_pending_lu_request = NORMAL_LU;
              }

            }
          }
          else if (message->cmd.rrc_abort_ind.abort_cause == RRC_AC_NOT_ALLOWED_CS_DOMAIN)
          {
            /* ---------------------------------------------
            ** CS Access barred because of access class control
            ** --------------------------------------------- */
            gmm_cause_directed_signalling_reest = FALSE;
            if ((gmm_check_if_ps_cell_access_allowed(ps_cell_access) == TRUE) &&
                ((gmm_ra_update_type == GMM_RA_UPDATING) ||
                 (gmm_ra_update_type == GMM_PERIODIC_UPDATING &&
                  ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                   || (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
                 ))))
            {
              MSG_HIGH_DS_1(MM_SUB, "=MM= PS Registration started due to AC blocking for CS domain %d",
                                                                                                    cs_cell_access);
              if ((gmm_ra_update_pending) && ((gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) ||
                  (gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)))
              {
                 gmm_ra_update_type = gmm_pending_ra_update_type ;
              }                                                                                 
              gmm_initiate_routing_area_update();
             }
             else
             {
                 gmm_set_substate( GMM_UPDATE_NEEDED );
                 gmm_set_state( GMM_REGISTERED );
             }
          }
#ifdef FEATURE_SILENT_REDIAL_AFTER_RA_LA_MISMATCH
          else if (message->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_LAI_RAI_CHANGED)
          {
            /* dont increment counter as its not a failure - wait for service indication to take care of this RAU*/
            if (gmm_update_status == GMM_GU1_UPDATED)
            {
              gmm_set_substate( GMM_NORMAL_SERVICE );
            }
            else
            {
              gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE );
            }

            gmm_set_state( GMM_REGISTERED );
          }
#endif
#ifdef FEATURE_DUAL_SIM
          else if (message->cmd.rrc_abort_ind.abort_cause == RRC_NO_RESOURCE_AVAILABLE||
                   message->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_CONN_REJECT_WAIT_TMR)
          {

            if ((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                     gmm_stored_rai.routing_area_code,
                                     mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                   (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
                (gmm_update_status == GMM_GU1_UPDATED)
#ifdef FEATURE_LTE
                &&(emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI)
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

            gmm_cause_directed_signalling_reest = FALSE;

            gmm_set_state( GMM_REGISTERED );
            if(message->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_CONN_REJECT_WAIT_TMR)
            {
              mm_start_timer( TIMER_GMM_RADIO_RETRY, message->cmd.rrc_abort_ind.conn_rej_wait_timer*1000);
            }
            else
            {
              mm_start_timer( TIMER_GMM_RADIO_RETRY, DEFAULT_TIMEOUT );
            }
          }
#endif
          else
          {
            /* ------------------------------------------------------------------
            ** Lower layer failure - the procedure shall be aborted.  Timer T3330
            ** shall be stopped if still running.  The routing area updating
            ** attempt counter shall be incremented.
            ** ------------------------------------------------------------------ */
            gmm_increment_routing_area_update_attempt_counter
            (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
            FALSE
#endif
            );
            mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
          }
          break;

        case RRC_OPEN_SESSION_CNF:
        case RRC_CLOSE_SESSION_IND:
        case RRC_CLOSE_SESSION_CNF:
        case RRC_REL_IND:
        case RRC_ABORT_CNF:
          /* ------------------------------------------------------------------
          ** Lower layer failure - the procedure shall be aborted.  Timer T3330
          ** shall be stopped if still running.  The routing area updating
          ** attempt counter shall be incremented.
          ** ------------------------------------------------------------------ */
          gmm_increment_routing_area_update_attempt_counter
          (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
           FALSE
#endif
          );
          mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);

          break;

        case MM_DATA_IND:
          switch (message->cmd.mm_data_ind.L3_data[1])
          {
            case GMM_ROUTING_AREA_UPDATE_ACCEPT:
              /* ---------------------------------------------------
              ** The routing area updating request has been accepted
              ** by the network (TS 24.008 4.7.5.1.3)
              ** --------------------------------------------------- */
#ifndef FEATURE_MODEM_HEAP
              ra_update_accept_msg = (gmm_ra_update_accept_msg_T*)gs_alloc(sizeof(gmm_ra_update_accept_msg_T));
#else
              ra_update_accept_msg = (gmm_ra_update_accept_msg_T*)modem_mem_calloc(1, sizeof(gmm_ra_update_accept_msg_T), MODEM_MEM_CLIENT_NAS);
#endif
              mm_check_for_null_ptr((void*)ra_update_accept_msg);
              if (gmm_valid_routing_area_update_accept_message(
                    message, ra_update_accept_msg ))
              {

                auth_reset_cache_memory(FALSE,NO_TIMER_EXPIRED, RRC_PS_DOMAIN_CN_ID);    
                RAU_accept_PLMN.identity[0] =  ra_update_accept_msg->routing_area_identification[0];
                RAU_accept_PLMN.identity[1] =  ra_update_accept_msg->routing_area_identification[1];
                RAU_accept_PLMN.identity[2] =  ra_update_accept_msg->routing_area_identification[2];

#ifdef FEATURE_LTE				
                /*Reset flag as soon as received RAU accept  */
                mm_rau_pending_in_conn_mode = FALSE;
#endif
                gmm_extend_connection_due_to_mt_csfb = FALSE;

                if((!mm_is_plmn_valid(RAU_accept_PLMN)) ||
                   ((mm_reg_waiting_for_reg_cnf) && 
                    ((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
                     (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
                      mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)) &&
                    !(sys_plmn_match(mm_serving_plmn.info.plmn , RAU_accept_PLMN ) ||
                      sys_eplmn_list_equivalent_plmn(RAU_accept_PLMN))))
                {
                  MSG_HIGH_DS_3(MM_SUB, "=MM= MCC/MNC %02x %02x %02x invalid -> retry RAU",
                                RAU_accept_PLMN.identity[0],
                                RAU_accept_PLMN.identity[1],
                                RAU_accept_PLMN.identity[2]);
                  if(gmm_state==GMM_ROUTING_AREA_UPDATING_INITIATED)
                  {
                    gmm_increment_routing_area_update_attempt_counter
                    (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                     FALSE
#endif
                    );
                    mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);

                  }
#ifndef FEATURE_MODEM_HEAP
                  gs_free(ra_update_accept_msg);
#else
                  modem_mem_free(ra_update_accept_msg, MODEM_MEM_CLIENT_NAS);
#endif
                  return;
                }

                /* reset the gmm suspended flag */
                gmm_is_suspended = FALSE;
                
				if(gmm_state==GMM_REGISTERED)
                {   
                  if(ra_update_accept_msg->list_of_receive_npdu_numbers_present )
                  {
                    for (i=0; i < GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS; i++)
                    {
                      npdu_number_list.nsapi_valid_list[i] =
                      ra_update_accept_msg->nsapi_valid_list[i];
                      npdu_number_list.npdu_number_list[i] =
                      ra_update_accept_msg->npdu_number_list[i];
                    }
#ifdef FEATURE_GSM_GPRS
                    gmmsm_process_receive_npdu_number_list(
                     npdu_number_list.nsapi_valid_list, npdu_number_list.npdu_number_list
#ifdef FEATURE_DUAL_SIM
                     ,(sys_modem_as_id_e_type)mm_as_id
#endif
                    );
#endif
                  }
                  if (ra_update_accept_msg->ms_identity_present &&
                       ((ra_update_accept_msg->ms_identity.id_type & 0x7) == 0x4))
                  {
                      allocated_tmsi_present = TRUE;
                  }
#ifdef FEATURE_DUAL_DATA
                  if (is_gsm_mode())
                  {
                     if (!ra_update_accept_msg->allocated_ptmsi_present &&  !allocated_tmsi_present)
                     {
                        mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_LOW);
                     }
                  }
#endif 
                  if((ra_update_accept_msg->allocated_ptmsi_present) ||
                     (allocated_tmsi_present) ||
                     (ra_update_accept_msg->list_of_receive_npdu_numbers_present))
                  {
                    gmm_send_routing_area_update_complete_message(
                      ra_update_accept_msg->list_of_receive_npdu_numbers_present,
                      &npdu_number_list );
                  }
                  if(ra_update_accept_msg != NULL)
                  {
#ifndef FEATURE_MODEM_HEAP
                    gs_free(ra_update_accept_msg);
#else
                    modem_mem_free(ra_update_accept_msg, MODEM_MEM_CLIENT_NAS);
#endif                
                  }  
                  return;
                }
                /* ------------------------------------------------------------
                ** Upon receipt of a ROUTING AREA UPDATE ACCEPT message, the MS
                ** stores the received routing area identification, stops timer
                ** T3330, shall reset the routing area updating attempt counter
                ** and sets the GPRS update status to GU1 UPDATED.
                ** ------------------------------------------------------------ */
               /* store previous serving rai in case its needed further, before changing its value.. */
                (void)memscpy(previous_plmn_identity,PLMN_SIZE, gmm_stored_rai.plmn_id.identity,PLMN_SIZE);
                 memset(&gmm_prev_stored_rai, 0x00, sizeof(routing_area_id_T));

                (void)memscpy(&gmm_prev_stored_rai,sizeof(routing_area_id_T), &gmm_stored_rai,sizeof(routing_area_id_T));
                
                gmm_stored_rai.plmn_id.identity[0] =
                  ra_update_accept_msg->routing_area_identification[0];
                gmm_stored_rai.plmn_id.identity[1] =
                  ra_update_accept_msg->routing_area_identification[1];
                gmm_stored_rai.plmn_id.identity[2] =
                  ra_update_accept_msg->routing_area_identification[2];
                gmm_stored_rai.location_area_code.lac[0] =
                  ra_update_accept_msg->routing_area_identification[3];
                gmm_stored_rai.location_area_code.lac[1] =
                  ra_update_accept_msg->routing_area_identification[4];
                gmm_stored_rai.routing_area_code =
                  ra_update_accept_msg->routing_area_identification[5];

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                if (is_gsm_mode())
                {
                  gmm_transaction_id++;

                  MSG_HIGH_DS_0(MM_SUB,"=MM= Transaction id Incremented Since ROUTING AREA UPDATE ACCEPT is received ");
                }
#endif
                gmm_foreground_search_pending = FALSE;

               if(gmm_ra_update_type == GMM_PERIODIC_UPDATING)                           
               {
                 gmm_send_reset_ind_to_ds(DS_3GPP_RAU_IND, DS_3GPP_RAU_COMPLETE_EVT,DS_3GPP_PERIODIC_TYPE);
               }
               else if ((gmm_plmn_changed( gmm_stored_rai.plmn_id, gmm_prev_stored_rai.plmn_id )) ||
                        (gmm_rai_changed ( gmm_stored_rai.location_area_code,
                                           gmm_stored_rai.routing_area_code,
                                           gmm_prev_stored_rai.location_area_code, gmm_prev_stored_rai.routing_area_code )))
               {
                 gmm_send_reset_ind_to_ds(DS_3GPP_RAU_IND, DS_3GPP_RAU_COMPLETE_EVT,DS_3GPP_RA_CAHNGE_TYPE);
               }
               else
               { 
                 gmm_send_reset_ind_to_ds(DS_3GPP_RAU_IND, DS_3GPP_RAU_COMPLETE_EVT,DS_3GPP_OTHER_TYPE);
               }


#ifdef FEATURE_LTE
                /* Set EMM state with flag is attach false */
                emm_set_state_multimode_registration_success(FALSE);
                if(emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent == NAS_SEND_DRX_ON_LTE_RAT) 
                {
                  emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent = NAS_SEND_DRX_ON_ALL_RATS;
                }
                else if(emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent == NAS_SEND_DRX_ON_ALL_RATS)
                {
                  emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent = NAS_SEND_DRX_ON_GW_RAT;
                }

#endif
                  
                /*------------------------------------------------------------
                ** Update currently camped PLMN/LA/RA with the PLMN/LA/RA Id received
                ** in RAU Accept message. This may happen as RAU may have been performed
                ** due to DSAC spec rules, however, RRC didn't inform currently
                ** camped PLMN/LA/RA Id due to RRC spec prohibiting it from informing
                ** so in connected mode.
                **-------------------------------------------------------------*/  
                if ((gmm_plmn_changed( gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn )) ||
                    (gmm_rai_changed ( gmm_stored_rai.location_area_code,
                                       gmm_stored_rai.routing_area_code,
                                       mm_serving_plmn.lac, mm_serving_plmn.rac )))
                {
                  MSG_HIGH_DS_0(MM_SUB, "=MM= RA Id in RAU Accept and the one informed by RRC differ");
                                                                       
                  gmm_previous_serving_rai.plmn_id                   = gmm_current_lai.PLMN_id;
                  gmm_previous_serving_rai.location_area_code.lac[0] = gmm_current_lai.location_area_code[0];
                  gmm_previous_serving_rai.location_area_code.lac[1] = gmm_current_lai.location_area_code[1];
                  gmm_previous_serving_rai.routing_area_code         = gmm_current_rac;
                  
                  gmm_current_lai.PLMN_id.identity[0]   = mm_serving_plmn.info.plmn.identity[0] = gmm_stored_rai.plmn_id.identity[0];
                  gmm_current_lai.PLMN_id.identity[1]   = mm_serving_plmn.info.plmn.identity[1] = gmm_stored_rai.plmn_id.identity[1];
                  gmm_current_lai.PLMN_id.identity[2]   = mm_serving_plmn.info.plmn.identity[2] = gmm_stored_rai.plmn_id.identity[2];
                  gmm_current_lai.location_area_code[0] = mm_serving_plmn.lac.lac[0] =  gmm_stored_rai.location_area_code.lac[0];
                  gmm_current_lai.location_area_code[1] = mm_serving_plmn.lac.lac[1] =  gmm_stored_rai.location_area_code.lac[1];                  
                  gmm_current_rac                       = mm_serving_plmn.rac = gmm_stored_rai.routing_area_code;
                }
                
                mm_stop_timer( TIMER_T3330 );
                gmm_update_status = GMM_GU1_UPDATED;

                if ((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) && (is_gsm_mode()))
                {
                  mm_start_timer(TIMER_T3350, DEFAULT_TIMEOUT);
                } 

                gmm_processing_directed_signalling_reestablishment = FALSE;
                gmm_procedure_rerun_ongoing                        = FALSE;
                gmm_cause_directed_signalling_reest                = FALSE;

#ifdef FEATURE_GSM_GPRS

                cell_notification_ind = ra_update_accept_msg->cell_notification_present ;

                /* -------------------------------------------------------
                ** Set the ps_data_suspend to FALSE as the RAU is accepted
                ** ------------------------------------------------------- */
                MSG_HIGH_DS_0( MM_SUB, "=MM= RAU accepted, DATA_SUSPEND to FALSE");
                mm_serving_plmn.info.ps_data_suspend  = FALSE;
#endif /* #ifdef FEATURE_GSM_GPRS */
#ifdef FEATURE_NAS_REL10
                if(ra_update_accept_msg->nw_ftr_support_present)
                {
                   gmm_stored_nw_feature_support = ra_update_accept_msg->nw_ftr_support;
                }
#endif

                /* -----------------------------------------
                ** Set the need_to_send_service_ind flag since the equivalent plmn
                ** list needs to be updated.
                */
                need_to_send_service_ind = TRUE;

                /* -----------------------------------------------------------------
                ** If the message contains a P-TMSI, the MS shall use this P-TMSI as
                ** new temporary identity for GPRS services and shall store the new
                ** P-TMSI.  If no P-TMSI was included by the network in the ROUTING
                ** AREA UPDATING ACCEPT message, the old P-TMSI shall be kept.
                ** ----------------------------------------------------------------- */
                if (ra_update_accept_msg->allocated_ptmsi_present)
                {
                  /* -------------------------------------------
                  ** Delete the old P-TMSI and store the new one
                  ** ------------------------------------------- */
                  for (i=0; i < PTMSI_SIZE; i++)
                  {
                    gmm_stored_ptmsi.digit[i] =
                      ra_update_accept_msg->allocated_ptmsi[i];
                  }

                  gmm_is_ptmsi_present_in_prev_gmm_msg = TRUE;
                }

#if defined(FEATURE_LTE)
               /* --------------------------------------------------
                 24.008 4.7.5.1.4 
                 i) no indication that ISR is activated, an MS supporting S1 mode 
                    shall set the TIN to "P-TMSI"; or
                 ii) an indication that ISR is activated, the MS shall regard the 
                    available GUTI and TAI list as valid and registered with the network. 
                    If the TIN currently indicates "GUTI" and the periodic tracking 
                    area update timer T3412 is running, the MS shall set the TIN 
                    to "RAT-related TMSI". If the TIN currently indicates "GUTI" 
                    and the periodic tracking area update timer T3412 has already
                    expired, the MS shall set the TIN to "P-TMSI"..
               **---------------------------------------------*/
                if((ra_update_accept_msg->update_result != GMM_RA_UPDATED_AND_ISR_ACTIVATED
#ifdef FEATURE_LTE_REL10
                   ||(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10 &&
                      mm_timer_status[TIMER_T3412] == TIMER_STOPPED &&
                      emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI)
#endif
                   ) &&
                   (!(ra_update_accept_msg->update_result == GMM_COMBINED_RA_LA_UPDATED_AND_ISR_ACTIVATED &&
                        (gmm_ra_update_type == GMM_PERIODIC_UPDATING) &&
                        emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI))
                   )
                {
                  emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);
                  mm_stop_timer(TIMER_T3412);
                  mm_stop_timer(TIMER_T3423);
                }
                else if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI)
                {
                  emm_set_and_write_tin_type(NAS_MM_TIN_RAT_RELATED_TMSI);
                  MSG_HIGH_DS_0(MM_SUB,"=MM= Routing area update accept with ISR activated");
                }
                if(gmm_set_drx_req_pending == TRUE)
                {
                  mm_send_mmr_set_drx_cnf(TRUE);
                  mm_send_rrc_set_drx_ind();
                  emm_send_rrc_drx_update();
                  emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);
                }
                gmm_reg_lte_cs_param = 
                    mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param;
                gmm_reg_pended_lte_cs_param = 
                    mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param;
#ifdef FEATURE_NAS_REL11
                
                if ((ra_update_accept_msg->update_result == GMM_RA_UPDATED_AND_ISR_ACTIVATED ||
                     ra_update_accept_msg->update_result == GMM_COMBINED_RA_LA_UPDATED_AND_ISR_ACTIVATED)
                    &&
                    (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
                    &&
                    (emm_lte_cs_domain_param_match(gmm_reg_lte_cs_param,emm_ctrl_data_ptr->reg_lte_cs_domain_param) == FALSE))
                    {
                      // Rel 11 changes: Local ISR deactivation in the MS upon change of the UE's usage setting or the voice domain preference for E-UTRAN
                      MSG_HIGH_DS_0(MM_SUB,"=MM= Rel 11: deactivating ISR due to LTE voice domain pref change");
                      emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);
                    }

#endif
#endif

                /* ----------------------------------------------------------------
                ** Furthermore, the MS shall store the P-TMSI signature if received
                ** in the ROUTING AREA UPDATING ACCEPT message.  If no P-TMSI
                ** signature was included in the message, the old P-TMSI signature,
                ** if available, shall be deleted.
                ** ---------------------------------------------------------------- */
                if (ra_update_accept_msg->ptmsi_signature_present)
                {
                  /* -----------------------------------------------------
                  ** Delete the old P-TMSI signature and store the new one
                  ** ----------------------------------------------------- */
                  for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
                  {
                    gmm_stored_ptmsi_signature.value[i] =
                      ra_update_accept_msg->ptmsi_signature[i];
                  }
                }
                else
                {
                  /* -------------------------------
                  ** Delete the old P-TMSI signature
                  ** ------------------------------- */
                  for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
                  {
                    gmm_stored_ptmsi_signature.value[i] = 0xFF;
                  }
                }

                mm_serving_plmn.update_equiv_plmn_list = TRUE;

                if (ra_update_accept_msg->equiv_plmn_list_present)
                {
                  /*
                  ** Copy the equivalent PLMN list to the mm_serving_plmn structure
                  */
                  mm_serving_plmn.equiv_plmn_list.length = ra_update_accept_msg->equiv_plmn_list.length;
                  memscpy(mm_serving_plmn.equiv_plmn_list.plmn, sizeof(sys_plmn_id_s_type)*EQUIVALENT_PLMN_LIST_MAX_LENGTH,
                         ra_update_accept_msg->equiv_plmn_list.plmn,
                         ra_update_accept_msg->equiv_plmn_list.length);
                }
                else
                { 
                  /* If the msg does not contain an equivalent plmn list, the stored list
                  *should be deleted*/

                  memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));
                }
#ifdef FEATURE_LTE
                emm_update_rplmn_type(MM_PS_RPLMN_TYPE);
#endif
                mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                    gmm_stored_ptmsi_signature,
                                                    gmm_stored_rai,
                                                    gmm_update_status );
                
                /* ----------------------------------------------------------------
                ** If the LAI or PLMN identity contained in the ROUTING AREA UPDATE
                ** ACCEPT message is a member of any of the "forbidden lists" then
                ** any such entry shall be  deleted (24.008 4.7.5.1.3)
                ** ---------------------------------------------------------------- */
                registered_lai.PLMN_id.identity[0] =
                  ra_update_accept_msg->routing_area_identification[0];
                registered_lai.PLMN_id.identity[1] =
                  ra_update_accept_msg->routing_area_identification[1];
                registered_lai.PLMN_id.identity[2] =
                  ra_update_accept_msg->routing_area_identification[2];
                registered_lai.location_area_code[0] =
                  ra_update_accept_msg->routing_area_identification[3];
                registered_lai.location_area_code[1] =
                  ra_update_accept_msg->routing_area_identification[4];

                mm_remove_forbidden_national_roaming( &registered_lai );
                mm_remove_forbidden_regional_roaming( &registered_lai );

                mm_remove_plmn_from_fplmn_list( gmm_stored_rai.plmn_id);

#ifdef FEATURE_FEMTO_CSG
                mm_add_csg_to_white_list(gmm_stored_rai.plmn_id, &mm_serving_plmn.info.csg_info);
                mm_remove_csg_from_rejected_list(gmm_stored_rai.plmn_id,
                                                 mm_serving_plmn.info.csg_info.csg_id);
#endif

#ifdef FEATURE_LTE
#ifdef FEATURE_DUAL_SIM
                if(reg_sim_plmn_gprs_forbidden(gmm_stored_rai.plmn_id,(sys_modem_as_id_e_type)mm_sub_id) &&
#else
                if(reg_sim_plmn_gprs_forbidden(gmm_stored_rai.plmn_id) &&
#endif
                   (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS)))
                {
                  send_lte_reselection_ind = TRUE;
                }
#endif

                mm_remove_plmn_from_gprs_fplmn_list( gmm_stored_rai.plmn_id);

#ifdef FEATURE_LTE
                if(send_lte_reselection_ind)
                {
                   if (is_umts_mode())
                   {
                     mm_send_rrc_lte_reselection_status_ind();
                   }
                   else if (is_gsm_mode())
                   {
                     mm_send_rr_lte_reselection_status_ind();
                   } 
                }
#endif
                
                if ( ra_update_accept_msg->list_of_receive_npdu_numbers_present )
                {

                  for (i=0; i < GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS; i++)
                  {
                    npdu_number_list.nsapi_valid_list[i] =
                      ra_update_accept_msg->nsapi_valid_list[i];
                    npdu_number_list.npdu_number_list[i] =
                      ra_update_accept_msg->npdu_number_list[i];
                  }

#ifdef FEATURE_GSM_GPRS
                  gmmsm_process_receive_npdu_number_list(
                    npdu_number_list.nsapi_valid_list, npdu_number_list.npdu_number_list
#ifdef FEATURE_DUAL_SIM
                    ,(sys_modem_as_id_e_type)mm_sub_id
#endif
                    );
#endif
                }

               /*
            ** TS 24.008 4.7.5.1.3 V10.7.0 Extended Routing Area Update Timer
            ** If the ROUTING AREA UPDATE ACCEPT message contains T3312 extended value IE, 
            ** then the MS shall use the T3312 extended value IE as periodic routing area update timer (T3312). 
            ** If the ROUTING AREA UPDATE ACCEPT message does not contain T3312 extended value IE, 
            ** then the MS shall use value in T3312 value IE as periodic routing area update timer (T3312).
            */
#ifdef FEATURE_NAS_REL10
                if (ra_update_accept_msg->ext_t3312_value_present)
                {
                        /* ---------------------
                     ** Set T3312 extended timer value
                     ** --------------------- */
                  gmm_t3312_timer_value = gmm_convert_timer3_value_ie_to_msecs(
                                               ra_update_accept_msg->ext_t3312_value );
                }
                else
#endif
                {
                /* ----------------------------
                ** Set Periodic RAU timer value
                ** ---------------------------- */
                gmm_t3312_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                            ra_update_accept_msg->periodic_ra_update_timer );
                }

                if( (is_umts_mode()) &&
                      (gmm_integrity_protection_activated == FALSE) &&
                      ((gmm_t3312_timer_value > t3312_timer_value) ||
                       (!gmm_t3312_timer_value)))
                {
                  gmm_t3312_timer_value = t3312_timer_value;
                }
#ifdef FEATURE_DUAL_SIM
                if(MM_MODEM_STATISTICS_SUB1)
#endif
                {
                if (t3312_timer_value != gmm_t3312_timer_value )
                {
                  mm_set_signal_for_cm(SYS_NAS_PERIODIC_TIMER_MASK);
                }
                }

                if (is_umts_mode() &&
                    (pmm_mode == PMM_IDLE) &&
                    (gmm_ra_update_type == GMM_PERIODIC_UPDATING))
                {
                  mm_stop_timer(TIMER_T3312);
                  mm_start_timer(TIMER_T3312, gmm_t3312_timer_value); 
                }

                if (ra_update_accept_msg->t3302_value_present)
                {
                   /*Spec_ref:9.4.15.7
                    In Iu mode, the network shall not include this IE if this message is to be sent non-integrity protected.
                    In Iu mode, if this message is received without integrity protection the MS shall ignore the contents
                    of this IE and use the last received value if available. If there is no last received value, the MS shall use the default value.*/
                  if ((is_umts_mode ()) &&
                   (gmm_integrity_protection_activated == FALSE))
                  {
#ifndef FEATURE_NAS_REL11
                      MSG_HIGH_DS_0(MM_SUB,"Ignoring the timer T3302 value");
#else
                    /*24.008: 4.7.2.7
                    The default value of this timer T3302 is used if In Iu mode, if the network provides a value in a non-integrity protected Iu mode GMM message
                               and the MS is not attaching for emergency services or not attached for emergency services*/
                     gmm_t3302_timer_value = 720000;
#endif
                  }
                  else
                  {
                  /* ---------------------
                  ** Set T3302 timer value
                  ** --------------------- */
                    gmm_t3302_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                            ra_update_accept_msg->t3302_value );
                  }
#ifdef FEATURE_NAS_REL11
                  /*Store PLMN-ID for PLMN which provided T3302 value*/
                  gmm_previous_plmn_with_t3302_value.identity[0] = ra_update_accept_msg->routing_area_identification[0];
                  gmm_previous_plmn_with_t3302_value.identity[1] = ra_update_accept_msg->routing_area_identification[1];
                  gmm_previous_plmn_with_t3302_value.identity[2] = ra_update_accept_msg->routing_area_identification[2];
#endif
                }
                else
                {
#ifndef FEATURE_NAS_REL11
                 /*24.008: 4.7.2.7
                 The default value of this timer T3302 is used if ATTACH ACCEPT message,ROUTING AREA UPDATE ACCEPT message, ATTACH REJECT message, or ROUTING AREA UPDATE REJECT message
                       is received without a value specified*/
                  if (is_umts_mode()    &&
                      (gmm_integrity_protection_activated == FALSE))
                  {
                    MSG_HIGH_DS_0(MM_SUB,"Ignoring the timer T3302 value");
                  }
                  else
#endif
                  {
                    gmm_t3302_timer_value = 720000;
                  }
                }
                if (ra_update_accept_msg->t3319_value_present)
                {
                  /* ---------------------
                  ** Set T3319 timer value
                  ** --------------------- */
                  gmm_t3319_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                          ra_update_accept_msg->t3319_value );
                } 
                if ((!ra_update_accept_msg->t3319_value_present ) ||(!gmm_t3319_timer_value)) 
                {
                  gmm_t3319_timer_value = 30000;
                }

#if defined(FEATURE_LTE)
                if (ra_update_accept_msg->t3323_value_present)
                {
                  /* ---------------------
                  ** Set T3323 timer value
                  ** --------------------- */
                  gmm_t3323_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                            ra_update_accept_msg->t3323_value );
                }
                /* ------------------------------------------------
                ** If network didnot provide the T3323 value then it should be
                ** set to the same value as T3312.
                ** --------------------- --------------------------*/
                else
                {
                  gmm_t3323_timer_value = gmm_t3312_timer_value;
                }
#endif

                if ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
                    (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)
#ifdef FEATURE_NAS_REL11
                     ||((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL11)&& 
                        (gmm_ra_update_type == GMM_PERIODIC_UPDATING) && (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
#endif
                   )
                {
                  if (ra_update_accept_msg->update_result == GMM_COMBINED_RA_LA_UPDATED 
#if defined(FEATURE_LTE)
                      || ra_update_accept_msg->update_result == GMM_COMBINED_RA_LA_UPDATED_AND_ISR_ACTIVATED
#endif 
                     )
                  {
                    mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_ACCEPT,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                    mm_pending_lu_request = NO_LU;                  
                    mm_lu_start_reason = NO_LU;
                    gmm_irat_lu_pending=NO_LU;
                    mm_substate_control(MM_STD_NORMAL_SERVICE);
                    gmm_rau_attempt_counter = 0;
                    MSG_HIGH_DS_1( MM_SUB, "=MM= RAU Attempt counter = %d", gmm_rau_attempt_counter );
                    t3330_timeout_counter   = 0;
                    MSG_HIGH_DS_1( MM_SUB, "=MM= T3330 timeout counter = %d", t3330_timeout_counter );

                    if (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY)
                    {
                      need_to_send_service_ind = TRUE;
                    }

                    gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     SYS_SRV_DOMAIN_CS_PS,
                                                     FALSE,
                                                     SYS_SRV_STATUS_SRV,
                                                     TRUE );
#ifdef FEATURE_GSTK          
                    mm_send_gstk_mm_idle_info();
#endif                      

                    /* --------------------------------------------------------------
                    ** The MS, receiving a ROUTING AREA UPDATE ACCEPT message, stores
                    ** the received location area identification, stops timer T3330,
                    ** resets the location update attempt counter and sets the
                    ** update status to U1 UPDATED.
                    ** -------------------------------------------------------------- */
                    mm_serving_lai[0] = mm_location_information.lai[0] =
                      ra_update_accept_msg->routing_area_identification[0];
                    mm_serving_lai[1] = mm_location_information.lai[1] =
                      ra_update_accept_msg->routing_area_identification[1];
                    mm_serving_lai[2] = mm_location_information.lai[2] =
                      ra_update_accept_msg->routing_area_identification[2];
                    mm_serving_lai[3] = mm_location_information.lai[3] =
                      ra_update_accept_msg->routing_area_identification[3];
                    mm_serving_lai[4] = mm_location_information.lai[4] =
                      ra_update_accept_msg->routing_area_identification[4];

                    mm_clear_lu_attempt_counter();

                    mm_location_information.location_update_status = UPDATED;

                    mm_substate_control( MM_STD_NORMAL_SERVICE );

                    /* Combined RAU accept, reset both manual mode user selection flags */
                    gmm_manual_mode_user_selection_procedure = FALSE;
                    mm_manual_mode_user_selection_procedure = FALSE;
#ifdef FEATURE_LTE
                    mm_set_emm_manual_selection_flag(FALSE);
#endif

                    /* --------------------------------------------------------------
                    ** If the ROUTING AREA UPDATE ACCEPT message contains an IMSI,
                    ** the mobile station is not allocated any TMSI, and shall delete
                    ** any TMSI accordingly.  If the message contains a TMSI, the MS
                    ** shall use this TMSI as the new temporary identity.  The MS
                    ** shall delete its old TMSI and shall store the new TMSI.  In
                    ** this case, a ROUTING AREA UPDATE COMPLETE message is returned
                    ** to the network.  If neither a TMSI nor an IMSI has been
                    ** included by the network in the ROUTING AREA UPDATE ACCEPT
                    ** message, the old TMSI, if any available, shall be kept.
                    ** ------------------------------------------------------------- */
                    if (ra_update_accept_msg->ms_identity_present)
                    {
                      if ((ra_update_accept_msg->ms_identity.id_type & 0x7) == 0x1)
                      {
                        (void)memset( mm_location_information.tmsi, 0xFF, TMSI_SIZE );
                      }
                      else if ((ra_update_accept_msg->ms_identity.id_type & 0x7) == 0x4)
                      {
                        mm_location_information.tmsi[0] =
                          ra_update_accept_msg->ms_identity.value[0];
                        mm_location_information.tmsi[1] =
                          ra_update_accept_msg->ms_identity.value[1];
                        mm_location_information.tmsi[2] =
                          ra_update_accept_msg->ms_identity.value[2];
                        mm_location_information.tmsi[3] =
                          ra_update_accept_msg->ms_identity.value[3];

                        allocated_tmsi_present = TRUE;
                        gmm_is_tmsi_present_in_prev_gmm_msg = TRUE;
                      }
                    }

                    /* -----------------------------------------------------------
                    ** Any timer used for triggering the location update procedure
                    ** (e.g., T3211, T3212) shall be stopped if running.
                    ** ----------------------------------------------------------- */
                    mm_stop_timer( TIMER_T3211 );
                    mm_stop_timer( TIMER_T3212 );
                    mm_stop_timer( TIMER_T3213 );
#ifdef FEATURE_DUAL_SIM
                    mm_stop_timer( TIMER_MM_RADIO_RETRY );
#endif
                    mm_idle_transition_timer = NULL;

                    /* --------------------------------
                    ** Copy location information to SIM
                    ** -------------------------------- */
                    mmsim_write_loci();
                    mm_possible_imsi_attach = FALSE;

                    if (mm_managed_roaming_enabled)
                    {
                      reg_sim_clear_low_priority_plmn_list(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                                           (sys_modem_as_id_e_type)mm_as_id
#endif
                                                          );
                      mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_SRV;
                    }
#ifdef FEATURE_ENHANCED_NW_SELECTION
                    if (reg_sim_read_ens_flag())
                    {
                      mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_SRV;
                    }
#endif

                    /* --------------------------------------------------
                    ** The MM state will transition back to IDLE when the
                    ** RR(C) connection is released
                    ** -------------------------------------------------- */

                    if (!gmm_detach_after_rau_pending)
                    {
                      if (mm_reg_waiting_for_reg_cnf)
                      {
                        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                        mm_send_mmr_reg_cnf();
                        mm_reg_waiting_for_reg_cnf = FALSE;
                        mm_ready_to_send_reg_cnf   = FALSE;
                      }
                      else
                      {
                        mmr_service_ind.cause         = MMR_CAUSE_NOT_SPECIFIED;
                        mmr_service_ind.service_state = mm_serving_plmn.info;
                        mm_send_mmr_service_ind( &mmr_service_ind);
                        need_to_send_service_ind = FALSE;
                      }
                    }
                  }
                  else if (ra_update_accept_msg->update_result == GMM_RA_UPDATED
#if defined(FEATURE_LTE)
                              || ra_update_accept_msg->update_result == GMM_RA_UPDATED_AND_ISR_ACTIVATED
#endif 
                             )
                  {
                    mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_ACCEPT_PS_ONLY,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                    if (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)
                    {
                      need_to_send_service_ind = TRUE;
                    }
                    mm_gs_association_established = FALSE; 
                    
                    if (ra_update_accept_msg->gmm_cause_present)
                    {
                      switch (ra_update_accept_msg->gmm_cause)
                      {
                        case IMSI_UNKNOWN_IN_HLR:
                          gmm_rau_attempt_counter = 0;
                          MSG_HIGH_DS_1(MM_SUB, "=MM= RAU Attempt counter = %d", gmm_rau_attempt_counter);
                          t3330_timeout_counter   = 0;
                          MSG_HIGH_DS_1( MM_SUB, "=MM= T3330 timeout counter = %d", t3330_timeout_counter );

                          /* ----------------------------------------------------
                          ** The MS shall set the update status to U3 ROAMING NOT
                          ** ALLOWED and shall delete any TMSI, LAI and ciphering
                          ** key sequence number.  The SIM shall be considered as
                          ** invalid for non-GPRS services until switching off or
                          ** the SIM is removed.
                          ** ---------------------------------------------------- */
                          mm_delete_lu_status( ROAMING_NOT_ALLOWED );

                          mm_manual_mode_user_selection_procedure = FALSE;

                               /* -----------------------------------------------------------
                           ** Any timer used for triggering the location update procedure
                           ** (e.g., T3211, T3212) shall be stopped if running.
                           ** ----------------------------------------------------------- */
                          mm_stop_timer( TIMER_T3211 );
                          mm_stop_timer( TIMER_T3212 );
                          mm_stop_timer( TIMER_T3213 );

                          gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_INVALID,
                                                         SYS_SRV_DOMAIN_PS_ONLY,
                                                         FALSE,
                                                         SYS_SRV_STATUS_SRV,
                                                         TRUE );

                          mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_ONLY,
                                             ra_update_accept_msg->gmm_cause,
                                             mm_serving_plmn.info.plmn,
                                             mm_serving_plmn.info.lac
                              #ifdef FEATURE_FEMTO_CSG
                                             , mm_serving_plmn.info.csg_info.csg_id
                              #endif 
                                             , mm_serving_plmn.info.active_rat
                                                   );

                          /* --------------------------------------------------
                          ** The MM state will transition back to IDLE when the
                          ** RR(C) connection is released
                          ** -------------------------------------------------- */
                          break;

#ifndef FEATURE_NAS_REL11
                        case MSC_TEMPORARILY_NOT_REACHABLE:
                        case NETWORK_FAILURE:
                        case CONGESTTION:
#else
/*24.008: 4.7.5.2.3.2
Other GMM cause values and the case that no GMM cause IE was received are considered as abnormal cases. 
The combined attach procedure shall be considered as failed for non-GPRS services. 
The behaviour of the MS in those cases is specified in subclause 4.7.5.2.5.*/
                        default:
#endif
                          /* --------------------------------------------------------
                          ** The MS shall change to state GMM-REGISTERED.ATTEMPTING-
                          ** TO-UPDATE-MM.  The routing area updating attempt counter
                          ** shall be incremented.
                          ** -------------------------------------------------------- */
                          gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE_MM );
                          gmm_previous_T3311_gmm_substate = gmm_substate; 
                          
                          gmm_rau_attempt_counter++;
                          t3330_timeout_counter   = 0;
                          MSG_HIGH_DS_1( MM_SUB, "=MM= T3330 timeout counter = %d", t3330_timeout_counter );
#ifdef FEATURE_NAS_REL11
                          /*24.008: 4.7.5.2.3.2
                          # 22  (Congestion);
                          The MS shall change to state GMM-REGISTERED.ATTEMPTING-TO-UPDATE-MM.The timer T3310 shall be stopped if still running. 
                          The MS shall set the routing area updating attempt counter to 5 and shall start timer T3302.
                          */
                          if(ra_update_accept_msg->gmm_cause == CONGESTTION)
                          {
                            if(mm_timer_status[TIMER_T3310] == TIMER_ACTIVE)
                            {
                              mm_stop_timer(TIMER_T3310);
                            }
                            gmm_rau_attempt_counter = 5;
                            mm_start_timer( TIMER_T3302, gmm_t3302_timer_value );
                          }
#endif
                          MSG_HIGH_DS_1( MM_SUB, "=MM= RAU Attempt counter = %d", gmm_rau_attempt_counter );
                          if(mm_managed_roaming_enabled && (ra_update_accept_msg->gmm_cause == NETWORK_FAILURE))
                          {
                            reg_sim_add_to_low_priority_plmn_list(mm_serving_plmn.info.plmn, mm_serving_plmn.info.active_rat
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                                                  ,(sys_modem_as_id_e_type)mm_as_id
#endif
                                                                 );

                          }

                 
                          mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_ONLY,
                                        ra_update_accept_msg->gmm_cause,
                                        mm_serving_plmn.info.plmn,
                                        mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                        , mm_serving_plmn.info.csg_info.csg_id
#endif
                                        , mm_serving_plmn.info.active_rat
                                         );

                          /* If in case the RA accept contains different RA than the one received in 
                               Service IND, the MM gives more priority to the one received in ACCEPT.
                               the RAI check and the updates status check are always TRUE
                            */
                          if ((gmm_rau_attempt_counter < 5) &&
                              (!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                                   gmm_stored_rai.routing_area_code,
                                                   mm_serving_plmn.lac,
                                                   mm_serving_plmn.rac )) ||
                                 (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
                              (gmm_update_status == GMM_GU1_UPDATED))
                          {
                            /* ---------------------------------------------------------
                            ** The MS shall keep the GMM update status to GU1 UPDATED.
                            ** The MS shall start timer T3311.  When timer T3311 expires
                            ** the combined routing area updating procedure indicating
                            ** "combined RA/LA updating with IMSI attach" is triggered.
                            ** --------------------------------------------------------- */
                            mm_start_timer( TIMER_T3311, DEFAULT_TIMEOUT );
                            gmm_ra_update_type =
                              GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
                          }
                          else if((gmm_rau_attempt_counter >= 5)
#ifdef FEATURE_NAS_REL11
                                   &&(ra_update_accept_msg->gmm_cause != CONGESTTION)
#endif
                                 )
                          {                         
                            mm_start_timer( TIMER_T3302, gmm_t3302_timer_value );
                            gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;

                            /* -----------------------------------------------------
                            ** A GPRS MS operating in MS operation mode A shall then
                            ** proceed with appropriate MM specific procedure. It can be either IMSI 
                            ** attach or nothing.
                            ** ----------------------------------------------------- */
                            if ((mm_state == MM_IDLE) &&
                                ((mm_idle_substate == MM_NORMAL_SERVICE) ||
                                 (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE)||
                                 (mm_idle_substate == MM_LIMITED_SERVICE)||
                                 (mm_idle_substate == MM_LOCATION_UPDATE_NEEDED)))
                            {
                              mm_substate_control(MM_STD_NORMAL_SERVICE);
                              if ((mm_location_information.location_update_status == UPDATED) &&
                                  (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                                  (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ))
                              {
                                if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                                {
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                                  if (is_gsm_mode()&&
                                      ((gmm_is_ptmsi_present_in_prev_gmm_msg) || (gmm_is_tmsi_present_in_prev_gmm_msg)||
                                       (ra_update_accept_msg->list_of_receive_npdu_numbers_present)))
                                  {
                                    mm_pending_lu_type = MM_LU_CAUSE_IMSI_ATTACH;
                                  }
                                  else
#endif
                                  {
                                    mm_initiate_lu( MM_LU_CAUSE_IMSI_ATTACH );
                                  }
                                }
                                else
                                {
#ifdef FEATURE_NAS_REL10
                                  if(mm_is_per_ms_t3212_present)
                                  {
                                    if ((mm_stored_per_ms_t3212_value != 0 ) &&
                                         (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                                    {

                                      mm_start_timer( TIMER_T3212,
                                                      mm_stored_per_ms_t3212_value);
                                    }
                                  }
                                  else
#endif
                                  if ((mm_system_information.T3212_timer_value != 0) &&
                                       (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                                  {
                                    mm_start_timer( TIMER_T3212,
                                                mm_system_information.T3212_timer_value );
                                  }                               
                                }
                              }
                              else
                              {
                                mm_substate_control(MM_STD_UPDATE_ATTEMPTED);
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                                if (is_gsm_mode() &&
                                    ((gmm_is_ptmsi_present_in_prev_gmm_msg) || (gmm_is_tmsi_present_in_prev_gmm_msg)||
                                     (ra_update_accept_msg->list_of_receive_npdu_numbers_present)))

                                {
                                  mm_pending_lu_type = MM_LU_CAUSE_NORMAL;
                                }
                                else
#endif
                                {
                                  mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                                }
                              }
                            }
                            else
                            {
                              if ((mm_location_information.location_update_status == UPDATED) &&
                                  (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                                  (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ))
                              {                            
                                if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                                {
                                  mm_pending_lu_request = IMSI_ATTACH; 
                                }
                              }
                              else
                              {
                                mm_pending_lu_request = NORMAL_LU;                               
                              }
                            }
                          }

                          if ((mm_location_information.location_update_status == UPDATED) &&
                              (memcmp( mm_location_information.lai,
                                       mm_serving_lai, LAI_SIZE ) == 0))
                          {
                            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                             SYS_SRV_DOMAIN_CS_PS,
                                                             FALSE,
                                                             SYS_SRV_STATUS_SRV,
                                                             TRUE );
                          }
                          else
                          {
                            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                             SYS_SRV_DOMAIN_PS_ONLY,
                                                             FALSE,
                                                             SYS_SRV_STATUS_SRV,
                                                             TRUE );
                          }

#ifndef FEATURE_NAS_REL11
                          break;

                        default:
                          /* ------------------------------------------------------------
                          ** Other values are considered as abnormal cases.  The combined
                          ** routing area updating shall be considered as failed for GPRS
                          ** and non-GPRS procedures.  The behavior of the MS in those
                          ** cases is specified in clause 4.7.5.2.5.
                          ** ------------------------------------------------------------ */
                          gmm_increment_routing_area_update_attempt_counter
                          (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                            ra_update_accept_msg->list_of_receive_npdu_numbers_present
#endif
                          );

                          /* As already the status is informed to REG through the above function */
                          need_to_send_service_ind = FALSE;

                          gmm_ra_update_type =
                            GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
#endif
                          break;
                      } /* end switch (ra_update_accept_msg.gmm_cause) */
                    }/*end if GMM Cause Present*/

                    else
                    {
/*24.008: The behaviour of the MS in those cases is specified in subclause 4.7.5.2.5.*/
#ifdef FEATURE_NAS_REL11
                         /* --------------------------------------------------------
                          ** The MS shall change to state GMM-REGISTERED.ATTEMPTING-
                          ** TO-UPDATE-MM.  The routing area updating attempt counter
                          ** shall be incremented.
                          ** -------------------------------------------------------- */
                          gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE_MM );
                          gmm_previous_T3311_gmm_substate = gmm_substate; 
                          
                          gmm_rau_attempt_counter++;
                          MSG_HIGH_DS_1( MM_SUB, "=MM= RAU Attempt counter = %d", gmm_rau_attempt_counter );
                          t3330_timeout_counter   = 0;
                          MSG_HIGH_DS_1( MM_SUB, "=MM= T3330 timeout counter = %d", t3330_timeout_counter );
                
                          mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_ONLY,
                                        ra_update_accept_msg->gmm_cause,
                                        mm_serving_plmn.info.plmn,
                                        mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                        , mm_serving_plmn.info.csg_info.csg_id
#endif
                                        , mm_serving_plmn.info.active_rat
                                         );

                          /* If in case the RA accept contains different RA than the one received in 
                               Service IND, the MM gives more priority to the one received in ACCEPT.
                               the RAI check and the updates status check are always TRUE
                            */
                          if ((gmm_rau_attempt_counter < 5) &&
                              (!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                                   gmm_stored_rai.routing_area_code,
                                                   mm_serving_plmn.lac,
                                                   mm_serving_plmn.rac )) ||
                                 (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
                              (gmm_update_status == GMM_GU1_UPDATED))
                          {
                            /* ---------------------------------------------------------
                            ** The MS shall keep the GMM update status to GU1 UPDATED.
                            ** The MS shall start timer T3311.  When timer T3311 expires
                            ** the combined routing area updating procedure indicating
                            ** "combined RA/LA updating with IMSI attach" is triggered.
                            ** --------------------------------------------------------- */
                            mm_start_timer( TIMER_T3311, DEFAULT_TIMEOUT );
                            gmm_ra_update_type =
                              GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
                          }
                          else if (gmm_rau_attempt_counter >= 5)
                          {                         
                            mm_start_timer( TIMER_T3302, gmm_t3302_timer_value );
                            gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;

                            /* -----------------------------------------------------
                            ** A GPRS MS operating in MS operation mode A shall then
                            ** proceed with appropriate MM specific procedure. It can be either IMSI 
                            ** attach or nothing.
                            ** ----------------------------------------------------- */
                            if ((mm_state == MM_IDLE) &&
                                ((mm_idle_substate == MM_NORMAL_SERVICE) ||
                                 (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE)||
                                 (mm_idle_substate == MM_LIMITED_SERVICE)||
                                 (mm_idle_substate == MM_LOCATION_UPDATE_NEEDED)))
                            {
                              mm_substate_control(MM_STD_NORMAL_SERVICE);
                              if ((mm_location_information.location_update_status == UPDATED) &&
                                  (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                                  (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ))
                              {
                                if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                                {
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                                  if (is_gsm_mode()&&
                                      ((gmm_is_ptmsi_present_in_prev_gmm_msg) || (gmm_is_tmsi_present_in_prev_gmm_msg)||
                                       (ra_update_accept_msg->list_of_receive_npdu_numbers_present)))
                                  {
                                    mm_pending_lu_type = MM_LU_CAUSE_IMSI_ATTACH;
                                  }
                                  else
#endif
                                  {
                                    mm_initiate_lu( MM_LU_CAUSE_IMSI_ATTACH );
                                  }
                                }
                                else
                                {
#ifdef FEATURE_NAS_REL10
                                  if(mm_is_per_ms_t3212_present)
                                  {
                                    if ((mm_stored_per_ms_t3212_value != 0 ) &&
                                         (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                                    {

                                      mm_start_timer( TIMER_T3212,
                                                      mm_stored_per_ms_t3212_value);
                                    }
                                  }
                                  else
#endif
                                  if ((mm_system_information.T3212_timer_value != 0) &&
                                       (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                                  {
                                    mm_start_timer( TIMER_T3212,
                                                mm_system_information.T3212_timer_value );
                                  }                               
                                }
                              }
                              else
                              {
                                mm_substate_control(MM_STD_UPDATE_ATTEMPTED);
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                                if (is_gsm_mode() &&
                                    ((gmm_is_ptmsi_present_in_prev_gmm_msg) || (gmm_is_tmsi_present_in_prev_gmm_msg)||
                                     (ra_update_accept_msg->list_of_receive_npdu_numbers_present)))

                                {
                                  mm_pending_lu_type = MM_LU_CAUSE_NORMAL;
                                }
                                else
#endif
                                {
                                  mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                                }
                              }
                            }
                            else
                            {
                              if ((mm_location_information.location_update_status == UPDATED) &&
                                  (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                                  (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ))
                              {                            
                                if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                                {
                                  mm_pending_lu_request = IMSI_ATTACH; 
                                }
                              }
                              else
                              {
                                mm_pending_lu_request = NORMAL_LU;                               
                              }
                            }
                          }

                          if ((mm_location_information.location_update_status == UPDATED) &&
                              (memcmp( mm_location_information.lai,
                                       mm_serving_lai, LAI_SIZE ) == 0))
                          {
                            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                             SYS_SRV_DOMAIN_CS_PS,
                                                             FALSE,
                                                             SYS_SRV_STATUS_SRV,
                                                             TRUE );
                          }
                          else
                          {
                            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                             SYS_SRV_DOMAIN_PS_ONLY,
                                                             FALSE,
                                                             SYS_SRV_STATUS_SRV,
                                                             TRUE );
                          }
#else
                      /* ------------------------------------------------------------
                      ** Other reject cause values and the case that no GMM cause IE was received
                      ** are considered as abnormal cases. The combined routing area updating shall
                      ** be considered as failed for GPRS and non-GPRS services. The specification of the
                      ** MS behaviour in those cases is specified in subclause 4.7.5.2.5
                      ** ------------------------------------------------------------ */
                      gmm_increment_routing_area_update_attempt_counter
                      (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                        ra_update_accept_msg->list_of_receive_npdu_numbers_present
#endif
                      );

                      /* As already the status is informed to REG through the above function */
                      need_to_send_service_ind = FALSE;

                      gmm_ra_update_type =
                          GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
#endif
                    }

#ifdef FEATURE_GSTK          
                    mm_send_gstk_mm_idle_info();
#endif                      
                    if(!gmm_detach_after_rau_pending)
                    {
                      if (mm_reg_waiting_for_reg_cnf)
                      {
                        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                        mm_send_mmr_reg_cnf();
                        mm_reg_waiting_for_reg_cnf = FALSE;
                        mm_ready_to_send_reg_cnf   = FALSE;
                      }
                      else if (need_to_send_service_ind)
                      {
                        mmr_service_ind.cause         = MMR_CAUSE_NOT_SPECIFIED;
                        mmr_service_ind.service_state = mm_serving_plmn.info;
                        mm_send_mmr_service_ind( &mmr_service_ind);
                        need_to_send_service_ind = FALSE;
                      }
                    }
                  }
                }
                else
                {
                  mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_ACCEPT,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                  gmm_rau_attempt_counter = 0;
                  MSG_HIGH_DS_1( MM_SUB, "=MM= RAU Attempt counter = %d", gmm_rau_attempt_counter );
                  t3330_timeout_counter   = 0;
                  MSG_HIGH_DS_1( MM_SUB, "=MM= T3330 timeout counter = %d", t3330_timeout_counter );

                  gmm_manual_mode_user_selection_procedure = FALSE;
#ifdef FEATURE_LTE
                    mm_set_emm_manual_selection_flag(FALSE);
#endif
#ifdef FEATURE_ENHANCED_NW_SELECTION
                  gmm_invalidate_foreground_search_rai((byte *)(&gmm_current_lai),gmm_current_rac);
#endif

                  if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                      (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                      ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                       (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)))
                  {
                    gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     SYS_SRV_DOMAIN_CS_PS,
                                                     FALSE,
                                                     SYS_SRV_STATUS_SRV,
                                                     TRUE );
                  }
                  else
                  {
                    gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     SYS_SRV_DOMAIN_PS_ONLY,
                                                     FALSE,
                                                     SYS_SRV_STATUS_SRV,
                                                     TRUE );
                  }
                  
                  if(!gmm_detach_after_rau_pending)
                  {
                    if (mm_reg_waiting_for_reg_cnf)
                    {
                      mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
   					  mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                      mm_send_mmr_reg_cnf();
                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;
                    }
                    else
                    {
                      mmr_service_ind.cause         = MMR_CAUSE_NOT_SPECIFIED;
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mm_send_mmr_service_ind( &mmr_service_ind);
                      need_to_send_service_ind = FALSE;
                    }
                  }

                 }

                if(ra_update_accept_msg->emergency_number_list_present)
                {
                  mm_update_mmr_emergency_num_list_if_needed(&(ra_update_accept_msg->emergency_number_list));
                }
                else
                {
                  mm_nullify_mmr_emergency_num_list_if_needed
                    (
                          &(previous_plmn_identity[0]),
                          &(gmm_current_lai.PLMN_id.identity[0])
                    );
                }

#ifdef FEATURE_MBMS
                #error code not present
#endif
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
               emm_ctrl_data_ptr->mmit_parameters.utran_ps_voice_parameters_ptr->is_last_utran_voims_supported = FALSE;
               if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9) &&
                  ((ra_update_accept_msg->nw_ftr_support & GMM_VOIMS_SUPPORTED_IN_UMTS)
                                                            == GMM_VOIMS_SUPPORTED_IN_UMTS))
               {
                 emm_ctrl_data_ptr->mmit_parameters.utran_ps_voice_parameters_ptr->is_last_utran_voims_supported =
                                                                               TRUE;
               }
#endif
                /***********************************************************************
                ** If network does not indicate the PDP STATUS IE then we need
        ** to deactivate the non transferrable contexts on receiving RAU
        ** accept. So encoding the PDP STATUS IE in this case.  
                *************************************************************************/         
                if (ra_update_accept_msg->pdp_context_status_present == FALSE)
                {
                  ra_update_accept_msg->pdp_context_status.length= GMM_PDP_CONTEXT_STATUS_LENGTH;
                  sm_pack_pdp_context_status (ra_update_accept_msg->pdp_context_status.value, (sys_modem_as_id_e_type)mm_sub_id);
                  ra_update_accept_msg->pdp_context_status_present = TRUE;
                }

                gmm_send_sm_pdp_mbms_status_ind_if_needed
                (
                   &(ra_update_accept_msg->pdp_context_status),
                   ra_update_accept_msg->pdp_context_status_present,
                   &(ra_update_accept_msg->mbms_context_status),
                   ra_update_accept_msg->mbms_context_status_present
                );
#ifdef FEATURE_DUAL_SIM                
                if ((cm_get_operator_info((sys_modem_as_id_e_type)mm_sub_id) == OPERATOR_CMCC) || (cm_get_operator_info((sys_modem_as_id_e_type)mm_sub_id) == OPERATOR_CU) || (mm_is_qrd_device) || (mm_optimised_ps_signalling_connection_release))
#else
                if ((cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CMCC) || (cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CU) || (mm_is_qrd_device) || (mm_optimised_ps_signalling_connection_release))
#endif
                {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS) 
                  is_data_available =  dsumtsps_is_ps_data_available();
#endif
                }
#ifdef FEATURE_REL6_PS_FOP
                if (is_umts_mode() && 
                     ( (gmm_ps_fop_req &&  ((gmm_service_request_initiator == MS_CC_MM) || (!data_is_available))) ||
#ifdef FEATURE_DUAL_SIM
                        ( sm_is_pdp_activation_pending((sys_modem_as_id_e_type)mm_sub_id))
#else
                        ( sm_is_pdp_activation_pending())
#endif
                      ))
                {
                  /* process the PS FoP if it is granted by nw */
                  if ( ra_update_accept_msg->follow_on_proceed_response == 0 )
                  {
                    if(gmm_service_request_initiator == MS_CC_MM)
                    {
                      /* Send MMCNM_EST_CNF to CNM */
                      mm_confirm_mm_connection(MS_CC_MM_PS) ;
                      gmm_service_request_initiator = NULL;
                      /* resetting the penging msg in order to drop service request message
                         which is as per spec 24.008 section 4.7.13 */
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)                         
                      gmm_rat_chng_pended_msg_valid = FALSE ;
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
                    }
                    else
                    {
                      /* process the PS FoP if it is granted by nw */
                      sm_cmd = sm_get_cmd_buf();

                      if (sm_cmd == NULL)
                      {
                        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                      }

                      sm_cmd->header.message_set = MS_GMM_SM;
                      sm_cmd->header.cmd_id      = GMMSM_PS_FOP_IND;
#ifdef FEATURE_DUAL_SIM
                      sm_cmd->cmd.gmm_ps_fop_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_PS_FOP_GRANTED by NW" );
                      sm_put_cmd( sm_cmd );
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)                      
                      gmm_rat_chng_pended_msg_valid = FALSE ;
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
                    } 
                    gmm_ps_fop_resp = PS_FOP_NW_GRANTED;
                  }
                  else
                  {
                    if(gmm_ps_fop_req && (gmm_service_request_initiator == MS_CC_MM))
                    {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE, MMCC_SMS_PS);
#else
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif /* FEATURE_SMS_ENHANCED_DOMAIN_SELECTION */
                      gmm_service_request_initiator = NULL;
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
                      gmm_rat_chng_pended_msg_valid = FALSE;
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
                    }
                    else if(gmm_ps_fop_req && (gmm_service_request_initiator == MS_GMM_SM))
                    {
                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ");
                      gmm_send_gmmsm_establish_rej(mm_convert_gmm_to_sm_cause(PROTOCOL_ERROR_UNSPECIFIED));
                      gmm_service_request_initiator = NULL;

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)                      
                      gmm_rat_chng_pended_msg_valid = FALSE;
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
                    }

#ifdef FEATURE_NAS_REL12
                  if(
#ifdef FEATURE_DUAL_SIM
                       rabm_is_rab_active((sys_modem_as_id_e_type)mm_sub_id) == FALSE
#else
                       rabm_is_rab_active() == FALSE
#endif
		    )
#endif
		  {
                    gmm_ps_fop_resp = PS_FOP_NW_NOT_GRANTED;
                    /**************************************************************
                    *24.008 4.7.1.9 The MS shall start the timer T3340  when the  *
                    * network indicates "no follow-on proceed" in the             *
                    *ROUTING AREA UPDATE ACCEPT or ATTACH ACCEPT message.         *
                    **************************************************************/
                    mm_start_timer(TIMER_T3340, DEFAULT_TIMEOUT);
                  } 
                }
                }
                else if (is_umts_mode())
                {
                  if ((ra_update_accept_msg->follow_on_proceed_response == 1 )
#ifdef FEATURE_NAS_REL12
#ifdef FEATURE_DUAL_SIM
                       && (rabm_is_rab_active((sys_modem_as_id_e_type)mm_sub_id) == FALSE)
#else
                       && (rabm_is_rab_active() == FALSE) 
#endif
#endif
                     )
                  {
                    gmm_ps_fop_resp = PS_FOP_NW_NOT_GRANTED;
                    /**************************************************************
                    *24.008 4.7.1.9 The MS shall start the timer T3340  when the  *
                    * network indicates "no follow-on proceed" in the             *
                    *ROUTING AREA UPDATE ACCEPT or ATTACH ACCEPT message.         *
                    **************************************************************/
                    mm_start_timer(TIMER_T3340, DEFAULT_TIMEOUT);
                  }
#ifdef FEATURE_DUAL_SIM
                  else if ((cm_get_operator_info((sys_modem_as_id_e_type)mm_sub_id) == OPERATOR_CMCC) || (cm_get_operator_info((sys_modem_as_id_e_type)mm_sub_id) == OPERATOR_CU) || (mm_is_qrd_device) || (mm_optimised_ps_signalling_connection_release))
#else
                  else if ((cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CMCC) || (cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CU) || (mm_is_qrd_device) || (mm_optimised_ps_signalling_connection_release))
#endif
                  {
                    if (!(gmm_ps_fop_req || is_data_available || gmm_follow_on_request ||
#ifdef FEATURE_DUAL_SIM
                       ( sm_is_pdp_activation_pending((sys_modem_as_id_e_type)mm_sub_id)) ||
                       ( rabm_is_rab_active((sys_modem_as_id_e_type)mm_sub_id))
#else
                       ( sm_is_pdp_activation_pending()) ||
                       ( rabm_is_rab_active())  
#endif
                        ))
                    {
                    /* there is no follow-on request in GPRS RAU REQUEST */
                      mm_start_timer(TIMER_T3340, DEFAULT_TIMEOUT); 
                  }
                }
                }
                else
#endif/* FEATURE_REL6_PS_FOP */
                if(is_gsm_mode())
                {
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_IND");
                  gmm_send_gmmsm_establish_ind(SM_PS_SIG_CONN_AVAIL);
                }

#ifdef FEATURE_GSM_GPRS
                if (is_gsm_mode())
                {
                  gmm_trigger_llc_type gmm_command = GMM_TRIGGER_FORCE_TO_STANDBY;

                  gmm_command |=
                    ra_update_accept_msg->allocated_ptmsi_present?
                      GMM_TRIGGER_PTMSI_ASSIGNED:gmm_command ;

                  if ( ra_update_accept_msg->ready_timer_value_present )
                  {
                    gmm_command |=
                    (
                      (
                        ra_update_accept_msg->list_of_receive_npdu_numbers_present ||
                        allocated_tmsi_present ||
                        ra_update_accept_msg->allocated_ptmsi_present
                      )
                      ?
                      GMM_TRIGGER_NEW_READY_TMR_VALUE_WO_UPDATE:
                      GMM_TRIGGER_NEW_READY_TMR_VALUE
                    );
                  }
                  else
                  {
                    gmm_command |=
                    (
                      (
                        ra_update_accept_msg->list_of_receive_npdu_numbers_present ||
                        allocated_tmsi_present ||
                        ra_update_accept_msg->allocated_ptmsi_present
                      )
                      ?
                      GMM_TRIGGER_NO_READY_TMR_VALUE_WO_UPDATE:
                      GMM_TRIGGER_NO_READY_TMR_VALUE
                    );
                  }

                  gmmllc_gmm_trigger_llc_event(gmm_command,
                      ra_update_accept_msg->force_to_standby,
                             gmm_convert_timer_value_ie_to_usecs(
                                     ra_update_accept_msg->ready_timer_value)) ;
                }

                cell_notification_ind = ra_update_accept_msg->cell_notification_present ;

#endif
                /* ---------------------------------------------------------------
                ** A ROUTING AREA UPDATE COMPLETE message shall be returned to the
                ** network if the ROUTING AREA UPDATE ACCEPT message contained:
                **  - a P-TMSI; and/or
                **  - Receive N-PDU Numbers
                ** --------------------------------------------------------------- */
                if ((ra_update_accept_msg->allocated_ptmsi_present) ||
                    (allocated_tmsi_present) ||
                    (ra_update_accept_msg->list_of_receive_npdu_numbers_present))
                {
                  gmm_send_routing_area_update_complete_message(
                    ra_update_accept_msg->list_of_receive_npdu_numbers_present,
                    &npdu_number_list );
                  
                  if((mm_held_cnm_service_req.present == TRUE) && is_gsm_mode() && mm_state == MM_IDLE)
                  {
                    MSG_HIGH_DS_0( MM_SUB, "=MM= EST_REJ: RR yet to send RAU_COMPLETE" );

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                    mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_PS);
#else
                    mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif
                  }
                  
                }

#ifdef FEATURE_DUAL_DATA
                if (is_gsm_mode())
                {
                   if (!ra_update_accept_msg->allocated_ptmsi_present && !allocated_tmsi_present &&( !ra_update_accept_msg->list_of_receive_npdu_numbers_present))
                   {
                     mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_LOW);
                   }
               }
#endif 
#ifdef FEATURE_DUAL_SIM
                if(MM_MODEM_STATISTICS_SUB1)
#endif
                {
                  if ((ra_update_accept_msg->allocated_ptmsi_present) ||
                    (allocated_tmsi_present))
                {
                   mm_set_signal_for_cm(SYS_NAS_REGISTRATION_INFO_MASK);
                }
                }

                if ((gmm_substate == GMM_UPDATE_NEEDED) || (gmm_substate == GMM_ATTEMPTING_TO_UPDATE) ||
                    ((ra_update_accept_msg->update_result == GMM_COMBINED_RA_LA_UPDATED) &&
                     (gmm_substate == GMM_ATTEMPTING_TO_UPDATE_MM)))
                {
                  gmm_set_substate(GMM_NORMAL_SERVICE);
                }
                
                if(is_gsm_mode()&&
                   (gmm_multi_slot_ra_update_pending == TRUE))
                {
                  gmm_set_state(GMM_REGISTERED);
                  MSG_HIGH_DS_0(MM_SUB,"=MM= Processing the pending combined RAU request ");
                  gmm_update_ra_type();
                  gmm_initiate_routing_area_update();
                }                
                if((gmm_ra_update_pending == TRUE)&&(mm_state == MM_IDLE)&&
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                   (mm_check_if_cs_cell_access_allowed(mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION)))&&
                   (gmm_check_if_ps_cell_access_allowed(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION))))
                {
                  gmm_set_state(GMM_REGISTERED);
#ifdef FEATURE_REL6_PS_FOP
                  if (is_umts_mode() && (ra_update_accept_msg->follow_on_proceed_response == 1))
                  {
                    MSG_HIGH_DS_0(MM_SUB,"=MM= combined RAU will be sent after connection release");
                  }
                  else
#endif
                  {
                    gmm_update_ra_type();
                    MSG_HIGH_DS_0(MM_SUB,"=MM= Processing the pending combined RAU request ");
                    gmm_initiate_routing_area_update();
                  }
                }

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)                
                 else if(gmm_rat_chng_pended_msg_valid)
                 {
#ifdef FEATURE_MODEM_HEAP
                  if(gmm_rat_chng_pended_msg == NULL)
                  {
                    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Null pointer for gmm_rat_chng_pended_msg", 0,0,0 );
                  }
#endif
                  protocol_discriminator = (byte) (((byte *) gmm_rat_chng_pended_msg + sizeof(IMH_T))[0] & 0x0F);
                  message_id = (byte) (((byte *) gmm_rat_chng_pended_msg + sizeof(IMH_T))[1]) ;

                  gmm_rat_chng_pended_msg_valid = FALSE ;  

                  if((protocol_discriminator == PD_GMM) && (message_id == GMM_SERVICE_REQUEST))
                  {
                    if(is_umts_mode())
                    {
                    gmm_initiate_service_request(gmm_rat_chng_pended_msg_cause, 
                                                 mm_get_current_security_context( RRC_PS_DOMAIN_CN_ID),
                                                 NULL
                                                );
                    gmm_set_state( GMM_SERVICE_REQUEST_INITIATED );

                    mm_start_timer( TIMER_T3317, DEFAULT_TIMEOUT );
                  }
                    else
                    {
                       gmm_set_state( GMM_REGISTERED ); 
                       if (gmm_service_request_initiator == MS_GMM_RABM)
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
                         gmm_service_request_initiator = NULL ;
                       }
                       else if (gmm_service_request_initiator == MS_GMM_SM)
                       {
                         MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_CNF");
                         gmm_send_gmmsm_establish_cnf(FALSE);
                         gmm_service_request_initiator = NULL ;
                       } 
                       else if(gmm_service_request_initiator == MS_CC_MM)
                       {
                         /* Send MMCNM_EST_CNF to CNM */
                        mm_confirm_mm_connection(MS_CC_MM_PS) ;
                        gmm_service_request_initiator = NULL;
                       }
                    }
                  }
                  else if((protocol_discriminator == PD_GMM) && (message_id == GMM_DETACH_REQUEST))
                  {
                     gmm_initiate_gprs_detach( &mmcoord_pending_reg_message );
                  }
                  else
                  {
                    mmrrcconn_send_L3_message_to_rrc(
                    gmm_rat_chng_pended_msg,
                    gmm_rat_chng_pended_msg_cause,
                    RRC_PS_DOMAIN_CN_ID );
                    gmm_set_state( GMM_REGISTERED );                   
                  }

                 }
                else if(gmm_detach_after_rau_pending)
                 {/* The (gmm_detach_after_rau_pending) and the above case of
                    (message_id == GMM_DETACH_REQUEST) are for two different scenarios. 
                    one is for the case where RAU has changed during detach procedure and the 
                    other is for the case where RAU is pending and DETACH is initiated */
                                       
                   gmm_initiate_gprs_detach( &mmcoord_pending_reg_message );
                 }
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/                 
                else
                 {
                  gmm_set_state( GMM_REGISTERED );
                 }
              } /* end if (gmm_valid_routing_area_update_accept_message */
              else
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Invalid ROUTING AREA UPDATE ACCEPT" );
              }
#ifndef FEATURE_MODEM_HEAP
              gs_free(ra_update_accept_msg);
#else
              modem_mem_free(ra_update_accept_msg, MODEM_MEM_CLIENT_NAS);
#endif

              break;

            case GMM_ROUTING_AREA_UPDATE_REJECT:
              /* -----------------------------------------------------------
              ** Routing area updating cannot be accepted (24.008 4.7.5.1.4)
              ** ----------------------------------------------------------- */
              if (gmm_valid_routing_area_update_reject_message(
                    message, &ra_update_reject_msg ))
              {
              
              #ifdef FEATURE_GSTK
              // Sending NW REJECT EVENT TO GSTK
                mm_send_gstk_routing_area_update_reject_evt(ra_update_reject_msg.gmm_cause);
              #endif /*FEATURE_GSTK*/

                mm_stop_timer( TIMER_T3330 ); 
                t3330_timeout_counter   = 0;
                MSG_HIGH_DS_1( MM_SUB, "=MM= T3330 timeout counter = %d", t3330_timeout_counter );

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                if (is_gsm_mode())
                {
                  gmm_transaction_id++;

                  MSG_HIGH_DS_0(MM_SUB,"=MM= Transaction id Incremented Since ROUTING AREA UPDATE REJECT is received ");
                }
#endif

#ifdef FEATURE_GPRS_PS_HANDOVER                  
                if (gmm_psho_status == GMM_PSHO_IN_PROGRESS)
                {
                  gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_LLC_SUSPENSION, FALSE, 0);
                }
#endif                

                gmm_extend_connection_due_to_mt_csfb = FALSE;

                if (ra_update_reject_msg.t3302_value_present)
                {
                   /* Spec Ref: 9.4.17.1     
                      In Iu mode, the MS shall ignore the contents of this IE if this message is received without integrity protection.
                      If this IE is not included or if in Iu mode the message is not integrity protected, the MS shall use the default value*/
                  if (is_umts_mode () &&
                        (gmm_integrity_protection_activated == FALSE))
                  {
                    gmm_t3302_timer_value = 720000;
                  }
                  else
                  {
                  /* ---------------------
                  ** Set T3302 timer value
                  ** --------------------- */
                    gmm_t3302_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                            ra_update_reject_msg.t3302_value );
                  }
#ifdef FEATURE_NAS_REL11 
                  /*Store PLMN id on which we got T3302 value*/                 
                  gmm_previous_plmn_with_t3302_value.identity[0] = mm_serving_plmn.info.plmn.identity[0];
                  gmm_previous_plmn_with_t3302_value.identity[1] = mm_serving_plmn.info.plmn.identity[1];
                  gmm_previous_plmn_with_t3302_value.identity[2] = mm_serving_plmn.info.plmn.identity[2];
#endif
                }
                else
                {
                  gmm_t3302_timer_value = 720000;
                }

#ifdef FEATURE_LTE
                /* If LTE is present, update EMM state and parameters.
                   Note: emm_multimode_complete_update must be also called
                   after GMM/MM processing to wrap up EMM update */
                if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
                {
                  emm_set_state_multimode_rau_failure((lte_nas_emm_cause_type)ra_update_reject_msg.gmm_cause);
                }
#endif
                /*----------------------------------------------------------
                ** The Equivalent PLMN List is deleted for all causes except
                ** #12, 14, 15 (24.008 section 4.7.5.1.4, 4.7.5.2.4)
                **----------------------------------------------------------*/
                if ((ra_update_reject_msg.gmm_cause != LA_NOT_ALLOWED) &&
                    (ra_update_reject_msg.gmm_cause != GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN) &&
                    (ra_update_reject_msg.gmm_cause != NO_SUITABLE_CELLS_IN_LA)
#ifdef FEATURE_NAS_REL10
                   && !((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10) &&
                        ra_update_reject_msg.gmm_cause == CONGESTTION)
#endif
#ifdef FEATURE_FEMTO_CSG
                   && !((ra_update_reject_msg.gmm_cause == CSG_NOT_AUTHORIZED) && (mm_serving_csg_id != SYS_CSG_ID_INVALID))
#endif 
                   )
                {
                   mm_serving_plmn.update_equiv_plmn_list = TRUE;
                   memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));
                }

                /* Log the RAU Reject event */
                mmsend_request_reject_event( FALSE,
                                             SMGMM_EVENT_ROUTING_AREA_UPDATE_REJECT,
                                             ra_update_reject_msg.gmm_cause);
               gmm_reject_cause = ra_update_reject_msg.gmm_cause;
#ifdef FEATURE_DUAL_SIM
                if(MM_MODEM_STATISTICS_SUB1)
#endif
                {
                mm_set_signal_for_cm(SYS_NAS_PS_REJECT_CAUSE_MASK);
                }
                mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_REJECTED,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);

#ifdef FEATURE_NAS_REL10
                if(ra_update_reject_msg.t3346_value_present && ra_update_reject_msg.gmm_cause == CONGESTTION)
                {
                  gmm_t3346_backoff_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                                                           ra_update_reject_msg.t3346_value );
                }
#endif

#ifdef FEATURE_GSTK
                prev_service_status = mm_serving_plmn.info.service_status;
#endif

                switch (ra_update_reject_msg.gmm_cause) 
                {
                  case ILLEGAL_MS:
                  case ILLEGAL_ME:
                  case GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
                    /* ------------------------------------------------------------
                    ** The MS shall set the GPRS update status to GU3 ROAMING NOT
                    ** ALLOWED (and shall store it according to section 4.1.3.2)
                    ** and enter the state GMM-DEREGISTERED.  Furthermore, it shall
                    ** delete any P-TMSI, P-TMSI signature, RAI and GPRS ciphering
                    ** key sequence number and shall consider the SIM as invalid
                    ** for GPRS services until switching off or the SIM is removed.
                    ** ------------------------------------------------------------ */
                    gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;
                    gmm_delete_ps_location_information();
                    gmm_delete_gprs_cksn();
                    mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                        gmm_stored_ptmsi_signature,
                                                        gmm_stored_rai,
                                                        gmm_update_status );

                    gmm_set_substate( GMM_LIMITED_SERVICE );
                    gmm_set_state( GMM_DEREGISTERED );
                    
                    if (mm_managed_roaming_enabled)
                    {
                      mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
                    }
#ifdef FEATURE_ENHANCED_NW_SELECTION
                    if (reg_sim_read_ens_flag())
                    {
                      mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
                    }
#endif

                    sm_cmd = sm_get_cmd_buf();

                    if (sm_cmd == NULL)
                    {
                      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                    }

                    sm_cmd->header.message_set = MS_GMM_SM;
                    sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                    sm_cmd->cmd.gmm_gprs_rel_ind.cause = 
                         mm_convert_gmm_to_sm_cause(ra_update_reject_msg.gmm_cause);
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


                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                        ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) ||
                         (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)))
                    {
                      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_PS,
                                            ra_update_reject_msg.gmm_cause,
                                            mm_serving_plmn.info.plmn,
                                            mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                            , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                            , mm_serving_plmn.info.active_rat
                                  );
                    } 
                    else
                    {                     
                      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                       ra_update_reject_msg.gmm_cause,
                                       mm_serving_plmn.info.plmn,
                                       mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                       , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                       , mm_serving_plmn.info.active_rat
                                      );
                    }

                    if (mm_reg_waiting_for_reg_cnf)
                    {
                      mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                      mm_send_mmr_reg_cnf();
                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;
                    }
                    else
                    {
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mmr_service_ind.cause = mm_convert_to_reg_cause(
                                                ra_update_reject_msg.gmm_cause );

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND" );

                      mm_send_mmr_service_ind( &mmr_service_ind);
                    }


                    break;

                  case GPRS_SERVICES_NOT_ALLOWED:
                    /* ------------------------------------------------------------
                    ** The MS shall set the GPRS update status to GU3 ROAMING NOT
                    ** ALLOWED (and shall store it according to section 4.1.3.2)
                    ** and enter the state GMM-DEREGISTERED.  Furthermore, it shall
                    ** delete any P-TMSI, P-TMSI signature, RAI and GPRS ciphering
                    ** key sequence number and shall consider the SIM as invalid
                    ** for GPRS services until switching off or the SIM is removed.
                    ** ------------------------------------------------------------ */
                    gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;
                    gmm_delete_ps_location_information();
                    gmm_delete_gprs_cksn();
                    mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                        gmm_stored_ptmsi_signature,
                                                        gmm_stored_rai,
                                                        gmm_update_status );

                    gmm_set_substate( GMM_LIMITED_SERVICE );
                    gmm_set_state( GMM_DEREGISTERED );

                    gmm_manual_mode_user_selection_procedure = FALSE;

                    mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                   ra_update_reject_msg.gmm_cause,
                                   mm_serving_plmn.info.plmn,
                                   mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                  , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                  , mm_serving_plmn.info.active_rat
                                     ); 

                    sm_cmd = sm_get_cmd_buf();

                    if (sm_cmd == NULL)
                    {
                      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                    }

                    sm_cmd->header.message_set = MS_GMM_SM;
                    sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                    sm_cmd->cmd.gmm_gprs_rel_ind.cause = 
                                 mm_convert_gmm_to_sm_cause(ra_update_reject_msg.gmm_cause);
#ifdef FEATURE_DUAL_SIM
                    sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                    MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

                    sm_put_cmd( sm_cmd );

                    if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
                    {
                      mm_serving_plmn.ms_op_mode = SYS_SRV_DOMAIN_CS_ONLY;
                      if((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                         (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
                      {
                        mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain =
                          SYS_SRV_DOMAIN_CS_ONLY; 
                      }
                    
                      if((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                         ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                          (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)))
                      {
                        /* -----------------------------------------------------------
                        ** A GPRS MS operating in MS operation mode A or B in network
                        ** operation mode I, is still IMSI attached for CS services in
                        ** the network and shall then proceed with the appropriate MM
                        ** specific procedure according to the MM service state.
                        ** ----------------------------------------------------------- */
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

                    if(gmm_irat_lu_pending == LTE_IRAT_LU)
                    {
                      mm_pending_lu_request=LTE_IRAT_LU;
                      gmm_irat_lu_pending=NO_LU;
                    }
#ifdef FEATURE_GSM_GPRS
                      if (is_gsm_mode() && (mm_state == MM_IDLE))
                      {
                        mm_send_service_req( &mmcoord_pending_reg_message,
                                             SYS_RAT_GSM_RADIO_ACCESS );
                      }
                      else
#endif

                      if((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                         ((mm_gs_association_established == TRUE) ||
                          (gmm_ra_update_type == GMM_RA_UPDATING)))
                      {
                        if ((mm_location_information.location_update_status == UPDATED) &&
                            (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                            (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ))
                        {
                          if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                          {
#ifdef FEATURE_NAS_REL10
                          if(mm_state == MM_IDLE)
                          {
                             mm_initiate_lu(MM_LU_CAUSE_IMSI_ATTACH);
                          }
                          else
#endif
                          {

                            mm_pending_lu_request = IMSI_ATTACH;
                          }
                        }
                        else
                        {
#ifdef FEATURE_NAS_REL10
                            if(mm_is_per_ms_t3212_present)
                            {
                               if ((mm_stored_per_ms_t3212_value != 0 ) &&
                                   (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                               {
                                 mm_start_timer( TIMER_T3212,
                                                            mm_stored_per_ms_t3212_value);
                               }
                        }
                            else
#endif
                        if ((mm_system_information.T3212_timer_value != 0 ) &&
                            (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                        {
                          mm_start_timer( TIMER_T3212,
                                          mm_system_information.T3212_timer_value );
                        }                    
                      }
                      }
                      else
                      {
#ifdef FEATURE_NAS_REL10
                         mm_substate_control(MM_STD_UPDATE_ATTEMPTED);
                         if(mm_state == MM_IDLE)
                         {
                           mm_initiate_lu(MM_LU_CAUSE_NORMAL);
                         }
                         else
#endif
                         {
                           mm_pending_lu_request = NORMAL_LU;
                         }
                      }                    
                    }

                    }
                    else
                    {
                      gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_PS_INVALID,
                                                         SYS_SRV_DOMAIN_NO_SRV,
                                                         FALSE,
                                                         SYS_SRV_STATUS_LIMITED,
                                                         TRUE );
                    }

#ifdef FEATURE_LTE 
                    if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS))
                    {
                      if (is_umts_mode())
                      {
                        mm_send_rrc_lte_reselection_status_ind();
                      }
                      else if (is_gsm_mode())
                      {
                        mm_send_rr_lte_reselection_status_ind();
                      }
                    }
#endif
                    if (mm_reg_waiting_for_reg_cnf)
                    {
                      mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                      if ((mm_state != MM_IDLE)||
                          (((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
                             (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
                            (mm_lu_attempt_counter <4)))
                      {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN;
                      }
                      mm_send_mmr_reg_cnf();
                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;
                    }
                    else
                    {
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      if ((mm_state != MM_IDLE)||
                          (((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
                             (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
                           (mm_lu_attempt_counter <4)))
                      {
                        mmr_service_ind.cause = MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN;
                      }
                      else
                      {
                        mmr_service_ind.cause = mm_convert_to_reg_cause(
                                                ra_update_reject_msg.gmm_cause );
                      }
                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND" );

                      mm_send_mmr_service_ind( &mmr_service_ind);
                    }


                    break;

                  case MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK:
                    /* -------------------------------------------------------------
                    ** The MS shall set the GPRS update status to GU2 NOT UPDATED
                    ** (and shall store it according to section 4.1.3.2), enter
                    ** the state GMM-DEREGISTERED, and shall delete any P-TMSI,
                    ** P-TMSI signature, RAI and GPRS ciphering key sequence number.
                    ** ------------------------------------------------------------- */
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
                               mm_convert_gmm_to_sm_cause(ra_update_reject_msg.gmm_cause);
#ifdef FEATURE_DUAL_SIM
                    sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                    MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

                    sm_put_cmd( sm_cmd );

                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                        ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
                         (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)))
                    {
                      /* -----------------------------------------------------------
                      ** A GPRS MS operating in MS operation mode A or B in network
                      ** operation mode I, is still IMSI attached for CS services in
                      ** the network.
                      ** ----------------------------------------------------------- */
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
                                            ra_update_reject_msg.gmm_cause,
                                            mm_serving_plmn.info.plmn,
                                            mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                            ,mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                            , mm_serving_plmn.info.active_rat
                                    );

                    /*
                    ** If we are waiting for SERVICE REQ from CM i.e. (mm_ps_detach_state == 
                    ** MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ) then NAS should not 
                    ** start  Attach or PLMN selection procedure
                    */
                    if (mm_reg_waiting_for_reg_cnf)
                    {
#ifdef FEATURE_LTE
                      if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
                      {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                      }
                      else
#endif
                      {
                      mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                      }
                      mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                      mm_send_mmr_reg_cnf();
                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;
                    }
                    else
                    {
                      mmr_service_ind.service_state = mm_serving_plmn.info;
#ifdef FEATURE_LTE
                      if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
                      {
                        mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                      }
                      else
#endif
                      {
                      mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                      }
                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service" );
                      mm_send_mmr_service_ind( &mmr_service_ind );
                    }

#ifdef FEATURE_LTE
                    if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
                    {
                      gmm_set_substate( GMM_LIMITED_SERVICE );
                    }
                    else
#endif
                    {
                      gmm_attach_request_on_network_reject = TRUE;
                    /* --------------------------------------------------------
                    ** Subsequently, the MS may automatically initiate the GPRS
                    ** attach procedure.
                    ** -------------------------------------------------------- */
                    gmm_set_substate( GMM_NORMAL_SERVICE );
                    }


                    break;

                  case IMPLICITLY_DETACHED:

#ifdef FEATURE_GPRS_PS_HANDOVER  
                    if ((gmm_psho_status == GMM_PSHO_IN_PROGRESS) && (gmm_psho_gea_change))
                    {
                      gmm_delete_gprs_cksn();
                    }
#endif                    

                    /* -------------------------------------------------------------
                    ** The MS shall change to state GMM-DEREGISTERED.NORMAL-SERVICE.
                    ** The MS shall then perform a new attach procedure.  The MS
                    ** should also activate PDP context(s) to replace any previously
                    ** active PDP contexts.
                    ** ------------------------------------------------------------- */
                    gmm_set_state( GMM_DEREGISTERED );

                    sm_cmd = sm_get_cmd_buf();

                    if (sm_cmd == NULL)
                    {
                      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                    }

                    sm_cmd->header.message_set = MS_GMM_SM;
                    sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                    sm_cmd->cmd.gmm_gprs_rel_ind.cause =
                            mm_convert_gmm_to_sm_cause(ra_update_reject_msg.gmm_cause);
#ifdef FEATURE_DUAL_SIM
                    sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                    MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND" );

                    sm_put_cmd( sm_cmd );

                    /* ----------------------------------------------------------
                    ** A GPRS MS operating in MS operation mode A or B in network
                    ** operation mode I is IMSI detached for both GPRS and CS
                    ** services in the network.
                    ** ---------------------------------------------------------- */

                    if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                       (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                       (gmm_ra_update_type == GMM_RA_UPDATING) &&
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

                    mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                               ra_update_reject_msg.gmm_cause,
                                               mm_serving_plmn.info.plmn,
                                               mm_serving_plmn.info.lac
                                               #ifdef FEATURE_FEMTO_CSG
                                               ,mm_serving_plmn.info.csg_info.csg_id
                                               #endif 
                                               ,mm_serving_plmn.info.active_rat
                                               );
                    if ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                        (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
                    {
                      mm_possible_imsi_attach = TRUE;
                    }

                    /*
                    ** If we are waiting for SERVICE REQ from CM i.e. (mm_ps_detach_state == 
                    ** MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ) then NAS should not 
                    ** start  Attach or PLMN selection procedure
                    */
                    if (mm_reg_waiting_for_reg_cnf)
                    {
                      mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
#ifdef FEATURE_LTE
                      if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
                      {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                      }
                      else
#endif
                      {
                      mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                      }
                      mm_send_mmr_reg_cnf();
                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;
                    }
                    else
                    {
                      mmr_service_ind.service_state = mm_serving_plmn.info;
#ifdef FEATURE_LTE
                      if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
                      {
                        mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                      }
                      else
#endif
                      {
                      mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                      }
                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service");
                      mm_send_mmr_service_ind( &mmr_service_ind );
                    }

#ifdef FEATURE_LTE
                    if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
                    {
                      gmm_set_substate( GMM_LIMITED_SERVICE );
                    }
                    else
#endif
                    {
                      gmm_attach_request_on_network_reject = TRUE;
                    gmm_set_substate( GMM_NORMAL_SERVICE );
                    }

                    break;

                  case PLMN_NOT_ALLOWED:
                  case LA_NOT_ALLOWED:
                  case NATIONAL_ROAMING_NOT_ALLOWED:
                  case NO_SUITABLE_CELLS_IN_LA:
                    /* -------------------------------------------------------------
                    ** The MS shall delete any RAI, P-TMSI, P-TMSI signature and
                    ** GPRS ciphering key sequence number, shall set the GPRS update
                    ** status to GU3 ROAMING NOT ALLOWED and enter the state
                    ** GMM-DEREGISTERED.
                    ** ------------------------------------------------------------- */
                    if (ra_update_reject_msg.gmm_cause == PLMN_NOT_ALLOWED)
                    {
                      gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;

                      mm_add_plmn_to_fplmn_list( mm_serving_plmn.info.plmn);
                    }
                    else
                    {
                      gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
                    }
                      /* Set cs service status to limited if rej. cause is #11, #12, or #13 only in manual mode */
                    if(mm_managed_roaming_enabled && 
                       (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) && 
                       (ra_update_reject_msg.gmm_cause == PLMN_NOT_ALLOWED ||
                        ra_update_reject_msg.gmm_cause == LA_NOT_ALLOWED ||
                        ra_update_reject_msg.gmm_cause == NATIONAL_ROAMING_NOT_ALLOWED) && 
                        (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                         mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
                    {
                      mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
                    }
#ifdef FEATURE_ENHANCED_NW_SELECTION
                    if(reg_sim_read_ens_flag() && 
                       (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) && 
                       (ra_update_reject_msg.gmm_cause == PLMN_NOT_ALLOWED ||
                        ra_update_reject_msg.gmm_cause == LA_NOT_ALLOWED ||
                        ra_update_reject_msg.gmm_cause == NATIONAL_ROAMING_NOT_ALLOWED) && 
                        (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                         mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
                    {
                      mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
                    }
#endif

                    if((ra_update_reject_msg.gmm_cause == PLMN_NOT_ALLOWED) ||
                       (ra_update_reject_msg.gmm_cause == LA_NOT_ALLOWED) ||
                       (ra_update_reject_msg.gmm_cause == NATIONAL_ROAMING_NOT_ALLOWED))
                    {
                      gmm_manual_mode_user_selection_procedure = FALSE;

                      /***************************************************************
                      **Ideally this flag should be reet in case of NMO1.But in case
                      **of NMO2, network may send PLMN_NOT_ALLOWED/LA_NOT_ALLOWED/
                      **NATIONAL_ROAMING_NOT_ALLOWED for RAU and some
                      **other casue for LU.In that case we should not initiate LU
                      **also.So reset this falg irrespective of NMO
                      ***************************************************************/
                      
                      mm_manual_mode_user_selection_procedure = FALSE;                                            
                    }
                       
#ifdef FEATURE_LTE
                    if((ra_update_reject_msg.gmm_cause == PLMN_NOT_ALLOWED) ||
                       (ra_update_reject_msg.gmm_cause == NATIONAL_ROAMING_NOT_ALLOWED))
                    {  
                      mm_set_emm_manual_selection_flag(FALSE);
                    }
#endif

                    if ((ra_update_reject_msg.gmm_cause == PLMN_NOT_ALLOWED) ||
                        (ra_update_reject_msg.gmm_cause == LA_NOT_ALLOWED))
                    {
                      gmm_delete_ps_location_information();
                      gmm_delete_gprs_cksn();

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

                        /* --------------------------------------------------
                        ** The MM state will transition back to IDLE when the
                        ** RR(C) connection is released
                        ** -------------------------------------------------- */
                      }
                    }
                    else
                    {
/*
   The REL6 CR916 says that RAU with type combined with imsi attach should be triggered after the PLMN selection
   Procedure if possible(i,e try to establish the Gs association). this will happen when the gs_association flag is set
   to FALSE.
  */
                      mm_gs_association_established = FALSE;

#ifdef FEATURE_GPRS_PS_HANDOVER                  
                      if ((gmm_psho_status == GMM_PSHO_IN_PROGRESS) && (gmm_psho_gea_change))
                      {
                        gmm_delete_gprs_cksn();
                      }
#endif
                      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
                      {
                        /* ---------------------------------------------------------
                        ** If the MS is IMSI attached, the MS shall in addition set
                        ** the update status to U3 ROAMING NOT ALLOWED and shall
                        ** reset the location update attempt counter.
                        ** --------------------------------------------------------- */
                        mm_location_information.location_update_status = ROAMING_NOT_ALLOWED;

                        /* --------------------------------
                        ** Copy location information to SIM
                        ** -------------------------------- */
                        mmsim_write_loci();

                        mm_clear_lu_attempt_counter( );

                        /* --------------------------------------------------
                        ** The MM state will transition back to IDLE when the
                        ** RR(C) connection is released
                        ** -------------------------------------------------- */
                      }
                    }

                    mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                        gmm_stored_ptmsi_signature,
                                                        gmm_stored_rai,
                                                        gmm_update_status );

                    /* -------------------------------------------------------------
                    ** The MS shall store the LAI or the PLMN identity in the
                    ** appropriate forbidden list, i.e., in the "forbidden PLMN
                    ** list" for cause #11, in the list of "forbidden location areas
                    ** for regional provision of service" for cause #12 or in the
                    ** list of "forbidden location areas for roaming" for cause #13.
                    ** If #11 #13 or #15 was received, the MS shall perform a PLMN
                    ** selection instead of a cell selection.
                    ** ------------------------------------------------------------- */
                    if (ra_update_reject_msg.gmm_cause == LA_NOT_ALLOWED)
                    {
#ifdef FEATURE_FORBID_LAI_UPON_REGIONAL_ROAMING_REJECT
                      /* ---------------------------------------------------------
                      ** This FEATURE was introduced to support a customer request
                      ** for a non-spec compliant PLMN selection on receipt of
                      ** reject cause LA_NOT_ALLOWED
                      ** --------------------------------------------------------- */
                      mm_add_forbidden_national_roaming( &gmm_current_lai );
#else
                      mm_add_forbidden_regional_roaming( &gmm_current_lai );
#endif
                    }
                    else if ((ra_update_reject_msg.gmm_cause == NATIONAL_ROAMING_NOT_ALLOWED) ||
                             (ra_update_reject_msg.gmm_cause == NO_SUITABLE_CELLS_IN_LA))
                    {
                      mm_add_forbidden_national_roaming( &gmm_current_lai );
                    }
                    if(ra_update_reject_msg.gmm_cause == NO_SUITABLE_CELLS_IN_LA)
                    {
                      if((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
                         (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH))
                      {
                        mm_possible_imsi_attach = TRUE;
                      }
                      /* add LAI in LAI reject list */ 
                      mm_add_lai_in_reject_list(&gmm_current_lai);
                    }
                    if(ra_update_reject_msg.gmm_cause == LA_NOT_ALLOWED)
                    {
                      if((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
                         (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH))
                      {
                        mm_substate_control( MM_STD_LIMITED_SERVICE );
                      }
                    }
                    gmm_set_substate( GMM_LIMITED_SERVICE );
                    gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     SYS_SRV_DOMAIN_NO_SRV,
                                                     TRUE,
                                                     SYS_SRV_STATUS_LIMITED,
                                                     TRUE );
                    /**************************************************************
                    *24.008 4.7.1.9 The MS shall start the timer T3340  if the MS *
                    * receives any of the reject cause values #11,#12,#13 or #15  *
                    **************************************************************/
                    if (is_umts_mode( ))
                    {
                      mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);
                    }

                    if (((ra_update_reject_msg.gmm_cause == NATIONAL_ROAMING_NOT_ALLOWED) ||
                        (ra_update_reject_msg.gmm_cause == NO_SUITABLE_CELLS_IN_LA))
#ifdef FEATURE_LTE
                       &&(mm_ps_detach_state != MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
#endif
                        )
                    {
                      gmm_set_state( GMM_REGISTERED );
                      gmm_processing_directed_signalling_reestablishment = FALSE; 
                    }
                    else
                    {
                      gmm_set_state( GMM_DEREGISTERED );

                      sm_cmd = sm_get_cmd_buf();

                      if (sm_cmd == NULL)
                      {
                        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                      }

                      sm_cmd->header.message_set = MS_GMM_SM;
                      sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                      sm_cmd->cmd.gmm_gprs_rel_ind.cause =
                          mm_convert_gmm_to_sm_cause(ra_update_reject_msg.gmm_cause);                        
#ifdef FEATURE_DUAL_SIM
                      sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND" );

                      sm_put_cmd( sm_cmd );
                    }                      

                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                        ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) ||
                         (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)))
                    {
                      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_PS,
                                        ra_update_reject_msg.gmm_cause,
                                        mm_serving_plmn.info.plmn,
                                        mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                        , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                        , mm_serving_plmn.info.active_rat
                              );
                    } 
                    else
                    {                     
                      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                    ra_update_reject_msg.gmm_cause,
                                    mm_serving_plmn.info.plmn,
                                    mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                   , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                   , mm_serving_plmn.info.active_rat
                                 );
                    }

                    /*
                    ** If we are waiting for SERVICE REQ from CM i.e. (mm_ps_detach_state == 
                    ** MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ) then NAS should not 
                    ** start  Attach or PLMN selection procedure
                    */
                    if (mm_reg_waiting_for_reg_cnf)
                    {
                      mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
#ifdef FEATURE_LTE
                      if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
                      {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                      }
                      else
#endif
                      {
                      mmcoord_mmr_reg_cnf.cause = mm_convert_to_reg_cause(
                                                ra_update_reject_msg.gmm_cause );
                      }

                      mm_send_mmr_reg_cnf();
                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;
                    }
                    else
                    {
                      mmr_service_ind.service_state = mm_serving_plmn.info;
#ifdef FEATURE_LTE
                      if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
                      {
                        mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                      }
                      else
#endif
                      {
                      mmr_service_ind.cause = mm_convert_to_reg_cause(
                                                  ra_update_reject_msg.gmm_cause );
                      }

                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service");
                      mm_send_mmr_service_ind( &mmr_service_ind );
                    }


                    break;

                  case GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN:
                    /* -------------------------------------------------------------
                    ** The MS shall delete any RAI, P-TMSI, P-TMSI signature and
                    ** GPRS ciphering key sequence number, shall set the GPRS update
                    ** status to GU3 ROAMING NOT ALLOWED and enter the state
                    ** GMM-DEREGISTERED.
                    ** ------------------------------------------------------------- */
                    gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
                    if (reg_sim_per_subs_is_ehplmn(mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
                    if (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), mm_serving_plmn.info.plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
                    if (reg_sim_is_ehplmn(mm_serving_plmn.info.plmn) )
#else
                    if (sys_plmn_match(reg_sim_read_hplmn(), mm_serving_plmn.info.plmn) )
#endif
#endif
                    {
                      gmm_update_gprs_forbidden_rai(gmm_previous_registration_serving_rai);
                    }
                    else
                    {
                      reg_sim_add_plmn_to_gprs_fplmn_list( mm_serving_plmn.info.plmn
#ifdef FEATURE_DUAL_SIM
                                                         ,(sys_modem_as_id_e_type)mm_sub_id
#endif
                                                       );
                    }              
#ifdef FEATURE_LTE 
                    if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS))
                    {
                      if (is_umts_mode())
                      {
                        mm_send_rrc_lte_reselection_status_ind();
                      }
                      else if (is_gsm_mode())
                      {
                        mm_send_rr_lte_reselection_status_ind();
                      }
                    }
#endif 
                    
                    gmm_delete_ps_location_information();
                    gmm_delete_gprs_cksn();
                    mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                        gmm_stored_ptmsi_signature,
                                                        gmm_stored_rai,
                                                        gmm_update_status );

                    gmm_manual_mode_user_selection_procedure = FALSE;

                    gmm_set_substate( GMM_LIMITED_SERVICE );
                    gmm_set_state( GMM_DEREGISTERED );

                    mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                      ra_update_reject_msg.gmm_cause,
                                      mm_serving_plmn.info.plmn,
                                      mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                      , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                      , mm_serving_plmn.info.active_rat
                              );

                    sm_cmd = sm_get_cmd_buf();

                    if (sm_cmd == NULL)
                    {
                      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                    }

                    sm_cmd->header.message_set = MS_GMM_SM;
                    sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                    sm_cmd->cmd.gmm_gprs_rel_ind.cause = 
                             mm_convert_gmm_to_sm_cause(ra_update_reject_msg.gmm_cause);
#ifdef FEATURE_DUAL_SIM
                    sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                    MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND" );

                    sm_put_cmd( sm_cmd );

                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                        ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
                        (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)))
                    {
                      /* -----------------------------------------------------------
                      ** A GPRS MS operating in MS operation mode A or B in network
                      ** operation mode I, is still IMSI attached for CS services in
                      ** the network and shall then proceed with the appropriate MM
                      ** specific procedure according to the MM service state.
                      ** Ref: TS 24.008 4.7.5.2.4 V10.7.0.
                      ** ----------------------------------------------------------- */
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


                    if(gmm_irat_lu_pending == LTE_IRAT_LU)
                    {
                      mm_pending_lu_request=LTE_IRAT_LU;
                      gmm_irat_lu_pending=NO_LU;
                    } 
#ifdef FEATURE_GSM_GPRS
                   if (is_gsm_mode() && (mm_state == MM_IDLE))
                   {
                     mm_send_service_req( &mmcoord_pending_reg_message,
                                             SYS_RAT_GSM_RADIO_ACCESS );
                   }
                   else
#endif

                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                        ((mm_gs_association_established == TRUE)||
                        (gmm_ra_update_type == GMM_RA_UPDATING)))
                    {
                      if (((!(( mm_location_information.location_update_status == UPDATED ) &&
                            ( memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)))
#ifdef FEATURE_LTE 
                            ||(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI)
#endif
                          )
                         )
                      {
                      
                        if(mm_pending_lu_request!=LTE_IRAT_LU)
                        {
                          mm_pending_lu_request = NORMAL_LU;
                        }                      
                      }      
#ifdef FEATURE_NAS_REL10
                      else
                      {
                        mm_substate_control(MM_STD_NORMAL_SERVICE);
                        if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                      {
                        if (mm_state == MM_IDLE)
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
#endif
                      else if ((mm_system_information.T3212_timer_value != 0 ) &&
                               (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                      {
                        mm_start_timer( TIMER_T3212,
                                        mm_system_information.T3212_timer_value );
                      }
#ifdef FEATURE_NAS_REL10
                         }
                    }
#endif
                    }

                    if (mm_reg_waiting_for_reg_cnf)
                    {
                      mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                      if ((mm_state != MM_IDLE)||
                          (((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
                             (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
                           (mm_lu_attempt_counter <4)) 
#ifdef FEATURE_NAS_REL10
                        ||
                          (mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)        
#endif
                      )
                      {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN;
                      }
                      else
                      {
                        //cause is set to MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES
                        mmcoord_mmr_reg_cnf.cause =
                           MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES;
                      }
                      mm_send_mmr_reg_cnf();
                      mm_reg_waiting_for_reg_cnf = FALSE;
                      mm_ready_to_send_reg_cnf   = FALSE;
                    }
                    else
                    {
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      if ((mm_state != MM_IDLE)||
                          (((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
                             (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
                           (mm_lu_attempt_counter <4))
#ifdef FEATURE_NAS_REL10
                            ||
                            (mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
#endif
                      )
                      {
                        mmr_service_ind.cause = MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN;
                      }
                      else
                      {
                        mmr_service_ind.cause = mm_convert_to_reg_cause(
                                              ra_update_reject_msg.gmm_cause );
                      }
                      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service");
                      mm_send_mmr_service_ind( &mmr_service_ind );
                    }


                    break;
#ifdef FEATURE_NAS_REL10
                    case CONGESTTION: 

                         if(gmm_t3346_backoff_timer_value == 0)
                         {
                           gmm_increment_routing_area_update_attempt_counter
                           (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                      FALSE
#endif
                           );
                           mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                                      ra_update_reject_msg.gmm_cause,
                                                      mm_serving_plmn.info.plmn,
                                                      mm_serving_plmn.info.lac
#ifdef FEATURE_FEMTO_CSG
                                                    , mm_serving_plmn.info.csg_info.csg_id
#endif 
                                                    , mm_serving_plmn.info.active_rat
                                                     );
                         }
                         else
                         {

                              /*******************************************************************************
                           **Cause #22-T3346 Started- The MS shall abort the RAU procedure, reset the RAU attempt counter, 
                           **set the GPRS update status to GU2 NOT UPDATED and enter state GMM-REGISTERED.ATTEMPTING-TO-UPDATE
                           **(24.008 section 4.7.5.1.4) *****************************************************/
                           gmm_rau_attempt_counter = 0;
                           MSG_HIGH_DS_1( MM_SUB, "=MM= GPRS RAU Attempt counter = %d",
                                        gmm_rau_attempt_counter);

                           gmm_set_state( GMM_REGISTERED );

                           gmm_is_congestion_due_to_rau = TRUE;

                           gmm_update_status = GMM_GU2_NOT_UPDATED;

                           mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                               gmm_stored_ptmsi_signature,
                                                               gmm_stored_rai,
                                                               gmm_update_status );

                           gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE);

                            /**-------------------------------------------------------------------------
                                         **A GPRS MS operating in MS operation mode A or B which is already IMSI attached
                                         **for CS services in the network is still IMSI attached for CS services in the network.
                                         **(24.008 section 4.7.3.1.4) ------------------------------------------------*/
                                                
                           if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                              (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                              (mm_location_information.location_update_status == UPDATED) &&
                              (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0) &&
                              ((mm_serving_plmn.info.reg_service_domain  == SYS_SRV_DOMAIN_CS_PS) ||
                               (mm_serving_plmn.info.reg_service_domain  == SYS_SRV_DOMAIN_CS_ONLY)))
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
                                                     ra_update_reject_msg.gmm_cause,
                                                     mm_serving_plmn.info.plmn,
                                                     mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                                   , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                                   , mm_serving_plmn.info.active_rat
                                                     );

                         if (mm_managed_roaming_enabled)
                         {
                           mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
                         }
#ifdef FEATURE_ENHANCED_NW_SELECTION
                         if (reg_sim_read_ens_flag())
                         {
                           mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
                         }
#endif
                            /**************************************************************
                         *24.008 4.7.1.9 The MS shall start the timer T3346    if the MS *
                         * receives    the reject cause values #22  *
                         **************************************************************/
                         if ((is_umts_mode ()) &&
                             (gmm_integrity_protection_activated == TRUE))
                         {
                           mm_start_timer(TIMER_T3346,gmm_t3346_backoff_timer_value);
                         }
                         else
                         {
                           gmm_t3346_backoff_timer_value = ((15 + mm_get_random_value(15)) * 60 * 1000);
                           mm_start_timer(TIMER_T3346,gmm_t3346_backoff_timer_value);
                         }

#ifdef FEATURE_NAS_REL11
                         if(mm_is_rrc_access_class_11_to_15() == TRUE)
                         {
                           MSG_HIGH_DS_0(MM_SUB ,"=MM= Retrying RAU in NW congestion for high AC users ");
                           gmm_ra_update_type = gmm_pending_ra_update_type;
                           gmm_initiate_routing_area_update();
                         }
#endif
                         /*saves congestion plmn associated with the timer*/
                         memscpy((void *)&gmm_3gpp_t3346_plmn_id, sizeof(sys_plmn_id_s_type),(void *)&(mm_serving_plmn.info.plmn), sizeof(sys_plmn_id_s_type));
                         gmm_t3346_backoff_timer_value = 0;

                         /* For High AC users if ATTACH was re-initiated above, then 
                          * reject the pending call with cause such that re-dial is allowed.
			  * In that case, don't send current Service status to REG, instead
			  * wait for Registration to conclude.
                          */			
                         if(gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
                         {
                           if (mm_held_cnm_service_req.present)
                           {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                             mm_send_mmcnm_est_rej(
                                AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL ,MMCC_SMS_CS);
#else
                             mm_send_mmcnm_est_rej(
                                AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
                           }
                         }
                         else
                         {
                           mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
  
                           if (mm_reg_waiting_for_reg_cnf)
                           {
                             mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                             mm_send_mmr_reg_cnf();
                             mm_reg_waiting_for_reg_cnf = FALSE;
                             mm_ready_to_send_reg_cnf = FALSE;
                           }
                           else
                           {
                             mmr_service_ind.service_state = mm_serving_plmn.info;
                             mmr_service_ind.cause = mmcoord_mmr_reg_cnf.cause;
                             mm_send_mmr_service_ind( &mmr_service_ind );
                           }
                         }
                      }
                    break;
#endif

#ifdef FEATURE_FEMTO_CSG
                    case CSG_NOT_AUTHORIZED:
                    if(mm_serving_csg_id != SYS_CSG_ID_INVALID)
                    {
                      gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
  
                      gmm_rau_attempt_counter = 0;
                      mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                          gmm_stored_ptmsi_signature,
                                                          gmm_stored_rai,
                                                          gmm_update_status );
  
                      mm_delete_csg_from_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info);
                      gmm_set_substate( GMM_LIMITED_SERVICE );
                      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                       SYS_SRV_DOMAIN_NO_SRV,
                                                       TRUE,
                                                       SYS_SRV_STATUS_LIMITED,
                                                       TRUE );
                         
                      if(mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
                      {
                        mm_clear_lu_attempt_counter();
                      }
                      /**************************************************************
                      *24.008 4.7.1.9 The MS shall start the timer T3340  if the MS *
                      * receives any of the reject cause values #25  *
                      **************************************************************/
                      if (is_umts_mode( ))
                      {
                        mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);
                      }
  
  #ifdef FEATURE_LTE
                       if(mm_ps_detach_state != MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
  #endif
                          
                      {
                        gmm_set_state( GMM_REGISTERED );  
                      }
  #ifdef FEATURE_LTE
                      else
                      {
                        gmm_set_state( GMM_DEREGISTERED );
  
                        sm_cmd = sm_get_cmd_buf();
  
                        if (sm_cmd == NULL)
                        {
                          MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                        }
  
                        sm_cmd->header.message_set = MS_GMM_SM;
                        sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                        sm_cmd->cmd.gmm_gprs_rel_ind.cause =
                            mm_convert_gmm_to_sm_cause(ra_update_reject_msg.gmm_cause);                        
#ifdef FEATURE_DUAL_SIM
                        sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif
                        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");
  
                        sm_put_cmd( sm_cmd );
                      }                      
  #endif 
                      if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                          ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) ||
                           (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)))
                      {
                        mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_PS,
                                     ra_update_reject_msg.gmm_cause,
                                     mm_serving_plmn.info.plmn,
                                     mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                     , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                     , mm_serving_plmn.info.active_rat
                                  );
                      } 
                      else
                      {                     
                        mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                    ra_update_reject_msg.gmm_cause,
                                    mm_serving_plmn.info.plmn,
                                    mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                    , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                    , mm_serving_plmn.info.active_rat
                                            );
                      }
  
                      /*
                      ** If we are waiting for SERVICE REQ from CM i.e. (mm_ps_detach_state == 
                      ** MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ) then NAS should not 
                      ** start  Attach or PLMN selection procedure
                      */
                      if (mm_reg_waiting_for_reg_cnf)
                      {
                        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
  #ifdef FEATURE_LTE
                        if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
                        {
                          mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                        }
                        else
  #endif
                        {
                          mmcoord_mmr_reg_cnf.cause = mm_convert_to_reg_cause(
                                                    ra_update_reject_msg.gmm_cause );
                        }
  
                        mm_send_mmr_reg_cnf();
                        mm_reg_waiting_for_reg_cnf = FALSE;
                        mm_ready_to_send_reg_cnf   = FALSE;
                      }
                      else
                      {
                        mmr_service_ind.service_state = mm_serving_plmn.info;
  #ifdef FEATURE_LTE
                        if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
                        {
                          mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                        }
                        else
  #endif
                        {
                          mmr_service_ind.cause = mm_convert_to_reg_cause(
                                                      ra_update_reject_msg.gmm_cause );
                        }
  
                        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service" );
                        mm_send_mmr_service_ind( &mmr_service_ind );
                      }
                    }
                    else
                    {
                      gmm_increment_routing_area_update_attempt_counter
                      (
  #ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                        FALSE
  #endif
                      );

                    }
                    break;
#endif 

                  default:
                    /* If rejected with cause #17, add to low priority PLMN list */
                    if(mm_managed_roaming_enabled && (ra_update_reject_msg.gmm_cause == NETWORK_FAILURE))
                    {
                      reg_sim_add_to_low_priority_plmn_list(mm_serving_plmn.info.plmn, mm_serving_plmn.info.active_rat
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                                            ,(sys_modem_as_id_e_type)mm_as_id
#endif
                                                           );

                    }
                    /* ----------------------------------------------------------
                    ** Other values are considered as abnormal cases - stop timer
                    ** T3330 if running and increment the routing area updating
                    ** attempt counter.
                    ** ---------------------------------------------------------- */
                      if(ra_update_reject_msg.gmm_cause == MM_SEMANTICALLY_INCORRECT_MESSAGE ||
                         ra_update_reject_msg.gmm_cause == MM_INVALID_MANDATORY_INFORMATION ||
                         ra_update_reject_msg.gmm_cause == MM_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED ||
                         ra_update_reject_msg.gmm_cause == MM_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED ||
                         ra_update_reject_msg.gmm_cause == MM_PROTOCOL_ERROR_UNSPECIFIED)
                      {
                        /* Upon reception of Cause Codes #95 #96 #97 #99 #111, The MS should set the 
                           gmm_rau_attempt_counter to 5 
                           and wait for rel-ind from NW 24.008 rel6 Section 4.7.5.1.5 */           
                        gmm_rau_attempt_counter = (byte) (MAX_ATTEMPT_COUNTER_RAU - 1);
                      }      
                    gmm_increment_routing_area_update_attempt_counter
                    (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                      FALSE
#endif
                    );


                    if ((ra_update_reject_msg.gmm_cause == IMSI_UNKNOWN_IN_HLR) ||
                        (ra_update_reject_msg.gmm_cause == IMSI_UNKNOWN_IN_VLR) ||
                        (ra_update_reject_msg.gmm_cause == NETWORK_FAILURE) ||
                        (ra_update_reject_msg.gmm_cause == CONGESTTION) ||
                        (ra_update_reject_msg.gmm_cause == MSC_TEMPORARILY_NOT_REACHABLE))
                    {
                      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY,
                                    ra_update_reject_msg.gmm_cause,
                                    mm_serving_plmn.info.plmn,
                                    mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                    , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                    , mm_serving_plmn.info.active_rat
                           );
                    }

#ifdef FEATURE_GSM_GPRS
                    if (gmm_reg_report_required)
                    {
                      /* ----------------------------------------------------
                      ** REG report is pending due to Inter RAT PS continuity
                      ** ---------------------------------------------------- */
                      mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mm_send_mmr_service_ind( &mmr_service_ind );
                    }
#endif /* #ifdef FEATURE_GSM_GPRS */
                    break;
                } /* end switch (ra_update_reject_msg.gmm_cause) */

#ifdef FEATURE_GSTK
                curr_service_status = mm_serving_plmn.info.service_status;
                if ( prev_service_status != curr_service_status)
                {
                   service_status_change = TRUE;
                }
                else
                {
                   service_status_change = FALSE;
                }
                gmm_send_gstk_idle_event_if_needed(ra_update_reject_msg.gmm_cause, service_status_change); 
#endif

#if defined(FEATURE_LTE)
                if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
                {
                  emm_multimode_complete_update();
                }
#endif

#ifdef FEATURE_GSM_GPRS
                if (is_gsm_mode())
                {
                  gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_FORCE_TO_STANDBY,
                                     ra_update_reject_msg.force_to_standby,0) ;
                }
#endif
              }
              else
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Invalid ROUTING AREA UPDATE REJECT");
              }
              break;

            default:
              MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message %d",
                         message->cmd.mm_data_ind.L3_data[1], 0,0 );
          } /* end switch (gmm_message_type) */

#ifdef FEATURE_GPRS_PS_HANDOVER  
          gmm_psho_status = GMM_PSHO_NONE;
#endif
          
      /* Reset the Inter RAT PS continuity flags now that RAU procedure is completed */
#ifdef FEATURE_GSM_GPRS
          if (gmm_reg_report_required)
          {
            gmm_reg_report_required = FALSE;
            gmm_ps_data_available = FALSE;
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)            
            gmm_rat_chng_pended_msg_valid = FALSE ;
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
          }
#endif /* #ifdef FEATURE_GSM_GPRS */

#ifdef FEATURE_MODEM_HEAP
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
          if((gmm_rat_chng_pended_msg_valid == FALSE) &&
             (gmm_rat_chng_pended_msg != NULL))
          {
            modem_mem_free(gmm_rat_chng_pended_msg,MODEM_MEM_CLIENT_NAS);
            gmm_rat_chng_pended_msg = NULL;
          }
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
#endif
          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected RRC message %d", message->cmd.hdr.message_id, 0,0 );
      } /* end switch (message->cmd.hdr.message_id) */

      break;

    case MS_TIMER:
      switch (message->cmd.timer_expiry.timer_id)
      {
        case TIMER_T3330:
          /* -------------------------------------------------------------------
          ** T3330 timeout - The procedure is restarted four times, i.e., on the
          ** fifth expiry of timer T3330, the MS shall abort the procedure and
          ** increment the routing area updating attempt counter.
          ** ------------------------------------------------------------------- */
          t3330_timeout_counter++;
          MSG_HIGH_DS_1(MM_SUB, "=MM= T3330 timeout counter = %d", t3330_timeout_counter);
          /*Cancel any running authentication req*/
          
#ifdef FEATURE_GPRS_PS_HANDOVER 
          gmm_psho_status = GMM_PSHO_NONE;
#endif
          gmm_extend_connection_due_to_mt_csfb = FALSE;

          if (t3330_timeout_counter < 5)
          {
            if ((gmm_ra_update_pending)
#ifdef FEATURE_WRLF_SYSTEM_SEL
                 &&(mm_wcdma_rlf_state == WCDMA_RLF_IDLE)
#endif
               )
            {
                gmm_ra_update_type = gmm_pending_ra_update_type;
            }          
            /* Spec_ref: 4.1.1.2.1
               A GPRS MS operating in mode A or B in a network that operates in mode I shall perform a normal location updating procedure (in order to remove the Gs association in the MSC/VLR) when the following conditions are fulfilled:
               -    the GPRS MS has camped on a cell where the PS domain is barred and the CS domain is unbarred;
               -    and -   T3312, T3311, T3302, or T3330 expires.*/
            if (gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)))
            {
              gmm_set_substate(GMM_UPDATE_NEEDED);
#ifdef FEATURE_DSAC
              if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                  ((mm_gs_association_established == TRUE) ||
                   (gmm_ra_update_type == GMM_PERIODIC_UPDATING)))
              {
                if (mm_state == MM_IDLE)
                {
                  mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                }
                else
                {
                  mm_pending_lu_request = NORMAL_LU;
                }
              }
#endif
            }
            else
            {
            gmm_initiate_routing_area_update();
#ifdef FEATURE_WRLF_SYSTEM_SEL
              if(mm_wcdma_rlf_state == WCDMA_RLF_STARTED)
              {
                gmm_ra_update_pending = TRUE;
              }
#endif
            }
          }
          else /* t3330_timeout_counter == 5 */
          {
            t3330_timeout_counter = 0;
            MSG_HIGH_DS_1( MM_SUB, "=MM= T3330 timeout counter = %d", t3330_timeout_counter );
            gmm_increment_routing_area_update_attempt_counter
            (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
             FALSE
#endif
            );

            {
               mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_TIME_OUT,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
              /* ------------------------------------------------
              ** The MS shall release the PS signaling connection, unconditionally 
              **    after t3330 expires 5 times
              ** ------------------------------------------------ */
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
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending RRC_CLOSE_SESSION_REQ");

                send_cmd_to_rrc( rrc_cmd, mm_serving_plmn.info.active_rat );
              }
#endif /* #ifdef FEATURE_WCDMA */

              gmm_ps_signalling_connected = FALSE;

              if (pmm_mode == PMM_CONNECTED)
              {
                /* -------------------------------------------------------------
                ** In UMTS the timer T3312 is reset and started with its initial
                ** value when the MS goes from PMM-CONNECTED to PMM-IDLE mode
                ** (TS 24.008 4.7.2.2)
                ** ------------------------------------------------------------- */
                pmm_mode = PMM_IDLE;
                MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_IDLE");
/*24.008:4.7.7.2 
The RAND and RES values stored in the mobile station shall be deleted and timer T3316, if running, shall be stopped:
-if the mobile station enters PMM-IDLE mode (Iu mode only).*/
#ifdef FEATURE_NAS_REL11
                if (is_umts_mode())
                {
                  auth_reset_cache_memory(FALSE,NO_TIMER_EXPIRED,RRC_PS_DOMAIN_CN_ID) ;
                }
#endif

                gmm_sm_send_con_release_ind(SIG_CONNECTION_RELEASE);
                if (gmm_t3312_timer_value != 0)
                {
                  mm_start_timer( TIMER_T3312, gmm_t3312_timer_value );
                }
                mm_stop_timer( TIMER_T3319 );
              }
            }
          }
          break;

        default:
          MSG_ERROR_DS_1( MM_SUB, "=MM= Unexpected timer message %d",
                message->cmd.timer_expiry.timer_id );
          break;
      }
      break;

    default:
      MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message set %d", message->cmd.hdr.message_set, 0,0 );
  } /* end switch (message->cmd.hdr.message_set) */
} /* end gmm_handle_routing_area_update() */
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/


/*===========================================================================

FUNCTION GMM_INITIATE_GPRS_DETACH

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
void gmm_initiate_gprs_detach( mm_cmd_type *message )
{
  mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_NONE);
  sm_cmd_type                *sm_cmd = NULL;
  boolean ps_access_barred = FALSE;
  
#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

  if (message == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= Unexpected NULL pointer", 0,0,0 );
  }
  if( (is_umts_mode()) && (ps_session_status == MMRRC_ACTIVE))
  {
    ps_access_barred = FALSE;
  }
  else
  {
    ps_access_barred = gmm_check_if_ps_cell_access_barred(ps_cell_access);
  }

 /* Check if GMM is in Limited Service */
  if ((gmm_substate == GMM_LIMITED_SERVICE)||
       (ps_access_barred)  ||
      (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
#ifdef FEATURE_GSM_GPRS
       ||
      ((gmm_state == GMM_REGISTERED_INITIATED)&&
       (gmmllc_get_glif_grr_camp_state() == GLIF_CAMP_ON_PCCCH_REQUESTED))
#endif
#ifdef FEATURE_LTE
      ||((message->cmd.hdr.message_id == (int)MMR_STOP_MODE_REQ)&&
           (message->cmd.mmr_stop_mode_req.stop_mode_reason == 
                                          SYS_STOP_MODE_REASON_LOCAL_DETACH))
      ||(emm_ctrl_data_ptr->l2gcco_state == EMM_LTOG_CCO_ABORTED)
      ||((emm_ctrl_data_ptr->is_sglte_device == TRUE) &&
           (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)&&
              ((is_gsm_mode()) && ((gmm_substate == GMM_SUSPENDED) || gmm_is_suspended)))
#endif
     )
  {
    /* If GMM is in REGISTERED and LIMITED SERVICE Detach not allowed */
    MSG_HIGH_DS_0(MM_SUB, "=MM= GMM substate Limited or PS cell access is barred or GPRS service not available");

    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) &&
        (message->cmd.hdr.message_id  == (int) MMR_REG_REQ) )
    {
      if (gmm_substate != GMM_NORMAL_SERVICE) 
      {
        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                        SYS_SRV_DOMAIN_NO_SRV,
                                        FALSE,
                                        SYS_SRV_STATUS_LIMITED,
                                        FALSE );
      }
      else
      {
        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                        SYS_SRV_DOMAIN_NO_SRV,
                                        FALSE,
                                        mm_serving_plmn.info.service_status,
                                        FALSE );
      }
#ifdef FEATURE_GSM_GPRS
      if ((gmm_state == GMM_REGISTERED_INITIATED)&&
          (gmmllc_get_glif_grr_camp_state() == GLIF_CAMP_ON_PCCCH_REQUESTED))
      {
        gmm_set_state( GMM_DEREGISTERED );
      }
#endif
    }
    else
    {
      gmm_set_state( GMM_DEREGISTERED );

      sm_cmd = sm_get_cmd_buf();

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

      if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
          (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS ||
           mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) )
    
      {
        gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                         SYS_SRV_DOMAIN_CS_ONLY,
                                         FALSE,
                                         SYS_SRV_STATUS_SRV,
                                         FALSE );
      }
      else
      {
        gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                         SYS_SRV_DOMAIN_NO_SRV,
                                         FALSE,
                                         SYS_SRV_STATUS_LIMITED,
                                         FALSE );
      } 
    }
    /* ---------------------------------------------------------------------
    ** Set ATT flag to FALSE so we don't attempt to perform multiple DETACHs
    ** --------------------------------------------------------------------- */
    if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2 ||
        mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
    {
      mm_system_information.ATT = FALSE;
    }
#ifdef FEATURE_LTE
    if(message->cmd.hdr.message_id  != (int) MMR_PS_DETACH_REQ)
#endif
    {
    mm_handle_message( message );
    }
    if((is_gsm_mode())&&((mm_state == MM_CONNECTION_ACTIVE) ||
       (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION))&&
        mm_reg_waiting_for_reg_cnf)
    {
        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
        mm_send_mmr_reg_cnf();
        mm_reg_waiting_for_reg_cnf = FALSE;
        mm_ready_to_send_reg_cnf   = FALSE;
      }
    else if (mm_serv_change_pending)
    {
      mm_change_service_domain();
      mm_serv_change_pending = FALSE;
    }
  }
            /* If Gs association is not there, no point in doing combined, instead the behaviour should
               be like the one in NMO2.
          */
  else if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) &&
#ifdef FEATURE_LTE
          (message->cmd.hdr.message_id  == (int) MMR_REG_REQ) &&
#endif
          (message->cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
          (mm_gs_association_established == FALSE))
  {    
    mm_handle_message( message );
  }            
  else if ((is_gsm_mode()) &&
           ((mm_state == MM_WAIT_FOR_RR_CONNECTION_IMSI_DETACH) ||
            (mm_state == MM_IMSI_DETACH_INITIATED)              ||
            (gmm_substate == GMM_SUSPENDED) ||
            (gmm_is_suspended)))
  {
    gmm_gprs_detach_pending = TRUE;
  }
#ifdef FEATURE_REL6_PS_FOP
  else if ((mm_timer_status [ TIMER_T3340 ] == TIMER_ACTIVE) &&
      (gmm_ps_fop_resp == PS_FOP_NW_NOT_GRANTED))
  {
    /**************************************************************
    *24.008 4.7.1.9 The MS shall start the timer T3340  when the  *
    * network indicates "no follow-on proceed" in the             *
    *ROUTING AREA UPDATE ACCEPT or ATTACH ACCEPT message.         *
    *if the MS has signalling pending, then it shall request a    *
    * new PS signalling connection for further signalling         *
    **************************************************************/
    gmm_gprs_detach_pending = TRUE;
    MSG_HIGH_DS_0( MM_SUB,"=MM= Detach is blocked - T3340 is running");
  }
#endif
else if ((message->cmd.hdr.message_set == MS_MM_REG) &&
      ((message->cmd.hdr.message_id  == (int) MMR_STOP_MODE_REQ) ||
       (message->cmd.hdr.message_id  == (int) MMR_REG_REQ)
#ifdef FEATURE_NAS_ECALL
        ||
       (message->cmd.hdr.message_id  == (int) MMR_ECALL_MODE_IND)
#endif
#ifdef FEATURE_LTE
            || (message->cmd.hdr.message_id  == (int) MMR_PS_DETACH_REQ)
#endif
       ))
  {
#ifdef FEATURE_LTE
    /*
    ** For MMR_PS_DETACH_REQ message set the detach type to gprs detach
    */
    if(message->cmd.hdr.message_id  == (int) MMR_PS_DETACH_REQ)
    {
      gmm_detach_type = GMM_GPRS_DETACH;
    }
    else
#endif
    if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
    {
      if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
      {
        /* If CS is INVALID do a GPRS Detach only */
        if ((!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
#ifdef FEATURE_DSAC
            ||
            (mm_check_if_cs_cell_access_barred(mm_get_cs_cell_access(PROCEDURE_NONE)) == TRUE)            
#endif        
           )
        {
          mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                          SYS_SRV_DOMAIN_NO_SRV,
                                          FALSE,
                                          SYS_SRV_STATUS_LIMITED,
                                          TRUE );        
          gmm_detach_type = GMM_GPRS_DETACH;
        }
        else
        {
          /* If Gs association is not there, no point in doing combined, instead the behaviour should
               be like the one in NMO2.
               If ISR is ON and SG association is established then perform combined detach even if GS
               association is not established.
          */
          if(((mm_gs_association_established == FALSE) 
#ifdef FEATURE_LTE
             && !((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
               (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_NOT_REQ))
#endif
             )
#ifdef FEATURE_NAS_ECALL
              ||
             ((mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY) &&
              (mm_state != MM_IDLE))
#endif
            )
          {
            gmm_detach_type = GMM_GPRS_DETACH;
            mm_handle_message( message );
          }            
          else
          {
            gmm_detach_type = GMM_COMBINED_GPRS_IMSI_DETACH;
          }
        }
      }
      else
      {
        gmm_detach_type = GMM_GPRS_DETACH;
      }
    }
    else
    {
      if ((message->cmd.hdr.message_id  == (int) MMR_REG_REQ) &&
          (message->cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
      {
        gmm_detach_type = GMM_MO_IMSI_DETACH;
      }
      else
      {
        gmm_detach_type = GMM_GPRS_DETACH;
      }
    }

    if(gmm_detach_type == GMM_COMBINED_GPRS_IMSI_DETACH || gmm_detach_type == GMM_MO_IMSI_DETACH )
    {
      mm_initiate_lu_upon_entry_into_new_cell = FALSE;
      mm_lu_pended_nmo_unknown = FALSE;
    } 

    if ((message->cmd.hdr.message_id == (int) MMR_STOP_MODE_REQ) &&
        ((message->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_LPM) ||
         (message->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_POWER_OFF) ||
         (message->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_OFFLINE)   ||
         (message->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_MODE_CHANGE) ||
#ifdef FEATURE_DUAL_SIM
         (message->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_MODE_CAP_CHANGE) ||
#endif
         (message->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_IRAT)))
    {
      gmm_power_down_active = TRUE;
    }
    else
    {
      gmm_power_down_active = FALSE;
    }

    /* --------------------------------------------
    ** Send a DETACH REQUEST message to the network
    ** -------------------------------------------- */
    gmm_send_mo_detach_request_message( gmm_detach_type, gmm_power_down_active );
    if(t3321_timeout_counter ==  0)
    {
      mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_START,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
    }
    else
    {
      mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_RESEND,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
    }
#ifdef FEATURE_GSM_GPRS
    gmm_rau_is_required     = FALSE;
#endif /*FEATURE_GSM_GPRS*/
    /* GS association is removed when DETACH is done */
    MSG_MED_DS_0( MM_SUB, "=MM= GS association is removed when DETACH is performed");
    mm_gs_association_established = FALSE;
    /* Storing the NMO as communication with NW is started on the PS */
    gmm_previous_network_op_mode = mm_serving_plmn.network_op_mode;

    /* UN Marking the pending rau and attach Flags as DETACH procedure has been triggered */
    MSG_MED_DS_0( MM_SUB, "=MM= UN Marking pending rau and attach flags when DETACH is performed" );
    gmm_gprs_attach_pending = FALSE;
    gmm_ra_update_pending = FALSE;

    
    #ifdef FEATURE_DSAC
    /* Updating the mm_last_procedure_to_update_registration flag */
    if (gmm_detach_type == GMM_GPRS_DETACH)
    {
       mm_last_procedure_to_update_registration &= ~MM_COMBINED;
       mm_last_procedure_to_update_registration &= ~MM_RAU;
    }
    else if ( gmm_detach_type == GMM_COMBINED_GPRS_IMSI_DETACH )
    {
       /* unsetting all the bits */
       mm_last_procedure_to_update_registration = MM_NONE;
    }
    else /* gmm_detach_type is GMM_MO_IMSI_DETACH */
    {
       mm_last_procedure_to_update_registration &= ~MM_COMBINED;
       mm_last_procedure_to_update_registration &= MM_RAU;
    }
    #endif

    gmm_gprs_detach_pending      = FALSE;
    gmm_detach_after_rau_pending = FALSE;
    mm_is_gmm_moved_dereg_due_to_csreg = FALSE;
    gmm_delete_gprs_forbidden_rai();

    mm_stop_timer(TIMER_T3340);
    mm_stop_timer(TIMER_PS_CONN_REL);

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
    mm_pending_lu_type = MM_LU_CAUSE_RESERVED;
#endif

    if ((message->cmd.hdr.message_id == (int) MMR_STOP_MODE_REQ) &&
        ((message->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_MODE_CHANGE) ||
#ifdef FEATURE_DUAL_SIM
         (message->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_MODE_CAP_CHANGE) ||
#endif
         (message->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_IRAT) ||
         (message->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_POWER_OFF) ||
         (message->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_OFFLINE) ||
         (message->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_LPM)))
    {
      /* --------------------------------------------------------------
      ** If the MS is to be switched off, the MS shall try for a period
      ** of 5 seconds to send the DETACH REQUEST message
      ** -------------------------------------------------------------- */
      if(mm_timer_status[TIMER_POWERDOWN] == TIMER_ACTIVE)
      {
        MSG_HIGH_DS_0( MM_SUB,"=MM= TIMER_POWERDOWN already running, don't restart it\n");
      }
      else
      {
        mm_start_timer( TIMER_POWERDOWN, DEFAULT_TIMEOUT );
      }


    }
    else
    {
      /* --------------------------------------------------------------------------
      ** If the MS is not switched off and the MS is in the state GMM_REGISTERED,
      ** timer T3321 shall be started after the DETACH REQUEST message has been sent
      ** --------------------------------------------------------------------------- */
      mm_start_timer( TIMER_T3321, DEFAULT_TIMEOUT );
      t3321_timeout_counter = 0;
      MSG_HIGH_DS_1( MM_SUB, "=MM= T3321 timeout counter = %d", t3321_timeout_counter );
    }

    /* ----------------------------------------------------------------------------
    ** If the detach type IE value indicates "IMSI Detach" the MS shall enter
    ** GMM-REGISTERED.IMSI-DETACH-INITIATED, otherwise the MS shall enter the state
    ** GMM-DEREGISTERED-INITIATED
    ** ---------------------------------------------------------------------------- */
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)    
    if(!(gmm_rat_chng_pended_msg_valid &&
#ifdef FEATURE_MODEM_HEAP
        (gmm_rat_chng_pended_msg != NULL) &&
#endif
        (byte) (((byte *) gmm_rat_chng_pended_msg + sizeof(IMH_T))[0] & 0x0F) ==  PD_GMM &&
        (byte) (((byte *) gmm_rat_chng_pended_msg + sizeof(IMH_T))[1]) == GMM_DETACH_REQUEST))
    {
      if ((gmm_detach_type == GMM_MO_IMSI_DETACH) && (gmm_state != GMM_REGISTERED_INITIATED))
      {
        gmm_set_state( GMM_REGISTERED );
        gmm_set_substate( GMM_IMSI_DETACH_INITIATED );
        mm_possible_imsi_attach = TRUE;
      }
      else
      {
        gmm_set_state( GMM_DEREGISTERED_INITIATED );
      }
    }
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
  }
  else
  {
    MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message %d", message->cmd.hdr.message_id, 0,0 );
  }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
  gmm_is_prev_mode_connected = FALSE;
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
} /* end gmm_initiate_gprs_detach() */
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

/*===========================================================================

FUNCTION GMM_PROCESS_DETACH_REQUEST_CAUSE

DESCRIPTION
  Function to process MT Detach request cause value and update context accordingly

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_process_detach_request_cause( gmm_mt_detach_request_msg_T *detach_request_msg_p )
{

      /* ------------------------------------------------------------------
    ** If the detach type IE indicates "re-attach not required", the,
    ** depending on the received cause code, MS takes the appropriate
    ** actions.
    ** ------------------------------------------------------------------ */
    if (detach_request_msg_p->detach_type == GMM_REATTACH_NOT_REQUIRED)
    {
#if defined(FEATURE_LTE)
      /* If LTE is present, update EMM state and parameters.
         Note: emm_multimode_complete_update must be also called after GMM/MM    
         processing to wrap up EMM update */
      if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
      {
        emm_update_multimode_mt_detach(
                      (lte_nas_emm_cause_type)detach_request_msg_p->gmm_cause);
      }
#endif
      switch (detach_request_msg_p->gmm_cause)
      {
        case IMSI_UNKNOWN_IN_HLR:
          /* ----------------------------------------------------------------
          ** The MS shall set the update status to U3 ROAMING NOT ALLOWED and
          ** shall delete any TMSI, LAI and ciphering key sequence number.
          ** The SIM shall be considered as invalid for non-GPRS services
          ** until switching off or the SIM is removed.
          ** ---------------------------------------------------------------- */
          mm_delete_lu_status( ROAMING_NOT_ALLOWED );

          mm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_INVALID,
                                          SYS_SRV_DOMAIN_NO_SRV,
                                          FALSE,
                                          SYS_SRV_STATUS_LIMITED,
                                          TRUE );
#ifdef FEATURE_DSAC
             mm_last_procedure_to_update_registration = MM_NONE;
#endif
          break;

       case ILLEGAL_MS:
       case ILLEGAL_ME:
       case GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
          /* ------------------------------------------------------------------
          ** The MS shall set the GPRS update status to GU3 ROAMING NOT ALLOWED
          ** (and shall store it according to clause 4.1.3.2) and shall delete
          ** any P-TMSI, P-TMSI signature, RAI and GPRS ciphering key sequence
          ** number.  The new GMM state is GMM-DEREGISTERED.  The SIM shall be
          ** considered as invalid for GPRS services until switching off or the
          ** SIM is removed.
          ** ------------------------------------------------------------------ */
          gmm_delete_ps_location_information();
          gmm_delete_gprs_cksn();

          gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;

          mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                              gmm_stored_ptmsi_signature,
                                              gmm_stored_rai,
                                              gmm_update_status );

          gmm_set_substate( GMM_LIMITED_SERVICE );

          if (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)
          {
            /* -----------------------------------------------------------------
            ** A GPRS MS operating in MS operation mode A or B shall in addition
            ** set the update status to U3 ROAMING NOT ALLOWED, shall delete
            ** any TMSI, LAI and ciphering key sequence number.
            ** ----------------------------------------------------------------- */
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
#ifdef FEATURE_NAS_FREE_FLOAT
            if (is_gsm_mode())
            {
              MSG_HIGH_DS_0(MM_SUB, "=MM= After receiving Ack for Detach Accept send ILLEGAL subscription indication to GSDI");
            }
            else
#endif
            {
              mmsim_mark_sim_as_illegal();
            }
          }
          else
          {
#ifdef FEATURE_NAS_FREE_FLOAT           
            if ((is_gsm_mode()) && (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_CS_INVALID) ) 
            {
              gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_PS_INVALID,
                                               SYS_SRV_DOMAIN_NO_SRV,
                                               FALSE,
                                               SYS_SRV_STATUS_LIMITED,
                                               TRUE );
            } 
            else 
#endif 
            {
              gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_PS_INVALID,
                                               SYS_SRV_DOMAIN_NO_SRV,
                                               FALSE,
                                               SYS_SRV_STATUS_LIMITED,
                                               TRUE );
            }
          }
#ifdef FEATURE_DSAC
          mm_last_procedure_to_update_registration = MM_NONE;
#endif

          break;

        case GPRS_SERVICES_NOT_ALLOWED:
          /* ------------------------------------------------------------------
          ** The MS shall set the GPRS update status to GU3 ROAMING NOT ALLOWED
          ** (and shall store it according to clause 4.1.3.2) and shall delete
          ** any P-TMSI, P-TMSI signature, RAI and GPRS ciphering key sequence
          ** number.  The new GMM state is GMM-DEREGISTERED.  The SIM shall be
          ** considered as invalid for GPRS services until switching off or the
          ** SIM is removed.
          ** ------------------------------------------------------------------ */
          gmm_delete_ps_location_information();
          gmm_delete_gprs_cksn();
          mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                              gmm_stored_ptmsi_signature,
                                              gmm_stored_rai,
                                              gmm_update_status );
          gmm_set_substate( GMM_LIMITED_SERVICE );
          gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;

          if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) 
          {
            mm_serving_plmn.ms_op_mode = SYS_SRV_DOMAIN_CS_ONLY;
            if((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
               (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
            {
              mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain =
                SYS_SRV_DOMAIN_CS_ONLY; 
            }
            if((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
               (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
            {
              /* --------------------------------------------------------------------
              ** A GPRS MS operating in MS operation mode A or B in network operation
              ** mode I, which receives a DETACH REQUEST message with detach type
              ** indicating "re-attach not required" and no cause code, is only
              ** detached for GPRS services in the network.
              ** -------------------------------------------------------------------- */
              gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_PS_INVALID,
                                               SYS_SRV_DOMAIN_CS_ONLY,
                                               FALSE,
                                               SYS_SRV_STATUS_SRV,
                                               TRUE );

#ifdef FEATURE_NAS_REL10
              if(mm_is_per_ms_t3212_present)
              {
                if ((mm_stored_per_ms_t3212_value != 0 ) &&
                    (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                {

                  mm_start_timer( TIMER_T3212,
                                  mm_stored_per_ms_t3212_value);
                }
              }
              else
#endif
              if ((mm_system_information.T3212_timer_value != 0 ) &&
                  (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
              {
                mm_start_timer( TIMER_T3212,
                                mm_system_information.T3212_timer_value );
              }

            }
            else
            {
 #ifdef FEATURE_NAS_FREE_FLOAT           
              if ((is_gsm_mode()) && (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_CS_INVALID) ) 
              {
                gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_PS_INVALID,
                                                 SYS_SRV_DOMAIN_NO_SRV,
                                                 FALSE,
                                                 SYS_SRV_STATUS_LIMITED,
                                                 TRUE );
              } 
              else 
#endif 
              {
                gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_PS_INVALID,
                                                 SYS_SRV_DOMAIN_NO_SRV,
                                                 FALSE,
                                                 SYS_SRV_STATUS_LIMITED,
                                                 TRUE );
              }
            }
          }
          else
          {
#ifdef FEATURE_NAS_FREE_FLOAT           
            if ((is_gsm_mode()) && (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_CS_INVALID) ) 
            {
              gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_PS_INVALID,
                                               SYS_SRV_DOMAIN_NO_SRV,
                                               FALSE,
                                               SYS_SRV_STATUS_LIMITED,
                                               TRUE );
            } 
            else 
#endif 
            {
              gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_PS_INVALID,
                                               SYS_SRV_DOMAIN_NO_SRV,
                                               FALSE,
                                               SYS_SRV_STATUS_LIMITED,
                                               TRUE );
            }
          }
#ifdef FEATURE_DSAC
          mm_last_procedure_to_update_registration &= ~MM_RAU;
          if(mm_last_procedure_to_update_registration & MM_COMBINED)
          {
            mm_last_procedure_to_update_registration |= MM_LU;
            mm_last_procedure_to_update_registration &= ~MM_COMBINED;
          }
#endif

          break;

        case PLMN_NOT_ALLOWED:
        case LA_NOT_ALLOWED:
        case NATIONAL_ROAMING_NOT_ALLOWED:
        case NO_SUITABLE_CELLS_IN_LA:
          /* -------------------------------------------------------------
          ** The MS shall delete any RAI, P-TMSI, P-TMSI signature and
          ** GPRS ciphering key sequence number, shall set the GPRS update
          ** status to GU3 ROAMING NOT ALLOWED and enter the state
          ** GMM-DEREGISTERED.
          ** ------------------------------------------------------------- */
          if (detach_request_msg_p->gmm_cause == PLMN_NOT_ALLOWED)
          {
            gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;

            mm_add_plmn_to_fplmn_list( mm_serving_plmn.info.plmn);
          }
          else
          {
            gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
          }

          gmm_set_substate( GMM_LIMITED_SERVICE );
          /**************************************************************
          *24.008 4.7.1.9 The MS shall start the timer T3340  if the MS *
          * receives any of the reject cause values #11,#12,#13 or #15  *
          **************************************************************/
          if (is_umts_mode( ))
          {
            mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);
          }

          gmm_delete_ps_location_information();
          gmm_delete_gprs_cksn();
          mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                              gmm_stored_ptmsi_signature,
                                              gmm_stored_rai,
                                              gmm_update_status );

          if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
          {
            /* ------------------------------------------------------------
            ** If the MS is operating in MS operation mode A , the MS shall
            ** in addition set the update status to U3 ROAMING NOT ALLOWED,
            ** shall delete any TMSI, LAI and ciphering key sequence number
            ** and shall reset the location update attempt counter.
            ** ------------------------------------------------------------ */
            mm_delete_lu_status( ROAMING_NOT_ALLOWED );
            mm_clear_lu_attempt_counter( );
          }

          /* -------------------------------------------------------------
          ** The MS shall store the LAI or the PLMN identity in the
          ** appropriate forbidden list, i.e., in the "forbidden PLMN
          ** list" for cause #11, in the list of "forbidden location areas
          ** for regional provision of service" for cause #12 or in the
          ** list of "forbidden location areas for roaming" for cause #13.
          ** If #11 #13 or #15 was received, the MS shall perform a PLMN
          ** selection instead of a cell selection.
          ** ------------------------------------------------------------- */
          if (detach_request_msg_p->gmm_cause == LA_NOT_ALLOWED)
          {
#ifdef FEATURE_FORBID_LAI_UPON_REGIONAL_ROAMING_REJECT
            /* ---------------------------------------------------------
            ** This FEATURE was introduced to support a customer request
            ** for a non-spec compliant PLMN selection on receipt of
            ** reject cause LA_NOT_ALLOWED
            ** --------------------------------------------------------- */
            mm_add_forbidden_national_roaming( &gmm_current_lai );
#else
            mm_add_forbidden_regional_roaming( &gmm_current_lai );
#endif
          }
          else if ((detach_request_msg_p->gmm_cause == NATIONAL_ROAMING_NOT_ALLOWED) ||
                   (detach_request_msg_p->gmm_cause == NO_SUITABLE_CELLS_IN_LA))
          {
            mm_add_forbidden_national_roaming( &gmm_current_lai );
          }
          if(detach_request_msg_p->gmm_cause == NO_SUITABLE_CELLS_IN_LA)
          {
            /* add LAI in LAI reject list */ 
            mm_add_lai_in_reject_list(&gmm_current_lai);
          }

          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           SYS_SRV_DOMAIN_NO_SRV,
                                           TRUE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE );
#ifdef FEATURE_DSAC
           mm_last_procedure_to_update_registration = MM_NONE;
#endif         
          break;

        case GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN:
          /* -------------------------------------------------------------
          ** The MS shall delete any RAI, P-TMSI, P-TMSI signature and
          ** GPRS ciphering key sequence number, shall set the GPRS update
          ** status to GU3 ROAMING NOT ALLOWED and enter the state
          ** GMM-DEREGISTERED.
          ** ------------------------------------------------------------- */
          gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
          gmm_set_substate( GMM_LIMITED_SERVICE );          
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
          if (reg_sim_per_subs_is_ehplmn(mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
          if (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), mm_serving_plmn.info.plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
          if (reg_sim_is_ehplmn(mm_serving_plmn.info.plmn) )
#else
          if (sys_plmn_match(reg_sim_read_hplmn(), mm_serving_plmn.info.plmn) )
#endif
#endif
          {
            gmm_update_gprs_forbidden_rai(gmm_previous_registration_serving_rai);
          }
          else
          {
            reg_sim_add_plmn_to_gprs_fplmn_list( mm_serving_plmn.info.plmn
#ifdef FEATURE_DUAL_SIM
                                               ,(sys_modem_as_id_e_type)mm_sub_id
#endif
                                             );
          }
#ifdef FEATURE_LTE 
          if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS))
          {
            if (is_umts_mode())
            {
              mm_send_rrc_lte_reselection_status_ind();
            }
            else if (is_gsm_mode())
            {
              mm_send_rr_lte_reselection_status_ind();
            }
          }
#endif 
          gmm_delete_ps_location_information();
          gmm_delete_gprs_cksn();
          mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                              gmm_stored_ptmsi_signature,
                                              gmm_stored_rai,
                                              gmm_update_status );

          if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
              (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
              ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
               (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)))
          {
            /* --------------------------------------------------------------------
            ** A GPRS MS operating in MS operation mode A or B in network operation
            ** mode I, is still IMSI attached for CS services in the network.
            ** -------------------------------------------------------------------- */
            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                             SYS_SRV_DOMAIN_CS_ONLY,
                                             FALSE,
                                             SYS_SRV_STATUS_SRV,
                                             TRUE );

#ifdef FEATURE_NAS_REL10
            if(mm_is_per_ms_t3212_present)
            {
                if ((mm_stored_per_ms_t3212_value != 0 ) &&
                    (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                {

                  mm_start_timer( TIMER_T3212,
                                  mm_stored_per_ms_t3212_value);
                }
            }
            else
#endif
            if ((mm_system_information.T3212_timer_value != 0 ) &&
                (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
            {
              mm_start_timer( TIMER_T3212,
                              mm_system_information.T3212_timer_value );

            }

          }
          else
          {
            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                             SYS_SRV_DOMAIN_NO_SRV,
                                             FALSE,
                                             SYS_SRV_STATUS_LIMITED,
                                             TRUE );
          }
#ifdef FEATURE_DSAC
          mm_last_procedure_to_update_registration &= ~MM_RAU;
          if (mm_last_procedure_to_update_registration & MM_COMBINED)
          {
            mm_last_procedure_to_update_registration |= MM_LU;
            mm_last_procedure_to_update_registration &= ~MM_COMBINED;
          }

#endif

          break;

#ifdef FEATURE_FEMTO_CSG
       case  CSG_NOT_AUTHORIZED:
        
         if(mm_serving_csg_id != SYS_CSG_ID_INVALID)
         {
           gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;  
           gmm_set_substate( GMM_LIMITED_SERVICE );
           mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                              gmm_stored_ptmsi_signature,
                                              gmm_stored_rai,
                                              gmm_update_status );
           gprs_attach_attempt_counter = 0;

           if (is_umts_mode( ))
           {
             mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);
           }
           mm_delete_csg_from_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info);
#ifdef FEATURE_DSAC
           mm_last_procedure_to_update_registration = MM_NONE;
#endif         
         }
         else
         {
           /* ---------------------------------------------------------------
           ** Cause #25 will be treated as abnormal
           ** --------------------------------------------------------------- */
           if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
               ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
               (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)))
           {
             /* --------------------------------------------------------------------
             ** A GPRS MS operating in MS operation mode A or B in network operation
             ** mode I, which receives a DETACH REQUEST message with detach type
             ** indicating "re-attach not required" and no cause code, is only
             ** detached for GPRS services in the network.
             ** -------------------------------------------------------------------- */
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
 #ifdef FEATURE_DSAC
           mm_last_procedure_to_update_registration &= ~MM_RAU;
           if (mm_last_procedure_to_update_registration & MM_COMBINED)
           {
             mm_last_procedure_to_update_registration |= MM_LU;
             mm_last_procedure_to_update_registration &= ~MM_COMBINED;
           }
 #endif
         }
         break;
#endif  /*FEATURE_FEMTO_CSG*/

       default:
          /* ---------------------------------------------------------------
          ** Other cause values shall not impact the update status.  Further
          ** actions of the MS are implementation dependent.
          ** --------------------------------------------------------------- */
          if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
              (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
              ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
              (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)))
          {
            /* --------------------------------------------------------------------
            ** A GPRS MS operating in MS operation mode A or B in network operation
            ** mode I, which receives a DETACH REQUEST message with detach type
            ** indicating "re-attach not required" and no cause code, is only
            ** detached for GPRS services in the network.
            ** -------------------------------------------------------------------- */
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
#ifdef FEATURE_DSAC
          mm_last_procedure_to_update_registration &= ~MM_RAU;
          if (mm_last_procedure_to_update_registration & MM_COMBINED)
          {
            mm_last_procedure_to_update_registration |= MM_LU;
            mm_last_procedure_to_update_registration &= ~MM_COMBINED;
          }

#endif
          break;
      } /* end switch (detach_request.gmm_cause) */
#if defined(FEATURE_LTE)
      if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
      {
        emm_multimode_complete_update();
      }
#endif

    }

}


/*===========================================================================

FUNCTION GMM_PROCESS_DETACH_REQUEST_MSG

DESCRIPTION
  Function to

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_process_detach_request_msg( mm_cmd_type *message )
{
  gmm_mt_detach_request_msg_T detach_request_msg;
  sm_cmd_type                 *sm_cmd;
  mmr_service_ind_s_type     mmr_service_ind;

  /* Set the Default cause in the Service indication primitive */
  mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (gmm_valid_mt_detach_request_message( message, &detach_request_msg ))
  {
    gmm_processed_mt_imsi_detach_request = FALSE;
    mmsend_nas_reg_event_packet(PROC_NAS_MT_DETACH_EVENT,PROC_START,PROC_END_NO_REASON,detach_request_msg.detach_type,detach_request_msg.gmm_cause);
#ifdef FEATURE_GSM_GPRS
    if (is_gsm_mode())
    {
      gmmllc_gmm_trigger_llc_event
        (GMM_TRIGGER_FORCE_TO_STANDBY,detach_request_msg.force_to_standby,0) ;
    }
#endif

#ifdef FEATURE_NAS_REL10
    if(mm_timer_status[TIMER_T3346]== TIMER_ACTIVE)
    {
      mm_stop_timer(TIMER_T3340);
    }
#endif
    mm_stop_timer(TIMER_PS_CONN_REL);

    if (gmm_state == GMM_REGISTERED_INITIATED)
    {
      if(detach_request_msg.detach_type == GMM_REATTACH_NOT_REQUIRED)
    {
    /*******************************************************************************************
       * spec 24.008 4.7.3.1.5 
       * h) Detach Pprocedure collision:
       *       If the MS receives a DETACH REQUEST message from the network in state GMM-REGISTERED-INITIATED
       *       with detach type of detach '"re-attach not required" and no cause code, or "re-attach not required"' and
       *       the cause code is not #2 "IMSI unknown in HLR", the GPRS detach procedure shall be progressed and the
       *       GPRS attach procedure shall be aborted. Otherwise the GPRS attach procedure shall be progressed and 
       *       the DETACH REQUEST message shall be ignored.
    *********************************************************************************************/
#ifdef FEATURE_NAS_REL11
      if((detach_request_msg.gmm_cause_present && detach_request_msg.gmm_cause == 0) ||
          (detach_request_msg.gmm_cause != IMSI_UNKNOWN_IN_HLR))
#endif
      {
        /* GS association is removed when MT DETACH is initiated */
        MSG_MED_DS_0( MM_SUB, "=MM= GS association is removed when MT DETACH is initiated" );
        mm_gs_association_established = FALSE;
    
      /* ----------------------------------------------------------------
      ** If the MS receives a DETACH REQUEST message from the network
      ** in state GMM-REGISTERED-INITIATED with type of detach "re-attach
      ** not required", the GPRS detach procedure shall be progressed
      ** and the GPRS attach procedure shall be aborted.  Otherwise the
      ** GPRS attach procedure shall be progressed and the DETACH REQUEST
      ** message shall be ignored. (see 24.008 4.7.3.1.5)
      ** ---------------------------------------------------------------- */

        gmm_send_mt_detach_accept_message();
        mmsend_nas_reg_event_packet(PROC_NAS_MT_DETACH_EVENT,PROC_END,PROC_END_ACCEPT,detach_request_msg.detach_type,detach_request_msg.gmm_cause);

        gmm_set_state( GMM_DEREGISTERED );

        mm_stop_timer(TIMER_T3310);

        gmm_process_detach_request_cause(&detach_request_msg);
        if (mm_reg_waiting_for_reg_cnf)
        {
          mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
          mm_send_mmr_reg_cnf();
          mm_reg_waiting_for_reg_cnf = FALSE;
          mm_ready_to_send_reg_cnf   = FALSE;
        }
        else
        {
          mmr_service_ind.service_state = mm_serving_plmn.info;

          MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service");
          mm_send_mmr_service_ind( &mmr_service_ind );
        }
      }
    }
      else if(detach_request_msg.detach_type == GMM_REATTACH_REQUIRED)
      {
        mm_send_sm_unblock_all_apn();
      }
    }
    else if (gmm_state == GMM_DEREGISTERED_INITIATED)
    {
      /* GS association is removed when MT DETACH is initiated */
      MSG_MED_DS_0( MM_SUB, "=MM= GS association is removed when MT DETACH is initiated");
      mm_gs_association_established = FALSE;
    
      /* ------------------------------------------------------------------
      ** If the MS receives a DETACH REQUEST message before the MS
      ** initiated GPRS detach procedure has been completed, a DETACH
      ** ACCEPT message shall be sent to the network (see 24.008 4.7.4.1.4)
      ** ------------------------------------------------------------------ */
      gmm_send_mt_detach_accept_message();
      mmsend_nas_reg_event_packet(PROC_NAS_MT_DETACH_EVENT,PROC_END,PROC_END_ACCEPT,detach_request_msg.detach_type,detach_request_msg.gmm_cause);
#ifdef FEATURE_NAS_REL11
      if(detach_request_msg.detach_type != GMM_MT_IMSI_DETACH)
#endif     
      {
        sm_cmd = sm_get_cmd_buf();

        if (sm_cmd == NULL)
        {
          MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
        }

        sm_cmd->header.message_set = MS_GMM_SM;
        sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
        if (is_wcdma_mode() &&
            (detach_request_msg.detach_type == GMM_REATTACH_NOT_REQUIRED) &&
            (detach_request_msg.gmm_cause == GPRS_SERVICES_NOT_ALLOWED))
        {
            sm_cmd->cmd.gmm_gprs_rel_ind.cause = 
                mm_convert_gmm_to_sm_cause(detach_request_msg.gmm_cause);			
        }
        else
        {
            sm_cmd->cmd.gmm_gprs_rel_ind.cause = NO_GPRS_CONTEXT;
        }        
#ifdef FEATURE_DUAL_SIM
        sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

        sm_put_cmd( sm_cmd );

        gmm_set_state( GMM_DEREGISTERED );
      }

      if (detach_request_msg.detach_type == GMM_REATTACH_REQUIRED) 
      {
        if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)&& 
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
          ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
          (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)))
        {    
          /* --------------------------------------------------------------------
          ** A GPRS MS operating in MS operation mode A or B in network operation
          ** mode I, which receives a DETACH REQUEST message with detach type
          ** indicating "re-attach required" or "re-attach not required" and no cause code, 
          ** is only detached for GPRS services in the network.
          ** -------------------------------------------------------------------- */
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
      }
#ifdef FEATURE_NAS_REL11 
      if (detach_request_msg.detach_type == GMM_MT_IMSI_DETACH) 
      {
         mm_location_information.location_update_status = NOT_UPDATED;
         mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                        SYS_SRV_DOMAIN_NO_SRV,
                                        FALSE,
                                        SYS_SRV_STATUS_LIMITED,
                                        TRUE );
      }
#endif
    
      gmm_process_detach_request_cause(&detach_request_msg);

      /* Set GMM state and substate to GMM NULL 
       * while processing MT DETACH REQUEST message */
       
#ifdef FEATURE_NAS_REL11
      if(detach_request_msg.detach_type != GMM_MT_IMSI_DETACH)
#endif
      {
        if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)
        {
           gmm_set_state( GMM_NULL );
           gmm_set_substate( GMM_NULL_SUBSTATE );
        }
        if((is_gsm_mode())&&((mm_state == MM_CONNECTION_ACTIVE) ||
           (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION))&&
             mm_reg_waiting_for_reg_cnf)
        {
          mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
          mm_send_mmr_reg_cnf();
          mm_reg_waiting_for_reg_cnf = FALSE;
          mm_ready_to_send_reg_cnf = FALSE;
        } 
        else
        { 
          mmr_service_ind.service_state = mm_serving_plmn.info;

          MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND, Limited Service" );
          mm_send_mmr_service_ind( &mmr_service_ind );
        }
      }
#ifdef FEATURE_DSAC      
      if (detach_request_msg.detach_type == GMM_MT_IMSI_DETACH)
      {
        mm_last_procedure_to_update_registration &=~MM_LU;
      }
#endif
      if(detach_request_msg.detach_type == GMM_REATTACH_REQUIRED)
      {
       mm_send_sm_unblock_all_apn();        
      }

    }
    else if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
    {
      /* ----------------------------------------------------
      ** Routing area updating and detach procedure collision
      ** ---------------------------------------------------- */
      if ((detach_request_msg.detach_type == GMM_REATTACH_REQUIRED) ||
          (detach_request_msg.detach_type == GMM_REATTACH_NOT_REQUIRED))
      {
        /* GS association is removed when MT DETACH is initiated */
        MSG_MED_DS_0( MM_SUB, "=MM= GS association is removed when MT DETACH is initiated");
        mm_gs_association_established = FALSE;
      
        /* -------------------------------------------------------------------
        ** If the MS receives a DETACH REQUEST message before the routing area
        ** updating procedure has been completed, the routing area updating
        ** procedure shall be aborted and the GPRS detach procedure shall be
        ** progressed.
        ** ------------------------------------------------------------------- */
        gmm_send_mt_detach_accept_message();
        mmsend_nas_reg_event_packet(PROC_NAS_MT_DETACH_EVENT,PROC_END,PROC_END_ACCEPT,detach_request_msg.detach_type,detach_request_msg.gmm_cause);
        gmm_set_state( GMM_DEREGISTERED );

        mm_stop_timer(TIMER_T3330);

#ifdef FEATURE_GPRS_PS_HANDOVER 
        gmm_psho_status = GMM_PSHO_NONE;
#endif

        sm_cmd = sm_get_cmd_buf();

        if (sm_cmd == NULL)
        {
          MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
        }

        sm_cmd->header.message_set = MS_GMM_SM;
        sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
        if (is_wcdma_mode() &&
           (detach_request_msg.detach_type == GMM_REATTACH_NOT_REQUIRED) &&
            (detach_request_msg.gmm_cause == GPRS_SERVICES_NOT_ALLOWED))
        {
            sm_cmd->cmd.gmm_gprs_rel_ind.cause = 
                mm_convert_gmm_to_sm_cause(detach_request_msg.gmm_cause);
        }
        else
        {
            sm_cmd->cmd.gmm_gprs_rel_ind.cause = NO_GPRS_CONTEXT;
        }
#ifdef FEATURE_DUAL_SIM
        sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

        sm_put_cmd( sm_cmd );

        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND");

        if (detach_request_msg.detach_type == GMM_REATTACH_REQUIRED) 
        {
          if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)&& 
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
          ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
          (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)))
          {    
            /* --------------------------------------------------------------------
            ** A GPRS MS operating in MS operation mode A or B in network operation
            ** mode I, which receives a DETACH REQUEST message with detach type
            ** indicating "re-attach required" or "re-attach not required" and no cause code, 
            ** is only detached for GPRS services in the network.
            ** -------------------------------------------------------------------- */
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
        }

        gmm_process_detach_request_cause(&detach_request_msg);

        mmr_service_ind.service_state = mm_serving_plmn.info;
        if(detach_request_msg.detach_type == GMM_REATTACH_REQUIRED)
        {
          mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN; 
        }
        mm_send_mmr_service_ind( &mmr_service_ind );

        if (detach_request_msg.detach_type == GMM_REATTACH_REQUIRED)
        {
          /* ----------------------------------------------------------------
          ** The MS shall, after the completion of the GPRS detach procedure,
          ** initiate a GPRS attach procedure.
          ** ---------------------------------------------------------------- */
          mm_send_sm_unblock_all_apn();
#ifdef FEATURE_NAS_FREE_FLOAT
          if (is_gsm_mode())
          {      
             gmm_gprs_attach_pending = TRUE;
          }
          else
#endif
          {
          gmm_initiate_gprs_attach();
        }

      }

      }
      else /* detach_type == IMSI_DETACH */
      {
        /* -------------------------------------------------------------------
        ** If the MS receives a DETACH REQUEST message before the routing area
        ** updating procedure has been completed, the routing area updating
        ** procedure shall be progressed, i.e., the DETACH REQUEST message
        ** shall be ignored.
        ** ------------------------------------------------------------------- */
      }
    }
    else if (gmm_state == GMM_SERVICE_REQUEST_INITIATED)
    {
      /* GS association is removed when MT DETACH is initiated */
      MSG_MED_DS_0( MM_SUB, "=MM= GS association is removed when MT DETACH is initiated" );
      mm_gs_association_established = FALSE;
    
      /* -------------------------------------------------------------------------
      ** If the MS receives a DETACH REQUEST message in state GMM-SERVICE-REQUEST-
      ** INITIATED, the GPRS detach procedure shall be progressed and the service
      ** request procedure shall be aborted.
      ** ------------------------------------------------------------------------- */
      gmm_send_mt_detach_accept_message();
      mmsend_nas_reg_event_packet(PROC_NAS_MT_DETACH_EVENT,PROC_END,PROC_END_ACCEPT,detach_request_msg.detach_type,detach_request_msg.gmm_cause);
      gmm_set_state( GMM_DEREGISTERED );

      sm_cmd = sm_get_cmd_buf();

      if (sm_cmd == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
      }

      sm_cmd->header.message_set = MS_GMM_SM;
      sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
      if (is_wcdma_mode() &&
         (detach_request_msg.detach_type == GMM_REATTACH_NOT_REQUIRED) &&
          (detach_request_msg.gmm_cause == GPRS_SERVICES_NOT_ALLOWED))
      {
          sm_cmd->cmd.gmm_gprs_rel_ind.cause = 
              mm_convert_gmm_to_sm_cause(detach_request_msg.gmm_cause);
      }
      else
      {
          sm_cmd->cmd.gmm_gprs_rel_ind.cause = NO_GPRS_CONTEXT;
      }
#ifdef FEATURE_DUAL_SIM
      sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

      sm_put_cmd( sm_cmd );

      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND");

      if (detach_request_msg.detach_type == GMM_REATTACH_REQUIRED) 
      {
        if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) && 
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
          ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
          (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)))
        {    
          /* --------------------------------------------------------------------
          ** A GPRS MS operating in MS operation mode A or B in network operation
          ** mode I, which receives a DETACH REQUEST message with detach type
          ** indicating "re-attach required" or "re-attach not required" and no cause code, 
          ** is only detached for GPRS services in the network.
          ** -------------------------------------------------------------------- */
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
      }
      else if(detach_request_msg.detach_type == GMM_MT_IMSI_DETACH)
      {
        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                     SYS_SRV_DOMAIN_NO_SRV,
                                     FALSE,
                                     SYS_SRV_STATUS_LIMITED,
                                     TRUE );      
      }

      gmm_process_detach_request_cause(&detach_request_msg);
      
      mmr_service_ind.service_state = mm_serving_plmn.info;
      if(detach_request_msg.detach_type == GMM_REATTACH_REQUIRED)
      {
        mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN; 
      } 
      mm_send_mmr_service_ind( &mmr_service_ind );

      if (detach_request_msg.detach_type == GMM_REATTACH_REQUIRED)
      {
        /* ----------------------------------------------------------------
        ** The MS shall, after the completion of the GPRS detach procedure,
        ** initiate a GPRS attach procedure.
        ** ---------------------------------------------------------------- */
        mm_send_sm_unblock_all_apn();
#ifdef FEATURE_NAS_FREE_FLOAT
        if (is_gsm_mode())
        {      
           gmm_gprs_attach_pending = TRUE;
        }
        else
#endif
        {
        gmm_initiate_gprs_attach();
        }
      }
#ifdef FEATURE_DSAC
      else if (detach_request_msg.detach_type == GMM_MT_IMSI_DETACH)
      {
        mm_last_procedure_to_update_registration &= ~MM_COMBINED;
        mm_last_procedure_to_update_registration &= MM_RAU;
      }
#endif      
      
    }
    else if (gmm_state == GMM_REGISTERED)
    {
      if (detach_request_msg.detach_type == GMM_REATTACH_REQUIRED)
      {
        /* ---------------------------------------------------------------------
        ** If the detach type information element indicates "re-attach required"
        ** or "re-attach not required" and the MS is attached for GPRS and
        ** non-GPRS services and the network operates in network operation
        ** mode I, then if in the MS the timer T3212 is not already running,
        ** the timer T3312 shall be set to its initial value and restarted.
        ** --------------------------------------------------------------------- */
        if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) &&
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
        {
#ifdef FEATURE_NAS_REL10
          if(mm_is_per_ms_t3212_present)
          {
              if ((mm_stored_per_ms_t3212_value != 0 ) &&
                  (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
              {

                mm_start_timer( TIMER_T3212,
                                mm_stored_per_ms_t3212_value);
              }
          }
          else
#endif
          if ((mm_system_information.T3212_timer_value != 0 ) &&
            (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
        {
              mm_start_timer( TIMER_T3212,
                              mm_system_information.T3212_timer_value );
          }
        }
        /* ----------------------------------------------------------------
        ** When receiving the DETACH REQUEST message and the detach type IE
        ** indicates "re-attach required", the MS shall deactivate the PDP
        ** contexts and deactivate the logical link(s), if any.
        ** ---------------------------------------------------------------- */
        sm_cmd = sm_get_cmd_buf();

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
        /* GS association is removed when MT DETACH is initiated */
        MSG_MED_DS_0( MM_SUB, "=MM= GS association is removed when MT DETACH is initiated" );
        mm_gs_association_established = FALSE;

        /* --------------------------------------------------------------
        ** The MS shall then send a DETACH ACCEPT message to the network
        ** and shall change state to GMM-DEREGISTERED. (24.008 4.7.4.2.2)
        ** -------------------------------------------------------------- */
        gmm_send_mt_detach_accept_message();
        mmsend_nas_reg_event_packet(PROC_NAS_MT_DETACH_EVENT,PROC_END,PROC_END_ACCEPT,detach_request_msg.detach_type,detach_request_msg.gmm_cause);
        gmm_set_state( GMM_DEREGISTERED );

        if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) && 
            (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
            ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
             (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)))
        {
          /* --------------------------------------------------------------------
          ** A GPRS MS operating in MS operation mode A or B in network operation
          ** mode I, which receives a DETACH REQUEST message with detach type
          ** indicating "re-attach required" is only detached for GPRS services
          ** in the network.
          ** -------------------------------------------------------------------- */
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

        mmr_service_ind.service_state = mm_serving_plmn.info;

        /* --------------------------------------------------------------------
        **  Since Network indicated attach was required, ask REG to remain on
        **  the same PLMN for attach procedure.
        ** -------------------------------------------------------------------- */
        mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;

        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND");
        mm_send_mmr_service_ind( &mmr_service_ind );

        /* ----------------------------------------------------------------
        ** The MS shall, after the completion of the GPRS detach procedure,
        ** initiate a GPRS attach procedure.
        ** ---------------------------------------------------------------- */
        mm_send_sm_unblock_all_apn();
#ifdef FEATURE_NAS_FREE_FLOAT
        if (is_gsm_mode())
        {      
           gmm_gprs_attach_pending = TRUE;
        }
        else
#endif
        {
        gmm_initiate_gprs_attach();
      }
      }
      else if (detach_request_msg.detach_type == GMM_REATTACH_NOT_REQUIRED)
      {
        /* ---------------------------------------------------------------------
        ** If the detach type information element indicates "re-attach required"
        ** or "re-attach not required" and the MS is attached for GPRS and
        ** non-GPRS services and the network operates in network operation
        ** mode I, then if in the MS the timer T3212 is not already running,
        ** the timer T3312 shall be set to its initial value and restarted.
        ** --------------------------------------------------------------------- */
        if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) &&
           (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
        {
  /*******************************************************************************************
    * spec 24.008 4.7.4.2.2 If the MS is attached for GPRS and non-GPRS services and the network operates in 
    * network operation mode I, then if in the MS the timer T3212 is not already running, the timer T3212 shall
    * be set to its initial value and restarted if:
    -   -the detach type IE indicates "re-attach not required" and no cause code is included.
    ******************************************************************************************/
#ifdef FEATURE_NAS_REL11
           if((detach_request_msg.gmm_cause_present)  &&
               (detach_request_msg.gmm_cause == 0) )
#endif
           {
#ifdef FEATURE_NAS_REL10
             if(mm_is_per_ms_t3212_present)
             {
               if ((mm_stored_per_ms_t3212_value != 0 ) &&
                  (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
               {
                 mm_start_timer( TIMER_T3212,
                                              mm_stored_per_ms_t3212_value);
               }
             }
             else
#endif
             if ((mm_system_information.T3212_timer_value != 0 ) &&
                (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
             {
                mm_start_timer( TIMER_T3212,
                                mm_system_information.T3212_timer_value );
             }
           }
        }

        if (detach_request_msg.gmm_cause != IMSI_UNKNOWN_IN_HLR)
        {
          /* ----------------------------------------------------------------
          ** When receiving the DETACH REQUEST message and the detach type IE
          ** indicates "re-attach not required" and the cause code is not "#2
          ** (IMSI unknown in HLR)", the MS shall deactivate the PDP contexts
          ** and deactivate the logical link(s), if any.
          ** ---------------------------------------------------------------- */
          sm_cmd = sm_get_cmd_buf();

          if (sm_cmd == NULL)
          {
            MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
          }

          sm_cmd->header.message_set = MS_GMM_SM;
          sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
          if (is_wcdma_mode() &&
             (detach_request_msg.detach_type == GMM_REATTACH_NOT_REQUIRED) &&
              (detach_request_msg.gmm_cause == GPRS_SERVICES_NOT_ALLOWED))
          {
              sm_cmd->cmd.gmm_gprs_rel_ind.cause = 
                  mm_convert_gmm_to_sm_cause(detach_request_msg.gmm_cause);	
          }
          else
          {
              sm_cmd->cmd.gmm_gprs_rel_ind.cause = NO_GPRS_CONTEXT;
          }
#ifdef FEATURE_DUAL_SIM
          sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

          MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

          sm_put_cmd( sm_cmd );
          /* GS association is removed when MT DETACH is initiated */
          MSG_MED_DS_0( MM_SUB, "=MM= GS association is removed when MT DETACH is initiated");
          mm_gs_association_established = FALSE;

          /* --------------------------------------------------------------
          ** The MS shall then send a DETACH ACCEPT message to the network
          ** and shall change state to GMM-DEREGISTERED. (24.008 4.7.4.2.2)
          ** -------------------------------------------------------------- */
          gmm_send_mt_detach_accept_message();
          mmsend_nas_reg_event_packet(PROC_NAS_MT_DETACH_EVENT,PROC_END,PROC_END_ACCEPT,detach_request_msg.detach_type,detach_request_msg.gmm_cause);
          gmm_set_state( GMM_DEREGISTERED );
        }

        gmm_process_detach_request_cause(&detach_request_msg);

        mmr_service_ind.cause = mm_convert_to_reg_cause( detach_request_msg.gmm_cause );
        mmr_service_ind.service_state = mm_serving_plmn.info;

        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND");
        mm_send_mmr_service_ind( &mmr_service_ind );
      }
      else /* detach_request_msg.detach_type == IMSI_DETACH */
      {
        /* --------------------------------------------------------------------------
        ** When receiving the DETACH REQUEST message and the detach type IE indicates
        ** "IMSI detach", the MS shall not deactivate the PDP contexts.  The MS shall
        ** set the MM update status to U2 NOT UPDATED.
        ** -------------------------------------------------------------------------- */
        mm_location_information.location_update_status = NOT_UPDATED;

        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                        SYS_SRV_DOMAIN_NO_SRV,
                                        FALSE,
                                        SYS_SRV_STATUS_LIMITED,
                                        TRUE );

        mmr_service_ind.service_state = mm_serving_plmn.info;

        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND" );
        mm_send_mmr_service_ind( &mmr_service_ind );

        /* GS association is removed when MT DETACH is initiated */
        MSG_MED_DS_0( MM_SUB, "=MM= GS association is removed when MT DETACH is initiated");
        mm_gs_association_established = FALSE;
#ifdef FEATURE_DSAC
        mm_last_procedure_to_update_registration &= ~MM_COMBINED;
        mm_last_procedure_to_update_registration &= MM_RAU;
#endif
#ifdef FEATURE_NAS_FREE_FLOAT
        if (is_gsm_mode())
        {
           gmm_processed_mt_imsi_detach_request = TRUE;
        }
#endif
        
        if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
            (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
        {
        
          /* --------------------------------------------------------------------------
          ** A MS in operation mode A or B in network operation mode I may send a
          ** DETACH ACCEPT message to the network, and shall re-attach to non-GPRS
          ** service by performing the combined routing area updating procedure
          ** according to clause 4.7.5.2, sending a ROUTING AREA UPDATE REQUEST message
          ** with Update Type IE indicating "combined RA/LA updating with IMSI attach."
          ** -------------------------------------------------------------------------- */
          gmm_send_mt_detach_accept_message();
          mmsend_nas_reg_event_packet(PROC_NAS_MT_DETACH_EVENT,PROC_END,PROC_END_ACCEPT,detach_request_msg.detach_type,detach_request_msg.gmm_cause);
          gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
          gmm_initiate_routing_area_update();
        }
        else
        {        
          /* -------------------------------------------------------------
          ** A MS in operation mode C, or in MS operation mode A or B in
          ** network operation mode II or III, shall send a DETACH ACCEPT
          ** message to the network
          ** ------------------------------------------------------------- */
          gmm_send_mt_detach_accept_message();
          mmsend_nas_reg_event_packet(PROC_NAS_MT_DETACH_EVENT,PROC_END,PROC_END_ACCEPT,detach_request_msg.detach_type,detach_request_msg.gmm_cause);
        }
        if(gmm_substate == GMM_IMSI_DETACH_INITIATED)
        {
          MSG_HIGH_DS_0( MM_SUB, "=MM= Since MS initiated IMSI Detach and Network Initiated IMSI detach collided, we are processing NW initiated detacg so stop Timer T3321, no need to retry\n" );
          mm_stop_timer(TIMER_T3321);
        }
      } /* end else (detach_request_msg.detach_type == IMSI_DETACH) */
    } /* end else if (gmm_state == GMM_REGISTERED) */
  } /* end if (gmm_valid_mt_detach_request_message( message, &detach_request_msg )) */
  else
  {
    MSG_HIGH_DS_0( MM_SUB, "=MM= Invalid DETACH REQUEST");
  }
} /* end gmm_process_detach_request_msg() */

/*===========================================================================

FUNCTION GMM_HANDLE_LOCAL_GPRS_DETACH

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_handle_local_gprs_detach( void )
{
  sm_cmd_type                 *sm_cmd;

  if (gmm_detach_type == GMM_MO_IMSI_DETACH)
  {
    mm_stop_timer( TIMER_T3321 );

    mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                    SYS_SRV_DOMAIN_NO_SRV,
                                    FALSE,
                                    SYS_SRV_STATUS_LIMITED,
                                    TRUE );

#ifdef FEATURE_GSM_GPRS
    if (is_gsm_mode())
    {      
      if (mm_serv_change_pending)
      {
        mm_change_service_domain();
        mm_serv_change_pending = FALSE;
      }
    }   
#endif
  }
  else
  {
    /* Notify SM that GPRS services are no longer available */
    sm_cmd = sm_get_cmd_buf();

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

    MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND" );

    sm_put_cmd( sm_cmd );
     
    mm_stop_timer( TIMER_T3321 );

    gmm_set_state( GMM_DEREGISTERED );

    gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           SYS_SRV_DOMAIN_NO_SRV,
                                           FALSE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE );
   

    if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)
    {
       gmm_set_state( GMM_NULL );
       gmm_set_substate( GMM_NULL_SUBSTATE );

      if((mm_idle_substate == MM_NORMAL_SERVICE) ||
         (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE))
      {
#ifdef FEATURE_NAS_REL10
        if(mm_is_per_ms_t3212_present)
        {
          if((mm_stored_per_ms_t3212_value != 0) &&
                (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
          {
            mm_start_timer( TIMER_T3212,
                             mm_stored_per_ms_t3212_value);
          }
        }
        else
#endif
       if ((mm_system_information.T3212_timer_value != 0 ) &&
            (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))        
       {
           mm_start_timer( TIMER_T3212,
                                           mm_system_information.T3212_timer_value );
       }
    }
    }

#ifdef FEATURE_GSM_GPRS
    if (is_gsm_mode())
    {
       if (mm_serv_change_pending)
       {
          mm_change_service_domain();
          mm_serv_change_pending = FALSE;
       }
    }
#endif

  }
  
  if (gmm_ptmsi_sig_tobe_deleted)
  {
     byte i ;

     for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
     {
        gmm_stored_ptmsi_signature.value[i] = 0xFF;
     }

     mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status );
  }
}


/*===========================================================================

FUNCTION GMM_HANDLE_GPRS_DETACH_LOW_LEVEL_FAILURE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_handle_gprs_detach_low_level_failure( void )
{
  sm_cmd_type                 *sm_cmd;
  mmr_service_ind_s_type     mmr_service_ind;
  /* -----------------------------------------------------------------
  ** Lower layer failure before reception of DETACH ACCEPT message
  ** The detach procedure is aborted and the MS shall change to state:
  ** GMM-DEREGISTERED if "GPRS detach" was requested
  ** ----------------------------------------------------------------- */
  if ((gmm_detach_type == GMM_GPRS_DETACH) ||
      (gmm_detach_type == GMM_COMBINED_GPRS_IMSI_DETACH))
  {
    if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) &&
        (gmm_detach_type == GMM_GPRS_DETACH))
    {
      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                       SYS_SRV_DOMAIN_CS_ONLY,
                                       FALSE,
                                       SYS_SRV_STATUS_NO_SRV,
                                       TRUE );
    }
    else
    {
#ifdef FEATURE_LTE
      /* If the ps detach state is waiting to send confirmation then reg service domain should
      ** either be CS+PS or PS only. If it PS only set the reg service domain and service
      ** status to NO_SRV.
      */
      if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
      {
      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                       SYS_SRV_DOMAIN_NO_SRV,
                                       FALSE,
                                         SYS_SRV_STATUS_NO_SRV,
                                         TRUE );
      }
      else
#endif
      {
        gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                         SYS_SRV_DOMAIN_NO_SRV,
                                         FALSE,
                                       SYS_SRV_STATUS_LIMITED,
                                       TRUE );
    }
    }
    if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)
    {
      gmm_set_state( GMM_NULL );
      gmm_set_substate( GMM_NULL_SUBSTATE );

        sm_cmd = sm_get_cmd_buf();

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
    

      if((mm_idle_substate == MM_NORMAL_SERVICE) ||
         (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE))
      {
#ifdef FEATURE_NAS_REL10
        if(mm_is_per_ms_t3212_present) 
        {
          if((mm_stored_per_ms_t3212_value != 0) &&
             (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
          {
             mm_start_timer( TIMER_T3212,
                                        mm_stored_per_ms_t3212_value);
      }
        }
        else
#endif
      if ((mm_system_information.T3212_timer_value != 0 ) &&
            (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
      {
        mm_start_timer( TIMER_T3212,
                        mm_system_information.T3212_timer_value );
      }
      }
      if(!mm_reg_waiting_for_reg_cnf)
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_SERVIC_IND ");
        mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
        mmr_service_ind.service_state = mm_serving_plmn.info;
        mm_send_mmr_service_ind( &mmr_service_ind );
      }
    }
    else
    {
      gmm_set_state( GMM_DEREGISTERED );


        sm_cmd = sm_get_cmd_buf();

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

        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND" );

        sm_put_cmd( sm_cmd ); 
     
      }

    MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_DETACHED");
    pmm_mode = PMM_DETACHED;

#ifdef FEATURE_NAS_ECALL
    if (mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY)
    {
      if (mm_reg_waiting_for_reg_cnf)
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF " );
        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
        mm_send_mmr_reg_cnf();
        mm_reg_waiting_for_reg_cnf = FALSE;
        mm_ready_to_send_reg_cnf = FALSE;
      }
      else
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVIC_IND ");
        mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
        mmr_service_ind.service_state = mm_serving_plmn.info;
        mm_send_mmr_service_ind( &mmr_service_ind );
      }
    }
#endif
#ifdef FEATURE_LTE
    if (mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
    {
      /* Send the service indication or confirmation to CM after the PS detach is completed */
      if (mm_reg_waiting_for_reg_cnf)
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_REG_CNF ");
  
        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;

        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
        mm_send_mmr_reg_cnf();
        mm_reg_waiting_for_reg_cnf = FALSE;
        mm_ready_to_send_reg_cnf   = FALSE;
  }
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_SERVIC_IND " );
        mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
        mmr_service_ind.service_state = mm_serving_plmn.info;
        mm_send_mmr_service_ind( &mmr_service_ind );
      }
    }
#endif
  }
  else if (gmm_detach_type == GMM_MO_IMSI_DETACH)
  {
    /* The gmm state here can be dereg_init for a MO_IMSI_DET type when we have 
         to trigger a detach in GMM_REG_INIT state(set in gmm_initiate_gprs_detach).This is 
         because we cannot go to GMM_REG as we are not yet successfully registered and any 
         other state other than GMM_REG or GMM_DEREG_INIT results in a status msg .*/
         
    if (gmm_state == GMM_DEREGISTERED_INITIATED)
    {
      gmm_set_state( GMM_DEREGISTERED );
    }
    else
    {  
      gmm_set_substate( GMM_NORMAL_SERVICE );

      mm_location_information.location_update_status = NOT_UPDATED;
    }      

    mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                    SYS_SRV_DOMAIN_NO_SRV,
                                    FALSE,
                                    SYS_SRV_STATUS_LIMITED,
                                    TRUE );

    if (mm_serv_change_pending)
    {
      mm_change_service_domain();
      mm_serv_change_pending = FALSE;
    }
  }
  /* Check if there is a Pending NREG_CNF to REG  */
  if (mm_nreg_release_pending)
  {
    /* Received the REL_IND while, stop the DETACH_REL timer. */
    mm_stop_timer(TIMER_DETACH_REL);
    mm_nreg_release_pending = FALSE;
  }
} /* end gmm_handle_gprs_detach_low_level_failure() */


/*===========================================================================

FUNCTION GMM_HANDLE_GPRS_DETACH

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_handle_gprs_detach( mm_cmd_type *message )
{
  gmm_mo_detach_accept_msg_T detach_accept_msg;
  sm_cmd_type   *sm_cmd;

#if defined(FEATURE_NAS_ECALL) || defined(FEATURE_LTE)
  mmr_service_ind_s_type     mmr_service_ind;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (message == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= Unexpected NULL pointer", 0,0,0 );
  }

  switch (message->cmd.hdr.message_set)
  {
    case MS_MM_RR:
      switch (message->cmd.hdr.message_id)
      {
        case RR_ACTIVATION_IND:
          gmm_handle_gprs_detach_low_level_failure();
          mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected RR message %d", message->cmd.hdr.message_id, 0,0 );
      } /* end switch (message->cmd.hdr.message_id) */
      break;

    case MS_MM_RRC:
    case MS_MM_TDSRRC:
      switch (message->cmd.hdr.message_id)
      {
        case RRC_ACTIVATION_IND:
          gmm_handle_gprs_detach_low_level_failure();
          mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
          break;

        case RRC_ABORT_IND:
#ifdef FEATURE_SILENT_REDIAL_AFTER_RA_LA_MISMATCH
          if ((message->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_LAI_RAI_CHANGED) &&
             (!gmm_power_down_active))
          {
            MSG_HIGH_DS_1(MM_SUB, "=MM= Lower layer failure with cause %d,", message->cmd.rrc_abort_ind.abort_cause);
          }
          else
#endif
#ifdef FEATURE_DUAL_SIM
          if ((message->cmd.rrc_abort_ind.abort_cause == RRC_NO_RESOURCE_AVAILABLE ||
               message->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_CONN_REJECT_WAIT_TMR) &&
              (!gmm_power_down_active))
          {
            if(message->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_CONN_REJECT_WAIT_TMR)
            {
              mm_start_timer( TIMER_GMM_RADIO_RETRY, message->cmd.rrc_abort_ind.conn_rej_wait_timer*1000);
            }
            else
            {
              mm_start_timer( TIMER_GMM_RADIO_RETRY, DEFAULT_TIMEOUT );
            }
          }
          else
#endif
          {
           mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
           gmm_handle_gprs_detach_low_level_failure();
          }
          break;

        case RRC_OPEN_SESSION_CNF:
        case RRC_CLOSE_SESSION_IND:
        case RRC_CLOSE_SESSION_CNF:
        case RRC_REL_IND:
        case RRC_ABORT_CNF:
          if(gmm_detach_type != GMM_MO_IMSI_DETACH && (mm_timer_status[TIMER_T3321] == TIMER_ACTIVE || mm_timer_status[TIMER_POWERDOWN] == TIMER_ACTIVE))
          {
            mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
          }
          gmm_handle_gprs_detach_low_level_failure();
          break;

        case MM_DATA_IND:
          switch (message->cmd.mm_data_ind.L3_data[1])
          {
            case GMM_DETACH_ACCEPT:
              if (gmm_valid_mo_detach_accept_message( message, &detach_accept_msg ))
              {

#ifdef FEATURE_GSM_GPRS
                if (is_gsm_mode())
                {
                  gmmllc_gmm_trigger_llc_event(
                             GMM_TRIGGER_FORCE_TO_STANDBY,
                                        detach_accept_msg.force_to_standby,0) ;
                }
#endif
                mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_ACCEPT,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                if (gmm_state == GMM_DEREGISTERED_INITIATED)
                {
                  /* --------------------------------------------------------
                  ** Wait for the Release message from Lower layers
                  ** Before changing the state to DEREGISTRED.
                  ** ------------------------------------------------------*/

                  /* Notify SM that GPRS services are no longer available */
                  sm_cmd = sm_get_cmd_buf();

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

                  /* Stop the T3321 timer as we received the DETACH_ACCEPT mesg */
                  mm_stop_timer( TIMER_T3321 );
#ifdef FEATURE_LTE
                  /* If the ps detach state is not None and if reg service domain is 
                  **  a) CS+PS. Set the registered service domain to CS only and service
                  **      status to SRV
                  **  b) PS only. Set the reg service domain and service status to NO_SRV.
                  */
                  if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
                  {
                    if(mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)
                    {
                      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                       SYS_SRV_DOMAIN_CS_ONLY,
                                                       FALSE,
                                                       SYS_SRV_STATUS_SRV,
                                                       TRUE );
                    }
                    else if(mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY)
                    {
                      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                       SYS_SRV_DOMAIN_NO_SRV,
                                                       FALSE,
                                                       SYS_SRV_STATUS_NO_SRV,
                                                       TRUE );
                    }
                  }
                  else
#endif
                  {
                  gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                   SYS_SRV_DOMAIN_NO_SRV,
                                                   FALSE,
                                                   SYS_SRV_STATUS_LIMITED,
                                                   TRUE );
                  }

#ifdef FEATURE_GSM_GPRS
                  if (is_gsm_mode())
                  {
                    gmm_set_state( GMM_DEREGISTERED );

                    if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)
                    {
                      gmm_set_state( GMM_NULL );
                      gmm_set_substate( GMM_NULL_SUBSTATE );

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                      gmm_transaction_id++;
#endif
                      if(((mm_state == MM_CONNECTION_ACTIVE) ||
                         (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION))&&
                          mm_reg_waiting_for_reg_cnf)
                      {
                        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                        mm_send_mmr_reg_cnf();
                        mm_reg_waiting_for_reg_cnf = FALSE;
                        mm_ready_to_send_reg_cnf   = FALSE;
                      }
                      if((mm_idle_substate == MM_NORMAL_SERVICE) ||
                         (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE))
                      {
#ifdef FEATURE_NAS_REL10
                          if(mm_is_per_ms_t3212_present)
                          {
                            if((mm_stored_per_ms_t3212_value != 0) &&
                               (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                            {
                               mm_start_timer( TIMER_T3212,
                                                          mm_stored_per_ms_t3212_value);
                            }
                          }
                          else
#endif
                      if ((mm_system_information.T3212_timer_value != 0 ) &&
                              (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                      {
                         mm_start_timer( TIMER_T3212,
                                         mm_system_information.T3212_timer_value );
                      }
                    }
                    }
#ifdef FEATURE_NAS_ECALL
                    else if (mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY)
                    {
                      if (mm_reg_waiting_for_reg_cnf)
                      {
                        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF ");
                  
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                        
                        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                        mm_send_mmr_reg_cnf();
                        mm_reg_waiting_for_reg_cnf = FALSE;
                        mm_ready_to_send_reg_cnf   = FALSE;
                      }
                      else
                      {
                        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVIC_IND ");
                        mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                        mmr_service_ind.service_state = mm_serving_plmn.info;
                        mm_send_mmr_service_ind( &mmr_service_ind );
                      }
                    }
#endif
#ifdef FEATURE_LTE
                    /* Send the service indication or service confirmation to CM after the PS detach is completed */
                    else if (mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
                    {
                      if (mm_reg_waiting_for_reg_cnf)
                      {
                        MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_REG_CNF ");
                  
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                        
                        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                        mm_send_mmr_reg_cnf();
                        mm_reg_waiting_for_reg_cnf = FALSE;
                        mm_ready_to_send_reg_cnf   = FALSE;
                      }
                      else
                      {
                        MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_SERVIC_IND " );
                        mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                        mmr_service_ind.service_state = mm_serving_plmn.info;
                        mm_send_mmr_service_ind( &mmr_service_ind );
                      }
                  }
#endif
                  }
                  else
#endif
                  {
#ifdef FEATURE_LTE
                    if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
                    {
                      gmm_set_state( GMM_DEREGISTERED );
                      if (mm_reg_waiting_for_reg_cnf)
                      {
                        MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_REG_CNF ");
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                        mm_send_mmr_reg_cnf();
                        mm_reg_waiting_for_reg_cnf = FALSE;
                        mm_ready_to_send_reg_cnf   = FALSE;
                      }
                      else
                      {
                        MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_SERVIC_IND " );
                        mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                        mmr_service_ind.service_state = mm_serving_plmn.info;
                        mm_send_mmr_service_ind( &mmr_service_ind );
                      }
                    }
                    else
#endif
                  {
                    mm_nreg_release_pending = TRUE;

                    /* --------------------------------------------------------
                    ** Start the Timer TIMER_DETACH_REL and wait for the RRC
                    ** connection release after the DETACH ACCEPT Message
                    ** is received.
                    ** ------------------------------------------------------*/
                    mm_start_timer(TIMER_DETACH_REL, DEFAULT_TIMEOUT);
                  }
                }
                }
                else if ((gmm_state == GMM_REGISTERED) &&
                         (gmm_substate == GMM_IMSI_DETACH_INITIATED))
                {
                  mm_stop_timer( TIMER_T3321 );

                  /**---------------------------------------------------------------
                  **MS shall stop timer T3340 if the MS receives a DETACH ACCEPT message
                  **while the T3346 timer is running and the MS has set the detach type
                  **to "IMSI detach" in the DETACH REQUEST message
                  **(24.008 section 4.7.1.9) --------------------------------------------------*/

#ifdef FEATURE_NAS_REL10
                  if (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
                  {
                    mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);
                  }
#endif
#ifdef FEATURE_GSM_GPRS
                  if (is_gsm_mode())
                  {
                    gmm_set_substate( GMM_NORMAL_SERVICE );

                    mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                    SYS_SRV_DOMAIN_NO_SRV,
                                                    FALSE,
                                                    SYS_SRV_STATUS_LIMITED,
                                                    TRUE );
                    if (mm_serv_change_pending)
                    {
                      mm_change_service_domain();
                      mm_serv_change_pending = FALSE;
                    }
                  }
                  else
#endif
                  {
                    mm_nreg_release_pending = TRUE;

                    /* --------------------------------------------------------
                    ** Start the Timer TIMER_DETACH_REL and wait for the RRC
                    ** connection release after the DETACH ACCEPT Message
                    ** is received.
                    ** ------------------------------------------------------*/
                    mm_start_timer(TIMER_DETACH_REL, DEFAULT_TIMEOUT);
                  }
                }
                else
                {
                  gmm_send_gmm_status_message(
                    MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE );
                }

                if (gmm_ptmsi_sig_tobe_deleted)
                {
                  byte i ;

                  /* -------------------------------
                  ** Delete the old P-TMSI signature
                  ** ------------------------------- */
                  for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
                  {
                    gmm_stored_ptmsi_signature.value[i] = 0xFF;
                  }

                  mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                      gmm_stored_ptmsi_signature,
                                                      gmm_stored_rai,
                                                      gmm_update_status );
                }
              }
              else
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Invalid DETACH ACCEPT" );
              }
              break;

            default:
              MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message type %d",
                          message->cmd.mm_data_ind.L3_data[1], 0,0 );
          } /* end switch (gmm_message_type) */
          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected RRC message %d", message->cmd.hdr.message_id, 0,0 );
      } /* end switch (message->cmd.hdr.message_id) */
      break;

    case MS_TIMER:
      switch (message->cmd.timer_expiry.timer_id)
      {
        case TIMER_T3321:
          /* ----------------------------------------------------------------
          ** On the first expiry of the timer, the MS shall retransmit the
          ** DETACH REQUEST message and shall reset and restart timer T3321.
          ** This retransmission is repeated four times.  On the fifth expiry
          ** of timer T3321, the GPRS detach procedure shall be aborted.  The
          ** MS shall change to state GMM_DEREGISTERED if "GPRS Detach" was
          ** requested.
          ** ---------------------------------------------------------------- */
          t3321_timeout_counter++;
          MSG_HIGH_DS_1(MM_SUB, "=MM= T3321 timeout counter = %d", t3321_timeout_counter);
#ifdef FEATURE_DUAL_DATA
          mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_LOW);
#endif 
#ifdef FEATURE_DUAL_SIM
          if(gmm_force_local_detach_sim[mm_sub_id])
          {
            MSG_HIGH_DS_1( MM_SUB, "=MM= DSDS: Perform Local Detach on sub%d as PS Attach is pended on other subs ", 
              (mm_sub_id+1) );
            t3321_timeout_counter = 5;
          }
#endif

          if (t3321_timeout_counter <= 4)
          {
            if (( gmm_substate == GMM_SUSPENDED ) || (is_gsm_mode() && gmm_is_suspended))
            {
               /* Pending the detach request and will be processed once
                  RR_GMM_GPRS_RESUMPTION_IND is received.
               */

               gmm_gprs_detach_pending = TRUE;
            }
            else
            {
               /* --------------------------------------------
               ** Send a DETACH REQUEST message to the network
               ** -------------------------------------------- */
               gmm_send_mo_detach_request_message( gmm_detach_type,
                                                   gmm_power_down_active );
               mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_RESEND,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);

               mm_start_timer( TIMER_T3321, DEFAULT_TIMEOUT );
            }
          }
          else
          {

            mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_TIME_OUT,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
            if ((gmm_detach_type == GMM_GPRS_DETACH) ||
                (gmm_detach_type == GMM_COMBINED_GPRS_IMSI_DETACH))
            {
              gmm_set_state( GMM_DEREGISTERED );
              MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_DETACHED" );
              pmm_mode = PMM_DETACHED;

              sm_cmd = sm_get_cmd_buf();

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
#ifdef FEATURE_GSM_GPRS
              if ((is_gsm_mode()) && (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY))
              {
                gmm_set_state( GMM_NULL );
                gmm_set_substate( GMM_NULL_SUBSTATE );
                if((mm_idle_substate == MM_NORMAL_SERVICE) ||
                   (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE))
                {
#ifdef FEATURE_NAS_REL10
                   if(mm_is_per_ms_t3212_present)
                   {
                      if((mm_stored_per_ms_t3212_value != 0) &&
                         (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                      {
                          mm_start_timer( TIMER_T3212,
                          mm_stored_per_ms_t3212_value);
                      }
                   }
                   else
#endif
                  if ((mm_system_information.T3212_timer_value != 0 ) &&
                      (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))        
                  {
                    mm_start_timer( TIMER_T3212,
                    mm_system_information.T3212_timer_value );
                  }
                }
              }
#endif

              if ((gmm_detach_type == GMM_GPRS_DETACH) &&
                  (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
              {
                gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                 SYS_SRV_DOMAIN_CS_ONLY,
                                                 FALSE,
                                                 SYS_SRV_STATUS_LIMITED,
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

              if((is_gsm_mode())&&((mm_state == MM_CONNECTION_ACTIVE) ||
                 (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION)) &&
                  mm_reg_waiting_for_reg_cnf)
              {
                mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                mm_send_mmr_reg_cnf();
                mm_reg_waiting_for_reg_cnf = FALSE;
                mm_ready_to_send_reg_cnf   = FALSE;
              }
              /* -----------------------------------------------------------
              **  GPRS detach procedure is aborted if T3321 expires 5 times
              ** ----------------------------------------------------------- */
              mm_abort_rr (RRC_PS_DOMAIN_CN_ID, TRUE);
            }
            else if (gmm_detach_type == GMM_MO_IMSI_DETACH)
            {
              gmm_set_substate( GMM_NORMAL_SERVICE );
              mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
              SYS_SRV_DOMAIN_NO_SRV,
              FALSE,
              SYS_SRV_STATUS_LIMITED,
              FALSE ); 

              if (mm_serv_change_pending && is_umts_mode())
              {
                mm_change_service_domain();
                mm_serv_change_pending = FALSE;
              }
            }
            mm_ps_domain_possible_reest = FALSE;
            if (gmm_ptmsi_sig_tobe_deleted)
            {
              byte i ;

              /* -------------------------------
              ** Delete the old P-TMSI signature
              ** ------------------------------- */
              for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
              {
                gmm_stored_ptmsi_signature.value[i] = 0xFF;
              }

              mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                  gmm_stored_ptmsi_signature,
                                                  gmm_stored_rai,
                                                  gmm_update_status );
            }
          }
          break;

        case TIMER_POWERDOWN:
          gmm_set_state( GMM_DEREGISTERED );
          sm_cmd = sm_get_cmd_buf();

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

          MSG_HIGH_DS_0(MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");
          sm_put_cmd( sm_cmd );

          MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_DETACHED" );
          pmm_mode = PMM_DETACHED;
          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           SYS_SRV_DOMAIN_NO_SRV,
                                           FALSE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE );
          break;

         case TIMER_DETACH_REL:
          // Add more support here
#ifdef FEATURE_DUAL_SIM
          if( (sm_is_pdp_active( (sys_modem_as_id_e_type)mm_sub_id)) && (rabm_is_rab_active((sys_modem_as_id_e_type)mm_sub_id )) )
#else
          if( (sm_is_pdp_active()) && (rabm_is_rab_active()) )
#endif
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Since RABs are active, PS call is ongoing, Don't do anything on TIMER_DETACH_REL expiry\n" );
            mm_stop_timer(TIMER_DETACH_REL);
          }
          else
          {
            MSG_ERROR_DS_0( MM_SUB, "=MM= DETACH Con_Rel timeout, send RRC_ABORT_REQ");
            mm_abort_rr( RRC_PS_DOMAIN_CN_ID, TRUE );

            // Once RRC_ABORT_CNF is received from RRC, The Pending
            // NREG_CNF is sent to the REG task.
            mm_stop_timer(TIMER_DETACH_REL);
          }
          break;

#ifdef FEATURE_DUAL_SIM
         case TIMER_GMM_RADIO_RETRY:
          mm_stop_timer(TIMER_T3321);
          /* --------------------------------------------
          ** Send a DETACH REQUEST message to the network
          ** -------------------------------------------- */
          gmm_send_mo_detach_request_message( gmm_detach_type,
                                              gmm_power_down_active );
          if(t3321_timeout_counter ==  0)
          {
            mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_START,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
          }
          else
          {
            mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_RESEND,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
          }
          break;
#endif

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected timer message %d",
                      message->cmd.timer_expiry.timer_id, 0,0 );
      } /* end switch (message->cmd.timer_expiry.timer_id) */
      break;

    default:
      MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message set %d", message->cmd.hdr.message_set, 0,0 );
  } /* end switch (message->cmd.hdr.message_set) */
} /* end gmm_handle_gprs_detach() */


/*===========================================================================

FUNCTION GMM_UPDATE_RA_TYPE

DESCRIPTION
 This function decides RA Update type based on current camping.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_update_ra_type(void)
{
  if(gmm_periodic_rau_pending )
  {
    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
        (mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY)))
    {
      gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;    
    }
    else if (!((gmm_plmn_changed( gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn )) ||
               (gmm_rai_changed ( gmm_stored_rai.location_area_code,
                                  gmm_stored_rai.routing_area_code, 
                                  mm_serving_plmn.lac, mm_serving_plmn.rac ))) && 
             (gmm_update_status == GMM_GU1_UPDATED))
    {
      gmm_ra_update_type = GMM_PERIODIC_UPDATING;    
    }                                                      
    else
    {
       gmm_ra_update_type = GMM_RA_UPDATING;
    }
  }
  else 
  {
    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
        (mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY)))
    {
      if(mm_possible_imsi_attach)
      {
        gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;    
      }
      else
      {
        gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;    
      }
    }
    else
    { 
      gmm_ra_update_type = GMM_RA_UPDATING;
    }  
  }
}

/*===========================================================================

FUNCTION GMM_PROCESS_SERVICE_IND

DESCRIPTION
  Function to

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_process_service_ind ( mm_cmd_type     *msg_p )
{
  mmr_service_ind_s_type mmr_service_ind;
  rabm_cmd_type *rabm_cmd;
  mm_as_service_state_T service_status;
  boolean is_cell_forbidden = FALSE;
  boolean is_serving_rai_changed = FALSE;
  inter_task_lai_T mm_serving_lai_local;
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type        mm_other_as_id;
#endif
    boolean                     ps_dreg_started = FALSE;

#ifdef FEATURE_FEMTO_CSG
  boolean is_manual_csg_selection_success = FALSE;
#endif 
#ifdef FEATURE_LTE
  boolean is_lte_to_gw_interrat = FALSE;
#endif

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
  byte lai[LAI_SIZE] = { 0xff, 0xff, 0xff, 0xFE, 0xFE };
#endif

#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memscpy((void *)&mm_serving_lai_local, sizeof(inter_task_lai_T),(void *)&mm_serving_lai[0],LAI_SIZE) ;
  service_status = msg_p->cmd.rr_service_ind.service_state; 
  /* Set the Default cause in the Service indication primitive */
  mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;

  gmm_previous_serving_rai.plmn_id                   = gmm_current_lai.PLMN_id;
  gmm_previous_serving_rai.location_area_code.lac[0] = gmm_current_lai.location_area_code[0];
  gmm_previous_serving_rai.location_area_code.lac[1] = gmm_current_lai.location_area_code[1];
  gmm_previous_serving_rai.routing_area_code         = gmm_current_rac;

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER

  /* CCO Failed, RRC will camp on the same cell that it was on before W to G CCO.
   ** Set the Previous service RAI to the serving cell info as this avoids
   ** sending of any RAU.
   */
   if ((WtoGCCO_state == CCO_FAILED)
#ifdef FEATURE_INTERRAT_PCCO_GTOW
       || (GtoWCCO_state == CCO_FAILED )
#endif
                                     )
   {
      gmm_previous_serving_rai.plmn_id                   = mm_serving_plmn.info.plmn;
      gmm_previous_serving_rai.location_area_code.lac[0] = mm_serving_plmn.lac.lac[0];
      gmm_previous_serving_rai.location_area_code.lac[1] = mm_serving_plmn.lac.lac[1];
      gmm_previous_serving_rai.routing_area_code         = mm_serving_plmn.rac;
      
      lai[0] = mm_serving_plmn.info.plmn.identity[0];
      lai[1] = mm_serving_plmn.info.plmn.identity[1];
      lai[2] = mm_serving_plmn.info.plmn.identity[2];
      lai[3] = mm_serving_plmn.lac.lac[0];
      lai[4] = mm_serving_plmn.lac.lac[1];

      if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
          (mm_location_information.location_update_status == UPDATED) &&
          (memcmp( mm_location_information.lai, lai, LAI_SIZE ) == 0)
         )
      {
        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                        SYS_SRV_DOMAIN_CS_ONLY,
                                        FALSE,
                                        SYS_SRV_STATUS_SRV,
                                        TRUE 
                                      );
        (void) memscpy( mm_previous_serving_lai,LAI_SIZE,
                        mm_location_information.lai,
                        LAI_SIZE );
        mm_substate_control(MM_STD_NORMAL_SERVICE);
        mm_previous_idle_substate = mm_idle_substate;

      }
      /* Inform REG about the the PLMN information on NEW RAT after CCO failed */
      mmr_service_ind.service_state = mm_serving_plmn.info;
      mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
      mm_send_mmr_service_ind( &mmr_service_ind );
   }
#endif

#ifdef FEATURE_GSM_GPRS

  if ((mm_interrat_state == MM_UMTS_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS) ||
      (mm_interrat_state == MM_GSM_TO_UMTS_INTERRAT_CHANGE_IN_PROGRESS))
  {
    if (is_gsm_mode())
    {
#ifdef FEATURE_INTERRAT_PCCO_GTOW
      if ( GtoWCCO_state == CCO_INACTIVE )
#endif
      {
        gmmllc_reinit(GMM_STANDBY_STATE) ;
      }
    }
    else if (is_umts_mode())
    {
#ifdef FEATURE_INTERRAT_PCCO_GTOW
      if ( GtoWCCO_state == CCO_INACTIVE )
#endif
      {        
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
        if (gmmllc_get_glif_state() == GMM_READY_STATE)
        {
          gmm_is_prev_mode_connected = TRUE;
          MSG_HIGH_DS_1(MM_SUB, "=MM= GMM Previous mode Ready or Connected = %d", gmm_is_prev_mode_connected);
        }
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
        gmmllc_reinit(GMM_IDLE_STATE) ;
      }
    }

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
    if ((WtoGCCO_state == CCO_INACTIVE)
#ifdef FEATURE_INTERRAT_PCCO_GTOW
        && (GtoWCCO_state == CCO_INACTIVE)
#endif
                                          )
    {
#ifdef FEATURE_GSM_GPRS
      gmm_rau_is_required = TRUE;
#endif /*FEATURE_GSM_GPRS*/
    }
    else if (WtoGCCO_state == CCO_FAILED)
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= Resetting the WtoGCCO_state to CCO_INACTIVE ");
      WtoGCCO_state = CCO_INACTIVE;
    }
#ifdef FEATURE_INTERRAT_PCCO_GTOW
    else if (GtoWCCO_state == CCO_FAILED )
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= Resetting the GtoWPCCO_state to CCO_INACTIVE ");
      GtoWCCO_state = CCO_INACTIVE;
    }
#endif
#else
#ifdef FEATURE_GSM_GPRS
    gmm_rau_is_required = TRUE;
#endif /*FEATURE_GSM_GPRS*/
#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */

    mm_interrat_state = MM_INTERRAT_CHANGE_STATE_TYPE_LO;
  }

#endif /* #ifdef FEATURE_GSM_GPRS */
#ifdef FEATURE_LTE
  if(mm_interrat_state == MM_LTE_TO_UMTS_INTERRAT_CHANGE_IN_PROGRESS ||
      mm_interrat_state == MM_LTE_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS)
  {
    is_lte_to_gw_interrat = TRUE;
    mm_interrat_state = MM_INTERRAT_CHANGE_STATE_TYPE_LO;

    /* The newly added check ((mm_state==MM_CONNECTION_ACTIVE)&& (mm_idle_substate==MM_NO_CELL_AVAILABLE))
      is for L2W srvcc case. In L2W srvcc case mm_idle_substate was MM_NO_CELL_AVAILABLE from begining.
      Here we need to change the mm_idle_substate to NORMAL_SERICE so that LAU can be triggered if there
      is RAU failure with cause #14 or #7. */
    if(((mm_state == MM_IDLE)||((mm_state==MM_CONNECTION_ACTIVE)&&
        (mm_idle_substate==MM_NO_CELL_AVAILABLE))) && 
        (((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
         (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)) ||
        (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY))&&
       ((mm_serving_plmn.info.plmn_service_capability == SYS_SRV_CAPABILITY_CS_PS_SERVICE)||
        (mm_serving_plmn.info.plmn_service_capability == SYS_SRV_CAPABILITY_CS_SERVICE_ONLY)))
    {
      mm_set_mm_substate_on_irat_from_lte();
    }
/*4.7.1.7a Intersystem change from S1 mode to A/Gb mode or S1 mode to Iu mode with ISR activated*/
#ifdef FEATURE_NAS_REL11
    if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
         &&((gmm_is_prev_mode_connected)||
           ((gmm_is_prev_mode_connected == FALSE)&&
              (mm_serving_plmn.info.active_rat != gmm_last_signalling_sent_rat)))&&
			  mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL11)
    {
      gmm_rau_is_required = TRUE;
    }
#endif
  }
#endif
  /* -----------------------------------------------------
  ** This local variable "emergency_call_redirected" is marked TRUE if
  ** emergency call due to inter-rat reselection/redirection is triggered.
  ** This will be used to prevent RR(C) Abort if mm state is 
  ** MM_WAIT_FOR_RR_CONNECTION_MM(14).
  ** ----------------------------------------------------- */
      
  if ((service_status == MM_AS_SERVICE_AVAILABLE) &&
      ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) ||
       (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)) &&
      (mm_held_cnm_service_req.present) && 
      (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
      ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
       (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)
#ifdef FEATURE_WTOG_PS_HANDOVER
       #error code not present
#endif
  ) )           
  {
    mm_check_pending_requests();
    MSG_HIGH_DS_1(MM_SUB, "=MM= Processing Pending E-call after InterRAT change - NMO 1: %d", 
              mm_held_cnm_service_req.interat_status);  
    mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;         
  }     

#if defined (FEATURE_GSM_DTM) || defined (FEATURE_NO_REG_UNTIL_NMO_KNOWN_AFTER_HANDOVER)
#if defined (FEATURE_GSM_DTM) && defined (FEATURE_NO_REG_UNTIL_NMO_KNOWN_AFTER_HANDOVER)
  if((msg_p->cmd.hdr.message_id != (int) RR_SERVICE_IND_FROM_OTA_MSG) &&
     (!((msg_p->cmd.hdr.message_id == (int) RRC_SERVICE_IND_FROM_OTA_MSG)&&
        (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE))))
#elif defined (FEATURE_GSM_DTM)
  if(msg_p->cmd.hdr.message_id != (int) RR_SERVICE_IND_FROM_OTA_MSG)
#else
  if(!((msg_p->cmd.hdr.message_id == (int) RRC_SERVICE_IND_FROM_OTA_MSG)&&
       (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)))
#endif
  {
    
#ifdef FEATURE_LTE
    /*The following code is cover for handover from L2G NMO1 non DTM cell*/
    if(((mm_lu_pended_nmo_unknown == TRUE) &&
       (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) &&
       (is_gsm_mode()) &&
       ((gmm_substate == GMM_SUSPENDED) || (gmm_is_suspended)) &&
       (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
       (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
        ))
    {
      gmm_pending_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
    }
#endif
    mm_lu_pended_nmo_unknown = FALSE;
  }
#endif /* defined (FEATURE_GSM_DTM) || defined (FEATURE_NO_REG_UNTIL_NMO_KNOWN_AFTER_HANDOVER)*/

  /* ---------------------------------------------------
  ** If SIM is invalid then GMM substate remains NO_IMSI
  ** --------------------------------------------------- */

  if (mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY))
  {
    if (service_status == MM_AS_NO_SERVICE ||
        (reg_state_is_no_service_to_be_send_due_to_china_mcc(mm_serving_plmn.info.plmn)))
    {
      if(reg_state_is_no_service_to_be_send_due_to_china_mcc(mm_serving_plmn.info.plmn) &&
         (rrc_connection_status != MMRRC_IDLE))
       {
         if(mm_state == MM_CONNECTION_ACTIVE)
         {
              mm_pending_no_service_to_reg= TRUE;
         }
         else if(rrc_connection_status != MMRRC_RELEASE_IN_PROGRESS)
         {
           rrc_cmd_type *rrc_cmd_p = NULL;
#ifdef FEATURE_WCDMA
           if (is_umts_mode())
           {
             rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
             if (rrc_cmd_p == NULL)
             {
                MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
             }
             rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
             rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
             rrc_cmd_p->cmd.rrc_abort_req.hard_abort = FALSE;
             rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
             rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = FALSE;
             MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RRC_ABORT_REQ");
             send_cmd_to_rrc(rrc_cmd_p, mm_serving_plmn.info.active_rat );
           }
           else
#endif
           if( is_gsm_mode())
           {
              MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RR_ABORT_REQ" );
              mm_send_rr_abort_req(FALSE);
           }
         }
      }
      else
    {
      /* ---------------------------
      ** GPRS coverage has been lost
      ** --------------------------- */
      mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;

      if(reg_state_is_no_service_to_be_send_due_to_china_mcc(mm_serving_plmn.info.plmn))
      {
        mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                       mm_serving_plmn.info.reg_service_domain,
                                       FALSE,
                                       SYS_SRV_STATUS_NO_SRV,
                                         FALSE );
      }
      else
      {
        gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                         mm_serving_plmn.info.reg_service_domain,
                                         FALSE,
                                         SYS_SRV_STATUS_NO_SRV,
                                       TRUE );
      }

      /* ---------------------------------------------------------------------------
      ** Handle service status changes so that PS data continuity is ensured during
      ** RAT changes due to loss of service.
      ** --------------------------------------------------------------------------- */
      mm_rat_handle_srv_status_update ( SYS_SRV_STATUS_NO_SRV );

      if((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
          ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                                       SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)))
      {
         mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_NO_SERVICE;
      }

      if ((mm_reg_waiting_for_reg_cnf) && (mm_ready_to_send_reg_cnf))
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF");
        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
        mm_send_mmr_reg_cnf();
        mm_reg_waiting_for_reg_cnf = FALSE;
        mm_ready_to_send_reg_cnf   = FALSE;
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
      if ((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) || 
          ((gmm_state == GMM_REGISTERED) && gmm_detach_after_rau_pending))
      {
        if (gmm_detach_after_rau_pending)
        {
          gmm_set_state( GMM_DEREGISTERED_INITIATED );
          gmm_handle_gprs_detach_low_level_failure();
        }
        else
        {
          /* ------------------------------------------------------------------
          ** Lower layer failure - the procedure shall be aborted.  Timer T3330
          ** shall be stopped if still running.  The routing area updating
          ** attempt counter shall be incremented.
          ** ------------------------------------------------------------------ */
          gmm_increment_routing_area_update_attempt_counter
          (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
            FALSE
#endif
          );
          mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
        }
      }
      else if (gmm_state == GMM_REGISTERED_INITIATED)
      {
        /* -------------------
        ** Lower layer failure
        ** ------------------- */
        gmm_increment_gprs_attach_attempt_counter();
        mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);

        /* -------------------
        ** TS 24.008 4.2.4.2.2
        ** ------------------- */
        gprs_attach_upon_entry_to_a_new_cell = TRUE;
      }
      else if  (gmm_state == GMM_DEREGISTERED_INITIATED)
      {
        gmm_handle_gprs_detach_low_level_failure();
        mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
    
      }

      gmm_set_substate( GMM_NO_CELL_AVAILABLE );

      /* Set LAI to invalid LAI */
      gmm_current_lai.PLMN_id.identity[0] = 0xFF;
      gmm_current_lai.PLMN_id.identity[1] = 0xFF;
      gmm_current_lai.PLMN_id.identity[2] = 0xFF;
      gmm_current_lai.location_area_code[0] = 0xFF;
      gmm_current_lai.location_area_code[1] = 0xFE;
      gmm_current_rac = 0xFF;

      /* Send a MMR_REG_CNF if there is a pending REG CNF */

      if (mm_reg_waiting_for_reg_cnf)
      {
        if (mm_ready_to_send_reg_cnf)
        {
          MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF NO_SERVICE" );
          mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
          mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
          mm_send_mmr_reg_cnf();
          mm_reg_waiting_for_reg_cnf = FALSE;
          mm_ready_to_send_reg_cnf   = FALSE;
        }
      }
      else
      {
        mmr_service_ind.service_state = mm_serving_plmn.info;
        mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
        mm_send_mmr_service_ind( &mmr_service_ind );
      }

      /* Notify the SM that the Service is lost */
      gmm_sm_send_con_release_ind(SIG_CONNECTION_RELEASE);

      if((gmm_state == GMM_SERVICE_REQUEST_INITIATED) && 
           (gmm_service_request_initiator == MS_CC_MM) &&
            (mm_sms_is_active == TRUE))
      {
            MSG_HIGH_DS_0( MM_SUB, "=MM= MMCNM_EST_REJ due to no service indication");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej( MM_REJECT_CAUSE, MM_REJ_NO_SERVICE , MMCC_SMS_PS);
#else
            mm_send_mmcnm_est_rej( MM_REJECT_CAUSE, MM_REJ_NO_SERVICE );
#endif
      }
      mm_send_mmcnm_rel_ind_ps(PS_SMS_FAIL,FALSE); 
//FR15492
    }
    }
    else /* SERVICE_AVAILABLE */
    {
      if (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
      {
        /* -----------------------------------------------------
        ** The cell is not supporting GPRS (TS 24.008 4.2.4.1.1)
        ** ----------------------------------------------------- */
        gmm_set_substate( GMM_NO_CELL_AVAILABLE );

        gmm_current_lai.PLMN_id.identity[0]   = mm_serving_plmn.info.plmn.identity[0];
        gmm_current_lai.PLMN_id.identity[1]   = mm_serving_plmn.info.plmn.identity[1];
        gmm_current_lai.PLMN_id.identity[2]   = mm_serving_plmn.info.plmn.identity[2];
        gmm_current_lai.location_area_code[0] = mm_serving_plmn.lac.lac[0];
        gmm_current_lai.location_area_code[1] = mm_serving_plmn.lac.lac[1];

        /* Set RAI to invalid RAI */
        gmm_current_rac = 0xFF;

        mm_serving_plmn.mm_service_state = SERVICE_AVAILABLE;

        gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                         mm_serving_plmn.info.reg_service_domain,
                                         FALSE,
                                         mm_serving_plmn.info.service_status,
                                         TRUE );

        if((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
            ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                                    SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
             (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                                         SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)))
        {
          mmcoord_mmr_reg_cnf.cause          = MMR_CAUSE_NOT_SPECIFIED;
          mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CAMPED_ONLY;
        }

        if ((mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) ||
            (mm_state == MM_LOCATION_UPDATE_INITIATED))
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= PS not available: Service update to REG pended till CS registration");  
        }
        else if (mm_reg_waiting_for_reg_cnf)
        {
          if (mm_ready_to_send_reg_cnf)
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF" );
            if((mm_idle_substate == MM_ATTEMPTING_TO_UPDATE) &&
                ((mm_timer_status[TIMER_T3211] != TIMER_STOPPED) ||
                 (mm_pending_lu_request == T3122_EXPIRED)))
            {
              mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
            }
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();
            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
          }
        }
        else
        {
          MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND" );
          if((mm_idle_substate == MM_ATTEMPTING_TO_UPDATE) &&
              ((mm_timer_status[TIMER_T3211] != TIMER_STOPPED) ||
               (mm_pending_lu_request == T3122_EXPIRED)))
          {
            mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
          }
          mmr_service_ind.service_state = mm_serving_plmn.info;
          mm_send_mmr_service_ind( &mmr_service_ind );
        }

        if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
        {
          /* ------------------------------------------------------------------
          ** Lower layer failure - the procedure shall be aborted.  Timer T3330
          ** shall be stopped if still running.  The routing area updating
          ** attempt counter shall be incremented.
          ** ------------------------------------------------------------------ */
          gmm_increment_routing_area_update_attempt_counter
          (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
            FALSE
#endif
          );
          mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
        }
        else if (gmm_state == GMM_REGISTERED_INITIATED)
        {
          /* -------------------
          ** Lower layer failure
          ** ------------------- */
          gmm_increment_gprs_attach_attempt_counter();
          mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);


          /* -------------------
          ** TS 24.008 4.2.4.2.2
          ** ------------------- */
          gprs_attach_upon_entry_to_a_new_cell = TRUE;
        }
        else if (gmm_state == GMM_DEREGISTERED_INITIATED)
        {
          gmm_handle_gprs_detach_low_level_failure();
          mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
        }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)        
        else if (gmm_gprs_detach_pending)
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
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/        
      }
      else /* GPRS service available */
      {
        boolean routine_man_selection_failed = FALSE;

        gmm_current_lai.PLMN_id.identity[0]   = mm_serving_plmn.info.plmn.identity[0];
        gmm_current_lai.PLMN_id.identity[1]   = mm_serving_plmn.info.plmn.identity[1];
        gmm_current_lai.PLMN_id.identity[2]   = mm_serving_plmn.info.plmn.identity[2];
        gmm_current_lai.location_area_code[0] = mm_serving_plmn.lac.lac[0];
        gmm_current_lai.location_area_code[1] = mm_serving_plmn.lac.lac[1];
        gmm_current_rac                       = mm_serving_plmn.rac;

        if ((gmm_previous_service_state == SERVICE_NOT_AVAILABLE ||
#if defined(FEATURE_LTE)
             (is_lte_to_gw_interrat)||
#endif
             gmm_substate == GMM_LIMITED_SERVICE) &&
#ifdef FEATURE_DSAC
            (gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)) != TRUE) &&
#endif
            (gmm_update_status == GMM_GU1_UPDATED))
        {
          gmm_previous_serving_rai = gmm_stored_rai;
        }


        if (!(((PLMN_MATCH( gmm_current_lai.PLMN_id.identity,
                           mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity )) ||
              (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
               sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))
     #ifdef FEATURE_FEMTO_CSG
                 && (mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
     #endif
     #if defined (FEATURE_LTE) && defined (FEATURE_DUAL_DATA)
                 && (mm_is_volte_registration_allowed(mm_serving_plmn.info.plmn, FALSE) == TRUE) 
     #endif
               )  &&
            (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
              SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
        {
          routine_man_selection_failed = TRUE;
          MSG_HIGH_DS_0(MM_SUB, "=MM= Different PLMN found during MAN_LIMITED_ACQ");
        }

#ifdef FEATURE_DUAL_SIM
        if (
            (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
            (reg_sim_per_subs_plmn_forbidden(gmm_current_lai.PLMN_id,(sys_modem_as_id_e_type)mm_sub_id ))||
            (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
            (mm_check_forbidden_regional_roaming( &gmm_current_lai )) ||
            (gmm_check_gprs_forbidden_rai(mm_serving_plmn.info.plmn,mm_serving_plmn.lac,mm_serving_plmn.rac))
#else
        if ((reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn )) ||
            (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
            (mm_check_forbidden_regional_roaming( &gmm_current_lai )) ||
            (reg_sim_plmn_forbidden(gmm_current_lai.PLMN_id))||
            (gmm_check_gprs_forbidden_rai(mm_serving_plmn.info.plmn,mm_serving_plmn.lac,mm_serving_plmn.rac))
#endif
#ifdef FEATURE_LTE
            /*
            ** After PS detach if we are waiting for REG req, we should not start any 
            ** registration on receiving the service indication.
            */
            || (mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
#ifdef FEATURE_DUAL_DATA
            || (!mm_is_volte_registration_allowed(mm_serving_plmn.info.plmn, FALSE))
#endif
#endif
#ifdef FEATURE_FEMTO_CSG
            || (!mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
#endif
            )
        {
          is_cell_forbidden = TRUE;
          MSG_HIGH_DS_0( MM_SUB, "=MM= Cell is forbidden");
        }

#ifdef FEATURE_FEMTO_CSG
        if((mm_serving_plmn.info.csg_info.csg_id == mmcoord_pending_reg_message.cmd.mmr_reg_req.csg_id) &&
            (sys_plmn_match(mm_serving_plmn.info.plmn, mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn)) &&
            (mm_serving_plmn.info.csg_info.csg_id != SYS_CSG_ID_INVALID) &&
            (mm_serving_csg_id == mm_serving_plmn.info.csg_info.csg_id))
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= CSG: Manual CSG selection success");
          is_manual_csg_selection_success = TRUE;
        }
#endif

        if ((gmm_rai_changed( gmm_previous_serving_rai.location_area_code,
                             gmm_previous_serving_rai.routing_area_code,
                             mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
           (gmm_plmn_changed( gmm_previous_serving_rai.plmn_id,
                                    mm_serving_plmn.info.plmn)))
        {
          is_serving_rai_changed = TRUE;
          MSG_HIGH_DS_0( MM_SUB, "=MM= RAI changed" );
        }

         
#if defined(FEATURE_LTE)
        /* If UE has PDP activated in LTE and ISR is activated, set TIN to GUTI*/
        if(((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
             !((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode 
                        ==  SYS_NETWORK_SELECTION_MODE_LIMITED) ||
               ((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                               SYS_NETWORK_SELECTION_MODE_AUTOMATIC )||
             (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                               SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) ||
             ((!gmm_manual_mode_user_selection_procedure)&&
              ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                               SYS_NETWORK_SELECTION_MODE_MANUAL) ||
               (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                               SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))))&& 
            (is_cell_forbidden))
              )
             )
           )
        {
          if(sm_check_isr_deact_status_on_interrat())
          {
            MSG_HIGH_DS_0(MM_SUB,"=MM= Deactivate ISR due to PDP context update while in LTE");
            emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
          }
        }       
#endif

        /* --------------- 3GPP TS 22.011 -------
        ** 3.2.2.2 At switch-on or recovery from lack of coverage 
        ** B) Manual network selection mode 
        **->  If a PLMN is selected but the UE cannot register for PS services on it because 
        **    registration is rejected with the cause "GPRS services not allowed in this PLMN", 
        **    the UE shall not re-attempt to register for PS services on that network. The PLMN 
        **    is added to the list "Forbidden PLMN's for GPRS services". The UE shall not re-attempt
        **    to register for PS on that network unless the same PLMN is selected again by the user.
        **    The reception of the cause "GPRS services not allowed in this PLMN", does not affect the CS service
        **->  If a PLMN is selected but the UE cannot register on it for other reasons, the UE shall, 
        **    upon detection of a new LA (not in a forbidden LA list) of the selected PLMN, 
        **    attempt to register on the PLMN.
        ** -------------------------------------------------------*/
        if ((routine_man_selection_failed) ||
             ((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                SYS_NETWORK_SELECTION_MODE_AUTOMATIC )||
              (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) ||
              ((!gmm_manual_mode_user_selection_procedure ||
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE             
                 mm_per_subs_check_lai_in_reject_list(mm_serving_lai_local, (sys_modem_as_id_e_type) mm_as_id)
#else
                 mm_check_lai_in_reject_list(mm_serving_lai_local)
#endif
               )&&
               ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                SYS_NETWORK_SELECTION_MODE_MANUAL) ||
                (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))))&& 
              (is_cell_forbidden
#ifdef FEATURE_FEMTO_CSG
               && !(is_manual_csg_selection_success)
#endif
               )))
        {
          /* -----------------------------------
          ** The new LA is in the forbidden list
          ** ----------------------------------- */
          gmm_set_substate( GMM_LIMITED_SERVICE );

          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           mm_serving_plmn.info.reg_service_domain,
                                           FALSE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE );

          if (is_serving_rai_changed)
          {

            if (gmm_state == GMM_REGISTERED_INITIATED)
            {
              gmm_stop_auth_fail_timers();
              gmm_set_state( GMM_DEREGISTERED );
            }
            else if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
            {
              /* ----------------------------------------------------------------
              ** Treat as a lower layer failure - the procedure shall be aborted.
              ** ---------------------------------------------------------------- */
              gmm_stop_auth_fail_timers();
              gmm_set_state( GMM_REGISTERED );
              gmm_update_status = GMM_GU2_NOT_UPDATED;
              mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                  gmm_stored_ptmsi_signature,
                                                  gmm_stored_rai,
                                                  gmm_update_status );

              gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               SYS_SRV_DOMAIN_NO_SRV,
                                               FALSE,
                                               SYS_SRV_STATUS_LIMITED,
                                               TRUE );   
            }
          }

          if ((mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) ||
              (mm_state == MM_LOCATION_UPDATE_INITIATED))
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= PS not available: Service update to REG pended till CS registration");  
          }
          else if (mm_reg_waiting_for_reg_cnf)
          {
            if (mm_ready_to_send_reg_cnf)
            {
              MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF LIMITED_SERV" );
              mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
              if ((mm_state != MM_IDLE)||
                  (((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
                     (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
                   (mm_lu_attempt_counter <4)))
              {
                mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN;
              }
              mm_send_mmr_reg_cnf();
              mm_reg_waiting_for_reg_cnf = FALSE;
              mm_ready_to_send_reg_cnf   = FALSE;
            }
          }
          else
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVIC_IND : LIMITED_SERV");
            mmr_service_ind.service_state = mm_serving_plmn.info;
            if (((mm_state != MM_IDLE)||
                (((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
                   (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
                 (mm_lu_attempt_counter <4)))
#ifdef FEATURE_LTE
            /*
             ** After PS detach if we are waiting for REG req,then do not
             ** set this reg cause. Otherwise REG might end up sending
             ** limited rouinte to CM if we are detached for CS services as well.
             */
            && (mm_ps_detach_state != MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
#endif
               )
            {
              mmr_service_ind.cause = MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN;
            }
            mm_send_mmr_service_ind( &mmr_service_ind );
          }

          if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
              (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
          {
             mm_handle_message( msg_p );
          }
        }
        else if((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
                ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                             SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
                (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                             SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)))
        {
          if((is_cell_forbidden == TRUE)||
             ((mm_timer_status[TIMER_T3302] == TRUE) && 
              (is_serving_rai_changed == FALSE)))
          {
#ifdef FEATURE_SGLTE           
            if(MM_SUB_IS_SGLTE)
            {
              if((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) ||
                 (mm_serving_plmn.info.plmn_reg_type != SYS_REG_DOMAIN_CS_SERVICE_ONLY))
              {
                mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CAMPED_ONLY;
              }
            }
            else
#endif
             {
              mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CAMPED_ONLY;  
            }
          }
          else 
          {
#ifdef FEATURE_SGLTE
            if(MM_SUB_IS_SGLTE)
            {
              if ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                  (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_PS))
              {
                 mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CS_PS_SERVICE;
              }
              else if (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_PS_ONLY)
              {
                 mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_PS_SERVICE_ONLY;
              }
              else 
              {
                if (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_PS)
                 {
                   if ( mm_serving_plmn.info.plmn_reg_type == SYS_REG_DOMAIN_CS_SERVICE_ONLY)
                    {
                      mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CS_PS_SERVICE;
                    }
                    else
                    {
                      mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_PS_SERVICE_ONLY;
                    }
                 }
              }
            }
            else
#endif
            {
            mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_PS_SERVICE_ONLY;
            }
          }

          gmm_set_substate( GMM_LIMITED_SERVICE );
      
          if (mm_reg_waiting_for_reg_cnf)
          {
            if (mm_ready_to_send_reg_cnf)
        {
              MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_REG_CNF LIMITED_SERV" );

              mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;

              mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
              mm_send_mmr_reg_cnf();
              mm_reg_waiting_for_reg_cnf = FALSE;
              mm_ready_to_send_reg_cnf   = FALSE;
            }
          }
          else
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_SERVIC_IND : LIMITED_SERV" );
            mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
            mmr_service_ind.service_state = mm_serving_plmn.info;
            mm_send_mmr_service_ind( &mmr_service_ind );
          }
        }
#ifdef FEATURE_NAS_ECALL
        else if (mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY)
        {
          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           mm_serving_plmn.info.reg_service_domain,
                                           FALSE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE ); 
          if (mm_reg_waiting_for_reg_cnf)
          {
            if (mm_ready_to_send_reg_cnf)
            {
              MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF LIMITED_SERV");

              mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;

              mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
              mm_send_mmr_reg_cnf();
              mm_reg_waiting_for_reg_cnf = FALSE;
              mm_ready_to_send_reg_cnf   = FALSE;
            }
          }
          else
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVIC_IND : LIMITED_SERV" );
            mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
            mmr_service_ind.service_state = mm_serving_plmn.info;
            mm_send_mmr_service_ind( &mmr_service_ind );
          }
          if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
              (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
          {
            mm_handle_message( msg_p );
          }
        }
#endif
#ifdef FEATURE_NAS_REL10
        else if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
#ifdef FEATURE_NAS_REL11
                (mm_is_rrc_access_class_11_to_15() == FALSE)&&
#endif
                (gmm_is_plmn_equivalent_t3346()) &&
                (
/*4.7.5.1.5 (j)*/
#ifndef FEATURE_NAS_REL11
                (gmmllc_get_glif_state() != GMM_READY_STATE)&&
#endif
                 (pmm_mode != PMM_CONNECTED)))
        {
           if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
              (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
           {
              mm_handle_message( msg_p );
           }

           else 
           {

             /** -----------------------------------------------------------------
             **If the stored RAI is different to the RAI of the current serving cell or
             **the TIN indicates "GUTI", the MS shall set the GPRS update status to GU2 NOT UPDATED
             **and change to state GMM-REGISTERED.ATTEMPTING-TO-UPDATE.
             **(24.008 section 4.7.5.1.5) ------------------------------------------------*/
             if (((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                       gmm_stored_rai.routing_area_code,
                                       mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                     (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn))) &&
                   (gmm_update_status == GMM_GU1_UPDATED)) 
     #if defined(FEATURE_LTE)
                      &&
                  ((emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI)
/*4.1.1.7:At an inter-system change from S1 mode to A/Gb mode, if the timer T3346 is running and the TIN indicates "RAT related TMSI", 
the MS shall set the GPRS update status to GU2 NOT UPDATED and enter state GMM-REGISTERED.ATTEMPTING-TO-UPDATE.*/
#ifdef FEATURE_NAS_REL11
                  && (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_RAT_RELATED_TMSI)
#endif
                   )
#endif        
                 )
                )
            {
              gmm_set_substate( GMM_NORMAL_SERVICE);
              gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               mm_serving_plmn.info.reg_service_domain,
                                               FALSE,
                                               SYS_SRV_STATUS_SRV,
                                               TRUE);
            }
            else 
            {
              if(gmm_state == GMM_DEREGISTERED)
              {
                gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH);
              }
              else 
              {
                gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE);
              }
              gmm_update_status = GMM_GU2_NOT_UPDATED;

              mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                  gmm_stored_ptmsi_signature,
                                                  gmm_stored_rai,
                                                  gmm_update_status );

              gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               mm_serving_plmn.info.reg_service_domain,
                                               FALSE,
                                               SYS_SRV_STATUS_LIMITED,
                                               TRUE );


              if((gmm_plmn_changed( gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn ))||
                 (gmm_rai_changed( gmm_stored_rai.location_area_code,
                                   gmm_stored_rai.routing_area_code,
                                   mm_serving_plmn.lac, mm_serving_plmn.rac )))
               {
                 if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                     (mm_serving_plmn.network_op_mode != gmm_previous_network_op_mode) &&
                     (gmm_previous_network_op_mode != PS_DOMAIN_SYS_INFO_UNAVAILABLE) &&
                     ((mm_timer_status[TIMER_T3246] == TIMER_STOPPED)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                      ||(mm_is_rrc_access_class_11_to_15() == TRUE)
#endif
                      ))
                 {
                    gmm_change_of_nmo_during_t3346_active();
                 }
               }
            }

            if(mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
            {
              MSG_HIGH_DS_0(MM_SUB, "=MM= LU initiated - T3346 running ");
            }
            else if (mm_reg_waiting_for_reg_cnf)
            {
              if (mm_ready_to_send_reg_cnf)
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_REG_CNF ");

                mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;

                mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                mm_send_mmr_reg_cnf();
                mm_reg_waiting_for_reg_cnf = FALSE;
                mm_ready_to_send_reg_cnf   = FALSE;
              }
            }
            else
            {
               MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_SERVIC_IND ");
               mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
               mmr_service_ind.service_state = mm_serving_plmn.info;
               mm_send_mmr_service_ind( &mmr_service_ind );
            }
           }

        }
#endif
        else if (is_serving_rai_changed &&
                 (gmm_state == GMM_REGISTERED) &&
                 ((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                             gmm_stored_rai.routing_area_code,
                             mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                      (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
                   (gmm_update_status == GMM_GU1_UPDATED)) &&
                 !gmm_processing_directed_signalling_reestablishment
#ifdef FEATURE_GSM_GPRS
                   && !((gmm_is_suspended) && (gmm_rau_is_required))
#endif /*FEATURE_GSM_GPRS*/
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
                 && !(WtoGCCO_state == CCO_IN_PROGRESS)
#endif
#ifdef FEATURE_INTERRAT_PCCO_GTOW
                 && !(GtoWCCO_state == CCO_IN_PROGRESS)
#if defined(FEATURE_LTE) 
                && !(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI)
#endif
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
#ifdef FEATURE_NAS_REL11
/*24.008: 4.7.5.1
The normal routing area updating procedure is initiated: in A/Gb mode, after Inter RAT handover from S1 mode or Iu mode.*/
                && (gmm_is_lw_to_g_interrat_in_progress == FALSE)
#endif
#ifdef FEATURE_DSAC
                 && !((mm_cs_access_barred_to_unbarred || mm_ps_access_barred_to_unbarred) &&
                      (mm_last_procedure_to_update_registration != MM_NONE &&
                       mm_last_procedure_to_update_registration != MM_COMBINED))
#endif
                 )
        {
          /* RAU not required if UPDATED and not suspended before RAT change */
          MSG_HIGH_DS_0( MM_SUB, "=MM= GMM UPDATED on RA: RAU not required " );
          if ( (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
               (mm_location_information.location_update_status == UPDATED ) &&
               ((memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 )) )
          {
            if (mm_idle_substate == MM_LIMITED_SERVICE)
            {
             mm_substate_control(MM_STD_NORMAL_SERVICE);
            }
            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                             SYS_SRV_DOMAIN_CS_PS,
                                             FALSE,
                                             SYS_SRV_STATUS_SRV,
                                             TRUE );
            
            mm_gs_association_established = TRUE; 
            gmm_gprs_detach_pending       = FALSE;
            
          }
          else
          {
            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                             SYS_SRV_DOMAIN_PS_ONLY,
                                             FALSE,
                                             SYS_SRV_STATUS_SRV,
                                             TRUE );
          }
          gmm_set_substate(GMM_NORMAL_SERVICE);
          mm_serving_plmn.info.ps_data_suspend = FALSE;
          gmm_ra_update_pending = FALSE;

          /* If mm_state is 13 
           * pend sending of MMR_SERVICE_IND until LAU is complete 
           * todo!! --> Move this check in mm_send_mmr_service_ind 
           * */
          if(mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
          {
             MSG_HIGH_DS_3(MM_SUB, "=MM= Not sending MMR_SERVICE_IND, mm_state = %d, ms_op_mode = %d, NW_op_mode = %d", \
                           mm_state,mm_serving_plmn.ms_op_mode,mm_serving_plmn.network_op_mode);
          }
          else
          {
          MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND" );
          mmr_service_ind.service_state = mm_serving_plmn.info;
          mm_send_mmr_service_ind( &mmr_service_ind );
        }

        }
#ifdef FEATURE_ENHANCED_NW_SELECTION
        else if ( is_serving_rai_changed &&
                  gmm_is_rai_in_foreground_search_lai_rai_timer_list((byte *)&gmm_current_lai,gmm_current_rac))
        {
          MSG_HIGH_DS_0( MM_SUB, "=MM= RAI is in foreground search LAI list");

          if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED ||
              gmm_state == GMM_SERVICE_REQUEST_INITIATED ||
              gmm_state == GMM_REGISTERED)
          {
             gmm_set_substate(GMM_ATTEMPTING_TO_UPDATE);
             gmm_set_state(GMM_REGISTERED);
          }
          else if (gmm_state == GMM_DEREGISTERED ||
                   gmm_state == GMM_REGISTERED_INITIATED ||
                   gmm_state == GMM_DEREGISTERED_INITIATED )
                    
          {
             gmm_sm_send_gprs_rel_ind_if_needed();
             gmm_set_substate(GMM_ATTEMPTING_TO_ATTACH);
             gmm_set_state(GMM_DEREGISTERED);
          }
          gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           SYS_SRV_DOMAIN_NO_SRV,
                                           FALSE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE );
          if (mm_reg_waiting_for_reg_cnf)
          {
            mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();
            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
          }
          else
          {
            mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
            mmr_service_ind.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
            mmr_service_ind.service_state = mm_serving_plmn.info;
            mm_send_mmr_service_ind( &mmr_service_ind );
          }
        }
#endif
        /* Condition is added for triggering RAU immediately
           if PMM is in commneted mode and WtoGCCO in progress
        */

        else if ((is_serving_rai_changed)
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
                  || (WtoGCCO_state == CCO_IN_PROGRESS)
#endif
#ifdef FEATURE_INTERRAT_PCCO_GTOW
                  || (GtoWCCO_state == CCO_IN_PROGRESS)
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
#if defined(FEATURE_LTE)
                || (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI && is_lte_to_gw_interrat == TRUE)
#endif
#ifdef FEATURE_NAS_REL11
/*24.008: 4.7.5.1: The normal routing area updating procedure is initiated: in A/Gb mode, after Inter RAT handover from S1 mode or Iu mode.*/
                || (gmm_is_lw_to_g_interrat_in_progress == TRUE)
#endif
#ifdef FEATURE_DSAC
                  ||
                 ((mm_cs_access_barred_to_unbarred || mm_ps_access_barred_to_unbarred) &&
                  (mm_last_procedure_to_update_registration != MM_NONE &&
                   mm_last_procedure_to_update_registration != MM_COMBINED))                                                 
#endif
                                                       )
        {
          /* -------------------------------------------------------------------
          ** The routing area or PLMN of the serving cell has changed
          ** ----------------------------------------------------------------- */

          /* If LAI has changed and NMO1 then reset MM attempt counter too */
          if((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
             ((memcmp(&(mm_previous_serving_lai[0]),&(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE) != 0 ) ||
              (memcmp(&(mm_previous_serving_lai[PLMN_SIZE]),&(mm_serving_plmn.lac.lac[0]), LAC_SIZE) != 0)))
          {
            mm_clear_lu_attempt_counter();
          }
          
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
          if (WtoGCCO_state == CCO_IN_PROGRESS)
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= W to G CCO while in PMM_CONNECTED, RAU would trigger" );
          }
          else
#endif
#ifdef FEATURE_INTERRAT_PCCO_GTOW
          if (GtoWCCO_state == CCO_IN_PROGRESS)
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= G to W CCO, RAU would trigger");
          }
          else
#endif
#if defined(FEATURE_LTE)
           if (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI)
           {
             MSG_HIGH_DS_0(MM_SUB,"=MM= TIN set to GUTI, RAU Would trigger"); 
           }
           else
#endif
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Serving RAI changed");
          }
#ifdef FEATURE_DSAC
          if (mm_cs_access_barred_to_unbarred || mm_ps_access_barred_to_unbarred)
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= CS or PS access got unbarred in NMO 1 network" );
          }
          if ((gmm_plmn_changed( gmm_previous_serving_rai.plmn_id,
                                 mm_serving_plmn.info.plmn)) ||
              (gmm_previous_serving_rai.location_area_code.lac[0] != mm_serving_plmn.lac.lac[0]   ||
               gmm_previous_serving_rai.location_area_code.lac[1] != mm_serving_plmn.lac.lac[1]))
          {
            mm_last_procedure_to_update_registration = (mm_last_procedure_to_update_registration & (~MM_LU));
            mm_last_procedure_to_update_registration = (mm_last_procedure_to_update_registration & (~MM_RAU));            
          }
          else if (gmm_previous_serving_rai.routing_area_code != mm_serving_plmn.rac)
          {
            mm_last_procedure_to_update_registration = (mm_last_procedure_to_update_registration & (~MM_RAU));
          }                                
#endif 

          if ((gmm_state == GMM_DEREGISTERED) &&
              (gmm_substate == GMM_ATTEMPTING_TO_ATTACH))
          {
            gprs_attach_attempt_counter = 0;
            MSG_HIGH_DS_1(MM_SUB, "=MM= GPRS Attach Attempt counter = %d", gprs_attach_attempt_counter);
          }
          else if (gmm_substate == GMM_ATTEMPTING_TO_UPDATE)
          {
            /* -------------------------------------------------------------
            ** The routing area updating attempt counter shall be reset when
            ** a new routing area is entered and the MS is in substate
            ** ATTEMPTING-TO-UPDATE (3GPP TS 24.008 section 4.7.5).
            ** ------------------------------------------------------------- */
            gmm_rau_attempt_counter = 0;
            MSG_HIGH_DS_1(MM_SUB, "=MM= RAU Attempt counter = %d", gmm_rau_attempt_counter);
          }

          /* -----------------------------------------------------------------
          ** RA or PLMN has changed, the GMM would not perform a RAU procedure,
          ** Any pending connection requests from SM/RABM/CC should be rejected
          ** ----------------------------------------------------------------*/
          if (gmm_service_request_initiator == MS_GMM_SM)
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_ESTABLISH_REJ" );
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
            rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
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
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL , MMCC_SMS_PS);
#else
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL );
#endif
          }

          gmm_service_request_initiator = NULL;

          switch (gmm_state)
          {
            case GMM_REGISTERED_INITIATED:
              /* -----------------------------------------------------------
              ** If a cell change into a new routing area occurs before an
              ** ATTACH ACCEPT or ATTACH REJECT message has been received,
              ** the GPRS attach procedure shall be aborted and re-initiated
              ** immediately (24.008 4.7.3.1.5)
              ** ----------------------------------------------------------- */
           /*Reset 3330 attempt counter as RAU is being triggered on new RAI*/
              t3310_timeout_counter = 0;
              gmm_initiate_gprs_attach();
              break;

            case GMM_DEREGISTERED_INITIATED:
              if (gmm_power_down_active)
              {
                gmm_sm_send_gprs_rel_ind_if_needed();
                gmm_set_state(GMM_DEREGISTERED);
                mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                
              }
#ifndef FEATURE_CLASS_B_OPERATION_IN_NMO3
              else if(mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
              {
                  MSG_HIGH_DS_0(MM_SUB,"=MM= Local detach  ");
                  gmm_handle_gprs_detach_low_level_failure();
                  mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
              }
#endif
              else
              {
                /* ---------------------------------------------------------------
                ** If a cell change into a new routing area occurs before a
                ** DETACH ACCEPT message has been received, the GPRS detach
                ** procedure shall be aborted and re-initiated after successfully
                ** performing a routing area updating procedure (24.008 4.7.4.1.4)
                ** --------------------------------------------------------------- */
                if (((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) ||
                     (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)) &&
                    (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
                {
                  gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
                }
                else
                {
                  gmm_ra_update_type = GMM_RA_UPDATING;
                }
                ps_dreg_started = FALSE;
#ifdef FEATURE_DUAL_SIM
                for(mm_other_as_id = SYS_MODEM_AS_ID_1 ; mm_other_as_id < MAX_NAS_STACKS ; mm_other_as_id++)
                {
                  if((mmcoord_reg_req_pending_sim[mm_other_as_id] == TRUE) &&
                         (gmm_force_local_detach_sim[mm_sub_id] == TRUE))
                  {           
                    MSG_HIGH_DS_1(MM_SUB,"=MM= Local detach on SUB %d",mm_as_id+1);
                    gmm_handle_gprs_detach_low_level_failure();
                    mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                    ps_dreg_started = TRUE;
                    if(mm_serv_change_pending)
                    {
                      mm_change_service_domain();
                    }
                  }
                }
#endif              
                 if (!ps_dreg_started)
                {
                  if ((mm_serv_change_pending)
#ifdef FEATURE_LTE
                      ||( mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
#endif
                     )
                  {
                    gmm_detach_after_rau_pending = TRUE;
                    mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
                  }
                  gmm_initiate_routing_area_update();
                }
              }
              break;

            case GMM_SERVICE_REQUEST_INITIATED:
              /* ---------------------------------------------------------------
              ** If a cell change into a new routing area occurs, and the
              ** necessity of routing area update procedure is determined before
              ** the security mode setting procedure is completed, a SERVICE
              ** ACCEPT message has been received, the Service request procedure
              ** shall be aborted and the routing area updating procedure is
              ** started immediately.  Follow-on request pending may be
              ** indicated in the ROUTING AREA UPDATE REQUEST for the service,
              ** which was the trigger of the aborted Service request procedure,
              ** to restart the pending service itself or the Service Request
              ** procedure after the completion of the routing area updating
              ** procedure.  If the service type of the aborted SERVICE REQUEST
              ** was indicating "data", then the routing area update procedure
              ** may be followed by a re-initiated Service request procedure
              ** indicating "data", if it is still necessary. (24.008 4.7.13.5)
              ** --------------------------------------------------------------- */
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
              break;

            case GMM_DEREGISTERED:
              if (gmm_substate == GMM_ATTEMPTING_TO_ATTACH)
              {
                /* -----------------------------------------------------------------
                ** Perform GPRS attach when the routing area of the serving cell has
                ** changed and the location area this cell is belonging to is not in
                ** the list of forbidden LAs (24.008 4.2.4.2.2)
                ** ----------------------------------------------------------------- */
                mm_stop_timer( TIMER_T3302 );
                mm_stop_timer( TIMER_T3311 );
#ifdef FEATURE_DUAL_SIM
                mm_stop_timer( TIMER_GMM_RADIO_RETRY );
#endif
                gmm_initiate_gprs_attach();
                gprs_attach_attempt_counter = 0;
                MSG_HIGH_DS_1(MM_SUB, "=MM= GPRS Attach Attempt counter = %d", gprs_attach_attempt_counter );
              }
              else if (gmm_substate == GMM_LIMITED_SERVICE ||
                       gmm_substate == GMM_ATTACH_NEEDED) 
              {
                /* -------------------------------------------------------------------
                ** Perform GPRS attach when a cell is entered which may provide normal
                ** service (e.g., location area is not in one of the forbidden lists
                ** (24.008 4.2.4.2.3)
                ** ----------------------------------------------------------------- */
                gmm_initiate_gprs_attach();
              }
              else if (gmm_substate == GMM_NO_CELL_AVAILABLE)
              {

                if((mm_reg_waiting_for_reg_cnf) &&
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
                {
                  mm_ready_to_send_reg_cnf = TRUE;
                }

                /* --------------------------
                ** Resumption of GPRS service
                ** -------------------------- */
                gmm_set_substate( GMM_NORMAL_SERVICE );

                if ((!gmm_gprs_attach_pending) &&
                    (gmm_state != GMM_REGISTERED_INITIATED ))
                {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND");
                mmr_service_ind.service_state = mm_serving_plmn.info;
                mm_send_mmr_service_ind( &mmr_service_ind );
              }
              }
              else if (gmm_substate == GMM_SUSPENDED)
              {
                gmm_set_substate(GMM_NORMAL_SERVICE);
              }              
              else if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                       (mm_serving_plmn.network_op_mode != gmm_previous_network_op_mode) &&
                       (gmm_previous_network_op_mode != PS_DOMAIN_SYS_INFO_UNAVAILABLE) &&
                       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode !=
                            SYS_NETWORK_SELECTION_MODE_LIMITED))
              {
                gmm_change_of_network_mode_of_operation();
              }
              else if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                       (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                       (gmm_substate == GMM_NORMAL_SERVICE))
              {
                mm_handle_message( msg_p );
              }

              break;

            case GMM_ROUTING_AREA_UPDATING_INITIATED:
              if ((gmm_rai_changed( gmm_previous_serving_rai.location_area_code,
                                gmm_previous_serving_rai.routing_area_code,
                                mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                  (gmm_plmn_changed( gmm_previous_serving_rai.plmn_id,
                                mm_serving_plmn.info.plmn)))
              {             
                /* --------------------------------------------------------------
                ** If a routing area border is crossed when the MS is in state
                ** GMM-ROUTING-AREA-UPDATING-INITIATED, the routing area updating
                ** procedure shall be aborted and re-initiated immediately.  The
                ** MS shall set the GPRS update status to GU2 NOT UPDATED
                ** -------------------------------------------------------------- */
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

                if(mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode !=
                            SYS_NETWORK_SELECTION_MODE_LIMITED)
                {
                  if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                      (mm_serving_plmn.network_op_mode != gmm_previous_network_op_mode) &&
                      (gmm_previous_network_op_mode != PS_DOMAIN_SYS_INFO_UNAVAILABLE))
                  {
                    if ((gmm_previous_network_op_mode == NETWORK_OP_MODE_1) &&
                         ( (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2) 
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                            || (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
                         )
                       )
                    {
                      gmm_set_state( GMM_REGISTERED );
                    }
                    /*Reset 3330  attempt counter as RAU is being triggered on new RAI*/
                    t3330_timeout_counter = 0;
                    gmm_change_of_network_mode_of_operation();
                  }
                  else
                  {
                    gmm_set_substate(GMM_ATTEMPTING_TO_UPDATE);
                    gmm_set_state( GMM_REGISTERED );

                    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
                    {
                      if (gmm_ra_update_type != GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)
                      {
                        gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
                      }
                    }

                    /*Reset 3330  attempt counter as RAU is being triggered on new RAI*/
                    t3330_timeout_counter = 0;
                    gmm_initiate_routing_area_update();
                  }
                }
              }  
              break;

            case GMM_REGISTERED:
              if ((gmm_substate == GMM_NO_CELL_AVAILABLE 
#ifdef FEATURE_LTE
                   && !( is_lte_to_gw_interrat)
#endif 
                   ) || (gmm_substate == GMM_LIMITED_SERVICE &&
                  mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode !=
                  SYS_NETWORK_SELECTION_MODE_LIMITED) )
              {
                /* --------------------------
                ** Resumption of GPRS service
                ** -------------------------- */
                gmm_set_substate( GMM_NORMAL_SERVICE );
                if((!gmm_ra_update_pending)&&
                   (gmm_state != GMM_ROUTING_AREA_UPDATING_INITIATED))
                {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND" );
                mmr_service_ind.service_state = mm_serving_plmn.info;
                mm_send_mmr_service_ind( &mmr_service_ind );
              }
              }
#ifdef FEATURE_LTE 
              else if (gmm_substate == GMM_NO_CELL_AVAILABLE  && is_lte_to_gw_interrat)
              {
                gmm_set_substate( GMM_NORMAL_SERVICE );      
              }
#endif               
              if (gmm_state != GMM_ROUTING_AREA_UPDATING_INITIATED)
              {
                /* ----------------------------------------------------------------
                ** In UMTS, the normal routing area updating procedure is initiated
                ** when the MS detects a change of the routing area in state
                ** GMM-REGISTERED (24.008 4.7.5.1)
                ** ---------------------------------------------------------------- */
                if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                    (mm_serving_plmn.network_op_mode != gmm_previous_network_op_mode) &&
                    (gmm_previous_network_op_mode != PS_DOMAIN_SYS_INFO_UNAVAILABLE) &&
                    (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode !=
                            SYS_NETWORK_SELECTION_MODE_LIMITED))
                {
                  gmm_change_of_network_mode_of_operation();
                }
                else
                {
                  gmm_update_ra_type();

                  if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                      (mm_state == MM_WAIT_FOR_RR_CONNECTION_MM) &&
                      (mm_cnm_srv_est_cause != MM_RR_EMERGENCY_CALL) &&
                      !((mm_held_cnm_service_req.present) && 
                      (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL)) &&
                      ((memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) != 0 ) ||
                       (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) != 0 ) ||
                       (mm_location_information.location_update_status != UPDATED)))
                  {
                    if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
                    {                                        
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                      mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL ,MMCC_SMS_PS);
#else
                      mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL );
#endif
                      mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE );
                      mm_state_control( MM_STD_RR_CONNECTION_ABORTED );
                    }
                    gmm_ra_update_pending = TRUE;
                    gmm_pending_ra_update_type = gmm_ra_update_type;
                  }
                  else
                  {
                    gmm_initiate_routing_area_update();
                  }
                }
              }

#ifdef FEATURE_DSAC
              if ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode
                       != SYS_NETWORK_SELECTION_MODE_LIMITED) &&
                  (gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION))) &&
                  (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                  (((mm_cs_access_barred_to_unbarred) &&
                    ((mm_last_procedure_to_update_registration == MM_COMBINED) ||
                     (mm_last_procedure_to_update_registration & MM_LU) != MM_LU ||
                     (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
                     (mm_pending_lu_request == T3212_EXPIRED || mm_pending_lu_request == T3312_EXPIRED))) ||
                   (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) != 0 ) ||
                   (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) != 0 )))
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= LU Required. Either cs domain is getting unbarred OR RA Change while PS is barred ");
                if((mm_location_information.location_update_status == UPDATED) &&
                   (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                   (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ) &&
                   ((mm_possible_imsi_attach) && (mm_system_information.ATT)))
                {   
                  if (mm_state == MM_IDLE)
                  {
                    mm_initiate_lu( MM_LU_CAUSE_IMSI_ATTACH );
                  }          
                  else
                  {
                    mm_pending_lu_request = IMSI_ATTACH;
                  }
                }
                else if ((mm_cs_access_barred_to_unbarred) && 
                         (mm_pending_lu_request == T3212_EXPIRED || mm_pending_lu_request == T3312_EXPIRED ) &&
                         (mm_location_information.location_update_status == UPDATED) &&
                         (memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                         (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 ))
                {
                  if (mm_state == MM_IDLE)
                  {
                    mm_initiate_lu( MM_LU_CAUSE_PERIODIC );
                  }          
                  else
                  {
                    mm_pending_lu_request = T3212_EXPIRED;
                  }                
                }
                else
                {   
                  if (mm_state == MM_IDLE)
                  {
                    mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                  }          
                  else
                  {
                    mm_pending_lu_request = NORMAL_LU;
                  }
                }                
                mm_cs_access_barred_to_unbarred = FALSE;
              }
#endif              
              
              break;

            default:
              break;
          } /* end switch (gmm_state) */
        }
        else /* RAI not changed */
        {
//        /* ----------------------------------------------------------------------
//        ** If entry into this state was caused by b) or d) with cause "Retry upon
//        ** entry into a new cell" of section 4.7.3.1.5, GPRS attach shall be
//        ** performed when a new cell is entered (24.008 4.2.4.2.2)
//        ** ---------------------------------------------------------------------- */
//        if ((gmm_state == GMM_DEREGISTERED) &&
//            (gmm_substate == GMM_ATTEMPTING_TO_ATTACH) &&
//            (gprs_attach_upon_entry_to_a_new_cell))
//        {
//          mm_stop_timer( TIMER_T3302 );
//          mm_stop_timer( TIMER_T3311 );
//          gmm_initiate_gprs_attach();
//          gprs_attach_attempt_counter = 0;
//        }

          mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);

#if defined(FEATURE_LTE)
          /* ----------------------------------------------------------------------
          ** If we are here because of LTE to GW interrat and ISR is on
          ** then resume the PS data suspend as RAU will not be initiated
          ** in this scenario.
          ** ---------------------------------------------------------------------- */
          if ((((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
/*4.7.1.7a Intersystem change from S1 mode to A/Gb mode or S1 mode to Iu mode with ISR activated*/
#ifdef FEATURE_NAS_REL11
                    && (gmm_rau_is_required == FALSE)
					&& (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL11)
#endif
                   ) ||
                (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI ))
               &&( is_lte_to_gw_interrat == TRUE))
          {
            mm_serving_plmn.info.ps_data_suspend = FALSE;
          }
#endif 
          /* ----------------------------------------------------------------------
          ** (24.008, secion 4.7.3.1.5, abnormal case a) Access is granted after
          ** access class had been blocked.  GPRS attach is initiated
          ** ---------------------------------------------------------------------- */
          if ((gmm_state == GMM_DEREGISTERED) &&
              (gmm_substate == GMM_ATTACH_NEEDED))
          {
            if(gmm_check_if_ps_cell_access_allowed(ps_cell_access) == TRUE)
            {
              gmm_set_substate(GMM_NORMAL_SERVICE);
            }
#ifdef FEATURE_DSAC
            else if ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                     (mm_cs_access_barred_to_unbarred))
            {
              if (mm_state == MM_IDLE)
              {         
                mm_initiate_lu(MM_LU_CAUSE_NORMAL);
              }
              else
              {
                mm_pending_lu_request = NORMAL_LU;
              }     
              mm_cs_access_barred_to_unbarred = FALSE;                        
            }               
#endif  
          }

          /* ----------------------------------------------------------------------
          ** If there is a pending RAU because of access class blocking and now
          ** the access class restriction has been removed,  the RAU procedure
          ** is initiated
          ** ---------------------------------------------------------------------- */
          else if ((gmm_state == GMM_REGISTERED) &&
                  ((gmm_substate == GMM_UPDATE_NEEDED)||
                   (is_gsm_mode() && (gmm_multi_slot_ra_update_pending == TRUE))
#ifdef FEATURE_LTE
                   ||
                   ((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) && (gmm_ra_update_pending == TRUE))
#endif
                    ))
          {
            if (gmm_check_if_ps_cell_access_allowed(ps_cell_access) == TRUE)
            {
              /* Initiate RAU */
              gmm_update_ra_type();

              gmm_initiate_routing_area_update() ;
            }
#ifdef FEATURE_DSAC
            else if ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                     (mm_cs_access_barred_to_unbarred))
            {
              if (mm_state == MM_IDLE)
              {         
                mm_initiate_lu(MM_LU_CAUSE_NORMAL);
              }
              else
              {
                mm_pending_lu_request = NORMAL_LU;
              }   
              mm_cs_access_barred_to_unbarred = FALSE;                          
            }               
#endif
          }
          else if ((gmm_state == GMM_REGISTERED) &&
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                   (mm_cell_changed_to_cs_capable) && 
                   (mm_serving_plmn.ms_op_mode==SYS_SRV_DOMAIN_CS_PS))
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= CS domain now available: RAU with IMSI attach reqd");
            mm_cell_changed_to_cs_capable = FALSE;
            gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
            gmm_initiate_routing_area_update();
          }

          if ((gmm_state != GMM_NULL) &&
              ((gmm_substate == GMM_NO_CELL_AVAILABLE) ||
               (gmm_substate == GMM_PLMN_SEARCH)||
               (gmm_substate == GMM_LIMITED_SERVICE)) &&
               (gmm_check_if_ps_cell_access_allowed(ps_cell_access) == TRUE) &&
               (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode !=
                   SYS_NETWORK_SELECTION_MODE_LIMITED))
          {
            /* --------------------------
            ** Resumption of GPRS service
            ** -------------------------- */
            if ((mm_idle_substate == MM_LIMITED_SERVICE)&&
                 (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                 (mm_location_information.location_update_status == UPDATED ) &&
                 ((memcmp( &(mm_location_information.lai[0]), &(mm_serving_plmn.info.plmn.identity[0]), PLMN_SIZE ) == 0 ) &&
                   (memcmp( & (mm_location_information.lai[PLMN_SIZE]), &(mm_serving_plmn.lac.lac[0]), LAC_SIZE ) == 0 )) )
            {
              mm_substate_control(MM_STD_NORMAL_SERVICE);
            }
            gmm_set_substate( GMM_NORMAL_SERVICE );

            gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                             mm_serving_plmn.info.reg_service_domain,
                                             FALSE,
                                             SYS_SRV_STATUS_SRV,
                                             TRUE );

            if (mm_reg_waiting_for_reg_cnf)
            {
              if (mm_ready_to_send_reg_cnf)
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF NORMAL_SERV" );
                mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                mm_send_mmr_reg_cnf();
                mm_reg_waiting_for_reg_cnf = FALSE;
                mm_ready_to_send_reg_cnf   = FALSE;
              }
            }
            else
            {
              MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND : NORMAL_SERV" );
              mmr_service_ind.service_state = mm_serving_plmn.info;
              mm_send_mmr_service_ind( &mmr_service_ind );
            }
          }

          if((mm_serving_plmn.info.active_rat == gmm_last_signalling_sent_rat)&&(gmm_is_suspended == FALSE)
#ifdef FEATURE_GSM_GPRS
            && (gmm_rau_is_required == TRUE)
#endif /*FEATURE_GSM_GPRS*/
            &&(mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode !=
                 SYS_NETWORK_SELECTION_MODE_LIMITED)
/*4.7.1.7a Intersystem change from S1 mode to A/Gb mode or S1 mode to Iu mode with ISR activated*/
#ifdef FEATURE_NAS_REL11
            &&!((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
                 &&(gmm_is_prev_mode_connected)
				 &&(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL11))
#endif
            )
          {
#ifdef FEATURE_GSM_GPRS
            gmm_rau_is_required = FALSE;
#endif /*FEATURE_GSM_GPRS*/
             
            mm_serving_plmn.info.ps_data_suspend = FALSE;
            if (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_SRV)
            { 
              if (mm_reg_waiting_for_reg_cnf)
              {
                if (mm_ready_to_send_reg_cnf)
                {
                  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF NORMAL_SERV" );
                  mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                  mm_send_mmr_reg_cnf();
                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf   = FALSE;
                }
              }
              else
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND : NORMAL_SERV" );
                mmr_service_ind.service_state = mm_serving_plmn.info;
                mm_send_mmr_service_ind( &mmr_service_ind );
              }
            }
          }
#ifdef FEATURE_GSM_GPRS
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
          if ( (gmm_rau_is_required) && (gmm_is_prev_mode_connected) && 
               (gmm_substate != GMM_LIMITED_SERVICE ))
          {
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

          }
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
#endif /* #ifdef FEATURE_GSM_GPRS */

#ifdef FEATURE_LTE
    if (gmm_is_lte_cap_changed() &&
        (gmm_state == GMM_REGISTERED) &&
        (gmm_substate == GMM_NORMAL_SERVICE))
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= RAU needs to be performed after LTE RAT capabilities is changed");
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
#endif

#ifdef FEATURE_FEMTO_CSG
         if((mm_serving_csg_id != mm_serving_plmn.info.csg_info.csg_id) &&
             (gmm_state == GMM_REGISTERED || gmm_state == GMM_DEREGISTERED))
         {
           MSG_HIGH_DS_0(MM_SUB, "=MM= CSG: Serving CSG Id change - Sending service confirm/indication");
           gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                  mm_serving_plmn.info.reg_service_domain,
                  FALSE,
                  SYS_SRV_STATUS_SRV,
                  TRUE );
           
           if (mm_reg_waiting_for_reg_cnf)
           {
             if (mm_ready_to_send_reg_cnf)
             {
               mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
               mm_send_mmr_reg_cnf();
               mm_reg_waiting_for_reg_cnf = FALSE;
               mm_ready_to_send_reg_cnf   = FALSE;
             }
           }
           else
           {
             mmr_service_ind.service_state = mm_serving_plmn.info;
             mm_send_mmr_service_ind( &mmr_service_ind );
           }
         }
#endif 
#if defined(FEATURE_LTE) && defined(FEATURE_GSM_GPRS)
          if(is_gsm_mode() &&
             emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI &&
             is_lte_to_gw_interrat == TRUE)
          {
            /* Resume LLC */
            gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_LLC_RESUMPTION,FALSE,0);
          }
#endif
        }

#ifdef FEATURE_GSM_GPRS
        if ( (gmm_rau_is_required) && (gmm_ps_data_available) && 
             (gmm_substate != GMM_LIMITED_SERVICE) 
#ifdef FEATURE_WTOG_PS_HANDOVER
             #error code not present
#endif
             )
        {
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
        else if((gmm_multi_slot_ra_update_pending ==  TRUE) && (is_gsm_mode()))
        {
          if( (gmm_state != GMM_REGISTERED_INITIATED) &&
              ((is_serving_rai_changed == FALSE) || (gmm_state != GMM_ROUTING_AREA_UPDATING_INITIATED)) )
          {
            MSG_HIGH_DS_2(MM_SUB,"=MM= Processing the pending RAU request, gmm_state = %d, serving_rai_changed = %d ", gmm_state, is_serving_rai_changed);
            gmm_update_ra_type();
            gmm_initiate_routing_area_update();
          }
          else if(is_serving_rai_changed == FALSE)
          {
             MSG_HIGH_DS_1(MM_SUB,"=MM= Pended RAU will be processed after completion of current procedure, current gmm_state = %d",gmm_state);
          }
        
          else
          {
            gmm_multi_slot_ra_update_pending = FALSE;
            MSG_HIGH_DS_0(MM_SUB,"=MM= Not processing pended RAU due to MultiSlot capability change as ATTACH had been initiated");
          }

        }
        
#endif /* #ifdef FEATURE_GSM_GPRS */
        if((mmcoord_mmr_reg_cnf.service_state.plmn_service_capability ==SYS_SRV_CAPABILITY_CS_SERVICE_ONLY ) &&
           (mm_serving_plmn.info.plmn_service_capability == SYS_SRV_CAPABILITY_CS_PS_SERVICE))
        {

          /* If LU in progress or RAU/ATTACH in progress 
           * pend the sending of MMR_SERVICE_IND
           * todo!! --> Move this check in mm_send_mmr_service_ind 
           * */
          if((mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) ||
             ((gmm_state == GMM_REGISTERED_INITIATED) || 
              (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)) )
          {
             MSG_HIGH_DS_3( MM_SUB, "=MM= Not Sending MMR_SERVICE_IND in mmstate = %d, gmm_state = %d, NW op mode = %d", mm_state,gmm_state,mm_serving_plmn.network_op_mode );
          }
          else
          {
             MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND" );
             mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
             mmr_service_ind.service_state = mm_serving_plmn.info;
             mm_send_mmr_service_ind( &mmr_service_ind );
          }
        }

        if (gmm_previous_gprs_service_state == SERVICE_NOT_AVAILABLE)
        {
          mm_rat_handle_srv_status_update( SYS_SRV_STATUS_SRV );
        }
      } /* end else GPRS service available */
    } /* end else SERVICE_AVAILABLE */
  } /* end if (mmsim_sim_valid()) */
  /* ---------------------------------------------------------------------------
  ** Even though GPRS services are invalidated in SIM, GMM still needs to notify
  ** loss of service to REG when MS operation mode is SYS_SRV_DOMAIN_CS_PS or
  ** SYS_SRV_DOMAIN_PS_ONLY and RR/RRC notify loss of service on serving cell.
  ** --------------------------------------------------------------------------- */
  else if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS ||
             mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY )
             && (service_status == MM_AS_NO_SERVICE ||
                reg_state_is_no_service_to_be_send_due_to_china_mcc(mm_serving_plmn.info.plmn)))
  {
   if(reg_state_is_no_service_to_be_send_due_to_china_mcc(mm_serving_plmn.info.plmn) &&
      (rrc_connection_status != MMRRC_IDLE))
   {
     if(mm_state == MM_CONNECTION_ACTIVE)
     {
       mm_pending_no_service_to_reg= TRUE;
     }
     else if(rrc_connection_status != MMRRC_RELEASE_IN_PROGRESS)
     {
       rrc_cmd_type *rrc_cmd_p = NULL;
#ifdef FEATURE_WCDMA
       if (is_umts_mode())
       {
         rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
         if (rrc_cmd_p == NULL)
         {
            MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
         }
         rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
         rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
         rrc_cmd_p->cmd.rrc_abort_req.hard_abort = FALSE;
         rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
         rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = FALSE;
         MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RRC_ABORT_REQ");
         send_cmd_to_rrc(rrc_cmd_p, mm_serving_plmn.info.active_rat );
       }
       else
#endif
       if( is_gsm_mode())
       {
         MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RR_ABORT_REQ");
         mm_send_rr_abort_req(FALSE);
       }
    }
   }
   else
  {
    mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;

    if(reg_state_is_no_service_to_be_send_due_to_china_mcc(mm_serving_plmn.info.plmn))
    {
      mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
    gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                     mm_serving_plmn.info.reg_service_domain,
                                     FALSE,
                                     SYS_SRV_STATUS_NO_SRV,
                                       FALSE );
    }
    else
    {
       gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                        mm_serving_plmn.info.reg_service_domain,
                                        FALSE,
                                        SYS_SRV_STATUS_NO_SRV,
                                     TRUE );
    }

    /* Set LAI to invalid LAI */
    gmm_current_lai.PLMN_id.identity[0] = 0xFF;
    gmm_current_lai.PLMN_id.identity[1] = 0xFF;
    gmm_current_lai.PLMN_id.identity[2] = 0xFF;
    gmm_current_lai.location_area_code[0] = 0xFF;
    gmm_current_lai.location_area_code[1] = 0xFE;
    gmm_current_rac = 0xFF;

    if (mm_reg_waiting_for_reg_cnf)
    {
      if (mm_ready_to_send_reg_cnf)
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF NO_SERVICE");
        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
        mm_send_mmr_reg_cnf();
        mm_reg_waiting_for_reg_cnf = FALSE;
        mm_ready_to_send_reg_cnf   = FALSE;
      }
    }
    else
    {
      mmr_service_ind.service_state = mm_serving_plmn.info;
      mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
      mm_send_mmr_service_ind( &mmr_service_ind );
    }
   }
 //FR15492
  }

  if (mm_serving_plmn.info.plmn_service_capability == SYS_SRV_CAPABILITY_PS_SERVICE_ONLY)
  {
    gmm_previous_service_state      = mm_serving_plmn.gmm_service_state;
    gmm_previous_gprs_service_state = mm_serving_plmn.gmm_service_state;
  }
  else
  {
    gmm_previous_gprs_service_state = mm_serving_plmn.gmm_service_state;
    gmm_previous_service_state      = mm_serving_plmn.mm_service_state;
  }

#ifdef FEATURE_DSAC
  mm_cs_access_barred_to_unbarred = FALSE;
  mm_ps_access_barred_to_unbarred = FALSE;
#endif  
} /* end gmm_process_service_ind() */


boolean gmm_check_rau_for_integrity_protection( mm_cmd_type *message)
{
  boolean                    integrity_check_passed = FALSE ;
  gmm_ra_update_accept_msg_T ra_update_accept_msg;
  routing_area_id_T          rau_rai;
 
  if( gmm_ra_update_type == GMM_PERIODIC_UPDATING)
  {
   if (gmm_valid_routing_area_update_accept_message(
                                     message, &ra_update_accept_msg ))
   {

    rau_rai.plmn_id.identity[0] =
           ra_update_accept_msg.routing_area_identification[0];
    rau_rai.plmn_id.identity[1] =
           ra_update_accept_msg.routing_area_identification[1];
    rau_rai.plmn_id.identity[2] =
           ra_update_accept_msg.routing_area_identification[2];
    rau_rai.location_area_code.lac[0] =
           ra_update_accept_msg.routing_area_identification[3];
    rau_rai.location_area_code.lac[1] =
           ra_update_accept_msg.routing_area_identification[4];
    rau_rai.routing_area_code =
           ra_update_accept_msg.routing_area_identification[5];
   
   /*-----------------------------------------------------------------
   * RAU Accept message is processed without integrity protection only
   * for PERIODIC RAU with no change to RAI & PTMSI
   *----------------------------------------------------------------*/
         /*
     ** TS 24.008 4.1.1.1.1 V10.7.0 Integrity Protection for periodic update
     ** the MS performs periodic routing area updating without the activation of integrity protection
     ** only if there is no change of routing area or temporary identity, and the T3312 extended value 
     ** and the Network feature support value are not changed even if network has not activated the integrity protection  
     */
   if ( (gmm_plmn_changed( gmm_stored_rai.plmn_id, rau_rai.plmn_id)) ||
        (gmm_rai_changed ( gmm_stored_rai.location_area_code,
                                       gmm_stored_rai.routing_area_code,
                                      rau_rai.location_area_code, rau_rai.routing_area_code )) ||
         ((ra_update_accept_msg.allocated_ptmsi_present) &&
          (memcmp(ra_update_accept_msg.allocated_ptmsi, gmm_stored_ptmsi.digit,PTMSI_SIZE) != 0))
#ifdef FEATURE_NAS_REL11
          ||((ra_update_accept_msg.ms_identity_present) &&((ra_update_accept_msg.ms_identity.id_type & 0x7) == TMSI)&&
              (memcmp(ra_update_accept_msg.ms_identity.value, mm_location_information.tmsi,TMSI_SIZE) != 0))
#endif
#ifdef FEATURE_NAS_REL10
         || ((ra_update_accept_msg.nw_ftr_support_present) && 
             (gmm_stored_nw_feature_support  != ra_update_accept_msg.nw_ftr_support))
         || ((ra_update_accept_msg.ext_t3312_value_present) &&
              (gmm_t3312_timer_value != (gmm_convert_timer3_value_ie_to_msecs(
                                                  ra_update_accept_msg.ext_t3312_value))))
#endif   
      )
   {
        MSG_HIGH_DS_0(MM_SUB, "=MM= Serving RAI or PTMSI or NW Fetaure Support or Ex T3312 is changed ");
       integrity_check_passed = gmm_integrity_protection_activated;
   }
   else
   {
       integrity_check_passed = TRUE;
   }
   }
  
  }
  else
  {
     integrity_check_passed = gmm_integrity_protection_activated;
  }

#ifdef FEATURE_TDSCDMA
  if (is_tdscdma_mode() && !gmm_anite_gcf_flag)
  {
    integrity_check_passed = TRUE;
  }
#endif /* FEATURE_TDSCDMA */

  return integrity_check_passed;
}

/*===========================================================================

FUNCTION GMM_CHECK_RAU_REJECT_FOR_INTEGRITY_PROTECTION

DESCRIPTION
  Function to perform check for integrity for rau reject 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean gmm_check_attach_reject_for_integrity_protection(mm_cmd_type *message)
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

FUNCTION GMM_CHECK_RAU_REJECT_FOR_INTEGRITY_PROTECTION

DESCRIPTION
  Function to perform check for integrity for rau reject 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean gmm_check_rau_reject_for_integrity_protection(mm_cmd_type *message)
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
#ifdef FEATURE_NAS_ECALL
/*===========================================================================

FUNCTION GMM_PERFORM_ECALL_INACTIVITY

DESCRIPTION
  Function to perform PS deregistration if it is possible or else locally 
  detach for CS and PS services in NMO1 network else locally detach
  for Ps services

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_perform_ecall_inactivity(mm_cmd_type *message )
{
  mmr_service_ind_s_type     mmr_service_ind;
  
  if (mm_waiting_for_stop_mode_cnf)
  {
    gmm_set_state( GMM_DEREGISTERED );

    gmm_gprs_attach_pending = FALSE;
    gmm_ra_update_pending = FALSE ;
   
    gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                     SYS_SRV_DOMAIN_NO_SRV,
                                     FALSE,
                                     SYS_SRV_STATUS_LIMITED,
                                     FALSE );

    if (mm_reg_waiting_for_reg_cnf)
    {
      if (mm_ready_to_send_reg_cnf)
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF ");

        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
      
        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
        mm_send_mmr_reg_cnf();
        mm_reg_waiting_for_reg_cnf = FALSE;
        mm_ready_to_send_reg_cnf   = FALSE;
      }
    }
    else
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVIC_IND " );
      mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
      mmr_service_ind.service_state = mm_serving_plmn.info;
      mm_send_mmr_service_ind( &mmr_service_ind );
    }
  }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)  
  else if ((gmm_state == GMM_REGISTERED_INITIATED) ||
           (gmm_state == GMM_REGISTERED) ||
           (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
           (gmm_state == GMM_DEREGISTERED_INITIATED) ||
           (gmm_state == GMM_SERVICE_REQUEST_INITIATED))
  {
    gmm_initiate_gprs_detach( message );
  }
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/ 
  else if (gmm_state == GMM_DEREGISTERED)
  {
    gmm_gprs_attach_pending = FALSE;

    if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) ||
        ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
         (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)))
    {
      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY)
      {
        /* ---------------------------------------------------------------------
        ** Set ATT flag to FALSE so we don't attempt to perform multiple DETACHs
        ** --------------------------------------------------------------------- */
       mm_system_information.ATT = FALSE;
      }

      mm_handle_message( message );
    }
  }
}

#endif

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION gmm_handle_set_drx_req

DESCRIPTION
   Function to handle DRX change request

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_handle_set_drx_req(mm_cmd_type *message )
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  if(gmm_drx_cn_coeff_s1 == message->cmd.mmr_set_drx_req.drx_coefficient)
  {
    MSG_HIGH_DS_0(MM_SUB,"DRX received in SET_DRX_REQ and one being used by UE are same");
    mm_send_mmr_set_drx_cnf(TRUE);    
  }
  else if(gmm_state != GMM_REGISTERED || mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
  {
    MSG_HIGH_DS_0(MM_SUB,"Rejecting SET_DRX_REQ received when GMM is not in REGISTERED state or UE is not having PS service");
    mm_send_mmr_set_drx_cnf(FALSE);
  }
  else if (mm_rat_handler_status != MM_RAT_LO && mm_rat_handler_status != MM_RAT_INITIAL)
  {
    MSG_HIGH_DS_0(MM_SUB,"Rejecting SET_DRX_REQ received during G-W inter-rat");
    mm_send_mmr_set_drx_cnf(FALSE);
  }
  else 
  {
   //GMM is in registered state
    if((gmm_substate == GMM_NORMAL_SERVICE) ||
        (gmm_substate == GMM_ATTEMPTING_TO_UPDATE) ||
        (gmm_substate == GMM_ATTEMPTING_TO_UPDATE_MM))
    {
      gmm_set_drx_req_pending = TRUE;
  
      if(message->cmd.mmr_set_drx_req.drx_coefficient == SYS_DRX_NOT_SPECIFIED_BY_MS)
      {
        gmm_drx_cn_coeff_s1 = gmm_nv_drx_cn_coeff_s1;
      }
      else 
      {
        gmm_drx_cn_coeff_s1 = message->cmd.mmr_set_drx_req.drx_coefficient;
      }

      //DRX should be included in both RAU/TAU
      emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent = NAS_SEND_DRX_ON_ALL_RATS;

      if ((mm_timer_status[TIMER_T3311] == TIMER_STOPPED) &&
           (mm_timer_status[TIMER_T3302] == TIMER_STOPPED) ) 
      {
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
    }
    else 
    {
      MSG_HIGH_DS_1(MM_SUB,"Rejecting SET_DRX_REQ received when GMM is REGISTERED and substate is %d", gmm_substate);
      mm_send_mmr_set_drx_cnf(FALSE);
    }
  }
 
}

#endif /*FEATURE_LTE*/

/*===========================================================================

FUNCTION    MM_PROCESS_IMS_VOICE_TAG_REG_STATUS_IND

DESCRIPTION
  This functions sets the IMS registration status parameter in emm ctrl data. 
  Also performs RAU if IMS registration status is changing from not registered to registered 
  and if ISR is ON, 

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  
NOTE:
  For now we wont hit this part of the code as IMS over GW is not implemented. But when it is
  implemented we need to check what other GMM states and substates this primitive will be 
  received and make the changes accordingly

===========================================================================*/
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
void gmm_process_ims_voice_tag_reg_status_ind
(
  boolean is_ims_registered
)
{
  boolean is_ims_reg_status_changed = FALSE;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS_2(MM_SUB,"=MM= Previous IMS reg status %d New IMS reg status %d",
        emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag,
        is_ims_registered);

  if(emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag != is_ims_registered)
  {
    is_ims_reg_status_changed = TRUE;
    emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag =
                                                                              is_ims_registered;
  }

  /* If IMS registration status is changing from not registered to registered and if ISR is ON, then
  ** perform TAU (24.301 Section 5.5.3.3.2)
  */
  if((is_ims_reg_status_changed == TRUE) &&
     (mm_emm_is_mmit_enabled() == TRUE) &&
     (gmm_state == GMM_REGISTERED))
  {
    if(mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
    {
      gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
    }
    else
    {
      gmm_ra_update_type = GMM_RA_UPDATING;
    }
    gmm_initiate_routing_area_update();
  }
}
#endif

/*===========================================================================

FUNCTION GMM_HANDLE_REGISTRATION

DESCRIPTION
  Function to route message relating to registration to the function
  dealing with it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_handle_registration( mm_cmd_type *message )
{
  byte gmm_message_type;
  mmr_service_ind_s_type     mmr_service_ind;

  sm_cmd_type                *sm_cmd = NULL;
  boolean                    is_cell_forbidden = FALSE;
  boolean                    is_serving_rai_changed = FALSE;
#ifdef FEATURE_ENHANCED_NW_SELECTION
   byte                             foreground_timer_expiry_lai[LAI_SIZE];
   byte                             rac;
#endif
#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (message == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= Unexpected NULL pointer", 0,0,0 );
  }

  switch (message->cmd.hdr.message_set)
  {
#ifdef FEATURE_GSM_GPRS
    case MS_MM_RR:
      switch (message->cmd.hdr.message_id)
      {
        case RR_DEACT_CNF:
          if ((gmm_state == GMM_DEREGISTERED) || (gmm_state == GMM_NULL))
          {
            gmm_set_state( GMM_NULL );
            gmm_set_substate( GMM_NULL_SUBSTATE );
          }
          else
          {
            MSG_ERROR_DS_1( MM_SUB, "=MM= RR_DEACT_CNF received in unexpected state %d", gmm_state );
          }
          break;

        case RR_PLMN_SELECT_CNF:
          /* ---------------------------------------------------
          ** If SIM is invalid then GMM substate remains NO_IMSI
          ** --------------------------------------------------- */
          if (mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY))
          {
            if (message->cmd.rr_plmn_select_cnf.service_state == MM_AS_NO_SERVICE)
            {
              /* ----------------------
              ** Set RAI to invalid LAI
              ** ---------------------- */
              gmm_current_lai.PLMN_id.identity[0] = 0xFF;
              gmm_current_lai.PLMN_id.identity[1] = 0xFF;
              gmm_current_lai.PLMN_id.identity[2] = 0xFF;
              gmm_current_lai.location_area_code[0] = 0xFF;
              gmm_current_lai.location_area_code[1] = 0xFE;
              gmm_current_rac = 0xFF;

              gmm_set_substate( GMM_NO_CELL_AVAILABLE );
            }
            else /* SERVICE_AVAILABLE */
            {
              boolean routine_man_selection_failed = FALSE;

#if defined(FEATURE_LTE) && defined (FEATURE_LTE_REL9)
              gmm_handle_emc_ps_srv(emm_ctrl_data_ptr);
#endif
              gmm_current_lai.PLMN_id.identity[0]   = mm_serving_plmn.info.plmn.identity[0];
              gmm_current_lai.PLMN_id.identity[1]   = mm_serving_plmn.info.plmn.identity[1];
              gmm_current_lai.PLMN_id.identity[2]   = mm_serving_plmn.info.plmn.identity[2];
              gmm_current_lai.location_area_code[0] = mm_serving_plmn.lac.lac[0];
              gmm_current_lai.location_area_code[1] = mm_serving_plmn.lac.lac[1];
              gmm_current_rac                       = mm_serving_plmn.rac;

              gmm_is_suspended = message->cmd.rr_plmn_select_cnf.gmm_suspended;
              MSG_HIGH_DS_1(MM_SUB, "=MM= GMM Suspend Ind - %d" , message->cmd.rr_plmn_select_cnf.gmm_suspended);
#ifdef FEATURE_LTE
              if(gmm_is_suspended)
              {
                emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_PENDING;
              }
#endif
              if (!((PLMN_MATCH( gmm_current_lai.PLMN_id.identity,
                                 mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity )) ||
                    (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
                     sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))  &&
                  (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                    SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
              {
                routine_man_selection_failed = TRUE;
                MSG_HIGH_DS_0(MM_SUB, "=MM= Different PLMN found during MAN_LIMITED_ACQ");
              }

              if (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
              {
                /* --------------------------------------------------
                ** The cell is not supporting GPRS (24.008 4.2.4.1.1)
                ** -------------------------------------------------- */
                gmm_set_substate( GMM_NO_CELL_AVAILABLE );
              }
              else
              {
                mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);
               
                /* Move GMM substate GMM_LIMITED_SERVICE if REG requested limited service */
                if (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode
                           == SYS_NETWORK_SELECTION_MODE_LIMITED)
                {
                  gmm_set_substate( GMM_LIMITED_SERVICE );
                }
                else if ((routine_man_selection_failed) ||
                         (((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                     SYS_NETWORK_SELECTION_MODE_AUTOMATIC )||
                           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                     SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) && 
                         (
#ifdef FEATURE_DUAL_SIM
                          (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
                          (reg_sim_per_subs_plmn_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
#else
                          (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn )) ||
                          (reg_sim_plmn_forbidden(gmm_current_lai.PLMN_id)) ||
#endif
                          (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
                          (mm_check_forbidden_regional_roaming( &gmm_current_lai )))))
                {
                  gmm_set_substate( GMM_LIMITED_SERVICE );
                }
                /* Check AC blocking restrictions */
                else if (gmm_check_if_ps_cell_access_allowed(ps_cell_access) == FALSE)
                {
                  if (gmm_state == GMM_REGISTERED)
                  {
                    if ((gmm_update_status != GMM_GU1_UPDATED)
#ifdef FEATURE_LTE
                        || (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) 
#endif
                    )
                    {
                      gmm_set_substate(GMM_UPDATE_NEEDED);
                    }
                    else
                    {
                      gmm_set_substate(GMM_LIMITED_SERVICE);
                    }
                  }
                  else
                  {                
                    gmm_set_substate(GMM_ATTACH_NEEDED);
                  }                    
                }
#ifdef FEATURE_SGLTE
                else if (MM_SUB_IS_SGLTE &&
                         mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY) &&
                         (mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
                         ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY )||
                         (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                                    SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)))
                      
                {
                  if ((gmm_rai_changed( gmm_previous_serving_rai.location_area_code,
                                        gmm_previous_serving_rai.routing_area_code,
                                        mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                      (gmm_plmn_changed( gmm_previous_serving_rai.plmn_id,
                                         mm_serving_plmn.info.plmn)))
                  {
                    is_serving_rai_changed = TRUE;
                    MSG_HIGH_DS_0( MM_SUB, "=MM= RAI changed");
                  }
              
                  if((mm_timer_status[TIMER_T3302] == TRUE) &&
                      (is_serving_rai_changed == FALSE))
                  {
                   if (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
                     {
                       mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CAMPED_ONLY;
                     }
                     else
                     {
                        if (mm_serving_plmn.info.plmn_reg_type != SYS_REG_DOMAIN_CS_SERVICE_ONLY)
                        {
                          mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CAMPED_ONLY;
                        }
              }
            }
                  else
                  {

                    if ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                       (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_PS))
                    {
                       mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CS_PS_SERVICE;
                    }
                    else if (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_PS_ONLY)
                    {
                      mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_PS_SERVICE_ONLY;
                    }
                    else 
                    {
                      if (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_PS)
                       {
                         if ( mm_serving_plmn.info.plmn_reg_type == SYS_REG_DOMAIN_CS_SERVICE_ONLY)
                         {
                           mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CS_PS_SERVICE;
                         }
                         else
                         {
                            mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_PS_SERVICE_ONLY;
                         }
                         
                       }
                    }  
                  }
                  gmm_set_substate( GMM_LIMITED_SERVICE );
                }
#endif

              }
            }
          } /* end if (mmsim_sim_valid()) */

          if (mm_serving_plmn.info.plmn_service_capability ==
                SYS_SRV_CAPABILITY_PS_SERVICE_ONLY)
          {
            gmm_previous_service_state      = mm_serving_plmn.gmm_service_state;
            gmm_previous_gprs_service_state = mm_serving_plmn.gmm_service_state;
          }
          else
          {
            gmm_previous_gprs_service_state = mm_serving_plmn.gmm_service_state;
            gmm_previous_service_state      = mm_serving_plmn.mm_service_state;
          }
          break;

        case RR_SERVICE_IND:
        {
          service_state_type gmm_saved_previous_service_state = gmm_previous_service_state;
          gmm_is_suspended = message->cmd.rr_service_ind.gmm_suspended;
#ifdef FEATURE_LTE
          if(gmm_is_suspended)
          {
            emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_PENDING;
          }
#endif
          MSG_HIGH_DS_1(MM_SUB, "=MM= GMM Suspend Ind - %d" , message->cmd.rr_service_ind.gmm_suspended);
#ifdef FEATURE_NAS_FREE_FLOAT
          if(message->cmd.rr_service_ind.cell_changed)
          {
            gmmllc_reset_service_state();
          }
#endif
          gmm_process_service_ind( message );
          gmmllc_handle_grr_event( message );
          if ((gmm_saved_previous_service_state != mm_serving_plmn.mm_service_state
               || (is_gsm_mode() && message->cmd.rr_service_ind.cell_changed)) && mm_state == MM_IDLE)
          {
#ifdef FEATURE_GSTK          
           mm_send_gstk_mm_idle_info();
#endif
          }
        }
          break;
          
#ifdef FEATURE_GSM_DTM
        case RR_SERVICE_IND_FROM_OTA_MSG:
        {
          service_state_type gmm_saved_previous_service_state = gmm_previous_service_state;
          gmm_is_suspended = message->cmd.rr_service_ind_from_ota_msg.gmm_suspended;
#ifdef FEATURE_LTE
          if(gmm_is_suspended)
          {
            emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_PENDING;
          }
#endif
          gmm_process_service_ind( message );
          gmmllc_handle_grr_event( message );
          if ((gmm_saved_previous_service_state != mm_serving_plmn.mm_service_state
               || (is_gsm_mode() && message->cmd.rr_service_ind_from_ota_msg.cell_changed)) && mm_state == MM_IDLE)
          {
#ifdef FEATURE_GSTK          
            mm_send_gstk_mm_idle_info();
#endif
          }
        }
          break;
#endif /* FEATURE_GSM_DTM */

        default:
          MSG_ERROR_DS_1( MM_SUB, "=MM= Unexpected RR primitive has been reseived (id=%d)",
                message->cmd.hdr.message_id );
          break;
      }
      break ;
#endif /* #ifdef FEATURE_GSM_GPRS */
    case MS_MM_REG:
      switch (message->cmd.hdr.message_id)
      {
        case MMR_STOP_MODE_REQ:
          if ((gmm_state == GMM_REGISTERED) &&
              (gmm_substate == GMM_ATTEMPTING_TO_UPDATE))
          {
            if (gmm_processing_directed_signalling_reestablishment)
            {
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)            
              gmm_initiate_gprs_detach( message );
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
              mmcoord_pending_reg_message.cmd.mmr_stop_mode_req = message->cmd.mmr_stop_mode_req;
            }
            else
            {
              gmm_sm_send_gprs_rel_ind_if_needed();
              gmm_set_state(GMM_DEREGISTERED);

              if (((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) &&
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)) ||
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2) ||
                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3))
              {
                mm_system_information.ATT = FALSE;
              }
              mm_handle_message( message );
            }
          }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)          
          else if ((gmm_state == GMM_REGISTERED_INITIATED) ||
                   (gmm_state == GMM_REGISTERED) ||
                   (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
                   (gmm_state == GMM_DEREGISTERED_INITIATED) ||
                   (gmm_state == GMM_SERVICE_REQUEST_INITIATED))
          {

            /* If PS SMS is pending, send connection reject here */
            if ((mm_cnm_is_active != TRUE) && (mm_sms_is_active == TRUE) &&
                 (gmm_service_request_initiator == MS_CC_MM) && (pmm_mode != PMM_CONNECTED))
            {
              /* check if ABORT_MSG_RECEIVED will cause WMS to retry */
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
              mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, ABORT_MSG_RECEIVED, MMCC_SMS_PS);
#else
              mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, ABORT_MSG_RECEIVED);
#endif
            }

            gmm_initiate_gprs_detach( message );

            /* -------------------------------------------
            ** Save message data for post processing by MM
            ** ------------------------------------------- */
            mmcoord_pending_reg_message.cmd.mmr_stop_mode_req = message->cmd.mmr_stop_mode_req;
          }
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/          
          else if (gmm_state == GMM_DEREGISTERED)
          {
            gmm_set_state( GMM_NULL );
            gmm_set_substate( GMM_NULL_SUBSTATE );

            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) ||
                ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)))
            {
              if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY)
              {
                /* ---------------------------------------------------------------------
                ** Set ATT flag to FALSE so we don't attempt to perform multiple DETACHs
                ** --------------------------------------------------------------------- */
                mm_system_information.ATT = FALSE;
              }

              mm_handle_message( message );
            }
          }
          else if (gmm_state == GMM_NULL)
          {
            MSG_ERROR_DS_0( MM_SUB, "=MM= Command received in unexpected state");
            mm_handle_message( message );
          }
          break;

        case MMR_REG_REQ:
          gmm_previous_serving_rai.plmn_id                   = gmm_current_lai.PLMN_id;
          gmm_previous_serving_rai.location_area_code.lac[0] =
            gmm_current_lai.location_area_code[0];
          gmm_previous_serving_rai.location_area_code.lac[1] =
            gmm_current_lai.location_area_code[1];
          gmm_previous_serving_rai.routing_area_code         = gmm_current_rac;

          if ((message->cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
              (message->cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_PS))
          {
            gmm_gprs_detach_pending = FALSE;

            if ((message->cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
                (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
                 (((gmm_state == GMM_REGISTERED_INITIATED) || (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)) && mm_serv_change_pending))) 
            {
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)            
              gmm_initiate_gprs_detach( message );
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

              /* -------------------------------------------
              ** Save message data for post processing by MM
              ** ------------------------------------------- */
              mmcoord_pending_reg_message.cmd.mmr_reg_req = message->cmd.mmr_reg_req;
            }
#ifndef FEATURE_CLASS_B_OPERATION_IN_NMO3
            else if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
                     (message->cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_PS) &&
                     (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3))
            {
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)            
              gmm_initiate_gprs_detach( message );
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

              /* -------------------------------------------
              ** Save message data for post processing by MM
              ** ------------------------------------------- */
              mmcoord_pending_reg_message.cmd.mmr_reg_req = message->cmd.mmr_reg_req;
            }
#endif
            else
            {
              if (gmm_state == GMM_NULL)
              {
                /* ----------------------------------------------------------
                ** The state GMM-DEREGISTERED is entered when GPRS capability
                ** has been enabled in the MS
                ** ---------------------------------------------------------- */
                gmm_set_state( GMM_DEREGISTERED );

                if (!mmsim_sim_valid( SYS_SRV_DOMAIN_PS_ONLY ))
                {
                  gmm_set_substate( GMM_NO_IMSI );
                }
                else if (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE)
                {
#ifdef FEATURE_NAS_REL10
                  if(mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
                  {
                    gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH);
                  }
                  else
#endif
                  {
                  gmm_set_substate( GMM_NORMAL_SERVICE );
                }
                }
                else
                {
                  gmm_set_substate( GMM_NO_CELL_AVAILABLE );
                }
              }
              else if ((gmm_state == GMM_DEREGISTERED_INITIATED) ||
                       ((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) && gmm_detach_after_rau_pending))
              {
                gmm_set_state( GMM_DEREGISTERED );

                sm_cmd = sm_get_cmd_buf();

                if (sm_cmd == NULL)
                {
                  MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
                }

                sm_cmd->header.message_set = MS_GMM_SM;
                sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                sm_cmd->cmd.gmm_gprs_rel_ind.cause = NO_GPRS_CONTEXT;
#ifdef FEATURE_DUAL_SIM
                sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

                 MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

                sm_put_cmd( sm_cmd );
                gmm_detach_after_rau_pending = FALSE;
                mm_ready_to_send_reg_cnf = TRUE;
              }

              if (mm_serv_change_pending)
              {
                if ((mm_state == MM_IDLE) &&
                    ((mm_idle_substate != MM_PLMN_SEARCH) &&
                     (mm_idle_substate != MM_PLMN_SEARCH_NORMAL_SERVICE)))
                {
                  mm_change_service_domain();
                  mm_serv_change_pending = FALSE;
                }
                else if (is_umts_mode() && (mm_state != MM_IDLE))
                {
                  if (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
                  {
                    /* --------------------------------------------------
                    ** The cell is not supporting GPRS (24.008 4.2.4.1.1)
                    ** -------------------------------------------------- */
                    gmm_set_substate( GMM_NO_CELL_AVAILABLE );

                    if (mm_reg_waiting_for_reg_cnf)
                    {
                      if (mm_ready_to_send_reg_cnf)
                      {
                        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                        mm_send_mmr_reg_cnf();
                        mm_reg_waiting_for_reg_cnf = FALSE;
                        mm_ready_to_send_reg_cnf   = FALSE;
                        mm_serv_change_pending = FALSE;
                      }
                    }
                  }
                  else
                  {
                    gmm_set_substate( GMM_NORMAL_SERVICE );
                  }
                }
              }
              else if ((mm_rat_change_pending) && (mm_state == MM_IDLE))
              {
                mm_stop_current_rat();
              }

              if (mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY))
              {
                /* ----------------------------------------------------------
                ** Set GMM substate to PLMN_SEARCH until MMCOORD signals that
                ** the correct PLMN has been located or not
                ** ---------------------------------------------------------- */
                if (!mm_already_camped( message))
                {
                  gmm_set_substate( GMM_PLMN_SEARCH );
                }

                if (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                   SYS_NETWORK_SELECTION_MODE_LIMITED)
                {
                  gmm_set_substate(GMM_LIMITED_SERVICE);
                }
              }
              else
              {
                gmm_set_substate( GMM_NO_IMSI );
              }
            }
          }
          else if (message->cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_ONLY)
          {
            if (((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
                 (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) ||
                 (gmm_state == GMM_REGISTERED_INITIATED) ||
                 (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)) &&
                (mm_rat_change_pending || mm_serv_change_pending))
            {
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
              /* If state is RAU initiated and detach after RAU pending, don't abort RAU
               * Instead, let RAU complete and do DETACH after RAU  */		    
              if((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) && (gmm_detach_after_rau_pending == TRUE)) 
              {
               MSG_HIGH_DS_0( MM_SUB, "If detach is pending after current RAU, don't abort ongoing RAU"); 
              }
              else
              {
                gmm_initiate_gprs_detach( message ); 
              }
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

              /* -------------------------------------------
              ** Save message data for post processing by MM
              ** ------------------------------------------- */
              mmcoord_pending_reg_message.cmd.mmr_reg_req =
                message->cmd.mmr_reg_req;
            }
            else
            {
              gmm_set_state( GMM_NULL );
              gmm_set_substate( GMM_NULL_SUBSTATE );

              sm_cmd = sm_get_cmd_buf();

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

              gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               SYS_SRV_DOMAIN_NO_SRV,
                                               FALSE,
                                               mm_serving_plmn.info.service_status,
                                               FALSE );


              if (mm_serv_change_pending)
              {
                /*-------------------------------------------------------------------
                ** CS+PS to CS only, send CCCH camp indication 
                ** instead of new service request only if MM is not IDLE.
                ** ------------------------------------------------------------------- */
                if (mm_state != MM_IDLE)
                {
#ifdef FEATURE_GSM_GPRS                
#ifdef FEATURE_NAS_FREE_FLOAT
                  gmmllc_reset_service_state ();
#else
                  glif_grr_send_camp_on_ccch_indication();
#endif 
#endif/*#ifdef FEATURE_GSM_GPRS*/
                }
                else
                {
                  mm_change_service_domain();
                }
                mm_serv_change_pending = FALSE;
              }
            }
          }
          break;

        case MMR_PLMN_SEARCH_REQ:
          if (is_umts_mode())
          {
            if (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
            {
              if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
              {
                /* ------------------------------------------------------------------
                ** Lower layer failure - the procedure shall be aborted.  Timer T3330
                ** shall be stopped if still running.  The routing area updating
                ** attempt counter shall be incremented.
                ** ------------------------------------------------------------------ */
                gmm_increment_routing_area_update_attempt_counter
                (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                  FALSE
#endif
                );
                mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
              }
              else if (gmm_state == GMM_REGISTERED_INITIATED)
              {
                /* -------------------
                ** Lower layer failure
                ** ------------------- */
                gmm_increment_gprs_attach_attempt_counter();
                mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);

                /* -------------------
                ** TS 24.008 4.2.4.2.2
                ** ------------------- */
                gprs_attach_upon_entry_to_a_new_cell = TRUE;
              }
              else if (gmm_state == GMM_DEREGISTERED_INITIATED)
              {
                /* ----------------------------------------------------------------
                ** Treat as a lower layer failure - the procedure shall be aborted.
                ** ---------------------------------------------------------------- */
                gmm_set_state( GMM_DEREGISTERED );
                sm_cmd = sm_get_cmd_buf();

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
              }
            }
          }
          break;
#ifdef FEATURE_NAS_ECALL
        case MMR_ECALL_MODE_IND:
          gmm_perform_ecall_inactivity(message);
          break;
#endif

#ifdef FEATURE_LTE
        case MMR_SET_DRX_REQ:
          gmm_handle_set_drx_req(message);   
          break;
#endif /*FEATURE_LTE*/
        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected REG message %d", message->cmd.hdr.message_id, 0,0 );
      } /* end switch (message->message_id) */
      break;

    case MS_MM_RRC:
    case MS_MM_TDSRRC:
      switch (message->cmd.hdr.message_id)
      {
        case RRC_DEACT_CNF:
        case RRC_MODE_CHANGE_CNF:
          if ((gmm_state == GMM_DEREGISTERED) || (gmm_state == GMM_NULL))
          {
            gmm_set_state( GMM_NULL );
            gmm_set_substate( GMM_NULL_SUBSTATE );
          }
          else
          {
            MSG_ERROR_DS_1( MM_SUB, "=MM= RRC_DEACT_CNF received in unexpected state %d", gmm_state );
          }
          break;

        case RRC_SERVICE_CNF:
          /* ---------------------------------------------------
          ** If SIM is invalid then GMM substate remains NO_IMSI
          ** --------------------------------------------------- */
          if (mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY))
          {
            if (message->cmd.rrc_service_cnf.service_status == MM_AS_NO_SERVICE)
            {
              /* ----------------------
              ** Set RAI to invalid LAI
              ** ---------------------- */
              gmm_current_lai.PLMN_id.identity[0] = 0xFF;
              gmm_current_lai.PLMN_id.identity[1] = 0xFF;
              gmm_current_lai.PLMN_id.identity[2] = 0xFF;
              gmm_current_lai.location_area_code[0] = 0xFF;
              gmm_current_lai.location_area_code[1] = 0xFE;
              gmm_current_rac = 0xFF;

              gmm_set_substate( GMM_NO_CELL_AVAILABLE );
            }
            else /* SERVICE_AVAILABLE */
            {
              boolean routine_man_selection_failed = FALSE;
       #ifdef FEATURE_FEMTO_CSG
              boolean is_manual_csg_selection_success = FALSE;
       #endif 

#if defined(FEATURE_LTE) && defined (FEATURE_LTE_REL9)
              gmm_handle_emc_ps_srv(emm_ctrl_data_ptr);
#endif
              gmm_current_lai.PLMN_id.identity[0]   = mm_serving_plmn.info.plmn.identity[0];
              gmm_current_lai.PLMN_id.identity[1]   = mm_serving_plmn.info.plmn.identity[1];
              gmm_current_lai.PLMN_id.identity[2]   = mm_serving_plmn.info.plmn.identity[2];
              gmm_current_lai.location_area_code[0] = mm_serving_plmn.lac.lac[0];
              gmm_current_lai.location_area_code[1] = mm_serving_plmn.lac.lac[1];
              gmm_current_rac                       = mm_serving_plmn.rac;


              if (!(((PLMN_MATCH( gmm_current_lai.PLMN_id.identity,
                                 mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity )) ||
                    (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
                     sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))
     #ifdef FEATURE_FEMTO_CSG
                   && (mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
     #endif
     #if defined (FEATURE_LTE) && defined (FEATURE_DUAL_DATA)
                   && (mm_is_volte_registration_allowed(mm_serving_plmn.info.plmn, FALSE) == TRUE)
     #endif
                   )  &&
                  (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                    SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
              {
                routine_man_selection_failed = TRUE;
                MSG_HIGH_DS_0(MM_SUB, "=MM= Different PLMN found during MAN_LIMITED_ACQ");
              }

#ifdef FEATURE_FEMTO_CSG
              if ((mmcoord_pending_reg_message.cmd.mmr_reg_req.csg_id == mm_serving_plmn.info.csg_info.csg_id) &&
                      (sys_plmn_match(mm_serving_plmn.info.plmn, mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn)) && 
                    (mmcoord_pending_reg_message.cmd.mmr_reg_req.csg_id != SYS_CSG_ID_INVALID))
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= CSG: Manual CSG selection success");
                is_manual_csg_selection_success = TRUE;
              }
#endif

              if (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
              {
                /* --------------------------------------------------
                ** The cell is not supporting GPRS (24.008 4.2.4.1.1)
                ** -------------------------------------------------- */
                gmm_set_substate( GMM_NO_CELL_AVAILABLE );
              }
              else
              {
                mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);              

                if(
#ifdef FEATURE_DUAL_SIM
                   (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
                   (reg_sim_per_subs_plmn_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
#else
                   (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn )) ||
                   (reg_sim_plmn_forbidden(gmm_current_lai.PLMN_id)) ||
#endif
                   (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
                   (mm_check_forbidden_regional_roaming( &gmm_current_lai ))
#ifdef FEATURE_FEMTO_CSG
                 || !mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info)
#endif 
#if defined (FEATURE_LTE) && defined (FEATURE_DUAL_DATA)
                 || (mm_is_volte_registration_allowed(mm_serving_plmn.info.plmn, FALSE) == FALSE)
#endif
                     )
                {
                  is_cell_forbidden = TRUE;
                }

                /* Move GMM substate GMM_LIMITED_SERVICE if REG requested limited service */
                if (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode
                           == SYS_NETWORK_SELECTION_MODE_LIMITED)
                {
                  gmm_set_substate( GMM_LIMITED_SERVICE );
                }
                else if ((routine_man_selection_failed) ||
                         (((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                     SYS_NETWORK_SELECTION_MODE_AUTOMATIC )||
                           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                     SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) && 
                         (is_cell_forbidden 
#ifdef FEATURE_FEMTO_CSG
                          /*Not manual CSG selection*/
                         &&(!is_manual_csg_selection_success)
#endif 
                        )))
                {
                  gmm_set_substate( GMM_LIMITED_SERVICE );
                }
                else if ((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
                         ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY )||
                         (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                                       SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)))
                        
                {
                  if ((gmm_rai_changed( gmm_previous_serving_rai.location_area_code,
                                        gmm_previous_serving_rai.routing_area_code,
                                        mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                      (gmm_plmn_changed( gmm_previous_serving_rai.plmn_id,
                                         mm_serving_plmn.info.plmn)))
                  {
                    is_serving_rai_changed = TRUE;
                    MSG_HIGH_DS_0( MM_SUB, "=MM= RAI changed");
                  }

                  if((is_cell_forbidden == TRUE) || 
                     ((mm_timer_status[TIMER_T3302] == TRUE) &&
                      (is_serving_rai_changed == FALSE)))
                  {
#ifdef FEATURE_SGLTE                    
                    if(MM_SUB_IS_SGLTE)
                    {
                       if((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) ||
                          (mm_serving_plmn.info.plmn_reg_type != SYS_REG_DOMAIN_CS_SERVICE_ONLY))
                    {
                         mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CAMPED_ONLY;
                       }
                    }
                    else
#endif
                    {
                      mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CAMPED_ONLY;
                    }
                  }
                  else
                  {
#ifdef FEATURE_SGLTE
                    if(MM_SUB_IS_SGLTE)
                    {
                      if ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                         (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_PS))
                      {
                         mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CS_PS_SERVICE;
                      }
                      else if (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_PS_ONLY)
                      {
                        mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_PS_SERVICE_ONLY;
                      }
                      else 
                      {
                        if (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_PS)
                         {
                           if ( mm_serving_plmn.info.plmn_reg_type == SYS_REG_DOMAIN_CS_SERVICE_ONLY)
                           {
                             mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CS_PS_SERVICE;
                           }
                           else
                           {
                              mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_PS_SERVICE_ONLY;
                           }
                           
                         }
                      }  
                    }
                    else
#endif
                {
                    mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_PS_SERVICE_ONLY;
                    }
                  }

                  gmm_set_substate( GMM_LIMITED_SERVICE );
                }
                /* Check AC blocking restrictions */
                else if (gmm_check_if_ps_cell_access_allowed(ps_cell_access) == FALSE)
                {
                  if (gmm_state == GMM_REGISTERED)
                  {
                    if ((gmm_update_status != GMM_GU1_UPDATED)
#ifdef FEATURE_LTE
                        || (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) 
#endif
                    )
                    {
                      gmm_set_substate(GMM_UPDATE_NEEDED);
                    }
                    else
                    {
                      gmm_set_substate(GMM_LIMITED_SERVICE);
                    }
                  }
                  else
                  {                
                    gmm_set_substate(GMM_ATTACH_NEEDED);
                  }                    
                }
              }
            }
          } /* end if (mmsim_sim_valid()) */

          if (mm_serving_plmn.info.plmn_service_capability ==
                SYS_SRV_CAPABILITY_PS_SERVICE_ONLY)
          {
            gmm_previous_service_state      = mm_serving_plmn.gmm_service_state;
            gmm_previous_gprs_service_state = mm_serving_plmn.gmm_service_state;
          }
          else
          {
            gmm_previous_gprs_service_state = mm_serving_plmn.gmm_service_state;
            gmm_previous_service_state      = mm_serving_plmn.mm_service_state;
          }
          break;

        case RRC_SERVICE_IND:
          gmm_process_service_ind( message);
          break;

        case RRC_SERVICE_IND_FROM_OTA_MSG:
          gmm_process_service_ind( message );
          break;

        case MM_DATA_IND:
          gmm_message_type = message->cmd.mm_data_ind.L3_data[1];

          switch (gmm_message_type)
          {
            case GMM_ATTACH_ACCEPT:
              MSG_HIGH_DS_0( MM_SUB, "=MM= Received ATTACH ACCEPT" );
#ifdef FEATURE_GSM_GPRS
              if ((is_umts_mode() && (gmm_integrity_protection_activated
#ifdef FEATURE_TDSCDMA
                                      || (is_tdscdma_mode() && !gmm_anite_gcf_flag)
#endif /* FEATURE_TDSCDMA */
                                      )) || is_gsm_mode())
#else
              if (gmm_integrity_protection_activated
#ifdef FEATURE_TDSCDMA
                  || (is_tdscdma_mode() && !gmm_anite_gcf_flag)
#endif /* FEATURE_TDSCDMA */
                  )
#endif
              {
                if ((gmm_state == GMM_REGISTERED_INITIATED) ||
                    ((gmm_state == GMM_REGISTERED) && is_gsm_mode()))
                {
                  gmm_handle_gprs_attach( message );
                }
                else if (gmm_state == GMM_DEREGISTERED_INITIATED)
                {
                  MSG_ERROR_DS_0( MM_SUB, "=MM= Detach Procedure in Progress, so this Attach is not valid");
                }                
                else
                {
                  gmm_send_gmm_status_message(
                    MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE );
                }
              }
              else
              {
                MSG_ERROR_DS_0( MM_SUB, "=MM= Integrity protection not activated");
              }
              break;

            case GMM_ATTACH_REJECT:
              MSG_HIGH_DS_1
              (
                MM_SUB, "=MM= Received ATTACH REJECT Cause 0x%x",
                message->cmd.mm_data_ind.L3_data[2]
              );
              if(gmm_check_attach_reject_for_integrity_protection(message))
              {
              if (gmm_state == GMM_REGISTERED_INITIATED)
              {
                gmm_handle_gprs_attach( message );
              }
              else if((gmm_state ==GMM_DEREGISTERED) && (mm_is_gmm_moved_dereg_due_to_csreg))
              {
                mm_is_gmm_moved_dereg_due_to_csreg = FALSE;
                MSG_HIGH_DS_1( MM_SUB, "=MM= Ignoring Attach Reject : %d ", mm_is_gmm_moved_dereg_due_to_csreg );
              }
              else
              {
                gmm_send_gmm_status_message(
                  MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE );
              }
              }
              else
              {
                MSG_ERROR_DS_0( MM_SUB, "=MM= Integrity protection not activated");
              }
              break;

            case GMM_DETACH_ACCEPT:
              MSG_HIGH_DS_0( MM_SUB, "=MM= Received DETACH ACCEPT" );
     /******************************************************************************************
          * spece 24.008 4.7.1.9 the MS shall start the timer T3340 in the following cases:
          * c) the MS receives a DETACH ACCEPT message and the MS has set the detach type to "IMSI detach" in the
          *     DETACH REQUEST message and user plane radio access bearers have not been set up.
     ********************************************************************************************/
#ifdef FEATURE_NAS_REL11 
              if(gmm_substate == GMM_IMSI_DETACH_INITIATED)
              {
#ifdef FEATURE_DUAL_SIM
                if(!( rabm_is_rab_active((sys_modem_as_id_e_type)mm_sub_id) ))
#else
                if(!( rabm_is_rab_active() ))
#endif
                {
                  mm_start_timer(TIMER_T3340,DEFAULT_TIMEOUT);
                }
              }
#endif
              if (gmm_power_down_active)
              {
#ifdef FEATURE_GSM_GPRS
                if ((is_umts_mode() && (gmm_integrity_protection_activated
#ifdef FEATURE_TDSCDMA
                                        || (is_tdscdma_mode() && !gmm_anite_gcf_flag)
#endif /* FEATURE_TDSCDMA */
                                        )) || is_gsm_mode())
#else
                if (gmm_integrity_protection_activated
#ifdef FEATURE_TDSCDMA
                    || (is_tdscdma_mode() && !gmm_anite_gcf_flag)
#endif /* FEATURE_TDSCDMA */
                    )
#endif
                {
                  gmm_handle_gprs_detach( message );
                }
                else
                {
                  MSG_ERROR_DS_0( MM_SUB, "=MM= Integrity protection not activated" );
                }
              }
              else
              {
                /* ----------------------------------------------------------------
                ** The DETACH ACCEPT message is processsed regardless of whether or
                ** not the security mode control procedure has been activated
                ** (for non power-off)
                ** ---------------------------------------------------------------- */
                gmm_handle_gprs_detach( message );
              }
              break;

            case GMM_DETACH_REQUEST:
              MSG_HIGH_DS_0( MM_SUB, "=MM= Received DETACH REQUEST");
              mm_stop_timer(TIMER_PS_CONN_REL);
#ifdef FEATURE_NAS_REL11
        /*-----------------------------------------------------------------------------------
               *spec 24.008 4.7.1.9 Release of the PS signalling connection (Iu mode only)
               *In case b and c,
               *   -upon receipt of a DETACH REQUEST message, the MS shall stop timer T3340 and respond to the 
               *     network initiated GPRS detach as specified in subclause 4.7.4.2.
               **---------------------------------------------------------------------------------*/
              
              mm_stop_timer(TIMER_T3340);
                
      /*-------------------------------------------------------------------------------------
           *spec 24.008 4.7.4.1.4  Abnormal cases in the MS
           * c)  Detach procedure collision:
           *       GPRS detach containing cause "power off":
           *           -If the MS receives a DETACH REQUEST message before the MS initiated GPRS detach procedure 
           *             has been completed, this message shall be ignored and the MS initiated GPRS detach procedure
           *             shall continue.
      **-----------------------------------------------------------------------------------------*/
              if (gmm_state == GMM_DEREGISTERED_INITIATED && gmm_power_down_active)//FEATURE REL 11
              {
                MSG_HIGH_DS_0( MM_SUB, "=MM= MS has already initiated detach with cause poweroff so ignoring NW initiated detach "); 
              }
              else
#endif

#ifdef FEATURE_GSM_GPRS
              if ((is_umts_mode() && (gmm_integrity_protection_activated
#ifdef FEATURE_TDSCDMA
                                      || (is_tdscdma_mode() && !gmm_anite_gcf_flag)
#endif /* FEATURE_TDSCDMA */
                                      )) || is_gsm_mode())
#else
              if (gmm_integrity_protection_activated
#ifdef FEATURE_TDSCDMA
                   || (is_tdscdma_mode() && !gmm_anite_gcf_flag)
#endif /* FEATURE_TDSCDMA */
                  )
#endif
              {
                gmm_process_detach_request_msg( message );
              }
              else
              {
                MSG_ERROR_DS_0( MM_SUB, "=MM= Integrity protection not activated");
              }
              break;

            case GMM_ROUTING_AREA_UPDATE_ACCEPT:
              MSG_HIGH_DS_0( MM_SUB, "=MM= Received ROUTING AREA UPDATE ACCEPT" );
              
              if ((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
                  ((gmm_state == GMM_REGISTERED) && is_gsm_mode()))
              {
                if ((gmm_ra_update_type == GMM_PERIODIC_UPDATING) &&
                    (!gmm_is_periodic_rau_accept_msg_valid(message)))
                {
                   MSG_HIGH_DS_0( MM_SUB, "=MM= Invalid ROUTING AREA UPDATE ACCEPT");
                }
                else
                {
#ifdef FEATURE_GSM_GPRS
                  if ((is_umts_mode() &&
                       gmm_check_rau_for_integrity_protection(message)) || is_gsm_mode())
#else
                  if (gmm_check_rau_for_integrity_protection(message))
#endif
                  {
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
                    gmm_handle_routing_area_update( message );
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
                  }
                  else
                  {
                    MSG_ERROR_DS_0( MM_SUB, "=MM= Integrity protection not activated");
                  }
                }
              }
              else
              {
                gmm_send_gmm_status_message(
                  MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE );
              }
              break;

            case GMM_ROUTING_AREA_UPDATE_REJECT:
              MSG_HIGH_DS_1
              (
                MM_SUB, "=MM= Received ROUTING AREA UPDATE REJECT Cause 0x%x",
                message->cmd.mm_data_ind.L3_data[2]
              );
              if(gmm_check_rau_reject_for_integrity_protection(message))
              {
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
              if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
              {
                gmm_handle_routing_area_update( message );
              }
              else if((gmm_state ==GMM_DEREGISTERED) && (mm_is_gmm_moved_dereg_due_to_csreg))
              {
                mm_is_gmm_moved_dereg_due_to_csreg = FALSE;
                MSG_HIGH_DS_1( MM_SUB, "=MM= Ignoring RAU Reject : %d ",mm_is_gmm_moved_dereg_due_to_csreg );        
              }
              else
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
              {
                gmm_send_gmm_status_message(
                  MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE );
              }
              }
              else
              {
                MSG_ERROR_DS_0( MM_SUB, "=MM= Integrity protection not activated" ); 
              }
              break;

            default:
              MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message type %d", gmm_message_type, 0,0 );
          }
          break;

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Unexpected RRC message %d", message->cmd.hdr.message_id, 0,0 );
      }
      break;

    case MS_TIMER:
      switch (message->cmd.timer_expiry.timer_id)
      {
        case TIMER_T3310:
          if (gmm_state == GMM_REGISTERED_INITIATED)
          {
            mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);

            /* Check blocking restrictions */
            if ((gmm_check_if_ps_cell_access_barred(ps_cell_access) == TRUE) ||
                (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE))
            {
              MSG_HIGH_DS_2( MM_SUB, "=MM= GPRS Attach blocked - AC %d or service_state %d",
                ps_cell_access, mm_serving_plmn.gmm_service_state );
              gmm_set_state( GMM_DEREGISTERED );
              if (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
              {
                gmm_set_substate( GMM_NO_CELL_AVAILABLE );
              }
              else
              {
                gmm_set_substate( GMM_ATTACH_NEEDED );
              }
            }
            else
            {
              gmm_handle_gprs_attach( message );
            }
          }
          break;

#ifdef FEATURE_NAS_FREE_FLOAT
        case TIMER_NW_INIT_DETACH :
#ifdef FEATURE_LTE
            if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
            {
             mm_send_mmr_ps_detach_cnf();
             mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
            }
#endif
           glif_execute_pended_gmm_triggers();
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
           gmm_transaction_id++;
#endif
          break;
#endif

        case TIMER_T3321:
        case TIMER_POWERDOWN:
          if ((gmm_state == GMM_DEREGISTERED_INITIATED) ||
              ((gmm_state == GMM_REGISTERED) &&
               (gmm_substate == GMM_IMSI_DETACH_INITIATED)))
          {
            gmm_handle_gprs_detach( message );
          }
          break;
        case TIMER_DETACH_REL:
          if ((gmm_state == GMM_DEREGISTERED_INITIATED) ||
              ((gmm_state == GMM_REGISTERED) &&
              (gmm_substate == GMM_IMSI_DETACH_INITIATED)) ||
              (gmm_state == GMM_DEREGISTERED)
             )
          {
            gmm_handle_gprs_detach( message );
          }
          break;
#ifdef FEATURE_DUAL_SIM
        case TIMER_GMM_RADIO_RETRY:
          if (gmm_state == GMM_DEREGISTERED_INITIATED)
          {
            gmm_handle_gprs_detach( message );
            break;
          }
          MSG_HIGH_DS_0(MM_SUB, "=MM= Retrying after TRM failure");
#endif
        case TIMER_T3302:
#ifdef FEATURE_NAS_REL10
        case TIMER_T3346:
#endif
        case TIMER_T3311:
          if (message->cmd.timer_expiry.timer_id == TIMER_T3302)
          {
            gprs_attach_attempt_counter = 0;
            gmm_rau_attempt_counter     = 0;
            MSG_HIGH_DS_2( MM_SUB, "=MM= GPRS Attach Attempt counter = %d and RAU Attempt counter = %d", gprs_attach_attempt_counter, gmm_rau_attempt_counter );

            if((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
                ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                                         SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
                (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                                              SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)) &&
               (gmm_substate == GMM_LIMITED_SERVICE))
            {
              if(
#ifdef FEATURE_DUAL_SIM
                 (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
                 (reg_sim_per_subs_plmn_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
#else
                 (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn )) ||
                 (reg_sim_plmn_forbidden(gmm_current_lai.PLMN_id)) ||
#endif
                 (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
                 (mm_check_forbidden_regional_roaming( &gmm_current_lai )))
              {
                mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CAMPED_ONLY;
              }
              else
              {
                mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_PS_SERVICE_ONLY;
              }

              mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
              mmr_service_ind.service_state = mm_serving_plmn.info;
              mm_send_mmr_service_ind( &mmr_service_ind );
              break;
          }
           }
#ifdef FEATURE_NAS_REL10
           if (message->cmd.timer_expiry.timer_id == TIMER_T3346)
           {
              sys_plmn_undefine_plmn_id(&gmm_3gpp_t3346_plmn_id);
#ifdef FEATURE_LTE
              mm_is_lu_to_be_initiated_t3346_ltogw = FALSE;
#endif
              if(gmm_state == GMM_REGISTERED && gmm_is_congestion_due_to_rau)
              {
                gmm_update_ra_type();
              }
              /* If T3312 expired when T3346 was running and the cause of
               * T3346 start was Service Reject (Congestion), not RAU reject (Congestion) */
              else if(mm_timer_status[TIMER_T3312] == TIMER_STOPPED)
              {
                gmm_ra_update_type = GMM_PERIODIC_UPDATING;
              }       
           }
#endif

#ifdef FEATURE_ENHANCED_NW_SELECTION
          if (gmm_is_rai_in_foreground_search_lai_rai_timer_list((byte *)&gmm_current_lai,gmm_current_rac))
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= RAI is in foreground search LAI list" );

            /*  If Timer T3311 expires when GMM is in no service and REG
             *  is awaiting MMR_REG_CNF, MM should not send it.
             */
            MSG_HIGH_DS_1( MM_SUB, "=MM= Handling T3311 expiry in GMM, gmm_service_state = %d",mm_serving_plmn.gmm_service_state );
            if(mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE)
            {
               if (mm_reg_waiting_for_reg_cnf)
               {
                 MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_REG_CNF ");
                 mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
                 mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                 mm_send_mmr_reg_cnf();
               
                 mm_reg_waiting_for_reg_cnf = FALSE;
                 mm_ready_to_send_reg_cnf   = FALSE;
          }
          else
               {
                 MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND ");
                 mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
                 mmr_service_ind.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
                 mmr_service_ind.service_state = mm_serving_plmn.info;
                 mm_send_mmr_service_ind( &mmr_service_ind );
               }
            }
          }
          else
#endif
          if( gmm_foreground_search_pending )
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Foreground search has to be initiated" );
          }
          else if ((gmm_state == GMM_DEREGISTERED) &&
              (mm_waiting_for_stop_mode_cnf))
          {
            MSG_HIGH_DS_0( MM_SUB,"=MM= Attach will be done after camping on new RAT" );
            gmm_set_substate(GMM_ATTACH_NEEDED);
          }
          else if ((gmm_state == GMM_DEREGISTERED) &&
                   (gmm_substate == GMM_ATTEMPTING_TO_ATTACH))
          {
            /* ----------------------------------------------------------
            ** Perform GPRS attach on the expiry of timers T3311 or T3302
            ** (TS 24.008 4.2.4.2.2)
            ** ---------------------------------------------------------- */
            gmm_initiate_gprs_attach();
          }
          else if ((gmm_state == GMM_DEREGISTERED) &&
                    ((gmm_substate == GMM_SUSPENDED) || (is_gsm_mode() && gmm_is_suspended)))
          {
            gmm_gprs_attach_pending = TRUE;
          }
#ifdef FEATURE_NAS_REL10
          else if( (gmm_state == GMM_REGISTERED) &&
                  (message->cmd.timer_expiry.timer_id == TIMER_T3346) &&
                  (!gmm_is_congestion_due_to_rau && !gmm_periodic_rau_pending ) &&
#ifdef FEATURE_LTE
                  (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI) &&
#endif 
                  /*  ---------------------------------------------------------- 
                  ** Start RAU at the expiry of T3346, if T3312 timeout given by NW is non-zero and 
                  ** T3312 is not running (i.e. it expired while T3346 was running).
                  ** ---------------------------------------------------------- */
                  ((gmm_t3312_timer_value == 0) || (mm_timer_status[TIMER_T3312] != TIMER_STOPPED))
                 )
          {
            MSG_HIGH_DS_0( MM_SUB,"=MM= RAU not required as already UPDATED"); 

            if (((gmm_update_status == GMM_GU1_UPDATED) &&
                 (!(gmm_rai_changed( gmm_stored_rai.location_area_code,
                                     gmm_stored_rai.routing_area_code,
                                     mm_serving_plmn.lac, mm_serving_plmn.rac ) ||
                    gmm_plmn_changed(gmm_stored_rai.plmn_id,mm_serving_plmn.info.plmn)))) ||
                   (gmm_substate == GMM_LIMITED_SERVICE)) 

            {
              mm_serving_plmn.info.ps_data_suspend = FALSE;

              /* -------------------------------------------------------
              ** Send indication to REG that PS data no longer suspended
              ** ------------------------------------------------------- */
              if (mm_reg_waiting_for_reg_cnf)
              {
                if (mm_ready_to_send_reg_cnf)
                {
                  mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                  mm_send_mmr_reg_cnf();

                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf   = FALSE;
                }
              }
              else
              {
                mmr_service_ind.service_state = mm_serving_plmn.info;
                mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                mm_send_mmr_service_ind( &mmr_service_ind );
              }
           }
         }
#endif
          /* Spec_ref: 4.1.1.2.1
               A GPRS MS operating in mode A or B in a network that operates in mode I shall perform a normal location updating procedure (in order to remove the Gs association in the MSC/VLR) when the following conditions are fulfilled:
               -    the GPRS MS has camped on a cell where the PS domain is barred and the CS domain is unbarred;
               -    and -   T3312, T3311, T3302, or T3330 expires.*/
          else if ((gmm_state == GMM_REGISTERED) &&
               (gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION))))
          {    
            gmm_set_substate(GMM_UPDATE_NEEDED);
#ifdef FEATURE_DSAC
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                ((mm_gs_association_established == TRUE) ||
                (gmm_ra_update_type == GMM_PERIODIC_UPDATING)))
            {
              if (mm_state == MM_IDLE)
              {
                mm_initiate_lu( MM_LU_CAUSE_NORMAL );
              }
              else
              {
                mm_pending_lu_request = NORMAL_LU;
              }
            }
#endif
          }
          else if((gmm_state == GMM_REGISTERED) &&
                  (mm_waiting_for_stop_mode_cnf))
          {
            MSG_HIGH_DS_0( MM_SUB,"=MM= RAU will be done after camping on new RAT" );
            gmm_set_substate(GMM_UPDATE_NEEDED);
            gmm_ra_update_type = gmm_pending_ra_update_type;
            if (gmm_ra_update_type == GMM_PERIODIC_UPDATING)
            {
              gmm_periodic_rau_pending = TRUE;
            }
          }       
          else if ((gmm_state == GMM_REGISTERED) &&
                   ((gmm_substate == GMM_NORMAL_SERVICE) ||
                    (gmm_substate == GMM_ATTEMPTING_TO_UPDATE) ||
                    (gmm_substate == GMM_ATTEMPTING_TO_UPDATE_MM)))
          {
            /* --------------------
            ** See 24.008 4.7.5.1.5
            ** -------------------- */
            if (gmm_ra_update_pending)
            {
              if(!((mm_state != MM_IDLE) && 
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) && 
                   (gmm_ra_update_type == GMM_PERIODIC_UPDATING) &&
                   ((gmm_pending_ra_update_type== GMM_COMBINED_RA_LA_UPDATING) ||
                    (gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH))))
              {
                gmm_ra_update_type = gmm_pending_ra_update_type;
              }
            }
            gmm_initiate_routing_area_update();
          }
          else if ((gmm_state == GMM_REGISTERED) &&
                   ((gmm_substate == GMM_SUSPENDED) || (is_gsm_mode() && gmm_is_suspended)))
          {
            gmm_ra_update_pending = TRUE;
            gmm_pending_ra_update_type = gmm_ra_update_type;
          }
          else if ((gmm_state == GMM_SERVICE_REQUEST_INITIATED) &&
                   ((gmm_substate == GMM_ATTEMPTING_TO_UPDATE) ||
                    (gmm_update_status != GMM_GU1_UPDATED)))
          {
            gmm_initiate_routing_area_update();
          }
          else
          {
            /*when Gs association or periodic needs to be performed and T3311/T3302 expires in OOS/Lim*/
            if ( (gmm_ra_update_type == GMM_PERIODIC_UPDATING) || 
                 (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) )
            {
               gmm_periodic_rau_pending = TRUE;
            }
            gmm_previous_T3311_gmm_substate = GMM_NULL_SUBSTATE;
            if((gmm_substate == GMM_LIMITED_SERVICE) && 
               (mm_timer_status[TIMER_T3210] == TIMER_STOPPED) &&
               (mm_timer_status[TIMER_T3211] == TIMER_STOPPED)) 
            {
              mmr_service_ind.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
              mmr_service_ind.service_state = mm_serving_plmn.info;
              mm_send_mmr_service_ind( &mmr_service_ind );
            }
          }
#ifdef FEATURE_NAS_REL10
          /* On T3346 expiry reset gmm_is_congestion_due_to_rau */
          if(message->cmd.timer_expiry.timer_id == TIMER_T3346)
          {
            gmm_is_congestion_due_to_rau = FALSE; 
            mm_send_mmr_t3346_timer_ind(SYS_NAS_T3346_STATUS_EXPIRED);	
          }
#endif    
          break;

        case TIMER_T3312:
          /* ----------------------------------------------------------------------
          ** When timer T3312 expires, the periodic routing area updating procedure
          ** shall be started and the timer shall be set to its initial value for
          ** the next start
          ** ---------------------------------------------------------------------- */
          if (gmm_state == GMM_REGISTERED)
          {
#if defined(FEATURE_LTE)
            /* If ISR is ON, start ISR deactivate timer T3323. If UE is able to be perform RAU
                then this timer will be stopped immediately after UE moves to connected state */
            if (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
            {
              if (gmm_t3323_timer_value != 0)
              {
                mm_start_timer( TIMER_T3323, gmm_t3323_timer_value );
              }
            }
#endif
#ifdef FEATURE_ENHANCED_NW_SELECTION
            if ( gmm_is_rai_in_foreground_search_lai_rai_timer_list((byte *)&gmm_current_lai,gmm_current_rac))
            {
              MSG_HIGH_DS_0( MM_SUB, "=MM= RAI is in foreground search LAI list");
            }
            else
#endif
/*24.008: 4.7.5.1.5 (j)  Timer T3346 is running
    The MS shall not start the routing area updating procedure unless the MS is accessing the network with access class 11 – 15*/
#ifdef FEATURE_NAS_REL10
            if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
#ifdef FEATURE_NAS_REL11
                  &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
              )
            {
              MSG_HIGH_DS_0( MM_SUB, "=MM= Ignore GMM Procedure since T3346 congestion timer is running");
            }
            else
#endif
            if( gmm_foreground_search_pending )
            {
              MSG_HIGH_DS_0( MM_SUB, "=MM= Foreground search has to be initiated" );
            }
            else if(mm_deep_sleep_alt_scan)
            {
              MSG_HIGH_DS_0(MM_SUB, "=MM= T3312 expired in ALT_SCAN mode. Mark Periodic RAU pending");
              gmm_periodic_rau_pending = TRUE;
              gmm_ra_update_type       = GMM_PERIODIC_UPDATING;
            }
            else if(mm_waiting_for_stop_mode_cnf)
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= Periodic RAU will be initiated after camping on new RAT");
              gmm_periodic_rau_pending = TRUE;
              gmm_ra_update_type       = GMM_PERIODIC_UPDATING;
              gmm_set_substate(GMM_UPDATE_NEEDED);
            }
            else if ((gmm_substate != GMM_NORMAL_SERVICE) || 
                     (gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)))
#ifdef FEATURE_WRLF_SYSTEM_SEL
                      ||(mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
#endif
             )
            {
              /* -------------------------------------------------------------------
              ** If the MS is in other state than GMM-REGISTERED.NORMAL-SERVICE when
              ** the timer expires the periodic routing area updating procedure is
              ** delayed until the MS returns to GMM-REGISTERED.NORMAL-SERVICE
              ** ------------------------------------------------------------------- */
              gmm_periodic_rau_pending = TRUE;
              if (!((gmm_plmn_changed( gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn )) ||
                     (gmm_rai_changed ( gmm_stored_rai.location_area_code,
                                        gmm_stored_rai.routing_area_code, 
                                        mm_serving_plmn.lac, mm_serving_plmn.rac ))) && 
                                     (gmm_update_status == GMM_GU1_UPDATED))
              {
                gmm_ra_update_type       = GMM_PERIODIC_UPDATING;
              }

              /* ---------------------------------------------------------------------
              ** If the MS is both IMSI attached for GPRS and non-GPRS services in a
              ** network that operates in network operation mode 1, and if the MS has
              ** camped on a cell that does not support GPRS, and timer T3312 expires,
              ** then the MS shall start an MM location updating procedure.
              ** --------------------------------------------------------------------- */
              if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) &&
                  (((mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE) &&
                    (gmm_previous_network_op_mode == NETWORK_OP_MODE_1)) || 
                   (gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)))))
              {
                if ((mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE) &&
                    (mm_gs_association_established == TRUE))
                {
#ifdef FEATURE_DSAC
                  if (gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)))
                  {
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
#endif                
                    if ((mm_state == MM_IDLE) &&
                        ((mm_idle_substate == MM_NORMAL_SERVICE) ||
                         (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE)))
                    {
                      mm_initiate_lu( MM_LU_CAUSE_PERIODIC );
                    }
                    else
                    {
                      mm_pending_lu_request = T3312_EXPIRED;
                    }
#ifdef FEATURE_DSAC                    
                  }
#endif                                      
                }

                /* ----------------------------------------------------------------
                ** In addition, the MS shall perform a combined routing area update
                ** procedure indicating "combined RA/LA updating with IMSI attach"
                ** when the MS enters a cell that supports GPRS and indicates that
                ** the network is in operation mode 1.
                ** ---------------------------------------------------------------- */
                if(gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)))
                {                                                        
                  gmm_set_substate(GMM_UPDATE_NEEDED);
                  if(mm_check_if_cs_cell_access_barred(mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION)))
                  {                   
                    gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
                  }
                }  
                if (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)
                {                                                        
                  gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
                }  
              }
              
              else if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&                       
                       (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) &&
                       ((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                          gmm_stored_rai.routing_area_code,
                                          mm_serving_plmn.lac, mm_serving_plmn.rac)) ||
                        (gmm_plmn_changed ( gmm_stored_rai.plmn_id,
                                            mm_serving_plmn.info.plmn)) || 
                        (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                                            SYS_NETWORK_SELECTION_MODE_LIMITED)))                    
              {
                gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;            
              }
              else if(gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)))
              {
                gmm_set_substate(GMM_UPDATE_NEEDED);
              }
            }  
            else
            {
              gmm_ra_update_type = GMM_PERIODIC_UPDATING;
              gmm_initiate_routing_area_update();

              if (gmm_t3312_timer_value != 0)
              {
                mm_start_timer( TIMER_T3312, gmm_t3312_timer_value );
              }
            }
          }
          else if (gmm_state == GMM_SERVICE_REQUEST_INITIATED)
          {
            MSG_HIGH_DS_0(MM_SUB,"=MM= T3312 expired in Service Request initiated state. Mark Periodic RAU pending");
            gmm_periodic_rau_pending = TRUE;
            gmm_ra_update_type       = GMM_PERIODIC_UPDATING;
          }
          break;
#ifdef FEATURE_ENHANCED_NW_SELECTION
          case TIMER_FOREGROUND_SEARCH:
            gmm_get_foreground_timer_expiry_rai(foreground_timer_expiry_lai,&rac);
            if ((memcmp(mm_serving_lai,foreground_timer_expiry_lai,LAI_SIZE) ==0) &&
                 (rac != 0xFF)&&
                 (rac == gmm_current_rac) &&
                 (mm_timer_status[TIMER_T3312] == TIMER_STOPPED) &&
                 (mm_timer_status[TIMER_T3302] == TIMER_STOPPED))
            {
              if (gmm_state == GMM_DEREGISTERED)
              {
                gmm_initiate_gprs_attach();
              }
              else
              {
                gmm_update_ra_type();
                gmm_initiate_routing_area_update();
              }
            }
          break;
#endif
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
        case TIMER_T3330:
          if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
          {
            gmm_handle_routing_area_update( message );
          }
          break;
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

        case TIMER_T3350:
          MSG_HIGH_DS_1(MM_SUB, "=MM= Ignoring timer expiry %d",message->cmd.timer_expiry.timer_id);
          break;

#if defined(FEATURE_LTE)
        case TIMER_T3323:
          /*
          ** If timer T3323 expires deactivate the ISR by setting the TIN to GUTI
          ** Also set the pending LU flag so as to perform LU if we find service in GW
          ** Reset the periodic RAU pending flag so that UE will perform normal RAU
          ** as we are going to use mapped identitites(REL10 CR CP-110683 )
          */
          emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
          mm_pending_lu_request = LTE_IRAT_LU;
          gmm_periodic_rau_pending = FALSE;
          break;
#endif

#ifdef FEATURE_LTE
        case TIMER_MM_PS_DETACH:
          if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF ||
             mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
          {
            if(mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
            {
              mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF;
            }
            gmm_set_state( GMM_DEREGISTERED_INITIATED );
            gmm_detach_type = GMM_GPRS_DETACH;
            gmm_handle_gprs_detach_low_level_failure();
            mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_TIME_OUT,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
          }
          break;
#endif

        default:
          MSG_FATAL_DS( MM_SUB, "=MM= Timer message unexpected %d",
                      message->cmd.timer_expiry.timer_id, 0,0 );
      }
      break;

    default:
      MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message set %d", message->cmd.hdr.message_set, 0,0 );
  }
} /* end gmm_handle_registration() */
/*===========================================================================

FUNCTION GMM_HANDLE_MULTI_SLOT_CLASS_IND

DESCRIPTION
  function to load multi slot values into corresponding global variables 
  to be used in the next RAU, for tempeature mitigation.
 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_handle_multi_slot_class_ind( mm_cmd_type *premitive)
{
  gmm_multislot_class = premitive->cmd.rr_gmm_gprs_multislot_class_ind.multi_slot_class;
#ifdef FEATURE_GSM_EGPRS
  gmm_edge_multislot_class = premitive->cmd.rr_gmm_gprs_multislot_class_ind.edge_multi_slot_class;
#endif  
#ifdef FEATURE_GSM_DTM
  gmm_nv_dtm_multislot_subclass = premitive->cmd.rr_gmm_gprs_multislot_class_ind.dtm_multi_slot_subclass;
#endif      

  gmmllc_reset_service_state ();


  MSG_HIGH_DS_3(MM_SUB,"=MM= gmm multislot class ind received, multislot class = %d edge multislot class = %d dtm multislot subclass =%d ",
                        premitive->cmd.rr_gmm_gprs_multislot_class_ind.multi_slot_class,
                        premitive->cmd.rr_gmm_gprs_multislot_class_ind.edge_multi_slot_class,
                        premitive->cmd.rr_gmm_gprs_multislot_class_ind.dtm_multi_slot_subclass);

  if((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED)&&
     (gmm_state != GMM_DEREGISTERED_INITIATED))
  {
    if(!is_gsm_mode())
    {
      gmm_multi_slot_ra_update_pending = TRUE;
      MSG_HIGH_DS_0(MM_SUB,"=MM= gmm multislot RAU pended, will be processed after comming to GSM");
    }
    else if((is_gsm_mode())&&((gmm_state == GMM_REGISTERED_INITIATED) || (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)))
    {
      gmm_multi_slot_ra_update_pending = TRUE;
      MSG_HIGH_DS_0(MM_SUB,"=MM= gmm multislot RAU pended");
    }
    else
    {
      gmm_update_ra_type();
      gmm_multi_slot_ra_update_pending = TRUE;
      gmm_initiate_routing_area_update();
    }
  }
}

