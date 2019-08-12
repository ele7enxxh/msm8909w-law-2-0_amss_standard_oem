/*===========================================================================

                         D S _ Q M I _ V O I C E _MSGR_IF . C

DESCRIPTION

 The Data Services Qualcomm Voice service Messenger interface source file.

EXTERNALIZED FUNCTIONS     

Copyright (c) 2004-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_voice_msgr_if.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/2313         Created Module.
===========================================================================*/

/*===========================================================================
 
  INCLUDE FILES FOR MODULE
 
===========================================================================*/


#include "cm.h"
#include "qmi_voice_msgr_if.h"
#include "qmi_mmode_msgr_msg.h"
#include "qmi_voice_call_list.h"
#include "qmi_voice_cm_util.h"
#include "modem_mem.h"
#include "qm_util.h"

#ifdef FEATURE_DUAL_SIM
#include  "cm_dualsim.h"
#endif /*FEATURE_DUAL_SIM*/

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*===========================================================================

                                DATA TYPES

===========================================================================*/


/*===========================================================================

                               INTERNAL DATA

===========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
  FUNCTION qmi_voice_ims_extn_handle_srvcc_handover_complete()

  DESCRIPTION
    Handle the SRVCC complete for other IMS cases

  PARAMETERS
    call_info_ptr:  cm_mm_call_info_s_type
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_voice_ims_extn_handle_srvcc_handover_complete
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
);

/*===========================================================================
  FUNCTION qmi_voice_msgr_conf_participant_status_ind()

  DESCRIPTION
    Send the status of add participant operation

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voice_msgr_conf_participant_status_ind
(
  mmode_qmi_voice_participant_status_cmd_msg_type *participant_status_info
);

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

static void qmi_voice_msgr_parsed_conf_info_ind
(
  mmode_qmi_voice_conf_participants_info_cmd_msg_type *conf_cmd_info
);

static void qmi_voice_msgr_srvcc_ho_complete
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data
);

static void qmi_voice_msgr_drvcc_ho_complete
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data
);

static void qmi_voice_msgr_tty_info_ind
(
  mmode_qmi_voice_tty_mode_info_cmd_msg_type *tty_cmd_info
);

static void qmi_voice_msgr_audio_rat_change_info_ind
(
  mmode_qmi_voice_audio_session_rat_change_cmd_msg_type *audio_rat_change_cmd_info
);

/*===========================================================================
            
                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*========================================================================
  FUNCTION qmi_voice_msgr_handler

  DESCRIPTION
    process messages received from msgr

  PARAMETERS
    msg pointer in *void. cast to qmi_mmodei_msgr_type before use.
    *void type is to avoid circular reference of headers

  RETURN VALUE
    None
===========================================================================*/
void qmi_voice_msgr_handler( void *param )
{

  qmi_mmodei_msgr_type       *msg;

  ASSERT( param );

  msg = (qmi_mmodei_msgr_type*)param;
  QM_MSG_MED_1("qmi_voice_msgr_handler cmd id = %d", msg->cmd.hdr.id);

  switch ( msg->cmd.hdr.id )
  {
    case QMI_VOICE_CONF_PARTICIPANTS_INFO_CMD:
      qmi_voice_msgr_parsed_conf_info_ind( &msg->cmd.qmi_voice.parsed_conf_cmd);
      break;

    case MM_CM_SRVCC_HO_COMPLETE_IND:
      qmi_voice_msgr_srvcc_ho_complete( &msg->cmd.qmi_voice.mid_call_srvcc_handover_ind);
      break;

    case MM_CM_DRVCC_HO_COMPLETE_IND:
      QM_MSG_HIGH("DRVCC_HO_COMPLETE");
      qmi_voice_msgr_drvcc_ho_complete( &msg->cmd.qmi_voice.mid_call_srvcc_handover_ind);
      break;

    case QMI_VOICE_TTY_MODE_INFO_CMD:
      qmi_voice_msgr_tty_info_ind( &msg->cmd.qmi_voice.tty_info_cmd);
      break;

    case QMI_VOICE_AUDIO_RAT_CHANGE_INFO_CMD:
      qmi_voice_msgr_audio_rat_change_info_ind( &msg->cmd.qmi_voice.audio_rat_change_info_cmd);
      break;

    case QMI_VOICE_CONF_PARTICIPANT_STATUS_INFO_CMD:
      qmi_voice_msgr_conf_participant_status_ind( &msg->cmd.qmi_voice.participant_status_info_cmd);
      break;

    case QMI_VOICE_VICE_DIALOG_IND:
      qmi_voice_msgr_vice_dialog_info_ind( &msg->cmd.qmi_voice.vice_dialog_info_cmd);
      break;

    default:
      /*not a QMI_VOICE handled msg */
      break;
    }
  }

