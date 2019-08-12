/*======================================================================

                         PDCOMM client API definitions file

 GENERAL DESCRIPTION
  This file contains API definitions by which PDCOMM services can 
  be accessed by clients (e.g Transport Manager) needing user-plane 
  connections. 

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/src/pd_comms_api.c#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 09/03/12   rh   Added inject msg PDCOMM API
 06/02/08   kj   Compensate for de-featurization of the Public API header file 
 04/04/06   mr   Initial creation of header file.
======================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "pd_comms_api.h"
#include "pd_comms_client_api.h"
#include "pd_comms_nv_efs.h"
#include "tm_pdapi_client.h"


#ifdef FEATURE_CGPS_PDCOMM
static pd_comms_register_functions_type p_registered_services[LINK_TYPE_MAX];
/*generate_hash_function is independent of transport type and hence is stored
  seperately*/
static pd_comms_app_generate_hash_type *generate_hash_func;   
#endif /* FEATURE_CGPS_PDCOMM */

#ifdef FEATURE_CGPS_LBS_TASK
/* A PD Comms client can register a callback function to receive
PD Comms Client API notifications (instead of via os_IpcSend()).
This functionality was added for remote clients i.e. on app processor 
since os_IpcSend() does not work across processor boundaries. */

ipc_func_table_entry ipc_func_table[MAX_IPC_CB_TABLE_SIZE];
#endif

/****************** Function Definitions**************************************/

/*===========================================================================
FUNCTION pd_comms_app_get_handle

DESCRIPTION
  This function returns a new pdcomm handle. A handle will be allocated by the 
  pd comms app task. This handle must be used in all other API functions to 
  identify the connection.

DEPENDENCIES
  None

RETURN VALUE
 pd_comms_handle_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_get_handle(
  /*get_handle parameters*/  
  const pd_comms_get_handle_params_type* const p_params,
  /*contains the handle allocated by pdcomm-task */
  pd_comms_handle_type* p_handle
)
{
#ifdef FEATURE_CGPS_PDCOMM
  if(p_params == NULL || p_handle == NULL)
  {
    return PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  
  if(p_params->e_link_type >= LINK_TYPE_MAX )
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }

  if(p_registered_services[(uint32)p_params->e_link_type].get_handle_func)
  {
    return p_registered_services[(uint32)p_params->e_link_type].get_handle_func
                               (p_params,p_handle);
  }
  else
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM */
}/*pd_comms_app_get_handle() ends*/

/*===========================================================================
FUNCTION pd_comms_app_connect

DESCRIPTION
  This function opens a new connection. A handle will be allocated by the 
  application layer. This handle must be used in all other API functions 
  to identify the connection.


DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_connect( 
  /*handle corresponding to the connection to be opened*/  
  const pd_comms_handle_type z_handle,  
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*Pointer to structure containing connect parameters*/
  const pd_comms_connect_params_type * const p_params
)
{
#ifdef FEATURE_CGPS_PDCOMM
  if(p_params == NULL)
  {
    return PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }  
  
  if(p_params->e_link_type >= LINK_TYPE_MAX )
  {
	return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }

  if(p_registered_services[(uint32)p_params->e_link_type].connect_func)
  {
	return p_registered_services[(uint32)p_params->e_link_type].connect_func
		                       (z_handle, q_source_thread_id, q_base_msg_id, p_params);
  }
  else
  {
	return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM */

}/*pd_comms_app_connect() ends*/

/*===========================================================================
FUNCTION pd_comms_app_disconnect

DESCRIPTION
  This function closes the connection associated with the given handle. 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_disconnect(
  /*handle corresponding to the connection to be closed*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*transaction ID*/
  const uint32 q_transaction_id,
  /*to abort or gracefully close*/
  const boolean b_abort
)
{
#ifdef FEATURE_CGPS_PDCOMM
  if(z_handle.e_link_type >= LINK_TYPE_MAX )
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }

  if(p_registered_services[(uint32)z_handle.e_link_type].disconnect_func)
  {
    return p_registered_services[(uint32)z_handle.e_link_type].disconnect_func
  							                  (z_handle, q_source_thread_id,
                                               q_base_msg_id,
                                               q_transaction_id, b_abort);
  }
  else
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM */
  
}/*pd_comms_app_disconnect() ends*/

