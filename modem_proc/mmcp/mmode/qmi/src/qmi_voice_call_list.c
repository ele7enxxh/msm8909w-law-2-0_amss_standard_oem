/*===========================================================================

                         Q M I _ V O I C E _ C A L L _ L I S T . C

DESCRIPTION

 The Data Services Qualcomm Voice command list management handled here.

EXTERNALIZED FUNCTIONS


Copyright (c) 2004-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_voice_call_list.c#1 $ $DateTime:$ $Author:$


when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/26/09    sk     Created Module.
===========================================================================*/

/*===========================================================================
 
  INCLUDE FILES FOR MODULE
 
===========================================================================*/
#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_QMI_SERVICE_SPLIT

#include <memory.h>
#include "IxErrno.h"
#include "cm.h"
#include "msg.h"
#include "amssassert.h"
#include "qmi_voice_call_list.h"
#include "qmi_voice_cm_util.h"
#include "qmi_mmode_task_cmd.h"
#include "modem_mem.h"
#include "qm_util.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#ifndef FEATURE_NO_QMI_VOICE

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/


/*! @brief Typedef variables internal to module qmi_voice_call_list.c
*/
typedef struct
{
  qmi_voice_call_list_buf_type *head_ptr; 
  qmi_voice_call_list_buf_type *tail_ptr; 
  uint8 num_of_calls;
} qmi_voice_call_list_struct_type;

/*! @brief Variables internal to module qmi_voice_call_list.c
*/
static qmi_voice_call_list_struct_type call_list;

static boolean is_mpty_recieved_from_mng_calls_conf;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_lookup_state_name

===========================================================================*/
/*!
    @brief
    Look up the call state name from the state
 
    @return
    A pointer to the state name string.
*/
/*=========================================================================*/
static char *qmi_voice_call_list_lookup_state_name
( 
  qmi_voice_call_list_state_e_type state
)
{
  switch( state )
  {
    case QMI_VOICE_CALL_LIST_STATE_IDLE:
      return "Idle";
    case QMI_VOICE_CALL_LIST_STATE_ACTIVE:
      return "Active";
    case QMI_VOICE_CALL_LIST_STATE_HOLDING:
      return "Holding";
    case QMI_VOICE_CALL_LIST_STATE_DIALING:
      return "Dialing";
    case QMI_VOICE_CALL_LIST_STATE_ALERTING:
      return "Alerting";
    case QMI_VOICE_CALL_LIST_STATE_INCOMING:
      return "Incoming";
    case QMI_VOICE_CALL_LIST_STATE_WAITING:
      return "Waiting";
    case QMI_VOICE_CALL_LIST_STATE_DISCONNECTING:
      return "Disconnecting";
    case QMI_VOICE_CALL_LIST_STATE_END:
      return "Ended";
    case QMI_VOICE_CALL_LIST_STATE_SETUP:
      return "Setup";
    case QMI_VOICE_CALL_LIST_STATE_CC_IN_PROG:
      return "CC in Progress";
    default:
      return "Unknown";
  } /* end switch */

} /* qmi_voice_call_list_lookup_state_name */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_find

===========================================================================*/
/*!
    @brief
    Find an entry in the list based on Call ID
 
    @return
    A pointer to the entry if found, NULL otherwise.
*/
/*=========================================================================*/
static qmi_voice_call_list_buf_type *qmi_voice_call_list_find
( 
  cm_call_id_type call_id
)
{
  qmi_voice_call_list_buf_type *buf_ptr = call_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL )
  {
    if ( buf_ptr->pub.call_id == call_id )
    {             
      QM_MSG_LOW_1("Found call list entry :  call id %d", call_id);
      return buf_ptr;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }
  QM_MSG_ERROR_1("call_list_find(): Call ID %d not found in Call List", call_id);
  return NULL;

} /* qmi_voice_call_list_find() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_find_with_conn_index

===========================================================================*/
/*!
    @brief
    Find an entry in the list based on Connection Index
 
    @return
    A pointer to the entry if found, NULL otherwise.
*/
/*=========================================================================*/
static qmi_voice_call_list_buf_type *qmi_voice_call_list_find_with_conn_index
( 
  uint8 conn_index
)
{
  qmi_voice_call_list_buf_type *buf_ptr = call_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL )
  {
    if ( buf_ptr->pub.conn_index == conn_index )
    {             
      QM_MSG_MED_2("Found call list entry :  conn_index %d, call id %d", conn_index, buf_ptr->pub.call_id);
      return buf_ptr;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  QM_MSG_MED_1("Not found call list entry :  conn index %d", conn_index);

  return NULL;

} /* qmi_voice_call_list_find_with_conn_index() */



/*===========================================================================

  FUNCTION:  qmi_voice_call_list_count_num_of_voice_calls_by_state

===========================================================================*/
/*!
    @brief
    Count the number of entries with CM_CALL_TYPE_VOICE or CM_CALL_TYPE_EMERGENCY 
    as call type with specified state in the list. 
 
    @return
*/
/*=========================================================================*/
uint32 qmi_voice_call_list_count_num_of_voice_calls_by_state
( 
  uint16 state_mask,
  uint8  as_id
)
{
  uint32 num_of_calls = 0;
  qmi_voice_call_list_buf_type *buf_ptr = call_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL )
  {
    if ( QMI_VOICE_CALL_LIST_CALL_TYPE_IS_VOICE( buf_ptr->pub.call_type ) &&
		 ( (!buf_ptr->pub.is_as_id_valid) || (as_id == buf_ptr->pub.asubs_id)) &&
         ( buf_ptr->pub.state & state_mask ) )
    {             
      num_of_calls++;
    }

    buf_ptr = buf_ptr->next_ptr;
  }

  QM_MSG_LOW_2( "Number of voice calls in call list (state mask %lu): %lu", state_mask, num_of_calls );

  return num_of_calls;

} /* qmi_voice_call_list_count_num_of_voice_calls_by_state() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_count_num_of_voice_calls

===========================================================================*/
/*!
    @brief
    Count the number of VOICE Call entries 
 
    @return
*/
/*=========================================================================*/
uint32 qmi_voice_call_list_count_num_of_voice_calls
( 
  uint8  as_id
)
{
  uint32 num_of_calls = 0;
  qmi_voice_call_list_buf_type *buf_ptr = call_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL )
  {
    if ( QMI_VOICE_CALL_LIST_CALL_TYPE_IS_VOICE( buf_ptr->pub.call_type ) &&
         ( (!buf_ptr->pub.is_as_id_valid) || (as_id == buf_ptr->pub.asubs_id)))
    {             
      num_of_calls++;
    }

    buf_ptr = buf_ptr->next_ptr;
  }

  QM_MSG_LOW_1( "Number of voice calls in call list : %lu", num_of_calls );  

  return num_of_calls;

} /* qmi_voice_call_list_count_num_of_voice_calls() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_count_num_of_voice_calls_by_state_and_mode

===========================================================================*/
/*!
    @brief
    Count the number of voice call entries in specific state in the call list
    and with a specific mode

    This function has to be used only after conversation state (MO) or 
    Incoming state (MT) because mode_info_type will not be set properly 
    in DIALING/ALERTING states.    
 
    @return
*/
/*=========================================================================*/
static uint32 qmi_voice_call_list_count_num_of_voice_calls_by_state_and_mode
( 
  uint16 state_mask,
  cm_call_mode_info_e_type mode_info_type,
  uint8  as_id
)
{
  uint32 num_of_calls = 0;
  qmi_voice_call_list_buf_type *buf_ptr = call_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL )
  {
    if ( QMI_VOICE_CALL_LIST_CALL_TYPE_IS_VOICE( buf_ptr->pub.call_type ) &&
        ( (!buf_ptr->pub.is_as_id_valid) || (as_id == buf_ptr->pub.asubs_id)) &&
         ( buf_ptr->pub.state & state_mask ) &&
         (buf_ptr->pub.mode_info_type == mode_info_type))
    {             
      num_of_calls++;
    }

    buf_ptr = buf_ptr->next_ptr;
  }

  QM_MSG_MED_3( "Number of voice calls in call list (state mask %lu) and mode_info_type %d: %lu", state_mask, mode_info_type,num_of_calls );

  return num_of_calls;

} /* qmi_voice_call_list_count_num_of_voice_calls_by_state_and_mode() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_new_buf

===========================================================================*/
/*!
    @brief
    Allocate a new buffer for the list
 
    @return
    A pointer to a free buffer, or NULL if there are no free buffers 
    available.
*/
/*=========================================================================*/
static qmi_voice_call_list_buf_type *qmi_voice_call_list_new_buf( void )
{

  qmi_voice_call_list_buf_type *buf_ptr;

  if ( (call_list.head_ptr != NULL) && (call_list.num_of_calls == QMI_VOICE_CM_IF_CALL_ID_MAX) )
  {
    return NULL;
  }

  /* Allocate new buf to hold call data */
  buf_ptr = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(qmi_voice_call_list_buf_type));

  if(buf_ptr != NULL)
  {
    call_list.num_of_calls++ ;
    memset(buf_ptr, 0, sizeof(qmi_voice_call_list_buf_type));
    return buf_ptr;
  }
  else
  {
    qmi_voice_mem_error();
    return NULL;
  }

} /* qmi_voice_call_list_new_buf() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_new_all

===========================================================================*/
/*!
    @brief
    Add an entry to the list of active calls.
 
    @return
    E_SUCCESS if the entry is created properly
    E_NOT_ALLOWED if the entry was already in the list
    E_NO_MEMORY if there were no buffers
*/
/*=========================================================================*/
static IxErrnoType qmi_voice_call_list_new_all
( 
  cm_call_id_type                     call_id,
  cm_call_type_e_type                 call_type,      
  sys_sys_mode_e_type                 call_mode,
  cm_call_direction_e_type            direction,
  qmi_voice_call_list_state_e_type    state,
  cm_als_line_e_type                  line,           
  cm_num_s_type                       num,            
  cm_alpha_s_type                     alpha,
  cm_call_mode_info_e_type            mode_info_type,
  qmi_voice_call_list_public_type     *info_ptr,
  uint8 as_id,
  cm_ip_call_attrib_info_s_type     call_attrib
)
{ 
  uint8 conn_index;
  qmi_voice_call_list_buf_type *buf_ptr;
  qmi_voice_call_list_public_type *pub_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if (buf_ptr != NULL)
  {
    /* Call ID is already in the list */
    QM_MSG_ERROR_1("Fail to add call list entry:  call id=%d (duplicate)", call_id);
    return E_NOT_ALLOWED;
  }

  buf_ptr = qmi_voice_call_list_new_buf();

  if (buf_ptr == NULL)
  {
    /* No free buffers in the list */
    return E_NO_MEMORY;
  }

  /* Initialize conn_index */
  conn_index = QMI_VOICE_CM_UTIL_CONN_ID_INVALID; 
  if ( QMI_VOICE_CALL_LIST_HANDLE_CALL_TYPES( call_type ) )
  {
    /* Allocate the Connection Index */
    if ( qmi_voice_cm_util_allocate_conn_id( &conn_index ) != E_SUCCESS )
    {
      /* No Connection Index is available */
      QM_MSG_ERROR_1("Fail to add Call List entry:  call id=%d (no available Conn Index)", call_id);
      return E_NOT_ALLOWED;
    }
  }

  pub_ptr = &buf_ptr->pub;

  pub_ptr->conn_index = conn_index;
  pub_ptr->prev_state = QMI_VOICE_CALL_LIST_STATE_IDLE;
  pub_ptr->state = state;
  pub_ptr->call_id = call_id;
  pub_ptr->call_type = call_type;
  pub_ptr->call_mode = call_mode;
  pub_ptr->direction = direction;
  pub_ptr->line = line;           
  pub_ptr->num = num;
  pub_ptr->num.len = MIN(num.len, CM_MAX_NUMBER_CHARS);
  pub_ptr->mode_info_type = mode_info_type;
  pub_ptr->is_privacy_valid = FALSE;
  pub_ptr->is_otasp_status_valid = FALSE;
  pub_ptr->otasp_status = QMI_VOICE_CALL_LIST_OTASP_STATUS_NONE ;
  pub_ptr->is_srv_opt_valid = FALSE;
  pub_ptr->is_secure_call_available = FALSE;
  pub_ptr->is_called_party_ringing_status_avail = FALSE;
  pub_ptr->is_called_party_ringing = FALSE;
  pub_ptr->srv_opt = 0;
  buf_ptr->pub.is_alerting_type_valid = FALSE;
  pub_ptr->is_cnap_info_valid = FALSE;
  memset(&pub_ptr->cnap_info,0,sizeof(pub_ptr->cnap_info));
  pub_ptr->uus_data.mt_user_data.present = FALSE;
  pub_ptr->answered = FALSE;
  pub_ptr->is_alpha_valid = FALSE;
  pub_ptr->is_conn_num_valid = FALSE;
  pub_ptr->is_diagnostics_info_valid = FALSE;
  pub_ptr->is_multi_party = FALSE;
  pub_ptr->is_called_party_num_valid = FALSE;
  pub_ptr->is_redirecting_party_num_valid = FALSE;
  pub_ptr->is_alert_pattern_valid = FALSE;
  pub_ptr->is_audio_attrib_valid = FALSE;
  pub_ptr->audio_attrib = 0;
  pub_ptr->is_video_attrib_valid = FALSE;
  pub_ptr->video_attrib = 0;
  pub_ptr->is_srvcc_call_available = FALSE;
  pub_ptr->is_attrib_status_valid = FALSE;
  pub_ptr->attrib_status = 0;
  pub_ptr->is_srvcc_call = FALSE;
  pub_ptr->is_second_alpha_valid = FALSE;
  memset(&pub_ptr->second_alpha_info, 0, sizeof(pub_ptr->second_alpha_info));
  pub_ptr->is_end_reason_text_available = FALSE;
  memset(&pub_ptr->end_reason_text,0,sizeof(pub_ptr->end_reason_text));
  pub_ptr->is_drvcc_call_available = FALSE;
  pub_ptr->is_drvcc_call = FALSE;
  pub_ptr->drvcc_parent_call_id = CM_CALL_ID_INVALID;
  pub_ptr->is_drvcc_parent_call_id_valid = FALSE;
  pub_ptr->is_drvcc_parent_call_id_cleared = FALSE;

  pub_ptr->is_local_cap_available = FALSE;
  pub_ptr->is_peer_cap_available = FALSE;
  pub_ptr->local_cap.audio_attrib = CM_CALL_ATTRIB_TX_RX;
  pub_ptr->local_cap.video_attrib = CM_CALL_ATTRIB_TX_RX;
  pub_ptr->peer_cap.audio_attrib = CM_CALL_ATTRIB_TX_RX;
  pub_ptr->peer_cap.video_attrib = CM_CALL_ATTRIB_TX_RX;
  pub_ptr->local_cap.audio_restrict_cause = QMI_VOICE_CAP_RESTRICT_CAUSE_NONE;
  pub_ptr->local_cap.video_restrict_cause = QMI_VOICE_CAP_RESTRICT_CAUSE_NONE;
  pub_ptr->peer_cap.audio_restrict_cause = QMI_VOICE_CAP_RESTRICT_CAUSE_NONE;
  pub_ptr->peer_cap.video_restrict_cause = QMI_VOICE_CAP_RESTRICT_CAUSE_NONE;


  if((call_attrib.audio_attrib >= CM_CALL_ATTRIB_NONE) && (call_attrib.audio_attrib <= CM_CALL_ATTRIB_TX_RX) )
  {
    pub_ptr->audio_attrib = (uint32)call_attrib.audio_attrib ;
    pub_ptr->is_audio_attrib_valid = TRUE;
  }

  if((call_attrib.video_attrib >= CM_CALL_ATTRIB_NONE) && (call_attrib.video_attrib <= CM_CALL_ATTRIB_TX_RX) )
  {
    pub_ptr->video_attrib = (uint32)call_attrib.video_attrib ;
    pub_ptr->is_video_attrib_valid = TRUE;
  }

  if((call_attrib.attrib_status >= CM_CALL_ATTRIB_STATUS_OK) && (call_attrib.attrib_status <= CM_CALL_ATTRIB_STATUS_MEDIA_NOT_READY))
  {
    pub_ptr->attrib_status = (uint32)call_attrib.attrib_status;
    pub_ptr->is_attrib_status_valid = TRUE;
  }

  pub_ptr->em_type= call_attrib.em_type;

  //Copy the VS call variant if valid
  if((call_type == CM_CALL_TYPE_VS) && (call_attrib.vs_ver >= CM_IP_VS_RCSE) && (call_attrib.vs_ver <= CM_IP_VS_RCSV5))
  {
    pub_ptr->vs_call_variant = (uint32) qmi_voice_cm_util_map_cm_to_qmi_vs_version(call_attrib.vs_ver);
    pub_ptr->is_vs_call_variant_valid = TRUE;
  }

  #ifdef FEATURE_DUAL_SIM
    pub_ptr->is_as_id_valid = TRUE;
    pub_ptr->asubs_id = as_id;
  #endif
  if(alpha.len > 0)
  {
    memset(&pub_ptr->alpha_info,0,sizeof(cm_alpha_s_type));
    pub_ptr->is_alpha_valid = TRUE;
    //memscpy(&(pub_ptr->alpha_info), sizeof(pub_ptr->alpha_info), &alpha, sizeof(cm_alpha_s_type));
    pub_ptr->alpha_info = alpha;
    QM_MSG_HIGH_2("Call in Origination and Alpha Present:  call id=%d,Alpha len =%d", call_id, alpha.len);
  }

  /* Sort items in the call list by the order of call being setup */
  buf_ptr->prev_ptr = call_list.tail_ptr;
  buf_ptr->next_ptr = NULL;
  if ( call_list.head_ptr == NULL )
  {
    call_list.head_ptr = buf_ptr;
  }             
  if ( call_list.tail_ptr != NULL )
  {
    call_list.tail_ptr->next_ptr = buf_ptr;
  }
  call_list.tail_ptr = buf_ptr;


  QM_MSG_HIGH_9( "Added CL entry : state=%s (%d), call id=%d,call type=%d, call mode=%d, dir=%d,mode_info_type=%d, line=%d, cm_num_len=%d,",
                   qmi_voice_call_list_lookup_state_name( pub_ptr->state ), pub_ptr->state, pub_ptr->call_id,
                   pub_ptr->call_type, pub_ptr->call_mode, pub_ptr->direction,
                   pub_ptr->mode_info_type, pub_ptr->line, num.len );


  QM_MSG_HIGH_8( " num_len=%d, num=%s, num_pres =%d,priv_mode=%d, name=%s, name_pres =%d,is_as_id_valid=%d, as_id=%d,",
                   pub_ptr->num.len, pub_ptr->num.buf, pub_ptr->num.pi, pub_ptr->privacy, pub_ptr->cnap_info.name, pub_ptr->cnap_info.name_presentation,
                   pub_ptr->is_as_id_valid, pub_ptr->asubs_id);

  QM_MSG_HIGH_6( " a_attrib_valid=%d, a_attrib=%d, v_attrib_valid=%d,v_attib=%d, attrib_status_valid=%d, attrib_status=%d",
                   pub_ptr->is_audio_attrib_valid, pub_ptr->audio_attrib,pub_ptr->is_video_attrib_valid,pub_ptr->video_attrib,
                   pub_ptr->is_attrib_status_valid, pub_ptr->attrib_status );

  
  if ( info_ptr != NULL )
  {
    *info_ptr = *pub_ptr;
  }

  return E_SUCCESS;

} /* qmi_voice_call_list_new_all() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_log

===========================================================================*/
/*!
    @brief
    Log call list changes.
 
    @return
    none
*/
/*=========================================================================*/
void qmi_voice_call_list_log
(
  qmi_voice_call_list_public_type *info_ptr
)
{
  char label[ 100 ];
 
  /*-----------------------------------------------------------------------*/

  ASSERT( info_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  (void) QMI_VOICE_CM_IF_STR_PRINTF( label, sizeof( label ), "Call Type=%d call id %d: %s -> %s, privacy=%d",
                   info_ptr->call_type , info_ptr->call_id, qmi_voice_call_list_lookup_state_name( info_ptr->prev_state ), 
                   qmi_voice_call_list_lookup_state_name( info_ptr->state ), info_ptr->privacy );

    QM_MSG_HIGH_2( "Change in call state %s with as_id = %d", label, info_ptr->asubs_id); 
} /* qmi_voice_call_list_log */

/*===========================================================================
  FUNCTION qmi_voice_call_list_fill_all_call_info()

  DESCRIPTION
    Fill the all call info data.

  PARAMETERS
    info                        : Ptr to qmi_voice_cm_if_all_call_info_s 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_voice_call_list_fill_all_call_info
(
  qmi_voice_cm_if_all_call_info_s *info,
  uint8                            as_id
)
{
  qmi_voice_call_list_call_info_list_type call_info_list;
  qmi_voice_call_list_public_type *call_data;
  uint32 i;

  /*-----------------------------------------------------------------------*/
  ASSERT(info != NULL);
  /*-----------------------------------------------------------------------*/
  QM_MSG_MED_1("qmi_voice_call_list_fill_all_call_info, as_id = %d", as_id);


  call_data = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(*call_data));
  if(NULL == call_data)
  {
    qmi_voice_mem_error();
    info->num_of_calls = 0;
    return;
  }

  memset(info, 0, sizeof(qmi_voice_cm_if_all_call_info_s));
  
  /*Update the MPTY bit based on Call states*/
  qmi_voice_call_list_update_mpty_bit(as_id);

  qmi_voice_call_list_query_call_info_list( &call_info_list, as_id );

  info->num_of_calls = (uint8)call_info_list.num_of_calls;

  for (i=0; i<call_info_list.num_of_calls; i++)
  {
    memset(call_data, 0, sizeof(qmi_voice_call_list_public_type));
    if( qmi_voice_call_list_query_by_call_id(call_info_list.call_id[i], call_data) != E_SUCCESS)
    {
      continue;
    }
    /* Fill the conn index which is the call id for control point */
    info->call_info[i].call_id = call_data->conn_index;
    info->call_info[i].call_state = qmi_voice_cm_util_map_call_state(call_data->state);
    info->call_info[i].call_type = qmi_voice_cm_util_map_cm_to_qmi_call_type(call_data->call_type, call_data->mode_info_type);
    if((info->call_info[i].call_type == QMI_VOICE_CM_IF_CALL_TYPE_EMERGENCY_IP) && 
        (call_data->em_type == CM_CALL_EM_VT))
    {
      QM_MSG_HIGH_2("call_type = %d, em_type = %d", info->call_info[i].call_type, call_data->em_type);
      info->call_info[i].call_type = QMI_VOICE_CM_IF_CALL_TYPE_EMERGENCY_VT;
    }
      
    
    info->call_info[i].direction = (call_data->direction == CM_CALL_DIRECTION_MO) ? \
                                        QMI_VOICE_CM_IF_MO_CALL : QMI_VOICE_CM_IF_MT_CALL;

    info->call_info[i].mode = qmi_voice_cm_util_map_sys_mode_to_call_mode(call_data->call_mode);

    /* Report Mode Unknown when in CC_IN_PROG state */
    if(info->call_info[i].call_state == QMI_VOICE_CM_IF_CALL_STATE_CC_IN_PROG)
    {
      info->call_info[i].mode = QMI_VOICE_CM_IF_MODE_UNKNOWN;
    }

    /* VoIP changes FR 1002 */
    if((call_data->call_mode == SYS_SYS_MODE_LTE) && (call_data->mode_info_type == CM_CALL_MODE_INFO_CDMA))
    {
      info->call_info[i].mode = QMI_VOICE_CM_IF_MODE_CDMA;  
    }

    #ifdef FEATURE_TDSCDMA
    if(call_data->call_mode == SYS_SYS_MODE_TDS)
    {
      info->call_info[i].mode = QMI_VOICE_CM_IF_MODE_TDS;
    }
    #endif /*FEATURE_TDSCDMA*/
    if ( ( ( call_data->state == QMI_VOICE_CALL_LIST_STATE_ACTIVE ) ||
           ( call_data->state == QMI_VOICE_CALL_LIST_STATE_HOLDING ) ||
           ( call_data->state == QMI_VOICE_CALL_LIST_STATE_ALERTING) ||
           ( call_data->state == QMI_VOICE_CALL_LIST_STATE_DIALING) ||
           ( call_data->state == QMI_VOICE_CALL_LIST_STATE_INCOMING) ) && 
         ( ( call_data->call_type == CM_CALL_TYPE_VOICE ) ||
           ( call_data->call_type == CM_CALL_TYPE_EMERGENCY ) ||
           ( call_data->call_type == CM_CALL_TYPE_VT ))  
           )
    {
      info->call_info[i].is_mpty = call_data->is_multi_party ;
    }

    if( (call_data->line > CM_ALS_LINE_NONE) && (call_data->line < CM_ALS_LINE_MAX) )
    {
      info->call_info[i].als = (uint8)call_data->line ; 
    }

    QM_MSG_MED_7("Call Info:  Conn id %d, Call State %d, call type %d, dir %d, mode %d, is_mpty %d, line %d", info->call_info[i].call_id, info->call_info[i].call_state, info->call_info[i].call_type,
		info->call_info[i].direction, info->call_info[i].mode, info->call_info[i].is_mpty, info->call_info[i].als);


    if( (call_data->num.len > 0) || (call_data->num.pi != 0) )    
    {
      info->call_info[i].is_num_available = TRUE;
      info->call_info[i].num.len = call_data->num.len;
      info->call_info[i].num.pi = call_data->num.pi;
      info->call_info[i].num.si = call_data->num.si;
      info->call_info[i].num.num_type= call_data->num.number_type;
      info->call_info[i].num.num_plan= call_data->num.number_plan;
      if( (call_data->num.buf[0] != '+') && (call_data->num.number_type == (uint8)CM_NUM_TYPE_INTERNATIONAL) && (call_data->num.len > 0))
      {
        info->call_info[i].num.buf[0] = '+';
        if((call_data->num.buf[0] == '0') && (call_data->num.buf[1] == '0'))
        {
          // Case when the first two digits are 00 (e.g. 0091)
          // Remove the 00 and add a '+' in the beginning
          info->call_info[i].num.len = info->call_info[i].num.len - 1;
          memscpy(&info->call_info[i].num.buf[1], sizeof(info->call_info[i].num.buf) - 1,
                  &call_data->num.buf[2], call_data->num.len-2);
        }
        else
        {
          info->call_info[i].num.len = info->call_info[i].num.len + 1;
          memscpy(&info->call_info[i].num.buf[1], sizeof(info->call_info[i].num.buf) - 1,
                  call_data->num.buf, call_data->num.len);
        }        
       QM_MSG_HIGH_2("Call Info: International num Len %d, number %s",info->call_info[i].num.len,info->call_info[i].num.buf);       
      }
      else
      {
        memscpy(info->call_info[i].num.buf, sizeof(info->call_info[i].num.buf),
                call_data->num.buf, call_data->num.len);
      }
    }
    if( call_data->is_cnap_info_valid )
    {
      info->call_info[i].is_cnap_info_available = TRUE;
      memscpy(&info->call_info[i].cnap_info, sizeof(info->call_info[i].cnap_info),
              &call_data->cnap_info, sizeof(qmi_voice_cm_if_cnap_info_s_type));
    }
    if( call_data->is_alerting_type_valid )
    {
      info->call_info[i].is_alerting_type_available = TRUE;
      info->call_info[i].alerting_type = call_data->alerting_type ;
    }
    if( call_data->uus_data.mt_user_data.present )
    {
      info->call_info[i].is_uus_info_available = TRUE;
      info->call_info[i].uus_info.uus_type = (uint8) call_data->uus_data.mt_user_data.user_user_type;
      info->call_info[i].uus_info.uus_dcs = (uint8) call_data->uus_data.mt_user_data.user_user_protocol_disc ;
      info->call_info[i].uus_info.uus_data_len = (uint8) call_data->uus_data.mt_user_data.user_user_data_length;
      memscpy(info->call_info[i].uus_info.uus_data, sizeof(info->call_info[i].uus_info.uus_data),
              call_data->uus_data.mt_user_data.user_user_data, info->call_info[i].uus_info.uus_data_len);
    }
    if( call_data->is_srv_opt_valid )
    {
      info->call_info[i].is_srv_opt_available = TRUE;
      info->call_info[i].srv_opt = call_data->srv_opt;
    }
    if( call_data->is_privacy_valid )
    {
      info->call_info[i].is_voice_privacy_available = TRUE;
      info->call_info[i].voice_privacy = call_data->privacy;
    }
    if( call_data->is_otasp_status_valid )
    {
      info->call_info[i].is_otasp_status_available = TRUE;
      info->call_info[i].otasp_status = (uint8)call_data->otasp_status ;
    }
    if( (call_data->state == QMI_VOICE_CALL_LIST_STATE_END) || (call_data->state == QMI_VOICE_CALL_LIST_STATE_DISCONNECTING) )
    {
      info->call_info[i].is_end_reason_available = TRUE;
      info->call_info[i].end_reason = qmi_voice_cm_if_get_call_fail_cause();
      
      info->call_info[i].raw_call_end_cause_code_valid   = 
        qmi_voice_cm_if_get_raw_call_end_cause_code (&info->call_info[i].raw_call_end_cause_code);
    }
    if(call_data->is_alpha_valid)
    {
      //Since alpha is null terminated as received from CM, an alpha_info.len of 1 would indicate a simple null in alpha_info.buf.
      info->call_info[i].is_alpha_available = TRUE;
      qmi_voice_cm_util_copy_alpha_info_from_cm(&(info->call_info[i].alpha_info),&(call_data->alpha_info));
      QM_MSG_HIGH_2("Recevied ALPHA DCS from CM = %d, Voice = %d", call_data->alpha_info.dcs, info->call_info[i].alpha_info.alpha_dcs);
    }
    if( call_data->is_conn_num_valid )
    {
      info->call_info[i].is_conn_num_available = TRUE;
      info->call_info[i].is_connected_number_ECT_valid= TRUE;
      memscpy(&info->call_info[i].conn_num_data, sizeof(info->call_info[i].conn_num_data),
              &call_data->conn_num_info, sizeof(call_data->conn_num_info));
      info->call_info[i].is_connected_number_ECT = call_data->is_connected_number_ECT;
    }
    if( (call_data->is_diagnostics_info_valid == TRUE) && (call_data->state == QMI_VOICE_CALL_LIST_STATE_END) )
    {
      info->call_info[i].is_diagnostics_info_available = TRUE;
      info->call_info[i].diagnostic_data.diagnostic_length = call_data->diagnostic_info.diagnostic_length;
      memscpy(info->call_info[i].diagnostic_data.diagnostics, sizeof(info->call_info[i].diagnostic_data.diagnostics),
              call_data->diagnostic_info.diagnostics, call_data->diagnostic_info.diagnostic_length);
    }    
    if( call_data->is_as_id_valid == TRUE)
    {
      info->call_info[i].is_as_id_available = TRUE;
      info->call_info[i].as_id = (uint8)call_data->asubs_id;
      QM_MSG_MED_3("Call Info:  Conn id %d, is_as_id_availble=%d, as_id=%d", info->call_info[i].call_id, info->call_info[i].is_as_id_available, info->call_info[i].as_id);
    }
    if( call_data->is_called_party_num_valid )
    {
      info->call_info[i].is_called_party_num_available = TRUE;
      memscpy(&info->call_info[i].called_party_num_data, sizeof(info->call_info[i].called_party_num_data),
              &call_data->called_party_num_info,sizeof(call_data->called_party_num_info));
    }
    if( call_data->is_redirecting_party_num_valid )
    {
      info->call_info[i].is_redirecting_party_num_available = TRUE;
      memscpy(&info->call_info[i].redirecting_party_num_data, sizeof(info->call_info[i].redirecting_party_num_data),
              &call_data->redirecting_party_num_info,sizeof(call_data->redirecting_party_num_info));
    }
    if( call_data->is_alert_pattern_valid )
    {
      info->call_info[i].is_alerting_pattern_available = TRUE;
      info->call_info[i].alerting_pattern = call_data->alert_pattern ;
    }

    if(info->call_info[i].call_type == QMI_VOICE_CM_IF_CALL_TYPE_VT)
    {    
      info->call_info[i].is_audio_attrib_available = TRUE;
      info->call_info[i].audio_attrib = call_data->audio_attrib;
   
      info->call_info[i].is_video_attrib_available = TRUE;
      info->call_info[i].video_attrib = call_data->video_attrib;

      info->call_info[i].is_call_attrib_status_available = TRUE;
      info->call_info[i].call_attrib_status = call_data->attrib_status;
      QM_MSG_MED_6("is_a_attrib_av=%d, a_attrib=%d,is_v_attrib_av=%d, v_attrib=%d, is_call_attrib_status_av=%d, call_attrib_status=%d",
                    info->call_info[i].is_audio_attrib_available, info->call_info[i].audio_attrib,
                    info->call_info[i].is_video_attrib_available, info->call_info[i].video_attrib,
                    info->call_info[i].is_call_attrib_status_available,
                    info->call_info[i].call_attrib_status);
    }

    if(call_data->call_type == CM_CALL_TYPE_VS && call_data->is_vs_call_variant_valid)
    {
      info->call_info[i].is_vs_call_variant_valid = TRUE;
      info->call_info[i].vs_call_variant = call_data->vs_call_variant;
      QM_MSG_MED_2("Call Info:  Conn id %d, vs_call_variant=%d", info->call_info[i].call_id, info->call_info[i].vs_call_variant);
    }
    info->call_info[i].is_srvcc_call_available= call_data->is_srvcc_call_available;
    info->call_info[i].is_srvcc_call = call_data->is_srvcc_call;
    QM_MSG_MED_1("Call Info: Is SRVCC Call = %d", info->call_info[i].is_srvcc_call);

    if(call_data->is_parent_call_id_valid)
    {
      info->call_info[i].is_parent_id_available = TRUE;
      info->call_info[i].parent_call_id = call_data->parent_call_id;

      info->call_info[i].is_parent_call_id_cleared = (uint8) call_data->is_parent_call_id_cleared;
      QM_MSG_MED_2("Call Info: Parent Call = %d, Cleared = %d", call_data->parent_call_id, call_data->is_parent_call_id_cleared);
    }//else {info->call_info[i].is_parent_id_available = FALSE;}

    info->call_info[i].is_drvcc_call_available= call_data->is_drvcc_call_available;
    info->call_info[i].is_drvcc_call = call_data->is_drvcc_call;
    QM_MSG_MED_1("Call Info: Is DRVCC Call = %d", info->call_info[i].is_drvcc_call);

    if(call_data->is_drvcc_parent_call_id_valid)
    {
      info->call_info[i].is_drvcc_parent_id_available = TRUE;
      info->call_info[i].drvcc_parent_call_id = call_data->drvcc_parent_call_id;

      info->call_info[i].is_drvcc_parent_call_id_cleared = (uint8) call_data->is_drvcc_parent_call_id_cleared;
      QM_MSG_MED_2("Call Info: DRVCC Parent Call = %d, Cleared = %d", call_data->drvcc_parent_call_id, call_data->is_drvcc_parent_call_id_cleared);
    }//else {info->call_info[i].is_drvcc_parent_id_available = FALSE;}

    /* info to be passed in active sate of IP calls*/
    if((call_data->state == QMI_VOICE_CALL_LIST_STATE_ACTIVE) && 
       QMI_VOICE_CM_IF_VOIP_CALL(info->call_info[i].call_type))
    {
      info->call_info[i].is_peer_cap_available = TRUE;
      info->call_info[i].peer_cap = call_data->peer_cap;

      info->call_info[i].is_local_cap_available = TRUE;
      info->call_info[i].local_cap = call_data->local_cap;

      QM_MSG_MED_9("Call Info: call_id: %d, peer: audio:%d, video:%d, a_cause:%d, v_cause:%d, local: audio:%d, video:%d, a_cause:%d, v_cause:%d",
                   info->call_info[i].call_id,
                   info->call_info[i].peer_cap.audio_attrib,
                   info->call_info[i].peer_cap.video_attrib,
                   info->call_info[i].peer_cap.audio_restrict_cause,
                   info->call_info[i].peer_cap.video_restrict_cause,
                   info->call_info[i].local_cap.audio_attrib,
                   info->call_info[i].local_cap.video_attrib,
                   info->call_info[i].local_cap.audio_restrict_cause,
                   info->call_info[i].local_cap.video_restrict_cause);
    }

    if(call_data->is_child_num_available && (call_data->state == QMI_VOICE_CALL_LIST_STATE_INCOMING))
    {
      info->call_info[i].is_child_num_available = TRUE;
      info->call_info[i].child_num = call_data->child_num;
      QM_MSG_HIGH_1("Call Info: Child num Len %d", info->call_info[i].child_num.len);
    }

    if(call_data->is_display_text_available && ((call_data->state == QMI_VOICE_CALL_LIST_STATE_INCOMING) || (call_data->state == QMI_VOICE_CALL_LIST_STATE_WAITING)))
    {
      info->call_info[i].is_display_text_available = TRUE;
      info->call_info[i].display_text = call_data->display_text;
      QM_MSG_HIGH_1("Call Info: Display Text Len %d", info->call_info[i].display_text.text_len);
    }

    if((call_data->is_adnl_call_info_available==TRUE) && (call_data->state == QMI_VOICE_CALL_LIST_STATE_INCOMING))
    {
      info->call_info[i].is_additional_call_info_available = TRUE;
      info->call_info[i].num_indications = call_data->num_indications;      
    }

    if (call_data->is_end_reason_text_available == TRUE)
    {
      info->call_info[i].is_end_reason_text_available = TRUE;
      info->call_info[i].end_reason_text = call_data->end_reason_text;
      QM_MSG_HIGH_1("Call Info: End Reason Text Len %d", info->call_info[i].end_reason_text.end_reason_text_length);
    }

    if( ((call_data->direction == CM_CALL_DIRECTION_MO) && ((call_data->state == QMI_VOICE_CALL_LIST_STATE_CC_IN_PROG) || (call_data->state == QMI_VOICE_CALL_LIST_STATE_DIALING))) ||
        ((call_data->direction == CM_CALL_DIRECTION_MT) && ((call_data->state == QMI_VOICE_CALL_LIST_STATE_INCOMING) || (call_data->state == QMI_VOICE_CALL_LIST_STATE_WAITING))) )
    {
      info->call_info[i].is_media_id_available= TRUE;
      info->call_info[i].media_id= call_data->call_id;
      QM_MSG_MED_3("Call Info:  Conn id %d, is_media_id_available=%d, media_id=%d", info->call_info[i].call_id, info->call_info[i].is_media_id_available, info->call_info[i].media_id);
    }

    if(call_data->is_second_alpha_valid)
    {
      /* Since second alpha is null terminated as received from CM, an second_alpha_info.len of 1 would indicate a simple null in second_alpha_info.buf. */
      info->call_info[i].is_second_alpha_available = TRUE;
      qmi_voice_cm_util_copy_alpha_info_from_cm(&(info->call_info[i].second_alpha_info),&(call_data->second_alpha_info));
      QM_MSG_HIGH_2("Recevied Second ALPHA DCS from CM = %d, Voice = %d", call_data->second_alpha_info.dcs, info->call_info[i].second_alpha_info.alpha_dcs);
    }

    //IP caller name update can be expected only when call is in incoming state
    if((call_data->is_ip_caller_name_available && ((call_data->state == QMI_VOICE_CALL_LIST_STATE_INCOMING) || (call_data->state == QMI_VOICE_CALL_LIST_STATE_WAITING))) ||
        call_data->is_srvcc_caller_name_available )
    {
      info->call_info[i].is_ip_caller_name_available = TRUE;
      if(call_data->is_srvcc_caller_name_available)
        {
          info->call_info[i].is_srvcc_caller_name_available = TRUE;
          info->call_info[i].name_pi       = call_data->name_pi;
        }
      info->call_info[i].ip_caller_name = call_data->ip_caller_name;
      QM_MSG_HIGH_1("Call Info: Ip Caller Name Len %d", info->call_info[i].ip_caller_name.name_len);
    }

    if(call_data->is_orig_fail_reason_available)
    {
      info->call_info[i].is_orig_fail_reason_available   = TRUE;
      info->call_info[i].orig_fail_reason                = (uint16)call_data->orig_fail_reason;
      QM_MSG_HIGH_1("Call Info: Orig Fail Reason %d", info->call_info[i].orig_fail_reason);
    }

    if(call_data->is_secure_call_available)
    {
      info->call_info[i].is_secure_call_available   = TRUE;
      info->call_info[i].is_secure_call             = (uint16)call_data->is_secure_call;
      QM_MSG_HIGH_1("Call Info: Is Secure Call %d", info->call_info[i].is_secure_call);
    }

    if(call_data->is_called_party_ringing_status_avail)
    {
      info->call_info[i].is_called_party_ringing_status_avail   = TRUE;
      info->call_info[i].is_called_party_ringing             = (uint16)call_data->is_called_party_ringing;
      QM_MSG_HIGH_2("Call Info:[%d] Is Called party ringing %d", i, info->call_info[i].is_called_party_ringing);

    }
    else
    {
      info->call_info[i].is_called_party_ringing_status_avail   = FALSE;
      info->call_info[i].is_called_party_ringing                = FALSE;
      QM_MSG_HIGH_2("Call Info:[%d] Is Called party ringing %d", i, info->call_info[i].is_called_party_ringing);
    }

	
  } /* for num_of_calls*/

  QMI_VOICE_CM_IF_MEM_FREE(call_data);

}/* qmi_voice_call_list_fill_all_call_info() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_report_all_call_status_changed

===========================================================================*/
/*!
    @brief
    Report the call status change for voice or CS data calls.
 
    @return
    none
*/
/*=========================================================================*/
void qmi_voice_call_list_report_all_call_status_changed
(
  uint8 as_id
)
{
  qmi_voice_cm_if_all_call_status_ind_s  *call_ind;

  call_ind = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(*call_ind));

  if(call_ind == NULL)
  {
    qmi_voice_mem_error();
    return;
  }

  /* memset done in fill all call info */
  /* Fill the calls data */
  qmi_voice_call_list_fill_all_call_info(&call_ind->info, as_id);

  QM_MSG_MED("All Call status indication will be sent");

  qmi_voice_cm_if_unsol_response( QMI_VOICE_CM_IF_CMD_ALL_CALL_STATUS_IND, (void*)call_ind, sizeof(qmi_voice_cm_if_all_call_status_ind_s), as_id);

  QMI_VOICE_CM_IF_MEM_FREE(call_ind);

} /* qmi_voice_call_list_report_all_call_status_changed() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_report_voice_call_state_changed

===========================================================================*/
/*!
    @brief
    Report the call status change for voice or CS data calls, if needed
 
    @return
    None
*/
/*=========================================================================*/
void qmi_voice_call_list_report_voice_call_state_changed
(
  cm_call_type_e_type call_type,
  uint8               as_id
)
{ 
  /*-----------------------------------------------------------------------*/
  if ( QMI_VOICE_CALL_LIST_REPORT_CALL_STATE_CHANGED( call_type ) )
  {
    qmi_voice_call_list_report_all_call_status_changed(as_id);
  }

} /* qmi_voice_call_list_report_voice_call_state_changed */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_init

===========================================================================*/
/*!
    @brief
    Initialize call list.
 
    @return
    None
*/
/*=========================================================================*/
void qmi_voice_call_list_init
(
  void
)
{ 
  /*-----------------------------------------------------------------------*/

  //In qmi_voice_call_list_init()

  call_list.head_ptr = NULL;
  call_list.tail_ptr = NULL;
  call_list.num_of_calls = 0;

} /* qmi_voice_call_list_init */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_new

