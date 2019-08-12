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

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdrlp.c_v   1.5   25 Jun 2002 13:12:00   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdrlp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/27/11   ND      Fixed compiler warnings.
04/05/11   SS      Q6 free floating support.
02/16/11   TTV     Fixed linker errors.
02/02/11   SS      Q6 free floating changes.
07/09/09   BS      Fixed offtarget lint medium and low's.
04/06/09   SS      Fixed Off Target Lint Errors
23/02/07   DA      Fixed high lint errors
02/09/07   DA      Fixed critical lint errors
05/05/06   AR      Incorporate changes from BoaApps interRAT effort.
02/09/06   AR      Lint corrections.
10/19/04   AR      Use rlp_adm_idle timer to terminate call after abnormal
                   event and entering ADM attached state.  Lint cleanup.
08/18/04   AR      Optimize NULL frame and remote ready logical tests.
                   Lint cleanup.
08/11/04   AR      Ensure first XID packet set on contend condition. Allow 
                   subset of parameters in received XID command.
07/14/04   AR      Correct payload handling of TEST frame.  Ensure SABM and 
                   DISC frames are not blocked by DRX mode.
                   Relocate frame handling macros to header file.
06/07/04   AR      Toggle use for SREJ based on NV item.
05/17/04   AR      Ensure first I-frame is sent with L2R status informaiton.
                   Add integrity check for XID packet.
05/10/04   AR      Relocate PDU_SIZE macro to internal header file.
03/24/04   AR      Make queued PDU Tx dependent on remote flow control.
02/25/04   AR      Fix sending spurious NULL and S frames; lint corrections.
                   Lint fixes.
12/10/03   TMR     Added check in ReadRLPFrame to make sure that N(R) is 
                   a valid number
11/18/03   TMR     Removed unused 'status' local in ActionADMDetached, and
                   added RLPReadFrame to ActionPendingConnectionResponse 
11/18/03   AR      Remove Rx response block in ADM detached handler.
09/18/03   AR      Removed obsolete code block.
09/18/03   TMR     Addes support for TX frame suppression
07/15/03   TMR     Added support for RLP Version 2
04/21/03   AR      Add support for using DSM items inplace of static buffers
                   Lint corrections
01/20/03   TMR     Added check to make sure remote receiver is ready before
                   transmitting SREJ data (line 2160).
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/




/******************************************************************************
 *
 *  File:         /home/briers/gsmqc/ds/SCCS/s.m51300.c
 *  -----
 *
 *  SCCS Version: 2.17
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
 *  Description:    RLP Main Module
 *  ------------
 *
 *  Functions Used:
 *  ---------------
 *
 *  Modification History:
 *  ---------------------
 *  Date       Vers  Auth   Details
 *  ----       ----  ----   -------
 *  11-Jan-99  2.1   RCB    Original version
 *  08-Jun-99  2.2   RCB    Added Debug
 *  11-Jun-99  2.3   RCB    Send RNR during a recovery
 *                          Only respond to SREJ if rxlist state=IDLE
 *                          Check Rxlist=IDLE before writing 
 *  17-Jun-99  2.4   RCB    Ensure RR sent again when I frames stop
 *  28-Jun-99  2.5   RCB    Various changes for Type Approval:
 *                          Corrections to XID negotiation
 *                          Ignore F=0 responses in checkpoint recovery
 *                          Poll retry logic changed for 11.10
 *  01-Jul-99  2.6   RCB    Minimise debug
 *  05-Jul-99  2.7   RCB    Dont send DM responses - it prevents the RACAL 
 *                          retrying to connect
 *  25-Oct-99  2.8   RCB    Code optimizing: replace DataToTransmit,
 *                          Inc/DecSeqNum,Start/StopTimer/Expired,
 *                          Frame Handling functions,  with MACROs
 *                          Better logic for sending data 
 *                          Remove check for PollResponse=F when StoppingTimer
 *                          Removed SendRNRDuringRecovery
 *                          Only send REJs when suitable to do so.
 *  27-Oct-99  2.9   RCB    Remove Frame-losing for test purposes
 *  27-Oct-99  2.10  RCB    Change VS_at_last_poll to VS_at_first_poll
 *                          ie. Ensure PollCount not reset unitl whole 
 *                          sequence is acknowledged.
 *  28-Oct-99  2.11  RCB    Don't error-check XID for Testharness only
 *                          Correct Debug output for TxXID
 *  12-Jun-00  2.12  RCB    Major restructure for addition of Phase2+ features
 *  26-Jul-00  2.13  RCB    Update file header
 *  26-Jul-00  2.14  RCB    Update file header
 *  17-Aug-00  2.15  SFL    Remove shg_ghdi include
 *  17-Aug-00  2.16  RCB    Mods to use ghdi for serial/modemlines/radio i.faces
 *  29-Aug-00  2.17  RCB    Terminate on compression negotiation if AT+DS=x,1,x,x
 *                          Update debug.
 *
 *****************************************************************************/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS)

#include "dsucsdnti.h"
#include "dsucsdrlp.h"
#include "dsucsdlog.h"
#include "memory.h"
#include "amssassert.h"
#include "data_msg.h"


/****************************************************************************
 * These are global variables used throughout RLP related files
 ***************************************************************************/
static boolean Rlp_process_uplink = FALSE;   /* local copy of ds_process_uplink */
static boolean Rlp_process_downlink = FALSE; /* local copy of ds_process_downlink */

static boolean RLP_ErrorHandling = FALSE;  /* Error handling after abnormal event */
static XID_Params_T    Rlp_CurrentXID = {0};

/****************************************************************************
 * These are the variables with scope within the RLP module
 ***************************************************************************/

/* General Variables */
/* ----------------- */
/* Latest Receive RLP frame */
static RLP_Frame_T        RxFrameBuffer = {0, NULL, {0}};
static RLP_Frame_T*       RxFrame = &RxFrameBuffer;


static RLP_Frame_T        TxFrame = {0, NULL, {0}};;           /* Next Tx RLP frame */
static short              RlpCurrentState = 0;   /* Current state of the RLP FSM */

static boolean     RecoveryUsesSREJ = FALSE;
static boolean     PolledDueToSREJFailure = FALSE;

#ifdef ENHANCED_POLL_RECOVERY
static boolean     AcksInSequence = FALSE;
#endif
static short       LastPollNr = 0;
static boolean     RepeatedPollResponse = FALSE;
static short       VS_at_first_poll = 0;   /* This is the mechanism for ensuring
                                         that the poll count is NOT reset
                                         until an entire sequence is ACK'd
                                         (Test 11.10 29.3.2.6.9 */

/* This flag is necessary to stop the remote end sending frames during
   an error recovery. The Racal 6103 test set continues to send frames
   regardless! Even if we send RNRs, the Racal sends I frames for a while
   before stopping! */
/*static boolean     SendRNRDuringRecovery;    Added in v2.3 */

#ifdef DS_UCSD_DEBUG
static short     OldRlpState = -1;     /* Used for debug log */
#endif



/* These are the various data buffers */
/* ---------------------------------- */
/* UI frame data buffers */
static dsm_item_type *   RxUIData = NULL;
static dsm_item_type *   TxUIData = NULL;

/* XID frame data buffers */
static dsm_item_type *   TxXIDData = NULL;
static XID_Params_T  NewXID;
#define MAX_RLP_XID_SIZE 22
static word          XIDsize = MAX_RLP_XID_SIZE;


/* TEST frame data buffers */
static dsm_item_type *   Test_C_Data = NULL;
static dsm_item_type *   Test_R_Data = NULL;
#define TEST_DATA  ("TheQuickBrownFoxJumpsOverTheLazyDog")

static short T1ValueInMs = 0; /* T1 value in millisecs */


/* These variables control the operation of the RLP mechanism.  The names   *
 * The names of these variables follow closely the model given in the ETS   *
 * Specification:  Radio Link Protocol (RLP) - GSM 04.22.                   */
/* ------------------------------------------------------------------------ */
static boolean Ackn_FBit = FALSE;     /* Value of Fbit for next Ack frame           */

static short   DISC_Count = 0;    /* Retry count for DISC frames sent           */
static boolean DISC_PBit = FALSE;     /* Value of Pbit in next DISC frame           */
static byte    DISC_State = 0;    /* State of DISC exchange                     */
static boolean DM_FBit = FALSE;       /* Value of Fbit to be used in next DM resp.  */
static byte    DM_State = 0;      /* State of DM exchange                       */ 

static short   Poll_Count = 0;    /* Retry count of poll requests               */

static byte    Poll_State = 0;    /* State of Poll Exchange (numbered frames):  */
                              /* SEND: a S frame with P=1 must be sent      */
                              /* (this occurs in response to a timeout when */
                              /* connected (checkpointing))                 */
                              /* WAIT: RLP is waiting for resp. with F=1    */

static byte    Poll_xchg = 0;     /* State of existing Poll exchange.           */
                              /* IDLE: send of P=1 is allowed               */
                              /* WAIT: ack of a previous Pbit is outstanding*/
                              /* (2 flags needed as U frames can start a    */
                              /* poll exchange as well as numbered ones, and*/
                              /* only one Poll exchange is allowed at       */
                              /* any time in each direction)                */

static byte    REJ_State = 0;     /* State of a REJ exchange                    */
static boolean remote_receiver_ready = FALSE; /* RemoteReceiverReady Flag           */

static short   SABM_Count = 0;    /* Retry count for SABM command               */
static byte    SABM_State = 0;    /* State of SABM command exchange             */

static byte    Test_C_State = 0;  /* State of Test Command exchange             */
static byte    Test_R_State = 0;  /* State of Test Response exchange            */
static byte    Test_R_FBit = 0;   /* State of Test Response exchange            */
static boolean Test_C_PBit = FALSE;
static short   Test_Count = 0;

static boolean UA_FBit = FALSE;       /* Value of Fbit in next UA frame             */
static byte    UA_State = 0;      /* State of UA exchange                       */
static byte    UI_State = 0;      /* State of UI exchange                       */

static short   XID_Count = 0;     /* Retries of XID command */
static short   XID_C_State = 0;   /* Current XID Command state */ 
static short   XID_R_State = 0;   /* Current XID Response state */ 
static boolean Negotiate_XID_ADM_Values = FALSE; 

static ds_ucsd_nt_rlp_parameters_T* current_RLP_params = NULL;

/* Whether or not we have sent or received an INFO frame yet (L2RCOP status) */
static uint32  RLP_Tx_Info_Frame = 0;
static uint32  RLP_Rx_Info_Frame = 0;

/* These variables reflect the state of the latest received packet */
/* --------------------------------------------------------------- */
static boolean  CR = FALSE;              /* Stores the last Command/Response bit    */
static boolean  PF = FALSE;              /* Stores the last Poll/Final bit          */
static short    Nr = 0;              /* Stores the last Nr                      */
static short    Ns = 0;              /* Stores the last Ns                      */
static short    SF = 0;              /* Stores the last supervisory frame type  */
static short    UnNumberedFrame = 0; /* Frame type of last UnNumbered Frame     */
static short    FrameType = 0;       /* UnNumbered, Supervisory or Information  */

/* These are used for duplicate TX frame suppression */
static word       Last_tx_cmd = 0;
static short      Last_tx_frame_type = 0;
static boolean    Last_tx_cr = FALSE;
static boolean    Last_tx_pf = FALSE;
static word       Last_tx_nr = 0;

/* Remap control variables */
#define INVALID_REMAP_NR (-99)
static short TxRemapNr = 0;
static short Remap_State = 0;
static boolean UplinkRemapped = FALSE;
static short   UplinkRemappedLastNr = 0;


/* These variables reflect the latest RLP version information */

static boolean Rlp_current_version_is_2 = FALSE;
static uint16  Rlp_current_PDU_payload_size = 0;
static uint16  Rlp_current_header_size = 0;
static int16   Rlp_current_max_window_size = 0;
static int16   Rlp_current_max_frame_number = 0;

/****************************************************************************
 * Function Prototypes:
 ****************************************************************************/
static void ActionADMDetached              (DSI_Event_T Event);
static void ActionADMAttached              (DSI_Event_T Event);
static void ActionPendingConnectionRequest (DSI_Event_T Event);
static void ActionPendingConnectionResponse(DSI_Event_T Event);
static void ActionConnectionEstablished    (DSI_Event_T Event);
static void ActionDiscInitiated            (DSI_Event_T Event);
static void ActionPendingResetRequest      (DSI_Event_T Event);
static void ActionPendingResetIndication   (DSI_Event_T Event);

static void InitialiseLinkVariables (void);
static short SendData                (void);
static short SendCommand             (void);
static short SendResponse            (void);
static short Send_REJ_or_REJ_I_Cmd   (void);
static short Send_SREJ_or_SREJ_I_Cmd (short SREJ_Nr);
static void I_handler               (void);
static void S_handler               (void);
static void Advance_VA_to_NR        (void);

static boolean   SupervisoryNrIsLegal    (void);

static short     SendNumberedFrame       (word Frame, 
                                          boolean cr, boolean pf,
                                          word    nr, word    ns,
                                          dsm_item_type * PDU);

static boolean PDU_to_XID           (dsm_item_type*, XID_Params_T*);
static void HandleXID               (const RLP_Frame_T* Frame);
static void CheckXIDTimeout         (void);
static void CheckTESTTimeout        (void);

/* Functions for Phase2+ */
static short GoTo_MS_Remapping           (void);
void ActionMSRemapping              (DSI_Event_T Event);


void DebugXID (const XID_Params_T* Xid, word msg);


/******************************************************************
* PDU Related data
******************************************************************/

/* Current Type of PDU. Small for <=9600, Large for 14.4bps */
static PDU_Type CurrentPDUType;




/****************************************************************************
 *
 * These Functions are the interface functions for the higher layer
 * 
 ****************************************************************************/

/****************************************************************************
 * Function name:  rlp_initialise
 * -------------
 * Description:    This routine initialise the RLP link
 * -----------     
 * Parameters:     ptr to RLP_params
 * ----------
 * Returns:        None
 * -------         
 ****************************************************************************/
void rlp_initialise (ds_ucsd_nt_rlp_parameters_T* RLP_params)
{
   /* Point to the current rlp params in the shared memory */
   if (RLP_params)
   {
     current_RLP_params = RLP_params;
     RecoveryUsesSREJ = RLP_params->rlp_uses_srej;
   }

   RlpCurrentState = RLPSTATE_ADM_DETACHED;
#ifdef DS_UCSD_DEBUG
   OldRlpState = -1;     /* Used for debug log */
#endif

   RxUIData   = NULL;
   TxUIData   = NULL;
 
   Ackn_FBit  = FALSE;
   DISC_Count = 0;
   DISC_PBit  = FALSE;
   DISC_State = STATE_IDLE;
   DM_FBit    = FALSE;
   DM_State   = STATE_IDLE;
   Poll_Count = 0;
   Poll_State = STATE_IDLE;
   Poll_xchg  = STATE_IDLE;
   REJ_State  = STATE_IDLE;
   
   SABM_Count = 0;
   SABM_State = STATE_IDLE;
   
   Test_C_State = STATE_IDLE;
   Test_R_State = STATE_IDLE;
   Test_Count   = 0;
   Test_C_PBit  = FALSE;
   Test_R_FBit  = FALSE;
   Test_C_Data  = NULL;
   Test_R_Data  = NULL;
 
   UA_FBit     = FALSE;
   UA_State    = STATE_IDLE;
   UI_State    = STATE_IDLE;

   XID_Count   = 0;
   XID_C_State = STATE_IDLE;
   XID_R_State = STATE_IDLE;
   TxXIDData   = NULL;
   
   remote_receiver_ready = TRUE;
   
   /* Start out RLP header size at 2 till we get XID */
   Rlp_current_header_size = RLP_HDR_SIZE_VER_0_1;
   Rlp_current_version_is_2 = FALSE;
   Rlp_current_PDU_payload_size = (int16) PDU_PAYLOAD_SIZE() - 5; //3 for FCS, 2 for header
   
   
   memset (&TxFrame, 0x00, sizeof(TxFrame));
   memset (&RxFrameBuffer, 0x00, sizeof(RxFrameBuffer));
   RxFrame = &RxFrameBuffer;
   
   /* Setup the default parameters for the link */
   Rlp_CurrentXID.RLPVersion    = XIDDEFAULT_VERSION;
   Rlp_CurrentXID.IWFtoMSWindow = (uint16) XIDDEFAULT_IWFtoMSWindow;
   Rlp_CurrentXID.MStoIWFWindow = (uint16) XIDDEFAULT_MStoIWFWindow;
   Rlp_CurrentXID.T1            = XIDDEFAULT_T1;
   Rlp_CurrentXID.N2            = XIDDEFAULT_N2;
   Rlp_CurrentXID.T2            = XIDDEFAULT_T2;
   
   Rlp_CurrentXID.Pt = XIDDEFAULT_Pt; /* Version>=1 only */
   Rlp_CurrentXID.P0 = XIDDEFAULT_P0;
   Rlp_CurrentXID.P1 = XIDDEFAULT_P1;
   Rlp_CurrentXID.P2 = XIDDEFAULT_P2;
   Rlp_CurrentXID.T4 = XIDDEFAULT_T4; /* Version>=2 only */

   T1ValueInMs = XIDDEFAULT_T1 * 10;
   NewXID = Rlp_CurrentXID;  /*cetecom!!*/
   Negotiate_XID_ADM_Values = TRUE; 
   ds_ucsd_set_rlp_error_handling(FALSE);

   PolledDueToSREJFailure = FALSE;

#ifdef ENHANCED_POLL_RECOVERY
   AcksInSequence = TRUE;
#endif
   /*SendRNRDuringRecovery = FALSE;*/
   VS_at_first_poll = 0; 

   Remap_State = STATE_IDLE;
   UplinkRemapped = FALSE;
   UplinkRemappedLastNr = INVALID_REMAP_NR;
   
   /* Indicate first I-frame with L2R status Rx'd/Tx'd */
   RLP_Rx_Info_Frame = FALSE;
   RLP_Tx_Info_Frame = FALSE;
   
   /* Initialize the last tx frame type to be none */
   Last_tx_frame_type = FRAMETYPE_NONE; 
}


