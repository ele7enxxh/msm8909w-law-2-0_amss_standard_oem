/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_ss_state_machine.c_v   1.9   22 Apr 2002 10:22:58   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_ss_state_machine.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
20-Jun-2001  VT    Removed unwanted reference(s) to m20000_debug_str

08/06/01    CD     Replaced mn header files by single include of mn.h

08/07/01    CD     Chenged MSG_MED to MSG_HIGH to report state transitions

08/14/01   AB      Renamed connection manager references to CNM from CM.

9/14/01    CD      Modifed call to MSG_HIGH to avoid using %s on target

09/17/01   CD      Added case MMSS_REEST_CNF_event, MMSS_REJ_IND_event

10/22/01   AB      Added utility functions, CNM_get_ss_state, and 
                     CNM_set_ss_state, to set and access SS state table, 
                     ss_transaction_information[], instead of accessing 
                     state directly.
11/01/01   CD      Fixed compile warning in calls to DEBUG_get_ss_state_name
                   after the ss_state type was changed

11/07/01   AB      Removed unnecessary check for (id >=0).

04/19/02   CD      Replaced MMSS_REEST_CNF_event by MMSS_REEST_REJ_event due
                   to new MM-CNM interface.  The CNF_event was the result of
                   receiving a MMCC_REEST_CNF primitive with a reestablish_success
                   flag set to FALSE.  New processing calls for a MMCC_REEST_REJ
                   primitive

02/14/03   AB      Removed Lint errors.

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "msg.h"
#include "err.h"

#include "environ.h"
#include "mn.h"
#include "mnglobal.h"
#include "ms.h"

#include "cc_cc.h"
#include "timers_v.h"
#include "ms_timer_v.h"

#include "cmlib.h"
#include "cnm.h"
#include "nasutils.h"



/**************** Begin of   CNM_ss_respond_to_event *******************************
 *
 *  Function name: CNM_ss_respond_to_event()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *  This function checks that the protocol state is correct for processing
 *  a particular message
 *
 *  Parameters:
 *  -----------
 *  SS_input_primitive : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

boolean   CNM_ss_respond_to_event( connection_id_T connection_id,
                                  event_T  ss_event )
{
   boolean              return_value;
   ss_state_T           current_state;
   ss_state_T           next_state;
   MNSS_REJ_IND_T       mnss_rej_ind;


   current_state = CNM_get_ss_state(connection_id);

   next_state = current_state;

   return_value = FALSE;


   switch (current_state)
   {
      case SS_IDLE:
         switch (ss_event)
         {
            case MNSS_BEGIN_REQ_event:
               return_value = TRUE;
               next_state = SS_MM_CONN_PEND;
               break;

            case MMSS_EST_IND_event:
               return_value = TRUE;
               next_state = SS_CONN_EST;
               break;

            case MNSS_END_IND_event:
            case MMSS_REL_IND_event:
               break;

            default:
               ERR("Unknown SS event: received event = %d\n",
                      (dword) ss_event, 0, 0);
               break;
         }
         break;

      case SS_MM_CONN_PEND:
         switch (ss_event)
         {
            case MMSS_REL_IND_event:
            case MNSS_END_REQ_event:
               next_state = SS_IDLE;
               break;

            case MMSS_EST_CNF_event:
               return_value = TRUE;
               next_state = SS_CONN_EST;
               break;

            case MMSS_REEST_REJ_event:
               mnss_rej_ind.message_header.message_set = MS_MN_SS;
               mnss_rej_ind.message_header.message_id  = MNSS_REJ_IND;
               mnss_rej_ind.lower_layer_cause = LLC_NORMAL_UNSPECIFIED;
               mnss_rej_ind.connection_id = connection_id;
#ifdef FEATURE_DUAL_SIM
               if(MNCNM_IS_SINGLE_STACK)
               {
                 mnss_rej_ind.as_id = ss_transaction_information[connection_id].as_id;
               }
#ifdef FEATURE_DUAL_ACTIVE
               else
               {
                 mnss_rej_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
               }
#endif
#endif
               PUT_IMH_LEN( sizeof(MNSS_REJ_IND_T) - sizeof(IMH_T),
                           &mnss_rej_ind.message_header );

               CNM_send_message( (IMH_T *)&mnss_rej_ind,
                                GS_QUEUE_MN_CM );

               next_state = SS_IDLE;
               break;

            case MMSS_REJ_IND_event:
               next_state = SS_IDLE;
               break;

            default:
               ERR("Unknown SS event: received event = %d\n",
                       (dword) ss_event, 0, 0);
               break;
         }
         break;

      case SS_CONN_EST:
         switch (ss_event)
         {
            case MNSS_END_REQ_event:
            case MNSS_END_IND_event:
            case MMSS_REL_IND_event:
               return_value = TRUE;
               next_state = SS_IDLE;
               break;

            case MNSS_FACILITY_REQ_event:
            case MNSS_FACILITY_IND_event:
               return_value = TRUE;
               /* no state change */
               break;

            case MMSS_REEST_REJ_event:
               mnss_rej_ind.message_header.message_set = MS_MN_SS;
               mnss_rej_ind.message_header.message_id  = MNSS_REJ_IND;
               mnss_rej_ind.lower_layer_cause = LLC_NORMAL_UNSPECIFIED;
               mnss_rej_ind.connection_id = connection_id;
#ifdef FEATURE_DUAL_SIM
               if(MNCNM_IS_SINGLE_STACK)
               {
                 mnss_rej_ind.as_id = ss_transaction_information[connection_id].as_id;
               }
#ifdef FEATURE_DUAL_ACTIVE
               else
               {
                 mnss_rej_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
               }
#endif
#endif
               PUT_IMH_LEN( sizeof(MNSS_REJ_IND_T) - sizeof(IMH_T),
                           &mnss_rej_ind.message_header );

               CNM_send_message((IMH_T *)&mnss_rej_ind,
                                GS_QUEUE_MN_CM );

                next_state = SS_IDLE;
                break;

            case MMSS_REJ_IND_event:
               next_state = SS_IDLE;
               break;

            default:
               ERR("Unknown SS event: received event = %d\n",
                     (dword) ss_event, 0, 0);
               break;
         }
         break;

      default:
         ERR("Unknown SS state: current_state = %d\n",
                (dword) current_state, 0, 0);
         break;

   }

#ifdef FEATURE_CNM_UNIT_TEST
   MSG_HIGH_DS_3(MN_SUB,"=MNCNM= SS state for transaction %d changed from %s to %s\n", 
             connection_id,
             DEBUG_get_ss_state_name((byte)ss_transaction_information[connection_id].ss_state),
             DEBUG_get_ss_state_name((byte)next_state));
#else
   MSG_HIGH_DS_3(MN_SUB,"=MNCNM= SS state for transaction %d changed from %d to %d\n", 
             connection_id,
             ss_transaction_information[connection_id].ss_state,
             next_state);
#endif
 

   ss_transaction_information[connection_id].ss_state = next_state;


   /* Move MN SS state also to IDLE */
   if(next_state == SS_IDLE)
   {
     ss_transaction_information[connection_id].ss_mn_state = SS_MN_IDLE;
   }

   return return_value;
}


/**************** Begin of   CNM_ss_respond_to_event *******************************
 *
 *  Function name: CNM_get_ss_state()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *      Provides the STATE of the given valid SS transaction ID.
 *
 *  Parameters:
 *  -----------
 *      transaction_id_T - transaction ID.
 *
 *  Returns:
 *  --------
 *       ss_state_T : 
 *          SS_IDLE               0
 *          SS_MM_CONN_PEND       1
 *          SS_CONN_EST           2
 *          SS_CONN_REESTABLISH   3
 *          SS_UNKNOWN
 *
 ******************************************************************************/
ss_state_T CNM_get_ss_state (transaction_id_T id)
{
    ss_state_T ss_state;

    if(id < MAXNO_SS_TRANSACTIONS) 
    {
            ss_state = ss_transaction_information[id].ss_state;
    }
    else
    {
            MSG_HIGH_DS_1(MN_SUB,"=MNCNM= SS: Invalid transaction ID = %d", id);

            ss_state = SS_UNKNOWN;
    }

    return ss_state;
}

 
/**************** Begin of   CNM_ss_respond_to_event *******************************
 *
 *  Function name: CNM_set_ss_state()
 *  -----------------------------------
 *
 *  Description:
 *  ------------
 *      Provides the STATE of the given valid SS transaction ID.
 *
 *  Parameters:
 *  -----------
 *      transaction_id_T - transaction ID.
 *      ss_state_T - desired state :
 *        SS_IDLE               0
 *        SS_MM_CONN_PEND       1
 *        SS_CONN_EST           2
 *        SS_CONN_REESTABLISH   3
 *        SS_UNKNOWN            0xff
 *
 *  Returns:
 *  --------
 *      none.
 *
 ******************************************************************************/
VOID_FUNC CNM_set_ss_state(transaction_id_T id, ss_state_T state)
{
    if(id < MAXNO_SS_TRANSACTIONS)
    {
        ss_transaction_information[id].ss_state = state;

            if(state == SS_UNKNOWN)
            {
              MSG_LOW_DS_2(MN_SUB,"=MNCNM= SS: Id = %d, explicit set to UNKOWN state = %d", id, state);
            }
    }
    else
    {
            MSG_HIGH_DS_2(MN_SUB,"=MNCNM= SS: Invalid Transaction ID = %d, State = %d", id, state);
    }
}



/**************** End of   CNM_ss_respond_to_event *******************************/
