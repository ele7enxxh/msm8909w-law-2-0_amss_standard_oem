/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmlu.c_v   1.31   17 Jul 2002 12:30:04   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmlu.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/11   rajesh  Made code changes to perform LU when there is a change of LA even for emergency
                   CSFB call when NV item "mm_lai_change_force_lau_for_emergency" is set.
08/18/11   abhi    Indicating CSMO flag in LAU request
07/13/11   rajesh  Handling expiry of T3412 and T3423 when ISR is ON
05/14/01   jca     Added service_type parameter to calls to mmr_send functions.
05/15/01   sbk     Removed unnecessary paging control message to update the TMSI
05/25/01   jca     Updated to reflect changes to REG<->MM interface.
06/03/01   sbk     Global variables are renamed to use mm_ prefix
06/03/01   sbk     The selected plmn/lai processing is changed to use mmcoord data
06/13/01   jca     Fixed bug where length field not set in LU request (GSM mode).
06/20/01   sbk     Consolidated LOCI/Ciphering Key management
08/03/01   jca     Now comparing new LAI to the previous serving LAI rather
                   then the stored LAI when SERVICE_IND received.
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
08/15/01   ab      Renamed CM to CNM.
10/19/01   sbk     LU Accept message is fixed to use the correct mobile size
11/28/01   jca     Now setting substate to NORMAL SERVICE when RRC_SERVICE_IND
                   indicates service restored on RPLMN.
12/06/01   jca     Now sending Dynamic ID log packet when TMSI value changes.
12/19/01   jca     Now waiting until LU procedure completed before clearing
                   mm_possible_imsi_attach flag.
02/25/02   jca     Now setting svc_type flag in MMR_SERVICE_IND to indicate
                   normal service if service available on registered PLMN.
06/05/02   ks      Added support for Dual mode operation.
06/18/02   ks      setting the Service status properly in mm_process_service_ind()
06/20/02   jca     Now adding PLMN to national forbidden list when LU Reject
                   message received with cause field set to "PLMN not allowed.
07/12/02   ks      Setting the default values for AVAILABLE/EQUIVALENT list info
                   in the MMR_REG_CNF primitive.
07/17/02   ks      Calling the gs_read_coded_classmark_2() and gs_get_classmark_1
                   for dual mode/ dual band classmark information
08/02/02   ks      Added support for new reject cause #15 NO_SUITABLE_CELLS_IN_LA
                   Now setting proper cause in mmr_reg_cnf & mmr_service_ind.
08/21/02   ks      Sending the MMR_REG_CNF primitive when MM receives a
                   SERVICE_IND with NO SERVICE during a registration procedure.
08/23/02   ks      changed the previous_service_state data type to
                   service_state_type.
09/04/02   vdr     Fixed determination of the CKSN
09/23/02   mks     Now stopping the timer T3213 on receiving SERVICE_IND with
                   service after cell re-selection
10/03/02   mks     Now properly setting the service state field in MMR_REG_CNF
                   primitive while handling location update failure
10/07/02   jca     Changes to support CS/PS combined attach procedures.
10/25/02   mks     Now cancelling any pending authentication transaction at
                   T3210 expiry
11/05/02   mks     Corrected the behavior of is_PLMN_different_from_current()
                   to handle the cases where MNC is only two digit long.
11/06/02   jca     Modified code to set MM PLMN info status through calls to
                   new function mm_update_mm_serving_plmn_info().
11/07/02   mks     Added function mm_get_lu_cause() which returns the cause in
                   the location update request.
11/08/02   cd      In mm_process_service_ind() added support for initiating
                   location updating after access class blocking has been removed
                   In mm_initiate_lu(), added checking for access class blocking
11/12/02   jca     Modified prototype for function mm_abort_rr().
11/14/02   cd      Added msg_high to indicate that follow_on_proceed IEI has been received
11/26/02   jca     Fixed bug in Access Class blocking processing.
12/05/02   jca     Changes to support PS_ONLY<->CS_ONLY mode switches in GSM mode.
12/12/02   ks      Replaced mm_sim_valid with function mmsim_sim_valid.
12/16/02   jca     Renamed mm_waiting_to_send_reg_cnf to mm_ready_to_send_reg_cnf.
01/13/03   jca     Send ind/cnf to REG when processing SERVICE_IND only if ms_op_mode
                   is set to CS_ONLY.
01/31/03   ks      When the UE is in MM_ATTEMPTING_TO_UPDATE substate of the Idle
                   state, start the LU procedure upon camping on a cell.
02/07/03   jca     Fixed bug where MMR_SERVICE_IND indicating LIMITED_SERVICE was
                   generated when a RR(C)_SERVICE_IND was received while LU attempt
                   was in-progress on new PLMN.
                   Added flag to direct REG to remain on same PLMN when indication
                   generated due to LU attempt counter reaching threshhold count.
03/07/03   jca     Now clearing LAU attempt counter when reject cause contained in
                   LAU REJECT message is set to NO_SUITABLE_CELLS_IN_LA.
03/14/03   mks     Added a fix to wait for T3211 expiry before initiating location
                   update when MS in MM_ATTEMPTING_TO_UPDATE substate.
03/18/03   mks     Corrected the processing of service indication so that location
                   update procedure is not performed if the update status is
                   "updated" and the LAI broadcasted on BCCH matches with the LOCI
                   in the SIM.
03/18/03   mks     Added a fix to stop T3213 when there is a LAC change and the
                   serving LAC matches with the LOCI in the SIM.
03/25/03   jca     Now setting mm_ready_to_send_reg_cnf to TRUE when SERVICE_IND
                   indicates SERVICE_NOT_AVAILABLE, mm_reg_waiting_for_reg_cnf == TRUE,
                   and ms_op_mode == CS_PS.
04/03/03   cd      Added support for processing an optional equivalent PLMN list
                   in the LU_ACCEPT msg
04/07/03   cd      Removed premature call to start T3210 and MSG_HIGH reporting a LU_REQ
                   has been sent in a LU procedure.
                   Corrected substate transitions for LU_REJECT with cause #13 and #15
04/08/03   jca     Added new reason code for pending LU request.
04/15/03   jca     Now ignoring T3212 timeout when ms_op_mode == CS/PS and NMO == 1.
04/29/03   jca     Now reset MM substate to NORMAL_SERVICE if SERVICE_IND indicates
                   resumption of service.
05/15/03   mks     Now notifying REG that service is available when MS returns
                   (upon a random access failure while registering on a neighbouring
                   cell) to a location area where it registered earlier.
06/17/03   jca     Set cause field to MMR_CAUSE_NOT_SPECIFIED instead of
                   MMR_CAUSE_FAILURE_REMAIN_ON_PLMN when mm_lu_attempt_counter >= 4
                   and phone is configured for dual mode operation.
06/18/03   jca     When a MM Location Update Reject message is received with cause code
                   #3, #6, #11, #12, #13 or #15, take the additional steps to update
                   GMM registration/SIM status and set the GMM state to DEREGISTERED.
07/08/03   jca     Now allow GMM registration attempt to proceed immediately
                   if MM LU attempt fails 4 times in a row and ms_op_mode is
                   set to CS+PS and network op mode is set to NMO2.
07/21/03   cd      Now sending a service indication to REG when the SIM is invalid,
                   access class restrictions for emergency calls have been
                   removed.
07/23/03   ks      Removed the start of timer 3211 upon receiving LU reject with
                   abnormal causes.  T3211 is started after RR conection release.
07/29/03   jca     Fixed PS Attach-on-demand problem related to clearing of the
                   mm_serv_change_pending boolean flag.
09/18/03   jca     Now resetting mm_pending_lu_request within mm_initiate_lu().
                   Now calling mm_initiate_lu() instead of
                   mm_state_control( MM_STD_LU_REQUIRED ).
09/25/03   ks      If MM is in Limited Service due to REG request, do not change
                   the substate when the Cell has changed to different LA.
10/02/03   cd      In mm_handle_location_updating() corrected timer actions when
                   the FOP flag is set
10/09/03   mks     Corrected MM behavior duirng LU reject to comply with special
                   clauses outlined in appendix G of TS24.008.
10/10/03   cd      Added check for EPLMN when a new PLMN is reported in a service_ind
10/21/03   cd      Now rejecting a pending call request if the LU procedure
                   does not complete successfully
11/06/03   jca     Fixed bug where we failed to perform an IMSI Attach if we handed
                   off (while IMSI Attach in-progress) to an LAI where the SIM
                   indicated we had previously registered.
11/24/03   jca     Allow LU attempt when camped on a Forbidden PLMN/LAI and
                   the network selection mode is set to MODE_MANUAL as per
                   GCF test case 44.2.1.1.4.3.2.
12/11/03   jca     No longer set substate to LIMITED_SERVICE when RR/RRC report
                   change of PLMN in SERVICE_IND.
12/12/03   jca     Now checking for forbidden PLMN when RRC_SERVICE_IND received.
                   Now sending status to REG when LAI received in SERVICE_IND is
                   included in the Forbidden LAI list or Forbidden PLMN list.
01/15/04   jca     Ensure SRV_STATUS = LIMITED when LU reject cause = PLMN_NOT_ALLOWED.
01/20/04   ks      Now sending AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED as the
                   reject cause to CNM when LU is failed due to pending MO call.
01/20/04   jca     Ensure T3212 is processed on expiration when the MM state is
                   set to MM_IDLE.ATTEMPTING_TO_UPDATE regardless of the MS op mode
                   and network op mode.
02/26/04   vdr     Now assigning TLLI to LLC/GRR if LAU is initiated during inter RAT change
03/02/04   ks      Setting the REG cause to MMR_CAUSE_FAILURE_REMAIN_ON_PLMN
                   when the LAI is regionally forbidden.
03/03/04   jca     Added code to explcitly set GMM state to DEREGISTERED when
                   LU REJECT message received with cause codes #11, #12, #13 or #15.
03/12/04   mks     Wrapped FEATURE_GSM_GPRS around the function call
                   gmmllc_gmm_tlli_assignment()
03/24/04   jca     Changed function name is_lai_forbidden() to mm_is_lai_forbidden()
                   and externalized function to other MM modules.
03/25/04   jca     Stop timers used for triggering LU procedure when service is lost.
04/06/04   cd      LCS support.  Upon LU reject, T3240 now is started in mm_state_control
04/19/04   jca     Do not process T3212 timeout when ms_op_mode set to PS_ONLY.
05/07/04   ks      Now sending GMMSM_GPRS_RELEASE_IND primitive to SM when GMM
                   state is changed to DEREGISTERED due to a LU reject.
05/14/04   cd      Enhancements to CS FOR
06/01/04   jca     Send MMR_SERVICE_IND on receipt of RR(C)_SERVICE_IND if
                   mm_needs_to_send_mmr_service_ind is set to TRUE.
06/08/04   cd      LCS support
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
08/31/04   jca     No longer delete LAI, TMSI and CKSN on receipt of LU REJECT
                   with cause codes #13 or #15 (as per 24.008 spec).
10/18/04   jca     No longer set GMM state to DEREGISTERED when LU REJECT message
                   received with cause codes #13 or #15.
11/10/04   jca     Clear gmm_gprs_attach_pending flag after a LU REJECT
                   containing cause code #11 or #12 is received.
11/16/04   sv      Introduced a new primitive for handling specific scenario's
                   related to CN Information update by network
12/07/04   jca     If a RR(C)_SERVICE_IND indicating a change of LA is received
                   while awaiting a EST_CNF for a CS call, then abort the CS call
                   and initiate a Location Update.
03/10/05   jca     Do not initiate LU procedure if camped on a forbidden LAI.
03/28/05   sm      Set the MM substate to NO_IMSI,
                   when SIM is invalid for CS services and do not trigger LU.
04/01/05   sm      Added state transition incase there is momentarily
                   Access block. Now, LU is triggered after the
                   access blocking is removed. Also, triggers the LU after cell
                   reselection, if there is access blocked in the previous cell.
04/21/05   hj      On receiving RRC_SERVICE_IND if service is available and
                   location update status is updated, if mm_idle_substate was limited
                   service it is changed to normal service.
05/20/05   jca     Attempt to process a pending emergency call request when returning to
                   MM Idle state after a failed location update attempt.
06/14/05   kvk     Forwarding the Location status to GSTK for every cell change indication.
06/19/05   abe     After receiving service indication from lower layer,if T3213 is running 
                   (previous LAU failed because of MAXIMUM RANDOM ACCESS FAILURE) 
                   initiate LAU even if LAI not changed. 
06/20/05   hj      Added support for new events EVENT_SMGMM_REQUEST_SENT, 
                   EVENT_SMGMM_REJECT_RECEIVED.
07/15/05   hj      Now initiating periodic LU on RR_SERVICE_IND, if T3212 has expired
                   and UE previous service state is SERVICE_AVAILABLE.
07/22/05   abe     Incase of WtoG CCO failed copy the current LAI information to the 
                   previous so that it won't cause any LAC change and doesn't trigger LAU. 
09/09/05   ajt     If cell access information changes from No Service/Emergency to Normal/
                   All Calls in service indication, update upper layers.     
09/16/05   vdr     Now suppressing LU procedure after a redirection/reselection to GSM
                   when an Emergency call is pending.  The LU will be done after E-call.               
09/16/05   ajt     Added parsing of EMERGENCY Number List IE in Location Update Message.                                      
09/23/05   up      Blocking LU if the PLMN is different than the requested one
                   in LIMITED_MANUAL_ACQ mode.
10/06/05   ajt     Invalidate authentication cache upon receiving Location Area Update Accept.                   
10/08/05   abe     Added support for FEATURE_INTERRAT_PCCO_GTOW
10/16/05   ks      Now setting the mm_pending_lu_request to NORMAL_LU whenever the LU is
                   suppressed due to E-call after a redirection from W to G and if the LAI
                   is different right after the redirection.   
12/05/05   hj      Setting mm_idle_transition_timer to T3211 when T3211 is started.
12/14/05   abe     Supported changes for FEATURE_GSM_DTM
02/03/06   vdr     Now setting up the CNM call if there is pended CNM Establish request and
                   LAI has not been chaned
03/22/06   ajt     On receiving LU Reject, Cause # 13 (National Roaming Not Allowed),
                   MM Idle Substate should be Limited, rather than PLMN Search.
04/18/06   ss    Added cause value to GMMSM_GPRS_RELEASE_IND
05/15/06   up      Process pending Emergency Call after Inter-RAT redirection
                   when SIM is Invalid/NO SIM or PLMN/LAI is forbidden.
06/14/06   up      Called new mm_update_mm_serving_plmn_info for GSTK event.
06/23/06   up      Perform LU when moving from NMO1 N/W to GSM Only N/W.
07/07/06   up      T3212 expired during AC restriction next LU is PERIODIC.
07/14/06   pk     Updated for Feature DSAC
07/21/06   up      Do nothing if LU is aborted with IMM Assignment reject.
                   Perform next LU after cell change occured or T3122 expired.
08/02/06   ajt     While RRC is OOS and PS call is going on, RRC cannot inform OOS to NAS.
                   If T3212 expires now, IDT procedure for LU will fail. After 4 failures, MM retries only after
                   T3212 expiry. This is spec violation due our UE design constraint. As per spec,
                   UE must delay T3212 processing till UE comes back in service, however, that OOS/back 
                   in service, is not informed to NAS by RRC. To take care of such scenarios, RRC will
                   send close session confirm with cause MM_CAUSE_RRC_OOS_CONNECTED_MODE. NAS will mark
                   LU as pending on receiving this reject cause for close session confirm.
08/28/06   sn      Add LAI in reject list on receiving reject with cause 15
09/25/06   sn      Handling of MMR_CAUSE_FAILURE_MAX_ATTEMPTED.
04/09/07   rd      SMS optimization changes are incorporated.                   
20/12/07   jbk     Responding to pending CNM Service Req after LU fail                 
08/25/11   jbk     API to check if PLMN ID is valid
09/21/11   sla     When UE is waiting stop mode cnf from AS,it will not do
                   registration when retry timers are getting expired.    
09/22/11   sla     Send MMR_REG_REJECT_IND to REG when msc tempororily not
                   reachable cause is received   
10/04/11   sla     Added RAC to forground search list when LU is failed for max no of times.    
                   Foreground search list is not cleared when LU is accepted    
10/12/11   sla     Initiate foreground search only if mm_is_foreground_search_to_be_initiated
                    is true.Reset the variable mm_is_foreground_search_to_be_initiated
                   to TRUE when LU is initiated because of LAI change   
11/08/11   sla     Update the mm_previous_serving_lai with mm location information lai  
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
12/06/11   gps     Remembering DSAC information of the cell where last registration was attempted
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stringl/stringl.h>
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"
#include "mm_v.h"
#include "err.h"
#include "mm_iei.h"
#include "libprot.h"
#include "cause.h"
#include "mm_sim.h"
#include "mm_umts_v.h"
#include "mm_coord.h"
#include "naslog_v.h"
#include "mmauth.h"
#include "mmsecurity.h"
#include "gmm_int_v.h"
#include "reg_sim_v.h"
#include "sys_eplmn_list_v.h"
#include "mmrat.h"
#include "gmmllcm.h"
#include "smtask.h"
#include "smtask_v.h"
#include "reg_nv.h"
#include "ttl_map.h"
//FR15492
#include "reg_state.h"
#ifdef FEATURE_ENHANCED_NW_SELECTION
#include "reg_sim_v.h"
#endif
#ifdef FEATURE_GSTK
#include "mm_gstk.h"
#endif
#if defined(FEATURE_LTE)
#include "emm_utility.h"
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern boolean  mm_cell_changed_to_cs_capable_sim[MAX_NAS_STACKS];
#else
extern boolean mm_cell_changed_to_cs_capable;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
byte lu_cause_sim[MAX_NAS_STACKS] = { MM_LU_CAUSE_RESERVED, MM_LU_CAUSE_RESERVED, MM_LU_CAUSE_RESERVED } ;
extern byte gmm_irat_lu_pending_sim[MAX_NAS_STACKS];
#define gmm_irat_lu_pending gmm_irat_lu_pending_sim[mm_as_id]
#define lu_cause lu_cause_sim[mm_as_id]

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
byte lu_cause_sim[MAX_NAS_STACKS] = { MM_LU_CAUSE_RESERVED, MM_LU_CAUSE_RESERVED } ;
#define lu_cause lu_cause_sim[mm_as_id]
extern byte gmm_irat_lu_pending_sim[MAX_NAS_STACKS];
#define gmm_irat_lu_pending gmm_irat_lu_pending_sim[mm_as_id]
#else
static byte lu_cause = MM_LU_CAUSE_RESERVED;
extern byte gmm_irat_lu_pending;
#endif /* FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a,(sys_modem_as_id_e_type)mm_sub_id,\
                                                                                             (sys_modem_as_id_e_type)mm_as_id)
#endif

#ifdef FEATURE_LTE
#ifdef FEATURE_DUAL_SIM
extern boolean gmm_lte_deact_isr_timer_expired_sim[MAX_AS_IDS];
#define gmm_lte_deact_isr_timer_expired gmm_lte_deact_isr_timer_expired_sim[mm_sub_id]
#else
extern boolean gmm_lte_deact_isr_timer_expired;
#endif
#endif

extern boolean mm_check_connection_active
(
  sys_modem_as_id_e_type sub_id,
  sys_modem_as_id_e_type stack_id_nas
);

extern void gs_read_coded_classmark_2_per_nas_stacks(
  classmark_2_ie_T  *classmark_2_ptr,
  classmark_params_T   *params_ptr,
  sys_modem_as_id_e_type stack_id_nas
);

extern byte gs_get_classmark_1_per_nas_stacks
(
  classmark_params_T    *params_ptrs,
  sys_modem_as_id_e_type stack_id_nas
);

extern boolean full_srv_req_in_emerg;

