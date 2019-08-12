/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmplmnsel.c_v   1.25   17 Jul 2002 12:30:06   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmplmnsel.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------              
              
07/13/11   rajesh  Handling expiry of T3412 and T3423 when ISR is ON
05/03/01   jca     Added new functions to support MM/MMCOORD/GMM integration.
05/14/01   jca     Added service_type parameter to mmr_send_nreg_ind().
05/23/01   jca     Updated to reflect changes to REG<->MM interface.
06/03/01   sbk     Global variables are renamed to use mm_ prefix
06/03/01   sbk     The selected plmn/lai processing is changed to use mmcoord data
06/20/01   sbk     mm_load_sim_data moved to mmsim.c
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
10/19/01   sbk     T3212 is processed at the end of its associated cmd processing
11/06/01   jca     Do not set substate to PLMN_SEARCH when already camped on
                   selected PLMN.
06/05/02   ks      Added support for Dual mode operation.
06/18/02   ks      setting the PLMN service_state to correct value in
                   function mm_handle_plmn_found()
06/20/02   jca     Setting mm_serving_plmn.info.plmn_forbidden to FALSE if
                   REG_REQ rejected because PLMN on national forbidden list.
06/21/02   ks      Setting variables select_specific_PLMN and is_gsm fields in
                   RR_PLMN_SELECT_REQ cmd and dual_mode in RRC_SERVICE_REQ.
06/26/02   ks      Storing the System information received in RR_PLMN_SELECT_CNF primitive.
07/01/02   jca     Process RRC_SERVICE_CNF based on service_status rather than PLMN match.
07/03/02   jca     Deleted function mm_plmn_list_control().
07/12/02   ks      Setting the default values for AVAILABLE/EQUIVALENT list info
                   in the MMR_REG_CNF primitive.
07/17/02   ks      Removed unused variable rr_plmn_list from mm_select_plmn()
07/22/02   ks      Setting the mm_serving_plmn.info.active_rat variable in the
                   mm_send_service_req, at the time of Activating RR or RRC.
08/02/02   ks      Removed present_all_plmns reference to mmr_reg_req message
                   Setting the proper MM state when NO_SERVICE is received
08/07/02   ks      Setting proper default values for MM_REG_CNF primitive.
08/10/02   mks     Removed references to TMSI, PTMSI and access class from
                   RRC_SERVICE_REQ in mm_send_service_req()
08/15/02   ks      Setting the AUTOMATIC/MANUAL PLMN search info in
                   rr_plmn_select_req based on received network_selection_mode
08/21/02   ks      Setting the mm_serving_plmn service state properly in the
                   mm_handle_plmn_found function.
09/10/02   ks      Add checking for network selection mode for
                   SYS_NETWORK_SELECTION_MODE_LIMITED in mm_send_service_req.
09/17/02   ks      Now initiating a LU procedure after receiving a
                   PLMN_SELECT_CNF if there is a pending T3212 expiry LU.
09/18/02   cd      In mm_send_service_req(), added setting the automatic flag in
                   rr_plmn_select_req to TRUE when the REG request received is set to
                   SYS_NETWORK_SELECTION_MODE_LIMITED. Also,  In UMTS mode,  set the
                   requested plmn type according to what was sent in the REG request.
                   In mm_select_plmn() added check for mm_sim_valid so that if the
                   SIM is not inserted, there is no transition to substate
                   MM_PLMN_SEARCH after a REG request is received.
                   Also,  after an rr_plmn_select_cnf/rrc_service_cnf is received,
                   deferred transitioning substates until checking to see if SIM is
                   inserted. If the SIM has not been inserted added code to send a
                   mmr_reg_cnf
09/24/02   cd      Removed tabs inserted in last check in
10/03/02   mks     Location update attempt counter is not reset if the serving PLMN
                   matches the REG requested PLMN
10/07/02   jca     Changes to support CS/PS combined attach procedures.
10/08/02   ks      Setting the service_domain field in rr_plmn_select_req.
10/11/02   cd      Corrected setting the mm_event in mm_select_plmn() when SIM is not
                   inserted
11/06/02   jca     Modified code to set MM PLMN info status through calls to
                   new function mm_update_mm_serving_plmn_info().
11/08/02   cd      Added support for AC blocking
11/15/02   cd      In mm_select_plmn() removed redundant call to store system
                   information
11/15/02   mks     Merged a fix so as to not reset the LU attempt counter on
                   receiving MMR_REG_REQ.
11/25/02   cd      In mmumts_process_cell_change() removed storing GMM current
                   LAI and RAC.
11/26/02   jca     Fixed some bugs in Access Class blocking processing.
                   Now sending immediate MMR_REG_CNF when camped on forbidden LAI.
12/05/02   jca     Changes to support PS_ONLY<->CS_ONLY mode switches in GSM mode.
12/12/02   ks      Replaced mm_sim_valid with function mmsim_sim_valid.
01/10/03   jca     Changes to support CS<->PS<->CS/PS mode switching.
01/13/03   jca     Corrected parameters in call to function mm_update_mm_serving_plmn_info().
01/15/03   mks     Resolved an invalid state transition upon receiving an
                   RR_PLMN_SELECT_CNF message with MM_AS_NO_SERVICE (when SIM
                   is invalidated for CS domain or SIM is not availble).
01/27/03   jca     Changes to support PS_ONLY<->CS/PS mode switching.
                   Changes to support IMSI Detach via Combined GPRS Detach procedure.
02/07/03   jca     Added mm_dual_mode flag.
02/10/03   cd      Fixed Lint errors
02/13/03   jca     Added mm_waiting_for_service_cnf boolean flag.
04/08/03   jca     Added new reason code for pending LU request.
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
06/16/03   jca     Make sure to generate MMR_SERVICE_IND when RR_PLMN_SELECT_CNF
                   received if REG is not already waiting for a MMR_REG_CNF.
07/16/03   jca     Added code to take GMM out of SUSPENDED substate when
                   RR_PLMN_SELECT_REQ sent to RR layer.
07/16/03   mks     Added support for PS service continuity due to loss of service
                   during Inter RAT changes.
07/17/03   cd      Added checking for ATT flag, location_update_status and PLMN match
                   when processing RR_PLMN_SELECT_CNF
07/18/03   jca     Now immediately send MMR_REG_CNF when CS registration status
                   is available instead of waiting for PS registration attempt
                   to complete (NMO II mode of operation).
07/18/03   cd      Added checking for ATT flag, location_update_status and PLMN match
                   when processing RRC_SERVICE_CNF
07/21/03   cd      Corrected processing of RRC_SERVICE_CNF/RR_PLMN_SELECT_CNF
                   when the SIM is not valid and there are access class restrictions
07/29/03   jca     Fixed PS Attach-on-demand problem related to clearing of the
                   mm_serv_change_pending boolean flag.
08/29/03   cd      Added support for passing the band_preference between REG and RR.
09/18/03   jca     Now resetting mm_pending_lu_request within mm_initiate_lu()
09/25/03   ks      If REG requests a NORMAL service when MM is in Limited service
                   return already_camped to FALSE to come of this sub-state.
10/03/03   jca     Added support for network selection mode
                   SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION.
10/02/03   mks     Now mm_serving_plmn.info.plmn_forbidden is set to TRUE only when
                   the serving PLMN is forbidden.
10/17/03   jca     Do not overwrite mm_serving_plmn_info.reg_service_domain when
                   camping on a forbidden PLMN/LAI, only the service_status.
                   Replaced select_specific_PLMN and automatic fields in
                   RR_PLMN_SELECT_REQ with network_select_mode field.
11/24/03   jca     Allow LU attempt when camped on a Forbidden PLMN/LAI and
                   the network selection mode is set to MODE_MANUAL as per
                   GCF test case 44.2.1.1.4.3.2.
12/12/03   ks      Passing the network_selection_mode in the RRC_SERVICE_REQ
                   Removed storign gsm system info, now done in mmcoord.c
01/28/04   jca     Restore MM substate to MM_ATTEMPTING_TO_UPDATE when MMR_REG_REQ
                   processing is completed and MS camps on the same cell where it
                   had just previously camped (i.e., MMR_REG_REQ to the same PLMN).
03/24/04   jca     Check for a forbidden PLMN/LAI in function mm_select_plmn()
                   so that the T3212 timer is not reset in error.
03/25/04   jca     Now calling mm_rat_handle_srv_status_update() from GMM state machine
                   context to help ensure that ps_data_suspend flag is cleared correctly
                   and PDP activation requests succeed when expected.
04/19/04   jca     Send immediate MMR_REG_CNG instructing REG to remain on the current cell
                   when cell access status set to ACCESS_NO_CALLS (transient condition).
05/12/04   cd      Added support for WCDMA multiband feature
05/19/04   cd      Now sending the band_pref in a RRC_SERVICE_REQ/RR_PLMN_SELECT_REQ
                   if either GSM or WCDMA multiband feature is defined
06/07/04   jca     Ensure mm_serving_plmn.info.service_status is set to correct value
                   on receipt of RRC_SERVICE_CNF/RR_PLMN_SELECT_CNF.
07/30/04   jca     Added regional variable mm_band_pref.
10/06/04   ks      Added function mm_send_mmr_camped_ind() to send the camped_ind
                   to REG after acquiring service on requested PLMN.
01/20/05   jca     Send an immediate REG_CNF to MMR_REG_REQ received at powerup
                   when acquired PLMN matches LOCI information on the SIM card
                   but ATT=1 system info indicates Power-up LU required.
02/24/05   sm      Now, MM is setinto Normal service for
                   SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION
                   network selection mode from Limited service
05/05/05   jca     On receipt of RRC_SERVICE_CNF/RR_PLMN_SELECT_CNF and access class
                   restrictions are in effect, only set MM substate to UPDATE_PENDING
                   when the mm_possible_imsi_attach flag is set to TRUE.
06/20/05   abe     Reset the attempt counter on a new PLMN selection if the
                   MM substate was set to ATTEMPTING_TO_UPDATE
06/20/05   up      Send LIMITED when N/W sel mode is LIMITED_MANUAL_ACQ.
                   Also set service to FULL if the currently served PLMN is
                   same as the requested PLMN, otherwise LIMITED.
08/18/05   jca     Ensure MMR_REG_CNF is first response sent to REG task after a
                   MMR_REG_REQ is received (instead of a MMR_SERVICE_IND).
09/16/05   ajt     Send MMR_EMERGENCY_NUM_LIST_IND to REG on receiving RRC_SERVICE_CNF/
                   RR_SELECT_PLMN_CNF from a PLMN with different country code than
                   the country code of currently camped on PLMN.                   
06/14/06   up      Called new mm_update_mm_serving_plmn_info for GSTK event.
07/14/06   pk     Updated for Feature DSAC
07/27/06   sn      Added Uniform OOS changes.
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING
08/21/06   up      Initate LU when CS FOR bit is set due to Call.
09/25/06   sn      Handling of MMR_CAUSE_FAILURE_MAX_ATTEMPTED
10/05/06   sn      Added handling for WCDMA_1X mode.
08/25/11   gps     Truncating number of plmns to sys_plmn_list_max_length if 
                   plmn list length is long in rrc_service_cnf and rr_plmn_select_cnf.