===========================================================================*/
/*!
    @brief
    Add an entry to the list of active calls.
 
    @return
    see qmi_voice_call_list_new_all
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_new
( 
  cm_call_id_type          call_id,
  cm_call_type_e_type      call_type,      
  sys_sys_mode_e_type      call_mode,
  cm_call_direction_e_type direction,
  qmi_voice_call_list_state_e_type      call_state,          
  cm_als_line_e_type        line,           
  cm_num_s_type             num,           
  cm_alpha_s_type           alpha,
  cm_call_mode_info_e_type  mode_info_type,
  boolean                   report_state_change,
  uint8                     as_id,
  cm_ip_call_attrib_info_s_type     call_attrib
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_state_e_type state = QMI_VOICE_CALL_LIST_STATE_IDLE;
  qmi_voice_call_list_public_type *info;
  boolean  add_call_list_entry = TRUE;
  uint8 asubs_id = (uint8)QMI_VOICE_CM_IF_AS_ID_PRIMARY;

#ifdef FEATURE_DUAL_SIM
    asubs_id = as_id;
#endif

  /*-----------------------------------------------------------------------*/
  QM_MSG_MED_2("Call with call_id(%d) and mode_info(%d) will be added/replaced", call_id, mode_info_type);

  info = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(*info));
  if(NULL == info)
  {
	  qmi_voice_mem_fatal();
    return E_FAILURE;
  }  

  memset(info, 0, sizeof(*info));

  /* MO, add an item to call list */
  if( (call_state == QMI_VOICE_CALL_LIST_STATE_CC_IN_PROG) ||
      (call_state == QMI_VOICE_CALL_LIST_STATE_ACTIVE) || 
      (call_state == QMI_VOICE_CALL_LIST_STATE_SETUP) || 
      (call_state == QMI_VOICE_CALL_LIST_STATE_HOLDING) ||
      (call_state == QMI_VOICE_CALL_LIST_STATE_ALERTING))
  {
    state = call_state;
  }
  else if ( call_state == QMI_VOICE_CALL_LIST_STATE_DIALING )
  {
    /* Check if the call object is present already */
    if( qmi_voice_call_list_query(call_id, info) != E_SUCCESS )
    { 
      /* Call not present. Add it here. Applicable for CS calls originated by other CM clients */
      state = QMI_VOICE_CALL_LIST_STATE_DIALING;
    }
    else
    {
      add_call_list_entry = FALSE;
      if(call_type == CM_CALL_TYPE_EMERGENCY && mode_info_type == CM_CALL_MODE_INFO_IP)
      {
        qmi_voice_call_list_buf_type *buf_ptr;

        if ((buf_ptr = qmi_voice_call_list_find( call_id )) != NULL)
        {
          buf_ptr->pub.em_type = call_attrib.em_type;
          QM_MSG_HIGH_1("em_type = %d", buf_ptr->pub.em_type);

        }
      }
      /* Call already present. Replace the call object here */
      status = qmi_voice_call_list_replace(call_id, info->conn_index, call_type, call_mode, direction, CM_CALL_STATE_ORIG, line, num, mode_info_type,FALSE);
      /* State change will anyway be reported as a part of replace. */
      if(qmi_voice_call_list_update_alpha_info(call_id, alpha) != E_SUCCESS)
      {
        QM_MSG_ERROR_1("Failed to update alpha_info for call list entry: call id = %d", call_id);
      }
    }
  }
  else if ( call_state == QMI_VOICE_CALL_LIST_STATE_INCOMING )
  {
    state = call_state;

    /* If in MPTY, state should be WAITING */
    if ( ( ( call_type == CM_CALL_TYPE_VOICE ) ||
            ( call_type == CM_CALL_TYPE_EMERGENCY ) ||
            ( call_type == CM_CALL_TYPE_VT ) ) && 
            ( qmi_voice_call_list_count_num_of_voice_calls_by_state_and_mode( (uint16)(QMI_VOICE_CALL_LIST_STATE_ACTIVE | QMI_VOICE_CALL_LIST_STATE_HOLDING), mode_info_type, asubs_id) > 0 )  
       )
    {
      state = QMI_VOICE_CALL_LIST_STATE_WAITING;
    }
  }
  else
  {
    /* Invalid state */
    QM_MSG_ERROR_3( "Fail to add call list entry : call id %d (invalid call state %s (%d))", 
                     call_id, qmi_voice_call_list_lookup_state_name( call_state ), call_state );
    status = E_NOT_ALLOWED;
    add_call_list_entry = FALSE;
  }

  if(add_call_list_entry)
  {
    status = qmi_voice_call_list_new_all( call_id, call_type, call_mode, direction, state, line, num,  
                                          alpha, mode_info_type, info, asubs_id, call_attrib
                                         );
  }

  /* Added a new entry to call list */
  if (status == E_SUCCESS)
  {
    /* Log call list changes */
    qmi_voice_call_list_log( info );

    if(report_state_change)
    {
      /* Send unsolicited call state change if needed */
      if ( QMI_VOICE_CALL_LIST_REPORT_CALL_STATE_CHANGED( info->call_type ) )
      {
        qmi_voice_call_list_report_all_call_status_changed(asubs_id);
      }
    }
  }

  if(NULL != info)
  {
    QM_MSG_MED("Freeing memory for info in call_list_new");
    QMI_VOICE_CM_IF_MEM_FREE(info);
  }

  return status;

} /* qmi_voice_call_list_new() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_free_all

===========================================================================*/
/*!
    @brief
    Free all entries in the list of active calls.
 
    @return
    None
*/
/*=========================================================================*/
void qmi_voice_call_list_free_all
( 
  void
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  qmi_voice_call_list_buf_type *next_buf_ptr;

  /*-----------------------------------------------------------------------*/

  QM_MSG_LOW("qmi_voice_call_list_free_all()");

  buf_ptr = call_list.head_ptr;

  while ( buf_ptr != NULL )
  {
    next_buf_ptr = buf_ptr->next_ptr;
    buf_ptr->next_ptr = NULL;
    buf_ptr->prev_ptr = NULL;
    /* Free the connection index */
    (void) qmi_voice_cm_util_deallocate_conn_id(buf_ptr->pub.conn_index);
    /* Free the call list buf here */
    QMI_VOICE_CM_IF_MEM_FREE(buf_ptr);
    buf_ptr = next_buf_ptr;
  }

  call_list.head_ptr = NULL;
  call_list.tail_ptr = NULL;
  call_list.num_of_calls = 0;

} /* qmi_voice_call_list_free_all() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_free

===========================================================================*/
/*!
    @brief
    Frees an entry from the list of active calls.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_free
( 
  cm_call_id_type call_id,
  boolean         report_state_changed
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr, *wait_buf_ptr,*wait_buf_ptr_no_active;
  qmi_voice_call_list_buf_type *prev_buf_ptr, *next_buf_ptr;
  uint32 current_num_of_active_voice_calls;
  cm_call_type_e_type           end_call_type;
  uint8 as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  /* Entry was found */
  if ( buf_ptr != NULL )
  {
    if(buf_ptr->pub.is_as_id_valid)
    {
      as_id = buf_ptr->pub.asubs_id;
    }
    current_num_of_active_voice_calls = qmi_voice_call_list_count_num_of_voice_calls_by_state( (uint16) QMI_VOICE_CALL_LIST_STATE_ACTIVE | QMI_VOICE_CALL_LIST_STATE_HOLDING , as_id );

    buf_ptr->pub.prev_state = buf_ptr->pub.state;
    buf_ptr->pub.state = QMI_VOICE_CALL_LIST_STATE_END;

    /* Log call list changes */
    qmi_voice_call_list_log( &buf_ptr->pub );

    /* Remember the end call type */
    end_call_type = buf_ptr->pub.call_type;

    /* Check whether any incoming voice call need to change state. This should only happen to GW calls */
    QM_MSG_MED_1("current_num_of_active_voice_calls :  %d", current_num_of_active_voice_calls);
    if ( ( current_num_of_active_voice_calls > 0 ) &&
         ( qmi_voice_call_list_count_num_of_voice_calls_by_state((uint16) QMI_VOICE_CALL_LIST_STATE_ACTIVE | QMI_VOICE_CALL_LIST_STATE_HOLDING, as_id ) == 0 ) )
    {
      wait_buf_ptr = call_list.head_ptr;
      while ( wait_buf_ptr != NULL )
      {
        if ( wait_buf_ptr->pub.state == QMI_VOICE_CALL_LIST_STATE_WAITING ) 
        {
          QM_MSG_MED_1("Changing from Waiting->Incoming call :  connex id %d", wait_buf_ptr->pub.conn_index);
          wait_buf_ptr->pub.prev_state = wait_buf_ptr->pub.state; 
          wait_buf_ptr->pub.state = QMI_VOICE_CALL_LIST_STATE_INCOMING;

          /* Log call list changes */
          qmi_voice_call_list_log( &wait_buf_ptr->pub );
          }
        wait_buf_ptr = wait_buf_ptr->next_ptr;
      }
    }
    else if(qmi_voice_call_list_count_num_of_voice_calls_by_state((uint16) QMI_VOICE_CALL_LIST_STATE_ACTIVE | QMI_VOICE_CALL_LIST_STATE_HOLDING, as_id ) == 0 &&
         qmi_voice_call_list_count_num_of_voice_calls_by_state((uint16) QMI_VOICE_CALL_LIST_STATE_WAITING, as_id ) > 0 )
    {
      wait_buf_ptr_no_active = call_list.head_ptr;
      while ( wait_buf_ptr_no_active != NULL )
      {
        if ( wait_buf_ptr_no_active->pub.state == QMI_VOICE_CALL_LIST_STATE_WAITING ) 
        {
          QM_MSG_MED_1("Changing from Waiting->Incoming call when no active or held calls:  connex id %d", wait_buf_ptr_no_active->pub.conn_index);
          wait_buf_ptr_no_active->pub.prev_state = wait_buf_ptr_no_active->pub.state; 
          wait_buf_ptr_no_active->pub.state = QMI_VOICE_CALL_LIST_STATE_INCOMING;

          /* Log call list changes */
          qmi_voice_call_list_log( &wait_buf_ptr_no_active->pub );
        }
        wait_buf_ptr_no_active = wait_buf_ptr_no_active->next_ptr;
      }      
    }

    /* When there are no more than one active call, clearing the multiparty flag which was set in mng_call_conf */  
    if(qmi_voice_call_list_count_num_of_voice_calls_by_state( (uint16)QMI_VOICE_CALL_LIST_STATE_ACTIVE, as_id ) <= 1 )
    {
      QM_MSG_MED_1("Resetting is_mpty_recieved_from_mng_calls_conf :  %d", is_mpty_recieved_from_mng_calls_conf);
      is_mpty_recieved_from_mng_calls_conf = FALSE;
    }
    
    /* Send unsolicited call state changed if needed */
    if ( report_state_changed )
    {
      qmi_voice_call_list_report_voice_call_state_changed( end_call_type, as_id );
    }

    /* Free the connection index */
    (void) qmi_voice_cm_util_deallocate_conn_id(buf_ptr->pub.conn_index);

    /* Call status indication sent. Now remove the entry */
    QM_MSG_MED_1("Delete call list entry :  call id %d", call_id);

    /* Detach the end item from the call list */
    prev_buf_ptr = buf_ptr->prev_ptr;
    next_buf_ptr = buf_ptr->next_ptr;

    /* The only item in the call list is being removed */
    if ( ( prev_buf_ptr == NULL ) && ( next_buf_ptr == NULL ) )
    {
      call_list.head_ptr = NULL;
      call_list.tail_ptr = NULL;
    }
    /* First item in the call list is being removed */
    else if ( prev_buf_ptr == NULL )
    {
      call_list.head_ptr = next_buf_ptr;
      next_buf_ptr->prev_ptr = NULL;
    }   
    /* Last item in the call list is being removed*/
    else if ( next_buf_ptr == NULL )
    {
      call_list.tail_ptr = prev_buf_ptr;
      prev_buf_ptr->next_ptr = NULL;
    }
    /* Middle item in the call list is being removed */
    else
    {
      prev_buf_ptr->next_ptr = buf_ptr->next_ptr;
      next_buf_ptr->prev_ptr = buf_ptr->prev_ptr;
    }

    /* Free the call list buf here */
    QMI_VOICE_CM_IF_MEM_FREE(buf_ptr);
    call_list.num_of_calls--;
  }
  /* Entry was not found */
  else
  {
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_free() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_alert

===========================================================================*/
/*!
    @brief
    Change the state of the entry in the list with given Call ID to 
    QMI_VOICE_CALL_LIST_STATE_ALERTING.  
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_alert
( 
  cm_call_id_type call_id,
  uint8 call_progress
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  uint8 as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( ( buf_ptr != NULL ) && ( buf_ptr->pub.direction == CM_CALL_DIRECTION_MO ) &&
       ((buf_ptr->pub.state == QMI_VOICE_CALL_LIST_STATE_DIALING) || 
        (call_progress == QMI_VOICE_CM_IF_REMOTE_ALERTING)  ||
        (call_progress == QMI_VOICE_CM_IF_LOCAL_ALERTING)))
  {
    buf_ptr->pub.prev_state = buf_ptr->pub.state;
    buf_ptr->pub.state = QMI_VOICE_CALL_LIST_STATE_ALERTING;

    /* Log call list changes */
    qmi_voice_call_list_log( &buf_ptr->pub );

    if(buf_ptr->pub.is_as_id_valid)
    {
      as_id = buf_ptr->pub.asubs_id;
    }
    /* Send unsolicited call state change if needed */
    qmi_voice_call_list_report_all_call_status_changed(as_id);
  }
  else
  {
    /* Call not in the list or not an outgoing call */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_alert() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_answer

===========================================================================*/
/*!
    @brief
    Record the incoming call as answered with given Call ID. 
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_answer
( 
  cm_call_id_type call_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {   
    if ( buf_ptr->pub.direction == CM_CALL_DIRECTION_MT )
    {
      buf_ptr->pub.prev_state = buf_ptr->pub.state;
      buf_ptr->pub.answered = TRUE;

      /* Log CList changes */
      qmi_voice_call_list_log( &buf_ptr->pub );
    }
    else
    {
      /*Call id found but not for MT Call. Hence failure.*/
      status = E_FAILURE;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_answer() */



/*===========================================================================

  FUNCTION:  qmi_voice_call_list_connect

===========================================================================*/
/*!
    @brief
    Change the state of the entry in the list with given Call ID to 
    QMI_VOICE_CALL_LIST_STATE_ACTIVE.  
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_connect
( 
  cm_call_id_type call_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  uint32 current_num_of_active_voice_calls;
  boolean report_call_state_changed = FALSE;
  uint8 as_id = (uint8)QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  /*-----------------------------------------------------------------------*/
          

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {   
    if(buf_ptr->pub.is_as_id_valid)
    {
      as_id = buf_ptr->pub.asubs_id;
    }
    current_num_of_active_voice_calls = qmi_voice_call_list_count_num_of_voice_calls_by_state((uint16) QMI_VOICE_CALL_LIST_STATE_ACTIVE, as_id );

    buf_ptr->pub.prev_state = buf_ptr->pub.state;
    buf_ptr->pub.state = QMI_VOICE_CALL_LIST_STATE_ACTIVE;

    /* End any local ringback by sending local alerting */
    if(qmi_voice_call_list_is_remote_alerting(call_id))
    {
      if(qmi_voice_call_list_update_alerting_type(call_id, QMI_VOICE_CM_IF_LOCAL_ALERTING) != E_SUCCESS)
      {
        QM_MSG_ERROR_1("Failed to update the Ringback information of CList entry for call_id %d", call_id);
      }
    }

    /* Log call list changes */
    qmi_voice_call_list_log( &buf_ptr->pub );

    /* Check to see if unsolicited call state changed has to be sent after all update is completed */
    if ( QMI_VOICE_CALL_LIST_REPORT_CALL_STATE_CHANGED( buf_ptr->pub.call_type ) )
    {
      report_call_state_changed = TRUE;
    }

    /* Check whether any incoming voice call need to change state */
    if ( ( current_num_of_active_voice_calls == 0 ) &&
         ( qmi_voice_call_list_count_num_of_voice_calls_by_state_and_mode((uint16) QMI_VOICE_CALL_LIST_STATE_INCOMING, buf_ptr->pub.mode_info_type, as_id ) > 0 ) )
    {
      buf_ptr = call_list.head_ptr;
      while ( buf_ptr != NULL )
      {
        if ( buf_ptr->pub.state == QMI_VOICE_CALL_LIST_STATE_INCOMING ) 
        {
          buf_ptr->pub.prev_state = buf_ptr->pub.state; 
          buf_ptr->pub.state = QMI_VOICE_CALL_LIST_STATE_WAITING;

          /* Log call list changes */
          qmi_voice_call_list_log( &buf_ptr->pub );

          /* Check to see if unsolicited call state changed has to be sent after all update is completed */
          if ( !report_call_state_changed )
          {
            report_call_state_changed = TRUE;
          }
        }

        buf_ptr = buf_ptr->next_ptr;
      }
    }

    /* Send unsolicited call state changed if needed */
    if ( report_call_state_changed )
    {
      qmi_voice_call_list_report_all_call_status_changed(as_id);
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_connect() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_disconnecting

===========================================================================*/
/*!
    @brief
    Change the state of the entry in the list with given Call ID to 
    QMI_VOICE_CALL_LIST_STATE_DISCONNECTING.  
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_disconnecting
( 
  cm_call_id_type call_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  uint8 as_id = (uint8)QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    buf_ptr->pub.prev_state = buf_ptr->pub.state;
    buf_ptr->pub.state = QMI_VOICE_CALL_LIST_STATE_DISCONNECTING;

    /* Log call list changes */
    qmi_voice_call_list_log( &buf_ptr->pub );

    if(buf_ptr->pub.is_as_id_valid)
    {
      as_id = buf_ptr->pub.asubs_id;
    }

    /* Send unsolicited call state change */
    qmi_voice_call_list_report_all_call_status_changed(as_id);
  }
  else
  {
    /* Call not in the list or not an outgoing call */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_disconnecting() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_manage

===========================================================================*/
/*!
    @brief
    Updates the state of the entries in the list with the specified 
    active call IDs list information on a particular subscription
 
    @return
    None
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_manage
( 
  const active_calls_list_s_type *active_calls_list_ptr,
  uint8                                   as_id
)
{
  uint32 i;
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  boolean report_call_state_changed = FALSE;

  /*-----------------------------------------------------------------------*/

  ASSERT( active_calls_list_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  /* Change all call states for all active calls to STATE_HOLDING */
  buf_ptr = call_list.head_ptr;
  while ( buf_ptr != NULL )
  {
    if ( ( buf_ptr->pub.call_type == CM_CALL_TYPE_VOICE ) || 
         ( buf_ptr->pub.call_type == CM_CALL_TYPE_EMERGENCY ) )
    {             
        buf_ptr->pub.prev_state = buf_ptr->pub.state;
      if ( buf_ptr->pub.state == QMI_VOICE_CALL_LIST_STATE_ACTIVE && ((!buf_ptr->pub.is_as_id_valid) || (as_id == buf_ptr->pub.asubs_id)))
      {
        buf_ptr->pub.state = QMI_VOICE_CALL_LIST_STATE_HOLDING;
        QM_MSG_LOW_1("call id:  %d state:  Holding", buf_ptr->pub.call_id);
      }
    }

    buf_ptr = buf_ptr->next_ptr;
  }

  /* Update call states according to active_calls_list. */
  for ( i = 0; i < active_calls_list_ptr->size; i++ )
  {
    buf_ptr = qmi_voice_call_list_find( active_calls_list_ptr->table[ i ] );
    if ( buf_ptr == NULL )
    {
      status = E_FAILURE;
    }
    else
    {
      buf_ptr->pub.state = QMI_VOICE_CALL_LIST_STATE_ACTIVE;
      QM_MSG_LOW_1("call id:  %d state:  Active", buf_ptr->pub.call_id);
    }
  } 

  if(active_calls_list_ptr->size > 1)
  {
   is_mpty_recieved_from_mng_calls_conf = TRUE;
  }
  else
  {
   is_mpty_recieved_from_mng_calls_conf = FALSE;
  }

  /* Check to see if there are call list changes need to be logged */
  buf_ptr = call_list.head_ptr;
  while ( buf_ptr != NULL )
  {
    if ( ( buf_ptr->pub.call_type == CM_CALL_TYPE_VOICE ) || 
         ( buf_ptr->pub.call_type == CM_CALL_TYPE_EMERGENCY ) )
    {             
      if ( buf_ptr->pub.prev_state != buf_ptr->pub.state )
      {
        /* Log call list changes */
        qmi_voice_call_list_log( &buf_ptr->pub );

        /* Remember to report unsolicted call state changed after all logging is done */
        report_call_state_changed = TRUE;
      }
    }

    buf_ptr = buf_ptr->next_ptr;
  }

  /* Send unsolicited call state changed if needed */
  if ( report_call_state_changed )
  {
    qmi_voice_call_list_report_all_call_status_changed(as_id);
  }

  return status;

} /* qmi_voice_call_list_manage */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_active_call

===========================================================================*/
/*!
    @brief
    Change the state of the entry in the list with given Call ID to 
    QMI_VOICE_CALL_LIST_STATE_ACTIVE.  
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_active_call
( 
  cm_call_id_type call_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  uint8 as_id = (uint8)QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    buf_ptr->pub.prev_state = buf_ptr->pub.state;
    buf_ptr->pub.state = QMI_VOICE_CALL_LIST_STATE_ACTIVE;

    /* Log call list changes */
    qmi_voice_call_list_log( &buf_ptr->pub );

    if(buf_ptr->pub.is_as_id_valid)
    {
      as_id = buf_ptr->pub.asubs_id;
    }

    /* Send unsolicited call state change if needed */
    qmi_voice_call_list_report_all_call_status_changed(as_id);
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_active_call() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_hold_call

===========================================================================*/
/*!
    @brief
    Change the state of the entry in the list with given Call ID to 
    QMI_VOICE_CALL_LIST_STATE_HOLDING.  
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_hold_call
( 
  cm_call_id_type call_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  uint8 as_id = (uint8)QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    buf_ptr->pub.prev_state = buf_ptr->pub.state;
    buf_ptr->pub.state = QMI_VOICE_CALL_LIST_STATE_HOLDING;

    /* Log call list changes */
    qmi_voice_call_list_log( &buf_ptr->pub );

    if(buf_ptr->pub.is_as_id_valid)
    {
      as_id = buf_ptr->pub.asubs_id;
    }

    /* Send unsolicited call state change if needed */
    qmi_voice_call_list_report_all_call_status_changed(as_id);  
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_hold_call() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_query

===========================================================================*/
/*!
    @brief
    Finds the entry in the list for the given Call ID.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_query
( 
  cm_call_id_type call_id,
  qmi_voice_call_list_public_type *info_ptr
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  qmi_voice_call_list_public_type *pub_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( info_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    pub_ptr = &buf_ptr->pub;
    *info_ptr = *pub_ptr;
  }
  else
  {
    /* Call ID not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_query */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_query_voice_call_ids_list_by_state

===========================================================================*/
/*!
    @brief
    Finds the Call IDs of voice entries in the list with the given state.
 
    @return
    None
*/
/*=========================================================================*/
void qmi_voice_call_list_query_voice_call_ids_list_by_state
( 
  uint32 state_mask,
  qmi_voice_call_list_call_ids_list_type *call_ids_list_ptr,
  uint8  as_id
)
{                                   
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( call_ids_list_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  call_ids_list_ptr->num_of_call_ids = 0;

  buf_ptr = call_list.head_ptr;

  while ( buf_ptr != NULL )
  {
    if ( ( buf_ptr->pub.asubs_id == as_id ) && QMI_VOICE_CALL_LIST_CALL_TYPE_IS_VOICE( buf_ptr->pub.call_type ) &&
         ( buf_ptr->pub.state & state_mask ) )
    {             
      call_ids_list_ptr->call_id[ call_ids_list_ptr->num_of_call_ids] = buf_ptr->pub.call_id; 
      call_ids_list_ptr->answered[ call_ids_list_ptr->num_of_call_ids++ ] = buf_ptr->pub.answered; 

      QM_MSG_MED_4( "call list entry with call id %d , state %s (%d), Answered : (%d)", 
                      buf_ptr->pub.call_id, qmi_voice_call_list_lookup_state_name( buf_ptr->pub.state ), buf_ptr->pub.state, buf_ptr->pub.answered);	  
    }

    buf_ptr = buf_ptr->next_ptr;
  }

} /* qmi_voice_call_list_query_voice_call_ids_by_state */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_query_call_info_list

===========================================================================*/
/*!
    @brief
    Find all voice call entries in the list.
 
    @return
    None
*/
/*=========================================================================*/
void qmi_voice_call_list_query_call_info_list
( 
  qmi_voice_call_list_call_info_list_type *info_list_ptr,
  uint8                                   as_id
)
{
  qmi_voice_call_list_buf_type *buf_ptr = call_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( info_list_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  info_list_ptr->num_of_calls = 0;

  buf_ptr = call_list.head_ptr;

  while ( buf_ptr != NULL )
  {
    if ( (buf_ptr->pub.asubs_id == as_id) && QMI_VOICE_CALL_LIST_HANDLE_CALL_TYPES( buf_ptr->pub.call_type ) && 
         (info_list_ptr->num_of_calls < QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS))
    {
      info_list_ptr->call_id[ info_list_ptr->num_of_calls++ ] = buf_ptr->pub.call_id;
    }
    buf_ptr = buf_ptr->next_ptr;
  }

} /* qmi_voice_call_list_query_call_info_list */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_query_call_info_list_per_subs

===========================================================================*/
/*!
    @brief
    Find all voice call entries in the list per subscription basis.
 
    @return
    None
*/
/*=========================================================================*/
void qmi_voice_call_list_query_call_info_list_per_subs
( 
  qmi_voice_call_list_call_info_list_type *info_list_ptr , uint8 as_id
)
{
  uint32 num_of_active_voice_calls = 0;
  qmi_voice_call_list_buf_type *buf_ptr = call_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( info_list_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  info_list_ptr->num_of_calls = 0;

  buf_ptr = call_list.head_ptr;

  while ( buf_ptr != NULL )
  {
    if ( QMI_VOICE_CALL_LIST_HANDLE_CALL_TYPES( buf_ptr->pub.call_type ) &&
         (buf_ptr->pub.asubs_id == as_id))
    {
      info_list_ptr->call_id[ info_list_ptr->num_of_calls++ ] = buf_ptr->pub.call_id;

      if ( buf_ptr->pub.state == QMI_VOICE_CALL_LIST_STATE_ACTIVE )
      {
        num_of_active_voice_calls++;
      }
    }

    buf_ptr = buf_ptr->next_ptr;
  }

} /* qmi_voice_call_list_query_call_info_list_per_subs */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_incoming

===========================================================================*/
/*!
    @brief
    Change the state of the entry in the list to incoming
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_incoming
( 
  cm_call_id_type call_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  uint8 as_id = (uint8)QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  /*-----------------------------------------------------------------------*/
                                    
  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( ( ( buf_ptr->pub.call_type == CM_CALL_TYPE_VOICE ) ||
           ( buf_ptr->pub.call_type == CM_CALL_TYPE_EMERGENCY ) ||
           ( buf_ptr->pub.call_type == CM_CALL_TYPE_VT ) ) &&
         ( buf_ptr->pub.direction == CM_CALL_DIRECTION_MT ) && 
         ( buf_ptr->pub.state == QMI_VOICE_CALL_LIST_STATE_SETUP ) )
    {
      /* Log call list changes */
      QM_MSG_HIGH_3( "Change in call state of call list entry :-- call id %d, state %s (%d)", 
                       call_id, qmi_voice_call_list_lookup_state_name( buf_ptr->pub.state ), buf_ptr->pub.state );

      buf_ptr->pub.prev_state = buf_ptr->pub.state;
      if(buf_ptr->pub.is_as_id_valid)
      {
		as_id = buf_ptr->pub.asubs_id;
      }

      /* If in MPTY, state should be WAITING */
      if ( (qmi_voice_call_list_count_num_of_voice_calls_by_state_and_mode( (uint16)(QMI_VOICE_CALL_LIST_STATE_ACTIVE | QMI_VOICE_CALL_LIST_STATE_HOLDING), buf_ptr->pub.mode_info_type, as_id ) > 0 ) )
      {
        buf_ptr->pub.state = QMI_VOICE_CALL_LIST_STATE_WAITING;
      }
      else
      {
        buf_ptr->pub.state = QMI_VOICE_CALL_LIST_STATE_INCOMING;
      }
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }
  return status;

} /* qmi_voice_call_list_incoming() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_voice_privacy_mode

===========================================================================*/
/*!
    @brief
    Change the privacy_mode of the entry in the list with given Call ID to 
    voice_privacy.  
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_voice_privacy_mode
( 
  cm_call_id_type call_id,
  boolean privacy_mode

)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  voice_privacy_ind_msg_v02  privacy_ind;
  boolean send_voice_priv_ind =  FALSE;
  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( ( buf_ptr->pub.call_type == CM_CALL_TYPE_VOICE ) ||
         ( buf_ptr->pub.call_type == CM_CALL_TYPE_EMERGENCY )  )
    {
      /* Log call list changes */
      QM_MSG_MED_3("Change in call state of call list privacy entry : call id %d, %d -> %d", call_id, buf_ptr->pub.privacy, privacy_mode);

      if ( ( buf_ptr->pub.is_privacy_valid == FALSE )|| 
           ( buf_ptr->pub.is_privacy_valid == TRUE && 
             buf_ptr->pub.privacy != privacy_mode ) )
      {
        send_voice_priv_ind = TRUE;
      }
      
      buf_ptr->pub.privacy = (uint8) privacy_mode;
      buf_ptr->pub.is_privacy_valid = TRUE;

      /* Send voice privacy indication */
      privacy_ind.voice_privacy_info.call_id = buf_ptr->pub.conn_index;
      privacy_ind.voice_privacy_info.voice_privacy = (voice_privacy_enum_v02)privacy_mode;

      if (send_voice_priv_ind == TRUE)
      {
        qmi_voice_cm_if_unsol_response(QMI_VOICE_CM_IF_CMD_PRIVACY_IND,(void *)&privacy_ind,sizeof(privacy_ind),buf_ptr->pub.asubs_id);
      }
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_voice_privacy_mode() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_name_presentation

===========================================================================*/
/*!
    @brief
    Change the name presentation of the entry in the list with given Call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_name_presentation
(
  cm_call_id_type call_id,
  uint8  name_presentation
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( name_presentation != buf_ptr->pub.cnap_info.name_presentation )
    {
      /* Log call list changes */
      QM_MSG_MED_3("Change in call state of call list entry :  call id %d, name_presentation %d -> %d", call_id, buf_ptr->pub.cnap_info.name_presentation, name_presentation);

      buf_ptr->pub.cnap_info.name_presentation = name_presentation;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_name_presentation() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_coding_scheme

===========================================================================*/
/*!
    @brief
    Change the coding scheme of the entry in the list with given Call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_coding_scheme
(
  cm_call_id_type call_id,
  uint8  coding_scheme
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( coding_scheme != buf_ptr->pub.cnap_info.coding_scheme )
    {
      /* Log call list changes */
      QM_MSG_MED_3("Change in call state of call list entry :  call id %d, coding scheme %d -> %d", call_id, buf_ptr->pub.cnap_info.coding_scheme, coding_scheme);

      buf_ptr->pub.cnap_info.coding_scheme = coding_scheme;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_coding_scheme() */



/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_number

===========================================================================*/
/*!
    @brief
    Change the number of the entry in the list with given Call ID
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_number
( 
  cm_call_id_type call_id,
  cm_num_s_type * number
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/
                                    

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    /* Check if the number has changed */
    if (strncmp( (char *)number->buf, (char *)buf_ptr->pub.num.buf, CM_MAX_NUMBER_CHARS) != 0)
    {
      /* Log call list changes */
      QM_MSG_MED_3( "Change in call state of call list entry : call id %d, number %s -> %s", 
                      call_id, buf_ptr->pub.num.buf, number->buf );
    }

    /* Check if the number presentation has changed */
    if (number->pi != buf_ptr->pub.num.pi)
    {
      /* Log call list changes */
      QM_MSG_MED_3("Change in call state of call list entry :  call id %d, number presentation %d -> %d", call_id, buf_ptr->pub.num.pi, number->pi);
    }

    buf_ptr->pub.num = *number;
    buf_ptr->pub.num.len = MIN(number->len, CM_MAX_NUMBER_CHARS);
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_number() */



/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_alpha_info

===========================================================================*/
/*!
    @brief
    Change the alpha_info of the entry in the list with given Call ID
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_alpha_info
( 
  cm_call_id_type call_id,
  cm_alpha_s_type alpha
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    QM_MSG_MED_2("Alpha Present:  call id=%d, Alpha len =%d", call_id, alpha.len);
    if(alpha.len > 0)
    {
      memset(&buf_ptr->pub.alpha_info,0,sizeof(cm_alpha_s_type));
      buf_ptr->pub.is_alpha_valid = TRUE;
      memscpy(&(buf_ptr->pub.alpha_info), sizeof(cm_alpha_s_type), &alpha, sizeof(cm_alpha_s_type));
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;
} /* qmi_voice_call_list_update_alpha_info() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_voice_is_emergency

===========================================================================*/
/*!
    @brief
    Check if the call_type of the given Call ID is an emergency call.  
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
boolean qmi_voice_call_list_voice_is_emergency
( 
  cm_call_id_type call_id
)
{
  boolean is_emergency = FALSE;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {   
    if ( buf_ptr->pub.call_type == CM_CALL_TYPE_EMERGENCY  )
    {
      is_emergency = TRUE;
    }
  }

  return is_emergency;

} /* qmi_voice_call_list_voice_is_emergency() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_query_call_id

===========================================================================*/
/*!
    @brief
    Find an entry in the list based on Call ID
 
    @return
    E_SUCCESS if an entry with passed Call ID is found
    E_FAILURE otherwise
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_query_call_id
(
  cm_call_id_type call_id,
  qmi_voice_call_list_public_type* info_ptr
) 
{
  IxErrnoType status = E_FAILURE;
  qmi_voice_call_list_buf_type *buf_ptr = call_list.head_ptr;

  /*-----------------------------------------------------------------------*/
  ASSERT( info_ptr != NULL );
  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL )
  {
    if ( buf_ptr->pub.call_id == call_id )
    {
      QM_MSG_MED_1("Found Call List entry:  call id %d", call_id);
      memscpy(info_ptr, sizeof(info_ptr), &buf_ptr->pub, sizeof(buf_ptr->pub));
      status = E_SUCCESS;
      break;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }
  //QM_MSG_MED_1("Not found Call List entry:  call id %d", call_id);
  return status;
} /* qmi_voice_call_list_query_call_id() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_query_by_call_id_and_call_state

===========================================================================*/
/*!
    @brief
    Find if an entry exists in call list with given state and Call ID
 
    @return
    E_SUCCESS if an entry with passed Call ID and Call state is found
    E_FAILURE otherwise
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_query_by_call_id_and_call_state
(
  cm_call_id_type call_id,
  qmi_voice_call_list_state_e_type call_state
) 
{
  IxErrnoType status = E_FAILURE;
  qmi_voice_call_list_buf_type *buf_ptr = call_list.head_ptr;

  while ( buf_ptr != NULL )
  {
    if ( (buf_ptr->pub.call_id == call_id)  && (buf_ptr->pub.state == call_state) )
    {
      QM_MSG_MED_2("Found Call List entry:  call id %d with state = %d", call_id, call_state);
      status = E_SUCCESS;
      break;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }
  //QM_MSG_MED_2("Not found Call List entry:  call id %d and state=%d", call_id, call_state);
  return status;
} /* qmi_voice_call_list_query_by_call_id_and_call_state() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_query_sys_mode_from_call_list

===========================================================================*/
/*!
    @brief
    get the sys mode of some call list entry
 
    @return
    E_SUCCESS if sys mode could be found in the list 
    E_FAILURE otherwise
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_query_sys_mode_from_call_list
(
  sys_sys_mode_e_type     *sys_mode,
  uint8                    as_id
) 
{
  IxErrnoType status = E_FAILURE;
  qmi_voice_call_list_buf_type *buf_ptr = call_list.head_ptr;

  while ( buf_ptr != NULL )
  {
    if ( (buf_ptr->pub.call_mode != SYS_SYS_MODE_NO_SRV) &&
         (buf_ptr->pub.asubs_id == as_id) )
    {
      *sys_mode = buf_ptr->pub.call_mode;
      QM_MSG_MED_1("Found Call List entry with call mode %d", *sys_mode);
      status = E_SUCCESS;
      break;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }
  //Not found Call List entry: with non-zero call mode
  return status;
} /* qmi_voice_call_list_query_sys_mode_from_call_list() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_query_call_mode_by_state

===========================================================================*/
/*!
    @brief
    get the mode of waiting call 
 
    @return
    E_SUCCESS if sys mode could be found in the list 
    E_FAILURE otherwise
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_query_call_mode_by_state
(
  sys_sys_mode_e_type     *sys_mode,
  uint8                    as_id,
  qmi_voice_call_list_state_e_type state
) 
{
  IxErrnoType status = E_FAILURE;
  qmi_voice_call_list_buf_type *buf_ptr = call_list.head_ptr;

  while ( buf_ptr != NULL )
  {
    if ( (buf_ptr->pub.call_mode != SYS_SYS_MODE_NO_SRV) &&
         (buf_ptr->pub.asubs_id == as_id) &&
         (buf_ptr->pub.state == state) )
    {
      *sys_mode = buf_ptr->pub.call_mode;
      QM_MSG_MED_1("Found Waiting Call in List with call mode %d", *sys_mode);
      status = E_SUCCESS;
      break;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }
  //QM_MSG_MED("Not found Waiting Call List entry: with non-zero call mode");
  return status;
} /* qmi_voice_call_list_query_call_mode_by_state() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_call_id

===========================================================================*/
/*!
    @brief
    Change the CM Call ID in Clist for a given Connection Index.

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_call_id
(
  uint8           conn_idx,
  cm_call_id_type call_id
)
{
  IxErrnoType status = E_FAILURE;
  qmi_voice_call_list_buf_type *buf_ptr = NULL;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find_with_conn_index( conn_idx );

  /*-----------------------------------------------------------------------*/

  if( (NULL != buf_ptr) && (buf_ptr->pub.call_id != call_id) )
  {
    QM_MSG_HIGH_2("Update Clist call_id from %d to %d", buf_ptr->pub.call_id, call_id);
    buf_ptr->pub.call_id = call_id;
    status = E_SUCCESS;
  }

  return status;

} /* qmi_voice_call_list_update_call_id() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_replace

===========================================================================*/
/*!
    @brief
    Replaces an existing call with a new call.
 
    @return
    E_SUCCESS if the existing call was successfully replaced
    E_FAILURE if the existing call was not successfully replaced
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_replace
( 
  cm_call_id_type           call_id,
  uint32                    conn_index,
  cm_call_type_e_type       call_type,      
  sys_sys_mode_e_type       call_mode,
  cm_call_direction_e_type  direction,
  cm_call_state_e_type      call_state,          
  cm_als_line_e_type        line,           
  cm_num_s_type             num,
  cm_call_mode_info_e_type  mode_info_type,
  boolean                   report_call_state_changed           
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  qmi_voice_call_list_state_e_type state=QMI_VOICE_CALL_LIST_STATE_IDLE;
  uint8 original_pi_rcvd_in_setup = 0;
  uint8 original_si_rcvd_in_setup = 0;
  uint8 as_id = (uint8)QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  buf_ptr = qmi_voice_call_list_find( call_id );

  /* Entry was found */
  if ( buf_ptr != NULL )
  {

    if(buf_ptr->pub.is_as_id_valid)
	  {
	    as_id = buf_ptr->pub.asubs_id;
	  }
    
    if ( call_state == CM_CALL_STATE_INCOM )
	  {
	    //qmi_voice_call_list_replace cm_call_state_incom
	    state = QMI_VOICE_CALL_LIST_STATE_INCOMING;	 
	    if ( (mode_info_type != CM_CALL_MODE_INFO_CDMA) && 
		     (  ( call_type == CM_CALL_TYPE_VOICE ) ||
			    ( call_type == CM_CALL_TYPE_VT ) ||
			    ( call_type == CM_CALL_TYPE_EMERGENCY )
		     ) && 
           ( qmi_voice_call_list_count_num_of_voice_calls_by_state_and_mode((uint16)(QMI_VOICE_CALL_LIST_STATE_ACTIVE | QMI_VOICE_CALL_LIST_STATE_HOLDING),mode_info_type, as_id ) > 0 )
           )
	    {
		    state = QMI_VOICE_CALL_LIST_STATE_WAITING;
	    }
    }
	  else if ( call_state == CM_CALL_STATE_CONV ) 
	  {
	    state = QMI_VOICE_CALL_LIST_STATE_ACTIVE;
	  }
	  else if ( call_state == CM_CALL_STATE_ORIG)
	  {
	    state = QMI_VOICE_CALL_LIST_STATE_DIALING;
	  }
	  else
	  {
	    /* Invalid state */
	    QM_MSG_ERROR_3( "Fail to replace CList entry : call id %d (invalid call state %s (%d))", 
					   call_id, qmi_voice_call_list_lookup_state_name( state ), call_state );
	    return E_NOT_ALLOWED;
	  }

    QM_MSG_HIGH_3( "Found CList entry to be replaced: %s (%d), call id=%d",
                     qmi_voice_call_list_lookup_state_name( buf_ptr->pub.state ), buf_ptr->pub.state, buf_ptr->pub.call_id );

    buf_ptr->pub.prev_state = buf_ptr->pub.state;

    buf_ptr->pub.conn_index = conn_index;
    /* Ignore PI in GW case - need to consider only the PI that came in SETUP indication
       Here we handled only INCOMING event assuming that this function won't be called for 
       CONNECT,END,DISCONNECTING,... except INCOMING for MT GW calls.
    */
    if( (call_state == CM_CALL_STATE_INCOM) && 
        (buf_ptr->pub.state == QMI_VOICE_CALL_LIST_STATE_SETUP) )
    {
    
      QM_MSG_HIGH_3("CList entry with orig=pi:(%d), changed pi=(%d), CM call id=%d", buf_ptr->pub.num.pi, num.pi, buf_ptr->pub.call_id);
      original_pi_rcvd_in_setup = buf_ptr->pub.num.pi;
      original_si_rcvd_in_setup = buf_ptr->pub.num.si;
      buf_ptr->pub.num = num;
      buf_ptr->pub.num.len = MIN(num.len, CM_MAX_NUMBER_CHARS);
      buf_ptr->pub.num.pi = original_pi_rcvd_in_setup;
      buf_ptr->pub.num.si = original_si_rcvd_in_setup;
    }
    else
    {
      QM_MSG_HIGH_3("CList entry during non SETUP state orig=pi:(%d), changed pi=(%d), CM call id=%d", buf_ptr->pub.num.pi, num.pi, buf_ptr->pub.call_id);
      buf_ptr->pub.num = num;
      buf_ptr->pub.num.len = MIN(num.len, CM_MAX_NUMBER_CHARS);
    }
    
    buf_ptr->pub.state = state;
    buf_ptr->pub.call_type = call_type;
    buf_ptr->pub.call_mode = call_mode;
    buf_ptr->pub.direction = direction;
    buf_ptr->pub.line = line;           
    buf_ptr->pub.mode_info_type = mode_info_type;
    buf_ptr->pub.answered = FALSE;
    buf_ptr->pub.is_privacy_valid = FALSE;
    buf_ptr->pub.is_otasp_status_valid = FALSE;
    buf_ptr->pub.otasp_status = QMI_VOICE_CALL_LIST_OTASP_STATUS_NONE ;
    buf_ptr->pub.is_srv_opt_valid = FALSE;
    buf_ptr->pub.is_alerting_type_valid = FALSE;
    buf_ptr->pub.uus_data.mt_user_data.present = FALSE;

    if(buf_ptr->pub.alpha_info.len == 0)
    {      
      buf_ptr->pub.is_alpha_valid = FALSE;
      memset(&buf_ptr->pub.alpha_info,0,sizeof(cm_alpha_s_type));
    }
    else
    {
      QM_MSG_MED_1("Alpha non-null, not reset in replace: len=%d", buf_ptr->pub.alpha_info.len);
    }

    if(buf_ptr->pub.second_alpha_info.len == 0)
    {      
      buf_ptr->pub.is_second_alpha_valid = FALSE;
      memset(&buf_ptr->pub.second_alpha_info,0,sizeof(cm_alpha_s_type));
    }
    else
    {
      QM_MSG_MED_1("Second Alpha non-null, not reset in replace: len=%d", buf_ptr->pub.alpha_info.len);
    }

    /* Log CList changes */
    qmi_voice_call_list_log( &buf_ptr->pub );
    QM_MSG_HIGH_4( "Replaced CList entry: %s (%d), call id=%d, mode_info_type=%d",
						 qmi_voice_call_list_lookup_state_name( buf_ptr->pub.state ), buf_ptr->pub.state, buf_ptr->pub.call_id,buf_ptr->pub.mode_info_type);

    /* Send unsolicited call state changed if needed. The assumption currently is that
    only voice call can happen to use the replace */
    if ( report_call_state_changed)
    {
      qmi_voice_call_list_report_all_call_status_changed(as_id );
    }
  }
  /* Entry was not found */
  else
  {
    status = E_FAILURE;
  }
  return status;

} /* qmi_voice_call_list_replace() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_set_uus_data

===========================================================================*/
/*!
    @brief
    Set the uus present entry in the list with given Call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_set_uus_data
(
  cm_call_id_type call_id,
  cm_call_event_user_data_s_type  *call_event_user_data
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  voice_uus_ind_msg_v02  uus_ind;
  uint8 conn_index=0;
  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( (buf_ptr != NULL) && (call_event_user_data != NULL) )
  {
    memset(&(buf_ptr->pub.uus_data),0,sizeof(cm_call_event_user_data_s_type));
    memscpy(&(buf_ptr->pub.uus_data),sizeof(buf_ptr->pub.uus_data),
            call_event_user_data, sizeof(cm_call_event_user_data_s_type));
    /* Log Call List changes */
    QM_MSG_MED_7( "Change in call state of CList entry : call id %d, uus_len=%d, uus_data=%s,uus_type=%d, uus_dcs=%d, uus_more_data_ind=%d,uus_reciver_busy=%d",
                                  call_id, buf_ptr->pub.uus_data.mt_user_data.user_user_data_length,
                                  buf_ptr->pub.uus_data.mt_user_data.user_user_data, buf_ptr->pub.uus_data.mt_user_data.user_user_type, buf_ptr->pub.uus_data.mt_user_data.user_user_protocol_disc, 
                                  buf_ptr->pub.uus_data.mt_user_data.more_data_indicator, buf_ptr->pub.uus_data.is_receiver_busy);

    if(qmi_voice_call_list_get_conn_idx_from_call_id(call_id, &conn_index) == E_SUCCESS)
    {
      memset(&uus_ind, 0, sizeof(uus_ind));
      uus_ind.uus_information.call_id = conn_index;
      uus_ind.uus_information.uus_data_len = buf_ptr->pub.uus_data.mt_user_data.user_user_data_length;
      uus_ind.uus_information.uus_dcs = (uus_dcs_enum_v02)buf_ptr->pub.uus_data.mt_user_data.user_user_protocol_disc;
      uus_ind.uus_information.uus_type = (uus_type_enum_v02)buf_ptr->pub.uus_data.mt_user_data.user_user_type;
      memscpy(uus_ind.uus_information.uus_data, sizeof(uus_ind.uus_information.uus_data),
              buf_ptr->pub.uus_data.mt_user_data.user_user_data, uus_ind.uus_information.uus_data_len);
      qmi_voice_cm_if_unsol_response(QMI_VOICE_CM_IF_CMD_UUS_IND,(void *)&uus_ind,sizeof(uus_ind), buf_ptr->pub.asubs_id);
      
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_set_uus_data() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_otasp_status

===========================================================================*/
/*!
    @brief
    Update otasp status of the entry in the list with given Call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_otasp_status
(
  cm_call_id_type         call_id,
  qmi_voice_call_list_otasp_status_e_type  otasp_status
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  voice_otasp_status_ind_msg_v02 otasp_ind;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( otasp_status != buf_ptr->pub.otasp_status)
    {
      /* Log call list changes */
      QM_MSG_MED_3("Change in call state of call list entry :  call id %d, otasp_status %d -> %d", call_id, buf_ptr->pub.otasp_status, otasp_status);

      buf_ptr->pub.otasp_status = otasp_status;
      buf_ptr->pub.is_otasp_status_valid = TRUE;

      /* Send otasp status indication */
      otasp_ind.otasp_status_info.call_id = buf_ptr->pub.conn_index;
      otasp_ind.otasp_status_info.otasp_status = (otasp_status_enum_v02)otasp_status;
      qmi_voice_cm_if_unsol_response(QMI_VOICE_CM_IF_CMD_OTASP_STATUS_IND,(void *)&otasp_ind,sizeof(otasp_ind), buf_ptr->pub.asubs_id);

    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_otasp_status() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_cdma_srv_opt

===========================================================================*/
/*!
    @brief
    Update service option of the entry in the list with given Call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_cdma_srv_opt
(
  cm_call_id_type    call_id,
  uint16             srv_opt
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( srv_opt != buf_ptr->pub.srv_opt)
    {
      /* Log call list changes */
      QM_MSG_MED_3("Change in call state of call list entry :  call id %d, srv_opt %d -> %d", call_id, buf_ptr->pub.srv_opt, srv_opt);

      buf_ptr->pub.srv_opt = srv_opt;
      buf_ptr->pub.is_srv_opt_valid = TRUE;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_cdma_srv_opt() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_second_alpha_info

===========================================================================*/
/*!
    @brief
    Update second alpha of the entry in the list with given Call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_second_alpha_info
(
  cm_call_id_type              call_id,
  cm_orig_client_categ_e_type  client_categ,
  cm_alpha_s_type              alpha
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( client_categ == CM_ORIG_CLIENT_CATEG_CARD )
    {
      QM_MSG_MED_2("Second Alpha Present:  call id=%d, Alpha len =%d", call_id, alpha.len);
      if(alpha.len > 0)
      {
        memset(&buf_ptr->pub.second_alpha_info,0,sizeof(cm_alpha_s_type));
        buf_ptr->pub.is_second_alpha_valid = TRUE;
        buf_ptr->pub.second_alpha_info = alpha;
    }
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;
}

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_call_type

===========================================================================*/
/*!
    @brief
    Update call type

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_call_type
(
  cm_call_id_type                  call_id,
  cm_call_type_e_type              call_type
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( buf_ptr->pub.call_type != call_type )
    {
      /* Log call list changes */
      QM_MSG_MED_3("Change in call type of call list entry :  call id %d, call_type %d -> %d", call_id, buf_ptr->pub.call_type, call_type);

      buf_ptr->pub.call_type = call_type;
    }
    else
    {
      status = E_FAILURE;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }
  return status;
} /* qmi_voice_call_list_update_call_type() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_num_pi

===========================================================================*/
/*!
    @brief
    Update num pi

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_num_pi
(
  cm_call_id_type                  call_id,
  cm_npi_type                      npi
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( buf_ptr->pub.num.pi != npi )
    {
      /* Log call list changes */
      QM_MSG_MED_3("Change in num pi of call list entry :  call id %d, num.pi %d -> %d", call_id, buf_ptr->pub.num.pi, npi);

      buf_ptr->pub.num.pi = npi;
    }
    else
    {
      status = E_FAILURE;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }
  return status;
} /* qmi_voice_call_list_update_num_pi() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_call_state

===========================================================================*/
/*!
    @brief
    Update call state 

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_call_state
(
  cm_call_id_type                  call_id,
  qmi_voice_call_list_state_e_type state
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( buf_ptr->pub.state != state )
    {
      /* Log call list changes */
      QM_MSG_MED_3("Change in call state of call list entry :  call id %d, call_state %d -> %d", call_id, buf_ptr->pub.state, state);

      buf_ptr->pub.prev_state = buf_ptr->pub.state;
      buf_ptr->pub.state = state;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }
  return status;
} /* qmi_voice_call_list_update_call_state() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_parent_id

===========================================================================*/
/*!
    @brief
    Update parent call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_parent_id
(
  cm_call_id_type                  call_id,
  cm_call_id_type                  parent_id,
  boolean                          is_cleared,
  boolean                          is_int_ims
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  uint8 parent_conn_idx=0;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if(parent_id != CM_CALL_ID_INVALID)
    {
      if(!is_int_ims)
      {
        /*In other IMS case, parent ID is same as instance ID given*/
        buf_ptr->pub.parent_call_id = parent_id;
      }
      else
      {
        /*In internal IMS case, find the connex idx for CM call ID*/
        if(qmi_voice_call_list_get_conn_idx_from_call_id(parent_id, &parent_conn_idx) == E_SUCCESS)
        {
          QM_MSG_MED_3("Parent call ID updated :  call id %d, parent ID %d -> %d", call_id, buf_ptr->pub.parent_call_id, parent_id);
          buf_ptr->pub.parent_call_id = parent_conn_idx;
        }
      }

      buf_ptr->pub.is_parent_call_id_cleared = is_cleared;
      buf_ptr->pub.is_parent_call_id_valid = TRUE;
    }// else {buf_ptr->pub.is_parent_call_id_valid = FALSE;}
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }
  return status;
} /* qmi_voice_call_list_update_parent_id() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_drvcc_parent_id

===========================================================================*/
/*!
    @brief
    Update DRVCC parent call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_drvcc_parent_id
(
  cm_call_id_type                  call_id,
  cm_call_id_type                  drvcc_parent_id,
  boolean                          is_cleared,
  boolean                          is_int_ims
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  uint8 drvcc_parent_conn_idx=0;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if(drvcc_parent_id != CM_CALL_ID_INVALID)
    {
      if(!is_int_ims)
      {
        /*In other IMS case, DRVCC parent ID is same as instance ID given*/
        buf_ptr->pub.drvcc_parent_call_id = drvcc_parent_id;
      }
      else
      {
        /*In internal IMS case, find the connex idx for CM call ID*/
        if(qmi_voice_call_list_get_conn_idx_from_call_id(drvcc_parent_id, &drvcc_parent_conn_idx) == E_SUCCESS)
        {
          QM_MSG_MED_3("DRVCC Parent call ID updated :  call id %d, DRVCC parent ID %d -> %d", call_id, buf_ptr->pub.drvcc_parent_call_id, drvcc_parent_id);
          buf_ptr->pub.drvcc_parent_call_id = drvcc_parent_conn_idx;
        }
        else
        {
          QM_MSG_MED_1("DRVCC Parent Call ID %d is not found!", drvcc_parent_id);
        }
      }

      buf_ptr->pub.is_drvcc_parent_call_id_cleared = is_cleared;
      buf_ptr->pub.is_drvcc_parent_call_id_valid = TRUE;
    }// else {buf_ptr->pub.is_parent_call_id_valid = FALSE;}
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }
  return status;
} /* qmi_voice_call_list_update_drvcc_parent_id() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_is_remote_alerting

===========================================================================*/
/*!
    @brief
    Check whether given Call ID has local ringback set

    @return
    TRUE if the entry was found and local ringback set
    FALSE if the entry was not found or local ringback not set
*/
/*=========================================================================*/
boolean qmi_voice_call_list_is_remote_alerting
(
  cm_call_id_type call_id
)
{
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    /* Log CList changes */
    QM_MSG_MED_2("call state of CList entry :  call id %d, alerting type :%d", call_id, buf_ptr->pub.alerting_type);
    return (buf_ptr->pub.alerting_type == QMI_VOICE_CM_IF_REMOTE_ALERTING);
  }
  
  return FALSE;
} /* qmi_voice_call_list_is_remote_alerting() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_alerting_type

===========================================================================*/
/*!
    @brief
    Change the local ringback present of the entry in the list with given Call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_alerting_type
(
  cm_call_id_type call_id,
  uint8           alert_type
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;


  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( alert_type != buf_ptr->pub.alerting_type)
    {
      /* Log CList changes */
      QM_MSG_MED_3("Change in call state of CList entry :  call id %d, Alerting type %d -> %d", call_id, buf_ptr->pub.alerting_type, alert_type);

      buf_ptr->pub.alerting_type= alert_type;
    }
    buf_ptr->pub.is_alerting_type_valid = TRUE;
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_alerting_type() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_srvcc_caller_info_type