/*===========================================================================

FUNCTION MM_IS_LAI_FORBIDDEN

DESCRIPTION
  This function checks if the GIVEN LAI is forbidden

DEPENDENCIES
  None

RETURN VALUE
  TRUE   if LAI is forbidden of national or regional roaming
  FALSE  otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_is_lai_forbidden( inter_task_lai_T lai )
{
   if ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
           SYS_NETWORK_SELECTION_MODE_MANUAL) ||
       ((mm_check_forbidden_national_roaming(&lai) == FALSE ) &&
        (mm_check_forbidden_regional_roaming(&lai) == FALSE )))
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}


/*===========================================================================

FUNCTION IS_LAI_DIFFERENT_FROM_CURRENT

DESCRIPTION
  This function checks if the given LAI is different from the current LAI
  stored in the MM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean is_lai_different_from_current
(
  inter_task_lai_T previous_serving_lai,
  inter_task_lai_T new_lai
)
{
   if (memcmp( &previous_serving_lai, &new_lai, LAI_SIZE ) == 0)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}


/*===========================================================================

FUNCTION MM_PROCESS_SERVICE_IND

DESCRIPTION
  This function process the RRC_SERVICE_IND or RRC_SERVICE_IND_FROM_OTA_MSG or
  RR_SERVICE_IND or RR_SERVICE_IND_FROM_OTA_MSG message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_process_service_ind(mm_cmd_type* msg_p)
{
   inter_task_lai_T        new_lai;
   inter_task_lai_T        previous_serving_lai;
   mmr_service_ind_s_type  mmr_service_ind;
   boolean                 is_cell_forbidden = FALSE;
   boolean routine_man_selection_failed = FALSE;
   service_state_type      previous_service_state = mm_serving_plmn.mm_service_state;
   mmr_cause_e_type reg_cause = MMR_CAUSE_NOT_SPECIFIED;
   boolean                 process_pended_lu_now = FALSE;

   mm_as_cell_access_status_T  cell_access = MM_AS_ACCESS_NO_CALLS;

#ifdef FEATURE_NAS_REL10
   boolean      is_lu_to_be_initiated = FALSE;
#endif

#ifdef FEATURE_FEMTO_CSG
   boolean         is_manual_csg_selection_success = FALSE;
#endif 

#ifdef FEATURE_LTE
   emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

   emm_ctrl_data_ptr = emm_db_get_ctrl_data();

   ASSERT(emm_ctrl_data_ptr != NULL);
#endif
   mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED; /*set to default cause */

   (void) memscpy( &previous_serving_lai, sizeof(inter_task_lai_T), mm_serving_lai, LAI_SIZE );
 
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER

  /* CCO Failed, RRC will camp on the same cell that it was on before W to G CCO.
   ** Set the Previous service LAI to the serving cell info as this avoids
   ** sending of any LAU.
   */
   if ((WtoGCCO_state == CCO_FAILED)
#ifdef FEATURE_INTERRAT_PCCO_GTOW
       || (GtoWCCO_state == CCO_FAILED )
#endif   
                                        )
   {
      previous_serving_lai.PLMN_id               = mm_serving_plmn.info.plmn;
      previous_serving_lai.location_area_code[0] = mm_serving_plmn.lac.lac[0];
      previous_serving_lai.location_area_code[1] = mm_serving_plmn.lac.lac[1];

      if ((mm_location_information.location_update_status == UPDATED) &&
          (memcmp( mm_location_information.lai,&previous_serving_lai, LAI_SIZE ) == 0)
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
   }
#endif


   /*
    * Convert the message from RRC to RR format
    * to be handled by the RR message handler
    */
   if(msg_p->cmd.hdr.message_id == (int) RRC_SERVICE_IND)
   {
      cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);

      mmumts_process_cell_change(msg_p->cmd.rrc_service_ind.service_status,
                                 msg_p->cmd.rrc_service_ind.service_info,NORMAL_SERVICE_MESSAGE);
   }

   /*-------------------------------------------
    Process RRC SERVICE IND FROM OTA MSG
    ------------------------------------------*/
   else if (msg_p->cmd.hdr.message_id == (int) RRC_SERVICE_IND_FROM_OTA_MSG)
   {
      cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);

      mmumts_process_cell_change(msg_p->cmd.rrc_service_ind_from_ota_msg.service_status,
                                 msg_p->cmd.rrc_service_ind_from_ota_msg.service_info,OTA_SERVICE_IND_MESSAGE);
   }

   /*-------------------------------------------
    Process RR SERVICE INDICATION
    -------------------------------------------*/
   else if (msg_p->cmd.hdr.message_id == (int) RR_SERVICE_IND)
   {
      cell_access = msg_p->cmd.rr_service_ind.cell_access;

      if (msg_p->cmd.rr_service_ind.service_state == MM_AS_SERVICE_AVAILABLE)
      {
         mm_serving_lai[0] =
            msg_p->cmd.rr_service_ind.location_area_identification.PLMN_id.identity[0];
         mm_serving_lai[1] =
            msg_p->cmd.rr_service_ind.location_area_identification.PLMN_id.identity[1];
         mm_serving_lai[2] =
            msg_p->cmd.rr_service_ind.location_area_identification.PLMN_id.identity[2];
         mm_serving_lai[3] =
            msg_p->cmd.rr_service_ind.location_area_identification.location_area_code[0];
         mm_serving_lai[4] =
            msg_p->cmd.rr_service_ind.location_area_identification.location_area_code[1];

         mm_serving_plmn.mm_service_state = SERVICE_AVAILABLE;
      }
      else /* service_state == NO_SERVICE */
      {
         /* Set LAI to invalid LAI */
         mm_serving_lai[0] = 0xFF;
         mm_serving_lai[1] = 0xFF;
         mm_serving_lai[2] = 0xFF;

         /* Per table 10.5.3/3 of TS24.008, deleted LAC is 0xfffe */
         mm_serving_lai[3] = 0xFF;
         mm_serving_lai[4] = 0xFE;

         mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
      }

      if ( memcmp( mm_serving_lai, &previous_serving_lai,  LAI_SIZE ) != 0 )
      {

         MSG_HIGH_DS_3(MM_SUB, "=MM= Serving LAI PLMN %X %X %X",
             mm_serving_lai[0], mm_serving_lai[1], mm_serving_lai[2]);

         MSG_HIGH_DS_2(MM_SUB, "=MM= Serving LAC %X %X", mm_serving_lai[3], mm_serving_lai[4]);
      }
   }

#ifdef FEATURE_GSM_DTM
   /*-------------------------------------------
    Process RR SERVICE INDICATION FROM OTA MSG
    -------------------------------------------*/
   else if (msg_p->cmd.hdr.message_id == (int) RR_SERVICE_IND_FROM_OTA_MSG)
   {
      cell_access = msg_p->cmd.rr_service_ind_from_ota_msg.cell_access;

      if (msg_p->cmd.rr_service_ind_from_ota_msg.service_state == MM_AS_SERVICE_AVAILABLE)
      {
         mm_serving_lai[0] =
            msg_p->cmd.rr_service_ind_from_ota_msg.location_area_identification.PLMN_id.identity[0];
         mm_serving_lai[1] =
            msg_p->cmd.rr_service_ind_from_ota_msg.location_area_identification.PLMN_id.identity[1];
         mm_serving_lai[2] =
            msg_p->cmd.rr_service_ind_from_ota_msg.location_area_identification.PLMN_id.identity[2];
         mm_serving_lai[3] =
            msg_p->cmd.rr_service_ind_from_ota_msg.location_area_identification.location_area_code[0];
         mm_serving_lai[4] =
            msg_p->cmd.rr_service_ind_from_ota_msg.location_area_identification.location_area_code[1];

         mm_serving_plmn.mm_service_state = SERVICE_AVAILABLE;
      }
      else /* service_state == NO_SERVICE */
      {
         /* Set LAI to invalid LAI */
         mm_serving_lai[0] = 0xFF;
         mm_serving_lai[1] = 0xFF;
         mm_serving_lai[2] = 0xFF;

         /* Per table 10.5.3/3 of TS24.008, deleted LAC is 0xfffe */
         mm_serving_lai[3] = 0xFF;
         mm_serving_lai[4] = 0xFE;

         mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
      }

      if ( memcmp( mm_serving_lai, &previous_serving_lai,  LAI_SIZE ) != 0 )
      {

         MSG_HIGH_DS_3(MM_SUB, "=MM= Serving LAI PLMN %X %X %X",
             mm_serving_lai[0], mm_serving_lai[1], mm_serving_lai[2]);

         MSG_HIGH_DS_2(MM_SUB, "=MM= Serving LAC %X %X", mm_serving_lai[3], mm_serving_lai[4]);
      }
   }
#endif /* FEATURE_GSM_DTM */

   else
   {
      MSG_FATAL_DS(MM_SUB, "=MM= Invalid message id in mm_process_service_ind", 0,0,0);
   }
   
      /**----------------------------------------------------------------------
    **The normal location updating procedure shall also be started when the MS is both IMSI attached for GPRS
    **and non-GPRS services and enters a new routing area where the network operates 
    **in network operation mode I and timer T3346 is running.
    **(24.008 section 4.4.1) ------------------------------------------------*/ 
   
      /**------------------------------------------------------------------------
    **The MS shall also start the normal location updating procedure when the network 
    **is operating in network operation mode I, T3346 is running, update status is
    **not U1 UPDATED and the user manually selects the current PLMN.
    **(24.008 section 4.4.1) ------------------------------------------------*/

#ifdef FEATURE_NAS_REL10
   is_lu_to_be_initiated = mm_is_lu_to_be_done_due_to_t3346();
#endif

#if defined(FEATURE_LTE)        
   if (mm_interrat_state == MM_LTE_TO_UMTS_INTERRAT_CHANGE_IN_PROGRESS ||
       mm_interrat_state == MM_LTE_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS)
   {
     mm_set_mm_substate_on_irat_from_lte();
   }
#endif

   if (mm_serving_plmn.mm_service_state == SERVICE_NOT_AVAILABLE)
   {
      if( (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
#ifdef FEATURE_WRLF_SYSTEM_SEL
          && (mm_wcdma_rlf_state == WCDMA_RLF_IDLE)
#endif
        )
      {
         mm_state_control( MM_STD_LOW_LEVEL_FAIL );
      }
      /*
       * if change to SERVICE_NOT_AVAILABLE from SERVICE_AVAILABLE, inform REG
       */

      if (previous_service_state == SERVICE_AVAILABLE)
      {
        switch (mm_serving_plmn.ms_op_mode)
        {
          case SYS_SRV_DOMAIN_CS_ONLY:
            mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                            mm_serving_plmn.info.reg_service_domain,
                                            FALSE,
                                            SYS_SRV_STATUS_NO_SRV,
                                            TRUE );

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
              mmr_service_ind.service_state = mm_serving_plmn.info;
              mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
              mm_send_mmr_service_ind( &mmr_service_ind );
            }
            break;

          default:
            if (mm_reg_waiting_for_reg_cnf)
            {
              mm_ready_to_send_reg_cnf = TRUE;
            }
            break;
        }
      }

      mm_substate_control( MM_STD_NO_SERVICE );

      MSG_MED_DS(MM_SUB, "=MM= No service available", 0,0,0);
      
   }
   else
   {              
      /* Change serving LAI format to mm_inter_task_lai_T */
      new_lai.PLMN_id.identity[0]   = mm_serving_lai[0];
      new_lai.PLMN_id.identity[1]   = mm_serving_lai[1];
      new_lai.PLMN_id.identity[2]   = mm_serving_lai[2];
      new_lai.location_area_code[0] = mm_serving_lai[3];
      new_lai.location_area_code[1] = mm_serving_lai[4];

      if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
      {
         if ((cell_access == MM_AS_ACCESS_EMERGENCY_ONLY) ||
             (cell_access == MM_AS_ACCESS_ALL_CALLS))
         {
            /*
            * Tell REG that Limited service is now available
            */
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
              mm_send_mmr_service_ind( &mmr_service_ind );
            }
         }
         /*
         ** Initiate pending Emergency Call when SIM is Invalid / or NO SIM.
         */
         if ((mm_held_cnm_service_req.present) && 
             (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
             ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
              (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)))
         {
           mm_check_pending_requests();
           MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing Pending E-call after InterRAT change - SIM Invalid/NO SIM: %d", 
                    mm_held_cnm_service_req.interat_status);

           mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;
         }
         return;
      }

      if (!((PLMN_MATCH( mm_serving_lai,
           mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity ) ||
           (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
            sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))
     #ifdef FEATURE_FEMTO_CSG
           && (mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
     #endif   
           ) &&
           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
             SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)) 
      {
        routine_man_selection_failed = TRUE;
        MSG_HIGH_DS_0(MM_SUB,"=MM= Different PLMN found during MAN_LIMITED_ACQ");
      }

      /* ------------------------------------
      ** Check that this LAI is not forbidden
      ** ------------------------------------ */
      if ((mm_check_forbidden_national_roaming(&new_lai)) ||
          (mm_check_forbidden_regional_roaming(&new_lai)) ||
#ifdef FEATURE_DUAL_SIM
          (reg_sim_per_subs_plmn_forbidden( new_lai.PLMN_id, (sys_modem_as_id_e_type)mm_sub_id ))
#else
          (reg_sim_plmn_forbidden( new_lai.PLMN_id ))
#endif
#ifdef FEATURE_FEMTO_CSG
         ||(!mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
#endif 
       )
      {
        is_cell_forbidden = TRUE;
      }

#ifdef FEATURE_FEMTO_CSG
      if((mm_serving_plmn.info.csg_info.csg_id == mmcoord_pending_reg_message.cmd.mmr_reg_req.csg_id) &&
          (sys_plmn_match(mm_serving_plmn.info.plmn, mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn)) &&
          (mm_serving_plmn.info.csg_info.csg_id != SYS_CSG_ID_INVALID) &&
          (mm_serving_csg_id == mm_serving_plmn.info.csg_info.csg_id))
      {
        MSG_HIGH_DS_0(MM_SUB,"=MM= CSG: Manual CSG selection successful");
        is_manual_csg_selection_success = TRUE;
      }
#endif

      /* Due to HO in G, NAS is not aware of current NMO at the end of the call */
#if defined (FEATURE_GSM_DTM) || defined (FEATURE_NO_REG_UNTIL_NMO_KNOWN_AFTER_HANDOVER)
      if (mm_lu_pended_nmo_unknown)
      {
#if defined (FEATURE_GSM_DTM) && defined (FEATURE_NO_REG_UNTIL_NMO_KNOWN_AFTER_HANDOVER)
        if ((msg_p->cmd.hdr.message_id != (int) RR_SERVICE_IND_FROM_OTA_MSG)&&
            (!((msg_p->cmd.hdr.message_id == (int) RRC_SERVICE_IND_FROM_OTA_MSG)&&
               (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE))))
#elif defined (FEATURE_GSM_DTM)
       if (msg_p->cmd.hdr.message_id != (int) RR_SERVICE_IND_FROM_OTA_MSG)
#else
       if (!((msg_p->cmd.hdr.message_id == (int) RRC_SERVICE_IND_FROM_OTA_MSG)&&
             (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)))
#endif
        {
          MSG_HIGH_DS_0(MM_SUB,"=MM= LU was pended as NMO was unknown");
          process_pended_lu_now = TRUE;
          mm_lu_pended_nmo_unknown = FALSE;
        }
        /* On HO back to Updated LA, NMO is known */
        else if ((mm_location_information.location_update_status == UPDATED) &&
                 (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)
#ifdef FEATURE_LTE
                  &&(mm_pending_lu_request != LTE_IRAT_LU)
#endif
                )
        {
          mm_lu_pended_nmo_unknown = FALSE;
        }
      }
#endif /*defined (FEATURE_GSM_DTM) || defined (FEATURE_NO_REG_UNTIL_NMO_KNOWN_AFTER_HANDOVER) */

      if ((is_lai_different_from_current( previous_serving_lai, new_lai )) ||
           mm_cell_changed_to_cs_capable ||
           process_pended_lu_now
#ifdef FEATURE_NAS_REL10
            ||
           ((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE) &&
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
            (mm_is_rrc_access_class_11_to_15() == FALSE)&&
#endif
            (mm_is_plmn_equivalent_t3246_plmn())) ||
           (is_lu_to_be_initiated == TRUE)
#endif
#ifdef FEATURE_FEMTO_CSG
           || (mm_serving_csg_id != mm_serving_plmn.info.csg_info.csg_id)
#endif 
      )
      {
#ifdef FEATURE_FEMTO_CSG
        MSG_HIGH_DS_3(MM_SUB,"=MM= LAI has changed OR (Cell CS capability change : %d OR Pended LU:%d) OR CSG ID Change %d",
                                        mm_cell_changed_to_cs_capable,process_pended_lu_now, mm_serving_csg_id);
#else
        MSG_HIGH_DS_2(MM_SUB,"=MM= LAI has changed OR (Cell CS capability change : %d OR Pended LU:%d)", mm_cell_changed_to_cs_capable,process_pended_lu_now);
#endif 

#ifdef FEATURE_NAS_REL10        
        MSG_HIGH_DS_1(MM_SUB ,"=MM= LU due to T3346 running: %d", is_lu_to_be_initiated);
#endif
#ifdef FEATURE_NAS_ECALL
        if (mm_idle_substate == MM_ECALL_INACTIVE)
        {
          mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                          mm_serving_plmn.info.reg_service_domain,
                                          TRUE,
                                          SYS_SRV_STATUS_LIMITED,
                                          TRUE );

          if (mm_reg_waiting_for_reg_cnf)
          {
             mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
             mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
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
#endif
        /* ------------------------------------
        ** Check that this LAI is not forbidden
        ** ------------------------------------ */
        if ((routine_man_selection_failed) ||
            (((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)||
                (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                  SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))&&
                (!mm_manual_mode_user_selection_procedure)) ||
              ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)||
               (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                   SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))) &&
              (is_cell_forbidden)
 #ifdef FEATURE_FEMTO_CSG
              &&(!is_manual_csg_selection_success)
#endif 
            ))
        {
            /* This LA is forbidden, so assume limited service */
            MSG_HIGH_DS_0(MM_SUB, "=MM= LAI is in a forbidden list");

            mm_substate_control( MM_STD_LIMITED_SERVICE );

            mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                            mm_serving_plmn.info.reg_service_domain,
                                            TRUE,
                                            SYS_SRV_STATUS_LIMITED,
                                            TRUE );

            if (mm_check_forbidden_regional_roaming(&new_lai))
            {
              reg_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
            }
            if((mm_reg_waiting_for_reg_cnf) &&
               (mmcoord_pending_reg_message.cmd.hdr.message_id == MMR_REG_REQ) && 
               (mm_is_plmn_valid(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn)) &&
               (!sys_plmn_match(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn, mm_serving_plmn.info.plmn)) &&
               ((!sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn)) && !sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)) &&
               (mm_state == MM_LOCATION_UPDATE_INITIATED) &&
               (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
              )
            {
               reg_cause = MMR_CAUSE_REQ_CAMPED_PLMN_MISMATCH;
            }
            if (mm_reg_waiting_for_reg_cnf)
            {
               mmcoord_mmr_reg_cnf.cause = reg_cause;
               mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
               mm_send_mmr_reg_cnf();
               mm_reg_waiting_for_reg_cnf = FALSE;
               mm_ready_to_send_reg_cnf   = FALSE;
            }
            else
            {
               mmr_service_ind.cause = reg_cause;
               mmr_service_ind.service_state = mm_serving_plmn.info;
               mm_send_mmr_service_ind( &mmr_service_ind );
            }
            /*
            ** Initiate pending Emergency Call when PLMN/LAI is forbidden.
            */
            if ((mm_held_cnm_service_req.present) && 
                (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
                 (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)))
            {
              mm_check_pending_requests();
              MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing Pending E-call after InterRAT change - PLMN/LAI is forbidden: %d", 
                       mm_held_cnm_service_req.interat_status);

              mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;         
            }
        }
//FR15492
        else if(reg_state_is_no_service_to_be_send_due_to_china_mcc(new_lai.PLMN_id))
        {
          if(rrc_connection_status != MMRRC_IDLE)
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
              MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RRC_ABORT_REQ");
              send_cmd_to_rrc(rrc_cmd_p, mm_serving_plmn.info.active_rat );
              }
              else
#endif
            if( is_gsm_mode())
            {
              MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RR_ABORT_REQ");
              mm_send_rr_abort_req(FALSE);
            }
          }
        }
        else
        {
          mm_substate_control( MM_STD_NO_SERVICE );
          mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
          mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
          mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                        mm_serving_plmn.info.reg_service_domain,
                                        FALSE,
                                        SYS_SRV_STATUS_NO_SRV,
                                        FALSE );
         if(mm_reg_waiting_for_reg_cnf)
         {
           mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
           /* Set the PLMN service stat information in the REG_CNF primitive */
           mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
           mm_send_mmr_reg_cnf();
           mm_reg_waiting_for_reg_cnf = FALSE;
           mm_ready_to_send_reg_cnf   = FALSE;
         }
         else
         {
           mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
           mmr_service_ind.service_state = mm_serving_plmn.info;
           mm_send_mmr_service_ind( &mmr_service_ind );
         }
        }
       }
#ifdef FEATURE_ENHANCED_NW_SELECTION
        else if(mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai))
        {
            MSG_HIGH_DS_0(MM_SUB,"=MM= Serving LAI is in FOREGROUND SEARCH LAI LIST- Dont do LU");

            mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list(mm_serving_lai);

            if (previous_serving_lai.location_area_code[0] != 0xff ||
                previous_serving_lai.location_area_code[1] != 0xfe)
            {
              mm_clear_lu_attempt_counter();
            }
            if(mm_cell_changed_to_cs_capable)
            {
              mm_substate_control(MM_STD_NORMAL_SERVICE);
              mm_substate_control(MM_STD_UPDATE_ATTEMPTED);
            }
            mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                            SYS_SRV_DOMAIN_NO_SRV,
                                            FALSE,
                                            SYS_SRV_STATUS_LIMITED,
                                            TRUE );
            
            
            if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
               ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
            ||  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
               ))
            {
              /* -------------------------------
              ** Proceed to GMM  procedure
              ** ------------------------------- */
              if(mm_reg_waiting_for_reg_cnf)
              {  
                mm_ready_to_send_reg_cnf = TRUE;
              }
            }
            else
            {
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

            if ((mm_held_cnm_service_req.present) && 
                (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
                 (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)))
            {
              MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing Pending E-call after InterRAT change when LAI is in fore ground lai list and interrat status is %d",
                       mm_held_cnm_service_req.interat_status);
              mm_check_pending_requests();
              mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;         
            } 
        }
#endif
#ifdef FEATURE_NAS_REL10
         else if  ((((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE) &&
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                      (mm_is_rrc_access_class_11_to_15() == FALSE)&&
#endif
                      (mm_is_plmn_equivalent_t3246_plmn()))||
                    ((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)&&
                     (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)&&
                     (mm_gs_association_established == TRUE 
#if defined(FEATURE_LTE)
                     ||((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) &&
                        (mm_pending_lu_request != LTE_IRAT_LU))
#endif
                     ) &&
                     (is_lu_to_be_initiated == FALSE)))
#ifdef FEATURE_SGLTE
                     && (MM_IS_SGLTE_ON_PS_STACK_ID)
#endif
                  )

         {
             if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS ||
                 mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY) &&
                (mm_location_information.location_update_status == UPDATED) &&
                (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0))
             {
               mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               mm_serving_plmn.info.reg_service_domain,
                                               FALSE,
                                               SYS_SRV_STATUS_SRV,
                                               TRUE );

             }
             else
             {
               mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               SYS_SRV_DOMAIN_NO_SRV,
                                               FALSE,
                                               SYS_SRV_STATUS_LIMITED,
                                               TRUE );
             }

             if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2) 
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                  || (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
                 ))
             {
                    /* -------------------------------
                     ** Proceed to GMM Attach procedure
                     ** ------------------------------- */
               if(mm_reg_waiting_for_reg_cnf)
               {
                 mm_ready_to_send_reg_cnf = TRUE;
               }
             }
             else
             {

               if (mm_reg_waiting_for_reg_cnf)
               {
                 mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                 mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
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
             if ((mm_held_cnm_service_req.present) && 
                 (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                 ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
                  (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)))
             {
               MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing Pending E-call after InterRAT change when T3346 or T3246 is running and interrat status is %d",
                           mm_held_cnm_service_req.interat_status);
               mm_check_pending_requests();
               mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;         
             } 
        }
#endif
        else if ((memcmp( mm_serving_lai, mm_previous_serving_lai,  LAI_SIZE ) == 0 ) &&
                 ((mm_lu_attempt_counter >= 4) ||
                  (mm_timer_status[TIMER_T3211] == TIMER_ACTIVE)))
        {
            if(mm_cell_changed_to_cs_capable)
            {
              mm_substate_control(MM_STD_NORMAL_SERVICE);
              mm_substate_control(MM_STD_UPDATE_ATTEMPTED);
            }
            if ((mm_held_cnm_service_req.present) && 
                (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
                 (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)))
            {

              MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing Pending E-call after InterRAT change when all LU retries got completed: ", 
                        mm_held_cnm_service_req.interat_status);
              mm_check_pending_requests();
              mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;
                              
            }
            else
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= All LU retries got completed or T3211 is running");
            }
        }

        else if((mm_serv_change_pending == TRUE) && 
                 (gmm_state == GMM_DEREGISTERED_INITIATED) &&
                 (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY))
        {
          MSG_HIGH_DS_0(MM_SUB," =MM= Don't perform LU if service change is pending and detach is in progress");
        }

        /* ----------------------------------------------------------
        ** Location Update procedure is not performed if LOCI in the
        ** SIM matches with the LOCI broadcasted on BCCH
        ** ----------------------------------------------------------*/
        else if ((mm_location_information.location_update_status == UPDATED) &&
                 (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0))
        {
            if ((mm_possible_imsi_attach) && (mm_system_information.ATT))
            {
              if(mm_cell_changed_to_cs_capable)
              {
                mm_substate_control(MM_STD_NORMAL_SERVICE);
              }
              if ((mm_held_cnm_service_req.present) && 
                  (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                  ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
                   (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)))
              {

                MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing Pending E-call after InterRAT change when UE is already updated: %d", 
                           mm_held_cnm_service_req.interat_status);
                mm_check_pending_requests();
                mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;
                mm_pending_lu_request = IMSI_ATTACH;
               
              }
              else
              {
                mm_initiate_lu( MM_LU_CAUSE_IMSI_ATTACH );
              }
            }
            else
            {
               MSG_HIGH_DS_0(MM_SUB,"=MM= LAI changed - LA same as SIM, so NORMAL LU not required- check for possible periodic LU");
               
               mm_substate_control( MM_STD_NORMAL_SERVICE );
#ifdef FEATURE_GSTK
              /* Location Status Event to SIM may be required since LOCI is updated */
               if (!reg_nv_is_imsi_switch_enabled())
               {
                  mm_gstk_set_location_status_evt (TRUE);
               }
#endif
               mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               SYS_SRV_DOMAIN_CS_ONLY,
                                               FALSE,
                                               SYS_SRV_STATUS_SRV,
                                               FALSE );
               (void) memscpy( mm_previous_serving_lai,LAI_SIZE,
                              mm_location_information.lai, LAI_SIZE );

               mm_previous_idle_substate = mm_idle_substate;
               if (mm_held_cnm_service_req.present)
               {
                 mm_check_pending_requests();
                 MSG_HIGH_DS_0(MM_SUB, "=MM= Processing Pending CNM request.");

                 mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;         
               } 
               else if ((mm_pending_lu_request == T3212_EXPIRED) ||
                        (mm_pending_lu_request == T3312_EXPIRED))
               {
                 mm_initiate_lu(MM_LU_CAUSE_PERIODIC);
               }        
#if defined(FEATURE_LTE)              
               /*
               ** Spec 24.008:Location updating procedure
               **  The normal location updating procedure shall also be started if the MS is configured to use CS fallback 
               **  and SMS over SGs, or SMS over SGs only, and the TIN indicates "RAT-related TMSI",
               **    a) When the MS enters a GERAN or UTRAN cell in network operation mode II or III and the 
               **        E-UTRAN deactivate ISR timer T3423 is running.
               **    b) When the MS, configured to use CS fallback and SMS over SGs, or SMS over SGs only, enters a
               **        GERAN or UTRAN cell in network operation mode II or III and the E-UTRAN deactivate ISR timer
               **        T3423 has expired.
               */
               else if ((((gmm_lte_deact_isr_timer_expired == TRUE )||
                              (mm_timer_status[TIMER_T3423] == TIMER_ACTIVE &&
                               emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI))
                          ||((mm_pending_lu_request == LTE_IRAT_LU || mm_lu_start_reason == LTE_IRAT_LU) 
                          && (mm_csfb_state == MM_CSFB_STATE_NULL)
                         && (mm_state != MM_CONNECTION_ACTIVE)))

#ifdef FEATURE_SGLTE
                            && MM_IS_SGLTE_ON_PS_STACK_ID
#endif
                        )
               {
                 MSG_HIGH_DS_3(MM_SUB,"=MM= LU started due to IRAT from LTE: gmm_lte_deact_isr_timer_expired=%d mm_pending_lu_request=%d T3423 status=%d",
                                 gmm_lte_deact_isr_timer_expired, mm_pending_lu_request, mm_timer_status[TIMER_T3423]);
                 MSG_HIGH("=MM= mm_lu_start_reason = %d",mm_lu_start_reason,0,0);
                 if(mm_lu_start_reason == LTE_IRAT_LU)
                 {
                   mm_pending_lu_request = LTE_IRAT_LU;
                 }
                 mm_initiate_lu(MM_LU_CAUSE_NORMAL);
               }
               else if ((mm_csfb_state == MM_CSFB_STATE_LU_PENDING) && (mm_state == MM_IDLE ))
               {
                 MSG_HIGH_DS_0(MM_SUB,"=MM= LU started due to mm csfb state being MM_CSFB_STATE_LU_PENDING ");
                 mm_initiate_lu(MM_LU_CAUSE_NORMAL);
               }
#endif
#ifdef FEATURE_NAS_REL10
               else if(is_lu_to_be_initiated)
               {
                 MSG_HIGH_DS_1(MM_SUB,"=MM= LU started due to T3346 running in LTE or in NMO1 netwrok  %d", mm_timer_status[TIMER_T3346]);
                 mm_initiate_lu(MM_LU_CAUSE_NORMAL);
               }
#endif
               if(mm_pending_lu_request != LTE_IRAT_LU)
               {
                 mm_pending_lu_request = NO_LU;
               }
               /* ----------------------------------
               ** Stop timer T3213 if it is running
               ** ----------------------------------*/
               if (mm_timer_status[TIMER_T3213] != TIMER_STOPPED)
               {
                  mm_stop_timer( TIMER_T3213 );
               }

               if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)
               {
                  /* --------------------------------------
                  ** Tell REG that service is now available
                  ** -------------------------------------- */
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
                     mm_send_mmr_service_ind( &mmr_service_ind );
                  }
               }
               else if (mm_reg_waiting_for_reg_cnf)
               {
                  mm_ready_to_send_reg_cnf = TRUE;
               }
            }
         }
         else if ((mm_idle_substate == MM_NORMAL_SERVICE) ||
                  ((mm_idle_substate == MM_LIMITED_SERVICE ) &&
                   (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode !=
                      SYS_NETWORK_SELECTION_MODE_LIMITED)))
         {
            if ((mm_held_cnm_service_req.present) && 
                (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
                 (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)))
            {
              MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing Pending E-call after InterRAT change: %d", 
                       mm_held_cnm_service_req.interat_status);
              mm_check_pending_requests();
              mm_pending_lu_request = NORMAL_LU;
              if (mm_idle_substate == MM_LIMITED_SERVICE)
              {
                mm_substate_control( MM_STD_UPDATE_ATTEMPTED );
              }
              mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;         
            } 
    #if defined(FEATURE_LTE) 
          /* If CSFB emergency call is pending strat call first and mark LU as pending for NMO2 */
          /* If NV full_srv_req_in_emerg is TRUE strat LU before placing the call */
            else if ( (mm_lai_change_force_lau_for_emergency != TRUE) &&
                      (mm_interrat_state == MM_LTE_TO_UMTS_INTERRAT_CHANGE_IN_PROGRESS ||
                        mm_interrat_state == MM_LTE_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS) 
                       && (mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
                #ifdef FEATURE_MODEM_HEAP    
                       &&(mm_csfb_service_req_message_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                             == EMERGENCY_CALL_ESTABLISHMENT)
                #else
                       &&(mm_csfb_service_req_message.cmd.mmcnm_est_req.CNM_service_type 
                                             == EMERGENCY_CALL_ESTABLISHMENT) 
                #endif
                       &&(full_srv_req_in_emerg == FALSE)
                         ) 
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= CSFB: Emergency call pending, starting call before LAU");
           
              if(mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
              {
                mm_pending_lu_request = NORMAL_LU;
              }
              mm_csfb_process_pending_call();
            }
    #endif
            else
            {
              if (mm_state == MM_WAIT_FOR_RR_CONNECTION_MM)
              {
                MSG_HIGH_DS_1(MM_SUB ,"=MM= RRC connection under establishment for %d",mm_cnm_srv_est_cause);              
                if (mm_cnm_srv_est_cause == MM_RR_EMERGENCY_CALL)
                {
                  //re-encode to ensure new LA info is used after re-direction
                  mm_check_pending_requests();
                  mm_pending_lu_request = NORMAL_LU;
                  if (mm_idle_substate == MM_LIMITED_SERVICE)
                  {
                    mm_substate_control( MM_STD_UPDATE_ATTEMPTED );
                  }
                  MSG_HIGH_DS_0(MM_SUB, "=MM= LU Pending due to emergency call set up in progress");
                }
                else
                {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                  mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL , MMCC_SMS_CS);
#else
                  mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
                  mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE );
                  mm_state_control( MM_STD_RR_CONNECTION_ABORTED );
                  mm_pending_lu_request = NORMAL_LU;
                }                  
              }
              else
              {
                mm_substate_control( MM_STD_UPDATE_ATTEMPTED );
                mm_initiate_lu( MM_LU_CAUSE_NORMAL );
              }
            }

            mm_is_foreground_search_to_be_initiated = TRUE;
         }
         else if (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE)
         {
            /* -------------------------------------------------
             *  LU attempt counter is not cleared if the previous
             *  serving LAC is 0xfffe. This is to avoid the case
             *  where LU attempts have been made before momentary
             *  loss of service and eventually regaining the service
             *  on the same cell.
             * -------------------------------------------------*/
            if (previous_serving_lai.location_area_code[0] != 0xff ||
                previous_serving_lai.location_area_code[1] != 0xfe)
            {
              mm_clear_lu_attempt_counter();
            }
            if ((mm_held_cnm_service_req.present) && 
                (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
                 (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)))
            {
              MSG_HIGH_DS_2(MM_SUB, "=MM= Processing Pending E-call after InterRAT change when MM substate is %d and interrat status is %d", mm_idle_substate,
                       mm_held_cnm_service_req.interat_status);
              mm_check_pending_requests();
              mm_pending_lu_request = NORMAL_LU;
              mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;         
            } 
            else
            {
              mm_initiate_lu( MM_LU_CAUSE_NORMAL );
            }
            mm_is_foreground_search_to_be_initiated = TRUE;
         }
         else if (mm_idle_substate == MM_LOCATION_UPDATE_NEEDED)
         {
            if ((mm_held_cnm_service_req.present) && 
                (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
                 (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)))
            {
              MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing Pending E-call after moving to access class unbarred RAT %d",
                       mm_held_cnm_service_req.interat_status);
              mm_check_pending_requests();
              mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;         
            } 
            else
            {
              mm_substate_control( MM_STD_UPDATE_ATTEMPTED );
              mm_initiate_lu( MM_LU_CAUSE_NORMAL );
            }
            mm_is_foreground_search_to_be_initiated = TRUE;
            
         }
         else if ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                      SYS_NETWORK_SELECTION_MODE_LIMITED)&&
                  (mm_held_cnm_service_req.present) && 
                  (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                  ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
                  (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)))
         {
            MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing Pending E-call after InterRAT change in pure LIMTED mode: %d", 
                      mm_held_cnm_service_req.interat_status);
            mm_check_pending_requests();
            mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;         
         }
         else if (mm_cell_changed_to_cs_capable &&
                  (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode !=
                      SYS_NETWORK_SELECTION_MODE_LIMITED))
         {
            mm_substate_control( MM_STD_NORMAL_SERVICE );
            mm_substate_control( MM_STD_UPDATE_ATTEMPTED );
            mm_initiate_lu( MM_LU_CAUSE_NORMAL );
            mm_is_foreground_search_to_be_initiated = TRUE;
         }
         mm_cell_changed_to_cs_capable = FALSE;
      }
      else  /* LAI has not changed */
      {
         MSG_HIGH_DS_0(MM_SUB,"=MM= LAI has not changed");

         if (previous_service_state == SERVICE_NOT_AVAILABLE)
         {
            /* ------------------------------------
            ** Check that this LAI is not forbidden
            ** ------------------------------------ */
            if (mm_is_lai_forbidden( new_lai ) ||
#ifdef FEATURE_DUAL_SIM 
                (reg_sim_per_subs_plmn_forbidden( new_lai.PLMN_id, (sys_modem_as_id_e_type)mm_sub_id ))
#else
                (reg_sim_plmn_forbidden( new_lai.PLMN_id ))
#endif
#ifdef FEATURE_FEMTO_CSG
               || !mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info)
#endif 
                )
            {
               /* This LA is forbidden, so assume limited service */
               MSG_HIGH_DS_0(MM_SUB, "=MM= LAI is in a forbidden list");
               mm_substate_control( MM_STD_LIMITED_SERVICE );

               mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               mm_serving_plmn.info.reg_service_domain,
                                               FALSE,
                                               SYS_SRV_STATUS_LIMITED,
                                               TRUE );
            }
            else
            {
               mm_substate_control( MM_STD_NORMAL_SERVICE );

               mm_stop_timer( TIMER_T3210);

               mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               mm_serving_plmn.info.reg_service_domain,
                                               FALSE,
                                               SYS_SRV_STATUS_SRV,
                                               TRUE );
            }

            if (mm_check_forbidden_regional_roaming(&new_lai))
            {
              reg_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
            }

            /*
            * Tell REG that service is now available
            */
            if (mm_reg_waiting_for_reg_cnf)
            {
              mmcoord_mmr_reg_cnf.cause = reg_cause;
              mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
              mm_send_mmr_reg_cnf();
              mm_reg_waiting_for_reg_cnf = FALSE;
              mm_ready_to_send_reg_cnf   = FALSE;
            }
            else
            {
              mmr_service_ind.cause = reg_cause;
              mmr_service_ind.service_state = mm_serving_plmn.info;
              mm_send_mmr_service_ind( &mmr_service_ind );
            }

            if ((mm_pending_lu_request == T3212_EXPIRED) ||
                (mm_pending_lu_request == T3312_EXPIRED))
            {
              mm_initiate_lu( MM_LU_CAUSE_PERIODIC );
            }
#if defined(FEATURE_LTE)                
            /*
            ** Spec 24.008:Location updating procedure
            **  The normal location updating procedure shall also be started if the MS is configured to use CS fallback 
            **  and SMS over SGs, or SMS over SGs only, and the TIN indicates "RAT-related TMSI",
            **    a) When the MS enters a GERAN or UTRAN cell in network operation mode II or III and the 
            **        E-UTRAN deactivate ISR timer T3423 is running.
            **    b) When the MS, configured to use CS fallback and SMS over SGs, or SMS over SGs only, enters a
            **        GERAN or UTRAN cell in network operation mode II or III and the E-UTRAN deactivate ISR timer
            **        T3423 has expired.
            */
            else if ((((gmm_lte_deact_isr_timer_expired == TRUE )||
                          (mm_timer_status[TIMER_T3423] == TIMER_ACTIVE &&
                           emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI))
                        ||(mm_pending_lu_request == LTE_IRAT_LU 
                        && (mm_csfb_state == MM_CSFB_STATE_NULL)
                        && (mm_state != MM_CONNECTION_ACTIVE)))

#ifdef FEATURE_SGLTE
                            && MM_IS_SGLTE_ON_PS_STACK_ID
#endif
                        )
             {
               MSG_HIGH_DS_3(MM_SUB,"=MM= LU started due to IRAT from LTE: gmm_lte_deact_isr_timer_expired=%d mm_pending_lu_request=%d T3423 status=%d",
                               gmm_lte_deact_isr_timer_expired, mm_pending_lu_request, mm_timer_status[TIMER_T3423]);
               mm_initiate_lu(MM_LU_CAUSE_NORMAL);
             }
#endif
         }
         /* previous service available but LU had not been done
          * due to access class blocking
          */
 //FR15492
        else if(reg_state_is_no_service_to_be_send_due_to_china_mcc(new_lai.PLMN_id))
        {
          if(rrc_connection_status != MMRRC_IDLE)
          {
            if(mm_state == MM_CONNECTION_ACTIVE)
            {
              mm_pending_no_service_to_reg = TRUE;
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
                MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RRC_ABORT_REQ");
                send_cmd_to_rrc(rrc_cmd_p, mm_serving_plmn.info.active_rat );
              }
              else
#endif
              if( is_gsm_mode())
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RR_ABORT_REQ");
                mm_send_rr_abort_req(FALSE);
              }
            }
          }
          else
          {
            mm_substate_control( MM_STD_NO_SERVICE );
            mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
            mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
            mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                          mm_serving_plmn.info.reg_service_domain,
                                          FALSE,
                                          SYS_SRV_STATUS_NO_SRV,
                                          FALSE );
           if(mm_reg_waiting_for_reg_cnf)
           {
             mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
             /* Set the PLMN service stat information in the REG_CNF primitive */
             mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
             mm_send_mmr_reg_cnf();
             mm_reg_waiting_for_reg_cnf = FALSE;
             mm_ready_to_send_reg_cnf   = FALSE;
           }
           else
           {
            mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
            mmr_service_ind.service_state = mm_serving_plmn.info;
            mm_send_mmr_service_ind( &mmr_service_ind );
           }
         }
        }
         else
         {  
            if ((mm_idle_substate == MM_LOCATION_UPDATE_NEEDED) &&
                (mm_check_if_cs_cell_access_allowed(cell_access) == TRUE) &&
                (mm_state != MM_WAIT_FOR_RR_CONNECTION_LU) &&
                (mm_state != MM_LOCATION_UPDATE_INITIATED) )
            {
              if ((mm_held_cnm_service_req.present) && 
                  (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                  ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
                   (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)))
              {
                MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing Pending E-call after moving to access class unbarred RAT of different cell %d",
                    mm_held_cnm_service_req.interat_status);
                mm_check_pending_requests();
                mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;         
              } 
              else if ((mm_location_information.location_update_status == UPDATED) &&
                       (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)
                      )
              {
                if ((mm_possible_imsi_attach) && (mm_system_information.ATT))
                {
                  /* Initiate location update IMSI ATTACH */
                  mm_substate_control(MM_STD_UPDATE_ATTEMPTED);
                  mm_initiate_lu(MM_LU_CAUSE_IMSI_ATTACH);                
                }
                else if ((mm_pending_lu_request == T3212_EXPIRED) ||
                         (mm_pending_lu_request == T3312_EXPIRED))
                {
                  mm_substate_control( MM_STD_NORMAL_SERVICE );
                  mm_initiate_lu( MM_LU_CAUSE_PERIODIC );
                }
                else
                {
                  mm_substate_control(MM_STD_UPDATE_ATTEMPTED);
                  mm_initiate_lu(MM_LU_CAUSE_NORMAL);                
                }
              }
              else
              {
                /* Initiate location update NORMAL LU */              
                mm_substate_control(MM_STD_UPDATE_ATTEMPTED);
                mm_initiate_lu(MM_LU_CAUSE_NORMAL);
              }
            } 
            /* If T3212 has expired and LU could not be performed,
             * initiate LU if access class is not barred */
            else if (((mm_pending_lu_request == T3212_EXPIRED)  ||
                (mm_pending_lu_request == T3312_EXPIRED))  &&
                (mm_check_if_cs_cell_access_allowed(cell_access) == TRUE)&&
                (mm_idle_substate != MM_LIMITED_SERVICE))
            {
                mm_substate_control( MM_STD_NORMAL_SERVICE ); 
                mm_initiate_lu( MM_LU_CAUSE_PERIODIC );
            }
            /* If T3213 is running then do LAU even LAI is
               not changed from the previous but the cell should have changed and 
               the message should be RR_Servince_ind or OTA _RR_Service_ind  */
           /* LU was pending due to IMM Assignment Rejct, so perform LU
           ** with previous type even though LAI is not changed */
            else if ( (mm_timer_status[TIMER_T3213] != TIMER_STOPPED) && 
                  (
#ifdef FEATURE_GSM_DTM
                  (msg_p->cmd.hdr.message_id == (int) RR_SERVICE_IND_FROM_OTA_MSG &&
                   msg_p->cmd.rr_service_ind_from_ota_msg.cell_changed )  ||
#endif
                                           (msg_p->cmd.hdr.message_id == (int) RR_SERVICE_IND &&
                                            msg_p->cmd.rr_service_ind.cell_changed )                ) )
            {
               /* --------------------------------------
               ** Do location update with type last used
               ** -------------------------------------- */

               mm_initiate_lu( lu_cause ); 

               mm_T3213_timed_out = TRUE; 
            }
            else if ((mm_pending_lu_request == T3122_EXPIRED)&&
            (
#ifdef FEATURE_GSM_DTM
                     ((msg_p->cmd.hdr.message_id == (int) RR_SERVICE_IND_FROM_OTA_MSG) &&
                      ((msg_p->cmd.rr_service_ind_from_ota_msg.cell_changed) ||
                       (!msg_p->cmd.rr_service_ind_from_ota_msg.t3122_running)))  ||
#endif
                    ((msg_p->cmd.hdr.message_id == (int) RR_SERVICE_IND) &&
                     ((msg_p->cmd.rr_service_ind.cell_changed) ||
                      (!msg_p->cmd.rr_service_ind.t3122_running)))))
            {
               mm_initiate_lu( lu_cause ); 
            }
            else if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                     (gmm_previous_network_op_mode == NETWORK_OP_MODE_1) &&
                     (mm_serving_plmn.network_op_mode == PS_DOMAIN_SYS_INFO_UNAVAILABLE) &&
                     (mm_gs_association_established == TRUE))
            {
              /* Perform LU when moving from NMO1 N/W
              ** to GSM Only N/W, where GPRS sys info not present
              ** This is to remove SGSN association in MSC/VLR.
              */
              mm_stop_timer(TIMER_T3312);
#if defined(FEATURE_LTE)
              mm_stop_timer(TIMER_T3323);
#endif

              mm_initiate_lu(MM_LU_CAUSE_NORMAL);
            }

            /*-----------------------------------------------------------
            **Section 4.2.2.2 of 24.008
            **if entry into the state ATTEMPTING TO UPDATE was caused by c) or d)
            **or f) (with cause different from "abnormal release, unspecified") or g) 
            **(with cause "retry upon entry into a new cell") of subclause 4.4.4.9, then
            **location updating shall be performed when a new cell is entered;
            **-----------------------------------------------------------*/
            else if ( mm_initiate_lu_upon_entry_into_new_cell == TRUE &&
                      (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE) &&
                      (mm_state == MM_IDLE) &&
                      (msg_p->cmd.hdr.message_id == (int) RR_SERVICE_IND &&
                       msg_p->cmd.rr_service_ind.cell_changed))
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= Cell is changed");
              mm_initiate_lu( mm_get_lu_cause() );
            }

            /*-----------------------------------------------------------
            **Section 4.2.2.2 of 24.008
            **if entry into the state ATTEMPTING TO UPDATE was caused by c) or d)
            **or f) (with cause different from "abnormal release, unspecified") or g) 
            **(with cause "retry upon entry into a new cell") of subclause 4.4.4.9, then
            **location updating shall be performed when a new cell is entered;
            **-----------------------------------------------------------*/
            else if ( mm_initiate_lu_upon_entry_into_new_cell == TRUE &&
                      (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE) &&
                      (mm_state == MM_IDLE) &&
#ifdef FEATURE_NAS_REL10
                      ((mm_timer_status[TIMER_T3246] != TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                       ||(mm_is_rrc_access_class_11_to_15() == TRUE)
#endif
                       ) &&
#endif
                      ((msg_p->cmd.hdr.message_id == (int) RRC_SERVICE_IND ||
                        msg_p->cmd.hdr.message_id == (int) RRC_SERVICE_IND_FROM_OTA_MSG) &&
                      ((mm_serving_cell_valid)&&
                       memcmp(&mm_lu_failed_cell_id,&mm_serving_cell_id,sizeof(uint32)) != 0)))
                       
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= Cell is changed");
              mm_initiate_lu( mm_get_lu_cause() );
            }
#if defined(FEATURE_LTE)
            else if (mm_pending_lu_request == LU_ABORTED_IN_UPDATED_CELL)
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= LU aborted in updated cell previously, start LU again");
              mm_initiate_lu( mm_get_lu_cause() );
            }

            else  if((mm_pending_lu_request == LTE_IRAT_LU)                     
                        && (mm_csfb_state == MM_CSFB_STATE_NULL)
                        && (mm_state != MM_CONNECTION_ACTIVE))
            {
              if ((memcmp( mm_serving_lai, mm_previous_serving_lai,  LAI_SIZE ) == 0 ) &&
                  (((mm_lu_attempt_counter >= 4)&&
                   (mm_timer_status[TIMER_T3212] == TIMER_ACTIVE))||
                  (mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
                  (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)||
                  ((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                       &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
                   )))
              {
                MSG_HIGH_DS_0(MM_SUB,"=MM= Do not initiate pended IRAT LU on attempted LAI ");
              }
              else
              {
                MSG_HIGH_DS_0(MM_SUB,"=MM= LU initiated due to IRAT from LTE");
                mm_initiate_lu( MM_LU_CAUSE_NORMAL );
              }
            }
#endif
            else if((gmm_previous_network_op_mode == NETWORK_OP_MODE_1) &&
                    (gmm_state == GMM_DEREGISTERED ) &&
                   ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                    ||
                    (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
            
#endif
                    ) &&
                   (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) != 0))
            {
              if ((((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) ||
                    (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)) &&
                   (mm_lu_attempt_counter <4)) ||
                   (mm_lu_attempt_counter >= 4) ||
                   ((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                       &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
                    ))
              {
                MSG_HIGH_DS_0(MM_SUB,"=MM= Do Not initiate LU due to blocking condition");
              }
              else if (mm_state != MM_IDLE)
              {
                mm_pending_lu_request = NORMAL_LU;
              }
              else
              {
                MSG_HIGH_DS_0(MM_SUB,"=MM= LU Initiated due to reselection from NMO1 -> NMO2");
                mm_initiate_lu(MM_LU_CAUSE_NORMAL);
              }
            }
            else if ((memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) != 0) &&
                      (mm_pending_lu_request == LU_PENDED_TMSI_REALLOCATION))
            {
                MSG_HIGH_DS_0(MM_SUB, "=MM= LU Initiated After LAI change during TMSI reallocation");
                mm_initiate_lu(MM_LU_CAUSE_NORMAL);
            }
            else
            {   
              if ((mm_held_cnm_service_req.present) && 
                  (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                  ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
                  (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER)))
              {
                mm_check_pending_requests();
                MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing Pending E-call after moving to different cell of INTERRAT: %d", 
                            mm_held_cnm_service_req.interat_status);
                mm_held_cnm_service_req.interat_status = MM_AS_SUCCESS ;
              } 
              else if(mm_state == MM_IDLE)
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= Processing Pending call after Connection release");
                mm_check_pending_requests();
              }

#ifdef FEATURE_NAS_REL10
              if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
                 (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
              {
                if (mm_reg_waiting_for_reg_cnf)
                {
                  mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                  mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                  mm_send_mmr_reg_cnf();
                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf = FALSE;
                }
                else
                {
                  mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                  mmr_service_ind.service_state = mm_serving_plmn.info;
                  mm_send_mmr_service_ind( &mmr_service_ind );
                }
             }
#endif        
              if(((mm_prev_cell_access == MM_AS_ACCESS_EMERGENCY_ONLY) ||
                    (mm_prev_cell_access == MM_AS_ACCESS_NO_CALLS)) &&
                   ((cell_access == MM_AS_ACCESS_NORMAL_ONLY) ||
                    (cell_access == MM_AS_ACCESS_ALL_CALLS)) ) 
               {
                  mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                    mm_serving_plmn.info.reg_service_domain,
                                    FALSE,
                                    SYS_SRV_STATUS_SRV,
                                    TRUE );
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
                     mm_send_mmr_service_ind( &mmr_service_ind );
                  }   
               }
            }
         }
      }
   }

#ifdef FEATURE_LTE   
   if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG ||
      mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)
   {
     mm_csfb_process_pending_call(); 
   }
#endif
}


/*===========================================================================

FUNCTION MM_HANDLE_LOCATION_UPDATING

DESCRIPTION
  This function handles messages relating to location updating

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_handle_location_updating(mm_cmd_type* msg_p)
{
   byte                             ie_count;
   ie_mobile_id_T                   *ie_id;
   ie_equiv_plmn_T                  *ie_equiv_plmn_list;
   byte                             equiv_plmn_list_len;
   mmr_service_ind_s_type           mmr_service_ind;
   byte                             *p_emergency_list_ie;
   byte                             emergency_list_length;
   byte                             count = 0;
   boolean                          periodic_t3212_present = FALSE;
   boolean                          emergency_number_list_present = FALSE;
   byte                             previous_location_information[LAI_SIZE];
#ifdef FEATURE_NAS_REL10
   byte                             *p_per_ms_3212_ie;
   byte                              per_ms_3212_length;
   dword                             new_T3212_timer_value = 0; /* Default - Should always be reset*/
#endif

#ifdef FEATURE_ENHANCED_NW_SELECTION
   byte                             foreground_timer_expiry_lai[LAI_SIZE];
#endif

   mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED; /*set to default cause */

   if (((msg_p->cmd.hdr.message_set == MS_MM_RR) ||
        ((msg_p->cmd.hdr.message_set == MS_MM_TDSRRC)
#ifdef FEATURE_SEGMENT_LOADING
&&get_tdscdma_interface()
#endif
))&&
        ( (msg_p->cmd.hdr.message_id == (int) RR_SERVICE_IND) ||
#ifdef FEATURE_GSM_DTM
          (msg_p->cmd.hdr.message_id == (int) RR_SERVICE_IND_FROM_OTA_MSG) ||
#endif
          (msg_p->cmd.hdr.message_id == (int) RRC_SERVICE_IND) ||
          (msg_p->cmd.hdr.message_id == (int) RRC_SERVICE_IND_FROM_OTA_MSG)) )
   {
      /* Process the service indication */
      mm_process_service_ind(msg_p);

   }
   else if ( is_OTA_message(msg_p) &&
      get_OTA_msg_type(msg_p) == LOCATION_UPDATING_ACCEPT )

   {
      boolean tmsi_reallocated = FALSE;
      byte lu_lai[5];
      sys_plmn_id_s_type lu_accept_plmn;
      mm_lu_start_reason = NO_LU;
      TTLBOOTUP(WG_CS_REG_SUCCESS);
      mm_stop_timer(TIMER_CSMT_GUARD);
      /*-------------------------------------------------------------
       Copy the LAI received in the LU Accept message for furthur
       processing.  LAI is the first IE after PD and type. Thats is
       why an offset of 2 is taken from the L3 Data
       --------------------------------------------------------------*/

       memscpy(lu_lai, 5, msg_p->cmd.rr_data_ind.layer3_message+ 2, LAI_SIZE);


#ifdef FEATURE_NAS_REL10
      mm_is_per_ms_t3212_present = FALSE;
#endif
      lu_accept_plmn.identity [0] = lu_lai[0];
      lu_accept_plmn.identity [1] = lu_lai[1];
      lu_accept_plmn.identity [2] = lu_lai[2];
      auth_reset_cache_memory(FALSE, NO_TIMER_EXPIRED, RRC_CS_DOMAIN_CN_ID);    

      /*
       * Processing to deal with the successful Location Update report.
       * If the current LAI is in any of the forbidden LAI lists, then
       * this is removed. If the response contains a TMSI allocation, then
       * the local store is updated. If follow on proceed is indicated in
       * the message, then this is flagged to the MM connection management
       * process.
       */

      mm_stop_timer( TIMER_T3212 );

      /*
       * Do consistency check...
       */
      if((!mm_is_plmn_valid(lu_accept_plmn)) || 
          ((mm_reg_waiting_for_reg_cnf) && 
          ((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
            mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)) &&
          !((sys_plmn_match(mm_serving_plmn.info.plmn , lu_accept_plmn )) ||
         (sys_eplmn_list_equivalent_plmn(lu_accept_plmn)))))
      {
        MSG_HIGH_DS_3(MM_SUB, "=MM= MCC/MNC %02x %02x %02x invalid -> retry LU",
                                 lu_lai[0],lu_lai[1],lu_lai[2]);

        (void)memset( mm_lu_reject_info.lai, 0xff, LAI_SIZE );
        mm_lu_reject_info.reject_cause = 0x00;
        mm_lu_reject_info.mm_conn_rej_timer_value = 0;
        mm_state_control( MM_STD_LU_REJECT );

        return;
      }
      if ( memcmp(lu_lai, mm_serving_lai, LAI_SIZE ) != 0 )
      {
         /*------------------------------------------------------------
          Not entirely sure if this is an error. These is a possibility
          that cell reselection happened and we drifted into a cell
          that belongs to a new LAI.  Again this already should have been
          indicated to us using SERVICE_IND
          -------------------------------------------------------------*/
         MSG_ERROR_DS(MM_SUB, "=MM= MM: LAI was not as expected", 0,0,0);

         
         /*------------------------------------------------------------
         ** Update currently camped PLMN/LA with the PLMN/LA Id received
         ** in LU Accept message. This may happen LU may have been performed
         ** due to DSAC spec rules, however, RRC didn't inform currently
         ** camped PLMN/LA Id due to RRC spec prohibiting it from informing
         ** so in connected mode.
         **-------------------------------------------------------------*/         
         mm_serving_plmn.info.plmn.identity[0] = lu_lai[0];
         mm_serving_plmn.info.plmn.identity[1] = lu_lai[1];
         mm_serving_plmn.info.plmn.identity[2] = lu_lai[2];
         mm_serving_plmn.lac.lac[0] = lu_lai[3];
         mm_serving_plmn.lac.lac[1] = lu_lai[4];
         
         (void) memscpy(mm_serving_lai, LAI_SIZE, lu_lai, LAI_SIZE);
      }

      /*
       * Store message.location_area_identification in location_information
       */
      (void)memscpy(previous_location_information,LAI_SIZE, mm_location_information.lai,LAI_SIZE); 
      (void)memscpy( mm_location_information.lai, LAI_SIZE, lu_lai, LAI_SIZE);

      mm_location_information.location_update_status = UPDATED;

      for ( count=0; count< mm_lu_attempt_counter && count< MAX_ATTEMPT_COUNTER_LAU; count++)
      {
        mm_lu_reject_cause[count] = 0xFF;
      }

      /* -----------------------------------------------------------------
      ** The attempt counter is reset when location update is successfully
      ** completed (3GPP TS 24.008 4.4.4.5)
      ** ----------------------------------------------------------------- */
      mm_clear_lu_attempt_counter();

      mm_idle_transition_timer = NULL;

      mm_stop_timer( TIMER_T3210);

      if (mm_managed_roaming_enabled)
      {
        /* Clear reject #17 list */
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

      /*
       * remove from either forbidden list if present
       */

      mm_remove_forbidden_national_roaming( (inter_task_lai_T *)lu_lai);
      mm_remove_forbidden_regional_roaming( (inter_task_lai_T *)lu_lai);

      mm_remove_plmn_from_fplmn_list( mm_serving_plmn.info.plmn);
#ifdef FEATURE_FEMTO_CSG
      mm_add_csg_to_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info);
      mm_remove_csg_from_rejected_list(mm_serving_plmn.info.plmn,
                                       mm_serving_plmn.info.csg_info.csg_id);
#endif
      mm_substate_control( MM_STD_NORMAL_SERVICE );
	  mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;

      mm_previous_idle_substate = mm_idle_substate;

      mm_follow_on_proceed = FALSE;
      /* Initialize the Equivalent PLMN list */
      /* If the msg does no contain an equivalent plmn list, the stored list
      *  should be deleted
      */
      mm_serving_plmn.update_equiv_plmn_list = TRUE;
      memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));

      for ( ie_count=0; ie_count < mm_no_of_ies; ie_count++ )
      {
         switch (mm_IE_info[ie_count].IEI)
         {
            case IEI_MOBILE_ID:
               ie_id = (ie_mobile_id_T *)mm_IE_info[ie_count].start;

               /*
                * check if this is a TMSI
                */

               if ( ( (byte)(ie_id->Octet3) & (byte)(0x07) ) == (byte)(0x04) )
               {
                  tmsi_reallocated = TRUE;

                  /*
                   * copy the 4 bytes of the TMSI to store
                   */

                  (void)memscpy(mm_location_information.tmsi, TMSI_SIZE,
                               ie_id->DigitPair, TMSI_SIZE );

               }
               else
               {
                  /*
                   * Message contains IMSI, so delete TMSI locally and
                   * in the SIM
                   */
                  memset( mm_location_information.tmsi,
                          0xFF,
                          TMSI_SIZE );

               }

               /* --------------------------------------------
               ** Send the log packet which updates TMSI value
               ** -------------------------------------------- */
               mmsend_nas_ue_dynamic_id_log_packet( LOG_TMSI_TYPE );
               break;

            case IEI_FOLLOW_ON_PROCEED:
               mm_follow_on_proceed = TRUE;
               MSG_HIGH_DS_0(MM_SUB, "=MM= Follow_on_proceed is set to TRUE");
               break;

            case IEI_EQUIV_PLMN:
                ie_equiv_plmn_list = (ie_equiv_plmn_T *)mm_IE_info[ie_count].start;
                /* get the list length from the IE */
                equiv_plmn_list_len = ie_equiv_plmn_list->equiv_plmn_list.length;

                /*copy the list to the mm_serving_plmn global structure */
                if (equiv_plmn_list_len > 0)
                {
                  /* (24.008, 4.4.1) The maximum number of entries in the stored equivalent
                  ** PLMN list is six (5 equiv PLMN + PLMN of the network that downloaded
                  ** the list.
                  */
                  if (equiv_plmn_list_len > (EQUIVALENT_PLMN_LIST_MAX_LENGTH * 3))
                  {
                    equiv_plmn_list_len = EQUIVALENT_PLMN_LIST_MAX_LENGTH * 3;
                  }

                  mm_serving_plmn.equiv_plmn_list.length = equiv_plmn_list_len;
                  memscpy(mm_serving_plmn.equiv_plmn_list.plmn, EQUIVALENT_PLMN_LIST_MAX_LENGTH * sizeof(sys_plmn_id_s_type), ie_equiv_plmn_list->equiv_plmn_list.plmn,
                      equiv_plmn_list_len);
                }

                MSG_MED_DS(MM_SUB, "=MM= Rec'd Equiv PLMN list",0,0,0);
                break;
                
            case IEI_EMERGENCY_NUM_LIST:
                
                p_emergency_list_ie = mm_IE_info[ie_count].start; 
                emergency_list_length = *(p_emergency_list_ie + 1);
        
                if (emergency_list_length > 0)
                {
                    mmr_emergency_num_list_s_type* emergency_number_list;
#ifndef FEATURE_MODEM_HEAP
                    emergency_number_list = (mmr_emergency_num_list_s_type*)gs_alloc(sizeof(mmr_emergency_num_list_s_type));
#else
                    emergency_number_list = (mmr_emergency_num_list_s_type*)modem_mem_calloc(1, sizeof(mmr_emergency_num_list_s_type), MODEM_MEM_CLIENT_NAS);
#endif
                    mm_check_for_null_ptr((void*)emergency_number_list);

                    mm_parse_emergency_number((emergency_number_list),
                         (p_emergency_list_ie + 1) );
                    mm_send_mmr_emergency_num_list_ind(emergency_number_list);
#ifndef FEATURE_MODEM_HEAP
                    gs_free(emergency_number_list);
#else
                    modem_mem_free(emergency_number_list, MODEM_MEM_CLIENT_NAS);
#endif

                }
                emergency_number_list_present = TRUE;
            break;  
#ifdef FEATURE_NAS_REL10
            case IEI_PER_MS_3212:

                p_per_ms_3212_ie = mm_IE_info[ie_count].start; 
                per_ms_3212_length = *(p_per_ms_3212_ie + 1);
                p_per_ms_3212_ie++;

                mm_is_per_ms_t3212_present = TRUE;
                if (per_ms_3212_length > 0)
                {
                    new_T3212_timer_value =  gmm_convert_timer3_value_ie_to_msecs(
                                                                 *(p_per_ms_3212_ie + 1) );

                     if ( new_T3212_timer_value != mm_stored_per_ms_t3212_value)
                     {
                        mm_stored_per_ms_t3212_value = new_T3212_timer_value; 

#ifdef FEATURE_DUAL_SIM
                        if(MM_MODEM_STATISTICS_SUB1)
#endif
                        {
                           mm_set_signal_for_cm(SYS_NAS_PERIODIC_TIMER_MASK);
                        }
                     }
                }
                else
                {
                  mm_stored_per_ms_t3212_value = 0;
                }

            break; 
#endif

            default:
               break;

         }
      }


      /*
       * Copy location information to SIM.
       */
      mmsim_write_loci();
#ifdef FEATURE_LTE
      emm_update_rplmn_type(MM_CS_RPLMN_TYPE);
#endif
      mm_possible_imsi_attach = FALSE;

      mm_manual_mode_user_selection_procedure = FALSE;

      mm_pending_lu_request = NO_LU;

      if(tmsi_reallocated)
      {
         /* Send completion message to the network */
         mm_send_tmsi_reallocation_complete();

#ifdef FEATURE_DUAL_SIM
         if(MM_MODEM_STATISTICS_SUB1)
#endif
         {
           mm_set_signal_for_cm(SYS_NAS_REGISTRATION_INFO_MASK);
         }

      }
      /*
       *  if this location update is in response to a MMR_REG_REQ
       */
      mmsend_nas_reg_event_packet(PROC_NAS_LAU_EVENT,PROC_END,PROC_END_ACCEPT,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
      mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                      SYS_SRV_DOMAIN_CS_ONLY,
                                      FALSE,
                                      SYS_SRV_STATUS_SRV,
                                      TRUE );

      if (mm_reg_waiting_for_reg_cnf)
      {
        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
        mm_ready_to_send_reg_cnf = TRUE;
        
        if ( (mm_follow_on_proceed) &&
            (mm_held_cnm_service_req.present ) )
        {
          mm_check_pending_requests();
          mm_send_mmr_reg_cnf();
          mm_reg_waiting_for_reg_cnf = FALSE;
          mm_ready_to_send_reg_cnf   = FALSE;        
        }        
        else
        {
          mm_state_control( MM_STD_LU_ACCEPT );
        }
      }
      else
      {
         if ( (mm_follow_on_proceed) &&
              (mm_held_cnm_service_req.present ) )
         {
           mm_check_pending_requests();
         }
         else
         {
           mm_state_control( MM_STD_LU_ACCEPT );
         }
        /* Send a Service indication to REG about this */
        mmr_service_ind.service_state = mm_serving_plmn.info;
        mm_send_mmr_service_ind( &mmr_service_ind );
      }
      if(!emergency_number_list_present)
      {
         mm_nullify_mmr_emergency_num_list_if_needed
         (
            &(previous_location_information[0]),
            &(mm_serving_lai[0])
         );
      }

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
      /*
      * If we have transitioned to state 9, and there is an LCS in progress,
      * MM needs to transition to state 25
      */
      if (mm_lcs_active)
      {
         mm_state_control(MM_STD_LCS_IN_PROGRESS);
      }
#endif
   }
   else if ( is_OTA_message(msg_p) &&
             get_OTA_msg_type(msg_p) == LOCATION_UPDATING_REJECT )
   {
      byte reject_cause;
#ifdef FEATURE_NAS_REL10
      byte msg_index;
      mm_t3246_backoff_timer_value = 0;

#endif
      /* The third byte a LU Reject message carries the reject cause */
 
      reject_cause = msg_p->cmd.rr_data_ind.layer3_message[2]; 

#ifdef FEATURE_NAS_REL10
      msg_index = 3;

      if((msg_p->cmd.rr_data_ind.layer3_message[msg_index] == IEI_PER_T3246_VALUE) &&
         (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10) && reject_cause == CONGESTTION)
      {
         mm_t3246_backoff_timer_value  = gmm_convert_timer_value_ie_to_usecs(
                                                (msg_p->cmd.rr_data_ind.layer3_message[msg_index+2]));
         msg_index = msg_index + msg_p->cmd.rr_data_ind.layer3_message[msg_index+1] + 2;}
#endif
      #ifdef FEATURE_GSTK
      mm_send_gstk_location_updating_reject_evt(reject_cause, lu_cause);
      #endif /* FEATURE_GSTK */

      /*
       * Processing to handle rejection of location update as reported by the
       * network. It stops timer T3210 (via LU_rejected flag), stores the
       * reject cause, (for later use after the RR_connection has been
       * released), starts T3240, and, if the reject cause was one of 2,3,6,11,
       * 12, or 13, as details in 04.08 4.4.4.7, flags LU_reject to the main
       * MM STD causing LOCATION_UPDATE_REJECTED state to be entered. If the
       * cause was anything else, then LU_rejected is flagged to the location
       * updating STD so that a retry can be attempted, without directly
       * flagging LU_reject. In this case, the cause value is not stored.RR
       * is then updated with the reject cause as it needs this information
       * to select the CELL_RESELECTION_HYSTERISIS value.
       */

      /*
       * Must stop T3212 when LU reject is received
       */

      mm_stop_timer( TIMER_T3212 );

#ifdef FEATURE_NAS_REL10
      mm_store_integrity_protected = mm_integrity_protected;
#endif

      /* Log the reject message */
      mmsend_request_reject_event(FALSE,SMGMM_EVENT_LOCATION_AREA_UPDATE_REJECT,reject_cause); 

      if (reject_cause == IMSI_UNKNOWN_IN_HLR          ||
          reject_cause == ILLEGAL_MS                   ||
          reject_cause == ILLEGAL_ME                   ||
          reject_cause == PLMN_NOT_ALLOWED             ||
          reject_cause == LA_NOT_ALLOWED               ||
          reject_cause == NATIONAL_ROAMING_NOT_ALLOWED ||
          reject_cause == NO_SUITABLE_CELLS_IN_LA  ||
          reject_cause == MM_SEMANTICALLY_INCORRECT_MESSAGE  ||
          reject_cause == MM_INVALID_MANDATORY_INFORMATION  ||
          reject_cause == MM_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED  ||
          reject_cause == MM_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED  ||
          reject_cause == MM_PROTOCOL_ERROR_UNSPECIFIED
#ifdef FEATURE_NAS_REL10
          || 
          reject_cause == CONGESTTION
#endif
#ifdef FEATURE_FEMTO_CSG
          || (reject_cause == CSG_NOT_AUTHORIZED && mm_serving_csg_id != SYS_CSG_ID_INVALID)
#endif 
         )
      {
         /* -----------------------------------------------------------
         ** The attempt counter is reset when location update completed
         ** with cause #11, #12, #13 or #15 (24.008 section 4.4.4.5)
         ** ----------------------------------------------------------- */
         if (reject_cause == PLMN_NOT_ALLOWED             ||
             reject_cause == LA_NOT_ALLOWED               ||
             reject_cause == NATIONAL_ROAMING_NOT_ALLOWED ||
             reject_cause == NO_SUITABLE_CELLS_IN_LA
#ifdef FEATURE_FEMTO_CSG
             || (reject_cause == CSG_NOT_AUTHORIZED && mm_serving_csg_id != SYS_CSG_ID_INVALID)
#endif 
            )
         {
            mm_clear_lu_attempt_counter();
            
            if( reject_cause == PLMN_NOT_ALLOWED )
            {
              mm_add_plmn_to_fplmn_list( mm_serving_plmn.info.plmn);
            }                     

            if(reject_cause == PLMN_NOT_ALLOWED ||
               reject_cause == LA_NOT_ALLOWED ||
               reject_cause == NATIONAL_ROAMING_NOT_ALLOWED)
            {
                  mm_manual_mode_user_selection_procedure = FALSE;

                  /******************************************************
                  **Reset this falg also as UE is not suposed to do PS
                  **registration if LU is rejected with the above causes
                  ******************************************************/
                  gmm_manual_mode_user_selection_procedure = FALSE; 
            }
#ifdef FEATURE_LTE
            if(reject_cause == PLMN_NOT_ALLOWED ||            
               reject_cause == NATIONAL_ROAMING_NOT_ALLOWED)
            {
              mm_set_emm_manual_selection_flag(FALSE);
            }
#endif
         }       

         /* Set cs service status to limited if rej. cause is #11, #12, or #13 only in manual mode */
         if( mm_managed_roaming_enabled && 
             (reject_cause == PLMN_NOT_ALLOWED ||
              reject_cause == LA_NOT_ALLOWED ||
              reject_cause == NATIONAL_ROAMING_NOT_ALLOWED) && 
             (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ||
              mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
         {
             mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
         }

#ifdef FEATURE_ENHANCED_NW_SELECTION
         /* Set cs service status to limited if rej. cause is #11, #12, or #13 only in manual mode */
         if( reg_sim_read_ens_flag() && 
             (reject_cause == PLMN_NOT_ALLOWED ||
              reject_cause == LA_NOT_ALLOWED ||
              reject_cause == NATIONAL_ROAMING_NOT_ALLOWED) && 
             (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ||
              mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
         {
             mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
         }
#endif
         else if(reject_cause == MM_SEMANTICALLY_INCORRECT_MESSAGE ||
                 reject_cause == MM_INVALID_MANDATORY_INFORMATION ||
                 reject_cause == MM_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED ||
                 reject_cause == MM_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED ||
                 reject_cause == MM_PROTOCOL_ERROR_UNSPECIFIED
#ifdef FEATURE_NAS_REL10
                || 
                ((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10) &&
                  reject_cause == CONGESTTION && (mm_t3246_backoff_timer_value == 0))
#endif
         )
         {
           /* Upon reception of Cause Codes #95 #96 #97 #99 #111, The MS should set the mm_lu_attempt_counter to 4 
              and wait for rel-ind from NW 24.008 rel6 Section 4.4.4.9 */           
           mm_lu_attempt_counter = MAX_ATTEMPT_COUNTER_LAU;
         }      
         mm_lu_reject_info.reject_cause = reject_cause;
         mm_state_control( MM_STD_LU_REJECT );
      }
      else
      {  
         if(mm_managed_roaming_enabled && (reject_cause == NETWORK_FAILURE))
         {
           reg_sim_add_to_low_priority_plmn_list(mm_serving_plmn.info.plmn,mm_serving_plmn.info.active_rat
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                                 ,(sys_modem_as_id_e_type)mm_as_id
#endif
                                                );
         }
         mm_state_control( MM_STD_LU_REJECT );
         (void)memset( mm_lu_reject_info.lai, 0xff, LAI_SIZE );
         mm_lu_reject_info.reject_cause = 0x00;
         mm_lu_reject_info.mm_conn_rej_timer_value = 0;

         if ((reject_cause == NETWORK_FAILURE) ||
             (reject_cause == MSC_TEMPORARILY_NOT_REACHABLE))
         {
           mm_lu_reject_info.reject_cause = reject_cause;
         }

         if (reject_cause >=RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MIN_VALUE &&
             reject_cause <= RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MAX_VALUE)
         {
           /*-----------------------------------------------
           **LU is failed because of g of section 4.4.4.9 of 24.0.08
           **------------------------------------------------*/
           if (is_umts_mode() && mm_serving_cell_valid)
           {
              mm_lu_failed_cell_id = mm_serving_cell_id;
              mm_initiate_lu_upon_entry_into_new_cell = TRUE;
           }
           else if(is_gsm_mode())
           {
              mm_initiate_lu_upon_entry_into_new_cell = TRUE;
           }
         }
      }

#ifdef FEATURE_DUAL_SIM
      if(MM_MODEM_STATISTICS_SUB1)
#endif
      {
        mm_set_signal_for_cm(SYS_NAS_CS_REJECT_CAUSE_MASK);
      }


      /*----------------------------------------------------------
      ** The Equivalent PLMN List is deleted for all causes except
      ** #12, 14, 15 (24.008 section 4.4.4.7)
      **----------------------------------------------------------*/
      if ((reject_cause != LA_NOT_ALLOWED) &&
          (reject_cause != GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN) &&
          (reject_cause != NO_SUITABLE_CELLS_IN_LA)
#ifdef FEATURE_NAS_REL10
       && !((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10) &&
            reject_cause == CONGESTTION)
#endif
#ifdef FEATURE_FEMTO_CSG
          && !(reject_cause == CSG_NOT_AUTHORIZED && mm_serving_csg_id != SYS_CSG_ID_INVALID)
#endif 
         )
      {
        mm_serving_plmn.update_equiv_plmn_list = TRUE;
        memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));
      }

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
      /*
      * If we have transitioned to state 10, and there is an LCS in progress,
      * MM needs to transition to state 25
      */
      if (mm_lcs_active)
      {
         mm_state_control(MM_STD_LCS_IN_PROGRESS);
      }
#endif
   }
   else if ( ( msg_p->cmd.hdr.message_set == MS_TIMER ) &&
             ( msg_p->cmd.hdr.message_id == MID_TIMER_EXPIRY ))
   {
   
      switch (msg_p->cmd.timer_expiry.timer_id)
      {
         case TIMER_T3210:
            mm_lu_reject_info.reject_cause = 0;
            mmsend_nas_reg_event_packet(PROC_NAS_LAU_EVENT,PROC_END,PROC_END_TIME_OUT,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
            /*
             * If T3210 expires, and there is an LCS procedure in progress,
             * MM needs to transition to state 25
             */
            if (mm_lcs_active)
            {
               mm_state_control( MM_STD_RR_CONNECTION_REL_NOT_ALLOWED );
            }
            else
            {
               mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE );

               mm_cancel_authentication();

               mm_state_control( MM_STD_T3210_TIMEOUT );
            }
#else
            mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE );

            /* -----------------------------------------------------------------
            ** Since the RR connection is already aborted due to T3210 expiry,
            ** cancel pending authentication attempt if any.
            ** ----------------------------------------------------------------- */
            mm_cancel_authentication();

            mm_state_control( MM_STD_T3210_TIMEOUT );
#endif
            break;

#ifdef FEATURE_DUAL_SIM
         case TIMER_MM_RADIO_RETRY:
            if(!mm_check_lu_allowed())
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= Restart radio timer as attach happening on other SUB");
              mm_start_timer(TIMER_MM_RADIO_RETRY,2000);
              break;
            }
            MSG_HIGH_DS_0(MM_SUB, "=MM= Re-trying after TRM failure");
#endif
         case TIMER_T3211:

            mm_previous_idle_substate = NULL;            
            /* When in NMO 1, and combined procedures are possible, MM Timers are to be ignored */
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) ||
                ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                 (mm_gs_association_established == TRUE ||
                  mm_timer_status[TIMER_T3311] == TIMER_ACTIVE))
#ifdef FEATURE_ENHANCED_NW_SELECTION
                 ||
                 (mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai))
#endif
              )
            {
              MSG_HIGH_DS_0(MM_SUB, "=MM= Ignoring T3211 timeout as combined procedure is ongoing/LAI is in forground search LAI list ");
#ifdef FEATURE_ENHANCED_NW_SELECTION
              if (mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai))
              {
                mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list(mm_serving_lai);

                mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               SYS_SRV_DOMAIN_NO_SRV,
                                               FALSE,
                                               SYS_SRV_STATUS_LIMITED,
                                               TRUE );
                if(mm_reg_waiting_for_reg_cnf)
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
            }
            else
            {            
              if(mm_waiting_for_stop_mode_cnf)
              {
                MSG_HIGH_DS_0(MM_SUB,"=MM= LU will be initiated after camping on new RAT ");
                mm_substate_control(MM_STD_UPDATE_PENDING);
                if ( lu_cause == MM_LU_CAUSE_PERIODIC ) 
                {
                  mm_pending_lu_request = T3212_EXPIRED;
                }
                else
                {
                  mm_pending_lu_request = T3211_EXPIRED;
                }
              }             
              else if ((mm_idle_substate == MM_NORMAL_SERVICE) ||
                       (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE))
              {
                /* --------------------------------------
                  ** Do location update with type last used
                  ** -------------------------------------- */
                if ( lu_cause == MM_LU_CAUSE_PERIODIC ) 
                {
                  mm_pending_lu_request = T3212_EXPIRED;
                }

                if(mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE)
                {
                  mm_initiate_lu( lu_cause );
                }
              }
			  else if(mm_idle_substate == MM_PLMN_SEARCH_NORMAL_SERVICE &&
                      mm_lu_start_reason == LTE_IRAT_LU)
              {
                mm_pending_lu_request = LTE_IRAT_LU;
              }
              else if (mm_idle_substate == MM_PLMN_SEARCH_NORMAL_SERVICE )
              {
                mm_pending_lu_request = T3211_EXPIRED;
              }
              else if (((mm_idle_substate == MM_NO_CELL_AVAILABLE) ||
                        (mm_idle_substate == MM_PLMN_SEARCH))&&
                       (mm_lu_start_reason == LTE_IRAT_LU))
              {
                mm_pending_lu_request = LTE_IRAT_LU;
              }
              
#ifdef FEATURE_CONNECTED_MODE_SERV_IND
              else if (((mm_idle_substate == MM_NO_CELL_AVAILABLE) ||
                        (mm_idle_substate == MM_PLMN_SEARCH) ||
                        (mm_idle_substate == MM_LOCATION_UPDATE_NEEDED)) &&
                       (mm_get_lu_cause() == MM_LU_CAUSE_PERIODIC))
              {
                mm_pending_lu_request = T3212_EXPIRED;
              }                      
#endif  
              else if((mm_idle_substate == MM_LIMITED_SERVICE) &&
                      (mm_timer_status[TIMER_T3310] == TIMER_STOPPED) &&
                      (mm_timer_status[TIMER_T3311] == TIMER_STOPPED))
              {
                mmr_service_ind.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
                mmr_service_ind.service_state = mm_serving_plmn.info;
                mm_send_mmr_service_ind( &mmr_service_ind );
              }
            }
            break;

         case TIMER_T3212:
            mm_previous_idle_substate = NULL;            

            /*
            Foreground search needs to be started, even if foreground was already done 
            on the cell, if periodic LU timer expires.
            */
            mm_is_foreground_search_to_be_initiated = TRUE;

 #ifdef FEATURE_NAS_REL10
           if(mm_is_per_ms_t3212_present)
           {
             if(mm_stored_per_ms_t3212_value != 0)
             {
               periodic_t3212_present = TRUE;
             }
           }
           else
 #endif
           if( mm_system_information.T3212_timer_value != 0 )
           {
              periodic_t3212_present  = TRUE;
           }
           if (mm_managed_roaming_enabled)
           {
             reg_sim_clear_low_priority_plmn_list(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
                                                   (sys_modem_as_id_e_type)mm_as_id
#endif
                                                 );
           }
                        
            /*
             * Processing to deal with timeout of T3212 to see if a
             * location update can be performed. A final check is made
             * to see if the current value of T3212 is not zero, which
             * would indicate that periodic updating is not required.
             *
             * if a PLMN search is in progress, then the request for
             * a LU is stored in possible_LU so that the LU can be
             * performed later.
             */

            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) ||
                ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
#ifdef FEATURE_DSAC
                 ((mm_last_procedure_to_update_registration & MM_LU) != MM_LU) &&
#endif
                 ((gmm_state == GMM_REGISTERED) ||
                  (gmm_state == GMM_REGISTERED_INITIATED) ||
                  (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
                  (gmm_state == GMM_SERVICE_REQUEST_INITIATED) ||
                  ((gmm_state == GMM_DEREGISTERED) &&
                   (gmm_substate == GMM_ATTEMPTING_TO_ATTACH) &&
                   (mm_idle_substate != MM_ATTEMPTING_TO_UPDATE)))) ||
                (mm_location_information.location_update_status == ROAMING_NOT_ALLOWED)
#ifdef FEATURE_NAS_REL10
                ||
                ((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                      &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
                )
#endif 
#ifdef FEATURE_ENHANCED_NW_SELECTION
                ||
                (mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai))
#endif
#ifdef FEATURE_LTE
                || ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)&&
                     (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)&&
                     (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_NOT_REQ))
#endif
               )

            {
               MSG_HIGH_DS_0(MM_SUB, "=MM= Ignoring T3212 timeout");
#ifdef FEATURE_ENHANCED_NW_SELECTION
              if (mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai))
              {
                mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list(mm_serving_lai);
              }
#endif
            }
            else if(mm_deep_sleep_alt_scan)
            {
               MSG_HIGH_DS_0(MM_SUB, "=MM= T3212 expired in ALT SCAN mode.");
               mm_clear_lu_attempt_counter();
               mm_pending_lu_request = T3212_EXPIRED;
            }
            else if(periodic_t3212_present)
            {
               if (mm_waiting_for_stop_mode_cnf)
               {
                  MSG_HIGH_DS_0(MM_SUB,"=MM= Periodic LU will be initiated after camping on new RAT");
                  mm_substate_control(MM_STD_UPDATE_PENDING);
                  mm_clear_lu_attempt_counter();
                  mm_pending_lu_request = T3212_EXPIRED;
               }            
               else if ( mm_idle_substate == MM_ATTEMPTING_TO_UPDATE &&
                         mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE)
               {
                  mm_clear_lu_attempt_counter();
                  mm_initiate_lu( MM_LU_CAUSE_NORMAL );
               }
               else if ( (mm_idle_substate == MM_NORMAL_SERVICE )&&
                         (mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE)
#ifdef FEATURE_WRLF_SYSTEM_SEL
                          &&(mm_wcdma_rlf_state == WCDMA_RLF_IDLE)
#endif
                          )
               {
                  /*
                  ** T3212 is expired but LU is blocked due to access class restriction.
                  ** In that case, remember the pending LU type, so that when access class
                  ** restriction is removed, UE should perform periodic LU.
                  ** If LU is not blocked then the lu pending flag will be reset to NO_LU.
                  */
                  mm_pending_lu_request = T3212_EXPIRED;
                  mm_initiate_lu( MM_LU_CAUSE_PERIODIC );
               }
               else if ((mm_idle_substate == MM_PLMN_SEARCH_NORMAL_SERVICE) ||
                        (mm_idle_substate == MM_PLMN_SEARCH) ||
                        (mm_idle_substate == MM_NO_CELL_AVAILABLE) ||
                        (mm_idle_substate == MM_LIMITED_SERVICE) ||
                        (mm_idle_substate == MM_LOCATION_UPDATE_NEEDED)||
                        (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE) ||
                        ((mm_idle_substate == MM_NORMAL_SERVICE ) && 
                         (mm_serving_plmn.mm_service_state == SERVICE_NOT_AVAILABLE))
#ifdef FEATURE_WRLF_SYSTEM_SEL
                        || (mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
#endif
                        )
               {
                  mm_clear_lu_attempt_counter();
                  mm_pending_lu_request = T3212_EXPIRED;
               }
            }
            break;

         case TIMER_T3213:
            /*
             * Timer started by random access failure during LU has matured.
             * If mm_T3213_timed_out is true, this indicates that the timer
             * has matured once before for this LU attempt, so no further
             * action is required.
             */

            /* When in NMO 1, and combined procedures are possible, MM Timers are to be ignored */
                 
            mm_previous_idle_substate = NULL;                                                    
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) ||
                ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                 (mm_gs_association_established == TRUE))
#ifdef FEATURE_NAS_REL10
                 ||
                 ((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                      &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
                 )
#endif 
               )
            {
              MSG_HIGH_DS_0(MM_SUB, "=MM= Ignoring T3213 timeout as combined procedure is ongoing ");            
            }
            else
            {
              if ( mm_T3213_timed_out == FALSE )
              {
                if(mm_waiting_for_stop_mode_cnf)
                {
                  MSG_HIGH_DS_0(MM_SUB,"=MM= LU will be initiated after camping on new RAT ");
                  mm_substate_control(MM_STD_UPDATE_PENDING);
                  if ( lu_cause == MM_LU_CAUSE_PERIODIC ) 
                  {
                    mm_pending_lu_request = T3212_EXPIRED;
                  }
                  else
                  {
                    mm_pending_lu_request = T3213_EXPIRED;
                  }
                }
                if ((mm_idle_substate == MM_NORMAL_SERVICE) ||
                    (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE))
                {
                  /* --------------------------------------
                  ** Do location update with type last used
                  ** -------------------------------------- */
                  if(mm_lu_start_reason == LTE_IRAT_LU)
                  {
                    mm_pending_lu_request = LTE_IRAT_LU;
                  }
                  mm_initiate_lu( lu_cause );
                }
                else if (mm_idle_substate == MM_PLMN_SEARCH_NORMAL_SERVICE )
                {
                  mm_pending_lu_request = T3213_EXPIRED;
                }
                else if (((mm_idle_substate == MM_NO_CELL_AVAILABLE) ||
                          (mm_idle_substate == MM_PLMN_SEARCH))&&
                         (mm_lu_start_reason == LTE_IRAT_LU))
                {
                  mm_pending_lu_request = LTE_IRAT_LU;
                }
                else if (((mm_idle_substate == MM_NO_CELL_AVAILABLE) ||
                          (mm_idle_substate == MM_PLMN_SEARCH) ||
                          (mm_idle_substate == MM_LOCATION_UPDATE_NEEDED)) &&
                         (mm_get_lu_cause() == MM_LU_CAUSE_PERIODIC))
                {
                  mm_pending_lu_request = T3212_EXPIRED;
                }
                else if((mm_idle_substate == MM_LIMITED_SERVICE) &&
                        (mm_timer_status[TIMER_T3310] == TIMER_STOPPED) &&
                        (mm_timer_status[TIMER_T3311] == TIMER_STOPPED))
                {
                  mmr_service_ind.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
                  mmr_service_ind.service_state = mm_serving_plmn.info;
                  mm_send_mmr_service_ind( &mmr_service_ind );
                }
                /* remembering that Timer T3213 has expired once */       
                mm_T3213_timed_out = TRUE;               
              }
              else
              {
                MSG_MED_DS(MM_SUB, "=MM= 3213 expired twice", 0,0,0);
              }
            }
            break;

            case TIMER_CSMT_GUARD:
            break;

#ifdef FEATURE_NAS_REL10
            case TIMER_T3246: 

                mm_previous_idle_substate = NULL;

                sys_plmn_undefine_plmn_id(&mm_3gpp_t3246_plmn_id);

                /* When in NMO 1, and combined procedures are possible, MM Timers are to be ignored */
                if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) ||
                    ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                     (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                     (mm_gs_association_established == TRUE ||
                      (mm_location_information.location_update_status == UPDATED) ||
                      (mm_timer_status[TIMER_T3311] == TIMER_ACTIVE)))
#ifdef FEATURE_ENHANCED_NW_SELECTION
                     ||
                    (mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai))
#endif
                    )
                {
                    MSG_HIGH_DS_0(MM_SUB, "=MM= Ignoring MM timeout as combined procedure is ongoing/LAI is in forground search LAI list ");
#ifdef FEATURE_ENHANCED_NW_SELECTION
                    if (mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai))
                    {
                       mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                       SYS_SRV_DOMAIN_NO_SRV,
                                                       FALSE,
                                                       SYS_SRV_STATUS_LIMITED,
                                                       TRUE );
                       if(mm_reg_waiting_for_reg_cnf)
                       {
                          mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
                          mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                          mm_send_mmr_reg_cnf();
                          mm_reg_waiting_for_reg_cnf = FALSE;
                          mm_ready_to_send_reg_cnf = FALSE;
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
                }
                else
                {
                   if(mm_waiting_for_stop_mode_cnf)
                   {
                     MSG_HIGH_DS_0(MM_SUB,"=MM= LU will be initiated after camping on new RAT ");
                     mm_substate_control(MM_STD_UPDATE_PENDING);
                     mm_pending_lu_request = NORMAL_LU;
                   } 

                   else if ((mm_idle_substate == MM_NORMAL_SERVICE) ||
                            (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE))
                   {
                     if(mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE)
                     {
                       if (lu_cause == MM_LU_CAUSE_RESERVED) 
                       {
                         if((mm_possible_imsi_attach) && (mm_system_information.ATT))
                         {
                           lu_cause = MM_LU_CAUSE_IMSI_ATTACH;
                         }
                         else
                         {
                           lu_cause = MM_LU_CAUSE_NORMAL;
                         }
                       }
                       mm_initiate_lu( lu_cause );
                     }
                   }
                   else if (mm_idle_substate == MM_PLMN_SEARCH_NORMAL_SERVICE )
                   {
                     mm_pending_lu_request = NORMAL_LU;
                   }
#ifdef FEATURE_CONNECTED_MODE_SERV_IND
                   else if (((mm_idle_substate == MM_NO_CELL_AVAILABLE) ||
                             (mm_idle_substate == MM_PLMN_SEARCH) ||
                             (mm_idle_substate == MM_LOCATION_UPDATE_NEEDED)) &&
                             (mm_get_lu_cause() == MM_LU_CAUSE_PERIODIC))
                   {
                     mm_pending_lu_request = NORMAL_LU;
                   }
#endif  
                   else if((mm_idle_substate == MM_LIMITED_SERVICE) &&
                           (mm_timer_status[TIMER_T3310] == TIMER_STOPPED) &&
                           (mm_timer_status[TIMER_T3311] == TIMER_STOPPED))
                   {
                      mmr_service_ind.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
                      mmr_service_ind.service_state = mm_serving_plmn.info;
                      mm_send_mmr_service_ind( &mmr_service_ind );
                   }

                }

            break;
#endif
#ifdef FEATURE_ENHANCED_NW_SELECTION
         case TIMER_FOREGROUND_SEARCH:

            mm_get_foreground_timer_expiry_lai(foreground_timer_expiry_lai);

            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) ||
                ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                 (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
#ifdef FEATURE_NAS_REL10
                ||
                ((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                      &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
                )
#endif 
            )
            {
              MSG_HIGH_DS_0(MM_SUB, "=MM= Ignoring Foregound search timer expiry");     
            }
            else if ((memcmp(mm_serving_lai,foreground_timer_expiry_lai,LAI_SIZE) ==0) &&
                        (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
            {
              mm_clear_lu_attempt_counter();
              mm_initiate_lu(MM_LU_CAUSE_NORMAL);
            }
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2))
            {
              gmm_handle_message( msg_p);
            }
            mm_process_foreground_seach_tmer_expiry();

            if((memcmp(mm_serving_lai,foreground_timer_expiry_lai,LAI_SIZE) !=0) &&
               (mm_foreground_search_lai_timer_list.length == 0))
            {
              if(mm_check_connection_active((sys_modem_as_id_e_type)mm_sub_id,(sys_modem_as_id_e_type)mm_as_id) == TRUE)      
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= Start TIMER_FORCE_OOS - MM connection active");
                mm_start_timer(TIMER_FORCE_OOS , 10000);
              }
              else
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= Force No Service");
                mm_substate_control( MM_STD_NO_SERVICE );
                mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
                mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
                mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                mm_serving_plmn.info.reg_service_domain,
                                                FALSE,
                                                SYS_SRV_STATUS_NO_SRV,
                                                FALSE );
                if(mm_reg_waiting_for_reg_cnf)
                {
                  mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                  /* Set the PLMN service stat information in the REG_CNF primitive */
                  mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                  mm_send_mmr_reg_cnf();
                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf   = FALSE;
                }
                else
                {
                  mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                  mmr_service_ind.service_state = mm_serving_plmn.info;
                  mm_send_mmr_service_ind( &mmr_service_ind );
                }
              }
            }
            break;

        case TIMER_FORCE_OOS:
            if (mm_foreground_search_lai_timer_list.length == 0)
            {
              if(mm_check_connection_active((sys_modem_as_id_e_type)mm_sub_id,(sys_modem_as_id_e_type)mm_as_id) == TRUE)      
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= Again Start TIMER_FORCE_OOS - MM connection active");
                mm_start_timer(TIMER_FORCE_OOS , 10000);
              }
              else
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= Force No Service");
                mm_substate_control( MM_STD_NO_SERVICE );
                mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
                mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
                mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                mm_serving_plmn.info.reg_service_domain,
                                                FALSE,
                                                SYS_SRV_STATUS_NO_SRV,
                                                FALSE );
                if(mm_reg_waiting_for_reg_cnf)
                {
                  mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                  /* Set the PLMN service stat information in the REG_CNF primitive */
                  mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                  mm_send_mmr_reg_cnf();
                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf   = FALSE;
                }
                else
                {
                  mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                  mmr_service_ind.service_state = mm_serving_plmn.info;
                  mm_send_mmr_service_ind( &mmr_service_ind );
                }
              }
            }
            break;
#endif
         case TIMER_MM_FORBIDDEN_LAI_CLEAR:

            /*
             * timer expires every 12-24 hours. Response is to clear all the
             * forbidden LAIs
             */

            mm_clear_forbidden_LAIs();
#ifdef FEATURE_FEMTO_CSG
            /* clear rejected csg list */
            mm_clear_rejected_csg_list();
#endif
            mm_start_timer( TIMER_MM_FORBIDDEN_LAI_CLEAR, DEFAULT_TIMEOUT );
            break;

         default:
            MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected timer id %d", msg_p->cmd.timer_expiry.timer_id, 0,0);
            break;
      }

   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected message set/id %d", msg_p->cmd.hdr.message_set * 0x100 +
               msg_p->cmd.hdr.message_id, 0,0);
   }
}


/*===========================================================================

FUNCTION MM_INITIATE_LU

DESCRIPTION
  This function starts the first location updating procedure, for a location
  update event. There may be more than one if the first one fails.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_initiate_lu( byte location_update_cause )
{
   inter_task_lai_T current_lai;
   mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);   
   boolean routine_man_selection_success = TRUE;
   boolean is_cell_forbidden = FALSE;

#ifdef FEATURE_FEMTO_CSG
  boolean is_manual_csg_selection_success = FALSE;
#endif 

   mm_lu_start_reason = mm_pending_lu_request;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   current_lai.PLMN_id.identity[0] = mm_serving_plmn.info.plmn.identity[0];
   current_lai.PLMN_id.identity[1] = mm_serving_plmn.info.plmn.identity[1];
   current_lai.PLMN_id.identity[2] = mm_serving_plmn.info.plmn.identity[2];
   current_lai.location_area_code[0] = mm_serving_plmn.lac.lac[0];
   current_lai.location_area_code[1] = mm_serving_plmn.lac.lac[1];

   if (mm_interrat_state == MM_UMTS_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS)
   {
#ifdef FEATURE_GSM_GPRS
      gmmllc_gmm_tlli_assignment() ;
#endif
   }
   if (!(PLMN_MATCH( mm_serving_lai,
                    mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity ) ||
        (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
         sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn))) &&
        (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                    SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
   {
      routine_man_selection_success = FALSE;
      MSG_HIGH_DS_0(MM_SUB, "=MM= Different PLMN found during MAN_LIMITED_ACQ");
   }   
   if( (mm_check_forbidden_national_roaming(&current_lai)) ||
       (mm_check_forbidden_regional_roaming(&current_lai)) || 
#ifdef FEATURE_DUAL_SIM 
       (reg_sim_per_subs_plmn_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id ))
#else
       (reg_sim_plmn_forbidden( mm_serving_plmn.info.plmn ))
#endif
       )
   {
     is_cell_forbidden = TRUE;
     MSG_HIGH_DS_0(MM_SUB, "=MM= Cell is forbidden");
   }

#ifdef FEATURE_FEMTO_CSG
   if((mm_serving_plmn.info.csg_info.csg_id == mmcoord_pending_reg_message.cmd.mmr_reg_req.csg_id) &&
       (sys_plmn_match(mm_serving_plmn.info.plmn, mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn)) &&
       (mm_serving_plmn.info.csg_info.csg_id != SYS_CSG_ID_INVALID))
   {
     is_manual_csg_selection_success = TRUE;
   }
#endif 
/* Check AC blocking restrictions */
#ifdef FEATURE_NAS_REL10
   if ((location_update_cause == MM_LU_CAUSE_PERIODIC)&&
       (mm_is_per_ms_t3212_present) && 
       (mm_stored_per_ms_t3212_value == 0))
   {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= periodic LU not allowed in this cell - %d",
                  mm_stored_per_ms_t3212_value);
      mm_substate_control(MM_STD_NORMAL_SERVICE);
      mm_T3213_timed_out = FALSE;
      mm_pending_lu_request = NO_LU;
   }
   else if((location_update_cause == MM_LU_CAUSE_PERIODIC) && 
            (!mm_is_per_ms_t3212_present) && 
            (mm_system_information.T3212_timer_value == 0))
   {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= periodic LU not allowed in this cell - %d",
                    mm_system_information.T3212_timer_value);
      mm_substate_control(MM_STD_NORMAL_SERVICE);
      mm_T3213_timed_out = FALSE;
      mm_pending_lu_request = NO_LU;
   } 
#else
   if ((location_update_cause == MM_LU_CAUSE_PERIODIC) && ( mm_system_information.T3212_timer_value == 0 ))
   {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= periodic LU not allowed in this cell - %d",
                    mm_system_information.T3212_timer_value);
      mm_substate_control(MM_STD_NORMAL_SERVICE);
      mm_T3213_timed_out = FALSE;
      mm_pending_lu_request = NO_LU;
   }
#endif
   else if (mm_check_if_cs_cell_access_barred(cs_cell_access) == TRUE)
   {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= LU not perfomed because AC blocking %d",
                    cs_cell_access);
      mm_substate_control(MM_STD_UPDATE_PENDING);
   }
   else if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
   {
      mm_substate_control(MM_STD_NO_IMSI);
      MSG_HIGH_DS_0(MM_SUB, "=MM= LU is not performed - SIM is invalid for CS");
   }
   else if ((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
            (((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)||
              (!routine_man_selection_success)) ||
             (((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                  SYS_NETWORK_SELECTION_MODE_MANUAL)||
                 (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                  SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))&&
                (!mm_manual_mode_user_selection_procedure)) ||
               ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                 SYS_NETWORK_SELECTION_MODE_AUTOMATIC)||
                (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                   SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))) &&
             ((is_cell_forbidden)
#ifdef FEATURE_FEMTO_CSG
              && !(is_manual_csg_selection_success)
#endif 
                ))))


   {
     MSG_HIGH_DS_0(MM_SUB, "=MM= LU not performed because LAI is forbidden");
   }
   else if ((mmcoord_pending_reg_message.cmd.hdr.message_set == (int) MS_MM_REG) &&
            (mmcoord_pending_reg_message.cmd.hdr.message_id == (int) MMR_STOP_MODE_REQ)&&
            (mm_nreg_req_received))
   {
     MSG_HIGH_DS_0(MM_SUB, "=MM= deactivating , LU trigger ignored");
   }  
   else if (((mm_state == MM_WAIT_FOR_RR_CONNECTION_MM) ||
             (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
             (mm_state == MM_CONNECTION_ACTIVE) ||
             (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) ||
             (mm_state == MM_REESTABLISHMENT_INITIATED) ||
             (mm_state == MM_WAIT_FOR_REESTABLISH_DECISION) ||
             (mm_state == MM_WAIT_FOR_RR_CONNECTION_REESTABLISHMENT) ||
             (mm_state == MM_WAIT_FOR_NETWORK_COMMAND)) &&
            (mm_lu_pended_nmo_unknown))
   {
     MSG_HIGH_DS_1(MM_SUB ,"=MM= NMO UNKNOWN: not MM_IDLE %d - LU is not performed ", mm_state);
   }
   else
   {
      if ( GS_IS_NOT_UNDEFINED( location_update_cause ) )
      {
         lu_cause = location_update_cause;
      }   
      mm_T3213_timed_out = FALSE;
      
      
#ifdef FEATURE_DSAC      
      mm_last_procedure_to_update_registration = 
          (mm_last_procedure_to_update_registration & (~MM_COMBINED));
      mm_last_procedure_to_update_registration |= MM_LU;
#endif 
#ifdef FEATURE_ENHANCED_NW_SELECTION
      if (reg_sim_read_ens_flag() && ((mmsim_is_ef_rat_present()) ||
         (gmm_nv_disable_call_type & GMM_CALL_TYPE_VOICE_MASK)))
      {
         mm_force_lu = FALSE;
      }
#endif

      mm_state_control( MM_STD_LU_REQUIRED ); 
      mm_update_last_reg_dsac_info();

      mm_initiate_lu_upon_entry_into_new_cell = FALSE;
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
      mm_pending_lu_type = MM_LU_CAUSE_RESERVED;
#endif
      mm_idle_transition_timer = NULL;
      mm_lu_pended_nmo_unknown = FALSE;
#ifdef FEATURE_NAS_REL10
      if(mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
      {
        mm_stop_timer(TIMER_T3246);
        sys_plmn_undefine_plmn_id(&mm_3gpp_t3246_plmn_id);
      }
#endif
/*Reset LU flag since LU is initiated*/
#if defined(FEATURE_LTE) && defined(FEATURE_NAS_REL10)
      mm_is_lu_to_be_initiated_t3346_ltogw = FALSE;
#endif
      if ((mm_state == MM_LOCATION_UPDATE_INITIATED)  ||
          ( mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
      {
   
        mm_pending_lu_request = NO_LU;

        gmm_irat_lu_pending = NO_LU;
        /* GS association is not happened in LU procedure */

        MSG_MED_DS( MM_SUB, "=MM= GS association is removed when LU is triggered", 0,0,0 );
        mm_gs_association_established = FALSE;
        (void) memscpy( mm_previous_serving_lai,LAI_SIZE,
                       mm_serving_lai,
                       LAI_SIZE );

#ifdef FEATURE_LTE
        //If LU is pending after CSFB set state to NULL
        if(mm_csfb_state == MM_CSFB_STATE_LU_PENDING)
        {
          mm_csfb_set_state(MM_CSFB_STATE_NULL);
        }
#endif /*FEATURE_LTE*/

        mm_previous_idle_substate = mm_idle_substate;

        if((mm_location_information.location_update_status != UPDATED) ||
           (memcmp( mm_serving_lai,
                          mm_location_information.lai,
                          LAI_SIZE ) != 0 ))
        {
          mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                          SYS_SRV_DOMAIN_NO_SRV,
                                          FALSE,
                                          SYS_SRV_STATUS_LIMITED,
                                          FALSE );
        }
      }
   }
}


/*===========================================================================

FUNCTION MM_CHECK_LU_FAIL_ACTION

DESCRIPTION
  Function to deal with the return to mm_idle state after a location
  update failure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_check_lu_fail_action( void )
{
   byte                    substate_event = MM_STD_INVALID_EVENT;
   mmr_service_ind_s_type  mmr_service_ind;
   sm_cmd_type *sm_cmd;
   byte count = 0;
   byte location_information[LAI_SIZE];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED; /* Set to default cause */

   if (mm_timer_status[TIMER_T3210] != TIMER_STOPPED)
   {
      mm_stop_timer( TIMER_T3210);
   }

   MSG_HIGH_DS_1(MM_SUB ,"=MM= LU Reject cause = %d", mm_lu_reject_info.reject_cause);

   if ((mm_lu_reject_info.reject_cause == IMSI_UNKNOWN_IN_HLR) ||
       (mm_lu_reject_info.reject_cause == ILLEGAL_MS) ||
       (mm_lu_reject_info.reject_cause == ILLEGAL_ME))
   {
      /* ---------------------------------------------------------
      ** The mobile station shall set the update status to ROAMING
      ** NOT ALLOWED (and  store it in the SIM according to clause
      ** 4.1.2.2),  and  delete any TMSI, stored LAI and ciphering
      ** key sequence number and shall consider the SIM as invalid
      ** for  non-GPRS  services  until  switch-off  or the SIM is
      ** removed. (Section 4.4.4.7 of TS24.008)
      ** ---------------------------------------------------------*/
      mm_delete_lu_status( ROAMING_NOT_ALLOWED );
      mm_previous_idle_substate = NULL;

      /* Set the Serving_plmn sim status to CS invalid */
      mm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_INVALID,
                                      SYS_SRV_DOMAIN_NO_SRV,
                                      FALSE,
                                      SYS_SRV_STATUS_LIMITED,
                                      TRUE );

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

      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_ONLY,
                                    mm_lu_reject_info.reject_cause,
                                    mm_serving_plmn.info.plmn,
                                    mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                    , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                    , mm_serving_plmn.info.active_rat
                                         );  

      if (mm_reg_waiting_for_reg_cnf)
      {
         if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
              ( (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2) 
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
              ||(mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
              )
            )
         {
            /* -------------------------------
            ** Proceed to GMM Attach procedure
            ** ------------------------------- */
            mm_ready_to_send_reg_cnf = TRUE;
         }
         else 
         {
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();
            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
         }
      }
      else
      {
         if((gmm_state != GMM_REGISTERED_INITIATED) &&
            (gmm_state != GMM_ROUTING_AREA_UPDATING_INITIATED) &&
            (mm_timer_status[TIMER_T3311] == TIMER_STOPPED))
         {
            mmr_service_ind.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;
         }
         mmr_service_ind.service_state = mm_serving_plmn.info;
         mm_send_mmr_service_ind( &mmr_service_ind );
      }

      /* Reject any pending requests for CM service */
      if (mm_held_cnm_service_req.present)
      {
         if (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL)
         {
           mm_check_pending_requests();
         }
         else
         {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
           mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED ,MMCC_SMS_CS);
#else
           mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED );
#endif
         }
      }
      mm_substate_control( MM_STD_NO_IMSI );
   }
   else if ((mm_lu_reject_info.reject_cause == PLMN_NOT_ALLOWED) ||
            (mm_lu_reject_info.reject_cause == LA_NOT_ALLOWED)   ||
            (mm_lu_reject_info.reject_cause == NATIONAL_ROAMING_NOT_ALLOWED) ||
            (mm_lu_reject_info.reject_cause == NO_SUITABLE_CELLS_IN_LA)
#ifdef FEATURE_FEMTO_CSG
            || (mm_lu_reject_info.reject_cause == CSG_NOT_AUTHORIZED && 
                mm_serving_csg_id != SYS_CSG_ID_INVALID)
#endif
           )
   {
      gmm_gprs_attach_pending = FALSE; 
      gmm_ra_update_pending = FALSE;
      substate_event = MM_STD_LIMITED_SERVICE;
      mm_previous_idle_substate = NULL;

      if ((mm_lu_reject_info.reject_cause == PLMN_NOT_ALLOWED) ||
          (mm_lu_reject_info.reject_cause == LA_NOT_ALLOWED))
      {
         /* -----------------------------------------------------------
         ** The mobile station shall delete any LAI, TMSI and ciphering
         ** key sequence number stored in the SIM, reset the attempt
         ** counter, set the update status to ROAMING NOT ALLOWED (and
         ** store it in the SIM according to section 4.1.2.2). The
         ** mobile station shall store the LAI or the PLMN identity in
         ** the suitable forbidden list, i.e. in the "forbidden PLMN
         ** list" for cause #11, in the list of "forbidden location
         ** areas for regional provision of service" for cause #12, and
         ** in the list of "forbidden location areas for roaming" for
         ** cause #13. In addition, the MS will memorize if cause #13
         ** was received, so to perform a PLMN selection instead of a
         ** cell selection when back to the MM IDLE state.
         ** -----------------------------------------------------------*/
         mm_delete_lu_status( ROAMING_NOT_ALLOWED );
      }
      else
      {
         mm_location_information.location_update_status = ROAMING_NOT_ALLOWED;

         /* --------------------------------
         ** Copy location information to SIM
         ** -------------------------------- */
         mmsim_write_loci();
      }

      if ((mm_lu_reject_info.reject_cause == PLMN_NOT_ALLOWED) ||
          (mm_lu_reject_info.reject_cause == LA_NOT_ALLOWED))
      {
         mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                         SYS_SRV_DOMAIN_NO_SRV,
                                         TRUE,
                                         SYS_SRV_STATUS_LIMITED,
                                         TRUE );

         if (gmm_state != GMM_NULL)
         {
            gmm_set_state( GMM_DEREGISTERED );

            mm_is_gmm_moved_dereg_due_to_csreg = TRUE;

            if (mm_timer_status[ TIMER_T3310 ] == TIMER_ACTIVE)
            {
              mm_stop_timer(TIMER_T3310);
            }

            sm_cmd = sm_get_cmd_buf();

            if (sm_cmd == NULL)
            {
               MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
            }

            sm_cmd->header.message_set = MS_GMM_SM;
            sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
            sm_cmd->cmd.gmm_gprs_rel_ind.cause = 
                 mm_convert_gmm_to_sm_cause(mm_lu_reject_info.reject_cause);
#ifdef FEATURE_DUAL_SIM
            sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

            MSG_HIGH_DS_0(MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

            sm_put_cmd( sm_cmd );
         }
      }
      else
      {
         mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                         SYS_SRV_DOMAIN_NO_SRV,
                                         FALSE,
                                         SYS_SRV_STATUS_LIMITED,
                                         TRUE );

         if (gmm_state != GMM_DEREGISTERED && gmm_state != GMM_NULL)
         {
           gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                            mm_serving_plmn.info.reg_service_domain,
                                            TRUE,
                                            SYS_SRV_STATUS_LIMITED,
                                            TRUE );
         }

      }

      if (mm_lu_reject_info.reject_cause == LA_NOT_ALLOWED)
      {
#ifdef FEATURE_FORBID_LAI_UPON_REGIONAL_ROAMING_REJECT
         /* ---------------------------------------------------------
         ** This FEATURE was introduced to support a customer request
         ** for a non-spec compliant PLMN selection on receipt of
         ** reject cause LA_NOT_ALLOWED
         ** --------------------------------------------------------- */
         /* Store LAI in forbidden_LAI_list_for_national_roaming */
         mm_add_forbidden_national_roaming( (inter_task_lai_T *) &mm_serving_lai[0] );
#else
         /* Store LAI in forbidden_LAI_list_for_regional_provision_of_service */
         mm_add_forbidden_regional_roaming( (inter_task_lai_T *) &mm_serving_lai[0] );
#endif
      }
      else if (mm_lu_reject_info.reject_cause == NATIONAL_ROAMING_NOT_ALLOWED)
      {
         /* Store LAI in forbidden_LAI_list_for_national_roaming */
         mm_add_forbidden_national_roaming( (inter_task_lai_T *) &mm_serving_lai[0] );
         /* 24.008, 4.2.3 */
         substate_event = MM_STD_LIMITED_SERVICE;
      }
      else if (mm_lu_reject_info.reject_cause  == NO_SUITABLE_CELLS_IN_LA)
      {
         /* Store LAI in forbidden_LAI_list_for_national_roaming */
         mm_add_forbidden_national_roaming( (inter_task_lai_T *) &mm_serving_lai[0] );
         /* 24.008, 4.2.3 */
         substate_event = MM_STD_LIMITED_SERVICE;
         /* add LAI in LAI reject list */ 
         mm_add_lai_in_reject_list((inter_task_lai_T *) &mm_serving_lai[0]);
      }
#ifdef FEATURE_FEMTO_CSG
      else if(mm_lu_reject_info.reject_cause == CSG_NOT_AUTHORIZED)
      {
        substate_event = MM_STD_LIMITED_SERVICE;
        /* Remove CSG from allowed or operator lsit */
        mm_delete_csg_from_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info);
      }
#endif 
      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_ONLY,
                                    mm_lu_reject_info.reject_cause,
                                    mm_serving_plmn.info.plmn,
                                    mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                    , mm_serving_plmn.info.csg_info.csg_id
                      #endif
                                    , mm_serving_plmn.info.active_rat
                                   );


      mm_substate_control( substate_event );
      gmm_set_substate( GMM_LIMITED_SERVICE );

      if (mm_reg_waiting_for_reg_cnf)
      {
        if((mmcoord_reg_req_pending == FALSE) ||
           (mm_lu_reject_info.reject_cause == PLMN_NOT_ALLOWED))
        {
          mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
          mmcoord_mmr_reg_cnf.cause =
             mm_convert_to_reg_cause( mm_lu_reject_info.reject_cause );
          mm_send_mmr_reg_cnf();
          mm_reg_waiting_for_reg_cnf = FALSE;
          mm_ready_to_send_reg_cnf   = FALSE;
        }
      }
      else
      {
         mmr_service_ind.service_state = mm_serving_plmn.info;
         mmr_service_ind.cause = mm_convert_to_reg_cause (mm_lu_reject_info.reject_cause);
         mm_send_mmr_service_ind( &mmr_service_ind );
      }
      /* Reject any pending requests for CM service */
      if (mm_held_cnm_service_req.present)
      {
         if (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL)
         {
            mm_check_pending_requests();
         }
         else
         {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED ,MMCC_SMS_CS);
#else
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED );
#endif
         }
      }
   }
   else if (( mm_lu_reject_info.reject_cause == CAUSE_ACCESS_BARRED ) ||
            ( mm_lu_reject_info.reject_cause == CAUSE_IMM_ASSIGN_REJECT) ||
            ( mm_lu_reject_info.reject_cause == CAUSE_LAI_CHANGED)||
            ( mm_lu_reject_info.reject_cause == CAUSE_CONN_EST_NOT_ATTEMPTED))
   {
      /*
       * do nothing - Access is barred due to access class control
       * (just stop the timer which indicated that LU attempt was in
       * progress)
       * - When LU is failed due to Imm Assignment Reject.
       * - When LU is failed due to change in LAI.
       */
       mm_idle_transition_timer = NULL;
#ifdef FEATURE_LTE
       /* If previous LU was started due to IRAT from LTE then set
       *  pending lu request again to IRAT LU.
       *  Note: for CAUSE_IMM_ASSIGN_REJECT we would have set pending
       *  LU flag to T3122_EXPIRED in 
       */
       if((mm_lu_start_reason == LTE_IRAT_LU) &&
          ((mm_lu_reject_info.reject_cause == CAUSE_ACCESS_BARRED ) ||
           (mm_lu_reject_info.reject_cause == CAUSE_LAI_CHANGED)))
       {
         mm_pending_lu_request = LTE_IRAT_LU;
         mm_substate_control(MM_STD_UPDATE_PENDING);
       }
#endif

      /* Let silent redial kick for LAI changed case */
      if (((mm_lu_reject_info.reject_cause == CAUSE_LAI_CHANGED) ||
           (mm_lu_reject_info.reject_cause == CAUSE_CONN_EST_NOT_ATTEMPTED)) &&
          (mm_held_cnm_service_req.present))
      {
         if (mm_held_cnm_service_req.establishment_cause != (int) MM_RR_EMERGENCY_CALL)
         {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL ,MMCC_SMS_CS);
#else
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL );
#endif
         }
      }
      if( mm_lu_reject_info.reject_cause == CAUSE_CONN_EST_NOT_ATTEMPTED)
      {
        mm_substate_control(MM_STD_UPDATE_PENDING);
      }
      /* Reject any pending requests for CM service */
      if ((mm_lu_reject_info.reject_cause == CAUSE_IMM_ASSIGN_REJECT) && 
          (mm_held_cnm_service_req.present))
      {
          if (mm_held_cnm_service_req.establishment_cause != (int) MM_RR_EMERGENCY_CALL)
         {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED ,MMCC_SMS_CS);
#else
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED );
#endif
         }
      }


   }
#ifdef FEATURE_CONNECTED_MODE_SERV_IND
   else if( mm_lu_reject_info.reject_cause == MM_CAUSE_RRC_OOS_CONNECTED_MODE &&
            mm_get_lu_cause() == MM_LU_CAUSE_PERIODIC )
   {
      mm_pending_lu_request = T3212_EXPIRED; 
      /* Reject any pending requests for CM service */
      if (mm_held_cnm_service_req.present)
      {
         if (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL)
         {
            mm_check_pending_requests();
         }
         else
         {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_IMMED_RETRY ,MMCC_SMS_CS);
#else
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_IMMED_RETRY );
#endif
         }
      }
   }
   else if( mm_lu_reject_info.reject_cause == MM_CAUSE_RRC_CONN_LOCAL_REL_OOS &&
            mm_get_lu_cause() == MM_LU_CAUSE_PERIODIC )
   {
      /* -------------------------------------------------------------------------
      ** Do nothing. RRC connection is locally released in CELL_FACH state due to 
      ** OOS for saving battery life (non-spec compliant). This should not be treated 
      ** as abnormal failure as it is local. Attempt counter should also not be
      ** incremented.
      ** -------------------------------------------------------------------------*/
      /* Reject any pending requests for CM service */
      if (mm_held_cnm_service_req.present)
      {
         if (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL)
         {
            mm_check_pending_requests();
         }
         else
         {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_IMMED_RETRY ,MMCC_SMS_CS);
#else
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_IMMED_RETRY );
#endif
         }
      }
   }
#endif   
#ifdef FEATURE_DUAL_SIM
   else if ( mm_lu_reject_info.reject_cause == CAUSE_RADIO_UNAVAILABLE)
   {
      if (mm_held_cnm_service_req.present)
      {
         if (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL)
         {
            mm_check_pending_requests();
         }
         else
         {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_ABORT_RADIO_UNAVAILABLE ,MMCC_SMS_CS);
#else
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_ABORT_RADIO_UNAVAILABLE );
#endif
         }
      }
      mm_idle_transition_timer = TIMER_MM_RADIO_RETRY;
#ifdef FEATURE_LTE
       /* If previous LU was started due to IRAT from LTE then set
       *  pending lu request again to IRAT LU.
       *  Note: for CAUSE_IMM_ASSIGN_REJECT we would have set pending
       *  LU flag to T3122_EXPIRED in 
       */
       if(mm_lu_start_reason == LTE_IRAT_LU)
       {
         mm_pending_lu_request = LTE_IRAT_LU;
         mm_substate_control(MM_STD_UPDATE_ATTEMPTED);
       }
#endif
   }
#endif
#ifdef FEATURE_SGLTE
   else if ( mm_lu_reject_info.reject_cause == CAUSE_SRVCC_SGLTE_CS_HANDOVER)
   {
      if (mm_held_cnm_service_req.present)
      {
         if (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL)
         {
            mm_check_pending_requests();
         }
         else
         {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, MM_REJ_WRONG_STATE ,MMCC_SMS_CS);
#else
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, MM_REJ_WRONG_STATE );
#endif
         }
      }
   }
#endif
#ifdef FEATURE_NAS_REL10
   else if (mm_lu_reject_info.reject_cause == CONGESTTION && (mm_t3246_backoff_timer_value != 0))
   {

      /******************************************************************************************************************
    **Cause #22- T3246 Started- The mobile station shall abort the location updating procedure, reset the attempt counter, 
    **set the MM update status to U2 NOT UPDATED and change to state MM IDLE sub-state ATTEMPTING TO UPDATE.
    **(24.008 section 4.4.4.7) ------------------------------------------------*/
          /* -------------------------------------
        ** Reset the MM LU attempt counter
        ** ------------------------------------- */
     mm_clear_lu_attempt_counter();
     MSG_HIGH_DS_1(MM_SUB ,"=MM= MM LU Attempt counter = %d",
                  mm_lu_attempt_counter);

     mm_location_information.location_update_status = NOT_UPDATED;
   
     mm_idle_transition_timer = NULL;
      /*
      * copy location information to SIM
      */
     mmsim_write_loci();

     /* Set the Serving_plmn sim status to CS invalid */
     mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                     SYS_SRV_DOMAIN_NO_SRV,
                                     FALSE,
                                     SYS_SRV_STATUS_LIMITED,
                                     TRUE );


     mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_ONLY,
                                    mm_lu_reject_info.reject_cause,
                                    mm_serving_plmn.info.plmn,
                                    mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                    ,mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                    , mm_serving_plmn.info.active_rat
                            );

     mm_substate_control( MM_STD_UPDATE_ATTEMPTED );

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
     *24.008 4.7.1.9 The MS shall start the timer T3346  if the MS *
     * receives  the reject cause values #22 *
     **************************************************************/

     if (mm_store_integrity_protected == TRUE)
     {
       mm_start_timer(TIMER_T3246, mm_t3246_backoff_timer_value);
     }
     else
     {
       mm_t3246_backoff_timer_value = ((15 + mm_get_random_value(15)) * 60 * 1000);
       mm_start_timer(TIMER_T3246, mm_t3246_backoff_timer_value);
     }

     /*saves congestion plmn associated with the timer*/
     memscpy((void *)&mm_3gpp_t3246_plmn_id, sizeof(sys_plmn_id_s_type),(void *)&(mm_serving_plmn.info.plmn), sizeof(sys_plmn_id_s_type));
     mm_t3246_backoff_timer_value = 0;

     /* After receiving LU Reject with cause congestion, Rel-11 version 
      * of spec allows (in-directly) high  AC users to retry LU.
      *
      * But when such UE receives LU Reject with cause 22, first check if there is 
      * any pending EM call. If yes, process it first and then do LU.
      * If no EM call pending, do LU straightaway for high AC users.
      *
      * Any normal call which is pending gets rejected - with redial_allowed cause
      * for high AC users as LU is going to succeed in some time. For other users, call 
      * is rejected with permanent cause.
      */
     if (mm_held_cnm_service_req.present)
     {
       if (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL)
       {
         mm_check_pending_requests();
       }

#ifdef FEATURE_NAS_REL11
       if(mm_state == MM_IDLE) /* If we started call in above block, pend LU */
       {
         if(mm_is_rrc_access_class_11_to_15() == TRUE)
         {
           MSG_HIGH_DS_0(MM_SUB ,"=MM= Retrying LU in NW congestion for High AC users ");
           mm_initiate_lu(mm_get_lu_cause());
         }
       }  
       else
       {
         mm_pending_lu_request = NORMAL_LU;
       }
#endif

       if (mm_held_cnm_service_req.establishment_cause != (int) MM_RR_EMERGENCY_CALL)
       {
         byte cause = AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED;
         if(mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
         {
           cause = AS_REJ_LOW_LEVEL_FAIL;
         }
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
         mm_send_mmcnm_est_rej(
            AS_REJECT_CAUSE, cause ,MMCC_SMS_CS);
#else
         mm_send_mmcnm_est_rej(
            AS_REJECT_CAUSE, cause);
#endif
       }
     }
     

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
#endif

   else
   {
      if ((mm_lu_reject_info.reject_cause == MM_SEMANTICALLY_INCORRECT_MESSAGE) ||
          (mm_lu_reject_info.reject_cause == MM_INVALID_MANDATORY_INFORMATION) ||
          (mm_lu_reject_info.reject_cause == MM_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED) ||
          (mm_lu_reject_info.reject_cause == MM_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED) ||
          (mm_lu_reject_info.reject_cause == MM_PROTOCOL_ERROR_UNSPECIFIED)
#ifdef FEATURE_NAS_REL10
      ||  ((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10) &&
            mm_lu_reject_info.reject_cause == CONGESTTION)
#endif
      )
      {
         MSG_HIGH_DS_0(MM_SUB,"=MM= Attempt counter is set to 4");

         for ( count=0; count< mm_lu_attempt_counter && count< MAX_ATTEMPT_COUNTER_LAU; count++)
         {
           mm_lu_reject_cause[count] = 0xFF;
         }
      }
      else
      {
         mm_lu_attempt_counter++;
      }

      /* Store LU reject cause for each failure attempt*/
      if(mm_lu_attempt_counter <= MAX_ATTEMPT_COUNTER_LAU && mm_lu_attempt_counter > 0 )
      {
        mm_lu_reject_cause[mm_lu_attempt_counter -1] = mm_lu_reject_info.reject_cause;
      }

      MSG_HIGH_DS_1(MM_SUB ,"=MM= Attempt counter = %d", mm_lu_attempt_counter);

      /* Reject any pending requests for CM service
       */
      if (mm_held_cnm_service_req.present)
      {
         if (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL)
         {
            mm_check_pending_requests();
         }
         else
         {
           if(mm_lu_start_reason == LTE_IRAT_LU)
           {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
             mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_CS);
#else
             mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
           }
           else
           {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
             mm_send_mmcnm_est_rej(
                AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED ,MMCC_SMS_CS);
#else
             mm_send_mmcnm_est_rej(
                AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED );
#endif
           }
         }
       }

      if ((mm_lu_reject_info.reject_cause == NETWORK_FAILURE)||
          (mm_lu_reject_info.reject_cause == MSC_TEMPORARILY_NOT_REACHABLE))
      {
         mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_ONLY,
                                    mm_lu_reject_info.reject_cause,
                                    mm_serving_plmn.info.plmn,
                                    mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                    ,mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                    , mm_serving_plmn.info.active_rat
                            );
      }

      location_information[0] = 0xFF;
      location_information[1] = 0xFF;
      location_information[2] = 0xFF;
      location_information[3] = 0x00;
      location_information[4] = 0x00;
      if(memcmp(mm_serving_lai,location_information,PLMN_SIZE) == 0)
      {
        memscpy(location_information,LAI_SIZE,mm_previous_serving_lai,LAI_SIZE);
      }
      else
      {
        memscpy(location_information,LAI_SIZE,mm_serving_lai,LAI_SIZE);
      }
      if (((mm_location_information.location_update_status == UPDATED ) &&
           ( memcmp( location_information,
                     mm_location_information.lai,
                     LAI_SIZE ) == 0 ) &&
           ( mm_lu_attempt_counter < 4 ) ) ||
           ((mm_serving_plmn.mm_service_state == SERVICE_NOT_AVAILABLE) &&
           (mm_get_lu_cause() == MM_LU_CAUSE_PERIODIC)))
      {
         mm_start_timer( TIMER_T3211, DEFAULT_TIMEOUT );
         mm_idle_transition_timer = TIMER_T3211;
         mm_previous_idle_substate = MM_ATTEMPTING_TO_UPDATE;
      }
      else if ((mm_location_information.location_update_status != UPDATED) ||
               (memcmp( location_information,
                        mm_location_information.lai,
                        LAI_SIZE ) != 0 ) ||
               (mm_lu_attempt_counter >= 4 ))
      {
         mm_delete_lu_status( NOT_UPDATED );

         if(mm_idle_substate != MM_LIMITED_SERVICE) 
         {
            mm_substate_control( MM_STD_UPDATE_ATTEMPTED );
            mm_previous_idle_substate = mm_idle_substate;
         }

         /* delete equivalent PLMN list */
         mm_serving_plmn.update_equiv_plmn_list = TRUE;
         memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));


         if((memcmp( mm_serving_lai, mm_location_information.lai, LAI_SIZE ) != 0 ) ||
             (mm_lu_attempt_counter >=4))
         {
            mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                            SYS_SRV_DOMAIN_NO_SRV,
                                            FALSE,
                                            SYS_SRV_STATUS_LIMITED,
                                            TRUE );
         }
         /*
          * select timer to start on transition to MM_IDLE state
          * See 04.08 4.4.4.9 last paragraph
          */

         if ( mm_lu_attempt_counter < 4 )
         {
            mm_idle_transition_timer = TIMER_T3211;
            if(mm_reg_waiting_for_reg_cnf)
            {
              mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
            }
         }
         else
         {
            /* MAX attempted, so set manual mode user selection flag to FALSE */
            mm_manual_mode_user_selection_procedure = FALSE;
                     
            if (mm_reg_waiting_for_reg_cnf)
            {
               if((mm_lu_attempt_counter == 4) &&
                  (mm_is_foreground_search_to_be_initiated))
               {
                 mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH;
#ifdef FEATURE_NAS_REL10
                 if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                    (mm_timer_status[TIMER_T3346]== TIMER_ACTIVE))
                 {
                   mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                 }
#endif
                  /* 4 attempts done Set cs service status to limited if manual mode */
                 if(mm_managed_roaming_enabled && 
                    (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                     mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
                 {
                    mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
                 }
#ifdef FEATURE_ENHANCED_NW_SELECTION
                 if(reg_sim_read_ens_flag() &&
                    (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                     mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
                 {
                    mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
                 }

                 if (reg_sim_read_ens_flag() && (mm_lu_reject_info.reject_cause != CAUSE_RANDOM_ACCESS_FAILURE))
                 {
                    mm_add_lai_to_foreground_search_lai_timer_list(mm_serving_lai);
                 }

#endif
                if(mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
                   mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)
                {
                   mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                   mm_send_mmr_reg_cnf();

                   mm_reg_waiting_for_reg_cnf = FALSE;
                   mm_ready_to_send_reg_cnf   = FALSE;
                }
                else
                {
                  if((((gprs_attach_attempt_counter < 5)||(gmm_rau_attempt_counter < 5)) &&
                       (mm_timer_status[TIMER_T3311] == TIMER_ACTIVE)) ||
                        gmm_ra_update_pending ||
                        gmm_gprs_attach_pending ||
                       (mm_timer_status[TIMER_T3310] == TIMER_ACTIVE) ||
                       (mm_timer_status[TIMER_T3330] == TIMER_ACTIVE))
                  {
                      MSG_HIGH_DS_0(MM_SUB,"=MM= Attach Attempt counter is less than the max value so not allowed to continue with someother RAT");

                      if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                         ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2) 
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                        ||(mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
                          )
                          )
                      {
                         mm_ready_to_send_reg_cnf = TRUE;
                      }

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
               else
               {
                 mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;  
#ifdef FEATURE_NAS_REL10
                 if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                    (mm_timer_status[TIMER_T3346]== TIMER_ACTIVE))
                 {
                   mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                 }
#endif
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
                 
                 if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                     ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2) 
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                      ||(mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
#endif
                      )
                    )
                 {
                    /* -------------------------------
                    ** Proceed to GMM Attach procedure
                    ** ------------------------------- */
                    mm_ready_to_send_reg_cnf = TRUE;
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
            else
            {
               if ((mm_lu_attempt_counter == 4) &&
                   (mm_is_foreground_search_to_be_initiated))
               {
                 mmr_service_ind.cause = MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH;
                  /* 4 attempts done Set cs service status to limited if manual mode */
                 if(mm_managed_roaming_enabled && 
                    (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                     mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
                 {
                    mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
                 }
#ifdef FEATURE_ENHANCED_NW_SELECTION
                 if(reg_sim_read_ens_flag() &&
                    (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                     mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
                 {
                    mm_serving_plmn.info.cs_service_status = SYS_SRV_STATUS_LIMITED;
                 }

                 if (reg_sim_read_ens_flag() && (mm_lu_reject_info.reject_cause != CAUSE_RANDOM_ACCESS_FAILURE))
                 {
                    mm_add_lai_to_foreground_search_lai_timer_list(mm_serving_lai);
                 }

#endif
               }
               else
               {
                 mmr_service_ind.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;  
               }
#ifdef FEATURE_NAS_REL10
               if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                  (mm_timer_status[TIMER_T3346]== TIMER_ACTIVE))
               {
                 mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
               }
#endif
               mmr_service_ind.service_state = mm_serving_plmn.info;
               mm_send_mmr_service_ind( &mmr_service_ind );
            }

            mm_idle_transition_timer = TIMER_T3212;
         }
      }
   }
 #ifdef FEATURE_NAS_REL10
   mm_store_integrity_protected = FALSE;
 #endif
   mm_lu_reject_info.reject_cause = 0;
}


/*===========================================================================

FUNCTION MM_SEND_LU_REQUEST

DESCRIPTION
  This function initiates a location area update. If the location update
  attempt counter is zero, then this is the first attempt, and the reason
  for the update is in the stimulus, otherwise the stored reason that was
  last sent is used.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_lu_request( auth_security_context_id security_context )
{
   location_updating_request_msg_T  location_updating_request;
   byte                             classmark_1;
   byte                             ciph_key_seq_num;
   byte                             data_index = 0;

   byte max_data_index = (REG_SIM_IMSI_MAX_LENGTH+1) + /* Mobile identity */
                                          CLASSMARK_2_TLV_SIZE + /* Mobile station classmark for UMTS */
                                          1 /* Additional update parameters */
                                          ;

#if defined(FEATURE_LTE)
   emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
   
   emm_ctrl_data_ptr = emm_db_get_ctrl_data();
   
   ASSERT(emm_ctrl_data_ptr != NULL);
#endif

   if ( ( mm_idle_substate != MM_NO_IMSI ) &&
        ( mm_idle_substate != MM_NO_CELL_AVAILABLE ) )
   {
      location_updating_request.msg_head.IMH.message_set = MS_MM_RR;
      location_updating_request.msg_head.IMH.message_id  = (int) RR_DATA_REQ;
      location_updating_request.msg_head.skip_pd = PD_MM;
      location_updating_request.msg_head.type = LOCATION_UPDATING_REQUEST;

      ciph_key_seq_num =
                     auth_get_ciphering_key_sequence_number(security_context) ;

      location_updating_request.ciphseq_lutype =
            (byte)( ((ciph_key_seq_num & (byte)(0x07)) << (byte)(4))  +
               (lu_cause & (byte)(0x03) ) +
               (mm_follow_on_request == FALSE ? (byte)(0) : (byte)(0x08))
               );

      /*----------------------------------------------------------------
        Get lai from the SIM, which may not be the current serving LAI
        TS 24.008 9.2.15.1 LAI stored in the SIM is used
        ---------------------------------------------------------------*/

      (void)memscpy(location_updating_request.location_area_identification_data, LAI_SIZE,
                   mm_location_information.lai, LAI_SIZE );

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
      {
        classmark_params_T classmark_params;
        classmark_params.gsm_supported = mm_gsm_supported;
        classmark_params.wcdma_in_use  = is_umts_mode();
        classmark_params.MSCR99        = mm_system_information.mscr;
        classmark_params.as_id         = (sys_modem_as_id_e_type)mm_sub_id;
        classmark_1 = gs_get_classmark_1_per_nas_stacks( &classmark_params,(sys_modem_as_id_e_type)mm_as_id );
      }
#else
      classmark_1 = gs_get_classmark_1( mm_gsm_supported, is_umts_mode(), mm_system_information.mscr );
#endif

      location_updating_request.classmark_data = classmark_1;

      if ( has_valid_tmsi())
      {
         /* Copy TMSI */
         data_index = (byte)mm_pack_tmsi(location_updating_request.data);
      }
      else
      {
         /* Copy IMSI */
         data_index = (byte)mm_pack_imsi(location_updating_request.data);
      }

      if (is_umts_mode())
      {
         /* In UMTS, the req shall include classmark 2 - 24.008 9.2.15.3*/

         /* set the classmark2 IEI */
         if(data_index + CLASSMARK_2_TLV_SIZE <= max_data_index)
         {
           location_updating_request.data[data_index] = IEI_CLASSMARK2_UMTS;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
           {
             classmark_params_T classmark_params;
             classmark_params.gsm_supported   = mm_gsm_supported;
             classmark_params.wcdma_supported = mm_wcdma_supported;
             classmark_params.wcdma_in_use    = is_umts_mode();
             classmark_params.MSCR99          = mm_system_information.mscr;
             classmark_params.as_id           = (sys_modem_as_id_e_type)mm_sub_id;
             gs_read_coded_classmark_2_per_nas_stacks(
              (classmark_2_ie_T *)(location_updating_request.data + data_index +1),
              &classmark_params,(sys_modem_as_id_e_type)mm_as_id );
           }
#else
           gs_read_coded_classmark_2((classmark_2_ie_T *)(location_updating_request.data + data_index +1),
                                 mm_gsm_supported, mm_wcdma_supported, is_umts_mode(), mm_system_information.mscr );
#endif
         }

         data_index += CLASSMARK_2_TLV_SIZE;
      }

#ifdef FEATURE_LTE
      if((mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG) || (mm_timer_status[TIMER_CSMT_GUARD] == TIMER_ACTIVE))
      {
        if(data_index + 1 <= max_data_index)
        {
          location_updating_request.data[data_index] = (IEI_ADDITIONAL_UPDATE_PARAM << 4) & 0xF0;
          location_updating_request.data[data_index] = location_updating_request.data[data_index] | 0x01;
        }
        data_index += 1;
      }
      else if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
      {
        if(data_index + 1 <= max_data_index)
        {
          location_updating_request.data[data_index] = (IEI_ADDITIONAL_UPDATE_PARAM << 4) & 0xF0;
          location_updating_request.data[data_index] = location_updating_request.data[data_index] | 0x02;
        }
        data_index += 1;
      }
#ifdef FEATURE_NAS_REL10
      else if(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10)
      { 
        /* ------------------------------------------------
        ** Set Network_support_feature IE 
        ** This IE shall be included if the MS supports extended periodic timer T3212 and the Additional update parameters IE is not included
        ** ------------------------------------------------ */ 
        if(data_index + 1 <= max_data_index)
        {
          location_updating_request.data[data_index] = (byte)(IEI_UL_NW_FTR_SUPPORT) |0x01;
        }
        data_index += 1;
      }
#endif

#else

#ifdef FEATURE_NAS_REL10
      /* ------------------------------------------------
      ** Set Network_support_feature IE 
      ** ------------------------------------------------ */ 
      if(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10)
      { 
        if(data_index + 1 <= max_data_index)
        {
          location_updating_request.data[data_index] = (byte)(IEI_UL_NW_FTR_SUPPORT) |0x01;
        }
        data_index += 1;
      }
#endif
#endif

      if(data_index > max_data_index)
      {
        data_index = MIN(data_index, max_data_index);
      }
      PUT_IMH_LEN( (((((sizeof(mm_msg_head_T) - sizeof(IMH_T )) +
            sizeof( byte )) +
            (sizeof( byte ) * LAI_SIZE)) +
            sizeof( byte ))) +
            data_index,
      &location_updating_request.msg_head.IMH );
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
      if (mm_lcs_active)
      {
        /* If there is an LCS session active, send the msg as a data request */
        dword ota_msg_length;

        mm_start_timer( TIMER_T3210, DEFAULT_TIMEOUT );

        GET_IMH_LEN(ota_msg_length , &location_updating_request.msg_head.IMH);
        mm_send_ota_message((byte *)&location_updating_request, ota_msg_length);
      }
      else
      {
        mm_initiate_rr_connection( MM_RR_LOCATION_UPDATE,
                                   (IMH_T *)&location_updating_request );
      }
#else
      mm_initiate_rr_connection( MM_RR_LOCATION_UPDATE,
                                 &location_updating_request.msg_head.IMH );
#endif

      /* -------------------------------------------------------------------
      ** Any timer used for triggering the location updating procedure
      ** (e.g., T3211, T3212) is stopped if running (3GPP TS 24.008 4.4.4.1)
      ** ------------------------------------------------------------------- */
      if (mm_timer_status[TIMER_T3211] != TIMER_STOPPED)
      {
        mm_stop_timer( TIMER_T3211 );
      }

      if (mm_timer_status[TIMER_T3212] != TIMER_STOPPED)
      {
        mm_stop_timer( TIMER_T3212 );
      }

      if (mm_timer_status[TIMER_T3213] != TIMER_STOPPED)
      {
        mm_stop_timer( TIMER_T3213 );
      }

#if defined(FEATURE_LTE)
      if( emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI
#ifdef FEATURE_SGLTE
        && (MM_IS_SGLTE_ON_PS_STACK_ID)
#endif 
       )
      {
        emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);
      }
      gmm_lte_deact_isr_timer_expired = FALSE;
#endif
      mmsend_request_reject_event(TRUE,SMGMM_EVENT_LOCATION_AREA_UPDATE_REQUEST,lu_cause);
      mmsend_nas_reg_event_packet(PROC_NAS_LAU_EVENT,PROC_START,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);

//FRDIAG Move EVENT DOWN
   }
}

/*===========================================================================

FUNCTION MM_GET_LU_CAUSE

DESCRIPTION
  This function returns the Location Update cause

DEPENDENCIES
  None

RETURN VALUE
  byte

SIDE EFFECTS
  None

===========================================================================*/

byte mm_get_lu_cause( void )
{
  return lu_cause;
}

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION  mm_csfb_process_service_ind_nmo1

DESCRIPTION - This function is called when MM recevies service indicaiton for NMO1.
This function 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_csfb_process_service_ind_nmo1(mm_cmd_type *msg_p)
{
   /* For NMO1 cell, call mm_handle_message. This will initiate LAU if needed
       and sets MM states correctly for handling call/page */     
  boolean err_indication = FALSE;

  if((mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG) ||
     (mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG))
  {
    if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
    {
      err_indication = mm_is_csfb_call_allowed();
    }
    if(err_indication==FALSE)
    {
      mm_handle_message( msg_p );
    }
    else
    {
      MSG_HIGH_DS_2( MM_SUB, "=MM= CNM establish req is rejected, type %d,value %d",
                             MM_REJECT_CAUSE,MM_REJ_ACCESS_CLASS_BLOCKED) ;
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_ACCESS_CLASS_BLOCKED, MMCC_SMS_CS);
#else
      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_ACCESS_CLASS_BLOCKED);
#endif
      mm_follow_on_request = FALSE;
      mm_csfb_set_state(MM_CSFB_STATE_NULL);
    }
  }
}  

/*===========================================================================

FUNCTION MM_SET_MM_SUBSTATE_ON_IRAT_FROM_LTE

DESCRIPTION
  This function is used to set the MM idle substates on interrat from LTE to GW.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_set_mm_substate_on_irat_from_lte()
{
  mm_event_T       mm_event = MM_STD_INVALID_EVENT;
  inter_task_lai_T   curr_lai;
  mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);
#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);
#endif


  if(mm_idle_substate == MM_NO_CELL_AVAILABLE)
  {
    curr_lai.PLMN_id.identity[0] = mm_serving_plmn.info.plmn.identity[0];
    curr_lai.PLMN_id.identity[1] = mm_serving_plmn.info.plmn.identity[1];
    curr_lai.PLMN_id.identity[2] = mm_serving_plmn.info.plmn.identity[2];
    curr_lai.location_area_code[0] = mm_serving_plmn.lac.lac[0];
    curr_lai.location_area_code[1] = mm_serving_plmn.lac.lac[1];
  
    /*******************
    * Set the mm_event *
    ********************/
    if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
    {
      mm_event = MM_STD_NO_IMSI;
    }
    else if (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode
       == SYS_NETWORK_SELECTION_MODE_LIMITED)
    {
      MSG_HIGH_DS_0(MM_SUB,"=MM= PLMN changed, Limited service req ");
      mm_event = MM_STD_LIMITED_SERVICE;
    }
    else if (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode
       == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
    {
      /*
      ** If requested PLMN match with the received PLMN then NORMAL service
      ** Otherwise, make this LIMITED service.
      */
      MSG_HIGH_DS_0(MM_SUB, "=MM= PLMN changed. Manual Routine acquisition of Service\n");
      if ((sys_plmn_match(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn, mm_serving_plmn.info.plmn)) ||
           (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
            sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))
      {
         mm_event = MM_STD_NORMAL_SERVICE;
      }
      else
      {
         mm_event = MM_STD_LIMITED_SERVICE;
      }
    }
    /* ------------------------------------
    ** Check that this LAI is not forbidden
    ** ------------------------------------ */
    else if (mm_is_lai_forbidden(curr_lai) ||
#ifdef FEATURE_DUAL_SIM
            (reg_sim_per_subs_plmn_forbidden(mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id))
#else
            (reg_sim_plmn_forbidden(mm_serving_plmn.info.plmn))
#endif
            )
    {
      MSG_HIGH_DS_0(MM_SUB, "=MM= PLMN or LAI is in a forbidden list");
      mm_event = MM_STD_LIMITED_SERVICE;
    }
    else if (mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE)
    {
      mm_event = MM_STD_NORMAL_SERVICE;
    }
    else
    {
      if ((mm_location_information.location_update_status == UPDATED) &&
          (memcmp(mm_location_information.lai,
                  mm_serving_lai,
                  LAI_SIZE) == 0))
      {
         if (((mm_possible_imsi_attach) && (mm_system_information.ATT)) ||
              (gmm_lte_deact_isr_timer_expired == TRUE )||
              (mm_timer_status[TIMER_T3423] == TIMER_ACTIVE &&
               emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) ||
              (mm_pending_lu_request == LTE_IRAT_LU))
             
         {
           mm_event = MM_STD_UPDATE_PENDING_IRAT_FROM_LTE;
         }
         else
         {
           mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           SYS_SRV_DOMAIN_CS_ONLY,
                                           FALSE,
                                           SYS_SRV_STATUS_LIMITED,
                                           TRUE );
           mm_event = MM_STD_NORMAL_SERVICE;
         }
      }
      else
      {
         mm_event = MM_STD_UPDATE_PENDING_IRAT_FROM_LTE;
      }
    }
    
    /*********************************************
    * Set the mm_serving_plmn global data values *
    **********************************************/
    mm_serving_plmn.mm_service_state = SERVICE_AVAILABLE;
    mm_substate_control( mm_event );
  }
}
#endif

