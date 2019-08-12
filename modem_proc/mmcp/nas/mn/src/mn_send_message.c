/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_send_message.c_v   1.7   18 Sep 2001 14:15:42   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_send_message.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA
                   Commented out #include "stubs.h"
04/16/2001  VT     Corrected a bug: Added break after calling gs_send_message

5/9/01      CD     Removed unnecesary header file includes
                   Included mn.h

5/25/01     CD     Removed cause.h and ms.h since they were not being used

6/5/01      CD     Added return status check of gs_send_message()

6/11/01     CD     Added case for GS_QUEUE_UASMS in mn_send_message()
                   Included gs.h

7/13/01     CD     Replaced GS_QUEUE_MMI by GS_QUEUE_CM

8/9/01      CD     Replaced call to gs_log_message by CNM_log_message
                   Removed queue.h

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.

9/13/01     CD     Replaced call to CNM_log_message by mn_log_message

===========================================================================*/

#include "customer.h"
#include "rex.h"
#include "err.h"
#include "gs_v.h"
#include "environ.h"
#include "timers_v.h"
#include "mn.h"

#include "ms_timer_v.h"
#include "cc_cc.h"
#include "ms_cc_mm_v.h"
#include "cnm.h"
/*
 * Function name :  MN_send_message
 * -------------
 *
 * Description :     Function which calls gs_send_message without
 * -----------       changing any of the parameters. It is used
 *                   in a debug scenario to cause the message which
 *                   is to be sent to be dumped to the screen.
 *
 *
 * Parameters:
 * ----------
 *
 * queue_id : IN - Id of the queue where the message is to be sent
 * message_p : IN - pointer to message to be sent
 * wait : IN - flag to indicated if call should block on transmission or not
 *
 * Returns:
 * --------
 * status - indication of success or otherwise of the gs_send_message call
 */


gs_status_T MN_send_message ( gs_queue_id_T   queue_id,
VOID_DATA       *message_p,
boolean         wait
)
{

   gs_status_T gs_status;

   mn_log_message(queue_id, (IMH_T*) message_p);

   /* Initialize return value */
   gs_status = GS_SUCCESS;   

   /* Switch based on the queue_id of the destination task */
   switch(queue_id)
   {
       /* Message destination is MN_CM (Internal to this task) */
       case GS_QUEUE_MN_CM  :
          cnm_internal_send_routine((void*)message_p);
          break;
          
       /* Message destination is the Call Manager (MMI) */
       case GS_QUEUE_CM  :
          
       /* Message destination is MM */
       case GS_QUEUE_MM  :

       /* Message destination is UASMS */
       case GS_QUEUE_UASMS:
          
         gs_status = gs_send_message(queue_id, (void*)message_p, wait);
         break;


       /* All others are an error */
       default :

          /* Set return status */
          gs_status = GS_INVALID_ID;

          ERR("ERROR: Invalid queue_id specified",0,0,0);
          
          break;
   }
   return gs_status;


}

