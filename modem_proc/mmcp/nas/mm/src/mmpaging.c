/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmpaging.c_v   1.16   17 Jul 2002 12:30:06   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmpaging.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/12/01   jca     Changed type cast in call to mm_send_nas_msg_to_rrc().
05/14/01   jca     Added mm_ prefix to MM/GMM shared variables.
05/15/01   sbk     Handling RRC_SYNC_IND after merging with 1.1 version
06/08/01   jca     Fixed bug where cmd_id was missing on RRC command.
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
10/19/01   sbk     TMSI updating is now done only on TMSI change 
04/30/02   ks      Added processing of RRC_PAGE_CAUSE_UNKNOWN paging_cause.
05/21/02   ks      Added processing of RRC_PAGE_CAUSE_LOW_PRI_SIGNALLING and
                   RRC_PAGE_CAUSE_HI_PRI_SIGNALLING in page response function.
05/28/02   jca     Now generating event MM_STD_RR_CONNECTION_ESTABLISHED to
                   mm_state_control when PAGING RESPONSE message is txed.
07/17/02   ks      replaced read_coded_classmark_2 with gs_read_coded_classmark_2
                   for dual mode classmark information.                  
08/11/02   mks     Removed mm_send_paging_control_to_update_tmsi to obsolete 
                   RRC_PAGING_CTRL_REQ. RRC_PAGING_CTRL_REQ is obsoleted as the 
                   information conveyed by this primitive is now sent to RRC 
                   through RRC_SIM_INSERTED_REQ & RRC_SIM_UPDATE_REQ. 
09/04/02   vdr     Fixed determination of the CKSN
02/10/03   cd      Fixed Lint errors
11/11/11   gps    Initiated RRC establish reqquest with all zeros.
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
#include "timers_v.h"
#include "gs_v.h"
#include "mm_v.h"
#include "rrcmmif.h"
#include "libprot.h"
#include "mm_umts_v.h"
#include "mm_coord.h"
#include "err.h"
#include "mmauth.h"
#ifdef FEATURE_DUAL_SIM
#include "subs_prio.h"
#endif
#include "mm_iei.h"
extern void gs_read_coded_classmark_2_per_nas_stacks(
  classmark_2_ie_T  *classmark_2_ptr,
  classmark_params_T   *params_ptr,
  sys_modem_as_id_e_type stack_id_nas
);


/* Local Functions */
static void mmsend_page_response_on_new_rrc_conn(mm_cmd_type *msg_p);


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern byte mm_last_paging_cause_sim[MAX_NAS_STACKS];

#define  mm_last_paging_cause mm_last_paging_cause_sim[mm_as_id]

#else
extern byte mm_last_paging_cause;
#endif /* FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/


/*===========================================================================

FUNCTION MM_PREPARE_PAGING_RESPONSE

DESCRIPTION
  This function sends the PAGING RESPONSE message in response to a page

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_prepare_paging_response(paging_response_msg_T *page_resp, mm_cmd_type *msg_p)
{
   uint32 length = 0;
   uint32 index = 0;
   
   mm_last_paging_cause = (byte)msg_p->cmd.rrc_page_ind.paging_cause;
   
   /* MM takes control of resel decision for CS MT call case */
   mm_controls_resel_dec_mt_cs_call = TRUE;
   
   page_resp->msg_head.IMH.message_set = MS_MM_RR;
   page_resp->msg_head.IMH.message_id  = (int) RR_DATA_REQ;
   page_resp->msg_head.skip_pd         = PD_RR;
   page_resp->msg_head.type            = PAGING_RESPONSE;

   page_resp->cksn_spare = auth_get_ciphering_key_sequence_number(
       mm_get_current_security_context(msg_p->cmd.rrc_page_ind.cn_domain_id)) ;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  {
    classmark_params_T classmark_params;
    classmark_params.gsm_supported   = mm_gsm_supported;
    classmark_params.wcdma_supported = (mm_wcdma_supported ||mm_tdscdma_supported);
    classmark_params.wcdma_in_use    = is_umts_mode();
    classmark_params.MSCR99          = mm_system_information.mscr;
    classmark_params.as_id           = (sys_modem_as_id_e_type)mm_sub_id;
    gs_read_coded_classmark_2_per_nas_stacks(
     (classmark_2_ie_T *)&page_resp->classmark2,
     &classmark_params, (sys_modem_as_id_e_type)mm_as_id);
  }
#else
  gs_read_coded_classmark_2((classmark_2_ie_T *)&page_resp->classmark2,
                             mm_gsm_supported,
                             (mm_wcdma_supported || mm_tdscdma_supported),
                             is_umts_mode(),
                             mm_system_information.mscr );