/*========================================================================
  FUNCTION qmi_voice_msgr_register

  DESCRIPTION
    register to msgr messages

  PARAMETERS
    msgr client object pointer

  RETURN VALUE
    None
===========================================================================*/
void qmi_voice_msgr_register( msgr_client_t *msgr_client )
{
  errno_enum_type err;

  err = msgr_register( MSGR_QMI_VOICE, msgr_client, MSGR_ID_REX, QMI_VOICE_CONF_PARTICIPANTS_INFO_CMD );
  QM_MSG_HIGH_1("QMI_VOICE_CONF_PARTICIPANTS_INFO_CMD register ret %d", err);

  err = msgr_register( MSGR_QMI_VOICE, msgr_client, MSGR_ID_REX, MM_CM_SRVCC_HO_COMPLETE_IND );
  QM_MSG_HIGH_1("SRVCC_HO_COMPLETE register ret %d", err);

  err = msgr_register( MSGR_QMI_VOICE, msgr_client, MSGR_ID_REX, QMI_VOICE_TTY_MODE_INFO_CMD );
  QM_MSG_HIGH_1("QMI_VOICE_TTY_MODE_INFO_CMD register ret %d", err);

  err = msgr_register( MSGR_QMI_VOICE, msgr_client, MSGR_ID_REX, QMI_VOICE_AUDIO_RAT_CHANGE_INFO_CMD );
  QM_MSG_HIGH_1("QMI_VOICE_AUDIO_RAT_CHANGE_INFO_CMD register ret %d", err);

  err = msgr_register( MSGR_QMI_VOICE, msgr_client, MSGR_ID_REX, QMI_VOICE_CONF_PARTICIPANT_STATUS_INFO_CMD );
  QM_MSG_HIGH_1("QMI_VOICE_CONF_PARTICIPANT_STATUS_INFO_CMD register ret %d", err);

  err = msgr_register( MSGR_QMI_VOICE, msgr_client, MSGR_ID_REX, QMI_VOICE_VICE_DIALOG_IND);
  QM_MSG_HIGH_1("VICE Dialog info %d", err);

  err = msgr_register( MSGR_QMI_VOICE, msgr_client, MSGR_ID_REX, MM_CM_DRVCC_HO_COMPLETE_IND );
  QM_MSG_HIGH_1("DRVCC_HO_COMPLETE register ret %d", err);

  ASSERT( err == E_SUCCESS );
}

