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
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsgcsdsio_iram.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/12   KV      AEEStd usage for sting fucntions.
04/01/11   TTV     Support for Q6 free floating.
11/02/09   SA      Replacing snprintf with std_snprintf.
11/15/07   AR      Use safety macros for watermark operations.
26/02/07   DA      Fixed high lint errors
09/01/06   AR      Add support for multiprocessor build.
08/17/06   AR      Add Gcsd_curr_call_state test to serial port read.
05/03/04   AR      Add SIO watermark level to Rx buffer full test. Lint fixes
08/05/03   TMR     Updated watermark callbacks for DSM 2
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


#include "dsucsdnti.h"
#include "dsgcsdi.h"
#include "msg.h"
#include "amssassert.h"
#include "data_msg.h"
#include "ds3gsiolib.h"
#include "ds3gsiolib_ex.h"


/*locals*/

static dsm_item_type          *Curr_rx_dsm_item = NULL;
static word                   Curr_rx_dsm_length = 0;
static dsm_item_type          *Curr_tx_dsm_item = NULL;
static boolean                SIO_high_WM_state = FALSE;
static uint32                 OTA_rx_buffer_cnt = 0;

// Used to turn transmission of data received over the air to SIO on/off
static boolean                Suspend_data_to_sio = FALSE;

#ifdef FEATURE_DATA_UCSD_DEBUG_SIODUMP
#include <stdio.h>

#define BUFSIZE   128
#define MAX_BUFFS 4
#define PASS_MOD  80
/* Need seperate buffers for sprintf */
static char buffer[BUFSIZE]  = {NULL};
static char buffer0[BUFSIZE] = {NULL};
static char buffer1[BUFSIZE] = {NULL};
static char buffer2[BUFSIZE] = {NULL};
static char buffer3[BUFSIZE] = {NULL};
static uint8 bufid = 0;
static int8 pass = 0;
#endif /* FEATURE_DATA_UCSD_DEBUG_SIODUMP */
     

/* <EJECT> */
/*===========================================================================

FUNCTION ds_gcsd_sio_wm_handler

DESCRIPTION
   This function is the function called by both the high and low SIO 
   watermark handlers to report the state of the watermarks to the 
   protocol stacks.  It will set the state of the SIO_high_WM_state
   variable and call the ds_gcsd_setup_online_modem_control() function
   to perform any necessary flow control. 
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void
ds_gcsd_sio_wm_handler 
(
   boolean highWm
)
{
   /* Set the high WM variable according to the passed in parameter, 
   ** and call the modem control function */
   
   SIO_high_WM_state = highWm;
   ds_gcsd_setup_online_modem_control();
}


/* <EJECT> */
/*===========================================================================

FUNCTION ds_gcsd_sio_high_wm_handler and 
         ds_gcsd_sio_low_wm_handler

DESCRIPTION
  These are the watermark functions registered with the actual watermarks.
  They are added to the FAX and (depending on the call
  type) the SIO watermarks to perform serial I/O flow control 
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
void 
ds_gcsd_sio_high_wm_handler 
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
)
#else
(
   void
)
#endif
{
   ds_gcsd_sio_wm_handler(TRUE);
} /* ds_gcsd_sio_high_wm_handler() */

/*ARGSUSED*/
void 
ds_gcsd_sio_low_wm_handler
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
)
#else
(
   void
)
#endif
{
   ds_gcsd_sio_wm_handler(FALSE);
} /* ds_gcsd_sio_low_wm_handler() */


/* <EJECT> */
/*===========================================================================

FUNCTIONS ds_gcsd_ota_rx_buffer_full

DESCRIPTION
   Routine that indicates whether or not the OTA RX buffer is full.  This
   simply checks the state of the WM state for OTA rx data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_gcsd_ota_rx_buffer_full(void)
{
  boolean result = TRUE;
  dsm_watermark_type  *Gcsd_dl_wm_ptr = NULL;
  Gcsd_dl_wm_ptr      = ds_gcsd_get_Gcsd_dl_wm_ptr();
  
  if(Gcsd_dl_wm_ptr)
  {
    result = ( (OTA_rx_buffer_cnt < GCSD_TPS_SIO_HI_WM_LEVEL) && 
               (Gcsd_dl_wm_ptr->current_cnt < Gcsd_dl_wm_ptr->hi_watermark) ) ?
             FALSE : TRUE;
  }
  return result;
}
   

   
/*
 *   Function name:  ds_gcsd_read_from_serial_port
 *   --------------
 *
 *   Description: Reads data from fifo in shared memory.
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
ds_gcsd_read_from_serial_port
(
   uint8                   *data
)

{

   boolean              rc = TRUE;
   dsm_watermark_type   *Gcsd_ul_wm_ptr = NULL;
   Gcsd_ul_wm_ptr = ds_gcsd_get_Gcsd_ul_wm_ptr();

   /* Initialize return */
   *data = 0;
   
   /* If we have no current DSM item, try to get one off the watermark */
   if ((Curr_rx_dsm_length == 0) || (Curr_rx_dsm_item == NULL))
   {
      //ds_gcsd_update_serial_status (&stored_status);
      //reads_since_status_update = 0;
      
      /* Free last packet if one exists*/
      if (Curr_rx_dsm_item != NULL)
      {
         dsm_free_packet (&Curr_rx_dsm_item);
      }
      
      /* Check for upper layer call control is connected before 
         dequeueing more bytes. */
      if( GCSD_CALL_STATE_CONNECT == ds_gcsd_get_Gcsd_curr_call_state() )
      {
        /* Get next packet off the watermark */
        Curr_rx_dsm_item = DSM_DEQUEUE( Gcsd_ul_wm_ptr );
      }
      
      /* If we were able to get a new packet, get it's length */
      /* It should never happen that we get 0 length DSM items in
      ** real life... but I made it happen in a test scenario and 
      ** it was broken... so I fixed it here.
      */
      while (Curr_rx_dsm_item != NULL)
      {
         Curr_rx_dsm_length = (word)dsm_length_packet( Curr_rx_dsm_item );
         
         /* If we have a 0 length DSM item, free it and get another...
         ** otherwise, break out of this loop 
         **
         */
         if (Curr_rx_dsm_length == 0)
         {
            dsm_free_packet (&Curr_rx_dsm_item);
            /* Get next packet off the watermark */
            Curr_rx_dsm_item = DSM_DEQUEUE( Gcsd_ul_wm_ptr );
         }
         else
         {
            break;
         }
      }
       
   }
       
   
   /* Get a character from the DSM item */
   if (Curr_rx_dsm_item != NULL)
   {
      (void)dsm_pullup (&Curr_rx_dsm_item, data, 1);
      Curr_rx_dsm_length--;
   }
   else
   {
      Curr_rx_dsm_length = 0;
      rc = FALSE;
   }
   
   /* Update the status if it is necessary */
   //if (reads_since_status_update++ >= NUM_READS_FOR_STATUS_UPDATE)
   //{
   //   ds_gcsd_update_serial_status (&stored_status);
   //   reads_since_status_update = 0;
   //}

#ifdef FEATURE_DATA_UCSD_DEBUG_SIODUMP
   if( pass == PASS_MOD )
   {
     buffer[PASS_MOD]='\0';

     DATA_MSG1_HIGH(" buffer %d", bufid);

     if(0 == bufid)      { (void)snprintf(buffer0, BUFSIZE,"FROM_SIO-0: %s", buffer); }
     else if(1 == bufid) { (void)snprintf(buffer1, BUFSIZE,"FROM_SIO-1: %s", buffer); }
     else if(2 == bufid) { (void)snprintf(buffer2, BUFSIZE,"FROM_SIO-2: %s", buffer); }
     else if(3 == bufid) { (void)snprintf(buffer3, BUFSIZE,"FROM_SIO-3: %s", buffer); }
	 
     if( bufid++ >= (MAX_BUFFS-1) ) { bufid=0; }

     memset((void*)buffer,0x0,sizeof(buffer));
     pass = 0;
   }
   buffer[pass++] = (char)*data;
#endif /* FEATURE_DATA_UCSD_DEBUG_SIODUMP */
   
   //*status = stored_status;
   return rc;
}  



/*
 *   Function name:  ds_gcsd_write_to_serial_port
 *   --------------
 *
 *   Description: Writes data into fifo in shared memory
 *   ------------ for the serial port isr
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
ds_gcsd_write_to_serial_port
(
   uint8                   output_data,
   boolean                 output_data_valid, 
   boolean                 flush_data
)
{
   dsm_item_type *tmp_dsm_ptr;
   boolean             Gcsd_curr_call_is_fax;
   dsm_watermark_type  *Gcsd_dl_wm_ptr = NULL;

   Gcsd_dl_wm_ptr = ds_gcsd_get_Gcsd_dl_wm_ptr();
   Gcsd_curr_call_is_fax = ds_gcsd_get_Gcsd_curr_call_is_fax();

   /* Only add data to the DSM item if the incoming data is valid and
   ** if the flush_data flag is set to false.  If flush_data is true,
   ** there isn't valid data in output_data
   */
   
   if (output_data_valid)
   {
      /* If we have no tx dsm item, allocate one */
      if (Curr_tx_dsm_item == NULL)
      {
         Curr_tx_dsm_item = dsm_new_buffer (DSM_DS_SMALL_ITEM_POOL);
      }
      ASSERT (Curr_tx_dsm_item != NULL);
      
      /* Add the data to the DSM item */
      (void)dsm_pushdown_tail (&Curr_tx_dsm_item, 
                               &output_data,
                               1,
                               DSM_DS_SMALL_ITEM_POOL);

      OTA_rx_buffer_cnt++;
   }
   
   if ((flush_data == TRUE) && (Curr_tx_dsm_item != NULL))
   {
#ifdef FEATURE_DATA_GCSD_FAX
      /* If in a FAX call, simply enqueue the FAX data on the downlink
      ** watermark
      */
      if (Gcsd_curr_call_is_fax)
      {
        DSM_ENQUEUE( Gcsd_dl_wm_ptr, &Curr_tx_dsm_item );
        /* DSM item is enqueued in watermark make it NULL */
        Curr_tx_dsm_item = NULL;
      }
      else
#endif
      if (!Suspend_data_to_sio) 
      {
         // All buffered receive data is now being flushed...
         OTA_rx_buffer_cnt = 0;

         /* Unchain DSM items and queue up on SIO */
         while (Curr_tx_dsm_item != NULL)
         {
            // Set up temporary DSM item pointer            
            tmp_dsm_ptr = Curr_tx_dsm_item;
            
            // Set Curr_tx_dsm_item to be next one in chain
            Curr_tx_dsm_item = tmp_dsm_ptr->pkt_ptr;
            
            // Set next in chain of temporary point to NULL
            tmp_dsm_ptr->pkt_ptr = NULL;
            
            // Queue up for transmit via temporary pointer
           if (ds3g_siolib_ex_setup_tx (tmp_dsm_ptr, 
                                         FALSE,
                                         ds3g_siolib_ex_get_csd_port()) 
                                       != DS3G_SIOLIB_OK)

            {
               DATA_MSG0_ERROR("GCSD: Unable to transmit to SIO");
            }
         }
      }         
   }

   return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION ds_gcsd_suspend_data_to_sio

DESCRIPTION
   This function turns suspension of data to SIO on/off. 
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  When data suspension to SIO is turned on, data will be queued up, but
  not transmitted to SIO.

===========================================================================*/
void
ds_gcsd_suspend_data_to_sio 
(
   boolean suspend
)
{
   Suspend_data_to_sio = suspend;

   /* If resuming data flow, flush out any buffered data */
   if (!suspend)
   {
      (void) ds_gcsd_write_to_serial_port (0,FALSE,TRUE);
   }
      
}

/*===========================================================================

FUNCTION DS_GCSD_SET_CURR_RX_DSM_LENGTH

DESCRIPTION
   This function sets the Curr_rx_dsm_length.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/

void 
ds_gcsd_set_Curr_rx_dsm_length
(
  word curr_rx_dsm_lnth
)
{
  Curr_rx_dsm_length = curr_rx_dsm_lnth;
}

/*===========================================================================

FUNCTION DS_GCSD_SET_SIO_HIGH_WM_STATE

DESCRIPTION
   This function returns the SIO_high_WM_state.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void 
ds_gcsd_set_SIO_high_WM_state
(
  boolean sio_high_WM_state
)
{
  SIO_high_WM_state = sio_high_WM_state;
}

/*===========================================================================

FUNCTION DS_GCSD_SET_OTA_RX_BUFFER_CNT

DESCRIPTION
   This function returns the OTA_rx_buffer_cnt.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void 
ds_gcsd_set_OTA_rx_buffer_cnt
(
  uint32 ota_rx_buffer_cnt
)
{
  OTA_rx_buffer_cnt = ota_rx_buffer_cnt;
}

/*===========================================================================

FUNCTION DS_GCSD_SET_SUSPEND_DATA_TO_SIO

DESCRIPTION
   This function returns the Suspend_data_to_sio.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void 
ds_gcsd_set_Suspend_data_to_sio
(
  boolean suspend_data_to_sio
)
{
  Suspend_data_to_sio = suspend_data_to_sio;
}

/*===========================================================================

FUNCTION DS_GCSD_CLEAN_CURR_DSM_ITEM

DESCRIPTION
   This function cleans the Curr_rx_dsm_item and Curr_tx_dsm_item.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/

void ds_gcsd_clean_Curr_dsm_item(void)
{
  if (Curr_rx_dsm_item != NULL)
  {
    dsm_free_packet (&Curr_rx_dsm_item);
    Curr_rx_dsm_item = NULL;
  }

  if (Curr_tx_dsm_item != NULL)
  {
    dsm_free_packet (&Curr_tx_dsm_item);
    Curr_tx_dsm_item = NULL;
  }
}

/*===========================================================================

FUNCTION ds_gcsd_get_Curr_rx_dsm_length

DESCRIPTION
   This function returns the Curr_rx_dsm_length.
         
DEPENDENCIES
  None
  
RETURN VALUE
  word - value of Curr_rx_dsm_length
    
SIDE EFFECTS
  None

===========================================================================*/

word 
ds_gcsd_get_Curr_rx_dsm_length(void)
{
  return Curr_rx_dsm_length;
}


/*===========================================================================

FUNCTION ds_gcsd_get_SIO_high_WM_state

DESCRIPTION
   This function returns the SIO_high_WM_state.
         
DEPENDENCIES
  None
  
RETURN VALUE
  booelan - value of SIO_high_WM_state
    
SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_gcsd_get_SIO_high_WM_state(void)
{
  return SIO_high_WM_state;
}

/*===========================================================================

FUNCTION ds_gcsd_get_Suspend_data_to_sio

DESCRIPTION
   This function returns the Suspend_data_to_sio.
         
DEPENDENCIES
  None
  
RETURN VALUE
  booelan - value of Suspend_data_to_sio
    
SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_gcsd_get_Suspend_data_to_sio(void)
{
  return Suspend_data_to_sio;
}

#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */
