/*===========================================================================

              GSM NON-TRANSPARENT_DATA L2R DATA SERVICES

DESCRIPTION
  Non-transparent data link control, 
  L2R data translation functions and interface to RLP.  This code has
  been ported from code written and purchased by Qualcomm from CCWW.

Copyright (c) 2002 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdnt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/29/11   SA      Added support for TD-SCDMA.
02/02/11   SS      Q6 free floating changes.
09/03/10   ttv     Merged the fixes of lint critical errors.
09/29/09   SA      Fixed KW errors.
08/20/07   AR      Ensure DSM RLPv2 status byte flag cleared before encodePDU.
08/07/07   DA      Added user data inactivity timer functionality
06/27/07   AR      Add multiple client support for unified API.
01/16/07   AR      Add UL frame flush timer on MO termination to limit delay.
08/22/06   AR      Changes to support multiprocessor architecture.
08/14/06   AR      Clear RLP interface buffers after handover.
05/05/06   AR      Incorporate changes from BoaApps interRAT effort.
02/09/06   AR      Lint corrections.
11/21/05   AR      Add support for multiple call instances; Lint corrections.
04/25/05   AR      Add use of NV item to set state when connect message is
                   sent to mode-specific handler.
01/24/05   AR      Check V42 Rx buffer before clearing DL processing flag.
10/25/04   AR      Use ADM idle timer when disconnect received from remote.
08/17/04   AR      Initialize Ds_FillPattern for user data field.
07/29/04   AR      Relocate logging intialization to support handover.
05/17/04   AR      Signal connect complete after RLP connection established.
04/20/04   AR      Process all available queued Rx frames during DL cycle
04/15/04   AR      Fix offset for L2R status bytes in DecodeStatus()
04/05/04   AR      Set default RLP version based on NV lookup.
03/17/04   AR      Added ds_ucsd_remote_fc_enabled() routine.  Lint cleanup.
                   Make queued PDU Tx dependent on remote flow control.
03/04/04   AR      Updates to support W2G handover; Lint fixes
02/05/04   AR      Use PDUSTATUS_STATUS_BITS instead of bad constant
01/12/04   AR      Make RLP XID parameter init dependent on WCDMA feature.
11/18/03   TMR     Made change to perform downlink RLP processing regardless
                   of L2R state
11/18/03   AR      Added query for current list PDU type for remap detection.
                   Process all Rx frames in rlp_main().
07/15/03   TMR     Added support for RLP Version 2
04/30/03   AR      Add suppport for DSM items inplace of local buffers.
                   Consolidate EncodePDU and DecodePDU routines.
04/17/03   AR      Added routine ds_ucsd_nt_pwrup_init(), Lint corrections.
08/10/01   TMR     Initial release
02/26/02   TMR     Added reporting completion of XID parameter negotiation to
                   DS task
07/10/02   TMR     Changed from looking for PDU size number of bytes in 
                   EncodeLargePDU

===========================================================================*/

/* <EJECT> */
/*===========================================================================
Header files
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_3gppi_utils.h"
#include <stringl/stringl.h>

#if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS)

#include "task.h"
#include "dsgcsdi.h"
#include "dsucsdnti.h"
#include "dsucsdrlp.h"
#include "dsucsdv42.h"
#include "dsucsdlog.h"
#include "amssassert.h"

/* <EJECT> */
/*===========================================================================
Data definitions
===========================================================================*/

#define DS_UCSD_NT_CALL_INIT_DELAY_TIME  100 /* 100 msec delay at call start */
#define DS_UCSD_NT_MIN_RLPADM_IDLE_TIME  100 /* 100 msec delay minimum  */
#define DS_UCSD_NT_MAX_RLPL2R_FLUSH_TIME 15000 /* 15 sec delay for UL flush */

/****************************************************************************
 * Global variables - externally visible 
 ****************************************************************************/
 
static ds_ucsd_nt_cb_type ds_ucsd_nt_cb_info;

/* Empty information field value */
static uint8 Ds_FillPattern[MAX_USER_BYTES];


/* Event Queue instances */
static DSISR_EventQueue_T         Evq_To_L2R;
static DSISR_EventQueue_T         Evq_To_RLP;

LOCAL XID_Params_T         Rlp_RequestedXID;

/****************************************************************************
 * Module scope variables 
 ****************************************************************************/
LOCAL v42_comp_direction_T    Compression = V42_NO_COMPRESSION;

/* Break handling flags */
LOCAL boolean                 BreakRxdFromNetwork = FALSE;
LOCAL boolean                 BreakRxdFromDTE = FALSE;
LOCAL boolean                 DiscardRxData = FALSE;

LOCAL uint8                   OldStatusByte = 0;

/**********************************************************
** The various registered callback function pointers
***********************************************************/
LOCAL 
ds_ucsd_nt_ind_report_cb_T         Ds_ucsd_nt_ind_report_cb        = NULL;
                                                                                                                                     
LOCAL 
ds_ucsd_nt_rx_from_sio_cb_T        Ds_ucsd_nt_rx_from_sio_cb       = NULL;
                                                                  
LOCAL 
ds_ucsd_nt_tx_to_sio_cb_T          Ds_ucsd_nt_tx_to_sio_cb         = NULL;

LOCAL 
ds_ucsd_nt_get_sio_status_cb_T     Ds_ucsd_nt_get_sio_status_cb    = NULL;

LOCAL 
ds_ucsd_nt_set_sio_cts_state_cb_T  Ds_ucsd_nt_set_sio_cts_state_cb = NULL;

LOCAL 
ds_ucsd_nt_suspend_sio_tx_cb_T     Ds_ucsd_nt_suspend_sio_tx_cb    = NULL;

LOCAL
ds_ucsd_nt_init_sio_cb_T           Ds_ucsd_nt_init_sio_cb          = NULL;

LOCAL
ds_ucsd_nt_init_ota_cb_T           Ds_ucsd_nt_init_ota_cb          = NULL;

LOCAL
ds_ucsd_nt_no_tx_frames_sent_cb_T  Ds_ucsd_nt_no_tx_frames_sent_cb = NULL;


/**********************************************************
** Date that is initialized via the 
** ds_ucsd_nt_init_call_data() function call
***********************************************************/
LOCAL sys_sys_mode_e_type              Ds_ucsd_nt_curr_mode = SYS_SYS_MODE_NONE; 
LOCAL ds_ucsd_frame_size_T             Ds_ucsd_nt_bytes_per_sdu = DS_UMTS_CSD_FRAME_SIZE_INVALID;
LOCAL uint32                           Ds_ucsd_nt_sdus_per_tti = 0;
LOCAL ds_ucsd_nt_rlp_parameters_T      Ds_ucsd_nt_rlp_params = {0};
LOCAL ds_ucsd_data_rate_T              Ds_ucsd_nt_data_rate = DS_UCSD_RATE_NONE;
LOCAL boolean                          Ds_ucsd_nt_suppress_duplicate_tx_frames = FALSE;

LOCAL ds_ucsd_set_data_counters_cb_T Ds_gcsd_set_data_counters_cb = NULL;

/* Call mangaer call ID for mode handler events */
LOCAL cm_call_id_type            Ds_ucsd_nt_call_id = CM_CALL_ID_INVALID;


/* This flag is used by RLP to indicate whether or not TX frame
** data was sent
*/
LOCAL boolean                          Ds_ucsd_nt_tx_frame_sent = FALSE;

/* This flag indicates whether or not data was suppressed */
LOCAL boolean                          Ds_ucsd_frame_suppressed = FALSE;

LOCAL 
boolean                          Ds_ucsd_nt_bc_compression_enabled = FALSE;

LOCAL 
boolean                          Ds_ucsd_nt_init_call_data_called = FALSE;
                                                         

/* Network data counters */
LOCAL uint32                     Ds_ucsd_nt_tx_data_count = 0;
LOCAL uint32                     Ds_ucsd_nt_rx_data_count = 0;

/* This flag is used to determine whether or not to perform 
 * a transmit of an RLP frame. This is because:
 * 4800 link:  240-bit frame transmitted every 40ms (2 cycles)
 * 9600 link:  240-bit frame transmitted every 20ms (1 cycle)
 * 14400 link: 576-bit frame transmitted every 40ms (2 cycles)
 * The L2R and RLP layers are therefore called only every other 
 * uplink cycle for 4800 and 14400 bit links.  
 * On the downlink cycle, the L2R/RLP is called on every tick, 
 * so that a frame will be read whenever one becomes available.
 */
LOCAL uint8                    Ds_ucsd_nt_perform_rlp_uplink = 0x01;

LOCAL filtered_rx_status_T     Ds_ucsd_nt_filtered_rx_status = {0};
LOCAL filtered_rx_status_T     Ds_ucsd_nt_saved_filtered_rx_status = {0};

LOCAL ds_ucsd_sio_status_T Ds_ucsd_nt_curr_modem_ctl = {0};

/* These are pointers to the buffer write/read functions */
LOCAL FPTR_WRITE_TO_BUFFER     pDownlinkWrite = NULL;
LOCAL FPTR_CHECK_FULL_BUFFER   pDownlinkCheckFull = NULL;
LOCAL FPTR_READ_FROM_BUFFER    pUplinkRead = NULL;
LOCAL FPTR_CHECK_BUFFER        pUplinkCheck = NULL;


LOCAL boolean                  Ds_ucsd_nt_check_DCD = FALSE;

/* The following is used locally in ds_ucsd_V24_interface_handler_byte_oriented()
** to buffer up data read from SIO in some pathological cases to make RLP PDU
** construction more efficient
*/
LOCAL uint8                   Sio_rx_saved_byte_count = 0;  

/* The following is used to determine whether or not we do L2R and RLP
** processing
*/
LOCAL boolean                 Do_L2R_RLP = FALSE;


#ifdef DS_UCSD_DEBUG
LOCAL uint32 ds_ucsd_nt_sio_tx_bytes = 0, ds_ucsd_nt_sio_rx_bytes = 0;
              
LOCAL uint32        num_chars_rxd=0;
LOCAL uint32        num_chars_txd=0;
LOCAL int     tick=0;

#endif




/* States of the L2R FSM */
enum 
{
/*   L2RSTATE_INVALID              = 0, */
   L2RSTATE_IDLE                 = 1,
   L2RSTATE_NEGOTIATING_XID      = 2,
   L2RSTATE_CONNECTING           = 3,
   L2RSTATE_WAIT_FOR_DCD_ON      = 4,
   L2RSTATE_CONNECTED            = 5,
   L2RSTATE_RLP_DISC_TERMINATING = 6,
   L2RSTATE_MO_TERMINATING       = 7,
   L2RSTATE_AWAITING_BREAK_ACK   = 8,
   L2RSTATE_REMAPPING            = 9
};

/* Non-Transparent state machine state */
LOCAL uint32 NTP_State = L2RSTATE_IDLE;
LOCAL uint32  Old_NTP_State = L2RSTATE_IDLE;

/* Preserve user data payload size before handover.  Needed to account
 * for XID renegotiation */
LOCAL int32  OldPDUsize = 0; 


/* Flag to indicate DSM item pool is low; used for flow control */
LOCAL boolean DSM_Level_Low = FALSE;



/* <EJECT> */
/*===========================================================================
Function definitions
===========================================================================*/

/****************************************************************************
 * Function name:  ds_ucsd_event_queue_init
 * -------------
 * Description:    Initialises an event queue
 * -----------     
 * Parameters:     Event Queue ptr
 * ----------
 * Returns:        None
 * -------
 ****************************************************************************/
void
ds_ucsd_event_queue_init 
(
   DSISR_EventQueue_T* Queue
)
{
   Queue->Read = 0;
   Queue->Write= 0;
}

/****************************************************************************
 * Function name:  ds_ucsd_read_event
 * -------------
 * Description:    Reads an event from an event queue
 * -----------     
 * Parameters:     Event Queue ptr
 * ----------
 * Returns:        Event
 * -------
 ****************************************************************************/
DSI_Event_T 
ds_ucsd_read_event 
(
   DSISR_EventQueue_T* Queue
)
{
   DSI_Event_T Event = (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT;
   short* pRead = &(Queue->Read);
   
   if (*pRead != Queue->Write)
   {
      Event = Queue->Event[*pRead];
      (*pRead)++;
      *pRead &= (QUEUE_SIZE-1);
   }
   
   return Event;
}

/****************************************************************************
 * Function name:  ds_ucsd_write_event
 * -------------
 * Description:    Writes an event to an event queue
 * -----------     
 * Parameters:     Event Queue ptr, Event
 * ----------
 * Returns:        None
 * -------
 ****************************************************************************/
void
ds_ucsd_write_event 
(
   DSISR_EventQueue_T   *Queue, 
   DSI_Event_T          Event
)
{
   short* pWrite = &(Queue->Write);
   
   Queue->Event[*pWrite] = Event;
   (*pWrite)++;
   (*pWrite) &= (QUEUE_SIZE-1);
}
 





/* <EJECT> */
/*===========================================================================

FUNCTION    ds_ucsd_v42_compression_direction

DESCRIPTION
    Initialise the control variable for compression direction.
         
DEPENDENCIES
  None
  
RETURN VALUE
  byte representing compression direction
    
SIDE EFFECTS
  None

===========================================================================*/
v42_comp_direction_T 
ds_ucsd_v42_compression_direction 
(
   byte     P0, 
   boolean  Initiator
)
{
   v42_comp_direction_T  RetVal = V42_NO_COMPRESSION;

   switch (P0)
   {
      case 3:
         RetVal = V42_BOTH_DIRECTIONS;
         break;

      case 2: /* Responder->Initiator direction only */
         if (Initiator)
            RetVal = V42_DECODE_ONLY;
         else
            RetVal = V42_ENCODE_ONLY;
         break;

      case 1: /* Initiator->Responder direction only */
         if (Initiator)
            RetVal = V42_ENCODE_ONLY;
         else
            RetVal = V42_DECODE_ONLY;
         break;

      default:
         RetVal = V42_NO_COMPRESSION;
         break;
   }
   return RetVal;
}



/* <EJECT> */
/*===========================================================================

FUNCTION
   ds_ucsd_remote_fc_enabled()

DESCRIPTION
  This routine reports the flow control state of the remote entity
  as decoded from the L2R status bits.
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE it CTS signal deasserted, FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
boolean
ds_ucsd_remote_fc_enabled ( void )
{
  return ( V24_CONTROL_OFF == Ds_ucsd_nt_filtered_rx_status.X_status_rx );
}


/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_InitialiseBufferpointers

DESCRIPTION
    Point to the buffers to be used for routing the data.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_InitialiseBufferpointers 
(
   v42_comp_direction_T compression
)
{
   switch (compression)
   {
      case V42_BOTH_DIRECTIONS:
         pUplinkRead        = ds_read_V42_tx_buffer;
         pUplinkCheck       = ds_check_V42_tx_buffer;
         pDownlinkWrite     = ds_write_V42_rx_buffer;
         pDownlinkCheckFull = ds_check_full_V42_rx_buffer;
         DS_UCSD_DEBUG_MSG ("NT: Compression is V42_BOTH_DIRECTIONS");
         break;

      case V42_ENCODE_ONLY:     /* Uplink Compression only */
         pUplinkRead        = ds_read_V42_tx_buffer;
         pUplinkCheck       = ds_check_V42_tx_buffer;
         pDownlinkWrite     = ds_ucsd_V24_interface_driver_byte_oriented;
         pDownlinkCheckFull = ds_ucsd_nt_cb_info.is_sio_tx_q_full_cb;
         DS_UCSD_DEBUG_MSG ("NT: Compression is V42_ENCODE_ONLY");
         break;

      case V42_DECODE_ONLY:     /* Downlink Compression only */
         pUplinkRead        = ds_ucsd_V24_interface_handler_byte_oriented;
         pUplinkCheck       = NULL;
         pDownlinkWrite     = ds_write_V42_rx_buffer;
         pDownlinkCheckFull = ds_check_full_V42_rx_buffer;
         DS_UCSD_DEBUG_MSG ("NT: Compression is V42_DECODE_ONLY");
         break;

      case V42_NO_COMPRESSION:
      default:
         pUplinkRead        = ds_ucsd_V24_interface_handler_byte_oriented;
         pUplinkCheck       = NULL;
         pDownlinkWrite     = ds_ucsd_V24_interface_driver_byte_oriented;
         pDownlinkCheckFull = ds_ucsd_nt_cb_info.is_sio_tx_q_full_cb;
         DS_UCSD_DEBUG_MSG ("NT: Compression is V42_NO_COMPRESSION");
         break;
   }

}


