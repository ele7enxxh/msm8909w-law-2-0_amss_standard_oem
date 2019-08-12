/**
@file ds_profile_remote_client.c
@brief
This file defines implmentation of function, variables and data structures 
defined in ds_profile_remote_client.

@copyright (c)2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/src/ds_profile_remote_client.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ds_Utils_DebugMsg.h"
#include "qmi_client.h"

#include "dcc_task_defs.h"
#include "dcc_task.h"
#include "dcc_task_svc.h"
#include "ds_profile_remote_client.h"

#ifdef FEATURE_8960_SGLTE_FUSION

/*===========================================================================

                         LOCAL DATA DECLARATIONS

===========================================================================*/

/**
  @brief WDS Client cookie
*/
#define DS_PROFILE_REMOTE_CLIENT_CB_DATA 0xFEAD
static int ds_profile_remote_client_cb_data = DS_PROFILE_REMOTE_CLIENT_CB_DATA;

/**
  @brief QMI Notifier handler
*/
static qmi_client_type notifier_handle = NULL;

static qmi_client_os_params          os_params;
static qmi_client_os_params          l_os_params;

/*===========================================================================

                         GLOBAL DATA DECLARATIONS

===========================================================================*/

/**
  @brief QMI Client handler
*/
qmi_client_type ds_profile_remote_client_handle = NULL;

/**
	@brief QMI WDS indication callback function. Currently we are not waiting  
         for indication from remote modem. All our calls are synchronous.
*/
static void  ds_profile_remote_client_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                          *ind_buf_ptr,
 unsigned int                   ind_buf_len,
 void                          *ind_cb_data
)
{

  LOG_MSG_INFO1_1("ds_profile_remote_client_ind_cb(): msg_id:%u", msg_id);

}/*ds_profile_remote_client_ind_cb()*/


/**
	@brief QMI WDS init error callback function
*/
static void ds_profile_remote_client_error_cb
(
  qmi_client_type user_handle,
  qmi_client_error_type qmi_error,
  void *err_cb_data
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO1_0("ds_profile_remote_client_error_cb(): ");
  
  /*---------------------------------------------------------------------
     Check if err cb data pointer is valid
  ---------------------------------------------------------------------*/
  if(err_cb_data == NULL)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_client_error_cb(): Null pointer "
                    "passed" );
    return;
  }
    
  /*---------------------------------------------------------------------
     Check if calback data is valid
  ---------------------------------------------------------------------*/
  if((int)(*((int*)err_cb_data)) != DS_PROFILE_REMOTE_CLIENT_CB_DATA)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_client_error_cb(): Invalid callback"
                    " data" );
    return;
  }  
    
  switch(qmi_error)
  {
    /*----------------------------------------------------------------------- 
      In case of service error, client will be released. No attempt to recover
      the QMI connection will be made.
    ------------------------------------------------------------------------*/
    case QMI_SERVICE_ERR:
      LOG_MSG_ERROR_0("ds_profile_remote_client_error_cb(): QMI WDS service "
                      "is down");
      /*----------------------------------------------------------------------- 
        Release the client. 
      ------------------------------------------------------------------------*/
      if(QMI_NO_ERR != qmi_client_release(ds_profile_remote_client_handle))
      {
        LOG_MSG_ERROR_0("ds_profile_remote_client_error_cb(): Client release "
                        "failed"); 
      }
    
      ds_profile_remote_client_handle = NULL;
      break;

    default:
      LOG_MSG_ERROR_1("ds_profile_remote_client_error_cb(): error=%ld",
                      qmi_error);
      break;
  }

  return;
}/*ds_profile_remote_client_error_cb()*/

/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_CLIENT_PROCESS_SIGNAL

DESCRIPTION
  Funtion to process WDS service notify signal. This function is used to 
  process signal sent to DS task by QCCI notifying WDS service.

PARAMETERS
  sig           :   Signal for which command handler was called
  user_data_ptr :   Optional User data sent
  
DEPENDENCIES 
  
RETURN VALUE 
  TRUE          :  For successful handling of signal
  FALSE         :  For any failure reason
  
SIDE EFFECTS 
 
===========================================================================*/
static boolean ds_profile_remote_client_process_signal
(
   dcc_sig_enum_type   sig,
   void              * user_data_ptr
)
{
  qmi_client_error_type                             retval;
  qmi_service_info                                  info;
  unsigned int                                      num_entries=1;
  unsigned int                                      num_services;
  qmi_idl_service_object_type                       wds_service_obj = NULL;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO1_0("ds_profile_remote_client_process_signal(): ");

  if( (sig & DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL) != 0 )
  {
    LOG_MSG_INFO2_0("ds_profile_remote_client_process_signal(): Clearing "
                    "DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL");
    rex_clr_sigs(rex_self(), DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL );
  }

  /*-----------------------------------------------------------------------
     If ds_profile_remote_client_handle!=NULL, then client_init aleady done. 
     The signal might be false signal. Return immediatley.
  -----------------------------------------------------------------------*/
  if(ds_profile_remote_client_handle != NULL)
  {
    LOG_MSG_ERROR_1("ds_profile_remote_client_process_signal(): False signal,"
                    " client handle: 0x%x not NULL. RETURN",
                    ds_profile_remote_client_handle);
    return TRUE;
  }

  /*-----------------------------------------------------------------------
     Get WDS service object
  -----------------------------------------------------------------------*/
  wds_service_obj = wds_get_service_object_v01();
  if(!wds_service_obj)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_client_process_signal(): Unable to get"
                    " WDS service object");
    return FALSE;
  }

  /*------------------------------------------------------------------
     Check if the WDS service is up, if not return. Else,
     Get addressing information for accessing wds service
  ------------------------------------------------------------------*/  
  retval = qmi_client_get_service_list(wds_service_obj,
                                       &info,
                                       &num_entries,
                                       &num_services);
                                       
  if(QMI_NO_ERR != retval)
  {
    LOG_MSG_ERROR_1("ds_profile_remote_client_process_signal(): Get "
                    "Service list err %d", retval);
    return FALSE;
  }
  
  LOG_MSG_INFO2_0("ds_profile_remote_client_process_signal(): QMI-WDS "
                  "Service up");

  /*----------------------------------------------------------------------- 
    Release the notifier handlle. QMI Service notify signal need not be  
    handled anymore.
    QCCI uses notify signal during power-up to signal the
    availability of QMI service. Once signal is processed, release the
    notifier_handle, so that QCCI will not send anymore signals.
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != qmi_client_release(notifier_handle))
  {
    LOG_MSG_ERROR_0("ds_profile_remote_client_process_signal(): Notifier release"
                    " failed");
    notifier_handle = NULL;
    return FALSE;
  }  

  /*-----------------------------------------------------------------------
       Set notifier handle to null since qmi_client_release doesnt do it
  ------------------------------------------------------------------------*/  
  notifier_handle = NULL; 

  /*-----------------------------------------------------------------------
       Initialize the arguments for QMI Client Init
  ------------------------------------------------------------------------*/
  memset(&l_os_params,0, sizeof(qmi_client_os_params));
  l_os_params.tcb = rex_self();
  l_os_params.sig = 
    (rex_sigs_type)(1 << DCC_DS_PROFILE_REMOTE_SEND_MSG_WAIT_SIGNAL);  
  l_os_params.timer_sig = 
    (rex_sigs_type)(1 << DCC_DS_PROFILE_REMOTE_OS_TIMER_SIGNAL); 
  
  /*----------------------------------------------------------------------- 
       Initialize connection to the WDS service
  -----------------------------------------------------------------------*/
  retval = qmi_client_init(&info,
                           wds_service_obj,
                           ds_profile_remote_client_ind_cb,
                           &ds_profile_remote_client_cb_data,
                           &l_os_params,
                           &ds_profile_remote_client_handle);

  if((retval != QMI_NO_ERR) || (!ds_profile_remote_client_handle))
  {
     LOG_MSG_ERROR_0("ds_profile_remote_client_process_signal(): QMI WDS client"
                     " init failed");
     ds_profile_remote_client_handle = NULL;
     return FALSE;
  }
  LOG_MSG_INFO2_0("QMI WDS client init successful");

  /*----------------------------------------------------------------------- 
      Register for Error callback
  ------------------------------------------------------------------------*/
  if (QMI_NO_ERR != qmi_client_register_error_cb(
                     ds_profile_remote_client_handle,
                     ds_profile_remote_client_error_cb,
                     &ds_profile_remote_client_cb_data
                   ))
  {
    LOG_MSG_ERROR_0("ds_profile_remote_client_process_signal(): QMI WDS "
                    "register error callback failed");
    return FALSE;
  }

  ds_profile_remote_perform_sync();
  
  return TRUE;
}/*ds_profile_remote_client_process_signal()*/

/*===========================================================================

                        EXTERNAL FUNCTION DEFINITION

===========================================================================*/


/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_CLIENT_INIT

DESCRIPTION
  This function Initializes the QMI-WDS client during DCC init function and  
  registers with QMI WDS service on remote modem.

PARAMETERS

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_client_init(void)
{
  qmi_client_error_type         retval;
  qmi_idl_service_object_type   wds_svc_obj=NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  LOG_MSG_INFO1_0("ds_profile_remote_client_init(): ");

  /*---------------------------------------------------------------------------
    This function will be called during powerup. Initialize notifier
    and client handles
  ---------------------------------------------------------------------------*/
  notifier_handle = NULL;
  ds_profile_remote_client_handle = NULL;
  
  /*---------------------------------------------------------------------------
   Get WDS service object
  ---------------------------------------------------------------------------*/
  wds_svc_obj = wds_get_service_object_v01();
  if(!wds_svc_obj)
  {
    LOG_MSG_ERROR_0("ds_profile_remote_client_init(): Unable to get WDS service"
                    " object");
    return;
  }
  LOG_MSG_INFO2_0("ds_profile_remote_client_init(): Get wds service object "
                  "success");
                
  (void)dcc_set_sig_handler( DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL,
                             ds_profile_remote_client_process_signal,
                             NULL );
                             
  dcc_enable_sig(DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL);

  /*---------------------------------------------------------------------------
    Initialize OS Signal required by QCCI to notify DCC task when WDS
    service comes up 
  ---------------------------------------------------------------------------*/
  memset(&os_params,0,sizeof(qmi_cci_os_signal_type));
  os_params.tcb = rex_self();
  os_params.sig = 
    (rex_sigs_type)(1 << DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL); 
  
  /*---------------------------------------------------------------------------
    Send QMI notifier init
  ---------------------------------------------------------------------------*/
  retval = qmi_client_notifier_init(wds_svc_obj,
                                    &os_params,
                                    &notifier_handle);
  if((QMI_NO_ERR != retval) || (!notifier_handle))
  {
    LOG_MSG_ERROR_1("ds_profile_remote_client_init(): Error during "
                    "qmi_client_notifier_init error=%d", retval);
    notifier_handle = NULL;
    return;
  }

  LOG_MSG_INFO2_0("ds_profile_remote_client_init(): Initialization successful");
  return;
}/* ds_profile_remote_client_init() */

#else  /* ifndef FEATURE_8960_SGLTE_FUSION */

/*===========================================================================
FUNCTION DS_PROFILE_REMOTE_CLIENT_INIT

DESCRIPTION
  This function Initializes the QMI-WDS client during DCC init function and  
  registers with QMI WDS service on remote modem.

PARAMETERS

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void ds_profile_remote_client_init(void)
{
  LOG_MSG_INFO1_0("ds_profile_remote_client_init(): Not supported"); 
  return;
}

#endif /* FEATURE_8960_SGLTE_FUSION */
