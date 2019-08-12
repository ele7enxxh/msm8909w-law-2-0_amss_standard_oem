/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_cc_dtmf.c_v   1.10   09 Jan 2002 19:09:32   vtawker  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_cc_dtmf.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
19-Jun-2001 VT     Removed unwanted extern char m20000_debug_string[];

08/06/01    CD     Replaced mn header files by single include of mn.h

08/14/01    AB     Renamed connection manager references to CNM from CM.

4-Oct-2001  VT     Added calls to CNM_stop_timer() to stop TIMER_DTMF_ON/OFF
                     in the following functions:
                       CNM_process_start_dtmf_ack_message(),
                       CNM_process_start_dtmf_rej_message()  &
                       CNM_process_stop_dtmf_ack_message
                       
24-Oct-2001  VT     DTMF: Moved dtmf_queue, dtmf_queue_read_ptr & 
                      dtmf_queue_write_ptr to mn_cc.h, inside 
                      cc_transaction_information_T as we have one DTMF
                      queue per connection_id now.

                      Also moved define for MAXNO_DTMF_QUEUE_ENTRIES to 
                        mn_cc.h from this file

25-Oct-2001  VT     DTMF: Added function, CNM_clear_dtmf_queue().
                      Modified function, CNM_init_dtmf_queue() to clear the
                       dtmf queues for all the connection_id values.

26-Oct-2001  VT     DTMF: Modifed all the functions in this file for DTMF
                      Re-design.
                    Added functions: CNM_process_t336_expiry() and 
                      CNM_process_t337_expiry()

31-Oct-2001  VT     DTMF: In CNM_clear_dtmf_queue(), added check for the validity
                      of connection_id.
                    In the following functions, changed 
                      connection_id<=MAXNO_CC_TRANSACTIONS to remove the equal
                      sign, as connection_id should always be less than 
                      MAXNO_CC_TRANSACTIONS:
                        CNM_init_dtmf_queue()  and
                        CNM_read_dtmf_queue()

6-Nov-2001   VT     DTMF: Added validity check for connection_in in functions:
                      CNM_get_dtmf_state() and CNM_set_dtmf_state()
                    Added some missing curly braces to improve code clarity in:
                      CNM_process_start_dtmf_ack_message() and
                      CNM_process_start_dtmf_rej_message
                    Added a few more MSG_*, which will help in debugging 
                      abnormal/error cases.

7-Nov-2001   VT    DTMF: Added call to CNM_read_dtmf_queue(), after setting 
                     the dtmf_state to DTMF_IDLE, in the following:
                       CNM_process_start_dtmf_ack_message(),
                       CNM_process_start_dtmf_rej_message() and
                       CNM_process_stop_dtmf_ack_message()

9-Jan-2002   VT    Removed unwanted \n in the beginning of MSG_MED (1 place)

9-Aug-2002   AB    In CNM_clear_dtmf_queue(), initialized the DTMF state for 
                   each connection ID to DTMF_IDLE.

                   Added debug message in DTMF request, in 
                   CNM_mo_start_dtmf_request(), when request is being queueed.
 
27-June-2005 AB    Fixed DTMF logic that determine that when DTMF queue is 
                   empty or full.

                   Drop DTMF request if there isn't any traffic channel per 
                   TS24.008, 5.5.7.  Also send DTMF failure indicator to CM 
                   when a DTMF request is dropped.   

8-Feb-2006   NR    DTMF requests are rejected in CC_STATE_U11, CC_STATE_U12, CC_STATE_U19, CC_STATE_U0 and Clearing the DTMF queue properly.

7-Sep-2006   RD    DTMF overflow condition is properly handled

02-Nov-2006  NR    Removing lint errors 
 
04-July-2011 AM    Putting range check for fixing buffer overflow of cc_IE_info. 

12-Sep-2011  PM    Fix for CR 272613.
 
28-Nov-2012  AM    Fixing compilation warning 
===========================================================================*/

#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include <string.h>
#include "com_iei_v.h"

#include "ms.h"
#include "timers_v.h"
#include "cause.h"

#include "mn.h"
#include "mnglobal.h"

#include "cmlib.h"
#include "cnm.h"
#include "nasutils.h"

#include <stringl/stringl.h>

/*
 * function data declaration
 */
cc_dtmf_state_e_type  CNM_get_dtmf_state(connection_id_T connection_id);
VOID_FUNC  CNM_set_dtmf_state(connection_id_T connection_id, cc_dtmf_state_e_type new_dtmf_state);

/*************** Begin of   CNM_init_dtmf_queue **********************************************
 *
 *  Function name: CNM_init_dtmf_queue
 *  ---------------------------------
 *  Description:
 *  ------------
 *  This function initialises the dtmf queues for all the connection_id 
 *  values by setting their read and write indices to zero.
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

VOID_FUNC CNM_init_dtmf_queue(void)
{
    connection_id_T connection_id;

    for (connection_id=0; connection_id<MAXNO_CC_TRANSACTIONS; connection_id++)
    {
        CNM_clear_dtmf_queue(connection_id);
    }

}
/**************** End of   CNM_init_dtmf_queue ************************************/


/*************** Begin of   CNM_clear_dtmf_queue **********************************************
 *
 *  Function name: CNM_clear_dtmf_queue
 *  ---------------------------------
 *  Description:
 *  ------------
 *  This function initialises the dtmf queue of the given connection_id, 
 *  by setting the read and write indices to zero.
 *
 *  Parameters:
 *  -----------
 *  connection_id
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_clear_dtmf_queue(connection_id_T connection_id)
{
  int i;
  cc_dtmf_state_e_type current_cc_dtmf_state = CNM_get_dtmf_state(connection_id);
   
  if(connection_id < MAXNO_CC_TRANSACTIONS)
  {
    if(cc_transaction_information_p[connection_id] != NULL)
    { 
      cc_transaction_information_p[connection_id]->dtmf_queue_read_index = NULL;
      cc_transaction_information_p[connection_id]->dtmf_queue_write_index = NULL;
      cc_transaction_information_p[connection_id]->cc_dtmf_state = DTMF_IDLE; 

      for(i=0; i<MAXNO_DTMF_QUEUE_ENTRIES; i++)
      {
        cc_transaction_information_p[connection_id]->dtmf_queue[i].start_dtmf_queue_entry.message_header.message_id = NULL;
      }

      MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Clearing DTMF queue for ID %d  in state U%d",connection_id,cc_transaction_information_p[connection_id]->cc_state);
    }
    
    if (current_cc_dtmf_state == WAIT_FOR_START_DTMF_ACK)
    {
       MNCC_START_DTMF_CNF_T      mncc_start_dtmf_cnf;
       //dtmf_queue_entry_T   dtmf_command;
       mncc_start_dtmf_cnf.message_header.message_set = MS_MN_CC;
       mncc_start_dtmf_cnf.message_header.message_id  = MNCC_START_DTMF_CNF;
       mncc_start_dtmf_cnf.connection_id = connection_id;

	   mncc_start_dtmf_cnf.dtmf_success = FALSE;
       PUT_IMH_LEN( sizeof(mncc_start_dtmf_cnf) - sizeof(IMH_T),
               &mncc_start_dtmf_cnf.message_header);
    
       /* TS24.008, 5.5.7.1.  If timer T336 expires, the MS shall terminate the ongoing 
          DTMF procedure without any retransmissions, and is free to begin another DTMF 
          procedure (e.g. another START DTMF message). */

       mncc_start_dtmf_cnf.dtmf_success = FALSE;  
       CNM_send_message( (IMH_T *)&mncc_start_dtmf_cnf,
                       GS_QUEUE_MN_CM );

    }
    else if(current_cc_dtmf_state == WAIT_FOR_STOP_DTMF_ACK)
    {
        MNCC_STOP_DTMF_CNF_T       mncc_stop_dtmf_cnf;

        mncc_stop_dtmf_cnf.message_header.message_set = MS_MN_CC;
        mncc_stop_dtmf_cnf.message_header.message_id  = MNCC_STOP_DTMF_CNF;
        mncc_stop_dtmf_cnf.connection_id = connection_id;

        PUT_IMH_LEN( sizeof(MNCC_STOP_DTMF_CNF_T) - sizeof(IMH_T), 
                &mncc_stop_dtmf_cnf.message_header );

        CNM_send_message ((IMH_T *)&mncc_stop_dtmf_cnf,
                    GS_QUEUE_MN_CM );
    }
    CNM_stop_timer(TIMER_T336,(unsigned char)FORM_TI_PD( connection_id, PD_CC ));
    CNM_stop_timer(TIMER_T337,(unsigned char)FORM_TI_PD( connection_id, PD_CC ));
  }
  else
  {
    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Invalid conn_id passed to CNM_clear_dtmf_queue()\n");
  }
}
/**************** End of   CNM_clear_dtmf_queue ************************************/


/**************** Begin of    CNM_add_to_dtmf_queue *******************************
 *
 *  Function name: CNM_add_to_dtmf_queue
 *  -----------------------------------
 *  Description:
 *  ------------
 *  This function adds an entry pointed to by the input parameter to the
 *  dtmf queue.
 *
 *  Parameters:
 *  -----------
 *  connection_id    : IN - connection_id to which the dtmf queue belongs to
 *  dtmf_queue_entry : IN - dtmf request message to be put on the queue
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_add_to_dtmf_queue(connection_id_T connection_id,
                                void *queue_entry)
{
    
    byte CurrentWriteIndex, previousWriteIndex;
    byte NextWriteIndex, NextWriteEntry;
    IMH_T  *DTMF_request;
    dtmf_queue_entry_T *dtmf_queue_entry = (dtmf_queue_entry_T *)queue_entry;


/*
 * check if queue is full first - if write index would become equal
 * to the read index after the write, then the queue is already as
 * full as it can be. First calculate what the write index would contain
 * after the update
 */
    if(connection_id < MAXNO_CC_TRANSACTIONS)
    {
       if(cc_transaction_information_p[connection_id] != NULL)
       {
         CurrentWriteIndex = cc_transaction_information_p[connection_id]->dtmf_queue_write_index;
   
         NextWriteIndex = (unsigned char)((CurrentWriteIndex+1) % MAXNO_DTMF_QUEUE_ENTRIES);
   
         NextWriteEntry = cc_transaction_information_p[connection_id]->dtmf_queue[NextWriteIndex].start_dtmf_queue_entry.message_header.message_id;               
    
         DTMF_request = (IMH_T *)dtmf_queue_entry;
    
     
         /* Note that Valid WriteIndex is from 0 to (MAXNO_DTMF_QUEUE_ENTRIES-1)
          */
   
         if( (DTMF_request->message_id == MNCC_START_DTMF_REQ) &&
             ( (NextWriteEntry != NULL) ||  /* queue is FULL */  
               (CNM_mmcc_sync_ind.reason == (byte)WCDMA_RAB_RELEASED)  /* No available traffic channel */
             )
           )
         {
           /* reject the DTMF start request */
           MNCC_START_DTMF_CNF_T      mncc_start_dtmf_cnf;
       
           mncc_start_dtmf_cnf.message_header.message_set = MS_MN_CC;
           mncc_start_dtmf_cnf.message_header.message_id  = MNCC_START_DTMF_CNF;
           mncc_start_dtmf_cnf.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
           {
             mncc_start_dtmf_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
           }
#endif
           mncc_start_dtmf_cnf.dtmf_success = FALSE;
           PUT_IMH_LEN( sizeof(mncc_start_dtmf_cnf) - sizeof(IMH_T), &mncc_start_dtmf_cnf.message_header);
   
           CNM_send_message ( (IMH_T *)&mncc_start_dtmf_cnf, GS_QUEUE_MN_CM );
   
   
           MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Dropped START_DTMF key = %d, connection = %d", 
                         dtmf_queue_entry->start_dtmf_queue_entry.keypad_facility.IA5Char, connection_id);
   
         }
         else if( (DTMF_request->message_id == MNCC_STOP_DTMF_REQ) &&
                  ( (NextWriteEntry != NULL) ||  /* queue is FULL */  
                    (CNM_mmcc_sync_ind.reason == (byte)WCDMA_RAB_RELEASED)  /* No available traffic channel */
                  )
                )  
         {
           MNCC_START_DTMF_CNF_T      mncc_start_dtmf_cnf;
           MNCC_STOP_DTMF_CNF_T       mncc_stop_dtmf_cnf;
           
           /* if we were to fail on attempting to queue a stop_dtmf_req, deleting the corresponding start would make things easy*/
           
           previousWriteIndex = ((CurrentWriteIndex == 0)?(MAXNO_DTMF_QUEUE_ENTRIES -1) : (CurrentWriteIndex -1));
           if (cc_transaction_information_p[connection_id]->dtmf_queue[previousWriteIndex].start_dtmf_queue_entry.message_header.message_id == MNCC_START_DTMF_REQ)
           {
               /* Delete the entry contents */
               cc_transaction_information_p[connection_id]->dtmf_queue[previousWriteIndex].
                                 start_dtmf_queue_entry.message_header.message_id = NULL;
               /* Update the next write index */
               cc_transaction_information_p[connection_id]->dtmf_queue_write_index = previousWriteIndex;
   
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Deleting previously added start_dtmf as the corresponding stop_dtmf failed, connection = %d", connection_id);
               /* reject the DTMF start request */
               mncc_start_dtmf_cnf.message_header.message_set = MS_MN_CC;
               mncc_start_dtmf_cnf.message_header.message_id  = MNCC_START_DTMF_CNF;
               mncc_start_dtmf_cnf.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
               {
                 mncc_start_dtmf_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
               }
#endif
               mncc_start_dtmf_cnf.dtmf_success = FALSE;
               PUT_IMH_LEN( sizeof(mncc_start_dtmf_cnf) - sizeof(IMH_T), &mncc_start_dtmf_cnf.message_header);
   
               CNM_send_message ((IMH_T *)&mncc_start_dtmf_cnf, GS_QUEUE_MN_CM );
   
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Dropped START_DTMF, connection = %d", connection_id);
   
              
           }
           /* reject the DTMF stop request */
           mncc_stop_dtmf_cnf.message_header.message_set = MS_MN_CC;
           mncc_stop_dtmf_cnf.message_header.message_id  = MNCC_STOP_DTMF_CNF;
           mncc_stop_dtmf_cnf.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
           {
             mncc_stop_dtmf_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
           }
#endif
           PUT_IMH_LEN( sizeof(MNCC_STOP_DTMF_CNF_T) - sizeof(IMH_T), &mncc_stop_dtmf_cnf.message_header );
           
           CNM_send_message ((IMH_T *)&mncc_stop_dtmf_cnf, GS_QUEUE_MN_CM );
           
           MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Dropped STOP_DTMF, connection = %d", connection_id);
   
         }
         else
         {
           previousWriteIndex = ((CurrentWriteIndex == 0)?(MAXNO_DTMF_QUEUE_ENTRIES -1) : (CurrentWriteIndex -1));
           if (!((DTMF_request->message_id == MNCC_STOP_DTMF_REQ) && (cc_transaction_information_p[connection_id]->dtmf_queue[previousWriteIndex].start_dtmf_queue_entry.message_header.message_id == MNCC_STOP_DTMF_REQ)))
           {
             (void)memscpy( &(cc_transaction_information_p[connection_id]->dtmf_queue[CurrentWriteIndex]),
                         sizeof(dtmf_queue_entry_T),
                         dtmf_queue_entry,
                         sizeof(dtmf_queue_entry_T)
                       );
   
             /*
              * update stored write index
              */
   
             cc_transaction_information_p[connection_id]->dtmf_queue_write_index = NextWriteIndex;
           }
           else
           {
             MNCC_STOP_DTMF_CNF_T       mncc_stop_dtmf_cnf;
             mncc_stop_dtmf_cnf.message_header.message_set = MS_MN_CC;
             mncc_stop_dtmf_cnf.message_header.message_id  = MNCC_STOP_DTMF_CNF;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
             {
               mncc_stop_dtmf_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
             }
#endif
             mncc_stop_dtmf_cnf.connection_id = connection_id;
             PUT_IMH_LEN( sizeof(MNCC_STOP_DTMF_CNF_T) - sizeof(IMH_T), &mncc_stop_dtmf_cnf.message_header );
           
             CNM_send_message ((IMH_T *)&mncc_stop_dtmf_cnf, GS_QUEUE_MN_CM );
             MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Dropped STOP_DTMF due to another STOP_DTMF in the queue, connection = %d", connection_id);
           }
   
   
           MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Adding DTMF command = %d to queue on connection = %d", 
                         DTMF_request->message_id, connection_id);
         }
       }
       else
       {
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Cannot Add to DTMF queue as cc_transaction_information_p[%d] id NULL",connection_id); 
       }
    }
    else
    {
       MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid connection_id = %d received in CNM_add_to_dtmf_queue()",connection_id);
    }
}

/**************** End of   CNM_add_to_dtmf_queue ************************************/


/**************** Begin of   CNM_pop_dtmf_queue_entry ********************************
 *
 *  Function name: CNM_pop_dtmf_queue_entry
 *  --------------------------------------
 *
 *  Description:
 *  ------------
 *  This function removes the top entry from the dtmf queue and returns it
 *  to the caller
 *
 *  Parameters:
 *  -----------
 *  connection_id    : IN - connection_id to which the dtmf queue belongs to
 *  dtmf_queue_entry : OUT - pointer to location where dtmf request message
 *                     is to be placed
 *
 *  Returns:
 *  --------
 *  boolean TRUE if queue not empty
 *
 ******************************************************************************/

boolean CNM_pop_dtmf_queue_entry(connection_id_T connection_id, 
                                 dtmf_queue_entry_T *dtmf_queue_entry)
{
    boolean return_value = FALSE;
    byte CurrentReadIndex;

    if(connection_id < MAXNO_CC_TRANSACTIONS)
    {
       if(cc_transaction_information_p[connection_id] != NULL)
       {
       
         CurrentReadIndex = cc_transaction_information_p[connection_id]->dtmf_queue_read_index;
   
   
         /*
          * check if queue is not empty first - if write index is equal
          * to the read index, then the queue is empty
          */
         if (cc_transaction_information_p[connection_id]->dtmf_queue[CurrentReadIndex].start_dtmf_queue_entry.message_header.message_id == NULL)
         {
             /*
              * queue is empty
              */
   
             return_value = FALSE;
   
         }
         else
         {
             (void)memscpy( dtmf_queue_entry,
                           sizeof(dtmf_queue_entry_T),
                           &(cc_transaction_information_p[connection_id]->dtmf_queue[CurrentReadIndex]),
                           sizeof(dtmf_queue_entry_T)
                         );
   
             /*
              * update stored read index to point at next location
              */
   
             cc_transaction_information_p[connection_id]->dtmf_queue_read_index
                   = (unsigned char)((CurrentReadIndex + 1) % MAXNO_DTMF_QUEUE_ENTRIES);
   
             cc_transaction_information_p[connection_id]->dtmf_queue[CurrentReadIndex].
                                 start_dtmf_queue_entry.message_header.message_id = NULL;
   
             return_value = TRUE;
   
         }
       }
       else
       {
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Cannot pop from DTMF queue as cc_transaction_information_p[%d] id NULL",connection_id);     
         return_value = FALSE;
       }
    }
    else
    {
       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Invalid conn_id = %d passed to CNM_pop_dtmf_queue_entry()\n", connection_id );
    }
    return(return_value);

}
/**************** End of   CNM_pop_dtmf_queue_entry *******************************/

/*************** Begin of   CNM_get_dtmf_state **********************************************
 *
 *  Function name: CNM_get_dtmf_state
 *  ---------------------------------
 *  Description:
 *  ------------
 *  This function returns the dtmf state of the given connection_id
 *
 *  Parameters:
 *  -----------
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  DTMF state
 *
 ******************************************************************************/

cc_dtmf_state_e_type CNM_get_dtmf_state(connection_id_T connection_id)
{
    cc_dtmf_state_e_type current_cc_dtmf_state = DTMF_IDLE;

    if (connection_id < MAXNO_CC_TRANSACTIONS)
    {
        if(cc_transaction_information_p[connection_id] != NULL)
        {
          current_cc_dtmf_state = 
            cc_transaction_information_p[connection_id]->cc_dtmf_state;
        }
        else
        {
            MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot get DTMF state as cc_transaction_information_p[%d] id NULL",connection_id);
        }
    }
    else
    {
        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Invalid conn_id passed to CNM_get_dtmf_state()\n" );
    }
    return (current_cc_dtmf_state);

}
/**************** End of   CNM_get_dtmf_state ************************************/


/*************** Begin of   CNM_set_dtmf_state **********************************************
 *
 *  Function name: CNM_set_dtmf_state
 *  ---------------------------------
 *  Description:
 *  ------------
 *  This function sets the dtmf state of the given connection_id to the given state.
 *
 *  Parameters:
 *  -----------
 *  connection_id : IN
 *  new_dtmf_state: IN
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC  CNM_set_dtmf_state(connection_id_T connection_id, 
                              cc_dtmf_state_e_type new_dtmf_state)
{
    if (connection_id < MAXNO_CC_TRANSACTIONS)
    {
        if(cc_transaction_information_p[connection_id] != NULL)
        {
          cc_transaction_information_p[connection_id]->cc_dtmf_state 
                 = new_dtmf_state;
        }
        else
        {
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot set DTMF state as cc_transaction_information_p[%d] id NULL",connection_id);
        }
    }
    else
    {
        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Invalid conn_id passed to CNM_set_dtmf_state()\n");
    }

}
/**************** End of   CNM_set_dtmf_state ************************************/


/**************** Begin of   CNM_process_mo_start_dtmf_request ********************
 *
 *  Function name: CNM_process_mo_start_dtmf_request()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle a request for the start of generation of a
 *  DTMF tone.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a start DTMF request and sends it on.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mo_start_dtmf_request( MNCC_START_DTMF_REQ_T * message )
{
    connection_id_T      connection_id;
    start_dtmf_msg_T     data_req_msg;
    byte                 action;
    unsigned int         length;
    word                 bytes_remaining;
    MNCC_START_DTMF_CNF_T      mncc_start_dtmf_cnf;
    
    
    mncc_start_dtmf_cnf.message_header.message_set = MS_MN_CC;
    mncc_start_dtmf_cnf.message_header.message_id  = MNCC_START_DTMF_CNF;
    mncc_start_dtmf_cnf.connection_id = message->connection_id;
    mncc_start_dtmf_cnf.dtmf_success = FALSE;
    PUT_IMH_LEN( sizeof(mncc_start_dtmf_cnf) - sizeof(IMH_T),
            &mncc_start_dtmf_cnf.message_header);


    /*
    * get connection id for this transaction
    */
    
    connection_id = message->connection_id;
    
    action =
        CNM_cc_check_respond_to_event(connection_id, MNCC_START_DTMF_REQ_event );
    
    if ( (action != NULL) && /* actually comparing with A_BAD_EVENT in 
                            * CNM_cc_state_machine.c  
                            */
         (CNM_mmcc_sync_ind.reason != (byte)WCDMA_RAB_RELEASED) ) /* TS24.008, 5.5.7, 2nd subclause */
    {
        if (CNM_get_dtmf_state(connection_id) == DTMF_IDLE)
        {
            data_req_msg.msg_head.type  = START_DTMF;

            GET_IMH_LEN(length, &(message->message_header));
        
            bytes_remaining = (unsigned short) (length - sizeof(connection_id_T) - sizeof(sys_modem_as_id_e_type));
        
            if ( bytes_remaining >= sizeof(ie_keypad_facility_T) )
            {
                CNM_cc_respond_to_event(connection_id, action );

                (void) memscpy( &data_req_msg.keypad_facility,
                               sizeof(ie_keypad_facility_T),
                               &(message->keypad_facility),
                               sizeof(ie_keypad_facility_T) );

                PUT_IMH_LEN(sizeof(ie_keypad_facility_T) +
                            sizeof(data_req_msg.msg_head.ti_pd) +
                            sizeof(data_req_msg.msg_head.type),
                            &data_req_msg.msg_head.IMH );

                CNM_cc_start_timer(TIMER_T336,connection_id);  /* TS24.008, Sec 5.5.7.1 */

                CNM_send_primitive_to_mm( connection_id, PD_CC,
                                          (IMH_T *)&data_req_msg
                                        );

                CNM_set_dtmf_state(connection_id, WAIT_FOR_START_DTMF_ACK);

            }  /* end of if bytes_remaining... */
            else
            {
                ERR("Incomplete Start DTMF Req msg: remaining bytes = %d\n",
                    bytes_remaining, 0, 0);

            }
        }   /* end of if CNM_get_dtmf_state... */
        else
        {
            CNM_add_to_dtmf_queue(connection_id, (void *) message);

            MSG_HIGH_DS_2(MN_SUB,"=MNCNM= START DTMF id=%d, key=%d is queueed", connection_id, message->keypad_facility.IA5Char);
        }
        
    }  /* end of if action... */
    else
    {
      if(connection_id < MAXNO_CC_TRANSACTIONS)
      {
        if(cc_transaction_information_p[connection_id] != NULL)
        {
        
          if((cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U0 ) ||
             (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U11) ||
             (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U12) ||
             (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U19))
          {
              MSG_HIGH_DS_3(MN_SUB,"=MNCNM= START DTMF id=%d, key=%d is discarded at U%d",
                        connection_id, message->keypad_facility.IA5Char, cc_transaction_information_p[connection_id]->cc_state);
              
              mncc_start_dtmf_cnf.dtmf_success = FALSE;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
              {
                 mncc_start_dtmf_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
              }
#endif
              CNM_send_message( (IMH_T *)&mncc_start_dtmf_cnf,
                    GS_QUEUE_MN_CM );
 

          }
          else
          {
              CNM_add_to_dtmf_queue(connection_id, (dtmf_queue_entry_T *) message);

              MSG_HIGH_DS_2(MN_SUB,"=MNCNM= START DTMF id=%d, key=%d is queueed", connection_id, message->keypad_facility.IA5Char);
          }
        }
        else
        {
           MSG_HIGH_DS_3(MN_SUB,"=MNCNM= START DTMF id=%d, key=%d is discarded as cc_transaction_information_p[%d] is NULL",
                        connection_id, message->keypad_facility.IA5Char, connection_id);
        }
      }
      else
      {
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= START DTMF is discarded as connection_id = %d is invalid",connection_id);
      }
    }
    
}
/**************** End of   CNM_process_mo_start_dtmf_request *******************************/


/**************** Begin of   CNM_process_mo_stop_dtmf_request *********************
 *
 *  Function name: CNM_process_mo_stop_dtmf_request()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle a request for the cessation of generation of a
 *  DTMF tone.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a stop DTMF request and sends it on.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mo_stop_dtmf_request( MNCC_STOP_DTMF_REQ_T * message )
{
    connection_id_T      connection_id;
    stop_dtmf_msg_T      data_req_msg;
    byte                 action;
    MNCC_STOP_DTMF_CNF_T       mncc_stop_dtmf_cnf;
   

    mncc_stop_dtmf_cnf.message_header.message_set = MS_MN_CC;
    mncc_stop_dtmf_cnf.message_header.message_id  = MNCC_STOP_DTMF_CNF;
    mncc_stop_dtmf_cnf.connection_id = message->connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
    {
      mncc_stop_dtmf_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
    }
#endif
    PUT_IMH_LEN( sizeof(MNCC_STOP_DTMF_CNF_T) - sizeof(IMH_T), 
            &mncc_stop_dtmf_cnf.message_header );

    /*
    * get connection id for this transaction
    */
    
    connection_id = message->connection_id;
    
    action = 
        CNM_cc_check_respond_to_event(connection_id, MNCC_STOP_DTMF_REQ_event );

    if ( ( action != NULL )  && /* actually comparing with A_BAD_EVENT in 
                                 * CNM_cc_state_machine.c  
                                 */
         (CNM_mmcc_sync_ind.reason != (byte)WCDMA_RAB_RELEASED) )
    {
        if (CNM_get_dtmf_state(connection_id) == DTMF_IDLE)
        {
            CNM_cc_respond_to_event(connection_id, action );
            
            data_req_msg.msg_head.type  = STOP_DTMF;                     
            
            PUT_IMH_LEN(sizeof(data_req_msg.msg_head.ti_pd) +
                sizeof(data_req_msg.msg_head.type),
                &data_req_msg.msg_head.IMH );
            
            CNM_cc_start_timer(TIMER_T337,connection_id);  /* TS24.008, Sec 5.5.7.3 */

            CNM_send_primitive_to_mm(connection_id, PD_CC,
                                     (IMH_T *)&data_req_msg
                                    );
            CNM_set_dtmf_state(connection_id, WAIT_FOR_STOP_DTMF_ACK);
        }
        else
        {
            CNM_add_to_dtmf_queue ( connection_id, (void *)message );

            MSG_HIGH_DS_1(MN_SUB,"=MNCNM= STOP DTMF id=%d is queueed", connection_id);
        }
        
    }
    else
    {
      if(connection_id < MAXNO_CC_TRANSACTIONS)
      {
        if(cc_transaction_information_p[connection_id] != NULL)
        {
          if((cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U0 ) ||
             (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U11) ||
             (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U12) ||
             (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U19))
          {
              MSG_HIGH_DS_2(MN_SUB,"=MNCNM= STOP DTMF id=%d, is discarded at U%d",
                        connection_id,  cc_transaction_information_p[connection_id]->cc_state);

              CNM_send_message ((IMH_T *)&mncc_stop_dtmf_cnf,
                        GS_QUEUE_MN_CM );

          }
          else
          {
              CNM_add_to_dtmf_queue ( connection_id, (dtmf_queue_entry_T *)message );

              MSG_HIGH_DS_1(MN_SUB,"=MNCNM= STOP DTMF id=%d is queueed", connection_id);
          }
        }
        else
        {
           MSG_HIGH_DS_2(MN_SUB,"=MNCNM= START DTMF id=%d is discarded as cc_transaction_information_p[%d] is NULL",
                        connection_id, connection_id);
        }
      }
      else
      {
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= STOP DTMF is discarded as connection_id = %d is invalid",connection_id);
      }
    }
    
}
/**************** End of   CNM_process_mo_stop_dtmf_request *******************************/