/*===========================================================================
FUNCTION pd_comms_app_bind

DESCRIPTION
  This function binds to a socket for a given handle 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_bind(
  /*handle corresponding to the connection to be closed*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*Pointer to structure containing connect parameters*/
  const pd_comms_bind_params_type * const p_params
)
{
#ifdef FEATURE_CGPS_PDCOMM
#ifndef FEATURE_UDP_BIND_DISABLE
  if(z_handle.e_link_type >= LINK_TYPE_MAX )
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }

  if(p_registered_services[(uint32)z_handle.e_link_type].bind_func)
  {
    return p_registered_services[(uint32)z_handle.e_link_type].bind_func
  							                  (z_handle, q_source_thread_id,
                                               q_base_msg_id,p_params);
  }
  else
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }
#else /*FEATURE_UDP_BIND_DISABLE*/
  return PDCOMM_RESULT_OK;
#endif /*FEATURE_UDP_BIND_DISABLE*/
#else /* FEATURE_CGPS_PDCOMM */
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM */
  
}/*pd_comms_app_disconnect() ends*/


/*===========================================================================
FUNCTION pd_comms_app_write

DESCRIPTION
  This function writes data to the specified connection. 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_write( 
  /*handle corresponding to the connection to be written to.*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*write parameters corresponding to the link type*/
  const pd_comms_write_params_type * const p_params
)
{
#ifdef FEATURE_CGPS_PDCOMM
  if(p_params == NULL)
  {
    return PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }    
  
  if(z_handle.e_link_type >= LINK_TYPE_MAX )
  {
	return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }

  if(p_registered_services[(uint32)z_handle.e_link_type].write_func)
  {
    return p_registered_services[(uint32)z_handle.e_link_type].write_func
  			     (z_handle, q_source_thread_id, q_base_msg_id, p_params);
  }
  else
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM */
  
}/*pd_comms_app_write() ends*/

/*===========================================================================
FUNCTION pd_comms_app_read

DESCRIPTION
  This function reads data from a specified connection. 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_read( 
  /*handle corresponding to the connection to be read from*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*read parameters corresponding to the link type*/
  const pd_comms_read_params_type * const p_params
)
{
#ifdef FEATURE_CGPS_PDCOMM
  if(p_params == NULL)
  {
    return PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }      
  
  if(z_handle.e_link_type >= LINK_TYPE_MAX )
  {
	return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }

  if(p_registered_services[(uint32)z_handle.e_link_type].read_func)
  {
    return p_registered_services[(uint32)z_handle.e_link_type].read_func
  			     (z_handle, q_source_thread_id, q_base_msg_id, p_params);
  }
  else
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM */

}/*pd_comms_app_read() ends*/

/*===========================================================================
FUNCTION pd_comms_app_generate_hash

DESCRIPTION
  This function generates a hash based upon a given data payload. 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_generate_hash(
  /*thread id of the client*/  
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*parameters for the message on which hash function is to be applied
   The following fields are included in the hash_params structure: 
     hash_type - Hash algorith to use
     msg - Pointer to data payload upon which the hash shall be generated
     msg_len - length in bytes of the data payload
     hash_output - Pointer to data buffer in which to store the generated hash*/
  const pd_comms_generate_hash_params_type* const p_hash_params
)
{
#ifdef FEATURE_CGPS_PDCOMM
  if(generate_hash_func)
  {
    return generate_hash_func(q_source_thread_id, q_base_msg_id, p_hash_params);
  }
  else
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM */
  
}/*pd_comms_app_generate_hash() ends*/


/*===========================================================================
FUNCTION pd_comms_app_ioctl

DESCRIPTION
  This function provides get/set I/O parameter functionality. The exact I/O option
  depends upon the specific kind of transport/data service being used.
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_ioctl(
  /*handle corresponding to the connection for which ioctl is needed*/
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/ 
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*ioctl parameters*/
  const pd_comms_ioctl_params_type *const p_params
)
{
#ifdef FEATURE_CGPS_PDCOMM
  if(z_handle.e_link_type >= LINK_TYPE_MAX )
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }

  if(p_registered_services[(uint32)z_handle.e_link_type].ioctl_func)
  {
    return p_registered_services[(uint32)z_handle.e_link_type].ioctl_func
  					 (z_handle, q_source_thread_id, q_base_msg_id, p_params);
  }
  else
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM */

}/*pd_comms_app_ioctl() ends*/

/*===========================================================================
FUNCTION pd_comms_app_release_handle

DESCRIPTION
  This function releases a pdcomm handle previously allocated by calling
  pd_comms_app_get_handle(). If the connection is in CONNECTED state, this call
  will also abort the connection before releasing handle.
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_release_handle(
  /*contains the handle allocated by pdcomm-task */
  const pd_comms_handle_type* p_handle)
{
#ifdef FEATURE_CGPS_PDCOMM
  pd_comms_return_type retval;
#ifdef FEATURE_CGPS_LBS_TASK
  int i;
#endif

  if(p_handle == NULL)
  {
    return PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }        
  
  if(p_handle->e_link_type >= LINK_TYPE_MAX)
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }

  if(p_registered_services[(uint32)p_handle->e_link_type].release_handle_func)
  {
    retval = p_registered_services[(uint32)p_handle->e_link_type].release_handle_func
      (p_handle);
#ifdef FEATURE_CGPS_LBS_TASK
    for(i = 0; i < MAX_IPC_CB_TABLE_SIZE; i++)
    {
      if( (ipc_func_table[i].occupied == TRUE) && 
        (ipc_func_table[i].z_handle.q_handle_id == p_handle->q_handle_id) &&
        (ipc_func_table[i].z_handle.e_link_type == p_handle->e_link_type) )
      {
        ipc_func_table[i].occupied = FALSE;
        ipc_func_table[i].ipc_send_func = NULL;
      }
    }
#endif

    return retval;  
  }
  else
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM */
  
}/*pd_comms_app_release_handle() ends*/

/*===========================================================================
FUNCTION pd_comms_app_srv_sys_change_notification

DESCRIPTION
  This function handles the serving system change notification
  
DEPENDENCIES
  None

RETURN VALUE
 void

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_app_srv_sys_change_notification(sys_sys_mode_e_type srv_system)
{
#ifdef FEATURE_CGPS_PDCOMM
  int link_type = 0;

  for(; link_type < LINK_TYPE_MAX; link_type++)
  {
    if(p_registered_services[link_type].srv_system_change_notification_handle_func)
    {
      (void)p_registered_services[link_type].srv_system_change_notification_handle_func(srv_system);
    }
  }
#endif
}

/*===========================================================================
FUNCTION pd_comms_app_apn_profile_change_notification

DESCRIPTION
  This function handles the apn profile change notification
  
DEPENDENCIES
  None

RETURN VALUE
 void

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_app_apn_profile_change_notification(uint32 apn_profile_type)
{
#ifdef FEATURE_CGPS_PDCOMM
  int link_type = 0;

  for(; link_type < LINK_TYPE_MAX; link_type++)
  {
    if(p_registered_services[link_type].p_apn_profile_change_notification_handle_func)
    {
      (void)p_registered_services[link_type].p_apn_profile_change_notification_handle_func(apn_profile_type);
    }
  }
#endif
}


/*===========================================================================
FUNCTION pd_comms_set_apn_profiles

DESCRIPTION
  This function shall be used to set the apn profiles. NV shall be updated
  to reflect the change.
  
DEPENDENCIES
  None

RETURN VALUE
 void

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_set_apn_profiles(pdsm_client_id_type client_id, uint32 apn_profile_type, pdsm_pa_apn_profiles_type *apn_profile_p)
{
#ifndef FEATURE_GNSS_BIT_API
  cgps_nv_efs_reg_item_type item = CGPS_NV_EFS_MAX_NUM_ITEMS;

  
  /*Write the apn profiles into the nv*/
  if(apn_profile_type == PDSM_PA_LBS_APN_PROFILE)
  {
    item = CGPS_NV_EFS_PDCOMM_LBS_APN_PROFILES;
  }
  else if( apn_profile_type == PDSM_PA_XTRA_APN_PROFILE)
  {
    item = CGPS_NV_EFS_PDCOMM_XTRA_APN_PROFILES;
  }
  else if( apn_profile_type == PDSM_PA_EMERGENCY_LBS_APN_PROFILE)
  {
  	item = CGPS_NV_EFS_PDCOMM_EMERGENCY_LBS_APN_PROFILES;
  }
  else
  {
    MSG_ERROR("Apn profile type %d not supported",apn_profile_type,0,0);
  }

  if((CGPS_NV_EFS_MAX_NUM_ITEMS != item) &&
     (FALSE != pd_comms_nv_efs_reg_item_write(
                 item, 
                 sizeof(pdsm_pa_apn_profiles_type) * PDSM_PA_MAX_APN_PROFILES,
                 apn_profile_p)))
  {
    /*Send PA event callback to PDAPI clients*/
    pdsm_pa_info_s_type pa_info_data;
    
    MSG_HIGH("Wrote the APN profiles to NV",0,0,0);

    pa_info_data.client_id  = client_id;
    
    pa_info_data.pa_event = (pdsm_pa_e_type)apn_profile_type;
    memscpy((void*)&(pa_info_data.pa_info.apn_profiles[0]), sizeof(pa_info_data.pa_info.apn_profiles),
		    (void*)apn_profile_p, sizeof(pa_info_data.pa_info.apn_profiles));
    
    tm_pdapi_pa_event_callback(&pa_info_data);  
  }

  /*Let the interested pdcomm modules know about this nv change*/
  pd_comms_app_apn_profile_change_notification(apn_profile_type);