/*========================================================================
  FUNCTION qmi_voice_msgr_send_srvcc_config_req

  DESCRIPTION
    to send the srvcc call context info request

  PARAMETERS
    Params ptr which has all the other call context data received from clients

  RETURN VALUE
    IxErrnoType : Success or Failure of operation
===========================================================================*/
IxErrnoType qmi_voice_msgr_send_srvcc_config_req
(
  qmi_voice_cm_if_set_srvcc_call_context_s *const in_data_ptr,
  qmi_voice_cm_if_as_id_e_type as_id
)
{
  IxErrnoType result = E_SUCCESS;
  cm_srvcc_call_context_rsp_s_type *srvcc_context_info_ptr = NULL;
  uint8 i=0, cm_conf_idx=0, cm_participant_idx=0;
  boolean is_cm_conf_info_filled=FALSE;
  cm_call_substate_type call_sub_state;
  cm_mid_srvcc_participant_info  *conf_participant = NULL;

  srvcc_context_info_ptr = (cm_srvcc_call_context_rsp_s_type *)QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(cm_srvcc_call_context_rsp_s_type));
  if ( NULL == srvcc_context_info_ptr )
  {
	  qmi_voice_mem_fatal();
    return E_NO_MEMORY;
  }

  memset(srvcc_context_info_ptr,0,sizeof(cm_srvcc_call_context_rsp_s_type));

  srvcc_context_info_ptr->num_of_calls = 0;

  /* copy call contexts from qmi_voice to cm structure*/
  if(in_data_ptr->num_calls > 0)
  {
    for(i=0;i<in_data_ptr->num_calls;i++)
    {
      if(in_data_ptr->srvcc_calls[i].is_mpty_call != 1)
      {
        cm_mid_srvcc_ip_call_info  *ip_call = &srvcc_context_info_ptr->call_context[srvcc_context_info_ptr->num_of_calls].call_info.ip_call;

        /*copy IP call info */
        srvcc_context_info_ptr->call_context[srvcc_context_info_ptr->num_of_calls].is_conf_call = FALSE;
        ip_call->call_id = in_data_ptr->srvcc_calls[i].instance_id;
        ip_call->call_type = qmi_voice_cm_util_map_qmi_to_cm_call_type((qmi_voice_cm_if_call_type_e_type)in_data_ptr->srvcc_calls[i].call_type);
        qmi_voice_util_map_qmi_to_cm_call_state_substate(&ip_call->call_state,
                                                         &ip_call->call_sub_state,
                                                         (qmi_voice_cm_if_call_state_e_type)in_data_ptr->srvcc_calls[i].call_state,
                                                         (qmi_voice_cm_if_call_substate_e_type)in_data_ptr->srvcc_calls[i].call_substate);
        ip_call->call_direction = in_data_ptr->srvcc_calls[i].direction;
        
        if(in_data_ptr->srvcc_calls[i].is_caller_name_type_valid)
        {
          ip_call->srvcc_caller_info.caller_name_pi = in_data_ptr->srvcc_calls[i].name_pi;
          ip_call->srvcc_caller_info.caller_len     = in_data_ptr->srvcc_calls[i].name_len;

          /*Allocating memory for Caller Name, which will be freed by CM once they copy it to their call list*/
          ip_call->srvcc_caller_info.caller_name = QMI_VOICE_CM_IF_MEM_ALLOC(2* in_data_ptr->srvcc_calls[i].name_len);
          if(NULL == ip_call->srvcc_caller_info.caller_name)
          {
            if(i > 0)
            {
              int j = i - 1;
              for(;  j>=0; j--)
              {
                /*Freeing the already allocated memory blocks*/
                cm_mid_srvcc_ip_call_info  *ip_call = &srvcc_context_info_ptr->call_context[j].call_info.ip_call;
                QM_MSG_MED("Freeing the already allocated memory");
                QMI_VOICE_CM_IF_MEM_FREE(ip_call->srvcc_caller_info.caller_name);
              }
            }
            QM_MSG_FATAL("No enough memory");
            return E_NO_MEMORY;
          }
          QM_MSG_HIGH_2("in_data_ptr->srvcc_calls[%d].name_len = %d", i, in_data_ptr->srvcc_calls[i].name_len);
          memscpy(ip_call->srvcc_caller_info.caller_name,
                  in_data_ptr->srvcc_calls[i].name_len*2,
                  in_data_ptr->srvcc_calls[i].caller_name,
                  in_data_ptr->srvcc_calls[i].name_len*2);
        }
        
        if(in_data_ptr->srvcc_calls[i].is_alerting_type_valid)
        {
          ip_call->call_alert_media_type = qmi_voice_cm_util_map_qmi_to_cm_alerting_type(in_data_ptr->srvcc_calls[i].alerting_type);
        }

        if(in_data_ptr->srvcc_calls[i].is_num_pi_valid)
        {
        ip_call->call_num.pi = in_data_ptr->srvcc_calls[i].num_pi;
        }
        
        ip_call->call_num.len = in_data_ptr->srvcc_calls[i].num_len;
        memscpy(ip_call->call_num.buf,
                sizeof(ip_call->call_num.buf),
                in_data_ptr->srvcc_calls[i].num,
                in_data_ptr->srvcc_calls[i].num_len);

        srvcc_context_info_ptr->num_of_calls++;
      }
      else
      {
        if(!is_cm_conf_info_filled)
        {
          is_cm_conf_info_filled = TRUE;
          cm_conf_idx = srvcc_context_info_ptr->num_of_calls;
          srvcc_context_info_ptr->num_of_calls++;
          srvcc_context_info_ptr->call_context[cm_conf_idx].call_info.ip_conf_call.num_of_participants =0;
 
          /* Fill CM overall conference call info*/
          srvcc_context_info_ptr->call_context[cm_conf_idx].is_conf_call = TRUE;
          srvcc_context_info_ptr->call_context[cm_conf_idx].call_info.ip_conf_call.call_id = CM_CALL_ID_UNASSIGNED;
          qmi_voice_util_map_qmi_to_cm_call_state_substate(&srvcc_context_info_ptr->call_context[cm_conf_idx].call_info.ip_conf_call.conf_call_state,
                                                           &srvcc_context_info_ptr->call_context[cm_conf_idx].call_info.ip_conf_call.conf_call_substate,
                                                           (qmi_voice_cm_if_call_state_e_type)in_data_ptr->srvcc_calls[i].call_state,
                                                           (qmi_voice_cm_if_call_substate_e_type)in_data_ptr->srvcc_calls[i].call_substate);
        }
        
        conf_participant = &srvcc_context_info_ptr->call_context[cm_conf_idx].call_info.ip_conf_call.participant_list[cm_participant_idx];

        /* Fill CM conference participant info*/
        conf_participant->participant_id = in_data_ptr->srvcc_calls[i].instance_id;
        conf_participant->call_type = qmi_voice_cm_util_map_qmi_to_cm_call_type((qmi_voice_cm_if_call_type_e_type)in_data_ptr->srvcc_calls[i].call_type);
        qmi_voice_util_map_qmi_to_cm_call_state_substate(&conf_participant->call_state,
                                                         &call_sub_state,
                                                         (qmi_voice_cm_if_call_state_e_type)in_data_ptr->srvcc_calls[i].call_state,
                                                         (qmi_voice_cm_if_call_substate_e_type)in_data_ptr->srvcc_calls[i].call_substate);

        conf_participant->call_direction = in_data_ptr->srvcc_calls[i].direction;
	conf_participant->participant_num.len = in_data_ptr->srvcc_calls[i].num_len;
        memscpy(conf_participant->participant_num.buf,
                sizeof(conf_participant->participant_num.buf),
                in_data_ptr->srvcc_calls[i].num,
                in_data_ptr->srvcc_calls[i].num_len);
 
        cm_participant_idx++;
        srvcc_context_info_ptr->call_context[cm_conf_idx].call_info.ip_conf_call.num_of_participants++;
      }
    }
    srvcc_context_info_ptr->as_id = as_id;
  }

  msgr_init_hdr( (msgr_hdr_struct_type*)srvcc_context_info_ptr, MSGR_QMI_VOICE, MM_CM_SRVCC_CONTEXT_RSP );

  result = msgr_send((msgr_hdr_struct_type*)srvcc_context_info_ptr, sizeof(cm_srvcc_call_context_rsp_s_type));

  QM_MSG_HIGH_1("SRVCC call context msgr_send ret=%d", result);

  QMI_VOICE_CM_IF_MEM_FREE(srvcc_context_info_ptr);

  return result;
}

/*===========================================================================
  FUNCTION qmi_voice_msgr_parsed_conf_info_ind()

  DESCRIPTION
    Send handover indication based on the call event 

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voice_msgr_parsed_conf_info_ind
(
  mmode_qmi_voice_conf_participants_info_cmd_msg_type *conf_cmd_info
)
{
  voice_conf_participants_info_ind_msg_v02 *ind;
  uint8 i=0,j=0;
  mmode_qmi_conference_call_info_s *conf_info;
  uint8 as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;
 
  if(conf_cmd_info == NULL)
  {
    QM_MSG_ERROR("qmi_voice_msgr_parsed_conf_info_ind: conf_cmd_info field is NULL");
    return;
  }

  // Allocate memory to the qmi voice indication structure
  ind = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(*ind));
  if( NULL == ind )
  {
	  qmi_voice_mem_fatal();
    return;
  }
 
  conf_info = &(conf_cmd_info->conf_call_info);
 
  //Inside qmi_voice_msgr_parsed_conf_info_ind

  memset(ind, 0, sizeof(*ind));

  ind->conf_call_info.update_type =  (voice_update_type_enum_v02) conf_info->type; 
  ind->conf_call_info.conf_participant_info_len = conf_info->conference_call_count; 
  QM_MSG_MED_2("msgr_parsed_conf_info_ind(): Update type = %d, call_count=%d", ind->conf_call_info.update_type, ind->conf_call_info.conf_participant_info_len);

  for(i=0;i<MIN(conf_info->conference_call_count,MMODE_QMI_NUM_CONF_PARTICIPANTS_MAX);i++)
  {
    mmode_qmi_conference_participant_info  *ims_part = &conf_info->call_info[i];
    voice_conf_participant_call_info_type_v02   *qmi_part = &(ind->conf_call_info.conf_participant_info[i]);
           
    
    qmi_part->user_uri_len = ims_part->user_uri_len;
    memscpy(qmi_part->user_uri, sizeof(qmi_part->user_uri), ims_part->user_uri, ims_part->user_uri_len*2);
    //Printing USR_URI received
    for(j=0;j<ims_part->user_uri_len;j++)
    {
      QM_MSG_HIGH_2("USR_URI: char [%d] is 0x%x", j, ims_part->user_uri[j]);
    }
    
    qmi_part->audio_attributes = (voice_call_attribute_type_mask_v02)ims_part->audio_attrib;
    qmi_part->video_attributes = (voice_call_attribute_type_mask_v02)ims_part->video_attrib;
    qmi_part->disconnection_method = (voice_conf_part_disconnection_method_enum_v02)ims_part->disconnection_method;
    qmi_part->status = (voice_conf_part_call_status_enum_v02)ims_part->status;

    QM_MSG_MED_7("Call %d, uri_len= %d, disc_len= %d, Audio = %d, Video=%d, Disc Method = %d, Status=%d", i, ims_part->user_uri_len,
ims_part->disconnection_info_len,
 qmi_part->audio_attributes,
 qmi_part->video_attributes,
 qmi_part->disconnection_method, 
 qmi_part->status);     

    qmi_part->disconnection_info_len = ims_part->disconnection_info_len;
    memscpy(qmi_part->disconnection_info, sizeof(qmi_part->disconnection_info), ims_part->disconnection_info, ims_part->disconnection_info_len); 
    //Printing DISC_INFO received
    for(j=0;j<ims_part->disconnection_info_len;j++)
    {
      QM_MSG_HIGH_2("DISC_INFO: char [%d] is 0x%x", j, ims_part->disconnection_info[j]);
    }
  }
  
  qmi_voice_cm_if_unsol_response(QMI_VOICE_CM_IF_CMD_CONF_PARTICIPANTS_INFO_IND, (void *) ind, sizeof(*ind),as_id);

  if(ind!=NULL)
  {
    QMI_VOICE_CM_IF_MEM_FREE(ind);
  }
} /* qmi_voice_msgr_parsed_conf_info_ind() */

/*===========================================================================
  FUNCTION QMI_VOICE_MSGR_SRVCC_HO_COMPLETE()

  DESCRIPTION
    Send handover indication based on the call event 

  PARAMETERS
    event          : Call event received
    call_info_ptr  : Call info from CM
    as_id          : Subscription id

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voice_msgr_srvcc_ho_complete
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data
)
{ 
  boolean success = TRUE;
  uint8 as_id = (uint8)QMI_VOICE_CM_IF_AS_ID_PRIMARY;  
  qmi_voice_call_list_call_ids_list_type incom_list;

  QM_MSG_MED_2("msgr_srvcc_ho_complete() cm_data:%d and MID-SRVCC: Num of CS calls = %d", 
  cm_data, cm_data ? cm_data->number_calls: 0);
  
  if(cm_data == NULL)
  {
    return;
  }

  if(cm_data->is_int_ims)
  {
    success = qmi_voice_call_list_update_srvcc_calls_info(cm_data, &as_id);
  }
  else
  {
    success = qmi_voice_ims_extn_handle_srvcc_handover_complete(cm_data, &as_id);
  }

  if(success)
  {
    // post processing the call list to update incoming to waiting is already active call is present
    qmi_voice_call_list_query_voice_call_ids_list_by_state(QMI_VOICE_CALL_LIST_STATE_INCOMING, &incom_list, as_id);
    QM_MSG_HIGH_1("Number of calls in incoming state: %d", incom_list.num_of_call_ids);
 
    if(incom_list.num_of_call_ids !=0)
     {
       if(qmi_voice_call_list_count_num_of_voice_calls_by_state((uint16)(QMI_VOICE_CALL_LIST_STATE_ACTIVE | QMI_VOICE_CALL_LIST_STATE_HOLDING), as_id ) > 0 )
       {
         if (qmi_voice_call_list_update_call_state(incom_list.call_id[0], QMI_VOICE_CALL_LIST_STATE_WAITING) != E_SUCCESS)
         {
           QM_MSG_ERROR_1("Fail to update call list entry state for incom:  call_id %d", incom_list.call_id[0]);
         }
       }
     }

    qmi_voice_call_list_report_all_call_status_changed(as_id);
  }  

  qmi_voice_call_list_reset_srvcc_available_flags();
  //The is_srvcc flag is only to be sent in the first call indication following the SRVCC.

  qmi_voice_call_list_reset_srvcc_caller_name_flags();
  //The SRVCC caller name info should be sent only once when the Handover is complete. It should not be sent after that.

  qmi_voice_call_list_reset_parent_id_valid_flags();
  //The parent_id is only to be sent in the first call indication following the SRVCC.
} /* qmi_voice_msgr_srvcc_ho_complete() */