#endif

   length = (sizeof(paging_response_msg_T) - sizeof(IMH_T)) - sizeof(page_resp->data); /* For now, remnove mobile ID w/ size */

   if(msg_p->cmd.rrc_page_ind.page_ue_id == RRC_PAGE_WITH_IMSI)
   {
      index= mm_pack_imsi(page_resp->data);
      length +=index;
   }
   else if(msg_p->cmd.rrc_page_ind.page_ue_id == RRC_PAGE_WITH_TMSI)
   {
      index= mm_pack_tmsi(page_resp->data);
      length +=index;
   }
   else 
   {
      MSG_ERROR_DS( MM_SUB, "=MM= Paged with unknown UE ID", 0,0,0 );
   }

   PUT_IMH_LEN(length, &page_resp->msg_head.IMH);

#ifdef FEATURE_NAS_ECALL
   if (mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY)
   {
     if(mm_call_est_cause == NO_CNM_SERVICE)
     {
       mm_call_est_cause = EMERGENCY_CALL_ESTABLISHMENT;
     }
     mm_stop_timer(TIMER_T3242);
     mm_stop_timer(TIMER_T3243);
   }
#endif
#ifdef FEATURE_NAS_REL10
   if(mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
   {
     mm_stop_timer(TIMER_T3246);
   }
#endif

}


/*===========================================================================

FUNCTION MM_PAGING_RESPONSE

DESCRIPTION
  This function responds to a page by taking appropriate response

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_paging_response(mm_cmd_type* msg_p)
{
   rrc_page_ue_id_choice_e_type 
      page_ue_id = msg_p->cmd.rrc_page_ind.page_ue_id;

   if ( mm_idle_substate == MM_NORMAL_SERVICE)
   {
      /* 24.008 4.2.2.1 */
      mmsend_page_response_on_new_rrc_conn(msg_p);
      mm_state_control( MM_STD_RR_CONNECTION_ESTABLISHED );
   }
   else if( mm_idle_substate == MM_ATTEMPTING_TO_UPDATE)
   {
      /* 24.008 4.2.2.2 */
      /* Also respond to TMSI page when MM is UPDATED (in case of LR abort after TMSI reallocation)*/
      if(( page_ue_id == RRC_PAGE_WITH_IMSI) ||
         ((mm_location_information.location_update_status == UPDATED) && 
         (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)))
      {
         mmsend_page_response_on_new_rrc_conn(msg_p);
         mm_state_control( MM_STD_RR_CONNECTION_ESTABLISHED );
      }
   }
   else if( mm_idle_substate == MM_LIMITED_SERVICE)
   {
      /* In MM_LIMITED_SERVICE state, 24.008 4.2.2.3 says when paged by IMSI,
         MM may respond to paging */
      if( page_ue_id == RRC_PAGE_WITH_IMSI)
      {
         mmsend_page_response_on_new_rrc_conn(msg_p);
         mm_state_control( MM_STD_RR_CONNECTION_ESTABLISHED );
      }
   }
   else if( mm_idle_substate == MM_NO_IMSI)
   {
      /* 24.008 4.2.2.4*/
      MSG_HIGH_DS_0(MM_SUB,"=MM= Should not have been paged as there is no IMSI");
   }
   else if( mm_idle_substate == MM_PLMN_SEARCH_NORMAL_SERVICE)
   {
      /* 24.008 4.2.2.5 */
      mmsend_page_response_on_new_rrc_conn(msg_p);
      mm_state_control( MM_STD_RR_CONNECTION_ESTABLISHED );
   }
   else if(  mm_idle_substate == MM_PLMN_SEARCH)
   {
      /* 24.008 4.2.2.6 */
      MSG_HIGH_DS_0(MM_SUB,"=MM= Ignoring page");
   }
   else 
   {
      MSG_HIGH_DS_2( MM_SUB, "=MM= Ignoring page in state %d substate %d", mm_state, mm_idle_substate);
   }
}


