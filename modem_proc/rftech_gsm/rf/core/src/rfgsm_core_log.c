/*!
   @file rfgsm_core_log.c

   @brief

   @details

*/


/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_log.c#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/14/14   zbz     Add SMPS/ICQ/temp_rem to FTM log packet 
07/22/13   ec	   Use new F3s/Null pointer checks
05/28/13   ak      Featurize/Fix G+W only build compiler warnings
03/18/13   svi     Replaced memcpy() by safer memscpy(). 
11/26/12   aka     Modified rfgsm_core_log_send_to_ftm_task to use rfgsm handle
02/22/12   shb     Fixed offtarget compiler error due to stray ";"
02/10/12   sb      Changes to support FTM logging for GSM
============================================================================*/

#include "queue.h"
#include "target.h"
#include "msg.h"
#include "ftm.h"
#include "rf_test_task_v.h"
#include "ftm_log.h"
#include "rfgsm_core_log.h"
#include "ftm_task.h"
#include "rfgsm_core.h"
#include "log_codes.h"
#include "stringl.h"

extern uint32 gstmr_rd_qsymbol_count( void );

#ifdef FTM_HAS_LOGGING

static void rfgsm_core_log_free_buffer( void *msg);

// Declare a queue
static rfgsm_log_info_type  rfgsm_core_log_status;


// This function is called from rf_test_task.c  (FTM task) to initialize the resources for queue

void rfgsm_core_log_q_init( void)
{
   uint8    buf_count;
   //The specified queue is initialized
  (void)q_init( &rfgsm_core_log_status.rfgsm_log_q);
  (void)q_init( &rfgsm_core_log_status.rfgsm_log_free_q);

  for ( buf_count = 0; buf_count < RFGSM_LOG_BUFFERS; buf_count++)
  {

    (void)q_link( (void *) &rfgsm_core_log_status.rfgsm_buffers[buf_count],
                  &(rfgsm_core_log_status.rfgsm_buffers[buf_count].link) );

    q_put( &rfgsm_core_log_status.rfgsm_log_free_q,
           &(rfgsm_core_log_status.rfgsm_buffers[buf_count].link)) ;

  }
  
}

// This function "Allocates" a buffer for RFGSM to send messages to FTM.

void *rfgsm_core_log_get_buf( void)
{

  /* Get a buffer from the Free Q */
  void *msg = NULL;

  msg = q_get( &rfgsm_core_log_status.rfgsm_log_free_q);

  return( msg);

}


void rfgsm_core_log_send_to_ftm_task(rfm_device_enum_type rfm_dev)
{
  //uint32 start_time = gstmr_rd_qsymbol_count();
  //uint32 end_time = 0;
  rfgsm_core_log_data *message = NULL;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  message = ( rfgsm_core_log_data *) rfgsm_core_log_get_buf();	

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  /* Specify FTM log packet version number */
  rfgsm_core_handle_ptr->tx_log.ver_num = 2;

  if( message != NULL )
  {
    //copy the populated log message into the buffer item of queue
    memscpy( &(message->msg), sizeof(message->msg), &(rfgsm_core_handle_ptr->tx_log), sizeof(rfgsm_core_tx_log_type));
    (void)q_link( ( void *) message, &message->link);
    q_put( &rfgsm_core_log_status.rfgsm_log_q, &message->link );
    (void)rex_set_sigs( &ftm_tcb, FTM_GSM_Q_SIG);
  }

  //end_time = gstmr_rd_qsymbol_count();
  //MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_rfgsm_send_data_to_ftm() took %d qs", (end_time - start_time));
}




// This function dequeues an data block containing RFGSM - FTM task info, populates the RFGSM Log packet and sends the log packet to diag
void rfgsm_core_log_handle_q_from_rf()
{

    rfgsm_core_log_type *gsm_log = NULL;

    rfgsm_core_log_data  *message = NULL;

    //uint32 start_time = gstmr_rd_qsymbol_count();
    //uint32 end_time = 0;    
    while ( (message = q_get( &rfgsm_core_log_status.rfgsm_log_q)) != NULL)
    {
        gsm_log = (rfgsm_core_log_type *)ftm_log_malloc( (uint16)FTM_LOG_GSM_ENH, sizeof (rfgsm_core_log_type));

        if ( gsm_log != NULL )
        { 
            //------------------------------dequeue item from the the queue, latest q item retrieved---------//
            memscpy ( &(gsm_log->data), sizeof(gsm_log->data), &(message->msg), sizeof(rfgsm_core_tx_log_type));

            log_commit( ( ftm_log_type*) gsm_log );

        }
       
        rfgsm_core_log_free_buffer(( void *)message);
        
    }

    //end_time = gstmr_rd_qsymbol_count();
    //MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"rfgsm_handle_logs_from_rf() took %d qs", (end_time - start_time));

   
}

static void rfgsm_core_log_free_buffer( void *msg)
{

  /* Return a buffer to the free Q for future use */

  if( msg != NULL )
  {
      rfgsm_core_log_data  *message = ( rfgsm_core_log_data *) msg;
    
      (void)q_link( ( void *) message, &message->link);
    
      q_put( &rfgsm_core_log_status.rfgsm_log_free_q, &message->link);
  }

}
#endif /* FTM_HAS_LOGGING */
