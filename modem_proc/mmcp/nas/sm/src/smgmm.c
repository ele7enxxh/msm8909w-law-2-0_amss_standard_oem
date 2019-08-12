/*===========================================================================
                       S M  G M M  S O U R C E   F I L E 

DESCRIPTION

   This file contains the function definitions between the SM and GMM entities.
   
EXTERNALIZED FUNCTIONS   

  sm_process_gmm_cmd() : Processes the command received from GMM entity.

  send_gmm_smpdu()  : Sends the command to GMM task.

  sm_process_sm_pdu() : Processes a received SM-PDU sent by GMM.

                  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/smgmm.c_v   1.14   18 Jul 2002 15:44:14   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/smgmm.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------- 
11/28/12   am      Fixing compilation warning 
11/22/12   am     Fixing DSDS switch issue.
10/01/12   am      Back off timer code
09/27/12  am       Handling of unexpected BCM from Network
08/17/12   am     Fixing compilation warning found in brex
07/19/12   am    Avoid unpacking NETWORK_MODIFY_PDP_REQUEST before validation check
05/25/12  us       Removing redundant F3 print messages. 
05/24/12  am       Making changes so that while processing MBMS status, PDP context 
                   in INACTIVE state is not deactivated.
02/23/12  us       Changes made to enable UE to send out PDP activate request immediately after connection release indication, without waiting for timer to expire 
02/21/12  am       Changes are made to send proper message to CM
12/21/11  am       Removing Compilation errors
12/13/11  am       Changes are made such that on receiving FOP IND, SM sends GMMSM_ESTABLISH_REQ
11/01/11   am      Aborting GMM connection procedure in case of reject cause SM_ACCESS_CLASS_DSAC_REJECTION
09/30/11   am      Fix for sub 2 loses service when sub 1 is in data transfer
09/14/11   rajesh  Made changes to set the ISR status correctly in ESM/SM.
08/04/11   am      Changes are done to make SM responds on the right subscription.
07/20/11   am      Fixing compilation issue.
07/19/11  abhi   Send PDP deactivate reject to CM if rat change to LTE happens while 
                            TIMER_EARLY_RAB_FAILURE is running.
07/13/11   am      Trigger PDP context deactivation when a PDP Modification message 
                   is received with a new PDP address
08/12/08   ab       Disabling MT PDP if feature FEATURE_DISABLE_MT_PDP is defined 
06/09/08  abhi     Sending SM status message when 2 PDP activation accept is received with different 
                           IP address.
05/14/08   ab    Set signaling connection available to false on getting rat change indication  
05/05/08   ab    If SM receives NW Modify PDP req. before getting SNSM_ACTIVATE_RESP from SNDCP then 
                        store NW Modify req., and process this after getting SNSM_ACTIVATE_RESP.
03/25/08   ab     Copying PCO value into pdp context for NW request for PDP activation
11/27/07   ab     Sending GSNSM_DEACTIVATE_IND to sndcp if deactivate confirm is due to qos failure
10/19/07   ab     Now seting PCO to whatever network has sent in PDP_ACTIVATE_ACCEPT                  
08/01/07    rk      Deactivate the whole group of PDPs, if UE goes to OOS, deactivation of 
                    Secondary PDP is initiated by higher layers and Secondary PDP is
                    picked(the best QoS) for promoting to Primary. 
12/27/06   ss      Not deactivating PDP for cause #13 and #15
12/13/06   ss      Only Service Procedure needs to aborted when Service Reject happens for cause #17
08/18/06   rk       Added Extended TI Feature Code
08/18/06  rk        Fixed Data flow resumption, after RAT is resumed in GSM mode.
05/03/06   ss       Not clearing the ESTABLISH Pending Timer when recieving Activate PDP Reject.
                  In SM only one timer run at a time for one context.
05/09/06  ss        Setting cause value to RAB SETUP FAILURE when PDP Modify for RAB setup fails.
05/02/06  kvk       Initializing the req_pdp_cxt before the SM PDU is unpacked
04/24/06  ss        Ignore SM STATUS for invalid TI
04/18/06  ss        Added cause value to GMMSM_GPRS_RELEASE_IND
04/06/06  ss        Not deactivating PDP locally when second PDP ACCEPT is recieved with same address.
03/23/06  ss        Corrected linked TI setting in case of promotion of PDP
03/21/06  ss         Now setting trans_id to invalid while dropping a duplicate request from N/W
03/18/06   kvk      Now clearing the PDP contexts in case if teh PDP context is waiting for SNSM_DEACTIVATE_RESP
                    and UE does handover to W. Teh order of call clearance will be Secondary followed by Primary
03/08/06   ss       Not deactivating locally for PDP context status IE in PDP_ACTIVE_PENDING
01/17/06   ss       Not promoting secondary PDP when teardown is set.
01/17/06   ss       Now releasing nsapi and initializing pdp while dropping a duplicate request from N/W
01/04/06  kvk       Now the PS signalling connection status will stored in SM
12/20/05  ss         Reverting Service request procedure T3317 timer expiry changes
12/01/05  kvk/hj   GCF12.9.9: Reverted the changes made to abort silent redial when
                   GMM rejects GMMSM_ESTABLISH_REQ with cause MSG_NOT_COMPATIBLE_PROTOCOL_STATE
11/28/05   ss       Now aborting gmm connection procedure on cause value NETWORK_FAILURE.
11/10/05   ss       Added check for cause value #12 for GMMSM_ESTABLISH_REJ
11/09/05   kvk     Added function for sm_check_pdp_procedure_pending() to check any 
                   PDP procedure pending for the Any of the Active PDP context
11/07/05  kvk      Now sending GMMSM_CLOSE_SESSION_REQ to GMM, if there is no PDP context 
                   waiting for GMM connection when GMM sends GMMSM_ESTABLISH_CNF
03/17/05   hj      Handled gprs reject cause MSG_NOT_COMPATIBLE_PROTOCOL_STATE, 
                   to turn off silent redial. 
09/13/05  kvk     MO PDP Modify supported. Now handling the Application initiated
                   PDP Modification for QoS and TFT.
09/12/05  kvk      Removed extern variable declration gmm_anite_gcf_flag from the function
                   sm_process_gprs_rej().
08/25/05  kvk      Now sending RAB_DEACTIVATE_IND to RABM in case PDp Modify gets rejected to
                   move RAB state to dormant.
08/22/05  kvk      Added handling for locally deactivating the PDP contexts when 
                   Service Request due to page response gets rejects with specific cause.
06/30/05  kvk      Now QoS Modify is triggered for  PDP with streaming / conversational 
                   traffic class if re-establishment is explicitly triggered by DS.
06/20/05   hj      Added support for new events EVENT_SMGMM_REQUEST_SENT, 
                   EVENT_SMGMM_REJECT_RECEIVED.
06/21/05  kvk      Now deactivating primary and secondary PDP contexts when MT PDP request
                   come with same PDPtype, PDP Address and APN.
06/08/05  kvk      Now processing NPDU list array of size 11(NSAPI 5-15) instead
                   of 16.
05/11/05  ss       Handling SM Status as per Release 5 Spec CR
04/22/04  kvk      QoS Validation is now handled before SNDCP is activated
04/18/05  kvk       PDP state is set to INACTIVE after PDP accept irrespective of 
                    RAB sub state.
04/15/05   ss       Now starting deactivation procedure when Network tries to modify
                   PDP Address.
04/04/05   kvk     Now sending pdp activate cnf in case PDP is waiting for 
                   RAB release and got deactivate cnf on W RAT and switching to 
                   different RAT.
03/28/05   kvk     SNSM_ACTIVATE_IND will be sent to SNDCP during rat resume on G
                   if SM already receives PDP accept on W and waiting for RABM response.
03/25/05   kvk     During Inter RAT handover from W2G SNSM_MODIFY_IND will not be sent to SNDCP in case PDP is in
                   PDP_ACTIVE_PENDING state.
03/17/05   hj      Handled gprs reject causes NATIONAL_ROAMING_NOT_ALLOWED, 
                   NO_SUITABLE_CELLS_IN_LA, to turn off silent redial. 
03/10/05   ss      Added support for handling incoming PDP Context Status IE while Rau Accept or Service
                   accept based on Rel-4 Spec CR N1-010325
03/01/05   kvk     Initializing the LLC substate to LLC_MT_DEACTIVATE_CNF_PENDING DURING pdp local
                   deactivation due to multiple PDP ACTIVATE ACCEPT messages from the network.
02/24/05   kvk     Fixed starting redundant RABM timer after send SNSM_DEACTINATE_IND
                   to SNDCP during PDP Deactivation in GPRS.
02/18/05   kvk     Notify RABM with primitive RABMSM_DEACTIVATE_IMPLICIT_IND
                   when SM does local decativation of all PDPs during GPRS rel
01/21/05   kvk     GMMSM_RAT_CNF will be notified right away with out waiting for
                   SNDCP sync up.
01/12/05   kvk     Added check to verify valid transaction ID and stop any pending
                   timers running when MT deactivation.
01/11/05   kvk     Now passing the extended TI received in request for packing 
                   SM status.
01/07/05   kvk     New function sm_abort_gmm_conn_proc() to abort
                   Silent redial when RRC timesout MAX access.
12/03/04   kvk     Fixed passing the NULL PDP address and APN for MT call on top of existing 
                   existing call with same transid.
11/09/04   ks      Added function sm_is_pdp_activation_pending() which returns
                   TRUE if there is a PDP context waiting for GMM connection.
10/19/04   kvk     MT PDP support added
10/18/04   kvk     Now PDP promotion after R99->R97 InterRAT handover is done
                   when GMM gives GMMSM_RAT_RESUME_REQ.
10/04/04   kvk     CR 47445: SNDCP Deactivate ind is now posted when pdp state
                   other than INACTIVE during 2G -> 3G InterRAT handover.
09/28/04   kvk     Added handling for SEC PDP in GPRS.
09/14/04   kvk     Fixed calling sm_pdp_continue_modify() when GMM gives
                   connection confirmation.
09/07/04   ab      CR39294, added processing of received NPDU list. 
09/02/04   kvk     Added handling of SEC pdp confirmation. Handling the decativation
                   of PDP with tear down indiactor flag. Handling the promotions of secondary.
                   MO PDP modification for reestablishing the streaming and conversational RAB 
                   is added.
07/28/04   kvk     Fixed dropping of RABM modify rsp by setting mt_orig_flag 
                   to true for MT pdp modification.                   
07/22/04   kvk     Fixed unnecessary RAB_MODIFY_IND to RABM during N/w inititaed
                   PDP modification in GPRS.
06/30/04   kvk     SIG CON REL IND will be sent to CM everytime
06/15/04   kvk     GMMSM_CLOSE_SESSION_REQ is sent to GMM only if no PDP context is active.
                   Added new function sm_check_pdp_exist() to verify any active PDP exits.
06/08/04   kvk     Added handling for processing SM_STATUS message. sm_proc_sm_status()
                   function handles the functionality. PDP con state is cleaned
                   reset along with PDP state.
06/03/04   kvk     Added extern variables for max_timer_cnt and timer_est_pen_flag
                   to support timers for MPDP and establish timer. 
03/30/04   ks      Added function sm_is_pdp_active to check if any PDP context is
                   currently ACTIVE.
02/28/04   ks      Added support for the new PDP connection state pdp_con_state
02/16/04   ks      Handling a Repeat PDP Activate accept message when it has
                   already received, due to race between T3380 and Accept msg.
12/15/03   ks      Setting max_timer_count to 0 after a successful connection
                   establishment from GMM.
12/04/03   ks      Added support for TIMER_ESTABLISH_PENDING for handling the
                   CMM connection before starting the PDP activation request.
12/01/03   ks      Calling sm_perform_local_deactivation() to locally deactivate
                   PDP contexts when GMM rejects with NO_PDP_CONTEXT_ACTIVATED.
09/08/03   mks     Now deactivating all the PDP contexts when GMM rejects SM
                   connection establishment requests with reject cause #10. 
08/13/03   mks     Corrected SM handling of the error conditions (per TS24.008) 
                   so as to return correct cause in the SM_STATUS message.
08/05/03   mks     Added support for validating the network offered QoS against
                   the minimum QoS requirements of the MS during MO PDP activation
                   and MT PDP modification procedures. If the QoS validation fails,
                   the context deactivation is initiated.
07/28/03   ks      Now sending the PDP modify indication to SNDCP during an
                   inter RAT change for PS continuity.
07/22/03   ks      Stopping the pending Timers while the UE receives Network
                   initiated PDP context deactivation in other than ACTIVE states.
07/14/03   ks      Processing the Deactivate Request from the Network when the
                   PDP context is in PDP_MODIFY_PENDING state.
06/24/03   ks      Removed the TIMER_RABM_RESP_PENDING from 
                   sm_proc_network_modify_pdp_request() function.
06/19/03   ks      Removed the TIMER_RABM_RESP_PENDING sanity timer whenever
                   SM waits for LLC confirmation, LLC already has a timer for this.
06/19/03   ks      Added debug statement when UE receives a PDP ACTIVATE REJ.
05/09/03   ks      Setting the Transaction ID to invalide only if the PDP
                   context is INACTIVE when MT PDP activation req is received
03/31/03   ks      If the TI_FLAG is set to 1 in MT PDP Activation message,
                   Ignoring the message.
03/21/03   ks      Rejecting the MT PDP activation with properly and also setting
                   the trans_id for the SM_STATUS message to '0'in abnormal cases.
03/18/03   mks     Now processing active RAT information in GMMSM_RAT_CHANGE_REQ
                   from GMM. Added support for delaying the RAT resumption 
                   confirmation to GMM till PDP activation is confirmed by SNDCP.
02/27/03   mks     Added support for PS data continuity feature - Added code to 
                   process RAT change and RAT resumption notifications from GMM
                   and retain the currently active PDP contexts even after the 
                   inter-RAT change.
02/24/03   ks      Fixed LINT errors, Added MSG_HIGH debug statements.
01/16/03   ks      Replaced mm_abort_rr() with send_gmm_close_session_req().
                   Stopping sending PDP ACTIVATE Request for few gmm reject causes.
12/02/02   ks      Sending PDP Deactivation indication to SNDCP due to Signaling 
                   connection release, after GPRS detach & NW detach request.
                   Added support for GPRS Modification procedures.
11/18/02   ks      Fixed the collision of PDP Deactivation request by both
                   the Network and the UE.
09/30/02   ks      Renamed GMMSM_ERROR_IND with GMMSM_CON_RELEASE_IND and
                   GMMSM_RELEASE_IND with GMMSM_GPRS_RELEASE_IND primitives.
07/18/02   ks      Renamed FEATURE_GPRS with FEATURE_GSM_GPRS
07/01/02   ks      Added GPRS support 
06/13/02   ks      Now Releasing the RAB and stopping the RABM pending timer
                   if RRC connection is released.
04/22/02   ks      Sending the Connection Release_Indication info to RABM for
                   PS preservation processing in sm_process_gmm_error_ind.
03/11/02   ks      Setting sig_connection_available field to FALSE in 
                   sm_process_gprs_rel() during a RELEASE notification from GMM
11/05/01   ks      Fixed the MSG_HIGH string length.
08/24/01   ks      Added support for SM_STATUS message. 
08/01/01   ks      Renamed sm_process_gprs_rsp() to sm_process_connection_cnf.
                   Added a function call rabm_rab_established() to check if
                   the RAB status during PDP Modification and Deactivation.             
06/29/01   ks      Added sm_process_gmm_error_ind() function.
06/04/01   ks      Added actual interface to call the SM-GMM command in the
                   send_gmm_smpdu() function. Added send_gmm_establish_req().
05/23/01   ks      Fixed/Added various debug statements.
05/17/01   ks      Added processing of SM-STATUS primitive. A new function
                   sm_send_sm_status() to send the SM-STATUS to GMM task.
04/30/01   ks      Added RABM notification during a GPRS release indication 
                   from GMM in the function sm_process_gprs_rel().
01/15/01   ks      Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "mmcp_variation.h"
#include "sm_v.h"
#include "smix_v.h"
#include "sm_gmm.h"
#include "sm_sndcp.h"
#include "ms.h"
#include "smtask.h"
#include "smtask_v.h"
#include "rabmtask_v.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "memory.h"
#include "string.h"
#include "cause.h"
#include "smtimers.h"
#include "mm_v.h"
#include "gmm_int_v.h"
#include "ds3gpp_api.h"
#include "fs_public.h"
#include "mcfg_fs.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#else
#include "gs_v.h"
#endif

#ifdef FEATURE_GSM_GPRS
#include "gsndcp.h"
#include "gsmsnif.h"
#endif

#ifdef FEATURE_MBMS
#error code not present
#endif
#if defined(FEATURE_LTE)
#include "esm_v.h"
#include "sm_multimode_common.h"
#endif
#include <stringl/stringl.h>
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_GSM_GPRS)
#include "geran_multi_sim_api.h" 
#endif /* FEATURE_DUAL_SIM && FEATURE_GSM_GPRS */



/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

#define NO_SPECIFIC_NSAPI 0
#define GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS 11

#ifdef FEATURE_DUAL_DATA
extern uint32 sm_tlli_sim[MAX_AS_IDS];
#define sm_tlli sm_tlli_sim[sm_as_id]

extern boolean sm_ps_vote_for_reselection_sim[MAX_AS_IDS];
#define sm_ps_vote_for_reselection sm_ps_vote_for_reselection_sim[sm_as_id]

#else
extern uint32 sm_tlli;
#endif

#if defined(FEATURE_DUAL_SIM)
extern boolean SM_IS_VALID_AS_ID(sys_modem_as_id_e_type cmd_as_id);
#endif

/* ========================================================================
** LOCAL FUNCTION PROTOTYPES.
** These functions are available only in the scope of this file.
** ====================================================================== */

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_CONNECTION_CNF()

** DESCRIPTION  : This function processes GPRS attach response from GMM.
** ----------------------------------------------------------------------- */
LOCAL void sm_process_connection_cnf(
        gmmsm_establish_cnf_T* gmmsm_establish_cnf,
        boolean retry
);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_CONNECTION_IND()

** DESCRIPTION  : 
   This function processes the GMM connection ind to indicate GMM is in 
   PMM_connected.state. SM has to trigger the PDP context modification procedure
   for PDPs with trafiic class streaming and conversational to bring upgrade the
   bitrate to original values.
** ----------------------------------------------------------------------- */
LOCAL void sm_process_connection_ind(gmmsm_establish_ind_T* gprs_ind);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_GMM_CON_RELEASE_IND()

** DESCRIPTION  : This function processes Error Indications from GMM.
** ----------------------------------------------------------------------- */
LOCAL void sm_process_gmm_con_release_ind(gmmsm_con_release_ind_T *release_ind);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_FAST_DORMANCY_CNF ()

** DESCRIPTION  : This function processes dormancy Indications from GMM.
** ----------------------------------------------------------------------- */
LOCAL void sm_process_fast_dormancy_cnf (gmmsm_fast_dormancy_cnf_T*dormancy_cnf);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_GPRS_REJ()

** DESCRIPTION  : This function processes GPRS attach reject from GMM.
** ----------------------------------------------------------------------- */
LOCAL void sm_process_gprs_rej(gmmsm_establish_rej_T* gprs_rej);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_GPRS_REL()

** DESCRIPTION  : This function processes GPRS release indication from GMM.
** ----------------------------------------------------------------------- */
LOCAL void sm_process_gprs_rel(gmmsm_gprs_release_ind_T* gprs_rel);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_SEND_SM_STATUS()

** DESCRIPTION  : This function sends the SM-STATUS message to GMM.
** ----------------------------------------------------------------------- */
LOCAL void sm_send_sm_status( pdp_context_info *pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_SM_PDU()

** DESCRIPTION  : This function processes a received SM-PDU from GMM.
** ----------------------------------------------------------------------- */
void sm_process_sm_pdu(gmmsm_unitdata_ind_T* sm_pdu);
/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_PDP_STATUS()

** DESCRIPTION  : This function processes a received PDP-CONTEXT-STatus IE  from GMM.
** ----------------------------------------------------------------------- */
LOCAL void sm_process_pdp_status(gmmsm_pdp_mbms_context_status_T* gmm_pdp_mbms_context_status);

/** FUNCTION     : SM_PROCESS_PDP_MBMS_STATUS()

** DESCRIPTION  : This function processes a received PDP-CONTEXT-STatus IE  from GMM.
** ----------------------------------------------------------------------- */
LOCAL void sm_process_pdp_mbms_status(const gmmsm_pdp_mbms_context_status_T* gmm_pdp_mbms_context_status);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_ACTIVATE_PDP_ACCEPT()

** DESCRIPTION  : This function processes ACTIVATE_PDP_ACCEPT SM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_proc_activate_pdp_accept(pdp_context_info *cur_pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_ACTIVATE_PDP_REJECT()

** DESCRIPTION  : This function processes ACTIVATE_PDP_REJECT SM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_proc_activate_pdp_reject(pdp_context_info *cur_pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_MODIFY_PDP_REJECT()

** DESCRIPTION  : This function processes MODIFY_PDP_REJECT SM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_proc_modify_pdp_reject(pdp_context_info *cur_pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_MS_MODIFY_PDP_ACCEPT()

** DESCRIPTION  : This function processes MODIFY_PDP_ACCEPT SM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_proc_ms_modify_pdp_accept(pdp_context_info *cur_pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : sm_proc_deactivate_pdp_accept()

** DESCRIPTION  : processes DEACTIVATE_PDP_ACCEPT SM-SM message..
** ----------------------------------------------------------------------- */
LOCAL void sm_proc_deactivate_pdp_accept(pdp_context_info *cur_pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_ACTIVATE_SEC_PDP_ACCEPT()

** DESCRIPTION  : processes ACTIVATE_SEC_PDP_ACCEPT SM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_proc_activate_sec_pdp_accept(pdp_context_info *cur_pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_ACTIVATE_SEC_PDP_REJECT()

** DESCRIPTION  : processes ACTIVATE_SEC_PDP_REJECT SM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_proc_activate_sec_pdp_reject(pdp_context_info *cur_pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_NETWORK_MODIFY_PDP_REQUEST()

** DESCRIPTION  : processes NETWORK_MODIFY_PDP_REQUEST SM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_proc_network_modify_pdp_request(pdp_context_info *cur_pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_REQUEST_PDP_ACTIVATION()

** DESCRIPTION  : processes MT, REQUEST_PDP_ACTIVATION SM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_proc_request_pdp_activation (pdp_context_info *cur_pdp); 

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_REQUEST_SEC_PDP_ACTIVATION()

** DESCRIPTION  : processes MT, REQUEST_PDP_ACTIVATION SM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_proc_request_sec_pdp_activation (pdp_context_info *cur_pdp); 

#ifdef FEATURE_MBMS
#error code not present
#endif

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_REQUEST_PDP_DEACTIVATION()

  ** DESCRIPTION  : processes MT, REQUEST_PDP_DEACTIVATION SM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_proc_request_pdp_deactivation(pdp_context_info *cur_pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_SM_STATUS()

  ** DESCRIPTION  : processes MT, SM_STATUS SM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_proc_sm_status(pdp_context_info *cur_pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_RAT_CHANGE()

  ** DESCRIPTION  : processes RAT-CHANGE GMM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_process_rat_change(sm_cmd_type *);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROC_RAT_RESUME()

  ** DESCRIPTION  : processes RAT-RESUME GMM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_process_rat_resume(
#ifdef FEATURE_DUAL_SIM
        sm_cmd_type* cmd_ptr
#else
        void
#endif
);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_SEND_SM_STATUS_ON_RX_EXTENDED_TID

** DESCRIPTION  : This function sends SM_STATUS with transaction ID set to 
                  '111'. This function should be called only when MS receives 
                  an SM PDU from the network with extended transaction ID.
** ----------------------------------------------------------------------- */
LOCAL void sm_send_sm_status_on_rx_extended_tid ( word );

/* -----------------------------------------------------------------------
**FUNCTION sm_proc_deact_req

**DESCRIPTION This function process PDP deactivation when requested by network
** ----------------------------------------------------------------------- */
void sm_proc_deact_req(pdp_context_info *cur_pdp);

/* -----------------------------------------------------------------------
** FUNCTION SM_VALIDATE_DUPLIACTE_COLLISION_PDP

** DESCRIPTION
   This function validates the REQUEST PDP ACTIVATION and verifies the message
   has to be handled or not depending on the state of the duplicate PDP.
** ----------------------------------------------------------------------- */
LOCAL boolean sm_validate_dupliacte_collision_pdp(pdp_context_info *pdp);

/* -----------------------------------------------------------------------
** FUNCTION SM_PROC_GET_BCM_VALIDATION_ON_BEARER_MOD_NV

** DESCRIPTION  : This function returns TRUE if  bcm_validation_on_bearer_mod NV is enabled
                  NV control is used to mitigate NW issue where BCM is omitted but still allow
                  PDP modification when tft is present
** ----------------------------------------------------------------------- */
LOCAL boolean sm_proc_get_bcm_validation_on_bearer_mod_nv(void);

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_STATUS_IND()

  ** DESCRIPTION  : processes STATUS_IND GMM-SM message.
** ----------------------------------------------------------------------- */
LOCAL void sm_process_status_ind(byte trans_id);

/* -----------------------------------------------------------------------
** FUNCTION SM_CONTINUE_RESEND_PDP_MSG

** DESCRIPTION:  This function continues the resending PDP response message to the NW which was thrashed by GRLC
** ------------------------------------------------------------------------- */
void sm_continue_resend_pdp_msg (pdp_context_info *pdp);
#endif /* #ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM */

#ifdef FEATURE_LTE
LOCAL void sm_process_isr_status_ind(gmmsm_isr_status_ind_t  gmm_isr_status_ind);
void sm_process_multimode_subs_chgd_req
(
  void
);
#endif


extern boolean is_ps_umts_mode( sys_modem_as_id_e_type as_id );
extern boolean is_ps_gsm_mode( sys_modem_as_id_e_type as_id ); 

#ifdef FEATURE_DUAL_SIM
#define is_umts_mode() is_ps_umts_mode((sys_modem_as_id_e_type)sm_as_id)
#define is_gsm_mode()  is_ps_gsm_mode((sys_modem_as_id_e_type)sm_as_id)
#else
#define is_umts_mode() is_ps_umts_mode(0)
#define is_gsm_mode()  is_ps_gsm_mode(0)
#endif


#ifdef FEATURE_GSM_GPRS

/* -----------------------------------------------------------------------
** FUNCTION     : SM_SEND_SNDCP_CMD

** DESCRIPTION  : This function sends command to SNDCP
** ----------------------------------------------------------------------- */
extern void sm_send_sndcp_cmd( pdp_context_info *cur_pdp, gsnsm_prim_type sndcp_cmd);

/* -------------------------------------------------------------------------
** FUNCTION     : SM_SEND_SNDCP_DEACT_ALL_NSAPI

** DESCRIPTION  : This function commands SNDCP to deactivate all active NSAPI
** ------------------------------------------------------------------------- */
extern void sm_send_sndcp_deact_all_nsapi(cc_cause_T cause);

#endif

/* -------------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_CON_REL_REJ

** DESCRIPTION  : This function process connection release rej from gmm
** ------------------------------------------------------------------------- */
LOCAL void sm_process_con_rel_rej(
#ifdef FEATURE_DUAL_SIM
        sm_cmd_type* cmd_ptr
#else
        void
#endif
);

extern void sm_unblock_all_apn(sys_modem_as_id_e_type as_id);

/*===========================================================================
** FUNCTION DEFINITIONS 
** =========================================================================*/

/*===========================================================================

FUNCTION GMMSM_PROCESS_RECEIVE_NPDU_NUMBER_LIST

DESCRIPTION
  
  Processes a received NPDU Number List and generates an exchange list for
  transmission back to the network. The exchange list is constructed through
  an SMSN_SEQ_IND/SNSM_SEQ_RSP exchange with SNDCP.
  
  This processing is designed for single thread operation and so the exchange
  with SNDCP is via function call.
  
  The NPDU list is passed as list pointers to valid flags and Receive NPDU
  numbers. List length must be 11 covering NSAPI5 to NSAPI15.
  
  The data at the location given by the specified pointers is modified to
  return the exchange NPDU Number list for transmission.

  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_GSM_GPRS
void sm_process_receive_npdu_number_list
(
  boolean *valid_list_p,
  uint8   *npdu_num_list_p
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  uint8 list_index;

#ifdef FEATURE_DUAL_SIM
  if(SM_IS_VALID_AS_ID(as_id) == FALSE)
      return;
#endif

  if(is_gsm_mode())
  {
    for (list_index = 0; list_index < GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS; list_index++ )
    {
      if ( valid_list_p[list_index] )
      {
        MSG_HIGH_DS_2(SM_SUB," Sending SNSM_SEQUENCE_IND for NSAPI: %d with NPDU Num:%d", 
                    list_index+MIN_NSAPI_NUM, npdu_num_list_p[list_index]);
        npdu_num_list_p[list_index] =
          (uint8)gsndcp_snsm_seq_ind(
#ifdef FEATURE_DUAL_DATA
                                     sm_tlli_sim[as_id],
#else
                                     sm_tlli, 
#endif
                                     (uint8)(list_index + MIN_NSAPI_NUM), 
                                     npdu_num_list_p[list_index]
#ifdef FEATURE_DUAL_SIM
                                      ,as_id
#else
                                      ,SYS_MODEM_AS_ID_NONE
#endif
          );
      }
    }
  } /* GSM */

} /* end of sm_process_npdu_number_list_exchange() */
#endif /* FEATURE_GSM_GPRS */


/*===========================================================================
FUNCTION SM_PROCESS_GMM_CMD 

DESCRIPTION
  This function processes a command received from the GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_process_gmm_cmd(sm_cmd_type* cmd_ptr)
{

  // Process the Command sent by GMM task 

  MSG_HIGH_DS_1( SM_SUB,"GMM Sent CMD_ID:0x%X", 
              cmd_ptr->header.cmd_id );

  switch (cmd_ptr->header.cmd_id)
  {
    case GMMSM_ESTABLISH_CNF :
      
      sm_process_connection_cnf(
        &cmd_ptr->cmd.gmm_establish_cnf,
        TRUE
      );
      break;

#ifdef FEATURE_REL6_PS_FOP
    case GMMSM_PS_FOP_IND :
      
      sm_process_connection_cnf(
        &cmd_ptr->cmd.gmm_establish_cnf,
        FALSE
      );
      break;
#endif/* FEATURE_REL6_PS_FOP */ 

    case GMMSM_ESTABLISH_IND:
      
      sm_process_connection_ind(&cmd_ptr->cmd.gmm_establish_ind);
      break;

    case GMMSM_CON_RELEASE_IND :
     
      sm_process_gmm_con_release_ind(&cmd_ptr->cmd.gmm_con_release_ind);
      break;

    case GMMSM_FAST_DORMANCY_CNF :
      
      sm_process_fast_dormancy_cnf(&cmd_ptr->cmd.gmm_fast_dormancy_cnf);
      break;

    case GMMSM_ESTABLISH_REJ:
      
      sm_process_gprs_rej(&cmd_ptr->cmd.gmm_establish_rej);
      break;

    case GMMSM_GPRS_RELEASE_IND:
      
      sm_process_gprs_rel(&cmd_ptr->cmd.gmm_gprs_rel_ind);
      break;

    case GMMSM_UNITDATA_IND:
      
      sm_process_sm_pdu(&cmd_ptr->cmd.gmm_unitdata_ind);
      break;

#ifdef FEATURE_GSM_GPRS
    case GMMSM_TLLI_ASSIGNMENT_IND:
     #ifdef FEATURE_DUAL_SIM
      #ifdef FEATURE_DUAL_DATA
       if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.snsm_tlli_assignment_ind.as_id) == FALSE)
         return;
      #endif
     #endif
      sm_tlli = cmd_ptr->cmd.snsm_tlli_assignment_ind.tlli;
      break;
#endif

    case GMMSM_RAT_CHANGE_REQ:
      
      sm_process_rat_change(cmd_ptr);
      break;

    case GMMSM_RAT_RESUME_REQ:
      
      sm_process_rat_resume(
        #ifdef FEATURE_DUAL_SIM
        cmd_ptr
        #endif
        );
      break;

    case GMMSM_PDP_MBMS_STATUS_IND:
      MSG_HIGH_DS_0(SM_SUB,"GMM sent GMMSM_PDP_MBMS_STATUS_IND:");
      sm_process_pdp_mbms_status(&cmd_ptr->cmd.gmm_pdp_mbms_context_status);
      break;

   case GMMSM_CON_RELEASE_REJ:
      
      sm_process_con_rel_rej(
        #ifdef FEATURE_DUAL_SIM
        cmd_ptr
        #endif
        );
      break;

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
   case GMMSM_STATUS_IND:
      MSG_HIGH_DS_1(SM_SUB,"GMM sent GMMSM_STATUS_IND for TI:: %d", cmd_ptr->header.connection_id);
      #ifdef FEATURE_DUAL_SIM
      #ifdef FEATURE_DUAL_DATA
       if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.gmm_status_ind.as_id) == FALSE)
         return;
      #endif
      #endif
      sm_process_status_ind (cmd_ptr->header.connection_id);
      break;
#endif /* #ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM */

#if defined(FEATURE_LTE)
   case GMMSM_ISR_STATUS_IND:
      MSG_HIGH_DS_1(SM_SUB,"GMM sent GMMSM_ISR_STATUS_IND ISR Status=%d", cmd_ptr->cmd.gmm_isr_status_ind.isr_status);
      sm_process_isr_status_ind(cmd_ptr->cmd.gmm_isr_status_ind);
      break;
#endif

   case GMMSM_UNBLOCK_ALL_APN:
      
      sm_unblock_all_apn(cmd_ptr->cmd.gmmsm_unblock_all_apn.as_id);
      break; 
#if defined(FEATURE_LTE) && defined(FEATURE_DUAL_SIM)
   case GMMSM_MULTIMODE_SUBS_CHGD_REQ:
      if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.gmmsm_multimode_subs_chgd_req.as_id) == TRUE)
      { 
        MSG_HIGH_DS_0(SM_SUB,"GMM sent GMMSM_MULTIMODE_SUBS_CHGD_REQ ");       
        sm_process_multimode_subs_chgd_req();
      }
      break; 
#endif

    default:
      MSG_ERROR_DS_1(SM_SUB," UNKNOWN cmd from GMM : %d", cmd_ptr->header.cmd_id);
      break;
  };
}

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION SEND_GMM_OK_TO_TUNEAWAY_IND 

DESCRIPTION
  This function sends tuneaway status to MM

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void send_gmm_ok_to_tuneaway_ind(void)
{
#ifdef FEATURE_NAS_FREE_FLOAT
  mm_cmd_type * gmm_cmd;
  boolean current_ok_for_tuneaway_status = FALSE;
  boolean umts_is_ok_to_tuneaway = FALSE;

  current_ok_for_tuneaway_status = sm_ok_for_tuneaway();
  
  /* if tuneaway not allowed, block tuneaway for active rat */
  if(current_ok_for_tuneaway_status == FALSE)
  {
    if(is_gsm_mode())
    {
#ifdef FEATURE_GSM_GPRS
      geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_PDP,(boolean)!(current_ok_for_tuneaway_status), (sys_modem_as_id_e_type)sm_as_id);
#endif /* FEATURE_GSM_GPRS */
      umts_is_ok_to_tuneaway = TRUE;
    }
    else if (is_umts_mode())
    {
#ifdef FEATURE_GSM_GPRS
      geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_PDP, current_ok_for_tuneaway_status, (sys_modem_as_id_e_type)sm_as_id);
#endif /* FEATURE_GSM_GPRS */
      umts_is_ok_to_tuneaway = FALSE;        
    }
  }
  else
  {
#ifdef FEATURE_GSM_GPRS
     geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_PDP, (boolean)!(current_ok_for_tuneaway_status), (sys_modem_as_id_e_type)sm_as_id);
#endif /* FEATURE_GSM_GPRS */
     umts_is_ok_to_tuneaway = TRUE;
  }

    gmm_cmd = mm_get_cmd_buf();
    if (gmm_cmd != NULL)
    {
      gmm_cmd->cmd.gmmsm_ok_to_tuneaway_ind.header.message_set     = MS_GMM_SM;
      gmm_cmd->cmd.gmmsm_ok_to_tuneaway_ind.header.message_id      = GMMSM_OK_TO_TUNEAWAY_IND;
      PUT_IMH_LEN( sizeof( gmmsm_ok_to_tuneaway_ind_T ) - sizeof(IMH_T),
                   &gmm_cmd->cmd.gmmsm_ok_to_tuneaway_ind.header );

      gmm_cmd->cmd.gmmsm_ok_to_tuneaway_ind.ok_to_tuneaway = umts_is_ok_to_tuneaway;

#ifdef FEATURE_DUAL_SIM
      gmm_cmd->cmd.gmmsm_ok_to_tuneaway_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif 
    
      mm_put_cmd (gmm_cmd);
    }
    else
    {
      MSG_ERROR_DS_0( SM_SUB, "Unable to get a GMM command buffer");
    } 
    
       
  MSG_HIGH_DS_2(SM_SUB, "Sending current_ok_for_tuneaway_status(%d), umts(%d) ",current_ok_for_tuneaway_status, umts_is_ok_to_tuneaway);
