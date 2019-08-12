/*=============================================================================
  @file sns_smgr_mr.c

  This file implements the Message Router module of SMGR.

*******************************************************************************
* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_mr.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-04-09  pn   Replaced calls to sns_em_get_timestamp() with sns_ddf_get_timestamp()
  2014-08-13  sc   Fixed compilation warning (removed unused variables)
  2014-07-31  pn   Removed PM task interface code
  2014-04-23  pn   Initial version

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sensor1.h"
#include "sns_em.h"
#include "sns_osa.h"
#include "sns_memmgr.h"

#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_idl_lib_internal.h"

#include "sns_reg_api_v02.h"
#include "sns_smgr_mr.h"
#include "sns_smgr_reg.h"
#include "sns_smgr_priv.h"
#include "sns_smgr_main.h"

typedef struct
{
  qmi_client_os_params        os_params;
  qmi_client_type             user_handle;
  qmi_client_type             notifier_handle;
  uint8_t                     svc_num;
} smgr_mr_qmi_client_if_s;

typedef struct  
{
  /* QMI client interface */
  smgr_mr_qmi_client_if_s     qmi_cl_reg;

  sns_q_s                     msg_queue;
  OS_EVENT*                   msg_queue_mutex_ptr;

  sns_q_s                     req_queue;
  OS_EVENT*                   req_queue_mutex_ptr;

} sns_smgr_mr_s;

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
sns_smgr_mr_s   sns_smgr_mr;

/*----------------------------------------------------------------------------
 * Macro
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*=============================================================================
  FUNCTION sns_smgr_mr_put_msg
=============================================================================*/
/*!
@brief
  This puts the SMGR QMI messages into Queue and posts the signal

@param[in]  msg_id          Message ID of the indication

@param[in]  buff_ptr        Buffer holding the data

@param[in]  qmi_msg_type    QMI message type

@param[in]  smgr_msg_type   SMGR message type

@param[in]  svc_num         Service number

@return  None

*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_mr_put_msg(
  unsigned int                 msg_id,
  void*                        buff_ptr,
  unsigned int                 buff_len,
  qmi_idl_type_of_message_type qmi_msg_type,
  uint8_t                      svc_num)
{
  sns_smgr_mr_msg_s* msg_ptr;
  uint8_t            os_err;
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF2(
    HIGH, "put_msg - svc/msg=0x%04x type=%d", (svc_num << 8) | msg_id, qmi_msg_type);

  msg_ptr = SMGR_ALLOC_STRUCT(sns_smgr_mr_msg_s);
  if ( msg_ptr != NULL )
  {
    msg_ptr->header.timestamp = sns_ddf_get_timestamp();
    msg_ptr->header.msg_type  = qmi_msg_type;
    msg_ptr->header.svc_num   = svc_num;
    msg_ptr->header.msg_id    = msg_id;
    msg_ptr->header.body_len  = buff_len;
    msg_ptr->body_ptr         = buff_ptr;
    sns_q_link(msg_ptr, &msg_ptr->q_link);

    sns_os_mutex_pend(sns_smgr_mr.msg_queue_mutex_ptr, 0, &os_err);
    sns_q_put(&sns_smgr_mr.msg_queue, &msg_ptr->q_link);
    os_err = sns_os_mutex_post(sns_smgr_mr.msg_queue_mutex_ptr);

    sns_smgr_signal_me(SNS_SMGR_QMI_MSG_SIG);
  }
  else
  {
    SNS_OS_FREE(buff_ptr);
    SNS_SMGR_PRINTF1(ERROR, "put_msg - fail alloc size=%d", buff_len);
  }
}

/*=========================================================================
  CALLBACK FUNCTION:  smgr_mr_qmi_client_resp_cb
  =========================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives an asynchronous response for this client

@param[in]   user_handle         Opaque handle used by the infrastructure to
                 identify different services.

@param[in]   msg_id              Message ID of the response

@param[in]   buf                 Buffer holding the decoded response

@param[in]   len                 Length of the decoded response

@param[in]   resp_cb_data        Cookie value supplied by the client

@param[in]   transp_err          Error value

*/
/*=======================================================================*/
SMGR_STATIC void smgr_mr_qmi_client_resp_cb(
  qmi_client_type         user_handle,
  unsigned int            msg_id,
  void*                   resp_c_struct,
  unsigned int            resp_c_struct_len,
  void*                   resp_cb_data,
  qmi_client_error_type   transp_err)
{
  if ( (transp_err != QMI_NO_ERR) || (resp_c_struct == NULL) )
  {
    SNS_SMGR_PRINTF2(
      ERROR, "client_resp_cb - transp_err=%d, msg_id=%d", transp_err, msg_id);
    SNS_OS_FREE(resp_cb_data);
    SNS_OS_FREE(resp_c_struct);
  }
  else
  {
    if( resp_cb_data )
    {
      sns_smgr_save_last_rcvd_reg_group_id(*(uint16_t*)resp_cb_data);
      SNS_OS_FREE(resp_cb_data);
    }
    sns_smgr_mr_put_msg(
      msg_id, resp_c_struct, resp_c_struct_len, QMI_IDL_RESPONSE, 
      sns_smgr_mr.qmi_cl_reg.svc_num);
  }
}

#if !defined(ADSP_STANDALONE) && !defined(SNS_QDSP_SIM)
/*=============================================================================
  CALLBACK FUNCTION sns_smgr_client_ind_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives an indication for this client

@param[in]   user_handle         Opaque handle used by the infrastructure to
                 identify different services.

@param[in]   msg_id              Message ID of the indication

@param[in]  ind_buf              Buffer holding the encoded indication

@param[in]  ind_buf_len          Length of the encoded indication

@param[in]  ind_cb_data          service ID supplied during registration

*/
/*=========================================================================*/
SMGR_STATIC void smgr_mr_qmi_client_ind_cb(
  qmi_client_type user_handle,
  unsigned int    msg_id,
  void*           ind_buf,
  unsigned int    ind_buf_len,
  void*           ind_cb_data)
{
  smgr_mr_qmi_client_if_s* cl_if_ptr = (smgr_mr_qmi_client_if_s*) ind_cb_data;
  /* ---------------------------------------------------------------------- */

  if ( user_handle == cl_if_ptr->user_handle )
  {
    void* ind_buf_ptr = NULL;
    if ( ind_buf_len > 0 )
    {
      ind_buf_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, ind_buf_len);
      if ( ind_buf_ptr != NULL )
      {
        SNS_OS_MEMCOPY(ind_buf_ptr, ind_buf, ind_buf_len);
      }
    }
    if ( ind_buf_len == 0 || ind_buf_ptr != NULL )
    {
      sns_smgr_mr_put_msg(
        msg_id, ind_buf_ptr, ind_buf_len, QMI_IDL_INDICATION, cl_if_ptr->svc_num);
    }
  }
  else
  {
    SNS_SMGR_PRINTF2(
      ERROR, "client_ind_cb - mismatched handle 0x%x v.s. 0x%x", 
      (unsigned)user_handle, (unsigned)cl_if_ptr->user_handle);
  }
}

/*===========================================================================

  FUNCTION:   smgr_mr_qmi_client_connect_service

===========================================================================*/
SMGR_STATIC void smgr_mr_qmi_client_connect_service(
  qmi_idl_service_object_type svc_obj,
  smgr_mr_qmi_client_if_s*    qmi_cl_ptr)
{
  qmi_client_error_type err;
  qmi_service_info service_info;
  unsigned int num_entries = 1;
  unsigned int num_services;
  /* ---------------------------------------------------------------------- */

  if ( (qmi_cl_ptr == NULL) || (qmi_cl_ptr->notifier_handle == 0) )
  {
    SNS_SMGR_PRINTF2(
      MED, "client_connect_service - cl_ptr(0x%x) notifier_handle(0x%x)",
      (unsigned)qmi_cl_ptr, qmi_cl_ptr ? (unsigned)qmi_cl_ptr->notifier_handle : 0);
    return;
  }

  memset(&(qmi_cl_ptr->os_params),0,sizeof(qmi_cl_ptr->os_params));

  err = qmi_client_get_service_list(
          svc_obj, &service_info, &num_entries, &num_services);

  SNS_SMGR_PRINTF3(
    MED, "client_connect_service - get_service_list(%d) result(%d) #svcs(%d)", 
    qmi_cl_ptr->svc_num, err, num_services);

  if ( err == QMI_NO_ERR )
  {
     err = qmi_client_init(&service_info, 
                           svc_obj, 
                           smgr_mr_qmi_client_ind_cb, qmi_cl_ptr, 
                           &qmi_cl_ptr->os_params,
                           &qmi_cl_ptr->user_handle);
     SNS_SMGR_PRINTF3(
       MED, "client_connect_service - qmi_client_init(%d) result(%d) hndl(%x)", 
       qmi_cl_ptr->svc_num, err, (unsigned)qmi_cl_ptr->user_handle);
  }
}
#endif

/*===========================================================================

  FUNCTION:   smgr_mr_qmi_client_notifier_cb

===========================================================================*/
SMGR_STATIC void smgr_mr_qmi_client_notifier_cb(
  qmi_client_type               user_handle,
  qmi_idl_service_object_type   service_obj,
  qmi_client_notify_event_type  service_event,
  void*                         notify_cb_data)
{
  smgr_mr_qmi_client_if_s* qmi_cl_ptr = (smgr_mr_qmi_client_if_s*)notify_cb_data;
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF3(
    HIGH, "client_notifier_cb(0x%x) - svc_id=%u ev=%d", 
    (unsigned)user_handle, (unsigned)service_obj->service_id, service_event);

  if ( service_event == QMI_CLIENT_SERVICE_COUNT_INC )
  {
    sns_smgr_signal_me(SNS_SMGR_QMI_REG_CL_TIMER_SIG);
  }
  else
  {
    SNS_SMGR_PRINTF1(FATAL, "Service %d disconnected", qmi_cl_ptr->svc_num);
  }
}

/*===========================================================================

  FUNCTION:   smgr_mr_qmi_client_init

===========================================================================*/
SMGR_STATIC void smgr_mr_qmi_client_init(
   OS_FLAG_GRP*                sigs_grp_ptr,
   uint8_t                     svc_num,
   uint32_t                    msg_sig_bit,
   uint32_t                    timer_sig_bit,
   qmi_idl_service_object_type svc_obj,
   smgr_mr_qmi_client_if_s*    qmi_cl_ptr)
{
  qmi_client_error_type err;
  /* ---------------------------------------------------------------------- */

  sns_os_sigs_add(sigs_grp_ptr, msg_sig_bit);
  memset(&(qmi_cl_ptr->os_params),0,sizeof(qmi_cl_ptr->os_params));
  qmi_cl_ptr->os_params.ext_signal = NULL;
  qmi_cl_ptr->os_params.sig        = msg_sig_bit;
  qmi_cl_ptr->os_params.timer_sig  = timer_sig_bit;
  qmi_cl_ptr->svc_num = svc_num;
  err = qmi_client_notifier_init(svc_obj,
                                 &qmi_cl_ptr->os_params,
                                 &qmi_cl_ptr->notifier_handle);
  SNS_SMGR_PRINTF3(
    MED, "qmi_client_notifier_init(%d) result(%d) hndl(%x)", 
    svc_num, err, (unsigned)qmi_cl_ptr->notifier_handle);

  err = qmi_client_register_notify_cb(qmi_cl_ptr->notifier_handle,
                                      smgr_mr_qmi_client_notifier_cb,
                                      qmi_cl_ptr);
  SNS_SMGR_PRINTF2(
    HIGH, "qmi_client_register_notify_cb(%d) - err=%d", svc_num, err);
}

/*===========================================================================

  FUNCTION:   smgr_mr_qmi_client_wait_for_service

===========================================================================*/
/*!
  @brief Returns only after PM and REG2 services become available

  @param [i]  sigs_grp_ptr
  @param [i]  sig_mask
 
  @return   none
 */
/*=========================================================================*/
SMGR_STATIC void smgr_mr_qmi_client_wait_for_service(
  OS_FLAG_GRP* sigs_grp_ptr,
  OS_FLAGS     sig_mask)
{
#if !defined(ADSP_STANDALONE) && !defined(SNS_QDSP_SIM)
  OS_FLAGS sig_flags = 0;
  uint8_t err;
  sns_em_timer_obj_t           init_tmr_obj;
  uint32_t timer_offset_us = 10000000;  /* 10 sec time out  */
  uint32_t tick_offset;               /* time out in ticks */
  sns_err_code_e result;
  
  /* ---------------------------------------------------------------------- */
  
  result = sns_em_create_utimer_obj(&sns_smgr_init_timer_cb, &sns_smgr.sig_grp, 
                          SNS_EM_TIMER_TYPE_ONESHOT, &init_tmr_obj);
  if (SNS_SUCCESS != result)
	{
		SNS_SMGR_PRINTF1(ERROR, "Init Timer Creation Failed with result = %d ", result);
	}
	
  sig_mask |= SNS_SMGR_INIT_TIMER_SIG;

  sns_os_sigs_add(sigs_grp_ptr, sig_mask);
  
  tick_offset = sns_em_convert_usec_to_dspstick(timer_offset_us);
  
  if ( SNS_ERR_FAILED ==
           sns_em_register_utimer(init_tmr_obj, tick_offset) )
      {
        SNS_SMGR_PRINTF0(ERROR, "Initialization Timer - register_timer failed");
      }
  while ( sig_mask != 0 )
  {
    SNS_SMGR_PRINTF1(MED, "wait4service - sig_mask=0x%08x ...", sig_mask);
    sig_flags = sns_os_sigs_pend(sigs_grp_ptr, sig_mask,
                                 OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);
    SNS_SMGR_PRINTF1(LOW, "wait4service - sig_flags=0x%x", sig_flags);
    sns_os_sigs_del(sigs_grp_ptr, sig_flags);
    if ( sig_flags & SNS_SMGR_QMI_REG_CL_TIMER_SIG )
    {
      smgr_mr_qmi_client_connect_service(
        SNS_REG2_SVC_get_service_object_v02(), &sns_smgr_mr.qmi_cl_reg);
		
	  sns_em_cancel_utimer(init_tmr_obj);
	  sig_mask &= (~SNS_SMGR_INIT_TIMER_SIG);
	  
    }
	else if( sig_flags & SNS_SMGR_INIT_TIMER_SIG )
	{
	  sig_mask &= (~SNS_SMGR_QMI_REG_CL_TIMER_SIG);
	  sns_smgr.all_init_state = SENSOR_ALL_INIT_DONE;
	}
    sig_mask &= ~sig_flags;
  }
  sns_em_delete_utimer_obj(init_tmr_obj);
#endif
}

/*===========================================================================

  FUNCTION:   sns_smgr_mr_init

===========================================================================*/
/*!
  @brief SMGR Message Router module initialization
 
  @details Only returns after successfully connected to server
 
  @param [i] sigs_grp_ptr
 
  @return   none
 */
/*=========================================================================*/
void sns_smgr_mr_init(OS_FLAG_GRP* sigs_grp_ptr)
{
  OS_FLAGS sig_mask = 0;
  uint8_t err;
  /* ---------------------------------------------------------------------- */

  sns_q_init(&sns_smgr_mr.msg_queue);
  sns_smgr_mr.req_queue_mutex_ptr = sns_os_mutex_create(SNS_SMGR_QUE_MUTEX, &err);
  SNS_ASSERT (err == OS_ERR_NONE );

  sns_q_init(&sns_smgr_mr.req_queue);
  sns_smgr_mr.msg_queue_mutex_ptr = sns_os_mutex_create(SNS_SMGR_QUE_MUTEX, &err);
  SNS_ASSERT (err == OS_ERR_NONE );

  smgr_mr_qmi_client_init(sigs_grp_ptr, 
                          SNS_REG2_SVC_ID_V01, 
                          SNS_SMGR_QMI_REG_CL_MSG_SIG,
                          SNS_SMGR_QMI_REG_CL_TIMER_SIG,
                          SNS_REG2_SVC_get_service_object_v02(),
                          &sns_smgr_mr.qmi_cl_reg);
  sig_mask |= SNS_SMGR_QMI_REG_CL_TIMER_SIG;

  smgr_mr_qmi_client_wait_for_service(sigs_grp_ptr, sig_mask);
}

/*===========================================================================

  FUNCTION:   sns_smgr_send_req

===========================================================================*/
/*!
  @brief This function fills request message header info then sends the message.

  @detail

  @param[i]  msg_header  - request message header
  @param[i]  req_ptr     - request message body
 
  @return  sns_err_code_e
 */
/*=========================================================================*/
sns_err_code_e sns_smgr_mr_send_req(
  const sns_smgr_mr_header_s* msg_header, 
  void*                       req_ptr,
  void*                       resp_ptr,
  uint16_t                    resp_len,
  void*                       resp_cb_data_ptr)
{
  sns_err_code_e  sns_err = SNS_ERR_BAD_PARM;
  qmi_client_error_type qmi_cl_err;

  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF2(
    HIGH, "send_req - svc_num=%d msg_id=%d", msg_header->svc_num, msg_header->msg_id);

  qmi_cl_err = qmi_client_send_msg_async(sns_smgr_mr.qmi_cl_reg.user_handle,
                                         msg_header->msg_id,
                                         req_ptr,
                                         msg_header->body_len,
                                         resp_ptr,
                                         resp_len,
                                         smgr_mr_qmi_client_resp_cb,
                                         resp_cb_data_ptr,
                                         NULL);
  if ( qmi_cl_err == QMI_NO_ERR )
  {
    sns_err = SNS_SUCCESS;
  }
  else
  {
    if (resp_cb_data_ptr )
    {
      SNS_OS_FREE(resp_cb_data_ptr);
    }
    SNS_OS_FREE(resp_ptr);

    SNS_SMGR_PRINTF1(ERROR, "send_req - err=%d", qmi_cl_err);
    sns_err = SNS_ERR_FAILED;
  }
  SNS_OS_FREE(req_ptr);
  return sns_err;
}

/*=========================================================================
  FUNCTION:  sns_smgr_mr_get_msg
  =========================================================================*/
sns_smgr_mr_msg_s* sns_smgr_mr_get_msg(void)
{
  uint8_t            os_err;
  sns_smgr_mr_msg_s* msg_ptr = NULL;
  /* ---------------------------------------------------------------------- */

  sns_os_mutex_pend(sns_smgr_mr.msg_queue_mutex_ptr, 0, &os_err );
  msg_ptr = (sns_smgr_mr_msg_s*) sns_q_get(&sns_smgr_mr.msg_queue);
  (void)sns_os_mutex_post(sns_smgr_mr.msg_queue_mutex_ptr );

  return msg_ptr;
}