/*===========================================================================
  FUNCTION QMI_VOICE_MSGR_DRVCC_HO_COMPLETE()

  DESCRIPTION
    Send handover indication based on the call event 

  PARAMETERS
    event          : Call event received
    call_info_ptr  : Call info from CM
    as_id          : Subscription id

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voice_msgr_drvcc_ho_complete
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data
)
{ 
  boolean success = TRUE;
  uint8 as_id = (uint8)QMI_VOICE_CM_IF_AS_ID_PRIMARY;  

  QM_MSG_MED("Inside qmi_voice_drvcc_handover_complete():");
  
  if(cm_data == NULL)
  {
    QM_MSG_ERROR("qmi_voice_msgr_drvcc_ho_complete: cm_data field is NULL");
    return;
  }

  QM_MSG_MED_1("DRVCC: Num of CS calls = %d", cm_data->number_calls);

  if(cm_data->is_int_ims)
  {
    success = qmi_voice_call_list_update_drvcc_calls_info(cm_data, &as_id);
  }
  else
  {
    QM_MSG_ERROR("Unsupported is_int_ims = 0 for DRVCC");
  }

  if(success)
  {
    qmi_voice_call_list_report_all_call_status_changed(as_id);
  }  

  qmi_voice_call_list_reset_drvcc_available_flags();
  //The is_drvcc flag is only to be sent in the first call indication following the DRVCC.

  qmi_voice_call_list_reset_drvcc_parent_id_valid_flags();
  //The drvcc_parent_id is only to be sent in the first call indication following the DRVCC.
} /* qmi_voice_msgr_drvcc_ho_complete() */

/*===========================================================================
  FUNCTION qmi_voice_msgr_tty_info_ind()

  DESCRIPTION
    Send tty info indication based on the call event 

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voice_msgr_tty_info_ind
(
  mmode_qmi_voice_tty_mode_info_cmd_msg_type *tty_cmd_info
)
{
  voice_tty_ind_msg_v02 tty_ind;
  uint8 as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  memset(&tty_ind,0,sizeof(tty_ind));

  tty_ind.tty_mode = (tty_mode_enum_v02)tty_cmd_info->tty_mode;

  qmi_voice_cm_if_unsol_response(QMI_VOICE_CM_IF_CMD_TTY_INFO_IND,(void *) &tty_ind, sizeof(tty_ind),as_id);

} /* qmi_voice_msgr_tty_info_ind() */


/*===========================================================================
  FUNCTION qmi_voice_msgr_audio_rat_change_info_ind()

  DESCRIPTION
    Send audio RAT change info indication based on the call event 

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voice_msgr_audio_rat_change_info_ind
(
  mmode_qmi_voice_audio_session_rat_change_cmd_msg_type *audio_rat_change_cmd_info
)
{
  voice_audio_rat_change_info_ind_msg_v02  audio_rat_change_ind;
  uint8 as_id;

  memset(&audio_rat_change_ind,0,sizeof(audio_rat_change_ind));

  as_id                                   = audio_rat_change_cmd_info->audio_rat_change.asid;
  audio_rat_change_ind.audio_session_info_valid = TRUE;
  audio_rat_change_ind.audio_session_info = (audio_session_enum_v02)audio_rat_change_cmd_info->audio_rat_change.audio_session;
  audio_rat_change_ind.rat_info_valid = TRUE;
  audio_rat_change_ind.rat_info = (call_mode_enum_v02)audio_rat_change_cmd_info->audio_rat_change.rat;

  QM_MSG_MED_3("msgr_audio_rat_change_info_ind(): as_id=%d, audio_session_info=%d, rat_info=%d", as_id, audio_rat_change_ind.audio_session_info,
  	        audio_rat_change_ind.rat_info);

  qmi_voice_cm_if_unsol_response(QMI_VOICE_CM_IF_CMD_AUDIO_RAT_CHANGE_INFO_IND,(void *) &audio_rat_change_ind, sizeof(audio_rat_change_ind),as_id);

} /* qmi_voice_msgr_audio_rat_change_info_ind() */

/*===========================================================================
  FUNCTION qmi_voice_msgr_conf_participant_status_ind()

  DESCRIPTION
    Send the status of add participant operation

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voice_msgr_conf_participant_status_ind
(
  mmode_qmi_voice_participant_status_cmd_msg_type *participant_status_info
)
{
  voice_conf_participant_status_info_ind_msg_v02  part_status_ind;
  uint8 qmi_call_id;
  uint8 as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  memset(&part_status_ind,0,sizeof(voice_conf_participant_status_info_ind_msg_v02));

  if(qmi_voice_call_list_get_conn_idx_from_call_id(participant_status_info->participant_status.cm_call_id, &qmi_call_id) == E_SUCCESS)
  {
    part_status_ind.call_id = qmi_call_id;
    part_status_ind.is_qmi_voice_transfer = participant_status_info->participant_status.is_qmi_voice_transfer;
    part_status_ind.op_status.operation = (conf_participant_operation_enum_v02) participant_status_info->participant_status.operation;	
	part_status_ind.op_status.sip_status = participant_status_info->participant_status.status_sip_code;	
    memscpy(part_status_ind.participant_uri,
		    sizeof(part_status_ind.participant_uri),
		    participant_status_info->participant_status.participant_uri,
		    participant_status_info->participant_status.participant_uri_len);

	part_status_ind.op_status_valid = TRUE;
  part_status_ind.is_qmi_voice_transfer_valid = TRUE;

	/*Get as id of the call */
	qmi_voice_call_list_get_as_id(participant_status_info->participant_status.cm_call_id, &as_id);
  }
  else
  {
    //qmi_voice_msgr_conf_participant_status_ind: Invalid call ID from IMS
	return;
  }

  QM_MSG_MED_3("conf_participant_status_ind(): Call ID =%d, operation=%d, sip_status=%d", part_status_ind.call_id, part_status_ind.op_status.operation, part_status_ind.op_status.sip_status);

  qmi_voice_cm_if_unsol_response(QMI_VOICE_CM_IF_CMD_CONF_PARTICIPANT_STATUS_IND,
  	                            (void *) &part_status_ind, 
  	                            sizeof(part_status_ind),
  	                            as_id);

} /* qmi_voice_msgr_conf_participant_status_ind() */

/*===========================================================================
  FUNCTION qmi_voice_msgr_vice_dialog_info_ind()

  DESCRIPTION
    Process the VICE dialog event from IMS

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_voice_msgr_vice_dialog_info_ind
(
  mmode_qmi_voice_viceDialog_info_ind_msg_type *vice_dialog_info
)
{
  vice_dialog_info_ind_msg_v02 *vice_info_ind;
 
  uint8 num_indications=0,ind=0;
  uint32 xml_length=0;
  uint8 *xml_ptr=NULL;
  uint8 *ptr_begin = NULL;
  uint32 copy_length=0;
  uint8 as_id = (uint8)QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(vice_dialog_info== NULL)
  {
    QM_MSG_ERROR("qmi_voice_msgr_vice_dialog_info_ind:vice_dialog_info");
    return;
  }
  if(vice_dialog_info->vice_xml == NULL)
  {
    QM_MSG_ERROR("qmi_voice_msgr_vice_dialog_info_ind:vice_dialog_info:vice_xml null");
    return;
  }

  as_id =  INST_ID_TO_SYS_AS_ID(vice_dialog_info->msg_hdr.inst_id);

  vice_info_ind = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(*vice_info_ind));
  if( NULL == vice_info_ind )
  {
    QM_MSG_FATAL("Can't allocate memory in qmi_voice_msgr_send_srvcc_config_req");
    return;
  }

  xml_ptr = vice_dialog_info->vice_xml;
  xml_length = (uint32) strlen( (char*) vice_dialog_info->vice_xml);
  num_indications = (int)(xml_length/QMI_VOICE_VICE_XML_MAX_LEN) + 1;
  
  QM_MSG_MED_1("num_indications=%d", num_indications);
  for(ind=0;ind<num_indications;ind++)
  {
    QM_MSG_MED_1("Sending Indication Number=%d", ind+1);
    memset( vice_info_ind, 0, sizeof(*vice_info_ind) );
    vice_info_ind->sequence = ind;
    if(vice_info_ind->sequence == 0) vice_info_ind->total_size_valid = TRUE;
    vice_info_ind->total_size = xml_length;

    
    ptr_begin = xml_ptr+(ind*QMI_VOICE_VICE_XML_MAX_LEN);
    if(ind == num_indications-1)
    {
      copy_length = xml_length - (ind*QMI_VOICE_VICE_XML_MAX_LEN);
    }
    else
    {
      copy_length = QMI_VOICE_VICE_XML_MAX_LEN;
    }
    QM_MSG_MED_1("copy: copy_len=%d", copy_length);
    vice_info_ind->vice_dialog_xml_len = copy_length;
    memscpy(vice_info_ind->vice_dialog_xml,sizeof(vice_info_ind->vice_dialog_xml),ptr_begin,copy_length);

    //Send indication for every 2k of XMl string
    qmi_voice_cm_if_unsol_response(QMI_VOICE_CM_IF_CMD_VICE_DIALOG_IND,
  	                            (void *) vice_info_ind, 
  	                            sizeof(*vice_info_ind),
  	                            as_id);				
				
  }
  if(vice_info_ind!=NULL)
  {
    QMI_VOICE_CM_IF_MEM_FREE(vice_info_ind);
  }
 

} /* qmi_voice_msgr_vice_dialog_info_ind() */