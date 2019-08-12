/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M   L O C K  T I M E  G P S  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock time GPS functions.


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/16   vdc      Remove F3 messages for memory allocation failure
01/06/16   stv      Remote simlock support
10/06/15   stv      Fix for compilation issue
06/04/15   stv      Using time uncertanity to validate time received
05/17/15   stv      Fix compiler warnings
04/15/15   stv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#if defined(FEATURE_SIMLOCK) && (defined(FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)) && defined(FEATURE_SIMLOCK_QMI_LOC)

#include "simlock_common.h"
#include "simlock_platform.h"
#include "rex.h"
#include "simlock.h"
#include "simlock_modem.h"
#include "simlock_time_gps.h"
#include "qmi_client.h"
#include "location_service_v02.h"

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_TIME_QMI_LOC_INFO

   DESCRIPTION: Contains information about the variables related to QMI LOC connection.
-------------------------------------------------------------------------------*/
static struct
{
  qmi_idl_service_object_type   qmi_loc_svc_obj;
  qmi_client_type               notifier_handle;
  qmi_client_type               client_handle;
  qmi_client_os_params          notifier_os_params;
  qmi_client_os_params          client_os_params;
}simlock_time_qmi_loc_info;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_TIME_QMI_LOC_GET_BEST_POS_REQ_INFO

   DESCRIPTION: Contains information about the variables related to
                        QMI LOC GET BEST AVAILABLE POSITION REQ
-------------------------------------------------------------------------------*/
static struct
{
  qmi_txn_handle                txn_hdl_from_request;
  uint32                        req_txn_id;
  boolean                       req_in_progress;
}simlock_time_qmi_loc_best_pos_req_info;

/* This timer is triggered once the FAILURE  is returned from QMI LOC GET BEST AVAILABLE POSTION REQ */
static rex_timer_type *simlock_time_gps_timer = NULL;

/* Two hours in seconds */
#define SIMLOCK_TIME_GPS_FAILURE_RETRY_TIME    (2 * 60 * 60)

/* Max uncertanity value (in milli seconds),
   acceptable to consider the time sent in QMI LOC IND */
#define SIMLOCK_TIME_UNCERTAINTY_MAX_VAL       (60000)

/*===========================================================================
FUNCTION SIMLOCK_TIME_QMI_LOC_INIT

DESCRIPTION
  Initializes the QMI-LOC client during power-up
  Register with QMI LOC service for system information indication

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void simlock_time_qmi_loc_init(
  void
)
{
  qmi_client_error_type ret_val = QMI_NO_ERR;

  SIMLOCK_MSG_LOW_0("SIMLOCK TIME QMI LOC INIT");

  memset(&simlock_time_qmi_loc_info, 0, sizeof(simlock_time_qmi_loc_info));

  /* Get the service object information for QMI LOC */
  simlock_time_qmi_loc_info.qmi_loc_svc_obj = loc_get_service_object_v02();

  if(simlock_time_qmi_loc_info.qmi_loc_svc_obj == NULL)
  {
    return;
  }

  /* Mechanism to notify SIM LOCK when QMI LOC comes up */
  simlock_time_qmi_loc_info.notifier_os_params.tcb = rex_self();
  simlock_time_qmi_loc_info.notifier_os_params.sig = SIMLOCK_TIME_QMI_LOC_READY_SIG;

  ret_val = qmi_client_notifier_init(simlock_time_qmi_loc_info.qmi_loc_svc_obj,
                                     &simlock_time_qmi_loc_info.notifier_os_params,
                                     &simlock_time_qmi_loc_info.notifier_handle);

  if(QMI_NO_ERR != ret_val ||
     simlock_time_qmi_loc_info.notifier_handle == NULL)
  {
    SIMLOCK_MSG_ERR_1("Error during qmi_client_notifier_init ret_val 0x%x", ret_val);
    simlock_time_qmi_loc_info.notifier_handle = NULL;
    return;
  }

  SIMLOCK_MSG_MED_0("SIMLOCK TIME QMI LOC INIT : SUCCESSFUL");
} /* simlock_time_qmi_loc_init */


/*===========================================================================
FUNCTION SIMLOCK_TIME_GPS_TIMER_EXPIRY_CB

DESCRIPTION
  This function is called once the timers related to GPS handling are expired

DEPENDENCIES
  Timer has to be started for the rex to inform expiry

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_time_gps_timer_expiry_cb(
  unsigned long param
)
{
  simlock_result_enum_type  simlock_status = SIMLOCK_SUCCESS;
  simlock_task_cmd_type    *task_cmd_ptr   = NULL;
    
  (void)param;
  
  SIMLOCK_CHECK_AND_MEM_MALLOC(task_cmd_ptr,
                               sizeof(simlock_task_cmd_type));
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  memset(task_cmd_ptr, 0, sizeof(simlock_task_cmd_type));

  task_cmd_ptr->cmd_type = SIMLOCK_TIME_HANDLE_GPS_TIMER_EXIPIRY_CMD;

  /* Put on SIM Lock command queue */
  simlock_status = simlock_queue_cmd(task_cmd_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    SIMLOCK_MEM_FREE(task_cmd_ptr);
  }

  SIMLOCK_MSG_MED_1("Queue of SIMLOCK_TIME_HANDLE_GPS_TIMER_EXIPIRY_CMD to SIMLOCK, status 0x%x",
                     simlock_status);
} /* simlock_time_gps_timer_expiry_cb */


/*===========================================================================
FUNCTION SIMLOCK_TIME_QMI_LOC_RESPONSE_CB

DESCRIPTION
  This is the RESPONSE call back function given to QMI LOC
  during QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ

  This will be called from QCCI framework context and hence
  any action on invoking this should be posted to SIMLOCK context.

  In case of SUCCESS - No action as IND will be followed
  In case of FAILURE  - No IND and hence need to restart the failure retry timer.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void simlock_time_qmi_loc_response_cb(
  qmi_client_type         user_handle,
  unsigned int            msg_id,
  void                   *resp_ptr,
  unsigned int            resp_len,
  void                   *resp_cb_data,
  qmi_client_error_type   transp_err
)
{
  qmi_response_type_v01 *response_ptr = NULL;

  (void)transp_err;
  (void)resp_cb_data;
  (void)user_handle;
  (void)resp_len;
  
  response_ptr = (qmi_response_type_v01 *)resp_ptr;

  /* Ignore checking for resp_cb_data as we don't fill anything in REQ.
     Request is made only for QMI_LOC_GET_BEST_AVAILABLE_POSITION
     hence any msg_id other than that should not be entertained */
  if(msg_id != QMI_LOC_GET_BEST_AVAILABLE_POSITION_RESP_V02 ||
     response_ptr == NULL ||
     response_ptr->error != QMI_ERR_NONE_V01)
  {
    simlock_result_enum_type  simlock_status = SIMLOCK_SUCCESS;
    simlock_task_cmd_type    *task_cmd_ptr   = NULL;

    SIMLOCK_MSG_ERR_2("Error in response for QMI_LOC_GET_BEST_AVAILABLE_POSITION : Error 0x%x msg id 0x%x",
                      (response_ptr != NULL ? response_ptr->error : -1),
                      msg_id);

    SIMLOCK_CHECK_AND_MEM_MALLOC(task_cmd_ptr,
                                 sizeof(simlock_task_cmd_type));
    if (task_cmd_ptr == NULL)
    {
      simlock_time_qmi_loc_best_pos_req_info.req_in_progress = FALSE;
      SIMLOCK_MEM_FREE(response_ptr);
      return;
    }

    memset(task_cmd_ptr, 0, sizeof(simlock_task_cmd_type));

    /* Error  handle command posted to SIMLOCK if error received for the request placed to QMI LOC */
    task_cmd_ptr->cmd_type                                               = SIMLOCK_TIME_HANDLE_SOURCE_TIME_INFO_CMD;
    task_cmd_ptr->data.source_time_info_msg.source                       = SIMLOCK_TIME_SOURCE_GPS;
    task_cmd_ptr->data.source_time_info_msg.src_data.gps_time.ind_status = FALSE;

    /* Put on SIM Lock command queue */
    simlock_status = simlock_queue_cmd(task_cmd_ptr);
    if (simlock_status != SIMLOCK_SUCCESS)
    {
      /* Free the pointer since the task_cmd_ptr has not been put
              onto the command queue */
      SIMLOCK_MEM_FREE(task_cmd_ptr);
    }

    SIMLOCK_MSG_MED_1("Queue of SIMLOCK_TIME_HANDLE_SOURCE_TIME_INFO_CMD to SIMLOCK, status 0x%x",
                      simlock_status);
  }

  /* In all cases free the response pointer allocated in request */
  SIMLOCK_MEM_FREE(response_ptr);
}/* simlock_time_qmi_loc_response_cb */


/*===========================================================================
FUNCTION SIMLOCK_TIME_QMI_LOC_SEND_TIME_REQUEST

DESCRIPTION
  Send a request to QMI LOC to fetch the latest time available.
  Request is posted to QMI LOC only if the available GPS time in SIMLOCK TIME
  is older than SIMLOCK_TIME_GPS_MAX_VALIDITY_TIME

DEPENDENCIES
  None.

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static simlock_result_enum_type simlock_time_qmi_loc_send_time_request(
  void
)
{
  qmiLocGetBestAvailablePositionReqMsgT_v02  *position_req_ptr  = NULL;
  qmi_response_type_v01                      *position_resp_ptr = NULL;
  qmi_client_error_type                       req_status        = QMI_NO_ERR;
  qmi_txn_handle                              txn_handle        = (qmi_txn_handle)0;
  simlock_result_enum_type                    ret_val           = SIMLOCK_SUCCESS;
  boolean                                     gps_time_required = FALSE;

  gps_time_required = simlock_time_is_gps_time_required();

  if(gps_time_required == FALSE)
  {
    /* GPS time is not needed now. Will request after sometime once the timer expires.*/
    return SIMLOCK_SUCCESS;
  }

  /* Return if already a command is in progress */
  if(simlock_time_qmi_loc_best_pos_req_info.req_in_progress)
  {
    SIMLOCK_MSG_MED_0("A request to QMI LOC is already in progress");
    return SIMLOCK_SUCCESS;
  }

  if(simlock_time_qmi_loc_info.client_handle == NULL)
  {
    return SIMLOCK_INVALID_STATE;
  }

  /* Allocate memory for request and response to QMI LOC */
  SIMLOCK_CHECK_AND_MEM_MALLOC(position_req_ptr, sizeof(qmiLocGetBestAvailablePositionReqMsgT_v02));
  if(NULL == position_req_ptr)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(position_resp_ptr, sizeof(qmi_response_type_v01));
  if(NULL == position_resp_ptr)
  {
    SIMLOCK_FREE(position_req_ptr);
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Initially this transaction ID will be memset to 0 and
     for every request this will be incremeted.
     First Request will contain transaction ID of 1
     This transaction ID will be received in the INDIACATION message to correspond */
  position_req_ptr->transactionId = ++simlock_time_qmi_loc_best_pos_req_info.req_txn_id;
  simlock_time_qmi_loc_best_pos_req_info.txn_hdl_from_request = 0;

  /* Send the QMI_LOC_GET_BEST_AVAILABLE_POSITION Request to QMI LOC in async fashion */
  req_status = qmi_client_send_msg_async(simlock_time_qmi_loc_info.client_handle,
                                         QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ_V02,
                                         (void *)position_req_ptr,
                                         sizeof(qmiLocGetBestAvailablePositionReqMsgT_v02),
                                         (void *)position_resp_ptr,
                                         sizeof(qmi_response_type_v01),
                                         simlock_time_qmi_loc_response_cb,
                                         NULL,
                                         &txn_handle);

  if(QMI_NO_ERR != req_status)
  {
    SIMLOCK_MSG_ERR_1("Failure in sending async message GET_BEST_AVAILABLE_POSITION 0x%x Retry again later", req_status);
    simlock_time_start_gps_timer(SIMLOCK_TIME_GPS_FAILURE_RETRY_TIME);
    SIMLOCK_FREE(position_resp_ptr);
    ret_val = SIMLOCK_GENERIC_ERROR;
  }
  else
  {
    /* Update the transaction handle received from request */
    simlock_time_qmi_loc_best_pos_req_info.txn_hdl_from_request = txn_handle;
    /* Note that a request is placed to QMI LOC */
    simlock_time_qmi_loc_best_pos_req_info.req_in_progress = TRUE;
    ret_val = SIMLOCK_SUCCESS;
    /* Don't free the response pointer  here
       It will be freed later, once simlock_time_qmi_loc_response_cb is called */
  }

  SIMLOCK_FREE(position_req_ptr);

  return ret_val;
} /* simlock_time_qmi_loc_send_time_request */


/*===========================================================================
FUNCTION SIMLOCK_TIME_STOP_GPS_TIMER

DESCRIPTION
  This function will stop the timer if it is already running.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void simlock_time_stop_gps_timer(
  void
)
{
  if(simlock_time_gps_timer != NULL)
  {
    rex_clr_timer(simlock_time_gps_timer);
    rex_delete_timer_ex(simlock_time_gps_timer);
    simlock_time_gps_timer = NULL;
  }
}/* simlock_time_stop_gps_timer */


/*===========================================================================
FUNCTION SIMLOCK_TIME_START_GPS_TIMER

DESCRIPTION
  This function starts the timer specified.
  Before starting clears if the same timer is running already.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void simlock_time_start_gps_timer(
  simlock_time_type timer_in_sec
)
{
  rex_timer_cnt_type timer_val = 0;

  timer_val = timer_in_sec * SIMLOCK_MILLISECONDS_IN_A_SECOND;

  /* Stop the timer if by chance already running */
  simlock_time_stop_gps_timer();

  /* Pass the timer enum as param so that we can retrive it call back called after expiry */
  simlock_time_gps_timer = rex_create_timer_ex(simlock_time_gps_timer_expiry_cb, 0);

  if(simlock_time_gps_timer == NULL)
  {
    SIMLOCK_MSG_ERR_0("Could not create the timer");
    return;
  }

  /* start the timer */
  (void)rex_set_timer(simlock_time_gps_timer, timer_val);
}/* simlock_time_start_gps_timer */


/*===========================================================================
FUNCTION SIMLOCK_TIME_HANDLE_GPS_TIMER_EXPIRY_CMD

DESCRIPTION
  This function handles SIMLOCK_TIME_HANDLE_GPS_TIMER_EXPIRY_CMD

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_handle_gps_timer_expiry_cmd(
  void
)
{
  simlock_result_enum_type ret_val = SIMLOCK_GENERIC_ERROR;

  /* Stop the timer */
  simlock_time_stop_gps_timer();

  /* Send a new request to QMI LOC for an updated time */
  ret_val = simlock_time_qmi_loc_send_time_request();

  SIMLOCK_MSG_LOW_1("Status of QMI LOC send time request is 0x%x", ret_val);
} /* simlock_time_handle_gps_timer_expiry_cmd */


/*===========================================================================
FUNCTION SIMLOCK_TIME_QMI_LOC_CLIENT_ERROR_CB

DESCRIPTION
  Function called by Framework incase of error.
  There is no recovery from this mode.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void simlock_time_qmi_loc_client_error_cb(
  qmi_client_type          user_handle,
  qmi_client_error_type    error,
  void                    *err_cb_data
)
{
  SIMLOCK_MSG_ERR_1("SIMLOCK_TIME_QMI_LOC_CLIENT_ERROR_CB error 0x%x", error);

  (void)user_handle;
  (void)err_cb_data;
  
  switch(error)
  {
    case QMI_SERVICE_ERR:
      /* RELEASE the client handle as this cannot be used further */
      (void)qmi_client_release(simlock_time_qmi_loc_info.client_handle);
      /* RESET the client handle information */
      simlock_time_qmi_loc_info.client_handle = NULL;
      break;

    default:
      break;
  }
}  /*simlock_time_qmi_loc_client_error_cb*/


