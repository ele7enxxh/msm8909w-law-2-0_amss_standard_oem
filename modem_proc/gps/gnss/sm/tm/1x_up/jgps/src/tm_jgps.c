/*===========================================================================

                  TM - JGPS Module

DESCRIPTION
  This file contains implementations for TM Japan-GPS (KDDI)

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/jgps/src/tm_jgps.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/13/09   gk      process end session command by ignoring it
05/27/09   ank     If MS-B Dload fails due to connection issues, abort fix.
12/10/08   rw      For incoming stop session requests that are not
                   recognized (i.e. MS-B), abort and cleanup JGPS state machine
03/06/06   rw      Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msg.h"

#include "pdapibuf.h"

#include "tm_1x_up_is801_drv_iface.h"

#ifdef FEATURE_JCDMA_GPSONE
#include "tm_jgps.h"
#include "tm_jgps_core.h"
#endif /* FEATURE_JCDMA_GPSONE */

/*=============================================================================

                                Data Structures

=============================================================================*/

/* TM-Core / JGPS protocl interface callback table */
tm_prtl_cb_s_type  tm_jgps_prtl_iface_table;


/*---------------------------------------------------------------------------

   This section contain code which is only compiled when 
   FEATURE_JCDMA_GPSONE is defined.
   
----------------------------------------------------------------------------*/
#ifdef FEATURE_JCDMA_GPSONE

/*=============================================================================

       Extern Functions: Interface with 1x UP (1x UP => JGPS)

=============================================================================*/

/*===========================================================================

FUNCTION: tm_jgps_trans_req_op_mode

DESCRIPTION:
  This function translates the external operation mode to internal
  TM jGPS operational mode.

PARAMETERS:
  
===========================================================================*/
tm_jgps_sess_req_mode_e_type tm_jgps_trans_sess_req_op_mode
( 
  tm_1x_up_smod_curr_op_mode_e_type  mode
)
{
  tm_jgps_sess_req_mode_e_type ret_mode = TM_JGPS_SESS_REQ_MODE_NONE;

  MSG_MED("mode = %d",mode,0,0);

  switch (mode)
  {
  case TM_1X_UP_SMOD_CURR_OP_MODE_MSA:
    ret_mode = TM_JGPS_SESS_REQ_MODE_MSA;
    break;
  case TM_1X_UP_SMOD_CURR_OP_MODE_DLOAD:
  case TM_1X_UP_SMOD_CURR_OP_MODE_MSB:
    ret_mode = TM_JGPS_SESS_REQ_MODE_MSB_DLOAD;
    break;
  default:
    ret_mode = TM_JGPS_SESS_REQ_MODE_NONE;
    break;
  }

  return ret_mode;
} /* tm_jgps_trans_req_op_mode */


/*===========================================================================

FUNCTION: tm_1x_up_smod_start_sess_req

DESCRIPTION:
  This function is called by IS-801 Driver(1x UP) to Japan-GPS sub-module
  to see if a request to start an IS-801 session should be allowed or not

  If the incoming session req is granted, a 1x UP protocol sub-module 
  should return 1X_UP_PRTL_REQ_STATUS_SUCCESS and 
  populate the "reason" pointer to tell IS-801 drv either grant or wait.

  If a protocol sub-module needs authenticate the session, it should return 
  TM_1X_UP_SMOD_REQ_STATUS_WAIT, to put IS-801 Drv in wait state.
  Once the protocol sub-module (Vx or JGPS/KDDI) it authenticates the session, 
  it should send a asynchronous response back to IS-801 Driver 
  by calling tm_1x_up_post_event() func to complete the session request.
  
  If for any reason, a protocol sub-module does not allow the session request, 
  it should return TM_1X_UP_SMOD_REQ_FAIL with req_status pointer set the 
  valid stop reason status. 

PARAMETERS:
  
===========================================================================*/
tm_1x_up_smod_req_result_e_type tm_jgps_smod_start_sess_req
(
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_1x_up_smod_curr_op_mode_e_type  curr_mode,
  tm_1x_up_smod_req_status_e_type    *req_status_ptr
)
{
  /* Local Variables*/
  tm_1x_up_smod_req_result_e_type retval = TM_1X_UP_SMOD_REQ_FAIL;
  
  tm_jgps_sess_info_s_type *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  do{
    /*----------------------------------------------------------------------
      Check input parameters
    -----------------------------------------------------------------------*/
    if( start_sess_req_param == NULL || req_status_ptr == NULL )
    {
      MSG_ERROR("Invalid input params, fail start req", 0,0,0);
      break;
    }
      
    /*----------------------------------------------------------------------
      Translate the request mode
    -----------------------------------------------------------------------*/
    sess_info_ptr->req_mode = tm_jgps_trans_sess_req_op_mode(curr_mode);
  
    if ( sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_NONE)
    {
      MSG_ERROR("Invalid Session request mode (%d)", sess_info_ptr->req_mode, 0,0);
       /* Status set to error unknown */
      *req_status_ptr = TM_1X_UP_SMOD_REQ_ERR_INVALID_MODE;
      break;
    }
    
    /*----------------------------------------------------------------------
     Store the start request param and current mode 
    -----------------------------------------------------------------------*/
    memscpy((void *)&(sess_info_ptr->smod_req_param), sizeof(sess_info_ptr->smod_req_param),
           start_sess_req_param, sizeof(*start_sess_req_param) );
  
  
    /*----------------------------------------------------------------------
     Send the start session request to JGPS core
    -----------------------------------------------------------------------*/
    if( tm_jgps_core_start_sess_req_handler() )
    {
      /* Status set to wait for Sub-module response */
      *req_status_ptr = TM_1X_UP_SMOD_REQ_STATUS_WAIT;
  
      /* Return Success */
      retval = TM_1X_UP_SMOD_REQ_SUCCESS;
    }
    else
    {
      /* Status set to error unknown */
      *req_status_ptr = TM_1X_UP_SMOD_REQ_ERR_UNKNOWN;
      
      /* Return Error */
      retval = TM_1X_UP_SMOD_REQ_FAIL;
    } 
  } while (0);

  
  return retval;
} /* tm_jgps_start_sess_req */


/*===========================================================================

FUNCTION: tm_jgps_smod_stop_sess_req

DESCRIPTION:
  This function is called by IS-801 Driver to protocol sub-module to stop
  current session.
  

PARAMETERS:
  
===========================================================================*/
tm_1x_up_smod_req_result_e_type tm_jgps_smod_stop_sess_req
(  
  pdsm_pd_end_e_type                  end_status,
  pdsm_pd_dload_data_s_type           *dload_info_ptr,
  tm_1x_up_smod_curr_op_mode_e_type   curr_mode,
  tm_1x_up_smod_req_status_e_type     *req_status_ptr
)
{
  /* Local Variables*/
  tm_1x_up_smod_req_result_e_type retval = TM_1X_UP_SMOD_REQ_FAIL;
  tm_jgps_sess_req_mode_e_type smod_req_mode =  TM_JGPS_SESS_REQ_MODE_NONE;
  tm_sess_stop_reason_e_type stop_reason;

  tm_jgps_sess_info_s_type *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  do{
    /*----------------------------------------------------------------------
      Check input parameters
    -----------------------------------------------------------------------*/
    if(req_status_ptr == NULL || dload_info_ptr == NULL)
    {
      MSG_ERROR("Invalid input params, fail stop req", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
      Translate the request mode
    -----------------------------------------------------------------------*/
    smod_req_mode = tm_jgps_trans_sess_req_op_mode(curr_mode);

    if ( smod_req_mode == TM_JGPS_SESS_REQ_MODE_NONE)
    {
      MSG_HIGH("End current session (curr mode: %d)", curr_mode, 0,0);

      /* Stop any ongoing sessions */
      tm_jgps_core_prtl_stop_sess_req_handler(TM_STOP_REASON_COMPLETED,
                          PRTL_SESS_STOP_SINGLE_FIX);

        /* Status set to error unknown */
        *req_status_ptr = TM_1X_UP_SMOD_REQ_ERR_INVALID_MODE;
        break;
    }

    /* In case of failure due to connectivity issues due to wrong IP addr/port, curr mode is DLOAD but req_mode is not initlialized */
    if(((curr_mode == TM_1X_UP_SMOD_CURR_OP_MODE_DLOAD) ||
        (curr_mode == TM_1X_UP_SMOD_CURR_OP_MODE_MSA)) && 
       (sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_NONE))
    {
      MSG_HIGH("End current session (curr mode: %d)", curr_mode, 0,0);
      stop_reason = tm_jgps_core_translate_end_status(end_status);
      /* Stop any ongoing sessions */
      tm_jgps_core_prtl_stop_sess_req_handler(stop_reason,
                                              PRTL_SESS_STOP_SINGLE_FIX);

       /* Status set to error unknown */
      *req_status_ptr = TM_1X_UP_SMOD_REQ_ERR_INVALID_MODE;
      break;
    }

    if(sess_info_ptr->req_mode != smod_req_mode)
    {
      MSG_ERROR("Sess Req mode(%d) does not match current session mode (%d)",
                smod_req_mode,
                sess_info_ptr->req_mode,
                0);
      break;
    }
    
    /*----------------------------------------------------------------------
     Send the stop session request to JGPS core
    -----------------------------------------------------------------------*/
    if( tm_jgps_core_stop_sess_req_handler(end_status, dload_info_ptr) )
    {
      /* Status set to wait for Sub-module response */
      *req_status_ptr = TM_1X_UP_SMOD_REQ_STATUS_GRANT;

      /* Return Success */
      retval = TM_1X_UP_SMOD_REQ_SUCCESS;
    }
    else
    {
      /* Status set to error unknown */
      *req_status_ptr = TM_1X_UP_SMOD_REQ_ERR_UNKNOWN;

      /* Return Error */
      retval = TM_1X_UP_SMOD_REQ_FAIL;
    } 
  } while (0);


  return retval;

} /* tm_jgps_smod_stop_sess_req */

/*===========================================================================

FUNCTION: tm_jgps_smod_info_available

DESCRIPTION:
  This function is called by IS-801 Driver to provide information to 
  1x UP sub-modules.  In most cases this is the LR data.

   
PARAMETERS:
  
===========================================================================*/
tm_1x_up_smod_req_result_e_type tm_jgps_smod_info_available
(
  pdsm_pd_info_s_type                *smod_pd_info_ptr,
  pdsm_pd_end_e_type                 smod_end_status, 
  tm_1x_up_smod_req_status_e_type    *req_status_ptr
)
{
  /* Local Variables*/
  tm_1x_up_smod_req_result_e_type retval = TM_1X_UP_SMOD_REQ_FAIL;

  do{
    /*----------------------------------------------------------------------
      Translate the request mode
    -----------------------------------------------------------------------*/
    if( smod_pd_info_ptr == NULL || req_status_ptr == NULL )
    {
      MSG_ERROR("Invalid input params, fail info available", 0,0,0);
      break;
    }

    /*----------------------------------------------------------------------
     Send the start session request to JGPS core
    -----------------------------------------------------------------------*/
    if( tm_jgps_core_info_available_handler(smod_pd_info_ptr) )
    {
      /* Status set to wait for Sub-module response */
      *req_status_ptr = TM_1X_UP_SMOD_REQ_STATUS_WAIT;

      /* Return Success */
      retval = TM_1X_UP_SMOD_REQ_SUCCESS;
    }
    else
    {
      /* Status set to error unknown */
      *req_status_ptr = TM_1X_UP_SMOD_REQ_ERR_UNKNOWN;

      /* Return Error */
      retval = TM_1X_UP_SMOD_REQ_FAIL;
    } 
  } while (0);


  return retval;

} /* tm_jgps_smod_info_available */


/*===========================================================================

FUNCTION: tm_jgps_smod_data_pipe_req

DESCRIPTION:
  This function is called by IS-801 Driver to request that protocol sub-module
  (Vx or JGPS/KDDI) to append any specific header information before sending 
  the data on the TCP/IP connection

   
PARAMETERS:
  
===========================================================================*/
int tm_jgps_smod_data_pipe_req
(
  boolean   close_flag,
  byte      *data_ptr,
  int16     data_ptr_len
)
{
  /*----------------------------------------------------------------------
     Call the JGPS core function to handle the data pipe request
  -----------------------------------------------------------------------*/
  return tm_jgps_core_data_pipe_req_handler(close_flag,
                                            data_ptr, 
                                            data_ptr_len);
} /* tm_jgps_smod_data_pipe_req */



/*=============================================================================

       Extern Functions: Interface with TM-Core (TM-Core => JGPS)

=============================================================================*/


/*===========================================================================

FUNCTION tm_jgps_prtl_start_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  prtl_start_sess_req_result_e_type

SIDE EFFECTS
  None.

===========================================================================*/
prtl_start_sess_req_result_e_type tm_jgps_prtl_start_sess_req_handler
(
  tm_sess_handle_type                     sess_handle,
  prtl_start_sess_req_param_s_type        *start_sess_req_param,
  tm_sess_req_start_param_s_type          *actions
)
{
  /*-----------------------------------------------------------------------
     Check parameters
  -----------------------------------------------------------------------*/
  if(start_sess_req_param == NULL)
  {
    MSG_ERROR("Null start_sess_req_param", 0, 0, 0);
    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }

  if( (start_sess_req_param->client_type <= PDSM_CLIENT_TYPE_NONE)
    ||(start_sess_req_param->client_type >= PDSM_CLIENT_TYPE_MAX))
  {
    MSG_ERROR("invalid client type", 0, 0, 0);
    return PRTL_START_SESS_REQ_NOT_ALLOWED;  
  }
  
  /*-----------------------------------------------------------------------
     Check TM-JGPS parameters
  -----------------------------------------------------------------------*/
  return  tm_jgps_core_prtl_start_sess_req_handler(
                                                   sess_handle, 
                                                   start_sess_req_param,
                                                   actions
                                                  );
  
} /* tm_jgps_prtl_start_sess_req_handler */


/*===========================================================================

FUNCTION tm_jgps_prtl_stop_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_jgps_prtl_stop_sess_req_handler
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,  
  tm_sess_stop_reason_e_type    stop_reason
)
{
  boolean ret_val = TRUE;
  
  if (sess_handle == TM_SESS_HANDLE_NONE)
  {
    return ret_val;
  }
  ret_val = tm_jgps_core_prtl_stop_sess_req_handler(stop_reason, stop_type);
  
  return ret_val;
} /* tm_jgps_prtl_stop_sess_req_handler */


/*===========================================================================
FUNCTION tm_jgps_prtl_sess_req_data_handler

DESCRIPTION

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None.

===========================================================================*/
boolean tm_jgps_prtl_sess_req_data_handler
(
  tm_sess_handle_type  session_handle,  
  prtl_data_req_type   req_data
)
{
  return tm_1x_up_is801_drv_req_aiding_data(session_handle, req_data);
} /* tm_jgps_prtl_sess_req_data_handler */

/*===========================================================================
FUNCTION tm_jpgs_prtl_sess_info_handler

DESCRIPTION

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None.

===========================================================================*/
boolean tm_jgps_prtl_sess_info_handler

(
  tm_sess_handle_type           session_handle,
  prtl_sess_info_e_type         sess_info_type,
  prtl_sess_info_param_u_type   *sess_info_param
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
          
  /* Send info to JGPS core */
  ret_val = tm_jgps_core_prtl_sess_info_handler(sess_info_type, sess_info_param);

  /* Handle all other session info data */
  if (ret_val == FALSE)
  { 
    ret_val = tm_1x_up_is801_drv_sess_info_handler( session_handle,
                                                 sess_info_type,
                                                 sess_info_param );
  }

  return ret_val;

} /* tm_jpgs_prtl_sess_info_handler */

/*===========================================================================
FUNCTION tm_jgps_prtl_data_pipe_handler

DESCRIPTION

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None

===========================================================================*/
boolean tm_jgps_prtl_data_pipe_handler
(
  pd_comms_client_ipc_message_type   msg_type,  
  void                               *msg_data_ptr
)
{
  boolean result;
  uint32  *tr_id_ptr;

  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Null msg_data_ptr", 0, 0, 0);
    return FALSE;
  }

  /*---------------------------------------------------------------------
      Based on transaction ID, service the PDCOMM data
  -----------------------------------------------------------------------*/
  tr_id_ptr = (uint32 *)msg_data_ptr;

  switch(*tr_id_ptr & TM_1X_UP_TRANSACTION_ID_DISPATCH_MASK)
  {
    case TM_1X_UP_IS801_DRV_TRANSACTION_ID_BASE:
      result = tm_jgps_core_prtl_data_pipe_handler(msg_type, msg_data_ptr);
      break;
    
    default:
      MSG_ERROR("Invalid transaction id received", 0, 0, 0);
      result = FALSE;
      break;
  }

  /*---------------------------------------------------------------------
      Forward the un-handled data to 1x UP (IS-801 Drv)
  -----------------------------------------------------------------------*/
  if(result == FALSE)
  {
    result = tm_1x_up_is801_drv_data_pipe_handler(msg_type, msg_data_ptr);
  }

  return result;

} /* tm_jgps_prtl_data_pipe_handler */


/*===========================================================================
FUNCTION tm_jgps_prtl_timer_cb_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_jgps_prtl_timer_cb_handler
(  
  void *timer_data
)
{
  os_TimerExpiryType *timer_param = (os_TimerExpiryType *)timer_data;

  /*---------------------------------------------------------------------
      Timer expires
  -----------------------------------------------------------------------*/
  if(timer_param != NULL)
  {
    switch(timer_param->q_TimerId & TM_1X_UP_TIMER_ID_DISPATCH_MASK)
    { 
      case TM_1X_UP_IS801_DRV_TIMER_ID_BASE:
        tm_1x_up_is801_drv_timer_handler(timer_data);
      break;
      
      case TM_1X_UP_KDDI_TIMER_ID_BASE:
        tm_jgps_core_prtl_timer_handler(timer_param);
      break;

      default:
        MSG_ERROR("Recv'd unknown timer expiry event", 0, 0, 0);
      break;
    }
  }
  else
  {
    MSG_ERROR("Can not dispatch null timer msg", 0, 0, 0);  
  }  
  

  return;

} /* tm_jgps_prtl_timer_cb_handler */


/*===========================================================================
FUNCTION tm_jgps_prtl_event_cb_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

  None.

===========================================================================*/
boolean tm_jgps_prtl_event_cb_handler
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *event_payload
)
{
  boolean ret_val = TRUE;
  
  if(event_payload == NULL)
  {
    MSG_ERROR("null event_payload pointer", 0, 0, 0);
    return FALSE;
  }

  /*---------------------------------------------------------------------
      Determine the correct handling for event
  -----------------------------------------------------------------------*/
  switch (event_type) 
  {
    case PRTL_EVENT_TYPE_PHONE:
      /* For phone events, pass it to is801 session driver module as well */
      tm_1x_up_is801_drv_event_handler(event_type, event_payload);      
      break;
    case PRTL_EVENT_TYPE_PA:
      /* Handle PA paramter for Application Information (i.e. URL, datum, accry, etc) */
      if(event_payload->pa_event_payload.pa_event == PDSM_PA_APP_INFO)
      {
        if(event_payload->pa_event_payload.pa_info_ptr != NULL)
        {
          if( tm_jgps_core_prtl_save_app_info( event_payload->pa_event_payload.pa_info_ptr) )
          {
             MSG_MED("Updated app info", 0,0,0);
          }
        }
        else
        {
          ret_val = FALSE;
        }
      }
      else
      {
        /* For PA events, pass it to is801 session driver module as well */
        tm_1x_up_is801_drv_event_handler(event_type, event_payload);
      }     
      break;
    default:
      MSG_MED("Ignore event type=%d event=%d",event_type,
              event_payload->pd_event_payload.pd_event,0);
      ret_val = FALSE;
      break;
  } /* end switch */
    
  return ret_val;
} /* tm_jgps_prtl_event_cb_handler */


/*===========================================================================
FUNCTION: tm_jgps_init_prtl_iface_tbl

DESCRIPTION:

PARAMETERS:

===========================================================================*/
void tm_jgps_init_prtl_iface_tbl(void)
{  
  tm_jgps_prtl_iface_table.start_sess_req_fp   = tm_jgps_prtl_start_sess_req_handler;   
  tm_jgps_prtl_iface_table.stop_sess_req_fp    = tm_jgps_prtl_stop_sess_req_handler;  
  tm_jgps_prtl_iface_table.sess_req_data_fp    = tm_jgps_prtl_sess_req_data_handler;  
  tm_jgps_prtl_iface_table.sess_info_fp        = tm_jgps_prtl_sess_info_handler;   
  tm_jgps_prtl_iface_table.data_pipe_handle_fp = tm_jgps_prtl_data_pipe_handler;   
  tm_jgps_prtl_iface_table.timer_cb_fp         = tm_jgps_prtl_timer_cb_handler;  
  tm_jgps_prtl_iface_table.event_cb_fp         = tm_jgps_prtl_event_cb_handler;
  tm_jgps_prtl_iface_table.general_query_fp    = NULL;
  tm_jgps_prtl_iface_table.gm_event_handle_fp  = NULL;
} /* tm_jgps_init_prtl_iface_tbl */

/*===========================================================================

FUNCTION tm_jgps_prtl_init

DESCRIPTION
  This function is called by TM-Core to initialize JGPS module
  
DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

  None.

===========================================================================*/
void tm_jgps_prtl_init(void)
{
  MSG_MED("tm_jgps_prtl_init", 0, 0, 0);

   /* Initialize JGPS internal Data Structure, 
      NV settings, prtl callback table, etc 
   */
  tm_jgps_core_init();

  /* Initialize TM-Core / JGPS function call back table */
  tm_jgps_init_prtl_iface_tbl();   
  
  /* Call IS801 drv init function */      
  tm_1x_up_is801_drv_init(); 

  /* Set up callback functions in TM-Core */
  (void)tm_prtl_reg(TM_PRTL_TYPE_1X_UP, &tm_jgps_prtl_iface_table);
} /* tm_jgps_prtl_init */

#endif /* FEATURE_JCDMA_GPSONE */
