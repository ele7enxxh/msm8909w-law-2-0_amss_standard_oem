/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_route_mn_msgs.c_v   1.9   18 Sep 2001 14:15:08   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_route_mn_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/9/01      CD     Removed unnecesary header file includes

5/25/01     CD     Removed cause.h since it was not being used

6/7/01      CD     Changed function name of MN_route_SMS_MMI_to_MN_primitives()
                   to MN_route_UASMS_MMI_to_MN_primitives()

                   In MN_handle_MN_messaging, changed GS_QUEUE_MN to GS_QUEUE_MN_CM

                   In MN_handle_MMI_to_MN_primitives, changed MMI SMS message groups
                   to UASMS message group

7/13/01     CD     Replaced MS_MN_MMI by MS_MN_CM
                   Added MS_MN_UASMS

8/8/01      CD     Replaced call to gs_log_message by CNM_log_message

8/9/01      CD     Added message direction to CNM_log_message
                   Added cnm.h and cnm related header files

8/10/01     CD     Removed unnecessary queue.h

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.

9/13/01     CD     Replaced call to CNM_log_message by mn_log_message

9/18/01     CD     Chnged call to mn_log_message
                   Added gs.h
09/12/11    HC     Replacing the #include "gs.h" with #include "gs_v.h"
                   as a part of CMI activity 

===========================================================================*/


/* includes */

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "ms.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include "gs_v.h"

/* these are needed to be able to use a CNM function */
#include "ms_timer_v.h"
#include "cc_cc.h"
#include "ms_cc_mm_v.h"
#include "cnm.h"


/*
 *   Function name : MN_handle_MN_messaging
 *   ---------------
 *
 *   Description:
 *   ------------
 * This function provides the high level routing of messages
 * to the MN layer from the CC and CM layers.
 *
 *  Parameters:
 *  -----------
 *  IN MN_input_primitives
 *
 *  Returns:
 *
 *
 */

VOID_FUNC  MN_handle_MN_messaging(IMH_T  *MN_input_primitives)

{
   /* Log a message to report the receipt of a primitive */
   mn_log_message(GS_QUEUE_MN_CM, MN_input_primitives);

   switch(MN_input_primitives->message_set)

   {

   case MS_MN_CC:

      {

         MN_route_CC_to_MN_messages(MN_input_primitives);

         break;

      }

   case MS_MN_SS:

      {

         MN_route_SS_to_MN_primitives(MN_input_primitives);

         break;

      }

   case MS_MN_SMS:

      {

         MN_route_SMS_to_MN_primitives(MN_input_primitives);

         break;

      }


   case MS_MN_CM:
   case MS_MN_UASMS:

      {

         MN_handle_CM_to_MN_primitives(MN_input_primitives);

         break;

      }

#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
   case MS_TIMER:

      {

         MN_handle_timeouts(MN_input_primitives);

         break;

      }
#endif /*(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))*/
   case MS_MN_TEST:

      {

         MN_handle_test(MN_input_primitives);

         break;

      }
   default:

      {
         ERR("ERROR: Invalid MN input primitive",0,0,0);


         break;

      }

   }


}


/*
 *   Function name : MN_handle_CM_to_MN_primitives
 *   ---------------
 *
 *   Description:
 *   ------------
 * This function provides the routing of messages
 * to the MN layer from the CM layer and routes them to
 * MN-CC, MN-SS, or MN-SMS as appropriate.
 *
 *  Parameters:
 *  -----------
 *  IN CM_to_MN_primitives
 *
 *  Returns:
 *
 *
 */

VOID_FUNC MN_handle_CM_to_MN_primitives(IMH_T  *CM_to_MN_primitives)

{


   /* Locals */

   byte  message_id;

   /* Code */

   message_id = CM_to_MN_primitives->message_id;

   /* According to the message type, call the relevent module */

   if(message_id < CM_MO_SS_REQ)

   {

      MN_route_CC_CM_to_MN_primitives(CM_to_MN_primitives);

   }

   else

   {

      if(message_id < UASMS_SMS_REQ)

      {

         MN_route_SS_CM_to_MN_primitives(CM_to_MN_primitives);

      }

      else

      {

         if(message_id < CM_MT_CC_RES )

         {

            MN_route_UASMS_to_MN_primitives(CM_to_MN_primitives);

         }

         else

         {

            if(message_id < CM_MT_SS_RES)

            {

               MN_route_CC_CM_to_MN_primitives(CM_to_MN_primitives);

            }

            else

            {

               if(message_id < UASMS_MT_SMS_RES )

               {

                  MN_route_SS_CM_to_MN_primitives(CM_to_MN_primitives);

               }

               else

               {

                  if (message_id < CM_MO_CC_CONF )

                  {

                     MN_route_UASMS_to_MN_primitives(CM_to_MN_primitives);

                  }



                  else
                  {
                     ERR("ERROR: Invalid MN-CC input primitive",0,0,0);
 
                  }
               }
            }
         }
      }
   }
}