#endif
}
#endif

/*===========================================================================
FUNCTION SEND_GMM_SMPDU 

DESCRIPTION
  This function sends the packed SM-PDU to the GMM to be sent to the Network. 

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void send_gmm_smpdu(word mesg_len, byte* l3_buf, boolean pdp_inactive_flag, boolean ps_vote_flag)
{
  mm_cmd_type * gmm_cmd;

  gmm_cmd = mm_get_cmd_buf();

  if (gmm_cmd != NULL)
  {
    gmm_cmd->cmd.gmmsm_unitdata_req.header.message_set     = MS_GMM_SM;
    gmm_cmd->cmd.gmmsm_unitdata_req.header.message_id      = GMMSM_UNITDATA_REQ;
    PUT_IMH_LEN((mesg_len),&gmm_cmd->cmd.gmmsm_unitdata_req.header);

#ifndef FEATURE_MODEM_HEAP
    gmm_cmd->cmd.gmmsm_unitdata_req.data =  gs_alloc(mesg_len);
#else
    gmm_cmd->cmd.gmmsm_unitdata_req.data =  modem_mem_calloc(1, mesg_len, MODEM_MEM_CLIENT_NAS);
#endif
    sm_check_for_null_ptr((void*)gmm_cmd->cmd.gmmsm_unitdata_req.data);
    memscpy(gmm_cmd->cmd.gmmsm_unitdata_req.data, mesg_len,
            l3_buf, mesg_len);


#ifdef FEATURE_DUAL_SIM
    gmm_cmd->cmd.gmmsm_unitdata_req.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

#ifdef FEATURE_GPRS_PS_HANDOVER    
    gmm_cmd->cmd.gmmsm_unitdata_req.purgeQueueFlag = pdp_inactive_flag;
#endif
    gmm_cmd->cmd.gmmsm_unitdata_req.ps_vote_flag = ps_vote_flag;
    MSG_HIGH_DS_2( SM_SUB,"Sending SM-PDU to GMM, pdp_inactive_flag=%d, ps_vote_flag=%d ",pdp_inactive_flag,ps_vote_flag);
    mm_put_cmd (gmm_cmd);
  }
  else
  {
    MSG_ERROR_DS_0( SM_SUB,"Unable to get a GMM command buffer");
  } 
}

/*===========================================================================
FUNCTION SEND_GMM_ESTABLISH_REQ 

DESCRIPTION
  This function sends the packed SM-PDU to the GMM to be sent to the Network. 

DEPENDENCIES
  Takes the RRC connection establishment cause as the cause.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void send_gmm_establish_req(rrc_establish_cause_e_type rrc_cause)
{
  mm_cmd_type * gmm_cmd;
  gmm_cmd = mm_get_cmd_buf();

  if (gmm_cmd != NULL)
  {
    gmm_cmd->cmd.gmmsm_establish_req.header.message_set     = MS_GMM_SM;
    gmm_cmd->cmd.gmmsm_establish_req.header.message_id      = GMMSM_ESTABLISH_REQ;
    gmm_cmd->cmd.gmmsm_establish_req.header.message_len_lsb = 0;
    gmm_cmd->cmd.gmmsm_establish_req.header.message_len_msb = 0;  

    gmm_cmd->cmd.gmmsm_establish_req.cause = rrc_cause;
#ifdef FEATURE_DUAL_SIM
    gmm_cmd->cmd.gmmsm_establish_req.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

    MSG_HIGH_DS_0( SM_SUB,"Sending Establish Req to GMM");
    mm_put_cmd (gmm_cmd);
  }
  else
  {
    MSG_ERROR_DS_0( SM_SUB,"Unable to get a GMM command buffer");
  } 
}

/*===========================================================================
FUNCTION SEND_GMM_FAST_DORMANCY_REQ 

DESCRIPTION
  This function sends the packed SM-PDU to the GMM to be sent to the Network. 

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void send_gmm_fast_dormancy_req()
{
  mm_cmd_type * gmm_cmd;

  gmm_cmd = mm_get_cmd_buf();
  if (gmm_cmd != NULL)
  {
    gmm_cmd->cmd.gmmsm_session_close_req.header.message_set     = MS_GMM_SM;
    gmm_cmd->cmd.gmmsm_session_close_req.header.message_id      = GMMSM_FAST_DORMANCY_REQ;
    gmm_cmd->cmd.gmmsm_session_close_req.header.message_len_lsb = 0;
    gmm_cmd->cmd.gmmsm_session_close_req.header.message_len_msb = 0;  
#ifdef FEATURE_DUAL_SIM
    gmm_cmd->cmd.gmmsm_session_close_req.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif 

    MSG_HIGH_DS_0(SM_SUB, "Sending GMMSM_FAST_DORMANCY_REQ to GMM");
    mm_put_cmd (gmm_cmd);
  }
  else
  {
    MSG_ERROR_DS_0( SM_SUB, "Unable to get a GMM command buffer");
  } 
}

/*===========================================================================
FUNCTION SEND_GMM_CLOSE_SESSION_REQ 

DESCRIPTION
  This function sends the packed SM-PDU to the GMM to be sent to the Network. 

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void send_gmm_close_session_req()
{
  mm_cmd_type * gmm_cmd;

  if (!sm_check_pdp_exist() 
        || cm_initiated_ps_signalling_release
     )
  {
    gmm_cmd = mm_get_cmd_buf();
    if (gmm_cmd != NULL)
    {
      gmm_cmd->cmd.gmmsm_session_close_req.header.message_set     = MS_GMM_SM;
      gmm_cmd->cmd.gmmsm_session_close_req.header.message_id      = GMMSM_CLOSE_SESSION_REQ;
      gmm_cmd->cmd.gmmsm_session_close_req.header.message_len_lsb = 0;
      gmm_cmd->cmd.gmmsm_session_close_req.header.message_len_msb = 0;  

#ifdef FEATURE_DUAL_SIM
       gmm_cmd->cmd.gmmsm_session_close_req.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

      gmm_cmd->cmd.gmmsm_session_close_req.cause = SM_PS_SIG_REL_REQ;
 
      if (cm_initiated_ps_signalling_release)
      {
  #ifdef FEATURE_DUAL_SIM
        if (cm_initiated_ps_signalling_release_cause == SMREG_DUAL_STANDBY_CS_MO_CALL)
        {
          gmm_cmd->cmd.gmmsm_session_close_req.cause = SM_DS_PS_SIG_REL_REQ;
        }
        else if (cm_initiated_ps_signalling_release_cause == SMREG_DUAL_TRIPLE_STANDBY_CHANGE)
        {
          gmm_cmd->cmd.gmmsm_session_close_req.cause = SM_TS_DS_SIG_REL_REQ;
        }
        else
  #endif
        {
          gmm_cmd->cmd.gmmsm_session_close_req.cause = SM_UE_PS_SIG_REL_REQ;
        }
      }

      MSG_HIGH_DS_0( SM_SUB,"Sending CLOSE Session Req to GMM");
      mm_put_cmd (gmm_cmd);
    }
    else
    {
      MSG_ERROR_DS_0( SM_SUB,"Unable to get a GMM command buffer");
    } 
  }
}

/*===========================================================================
FUNCTION SM_PROCESS_CONNECTION_CNF 

DESCRIPTION
  This function processes the GPRS received confirmation cmd from GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_process_connection_cnf(
        gmmsm_establish_cnf_T* gmmsm_establish_cnf,
        boolean retry
)
{
  word i;
  int pending_msg;
  boolean pdp_con_pending = FALSE;
  pending_msg = 0; 
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  if (!( SM_IS_AS_ID_IN_VALID_RANGE(gmmsm_establish_cnf->as_id) == TRUE && 
       (SM_IS_VALID_AS_ID(gmmsm_establish_cnf->as_id)) == TRUE))
  {
     MSG_HIGH_DS_1( SM_SUB,"Invalid AS ID in GMMSM_UNITDATA_IND : %d", gmmsm_establish_cnf->as_id );
     return;
  }
#endif
#endif

  /* Clear the establishment timer. */
  #ifndef FEATURE_DUAL_DATA
  sm_clear_pdp_timer(TIMER_ESTABLISH_PENDING);
  #endif

  //Reset the timer_est_pen flag
  timer_est_pen_flag = FALSE;
  
  // Set the Signaling Connection variable as TRUE
  if(retry == TRUE)  
  {
   sig_connection_available = TRUE;

   //Set the PS signalling connection state as connected
   ps_signalling_conn_state = TRUE;

   max_est_timer_count = 0;

   sm_is_pdp_retranmsit_req = gmmsm_establish_cnf->is_pdp_retransmit_req;
  }
  
  
  /* --------------------------------------------------------------------
  * GPRS attach is performed by GMM, Proceed with pending Activations 
  * Get the pending PDP context from the list and activate. 
  ** -------------------------------------------------------------------*/
  for (i=0; i<MAX_PDP_NUM; i++)
  {
    if (pdp_list[i] != NULL)
    {
      if (pdp_list[i]->pdp_con_state == SIG_CONNECTION_PENDING)
      {
        /*If PDP connection state is SIG_CONNECTION_PENDING then set the 
          pdp_con_pending to TRUE*/
         pdp_con_pending = TRUE;
         #ifdef FEATURE_DUAL_DATA
          sm_stop_timer(i);
         #endif
  
        //set the pdp_con_state to SIG_CONNECTION_AVAILABLE
        if(retry == TRUE)
        {
         pdp_list[i]->pdp_con_state = SIG_CONNECTION_AVAILABLE;
        }
        /* ----------------------------------------------------------------
        ** Found an entry in the PDP list that needs activation, Continue
        ** with sending the Signaling message for the PDP context.
        ** ---------------------------------------------------------------*/
        pending_msg = pdp_list[i]->cm_pending_msg;
  
        if (pending_msg == (int)SMREG_PDP_ACTIVATE_REQ || 
            pending_msg == (int)SMREG_PDP_ACTIVATE_SEC_REQ)
        {
        MSG_MED_DS_1(SM_SUB,"PDP nsapi# %d being activated", pdp_list[i]->nsapi.nsapi);
          sm_continue_pdp_activation(pdp_list[i]);
        }
      #ifdef FEATURE_MBMS
        #error code not present
#endif
        else if (pending_msg == (int)SMREG_PDP_ACTIVATE_REJ_RSP)
        {
        MSG_MED_DS_1(SM_SUB,"Network initiated PDP request for %d being rejected", pdp_list[i]->trans_id);
          sm_continue_pdp_activate_rej_rsp (pdp_list[i]);
        }   
        else if (pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ ||    
                 !pdp_list[i]->qos_valid
                 || pending_msg == (int)SMREG_PDP_GRACEFUL_ABORT_REQ
              )
        {
        MSG_MED_DS_1(SM_SUB,"PDP nsapi# %d being deactivated", pdp_list[i]->nsapi.nsapi);
          sm_continue_pdp_deactivate(pdp_list[i]);
        }
        else if (pending_msg == (int)MS_MODIFY_PDP_ACCEPT)
        {
          MSG_MED_DS_1( SM_SUB,"PDP ConnID# %d NW modify PDP Accept", pdp_list[i]->trans_id);
          sm_continue_pdp_nw_modify_cnf(pdp_list[i]);
        }
        else if(pending_msg == (int) MODIFY_PDP_REJECT)
        {
          MSG_MED_DS_1( SM_SUB,"PDP ConnID# %d NW modify PDP Reject", pdp_list[i]->trans_id);
          sm_continue_pdp_nw_modify_rej(pdp_list[i]);
        }       
        else if (pdp_list[i]->pdp_state == PDP_MODIFY_PENDING)
        {
        MSG_MED_DS_1(SM_SUB,"PDP nsapi# %d being modified", pdp_list[i]->nsapi.nsapi);
          sm_continue_pdp_modify(pdp_list[i]);
        }           
      }
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
      else if (pdp_list[i]->status_ind_flag == TRUE)
      {
        sm_continue_resend_pdp_msg (pdp_list[i]);
      }
#endif      
    }
  }

  /*If none of the PDP is waiting for GMM connection close the session*/
  if (!pdp_con_pending  && is_umts_mode())
  {
    // Close PS Signaling Session 
    MSG_HIGH_DS_0( SM_SUB,"Calling GMM Close Session Req");
    send_gmm_close_session_req();
  }
}

/*===========================================================================
FUNCTION SM_PROCESS_CONNECTION_IND 

DESCRIPTION
  This function processes the GMM connection ind to indicate GMM is in 
   PMM_connected.state. SM has to trigger the PDP context modification procedure
   for PDPs with trafiic class streaming and conversational to bring upgrade the
   bitrate to original values.If a Service request was sent in response of page and a 
   reject is recieved from the network then locally deactivate pdp
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_process_connection_ind(gmmsm_establish_ind_T *gprs_ind)
{
  word i;
  int pending_msg;
  pending_msg = 0;

  if (gprs_ind->cause == SM_UNASSIGNED_CAUSE )
  {
    //Set the sig coonection flag to true
    sig_connection_available = TRUE;

    //Set the PS signalling connection state as connected
    ps_signalling_conn_state = TRUE;

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
     if(SM_IS_VALID_AS_ID(gprs_ind->as_id) == FALSE)
     {
       MSG_ERROR_DS_2( SM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , gprs_ind->as_id);
       return ;
     }
#endif
#endif

    /*GMM indicates the PS signalling connection due to the RRC page ind. Iniate the 
    RAB modification procedures for any RAB which is in dormant with traffic class
    streaming or conversational*/
    sm_modify_rab(TRUE);
  }
  else
  {
    if(gprs_ind->cause == SM_PS_SIG_CONN_AVAIL)
    {
	  /* Clear the establishment timer. */
	  #ifndef FEATURE_DUAL_DATA
	  sm_clear_pdp_timer(TIMER_ESTABLISH_PENDING);
	  #endif

	  //Reset the timer_est_pen flag
	  timer_est_pen_flag = FALSE;
	  
	  // Set the Signaling Connection variable as TRUE
	   sig_connection_available = TRUE;

	   //Set the PS signalling connection state as connected
	   ps_signalling_conn_state = TRUE;

	   max_est_timer_count = 0;

	  
	  
	  /* --------------------------------------------------------------------
	  * GPRS attach is performed by GMM, Proceed with pending Activations 
	  * Get the pending PDP context from the list and activate. 
	  ** -------------------------------------------------------------------*/
	  for (i=0; i<MAX_PDP_NUM; i++)
	  {
		if (pdp_list[i] != NULL)
		{
		  if (pdp_list[i]->pdp_con_state == SIG_CONNECTION_PENDING)
		  {

		     #ifdef FEATURE_DUAL_DATA
			  sm_stop_timer(i);
		     #endif
	  
			pdp_list[i]->pdp_con_state = SIG_CONNECTION_AVAILABLE;
			
			/* ----------------------------------------------------------------
			** Found an entry in the PDP list that needs activation, Continue
			** with sending the Signaling message for the PDP context.
			** ---------------------------------------------------------------*/
			pending_msg = pdp_list[i]->cm_pending_msg;
	  
			if (pending_msg == (int)SMREG_PDP_ACTIVATE_REQ || 
				pending_msg == (int)SMREG_PDP_ACTIVATE_SEC_REQ)
			{
			MSG_MED_DS_1(SM_SUB,"PDP nsapi# %d being activated", pdp_list[i]->nsapi.nsapi);
			  sm_continue_pdp_activation(pdp_list[i]);
			}
		#ifdef FEATURE_MBMS
			#error code not present
#endif
			else if (pending_msg == (int)SMREG_PDP_ACTIVATE_REJ_RSP)
			{
			MSG_MED_DS_1(SM_SUB,"Network initiated PDP request for %d being rejected", pdp_list[i]->trans_id);
			  sm_continue_pdp_activate_rej_rsp (pdp_list[i]);
			}   
			else if (pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ ||    
					 !pdp_list[i]->qos_valid
					 || pending_msg == (int)SMREG_PDP_GRACEFUL_ABORT_REQ
				  )
			{
			MSG_MED_DS_1(SM_SUB,"PDP nsapi# %d being deactivated", pdp_list[i]->nsapi.nsapi);
			  sm_continue_pdp_deactivate(pdp_list[i]);
			}
			else if (pending_msg == (int)MS_MODIFY_PDP_ACCEPT)
			{
			  MSG_MED_DS_1( SM_SUB,"PDP ConnID# %d NW modify PDP Accept", pdp_list[i]->trans_id);
			  sm_continue_pdp_nw_modify_cnf(pdp_list[i]);
			}
			else if(pending_msg == (int) MODIFY_PDP_REJECT)
			{
			  MSG_MED_DS_1( SM_SUB,"PDP ConnID# %d NW modify PDP Reject", pdp_list[i]->trans_id);
			  sm_continue_pdp_nw_modify_rej(pdp_list[i]);
			}       
			else if (pdp_list[i]->pdp_state == PDP_MODIFY_PENDING)
			{
			MSG_MED_DS_1(SM_SUB,"PDP nsapi# %d being modified", pdp_list[i]->nsapi.nsapi);
			  sm_continue_pdp_modify(pdp_list[i]);
			}           
		  }
	#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
		  else if (pdp_list[i]->status_ind_flag == TRUE)
		  {
			sm_continue_resend_pdp_msg (pdp_list[i]);
		  }
	#endif      
		}
	  } /* end of for */
    }   /* if(gprs_ind->cause == SM_PS_SIG_CONN_AVAIL) */
    else if ((gprs_ind->cause == SM_NO_PDP_CONTEXT_ACTIVATED)
       ||(gprs_ind->cause == SM_ILLEGAL_MS)
       ||(gprs_ind->cause == SM_GPRS_SERVICES_NOT_ALLOWED)
       ||(gprs_ind->cause == SM_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK)
       ||(gprs_ind->cause == SM_IMPLICITLY_DETACHED)
       ||(gprs_ind->cause == SM_PLMN_NOT_ALLOWED)
       ||(gprs_ind->cause == SM_LA_NOT_ALLOWED)
       ||(gprs_ind->cause == SM_ILLEGAL_ME))
    {
      /* Perform a Local deactivation of the ACTIVE PDP contexts */
      sm_perform_local_deactivation((cc_cause_T)gprs_ind->cause);
    }
  }
}

/*===========================================================================
FUNCTION SM_PROCESS_FAST_DORMANCY_CNF 

DESCRIPTION
  This function processes connection release ind from GMM
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void sm_process_fast_dormancy_cnf (gmmsm_fast_dormancy_cnf_T *dormancy_cnf)
{

  MSG_HIGH_DS_1(SM_SUB," Recd GMMSM_FAST_DORMANCY_CNF, cause %d", dormancy_cnf->cause);

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
    if(SM_IS_VALID_AS_ID(dormancy_cnf->as_id) == FALSE)
       return;
#endif
    MSG_HIGH_DS_1( SM_SUB,"Message as id: %d", dormancy_cnf->as_id);
#endif

 if (cm_initiated_ps_signalling_release == TRUE)
    {
       cm_initiated_ps_signalling_release = FALSE;
       if ( dormancy_cnf->cause == GMMSM_FAST_DORMANCY_SUCCESS_APP_NO_RETRY)
       {
         /*Send CM about this SIG CON REL confirm */      
         sm_send_cm_ps_sig_rel_confirm(SM_PS_SIG_REL_SUCCESS);
       }
       else
       {
          sm_send_cm_ps_sig_rel_confirm(SM_PS_SIG_REL_FAILURE);
       }
    }

}

/*===========================================================================
FUNCTION SM_PROCESS_GMM_CON_RELEASE_IND 

DESCRIPTION
  This function processes connection release ind from GMM
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void sm_process_gmm_con_release_ind(gmmsm_con_release_ind_T *rel_ind)
{                            
  word i, restore_max_timer_count;
 

#if defined (FEATURE_DUAL_SIM)
  boolean as_id_valid = TRUE;
#ifndef FEATURE_DUAL_DATA
   ctxt_as_id = rel_ind->as_id;
   if(SM_IS_VALID_AS_ID(rel_ind->as_id) == FALSE)
   {
     as_id_valid = FALSE;
   }
#endif
#endif

   //Set the PS signalling connection as FALSE
  ps_signalling_conn_state = FALSE;

#if defined (FEATURE_DUAL_SIM)
  if (as_id_valid == TRUE)
  {
#endif

  // Process the Error Indication report received from GMM.
  if (rel_ind->cause == SIG_CONNECTION_RELEASE || 
      rel_ind->cause == SIG_CONNECTION_RELEASE_UE_INITIATED)
  {
    MSG_HIGH_DS_0(SM_SUB," Recd Signaling Connection Release Indication");

    /* ----------------------------------------------------------------------
    ** Set the Signaling connection flag to FALSE, this forces the
    ** SM to re-establish the connection before sending a SIG message to GMM
    ** --------------------------------------------------------------------*/
    sig_connection_available = FALSE;
    
#ifdef FEATURE_GSM_GPRS
    /* ------------------------------------------------------------------
    ** Check if the UE is running in GPRS mode of operation and send
    ** GSNSM_DEACTIVATE_IND to the SNDCP.
    ** -----------------------------------------------------------------*/
    if ( is_gsm_mode() )
    {
      /* ------------------------------------------------------------------
      ** Move all the active PDP contexts to dormant mode
      ** ----------------------------------------------------------------*/
      /*
      for (i=0; i<MAX_PDP_NUM; i++)
      {
        if ( pdp_list[i] != NULL && pdp_list[i]->pdp_state != PDP_INACTIVE )
        {
          MSG_ERROR_DS_0(SM_SUB,"RR Sig con lost, PDP in dormant");
        }
      }
      */
    }     

#endif /*FEATURE_GSM_GPRS*/    
      /* Inform the RABM that the Signaling Connection is lost. */
    MSG_HIGH_DS_0(SM_SUB, "Inform RABM SIG connection lost");             
    sm_notify_rabm( (int)RABMSM_SIG_CON_RELEASE_IND, NO_SPECIFIC_NSAPI);
  }
#if defined (FEATURE_DUAL_SIM)
  }
#endif
  if(cm_initiated_ps_signalling_release)
  {
    cm_initiated_ps_signalling_release = FALSE;
    sm_send_cm_ps_sig_rel_confirm(SM_PS_SIG_REL_SUCCESS);
  }
/*Send CM about this SIG CON REL Indication */
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
  sm_send_cm_confirm( NULL, SMREG_PDP_SIG_CON_REL_IND,
                           SM_INTERNAL_CAUSE );
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
#if defined (FEATURE_DUAL_SIM)
  if (as_id_valid == TRUE)
  {
#endif
// Reset PDP cause values
// set the PDP cause to Reject cause for all the PDPs waiting for Signalling Connection
  for (i = 0 ; i < MAX_PDP_NUM ; i++)
  {
    if (pdp_list[i] != NULL && pdp_list[i]->pdp_con_state == SIG_CONNECTION_PENDING)
    {
       //Don't set the cause to default value for the PDPs in INACTIVE_PENDING, to reatin the original cause while sending it to NW
       if(pdp_list[i]->pdp_state != PDP_INACTIVE_PENDING)
       {
              pdp_list[i]->current_cause = (cc_cause_T) PPD_UNKNOWN_REASON ;
           }
    }

// As soon as SM receives a connection release indication, it does not wait for timer to expire, rather, timer is stopped
// & SM sends out another SMREG_PDP_ACTIVATE_REQ to the network.
    if ( pdp_list[i] != NULL && pdp_list[i]->pdp_state == PDP_ACTIVE_PENDING && sm_is_pdp_retranmsit_req == TRUE)
    {
      restore_max_timer_count = sm_max_timer_count[i];
      sm_stop_timer(i);
      sm_max_timer_count[i] = restore_max_timer_count;
      sm_continue_pdp_activation(pdp_list[i]);
    }

  }

#if defined (FEATURE_DUAL_SIM)
  }
#ifndef FEATURE_DUAL_DATA
  ctxt_as_id = SYS_MODEM_AS_ID_NONE;
#endif
#endif

}

/*===========================================================================
FUNCTION SM_PROCESS_GPRS_REL 

DESCRIPTION
  This function processes the GPRS release cmd from GMM. 
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_process_gprs_rel(gmmsm_gprs_release_ind_T* gprs_rel )
{
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
    if(SM_IS_VALID_AS_ID(gprs_rel->as_id) == FALSE)
    {
      return ;
    }
    MSG_HIGH_DS_2(SM_SUB,"Message as id: %d ctxt_as_id %d", gprs_rel->as_id, ctxt_as_id);
    ctxt_as_id= gprs_rel->as_id;
#else
    MSG_HIGH_DS_1(SM_SUB,"Message as id: %d", gprs_rel->as_id);
#endif 

    //Avoid releasing inactive PDP on non gmm as_id
    if((sys_modem_as_id_e_type)sm_as_id == SYS_MODEM_AS_ID_NONE)
    {
      word i;
      for (i=0; i<MAX_PDP_NUM; i++)
      {
        if (pdp_list[i] != NULL && 
            pdp_list[i]->pdp_state == PDP_INACTIVE)
        {
          MSG_HIGH_DS_2( SM_SUB,"Ignoring GPRS rel IND sm_as_id : %d, message as_id: %d", sm_as_id , gprs_rel->as_id);
          return;
        }
      }
    }
#endif

  // Set the connection unavailable before deactivating all the PDP contexts.
  sig_connection_available = FALSE;
  max_est_timer_count = 0;
  //Reset the timer_est_pen flag
  timer_est_pen_flag = FALSE;
  /* Clear the establishment timer. */
  #ifndef FEATURE_DUAL_DATA
  sm_clear_pdp_timer(TIMER_ESTABLISH_PENDING);
  #endif

  sm_perform_local_deactivation((cc_cause_T)gprs_rel->cause);
  
#if defined(FEATURE_LTE)
  sm_abort_pending_esm_proc();
  esm_deactivate_bearer_ctxts_irat_to_GW();
#endif

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  ctxt_as_id = SYS_MODEM_AS_ID_NONE;
#endif
#endif
}

/*===========================================================================
FUNCTION SM_PROCESS_GPRS_REJ 

DESCRIPTION
  This function processes the GPRS reject message from GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_process_gprs_rej(gmmsm_establish_rej_T* gprs_rej)
{
  int index;
  /* -----------------------------------------------------------------
  ** GPRS attach is not successful. Send a REJECT message to CM and 
  ** delete and free the transaction ID from the PDP context list 
  ** ----------------------------------------------------------------*/
  MSG_HIGH_DS_1(SM_SUB," Connection Establish failed, cause 0x:%0x", gprs_rej->cause);
  
  
  if ((gprs_rej->cause == SM_ILLEGAL_MS) ||
      (gprs_rej->cause == SM_ILLEGAL_ME) ||
      (gprs_rej->cause == PDP_LOWERLAYER_ERROR) ||
      (gprs_rej->cause == SM_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK) ||
      (gprs_rej->cause == SM_GPRS_SERVICES_NOT_ALLOWED ) ||
      (gprs_rej->cause == SM_IMPLICITLY_DETACHED) ||
      (gprs_rej->cause == SM_ACCESS_CLASS_DSAC_REJECTION) ||
      (gprs_rej->cause == SM_CONGESTION) ||
      (gprs_rej->cause == SM_LA_NOT_ALLOWED)
      || ((sm_abort_cnf_pending) && (gprs_rej->cause == SM_ABORT_SERVICE_NOT_AVAILABLE))
      )
  {
    //Abort the connection establish procedure
    MSG_HIGH_DS_0(SM_SUB," Aborting sm gmm conn proc ");
    sm_abort_gmm_conn_proc((cc_cause_T)gprs_rej->cause);
  }
#ifdef FEATURE_DUAL_SIM
  else if (gprs_rej->cause == SM_NO_RADIO_AVAILABLE)
  {
    //Abort the connection establish procedure
    MSG_HIGH_DS_1(SM_SUB," Aborting sm gmm conn proc message as id: %d", gprs_rej->as_id);
    sm_abort_gmm_conn_proc((cc_cause_T) gprs_rej->cause);
  }
#endif
  else if (gprs_rej->cause == SM_NO_PDP_CONTEXT_ACTIVATED)
  {
    sm_perform_local_deactivation((cc_cause_T)gprs_rej->cause);
  }
  else if ((gprs_rej->cause == SM_NATIONAL_ROAMING_NOT_ALLOWED || 
             gprs_rej->cause == SM_NO_SUITABLE_CELLS_IN_LA) && gmm_anite_gcf_flag)
  {
    /* Abort the Connection Establishment Procedure for these causes. CR # 58591 */
    MSG_HIGH_DS_0(SM_SUB," Aborting sm gmm conn proc ");
    sm_abort_gmm_conn_proc((cc_cause_T)PPD_UNKNOWN_REASON); 
  }
  else if (rabm_get_3gpp_global_throtling_enabled())
  {
   /* Abort the Connection Establishment Procedure for other causes 4.7.13.4 in TS 24.008*/
    MSG_HIGH_DS_0(SM_SUB," Aborting sm gmm conn proc with cause SM_NAS_SRV_REQ_FAILURE  ");
    sm_abort_gmm_conn_proc((cc_cause_T)SM_NAS_SRV_REQ_FAILURE); 
  }  
  /* --------------------------------------------------------------------
  ** Set the Signaling connection flag to FALSE, this forces the SM to
  ** re-establish the connection before sending a SIG message to GMM.
  ** ------------------------------------------------------------------*/
  sig_connection_available = FALSE;
  
  // set the PDP cause to Reject cause for all the PDPs waiting for Signalling Connection
  for (index = 0 ; index < MAX_PDP_NUM ; index++)
  {
    if (pdp_list[index] != NULL  &&
            pdp_list[index]->pdp_con_state == SIG_CONNECTION_PENDING && 
        (gprs_rej->cause == SM_CONGESTION || gprs_rej->cause == SM_ACCESS_CLASS_DSAC_REJECTION) )
    {
        pdp_list[index]->current_cause = (cc_cause_T)gprs_rej->cause;
    }
  }

}

/*===========================================================================
FUNCTION SM_PROCESS_SM_PDU 

DESCRIPTION
  This function processes SM-PDU sent by GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_process_sm_pdu(gmmsm_unitdata_ind_T* sm_pdu)
{
  xlate_status_T status;      /* Status of the decoded message */
  sm_layer_msg_T recd_msg;    /* Received L3 message type */
  word recd_trans_id;         /* Trasaction Id in the L3 message */
  word i;
  boolean ti_found, ti_flag;        
  uint16 msg_len;
  pdp_context_info* cur_pdp;
  smreg_pdp_activate_rej_resp_T *rej_resp = NULL;
#ifdef FEATURE_MBMS
  #error code not present
#endif
  
  pdp_context_info* req_pdp_cxt=NULL;
//   word stored_trans_id;
  byte data_index = 1;
  boolean mem_allocated = FALSE;
  boolean status_msg_sent = FALSE;


#ifdef FEATURE_MODEM_HEAP
  req_pdp_cxt = modem_mem_calloc(1,sizeof(pdp_context_info),MODEM_MEM_CLIENT_NAS);
#else
  req_pdp_cxt = (pdp_context_info *)gs_alloc(sizeof(pdp_context_info));
#endif

  if (req_pdp_cxt == NULL)
  {
    MSG_ERROR_DS_0(SM_SUB,"Memory Allocation failed");
    return;
  }

#ifdef FEATURE_MODEM_HEAP
  rej_resp = modem_mem_calloc(1,sizeof(smreg_pdp_activate_rej_resp_T),MODEM_MEM_CLIENT_NAS);
#else
  rej_resp = (smreg_pdp_activate_rej_resp_T *)gs_alloc(sizeof(smreg_pdp_activate_rej_resp_T));
#endif
  
  if (rej_resp == NULL)
  {
    MSG_ERROR_DS_0(SM_SUB,"Memory Allocation failed");
    return;
  }
  
#ifdef FEATURE_MBMS
#error code not present
#endif

  sm_init_pdp_context(req_pdp_cxt);

  (void) memset(rej_resp,0,sizeof(smreg_pdp_activate_rej_resp_T));
  #ifdef FEATURE_MBMS
  #error code not present
#endif
  cur_pdp = NULL;     /* Set the default pointer value to NULL */
  ti_found  = FALSE;  /* Set default value */
  
  MSG_HIGH_DS_0(SM_SUB," GMM sent SM-PDU: Processing SM-PDU");


  // Validate the PD and TI of the received L3 message and get the TI.
  status = validate_layer3_msg(sm_pdu->data, &recd_trans_id);
  if (recd_trans_id > 6)
    {
        data_index = 2; // index to Message Id
    }

  if (status == L3_PARSE_SUCCESS)
  {
    // Get the TI flag, this identifies if its a MT or MO message
    ti_flag = sm_pdu->data[0] & 0x80;

    // for network init pdp activations
    recd_trans_id = (!ti_flag)?  (recd_trans_id + MIN_MT_TI) : recd_trans_id;   
     

    // Get the Message type sent by the Network in the SM-PDU
    recd_msg = (sm_layer_msg_T)sm_pdu->data[data_index];

    // Get the PDP context based on the Transaction Id
    ti_found = FALSE;
    if(pdp_list[recd_trans_id] != NULL)
    {
      ti_found = TRUE;     
    }
    // ti_found = (pdp_list[recd_trans_id]->trans_id == recd_trans_id) ? TRUE : FALSE;
    if((!ds3gpp_per_subs_get_config_umts_nw_init_qos_support((sys_modem_as_id_e_type)sm_as_id))&& (recd_msg == REQUEST_SEC_PDP_ACTIVATION))
    {
       MSG_ERROR_DS_0( SM_SUB,"UMTS_NW_INITIATED_QOS_SUPPORT disabled, but got REQUEST_SEC_PDP_ACTIVATION");
       cur_pdp = req_pdp_cxt;
       cur_pdp->trans_id = (byte) recd_trans_id;
  
       // Send a SM-STATUS message to the network with casue "Invalid TI"
       cur_pdp->current_cause = (cc_cause_T)SM_FEATURE_NOT_SUPPORTED;
       sm_send_sm_status(cur_pdp); 
#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(req_pdp_cxt,MODEM_MEM_CLIENT_NAS);
#else
        gs_free(req_pdp_cxt);
#endif    
        req_pdp_cxt = NULL;
        return;
    }
#ifdef FEATURE_DUAL_SIM
    if ((recd_msg != REQUEST_PDP_ACTIVATION) && 
        (SM_IS_VALID_AS_ID(sm_pdu->as_id) == FALSE) )
    {

      MSG_ERROR_DS_2( SM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , sm_pdu->as_id);
      if(req_pdp_cxt != NULL)
      {
#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(req_pdp_cxt,MODEM_MEM_CLIENT_NAS);
#else
        gs_free(req_pdp_cxt);
#endif    
        req_pdp_cxt = NULL;
      }

      if(rej_resp != NULL)
      {
#ifdef FEATURE_MODEM_HEAP
         modem_mem_free(rej_resp,MODEM_MEM_CLIENT_NAS);
#else
         gs_free(rej_resp);
#endif
         rej_resp = NULL;
      }

#ifdef FEATURE_MBMS
      #error code not present
#endif

      return ;
    }
#endif /*#ifdef FEATURE_DUAL_DATA*/

    if (recd_msg == REQUEST_PDP_ACTIVATION
        #ifdef FEATURE_MBMS
        #error code not present
#endif
        || recd_msg == REQUEST_SEC_PDP_ACTIVATION
    )
    {

      if(!ti_flag)
      {
       MSG_HIGH_DS_2(SM_SUB,"Received MT PDP/MBMS context activation TI=%d, Msg=%d", recd_trans_id, recd_msg);
      }
      else
      {
        MSG_HIGH_DS_2(SM_SUB," Ignore MT/MBMS Activation Req, TIflag 1 TI=%d, Msg", recd_trans_id, recd_msg);
        if(req_pdp_cxt != NULL)
        {
  #ifdef FEATURE_MODEM_HEAP
          modem_mem_free(req_pdp_cxt,MODEM_MEM_CLIENT_NAS);
  #else
          gs_free(req_pdp_cxt);
  #endif      
          req_pdp_cxt = NULL;
        }
        if(rej_resp != NULL)
        {
#ifdef FEATURE_MODEM_HEAP
          modem_mem_free(rej_resp,MODEM_MEM_CLIENT_NAS);
#else
          gs_free(rej_resp);
#endif
          rej_resp = NULL;
        }
#ifdef FEATURE_MBMS
        #error code not present
#endif

        return; 
      }
    }

    if (!ti_found && recd_msg == SM_STATUS)
    {
      MSG_HIGH_DS_0(SM_SUB," Ignore SM STATUS for invalid TI");
      if(req_pdp_cxt != NULL)
      {
#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(req_pdp_cxt,MODEM_MEM_CLIENT_NAS);
#else
        gs_free(req_pdp_cxt);
#endif    
        req_pdp_cxt = NULL;
      }
      if(rej_resp != NULL)
      {
#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(rej_resp,MODEM_MEM_CLIENT_NAS);
#else
        gs_free(rej_resp);
#endif    
        rej_resp = NULL;
      }
#ifdef FEATURE_MBMS
      #error code not present
#endif

      return;
    }

   if (!ti_found && !( (recd_msg == REQUEST_PDP_ACTIVATION)
        #ifdef FEATURE_MBMS
        #error code not present
#endif
        || (recd_msg == REQUEST_SEC_PDP_ACTIVATION)
        ) )
    {
      // Set the transaction id to 0 and send the STATUS message
      cur_pdp = req_pdp_cxt;
      cur_pdp->trans_id = (byte) recd_trans_id;
  
      // Send a SM-STATUS message to the network with casue "Invalid TI"
      cur_pdp->current_cause = (cc_cause_T)SM_INVALID_TRANS_ID;
      sm_send_sm_status(cur_pdp); 

#ifdef FEATURE_MODEM_HEAP
      modem_mem_free(req_pdp_cxt,MODEM_MEM_CLIENT_NAS);
      modem_mem_free(rej_resp,MODEM_MEM_CLIENT_NAS);
#ifdef FEATURE_MBMS
      #error code not present
#endif
#else
      gs_free(req_pdp_cxt);
      gs_free(rej_resp);
#ifdef FEATURE_MBMS
      #error code not present
#endif
#endif  
      req_pdp_cxt = NULL;
      rej_resp = NULL;
#ifdef FEATURE_MBMS
      #error code not present
#endif

      return;
    }
    else if((recd_msg == NETWORK_MODIFY_PDP_REQUEST) && 
                (pdp_list[recd_trans_id]->pdp_state != PDP_ACTIVE &&
                 pdp_list[recd_trans_id]->pdp_state != PDP_MODIFY_PENDING))
    {
      // Received PDP accept message in wrong state, Ignore
      MSG_ERROR_DS_1( SM_SUB,"Received REQ_PDP_MODIFY in wrong state %d",pdp_list[recd_trans_id]->pdp_state);

      /* --------------------------------------------------------------------
      ** If the Mobile receives a message not compatible with the protocol
      ** state, the MS shall ignore the message if there no RR connection,
      ** otherwise the UE returns a SM-STATUS message with proper cause.
      ** Refer to section 8.4 of TS 24.008 for more details.
      ** ------------------------------------------------------------------*/
      req_pdp_cxt->trans_id = (byte)recd_trans_id;
      req_pdp_cxt->current_cause = (cc_cause_T)SM_MESG_TYPE_NOT_COMPATIBLE;
      sm_send_sm_status(req_pdp_cxt);
#ifdef FEATURE_MODEM_HEAP
      modem_mem_free(req_pdp_cxt,MODEM_MEM_CLIENT_NAS);
      modem_mem_free(rej_resp,MODEM_MEM_CLIENT_NAS);
#ifdef FEATURE_MBMS
      #error code not present
#endif
#else
      gs_free(req_pdp_cxt);
      gs_free(rej_resp);
#ifdef FEATURE_MBMS
      #error code not present
#endif
#endif   
      req_pdp_cxt = NULL;
      rej_resp = NULL;
#ifdef FEATURE_MBMS
      #error code not present
#endif

      return;
    }
    else
    {
      
      MSG_HIGH_DS_1(SM_SUB,"Received Layer 3 message with TI %d", recd_trans_id);
      i = recd_trans_id;    
/*
      cur_pdp = pdp_list[i];

      if (cur_pdp->trans_id == INVALID_ID)
      {
        cur_pdp->trans_id = (byte)recd_trans_id;
      }   
  */
      // Get the message length from the SM-PDU.
      msg_len = sm_pdu->message_len;
  
      // Decode the received L3 message received from the Network
      if (recd_msg == REQUEST_PDP_ACTIVATION
        #ifdef FEATURE_MBMS
        #error code not present
#endif
        )
      {
        /*If Recd Message is REQUEST PDP ACTIVATION then 
          store the context info into temporary object*/
        req_pdp_cxt->trans_id = (byte)recd_trans_id;
        status = unpack_layer3_mesg(msg_len, sm_pdu->data, req_pdp_cxt, &recd_msg);
      }
      else if (recd_msg == ACTIVATE_PDP_ACCEPT) // similar check is not required for ACTIVATE_MBMS_ACCEPT, coz no need for comparing address
      {
        memset((void *)req_pdp_cxt, 0, sizeof(pdp_context_info));
        req_pdp_cxt->trans_id = (byte)recd_trans_id;
        cur_pdp = pdp_list[i];
        status = unpack_layer3_mesg(msg_len, sm_pdu->data, req_pdp_cxt, &recd_msg);
      }     
      else if (recd_msg == DEACTIVATE_PDP_REQUEST) 
      {
        cur_pdp = pdp_list[recd_trans_id] ;
        if (pdp_list[i]->pdp_state == PDP_ACTIVE_PENDING) 
        {
          // Dont decode the NW message. Decoding is resulting in overwriting PCO which is used for activation.
          // anyways STATUS message will be sent to the NW and UE continues with Activation procedure
          // For other states, handling is done is sm_proc_request_pdp_deactivation function.
        }
        else
        {
          status = unpack_layer3_mesg(msg_len, sm_pdu->data, cur_pdp, &recd_msg);
        }
      }     
#ifdef FEATURE_GSM_GPRS      
      /*If Recd message is NETWORK_MODIFY_PDP_REQUEST and LLC is still not activte
           store PDU*/
      else if(recd_msg == NETWORK_MODIFY_PDP_REQUEST && is_gsm_mode() && 
                 pdp_list[recd_trans_id]->llc_state == LLC_MO_ACTIVATE_CNF_PENDING)
      {
        if(pending_sm_pdu_id== INVALID_ID || pending_sm_pdu_id == recd_trans_id)
        {
          MSG_HIGH_DS_0(SM_SUB,"Recived NW Modify PDP Request in LLC State ACTIVATE_CNF_PENDING");

#ifdef FEATURE_MODEM_HEAP 
          pending_sm_pdu.data = modem_mem_calloc(1,sm_pdu->message_len,MODEM_MEM_CLIENT_NAS);
#else
          pending_sm_pdu.data = (uint8*) gs_alloc(sm_pdu->message_len);
#endif
          sm_check_for_null_ptr((void*)pending_sm_pdu.data );
          pending_sm_pdu.message_len= sm_pdu->message_len; 
          memscpy(pending_sm_pdu.data,pending_sm_pdu.message_len,
                  sm_pdu ->data, sm_pdu->message_len);

          pending_sm_pdu_id = (byte)recd_trans_id;
        }
        else
        {
          MSG_HIGH_DS_1(SM_SUB,"Can not process NETWORK_MODIFY_PDP_REQUEST alerady pending message for other TI %d",
                             pending_sm_pdu_id);           
        }
        if(req_pdp_cxt != NULL)
        {
  #ifdef FEATURE_MODEM_HEAP
          modem_mem_free(req_pdp_cxt,MODEM_MEM_CLIENT_NAS);
  #else
          gs_free(req_pdp_cxt);
  #endif      
          req_pdp_cxt = NULL;           
        }
        if(rej_resp != NULL)
        {
#ifdef FEATURE_MODEM_HEAP
          modem_mem_free(rej_resp,MODEM_MEM_CLIENT_NAS);
#else
          gs_free(rej_resp);
#endif
          rej_resp = NULL;
        }
#ifdef FEATURE_MBMS
        #error code not present
#endif


        return;
      }
#endif      
      else
      {
        if (recd_msg == REQUEST_SEC_PDP_ACTIVATION)
        {
          (void) sm_get_connection_id_int((byte) recd_trans_id);
        }
        cur_pdp = pdp_list[recd_trans_id] ;
        status = unpack_layer3_mesg(msg_len, sm_pdu->data, cur_pdp, &recd_msg);
      }      

#ifdef FEATURE_GSM_GPRS      
      /* If a message is received from NW and there is alerady pending message 
      for the PDP context clear pending message*/
      if(pending_sm_pdu_id == recd_trans_id)
      {
        pending_sm_pdu_id = INVALID_ID;            
      }
 #endif       
 
      if ((status == L3_PARSE_SUCCESS) && (recd_msg == REQUEST_PDP_ACTIVATION || cur_pdp != NULL))
      {
        // Convert the received Transaction Id to corresponding NSAPI value
        //cur_pdp->nsapi.nsapi = GET_NSAPI(cur_pdp->trans_id);
  
        switch (recd_msg)
        {
          case ACTIVATE_PDP_ACCEPT:
            MSG_HIGH_DS_0( SM_SUB,"Received ACTIVATE_PDP_ACCEPT");        
            if ((!(sm_compare_pdpaddr ( &(req_pdp_cxt->pdp_addr) , &cur_pdp->pdp_addr))) && (cur_pdp->pdp_state == PDP_ACTIVE))
            {
            
              /* --------------------------------------------------------------------
              ** If the Mobile receives a message not compatible with the protocol
              ** state, the MS shall ignore the message if there no RR connection,
              ** otherwise the UE returns a SM-STATUS message with proper cause.
              ** Refer 24.008 section 8.4
              ** ------------------------------------------------------------------*/
              cur_pdp->current_cause = (cc_cause_T)SM_MESG_TYPE_NOT_COMPATIBLE;
              sm_send_sm_status(cur_pdp);
            
              /* --------------------------------------------------------------------
              ** A Repeat PDP Activate accept message has been received, perhaps due
              ** to overlapped transactions. The Data Services layers have been
              ** configured already with the PDP Address of the first message, but
              ** the network will likely be using the PDP Address from this message.
              ** Data Services and windows dialup networking are not able to
              ** renegotiate the PDP Address, and so the only option is to locally
              ** deactivate this context.
              ** ------------------------------------------------------------------*/
              
                MSG_MED_DS_1( SM_SUB,"Changing PDP = %d state to PDP_INACTIVE",cur_pdp->trans_id);
                cur_pdp->pdp_state = PDP_INACTIVE;
                //Reset the PDP con state
                cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;

                cur_pdp->current_cause = (cc_cause_T)SM_INTERNAL_CAUSE;

                // Clear the PDP_ACTIVATE_PENDING timer
                sm_stop_timer(cur_pdp->trans_id);

                MSG_HIGH_DS_1(SM_SUB,"Locally Deactivating PDP = %d",cur_pdp->trans_id);

#ifdef FEATURE_GSM_GPRS
                if ( cur_pdp->llc_state != LLC_NOT_CONFIRMED)
                {
                  /* Set the llc_state to LLC_CONFIRM_PENDING, start RABM insanity timer */
                  cur_pdp->llc_state = LLC_MT_DEACTIVATE_CNF_PENDING;
                  sm_send_sndcp_cmd (cur_pdp, GSNSM_DEACTIVATE_IND);
                }
                else
#endif                
                {
                  // Send the SMREG_PDP_DEACTIVATE_IND to the CM for Mobile terminated req.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
                  if ( !cur_pdp->teardown_ind.ti_flag 
                       &&cur_pdp->is_primary_pdp 
                       &&sm_promote_sec_pri(cur_pdp))
                  {
                     sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_PROMOTE_IND, SM_INTERNAL_CAUSE);
                  }
                  else
                  {
#endif
                     sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);

#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
                  }
#endif
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
                }              
            }
            else
            {
              if (req_pdp_cxt->pdp_addr.valid == TRUE)
              {
                cur_pdp->pdp_addr = req_pdp_cxt->pdp_addr;
              }
              cur_pdp->llc_sapi = req_pdp_cxt->llc_sapi;
              cur_pdp->qos = req_pdp_cxt->qos;
              cur_pdp->radio_pri = req_pdp_cxt->radio_pri;
              if(cur_pdp->config_options.valid != TRUE)
              {
                 sm_set_bcm_in_pco(req_pdp_cxt, BCM_MS_ONLY);
              }
              else
              {
                boolean is_bcm_in_req_PCO_present = FALSE;
                for (i = 0; i < cur_pdp->config_options.num_protocol_ids; i++)
                {
                   if (cur_pdp->config_options.procol_info[i].protocol_id[0] == 0x00 && 
                       cur_pdp->config_options.procol_info[i].protocol_id[1] == 0x05
                      )
                    {
                      is_bcm_in_req_PCO_present = TRUE;
                      break;
                    }
                 }
                 if(is_bcm_in_req_PCO_present == FALSE)
                 {                   
                    sm_set_bcm_in_pco(req_pdp_cxt, BCM_MS_ONLY);
                 }
               } 
              cur_pdp->config_options = req_pdp_cxt->config_options;
              if (req_pdp_cxt->flow_id.valid == TRUE)
              {
                cur_pdp->flow_id = req_pdp_cxt->flow_id;
              }
#ifdef FEATURE_DUAL_IP_BEARER
              cur_pdp->current_cause = req_pdp_cxt->current_cause;
#endif
              sm_proc_activate_pdp_accept(cur_pdp);
            }
            break;
  
          case ACTIVATE_PDP_REJECT:
            
            sm_proc_activate_pdp_reject(cur_pdp);
            break;
  
          case REQUEST_PDP_ACTIVATION:
           #ifdef FEATURE_DUAL_SIM
           if(SM_IS_VALID_AS_ID(sm_pdu->as_id) == FALSE)
           { 
             sm_as_id = sm_pdu->as_id ;
     	   }
           #endif

#ifdef FEATURE_DISABLE_MT_PDP
            MSG_HIGH_DS_0( SM_SUB,"FEATURE_DISABLE_MT_PDP Active");
            (void) sm_get_connection_id_int(recd_trans_id);
            cur_pdp = pdp_list[recd_trans_id] ;

            rej_resp->cause = SM_FEATURE_NOT_SUPPORTED;
            rej_resp->config_options.valid = FALSE;            
#ifdef FEATURE_DUAL_SIM
            rej_resp->as_id = sm_as_id;
#endif
            if (cur_pdp == NULL)
            {
                MSG_ERROR_DS_1( SM_SUB,"PDP is NULL, Connection id %d is unreserved", recd_trans_id);
            }
            else
            {
                //set the orig type to MT
                cur_pdp->mt_orig_type = TRUE;
                cur_pdp->config_options.valid = FALSE;
                cur_pdp->is_primary_pdp = TRUE;
                sm_pdp_activate_rej_rsp(cur_pdp->trans_id, rej_resp);
            }
#else
            if (sm_validate_dupliacte_collision_pdp(req_pdp_cxt))
            {
              (void) sm_get_connection_id_int((byte) recd_trans_id);
              cur_pdp = pdp_list[recd_trans_id] ;
              if (cur_pdp == NULL)
              {
                MSG_ERROR_DS_1( SM_SUB,"PDP is NULL, Connection id %d is unreserved", recd_trans_id);
              }
              else
              {
                 /*Copy the PDP address*/
                cur_pdp->apn_name = req_pdp_cxt->apn_name;
   
                 /* Copy the PDP address */
                cur_pdp->pdp_addr = req_pdp_cxt->pdp_addr;
   
                 /*Copy the Allocation type*/
                cur_pdp->pdp_addr_alloc_type = (req_pdp_cxt->pdp_addr_alloc_type);
   
                 /*Copy the Protocol configuration option*/
                cur_pdp->config_options = req_pdp_cxt->config_options;
   
                sm_proc_request_pdp_activation(cur_pdp);
              }
            }
#endif /*FEATURE_DISABLE_MT_PDP*/
            break;
  
          case DEACTIVATE_PDP_REQUEST:
            
            sm_proc_request_pdp_deactivation(cur_pdp);
            break;
  
          case DEACTIVATE_PDP_ACCEPT:
            
            sm_proc_deactivate_pdp_accept(cur_pdp);
            break;
  
          case NETWORK_MODIFY_PDP_REQUEST:
            
            sm_proc_network_modify_pdp_request(cur_pdp);
            break;
  
          case MODIFY_PDP_ACCEPT:
            
            sm_proc_ms_modify_pdp_accept(cur_pdp);
            break;      
  
          case MODIFY_PDP_REJECT:
            
            sm_proc_modify_pdp_reject(cur_pdp);
            break;
  
          case ACTIVATE_SEC_PDP_ACCEPT:
            
            sm_proc_activate_sec_pdp_accept(cur_pdp);
            break;
  
          case ACTIVATE_SEC_PDP_REJECT:
            
            sm_proc_activate_sec_pdp_reject(cur_pdp);
            break;

          case SM_STATUS:
            
            sm_proc_sm_status(cur_pdp);
            break;
            
          case REQUEST_SEC_PDP_ACTIVATION:
             
             sm_proc_request_sec_pdp_activation(cur_pdp);
              break;

#ifdef FEATURE_MBMS
          #error code not present
#endif

          default:
            MSG_ERROR_DS_1( SM_SUB,"Unknown Msg Received, Ignore it : %d", recd_msg); 
            break;
        }
      }
      else
      {
          if (pdp_list[recd_trans_id] == NULL)
          {
            (void) sm_get_connection_id_int((byte) recd_trans_id);
            cur_pdp = pdp_list[recd_trans_id] ;
            mem_allocated = TRUE;
          }
          if (cur_pdp == NULL)
          {
            MSG_ERROR_DS_1( SM_SUB,"PDP is NULL, Connection id %d is unreserved", recd_trans_id);
          }
          else
          {
             /* --------------------------------------------------------------------
             ** If the Network sent REQUEST_PDP_CONEXT_ACTIVATION, a reject msg
             ** REQUEST_PDP_CONTEXT_REJECT is sent to the network. Refer to section
             ** 8.5.5 in TS 24.008 for more details.
             ** -------------------------------------------------------------------*/
             cur_pdp->trans_id = (byte) recd_trans_id;
             if (status == L3_UNSUPPORTED_MSG )
             {
              MSG_HIGH_DS_1(SM_SUB,"Invalid MSG type for ti %d", cur_pdp->trans_id);
               cur_pdp->current_cause = (cc_cause_T)SM_MESG_TYPE_NON_EXISTENT;
               status_msg_sent = TRUE;
               sm_send_sm_status(cur_pdp);
             }
             else if (recd_msg == REQUEST_PDP_ACTIVATION)
             {
               MSG_HIGH_DS_1(SM_SUB,"Invalid mandatory info in REQUEST_PDP_ACTIVATION for ti %d",
                          cur_pdp->trans_id);
               rej_resp->cause = SM_INVALID_MANDATORY_INFO;
               //set the orig type to MT
               cur_pdp->mt_orig_type = TRUE;
               sm_pdp_activate_rej_rsp(cur_pdp->trans_id, rej_resp);
             }
            #ifdef FEATURE_MBMS
             #error code not present
#endif
             else if (status == L3_UNSUPPORTED_IE)
             {
                   byte index;
                   cur_pdp->mt_orig_type = TRUE;
                   cur_pdp->rab_state = RABM_INITIAL;
                   sig_connection_available = FALSE;
     
                   if (cur_pdp->pdp_state == PDP_ACTIVE) // MBMS_PENDING should add the similar code for MBMS_ACTIVE
                   {
                       cur_pdp->current_cause = (cc_cause_T)SM_FEATURE_NOT_SUPPORTED;
                       cur_pdp->cm_pending_msg = (int)SMREG_PDP_DEACTIVATE_REQ;
                       cur_pdp->teardown_ind.ti_flag = TRUE;
                       cur_pdp->teardown_ind.valid = TRUE;
                       
                       /*Stop the timer in case any timer is running*/
                      sm_stop_timer(cur_pdp->trans_id);

                      for (index = 0; index < MAX_PDP_NUM; index++)
                      {
                        if ( pdp_list[index] != NULL && 
                             (index != cur_pdp->trans_id) &&
                             (cur_pdp->group_id == pdp_list[index]->group_id) &&
                             (pdp_list[index]->pdp_state != PDP_INACTIVE)
                           )
                        {
                          //If the profile matches change the state to PDP_INACTIVE_PENDING
                          MSG_HIGH_DS_1(SM_SUB, "PDP state changed to INACTIVE_PENDING %d",pdp_list[index]->nsapi.nsapi);
                          pdp_list[index]->mt_orig_type = TRUE;
                          pdp_list[index]->pdp_state = PDP_INACTIVE_PENDING;
                          //Change the RAB state to RAB_INTIAL
                          pdp_list[index]->rab_state = RABM_INITIAL;
                          /* set the current pending CM message */
                          pdp_list[index]->cm_pending_msg = (int)SMREG_PDP_DEACTIVATE_REQ;
                          #ifdef FEATURE_MBMS
                          #error code not present
#endif
                          if (is_umts_mode())
                          {
                            // Inform the RABM that the PDP context is being deactivated
                            MSG_MED_DS_1(SM_SUB, "Inform RABM PDP Nsapi %d deactivated",
                                                 pdp_list[index]->nsapi.nsapi);
                            pdp_list[index]->rab_state = RABM_INACTIVE_PENDING;
                            sm_notify_rabm ((int)RABMSM_DEACTIVATE_IND, pdp_list[index]->nsapi.nsapi);
                          }
                        }
                        /* If SM is waiting for secondary PDP activaiton request form CM for 
                             MT sec PDP activation*/
                        else
                        if( pdp_list[index] != NULL && 
                            (index != cur_pdp->trans_id) &&
                            (cur_pdp->group_id == pdp_list[index]->group_id) &&
                            (pdp_list[index]->mt_orig_type == TRUE)
                          )
                        {
                          if (cur_pdp->nsapi.valid == TRUE)
                          (void)sm_release_nsapi_sm (cur_pdp->trans_id);

                          (void) sm_unreserve_connection_id_int(cur_pdp->trans_id);
                        }
                      }
     
                      MSG_HIGH_DS_1(SM_SUB,"PDP De-Activate Req for NSAPI = %d", cur_pdp->nsapi.nsapi);
     
                       /* ----------------------------------------------------------------------
                       ** Call sm_continue_pdp_deactivate function, this function will send the
                       ** Signaling message to GMM if the Signaling connection is available. 
                       ** Otherwise an ESTABLISH_REQ is sent to GMM for a signaling connection.
                       ** ---------------------------------------------------------------------*/
    #ifdef FEATURE_MBMS
                       #error code not present
#endif
                       sm_continue_pdp_deactivate(cur_pdp);
                   }
             }
             else
             {
               if (recd_msg == REQUEST_SEC_PDP_ACTIVATION && 
                  ( !(cur_pdp->tft.operation_code == 1 || cur_pdp->tft.operation_code == 6) ) )
               {
                  MSG_HIGH_DS_2(SM_SUB, "Received REQUEST_SEC_PDP_ACTIVATION message with OP Code other than CREATE TI: %d, OP: %d", 
                      cur_pdp->trans_id, cur_pdp->tft.operation_code);
                  cur_pdp->config_options.valid = FALSE;
                  cur_pdp->current_cause = (cc_cause_T)SM_SEMANTIC_ERR_IN_TFT;
                  sm_continue_pdp_activate_rej_rsp (cur_pdp);
               }
	       else if (recd_msg == REQUEST_SEC_PDP_ACTIVATION && 
                  (cur_pdp->tft.operation_code == 6 && cur_pdp->current_cause != (cc_cause_T)PPD_UNKNOWN_REASON) )
               {
                  MSG_HIGH_DS_2(SM_SUB, "Received REQUEST_SEC_PDP_ACTIVATION message with OP Code : %d and non-empty list for TI: %d",  cur_pdp->tft.operation_code,cur_pdp->trans_id);
                  cur_pdp->config_options.valid = FALSE;
                  cur_pdp->current_cause = (cc_cause_T)SM_SYNTACTIC_ERR_IN_TFT;
                  sm_continue_pdp_activate_rej_rsp (cur_pdp);
               }
               else
               {
  
                  MSG_HIGH_DS_1(SM_SUB,"Invalid mandatory info for ti %d", cur_pdp->trans_id);
                 /* Pack the SM status Message and send it to GMM */
                 cur_pdp->current_cause = (cc_cause_T)SM_INVALID_MANDATORY_INFO;
                 status_msg_sent = TRUE;
                 sm_send_sm_status(cur_pdp);
               }
             }
          }
         if (mem_allocated == TRUE && status_msg_sent == TRUE)
         {
            (void) sm_unreserve_connection_id_int((byte) recd_trans_id);
         }
      }
    }
  }
  else
  {
    // Invalid PD or Extended TI proposed in the Network message.
    MSG_ERROR_DS_0(SM_SUB,"Invalid PD or Extended TI received in the message");

    // Notify the Network that extended transaction ID is not yet supported by MS
    sm_send_sm_status_on_rx_extended_tid (recd_trans_id);
  } 
    
  if(req_pdp_cxt != NULL)
  {
#ifdef FEATURE_MODEM_HEAP
    modem_mem_free(req_pdp_cxt,MODEM_MEM_CLIENT_NAS);
#else
    gs_free(req_pdp_cxt);
#endif    
    req_pdp_cxt = NULL;
  } 
    
  if(rej_resp != NULL)
  {
#ifdef FEATURE_MODEM_HEAP
    modem_mem_free(rej_resp,MODEM_MEM_CLIENT_NAS);
#else
    gs_free(rej_resp);
#endif    
    rej_resp = NULL;
  }