/*****************************************************************************
 * Function name :  ds_V24_interface_handler_byte_oriented
 * -------------
 * Description :    This function deals with the tx data from the serial
 * -----------      ghdi i/face (and the modem control lines).
 *                  It reads bytes from the ghdi and writes them into the 
 *                  Tx buffer. No addition of parity, start and stop bits
 *                  is performed in this case (as it is in the above function).
 * Parameters:      Pointer to Tx PDU buffer
 * ----------
 * Returns:         Number of bytes read
 * --------
 ****************************************************************************/
uint32
ds_ucsd_V24_interface_handler_byte_oriented
(
   dsm_item_type  **pdu_ptr,
   uint32         num_bytes_needed  
)
{
   static   uint8    Sio_rx_saved_bytes [2] = { 0,0 };
            uint8    Tx_data;
   register uint32   num_read = 0;
   register boolean  ds_ucsd_status = FALSE;
   register boolean  dsm_item_allocated;
   register uint32   temp_num_bytes = num_bytes_needed;

   // TMR_TODO
   /* Calculate number of bytes we need to keep the tx buffers full */
   /* If no compression, just need to fill current PDU size         */
   /* With compression, use MAX_BUFFERED_USER_BYTES                 */

   ASSERT(NULL != pdu_ptr);
   if (*pdu_ptr)
   {
      num_read += (uint32)dsm_length_packet(*pdu_ptr);
      dsm_item_allocated = TRUE;
   }
   else
   {
      dsm_item_allocated = FALSE;
   }
   
   /* Put the buffered data, if any into the DSM item */
   if (Sio_rx_saved_byte_count)
   {
      if ((temp_num_bytes - num_read) < Sio_rx_saved_byte_count)
      {
         MSG_ERROR ("No room for prior saved data %d %d %d",
                    temp_num_bytes,num_read,Sio_rx_saved_byte_count);
         return num_read;
      }
      /* Allocate DSM packet for Tx buffer if none provided */
      if (!dsm_item_allocated)
      {
         dsm_item_allocated = TRUE;
         *pdu_ptr = ds_ucsd_UplinkPDUMgr_New();
         if(NULL == *pdu_ptr)
         {
           return num_read;
         }
      }
      DSM_PUSHDOWN_TAIL (pdu_ptr, Sio_rx_saved_bytes, Sio_rx_saved_byte_count); /*lint !e666*/
      num_read += Sio_rx_saved_byte_count;
      Sio_rx_saved_byte_count = 0;
   }
      

         
   for (; num_read < temp_num_bytes; num_read++)
   {
      /* Read user data from DTE serial port */
      /* Might want to look at transferring complete DSM item here */
      ds_ucsd_status = Ds_ucsd_nt_rx_from_sio_cb(&Tx_data);

      /* Copy serial user data (if there is any!) to buffer */
      if (ds_ucsd_status)
      {
         /* Allocate DSM packet for Tx buffer if none provided */
         if (!dsm_item_allocated)
         {
            dsm_item_allocated = TRUE;
            *pdu_ptr = ds_ucsd_UplinkPDUMgr_New();
         }
         
         /* Push byte onto end of DSM packet */
         ASSERT( ( NULL != pdu_ptr) && ( NULL != *pdu_ptr) );
         DSM_PUSHDOWN_TAIL (pdu_ptr, &Tx_data, 1); /*lint !e666*/

 #ifdef DS_UCSD_DEBUG
         ds_ucsd_nt_sio_rx_bytes++;
 #endif  
      }
      else
      {
         /* No more serial characters in the buffer, so leave */
         break;
      }
   }
   
   /* Large PDU construction can require inserting of bytes into 
   ** the data stream if there aren't at least three spare bytes
   ** in the PDU.  We will screen out this case here in order 
   ** to prevent this pathological case
   */
   /* Set tmp_num_bytes to the number of bytes that 
   ** we came up short from the request 
   */
   temp_num_bytes = temp_num_bytes - num_read;
   
   /* If we come up short (tmp_num_bytes > 0), and we 
   ** are getting data for a large PDU, then we need to come
   ** up at least 3 bytes short.  If we came up less than
   ** three bytes short, save bytes locally to make it so
   ** we come up 3 short 
   */
   if ( temp_num_bytes && (temp_num_bytes < 3) &&
        (ds_ucsd_get_CurrentPDUType() == PDU_TYPE_LARGE )
      )   
   {
      /* Calculate number of bytes we need to store and then
      ** put them in the Sio_rx_saved_bytes buffer 
      */
      temp_num_bytes = 3 - temp_num_bytes;
      DSM_PULLUP_TAIL (pdu_ptr,Sio_rx_saved_bytes,(word)temp_num_bytes); /*lint !e666 */
      Sio_rx_saved_byte_count = (byte)temp_num_bytes;
      num_read -= temp_num_bytes;
   } 
      
   return num_read;
}




/****************************************************************************
 * Function name:  ds_V24_interface_driver_byte_oriented
 * -------------
 * Description:    Read bytes from the rx-buffer and write to the ghdi
 * -----------     along with the modem status lines.
 * Parameters:     Pointer to Rx PDU buffer
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
void
ds_ucsd_V24_interface_driver_byte_oriented
(
   dsm_item_type ** pdu_ptr
)
{

   byte  Rx_data = 0x00;

   ASSERT (NULL != *pdu_ptr);
   
   /* Send data characters and control information to V24 interface */
   
   /* Set up modem control block -- this is here to assert/deassert
   ** flow control if requested to by status flags received from the 
   ** network L2R entity
   */
   /*
   ds_ucsd_setup_online_modem_control();
   */
      
   /* Transfer data from buffer to V24 interface */
   while ( 1 == dsm_pullup (pdu_ptr, &Rx_data, 1) )
   {
      /*send one character and control bits to serial port driver
       *until buffer is empty*/
#ifdef DS_UCSD_DEBUG
      ds_ucsd_nt_sio_tx_bytes++;
#endif
      (void)Ds_ucsd_nt_tx_to_sio_cb (Rx_data, TRUE, FALSE);
   }

   (void)Ds_ucsd_nt_tx_to_sio_cb (Rx_data, FALSE, TRUE);

   /* Free DSM item (if necessary) */
   if (NULL != *pdu_ptr)
   {
      ds_ucsd_DownlinkPDUMgr_Free (pdu_ptr);
   }
}


/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_HandleBreak

DESCRIPTION
    Destroys all buffered data, initialises V42bis
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_HandleBreak (void)
{
  XID_Params_T Rlp_CurrentXID;
  DS_UCSD_DEBUG_MSG ("ds_ucsd_HandleBreak: Destroying all buffered data!!");

  /* Destroy all buffered data (see 07.02 para 3.5) */
  ds_init_V42_rx_buffer ();
  ds_init_V42_tx_buffer ();
  ds_init_remap_pending_buffer ();
  ds_ucsd_DownlinkPDUMgr_Empty ();
  ds_ucsd_UplinkPDUMgr_Empty ();
  
  memscpy(&Rlp_CurrentXID, sizeof(XID_Params_T), 
          ds_ucsd_get_current_xid(), sizeof(XID_Params_T));
  /* Initialise the v42bis data compression */
  V42_initialise (Rlp_CurrentXID.P2,Rlp_CurrentXID.P1,Compression);
}


/* <EJECT> */
/*===========================================================================

FUNCTION
   ds_ucsd_EncodePDU 

DESCRIPTION
   Encode a PDU from the specified input buffer.  The entire 
   buffer is read by passing the DSM item.  The input buffer is checked
   for it's fullness.  A full PDU is built if possible, otherwise a
   partially full one is built.
         
DEPENDENCIES
  None
  
RETURN VALUE
  Send PDU: TRUE/FALSE
    
SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
LOCAL boolean 
ds_ucsd_EncodePDU 
(
   dsm_item_type **        pdu_ptr, 
   PDU_Type                PDU_type,
   FPTR_READ_FROM_BUFFER   pReadFromBuffer,
   FPTR_CHECK_BUFFER       pCheckBuffer
)
{                       
   boolean RetVal;
   
   register uint16  PDU_user_data_size;
   register boolean status_byte_changed;
   register boolean status_byte_is_present;
   register boolean break_or_break_ack_in_status = FALSE;
   register uint8   num_chars_read               = 0;
            uint8   status_byte                  = 0;

   
   /* Determine byte size of PDU */
   PDU_user_data_size = get_current_rlp_pdu_payload_size();
   
   
   /* Read the status lines to put in the PDU */
   if (Ds_ucsd_nt_curr_modem_ctl.DTR == V24_CONTROL_OFF)
      status_byte |= PDUSTATUS_DTR_MASK;
   
   if (Ds_ucsd_nt_curr_modem_ctl.RTS == V24_CONTROL_OFF)
      status_byte |= PDUSTATUS_RTS_MASK;
   
   /* If the rx buffer is becoming full, lower CTS to the remote node */
   if ((*pDownlinkCheckFull)())
   {
     status_byte |= PDUSTATUS_CTS_MASK;
   }


   /* Determine if status byte has changed 
   */
   if (status_byte != OldStatusByte)
   {
      DS_UCSD_DEBUG_MSG_3 ("L2R state change: DTR=%d, RTS=%d, CTS=%d",
                           (status_byte & PDUSTATUS_DTR_MASK),
                           (status_byte & PDUSTATUS_RTS_MASK),
                           (status_byte & PDUSTATUS_CTS_MASK));
                           
      status_byte_changed = TRUE;
      
      /* Save old status byte if it has changed */
      OldStatusByte = status_byte;
   }
   else
   {
      status_byte_changed = FALSE;
   }
   
   
   /* Check to see if we have received a break from either 
   ** the network or from the local DTE.  If so, set up 
   ** the status byte and indicate that it has changed.
   ** Also set break_or_break_ack_in_status flag so that
   ** we don't attempt to send any data. 
   */
   if (BreakRxdFromNetwork)
   {
      break_or_break_ack_in_status = TRUE;
      status_byte_changed = TRUE;
      status_byte |= PDUSTATUS_BREAK_ACK;
      ds_ucsd_HandleBreak ();
      BreakRxdFromNetwork = FALSE;
   }
   else if (BreakRxdFromDTE)
   {
      break_or_break_ack_in_status = TRUE;
      status_byte_changed = TRUE;
      status_byte |= PDUSTATUS_BREAK;
      DS_UCSD_DEBUG_MSG ("Sending BREAK in PDU");
      BreakRxdFromDTE = FALSE;
   }
   
   
      
   /* If we have to send a status byte (i.e. it has changed), or we 
   ** have to because we are using RLP version 0 or 1, decrement
   ** the amount of user data we can handle to make room for
   ** status byte.  To avoid doing this check in other places
   ** in the code, use the status_byte_is_present flag.
   */
   if ( status_byte_changed || (!current_rlp_version_is_2()) )
   {
      status_byte_is_present = TRUE;
      PDU_user_data_size--;
   }
   else
   {
      status_byte_is_present = FALSE;
   }
   
   
   /* Read data from SIO or Remap buffer only if we don't have a
   ** break or break_ack in the status byte and CTS is asserted
   */
   if ( (!break_or_break_ack_in_status) && 
        (!ds_ucsd_remote_fc_enabled()) 
      )
   {
      num_chars_read = (uint8)(*pReadFromBuffer)(pdu_ptr,PDU_user_data_size);
      
#ifdef DS_UCSD_DEBUG
      num_chars_txd += (int)num_chars_read;
#endif

   }
   
   /* If status changed OR we have user data to send then build a PDU
   */ 
   if (status_byte_changed || num_chars_read)
   {
      RetVal = TRUE;
      
       /* Allocate PDU if necessary */
      ASSERT(NULL != pdu_ptr);
      if (*pdu_ptr == NULL)
      {
         *pdu_ptr = ds_ucsd_UplinkPDUMgr_New();

      }
      /*----------------------------------------------------------- 
        KW Fix: Check for Null pointer dereferencing after call to
        ds_ucsd_UplinkPDUMgr_New()
      -----------------------------------------------------------*/
      if (*pdu_ptr != NULL)
      {
         /* Ensure status byte indicator cleared */
         (*pdu_ptr)->app_field &= ~RLP_DSM_APP_FIELD_NO_STATUS_BYTE;
      }
      else
      {
         DS_3GPP_MSG3_ERROR ("*pdu_ptr returned NULL ",0,0,0);
         RetVal = FALSE;
         return RetVal;
      }
	/* If break or break_ack, just send the status byte     
	*/ 	
      if (break_or_break_ack_in_status)
      {
         /* Write a status byte to the start of the PDU */
         DSM_PUSHDOWN (pdu_ptr, &status_byte, 1); /*lint !e666*/
      }
      
      /* Check if no data was read -- empty PDU, but if we got 
      ** here, the status byte changed and needs to be sent
      */
      else if (num_chars_read == 0)
      {
         /* Write a status byte to the start of the PDU */
         status_byte |= PDUSTATUS_REMAINDER_EMPTY;
         DSM_PUSHDOWN (pdu_ptr, &status_byte, 1); /*lint !e666*/
      }
      
      /* Check to see if all data requested was read */
      else if (num_chars_read == PDU_user_data_size)
      {
         /* If status byte is present either due to status change
         ** or RLP versions 0 or 1, then set it to be 
         ** REMAINDER_FULL 
         */
         if (status_byte_is_present)
         {
            status_byte |= PDUSTATUS_REMAINDER_FULL;
            DSM_PUSHDOWN (pdu_ptr, &status_byte, 1); /*lint !e666*/
         }
         /* Otherwise this is a full PDU for RLP version 2... indicate
         ** it in the DSM app_field so that the appropriate RLP
         ** header bit can be set later on
         */
         else
         {
            (*pdu_ptr)->app_field |= RLP_DSM_APP_FIELD_NO_STATUS_BYTE;
         }          
      }
      
      /* Make a partially full PDU */
      else
      {        
         /* Small and Large patrial PDU constructed differently */
         if (PDU_TYPE_SMALL == PDU_type)
         {
            /* Take care of case where status byte is present (and we have
            ** at least one other byte available) or we have
            ** at least 2 extra bytes in the PDU.  In this case we will
            ** add one status byte in the beginning of the PDU with
            ** and address field filled in, and another at the end of 
            ** the PDU with REMAINDER_EMPTY */
            if ((status_byte_is_present) ||
                ((PDU_user_data_size - num_chars_read) > 1)
               )
            {
               /* SMALL patrial PDU */
               /* 2 L2R status bytes, one at the start */
               /* of the data and one at the end */
               
               /* Add address field and put at beginning of PDU */
               status_byte |= (uint8) num_chars_read;
               DSM_PUSHDOWN (pdu_ptr, &status_byte, 1); /*lint !e666*/
               
               /* Add remainder_empty and add to end of PDU */             
               status_byte = (status_byte & PDUSTATUS_STATUS_BITS) |
                             PDUSTATUS_REMAINDER_EMPTY;
               DSM_PUSHDOWN_TAIL (pdu_ptr, &status_byte, 1); /*lint !e666*/
            }
             
            /* It is an RLP version 2 PDU with only one extra byte
            ** in it.
            ** In this case, we will add a 
            ** status byte to the beginning and indicate that the 
            ** remainder of the PDU is full
            */
            else 
            {
               status_byte |= PDUSTATUS_REMAINDER_FULL;
               DSM_PUSHDOWN (pdu_ptr, &status_byte, 1); /*lint !e666*/
            }              
         }
         else /* Partially filled Large PDU */
         {
            /* For partially filled large PDU's, we know that we have at 
            ** least 3 free bytes in the PDU.... the 
            ** ds_ucsd_V24_interface_handler_byte_oriented() guarantees it.
            ** Therefore, we come down to two cases:
            */
            
            /* If there are 23 user bytes or less, we will add a status
            ** byte at the beginning with a valid address field, and 
            ** another status byte at the end with REMAINDER_EMPTY
            */
            if (num_chars_read < 24) // More efficient than '<= 23' ?
            {
               /* Add address field and put at beginning of PDU */
               status_byte |= (uint8) num_chars_read;
               DSM_PUSHDOWN (pdu_ptr, &status_byte, 1); /*lint !e666*/
               
               /* Add remainder_empty and add to end of PDU */             
               status_byte = (status_byte & PDUSTATUS_STATUS_BITS) |
                             PDUSTATUS_REMAINDER_EMPTY;
               DSM_PUSHDOWN_TAIL (pdu_ptr, &status_byte, 1); /*lint !e666*/
            }
            else
            {
               /* Otherwise, we will add a 2 byte status at the beginning
               ** of the PDU, and then another at the end with the
               ** REMAINDER_EMPTY flag set
               */
               
               /* Two byte status field... first add address byte
               ** then add Status byte with PDUSTATUS_2BYTE_STATUS
               */
               /* Store in temp, then use temp for pushdown so
               ** we can keep num_char_read a register variable
               */
               uint8 tmp;
               tmp = num_chars_read;
               ASSERT(NULL != pdu_ptr);
               DSM_PUSHDOWN (pdu_ptr, &tmp, 1); /*lint !e666*/
               status_byte |= PDUSTATUS_2BYTE_STATUS;
               ASSERT(NULL != pdu_ptr);
               DSM_PUSHDOWN (pdu_ptr, &status_byte, 1); /*lint !e666*/
               
               /* Add remainder_empty and add to end of PDU */             
               status_byte = (status_byte & PDUSTATUS_STATUS_BITS) |
                             PDUSTATUS_REMAINDER_EMPTY;
               ASSERT(NULL != pdu_ptr);
               DSM_PUSHDOWN_TAIL (pdu_ptr, &status_byte, 1); /*lint !e666*/
            }
            
         }
         //MSG_LOW ("ds_ucsd_EncodePDU: Made partial PDU [%d]",num_chars_read,0,0);
      }
   }
   else /* No status or data to send, send no PDU */
   {
      RetVal = FALSE;
      /* Purge DSM item if not sending PDU */
      if (*pdu_ptr != NULL)
      {
         //MSG_LOW ("Not sending PDU - Purge",0,0,0);
         ds_ucsd_UplinkPDUMgr_Free(pdu_ptr);
      }
   }   
   /* Update user data inactivity timer */
   ds_gcsd_s30_timer_control(num_chars_read);
   
   /* Increment the tx data count */
   Ds_ucsd_nt_tx_data_count += num_chars_read;
      
   return RetVal;
}


/* <EJECT> */
/*===========================================================================

FUNCTION 
   ds_ucsd_DecodeStatus

DESCRIPTION
   This routine decodes the Rx PDU status byte.
         
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if there are more status bytes in the PDU to be processed
  FALSE if there are not any more status bytes in the PDU to be processed
    
SIDE EFFECTS
  Event may be sent to L2R.  May also alter BreakRxdFromNetwork, DiscardRxData
  and Ds_ucsd_nt_filtered_rx_status globals. 

===========================================================================*/
LOCAL 
boolean
ds_ucsd_DecodeStatus 
(
   dsm_item_type **pdu_ptr,
   int32         *pdu_size,
   int32         *bytes_till_next_status,
   boolean       *valid_rx_data,
   boolean        peek_and_trim,
   boolean        useStatusBytes
)
{
   register int32   local_pdu_size   = *pdu_size;
   register boolean more_status_bytes_in_pdu = FALSE;
            uint8   pdu_byte;

   /* Extract L2R status header */
   DSM_PULLUP (pdu_ptr, &pdu_byte, 1); /*lint !e666 */
   
   local_pdu_size--;
   //DS_UCSD_DEBUG_MSG_2 ("Got status byte %x, pdu size is now %d",
   //                                       pdu_byte,local_pdu_size);

   if (useStatusBytes)
   {
     /* Extract the relevant status information */
     Ds_ucsd_nt_filtered_rx_status.SB_status_rx =
        (pdu_byte & PDUSTATUS_DCD_MASK) ? V24_CONTROL_OFF : V24_CONTROL_ON;

     Ds_ucsd_nt_filtered_rx_status.X_status_rx =
        (pdu_byte & PDUSTATUS_CTS_MASK) ? V24_CONTROL_OFF : V24_CONTROL_ON;
   }
   
   /* Now get the address portion of the PDU status byte */               
   pdu_byte &= PDUSTATUS_ADDRESS_MASK;
         
   if (useStatusBytes && (pdu_byte == PDUSTATUS_BREAK))
   {
      *valid_rx_data = FALSE;
      BreakRxdFromNetwork = TRUE;
      DS_UCSD_DEBUG_MSG ("BREAK rxd from Network");
   }
   
   if (useStatusBytes && (pdu_byte == PDUSTATUS_BREAK_ACK))
   {
      WRITE_EVENT_TO_L2R (L2R_BREAK_ACK_IND);
      *valid_rx_data = FALSE;
      ds_ucsd_DownlinkPDUMgr_Empty (); /* remove any PDUs rxd in the meantime */
      DS_UCSD_DEBUG_MSG ("BREAK_ACK rxd from Network");
   }
   else if (pdu_byte == PDUSTATUS_REMAINDER_EMPTY)
   {
      *valid_rx_data = FALSE;
   }      
   else if (pdu_byte == PDUSTATUS_REMAINDER_FULL)
   {
      *valid_rx_data = TRUE;
   }
   
   /* There is an address in the address field of the PDU */
   else 
   {
      /* There is valid data in the PDU */
      *valid_rx_data = TRUE;
      
      /* See if it is a 2-byte status */
      if (pdu_byte == PDUSTATUS_2BYTE_STATUS)
      {
         DSM_PULLUP (pdu_ptr, &pdu_byte, 1); /*lint !e666 */
         local_pdu_size--;
         pdu_byte &= PDUSTATUS_2BYTE_STATUS_ADDRESS_MASK;
      }
      /* At this point, pdu_byte contains the number of data 
      ** bytes till the next status byte.  If there is only one
      ** remaining status byte and it is at the end of the PDU,
      ** let's pull it out and save the info here, thus maybe
      ** requiring possibly no copying from the calling routine
      */
      if ((int32) pdu_byte == (local_pdu_size - 1))
      {
         DSM_PULLUP_TAIL (pdu_ptr, &pdu_byte, 1) /*lint !e666 */
         local_pdu_size--;
         
         if (useStatusBytes)
         {
           /* Extract the relevant status information */
           Ds_ucsd_nt_filtered_rx_status.SB_status_rx =
              (pdu_byte & PDUSTATUS_DCD_MASK) ? V24_CONTROL_OFF : V24_CONTROL_ON;

           Ds_ucsd_nt_filtered_rx_status.X_status_rx =
             (pdu_byte & PDUSTATUS_CTS_MASK) ? V24_CONTROL_OFF : V24_CONTROL_ON;
         }
      }
      /* Next status byte is not last byte of PDU */ 
      else
      {
         if (peek_and_trim)
         {
            uint8 next_status_byte;
            /* Take a peek at the next status byte */
            if (!dsm_peek_byte (*pdu_ptr,(uint16)pdu_byte,&next_status_byte))
            {
               MSG_ERROR ("dsm_peek_byte failure: offset=%d, size=%d",pdu_byte,
                          dsm_length_packet(*pdu_ptr),0);
            }
            /* If next status byte indicates that the remainder of the
            ** packet is empty, save the status information from it and
            ** then trim the packet to include only the user data
            */ 
            else if ((next_status_byte & PDUSTATUS_ADDRESS_MASK) ==
                                              PDUSTATUS_REMAINDER_EMPTY) 
            {
              /* DS_UCSD_DEBUG_MSG_1 ("Peek of next stat is REM_EMPTY,"
                                      " trim to %d",pdu_byte); */
              if (useStatusBytes)
              {
                 /* Extract the relevant status information */
                 Ds_ucsd_nt_filtered_rx_status.SB_status_rx =
                   (next_status_byte & PDUSTATUS_DCD_MASK) ? V24_CONTROL_OFF 
                                                           : V24_CONTROL_ON;

                 Ds_ucsd_nt_filtered_rx_status.X_status_rx =
                   (next_status_byte & PDUSTATUS_CTS_MASK) ? V24_CONTROL_OFF 
                                                           : V24_CONTROL_ON;
              }
              /* Trim the packet to be just the user data */
              dsm_trim_packet(pdu_ptr,(uint16)pdu_byte);

              /* Set the new PDU size */
              local_pdu_size = pdu_byte;
            }
            else
            {
              *bytes_till_next_status = (int32) pdu_byte;
              more_status_bytes_in_pdu = TRUE;
               //DS_UCSD_DEBUG_MSG_2 ("Peek at next stat is %x, bytes to next stat=%d",
               //                  next_status_byte,pdu_byte,0);
            }     
         }

         /* peek_and_trim not set, just indicate that there are more 
         ** status bytes in the PDU
         */
         else
         {
            *bytes_till_next_status = (int32) pdu_byte;
            more_status_bytes_in_pdu = TRUE;
         }
      }/* next status byte isn't last byte of PDU */         
   } /* there is an address field in PDU */

   *pdu_size = local_pdu_size;
   return more_status_bytes_in_pdu;
}


/* <EJECT> */
/*===========================================================================

FUNCTION
   ds_ucsd_UnpackPDU

DESCRIPTION
   This routine decodes the PDUs (for Phase2).
   The data bytes are read from the PDU and written to 
   the appropriate rx buffer (depending on whether or not
   rx compression is being used). The rx status info 
   is stored depending on flag.
         
DEPENDENCIES
  None
  
RETURN VALUE
  Number of bytes in PDU 
    
SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
uint32
ds_ucsd_UnpackPDU 
(
   dsm_item_type ** pdu_ptr,
   PDU_Type PDU_type,
   boolean useStatusBytes
)
{
   uint32  user_data_count = 0;
   int32  bytes_till_next_status;
   int32  pdu_size;
   boolean valid_rx_data;

   if (NULL == pdu_ptr)
   {
     DATA_MSG0_ERROR( "Undefined target PDU pointer");
     return 0;
   }
   
   /* Determine byte size of PDU (including status header if present).
    * During remap when status bytes are not to be stored, original
    * payload size is required. */ 
   pdu_size = (TRUE == useStatusBytes)?
                 get_current_rlp_pdu_payload_size() : OldPDUsize;
      
   /* Check to see if there is embedded status data in the PDU
   */
   if ((*pdu_ptr)->app_field & RLP_DSM_APP_FIELD_NO_STATUS_BYTE)
   {
      user_data_count = (uint32)pdu_size;
   }
   
   /* If there is embedded status data, check to see if we may need to
   ** remove embedded status bytes and if there is valid RX data in the
   ** PDU
   */
   else if ((TRUE == ds_ucsd_DecodeStatus (pdu_ptr,
                                           &pdu_size,
                                           &bytes_till_next_status,
                                           &valid_rx_data,
                                           TRUE,
                                           useStatusBytes))
           && valid_rx_data)
   {
      /* If we get to this point, it means there are more status
      ** bytes in the PDU
      */
      register boolean more_status_bytes_in_pdu = TRUE;

      /* For efficiency */
      register int32 tmp_size = bytes_till_next_status;

      /* Use temporary buffers for byte work */
      byte  PDU[RLP_PDU_MAX_USER_DATA_SIZE];      
      PDU[0] = '\0';
      
      /* Partial PDU byte-wise scan */
      while ( (pdu_size > 0) && valid_rx_data)
      {
         /* Pull out the user data into the temporary buffer.  Update
         ** the user_data_count and the pdu_size 
         */
         DSM_PULLUP (pdu_ptr, &PDU[user_data_count], (word)tmp_size); /*lint !e666 */
         user_data_count += (uint32)tmp_size;
         pdu_size -= tmp_size;

   
         /* If there are more status bytes, decode them. */
         if (more_status_bytes_in_pdu)
         {
           more_status_bytes_in_pdu =
             ds_ucsd_DecodeStatus ( pdu_ptr,
                                    &pdu_size,
                                    &bytes_till_next_status,
                                    &valid_rx_data,
                                    FALSE,
                                    useStatusBytes );
         }
         
         /* Set the size of the next read from the PDU */
         if (more_status_bytes_in_pdu)
         {
            tmp_size = bytes_till_next_status;
            
         }
         else
         {
            tmp_size = pdu_size;
         }      
      }
      
      /* Purge the temp DSM item */
      ds_ucsd_DownlinkPDUMgr_Free(pdu_ptr);
         
      /* Put temporary buffer in new PDU */
      /*lint -e613 */
      *pdu_ptr = ds_ucsd_DownlinkPDUMgr_New();
      ASSERT (NULL != *pdu_ptr);
      DSM_PUSHDOWN_TAIL (pdu_ptr, PDU, (uint16)user_data_count); /*lint !e666*/
      /*lint +e613 */
   }
   
   /* Either there was no valid rx data, or there were no
   ** status bytes embedded in the PDU.  If there was valid
   ** RX data, update the user_data_count
   */
   else if (valid_rx_data)
   {
      user_data_count = (uint16)pdu_size;
   }
   
   return user_data_count;
}

/* <EJECT> */
/*===========================================================================

FUNCTION
   ds_ucsd_DecodePDU

DESCRIPTION
  This routine manages unpacking the PDU and writing to downstream buffer.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
LOCAL void 
ds_ucsd_DecodePDU 
(
   dsm_item_type ** pdu_ptr,
   PDU_Type               PDU_type,
   boolean useStatusBytes,
   FPTR_WRITE_TO_BUFFER pWriteToBuffer
)
{
   uint32  user_data_count = 0;

   ASSERT( NULL != pWriteToBuffer );
   
   /* Unpack PDU to remove status bytes */
   user_data_count = ds_ucsd_UnpackPDU (pdu_ptr, PDU_type, useStatusBytes);

   /* Write PDU to Rx buffer (unless discarding) */
   if (user_data_count && (FALSE == DiscardRxData))
   {

#ifdef DS_UCSD_DEBUG
      num_chars_rxd += user_data_count;
#endif

      /* Write to downstream stage */
      (*pWriteToBuffer)(pdu_ptr);
   }
   /* Update user data inactivity timer */
   ds_gcsd_s30_timer_control(user_data_count);
 
   /* Increment the rx data count */
   Ds_ucsd_nt_rx_data_count +=  user_data_count;
}

/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_HandleRemapData

DESCRIPTION
   Retrieves a PDU from the UplinkPDUMgr (after a REMAP)
   and stores the data in the remap_pending_buffer.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_HandleRemapData ( void )
{
   PDU_Type status;
   dsm_item_type * PDU;

   DS_UCSD_DEBUG_MSG ("In ds_ucsd_HandleRemapData");

   /* Get data from old PDU list */
   status = ds_ucsd_UplinkPDUMgr_Retrieve (&PDU);

   if (status != PDU_TYPE_INVALID)
   {
      /* Find alternate PDU type for original UL list */
      PDU_Type PDUtype = (PDU_TYPE_SMALL == ds_ucsd_UplinkPDUMgr_GetType())?
                     PDU_TYPE_LARGE : PDU_TYPE_SMALL;

      DS_UCSD_DEBUG_MSG ("Decoding PDU into Remap buffer");
           
      /* Extract the data and status from PDU */
      ds_ucsd_DecodePDU (&PDU, PDUtype, FALSE,
                         ds_write_remap_pending_buffer);
   }

   /* Release the retrieved PDU */
   ds_ucsd_UplinkPDUMgr_Free (&PDU);
}



/* <EJECT> */
/*===========================================================================

FUNCTION    ds_ucsd_l2r_transmit

DESCRIPTION
   This function sends data over the non-transparent link.
   It takes V24 status/data words from the tx buffer,
   and builds a PDU structure. This is then sent to the RLP.
         
DEPENDENCIES
  None
  
RETURN VALUE
   0:   No data sent
   1:   1 PDU sent
   RLP_ERROR: Error ocurred
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL short 
ds_ucsd_l2r_transmit (void)
{
   short RetVal = 0;
   dsm_item_type * PDU = NULL;
   boolean Send = FALSE;

   /* Check that it is ok to send to the RLP.  If it isn't,
      then data will build up in the tx_buffer, and eventually
      CTS will be brought down by the V24_interface_driver */
   /*   if ((!rlp_flow_control()) && (ds_HardwareTxReady ()))*/
   if ((ds_ucsd_UplinkPDUMgr_SendSlotsAvailable()) &&
       (ds_HardwareTxReady()) && Do_L2R_RLP &&
       (FALSE == DSM_Level_Low))
   {
      /* Fill PDU with data */
      /* First two cases deal with remapped data */
      if (!ds_check_empty_remap_pending_buffer ())
      {
         /* Send data stored in the Remap Pending Buffer */
         Send = ds_ucsd_EncodePDU (&PDU,
                                   ds_ucsd_get_CurrentPDUType(),
                                   ds_read_remap_pending_buffer,
                                   ds_check_remap_pending_buffer);
         DS_UCSD_DEBUG_MSG_1 ("Called ds_ucsd_EncodePDU with Remap buffer: Send=%d", Send);
      }
      else if (ds_ucsd_UplinkPDUMgr_RemapDataPending ())
      {
         /* Retrive data from UplinkPDUMgr, which contains PDUs in
            the old PDU list which have been encoded before the Remap. The
            data from these will be written to the Remap Pending Buffer*/
         ds_ucsd_HandleRemapData ();
      }
      else 
      {
         /* Set te current PDU size so its known in case a remap
          * occurs.  Once the remap starts, the PDUMgr is switched to
          * the alternate frame size so we do not easily have it */
         OldPDUsize = get_current_rlp_pdu_payload_size();
        
         /* Send data from the current input buffer (either the serial
            tx buffer or the V42bis tx buffer if using compression) */
         Send = ds_ucsd_EncodePDU (&PDU,
                                   ds_ucsd_get_CurrentPDUType(),
                                   pUplinkRead,
                                   pUplinkCheck);
      }
      
      /* Send the PDU to the RLP for transmission */
      if (Send)
      {
         /* Update PDU state and release DSM item */
         if (ds_ucsd_UplinkPDUMgr_Write (&PDU) != PDU_TYPE_INVALID)
         {
            RetVal = 1; /* Indicate data sent */
         }
         else
         {
            RetVal = RLP_ERROR;
         }
      }
   }
   
   return RetVal;
}





/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_l2r_receive

DESCRIPTION
    This function reads a PDU from the RLP, and sends it to the serial driver.
         
DEPENDENCIES
  None
  
RETURN VALUE
  0:No data read, 1:Data Read OK   RLP_ERROR:Error!
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL short 
ds_ucsd_l2r_receive (void)
{
   register short  return_code = FALSE;
   PDU_Type        pdu_type;
   dsm_item_type * PDU = NULL;
   
   /* Always DSR=On when RLP is connected (which it
    * must be now that PDUs are being received) */         
   Ds_ucsd_nt_filtered_rx_status.SA_status_rx = V24_CONTROL_ON; /* DSR */

   /* While data is available from the RLP then fetch PDUs */
   while ( ds_ucsd_DownlinkPDUMgr_DataAvailable() &&
           Do_L2R_RLP &&
           (!(*pDownlinkCheckFull)()) )
   {
      /* Get PDU with data */
      pdu_type = ds_ucsd_DownlinkPDUMgr_Read (&PDU);
      
      if ((pdu_type != PDU_TYPE_INVALID) && (NULL != PDU))
      {
         /* Extract the data and status from PDU */
         ds_ucsd_DecodePDU (&PDU, pdu_type, TRUE, pDownlinkWrite);
         
         /* Indicate data read */
         return_code = TRUE;
      }

      /* Free PDU from list */
      ds_ucsd_DownlinkPDUMgr_Free (&PDU);
   }
   
   return return_code;
}




/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_StoreRLPParametersInConfigArea

DESCRIPTION
   This routine stores the Current XID parameters into the config data 
   (shared memory).
   
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_StoreRLPParametersInConfigArea 
(
   ds_ucsd_nt_rlp_parameters_T     *RLP_params,
   v42_comp_direction_T compression
)
{
  XID_Params_T Rlp_CurrentXID;

  memscpy(&Rlp_CurrentXID, sizeof(XID_Params_T), 
          ds_ucsd_get_current_xid(), sizeof(XID_Params_T));
  if (RLP_params)
  {
    RLP_params->compression = compression;
      
    RLP_params->rlp_iws = Rlp_CurrentXID.IWFtoMSWindow;
    RLP_params->rlp_mws = Rlp_CurrentXID.MStoIWFWindow;
    RLP_params->rlp_t1  = Rlp_CurrentXID.T1;
    RLP_params->rlp_n2  = Rlp_CurrentXID.N2;
      
    RLP_params->rlp_Pt = Rlp_CurrentXID.Pt;
    RLP_params->rlp_P0 = Rlp_CurrentXID.P0;
    RLP_params->rlp_P1 = Rlp_CurrentXID.P1;
    RLP_params->rlp_P2 = Rlp_CurrentXID.P2;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_nt_rlp_requestedXID_initialise 

DESCRIPTION
  This routine initializes the XID requested by the UE based on the
  call mode.  This ensures the appropropiate RLP version is proposed
  to the network during XID negotiations.  Some networks do not handle
  RLP v2, fail to respond or respond incorrectly. 
         
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_nt_rlp_requestedXID_initialise 
(
   sys_sys_mode_e_type          curr_mode
)
{
   /* Initialise RLP Parameters */
   Rlp_RequestedXID.T2            = XIDDEFAULT_T2;

   /* RLP version 2 values apply only to WCDMA builds */
   Rlp_RequestedXID.RLPVersion    = (DS_UCSD_IS_WCDMA_TDS_MODE(curr_mode))?
                                      Ds_ucsd_nt_rlp_params.rlp_ver_wcdma :
                                      Ds_ucsd_nt_rlp_params.rlp_ver_gsm;
   
   /* Select version specific parameters */
   if (XIDDEFAULT_VERSION_2 == Rlp_RequestedXID.RLPVersion)
   {
     /* RLP version 2 */
     Rlp_RequestedXID.IWFtoMSWindow = Ds_ucsd_nt_rlp_params.rlp_iws_V2;
     Rlp_RequestedXID.MStoIWFWindow = Ds_ucsd_nt_rlp_params.rlp_mws_V2;
     Rlp_RequestedXID.T1            = Ds_ucsd_nt_rlp_params.rlp_t1_V2;
     Rlp_RequestedXID.N2            = Ds_ucsd_nt_rlp_params.rlp_n2_V2;
   }
   else
   {
     /* RLP version 0,1 */
     Rlp_RequestedXID.IWFtoMSWindow = Ds_ucsd_nt_rlp_params.rlp_iws_V1;
     Rlp_RequestedXID.MStoIWFWindow = Ds_ucsd_nt_rlp_params.rlp_mws_V1;
     Rlp_RequestedXID.T1            = Ds_ucsd_nt_rlp_params.rlp_t1_V1;
     Rlp_RequestedXID.N2            = Ds_ucsd_nt_rlp_params.rlp_n2_V1;
   }
   
   /* These values control the V42bis data compression */
   Rlp_RequestedXID.Pt = Ds_ucsd_nt_rlp_params.rlp_Pt;
   Rlp_RequestedXID.P1 = Ds_ucsd_nt_rlp_params.rlp_P1;
   Rlp_RequestedXID.P2 = Ds_ucsd_nt_rlp_params.rlp_P2;
   
   /* If negotiated bearer capability has compression on,
   ** set compression according to +CRLP value, otherwise
   ** turn it off
   */
   if (Ds_ucsd_nt_bc_compression_enabled)
   {
      Rlp_RequestedXID.P0 = Ds_ucsd_nt_rlp_params.rlp_P0;
   }
   else
   {
      Rlp_RequestedXID.P0 = (byte)V42_NO_COMPRESSION;
   }
   
   /* This value only used for >= version 2 */
   Rlp_RequestedXID.T4 = XIDDEFAULT_T4;

} /* ds_ucsd_nt_rlp_requestedXID_initialise */


/* <EJECT> */
/*===========================================================================

FUNCTION ds_ucsd_nt_register_ind_report_func

DESCRIPTION
  This function registers the function that will be used by the UMTS
  non-transparent data stack to report various indications.
  
DEPENDENCIES
  This function MUST be called prior to calling
  ds_ucsd_nt_init_call_data()

RETURN VALUE
  None

SIDE EFFECTS
  Sets the internally kept NT data stack indicator report callback function

===========================================================================*/
void  
ds_ucsd_nt_register_ind_report_func
(
   ds_ucsd_nt_ind_report_cb_T ind_report_cb   
)
{
   Ds_ucsd_nt_ind_report_cb = ind_report_cb;
}
                                     

/*===========================================================================

FUNCTION ds_ucsd_nt_register_no_tx_frames_sent_func

DESCRIPTION
  This function registers the function that will be used by the UMTS
  non-transparent data stack to report when no tx data frames were
  sent in WCDMA mode due to duplicate frame suppression being enabled,
  and tx data being all duplicate frames
  
DEPENDENCIES
  This function MUST be called prior to calling
  ds_ucsd_nt_init_call_data()

RETURN VALUE
  None

SIDE EFFECTS
  Sets the internally kept NT data stack indicator report callback function

===========================================================================*/
void
ds_ucsd_nt_register_no_tx_frames_sent_func
(
   ds_ucsd_nt_no_tx_frames_sent_cb_T    no_tx_frames_sent_cb   
)
{
   Ds_ucsd_nt_no_tx_frames_sent_cb = no_tx_frames_sent_cb;
}
   

/*===========================================================================

FUNCTION ds_ucsd_nt_register_sio_funcs

DESCRIPTION
  This function is used to register with the UMTS NT data stack the 
  various callback functions related to the SIO interface.  Note that
  valid callback function pointers MUST be supplied for all functions
  except for the initialization (ini_sio_cb) function which may be
  NULL.
  
DEPENDENCIES
  This function MUST be called prior to calling
  ds_ucsd_nt_init_call_data()

RETURN VALUE
  None

SIDE EFFECTS
  Sets the internally kept NT data stack SIO callback function pointers

===========================================================================*/
void
ds_ucsd_nt_register_sio_funcs
(
   ds_ucsd_nt_rx_from_sio_cb_T        rx_from_sio_cb,
   ds_ucsd_nt_tx_to_sio_cb_T          tx_to_sio_cb,
   ds_ucsd_nt_get_sio_status_cb_T     get_sio_status_cb,
   ds_ucsd_nt_set_sio_cts_state_cb_T  set_sio_cts_state_cb,
   ds_ucsd_nt_is_sio_tx_q_full_cb_T   is_sio_tx_q_full_cb,
   ds_ucsd_nt_suspend_sio_tx_cb_T     suspend_sio_tx_cb,
   ds_ucsd_nt_init_sio_cb_T           init_sio_cb
)
{
   Ds_ucsd_nt_rx_from_sio_cb               = rx_from_sio_cb;
   Ds_ucsd_nt_tx_to_sio_cb                 = tx_to_sio_cb;
   Ds_ucsd_nt_get_sio_status_cb            = get_sio_status_cb;
   Ds_ucsd_nt_set_sio_cts_state_cb         = set_sio_cts_state_cb;
   ds_ucsd_nt_cb_info.is_sio_tx_q_full_cb  = is_sio_tx_q_full_cb;
   Ds_ucsd_nt_suspend_sio_tx_cb            = suspend_sio_tx_cb;
   Ds_ucsd_nt_init_sio_cb                  = init_sio_cb;
}
   


/*===========================================================================

FUNCTION ds_ucsd_nt_register_ota_funcs

DESCRIPTION
  This function is used to register with the UMTS NT data stack the 
  various callback functions related the OTA interface.  Note that
  valid callback function pointers MUST be supplied for all functions
  except for the initialization (ini_ota_cb) function which may be
  NULL.
  
DEPENDENCIES
  This function MUST be called prior to calling
  ds_ucsd_nt_init_call_data()

RETURN VALUE
  None

SIDE EFFECTS
  Sets the internally kept NT data stack OTA callback function pointers

===========================================================================*/
void
ds_ucsd_nt_register_ota_funcs
(
   ds_ucsd_nt_read_dl_ota_data_cb_T   read_dl_ota_data_cb,
   ds_ucsd_nt_write_ul_ota_data_cb_T  write_ul_ota_data_cb,
   ds_ucsd_nt_get_ul_ota_buf_cb_T     get_ul_ota_buf_cb,
   ds_ucsd_nt_init_ota_cb_T           init_ota_cb
)
{
   ds_ucsd_nt_cb_info.read_dl_ota_data_cb = read_dl_ota_data_cb;
   ds_ucsd_nt_cb_info.write_ul_ota_data_cb = write_ul_ota_data_cb;
   ds_ucsd_nt_cb_info.get_ul_ota_buf_cb    = get_ul_ota_buf_cb;
   Ds_ucsd_nt_init_ota_cb                  = init_ota_cb;
}




/*===========================================================================

FUNCTION ds_ucsd_nt_init_call_data

DESCRIPTION
  This function is used to initialize data for every call and on
  interRAT handover.  It should be called when the call is connected.
  Note that the suppress_wcdma_duplicate_tx_data parameter should be
  set to TRUE to stop duplicate TX data from being transmitted in
  WCDMA mode.
  
DEPENDENCIES
  ds_ucsd_nt_register_ota_funcs() and ds_ucsd_nt_register_sio_funcs(),
  and ds_ucsd_nt_register_ind_report_func() MUST be called prior to 
  calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  Sets the internally kept NT data stack state information.

===========================================================================*/
void
ds_ucsd_nt_init_call_data
(
  const cm_call_id_type              call_id,
  const sys_sys_mode_e_type          curr_mode,
  const ds_ucsd_frame_size_T         bytes_per_sdu,                           
  const uint32                       sdus_per_tti,
  const ds_ucsd_nt_rlp_parameters_T  *rlp_params_ptr,
  const ds_ucsd_data_rate_T          data_rate,
  const boolean                      bc_compression_enabled,
  const boolean                          suppress_wcdma_duplicate_tx_data,
  ds_ucsd_set_data_counters_cb_T         set_data_counters_cb      
)
{
   uint8 pattern;

   ASSERT( NULL != rlp_params_ptr );
   
   /* Check to make sure other initialization functions have
   ** already been called
   */
   if (! ((Ds_ucsd_nt_ind_report_cb)        &&
          (Ds_ucsd_nt_rx_from_sio_cb)       &&
          (Ds_ucsd_nt_tx_to_sio_cb)         &&
          (Ds_ucsd_nt_get_sio_status_cb)    &&
          (Ds_ucsd_nt_set_sio_cts_state_cb) &&
          (ds_ucsd_nt_cb_info.is_sio_tx_q_full_cb)  &&
          (Ds_ucsd_nt_suspend_sio_tx_cb)    &&
          (ds_ucsd_nt_cb_info.read_dl_ota_data_cb)  &&
          (ds_ucsd_nt_cb_info.write_ul_ota_data_cb) &&                                                               
          (ds_ucsd_nt_cb_info.get_ul_ota_buf_cb))
       )
   {
      DS_3GPP_MSG3_ERROR ("UCSD NT: Not all reqd callbacks registered",0,0,0);
   }
   
   /* Set the various data values */
   Ds_ucsd_nt_call_id               = call_id;
   Ds_ucsd_nt_curr_mode             = curr_mode; 
   Ds_ucsd_nt_bytes_per_sdu         = bytes_per_sdu;
   Ds_ucsd_nt_sdus_per_tti          = sdus_per_tti;
   Ds_ucsd_nt_rlp_params            = *rlp_params_ptr;
   Ds_ucsd_nt_data_rate             = data_rate;
   Ds_ucsd_nt_bc_compression_enabled = bc_compression_enabled;

   /* Set data traffic callback if specified (not provided on handover) */
   if( set_data_counters_cb )
   {
     Ds_gcsd_set_data_counters_cb       = set_data_counters_cb;
   }
     
   /* Set suppress duplicate TX frames flag */
   if ((DS_UCSD_IS_WCDMA_TDS_MODE(curr_mode)) && 
       (suppress_wcdma_duplicate_tx_data == TRUE))
   {
      Ds_ucsd_nt_suppress_duplicate_tx_frames = TRUE;
   }
   else
   {
      Ds_ucsd_nt_suppress_duplicate_tx_frames = FALSE;
   }

   /* Set current PDU type */
   if (Ds_ucsd_nt_bytes_per_sdu == DS_UMTS_CSD_FRAME_SIZE_240_BIT)
   {
      ds_ucsd_set_CurrentPDUType(PDU_TYPE_SMALL);
   }
   else
   {
      ds_ucsd_set_CurrentPDUType(PDU_TYPE_LARGE);
   }

   /* Setup default RLP XID parameters based on call mode */
   ds_ucsd_nt_rlp_requestedXID_initialise (curr_mode);
   
   /* Setup I-frame empty information field pattern on call init and remap. */
   /* According to 3GPP 04.21 section 12, TCH/F9.6 uses zero fill
    * pattern while faster data rates use one fill pattern */
   pattern = (DS_UMTS_CSD_FRAME_SIZE_240_BIT == Ds_ucsd_nt_bytes_per_sdu)?
              0x00 : 0xFF;
   memset ((void*)Ds_FillPattern, pattern, sizeof(Ds_FillPattern));

   Ds_ucsd_nt_init_call_data_called = TRUE;             
   
   /* Initialize CSD packet logging on call init and handover. */
   ds_ucsd_log_init(0,
                    0,
                    0,
                    FALSE,
                    Ds_ucsd_nt_data_rate,
                    Ds_ucsd_nt_curr_mode,
                    Rlp_RequestedXID.RLPVersion);
   
   MSG_HIGH ("In ds_ucsd_nt_init_call_data: PDU Type=%d, SupDupTxFrm=%d",
             ds_ucsd_get_CurrentPDUType(),
             Ds_ucsd_nt_suppress_duplicate_tx_frames, 0);
} /* ds_ucsd_nt_init_call_data */

/* <EJECT> */
/*===========================================================================

FUNCTION    ds_ucsd_non_transparent_data_services_initialise

DESCRIPTION
    This function initialises the non-transparent data services.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void 
ds_ucsd_non_transparent_data_services_initialise 
(
   boolean init_end_of_call_data
)
{   
   
   NTP_State = L2RSTATE_IDLE;
   Old_NTP_State = L2RSTATE_IDLE;
   
#ifdef DS_UCSD_DEBUG
   num_chars_rxd = num_chars_txd = 0;
   ds_ucsd_nt_sio_rx_bytes = ds_ucsd_nt_sio_tx_bytes = 0;
#endif

   Sio_rx_saved_byte_count = 0;
   
   Ds_ucsd_nt_check_DCD = FALSE;

   /* Initialise module-scope variables */   

   BreakRxdFromNetwork    = FALSE;
   BreakRxdFromDTE        = FALSE;
   DiscardRxData          = FALSE;
   
   /* Initialize the OldStatusByte */
   /* Force first Tx I-frame to have L2R status */
   OldStatusByte =  0xFF;  
   
   /* Initialise the storage for Modem Input Lines */
   Ds_ucsd_nt_curr_modem_ctl.RTS   = V24_CONTROL_OFF;
   Ds_ucsd_nt_curr_modem_ctl.DTR   = V24_CONTROL_OFF;   

   /* Initialize all output modem control lines  per 3GPP 27.001 8.1.2.1 */
   Ds_ucsd_nt_filtered_rx_status.X_status_rx  = V24_CONTROL_OFF; /* CTS */
   Ds_ucsd_nt_filtered_rx_status.SA_status_rx = V24_CONTROL_OFF; /* DSR */
   Ds_ucsd_nt_filtered_rx_status.SB_status_rx = V24_CONTROL_OFF; /* DCD */
   Ds_ucsd_nt_saved_filtered_rx_status = Ds_ucsd_nt_filtered_rx_status;
      
   ds_HardwareInitialise( TRUE, TRUE );

   /* Initialise the counter-timers */
   INIT_TIMERS();
   
   /* Initialise the event Queues between L2R/RLP */
   ds_ucsd_event_queue_init (&Evq_To_L2R);
   ds_ucsd_event_queue_init (&Evq_To_RLP);

   /* Initialise the PDU managers */
   ds_ucsd_UplinkPDUMgr_ColdStart   (ds_ucsd_get_CurrentPDUType());
   ds_ucsd_DownlinkPDUMgr_ColdStart (ds_ucsd_get_CurrentPDUType());

   /* Initialise the fifo buffers */
   ds_init_V42_rx_buffer ();
   ds_init_V42_tx_buffer ();
   ds_init_remap_pending_buffer ();

   /* Set up the values for now - these may change on XID negotiation */
   if (Ds_ucsd_nt_bc_compression_enabled)
   {
     Compression =
       ds_ucsd_v42_compression_direction (Ds_ucsd_nt_rlp_params.rlp_P0, TRUE);
   }
   else
   {
     Compression = ds_ucsd_v42_compression_direction ((byte)V42_NO_COMPRESSION, TRUE);
   }
   ds_ucsd_InitialiseBufferpointers (Compression);

   /* Initialise the RLP module */
   rlp_initialise (&Ds_ucsd_nt_rlp_params);
   
   /* Initialize the OTA subsystem if an initialization function has been
   ** supplied
   */
   if (Ds_ucsd_nt_init_ota_cb)
   {
      Ds_ucsd_nt_init_ota_cb();
   }
   
   /* Initialize the SIO subsystem if an initialization function has been
   ** supplied
   */
   if (Ds_ucsd_nt_init_sio_cb)
   {
      Ds_ucsd_nt_init_sio_cb();
   }
    
   /* This flag is used to determine whether or not to perform 
      a transmit of an RLP frame. This is because:
      4800 link:  240-bit frame transmitted every 40ms (2 cycles)
      9600 link:  240-bit frame transmitted every 20ms (1 cycle)
     14400 link:  576-bit frame transmitted every 40ms (2 cycles)
     The L2R and RLP layers are therefore called only every other 
     cycle for 4800 and 14400 bit links.
   */
   Ds_ucsd_nt_perform_rlp_uplink = 0x01;
   
   
   /* If requested, initialize the init_call_data.  This
   ** will be done at the end of a call, but not at the
   ** beginning
   */
   if (init_end_of_call_data)
   {
     Ds_ucsd_nt_init_call_data_called = FALSE;
     Ds_ucsd_nt_suppress_duplicate_tx_frames = FALSE;

     /* Post final data traffic counters */
     if( Ds_gcsd_set_data_counters_cb )
     {
       if( !Ds_gcsd_set_data_counters_cb( Ds_ucsd_nt_call_id,
                                          FALSE,
                                          Ds_ucsd_nt_rx_data_count,
                                          Ds_ucsd_nt_tx_data_count ) )
       {
         DATA_MSG1_ERROR("Problem updating data traffic counters: call ID=%d",
                    Ds_ucsd_nt_call_id);
       }
     }
   }

   /* Initialize the data traffic counters */
   Ds_ucsd_nt_tx_data_count = 0;
   Ds_ucsd_nt_rx_data_count = 0;
       
}


/* <EJECT> */
/*===========================================================================

FUNCTION ds_ucsd_nt_pwrup_init

DESCRIPTION
  This function is used to initialize NT at powerup. Operations that occur
  only during task startup are invoked here.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/ 
void
ds_ucsd_nt_pwrup_init ( void )
{
   /* Initialize PDU Managers */
   ds_ucsd_UplinkPDUMgr_PwrUp_Init ( );
   ds_ucsd_DownlinkPDUMgr_PwrUp_Init ( );
}


/* <EJECT> */
/*===========================================================================

FUNCTION ds_ucsd_nt_pdu_cleanup

DESCRIPTION
  This function is used to ensure the PDU Managers are cleaned up on
  call termination.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/ 
void
ds_ucsd_nt_pdu_cleanup ( void )
{
   //MSG_MED ("In ds_ucsd_nt_pdu_cleanup",0,0,0);
   
   /* Cleanup PDU Managers */
   ds_ucsd_UplinkPDUMgr_Cleanup ( );
   ds_ucsd_DownlinkPDUMgr_Cleanup ( );

   /* Cleanup RLP hardware module */
   ds_HardwareInitialise ( TRUE, TRUE );
}

/* <EJECT> */
/*===========================================================================

FUNCTION    ds_ucsd_Action_L2R_Idle

DESCRIPTION
     Action routine for IDLE state
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_Action_L2R_Idle 
(
   ds_ucsd_nt_rlp_parameters_T  *RLP_params,
   DSI_Event_T                   Event
)
{
   XID_Params_T Rlp_CurrentXID;

   memscpy(&Rlp_CurrentXID, sizeof(XID_Params_T), 
           ds_ucsd_get_current_xid(), sizeof(XID_Params_T));
   switch (Event)
   {
      case DS_UMTS_CSD_CONNECT_EVENT:
         /* When we get the call connect, start a timer that will delay
         ** the start of XID frame transmission */
#ifndef BOA_APPS
         DS_UCSD_DEBUG_MSG("L2R: Got connect request, start connect delay");
         START_TIMER (TIMERID_NT_INIT_DELAY, DS_UCSD_NT_CALL_INIT_DELAY_TIME);
#else
         DS_UCSD_DEBUG_MSG("L2R: Got connect request, wait for XID");
         NTP_State = L2RSTATE_NEGOTIATING_XID;
         WRITE_EVENT_TO_RLP (RLP_ATTACH_REQ);
#endif
         break;

      case RLP_CONNECT_IND:
         /* Connect received from remote node: DON'T connect to it while
            in IDLE state. */
         WRITE_EVENT_TO_RLP (RLP_CONNECT_RES_NEG);
         break;


#ifndef BOA_APPS
      case RLP_XID_IND:

         /* We have received a XID command. The RLP will
            negotiate any values and send the response.
            We are therefore the Responder for the purposes of 
            the negotiation (see 04.22 para 5.5.5.).

            This could happen in this state while we are 
            waiting to send our XID.  If we are not waiting
            for the timer to expire, ignore the XID (call is
            not connected yet)
         */
         if (TIMER_RUNNING(TIMERID_NT_INIT_DELAY))
         {
            DS_UCSD_DEBUG_MSG ("L2R: Processing XID IND in IDLE");

            STOP_TIMER (TIMERID_NT_INIT_DELAY);

            Compression = ds_ucsd_v42_compression_direction (Rlp_CurrentXID.P0, FALSE);
            ds_ucsd_InitialiseBufferpointers (Compression);
            V42_initialise (Rlp_CurrentXID.P2,Rlp_CurrentXID.P1,Compression);

            /* Store parameters in global store */
            ds_ucsd_StoreRLPParametersInConfigArea (RLP_params,Compression);         
         
            WRITE_EVENT_TO_RLP (RLP_CONNECT_REQ);
            NTP_State = L2RSTATE_CONNECTING;
         }
         break;
#endif

      case DS_UMTS_CSD_DISCONNECT_EVENT:
         break;
      
      /* No data to flush, so respond immediately with flush complete 
      ** indication 
      */   
      case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                   Ds_ucsd_nt_call_id );
         break;


      case DS_UMTS_CSD_NULL_EVENT:
      default:

#ifndef BOA_APPS
         /* If delay timer expires, enter the XID negotiation phase */
         if (TIMER_EXPIRED(TIMERID_NT_INIT_DELAY))
         {
            STOP_TIMER (TIMERID_NT_INIT_DELAY);
            /* Connect request from higher layer - proceed with connection:
            **  First ATTACH the RLP then begin XID negotiation 
            */
            DS_UCSD_DEBUG_MSG("L2R: Connect delay expired, neg XID");
            NTP_State = L2RSTATE_NEGOTIATING_XID;
            WRITE_EVENT_TO_RLP (RLP_ATTACH_REQ);
            (void)rlp_XID_send (&Rlp_RequestedXID);
         }
#endif
         break;
   }
}


/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_Action_L2R_Connecting

DESCRIPTION
     Action routine for CONNECTING state
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void  
ds_ucsd_Action_L2R_Connecting 
(
   ds_ucsd_nt_rlp_parameters_T  *RLP_params,
   DSI_Event_T Event
)
{
   int32 delay;
   XID_Params_T Rlp_CurrentXID;

   memscpy(&Rlp_CurrentXID, sizeof(XID_Params_T), 
           ds_ucsd_get_current_xid(), sizeof(XID_Params_T));

   ASSERT( NULL != RLP_params );
   
   switch (Event)
   {
      case L2R_RX_INFO_FRAME:
         DS_UCSD_DEBUG_MSG("L2R: Got RX Info Frame indication");

         break;

      /* It's possible that our XID response was lost and that
      ** we may get another XID indication
      */
      case RLP_XID_IND:
         /* We have received a XID command. The RLP will
            negotiate any values and send the response.
            We are therefore the Responder for the purposes of 
            the negotiation (see 04.22 para 5.5.5.). */
         DS_UCSD_DEBUG_MSG ("L2R: Got XID IND");
         Compression = ds_ucsd_v42_compression_direction (Rlp_CurrentXID.P0, FALSE);
         ds_ucsd_InitialiseBufferpointers (Compression);
         V42_initialise (Rlp_CurrentXID.P2,Rlp_CurrentXID.P1,Compression);

         /* Store parameters in global store */
         ds_ucsd_StoreRLPParametersInConfigArea (RLP_params,Compression);
                  
#ifndef BOA_APPS         
         WRITE_EVENT_TO_RLP (RLP_CONNECT_REQ);
#endif

         NTP_State = L2RSTATE_CONNECTING;
         break;

      case RLP_CONNECT_CNF:
         /* Connected successfully to other node */
         DS_UCSD_DEBUG_MSG_1("L2R: Got connect CNF, OSB=%x",OldStatusByte);
         NTP_State = L2RSTATE_WAIT_FOR_DCD_ON;
         Ds_ucsd_nt_curr_modem_ctl.DTR = V24_CONTROL_ON;
         Ds_ucsd_nt_curr_modem_ctl.RTS = V24_CONTROL_ON;

         if (NT_CONNECT_STATE_RLP == Ds_ucsd_nt_rlp_params.rlp_connect_state)
         {
           /* Signal connection complete to mode-specific handler */
           Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_CONNENCT_COMPLETE_IND,
                                     Ds_ucsd_nt_call_id );
         }
         break;
         
      case RLP_CONNECT_IND:
         /* Connect received from other node: connect to it */
         WRITE_EVENT_TO_RLP (RLP_CONNECT_RES);
         DS_UCSD_DEBUG_MSG_1("L2R: Got connect IND, OSB=%x",OldStatusByte);
         NTP_State = L2RSTATE_WAIT_FOR_DCD_ON;
         Ds_ucsd_nt_curr_modem_ctl.DTR = V24_CONTROL_ON;
         Ds_ucsd_nt_curr_modem_ctl.RTS = V24_CONTROL_ON;

         if (NT_CONNECT_STATE_RLP == Ds_ucsd_nt_rlp_params.rlp_connect_state)
         {
           /* Signal connection complete to mode-specific handler */
           Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_CONNENCT_COMPLETE_IND,
                                     Ds_ucsd_nt_call_id );
         }
         break;
         
      /* If we get a disconnect indication from far end RLP, start
      ** a timer and enter the RLP_DISC_TERMINATING state.  This
      ** gives time for the RLP UA to be transmitted back to the
      ** far end prior to ending the call. 
      */  
      case RLP_DISC_IND:
         ds_ucsd_HandleBreak ();
         /* If ADM idle timer is zero, allow minimum time to send UA frame */
         delay = (0 == RLP_params->rlp_adm_idle_timer)?
                 DS_UCSD_NT_MIN_RLPADM_IDLE_TIME :
                 (int32)(RLP_params->rlp_adm_idle_timer * TIMER_COUNT_1_SEC);
         START_TIMER ( TIMERID_MO_FLUSH, delay );
         NTP_State = L2RSTATE_RLP_DISC_TERMINATING;
         break;
       
      case RLP_ERROR_IND:
         ds_ucsd_HandleBreak ();
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_TERMINATE_CALL_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;   
         
      case RLP_CONNECT_CNF_NEG:
         /* Couldn't connect, try again!  */
         WRITE_EVENT_TO_RLP (RLP_CONNECT_REQ);
         break;

      case DS_UMTS_CSD_DISCONNECT_EVENT:
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         NTP_State = L2RSTATE_IDLE;
         break;
         
      /* No data to flush, so respond immediately with flush complete 
      ** indication 
      */   
      case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;

      default:
         break;  /* Making Lint happy */
   }
}




/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_Action_L2R_NegotiatingXID

DESCRIPTION
    Action routine for NEGOTIATING_XID state.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_Action_L2R_NegotiatingXID 
(
   ds_ucsd_nt_rlp_parameters_T  *RLP_params,
   DSI_Event_T       Event
)
{
   int32 delay;
   XID_Params_T Rlp_CurrentXID;

   memscpy(&Rlp_CurrentXID, sizeof(XID_Params_T), 
           ds_ucsd_get_current_xid(), sizeof(XID_Params_T));

   ASSERT( NULL != RLP_params );
   
   switch (Event)
   {
      case RLP_XID_IND:
         /* We have received a XID command. The RLP will
            negotiate any values and send the response.
            We are therefore the Responder for the purposes of 
            the negotiation (see 04.22 para 5.5.5.). */
         DS_UCSD_DEBUG_MSG ("L2R: Got XID IND");
         Compression = ds_ucsd_v42_compression_direction (Rlp_CurrentXID.P0, FALSE);
         ds_ucsd_InitialiseBufferpointers (Compression);
         V42_initialise (Rlp_CurrentXID.P2,Rlp_CurrentXID.P1,Compression);

         /* Store parameters in global store */
         ds_ucsd_StoreRLPParametersInConfigArea (RLP_params,Compression);
                  
#ifndef BOA_APPS         
         WRITE_EVENT_TO_RLP (RLP_CONNECT_REQ);
#endif

         NTP_State = L2RSTATE_CONNECTING;
         break;
         
      case RLP_XID_CNF:
         /* We have received a XID response to our XID command.
            We are therefore the initiator for the purposes of
            the negotiation (see 04.22 para 5.5.5.). */
         DS_UCSD_DEBUG_MSG ("L2R: Got XID CNF");

         Compression = ds_ucsd_v42_compression_direction (Rlp_CurrentXID.P0, TRUE);
         ds_ucsd_InitialiseBufferpointers (Compression);
         V42_initialise (Rlp_CurrentXID.P2,Rlp_CurrentXID.P1,Compression);
         
         /* Store parameters in global store */
         ds_ucsd_StoreRLPParametersInConfigArea (RLP_params,Compression);         
         WRITE_EVENT_TO_RLP (RLP_CONNECT_REQ);
         NTP_State = L2RSTATE_CONNECTING;
         break;
      
      /* If we get a disconnect indication from far end RLP, start
      ** a timer and enter the RLP_DISC_TERMINATING state.  This
      ** gives time for the RLP UA to be transmitted back to the
      ** far end prior to ending the call. 
      */  
      case RLP_DISC_IND:
         ds_ucsd_HandleBreak ();
         /* If ADM idle timer is zero, allow minimum time to send UA frame */
         delay = (0 == RLP_params->rlp_adm_idle_timer)?
                 DS_UCSD_NT_MIN_RLPADM_IDLE_TIME :
                 (int32)(RLP_params->rlp_adm_idle_timer * TIMER_COUNT_1_SEC);
         START_TIMER ( TIMERID_MO_FLUSH, delay );
         NTP_State = L2RSTATE_RLP_DISC_TERMINATING;
         break;
       
      case RLP_ERROR_IND:
         ds_ucsd_HandleBreak ();
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_TERMINATE_CALL_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;   
         
      case RLP_CONNECT_IND:
         /* Dont connect until we finish XID negotiation
         */
         DS_UCSD_DEBUG_MSG ("L2R: Got Connect IND while doing XID neg...."); 
         WRITE_EVENT_TO_RLP (RLP_CONNECT_RES);
         break;
   
      case DS_UMTS_CSD_DISCONNECT_EVENT:
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         NTP_State = L2RSTATE_IDLE;
         break;
         
      /* No data to flush, so respond immediately with flush complete 
      ** indication 
      */   
      case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;

      default:
         break;  /* Making Lint happy */
   }
}


/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_Action_L2R_Waiting_For_DCD_On

DESCRIPTION
     Action routine for waiting for DCD state

         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void
ds_ucsd_Action_L2R_Waiting_For_DCD_On 
(
   DSI_Event_T Event
)
{
   int32 delay;
  
   switch (Event)
   {
      /* If we get a disconnect indication from far end RLP, start
      ** a timer and enter the RLP_DISC_TERMINATING state.  This
      ** gives time for the RLP UA to be transmitted back to the
      ** far end prior to ending the call. 
      */  
      case RLP_DISC_IND:
         ds_ucsd_HandleBreak ();
         /* If ADM idle timer is zero, allow minimum time to send UA frame */
         delay = (0 == Ds_ucsd_nt_rlp_params.rlp_adm_idle_timer)?
                 DS_UCSD_NT_MIN_RLPADM_IDLE_TIME :
                 (int32)(Ds_ucsd_nt_rlp_params.rlp_adm_idle_timer *
                         TIMER_COUNT_1_SEC);
         START_TIMER ( TIMERID_MO_FLUSH, delay );
         NTP_State = L2RSTATE_RLP_DISC_TERMINATING;
         break;
       
      case RLP_ERROR_IND:
         ds_ucsd_HandleBreak ();
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_TERMINATE_CALL_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;
         
      case DS_UMTS_CSD_DISCONNECT_EVENT:
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         NTP_State = L2RSTATE_IDLE;
         break;
         
      /* No data to flush, so respond immediately with flush complete 
      ** indication 
      */   
      case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;
      
      case DS_UMTS_CSD_NULL_EVENT:   
      default:
         
         // Do the l2receive to decode status bytes and get
         // SB_status_rx, but discard any data
          
         // Dont process any received data 
         DiscardRxData = TRUE;
         
         if (ds_ucsd_get_rlp_process_uplink())
         {
            (void)ds_ucsd_l2r_transmit ();
         }
         
         if (ds_ucsd_get_rlp_process_downlink())
         {
            (void)ds_ucsd_l2r_receive ();
         }

         if (Ds_ucsd_nt_filtered_rx_status.SB_status_rx == V24_CONTROL_ON)
         {
            ds_ucsd_DownlinkPDUMgr_Empty ();
            NTP_State = L2RSTATE_CONNECTED;
            DiscardRxData = FALSE;

            if (NT_CONNECT_STATE_L2R == Ds_ucsd_nt_rlp_params.rlp_connect_state)
            {
              /* Signal connection complete to mode-specific handler */
              Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_CONNENCT_COMPLETE_IND,
                                        Ds_ucsd_nt_call_id );
            }
         }
         
         break;
   }    
   
}


/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_Action_L2R_Connected

DESCRIPTION
     Action routine for CONNECTED state

         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void  
ds_ucsd_Action_L2R_Connected 
(
   DSI_Event_T          Event
)
{
   short status = RLP_OK;
   int32 delay;

   if (Event == (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT)
   {
#ifndef BOA_APPS
      /* If a REMAP to new frame size is required */
      if (ds_ucsd_get_CurrentPDUType() != ds_ucsd_UplinkPDUMgr_GetType())
      {
         Event = L2R_REMAP_REQ;
      }
#endif /* BOA_APPS */
      
      // If doing downlink processing....
      if (ds_ucsd_get_rlp_process_downlink())
      {
         // If we are not checking DCD and DCD goes on, set to start looking
         // for the DCD off condition
         if ((!Ds_ucsd_nt_check_DCD) && 
             (Ds_ucsd_nt_filtered_rx_status.SB_status_rx == V24_CONTROL_ON))
         {
            DS_UCSD_DEBUG_MSG ("NT: DCD checking ON");
            Ds_ucsd_nt_check_DCD = TRUE;
         }
         
         // If DCD checking is on, and we get a DCD off event, set the event
         // to RLP_RESET_IND to end the call
         if (Ds_ucsd_nt_check_DCD && 
             (Ds_ucsd_nt_filtered_rx_status.SB_status_rx == V24_CONTROL_OFF))
         {
            DS_UCSD_DEBUG_MSG ("NT: DCD goes OFF");
            Ds_ucsd_nt_check_DCD = FALSE;
            Event = RLP_ERROR_IND;
         }
      }
   }

   switch (Event)
   {
      case DS_UMTS_CSD_DISCONNECT_EVENT:
         /* A call terminate without a data flush */
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         NTP_State = L2RSTATE_IDLE;
         break;

      case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
         /* local call termination. Flush data before disconnecting */
         V42_terminate  ();
         NTP_State = L2RSTATE_MO_TERMINATING;
         break;
      
      // If we go into online command mode, suspend transmission to SIO   
      case DS_UMTS_CSD_ONLINE_CMD_MODE_EVENT:
         Ds_ucsd_nt_suspend_sio_tx_cb(TRUE);
         break;

      // If we come out of online command mode (back to online mode),
      // resume transmission to SIO
      case DS_UMTS_CSD_ONLINE_DATA_MODE_EVENT:
         Ds_ucsd_nt_suspend_sio_tx_cb(FALSE);
         break;
     
      case L2R_REMAP_REQ:
         WRITE_EVENT_TO_RLP (RLP_REMAP_REQ);
         NTP_State = L2RSTATE_REMAPPING;
         break;
    
      case L2R_DTE_BREAK_IND:
         DS_UCSD_DEBUG_MSG ("Break received from DTE");
         BreakRxdFromDTE = TRUE;
         DiscardRxData   = TRUE;
         ds_ucsd_HandleBreak ();
         NTP_State = L2RSTATE_AWAITING_BREAK_ACK;
         break;
                 
      
      case RLP_RESET_CNF:
      case RLP_RESET_IND:
         ds_ucsd_HandleBreak ();
         break;

      
      /* If we get a disconnect indication from far end RLP, start
      ** a timer and enter the RLP_DISC_TERMINATING state.  This
      ** gives time for the RLP UA to be transmitted back to the
      ** far end prior to ending the call. 
      */  
      case RLP_DISC_IND:
         ds_ucsd_HandleBreak ();
         /* If ADM idle timer is zero, allow minimum time to send UA frame */
         delay = (0 == Ds_ucsd_nt_rlp_params.rlp_adm_idle_timer)?
                 DS_UCSD_NT_MIN_RLPADM_IDLE_TIME :
                 (int32)(Ds_ucsd_nt_rlp_params.rlp_adm_idle_timer *
                         TIMER_COUNT_1_SEC);
         START_TIMER ( TIMERID_MO_FLUSH, delay );
         NTP_State = L2RSTATE_RLP_DISC_TERMINATING;
         break;
       
      case RLP_ERROR_IND:
         ds_ucsd_HandleBreak ();
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_TERMINATE_CALL_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;  
         
         
      case DS_UMTS_CSD_NULL_EVENT:
      default:
         
         /* Do a V42bis cycle */
         status = V42_main ();
         if (status == V42_OK)
         {
            if (ds_ucsd_get_rlp_process_uplink())
            {
               status = ds_ucsd_l2r_transmit ();
            }
            
            if (ds_ucsd_get_rlp_process_downlink())
            {
               status = ds_ucsd_l2r_receive ();
            }
         }
         
         /* A serious problem (not protocol related) has occurred */
         if ((V42_ERROR == status) || (RLP_ERROR == status))
         {

            DATA_MSG0_ERROR("NT:Fatal RLP Error");
            ds_ucsd_HandleBreak (); /* Discard data */
            WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
            Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_TERMINATE_CALL_IND,
                                      Ds_ucsd_nt_call_id );
            NTP_State = L2RSTATE_IDLE;
         }
         break;
   }
}




/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_Action_L2R_RLP_Disc_Terminating

DESCRIPTION
    Must try to deliver all Downlink data before issuing RES.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void  
ds_ucsd_Action_L2R_RLP_Disc_Terminating 
( 
   DSI_Event_T          Event
)
{
   short status;


   switch (Event)
   {         
      case DS_UMTS_CSD_DISCONNECT_EVENT:
         NTP_State = L2RSTATE_IDLE;
         break;
         
      /* No data to flush, so respond immediately with flush complete 
      ** indication 
      */   
      case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;
   
      case DS_UMTS_CSD_NULL_EVENT:
      default:
           
         if (TIMER_EXPIRED (TIMERID_MO_FLUSH))
         {
            STOP_TIMER (TIMERID_MO_FLUSH);
            DS_UCSD_DEBUG_MSG("DISC timer expired");
            Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_TERMINATE_CALL_IND,
                                      Ds_ucsd_nt_call_id );
            NTP_State = L2RSTATE_IDLE;
         }
         
         else
         { 
            status = V42_main ();  /* Do a V42bis cycle */

            if (V42_OK == status)
            {
               if (ds_ucsd_get_rlp_process_uplink())
               {
                  status = ds_ucsd_l2r_transmit ();
               }
               if (ds_ucsd_get_rlp_process_downlink())
               {
                  status = ds_ucsd_l2r_receive ();
               }
            }

            if ((V42_ERROR == status) || (RLP_ERROR == status))
            {
               DS_UCSD_DEBUG_MSG ("L2R:MT_RLP_Disc abort due to error");
               ds_ucsd_HandleBreak(); /* Discard data */
               Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_TERMINATE_CALL_IND,
                                         Ds_ucsd_nt_call_id );
               NTP_State = L2RSTATE_IDLE;
            }
         }
         break;
   } /* end switch */
}



/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_Action_L2R_MO_Terminating

DESCRIPTION
    Must try to TX all Uplink data before terminating call.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void  
ds_ucsd_Action_L2R_MO_Terminating 
(
   DSI_Event_T          Event
)
{
   short status;
   int32 delay;
   
   if (Event == (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT)
   {
 
      /* Check to see if we have flushed out the TX buffers */
      if ((V42_terminated ()) &&
          (ds_check_empty_V42_tx_buffer ()) &&
          (ds_check_empty_remap_pending_buffer ()) &&
          (!ds_ucsd_UplinkPDUMgr_RemapDataPending ()) &&
          (ds_ucsd_UplinkPDUMgr_IsEmpty ()))
      {
         Event = L2R_FLUSH_DONE;
      }

   }

   switch (Event)
   {
      case L2R_FLUSH_DONE:
         DS_UCSD_DEBUG_MSG ("L2R:MO All Flushed!");
         STOP_TIMER         (TIMERID_MO_FLUSH);
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;

      case L2R_FLUSH_TIMEOUT:
         DS_UCSD_DEBUG_MSG ("L2R:MO_Flush T.out");
         ds_ucsd_HandleBreak();
         STOP_TIMER         (TIMERID_MO_FLUSH);
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;

      case L2R_FLUSH_ABORT:
         DS_UCSD_DEBUG_MSG ("L2R:MO_Flush abort");
         ds_ucsd_HandleBreak ();
         STOP_TIMER         (TIMERID_MO_FLUSH);
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;

      case DS_UMTS_CSD_DISCONNECT_EVENT:
         ds_ucsd_HandleBreak();
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         NTP_State = L2RSTATE_IDLE;
         break;

      /* This shouldn't happen */
      case DS_UMTS_CSD_ONLINE_CMD_MODE_EVENT:
         ds_ucsd_HandleBreak();
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                   Ds_ucsd_nt_call_id );
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         NTP_State = L2RSTATE_IDLE;
         break;
         
      /* If we get a disconnect indication from far end RLP, start
      ** a timer and enter the RLP_DISC_TERMINATING state.  This
      ** gives time for the RLP UA to be transmitted back to the
      ** far end prior to ending the call. 
      */  
      case RLP_DISC_IND:
         ds_ucsd_HandleBreak ();
         /* If ADM idle timer is zero, allow minimum time to send UA frame */
         delay = (0 == Ds_ucsd_nt_rlp_params.rlp_adm_idle_timer)?
                 DS_UCSD_NT_MIN_RLPADM_IDLE_TIME :
                 (int32)(Ds_ucsd_nt_rlp_params.rlp_adm_idle_timer *
                         TIMER_COUNT_1_SEC);
         START_TIMER ( TIMERID_MO_FLUSH, delay );
         NTP_State = L2RSTATE_RLP_DISC_TERMINATING;
         break;
       
      case RLP_ERROR_IND:
         ds_ucsd_HandleBreak ();
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;

      default:
         /* Perform a flush of the data buffers before a call termination */
       
         /* Do a V42bis cycle */
         status = V42_main ();         
         
         if (V42_OK == status)
         {
            /* If on Uplink cycle */
            if (ds_ucsd_get_rlp_process_uplink())
            {
               status = ds_ucsd_l2r_transmit ();
            }
            if (ds_ucsd_get_rlp_process_downlink())
            {
               status = ds_ucsd_l2r_receive ();
            }
         }

         if ((V42_ERROR == status) || (RLP_ERROR == status))
         {
            DS_UCSD_DEBUG_MSG ("L2R:MO_Flush abort due to error");
            ds_ucsd_HandleBreak ();
            STOP_TIMER         (TIMERID_MO_FLUSH);
            WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
            Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                      Ds_ucsd_nt_call_id );
            NTP_State = L2RSTATE_IDLE;
         }         

#ifndef FEATURE_DATA_UCSD_NOFLUSHTIMER
         /* Use timer to limit elasped time for UL flush. This is
          * necessary to avoid case where remote peer has invoked flow
          * control during link termination.  In such cases, RLP frames
          * are stuck in UL queue and flush may never complete. */
         if( TIMER_RUNNING( TIMERID_MO_FLUSH ) )
         {
           /* Check for timer expired to signal timeout */
           if( TIMER_EXPIRED( TIMERID_MO_FLUSH ) )
           {
             /* Send L2R flush timout event */
             DATA_MSG0_HIGH( "L2R MO terminate flush timer expired");
             WRITE_EVENT_TO_L2R( L2R_FLUSH_TIMEOUT );
           }
         }
         else
         {
           /* Initiate flush timer */
           START_TIMER( TIMERID_MO_FLUSH, DS_UCSD_NT_MAX_RLPL2R_FLUSH_TIME );
           DATA_MSG1_HIGH( "L2R MO terminate flush timer started: timer=%d ms",
                    DS_UCSD_NT_MAX_RLPL2R_FLUSH_TIME); 
         }
#endif /* FEATURE_DATA_UCSD_NOULFLUSHTIMER */
         
         break;
   }
}


/* <EJECT> */
/*===========================================================================

FUNCTION   ds_ucsd_Action_L2R_AwaitingBreakAck

DESCRIPTION
   This state will be used in Phase 2+.      
   It is used to wait for the Destructive Break Acknowledge   
   from the other L2R, after we have sent a BREAK.            
   At this point, the V42bis must be initialised to correctly 
   synchronise the Coder/Decoder of the V42bis.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_Action_L2R_AwaitingBreakAck 
( 
   DSI_Event_T          Event
)
{
   int32 delay;

   switch (Event)
   {
      case DS_UMTS_CSD_DISCONNECT_EVENT:
         /* A call terminate without a data flush */
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         NTP_State = L2RSTATE_IDLE;
         break;

      case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
         /* local call termination. Flush data before disconnecting */
         V42_terminate  ();
         NTP_State = L2RSTATE_MO_TERMINATING;
         break;
         
      /* If we get a disconnect indication from far end RLP, start
      ** a timer and enter the RLP_DISC_TERMINATING state.  This
      ** gives time for the RLP UA to be transmitted back to the
      ** far end prior to ending the call. 
      */  
      case RLP_DISC_IND:
         ds_ucsd_HandleBreak ();
         /* If ADM idle timer is zero, allow minimum time to send UA frame */
         delay = (0 == Ds_ucsd_nt_rlp_params.rlp_adm_idle_timer)?
                 DS_UCSD_NT_MIN_RLPADM_IDLE_TIME :
                 (int32)(Ds_ucsd_nt_rlp_params.rlp_adm_idle_timer *
                         TIMER_COUNT_1_SEC);
         START_TIMER ( TIMERID_MO_FLUSH, delay );
         NTP_State = L2RSTATE_RLP_DISC_TERMINATING;
         break;
       
      case RLP_ERROR_IND:
         ds_ucsd_HandleBreak ();
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_TERMINATE_CALL_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;
         
      case L2R_BREAK_ACK_IND:
         DiscardRxData = FALSE;
         NTP_State     = L2RSTATE_CONNECTED;
         break;
         

      default:

         if (ds_ucsd_get_rlp_process_uplink())
         {
            (void)ds_ucsd_l2r_transmit ();
         }
         if (ds_ucsd_get_rlp_process_downlink())
         {
            (void)ds_ucsd_l2r_receive ();
         }
         break;
   }
}



/* <EJECT> */
/*===========================================================================

FUNCTION    ds_ucsd_Action_L2R_Remapping

DESCRIPTION
    Action routine for remapping state
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_Action_L2R_Remapping 
(
   DSI_Event_T          Event
)
{
   int32 delay;
  
   switch (Event)
   {
      case RLP_REMAP_CNF:
         NTP_State = L2RSTATE_CONNECTED;
         break;


      /* If we get a disconnect indication from far end RLP, start
      ** a timer and enter the RLP_DISC_TERMINATING state.  This
      ** gives time for the RLP UA to be transmitted back to the
      ** far end prior to ending the call. 
      */  
      case RLP_DISC_IND:
         ds_ucsd_HandleBreak ();
         /* If ADM idle timer is zero, allow minimum time to send UA frame */
         delay = (0 == Ds_ucsd_nt_rlp_params.rlp_adm_idle_timer)?
                 DS_UCSD_NT_MIN_RLPADM_IDLE_TIME :
                 (int32)(Ds_ucsd_nt_rlp_params.rlp_adm_idle_timer *
                         TIMER_COUNT_1_SEC);
         START_TIMER ( TIMERID_MO_FLUSH, delay );
         NTP_State = L2RSTATE_RLP_DISC_TERMINATING;
         break;
       
      case RLP_ERROR_IND:
         ds_ucsd_HandleBreak ();
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_TERMINATE_CALL_IND,
                                   Ds_ucsd_nt_call_id );
         NTP_State = L2RSTATE_IDLE;
         break;

      case DS_UMTS_CSD_DISCONNECT_EVENT:
         /* A call terminate without a data flush */
         WRITE_EVENT_TO_RLP (RLP_DISC_REQ);
         NTP_State = L2RSTATE_IDLE;
         break;

      case DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT:
         /* local call termination. Flush data before disconnecting */
         V42_terminate  ();
         NTP_State = L2RSTATE_MO_TERMINATING;
         break;
         
      default:
         break;  /* Making Lint happy */
   }
}



/* <EJECT> */
/*===========================================================================

FUNCTION  ds_control_non_transparent_data_services

DESCRIPTION
   This function is used to drive the Non-Transparent data processing
   and state machine.  It should be called only after the above initialization
   functions are called.  This function will be called each time one
   of the relevent events is to be processed and/or for each transmit and
   receive TTI.  Each TTI time, the process_tx or process_rx_data 
   parameters must be set.  Although in theory, both the process_tx_data and
   process_rx_data can both be set when calling this function, this has not 
   been tested in the Qualcomm GSM environment.
         
DEPENDENCIES
  ds_ucsd_nt_register_ota_funcs(), ds_ucsd_nt_register_sio_funcs() and
  ds_ucsd_nt_init_call_data() must be called first
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Lots.

===========================================================================*/
void 
ds_ucsd_nt_process_data
(                                                 
   ds_ucsd_event_T input_event,
   uint8           call_id,                                               
   boolean         process_tx_data,
   boolean         process_rx_data
)
{
   /* Locals */
   register uint32 process_tx_data_cnt = 0;
   static uint8 traffic_report_cnt = GCSD_TRAFFIC_REPORTING_INTERVAL;

   /* To help performance */
   register uint32 local_process_rx_data = (uint32) process_rx_data;
   register uint32 local_process_tx_data = (uint32) process_tx_data;
                   
   /* Convert input_event to uint32 since it must also be used
   ** to contain the interal L2R and RLP events
   */
   register DSI_Event_T event = (DSI_Event_T)input_event;
   

   /* If this function is called before the NT stack has been 
   ** initialized, then just return without doing anything.
   */
   if (!Ds_ucsd_nt_init_call_data_called)
   {
      /* It's possible that we will get a disconnect_after_flush event 
      ** before we have received the call initialization data.  (This
      ** will happen in the UE when we get MO call aborted).  If we get this,
      ** respond to it if the Ds_ucsd_nt_ind_report_cb has been registered
      */
      if (event == (DSI_Event_T)DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT)
      {
         if (Ds_ucsd_nt_ind_report_cb)
         {
           /* Use passed call_id as stack has not been initialized yet. */
           Ds_ucsd_nt_ind_report_cb( DS_UMTS_CSD_DISCONNECT_COMPLETE_IND,
                                     call_id );
         }
         else
         {
            DATA_MSG0_ERROR("UCSD NT: Not able to respond to DISC_AFTER_FLUSH");
         }
      }
        
      /* MSG_ERROR if we get an event before stack is initialized. 
      ** We will get the DISCONNECT event in the 
      ** case where the call is terminated by the stack, so don't 
      ** bother printing out MSG_ERROR for this case. 
      */
      else if ( (event != (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT) &&
                (event != (DSI_Event_T)DS_UMTS_CSD_DISCONNECT_EVENT) )
      {
         DATA_MSG1_ERROR( "UCSD NT: Event %d arrived before NT init - ignoring",
                    event);
      }

      // Call initialization has not happened, so go no further.
      return;
   }

   /* We can't handle doing both uplink and downlink at the same time. */
   if (local_process_tx_data && local_process_rx_data)
   {
      DS_3GPP_MSG0_ERROR("UCSD NT: Cannot process both TX and RX data");
   }

   /* If we are starting up a call, initialize the NT data. */
   if (event == (DSI_Event_T)DS_UMTS_CSD_CONNECT_EVENT)
   {
      /* Initialize NT data */
      ds_ucsd_non_transparent_data_services_initialise(FALSE);
   }
   /* If we have no input event to handle, get an L2R event to process */
   else if (event == (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT)
   {
      event = READ_EVENT_TO_L2R();
   }

   /* If we are processing TX data, initialize the state variables
   ** associated with TX processing
   */
   if (local_process_tx_data)
   {
      process_tx_data_cnt = Ds_ucsd_nt_sdus_per_tti;
      Ds_ucsd_nt_tx_frame_sent = FALSE;
      Ds_ucsd_frame_suppressed = FALSE;
      ds_ucsd_set_rlp_process_uplink(TRUE);
      ds_ucsd_set_rlp_process_downlink(FALSE);
   }
   else /* doing rx */
   {
      ds_ucsd_set_rlp_process_uplink(FALSE);
      ds_ucsd_set_rlp_process_downlink(TRUE);
   }
   
   /* Initialize the Do_L2R_RLP state... this will only ever get
   ** varied for GSM 14.4 uplink processing
   */      
   Do_L2R_RLP = TRUE;
   
     
#ifdef DS_UCSD_DEBUG

   if (tick++ >600)
   {
      DS_UCSD_DEBUG_MSG_2 ("NT Num Data Bytes :Rx=%d Tx=%d", num_chars_rxd, num_chars_txd);
      DS_UCSD_DEBUG_MSG_2 ("NT Num SIO Bytes :Rx=%d Tx=%d", ds_ucsd_nt_sio_rx_bytes, ds_ucsd_nt_sio_tx_bytes);
      tick = 0;
   }
#endif

   /* Loop over all RX frames OR for allowed number of TX frames OR specified event */
   while ( (local_process_rx_data) ||
           ( (process_tx_data_cnt) && (!Ds_ucsd_frame_suppressed) ) ||
           (event != (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT) ) 
   {
      /* The ds-isr is called twice in each 20ms cycle: once for uplink and
         once for downlink. This is to reduce the time taken in the ISR. */
      /* local copy of ds_process_uplink */ 
      if (process_tx_data_cnt)
      {
         process_tx_data_cnt--;

         /* Only perform a L2R/RLP cycle every other cycle for 4800 and 14400 */
         if (Ds_ucsd_nt_bytes_per_sdu == DS_UMTS_CSD_FRAME_SIZE_288_BIT)
         {
            if (!Ds_ucsd_nt_perform_rlp_uplink)
            {
               Do_L2R_RLP = FALSE;
            }
            Ds_ucsd_nt_perform_rlp_uplink ^= 0x01;
         }
      }
      
      
      /* If we get the process_rx_data indication, base actually processing
      ** the downlink message on whether or not we successfully are able to 
      ** read a downlink frame.
      */
      if (local_process_rx_data) 
      {
         
         /* Attempt to read an RX frame.  When this returns false,
         ** it will break us out of the enclosing while loop.  Note
         ** that this is a change, and we now read all RX frames, not
         ** just the number per TTI
         */ 
         local_process_rx_data = ds_HardwareRLPRx();

         /* If frame was successfully read or there is data pending in
          * the Rx V.42 compression buffer, process it. Otherwise, skip
          * DL processing. */
         if (! (local_process_rx_data ||
               ( (V42_NO_COMPRESSION != Compression) &&
                 (0 < ds_check_V42_rx_buffer())) ) )
         {
           ds_ucsd_set_rlp_process_downlink(FALSE);
         }
      }


      /* Query V24 state from hardware */
      if (local_process_tx_data)
      {
         (void)Ds_ucsd_nt_get_sio_status_cb (&Ds_ucsd_nt_curr_modem_ctl);
      }
      

      /* Do an RX RLP cycle. We will process all Rx frames. */
      if (ds_ucsd_get_rlp_process_downlink())
      {
         rlp_main ();

         /* If there was no previous event to process, see
         ** if downlink RLP processing generated and event
         */
         if (event == (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT)
         {
            event = READ_EVENT_TO_L2R();
         }
      }


      /* State machine processing */
      switch (NTP_State)
      {
         case L2RSTATE_IDLE:           
            ds_ucsd_Action_L2R_Idle (&Ds_ucsd_nt_rlp_params,event); 
            break;

         case L2RSTATE_CONNECTING:     
            ds_ucsd_Action_L2R_Connecting (&Ds_ucsd_nt_rlp_params,event); 
            break;

         case L2RSTATE_NEGOTIATING_XID:
            ds_ucsd_Action_L2R_NegotiatingXID (&Ds_ucsd_nt_rlp_params,event); 
            break;

         case L2RSTATE_WAIT_FOR_DCD_ON:
            ds_ucsd_Action_L2R_Waiting_For_DCD_On (event); 
            break;

         case L2RSTATE_CONNECTED:      
            ds_ucsd_Action_L2R_Connected (event); 
            break;

         case L2RSTATE_RLP_DISC_TERMINATING: 
            ds_ucsd_Action_L2R_RLP_Disc_Terminating (event); 
            break;

         case L2RSTATE_MO_TERMINATING: 
            ds_ucsd_Action_L2R_MO_Terminating (event);
            break;

         case L2RSTATE_AWAITING_BREAK_ACK:
            ds_ucsd_Action_L2R_AwaitingBreakAck (event);
            break;

         case L2RSTATE_REMAPPING:      
            ds_ucsd_Action_L2R_Remapping (event); 
            break;
            
         default:   
            NTP_State = L2RSTATE_IDLE;         
            break;
      }
      
            
      /* Do RLP processing of uplink data if necessary and we are not
      ** in idle state
      */
      if (ds_ucsd_get_rlp_process_uplink() && Do_L2R_RLP && (NTP_State != L2RSTATE_IDLE))
      {
         rlp_main ();
      }

      /* Tx to the Radio Interface */
      if ( local_process_tx_data ) //&& (RlpCurrentState != RLPSTATE_ADM_DETACHED) )
      {  
         ds_HardwareRLPTx ();
      }

      /* If the received CTS state has changed, call the CTS state
      ** setting callback.
      */
      if (Ds_ucsd_nt_saved_filtered_rx_status.X_status_rx !=
          Ds_ucsd_nt_filtered_rx_status.X_status_rx)
      {
         if (Ds_ucsd_nt_set_sio_cts_state_cb != NULL)
         {
            Ds_ucsd_nt_set_sio_cts_state_cb (
                (Ds_ucsd_nt_filtered_rx_status.X_status_rx == V24_CONTROL_ON) 
                                                              ? TRUE : FALSE);

                                                   
         }
         
         Ds_ucsd_nt_saved_filtered_rx_status = Ds_ucsd_nt_filtered_rx_status;         
      }
      
      /* See if there are more events to process */
      event = READ_EVENT_TO_L2R();
         
   } /* end while */

   /* Debug output if state change occurs   */
   if ( (NTP_State != Old_NTP_State) ||
        (event != (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT) )  /*lint !e774 */
   {
      DS_UCSD_DEBUG_MSG_3 ("Start L2R_St=%d, End L2R_St=%d, Evt=%d",
                                         Old_NTP_State, NTP_State,event);
   }

   /* and
   ** re-initialize the SIO interface if we transition back to the 
   ** IDLE state from a non-idle state
   */  
   if (NTP_State != Old_NTP_State)
   {
      /* If we transition to IDLE state from a non-idle state */
      if (NTP_State == L2RSTATE_IDLE)
      {
         /* Cleanup PDU Mgrs storage */
         ds_ucsd_nt_pdu_cleanup ( );
         
         /* Initialize NT data */
         ds_ucsd_non_transparent_data_services_initialise(TRUE);

      }
      
      /* Set old state to current state */
      Old_NTP_State = NTP_State;
   }

   /* Update call instance data traffic counters at set interval */
   if( (Ds_gcsd_set_data_counters_cb) &&
       ( !traffic_report_cnt-- ) )
   {
     if( !Ds_gcsd_set_data_counters_cb( Ds_ucsd_nt_call_id,
                                        FALSE,
                                        Ds_ucsd_nt_rx_data_count,
                                        Ds_ucsd_nt_tx_data_count ) )
     {
       DATA_MSG1_ERROR("Problem updating data traffic counters: call ID=%d",
                  Ds_ucsd_nt_call_id);
     }
     traffic_report_cnt = GCSD_TRAFFIC_REPORTING_INTERVAL;
   }
   
   /* Now call the no tx frames sent callback if 
   ** duplicate frame suppression is on, we
   ** are processing tx data, no frames were sent,
   ** and the no frames sent callback has been registered.
   ** Note:  Ds_ucsd_nt_suppress_duplicate_tx_frames will
   ** be turned off when 
   ** ds_ucsd_non_transparent_data_services_initialise(TRUE);
   ** is called, as this sets
   ** Ds_ucsd_nt_suppress_duplicate_tx_frames to FALSE
   */
   if ((Ds_ucsd_nt_suppress_duplicate_tx_frames) &&
       (local_process_tx_data) && 
       (!Ds_ucsd_nt_tx_frame_sent) &&
       (Ds_ucsd_nt_no_tx_frames_sent_cb))
   {
      Ds_ucsd_nt_no_tx_frames_sent_cb();
   }
}

/*===========================================================================

FUNCTION ds_ucsd_nt_get_Ds_FillPattern

DESCRIPTION
  This function is used to access the Ds_FillPattern array

DEPENDENCIES
  None

RETURN VALUE
  uint8 *

SIDE EFFECTS
  None

===========================================================================*/ 
uint8 * ds_ucsd_nt_get_Ds_FillPattern(void)
{
  return Ds_FillPattern;
}

/*===========================================================================

FUNCTION ds_ucsd_nt_get_Evq_To_L2R

DESCRIPTION
  This function is used to access Evq_To_L2R

DEPENDENCIES
  None

RETURN VALUE
  DSISR_EventQueue_T *

SIDE EFFECTS
  None

===========================================================================*/
DSISR_EventQueue_T * ds_ucsd_nt_get_Evq_To_L2R(void)
{
  return &Evq_To_L2R;
}

/*===========================================================================

FUNCTION ds_ucsd_nt_get_Evq_To_RLP

DESCRIPTION
  This function is used to access Evq_To_RLP

DEPENDENCIES
  None

RETURN VALUE
  DSISR_EventQueue_T *

SIDE EFFECTS
  None

===========================================================================*/
DSISR_EventQueue_T * ds_ucsd_nt_get_Evq_To_RLP(void)
{
  return &Evq_To_RLP;
}

/*===========================================================================

FUNCTION ds_ucsd_nt_get_curr_mode

DESCRIPTION
  This function is used to access Ds_ucsd_nt_curr_mode

DEPENDENCIES
  None

RETURN VALUE
  sys_sys_mode_e_type

SIDE EFFECTS
  None

===========================================================================*/
sys_sys_mode_e_type ds_ucsd_nt_get_curr_mode(void)
{
  return Ds_ucsd_nt_curr_mode;
}

/*===========================================================================

FUNCTION ds_ucsd_nt_get_bytes_per_sdu

DESCRIPTION
  This function is used to access Ds_ucsd_nt_bytes_per_sdu

DEPENDENCIES
  None

RETURN VALUE
  ds_ucsd_frame_size_T

SIDE EFFECTS
  None

===========================================================================*/
ds_ucsd_frame_size_T ds_ucsd_nt_get_bytes_per_sdu(void)
{
  return Ds_ucsd_nt_bytes_per_sdu;
}

/*===========================================================================

FUNCTION ds_ucsd_nt_get_data_rate

DESCRIPTION
  This function is used to access Ds_ucsd_nt_data_rate

DEPENDENCIES
  None

RETURN VALUE
  ds_ucsd_data_rate_T

SIDE EFFECTS
  None

===========================================================================*/
ds_ucsd_data_rate_T ds_ucsd_nt_get_data_rate(void)
{
  return Ds_ucsd_nt_data_rate;
}

/*===========================================================================

FUNCTION ds_ucsd_nt_get_suppress_duplicate_tx_frames

DESCRIPTION
  This function is used to access Ds_ucsd_nt_suppress_duplicate_tx_frames

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_nt_get_suppress_duplicate_tx_frames(void)
{
  return Ds_ucsd_nt_suppress_duplicate_tx_frames;
}

/*===========================================================================

FUNCTION ds_ucsd_nt_set_tx_frame_sent

DESCRIPTION
  This function is used to set Ds_ucsd_nt_tx_frame_sent

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_nt_set_tx_frame_sent
(
  boolean value
)
{
  Ds_ucsd_nt_tx_frame_sent = value;
}

/*===========================================================================

FUNCTION ds_ucsd_nt_set_frame_suppressed

DESCRIPTION
  This function is used to set Ds_ucsd_frame_suppressed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_nt_set_frame_suppressed
(
  boolean value
)
{
  Ds_ucsd_frame_suppressed = value;
}

/*===========================================================================

FUNCTION ds_ucsd_nt_get_DSM_Level_Low

DESCRIPTION
  This function is used to get DSM_Level_Low

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_nt_get_DSM_Level_Low(void)
{
  return DSM_Level_Low;
}

/*===========================================================================

FUNCTION ds_ucsd_nt_set_DSM_Level_Low

DESCRIPTION
  This function is used to set DSM_Level_Low

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_nt_set_DSM_Level_Low(boolean value)
{
  DSM_Level_Low = value;
}

/*===========================================================================

FUNCTION ds_ucsd_nt_get_rlp_P0

DESCRIPTION
  This function is used to get Ds_ucsd_nt_rlp P0 param

DEPENDENCIES
  None

RETURN VALUE
  byte

SIDE EFFECTS
  None

===========================================================================*/
byte ds_ucsd_nt_get_rlp_P0(void)
{
  return Ds_ucsd_nt_rlp_params.rlp_P0;
}

/*===========================================================================

FUNCTION ds_ucsd_nt_get_cb_info

DESCRIPTION
  This function is used to access ds_ucsd_nt_cb_info

DEPENDENCIES
  None

RETURN VALUE
  ds_ucsd_nt_cb_type *

SIDE EFFECTS
  None

===========================================================================*/
ds_ucsd_nt_cb_type * ds_ucsd_nt_get_cb_info(void)
{
  return &ds_ucsd_nt_cb_info;
}

#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */
