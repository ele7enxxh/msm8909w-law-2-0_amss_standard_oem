/*!
  @file
  mem_share_qmi_poll_client.c

  @brief
  Task to query the qmi service availability status for mem_share client.

*/

/*===========================================================================

  Copyright (c) 2009-2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/mem_share_qmi_poll_client.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/27/14   abh      Initial Version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "stdio.h"
#include "stdlib.h"
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "rcinit.h"
#include "dynamic_heap_memory_sharing_v01.h"
#include "mem_share_client.h"
#include "msg.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define QMI_CLNT_WAIT_SIG      (0x1 << 5)
#define QMI_CLNT_TIMER_SIG     (0x1 << 6)
#define MEM_SHARE_QMI_POLL_TIMEOUT_MS  5000  /* 30 secs */

qmi_idl_service_object_type				mem_share_service_object;
qmi_cci_os_signal_type					os_params;
qmi_client_type               mem_client_handle;

static mem_share_status_t               mem_share_qmi_poll_ret_status = MEM_SHARE_FAILURE;

/*===========================================================================

  FUNCTION:  mem_share_get_qmi_poll_status

===========================================================================*/
/*!
    @brief
    Provides the status for mem_share qmi service polling.

    @detail
    Provides the status for qmi service availability.

    @return
    mem_share_qmi_poll_ret_status variable value, which is updated in mem_share_qmi_poll_task().

*/
/*=========================================================================*/
mem_share_status_t mem_share_get_qmi_poll_status( void )
{
  return mem_share_qmi_poll_ret_status;
}

/*===========================================================================*/
/*!
    @brief
    Helper function for the client indication callback

    @detail
    Does nothing as of now.

    @return
    None

*/
/*=========================================================================*/
void mem_client_ind_cb
(
 qmi_client_type                mem_client_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
  return;
}


/*===========================================================================

  FUNCTION:  MEM_SHARE_QMI_POLL_TASK

===========================================================================*/
/*!
    @brief
    Initialize the QMI client for dynamic memory allocation requests

    @detail
    This function will initialize the mem_share client to talk to the service on HLOS that will 
    allocate the memory on HLOS and pass the handle back to the client.

    @return
    None.
    
*/
/*=========================================================================*/
void mem_share_qmi_poll_task
(
  /* Unused parameter - required to match rex_task_func_type prototype */
  dword                           ignored    /*lint -esym(715,ignored) */

)
{
   rcinit_handshake_startup(); 
   os_params.tcb = rex_self();
   os_params.sig = QMI_CLNT_WAIT_SIG;
   os_params.timer_sig = QMI_CLNT_TIMER_SIG;

   
   mem_share_service_object = dhms_get_service_object_v01();
   if (!mem_share_service_object)
   {
      mem_share_qmi_poll_ret_status = MEM_SHARE_FAILURE;
	  return;
   }

   mem_share_qmi_poll_ret_status = MEM_SHARE_INITIALIZING;

   /* Initialize the qmi client notifier */
   if(QMI_NO_ERR != qmi_client_init_instance( mem_share_service_object,
                                              QMI_CLIENT_INSTANCE_ANY,
                                              mem_client_ind_cb,
                                              NULL,
                                              &os_params,
                                              MEM_SHARE_QMI_POLL_TIMEOUT_MS,
                                              &mem_client_handle))
   {
      mem_share_qmi_poll_ret_status = MEM_SHARE_FAILURE;
       return;
   }


   mem_share_qmi_poll_ret_status = MEM_SHARE_SUCCESS;
   return;   

}

