/*=============================================================================
  @file sns_rh_mr.c

  This file implements the Message Router module of Request Handler task.

*******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_mr.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-06-21  pn   Cleans up connection entry when disconnected
  2016-04-01  jtl  Fix disconnect callback to fully clean up internal services
  2016-02-11  pn   RH fakes WAKE_SIG when it receives messages from AP
  2015-11-23  pn   Locks connection when updating its busy flag
  2015-05-27  bd   Indication flow control in SMGR
  2015-04-09  pn   Replaced calls to sns_em_get_timestamp() with sns_ddf_get_timestamp()
  2014-12-30  pn   Fixed memory access after free in sns_rh_mr_send_resp()
  2014-12-16  pn   Fixed potential memory corruption in sns_rh_mr_put_msg()
  2014-12-07  pn   Drops requests orphaned by QMI disconnection
  2014-09-16  pn   Consolidated all process_req callbacks into one
  2014-08-13  sc   Fixed compilaton warning (corrected function prototyping)
  2014-05-02  aj   Replaced QMI with SMR
  2014-04-29  pn   Refactored to support microImage
  2014-04-23  pn   Initial version

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_em.h"
#include "sns_memmgr.h"
#include "sns_rh_main.h"
#include "sns_rh_mr.h"
#include "sns_rh_internal_svc.h"
#include "sns_rh_restricted_svc.h"
#include "sns_rh_util.h"
#include "sns_smgr_restricted_api_v01.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
sns_rh_mr_s   sns_rh_mr;
extern sns_rh_mr_connection_s  sns_rh_connection_list[SNS_RH_MR_MAX_CONNECTIONS];


/*----------------------------------------------------------------------------
 * Macro
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*===========================================================================

  FUNCTION:   sns_rh_mr_track_msg_history

===========================================================================*/
SMGR_STATIC void sns_rh_mr_track_msg_history(sns_rh_mr_req_q_item_s* msg_ptr)
{
  uint32_t idx = sns_rh_mr.msg_history.num_rcvd_msgs %
    ARR_SIZE(sns_rh_mr.msg_history.msg_contents);
  sns_rh_mr.msg_history.msg_contents[idx].msg_header = msg_ptr->header;
  memscpy(sns_rh_mr.msg_history.msg_contents[idx].body,
          sizeof(sns_rh_mr.msg_history.msg_contents[idx].body),
          &msg_ptr->request, sizeof(sns_rh_mr.msg_history.msg_contents[idx].body));
  sns_rh_mr.msg_history.num_rcvd_msgs++;
}

/*=============================================================================
  FUNCTION sns_rh_mr_put_msg
=============================================================================*/
/*!
@brief
  This puts the SMGR QMI messages into Queue and posts the signal

@param[in]  connection_handle  connection handle to differntiate the clients

@param[in]  req_handle         handle used for each requests

@param[in]  msg_id             Message ID of the indication

@param[in]  buff_ptr           Buffer holding the data

@param[in]  buff_len           Number of bytes in buffer

@param[in]  svc_num            Service number

@return  Returns QMI_CSI_CB_NO_ERR or QMI_CSI_CB_INTERNAL_ERR or QMI_CSI_CB_NO_MEM

*/
/*=========================================================================*/
SMGR_STATIC smr_cb_err sns_rh_mr_put_msg(
  void*           connection_handle,
  smr_req_handle  req_handle,
  unsigned int    msg_id,
  void*           buff_ptr,
  unsigned int    buff_len,
  uint8_t         svc_num)
{
  sns_rh_mr_req_q_item_s* msg_ptr = NULL;

  if ( !sns_rh_mr_validate_connection(connection_handle) )
  {
    SNS_SMGR_PRINTF4(
      ERROR, "put_msg - dropping request conn=%u req_hndl=0x%x type=%u/=0x%02x",
      connection_handle, req_handle, svc_num, msg_id);
    return SMR_CB_CONN_REFUSED;
  }

  SNS_SMGR_PRINTF4(
    HIGH, "put_msg - conn=%u req_hndl=0x%x type=%u/0x%02x",
    connection_handle, req_handle, svc_num, msg_id);

  msg_ptr = SMGR_ALLOC_STRUCT(sns_rh_mr_req_q_item_s);
  if ( msg_ptr == NULL )
  {
    SNS_SMGR_PRINTF1(ERROR, "put_msg - fail alloc size=%d", buff_len);
    return SMR_CB_NO_MEM;
  }

  msg_ptr->header.timestamp         = sns_ddf_get_timestamp();
  msg_ptr->header.svc_num           = svc_num;
  msg_ptr->header.msg_id            = msg_id;
  msg_ptr->header.body_len          = buff_len;
  msg_ptr->header.connection_handle = connection_handle;
  msg_ptr->header.req_handle        = req_handle;
  memscpy(&msg_ptr->request, sizeof(msg_ptr->request), buff_ptr, buff_len);
  sns_q_link(msg_ptr, &msg_ptr->q_link);

  sns_rh_mr_track_msg_history(msg_ptr);

  sns_os_mutex_pend(sns_rh_mr.mutex_ptr, 0, NULL);
  sns_q_put(&sns_rh_mr.req_queue, &msg_ptr->q_link);
  (void) sns_os_mutex_post(sns_rh_mr.mutex_ptr);

  if ( sns_rh_mr_get_proc_type(connection_handle) == SNS_PROC_APPS_V01 )
  {
    sns_rh_signal_me(SNS_RH_APP_WAKE_SIG); /* fakes an APP_WAKE signal */
  }

  sns_rh_signal_me(SNS_RH_SMR_MSG_SIG);

  return SMR_CB_NO_ERR;
}


/*=========================================================================
  FUNCTION:  sns_rh_mr_get_msg
  =========================================================================*/
sns_rh_mr_req_q_item_s* sns_rh_mr_get_msg(void)
{
  sns_rh_mr_req_q_item_s* msg_ptr = NULL;

  sns_os_mutex_pend(sns_rh_mr.mutex_ptr, 0, NULL);
  msg_ptr = (sns_rh_mr_req_q_item_s*) sns_q_get(&sns_rh_mr.req_queue);
  (void) sns_os_mutex_post(sns_rh_mr.mutex_ptr);

  return msg_ptr;
}

/*=========================================================================
  CALLBACK FUNCTION:  sns_rh_mr_smr_connect_cb
  =========================================================================*/
/*!
  @brief Callback registered with SMR to receive connect requests
*/
/*=======================================================================*/
SMGR_STATIC smr_cb_err sns_rh_mr_smr_connect_cb(
  smr_qmi_client_handle client_handle,
  void*                 service_cookie,
  void**                connection_handle)
{
  smr_cb_err smr_err = SMR_CB_CONN_REFUSED;

  /* Assign client_handle pointer to connection_handle so that it can be
     used in the handle_req_cb to send indications if necessary */
  if( connection_handle != NULL )
  {
    uint32_t i;
    for ( i=0; i<ARR_SIZE(sns_rh_connection_list); i++ )
    {
      if ( !sns_rh_connection_list[i].is_valid &&
           (sns_rh_connection_list[i].client_handle == NULL) )
      {
        sns_rh_connection_list[i].is_valid = true;
        sns_rh_connection_list[i].client_handle = client_handle;
        sns_rh_connection_list[i].service_cookie = service_cookie ?
            *(uint32_t*)service_cookie : -1;
        sns_rh_connection_list[i].is_busy = false;
        *connection_handle = (void*)i;
        smr_err = SMR_CB_NO_ERR;
        SNS_SMGR_PRINTF3(
          HIGH, "connect_cb - conn=%u cli_hndl=0x%x cookie=%d",
          *connection_handle, client_handle,
          service_cookie ? *(uint32_t*)service_cookie : -1 );
        break;
      }
    }
  }
  if ( smr_err == SMR_CB_CONN_REFUSED )
  {
    SNS_SMGR_PRINTF1(
      ERROR, "connect_cb - cli_hndl 0x%x refused", (unsigned)client_handle);
  }
  return smr_err;
}

/*=========================================================================
  CALLBACK FUNCTION:  sns_rh_mr_smr_disconnect_cb
  =========================================================================*/
/*!
  @brief Callback registered with SMR to receive disconnect requests
*/
/*=======================================================================*/
SMGR_STATIC void sns_rh_mr_smr_disconnect_cb(
  void* connection_handle,
  void* service_cookie)
{
  uint32_t i = (uint32_t) connection_handle;
  SNS_SMGR_PRINTF2(
    HIGH, "disconnect_cb - conn=%u cookie=%u",
    connection_handle, service_cookie ? *(uint32_t*)service_cookie : -1);

  if ( i < ARR_SIZE(sns_rh_connection_list) )
  {
    if ( sns_rh_connection_list[i].is_valid )
    {
      sns_rh_connection_list[i].is_valid = false;
      sns_rh_signal_me(SNS_RH_SMR_DISC_SIG);
    }
    else
    {
      SNS_SMGR_PRINTF1(
        ERROR, "disconnect_cb - invalid conn=%u", connection_handle);
    }
  }
}

/*=========================================================================
  CALLBACK FUNCTION:  sns_rh_mr_smr_resume_cb
  =========================================================================*/
/*!
  @brief Callback registered with SMR to receive resume requests
*/
/*=======================================================================*/
SMGR_STATIC void sns_rh_mr_smr_resume_cb(
  void* connection_handle,
  void* service_cookie)
{
  uint32_t i = (uint32_t)connection_handle;

  SNS_SMGR_PRINTF1(HIGH, "resume_cb - conn=0x%x", connection_handle);

  if ( i < ARR_SIZE(sns_rh_connection_list) )
  {
    sns_rh_mr_update_connection_state(&sns_rh_connection_list[i], false);
  }
  sns_rh_signal_me(SNS_RH_SMR_RESUME_SIG);
}


/*=========================================================================
  CALLBACK FUNCTION:  sns_rh_mr_smr_process_req_cb
  =========================================================================*/
/*!
  @brief Callback registered with SMR to receive service requests
*/
/*=======================================================================*/
SMGR_STATIC smr_cb_err sns_rh_mr_smr_process_req_cb(
  void*          connection_handle,
  smr_req_handle req_handle,
  unsigned int   msg_id,
  void*          req_c_struct,
  unsigned int   req_c_struct_len,
  void*          service_cookie)
{
  if ( service_cookie != NULL )
  {
    return sns_rh_mr_put_msg(connection_handle,
                             req_handle,
                             msg_id,
                             req_c_struct,
                             req_c_struct_len,
                             *(uint32_t*)service_cookie);
  }
  else
  {
    return SMR_CB_INTERNAL_ERR;
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_mr_smr_external_service_register

===========================================================================*/
SMGR_STATIC void sns_rh_mr_smr_external_service_register(void)
{
  smr_err smr_err;

  sns_rh_mr.service_cookie_ext = SNS_SMGR_SVC_ID_V01;
  SNS_SMGR_PRINTF1(HIGH, "SNS_SMGR_SVC_get_service_object_v01 = %s", SNS_SMGR_SVC_get_service_object_v01());
  smr_err = smr_service_register(SNS_SMGR_SVC_get_service_object_v01(),
                                 SNS_SMR_SVC_PRI_MED,
                                 sns_rh_mr_smr_connect_cb,
                                 sns_rh_mr_smr_disconnect_cb,
                                 sns_rh_mr_smr_resume_cb,
                                 sns_rh_mr_smr_process_req_cb,
                                 &sns_rh_mr.service_cookie_ext,
                                 &sns_rh_mr.service_handle_ext);
  SNS_SMGR_PRINTF1(HIGH, "ext_svc_reg - result=%u", smr_err);
}

/*===========================================================================

  FUNCTION:   sns_rh_mr_smr_internal_service_register

===========================================================================*/
SMGR_STATIC void sns_rh_mr_smr_internal_service_register(void)
{
  smr_err err;

  sns_rh_mr.service_cookie_int = SNS_SMGR_INTERNAL_SVC_ID_V01;
  err = smr_service_register(SNS_SMGR_INTERNAL_SVC_get_service_object_v02(),
                             SNS_SMR_SVC_PRI_MED,
                             sns_rh_mr_smr_connect_cb,
                             sns_rh_mr_smr_disconnect_cb,
                             sns_rh_mr_smr_resume_cb,
                             sns_rh_mr_smr_process_req_cb,
                             &sns_rh_mr.service_cookie_int,
                             &sns_rh_mr.service_handle_int);
  SNS_SMGR_PRINTF1(HIGH, "int_svc_reg - result=%u", err);
}

/*===========================================================================

  FUNCTION:   sns_rh_mr_smr_restricted_service_register

===========================================================================*/
SMGR_STATIC void sns_rh_mr_smr_restricted_service_register(void)
{
  smr_err err;

  sns_rh_mr.service_cookie_restricted = SNS_SMGR_RESTRICTED_SVC_ID_V01;
  err = smr_service_register(SNS_SMGR_RESTRICTED_SVC_get_service_object_v01(),
                             SNS_SMR_SVC_PRI_MED,
                             sns_rh_mr_smr_connect_cb,
                             sns_rh_mr_smr_disconnect_cb,
                             sns_rh_mr_smr_resume_cb,
                             sns_rh_mr_smr_process_req_cb,
                             &sns_rh_mr.service_cookie_restricted,
                             &sns_rh_mr.service_handle_restricted);
  SNS_SMGR_PRINTF1(HIGH, "restricted_svc_reg - result=%u", err);
}

/*===========================================================================

  FUNCTION:   sns_rh_mr_send_resp

===========================================================================*/
/*!
  @brief This function fills response message header info then sends the message.

  @detail

  @param[i]  msg_header - response message header
  @param[i]  resp_ptr   - response message body

  @return  sns_err_code_e
 */
/*=========================================================================*/
sns_err_code_e sns_rh_mr_send_resp(
  const sns_rh_mr_header_s* msg_header,
  void*                     resp_ptr)
{
  smr_req_handle req_handle = msg_header->req_handle;
  smr_err err =
    smr_service_send_resp(msg_header->req_handle,
                          msg_header->msg_id,
                          resp_ptr,
                          msg_header->body_len);
  SNS_SMGR_PRINTF4(
    HIGH, "send_resp - type=%u/0x%02x req_hndl=0x%x err=%d",
    msg_header->svc_num, msg_header->msg_id, req_handle, err);
  SNS_OS_FREE(resp_ptr);
  return (err == SMR_NO_ERR) ? SNS_SUCCESS : SNS_ERR_FAILED;
}

/*===========================================================================

  FUNCTION:   sns_smgr_send_resp_w_err

===========================================================================*/
/*!
  @brief This function sends error response due to unavailable memory resource.

  @detail

  @param[i]   msg_ptr - the message
  @return  none
 */
/*=========================================================================*/
void sns_rh_mr_send_resp_w_err(sns_rh_mr_req_q_item_s* item_ptr)
{
  SNS_SMGR_PRINTF1(
    ERROR, "send_resp_w_err - dropping msg_id=0x%x", item_ptr->header.msg_id);
  SNS_OS_FREE(item_ptr);
}

/*===========================================================================

  FUNCTION:   sns_rh_mr_init

===========================================================================*/
/*!
  @brief Message Router init

  @param    none
  @return   none
 */
/*=========================================================================*/
void sns_rh_mr_init(void)
{
  uint8_t err;
  uint32_t i;

  sns_rh_mr.mutex_ptr = sns_os_mutex_create(SNS_SMGR_QUE_MUTEX, &err);
  SNS_ASSERT (err == OS_ERR_NONE );

  for ( i=0; i<ARR_SIZE(sns_rh_connection_list); i++ )
  {
    sns_rh_connection_list[i].proc_type          = SNS_PROC_TYPE_E_MAX_ENUM_VAL_V01;
    sns_rh_connection_list[i].service_cookie     = -1;
  }

  sns_q_init(&sns_rh_mr.req_queue);

  sns_rh_mr_smr_external_service_register();
  sns_rh_mr_smr_internal_service_register();
  sns_rh_mr_smr_restricted_service_register();
}

/*===========================================================================

  FUNCTION:   sns_rh_mr_handle_disconnect_sig

===========================================================================*/
/*!
  @brief Handles SNS_RH_QMI_DISC_SIG signal

  @detail
  @param  none
  @return none
 */
/*=========================================================================*/
void sns_rh_mr_handle_disconnect_sig(void)
{
  uint32_t i;

  for ( i=0; i<ARR_SIZE(sns_rh_connection_list); i++ )
  {
    if ( (sns_rh_connection_list[i].client_handle != NULL) &&
         !sns_rh_connection_list[i].is_valid )
    {
      SNS_SMGR_PRINTF3( HIGH, "disconnect_sig - conn=%u cli_hndl=0x%x serv=%u",
        i,
        sns_rh_connection_list[i].client_handle,
        sns_rh_connection_list[i].service_cookie);
      if (sns_rh_connection_list[i].service_cookie == sns_rh_mr.service_cookie_ext)
      {
        sns_rh_cancel_service((void*)i);
      }
      else if (sns_rh_connection_list[i].service_cookie == sns_rh_mr.service_cookie_restricted)
      {
        sns_rh_cancel_restricted_service((void*)i);
      }
      else if (sns_rh_connection_list[i].service_cookie == sns_rh_mr.service_cookie_int)
      {
        sns_rh_cancel_internal_service((void*)i);
        sns_rh_cancel_service((void*)i);
      }
      else
      {
        SNS_SMGR_PRINTF1( ERROR, "disconnect_sig - unrecognized service %d",
          sns_rh_connection_list[i].service_cookie);
      }
      sns_rh_connection_list[i].is_busy            = false;
      sns_rh_connection_list[i].proc_type_is_valid = false;
      sns_rh_connection_list[i].proc_type          = SNS_PROC_TYPE_E_MAX_ENUM_VAL_V01;
      sns_rh_connection_list[i].service_cookie     = -1;
      sns_rh_connection_list[i].client_handle      = NULL;
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_mr_validate_connection

===========================================================================*/
/*!
  @brief Checks if the connection associated with given handle is still valid

  @param [i]  conn_handle

  @return true if connection is valid
 */
/*=========================================================================*/
bool sns_rh_mr_validate_connection(const void* conn_handle)
{
  uint32_t idx = (uint32_t)conn_handle;
  return (idx < ARR_SIZE(sns_rh_connection_list) &&
          sns_rh_connection_list[idx].is_valid   &&
          sns_rh_connection_list[idx].client_handle != NULL);
}

/*===========================================================================

  FUNCTION:   sns_rh_mr_update_connection_state

===========================================================================*/
/*!
  @brief Updates the busy state of the given connection

  @param [io] conn_ptr - the connection to update
  @param [i]  busy     - true if the connection became busy

  @return None
 */
/*=========================================================================*/
void sns_rh_mr_update_connection_state(sns_rh_mr_connection_s* conn_ptr, bool busy)
{
  sns_os_mutex_pend(sns_rh_mr.mutex_ptr, 0, NULL);
  conn_ptr->is_busy = busy;
  sns_os_mutex_post(sns_rh_mr.mutex_ptr);
}

/*===========================================================================

  FUNCTION:   sns_rh_mr_update_connection

===========================================================================*/
/*!
  @brief Updates the processor type of the connection

  @param [i]  conn_handle
  @param [i]  proc_type

  @return none
 */
/*=========================================================================*/
void sns_rh_mr_update_connection(
  const void*               conn_handle,
  sns_proc_type_e_v01       proc_type)
{
  uint32_t idx = (uint32_t)conn_handle;
  if ( idx < ARR_SIZE(sns_rh_connection_list) &&
       (!sns_rh_connection_list[idx].proc_type_is_valid ||
        proc_type != sns_rh_connection_list[idx].proc_type) )
  {
    SNS_SMGR_PRINTF3(
      HIGH, "update_connection - conn=%u proc_type(was/now)=%u/%u",
      idx, sns_rh_connection_list[idx].proc_type, proc_type);
    sns_rh_connection_list[idx].proc_type = proc_type;
    sns_rh_connection_list[idx].proc_type_is_valid = true;
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_mr_get_proc_type

===========================================================================*/
/*!
  @brief Gets the processor type of the connection

  @param [i]  conn_handle

  @return processor type if valid, else SNS_PROC_TYPE_E_MAX_ENUM_VAL_V01
 */
/*=========================================================================*/
sns_proc_type_e_v01 sns_rh_mr_get_proc_type(const void* conn_handle)
{
  sns_proc_type_e_v01 proc_type = SNS_PROC_TYPE_E_MAX_ENUM_VAL_V01;
  uint32_t idx = (uint32_t)conn_handle;
  if ( idx < ARR_SIZE(sns_rh_connection_list) &&
       sns_rh_connection_list[idx].proc_type_is_valid )
  {
    proc_type = sns_rh_connection_list[idx].proc_type;
  }
  return proc_type;
}