/****************************************************************************
 * Function name:  rlp_read_UI_data
 * -------------
 * Description:    This routine copies the latest rx'd UI data into
 * -----------     the callers buffer.
 * Parameters:     ptr to callers buffer
 * ----------
 * Returns:        RLP_OK:Data returned   RLP_ERROR:No UI Data to read
 * -------         
 ****************************************************************************/
short rlp_read_UI_data (byte* Data)
{
   uint16 cnt;
   
   if (RLP_CURRENT_PDU_SIZE !=
       (cnt = dsm_pullup(&RxUIData, (byte*)Data, RLP_CURRENT_PDU_SIZE)))
   {
      DATA_MSG2_HIGH("rlp_read_UI_data read problem: %d!=%d",
                RLP_CURRENT_PDU_SIZE, cnt);
      return RLP_ERROR;
   }
   return RLP_OK;
}

/****************************************************************************
 * Function name:  rlp_send_UI_data
 * -------------
 * Description:    This routine sends the caller UI data as a UI frame
 * -----------     
 * Parameters:     ptr to caller buffer
 * ----------
 * Returns:        RLP_OK:Request OK     RLP_ERROR:UI xchg busy, try later!
 * -------         
 ****************************************************************************/
short rlp_send_UI_data (const byte* Data)
{
   short RetVal = RLP_ERROR;

   //MSG_LOW ("In rlp_send_UI_data",0,0,0);
   
   if (UI_State == STATE_IDLE)
   {
      uint16 cnt;

      /* Allocate PDU for UI Frame */
      TxUIData = (NULL != TxUIData)? TxUIData : ds_ucsd_UplinkPDUMgr_New();
      if (NULL == TxUIData)
      {
        return RLP_ERROR;
      }
      /* Transfer PDU data */
      if (RLP_CURRENT_PDU_SIZE !=
          (cnt = dsm_pushdown_tail(&TxUIData, (byte*)Data, RLP_CURRENT_PDU_SIZE,
                                   DSM_ITEM_POOL(TxUIData))))
      {
         DATA_MSG2_HIGH("rlp_send_UI_data write problem: %d!=%d",
                   RLP_CURRENT_PDU_SIZE, cnt);
         return RLP_ERROR;
      }
      
      UI_State = STATE_SEND;
      RetVal = RLP_OK;
   }
   
   return RetVal;
}

/****************************************************************************
 * Function name:  rlp_flow_control
 * -------------
 * Description:    This routine returns true if an RLP flow control condition
 * -----------     exists.
 * Parameters:     None
 * ----------
 * Returns:        TRUE=Flow control condition
 * -------         
 ****************************************************************************/
boolean rlp_flow_control (void)
{
   boolean RetVal = FALSE;

   if (( ds_ucsd_UplinkPDUMgr_SendSlotsAvailable() == FALSE ) ||
       ( remote_receiver_ready == FALSE ) ||
       ( ds_ucsd_remote_fc_enabled() == TRUE ))
   {
      RetVal = TRUE;
   }
   return RetVal;
}

/****************************************************************************
 * Function name:  rlp_remote_receiver_ready
 * -------------
 * Description:    Returns the remote_receiver_ready flag.
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        TRUE/FALSE
 * -------         
 ****************************************************************************/
boolean rlp_remote_receiver_ready (void)
{
   return remote_receiver_ready;
}

/****************************************************************************
 * Function name:  rlp_XID_send
 * -------------
 * Description:    This routine causes a XID negotiation with the remote node. 
 * -----------     
 * Parameters:     ptr to XID params to send
 * ----------
 * Returns:        RLP_OK: XID request, RLP_ERROR: cannot request XID 
 * -------         
 ****************************************************************************/
short rlp_XID_send (const XID_Params_T* SendXID)
{

   if ((XID_C_State == STATE_IDLE) && (SendXID != NULL))
   {
      /* Use the values with no error checking!! */
      NewXID = *SendXID;
      
      XID_C_State = STATE_SEND;
      XID_Count   = 0;
      return RLP_OK;
   }
   else
   {
      DATA_MSG1_ERROR("XID_C_State!= STATE_IDLE or SendXID== NULL %p",SendXID);
      return RLP_ERROR;
   }
}

/****************************************************************************
 * Function name:  rlp_TEST_send
 * -------------
 * Description:    This routine causes a TEST sequence to start
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        RLP_OK: TEST request acknowledged, 
 * -------         RLP_ERROR: cannot request TEST 
 ****************************************************************************/

short rlp_TEST_send (void)
{
   word len;
   
   //MSG_LOW ("In rlp_TEST_send",0,0,0);
   
   Test_C_State = STATE_SEND;
   Test_Count   = 0;
   Test_C_PBit  = TRUE;

   /* Allocate new Uplink PDU */
   Test_C_Data = (NULL != Test_C_Data)?
                 Test_C_Data : ds_ucsd_UplinkPDUMgr_New();

   /* Load test data into PDU */
   len = (uint16)strlen((char*)TEST_DATA);
   
   /* -------------------------------------------------------------------------
   KW Fix:NULL ptr dereferencing Check after call to ds_ucsd_UplinkPDUMgr_New()
   --------------------------------------------------------------------------*/
   if (Test_C_Data != NULL)
   {
       DSM_PUSHDOWN_TAIL (&Test_C_Data, (byte*)TEST_DATA, len); /*lint !e666*/
       return RLP_OK;
   }
   else
   {
       DATA_MSG0_ERROR("Test_C_Data = NULL");	   
	   return RLP_ERROR;
   }

}



/****************************************************************************
 * Function name:  rlp_main
 * -------------
 * Description:    This is the main entry point for the RLP
 * -----------     Non-transparent data layer.  It is called
 *                 from the DS-ISR upon each 'tick' at 
 *                 20ms intervals.
 *                 Each time it is called the function branches
 *                 depending upon it's current state.
 * Parameters:   None
 * ----------
 * Returns:      None
 * --------
 ****************************************************************************/
void rlp_main (void)
{
   
   register uint32         loopcounter          = 0;
   register boolean        rx_frame_available   = FALSE;
   register DSI_Event_T    Event                = READ_EVENT_TO_RLP ();

   
   do
   {
      /* Branch to the current state action routine */
      switch (RlpCurrentState)
      {
         case RLPSTATE_ADM_DETACHED:
            ActionADMDetached (Event);
            break;
         
         case RLPSTATE_ADM_ATTACHED:
            ActionADMAttached (Event);
            break;
         
         case RLPSTATE_PENDING_CONNECTION_REQUEST:
            ActionPendingConnectionRequest (Event);
            break;
         
         case RLPSTATE_PENDING_CONNECTION_RESPONSE:
            ActionPendingConnectionResponse(Event);
            break;
         
         case RLPSTATE_CONNECTION_ESTABLISHED:
            ActionConnectionEstablished (Event);
            break;
         
         case RLPSTATE_DISC_INITIATED:
            ActionDiscInitiated (Event);
            break;
         
         case RLPSTATE_PENDING_RESET_REQUEST:
            ActionPendingResetRequest (Event);
            break;
         
         case RLPSTATE_PENDING_RESET_INDICATION:
            ActionPendingResetIndication (Event);
            break;

         case RLPSTATE_MS_REMAPPING:
            ActionMSRemapping (Event);
            break;

         
         default:
            break;
      }
   
   
      /* Service a Detach Request in ANY state */
      if (Event == RLP_DETACH_REQ)
      {
         rlp_initialise (NULL);
      }
   
   #ifdef DS_UCSD_DEBUG
      if ((OldRlpState != RlpCurrentState) ||
	  (Event != (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT))
      {
         DS_UCSD_DEBUG_MSG_3 ("RLP: Evt=%d, OldSt=%d, CurrSt=%d",
                              Event,OldRlpState,RlpCurrentState);
         OldRlpState = RlpCurrentState;
      }
                            
   #endif   
      

      /* Get a new RLP event */
      Event = READ_EVENT_TO_RLP ();

      /* See if there is still an RLP RX frame available... sometimes
      ** this will happen because some the transient RLP states 
      ** (ActionPendingConnectionResponse for example) will not
      ** do an RLPReadFrame.  If we get out of this loop without reading a
      ** a frame, this is a problem that we will flag below.
      */
      if (ds_ucsd_get_rlp_process_downlink())
      {
         rx_frame_available = ds_HardwareFrameAvailable ();
      }
      
     /* Keep going while there are RLP events to process and there
     ** or there is an RLP RX frame to process, and we haven't exceeded
     ** the max number of loops (currently 3)
     */ 
   } while ( (rx_frame_available || 
             (Event != (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT)) &&
            (loopcounter++ < 3));


   /* If we get out of the above, and a frame is still available, that
   ** is a problem that should be flagged.  If this happens, we will
   ** read the frame here, otherwise we will stop processing RX frames
   */
   if (rx_frame_available)
   {
      DATA_MSG1_ERROR("RLP in state %d exited without reading frame",RlpCurrentState);
      (void)ReadRLPFrame (RxFrame);
   }

   /* Check XID and TEST timers in ANY state */
   CheckXIDTimeout ();
   CheckTESTTimeout();

   /* Clean up Rx Frame data buffer */
   if (NULL != RxFrame->Data)
   {
     ds_ucsd_DownlinkPDUMgr_Free (&RxFrame->Data);
   }
}



/****************************************************************************
 * Function name:  ActionADMDetached
 * -------------
 * Description:    This is the action routine for the state ADM_DETACHED. 
 * -----------     In this state the RLP is waiting for the higher layer
 *                 to attach it, before it can be used.
 * Uses:
 * ----
 * Parameters:   RLP event
 * ----------
 * Returns:      None
 * --------
 *****************************************************************************/
static void ActionADMDetached (DSI_Event_T Event)
{
   if (ds_ucsd_get_rlp_process_downlink())  /* see l1_ds.h */
   {
      /* Deal with any frames received. Any frames not relevant in this  */ 
      /* state will be automatically discarded. */
      (void) ReadRLPFrame (RxFrame);
   }
   
   if ((ds_ucsd_get_rlp_process_uplink()) && (ds_HardwareTxReady()))
   {
      /* Now send any frames if required */ 
      if (DM_State == STATE_SEND) 
      {
         (void) SendUnNumberedFrame (DM, FALSE, DM_FBit, NULL);
         DM_State = STATE_IDLE; 
      }
      else
      {  
         (void) SendUnNumberedFrame (FRAME_NULL, FALSE, FALSE, NULL);
      }
   }
   
   
   /* If the higher layer requested an Attach, then switch state */ 
   if (Event == RLP_ATTACH_REQ)
   {
      UA_State = STATE_IDLE;
      RlpCurrentState = RLPSTATE_ADM_ATTACHED;
   }
}

/****************************************************************************
 * Function name:  ActionADMAttached
 * -------------
 * Description:    This is the action routine for the state ADM_ATTACHED. 
 * -----------     In this state the RLP is waiting for a connect request
 *                 This can come from the higher layer, or over the air
 *                 in the form of a SABM frame.
 *                 If RLP encounters an abnornal error event, ADM
 *                 state is entered. A timer is used to force teardown
 *                 of call after a period of time to free system
 *                 resources.
 * Parameters:   RLP event
 * ----------
 * Returns:      None
 * --------
 *****************************************************************************/
static void ActionADMAttached (DSI_Event_T Event)
{
  short status;
   
  /* Check for error handling mode after abnormal event */
  if ( TRUE == ds_ucsd_get_rlp_error_handling() )
  {
    /* Some GCF test cases (e.g. 14.8.2) require UE to remain in idle
     * after failed XID negotiation.  Others require call to be
     * automatically terminated (e.g. 29.3.3.*).  Use a timer
     * specified by NV parameter to control how long to remain in ADM
     * state before call teardown */

    /* See if idle timer is active */
    if (TIMER_RUNNING (TIMERID_ADM_IDLE))
    {
      /* See if timer expired */
      if (TIMER_EXPIRED (TIMERID_ADM_IDLE))
      {
        STOP_TIMER (TIMERID_ADM_IDLE);
        DATA_MSG0_HIGH("RLP: ADM state idle timer expired");

        /* Request call teardown */
        WRITE_EVENT_TO_L2R (RLP_ERROR_IND);
        ds_ucsd_set_rlp_error_handling(FALSE);
      }
    }
    else
    {
      /* Use timer on non-zero idle delay */
      if ( 0 < current_RLP_params->rlp_adm_idle_timer )
      {
        /* Setup timer based on value from NV, in seconds */
        int32 delay = (int32)(current_RLP_params->rlp_adm_idle_timer *
                              TIMER_COUNT_1_SEC);
        START_TIMER (TIMERID_ADM_IDLE, delay);
        DATA_MSG0_HIGH("RLP: ADM state idle timer started");
      }
      /* Otherwise immediately terminate call */
      else
      {
        /* Request call teardown */
        WRITE_EVENT_TO_L2R (RLP_ERROR_IND);
        ds_ucsd_set_rlp_error_handling(FALSE);
      }
    }
  }
  
   
  if (ds_ucsd_get_rlp_process_downlink())  /* see l1_ds.h */
  {
    /* Deal with any frames received. Any frames not relevant in this  */ 
    /* state will be automatically discarded. */
    status = ReadRLPFrame (RxFrame);
    if ((status == RLP_OK) && (FrameType == FRAMETYPE_UNNUMBERED))
    {
      switch (UnNumberedFrame)
      {
      case DISC:  
        UA_State = STATE_SEND;
        UA_FBit  = PF;
        break;   
               
      case SABM:
        InitialiseLinkVariables ();  
        WRITE_EVENT_TO_L2R (RLP_CONNECT_IND);
        RlpCurrentState = RLPSTATE_PENDING_CONNECTION_RESPONSE;
        break;   
               
      case UI:    HandleUI    (RxFrame);  break;
      case XID:   HandleXID   (RxFrame);  break;
      case _TEST: HandleTEST  (RxFrame);  break;
            
      default:    break;    /* Make Lint happy */
      }
    }
  }
   
  if ((ds_ucsd_get_rlp_process_uplink()) && (ds_HardwareTxReady()))
  {
    /* Now send any frames if required */ 
    status = Send_TXU();
    if (status == 0)
    {
      if (UA_State == STATE_SEND) 
      {
        status = SendUnNumberedFrame (UA, FALSE, UA_FBit, NULL);
        UA_State = STATE_IDLE; 
      }
      else if (DM_State == STATE_SEND) 
      {
        status = SendUnNumberedFrame (DM, FALSE, DM_FBit, NULL);
        DM_State = STATE_IDLE; 
      }
      else
      {
        status = SendUnNumberedFrame (FRAME_NULL, FALSE, FALSE, NULL);
      }
    }
  }
   
  /* If higher layer requested a Connect, then issue SABM and switch state */ 
  if (Event == RLP_CONNECT_REQ)
  {
    SABM_State = STATE_SEND;
    SABM_Count = 0;
    RlpCurrentState = RLPSTATE_PENDING_CONNECTION_REQUEST;
  }

  /* Check to exiting state after error condition */
  if (( TRUE == ds_ucsd_get_rlp_error_handling() ) &&
      ( RLPSTATE_PENDING_CONNECTION_RESPONSE == RlpCurrentState ))
  {
    /* Cancel timer and clear error flag */
    STOP_TIMER (TIMERID_ADM_IDLE);
    ds_ucsd_set_rlp_error_handling(FALSE);
  }

}

/****************************************************************************
 * Function name:  ActionPendingConnectionRequest
 * -------------
 * Description:    This is the action routine for the state 
 * -----------     PENDING_CONNECTION_REQUEST. In this state 
 *                 the RLP waits for a UA response from the 
 *                 SABM that it issued. 
 *                 (So this state would be better named as
 *                  PENDING_CONNECTION_RESPONSE, but the names
 *                  have been kept consistent with the ETS Spec 04.22:
 *                  Radio Link Protocol (RLP)).
 * Uses:
 * ----
 * Parameters:   RLP event
 * ----------
 * Returns:      None
 * --------
 ****************************************************************************/
static void ActionPendingConnectionRequest (DSI_Event_T Event)
{
   short status;
   
   if (ds_ucsd_get_rlp_process_downlink())  /* see l1_ds.h */
   {
      /* Deal with any frames received. Any frames not relevant in this  */ 
      /* state will be automatically discarded. */
      status = ReadRLPFrame (RxFrame);
      
      if ((status == RLP_OK) && (FrameType == FRAMETYPE_UNNUMBERED))
      {
         switch (UnNumberedFrame)
         {
            case UA:
               /* IF a correct UA then goto CONNECTED state! */
               if ((SABM_State == STATE_WAIT) && (PF))
               {
                  STOP_TIMER (TIMERID_T1);
                  InitialiseLinkVariables ();
                  
                  WRITE_EVENT_TO_L2R (RLP_CONNECT_CNF);
                  RlpCurrentState  = RLPSTATE_CONNECTION_ESTABLISHED;
               }
               break;
               
            case DM:
               /* IF DM then remote RLP will not connect! */
               if ((SABM_State == STATE_WAIT) && (PF))
               {
                  STOP_TIMER (TIMERID_T1);
                  Poll_xchg = STATE_IDLE;
                  
                  WRITE_EVENT_TO_L2R (RLP_CONNECT_CNF_NEG);
                  RlpCurrentState = RLPSTATE_ADM_ATTACHED;
               }
               break;
               
            case DISC:  
               UA_State = STATE_SEND;
               UA_FBit  = PF;
               WRITE_EVENT_TO_L2R (RLP_DISC_IND);
               RlpCurrentState  = RLPSTATE_ADM_ATTACHED;
               break;   

            case SABM:  
               STOP_TIMER (TIMERID_T1);
               UA_State = STATE_SEND;
               UA_FBit  = TRUE;
               InitialiseLinkVariables ();
               WRITE_EVENT_TO_L2R (RLP_CONNECT_CNF);
               RlpCurrentState  = RLPSTATE_CONNECTION_ESTABLISHED;
               break;   
               
            case UI:    HandleUI    (RxFrame);  break;
            case XID:   HandleXID   (RxFrame);  break;
            case _TEST: HandleTEST  (RxFrame);  break;  

            default:    break;    /* Make Lint happy */
         }
      }
   }
      
   if ((ds_ucsd_get_rlp_process_uplink()) && (ds_HardwareTxReady()))
   {
      /* IF Timeout Occurred */
      if (TIMER_EXPIRED (TIMERID_T1))
      {
         DS_UCSD_DEBUG_MSG ("RLP:T1 out!");

         STOP_TIMER (TIMERID_T1); /* Ensure we dont get another timeout event*/
         Poll_xchg  = STATE_IDLE;
         
         if (SABM_Count > Rlp_CurrentXID.N2)
         {
            DATA_MSG0_ERROR("RLP: Start ABM failed to complete");
            ds_ucsd_set_rlp_error_handling(TRUE);
           
            /* Exceeded the SABM retries, so goto ADM_ATTACHED state */
            RlpCurrentState  = RLPSTATE_ADM_ATTACHED;
            SABM_State = STATE_IDLE;         
         }
         else
         {
            /* Try again to SABM */
            SABM_State = STATE_SEND;
         }
      }
      
      
      /* Now send any frames if required */ 
      status = Send_TXU();
      if (status == 0)
      {
         if ((SABM_State == STATE_SEND) && (Poll_xchg == STATE_IDLE)) 
         {
            status = SendUnNumberedFrame (SABM, TRUE, TRUE, NULL);
            
            SABM_State = STATE_WAIT;
            Poll_xchg  = STATE_WAIT;
            SABM_Count++;
            //MSG_LOW ("Sending SABM - T1 Timeout: %d",T1ValueInMs,0,0);
            START_TIMER (TIMERID_T1, T1ValueInMs);
         }
         else
         {
            status = SendUnNumberedFrame (FRAME_NULL, FALSE, FALSE, NULL);
         }
      }
   }

   switch (Event)
   {
      case RLP_DISC_REQ:
         Disconnect();
         break;
                    
      default:
         break;    /* Make Lint happy */
   }
}

/****************************************************************************
 * Function name:  ActionPendingConnectionResponse
 * -------------
 * Description:    This is the action routine for the state 
 * -----------     PENDING_CONNECTION_RESPONSE. In this state 
 *                 the RLP waits for a Higher Layer response
 *                 to an SABM connect request from a remote RLP.
 *
 * Uses:
 * ----
 * Parameters:   Event
 * ----------
 * Returns:      None
 * --------
 ****************************************************************************/
static void ActionPendingConnectionResponse(DSI_Event_T Event)
{
    switch (Event)
    {
       case RLP_CONNECT_RES:
          UA_State = STATE_SEND;
          UA_FBit  = TRUE;
          RlpCurrentState = RLPSTATE_CONNECTION_ESTABLISHED;    
          break;
          
       case RLP_CONNECT_RES_NEG:
          
          
          //DM_State = STATE_SEND;
          //DM_FBit  = TRUE;
   
          RlpCurrentState = RLPSTATE_ADM_ATTACHED;
          break;
          
      default:
         /* Read, but don't process downlink frames while waiting
         ** for L2R response 
         */
         if (ds_ucsd_get_rlp_process_downlink())
         {
            (void) ReadRLPFrame (RxFrame);
         }
         break;    /* Make Lint happy */
    }
}

/****************************************************************************
 * Function name:  ActionConnectionEstablished
 * -------------
 * Description:    This is the action routine for the state 
 * -----------     CONNECTION_ESTABLISHED.
 *                 This is the state in which the data transfer
 *                 takes place.
 * Uses:
 * ----
 * Parameters:   Event
 * ----------
 * Returns:      None
 * --------
 ****************************************************************************/
static void ActionConnectionEstablished (DSI_Event_T Event)
{
   short status = RLP_OK;
   
   if (ds_ucsd_get_rlp_process_downlink())  /* see l1_ds.h */
   {
      /* Deal with any frames received. Any frames not relevant in this  */ 
      /* state will be automatically discarded. */
      status = ReadRLPFrame (RxFrame);
      
      if (status == RLP_OK)
      {
         switch (FrameType)
         {
            case FRAMETYPE_UNNUMBERED:
               
               switch (UnNumberedFrame)
               {
                  case DISC:  
                     STOP_TIMER (TIMERID_T1);
                     UA_State = STATE_SEND;
                     UA_FBit  = PF;
                     WRITE_EVENT_TO_L2R (RLP_DISC_IND);
                     RlpCurrentState  = RLPSTATE_ADM_ATTACHED;
                     break;   
                     
                  case SABM:  
                     STOP_TIMER (TIMERID_T1);
                     /* Issue a Reset Indication to higher layer*/
                     WRITE_EVENT_TO_L2R (RLP_RESET_IND);
                     RlpCurrentState = RLPSTATE_PENDING_RESET_INDICATION;
                     break;   
                     
                  case UI:    HandleUI    (RxFrame);  break;
                  case XID:   HandleXID   (RxFrame);  break;
                  case _TEST: HandleTEST  (RxFrame);  break;  

                  default:
                     break;    /* Make Lint happy */
               }
               break;
               
            case FRAMETYPE_SUPERVISORY:
               /* If the Frame has an Unsolicited F Bit then discard it */
               if ((CR == FALSE) && (PF) && (Poll_xchg == STATE_IDLE))
               {
                  DS_UCSD_DEBUG_MSG ("RLP:Unsolicited F Bit! discarding");
                  break;
               }
               
               /* If the remote end has finally stopped sending I frames
                  then we can stop sending RNR's. */
               /** SendRNRDuringRecovery = FALSE; **/
               
               /* If the Nr of the received Frame is in range then use it */
               if (SupervisoryNrIsLegal())
               {
                  S_handler ();
               }
               else
               {
                  /* Respond to a RNR to be safe */ 
                  if ( (SF == RNR) && (remote_receiver_ready) )
                  {
                     DS_UCSD_DEBUG_MSG ("RLP:remote_receiver_rdy=FALSE");
                     remote_receiver_ready = FALSE;
                  }
                  else if ( (SF == RR) && (!remote_receiver_ready) )
                  {
                     DS_UCSD_DEBUG_MSG ("RLP:remote_receiver_rdy=TRUE");
                     remote_receiver_ready = TRUE;
                  }
                        
                  //DS_UCSD_DEBUG_MSG_1 ("Bad Nr-discard S:%d", Nr);
               }
               break;
               
            case FRAMETYPE_INFORMATION:
               
#ifdef DONT_ALLOW_PBIT_IFRAME
               /* The ETS 04.22 RLP spec says that an I-Frame cannot
                  have P=1, BUT then the ETS Test Spec 11.10 says it can!
                  (Test 29.3.2.6.1) */
               
               /* If the Frame has an Unsolicited F Bit then discard it */
               if ((PF) && (Poll_xchg == STATE_IDLE))
               {
                  break;
               }
#endif
               
               /* If first time we have received an INFO frame, let
               ** L2R state machine know about it
               */
               if (!RLP_Rx_Info_Frame)
               {
                  DS_UCSD_DEBUG_MSG("Got first info frame");
                  WRITE_EVENT_TO_L2R(L2R_RX_INFO_FRAME);
                  RLP_Rx_Info_Frame = TRUE;
               }

               /* Handle the I part of the frame independently */
               I_handler ();
               
               /* If the Nr of the received S-Frame is in range then use it */
               if (SupervisoryNrIsLegal())
               {
                  S_handler ();
               }
               else
               {
                  /* Respond to a RNR to be safe */ 
                  if ( (SF == RNR) && (remote_receiver_ready) )
                  {
                     DS_UCSD_DEBUG_MSG ("RLP:remote_receiver_rdy=FALSE");
                     remote_receiver_ready = FALSE;
                  }
                  else if ( (SF == RR) && (!remote_receiver_ready) )
                  {
                     DS_UCSD_DEBUG_MSG ("RLP:remote_receiver_rdy=TRUE");
                     remote_receiver_ready = TRUE;
                  }
                        
                  //DS_UCSD_DEBUG_MSG_1 ("Bad Nr-discard S:%d", Nr);
               }
               break;

            default:
               break;    /* Make Lint happy */
         }
      }
   }
   
   if ((ds_ucsd_get_rlp_process_uplink()) && (ds_HardwareTxReady()))
   {
      /* Now send any frames if required */
      status = Send_TXU();
      if (status == 0)
      {
         status = SendData ();
      }
      
      
      /* IF REJ/SREJ Timeout Occurred */
      if (TIMER_EXPIRED (TIMERID_TRCVR))
      {
         /* Ensure we dont get another timeout event*/
         STOP_TIMER (TIMERID_TRCVR); 
         
         /* Cancel the REJ-it is NOT retried (04.22 p5.2.3.4) but why not? */
         DS_UCSD_DEBUG_MSG ("RLP: TRCVR.out! REJ_State=IDLE");
         REJ_State = STATE_IDLE;
         
         /* IF using SREJ THEN cancel the SREJ frames */
         if ( SREJisSuitable() )
         {
            /* Getting here means that we have tried to recover using
               SREJ, but this has failed. The remote node will eventually
               recover with a Poll checkpoint recovery sequence, so
               we need do nothing here. If it is required to RETRY
               SREJ frames, then the code should be inserted here 
               (it's optional - 04.22 p5.2.3.6).
               The reason this has not been implemented is that it would
               necessitate a separate timer for each SREJ frame, ie up to 61,
               (or 480 in the phase 2+ versions!) and the overhead of all those
               counter-timers would be high. */
         }
      }
      
      /* IF Timeout Occurred */
      if (TIMER_EXPIRED (TIMERID_T1))
      {
         DS_UCSD_DEBUG_MSG_3 ("RLP:T1out! ps=%d px=%d pc=%d",
                               Poll_State, Poll_xchg, Poll_Count);
         
         STOP_TIMER (TIMERID_T1); /* Ensure we dont get another timeout event*/
         Poll_xchg  = STATE_IDLE;

         
         if ((Poll_State == STATE_IDLE) && (RepeatedPollResponse == FALSE))
         {
            /* for 29.3.2.6.9 - cetecom test !!*/
            if (Poll_Count > Rlp_CurrentXID.N2)
            {
               DS_UCSD_DEBUG_MSG ("RLP: RESET!");
               WRITE_EVENT_TO_RLP (RLP_RESET_REQ);
            }
            else if (Poll_Count == 0)
            {
               /* Start a poll recovery */
               Poll_State = STATE_SEND;
               VS_at_first_poll = ds_ucsd_UplinkPDUMgr_VS(); 
            }
            else
            {
               Poll_State = STATE_SEND; /* Retry the poll */
            }
         }
         else
         {
            if (Poll_Count > Rlp_CurrentXID.N2)
            {

               DS_UCSD_DEBUG_MSG ("RLP: RESET!");
               WRITE_EVENT_TO_RLP (RLP_RESET_REQ);
            }
            else
            {
               /* Try again */
               Poll_State = STATE_SEND;
            }
         }
      }
   }
   
   
   /* Check on any other requests from the Higher Layer */
   switch (Event)
   {
      case RLP_DISC_REQ:
         Disconnect ();
         break;

      case RLP_RESET_REQ:
         STOP_TIMER (TIMERID_T1);
         SABM_State = STATE_SEND;
         SABM_Count = 0;
         RlpCurrentState = RLPSTATE_PENDING_RESET_REQUEST;
         break;

      case RLP_REMAP_REQ:
         status = GoTo_MS_Remapping ();
         if (status == RLP_ERROR)
         {
            DATA_MSG0_ERROR("RLP: Problem on REMAP request");
            ds_ucsd_set_rlp_error_handling(TRUE);

            RlpCurrentState  = RLPSTATE_ADM_ATTACHED;
         }
         else
         {
            RlpCurrentState = RLPSTATE_MS_REMAPPING;
         }
         break;
         
      /* It is possible that we got the SABM from the far end
      ** and are already connected... if this is the case, 
      ** tell L2R
      */
      case RLP_CONNECT_REQ:
         WRITE_EVENT_TO_L2R (RLP_CONNECT_CNF);
         break;

      default:
         break;    /* Make Lint happy */
   }
}

/****************************************************************************
 * Function name:  GoTo_MS_Remapping
 * -------------
 * Description:    Performs state transition to MS REMAP.
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        None
 * -------
 ****************************************************************************/
static short GoTo_MS_Remapping (void)
{
   short status = RLP_OK;

   /* Cancel all Poll/REJ etc. control variables */
   Ackn_FBit  = FALSE;
   Poll_State = STATE_IDLE;
   Poll_Count = 0;
   Poll_xchg  = STATE_IDLE;
   REJ_State  = STATE_IDLE;
   SABM_State = STATE_IDLE;
   SABM_Count = 0;
   DISC_State = STATE_IDLE;
   remote_receiver_ready = TRUE;
   PolledDueToSREJFailure = FALSE;
#ifdef ENHANCED_POLL_RECOVERY
   AcksInSequence = TRUE;
#endif
   LastPollNr = -1;
   RepeatedPollResponse = FALSE;

   /* Remap the Downlink PDU list */
   TxRemapNr = ds_ucsd_DownlinkPDUMgr_VR ();
   MakeRemapFrame (TxRemapNr, &Rlp_CurrentXID);
   /* Reset last remap N(R) incase UE is bouncing between modes */
   UplinkRemappedLastNr = INVALID_REMAP_NR;

   /* Current PDU Type is now the new one - set by L2R */
   status = ds_ucsd_DownlinkPDUMgr_Remap (CurrentPDUType);

   if (status == RLP_OK)
   {
     /* Stop the T1 timer, see 24.022 section 5.4 */
     STOP_TIMER (TIMERID_T1);
     
     /* Start Timer for REMAP exchange */
     START_TIMER (TIMERID_REMAP, (int32)(T1ValueInMs*Rlp_CurrentXID.N2) );
   }

   return status;
}


/****************************************************************************
 * Function name:  ActionDiscInitiated
 * -------------
 * Description:    This is the action routine for the state
 * -----------     DISC_INITIATED. The RLP enters this state
 *                 in response to a Disconnect Request from
 *                 the higher layer.
 * Uses:
 * ----
 * Parameters:   Event
 * ----------
 * Returns:      None
 * -------
 ****************************************************************************/
/*ARGSUSED*/
static void ActionDiscInitiated (DSI_Event_T Event)
{
   short status = RLP_OK;
   
   if (ds_ucsd_get_rlp_process_downlink())  /* see l1_ds.h */
   {   
      /* Deal with any frames received. Any frames not relevant in this  */ 
      /* state will be automatically discarded. */
      status = ReadRLPFrame (RxFrame);
      
      if ((status == RLP_OK) && (FrameType == FRAMETYPE_UNNUMBERED))
      {
         switch (UnNumberedFrame)
         {
            case UA:
            case DM:
               if ((DISC_State == STATE_WAIT) && (DISC_PBit  == PF))
               {
                  if (DISC_PBit)
                  {
                     Poll_xchg = STATE_IDLE;
                  }
                  
                  STOP_TIMER (TIMERID_T1);
                  WRITE_EVENT_TO_L2R (RLP_DISC_IND);
                  RlpCurrentState  = RLPSTATE_ADM_ATTACHED;
               }
               break;
               
            case DISC:  
               STOP_TIMER (TIMERID_T1);
               UA_State = STATE_SEND;
               UA_FBit  = PF;
               WRITE_EVENT_TO_L2R (RLP_DISC_IND);
               RlpCurrentState   = RLPSTATE_ADM_ATTACHED;
               break;   
               
            case UI:    HandleUI    (RxFrame);  break;
            case XID:   HandleXID   (RxFrame);  break;
            case _TEST: HandleTEST  (RxFrame);  break;
            
            default:     break;    /* Make Lint happy */
         }
      }
   }   
   
   if ((ds_ucsd_get_rlp_process_uplink()) && (ds_HardwareTxReady()))
   {
      /* IF Timeout Occurred */
      if (TIMER_EXPIRED (TIMERID_T1))
      {

         DS_UCSD_DEBUG_MSG ("RLP: T1out!");
         
         STOP_TIMER (TIMERID_T1); /* Ensure we dont get another timeout event*/
         
         if (DISC_PBit)
         {
            Poll_xchg  = STATE_IDLE;
         }
         
         if (DISC_Count > Rlp_CurrentXID.N2)
         {
            DATA_MSG0_ERROR("RLP: Disconnect failed to complete");
            ds_ucsd_set_rlp_error_handling(TRUE);
           
            /* Exceeded the retries, so goto ADM_ATTACHED state */
            RlpCurrentState  = RLPSTATE_ADM_ATTACHED;
         }
         else
         {
            /* Try again */
            DISC_State = STATE_SEND;
         }
      }
      
      
      
      /* Now send any frames if required */ 
      status = Send_TXU();
      if (status == 0)
      {
         if ((DISC_State == STATE_WAIT) ||
             ((DISC_PBit) && (Poll_xchg == STATE_WAIT)))
         {
            /* Send a NULL Frame */
            status = SendUnNumberedFrame (FRAME_NULL, FALSE, FALSE, NULL);
         }
         else
         {
            status = SendUnNumberedFrame (DISC, TRUE, DISC_PBit, NULL);
            if (DISC_PBit)
            {
               Poll_xchg = STATE_WAIT;
            }
            DISC_State = STATE_WAIT;
            DISC_Count++;
            //MSG_LOW ("Sending DISC - T1 Timeout: %d",T1ValueInMs,0,0);
            START_TIMER (TIMERID_T1, T1ValueInMs);
         }
      }
   }
}

/****************************************************************************
 * Function name:  ActionPendingResetRequest
 * -------------
 * Description:    This is the action routine for the state
 * -----------     PRNDING_RESET_REQUEST.  The RLP enters this state
 *                 in response to a reset request from the higher layer.
 *                 After sending a SABM (to invoke a link reset),
 *                 it remains in this state until an acknowledge is
 *                 received from the remote RLP.
 * Uses:
 * ----
 * Parameters:     Event
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
/*ARGSUSED*/
static void ActionPendingResetRequest (DSI_Event_T Event)
{
   short status = RLP_OK;
   
   if (ds_ucsd_get_rlp_process_downlink())  /* see l1_ds.h */
   {   
      /* Deal with any frames received. Any frames not relevant in this  */ 
      /* state will be automatically discarded. */
      status = ReadRLPFrame (RxFrame);
      
      if ((status == RLP_OK) && (FrameType == FRAMETYPE_UNNUMBERED))
      {
         switch (UnNumberedFrame)
         {
            case UA:
               if ((SABM_State == STATE_WAIT) && (PF))
               {
                  STOP_TIMER (TIMERID_T1);
                  InitialiseLinkVariables ();
                  WRITE_EVENT_TO_L2R (RLP_RESET_CNF);
                  RlpCurrentState = RLPSTATE_CONNECTION_ESTABLISHED;
               }
               break;
               
            case DISC:  
               STOP_TIMER (TIMERID_T1);
               
               UA_State = STATE_SEND;
               UA_FBit  = PF;
               WRITE_EVENT_TO_L2R (RLP_DISC_IND);
               RlpCurrentState = RLPSTATE_ADM_ATTACHED;
               break;   
               
            case SABM:
               STOP_TIMER (TIMERID_T1);
               
               UA_State = STATE_SEND;
               UA_FBit  = TRUE;
               InitialiseLinkVariables ();
               WRITE_EVENT_TO_L2R (RLP_RESET_CNF);
               RlpCurrentState  = RLPSTATE_CONNECTION_ESTABLISHED;  
               break;
               
            case UI:    HandleUI    (RxFrame);  break;
            case XID:   HandleXID   (RxFrame);  break;
            case _TEST: HandleTEST  (RxFrame);  break;
            default:    break;    /* Making Lint happy */
         }
      }
   }
   
   if ((ds_ucsd_get_rlp_process_uplink()) && (ds_HardwareTxReady()))
   {
      /* IF Timeout Occurred */
      if (TIMER_EXPIRED (TIMERID_T1))
      {

         DS_UCSD_DEBUG_MSG ("RLP: T1out!");
         
         STOP_TIMER (TIMERID_T1); /* Ensure we dont get another timeout event*/
         Poll_xchg  = STATE_IDLE;
         
         if (SABM_Count > Rlp_CurrentXID.N2)
         {
            DATA_MSG0_ERROR("RLP: Start ABM failed to complete");
            ds_ucsd_set_rlp_error_handling(TRUE);

            /* Exceeded the retries, so goto ADM_ATTACHED state */
            RlpCurrentState   = RLPSTATE_ADM_ATTACHED;
         }
         else
         {
            SABM_State = STATE_SEND;    /* Try again */
         }
      }
      
      
      
      /* Now send any frames if required */ 
      status = Send_TXU();
      if (status == 0)
      {
         if ((SABM_State == STATE_SEND) && (Poll_xchg == STATE_IDLE))
         {
            status = SendUnNumberedFrame (SABM, TRUE, TRUE, NULL);
            
            Poll_xchg  = STATE_WAIT;
            SABM_State = STATE_WAIT;
            SABM_Count++;
            //MSG_LOW ("Sending SABM - T1 Timeout: %d",T1ValueInMs,0,0);
            START_TIMER (TIMERID_T1, T1ValueInMs);
         }
         else
         {
            status = SendUnNumberedFrame (FRAME_NULL, FALSE, FALSE, NULL);
         }
      }
   }
}

/****************************************************************************
 * Function name:  ActionPendingResetIndication
 * -------------
 * Description:    This is the action routine for state
 * -----------     PENDING_RESET_INDICATION. The RLP enters this
 *                 state upon receiving a SABM (reset) when already 
 *                 connected.  It remains in this state until it 
 *                 gets a Reset Response event from the higher layer
 *                 (or a Disconnect Request).
 *          
 *                 For this implementation, assume that the higher layer 
 *                 wants to reset, so assume the Reset Response Event
 *                 implicitely.
 * Uses:
 * ----
 * Parameters:     Event
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
/*ARGSUSED*/
static void ActionPendingResetIndication (DSI_Event_T Event)
{
   /* IF a ResetResponse Event received from the higher layer */
   /* (assume that this is the case implicitely for this implementation) */
   UA_State = STATE_SEND;
   UA_FBit  = TRUE;
   InitialiseLinkVariables ();
   RlpCurrentState = RLPSTATE_CONNECTION_ESTABLISHED;                
}




/****************************************************************************
 * Function name:  InitialiseLinkVariables
 * -------------
 * Description:    This routine initialises all the variables which 
 * -----------     control the RLP mechanism.
 * Uses:
 * ----
 * Parameters:   None
 * ----------
 * Returns:      None
 * --------
 *****************************************************************************/
static void InitialiseLinkVariables (void)
{
   Ackn_FBit  = FALSE;
   Poll_State = STATE_IDLE;
   Poll_Count = 0;
   Poll_xchg  = STATE_IDLE;
   REJ_State  = STATE_IDLE;
   SABM_State = STATE_IDLE;
   SABM_Count = 0;
   DISC_State = STATE_IDLE;
   remote_receiver_ready = TRUE;
   
   ds_ucsd_UplinkPDUMgr_Initialise   (CurrentPDUType, TRUE);
   ds_ucsd_DownlinkPDUMgr_Initialise (CurrentPDUType, TRUE);

   PolledDueToSREJFailure = FALSE;
#ifdef ENHANCED_POLL_RECOVERY
   AcksInSequence = TRUE;
#endif
   LastPollNr = -1;
   RepeatedPollResponse = FALSE;
   Negotiate_XID_ADM_Values = FALSE;
}


/****************************************************************************
 * Function name:  SendUI
 * -------------
 * Description:    This routine sends a UI frame if required.
 * -----------
 * Returns:        RLP_ERROR - Error Occurred
 * -------         0         - No Frames Sent
 *                 >0        - Frame Sent OK
 ****************************************************************************/
static short SendUI (void)
{
   short RetVal = RLP_OK;
   short status = RLP_OK;

   if (UI_State == STATE_SEND)
   {
      status = SendUnNumberedFrame (UI, TRUE, FALSE, TxUIData);
      TxUIData = NULL;
      
      if (status == RLP_OK)
      {
         UI_State = STATE_IDLE;
         RetVal = 1;
      }
      else
      {
         RetVal = RLP_ERROR;
      }
   }
   
   return RetVal;
}

/****************************************************************************
 * Function name:  Send_TXU
 * -------------
 * Description:    This routine sends any required TEST, XID or UI frames.
 * -----------
 * Returns:        0         - No Frames Sent
 * -------         RLP_ERROR - Error Occurred
 *                 >0        - Frame Sent OK
 ****************************************************************************/
short Send_TXU (void)
{
   short RetVal = RLP_OK;
   short status = RLP_OK;
   
   /* Test Response to send */
   if (Test_R_State == STATE_SEND)
   {
      status = SendUnNumberedFrame (_TEST, FALSE, Test_R_FBit, Test_R_Data);
      Test_R_Data = NULL;
      
      if (status == RLP_OK)
      {
         Test_R_State = STATE_IDLE;
         RetVal = 1;
      }
   }
   /* XID Response to send */
   else if (XID_R_State == STATE_SEND)
   {
      DS_UCSD_DEBUG_MSG ("RLP: Sending XID response");
      status = SendUnNumberedFrame (XID, FALSE, TRUE, TxXIDData);
      TxXIDData = NULL;
      
      if (status == RLP_OK)
      {
         XID_R_State = STATE_IDLE;
         RetVal = 1;
      }
   }
   /* Test Command to send */
   else if (Test_C_State == STATE_SEND)
   {
      if ((Test_C_PBit) && (Poll_xchg == STATE_WAIT))
      {
         RetVal = SendUI();
      }
      else
      {
         status = SendUnNumberedFrame (_TEST, TRUE, Test_C_PBit, Test_C_Data);
         Test_C_Data = NULL;
         
         if (status == RLP_OK)
         {
            Test_Count++;
            Test_C_State = STATE_WAIT;
            //MSG_LOW ("Sending TEST - T1 Timeout: %d",T1ValueInMs,0,0);
            START_TIMER (TIMERID_T1, T1ValueInMs);
            RetVal = 1;
            
            if (Test_C_PBit)
            {
               Poll_xchg = STATE_WAIT;
            }
         }
      }
   }
   /* XID Command to send */
   else if (XID_C_State == STATE_SEND)
   {
      if (Poll_xchg == STATE_WAIT)
      {
         RetVal = SendUI ();
      }
      else
      {
         //MSG_LOW ("Sending XID parameters",0,0,0);
         
         /* Allocate PDU for XID Frame */
         if (NULL != TxXIDData)
         {
            ds_ucsd_UplinkPDUMgr_Free(&TxXIDData);
         }
         TxXIDData = ds_ucsd_UplinkPDUMgr_New();
         
         /* Send the required XID parameters */
         XID_to_PDU (&TxXIDData, &NewXID);
         status = SendUnNumberedFrame (XID, TRUE, TRUE, TxXIDData);
         TxXIDData = NULL;
         
         DebugXID (&NewXID, 0);
         
         if (status == RLP_OK)
         {
            XID_Count++;
            XID_C_State = STATE_WAIT;
            Poll_xchg   = STATE_WAIT;
            START_TIMER (TIMERID_TXID, T1ValueInMs);
            
            RetVal = 1;
         }
      }
   }
   else 
   {
      RetVal = SendUI ();
   }
   
   
   if (status == RLP_ERROR)
      RetVal = RLP_ERROR;
   
   return RetVal;
}

/****************************************************************************
 * Function name:  SendData
 * -------------
 * Description:    This routine is called in the Connection Established state.
 * -----------     It sends an I+S-Frame or a S-Frame (Command or Response).
 * Parameters:     None
 * ----------
 * Returns:        RLP_OK    - No Frames Sent
 * -------         RLP_ERROR - Error Occurred
 ****************************************************************************/
static short SendData (void)
{
   short status = RLP_OK;
   short SREJIndex;
   
   if (UA_State == STATE_SEND)
   {
      status   = SendUnNumberedFrame (UA, FALSE, UA_FBit, NULL);
      UA_State = STATE_IDLE; 
   }
   else
   {
     if (Ackn_FBit == FALSE)
     {
       if (REJ_State == STATE_SEND)
       {
         status = Send_REJ_or_REJ_I_Cmd ();
       }
       else
       {
         /* Recovery mode is selective reject */
         if ( SREJisSuitable() )
         {  
           SREJIndex = ds_ucsd_DownlinkPDUMgr_CheckSREJToSend();
            
           /* IF SREJ to send */
           if (SREJIndex != RLP_ERROR)
           {
             DS_UCSD_DEBUG_MSG_1 ("RLP: Tx SREJ(%d)", SREJIndex);
             status = Send_SREJ_or_SREJ_I_Cmd (SREJIndex);
           } 
           else
           {
             status = SendCommand ();
           }
         }
         else
         {
           /* Send a packet as a command (C=1) */
           status = SendCommand ();
         }
       }
     }
     else
     {
       /* We must send a Response (C=0) to a poll from the other end */
       status = SendResponse ();
     }
   }
   
   return status;
}

/****************************************************************************
 * Function name:  SendCommand
 * -------------
 * Description:    This routine is called from SendData.
 * -----------     It sends either an I+SFrame or SFrame as a Command.
 * Parameters:     None
 * ----------
 * Returns:        RLP_OK    - No Frames Sent
 * -------         RLP_ERROR - Error Occurred
 ****************************************************************************/
static short SendCommand (void)
{
   short status = RLP_OK;
   word Frame;
   short SREJIndex;
   dsm_item_type * PDU = NULL;
  boolean remote_ready = FALSE;
   short VR = ds_ucsd_DownlinkPDUMgr_VR ();
   short VS = ds_ucsd_UplinkPDUMgr_VS ();
  
   if (ds_ucsd_DownlinkPDUMgr_ReceiveListFull()) /**  || SendRNRDuringRecovery)**/
   {
      Frame = RNR;
   }
   else
   {
      Frame = RR;
   }
   
   /* IF a Poll is required AND no previous Poll is outstanding */
   if ((Poll_State == STATE_SEND) && (Poll_xchg == STATE_IDLE))
   {
      /* Perform a Poll for recovery */
      status = SendNumberedFrame (Frame, TRUE, TRUE, (word)VR, 0, NULL);
      
      Poll_Count++;
      Poll_State = STATE_WAIT;
      Poll_xchg  = STATE_WAIT;
      //MSG_LOW ("Sending POLL - T1 Timeout: %d  Frame: %d",T1ValueInMs,Frame,0);
      START_TIMER (TIMERID_T1, T1ValueInMs);
   }
   else /* Send an I frame or a supervisory frame */
   {
    /* Check flow control conditions for I frame */
    remote_ready = ( remote_receiver_ready &&
                     !(RLP_Tx_Info_Frame && ds_ucsd_remote_fc_enabled()) );
     
      /* Send an I-Frame due to SREJ from remote node */
    if ( remote_ready && 
         ((SREJIndex = ds_ucsd_UplinkPDUMgr_SREJDataToTransmit())!= RLP_ERROR ))
      {
         DS_UCSD_DEBUG_MSG_1 ("RLP: Tx By SREJ=%d!", SREJIndex);

         PDU = ds_ucsd_UplinkPDUMgr_ReadOutOfSeq (SREJIndex);
         status = SendNumberedFrame (Frame, TRUE, FALSE, (word)VR,
                                     (word)SREJIndex, PDU);
         PDU = NULL;
      } 
    else if ( remote_ready &&
              ds_ucsd_UplinkPDUMgr_TransmitWindowOpen() &&
              ds_ucsd_UplinkPDUMgr_DataToSend() )
      {
         /* IF Data to Send, Send the next I-Frame in sequence */
         PDU = ds_ucsd_UplinkPDUMgr_Read ();
         if (PDU)
         {
            status = SendNumberedFrame (Frame, TRUE, FALSE, (word)VR,
                                        (word)VS, PDU);
            PDU = NULL;
            
            /* Start the timer for poll recovery */
#ifdef ENHANCED_POLL_RECOVERY
            if ((AcksInSequence)||(TIMER_RUNNING(TIMERID_T1)==FALSE))
#endif
               START_TIMER (TIMERID_T1, T1ValueInMs);
         }
      }
      else /* Send an RR (or RNR) */
      {
         status = SendNumberedFrame (Frame, TRUE, FALSE, (word)VR, 0, NULL);
      }
   }
   
   return status;
}

/****************************************************************************
 * Function name:  SendResponse
 * -------------
 * Description:    This routine is called from SendData.
 * -----------     It sends a frame in response to a poll from the remote 
 *                 node.
 * Parameters:     None
 * ----------
 * Returns:        RLP_OK    - No Frames Sent
 * -------         RLP_ERROR - Error Occurred
 ****************************************************************************/
static short SendResponse (void)
{
   short status = RLP_OK;
   word Frame;
   short VR;
#ifdef ALLOW_FBIT_IFRAME_RESPONSE
  boolean remote_ready = FALSE;
   dsm_item_type * PDU = NULL;
   short Length;
   short SREJIndex;
   short VS = ds_ucsd_UplinkPDUMgr_VS ();
#endif
   
   VR = ds_ucsd_DownlinkPDUMgr_VR ();

   if (ds_ucsd_DownlinkPDUMgr_ReceiveListFull()) /** || SendRNRDuringRecovery) **/
   {
      Frame = RNR;
   }
   else
   {
      Frame = RR;
   }
   
#ifdef ALLOW_FBIT_IFRAME_RESPONSE
   /* The ETSI 3GPP 04.22 RLP spec says that an I-Frame response CAN have F=1, 
      BUT then the ETSI Test Spec 11.10 says it cannot! (Test 29.3.2.6.1),
      so remove this code with a conditional compile */

  /* Check flow control conditions for I frame */
  remote_ready = ( remote_receiver_ready &&
                   !(RLP_Tx_Info_Frame && ds_ucsd_remote_fc_enabled()) );

   /* Send an I-Frame due to an SREJ from remote node */
  if ( remote_ready && 
       ((SREJIndex = ds_ucsd_UplinkPDUMgr_SREJDataToTransmit())!= RLP_ERROR ) )
   {
      PDU = ds_ucsd_UplinkPDUMgr_ReadOutOfSeq (SREJIndex);
      status = SendNumberedFrame (Frame, FALSE, TRUE, VR, SREJIndex, PDU);
      PDU = NULL;
   } 
  else if ( remote_ready &&
            ds_ucsd_UplinkPDUMgr_TransmitWindowOpen() &&
            ds_ucsd_UplinkPDUMgr_DataToSend() )
   {
      /* IF Data to Send, Send the next I-Frame in sequence */
      PDU = ds_ucsd_UplinkPDUMgr_Read ();
      if (PDU)
      {
         status = SendNumberedFrame (Frame, FALSE, TRUE, VR, VS, PDU);
         PDU = NULL;
      
         Ackn_FBit = FALSE;
         
         /* Start the timer for poll recovery */
#ifdef ENHANCED_POLL_RECOVERY
         if ((AcksInSequence) ||(TIMER_RUNNING(TIMERID_T1)==FALSE))
#endif
            START_TIMER (TIMERID_T1, T1ValueInMs);
         //MSG_LOW ("Sending Frame - T1 Timeout: %d  Frame: %d",T1ValueInMs,Frame,0);
      }
   }
   else
#endif /* ALLOW_FBIT_IFRAME_RESPONSE */
      
   {
      /* Send an RR (or RNR) Response */
      status = SendNumberedFrame (Frame, FALSE, TRUE, (word)VR, 0, NULL);
      Ackn_FBit = FALSE;
   }
   
   return status;
}

/****************************************************************************
 * Function name:  Send_REJ_or_REJ_I_Cmd
 * -------------
 * Description:    This routine sends a REJ frame. This may be piggy-backed
 * -----------     with data (REJ_I).
 * Parameters:     None
 * ----------
 * Returns:        status
 * -------
 ****************************************************************************/
static short Send_REJ_or_REJ_I_Cmd (void)
{
   short status = RLP_OK;
   short SREJIndex;
   dsm_item_type * PDU = NULL;
   boolean remote_ready = FALSE;
   short VR = ds_ucsd_DownlinkPDUMgr_VR ();
   short VS = ds_ucsd_UplinkPDUMgr_VS ();
   
   if ((Poll_xchg != STATE_WAIT) && (Poll_State == STATE_SEND))
   {
      /* Send a REJ frame with Poll=1 */
      status = SendNumberedFrame (REJ, TRUE, TRUE, (word)VR, 0, NULL);
      
      REJ_State = STATE_WAIT;
      
      START_TIMER (TIMERID_TRCVR, T1ValueInMs);
      START_TIMER (TIMERID_T1,    T1ValueInMs);
      //MSG_LOW ("Sending REJ - T1 Timeout: %d",T1ValueInMs,0,0);
      
      Poll_Count++;
      Poll_State = STATE_WAIT;
      Poll_xchg  = STATE_WAIT;
   }
   else
   {
      /* Check flow control conditions for I frame */
      remote_ready = ( remote_receiver_ready &&
                        !(RLP_Tx_Info_Frame && ds_ucsd_remote_fc_enabled()) );

      /* Send an I-Frame due to an SREJ from remote node */
      if ( remote_ready &&
           (SREJIndex = ds_ucsd_UplinkPDUMgr_SREJDataToTransmit()) !=
            RLP_ERROR )
      {
         PDU = ds_ucsd_UplinkPDUMgr_ReadOutOfSeq (SREJIndex);
         status = SendNumberedFrame (REJ, TRUE, FALSE, (word)VR, (word)SREJIndex, PDU);
         PDU = NULL;
      } 
      else if ( remote_ready &&
                ds_ucsd_UplinkPDUMgr_TransmitWindowOpen() &&
                 ds_ucsd_UplinkPDUMgr_DataToSend() )
      {
         /* IF Data to Send, Send the next I-Frame in sequence */
         PDU = ds_ucsd_UplinkPDUMgr_Read ();
         if (PDU)
         {
            status = SendNumberedFrame (REJ, TRUE, FALSE, (word)VR, (word)VS, PDU);
            PDU = NULL;
            
            REJ_State = STATE_WAIT;
            START_TIMER (TIMERID_TRCVR, T1ValueInMs);
            START_TIMER (TIMERID_T1,    T1ValueInMs);
            //MSG_LOW ("Sending REJ - T1 Timeout: %d",T1ValueInMs,0,0);
         }
      }
      else
      {
         /* Send a REJ frame */
         status = SendNumberedFrame (REJ, TRUE, FALSE, (word)VR, 0, NULL);
         REJ_State = STATE_WAIT;
         START_TIMER (TIMERID_TRCVR, T1ValueInMs);
      }
   }
   
   return status;
}

/****************************************************************************
 * Function name:  Send_SREJ_or_SREJ_I_Cmd
 * -------------
 * Description:    This routine sends a SREJ Frame. This maybe piggy-backed
 * -----------     with data (SREJ_I).
 * Parameters:     SREJ Nr to send
 * ----------
 * Returns:        status
 * -------
 ****************************************************************************/
static short Send_SREJ_or_SREJ_I_Cmd (short SREJ_Nr)
{
   short status = RLP_OK;
   short SREJIndex; /* For sending data in response to
                       SREJ from the OTHER end!! */
   dsm_item_type * PDU = NULL;
   boolean remote_ready = FALSE;
   short VS = ds_ucsd_UplinkPDUMgr_VS ();

   if ((Poll_xchg != STATE_WAIT) && (Poll_State == STATE_SEND))
   {
      /* Send a SREJ frame with Poll=1 */
      status = SendNumberedFrame (SREJ, TRUE, TRUE, (word)SREJ_Nr, 0, NULL);
      ds_ucsd_DownlinkPDUMgr_SetState (SREJ_Nr, PDU_STATE_WAIT);

      START_TIMER (TIMERID_TRCVR, T1ValueInMs);       
      START_TIMER (TIMERID_T1,    T1ValueInMs);
      //MSG_LOW ("Sending SREJ - T1 Timeout: %d",T1ValueInMs,0,0);

      Poll_Count++;
      Poll_State = STATE_WAIT;
      Poll_xchg  = STATE_WAIT;
   }
   else
   {
       /* Check flow control conditions for I frame */
       remote_ready = ( remote_receiver_ready &&
                        !(RLP_Tx_Info_Frame && ds_ucsd_remote_fc_enabled()) );

      /* This is to see if we must send data in response to 
         an SREJ from the remote node! (i.e. this is in addition
         to sending an SREJ(I) frame ourselves). */
       if ( remote_ready &&
            (SREJIndex = ds_ucsd_UplinkPDUMgr_SREJDataToTransmit()) !=
            RLP_ERROR )
       {
         PDU = ds_ucsd_UplinkPDUMgr_ReadOutOfSeq (SREJIndex);
         status = SendNumberedFrame (SREJ, TRUE, FALSE,
                                     (word)SREJ_Nr, (word)SREJIndex, PDU);
         PDU = NULL;
       }
       else if ( remote_ready &&
                 ds_ucsd_UplinkPDUMgr_TransmitWindowOpen() &&
                 ds_ucsd_UplinkPDUMgr_DataToSend() )
       {
         /* IF Data to Send, Send the next I-Frame in sequence */
         PDU = ds_ucsd_UplinkPDUMgr_Read ();
         if (PDU)
         {
            status = SendNumberedFrame (SREJ, TRUE, FALSE,
                                        (word)SREJ_Nr, (word)VS, PDU);
            PDU = NULL;
            
            ds_ucsd_DownlinkPDUMgr_SetState (SREJ_Nr, PDU_STATE_WAIT);
            
            START_TIMER (TIMERID_TRCVR, T1ValueInMs);       
            START_TIMER (TIMERID_T1,    T1ValueInMs);       
            //MSG_LOW ("Sending SREJ - T1 Timeout: %d",T1ValueInMs,0,0);
         }
       }
       else
       {
         /* Send a SREJ frame */
         status = SendNumberedFrame (SREJ, TRUE, FALSE, (word)SREJ_Nr, 0, NULL);
         ds_ucsd_DownlinkPDUMgr_SetState (SREJ_Nr, PDU_STATE_WAIT);
         START_TIMER (TIMERID_TRCVR, T1ValueInMs);       
       }
   }
   
   return status;
}

/****************************************************************************
 * Function name:  SREJisSuitable
 * -------------
 * Description:    This routine returns whether or not to send a SREJ.
 * -----------     It returns TRUE if only one frame went missing.
 *                 Any more than that and it is more efficient to use
 *                 a REJ recovery.
 * Parameters:     None
 * ----------
 * Returns:        TRUE/FALSE
 * -------         
 ****************************************************************************/
boolean SREJisSuitable(void)
{
   //register boolean RetVal = FALSE;
   //register short Gap;


   /* This routine needs further work. The idea of
      detecting a gap of 1 is sound, but the method isn't!
      For the time-being, always use SREJ if required */
   return RecoveryUsesSREJ;

   /*********TODO!!!*************
   Gap = Ns-ds_ucsd_DownlinkPDUMgr_VR();
   if (Gap < 0)
   {
      Gap += RLP_M;
   }

   if (Gap == 1)
   {
      RetVal = TRUE;
   }
   else
   {
      DS_UCSD_DEBUG_MSG ("RLP:SREJ not suitable");
   }
   
   return RetVal;
    *********TODO!!!*************/
}


/****************************************************************************
 * Function name:  REJisSuitable
 * -------------
 * Description:    This routine returns whether or not to send a REJ.
 * -----------     It is necessary because the RACAL 6103 sends a 
 *                 response to a REJ TWICE(!) - so the second time we don't
 *                 want to send another REJ, just discard the frame(s).
 *
 * Returns:        TRUE/FALSE
 * -------         
 ****************************************************************************/
static boolean REJisSuitable(void)
{
   //register boolean RetVal = FALSE;
   //register short Gap;

   /************************************************************************
      IF the received frame is a few ahead of the current VR,
      then a REJ recovery IS suitable. This prevents a REJ being sent
      if we receive a Ns twice in rapid succession (like the Racal sends!)

      For example:
      Rx: RR_I  Ns=1 
      Tx: RR    Nr=2
      Rx: RR_I  Ns=2 
      Tx: RR    Nr=3
      Rx: RR_I  Ns=4  Where is frame 3?, send a REJ 
      Tx: REJ   Nr=3
      Rx: RR_I  Ns=3  ok, there it is
      Tx: RR    Ns=4
      Rx: RR_I  Ns=4  
      Tx: RR    Ns=5
      Rx: RR_I  Ns=5 
      Tx: RR    Ns=6
      Rx: RR_I  Ns=3  ?? There's 3 again (from Racal 6103) - Discard it.
      Tx: RR    Nr=6  !NOT: REJ Nr=6!
   ************************************************************************/

   //Gap = Ns-ds_ucsd_DownlinkPDUMgr_VR();
   //if (Gap < 0)
   //{
   //   Gap += RLP_M;
   //}

   //if (Gap < 10)
   //{
   //   RetVal = TRUE;
   //}
   //else
   //{
   //   DS_UCSD_DEBUG_MSG ("RLP: REJ not suitable");
   //}
   
   //return RetVal;
   return TRUE;
}

/****************************************************************************
 * Function name:  I_Handler
 * -------------
 * Description:    This routine handles the Information part of an I+S Frame
 * -----------
 * Parameters:     None
 * ----------
 * Returns:        None
 * -------         
 ****************************************************************************/
static void I_handler (void)
{
   boolean Discarded = TRUE;
   boolean use_srej = SREJisSuitable();


#ifdef DONT_ALLOW_PBIT_IFRAME
   /* The ETSI 04.22 RLP spec says that an I-Frame cannot have P=1,
      BUT then the ETSI Test Spec 11.10 says it can! (Test 29.3.2.6.1).
      So as default, don't send them, but receive them ok. */
   if ((CR) && (PF))
   {
      /* Improper Frame */
   }
   else
#endif  /* DONT_ALLOW_PBIT_IFRAME */

   {
     if (ds_ucsd_DownlinkPDUMgr_ReceiveWindowOpen(Ns))
     {
       /* If frame is correctly in sequence */
       if (Ns == ds_ucsd_DownlinkPDUMgr_VR())
       {
         /* Fill the receive list entry */
         if ( use_srej ||
              ds_ucsd_DownlinkPDUMgr_GetState (Ns) == PDU_STATE_IDLE) /* v2.3 */
         {
           /** SendRNRDuringRecovery = FALSE; **/
           ds_ucsd_DownlinkPDUMgr_Write (&RxFrame->Data);

           REJ_State = STATE_IDLE;
           STOP_TIMER (TIMERID_TRCVR);   
           Discarded = FALSE;
           if ( use_srej )
           {
             PolledDueToSREJFailure = FALSE;
           }
         }
         else
         {
           DS_UCSD_DEBUG_MSG ("RLP: RxListFull!");             
         }
       }
       else
       {
         /* Prevent any further I frames until all ok again 
            SendRNRDuringRecovery = TRUE;   !!Removed - V2.8!!
         *************************************************/

            
         DS_UCSD_DEBUG_MSG_1 ("RLP: o.o.s I: Ns=%d", Ns);
            
         /* Reaching here means that an I frame has been received
            out of sequence. Perform a recovery if required */
            
         /* Do SREJ recovery if we need it */
         if ( use_srej  && (PolledDueToSREJFailure == FALSE) )
         {              
           //DS_UCSD_DEBUG_MSG ("RLP:o.o.s IFrame!");
           /* Store the out-of-sequence I Frame and flag as missing 
              the frames in between. These will be requested using 
              an SREJ */
           ds_ucsd_DownlinkPDUMgr_WriteOutOfSeq (&RxFrame->Data, Ns);
           Discarded = FALSE;
         }
         else

         /* TMR: Add REJ recovery for 11.10 29.3.3.2 & 29.3.3.3 */ 
         {
           /* ELSE do REJ recovery if we need it */
           if ( REJisSuitable() && (REJ_State == STATE_IDLE) )
           {

             DS_UCSD_DEBUG_MSG_1 ("RLP: REJ STATE_SEND: VR=%d",
                                  ds_ucsd_DownlinkPDUMgr_VR());
             REJ_State = STATE_SEND;
           }
         }


         /********************************************************
             (In fact the link will recover withour either, thanks to
             timeout checkpoint recovery from the other end...) 
         ********************************************************/
       }
     }
     else
     {
       DS_UCSD_DEBUG_MSG ("RLP:RxWnd closed!");         
     }
   }
   
   if (Discarded)
   {
     DS_UCSD_DEBUG_MSG_1 ("RLP: Discard I: Ns=%d", Ns);
   }
}

/****************************************************************************
 * Function name:  S_Handler
 * -------------
 * Description:    This routine handles the Supervisory part of an I+S Frame
 * -----------
 * Parameters:     None
 * ----------
 * Returns:        None
 * -------         
 ****************************************************************************/
static void S_handler (void)
{
   boolean DiscardFrame = FALSE;

   /* We have been polled for our latest status */
   if ((CR) && (PF))
   {
      DS_UCSD_DEBUG_MSG ("RLP:Ackn_F=1");
      /* Respond to a Poll */
      Ackn_FBit = TRUE;
      
      /* Getting here means that the other end has been sending us I frames,
         but it hasn't received our Acks. Alternatively, I frames that
         it sent have been lost, so we haven't rx'd them or ack'd them.
         Either way, it has polled us for our latest VR and VS.
         This means that it will send again from our VR, so we can delete
         any data that has been received, but not Ack'd.
         BUT! This will only occur if we are using SREJ recovery, because
         this is the only way to accept a frame that is out of sequence.
         In this case, we can delete frames from VR to VE-1. Any frames
         between VE and VR-1 are ready to be read by the higher layer
         and should be left alone! */
      
      if ( SREJisSuitable() )
      {
        ds_ucsd_DownlinkPDUMgr_DeleteUnAcknowledgedFrames ();
      
        /* This flag prevents the further storage of out-of-sequence
           frames while the Poll checkpoint recovery is taking place. */
        PolledDueToSREJFailure = TRUE;
      }
      
      /* Stop the REJ timer */
      STOP_TIMER (TIMERID_TRCVR);
   }
   
   
   
   if (Poll_State != STATE_IDLE)
   {
      /* Ignore SREJ and REJ frames during a poll cycle */
      if ((SF != SREJ) && (SF != REJ))
      {
         /* IF this is a response to our Poll */
         if ((PF) && (CR == FALSE))
         {
            ds_ucsd_UplinkPDUMgr_DecreaseVStoNR (Nr);
            
            /* For 29.3.2.6.7 & 3.2.6.9 cetecom tests*/
            /* It seems they want N2 retries for a bad seq. of responses,
               but N2+1 retries for NO responses... */
            if (Poll_Count == 1) Poll_Count++; 
            Poll_State = STATE_IDLE;
            Poll_xchg  = STATE_IDLE;
            
            
            /* Deal with repeated Poll Responses */
            if (RepeatedPollResponse == FALSE)
            {
               if (LastPollNr == Nr)
               {
                  DS_UCSD_DEBUG_MSG ("RLP:RepeatedPollResponse->TRUE");
                  RepeatedPollResponse = TRUE;
               }
               LastPollNr = Nr;
            }
            else
            {
               if (LastPollNr != Nr)
               {
                  DS_UCSD_DEBUG_MSG ("RLP:RepeatedPollResponse->FALSE");
                  RepeatedPollResponse = FALSE;
                  LastPollNr = -1;
               }
            }
         }
         else
         {
            DS_UCSD_DEBUG_MSG ("RLP:Discard F=0 in Poll!");
            DiscardFrame = TRUE;
         }
      }
      else
      {
         DS_UCSD_DEBUG_MSG ("RLP:Discard (S)REJ in Poll!");
         DiscardFrame = TRUE;
      }
   }
   
   if (DiscardFrame == FALSE)
   {
      switch (SF)
      {
         case RR:
            Advance_VA_to_NR ();
            if (!remote_receiver_ready)
            {
               DS_UCSD_DEBUG_MSG ("RLP:remote_receiver_rdy=TRUE");
               remote_receiver_ready = TRUE;
            }
            break;
            
         case RNR:
            Advance_VA_to_NR ();
            if (remote_receiver_ready)
            {
               DS_UCSD_DEBUG_MSG ("RLP:remote_receiver_rdy=FALSE");
               remote_receiver_ready = FALSE;
            }
            break;
            
         case REJ:
            DS_UCSD_DEBUG_MSG_1 ("REJ rxd, Nr:%d",Nr);
               
            Advance_VA_to_NR ();
            remote_receiver_ready = TRUE;
            ds_ucsd_UplinkPDUMgr_DecreaseVStoNR (Nr);
            break;
            
         case SREJ:
            /* Mark the frame for re-transmission */
            ds_ucsd_UplinkPDUMgr_MarkForRetransmission (Nr);
            break;

         default:
            break;       /* Making Lint happy */
      }
      
      if (SF != SREJ)
      {
         /* IF all frames are acknowleged stop timer */
         if (ds_ucsd_UplinkPDUMgr_AllFramesAcknowledged())
            /* && (PollResponse==FALSE))  cetecom!!*/
         {
            STOP_TIMER (TIMERID_T1);
         }
      }
   }
}


/****************************************************************************
 * Function name:  Advance_VA_to_NR 
 * -------------
 *
 * Description:    This routine advances the value of VA upto the latest
 * -----------     valid received Nr. This effectively acknowledges the 
 *                 frames.
 * Parameters:     None
 * ----------
 * Returns:        None
 * -------
 ****************************************************************************/
static void Advance_VA_to_NR (void)
{
   short VA = ds_ucsd_UplinkPDUMgr_VA ();

   
   /*DS_UCSD_DEBUG_MSG_2 ("In:Advance_VA_to_NR: VA:%d Nr:%d",VA,Nr);*/

   ds_ucsd_UplinkPDUMgr_AdvanceVAtoNR (Nr);

   /* For 29.3.2.6.7/9 cetecom tests!! */
   /* Reset the Poll_Count if original WHOLE sequence is all ack'd. */
   if (Poll_Count)
   {
      while (VA != Nr)
      {
         VA = INC_SEQ_NUM (VA);

         if (VA == VS_at_first_poll)
         {
            DS_UCSD_DEBUG_MSG ("RLP:pc=0");
            Poll_Count = 0;
         }
      }
   }
   
   
   /* If we have had a repeated Poll Response, but now the Acks are
      OK again, reset the Repeated Poll variables. This mechanism is
      necessary to deal with multiple responses to a Poll (the like
      of which are sent by the racal 6103!). */
   if (RepeatedPollResponse)
   {
      if (LastPollNr != Nr)
      {
         DS_UCSD_DEBUG_MSG ("RLP:RepeatedPollResponse=0 (2)");
         RepeatedPollResponse = FALSE;
         LastPollNr = -1;
      }
   }
}

/****************************************************************************
 * Function name:  HandleUI
 * -------------
 * Description:    This routine buffers UI data. This data is not queued,
 * -----------     and will be overwritten if more UI data arrives before
 *                 the higher layer reads it.
 * Parameters:     ptr to RLP Frame
 * ----------
 * Returns:        None
 * -------         
 ****************************************************************************/
void HandleUI (RLP_Frame_T* Frame)
{
   if(Frame == NULL)
   {
     DATA_MSG0_ERROR("NULL ptr input to HandleUI()");
     return;
   }

   /* Purge previous UI data */
   if (NULL != RxUIData)
   {
      ds_ucsd_DownlinkPDUMgr_Free (&RxUIData);
   }
   
   /* Transfer Frame data to local buffer */
   RxUIData = Frame->Data;
   Frame->Data = NULL;

   WRITE_EVENT_TO_L2R (RLP_UI_IND);
}

/****************************************************************************
 * Function name:  SupervisoryNrIsLegal
 * -------------
 * Description:    This routine returns TRUE if the received Nr of
 * -----------     the received Supervisory Frame is within range.
 * Parameters:     None
 * ----------
 * Returns:        TRUE = In Range/FALSE = Out Of Range
 * -------
 ****************************************************************************/
static boolean SupervisoryNrIsLegal (void)
{
   boolean RetVal = FALSE;
   short NrMinus1;
   
#ifdef ENHANCED_POLL_RECOVERY
   AcksInSequence = FALSE;
#endif
   NrMinus1 = DEC_SEQ_NUM(Nr);
   
   /* IF the Nr of the received Supervisory Frame refers to
    * an un-acknowledged frame then it is legal, OR
    * if it is the latest one repeated (VS) */
   if ((ds_ucsd_UplinkPDUMgr_State (NrMinus1) == PDU_STATE_WAIT) ||
       (Nr == ds_ucsd_UplinkPDUMgr_VS()))
   {
      
      
     /* DS_UCSD_DEBUG_MSG_1 ("Nr ok! Nr=%d", Nr);*/

#ifdef ENHANCED_POLL_RECOVERY
      AcksInSequence = TRUE;
#endif
      RetVal = TRUE;
   }
   /* ALTERNATIVELY, this maybe a response to our poll, so it may not
      be a bad Nr after all! */
   else if ((Poll_State == STATE_WAIT) && (CR==FALSE) && (PF))
   {
      RetVal = TRUE;
   }
   /* Or this is a poll */
   else if ((CR) && (PF))
   {
      RetVal = TRUE;
   }
   /* Or indeed a REJ or SREJ will have an unexpected Nr as well */
   else if ((SF == REJ) || (SF == SREJ))
   {
      RetVal = TRUE;
   }
   
   return RetVal;
}

/****************************************************************************
 * Function name:  Disconnect
 * -------------
 * Description:    Send a DISC and change state to DISC_INITIATED
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
void Disconnect (void)
{
   STOP_TIMER (TIMERID_T1);
   DISC_State = STATE_SEND;
   DISC_Count = 0;
   
   if (Poll_xchg == STATE_IDLE)
   {
      DISC_PBit = TRUE;
   }
   else
   {
      DISC_PBit = FALSE;
   }
   RlpCurrentState = RLPSTATE_DISC_INITIATED;
}

/****************************************************************************
 * Function name:  PDU_to_XID
 * -------------
 * Description:    Converts an XID frame data into an XID parameter structure.
 * -----------     
 * Uses:
 * ----
 * Parameters:     Frame Data ptr, XID parameter structure ptr.
 * ----------
 * Returns:        TRUE if required types decoded; FALSE otherwise 
 * --------
 ****************************************************************************/
static boolean PDU_to_XID (dsm_item_type* PDU, XID_Params_T* Xid)
{
   boolean Loop = TRUE;
   boolean result = TRUE;
   short   Index = 0;
   byte    PDUByte;
   byte    Data[RLP_PDU_MAX_USER_DATA_SIZE];
   byte    Type;
   byte    Length;

   if(Xid == NULL)
   {
     DS_UCSD_DEBUG_MSG ("XID ptr is Null in PDU_to_XID()");
     return FALSE;
   }

   /* Copy packet to working buffer */
   DSM_EXTRACT (PDU, 0, Data, XIDsize);  /*lint !e666 */
   
   while ((Loop) && (Index <  XIDsize))
   {
      PDUByte = Data[Index++];
      
      Type   = (byte)((PDUByte)>>4);
      Length = PDUByte & 0x0f;

      switch (Type)
      {
         case 0:
            Loop = FALSE;
            break;
            
         case 1:            
            Xid->RLPVersion = Data[Index++];
            break;
            
         case 2:
            Xid->IWFtoMSWindow = Data[Index++];   
            
            if (Xid->RLPVersion >= 2)
               Xid->IWFtoMSWindow *= 8;
            break;
            
         case 3:
            Xid->MStoIWFWindow = Data[Index++];
            
            if (Xid->RLPVersion >= 2)
               Xid->MStoIWFWindow *= 8;
            break;
            
         case 4:
            PDUByte = Data[Index++];   
            if (PDUByte < XIDMIN_T1_12KB)
               PDUByte = XIDMIN_T1_12KB;
            Xid->T1 = PDUByte;
            break;
                                
         case 5:
            Xid->N2 = Data[Index++];
            break;

         case 6:
            PDUByte = Data[Index++];
            Xid->T2 = PDUByte;
            break;

         case 7:
            if (Negotiate_XID_ADM_Values)
            {
               PDUByte = Data[Index++];
               Xid->Pt = (PDUByte)>>4;
               Xid->P0 =  PDUByte & 0x03;

               PDUByte = Data[Index++];
               Xid->P1 =  PDUByte;
               PDUByte = Data[Index++];
               Xid->P1 +=  PDUByte*256;
               
               PDUByte = Data[Index++];
               Xid->P2 = PDUByte;
            }
            else
            {
               Index+=Length;
            }
            break;

         case 8:
            PDUByte = Data[Index++];
            Xid->T4 = (byte)(PDUByte * 10);   
            break;
         
         /* Ignore optional features... we will always set to 0 in 
         ** XID response 
         */
         case 9:
            Index++;
            break;
              
         default:
            break;       /* Making Lint happy */
      }
   }

   return result;
}

/****************************************************************************
 * Function name:  XID_to_PDU
 * -------------
 * Description:    Builds a RLP XID frame from the specified XID data 
 * -----------     structure.
 * Parameters:     Data ptr, XID structure ptr
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
void XID_to_PDU (dsm_item_type** PDU_ptr, const XID_Params_T* Xid)
{
   byte PDUByte;
   short Index = 0;
   byte Data[RLP_PDU_MAX_USER_DATA_SIZE];

   if((Xid == NULL) || (PDU_ptr == NULL))
   {
     ERR ("Ptr to PDU %p or XID struct %p is NULL",PDU_ptr,Xid,0);
     return;  
   }

   /* Version number */
   Data[Index++] = 0x11;
   PDUByte = Xid->RLPVersion;
   Data[Index++] = PDUByte;

   /* IWF to MS window size */
   Data[Index++] = 0x21;
   if (Xid->RLPVersion >= 2)
   {
      PDUByte = (byte)((Xid->IWFtoMSWindow)/8);
   }
   else
   {
      PDUByte = (byte)(Xid->IWFtoMSWindow);
   }
   Data[Index++] = PDUByte;


   /* MS to IWF window size */
   Data[Index++] = 0x31;
   if (Xid->RLPVersion >= 2)
   {
      PDUByte = (byte)((Xid->MStoIWFWindow)/8);
   }
   else
   {
      PDUByte = (byte)(Xid->MStoIWFWindow);
   }
   Data[Index++] = PDUByte;
   
   /* T1 */
   Data[Index++] = 0x41;
   PDUByte = (byte)((Xid->T1));
   Data[Index++] = PDUByte;

   /* N2 */
   Data[Index++] = 0x51;
   Data[Index++] = Xid->N2;

   /* T2 */
   Data[Index++] = 0x61;
   PDUByte = (byte)(Xid->T2);
   Data[Index++] = PDUByte;

   if (Xid->RLPVersion >= 1)
   {
      if (Negotiate_XID_ADM_Values)
      {
         /* Compression */ 
         Data[Index++] = 0x74;
         PDUByte =  (byte)((Xid->Pt) << 4);
         PDUByte += (byte)(Xid->P0);
         Data[Index++] = PDUByte;
         
         Data[Index++] = (byte)((Xid->P1) & 0x00ff);
         Data[Index++] = (byte)((Xid->P1) >> 8);
         Data[Index++] = (byte)(Xid->P2);
      }
      
      if (Xid->RLPVersion >= 2)
      {
         /* T4 */
         Data[Index++] = 0x81;
         PDUByte = (byte)((Xid->T4)/10);
         Data[Index++] = PDUByte;
         
         /* Optional features */
         Data[Index++] = 0x91;
         Data[Index++] = 0x00;

         Data[Index++] = 0x00; /* Terminate the parameter list */
      }
      else
      {
         Data[Index++] = 0x00; /* Terminate the parameter list */
      }
   }
   else
   {
      Data[Index++] = 0x00; /* Terminate the parameter list */
   }

   /* Populate the PDU */
   XIDsize = (word)Index;
   DSM_PUSHDOWN_TAIL (PDU_ptr, (byte*)Data, XIDsize); /*lint !e666*/
}

/****************************************************************************
 * Function name:  ProcessXIDCommand
 * -------------
 * Description:    Processes an XID command from the remote node. This 
 * -----------     routine checks that all the parameters are OK, and
 *                 if so actions them. 
 * Parameters:     ptr to PDU, ptr to current XID settings
 * ----------
 * Returns:        TRUE on successful decoding; FALSE otherwise
 * --------
 ****************************************************************************/
boolean ProcessXIDCommand (dsm_item_type* PDU, XID_Params_T* Current)
{
   XID_Params_T    ProposedXID;

   if(Current == NULL)
   {
     ERR ("Ptr to XID struct is NULL",0,0,0);
     return FALSE;
   }
   /* Initialize proposed parameter set with current defaults */
   ProposedXID = *Current;
   
   /* Get the XID params from the callers PDU */
   if (TRUE != PDU_to_XID (PDU, &ProposedXID))
   {
     ERR ("Aborting ProcessXIDCommand on XID decode failure",0,0,0);
     return FALSE;
   }
   
   DS_UCSD_DEBUG_MSG ("In ProcessXIDCommand");
   DebugXID (&ProposedXID, 1);

   /* Now verify that these are ok, correcting them as necessary for 
      our capability. Assume that the Rlp_CurrentXID parameters are set
      to legal (ie. below or equal to the defaults) values */
   if ((ProposedXID.RLPVersion <= Current->RLPVersion) &&
       (ProposedXID.RLPVersion <= XIDMAX_VERSION))
   {
      Current->T2 = ProposedXID.T2;
      Current->RLPVersion = ProposedXID.RLPVersion; 

      if (current_RLP_params->rlp_t4 > ProposedXID.T4)
      {
         Current->T4 = current_RLP_params->rlp_t4;
      }
      else
      {
         Current->T4 = ProposedXID.T4; 
      }
      
      /* IF RLP version 2 */
      if (Current->RLPVersion >= 2)
      {
         if (current_RLP_params->rlp_n2_V2 > ProposedXID.N2)
         {
            Current->N2 = current_RLP_params->rlp_n2_V2;
         }
         else
         {
            Current->N2 = ProposedXID.N2;
         }

         if (current_RLP_params->rlp_t1_V2 > ProposedXID.T1)
         {
            Current->T1 = current_RLP_params->rlp_t1_V2;
         }
         else
         {
            Current->T1 = ProposedXID.T1;
         }

         if (current_RLP_params->rlp_iws_V2 < ProposedXID.IWFtoMSWindow)
         {
            Current->IWFtoMSWindow = current_RLP_params->rlp_iws_V2;
         }
         else
         {
            Current->IWFtoMSWindow = ProposedXID.IWFtoMSWindow;
         }

         if (current_RLP_params->rlp_mws_V2 < ProposedXID.MStoIWFWindow)
         {
            Current->MStoIWFWindow = current_RLP_params->rlp_mws_V2;
         }
         else
         {
            Current->MStoIWFWindow = ProposedXID.MStoIWFWindow;
         }         
      }
      else if (Current->RLPVersion == 1) /* RLP version 1 */
      {
         if (current_RLP_params->rlp_n2_V1 > ProposedXID.N2)
         {
            Current->N2 = current_RLP_params->rlp_n2_V1;
         }
         else
         {
            Current->N2 = ProposedXID.N2;
         }

         if (current_RLP_params->rlp_t1_V1 > ProposedXID.T1)
         {
            Current->T1 = current_RLP_params->rlp_t1_V1;
         }
         else
         {
            Current->T1 = ProposedXID.T1;
         }

         if (current_RLP_params->rlp_iws_V1 < ProposedXID.IWFtoMSWindow)
         {
            Current->IWFtoMSWindow = current_RLP_params->rlp_iws_V1;
         }
         else
         {
            Current->IWFtoMSWindow = ProposedXID.IWFtoMSWindow;
         }

         if (current_RLP_params->rlp_mws_V1 < ProposedXID.MStoIWFWindow)
         {
            Current->MStoIWFWindow = current_RLP_params->rlp_mws_V1;
         }
         else
         {
            Current->MStoIWFWindow = ProposedXID.MStoIWFWindow;
         }          
      }
      else  /* RLP version 0 */
      {
         if (current_RLP_params->rlp_n2 > ProposedXID.N2)
         {
            Current->N2 = current_RLP_params->rlp_n2;
         }
         else
         {
            Current->N2 = ProposedXID.N2;
         }

         if (current_RLP_params->rlp_t1 > ProposedXID.T1)
         {
            Current->T1 = current_RLP_params->rlp_t1;
         }
         else
         {
            Current->T1 = ProposedXID.T1;
         }

         if (current_RLP_params->rlp_iws < ProposedXID.IWFtoMSWindow)
         {
            Current->IWFtoMSWindow = current_RLP_params->rlp_iws;
         }
         else
         {
            Current->IWFtoMSWindow = ProposedXID.IWFtoMSWindow;
         }

         if (current_RLP_params->rlp_mws < ProposedXID.MStoIWFWindow)
         {
            Current->MStoIWFWindow = current_RLP_params->rlp_mws;
         }
         else
         {
            Current->MStoIWFWindow = ProposedXID.MStoIWFWindow;
         }          
      }

      if (Current->RLPVersion >= 1)
      {
         if (Negotiate_XID_ADM_Values)
         {
            if (current_RLP_params->rlp_P0 < ProposedXID.P0)
            {
               Current->P0 = current_RLP_params->rlp_P0;
            }
            else
            {
               Current->P0 = ProposedXID.P0;
            }
               
            if (current_RLP_params->rlp_P1 < ProposedXID.P1)
            {
               Current->P1 = current_RLP_params->rlp_P1;
            }
            else
            {
               Current->P1 = ProposedXID.P1;
            }
                              
            if (current_RLP_params->rlp_P2 < ProposedXID.P2)
            {
               Current->P2 = current_RLP_params->rlp_P2;
            }
            else
            {
               Current->P2 = ProposedXID.P2;
            }
         }
      }

      if (Current->RLPVersion == 2)
      {
         Rlp_current_version_is_2 = TRUE;
      
         /* Payload size is number of bytes per SDU minus 3 for RLP V2 header
         ** and minus another 3 for FCS
         */ 
         Rlp_current_PDU_payload_size = (int16) PDU_PAYLOAD_SIZE() - 6;
      
         Rlp_current_header_size = RLP_HDR_SIZE_VER_2;
      
         Rlp_current_max_window_size = RLP_M_VER_2;
      
         Rlp_current_max_frame_number = Rlp_current_max_window_size - 1;
      }
      else
      {
         Rlp_current_version_is_2 = FALSE;
      
         /* Payload size is number of bytes per SDU minus 2 for RLP V0/V1 header
         ** and minus another 3 for FCS
         */ 
         Rlp_current_PDU_payload_size = (int16) PDU_PAYLOAD_SIZE() - 5;
      
         Rlp_current_header_size = RLP_HDR_SIZE_VER_0_1;
      
         Rlp_current_max_window_size = RLP_M_VER_0_1;
      
         Rlp_current_max_frame_number = Rlp_current_max_window_size - 1;
      }
    
      /* RLP version may have changed, so re-initialize logging data */
      ds_ucsd_log_init(0,
                       0,
                       0,
                       FALSE,
                       ds_ucsd_nt_get_data_rate(),
                       ds_ucsd_nt_get_curr_mode(),
                       Current->RLPVersion);

      DebugXID (Current, 2);
   }
   else
   {
      DATA_MSG1_ERROR("Illegal RLP version received = %d", ProposedXID.RLPVersion);
   }

   return TRUE;
}


/****************************************************************************
 * Function name:  ProcessXIDResponse
 * -------------
 * Description:    This routine processes an XID frame which is a response
 * -----------     to our XID command. As such we can assume that the values
 *                 are legal.
 * Parameters:     ptr to PDU, ptr to XID params to setup
 * ----------
 * Returns:        TRUE on successful decoding; FALSE otherwise
 * --------
 ****************************************************************************/
boolean ProcessXIDResponse (dsm_item_type * PDU, XID_Params_T* Xid)
{

  if(Xid == NULL)
  {
    DATA_MSG0_ERROR("Ptr to XID struct is NULL");
    return FALSE;
  }

  /* Get the XID params from the RLP response frame */
  if ( FALSE == PDU_to_XID (PDU, Xid) )
  {
    DATA_MSG0_ERROR("Aborting ProcessXIDResponse on XID decode failure");
    return FALSE;
  }

  if (Xid->RLPVersion == 2)
  {
    Rlp_current_version_is_2 = TRUE;
      
    /* Payload size is number of bytes per SDU minus 3 for RLP V2 header
    ** and minus another 3 for FCS
    */ 
    Rlp_current_PDU_payload_size = (int16) PDU_PAYLOAD_SIZE() - 6;
      
    Rlp_current_header_size = RLP_HDR_SIZE_VER_2;
      
    Rlp_current_max_window_size = RLP_M_VER_2;
      
    Rlp_current_max_frame_number = Rlp_current_max_window_size - 1;
  }
  else
  {
    Rlp_current_version_is_2 = FALSE;
      
    /* Payload size is number of bytes per SDU minus 2 for RLP V0/V1 header
    ** and minus another 3 for FCS
    */ 
    Rlp_current_PDU_payload_size = (int16) PDU_PAYLOAD_SIZE() - 5;
      
    Rlp_current_header_size = RLP_HDR_SIZE_VER_0_1;
      
    Rlp_current_max_window_size = RLP_M_VER_0_1;
      
    Rlp_current_max_frame_number = Rlp_current_max_window_size - 1;
  }
   
  /* RLP version may have changed, so re-initialize logging data */
  ds_ucsd_log_init( 0,
                    0,
                    0,
                    FALSE,
                    ds_ucsd_nt_get_data_rate(),
                    ds_ucsd_nt_get_curr_mode(),
                    Xid->RLPVersion );
                    
  DebugXID (Xid, 1);
  return TRUE;
}


/****************************************************************************
 * Function name:  HandleXID
 * -------------
 * Description:    Controls the XID negotiation
 * -----------
 * Parameters:     ptr to RLP frame
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
static void HandleXID (const RLP_Frame_T* Frame)
{
    boolean terminate= FALSE;

    //MSG_LOW ("In HandleXID",0,0,0);
    if(Frame == NULL)
    {
      DATA_MSG0_ERROR("Ptr to RLP frmae is NULL");
      return;      
    }
    
    if (CR == FALSE)
    {
        if ((XID_C_State == STATE_WAIT) && (PF))
        {
            /* This is a response to one of our XIDs */
            if ( TRUE == ProcessXIDResponse (Frame->Data, &Rlp_CurrentXID) )
            {
              Poll_xchg = STATE_IDLE;
              STOP_TIMER (TIMERID_TXID);
              XID_C_State = STATE_IDLE;
            
              /* If the other end has changed the compression direction 
                 and compression-negotiation=1 then terminate the call.
                 (see V25ter para.6.6.1) */
              if (current_RLP_params->rlp_comp_neg == 1)
              {
                if (NewXID.P0 != Rlp_CurrentXID.P0)
                {
                  terminate = TRUE;
                }
              }

              T1ValueInMs = Rlp_CurrentXID.T1*10;
            
              if (terminate)
              {
                /* Other end has changed compression direction - disconnect */
                DATA_MSG0_ERROR("RLP: Compression mode changed in balanced mode");
                RlpCurrentState  = RLPSTATE_ADM_ATTACHED;
                ds_ucsd_set_rlp_error_handling(TRUE);
              }
              else
              {
                /* Indicate XID to higher layer */
                WRITE_EVENT_TO_L2R (RLP_XID_CNF);
              }
            }
        }
    }
    else
    {
        if (XID_C_State == STATE_IDLE)
        {
            DS_UCSD_DEBUG_MSG ("RLP: Received XID command");
            /* XID data received */
            if ( TRUE == ProcessXIDCommand (Frame->Data, &Rlp_CurrentXID) )
            {
              T1ValueInMs = Rlp_CurrentXID.T1*10;
            
              /* Indicate XID to higher layer */
              WRITE_EVENT_TO_L2R (RLP_XID_IND);
            
              /* Allocate PDU for XID Frame */
              if (NULL != TxXIDData)
              {
                ds_ucsd_UplinkPDUMgr_Free(&TxXIDData);
              }
              TxXIDData = ds_ucsd_UplinkPDUMgr_New();
            
              /* Create a XID response to send back */
              XID_R_State = STATE_SEND;
              XID_to_PDU (&TxXIDData, &Rlp_CurrentXID);
            }
        }
        else
        {
            DS_UCSD_DEBUG_MSG ("RLP:XID contend!");
            /* There has been a contention. Ignore the XID and retry */
            /* Reset state variables only if T1 timer is already running */
            if ( STATE_WAIT == XID_C_State )
            {
              Poll_xchg = STATE_IDLE;
              XID_C_State = STATE_IDLE;
            }
            /*START_TIMER (TIMERID_TXID, (Rlp_CurrentXID.T1*10));*//*cetecom!!*/
        }
    }
}

