/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_send_msg.c_v   1.6   01 May 2002 13:26:52   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_send_msg.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
7/13/01    CD      Replaced GS_QUEUE_MMI by GS_QUEUE_CM
                   
8/9/01     CD      Replaced gs_log_message by CNM_log_message

08/14/01   AB      Renamed connection manager references to CNM from CM.

9/13/01    CD      Changed CNM_log_message calling sequence
                   Deleted queue.h since it is no longer used

04/19/02   CD      Added function CNM_check_for_last_connection()
                   Updated copyright

04/30/02   CD      Moved function CNM_check_for_last_connection() to 
                   cnm_msg_defines_and_utils.c

02/14/03   AB      Checked the return value for gs_send_message() per Lint.

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include <string.h>
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"

#include "nasutils.h"
#include "cc_iei_v.h"
#include "cc_cc.h"
#include "cnm.h"
#include "mnglobal.h"
#include "cmlib.h"
#include <stringl/stringl.h>

/*
 * global data declaration
 */

/*******************************************************************************
 *
 *  Function name: CNM_send_message()
 *  --------------------------------
 *
 *  Description:
 *  ------------
 *  This function uses a generic function call to send messages to other
 *  queues
 *
 *  Parameters:
 *  -----------
 *  p_message : IN - pointer to message to be sent
 *  queue : IN - id of queue to send message to
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_send_message( IMH_T * p_message, gs_queue_id_T queue)
{
    void *local_p_message = p_message;
    unsigned int length;

    if(p_message->message_id == MMCNM_DATA_REQ)  
    {
      mmcnm_data_req_T *local_mmcnm_data_req = (mmcnm_data_req_T *)CNM_alloc(sizeof(mmcnm_data_req_T));
      GET_IMH_LEN(length, &((mmcnm_data_req_T*)p_message)->header);
      memscpy(local_mmcnm_data_req, sizeof(mmcnm_data_req_T), p_message, length + sizeof(IMH_T));

      CNM_fill_mmcnm_data_req(local_mmcnm_data_req);
      local_p_message = (void *)local_mmcnm_data_req;
    }
#ifdef FEATURE_DUAL_SIM                 
    else if(p_message->message_id == MMCNM_EST_REQ)  
    {
      mmcnm_est_req_T *local_mmcnm_est_req = (mmcnm_est_req_T *)CNM_alloc(sizeof(mmcnm_est_req_T));
      GET_IMH_LEN(length, &((mmcnm_est_req_T*)p_message)->header);
      memscpy(local_mmcnm_est_req, sizeof(mmcnm_est_req_T), p_message, length + sizeof(IMH_T));

      CNM_fill_mmcnm_est_req(local_mmcnm_est_req);
      local_p_message = (void *)local_mmcnm_est_req;
    } 
#endif

    CNM_log_message((IMH_T *)local_p_message);

    /* Switch based on the queue_id of the destination task */
    switch(queue)
    {
       /* Message destination is MN_CM (Internal to this task) */
       case GS_QUEUE_MN_CM  :
           cnm_internal_send_routine((void*)p_message);
           break;
       
      /* Message destination is the Call Manager (MMI) */
       case GS_QUEUE_CM  :
           
       /* Message destination is MM */
       case GS_QUEUE_MM  :
          
         if(gs_send_message(queue, (void*)local_p_message, TRUE) != GS_SUCCESS)
         {
             MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Can't sent message q=%d", queue);
         }
         break;
           
       /* All others are an error */
       default :
         ERR("Invalid queue_id", 0, 0, 0);
           break;
    }
    if(p_message->message_id == MMCNM_DATA_REQ)
    {
      CNM_free(local_p_message);
    }
#ifdef FEATURE_DUAL_SIM  
    else if(p_message->message_id == MMCNM_EST_REQ)  
    {
      CNM_free(local_p_message);
    }
#endif

}