/**************** Begin of   CNM_process_start_dtmf_ack_message *******************
 *
 *  Function name: CNM_process_start_dtmf_ack_message()
 *  --------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a start dtmf ack message received from the network.
 *  It assumes that the message has been checked for IE content
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_start_dtmf_ack_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{
    MNCC_START_DTMF_CNF_T      mncc_start_dtmf_cnf;
    byte                       IEI;
    byte                       IE_count;
    
    if (CNM_get_dtmf_state(connection_id) == WAIT_FOR_START_DTMF_ACK)
    {
        mncc_start_dtmf_cnf.message_header.message_set = MS_MN_CC;
        mncc_start_dtmf_cnf.message_header.message_id  = MNCC_START_DTMF_CNF;
        mncc_start_dtmf_cnf.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
        {
          mncc_start_dtmf_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
        }
#endif
    mncc_start_dtmf_cnf.dtmf_success = TRUE;
        
        PUT_IMH_LEN( sizeof(mncc_start_dtmf_cnf) - sizeof(IMH_T),
                     &mncc_start_dtmf_cnf.message_header);
        
        IE_count = 2;              /* start at first IE after message type */
        

        if ( no_of_IEs > MAXNO_IES_IN_MESSAGE )
        {
           ERR("no_of_IEs exceeds MAXNO_IES_IN_MESSAGE", 0, 0, 0);
           no_of_IEs = MAXNO_IES_IN_MESSAGE;
        }
        /*
        * set up IE_count to add valid IEs with correct syntax
        */
        
        while (( syntax_ok != FALSE ) && ( IE_count < no_of_IEs ))
        {
            
        /*
        * get information of the next IE from global store
            */
            
            IEI       = cc_IE_info[IE_count].IEI;

            if(cc_IE_info[IE_count].syntax_ok == TRUE)
            {
              switch (IEI)
              {
                    
              /*
              * accept the following IEs
              */
                    
              case IEI_KEYPAD_FACILITY:  /* no use for this information */
                  break;
                    
              default:                            /* this cannot happen! */
                  ERR("Unexpected IE in start_dtmf_ack msg: received IEI = %d\n",
                      IEI, 0, 0);
                    
                  syntax_ok = FALSE;
                  break;
              }
            }

            IE_count++;       /* index next IE in the message */
                    
        }   /* END WHILE more IEs and Syntax is OK */
        
        if (!syntax_ok)
        {
           ERR("Syntax error in Start DTMF Ack Message\n", 0, 0, 0);
           CNM_send_status ( connection_id,
                    PD_CC,
                    cause );
        }

        /* Process next DTMF */

        CNM_stop_timer(TIMER_T336, 
                (unsigned char)FORM_TI_PD( connection_id, PD_CC ));
        /* START_DTMF_ACK received from the n/w will not be 
         * forwarded to MN once the call has been put on hold 
        */
        if((cc_transaction_information_p[connection_id] != NULL)
            && (cc_transaction_information_p[connection_id]->hold_auxiliary_state != HOLD_CALL_HELD)
            && (cc_transaction_information_p[connection_id]->hold_auxiliary_state != HOLD_RETRIEVE_REQUEST))
        {
            CNM_send_message( (IMH_T *)&mncc_start_dtmf_cnf,
                              GS_QUEUE_MN_CM );
        }
        
        CNM_set_dtmf_state(connection_id, DTMF_IDLE);

        CNM_read_dtmf_queue(connection_id);


    }
    else
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= StartDtmfAck Msg rxed in incorrect DTMF state for connection_id = %d", connection_id);
    }
}

/**************** End of   CNM_process_start_dtmf_ack_message *******************************/


/**************** Begin of   CNM_process_start_dtmf_rej_message *******************
 *
 *  Function name: CNM_process_start_dtmf_rej_message()
 *  --------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a start dtmf rej message received from the network.
 *  It assumes that the message has been checked for IE content
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_start_dtmf_rej_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{
    MNCC_START_DTMF_CNF_T      mncc_start_dtmf_cnf;
    byte                       IEI;
    byte                       IE_count;
    
    if (CNM_get_dtmf_state(connection_id) == WAIT_FOR_START_DTMF_ACK)
    {
        dtmf_queue_entry_T   dtmf_command;
    
        /* TS24.008, 5.5.7.1: remove corresponding STOP DTMF from queue */
        if(CNM_pop_dtmf_queue_entry( connection_id, &dtmf_command ))
        {
            MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Removed related STOP DTMF on T336 expired=%d, cmd=%d\n", connection_id, 
                                dtmf_command.stop_dtmf_queue_entry.message_header.message_id);
        }

        mncc_start_dtmf_cnf.message_header.message_set = MS_MN_CC;
        mncc_start_dtmf_cnf.message_header.message_id  = MNCC_START_DTMF_CNF;
        mncc_start_dtmf_cnf.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
        {
          mncc_start_dtmf_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
        }
#endif
        mncc_start_dtmf_cnf.dtmf_success = FALSE;
        PUT_IMH_LEN( sizeof(mncc_start_dtmf_cnf) - sizeof(IMH_T),
            &mncc_start_dtmf_cnf.message_header);
        
        IE_count = 2;              /* start at first IE after message type */
        

        if ( no_of_IEs > MAXNO_IES_IN_MESSAGE )
        {
           ERR("no_of_IEs exceeds MAXNO_IES_IN_MESSAGE", 0, 0, 0);
           no_of_IEs = MAXNO_IES_IN_MESSAGE;
        }
        /*
        * set up write pointer ready to append the IEs
        */
        
        while (( syntax_ok != FALSE ) && ( IE_count < no_of_IEs ))
        {
            
        /*
        * get information of the next IE from global store
            */
            
            IEI  = cc_IE_info[IE_count].IEI;

            if(cc_IE_info[IE_count].syntax_ok == TRUE)            
            {
              switch (IEI)
              {

               /*
               * accept the following IEs
               */
                    
               case IEI_CAUSE:     /* NOTE - This will not contain an IEI */
                  break;

               default:            /* this cannot happen! */
                  ERR("Unexpected IE in start_dtmf_rej msg: received IEI = %d\n",
                       IEI, 0, 0);
                    
                  syntax_ok = FALSE;
                  break;
              }
            }

            IE_count++;       /* index next IE in the message */
                    
        }   /* END WHILE more IEs and Syntax is OK */
        
        if (!syntax_ok)
        {
            ERR("Syntax error in Start DTMF Rej Message \n", 0, 0, 0);
            CNM_send_status ( connection_id,
                    PD_CC,
                    cause );
        }

        CNM_stop_timer(TIMER_T336, 
                (unsigned char)FORM_TI_PD( connection_id, PD_CC ));
        /* START_DTMF_REJ received from the n/w will not be 
         * forwarded to MN once the call has been put on hold 
        */
        if( (cc_transaction_information_p[connection_id] != NULL)
            && (cc_transaction_information_p[connection_id]->hold_auxiliary_state != HOLD_CALL_HELD)
            && (cc_transaction_information_p[connection_id]->hold_auxiliary_state != HOLD_RETRIEVE_REQUEST))
        {
            CNM_send_message( (IMH_T *)&mncc_start_dtmf_cnf,
                    GS_QUEUE_MN_CM );
        }

        CNM_set_dtmf_state(connection_id, DTMF_IDLE);

        CNM_read_dtmf_queue(connection_id);

    }
    else
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= StartDtmfRej Msg rxed in incorrect DTMF state for connection_id = %d",connection_id);
    }
    
}

/**************** End of   CNM_process_start_dtmf_rej_message *******************************/


/**************** Begin of   CNM_process_stop_dtmf_ack_message ********************
 *
 *  Function name: CNM_process_stop_dtmf_ack_message()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a stop dtmf ack message received from the network.
 *  It assumes that the message has been checked for IE content
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none 
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_stop_dtmf_ack_message( connection_id_T connection_id ) 
{
    MNCC_STOP_DTMF_CNF_T       mncc_stop_dtmf_cnf;
    
    if (CNM_get_dtmf_state(connection_id) == WAIT_FOR_STOP_DTMF_ACK)
    {
        mncc_stop_dtmf_cnf.message_header.message_set = MS_MN_CC;
        mncc_stop_dtmf_cnf.message_header.message_id  = MNCC_STOP_DTMF_CNF;
        mncc_stop_dtmf_cnf.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
        {
           mncc_stop_dtmf_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
        }
#endif
        PUT_IMH_LEN( sizeof(MNCC_STOP_DTMF_CNF_T) - sizeof(IMH_T), 
            &mncc_stop_dtmf_cnf.message_header );
        
        CNM_stop_timer(TIMER_T337, 
            (unsigned char)FORM_TI_PD( connection_id, PD_CC ));

        /* STOP_DTMF_ACK received from the n/w will not be 
         * forwarded to MN once the call has been put on hold 
        */  
        if( (cc_transaction_information_p[connection_id] != NULL)
            && (cc_transaction_information_p[connection_id]->hold_auxiliary_state != HOLD_CALL_HELD)
            && (cc_transaction_information_p[connection_id]->hold_auxiliary_state != HOLD_RETRIEVE_REQUEST))
        {
            CNM_send_message ((IMH_T *)&mncc_stop_dtmf_cnf,
            GS_QUEUE_MN_CM );
        }
        
        CNM_set_dtmf_state(connection_id, DTMF_IDLE);

        CNM_read_dtmf_queue(connection_id);
        
    }
    else
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= StopDtmfAck Msg rxed in incorrect DTMF state for connection_id = %d",connection_id);
    }
    
}
/**************** End of   CNM_process_stop_dtmf_ack_message *******************************/


/**************** Begin of   CNM_process_t336_expiry ************************************
 *
 *  Function name: CNM_process_t336_expiry()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles the expiry of Timer, T336 (non-receipt of a start dtmf ack 
 *  message from the network).
 *
 *  Parameters:
 *  -----------
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none 
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_t336_expiry( connection_id_T connection_id ) 
{
   
    MNCC_STOP_DTMF_CNF_T       mncc_stop_dtmf_cnf;
    MNCC_START_DTMF_CNF_T      mncc_start_dtmf_cnf;

    mncc_stop_dtmf_cnf.message_header.message_set = MS_MN_CC;
    mncc_stop_dtmf_cnf.message_header.message_id  = MNCC_STOP_DTMF_CNF;
    mncc_stop_dtmf_cnf.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
    {
       mncc_stop_dtmf_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
    }
#endif
    PUT_IMH_LEN( sizeof(MNCC_STOP_DTMF_CNF_T) - sizeof(IMH_T), 
            &mncc_stop_dtmf_cnf.message_header );

    mncc_start_dtmf_cnf.message_header.message_set = MS_MN_CC;
    mncc_start_dtmf_cnf.message_header.message_id  = MNCC_START_DTMF_CNF;
    mncc_start_dtmf_cnf.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
    {
       mncc_start_dtmf_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
    }
#endif
    mncc_start_dtmf_cnf.dtmf_success = FALSE;
    PUT_IMH_LEN( sizeof(mncc_start_dtmf_cnf) - sizeof(IMH_T),
            &mncc_start_dtmf_cnf.message_header);

    if (CNM_get_dtmf_state(connection_id) == WAIT_FOR_START_DTMF_ACK)
    {
        dtmf_queue_entry_T   dtmf_command;
    
        /* TS24.008, 5.5.7.1.  If timer T336 expires, the MS shall terminate the ongoing 
           DTMF procedure without any retransmissions, and is free to begin another DTMF 
           procedure (e.g. another START DTMF message). */
        
		mncc_start_dtmf_cnf.dtmf_success = FALSE;	  
        CNM_send_message( (IMH_T *)&mncc_start_dtmf_cnf,
                        GS_QUEUE_MN_CM );
        
		if(CNM_pop_dtmf_queue_entry( connection_id, &dtmf_command ))
        {
          
		  MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Removed related STOP DTMF on T336 expired=%d, cmd=%d\n", connection_id, 
                                dtmf_command.stop_dtmf_queue_entry.message_header.message_id);
          CNM_send_message ((IMH_T *)&mncc_stop_dtmf_cnf,
                        GS_QUEUE_MN_CM );
		}

        
        CNM_set_dtmf_state(connection_id, DTMF_IDLE);
        CNM_read_dtmf_queue(connection_id);
    }
    else
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= T336 expired in incorrect DTMF state for connection_id = %d",connection_id);
    }
}

/**************** End of   CNM_process_t336_expiry ***********************************/


/**************** Begin of   CNM_process_t337_expiry ************************************
 *
 *  Function name: CNM_process_t337_expiry()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles the expiry of Timer, T337 (non-receipt of a stop dtmf ack 
 *  message from the network).
 *
 *  Parameters:
 *  -----------
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none 
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_t337_expiry( connection_id_T connection_id ) 
{

    MNCC_STOP_DTMF_CNF_T       mncc_stop_dtmf_cnf;

    mncc_stop_dtmf_cnf.message_header.message_set = MS_MN_CC;
    mncc_stop_dtmf_cnf.message_header.message_id  = MNCC_STOP_DTMF_CNF;
    mncc_stop_dtmf_cnf.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
    {
       mncc_stop_dtmf_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
    }
#endif
    PUT_IMH_LEN( sizeof(MNCC_STOP_DTMF_CNF_T) - sizeof(IMH_T), 
            &mncc_stop_dtmf_cnf.message_header );
    
    if (CNM_get_dtmf_state(connection_id) == WAIT_FOR_STOP_DTMF_ACK)
    {

        CNM_send_message ((IMH_T *)&mncc_stop_dtmf_cnf,
                    GS_QUEUE_MN_CM );

        CNM_set_dtmf_state(connection_id, DTMF_IDLE);
        CNM_read_dtmf_queue(connection_id);
    }
    else
    {
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= T337 expired in incorrect DTMF state for connection_id = %d", connection_id);
    }
    
}
/**************** End of   CNM_process_t337_expiry ***********************************/


/**************** Begin of   CNM_read_dtmf_queue **********************************
 *
 *  Function name: CNM_read_dtmf_queue()
 *  -----------------------------------
 *
 *  Description:
 *  ------------
 *  This function gets the top entry from the dtmf queue. If there is one,
 *  it process the dtmf request.
 *
 *  Parameters:
 *  -----------
 *  None
 *
 *  Returns:
 *  --------
 *  connection_id : IN
 *
 ******************************************************************************/

VOID_FUNC CNM_read_dtmf_queue( connection_id_T connection_id )
{
    
    dtmf_queue_entry_T   dtmf_command;
    boolean              msg_present;
    
    if (connection_id < MAXNO_CC_TRANSACTIONS)
    {
        MSG_MED_DS_0(MN_SUB,"=MNCNM= Reading DTMF QUEUE\n" );
        
        msg_present = CNM_pop_dtmf_queue_entry( connection_id, &dtmf_command );
        
        if ( msg_present != FALSE )
        {
            switch (dtmf_command.start_dtmf_queue_entry.message_header.message_id)
            {
              case MNCC_START_DTMF_REQ:
                CNM_process_mo_start_dtmf_request(
                    (MNCC_START_DTMF_REQ_T *) &dtmf_command );
                break;
                
              case MNCC_STOP_DTMF_REQ:
                CNM_process_mo_stop_dtmf_request(
                    (MNCC_STOP_DTMF_REQ_T *) &dtmf_command );
                break;
                
              default:
                ERR("Unexpected msg: received message_id = %d\n",
                    (word) (dtmf_command.start_dtmf_queue_entry.
                    message_header.message_id), 0, 0);
                
                break;
            }
        }
    }
    else
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= DTMF queue empty for connection_id = %d\n", connection_id);
    }
}
/**************** End of   CNM_read_dtmf_queue *************************************/
