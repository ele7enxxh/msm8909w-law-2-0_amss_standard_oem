/*===========================================================================

              GSM Circuit Switched Data XXXX Module

DESCRIPTION
  This file contains the implementation of the GSM Circuit switched 
  data XXXX functionality.

   Copyright (c) 2002 - 2014 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdtdata.c_v   1.12   26 Jun 2002 19:03:10   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsgcsdtdata.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/11   ttv     Support for Q6 free floating.
06/29/09   bs      Fixed off-target lint medium & lows.
09/01/06   ar      Add support for multiprocessor build.
03/13/06   ar      Pass CM call ID with incoming events.
11/21/05   ar      Add support for multiple call instances.   
05/30/04   ar      Lint corrections.
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#if defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)

#include "dsgcsdi.h"
#include "dsucsdlog.h"




/******************************************************************
** Definitions for the module
******************************************************************/
/* States of the Transparent FSM */
typedef enum
{
   IDLE_MODE                      = 0,
   WAITING_TO_SYNCHRONISE         = 1,
   WAITING_FOR_TIMEOUT            = 2,
   WAITING_FOR_DCD_ON             = 3,
   WAITING_FOR_FAX_TIMEOUT        = 4,
   USER_DATA_CONNECTION_ACTIVE    = 5,
   MO_TERMINATING                 = 6
/* TSTATE_UNDEFINED               = 0xFF */
} T_State_T;


#define SYNC_TIMEOUT_TICKS       500   // 500 msec timeout

// TODO: Need to find a better way to handle the FAX delay issue
/*
#ifdef DS_UCSD_DEBUG_RACAL_TEST
#define FAX_DELAY_TIMER_TICKS   100   // RACAL doesn't need big delay
#else
#define FAX_DELAY_TIMER_TICKS   3000  // 3 second fax delay timeout
#endif
*/


/* Integration timer structures */
typedef struct
{
   boolean            cts_on_timer;
   boolean            cts_off_timer;
   boolean            dcd_on_timer;
   boolean            dcd_off_timer;
} start_integration_timer_T;

typedef struct
{
   boolean            cts_on_timer;
   boolean            cts_off_timer;
   boolean            dcd_on_timer;
   boolean            dcd_off_timer;
} cancel_integration_timer_T;


/******************************************************************
** Module Scope Variables
******************************************************************/

LOCAL T_State_T               T_State     = IDLE_MODE;
LOCAL T_State_T               Old_T_State = IDLE_MODE;

LOCAL rx_status_T             Previous_rx_status = {0};
LOCAL byte                    Saved_X_status_rx = 0;
LOCAL byte                    Saved_SB_status_rx = 0;

/* Call mangaer call ID for mode handler events */
LOCAL cm_call_id_type         Call_id = CM_CALL_ID_INVALID;

/* Network data counters */
LOCAL uint32                  Tx_data_count = 0;
LOCAL uint32                  Rx_data_count = 0;

LOCAL ds_ucsd_set_data_counters_cb_T Set_data_counters_cb = NULL;


/******************************************************************
** External function prototypes
******************************************************************/





/***********************************************************************
 * Function name :  ds_gcsd_transparent_data_services_initialise
 * -------------
 * Description :    Initialise the variables for a new Trans. Call.
 * -----------
 * Parameters:      None
 * ----------
 * Returns:         None
 * --------
 **********************************************************************/
void
ds_gcsd_transparent_data_services_initialise
( 
  cm_call_id_type                    call_id,
  const cm_bearer_capability_s_type *bearer_cap_ptr,
  ds_ucsd_data_rate_T                data_rate,
  sys_sys_mode_e_type                curr_mode,
  ds_ucsd_set_data_counters_cb_T     set_data_counters_cb      
)
{
  ds_ucsd_sio_status_T   *CurrentModemControlLines_ptr;
  filtered_rx_status_T   *Filtered_rx_status;

  Filtered_rx_status           = ds_gcsd_get_Filtered_rx_status();
  CurrentModemControlLines_ptr = ds_gcsd_get_CurrentModemControlLines();

   ASSERT( NULL != bearer_cap_ptr );
  
   /* Preserve the mode handler call instance ID */
   Call_id = call_id;
  
   /* Set data traffic callback if specified */
   if( set_data_counters_cb )
   {
     Set_data_counters_cb       = set_data_counters_cb;
   }
   
   /* Initialise the Transparent Handler variables */
   T_State = IDLE_MODE;
   Old_T_State = IDLE_MODE;

   /* Initialise the storage for Modem Input Lines */
   CurrentModemControlLines_ptr->RTS   = V24_CONTROL_OFF;
   CurrentModemControlLines_ptr->DTR   = V24_CONTROL_OFF;

   ds_gcsd_set_CurrentModemControlLines(CurrentModemControlLines_ptr);
   ds_gcsd_set_OldModemControlLines(CurrentModemControlLines_ptr);
   
   /* Initialize received RX status info */
   Saved_X_status_rx               = V24_CONTROL_OFF;
   Saved_SB_status_rx              = V24_CONTROL_OFF;
   Previous_rx_status.X1_status_rx = V24_CONTROL_OFF;
   Previous_rx_status.X2_status_rx = V24_CONTROL_OFF;
   Previous_rx_status.S1_status_rx = V24_CONTROL_OFF;
   Previous_rx_status.S3_status_rx = V24_CONTROL_OFF;
   Previous_rx_status.S6_status_rx = V24_CONTROL_OFF;
   Previous_rx_status.S8_status_rx = V24_CONTROL_OFF;
   Previous_rx_status.S4_status_rx = V24_CONTROL_OFF;
   Previous_rx_status.S9_status_rx = V24_CONTROL_OFF;
   Previous_rx_status.E4_status_rx = V24_CONTROL_OFF;
   Previous_rx_status.E5_status_rx = V24_CONTROL_OFF;
   Previous_rx_status.E6_status_rx = V24_CONTROL_OFF;
   Previous_rx_status.E7_status_rx = V24_CONTROL_OFF;
   
   /* Turn off all output modem control lines */
   Filtered_rx_status->X_status_rx  = V24_CONTROL_OFF; /* CTS */
   Filtered_rx_status->SA_status_rx = V24_CONTROL_OFF; /* DSR */
   Filtered_rx_status->SB_status_rx = V24_CONTROL_OFF; /* DCD */ 
   /* Initialize the V24 (SIO interface) subsystem */
   ds_gcsd_V24_reset();   

   /* Initialise the counter-timers */
   INIT_TIMERS();

   /* Initialise rate adaption for uplink and downlink */
   ds_gcsd_init_RA1d_adaption_tx();
   ds_gcsd_init_RA1d_adaption_rx();
   
   /* Initialize logging for call */
   ds_ucsd_log_init
   (
     bearer_cap_ptr->number_of_data_bits,
     bearer_cap_ptr->number_of_stop_bits,
     bearer_cap_ptr->parity,
     TRUE,
     data_rate,
     curr_mode,
     0 /* RLP number is N/A */
   );
         
   ds_gcsd_init_RA0( bearer_cap_ptr, data_rate );

   // Initialize the data counters 
   Tx_data_count = 0;
   Rx_data_count = 0;
}


/*
** STATIC FUNCS
*/

/*
 * Function name :  ds_integration_period_timer()
 * -------------
 *
 * Description :   control signal filter integration timer function
 * -----------
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 * start_integration_timer
 * cancel_integration_timer
 *
 * Returns:
 * --------
 * none
 */

void
ds_gcsd_integration_period_timer
(
   start_integration_timer_T start_integration_timer,
   cancel_integration_timer_T cancel_integration_timer
)
{
   /*ref GSM 07.01 sect 8.2.2*/

   /*handles timing of integration periods for
    *filtering of CTS and DCD control signals to DTE*/

   if (start_integration_timer.cts_on_timer)
   {
      /*start 1 sec timer*/
      DS_UCSD_DEBUG_MSG ("Starting CTS ON timer");
      START_TIMER (TIMERID_CTS_INTEG_ON, TIMER_COUNT_1_SEC);
   }

   else if (cancel_integration_timer.cts_on_timer)
   {
      DS_UCSD_DEBUG_MSG ("Cancelling CTS ON timer");
      STOP_TIMER (TIMERID_CTS_INTEG_ON);
   }

   if (start_integration_timer.cts_off_timer)
   {
      /*start 1 sec timer*/
      DS_UCSD_DEBUG_MSG ("Starting CTS OFF timer");
      START_TIMER (TIMERID_CTS_INTEG_OFF, TIMER_COUNT_1_SEC);
   }

   else if (cancel_integration_timer.cts_off_timer)
   {
      DS_UCSD_DEBUG_MSG ("Cancelling CTS OFF timer");
      STOP_TIMER (TIMERID_CTS_INTEG_OFF);
   }

   if (start_integration_timer.dcd_on_timer)
   {
      /*start 200ms timer*/
      DS_UCSD_DEBUG_MSG ("Starting DCD ON timer");
      START_TIMER (TIMERID_DCD_INTEG_ON, TIMER_COUNT_200_MSEC);
   }

   else if (cancel_integration_timer.dcd_on_timer)
   {
      DS_UCSD_DEBUG_MSG ("Cancelling DCD ON timer");
      STOP_TIMER (TIMERID_DCD_INTEG_ON);
   }

   if (start_integration_timer.dcd_off_timer)
   {
      /*start 5 sec timer*/
      DS_UCSD_DEBUG_MSG ("Starting DCD OFF timer");
      START_TIMER (TIMERID_DCD_INTEG_OFF, TIMER_COUNT_5_SEC);
   }

   else if (cancel_integration_timer.dcd_off_timer)
   {
      DS_UCSD_DEBUG_MSG ("Cancelling DCD OFF timer");
      STOP_TIMER (TIMERID_DCD_INTEG_OFF);
   }
}


/*
 * Function name :  ds_filter_status_info()
 * -------------
 *
 * Description :
 * -----------
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 */
LOCAL
filtered_rx_status_T 
ds_gcsd_filter_status_info 
(
  const rx_status_T  *rx_status
)
{
   /*locals*/

   start_integration_timer_T        start_integration_timer;
   cancel_integration_timer_T       cancel_integration_timer;
   filtered_rx_status_T             filtered_rx_status;
   byte                             frame;
   byte                             max_frames;
   int                              transition;
   byte                             current_status;
   byte                             previous_status;
   byte                             bit_count;
   ds_ucsd_data_rate_T              Gcsd_curr_data_rate;

   Gcsd_curr_data_rate = ds_gcsd_get_Gcsd_curr_data_rate();

   /*ref GSM 07.01 sect 8.2.2 for spec of filtering*/

   /*check for completion of any integration timers*/
   
   if( TIMER_EXPIRED (TIMERID_CTS_INTEG_ON) )
   {
      /*stability period for CTS signal on has completed*/
      DS_UCSD_DEBUG_MSG ("Filter CTS ON expiry");
      STOP_TIMER (TIMERID_CTS_INTEG_ON);
      Saved_X_status_rx = 0;
   }

   else if (TIMER_EXPIRED (TIMERID_CTS_INTEG_OFF))
   {
      /*stability period for CTS signal off has completed*/
      DS_UCSD_DEBUG_MSG ("Filter CTS OFF expiry");
      STOP_TIMER (TIMERID_CTS_INTEG_OFF);
      Saved_X_status_rx = 1;
   }

   if (TIMER_EXPIRED (TIMERID_DCD_INTEG_ON))
   {
      /*stability period for DCD signal on has completed*/
      DS_UCSD_DEBUG_MSG ("Filter DCD ON expiry");
      STOP_TIMER (TIMERID_DCD_INTEG_ON);
      Saved_SB_status_rx = 0;
   }

   else if (TIMER_EXPIRED (TIMERID_DCD_INTEG_OFF))
   {
      /*stability period for DCD signal off has completed*/
      DS_UCSD_DEBUG_MSG ("Filter DCD OFF expiry");
      STOP_TIMER (TIMERID_DCD_INTEG_OFF);
      Saved_SB_status_rx = 1;
   }


   /*clear all start and stop timer flags*/

   start_integration_timer.cts_on_timer = FALSE;
   start_integration_timer.cts_off_timer = FALSE;
   start_integration_timer.dcd_on_timer = FALSE;
   start_integration_timer.dcd_off_timer = FALSE;

   cancel_integration_timer.cts_on_timer = FALSE;
   cancel_integration_timer.cts_off_timer = FALSE;
   cancel_integration_timer.dcd_on_timer = FALSE;
   cancel_integration_timer.dcd_off_timer = FALSE;

   /*set defaults for filtered_rx_status*/

   filtered_rx_status.X_status_rx = Saved_X_status_rx;
   filtered_rx_status.SB_status_rx = Saved_SB_status_rx;

   /*set SA bit to value of S1, S3, S6, S8 bits in rx_status
    *no filtering is necessary for SA bits
    *just set SA bit to value of last bit received (S8)*/

   filtered_rx_status.SA_status_rx = rx_status[0].S8_status_rx;


   if (Gcsd_curr_data_rate == DS_UCSD_RATE_9600)
   {
      max_frames = 4;
   }

   else if (Gcsd_curr_data_rate == DS_UCSD_RATE_14400)
   {
      max_frames = 1;
   }
   else
   {
      max_frames = 2;
   }

   /*filter status bits for CTS signal*/

   previous_status = Previous_rx_status.X2_status_rx;

   transition = 0;

   for (frame = 0; frame < max_frames; frame++)
   {
      current_status = rx_status[frame].X1_status_rx;

      /*
       * loop for all the DCD bits in the frame
       */

      for ( bit_count = 0; bit_count < 2; bit_count++ )
      {
         if ( ( current_status == 1 ) &&
              ( previous_status == 0 ) )
         {
            /*
             * its a 0 to 1 transition
             */

            transition = transition + 1;

#ifdef LOGDEBUG
#ifdef HOST
            if ( transition > 1 )
            {
               gs_raise_warning(GS_UNIQUE_EXCEPTION_NUMBER,
                                transition,
                                "Logical error - too many rising edges\n");
            }
#endif
#endif
         }
         else
         if ( ( current_status == 0 ) &&
              ( previous_status == 1 ) )
         {
            /*
             * its a 1 to 0 transition
             */

            transition = transition - 1;

#ifdef LOGDEBUG
#ifdef HOST
            if ( transition < -1 )
            {
               gs_raise_warning(GS_UNIQUE_EXCEPTION_NUMBER,
                                transition,
                                "Logical error - too many falling edges\n");
            }
#endif
#endif
         }
         else
         {
            /* EMPTY */
         }

         previous_status = current_status;
         current_status = rx_status[frame].X2_status_rx;
      }
   }

   switch (transition)
   {
      case 1:

         /*
          * Check if this is a change from the filtered output, if it
          * is we need to start the off timer, otherwise its just a
          * transition to the current filtered output value so the
          * transition constitutes a glitch and can be ignored
          * If this is a return to the original state, then need to
          * cancel the timer that may have been started
          */

         if (filtered_rx_status.X_status_rx == 0 )
         {
            start_integration_timer.cts_off_timer = TRUE;
         }
         else
         {
            cancel_integration_timer.cts_on_timer = TRUE;
         }
         break;

      case -1:

         /*
          * and opposite case for 1->0 transitions
          */

         if (filtered_rx_status.X_status_rx == 1 )
         {
            start_integration_timer.cts_on_timer = TRUE;
         }
         else
         {
            cancel_integration_timer.cts_off_timer = TRUE;
         }
         break;

      case 0:
         break;

      default:
#ifdef HL_DEBUG
#error code not present
#endif
         break;
   }

   /*save current X1, X2 as previous values for next block*/

   Previous_rx_status.X1_status_rx = rx_status[frame-1].X1_status_rx;
   Previous_rx_status.X2_status_rx = rx_status[frame-1].X2_status_rx;

   /*filter status bits for DCD signal*/

   previous_status = Previous_rx_status.S9_status_rx;

   transition = 0;

   for (frame = 0; frame < max_frames; frame++)
   {
      current_status = rx_status[frame].S4_status_rx;

      /*
       * loop for all the DCD bits in the frame
       */

      for ( bit_count = 0; bit_count < 2; bit_count++ )
      {
         if ( ( current_status == 1 ) &&
              ( previous_status == 0 ) )
         {
            /*
             * its a 0 to 1 transition
             */

            transition = transition + 1;

#ifdef LOGDEBUG
#ifdef HOST
            if ( transition > 1 )
            {
               gs_raise_warning(GS_UNIQUE_EXCEPTION_NUMBER,
                                transition,
                                "Logical error - too many rising edges\n");
            }
#endif
#endif
         }
         else
         if ( ( current_status == 0 ) &&
              ( previous_status == 1 ) )
         {
            /*
             * its a 1 to 0 transition
             */

            transition = transition - 1;

#ifdef LOGDEBUG
#ifdef HOST
            if ( transition < -1 )
            {
               gs_raise_warning(GS_UNIQUE_EXCEPTION_NUMBER,
                                transition,
                                "Logical error - too many falling edges\n");
            }
#endif
#endif
         }
         else
         {
            /* EMPTY */
         }

         previous_status = current_status;
         current_status = rx_status[frame].S9_status_rx;
      }
   }

   switch (transition)
   {
      case 1:     /* 0 -> 1 */

         /*
          * Check if this is a change from the filtered output, if it
          * is we need to start the off timer, otherwise its just a
          * transition to the current filtered output value so the
          * transition constitutes a glitch and can be ignored
          * If this is a return to the original state, then need to
          * cancel the timer that may have been started
          */

         if (filtered_rx_status.SB_status_rx == 0 )
         {
            start_integration_timer.dcd_off_timer = TRUE;
         }
         else
         {
            cancel_integration_timer.dcd_on_timer = TRUE;
         }
         break;

      case -1:    /* 1 -> 0 */

         /*
          * and opposite case for 1->0 transitions
          */

         if (filtered_rx_status.SB_status_rx == 1 )
         {
            start_integration_timer.dcd_on_timer = TRUE;
         }
         else
         {
            cancel_integration_timer.dcd_off_timer = TRUE;
         }
         break;

      case 0:
         break;

      default:
#ifdef HL_DEBUG
#error code not present
#endif
         break;
   }


   /*save current S4, S9 as previous values for next block*/

   Previous_rx_status.S4_status_rx = rx_status[frame-1].S4_status_rx;
   Previous_rx_status.S9_status_rx = rx_status[frame-1].S9_status_rx;

   /* start or stop integration period timers*/

   ds_gcsd_integration_period_timer (start_integration_timer,
                                     cancel_integration_timer);



   return filtered_rx_status;
}



/*
 * Function name :  ds_control_receiving_user_data()
 * -------------
 *
 * Description :    control function for receiving user data in DS task
 * -----------
 *
 * Uses : 
 * ----
 *
 * Parameters:  
 * ----------
 *
 * Returns:    
 * --------
 *
 */

boolean 
ds_gcsd_control_receiving_user_data 
(
   boolean e_receive_sync_pattern,
   boolean enable_user_data_connection
)
{
   /*locals*/

   boolean               rx_error;
   boolean               c_sync_pattern_received = FALSE;
   rx_status_T           rx_status[4];
   sync_rx_data_T        sync_rx_data[BYTES_PER_14400_20_MSEC / 4 + 1];
   filtered_rx_status_T  *Filtered_rx_status;
   Filtered_rx_status = ds_gcsd_get_Filtered_rx_status();

   /* Receive data */  
   c_sync_pattern_received = ds_gcsd_RA1d_adaption_rx (e_receive_sync_pattern,
                                                       sync_rx_data,
                                                       rx_status,
                                                       &rx_error);
   if (rx_error)
   {
      return FALSE;
   }
   
   if (!e_receive_sync_pattern)
   {
      *Filtered_rx_status = ds_gcsd_filter_status_info(rx_status);
   }
   if (enable_user_data_connection)
   {
      /*only run RA0 rate adaption if on-line*/

      ds_gcsd_setup_online_modem_control();
      Rx_data_count += ds_gcsd_RA0_rx_data(sync_rx_data);
   }

   
   return c_sync_pattern_received;
}



/*
 * Function name :  ds_control_transmitting_user_data()
 * -------------
 *
 * Description :    control function for transmitting user data in DS task
 * -----------
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 * negotiated_capability
 * e_send_sync_pattern
 * enable_user_data_connection
 *
 * Returns:
 * --------
 *  Nothing
 */

void 
ds_gcsd_control_transmitting_user_data
(
   boolean               e_send_sync_pattern,
   boolean               enable_user_data_connection,
   ds_ucsd_sio_status_T  *ModemControl
)
{
   /*locals*/
   register int32          i;
   uint8       *radio_tx_data;
   ds_ucsd_sio_status_T  v24_status;
   tx_status_T             tx_status;
   sync_tx_data_T          sync_tx_data[BYTES_PER_14400_20_MSEC / 4 + 1];


   /*process a block of user data from terminal every 20msec*/

   
   /* Get the buffer from L1 to add the transmit data to... if this call fails,
   ** no need to go any further
   */
   if (!ds_gcsd_get_tx_buffer (&radio_tx_data))
   {
      DATA_MSG0_ERROR("GCSD RA1' TX: ds_gcsd_get_tx_buffer call failed");
      return;
   }
      
   (void)ds_gcsd_update_serial_status (&v24_status);
   ModemControl->DTR = tx_status.SA_status_tx = v24_status.DTR;
   ModemControl->RTS = tx_status.SB_status_tx = v24_status.RTS;
   
   
   /* use real user data if instructed to */
   if (enable_user_data_connection)
   {
      Tx_data_count += ds_gcsd_RA0_tx_data(sync_tx_data);
   }
   
   /* Otherwise, just use all stop bits as user data 
   ** if we aren't going to be sending the sync pattern 
   */
   else if (!e_send_sync_pattern)
   {
      for (i = 0; i < BYTES_PER_14400_20_MSEC / 4 + 1; i++)
      {
         sync_tx_data[i] = (sync_tx_data_T)0xFFFFFFFF;
      }
   }

   
   ds_gcsd_RA1d_adaption_tx(sync_tx_data,
                            tx_status,
                            e_send_sync_pattern,
                            radio_tx_data);

}


 
/* <EJECT> */
/*===========================================================================

FUNCTION ds_gcsd_user_data_with_rate_adaption

DESCRIPTION
   Do a Tx/Rx cycle of data with full rate adaption.
   This is used by Transparent calls, which add start/
   stop/parity etc. bits to the data stream in the 
   ds tx and rx buffers.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the T-data sync pattern is received, FALSE if not.

SIDE EFFECTS
  Yes... lots

===========================================================================*/
LOCAL boolean
ds_gcsd_user_data_with_rate_adaption 
(
   boolean                    e_send_receive_sync_pattern,
   boolean                    enable_user_data_connection,
   boolean                    process_uplink,
   boolean                    process_downlink
)
{
   boolean c_sync_pattern_received = FALSE;
   static uint8 traffic_report_cnt = 0;
   ds_ucsd_sio_status_T *CurrentModemControlLines_ptr;

   CurrentModemControlLines_ptr = ds_gcsd_get_CurrentModemControlLines();

   if (process_uplink)
   {
      
      ds_gcsd_control_transmitting_user_data
                              (e_send_receive_sync_pattern,
                               enable_user_data_connection,
                               CurrentModemControlLines_ptr);
      
   }
   
   if (process_downlink)
   {
      c_sync_pattern_received =
         ds_gcsd_control_receiving_user_data
                              (e_send_receive_sync_pattern,
                               enable_user_data_connection);
   }

   /* Update call instance data traffic counters at set interval */
   if( (Set_data_counters_cb) &&
       (GCSD_TRAFFIC_REPORTING_INTERVAL == traffic_report_cnt++) )
   {
     if( !Set_data_counters_cb( Call_id,
				FALSE,
                                Rx_data_count,
                                Tx_data_count ) )
     {
       DATA_MSG1_ERROR("Problem updating data traffic counters: call ID=%d",
                        Call_id);
     }
     traffic_report_cnt = 0;
   }

   return c_sync_pattern_received;
}


 
   

/*********************************************************************
 * Function name :  ds_gcsd_control_transparent_data_services
 * -------------
 * Description :    Main FSM for Transparent data calls
 * -----------      
 * Parameters:      ISR Event
 * ----------
 * Returns:         None
 * --------
 *********************************************************************/
void 
ds_gcsd_control_transparent_data_services 
(
   DSI_Event_T               Event,
   uint8                     cm_call_id,
   boolean                   process_uplink,
   boolean                   process_downlink
)
{
   
   boolean                    c_sync_pattern_received;
   filtered_rx_status_T *Filtered_rx_status;
   Filtered_rx_status = ds_gcsd_get_Filtered_rx_status();

   switch (T_State)
   {
      /********************************/
      case IDLE_MODE:
      /********************************/
         switch (Event)
         {
            case DS_UMTS_CSD_CONNECT_EVENT:               
               T_State = WAITING_TO_SYNCHRONISE;
               break;
               
            case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
               /* A call terminate from the user  - terminate immediately */
               /* Use passed call_id as stack has not been initialized yet. */
               ds_gcsd_send_event_to_DS( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                         cm_call_id );
               break;

            default:
               break;  /* make lint happy */
         }
         
         break;

      /************************************/
      case WAITING_TO_SYNCHRONISE:
      /************************************/
      /* Send/receive sync-data */
      c_sync_pattern_received = 
            ds_gcsd_user_data_with_rate_adaption (TRUE, 
                                                  FALSE,
                                                  process_uplink,
                                                  process_downlink);


      if (process_downlink && c_sync_pattern_received)
      {
         START_TIMER (TIMERID_SYNC,SYNC_TIMEOUT_TICKS);
         T_State = WAITING_FOR_TIMEOUT;
      }

      switch (Event)                      {
         case DS_UMTS_CSD_DISCONNECT_EVENT:
            T_State = IDLE_MODE;
            break;
            
         case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
            ds_gcsd_send_event_to_DS( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                      Call_id );
            T_State = IDLE_MODE;
            break;

         default:
            break;  /* make lint happy */
      }
      break;


      
      /********************************/
      case WAITING_FOR_TIMEOUT:
      /********************************/
         /* Continue sending sync-pattern for timeout period */
         (void) ds_gcsd_user_data_with_rate_adaption (TRUE, 
                                                      FALSE,
                                                      process_uplink,
                                                      process_downlink);

         if (TIMER_EXPIRED (TIMERID_SYNC))
         {
            T_State = WAITING_FOR_DCD_ON;
            STOP_TIMER (TIMERID_SYNC);
         }

         switch (Event)
         {
            case DS_UMTS_CSD_DISCONNECT_EVENT:
               T_State = IDLE_MODE;
               break;
               
            case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
               ds_gcsd_send_event_to_DS( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                         Call_id );
               T_State = IDLE_MODE;
               break;
               
            default:
               break;  /* make lint happy */
         }
         break;



      /************************************/
      case WAITING_FOR_DCD_ON:
      /************************************/
   
         (void) ds_gcsd_user_data_with_rate_adaption(FALSE, 
                                                     FALSE,
                                                     process_uplink,
                                                     process_downlink);
                                                     
         /* When DCD goes on, transition to active connection state */
         if (Filtered_rx_status->SB_status_rx == V24_CONTROL_ON)
         {
            
            /* If this is a FAX call, do an extra delay before going to the
            ** connected state.  This was found to be necessary while doing
            ** field testing.  We will just re-use the SYNC timer ID since it
            ** is not otherwise used in this state.
            */
            //if (config_data->negotiated_capability.bearer_capability_1.
            //    information_transfer_capability == ITC_FAX_G3)
            //{
            //   START_TIMER (TIMERID_SYNC,FAX_DELAY_TIMER_TICKS);
            //   T_State = WAITING_FOR_FAX_TIMEOUT;
            //}
            
            /* Not a FAX call... go to connected state */
            //else
            //{
               T_State = USER_DATA_CONNECTION_ACTIVE;
               ds_gcsd_send_event_to_DS( DS_UMTS_CSD_CONNENCT_COMPLETE_IND,
                                         Call_id );
            //}
         }

         switch (Event)
         {
            case DS_UMTS_CSD_DISCONNECT_EVENT:
               T_State = IDLE_MODE;
               break;
               
            case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
               ds_gcsd_send_event_to_DS( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                         Call_id );
               T_State = IDLE_MODE;
               break;
               
            default:
               break;  /* make lint happy */
         }
      break;
      
      /***********************************************/
      case WAITING_FOR_FAX_TIMEOUT:
      /***********************************************/
         (void) ds_gcsd_user_data_with_rate_adaption(FALSE, 
                                                     FALSE,
                                                     process_uplink,
                                                     process_downlink);
         if (TIMER_EXPIRED (TIMERID_SYNC))
         {
            T_State = USER_DATA_CONNECTION_ACTIVE;
            ds_gcsd_send_event_to_DS( DS_UMTS_CSD_CONNENCT_COMPLETE_IND,
                                      Call_id );
            STOP_TIMER (TIMERID_SYNC) ;
         }
         
         switch (Event)
         {
            case DS_UMTS_CSD_DISCONNECT_EVENT:
               T_State = IDLE_MODE;
               break;
               
            case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
               ds_gcsd_send_event_to_DS( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                         Call_id );
               T_State = IDLE_MODE;
               break;
               
            default:
               break;  /* make lint happy */
         }
      break;
      /***********************************************/
      case USER_DATA_CONNECTION_ACTIVE:
      /***********************************************/
         ds_gcsd_CheckModemControlLines ();
   

         (void) ds_gcsd_user_data_with_rate_adaption (FALSE, 
                                                      TRUE, 
                                                      process_uplink,
                                                      process_downlink);

         
         
         switch (Event)
         {
            // If we go into online command mode, suspend transmission to SIO   
            case DS_UMTS_CSD_ONLINE_CMD_MODE_EVENT:
               ds_gcsd_suspend_data_to_sio(TRUE);
               break;

            // If we come out of online command mode (back to online mode),
            // resume transmission to SIO
            case DS_UMTS_CSD_ONLINE_DATA_MODE_EVENT:
               ds_gcsd_suspend_data_to_sio(FALSE);
               break;


            case DS_UMTS_CSD_DISCONNECT_EVENT: 
               T_State = IDLE_MODE;
               break;

            /* case L2R_DTE_DTR_LOW_IND: */
            case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
               /* DTR call termination. Flush data before disconnecting */
               /*
               START_TIMER (TIMERID_MO_FLUSH,
                            ETBM_param_ptr->timer);
               */
               T_State = MO_TERMINATING;
               break;

                
            case DS_UMTS_CSD_NULL_EVENT:
               // If DCD goes off, drop call
               if  ( (process_downlink) &&
                     (Filtered_rx_status->SB_status_rx == V24_CONTROL_OFF) )
               {
                  ds_gcsd_send_event_to_DS( DS_UMTS_CSD_TERMINATE_CALL_IND,
                                            Call_id );
                  T_State = IDLE_MODE;
               }
               break;
               
            default:
               break;  /* make lint happy */
         }
         break;


      /**************************/
      case MO_TERMINATING:
      /**************************/
         ds_gcsd_CheckModemControlLines ();
         
         /* Handle online data */
         (void) ds_gcsd_user_data_with_rate_adaption (FALSE, 
                                                      TRUE,
                                                      process_uplink,
                                                      process_downlink);


         /* Check for flush terminated conditions */
        
         if ((process_uplink) &&
             (!(ds_gcsd_tx_transparent_data_read_from_serial_port())))
         {
            ds_gcsd_send_event_to_DS( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                      Call_id );
            T_State = IDLE_MODE;
         }
       
            
      break;

      

      /**************************/
      default:  /* UNKNOWN STATE   */
      /**************************/
      break;
   }
   
   /* Debug state machine changes and/or events */
   if ( (T_State != Old_T_State) ||
        (Event != (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT) )
   {
     DS_UCSD_DEBUG_MSG_3 ("Start T_St=%d, End T_St=%d, Evt=%d",
                          Old_T_State, T_State,Event);
   }
   
   /* If we transition to idle state, reset SIO */
   if (T_State != Old_T_State)
   {   
      if (T_State == IDLE_MODE)
      {
         ds_gcsd_V24_reset();
      }
      
      Old_T_State = T_State;
   }   

}



#endif /* defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD) */
