
/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_gps_handler.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $ 
 
when       who     what
--------   ---     ----------------------------------------------------------
04/22/10   zren    Updated to use global structure for NAS outgoing message
09/25/09   hnam    Added TLB support
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#if defined (FEATURE_LTE) && defined (FEATURE_LTE_REL9)
#include "rex.h"
#include "emm_database.h"
#include "msgr.h"
#include "msgr_lte.h"
#include "lte_nas.h"
#include "err.h"
#include "comdef.h"
#include "msgr_lte.h"
#include "msgr_nas.h"
#include "emm_gps_if_msg.h"
#include "emm_gps_handler.h"
#include "lte_nas_emm_message_type.h"
#include "emm_gps_lib.h"
#include "emm_utility.h"
#include "emm_service_request_lib.h"
#include <dsm_item.h>
#include "emm_rrc_if.h"


/* used by lte nas fuzz testing */
#ifdef TEST_FRAMEWORK
#error code not present
#endif



/*************************************************************************** 
                   External functions 
****************************************************************************/

/*===========================================================================
FUNCTION    EMM_PROCESS_UL_GENERIC_NAS_TRANSPORT_MESSAGE

DESCRIPTION
  This function puts the incoming RAW buffer from GPS module, into the correct 
  'C' structure that MSG_LIB identifies and then calls emm_Send_message, 
  to take care of encoding and hence routing this message to RRC

DEPENDENCIES
  None

RETURN VALUE
  NONE 

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_process_ul_generic_nas_transport_msg
(
  void                   *cmd_ptr, 
  emm_ctrl_data_type     *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp  **mm_emm_dispatcher_fn_ptr
)
{
  lte_nas_emm_ul_generic_nas_transport *ul_generic_transport = NULL;
  emm_gps_cmd_type                     *emm_cmd_ptr = NULL;
  emm_state_type                        emm_state = EMM_INVALID_STATE;
  emm_substate_type                     emm_substate;
  emm_csfb_rsp                     csfb_rsp;

  uint8 num_attach, i;
  dsm_item_type            *dsm_arr_ptr[EMM_GENERIC_NAS_TRANSPORT_MAX_DSM_ARRAY_SIZE];
  msgr_attach_struct_type  *att_ptr;
  
  ASSERT(cmd_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);

  /* typecast it to emm_gps_cmd_type */
  emm_cmd_ptr = (emm_gps_cmd_type*)cmd_ptr; 
  /*initialize it*/
  memset((void *)&csfb_rsp,0,sizeof(emm_csfb_rsp));

  if(emm_cmd_ptr->gps_msgr_cmd.msg_hdr.id != NAS_EMM_UL_GENERIC_NAS_TRANSPORT_REQ) 
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= unknown message from GPS module, discarding it");
    return FALSE;
  }
  
  MSG_HIGH_DS_0(MM_SUB, "=EMM= received UL_GENERIC_NAS_TRANSPORT");

  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL9)
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Spec version '%d',drop this message",
                   emm_ctrl_data_ptr->emm_3gpp_spec_ver);
    return FALSE;
  }

  num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&emm_cmd_ptr->gps_msgr_cmd);
  ASSERT(num_attach != 0);

  /* get the dsm ptr */
  for(i=0; i< num_attach && i<EMM_GENERIC_NAS_TRANSPORT_MAX_DSM_ARRAY_SIZE ;i++)
  {
    att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&emm_cmd_ptr->gps_msgr_cmd, i);
    ASSERT(att_ptr != NULL);

    msgr_get_dsm_attach(att_ptr, &dsm_arr_ptr[i]);
    ASSERT(&dsm_arr_ptr[i] != NULL);
  }

  /* check if we are on LTE otherwise return failure as LPP session is not
     aware if we lost service on LTE when the sesion is started */
  if((emm_ctrl_data_ptr->out_msg_ptr == NULL) ||
     (emm_ctrl_data_ptr->rrc_active == FALSE))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= GPS UL_MSG_REQ received while not in LTE rrc_active %d",
                  emm_ctrl_data_ptr->rrc_active);
    /* free the dsm ptr */
    emm_generic_nas_dsm_free(num_attach, dsm_arr_ptr);

    /* send failure ind */
    emm_send_generic_nas_failure_ind(LTE_NAS_IRAT_INVALID_STATE,
                                     LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                     emm_cmd_ptr->gps_msgr_cmd.trans_id,
                                     LTE_NAS_IRAT_ACCESS_BARRING_NONE);
    return TRUE;
  }


  /* reset the memory */
  memset((void *)emm_ctrl_data_ptr->out_msg_ptr, 0x0,
                          sizeof(lte_nas_outgoing_msg_type)); 

  ul_generic_transport = &(emm_ctrl_data_ptr->out_msg_ptr-> \
                           emm_outgoing_msg.ul_generic_nas_transport_msg);

  emm_ctrl_data_ptr->out_msg_ptr->nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES; 
  emm_ctrl_data_ptr->out_msg_ptr->nas_hdr.msg_id   = UL_GENERIC_NAS_TRANSPORT; 
  ul_generic_transport->generic_msg_container_type = 
                             emm_cmd_ptr->gps_msgr_cmd.generic_msg_container_type; 

  emm_state      = EMM_GET_STATE(); 
  emm_substate   = EMM_GET_SUBSTATE();


  switch(emm_state)
  {  
    case EMM_REGISTERED:
      if((emm_ctrl_data_ptr->rrc_active == TRUE) &&
         ((emm_substate == EMM_REGISTERED_NORMAL_SERVICE) ||
          (emm_substate == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM))
         &&
         ((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                          SYS_SRV_STATUS_SRV)
#ifdef FEATURE_LTE_REL9
          ||
          ((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                     SYS_SRV_STATUS_LIMITED) &&
           (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY))
#endif
         ) 
#ifdef FEATURE_LTE_REL10
        &&
        ((mm_timer_status[TIMER_T3346] == TIMER_STOPPED)
#ifdef FEATURE_LTE_REL11
          || 
          ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
           ((emm_check_high_priority(emm_ctrl_data_ptr)) ||
            (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE)))
#endif
        )
#endif
         &&(mm_timer_status[TIMER_T3440] == TIMER_STOPPED)
         &&(emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED)
         && (emm_reg_container == NULL)
         && ((EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE) ||
             (emm_mmtel_status(emm_ctrl_data_ptr) == TRUE) ||
             (emm_wms_sms_status(emm_ctrl_data_ptr) == TRUE)||
             (emm_check_mplmn_in_progress(emm_ctrl_data_ptr) == FALSE)))
      {        
        /* allocate nas memory for items pulled out of dsm*/
        emm_allocate_ul_generic_iei_memory(num_attach,
                                           dsm_arr_ptr,
                                           ul_generic_transport,
                                           emm_ctrl_data_ptr);

        if(EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE)
        {
          (void)emm_send_message(emm_ctrl_data_ptr->out_msg_ptr,
                                  emm_cmd_ptr->gps_msgr_cmd.trans_id, 
                                  MSGR_NAS_EMM, 
                                  emm_ul_generic_nas_transport_sucess, 
                                  emm_ul_generic_nas_transport_failed, 
                                 emm_ctrl_data_ptr);
        }
        else
        {
         MSG_HIGH_DS_0(MM_SUB, "=EMM= Cache UL GENERIC NAS Message");
         (void)emm_cache_new_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                   emm_cmd_ptr->gps_msgr_cmd.trans_id,
                                   MSGR_NAS_EMM, 
                                   emm_ul_generic_nas_transport_sucess, 
                                   emm_ul_generic_nas_transport_failed, 
                                   emm_ctrl_data_ptr);
          if(emm_check_esr_for_ps(emm_ctrl_data_ptr) == TRUE)
          {
            /* Start extended service request procedure for PS */
            emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MO_DATA,LTE_PACKET_SERVICES_VIA_S1,
                                                             csfb_rsp, NAS_EMM_CSFB_NONE);
          }
          else
          {
            /* Start service request procedure with the cause of MO_DATA */
            emm_start_nas_service_request_procedure(emm_ctrl_data_ptr,
                                                   LTE_RRC_EST_CAUSE_MO_DATA);
          }
       }
      }
      else
      {
        MSG_HIGH_DS_3(MM_SUB, "=EMM= free the UL GENERIC NAS msg dsm items %d rrc active %d emm_substate %d",
                      num_attach,
                      emm_ctrl_data_ptr->rrc_active,
                      emm_substate);
        emm_generic_nas_dsm_free(num_attach, dsm_arr_ptr);
        emm_send_generic_nas_failure_ind(LTE_NAS_IRAT_INVALID_STATE,
                                         LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                         emm_cmd_ptr->gps_msgr_cmd.trans_id,
                                         LTE_NAS_IRAT_ACCESS_BARRING_NONE);
      }
      break ;
    case EMM_SERVICE_REQUEST_INITIATED:
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      /* allocate nas memory for items pulled out of dsm*/
      emm_allocate_ul_generic_iei_memory(num_attach,
                                         dsm_arr_ptr,
                                         ul_generic_transport,
                                         emm_ctrl_data_ptr);

      MSG_HIGH_DS_1(MM_SUB, "=EMM= Cache UL Generic NAS message emm_state %d", emm_state);
     (void)emm_cache_new_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                 emm_cmd_ptr->gps_msgr_cmd.trans_id,
                                 MSGR_NAS_EMM, 
                                 emm_ul_generic_nas_transport_sucess, 
                                 emm_ul_generic_nas_transport_failed, 
                                 emm_ctrl_data_ptr);
      break ;
    default:
      MSG_HIGH_DS_2(MM_SUB, "=EMM= free the UL GENERIC NAS msg dsm items %d emm_state %d",
                    num_attach, emm_state);
      emm_generic_nas_dsm_free(num_attach, dsm_arr_ptr);
  
      emm_send_generic_nas_failure_ind(LTE_NAS_IRAT_INVALID_STATE,
                                       LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                       emm_cmd_ptr->gps_msgr_cmd.trans_id,
                                       LTE_NAS_IRAT_ACCESS_BARRING_NONE);
      break;
  }

  return TRUE;
}