10/05/11   gps     Initiated rr plmn select req primitive with all zeros
10/12/11   sla     Reset the variable to mm_is_foreground_search_to_be_initiated 
                     to FALSE when 5th LU is attempted because of managed roaming
11/08/11   sla     Update the mm_previous_serving_lai with mm location information
                   lai
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                     as a part of CMI activity
==========================================================================*/


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
#include "mm_v.h"
#include "ms.h"
#include "mm_rr.h"
#include "reg_mm_v.h"
#include "rrcmmif.h"
#include "rrccmd.h"
#include "mm_umts_v.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm_coord.h"
#include "sys.h"
#include "sys_v.h"
#include "mm_sim.h"
#include "err.h"
#include "gmm_int_v.h"
#include "mmrat.h"
#include "reg_sim_v.h"
#include "sys_eplmn_list_v.h"
#include "reg_nv.h"
#include "mm_gstk.h"
#include "reg_mode.h"
#ifdef FEATURE_LTE
#include "emm_utility.h"
#endif
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE)
#include "emm_rrc_if.h"
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_validate(a) sys_eplmn_per_nas_stacks_list_validate(a,(sys_modem_as_id_e_type)mm_as_id)
#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a,(sys_modem_as_id_e_type)mm_sub_id,\
                                                                                             (sys_modem_as_id_e_type)mm_as_id)
#endif

#if defined(FEATURE_LTE)
#ifdef FEATURE_DUAL_SIM
extern boolean gmm_lte_deact_isr_timer_expired_sim[MAX_AS_IDS];
#define gmm_lte_deact_isr_timer_expired gmm_lte_deact_isr_timer_expired_sim[mm_sub_id]
#else
extern boolean gmm_lte_deact_isr_timer_expired;
#endif
#endif

#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK

extern boolean full_srv_req_in_emerg;

#ifdef FEATURE_LTE
extern boolean emm_bit_mask_check
(
  uint32 value,
  byte bit_mask
);
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*===========================================================================

FUNCTION MM_SEND_SERVICE_REQ

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_service_req( mm_cmd_type *message , sys_radio_access_tech_e_type rat )
{
   rr_plmn_select_req_T  rr_plmn_select_req;
#ifdef FEATURE_WCDMA
   rrc_cmd_type          *rrc_cmd;
#endif
   byte                  i;
   memset(&rr_plmn_select_req,0x00,sizeof(rr_plmn_select_req_T));

#if defined (FEATURE_LTE) && defined (FEATURE_DUAL_DATA)
   /* Send this indication to L-RRC to check whether LTE is active on Non-DDS SUB */
   if(mm_is_multimode_sub() == FALSE)
   {
     emm_send_rrc_nas_domain_pref_ind();
   }
#endif
   
#ifdef FEATURE_WCDMA
   if ((rat == SYS_RAT_UMTS_RADIO_ACCESS) || (rat == SYS_RAT_TDS_RADIO_ACCESS))
   {
      /*
       * Send the RRC_SERVICE_REQ to RRC when talking to RRC
       */
#ifdef FEATURE_WCDMA      
       rrc_cmd = mm_get_rrc_cmd_buf( rat );
#endif /* FEATURE_WCDMA */

      if (rrc_cmd == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
      }

      rrc_cmd->cmd_hdr.cmd_id = RRC_SERVICE_REQ;

      rrc_cmd->cmd.rrc_service_req.network_select_mode =
        message->cmd.mmr_reg_req.network_selection_mode;
      /*
      Remember last send network selection mode. 
      Update RRC when there is a transition in the last send network selection mode
      using nw_sel_mode_reset_ind
      */
      last_requested_network_selection_mode  =
        message->cmd.mmr_reg_req.network_selection_mode;

#ifdef FEATURE_LTE
     /* While sending the service request, update the variable so that, we can check later if voice domain pref gets changed with another service request*/
      gmm_reg_pended_lte_cs_param = mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param;
#endif

#ifndef FEATURE_RAT_PRIORITY_LIST
#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
      rrc_cmd->cmd.rrc_service_req.band_pref = get_preferred_band_class();
#endif
#endif /* FEATURE_RAT_PRIORITY_LIST */
      /* Set the ACTIVE RAT to UMTS at the time of Activating UMTS mode */
      if (rat == SYS_RAT_UMTS_RADIO_ACCESS)
      {
         mm_serving_plmn.info.active_rat = SYS_RAT_UMTS_RADIO_ACCESS;
      }
      else if (rat == SYS_RAT_TDS_RADIO_ACCESS)
      {
         mm_serving_plmn.info.active_rat = SYS_RAT_TDS_RADIO_ACCESS;
      }

      fill_rrc_forbidden_lai_list( &rrc_cmd->cmd.rrc_service_req.forbid_lai_list );

      /*
       * Set the requested plmn type based on the plmn received in the REG request
       */
      if ((message->cmd.mmr_reg_req.network_selection_mode ==
             SYS_NETWORK_SELECTION_MODE_LIMITED) ||
          (message->cmd.mmr_reg_req.network_selection_mode ==
             SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) )
      {
         rrc_cmd->cmd.rrc_service_req.requested_plmn.plmn_type =
            RRC_GSM_MAP_ANY_PLMN_REQ;
      }
      else
      {
         rrc_cmd->cmd.rrc_service_req.requested_plmn.plmn_type =
            RRC_GSM_MAP_SPECIFIC_PLMN_REQ;
      }

      /*Convert the n/w selection mode approp, AS can handle*/
      if (rrc_cmd->cmd.rrc_service_req.network_select_mode ==
            SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
      {
        rrc_cmd->cmd.rrc_service_req.network_select_mode = SYS_NETWORK_SELECTION_MODE_LIMITED;
        rrc_cmd->cmd.rrc_service_req.requested_plmn.plmn_type = RRC_GSM_MAP_ANY_PLMN_REQ;
      }
      else if (rrc_cmd->cmd.rrc_service_req.network_select_mode ==
                       SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY)
      {
        rrc_cmd->cmd.rrc_service_req.network_select_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
      }
      else if(rrc_cmd->cmd.rrc_service_req.network_select_mode ==
                SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)
      {
        rrc_cmd->cmd.rrc_service_req.network_select_mode = SYS_NETWORK_SELECTION_MODE_MANUAL;
      }
      
      rrc_cmd->cmd.rrc_service_req.requested_plmn.plmn_id =
         mmumts_convert_nas_plmn_id_to_rrc_plmn_id( message->cmd.mmr_reg_req.plmn );

#ifdef FEATURE_FEMTO_CSG
      rrc_cmd->cmd.rrc_service_req.requested_plmn.csg_id = message->cmd.mmr_reg_req.csg_id;
#endif
      /* Set the operating RAT of the UE */
#ifndef FEATURE_RAT_PRIORITY_LIST
      rrc_cmd->cmd.rrc_service_req.dual_mode = message->cmd.mmr_reg_req.dual_mode;
      mm_dual_mode                           = message->cmd.mmr_reg_req.dual_mode;
      mm_band_pref                           = message->cmd.mmr_reg_req.band_pref;
#else
      /* The following two lines have to be removed once code changes for
       * rat priority list in RRC layer is completed */
      rrc_cmd->cmd.rrc_service_req.dual_mode = message->cmd.mmr_reg_req.dual_mode;
#ifdef FEATURE_GSM_BAND_PREF
      rrc_cmd->cmd.rrc_service_req.band_pref = message->cmd.mmr_reg_req.band_pref;
#endif /*FEATURE_GSM_BAND_PREF*/
      memscpy(&rrc_cmd->cmd.rrc_service_req.rat_pri_list_info, sizeof(sys_rat_pri_list_info_s_type) ,
             &message->cmd.mmr_reg_req.rat_pri_list_info,
             sizeof(sys_rat_pri_list_info_s_type)); 

      memscpy(&mm_rat_pri_list_info,sizeof(sys_rat_pri_list_info_s_type),&message->cmd.mmr_reg_req.rat_pri_list_info,
         sizeof(sys_rat_pri_list_info_s_type));

#endif /* FEATURE_RAT_PRIORITY_LIST */

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
      #error code not present
#endif
      rrc_cmd->cmd.rrc_service_req.grat_scan_done = message->cmd.mmr_reg_req.grat_scan_done;

      rrc_cmd->cmd.rrc_service_req.scan_scope  = message->cmd.mmr_reg_req.scan_scope;             
      
      rrc_cmd->cmd.rrc_service_req.trm_timeout_value = message->cmd.mmr_reg_req.trm_timeout;

      if ((rrc_cmd->cmd.rrc_service_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
          (!sys_plmn_id_is_undefined(message->cmd.mmr_reg_req.plmn)))
      {
        rrc_cmd->cmd.rrc_service_req.is_ehplmn_to_be_camped = reg_mode_get_hplmn_to_selected(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                                              (sys_modem_as_id_e_type)mm_as_id
#endif
                                                              );
      }

     /* latest rrcmmif.h file is not getting picked up by the mob thereby breaking the genesis 
      * mob. Should remove this feature flag once latest rrcmmif.h file is picked up 
     */
      if(((mm_follow_on_request)
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE)
         || (mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG) 
         || (mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)
#endif
         )
#ifdef FEATURE_LTE
         && ((emm_bit_mask_check(message->cmd.mmr_reg_req.additional_info,
                                (byte)SYS_ADDTNL_INFO_EMERG_CALL) == FALSE)
              || (full_srv_req_in_emerg == FALSE)
            )
#endif
        )
      {
        rrc_cmd->cmd.rrc_service_req.service_req_cause = RRC_MO_CS_CALL;
      }
#ifdef FEATURE_LTE
      else if ((emm_bit_mask_check(message->cmd.mmr_reg_req.additional_info,
                                (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE) 
                && (full_srv_req_in_emerg == TRUE)
               )
      {
        rrc_cmd->cmd.rrc_service_req.service_req_cause = RRC_MO_CS_ECALL;
        MSG_HIGH_DS_0( MM_SUB, "=MM= rrc_service_req with service request cause RRC_MO_CS_ECALL");
      }
#endif
      else if(gmm_follow_on_request)
      {
        rrc_cmd->cmd.rrc_service_req.service_req_cause = RRC_MO_PS_CALL;
      }
      else
      {
        rrc_cmd->cmd.rrc_service_req.service_req_cause = RRC_NORMAL_ACQ;
      }
      MSG_HIGH_DS_2( MM_SUB, "=MM= rrc_service_req with service request cause %d scan scope %d", rrc_cmd->cmd.rrc_service_req.service_req_cause,message->cmd.mmr_reg_req.scan_scope);

      /* Put the command in the RRC queue */
      send_cmd_to_rrc( rrc_cmd, rat );

      mm_waiting_for_service_cnf = TRUE;
      mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
      mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
      
      /* Reset cell this as cell service indication will be sent on getting cnf message*/
      mm_mmr_cell_service_ind_pending = FALSE;
   }
   else
#endif
   if (rat == SYS_RAT_GSM_RADIO_ACCESS)
   {
      /*
       * Form RR_PLMN_SELECT_REQ with PLMN from input
       * message
       */

      rr_plmn_select_req.message_header.message_set = MS_MM_RR;
      rr_plmn_select_req.message_header.message_id  = (int) RR_PLMN_SELECT_REQ;
#ifndef FEATURE_RAT_PRIORITY_LIST
#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
      rr_plmn_select_req.band_pref = get_preferred_band_class();
#endif
#endif /* FEATURE_RAT_PRIORITY_LIST */

      /* Set the ACTIVE RAT to GSM at the time of Activating UMTS mode */
      mm_serving_plmn.info.active_rat = SYS_RAT_GSM_RADIO_ACCESS;

#ifndef FEATURE_RAT_PRIORITY_LIST
      /* Set the operating RAT as GSM if the dual_mode is TRUE in mmr_reg_req */
      rr_plmn_select_req.gsm_only = !message->cmd.mmr_reg_req.dual_mode;
      mm_dual_mode                = message->cmd.mmr_reg_req.dual_mode;
      mm_band_pref                = message->cmd.mmr_reg_req.band_pref;
#else
      /* The following two lines have to be removed once code changes for
       * rat priority list in RR layer is completed */
      rr_plmn_select_req.gsm_only = !message->cmd.mmr_reg_req.dual_mode;
#ifdef FEATURE_GSM_BAND_PREF
      rr_plmn_select_req.band_pref = message->cmd.mmr_reg_req.band_pref;
#endif /*FEATURE_GSM_BAND_PREF*/

      memscpy(&rr_plmn_select_req.rat_pri_list_info, sizeof(sys_rat_pri_list_info_s_type),
             &message->cmd.mmr_reg_req.rat_pri_list_info,sizeof(sys_rat_pri_list_info_s_type));
      memscpy(&mm_rat_pri_list_info,sizeof(sys_rat_pri_list_info_s_type), 
             &message->cmd.mmr_reg_req.rat_pri_list_info,sizeof(sys_rat_pri_list_info_s_type));

#endif /* FEATURE_RAT_PRIORITY_LIST */

      /* Set the Requested Service domain CS, PS or CS_PS */
      rr_plmn_select_req.service_domain = mm_serving_plmn.ms_op_mode;
      
      if ((rr_plmn_select_req.service_domain == SYS_SRV_DOMAIN_CS_PS) &&
          ((mm_serving_plmn.info.sim_state == SYS_SIM_STATE_CS_PS_INVALID) ||
           (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_PS_INVALID)))
      {
        rr_plmn_select_req.service_domain = SYS_SRV_DOMAIN_CS_ONLY;
      }


      if (message->cmd.mmr_reg_req.network_selection_mode ==
                       SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY)
      {
        rr_plmn_select_req.network_select_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
      }
      else if(message->cmd.mmr_reg_req.network_selection_mode ==
                SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)
      {
        rr_plmn_select_req.network_select_mode = SYS_NETWORK_SELECTION_MODE_MANUAL;
      }
      else
      {
        rr_plmn_select_req.network_select_mode = message->cmd.mmr_reg_req.network_selection_mode;
      }

      /*
      Remember last send network selection mode.
      Update RR when there is a transition in the last send network selection mode
      using nw_sel_mode_reset_ind
      */
      last_requested_network_selection_mode  = message->cmd.mmr_reg_req.network_selection_mode;

#ifdef FEATURE_LTE
      /* While sending the service request, update the variable so that, we can check later if voice domain pref gets changed with another service request*/
      gmm_reg_pended_lte_cs_param = mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param;
#endif
      if (rr_plmn_select_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
      {
        rr_plmn_select_req.network_select_mode = SYS_NETWORK_SELECTION_MODE_LIMITED;
        memset(rr_plmn_select_req.PLMN_id.identity, 0xFF, 3);
      }
      else
      {
         /* --------------------------------------------------------------------
         ** Set the PLMN received in the mmr_reg_req cmd.
         ** If PLMN is not valid, RR ignores it and performs Any PLMN selection
         ** ------------------------------------------------------------------*/
         for (i=0; i < PLMN_SIZE; i++)
         {
            rr_plmn_select_req.PLMN_id.identity[i] = message->cmd.mmr_reg_req.plmn.identity[i];
         }
      }

      if ((rr_plmn_select_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
          (!sys_plmn_id_is_undefined(message->cmd.mmr_reg_req.plmn)))
      {
        rr_plmn_select_req.is_ehplmn_to_be_camped = reg_mode_get_hplmn_to_selected(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                                           (sys_modem_as_id_e_type)mm_as_id
#endif
                                                          );

      }

#ifdef FEATURE_DUAL_SIM
      if((mm_follow_on_request)
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE)
          || (mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG) 
          || (mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)
#endif 
           )
      {
        rr_plmn_select_req.service_req_cause = RR_MM_MO_CS_CALL;
      }
      else if(gmm_follow_on_request)
      {
        rr_plmn_select_req.service_req_cause = RR_MM_MO_PS_CALL;
      }
	  else 
#endif
	  if( message->cmd.mmr_reg_req.scan_scope == SYS_SCAN_SCOPE_ACQ_DB)   
      {
        rr_plmn_select_req.service_req_cause = RR_MM_ACQ_DB_SCAN;
      }
      else
      {
        rr_plmn_select_req.service_req_cause = RR_MM_NORMAL_ACQ;
      }

      PUT_IMH_LEN( sizeof( rr_plmn_select_req_T ) - sizeof(IMH_T),
                           &rr_plmn_select_req.message_header );
      MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent RR_PLMN_SELECT_REQ with service request cause %d", rr_plmn_select_req.service_req_cause);

#ifdef FEATURE_SGLTE
     if( MM_IS_IN_SGLTE_MODE && 
         (mm_cs_stack_id != mm_ps_stack_id) &&
         (mm_as_id == mm_ps_stack_id))
     {            
       MSG_FATAL_DS(MM_SUB,"=MM= GSM on PS stack in dual stack mode -- ERROR", 0, 0, 0);
     }
#endif

      mm_send_message( (IMH_T *)&rr_plmn_select_req, GS_QUEUE_RR );

      mm_waiting_for_service_cnf = TRUE;
      mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
      mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;

      /* Reset as cell service indication will be sent on getting cnf message*/
      mm_mmr_cell_service_ind_pending = FALSE;

      if ((gmm_substate == GMM_SUSPENDED)
#ifdef FEATURE_WRLF_SYSTEM_SEL
          &&(mm_wcdma_rlf_state == WCDMA_RLF_IDLE)
#endif
         )
      {
        gmm_set_substate( GMM_PLMN_SEARCH );
      }
   }/* if gsm mode */

   else
   {
      MSG_FATAL_DS(MM_SUB, "=MM= RAT Not Supported:",0,0,0);
   }
} /* end mm_send_service_req() */


/*===========================================================================

FUNCTION MM_ALREADY_CAMPED

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_already_camped(const mm_cmd_type *message)
{
   boolean already_camped;

   /*
    * Check if we are already camped on the selected PLMN
    */

   already_camped = FALSE;

   if (((mm_serving_plmn.mm_service_state  == SERVICE_AVAILABLE) ||
        (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE)
       ) &&
       (PLMN_MATCH(mm_serving_lai, message->cmd.mmr_reg_req.plmn.identity)
        ||
       ( (message->cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)&&
         (mm_plmn_match(mm_serving_plmn.info.plmn, message->cmd.mmr_reg_req.plmn ))
        ) 
       ) &&
       ((mm_idle_substate != MM_PLMN_SEARCH ) && (mm_idle_substate != MM_LIMITED_SERVICE)) 
#ifdef FEATURE_LTE
        && (emm_lte_cs_domain_param_match(
              message->cmd.mmr_reg_req.lte_cs_domain_param,
               gmm_reg_pended_lte_cs_param) == TRUE)
#endif
#ifdef FEATURE_FEMTO_CSG
        && ((mm_serving_plmn.info.csg_info.csg_id == message->cmd.mmr_reg_req.csg_id) ||
                ((message->cmd.mmr_reg_req.csg_id == SYS_CSG_ID_INVALID) && 
                 mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info)))
#endif 
         )
   {

      /*
       * check that we are not camped on a forbidden LAI at the
       * moment
       */
      if (( mm_check_forbidden_national_roaming(
               (inter_task_lai_T *)mm_serving_lai ) == FALSE ))
      {
         /*
          * we are allowed to stay in an LAI even though it is
          * forbidden for regional provision of service, so only
          * areas forbidden for national roaming are checked
          */

         already_camped = TRUE;
      }
   }

   /* while in CAMP_ONLY mode, CM sends the serv req on same plmn,
     send appropriate plmn_reg_type in the CNF */
   if((already_camped == TRUE) &&
      (mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
      ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                                SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                                  SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)))
   {
     if ((!mm_check_forbidden_national_roaming((inter_task_lai_T *)mm_serving_lai ))  &&
         (!mm_check_forbidden_regional_roaming((inter_task_lai_T *)mm_serving_lai ))  &&
#ifdef FEATURE_DUAL_SIM
         (!reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id ))     &&
         (!reg_sim_per_subs_plmn_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) &&
#else
         (!reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn )) &&
         (!reg_sim_plmn_forbidden( mm_serving_plmn.info.plmn ))      &&
#endif
         (mm_timer_status[TIMER_T3302] != TIMER_ACTIVE))
     {
       mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_PS_SERVICE_ONLY;
     }
     else
     {
       mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CAMPED_ONLY;
     }
   }

   return already_camped;

} /* end mm_already_camped() */


/*===========================================================================

FUNCTION MMUMTS_PROCESS_CELL_CHANGE

DESCRIPTION
  This function handles the change in cell info due to no service, plmn change,
  LAI change or system information change. Sets the mm_serving_lai and
  service_state variables

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmumts_process_cell_change( mm_as_service_state_T service_status,
                                 rrc_service_info_type service_info,service_ind_message_type service_ind_type)
{
   dword i;
   byte lai[LAI_SIZE] = { 0xff, 0xff, 0xff, 0, 0 };

   if (service_status == MM_AS_SERVICE_AVAILABLE)
   {
      lai[0] = mm_serving_plmn.info.plmn.identity[0];
      lai[1] = mm_serving_plmn.info.plmn.identity[1];
      lai[2] = mm_serving_plmn.info.plmn.identity[2];
      lai[3] = mm_serving_plmn.lac.lac[0];
      lai[4] = mm_serving_plmn.lac.lac[1];

      if (mm_serving_plmn.info.plmn_service_capability == SYS_SRV_CAPABILITY_PS_SERVICE_ONLY)
      {
        mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
      }
      else
      {
        mm_serving_plmn.mm_service_state = SERVICE_AVAILABLE;
      }

      if( memcmp(lai, mm_serving_lai, LAI_SIZE) != 0) /* New LAI */
      {
          memscpy(mm_serving_lai, LAI_SIZE, lai, LAI_SIZE);

          MSG_HIGH_DS_3(MM_SUB, "=MM= Serving LAI PLMN %x %x %x",
                        mm_serving_lai[0], mm_serving_lai[1], mm_serving_lai[2]);

          MSG_HIGH_DS_2(MM_SUB, "=MM= Serving LAC %x %x", mm_serving_lai[3], mm_serving_lai[4]);
      }

      /* Set the MSCR to 1 in case of UMTS operation */
      mm_system_information.mscr = 1;

      /* Store the cell_access information */
#ifndef FEATURE_DSAC      
      mm_system_information.cell_access = service_info.cell_access;
#endif

      if(service_ind_type == NORMAL_SERVICE_MESSAGE)
      {
        for (i=0; i < service_info.num_cn_domains; i++)
        {
           if (service_info.cn_domain_info[i].cn_domain_id == RRC_CS_DOMAIN_CN_ID)
           {
              if (service_info.cn_domain_info[i].cn_domain_specific_info.gsm_map_info.value[1]
                    & 0x01)
              {
                 mm_system_information.ATT = TRUE;
              }
              else
              {
                 mm_system_information.ATT = FALSE;
              }
           }
        }
      }
   }
   else if (service_status == MM_AS_NO_SERVICE)
   {
      (void)memscpy( mm_serving_lai, LAI_SIZE, lai, LAI_SIZE);
      mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;

      MSG_MED_DS(MM_SUB, "=MM= No serving LAI", 0,0,0);
   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= MM: Unknown rrc service status", 0,0,0);
   }
}


/*===========================================================================

FUNCTION MM_SELECT_PLMN

DESCRIPTION
  This function handles PLMN selection messages, either the request
  from MM for a particular PLMN/LAI, or report from RR as to the
  success or otherwise of the attempt to camp on the requested
  PLMN/LAI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_select_plmn(mm_cmd_type* msg_p)
{
   dword                                  entry;
   dword                                  num_plmns;
   rrc_avail_plmn_type*                   rrc_plmn_list;
   sys_detailed_plmn_list_info_s_type*    reg_plmn_list;
   mm_event_T                             mm_event = MM_STD_INVALID_EVENT;
   
   switch (msg_p->cmd.hdr.message_set)
   {
      case MS_MM_REG:
         if (msg_p->cmd.hdr.message_id == (int) MMR_REG_REQ)
         {

            if (!mm_already_camped( msg_p ))
            {
               mm_substate_control( MM_STD_PLMN_SEARCH );
            }
         }
         else
         {
            MSG_ERROR_DS(MM_SUB, "=MM= Unexpected REG message id %d", msg_p->cmd.hdr.message_id, 0,0);
         }
         break;

      case MS_MM_RR:
      case MS_MM_TDSRRC:
         if (msg_p->cmd.hdr.message_id == (int) RRC_SERVICE_CNF)
         {
             sys_plmn_id_s_type nas_type_plmn;
             inter_task_lai_T       curr_lai;

             /************************************************
             * Store the System information received from RRC *
             *************************************************/
             mmumts_process_cell_change(msg_p->cmd.rrc_service_cnf.service_status,
                                       msg_p->cmd.rrc_service_cnf.service_info,NORMAL_SERVICE_MESSAGE);

             curr_lai.PLMN_id.identity[0] = mm_serving_plmn.info.plmn.identity[0];
             curr_lai.PLMN_id.identity[1] = mm_serving_plmn.info.plmn.identity[1];
             curr_lai.PLMN_id.identity[2] = mm_serving_plmn.info.plmn.identity[2];
             curr_lai.location_area_code[0] = mm_serving_plmn.lac.lac[0];
             curr_lai.location_area_code[1] = mm_serving_plmn.lac.lac[1];

             num_plmns = MIN(msg_p->cmd.rrc_service_cnf.avail_plmn_list.num_plmns,SYS_PLMN_LIST_MAX_LENGTH);
            /* -------------------------------------------------------------
            ** If the PLMN was not found then send the available list to REG
            ** in the MMR_REG_CNF
            ** ------------------------------------------------------------- */
            if (msg_p->cmd.rrc_service_cnf.service_status == MM_AS_NO_SERVICE)
            {
               /*******************
               * Set the mm_event *
               ********************/
               mm_event = MM_STD_NO_SERVICE;

               /*********************************************
               * Set the mm_serving_plmn global data values *
               **********************************************/
               mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;

               mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               mm_serving_plmn.info.reg_service_domain,
                                               TRUE,
                                               SYS_SRV_STATUS_NO_SRV,
                                               TRUE );

               /*if the SIM card is not inserted set the sim_state to NOT_AVAILABLE */
               if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
               {
                  mm_event = MM_STD_NO_IMSI;
               }
               
               /******************************
               * Build a REG CNF and send it *
               *******************************/

               /* Retrieve the PLMN list given in the Service Confirmation message */
               mmcoord_mmr_reg_cnf.available_plmn_list.length = num_plmns;

               /* Fill the Available PLMN list into the REG_CNF primitive */
               for (entry = 0; entry < num_plmns; entry++ )
               {
                 if(msg_p->cmd.rrc_service_cnf.avail_plmn_list.plmn[entry].rat == mm_serving_plmn.info.active_rat)
                 {
                   rrc_plmn_list = &msg_p->cmd.rrc_service_cnf.avail_plmn_list.plmn[entry];
                   reg_plmn_list = &mmcoord_mmr_reg_cnf.available_plmn_list.info[entry];

                   nas_type_plmn =
                     mmumts_convert_rrc_plmn_id_to_nas_plmn_id(rrc_plmn_list->plmn);

                   memscpy(reg_plmn_list->plmn.identity,PLMN_SIZE, nas_type_plmn.identity, PLMN_SIZE);
                   reg_plmn_list->plmn_service_capability = rrc_plmn_list->service_capability;
                   reg_plmn_list->signal_quality = rrc_plmn_list->signal_quality;
                   reg_plmn_list->signal_strength = rrc_plmn_list->signal_strength;
                   reg_plmn_list->rat = rrc_plmn_list->rat;
                   reg_plmn_list->plmn_forbidden = FALSE;
                   reg_plmn_list->list_category = SYS_DETAILED_PLMN_LIST_CATEGORY_PREFERRED;
                 }
                 else
                 {
                   MSG_FATAL_DS(MM_SUB," =MM= Rquested RAT UMTS recieved RAT GSM in entry %d,rat %d ",
                                     entry,msg_p->cmd.rrc_service_cnf.avail_plmn_list.plmn[entry].rat,0);
                 }
               }
               mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
               /* Set the PLMN service stat information in the REG_CNF primitive */
               mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
               mm_send_mmr_reg_cnf();

               mm_reg_waiting_for_reg_cnf = FALSE;
               mm_ready_to_send_reg_cnf   = FALSE;

               /*----------------------
                 Transition substates
                -----------------------*/
               mm_substate_control( mm_event );
            }
            else if (msg_p->cmd.rrc_service_cnf.service_status == MM_AS_SERVICE_AVAILABLE)
            {
               mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);            
               mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);
#ifdef FEATURE_FEMTO_CSG
               boolean   is_manual_csg_selection_success = FALSE;
#endif 

               sys_plmn_id_s_type curr_plmn = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(msg_p->cmd.rrc_service_cnf.service_info.selected_plmn.plmn_id);
               

#ifdef FEATURE_FEMTO_CSG
             /* Manual CSG selection is successful only if UE camps on non forbidden cell */
              if ((mmcoord_pending_reg_message.cmd.mmr_reg_req.csg_id == mm_serving_plmn.info.csg_info.csg_id) &&
                      (sys_plmn_match(mm_serving_plmn.info.plmn, mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn)) && 
                    (mmcoord_pending_reg_message.cmd.mmr_reg_req.csg_id != SYS_CSG_ID_INVALID))
              {
                is_manual_csg_selection_success = TRUE;
              }
#endif

               /*******************
               * Set the mm_event *
               ********************/
               if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
               {
                  mm_event = MM_STD_NO_IMSI;
               }
#ifdef FEATURE_NAS_ECALL
               else if ((mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY) &&
                        (mm_timer_status[TIMER_T3242] == TIMER_STOPPED) &&
                        (mm_timer_status[TIMER_T3243] == TIMER_STOPPED))
               {
                  mm_event =  MM_STD_ECALL_INACTIVE;
               }
#endif
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
                  if (((sys_plmn_match(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn, curr_plmn)) ||
                       (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
                        sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))
#ifdef FEATURE_FEMTO_CSG
                      &&(mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
#endif
                     )
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
                        (reg_sim_per_subs_plmn_forbidden(curr_plmn,(sys_modem_as_id_e_type)mm_sub_id))
#else
                        (reg_sim_plmn_forbidden(curr_plmn))
#endif
#ifdef FEATURE_FEMTO_CSG
                       /*If CSG is not in white list and this is not user selection of CSG*/
                      || (!mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info) && 
                             !is_manual_csg_selection_success)
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
                     if ((mm_possible_imsi_attach) && (mm_system_information.ATT))
                     {
                       mm_event = MM_STD_UPDATE_PENDING;
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
                     mm_event = MM_STD_UPDATE_PENDING;
                  }
               }
#ifdef FEATURE_SGLTE
              if (MM_SUB_IS_SGLTE &&
                   mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY) && 
              ((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
                         ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY )||
                         (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                                    SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY))&&
               (((mm_serving_plmn.network_op_mode != NETWORK_OP_MODE_1) && 
                (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_PS))||
                (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_ONLY))))
             {
                boolean mm_is_lai_changed ;
                if ( !((mm_location_information.location_update_status == UPDATED) &&
                    (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)) ||
                    !(sys_plmn_match(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn, curr_plmn)))
                 {
                    mm_is_lai_changed = TRUE;
                    MSG_HIGH_DS_0(MM_SUB, "=MM= LAI changed CAMP ONLY MODE");
                 }
                 if((mm_timer_status[TIMER_T3211] == TRUE) &&
                      (mm_is_lai_changed == FALSE))
                  {
                    mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CAMPED_ONLY;
                  }
                  else
                  {
                      mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CS_SERVICE_ONLY;
                  }
             }
#endif

               /*********************************************
               * Set the mm_serving_plmn global data values *
               **********************************************/
               mm_serving_plmn.mm_service_state = SERVICE_AVAILABLE;

               if ( ((mm_serving_plmn.info.service_status == SYS_SRV_STATUS_LIMITED) ||
                     (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_NO_SRV)) &&
                    (mm_event == MM_STD_NORMAL_SERVICE) &&
                    (mm_location_information.location_update_status == UPDATED) &&
                    (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0) &&
                    ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) ||
                     (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)))
               {
                  mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                  SYS_SRV_DOMAIN_CS_ONLY,
                                                  FALSE,
                                                  SYS_SRV_STATUS_SRV,
                                                  TRUE );
               }
               else
               {
                  mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                  mm_serving_plmn.info.reg_service_domain,
                                                  TRUE,
                                                  SYS_SRV_STATUS_SRV,
                                                  FALSE );
               }

               /* -----------------------------------------------------------
               ** Send camped Indication Call Manager to notify that UE has
               ** has acquired Service on a cell
               ** ---------------------------------------------------------*/
               mm_send_mmr_camped_ind(FALSE);

               if ((!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_PS)) ||
                   (mm_check_if_cs_cell_access_allowed(cs_cell_access) == FALSE)
#ifdef FEATURE_NAS_ECALL
                    ||
                   (mm_event == MM_STD_ECALL_INACTIVE)