#ifdef FEATURE_MBMS
  #error code not present
#endif

   
}
/*===========================================================================
FUNCTION SM_PROCESS_PDP_STATUS 

DESCRIPTION
  This function processes the PDP CONTEXT STATUS IE from GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_process_pdp_status(gmmsm_pdp_mbms_context_status_T* gmm_pdp_mbms_context_status)
{
    word i;
    byte nsapi_map;
    /* -----------------------------------------------------------------
    ** RAU ACCEPT or SERVICE ACCEPT has been recieved with  
    ** PDP CONTEXT STATUS IE. 3GPP CR N1-010325
    ** ----------------------------------------------------------------*/
    MSG_HIGH_DS_0(SM_SUB," PDP CONTEXT STATUS has been recieved");
    
    for (i=MIN_NSAPI_NUM; i<MAX_NSAPI_NUM; i++) // have to change the logic later
    {
        nsapi_map=1;
        if(i>7)
        {
            nsapi_map=(byte)(nsapi_map<<(i-8)); // MBMS_PENDING, this is to be changed according to handle nsapi upto 254
        }
        else
        {
            nsapi_map=(byte)(nsapi_map<<i);
        }
        if ( (nsapi_list[i] != INVALID_ID) && 
              pdp_list[nsapi_list[i]] != NULL &&
             (pdp_list[nsapi_list[i]]->pdp_state != PDP_ACTIVE_PENDING))  //  used later
        {
            if(i>7)/*Checking for NSAPI 8 to 15 in octet 4 of PDP Context Status IE*/
            {
                if(((gmm_pdp_mbms_context_status->value[1])&(nsapi_map))==0)
                {
                    sm_perform_local_pdp_deactivation(pdp_list[nsapi_list[i]]);
                }
            }
            else/*Checking for NSAPI 5,6,7 in octet 3 of PDP Context Status IE*/
            {
                if(((gmm_pdp_mbms_context_status->value[0])&(nsapi_map))==0)
                {
                    sm_perform_local_pdp_deactivation(pdp_list[nsapi_list[i]]);
                }
            }   
        }
    }
    
}
    
/*===========================================================================
FUNCTION SM_PROC_ACTIVATE_PDP_ACCEPT 

DESCRIPTION
  This function processes the Accept message sent by the Network for a
  Mobile originated PDP context activation request. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/         
void sm_proc_activate_pdp_accept(pdp_context_info *cur_pdp)
{
  if (cur_pdp->pdp_state == PDP_ACTIVE_PENDING)
  {
    // Change the state of the PDP context
    MSG_MED_DS_0( SM_SUB,"Changing PDP state to PDP_ACTIVE");
    cur_pdp->pdp_state = PDP_ACTIVE;

    // Stop the Timer T3380 and clear the Timer signal
    sm_stop_timer (cur_pdp->trans_id);

    /* ----------------------------------------------------------------------
    ** Validate the network offered QoS against the minimum QoS requirements
    ** of  the  UE. If  the  validation fails, initiate the PDP deactivation 
    ** procedures.
    ** ----------------------------------------------------------------------*/
    if (!sm_validate_qos(cur_pdp->qos, cur_pdp->minimum_qos))
    {
      MSG_ERROR_DS_1( SM_SUB,"QoS valdn fail on NSAPI = %d", cur_pdp->nsapi.nsapi);
      sm_handle_qos_validation_failure ( cur_pdp );
      return;
    }
   
  //Extracting BCM from PCO  
  cur_pdp->bcm = sm_get_bcm_from_pco(&cur_pdp->config_options);  

#ifdef FEATURE_GSM_GPRS
    /* ------------------------------------------------------------------
    ** Check if the UE is running in GPRS mode of operation and send
    ** GSNSM_ACTIVATE_IND to the SNDCP.
    ** -----------------------------------------------------------------*/
    if (is_gsm_mode())
    {
      sm_send_sndcp_cmd (cur_pdp, GSNSM_ACTIVATE_IND);  

      /* Set the llc_state to LLC_CONFIRM_PENDING, start RABM insanity timer */
      cur_pdp->llc_state = LLC_MO_ACTIVATE_CNF_PENDING;

      /* Put the connection Id into the timers_pending array */
      //timers_pending[TIMER_PENDING] = cur_pdp->trans_id;

      /* ---------------------------------------------------------------------
      ** The PDP context has been activated, but LLC RAB has to be established
      ** before sending the SMREG_PDP_ACTIVATE_CNF primitive to CM.
      ** --------------------------------------------------------------------*/      
      MSG_HIGH_DS_1(SM_SUB,"PDP %d ACTIVATED, Waiting LLC Confirm ",cur_pdp->trans_id);
    }    

#endif /*FEATURE_GSM_GPRS*/

    if (is_umts_mode())
    {
      /* ------------------------------------------------------------------
      ** Check if the RABM has already sent a response message 
      ** RABMSM_ACTIVATE_RSP before sending the confirmation to the CM.
      ** -----------------------------------------------------------------*/
      if ((cur_pdp->rab_state == RABM_ACTIVATED) && (sm_timer_status[cur_pdp->trans_id] != (word) TIMER_EARLY_RAB_FAILURE))
      {
        // Send the SMREG_PDP_ACTIVATE_CNF to the CM.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_CNF, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      }
      else if (sm_timer_status[cur_pdp->trans_id] != (word) TIMER_EARLY_RAB_FAILURE)
      {
        // Set the rab_state to RAB_ACTIVE_PENDING and start RABM insanity timer.
        cur_pdp->rab_state = RABM_ACTIVE_PENDING;

        /* Put the connection Id into the timers_pending array */
       // timers_pending[TIMER_PENDING] = cur_pdp->trans_id;

        /* Set the RAB Response Timer and wait for the RABM to respond */
        sm_start_timer(cur_pdp->trans_id, TIMER_RABM_RESP_PENDING); 

        /* ------------------------------------------------------------------
        ** The PDP context has been activated, but RAB has to be established
        ** before sending the SMREG_PDP_ACTIVATE_CNF primitive to CM.
        ** -----------------------------------------------------------------*/      
        MSG_HIGH_DS_1(SM_SUB," PDP %d ACTIVATED, Waiting for RAB to connect",
                   cur_pdp->trans_id);
      }
    }    
  }
  else
  {
  
    cc_cause_T             exisitng_cause;
    // Received PDP accept message in wrong state, Ignore
    MSG_ERROR_DS_1(SM_SUB,"Received ACTIVATE_PDP_ACCEPT in wrong state %d",cur_pdp->pdp_state);

    /* Store the cuase in a temporary variable to reatain the value before sending SM STATUS;
       Cuase is overwritten while sending SM STATUS message below
       Cause in PDP ACTIVE CNF is needed for DS incase of IPv4v6 where NW might include Cause in PDP ACCEPT
    */
    exisitng_cause = cur_pdp->current_cause;
    /* --------------------------------------------------------------------
    ** If the Mobile receives a message not compatible with the protocol
    ** state, the MS shall ignore the message if there no RR connection,
    ** otherwise the UE returns a SM-STATUS message with proper cause.
    ** Refer to section 8.4 of TS 24.008 for more details.
    ** ------------------------------------------------------------------*/
    cur_pdp->current_cause = (cc_cause_T)SM_MESG_TYPE_NOT_COMPATIBLE;
    sm_send_sm_status(cur_pdp);
    cur_pdp->current_cause = exisitng_cause;


  }
}

/*===========================================================================
FUNCTION SM_PROC_ACTIVATE_PDP_REJECT 

DESCRIPTION
  This function processes the Network sent Reject message for a Mobile 
  Originated PDP context activate request. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_proc_activate_pdp_reject(pdp_context_info *cur_pdp)
{

  
 

  sm_log_pdp_request_reject_event(FALSE,(int)SMGMM_EVENT_PDP_ACTIVATE_REJECT,cur_pdp->current_cause);
   
  if (cur_pdp->pdp_state == PDP_ACTIVE_PENDING)
  {
    // Change the state of the PDP context back to INACTIVE.
    MSG_MED_DS_1( SM_SUB,"Changing PDP %d state to PDP_INACTIVE",cur_pdp->trans_id);
    cur_pdp->pdp_state = PDP_INACTIVE;
    //Reset the PDP con state
    cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;

    // Clear the PDP_ACTIVATE_PENDING timer
    sm_stop_timer (cur_pdp->trans_id);
    
    if (is_umts_mode())
    {
       sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, cur_pdp->nsapi.nsapi);
    }    
    // Send the SMREG_PDP_ACTIVATE_REJ to the CM.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, SM_NETWORK_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
  else
  {
    // Received PDP accept message in wrong state, Ignore
    MSG_ERROR_DS_1(SM_SUB,"Received ACTIVATE_PDP_REJECT in wrong state %d",cur_pdp->pdp_state);

    /* --------------------------------------------------------------------
    ** If the Mobile receives a message not compatible with the protocol
    ** state, the MS shall ignore the message if there no RR connection,
    ** otherwise the UE returns a SM-STATUS message with proper cause.
    ** Refer to section 8.4 of TS 24.008 for more details.
    ** ------------------------------------------------------------------*/
    cur_pdp->current_cause = (cc_cause_T)SM_MESG_TYPE_NOT_COMPATIBLE;
    sm_send_sm_status(cur_pdp);
  }
}

/*===========================================================================
FUNCTION SM_PROC_ACTIVATE_SEC_PDP_ACCEPT 

DESCRIPTION
  This function processes the received ACTIVATE_SEC_PDP_ACCEPT msg 
  from the network.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_proc_activate_sec_pdp_accept(pdp_context_info *cur_pdp)
{
  // Received PDP accept message in wrong state, Ignore
  MSG_HIGH_DS_1(SM_SUB,"Recvd PDP ACTIVATE SEC ACCEPT for %d", cur_pdp->trans_id);
  if (cur_pdp->pdp_state == PDP_ACTIVE_PENDING)
  {
    // Change the state of the PDP context
    MSG_MED_DS_1( SM_SUB,"Changing SEC PDP state to PDP_ACTIVE %d",cur_pdp->trans_id);
    cur_pdp->pdp_state = PDP_ACTIVE;

    // Stop the Timer T3380 and clear the Timer signal
    sm_stop_timer (cur_pdp->trans_id);

    
    /* ----------------------------------------------------------------------
    ** Validate the network offered QoS against the minimum QoS requirements
    ** of  the  UE. If  the  validation fails, initiate the PDP deactivation 
    ** procedures.
    ** ----------------------------------------------------------------------*/
    if (!sm_validate_qos(cur_pdp->qos, cur_pdp->minimum_qos))
    {
      MSG_ERROR_DS_1( SM_SUB,"QoS valdn fail on NSAPI = %d", cur_pdp->nsapi.nsapi);
      sm_handle_qos_validation_failure ( cur_pdp );
      return;
    }

#ifdef FEATURE_GSM_GPRS
    /* ------------------------------------------------------------------
    ** Check if the UE is running in GPRS mode of operation and send
    ** GSNSM_ACTIVATE_IND to the SNDCP.
    ** -----------------------------------------------------------------*/
    if (is_gsm_mode())
    {
      sm_send_sndcp_cmd (cur_pdp, GSNSM_ACTIVATE_IND);  

      /* Set the llc_state to LLC_CONFIRM_PENDING, start RABM insanity timer */
      cur_pdp->llc_state = LLC_MO_ACTIVATE_CNF_PENDING;

      /* Put the connection Id into the timers_pending array */
      //timers_pending[TIMER_PENDING] = cur_pdp->trans_id;

      /* ---------------------------------------------------------------------
      ** The PDP context has been activated, but LLC RAB has to be established
      ** before sending the SMREG_PDP_ACTIVATE_CNF primitive to CM.
      ** --------------------------------------------------------------------*/      
      MSG_HIGH_DS_1(SM_SUB,"PDP %d ACTIVATED, Waiting LLC Confirm ",cur_pdp->trans_id);
    }    

#endif /*FEATURE_GSM_GPRS*/

    if (is_umts_mode())
    {
      /* ------------------------------------------------------------------
      ** Check if the RABM has already sent a response message 
      ** RABMSM_ACTIVATE_RSP before sending the confirmation to the CM.
      ** -----------------------------------------------------------------*/
      if ((cur_pdp->rab_state == RABM_ACTIVATED) && (sm_timer_status[cur_pdp->trans_id] != (word) TIMER_EARLY_RAB_FAILURE))
      {
        // Send the SMREG_PDP_ACTIVATE_CNF to the CM.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_CNF, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      }
      else if (sm_timer_status[cur_pdp->trans_id] != (word) TIMER_EARLY_RAB_FAILURE)
      {
        // Set the rab_state to RAB_ACTIVE_PENDING and start RABM insanity timer.
        cur_pdp->rab_state = RABM_ACTIVE_PENDING;

        /* Put the connection Id into the timers_pending array */
       // timers_pending[TIMER_PENDING] = cur_pdp->trans_id;

        /* Set the RAB Response Timer and wait for the RABM to respond */
        sm_start_timer(cur_pdp->trans_id, TIMER_RABM_RESP_PENDING); 

        /* ------------------------------------------------------------------
        ** The PDP context has been activated, but RAB has to be established
        ** before sending the SMREG_PDP_ACTIVATE_CNF primitive to CM.
        ** -----------------------------------------------------------------*/      
        MSG_HIGH_DS_1(SM_SUB," SEC PDP %d ACTIVATED, Waiting for RAB to connect",
                   cur_pdp->trans_id);
      }
    }    
  }
  else
  {
    // Received PDP accept message in wrong state, Ignore
    MSG_ERROR_DS_1(SM_SUB,"Received SEC ACTIVATE_PDP_ACCEPT in wrong state %d",cur_pdp->pdp_state);

    /* --------------------------------------------------------------------
    ** If the Mobile receives a message not compatible with the protocol
    ** state, the MS shall ignore the message if there no RR connection,
    ** otherwise the UE returns a SM-STATUS message with proper cause.
    ** Refer to section 8.4 of TS 24.008 for more details.
    ** ------------------------------------------------------------------*/
    cur_pdp->current_cause = (cc_cause_T)SM_MESG_TYPE_NOT_COMPATIBLE;
    sm_send_sm_status(cur_pdp);

    /* --------------------------------------------------------------------
    ** A Repeat PDP Activate accept message has been received, perhaps due
    ** to overlapped transactions. The Data Services layers have been
    ** configured already with the PDP Address of the first message, but
    ** the network will likely be using the PDP Address from this message.
    ** Data Services and windows dialup networking are not able to
    ** renegotiate the PDP Address, and so the only option is to locally
    ** deactivate this context.
    ** ------------------------------------------------------------------*/
    
    if (cur_pdp->pdp_state == PDP_ACTIVE)
    {
      MSG_MED_DS_1( SM_SUB,"Changing PDP = %d state to PDP_INACTIVE",cur_pdp->trans_id);
      cur_pdp->pdp_state = PDP_INACTIVE;
      //Reset the PDP con state
      cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;

      cur_pdp->current_cause = (cc_cause_T)SM_INTERNAL_CAUSE;

      // Clear the PDP_ACTIVATE_PENDING timer
      sm_stop_timer (cur_pdp->trans_id);

      MSG_HIGH_DS_1(SM_SUB,"Locally Deactivating PDP = %d",cur_pdp->trans_id);

#ifdef FEATURE_GSM_GPRS
      if (cur_pdp->llc_state != LLC_NOT_CONFIRMED)
      {
        /* Set the llc_state to LLC_CONFIRM_PENDING, start RABM insanity timer */
        cur_pdp->llc_state = LLC_MT_DEACTIVATE_CNF_PENDING;
        sm_send_sndcp_cmd (cur_pdp, GSNSM_DEACTIVATE_IND);
      }
      else
#endif
      {
        // Send the SMREG_PDP_DEACTIVATE_IND to the CM 
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
        if ( !cur_pdp->teardown_ind.ti_flag 
            &&cur_pdp->is_primary_pdp 
            &&sm_promote_sec_pri(cur_pdp))
        {
           sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_PROMOTE_IND,
                                                     SM_INTERNAL_CAUSE);
        }
        else
        {
#endif
           sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, 
                                              SM_INTERNAL_CAUSE);
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
        }
#endif
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      }
    }
  }

}


/*===========================================================================
FUNCTION SM_PROC_ACTIVATE_SEC_PDP_REJECT 

DESCRIPTION
  This function processes the received ACTIVATE_SEC_PDP_ACCEPT msg 
  from the network.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_proc_activate_sec_pdp_reject(pdp_context_info *cur_pdp)
{

 MSG_HIGH_DS_2(SM_SUB,"Rcvd SEC PDP Reject for %d, Cause = %d",cur_pdp->trans_id, 
                                           cur_pdp->current_cause);
 
  if (cur_pdp->pdp_state == PDP_ACTIVE_PENDING)
  {
    // Change the state of the PDP context back to INACTIVE.
    MSG_MED_DS_1( SM_SUB,"Changing SEC PDP %d state to PDP_INACTIVE",cur_pdp->trans_id);
    cur_pdp->pdp_state = PDP_INACTIVE;
    //Reset the PDP con state
    cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;

    // Clear the PDP_ACTIVATE_PENDING timer
    sm_stop_timer (cur_pdp->trans_id);
    
    if (is_umts_mode())
    {
      sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, cur_pdp->nsapi.nsapi);
    }
    
    // Send the SMREG_PDP_ACTIVATE_REJ to the CM.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_REJ, SM_NETWORK_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
  else
  {
 // Received PDP accept message in wrong state, Ignore
    MSG_ERROR_DS_1(SM_SUB,"Received SEC_ACTIVATE_PDP_REJECT in wrong state %d",cur_pdp->pdp_state);

  /* --------------------------------------------------------------------
  ** If the Mobile receives a message not compatible with the protocol
  ** state, the MS shall ignore the message if there no RR connection,
  ** otherwise the UE returns a SM-STATUS message with proper cause.
  ** Refer to section 8.4 of TS 24.008 for more details.
  ** ------------------------------------------------------------------*/
   cur_pdp->current_cause = (cc_cause_T)SM_MESG_TYPE_NOT_COMPATIBLE;
   sm_send_sm_status(cur_pdp);
  }
}


/*===========================================================================
FUNCTION SM_PROC_MODIFY_PDP_REJECT 

DESCRIPTION
  This function processes the received MODIFY_PDP_REJECT msg 
  from the network.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_proc_modify_pdp_reject(pdp_context_info *cur_pdp)
{
  if (cur_pdp->pdp_state == PDP_MODIFY_PENDING)
  {
    // Change the state of the PDP context back to ACTIVE.
    MSG_MED_DS_0( SM_SUB, "Changing PDP state to PDP_ACTIVE");
    cur_pdp->pdp_state = PDP_ACTIVE;

    // Clear the PDP_ACTIVATE_PENDING timer
    sm_stop_timer (cur_pdp->trans_id);

    /*Set the requested values to invalid*/
    cur_pdp->req_qos.valid = FALSE;
    cur_pdp->req_tft.valid = FALSE;
    //cur_pdp->req_llc_sapi.valid = FALSE;

    if (cur_pdp->rab_state == RABM_MODIFY_PENDING)
    {
      //Send RABMSM_DEACTIVATE_IND to RABM
      sm_notify_rabm((int)RABMSM_DEACTIVATE_IND, cur_pdp->nsapi.nsapi);
      cur_pdp->rab_state = RABM_DEACTIVATED;
    }

    if ((cur_pdp->cm_pending_msg != (int)SMREG_PDP_MODIFY_REQ)&&
         ( cur_pdp->qos.traffic_class == (byte)CONVERSATION_CLASS || 
           cur_pdp->qos.traffic_class == (byte)STREAMING_CLASS))
    {
      cur_pdp->current_cause = (cc_cause_T)RAB_SETUP_FAILURE;
    }

    // Send the SMREG_PDP_MODIFY_REJ to the CM.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_REJ, SM_NETWORK_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
  else
  {
    // Received PDP accept message in wrong state, Ignore
    MSG_ERROR_DS_1( SM_SUB,"Received MODIFY_PDP_REJECT in wrong state %d",cur_pdp->pdp_state);

    /* --------------------------------------------------------------------
    ** If the Mobile receives a message not compatible with the protocol
    ** state, the MS shall ignore the message if there no RR connection,
    ** otherwise the UE returns a SM-STATUS message with proper cause.
    ** Refer to section 8.4 of TS 24.008 for more details.
    ** ------------------------------------------------------------------*/
    cur_pdp->current_cause = (cc_cause_T)SM_MESG_TYPE_NOT_COMPATIBLE;
    sm_send_sm_status(cur_pdp);       
  }
}

