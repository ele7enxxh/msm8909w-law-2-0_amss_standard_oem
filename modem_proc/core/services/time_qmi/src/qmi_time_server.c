/*=============================================================================
  Copyright (c) 2003-2013,2015 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/**
@file qmi_time_server.c
@brief

Implements time server for QMI remoting.

INITIALIZATION AND SEQUENCING REQUIREMENTS

time server should be started from tmc.c

*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time_qmi/src/qmi_time_server.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/23/15   dks     Replaced qmi core framework with qcsi calls
02/08/13   NA      Added support to query commands/TLVs supported by service
04/12/11   gparikh File created.

=============================================================================*/

/*includes*/
#include <stringl/stringl.h>
#include "qmi_time_server.h"
#include "time_remote.h"
#include "time_genoff.h"
#include "ats_v.h"
#include "rcinit.h"
#include "err.h"

/*end includes*/

/*defines go here*/
#define NUM_OF_MSGS 6
#define NUM_OF_IND_MSGS 5
/*end defines*/

/*static variables here*/
static int32_t time_server_ind_filter[NUM_OF_IND_MSGS];
static uint32_t time_server_filtered_msg_count=0;
/*end static variables*/

extern void time_server_event_loop
(
  time_server  *me
);

extern time_server* qmi_return_time_server_handle
(
  void
);


/* Forward declaration of local functions */
uint32_t time_server_get_client_index
(
  time_server  *me
);

static time_server time_server_inst;


/**
@ingroup QMI Time Server Functions

@brief
This fn is called by the client to set the offset
for the specified base

@return
None

@param[in]

  void              * - ptr to server_data,
  void               * - ptr to conn_obj,
  qmi_req_handle     - request handle
  uint32_t           - msg id
  void               * - ptr to data
  uint32_t           - data len


@param[out]
 qmi_csi_cb_error

@dependencies
time server should be started before this fn is called

@sideeffects

*/
qmi_csi_cb_error qmi_time_genoff_set_required
(
  void               *connection_handle,
  qmi_req_handle     req_handle,
  unsigned int       msg_id,
  void               *req_c_struct,
  unsigned int       req_c_struct_len,
  void               *service_cookie
)
{
   qmi_csi_error rc;
   time_set_genoff_offset_req_msg_v01 *recv_msg=NULL;
   time_set_genoff_offset_resp_msg_v01 resp_msg;
   uint32_t  temp0=0,temp1=0;

   resp_msg.resp.error = QMI_ERR_NONE_V01;
   recv_msg =  (time_set_genoff_offset_req_msg_v01 *)req_c_struct;

   /*error checking. If the req_handle is NULL, then
    return an error */
   if(NULL == req_handle)
   {
     ATS_ERR("NULL request handle received");
     return QMI_CSI_CB_INTERNAL_ERR;
   }

   if(NULL != recv_msg)
   {
     temp0 = (uint32)((recv_msg->generic_offset>>32)&0xFFFFFFFF);
     temp1 = (uint32)((recv_msg->generic_offset)&0xFFFFFFFF);

     ATS_MSG_ERROR("Got update from Apps, base = %d, offset = 0x%x %x",recv_msg->base,temp0, temp1);

     /*check for base sanity and ats_secure base*/
     if(recv_msg->base == TIME_SERVICE_BASE_SECURE_V01
          || recv_msg->base < TIME_SERVICE_BASE_RTC_V01
          || recv_msg->base > TIME_SERVICE_BASE_MFLO_V01)
     {
       ATS_MSG_HIGH("Failure while updating modem base = %d and offset = 0x%x %x",
                  recv_msg->base,temp0, temp1);
       resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
       resp_msg.resp.error = QMI_ERR_INVALID_INDEX_V01;
     }
     else
     {
       time_remote_atom_genoff_set_generic_offset(recv_msg->base, recv_msg->generic_offset);
       ATS_MSG_HIGH("Successfully updated modem base = %d and offset = 0x%x %x",
                  recv_msg->base,temp0, temp1);
       resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
     }
   }
   else
   {
     ATS_MSG("received null message ");
     resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
     resp_msg.resp.error = QMI_ERR_MALFORMED_MSG_V01;
   }

   rc = qmi_csi_send_resp(req_handle,
                            QMI_TIME_GENOFF_SET_REQ_V01,
                            &resp_msg,
                            sizeof(time_set_genoff_offset_resp_msg_v01));
   return rc == QMI_CSI_NO_ERR ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR;
}

/**
@ingroup QMI Time Server Functions

@brief
This fn is called by the client to set the offset
for the specified base

@return
None

@param[in]

  void              * - ptr to server_data,
  void               * - ptr to conn_obj,
  qmi_req_handle     - request handle
  uint32_t           - msg id
  void               * - ptr to data
  uint32_t           - data len


@param[out]
 qmi_csi_cb_error

@dependencies
time server should be started before this fn is called

@sideeffects

*/
qmi_csi_cb_error qmi_time_genoff_get_required
(
  void              *connection_handle,
  qmi_req_handle    req_handle,
  unsigned int      msg_id,
  void              *req_c_struct,
  unsigned int      req_c_struct_len,
  void              *service_cookie
)
{

   qmi_csi_error rc;
   time_get_genoff_offset_resp_msg_v01 resp_msg;
   time_get_genoff_offset_req_msg_v01* req_msg;
   time_genoff_args_struct_type temp_genoff_args;
   uint64 ts_val_msecs = 0;


   /*error checking. If the req_handle is NULL, then
    return an error */
   if(NULL == req_handle || NULL == req_c_struct)
   {
     ATS_MSG("NULL request handle received");
     return QMI_CSI_CB_INTERNAL_ERR;
   }

   resp_msg.resp.error = QMI_ERR_NONE_V01;
   req_msg = (time_get_genoff_offset_req_msg_v01*)req_c_struct;

   ATS_MSG_1("Request from Apps to get absolute time for base = %d from modem ", req_msg->base);

   if (req_msg->base < TIME_SERVICE_BASE_RTC_V01 || req_msg->base > TIME_SERVICE_BASE_MFLO_V01)
   {
     ATS_MSG_ERROR("The base = %d requested is out of bounds, min %d, max %d",req_msg->base, 
	 TIME_SERVICE_BASE_RTC_V01,(((uint32)TIME_SERVICE_BASE_MFLO_V01)-1));
     resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
     resp_msg.resp.error = QMI_ERR_INVALID_INDEX_V01;
   }
   else
   {
     /*get the time using genoff api*/
     temp_genoff_args.base        = req_msg->base;
     temp_genoff_args.base_source = TIME_SCLK;
     temp_genoff_args.ts_val      = &ts_val_msecs;
     temp_genoff_args.unit        = TIME_MSEC;
     temp_genoff_args.operation   = T_GET;
     time_genoff_opr( &temp_genoff_args );

     resp_msg.base = req_msg->base;
     resp_msg.generic_offset = ts_val_msecs;
     resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;

     ATS_MSG_2("Returning absolute time = 0x%x %x msecs",ts_val_msecs&0xFFFFFFFF, ts_val_msecs>>32);

   }



   rc = qmi_csi_send_resp(req_handle,
                          QMI_TIME_GENOFF_GET_RESP_V01,
                          &resp_msg,
                          sizeof(time_get_genoff_offset_resp_msg_v01));

   return rc == QMI_CSI_NO_ERR ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR;
}


/**
@ingroup QMI Time Server Functions

@brief
This fn is called by the client to turn off the specified*
*indication message

@return
None

@param[in]

  void              * - ptr to server_data,
  void               * - ptr to conn_obj,
  qmi_req_handle     - request handle
  uint32_t           - msg id
  void               * - ptr to data
  uint32_t           - data len


@param[out]
 qmi_csi_cb_error

@dependencies
time server should be started before this fn is called

@sideeffects

*/
qmi_csi_cb_error qmi_time_turn_off_ind_required
(
  void               *connection_handle,
  qmi_req_handle     req_handle,
  unsigned int       int_msg_id,
  void               *req_c_struct,
  unsigned int       req_c_struct_len,
  void               *service_cookie
)
{
  qmi_csi_error rc;
  uint32_t index = 0;
  boolean msg_filtered = FALSE;
  time_turn_off_ind_req_msg_v01 *recv_msg=NULL;
  time_turn_off_ind_resp_msg_v01 resp_msg;
  int32_t msg_id =0;

  resp_msg.resp.error = QMI_ERR_NONE_V01;
  recv_msg =  (time_turn_off_ind_req_msg_v01 *)req_c_struct;

  /*error checking. If the req_handle is NULL, then
   return an error */
  if(NULL == req_handle)
  {
    ATS_ERR("NULL request handle received");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  if(NULL != recv_msg)
  {
    /*Get the leap seconds to be set*/
    msg_id = recv_msg->msg_id;

    ATS_MSG_1("Trying to filter 0x%x \n",msg_id);

    if(msg_id == QMI_TIME_ATS_USER_UPDATE_IND_V01
       || msg_id == QMI_TIME_ATS_TOD_UPDATE_IND_V01
       || msg_id == QMI_TIME_ATS_1X_UPDATE_IND_V01
       || msg_id == QMI_TIME_ATS_GPS_UPDATE_IND_V01
       || msg_id == QMI_TIME_ATS_SECURE_UPDATE_IND_V01)
    {
      for(index=0; index<time_server_filtered_msg_count; index++)
      {
        if(msg_id == time_server_ind_filter[index])
        {
          ATS_MSG_1("Indication message = 0x%x already filtered\n",msg_id);
          msg_filtered = TRUE;
          break;
        }
      }

      if(FALSE == msg_filtered
         && time_server_filtered_msg_count < NUM_OF_IND_MSGS )
      {
        time_server_ind_filter[time_server_filtered_msg_count] = msg_id;
        time_server_filtered_msg_count++;
        msg_filtered = TRUE;
        ATS_MSG_2("Indication message = 0x%x Added to filtering list. Filtered count=%d\n",
                 msg_id, time_server_filtered_msg_count);
      }

      if(FALSE == msg_filtered)
      {
        ATS_ERR_1("Max ind filter limit reached. Ind message = 0x%x could not be filtered\n"
                  ,msg_id);
      }
      resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    }
    else
    {
      ATS_ERR_1("Indication 0x%x not supported\n",msg_id);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INVALID_INDEX_V01;
    }
  }
  else
  {
    ATS_ERR("received null message");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_MALFORMED_MSG_V01;
  }

  rc = qmi_csi_send_resp(req_handle,
                         QMI_TIME_TURN_OFF_IND_REQ_V01,
                         &resp_msg,
                         sizeof(time_turn_off_ind_resp_msg_v01));
  return rc == QMI_CSI_NO_ERR ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR;
}

/**
@ingroup QMI Time Server Functions

@brief
This fn is called by the client to turn on the specified*
*indication message

@return
None

@param[in]

  void              * - ptr to server_data,
  void               * - ptr to conn_obj,
  qmi_req_handle     - request handle
  uint32_t           - msg id
  void               * - ptr to data
  uint32_t           - data len


@param[out]
 qmi_csi_cb_error

@dependencies
time server should be started before this fn is called

@sideeffects

*/
qmi_csi_cb_error qmi_time_turn_on_ind_required
(
  void               *connection_handle,
  qmi_req_handle     req_handle,
  unsigned int       int_msg_id,
  void               *req_c_struct,
  unsigned int       req_c_struct_len,
  void               *service_cookie
)
{
  qmi_csi_error rc;
  uint32_t index=0;
  boolean found_msg=0;
  time_turn_on_ind_req_msg_v01 *recv_msg;
  time_turn_on_ind_resp_msg_v01 resp_msg;
  int32_t msg_id =0;

  resp_msg.resp.error = QMI_ERR_NONE_V01;

  recv_msg =  (time_turn_on_ind_req_msg_v01 *)req_c_struct;

  /*error checking. If the req_handle is NULL, then
    return an error */
  if(NULL == req_handle)
  {
    ATS_ERR("NULL request handle received");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  if(NULL != recv_msg)
  {
    /*Get the leap seconds to be set*/
    msg_id = recv_msg->msg_id;

    ATS_MSG_1("Trying to unfilter 0x%x \n",msg_id);

    /*try to see if the message is in the list*/
    for(index = 0; index< time_server_filtered_msg_count; index++)
    {
      if(time_server_ind_filter[index] == msg_id)
      {
        found_msg=1;

        /*set the element to zero in case it is the last element*/
        time_server_ind_filter[index] = 0;
        break;
      }
    }

    if(found_msg)
    {
      memscpy(&time_server_ind_filter[index],
             (sizeof(int32_t)*(time_server_filtered_msg_count-1-index)),
             &time_server_ind_filter[index+1],
             (sizeof(int32_t)*(time_server_filtered_msg_count-1-index)));
      time_server_filtered_msg_count--;

      ATS_MSG_2("Indication msg = 0x%x unfiltered. Filtered count = %d",
                msg_id, time_server_filtered_msg_count);
      resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    }
    else
    {
      ATS_MSG_2("Indication msg = 0x%x could not be filtered. Filtered count = %d",
                msg_id, time_server_filtered_msg_count);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INVALID_INDEX_V01;
    }
  }
  else
  {
    ATS_MSG("received null message ");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_MALFORMED_MSG_V01;
  }

  rc = qmi_csi_send_resp(req_handle,
                         QMI_TIME_TURN_ON_IND_REQ_V01,
                         &resp_msg,
                         sizeof(time_turn_on_ind_resp_msg_v01));
  return rc == QMI_CSI_NO_ERR ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR;
}

/**
@ingroup QMI Time Server Functions

@brief
This fn is internally in this file

@return
None

@param[in]

  int32_t - the msg id


@param[out]
 boolean - whether the msg id is filtered or not

@dependencies
time server should be started before this fn is called

@sideeffects

*/
boolean time_server_is_ind_filtered
(
  int32_t msg_id
)
{
  uint32_t index=0;
  boolean found_msg=0;

  if(time_server_filtered_msg_count > NUM_OF_IND_MSGS )
  {
    ATS_MSG_1("time_server_filtered_msg_count =%d can cause out of bound index",
             time_server_filtered_msg_count);
    return FALSE;
  }

  /*try to see if the message is in the list*/
  for(index = 0; index< time_server_filtered_msg_count; index++)
  {
    if(time_server_ind_filter[index] == msg_id)
    {
      found_msg=1;
	  break;
    }
  }
  return found_msg;
}


/**
@ingroup QMI Time Server Functions

@brief
This fn is called by the client to set the lp sec on modem

@return
None

@param[in]

  void              * - ptr to server_data,
  void               * - ptr to conn_obj,
  qmi_req_handle     - request handle
  uint32_t           - msg id
  void               * - ptr to data
  uint32_t           - data len


@param[out]
 qmi_csi_cb_error

@dependencies
time server should be started before this fn is called

@sideeffects

*/
qmi_csi_cb_error qmi_time_leap_sec_set_required
(
  void               *connection_handle,
  qmi_req_handle     req_handle,
  unsigned int       msg_id,
  void               *req_c_struct,
  unsigned int       req_c_struct_len,
  void               *service_cookie
)
{
   qmi_csi_error rc;
   time_set_leap_sec_req_msg_v01 *recv_msg=NULL;
   time_set_leap_sec_resp_msg_v01 resp_msg;
   uint8 lp_sec =0;

   resp_msg.resp.error = QMI_ERR_NONE_V01;

   recv_msg =  (time_set_leap_sec_req_msg_v01 *)req_c_struct;

   /*error checking. If the req_handle is NULL, then
    return an error */
   if(NULL == req_handle)
   {
     ATS_ERR("NULL request handle received");
     return QMI_CSI_CB_INTERNAL_ERR;
   }

   if(NULL != recv_msg)
   {

     /*Get the leap seconds to be set*/
     lp_sec = recv_msg->leap_sec_set_value;

     ATS_MSG_1("Request from Apps to set leap sec = %d on modem ",lp_sec);

     #if (!defined (FEATURE_NO_DB) && !defined (FEATURE_GSM_PLT))

     /*as of now this call is commented out. Need to find api to set db on modem*/
     ATS_ERR("As of now, setting of lp_sec on modem is not supported");

     resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;

     #else
     ATS_ERR("The build either does not suppport DB feature or has GSM_PLT ");
     resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
     resp_msg.resp.error = QMI_ERR_NOT_SUPPORTED_V01;
     #endif

   }
   else
   {
     ATS_MSG("received null message ");
     resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
     resp_msg.resp.error = QMI_ERR_MALFORMED_MSG_V01;
   }

   rc = qmi_csi_send_resp(req_handle,
                          QMI_TIME_LEAP_SEC_SET_RESP_V01,
                          &resp_msg,
                          sizeof(time_set_leap_sec_resp_msg_v01));

   return rc == QMI_CSI_NO_ERR ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR;
}

/**
@ingroup QMI Time Server Functions

@brief
This fn is called by the client to get the lp sec on modem

@return
None

@param[in]

  void              * - ptr to server_data,
  void               * - ptr to conn_obj,
  qmi_req_handle     - request handle
  uint32_t           - msg id
  void               * - ptr to data
  uint32_t           - data len


@param[out]
 qmi_csi_cb_error

@dependencies
time server should be started before this fn is called

@sideeffects

*/
qmi_csi_cb_error qmi_time_leap_sec_get_required
(
  void               *connection_handle,
  qmi_req_handle     req_handle,
  unsigned int       msg_id,
  void               *req_c_struct,
  unsigned int       req_c_struct_len,
  void               *service_cookie
)
{
   qmi_csi_error rc;
   time_get_leap_sec_resp_msg_v01 resp_msg;
   #if (!defined (FEATURE_NO_DB) && !defined (FEATURE_GSM_PLT))
   uint8 lp_sec =0;
   #endif /*(!defined (FEATURE_NO_DB) && !defined (FEATURE_GSM_PLT))*/

   /*error checking. If the req_handle is NULL, then
    return an error */
   if(NULL == req_handle)
   {
     ATS_ERR("NULL request handle received");
     return QMI_CSI_CB_INTERNAL_ERR;
   }

   resp_msg.resp.error = QMI_ERR_NONE_V01;

   ATS_MSG("Request from Apps to get leap sec = %d from modem ");

   #if (!defined (FEATURE_NO_DB) && !defined (FEATURE_GSM_PLT))
   {
     db_items_value_type db;
     db_get( DB_LP_SEC, &db );
     lp_sec = db.lp_sec;
     resp_msg.leap_second = lp_sec;
     ATS_MSG_1("lp_sec = %d sent to apps",resp_msg.leap_second);
   }
   resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
   #else
   ATS_ERR("The build either does not suppport DB feature or has GSM_PLT ");
   resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
   resp_msg.resp.error = QMI_ERR_NOT_SUPPORTED_V01;
   #endif


   rc = qmi_csi_send_resp(req_handle,
                          QMI_TIME_LEAP_SEC_GET_RESP_V01,
                          &resp_msg,
                          sizeof(time_get_leap_sec_resp_msg_v01));

   return rc == QMI_CSI_NO_ERR ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR;

}

/* Dispatch Table for time server */
qmi_csi_req_handler_tbl_type time_server_dispatcher[NUM_OF_MSGS] = {
     {QMI_TIME_GENOFF_SET_REQ_V01, 1, qmi_time_genoff_set_required},
     {QMI_TIME_GENOFF_GET_REQ_V01, 1, qmi_time_genoff_get_required},
     {QMI_TIME_TURN_OFF_IND_REQ_V01, 1, qmi_time_turn_off_ind_required},
     {QMI_TIME_TURN_ON_IND_REQ_V01, 1, qmi_time_turn_on_ind_required},
     {QMI_TIME_LEAP_SEC_SET_REQ_V01, 1, qmi_time_leap_sec_set_required},
     {QMI_TIME_LEAP_SEC_GET_REQ_V01,1, qmi_time_leap_sec_get_required},
};



/**
@ingroup QMI Time Server Functions

@brief
This fn is called internally by QMI framework

@return
None

@param[in]

  qmi_client_handle client_handle,
  time_server       *me,
  void              **connection_handle


@param[out]
 qmi_csi_cb_error

@dependencies

@sideeffects

*/
qmi_csi_cb_error time_server_connect
(
  qmi_client_handle client_handle,
  void              *service_cookie,
  void              **connection_handle
)
{
    time_server       *me = (time_server *)service_cookie;
    uint32_t client_index=TIME_MAX_NUM_CLIENTS;

    /*error checking. If the req_handle is NULL, then
    return an error */
    if(NULL == me)
    {
      ATS_ERR("NULL time server object ptr received");
      return QMI_CSI_CB_INTERNAL_ERR;
    }

    ATS_MSG_1("Connect callback called.............. with server pointer:%p\n",me);

    if((client_index = time_server_get_client_index(me)) >= TIME_MAX_NUM_CLIENTS )
    {
      return QMI_CSI_CB_CONN_REFUSED;
    }

    /* Set this client as active client */
    me->client_data[client_index].active_flag = 1;
    me->client_data[client_index].client = client_handle;

    /* Assign connection handle to the address of the connection object */
    *connection_handle = &(me->client_data[client_index]);

    ATS_MSG_1("Returning from the connect callback.........with connection handle set to :%p\n",*connection_handle);
    return QMI_CSI_CB_NO_ERR;
}

/**
@ingroup QMI Time Server Functions

@brief
This fn is called internally by QMI framework

@return
None

@param[in]

  void  * -  ptr to connection handle
  time_server  * - ptr to time server object


@param[out]
 qmi_csi_cb_error

@dependencies

@sideeffects

*/
void time_server_disconnect
(
  void          *connection_handle,
  void          *service_cookie
)
{
    time_server   *me = (time_server *)service_cookie;
    time_server_client_data_type *client_data=NULL;

    /*error checking. If the req_handle is NULL, then
    return an error */
    if(NULL == me )
    {
      ATS_ERR("NULL time server object ptr received");
      return;
    }

    ATS_MSG_2("Disconnect callback called........with connection handle:%p, server handle:%p\n",connection_handle,me);

   client_data = (time_server_client_data_type *)connection_handle;
   if(NULL != client_data)
   {
     client_data->active_flag = 0;
     client_data->client = NULL;
   }
}


/**
@ingroup QMI Time Server Functions

@brief
fn for initializing a time server

@return
time_server_error

@param[in]
time_server ** - Buffer to store pointer to time_server instance
os_params* - OS params to use to register time_server with QMI

@dependencies

@sideeffects

*/
time_server_error time_server_init (time_server **handle,
                                    qmi_csi_os_params *os_params)
{
    time_server_error rc = TIME_SERVER_INTERNAL_ERR;

    if (handle == NULL || os_params == NULL)
    {
        ATS_ERR_2 ("Invalid param to time_server_init %d %d", (uint32)handle,
                   (uint32)os_params);
        return rc;
    }

    memset (&time_server_inst, 0, sizeof (time_server_inst));

    rc = time_server_register (&time_server_inst, os_params);

    if (rc == TIME_SERVER_NO_ERROR)
    {
        *handle = &time_server_inst;
    }

    return rc;
}




/**
@ingroup QMI Time Server Functions

@brief
fn for registering time server

@return
None

@param[in]
  time_server  * - ptr to time server object


@param[out]
time_server_error - error returned

@dependencies

@sideeffects

*/
time_server_error time_server_register
(
  time_server *me,
  qmi_csi_os_params *os_params
)
{
    qmi_csi_error rc;
    qmi_idl_service_object_type time_service_object = time_get_service_object_v01();
    qmi_csi_options options;

    /*error checking. If the req_handle is NULL, then
    return an error */
    if(NULL == me)
    {
      ATS_ERR("NULL request handle received");
      return TIME_SERVER_INTERNAL_ERR;
    }


    /* Register QMI TMD service for logging of supported interfaces */
    (void) qmi_si_register_object(time_service_object,
                                  0, /* Service Instance */
                                  time_get_service_impl_v01());

    /* Registering the service object and callbacks with QCSI framework */
    QMI_CSI_OPTIONS_INIT(options);
    QMI_CSI_OPTIONS_SET_REQ_HANDLER_TBL(options, time_server_dispatcher, NUM_OF_MSGS);

    rc = qmi_csi_register_with_options(time_service_object,
                                       time_server_connect,
                                       time_server_disconnect,
                                       NULL,
                                       me,
                                       os_params,
                                       &options,
                                       &me->service_handle);
    if(rc != QMI_CSI_NO_ERR)
    {
      return TIME_SERVER_INTERNAL_ERR;
    }

    me->service_registered = 1;
    return TIME_SERVER_NO_ERROR;
}

/**
@ingroup QMI Time Server Functions

@brief
fn for unregistering time server

@return
None

@param[in]
  time_server  * - ptr to time server object


@param[out]
time_server_error - error returned

@dependencies
qmi time server has to be started before
calling this function

@sideeffects

*/
time_server_error time_server_unregister
(
  time_server *me
)
{
    me->service_registered = 0;
    return TIME_SERVER_NO_ERROR;
}

/**
@ingroup QMI Time Server Functions

@brief
fn for getting number of clients

@return
None

@param[in]
  time_server  * - ptr to time server object


@param[out]
time_server_error - error returned

@dependencies
qmi time server has to be started before
calling this function

@sideeffects

*/
uint32_t time_server_get_client_index
(
  time_server *me
)
{

    uint32_t client_index=0;

    /*error checking. If the req_handle is NULL, then
    return an error */
    if(NULL == me)
    {
      ATS_ERR("NULL request handle received");
      return 0;
    }

    for (client_index = 0; client_index < TIME_MAX_NUM_CLIENTS; client_index++ ) {
        if (me->client_data[client_index].active_flag == 0)
        break;
    }

    return  client_index;
}

/**
@ingroup QMI Time Server Functions

@brief
fn for sending indication.

@return
None

@param[in]
  time_server  * - ptr to time server object
  int32_t       - msg_id
  uint16_t      - base,
  uint64_t      - offset

@param[out]
time_server_error - error returned

@dependencies
qmi time server has to be started before
calling this function

@sideeffects

*/
time_server_error time_server_send_ind
(
  time_server  *me,
  int32_t      msg_id,
  uint16_t     base,
  uint64_t     offset
)
{

    int32_t                    ind_c_struct_len;
    qmi_csi_error err=QMI_CSI_NO_ERR;
    time_update_indication_message_v01 event_report_ind;

    /*error checking. If the req_handle is NULL, then
    return an error */
    if(NULL == me)
    {
      ATS_ERR("NULL request handle received");
      return TIME_SERVER_INTERNAL_ERR;
    }

    ATS_MSG("Inside time_server_send_ind()");

    switch (msg_id)
    {
      /*supported messages will go here*/
      case QMI_TIME_ATS_USER_UPDATE_IND_V01:
      case QMI_TIME_ATS_TOD_UPDATE_IND_V01:
      case QMI_TIME_ATS_1X_UPDATE_IND_V01:
      case QMI_TIME_ATS_GPS_UPDATE_IND_V01:
      case QMI_TIME_ATS_SECURE_UPDATE_IND_V01:
        /* Allocate the indication structure and
           change any server state if required */
        ATS_MSG_1("time_server_test: Calling qmi_csi_send_broadcast_ind() for msg_id=%d ",msg_id);
        memset(&event_report_ind,0,sizeof(time_update_indication_message_v01));
        ind_c_struct_len  =  sizeof(time_update_indication_message_v01);
        event_report_ind.base = base;
        event_report_ind.offset = offset;
        /* This function will eventually call time_server_event_report_ind
           on each client registered */
        err = qmi_csi_send_broadcast_ind(me->service_handle,
                                       msg_id,
                                       &event_report_ind,
                                       ind_c_struct_len);

        ATS_MSG_HIGH("time_server_test: Calling qmi_csi_send_broadcast_ind(),base=%d, offset=%d err=%d ",event_report_ind.base,event_report_ind.offset,err);


        break;

      /*unsuppported messages can be clubbed under default*/
      default:
        ATS_MSG_1("time_server_test: msg_id = %d not supported.",msg_id);
        return TIME_SERVER_INTERNAL_ERR;
      break;
    }
    return  (time_server_error)err;
}

/**
@ingroup QMI Time Server Functions

@brief
Function called by modem to send over
the indications to apps

@return
None

@param[in]
   uint16 - base to be set
   uint64 - offset to be set

@param[out]
None

@dependencies
qmi time server has to be started before
calling this function

@sideeffects

*/
void time_server_call_ind
(
  uint16 base,
  uint64 offset
)
{
  int msg_id=-1;
  time_server* time_server_obj=NULL;
  time_server_error error = TIME_SERVER_INVALID_OBJECT;
  boolean status = TRUE;
  time_server_obj = qmi_return_time_server_handle();

  /*assign the message id here*/
  switch(base)
  {
    case ATS_TOD:
      msg_id = QMI_TIME_ATS_TOD_UPDATE_IND_V01;
      break;

    case ATS_USER:
      msg_id = QMI_TIME_ATS_USER_UPDATE_IND_V01;
      break;

    case ATS_1X:
      msg_id = QMI_TIME_ATS_1X_UPDATE_IND_V01;
      break;

    case ATS_GPS:
      msg_id = QMI_TIME_ATS_GPS_UPDATE_IND_V01;
      break;

    case ATS_SECURE:
      msg_id= QMI_TIME_ATS_SECURE_UPDATE_IND_V01;
      break;
    default:
      ATS_MSG("Remoting of base=%d not supported");
      status = FALSE;
      break;
  }

  if(NULL == time_server_obj)
  {
    ATS_MSG("Time server object is null");
    status = FALSE;
  }

  if(TRUE == time_server_is_ind_filtered(msg_id))
  {
    ATS_MSG_1("Indication 0x%x is filtered out",msg_id);
    status = FALSE;
  }

  if(TRUE==status)
  {
    error = time_server_send_ind( time_server_obj, msg_id, base, offset);
    ATS_MSG_1("time_server_send_ind returned = %d", error);
  }
}

time_server* qmi_return_time_server_handle
(
  void
)
{
  return &time_server_inst;
}