/*===========================================================================
FUNCTION SIMLOCK_TIME_QMI_LOC_CLIENT_IND_CB

DESCRIPTION
  Called by QMI LOC in case of an indication from it.
  A command will be posted to SIMLOCK to handle further.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void simlock_time_qmi_loc_client_ind_cb(
  qmi_client_type       user_handle,
  unsigned int          msg_id,
  void                 *ind_buf_ptr,
  unsigned int          ind_buf_len,
  void                 *ind_cb_data
)
{
  qmi_client_error_type                      ret_val            = QMI_NO_ERR;
  qmiLocGetBestAvailablePositionIndMsgT_v02 *ind_msg_ptr        = NULL;
  simlock_result_enum_type                   simlock_status     = SIMLOCK_SUCCESS;
  simlock_task_cmd_type                     *task_cmd_ptr       = NULL;
  boolean                                    ind_success_status = FALSE;

  (void)ind_cb_data;
  
  if(ind_buf_ptr == NULL ||
     msg_id != QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND_V02)
  {
    return;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(ind_msg_ptr, sizeof(qmiLocGetBestAvailablePositionIndMsgT_v02));
  if(ind_msg_ptr == NULL)
  {
    simlock_time_qmi_loc_best_pos_req_info.req_in_progress = FALSE;
    return;
  }

  ret_val = qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      ind_buf_ptr,
                                      ind_buf_len,
                                      ind_msg_ptr,
                                      sizeof(qmiLocGetBestAvailablePositionIndMsgT_v02));

  do
  {
    if(ret_val != QMI_NO_ERR ||
       ind_msg_ptr->status != eQMI_LOC_SUCCESS_V02)
    {
      SIMLOCK_MSG_MED_2("Decoding error for QMI LOC IND 0x%x status 0x%x", ret_val, ind_msg_ptr->status);
      ind_success_status = FALSE;
      break;
    }

    /* If the source is found to be invalid,
       no matter of time, don't use this time and retry again */
    if(ind_msg_ptr->timeSrc_valid == FALSE ||
       (ind_msg_ptr->timeSrc != eQMI_LOC_TIME_SRC_TOW_DECODE_V02 &&
        ind_msg_ptr->timeSrc != eQMI_LOC_TIME_SRC_TOW_CONFIRMED_V02 &&
        ind_msg_ptr->timeSrc != eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED_V02 &&
        ind_msg_ptr->timeSrc != eQMI_LOC_TIME_SRC_NAV_SOLUTION_V02 &&
        ind_msg_ptr->timeSrc != eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME_V02 &&
        ind_msg_ptr->timeSrc != eQMI_LOC_TIME_SRC_TIME_TRANSFORM_V02 &&
        ind_msg_ptr->timeSrc != eQMI_LOC_TIME_SRC_GLO_TOW_DECODE_V02 &&
        ind_msg_ptr->timeSrc != eQMI_LOC_TIME_SRC_QZSS_TOW_DECODE_V02 &&
        ind_msg_ptr->timeSrc != eQMI_LOC_TIME_SRC_BDS_TOW_DECODE_V02))
    {
      ind_success_status = FALSE;
      break;
    }

    /* Ignore this time and retry after sometime 
       1. If the Uncertainty TLV is absent in IND
       2. If the Uncertainty TLV is present and the value of uncertainty is greater than 1 min */
    if(ind_msg_ptr->timeUnc_valid == FALSE ||
       ind_msg_ptr->timeUnc > SIMLOCK_TIME_UNCERTAINTY_MAX_VAL)
    {
      ind_success_status = FALSE;
      break;
    }

    /* Mark the ind as success after above validations */
    ind_success_status = TRUE;
  }while(0);

  /* Put the command to SIMLOCK with QMI LOC  IND status and time */
  SIMLOCK_CHECK_AND_MEM_MALLOC(task_cmd_ptr,
                               sizeof(simlock_task_cmd_type));
  if(task_cmd_ptr == NULL)
  {
    SIMLOCK_FREE(ind_msg_ptr);
    simlock_time_qmi_loc_best_pos_req_info.req_in_progress = FALSE;
    return;
  }

  if(ind_success_status)
  {
    /* Fill transaction details */
    task_cmd_ptr->data.source_time_info_msg.src_data.gps_time.is_txn_id_present = ind_msg_ptr->transactionId_valid;
    if(task_cmd_ptr->data.source_time_info_msg.src_data.gps_time.is_txn_id_present)
    {
      task_cmd_ptr->data.source_time_info_msg.src_data.gps_time.txn_id = ind_msg_ptr->transactionId;
    }

    /* Fill time details */
    task_cmd_ptr->data.source_time_info_msg.src_data.gps_time.is_time_present = ind_msg_ptr->timestampUtc_valid;
    if(task_cmd_ptr->data.source_time_info_msg.src_data.gps_time.is_time_present)
    {
      task_cmd_ptr->data.source_time_info_msg.src_data.gps_time.time = ind_msg_ptr->timestampUtc;
    }
  }

  task_cmd_ptr->cmd_type                                               = SIMLOCK_TIME_HANDLE_SOURCE_TIME_INFO_CMD;
  task_cmd_ptr->data.source_time_info_msg.source                       = SIMLOCK_TIME_SOURCE_GPS;
  task_cmd_ptr->data.source_time_info_msg.src_data.gps_time.ind_status = ind_success_status;

  SIMLOCK_FREE(ind_msg_ptr);

  /* Put on SIM Lock command queue */
  simlock_status = simlock_queue_cmd(task_cmd_ptr);
  if (simlock_status != SIMLOCK_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    SIMLOCK_MEM_FREE(task_cmd_ptr);
  }

  SIMLOCK_MSG_MED_1("Queue of SIMLOCK_TIME_HANDLE_SOURCE_TIME_INFO_CMD to SIMLOCK, status 0x%x",
                     simlock_status);
}  /*simlock_time_qmi_loc_client_ind_cb*/


/*===========================================================================
FUNCTION SIMLOCK_TIME_HANDLE_QMI_LOC_READY_SIG

DESCRIPTION
  Process the service ready signal from QMI LOC
  Releases the notifier handle so as to avoid any redundant signal notifications
  Registration for error call back is done

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void simlock_time_handle_qmi_loc_ready_sig(
  void
)
{
  qmi_client_error_type    ret_val    = QMI_NO_ERR;
  simlock_result_enum_type req_status = SIMLOCK_GENERIC_ERROR;
  qmi_service_info         srv_info;

  memset(&srv_info, 0, sizeof(srv_info));

  SIMLOCK_MSG_MED_0("SIMLOCK TIME handle QMI LOC Sig");

  (void)rex_clr_sigs(UIM_SIMLOCK_TCB, SIMLOCK_TIME_QMI_LOC_READY_SIG);

  /* Client Hanlde should be NULL as it is yet to be retrived from Service
     If it is not NULL , one possibility the sig is set incorrectly again  */
  if(simlock_time_qmi_loc_info.client_handle != NULL)
  {
    SIMLOCK_MSG_ERR_1("Incorrect client handle at this point 0x%x",
                       simlock_time_qmi_loc_info.client_handle);
    return;
  }

  /* Check if the required QMI service is up */
  if(QMI_NO_ERR != qmi_client_get_any_service(simlock_time_qmi_loc_info.qmi_loc_svc_obj,
                                              &srv_info))
  {
    SIMLOCK_MSG_HIGH_0("QMI LOC Service is NOT Ready");
    return;
  }

  /*------------------------------------------------------------------------------
     QMI LOC Serive is UP now

     Perform the following now :
     1.  RELEASE the notifier Handle
     2.  ESTABLISH Connection with Service
     3.  REGISTER error call back with Service.
    --------------------------------------------------------------------------------*/

  /* QMI LOC is up by this time and the notification mechanism for
     service availability from framework is no more necessary.
     Hence release the notifier handle .Even if notifier handle release is failed,
     proceed further.*/
  (void)qmi_client_release(simlock_time_qmi_loc_info.notifier_handle);

  simlock_time_qmi_loc_info.notifier_handle = NULL;
  memset(&simlock_time_qmi_loc_info.client_os_params, 0, sizeof(qmi_client_os_params));
  simlock_time_qmi_loc_info.client_os_params.tcb = rex_self();
  simlock_time_qmi_loc_info.client_os_params.sig = SIMLOCK_TIME_QMI_LOC_RESPONSE_SIG;

  /* Establish connection between Service and SIMLOCK TIME
     Provide the IND call back to be called by QMI LOC when
     there is an indication to be sent to the control point */
  ret_val =  qmi_client_init(&srv_info,
                             simlock_time_qmi_loc_info.qmi_loc_svc_obj,
                             simlock_time_qmi_loc_client_ind_cb,
                             NULL,
                             &simlock_time_qmi_loc_info.client_os_params,
                             &simlock_time_qmi_loc_info.client_handle);

  if(ret_val != QMI_NO_ERR ||
     simlock_time_qmi_loc_info.client_handle == NULL)
  {
    SIMLOCK_MSG_ERR_1("Error during qmi_client_init 0x%x", ret_val);
    simlock_time_qmi_loc_info.client_handle = NULL;
    return;
  }

  /* Register for Error Call Back */
  ret_val = qmi_client_register_error_cb(simlock_time_qmi_loc_info.client_handle,
                                         simlock_time_qmi_loc_client_error_cb,
                                         NULL);

  /* Place a request to QMI LOC if GPS time is obsolete */
  req_status = simlock_time_qmi_loc_send_time_request();
  SIMLOCK_MSG_HIGH_1("Status of QMI LOC send time request is 0x%x", req_status);
}/* simlock_time_handle_qmi_loc_ready_sig */


/*===========================================================================
FUNCTION SIMLOCK_TIME_HANDLE_QMI_LOC_IND

DESCRIPTION
  This function handles the command posted to SIMLOCK for QMI LOC indication
  In case of
  a. Invalid IND data - Starts failure retry timer.
  b. Success IND data - Starts max validity timer.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void simlock_time_handle_qmi_loc_ind(
  const simlock_time_qmi_loc_ind_info_type *qmi_loc_ind_msg_ptr
)
{
  boolean           ind_success = FALSE;
  simlock_time_type time_in_sec = 0;

  if(qmi_loc_ind_msg_ptr == NULL)
  {
    return;
  }

  if(qmi_loc_ind_msg_ptr->ind_status         &&
     qmi_loc_ind_msg_ptr->is_txn_id_present  &&
     qmi_loc_ind_msg_ptr->txn_id == simlock_time_qmi_loc_best_pos_req_info.req_txn_id &&
     qmi_loc_ind_msg_ptr->is_time_present)
  {
    /* QMI LOC sends time in milli seconds - Convert to seconds  */
    time_in_sec = qmi_loc_ind_msg_ptr->time / SIMLOCK_MILLISECONDS_IN_A_SECOND;

    if(time_in_sec > SIMLOCK_TIME_BASE)
    {
      /* Under the following conditions - IND is processed sucessfully 
         1. IND decoding is successful.
         2. Transaction ID detail TLV is present in IND and matching with the one sent in request.
         3. Time TLV is present and is greater than the SIMLOCK time base.*/
      ind_success = TRUE;
    }
  }

  if(ind_success)
  {
    /* Update the global and send it SIMLOCK if its more new  time */
    simlock_time_update_send_time(SIMLOCK_TIME_SOURCE_GPS, time_in_sec);

    /* Start the timer for max validity.
       So that after timer expiry, a new request is posted to QMI LOC */
    simlock_time_start_gps_timer(SIMLOCK_TIME_GPS_MAX_VALIDITY_TIME);
  }
  else
  {
    /* Start the failure retry timer.
       So that after timer expiry, a new request is posted to QMI LOC */
    simlock_time_start_gps_timer(SIMLOCK_TIME_GPS_FAILURE_RETRY_TIME);
  }

  /* Reset the request in progress flag */
  simlock_time_qmi_loc_best_pos_req_info.req_in_progress = FALSE;
} /* simlock_time_handle_qmi_loc_ind */

#endif /* FEATURE_SIMLOCK && (FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK) && FEATURE_SIMLOCK_QMI_LOC */