#endif
                  )
               {
                  /******************************
                  * Build a REG CNF and send it *
                  *******************************/

                  /* Retrieve the PLMN list given in the Service Confirmation message */
                  mmcoord_mmr_reg_cnf.available_plmn_list.length = num_plmns;

                  /* Fill the Available PLMN list into the REG_CNF primitive */
                  for (entry = 0; entry < num_plmns; entry++)
                  {
                     rrc_plmn_list = &msg_p->cmd.rrc_service_cnf.avail_plmn_list.plmn[entry];
                     reg_plmn_list = &mmcoord_mmr_reg_cnf.available_plmn_list.info[entry];

                     nas_type_plmn =
                          mmumts_convert_rrc_plmn_id_to_nas_plmn_id(rrc_plmn_list->plmn);

                     memscpy(reg_plmn_list->plmn.identity, PLMN_SIZE, nas_type_plmn.identity, PLMN_SIZE);
                     reg_plmn_list->plmn_service_capability = rrc_plmn_list->service_capability;
                     reg_plmn_list->signal_quality = rrc_plmn_list->signal_quality;
                     reg_plmn_list->signal_strength = rrc_plmn_list->signal_strength;
                     reg_plmn_list->rat = rrc_plmn_list->rat;
                     reg_plmn_list->plmn_forbidden = FALSE;
                     reg_plmn_list->list_category = SYS_DETAILED_PLMN_LIST_CATEGORY_PREFERRED;
                  }

                  if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_PS))
                  {
                     if ((cs_cell_access == MM_AS_ACCESS_NO_CALLS) ||
                         (cs_cell_access == MM_AS_ACCESS_NORMAL_ONLY))

                     {
                        mm_update_mm_serving_plmn_info(mm_serving_plmn.info.sim_state,
                                                       mm_serving_plmn.info.reg_service_domain,
                                                       TRUE,
                                                       SYS_SRV_STATUS_NO_SRV,
                                                       TRUE );
                     }
                     else
                     {
                        mm_update_mm_serving_plmn_info(mm_serving_plmn.info.sim_state,
                                                       mm_serving_plmn.info.reg_service_domain,
                                                       TRUE,
                                                       SYS_SRV_STATUS_LIMITED,
                                                       TRUE );
                     }
                  }
#ifdef FEATURE_NAS_ECALL
                  else if (mm_event ==  MM_STD_ECALL_INACTIVE)
                  {
              
                     mm_delete_lu_status(UPDATING_DISABLED);

                     mm_update_mm_serving_plmn_info(mm_serving_plmn.info.sim_state,
                                                       SYS_SRV_DOMAIN_NO_SRV,
                                                       TRUE,
                                                       SYS_SRV_STATUS_LIMITED,
                                                       TRUE );

                     mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;

                  }
#endif
                  else if (cs_cell_access == MM_AS_ACCESS_NO_CALLS)
                  {
                     mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     mm_serving_plmn.info.reg_service_domain,
                                                     TRUE,
                                                     SYS_SRV_STATUS_NO_SRV,
                                                     TRUE );

                     if (mm_event == MM_STD_LIMITED_SERVICE)
                     {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                     }
                     else
                     {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                     }
                  }
                  else
                  {
                     mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     mm_serving_plmn.info.reg_service_domain,
                                                     TRUE,
                                                     SYS_SRV_STATUS_LIMITED,
                                                     TRUE );

                     if (mm_event == MM_STD_LIMITED_SERVICE)
                     {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                     }
                     else
                     {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                     }
                  }

                  mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
 
                  /*****************************************************************
                  ** If DSAC is enabled, CS and PS Access Restrictions will be 
                  ** different. Therefore, it will be incorrect to inform REG about 
                  ** No Service/Limited Service if some service on PS domain is
                  ** allowed, because that might trigger PLMN Selection from REG/CM.
                  *****************************************************************/
 
                  if ((gmm_check_if_ps_cell_access_allowed(ps_cell_access) == FALSE) || 
                      (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY))
                  {                  
                     mm_send_mmr_reg_cnf();

                     mm_reg_waiting_for_reg_cnf = FALSE;
                     mm_ready_to_send_reg_cnf   = FALSE;
                  }   
               }

               /*----------------------
                 Transition substates
                -----------------------*/
               mm_substate_control( mm_event );
            }
         }

         /*------------------------------------------------
          RR PLMN SELECT CNF handling
          ------------------------------------------------*/
         else if (msg_p->cmd.hdr.message_id == (int) RR_PLMN_SELECT_CNF)
         {
            mm_as_service_state_T plmn_select_status =
              msg_p->cmd.rr_plmn_select_cnf.service_state;

            num_plmns =  MIN(msg_p->cmd.rr_plmn_select_cnf.available_plmn_list.length,SYS_PLMN_LIST_MAX_LENGTH);

            /*
             *  Handle the response from RR after a LAI selection
             *  If the success or failure is reported to MMR. If a
             *  failure, RR reports the current list of location area
             *  identifications (LAIs).
             */

            /*
             * first store the LAI where we are currently camped
             */

            if (plmn_select_status == MM_AS_NO_SERVICE)
            {
               byte null_lai[LAI_SIZE] = { 0xff, 0xff, 0xff, 0, 0 };

               /*******************
               * Set the mm_event *
               ********************/
               mm_event = MM_STD_NO_SERVICE;

               /*********************************************
               * Set the mm_serving_lai global data values *
               **********************************************/
               (void)memscpy( mm_serving_lai, LAI_SIZE, null_lai, LAI_SIZE);

               /*********************************************
               * Set the mm_serving_plmn global data values *
               **********************************************/
               mm_serving_plmn.mm_service_state    = SERVICE_NOT_AVAILABLE;

               mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                               mm_serving_plmn.info.reg_service_domain,
                                               TRUE,
                                               SYS_SRV_STATUS_NO_SRV,
                                               TRUE );

               /*if the SIM card is not inserted set the sim_state to NOT_AVAILABLE */
               if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
               {
                  mm_event = MM_STD_NO_IMSI;
               }

               mm_substate_control( mm_event );

               /******************************
               * Build a REG CNF and send it *
               *******************************/

               /* Fill the REG PLMN list from the received RR list */
               mmcoord_mmr_reg_cnf.available_plmn_list.length = num_plmns;

               for (entry = 0; entry < num_plmns; entry++)
               {
                 if( msg_p->cmd.rr_plmn_select_cnf.available_plmn_list.info[entry].rat == mm_serving_plmn.info.active_rat)
                 {
                   mmcoord_mmr_reg_cnf.available_plmn_list.info[entry] =
                      msg_p->cmd.rr_plmn_select_cnf.available_plmn_list.info[entry];
                 }
                 else
                 {
                  MSG_FATAL_DS(MM_SUB," =MM= Rquested RAT GSM recieved RAT UNMTS in entry %d,rat %d ",
                                     entry,msg_p->cmd.rr_plmn_select_cnf.available_plmn_list.info[entry].rat,0);
                 }
               }
               mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
               /* Set the REG CNF plmn service state */
               mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;

               /*
               * Send a registration confirmation to REG
               */

               mm_send_mmr_reg_cnf();

               mm_reg_waiting_for_reg_cnf = FALSE;
               mm_ready_to_send_reg_cnf   = FALSE;
            }
            else if (plmn_select_status == MM_AS_SERVICE_AVAILABLE)
            {
               /*******************
               * Set the mm_event *
               ********************/
               if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
               {
                  mm_event = MM_STD_NO_IMSI;
               }
#ifdef FEATURE_NAS_ECALL
               else if ((mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY)&&
                        (mm_timer_status[TIMER_T3242] == TIMER_STOPPED) &&
                        (mm_timer_status[TIMER_T3243] == TIMER_STOPPED))
               {
                  mm_event =  MM_STD_ECALL_INACTIVE;
               }
#endif
               else if (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode
                   == SYS_NETWORK_SELECTION_MODE_LIMITED)
               {
                  MSG_HIGH_DS_0(MM_SUB,"=MM= PLMN changed, Limited service req ");
                  mm_event = MM_STD_LIMITED_SERVICE;
               }
               else if (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode
                   == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
               {
                  sys_plmn_id_s_type curr_plmn = msg_p->cmd.rr_plmn_select_cnf.location_area_identification.PLMN_id;
                  /*
                  ** If requested PLMN match with the received PLMN then NORMAL service
                  ** Otherwise, make this LIMITED service.
                  */
                  MSG_HIGH_DS_0(MM_SUB, "=MM= PLMN changed. Manual acquisition of Service\n");
                  if ((sys_plmn_match(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn, curr_plmn)) ||
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
               else if (mm_is_lai_forbidden(
                           msg_p->cmd.rr_plmn_select_cnf.location_area_identification ) ||
#ifdef FEATURE_DUAL_SIM
                        (reg_sim_per_subs_plmn_forbidden(
                           msg_p->cmd.rr_plmn_select_cnf.location_area_identification.PLMN_id, (sys_modem_as_id_e_type)mm_sub_id ))
#else
                        (reg_sim_plmn_forbidden(
                           msg_p->cmd.rr_plmn_select_cnf.location_area_identification.PLMN_id ))
#endif
                          )
               {
                  MSG_HIGH_DS_0(MM_SUB, "=MM= PLMN or LAI is in a forbidden list");
                  mm_event = MM_STD_LIMITED_SERVICE;
               }
               else if ((msg_p->cmd.rr_plmn_select_cnf.cell_access ==
                           MM_AS_ACCESS_ALL_CALLS) ||
                        (msg_p->cmd.rr_plmn_select_cnf.cell_access ==
                           MM_AS_ACCESS_NORMAL_ONLY))
               {
                  mm_event = MM_STD_NORMAL_SERVICE;
               }
               else
               {
                  if ((mm_location_information.location_update_status == UPDATED) &&
                      (memcmp(mm_location_information.lai,
                              &msg_p->cmd.rr_plmn_select_cnf.location_area_identification,
                              LAI_SIZE) == 0))
                  {
                     if ((mm_possible_imsi_attach) && (msg_p->cmd.rr_plmn_select_cnf.ATT))
                     {
                       mm_event = MM_STD_UPDATE_PENDING;
                     }
                     else
                     {
                       mm_event = MM_STD_NORMAL_SERVICE;
                     }
                  }
                  else
                  {
                     mm_event = MM_STD_UPDATE_PENDING;
                  }
               }

#ifdef FEATURE_SGLTE
              if (MM_SUB_IS_SGLTE &&
                   mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY) && 
              ((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
                         ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY )||
                         (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                                    SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY))&&
               (((mm_serving_plmn.network_op_mode != NETWORK_OP_MODE_1) && 
                (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_PS))||
                (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_ONLY))))
             {
                boolean mm_is_lai_changed ;
                sys_plmn_id_s_type curr_plmn = msg_p->cmd.rr_plmn_select_cnf.location_area_identification.PLMN_id;
                if ( !((mm_location_information.location_update_status == UPDATED) &&
                    (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)) ||
                    !(sys_plmn_match(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn, curr_plmn)))
                 {
                    mm_is_lai_changed = TRUE;
                    MSG_HIGH_DS_0(MM_SUB, "=MM= LAI changed CAMP ONLY MODE");
                 }
                 if((mm_timer_status[TIMER_T3211] == TRUE) &&
                      (mm_is_lai_changed == FALSE))
                  {
                    mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CAMPED_ONLY;
                  }
                  else
                  {
                      mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_CS_SERVICE_ONLY;
                  }
             }
#endif
               mm_substate_control( mm_event );

               /*********************************************
               * Set the mm_serving_lai global data values *
               **********************************************/
               (void)memscpy( mm_serving_lai, LAI_SIZE ,
                             &msg_p->cmd.rr_plmn_select_cnf.location_area_identification,
                             LAI_SIZE);

               MSG_HIGH_DS_5(MM_SUB, "=MM= Serving LAI PLMN %x %x %x and Serving LAC %x %x",
				   mm_serving_lai[0], mm_serving_lai[1], mm_serving_lai[2], 
				   mm_serving_lai[3], mm_serving_lai[4]);

               /*********************************************
               * Set the mm_serving_plmn global data values *
               **********************************************/
               mm_serving_plmn.mm_service_state = SERVICE_AVAILABLE;
               
               if ( ((mm_serving_plmn.info.service_status == SYS_SRV_STATUS_LIMITED) ||
                     (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_NO_SRV)) &&
                    (mm_event == MM_STD_NORMAL_SERVICE) &&
                    (mm_location_information.location_update_status == UPDATED) &&
                    (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0) &&
                     ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY) ||
                     (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)))
               {
                  mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                  SYS_SRV_DOMAIN_CS_ONLY,
                                                  FALSE,
                                                  SYS_SRV_STATUS_SRV,
                                                  TRUE );
               }
               else
               {
                  mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                  mm_serving_plmn.info.reg_service_domain,
                                                  TRUE,
                                                  SYS_SRV_STATUS_SRV,
                                                  FALSE );
               }

               /************************************************
               * Store the System information received from RR *
               *************************************************/
               mmgsm_store_system_information( msg_p );

               /* ------------------------------------------------------------
               ** Send camped Indication Call Manager to notify that UE has
               ** has acquired Service on a cell
               ** ----------------------------------------------------------*/
               mm_send_mmr_camped_ind(FALSE);

               if ((!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_PS)) ||
                   ((msg_p->cmd.rr_plmn_select_cnf.cell_access != MM_AS_ACCESS_ALL_CALLS) &&
                    (msg_p->cmd.rr_plmn_select_cnf.cell_access != MM_AS_ACCESS_NORMAL_ONLY))
#ifdef FEATURE_NAS_ECALL
                     ||
                    (mm_event == MM_STD_ECALL_INACTIVE)
#endif
                  )
               {
                  /******************************
                  * Build a REG CNF and send it *
                  *******************************/

                  /* Fill the REG PLMN list from the received RR list */
                  mmcoord_mmr_reg_cnf.available_plmn_list.length = num_plmns;

                  for (entry = 0; entry < num_plmns; entry++)
                  {
                     mmcoord_mmr_reg_cnf.available_plmn_list.info[entry] =
                        msg_p->cmd.rr_plmn_select_cnf.available_plmn_list.info[entry];
                  }

                  if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_PS))
                  {
                     if ((msg_p->cmd.rr_plmn_select_cnf.cell_access ==
                             MM_AS_ACCESS_NO_CALLS) ||
                         (msg_p->cmd.rr_plmn_select_cnf.cell_access ==
                             MM_AS_ACCESS_NORMAL_ONLY))

                     {
                        mm_update_mm_serving_plmn_info(mm_serving_plmn.info.sim_state,
                                                       mm_serving_plmn.info.reg_service_domain,
                                                       TRUE,
                                                       SYS_SRV_STATUS_NO_SRV,
                                                       TRUE );
                     }
                     else
                     {
                        mm_update_mm_serving_plmn_info(mm_serving_plmn.info.sim_state,
                                                       mm_serving_plmn.info.reg_service_domain,
                                                       TRUE,
                                                       SYS_SRV_STATUS_LIMITED,
                                                       TRUE );
                     }
                  }
#ifdef FEATURE_NAS_ECALL
                  else if (mm_event ==  MM_STD_ECALL_INACTIVE)
                  {
              
                     mm_delete_lu_status(UPDATING_DISABLED);

                     mm_update_mm_serving_plmn_info(mm_serving_plmn.info.sim_state,
                                                       SYS_SRV_DOMAIN_NO_SRV,
                                                       TRUE,
                                                       SYS_SRV_STATUS_LIMITED,
                                                       TRUE );

                     mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;

                  }
#endif
                  else if (msg_p->cmd.rr_plmn_select_cnf.cell_access ==
                             MM_AS_ACCESS_NO_CALLS)
                  {
                     mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     mm_serving_plmn.info.reg_service_domain,
                                                     TRUE,
                                                     SYS_SRV_STATUS_NO_SRV,
                                                     TRUE );

                     if (mm_event == MM_STD_LIMITED_SERVICE)
                     {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                     }
                     else
                     {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                     }
                  }
                  else
                  {
                     mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     mm_serving_plmn.info.reg_service_domain,
                                                     TRUE,
                                                     SYS_SRV_STATUS_LIMITED,
                                                     TRUE );

                     if (mm_event == MM_STD_LIMITED_SERVICE)
                     {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                     }
                     else
                     {
                        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
                     }
                  }

                  mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                  mm_send_mmr_reg_cnf();

                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf   = FALSE;
               }
            }
         }
         else
         {
            MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected RR message id %d", msg_p->cmd.hdr.message_id, 0,0);
         }
   
        
         break;

      default:
         MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected message set %d", msg_p->cmd.hdr.message_set, 0,0);
         break;
   }
}


/*===========================================================================

FUNCTION MM_HANDLE_PLMN_FOUND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_handle_plmn_found( serving_plmn_info_T serving_plmn,
                           boolean             location_update_required )
{
   boolean                forbidden_national_roaming;
   inter_task_lai_T       found_lai;
   mmr_service_ind_s_type mmr_service_ind;
   byte                   current_lai[LAI_SIZE];
   boolean routine_manual_selection_success = FALSE;
   boolean is_cell_forbidden = TRUE;
   
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
   boolean is_lu_to_be_initiated = mm_is_lu_to_be_done_due_to_t3346();
#endif

#ifdef FEATURE_FEMTO_CSG
   boolean is_manual_csg_selection_success = FALSE;
#endif 

#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   found_lai.PLMN_id.identity[0] = serving_plmn.info.plmn.identity[0];
   found_lai.PLMN_id.identity[1] = serving_plmn.info.plmn.identity[1];
   found_lai.PLMN_id.identity[2] = serving_plmn.info.plmn.identity[2];

   found_lai.location_area_code[0] = serving_plmn.lac.lac[0];
   found_lai.location_area_code[1] = serving_plmn.lac.lac[1];

   current_lai[0] = serving_plmn.info.plmn.identity[0];
   current_lai[1] = serving_plmn.info.plmn.identity[1];
   current_lai[2] = serving_plmn.info.plmn.identity[2];
   current_lai[3] = serving_plmn.lac.lac[0];
   current_lai[4] = serving_plmn.lac.lac[1];
   /*
    * check if REG has enabled LU and that we are not in a forbidden PLMN
    */

   if (!mmsim_sim_valid( SYS_SRV_DOMAIN_CS_ONLY ))
   {
      mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                      SYS_SRV_DOMAIN_NO_SRV,
                                      FALSE,
                                      SYS_SRV_STATUS_LIMITED,
                                      TRUE );

      if((!((mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY) && 
           (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
           (mm_serving_plmn.info.plmn_service_capability == SYS_SRV_CAPABILITY_CS_PS_SERVICE)))) ||
         (((gmm_check_gprs_forbidden_rai(mm_serving_plmn.info.plmn,mm_serving_plmn.lac,mm_serving_plmn.rac)) ||
#ifdef FEATURE_DUAL_SIM
           (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id ))
#else
           (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn ))
#endif
          ) &&
          (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
          (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)))
      {
        if (mm_reg_waiting_for_reg_cnf)
        {
          mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
          mm_send_mmr_reg_cnf();
          mm_reg_waiting_for_reg_cnf = FALSE;
          mm_ready_to_send_reg_cnf   = FALSE;
        }
      }
      else
      {
        mm_ready_to_send_reg_cnf = TRUE;
      }
   }
   else if (mm_serving_plmn.mm_service_state == SERVICE_NOT_AVAILABLE)
   {
      mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                      mm_serving_plmn.info.reg_service_domain,
                                      FALSE,
                                      SYS_SRV_STATUS_NO_SRV,
                                      TRUE );

      if (mm_reg_waiting_for_reg_cnf)
      {
        if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
             (mm_mmr_attach_ind_status == ATTACH_FAILED_IND_REQUIRED) &&
             (mm_serving_plmn.network_op_mode != NETWORK_OP_MODE_1) &&
             ((mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE) && 
              (gmm_check_if_ps_cell_access_allowed(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)) == TRUE))) 
         {
            mm_mmr_attach_ind_status = ATTACH_FAILED_IND_WAITING_FOR_GMM;
         }
         mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
         mm_send_mmr_reg_cnf();
         mm_reg_waiting_for_reg_cnf = FALSE;
         mm_ready_to_send_reg_cnf   = FALSE;
      }
   }
#ifdef FEATURE_NAS_ECALL
   else if ((mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY) &&
            (mm_idle_substate == MM_ECALL_INACTIVE) )
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
 
   }
#endif
#ifdef FEATURE_NAS_REL10
   else if  ((((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                  &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
                ) &&(mm_is_plmn_equivalent_t3246_plmn())) ||
               (!(mm_manual_mode_user_selection_procedure &&
               (sys_plmn_match(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn,
                                gmm_3gpp_t3346_plmn_id) == TRUE)) &&
               (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
               (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
               (gmm_is_plmn_equivalent_t3346()) &&
               (is_lu_to_be_initiated == FALSE)))
   {
       MSG_HIGH_DS_0(MM_SUB, "=MM=T3246 is running. pending LU");

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
   }
#endif
   else if (location_update_required)
   {
      forbidden_national_roaming = mm_check_forbidden_national_roaming( &found_lai );
      if (((PLMN_MATCH( mm_serving_lai,
                       mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity )) ||
          (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
           sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))  &&                       
          (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                       SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
     #ifdef FEATURE_FEMTO_CSG
                 &&  (mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
     #endif  
            )
      {
        routine_manual_selection_success = TRUE;
        MSG_HIGH_DS_0(MM_SUB, "=MM= Same PLMN found during MAN_LIMITED_ACQ");
      }

     if ((!forbidden_national_roaming) &&
         (!mm_check_forbidden_regional_roaming( &found_lai )) &&
#ifdef FEATURE_DUAL_SIM
         (!reg_sim_per_subs_plmn_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id ))
#else
         (!reg_sim_plmn_forbidden( mm_serving_plmn.info.plmn ))
#endif
#ifdef FEATURE_FEMTO_CSG
         && (mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
#endif 
         )
     {
       is_cell_forbidden = FALSE;
     }

#ifdef FEATURE_FEMTO_CSG
      if((mmcoord_pending_reg_message.cmd.mmr_reg_req.csg_id == mm_serving_plmn.info.csg_info.csg_id) &&
         (sys_plmn_match(mm_serving_plmn.info.plmn, mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn)) &&
         (mm_serving_plmn.info.csg_info.csg_id != SYS_CSG_ID_INVALID))
     {
       is_manual_csg_selection_success = TRUE;
     }
#endif 

     if ((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
             SYS_NETWORK_SELECTION_MODE_MANUAL) ||
          (routine_manual_selection_success)) &&
         ((mm_manual_mode_user_selection_procedure) ||
          (!is_cell_forbidden))) ||
         (((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
             SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
             SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) &&
          ((!is_cell_forbidden)))
#ifdef FEATURE_FEMTO_CSG
             || (is_manual_csg_selection_success)
#endif 
          )

      {
            
#ifdef FEATURE_DSAC
         /* ---------------------------------------------------------------- 
         ** UE is UPDATED, however, No Imsi Attach required. Check if LU is 
         ** required as per section 4.7.2.2 and 
         ** ---------------------------------------------------------------- */
         if ((((byte)(mm_location_information.location_update_status) & (byte)(0x07)) == 
                (byte)UPDATED) &&
             (memcmp(mm_location_information.lai,&found_lai,LAI_SIZE) == 0) &&
             (!(mm_possible_imsi_attach && mm_system_information.ATT)))
         {
            if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                ((gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)) == TRUE) ||
                 (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE))) 
            {
               if(gmm_periodic_rau_pending)
               {
                   if ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                            ((mm_last_procedure_to_update_registration & MM_LU) != MM_LU))
                   {
                      MSG_HIGH_DS_0(MM_SUB, "=MM= Performing NORMAL LU as T3312 expired in OOS in NMO 1 nw, Breaking Gs Association ");
                      mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                   } 
                   else if ((mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE) && 
                                     ((mm_last_procedure_to_update_registration & MM_LU) != MM_LU))
                   {
                      MSG_HIGH_DS_0(MM_SUB, "=MM= Performing PERIODIC LU as T3312 expired in OOS in NMO 1 nw");
                      mm_initiate_lu( MM_LU_CAUSE_PERIODIC );
                   }                  
               }
               else if ((mm_cs_access_barred_to_unbarred) &&
                        (gmm_check_if_ps_cell_access_barred(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)) == TRUE) &&
                        ((mm_last_procedure_to_update_registration & MM_LU) != MM_LU) &&
                        (mm_last_procedure_to_update_registration != MM_COMBINED))
               {
                  MSG_HIGH_DS_0(MM_SUB, "=MM= Performing NORMAL LU as T3312 expired in OOS in NMO 1 nw, Breaking Gs Association ");
                  mm_initiate_lu( MM_LU_CAUSE_NORMAL );            
               }               
            }            
            else if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)
            {
               if(gmm_periodic_rau_pending &&                             
                                   ((mm_last_procedure_to_update_registration & MM_LU) != MM_LU))
               {
                      MSG_HIGH_DS_0(MM_SUB, "=MM= Performing PERIODIC LU as T3312 expired in OOS in NMO 1 nw");
                      mm_initiate_lu( MM_LU_CAUSE_PERIODIC );                  
               }
            }            
         }          
#endif      
      
         /*
          * read the LOCI file from the SIM, which contains
          * TMSI, LAI, TMSI time, and location update status
          */

         if (( mm_system_information.ATT ) &&
             ( ((byte)(mm_location_information.location_update_status) &
             (byte)(0x07)) == (byte)UPDATED )
            &&
             ( memcmp( mm_location_information.lai,
                       &found_lai,
                       LAI_SIZE ) == 0 ) )
         {
            mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                            SYS_SRV_DOMAIN_CS_ONLY,
                                            FALSE,
                                            SYS_SRV_STATUS_SRV,
                                            TRUE );

            MSG_HIGH_DS_0(MM_SUB,"=MM= Same Lai-LU not required, though possible IMSI attach");

            mm_substate_control( MM_STD_NORMAL_SERVICE );

            /* Perform any Pending T3212 expired pending LU procedure now */
            if ((mm_pending_lu_request == T3212_EXPIRED) ||
                (mm_pending_lu_request == T3312_EXPIRED))
            {
              MSG_HIGH_DS_0(MM_SUB, "=MM= Performing Pending T3212 LU procedure");
              mm_initiate_lu( MM_LU_CAUSE_PERIODIC );
            }
#if defined(FEATURE_LTE)
            else if (((gmm_lte_deact_isr_timer_expired == TRUE )||
                          (mm_timer_status[TIMER_T3423] == TIMER_ACTIVE &&
                           emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI))
                        || ((mm_pending_lu_request == LTE_IRAT_LU ||
                             mm_pending_lu_request == LU_ABORTED_IN_UPDATED_CELL)
                       && (mm_csfb_state == MM_CSFB_STATE_NULL)))
            {
              MSG_HIGH_DS_3(MM_SUB,"=MM= LU started due to IRAT from LTE: gmm_lte_deact_isr_timer_expired=%d mm_pending_lu_request=%d T3423 status=%d",
                               gmm_lte_deact_isr_timer_expired, mm_pending_lu_request, mm_timer_status[TIMER_T3423]);

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
            else if (mm_possible_imsi_attach)
            {
               if(sys_eplmn_list_validate(TRUE))
               {
                 MSG_HIGH_DS_0(MM_SUB, "=MM= EPLMN list is now validated");
                 mm_send_rrc_eq_plmn_list_change_ind();
                 mm_send_rr_eq_plmn_list_change_ind();
#ifdef FEATURE_LTE
                 emm_send_multimode_rrc_system_update_from_GW(emm_ctrl_data_ptr);
#endif    
               }
               if (mm_reg_waiting_for_reg_cnf)
               {
                  if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                       (mm_mmr_attach_ind_status == ATTACH_FAILED_IND_REQUIRED) &&
                       (mm_serving_plmn.network_op_mode != NETWORK_OP_MODE_1) &&
                       ((mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE) && 
                        (gmm_check_if_ps_cell_access_allowed(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)) == TRUE))) 
                  {
                    mm_mmr_attach_ind_status = ATTACH_FAILED_IND_WAITING_FOR_GMM;
                  }

                  mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                  mm_send_mmr_reg_cnf();
                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf   = FALSE;
               }

               if ((mm_previous_idle_substate == MM_ATTEMPTING_TO_UPDATE) ||
                   (mm_timer_status[TIMER_T3211] != TIMER_STOPPED))
               {
                  if ((memcmp( mm_previous_serving_lai, current_lai, LAI_SIZE ) == 0) && (mm_follow_on_request == FALSE))
                  {
                     if (mm_previous_idle_substate == MM_ATTEMPTING_TO_UPDATE)
                     {
                       mm_substate_control( MM_STD_UPDATE_ATTEMPTED );
                     }
                  }
                  else
                  {
                     mm_initiate_lu( MM_LU_CAUSE_IMSI_ATTACH );
                  }
               }
               else
               {
                  mm_initiate_lu( MM_LU_CAUSE_IMSI_ATTACH );
               }
            }
            else if ((mm_pending_lu_request == T3122_EXPIRED) &&
                     (!mm_serving_plmn.t3122_running))
            {
               mm_initiate_lu(mm_get_lu_cause());
            }
#ifdef FEATURE_FEMTO_CSG
            else if (is_manual_csg_selection_success 
                         && !mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
            {
               MSG_HIGH_DS_0(MM_SUB,"=MM= CSG: Manually selected CSG not in white list - Perform LAU");
               mm_initiate_lu(MM_LU_CAUSE_NORMAL);
            }
#endif 
            else
            {
               if (mm_managed_roaming_enabled)
               {
                 mm_serving_plmn.info.cs_service_status  = SYS_SRV_STATUS_SRV;  
               }
#ifdef FEATURE_ENHANCED_NW_SELECTION
               if (reg_sim_read_ens_flag())
               {
                 mm_serving_plmn.info.cs_service_status  = SYS_SRV_STATUS_SRV;  
               }
#endif
               mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;

               /* Since responding to REG without LU, set EPLMN list to valid again */
               if(sys_eplmn_list_validate(TRUE))
               {
                 MSG_HIGH_DS_0(MM_SUB, "=MM= EPLMN list is now validated");
                 mm_send_rrc_eq_plmn_list_change_ind();
                 mm_send_rr_eq_plmn_list_change_ind();  
#ifdef FEATURE_LTE
                 emm_send_multimode_rrc_system_update_from_GW(emm_ctrl_data_ptr);
#endif    
               }
               
               if (mm_reg_waiting_for_reg_cnf)
               {
                  if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                       (mm_mmr_attach_ind_status == ATTACH_FAILED_IND_REQUIRED) &&
                       (mm_serving_plmn.network_op_mode != NETWORK_OP_MODE_1) &&
                       (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE) &&
                       (gmm_check_if_ps_cell_access_allowed(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)) == TRUE))
                  {
                     mm_mmr_attach_ind_status = ATTACH_FAILED_IND_WAITING_FOR_GMM;
                  }
                  gmm_manual_mode_user_selection_procedure = FALSE;
                  mm_manual_mode_user_selection_procedure = FALSE;
#ifdef FEATURE_LTE
                  mm_set_emm_manual_selection_flag(FALSE);
#endif
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
#ifdef FEATURE_ENHANCED_NW_SELECTION
               if (reg_sim_read_ens_flag() && ((mmsim_is_ef_rat_present()) ||
                  (gmm_nv_disable_call_type & GMM_CALL_TYPE_VOICE_MASK))) 
               {
                  if (mm_force_lu)
                  {
                     MSG_HIGH_DS_0(MM_SUB,"=MM= LU required for ENS purposes-forced to do normal LU here");
                     mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                  }
               }
#endif
            }

            if(mm_state == MM_LOCATION_UPDATE_INITIATED || mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= Do not stop T3210 if mm state is 3 or 13");
            }
            else
            {
              mm_stop_timer( TIMER_T3210);
            }
			
            if(mm_pending_lu_request != LTE_IRAT_LU)
            {
              mm_pending_lu_request = NO_LU;
            }
            mm_lu_pended_nmo_unknown = FALSE;
         }
         else
         {
            /*
             * If location_information.LAI does not match LAI in message
             * or the update status is not updated, then do a LU
             */

            if ( ( memcmp( mm_location_information.lai,
                           &found_lai,
                           LAI_SIZE ) != 0 ) ||
                 ( ((byte)(mm_location_information.location_update_status) &
                              (byte)(0x07)) != (byte)UPDATED ))
            {
               mm_substate_control( MM_STD_UPDATE_ATTEMPTED );

               MSG_HIGH_DS_1( MM_SUB, "=MM= LAI change, or status(%u) is NOT_UPDATED - LU required",
                     (uint32)mm_location_information.location_update_status);
#ifdef FEATURE_ENHANCED_NW_SELECTION
               if(mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai))
               {
                  MSG_HIGH_DS_0(MM_SUB,"=MM= Serving LAI is in FOREGROUND SEARCH LAI LIST- Dont do LU");

                  mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list(mm_serving_lai);

                  if ((memcmp( mm_previous_serving_lai, current_lai, LAI_SIZE ) != 0) || (mm_follow_on_request))
                  {
                     /* Reset attempt counter on new plmn selection if
                        mm susbstate is ATTEMPTING TO UPDATE
                     */
                     mm_clear_lu_attempt_counter();
                  }
                  if (mm_follow_on_request)
                  {
                    mm_clear_foreground_search_lai_list();
                    mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                  }
                  else
                  {
                    mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                    SYS_SRV_DOMAIN_NO_SRV,
                                                    FALSE,
                                                    SYS_SRV_STATUS_LIMITED,
                                                    TRUE );
                    
                    
                    if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                       ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                     || (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
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
                  }
               }
               else
#endif

               if (mm_previous_idle_substate == MM_ATTEMPTING_TO_UPDATE)
               {
                  if ((memcmp( mm_previous_serving_lai, current_lai, LAI_SIZE ) != 0) || (mm_follow_on_request))
                  {
                     /* Reset attempt counter on new plmn selection if
                        mm susbstate is ATTEMPTING TO UPDATE
                     */
                     mm_clear_lu_attempt_counter();

                     mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                     mm_is_foreground_search_to_be_initiated = TRUE;
                  }
                  else if ((mm_pending_lu_request == T3122_EXPIRED) &&
                           (!mm_serving_plmn.t3122_running))
                  {
                     mm_initiate_lu(mm_get_lu_cause());
                  }
                  else if(mm_managed_roaming_enabled &&
                          (memcmp( mm_previous_serving_lai, current_lai, LAI_SIZE ) == 0) &&
                          (mm_lu_attempt_counter == 4) &&
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                          (reg_sim_plmn_in_low_priority_plmn_list(serving_plmn.info.plmn, serving_plmn.info.active_rat,(sys_modem_as_id_e_type)mm_as_id)) &&
#else
                          (reg_sim_plmn_in_low_priority_plmn_list(serving_plmn.info.plmn, serving_plmn.info.active_rat)) &&
#endif
                          (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
                           mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))
                  {
                    mm_clear_lu_attempt_counter();
                    mm_initiate_lu( MM_LU_CAUSE_NORMAL );  
                    mm_is_foreground_search_to_be_initiated = FALSE;
                    MSG_HIGH_DS_1(MM_SUB ,"=MM= Foreground search need not be done again %d",
                    mm_is_foreground_search_to_be_initiated);
                  }
                  else if(mm_managed_roaming_enabled &&
                          (memcmp( mm_previous_serving_lai, current_lai, LAI_SIZE ) == 0) &&
                          (mm_lu_attempt_counter == 4) &&
                          mm_manual_mode_user_selection_procedure &&
                          (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                           mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
                  {

                  /* User selection shall trigger 4 more attempts*/
                    mm_clear_lu_attempt_counter();
                    
                    mm_initiate_lu( MM_LU_CAUSE_NORMAL );  
                    if( (mm_managed_roaming_retry_lu_in_manual_mode) &&
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                         (reg_sim_plmn_in_low_priority_plmn_list(serving_plmn.info.plmn, serving_plmn.info.active_rat,(sys_modem_as_id_e_type)mm_as_id)) 
#else
                         (reg_sim_plmn_in_low_priority_plmn_list(serving_plmn.info.plmn, serving_plmn.info.active_rat))
#endif
                     )
                     {
                       mm_is_foreground_search_to_be_initiated = FALSE;
                     }
                  }
#ifdef FEATURE_NAS_REL10
                  else if(((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
                           (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                           (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                           (mm_manual_mode_user_selection_procedure &&
                            (sys_plmn_match(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn,
                                                         gmm_3gpp_t3346_plmn_id) == TRUE)))
                          ||  (is_lu_to_be_initiated == TRUE))
                  {
                      MSG_HIGH_DS_1(MM_SUB,"=MM= LU started due to T3346 running in LTE or in NMO1 netwrok %d", mm_timer_status[TIMER_T3346]);
                      mm_initiate_lu(MM_LU_CAUSE_NORMAL);

                  }
#endif
                  else
                  {
                     if ((mm_lu_attempt_counter >= 4) && (mm_reg_waiting_for_reg_cnf))
                     {
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
                              || (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3)
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
                           mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;                        
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
                  if ((memcmp( mm_previous_serving_lai, current_lai, LAI_SIZE ) != 0) || (mm_follow_on_request))
                  {
                     mm_is_foreground_search_to_be_initiated = TRUE;
                     mm_clear_lu_attempt_counter();
                  }               
                  mm_initiate_lu( MM_LU_CAUSE_NORMAL );
               }
            }
            else
            {
               MSG_HIGH_DS_0(MM_SUB,"=MM= LAI changed - LA same as sim, so NO LU required");

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
                                               TRUE );
               (void) memscpy( mm_previous_serving_lai, LAI_SIZE,
                              mm_location_information.lai, LAI_SIZE );
               mm_previous_idle_substate = mm_idle_substate;

               if (mm_managed_roaming_enabled)
               {
                 mm_serving_plmn.info.cs_service_status  = SYS_SRV_STATUS_SRV;  
               }
#ifdef FEATURE_ENHANCED_NW_SELECTION
               if (reg_sim_read_ens_flag())
               {
                 mm_serving_plmn.info.cs_service_status  = SYS_SRV_STATUS_SRV;  
               }
#endif

               mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;

#ifdef FEATURE_FEMTO_CSG
               if(mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
               {
#endif 
               if (mm_reg_waiting_for_reg_cnf)
               {
                  if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                       (mm_mmr_attach_ind_status == ATTACH_FAILED_IND_REQUIRED) &&
                       (mm_serving_plmn.network_op_mode != NETWORK_OP_MODE_1) &&
                       (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE) &&
                       (gmm_check_if_ps_cell_access_allowed(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION)) == TRUE)) 
                  {
                     mm_mmr_attach_ind_status = ATTACH_FAILED_IND_WAITING_FOR_GMM;
                  }
                  mm_send_mmr_reg_cnf();
                  gmm_manual_mode_user_selection_procedure = FALSE;
                  mm_manual_mode_user_selection_procedure = FALSE;
#ifdef FEATURE_LTE 
                  mm_set_emm_manual_selection_flag(FALSE);
#endif
                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf   = FALSE;
               }
#ifdef FEATURE_FEMTO_CSG
               }
#endif 
               if ((mm_pending_lu_request == T3212_EXPIRED) ||
                   (mm_pending_lu_request == T3312_EXPIRED))
               {
                  MSG_HIGH_DS_0(MM_SUB, "=MM= Performing Pending Periodic LU procedure on service recovery");
                  mm_initiate_lu( MM_LU_CAUSE_PERIODIC );
               }
               else if ((mm_pending_lu_request == T3122_EXPIRED) &&
                        (!mm_serving_plmn.t3122_running))
               { 
                  mm_initiate_lu(mm_get_lu_cause());
               } 
#if defined(FEATURE_LTE)
               else if (((gmm_lte_deact_isr_timer_expired == TRUE )||
                          (mm_timer_status[TIMER_T3423] == TIMER_ACTIVE &&
                           emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI))
                          || (mm_pending_lu_request == LTE_IRAT_LU
                       && (mm_csfb_state == MM_CSFB_STATE_NULL)))
               {
                 MSG_HIGH_DS_3(MM_SUB,"=MM= LU started due to IRAT from LTE: gmm_lte_deact_isr_timer_expired=%d mm_pending_lu_request=%d T3423 status=%d",
                                gmm_lte_deact_isr_timer_expired, mm_pending_lu_request, mm_timer_status[TIMER_T3423]);
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

#ifdef FEATURE_ENHANCED_NW_SELECTION
               else if (reg_sim_read_ens_flag() && ((mmsim_is_ef_rat_present()) ||
                       (gmm_nv_disable_call_type & GMM_CALL_TYPE_VOICE_MASK))) 
               {
                  if (mm_force_lu)
                  {
                     MSG_HIGH_DS_0(MM_SUB,"=MM= LU required for ENS purposes-forced to do normal LU here");
                     mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                  }
               }
#endif
#ifdef FEATURE_FEMTO_CSG
               else if (!mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info))
               {
                  MSG_HIGH_DS_0(MM_SUB,"=MM= CSG: Perform LAU -CSG not in white list ");
                  mm_initiate_lu(MM_LU_CAUSE_NORMAL);
               }
#endif 
               else
               {
                 /* Since responding to REG without LU, set EPLMN list to valid again */
                 if(sys_eplmn_list_validate(TRUE))
                 {
                   MSG_HIGH_DS_0(MM_SUB, "=MM= EPLMN list is now validated");
                   mm_send_rrc_eq_plmn_list_change_ind();
                   mm_send_rr_eq_plmn_list_change_ind();  
#ifdef FEATURE_LTE
                   emm_send_multimode_rrc_system_update_from_GW(emm_ctrl_data_ptr);
#endif   
                 }

               }
			   
               if(mm_pending_lu_request != LTE_IRAT_LU)
               {
                 mm_pending_lu_request = NO_LU;
               }
               mm_lu_pended_nmo_unknown = FALSE;
            }
         }
#ifdef FEATURE_LTE   
        /*Start CSFB calls is LAI is not forbidded*/
        if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG ||
           mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)
        {
          mm_csfb_process_pending_call(); 
        }
#endif

      }
      else  /* LU requested, but LAI forbidden */
      {
#ifndef FEATURE_FEMTO_CSG
         MSG_HIGH_DS_0(MM_SUB,"=MM= PLMN or LAI is forbidden");
#else
         MSG_HIGH_DS_0(MM_SUB,"=MM= PLMN or LAI is forbidden or CSG not in white list");
#endif 
         /* Set the Serving PLMN service status to Limited */
         mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                         mm_serving_plmn.info.reg_service_domain,
                                         TRUE,
                                         SYS_SRV_STATUS_LIMITED,
                                         TRUE );

         /* Set the REG CNF plmn service state to the Serving PLMN info */
         mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;

         mm_send_mmr_reg_cnf();
         mm_reg_waiting_for_reg_cnf = FALSE;
         mm_ready_to_send_reg_cnf   = FALSE;
         mm_substate_control( MM_STD_LIMITED_SERVICE );
      }
   }
   else
   {
      mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);
      mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);
         
      MSG_HIGH_DS_0(MM_SUB,"=MM= LAI changed - but REG has said NO LU, so NO LU required");
      /* Transition to Limited service substate only if there are no AC restrictions */
      if (cs_cell_access == MM_AS_ACCESS_ALL_CALLS)
      {
         mm_substate_control( MM_STD_LIMITED_SERVICE );
      }
      else if (cs_cell_access == MM_AS_ACCESS_NO_CALLS)
      {
         if (ps_cell_access == MM_AS_ACCESS_NO_CALLS)
         {
           mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           mm_serving_plmn.info.reg_service_domain,
                                           TRUE,
                                           SYS_SRV_STATUS_NO_SRV,
                                           TRUE );
         }
         else
         {
           mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           mm_serving_plmn.info.reg_service_domain,
                                           FALSE,
                                           SYS_SRV_STATUS_NO_SRV,
                                           TRUE );         
         }         

         if (mm_reg_waiting_for_reg_cnf)
         {
            if (mm_ready_to_send_reg_cnf)
            {
               mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
               mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
               mm_send_mmr_reg_cnf();
               mm_reg_waiting_for_reg_cnf = FALSE;
               mm_ready_to_send_reg_cnf   = FALSE;
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
} /* end mm_handle_plmn_found() */