/*===========================================================================
FUNCTION SM_PROC_MODIFY_PDP_ACCEPT 

DESCRIPTION
  This function processes the received MODIFY_PDP_ACCEPT msg 
  from the network for a MO PDP modification request.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_proc_ms_modify_pdp_accept(pdp_context_info *cur_pdp)
{
  boolean check_validate_qos = TRUE;

  if (cur_pdp->pdp_state == PDP_MODIFY_PENDING)
  {
    // Stop the Timer T3381 and clear the Timer signal
    sm_stop_timer (cur_pdp->trans_id);

    // Change the state of the PDP context back to ACTIVE.
    MSG_MED_DS_0( SM_SUB, "Changing PDP state to PDP_ACTIVE");
    cur_pdp->pdp_state = PDP_ACTIVE;

    /*Copy the requested values to actual values*/

    if (cur_pdp->req_qos.valid)
    {
      cur_pdp->qos = cur_pdp->req_qos;
      /*reset the valid flag*/
      cur_pdp->req_qos.valid = FALSE;
      check_validate_qos = FALSE;
    }

    if (cur_pdp->req_tft.valid)
    {
      cur_pdp->tft = cur_pdp->req_tft;
      /*reset the valid flag*/
      cur_pdp->req_tft.valid = FALSE;
    }

    if (cur_pdp->req_minqos.valid)
    {
      cur_pdp->minimum_qos = cur_pdp->req_minqos;
      /*reset the valid flag*/
      cur_pdp->req_minqos.valid = FALSE;
    }

    if (check_validate_qos == TRUE && !sm_validate_qos(cur_pdp->qos,cur_pdp->minimum_qos))
    {
      MSG_ERROR_DS_1( SM_SUB, "QoS valdn fail on NSAPI = %d", cur_pdp->nsapi.nsapi);
      sm_handle_qos_validation_failure ( cur_pdp );
      return;
    }
    
#ifdef FEATURE_GSM_GPRS
    /* ------------------------------------------------------------------
    ** Check if the UE is running in GPRS mode of operation and send
    ** GSNSM_MODIFY_IND to the SNDCP.
    ** -----------------------------------------------------------------*/
    if (is_gsm_mode())
    {
      sm_send_sndcp_cmd (cur_pdp, GSNSM_MODIFY_IND);  
 
      /* Set the llc_state to LLC_CONFIRM_PENDING, start sanity timer */
      cur_pdp->llc_state = LLC_MO_MODIFY_CNF_PENDING;

      /* Put the connection Id into the timers_pending array */
      //timers_pending[TIMER_PENDING] = cur_pdp->trans_id;

      /* ---------------------------------------------------------------------
      ** The PDP context has been activated, but LLC RAB has to be established
      ** before sending the SMREG_PDP_MODIFY_CNF primitive to CM.
      ** --------------------------------------------------------------------*/      
      MSG_HIGH_DS_0(SM_SUB,"PDP MODIFIED, Waiting LLC Confirm");
    }     

#endif /*FEATURE_GSM_GPRS*/
  
    if (is_umts_mode())
    {
      // Send the confirmation message to CM if the RAB has been modified
      if (cur_pdp->rab_state == RABM_MODIFIED)
      {
        cur_pdp->pdp_state = PDP_ACTIVE;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
        if (cur_pdp->cm_pending_msg == (int)SMREG_PDP_MODIFY_REQ)
        {
          // Send the SMREG_PDP_ACTIVATE_CNF to the CM.
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_CNF, SM_INTERNAL_CAUSE);        
        }
        else
        {
          /* In case of modification triggered to set up RABs with traffic class 
          streaming and conversationa; send the SMREG_PDP_ACTIVATE_IND to the CM*/
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_IND, SM_INTERNAL_CAUSE);  
        }
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      }
      else
      {
        // Set the rab_state to RAB_MODIFY_PENDING and start RABM insanity timer.
        cur_pdp->rab_state = RABM_MODIFY_PENDING;

        /* Put the connection Id into the timers_pending array */
        //timers_pending[TIMER_PENDING] = cur_pdp->trans_id;

        /* Set the RAB Response Timer and wait for the RABM to respond */
        sm_start_timer(cur_pdp->trans_id, TIMER_RABM_RESP_PENDING); 

        // Wait for the RABMSM_MODIFY_RSP primitive from RABM.
        MSG_HIGH_DS_0(SM_SUB, "Waiting for RABMSM_MODIFY_RSP from RABM");
      }
    } /* end if_umts_mode() */   
  }
  else
  {
    // Received PDP accept message in wrong state, Ignore
    MSG_ERROR_DS_1( SM_SUB,"Received MODIFY_PDP_CNF in wrong state %d",cur_pdp->pdp_state);

    /* --------------------------------------------------------------------
    ** If the Mobile receives a message not compatible with the protocol
    ** state, the MS shall ignore the message if there no RR connection,
    ** otherwise the UE returns a SM-STATUS message with proper cause.
    ** Refer to section 8.4 of TS 24.008 for more details.
    ** ------------------------------------------------------------------*/
    cur_pdp->current_cause = (cc_cause_T)SM_MESG_TYPE_NOT_COMPATIBLE;
    sm_send_sm_status(cur_pdp);    
  }
}

/*===========================================================================
FUNCTION SM_PROC_DEACTIVATE_PDP_ACCEPT 

DESCRIPTION
  This function processes the received DEACTIVATE_PDP_ACCEPT msg 
  from the network for a MO PDP deactivation request.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_proc_deactivate_pdp_accept(pdp_context_info *cur_pdp)
{
  if (cur_pdp->pdp_state == PDP_INACTIVE_PENDING)
  {
    cur_pdp->pdp_state = PDP_INACTIVE;
    // Stop the Timer T3390 and clear the Timer signal
    sm_stop_timer (cur_pdp->trans_id);

    /*If PDP deactivate is due to QoS validation failure */      
    if (!cur_pdp->qos_valid )
    {
      /*------------------------------------------------------------------------------
      **RABM is to be notified for deactivation for all cases when QOS validation fails
      **SNDCP in not required to be notified when QOS validation failures occurs during 
      **primary or sec pdp accept because LLC RAB is not activte at this stage.
      --------------------------------------------------------------------------------*/
      if (is_umts_mode())
      {
        sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, cur_pdp->nsapi.nsapi);
      }
      //Now notify CM
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      if (cur_pdp->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_REQ)
      {
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, 
                             SM_INTERNAL_CAUSE);
      }
      else if (cur_pdp->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_SEC_REQ)
      {
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_REJ, 
                            SM_INTERNAL_CAUSE);        
      }
      else 
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      {
#ifdef FEATURE_GSM_GPRS      
        if (cur_pdp->llc_state != LLC_NOT_CONFIRMED)
        {
          /*Send Deactivate indication to SNDCP */
          sm_send_sndcp_cmd (cur_pdp, GSNSM_DEACTIVATE_IND);  

          /* Set the llc_state to LLC_CONFIRM_PENDING*/
          cur_pdp->llc_state = LLC_MT_DEACTIVATE_CNF_PENDING;

          /* ---------------------------------------------------------------------
          ** The PDP context has been deactivated, but LLC RAB has to be deactivated
          ** before sending the SMREG_PDP_DEACTIVATE_IND primitive to CM.
          ** --------------------------------------------------------------------*/      
          MSG_HIGH_DS_1(SM_SUB,"PDP %d DEACTIVATED, Waiting LLC Confirm ",cur_pdp->trans_id);        
        }
        else
#endif  /*FEATURE_GSM_GPRS*/              
        {
           /*If the PDP context id primary then promote 
           one secondary to primary and give promote and deactivate ind*/      
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL 
          if (cur_pdp->is_primary_pdp && sm_promote_sec_pri(cur_pdp))
          {
            //Send the SMREG_PDP_DEACTIVATE_PROMOTE_IND to the CM
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_PROMOTE_IND, SM_INTERNAL_CAUSE);
          }
          else
          {
#endif
            // Send the SMREG_PDP_DEACTIVATE_IND to the CM.
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);

#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
          }
#endif
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
        }
      }
      return;
    }
    
 
    //If Tear down indicator is on Deactivate the group
    if (cur_pdp->teardown_ind.ti_flag)
    {
      word i;
      byte group_id = cur_pdp->group_id;
      pdp_context_info *pri_pdp = NULL;
      //If TI indicator is On deactivate all RABS in group
      for (i = 0; i < MAX_PDP_NUM; i++)
      {
        if (pdp_list[i] == NULL || 
            !(pdp_list[i]->trans_id != INVALID_ID &&
             pdp_list[i]->group_id == group_id) )
          continue;

        pdp_list[i]->pdp_state = PDP_INACTIVE;
#ifdef FEATURE_GSM_GPRS
        /* ------------------------------------------------------------------
        ** Check if the UE is running in GPRS mode of operation and send
        ** GSNSM_DEACTIVATE_IND to the SNDCP.
        ** -----------------------------------------------------------------*/
        if (pdp_list[i]->llc_state != LLC_NOT_CONFIRMED)
        {
          //In case of Primary store the pointer address
          if (pdp_list[i]->is_primary_pdp)
          {
             //Store the pri_pdp address to release at end
              pri_pdp = pdp_list[i];
          }
          else
          {
            /*Send Deactivate indication to SNDCP */
            sm_send_sndcp_cmd (pdp_list[i], GSNSM_DEACTIVATE_IND);  

            /* Set the llc_state to LLC_CONFIRM_PENDING, start RABM insanity timer */
            pdp_list[i]->llc_state = LLC_MO_DEACTIVATE_CNF_PENDING;

            /* Put the connection Id into the timers_pending array */
            //timers_pending[TIMER_PENDING] = cur_pdp->trans_id;

            /* ---------------------------------------------------------------------
            ** The PDP context has been activated, but LLC RAB has to be established
            ** before sending the SMREG_PDP_DEACTIVATE_CNF primitive to CM.
            ** --------------------------------------------------------------------*/      
            MSG_HIGH_DS_1(SM_SUB,"PDP %d DEACTIVATED, Waiting LLC Confirm ",pdp_list[i]->trans_id);
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
            if(sm_abort_cnf_pending)
            {
              if(pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
              {
                sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_REJ, 
                                                   SM_INTERNAL_CAUSE);                                         
              }
              else
              {
                sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_IND, 
                                                     SM_INTERNAL_CAUSE);                                                                      
              }
            }          
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/

          }
        }     
        else
#endif /*FEATURE_GSM_GPRS*/        
        {
          //Reset the PDP con state
          pdp_list[i]->pdp_con_state = SIG_CONNECTION_NONE;
          //If TI flag is off deactivate
          if (pdp_list[i]->rab_state == RABM_DEACTIVATED || pdp_list[i]->rab_state == RABM_INITIAL
            ||(sm_abort_cnf_pending)
          )
          {
            /*If the PDP context id primary and TI indicator is switched off  then 
            promote  one secondary to primary and give promote and deactivate ind*/
        
            if (pdp_list[i]->is_primary_pdp )
            {
              //Store the pri_pdp address to release at end
              pri_pdp = pdp_list[i];
            }
            else
            {
              #ifdef FEATURE_MBMS
              #error code not present
#endif
              //Check whether the PDP is the one requested for Deactivation from CM
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
              if (pdp_list[i] == cur_pdp)
              {
                 if(cur_pdp->mt_orig_type == TRUE)
                 {
                   // Send the SMREG_PDP_DEACTIVATE_IND to the CM.
                   sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_IND, 
                                                SM_NETWORK_CAUSE);
                 }
                 else
                 {
                   // Send the SMREG_PDP_DEACTIVATE_CNF to the CM.
                    if(pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
                    {
                     // Send the SMREG_PDP_ACTIVATE_IND to the CM.
                     sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
                    }
                    else
                    {
                      // Send the SMREG_PDP_DEACTIVATE_CNF to the CM.
                      sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);
                    }
                  }
              }
              else
              {
                if(cur_pdp->mt_orig_type == TRUE)
                {
                  // Send the SMREG_PDP_DEACTIVATE_IND to the CM.
                  sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_IND, 
                                                SM_NETWORK_CAUSE);
                 }
                else
                {
                  // Send the SMREG_PDP_DEACTIVATE_IND to the CM.
                  sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_IND, 
                                                SM_INTERNAL_CAUSE);
                }

              }
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
            }
          }
          else
          {
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
            if (pri_pdp != NULL)
            {
                MSG_HIGH_DS_2(SM_SUB," Primary is going to deactivate: %d, deactivating even Secondary PDP: %d", pri_pdp->trans_id, pdp_list[i]->trans_id);
                sm_stop_timer(pdp_list[i]->trans_id);
                sm_notify_rabm ((int)RABMSM_DEACTIVATE_IND, pdp_list[i]->nsapi.nsapi);
                #if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
                if(cur_pdp->mt_orig_type == TRUE)
                {
                  sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_IND, SM_NETWORK_CAUSE);
                }
                else
                {
                  sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);
                }
                #endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
            }
            else
            {
#endif
              // Wait for the RABM to inform about RAB releases.  
              MSG_HIGH_DS_0(SM_SUB," Waiting for RABM for Release IND");
  
              /* Set the rab_state to RAB_DEACTIVATE_PENDING and 
               start RABM insanity timer.*/
              pdp_list[i]->rab_state = RABM_INACTIVE_PENDING;
  
              /* Put the connection Id into the timers_pending array */
              //timers_pending[TIMER_PENDING] = pdp_list[i].trans_id;
  
              /* Set the RAB Response Timer and wait for the RABM to respond */
              sm_start_timer(pdp_list[i]->trans_id, TIMER_RABM_RESP_PENDING); 
  
              // Wait for the RABMSM_MODIFY_RSP primitive from RABM.
              MSG_HIGH_DS_1(SM_SUB, "PDP %d DEACTIVTED, Wait for RABM",pdp_list[i]->trans_id);
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
            }
#endif
          }
        }
      }//end for

      //Now deactivate the primary context if exists
      if (pri_pdp)
      {
#ifdef FEATURE_GSM_GPRS
        /* ------------------------------------------------------------------
        ** Check if the UE is running in GPRS mode of operation and send
        ** GSNSM_DEACTIVATE_IND to the SNDCP.
        ** -----------------------------------------------------------------*/
        if (pri_pdp->llc_state != LLC_NOT_CONFIRMED)
        {
       
          /*Send Deactivate indication to SNDCP */
          sm_send_sndcp_cmd (pri_pdp, GSNSM_DEACTIVATE_IND);  

          /* Set the llc_state to LLC_CONFIRM_PENDING, start RABM insanity timer */
          pri_pdp->llc_state = LLC_MO_DEACTIVATE_CNF_PENDING;

          /* Put the connection Id into the timers_pending array */
          //timers_pending[TIMER_PENDING] = cur_pdp->trans_id;

          /* ---------------------------------------------------------------------
          ** The PDP context has been activated, but LLC RAB has to be established
          ** before sending the SMREG_PDP_DEACTIVATE_CNF primitive to CM.
          ** --------------------------------------------------------------------*/      
          MSG_HIGH_DS_1(SM_SUB,"PDP %d DEACTIVATED, Waiting LLC Confirm ",pri_pdp->trans_id);
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
          if(sm_abort_cnf_pending)
          {
            if(pri_pdp->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
            {
              sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_REJ, 
                                                 SM_INTERNAL_CAUSE);                                         
            }
            else
            {
              sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_IND, 
                                                   SM_INTERNAL_CAUSE);                                                                      
            }
          }          
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
        
        }
        else
#endif /*FEATURE_GSM_GPRS*/
        {
          if (pri_pdp->rab_state == RABM_DEACTIVATED || pri_pdp->rab_state == RABM_INITIAL
              ||(sm_abort_cnf_pending)
             )
          {
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
            if (pri_pdp == cur_pdp 
                && (pri_pdp->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
               )
            {
              if(pri_pdp->mt_orig_type == TRUE)
              {
                 //Send the SMREG_PDP_DEACTIVATE_IND to the CM
                 sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_IND,
                                                          SM_NETWORK_CAUSE);
              }
              else
              {
                 //Send the SMREG_PDP_DEACTIVATE_PROMOTE_IND to the CM
                 sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_CNF,
                                                          SM_INTERNAL_CAUSE);
               }
             }
             else
             {
               if(pri_pdp->mt_orig_type == TRUE)
               {
                  //Send the SMREG_PDP_DEACTIVATE_PROMOTE_IND to the CM
                  sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_IND,
                                                       SM_NETWORK_CAUSE);
               }
               else
               {
                  //Send the SMREG_PDP_DEACTIVATE_PROMOTE_IND to the CM
                  sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_IND,
                                                       SM_INTERNAL_CAUSE);
                }

            }//if (pri_pdp == cur_pdp)
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
          }//if (sm_promote_sec_pri(pri_pdp))
          else
          {
              // Wait for the RABM to inform about RAB releases.  
              MSG_HIGH_DS_0(SM_SUB," Waiting for RABM for Release IND");
  
              /* Set the rab_state to RAB_DEACTIVATE_PENDING and 
               start RABM insanity timer.*/
              pri_pdp->rab_state = RABM_INACTIVE_PENDING;
  
              /* Put the connection Id into the timers_pending array */
              //timers_pending[TIMER_PENDING] = pdp_list[i].trans_id;
  
              /* Set the RAB Response Timer and wait for the RABM to respond */
              sm_start_timer(pri_pdp->trans_id, TIMER_RABM_RESP_PENDING); 
  
              // Wait for the RABMSM_MODIFY_RSP primitive from RABM.
              MSG_HIGH_DS_1(SM_SUB, "PDP %d DEACTIVTED, Wait for RABM",pri_pdp->trans_id);
          }


        }//if (is_umts_mode)..
        
      }// if (pri_pdp)..      
      
    }
    //If tear down indicator flag is false Deactivate single PDP
    else
    {
#ifdef FEATURE_GSM_GPRS
      /* ------------------------------------------------------------------
      ** Check if the UE is running in GPRS mode of operation and send
      ** GSNSM_DEACTIVATE_IND to the SNDCP.
      ** -----------------------------------------------------------------*/
      if ( cur_pdp->llc_state != LLC_NOT_CONFIRMED)
      {
       
        /*Send Deactivate indication to SNDCP */
        sm_send_sndcp_cmd (cur_pdp, GSNSM_DEACTIVATE_IND);  

        /* Set the llc_state to LLC_CONFIRM_PENDING, start RABM insanity timer */
        cur_pdp->llc_state = LLC_MO_DEACTIVATE_CNF_PENDING;

        /* Put the connection Id into the timers_pending array */
        //timers_pending[TIMER_PENDING] = cur_pdp->trans_id;

        /* ---------------------------------------------------------------------
        ** The PDP context has been activated, but LLC RAB has to be established
        ** before sending the SMREG_PDP_DEACTIVATE_CNF primitive to CM.
        ** --------------------------------------------------------------------*/      
        MSG_HIGH_DS_1(SM_SUB,"PDP %d DEACTIVATED, Waiting LLC Confirm ",cur_pdp->trans_id);
        
      }     
      else
#endif /*FEATURE_GSM_GPRS*/
      
      {
        //Reset the PDP con state
        cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;
        //If TI flag is off deactivate
        if (cur_pdp->rab_state == RABM_DEACTIVATED || cur_pdp->rab_state == RABM_INITIAL)
        {
          /*If the PDP context id primary and TI indicator is switched off then promote 
           one secondary to primary and give promote and deactivate ind*/
      
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
          if (cur_pdp->is_primary_pdp && !cur_pdp->teardown_ind.ti_flag && sm_promote_sec_pri(cur_pdp))
          {
            //Send the SMREG_PDP_DEACTIVATE_PROMOTE_IND to the CM
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_PROMOTE_CNF, SM_INTERNAL_CAUSE);
          }
          else
          {
#endif
            // Send the SMREG_PDP_DEACTIVATE_CNF to the CM.
            if(cur_pdp->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
            {
              // Send the SMREG_PDP_ACTIVATE_IND to the CM.
             sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
            }
            else
            {
             // Send the SMREG_PDP_DEACTIVATE_CNF to the CM.
             sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);
            }
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
          }
#endif
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
        }
        else
        {
          // Wait for the RABM to inform about RAB releases.  
          MSG_HIGH_DS_0(SM_SUB," Waiting for RABM for Release IND");

          // Set the rab_state to RAB_DEACTIVATE_PENDING and start RABM insanity timer.
          cur_pdp->rab_state = RABM_INACTIVE_PENDING;

          /* Put the connection Id into the timers_pending array */
          //timers_pending[TIMER_PENDING] = cur_pdp->trans_id;

          /* Set the RAB Response Timer and wait for the RABM to respond */
          sm_start_timer(cur_pdp->trans_id, TIMER_RABM_RESP_PENDING); 

          // Wait for the RABMSM_MODIFY_RSP primitive from RABM.
          MSG_HIGH_DS_1(SM_SUB, "PDP %d DEACTIVTED, Wait for RABM",cur_pdp->trans_id);
        }
      }//if (is_umts_mode())...
      
    }// if (cur_pdp->teardown_ind.ti_flag)  
    
  }//if (cur_pdp->pdp_state == PDP_INACTIVE_PENDING)..
  else
  {
    // Received PDP accept message in wrong state, Ignore
    MSG_ERROR_DS_1( SM_SUB,"Received DEACTIVATE_ACCEPT in wrong state %d",cur_pdp->pdp_state);

    /* --------------------------------------------------------------------
    ** If the Mobile receives a message not compatible with the protocol
    ** state, the MS shall ignore the message if there no RR connection,
    ** otherwise the UE returns a SM-STATUS message with proper cause.
    ** Refer to section 8.4 of TS 24.008 for more details.
    ** ------------------------------------------------------------------*/
    cur_pdp->current_cause = (cc_cause_T)SM_MESG_TYPE_NOT_COMPATIBLE;
    sm_send_sm_status(cur_pdp);
  }
}

/*===========================================================================
FUNCTION SM_PROC_REQUEST_PDP_ACTIVATION 

DESCRIPTION
  This function processes REQUEST_PDP_ACTIVATION received from the network.
  
DEPENDENCIES
  THIS IS NOT SUPPORTED CURRENTLY.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_proc_request_pdp_activation (pdp_context_info *cur_pdp)
{
  byte nsapi, reject_request=FALSE;
  word mesg_bit_len;
  byte l3_buf[MAX_OCTETS_L3_MESSAGE]; /* Buffer to store the packed L3 msg*/
  xlate_status_T status;
  byte saved_nsapi, saved_ti;

  // Do not allow Reselections/HOs when RRC quries as the PDP activation is in progress
  sm_ps_vote_for_reselection = FALSE;
#if defined(FEATURE_LTE)
  if(esm_isr_status_get() == TRUE)
  {
    cur_pdp->isr_pdp_act_status = CTXT_ACT_AFTER_ISR_IN_GW;
  }
  else
  {
    cur_pdp->isr_pdp_act_status = CTXT_ACT_BEFORE_ISR;
  }
#endif
  /* -----------------------------------------------------------------------
  ** Check if the PDP context is already active.  If so the SM will locally
  ** deactivate the PDP context and then proceed with the Network requested
  ** PDP context activation.
  ** ----------------------------------------------------------------------*/
  if (cur_pdp->pdp_state == PDP_ACTIVE ||
      cur_pdp->pdp_state == PDP_MODIFY_PENDING
      #ifdef FEATURE_MBMS
      #error code not present
#endif
      )
  {
    pdp_address_T stored_pdp_addr;
    apn_T         stored_apn_name;

    MSG_HIGH_DS_0(SM_SUB, "Network req PDP activation for Active PDP");
    #ifdef FEATURE_MBMS
      #error code not present
#endif

    saved_nsapi = cur_pdp->nsapi.nsapi;
    saved_ti = cur_pdp->trans_id;
    // Set the PDP to Inactive and send the deactivation Indication to CM.
    cur_pdp->pdp_state = PDP_INACTIVE;

    //Reset the PDP con state
    cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;

    //Send RABMSM_DEACTIVATE_IMPLICIT_IND to RABM
    sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, cur_pdp->nsapi.nsapi); // MBMS_PENDING
    
    /* Store the PDP address */
    stored_pdp_addr = cur_pdp->pdp_addr;  // MBMS_PENDING

    /* Store the APN Name */
    stored_apn_name = cur_pdp->apn_name;


    // Send the SMREG_PDP_DEACTIVATE_IND to the CM.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);   
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/

    (void) sm_get_connection_id_int(saved_ti);
    cur_pdp = pdp_list[saved_ti] ;
    
    if (cur_pdp == NULL)
    {
      MSG_ERROR_DS_1( SM_SUB, "PDP is NULL, Connection id %d is unreserved", saved_ti);
    }
    else
    {
      //Now trigger PDP Activation for the NSAPI
      cur_pdp->nsapi.valid = TRUE;
      cur_pdp->nsapi.nsapi = saved_nsapi;
      cur_pdp->trans_id = saved_ti;
      nsapi_list[saved_nsapi] = saved_ti;
  
      //restore the pdp_addr and APN
     /* Store the PDP address */
      cur_pdp->pdp_addr = stored_pdp_addr;
  
      /* Store the APN Name */
      cur_pdp->apn_name = stored_apn_name;
  
      // Set the mt_orig_type to true for Mobile terminated request
      cur_pdp->mt_orig_type = TRUE;
      cur_pdp->is_primary_pdp = TRUE;
      // Send the SMREG_PDP_ACTIVATE_IND to the CM to continue.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_IND, SM_INTERNAL_CAUSE); 
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    }
  }  
  else if (cur_pdp->pdp_state == PDP_ACTIVE_PENDING)
  {
    MSG_HIGH_DS_1(SM_SUB,"PDP activation is already pending on con id %d",cur_pdp->trans_id);
    saved_nsapi = cur_pdp->nsapi.nsapi;
    saved_ti = cur_pdp->trans_id;

    cur_pdp->current_cause = (cc_cause_T)PDP_DUPLICATE; // just for now
    //Send RABMSM_DEACTIVATE_IMPLICIT_IND to RABM
    sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, cur_pdp->nsapi.nsapi);

    // Send the SMREG_PDP_ACTIVATE_REJ to the CM.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);  
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    
    (void) sm_get_connection_id_int(saved_ti);
    cur_pdp = pdp_list[saved_ti] ;
    
    if (cur_pdp == NULL)
    {
      MSG_ERROR_DS_1( SM_SUB, "PDP is NULL, Connection id %d is unreserved", saved_ti);
    }
    else
    {
      cur_pdp->nsapi.valid = TRUE;
      cur_pdp->nsapi.nsapi = saved_nsapi;
      cur_pdp->trans_id = saved_ti;
      nsapi_list[saved_nsapi] = saved_ti;
  
      // Set the mt_orig_type to true for Mobile terminated request
      cur_pdp->mt_orig_type = TRUE;
      cur_pdp->is_primary_pdp = TRUE;
  
      // Send the SMREG_PDP_ACTIVATE_IND to the CM to continue.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_IND, SM_INTERNAL_CAUSE); 
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    }
  }
  #ifdef FEATURE_MBMS
  #error code not present
#endif
  else
  {
    MSG_MED_DS_1( SM_SUB, "Network Requested PDP activation, NSAPI %d",cur_pdp->trans_id);

    /*If already CM request is gone and waiting for CM response
      then drop the duplicate request*/
    if (cur_pdp->mt_orig_type)
    {
      //Do nothing just drop it
      return;
    }

    if ((nsapi = sm_per_subs_reserve_nsapi(cur_pdp->trans_id, (sys_modem_as_id_e_type)sm_as_id)) == INVALID_ID)
    {
      reject_request = TRUE;
      cur_pdp->current_cause = (cc_cause_T)INVALID_NSAPI;  // just for now
    }

    if (reject_request)
    {
      /* Pack the Layer 3 Message and send it to GMM */
      status = pack_layer3_mesg(&mesg_bit_len, l3_buf, cur_pdp, REQUEST_PDP_ACTIVATION_REJ);

      if (status == L3_PARSE_SUCCESS)
      {
        // Send the packed SM-PDU to the GMM to be sent to the Network
        send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);
      }
      else
      {
        MSG_ERROR_DS_0( SM_SUB, "Error in Encoding the Request PDP activation reject msg");
      }

      if (cur_pdp->nsapi.valid == TRUE)
      (void)sm_release_nsapi_sm (cur_pdp->trans_id);

      (void) sm_unreserve_connection_id_int(cur_pdp->trans_id);
      return;
    }
    else
    {
      cur_pdp->nsapi.valid = TRUE;
      cur_pdp->nsapi.nsapi = nsapi;
    }

    
    // Set the mt_orig_type to true for Mobile terminated request
    cur_pdp->mt_orig_type = TRUE;
    cur_pdp->is_primary_pdp = TRUE;

    // Send the SMREG_PDP_ACTIVATE_IND to the CM.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_IND, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
}

/*===========================================================================
FUNCTION SM_PROC_REQUEST_PDP_DEACTIVATION 

DESCRIPTION
  This function processes REQUEST_PDP_DEACTIVATION received from the network.
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_proc_request_pdp_deactivation(pdp_context_info *cur_pdp)
{
  xlate_status_T status;
  word mesg_bit_len;
  word group_id;
  
  byte l3_buf[MAX_OCTETS_L3_MESSAGE]; /* Buffer to store the packed L3 msg*/

  MSG_HIGH_DS_2(SM_SUB,"Network Initiated PDP deactivation, NSAPI : %d, TransID=%d",
           cur_pdp->nsapi.nsapi,cur_pdp->trans_id);

  // Check the status of the PDP
  if ( (cur_pdp->pdp_state != PDP_ACTIVE           ) &&
    #ifdef FEATURE_MBMS
       #error code not present
#endif
       (cur_pdp->pdp_state != PDP_INACTIVE_PENDING ) &&
       (cur_pdp->pdp_state != PDP_MODIFY_PENDING)) 
  {
    // Received PDP Deactive message in wrong state, Ignore
    #ifdef FEATURE_MBMS
        #error code not present
#endif

    MSG_ERROR_DS_1( SM_SUB,"Received REQ_PDP_DEACTIVATE in wrong state %d",cur_pdp->pdp_state);

    /* --------------------------------------------------------------------
    ** If the Mobile receives a message not compatible with the protocol
    ** state, the MS shall ignore the message if there no RR connection,
    ** otherwise the UE returns a SM-STATUS message with proper cause.
    ** Refer to section 8.4 of TS 24.008 for more details.
    ** ------------------------------------------------------------------*/
    cur_pdp->current_cause = (cc_cause_T)SM_MESG_TYPE_NOT_COMPATIBLE;
    sm_send_sm_status(cur_pdp); 
  }
#ifdef FEATURE_MBMS
  #error code not present
#endif

  else
  {
    //Stop in case any timer running
    sm_stop_timer(cur_pdp->trans_id);
    /* Pack the Layer 3 Message and send it to GMM */
    status = pack_layer3_mesg(&mesg_bit_len, l3_buf, cur_pdp, 
                              DEACTIVATE_PDP_ACCEPT);
    if (status == L3_PARSE_SUCCESS)
    {
      word index;
      // Send the packed SM-PDU to the GMM to be sent to the Network
      send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);    
      //Copy the group id
      group_id = cur_pdp->group_id;
      if (!cur_pdp->teardown_ind.ti_flag)
      {
        //If TI indicator is off then Deactivate single PDP
        MSG_HIGH_DS_1(SM_SUB,"Single PDP Deactivation for %d TI is OFF", cur_pdp->trans_id);
        sm_proc_deact_req(cur_pdp);
      }     
      else
      {
        pdp_context_info *pri_pdp = NULL;
        //If TI indicator is on then Deactivate PDPs in the group
        MSG_HIGH_DS_1(SM_SUB,"Multiple PDP Deactivation for %d TI is ON", cur_pdp->trans_id);

        for (index = 0; index < MAX_PDP_NUM; index++)
        {
          if (pdp_list[index] != NULL && 
               group_id == pdp_list[index]->group_id)
          {
            //PDPs in the same group is deactivated
            pdp_list[index]->teardown_ind.valid= TRUE;
            pdp_list[index]->teardown_ind.ti_flag = TRUE;
            if (pdp_list[index]->is_primary_pdp)
            {
              //If primary PDP then deactivate last
              pri_pdp = pdp_list[index];
            }
            else
            {
              //If not primary deactivate right away
              sm_proc_deact_req(pdp_list[index]);
            }
          }//if (group_id == ....
        }
        //Now deactivate primary
        if(pri_pdp)
        {
        sm_proc_deact_req(pri_pdp);
        }
      }
    }//If status = L3_PARSE_....
    else
    {
      MSG_ERROR_DS_0( SM_SUB, "Error in Encoding the Deactivate Req msg");
    }
  }
}
/* -----------------------------------------------------------------------
** FUNCTION     : sm_proc_get_bcm_validation_on_bearer_mod()
** DESCRIPTION  : This function returns TRUE if  bcm_validation_on_bearer_mod NV is enabled
** ----------------------------------------------------------------------- */

boolean sm_proc_get_bcm_validation_on_bearer_mod_nv(void)
{
  const char *file_path = 
      "/nv/item_files/modem/data/3gpp/bcm_validation_on_bearer_mod";
  
  mcfg_fs_status_e_type  result = MCFG_FS_STATUS_EFS_ERR;
  ds_3gpp_bcm_info_validation_s bcm_validation_on_bearer_mod;
  memset((void*)&bcm_validation_on_bearer_mod,0,
          sizeof(bcm_validation_on_bearer_mod));

  /* Set to Default Value */
  bcm_validation_on_bearer_mod.bcm_info_validation_enabled = TRUE;
  
  result = mcfg_fs_read(file_path, (void *)(&bcm_validation_on_bearer_mod), 
                   sizeof(bcm_validation_on_bearer_mod), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)0);
  if(result != MCFG_FS_STATUS_OK)
  {
          MSG_ERROR_DS_1( SM_SUB, "NV bcm_validation_on_bearer_mod Read failed, using default value as TRUE, status = %d", result);
		  bcm_validation_on_bearer_mod.bcm_info_validation_enabled = TRUE;
  }
  return bcm_validation_on_bearer_mod.bcm_info_validation_enabled;
}

/*===========================================================================
FUNCTION SM_PROC_NETWORK_MODIFY_PDP_REQUEST 

DESCRIPTION
  This function processes the Network requested PDP context modification. 

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_proc_network_modify_pdp_request(pdp_context_info *pdp)
{

  xlate_status_T status;
  word mesg_bit_len;
  byte l3_buf[MAX_OCTETS_L3_MESSAGE]; /* Buffer to store the packed L3 msg*/
  boolean qos_validation_status = TRUE;
  boolean bcm_validation_needed = TRUE;
  boolean is_nw_initiated_qos_support = ds3gpp_per_subs_get_config_umts_nw_init_qos_support((sys_modem_as_id_e_type)sm_as_id);
  qos_T *offered_qos = NULL;
  bearer_ctrl_mode_T bcm;
  word index;
  MSG_HIGH_DS_2(SM_SUB,"Processing n/w modify request trans id: %d state %d", 
                                         pdp->trans_id, pdp->pdp_state);
  if(is_nw_initiated_qos_support)
  {
    bcm = sm_get_bcm_from_pco(&pdp->config_options);
    bcm_validation_needed = sm_proc_get_bcm_validation_on_bearer_mod_nv();
    if ((pdp->tft.valid) && 
        (bcm == BCM_MS_ONLY)&&
        (bcm_validation_needed))
    {
      pdp->current_cause = (cc_cause_T)SM_ACTIVATION_REJECTED_BCM_VIOLATION;
      MSG_HIGH_DS_0(SM_SUB, "Network req Modification Rejected due to BCM violation");    
  
      status = pack_layer3_mesg(&mesg_bit_len, l3_buf, pdp, MODIFY_PDP_REJECT);
      if(status == L3_PARSE_SUCCESS)
      {
        send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);  
      }   
      return;
    }
  }

  if(is_nw_initiated_qos_support)
  {
  qos_validation_status = sm_validate_qos ( pdp->req_qos, pdp->minimum_qos );
  offered_qos = &(pdp->req_qos);
  }
  else
  {
  qos_validation_status = sm_validate_qos ( pdp->qos, pdp->minimum_qos );
  offered_qos = &(pdp->qos);
  }
  if (offered_qos->max_bit_rare_uplink == 0xFF && offered_qos->max_bit_rare_downlink == 0xFF)
  {
    MSG_ERROR_DS_1( SM_SUB, "MBR Uplink and Downlink 0kbps on NSAPI = %d", pdp->nsapi.nsapi);
    pdp->current_cause = (cc_cause_T)SM_INVALID_MANDATORY_INFO;
    sm_send_sm_status( pdp );
  }
  else if (qos_validation_status == FALSE )
  {
    MSG_ERROR_DS_1( SM_SUB, "QoS valdn fail on NSAPI = %d", pdp->nsapi.nsapi);
    sm_handle_qos_validation_failure ( pdp );
  }
  else if( (is_nw_initiated_qos_support == 0) || ((is_nw_initiated_qos_support)&&(!pdp->tft.valid)))    
  {
    /* Pack the Layer 3 Message and send it to GMM */
    status = pack_layer3_mesg(&mesg_bit_len, l3_buf, pdp, MS_MODIFY_PDP_ACCEPT);

    if (status == L3_PARSE_SUCCESS)
    {
      // Send the Modify PDP accept message to network
      MSG_HIGH_DS_0(SM_SUB, "Sending SM-PDU to GMM ");
      send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);  
      
      #ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
      pdp->sm_msg_to_resend = MS_MODIFY_PDP_ACCEPT;
      pdp->status_ind_flag = FALSE;
      #endif

      if(is_nw_initiated_qos_support)
      {
      /*Copy data from temporary locations*/
      pdp->radio_pri = pdp->req_radio_pri;
      pdp->llc_sapi = pdp->req_llc_sapi;
      pdp->qos = pdp->req_qos;
      if(pdp->req_flow_id.valid)
      {
        pdp->flow_id = pdp->req_flow_id;
      }  

      pdp->req_radio_pri.valid = FALSE;
      pdp->req_llc_sapi.valid = FALSE;
      pdp->req_qos.valid = FALSE;
      pdp->req_flow_id.valid = FALSE;
      
      if(bcm_validation_needed)
      {
        pdp->bcm = sm_get_bcm_from_pco(&pdp->config_options);

        /* Set BCM of all PDP in group same as cur pdp*/
        for (index = 0; index < MAX_PDP_NUM; index++)
        {
          if ((index != pdp->trans_id) && 
              (pdp_list[index] != NULL) &&
               (pdp->group_id == pdp_list[index]->group_id) &&
               (pdp_list[index]->pdp_state != PDP_INACTIVE))
          {
            pdp_list[index]->bcm = pdp->bcm;
            MSG_HIGH_DS_2(SM_SUB, "BCM Mode for TI %d is %d", index, pdp_list[index]->bcm);
          }
        } /* end of for */
       } /* end of NV check if */
      } 
      
      /* ---------------------------------------------------------------------
      ** If the received message is UNPACK_NETWORK_MODIFY_PDP_REQ, make sure
      ** that the Network is requesting a Modify request for an already ACTIVE
      ** PDP context, otherwise throw an ERROR..
      ** --------------------------------------------------------------------*/   
      if (pdp->pdp_state == PDP_MODIFY_PENDING && 
          pdp->cm_pending_msg == (int)SMREG_PDP_MODIFY_REQ)
      {
         /* ---------------------------------------------------------------------
        ** MODIFICATION collision, terminate the mobile originated modification
        ** procedures and continue with the network originated modification.
        ** -------------------------------------------------------------------*/
         MSG_HIGH_DS_1(SM_SUB, "COLLISION: Network/MS requested PDP Modification NSAPI%d",
                    pdp->nsapi.nsapi);

        /*Clear the CM Pending Request*/
        pdp->cm_pending_msg = UNKNOWN_CMD;

        /*Stop the T3381 timer*/
        sm_stop_timer(pdp->trans_id);

        /*Set the PDP state to PDP_ACTIVE*/
        pdp->pdp_state = PDP_ACTIVE;

        /*Reset the request QoS values*/
        //pdp->req_llc_sapi.valid = FALSE;
        pdp->req_qos.valid      = FALSE;
        pdp->req_tft.valid      = FALSE;

        /*Set the cause*/
        pdp->current_cause = (cc_cause_T)PDP_MODIFY_COLLISION;

        /*Send the Modify Reject to CM*/
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
        sm_send_cm_confirm(pdp, SMREG_PDP_MODIFY_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      }

#ifdef FEATURE_GSM_GPRS
      if (is_gsm_mode())
      {
        pdp->pdp_state = PDP_MODIFY_PENDING;
        /* ------------------------------------------------------------------
        ** Check if the UE is running in GPRS mode of operation and send
        ** GSNSM_MODIFY_IND to the SNDCP.
        ** -----------------------------------------------------------------*/
        sm_send_sndcp_cmd (pdp, GSNSM_MODIFY_IND);  
 
        /* Set the llc_state to LLC_CONFIRM_PENDING, start sanity timer */
        pdp->llc_state = LLC_MT_MODIFY_CNF_PENDING;

        /* Put the connection Id into the timers_pending array */
       // timers_pending[TIMER_PENDING] = pdp->trans_id;


        /* ---------------------------------------------------------------------
        ** The PDP context has been activated, but LLC RAB has to be established
        ** before sending the SMREG_PDP_MODIFY_IND primitive to CM.
        ** --------------------------------------------------------------------*/      
        MSG_HIGH_DS_1(SM_SUB,"PDP MODIFIED %d, Waiting LLC Confirm",pdp->trans_id);
      }

#endif /*FEATURE_GSM_GPRS*/
  
      if (is_umts_mode())
      {
          MSG_HIGH_DS_0(SM_SUB, "UMTS mode, send modify indication of CM ");
        /* -------------------------------------------------------------------
             6.1.3.3.1 : "if the PDP context is preserved, then the network may re-establish a 
             Radio Access Bearer with the new QoS indicated in the MODIFY PDP CONTEXT 
             REQUEST message."
             So it is not mandatory for NW to establsih RAB.
        ** ------------------------------------------------------------------*/
            
          // set pdp context to active, because it is Mobile Terminated. 
          pdp->pdp_state = PDP_ACTIVE;

          // Send the SMREG_PDP_MODIFY_IND to the CM for Mobile terminated req.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
          sm_send_cm_confirm(pdp, SMREG_PDP_MODIFY_IND, SM_NETWORK_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
        }
    }
  }
  else
  {
    if (pdp->pdp_state == PDP_MODIFY_PENDING && 
           pdp->cm_pending_msg == (int)SMREG_PDP_MODIFY_REQ)
    {
       /* ---------------------------------------------------------------------
      ** MODIFICATION collision, terminate the mobile originated modification
      ** procedures and continue with the network originated modification.
      ** -------------------------------------------------------------------*/
      MSG_HIGH_DS_1(SM_SUB, "COLLISION: Network/MS requested PDP Modification NSAPI%d",
                    pdp->nsapi.nsapi);

      /*Clear the CM Pending Request*/
      pdp->cm_pending_msg = UNKNOWN_CMD;

      /*Stop the T3381 timer*/
      sm_stop_timer(pdp->trans_id);

      /*Set the PDP state to PDP_ACTIVE*/
      pdp->pdp_state = PDP_ACTIVE;
    
      /*Set the cause*/
      pdp->current_cause = (cc_cause_T)PDP_MODIFY_COLLISION;

      /*Send the Modify Reject to CM*/
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm(pdp, SMREG_PDP_MODIFY_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    }
    
    pdp->pdp_state = PDP_MODIFY_PENDING;
    pdp->cm_pending_msg = UNKNOWN_CMD;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)    
    sm_send_cm_confirm(pdp, SMREG_PDP_NW_MODIFY_REQ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
}


/*===========================================================================
FUNCTION SM_PROC_REQUEST_SEC_PDP_ACTIVATION 

DESCRIPTION
  This function processes MT secondary PDP activation request message. 

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_proc_request_sec_pdp_activation (pdp_context_info *cur_pdp)
{

  word mesg_bit_len;
  byte* l3_buf; /* Buffer to store the packed L3 msg*/
  xlate_status_T status;
  boolean reject_request = FALSE;
  byte nsapi;
  
  qos_T saved_qos;
  tft_type_T* saved_tft;
  protocol_cfg_options_T* saved_pco;
  byte saved_nsapi, saved_ti;
  byte saved_linked_ti;
  
  #ifdef FEATURE_MODEM_HEAP
  l3_buf = (byte *)modem_mem_calloc(1,MAX_OCTETS_L3_MESSAGE,MODEM_MEM_CLIENT_NAS);
  saved_tft = (tft_type_T *)modem_mem_calloc(1,sizeof(tft_type_T),MODEM_MEM_CLIENT_NAS);
  saved_pco = (protocol_cfg_options_T *)modem_mem_calloc(1,sizeof(protocol_cfg_options_T),MODEM_MEM_CLIENT_NAS);
#else
  l3_buf = (byte *)gs_alloc(MAX_OCTETS_L3_MESSAGE);
  saved_tft = (tft_type_T *)gs_alloc(sizeof(tft_type_T));
  saved_pco = (protocol_cfg_options_T *)gs_alloc(sizeof(protocol_cfg_options_T));
#endif
  sm_check_for_null_ptr((void *) l3_buf);
  sm_check_for_null_ptr((void *) saved_tft);
  sm_check_for_null_ptr((void *) saved_pco);  

  memset((void *)l3_buf,0,MAX_OCTETS_L3_MESSAGE);
  memset((void *)saved_tft,0,sizeof(tft_type_T));
  memset((void *)saved_pco,0,sizeof(protocol_cfg_options_T));

  // If the primary context does't exist then set reject_request to true. 
  // Call will be reject only after checking condition regarding pdp state  
  if (cur_pdp->linked_ti >= MAX_PDP_NUM || 
      pdp_list[cur_pdp->linked_ti] == NULL ||
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
      pdp_list[cur_pdp->linked_ti]->is_primary_pdp == FALSE ||
#endif
      !( pdp_list[cur_pdp->linked_ti]->pdp_state == PDP_ACTIVE ||
      pdp_list[cur_pdp->linked_ti]->pdp_state == PDP_MODIFY_PENDING) )
  {
    MSG_HIGH_DS_0(SM_SUB, "Network req Seondary Activation Rej as Primary PDP Inactive");    
    cur_pdp->current_cause = (cc_cause_T)SM_UNKNOWN_PDP_CONTEXT;
    reject_request = TRUE;
  }
  // If primary BCM mode is not BCM_MS_ONLY then reject secondary activation
  else if (pdp_list[cur_pdp->linked_ti]->bcm == BCM_MS_ONLY)
  {
    MSG_HIGH_DS_0(SM_SUB, "Network req Seondary Activation Rej due to BCM violation");    
    cur_pdp->current_cause = (cc_cause_T)SM_ACTIVATION_REJECTED_BCM_VIOLATION;
    reject_request = TRUE;
  }
  else if ( !(cur_pdp->tft.operation_code == 1 || cur_pdp->tft.operation_code == 6)  )
  {
    MSG_HIGH_DS_2(SM_SUB, "Received REQUEST_SEC_PDP_ACTIVATION message with OP Code other than CREATE TI: %d, OP: %d", 
      cur_pdp->trans_id, cur_pdp->tft.operation_code);
    cur_pdp->config_options.valid = FALSE;
    cur_pdp->current_cause = (cc_cause_T)SM_SEMANTIC_ERR_IN_TFT;
    reject_request = TRUE;
  }
  else if ( cur_pdp->tft.operation_code == 6 && cur_pdp->current_cause != (cc_cause_T)PPD_UNKNOWN_REASON )
  {
    MSG_HIGH_DS_2(SM_SUB, "Received REQUEST_SEC_PDP_ACTIVATION message with OP Code : %d and non-empty list for TI: %d",  cur_pdp->tft.operation_code,cur_pdp->trans_id);
    cur_pdp->config_options.valid = FALSE;
    cur_pdp->current_cause = (cc_cause_T)SM_SYNTACTIC_ERR_IN_TFT;
    reject_request = TRUE;
  }

  /* If PDP is not Inactive state, send deactivate to higher layers and process activation request*/
  if (cur_pdp->pdp_state != PDP_INACTIVE)
  {       
    MSG_HIGH_DS_0(SM_SUB, "Network req Seondary PDP activation for Active PDP");     
  
#ifdef FEATURE_MBMS
    #error code not present
#endif /*FEATURE_MBMS*/    
  
    saved_nsapi = cur_pdp->nsapi.nsapi;
    saved_ti = cur_pdp->trans_id;
    saved_linked_ti = cur_pdp->linked_ti;
    /* Store the QOS parameters */
    saved_qos = cur_pdp->qos;
  
    /* Store the TFT*/
    memscpy((void *)saved_tft,sizeof(tft_type_T),(void *)&cur_pdp->tft,sizeof(tft_type_T));
  
    /*Store the PCO*/
    memscpy((void *)saved_pco,sizeof(protocol_cfg_options_T),(void *)&cur_pdp->config_options,sizeof(protocol_cfg_options_T)); 
     
    // Set the PDP to Inactive and send the deactivation Indication to CM.
    cur_pdp->pdp_state = PDP_INACTIVE;
    
    //Reset the PDP con state
    cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;
  
    if (is_umts_mode())
    {
      //Send RABMSM_DEACTIVATE_IMPLICIT_IND to RABM
      sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, cur_pdp->nsapi.nsapi);
    }
#ifdef FEATURE_GSM_GPRS
    else if (is_gsm_mode())
    {
      /* Set the llc_state to LLC_CONFIRM_PENDING*/
      cur_pdp->llc_state = LLC_NOT_CONFIRMED;     
      /*Send Deactivate indication to SNDCP */
      sm_send_sndcp_cmd (cur_pdp, GSNSM_DEACTIVATE_IND);            
    }
#endif /*FEATURE_GSM_GPRS*/  
    //Stop in case any timer running
    sm_stop_timer (cur_pdp->trans_id);
     
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    if (cur_pdp->pdp_state == PDP_ACTIVE_PENDING)
    {
      cur_pdp->current_cause = (cc_cause_T)PDP_DUPLICATE;        
      // Send the SMREG_PDP_ACTIVATE_REJ to the CM.
      sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);  
    }
  #ifdef FEATURE_MBMS    
    #error code not present
#endif /*FEATURE_MBMS*/    
    else 
    {
      // Send the SMREG_PDP_DEACTIVATE_IND to the CM.
      sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);   
    }
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/

    // If reject request has been set above then reject secondary PDP activation
    if (reject_request)
    {
      cur_pdp->config_options.valid = FALSE;
      status = pack_layer3_mesg(&mesg_bit_len, l3_buf, cur_pdp, REQUEST_SEC_PDP_ACTIVATION_REJ);
      if (status == L3_PARSE_SUCCESS)
      {
        // Send the packed SM-PDU to the GMM to be sent to the Network
        send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);
      }
      else
      {
        MSG_ERROR_DS_0( SM_SUB, "Error in Encoding the Request Seondary PDP activation reject msg");
      }
      //Stop in case any timer running
      sm_stop_timer (cur_pdp->trans_id);    
      if (cur_pdp->nsapi.valid == TRUE)
      (void)sm_release_nsapi_sm (cur_pdp->trans_id);


      (void) sm_unreserve_connection_id_int(cur_pdp->trans_id);
    }
    //Now trigger PDP Activation for the NSAPI 
    else
    {
      (void) sm_get_connection_id_int(saved_ti);
      cur_pdp = pdp_list[saved_ti] ;
      
      if (cur_pdp == NULL)
      {
        MSG_ERROR_DS_1( SM_SUB, "PDP is NULL, Connection id %d is unreserved", saved_ti);
      }
      else
      {
        //restore the values       
        cur_pdp->nsapi.valid = TRUE;
        cur_pdp->nsapi.nsapi = saved_nsapi;
        cur_pdp->trans_id = saved_ti;
        nsapi_list[saved_nsapi] = saved_ti;
        cur_pdp->linked_ti = saved_linked_ti;            
        cur_pdp->qos = saved_qos;
        cur_pdp->tft = *saved_tft; 
        cur_pdp->config_options = *saved_pco; 
  
        memset((void *)&cur_pdp->minimum_qos, 0, sizeof(qos_T));
        cur_pdp->minimum_qos.valid = TRUE;
      
        //Set BCM mode as that of primary
        cur_pdp->bcm = pdp_list[cur_pdp->linked_ti]->bcm;
        
        cur_pdp->is_primary_pdp = FALSE;
        
        // Set the mt_orig_type to true for Mobile terminated request
        cur_pdp->mt_orig_type = TRUE;
        
       // Donot Allow Resleections/HO when RRC query as the PDP Activation is in progress
       sm_ps_vote_for_reselection = FALSE;

        // Send the SMREG_PDP_ACTIVATE_IND to the CM to continue.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_IND, SM_INTERNAL_CAUSE); 
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      }
    }
  }
  else
  {
    // If reject request has been set above then reject secondary PDP activation
    if (reject_request)
    {
      status = pack_layer3_mesg(&mesg_bit_len, l3_buf, cur_pdp, REQUEST_SEC_PDP_ACTIVATION_REJ);
      if (status == L3_PARSE_SUCCESS)
      {
        // Send the packed SM-PDU to the GMM to be sent to the Network
        send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);
      }
      else
      {
        MSG_ERROR_DS_0( SM_SUB, "Error in Encoding the Request Seondary PDP activation reject msg");
      }
      //Stop in case any timer running
      sm_stop_timer (cur_pdp->trans_id);    
      (void) sm_unreserve_connection_id_int(cur_pdp->trans_id);
    }
    else
    {    
      MSG_MED_DS_1( SM_SUB, "Network Requested PDP activation, TI=%d",cur_pdp->trans_id);
    
      /*If already CM request is gone and waiting for CM response
        then drop the duplicate request*/
      if (cur_pdp->mt_orig_type)
      {
        //Do nothing just drop it
#ifdef FEATURE_MODEM_HEAP
		modem_mem_free(l3_buf,MODEM_MEM_CLIENT_NAS);
        modem_mem_free(saved_tft,MODEM_MEM_CLIENT_NAS);
        modem_mem_free(saved_pco,MODEM_MEM_CLIENT_NAS);
#else
		gs_free(l3_buf);
        gs_free(saved_tft);
        gs_free(saved_pco);
#endif
        return;
      }
      if ((nsapi = sm_per_subs_reserve_nsapi(cur_pdp->trans_id, (sys_modem_as_id_e_type)sm_as_id)) == INVALID_ID)
      {
        reject_request = TRUE;
        cur_pdp->current_cause = (cc_cause_T)INVALID_NSAPI;  // just for now
      }
      
      if (reject_request)
      {
        /* Pack the Layer 3 Message and send it to GMM */
        status = pack_layer3_mesg(&mesg_bit_len, l3_buf, cur_pdp, REQUEST_SEC_PDP_ACTIVATION_REJ);
      
        if (status == L3_PARSE_SUCCESS)
        {
          // Send the packed SM-PDU to the GMM to be sent to the Network
          send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);
        }
        else
        {
          MSG_ERROR_DS_0( SM_SUB, "Error in Encoding the Secondary Request PDP activation reject msg");
        }
      
        if (cur_pdp->nsapi.valid == TRUE)
        (void)sm_release_nsapi_sm (cur_pdp->trans_id);

        (void) sm_unreserve_connection_id_int(cur_pdp->trans_id);
#ifdef FEATURE_MODEM_HEAP
		modem_mem_free(l3_buf,MODEM_MEM_CLIENT_NAS);
        modem_mem_free(saved_tft,MODEM_MEM_CLIENT_NAS);
        modem_mem_free(saved_pco,MODEM_MEM_CLIENT_NAS);
#else
		gs_free(l3_buf);
        gs_free(saved_tft);
        gs_free(saved_pco);
#endif
        return;
      }
      else
      {
        cur_pdp->nsapi.valid = TRUE;
        cur_pdp->nsapi.nsapi = nsapi;
      }
            
      // Set the mt_orig_type to true for Mobile terminated request
      cur_pdp->mt_orig_type = TRUE;    
      //Set BCM mode as that of primary
      cur_pdp->bcm = pdp_list[cur_pdp->linked_ti]->bcm;      
      cur_pdp->is_primary_pdp = FALSE;      
      memset((void *)&cur_pdp->minimum_qos, 0, sizeof(qos_T));
      cur_pdp->minimum_qos.valid = TRUE;
      
     // Donot Allow Resleections/HO when RRC query as the PDP Activation is in progress
     sm_ps_vote_for_reselection = FALSE;

      // Send the SMREG_PDP_ACTIVATE_IND to the CM.      
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_IND, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    }    
  }
#ifdef FEATURE_MODEM_HEAP
		modem_mem_free(l3_buf,MODEM_MEM_CLIENT_NAS);
        modem_mem_free(saved_tft,MODEM_MEM_CLIENT_NAS);
        modem_mem_free(saved_pco,MODEM_MEM_CLIENT_NAS);
#else
		gs_free(l3_buf);
        gs_free(saved_tft);
        gs_free(saved_pco);
#endif         
}


/*===========================================================================
FUNCTION SM_SEND_SM_STATUS 

DESCRIPTION
  This function processes SM-STATUS message. 

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_send_sm_status
(
  pdp_context_info *pdp
)
{
  xlate_status_T status;
  word mesg_bit_len;
  byte l3_buf[MAX_OCTETS_L3_MESSAGE]; /* Buffer to store the packed L3 msg*/
  /* Pack the Layer 3 Message and send it to GMM */
  status = pack_layer3_mesg(&mesg_bit_len, l3_buf, pdp, SM_STATUS);

  if (status == L3_PARSE_SUCCESS)
  {
    send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);
  }
  else
  {
    MSG_ERROR_DS_0( SM_SUB,"Unable to pack the SM-STATUS message");
  }
}

/*===========================================================================
FUNCTION SEND_GMM_RAT_RESUME_CNF

DESCRIPTION
  This function confirms the resumption of SM operation on the new RAT

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void send_gmm_rat_resume_cnf(void)
{
  mm_cmd_type * gmm_cmd;
  gmm_cmd = mm_get_cmd_buf();

  if (gmm_cmd != NULL)
  {
    gmm_cmd->cmd.gmmsm_rat_resume_cnf.header.message_set     = MS_GMM_SM;
    gmm_cmd->cmd.gmmsm_rat_resume_cnf.header.message_id      = GMMSM_RAT_RESUME_CNF;
    gmm_cmd->cmd.gmmsm_rat_resume_cnf.header.message_len_lsb = 0;
    gmm_cmd->cmd.gmmsm_rat_resume_cnf.header.message_len_msb = 0;   

#ifdef FEATURE_DUAL_SIM

    #ifdef FEATURE_DUAL_DATA
    gmm_cmd->cmd.gmmsm_rat_resume_cnf.as_id = (sys_modem_as_id_e_type)sm_as_id; //NEED_TO_CEHCK -- Ravi
    #else
    if (SM_IS_VALID_AS_ID(ctxt_as_id) == FALSE)
      gmm_cmd->cmd.gmmsm_rat_resume_cnf.as_id= ctxt_as_id;  
    else
      gmm_cmd->cmd.gmmsm_rat_resume_cnf.as_id= (sys_modem_as_id_e_type)sm_as_id;  
    #endif /*#ifdef FEATURE_DUAL_DATA*/
#endif
    MSG_HIGH_DS_0(SM_SUB, "Sending GMMSM_RAT_RESUME_CNF to GMM");
    mm_put_cmd (gmm_cmd);
  }
  else
  {
    MSG_ERROR_DS_0( SM_SUB, "Unable to get a GMM command buffer");
  } 
}

/*===========================================================================
FUNCTION SM_PROC_RAT_CHANGE

DESCRIPTION
  This function processes the RAT change notification from GMM.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sm_process_rat_change(sm_cmd_type *msg)
{
  int index;
  pdp_context_info *cur_pdp;

  sig_connection_available = FALSE;
  ps_signalling_conn_state = FALSE;

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  if(SM_IS_VALID_AS_ID(msg->cmd.gmm_rat_change_req.as_id) == FALSE)
  {
    MSG_HIGH_DS_2( SM_SUB,"SM AS ID Conflict sm_as_id : %d, message as_id: %d", sm_as_id , msg->cmd.gmm_rat_change_req.as_id);
    return ;
  }
#endif
#endif

  /* -----------------------------------------------------------
  ** Notify RABM about change of active Radio Access Technology
  ** -----------------------------------------------------------*/
  if ((msg->cmd.gmm_rat_change_req.rat == SYS_RAT_UMTS_RADIO_ACCESS) ||
      (msg->cmd.gmm_rat_change_req.rat == SYS_RAT_TDS_RADIO_ACCESS))
  {
    sm_notify_rabm ((int)RABMSM_RAT_CHANGE_IND, NO_SPECIFIC_NSAPI);
  }

  for (index=0; index<MAX_PDP_NUM; index++)
  {
    cur_pdp = pdp_list[index];
    if (cur_pdp != NULL)
    {
      if (cur_pdp->trans_id == index)
      {
  #ifdef FEATURE_GSM_GPRS
        /* ------------------------------------------------------------------
        ** Check if the UE is running in GPRS mode of operation and send
        ** GSNSM_DEACTIVATE_IND to the SNDCP.
        ** -----------------------------------------------------------------*/
        if (msg->cmd.gmm_rat_change_req.rat == SYS_RAT_GSM_RADIO_ACCESS)
        {
          /*If the PDP is primary look if any Secondary PDP is waiting for LLC CNF*/
          if (cur_pdp->is_primary_pdp)
          {
            uint8 srch_index;
  
            for(srch_index = 0; srch_index <= MAX_MO_TI; srch_index++)
            {
              if (pdp_list[srch_index] != NULL &&
                  (pdp_list[srch_index]->trans_id != cur_pdp->trans_id) &&
                   (pdp_list[srch_index]->group_id == cur_pdp->group_id))
              {
  
                if (pdp_list[srch_index]->llc_state == LLC_MO_DEACTIVATE_CNF_PENDING )
                {
                  sm_send_cm_confirm(pdp_list[srch_index], SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
                }
                else if (pdp_list[srch_index]->llc_state == LLC_MT_DEACTIVATE_CNF_PENDING)
                {
                  sm_send_cm_confirm(pdp_list[srch_index], SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);
                }
                else if (pdp_list[srch_index]->llc_state != LLC_NOT_CONFIRMED)
                {
                  sm_send_sndcp_cmd (pdp_list[srch_index], GSNSM_DEACTIVATE_IND);  
                  pdp_list[srch_index]->llc_state = LLC_NOT_CONFIRMED;
                }
              }
            }     
          }
          
          /*Process the current PDP*/
  
          if (cur_pdp->llc_state == LLC_MO_DEACTIVATE_CNF_PENDING )
          {
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
          }
          else if (cur_pdp->llc_state == LLC_MT_DEACTIVATE_CNF_PENDING)
          {
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);
          }
          else if (cur_pdp->llc_state != LLC_NOT_CONFIRMED)
          {
            sm_send_sndcp_cmd (cur_pdp, GSNSM_DEACTIVATE_IND);  
            cur_pdp->llc_state = LLC_NOT_CONFIRMED;
          }
            /* Reset the sm resend msg flags on RAT Change; SM retry will kick-in for G2G reselections on reception of GMMSM_STATUS_IND
            For IRAT, NW retry can address the issue, as anyway NAS need to wait till RAU on Target RAT to re-try. And also, if NAS re-tries
            PDP Modify Accept would be the first message on Target RAT (W), which might have further complications */
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
          cur_pdp->sm_msg_to_resend   = (sm_layer_msg_T)0xFF;
          cur_pdp->status_ind_flag = FALSE;
#endif
        }     
  #endif /*FEATURE_GSM_GPRS*/
      }/*if (msg->cmd.gmm_rat_change_req.rat == SYS_RAT_GSM_RADIO_ACCESS)*/
      //If UE is leaving UMTS RAT
      if ((msg->cmd.gmm_rat_change_req.rat == SYS_RAT_UMTS_RADIO_ACCESS) ||
          (msg->cmd.gmm_rat_change_req.rat == SYS_RAT_TDS_RADIO_ACCESS))
      {
        if (cur_pdp->pdp_state == PDP_INACTIVE &&
            sm_timer_status[cur_pdp->trans_id] == (word) TIMER_RABM_RESP_PENDING)
        {
          /*stop the timer*/
          sm_stop_timer(cur_pdp->trans_id);
          /*Send SMREG_DEACTIVATE_CNF to CM*/
          MSG_HIGH_DS_0(SM_SUB, "RABM RELEASE response timer running at Rat Change");
       
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
          if (cur_pdp->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
          {
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
            (cur_pdp->is_primary_pdp && !cur_pdp->teardown_ind.ti_flag && sm_promote_sec_pri(cur_pdp))?
            // Send a Deactivate CNF message to CM with the appropriate cause
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_PROMOTE_CNF, 
            SM_INTERNAL_CAUSE ):
#endif
            // Send a reject message to CM with the appropriate cause
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_CNF, 
                               SM_INTERNAL_CAUSE);
          }
          else{
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
            (cur_pdp->is_primary_pdp && !cur_pdp->teardown_ind.ti_flag && sm_promote_sec_pri(cur_pdp))?
            // Send a Deactivate CNF message to CM with the appropriate cause
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_PROMOTE_IND, 
            SM_NETWORK_CAUSE ):
#endif
            // Send a reject message to CM with the appropriate cause
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, 
                               SM_NETWORK_CAUSE);
          }
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
        }
        else if (cur_pdp->rab_state == RABM_MODIFY_PENDING &&
                 sm_timer_status[cur_pdp->trans_id] == (word) TIMER_RABM_RESP_PENDING)
        {
          /*stop the timer*/
         sm_stop_timer(cur_pdp->trans_id);
         /*Set the cause as RAB_SETUP_FAILURE*/
         cur_pdp->current_cause = (cc_cause_T)RAB_SETUP_FAILURE;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
         /*Send Modify notification to CM*/
         (cur_pdp->mt_orig_type)? sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_IND, SM_INTERNAL_CAUSE):
                                sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_CNF, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
        }
        else
        {
           cur_pdp->rab_state = RABM_DEACTIVATED;
        }
      }/*if (msg->cmd.gmm_rat_change_req.rat == SYS_RAT_UMTS_RADIO_ACCESS)*/
    }
  }/*for (index=0; index<MAX_PDP_NUM; index++)*/
  
#if defined(FEATURE_LTE)
  if(msg->cmd.gmm_rat_change_req.abort_procedure == TRUE)
  {
    MSG_HIGH_DS_0(SM_SUB,"GMMSM_RAT_CHANGE_REQ recevied with Abort Procedure");

    timer_est_pen_flag = FALSE;
    #ifndef FEATURE_DUAL_DATA
    sm_clear_pdp_timer (TIMER_ESTABLISH_PENDING);   
    #endif   
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    for (index=0; index<MAX_PDP_NUM; index++)
    {
      cur_pdp = pdp_list[index];
      if(cur_pdp != NULL)
      {
        /* Send activate reject for all PDPs which are in active pending state*/
        if((cur_pdp->pdp_con_state == SIG_CONNECTION_PENDING 
             && cur_pdp->pdp_state ==PDP_INACTIVE) 
             ||cur_pdp->pdp_state == PDP_ACTIVE_PENDING)
        {
          cur_pdp->current_cause = (cc_cause_T)UE_GW_L_RAT_CHANGE;
          sm_stop_timer((word)index);           

          if (cur_pdp->is_primary_pdp)
          {
            // If primary PDP reject with appropriate cause
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, 
                                                  SM_INTERNAL_CAUSE);
          }
          else
          {
            // If it is a secondary PDP reject with appropriate cause
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_REJ, 
                                                    SM_INTERNAL_CAUSE);
          }
        }
        /*
        ** If the pdp state is inactive pending and it is not primary pdp then send the deactivate rej to CM.
        ** Deactivation of primary pdps will be done later as we have to first deactivate secondary pdps.
        */
        else if(((cur_pdp->pdp_state == PDP_INACTIVE_PENDING)
                   ||((sm_timer_status[cur_pdp->trans_id] == (word) TIMER_EARLY_RAB_FAILURE) && 
                   cur_pdp->pdp_state == PDP_INACTIVE)
                   ) && (cur_pdp->is_primary_pdp == FALSE))
        {
          cur_pdp->current_cause = (cc_cause_T)UE_GW_L_RAT_CHANGE;
          sm_stop_timer((word)index);
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_REJ, 
                                                  SM_INTERNAL_CAUSE);
        }
        else if(cur_pdp->pdp_state  == PDP_MODIFY_PENDING)
        {
          cur_pdp->current_cause = (cc_cause_T)UE_GW_L_RAT_CHANGE;
          sm_stop_timer((word)index);
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_REJ, 
                                                  SM_INTERNAL_CAUSE);
        }
      }                 
    }
    /*
    ** Deactivate if there are any primary pdps with state as inactive pending
    */
    for (index=0; index<MAX_PDP_NUM; index++)
    {
      cur_pdp = pdp_list[index];
      if(cur_pdp != NULL)
      {
        if(((cur_pdp->pdp_state == PDP_INACTIVE_PENDING)
            ||((sm_timer_status[cur_pdp->trans_id] == (word) TIMER_EARLY_RAB_FAILURE) && 
             cur_pdp->pdp_state == PDP_INACTIVE)
            ) && (cur_pdp->is_primary_pdp == TRUE))

        {
          cur_pdp->current_cause = (cc_cause_T)UE_GW_L_RAT_CHANGE;
          sm_stop_timer((word)index);
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_REJ, 
                                                  SM_INTERNAL_CAUSE);
        }
      }
    }
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
#endif

}

/*===========================================================================
FUNCTION SM_PROC_RAT_RESUME

DESCRIPTION
  This function processes the RAT resumption notification from GMM.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sm_process_rat_resume(
#ifdef FEATURE_DUAL_SIM
        sm_cmd_type* cmd_ptr
#else
        void
#endif
)
{
  int index;
  byte best_qos_index;
  pdp_context_info *cur_pdp;
#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
  int index2;
  byte qos_index, group_id;
#endif

#ifdef FEATURE_GSM_GPRS
  ue_rev_level_indicator_type revision ;
#endif

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  ctxt_as_id = cmd_ptr->cmd.gmm_rat_resume_req.as_id;
  if (SM_IS_VALID_AS_ID(cmd_ptr->cmd.gmm_rat_resume_req.as_id) == FALSE)
  {
    MSG_HIGH_DS_2( SM_SUB,"SM AS ID Conflict sm_as_id : %d, message as_id: %d", sm_as_id , cmd_ptr->cmd.gmm_rat_resume_req.as_id);
    send_gmm_rat_resume_cnf();
    ctxt_as_id = SYS_MODEM_AS_ID_NONE;
    return ;
  }
#endif /*#ifdef FEATURE_DUAL_DATA*/
#endif

#ifdef FEATURE_LTE
  sm_abort_pending_esm_proc();
#endif

  /* -----------------------------------------------------
  ** Command RABM to resume its operation on the new RAT
  ** -----------------------------------------------------*/
  if (is_umts_mode())
  {
    sm_notify_rabm ((int)RABMSM_RAT_RESUME_IND, NO_SPECIFIC_NSAPI);
    send_gmm_rat_resume_cnf();
  }

  if (is_gsm_mode())
  {
    /* -----------------------------------------------
    ** Acknowledge RAT resumption confirmation to GMM
    ** -----------------------------------------------*/
    send_gmm_rat_resume_cnf();
  }

  #ifdef FEATURE_GSM_GPRS
  /*If UE did inter RAT selection from R99 to R97 
   * Pick the PDP with best QoS and promote it to primary*/
  revision = sm_get_revision_level_indicator () ;
  MSG_HIGH_DS_1(SM_SUB,"REVISION LEVEL INDICATOR(0) UE_REV_PRE_REL99 %d", revision);
  if (!sm_get_revision_level_indicator())
  {
    for (index=0; index<MAX_PDP_NUM; index++)
    {
      if(pdp_list[index] == NULL)
        continue;

      cur_pdp = pdp_list[index];

#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
      group_id = cur_pdp->group_id ;
      
      MSG_HIGH_DS_1(SM_SUB,"considering PDP trans-id: %d", cur_pdp->trans_id);
      if (cur_pdp->trans_id != INVALID_ID &&
         (cur_pdp->pdp_state == PDP_ACTIVE || cur_pdp->pdp_state == PDP_MODIFY_PENDING) )
      {
        MSG_HIGH_DS_1(SM_SUB,"\tPDP state : %d", cur_pdp->pdp_state);
        best_qos_index  = qos_index=index;
        for (index2=index+1; index2<MAX_PDP_NUM; index2++)
        {
          if (pdp_list[index2] != NULL && 
            (pdp_list[index2]->group_id == group_id) &&
            (pdp_list[index2]->pdp_state == PDP_ACTIVE || pdp_list[index2]->pdp_state == PDP_MODIFY_PENDING))
          {
            best_qos_index = sm_pick_best_qos(pdp_list[qos_index], pdp_list[index2]);
            MSG_HIGH_DS_1(SM_SUB,"\tBest QoS: %d", best_qos_index);
            if (qos_index == best_qos_index)
            {
              cur_pdp = pdp_list[index2];
            }
            else
            {
              cur_pdp = pdp_list[qos_index];
            }
            qos_index = best_qos_index;

            MSG_HIGH_DS_1(SM_SUB,"Deactivate %d while Inter RAT", cur_pdp->trans_id);
            //Notify RABM about implicit deactivation
            sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, cur_pdp->nsapi.nsapi);
            //Reset the PDP state
            cur_pdp->pdp_state = PDP_INACTIVE;
            //Reset the SIG connection state
            cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;
            
            cur_pdp->current_cause = (cc_cause_T)UE_RAT_CHANGE;
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, SM_NETWORK_CAUSE);
          }
        }
      }
#else
      MSG_HIGH_DS_1(SM_SUB,"considering PDP trans-id: %d", cur_pdp->trans_id);
      if (cur_pdp->trans_id != INVALID_ID &&
         cur_pdp->pdp_state != PDP_INACTIVE)
      {
        //Check the group and promote PDPs
        /*---------------------------------------------------------------------
        Note: Currently promotion happens considering one secondary per primary
        Later it will be extended to multiple secondaries per primary
        ----------------------------------------------------------------------*/
        MSG_HIGH_DS_1(SM_SUB,"\tPDP state : %d", cur_pdp->pdp_state);
        if (!cur_pdp->is_primary_pdp)
        {
          
          best_qos_index = sm_pick_best_qos(pdp_list[cur_pdp->linked_ti], cur_pdp);
          MSG_HIGH_DS_1(SM_SUB,"\tSecondary PDP; Best QoS: %d", best_qos_index);

          if (best_qos_index == cur_pdp->trans_id)
          {
            if (cur_pdp->pdp_state == PDP_INACTIVE_PENDING)
            {
              MSG_HIGH_DS_0(SM_SUB,"Setting TearDown Flag to TRUE (Spl case R99 and Secondary in INACTIVE PENDING)");
              cur_pdp->teardown_ind.ti_flag = TRUE;
            }
            else
            {
              MSG_HIGH_DS_1(SM_SUB,"Promote %d to primary during Inter RAT", cur_pdp->trans_id);
              cur_pdp->is_primary_pdp = TRUE;
              pdp_list[cur_pdp->linked_ti]->is_primary_pdp = FALSE;
              pdp_list[cur_pdp->linked_ti]->linked_ti      = cur_pdp->trans_id;
              pdp_list[cur_pdp->linked_ti]->current_cause = (cc_cause_T)UE_RAT_CHANGE;
              //Notify RABM about implicit deactivation
              sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, 
                            pdp_list[cur_pdp->linked_ti]->nsapi.nsapi);
              //Reset the PDP state
              pdp_list[cur_pdp->linked_ti]->pdp_state = PDP_INACTIVE;
              //Reset the SIG connection state
              pdp_list[cur_pdp->linked_ti]->pdp_con_state = SIG_CONNECTION_NONE;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
              sm_send_cm_confirm(pdp_list[cur_pdp->linked_ti], 
                      SMREG_PDP_DEACTIVATE_PROMOTE_IND, SM_NETWORK_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
              //change the linked ti to it's own TI
              cur_pdp->linked_ti = cur_pdp->trans_id;
            }
          }
          else
          {
            MSG_HIGH_DS_1(SM_SUB,"Deactivate %d while Inter RAT", cur_pdp->trans_id);
            //Notify RABM about implicit deactivation
            sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, cur_pdp->nsapi.nsapi);
            //Reset the PDP state
            cur_pdp->pdp_state = PDP_INACTIVE;
            //Reset the SIG connection state
            cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;
            
            cur_pdp->current_cause = (cc_cause_T)UE_RAT_CHANGE;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, SM_NETWORK_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
          }
        }//if (!cur_pdp->is_primary_pdp)..       
      }//if (cur_pdp->pdp_state != PDP_INACTIVE)..
#endif
    }//for (index=0; index<MAX_PDP_NUM...
  }//if (!sm_get_revision_level_indicator())..
#endif
  for (index=0; index<MAX_PDP_NUM; index++)
  {
    cur_pdp = pdp_list[index];
    if (cur_pdp != NULL)
    {
    #ifdef FEATURE_GSM_GPRS
        /*If we already received PDP ACCEPT in UMTS RAT then send 
          SNSM_ACTIVATE_IND to SNDCP*/
        if (cur_pdp->pdp_state == PDP_ACTIVE &&
            sm_timer_status[cur_pdp->trans_id] == (word)TIMER_RABM_RESP_PENDING)
        {
          /*Send SNSM_ACTIVATE_IND to SNDCP*/
          sm_stop_timer(cur_pdp->trans_id);
    
          if ( is_gsm_mode () && 
              cur_pdp->llc_sapi.valid && cur_pdp->radio_pri.valid)
          {
    
            sm_send_sndcp_cmd (cur_pdp, GSNSM_ACTIVATE_IND);  
    
            /* Set the llc_state to LLC_CONFIRM_PENDING */
            cur_pdp->llc_state = LLC_MO_ACTIVATE_CNF_PENDING;
          }
    
        }
        else if (cur_pdp->pdp_state == PDP_ACTIVE ||
            cur_pdp->pdp_state == PDP_MODIFY_PENDING ||
            cur_pdp->pdp_state == PDP_INACTIVE_PENDING)
        {
        
          /* ------------------------------------------------------------------
          ** Check if the UE is running in GPRS mode of operation and send
          ** GSNSM_ACTIVATE_IND to the SNDCP.
          ** -----------------------------------------------------------------*/
          if (is_gsm_mode () && 
              cur_pdp->llc_sapi.valid && cur_pdp->radio_pri.valid )
          {
            sm_send_sndcp_cmd (cur_pdp, GSNSM_MODIFY_IND);  
            cur_pdp->llc_state = LLC_MO_MODIFY_CNF_PENDING ;
          }    
        }
    #endif /*FEATURE_GSM_GPRS*/
    }
  }
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  ctxt_as_id = SYS_MODEM_AS_ID_NONE;
#endif
#endif
}
/*===========================================================================
FUNCTION SM_PROC_SM_STATUS 

DESCRIPTION
  This function process the SM_STATUS message from Network.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  NOne
===========================================================================*/
void sm_proc_sm_status(pdp_context_info *cur_pdp)
{
  byte con_id;
  if(cur_pdp->pdp_state == PDP_INACTIVE)
  {
    // If the PDP state is already INACTIVE report error
    MSG_ERROR_DS_2( SM_SUB,"Received SM_STATUS in wrong state %d for NSAPI:%d",cur_pdp->pdp_state,
          cur_pdp->nsapi.nsapi);
  }
  else 
  {
    MSG_HIGH_DS_2(SM_SUB," Recd SM_STATUS %d for NSAPI: %d",cur_pdp->status_message_cause,
                    cur_pdp->nsapi.nsapi);
    switch(cur_pdp->status_message_cause)
    {
#ifdef FEATURE_GSM_GPRS
      case SM_LLC_SNDCP_FAILURE:
#endif
      case SM_REGULAR_DEACTIVATION:
      case SM_INVALID_TRANS_ID:
      case SM_PROTOCOL_ERR:
       /* Copy status message cause to current cause for indication to higher layers*/
       cur_pdp->current_cause = cur_pdp->status_message_cause;
        /*Perform local deactivation in case of UE receives SM status message*/
       sm_perform_local_pdp_deactivation(cur_pdp);
       break;
      case SM_SEMANTIC_INCORRECT_MESG:
      case SM_INVALID_MANDATORY_INFO:
      case SM_MESG_TYPE_NON_EXISTENT:
      case SM_MESG_TYPE_NOT_COMPATIBLE:
       /* Copy status message cause to current cause for indication to higher layers*/
       cur_pdp->current_cause = cur_pdp->status_message_cause; 
       /* Abort the ongoing procedure*/
       sm_stop_timer (cur_pdp->trans_id);
       con_id = cur_pdp->trans_id;
       if (cur_pdp->pdp_state == PDP_ACTIVE_PENDING)
        {
           pdp_list[con_id]->pdp_state = PDP_INACTIVE;

           //Reset the PDP con state
           pdp_list[con_id]->pdp_con_state = SIG_CONNECTION_NONE;

           // Reset the sm_max_timer_count
           sm_max_timer_count[cur_pdp->trans_id] = 0;

           // Set the cause to appropriate reject cause value.
           pdp_list[con_id]->current_cause = cur_pdp->current_cause;

           // Close PS Signaling Session 
           MSG_HIGH_DS_0(SM_SUB, "Calling GMM Close Session Req");
           send_gmm_close_session_req();

#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
           if (pdp_list[con_id]->is_primary_pdp)
           {
             // If primary PDP rejcet with appropriate cause
             sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_ACTIVATE_REJ, 
                              SM_INTERNAL_CAUSE);
           }
           else
           {
             // If it is a secondary PDP reject with appropriate cause
             sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_ACTIVATE_SEC_REJ, 
                                SM_INTERNAL_CAUSE);
           }
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
        }
       else if ( cur_pdp->pdp_state == PDP_MODIFY_PENDING )
        {

           // Reset the sm_max_timer_count
           sm_max_timer_count[con_id] = 0;

           pdp_list[con_id]->pdp_state = PDP_ACTIVE;

           // Set the cause to appropriate value, 
           pdp_list[con_id]->current_cause = cur_pdp->current_cause;

           // Close PS Signaling Session
           MSG_HIGH_DS_0(SM_SUB, "Calling GMM Close Session Req");
           send_gmm_close_session_req();

           // Send a reject message to CM with the appropriate cause
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
           sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_MODIFY_REJ, 
                              SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
        }
       else if (cur_pdp->pdp_state == PDP_INACTIVE_PENDING )
        {
          pdp_context_info *pri_pdp = NULL;
          word index;
          byte group_id = pdp_list[con_id]->group_id;
          boolean ti_valid = pdp_list[con_id]->teardown_ind.valid;
          boolean ti_flag = pdp_list[con_id]->teardown_ind.ti_flag;
          cc_cause_T cause = cur_pdp->current_cause;

          // Reset the sm_max_timer_count
          sm_max_timer_count[con_id] = 0;
          group_id = pdp_list[con_id]->group_id ;
          if (ti_valid && ti_flag)
          {
            for (index = 0; index < MAX_PDP_NUM; index++)
            {
              if (pdp_list[index] != NULL && 
                   group_id == pdp_list[index]->group_id)
              {
                // Set the cause to appropriate value, 
                pdp_list[index]->current_cause = cause;
                      /* Force an SNDCP Deactivation. */
#ifdef FEATURE_GSM_GPRS
                if (pdp_list[index]->llc_state != LLC_NOT_CONFIRMED &&
                        pdp_list[index]->pdp_state != PDP_INACTIVE )
                {
                  if (pdp_list[index]->is_primary_pdp)
                  {
                    pri_pdp = pdp_list[index];
                  }
                  else
                  {
                    pdp_list[index]->llc_state = LLC_MO_DEACTIVATE_CNF_PENDING;
                    sm_send_sndcp_cmd (pdp_list[index], GSNSM_DEACTIVATE_IND);  
                    pdp_list[index]->pdp_state = PDP_INACTIVE;
                  }
                }
                else
#endif /*FEATURE_GSM_GPRS*/
                {
                  pdp_list[index]->pdp_state = PDP_INACTIVE;
                   //Reset the PDP con state
                  pdp_list[index]->pdp_con_state = SIG_CONNECTION_NONE;
                  if (pdp_list[index]->is_primary_pdp)
                  {
                    pri_pdp = pdp_list[index];                    
                  }
                  else
                  {
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
                    if (index == con_id)
                    {
                       // Send a reject message to CM with the appropriate cause
                      sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_REJ, 
                                                            SM_INTERNAL_CAUSE);
                    }
                    else
                    {
                      // Send a reject message to CM with the appropriate cause
                      sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, 
                                                            SM_INTERNAL_CAUSE);
                    }
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
                  }
                }//end if(is_umts_mode())..               
              }//end if (pdp_list[con_id].teardown_ind.ti_flag)..
            }//end for(index = 0  ...            
            //Send  primary now
            if (pri_pdp)
            {
#ifdef FEATURE_GSM_GPRS
              if(pri_pdp->llc_state != LLC_NOT_CONFIRMED &&
                    pri_pdp->pdp_state != PDP_INACTIVE)
              {
               pri_pdp->llc_state = LLC_MO_DEACTIVATE_CNF_PENDING;
               sm_send_sndcp_cmd (pri_pdp, GSNSM_DEACTIVATE_IND);
               pri_pdp->pdp_state = PDP_INACTIVE;
              }
              else
#endif                                     
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
              if (pri_pdp == pdp_list[con_id])
              {
                 // Send a reject message to CM with the appropriate cause
                sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_REJ, 
                                                      SM_INTERNAL_CAUSE);
              }
              else
              {
                // Send a reject message to CM with the appropriate cause
                sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_IND, 
                                           SM_INTERNAL_CAUSE);
              }
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
            }
          }
          //If TI flag is off deactivate one PDP
          else
          {
            if (pdp_list[con_id]->llc_state != LLC_NOT_CONFIRMED)
            {
  #ifdef FEATURE_GSM_GPRS
              /* Set the llc_state to LLC_CONFIRM_PENDING, start RABM insanity timer */
              pdp_list[con_id]->llc_state = LLC_MO_DEACTIVATE_CNF_PENDING;
              if (pdp_list[con_id]->pdp_state != PDP_INACTIVE)
              {
                  sm_send_sndcp_cmd (pdp_list[con_id], GSNSM_DEACTIVATE_IND);  
                  pdp_list[con_id]->pdp_state = PDP_INACTIVE;
              }
  #endif
            }
            else
            {
              pdp_list[con_id]->pdp_state = PDP_INACTIVE;
      
              //Reset the PDP con state
              pdp_list[con_id]->pdp_con_state = SIG_CONNECTION_NONE;
      
              // Reset the sm_max_timer_count
              sm_max_timer_count[con_id] = 0;
        
              // Set the cause to appropriate reject cause value.
              pdp_list[con_id]->current_cause = cur_pdp->current_cause;
              
              // Send a reject message to CM with the appropriate cause
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
              if (pdp_list[con_id]->is_primary_pdp && !pdp_list[con_id]->teardown_ind.ti_flag && sm_promote_sec_pri(pdp_list[con_id]))
              {
                  sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_DEACTIVATE_PROMOTE_CNF, 
                               SM_INTERNAL_CAUSE);
              }
              else
              {
#endif
                  sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_DEACTIVATE_REJ, 
                               SM_INTERNAL_CAUSE);
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
              }
#endif
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
            }

          }
          send_gmm_close_session_req();
        }
#ifdef FEATURE_MBMS
        #error code not present
#endif
       break;
     default:
      break;
    }//end of switch
  }//end of if(cur_pdp->pdp_state  ==....)
  
}

/*===========================================================================
FUNCTION SM_HANDLE_QOS_VALIDATION_FAILURE

DESCRIPTION
  This function handles the QoS validation failure and initiates the PDP 
  deactivation procedures.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  The associated PDP context (which caused the QoS validation failure) will be 
  deactivated.
===========================================================================*/
void sm_handle_qos_validation_failure 
(
  pdp_context_info *pdp
)
{
  /* set the current pending CM message */
  //pdp->cm_pending_msg = (int)SMREG_PDP_DEACTIVATE_REQ;

  pdp->qos_valid = FALSE;

  // Set the RABM response state for this PDP context to RABM_INITIAL
  pdp->rab_state = RABM_INITIAL;

  if (pdp->pdp_state == PDP_ACTIVE)
  {
    // Set the cause of the deactivation in the PDP context
    pdp->current_cause = (cc_cause_T)SM_QOS_NOT_ACCEPTED;

    MSG_HIGH_DS_1(SM_SUB,"PDP De-Activate Req for NSAPI = %d", pdp->nsapi.nsapi);

    /* ----------------------------------------------------------------------
    ** Call sm_continue_pdp_deactivate function, this function will send the
    ** Signaling message to GMM if the Signaling connection is available. 
    ** Otherwise an ESTABLISH_REQ is sent to GMM for a signaling connection.
    ** ---------------------------------------------------------------------*/
#ifdef FEATURE_MBMS
    #error code not present
#endif
    sm_continue_pdp_deactivate(pdp);
  }
}

/*===========================================================================
FUNCTION sm_proc_deact_req

DESCRIPTION
  This function process PDP deactivation when requested by network
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  The associated PDP context (which caused the QoS validation failure) will be 
  deactivated.
===========================================================================*/
void sm_proc_deact_req
(
  pdp_context_info *cur_pdp
)
{
  /* Stop the Pending Timers in case the PDP state is in 
           INACTIVE/MODIFY Pending */

  if (cur_pdp->pdp_state == PDP_INACTIVE_PENDING)
  {
    sm_stop_timer (cur_pdp->trans_id);
  }
  else if (cur_pdp->pdp_state == PDP_MODIFY_PENDING)
  {
    sm_stop_timer (cur_pdp->trans_id);
  }

  //Reset the PDP con state
  cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;
  // Set the mt_orig_type to true for Mobile terminated request
  cur_pdp->mt_orig_type = TRUE;
#ifdef FEATURE_MBMS
  #error code not present
#endif
#ifdef FEATURE_GSM_GPRS
  /* ------------------------------------------------------------------
  ** Check if the UE is running in GPRS mode of operation and send
  ** GSNSM_DEACTIVATE_IND to the SNDCP.
  ** -----------------------------------------------------------------*/
  if (cur_pdp->llc_state != LLC_NOT_CONFIRMED)
  {
    /*Send deactivate indication to SNDCP*/
    if (cur_pdp->pdp_state != PDP_INACTIVE)
    {
      sm_send_sndcp_cmd (cur_pdp, GSNSM_DEACTIVATE_IND);  
    }

    cur_pdp->pdp_state = PDP_INACTIVE;

    /* Set the llc_state to LLC_CONFIRM_PENDING, start RABM insanity timer */
    cur_pdp->llc_state = LLC_MT_DEACTIVATE_CNF_PENDING;


    /* ---------------------------------------------------------------------
    ** The PDP context has been deactivated, but LLC RAB has to be established
    ** before sending the SMREG_PDP_DEACTIVATE_IND primitive to CM.
    ** --------------------------------------------------------------------*/      
    MSG_HIGH_DS_1(SM_SUB,"PDP %d DEACTIVATED, Waiting LLC Confirm ",cur_pdp->trans_id);
  }     
  else
#endif /*FEATURE_GSM_GPRS*/
  {
     // set pdp context to INACTIVE
     cur_pdp->pdp_state = PDP_INACTIVE;

    /* -------------------------------------------------------------------
    ** No need to wait for RABM response if the RAB corresponding to this
    ** PDP context is already established, otherwise wait for the RABM 
    ** response that indicates the RAB is established.
    ** ------------------------------------------------------------------*/
#ifdef FEATURE_MBMS
#error code not present
#endif

     if (!rabm_rab_established(cur_pdp->nsapi.nsapi))
     {
       MSG_MED_DS_2( SM_SUB,"RAB is not established for the PDP - %d, RAB - %d",cur_pdp->trans_id,cur_pdp->nsapi.nsapi);
 
       sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, cur_pdp->nsapi.nsapi);
       // Send the SMREG_PDP_DEACTIVATE_IND to the CM for Mobile terminated req.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
       if (!cur_pdp->teardown_ind.ti_flag 
           && cur_pdp->is_primary_pdp 
           && sm_promote_sec_pri(cur_pdp))
       {
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_PROMOTE_IND,
                                                        SM_NETWORK_CAUSE);
       }
       else
       {
#endif
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, SM_NETWORK_CAUSE);
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
       }
