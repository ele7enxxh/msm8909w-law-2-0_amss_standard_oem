/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_initialise_mn_data.c_v   1.6   25 Oct 2001 09:23:08   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_initialise_mn_data.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD      Replaced old header with QCOM header
                    Removed #ifdef ASW_SIMBA

5/9/01      CD      Removed unnecessary header file includes

5/22/01     CD      Corrected calls to memset to avoid lint warnings
                    
                    Removed redundant declaration of ss_transaction_information

                    Removed #include for cause.h and ms.h to avoid lint error

6/4/01      CD      Added initialization of sms_transaction_information

6/8/01      CD      Deleted global declaration and initialization of 
                    sms_storage_configuration,short_message_store

                    Deleted declaration and intialization of sms_mem_cap_ex

7/18/01     CD      Added initialization of Multicall_Status, sm_rl_transaction_information,
                    mo_rpdu_store

                    Removed initialization of sms_transaction_information and 
                    ss_transaction_information since it is done in the CNM task

                    Removed tp_mo_mr since is no longer used by MN (see UASMS)

                    Deleted active_calls_list since it is non longer used

7/19/01     CD      Removed global data declarations to mnglobal.c

8/10/01     CD      Removed unnecessary queue.h

08/16/01    AB      Renamed MMI references to CM.

9/27/01     CD      Corrected initialization of sm_rl_transaction_information due to changes
                    in the structure definition

10/12/02    AB      In MN_initialize_data(), initialize the channel configuration
                    to default value instead of zero.

01/28/03    AB      Initialized the whole ss_invoke_data[] table instead
                    of key element. 

04/15/03    AB      Initialized the new mn_deferred_end_call_ind.

11/02/04    AB      Added SS Call related User-to-User Signaling feature.

08/31/05    HS      Detailed init of mn_call_information

09/09/05    HS      Reverted 'Detailed init of mn_call_information'

11/09/05    AB      Added supports for FEATURE_REL5 via NV_FORCE_UE_MSCR_R99_I
01/18/06    up      Use NV_REL_MODE_MAX instead of NV_FORCE_UE_DYNAMIC for
                    MAX value checking.

03/14/06    NR      Removing reading of nv item NV_FORCE_UE_MSCR_R99_I
                    and directly reading the flag from mm and gmm

02/11/06    NR      Fixed lint errors
===========================================================================*/

/* Includes */

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include <string.h>
#include "environ.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include "fs_lib.h"

/*
 * Function name :  MN_initialise_MN_data
 * -------------
 *
 * Description :  Initialises MN Global data on power up.
 * -----------    Note that the cc_transaction_information is initialized 
 *                as part of the CNM_initialize_CNM_data
 *
 *
 *
 *
 *
 *
 * Parameters: none
 * ----------
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC   MN_initialise_MN_data (void)
{
   /* locals */
   word            i;
   word            j;
   transaction_id_T     transaction_id;

  /*
   * Initialise mn_call_information
   */

   for ( transaction_id = 0;
         transaction_id < MAXNO_CC_TRANSACTIONS ;
         transaction_id++ )
   {
     mn_call_information_p[transaction_id] = NULL;
   }
#if defined(FEATURE_DUAL_SIM)
   mn_as_id = SYS_MODEM_AS_ID_NONE;
#endif
   

   /*
    * Initialise ss_invoke_data
    */
   for (i=0;i<MAXNO_INVOKE_IDS;i++)
   {
      memset(&ss_invoke_data[i], NULL, sizeof(ss_invoke_data_T));
      ss_invoke_data[i].ss_stored_operation = NULL_SS_OPERATION;
      ss_invoke_data[i].connection_id = UNKNOWN_CONNECTION_ID;
      ss_invoke_data[i].mt_connection_id = UNKNOWN_CONNECTION_ID;
   }

   /*
    * Initialise the user_connections_store
    */

   /* NULL = 0, is a valid configuration for tch_user_connection_store value */
   for(i=0; i < MAX_NUM_SPEECH_BEARERS; i++)
   {
      tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected = FALSE;
      tch_user_connections_store[DEFAULT_RAB_ID].speech_channel_connected = FALSE;
      tch_user_connections_store[DEFAULT_RAB_ID].data_channel_connected = FALSE;
      tch_user_connections_store[DEFAULT_RAB_ID].tch_rate = TCH_FULL_RATE;
      tch_user_connections_store[DEFAULT_RAB_ID].subchannel = 0;  /* determine FULL or HALF RATE */
      tch_user_connections_store[DEFAULT_RAB_ID].channel_mode = SPEECH_V1;  /* determine FULL or HALF RATE */
   }

   /*
    * Set the default short message references to zero
    */

   rl_mo_mr = 0;

   /*
    * Set default value for last_allocated_mo_sms_connection_id to 0x06
    * so that next allocated mo sms connection_id is 0x00.
    */
   last_allocated_mo_sms_connection_id = MAX_MO_CONNECTION_ID;

   /*
    * Initialise sm_rl_transaction_information
    */
   for (i = 0; i < MAXNO_SMS_TRANSACTIONS; i++)
   {
        sm_rl_transaction_information[i].rl_state = IDLE;
        sm_rl_transaction_information[i].smma_transaction = FALSE;
        sm_rl_transaction_information[i].retrans_flag = FALSE;
        sm_rl_transaction_information[i].mtf_flag = FALSE;
        sm_rl_transaction_information[i].rl_message_ref = 0xff;
        sm_rl_transaction_information[i].short_message_identifier= 0xff;
#if defined(FEATURE_DUAL_SIM )
        if(MNCNM_IS_SINGLE_STACK)
        {
          sm_rl_transaction_information[i].as_id = SYS_MODEM_AS_ID_NONE;
        }
#endif
   }

   /*
    * Initialize mo_rpdu_store
    */

   memset(&mo_rpdu_store, 0xff, sizeof(mo_rpdu_store_T));
   memset(&mn_alternate_proc_status, 0x00, sizeof(mn_alternate_procedure_status_T));
   

   /*
    * Initialise Multicall_Status
    */
   for (i = 0; i < 3; i++)
   {
       Multicall_Status[i].call_activity = CALL_INACTIVE;
       Multicall_Status[i].call_is_mpty = FALSE;
       Multicall_Status[i].split_mpty = FALSE;
       Multicall_Status[i].id_index = 0xff;

       for (j= 0; j < 14; j++)
       {
          Multicall_Status[i].connection_id[j] = 0xff;
       }
   }

#ifdef FEATURE_UUS
   /* 
    * User to User Data buffers & Queues
    */

   memset(&mn_user_user_data_buffers[0], NULL, sizeof(cm_mo_user_data_req_T) * (MN_MAXNO_USER_USER_DATA+1));

   for (i = 0; i <= USER_USER_DATA_Q_ID; i++)
   {
       MN_q_init(&mn_user_user_data_q[i]);
   }

   for (j = 0; j <= MN_MAXNO_USER_USER_DATA; j++)
   {
       /* link items = buffer -> q element */
       (void)MN_q_link(&mn_user_user_data_buffers[j], &mn_user_user_data_pool[j]);
    
       /* queue the buffers */
       MN_q_put(&mn_user_user_data_q[USER_USER_DATA_Q_ID], &mn_user_user_data_pool[j]);
   }
#endif


   memset(&sms_link_control, NULL, sizeof(sms_link_control_T));

   /* transport type is initialized to invalid value,
    * so that MO SMS can be served over any transport layer.
    * */
   mo_sms_transport_type   = NAS_MSG_TRANSPORT_TYPE_MAX;
   mt_sms_transport_type   = NAS_MSG_TRANSPORT_TYPE_MAX;

   sip_config_timer_f = 0;

}
