/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_put_cnm_ss_msgs.c_v   1.5   11 Oct 2001 16:56:14   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_put_cnm_ss_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/7/01      CD     Fixed several compile warnings C4244 conversion from int to byte

5/10/01     CD     Removed unnecessary header file includes

5/24/01     CD     Removed cause.h and environ.h since they were not used

                   Corrected calling sequence for PUT_IMH_LEN to avoid lint errors.
                   Removed use of temporary structure 'message'

8/10/01     CD     Removed unnecessary queue.h

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.

10/12/01    AB     Fixed Lint errors.
===========================================================================*/

#include "rex.h"
#include <string.h>
#include "ms.h"
#include "timers_v.h"
#include "mn.h"
#include "msg.h"
#include "err.h"
#include "mnglobal.h"
#include <stringl/stringl.h>



/*
 * Function name :  MN_put_MNSS_END_REQ
 * -------------
 *
 * Description :  Creates a MNSS_END_REQ message
 * -----------
 *
 *
 *
 *
 *
 *
 * Parameters: connection id
 * ----------  facility
 *
 *
 *
 * Returns:    none
 * --------
 *
 */
void
MN_put_MNSS_END_REQ(connection_id_T connection_id, ie_facility_T *facility)
{
   MNSS_END_REQ_T  message_ptr;
   byte            message_len,facility_len;
   gs_status_T     func_return_val;

   message_ptr.message_header.message_set = MS_MN_SS;
   message_ptr.message_header.message_id = MNSS_END_REQ;

   message_ptr.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif

   if (facility == NULL)

   {
      /* There is no facility component */
      facility_len = 0;
   }
   else
   {
      facility_len = (byte)(facility->Length + 2);
      memscpy(message_ptr.data, sizeof(message_ptr.data) ,facility, facility_len);

   }

   message_len = (byte)(facility_len + sizeof(connection_id) + sizeof(sys_modem_as_id_e_type));

   PUT_IMH_LEN(message_len, &message_ptr.message_header);

   /* Send the message */

   func_return_val = MN_send_message(GS_QUEUE_MN_CM, &message_ptr, TRUE);

   if (func_return_val != GS_SUCCESS)
   {
      ERR("ERROR: Message send failed for MNSS_END_REQ messag",0,0,0);
   }
}



/*
 * Function name :  MN_put_MNSS_BEGIN_REQ
 * -------------
 *
 * Description :  Creates a MNSS_BEGIN_REQ message
 * -----------
 *
 *
 *
 *
 *
 *
 * Parameters: connection id
 * ----------  facility
 *
 *
 *
 * Returns:    none
 * --------
 *
 */
void 
MN_put_MNSS_BEGIN_REQ(connection_id_T connection_id, ie_facility_T *facility)
{
   MNSS_BEGIN_REQ_T message_ptr;
   byte             message_len,facility_length;
   gs_status_T      success;


   /* Create a MNSS_BEGIN_REQ message */

   message_ptr.message_header.message_set = MS_MN_SS;
   message_ptr.message_header.message_id = MNSS_BEGIN_REQ;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif

   ss_transaction_information[connection_id].ss_mn_state = SS_MN_NOT_IDLE;

   message_ptr.connection_id = connection_id;

   /* Put the facility in to message->data */

   /* allow for the IEI and length octets */

   if (facility == NULL)
   {
      /* there is no facility component */
      facility_length = 0;
   }
   else
   {
      facility_length = (byte)((facility->Length)+2);
      memscpy (message_ptr.data, sizeof(message_ptr.data) ,facility,facility_length);

   }

   message_len = (byte)(facility_length + sizeof(connection_id) + sizeof(sys_modem_as_id_e_type));

   PUT_IMH_LEN(message_len, &message_ptr.message_header);

   /* Send the message */

   success = MN_send_message(GS_QUEUE_MN_CM, &message_ptr, TRUE);

   if (success != GS_SUCCESS)
   {

      ERR("ERROR: Message send failed for MNSS_BEGIN_REQ message",0,0,0);

   }

}



/*
 * Function name :  MN_put_MNSS_FACILITY_REQ
 * -------------
 *
 * Description :  Creates a MNSS_FACILITY_REQ message
 * -----------
 *
 *
 *
 *
 *
 *
 * Parameters: connection id
 * ----------  facility
 *
 *
 *
 * Returns:    none
 * --------
 *
 */
void 
MN_put_MNSS_FACILITY_REQ(connection_id_T connection_id, ie_facility_T *facility)
{
   MNSS_FACILITY_REQ_T message_ptr;
   byte             message_len,facility_length;
   gs_status_T      success;


   /* Create a MNSS_FACILITY_REQ message */

   message_ptr.message_header.message_set = MS_MN_SS;
   message_ptr.message_header.message_id = MNSS_FACILITY_REQ;

   message_ptr.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message_ptr.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif

   ss_transaction_information[connection_id].ss_mn_state = SS_MN_NOT_IDLE;

   if (facility == NULL)

   {
      /* There is no facility component */
      facility_length = 0;
   }
   else
   {
      facility_length = (byte)(facility->Length + 2); /* Includes the header */

      /* copy the facility into the output message */

      memscpy (message_ptr.data, sizeof(message_ptr.data) ,facility,facility_length);
   }

   message_len = (byte)(facility_length + sizeof(connection_id) + sizeof(sys_modem_as_id_e_type));

   PUT_IMH_LEN(message_len, &message_ptr.message_header);

   /* Send the message */

   success = MN_send_message(GS_QUEUE_MN_CM, &message_ptr, TRUE);

   if (success != GS_SUCCESS)
   {
      ERR("ERROR: Message send failed for MNSS_BEGIN_REQ message",0,0,0);

   }

}