#else
  MSG_ERROR("APN profiles NV are not supported when BIT API is enabled",0,0,0);
#endif /* FEATURE_GNSS_BIT_API */
  
}

/*===========================================================================
FUNCTION pd_comms_get_apn_profiles

DESCRIPTION
  This function shall be used to get the apn profiles. 
  
DEPENDENCIES
  None

RETURN VALUE
 void

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_get_apn_profiles(uint32 apn_profile_type, pdsm_pa_apn_profiles_type *apn_profile_p)
{
#ifndef FEATURE_GNSS_BIT_API
  cgps_nv_efs_reg_item_type item = CGPS_NV_EFS_MAX_NUM_ITEMS;

  /*Write the apn profiles into the nv*/
  if(apn_profile_type == PDSM_PA_LBS_APN_PROFILE)
  {
    item = CGPS_NV_EFS_PDCOMM_LBS_APN_PROFILES;
  }
  else if( apn_profile_type == PDSM_PA_XTRA_APN_PROFILE)
  {
    item = CGPS_NV_EFS_PDCOMM_XTRA_APN_PROFILES;
  }
  else if( apn_profile_type == PDSM_PA_EMERGENCY_LBS_APN_PROFILE)
  {
    item = CGPS_NV_EFS_PDCOMM_EMERGENCY_LBS_APN_PROFILES;
  }  
  else
  {
    MSG_ERROR("Apn profile type %d not supported",apn_profile_type,0,0);
  }

  if(CGPS_NV_EFS_MAX_NUM_ITEMS != item)
  {
    (void)pd_comms_nv_efs_reg_item_read(
            item, 
            sizeof(*apn_profile_p) * PDSM_PA_MAX_APN_PROFILES, 
            apn_profile_p, FALSE); 
  }
#else
  MSG_ERROR("APN profiles NV are not supported when BIT API is enabled",0,0,0);  
#endif /* FEATURE_GNSS_BIT_API */
}


/*===========================================================================
FUNCTION pd_comms_app_reg_ipc_cb

DESCRIPTION
This function is called by PDCOMM APP to register a callback to be used by
PDCOMMS to inform the app of PDCOMM events. 

A PD Comms client can register a callback function to receive
PD Comms Client API notifications (instead of via os_IpcSend()).
This functionality was added for remote clients i.e. on app processor 
since os_IpcSend() does not work across processor boundaries.

DEPENDENCIES
None

RETURN VALUE
pd_comms_return_type 

SIDE EFFECTS
None
===========================================================================*/
pd_comms_return_type pd_comms_app_reg_ipc_cb(
  /*contains the callback for pdcomms to inform the client about events */
  const pd_comms_handle_type* p_handle,
  pd_comms_app_ipc_send_type *ipc_send_func
  )
{
#ifdef FEATURE_CGPS_LBS_TASK
  int i = 0;

  if ((p_handle == NULL) || (ipc_send_func == NULL))
  {
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }

  for(i = 0; (i < MAX_IPC_CB_TABLE_SIZE) && (ipc_func_table[i].occupied == TRUE); i++)
    ;

  if(i < MAX_IPC_CB_TABLE_SIZE)
  {
    ipc_func_table[i].occupied      = TRUE;
    ipc_func_table[i].z_handle      = *p_handle;
    ipc_func_table[i].ipc_send_func = ipc_send_func;
    return PDCOMM_RESULT_OK;
  }
  else
  {
    return PDCOMM_RESULT_ERROR_NO_MORE_HANDLES;
  }
#else /* FEATURE_CGPS_LBS_TASK */
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif
}

/*===========================================================================
FUNCTION pd_comms_register_service

DESCRIPTION
  This function is called by PDCOMM APP to register/de-register communication 
  services it offers for a particular transport type. If a functions mask is set
  and the corresponding function pointer is passed as NULL, the function will
  be de-registered from PDCOMM layer.
  
DEPENDENCIES
  None

RETURN VALUE
 boolean - Registration successfull or not

SIDE EFFECTS
  None
===========================================================================*/
boolean pd_comms_register_service(
  const pd_comms_register_service_type *p_reg,
  pd_comms_app_get_handle_type *p_get_handle_func, 
  pd_comms_app_connect_type *p_connect_func, 
  pd_comms_app_disconnect_type *p_disconnect_func,  
  pd_comms_app_bind_type *p_bind_func,
  pd_comms_app_write_type *p_write_func,
  pd_comms_app_generate_hash_type *p_generate_hash_func,
  pd_comms_app_ioctl_type *p_ioctl_func,
  pd_comms_app_read_type *p_read_func,
  pd_comms_app_release_handle_type *p_release_handle_func,
  pd_comms_app_srv_sys_change_handle_type *p_srv_system_change_notification_handle_func,
  pd_comms_app_set_data_enable_type *p_set_data_enable_func,
  pd_comms_app_apn_profile_change_handle_type *p_apn_profile_change_notification_handle_func,
  pd_comms_app_inject_msg_type *p_inject_msg_func
)
{
#ifdef FEATURE_CGPS_PDCOMM
  uint32 q_service_index;

  if(p_reg == NULL)
  {
    return FALSE;
  }
  
  q_service_index = (uint32) p_reg->e_link_type;

  if(q_service_index >= (uint32) LINK_TYPE_MAX)
  {
    return FALSE;
  }

  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_GET_HANDLE)
  {
	p_registered_services[q_service_index].get_handle_func = p_get_handle_func;
  }

  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_CONNECT)
  {
	p_registered_services[q_service_index].connect_func = p_connect_func;
  }

  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_DISCONNECT)
  {
	p_registered_services[q_service_index].disconnect_func = p_disconnect_func;
  }

  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_WRITE)
  {
	p_registered_services[q_service_index].write_func = p_write_func;
  }

  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_GENERATE_HASH)
  {
	/*generate_hash_function is independent of transport type and hence is stored
    seperately*/
	generate_hash_func = p_generate_hash_func;
  }

  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_IOCTL)
  {
	p_registered_services[q_service_index].ioctl_func = p_ioctl_func;
  }

  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_READ)
  {
	p_registered_services[q_service_index].read_func = p_read_func;
  }

  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_RELEASE_HANDLE)
  {
	p_registered_services[q_service_index].release_handle_func = p_release_handle_func;
  }
  
  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_BIND)
  {
	p_registered_services[q_service_index].bind_func = p_bind_func;
  }

  
  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_SRV_SYSTEM_NOTIFICATION)
  {
	p_registered_services[q_service_index].srv_system_change_notification_handle_func = p_srv_system_change_notification_handle_func;
  }

  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_SET_DATA_ENABLE)
  {
	p_registered_services[q_service_index].set_data_enable_func= p_set_data_enable_func;
  }

  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_APN_PROFILE_CHANGE_NOTIFICATION)
  {
	p_registered_services[q_service_index].p_apn_profile_change_notification_handle_func = p_apn_profile_change_notification_handle_func;
  }

  if(p_reg->q_function_mask & (uint32)PD_COMMS_FUNC_INJECT_MSG)
  {  
    p_registered_services[q_service_index].inject_msg_func = p_inject_msg_func;  
  }
  
  return TRUE;
#else
  return FALSE;
#endif  /* FEATURE_CGPS_PDCOMM */

} /* pd_comms_register_service() ends*/


/*===========================================================================
FUNCTION pd_comms_app_set_data_enable

DESCRIPTION
  This function enables/disables data
  
DEPENDENCIES
  None

RETURN VALUE
 void

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_app_set_data_enable(boolean data_enable)
{
#ifdef FEATURE_CGPS_PDCOMM
  int link_type = 0;

  for(; link_type < LINK_TYPE_MAX; link_type++)
  {
    if(p_registered_services[link_type].set_data_enable_func)
    {
      (void)p_registered_services[link_type].set_data_enable_func(data_enable);
    }
  }
#endif
}

/*===========================================================================
FUNCTION pd_comms_app_inject_msg

DESCRIPTION
  This function is called by PDCOMM client to inject message into PDCOMM's
  read buffer. It's used by TM Core's SUPL module to inject external SMS to
  WMS Task, this is mainly for SUPL INIT NI SMS received from HLOS
 
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_inject_msg(
  /*contains the handle allocated by pdcomm-task */
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*Pointer to structure containing connect parameters*/
  const pd_comms_inject_msg_params_type * const p_params)
{
#ifdef FEATURE_CGPS_PDCOMM
  if(z_handle.e_link_type >= LINK_TYPE_MAX )
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }

  if(p_registered_services[(uint32)z_handle.e_link_type].inject_msg_func)
  {
    return p_registered_services[(uint32)z_handle.e_link_type].inject_msg_func
  							                  (z_handle, q_source_thread_id,
                                               q_base_msg_id,p_params);
  }
  else
  {
    return PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED;
  }
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM */
  
}/*pd_comms_app_inject_msg() ends*/