/****************************************************************************
 * Function name:  HandleTEST
 * -------------
 * Description:    Handles a received TEST frame.
 * -----------     
 * Parameters:     Pointer to TEST frame
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
void HandleTEST (RLP_Frame_T* Frame)
{
   byte tmp_buf[RLP_PDU_MAX_USER_DATA_SIZE];
   uint16 current_rlp_pdu_size = get_current_rlp_pdu_payload_size();

   if(Frame == NULL)
   {
     DATA_MSG0_ERROR("Ptr to RLP frmae is NULL");
     return;      
   }

   if (CR == FALSE)
   {
      if ((Test_C_State == STATE_WAIT) && (PF == Test_C_PBit))
      {
         /* This is a response to our TEST command */
         STOP_TIMER (TIMERID_TTEST);
         
         if (Test_C_PBit)
            Poll_xchg = STATE_IDLE;
         
         Test_C_State = STATE_IDLE;
         
/* TODO: This looks suspect, (was in RLP_DEBUG block), commented out for now - TMR

#ifdef DS_UCSD_DEBUG
*/
         DS_UCSD_DEBUG_MSG ("RLP:Test Resp Rxd!");

         /* Extract data from PDU for comparison */
         memset((void*)tmp_buf, 0, sizeof(tmp_buf));

         DSM_PULLUP (&Frame->Data, tmp_buf, current_rlp_pdu_size);

         /* Compare data for validation */
         if (memcmp(tmp_buf, TEST_DATA, strlen((char*)TEST_DATA)) == 0)
         {
            DS_UCSD_DEBUG_MSG ("RLP:Test Resp OK!");
            WRITE_EVENT_TO_L2R (RLP_TEST_CNF);
         }
         else
         {
            DS_UCSD_DEBUG_MSG ("RLP:Test Resp Bad!");
            WRITE_EVENT_TO_L2R (RLP_TEST_CNF_NEG);
         }
/*
#endif
*/
      }
   }
   else
   {
      Test_R_State = STATE_SEND;
      Test_R_FBit  = PF;

      /* Loop received PDU back to Tx */
      Test_R_Data = RxFrame->Data;
      RxFrame->Data = NULL;
      
      DS_UCSD_DEBUG_MSG ("RLP:Test Cmd Rxd!");
   }
}

/****************************************************************************
 * Function name:  CheckXIDTimeout
 * -------------
 * Description:    Checks the timer for XID timeout
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        None
 * --------
 ****************************************************************************/
static void CheckXIDTimeout(void)
{
   if (TIMER_EXPIRED (TIMERID_TXID))
   {
      DS_UCSD_DEBUG_MSG ("RLP:XID_T.out");
      
      STOP_TIMER (TIMERID_TXID);
      
      Poll_xchg = STATE_IDLE;
      if (XID_Count > Rlp_CurrentXID.N2)
      {
        DATA_MSG0_ERROR("RLP: XID negotiation failed to complete");
        ds_ucsd_set_rlp_error_handling(TRUE);
        
        /* Exceeded the retries, so goto ADM_ATTACHED state */
        RlpCurrentState = RLPSTATE_ADM_ATTACHED;
      }
      else
      {
         XID_C_State = STATE_SEND;
      }
   }
}

/****************************************************************************
 * Function name:   CheckTESTTimeout
 * -------------
 * Description:     Checks the timer for TEST timeout
 * -----------     
 * Parameters:      None
 * ----------
 * Returns:         None
 * --------
 ****************************************************************************/
static void CheckTESTTimeout(void)
{
   if (TIMER_EXPIRED (TIMERID_TTEST))
   {
      STOP_TIMER (TIMERID_TTEST);

      if (Test_C_PBit)
      {
         Poll_xchg = STATE_IDLE;
      }

      if (Test_Count > Rlp_CurrentXID.N2)
      {
         Test_C_State = STATE_IDLE;

         DS_UCSD_DEBUG_MSG ("RLP:No Test Resp!");
      }
      else
      {
         Test_C_State = STATE_SEND;
      }
   }
}

/****************************************************************************
 * Function name:  ReadRLPFrame
 * -------------
 * Description:    Gets the info from the received RLP Frame.
 * -----------     
 * Parameters:     ptr to RLP frame
 * ----------
 * Returns:        status OK/ERROR
 * --------
 ****************************************************************************/
short ReadRLPFrame (RLP_Frame_T* Frame)
{
   short status = RLP_ERROR;

   if(Frame == NULL)
   {
     DATA_MSG0_ERROR("Ptr to RLP frame is NULL");
     return status;      
   }

   if (ds_HardwareFrameAvailable ())
   {
      status = ds_HardwareReceiveRLPFrame (Frame);
      if (status == RLP_OK)
      {
         /* Extract the frame type */
         Ns = (short)GET_NS (Frame);
         
         /* Now extract the control information from the frame 
         ** depending 
         */
         if (Ns == NS_UNNUMBEREDFRAME)
         {
            /* Unnumbered frames are same format for all RLP versions */
            FrameType = FRAMETYPE_UNNUMBERED;
            CR = GET_CR (Frame);
            PF = GET_PF (Frame);
            UnNumberedFrame = (short)GET_UNNUMBERED_FRAME (Frame);
         }
         else /* Frame is I + S or S */
         {
            if (Ns == NS_SUPERVISORYFRAME)
            {
            
               FrameType = FRAMETYPE_SUPERVISORY;
               if (current_rlp_version_is_2())
               {
                  SF = (short)GET_SUPERVISORY_FRAME_V2 (Frame);
                  Nr = (short)GET_NR_V2 (Frame);
                  CR = GET_CR_V2 (Frame);
                  PF = GET_PF_V2 (Frame);
               }
               else
               {
                  SF = (short)GET_SUPERVISORY_FRAME(Frame);
                  Nr = (short)GET_NR (Frame);
                  CR = GET_CR (Frame);
                  PF = GET_PF (Frame);
               }
            }
            else
            {
               FrameType = FRAMETYPE_INFORMATION;
               if (current_rlp_version_is_2())
               {
                  /* Get the full Ns value */
                  Ns = (short)GET_NS_V2 (Frame);
                  SF = (short)GET_SUPERVISORY_FRAME_V2 (Frame);
                  Nr = (short)GET_NR_V2 (Frame);
                  CR = GET_CR_V2 (Frame);
                  PF = GET_PF_V2 (Frame);
               
                  /* If L2R Status bit is not set, PDU doesn't carry carry
                  ** a status octet, keep record of this with PDU
                  */
                  if (GET_S_V2 (Frame) == FALSE)
                  {
                     Frame->Data->app_field |= RLP_DSM_APP_FIELD_NO_STATUS_BYTE;
                  }
                  
                  
               }
               else
               {
                  SF = (short)GET_SUPERVISORY_FRAME(Frame);
                  Nr = (short)GET_NR (Frame);
                  CR = GET_CR (Frame);
                  PF = GET_PF (Frame);
               }
            }

            /* Check range of N(R) if necessary -- this is (was) a bug
            ** with Nokia infrastructure
            */
            if ( (Nr > get_current_rlp_max_frame_number()) ||
                 (Nr < 0) )
            {
               DATA_MSG1_ERROR("RLP: received invalid N(R)=%d",Nr);
               status = RLP_ERROR;
            }
         } /* Frame is I + S or S */
      } 
   }

   return status;
}

/****************************************************************************
 * Function name:  SendNumberedFrame
 * -------------
 * Description:    Sends a RLP frame as defined by the parameters
 * -----------     
 * Parameters:     Frame type, CR bit, PF bit, Nr, Ns, PDU
 * ----------
 * Returns:        status OK/ERROR
 * --------
 ****************************************************************************/
static short SendNumberedFrame (word Frame,
                                boolean cr, boolean pf,
                                word   nr, word   ns,
                                dsm_item_type* PDU)
{
   short            status         = RLP_ERROR;
   register uint32  rlp_hdr_size;

   /* MSG_LOW ("In SendNumberedFrame - PDU: 0x%x",PDU,0,0); */
         
   memset (&TxFrame, 0x00, sizeof(TxFrame));
   
   if (NULL != PDU) /* Information & Supervisory frame */
   {
      /* Allocate PDU if none passed by caller */
      TxFrame.Data = PDU;
      PDU->app_field |= RLP_DSM_APP_FIELD_INFO_FRAME; 
      if (current_rlp_version_is_2())
      {
         SET_INFORMATION_FRAME_V2 (TxFrame, Frame);
         SET_CR_V2                (TxFrame, cr);
         SET_PF_V2                (TxFrame, pf);
         SET_NR_V2                (TxFrame, nr);
         SET_NS_V2                (TxFrame, ns);
         
         /* If there is a status byte in the V2 PDU, (i.e. 
         ** NO_STATUS_BYTE flag is not set), set the L2R Status
         ** bit to on.  It is off by default
         */
         if (!(PDU->app_field & RLP_DSM_APP_FIELD_NO_STATUS_BYTE))
         {
           SET_S_V2 (TxFrame, TRUE); /*lint !e774 !e506 */
         }

         rlp_hdr_size = RLP_HDR_SIZE_VER_2;

         /* Indicate first I-frame sent with L2R status */
         if (FALSE == RLP_Tx_Info_Frame)
         {
           DS_UCSD_DEBUG_MSG("Sending first info frame");
           RLP_Tx_Info_Frame = TRUE;
         }
      }
      else
      {
         SET_INFORMATION_FRAME (TxFrame, Frame);
         SET_CR                (TxFrame, cr);
         SET_PF                (TxFrame, pf);
         SET_NR                (TxFrame, nr);
         SET_NS                (TxFrame, ns);
         rlp_hdr_size = RLP_HDR_SIZE_VER_0_1;
      }

      status = ds_HardwareSendRLPFrame (&TxFrame,rlp_hdr_size);

      /* Save the most recently transmitted frame data */
      Last_tx_frame_type   = FRAMETYPE_INFORMATION;
      Last_tx_cmd          = Frame;
      Last_tx_cr           = cr;
      Last_tx_pf           = pf;
      Last_tx_nr           = nr;

      ds_ucsd_nt_set_tx_frame_sent(TRUE);
   }
   else  /* Supervisory only frame */
   {
      /* If duplicate tx frame suppression is not turned on, 
      ** or the supervisory frame is not the same as the last
      ** one sent out, then send current one out.
      */
      if ( (!ds_ucsd_nt_get_suppress_duplicate_tx_frames()) ||
           ( ( (Last_tx_frame_type != FRAMETYPE_SUPERVISORY) &&
               (Last_tx_frame_type != FRAMETYPE_INFORMATION) ) ||
             (Last_tx_cmd != Frame) ||  
             (Last_tx_cr != cr) ||
             (Last_tx_pf != pf) ||
             (Last_tx_nr != nr) ) )
      {
         
         /* Allocate PDU if none passed by caller */
         TxFrame.Data = ds_ucsd_UplinkPDUMgr_New ();
         ASSERT (NULL != TxFrame.Data);

         /* Set frame up */
         if (current_rlp_version_is_2())
         {
            SET_SUPERVISORY_FRAME_V2 (TxFrame, Frame);
            SET_CR_V2                (TxFrame, cr);
            SET_PF_V2                (TxFrame, pf);
            SET_NR_V2                (TxFrame, nr);
            rlp_hdr_size = RLP_HDR_SIZE_VER_2;
         
         }
         else
         {
            SET_SUPERVISORY_FRAME (TxFrame, Frame);
            SET_CR                (TxFrame, cr);
            SET_PF                (TxFrame, pf);
            SET_NR                (TxFrame, nr);
            rlp_hdr_size = RLP_HDR_SIZE_VER_0_1;
         }
         
         /* Send the frame */
         status = ds_HardwareSendRLPFrame (&TxFrame, rlp_hdr_size);

         /* Save the most recently transmitted frame data */
         Last_tx_frame_type   = FRAMETYPE_SUPERVISORY;
         Last_tx_cmd          = Frame;
         Last_tx_cr           = cr;
         Last_tx_pf           = pf;
         Last_tx_nr           = nr;

         /* Indicate that we have sent some frame data.  Note: this
         ** flag should never be cleared by RLP
         */
         ds_ucsd_nt_set_tx_frame_sent(TRUE);
         
      }
      else
      {
         ds_ucsd_nt_set_frame_suppressed(TRUE);
         /*  MSG_HIGH ("Suppressed duplicate numbered TX frame: type=%d",
             Frame,0,0); */
      }
   }

      
   
   return status;
}

/****************************************************************************
 * Function name:  SendUnNumberedFrame
 * -------------
 * Description:    Sends a RLP frame as defined by the parameters
 * -----------     
 * Parameters:     Frame type, CR bit, PF bit, PDU
 * ----------
 * Returns:        status OK/ERROR
 * --------
 ****************************************************************************/
short SendUnNumberedFrame (word Frame, 
                           boolean cr, boolean pf,
                           dsm_item_type * PDU)
{
   short status = RLP_ERROR;

   /* MSG_LOW ("In SendUnNumberedFrame - PDU: 0x%x",PDU,0,0); */

   /* If duplicate tx frame suppression is not turned on, 
   ** or the unnumbered frame is not NULL, then send current 
   ** one out.  See 3GPP TS24.022 section 5.6.
   */
   if ( (!ds_ucsd_nt_get_suppress_duplicate_tx_frames()) ||
        (FRAME_NULL != Frame) )
   {
      memset (&TxFrame, 0x00, sizeof(TxFrame));

      /* Allocate PDU if none passed by caller */
      TxFrame.Data = (NULL != PDU)? PDU : ds_ucsd_UplinkPDUMgr_New ();
      ASSERT (NULL != TxFrame.Data);
      
      SET_UNNUMBERED_FRAME(TxFrame, Frame);
      SET_CR              (TxFrame, cr);
      SET_PF              (TxFrame, pf); 
      
      /* Send RLP UnNumbered frame.  Note that unnumbered frames always have 
      ** a 2 byte RLP header
      */  
      status = ds_HardwareSendRLPFrame (&TxFrame,(uint32)RLP_HDR_SIZE_VER_0_1);

      /* Save the most recently transmitted frame data */
      Last_tx_frame_type   = FRAMETYPE_UNNUMBERED;
      Last_tx_cmd          = Frame;
      Last_tx_cr           = cr;
      Last_tx_pf           = pf;

      /* Indicate that we have sent some frame data.  Note: this
      ** flag should never be cleared by RLP
      */
      ds_ucsd_nt_set_tx_frame_sent(TRUE);
   }
   else
   {
      ds_ucsd_nt_set_frame_suppressed(TRUE);
      if (PDU)
      {
         ds_ucsd_UplinkPDUMgr_Free (&PDU);
      }
      /* MSG_HIGH ("Suppressed duplicate unnumbered TX frame: type=%d",
         Frame,0,0); */
   }   
   
   return status;
}



/****************************************************************************
 * Function name:  DebugXID
 * ------------- 
 * Description:    Show a XID structure
 * -----------     
 * Parameters:     XID params struct ptr
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void
DebugXID (const XID_Params_T* Xid, word msg)
{
#ifdef DS_UCSD_DEBUG

   if(Xid == NULL)
   {
     DATA_MSG0_ERROR("Ptr to XID struct is NULL");
     return;
   }

   switch (msg)
   {
      case 0:
         DS_UCSD_DEBUG_MSG ("TX XID as follows:");
         break;

      case 1:
         DS_UCSD_DEBUG_MSG ("RX XID as follows:");
         break;

      case 2:
         DS_UCSD_DEBUG_MSG ("DO XID as follows:");
         break;

      default:
         DS_UCSD_DEBUG_MSG ("?? XID as follows:");
         break;
   }
   
   DS_UCSD_DEBUG_MSG_3 ("     XID: Vr=%d Iwf=%d Iws=%d",Xid->RLPVersion,
                                                        Xid->IWFtoMSWindow,
                                                        Xid->MStoIWFWindow);
                                                         
   DS_UCSD_DEBUG_MSG_3 ("     XID: T1=%d N2=%d T2=%d",Xid->T1,
                                                      Xid->N2,
                                                      Xid->T2);

   if (Xid->RLPVersion >= 1)
   {
     DS_UCSD_DEBUG_MSG_3 ("     XID: T4=%d Pt=%d P0=%d", Xid->T4,
                                                         Xid->Pt,
                                                         Xid->P0);

     DS_UCSD_DEBUG_MSG_2 ("     XID: P1=%d P2=%d",Xid->P1,Xid->P2);
   }
#endif /* DS_UCSD_DEBUG */
}

/****************************************************************************
 * Function name:  ds_ucsd_get_rlp_process_uplink
 * ------------- 
 * Description:    Returns the value of Rlp_process_uplink
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_rlp_process_uplink(void)
{
  return Rlp_process_uplink;
}

/****************************************************************************
 * Function name:  ds_ucsd_set_rlp_process_uplink
 * ------------- 
 * Description:    Sets the value of Rlp_process_uplink
 * -----------     
 * Parameters:     boolean: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_rlp_process_uplink(boolean value)
{
  Rlp_process_uplink = value;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_rlp_process_downlink
 * ------------- 
 * Description:    Returns the value of Rlp_process_downlink
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_rlp_process_downlink(void)
{
  return Rlp_process_downlink;
}

/****************************************************************************
 * Function name:  ds_ucsd_set_rlp_process_downlink
 * ------------- 
 * Description:    Sets the value of Rlp_process_downlink
 * -----------     
 * Parameters:     boolean: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_rlp_process_downlink(boolean value)
{
  Rlp_process_downlink = value;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_rlp_error_handling
 * ------------- 
 * Description:    Returns the value of RLP_ErrorHandling
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_rlp_error_handling(void)
{
  return RLP_ErrorHandling;
}

/****************************************************************************
 * Function name:  ds_ucsd_set_rlp_error_handling
 * ------------- 
 * Description:    Sets the value of RLP_ErrorHandling
 * -----------     
 * Parameters:     boolean: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_rlp_error_handling(boolean value)
{
  RLP_ErrorHandling = value;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_current_xid
 * ------------- 
 * Description:    Returns a pointer to the Rlp_CurrentXID
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        Pointer of type XID_Params_T
 * -------- 
 ****************************************************************************/
XID_Params_T * ds_ucsd_get_current_xid(void)
{
  return &Rlp_CurrentXID;
}

/****************************************************************************
 * Function name:  ds_ucsd_set_RlpCurrentState
 * ------------- 
 * Description:    Sets the value of RlpCurrentState
 * -----------     
 * Parameters:     short: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_RlpCurrentState(short value)
{
  RlpCurrentState = value;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_RxFrame
 * ------------- 
 * Description:    Returns RxFrame
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        Pointer of type RLP_Frame_T
 * -------- 
 ****************************************************************************/
RLP_Frame_T * ds_ucsd_get_RxFrame(void)
{
  return RxFrame;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_TxFrame
 * ------------- 
 * Description:    Returns RxFrame
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        Pointer of type RLP_Frame_T
 * -------- 
 ****************************************************************************/
RLP_Frame_T * ds_ucsd_get_TxFrame(void)
{
  return &TxFrame;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_UA_FBit
 * ------------- 
 * Description:    Returns the value of UA_FBit
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_UA_FBit(void)
{
  return UA_FBit;
}

/****************************************************************************
 * Function name:  ds_ucsd_set_UA_FBit
 * ------------- 
 * Description:    Sets the value of UA_FBit
 * -----------     
 * Parameters:     boolean: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_UA_FBit(boolean value)
{
  UA_FBit = value;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_UA_State
 * ------------- 
 * Description:    Returns the value of UA_State
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        byte
 * -------- 
 ****************************************************************************/
byte ds_ucsd_get_UA_State(void)
{
  return UA_State;
}

/****************************************************************************
 * Function name:  ds_ucsd_set_UA_State
 * ------------- 
 * Description:    Sets the value of UA_State
 * -----------     
 * Parameters:     byte: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_UA_State(byte value)
{
  UA_State = value;
}

/****************************************************************************
 * Function name:  ds_ucsd_set_Negotiate_XID_ADM_Values
 * ------------- 
 * Description:    Sets the value of Negotiate_XID_ADM_Values
 * -----------     
 * Parameters:     boolean: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_Negotiate_XID_ADM_Values(boolean value)
{
  Negotiate_XID_ADM_Values = value;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_CR
 * ------------- 
 * Description:    Returns the value of CR
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_CR(void)
{
  return CR;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_PF
 * ------------- 
 * Description:    Returns the value of PF
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_PF(void)
{
  return PF;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_UnNumberedFrame
 * ------------- 
 * Description:    Returns the value of UnNumberedFrame
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
short ds_ucsd_get_UnNumberedFrame(void)
{
  return UnNumberedFrame;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_FrameType
 * ------------- 
 * Description:    Returns the value of FrameType
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
short ds_ucsd_get_FrameType(void)
{
  return FrameType;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_Remap_State
 * ------------- 
 * Description:    Returns the value of Remap_State
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        short
 * -------- 
 ****************************************************************************/
short ds_ucsd_get_Remap_State(void)
{
  return Remap_State;
}

/****************************************************************************
 * Function name:  ds_ucsd_set_Remap_State
 * ------------- 
 * Description:    Sets the value of Remap_State
 * -----------     
 * Parameters:     short: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_Remap_State(short value)
{
  Remap_State = value;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_UplinkRemapped
 * ------------- 
 * Description:    Returns the value of UplinkRemapped
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_UplinkRemapped(void)
{
  return UplinkRemapped;
}

/****************************************************************************
 * Function name:  ds_ucsd_set_UplinkRemapped
 * ------------- 
 * Description:    Sets the value of UplinkRemapped
 * -----------     
 * Parameters:     boolean: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_UplinkRemapped(boolean value)
{
  UplinkRemapped = value;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_UplinkRemappedLastNr
 * ------------- 
 * Description:    Returns the value of UplinkRemappedLastNr
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        short
 * -------- 
 ****************************************************************************/
short ds_ucsd_get_UplinkRemappedLastNr(void)
{
  return UplinkRemappedLastNr;
}

/****************************************************************************
 * Function name:  ds_ucsd_set_UplinkRemappedLastNr
 * ------------- 
 * Description:    Sets the value of UplinkRemappedLastNr
 * -----------     
 * Parameters:     short: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_UplinkRemappedLastNr(short value)
{
  UplinkRemappedLastNr = value;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_CurrentPDUType
 * ------------- 
 * Description:    Returns the value of CurrentPDUType
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        PDU_Type
 * -------- 
 ****************************************************************************/
PDU_Type ds_ucsd_get_CurrentPDUType(void)
{
  return CurrentPDUType;
}

/****************************************************************************
 * Function name:  ds_ucsd_set_CurrentPDUType
 * ------------- 
 * Description:    Sets the value of CurrentPDUType
 * -----------     
 * Parameters:     PDU_Type: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_CurrentPDUType(PDU_Type value)
{
  CurrentPDUType = value;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_Rlp_current_version_is_2
 * ------------- 
 * Description:    Gets the value of Rlp_current_version_is_2
 * -----------     
 * Parameters:     PDU_Type: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_Rlp_current_version_is_2(void)
{
  return Rlp_current_version_is_2;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_Rlp_current_PDU_payload_size
 * ------------- 
 * Description:    Gets the value of Rlp_current_PDU_payload_size
 * -----------     
 * Parameters:     PDU_Type: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
uint16 ds_ucsd_get_Rlp_current_PDU_payload_size(void)
{
  return Rlp_current_PDU_payload_size;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_Rlp_current_header_size
 * ------------- 
 * Description:    Gets the value of Rlp_current_header_size
 * -----------     
 * Parameters:     PDU_Type: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
uint16 ds_ucsd_get_Rlp_current_header_size(void)
{
  return Rlp_current_header_size;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_Rlp_current_max_window_size
 * ------------- 
 * Description:    Gets the value of Rlp_current_max_window_size
 * -----------     
 * Parameters:     PDU_Type: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
uint16 ds_ucsd_get_Rlp_current_max_window_size(void)
{
  return Rlp_current_max_window_size;
}

/****************************************************************************
 * Function name:  ds_ucsd_get_Rlp_current_max_frame_number
 * ------------- 
 * Description:    Gets the value of Rlp_current_max_frame_number
 * -----------     
 * Parameters:     PDU_Type: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
uint16 ds_ucsd_get_Rlp_current_max_frame_number(void)
{
  return Rlp_current_max_frame_number;
}

#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */


