/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Ecall GPS File

GENERAL DESCRIPTION 
This file contains the GPS APIs needed by the Ecall module. 
  These APIs internally call the GPS PDAPIs.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/hlap/src/ecall_gps.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

          C O M M O N   I N C L U D E   F I L E S   A N D   D A T A

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#if (defined(FEATURE_ECALL_APP) && defined(FEATURE_CGPS))

#include "msg.h"
#include "rex.h"
#include "err.h"

#include "ecall_gps.h"
#include "ecall_app_task.h"
#include "ecall_efs.h"

ecall_gps_data_type ecall_gpsdata_struct;

/* UTC starts at Jan 1st 1970 midnight,GPS starts at Jan 6th 1980.
   Difference of 3657 days */
#define ECALL_UTC_GPS_OFFSET 315964800 


/*===========================================================================

          C O M M O N   F U N C T I O N   D E C L A R A T I O N S

===========================================================================*/

/*===========================================================================
FUNCTION ECALL_GETGPSDATA

DESCRIPTION
  This function populates a GPS position structure for ecall, for MSD.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void ecall_getgpsdata(ecall_msd_structure_s_type *ecall_msd)
{
    ecall_msd->ecall_control_info.positionconfidence    = ecall_gpsdata_struct.positionconfidence;
    ecall_msd->ecall_vehicle_location.positionlatitude  = ecall_gpsdata_struct.latitude;
    ecall_msd->ecall_vehicle_location.positionlongitude = ecall_gpsdata_struct.longitude;
    ecall_msd->ecall_timestamp                          = ecall_gpsdata_struct.timestamp;
    ecall_msd->ecall_vehicledirection                   = ecall_gpsdata_struct.vehicledirection;

    ECALL_MSG_0( ECALL_MED, "Convey the following GPS info into MSD:" );
    ECALL_MSG_3( ECALL_MED, "PositionConfidence: %d, Latitude: %d, Longitude: %d",
                             ecall_msd->ecall_control_info.positionconfidence,
                             ecall_msd->ecall_vehicle_location.positionlatitude,
                             ecall_msd->ecall_vehicle_location.positionlongitude);
    ECALL_MSG_2( ECALL_MED, "Timestamp: %d, VehicleDirection: %d",
                             ecall_msd->ecall_timestamp,
                             ecall_msd->ecall_vehicledirection );

} /* ECALL_GETGPSDATA */

/*===========================================================================
FUNCTION ECALL_GPS_PROCESS_GPS_ERROR_RECVD

DESCRIPTION
  This function sets position confidence to False (Low Confidence)
  if GPS Timer Expires or GPS engine returns error
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ecall_gps_process_gps_error_recvd
(
  void
)
{
  /* We need to set position confidence to False (Low Confidence)
     if GPS Timer Expires or GPS engine returns error */
  ecall_gpsdata_struct.positionconfidence = FALSE;
} /* ECALL_GPS_PROCESS_GPS_ERROR_RECVD */

/*===========================================================================

          Q M I - L O C   F U N C T I O N   D E C L A R A T I O N S

===========================================================================*/

#ifdef FEATURE_ECALL_HAS_QMI_LOC

#include "qmi_client.h"
#include "modem_mem.h"
#define ECALL_LOC_CLIENT_CB_DATA 0xDEAD

void ecall_qmi_loc_client_ind_cb
(
  qmi_client_type   user_handle,
  unsigned int      msg_id,
  void              *ind_buf_ptr,
  unsigned int      ind_buf_len,
  void              *ind_cb_data
);

void ecall_qmi_loc_client_error_cb
(
  qmi_client_type         user_handle,
  qmi_client_error_type   error,
  void                    *err_cb_data
);

void ecall_qmi_loc_clent_recv_loc_start_resp
(
  qmi_client_type         user_handle,
  unsigned int            msg_id,
  void                    *resp_c_struct,
  unsigned int            resp_c_struct_len,
  void                    *resp_cb_data,
  qmi_client_error_type   transp_err
);

/*---------------------------------------------------------------------------
   QMI Notifier handler
---------------------------------------------------------------------------*/
static qmi_client_type notifier_handle = NULL;

/*---------------------------------------------------------------------------
   QMI Client handler
---------------------------------------------------------------------------*/
static qmi_client_type client_handle = NULL;

/*---------------------------------------------------------------------------
   ECALL Client cookie
---------------------------------------------------------------------------*/
static int client_cb_data = ECALL_LOC_CLIENT_CB_DATA;

/*---------------------------------------------------------------------------
   Notifier OS param
---------------------------------------------------------------------------*/
static qmi_client_os_params          notifier_os_params;

/*---------------------------------------------------------------------------
   Client OS param
---------------------------------------------------------------------------*/
static qmi_client_os_params          client_os_params;

/*===========================================================================

                        EXTERNAL FUNCTION DEFINITION

===========================================================================*/

/*===========================================================================
FUNCTION ECALL_GPS_INIT

DESCRIPTION 
  Initializes the QMI-LOC client during power-up and register
  with QMI LOC service for system information indication

DEPENDENCIES 
  FEATURE_ECALL_HAS_QMI_LOC 

RETURN VALUE 
  None. 

SIDE EFFECTS 
  None. 
===========================================================================*/
boolean ecall_gps_init(void)
{
  qmi_client_error_type         retval;
  qmi_idl_service_object_type   loc_svc_obj=NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ECALL_MSG_0( ECALL_HIGH, "ecall_qmi_loc_client_init()" );

  /*-----------------------------------------------------------------------
    This function will be called during powerup. Initialize notifier
    and client handles
  -----------------------------------------------------------------------*/
  notifier_handle = NULL;
  client_handle = NULL;

  /*-----------------------------------------------------------------------
     Get LOC service object
  -----------------------------------------------------------------------*/
  loc_svc_obj = loc_get_service_object_v02();
  if(!loc_svc_obj)
  {
    ECALL_MSG_0( ECALL_ERROR, "Unable to get LOC service object" );
    return FALSE;
  }
  ECALL_MSG_0( ECALL_HIGH, "Get loc service object success" );

  /*-----------------------------------------------------------------------
    Initialize OS Signal required by QCCI to notify ecall_app_task when LOC
    service comes up
  -----------------------------------------------------------------------*/
  memset(&notifier_os_params,0,sizeof(qmi_cci_os_signal_type));
  notifier_os_params.tcb = rex_self();
  notifier_os_params.sig = ECALL_APP_NOTIFY_LOC_SVC_SIG;

  /*-----------------------------------------------------------------------
    Send QMI notifier init
  -----------------------------------------------------------------------*/
  retval = qmi_client_notifier_init(loc_svc_obj,
                                    &notifier_os_params,
                                    &notifier_handle);
  if((QMI_NO_ERR != retval) || (!notifier_handle))
  {
    ECALL_MSG_1( ECALL_ERROR, "Error during qmi_client_notifier_init error=%d",
                               retval );
    notifier_handle = NULL;
    return FALSE;
  }

  ECALL_MSG_0( ECALL_HIGH, "ecall_qmi_loc_client_init(): Initialization successful" );
  return TRUE;
}/* ECALL_GPS_INIT */

typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

}loc_resp_msg_v02;

/*===========================================================================
FUNCTION  ECALL_QMI_LOC_CLIENT_RECV_LOC_START_RESP

DESCRIPTION 
  Handle callbacks response of QMI_LOC_START_REQ_V02 and
  QMI_LOC_REG_EVENTS_REQ_V02.

DEPENDENCIES 
  FEATURE_ECALL_HAS_QMI_LOC

RETURN VALUE 
  None 

SIDE EFFECTS 
  Release the memory assigned for response
===========================================================================*/
void ecall_qmi_loc_client_recv_loc_start_resp
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                          *resp_c_struct,
 unsigned int                   resp_c_struct_len,
 void                          *resp_cb_data,
 qmi_client_error_type          transp_err
)
{
  loc_resp_msg_v02    *loc_resp = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ECALL_MSG_0( ECALL_HIGH, "ecall_qmi_loc_client_recv_loc_start_resp()" );

  /*---------------------------------------------------------------------
     Check if response pointer is valid
  ---------------------------------------------------------------------*/
  if((resp_c_struct == NULL) || (resp_cb_data == NULL))
  {
    ECALL_MSG_0( ECALL_ERROR, "Null pointer passed" );
    return;
  }

  /*---------------------------------------------------------------------
     Check if callback data is valid
  ---------------------------------------------------------------------*/
  if((int)(*((int*)resp_cb_data)) != ECALL_LOC_CLIENT_CB_DATA)
  {
    ECALL_MSG_0( ECALL_HIGH, "Invalid callback data" );
    return;
  }

  /*-----------------------------------------------------------------------
     Message already decoded. Hence no need to call QCCI decode api
  ------------------------------------------------------------------------*/
  loc_resp = (loc_resp_msg_v02*)resp_c_struct;

  if( (msg_id != QMI_LOC_START_RESP_V02) && 
      (msg_id != QMI_LOC_REG_EVENTS_RESP_V02) &&
      (msg_id != QMI_LOC_STOP_RESP_V02) )
  {
    ECALL_MSG_2( ECALL_ERROR, "Invalid msg_id=%u. Free mem 0x%x",
                               msg_id,
                               loc_resp );
    modem_mem_free(loc_resp,MODEM_MEM_CLIENT_ECALL);
    return;
  }

  if(loc_resp->resp.error != QMI_ERR_NONE_V01)
  {
    ECALL_MSG_2( ECALL_ERROR, "Error in get_loc_resp error=%d. Free mem 0x%x",
                               loc_resp->resp.error,
                               loc_resp );
    modem_mem_free(loc_resp,MODEM_MEM_CLIENT_ECALL);
    return;
  }

  modem_mem_free(loc_resp,MODEM_MEM_CLIENT_ECALL);

  return;
}/*ecall_qmi_loc_client_recv_loc_start_resp()*/

/*===========================================================================
FUNCTION ECALL_QMI_LOC_CLIENT_REGISTER_EVENT

DESCRIPTION


DEPENDENCIES 
  FEATURE_ECALL_HAS_QMI_LOC 

RETURN VALUE 
  None

SIDE EFFECTS 
  None
===========================================================================*/
void ecall_qmi_loc_client_register_event
(
  void
)
{
  qmi_txn_handle get_sys_txn_hdl;
  qmiLocRegEventsReqMsgT_v02 *LocRegEventsReqMsg;
  loc_resp_msg_v02 *loc_ind_reg_resp_msg;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  loc_ind_reg_resp_msg
    = (loc_resp_msg_v02*)
        modem_mem_alloc(sizeof(loc_resp_msg_v02),
                        MODEM_MEM_CLIENT_ECALL_CRIT);

  if(loc_ind_reg_resp_msg == NULL)
  {
    ECALL_MSG_0( ECALL_HIGH, "Couldn't get data_block_ptr memory" );
    return;
  }

  LocRegEventsReqMsg
    = (qmiLocRegEventsReqMsgT_v02*)
        modem_mem_alloc(sizeof(qmiLocRegEventsReqMsgT_v02),
                        MODEM_MEM_CLIENT_ECALL_CRIT);

  if(LocRegEventsReqMsg == NULL)
  {
    ECALL_MSG_0( ECALL_HIGH, "Couldn't get data_block_ptr memory" );
    modem_mem_free(loc_ind_reg_resp_msg,MODEM_MEM_CLIENT_ECALL_CRIT);
    return;
  }

  memset(LocRegEventsReqMsg,
         0,
         sizeof(qmiLocRegEventsReqMsgT_v02));

  LocRegEventsReqMsg->eventRegMask = 0x00000001;  /* POSITION_REPORT */

  memset(loc_ind_reg_resp_msg,
         0,
         sizeof(loc_resp_msg_v02));

  /*-----------------------------------------------------------------------
    Send request QMI_loc_GET_SYS_INFO_REQ_MSG to QMI-loc service.
    QMI_loc_GET_SYS_INFO_REQ message has no TLV.
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR !=
       qmi_client_send_msg_async(client_handle,
                                 QMI_LOC_REG_EVENTS_REQ_V02,
                                 LocRegEventsReqMsg,
                                 sizeof(qmiLocRegEventsReqMsgT_v02),
                                 loc_ind_reg_resp_msg,
                                 sizeof(loc_resp_msg_v02),
                                 ecall_qmi_loc_client_recv_loc_start_resp,
                                 &client_cb_data,
                                 &get_sys_txn_hdl))
  {
    ECALL_MSG_1( ECALL_ERROR, "Send loc get sys info request failed. Free mem 0x%x",
                               loc_ind_reg_resp_msg );
    modem_mem_free(loc_ind_reg_resp_msg,MODEM_MEM_CLIENT_ECALL_CRIT);
    modem_mem_free(LocRegEventsReqMsg, MODEM_MEM_CLIENT_ECALL_CRIT);
    return;
  }
  ECALL_MSG_0( ECALL_HIGH, "Send loc start request successful" );

  modem_mem_free(LocRegEventsReqMsg, MODEM_MEM_CLIENT_ECALL_CRIT);
} /* ECALL_QMI_LOC_CLIENT_REGISTER_EVENT */

/*===========================================================================
FUNCTION       ECALL_GETPOSITION

DESCRIPTION
  This function will send the QMI_LOC_START_REQ_V02 command to QMI_LOC
  to start the location session.

DEPENDENCIES 
  FEATURE_ECALL_HAS_QMI_LOC 

RETURN VALUE 
  None

SIDE EFFECTS 
  None
===========================================================================*/
void ecall_getposition
(
  uint32 gnss_update_time_ms
)
{
  qmi_txn_handle          get_sys_txn_hdl;
  loc_resp_msg_v02        *loc_resp_msg;
  qmiLocStartReqMsgT_v02  *loc_req_msg;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ECALL_MSG_0( ECALL_HIGH, "ecall_qmi_loc_client_session_start()" );

  /*-----------------------------------------------------------------------
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  loc_resp_msg
    = (loc_resp_msg_v02*)
        modem_mem_alloc(sizeof(loc_resp_msg_v02),
                        MODEM_MEM_CLIENT_ECALL_CRIT);

  if(loc_resp_msg == NULL)
  {
    ECALL_MSG_0( ECALL_HIGH, "Couldn't get data_block_ptr memory" );
    return;
  }

  loc_req_msg
    = (qmiLocStartReqMsgT_v02*)
        modem_mem_alloc(sizeof(qmiLocStartReqMsgT_v02),
                        MODEM_MEM_CLIENT_ECALL_CRIT);

  if(loc_req_msg == NULL)
  {
    ECALL_MSG_0( ECALL_HIGH, "Couldn't get data_block_ptr memory" );
    modem_mem_free(loc_resp_msg,MODEM_MEM_CLIENT_ECALL_CRIT);
    return;
  }
  memset(loc_req_msg,
         0,
         sizeof(qmiLocStartReqMsgT_v02));

  loc_req_msg->sessionId = 0xFF;
  /* Request for periodic Fixes */
  loc_req_msg->fixRecurrence_valid = FALSE;  /* Single Fix */
  /* Request for stable fixes only */
  loc_req_msg->intermediateReportState_valid = TRUE;
  loc_req_msg->intermediateReportState = 0x00000002; /* 0x00000002 -- OFF; default: 0x00000001 -- ON */
  /* Set the minimum interval between fixes */
  loc_req_msg->minInterval_valid = TRUE;
  loc_req_msg->minInterval = gnss_update_time_ms;
  /* Set horizontal accuracy to HIGH */
  loc_req_msg->horizontalAccuracyLevel_valid = TRUE;
  loc_req_msg->horizontalAccuracyLevel = 0x00000003; /* 0x00000003 -- HIGH; default: 0x00000001 -- LOW */

  memset(loc_resp_msg,
         0,
         sizeof(loc_resp_msg_v02));

  /*-----------------------------------------------------------------------
    Send request QMI_loc_GET_SYS_INFO_REQ_MSG to QMI-loc service.
    QMI_loc_GET_SYS_INFO_REQ message has no TLV.
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR !=
       qmi_client_send_msg_async(client_handle,
                                 QMI_LOC_START_REQ_V02,
                                 loc_req_msg,
                                 sizeof(qmiLocStartReqMsgT_v02),
                                 loc_resp_msg,
                                 sizeof(loc_resp_msg_v02),
                                 ecall_qmi_loc_client_recv_loc_start_resp,
                                 &client_cb_data,
                                 &get_sys_txn_hdl))
  {
    ECALL_MSG_1( ECALL_ERROR, "Send loc get sys info request failed. Free mem 0x%x",
                               loc_resp_msg );
    modem_mem_free(loc_resp_msg,MODEM_MEM_CLIENT_ECALL_CRIT);
    modem_mem_free(loc_req_msg,MODEM_MEM_CLIENT_ECALL_CRIT);
    return;
  }
  ECALL_MSG_0( ECALL_HIGH, "Send loc start request successful" );
  modem_mem_free(loc_req_msg,MODEM_MEM_CLIENT_ECALL_CRIT);

} /* ECALL_GETPOSITION */

/*===========================================================================
FUNCTION       ECALL_END_POSITION_SESSION

DESCRIPTION
  This function will send the QMI_LOC_STOP_REQ_V02 command to QMI_LOC
  to stop the location session.

DEPENDENCIES 
  FEATURE_ECALL_HAS_QMI_LOC 

RETURN VALUE 
  None

SIDE EFFECTS 
  None
===========================================================================*/
void ecall_end_position_session
(
  void
)
{
  qmi_txn_handle get_sys_txn_hdl;
  loc_resp_msg_v02 *loc_resp_msg;
  qmiLocStopReqMsgT_v02 *loc_req_msg;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ECALL_MSG_0( ECALL_HIGH, "ecall_qmi_loc_client_session_stop()" );

  /*-----------------------------------------------------------------------
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  loc_resp_msg
    = (loc_resp_msg_v02*)
        modem_mem_alloc(sizeof(loc_resp_msg_v02),
                        MODEM_MEM_CLIENT_ECALL_CRIT);

  if(loc_resp_msg == NULL)
  {
    ECALL_MSG_0( ECALL_HIGH, "Couldn't get data_block_ptr memory" );
    return;
  }

  loc_req_msg
    = (qmiLocStopReqMsgT_v02*)
        modem_mem_alloc(sizeof(qmiLocStopReqMsgT_v02),
                        MODEM_MEM_CLIENT_ECALL_CRIT);

  if(loc_req_msg == NULL)
  {
    ECALL_MSG_0( ECALL_HIGH, "Couldn't get data_block_ptr memory" );
    modem_mem_free(loc_resp_msg,MODEM_MEM_CLIENT_ECALL_CRIT);
    return;
  }
  memset(loc_req_msg,
         0,
         sizeof(qmiLocStopReqMsgT_v02));

  loc_req_msg->sessionId = 0xFF;

  memset(loc_resp_msg,
         0,
         sizeof(loc_resp_msg_v02));

  /*-----------------------------------------------------------------------
    Send request QMI_LOC_STOP_REQ_V02 to QMI-loc service.
    QMI_loc_GET_SYS_INFO_REQ message has no TLV.
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR !=
       qmi_client_send_msg_async(client_handle,
                                 QMI_LOC_STOP_REQ_V02,
                                 loc_req_msg,
                                 sizeof(qmiLocStopReqMsgT_v02),
                                 loc_resp_msg,
                                 sizeof(loc_resp_msg_v02),
                                 ecall_qmi_loc_client_recv_loc_start_resp,
                                 &client_cb_data,
                                 &get_sys_txn_hdl))
  {
    ECALL_MSG_1( ECALL_ERROR, "Send loc stop failed. Free mem 0x%x",
                               loc_resp_msg );
    modem_mem_free(loc_resp_msg,MODEM_MEM_CLIENT_ECALL_CRIT);
    modem_mem_free(loc_req_msg,MODEM_MEM_CLIENT_ECALL_CRIT);
    return;
  }
  ECALL_MSG_0( ECALL_HIGH, "Send loc stop request successful" );
  modem_mem_free(loc_req_msg,MODEM_MEM_CLIENT_ECALL_CRIT);

} /* ECALL_END_POSITION_SESSION */

/*==========================================================================
FUNCTION ECALL_QMI_LOC_CLIENT_PROCESS_SIGNAL

DESCRIPTION 
  Funtion to process LOC service notify signal. This function is
  used to process signal sent to ecall_ivs_task by QCCI notifying LOC
  service. 
    
DEPENDENCIES
  FEATURE_ECALL_HAS_QMI_LOC

RETURN VALUE
  None

SIDE EFFECTS 
  None 
===========================================================================*/
void ecall_qmi_loc_client_process_signal
(
  void
)
{
  qmi_client_error_type                       retval;
  qmi_service_info                            info;
  unsigned int                                num_entries=1;
  unsigned int                                num_services;
  qmi_idl_service_object_type                 loc_service_obj=NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ECALL_MSG_0( ECALL_HIGH, "ecall_qmi_loc_client_process_signal()" );

  /*-----------------------------------------------------------------------
     If client_handle!=NULL, then client_init aleady done. The signal might
     be false signal. Return immediatley.
  -----------------------------------------------------------------------*/
  if(client_handle != NULL)
  {
    ECALL_MSG_1( ECALL_ERROR, "False signal, client handle: 0x%x not NULL. RETURN",
                               client_handle );
    return;
  }

  /*-----------------------------------------------------------------------
     Get LOC service object
  -----------------------------------------------------------------------*/
  loc_service_obj = loc_get_service_object_v02();
  if(!loc_service_obj)
  {
    ECALL_MSG_0( ECALL_HIGH, "Unable to get loc service object" );
    return;
  }

  /*------------------------------------------------------------------
     Check if the loc service is up, if not return. Else,
     Get addressing information for accessing loc service
  ------------------------------------------------------------------*/
  if(QMI_NO_ERR != qmi_client_get_service_list(loc_service_obj,
                                               &info,
                                               &num_entries,
                                               &num_services))
  {
    ECALL_MSG_0( ECALL_HIGH, "QMI-loc service not up, continue to wait for signal" );
    return;
  }

  ECALL_MSG_0( ECALL_HIGH, "QMI-loc Service up" );

  /*-----------------------------------------------------------------------
    Release the notifier handlle. QMI Service notify signal need not be
    handled anymore.
    QCCI uses notify signal during power-up to signal the
    availability of QMI service. Once signal is processed, release the
    notifier_handle, so that QCCI will not send anymore signals.
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != qmi_client_release(notifier_handle))
  {
    ECALL_MSG_0( ECALL_HIGH, "Notifier release failed" );
    notifier_handle = NULL;
    return;
  }

  /*-----------------------------------------------------------------------
    Initialize the arguments for QMI Client Init
  ------------------------------------------------------------------------*/
  memset(&client_os_params,0, sizeof(qmi_client_os_params));
  client_os_params.tcb = rex_self();
  client_os_params.sig = ECALL_APP_SEND_MSG_WAIT_SIG;

  /*-----------------------------------------------------------------------
       Initialize connection to the loc service
  -----------------------------------------------------------------------*/
  retval =  qmi_client_init(&info,
                            loc_service_obj,
                            ecall_qmi_loc_client_ind_cb,
                            &client_cb_data,
                            &client_os_params,
                            &client_handle);

  if((retval != QMI_NO_ERR) || (!client_handle))
  {
     ECALL_MSG_0( ECALL_HIGH, "QMI loc client init failed" );
     client_handle = NULL;
     return;
  }
  ECALL_MSG_0( ECALL_HIGH, "QMI loc client init successful" );

  /*-----------------------------------------------------------------------
      Register for Error callback
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != qmi_client_register_error_cb(client_handle,
                                                ecall_qmi_loc_client_error_cb,
                                                &client_cb_data))
  {
    ECALL_MSG_0( ECALL_HIGH, "QMI loc register error callback failed" );
  }
  ECALL_MSG_0( ECALL_HIGH, "QMI loc register error callback successful" );

  /*-----------------------------------------------------------------------
      Register for the QMI_LOC events
  ------------------------------------------------------------------------*/
  ecall_qmi_loc_client_register_event();
}/*ecall_qmi_loc_client_process_signals()*/

/*===========================================================================
FUNCTION       ECALL_QMI_LOC_CLIENT_IND_CB

DESCRIPTION 
  QMI-LOC indication callback handler. This function is called
  by QCCI whenver there is a LOC indication message.

DEPENDENCIES 
  FEATURE_ECALL_HAS_QMI_LOC 

RETURN VALUE 
  None

SIDE EFFECTS 
  None
===========================================================================*/
void  ecall_qmi_loc_client_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf_ptr,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
)
{
  qmi_client_error_type             retval;
  qmiLocEventPositionReportIndMsgT_v02*         LocEventPositionReportInd;
  ecall_app_task_cmd_type *cmd_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------
     Check if indication buffer pointer is valid
  ---------------------------------------------------------------------*/
  if((ind_buf_ptr == NULL) || (ind_cb_data == NULL))
  {
    ECALL_MSG_0( ECALL_HIGH, "Null pointer passed" );
    return;
  }

  /*---------------------------------------------------------------------
     Check if calback data is valid
  ---------------------------------------------------------------------*/
  if((int)(*((int*)ind_cb_data)) != ECALL_LOC_CLIENT_CB_DATA)
  {
    ECALL_MSG_0( ECALL_HIGH, "Invalid callback data" );
    return;
  }

  /*---------------------------------------------------------------------
     Check if msg id is correct
  ---------------------------------------------------------------------*/
  if(msg_id != QMI_LOC_EVENT_POSITION_REPORT_IND_V02)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_qmi_loc_client_ind_cb(): Invalid msg_id: %u",
                               msg_id );
    return;
  }

  if(msg_id == QMI_LOC_EVENT_POSITION_REPORT_IND_V02)
  {
    /*-----------------------------------------------------------------------
        Allocate dynamic memory for indication message
    ------------------------------------------------------------------------*/
    LocEventPositionReportInd = modem_mem_alloc(sizeof(qmiLocEventPositionReportIndMsgT_v02),
                                   MODEM_MEM_CLIENT_ECALL_CRIT);
    if(LocEventPositionReportInd == NULL)
    {
      ECALL_MSG_0( ECALL_HIGH, "Couldn't get data_block_ptr memory" );
      return;
    }

    memset(LocEventPositionReportInd,0,sizeof(qmiLocEventPositionReportIndMsgT_v02));

    /*-----------------------------------------------------------------------
      Decode the ECALL Indication message using QCCI api
    ------------------------------------------------------------------------*/
    retval = qmi_client_message_decode(user_handle,
                                       QMI_IDL_INDICATION,
                                       msg_id,
                                       ind_buf_ptr,
                                       ind_buf_len,
                                       LocEventPositionReportInd,
                                       sizeof(qmiLocEventPositionReportIndMsgT_v02));

    if(QMI_NO_ERR != retval)
    {
      ECALL_MSG_2( ECALL_HIGH, "Unable to decode message error=%d Free mem 0x%x",
                                retval,
                                LocEventPositionReportInd );
      modem_mem_free(LocEventPositionReportInd,MODEM_MEM_CLIENT_ECALL_CRIT);
      return;
    }

    ECALL_MSG_0( ECALL_MED, "Rx POSITION, [Location Info]:" );
    if(LocEventPositionReportInd->latitude_valid == TRUE)
    {
      ECALL_MSG_1( ECALL_MED, "Latitude : %d",
                               (int32)LocEventPositionReportInd->latitude );
    }
    if(LocEventPositionReportInd->longitude_valid == TRUE)
    {
      ECALL_MSG_1( ECALL_MED, "Longitude: %d",
                               (int32)LocEventPositionReportInd->longitude );
    }
    if(LocEventPositionReportInd->timestampUtc_valid == TRUE)
    {
      ECALL_MSG_1( ECALL_MED, "Timestamp: %d",
                               LocEventPositionReportInd->timestampUtc );
    }
    if(LocEventPositionReportInd->heading_valid == TRUE)
    {
      ECALL_MSG_1( ECALL_MED, "Heading  : %d",
                               LocEventPositionReportInd->heading );
    }
      
    cmd_ptr = ecall_app_task_get_cmd_buf();
    if (cmd_ptr != NULL)
    {
      /* Post a command to the eCall App task to process this GPS fix */
      cmd_ptr->cmd_id = ECALL_APP_CMD_GPS_FIX_RECVD;
      cmd_ptr->data.gps_data.latitude_valid = LocEventPositionReportInd->latitude_valid;
      cmd_ptr->data.gps_data.longitude_valid = LocEventPositionReportInd->longitude_valid;
      cmd_ptr->data.gps_data.timestamp_valid = LocEventPositionReportInd->timestampUtc_valid;
      cmd_ptr->data.gps_data.vehicledirection_valid = LocEventPositionReportInd->heading_valid;
      cmd_ptr->data.gps_data.horConfidence_valid = LocEventPositionReportInd->horConfidence_valid;
      cmd_ptr->data.gps_data.horUncCircular_valid = LocEventPositionReportInd->horUncCircular_valid;
      cmd_ptr->data.gps_data.latitude = LocEventPositionReportInd->latitude;
      cmd_ptr->data.gps_data.longitude = LocEventPositionReportInd->longitude;
      cmd_ptr->data.gps_data.timestamp = LocEventPositionReportInd->timestampUtc;
      cmd_ptr->data.gps_data.vehicledirection = LocEventPositionReportInd->heading;
      cmd_ptr->data.gps_data.horConfidence = LocEventPositionReportInd->horConfidence;
      cmd_ptr->data.gps_data.horUncCircular = LocEventPositionReportInd->horUncCircular;
      ecall_app_task_put_cmd(cmd_ptr);
    }
    else
    {
      ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_pd_reg_event_cb()" );
    }

    /*-----------------------------------------------------------------------
       Release the memory allocated for response message
    -----------------------------------------------------------------------*/
    modem_mem_free(LocEventPositionReportInd,MODEM_MEM_CLIENT_ECALL_CRIT);
  }


  return;
}/*ecall_qmi_loc_client_ind_cb()*/



/*===========================================================================
FUNCTION       ECALL_QMI_LOC_CLIENT_ERROR_CB

DESCRIPTION 
  QMI_LOC error callback handler. This callback is called by QCCI
  to notify error.

DEPENDENCIES 
  FEATURE_ECALL_HAS_QMI_LOC 

RETURN VALUE 
  None 

SIDE EFFECTS 
  None 
===========================================================================*/
void ecall_qmi_loc_client_error_cb
(
  qmi_client_type user_handle,
  qmi_client_error_type error,
  void *err_cb_data
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ECALL_MSG_0( ECALL_HIGH, "ecall_qmi_loc_client_error_cb()" );

  /*---------------------------------------------------------------------
     Check if err cb data pointer is valid
  ---------------------------------------------------------------------*/
  if(err_cb_data == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "Null pointer passed" );
    return;
  }

  /*---------------------------------------------------------------------
     Check if calback data is valid
  ---------------------------------------------------------------------*/
  if((int)(*((int*)err_cb_data)) != ECALL_LOC_CLIENT_CB_DATA)
  {
    ECALL_MSG_0( ECALL_ERROR, "Invalid callback data" );
    return;
  }

  switch(error)
  {
    /*-----------------------------------------------------------------------
      In case of service error, client will be released. No attempt to recover
      the QMI connection will be made.
    ------------------------------------------------------------------------*/
    case QMI_SERVICE_ERR:
      ECALL_MSG_0( ECALL_ERROR, "QMI_LOC service is down" );
      /*-----------------------------------------------------------------------
        Release the client.
      ------------------------------------------------------------------------*/
      if(QMI_NO_ERR != qmi_client_release(client_handle))
      {
        ECALL_MSG_0( ECALL_ERROR, "Client release failed" );
        return;
      }

      client_handle = NULL;
      break;

    default:
      ECALL_MSG_0( ECALL_ERROR, "ecall_qmi_loc_client_error_cb(): error=%ld" );
  }
  return;
}

/*===========================================================================
FUNCTION ECALL_SAVE_POSITION

DESCRIPTION
  This function tstores the GPS position in the GPS structure.

DEPENDENCIES
  FEATURE_ECALL_HAS_QMI_LOC

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ecall_save_position
(
  boolean         qmiloc_latitude_valid,
  boolean         qmiloc_longitude_valid,
  boolean         qmiloc_timestamp_valid,
  boolean         qmiloc_vehicledirection_valid,
  boolean         qmiloc_horConfidence_valid,
  boolean         qmiloc_horUncCircular_valid,
  double          qmiloc_latitude,
  double          qmiloc_longitude,
  uint64_t        qmiloc_timestamp,
  float           qmiloc_vehicledirection,
  uint8_t         qmiloc_horConfidence,
  float           qmiloc_horUncCircular
)
{
  int32 Lat_InMilliArcSeconds = ECALL_INVALID_GPS;
  int32 Lon_InMilliArcSeconds = ECALL_INVALID_GPS;
  uint8 vehicle_direction_step = 0xFF;
  uint32 horizontal_uncertainty;

  double minute_double;
  double second_double;
  int32  degree_int;
  int32  minute_int;

  if( qmiloc_horConfidence_valid == TRUE ) 
  {
    /* The horConfidence from QMI_LOC is always 39%, we need to scale horConfidence
     * and horUncCircular to 95% by multiply 2.45 */
    horizontal_uncertainty = (uint32)( abs(qmiloc_horUncCircular) * 2.45 );

    if( (horizontal_uncertainty <= 150) && 
        (qmiloc_latitude_valid == TRUE) && (qmiloc_longitude_valid == TRUE) )
    { 
      /* Set positionconfidence to TRUE only if the position is within the limits
           of +-150m with 95% confidence and valid latitude & longitude */
      ECALL_MSG_1( ECALL_MED, "Position can be trusted (horUncCircular %d meters with 95 percents horConfidence)",
                               horizontal_uncertainty );
      ecall_gpsdata_struct.positionconfidence = TRUE;
    }
    else
    {
      ECALL_MSG_2( ECALL_MED, "Low confidence in position (horUncCircular %d meters, horConfidence %d percent)",
                               qmiloc_horUncCircular,
                               qmiloc_horConfidence );
      ecall_gpsdata_struct.positionconfidence = FALSE;
    }
  }
  else
  {
    /* No position uncertainty info */
    ECALL_MSG_0( ECALL_MED, "Low Confidence in position (no PDSM_PD_UNCERTAINTY info)" );
    ecall_gpsdata_struct.positionconfidence = FALSE;
  }

  if(qmiloc_latitude_valid == TRUE)
  {
    degree_int = (int32)qmiloc_latitude;
    minute_double = (qmiloc_latitude - degree_int) * 60;
    minute_int = (int32)minute_double;
    second_double = (minute_double - minute_int) * 60;
    Lat_InMilliArcSeconds = (int32)( (degree_int*3600 + minute_int*60 + second_double) * 1000 );

    if(Lat_InMilliArcSeconds <= (int32)ECALL_MIN_LAT ||
       Lat_InMilliArcSeconds >  (int32)ECALL_MAX_LAT)
    {
      ECALL_MSG_1( ECALL_MED, "Invalid GPS latitude location %d",
                               Lat_InMilliArcSeconds );
    }
    else
    {
      /* Only update eCall GPS data if latitude value is valid */
      ECALL_MSG_1( ECALL_MED, "ecall_gps: InMilliArcSeconds Lat = %X",
                               Lat_InMilliArcSeconds );
      ecall_gpsdata_struct.latitude  = Lat_InMilliArcSeconds;
    }
  }
  else
  {
    ECALL_MSG_0( ECALL_MED, "ecall_gps: Invalid latitude received" );
  }

  if (qmiloc_longitude_valid == TRUE)
  {
    degree_int = (int32)qmiloc_longitude;
    minute_double = (qmiloc_longitude - degree_int) * 60;
    minute_int = (int32)minute_double;
    second_double = (minute_double - minute_int) * 60;
    Lon_InMilliArcSeconds = (int32)( (degree_int*3600 + minute_int*60 + second_double) * 1000 );

    if(Lon_InMilliArcSeconds <= (int32)ECALL_MIN_LON ||
       Lon_InMilliArcSeconds >  (int32)ECALL_MAX_LON)
    {
      ECALL_MSG_1( ECALL_MED, "Invalid GPS longitude location %d",
                               Lon_InMilliArcSeconds );
    }
    else
    {
      /* Only update eCall GPS data if longitude is valid */
      ECALL_MSG_1( ECALL_MED, "ecall_gps: InMilliArcSeconds Lon = %X",
                               Lon_InMilliArcSeconds );
      ecall_gpsdata_struct.longitude = Lon_InMilliArcSeconds;
    }
  }
  else
  {
    ECALL_MSG_0( ECALL_MED, "ecall_gps: Invalid longitude received" );
  }

  if(qmiloc_vehicledirection_valid == TRUE)
  {
    /* Heading units in QMI_LOC is degrees; 1 step = 2 degrees */
    vehicle_direction_step = (uint8)(qmiloc_vehicledirection*0.5); 
    ECALL_MSG_1( ECALL_MED, "ecall_gps: vehicle direction: %d steps",
                             vehicle_direction_step );
    ecall_gpsdata_struct.vehicledirection = vehicle_direction_step;
  }
  else
  {
    ECALL_MSG_0( ECALL_MED, "ecall_gps: No valid vehicle direction received" );
  }
  
  if(qmiloc_timestamp_valid == TRUE)
  {
    /* QMI_LOC Units: Milliseconds since Jan. 1, 1970
       MSD Units:     Seconds since Jan. 1, 1970      */
    ecall_gpsdata_struct.timestamp = qmiloc_timestamp*0.001;

    ECALL_MSG_1( ECALL_MED, "ecall_gps: UTC timstamp %d",
                             ecall_gpsdata_struct.timestamp );
  }
  else
  {
    ECALL_MSG_0( ECALL_MED, "ecall_gps: No valid timestamp from GPS" );
  }
 
} /* ECALL_SAVE_POSITION */


#else /* PDAPI */

/*===========================================================================

          P D A P I   F U N C T I O N   D E C L A R A T I O N S

===========================================================================*/

#define PDSM_PD_ECALL_GPS_EVENT_MASK ( (PDSM_PD_GPS_EVENT_MASK) | (PDSM_PD_EVENT_POSITION)\
                                     | (PDSM_PD_EVENT_DONE) | (PDSM_PD_EVENT_END) \
                                     | (PDSM_PD_EVENT_BEGIN))

static pdsm_client_id_type ecall_gps_ecallclient_id = 0; /* id of our client */
static pdsm_client_type_e_type ecall_gps_clienttype = PDSM_CLIENT_TYPE_OEM;

/*===========================================================================

FUNCTION ecall_init_gps_data

DESCRIPTION
  This procedure intialises the GPS local struct to default values

DEPENDENCIES
  PDAPI

RETURN VALUE
  None

SIDE EFFECTS
  ecall_ivs_tcb

===========================================================================*/
LOCAL void ecall_init_gps_data (void)
{
  /* Initialize GPS Struct to 0 and position validity to FALSE */
  ecall_gpsdata_struct.longitude = ECALL_INVALID_GPS;
  ecall_gpsdata_struct.latitude = ECALL_INVALID_GPS;
  ecall_gpsdata_struct.timestamp = 0;
  ecall_gpsdata_struct.vehicledirection    = 0xFF;
  ecall_gpsdata_struct.positionconfidence  = FALSE;

} /* ecall_init_gps_data */

/*===========================================================================/
FUNCTION ECALL_GET_FIX_OPTIONS

DESCRIPTION
  This function populates the fix configuration information to Default values

DEPENDENCIES
  PDAPI

 PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS

==============================================================================*/
void ecall_get_fix_options(pdsm_pd_option_s_type *get_position_options_ptr)
{
  /*
  define parameters configuring the fix
  */
  get_position_options_ptr->session = PDSM_PD_SESS_TYPE_NEW;
  get_position_options_ptr->operation_mode = PDSM_SESSION_OPERATION_STANDALONE_ONLY;
  get_position_options_ptr->fix_rate.num_fixes = 1;
  get_position_options_ptr->fix_rate.time_between_fixes = 100;
  get_position_options_ptr->lsinfo.server_option = PDSM_SERVER_OPTION_USE_DEFAULT;
  //IF THE SERVER OTPINO IS SET TO DEFAULT, THERE IS NO NEED FOR HARDCODING THE IP - DOUBLE CHECK

  get_position_options_ptr->class_id = 0;  /* don't care */
  get_position_options_ptr->auth_info.auth_hash.data_id = 0;
  get_position_options_ptr->auth_info.auth_hash.data_len = 0;

} /* ecall_get_fix_options */

/*===========================================================================/
FUNCTION ECALL_PD_REG_EVENT_CB

DESCRIPTION
  This function handles PD events. Registered with pdsm_client_pd_reg()

DEPENDENCIES 
  PDAPI 

RETURN VALUE
  None

SIDE EFFECTS 
 
==============================================================================*/
static void ecall_pd_reg_event_cb
(
  void   *ecall_datablock,
  pdsm_pd_event_type  ecall_pd_event,
  const pdsm_pd_info_s_type  *ecall_pd_info 
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  if (ecall_pd_info == NULL)
  {
     ECALL_MSG_0( ECALL_HIGH, "ecall_pd_reg_event_cb: No Valid Information" );
     return;
  }
  /*lint -e525*/
  if (ecall_pd_event & PDSM_PD_EVENT_POSITION)
  {
     ECALL_MSG_0( ECALL_MED, "Rx PDSM_PD_EVENT_POSITION" );
     ECALL_MSG_1( ECALL_MED, "[Location Info] Latitude : %d",
                              ecall_pd_info->pd_info.pd_data.lat );
     ECALL_MSG_1( ECALL_MED, "                Longitude: %d",
                              ecall_pd_info->pd_info.pd_data.lon );
     ECALL_MSG_1( ECALL_MED, "                Timestamp: %d",
                              ecall_pd_info->pd_info.pd_data.time_stamp );
     ECALL_MSG_1( ECALL_MED, "                Heading  : %d",
                               ecall_pd_info->pd_info.pd_data.heading );

     cmd_ptr = ecall_app_task_get_cmd_buf();
     if (cmd_ptr != NULL)
     {
       /* Post a command to the eCall App task to process this GPS fix */
       cmd_ptr->cmd_id = ECALL_APP_CMD_GPS_FIX_RECVD;
       cmd_ptr->data.gps_data.ecall_cmd_gps_data = ecall_pd_info->pd_info.pd_data;
       ecall_app_task_put_cmd(cmd_ptr);
     }
     else
     {
       ECALL_MSG_0( ECALL_HIGH, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_pd_reg_event_cb()" );
     }
  }
  else if( ecall_pd_event & PDSM_PD_EVENT_BEGIN )
  {
    /* Indicate the start of session. */
    ECALL_MSG_0( ECALL_MED, "OTHER PD EV: PDSM_PD_EVENT_BEGIN" );
  }
  else if( ecall_pd_event & PDSM_PD_EVENT_DONE )
  {
    /* Indicating end of session. */
    ECALL_MSG_0( ECALL_MED, "OTHER PD EV: PDSM_PD_EVENT_DONE" );
  }
  else if( ecall_pd_event & PDSM_PD_EVENT_GPS_BEGIN )
  {
    /* Indicating start of positioning processing */
    ECALL_MSG_0( ECALL_MED, "OTHER PD EV: PDSM_PD_EVENT_GPS_BEGIN" );
  }
  else if( ecall_pd_event & PDSM_PD_EVENT_GPS_DONE )
  {
    /* Indicating end of positioning processing */
    ECALL_MSG_0( ECALL_MED, "OTHER PD EV: PDSM_PD_EVENT_GPS_DONE" );
  }
  else
  {
    ECALL_MSG_0( ECALL_MED, "OTHER PD EV" );
  }

} /* ECALL_PD_REG_EVENT_CB */


/*===========================================================================
FUNCTION ECALL_GPS_REPORT_GPSERROR_TO_APP

DESCRIPTION
  Internal function to report GPS error to the ecall_app_task

DEPENDENCIES
  PDAPI

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void ecall_gps_report_gpserror_to_app(void)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  cmd_ptr = ecall_app_task_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
     /* Post a command to the eCall App task to report GPS error */
    cmd_ptr->cmd_id = ECALL_APP_CMD_GPS_ERROR_RECVD;
    ecall_app_task_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_gps_report_gpserror_to_app()" );
  }

} /* ECALL_GPS_REPORT_GPSERROR_TO_APP */

/*===========================================================================
FUNCTION ECALL_PD_CLIENT_ERROR_CB

DESCRIPTION
  This is the callback function issuing a pdsm_get_position or pdsm_end_session
  command. This function ensures that the command went through successfully.
  If so, an event should be received which gives the positional information
  shortly. If not, an error msg results.

DEPENDENCIES
  PDAPI

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ecall_pd_client_error_cb
( 
  void* ecall_datablock,
  pdsm_pd_cmd_e_type ecall_pd_cmd,
  pdsm_pd_cmd_err_e_type ecall_pd_cmderr
)
{
  switch ( ecall_pd_cmd )
  {
    case PDSM_PD_CMD_GET_POS:  
      /* If there was an error associated with the request, print error */
      if ( ecall_pd_cmderr == PDSM_PD_CMD_ERR_BUSY_S )
      {
        /* we get busy error if some one is using the GPS engine,
           ex: GPS APP Tracking might be running */
        ECALL_MSG_1( ECALL_HIGH, "Err=%d, wait for PD_EVENT_POS.GPS engine in use",
                                  ecall_pd_cmderr );
      }
      else if ( ecall_pd_cmderr != PDSM_PD_CMD_ERR_NOERR )
      {
        ECALL_MSG_1( ECALL_HIGH, "ecall_pd_client_error_cb: Err=%d for GST_POS cmd, calling ecall_gps_report_gpserror_to_app....",
                                  ecall_pd_cmderr );
        ecall_gps_report_gpserror_to_app();
      }
      break;

    case PDSM_PD_CMD_END_SESSION:  
      if ( ecall_pd_cmderr == PDSM_PD_CMD_ERR_CLIENT_ID_P )
      {
        /* Invalid client ID or client not initialized */
        ECALL_MSG_2( ECALL_MED, "Err=%d, invalid client ID (eCall client ID: %d)",
                                 ecall_pd_cmderr,
                                 ecall_gps_ecallclient_id );
      }
      else if ( ecall_pd_cmderr != PDSM_PD_CMD_ERR_NO_SESS_S )
      {
        /* No active PD session for eCall client */
        ECALL_MSG_1( ECALL_MED, "Err=%d, no active PD session",
                                 ecall_pd_cmderr );
      }
      else if ( ecall_pd_cmderr != PDSM_PD_CMD_ERR_NOERR )
      {
        ECALL_MSG_1( ECALL_MED, "ecall_pd_client_error_cb: Err=%d for END_SESSION cmd",
                                 ecall_pd_cmderr );
        ecall_gps_report_gpserror_to_app();
      }
      break;

    default:
      ECALL_MSG_0( ECALL_HIGH, "ecall_pd_cmd != PDSM_PD_CMD_GET_POS || PDSM_PD_CMD_END_SESSION" );
      break;
  }

} /* ECALL_PD_CLIENT_ERROR_CB */

/*===========================================================================/
FUNCTION ECALL_GPS_INIT

DESCRIPTION
  This function is used to initialize a client object.
     To receive notification of PD events, a client must do the following as well:
       Register specific PD callbacks pdsm_client_pd_reg() and events
       Activate the client using pdsm_client_act()

DEPENDENCIES
   PDAPI

 PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS

==============================================================================*/
boolean ecall_gps_init(void)
{
  ecall_init_gps_data(); /* Set gps data to default value */

  ecall_gps_ecallclient_id = pdsm_client_init((pdsm_client_type_e_type)ecall_gps_clienttype);

  if(ecall_gps_ecallclient_id < 0) 
  {
    ECALL_MSG_0( ECALL_HIGH, "Error: Ecall_ecall_gps_init, pdsm_client_init - ecall_gps_ecallclient_id < 0" );
    return FALSE;
  }

  if (pdsm_client_pd_reg(ecall_gps_ecallclient_id,
                         (void*)&ecall_gps_clienttype,
                         ecall_pd_reg_event_cb,
                         PDSM_CLIENT_EVENT_REG,
                         (pdsm_pd_event_type)PDSM_PD_ECALL_GPS_EVENT_MASK,
                         NULL)!= PDSM_CLIENT_OK )
  {
    ECALL_MSG_0( ECALL_HIGH, "Error: Ecall_ecall_gps_init, call pdsm_client_pd_reg Failed..." );
    return FALSE;
  }

  if ( PDSM_CLIENT_OK != pdsm_client_act(ecall_gps_ecallclient_id) )
  {
    ECALL_MSG_0( ECALL_HIGH, "Error: Ecall_ecall_gps_init, pdsm_client_act - != PDSM_CLIENT_OK" );
    return FALSE;
  }

  return TRUE;
} //ecall_gps_init


/*===========================================================================
FUNCTION ECALL_GETPOSITION

DESCRIPTION
  This function triggers pdsm_get_position() during an application tracking 
  session.

DEPENDENCIES
  PDAPI

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/

void ecall_getposition
(
  uint32 gps_timeout_ms
)
{
  pdsm_pd_option_s_type opt;
  pdsm_pd_qos_type qos;

  /* Initialize the option structure to all 0 first */ 
  memset( &opt, 0, sizeof(opt) );
  /* Set the options we need only */ 
  ecall_get_fix_options(&opt);

  /* set some arbitrary initial qos parameters - will be overwritten based on user choice */
  qos.gps_session_timeout = (byte)(gps_timeout_ms * 0.001);
  /* GPS session should be eneded 1 second ahead of the next fix query */
  if (qos.gps_session_timeout > 1)
  {
    qos.gps_session_timeout -= 1;
  }
  qos.accuracy_threshold  = ECALL_INIT_FIX_ACCURACY_THRESHOLD;

  if (!pdsm_get_position(ecall_pd_client_error_cb,
                       (void*)&ecall_gps_clienttype, /* user data pointer */
                       &opt,
                       &qos,
                       ecall_gps_ecallclient_id))
  {
    ECALL_MSG_0( ECALL_HIGH, "Unable to call pdsm_get_position, calling ecall_gps_report_gpserror_to_app" );
    ecall_gps_report_gpserror_to_app();
  }
  
} /* ecall_getposition */

/*===========================================================================
FUNCTION ECALL_END_POSITION_SESSION

DESCRIPTION
  This function triggers pdsm_end_session() to ends an active eCall
  PD session.

DEPENDENCIES
  PDAPI

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void ecall_end_position_session
(
  void
)
{
  pdsm_pd_end_session_e_type  ecall_session_type = PDSM_PD_END_SESSION_FIX_REQ;

  if (!pdsm_end_session(ecall_pd_client_error_cb,
                       ecall_session_type,
                       NULL,
                       ecall_gps_ecallclient_id))
  {
    ECALL_MSG_0( ECALL_HIGH, "Unable to call pdsm_end_session, calling ecall_gps_report_gpserror_to_app" );
    ecall_gps_report_gpserror_to_app();
  }
  
} /* ECALL_END_POSITION_SESSION */

/*===========================================================================
FUNCTION ECALL_SAVE_POSITION

DESCRIPTION
  This function tstores the GPS position in the GPS structure.

DEPENDENCIES
  PDAPI

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void ecall_save_position
(
  pdsm_pd_data_type  ecall_gps_data
)
{
  int32 Lat_InMilliArcSeconds = ECALL_INVALID_GPS;
  int32 Lon_InMilliArcSeconds = ECALL_INVALID_GPS;
  uint8 vehicle_direction = 0xFF;
  uint32 horizontal_uncertainty;

  const int32 mArcSecPerDegree = 3600000;
  double  is801UnitsPerDegree; 
  double  mArcSecsPerIs801Unit;
  /*  mArcSecsPerIs801Unit could be defined explicitly as: */
  /*  const double mArcSecsPerIs801Unit = 19.311905 */

  /* Set positionconfidence */
  if(ecall_gps_data.ext_opt_field_mask & PDSM_PD_UNCERTAINTY_VALID)
  {
    /* The horConfidence from PDAPI is always 39%, we need to scale horConfidence
     * and horUncCircular to 95% by multiply 2.45 */
    horizontal_uncertainty = (uint32)( abs(ecall_gps_data.ext_pos_data.loc_unc_horizontal) * 2.45 );

    if( (horizontal_uncertainty <= 150) &&
        (ecall_gps_data.ext_opt_field_mask & PDSM_PD_LAT_LONG_VALID) )
    {
      /* Set positionconfidence to TRUE only if the position is within the limits
         of +-150m with 95% confidence and valid latitude & longitude */
      ECALL_MSG_1( ECALL_MED, "Position can be trusted (horUncCircular %d meters with 95 percents horConfidence)",
                               horizontal_uncertainty );
      ecall_gpsdata_struct.positionconfidence = TRUE;
    }
    else
    {
      ECALL_MSG_2( ECALL_MED, "Low confidence in position (horUncCircular %d meters, horConfidence %d percent)",
                               ecall_gps_data.ext_pos_data.loc_unc_horizontal,
                               ecall_gps_data.loc_uncertainty_conf );
      ecall_gpsdata_struct.positionconfidence = FALSE;
    }
  }
  else
  {
    /* No position uncertainty info */
    ECALL_MSG_0( ECALL_MED, "Low Confidence in position (no PDSM_PD_UNCERTAINTY info)" );
    ecall_gpsdata_struct.positionconfidence = FALSE;
  }

  if(ecall_gps_data.ext_opt_field_mask & PDSM_PD_LAT_LONG_VALID)
  {
    is801UnitsPerDegree   = (double)(1<<25) / 180;
    mArcSecsPerIs801Unit  = (double) mArcSecPerDegree / is801UnitsPerDegree;

    Lat_InMilliArcSeconds = (int32) ((double)ecall_gps_data.lat * mArcSecsPerIs801Unit); 
    Lon_InMilliArcSeconds = (int32) ((double)ecall_gps_data.lon * mArcSecsPerIs801Unit); 

    if(Lat_InMilliArcSeconds <= (int32)ECALL_MIN_LAT ||
       Lat_InMilliArcSeconds >  (int32)ECALL_MAX_LAT)
    {
      ECALL_MSG_1( ECALL_MED, "Invalid GPS latitude location %d",
                               Lat_InMilliArcSeconds );
    }
    else
    {
      /* Only update eCall GPS data if latitude value is valid */
      ECALL_MSG_1( ECALL_MED, "ecall_gps: InMilliArcSeconds Lat = %X",
                               Lat_InMilliArcSeconds );
      ecall_gpsdata_struct.latitude  = Lat_InMilliArcSeconds;
    }

    if(Lon_InMilliArcSeconds <= (int32)ECALL_MIN_LON ||
       Lon_InMilliArcSeconds >  (int32)ECALL_MAX_LON)
    {
      ECALL_MSG_1( ECALL_MED, "Invalid GPS longitude location %d",
                               Lon_InMilliArcSeconds );
    }
    else
    {
      /* Only update eCall GPS data if longitude is valid */
      ECALL_MSG_1( ECALL_MED, "ecall_gps: InMilliArcSeconds Lon = %X",
                               Lon_InMilliArcSeconds );
      ecall_gpsdata_struct.longitude = Lon_InMilliArcSeconds;
    }
  }
  else
  {
    ECALL_MSG_0( ECALL_MED, "ecall_gps: Invalid latitude and longitude received" );
  }

  if(ecall_gps_data.ext_opt_field_mask & PDSM_PD_HEADING_VALID)
  {
    /* IS801 unit to degrees: [360/2^10] per degree.
       and then 2 degrees per step */
    vehicle_direction = (uint8)(ecall_gps_data.heading*(360/2048));
    ECALL_MSG_1( ECALL_MED, "ecall_gps: vehicle direction %d steps",
                             vehicle_direction );
    ecall_gpsdata_struct.vehicledirection = vehicle_direction;
  }
  else
  {
    ECALL_MSG_1( ECALL_MED, "ecall_gps: Invalid vehicle direction received %X",
                             vehicle_direction );
  }

  if(ecall_gps_data.ext_opt_field_mask & PDSM_PD_GPS_TIME_VALID )
  {
    /* Convert to UTC time stamp UTC since Jan 1st 1970 Midnight
       GPS received is since Jan 6th 1980. Diff ECALL_UTC_GPS_OFFSET */
    ecall_gpsdata_struct.timestamp = 
       ecall_gps_data.time_stamp - ecall_gps_data.gpsUtcOffset + ECALL_UTC_GPS_OFFSET;

    ECALL_MSG_1( ECALL_MED, "ecall_gps: UTC timstamp %d",
                             ecall_gpsdata_struct.timestamp );
  }
  else
  {
    ECALL_MSG_0( ECALL_MED, "ecall_gps: Invalid timestamp from GPS" );
  }
 
} /* ECALL_SAVE_POSITION */

/*===========================================================================
FUNCTION ECALL_GPS_RELEASE

DESCRIPTION
   Release the Ecall client object.

  This function should be called if a client doesn't need PDSM anymore.
  This will release the client object.
  If the Ecall client needs to use PDSM again, it should PD
  pdsm_client_init function to establish connection.

DEPENDENCIES
  PDAPI

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void ecall_gps_release(void)
{
  ECALL_MSG_0( ECALL_HIGH, "ecall_gps_release " );
  ecall_init_gps_data(); /* Set gps data to default value */

  /* Deallocate client to PDSM */
  (void) pdsm_client_release( ecall_gps_ecallclient_id);

} //ecall_gps_release

#endif /* FEATURE_ECALL_HAS_QMI_LOC */


#else

void ecall_gps_dummy_func(void);

#endif //End Featurisation