===========================================================================*/
/*!
    @brief
    Update the call list with the caller info (caller name and caller name pi)

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_srvcc_caller_info_type
(
  cm_call_id_type call_id,
  cm_srvcc_caller_name_s_type     name_info
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;


  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {

      /* Log CList changes */
      QM_MSG_MED_2("Change in call state of CList entry :  name len %d, name pi %d", name_info.caller_len, name_info.caller_name_pi);

      memscpy(buf_ptr->pub.ip_caller_name.name, sizeof(buf_ptr->pub.ip_caller_name.name), name_info.caller_name, 2*name_info.caller_len);
      buf_ptr->pub.ip_caller_name.name_len= (uint8)name_info.caller_len;
      buf_ptr->pub.name_pi = name_info.caller_name_pi;
      buf_ptr->pub.is_srvcc_caller_name_available = TRUE;

      /*Freeing memory which is allocated by CM while sending HO complete IND*/
      QMI_VOICE_CM_IF_MEM_FREE(name_info.caller_name);
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_srvcc_caller_info_type() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_end_reason_text

===========================================================================*/
/*!
    @brief
    Change the end reason text of the entry in the list with given Call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_end_reason_text
(
  cm_call_id_type                call_id,
  cm_ip_call_end_reason_s_type  *ims_end_reason_ptr
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;


  /*-----------------------------------------------------------------------*/

  if (ims_end_reason_ptr == NULL)
  {
    QM_MSG_ERROR("ims_end_reason_ptr is NULL");
    return E_FAILURE;
  }

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    QM_MSG_MED_3("update_end_reason_text: call_id %d, current character count %d, new character count %d",
                  call_id,
                  buf_ptr->pub.end_reason_text.end_reason_text_length,
                  ims_end_reason_ptr->end_reason_len );
    buf_ptr->pub.end_reason_text.end_reason_text_length = ims_end_reason_ptr->end_reason_len;
    memscpy( buf_ptr->pub.end_reason_text.end_reason_text_buf, sizeof(buf_ptr->pub.end_reason_text.end_reason_text_buf),
             ims_end_reason_ptr->end_reason_text, 2*ims_end_reason_ptr->end_reason_len );
    buf_ptr->pub.is_end_reason_text_available = TRUE;
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_end_reason_text() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_mode_info_type

