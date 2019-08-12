/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmsend.c_v   1.26   17 Jul 2002 12:30:08   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmsend.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/14   am      Back off timer code
08/04/11   am      Replacing FEATURE_DISABLE_GSDI_INTERFACE with 
                   FEATURE_UIM_DISABLE_GSDI_INTERFACE
06/24/11   rajesh  Updated mm_send_rrc_disable_bplmn_ind function 
05/14/01   jca     Added service_type parameter to mm_send_mmr functions.
05/25/01   jca     Updated to reflect changes to REG<->MM interface.
06/03/01   sbk     Global variables are renamed to use mm_ prefix
06/08/01   jca     Reset ATT flag after sending IMSI_DETACH so we don't send multiples.
06/18/01   jca     Added function mm_send_mmr_act_cnf().
08/03/01   jca     Added parameters to functions mm_send_mmr_nreg_cnf() and
                   mm_send_mmr_service_ind().
08/15/01   ab      Rename CM to CNM.
09/18/01   sbk     Intertask primitives are logged and QXDM logging is supported
02/12/02   jca     Added svc_type parameter to MMR_SERVICE_IND.
04/24/02   jca     Modifications to reflect changes to MM-CNM interface.
05/15/02   jca     Now checking proper MM state to see if RR connection exists
                   when sending an IMSI DETACH message.
05/31/02   ks      Now starting the T3220 timer after establishing the
                   RRC connection before sending the IMSI DETACH message.
06/05/02   ks      Added support for Dual mode operation.
06/18/02   ks      Fixed memcpy in mm_send_mmr_service_ind
07/02/02   mks     Added function mm_send_rr_sim_update_req() to send SIM updates to RR
07/11/02   ks      Removed hardcoded value for mmr_reg_cnf->available_plmn_list.length
                   in mm_send_mmr_reg_cnf, removed a redundant memset.
07/12/02   vdr     Fixed memcpy in mm_send_rr_sim_update_req() for GSM GPRS Kc
07/12/02   ks      Removed the Hardcoded value for equivalent PLMN list length
                  in mm_send_mmr_reg_cnf()
07/17/02   ks     Replaced get_classmark_1 with gs_get_classmark_1() for dual
                  mode classmark information.
08/02/02   ks     Replaced the input parameter from serving_plmn_info_T to
                  mmr_service_ind_s_type.
08/11/02   mks    Added functions mm_send_rrc_sim_update_req() to send SIM
                  updates to RRC and mm_send_sim_inserted_req() to send SIM
                  insertion event to RRC
08/26/02   mks    Added function mm_send_rrc_security_update_req() to send
                  security update event to RRC
08/19/02   cd     In mm_send_mmr_sim_available_cnf(), and mm_send_mmr_sim_not_available_cnf()
                  set message length to 0.
                  In mm_send_rr_sim_update_req(), corrected copying the values of
                  gprs_ciphering_key.
10/07/02   jca    Modified function mm_send_mmr_reg_cnf().
10/09/02   vdr    Modified function mm_send_rrc_sim_inserted_req()
10/11/02   cd     Modified mm_send_mmcnm_est_rej to include passing the reject cause
01/02/03   ks     Setting the equivalent_plmn_list length to 0 in mmr_service_ind
                  primitive until the Equivalent PLMN list feature is supported.
01/10/03   jca    Changes to support CS<->PS<->CS/PS mode switching.
02/10/03   cd     Fixed Lint errors
03/07/03   vdr    Changed function mm_send_mmr_service_ind() in order to support
                  PS data continuity feature
04/03/03   cd     Now including equiv PLMN list in mmr_reg_cnf, and mmr_service_ind
04/28/03   vdr    Now global flag will be reseted only when PS services are no
                  longer suspended
05/08/03   cd     Added check for the MM state when processing a MMCNM_EST_REJ to
                  set the mm_cnm_is_active flag
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
07/14/03   cd     Added more debugging msgs for ePLMN
07/18/03   cd     Corrected print statement for ePLMN
08/29/03   cd     Added support for passing the band_preference between REG and RR
10/09/03   cd     Now sending the ePLMN list length in number of PLMNs included
                  rather than total number of octets
03/15/04   cd     Corrected msg header length in IMSI_DETACH_INDICATION
03/15/04   cd     Set mm_cnm_is_active to FALSE after sending a MMCC_REEST_REJ
05/12/04   cd     Added support for WCDMA multiband feature.
                  Fixed lint errors.
07/18/04   ks     Now setting the lai_forbidden field in the MMR_REG_CNF and
                  MMR_SERVICE_IND primitives to inform REG.
07/19/04   cd     Stopped T3230 if still running and MM is sending a MMCNM_EST_REJ
08/03/04   cd     Keeping mm_cnm_is_active set to TRUE if rejecting a request 
                  to establish an additional MM connection
10/06/04   ks     Added function mm_send_mmr_camped_ind() to send the camped_ind
                  to REG after acquiring service on requested PLMN.
12/17/04   vdr    Changed the interfaces for mm_send_rrc_sim_update_req() &
                  mm_send_rrc_security_update_req()
02/14/05   ks     Added new function mm_send_mmcnm_rel_cnf_nwdata()
03/31/05   hj     Added support for sending CM_CIPHER_IND to cm.
04/12/05   hj     Changed definition of mm_send_cm_cipher_ind to reflect new 
                  enum type for cipher_domain.
04/21/05   up     Added new function mm_send_rrc_eq_plmn_list_change_ind(),
                  to send notification of change of ePLMN list to RRC.
04/26/05   sm     Sending LAC in word format in mm_send_mmr_service_ind(),
                  mm_send_mmr_reg_cnf().
04/28/05   hj     Removing Lint Errors caused by sending LAC in word format.
05/10/05   up     Updated mm_send_mmr_reg_cnf and mm_send_mmr_service_ind
                  to send RRC_NW_SEL_MODE_RESET_IND when service status is
                  FULL SERVICE in LIMITED_ROUTINE_ACQ mode.
05/18/05  kvk     Feature wrapped the code exclusively used in WCDMA only.
05/31/05  sm      Setting CS, PS HFNs to thresholds value if they are equal or more   
                  than threshold value
05/30/05  abe     Added function mm_send_mmcnm_rel_cnf_ps_sms used by GMM to send 
                  MMCNM_REL_CNF. Added check if mm_cnm_rel_req_received set to TRUE 
                  before sending MMCNM_REL_CNF.
06/06/05   abe     Added code for CCBS support
06/21/05   up     Added mm_send_rr_eq_plmn_list_change_ind to send
                  EPLMN list change indication to RR.
08/23/05   kvk    Added function mm_send_cm_ps_data_suspend_ind() to send ps_data_suspend
                  indication to CM.
09/16/05   ajt    Added mm_send_mmr_emergency_num_list_ind to send list of 
                  Emergency numbers to REG.
01/02/06   up      Added mm_send_rr/rrc_bplmn_search_abort_req to send abort 
                   request to RR/RRC to abort BPLMN Search.
04/06/06   sn     Added mm_send_mmr_cell_service_ind
05/09/06   up     Added function mm_send_rrc_change_mode_ind.
06/15/06   nr     New arguments/causes added in mm_send_mmcc_reest_rej.
10/13/06   up    Set message_id and message_set to 0 when STOP_MODE_CNF is sent to REG.
09/15/06   sn     Send RR/C_NW_SEL_MODE_RESET_IND if got full service while
                  in LIMITED_ROUTINE_ACQ or LIMITED_MANUAL_ACQ
04/09/07   rd     SMS optimization changes are incorporated.
04/04/08   ab   Klockwork fixes
07/05/11   rajesh  Setting "mm_service_ind_blocked_due_to_cs_reg" to TRUE whenever NAS doesnot
                            send the service indication to CM/DS because of ongoing CS procedure.
23/08/11   sla   Send CELL SERVICE IND to CM only if service status is not blocked at REG
10/01/11   abhi    sending mapped/registered RAI to GRR in sim update request
10/10/11   gps    Initiating some primitives with all zeros
08/25/11   jbk   Block REG CNF/SER IND from CM if GMM is not ready
11/22/11   gps   Removed refrence for header file "gsdi_exp.h"
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "ms_cc_mm_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "com_iei_v.h"
#include "mm_mm_v.h"
#include "mm_rr.h"
#include "mm_v.h"
#include "mm_umts_v.h"
#include "libprot.h"
#include "err.h"
#include "mm_sim.h"
#include "mmauth.h"
#include "gmm_int_v.h"
#include "rrcmmif.h"
#include "mm_coord.h"
#include "sys.h"
#include "sys_v.h"
#include "cmwll.h" 
#include "bit.h"
#include "sys_eplmn_list_v.h"
#include "mmgsdisessionlib_v.h"
#include <stringl/stringl.h>
#include "tdsrrccmd.h"
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE)
#include "emm_rrc_if.h"
#endif
#ifdef FEATURE_SEGMENT_LOADING
#include "mcpm_api.h"
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#endif

#if defined(FEATURE_SGLTE_DUAL_SIM) || defined (FEATURE_SVLTE_DUAL_SIM)
#include "policyman.h"
#endif
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK
#ifdef FEATURE_LTE 
#ifdef FEATURE_DUAL_SIM
extern boolean gmm_set_drx_req_pending_sim[MAX_AS_IDS];
#define gmm_set_drx_req_pending gmm_set_drx_req_pending_sim[mm_sub_id]
#else
extern boolean gmm_set_drx_req_pending;
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1_sim[MAX_AS_IDS];
#define gmm_drx_cn_coeff_s1 gmm_drx_cn_coeff_s1_sim[mm_sub_id]
#else
extern sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1;
#endif
#endif /*FEATURE_LTE*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern boolean mm_mn_sglte_svlte_mode;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
extern ue_rev_level_indicator_type get_revision_level_indicator_per_nas_stacks
(
  sys_modem_as_id_e_type as_id 
);
#define get_revision_level_indicator() get_revision_level_indicator_per_nas_stacks((sys_modem_as_id_e_type)mm_as_id)

extern  auth_gsm_cipherkey_type     gsm_kc_128bit_sim[MAX_NAS_STACKS] ;

extern  auth_gsm_cipherkey_type     gsm_gprs_kc_128bit_sim[MAX_NAS_STACKS] ;
#else
extern ue_rev_level_indicator_type get_revision_level_indicator( void );
extern  auth_gsm_cipherkey_type     gsm_kc_bit128 ;
extern  auth_gsm_cipherkey_type     gsm_gprs_kc_bit128 ;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern byte mm_last_paging_cause_sim[MAX_NAS_STACKS];

#define  mm_last_paging_cause mm_last_paging_cause_sim[mm_as_id]

#else
extern byte mm_last_paging_cause;
#endif /* FEATURE_DUAL_SIM */

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

extern sys_cell_access_status_e_type  mm_convert_to_reg_access_info
(
mm_as_cell_access_status_T mm_cell_access_status
);
extern void gmm_handle_gprs_detach_low_level_failure( void );
extern byte gs_get_classmark_1_per_nas_stacks
(
  classmark_params_T    *params_ptrs,
  sys_modem_as_id_e_type stack_id_nas
);


#if defined(FEATURE_DUAL_SIM) && !defined(FEATURE_LTE)
extern sys_mode_pref_e_type mm_sim_mode_pref_sim[MAX_NAS_STACKS];
#define  mm_sim_mode_pref mm_sim_mode_pref_sim[mm_as_id]

extern boolean gmm_force_local_detach_sim[MAX_NAS_STACKS];
#endif


#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)

boolean mm_is_gsm_allowed_on_sub( void )
{
  boolean is_gsm_sub = FALSE;
#ifndef FEATURE_LTE
 if((mm_sim_mode_pref == SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED) ||
     (mm_sim_mode_pref == SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED) ||
     (mm_sim_mode_pref == SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY))
  {
    is_gsm_sub = TRUE;
  }
  else
  {
    is_gsm_sub = FALSE;
  }
#else
  #ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE)
  {
    if(mm_as_id == mm_cs_stack_id)
    {
      is_gsm_sub = TRUE;
    }
    else
    {
      is_gsm_sub = FALSE;
    }
  }
  else
  #endif
  if((mm_sub_capability == SUBS_CAPABILITY_MULTIMODE) ||
     (mm_sub_capability == SUBS_CAPABILITY_GSM_ONLY))
  {
    is_gsm_sub = TRUE;
  }
#endif
  return is_gsm_sub;
}

boolean mm_is_umts_allowed_on_sub( void )
{
  boolean is_wcdma_sub = FALSE;
#ifndef FEATURE_LTE
  if((mm_sim_mode_pref == SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED) ||
     (mm_sim_mode_pref == SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED) ||
     (mm_sim_mode_pref == SYS_MODE_PREF_TDS_RADIO_ACCESS_ONLY) ||
     (mm_sim_mode_pref == SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY))
  {
    is_wcdma_sub = TRUE;
  }
  else
  {
    is_wcdma_sub = FALSE;
  }
#else
  #ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE)
  {
    if(mm_as_id == mm_ps_stack_id)
    {
      is_wcdma_sub = TRUE;
    }
    else
    {
      is_wcdma_sub = FALSE;
    }
  }
  else
  #endif
  if(mm_sub_capability == SUBS_CAPABILITY_MULTIMODE)
  {
    is_wcdma_sub = TRUE;
  }
#endif
  return is_wcdma_sub;
}
#endif

/*===========================================================================

FUNCTION mm_is_multimode_sub

DESCRIPTION
  This function checks whether one of LTE or UMTS or TD rats are allowed on this subscription

DEPENDENCIES
  None

RETURN VALUE
  TRUE if multimode sub

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_is_multimode_sub()
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  boolean is_multimode_sub = FALSE;
#ifndef FEATURE_LTE
  if((mm_sim_mode_pref == SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED) ||
     (mm_sim_mode_pref == SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED) ||
     (mm_sim_mode_pref == SYS_MODE_PREF_TDS_RADIO_ACCESS_ONLY) ||
     (mm_sim_mode_pref == SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY))
  {
    is_multimode_sub = TRUE;
  }
#else
  #ifdef FEATURE_SGLTE
  if((MM_SUB_IS_SGLTE) 
#ifdef FEATURE_DUAL_SIM
    && (mm_sub_capability == SUBS_CAPABILITY_MULTIMODE)
#endif
    )
  {
    if((mm_as_id == mm_ps_stack_id) ||
       !MM_IS_IN_SGLTE_MODE)
    {
      is_multimode_sub = TRUE;
    }
  }
  else
  #endif
  {
    if(mm_sub_capability == SUBS_CAPABILITY_MULTIMODE)
    {
      is_multimode_sub = TRUE;
    }
  }
#endif
  return is_multimode_sub;
#else
  return TRUE;
#endif
}

/*===========================================================================

FUNCTION mm_send_rrc_bplmn_search_abort_req

DESCRIPTION
  This function sends the abort request to RRC to abort the Manual BPLMN search.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_bplmn_search_abort_req()
{
#ifdef FEATURE_WCDMA
   rrc_cmd_type *rrc_cmd = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );

   if (rrc_cmd == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_BPLMN_SEARCH_ABORT_REQ", 0, 0, 0);
   }
   else
   {
      rrc_cmd->cmd_hdr.cmd_id = RRC_BPLMN_SEARCH_ABORT_REQ;

      send_cmd_to_rrc( rrc_cmd, mm_serving_plmn.info.active_rat );
   }
#ifdef FEATURE_DUAL_SIM
   mm_send_rr_ps_resume_ind();
#endif
   mm_plmn_search_in_progress = FALSE;
   mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_NONE;
#endif /* FEATURE_WCDMA */

} /* end mm_send_rrc_bplmn_search_abort_req() */


/*===========================================================================

FUNCTION mm_send_rr_bplmn_search_abort_req

DESCRIPTION
  This function send the abort request to RRC to abort the Manual BPLMN search.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_bplmn_search_abort_req()
{
   rr_bplmn_search_abort_req_T rr_bplmn_search_abort_req;
   memset(&rr_bplmn_search_abort_req,0x00,sizeof(rr_bplmn_search_abort_req_T));
   rr_bplmn_search_abort_req.message_header.message_set = MS_MM_RR;
   rr_bplmn_search_abort_req.message_header.message_id  = (int)RR_BPLMN_SEARCH_ABORT_REQ;
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof( rr_bplmn_search_abort_req_T ) - sizeof(IMH_T),
                 &rr_bplmn_search_abort_req.message_header );
   /*lint -restore */
   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RR_BPLMN_SEARCH_ABORT_REQ");

   mm_send_message( (IMH_T *)&rr_bplmn_search_abort_req, GS_QUEUE_RR );
#ifdef FEATURE_DUAL_SIM
   mm_send_rr_ps_resume_ind();
#endif
   mm_plmn_search_in_progress = FALSE;
   mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_NONE;   
} /* end mm_send_rr_bplmn_search_abort_req() */

#ifdef FEATURE_DUAL_SIM
/*************************************************************
FUNCTION mm_send_rr_ps_resume_ind()

DESCRIPTION
  This function checks if UE should resume ps on RR 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 *************************************************************/
void mm_send_rr_ps_resume_ind()
{
  mm_as_id_e_type      mm_other_as_id, mm_tmp_as_id;
  mm_tmp_as_id = mm_as_id;

  if (MMCNM_IS_MODE_DUAL_ACTIVE && (mm_mn_sglte_svlte_mode == FALSE))
  {
    return;
  }
  for( mm_other_as_id = MM_AS_ID_1; mm_other_as_id < (mm_as_id_e_type)MAX_NAS_STACKS ; mm_other_as_id++ )
  {
    if( mm_other_as_id != mm_tmp_as_id )
    {
      mm_as_id = mm_other_as_id;
      mm_sub_id = mm_sub_id_stack[mm_as_id];
      if((mm_conn_mode_manual_search_enabled == TRUE) &&
         (mm_plmn_search_in_progress_sim[mm_tmp_as_id] == TRUE) &&
         (mm_waiting_for_list_cnf_type_sim[mm_tmp_as_id] == SYS_NETWORK_SELECTION_MODE_MANUAL) &&
         ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) ||
          (mm_serving_plmn.ms_op_mode ==  SYS_SRV_DOMAIN_PS_ONLY) ||
          ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY) && 
            (mm_serv_change_pending == TRUE) && 
            (gmm_gprs_detach_pending == TRUE))))
      {
        rr_ps_resume_ind_T     rr_ps_resume_ind;
        rr_ps_resume_ind.message_header.message_set = MS_MM_RR;
        rr_ps_resume_ind.message_header.message_id  = (int) RR_PS_RESUME_IND;
        PUT_IMH_LEN( sizeof( rr_ps_resume_ind_T ) - sizeof(IMH_T),
                     &rr_ps_resume_ind.message_header);
        MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RR_PS_RESUME_IND");
        mm_send_message( &rr_ps_resume_ind.message_header, GS_QUEUE_RR );
      }
    }
  }
  mm_as_id = mm_tmp_as_id;
  mm_sub_id = mm_sub_id_stack[mm_as_id];
}

/*************************************************************
FUNCTION mm_send_rr_ps_abort_req()

DESCRIPTION
  This function checks if UE should abort ps on RR 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 *************************************************************/
void mm_send_rr_ps_abort_req(rr_ps_abort_cause_e_type cause)
{
  rr_ps_abort_req_T    rr_ps_abort_req;
  MSG_HIGH_DS_0(MM_SUB,"=MM= Received Manual PLMN search req,send PS abort req to GSM");
  rr_ps_abort_req.message_header.message_set = MS_MM_RR;
  rr_ps_abort_req.message_header.message_id  = (int) RR_PS_ABORT_REQ;
  PUT_IMH_LEN( sizeof( rr_ps_abort_req_T ) - sizeof(IMH_T),
                 &rr_ps_abort_req.message_header);

  rr_ps_abort_req.abort_cause = MM_RR_PS_ABORT_MANUAL_PLMN_SEARCH;

  MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RR_PS_ABORT_REQ");
  mm_send_message( &rr_ps_abort_req.message_header, GS_QUEUE_RR );
}
#endif

/*===========================================================================

FUNCTION MM_SEND_MMR_EMERGENCY_NUM_LIST_IND

DESCRIPTION
  This function notifies emergency numbers to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_emergency_num_list_ind(
       mmr_emergency_num_list_s_type* emergency_number_list)
{
   ue_rev_level_indicator_type revision_level = get_revision_level_indicator();
   if ((revision_level == UE_REV_REL99) && (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL5))
   {
     mmr_emergency_num_list_ind_s_type mmr_emergency_number_list_ind;
    
     mmr_emergency_number_list_ind.message_header.message_set = MS_MM_REG;
     mmr_emergency_number_list_ind.message_header.message_id = (int) MMR_EMERGENCY_NUM_LIST_IND;
    
     memscpy(&(mmr_emergency_number_list_ind.emergency_number_list), sizeof(mmr_emergency_num_list_s_type),
             emergency_number_list, sizeof(mmr_emergency_num_list_s_type));
    
     PUT_IMH_LEN( sizeof(mmr_emergency_num_list_ind_s_type) - sizeof(IMH_T),
                    & (mmr_emergency_number_list_ind.message_header) );
                                       
     MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent MMR_EMERGENCY_NUM_LIST_IND. List Length: %d", 
       mmr_emergency_number_list_ind.emergency_number_list.emergency_number_count );
       
     mm_send_message( (IMH_T *)&mmr_emergency_number_list_ind, GS_QUEUE_REG);
   }
   else
   {
     MSG_HIGH_DS_0(MM_SUB, "=MM= UE is Pre R5");
   }     
} /* end mm_send_mmr_emergency_num_list_ind() */

/*===========================================================================

FUNCTION RRC_EQ_PLMN_LIST_CHANGE_IND

DESCRIPTION
  This function notifies ePLMN changes to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_eq_plmn_list_change_ind()
{
#ifdef FEATURE_WCDMA
   if (mm_is_multimode_sub())
   {
     if (mm_wcdma_supported)
     {
       rrc_cmd_type *rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );

       if (rrc_cmd == NULL)
       {
          MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_EQ_PLMN_LIST_CHANGE_IND", 0, 0, 0);
       }
       else
       {
          rrc_cmd->cmd_hdr.cmd_id = RRC_EQ_PLMN_LIST_CHANGE_IND;
          send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
       }
     }

     if (mm_tdscdma_supported)
     {
       rrc_cmd_type *rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );

       if (rrc_cmd == NULL)
       {
          MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending TDSRRC_EQ_PLMN_LIST_CHANGE_IND", 0, 0, 0);
       }
       else
       {
          rrc_cmd->cmd_hdr.cmd_id = RRC_EQ_PLMN_LIST_CHANGE_IND;
          send_cmd_to_rrc( rrc_cmd, SYS_RAT_TDS_RADIO_ACCESS );
       }
     }
   }
#endif
} /* end mm_send_rrc_eq_plmn_list_change_ind() */

/*===========================================================================

FUNCTION MM_SEND_RR_EQ_PLMN_LIST_CHANGE_IND

DESCRIPTION
  This function sends a rr_eq_plmn_list_change_ind to rr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_eq_plmn_list_change_ind( void )
{
#if (defined(FEATURE_DUAL_SIM) && !defined(FEATURE_TWO_3GPP_SIM)) || defined(FEATURE_SGLTE)
   if (mm_is_gsm_allowed_on_sub())
#endif
   {
   rr_eq_plmn_list_change_ind_T rr_eq_plmn_list_change_ind;
   memset(&rr_eq_plmn_list_change_ind,0x00,sizeof(rr_eq_plmn_list_change_ind_T));
   rr_eq_plmn_list_change_ind.message_header.message_set = MS_MM_RR;
   rr_eq_plmn_list_change_ind.message_header.message_id  = (int)RR_EQ_PLMN_LIST_CHANGE_IND;
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof( rr_eq_plmn_list_change_ind_T ) - sizeof(IMH_T),
                 &rr_eq_plmn_list_change_ind.message_header );
   /*lint -restore */
   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RR_EQ_PLMN_LIST_CHANGE_IND");

   mm_send_message( (IMH_T *)&rr_eq_plmn_list_change_ind, GS_QUEUE_RR );
   }
}
/*===========================================================================

FUNCTION MM_SEND_RR_CHANGE_MODE_IND

DESCRIPTION
  This function sends a rr_change_mode_ind to rr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_change_mode_ind( mm_cmd_type *message )
{
#ifdef FEATURE_NAS_MODE_CHANGE
   rr_change_mode_ind_T rr_change_mode_ind;

   rr_change_mode_ind.message_header.message_set = MS_MM_RR;
   rr_change_mode_ind.message_header.message_id  = (int)RR_CHANGE_MODE_IND;
   
#ifdef FEATURE_RAT_PRIORITY_LIST
   memscpy(&rr_change_mode_ind.rat_pri_list_info, sizeof(sys_rat_pri_list_info_s_type),
          &message->cmd.mmr_reg_req.rat_pri_list_info,
          sizeof(sys_rat_pri_list_info_s_type));
   memscpy(&mm_rat_pri_list_info, sizeof(sys_rat_pri_list_info_s_type), &message->cmd.mmr_reg_req.rat_pri_list_info,
          sizeof(sys_rat_pri_list_info_s_type));
#else
   rr_change_mode_ind.gsm_only = !message->cmd.mmr_reg_req.dual_mode;
#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
   mm_ue_band_class_pref = message->cmd.mmr_reg_req.band_pref;
  /* Set the Band */
   rr_change_mode_ind.band_pref  = get_preferred_band_class();