/*===========================================================================

FUNCTION MMSEND_PAGE_RESPONSE_ON_NEW_RRC_CONN

DESCRIPTION
  This function sends the paging response message by initiating a new rrc conn

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void mmsend_page_response_on_new_rrc_conn(mm_cmd_type * msg_p)
{
   rrc_establish_cause_e_type cause;
   paging_response_msg_T paging_resp;

#ifdef FEATURE_DUAL_SIM
   sp_mt_call_info mt_call_subs_update = {SP_TECH_MT_CALL_STATE_NONE, SYS_MODEM_AS_ID_NO_CHANGE, SYS_SYS_MODE_MAX};
#endif

   /*
    * Prepare a paging response command TS 4.18 9.1.25
    */
   mm_prepare_paging_response(&paging_resp, msg_p);
   
   switch (msg_p->cmd.rrc_page_ind.paging_cause)
   {
      case RRC_PAGE_CAUSE_TERM_CONV_CALL:
         cause = RRC_EST_TERM_CONV_CALL;
         break;

      case RRC_PAGE_CAUSE_TERM_STREAM_CALL:
         cause = RRC_EST_TERM_STREAM_CALL;
         break;

      case RRC_PAGE_CAUSE_TERM_INTERACT_CALL:
         cause = RRC_EST_TERM_INTERACT_CALL;
         break;

      case RRC_PAGE_CAUSE_TERM_BACKGND_CALL:
         cause = RRC_EST_TERM_BACKGND_CALL;
         break;

      case RRC_PAGE_CAUSE_SMS:
         cause = RRC_EST_HI_PRI_SIGNALLING;
         break;
         
      case RRC_PAGE_CAUSE_HIGH_PRI_SIGNALLING:
        cause = RRC_EST_TERM_HI_SIGNALLING;
        break;
              
      case RRC_PAGE_CAUSE_LOW_PRI_SIGNALLING:
        cause = RRC_EST_TERM_LOW_SIGNALLING;
        break;      

      case RRC_PAGE_CAUSE_UNKNOWN:
         cause = RRC_EST_TERM_UNKNOWN; 
         break;

      default:
         MSG_ERROR_DS( MM_SUB, "=MM= Unknown paging cause type", 0,0,0 );
         mm_last_paging_cause = 0xFF;
         mm_controls_resel_dec_mt_cs_call = FALSE;	 
         return;
   }

#ifdef FEATURE_DUAL_SIM
   /* Inform MCS module that MM is responding to CS page
      This helps MCS module in deciding priority of subscriptions.
       */

   if(mm_last_paging_cause != 0xFF)
   {
      mt_call_subs_update.state = SP_TECH_MT_CALL_START;
      mt_call_subs_update.subscription_id = (sys_modem_as_id_e_type)mm_sub_id;

      mm_convert_rat_to_tech_id(mm_serving_plmn.info.active_rat, &mt_call_subs_update.tech_id);

      MSG_HIGH_DS_3( MM_SUB, "=MM= MCS being informed of SP_TECH_MT_CALL_START by NAS state = %d, subs_id = %d, tech_id = %d", mt_call_subs_update.state, 
                                                                         mt_call_subs_update.subscription_id, 
                                                                         mt_call_subs_update.tech_id );
      (void)sp_subscription_mt_call_info_update(mt_call_subs_update);
   }
#endif

   /*
    * Initiate a new RR Connection 
    */
   mm_initiate_rrc_connection(cause, (IMH_T *)&paging_resp );
}
/*===========================================================================

FUNCTION MM_SEND_PAGING_RESPONSE

DESCRIPTION
  This function sends paging response to RR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_paging_response
(
#ifdef FEATURE_LTE_TO_GSM
  rr_paging_type_T paging_type
#else            
  void
#endif
)
{

#ifdef FEATURE_DUAL_SIM
  sp_mt_call_info mt_call_subs_update = {SP_TECH_MT_CALL_STATE_NONE, SYS_MODEM_AS_ID_NO_CHANGE, SYS_SYS_MODE_MAX};
#endif

#ifdef FEATURE_LTE_TO_GSM
  uint32 length;
  uint32 index=0;
  paging_response_msg_T paging_resp;
#else
  rr_est_req_T               rr_est_req;
  memset(&rr_est_req,0x00,sizeof(rr_est_req_T));
#endif

  mm_last_paging_cause = (byte)CS_PAGING;
  
#ifdef FEATURE_NAS_ECALL
  if (mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY)
  {
    if(mm_call_est_cause == NO_CNM_SERVICE)
    {
      mm_call_est_cause = EMERGENCY_CALL_ESTABLISHMENT;
    }
    mm_stop_timer(TIMER_T3242);
    mm_stop_timer(TIMER_T3243);
  }
#endif
#ifdef FEATURE_NAS_REL10
  if(mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
  {
    mm_stop_timer(TIMER_T3246);
  }
#endif

#ifndef FEATURE_LTE_TO_GSM
  rr_est_req.establishment_cause = (int)MM_RR_ANSWER_TO_PAGING;
  rr_est_req.message_header.message_len_lsb = 0;
  rr_est_req.message_header.message_len_msb = 0;
  rr_est_req.message_header.message_set = MS_MM_RR;
  mm_initiate_rr_connection( MM_RR_ANSWER_TO_PAGING,
                               (IMH_T *)&rr_est_req);
#else
    
  /************************************************************/
  /* Encode the Protocol Discriminator and the Skip Indicator */
  /* The Skip Indicator is set to 0*/
  /************************************************************/
  paging_resp.msg_head.skip_pd = PD_RR;
  

  /************************************************/
  /* Encode the Message Type as a PAGING_RESPONSE */
  /************************************************/

  paging_resp.msg_head.type = PAGING_RESPONSE;


  /************************************************/
  /* Encode the Ciphering Key Sequence Number and */
  /* the Spare Half Octet*/
  /************************************************/

  paging_resp.cksn_spare = auth_get_ciphering_key_sequence_number(
                                                        mm_get_current_security_context(RRC_CS_DOMAIN_CN_ID)) ;


  /*****************************************/
  /* Encode the mobile station classmark 2 */
  /*****************************************/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
    {
      classmark_params_T classmark_params;
      classmark_params.gsm_supported   = mm_gsm_supported;
      classmark_params.wcdma_supported = (mm_wcdma_supported ||mm_tdscdma_supported);
      classmark_params.wcdma_in_use    = is_umts_mode();
      classmark_params.MSCR99          = mm_system_information.mscr;
      classmark_params.as_id           = (sys_modem_as_id_e_type)mm_sub_id;
      gs_read_coded_classmark_2_per_nas_stacks(
       (classmark_2_ie_T *)&paging_resp.classmark2,
       &classmark_params, (sys_modem_as_id_e_type)mm_as_id);
    }
#else
  gs_read_coded_classmark_2((classmark_2_ie_T *)&paging_resp.classmark2, mm_gsm_supported,
                                              (mm_wcdma_supported||mm_tdscdma_supported), is_umts_mode(), mm_system_information.mscr );
#endif

  /**************************************************/
  /* Encode the mobile identity information element */
  /**************************************************/
  length = (sizeof(paging_response_msg_T) - sizeof(IMH_T)) - sizeof(paging_resp.data);


  if(paging_type == CS_PAGING_WITH_IMSI)
  {
    index= mm_pack_imsi(paging_resp.data);
    length +=index;
  }
  else if(paging_type == CS_PAGING)
  {
    index= mm_pack_tmsi(paging_resp.data);
    length +=index;
  }
#ifdef FEATURE_LTE_REL11
   /* 44.018 3.3.2.2    Paging response	  
   **If the PAGING RESPONSE message is being sent as part of a CS FallBack procedure,	
   **then the mobile station shall set the CSMT flag in the Additional Update Parameters IE.*/
  if ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) && (!mm_suppress_additional_update_param))
  {
  
    if(index < sizeof(paging_resp.data) && (mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG 
      || mm_csfb_state == MM_CSFB_STATE_MT_CALL_CCO))
    {
      paging_resp.data[index]= (IEI_ADDITIONAL_UPDATE_PARAM << 4) & 0xF0;  
      paging_resp.data[index] = paging_resp.data[index] | 0x01;	
      length++;
      MSG_HIGH_DS_2(MM_SUB,"=MM= CSFB: setting addition IE to true in page response %d length %d",paging_resp.data[index],length); 
    }
  } 
#endif
  PUT_IMH_LEN(length, &paging_resp.msg_head.IMH);
  mm_initiate_rr_connection( MM_RR_ANSWER_TO_PAGING,
                               (IMH_T *)&paging_resp); 
#endif /*FEATURE_LTE_TO_GSM */

#ifdef FEATURE_DUAL_SIM
   /* Inform MCS module that MM is responding to CS page
      This helps MCS module in deciding priority of subscriptions.
    */

  if(mm_last_paging_cause != 0xFF)
  {
     mt_call_subs_update.state = SP_TECH_MT_CALL_START;
     mt_call_subs_update.subscription_id = (sys_modem_as_id_e_type)mm_sub_id;

     mm_convert_rat_to_tech_id(mm_serving_plmn.info.active_rat, &mt_call_subs_update.tech_id);

     MSG_HIGH_DS_3( MM_SUB, "=MM= MCS being informed of SP_TECH_MT_CALL_START by NAS state = %d, subs_id = %d, tech_id = %d", mt_call_subs_update.state, 
                                                                      mt_call_subs_update.subscription_id, 
                                                                      mt_call_subs_update.tech_id );
     (void)sp_subscription_mt_call_info_update(mt_call_subs_update);
  }
#endif

  mm_state_control( MM_STD_RR_CONNECTION_ESTABLISHED );
  if (mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page)
  {
    mm_start_timer(TIMER_SCRI_PAGING, DEFAULT_TIMEOUT );
  }
}



#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION mm_csfb_send_paging_response

DESCRIPTION
  This function sends paging response for LTE CS MT pages

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_csfb_send_paging_response()
{

  paging_response_msg_T paging_resp;
  uint32 length = 0;
  uint32 index =0;
  if (is_umts_mode())
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= CSFB: Responding to LTE page in UMTS");
    
    paging_resp.msg_head.IMH.message_set = MS_MM_RR;
    paging_resp.msg_head.IMH.message_id  = (int) RR_DATA_REQ;
    paging_resp.msg_head.skip_pd   = PD_RR;
    paging_resp.msg_head.type     = PAGING_RESPONSE;
    
    paging_resp.cksn_spare = auth_get_ciphering_key_sequence_number(
                           mm_get_current_security_context(RRC_CS_DOMAIN_CN_ID)) ;

  
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
    {
      classmark_params_T classmark_params;
      classmark_params.gsm_supported   = mm_gsm_supported;
      classmark_params.wcdma_supported = (mm_wcdma_supported||mm_tdscdma_supported);
      classmark_params.wcdma_in_use    = is_umts_mode();
      classmark_params.MSCR99          = mm_system_information.mscr;
      classmark_params.as_id           = (sys_modem_as_id_e_type)mm_sub_id;
      gs_read_coded_classmark_2_per_nas_stacks(
       (classmark_2_ie_T *)&paging_resp.classmark2,
       &classmark_params,(sys_modem_as_id_e_type)mm_as_id );
    }
#else
    gs_read_coded_classmark_2((classmark_2_ie_T *)&paging_resp.classmark2, mm_gsm_supported,
                                                (mm_wcdma_supported ||mm_tdscdma_supported), is_umts_mode(), mm_system_information.mscr );
#endif
    length = (sizeof(paging_response_msg_T) - sizeof(IMH_T)) - sizeof(paging_resp.data); /* For now, remnove mobile ID w/ size */
    
    if(mm_csfb_mt_call_info.ue_identity == EMM_CSFB_PAGING_TYPE_IMSI)
    {
      index= mm_pack_imsi(paging_resp.data);
      length +=index;
    }
    else if(mm_csfb_mt_call_info.ue_identity == EMM_CSFB_PAGINGI_TYPE_TMSI)
    {
      index = mm_pack_tmsi(paging_resp.data);
      length +=index;
    }
#ifdef FEATURE_LTE_REL11
   /* 44.018 3.3.2.2	Paging response    
   **If the PAGING RESPONSE message is being sent as part of a CS FallBack procedure,  
   **then the mobile station shall set the CSMT flag in the Additional Update Parameters IE.*/
   if ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) && (!mm_suppress_additional_update_param))
   {
     if(index < sizeof(paging_resp.data) && (mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG 
      || mm_csfb_state == MM_CSFB_STATE_MT_CALL_CCO))    
     {
       paging_resp.data[index]= (IEI_ADDITIONAL_UPDATE_PARAM << 4) & 0xF0;  
       paging_resp.data[index] = paging_resp.data[index] | 0x01;	
       length++;     
       MSG_HIGH_DS_2(MM_SUB,"=MM= CSFB: setting addition IE to true in page response %d length %d",paging_resp.data[index],length); 
     }
    }
#endif   
    PUT_IMH_LEN(length, &paging_resp.msg_head.IMH);
#ifdef FEATURE_LTE_REL10
    if(mm_csfb_mt_call_info.mt_csfb_highPriority == TRUE)
    {
      mm_initiate_rrc_connection(RRC_EST_TERM_HI_SIGNALLING, (IMH_T *)&paging_resp );
      mm_csfb_mt_call_info.mt_csfb_highPriority = FALSE;
    }
    else
#endif
    {
      mm_initiate_rrc_connection(RRC_EST_TERM_CONV_CALL, (IMH_T *)&paging_resp);
    }
    mm_state_control( MM_STD_RR_CONNECTION_ESTABLISHED );
  }
#ifdef FEATURE_LTE_TO_GSM
  else
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= CSFB: Responding to LTE page in GSM");
    if(mm_csfb_mt_call_info.ue_identity == EMM_CSFB_PAGING_TYPE_IMSI)
    {
      mm_send_paging_response(CS_PAGING_WITH_IMSI);
    }
    else if(mm_csfb_mt_call_info.ue_identity == EMM_CSFB_PAGINGI_TYPE_TMSI)
    {
      mm_send_paging_response(CS_PAGING); 
    }
  }
#endif
}

#endif /*FEATURE_LTE*/