===========================================================================*/
/*!
    @brief
    Change the Mode info type of the entry in the list with given Call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_mode_info_type
(
  cm_call_id_type call_id,
  cm_call_mode_info_e_type  mode_type
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;


  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( mode_type != buf_ptr->pub.mode_info_type)
    {
      /* Log CList changes */
      QM_MSG_HIGH_3("Change in call state of CList entry :  call id %d, Mode type %d -> %d", call_id, buf_ptr->pub.mode_info_type, mode_type);

      buf_ptr->pub.mode_info_type= mode_type;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_mode_info_type() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_query_by_call_id

===========================================================================*/
/*!
    @brief
    Finds the entry in the list for the given call id.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_query_by_call_id
( 
  cm_call_id_type call_id,
  qmi_voice_call_list_public_type *info_ptr
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  qmi_voice_call_list_public_type *pub_ptr;

  /*-----------------------------------------------------------------------*/

ASSERT( info_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = call_list.head_ptr;

  while ( buf_ptr != NULL )
  {
    if ( buf_ptr->pub.call_id == call_id )
    {             
      break;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  if ( buf_ptr != NULL )
  {
    pub_ptr = &buf_ptr->pub;
    *info_ptr = *pub_ptr;
    QM_MSG_MED_1("Found CList entry :  call id %d ", buf_ptr->pub.call_id);
    QM_MSG_MED_2( " %s (%d)", qmi_voice_call_list_lookup_state_name( buf_ptr->pub.state ), buf_ptr->pub.state);
  }
  else
  {
    /* Call id not in the list */
    QM_MSG_MED_1("Not found CList entry :  call id %d", call_id);
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_query_by_call_id */



/*===========================================================================

  FUNCTION:  qmi_voice_call_list_is_call_present

===========================================================================*/
/*!
    @brief
    Checks if the call entry is already present.
 
    @return
    TRUE if the entry was found
    FALSE if the entry was not found
*/
/*=========================================================================*/
boolean qmi_voice_call_list_is_call_present
( 
  cm_call_id_type call_id
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  boolean is_present = FALSE;

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    is_present = TRUE;
  }
  return is_present;
} /* qmi_voice_call_list_is_call_present */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_query_voice_calls_by_conn_index

===========================================================================*/
/*!
    @brief
    Finds the entry in the list for the given Connection Index among VOICE_CALLS only
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_query_voice_calls_by_conn_index
( 
  uint8 conn_index,
  qmi_voice_call_list_public_type *info_ptr
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( info_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = call_list.head_ptr;

  while ( buf_ptr != NULL )
  {
    if ( (buf_ptr->pub.conn_index == conn_index)  &&  QMI_VOICE_CALL_LIST_CALL_TYPE_IS_VOICE( buf_ptr->pub.call_type ) )
    {             
      break;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  if ( buf_ptr != NULL )
  {
    *info_ptr = buf_ptr->pub;
    QM_MSG_MED_3( "Found Voice Call List entry : conn index %lu, call id %d, state %s", 
                       conn_index, buf_ptr->pub.call_id, qmi_voice_call_list_lookup_state_name( buf_ptr->pub.state ) );
  }
  else
  {
    /* Connection Index not in the list */
    QM_MSG_MED_1("Not found Voice Call List entry : conn index %lu", conn_index);
    status = E_FAILURE;
  }

  return status;
} /* qmi_voice_call_list_query_voice_calls_by_conn_index */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_is_mpty_call

===========================================================================*/
/*!
    @brief
    Check whether given Call ID has local ringback set

    @return
    TRUE if the entry was found and local ringback set
    FALSE if the entry was not found or local ringback not set
*/
/*=========================================================================*/
boolean qmi_voice_call_list_is_mpty_call
(
  cm_call_id_type call_id
)
{
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    /* Log CList changes */
    QM_MSG_MED_2("call id %d,is_mpty = %d", call_id, buf_ptr->pub.is_multi_party);
    return (buf_ptr->pub.is_multi_party);
  }
  
  return FALSE;
} /* qmi_voice_call_list_is_mpty_call() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_query_by_conn_index

===========================================================================*/
/*!
    @brief
    Finds the entry in the list for the given Connection Index.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_query_by_conn_index
( 
  uint8 conn_index,
  qmi_voice_call_list_public_type *info_ptr
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  qmi_voice_call_list_public_type *pub_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( info_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = call_list.head_ptr;

  while ( buf_ptr != NULL )
  {
    if ( buf_ptr->pub.conn_index == conn_index )
    {             
      break;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  if ( buf_ptr != NULL )
  {
    pub_ptr = &buf_ptr->pub;
    *info_ptr = *pub_ptr;
    QM_MSG_MED_3( "Found Call List entry : conn index %lu, call id %d, state %s", 
                       conn_index, buf_ptr->pub.call_id, qmi_voice_call_list_lookup_state_name( buf_ptr->pub.state ) );
  }
  else
  {
    /* Connection Index not in the list */
    QM_MSG_MED_1( "Not found Call List entry : conn index %lu", conn_index );
    status = E_FAILURE;
  }

  return status;
} /* qmi_voice_call_list_query_by_conn_index */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_get_as_id

===========================================================================*/
/*!
    @brief
    Retrieves the conn index for the given call id.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_get_as_id
( 
  cm_call_id_type call_id,
  uint8  *as_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( as_id != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = call_list.head_ptr;

  while ( buf_ptr != NULL )
  {
    if ( buf_ptr->pub.call_id == call_id )
    {             
      break;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  if ( buf_ptr != NULL && buf_ptr->pub.is_as_id_valid)
  {
    *as_id = buf_ptr->pub.asubs_id;
    QM_MSG_MED_2("Found AS ID %d for call id %d ", buf_ptr->pub.asubs_id, buf_ptr->pub.call_id);
  }
  else
  {
    /* Call id not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_get_as_id */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_get_conn_idx_from_call_id

===========================================================================*/
/*!
    @brief
    Retrieves the conn index for the given call id.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_get_conn_idx_from_call_id
( 
  cm_call_id_type call_id,
  uint8  *conn_idx
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( conn_idx != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = call_list.head_ptr;

  while ( buf_ptr != NULL )
  {
    if ( buf_ptr->pub.call_id == call_id )
    {             
      break;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  if ( buf_ptr != NULL )
  {
    *conn_idx = buf_ptr->pub.conn_index;
    QM_MSG_MED_2("Found Conn idx %d for call id %d ", buf_ptr->pub.conn_index, buf_ptr->pub.call_id);
  }
  else
  {
    /* Call id not in the list */
    QM_MSG_MED_1("Not found Call List entry :  call id %d", call_id);
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_get_conn_idx_from_call_id */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_is_mode_updated

===========================================================================*/
/*!
    @brief
    Update call mode for RAT Changes in between calls.

    @return
    E_SUCCESS if the mode updated.
    E_FAILURE if mode not updated.
*/
/*=========================================================================*/
boolean qmi_voice_call_list_is_mode_updated
(
void
)
{
  boolean status = FALSE;
  qmi_voice_call_list_buf_type *buf_ptr;
  qmi_voice_cm_util_ss_info_s util_sys_info;

  /*-----------------------------------------------------------------------*/

  buf_ptr = call_list.head_ptr;

  memset(&util_sys_info, 0, sizeof(util_sys_info));

  while ( buf_ptr != NULL )
  {
    qmi_voice_cm_util_get_ss_info(&util_sys_info,buf_ptr->pub.asubs_id);

    if ( buf_ptr->pub.call_mode != util_sys_info.sys_mode )
    {
      /* Log call list changes */
      QM_MSG_MED_3("Change in call_mode of call list entry :  call id %d, mode change %d -> %d", buf_ptr->pub.call_id, buf_ptr->pub.call_mode, util_sys_info.sys_mode);
      buf_ptr->pub.call_mode= util_sys_info.sys_mode;
      status = TRUE;
    }
    buf_ptr = buf_ptr->next_ptr;
  }

  return status;
} /* qmi_voice_call_list_is_mode_updated() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_num_info

===========================================================================*/
/*!
    @brief
    Change the number information for a given Call ID is used in CDMA during CM_CALL_EVENT_CALLER_ID.

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_num_info
(
  cm_call_id_type call_id,
  cm_num_s_type   *num_info
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;


  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( (num_info->len > 0) || (buf_ptr->pub.num.pi != num_info->pi))
    {
      /* Log CList changes */
      QM_MSG_HIGH_5( "Change in number info of CList entry : call id %d, num len=%d->%d, num=%s->%s",
                    call_id, buf_ptr->pub.num.len, num_info->len, buf_ptr->pub.num.buf,num_info->buf);
       
      memscpy(&(buf_ptr->pub.num),sizeof(buf_ptr->pub.num),num_info,sizeof(cm_num_s_type));
      buf_ptr->pub.num.len = MIN(num_info->len, CM_MAX_NUMBER_CHARS);
    }

  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_num_info() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_connected_num_info

===========================================================================*/
/*!
    @brief
    updatinh the connected number info for a given Call ID is used in GW/CDMA during CM_CALL_EVENT_CONNECTED_NUM.

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_connected_num_info
(
  cm_call_id_type call_id,
  cm_num_s_type   *num_info,
  boolean is_connected_number_ECT
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr = NULL;

  ASSERT( num_info != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( num_info->len > 0)
    {
      /* Log CList changes */
      QM_MSG_HIGH_5( "Update in connected number info of CList entry : call id %d, num len=%d->%d, num=%s->%s",
                    call_id, buf_ptr->pub.conn_num_info.len, num_info->len, buf_ptr->pub.conn_num_info.num,num_info->buf);

      buf_ptr->pub.is_conn_num_valid = TRUE;
      buf_ptr->pub.conn_num_info.len = MIN(num_info->len, CM_MAX_NUMBER_CHARS);
      buf_ptr->pub.conn_num_info.pi = num_info->pi;
      buf_ptr->pub.conn_num_info.si = num_info->si;
      buf_ptr->pub.conn_num_info.num_plan = num_info->number_plan;
      buf_ptr->pub.conn_num_info.num_type = num_info->number_type;
      memscpy(buf_ptr->pub.conn_num_info.num, sizeof(buf_ptr->pub.conn_num_info.num),
              num_info->buf, buf_ptr->pub.conn_num_info.len);
      buf_ptr->pub.is_connected_number_ECT = is_connected_number_ECT;
      buf_ptr->pub.is_connected_number_ECT_valid = TRUE;
    }

  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_connected_num_info() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_secure_call_info

===========================================================================*/
/*!
    @brief
    updating the secure call info about the call, informing if the call is a secure call or not. This information is recieved 
    from CM with the call event "CM_CALL_EVENT_INCOM"

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_secure_call_info
(
  cm_call_id_type call_id,
  boolean is_secure_call
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr = NULL;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
      /* Log CList changes */
      QM_MSG_HIGH_2( "Update in secure call info of CList entry : call id %d, is_secure_call -> %d",
                    call_id, is_secure_call);

      buf_ptr->pub.is_secure_call_available = TRUE;
      buf_ptr->pub.is_secure_call = is_secure_call;

  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_secure_call_info() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_called_party_ring_alert_info

===========================================================================*/
/*!
    @brief
    updating if called party ring alert rxd in the call object, informing if IMS rxd the 180 ringing from called party for VOLTE call.
    for GW CS CALL updates, if RING_ALERT_IND is received.
    This information is recieved  from CM with the call event "CM_CALL_EVENT_ABRV_ALERT"

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_called_party_ring_alert_info
(
  cm_call_id_type call_id,
  boolean is_called_party_ringing
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr = NULL;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
      /* Log CList changes */
      QM_MSG_HIGH_2( "Update is called party ring rxd info of CList entry : call id %d, is_called_party_ringing -> %d",
                    call_id, is_called_party_ringing);

       /* Set optional TLV in ALL_CALL_STATUS only when is_called_party_ringing is TRUE */
       if(is_called_party_ringing)
       {
         buf_ptr->pub.is_called_party_ringing_status_avail = TRUE;
         buf_ptr->pub.is_called_party_ringing 		= TRUE;

       }
       else
       {
         buf_ptr->pub.is_called_party_ringing_status_avail = FALSE;
         buf_ptr->pub.is_called_party_ringing = FALSE;
       }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_called_party_ring_alert_info() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_diagnostic_info

===========================================================================*/
/*!
    @brief
    Update the diagnostic call end information of the entry in the list with given Call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_diagnostic_info
(
  cm_call_id_type call_id,
  ie_cm_cc_cause_s_type   *cc_cause
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  ASSERT( cc_cause != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( (cc_cause->diagnostic_length > 0) && (cc_cause->diagnostic_length <= MAX_DIAGNOSTICS_LENGTH) )
    {
      /* Log CList changes */
      QM_MSG_MED_3("Update in call end diagnostic information of CList entry :  call id %d, diagnostics len %d -> %d", call_id, buf_ptr->pub.diagnostic_info.diagnostic_length, cc_cause->diagnostic_length);

      memscpy(buf_ptr->pub.diagnostic_info.diagnostics, 
              sizeof(buf_ptr->pub.diagnostic_info.diagnostics),
              cc_cause->diagnostics, cc_cause->diagnostic_length);
      buf_ptr->pub.diagnostic_info.diagnostic_length = cc_cause->diagnostic_length;
      buf_ptr->pub.is_diagnostics_info_valid = TRUE;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_diagnostic_info() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_set_mpty_bit()

===========================================================================*/
/*!
    @brief
    Set the the MPTY bits for Multiple Active/Held calls where ever applicable.

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
void qmi_voice_call_list_update_mpty_bit
(
  uint8 as_id
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  qmi_voice_call_list_call_ids_list_type call_ids_list_held;
  qmi_voice_call_list_call_ids_list_type call_ids_list_active;
  qmi_voice_call_list_call_ids_list_type call_ids_list_other;
  uint8 i=0;

  /*-----------------------------------------------------------------------*/
  memset(&call_ids_list_held,0,sizeof(call_ids_list_held));
  memset(&call_ids_list_active,0,sizeof(call_ids_list_active));
  memset(&call_ids_list_other,0,sizeof(call_ids_list_other));


   /*Set MPTY bit if Multiple Active Calls */
  qmi_voice_call_list_query_voice_call_ids_list_by_state( (uint32) QMI_VOICE_CALL_LIST_STATE_ACTIVE,
                                                      &call_ids_list_active, as_id );
   /*Set MPTY bit if Multiple Held Calls */
  qmi_voice_call_list_query_voice_call_ids_list_by_state( (uint32) QMI_VOICE_CALL_LIST_STATE_HOLDING,
                                                     &call_ids_list_held, as_id );

   /*Disable MPTY bit for All other Calls*/
  qmi_voice_call_list_query_voice_call_ids_list_by_state( (uint32) QMI_VOICE_CALL_LIST_STATE_DIALING | QMI_VOICE_CALL_LIST_STATE_ALERTING
                                                                   | QMI_VOICE_CALL_LIST_STATE_INCOMING | QMI_VOICE_CALL_LIST_STATE_WAITING
                                                                   | QMI_VOICE_CALL_LIST_STATE_DISCONNECTING | QMI_VOICE_CALL_LIST_STATE_END
                                                                   | QMI_VOICE_CALL_LIST_STATE_CC_IN_PROG | QMI_VOICE_CALL_LIST_STATE_SETUP
                                                                   , &call_ids_list_other, as_id );   
       
  if ( (call_ids_list_active.num_of_call_ids > 1) && (is_mpty_recieved_from_mng_calls_conf == TRUE) )
  {
    QM_MSG_MED("Setting MPTY bit for all the Multiple Active calls only after Mng calls conf is recieved");
    for ( i = 0; i < call_ids_list_active.num_of_call_ids; i++ )
    {
      buf_ptr = qmi_voice_call_list_find( call_ids_list_active.call_id[i] );
      if ( buf_ptr == NULL )
      {
        QM_MSG_ERROR_1("Active Call ID %d not found in call list", call_ids_list_active.call_id[i]);
      }
      else
      {
        buf_ptr->pub.is_multi_party = TRUE;
        QM_MSG_MED_3("Connex id:  %d state:  %d, is_Mpty=%d", buf_ptr->pub.conn_index, (int)buf_ptr->pub.state, buf_ptr->pub.is_multi_party);
      }
    }
    is_mpty_recieved_from_mng_calls_conf = FALSE;
  }
  else if ( call_ids_list_held.num_of_call_ids > 1 )
  {
    /* Do not put multiple held calls as MPTY if they are IP calls */
    buf_ptr = qmi_voice_call_list_find( call_ids_list_held.call_id[0] );
    if( ( buf_ptr != NULL ) && (buf_ptr->pub.mode_info_type != CM_CALL_MODE_INFO_IP) )
    {
    QM_MSG_MED("setting MPTY bit for all the Multiple Held calls");
    for ( i = 0; i < call_ids_list_held.num_of_call_ids; i++ )
    {
      buf_ptr = qmi_voice_call_list_find( call_ids_list_held.call_id[i] );
      if ( buf_ptr == NULL )
      {
        QM_MSG_ERROR_1("Held Call ID %d not found in call list", call_ids_list_held.call_id[i]);
      }
      else
      {
        buf_ptr->pub.is_multi_party = TRUE;
        QM_MSG_MED_3("Connex id:  %d state:  %d, is_Mpty=%d", buf_ptr->pub.conn_index, (int)buf_ptr->pub.state, buf_ptr->pub.is_multi_party);
      }
    } 
  }
  else
  {
      QM_MSG_MED("MPTY bit not set for Multiple held calls, they are IP");
    }
  }
  else
  {
    QM_MSG_ERROR("No Multiple Active/Held calls found");
  }

  if ( call_ids_list_other.num_of_call_ids >= 1 )
  {
    QM_MSG_MED("Resetting MPTY bit for all the Non-Active/Non-Held calls");
    for ( i = 0; i < call_ids_list_other.num_of_call_ids; i++ )
    {
      buf_ptr = qmi_voice_call_list_find( call_ids_list_other.call_id[i] );
      if ( buf_ptr == NULL )
      {
        QM_MSG_ERROR_1("Non Active/Held Call ID %d not found in call list", call_ids_list_other.call_id[i]);
      }
      else
      {
        /* For VoIP calls there is only a single visible call which acts as a conf call */
        if(buf_ptr->pub.is_multi_party  && (buf_ptr->pub.mode_info_type != CM_CALL_MODE_INFO_IP))
        {
          buf_ptr->pub.is_multi_party = FALSE;
        }
        QM_MSG_MED_3("Connex id:  %d state:  %d, is_Mpty=%d", buf_ptr->pub.conn_index, (int)buf_ptr->pub.state, buf_ptr->pub.is_multi_party);
      }
    } 
  }

  if ( call_ids_list_active.num_of_call_ids == 1 )
  {
    QM_MSG_MED("Resetting MPTY bit for Single Active call");
    buf_ptr = qmi_voice_call_list_find( call_ids_list_active.call_id[0] );
    if ( buf_ptr == NULL )
    {
      QM_MSG_ERROR_1("Active Call ID %d not found in call list", call_ids_list_active.call_id[0]);
    }
    else
    {
      /* For VoIP calls there is only a single visible call which acts as a conf call */
      if(buf_ptr->pub.is_multi_party  && (buf_ptr->pub.mode_info_type != CM_CALL_MODE_INFO_IP))
      {
        buf_ptr->pub.is_multi_party = FALSE;
      }
      QM_MSG_MED_3("Connex id:  %d state:  %d, is_Mpty=%d", buf_ptr->pub.conn_index, (int)buf_ptr->pub.state, buf_ptr->pub.is_multi_party);
    }
   } 

  if ( call_ids_list_held.num_of_call_ids == 1 )
  {
    QM_MSG_MED("Resetting MPTY bit for Single held call");
    buf_ptr = qmi_voice_call_list_find( call_ids_list_held.call_id[0] );
    if ( buf_ptr == NULL )
    {
      QM_MSG_ERROR_1("Held Call ID %d not found in call list", call_ids_list_held.call_id[0]);
    }
    else
    {
      /* For VoIP calls there is only a single visible call which acts as a conf call */
      if(buf_ptr->pub.is_multi_party  && (buf_ptr->pub.mode_info_type != CM_CALL_MODE_INFO_IP))
      {
        buf_ptr->pub.is_multi_party = FALSE;
      }
      QM_MSG_MED_3("Connex id:  %d state:  %d, is_Mpty=%d", buf_ptr->pub.conn_index, (int)buf_ptr->pub.state, buf_ptr->pub.is_multi_party);
    }
  }   

} /* qmi_voice_call_list_update_mpty_bit() */



/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_alerting_pattern

===========================================================================*/
/*!
    @brief
    Update Alerting Pattern of the entry in the list with given Call ID - GW

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_alerting_pattern
(
  cm_call_id_type         call_id,
  uint8  alerting_pattern
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
      /* Log call list changes */
      QM_MSG_HIGH_3("Change in call state of call list entry :  call id %d, Alerting pattern %d -> %d", call_id, buf_ptr->pub.alert_pattern, alerting_pattern);
      buf_ptr->pub.alert_pattern = alerting_pattern;
      buf_ptr->pub.is_alert_pattern_valid = TRUE;
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_alerting_pattern() */




/*===========================================================================

  FUNCTION:  qmi_voice_call_list_set_mpty_status

===========================================================================*/
/*!
    @brief
    Set the multiparty bit for single call.
    This is useful for VoIP calls as there is only one(visible) call in 
    conference. Also this will be useful for GW if "seperate connection"
    supplementary service is supported.

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_set_mpty_status
(
  cm_call_id_type call_id,
  boolean         is_conf_call
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( is_conf_call != buf_ptr->pub.is_multi_party )
    {
      /* Log call list changes */
      QM_MSG_MED_4( "Change in mpty status for call_id %d (conn-id %d), is_mpty %d -> %d",
                                    call_id,buf_ptr->pub.conn_index, buf_ptr->pub.is_multi_party, is_conf_call);

      buf_ptr->pub.is_multi_party = is_conf_call;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_set_mpty_status() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_cnap_name

===========================================================================*/
/*!
    @brief
    Change the CNAP name of the entry in the list with given Call ID
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_cnap_name
( 
  cm_call_id_type call_id,
  const byte *name,
  uint8 len
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( (buf_ptr != NULL) && (len > 0) )
  {
      /* Log call list changes */
    QM_MSG_MED_3("Change in call state of call list entry :  call id %d, name_len %d -> %d", call_id, buf_ptr->pub.cnap_info.name_len, len);
    memscpy(buf_ptr->pub.cnap_info.name, sizeof(buf_ptr->pub.cnap_info.name), name, len);
    buf_ptr->pub.cnap_info.name_len = len;
    buf_ptr->pub.is_cnap_info_valid = TRUE;
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_cnap_name() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_called_party_num_info

===========================================================================*/
/*!
    @brief
    updating the called party number info for a given Call ID is used in GW during CM_CALL_EVENT_SETUP_IND 
    when Called party BCD number info is recieved from the N/W.

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/

IxErrnoType qmi_voice_call_list_update_called_party_num_info
(
  cm_call_id_type call_id,
  cm_called_party_bcd_no_s_type *bcd_num
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  cm_num_s_type num_info;
  
  /*-----------------------------------------------------------------------*/
  buf_ptr = qmi_voice_call_list_find( call_id );
  if ( buf_ptr != NULL )
  {
    if ( bcd_num->present )
    {
      /* Log CList changes */
      qmi_voice_cm_util_get_num_info_from_bcd_num(&num_info,bcd_num);
      QM_MSG_HIGH_4( "Change in number info of CList entry for called party number: call id %d, num len=%d->%d, updated num=%s",
                    call_id, buf_ptr->pub.called_party_num_info.len,num_info.len, num_info.buf);   
      buf_ptr->pub.is_called_party_num_valid = TRUE;
      buf_ptr->pub.called_party_num_info.num_plan = num_info.number_plan;
      buf_ptr->pub.called_party_num_info.num_type = num_info.number_type;
      buf_ptr->pub.called_party_num_info.pi = num_info.pi;
      buf_ptr->pub.called_party_num_info.si = num_info.si;
      buf_ptr->pub.called_party_num_info.len = MIN(num_info.len, CM_MAX_NUMBER_CHARS);
      memscpy(&(buf_ptr->pub.called_party_num_info.num), sizeof(buf_ptr->pub.called_party_num_info.num),
              num_info.buf, buf_ptr->pub.called_party_num_info.len);
    }
    else
    {
      //Called Party BCD number not present
      status = E_FAILURE;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }
  return status;
}/*qmi_voice_call_list_update_called_party_num_info*/


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_redirecting_party_num_info

===========================================================================*/
/*!
    @brief
    updating the Redirecting party number info for a given Call ID is used in GW during CM_CALL_EVENT_SETUP_IND 
    when Redirecting party number info is recieved from the N/W.
    
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/

IxErrnoType qmi_voice_call_list_update_redirecting_party_num_info
(
  cm_call_id_type call_id,
  cm_num_s_type *num
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;
  /*-----------------------------------------------------------------------*/
  buf_ptr = qmi_voice_call_list_find( call_id );
  if ( buf_ptr != NULL )
  {
    if ( num->len > 0)
    {
      /* Log CList changes */
      QM_MSG_HIGH_4( "Change in number info of CList entry : call id %d, num len=%d->%d, num=%s",
                    call_id, buf_ptr->pub.redirecting_party_num_info.len, num->len, num->buf);
      buf_ptr->pub.is_redirecting_party_num_valid = TRUE;
      buf_ptr->pub.redirecting_party_num_info.num_plan = num->number_plan;
      buf_ptr->pub.redirecting_party_num_info.num_type = num->number_type;
      buf_ptr->pub.redirecting_party_num_info.pi = num->pi;
      buf_ptr->pub.redirecting_party_num_info.si = num->si;
      buf_ptr->pub.redirecting_party_num_info.len = MIN(num->len, CM_MAX_NUMBER_CHARS);
      memscpy(&(buf_ptr->pub.redirecting_party_num_info.num), sizeof(buf_ptr->pub.redirecting_party_num_info.num),
              num->buf, buf_ptr->pub.redirecting_party_num_info.len);
    }
    else
    {
      //Redirect Party number is of length Zero
      status = E_FAILURE;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }
  return status;
}/*qmi_voice_call_list_update_redirecting_party_num_info*/

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_call_attributes_type

===========================================================================*/
/*!
    @brief
    Check whether for given Call ID Call attributes and Call type needs update

    @return
    TRUE if the entry was found and local ringback set
    FALSE if the entry was not found or local ringback not set
*/
/*=========================================================================*/
boolean qmi_voice_call_list_update_call_attributes_type
(
  cm_call_id_type call_id,
  cm_ip_call_attrib_info_s_type  call_attrib_info,
  cm_call_type_e_type            call_type
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  IxErrnoType status = E_SUCCESS;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( call_type != buf_ptr->pub.call_type )
    {
      /* Log CList changes */
      QM_MSG_MED_3("Update in call type of CList entry :  call id %d, call type %d -> %d", call_id, buf_ptr->pub.call_type, call_type);

      buf_ptr->pub.call_type = call_type;
    }

    if((call_type == CM_CALL_TYPE_VS) && (call_attrib_info.vs_ver >= CM_IP_VS_RCSE) && (call_attrib_info.vs_ver <= CM_IP_VS_RCSV5) )
    {
      buf_ptr->pub.vs_call_variant = (uint32) qmi_voice_cm_util_map_cm_to_qmi_vs_version (call_attrib_info.vs_ver);
      buf_ptr->pub.is_vs_call_variant_valid = TRUE;
    }

    if ( ((uint32)call_attrib_info.audio_attrib != buf_ptr->pub.audio_attrib) && (call_attrib_info.audio_attrib  <= CM_CALL_ATTRIB_TX_RX) && (call_attrib_info.audio_attrib  >= CM_CALL_ATTRIB_NONE))
    {
      /* Log CList changes */
      QM_MSG_MED_3("Update in audio attrib of CList entry :  call id %d, audio_attrib %d -> %d", call_id, buf_ptr->pub.audio_attrib, call_attrib_info.audio_attrib);

      buf_ptr->pub.audio_attrib = (uint32)call_attrib_info.audio_attrib;
      buf_ptr->pub.is_audio_attrib_valid = TRUE;
    }


    if ( ((uint32)call_attrib_info.video_attrib != buf_ptr->pub.video_attrib) && (call_attrib_info.video_attrib <= CM_CALL_ATTRIB_TX_RX) && (call_attrib_info.video_attrib >= CM_CALL_ATTRIB_NONE))
    {
      /* Log CList changes */
      QM_MSG_MED_3("Update in video attrib of CList entry :  call id %d, video_attrib %d -> %d", call_id, buf_ptr->pub.video_attrib, call_attrib_info.video_attrib);

      buf_ptr->pub.video_attrib = (uint32)call_attrib_info.video_attrib;
      buf_ptr->pub.is_video_attrib_valid = TRUE;
    }
    if(buf_ptr->pub.em_type != call_attrib_info.em_type)
    {
      buf_ptr->pub.em_type = call_attrib_info.em_type;
      QM_MSG_MED_3("Update in emergency_type of CList entry :  call id %d, em_type %d -> %d", call_id, buf_ptr->pub.em_type, call_attrib_info.em_type);
    }
    else
    {
      QM_MSG_MED_1("em_type :%d", call_attrib_info.em_type);
    }

    if ( ((uint32)call_attrib_info.attrib_status != buf_ptr->pub.attrib_status) && 
         (call_attrib_info.attrib_status >= CM_CALL_ATTRIB_STATUS_OK) && 
         (call_attrib_info.attrib_status <= CM_CALL_ATTRIB_STATUS_MEDIA_NOT_READY) )
    {
      /* Log CList changes */
      QM_MSG_MED_3("Update in attrib status of CList entry :  call id %d, attrib_status %d -> %d",
                    call_id, buf_ptr->pub.attrib_status, call_attrib_info.attrib_status);

      buf_ptr->pub.attrib_status = (uint32)call_attrib_info.attrib_status;
      buf_ptr->pub.is_attrib_status_valid = TRUE;
    }
    else
    {
      QM_MSG_MED_2("No update made, received :  call id %d, attrib_status %d",
                    call_id, call_attrib_info.attrib_status);
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

return status;

} /* qmi_voice_call_list_update_call_attributes_type() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_call_capabilities

===========================================================================*/
/*!
    @brief
    Update the local and peer capabilities for the call

    @return
    TRUE if the entry was found 
    FALSE if the entry was not found 
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_call_capabilities
(
  cm_call_id_type call_id,
  cm_ip_call_cap_attrib_info_s_type  local_attr,
  cm_ip_call_cap_attrib_info_s_type  peer_attr
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  IxErrnoType status = E_SUCCESS;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    buf_ptr->pub.is_local_cap_available = TRUE;
    buf_ptr->pub.local_cap.audio_attrib = (uint64) local_attr.audio_attrib;
    buf_ptr->pub.local_cap.audio_restrict_cause = qmi_voice_cm_util_map_cm_to_qmi_restrict_cause(local_attr.audio_restrict_cause);
    buf_ptr->pub.local_cap.video_attrib = (uint64) local_attr.video_attrib;
    buf_ptr->pub.local_cap.video_restrict_cause = qmi_voice_cm_util_map_cm_to_qmi_restrict_cause(local_attr.video_restrict_cause);

    buf_ptr->pub.is_peer_cap_available = TRUE;
    buf_ptr->pub.peer_cap.audio_attrib = (uint64) peer_attr.audio_attrib;
    buf_ptr->pub.peer_cap.audio_restrict_cause = qmi_voice_cm_util_map_cm_to_qmi_restrict_cause(peer_attr.audio_restrict_cause);
    buf_ptr->pub.peer_cap.video_attrib = (uint64) peer_attr.video_attrib;
    buf_ptr->pub.peer_cap.video_restrict_cause = qmi_voice_cm_util_map_cm_to_qmi_restrict_cause(peer_attr.video_restrict_cause);
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;
} /* qmi_voice_call_list_update_call_capabilities() */



/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_call_mode

===========================================================================*/
/*!
    @brief
    Change the Call Mode of the entry in the list with given Call ID 
    Generally used when SRVCC from L->W or G happens.

    @return
    E_SUCCESS if the entry was updated
    E_FAILURE if the entry was not found
    E_ALREADY_DONE if entry was found but no update necessary
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_call_mode
(
  cm_call_id_type call_id,
  sys_sys_mode_e_type  call_mode
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;


  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    QM_MSG_MED_3("Current Mode Type CList entry :  call id %d,Orig call Mode %d -> %d", call_id, buf_ptr->pub.call_mode, call_mode);

    if ( call_mode != buf_ptr->pub.call_mode )
    {
      /* Log CList changes */
      buf_ptr->pub.call_mode = call_mode;
    }
    else
    {
      status = E_ALREADY_DONE;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_call_mode() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_ip_caller_name

===========================================================================*/
/*!
    @brief
    Update the caller name of an IP call
    
    @return
    E_SUCCESS if the entry was updated
    E_FAILURE if the entry was not found
    E_ALREADY_DONE if entry was found but no update necessary
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_ip_caller_name
(
  const cm_mm_call_info_s_type * call_info_ptr
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_info_ptr->call_id );

  if ( buf_ptr != NULL )
  {
    uint8 as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;
    if(buf_ptr->pub.is_as_id_valid)
    {
      as_id = buf_ptr->pub.asubs_id;
    }

    QM_MSG_MED_1("Updating caller name of call %d", call_info_ptr->call_id);

      /* Log CList changes */
      buf_ptr->pub.is_ip_caller_name_available = TRUE;
    if(call_info_ptr->mode_info.info.ip_call.call_attrib_info.caller_name->caller_name_len > 0)
    {
      /*caller_name_len is number of utf16 chars, so number of bytes would be twice that length*/
      memscpy(buf_ptr->pub.ip_caller_name.name,
              sizeof(buf_ptr->pub.ip_caller_name.name),
              call_info_ptr->mode_info.info.ip_call.call_attrib_info.caller_name->caller_name_text,
              (call_info_ptr->mode_info.info.ip_call.call_attrib_info.caller_name->caller_name_len*2));

      buf_ptr->pub.ip_caller_name.name_len = call_info_ptr->mode_info.info.ip_call.call_attrib_info.caller_name->caller_name_len;
    }
    else
    {
        buf_ptr->pub.ip_caller_name.name_len = 0;
    }
    qmi_voice_call_list_report_all_call_status_changed(as_id);
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_ip_caller_name() */



/*===========================================================================

  FUNCTION:  qmi_voice_call_list_is_pure_incom_call_present

===========================================================================*/
/*!
    @brief
    Checks if INCOMING (unanswered) call is present when there is no other active call
 
    @return
    TRUE if the entry was found
    FALSE if the entry was not found
*/
/*=========================================================================*/
boolean qmi_voice_call_list_is_pure_incom_call_present
( 
  uint8  as_id
)
{
  qmi_voice_call_list_call_ids_list_type incom_list;
  boolean is_present = FALSE;

  memset(&incom_list, 0, sizeof(incom_list));
  qmi_voice_call_list_query_voice_call_ids_list_by_state(QMI_VOICE_CALL_LIST_STATE_INCOMING,&incom_list, as_id);
  
  if((incom_list.num_of_call_ids > 0) && !incom_list.answered[0] &&
    (qmi_voice_call_list_count_num_of_voice_calls_by_state((uint16)( QMI_VOICE_CALL_LIST_STATE_ACTIVE), as_id ) == 0) )
  {
    is_present = TRUE;
  }
  return is_present;
} /* qmi_voice_call_list_is_pure_incom_call_present */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_child_number

===========================================================================*/
/*!
    @brief
    Update child number for the call

    @return
    TRUE if the entry was found 
    FALSE if the entry was not found 
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_child_number
(
  cm_call_id_type call_id,
  cm_num_s_type child_num
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  IxErrnoType status = E_SUCCESS;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if(child_num.len > 0)
    {
      buf_ptr->pub.is_child_num_available = TRUE;
      buf_ptr->pub.child_num.len = child_num.len;
      memscpy(buf_ptr->pub.child_num.buf,sizeof(buf_ptr->pub.child_num.buf),child_num.buf,child_num.len);
    }
    else
    {
      buf_ptr->pub.is_child_num_available = FALSE;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;
} /* qmi_voice_call_list_update_child_number() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_set_srvcc_flag

===========================================================================*/
/*!
    @brief
    Set the SRVCC flag of the entry in the list with given Call ID 

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_set_srvcc_flag
(
  cm_call_id_type call_id,
  boolean         is_srvcc_call
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;


  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( is_srvcc_call != buf_ptr->pub.is_srvcc_call )
    {
      /* Log CList changes */
      QM_MSG_HIGH_3("Change in call state of CList entry : call id %d, IS SRVCC %d -> %d", call_id, buf_ptr->pub.is_srvcc_call, is_srvcc_call);

      buf_ptr->pub.is_srvcc_call_available = TRUE;
      buf_ptr->pub.is_srvcc_call = is_srvcc_call;
    }
  }
  else
  {
    /* Call not in the list */
    QM_MSG_ERROR_1("qmi_voice_call_list_set_srvcc_flag(): Call with cm call id %d not in the list", call_id);
    status = E_FAILURE;
  }

  return status;
}/* qmi_voice_call_list_set_srvcc_flag */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_reset_srvcc_available_flags

===========================================================================*/
/*!
    @brief
    Resets the validity of the SRVCC flag of all the calls in the call list 

    @return
   None
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_reset_srvcc_available_flags
(
  void
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  IxErrnoType status = E_SUCCESS;

  //In qmi_voice_call_list_reset_srvcc_available_flags()

  buf_ptr = call_list.head_ptr;

  if(!buf_ptr)
  {
    QM_MSG_ERROR("call_list.head_ptr is NULL.");
    status = E_FAILURE;
  }

  while(buf_ptr)
  {
    /* Reset the is_srvcc_call information. */
    buf_ptr->pub.is_srvcc_call_available = FALSE;
    buf_ptr->pub.is_srvcc_call = FALSE;
    buf_ptr = buf_ptr->next_ptr;
  }

  return status;
}/* qmi_voice_call_list_reset_srvcc_available_flags */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_reset_srvcc_caller_name_flags

===========================================================================*/
/*!
    @brief
    Resets the validity of the SRVCC flag of all the calls in the call list 

    @return
   None
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_reset_srvcc_caller_name_flags
(
  void
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  IxErrnoType status = E_SUCCESS;

  QM_MSG_LOW("In qmi_voice_call_list_reset_srvcc_caller_name_flags().");

  buf_ptr = call_list.head_ptr;

  if(!buf_ptr)
  {
    QM_MSG_ERROR("call_list.head_ptr is NULL.");
    status = E_FAILURE;
  }

  while(buf_ptr)
  {
    /* Reset the is_srvcc_call information. */
    buf_ptr->pub.is_srvcc_caller_name_available = FALSE;
    buf_ptr = buf_ptr->next_ptr;
  }

  return status;
}/* qmi_voice_call_list_reset_srvcc_caller_name_flags */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_reset_parent_id_valid_flags

===========================================================================*/
/*!
    @brief
    Resets the validity of the parent_id flag of all the calls in the call list 

    @return
   None
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_reset_parent_id_valid_flags
(
  void
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  IxErrnoType status = E_SUCCESS;

  //In qmi_voice_call_list_reset_parent_id_valid_flags()

  buf_ptr = call_list.head_ptr;

  if(!buf_ptr)
  {
    QM_MSG_ERROR("call_list.head_ptr is NULL.");
    status = E_FAILURE;
  }

  while(buf_ptr)
  {
    /* Reset the parent_call_id information. */
    buf_ptr->pub.is_parent_call_id_valid= FALSE;
    buf_ptr->pub.parent_call_id = 0;
    buf_ptr->pub.is_parent_call_id_cleared = FALSE;
    buf_ptr = buf_ptr->next_ptr;
  }

  return status;
}/* qmi_voice_call_list_reset_parent_id_valid_flags */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_set_drvcc_flag

===========================================================================*/
/*!
    @brief
    Set the DRVCC flag of the entry in the list with given Call ID 

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_set_drvcc_flag
(
  cm_call_id_type call_id,
  boolean         is_drvcc_call
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;


  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if ( is_drvcc_call != buf_ptr->pub.is_drvcc_call )
    {
      /* Log CList changes */
      QM_MSG_HIGH_3("Change in call state of CList entry : call id %d, IS DRVCC %d -> %d", call_id, buf_ptr->pub.is_drvcc_call, is_drvcc_call);

      buf_ptr->pub.is_drvcc_call_available = TRUE;
      buf_ptr->pub.is_drvcc_call = is_drvcc_call;
    }
  }
  else
  {
    /* Call not in the list */
    QM_MSG_ERROR_1("qmi_voice_call_list_set_drvcc_flag(): Call with cm call id %d not in the list", call_id);
    status = E_FAILURE;
  }

  return status;
}/* qmi_voice_call_list_set_drvcc_flag */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_reset_drvcc_available_flags

===========================================================================*/
/*!
    @brief
    Resets the validity of the DRVCC flag of all the calls in the call list 

    @return
   None
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_reset_drvcc_available_flags
(
  void
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  IxErrnoType status = E_SUCCESS;

  QM_MSG_LOW("In qmi_voice_call_list_reset_drvcc_available_flags().");

  buf_ptr = call_list.head_ptr;

  if(!buf_ptr)
  {
    QM_MSG_ERROR("call_list.head_ptr is NULL.");
    status = E_FAILURE;
  }

  while(buf_ptr)
  {
    /* Reset the is_drvcc_call information. */
    buf_ptr->pub.is_drvcc_call_available = FALSE;
    buf_ptr->pub.is_drvcc_call = FALSE;
    buf_ptr = buf_ptr->next_ptr;
  }

  return status;
}/* qmi_voice_call_list_reset_drvcc_available_flags */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_reset_drvcc_parent_id_valid_flags

===========================================================================*/
/*!
    @brief
    Resets the validity of the drvcc_parent_id flag of all the calls in the call list 

    @return
   None
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_reset_drvcc_parent_id_valid_flags
(
  void
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  IxErrnoType status = E_SUCCESS;

  QM_MSG_LOW("In qmi_voice_call_list_reset_drvcc_parent_id_valid_flags().");

  buf_ptr = call_list.head_ptr;

  if(!buf_ptr)
  {
    QM_MSG_ERROR("call_list.head_ptr is NULL.");
    status = E_FAILURE;
  }

  while(buf_ptr)
  {
    /* Reset the drvcc_parent_call_id information. */
    buf_ptr->pub.is_drvcc_parent_call_id_valid= FALSE;
    buf_ptr->pub.drvcc_parent_call_id = CM_CALL_ID_INVALID;
    buf_ptr->pub.is_drvcc_parent_call_id_cleared = FALSE;
    buf_ptr = buf_ptr->next_ptr;
  }

  return status;
}/* qmi_voice_call_list_reset_drvcc_parent_id_valid_flags */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_additional_call_info

===========================================================================*/
/*!
    @brief
    Update the additional call info for the call if present

    @return
    TRUE if the entry was found 
    FALSE if the entry was not found 
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_additional_call_info
(
  cm_call_id_type          call_id,
  cm_ip_caller_info_s_type *adnl_call_info_ptr,
  boolean                  *is_adnl_info_flag_set
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  IxErrnoType status = E_SUCCESS;

  //calculate the number of indications
  uint32 info_length = (uint32) adnl_call_info_ptr->info_len;
  uint16  num_indications = (int)(info_length/QMI_VOICE_CONF_ADNL_CALL_INFO_MAX_LEN) + 1;


  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if( (adnl_call_info_ptr->info_text!= NULL) && (adnl_call_info_ptr->info_len >0))
    {
      buf_ptr->pub.is_adnl_call_info_available = TRUE;
      *is_adnl_info_flag_set = TRUE;
      buf_ptr->pub.num_indications = num_indications;
      QM_MSG_HIGH_1("Call Info text flag set, num=%d ",num_indications);
    }
    else
    {
      //Info text is NULL 
      buf_ptr->pub.is_adnl_call_info_available = FALSE;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
    QM_MSG_HIGH_1("Call %d not found", call_id);
  }
  
  return status;
} /* qmi_voice_call_list_update_additional_call_info() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_display_text

===========================================================================*/
/*!
    @brief
    Update the display text for the call if present

    @return
    TRUE if the entry was found 
    FALSE if the entry was not found 
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_display_text
(
  cm_call_id_type call_id,
  cm_display_text_s_type display_text
)
{
  qmi_voice_call_list_buf_type *buf_ptr;
  IxErrnoType status = E_SUCCESS;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if(display_text.displayTextLength > 0)
    {
      QM_MSG_HIGH_3("Change in call state of CList entry : call id %d, display_text %d -> %d", call_id, buf_ptr->pub.display_text.text_len, display_text.displayTextLength);
      buf_ptr->pub.is_display_text_available = TRUE;
      buf_ptr->pub.display_text.text_len = display_text.displayTextLength;
      memscpy(buf_ptr->pub.display_text.text,sizeof(buf_ptr->pub.display_text.text),display_text.display_text,display_text.displayTextLength*2);
    }
    else
    {
      //Display text len <= 0
      buf_ptr->pub.is_display_text_available = FALSE;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;
} /* qmi_voice_call_list_update_display_text() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_srvcc_calls_info

===========================================================================*/
/*!
    @brief
    Update the call list with the call data received after SRVCC

    @return
    None
*/
/*=========================================================================*/
boolean qmi_voice_call_list_update_srvcc_calls_info
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
)
{
  boolean success = TRUE;

  if(cm_data == NULL)
  {
    QM_MSG_ERROR("MID-SRVCC: cm_data == NULL ");
    return FALSE;
  }

  //SRVCC: Adding new call instances.
  success = qmi_voice_call_list_update_srvcc_calls_info_add(cm_data, as_id);

  //SRVCC: Replacing existing call instances
  success = qmi_voice_call_list_update_srvcc_calls_info_replace(cm_data, as_id);

  //SRVCC: Deleting freed call instances.
  success = qmi_voice_call_list_update_srvcc_calls_info_delete(cm_data, as_id);

  return success;
}/* qmi_voice_call_list_update_srvcc_calls_info() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_srvcc_calls_info_add

===========================================================================*/
/*!
    @brief
    Update the call list with new calls in the call data received after SRVCC

    @return
    None
*/
/*=========================================================================*/
boolean qmi_voice_call_list_update_srvcc_calls_info_add
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
)
{
  uint8 i;
  boolean success = TRUE;
  qmi_voice_call_list_state_e_type state;
  cm_alpha_s_type  alpha;
  cm_ip_call_attrib_info_s_type   call_attrib;
  cm_mm_srvcc_call_info_s_type *cm_call;

  //SRVCC: Inside qmi_voice_call_list_update_srvcc_calls_info_add()

  memset(&alpha,0,sizeof(alpha));
  memset(&call_attrib,0,sizeof(call_attrib));

  for(i=0;i < MIN(cm_data->number_calls,MAX_CALL_NUM_SRVCC);i++)
  { 
    cm_call = &cm_data->info[i];
    QM_MSG_MED_2("Update srvcc call info add: MID-SRVCC: Call %d, Call ID = %d", i, cm_call->call_id);
    state = qmi_voice_cm_util_map_cm_to_qmi_call_state(cm_call->call_state, cm_call->call_sub_state);
   
    /* when call state is dialing, if early media is present, qmi-voice updates call state to alerting*/
    if(state == QMI_VOICE_CALL_LIST_STATE_DIALING) 
    {
      if(cm_call->alert_media_type == CM_SRVCC_ALERTING_NW) 
      {
        state = QMI_VOICE_CALL_LIST_STATE_ALERTING;
        QM_MSG_MED_1("MID-SRVCC: Call State %d", state); 
      }
    }

    if(qmi_voice_call_list_is_call_present(cm_call->call_id))
    {      
      continue; /*Replacing existing call list entries will be deferred till after adding new entries following CR#608077*/
    }
    else /* new call instance*/
    {
      QM_MSG_MED("SRVCC: New call instance ");   
      if ( qmi_voice_call_list_new( cm_call->call_id, cm_call->call_type,cm_call->sys_mode,
                                    cm_call->direction, state,
                                    CM_ALS_LINE_01, cm_call->num, 
                                    alpha, cm_call->mode_info_type, FALSE,
                                    (uint8)cm_call->asubs_id,
                                    call_attrib) == E_SUCCESS )
      {
        /* Call_id can be know from call_list_new()*/
        /*Call with id( cm_call->call_id) added after SRVCC handover*/

        qmi_voice_call_list_set_mpty_status(cm_call->call_id, cm_call->is_mpty_call);
  
        /* when a new call instance is created after SRVCC, the parent ID should be a conf call */
        if(cm_call->parent_call_id != CM_CALL_ID_INVALID)
        {
          if(qmi_voice_call_list_is_mpty_call(cm_call->parent_call_id) &&
            (cm_call->call_id != cm_call->parent_call_id))
          {
            /* Though parent ID cleared is set to TRUE, actual clearing is happening after call list updation */
            qmi_voice_call_list_update_parent_id(cm_call->call_id,cm_call->parent_call_id,TRUE,TRUE);
          } 
          else
          {
            QM_MSG_ERROR_1("New call list entry added but parent call ID (%d) not updated", cm_call->parent_call_id);
          }
        }
        else
        {
          QM_MSG_MED_1("Parent call ID (%d) is invalid and will not be notified.", cm_call->parent_call_id);
        }
      }
      else
      {
        success = FALSE;
      }

      /*Substates or mapping is applicable only in CONV and ORIG states, rest are already updated */
      if( (cm_call->call_state == CM_CALL_STATE_CONV) || 
          (cm_call->call_state == CM_CALL_STATE_ORIG))
      {
        qmi_voice_call_list_update_call_state(cm_call->call_id,state);
      }
 
      //Only 1 call can have alerting_type set. is_alerting_typ_valid flag is reset in replace and new functions.
      //So updating the alerting type for the call in alerting state only
      if(state == QMI_VOICE_CALL_LIST_STATE_ALERTING)
      {
        qmi_voice_call_list_update_alerting_type(cm_call->call_id,
                                                 qmi_voice_cm_util_map_cm_to_qmi_alerting_type(cm_call->alert_media_type));
      }
      *as_id = (uint8)cm_call->asubs_id;
      qmi_voice_call_list_set_srvcc_flag(cm_call->call_id, TRUE);
    }
  }

  return success;
}/* qmi_voice_call_list_update_srvcc_calls_info_add() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_srvcc_calls_info_replace

===========================================================================*/
/*!
    @brief
    Update the call list with the existing calls in the call data received after SRVCC

    @return
    None
*/
/*=========================================================================*/
boolean qmi_voice_call_list_update_srvcc_calls_info_replace
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
)
{
  uint8 i;
  boolean success = TRUE;
  qmi_voice_call_list_public_type *call_info;
  qmi_voice_call_list_state_e_type state;
  cm_mm_srvcc_call_info_s_type *cm_call;

  //SRVCC: Inside qmi_voice_call_list_update_srvcc_calls_info_replace()

  call_info = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(qmi_voice_call_list_public_type));
  if(NULL == call_info)
  {
	  qmi_voice_mem_fatal();
    return FALSE;
  }

  memset(call_info, 0, sizeof(qmi_voice_call_list_public_type));

  for(i=0;i < MIN(cm_data->number_calls,MAX_CALL_NUM_SRVCC);i++)
  { 
    cm_call = &cm_data->info[i];
    QM_MSG_MED_2("Update srvcc call info replace: MID-SRVCC: Call %d, Call ID = %d", i, cm_call->call_id);
    state = qmi_voice_cm_util_map_cm_to_qmi_call_state(cm_call->call_state, cm_call->call_sub_state);
   
    /* when call state is dialing, if early media is present, qmi-voice updates call state to alerting*/
    if(state == QMI_VOICE_CALL_LIST_STATE_DIALING) 
    {
      if(cm_call->alert_media_type == CM_SRVCC_ALERTING_NW) 
      {
        state = QMI_VOICE_CALL_LIST_STATE_ALERTING;
        QM_MSG_MED_1("MID-SRVCC: Call State %d", state); 
      }
    }

    if(( qmi_voice_call_list_query(cm_call->call_id, call_info) == E_SUCCESS ))
    {
      if((call_info->is_srvcc_call_available) && (call_info->is_srvcc_call))
      {
        continue; /* If the call is already an srvcc call, then it is one which was added in the previous loop. */
      }
      if (qmi_voice_call_list_replace(cm_call->call_id, call_info->conn_index,
                                      cm_call->call_type, cm_call->sys_mode,
                                      cm_call->direction, cm_call->call_state,
                                      call_info->line, cm_call->num, cm_call->mode_info_type, FALSE
                                     ) == E_SUCCESS )
      {
         qmi_voice_call_list_set_mpty_status(cm_call->call_id, cm_call->is_mpty_call);
         qmi_voice_call_list_update_parent_id(cm_call->call_id, cm_call->parent_call_id,FALSE,TRUE);
  #ifdef FEATURE_DUAL_SIM
         qmi_voice_call_list_update_as_id(cm_call->call_id, cm_call->asubs_id);
  #endif
  
         if( (cm_call->parent_call_id != CM_CALL_ID_INVALID) && (cm_call->call_id != cm_call->parent_call_id) )
         {
           /* Since we are replacing an existing call instance, parent ID should ideally be same as the call ID*/
           /* check if there is something wrong in the data received from CM*/
           QM_MSG_ERROR_2("Replace::Parent Call ID not same as call ID:call_id %d, parent=%d", cm_call->call_id, cm_call->parent_call_id);
         }
      }
      else
      {
        success = FALSE;
        QM_MSG_ERROR_1("Fail to replace CList entry for existing call :  call_id %d", cm_call->call_id);
      }
    }
    else /* new call instance*/
    {
      continue; /* Already added new call list entries in previous loop and hence they will already exist; this condition should not be hit. */
    }

    /*Substates or mapping is applicable only in CONV and ORIG states, rest are already updated */
    if( (cm_call->call_state == CM_CALL_STATE_CONV) || 
        (cm_call->call_state == CM_CALL_STATE_ORIG))
    {
      qmi_voice_call_list_update_call_state(cm_call->call_id,state);
    }

    //Only 1 call can have alerting_type set. is_alerting_typ_valid flag is reset in replace and new functions.
    //So updating the alerting type for the call in alerting state only
    if(state == QMI_VOICE_CALL_LIST_STATE_ALERTING)
    {
      qmi_voice_call_list_update_alerting_type(cm_call->call_id,
                                               qmi_voice_cm_util_map_cm_to_qmi_alerting_type(cm_call->alert_media_type));
    }
    *as_id = (uint8)cm_call->asubs_id;
    qmi_voice_call_list_set_srvcc_flag(cm_call->call_id, TRUE);
  }

  if(NULL != call_info)
  {
    QM_MSG_MED("Freeing memory for call_info in srvcc_update_calls");
    QMI_VOICE_CM_IF_MEM_FREE(call_info);
  }

  return success;
}/* qmi_voice_call_list_update_srvcc_calls_info_replace() */


/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_srvcc_calls_info_delete

===========================================================================*/
/*!
    @brief
    Update the call list with the deleted calls in the call data received after SRVCC

    @return
    None
*/
/*=========================================================================*/
boolean qmi_voice_call_list_update_srvcc_calls_info_delete
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
)
{
  uint8 i;
  cm_mm_srvcc_call_info_s_type *cm_call;

  //SRVCC: Inside qmi_voice_call_list_update_srvcc_calls_info_delete()

  for(i=0;i < MIN(cm_data->number_calls,MAX_CALL_NUM_SRVCC);i++)
  { 
    cm_call = &cm_data->info[i];
    if(cm_call->parent_call_id == CM_CALL_ID_INVALID)
    {
      QM_MSG_ERROR_2("parent_call_id %d not valid and not freed for call_id %d", cm_call->parent_call_id, cm_call->call_id);
      continue;
    }
    /* Free a parent call insance if 
           1. It is was present before SRVCC
           2. It is a an MPTY Call (for non-mpty call there should not be a need to free them, they will be replaced
           3. A new call instance would have been created from parent ID*/

    if(qmi_voice_call_list_is_call_present(cm_call->parent_call_id) && 
       qmi_voice_call_list_is_mpty_call(cm_call->parent_call_id) &&
       (cm_call->call_id != cm_call->parent_call_id))
    {
      QM_MSG_MED_1("Update srvcc call info del:MID-SRVCC: Removing call instance = %d", cm_call->parent_call_id);

      /* Remove the record from the CList, but dont report it yet */
      qmi_voice_call_list_free( cm_call->parent_call_id, FALSE );
    }
  }

  return TRUE;
}/* qmi_voice_call_list_update_srvcc_calls_info_delete() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_drvcc_calls_info

===========================================================================*/
/*!
    @brief
    Update the call list with the call data received after DRVCC

    @return
    None
*/
/*=========================================================================*/
boolean qmi_voice_call_list_update_drvcc_calls_info
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
)
{
  boolean success = TRUE;

  if(cm_data == NULL)
  {
    QM_MSG_ERROR("DRVCC: cm_data == NULL ");
    return FALSE;
  }

  //This is needed for 3GPP DRVCC
  //QM_MSG_MED("DRVCC: Adding new call instances. ");
  //success = qmi_voice_call_list_update_drvcc_calls_info_add(cm_data, as_id);

  QM_MSG_MED("DRVCC: Replacing existing call instances. ");
  success = qmi_voice_call_list_update_drvcc_calls_info_replace(cm_data, as_id);

  QM_MSG_MED("DRVCC: Deleting freed call instances. ");
  success = qmi_voice_call_list_update_drvcc_calls_info_delete(cm_data, as_id);

  return success;
}/* qmi_voice_call_list_update_drvcc_calls_info() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_drvcc_calls_info_add

===========================================================================*/
/*!
    @brief
    Update the call list with new calls in the call data received after DRVCC

    @return
    None
*/
/*=========================================================================*/
boolean qmi_voice_call_list_update_drvcc_calls_info_add
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
)
{
  uint8 i;
  boolean success = TRUE;
  qmi_voice_call_list_state_e_type state;
  cm_alpha_s_type  alpha;
  cm_ip_call_attrib_info_s_type   call_attrib;
  cm_mm_srvcc_call_info_s_type *cm_call = NULL;

  QM_MSG_MED("DRVCC: Inside qmi_voice_call_list_update_drvcc_calls_info_add().");

  memset(&alpha,0,sizeof(alpha));
  memset(&call_attrib,0,sizeof(call_attrib));

  for(i=0;i < MIN(cm_data->number_calls,MAX_CALL_NUM_DRVCC);i++)
  { 
    cm_call = &cm_data->info[i];
    QM_MSG_MED_3("DRVCC: Call %d, Call ID = %d, DRVCC Parent Call ID = %d", i, cm_call->call_id, cm_call->drvcc_parent_call_id);
    state = qmi_voice_cm_util_map_cm_to_qmi_call_state(cm_call->call_state, cm_call->call_sub_state);

    if(qmi_voice_call_list_is_call_present(cm_call->drvcc_parent_call_id))
    {      
      continue; /*Replacing existing call list entries will be deferred till after adding new entries */
    }
    else /* new call instance*/
    {
      QM_MSG_MED("DRVCC: New call instance ");   
      if ( qmi_voice_call_list_new( cm_call->call_id, cm_call->call_type,cm_call->sys_mode,
                                    cm_call->direction, state,
                                    CM_ALS_LINE_01, cm_call->num, 
                                    alpha, cm_call->mode_info_type, FALSE,
                                    (uint8)cm_call->asubs_id,
                                    call_attrib) == E_SUCCESS )
      {
        QM_MSG_MED_1("Call with id %d added after DRVCC handover", cm_call->call_id);

        qmi_voice_call_list_set_mpty_status(cm_call->call_id, cm_call->is_mpty_call);
  
        /* when a new call instance is created after DRVCC, the DRVCC parent ID should be a conf call */
        if(cm_call->drvcc_parent_call_id != CM_CALL_ID_INVALID)
        {
          if(cm_call->call_id != cm_call->drvcc_parent_call_id)
          {
            /* Even though DRVCC parent ID cleared is set to TRUE, actual clearing is happening after call list updation */
            qmi_voice_call_list_update_drvcc_parent_id(cm_call->call_id,cm_call->drvcc_parent_call_id,TRUE,TRUE);
          } 
          else
          {
            QM_MSG_ERROR_1("New call list entry added but DRVCC parent call ID (%d) not updated", cm_call->drvcc_parent_call_id);
          }
        }
        else
        {
          QM_MSG_MED_1("DRVCC Parent call ID (%d) is invalid and will not be notified.", cm_call->drvcc_parent_call_id);
        }
      }
      else
      {
        success = FALSE;
      }

      /*Substates or mapping is applicable only in CONV and ORIG states, the rest are already updated */
      if( (cm_call->call_state == CM_CALL_STATE_CONV) || 
          (cm_call->call_state == CM_CALL_STATE_ORIG))
      {
        qmi_voice_call_list_update_call_state(cm_call->call_id,state);
      }
 
      *as_id = (uint8)cm_call->asubs_id;
      qmi_voice_call_list_set_drvcc_flag(cm_call->call_id, TRUE);
    }
  }

  return success;
}/* qmi_voice_call_list_update_drvcc_calls_info_add() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_drvcc_calls_info_replace

===========================================================================*/
/*!
    @brief
    Update the call list with the existing calls in the call data received after DRVCC

    @return
    None
*/
/*=========================================================================*/
boolean qmi_voice_call_list_update_drvcc_calls_info_replace
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
)
{
  uint8 i;
  boolean success = TRUE;
  qmi_voice_call_list_public_type *call_info = NULL;
  qmi_voice_call_list_state_e_type state;
  cm_mm_srvcc_call_info_s_type *cm_call = NULL;

  QM_MSG_MED("DRVCC: Inside qmi_voice_call_list_update_drvcc_calls_info_replace().");

  call_info = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(qmi_voice_call_list_public_type));
  if(NULL == call_info)
  {
    QM_MSG_FATAL("No enough memory");
    return FALSE;
  }

  memset(call_info, 0, sizeof(qmi_voice_call_list_public_type));

  for(i=0;i < MIN(cm_data->number_calls,MAX_CALL_NUM_DRVCC);i++)
  { 
    cm_call = &cm_data->info[i];
    QM_MSG_MED_3("DRVCC: Call %d, Call ID = %d, DRVCC Parent Call ID = %d", i, cm_call->call_id, cm_call->drvcc_parent_call_id);
    state = qmi_voice_cm_util_map_cm_to_qmi_call_state(cm_call->call_state, cm_call->call_sub_state);

    if(( qmi_voice_call_list_query(cm_call->drvcc_parent_call_id, call_info) == E_SUCCESS ))
    {
      //This is needed for 3GPP DRVCC
      //if((call_info->is_drvcc_call_available) && (call_info->is_drvcc_call))
      //{
      //  continue; /* If the call is already an drvcc call, then it is one which was added in the previous loop. */
      //}

      /* For DRVCC, CM will create new call with new CM call_id. We need to update the CM call_id in call_list to reflect this */
      if (qmi_voice_call_list_update_call_id(call_info->conn_index, cm_call->call_id
                                     ) != E_SUCCESS )
      {
        QM_MSG_ERROR_2("Fail to update call_id in CList with conn_idx : conn_idx %d, call_id %d", call_info->conn_index, cm_call->call_id);
      }
                                     
      if (qmi_voice_call_list_replace(cm_call->call_id, call_info->conn_index,
                                      cm_call->call_type, cm_call->sys_mode,
                                      cm_call->direction, cm_call->call_state,
                                      call_info->line, cm_call->num, cm_call->mode_info_type, FALSE
                                     ) == E_SUCCESS )
      {
         //This is needed for 3GPP DRVCC
         //qmi_voice_call_list_set_mpty_status(cm_call->call_id, cm_call->is_mpty_call);
         //qmi_voice_call_list_update_drvcc_parent_id(cm_call->call_id, cm_call->drvcc_parent_call_id,FALSE,TRUE);
  #ifdef FEATURE_DUAL_SIM
         qmi_voice_call_list_update_as_id(cm_call->call_id, cm_call->asubs_id);
  #endif
  
         if( (cm_call->drvcc_parent_call_id != CM_CALL_ID_INVALID) && (cm_call->call_id == cm_call->drvcc_parent_call_id) )
         {
           /* For DRVCC, the drvcc parent call ID should be different from the call ID since it is a new created from CM */
           /* check if there is something wrong in the data received from CM*/
           QM_MSG_ERROR_2("Replace: DRVCC Parent Call ID is the same as call ID:call_id %d, drvcc_parent_call_id=%d", cm_call->call_id, cm_call->drvcc_parent_call_id);
         }
      }
      else
      {
        success = FALSE;
        QM_MSG_ERROR_1("Fail to replace CList entry for existing call :  call_id %d", cm_call->call_id);
      }
    }
    else /* new call instance*/
    {
      continue; /* Already added new call list entries in previous loop and hence they will already exist; this condition should not be hit. */
    }

    /*Substates or mapping is applicable only in CONV and ORIG states, rest are already updated */
    if( (cm_call->call_state == CM_CALL_STATE_CONV) || 
        (cm_call->call_state == CM_CALL_STATE_ORIG))
    {
      qmi_voice_call_list_update_call_state(cm_call->call_id,state);
    }

    *as_id = (uint8)cm_call->asubs_id;
    qmi_voice_call_list_set_drvcc_flag(cm_call->call_id, TRUE);
  }

  if(NULL != call_info)
  {
    QM_MSG_MED("Freeing memory for call_info in drvcc_update_calls");
    QMI_VOICE_CM_IF_MEM_FREE(call_info);
  }

  return success;
}/* qmi_voice_call_list_update_drvcc_calls_info_replace() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_drvcc_calls_info_delete

===========================================================================*/
/*!
    @brief
    Update the call list with the deleted calls in the call data received after DRVCC

    @return
    None
*/
/*=========================================================================*/
boolean qmi_voice_call_list_update_drvcc_calls_info_delete
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
)
{
  uint8 i;
  cm_mm_srvcc_call_info_s_type *cm_call = NULL;

  QM_MSG_MED("DRVCC: Inside qmi_voice_call_list_update_drvcc_calls_info_delete().");

  for(i=0;i < MIN(cm_data->number_calls,MAX_CALL_NUM_DRVCC);i++)
  { 
    cm_call = &cm_data->info[i];
    if(cm_call->drvcc_parent_call_id == CM_CALL_ID_INVALID)
    {
      QM_MSG_ERROR_2("drvcc_parent_call_id %d not valid and not freed for call_id %d", cm_call->drvcc_parent_call_id, cm_call->call_id);
      continue;
    }
    /* Free a DRVCC parent call instance if 
           1. It was presented before DRVCC
           2. It is a an MPTY Call (for non-mpty calls there should not be a need to free them, they will be replaced
           3. A new call instance would have been created from DRVCC parent ID*/

    if(qmi_voice_call_list_is_call_present(cm_call->drvcc_parent_call_id) &&
       (cm_call->call_id != cm_call->drvcc_parent_call_id))
    {
      QM_MSG_MED_1("DRVCC: Removing call instance = %d", cm_call->drvcc_parent_call_id);

      /* Remove the record from the CList, but dont report it yet */
      if ( qmi_voice_call_list_free( cm_call->drvcc_parent_call_id, FALSE ) == E_SUCCESS )
      {
        QM_MSG_ERROR_1("DRVCC: Freed the Call list entry :  call_id %d", cm_call->drvcc_parent_call_id);
      } 
      else
      {
        QM_MSG_ERROR_1("DRVCC: Failed to delete the Call list entry :  call_id %d", cm_call->drvcc_parent_call_id);
      }
    }
  }

  return TRUE;
}/* qmi_voice_call_list_update_drvcc_calls_info_delete() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_get_conf_call_id

===========================================================================*/
/*!
    @brief
   . Finds the call ID with the MPTY bit set to 1.

    @return
    None
*/
/*=========================================================================*/

IxErrnoType qmi_voice_call_list_get_conf_call_id
( 
  cm_call_id_type *call_id
)
{
  
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;


  buf_ptr = call_list.head_ptr;


  while ( buf_ptr != NULL )
  {
    if ( buf_ptr->pub.is_multi_party )
    {             
      break;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  if ( buf_ptr != NULL )
  {
    *call_id = buf_ptr->pub.call_id;
    QM_MSG_MED_1("call id %d which is a conference call ", buf_ptr->pub.call_id);
  }
  else
  {
    /* Call id not in the list */
    status = E_FAILURE;
  }

  return status;

}


#ifdef FEATURE_DUAL_SIM
/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_as_id

===========================================================================*/
/*!
    @brief
    Change the Active subscription ID for the entry in the list with given Call ID

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_as_id
(
  cm_call_id_type call_id,
  uint8           asubs_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  if ( buf_ptr != NULL )
  {
    if(buf_ptr->pub.is_as_id_valid != TRUE)
    {
    
      QM_MSG_MED_2("No Change in call As_id of CList entry :  call id %d, Is_as_id_avail %d", call_id, buf_ptr->pub.is_as_id_valid);
      return E_FAILURE;
    }
    if ( asubs_id != buf_ptr->pub.asubs_id )
    {
      /* Log CList changes */
      QM_MSG_ERROR_3("Changing existing ASubs_ID of CList entry :  call id %d, subs id:  %d -> %d", call_id, buf_ptr->pub.asubs_id, asubs_id);

      buf_ptr->pub.asubs_id = asubs_id;
    }
  }
  else
  {
    /* Call not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_call_list_update_as_id() */

/*===========================================================================

  FUNCTION:  qmi_voice_call_list_get_callid_info

===========================================================================*/
/*!
    @brief
    Find an entry in the list based on Call ID
 
    @return
    A pointer to the entry if found, NULL otherwise.
*/
/*=========================================================================*/
qmi_voice_call_list_buf_type *qmi_voice_call_list_get_callid_info
( 
  cm_call_id_type call_id
)
{
  qmi_voice_call_list_buf_type *buf_ptr = call_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL )
  {
    if ( buf_ptr->pub.call_id == call_id )
    {             
      QM_MSG_MED_1("qmi_voice_call_list_get_callid_info :Found call list entry :  call id %d", call_id);
      return buf_ptr;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }
  return NULL;

} /* qmi_voice_call_list_get_callid_info() */

#endif /* FEATURE_DUAL_SIM*/



/*===========================================================================

  FUNCTION:  qmi_voice_call_list_update_orig_fail_reason

===========================================================================*/
/*!
    @brief
    Update the origination failure reason in the call list record, for the
    call with the specified CM call ID.

    @return
    E_SUCCESS if the entry was updated
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_call_list_update_orig_fail_reason
(
  cm_call_id_type      call_id,
  cm_call_end_e_type   end_status
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_call_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_call_list_find( call_id );

  QM_MSG_MED_2("qmi_voice_call_list_update_orig_fail_reason(): call_id=%d, end_status=%d", call_id, end_status);

  if ( buf_ptr != NULL )
  {
    buf_ptr->pub.is_orig_fail_reason_available = TRUE;
    switch(end_status)
    {
      case CM_CALL_END_SIP_403_FORBIDDEN:
        buf_ptr->pub.orig_fail_reason = QMI_FAILURE_CAUSE_SIP_403_FORBIDDEN;
        break;
      case CM_CALL_END_NETWORK_NO_RESP_TIME_OUT:
        buf_ptr->pub.orig_fail_reason = QMI_FAILURE_CAUSE_NETWORK_NO_RESP_TIME_OUT;
        break;
      case CM_CALL_END_PEER_NOT_REACHABLE:
        buf_ptr->pub.orig_fail_reason = QMI_FAILURE_CAUSE_PEER_NOT_REACHABLE;
        break;
      case CM_CALL_END_SIP_486_BUSY_HERE:
        buf_ptr->pub.orig_fail_reason = QMI_FAILURE_CAUSE_USER_BUSY;
        break;
      case CM_CALL_END_CAUSE_SERVER_INTERNAL_ERROR:
        buf_ptr->pub.orig_fail_reason = QMI_FAILURE_CAUSE_SERVER_INTERNAL_ERROR;
        break;
      case CM_CALL_END_NOT_IMPLEMENTED:
        buf_ptr->pub.orig_fail_reason = QMI_FAILURE_CAUSE_NOT_IMPLEMENTED;
        break;
      case CM_CALL_END_BAD_GATEWAY:
        buf_ptr->pub.orig_fail_reason = QMI_FAILURE_CAUSE_BAD_GATEWAY;
        break;
      case CM_CALL_END_NO_NETWORK_RESP:
        buf_ptr->pub.orig_fail_reason = (qmi_voice_cm_if_failure_cause_e_type)QMI_VOICE_CM_IF_END_NO_NETWORK_RESP;
        break;
      case CM_CALL_END_SERVER_TIME_OUT:
        buf_ptr->pub.orig_fail_reason = QMI_FAILURE_CAUSE_SERVER_TIME_OUT;
        break;
      case CM_CALL_END_VERSION_NOT_SUPPORTED:
        buf_ptr->pub.orig_fail_reason = QMI_FAILURE_CAUSE_VERSION_NOT_SUPPORTED;
        break;
      case CM_CALL_END_MESSAGE_TOO_LARGE:
        buf_ptr->pub.orig_fail_reason = QMI_FAILURE_CAUSE_MESSAGE_TOO_LARGE;
        break;
      case CM_CALL_END_PRECONDITION_FAILURE:
        buf_ptr->pub.orig_fail_reason = QMI_FAILURE_CAUSE_PRECONDITION_FAILURE;
        break;
      default:
        buf_ptr->pub.orig_fail_reason = QMI_FAILURE_CAUSE_UNKNOWN;
        break;
    }
  }
  else
  {
    /* Call not in the list */
    QM_MSG_ERROR("Call not in the list");
    status = E_FAILURE;
  }

  return status;
} /* qmi_voice_call_list_update_orig_fail_reason() */

#endif/* !FEATURE_NO_QMI_VOICE */

#endif // FEATURE_QMI_SERVICE_SPLIT