/*===========================================================================
FUNCTION    EMM_PROCESS_DL_GENERIC_NAS_TRANSPORT_MESSAGE

DESCRIPTION
  This function puts the incoming RAW buffer from RRC module, into the correct 
  'C' structure that MSG_LIB identifies and then sends it to the GPS module

DEPENDENCIES
  None

RETURN VALUE
  NONE 

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_dl_generic_nas_transport_msg
(
  lte_nas_emm_dl_generic_nas_transport *dl_gen_nas_msg_ptr,
  emm_ctrl_data_type                   *emm_ctrl_data_ptr
)
{  
  lte_nas_emm_dl_generic_transport_s_type dl_generic_cmd;
  dsm_item_type *dsm_ptr = NULL,*dsm_opt_iei_ptr = NULL;
  msgr_attach_struct_type *att_ptr = NULL, *att_iei_ptr = NULL;  
  IxErrnoType error_number = E_SUCCESS;

  ASSERT(dl_gen_nas_msg_ptr != NULL) ; 
  ASSERT(emm_ctrl_data_ptr != NULL) ;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= received DL_GENERIC_NAS_TRANSPORT");

  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL9)
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Spec version '%d',drop this message",
                   emm_ctrl_data_ptr->emm_3gpp_spec_ver);
    return;
  }

  /* reset the structure */
  memset(&dl_generic_cmd,0,sizeof(lte_nas_emm_dl_generic_transport_s_type));

  /* if additional info exists initilize attach_ptr for two */
  if(dl_gen_nas_msg_ptr->additional_info_exists)
  {
     msgr_init_hdr_attach((msgr_hdr_s *)&dl_generic_cmd.msg_hdr, 
                          MSGR_NAS_EMM, NAS_EMM_DL_GENERIC_NAS_TRANSPORT_IND, 0, 2);
  }
  else
  {
     msgr_init_hdr_attach((msgr_hdr_s *)&dl_generic_cmd.msg_hdr, 
                          MSGR_NAS_EMM, NAS_EMM_DL_GENERIC_NAS_TRANSPORT_IND, 0, 1);
  }
  
  /* container type to be sent */
  dl_generic_cmd.generic_msg_container_type = 
                                dl_gen_nas_msg_ptr->generic_msg_container_type;


  /*push down into appropriate pool*/                          
  emm_dsm_pushdown((uint16)dl_gen_nas_msg_ptr->generic_msg_container.container_length,
                   &dsm_ptr, 
                   dl_gen_nas_msg_ptr->generic_msg_container.container_value_ptr,
                   emm_ctrl_data_ptr);

  /* Set DSM attach */
  att_ptr = msgr_get_attach(&dl_generic_cmd.msg_hdr, 0);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);
  ASSERT(dsm_ptr != NULL);  

  /* attach another dsm if additional info exists */
  if(dl_gen_nas_msg_ptr->additional_info_exists)
  {
    /*push down into appropriate pool*/                          
    emm_dsm_pushdown((uint16)dl_gen_nas_msg_ptr->additional_info.additional_info_length,
                     &dsm_opt_iei_ptr, 
                     dl_gen_nas_msg_ptr->additional_info.additional_info_value_ptr,
                     emm_ctrl_data_ptr);

    /* Set DSM attach */
    att_iei_ptr = msgr_get_attach(&dl_generic_cmd.msg_hdr, 1);
    ASSERT(att_iei_ptr != NULL);
    msgr_set_dsm_attach(att_iei_ptr, dsm_opt_iei_ptr);
    ASSERT(dsm_opt_iei_ptr != NULL); 
  }

  error_number = emm_msgr_send((msgr_hdr_s *)&dl_generic_cmd, 
               sizeof(lte_nas_emm_dl_generic_transport_s_type));

  MSG_HIGH_DS_1(MM_SUB, "=EMM= NAS_EMM_DL_GENERIC_NAS_TRANSPORT_IND : Success(0)/Failure(non Zero) = %d", error_number);


#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */  
}

#endif /*FEATURE_LTE && FEATURE_LTE_REL9 */
