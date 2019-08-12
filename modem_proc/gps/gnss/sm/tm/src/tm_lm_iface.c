/*=============================================================================

                        Transport Manager API Header File

GENERAL DESCRIPTION
  This file contains contains the structure definitions and function prototypes for 
  the Transport Manager software interface.

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_lm_iface.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/13    ah     Support extraction of MPG event from union tm_info_s_type
06/12/06    mr     Initial version.

=============================================================================*/


/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "tm_lm_iface.h"
#include "aries_os_api.h"
#include "msg.h"
#include "tm_data.h"

/*=============================================================================

       Function definitions

=============================================================================*/

/*=============================================================================

FUNCTION 
  tm_session_request

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  p_req - Pointer to the structure containing session related request from 
  LM to TM

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_session_request(const tm_session_request_s_type *p_req)
{
  os_IpcMsgType              *ipc_msg_ptr;
  tm_session_request_s_type  *ipc_data_ptr;
  
  ipc_msg_ptr = os_IpcCreate(sizeof(tm_session_request_s_type), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (tm_session_request_s_type *)ipc_msg_ptr->p_Data; /*lint !e826 */

    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_SESS_REQ_API;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(tm_session_request_s_type);
      
    *ipc_data_ptr = *p_req;

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);     
      (void)os_IpcDelete(ipc_msg_ptr);   
      return FALSE;
    }
    return TRUE; /*lint !e506 */
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0); 
    return FALSE;
  } /* End ipc_msg_ptr */
}

/*=============================================================================

FUNCTION 
  tm_session_update

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  p_update - Pointer to the structure containing the session related update 
  from LM to TM

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_session_update(const tm_session_update_info_s_type   *p_update )
{
  os_IpcMsgType                  *ipc_msg_ptr;
  tm_session_update_info_s_type  *ipc_data_ptr;
  
  ipc_msg_ptr = os_IpcCreate(sizeof(tm_session_update_info_s_type), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (tm_session_update_info_s_type *)ipc_msg_ptr->p_Data; /*lint !e826 */

    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_SESS_UPDATE_API;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(tm_session_update_info_s_type);
      
    *ipc_data_ptr = *p_update;  

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);     
      (void)os_IpcDelete(ipc_msg_ptr);   
      return FALSE;
    }
    return TRUE; /*lint !e506 */
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0); 
    return FALSE;
  } /* End ipc_msg_ptr */
}
      
/*=============================================================================

FUNCTION 
  tm_info

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  p_info - Pointer to the structure containing the session independent info 
  from LM to TM

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_info(const tm_info_s_type   *p_info )
{ 
  os_IpcMsgType     *ipc_msg_ptr;
  tm_info_s_type    *ipc_data_ptr;
  
  ipc_msg_ptr = os_IpcCreate(sizeof(tm_info_s_type), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);
  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (tm_info_s_type *)ipc_msg_ptr->p_Data; /*lint !e826 */

    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_SESS_INFO_API;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(tm_info_s_type);
      
    *ipc_data_ptr = *p_info;  
    
    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);
      (void)os_IpcDelete(ipc_msg_ptr);
      return FALSE;
    }
    return TRUE; /*lint !e506 */
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0);
    return FALSE;
  } /* End ipc_msg_ptr */
}

/*=============================================================================

FUNCTION 
  tm_info_mgp_event

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  p_info - Pointer to the structure containing the session independent info 
  from LM to TM

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_info_mgp_event(const tm_info_mgp_event_s_type *p_info )
{ 
  os_IpcMsgType            *ipc_msg_ptr;
  tm_info_mgp_event_s_type *ipc_data_ptr;
  
  ipc_msg_ptr = os_IpcCreate(sizeof(tm_info_mgp_event_s_type), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);
  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (tm_info_mgp_event_s_type*)ipc_msg_ptr->p_Data; /*lint !e826 */

    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_SESS_INFO_API;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(tm_info_mgp_event_s_type);
      
    *ipc_data_ptr = *p_info;  
    
    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_ERROR("Failed to send IPC mesage",0,0,0);
      (void)os_IpcDelete(ipc_msg_ptr);
      return FALSE;
    }
    return TRUE; /*lint !e506 */
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0);
    return FALSE;
  } /* End ipc_msg_ptr */
}
