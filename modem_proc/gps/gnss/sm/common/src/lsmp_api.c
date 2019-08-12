/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Location Session Manager and Protocols API Module

GENERAL DESCRIPTION
  This module defines API for LSMP module.

  
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/src/lsmp_api.c#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/14   ah      Added handling of Set Premium Services indication status
12/24/12   mj      Add support for new lsmp parm for SS/DSDS/DSDA/TSTS devices
07/30/12   mj      Initial file creation and added API for premium services NV

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"     /* Definition for basic types and macros */

#include <string.h>     /* Interface to string library */
#include <stdlib.h>
#include "aries_os_api.h"
#include "tm_api.h"
#include "lsmp_api.h"

/* <EJECT> */
/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global definitions for constants, macros, types,
variables

===========================================================================*/



/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/



/*===========================================================================

FUNCTION lsmp_get_param

DESCRIPTION
    Request to read param value from tm_core.
 
DEPENDENCIES 
   
 
PARAMETERS
   parm         	: Specifies the type of parameter in request
   thread_id    	: Specifies the thread id to which response needs to be sent via
				  	  an ipc message with value
   msg_id       	: Specifies the message id that's associated with this ipc message
   param_value_size : Size of the parameter value   
               
 
RETURN VALUE
   TRUE        		: If the request to read param value is successful
   FALSE        	: If the request to read param value is not successful

SIDE EFFECTS 
  
===========================================================================*/

boolean lsmp_get_param(lsmp_get_msg_type msg)
{
  os_IpcMsgType             *ipc_msg_ptr = NULL;
  lsmp_get_msg_type	    	*get_msg = NULL;

  uint32  length = 0;
  boolean status = FALSE;

  length = sizeof(lsmp_get_msg_type); 

  /* Verify if size match in the request msg */
  switch (msg.param)
  {
    case LSMP_PREMIUM_SERVICES_TYPE:
		if (msg.param_value_size != sizeof(lsmp_premium_services_param_type))
		{
		  MSG_ERROR("Error in param_value_size in request",0,0,0); 
		  return status;
		}
        break;

	case LSMP_DEVICE_CONFIG_TYPE:
		if (msg.param_value_size != sizeof(lsmp_device_config_param_s_type))
		{
		  MSG_ERROR("Error in param_value_size for lsmp_device_config request",0,0,0); 
		  return status;
		}
        break;	

	default:
		MSG_ERROR("Unknown param type in request",0,0,0);
		return status;
  }
  
  ipc_msg_ptr = os_IpcCreate(length, IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr == NULL)
  {
    MSG_ERROR("IPC message allocation failed for %d",msg.param,0,0);
  }
  else
  {
    get_msg = (lsmp_get_msg_type*)ipc_msg_ptr->p_Data; 
	
	get_msg->param = msg.param;
    get_msg->thread_id = msg.thread_id;
    get_msg->msg_id = msg.msg_id;
	
    ipc_msg_ptr->q_MsgId  = TM_CORE_MSG_ID_LSMP_GET_PARAM;
    status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
    if (status == FALSE)
    {
      MSG_ERROR("Error in sending LSMP GET PARAM msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
      status = os_IpcDelete(ipc_msg_ptr);
      ipc_msg_ptr = NULL;
    } 
    else
    {
      MSG_HIGH("Sending LSMP GET PARAM msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
	  status = TRUE;
    } 
  }
  return status;
}

/*===========================================================================

FUNCTION lsmp_send_premium_service_ind_status

DESCRIPTION
    Request to send QMI indication status to TM Core.

DEPENDENCIES

PARAMETERS
   status         	: Specifies success (TRUE) or failure (FALSE)

RETURN VALUE
   TRUE        		: If the request to send ind status is successful
   FALSE        	: If the request to send ind status is not successful

SIDE EFFECTS

===========================================================================*/

boolean lsmp_send_premium_service_ind_status(boolean ind_status)
{
  os_IpcMsgType *ipc_msg_ptr = NULL;
  boolean status = FALSE;

  ipc_msg_ptr = os_IpcCreate(sizeof(boolean), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (NULL == ipc_msg_ptr)
  {
    MSG_ERROR("IPC message allocation failed for lsmp_send_premium_service_ind_status", 0, 0, 0);
  }
  else
  {
    *(ipc_msg_ptr->p_Data) = ind_status;
    ipc_msg_ptr->q_MsgId   = TM_CORE_MSG_ID_LSMP_SEND_PREM_SVC_IND_STATUS;
    status                 = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);

    if (FALSE == status)
    {
      MSG_ERROR("Error in sending LSMP SEND PREMIUM SERVICE IND STATUS msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId, 0, 0);
      status = os_IpcDelete(ipc_msg_ptr);
      ipc_msg_ptr = NULL;
    }
    else
    {
      MSG_HIGH("Sending LSMP SEND PREMIUM SERVICE IND STATUS msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId, 0, 0);
	  status = TRUE;
    }
  }
  return status;
}
