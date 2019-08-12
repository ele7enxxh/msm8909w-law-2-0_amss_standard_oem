/******************************************************************************
  @file:  loc_conn.c
  @brief: Location Middleware User Plane Data Connection Manager

  DESCRIPTION
  This module contains routines used to manage User Plane Data Connections
 
 
  -----------------------------------------------------------------------------
  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
  All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  -----------------------------------------------------------------------------
 ******************************************************************************/


/*=============================================================================
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_conn.c#1 $
$DateTime: 2016/12/13 07:59:45 $
$Author: mplcsds1 $
=============================================================================*/

#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include "msg.h"

#include "msg.h"
#include "queue.h"

#include "aries_os_api.h"
#include "pdapi.h"
#include "pdsm_atl.h"

#include "loc_api_2.h"
#include "loc_task.h"
#include "loc_client.h"
#include "loc_pd.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_pa.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"

/*===================================================================
This current module supports only one connection at any time. If there
are two simulatenous connection need to be supported at the same time,
this module will need to be reimplemented.
====================================================================*/

// callback function when GPS engine needs to open a data session
pdsm_atl_operation_return_value_e_type loc_conn_open_cb
(
  pdsm_atl_session_handle_type  session_handle,
  pdsm_atl_open_params_type     open_param
);

// callback function when GPS engine is done with the data session
pdsm_atl_operation_return_value_e_type loc_conn_close_cb
(
  pdsm_atl_session_handle_type  session_handle,
  boolean                       e911_close
);

// Routine to initialize the location middleware conn module.
boolean loc_conn_init (void)
{
   boolean status=TRUE;

   // Registers with PDSM ATL module to only process connection open and
   // close request
#ifndef FEATURE_GNSS_SA
   status = pdsm_atl_l2_proxy_reg (PDSM_ATL_TYPE_IP,
                                   loc_conn_open_cb,
                                   loc_conn_close_cb);
#endif
   memset (&(loc_middleware_data.conn_module_data),
           0,
           sizeof (loc_conn_module_data_s_type));

   return status;
}

// Routine to process ioctl that are for location middleware conn module.
int loc_conn_process_ioctl (loc_client_handle_type       client_handle,
                            loc_ioctl_e_type             ioctl_type,
                            const loc_ioctl_data_u_type* ioctl_data)
{

   const loc_server_multi_open_status_s_type*  open_status_ptr;
   loc_conn_module_data_s_type*          conn_module_data_ptr;
   pdsm_atl_event_type                   event = PDSM_ATL_EVENT_CLEAR;
   int                                   ret_val = LOC_API_GENERAL_FAILURE;
#ifndef FEATURE_GNSS_SA
   boolean                               status=TRUE;
#endif

   conn_module_data_ptr = &(loc_middleware_data.conn_module_data);

   if(NULL == ioctl_data)
   {
       ret_val = LOC_API_INVALID_PARAMETER;
   }

   // Application issues the IOCTL to inform location engine of
   // the connection open status
   else if (ioctl_type == LOC_IOCTL_INFORM_SERVER_MULTI_OPEN_STATUS)
   {
      open_status_ptr = &(ioctl_data->multi_conn_open_status);

      LOC_MSG_HIGH ("loc_conn_process_ioctl for open, session handle = %d, saved session handle = %d \n",
                    open_status_ptr->conn_handle, conn_module_data_ptr->session_handle, 0);
      // If connection is being opened successfully, for UMTS target
      // we also need to pass down the apn name
      if (open_status_ptr->open_status == LOC_SERVER_OPEN_SUCCESS)
      {
         event = PDSM_ATL_EVENT_OPEN_SUCCESS;
         memscpy (&(conn_module_data_ptr->open_param.apn_name),
		 	     sizeof(conn_module_data_ptr->open_param.apn_name),
                 &(open_status_ptr->apn_name),
                 sizeof(open_status_ptr->apn_name));
         conn_module_data_ptr->open_param.pdp_type = (pdsm_atl_pdp_type_e_type)open_status_ptr->pdp_type;
      }
      else
      {
         event = PDSM_ATL_EVENT_OPEN_FAIL;
      }

      LOC_MSG_HIGH ("LOC_IOCTL_INFORM_SERVER_OPEN_STATUS, event = %d, apn name = %s, session handle = %d \n",
                     event, conn_module_data_ptr->open_param.apn_name, conn_module_data_ptr->session_handle);
#ifndef FEATURE_GNSS_SA
      // Inform lcoation engine on the modem about the atl open status
      status = pdsm_atl_post_event (PDSM_ATL_TYPE_IP,
                                    open_status_ptr->conn_handle,
                                    event,
                                    conn_module_data_ptr->open_param);
      if (status == TRUE)
      {
         ret_val = LOC_API_SUCCESS;
      }
#endif
   }

   // Application issues the IOCTL to inform location engine of
   // the connection close status
   else if (ioctl_type == LOC_IOCTL_INFORM_SERVER_CLOSE_STATUS)
   {
      LOC_MSG_HIGH ("LOC_IOCTL_INFORM_SERVER_CLOSE_STATUS, session handle = %d, saved session handle = %d \n",
                     ioctl_data->conn_close_status.conn_handle, conn_module_data_ptr->session_handle, 0);

      if (ioctl_data->conn_close_status.close_status == LOC_SERVER_CLOSE_SUCCESS)
      {
         event = PDSM_ATL_EVENT_CLOSE_SUCCESS;
      }
      else
      {
         event = PDSM_ATL_EVENT_CLOSE_FAIL;
      }

#ifndef FEATURE_GNSS_SA
      // Inform lcoation engine on the modem about the atl close status
      status = pdsm_atl_post_event (PDSM_ATL_TYPE_IP,
                                    ioctl_data->conn_close_status.conn_handle,
                                    event,
                                    conn_module_data_ptr->open_param);
      if (status == TRUE)
      {
         ret_val = LOC_API_SUCCESS;
      }
#endif
   }

   // In success, queue the ioctl callback. It is required all loc_ioctl that returns LOC_API_SUCCESS
   // will have a callback to inform the final processing status
   if (ret_val == LOC_API_SUCCESS)
   {
      loc_client_queue_ioctl_callback (client_handle, ioctl_type, LOC_API_SUCCESS, NULL);
   }

   LOC_MSG_HIGH ("loc_conn_process_ioctl, ioctl = %d, event = %d, ret_val = %d", ioctl_type, event, ret_val);

   return ret_val;
}


// Callback function when GPS engine needs a data data session to be brought up on A-proc
pdsm_atl_operation_return_value_e_type loc_conn_open_cb
(
  pdsm_atl_session_handle_type  session_handle,
  pdsm_atl_open_params_type     open_param
)
{
   boolean                       result = FALSE;

   LOC_MSG_HIGH ("loc_conn_open_cb session handle = %d, iface name = %d \n", session_handle, open_param.iface_name , 0);
#ifndef FEATURE_GNSS_SA
   // Inform PDSM that ATL event has been received
   pdsm_atl_post_event (PDSM_ATL_TYPE_IP,
                        session_handle,
                        PDSM_ATL_EVENT_L2_PROXY_OPEN_ACK,
                        open_param);
#endif
   // see if there is anyone to handle this event : LOC_EVENT_LOCATION_SERVER_REQUEST
   if(loc_client_is_server_request_handler_registered())
   {
       loc_cmd_s_type                cmd_data;
       loc_cmd_s_type*               cmd_ptr = &cmd_data;
       loc_server_request_s_type*    request_ptr;
       loc_conn_module_data_s_type*  conn_module_data_ptr = &(loc_middleware_data.conn_module_data);
       loc_cmd_nty_client_s_type     *ptr_nty_client_data = NULL;

       conn_module_data_ptr->session_handle = session_handle;
       memscpy (&(conn_module_data_ptr->open_param), sizeof(conn_module_data_ptr->open_param), &open_param, sizeof (open_param));

       memset (cmd_ptr, 0, sizeof (loc_cmd_s_type));
       cmd_ptr->ptr_data = NULL;
       ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
       if(NULL != ptr_nty_client_data)
       {
           request_ptr = &(ptr_nty_client_data->event_data.loc_server_request);
           LOC_MSG_HIGH ( "loc_conn_open_cb session handle = %d, iface name = %d \n", session_handle, open_param.iface_name , 0);//auto-gen, to change remove 'QSR_' and first param
        
           cmd_ptr->cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
           cmd_ptr->client_handle = LOC_HANDLE_REGISTERED_CLIENTS;
           ptr_nty_client_data->event_type = LOC_EVENT_LOCATION_SERVER_REQUEST;
        
           request_ptr->event = LOC_SERVER_REQUEST_MULTI_OPEN;
        
           // In Aries 4.0 (LOC API 1.0), open connection request only instructs A-proc to open up the connection on available air interface.
           // In Aries 5.0 (LOC API 1.1), open connection request will include the protocol type so the A-proc will know which server to connect to.
           request_ptr->payload.multi_open_req.protocol = LOC_SERVER_PROTOCOL_DEFAULT;

           result = TRUE;
           /*Set the connection type*/
           switch(open_param.connection_type) 
           {
             case PDSM_ATL_CONNECTION_LBS:
               request_ptr->payload.multi_open_req.connection_type = eQMI_LOC_WWAN_TYPE_AGNSS_V02;
               break;
             case PDSM_ATL_CONNECTION_WWAN_INTERNET:
               request_ptr->payload.multi_open_req.connection_type = eQMI_LOC_WWAN_TYPE_INTERNET_V02;
               break;
             case PDSM_ATL_CONNECTION_LBS_EMERGENCY:
               request_ptr->payload.multi_open_req.connection_type = eQMI_LOC_WWAN_TYPE_AGNSS_EMERGENCY_V02;
               break;
             default:    
               // Unrecognised connection type . Flag an error.  
               LOC_MSG_ERROR ("loc_conn_open_cb, unrecognised connection type %d", open_param.connection_type, 0, 0);
               result = FALSE;             
               break;
           }
            
           if(TRUE == result) 
           {      
             // Session handle will be used in future when there is a need to talk to MPC and PDE simultaneously
             request_ptr->payload.multi_open_req.conn_handle = session_handle;
        
             // Deliver the connection open request event to the registered location client
             cmd_ptr->ptr_data = (void *)ptr_nty_client_data;
             if(TRUE != loc_middleware_queue_cmd (cmd_ptr))
             {
               // reclaim resources allocated for the message
               loc_free(ptr_nty_client_data);
               ptr_nty_client_data = NULL;
               cmd_ptr->ptr_data = NULL;
               result = FALSE;
             }
             else
             {
               // IPC sent, we're done.
               LOC_MSG_LOW("loc_conn_open_cb, IPC sent to loc MW task", 0, 0, 0);
             }
           }
       }
       else
       {
          // memory allocation error
          result = FALSE;
       }
   }
   else
   {
       // no one can handle server request, let's fail it now
       result = FALSE;

       LOC_MSG_HIGH ("loc_conn_open_cb: no client to handle open request. reject", 0,0,0);
   }

   if(TRUE != result)
   {
      // TODO: what else can we do?
      LOC_MSG_ERROR ("loc_conn_open_cb, failed to inform client", 0, 0, 0);
      return PDSM_ATL_OPERATION_FAIL;
   }
   else
   {
      return PDSM_ATL_OPERATION_WAIT;
   }
}

// callback function when GPS engine is done with the data session
pdsm_atl_operation_return_value_e_type loc_conn_close_cb
(
  pdsm_atl_session_handle_type  session_handle,
  boolean                       e911_close
)
{
   boolean                       result = FALSE;
   loc_conn_module_data_s_type*  conn_module_data_ptr = &(loc_middleware_data.conn_module_data);
   conn_module_data_ptr->session_handle = session_handle;
   memset (&(conn_module_data_ptr->open_param), 0, sizeof (pdsm_atl_open_params_type));

   LOC_MSG_HIGH ("loc_conn_close_cb session handle = %d, \n", session_handle, 0 , 0);
#ifndef FEATURE_GNSS_SA
   pdsm_atl_post_event (PDSM_ATL_TYPE_IP,
                        session_handle,
                        PDSM_ATL_EVENT_L2_PROXY_CLOSE_ACK,
                        conn_module_data_ptr->open_param);
#endif
   // see if there is anyone to handle this event : LOC_EVENT_LOCATION_SERVER_REQUEST
   if(loc_client_is_server_request_handler_registered())
   {
       loc_cmd_s_type                cmd_data;
       loc_cmd_s_type*               cmd_ptr = &cmd_data;
       loc_server_request_s_type*    request_ptr;
       loc_cmd_nty_client_s_type     *ptr_nty_client_data = NULL;

       memset (cmd_ptr, 0, sizeof (loc_cmd_s_type));
    
       // Notify the registered client
       cmd_ptr->cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
       cmd_ptr->client_handle = LOC_HANDLE_REGISTERED_CLIENTS;
       cmd_ptr->ptr_data = NULL;
       ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
       if(NULL != ptr_nty_client_data)
       {
           ptr_nty_client_data->event_type = LOC_EVENT_LOCATION_SERVER_REQUEST;
        
           request_ptr = &(ptr_nty_client_data->event_data.loc_server_request);
           request_ptr->event = LOC_SERVER_REQUEST_CLOSE;
           request_ptr->payload.close_req.conn_handle = session_handle;
        
           // Deliver the connection open request event to the registered location client
           cmd_ptr->ptr_data = (void *)ptr_nty_client_data;
           if ( TRUE != loc_middleware_queue_cmd (cmd_ptr))
           {
             // reclaim resources allocated for the message
             loc_free(ptr_nty_client_data);
             ptr_nty_client_data = NULL;
             cmd_ptr->ptr_data = NULL;
             result = FALSE;
           }
           else
           {
             // IPC sent, we're done.
             result = TRUE;
           }
       }
       else
       {
          // memory allocation error
          result = FALSE;
       }
   }
   else
   {
       // no one can handle server request, let's fail it now
       result = FALSE;

       LOC_MSG_HIGH ("loc_conn_close_cb: no client to handle close request. reject", 0,0,0);
   }

   if(TRUE != result)
   {
      // TODO: what else can we do?
      LOC_MSG_ERROR ("loc_conn_close_cb, failed to inform client", 0, 0, 0);
      return PDSM_ATL_OPERATION_FAIL;
   }
   else
   {
      return PDSM_ATL_OPERATION_WAIT;
   }
}