#endif  
   mm_dual_mode                           = message->cmd.mmr_reg_req.dual_mode;
   mm_band_pref                           = message->cmd.mmr_reg_req.band_pref;
#endif
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof( rr_change_mode_ind ) - sizeof(IMH_T),
                 &rr_change_mode_ind.message_header );
   /*lint -restore */
   MSG_HIGH_DS_0(MM_SUB," =MM= MM sent RR_CHANGE_MODE_IND");

   mm_send_message( &rr_change_mode_ind.message_header, GS_QUEUE_RR );
#else
#ifdef FEATURE_RAT_PRIORITY_LIST
   memscpy(&mm_rat_pri_list_info, sizeof(sys_rat_pri_list_info_s_type), &message->cmd.mmr_reg_req.rat_pri_list_info,
          sizeof(sys_rat_pri_list_info_s_type));  
#endif
#endif
}

#ifdef FEATURE_PREFERRED_MODE_CHANGE
/*===========================================================================

FUNCTION MM_SEND_RRC_CHANGE_MODE_IND

DESCRIPTION
  This function sends RRC_CHANGE_MODE_IND to RRC when user mode
  preference is changed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_change_mode_ind( mm_cmd_type *message )
{
#ifdef FEATURE_WCDMA
   rrc_cmd_type          *rrc_cmd;
#endif
   /*
   * Send the RRC_CHANGE_MODE_IND to RRC.
   */
#ifdef FEATURE_SEGMENT_LOADING
    if ((!mm_wcdma_supported) && 
        (mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS))
    {
         /* W not loaded */
         return;
    }
    if ((!mm_tdscdma_supported) && 
        (mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS))
    {
           /* T not loaded */
        return;
    }
#endif /* FEATURE_SEGMENT_LOADING */

#ifdef FEATURE_WCDMA
   rrc_cmd = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );

   if (rrc_cmd == NULL)
   {
     MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }

   rrc_cmd->cmd_hdr.cmd_id = RRC_CHANGE_MODE_IND;
#ifndef FEATURE_RAT_PRIORITY_LIST
#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
  mm_ue_band_class_pref = message->cmd.mmr_reg_req.band_pref;
  /* Set the Band */
  rrc_cmd->cmd.rrc_change_mode_ind.band_pref = get_preferred_band_class();
#endif
      /* Set the dual mode flag */
   rrc_cmd->cmd.rrc_change_mode_ind.dual_mode = message->cmd.mmr_reg_req.dual_mode;
   mm_dual_mode                           = message->cmd.mmr_reg_req.dual_mode;
   mm_band_pref                           = message->cmd.mmr_reg_req.band_pref;
#else
   memscpy(&rrc_cmd->cmd.rrc_change_mode_ind.rat_pri_list_info,sizeof(sys_rat_pri_list_info_s_type),
          &message->cmd.mmr_reg_req.rat_pri_list_info,
          sizeof(sys_rat_pri_list_info_s_type));

   /* The following two lines have to be removed once code changes for
    * rat priority list in RRC layer is completed */
   rrc_cmd->cmd.rrc_change_mode_ind.band_pref = message->cmd.mmr_reg_req.band_pref;
   rrc_cmd->cmd.rrc_change_mode_ind.dual_mode = message->cmd.mmr_reg_req.dual_mode;

   memscpy(&mm_rat_pri_list_info, sizeof(sys_rat_pri_list_info_s_type), &message->cmd.mmr_reg_req.rat_pri_list_info,
          sizeof(sys_rat_pri_list_info_s_type));
#endif /* FEATURE_RAT_PRIORITY_LIST */
   /* Put the command in the RRC queue */
   send_cmd_to_rrc( rrc_cmd, mm_serving_plmn.info.active_rat );
#endif
}
#endif

/*===========================================================================

FUNCTION MM_SEND_STATUS

DESCRIPTION
  This function sends an MM-STATUS message to the network

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_status( byte reject_cause )
{
   mm_status_msg_T   mm_status;
   dword ota_msg_length = 2; /* Start with PD and Type */

   memset(&mm_status, 0x00, sizeof(mm_status_msg_T));
   mm_status.msg_head.IMH.message_set = MS_MM_RR;
   mm_status.msg_head.IMH.message_id  = (int)RR_DATA_REQ;
   mm_status.msg_head.skip_pd = PD_MM;
   mm_status.msg_head.type = MM_STATUS;

   /* Set Reject cuase */
   mm_status.reject_cause = reject_cause;
   ota_msg_length++;

   MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent MM_STATUS");

   /* Send OTA message */
   mm_send_ota_message( (byte *)&mm_status, ota_msg_length);
}


/*===========================================================================

FUNCTION MM_SEND_MMCNM_EST_REJ

DESCRIPTION
  This function sends a MMCNM_EST_REJ primitive to the CNM layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
void mm_send_mmcnm_est_rej( reject_type_enum_T cause_type, reject_cause_T cause_value , mmcc_domain_name_T domain)
#else
void mm_send_mmcnm_est_rej( reject_type_enum_T cause_type, reject_cause_T cause_value )
#endif
{
   mmcnm_est_rej_T mmcnm_est_rej;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   mmcnm_est_rej.header.message_set = MS_CC_MM;
   mmcnm_est_rej.header.message_id  = MMCNM_EST_REJ;
   mmcnm_est_rej.cause_type = cause_type;
   mmcnm_est_rej.cause_value = cause_value;
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
   mmcnm_est_rej.domain = domain;
#endif

   PUT_IMH_LEN( sizeof(mmcnm_est_rej_T) - sizeof(IMH_T), &mmcnm_est_rej.header );

   MSG_HIGH_DS_2( MM_SUB, "=MM= MM sent MMCNM_EST_REJ, type %d, value %d", cause_type,cause_value);

   mm_send_message( (IMH_T *)&mmcnm_est_rej, GS_QUEUE_MN_CM );

   /*
   ** If MM is in CONNECTION_ACTIVE or WF_NW_CMD state,  do not clear the mm_cnm_is_active flag
   ** since we have another MM connection active
   */
   if ((mm_state != MM_CONNECTION_ACTIVE) && 
       (mm_state != MM_WAIT_FOR_NETWORK_COMMAND) &&
       (mm_state != MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION))
   {
     mm_cnm_is_active = FALSE;
   }

   mm_sms_is_active = FALSE;
   
   /* ----------------------------------------------
   ** Flag that the CNM request is no longer pending
   ** ---------------------------------------------- */
   mm_held_cnm_service_req.present = FALSE;
   mm_follow_on_request = FALSE;

   /* -------------------------------------
   ** Stop T3230 if it is running
   ** ------------------------------------ */

   mm_stop_timer( TIMER_T3230 );
}


/*===========================================================================

FUNCTION MM_SEND_MMCNM_REL_IND

DESCRIPTION
  This function sends a mmcnm_rel_ind message to the cm

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_mmcnm_rel_ind
(
   byte    reject_cause_value,
   boolean reestablishment_possible
)
{
   mmcnm_rel_ind_T mmcnm_rel_ind;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   memset(&mmcnm_rel_ind,0x00,sizeof(mmcnm_rel_ind_T));
   mmcnm_rel_ind.header.message_set = MS_CC_MM;
   mmcnm_rel_ind.header.message_id  = MMCNM_REL_IND;

   mmcnm_rel_ind.rel_cause      = reject_cause_value;
   mmcnm_rel_ind.reest_possible = reestablishment_possible;

   PUT_IMH_LEN( sizeof(mmcnm_rel_ind_T) - sizeof(IMH_T),
                &mmcnm_rel_ind.header );

   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMCNM_REL_IND");
   mm_connection_type = NO_MM_CONNECTION;
   mm_send_message( (IMH_T *)&mmcnm_rel_ind, GS_QUEUE_MN_CM );

   /* ----------------------------------------------
   ** Flag that the CNM request is no longer pending
   ** ---------------------------------------------- */
   mm_cnm_is_active = FALSE;
   mm_sms_is_active = FALSE;
   mm_held_cnm_service_req.present = FALSE;
   mm_follow_on_request = FALSE;

   /* Reset the CNM service establish cause */
   mm_cnm_srv_est_cause = MM_RR_NO_CNM_SERVICE;

   if ((mm_psm_ready_req_rejected == TRUE) &&
       (mm_check_ready_for_psm() == SYS_PSM_STATUS_READY))
   {
     mm_send_mmr_psm_ready_ind();
   }
}


/*===========================================================================

FUNCTION MM_SEND_MMCNM_REL_CNF

DESCRIPTION
  This function sends a MMCNM_REL_CNF primitive to the CNM layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_mmcnm_rel_cnf( void )
{
   mmcnm_rel_cnf_T mmcnm_rel_cnf;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (mm_cnm_rel_req_received == TRUE )
  {
    mmcnm_rel_cnf.header.message_set = MS_CC_MM;
    mmcnm_rel_cnf.header.message_id  = MMCNM_REL_CNF;

    PUT_IMH_LEN( sizeof(mmcnm_rel_cnf_T) - sizeof(IMH_T) , &mmcnm_rel_cnf.header );

    mmcnm_rel_cnf.mm_conn_active_nw_data_recd = FALSE;

    MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent MMCNM_REL_CNF");

    mm_send_message( (IMH_T *)&mmcnm_rel_cnf, GS_QUEUE_MN_CM );
    mm_connection_type = NO_MM_CONNECTION;
    mm_cnm_rel_req_received = FALSE;
  }

  /* ----------------------------------------------
  ** Flag that the CNM request is no longer pending
  ** ---------------------------------------------- */
  mm_cnm_is_active = FALSE;
  mm_held_cnm_service_req.present = FALSE;
  mm_follow_on_request = FALSE;
  mm_sms_is_active = FALSE;

  /* Reset the CNM service establish cause */
  mm_cnm_srv_est_cause = MM_RR_NO_CNM_SERVICE;
}

#ifdef FEATURE_CCBS 
/*===========================================================================

FUNCTION MM_SEND_MMCNM_PROMPT_IND

DESCRIPTION
  This function sends a MMCNM_PROMPT_IND primitive to the CNM layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_mmcnm_prompt_ind( byte pd_sapi )
{
   mmcnm_prompt_ind_T mmcnm_prompt_ind;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   mmcnm_prompt_ind.header.message_set = MS_CC_MM;
   mmcnm_prompt_ind.header.message_id  = MMCNM_PROMPT_IND;
   mmcnm_prompt_ind.pd_sapi = pd_sapi;
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof(mmcnm_prompt_ind_T) - sizeof(IMH_T), &mmcnm_prompt_ind.header );
   /*lint -restore */
   MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent MMCNM_PROMPT_IND, pd_sapi = %d", pd_sapi);

   mm_send_message( (IMH_T *)&mmcnm_prompt_ind, GS_QUEUE_MN_CM );
}


#endif /* FETAURE_CCBS */

/*===========================================================================

FUNCTION MM_SEND_MMCNM_REL_CNF_NWDATA

DESCRIPTION
  This function sends a MMCNM_REL_CNF message to the CNM layer, with 
  the "message_len_lsb" set to TRUE to signal CNM that MM connection
  had been released for the previous call, but MM transitioned state 6 
  from state 9 and has ACTIVE CONNECTION, upon receiving a NW data ind.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_mmcnm_rel_cnf_nwdata( void )
{
   mmcnm_rel_cnf_T mmcnm_rel_cnf;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if (mm_cnm_rel_req_received == TRUE )
   {
     mmcnm_rel_cnf.header.message_set = MS_CC_MM;
     mmcnm_rel_cnf.header.message_id  = MMCNM_REL_CNF;

     PUT_IMH_LEN( sizeof(mmcnm_rel_cnf_T) - sizeof(IMH_T), &mmcnm_rel_cnf.header );

     mmcnm_rel_cnf.mm_conn_active_nw_data_recd = TRUE;

     /*lint -restore */
     MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent MMCNM_REL_CNF");

     mm_send_message( (IMH_T *)&mmcnm_rel_cnf, GS_QUEUE_MN_CM );
     mm_cnm_rel_req_received = FALSE;
   }

   /* ----------------------------------------------
   ** Flag that the CNM request is no longer pending
   ** ---------------------------------------------- */
   mm_cnm_is_active = FALSE;
   mm_held_cnm_service_req.present = FALSE;
   mm_follow_on_request = FALSE;
}


/*===========================================================================

FUNCTION MM_SEND_MMCNM_REL_CNF_PS_SMS

DESCRIPTION
  This function sends a MMCNM_REL_CNF primitive to the CNM layer if necessary

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_mmcnm_rel_cnf_ps_sms( void )
{
   mmcnm_rel_cnf_T mmcnm_rel_cnf;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   /* If MMCNM_REL_CNF is sent then don't send it again
   */
   if (mm_cnm_rel_req_received == TRUE )
   {  
     mmcnm_rel_cnf.header.message_set = MS_CC_MM;
     mmcnm_rel_cnf.header.message_id  = MMCNM_REL_CNF;
     /*lint -save -e778 */
     PUT_IMH_LEN( sizeof(mmcnm_rel_cnf_T) - sizeof(IMH_T), &mmcnm_rel_cnf.header );
     /*lint -restore */
     MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent MMCNM_REL_CNF");

     mm_send_message((IMH_T *) &mmcnm_rel_cnf, GS_QUEUE_MN_CM );
     mm_cnm_rel_req_received = FALSE;
   }
  
   /* ----------------------------------------------
   ** Flag that the CNM request is no longer pending
   ** ---------------------------------------------- */
   mm_sms_is_active = FALSE;
   
   if ((mm_psm_ready_req_rejected == TRUE) &&
       (mm_check_ready_for_psm() == SYS_PSM_STATUS_READY))
   {
     mm_send_mmr_psm_ready_ind();
   }
}

/*===========================================================================

FUNCTION MM_SEND_MMCC_REEST_CNF

DESCRIPTION
  This function sends a MMCC_REEST_CNF message to the CNM layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_mmcc_reest_cnf( void )
{
   mmcc_reest_cnf_T  mmcc_reest_cnf;

   mmcc_reest_cnf.header.message_set = MS_CC_MM;
   mmcc_reest_cnf.header.message_id  = MMCC_REEST_CNF;
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof(mmcc_reest_cnf_T) - sizeof(IMH_T),
                  &mmcc_reest_cnf.header );
   /*lint -restore */
   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMCC_REEST_CNF");
   mm_send_message( (IMH_T *)&mmcc_reest_cnf, GS_QUEUE_MN_CM );
}


/*===========================================================================

FUNCTION MM_SEND_MMCC_REEST_REJ

DESCRIPTION
  This function sends a MMCC_REEST_REJ message to the CNM layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_mmcc_reest_rej( reject_type_enum_T cause_type, reject_cause_T cause_value)
{
   mmcc_reest_rej_T  mmcc_reest_rej;

   mmcc_reest_rej.header.message_set = MS_CC_MM;
   mmcc_reest_rej.header.message_id  = MMCC_REEST_REJ;
   mmcc_reest_rej.cause_type = cause_type;
   mmcc_reest_rej.cause_value = cause_value;
   
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof(mmcc_reest_rej_T) - sizeof(IMH_T), &mmcc_reest_rej.header );
   /*lint -restore */
   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMCC_REEST_REJ");

   mm_send_message( (IMH_T *)&mmcc_reest_rej, GS_QUEUE_MN_CM );

   mm_cnm_is_active = FALSE;
}

/*===========================================================================

FUNCTION MM_SEND_MMCC_SIG_CONN_REL_IND

DESCRIPTION
  This function sends a MMCC_SIG_CONN_REL_IND primitive to the CNM layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_mmcc_sig_conn_rel_ind( void )
{
   mmcc_sig_conn_rel_ind_T mmcc_sig_conn_rel_ind;

   mmcc_sig_conn_rel_ind.header.message_set = MS_CC_MM;
   mmcc_sig_conn_rel_ind.header.message_id  = MMCC_SIG_CONN_REL_IND;

   PUT_IMH_LEN( sizeof(mmcc_sig_conn_rel_ind) - sizeof(IMH_T), &mmcc_sig_conn_rel_ind.header );

   MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent MMCC_SIG_CONN_REL_IND");

   mm_send_message( &mmcc_sig_conn_rel_ind.header, GS_QUEUE_MN_CM );
}


/*===========================================================================

FUNCTION MM_SEND_MMR_STOP_MODE_CNF

DESCRIPTION
  This function sends a mm_send_mmr_stop_mode_cnf message to the REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_stop_mode_cnf( void )
{
   mmr_stop_mode_cnf_s_type  mmr_stop_mode_cnf;

   mmr_stop_mode_cnf.message_header.message_set = MS_MM_REG;
   mmr_stop_mode_cnf.message_header.message_id  = (int)MMR_STOP_MODE_CNF;
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof( mmr_stop_mode_cnf_s_type) - sizeof(IMH_T),
      &mmr_stop_mode_cnf.message_header );
   /*lint -restore */
   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMR_STOP_MODE_CNF");
   mm_send_message( (IMH_T *)&mmr_stop_mode_cnf, GS_QUEUE_REG );
}

/*===========================================================================

FUNCTION MM_SEND_MMR_PLMN_SEARCH_CNF

DESCRIPTION
  This function sends a mm_send_mmr_plmn_search_cnf message to the REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_plmn_search_cnf( mmr_plmn_search_cnf_s_type *plmn_search_cnf )
{
   plmn_search_cnf->message_header.message_set = MS_MM_REG;
   plmn_search_cnf->message_header.message_id  = (int)MMR_PLMN_SEARCH_CNF;

   PUT_IMH_LEN( sizeof( mmr_plmn_search_cnf_s_type ) - sizeof(IMH_T),
                            &plmn_search_cnf->message_header );

   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMR_PLMN_SEARCH_CNF");
   if(plmn_search_cnf->service_search != SYS_SERVICE_SEARCH_PCI_PLMN)
   {
#ifdef FEATURE_DUAL_SIM
    nasdiag_plmn_list_event(&(plmn_search_cnf->found_plmn_list.plmn_list), (sys_modem_as_id_e_type)mm_sub_id);
#else
    nasdiag_plmn_list_event(&(plmn_search_cnf->found_plmn_list.plmn_list), 0);
#endif
   }  
   mm_send_message( (IMH_T *)plmn_search_cnf, GS_QUEUE_REG );
}

/*===========================================================================

FUNCTION MM_SEND_MMR_CAMPED_IND

DESCRIPTION
  This function sends a mmr_camped_ind to REG upon successful camping on a
  requested PLMN that provides service. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_camped_ind( boolean policy_change_pending )
{
  mmr_camped_ind_s_type camped_ind;

  camped_ind.message_header.message_set = MS_MM_REG;
  camped_ind.message_header.message_id  = (int)MMR_CAMPED_IND;

  camped_ind.active_rat = mm_serving_plmn.info.active_rat;
  camped_ind.plmn = mm_serving_plmn.info.plmn;
  camped_ind.lac_id = b_unpackw(mm_serving_plmn.lac.lac,0,16);

#if defined(FEATURE_LTE)
 emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif
  camped_ind.policy_change_pending = policy_change_pending;
#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
  if (camped_ind.active_rat == SYS_RAT_TDS_RADIO_ACCESS)
  {
    camped_ind.active_band.tds_active_band = get_active_band();
    MSG_HIGH_DS_2(MM_SUB, "=MM= MM sent MMR_CAMPED_IND  RAT : %d, BAND : %d",camped_ind.active_rat,
              camped_ind.active_band.tds_active_band);
  }
#if defined(FEATURE_LTE)
  else if (camped_ind.active_rat == SYS_RAT_LTE_RADIO_ACCESS)
  {
    camped_ind.active_band.lte_active_band = emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band;
#if (LTE_BAND_NUM == 256)
  MSG_HIGH_DS_5(MM_SUB, "MM sent MMR_CAMPED_IND  RAT : %d, LTE bands 1_64 : %u,LTE bands 65_128 : %u,LTE bands 129_192 : %u,LTE bands 193_256 : %u",
                   camped_ind.active_rat, 
                   camped_ind.active_band.lte_active_band.bits_1_64,
                   camped_ind.active_band.lte_active_band.bits_65_128,
                   camped_ind.active_band.lte_active_band.bits_129_192,
                   camped_ind.active_band.lte_active_band.bits_193_256);
#else
  MSG_HIGH_DS_2(MM_SUB, "MM sent MMR_CAMPED_IND  RAT : %d, BAND : %u",
                camped_ind.active_rat,
              camped_ind.active_band.lte_active_band);
#endif
  }
#endif
  else
  {
    camped_ind.active_band.active_band = get_active_band();
    MSG_HIGH_DS_2(MM_SUB, "=MM= MM sent MMR_CAMPED_IND  RAT : %d, BAND : %d",camped_ind.active_rat,
              camped_ind.active_band.active_band);
  }
#endif
  
  PUT_IMH_LEN( sizeof( mmr_camped_ind_s_type ) - sizeof(IMH_T),
                            &camped_ind.message_header );

  mm_send_message( (IMH_T *)&camped_ind, GS_QUEUE_REG );  
}



/*===========================================================================

FUNCTION MM_SEND_REG_ATTACH_FAILED_IND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void   mm_send_reg_attach_failed_ind(void)
{
  mmr_attach_failed_ind_s_type attach_failed_ind;

  attach_failed_ind.message_header.message_set = MS_MM_REG;
  attach_failed_ind.message_header.message_id  = (int)MMR_ATTACH_FAILED_IND;
  PUT_IMH_LEN( sizeof( mmr_attach_failed_ind_s_type ) - sizeof(IMH_T),
                            &attach_failed_ind.message_header );

  MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMR_ATTACH_FAILED_IND");
  mm_mmr_attach_ind_status = ATTACH_FAILED_IND_NOT_REQUIRED;

  mm_send_message( (IMH_T *)&attach_failed_ind, GS_QUEUE_REG );
}

/*===========================================================================

FUNCTION MM_SEND_REG_ATTACH_FAILED_IND_if_needed

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void   mm_send_reg_attach_failed_ind_if_needed(void)
{
  /*Fail Any attach Procedure for PS service if it is pending*/
  if (mm_mmr_attach_ind_status != ATTACH_FAILED_IND_NOT_REQUIRED )
  {
     mm_send_reg_attach_failed_ind();
  }
}

/*===========================================================================

FUNCTION MM_SEND_MMR_REG_REQ_IGNORED_IND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_reg_req_ignored_ind(sys_modem_as_id_e_type as_id)
{
  mmr_reg_req_ignored_ind_s_type mmr_reg_req_ignored_ind;

  mmr_reg_req_ignored_ind.message_header.message_set = MS_MM_REG;
  mmr_reg_req_ignored_ind.message_header.message_id  = (int)MMR_REG_REQ_IGNORED_IND;
  PUT_IMH_LEN( sizeof( mmr_reg_req_ignored_ind_s_type ) - sizeof(IMH_T),
                            &mmr_reg_req_ignored_ind.message_header );

  mmr_reg_req_ignored_ind.as_id = as_id;
  MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMR_REG_REQ_IGNORED_IND");

  mm_send_message( (IMH_T *)&mmr_reg_req_ignored_ind, GS_QUEUE_REG );
}


/*===========================================================================

FUNCTION MM_SEND_MMR_REG_CNF

DESCRIPTION
  This function sends a mmr_reg_cnf message to the reg

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_mmr_reg_cnf( void )
{
   unsigned int i = 0;
   boolean is_rai_present_foreground_search_list = FALSE;

   /* Update equivalent PLMN list information */
   mmcoord_mmr_reg_cnf.update_equivalent_plmn_list =
       mm_serving_plmn.update_equiv_plmn_list;

#ifdef FEATURE_LTE 
  mmcoord_mmr_reg_cnf.service_state.lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif
   if ((mm_serving_plmn.equiv_plmn_list.length > 0) &&
       (mm_serving_plmn.equiv_plmn_list.length <= (EQUIVALENT_PLMN_LIST_MAX_LENGTH * sizeof(sys_plmn_id_s_type))))
   {
     mmcoord_mmr_reg_cnf.equivalent_plmn_list.length =
            mm_serving_plmn.equiv_plmn_list.length / sizeof(sys_plmn_id_s_type);

     memscpy(mmcoord_mmr_reg_cnf.equivalent_plmn_list.plmn, SYS_PLMN_LIST_MAX_LENGTH*sizeof(sys_plmn_id_s_type),
             mm_serving_plmn.equiv_plmn_list.plmn, mm_serving_plmn.equiv_plmn_list.length);
   }
   else if (mm_serving_plmn.equiv_plmn_list.length > (EQUIVALENT_PLMN_LIST_MAX_LENGTH * sizeof(sys_plmn_id_s_type)))
   {
     MSG_ERROR_DS(MM_SUB,"Equiv PLMN list len %d too long",
        mm_serving_plmn.equiv_plmn_list.length,0,0);
   }
   if ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)&&
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.type == MMR_REG_REQ_USER_SELECTION)&&
       (mmcoord_reg_req_pending))
   {
     MSG_HIGH_DS_0(MM_SUB,"=MM= Ignoring MMR_REG_CNF.Another MMR_REG_REQ will be processed soon");
	 if(mm_wcdma_rlf_state == WCDMA_RLF_STARTED)
     {
       MSG_HIGH_DS_0(MM_SUB,"=MM= sending hard abort to WRRC");
       mm_send_rrc_hard_abort_req( mm_wrlf_serving_plmn_ptr->info.active_rat );
     }
   }
#ifdef FEATURE_LTE
   else if(emm_ctrl_data_ptr->reg_req_pended_indication == TRUE &&
           emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL &&
           emm_ctrl_data_ptr->reg_req_pending_ptr->type == MMR_REG_REQ_USER_SELECTION &&
           (!sys_plmn_match(emm_ctrl_data_ptr->reg_req_pending_ptr->plmn,mm_wrlf_serving_plmn_ptr->info.plmn)) 
          )
   {
      MSG_HIGH_DS_0(MM_SUB,"=MM= Abort RLF by sending hard abort and process manual selection");
      mm_send_rrc_hard_abort_req( mm_wrlf_serving_plmn_ptr->info.active_rat );
   }
#endif
   else
   {
     mmcoord_mmr_reg_cnf.message_header.message_set = MS_MM_REG;
     mmcoord_mmr_reg_cnf.message_header.message_id  = (int)MMR_REG_CNF;

#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
     if (is_tdscdma_mode())
     {
       mmcoord_mmr_reg_cnf.service_state.active_band.tds_active_band = get_active_band();
     }
#ifdef FEATURE_LTE 	 
     else if (is_lte_mode())
     {
       mmcoord_mmr_reg_cnf.service_state.active_band.lte_active_band = emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band;
     }
#endif
     else
     {
       mmcoord_mmr_reg_cnf.service_state.active_band.active_band = get_active_band();
     }
#endif

     if((mm_managed_roaming_enabled)
#ifdef FEATURE_ENHANCED_NW_SELECTION
       || (reg_sim_read_ens_flag())
#endif
       )
     {
       if((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
         mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) ||
         (mmcoord_mmr_reg_cnf.service_state.cs_service_status != SYS_SRV_STATUS_LIMITED &&
          mmcoord_mmr_reg_cnf.service_state.service_status != SYS_SRV_STATUS_LIMITED))
       {
         mmcoord_mmr_reg_cnf.service_state.cs_service_status = mmcoord_mmr_reg_cnf.service_state.service_status;
       }
     }
     else
     {
       mmcoord_mmr_reg_cnf.service_state.cs_service_status = mmcoord_mmr_reg_cnf.service_state.service_status;
     } 

     /* Inform REG that the LAI is forbidden */
     if ((mm_check_forbidden_national_roaming((inter_task_lai_T*)mm_serving_lai)) ||
         (mm_check_forbidden_regional_roaming((inter_task_lai_T*)mm_serving_lai)))
     {
       mmcoord_mmr_reg_cnf.lai_forbidden = TRUE;
     }
     else
     {
       mmcoord_mmr_reg_cnf.lai_forbidden = FALSE;
     }

 #ifdef FEATURE_FEMTO_CSG
     if (mm_is_csg_in_allowed_list(mmcoord_mmr_reg_cnf.service_state.plmn, mmcoord_mmr_reg_cnf.service_state.csg_info.csg_id)
        ||mm_is_csg_in_operator_list(mmcoord_mmr_reg_cnf.service_state.plmn, mmcoord_mmr_reg_cnf.service_state.csg_info.csg_id, FALSE))
     {
       mmcoord_mmr_reg_cnf.service_state.csg_in_white_list = TRUE;
     }
     else
     {
       mmcoord_mmr_reg_cnf.service_state.csg_in_white_list = FALSE;
     }
 #endif 
     mmcoord_mmr_reg_cnf.only_for_reg = mm_to_only_reg;
#ifdef FEATURE_WRLF_SYSTEM_SEL
     if(mm_wcdma_rlf_state == WCDMA_RLF_STARTED)
     {
       mmcoord_mmr_reg_cnf.only_for_reg = FALSE;
       mm_service_ind_blocked_due_to_cs_reg = FALSE;
     } else
#endif
     if( (mm_state == MM_LOCATION_UPDATE_INITIATED) ||
          (mm_state == MM_LOCATION_UPDATE_REJECTED) ||
          (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) ||
          ((mm_state == MM_IDLE) &&
           ((mm_timer_status[TIMER_T3211]  == TIMER_ACTIVE ||
              mm_timer_status[TIMER_T3213]  == TIMER_ACTIVE) &&
               (!(mm_idle_substate == MM_LIMITED_SERVICE ||
                 mm_idle_substate == MM_NO_CELL_AVAILABLE))) &&
           (mm_csfb_service_ind_status == MM_CSFB_SEND_SERVICE_STATUS_NONE) &&
           ((mm_serving_plmn.ms_op_mode ==SYS_SRV_DOMAIN_CS_ONLY ) ||
            (mm_serving_plmn.ms_op_mode ==SYS_SRV_DOMAIN_CS_PS)) &&
           (mm_serving_plmn.info.service_status != SYS_SRV_STATUS_NO_SRV )))
     {
        mmcoord_mmr_reg_cnf.only_for_reg = TRUE;
        mm_service_ind_blocked_due_to_cs_reg = TRUE;
     }
     else
     {
       mm_service_ind_blocked_due_to_cs_reg = FALSE;
       
#ifdef FEATURE_ENHANCED_NW_SELECTION
       if(gmm_is_rai_in_foreground_search_lai_rai_timer_list((byte*)&gmm_current_lai,gmm_current_rac))
       {
         is_rai_present_foreground_search_list = TRUE;
       }     
#endif
       /* 
       ** block service update to CM from GMM
       ** if 
       **  - Attach or RAU pending or initiated (guard timers are running)
       ** - Attach or RAU retry pending and not [OOS or Limited srv or PS only service status and 3311 is running for combined procedure]
       ** but not if
       ** - CS only service update / CS is updated
       ** - Remain on PLMN or PLMN not allowed cause whic have some actions at CM
       */
       if(((mm_state != MM_CONNECTION_ACTIVE) &&
              (mm_state != MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) &&
              (mm_state != MM_WAIT_FOR_RR_CONNECTION_MM)) &&
          (mmcoord_mmr_reg_cnf.service_state.reg_service_domain != SYS_SRV_DOMAIN_CS_ONLY) &&
          !((mm_location_information.location_update_status == UPDATED) &&
             (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)) &&
          (mmcoord_mmr_reg_cnf.cause != MMR_CAUSE_FAILURE_REMAIN_ON_PLMN) &&
          (mmcoord_mmr_reg_cnf.cause != MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED) &&
          (((gmm_gprs_attach_pending || gmm_ra_update_pending) &&
            !(is_rai_present_foreground_search_list) &&
            !((gmm_substate == GMM_LIMITED_SERVICE) ||
              (gmm_substate == GMM_NO_CELL_AVAILABLE))) ||
           ((mm_timer_status[TIMER_T3310]  == TIMER_ACTIVE ||    
              mm_timer_status[TIMER_T3330]  == TIMER_ACTIVE) &&
               (!(gmm_substate == GMM_LIMITED_SERVICE ||
                 gmm_substate == GMM_NO_CELL_AVAILABLE))) ||
           ((mm_timer_status[TIMER_T3311]  == TIMER_ACTIVE) &&
            ((mmcoord_mmr_reg_cnf.service_state.service_status != SYS_SRV_STATUS_LIMITED) &&
             !((mmcoord_mmr_reg_cnf.service_state.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
               (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))))) &&
                         (mmcoord_mmr_reg_cnf.service_state.service_status != SYS_SRV_STATUS_NO_SRV))
       {
          mmcoord_mmr_reg_cnf.only_for_reg = TRUE;
       }
     }
     if((mm_to_only_reg==TRUE)&& (mm_previous_serving_rat != SYS_RAT_NONE) &&
         mmcoord_mmr_reg_cnf.cause == MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH)
     {
         mmcoord_mmr_reg_cnf.service_state.active_rat = mm_previous_serving_rat;
     }
     if (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode  == SYS_NETWORK_SELECTION_MODE_LIMITED)
     {
	   mmcoord_mmr_reg_cnf.only_for_reg = FALSE;
       if (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)
       {
         if (((mm_location_information.location_update_status == UPDATED) &&
              (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)) &&
             ((gmm_state ==  GMM_REGISTERED) &&
              ((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                    gmm_stored_rai.routing_area_code,
                                    mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                  (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
               (gmm_update_status == GMM_GU1_UPDATED))))
         {
           mmcoord_mmr_reg_cnf.service_state.service_status = SYS_SRV_STATUS_SRV;
         }
       }
       else if(mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)
       {
          if ((mm_location_information.location_update_status == UPDATED) &&
                (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0))
          {
             mmcoord_mmr_reg_cnf.service_state.service_status = SYS_SRV_STATUS_SRV;
          }
       }
       else if(mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY)
       {
           if ((gmm_state ==  GMM_REGISTERED) &&
                ((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                      gmm_stored_rai.routing_area_code,
                                      mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                    (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
                 (gmm_update_status == GMM_GU1_UPDATED)))
           {
              mmcoord_mmr_reg_cnf.service_state.service_status = SYS_SRV_STATUS_SRV;
              mm_csfb_service_ind_status = MM_CSFB_SEND_SERVICE_STATUS_NONE;
           }
       }
     }


     if (mmcoord_mmr_reg_cnf.service_state.service_status == SYS_SRV_STATUS_SRV)
     {
       mm_csfb_service_ind_status = MM_CSFB_SEND_SERVICE_STATUS_NONE;
     }

     mmcoord_mmr_reg_cnf.cell_id = mm_coord_camped_cell_id;
#ifdef FEATURE_ENHANCED_NW_SELECTION
     if( reg_sim_read_ens_flag() &&
         (mm_foreground_search_lai_timer_list.length != 0) &&
       !(mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai)) &&
         mmcoord_mmr_reg_cnf.service_state.service_status == SYS_SRV_STATUS_SRV)
     {
       MSG_HIGH_DS_0(MM_SUB,"=MM= Restore forbidden list- Full Service on different LAI ");
       mm_inform_updated_foreground_search_list();
     }

     if(reg_sim_read_ens_flag() &&
        (gmm_state == GMM_REGISTERED)&&
        ((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                              gmm_stored_rai.routing_area_code,
                              mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
          (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
          (gmm_update_status == GMM_GU1_UPDATED)) &&
          (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY ) &&
          (mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai)) &&
          (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))
     {
        MSG_HIGH_DS_0(MM_SUB,"=MM= Remove LAI from forbidden list- PS Full Service ");
        mm_remove_ps_registered_lai_from_foreground_search_lai_list();
     }            
#endif
     if(mmcoord_mmr_reg_cnf.only_for_reg == FALSE)
     {
       mm_service_ind_blocked_due_to_irat_cs_reg = FALSE;
     }
     PUT_IMH_LEN( sizeof(mmr_reg_cnf_s_type) - sizeof(IMH_T),
                  &mmcoord_mmr_reg_cnf.message_header );

     MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMR_REG_CNF");
     MSG_HIGH_DS_3(MM_SUB, "=MM=   cause = %d sim_state = %d svc_capability = %d",
               mmcoord_mmr_reg_cnf.cause,
               mmcoord_mmr_reg_cnf.service_state.sim_state,
               mmcoord_mmr_reg_cnf.service_state.plmn_service_capability );
     MSG_HIGH_DS_3(MM_SUB, "=MM=   plmn_forbidden = %d svc_status = %d reg_svc_domain = %d",
               mmcoord_mmr_reg_cnf.service_state.plmn_forbidden,
               mmcoord_mmr_reg_cnf.service_state.service_status,
               mmcoord_mmr_reg_cnf.service_state.reg_service_domain );
     MSG_HIGH_DS_3(MM_SUB, "=MM=   active_rat = %d ps_data_suspend = %d plmn_reg_type = %d",
               mmcoord_mmr_reg_cnf.service_state.active_rat,
               mmcoord_mmr_reg_cnf.service_state.ps_data_suspend, 
               mmcoord_mmr_reg_cnf.service_state.plmn_reg_type);
     MSG_HIGH_DS_3(MM_SUB, "=MM=   equiv PLMN update=%d list len=%d info_blocked_at_reg = %d",
               mmcoord_mmr_reg_cnf.update_equivalent_plmn_list,
               mmcoord_mmr_reg_cnf.equivalent_plmn_list.length, 
               mmcoord_mmr_reg_cnf.only_for_reg);
     MSG_HIGH_DS_1(MM_SUB,"=MM= LTE IRAT BPLMN: camped cell id %d",mmcoord_mmr_reg_cnf.cell_id);
  #ifdef FEATURE_FEMTO_CSG
     MSG_HIGH_DS_1(MM_SUB ,"=MM=  CSG: csg_id = %lx",
               mmcoord_mmr_reg_cnf.service_state.csg_info.csg_id);
  #endif 

     if( mmcoord_mmr_reg_cnf.cause != MMR_CAUSE_FAILURE_MAX_ATTEMPTED )
     {
       mm_reg_cause =  mmcoord_mmr_reg_cnf.cause;
     }
     else
     {
       mm_reg_cause =   MMR_CAUSE_NONE;
     }
     mm_reg_ind_sent = TRUE;
     for (i = 0; i< mmcoord_mmr_reg_cnf.equivalent_plmn_list.length; i++)
     {
        MSG_MED_DS(MM_SUB,"=MM= equiv PLMN-%d", i,0,0);
        MSG_MED_DS(MM_SUB,"=MM= \t %x %x %x",
           mmcoord_mmr_reg_cnf.equivalent_plmn_list.plmn[i].identity[0],
           mmcoord_mmr_reg_cnf.equivalent_plmn_list.plmn[i].identity[1],
           mmcoord_mmr_reg_cnf.equivalent_plmn_list.plmn[i].identity[2]);
     }

#ifdef FEATURE_SGLTE
     if(!MM_IS_IN_SGLTE_MODE || (mm_as_id == mm_cs_stack_id))
#endif
     {
       mm_send_rr_status_ind(mmcoord_mmr_reg_cnf.service_state.service_status, mmcoord_mmr_reg_cnf.service_state.reg_service_domain);
     }

   
     /* set the LAC info from serving plmn */
#ifdef FEATURE_GSM_BAND_PREF
     mmcoord_mmr_reg_cnf.service_state.lac = b_unpackw(mm_serving_plmn.lac.lac,0,16);
#endif /*FEATURE_GSM_BAND_PREF*/
     mmcoord_mmr_reg_cnf.rac = mm_serving_plmn.rac;
     mmcoord_mmr_reg_cnf.service_state.rac_or_mme_code = mm_serving_plmn.rac;
#if defined(FEATURE_WRLF_SYSTEM_SEL) || defined(FEATURE_LTE)
     if(mm_serving_plmn.info.no_service_cause == SYS_NO_SVC_CAUSE_RLF)
     {
       mmcoord_mmr_reg_cnf.service_state.no_service_cause = SYS_NO_SVC_CAUSE_RLF;
     }
     else
     {
       mmcoord_mmr_reg_cnf.service_state.no_service_cause = SYS_NO_SVC_CAUSE_NORMAL;
     }
#endif 
     mm_send_message( (IMH_T *)&mmcoord_mmr_reg_cnf, GS_QUEUE_REG );
#ifdef FEATURE_LTE
     emm_update_rrc_rplmn_info();
#endif

     if ((mm_mmr_cell_service_ind_pending) &&
         (mmcoord_mmr_reg_cnf.only_for_reg == FALSE))
     {
       mm_send_mmr_cell_service_ind();
     }
        /* check if Attach transaction is ongoing and nothing is pending in GMM */
   if (((mm_mmr_attach_ind_status == ATTACH_FAILED_IND_REQUIRED) &&
        (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS || mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) &&
         (!(gmm_gprs_attach_pending) ||
            ((gmm_substate == GMM_LIMITED_SERVICE) ||
              (gmm_substate == GMM_NO_CELL_AVAILABLE))) &&
         (((mmcoord_mmr_reg_cnf.service_state.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY)||
            (mmcoord_mmr_reg_cnf.service_state.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))||
            ((mm_timer_status[TIMER_T3330]  != TIMER_ACTIVE)&&
              !gmm_ra_update_pending))&&
          (gmm_state != GMM_REGISTERED_INITIATED) &&
          (mm_timer_status[TIMER_T3310]  != TIMER_ACTIVE) &&
         (((mm_timer_status[TIMER_T3311]  != TIMER_ACTIVE) ||
           (((mmcoord_mmr_reg_cnf.service_state.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
             (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)))) ||
             (mmcoord_mmr_reg_cnf.service_state.service_status == SYS_SRV_STATUS_NO_SRV)))
       )
  {
     MSG_HIGH_DS_0(MM_SUB,"=MM= Attach failed indication not required");
     mm_mmr_attach_ind_status = ATTACH_FAILED_IND_NOT_REQUIRED;

     /* check if attach failed indication has to be sent */
     /*Checkin to ensure that On No service we send attach failed */
     /*Or Check for registered Service domain is ensuring that we dont send Failed IND on Successful ATTACH*/
     if((mm_serving_plmn.info.service_status == SYS_SRV_STATUS_NO_SRV ) ||
        (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_LIMITED) ||
        ((mmcoord_mmr_reg_cnf.service_state.reg_service_domain != SYS_SRV_DOMAIN_PS_ONLY) &&
         (mmcoord_mmr_reg_cnf.service_state.reg_service_domain != SYS_SRV_DOMAIN_CS_PS)))
     {
       mm_send_reg_attach_failed_ind();
     }
  }
     /* Update RR, RRC with new network selection mode if got full service
     ** in LIMITED_ROUTINE_ACQ or LIMITED_MANUAL_ACQ 
     */
     if((mmcoord_mmr_reg_cnf.service_state.service_status == SYS_SRV_STATUS_SRV) &&
        (mmcoord_pending_reg_message.cmd.hdr.message_id == (int) MMR_REG_REQ) &&
        ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) ||
         (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)))
     {
#ifdef FEATURE_WCDMA
       if (mm_is_multimode_sub())
       {
         rrc_cmd_type *rrc_cmd = NULL;
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifdef FEATURE_SEGMENT_LOADING
         if (!mm_wcdma_supported)
         {
            return;
         }
#endif /* FEATURE_SEGMENT_LOADING*/
         rrc_cmd =  mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );
         if (rrc_cmd == NULL)
         {
           MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
         }         
         rrc_cmd->cmd_hdr.cmd_id = RRC_NW_SEL_MODE_RESET_IND;
         rrc_cmd->cmd.rrc_nw_sel_mode_reset_ind.network_select_mode =
             (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)?
             SYS_NETWORK_SELECTION_MODE_AUTOMATIC:SYS_NETWORK_SELECTION_MODE_MANUAL;  
         send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
#else /* FEATURE_RAT_PRIORITY_LIST*/

         /* Broadcast the message to T and W, if present */
         /* If a RAT is present in the rat prio list, its module must have been loaded */
         if (mm_checkif_rat_pri_list_has_rat( &mm_rat_pri_list_info, SYS_RAT_TDS_RADIO_ACCESS ))
         {
           rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );         
           if (rrc_cmd == NULL)
           {
               MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
           }
           rrc_cmd->cmd_hdr.cmd_id = RRC_NW_SEL_MODE_RESET_IND;
           rrc_cmd->cmd.rrc_nw_sel_mode_reset_ind.network_select_mode =
               (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)?
               SYS_NETWORK_SELECTION_MODE_AUTOMATIC:SYS_NETWORK_SELECTION_MODE_MANUAL;  
           send_cmd_to_rrc( rrc_cmd, SYS_RAT_TDS_RADIO_ACCESS );
         }
         if (mm_checkif_rat_pri_list_has_rat( &mm_rat_pri_list_info, SYS_RAT_UMTS_RADIO_ACCESS ))
         {
           rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );         
           if (rrc_cmd == NULL)
           {
               MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
           }
           rrc_cmd->cmd_hdr.cmd_id = RRC_NW_SEL_MODE_RESET_IND;
           rrc_cmd->cmd.rrc_nw_sel_mode_reset_ind.network_select_mode =
               (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)?
               SYS_NETWORK_SELECTION_MODE_AUTOMATIC:SYS_NETWORK_SELECTION_MODE_MANUAL;  
           send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
         }      
#endif  /* FEATURE_RAT_PRIORITY_LIST*/
       } /* mm_is_multimode_sub() */
#endif /* FEATURE_WCDMA */
       
#if (defined(FEATURE_DUAL_SIM) && !defined(FEATURE_TWO_3GPP_SIM)) || defined(FEATURE_SGLTE)
       if (mm_is_gsm_allowed_on_sub())
#endif
       {
         rr_nw_sel_mode_reset_ind_T reset_ind; 
         memset(&reset_ind,0x00,sizeof(rr_nw_sel_mode_reset_ind_T));
         reset_ind.message_header.message_set = MS_MM_RR;
         reset_ind.message_header.message_id  = (int)RR_NW_SEL_MODE_RESET_IND;
         reset_ind.network_select_mode =  
           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)?
             SYS_NETWORK_SELECTION_MODE_AUTOMATIC:SYS_NETWORK_SELECTION_MODE_MANUAL;
#ifdef FEATURE_LTE
         mm_emm_store_reg_req_nw_selection_mode(reset_ind.network_select_mode);
#else
         mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode = reset_ind.network_select_mode;
#endif
         PUT_IMH_LEN( sizeof( rr_nw_sel_mode_reset_ind_T ) - sizeof(IMH_T),
                   &reset_ind.message_header );
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RR_NW_SEL_MODE_RESET_IND");
  
         mm_send_message( (IMH_T *)&reset_ind, GS_QUEUE_RR );
       }
     }
   }
   /* Re-initialize the equivalent PLMN list information
   *  in the mm_serving_plmn global structure
   */
   mm_serving_plmn.update_equiv_plmn_list = FALSE;
   memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));

   /*reset to NA*/ 
   mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_NOT_APPLICABLE;


}


/*===========================================================================

FUNCTION MM_SEND_TMSI_REALLOCATION_COMPLETE

DESCRIPTION
  This function sends a tmsi_reallocation_complete message to the rr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_tmsi_reallocation_complete( void )
{
   tmsi_reallocation_complete_msg_T tmsi_reallocation_complete;
   dword ota_msg_length = 2; /* Start with PD and Type */

   memset(&tmsi_reallocation_complete, 0x00, sizeof(tmsi_reallocation_complete_msg_T));
   tmsi_reallocation_complete.msg_head.skip_pd = PD_MM;
   tmsi_reallocation_complete.msg_head.type = TMSI_REALLOCATION_COMPLETE;

   MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent TMSI_REALLOCATION_COMPLETE");

   /* Send the OTA message */
   mm_send_ota_message( (byte*)&tmsi_reallocation_complete, ota_msg_length);
}



/*===========================================================================

FUNCTION MM_SEND_RR_SIM_REMOVED_REQ

DESCRIPTION
  This function sends a rr_sim_removed_req to rr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_sim_removed_req( void )
{
#if defined(FEATURE_DUAL_SIM) && !defined(FEATURE_TWO_3GPP_SIM)
   if (mm_is_gsm_allowed_on_sub())
#endif
   {
   rr_invalidate_sim_data_req_T    rr_sim_removed_req;
   memset(&rr_sim_removed_req,0x00,sizeof(rr_invalidate_sim_data_req_T));
   rr_sim_removed_req.message_header.message_set = MS_MM_RR;
   rr_sim_removed_req.message_header.message_id  = (int)RR_INVALIDATE_SIM_DATA_REQ;

   PUT_IMH_LEN( (sizeof(rr_invalidate_sim_data_req_T) -sizeof(IMH_T)),
                &rr_sim_removed_req.message_header );

   MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RR_INVALIDATE_SIM_DATA_REQ");

   mm_send_message( (IMH_T *)&rr_sim_removed_req, GS_QUEUE_RR );
   }
}


/*===========================================================================

FUNCTION MM_SEND_RR_SIM_UPDATE_REQ

DESCRIPTION
  This function sends a rr_sim_update_req to rr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_sim_update_req( void )
{

#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

#if (defined(FEATURE_DUAL_SIM) && !defined(FEATURE_TWO_3GPP_SIM)) || defined(FEATURE_SGLTE)
  if (mm_is_gsm_allowed_on_sub())
#endif
  {
   rr_sim_update_req_T    rr_sim_update_req;
   memset(&rr_sim_update_req,0x00,sizeof(rr_sim_update_req_T));
   rr_sim_update_req.message_header.message_set = MS_MM_RR;
   rr_sim_update_req.message_header.message_id  = (int)RR_SIM_UPDATE_REQ;

   /* Set the cipher key & cipher key sequence number */
   memscpy((void*)rr_sim_update_req.ciphering_key, sizeof(mm_sim_efkc_T), (void*)auth_gsm_cipherkey.value_data,
             auth_gsm_cipherkey.value_length);
   rr_sim_update_req.ciphering_key_seq_no = auth_gsm_cksn;

   /* Set TMSI & PTMSI */
   memscpy(rr_sim_update_req.tmsi, sizeof(mm_sim_tmsi_T), mm_location_information.tmsi,TMSI_SIZE);
   memscpy(rr_sim_update_req.ptmsi, sizeof(mm_sim_ptmsi_T), gmm_stored_ptmsi.digit,PTMSI_SIZE);

   /* To be revisited after GPRS related changes to Auth Server are available */

   /* Set the GPRS cipher key & GPRS cipher key sequence number */
   memscpy((void*)rr_sim_update_req.gprs_ciphering_key,sizeof(mm_sim_efkcgprs_T),
                  (void*)auth_gsm_gprs_cipherkey.value_data,MM_SIM_EFKC_GPRS_SIZE) ;
   rr_sim_update_req.gprs_ciphering_key_seq_no = auth_gsm_gprs_cksn;
   
   /* Set the GSM 128 bit generated kc */
   memscpy((void*)rr_sim_update_req.gsm_kc_128bit,sizeof(mm_gsm_kc_128bit_T),
                  (void*)gsm_kc_bit128.value_data,MM_GSM_128BIT_KC_SIZE) ;
   memscpy((void*)rr_sim_update_req.gsm_gprs_kc_128bit,sizeof(mm_gsm_kc_128bit_T),
                  (void*)gsm_gprs_kc_bit128.value_data,MM_GSM_128BIT_KC_SIZE) ;
#ifdef FEATURE_LTE
   if(((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) &&
       (emm_ctrl_data_ptr->emm_guti_valid == TRUE)

#ifdef FEATURE_SGLTE
                            &&     MM_IS_SGLTE_ON_PS_STACK_ID)
#else
                                    )
#endif
       )
   {
     emm_map_guti_to_rai(&rr_sim_update_req.registered_rai, emm_ctrl_data_ptr);
   }
   else if(((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI ||
               emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
               mm_is_rai_valid()

#ifdef FEATURE_SGLTE
                            &&     MM_IS_SGLTE_ON_PS_STACK_ID)
#else
                                    )
#endif
               )
   {
     memscpy((void *)&rr_sim_update_req.registered_rai, sizeof(routing_area_id_T),
                   (void *)&gmm_stored_rai,sizeof(routing_area_id_T)) ;     
   }
   else if(mm_is_rai_valid())
   {
     memscpy((void *)&rr_sim_update_req.registered_rai, sizeof(routing_area_id_T),
                   (void *)&gmm_stored_rai,sizeof(routing_area_id_T)) ;    
   }
   else if( emm_ctrl_data_ptr->emm_guti_valid == TRUE

#ifdef FEATURE_SGLTE
                            &&     MM_IS_SGLTE_ON_PS_STACK_ID
#endif
   )
   {
     emm_map_guti_to_rai(&rr_sim_update_req.registered_rai, emm_ctrl_data_ptr);
   }
   else
   {
      /*This will only happen when GMM is not in registered state
         This will sned registered_rai as invalid to GRR */
     memscpy((void *)&rr_sim_update_req.registered_rai, sizeof(routing_area_id_T),
                   (void *)&gmm_stored_rai,sizeof(routing_area_id_T)) ;    
   }
#else /*FEATURE_LTE*/
   memscpy((void *)&rr_sim_update_req.registered_rai, sizeof(routing_area_id_T),
                  (void *)&gmm_stored_rai,sizeof(routing_area_id_T)) ;  

#endif   /*FEATURE_LTE */

#ifdef FEATURE_GAN
    #error code not present
#endif /* FEATURE_GAN */

#if defined(FEATURE_DUAL_SIM) && !defined(FEATURE_LTE)
   rr_sim_update_req.wcdma_allowed =  mm_is_umts_allowed_on_sub();
#endif


   PUT_IMH_LEN( sizeof( rr_sim_update_req_T ) - sizeof(IMH_T),
                 &rr_sim_update_req.message_header );

   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RR_SIM_UPDATE_REQ");

   mm_send_message( (IMH_T *)&rr_sim_update_req, GS_QUEUE_RR );
  }
}

#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================

FUNCTION MM_SEND_RR_PSHO_CNF

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_psho_cnf()
{
   rr_psho_cnf_T psho_cnf;

   psho_cnf.message_header.message_set = MS_MM_RR;
   psho_cnf.message_header.message_id  = (int) RR_PSHO_CNF;

   PUT_IMH_LEN((sizeof(rr_psho_cnf_T)-sizeof(IMH_T)),&psho_cnf.message_header);

   mm_send_message( (IMH_T *)&psho_cnf, GS_QUEUE_RR );

} /* end mm_send_rr_psho_cnf() */
#endif

#ifdef FEATURE_DUAL_DATA
/*===========================================================================
FUNCTION MM_SEND_RR_DATA_PRIORITY_IND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_send_rr_data_priority_ind(sys_data_priority_e_type priority)
{
  rr_data_priority_ind_T   rr_data_prio_ind;
  memset(&rr_data_prio_ind,0x00,sizeof(rr_data_priority_ind_T));
  MSG_HIGH_DS_1(MM_SUB, "=MM= Sending RR_DATA_PRIORITY_IND to RR %d", priority);
  rr_data_prio_ind.message_header.message_set = MS_MM_RR;
  rr_data_prio_ind.message_header.message_id = (int) RR_DATA_PRIORITY_IND;
  rr_data_prio_ind.priority = priority;
  PUT_IMH_LEN( sizeof( rr_data_priority_ind_T ) - sizeof(IMH_T),
                &rr_data_prio_ind.message_header );
  mm_send_message((IMH_T *)&rr_data_prio_ind,GS_QUEUE_RR );
}
#endif

/*===========================================================================

FUNCTION MM_SEND_IMSI_DETACH

DESCRIPTION
  This function sends the IMSI detach mesage, using, in order of precedence,
  TMSI, or IMSI. There is no flag to indicate that this is done, as, in
  the fullness of time, the RR connection will be released

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_imsi_detach( void )
{

   imsi_detach_indication_msg_T     imsi_detach_indication;
   uint32                           message_length;

   imsi_detach_indication.msg_head.IMH.message_set = MS_MM_RR;
   imsi_detach_indication.msg_head.IMH.message_id  = (int)RR_DATA_REQ;

   imsi_detach_indication.msg_head.skip_pd = PD_MM;
   imsi_detach_indication.msg_head.type = IMSI_DETACH_INDICATION;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
   {
     classmark_params_T classmark_params;
     classmark_params.gsm_supported = mm_gsm_supported;
     classmark_params.wcdma_in_use  = is_umts_mode();
     classmark_params.MSCR99        = mm_system_information.mscr;
     classmark_params.as_id         = (sys_modem_as_id_e_type)mm_sub_id;
     imsi_detach_indication.classmark_1_data = gs_get_classmark_1_per_nas_stacks( &classmark_params,(sys_modem_as_id_e_type)mm_as_id );
   }
#else
   imsi_detach_indication.classmark_1_data =  gs_get_classmark_1
                                              ( mm_gsm_supported, is_umts_mode(),
                                                mm_system_information.mscr );
#endif

   message_length = (sizeof( imsi_detach_indication.msg_head ) -
                     sizeof( IMH_T )) +
                     sizeof( imsi_detach_indication.classmark_1_data );

   if ( (has_valid_tmsi()) &&
        ( memcmp( mm_serving_lai,
                  mm_location_information.lai,
                  LAI_SIZE ) == 0 ) )
   {
      /* Copy TMSI */
      message_length += mm_pack_tmsi(imsi_detach_indication.mobile_identity_data);
   }
   else
   {
      /* Copy IMSI */
      message_length += mm_pack_imsi(imsi_detach_indication.mobile_identity_data);
   }

   PUT_IMH_LEN( message_length, &imsi_detach_indication.msg_head.IMH );

   /*
    *    IF RR connection exists
    */

   MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent IMSI_DETACH_INDICATION");

   if (mm_state == MM_IMSI_DETACH_INITIATED)
   {

      /* Send the OTA message */
      if (message_length <= ((MMGSDI_MAX_VALID_IMSI_LEN+1) +
                             (sizeof( imsi_detach_indication.msg_head )) -
                             (sizeof( IMH_T )) +
                             (sizeof( imsi_detach_indication.classmark_1_data ))))
      {
        mm_send_ota_message( (byte*)&imsi_detach_indication,
                             (dword)message_length);
      }
      else
      {
        MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
      }

      mm_start_timer( TIMER_T3220, DEFAULT_TIMEOUT );
   }
   else
   {
      mm_initiate_rr_connection(MM_RR_IMSI_DETACH,
                                 (IMH_T *)&imsi_detach_indication);
   }
   /* Gs association gets broken here since CS services are explicitly de-registered */
   mm_gs_association_established = FALSE;
   #ifdef FEATURE_DSAC      
     mm_last_procedure_to_update_registration &=~MM_LU;
     mm_last_procedure_to_update_registration &=~MM_COMBINED;
   #endif

}


/*===========================================================================

FUNCTION MM_SEND_MMR_SERVICE_IND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_service_ind(mmr_service_ind_s_type *mmr_service_ind)
{
   unsigned int i = 0;
   boolean is_rai_present_foreground_search_list = FALSE;
   
   mmr_service_ind->message_header.message_set = MS_MM_REG;
   mmr_service_ind->message_header.message_id  = (int)MMR_SERVICE_IND;
#ifdef FEATURE_LTE  
  mmr_service_ind->service_state.lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
  
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif

#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
   if (is_tdscdma_mode())
   {
     mmr_service_ind->service_state.active_band.tds_active_band = get_active_band();
   }
#ifdef FEATURE_LTE  
   else if (is_lte_mode())
   {
     mmr_service_ind->service_state.active_band.lte_active_band = emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band;
   }
#endif
   else
   {
     mmr_service_ind->service_state.active_band.active_band = get_active_band();
   }
#endif
   /*
   ** Initialize equiv plmn lit
   **/

   /* Inform REG that the LAI is forbidden */
   if ((mm_check_forbidden_national_roaming((inter_task_lai_T*)mm_serving_lai)) ||
       (mm_check_forbidden_regional_roaming((inter_task_lai_T*)mm_serving_lai)))
   {
     mmr_service_ind->lai_forbidden = TRUE;
   }
   else
   {
     mmr_service_ind->lai_forbidden = FALSE;
   }

   memset(&(mmr_service_ind->equivalent_plmn_list), 0x00, sizeof(sys_plmn_list_s_type));

   /*
   ** Copy the equivalent PLMN list information from the mm_serving_plmn structure
   */
   mmr_service_ind->update_equivalent_plmn_list = mm_serving_plmn.update_equiv_plmn_list;

   if ((mm_managed_roaming_enabled) 
#ifdef  FEATURE_ENHANCED_NW_SELECTION
    || (reg_sim_read_ens_flag())
#endif
      )
   {
     if((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
       mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) ||
       (mmr_service_ind->service_state.cs_service_status != SYS_SRV_STATUS_LIMITED &&
         mmr_service_ind->service_state.service_status != SYS_SRV_STATUS_LIMITED))
     {
       mmr_service_ind->service_state.cs_service_status = mmr_service_ind->service_state.service_status;
     }
   }
   else
   {
     mmr_service_ind->service_state.cs_service_status = mmr_service_ind->service_state.service_status;
   }

   if ((mm_serving_plmn.equiv_plmn_list.length > 0) &&
       (mm_serving_plmn.equiv_plmn_list.length <= (EQUIVALENT_PLMN_LIST_MAX_LENGTH * sizeof(sys_plmn_id_s_type))))
   {
      mmr_service_ind->equivalent_plmn_list.length =
             mm_serving_plmn.equiv_plmn_list.length / sizeof(sys_plmn_id_s_type);

      memscpy(mmr_service_ind->equivalent_plmn_list.plmn, SYS_PLMN_LIST_MAX_LENGTH * sizeof(sys_plmn_id_s_type),
             mm_serving_plmn.equiv_plmn_list.plmn, mm_serving_plmn.equiv_plmn_list.length);
   }
   else if (mm_serving_plmn.equiv_plmn_list.length > (EQUIVALENT_PLMN_LIST_MAX_LENGTH * sizeof(sys_plmn_id_s_type)))
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Equiv PLMN list len %d too long",
        mm_serving_plmn.equiv_plmn_list.length,0,0);
   }

 #ifdef FEATURE_FEMTO_CSG
   if(mm_is_csg_in_allowed_list(mmr_service_ind->service_state.plmn, mmr_service_ind->service_state.csg_info.csg_id) ||
     mm_is_csg_in_operator_list(mmr_service_ind->service_state.plmn, mmr_service_ind->service_state.csg_info.csg_id, FALSE))
   {
     mmr_service_ind->service_state.csg_in_white_list = TRUE;
   }
   else
   {
     mmr_service_ind->service_state.csg_in_white_list = FALSE;
   }
 #endif 

   mmr_service_ind->only_for_reg = mm_to_only_reg;
#ifdef FEATURE_WRLF_SYSTEM_SEL
   if(mm_wcdma_rlf_state == WCDMA_RLF_STARTED)
   {
     mmr_service_ind->only_for_reg = FALSE;
     mm_service_ind_blocked_due_to_cs_reg = FALSE;
   }else
#endif
   if( (mm_state == MM_LOCATION_UPDATE_INITIATED) ||
        (mm_state == MM_LOCATION_UPDATE_REJECTED) ||
        (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) ||
        ((mm_state == MM_IDLE) &&
         ((mm_timer_status[TIMER_T3211]  == TIMER_ACTIVE||
           mm_timer_status[TIMER_T3213]  == TIMER_ACTIVE)&& 
            (!(mm_idle_substate == MM_LIMITED_SERVICE ||
              mm_idle_substate == MM_NO_CELL_AVAILABLE))) &&
         (mm_csfb_service_ind_status == MM_CSFB_SEND_SERVICE_STATUS_NONE) &&
         ((mm_serving_plmn.ms_op_mode ==SYS_SRV_DOMAIN_CS_ONLY ) ||
          (mm_serving_plmn.ms_op_mode ==SYS_SRV_DOMAIN_CS_PS)) &&
         (mm_serving_plmn.info.service_status != SYS_SRV_STATUS_NO_SRV )))
   {
      mmr_service_ind->only_for_reg = TRUE;
      mm_service_ind_blocked_due_to_cs_reg = TRUE;
   }
   else
   {
     mm_service_ind_blocked_due_to_cs_reg = FALSE;
     
#ifdef FEATURE_ENHANCED_NW_SELECTION
     if(gmm_is_rai_in_foreground_search_lai_rai_timer_list((byte*)&gmm_current_lai,gmm_current_rac))
     {
       is_rai_present_foreground_search_list = TRUE;
     }  
#endif

     if(((mm_state != MM_CONNECTION_ACTIVE) &&
              (mm_state != MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) &&
              (mm_state != MM_WAIT_FOR_RR_CONNECTION_MM)) &&
        (mmr_service_ind->service_state.reg_service_domain != SYS_SRV_DOMAIN_CS_ONLY) &&
        !((mm_location_information.location_update_status == UPDATED) &&
           (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)) &&
        (mmr_service_ind->cause != MMR_CAUSE_FAILURE_REMAIN_ON_PLMN) &&
        (mmr_service_ind->cause != MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED) &&
        (((gmm_gprs_attach_pending || gmm_ra_update_pending) &&
            !(is_rai_present_foreground_search_list) &&
            !((gmm_substate == GMM_LIMITED_SERVICE) ||
              (gmm_substate == GMM_NO_CELL_AVAILABLE))) ||
         ((mm_timer_status[TIMER_T3310]  == TIMER_ACTIVE ||    
              mm_timer_status[TIMER_T3330]  == TIMER_ACTIVE) &&
               (!(gmm_substate == GMM_LIMITED_SERVICE ||
                 gmm_substate == GMM_NO_CELL_AVAILABLE))) || 
         ((mm_timer_status[TIMER_T3311]  == TIMER_ACTIVE) &&
          ((mmr_service_ind->service_state.service_status != SYS_SRV_STATUS_LIMITED) &&
           !((mmr_service_ind->service_state.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
             (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))))) &&
                       (mmr_service_ind->service_state.service_status != SYS_SRV_STATUS_NO_SRV))
     {
        mmr_service_ind->only_for_reg = TRUE;
     }
   }
   if((mm_to_only_reg==TRUE)&& (mm_previous_serving_rat != SYS_RAT_NONE) && 
       mmr_service_ind->cause == MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH)
   {
         mmr_service_ind->service_state.active_rat = mm_previous_serving_rat;
   }
   mmcoord_mmr_reg_cnf.only_for_reg = mmr_service_ind->only_for_reg;
   mmcoord_mmr_reg_cnf.cell_id = mm_coord_camped_cell_id;
   mmr_service_ind->cell_id = mm_coord_camped_cell_id;
   if (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode  == SYS_NETWORK_SELECTION_MODE_LIMITED)
   {
     mmr_service_ind->only_for_reg = FALSE;
     if (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)
     {
        if (((mm_location_information.location_update_status == UPDATED) &&
              (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)) &&
             ((gmm_state ==  GMM_REGISTERED) &&
              ((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                    gmm_stored_rai.routing_area_code,
                                    mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                  (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
               (gmm_update_status == GMM_GU1_UPDATED))))
        {
           mmr_service_ind->service_state.service_status = SYS_SRV_STATUS_SRV;
        }
     }
     else if(mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)
     {
        if ((mm_location_information.location_update_status == UPDATED) &&
            (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0))
        {
           mmr_service_ind->service_state.service_status = SYS_SRV_STATUS_SRV;
        }
     }
     else if(mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY)
     {
        if ((gmm_state ==  GMM_REGISTERED) &&
            ((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                  gmm_stored_rai.routing_area_code,
                                  mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
                (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
             (gmm_update_status == GMM_GU1_UPDATED)))
        {
           mmr_service_ind->service_state.service_status = SYS_SRV_STATUS_SRV;
           mm_csfb_service_ind_status = MM_CSFB_SEND_SERVICE_STATUS_NONE;
        }
     }
   }

   if (mmr_service_ind->service_state.service_status == SYS_SRV_STATUS_SRV)
   {
     mm_csfb_service_ind_status = MM_CSFB_SEND_SERVICE_STATUS_NONE;
   }

#ifdef FEATURE_ENHANCED_NW_SELECTION
     if( reg_sim_read_ens_flag() &&
         (mm_foreground_search_lai_timer_list.length != 0) &&
         !(mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai)) &&
         mmr_service_ind->service_state.service_status == SYS_SRV_STATUS_SRV)
     {
       MSG_HIGH_DS_0(MM_SUB,"=MM= Restore forbidden list - Full service on diff LAI");
       mm_inform_updated_foreground_search_list();
     }

     if(reg_sim_read_ens_flag() &&
        (gmm_state == GMM_REGISTERED)&&
        ((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                              gmm_stored_rai.routing_area_code,
                              mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
          (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
          (gmm_update_status == GMM_GU1_UPDATED)) &&
          (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY ) &&
          (mm_is_lai_in_foreground_search_lai_timer_list(mm_serving_lai)) &&
          (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))
     {
       MSG_HIGH_DS_0(MM_SUB,"=MM= Remove LAI from forbidden list- PS Full Service 2 ");
        mm_remove_ps_registered_lai_from_foreground_search_lai_list();
     }
#endif
     if(mmr_service_ind->only_for_reg == FALSE)
     {
       mm_service_ind_blocked_due_to_irat_cs_reg = FALSE;
     }
   PUT_IMH_LEN( sizeof( mmr_service_ind_s_type ) - sizeof( IMH_T ),
                &mmr_service_ind->message_header );

   MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent MMR_SERVICE_IND");
   MSG_HIGH_DS_3( MM_SUB, "=MM=   cause = %d sim_state = %d svc_capability = %d",
             mmr_service_ind->cause, mmr_service_ind->service_state.sim_state,
             mmr_service_ind->service_state.plmn_service_capability );
   MSG_HIGH_DS_3( MM_SUB, "=MM=   plmn_forbidden = %d svc_status = %d reg_svc_domain = %d",
             mmr_service_ind->service_state.plmn_forbidden,
             mmr_service_ind->service_state.service_status,
             mmr_service_ind->service_state.reg_service_domain );
   MSG_HIGH_DS_3( MM_SUB, "=MM=   active_rat = %d ps_data_suspend = %d plmn_reg_type = %d",
             mmr_service_ind->service_state.active_rat,
             mmr_service_ind->service_state.ps_data_suspend, 
             mmr_service_ind->service_state.plmn_reg_type );
   MSG_HIGH_DS_3( MM_SUB, "=MM=   equiv PLMN update=%d list len=%d info_blocked_at_reg = %d",
      mmr_service_ind->update_equivalent_plmn_list,
      mmr_service_ind->equivalent_plmn_list.length,
      mmr_service_ind->only_for_reg);
   MSG_HIGH_DS_1(MM_SUB,"=MM= LTE IRAT BPLMN : camped cell id %d",mmcoord_mmr_reg_cnf.cell_id);
  #ifdef FEATURE_FEMTO_CSG

     MSG_HIGH_DS_1(MM_SUB ,"=MM= CSG:  csg_id = %lx",
               mmr_service_ind->service_state.csg_info.csg_id);
  #endif 
   mm_reg_cause =  mmr_service_ind->cause;
   mm_reg_ind_sent = TRUE;

   for (i = 0; i< mmr_service_ind->equivalent_plmn_list.length; i++)
   {
      MSG_MED_DS(MM_SUB, "=MM= equiv PLMN-%d", i,0,0);
      MSG_MED_DS(MM_SUB, "=MM= \t %x %x %x",
         mmr_service_ind->equivalent_plmn_list.plmn[i].identity[0],
         mmr_service_ind->equivalent_plmn_list.plmn[i].identity[1],
         mmr_service_ind->equivalent_plmn_list.plmn[i].identity[2]);
   }

   /* set the LAC info from serving plmn */
#ifdef FEATURE_GSM_BAND_PREF
   mmr_service_ind->service_state.lac = b_unpackw(mm_serving_plmn.lac.lac,0,16);
#endif /*FEATURE_GSM_BAND_PREF*/
   mmr_service_ind->service_state.rac_or_mme_code = mm_serving_plmn.rac;
#if defined(FEATURE_WRLF_SYSTEM_SEL) || defined(FEATURE_LTE)
     if(mm_serving_plmn.info.no_service_cause == SYS_NO_SVC_CAUSE_RLF)
     {
       mmr_service_ind->service_state.no_service_cause = SYS_NO_SVC_CAUSE_RLF;
     }
     else
     {
       mmr_service_ind->service_state.no_service_cause = SYS_NO_SVC_CAUSE_NORMAL;
     }
#endif 

   mm_send_message( (IMH_T *)mmr_service_ind, GS_QUEUE_REG );
#ifdef FEATURE_LTE
   emm_update_rrc_rplmn_info();
#endif


#ifdef FEATURE_SGLTE
   if(!MM_IS_IN_SGLTE_MODE || (mm_as_id == mm_cs_stack_id))
#endif
   {
     mm_send_rr_status_ind(mmr_service_ind->service_state.service_status, mmr_service_ind->service_state.reg_service_domain);
   }

   if ((mm_mmr_cell_service_ind_pending) &&
       (mmcoord_mmr_reg_cnf.only_for_reg == FALSE))
   {
     mm_send_mmr_cell_service_ind();
   }


   /* check if Attach transaction is ongoing and nothing is pending in GMM */
   if (((mm_mmr_attach_ind_status == ATTACH_FAILED_IND_REQUIRED) &&
         (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS || mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) &&
         (!(gmm_gprs_attach_pending ) ||
            ((gmm_substate == GMM_LIMITED_SERVICE) ||
              (gmm_substate == GMM_NO_CELL_AVAILABLE))) &&
         (gmm_state != GMM_REGISTERED_INITIATED) &&
         (((mmcoord_mmr_reg_cnf.service_state.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY)||
            (mmcoord_mmr_reg_cnf.service_state.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))||
            ((mm_timer_status[TIMER_T3330] != TIMER_ACTIVE)&&
              !gmm_ra_update_pending))&&
          (mm_timer_status[TIMER_T3310]  != TIMER_ACTIVE) &&
         (((mm_timer_status[TIMER_T3311]  != TIMER_ACTIVE) ||
           (((mmr_service_ind->service_state.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
             (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)))) ||
             (mmr_service_ind->service_state.service_status == SYS_SRV_STATUS_NO_SRV)))
       )
  {
     MSG_HIGH_DS_0(MM_SUB,"=MM= Attach failed indication not required");
     mm_mmr_attach_ind_status = ATTACH_FAILED_IND_NOT_REQUIRED;

     /* check if attach failed indication has to be sent */
     /*Checkin to ensure that On No service we send attach failed */
     /*Or Check for registered Service domain is ensuring that we dont send Failed IND on Successful ATTACH*/
     if((mm_serving_plmn.info.service_status == SYS_SRV_STATUS_NO_SRV ) ||
        (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_LIMITED)  ||
        ((mmr_service_ind->service_state.reg_service_domain != SYS_SRV_DOMAIN_PS_ONLY) &&
         (mmr_service_ind->service_state.reg_service_domain != SYS_SRV_DOMAIN_CS_PS)))
     {
       mm_send_reg_attach_failed_ind();
     }
  }


   /* Update RR, RRC with new network selection mode if got full service
   ** in LIMITED_ROUTINE_ACQ or LIMITED_MANUAL_ACQ 
   */
   if((mmr_service_ind->service_state.service_status == SYS_SRV_STATUS_SRV) &&
      (mmcoord_pending_reg_message.cmd.hdr.message_id == (int) MMR_REG_REQ) &&
      ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) ||
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)))
   {
     rr_nw_sel_mode_reset_ind_T reset_ind; 
     memset(&reset_ind,0x00,sizeof(rr_nw_sel_mode_reset_ind_T));
#ifdef FEATURE_WCDMA
     if (mm_is_multimode_sub())
     {
       rrc_cmd_type *rrc_cmd = NULL;
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifdef FEATURE_SEGMENT_LOADING
         if (!mm_wcdma_supported)
         {
            return;
         }
#endif /* FEATURE_SEGMENT_LOADING*/
         rrc_cmd =  mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );
         if (rrc_cmd == NULL)
         {
           MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
         }         
         rrc_cmd->cmd_hdr.cmd_id = RRC_NW_SEL_MODE_RESET_IND;
         rrc_cmd->cmd.rrc_nw_sel_mode_reset_ind.network_select_mode =
             (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)?
             SYS_NETWORK_SELECTION_MODE_AUTOMATIC:SYS_NETWORK_SELECTION_MODE_MANUAL;  
         send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
#else /* FEATURE_RAT_PRIORITY_LIST*/

         /* Broadcast the message to T and W, if present */
         /* If a RAT is present in the rat prio list, its module must have been loaded */
         if (mm_checkif_rat_pri_list_has_rat( &mm_rat_pri_list_info, SYS_RAT_TDS_RADIO_ACCESS ))
         {
           rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );         
           if (rrc_cmd == NULL)
           {
               MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
           }
           rrc_cmd->cmd_hdr.cmd_id = RRC_NW_SEL_MODE_RESET_IND;
           rrc_cmd->cmd.rrc_nw_sel_mode_reset_ind.network_select_mode =
               (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)?
               SYS_NETWORK_SELECTION_MODE_AUTOMATIC:SYS_NETWORK_SELECTION_MODE_MANUAL;  
           send_cmd_to_rrc( rrc_cmd, SYS_RAT_TDS_RADIO_ACCESS );
         }
         if (mm_checkif_rat_pri_list_has_rat( &mm_rat_pri_list_info, SYS_RAT_UMTS_RADIO_ACCESS ))
         {
           rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );         
           if (rrc_cmd == NULL)
           {
               MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
           }
           rrc_cmd->cmd_hdr.cmd_id = RRC_NW_SEL_MODE_RESET_IND;
           rrc_cmd->cmd.rrc_nw_sel_mode_reset_ind.network_select_mode =
               (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)?
               SYS_NETWORK_SELECTION_MODE_AUTOMATIC:SYS_NETWORK_SELECTION_MODE_MANUAL;  
           send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
         }      
#endif  /* FEATURE_RAT_PRIORITY_LIST*/
     }
#endif
#if (defined(FEATURE_DUAL_SIM) && !defined(FEATURE_TWO_3GPP_SIM)) || defined(FEATURE_SGLTE)
     if (mm_is_gsm_allowed_on_sub())
#endif
     {
       reset_ind.message_header.message_set = MS_MM_RR;
       reset_ind.message_header.message_id  = (int)RR_NW_SEL_MODE_RESET_IND;
       reset_ind.network_select_mode =  
         (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)?
           SYS_NETWORK_SELECTION_MODE_AUTOMATIC:SYS_NETWORK_SELECTION_MODE_MANUAL;
#ifdef FEATURE_LTE
       mm_emm_store_reg_req_nw_selection_mode(reset_ind.network_select_mode);
#else
       mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode = reset_ind.network_select_mode;
#endif
       PUT_IMH_LEN( sizeof( rr_nw_sel_mode_reset_ind_T ) - sizeof(IMH_T),
                 &reset_ind.message_header );
       MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RR_NW_SEL_MODE_RESET_IND");

       mm_send_message( (IMH_T *)&reset_ind, GS_QUEUE_RR );
     }
   }

   /* Re-initialize the equivalent PLMN list information
   *  in the mm_serving_plmn global structure
   */
   mm_serving_plmn.update_equiv_plmn_list = FALSE;
   memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));

#ifdef FEATURE_GSM_GPRS
   if (!mmr_service_ind->service_state.ps_data_suspend)
   {
     gmm_reg_report_required = FALSE ;
   }
#endif

   mm_needs_to_send_mmr_service_ind = FALSE;

   /*reset to NA*/ 
   mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_NOT_APPLICABLE;

   mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
#ifdef FEATURE_GSM_BAND_PREF
   mmcoord_mmr_reg_cnf.service_state.lac = b_unpackw(mm_serving_plmn.lac.lac,0,16);
#endif /*FEATURE_GSM_BAND_PREF*/
   mmcoord_mmr_reg_cnf.rac = mm_serving_plmn.rac;
   mmcoord_mmr_reg_cnf.service_state.rac_or_mme_code = mm_serving_plmn.rac;

} /* end mm_send_mmr_service_ind() */

/*===========================================================================

FUNCTION MM_SEND_MMR_SIM_NOT_AVAILABLE_CNF

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_sim_not_available_cnf()
{
   mmr_sim_not_available_cnf_s_type mmr_sim_notavail_ind;

   mmr_sim_notavail_ind.message_header.message_set = MS_MM_REG;
   mmr_sim_notavail_ind.message_header.message_id  = (int)MMR_SIM_NOT_AVAILABLE_CNF;

   PUT_IMH_LEN((sizeof(mmr_sim_not_available_cnf_s_type)-sizeof(IMH_T)),&mmr_sim_notavail_ind.message_header);
   mm_send_message( (IMH_T *)&mmr_sim_notavail_ind, GS_QUEUE_REG );

} /* end mm_send_mmr_sim_not_available_cnf() */


/*===========================================================================

FUNCTION MM_SEND_MMR_SIM_AVAILABLE_CNF

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_sim_available_cnf()
{
   mmr_sim_available_cnf_s_type mmr_sim_avail_ind;

   mmr_sim_avail_ind.message_header.message_set = MS_MM_REG;
   mmr_sim_avail_ind.message_header.message_id  = (int)MMR_SIM_AVAILABLE_CNF;

   PUT_IMH_LEN((sizeof(mmr_sim_available_cnf_s_type)-sizeof(IMH_T)),&mmr_sim_avail_ind.message_header);

   mm_send_message( (IMH_T *)&mmr_sim_avail_ind, GS_QUEUE_REG );

} /* end mm_send_mmr_sim_available_cnf() */

/*===========================================================================

FUNCTION MM_SEND_RRC_SIM_UPDATE_REQ

DESCRIPTION
  This function notifies the SIM/USIM updates to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_sim_update_req
(
  rrc_cn_domain_identity_e_type domain_identity,
  sys_radio_access_tech_e_type rat
)
{
  mm_send_rrc_security_update_req(RRC_SIM_UPDATE_VALUE_VALID,domain_identity,rat);
}

/*===========================================================================

FUNCTION MM_SEND_RRC_SECURITY_UPDATE_REQ

DESCRIPTION
  This function notifies the security updates to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_security_update_req
(
  rrc_sim_update_e_type status,
  rrc_cn_domain_identity_e_type auth_domain_identity_local,
  sys_radio_access_tech_e_type rat
)
{
#ifdef FEATURE_WCDMA
  rrc_cmd_type *rrc_cmd;
  rrc_sim_update_req_type *rrc_sim_update_req_p = NULL;
  rrc_cn_ciphering_info_type *ciph_info;

   if (mm_is_multimode_sub())
   {

#ifdef FEATURE_SEGMENT_LOADING
       if ((!mm_wcdma_supported) && (rat == SYS_RAT_UMTS_RADIO_ACCESS))
       {
           /* W not loaded */
           return;
       }
       if ((!mm_tdscdma_supported) && (rat == SYS_RAT_TDS_RADIO_ACCESS))
       {
           /* T not loaded */
           return;
       }
#endif /* FEATURE_SEGMENT_LOADING */

     rrc_cmd = mm_get_rrc_cmd_buf( rat );

     if (rrc_cmd == NULL)
     {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
     }

     rrc_cmd->cmd_hdr.cmd_id = RRC_SIM_UPDATE_REQ ;
     rrc_sim_update_req_p = &(rrc_cmd->cmd.rrc_sim_update_req);

     /* Set the UE access class */
     rrc_sim_update_req_p->lo_access_class = mm_lo_access_class;
     rrc_sim_update_req_p->hi_access_class = mm_hi_access_class;

     rrc_sim_update_req_p->tmsi_present = has_valid_tmsi();

     /* Set the TMSI if it is valid */
     if (rrc_sim_update_req_p->tmsi_present)
     {
        memscpy(rrc_sim_update_req_p->tmsi.value, RRC_MAX_GSMMAP_TMSI_LENGTH,
               mm_location_information.tmsi, TMSI_SIZE);
     }

     rrc_sim_update_req_p->ptmsi_present = mm_has_valid_ptmsi();

     /* Set the PTMSI if it is valid */
     if (rrc_sim_update_req_p->ptmsi_present)
     {
        memscpy (rrc_sim_update_req_p->ptmsi.value, RRC_MAX_GSMMAP_PTMSI_LENGTH,
                gmm_stored_ptmsi.digit, RRC_MAX_GSMMAP_PTMSI_LENGTH);
     }

     ciph_info = &rrc_sim_update_req_p->security_update_info.ciphering_info;

     switch (auth_domain_identity_local)
     {
        case RRC_CS_DOMAIN_CN_ID:
           memscpy(ciph_info->ciphering_key, RRC_MAX_CIPHERING_KEY_LENGTH, auth_cs_gprs_cipherkey.value_data,
                  auth_cs_gprs_cipherkey.value_length);
           memscpy(ciph_info->integrity_key,RRC_MAX_INTEGRITY_KEY_LENGTH, auth_cs_gprs_integritykey.value_data,
                  auth_cs_gprs_integritykey.value_length);
           ciph_info->cipher_algo = (auth_ps_ciphering_algorithm != 0xff) ?
                   (rrc_cipher_algo_e_type)auth_ps_ciphering_algorithm : RRC_KASUMI_ALGORITHM;
           ciph_info->domain_id = auth_domain_identity_local;
           if((mm_sim_card_mode == MMGSDI_APP_USIM)
#ifdef FEATURE_NAS_REL11
                || (mm_sim_card_mode == MMGSDI_APP_SIM)
#endif
              )
           {
              auth_get_cs_hfn(ciph_info->hfn);
              if (auth_translate_hfn(ciph_info->hfn) >= auth_get_hfn_threshold())
              {
                 dword tmp = auth_get_hfn_threshold();
                 memset((void*)ciph_info->hfn, 0x00, 3); 
                 b_packd(tmp,ciph_info->hfn,0,20) ;
              }
           }
           else
           {
              memset((void*)ciph_info->hfn, 0x00, 3);
           }
           rrc_sim_update_req_p->security_update_info.security_update_status = status;
           break;

        case RRC_PS_DOMAIN_CN_ID:
           memscpy(ciph_info->ciphering_key, RRC_MAX_CIPHERING_KEY_LENGTH, auth_ps_gprs_cipherkey.value_data,
                  auth_ps_gprs_cipherkey.value_length);
           memscpy(ciph_info->integrity_key, RRC_MAX_INTEGRITY_KEY_LENGTH,
                  auth_ps_gprs_integritykey.value_data, auth_ps_gprs_integritykey.value_length);
           ciph_info->cipher_algo = (auth_ps_ciphering_algorithm != 0xff) ?
                   (rrc_cipher_algo_e_type)auth_ps_ciphering_algorithm : RRC_KASUMI_ALGORITHM;
           ciph_info->domain_id = auth_domain_identity_local;

           if((mm_sim_card_mode == MMGSDI_APP_USIM)
#ifdef FEATURE_NAS_REL11
                || (mm_sim_card_mode == MMGSDI_APP_SIM)
#endif
              )
           {
              auth_get_ps_hfn(ciph_info->hfn);
              if (auth_translate_hfn(ciph_info->hfn) >= auth_get_hfn_threshold())
              {
                 dword tmp = auth_get_hfn_threshold();
                 memset((void*)ciph_info->hfn, 0x00, 3);
                 b_packd(tmp,ciph_info->hfn,0,20) ;
              }
           }
           else
           {
              memset((void*)ciph_info->hfn, 0x00, 3);
           }
           rrc_sim_update_req_p->security_update_info.security_update_status = status;
           break;

        default:
           MSG_ERROR_DS( MM_SUB, "=MM= Unknown domain identity : %d", auth_domain_identity_local, 0, 0);
           rrc_sim_update_req_p->security_update_info.security_update_status =
                      RRC_SIM_UPDATE_VALUE_INVALID;
           break;
     }

     send_cmd_to_rrc( rrc_cmd, rat );
   }
#endif /* FEATURE_WCDMA */ 
} /* end mm_send_rrc_security_update_req() */


/*===========================================================================

FUNCTION MM_SEND_RRC_SIM_INSERTED_REQ

DESCRIPTION
  This function notifies SIM insertion event to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_sim_inserted_req( sys_radio_access_tech_e_type rat )
{
#ifdef FEATURE_WCDMA   

   rrc_cmd_type *rrc_cmd;
   rrc_sim_inserted_req_type *rrc_sim_inserted_req_p = NULL;
   rrc_cn_ciphering_info_type *cs_ciph_info;
   rrc_cn_ciphering_info_type *ps_ciph_info;

#ifdef FEATURE_SEGMENT_LOADING
   if ((!mm_wcdma_supported) && (rat == SYS_RAT_UMTS_RADIO_ACCESS))
   {
       /* W not loaded */
       return;
   }
   if ((!mm_tdscdma_supported) && (rat == SYS_RAT_TDS_RADIO_ACCESS))
   {
       /* T not loaded */
       return;
   }
#endif /* FEATURE_SEGMENT_LOADING */

   rrc_cmd = mm_get_rrc_cmd_buf( rat );
   if (rrc_cmd == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_SIM_INSERTED_REQ", 0, 0, 0);
   }
   else
   {
      rrc_cmd->cmd_hdr.cmd_id = RRC_SIM_INSERTED_REQ ;
      rrc_sim_inserted_req_p = &(rrc_cmd->cmd.rrc_sim_inserted_req);

      /* Set the UE access class */
      rrc_sim_inserted_req_p->lo_access_class = mm_lo_access_class;
      rrc_sim_inserted_req_p->hi_access_class = mm_hi_access_class;

      rrc_sim_inserted_req_p->tmsi_present = has_valid_tmsi();

      /* Set the TMSI if it is valid */
      if (rrc_sim_inserted_req_p->tmsi_present)
      {
         memscpy(rrc_sim_inserted_req_p->tmsi.value, RRC_MAX_GSMMAP_TMSI_LENGTH,
                mm_location_information.tmsi, TMSI_SIZE);
      }

      rrc_sim_inserted_req_p->ptmsi_present = mm_has_valid_ptmsi();

      /* Set the PTMSI if it is valid */
      if (rrc_sim_inserted_req_p->ptmsi_present)
      {
         memscpy (rrc_sim_inserted_req_p->ptmsi.value,RRC_MAX_GSMMAP_PTMSI_LENGTH,
                 gmm_stored_ptmsi.digit, RRC_MAX_GSMMAP_PTMSI_LENGTH);
      }

      rrc_sim_inserted_req_p->imsi_present = (mm_stored_imsi.length != 0);

      /* Set the IMSI if it is valid */
      if (rrc_sim_inserted_req_p->imsi_present)
      {
         rrc_sim_inserted_req_p->imsi = convert_imsi_to_digits(mm_stored_imsi);
      }

      cs_ciph_info =
         &rrc_sim_inserted_req_p->cipher_info[0];

      cs_ciph_info->domain_id = RRC_CS_DOMAIN_CN_ID;

      memscpy(cs_ciph_info->ciphering_key, RRC_MAX_CIPHERING_KEY_LENGTH, auth_cs_gprs_cipherkey.value_data,
             auth_cs_gprs_cipherkey.value_length);
      memscpy(cs_ciph_info->integrity_key, RRC_MAX_INTEGRITY_KEY_LENGTH, 
             auth_cs_gprs_integritykey.value_data, auth_cs_gprs_integritykey.value_length);
      cs_ciph_info->cipher_algo = (auth_ps_ciphering_algorithm != 0xff) ?
             (rrc_cipher_algo_e_type)auth_ps_ciphering_algorithm : RRC_KASUMI_ALGORITHM;

      ps_ciph_info =
         &rrc_sim_inserted_req_p->cipher_info[1];

      ps_ciph_info->domain_id = RRC_PS_DOMAIN_CN_ID;

      memscpy(ps_ciph_info->ciphering_key, RRC_MAX_CIPHERING_KEY_LENGTH, auth_ps_gprs_cipherkey.value_data,
             auth_ps_gprs_cipherkey.value_length);
      memscpy(ps_ciph_info->integrity_key, RRC_MAX_INTEGRITY_KEY_LENGTH, 
             auth_ps_gprs_integritykey.value_data, auth_ps_gprs_integritykey.value_length);
      ps_ciph_info->cipher_algo = (auth_ps_ciphering_algorithm != 0xff) ?
             (rrc_cipher_algo_e_type)auth_ps_ciphering_algorithm : RRC_KASUMI_ALGORITHM;

      if (mm_sim_card_mode == MMGSDI_APP_SIM)
      {
         memset((void*)cs_ciph_info->hfn, 0x00, 3);
         memset((void*)ps_ciph_info->hfn, 0x00, 3);
      }
      else
      {
        if (mm_sim_card_mode == MMGSDI_APP_USIM)
        {
          auth_get_cs_hfn(cs_ciph_info->hfn);
          auth_get_ps_hfn(ps_ciph_info->hfn);
        }
        else
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unknown SIM card mode",0,0,0) ;
        }
      }

      rrc_sim_inserted_req_p->admin_data = mm_ms_opmode;

      send_cmd_to_rrc( rrc_cmd, rat );
   }
#endif /* FEATURE_WCDMA */
} /* end mm_send_rrc_sim_inserted_req() */
#if defined FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION MM_SEND_RRC_mode_pref_change_ind

DESCRIPTION
  This function notifies mode preference change to RRC, when SIM is not available

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mode_pref_change_ind_to_rrc(void)
{
#ifdef FEATURE_WCDMA   

   rrc_cmd_type *rrc_cmd = NULL;

#ifdef FEATURE_SEGMENT_LOADING
   if (mm_wcdma_supported)
   {
#endif       
      rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );
      if (rrc_cmd == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_MODE_PREF_CHNAGE_IND", 0, 0, 0);
      }
      else
      {
        rrc_cmd->cmd_hdr.cmd_id = RRC_MODE_PREF_CHANGE_IND;
        send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
      }
#ifdef FEATURE_SEGMENT_LOADING
   }
#endif

#ifdef FEATURE_SEGMENT_LOADING
   if (mm_tdscdma_supported)
   {
#endif /* FEATURE_SEGMENT_LOADING */

#ifdef FEATURE_TDSCDMA
      rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );
  
      if (rrc_cmd == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending TDSRRC_MODE_PREF_CHNAGE_IND", 0, 0, 0);
      }
      else
      {
        rrc_cmd->cmd_hdr.cmd_id = RRC_MODE_PREF_CHANGE_IND;
        send_cmd_to_rrc( rrc_cmd, SYS_RAT_TDS_RADIO_ACCESS );
      }
#endif /* FEATURE_TDSCDMA */
#ifdef FEATURE_SEGMENT_LOADING
   }
#endif
#endif /* FEATURE_WCDMA */

}
#endif
/*===========================================================================
 
FUNCTION MM_SEND_CM_CIPHER_IND

DESCRIPTION
  This function sends a cm_cipher_ind message to the cm

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_cm_cipher_ind
(
   sys_srv_domain_e_type    cn_domain_ciphering_established,
   boolean                  cipher_domain_status
)
{
   cm_cipher_ind_s_type cm_cipher_ind;
   static boolean cs_cipher_status = FALSE;
   static boolean ps_cipher_status = FALSE;

   memset(&cm_cipher_ind, 0x00, sizeof(cm_cipher_ind_s_type));
   if (((cn_domain_ciphering_established == SYS_SRV_DOMAIN_CS_ONLY) &&
        (cs_cipher_status == cipher_domain_status)) ||
       ((cn_domain_ciphering_established == SYS_SRV_DOMAIN_PS_ONLY) &&
        (ps_cipher_status == cipher_domain_status)))
   {
     /* Filter duplicate indication */
     return;
   }
   else if (cn_domain_ciphering_established == SYS_SRV_DOMAIN_CS_ONLY)
   {
     cs_cipher_status = cipher_domain_status;
   }
   else if (cn_domain_ciphering_established == SYS_SRV_DOMAIN_PS_ONLY)
   {
     ps_cipher_status = cipher_domain_status;
   }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   cm_cipher_ind.message_header.message_set = MS_CM_MM;

   cm_cipher_ind.message_header.message_id = (int) CM_CIPHER_IND;

   cm_cipher_ind.cipher_domain = cn_domain_ciphering_established;

   cm_cipher_ind.cipher_status = cipher_domain_status;

   PUT_IMH_LEN( sizeof(cm_cipher_ind_s_type) - sizeof(IMH_T),
                &cm_cipher_ind.message_header );

   MSG_HIGH_DS_2(MM_SUB,"=MM= MM sent CM_CIPHER_IND for domain %d status %d", cm_cipher_ind.cipher_domain,cm_cipher_ind.cipher_status);
   mm_send_message( (IMH_T *)&cm_cipher_ind, GS_QUEUE_CM);

} /* end mm_send_cm_cipher_ind */ 



/*===========================================================================
 
FUNCTION MM_SEND_CM_BEARER_CTXT_TRANSFER_IND

DESCRIPTION
  This function sends a CM_BEARER_CTXT_TRANSFER_IND message to the cm

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#if defined(FEATURE_LTE)

void mm_send_cm_bearer_ctxt_transfer_ind
(
   sys_radio_access_tech_e_type    rat
)
{
   cm_bearer_ctxt_transfer_ind_s_type cm_bearer_ctxt_transfer_ind;

  memset(&cm_bearer_ctxt_transfer_ind, 0, sizeof(cm_bearer_ctxt_transfer_ind_s_type));
   cm_bearer_ctxt_transfer_ind.message_header.message_set = MS_CM_MM;
   cm_bearer_ctxt_transfer_ind.message_header.message_id = (int) CM_BEARER_CTXT_TRANSFER_IND;

   cm_bearer_ctxt_transfer_ind.rat = rat;

   PUT_IMH_LEN( sizeof(cm_bearer_ctxt_transfer_ind_s_type) - sizeof(IMH_T),
                &cm_bearer_ctxt_transfer_ind.message_header );

   MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent CM_BEARER_CTXT_TRANSFER_IND rat %d", cm_bearer_ctxt_transfer_ind.rat);
   mm_send_message( (IMH_T *)&cm_bearer_ctxt_transfer_ind, GS_QUEUE_CM);

} /* end mm_send_cm_bearer_ctxt_transfer_ind */ 

#endif
/*===========================================================================
 
FUNCTION MM_SEND_CM_PS_DATA_SUSPEND_IND

DESCRIPTION
  This function sends ps data suspend indication to the cm

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_cm_ps_data_suspend_ind
(
 void
)
{

   cm_mm_ps_data_suspend_ind_s_type message;
 
   memset(&message, 0x00, sizeof(cm_mm_ps_data_suspend_ind_s_type));
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   message.message_header.message_set = MS_CM_MM;

   message.message_header.message_id = (int) CM_MM_PS_DATA_SUSPEND_IND;

   PUT_IMH_LEN( sizeof(cm_mm_ps_data_suspend_ind_s_type) - sizeof(IMH_T),
                &message.message_header );

   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent CM_MM_PS_DATA_SUSPEND_IND ");
   mm_send_message( &message.message_header, GS_QUEUE_CM);

}
/*===========================================================================
 
FUNCTION MM_SEND_CM_MT_CALL_FAIL_IND

DESCRIPTION
  This function sends MT Page fail cause to CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_cm_mt_call_fail_ind
(
 sys_modem_mt_call_fail_cause_type cause
)
{

  cm_mm_mt_call_fail_ind_s_type message;
  memset(&message, 0, sizeof(cm_mm_mt_call_fail_ind_s_type));
  message.message_header.message_set=MS_CM_MM;
  message.message_header.message_id = CM_MT_CALL_PAGE_FAIL_IND;
  message.mt_call_fail_cause = cause;
  message.rat = mm_serving_plmn.info.active_rat;

#ifdef FEATURE_WRLF_SYSTEM_SEL
  if(mm_wcdma_rlf_state == WCDMA_RLF_STARTED)
  {
    message.rat = SYS_RAT_UMTS_RADIO_ACCESS;
  }
#endif
  if(message.rat == SYS_RAT_UMTS_RADIO_ACCESS)
  {
    message.paging_cause = mm_last_paging_cause;
  }
  else
  {
    message.paging_cause = RRC_PAGE_CAUSE_UNKNOWN;
  }
  

  PUT_IMH_LEN( sizeof(cm_mm_mt_call_fail_ind_s_type) - sizeof(IMH_T),
                &message.message_header );
  MSG_HIGH_DS_2(MM_SUB,"=MM= MM sent MT_CALL_FAIL_IND with failure cause %d and paging cause %d",message.mt_call_fail_cause, message.paging_cause);
  mm_send_message( &message.message_header, GS_QUEUE_CM);
}
/*===========================================================================
 
FUNCTION MM_SEND_MMR_CELL_SERVICE_IND

DESCRIPTION
  This function sends cell service ind to CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_cell_service_ind(void)
{
  mmr_cell_service_ind_s_type cell_service_ind;
  cell_service_ind.message_header.message_set = MS_MM_REG;
  cell_service_ind.message_header.message_id  = (int)MMR_CELL_SERVICE_IND;

  PUT_IMH_LEN( sizeof( mmr_cell_service_ind_s_type ) - sizeof(IMH_T),
               &cell_service_ind.message_header ); 

  cell_service_ind.egprs_support = SYS_EGPRS_SUPPORT_NOT_AVAIL;  
  cell_service_ind.dtm_support = SYS_DTM_SUPPORT_NOT_AVAIL;
  cell_service_ind.hsdpa_hsupa_support = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
  
  if(is_gsm_mode())
  {
    if(mm_serving_plmn.egprs_supported)
    {
      cell_service_ind.egprs_support = SYS_EGPRS_SUPPORT_AVAIL;    
    }
#ifdef FEATURE_GSM_DTM
    if(mm_serving_plmn.dtm_supported)
    {
      cell_service_ind.dtm_support = SYS_DTM_SUPPORT_AVAIL;
    }
#endif
  }
  else 
  {
    cell_service_ind.hsdpa_hsupa_support = mm_serving_plmn.hsdpa_hsupa_ind;
  }

  mm_send_message( (IMH_T *)&cell_service_ind, GS_QUEUE_REG );
    
  MSG_HIGH_DS_3(MM_SUB, "=MM= MM sent MMR_CELL_SERVICE_IND: hsdpa_supp=%d, dtm_supp=%d, egprs_supp=%d",
                           cell_service_ind.hsdpa_hsupa_support,
                           cell_service_ind.dtm_support,
                           cell_service_ind.egprs_support);
  
  mm_mmr_cell_service_ind_pending = FALSE;

}

#ifdef FEATURE_HSPA_CALL_STATUS_IND
/*===========================================================================

FUNCTION MM_SEND_MMR_HSPA_CALL_STATUS_IND

DESCRIPTION
  This function sends hspa call status ind to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_hspa_call_status_ind(mmr_hspa_call_status_ind_s_type* hspa_call_status_ind)
{
  hspa_call_status_ind->message_header.message_set = MS_MM_REG;
  hspa_call_status_ind->message_header.message_id  = (int)MMR_HSPA_CALL_STATUS_IND;

  PUT_IMH_LEN( sizeof( mmr_hspa_call_status_ind_s_type ) - sizeof(IMH_T),
               &hspa_call_status_ind->message_header );

  MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent MMR_HSPA_CALL_STATUS_IND.");
  mm_send_message( (IMH_T *)hspa_call_status_ind, GS_QUEUE_REG );

}
#endif

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
void mm_send_mmr_conn_mode_service_status_ind(mmr_conn_mode_service_status_ind_s_type* conn_mode_service_status_ind)
{
  conn_mode_service_status_ind->message_header.message_set = MS_MM_REG;
  conn_mode_service_status_ind->message_header.message_id = (int)MMR_CONN_MODE_SERVICE_STATUS_IND;

  PUT_IMH_LEN( sizeof( mmr_conn_mode_service_status_ind_s_type ) - sizeof(IMH_T),
               &conn_mode_service_status_ind->message_header );

  MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent MMR_CONN_MODE_SERVICE_STATUS_IND.");
  mm_send_message( (IMH_T *)conn_mode_service_status_ind, GS_QUEUE_REG );
}
#endif

/*===========================================================================

FUNCTION MM_SEND_MMR_REG_REJECT_IND

DESCRIPTION
  This function sends a mmr_reg_reject_ind to REG upon reception of certain reject causes: #2,#3,#6,#11,#12,#13. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_reg_reject_ind
(
  sys_srv_domain_e_type                               reject_domain,
  byte                                                reject_cause,
  sys_plmn_id_s_type                                  plmn,
  sys_lac_type                                        tac
#ifdef FEATURE_FEMTO_CSG
  ,sys_csg_id_type                                    csg_id
#endif 
  ,sys_radio_access_tech_e_type     reject_rat
)
{
  mmr_reg_reject_ind_s_type reg_reject_ind;

  reg_reject_ind.message_header.message_set = MS_MM_REG;
  reg_reject_ind.message_header.message_id  = (int)MMR_REG_REJECT_IND;

  reg_reject_ind.reject_domain = reject_domain;
  reg_reject_ind.reject_cause = reject_cause;
  reg_reject_ind.reject_rat = reject_rat;
  reg_reject_ind.plmn = plmn;
  reg_reject_ind.tac = tac;
#ifdef FEATURE_FEMTO_CSG
  reg_reject_ind.csg_id = csg_id;
  if((reject_cause == NATIONAL_ROAMING_NOT_ALLOWED) ||
     (reject_cause == NO_SUITABLE_CELLS_IN_LA))
  {
    mm_add_csg_to_rejected_list(plmn, csg_id, reject_cause);
  }
#endif 
   PUT_IMH_LEN( sizeof( mmr_reg_reject_ind_s_type ) - sizeof(IMH_T),
                            &reg_reject_ind.message_header );

   MSG_HIGH_DS_2(MM_SUB, "=MM= MM sent MMR_REG_REJECT_IND  REG_DOMAIN : %d, REJ_CAUSE : %d",reg_reject_ind.reject_domain,
             reg_reject_ind.reject_cause);

   mm_send_message( (IMH_T *)&reg_reject_ind, GS_QUEUE_REG );  
}

/*===========================================================================
 
FUNCTION MM_SEND_NW_SEL_MODE_RESET_IND

DESCRIPTION
  This function sends NW_SEL_MODE_RESET_IND to RR/RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_nw_sel_mode_reset_ind
(
  sys_network_selection_mode_e_type network_selection_mode ,
  byte msg_id
)
{
   if((((mmcoord_mmr_reg_cnf.service_state.service_status == SYS_SRV_STATUS_SRV) ||
       ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ) &&
        (last_requested_network_selection_mode !=SYS_NETWORK_SELECTION_MODE_MANUAL)) ||
       ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
        (last_requested_network_selection_mode !=SYS_NETWORK_SELECTION_MODE_AUTOMATIC)))&&
        (mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ)) ||
        (msg_id == (int)MMR_NET_SEL_MODE_CHANGE_IND))
   {
     rr_nw_sel_mode_reset_ind_T reset_ind; 
     memset(&reset_ind,0x00,sizeof(rr_nw_sel_mode_reset_ind_T));

#ifdef FEATURE_LTE
     mm_emm_store_reg_req_nw_selection_mode(network_selection_mode);
#else
     mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode = reset_ind.network_select_mode;
#endif

#ifdef FEATURE_WCDMA
     if (mm_is_multimode_sub())
     {
         rrc_cmd_type *rrc_cmd = NULL;
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifdef FEATURE_SEGMENT_LOADING
         if (!mm_wcdma_supported)
         {
            return;
         }
#endif /* FEATURE_SEGMENT_LOADING*/
         rrc_cmd =  mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );
         if (rrc_cmd == NULL)
         {
           MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
         }         
         rrc_cmd->cmd_hdr.cmd_id = RRC_NW_SEL_MODE_RESET_IND;
         rrc_cmd->cmd.rrc_nw_sel_mode_reset_ind.network_select_mode = network_selection_mode;  
         send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
#else /* FEATURE_RAT_PRIORITY_LIST*/

         /* Broadcast the message to T and W, if present */
         /* If a RAT is present in the rat prio list, its module must have been loaded */
         if (mm_checkif_rat_pri_list_has_rat( &mm_rat_pri_list_info, SYS_RAT_TDS_RADIO_ACCESS ))
         {
           rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );         
           if (rrc_cmd == NULL)
           {
               MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
           }
           rrc_cmd->cmd_hdr.cmd_id = RRC_NW_SEL_MODE_RESET_IND;
           rrc_cmd->cmd.rrc_nw_sel_mode_reset_ind.network_select_mode = network_selection_mode;              
           send_cmd_to_rrc( rrc_cmd, SYS_RAT_TDS_RADIO_ACCESS );
         }
         if (mm_checkif_rat_pri_list_has_rat( &mm_rat_pri_list_info, SYS_RAT_UMTS_RADIO_ACCESS ))
         {
           rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );         
           if (rrc_cmd == NULL)
           {
               MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
           }
           rrc_cmd->cmd_hdr.cmd_id = RRC_NW_SEL_MODE_RESET_IND;
           rrc_cmd->cmd.rrc_nw_sel_mode_reset_ind.network_select_mode = network_selection_mode;                 
           send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
         }      
#endif  /* FEATURE_RAT_PRIORITY_LIST*/
     }
#endif
#if (defined(FEATURE_DUAL_SIM) && !defined(FEATURE_TWO_3GPP_SIM)) || defined(FEATURE_SGLTE)
     if (mm_is_gsm_allowed_on_sub())
#endif
     {
       reset_ind.message_header.message_set = MS_MM_RR;
       reset_ind.message_header.message_id  = (int)RR_NW_SEL_MODE_RESET_IND;
       reset_ind.network_select_mode = network_selection_mode;

       PUT_IMH_LEN( sizeof( rr_nw_sel_mode_reset_ind_T ) - sizeof(IMH_T),
                 &reset_ind.message_header );
       MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RR_NW_SEL_MODE_RESET_IND");

       mm_send_message( (IMH_T *)&reset_ind, GS_QUEUE_RR );
     }
   }
   return;
}

/*===========================================================================

FUNCTION MM_SEND_MMR_PH_STATUS_CHANGE_CNF

DESCRIPTION
  This function sends a MMR_PH_STATUS_CHANGE_CNF message to the REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_ph_status_change_cnf( void )
{
   mmr_ph_status_change_cnf_s_type  mmr_ph_status_change_cnf;

   mmr_ph_status_change_cnf.message_header.message_set = MS_MM_REG;
   mmr_ph_status_change_cnf.message_header.message_id  = (int)MMR_PH_STATUS_CHANGE_CNF;
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof( mmr_ph_status_change_cnf_s_type) - sizeof(IMH_T),
                          &mmr_ph_status_change_cnf.message_header );
   /*lint -restore */
   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMR_PH_STATUS_CHANGE_CNF");
   mm_send_message( (IMH_T *)&mmr_ph_status_change_cnf, GS_QUEUE_REG );

   mm_waiting_for_ph_status_change_cnf = FALSE;
}

#ifdef FEATURE_OOSC_USER_ACTION
/*===========================================================================

FUNCTION mm_send_rrc_exit_from_pwr_save

DESCRIPTION
  This function notifies UI key press to RRC as part of UOOS Connected mode UI Update

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_exit_from_pwr_save( void )
{
#ifdef FEATURE_WCDMA
   rrc_cmd_type *rrc_cmd = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
   if (rrc_cmd == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_EXIT_FROM_PWR_SAVE", 0, 0, 0);
   }
   else
   {
      rrc_cmd->cmd_hdr.cmd_id = RRC_EXIT_FROM_PWR_SAVE;

      send_cmd_to_rrc( rrc_cmd, mm_serving_plmn.info.active_rat );
   }
#endif
} /* end mm_send_rrc_exit_from_pwr_save() */
#endif

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION mm_send_ds_stat_chgd_ind

DESCRIPTION
  This function sends change in DUAL STANDBY STATUS to RR(C)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_ds_stat_chgd_ind( mm_cmd_type *message )
{

  boolean dual_standby_mode;
  sys_modem_dual_standby_pref_e_type dual_standby_mode_info;
  sys_modem_device_mode_e_type   device_mode;

#if defined FEATURE_WCDMA || defined FEATURE_TDSCDMA
   rrc_cmd_type *rrc_cmd = NULL;
#endif

#if defined(FEATURE_SGLTE_DUAL_SIM) || defined (FEATURE_SVLTE_DUAL_SIM)
  sys_specialization_t  specialization = SYS_SPECIAL_NONE;
  policyman_status_t  status = POLICYMAN_STATUS_ERR;
  policyman_item_t    *pItem;
  policyman_item_id_t ids[] = {POLICYMAN_ITEM_DEVICE_CONFIGURATION};
#endif
   rr_ds_status_change_ind_T    rr_ds_status_change_ind;

#if defined(FEATURE_SGLTE_DUAL_SIM) || defined (FEATURE_SVLTE_DUAL_SIM)
  status = policyman_get_items(ids, ARR_SIZE(ids), (policyman_item_t const **) &pItem);    

  if (POLICYMAN_SUCCEEDED(status))
  {
     status = policyman_device_config_get_specialization(pItem, &specialization);
     policyman_item_release(pItem);
  }
#endif    

   dual_standby_mode = message->cmd.mmr_ds_stat_chgd_req.dual_standby_mode;
   dual_standby_mode_info = message->cmd.mmr_ds_stat_chgd_req.dual_standby_mode_info;
   device_mode = message->cmd.mmr_ds_stat_chgd_req.device_mode;

   rr_ds_status_change_ind.message_header.message_set = MS_MM_RR;
   rr_ds_status_change_ind.message_header.message_id  = (int)RR_DS_STATUS_CHANGE_IND;
   rr_ds_status_change_ind.device_mode = device_mode;

   rr_ds_status_change_ind.multi_standby_mode_pref = dual_standby_mode_info;

   PUT_IMH_LEN( (sizeof(rr_ds_status_change_ind_T) -sizeof(IMH_T)),
                 &rr_ds_status_change_ind.message_header );

   MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RR_DS_STATUS_CHANGE_IND");

   mm_send_message( &rr_ds_status_change_ind.message_header, GS_QUEUE_RR );

#ifdef FEATURE_WCDMA
#ifdef FEATURE_SEGMENT_LOADING
  if (get_wcdma_interface() != NULL)
  {   
#endif
   rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );
   
   if (rrc_cmd == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_DS_STATUS_CHANGE_IND", 0, 0, 0);
   }
   else
   {
     rrc_cmd->cmd_hdr.cmd_id = RRC_DS_STATUS_CHANGE_IND;
     rrc_cmd->cmd.rrc_ds_status_change_ind.dual_standby_mode = dual_standby_mode;
     rrc_cmd->cmd.rrc_ds_status_change_ind.dual_standby_mode_info = dual_standby_mode_info;
     rrc_cmd->cmd.rrc_ds_status_change_ind.device_mode = device_mode;
#if defined(FEATURE_SGLTE_DUAL_SIM) || defined (FEATURE_SVLTE_DUAL_SIM)
     if((POLICYMAN_SUCCEEDED(status)) && (specialization == SYS_SPECIAL_DSDS_ROAM))
     {
       rrc_cmd->cmd.rrc_ds_status_change_ind.device_mode = SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY;
       MSG_HIGH_DS_0(MM_SUB, "=MM= DSDS roaming specialization, indicating device mode as DSDS to WRRC");
     }
#endif
     send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
   }
#ifdef FEATURE_SEGMENT_LOADING
   }
#endif
#endif
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
  if (mm_tdscdma_supported)
  {   
#endif
   rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );

   if (rrc_cmd == NULL)
   {
     MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending TDSRRC_DS_STATUS_CHANGE_IND", 0, 0, 0);
   }
   else
   {
     rrc_cmd->cmd_hdr.cmd_id = RRC_DS_STATUS_CHANGE_IND;
     rrc_cmd->cmd.rrc_ds_status_change_ind.dual_standby_mode = dual_standby_mode;
     rrc_cmd->cmd.rrc_ds_status_change_ind.dual_standby_mode_info = dual_standby_mode_info;
     rrc_cmd->cmd.rrc_ds_status_change_ind.device_mode = device_mode;
     send_cmd_to_rrc( rrc_cmd, SYS_RAT_TDS_RADIO_ACCESS );
   }
#ifdef FEATURE_SEGMENT_LOADING
   }
#endif
#endif
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE)
  emm_send_rrc_ds_stat_change_req(dual_standby_mode,dual_standby_mode_info,device_mode);
#endif 

}

/*===========================================================================

FUNCTION MM_SEND_MMR_DS_STAT_CHGD_CNF

DESCRIPTION
  This function sends a MMR_DS_STAT_CHGD_CNF message to the REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_ds_stat_chgd_cnf( void )
{
   mmr_ds_stat_chgd_cnf_s_type  mmr_ds_stat_chgd_cnf;

   mmr_ds_stat_chgd_cnf.message_header.message_set = MS_MM_REG;
   mmr_ds_stat_chgd_cnf.message_header.message_id  = (int)MMR_DS_STAT_CHGD_CNF;

   PUT_IMH_LEN( sizeof( mmr_ds_stat_chgd_cnf_s_type) - sizeof(IMH_T),
                          &mmr_ds_stat_chgd_cnf.message_header );

   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMR_DS_STAT_CHGD_CNF");
   mm_send_message( &mmr_ds_stat_chgd_cnf.message_header, GS_QUEUE_REG );
}

/*===========================================================================

FUNCTION mm_send_rr_multimode_subs_chgd_req

DESCRIPTION
  This function sends subs_capability to RR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_multimode_subs_chgd_req
(
subs_capability_e_type*  subs_capability
)
{
    sys_modem_as_id_e_type as_id;
    rr_multimode_subs_chgd_req_T    rr_multimode_subs_chgd_req;

    rr_multimode_subs_chgd_req.message_header.message_set = MS_MM_RR;
    rr_multimode_subs_chgd_req.message_header.message_id  = (int)RR_MULTIMODE_SUBS_CHGD_REQ ;

    for(as_id = SYS_MODEM_AS_ID_1; as_id < MAX_AS_IDS;as_id++)
    {
     rr_multimode_subs_chgd_req.subs_capability[as_id] = subs_capability[as_id];
    }

    PUT_IMH_LEN( (sizeof(rr_multimode_subs_chgd_req_T) -sizeof(IMH_T)),
                 &rr_multimode_subs_chgd_req.message_header );

    MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RR_MULTIMODE_SUBS_CHGD_REQ");

    mm_send_message( &rr_multimode_subs_chgd_req.message_header, GS_QUEUE_RR );
}

/*===========================================================================

FUNCTION mm_send_rr_multimode_subs_chgd_req

DESCRIPTION
  This function sends subs_capability to RR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_multimode_subs_chgd_ind
(
  subs_capability_e_type*  subs_capability
)
{

#ifdef FEATURE_TDSCDMA
sys_modem_as_id_e_type as_id_td;
rrc_cmd_type *rrc_cmd_td = NULL;
#endif
#ifdef FEATURE_SEGMENT_LOADING
 if (!(mm_wcdma_supported || mm_tdscdma_supported))
 {
     /* W not loaded */
     return;
 }            
#endif /* FEATURE_SEGMENT_LOADING */

#ifdef FEATURE_WCDMA
sys_modem_as_id_e_type as_id;
rrc_cmd_type *rrc_cmd = NULL;

if(mm_wcdma_supported)
{
  rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );

  if (rrc_cmd == NULL)
  {
     MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_MULTIMODE_SUBS_CHGD_IND", 0, 0, 0);
  }
  else
  {
     rrc_cmd->cmd_hdr.cmd_id = RRC_MULTIMODE_SUBS_CHGD_IND;
     for(as_id = SYS_MODEM_AS_ID_1; as_id < MAX_AS_IDS;as_id++)
     {
       rrc_cmd->cmd.rrc_multimode_subs_chgd_ind.subs_capability[as_id] = subs_capability[as_id];
     }
     send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
  }
}
#endif

#ifdef FEATURE_TDSCDMA
if(mm_tdscdma_supported)
{

  rrc_cmd_td= mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );

  if (rrc_cmd_td== NULL)
  {
     MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_MULTIMODE_SUBS_CHGD_IND", 0, 0, 0);
  }
  else
  {
    rrc_cmd_td->cmd_hdr.cmd_id = RRC_MULTIMODE_SUBS_CHGD_IND;
    /*As TDS-RRC doesn't support triple sim as of now, so filling subs_capability as None for 3rd sub*/
    for(as_id_td= SYS_MODEM_AS_ID_1; as_id_td< MAX_AS_IDS;as_id_td++)
    {
      rrc_cmd_td->cmd.rrc_multimode_subs_chgd_ind.subs_capability[as_id_td] = subs_capability[as_id_td];
    }
#ifdef FEATURE_TRIPLE_SIM
    rrc_cmd_td->cmd.rrc_multimode_subs_chgd_ind.subs_capability[as_id_td] = SUBS_CAPABILITY_NONE;
#endif
    send_cmd_to_rrc( rrc_cmd_td, SYS_RAT_TDS_RADIO_ACCESS );
  }
}
#endif
}
/*===========================================================================

FUNCTION mm_send_mmr_multimode_subs_chgd_cnf

DESCRIPTION
  This function sends a MMR_DS_STAT_CHGD_CNF message to the REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_mmr_multimode_subs_chgd_cnf( void )
{
   mmr_multimode_subs_chgd_cnf_s_type  mmr_multimode_subs_chgd_cnf;

   mmr_multimode_subs_chgd_cnf.message_header.message_set = MS_MM_REG;
   mmr_multimode_subs_chgd_cnf.message_header.message_id  = (int)MMR_MULTIMODE_SUBS_CHGD_CNF;

   PUT_IMH_LEN( sizeof( mmr_multimode_subs_chgd_cnf_s_type) - sizeof(IMH_T),
                          &mmr_multimode_subs_chgd_cnf.message_header );

   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMR_MULTIMODE_SUBS_CHGD_CNF");
   mm_send_message( &mmr_multimode_subs_chgd_cnf.message_header, GS_QUEUE_REG );
}
#endif

#ifdef FEATURE_WCDMA
#if defined(FEATURE_DUAL_SIM)
/*===========================================================================

FUNCTION mm_send_rrc_ds_tuneaway_status_change_ind

DESCRIPTION
  This function sends change in DS TUNEAWAY STATUS CHANGE to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_ds_tuneaway_status_change_ind
(
  const boolean tuneaway_status
)
{
   rrc_cmd_type *rrc_cmd = NULL;

#ifdef FEATURE_DUAL_SIM
  gmm_cm_ds_tuneaway_status = tuneaway_status;
#endif

#ifdef FEATURE_SEGMENT_LOADING
  if (mm_wcdma_supported)
  {  
#endif
   rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );

   if (rrc_cmd == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_DS_TUNEAWAY_STATUS_CHANGE_IND", 0, 0, 0);
   }
   else
   {
      rrc_cmd->cmd_hdr.cmd_id = RRC_DS_TUNEAWAY_STATUS_CHANGE_IND;
      rrc_cmd->cmd.rrc_ds_tuneaway_status_change_ind.tuneaway_status = tuneaway_status;
      send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
   }
#ifdef FEATURE_SEGMENT_LOADING
   }
#endif
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
  if (mm_tdscdma_supported)
  {  
#endif
   rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );

   if (rrc_cmd == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending TDSRRC_DS_TUNEAWAY_STATUS_CHANGE_IND", 0, 0, 0);
   }
   else
   {
      rrc_cmd->cmd_hdr.cmd_id = RRC_DS_TUNEAWAY_STATUS_CHANGE_IND;
      rrc_cmd->cmd.rrc_ds_tuneaway_status_change_ind.tuneaway_status = tuneaway_status;
      send_cmd_to_rrc( rrc_cmd, SYS_RAT_TDS_RADIO_ACCESS );
   }
#ifdef FEATURE_SEGMENT_LOADING
   }
#endif
#endif /* FEATURE_TDSCDMA */
}
#endif
#endif /* #ifdef FEATURE_WCDMA */

#ifdef FEATURE_WRLF_SYSTEM_SEL
/*===========================================================================

FUNCTION MM SEND RRC SUITABLE SEARCH IND

DESCRIPTION
  This function sends MMR SUITABLE SEARCH END INDICATION message to reg

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_suitable_search_end_ind(void)
  {
    mmr_suitable_search_end_ind_s_type   mmr_search_end_ind;
    mmr_search_end_ind.message_header.message_set = MS_MM_REG ;
    mmr_search_end_ind.message_header.message_id  = (int)MMR_SUITABLE_SEARCH_END_IND ;
    mm_received_suitable_search_end_ind = TRUE;

    /*lint -save -e778 */
    PUT_IMH_LEN( sizeof(mmr_suitable_search_end_ind_s_type) - sizeof(IMH_T),&mmr_search_end_ind.message_header );
    /*lint -restore */
#ifdef FEATURE_LTE
    if ( mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
    {
      mm_stop_timer(TIMER_MM_PS_DETACH);
      mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
      gmm_detach_type =GMM_GPRS_DETACH;
      gmm_handle_gprs_detach_low_level_failure();
      mm_send_mmr_ps_detach_cnf();
#ifdef FEATURE_MODEM_HEAP
      modem_mem_free(mm_pending_mmr_ps_detach_req_ptr,MODEM_MEM_CLIENT_NAS);
      mm_pending_mmr_ps_detach_req_ptr = NULL;
#else
      memset(&mm_pending_mmr_ps_detach_req,0,sizeof(mm_cmd_type));
#endif
    }
#endif
    

    if ((mm_psm_ready_req_rejected == TRUE) &&
        (mm_check_ready_for_psm() == SYS_PSM_STATUS_READY))
    {
       mm_send_mmr_psm_ready_ind();
    }

    MSG_HIGH_DS_0(MM_SUB,"=MM= sent MMR_SUITABLE_SEARCH_END_IND to REG");
    mm_send_message( &mmr_search_end_ind.message_header, GS_QUEUE_REG );
  }

/*===========================================================================

FUNCTION MM SEND  RRC DISABLE BPLMN IND

DESCRIPTION
  This function sends MMR DISABLE BPLMN INDICATION message to reg

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_disable_bplmn_ind(const boolean disable_status)
{
  mmr_disable_bplmn_ind_s_type mmr_disable_bplmn_ind;
  mmr_disable_bplmn_ind.message_header.message_set = MS_MM_REG ;
  mmr_disable_bplmn_ind.message_header.message_id  = (int)MMR_DISABLE_BPLMN_IND ;
  mmr_disable_bplmn_ind.disable_status = disable_status;
  
  /*lint -save -e778 */
  PUT_IMH_LEN( sizeof(mmr_disable_bplmn_ind_s_type) - sizeof(IMH_T),&mmr_disable_bplmn_ind.message_header );
  /*lint -restore */
  
  MSG_HIGH_DS_1(MM_SUB ,"=MM= sent MMR_DISABLE_BPLMN_IND to REG disable_status = %d", disable_status);
  mm_send_message( &mmr_disable_bplmn_ind.message_header, GS_QUEUE_REG );
}
#endif

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION MM SEND RRC CSFB CALL STATUS IND

DESCRIPTION
  This function sends RRC_CSFB_CALL_STATUS_IND message to rrc

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_csfb_call_status_ind(const boolean csfb_call_status)
{
#ifdef FEATURE_WCDMA
  rrc_cmd_type *rrc_cmd_p = NULL;   
 
#ifdef FEATURE_TDSCDMA
  if (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                       SYS_RAT_TDS_RADIO_ACCESS))
  {
      rrc_cmd_p = mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );
      if (rrc_cmd_p == NULL)
      {
          MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0, 0, 0);
      }
      rrc_cmd_p->cmd_hdr.cmd_id = RRC_CSFB_CALL_STATUS_IND;
      rrc_cmd_p->cmd.rrc_csfb_call_status_ind.csfb_call_status = csfb_call_status;
      send_cmd_to_rrc( rrc_cmd_p, SYS_RAT_TDS_RADIO_ACCESS );
  }
#endif
  if (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                       SYS_RAT_UMTS_RADIO_ACCESS))
  {
      rrc_cmd_p = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );
      if (rrc_cmd_p == NULL)
      {
          MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0, 0, 0);
      }
      rrc_cmd_p->cmd_hdr.cmd_id = RRC_CSFB_CALL_STATUS_IND;
      rrc_cmd_p->cmd.rrc_csfb_call_status_ind.csfb_call_status = csfb_call_status;
      send_cmd_to_rrc( rrc_cmd_p, SYS_RAT_UMTS_RADIO_ACCESS );
  }
#endif  
}

/*===========================================================================

FUNCTION MM_SEND_RR_CSFB_CALL_STATUS_IND

DESCRIPTION
  This function sends RR_CSFB_CALL_STATUS_IND message to rrc

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_csfb_call_status_ind(boolean csfb_call_status, boolean mt_call_orig)
{

   rr_csfb_call_status_ind_T rr_csfb_call_status_ind;
   memset(&rr_csfb_call_status_ind,0x00,sizeof(rr_csfb_call_status_ind_T));
   rr_csfb_call_status_ind.message_header.message_set = MS_MM_RR;
   rr_csfb_call_status_ind.message_header.message_id  = (int)RR_CSFB_CALL_STATUS_IND;
   rr_csfb_call_status_ind.csfb_call_status = csfb_call_status;

   rr_csfb_call_status_ind.csmt = mt_call_orig;
   
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof( rr_csfb_call_status_ind_T ) - sizeof(IMH_T),
                &rr_csfb_call_status_ind.message_header );
   /*lint -restore */
   MSG_HIGH_DS_1(MM_SUB,"=MM= MM sent RR_CSFB_CALL_STATUS_IND CSMT %d",mt_call_orig);
   
   mm_send_message((IMH_T *)&rr_csfb_call_status_ind, GS_QUEUE_RR );  
}

/*===========================================================================

FUNCTION MM_SEND_RR_MT_CSFB_FAILURE_IND

DESCRIPTION
  This function sends RR_MT_CSFB_FAILURE_IND message to GRR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_mt_csfb_failure_ind(boolean depriotize_cells)
{

   rr_mt_csfb_failure_ind_T rr_mt_csfb_failure_ind;
   memset(&rr_mt_csfb_failure_ind,0x00,sizeof(rr_mt_csfb_failure_ind_T));
   rr_mt_csfb_failure_ind.message_header.message_set = MS_MM_RR;
   rr_mt_csfb_failure_ind.message_header.message_id  = (int)RR_MT_CSFB_FAILURE_IND;
   rr_mt_csfb_failure_ind.csmt_fail_depriotize_cells = depriotize_cells;
   
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof( rr_mt_csfb_failure_ind_T ) - sizeof(IMH_T),
                &rr_mt_csfb_failure_ind.message_header );
   /*lint -restore */
   
    MSG_HIGH_DS_1(MM_SUB,"=MM= MM sent RR_MT_CSFB_FAILURE_IND cell_depriotize = %d",depriotize_cells);
   
   mm_send_message((IMH_T *)&rr_mt_csfb_failure_ind, GS_QUEUE_RR );  
}

#endif
/*===========================================================================

FUNCTION mm_send_mmr_cell_access_ind

DESCRIPTION
  This function sends CS and PS cell access information to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_cell_access_ind( void )
{
  
  mmr_cell_access_ind_s_type cell_access_ind;
  
  cell_access_ind.message_header.message_set = MS_MM_REG;
  cell_access_ind.message_header.message_id  = (int)MMR_CELL_ACCESS_IND;
  
  cell_access_ind.cell_access_status.cs_bar_status = mm_convert_to_reg_access_info(mm_get_cs_cell_access(PROCEDURE_NONE));
  cell_access_ind.cell_access_status.ps_bar_status = mm_convert_to_reg_access_info(gmm_get_ps_cell_access(PROCEDURE_NONE));

  PUT_IMH_LEN( sizeof( mmr_cell_access_ind_s_type ) - sizeof(IMH_T),
                                                      &cell_access_ind.message_header );
  
  MSG_HIGH_DS_2(MM_SUB,"MM sent MMR_CELL_ACCESS_IND  CS_BAR_STATUS : %d, PS_BAR_STATUS : %d",mm_get_cs_cell_access(PROCEDURE_NONE),gmm_get_ps_cell_access(PROCEDURE_NONE));
 
  mm_send_message( (IMH_T *)&cell_access_ind, GS_QUEUE_REG );
} /* end  mm_send_mmr_cell_access_ind() */

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION MM SEND MMR PS DETACH CNF

DESCRIPTION
  This function sends MMR_PS_DETACH_CNF message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_ps_detach_cnf(void)
  {
    mmr_ps_detach_cnf_s_type mmr_ps_detach_cnf;
    mmr_ps_detach_cnf.message_header.message_set = MS_MM_REG ;
    mmr_ps_detach_cnf.message_header.message_id  = (int)MMR_PS_DETACH_CNF ;

    /*lint -save -e778 */
    PUT_IMH_LEN( sizeof(mmr_ps_detach_cnf_s_type) - sizeof(IMH_T),&mmr_ps_detach_cnf.message_header );
    /*lint -restore */

#ifdef FEATURE_DUAL_SIM
    gmm_force_local_detach_sim[mm_sub_id] = FALSE;
#endif

    MSG_HIGH_DS_0(MM_SUB,"=MM= sent MMR_PS_DETACH_CNF to REG");
    mm_send_message( &mmr_ps_detach_cnf.message_header, GS_QUEUE_REG );
  }

/*===========================================================================

FUNCTION MM SEND FPLMN IND

DESCRIPTION
  This function sends MMR_FPLMN_LIST_CHANGE_IND message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_fplmn_list_change_ind(sys_forbidden_list_type_e_type  list_type)
  {
    mmr_fplmn_list_change_ind_s_type mmr_fplmn_change_ind;
    mmr_fplmn_change_ind.message_header.message_set = MS_MM_REG ;
    mmr_fplmn_change_ind.message_header.message_id  = (int)MMR_FPLMN_LIST_CHANGE_IND;
    mmr_fplmn_change_ind.list_type = list_type; 
    /*lint -save -e778 */
    PUT_IMH_LEN( sizeof(mmr_fplmn_list_change_ind_s_type) - sizeof(IMH_T),&mmr_fplmn_change_ind.message_header );
    /*lint -restore */

    MSG_HIGH_DS_0(MM_SUB,"=MM= sent MMR_FPLMN_LIST_CHANGE_IND to REG");
    mm_send_message( (IMH_T *)&mmr_fplmn_change_ind, GS_QUEUE_REG );
  }

/*===========================================================================

FUNCTION MM_SEND_MMR_UPDATE_CURR_RAT_LIST_IND

DESCRIPTION
  This function sends MMR_UPDATE_CURR_RAT_LIST_IND message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_update_curr_rat_list_ind(sys_rat_pri_list_info_s_type *rat_pri_list)
{
  mmr_update_curr_rat_list_ind_type mmr_update_curr_rat_list_ind;
  mmr_update_curr_rat_list_ind.message_header.message_set = MS_MM_REG;
  mmr_update_curr_rat_list_ind.message_header.message_id  = (int)MMR_UPDATE_CURR_RAT_LIST_IND;
  memscpy( &mmr_update_curr_rat_list_ind.rat_pri_list_info,sizeof(sys_rat_pri_list_info_s_type),
                              rat_pri_list,sizeof(sys_rat_pri_list_info_s_type) );

  PUT_IMH_LEN( sizeof(mmr_update_curr_rat_list_ind_type) - sizeof(IMH_T),&mmr_update_curr_rat_list_ind.message_header );

  MSG_HIGH_DS_0(MM_SUB,"=MM= sent MMR_UPDATE_CURR_RAT_LIST_IND to REG");
  mm_send_message( (IMH_T *)&mmr_update_curr_rat_list_ind, GS_QUEUE_REG );
}

#endif

/*===========================================================================

FUNCTION mm_send_rrc_hard_abort_req

DESCRIPTION
  This function send the hard abort request to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_hard_abort_req( sys_radio_access_tech_e_type rat )
{

#ifdef FEATURE_WCDMA

  rrc_cmd_type *rrc_cmd_p;

#ifdef FEATURE_SEGMENT_LOADING
  if ((!mm_wcdma_supported) && (rat == SYS_RAT_UMTS_RADIO_ACCESS))
  {
      /* W not loaded */
      return;
  }
  if ((!mm_tdscdma_supported) && (rat == SYS_RAT_TDS_RADIO_ACCESS))
  {
      /* T not loaded */
      return;
  }
#endif /* FEATURE_SEGMENT_LOADING */

  rrc_cmd_p = mm_get_rrc_cmd_buf( rat );
  if (rrc_cmd_p == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
  }
  rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
  rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
  rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = FALSE;
  rrc_cmd_p->cmd.rrc_abort_req.hard_abort = TRUE;
  rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;

  send_cmd_to_rrc( rrc_cmd_p, rat );
#endif /* FEATURE_WCDMA */

} /* end mm_send_rrc_hard_abort_req() */

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION mm_send_rrc_lte_reselection_status_ind

DESCRIPTION
  This function send LTE reselection enable/disable indicaiton to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_lte_reselection_status_ind( void )
{
#ifdef FEATURE_WCDMA
  rrc_cmd_type *rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );

  if (rrc_cmd_p == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
  }

  rrc_cmd_p->cmd_hdr.cmd_id = RRC_LTE_RESELECTION_ALLOWED_STATUS_IND;
  rrc_cmd_p->cmd.rrc_lte_resel_allowed_status_ind.lte_reselection_allowed = 
                                     mm_is_lte_reselection_allowed(mm_serving_plmn.info.plmn);

  send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
#endif

}

/*===========================================================================

FUNCTION MM_SEND_RR_LTE_RESELECTION_STATUS_IND

DESCRIPTION
  This function sends a rr_eq_plmn_list_change_ind to rr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_lte_reselection_status_ind( void )
{
   rr_lte_reselection_allowed_status_ind_T rr_lte_reselection_status_ind;
   memset(&rr_lte_reselection_status_ind,0x00,sizeof(rr_lte_reselection_allowed_status_ind_T));
   rr_lte_reselection_status_ind.message_header.message_set = MS_MM_RR;
   rr_lte_reselection_status_ind.message_header.message_id  = (int)RR_LTE_RESELECTION_ALLOWED_STATUS_IND;
   rr_lte_reselection_status_ind.lte_reselection_allowed = mm_is_lte_reselection_allowed(mm_serving_plmn.info.plmn);
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof( rr_lte_reselection_allowed_status_ind_T ) - sizeof(IMH_T),
                &rr_lte_reselection_status_ind.message_header );
   /*lint -restore */
   MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RR_LTE_RESELECTION_ALLOWED_STATUS_IND");
   
   mm_send_message(&rr_lte_reselection_status_ind.message_header, GS_QUEUE_RR );  
}

/*===========================================================================
FUNCTION mm_send_mmcnm_srvcc_handover_complete

DESCRIPTION
  This function send the srvcc handover complete to CNM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_mmcnm_srvcc_handover_complete(boolean first_trigger )
{
   mmcnm_srvcc_handover_complete_ind_T mmcnm_srvcc_handover_complete_ind;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   mmcnm_srvcc_handover_complete_ind.header.message_set = MS_CC_MM;
   mmcnm_srvcc_handover_complete_ind.header.message_id  = MMCNM_SRVCC_HANDOVER_COMPLETE_IND;
   mmcnm_srvcc_handover_complete_ind.active_rat=mm_serving_plmn.info.active_rat;
   mmcnm_srvcc_handover_complete_ind.first_trigger = first_trigger;

   PUT_IMH_LEN( sizeof(mmcnm_srvcc_handover_complete_ind_T) - sizeof(IMH_T), &mmcnm_srvcc_handover_complete_ind.header );

   MSG_HIGH_DS_1(MM_SUB,"=MM= MM sent MMCNM_SRVCC_HANDOVER_COMPLETE with trigger = %d" ,first_trigger);

   mm_send_message( &mmcnm_srvcc_handover_complete_ind.header, GS_QUEUE_MN_CM );
}


/*===========================================================================

FUNCTION MM SEND CM MM SRVCC HANDOVER FAILURE IND

DESCRIPTION
  This function sends CM_MM_SRVCC_HANDOVER_FAILURE_IND message to CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_cm_mm_srvcc_handover_failure_ind(void)
{
  cm_mm_srvcc_handover_failure_ind_s_type   cm_mm_srvcc_handover_failure_ind;
  memset(&cm_mm_srvcc_handover_failure_ind, 0, sizeof(cm_mm_srvcc_handover_failure_ind_s_type));
  cm_mm_srvcc_handover_failure_ind.message_header.message_set = MS_CM_MM;
  cm_mm_srvcc_handover_failure_ind.message_header.message_id  = (int)CM_MM_SRVCC_HANDOVER_FAILURE_IND;

  PUT_IMH_LEN( sizeof(cm_mm_srvcc_handover_failure_ind_s_type) - sizeof(IMH_T),
                                                    &cm_mm_srvcc_handover_failure_ind.message_header );

  MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent CM_MM_SRVCC_HANDOVER_FAILURE_IND");

  mm_send_message( &cm_mm_srvcc_handover_failure_ind.message_header, GS_QUEUE_CM);

}

/*===========================================================================

FUNCTION MM SEND MMR SET DRX CNF

DESCRIPTION
  This function sends MMR SET DRX CNF  message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_set_drx_cnf(boolean result)
{
  mmr_set_drx_cnf_s_type mmr_set_drx_cnf;
  mmr_set_drx_cnf.message_header.message_set = MS_MM_REG ;
  mmr_set_drx_cnf.message_header.message_id  = (int)MMR_SET_DRX_CNF;
  
  mmr_set_drx_cnf.result = result;
  
  /*lint -save -e778 */
  PUT_IMH_LEN( sizeof(mmr_set_drx_cnf_s_type) - sizeof(IMH_T),&mmr_set_drx_cnf.message_header );
  /*lint -restore */
  
  MSG_HIGH_DS_1(MM_SUB ,"=MM= sent MMR_SET_DRX_CNF to REG with result = %d", result);
  mm_send_message( &mmr_set_drx_cnf.message_header, GS_QUEUE_REG );
  gmm_set_drx_req_pending = FALSE;
}

/*===========================================================================

FUNCTION MM SEND MMR GET DRX CNF

DESCRIPTION
  This function sends MMR GET DRX CNF  message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_mmr_get_drx_cnf(void)
{
  mmr_get_drx_cnf_s_type mmr_get_drx_cnf;
  mmr_get_drx_cnf.message_header.message_set = MS_MM_REG ;
  mmr_get_drx_cnf.message_header.message_id  = (int)MMR_GET_DRX_CNF;
  
  mmr_get_drx_cnf.drx_coefficient = gmm_drx_cn_coeff_s1;
  
  /*lint -save -e778 */
  PUT_IMH_LEN( sizeof(mmr_get_drx_cnf_s_type) - sizeof(IMH_T),&mmr_get_drx_cnf.message_header );
  /*lint -restore */
  
  MSG_HIGH_DS_1(MM_SUB ,"=MM= sent MMR_GET_DRX_CNF to REG with DRX = %d", gmm_drx_cn_coeff_s1);
  mm_send_message( (IMH_T *)&mmr_get_drx_cnf, GS_QUEUE_REG );
}

/*===========================================================================

FUNCTION mm_send_rrc_set_drx_ind

DESCRIPTION
  This function send the drx indication to WRRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_set_drx_ind(void)
{
#ifdef FEATURE_WCDMA
   rrc_cmd_type *rrc_cmd = NULL;

#ifdef FEATURE_SEGMENT_LOADING
/* This feature is not applicable to TDSCDMA */
  if ((get_tdscdma_interface() != NULL) && (get_wcdma_interface() == NULL))
  {
     return;
  }
#endif
  if(mm_is_multimode_sub())
  {
#ifdef FEATURE_SEGMENT_LOADING
  if (mm_wcdma_supported)
  {
#endif

    rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );
    if (rrc_cmd == NULL)
    {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
    }
    rrc_cmd->cmd_hdr.cmd_id = RRC_SET_DRX_IND;
    rrc_cmd->cmd.rrc_set_drx_ind.drx_coefficient = gmm_drx_cn_coeff_s1;
    
    send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
  }
#ifdef FEATURE_SEGMENT_LOADING
  }
#endif
#endif

} 

#endif /*FEATURE_LTE*/
/*===========================================================================

FUNCTION MM_SEND_RR_CSFB_BLIND_REDIR_TO_LTE_REQ

DESCRIPTION
  This function sends a RR_CSFB_BLIND_REDIR_TO_LTE_REQ to rr. Also resets 
  mm_csfb_is_blind_redir_to_lte_req_to_be_sent global variable

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


#ifdef FEATURE_LTE
void mm_send_rr_csfb_blind_redir_to_lte_req(boolean call_aborted)
{
   rr_csfb_blind_redir_to_lte_req_T rr_csfb_blind_redir_to_lte_req;
   mm_csfb_is_blind_redir_to_lte_req_to_be_sent = FALSE;
   emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = FALSE;
   memset(&rr_csfb_blind_redir_to_lte_req,0x00,sizeof(rr_csfb_blind_redir_to_lte_req_T));
   rr_csfb_blind_redir_to_lte_req.message_header.message_set = MS_MM_RR;
   rr_csfb_blind_redir_to_lte_req.message_header.message_id  = (int)RR_CSFB_BLIND_REDIR_TO_LTE_REQ;
   rr_csfb_blind_redir_to_lte_req.call_aborted = call_aborted;
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof( rr_csfb_blind_redir_to_lte_req_T ) - sizeof(IMH_T),
                &rr_csfb_blind_redir_to_lte_req.message_header );
   /*lint -restore */
   MSG_HIGH_DS_1(MM_SUB,"=MM= MM sent RR_CSFB_BLIND_REDIR_TO_LTE_REQ call aborted = %d ",call_aborted);
   
   mm_send_message( (IMH_T *)&rr_csfb_blind_redir_to_lte_req, GS_QUEUE_RR );
}
#endif




/*===========================================================================

FUNCTION MM_SEND_RRC_LAI_REJECT_LIST_IND

DESCRIPTION
  This function notifies LAI Reject list changes to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_lai_reject_list_ind( lai_reject_list_T* list )
{
#ifdef FEATURE_NAS_FREE_FLOAT
#ifdef FEATURE_WCDMA
   rrc_cmd_type *rrc_cmd = NULL;

   if (list != NULL)
   {
     if(mm_is_multimode_sub())
     {
       if (mm_wcdma_supported)
       {
         rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );
  
         if (rrc_cmd == NULL)
         {
            MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_LAI_REJECT_LIST_IND", 0, 0, 0);
         }
         else
         {
            rrc_cmd->cmd_hdr.cmd_id = RRC_LAI_REJECT_LIST_IND;
            memscpy( &rrc_cmd->cmd.rrc_lai_reject_list_ind.nas_lai_reject_list,sizeof(lai_reject_list_T),list,sizeof(lai_reject_list_T) );
            send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
         }
       }
     }
#ifdef FEATURE_TDSCDMA
     if(mm_is_multimode_sub())
     {
       if (mm_tdscdma_supported)
       {
         rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );
  
         if (rrc_cmd == NULL)
         {
            MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending TDSRRC_LAI_REJECT_LIST_IND", 0, 0, 0);
         }
         else
         {
            rrc_cmd->cmd_hdr.cmd_id = RRC_LAI_REJECT_LIST_IND;
            memscpy( &rrc_cmd->cmd.rrc_lai_reject_list_ind.nas_lai_reject_list,sizeof(lai_reject_list_T),list,sizeof(lai_reject_list_T) );
            send_cmd_to_rrc( rrc_cmd, SYS_RAT_TDS_RADIO_ACCESS );
         }
       }
     }
#endif /* FEATURE_TDSCDMA */
   }
#endif /* FEATURE_WCDMA */
#endif /* FEATURE_NAS_FREE_FLOAT */
} /* end mm_send_rrc_lai_reject_list_ind() */


/*===========================================================================

FUNCTION MM_SEND_RRC_TUNEAWAY_ALLOWED_IND

DESCRIPTION
  This function notifies tuneaway allowed indication to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_tuneaway_allowed_ind(boolean status)
{
#ifdef FEATURE_NAS_FREE_FLOAT
#ifdef FEATURE_WCDMA
   rrc_cmd_type *rrc_cmd = NULL;
   if (mm_is_multimode_sub())
   {
     if (mm_wcdma_supported)
     {
       rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );

       if (rrc_cmd == NULL)
       {
          MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_TUNEAWAY_ALLOWED_IND", 0, 0, 0);
       }
       else
       {
          rrc_cmd->cmd_hdr.cmd_id = RRC_TUNEAWAY_ALLOWED_IND;
          rrc_cmd->cmd.rrc_sm_tuneaway_allowed_ind.sm_ok_to_tuneaway = status;
          send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
       }
     }
   }
#ifdef FEATURE_TDSCDMA
   if (mm_is_multimode_sub())
   {
     if (mm_tdscdma_supported)
     {
       rrc_cmd_type *rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );

       if (rrc_cmd == NULL)
       {
          MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_TUNEAWAY_ALLOWED_IND", 0, 0, 0);
       }
       else
       {
          rrc_cmd->cmd_hdr.cmd_id = RRC_TUNEAWAY_ALLOWED_IND;
          rrc_cmd->cmd.rrc_sm_tuneaway_allowed_ind.sm_ok_to_tuneaway = status;
          send_cmd_to_rrc( rrc_cmd, SYS_RAT_TDS_RADIO_ACCESS );
       }
     }
   }
#endif  /* FEATURE_TDSCDMA */
#endif  /* FEATURE_WCDMA */
#endif  /* FEATURE_NAS_FREE_FLOAT */
} /* end mm_send_rrc_lai_reject_list_ind() */


/*===========================================================================

FUNCTION MM_SEND_RR_LAI_REJECT_LIST_CHANGE_IND

DESCRIPTION
  This function sends LAI Reject list changes to rr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_lai_reject_list_ind( lai_reject_list_T* list )
{
#ifdef FEATURE_NAS_FREE_FLOAT
#if (defined(FEATURE_DUAL_SIM) && !defined(FEATURE_TWO_3GPP_SIM)) || defined(FEATURE_SGLTE)
   if (mm_is_gsm_allowed_on_sub())
#endif
   {
     rr_lai_reject_list_ind_T rr_lai_reject_list_ind;
     memset(&rr_lai_reject_list_ind,0x00,sizeof(rr_lai_reject_list_ind_T));
     rr_lai_reject_list_ind.message_header.message_set = MS_MM_RR;
     rr_lai_reject_list_ind.message_header.message_id  = (int)RR_LAI_REJECT_LIST_IND;
     /*lint -save -e778 */
     PUT_IMH_LEN( sizeof( rr_lai_reject_list_ind_T ) - sizeof(IMH_T),
                   &rr_lai_reject_list_ind.message_header );
     /*lint -restore */

     rr_lai_reject_list_ind.nas_lai_reject_list.no_of_entries = MIN(list ->length , MAX_NO_FORBIDDEN_LAIS);
     rr_lai_reject_list_ind.nas_lai_reject_list.no_of_entries=MIN(rr_lai_reject_list_ind.nas_lai_reject_list.no_of_entries,MAX_NO_AVAILABLE_LAIS);

     memscpy(&(rr_lai_reject_list_ind.nas_lai_reject_list.location_area_identification[0]), sizeof(rr_lai_reject_list_ind.nas_lai_reject_list.location_area_identification), &(list->lai[0]), sizeof(inter_task_lai_T)*(rr_lai_reject_list_ind.nas_lai_reject_list.no_of_entries));

     MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RR_LAI_REJECT_LIST_IND");

     mm_send_message( (IMH_T *)&rr_lai_reject_list_ind, GS_QUEUE_RR );
   }
#endif
}

/*===========================================================================

FUNCTION MM_SEND_SEND_SM_UNBLOCK_ALL_APN

DESCRIPTION
  This function sends GMMSM_UNBLOCK_ALL_APN to SM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_sm_unblock_all_apn(void)
{
     sm_cmd_type              *sm_cmd;
     sm_cmd = sm_get_cmd_buf();

     if (sm_cmd == NULL)
     {
       MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
     }

     sm_cmd->header.message_set = MS_GMM_SM;
     sm_cmd->header.cmd_id      = GMMSM_UNBLOCK_ALL_APN;
#ifdef FEATURE_DUAL_SIM
     sm_cmd->cmd.gmm_establish_cnf.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

     MSG_HIGH_DS_0(MM_SUB, "=MM= Sending GMMSM_UNBLOCK_ALL_APN");

     sm_put_cmd( sm_cmd );

}

#if defined(FEATURE_LTE) && defined(FEATURE_DUAL_SIM)
/*===========================================================================

FUNCTION MM_SEND_SM_MULTIMODE_SUBS_CHGD_REQ

DESCRIPTION
  This function sends GMMSM_MULTIMODE_SUBS_CHGD_REQ to SM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_sm_multimode_subs_chgd_req
(
  mm_as_id_e_type multimode_as_id
)
{
  sm_cmd_type              *sm_cmd;
  sm_cmd = sm_get_cmd_buf();

  if (sm_cmd == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
  }

  sm_cmd->header.message_set = MS_GMM_SM;
  sm_cmd->header.cmd_id      = GMMSM_MULTIMODE_SUBS_CHGD_REQ;
  sm_cmd->cmd.gmmsm_multimode_subs_chgd_req.as_id = (sys_modem_as_id_e_type)multimode_as_id;

  MSG_HIGH_DS_0(MM_SUB, "=MM= Sending GMMSM_MULTIMODE_SUBS_CHGD_REQ");
  sm_put_cmd( sm_cmd );
}
#endif

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION mm_send_rrc_csg_white_list_ind

DESCRIPTION
  This function sends white list to WRRC 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_csg_white_list_ind(sys_csg_list_s_type *operator_list, sys_csg_list_s_type * allowed_list)
{

#ifdef FEATURE_WCDMA

  rrc_cmd_type *rrc_cmd_p = NULL;
  int index;
  uint8 length = 0;

#ifdef FEATURE_SEGMENT_LOADING
 if (get_wcdma_interface() == NULL)
 {
     return;
 }
#endif

  rrc_cmd_p = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );

  if (rrc_cmd_p == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
  }

  ASSERT(operator_list != NULL);
  ASSERT(allowed_list != NULL);

  rrc_cmd_p->cmd_hdr.cmd_id = RRC_CSG_WHITE_LIST_UPDATE_IND;
  rrc_cmd_p->cmd.white_list.length = 0;

  if(operator_list->csg != NULL)
  {
    for(index = 0; (index < operator_list->length && length < SYS_CSG_LIST_MAX_LENGTH); index++)
    {
      if(operator_list->csg[index].timer_count == MM_TIMER_DISABLED &&
         operator_list->csg[index].csg_id != SYS_CSG_ID_INVALID)
      {
        rrc_cmd_p->cmd.white_list.csg_list[length].csg_id = operator_list->csg[index].csg_id;
        rrc_cmd_p->cmd.white_list.csg_list[length].plmn_id = operator_list->csg[index].plmn;
        length++;
      }
    }
  }

  if(allowed_list->csg != NULL)
  {
    for(index = 0; (index < allowed_list->length && length < SYS_CSG_LIST_MAX_LENGTH); index++)
    {
      if(allowed_list->csg[index].csg_id != SYS_CSG_ID_INVALID)
      {
        rrc_cmd_p->cmd.white_list.csg_list[length].csg_id = allowed_list->csg[index].csg_id;
        rrc_cmd_p->cmd.white_list.csg_list[length].plmn_id = allowed_list->csg[index].plmn;
        length++;
      }
    }
  }
  rrc_cmd_p->cmd.white_list.length = length;

  send_cmd_to_rrc( rrc_cmd_p, SYS_RAT_UMTS_RADIO_ACCESS );

#endif

}

/*===========================================================================

FUNCTION mm_send_rr_csg_white_list_ind

DESCRIPTION
  This function sends white list update to GRr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_csg_white_list_ind(void)
{
   rr_csg_white_list_update_ind_T rr_csg_white_list_update_ind;
   memset(&rr_csg_white_list_update_ind,0x00,sizeof(rr_csg_white_list_update_ind_T));
   rr_csg_white_list_update_ind.message_header.message_set = MS_MM_RR;
   rr_csg_white_list_update_ind.message_header.message_id  = (int)RR_CSG_WHITE_LIST_UPDATE_IND;
   
   /*lint -save -e778 */
   PUT_IMH_LEN( sizeof( rr_csg_white_list_update_ind_T ) - sizeof(IMH_T),
                &rr_csg_white_list_update_ind.message_header );
   /*lint -restore */
   MSG_HIGH( "=MM= MM sent RR_CSG_WHITE_LIST_UPDATE_IND", 0, 0, 0);
   
   mm_send_message((IMH_T *)&rr_csg_white_list_update_ind, GS_QUEUE_RR );  

}

#endif

/*===========================================================================
FUNCTION MM_SEND_EMPTY_MMR_PLMN_SEARCH_CNF

DESCRIPTION
  This function sends MMR_PLMN_SEARCH_CNF message to the REG with 0 PLMNs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_send_empty_mmr_plmn_search_cnf
( 
  mmr_plmn_search_req_s_type  *plmn_search_req_ptr,
  reg_mm_plmn_search_status_e_type    search_result
)
{
  mmr_plmn_search_cnf_s_type  *mmr_plmn_search_cnf_ptr = NULL;

  ASSERT(plmn_search_req_ptr != NULL);
  mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_NONE;
  /* Init */
  mmr_plmn_search_cnf_ptr =(mmr_plmn_search_cnf_s_type  *) mm_mem_alloc(
                            sizeof(mmr_plmn_search_cnf_s_type));
  ASSERT(mmr_plmn_search_cnf_ptr != NULL);
 
  mmr_plmn_search_cnf_ptr->message_header.message_set = MS_MM_REG;
  mmr_plmn_search_cnf_ptr->message_header.message_id  = (int)MMR_PLMN_SEARCH_CNF;

#if defined (FEATURE_WTOG_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_WTOW_BACKGROUND_PLMN_SEARCH) \
    || defined (FEATURE_RAT_PRIORITY_LIST)
   mmr_plmn_search_cnf_ptr->network_selection_mode = 
                                   plmn_search_req_ptr->network_selection_mode;
#endif

   mmr_plmn_search_cnf_ptr->plmn_search_result = search_result;

   mmr_plmn_search_cnf_ptr->found_plmn_list.plmn_list.length = 0;

   mmr_plmn_search_cnf_ptr->transaction_id = plmn_search_req_ptr->transaction_id;

#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  mmr_plmn_search_cnf_ptr->service_search = plmn_search_req_ptr->service_search;
#endif

  PUT_IMH_LEN( sizeof( mmr_plmn_search_cnf_s_type) - sizeof(IMH_T),
                &mmr_plmn_search_cnf_ptr->message_header );

  MSG_HIGH_DS_0(MM_SUB,"=MM= Sending empty MMR_PLMN_SEARCH_CNF");
  mm_send_message((IMH_T*)mmr_plmn_search_cnf_ptr, GS_QUEUE_REG);
  modem_mem_free((void *)mmr_plmn_search_cnf_ptr,MODEM_MEM_CLIENT_NAS);
}

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================
FUNCTION MM_SEND_MMR_CSG_SEARCH_TRIGGER_IND

DESCRIPTION
  This function sends MMR_CSG_SEARCH_TRIGGER_IND message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_send_mmr_csg_search_trigger_ind(void)
{
  mmr_csg_search_trigger_ind_s_type mmr_csg_search_trigger_ind;
  mmr_csg_search_trigger_ind.message_header.message_set = MS_MM_REG ;
  mmr_csg_search_trigger_ind.message_header.message_id  = 
                                             (int)MMR_CSG_SEARCH_TRIGGER_IND;
  
  PUT_IMH_LEN( sizeof(mmr_csg_search_trigger_ind_s_type) - sizeof(IMH_T),
                                  &mmr_csg_search_trigger_ind.message_header );
  
  MSG_HIGH_DS_0(MM_SUB,"MM: sent MMR_CSG_SEARCH_TRIGGER_IND to REG");
  mm_send_message( (IMH_T *)&mmr_csg_search_trigger_ind, GS_QUEUE_REG );
}

/*===========================================================================
FUNCTION MMR_CSG_LIST_READ_COMPLETE_IND

DESCRIPTION
  This function sends MMR_CSG_LIST_READ_COMPLETE_IND message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_send_mmr_csg_list_read_complete_ind(void)
{
  mmr_csg_list_read_complete_ind_s_type mmr_csg_list_read_ind;
  mmr_csg_list_read_ind.message_header.message_set = MS_MM_REG;
  mmr_csg_list_read_ind.message_header.message_id  = 
                                             (int)MMR_CSG_LIST_READ_COMPLETE_IND;
  
  PUT_IMH_LEN( sizeof(mmr_csg_list_read_complete_ind_s_type) - sizeof(IMH_T),
                                  &mmr_csg_list_read_ind.message_header );
  
  MSG_HIGH_DS_0(MM_SUB,"MM: sent MMR_CSG_LIST_READ_COMPLETE_IND to REG");
  mm_send_message( (IMH_T *)&mmr_csg_list_read_ind, GS_QUEUE_REG );
}
#endif

#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE || defined FEATURE_SVLTE_DUAL_SIM
void mm_send_rrc_ue_mode_change_ind( sys_radio_access_tech_e_type rat,sys_ue_mode_e_type   ue_mode )
{
#ifdef FEATURE_WCDMA   
   rrc_cmd_type *rrc_cmd_p = NULL;
#ifdef FEATURE_SEGMENT_LOADING
  if ((!mm_wcdma_supported) && (rat == SYS_RAT_UMTS_RADIO_ACCESS))
  {
      /* W not loaded */
      return;
  }
  if ((!mm_tdscdma_supported) && (rat == SYS_RAT_TDS_RADIO_ACCESS))
  {
      /* T not loaded */
      return;
  }
#endif /* FEATURE_SEGMENT_LOADING */


   rrc_cmd_p = mm_get_rrc_cmd_buf( rat );


   if (rrc_cmd_p == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_UE_MODE_CHANGE_IND", 0, 0, 0);
   }
   else
   {
      
      rrc_cmd_p->cmd_hdr.cmd_id = RRC_UE_MODE_IND ;
      rrc_cmd_p->cmd.rrc_ue_mode_ind.ue_mode=ue_mode;
      send_cmd_to_rrc( rrc_cmd_p, rat );
   } 
#endif /* FEATURE_WCDMA */
}
#endif

void mm_send_rr_pseudo_list_update_ind(boolean can_use_pseudo)
{
   rr_pseudo_lte_list_update_ind_T   rr_pseudo_list_update_ind;
   memset(&rr_pseudo_list_update_ind,0x00,sizeof(rr_pseudo_lte_list_update_ind_T));
   MSG_HIGH_DS_1(MM_SUB, "=MM= Sending RR_PSEUDO_LTE_LIST_UPDATE_IND to RR can use pseudo = %d ",can_use_pseudo);
   rr_pseudo_list_update_ind.message_header.message_set = MS_MM_RR;
   rr_pseudo_list_update_ind.message_header.message_id = (int) RR_PSEUDO_LTE_LIST_UPDATE_IND;
   rr_pseudo_list_update_ind.can_use_pseudo = can_use_pseudo;
   PUT_IMH_LEN( sizeof( rr_pseudo_lte_list_update_ind_T ) - sizeof(IMH_T),
            &rr_pseudo_list_update_ind.message_header );
   mm_send_message((IMH_T *)&rr_pseudo_list_update_ind,GS_QUEUE_RR );
}


/*===========================================================================

FUNCTION mm_send_rrc_pseudo_list_update_ind

DESCRIPTION
  This function sends a mm_send_rrc_pseudo_list_update_ind to RR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_pseudo_list_update_ind(boolean can_use_pseudo)
{

#ifdef FEATURE_WCDMA   
      rrc_cmd_type *rrc_cmd_p = NULL;
#ifdef FEATURE_SEGMENT_LOADING
     if (!mm_wcdma_supported)
     {
       /* W not loaded */
        return;
     }
#endif /* FEATURE_SEGMENT_LOADING */
     rrc_cmd_p = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );
      if (rrc_cmd_p == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= Out of memory while sending RRC_PSEUDO_LTE_LIST_UPDATE_IND", 0, 0, 0);
      }
      else
{
        rrc_cmd_p->cmd_hdr.cmd_id = RRC_PSEUDO_LTE_LIST_UPDATE_IND ;
        rrc_cmd_p->cmd.rrc_psuedo_lte_list_update_ind.can_use_pseudo=can_use_pseudo;
        MSG_HIGH_DS_1(MM_SUB, "=MM= Sending RRC_PSEUDO_LTE_LIST_UPDATE_IND can use pseudo = %d",can_use_pseudo);
        send_cmd_to_rrc( rrc_cmd_p, SYS_RAT_UMTS_RADIO_ACCESS );
      } 
#endif
}

#ifdef FEATURE_TDSCDMA

void mm_send_tdsrrc_pseudo_list_update_ind(boolean can_use_pseudo)
{

   tdsrrc_mm_cmd_type *tdsrrc_cmd = NULL;
#ifdef FEATURE_SEGMENT_LOADING   
   interface_t* ptr_T = get_tdscdma_interface();

   if (ptr_T == NULL)
   {
       MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Wrong segment", 0,0,0 );
   }

   tdsrrc_cmd = (tdsrrc_mm_cmd_type *) ITDSCDMA_tdsrrc_get_mm_cmd_buf(ptr_T);
#else
   tdsrrc_cmd = (tdsrrc_mm_cmd_type *) tdsrrc_get_mm_cmd_buf();
#endif
   if (tdsrrc_cmd == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }
   else
   {
      tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_PSEUDO_LTE_LIST_UPDATE_IND ;
      tdsrrc_cmd->cmd.rrc_pseudo_lte_avail_update_ind.can_use_pseudo = can_use_pseudo;
#ifdef FEATURE_DUAL_SIM
      tdsrrc_cmd->cmd.rrc_pseudo_lte_avail_update_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif
      MSG_HIGH_DS_1(MM_SUB, "=MM= Sending TDSRRC_PSEUDO_LTE_LIST_UPDATE_IND can use pseudo = %d",can_use_pseudo);

   } 

     /* Add sent messages to the debug buffer */
    mm_add_message_to_debug_buffer( MS_MM_TDSRRC,
                                    (dword)(tdsrrc_cmd->cmd_hdr.cmd_id),
                                    FROM_MM
                                    ,(sys_modem_as_id_e_type)mm_sub_id
                                                );
#ifndef FEATURE_SEGMENT_LOADING
    tdsrrc_put_mm_cmd( tdsrrc_cmd );
#else
    ITDSCDMA_tdsrrc_put_mm_cmd(ptr_T, tdsrrc_cmd );
#endif

}
#endif

#ifdef FEATURE_SGLTE
void mmsend_mmr_stack_act_ind(void)
{
  mmr_stack_act_ind_s_type mmr_stack_act_ind;
  mmr_stack_act_ind.message_header.message_set = MS_MM_REG ;
  mmr_stack_act_ind.message_header.message_id  = (int)MMR_STACK_ACT_IND;
  mmr_stack_act_ind.as_id = (sys_modem_as_id_e_type)MM_SGLTE_STACK_2;
     /*lint -save -e778 */
  PUT_IMH_LEN( sizeof(mmr_stack_act_ind_s_type) - sizeof(IMH_T),&mmr_stack_act_ind.message_header );
  /*lint -restore */
  
   MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_STACK_ACT_IND");
  mm_send_message( (IMH_T *)&mmr_stack_act_ind, GS_QUEUE_REG );
 
}

void mmsend_mmr_stack_deact_ind(void)
{
  mmr_stack_deact_ind_s_type mmr_stack_deact_ind;
  mmr_stack_deact_ind.message_header.message_set = MS_MM_REG ;
  mmr_stack_deact_ind.message_header.message_id  = (int)MMR_STACK_DEACT_IND;
  mmr_stack_deact_ind.as_id = (sys_modem_as_id_e_type)mm_cs_stack_id;
     /*lint -save -e778 */
  PUT_IMH_LEN( sizeof(mmr_stack_deact_ind_s_type) - sizeof(IMH_T),&mmr_stack_deact_ind.message_header );
  /*lint -restore */
  
  MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMR_STACK_DEACT_IND");
  mm_send_message( (IMH_T *)&mmr_stack_deact_ind, GS_QUEUE_REG );
 
}


void mmsend_mmr_service_stat_ind( mm_as_id_e_type mm_as_id_in)
{

  mmr_service_stat_ind_s_type mmr_service_stat_ind;
  mmr_service_stat_ind.message_header.message_set = MS_MM_REG ;
  mmr_service_stat_ind.message_header.message_id  = (int)MMR_SERVICE_STAT_IND;
  mmr_service_stat_ind.as_id = (sys_modem_as_id_e_type)mm_as_id_in;
  mmr_service_stat_ind.service_state = mm_serving_plmn_sim[mm_as_id_in].info;
  mmr_service_stat_ind.service_state.lac = b_unpackw(mm_serving_plmn_sim[mm_as_id_in].lac.lac,0,16);
  mmr_service_stat_ind.service_state.rac_or_mme_code = mm_serving_plmn_sim[mm_as_id_in].rac;
  if ((mm_serving_plmn_sim[mm_cs_stack_id].info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY ) ||
     (mm_serving_plmn_sim[mm_cs_stack_id].info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS ))
  {
    mmr_service_stat_ind.service_state.reg_service_domain =SYS_SRV_DOMAIN_CS_ONLY;    
  }
  else 
  {
    mmr_service_stat_ind.service_state.reg_service_domain =SYS_SRV_DOMAIN_NO_SRV;  
  }
  mmr_service_stat_ind.service_state.service_status = mm_serving_plmn_sim[mm_cs_stack_id].info.service_status;
  if ((mmr_service_stat_ind.service_state.service_status == SYS_SRV_STATUS_LIMITED) &&
        ((mm_state_sim[mm_cs_stack_id] == MM_LOCATION_UPDATE_INITIATED)||(mm_state_sim[mm_cs_stack_id] == MM_LOCATION_UPDATE_REJECTED)
       ||(mm_state_sim[mm_cs_stack_id] == MM_WAIT_FOR_RR_CONNECTION_LU)||
       ((mm_state_sim[mm_cs_stack_id] == MM_IDLE)&&((mm_timer_status_sim[mm_cs_stack_id][TIMER_T3211]  == TIMER_ACTIVE) ||
            (mm_timer_status_sim[mm_cs_stack_id][TIMER_T3213]  == TIMER_ACTIVE)))) )
  {
    mmr_service_stat_ind.service_state.service_status = SYS_SRV_STATUS_SRV;
  }
    
     /*lint -save -e778 */
  PUT_IMH_LEN( sizeof(mmr_service_stat_ind_s_type) - sizeof(IMH_T),&mmr_service_stat_ind.message_header );
  /*lint -restore */
  
  MSG_HIGH_DS_2(mm_as_id_in,"=MM= Sending MMR_SERVICE_STAT_IND with service domain = %d and service status = %d",
                mmr_service_stat_ind.service_state.reg_service_domain,
                mmr_service_stat_ind.service_state.service_status);
  mm_send_message((IMH_T *)&mmr_service_stat_ind, GS_QUEUE_REG );
    
}
#endif

void mm_send_rr_status_ind(sys_srv_status_e_type service_status, sys_srv_domain_e_type service_domain)
{
  rr_service_status_ind_T rr_service_status;
  rr_service_status.message_header.message_id = RR_SERVICE_STATUS_IND;
  rr_service_status.message_header.message_set = MS_MM_RR;
  rr_service_status.service_domain = service_domain;
  rr_service_status.service_state = service_status;
  
  PUT_IMH_LEN( sizeof(rr_service_status_ind_T) - sizeof(IMH_T),&rr_service_status.message_header );

  MSG_HIGH_DS_2(MM_SUB,  "=MM= Sending RR_SERVICE_STATUS_IND, with service domain %d and service status %d", service_domain,service_status);
  mm_send_message((IMH_T *)&rr_service_status, GS_QUEUE_RR );
    
}
 

/*===========================================================================
FUNCTION mm_send_mmr_t3346_timer_ind

DESCRIPTION
  This function sends MMR_T3346_TIMER_IND message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_send_mmr_t3346_timer_ind(const sys_nas_t3346_status_e_type t3346_notification)
{
  mmr_t3346_timer_ind_s_type mmr_t3346_notification_ind;
  mmr_t3346_notification_ind.message_header.message_set = MS_MM_REG;
  mmr_t3346_notification_ind.message_header.message_id  = 
                                             (int)MMR_T3346_TIMER_IND;
  
  mmr_t3346_notification_ind.active_rat = mm_serving_plmn.info.active_rat;
  mmr_t3346_notification_ind.t3346_notification = t3346_notification;
  PUT_IMH_LEN( sizeof(mmr_t3346_timer_ind_s_type) - sizeof(IMH_T),
                                  &mmr_t3346_notification_ind.message_header );
  
  MSG_HIGH_DS_2(MM_SUB,"MM: sent MMR_T3346_TIMER_IND to REG, RAT: %d, T3346 Notification: %d",
                         mmr_t3346_notification_ind.active_rat,
                         mmr_t3346_notification_ind.t3346_notification);
  mm_send_message( (IMH_T *)&mmr_t3346_notification_ind, GS_QUEUE_REG );
}

/*===========================================================================
FUNCTION MM_SEND_MMR_PSM_READY_RSP

DESCRIPTION
  This function sends MMR_PSM_READY_RSP message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_send_mmr_psm_ready_rsp
(
  sys_psm_status_e_type status  
)
{
  mmr_psm_ready_rsp_s_type mmr_psm_ready_rsp;
  mmr_psm_ready_rsp.message_header.message_set = MS_MM_REG;
  mmr_psm_ready_rsp.message_header.message_id  = 
                                             (int)MMR_PSM_READY_RSP;
  
  mmr_psm_ready_rsp.status = status;
  if(status != SYS_PSM_STATUS_READY)
  {
    /* set the flag when not ready for PSM */
    mm_psm_ready_req_rejected = TRUE;
  }

  PUT_IMH_LEN( sizeof(mmr_psm_ready_rsp_s_type) - sizeof(IMH_T),
                                  &mmr_psm_ready_rsp.message_header );
  
  MSG_HIGH_DS_1(MM_SUB,"MM: sent MMR_PSM_READY_RSP to REG, status: %d", status);
  mm_send_message( (IMH_T *)&mmr_psm_ready_rsp, GS_QUEUE_REG );
}

/*===========================================================================
FUNCTION MM_SEND_MMR_PSM_READY_IND

DESCRIPTION
  This function sends MMR_PSM_READY_IND message to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_send_mmr_psm_ready_ind
(
  void
)
{
  mmr_psm_ready_ind_s_type mmr_psm_ready_ind;
  mmr_psm_ready_ind.message_header.message_set = MS_MM_REG;
  mmr_psm_ready_ind.message_header.message_id  = 
                                             (int)MMR_PSM_READY_IND;
  
  PUT_IMH_LEN( sizeof(mmr_psm_ready_ind_s_type) - sizeof(IMH_T),
                                  &mmr_psm_ready_ind.message_header );
  
  /* reset the flag */
  mm_psm_ready_req_rejected = FALSE;

  MSG_HIGH_DS_0(MM_SUB,"MM: sent MMR_PSM_READY_IND to REG");
  mm_send_message( (IMH_T *)&mmr_psm_ready_ind, GS_QUEUE_REG );
}
