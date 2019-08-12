/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_send_mn_msgs.c_v   1.4   06 Dec 2001 15:36:32   vtawker  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_send_mn_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/01    CD     Replaced mn header files by single include of mn.h

08/14/01   AB      Renamed connection manager references to CNM from CM.

09/17/01    CD     Replaced SS state transition to SS_IDLE by call to the SS state machine

6-Dec-2001  VT     Bug Fix: In CNM_send_reject_message(), while setting CC 
                     state to CC_STATE_U0, also stop timer T303.

04/16/03    AB     In CNM_send_reject_message() added supports for new
                     reject type and reject value received from MM.

===========================================================================*/

#include "customer.h"
#include "rex.h"

#include "environ.h"
#include "com_iei_v.h"
#include "mn.h"

#include "cc_cc.h"
#include "ms_cc_mm_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"

#include "cmlib.h"
#include "cnm.h"
#include "cm_sms.h"

#include "mnglobal.h"
/**************** Begin of   CNM_send_reject_message *******************************
 *
 *  Function name: CNM_send_reject_message()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to send the appropriate MNxx_REJ_IND message to MN, depending
 *  on the service type
 *
 *  Parameters:
 *  -----------
 * cnm_request_id - pointer to associated request_id information block
 * cause_value - value to use for reject cause
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_send_reject_message( cnm_request_id_T * p_cnm_request_id,
                                   reject_type_enum_T rej_type,
                                   reject_cause_T rej_value )
{
   MNCC_REJ_IND_T       mncc_rej_ind;
   boolean              send_message = TRUE;    /* default to sending   */
                                                /* MNCC_REJ_IND         */

   if ((p_cnm_request_id->CNM_service_type == MO_CALL_ESTABLISHMENT) ||
       (p_cnm_request_id->CNM_service_type == MO_CALL_ESTABLISHMENT_FULL_SPEECH) ||
       (p_cnm_request_id->CNM_service_type == MO_CALL_ESTABLISHMENT_HALF_SPEECH) ||
       (p_cnm_request_id->CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_9600) ||
       (p_cnm_request_id->CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_4800) ||
       (p_cnm_request_id->CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_2400) ||
       (p_cnm_request_id->CNM_service_type == EMERGENCY_CALL_ESTABLISHMENT) )
   {
      send_message = CNM_setup_MNCC_REJ_IND(
                                    &mncc_rej_ind,
                                    p_cnm_request_id->connection_id,
                                    rej_type,
                                    rej_value,
                                    NULL);

      if ( send_message )
      {
         CNM_send_message((IMH_T *)&mncc_rej_ind, GS_QUEUE_MN_CM );

         CNM_set_state_to_U0( p_cnm_request_id->connection_id );

         CNM_stop_timer(TIMER_T303, (unsigned char)FORM_TI_PD( p_cnm_request_id->connection_id, PD_CC ));
      }

   }
   else if ( p_cnm_request_id->CNM_service_type == SUPPLEMENTARY_SERVICE_ACTIVATION )
   {
      CNM_send_MNSS_REJ_IND(p_cnm_request_id->connection_id,
                            rej_value);
      /*
      * call SS state machine to transition state to SS_IDLE
      */
      (void) CNM_ss_respond_to_event(p_cnm_request_id->connection_id, MMSS_REJ_IND_event);
   }

    
   /* Rejecting the pending SMS requests after reestablishment failure*/
   else if(p_cnm_request_id->CNM_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT)
   {

      CNM_sms_respond_to_event(MMSMS_REEST_REJ_event,
              sms_transaction_information[p_cnm_request_id->connection_id].sms_state,
              p_cnm_request_id->connection_id);

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
	   CNM_send_MNSMS_ERROR_IND( p_cnm_request_id->connection_id, (byte)0,NORMAL_UNSPECIFIED,rej_value,MMCC_SMS_CS);
#else
	   CNM_send_MNSMS_ERROR_IND( p_cnm_request_id->connection_id,(byte)0, NORMAL_UNSPECIFIED);
#endif

    }
   p_cnm_request_id->CNM_service_type = NO_CNM_SERVICE;
}
/**************** End of   CNM_send_reject_message *******************************/