#endif
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
     }
     else
     {
       // Inform the RABM that the PDP context is being deactivated
       MSG_HIGH_DS_1(SM_SUB, "Inform RABM PDP Nsapi %d deactivated",cur_pdp->nsapi.nsapi);
       cur_pdp->rab_state = RABM_INACTIVE_PENDING;
       sm_notify_rabm((int)RABMSM_DEACTIVATE_IND, cur_pdp->nsapi.nsapi);
       // Wait for the RABM to inform about RAB releases.  
       
 
       // Set the rab_state to RAB_DEACTIVATE_PENDING and start RABM insanity timer.
       cur_pdp->rab_state = RABM_INACTIVE_PENDING;
 
        /* Set the RAB Response Timer and wait for the RABM to respond */
       sm_start_timer(cur_pdp->trans_id, TIMER_RABM_RESP_PENDING); 
 
       /* Wait for the RABMSM_RELEASE_RSP primitive from RABM*/
       MSG_HIGH_DS_1(SM_SUB, "PDP %d MT Deactivate, wait for RABM",cur_pdp->trans_id);
     }
  }
}

/*===========================================================================
FUNCTION SM_SEND_SM_STATUS_ON_RX_EXTENDED_TID

DESCRIPTION
  This function sends SM_STATUS with transaction ID set to '111'. This function
  should be called only when MS receives an SM PDU from the network with extended 
  transaction ID.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  NOne
===========================================================================*/
void sm_send_sm_status_on_rx_extended_tid ( word transactionID )
{
  xlate_status_T status;
  word mesg_bit_len;
  byte l3_buf[MAX_OCTETS_L3_MESSAGE]; /* Buffer to store the packed L3 msg*/

  if ( transactionID >= INVALID_EXTENDED_TID ) // later change this value to include Ext_TI
  {
    /* Pack the Layer 3 Message to be sent to GMM */
    status = pack_sm_status_info(&mesg_bit_len, l3_buf, 
                   transactionID, (word)SM_INVALID_TRANS_ID);

    if (status == L3_PARSE_SUCCESS)
    {
      send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);
    }
    else
    {
      MSG_ERROR_DS_0( SM_SUB,"Unable to pack the SM-STATUS message");
    }
  }
  else
  {
    MSG_ERROR_DS_1( SM_SUB,"Invalid Transaction Id: %d", transactionID);
  }
}

/*===========================================================================
FUNCTION SM_VALIDATE_DUPLIACTE_COLLISION_PDP

DESCRIPTION
  This function validates the REQUEST PDP ACTIVATION and verifies the message has to
   be handled or not depending on the state of the duplicate PDP.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Return TRUE in case the request has to be handled and FALSE otherwise

SIDE EFFECTS
  NOne
===========================================================================*/
boolean sm_validate_dupliacte_collision_pdp
(
 pdp_context_info *pdp
)
{
  byte con_id;
  if ((con_id = sm_check_pdp_collision(pdp)) != INVALID_ID)
  {
    if (pdp_list[con_id]->pdp_state == PDP_INACTIVE &&
        pdp_list[con_id]->mt_orig_type)
    {
      MSG_HIGH_DS_2(SM_SUB,"Dup PDP %d, deactivate old one %d",pdp_list[con_id]->trans_id,pdp->trans_id);
      /*In case already MT is in processing but haven't sent the 
        ACTIVATE_PDP_CXT_REQ then deactivate the old one and proceed
        with new*/
      //Send RABMSM_DEACTIVATE_IMPLICIT_IND to RABM
      sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, pdp->nsapi.nsapi);

      // Send the SMREG_PDP_DEACTIVATE_IND to the CM.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm(pdp_list[con_id], SMREG_PDP_DEACTIVATE_IND,
                         SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      return TRUE;       
    }

#ifdef FEATURE_MBMS
    #error code not present
#endif

    else if (pdp_list[con_id]->pdp_state == PDP_ACTIVE_PENDING)
    {
      MSG_HIGH_DS_1(SM_SUB,"Dup MT PDP received while MO req gone, drop MT %d",pdp->trans_id);
      /*Drop the message and proceed with old one
      ** Refer 24.008 6.1.3.1.5(b)
      */
      // pdp_list[pdp->trans_id]->trans_id = INVALID_ID;
      if (pdp->nsapi.valid)
      (void)sm_release_nsapi_sm (pdp->trans_id);


      // sm_unreserve_connection_id_int(pdp->trans_id);
      return FALSE;
    }
    else if (pdp_list[con_id]->pdp_state == PDP_ACTIVE ||
             pdp_list[con_id]->pdp_state == PDP_MODIFY_PENDING)
    {
      word index;
      pdp_context_info *pri_pdp = NULL;
      byte group_id = pdp_list[con_id]->group_id;

      MSG_HIGH_DS_1(SM_SUB,"Dup MT PDP received while PDP is active, deactivate old %d",pdp->trans_id);
      /* Deactivate the old PDP context locally and proceed with new
      ** Refer 24.008 6.1.3.1.5(c)
      */
      /*Deactivate all the secondary contexts first*/
      for (index = 0; index < MAX_PDP_NUM; index++)
      {
        if (pdp_list[index] != NULL && pdp_list[index]->group_id == group_id)
        {
          if (pdp_list[index]->is_primary_pdp)
          {
            pri_pdp = pdp_list[index];
          }
          else{
            sm_perform_local_pdp_deactivation(pdp_list[index]);
          }
        }
      }

      /*Now Deactivate primary*/
      if (pri_pdp)
      {
        sm_perform_local_pdp_deactivation(pri_pdp);
      }
      return TRUE;
    }
  }
  else 
  {
    word index;
    word mesg_bit_len;
    byte l3_buf[MAX_OCTETS_L3_MESSAGE]; /* Buffer to store the packed L3 msg*/
    xlate_status_T status;
 
    for (index = 0; index < MAX_PDP_NUM; index++)
    {
      if ( pdp_list[index] != NULL &&
           (pdp_list[index]->pdp_state == PDP_ACTIVE_PENDING) && (!pdp_list[index]->mt_orig_type) )
      {
        if (!pdp->apn_name.valid || pdp->pdp_addr_alloc_type == PDP_ADDR_DYNAMIC ||
          !pdp_list[index]->apn_name.valid || pdp_list[index]->pdp_addr_alloc_type == PDP_ADDR_DYNAMIC || 
          pdp->pdp_addr.pdp_type_org == 0x0F)
        {
          MSG_HIGH_DS_1(SM_SUB,"Rejecting MT Request, MO Activate in progress, MO/MT collision, cannot compare params MO TI =%d", 
            pdp_list[index]->trans_id);

          //set the cause to SM_INSUFFICIENT_RESOURCES
          pdp->current_cause = (cc_cause_T) SM_INSUFFICIENT_RESOURCES;
          
          /* Pack the Layer 3 Message and send it to GMM */
          status = pack_layer3_mesg(&mesg_bit_len, l3_buf, pdp, REQUEST_PDP_ACTIVATION_REJ);
          if (status == L3_PARSE_SUCCESS)
          {
            // Send the packed SM-PDU to the GMM to be sent to the Network
            send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);
          }
          else
          {
            MSG_ERROR_DS_0( SM_SUB, "Error in Encoding the Request PDP activation reject msg");
          }
          
          // pdp_list[pdp->trans_id]->trans_id = INVALID_ID;
          // clear the PDP resources
          if (pdp->nsapi.valid)
          (void)sm_release_nsapi_sm (pdp->trans_id);

           // sm_unreserve_connection_id_int(pdp->trans_id);

          return FALSE;
        }
      }
    }
  }

  return TRUE;
}

/*===========================================================================
FUNCTION SM_ABORT_GMM_CONN_PROC

DESCRIPTION
  Aborts GMM connection procedure

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  NOne
===========================================================================*/
void sm_abort_gmm_conn_proc(cc_cause_T abort_cause)
{
  word i;
  word index;

  /* Clear the establishment timer. */
  #ifndef FEATURE_DUAL_DATA
  sm_clear_pdp_timer(TIMER_ESTABLISH_PENDING);
  #endif

  //Reset the timer_est_pen flag
  timer_est_pen_flag = FALSE;
  
  // Set the Signaling Connection variable as False
  sig_connection_available = FALSE;
  max_est_timer_count = 0;
  for (i = 0; i < MAX_PDP_NUM; i++)
  {
    if (pdp_list[i] != NULL && 
         pdp_list[i]->pdp_con_state == SIG_CONNECTION_PENDING)
    {
      pdp_list[i]->current_cause = abort_cause;

      #ifndef FEATURE_DUAL_DATA
      sm_stop_timer(i);
      #endif

      //Notify RABM 
      if (is_umts_mode())
      {
        sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, pdp_list[i]->nsapi.nsapi);
      }
      
      //Process the CM pending request if any
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      if (pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_REQ)
      {
        sm_send_cm_confirm(pdp_list[i], SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);
      }
      else if (pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_SEC_REQ)
      {
        sm_send_cm_confirm(pdp_list[i], SMREG_PDP_ACTIVATE_SEC_REJ, SM_INTERNAL_CAUSE);
      }
      else 
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      if (pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ)
      {
         pdp_context_info *pri_pdp = NULL;
        //Check if the tear down ind is set for this PDP
        if( pdp_list[i]->teardown_ind.ti_flag)
        {
           byte group_id = pdp_list[i]->group_id;
          // deactivate secondary PDPs first
          for (index = 0; index < MAX_PDP_NUM; index++)
          {
            if(pdp_list[index] == NULL)
              continue;
            if (group_id == pdp_list[index]->group_id
              &&  i !=  index
                )          
            {
#ifdef FEATURE_DUAL_DATA
              sm_stop_timer(index);
#endif
              if (pdp_list[index]->is_primary_pdp)
              {
                 //Here TI flag was set for Secondary PDP,,Store the pri_pdp address to release at end
                  pri_pdp = pdp_list[index];
              }
              else
              {
#ifdef FEATURE_GSM_GPRS
                if (pdp_list[index]->llc_state != LLC_NOT_CONFIRMED)
                {
                  sm_send_sndcp_cmd(pdp_list[index], GSNSM_DEACTIVATE_IND);
                }            
#endif /*FEATURE_GSM_GPRS*/
                if (is_umts_mode())
                {
                   sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, pdp_list[index]->nsapi.nsapi);
                }
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
               pdp_list[index]->current_cause = abort_cause;
               sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_REJ, SM_INTERNAL_CAUSE);          
              } //else is_primary_pdp
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
             }
          } //if teardown_ind.ti_flag
        }
#ifdef FEATURE_GSM_GPRS
        if (pdp_list[i]->llc_state != LLC_NOT_CONFIRMED)
        {
          sm_send_sndcp_cmd(pdp_list[i], GSNSM_DEACTIVATE_IND);
        }
#endif /*FEATURE_GSM_GPRS*/
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
        sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
        //Now Deactivate the Primary PDP
         if( pri_pdp )
         {
#ifdef FEATURE_GSM_GPRS
           if (pri_pdp->llc_state != LLC_NOT_CONFIRMED)
           {
              sm_send_sndcp_cmd(pri_pdp, GSNSM_DEACTIVATE_IND);
           }
#endif /*FEATURE_GSM_GPRS*/
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
          pri_pdp->current_cause = abort_cause;
          sm_send_cm_confirm(pri_pdp, SMREG_PDP_DEACTIVATE_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
        }

      }
      else if (pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_MODIFY_IND)
      {
        //Set the PDP state to ACTIVE
        pdp_list[i]->pdp_state = PDP_ACTIVE;
      }
      else if (pdp_list[i]->cm_pending_msg == (int)MS_MODIFY_PDP_ACCEPT ||
                  pdp_list[i]->cm_pending_msg == (int)MODIFY_PDP_REJECT)
      {
        //Set the PDP state to ACTIVE
        pdp_list[i]->pdp_state = PDP_ACTIVE;
      }       
#ifdef FEATURE_MBMS
#error code not present
#endif
      else if(pdp_list[i]->cm_pending_msg == (int)SMREG_PDP_GRACEFUL_ABORT_REQ)
      {
        byte group_id = pdp_list[i]->group_id;
       // deactivate secondary PDPs first
        for (index = 0; index < MAX_PDP_NUM; index++)
        {
          if(pdp_list[index] == NULL)
            continue;
          if (group_id == pdp_list[index]->group_id
            &&  i !=  index
              )          
          {
#ifdef FEATURE_GSM_GPRS
            if (pdp_list[index]->llc_state != LLC_NOT_CONFIRMED)
            {
              sm_send_sndcp_cmd(pdp_list[index], GSNSM_DEACTIVATE_IND);
            }            
#endif /*FEATURE_GSM_GPRS*/
            if (is_umts_mode())
            {
              sm_notify_rabm((int)RABMSM_DEACTIVATE_IMPLICIT_IND, pdp_list[index]->nsapi.nsapi);
            }
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
            sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);          
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
          }
        }

        // Deactivate primary now
#ifdef FEATURE_GSM_GPRS
        if (pdp_list[i]->llc_state != LLC_NOT_CONFIRMED)
        {
          sm_send_sndcp_cmd(pdp_list[i], GSNSM_DEACTIVATE_IND);
        }
#endif /*FEATURE_GSM_GPRS*/
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
        sm_send_cm_confirm(pdp_list[i], SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      }
    }
  }
}

/*===========================================================================
FUNCTION SM_IS_PDP_ACTIVE

DESCRIPTION
  This function returns TRUE if any PDP Context is active in SM.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  NOne
===========================================================================*/
boolean sm_is_pdp_active
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id
#else
  void
#endif
)
{
  word i;
  boolean status = FALSE; 

#ifdef FEATURE_DUAL_SIM
  #ifdef FEATURE_DUAL_DATA
  if(SM_IS_VALID_AS_ID(as_id) == FALSE)
  {
    return status;
  }
  #else

  if (as_id != sm_as_id)
  {
    return status;
  }
 #endif /*#ifdef FEATURE_DUAL_DATA*/
#endif

  for (i=0; i<MAX_PDP_NUM; i++)
  {
#ifdef FEATURE_DUAL_DATA
    if (pdp_list_sim[as_id][i] != NULL && pdp_list_sim[as_id][i]->pdp_state != PDP_INACTIVE)
#else
    if (pdp_list[i] != NULL && pdp_list[i]->pdp_state != PDP_INACTIVE)
#endif
    {
      status = TRUE;
      break;
    }
  }

  return status;
}

/*===========================================================================
FUNCTION SM_IS_PDP_ACTIVATION_PENDING

DESCRIPTION
  This function returns TRUE if any PDP Context is active in SM.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  NOne
===========================================================================*/
boolean sm_is_pdp_activation_pending
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id
#else
  void
#endif
)
{
  word i;
  boolean status = FALSE;

#ifdef FEATURE_DUAL_SIM
   if(SM_IS_VALID_AS_ID(as_id) == FALSE)
   {
     return status;
   }
#endif
  rex_enter_crit_sect(&sm_crit_sect);

  for (i=0; i<MAX_PDP_NUM; i++)
  {
#ifdef FEATURE_DUAL_DATA
    if (pdp_list_sim[as_id][i] != NULL && pdp_list_sim[as_id][i]->pdp_con_state == SIG_CONNECTION_PENDING)
#else
    if (pdp_list[i] != NULL && pdp_list[i]->pdp_con_state == SIG_CONNECTION_PENDING)
#endif
    {
      status = TRUE;
      break;
    }
  }
  rex_leave_crit_sect(&sm_crit_sect);
  return status;
}


#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION SM_CHECK_PDP_OBJECT_EXIST

DESCRIPTION
  This function checks any PDP context objects exists

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if match is found FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_check_pdp_object_exist(void)
{
  word index;
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL)
    {
      return TRUE;
    }
        
  }
  return FALSE;
}
#endif

/*===========================================================================
FUNCTION SM_CHECK_PDP_EXIST

DESCRIPTION
  This function checks any active PDP context

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if match is found FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_check_pdp_exist(void)
{
  word index;
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL &&
        pdp_list[index]->trans_id != INVALID_ID &&
        (pdp_list[index]->pdp_state != PDP_INACTIVE || 
        pdp_list[index]->pdp_con_state == SIG_CONNECTION_PENDING)){
      return TRUE;
    }
        
  }
  return FALSE;
}

/*===========================================================================
FUNCTION SM_CHECK_PDP_PROCEDURE_PENDING

DESCRIPTION
  This function checks any PDP procedure is pending

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if match is found FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean sm_check_pdp_procedure_pending()
{
  word index;
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL && 
        pdp_list[index]->trans_id != INVALID_ID && 
       ((pdp_list[index]->pdp_state == PDP_ACTIVE_PENDING) || 
       (pdp_list[index]->pdp_state == PDP_MODIFY_PENDING) ||
       (pdp_list[index]->pdp_state == PDP_INACTIVE_PENDING)))
    {
      return TRUE;
    }        
  }
  return FALSE;
}


/*===========================================================================
FUNCTION SM_MODIFY_RAB

DESCRIPTION
  This function checks if any RAB with traffic class straming or conversational
  is in dormant state and trigger RAB reestablishment

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
void sm_modify_rab(boolean mt_page)
{
  word index, rab_id;
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL)
    {
      rab_id = pdp_list[index]->nsapi.nsapi;
      /*check whether the QoS has trafiic class streaming and conversational*/
      if (pdp_list[index]->trans_id != INVALID_ID &&
          pdp_list[index]->pdp_state == PDP_ACTIVE &&
          ((mt_page && rabm_check_rab_dormant(rab_id)) ||
           (!mt_page && rabm_verify_rab_reest_pend_state(rab_id))) &&
          (pdp_list[index]->qos.traffic_class == (int)STREAMING_CLASS ||
           pdp_list[index]->qos.traffic_class == (int)CONVERSATION_CLASS))
      {
        if(sm_is_apn_blocked(pdp_list[index]->apn_name))
        {
          MSG_HIGH_DS_1( RABM_SUB,"APN blocked for TI %d",index);
        }
        else
        {
          MSG_HIGH_DS_1(SM_SUB,"Sending PDP modification for %d", pdp_list[index]->trans_id);
          pdp_list[index]->cm_pending_msg = (int)SMREG_PDP_MODIFY_IND;
          pdp_list[index]->pdp_state = PDP_MODIFY_PENDING;
          /*Copy the Qos into req_qos*/
          pdp_list[index]->req_qos = pdp_list[index]->qos;
          sm_continue_pdp_modify(pdp_list[index]);
        }
      }
    }
  }
}

/*===========================================================================
FUNCTION SM_PROCESS_CON_REL_REJ

DESCRIPTION
    This function function process ps connection release reject from gmm
DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
void sm_process_con_rel_rej(
#ifdef FEATURE_DUAL_SIM
        sm_cmd_type* cmd_ptr
#else
        void
#endif
)
{
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  if (SM_IS_VALID_AS_ID(cmd_ptr->cmd.gmmsm_con_release_rej.as_id) == FALSE)
  {
    MSG_ERROR_DS_2( SM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , cmd_ptr->cmd.gmmsm_con_release_rej.as_id);
    return ;
  }
#endif
#endif

 if (cm_initiated_ps_signalling_release)
 {
   cm_initiated_ps_signalling_release = FALSE;
   sm_send_cm_ps_sig_rel_confirm(SM_PS_SIG_REL_FAILURE);
 }
}

#ifdef FEATURE_MBMS
#error code not present
#endif

/*===========================================================================
FUNCTION SM_PROCESS_PDP_MBMS_STATUS

DESCRIPTION
  This function processes the PDP CONTEXT STATUS IE from GMM.

DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void sm_process_pdp_mbms_status(const gmmsm_pdp_mbms_context_status_T* gmm_pdp_mbms_context_status)
{
    word nsapi_id;            // nsapi number, being considered
    byte working_byte;   // PDP/MBMS context, being considered
    byte mask[8];             // to mask the PDP/MBMS context byte
    byte i, j;                     // index variables
#ifdef FEATURE_MBMS
    #error code not present
#endif

    // set mask array
    mask[0] = 1;
    mask[1] = 2;
    mask[2] = 4;
    mask[3] = 8;
    mask[4] = 16;
    mask[5] = 32;
    mask[6] = 64;
    mask[7] = 128;

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  if(SM_IS_VALID_AS_ID(gmm_pdp_mbms_context_status->as_id) == FALSE)
  {
    MSG_ERROR_DS_2( SM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , gmm_pdp_mbms_context_status->as_id);
    return ;
  }
#endif
#endif

    // process PDP context Status
    nsapi_id=0;
      for (i = 0 ; i < gmm_pdp_mbms_context_status->pdp_status_length ; i++)
      {
#ifdef FEATURE_MBMS
          #error code not present
#endif
          working_byte = gmm_pdp_mbms_context_status->value[i];
          for (j = 0 ; j < 8 ; j++)
          {
              if (nsapi_id >= MIN_NSAPI_NUM ) // consider only NSAPIs greater than or equal to MIN_NSAPI_NUM
              {
                  if ( (nsapi_list[nsapi_id] != INVALID_ID) && 
                    pdp_list[nsapi_list[nsapi_id]] != NULL
                    && (!  ((pdp_list[nsapi_list[nsapi_id]]->pdp_state == PDP_ACTIVE_PENDING) || 
                                ((pdp_list[nsapi_list[nsapi_id]]->pdp_state == PDP_INACTIVE ) 
                                 &&(pdp_list[nsapi_list[nsapi_id]]->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_REQ ||
                                    pdp_list[nsapi_list[nsapi_id]]->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_SEC_REQ ||
                                    pdp_list[nsapi_list[nsapi_id]]->mt_orig_type == TRUE ))

                      )))
                  {
                      if ((working_byte & mask[j]) == 0)
                      {
                          // Trigger Deactivation for all associated PDPs before deactivating Primary
                          if (pdp_list[nsapi_list[nsapi_id]]->is_primary_pdp)
                          {
                            MSG_HIGH_DS_0(SM_SUB, "Primary PDP being deactivated, Check and Deactivate all associated PDPs");
                            sm_local_deact_all_associated_pdp_ctxt(pdp_list[nsapi_list[nsapi_id]]);
                          }
                          sm_perform_local_pdp_deactivation(pdp_list[nsapi_list[nsapi_id]]);
                      }
                  }
              }
              nsapi_id++;
          }
      }

#ifdef FEATURE_MBMS
    #error code not present
#endif
#if defined(FEATURE_LTE)
    esm_deactivate_bearer_ctxts_irat_to_GW();
#endif

}


#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
/*===========================================================================
FUNCTION SM_CONTINUE_RESEND_PDP_MSG

DESCRIPTION
  This function continues the resending PDP response message to the NW which was thrashed by GRLC

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
void sm_continue_resend_pdp_msg (pdp_context_info *pdp)
{
  word mesg_bit_len;              /* Length of the packed L3 mesg in bits */
  xlate_status_T status;          /* status of the packed L3 message */


  rrc_establish_cause_e_type cause_type = convert_to_rrc_est_cause(pdp->qos.traffic_class);

  /* ----------------------------------------------------------------------
  ** Make sure the RRC Signaling Connection is available, before sending
  ** the SM Signaling message. Request GMM to establish a connection if it
  ** is not available.
  ** ---------------------------------------------------------------------*/
  if (!sig_connection_available)
  {    
    MSG_HIGH_DS_0(SM_SUB, "Send GMMSM_ESTABLISH_REQ, REQ CONN ");
   
#ifdef FEATURE_DUAL_DATA
    if ( (!timer_est_pen_flag) || (sm_timer_status[pdp->trans_id] == (word)SM_TIMER_STOPPED) )
#else
    if ( !timer_est_pen_flag )
#endif
    {
      #ifdef FEATURE_DUAL_DATA
      sm_start_timer(pdp->trans_id, TIMER_ESTABLISH_PENDING);
      #else
      timers_pending[TIMER_PENDING] = pdp->trans_id;
      sm_set_pdp_timer(TIMER_ESTABLISH_PENDING);  
      #endif   

      //Set the timer_est_pen_flag
      timer_est_pen_flag = TRUE;
      
      // Send an Establish Request GMM task to send 
      send_gmm_establish_req(cause_type);    
    }
    else
    {
      max_est_timer_count = 0;
    }
  }
  else  /* Signalling Connection is available*/  
  {
#ifndef FEATURE_MODEM_HEAP
    byte *l3_buf = (uint8*) gs_alloc(gmm_max_L3_msg_length(MM_PS_DOMAIN));
#else
    byte *l3_buf = (uint8*) modem_mem_calloc(1, gmm_max_L3_msg_length(MM_PS_DOMAIN), MODEM_MEM_CLIENT_NAS);
#endif
    sm_check_for_null_ptr((void*)l3_buf );

    /* Pack the Layer 3 Message and send it to GMM */
    status = pack_layer3_mesg(&mesg_bit_len, l3_buf, pdp, pdp->sm_msg_to_resend);
  
    if (status == L3_PARSE_SUCCESS)
    {
      pdp->status_ind_flag = FALSE;
      // Send the packed SM-PDU to the GMM to be sent to the Network
      send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE); 
    }
    else
    {
      MSG_ERROR_DS_1( SM_SUB, "Unable to Encode OTA mesg: %d, INVALID field", pdp->sm_msg_to_resend);
    }
    if(l3_buf != NULL)
    {
#ifdef FEATURE_MODEM_HEAP
      modem_mem_free( l3_buf , MODEM_MEM_CLIENT_NAS);
      l3_buf = NULL;
#else
      gs_free( l3_buf);
      l3_buf = NULL;
#endif
    }
  } /*end of - Signalling Connection is available else*/
}

/*===========================================================================
FUNCTION SM_PROCESS_STATUS_IND

DESCRIPTION
  This function continues the PDP activation for the specific Transaction Id

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
void sm_process_status_ind (byte trans_id)
{
  word restore_max_timer_count;
  if (trans_id != INVALID_ID && pdp_list[trans_id] != NULL) 
  {
    if (pdp_list[trans_id]->pdp_state == PDP_ACTIVE_PENDING)
    {
      restore_max_timer_count = sm_max_timer_count[trans_id];
      sm_stop_timer(trans_id);
      sm_max_timer_count[trans_id] = restore_max_timer_count;
      sig_connection_available = FALSE;
      sm_continue_pdp_activation(pdp_list[trans_id]);
    }
    else if ( (pdp_list[trans_id]->pdp_state == PDP_MODIFY_PENDING ||
                pdp_list[trans_id]->pdp_state == PDP_ACTIVE) && 
          (pdp_list[trans_id]->sm_msg_to_resend == MS_MODIFY_PDP_ACCEPT) )
    {
      sig_connection_available = FALSE;
      pdp_list[trans_id]->status_ind_flag = TRUE;
      sm_continue_resend_pdp_msg (pdp_list[trans_id]);
    }
  }
}
#endif /* #ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM */




#if defined(FEATURE_LTE)
/*===========================================================================
FUNCTION SM_PROCESS_ISR_STATUS_IND

DESCRIPTION
  This function process the ISR status indication from GMM.

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
void sm_process_isr_status_ind(gmmsm_isr_status_ind_t  gmm_isr_status_ind)
{
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  if(SM_IS_VALID_AS_ID(gmm_isr_status_ind.as_id) == FALSE)
  {
    MSG_HIGH_DS_0( SM_SUB,"SM AS ID Conflict GMMSM_ISR_STATUS_IND - processing skipped");
  }
  else
#endif
#endif
  {
    sm_esm_isr_status_set(gmm_isr_status_ind.isr_status);
  }
}

/*===========================================================================
FUNCTION SM_RESET_ISR_PDP_ACT_STATUS

DESCRIPTION
  This function resets the isr pdp activation status of all the pdps

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/

void sm_reset_isr_pdp_act_status (void)
{
  int index;

  for (index=0; index<MAX_PDP_NUM; index++)
  {
    if(pdp_list[index] != NULL)
    {
      pdp_list[index]->isr_pdp_act_status = CTXT_ACT_BEFORE_ISR;
    }
  }
}

#endif

#if defined(FEATURE_LTE) && defined(FEATURE_DUAL_SIM)
/*===========================================================================
FUNCTION SM_PROCESS_MULTIMODE_SUBS_CHGD_REQ

DESCRIPTION
  This function process the multimode subs chgd request.

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
void sm_process_multimode_subs_chgd_req
(
  void
)
{
  word i;
  for (i=0; i<MAX_PDP_NUM; i++)
  {
    if(pdp_list[i] != NULL )
    {
      if(pdp_list[i]->pdp_state == PDP_ACTIVE)
      {
        (void)esm_update_esm_context_from_sm(pdp_list[i]);
      }
    }
  }
}
#endif

