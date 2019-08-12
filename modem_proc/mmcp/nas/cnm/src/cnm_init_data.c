/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_init_data.c_v   1.9   13 Jun 2002 13:01:36   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_init_data.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/01   CD      Replaced mn header files by single include of mn.h

08/14/01   AB      Renamed connection manager references to CNM from CM.

27-Sep-2001  VT    Moved the declaration of the following global data from 
                     this file to cnmglobal.c:
                        rex_timer_type mn_cnm_sleep_timer;

9/27/01    CD      Removed external definition of mn_cnm_sleep_timer.

10/22/01   AB      Replaced SS state variable, ss_transaction_information[], 
                    with SS state changes function.

01/02/02   AB      Set the cc_transaction_information[transaction_id].reserved = TRUE
                    for unit testing, that normally set by CM.

12-Mar-2002  VT    Removed Lint error.

04/19/02   CD      Added initialization of mm_rel_req_sent.
                   Deleted initialization of mm_connections_active.
                   Updated Copyright.

06/12/02   AB      Initialize the cc_transaction_information[].reserved data
                   field which normally set by CM/UI.   

                   Also, removed the UNIT test conditional
                   compiling to set the reserved data field as its already
                   been provided by cm_reserved_connection_id() function.

02/10/03   AB      Initialized the new SMS prefer radio link domain, 
                   pref_rl_domain, value to CS domain, MS_CC_MM.

04/19/05   HS      Added initialization of auxiliary states in 
                   CNM_init_transaction_information()
                   
08/24/05   HS      Added initialization of T308 expired flag in 
                   CNM_init_transaction_information()

02/11/06   NR      Removing lint errors
===========================================================================*/

#include "customer.h"
#include "rex.h"

#include "ms.h"
#include "environ.h"
#include "mn.h"
#include "mnglobal.h"
#include "cc_cc.h"

#include "ms_cc_mm_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"

#include "cmlib.h"
#include "cnm.h"


struct MessageIETag  *next_ref;

/**************** Begin of   CNM_initialise_CNM_data *******************************
 *
 *  Function name: CNM_initialise_CNM_data()
 *  --------------------------------------
 *
 *  Description:
 *  ------------
 *  This function initialises all the data used by the CNM task.
 *
 *  Parameters:
 *  -----------
 *  None
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_initialise_CNM_data( void )
{

   transaction_id_T transaction_id;

   /*
    * initialise static data through function calls
    */

   CNM_init_cnm_service_request_queue();
   CNM_init_dtmf_queue();
   CNM_init_cnm_data_queue();
   CNM_init_sms_request_queue();
   CNM_init_transaction_information();

   /* Reset the transient buffers */
   CNM_reset_buffer_transient_triggers();

   /*
    * initialise global data directly
    */

   for (transaction_id=0; transaction_id < MAXNO_CC_TRANSACTIONS; transaction_id++)
   {
      cc_progress_information[transaction_id] = 0;
   }

   mm_connection_pending.CNM_service_type = NO_CNM_SERVICE;
   mm_link_reestablishing = FALSE;
   mm_rel_req_sent = FALSE;

   (void)memset(timer_store, 0xff, sizeof(timer_store));
   /*
    * take entry 0 out of available list, as timer_id of zero is not allowed
    */

   (void)memset(&timer_store[0], 0x00, sizeof(timer_store[0]));
   timer_store_pointer = 0;
}
/**************** End of   CNM_initialise_CNM_data ********************************/

/**************** Begin of   CNM_init_transaction_information *********************
 *
 *  Function name: CNM_init_transaction_information()
 *  ------------------------------------------------
 *  Description:
 *  ------------
 *  Function to initialise the cc, ss, and sms transaction_information stores.
 *  This sets all the cc_transaction_information entries to NO_CNM_SERVICE, all
 *  ss_transaction_information entries to SS_idle, and all 
 *  sms_transaction_information entries to SMS_idle.
 * 
 *
 *  Parameters:
 *  -----------
 *  None
 *
 *  Returns:
 *  --------
 *  NOne
 *
 ******************************************************************************/

VOID_FUNC CNM_init_transaction_information(void)
{
   


   transaction_id_T     transaction_id;

   for ( transaction_id = 0;
         transaction_id < MAXNO_CC_TRANSACTIONS ;
         transaction_id++ )
   {
      cc_transaction_information_p[transaction_id] = NULL;
   }
      
   for ( transaction_id = 0;
         transaction_id < MAXNO_SS_TRANSACTIONS ;
         transaction_id++ )
   {
      CNM_set_ss_state(transaction_id, SS_IDLE);
   }

   for ( transaction_id = 0;
         transaction_id < MAXNO_SMS_TRANSACTIONS ;
         transaction_id++ )
   {
      sms_transaction_information[transaction_id].sms_state = SMS_IDLE;
      sms_transaction_information[transaction_id].release_pending = FALSE;
      sms_transaction_information[transaction_id].pref_rl_domain = MS_CC_MM;
   }

   CNM_mmcc_sync_ind.reason = (byte)WCDMA_RAB_RELEASED;

}
/**************** End of   CNM_init_transaction_information **********************/
