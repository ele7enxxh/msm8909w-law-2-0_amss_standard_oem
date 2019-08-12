/*===========================================================================

              GSM Circuit Switched Data XXXX Module

DESCRIPTION
  This file contains the implementation of the GSM Circuit switched 
  data XXXX functionality.

Copyright (c) 2002 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdsio.c_v   1.8   27 Jun 2002 16:46:56   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsgcsdsio.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/12   ND      Serializing modification of global var Curr_*_dsm_item in
                   function ds_gcsd_V24_reset() which is called from two task context.
04/01/11   TTV     Support for Q6 free floating.
11/07/08   DH      Porting code for Q6, off-target build.
06/06/08   SA      Migrated INTLOCK for V.24 modem control to Rex Critical section.
11/15/07   AR      Use safety macros for watermark operations.
26/02/07   DA      Fixed high lint errors
08/29/06   AR      Add critical section around V.24 state changes.
08/22/06   AR      Changes to supprot multiprocessor architecture.
07/10/06   AR      Add Gcsd_curr_call_state test to serial state query.
04/21/04   AR      Lint cleanup
01/23/03   TMR     Removed some debug messages
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/



/*******************************************************************************
 *
 *  File:           /home/briers/gsmqc/ds/SCCS/s.m51201.c
 *  -----
 *
 *  SCCS Version:   2.19
 *  -------------
 *
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained herein
 *  is confidential.  The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of Communications Consultants Worldwide Limited (C) 2000
 *
 *  Project:        P60
 *  --------
 *
 *  Software Unit:  GSM/MS/DS
 *  --------------
 *
 *  Description:    function for receiving data from V24 interface
 *  ------------
 *
 *  Functions Used:
 *  ---------------
 *
 *  Comment:
 *  --------
 *
 *  Modification History:
 *  ---------------------
 *  Date       Vers  Auth   Details
 *  ----       ----  ----   -------
 * 01-Jun-97  2.1   RAC  Original version
 * 05-Jul-97  2.2   RAC  add write_to_tx_buffer function
 * 07-Jun-97  2.3   DBJ  Changed Command Line i/p, add call to
 *                       ds_gcsd_read_from_serial_port in on-line mode,
 *                       correct calculation of number of data and start bits,
 *                       don't write anything to tx_buffer if no valid user data
 * 14-Jul-97  2.4   RAC  break out of loop if escape sequence recognised
 * 16-Jul-97  2.5   RAC  generate break signal
 * 17-Jul-97  2.7   APB  Correct calculation of no of chars in block
 * 26-Aug-98  2.8   BP   Add off_line_data_type
 * 02-Nov-98  2.9   SFL  Include cm_reg.h
 * 11-Nov-98  2.10  BP   Correction to ^Z value
 * 17-Jun-99  2.11  SFL  Allow user to modify ESC, CR and LF values
 * 01-Dec-99  2.12  BP   Allow command line termination with '/'.
 * 26-Jul-00  2.13  RCB  Update file header
 * 26-Jul-00  2.14  RCB  Update file header
 * 17-Aug-00  2.15  RCB  Add ds_V24_interface_handler_byte_oriented()
 * 30-Aug-00  2.16  RCB  Handle CTRLZ character in DS_ASCII_PACKET_DATA mode
 *                       Add ds_filter_plus_plus_plus()
 * 12-Sep-00  2.17  DBT  Added programmable +++ delay (AT&S12)
 * 12-Sep-00  2.18  DBT  Filter debug output
 * 27-Sep-00  2.19  SFL  Change ampersand_s12 to s12_register
 *
 ******************************************************************************/
 
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS)

#include "dsucsdi.h"
#include "dsucsdnti.h"
#include "dsgcsdi.h"
#include "msg.h"
#include "dsgcsd_task_v.h"
#include "ds3gsiolib_ex.h"

#include "ds3gsiolib.h"



/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/

typedef struct
{
   uint8               CTS;
   uint8               DSR;
   uint8               DCD;
   /* used on serial output only */
/* uint8               BREAK; 
   uint8               DATA_INVALID;
   uint8               RI;       */
} ds_gcsd_async_control_T;

#define GCSD_V24_STATE_QUERY_INTERVAL  0x0F

/*locals*/

LOCAL  ds_gcsd_async_control_T   Saved_V24_control = {0};

/* extern'd locals -- now declared in dsgcsdsio_iram.c */

/* Macro to enable(TRUE)/disable(FALSE) inbound flow control */
#define SET_INBOUND_FLOW( state )                                       \
     ds3g_siolib_ex_set_inbound_flow(DS_FLOW_GCSD_MASK,                    \
                                  (state)? DS_FLOW_ENABLE : DS_FLOW_DISABLE,\
                                  ds3g_siolib_ex_get_csd_port());

/* Declare a task critical section for serialized code execution. */
static rex_crit_sect_type ds_gcsd_v24_modem_control_crit_sect = {{NULL}};

/* Enter critical section to ensure serialized exection. */
#define DS_GCSD_V24_MODEM_CONTROL_LOCK()                       \
   rex_enter_crit_sect(&ds_gcsd_v24_modem_control_crit_sect); 

/* Allow waiting task to proceed. */
#define DS_GCSD_V24_MODEM_CONTROL_UNLOCK()                     \
   rex_leave_crit_sect( &ds_gcsd_v24_modem_control_crit_sect );


/* <EJECT> */                       
/*===========================================================================

                            FUNCTION DEFINITIONS
   
===========================================================================*/

/*===========================================================================

FUNCTION DS_GCSD_SIO_INIT

DESCRIPTION
  This function initializes the task critical section for V.24 modem 
  control. It is called once during powerup.

DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void ds_gcsd_sio_init( void )
{
  /* Initialize task critical section for V24 modem control */
  memset(&ds_gcsd_v24_modem_control_crit_sect, 0, sizeof(rex_crit_sect_type));
  rex_init_crit_sect( &ds_gcsd_v24_modem_control_crit_sect );
} /* ds_gcsd_sio_init() */

void 
ds_gcsd_V24_reset()
{
   dsm_watermark_type  *Gcsd_dl_wm_ptr = NULL;
   dsm_watermark_type  *Gcsd_ul_wm_ptr = NULL;
   dsm_watermark_type  *Gcsd_fax_sio_dl_wm_ptr = NULL;
   dsm_watermark_type  *Gcsd_fax_sio_ul_wm_ptr = NULL;

   DS_GCSD_V24_MODEM_CONTROL_LOCK()
   Gcsd_dl_wm_ptr         = ds_gcsd_get_Gcsd_dl_wm_ptr();
   Gcsd_ul_wm_ptr         = ds_gcsd_get_Gcsd_ul_wm_ptr();
   Gcsd_fax_sio_dl_wm_ptr = ds_gcsd_get_Gcsd_fax_sio_dl_wm_ptr();
   Gcsd_fax_sio_ul_wm_ptr = ds_gcsd_get_Gcsd_fax_sio_ul_wm_ptr();
   /* Initialize all file static variables */
   ds_gcsd_set_Curr_rx_dsm_length(0);

   // Initialize the count of buffered RX data
   ds_gcsd_set_OTA_rx_buffer_cnt(0);

   // Initialize the watermark state variables
   ds_gcsd_set_SIO_high_WM_state(FALSE);

   // Turn off suspension of data to SIO
   ds_gcsd_set_Suspend_data_to_sio(FALSE);
      
   
   /* Delete any current DSM items */
   ds_gcsd_clean_Curr_dsm_item();
   
   /* Delete any remaining DSM items in the watermarks */
   EMPTY_WATERMARK( Gcsd_ul_wm_ptr );
   EMPTY_WATERMARK( Gcsd_dl_wm_ptr );
   EMPTY_WATERMARK( Gcsd_fax_sio_ul_wm_ptr );
   EMPTY_WATERMARK( Gcsd_fax_sio_dl_wm_ptr );

   /* Turn off DCD, but DSR and CTS go on */
   Saved_V24_control.CTS = V24_CONTROL_ON;
   Saved_V24_control.DSR = V24_CONTROL_ON;
   Saved_V24_control.DCD = V24_CONTROL_OFF;
 
   DS_GCSD_V24_MODEM_CONTROL_UNLOCK()
   
   /* Enable flow control */
   SET_INBOUND_FLOW( TRUE ); /*lint !e506 */
}
   
/*
*   Function name:  ds_gcsd_update_status
*   --------------
*
*   Description:  Gets serial line status from hardware
*   ------------
*
*
*   Parameters:
*   -----------
*
*
*
*   Returns:
*   --------
*
*
*/

boolean
ds_gcsd_update_serial_status 
(
   ds_ucsd_sio_status_T *status
)
{
   static ds_ucsd_sio_status_T   saved_status   = { 0, 0 };
   static uint8                  count          = 0;
   
          boolean                dtr_status     = FALSE;
        //boolean                rts_status     = FALSE;
          boolean                rc             = TRUE;
   
   /* If upper layer call control is not connected, return the previous
   ** status and exit immediately.
   */
   if( GCSD_CALL_STATE_CONNECT != ds_gcsd_get_Gcsd_curr_call_state() )
   {
     *status = saved_status;
     return rc;
   }
   
   /* If the counter is 0, actually read RTS and DTR, otherwise,
   ** we will return the previously saved values.  We will actually
   ** only poll these values every GCSD_V24_STATE_QUERY_INTERVAL calls.
   */
   if (count++ == 0)
   {
      if (ds3g_siolib_ex_is_dtr_asserted (&dtr_status,
                                           ds3g_siolib_ex_get_csd_port()))
      {
         rc = FALSE;
      }
      
      /* TODO:  Getting RTS status is currently broken... always returns OFF...
      ** this isn't good, so for now, we will asssume that RTS follows
      ** the DTR state (hopefully the TE can keep up with the blazing
      ** 14.4 Kbps max data rate)
#ifdef FEATURE_DATA_MM
      if (ds3g_siolib_is_rts_asserted (&rts_status) != DS3G_SIOLIB_OK)
#else 
      if (ds3g_sio_is_rts_asserted (&rts_status) != DS3G_SIO_OK)
#endif
      {
         rc = FALSE;
      }
      */
      
      if (rc)
      {
         status->DTR = saved_status.DTR = ((dtr_status) ? V24_CONTROL_ON : V24_CONTROL_OFF);
         status->RTS = saved_status.RTS = saved_status.DTR;
         //status->RTS = saved_status.RTS = (rts_status) ? V24_CONTROL_ON : V24_CONTROL_OFF;
      }
   }
   else
   {
      *status = saved_status;
      
      // Roll the counter back to 0 every 15 times...
      count &= GCSD_V24_STATE_QUERY_INTERVAL;
   }
   
   return rc;
}


void ds_gcsd_set_cts_state  
(
   boolean on_off
)
{
   filtered_rx_status_T *Filtered_rx_status;
   Filtered_rx_status = ds_gcsd_get_Filtered_rx_status();
   if (on_off)
   {
      Filtered_rx_status->X_status_rx = V24_CONTROL_ON;
   }
   else
   {
      Filtered_rx_status->X_status_rx = V24_CONTROL_OFF;
   }
   
   ds_gcsd_setup_online_modem_control();
}
      
/****************************************************************************
 * Function name:  ds_setup_online_ds_gcsd_modem_control
 * -------------
 * Description:    Sets up V24 modem control for use when calling ghdi.
 * -----------     Applies hysteresis to CTS line.
 * Parameters:     V24_control structure, filtered_rx_status
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
void 
ds_gcsd_setup_online_modem_control (void)
{
  boolean                  v24_state_changed = FALSE;
  ds_gcsd_async_control_T  v24_control;
  filtered_rx_status_T     *Filtered_rx_status;
  boolean                  SIO_high_WM_state;

  Filtered_rx_status = ds_gcsd_get_Filtered_rx_status();

  /* Enter critical section for modifying global state.
   * THIS LOCK INTERVAL MUST BE KEPT SHORT TO AVOID PERFORMACE PROBLEMS. */

  DS_GCSD_V24_MODEM_CONTROL_LOCK()

  /* Not currently used... set control signals */
  v24_control.DCD = Filtered_rx_status->SB_status_rx;
   
  /* TODO:  CCWW did not use SA_status_rx to determine DSR... we won't either
  ** for the time being.
  */
  v24_control.DSR = V24_CONTROL_ON;
   
   
  /* Handle CTS with hysteresis */
  SIO_high_WM_state = ds_gcsd_get_SIO_high_WM_state();
  if ((Filtered_rx_status->X_status_rx == V24_CONTROL_OFF) ||
      (SIO_high_WM_state == TRUE) )
  {
    v24_control.CTS = V24_CONTROL_OFF;
  }
  else
  {
    v24_control.CTS = V24_CONTROL_ON;
  }

  if (Saved_V24_control.CTS != v24_control.CTS)
  {
    v24_state_changed = TRUE;
    Saved_V24_control = v24_control;
  }

  DS_GCSD_V24_MODEM_CONTROL_UNLOCK()
    
  /* Check if V.24 state changed */ 
  if( v24_state_changed )
  {
      
    if (v24_control.CTS == V24_CONTROL_ON)
    {
      MSG_LOW("Enable SIO Flow: HighWm=%d CTS=%d CTS(Saved)=%d",
              SIO_high_WM_state, v24_control.CTS, Saved_V24_control.CTS);
      SET_INBOUND_FLOW( TRUE ); /*lint !e506 */
    }
    else
    {
      MSG_LOW("Disable SIO Flow: HighWm=%d CTS=%d CTS(Saved)=%d",
              SIO_high_WM_state, v24_control.CTS, Saved_V24_control.CTS);
      SET_INBOUND_FLOW( FALSE ); /*lint !e506 */
    }
  }
  else
  {
    MSG_LOW("Skipping WM flow control op: HighWm=%d CTS=%d CTS(Saved)=%d",
            SIO_high_WM_state, v24_control.CTS, Saved_V24_control.CTS );
  }

}

#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */
