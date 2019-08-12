/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  F L O W   C O N T R O L   P R O T O C O L
                 

GENERAL DESCRIPTION
  The Flow Control Protocol (FCP) provides procedures and messages to
  perform flow control for RLP.

EXTERNALIZED FUNCTIONS (Global)

  hdrfcp_send_xon_req()  - Called to send Xon msg for a particular stream
  hdrfcp_send_xoff_req() - Called to send Xoff msg for a particular stream
  hdrfcp_process_msg()   - Called to process received FCP messages.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrfcp_powerup_init should be called at powerup.

Copyright (c) 2001 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrfcp.c_v   1.12   16 May 2002 16:14:30   vsali  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrfcp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/15   vko     Fixed DO not sleeping due to HMQ queue stuck
10/01/14   vko     Fix searching hdrhmp_rasm_acked with correct index.
07/09/14   cnx     Support NV refresh without reset.
09/19/13   ukl     Reduce Code Duplication HDR-CP
03/15/13   vko     Fixed transaction id specific to stream
10/07/12   cnx     Clean up RASM before idle handoff to native HDR network.
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
12/11/09   wsh     Fixed compile issue on 2h09.genesis MOB
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/18/09   wsh     Updated file to include private header hdrmc_v.h
12/18/08   sju     Added support to handle E_AGAIN in send_xon_outcome_cb().
12/10/08   smd     Replaced assert.h with amssassert.h
10/06/08   sju     Fixed Lint warnings.
06/18/08   sju     Reset xon_tx_state after personality switch
05/22/08   pba     Klockwork fixes.
02/12/07   mpa     Fixed RVCT warnings.
11/16/06   sju     Obsoleted HDRIND_FCP_ALL_STREAMS_CLOSED indication.
10/23/06   sju     Added support to send Xon/XoffRequest by using RASM.
06/30/06   mpa     Moved hdrfcp_commit_session_data_to_efs() def for RVCT comp.
06/20/06   mpa     Save session data to efs as part of processing xon/xoff 
                   cmds now that hdrfcp runs in HDRMC task context.
06/05/06   pba     Added support to store session data in EFS.
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
12/13/04   hal     Changed dsm_offset_new_buffer to use hdrutil_new_buffer.
                   Added hdrutil.h header file.
09/23/04   vas     In process_msg, return if match to stream_id not found.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 SSID
05/16/02   vas     Changed ERR to MSG_ERROR for unexpected SLP callbacks.
03/12/02   vas     Changed so that FCP will give an indication always when it
                   receives a DataReady message, instead of checking whether
                   the stream is "Closed" before giving the indication.
02/13/02   vas     Added code to give indication when all streams are Xoff-ed
                   Changed msg when unexpected Xon/Xoff rsp recvd from ERR to
                   MSG_HIGH, since it is possible to rcv rsp after timeout or
                   multiple responses because of resent msgs.
12/07/01   vas     Added code to retransmit Xon/Xoff a max of 3 times.
11/21/01   vas     Code rewrite. Not retransmitting Xon/Xoff msgs. Msg 
                   transmission has state.Removed HDRFCP_OPEN_XOFF_SENT state
09/25/01   vas     Free dsm item containing message after processing it.
09/19/01   vas     Fixed to clear Xoff timer (not Xon timer) if the Xoff
                   Response message is received.
09/14/01   vas     New state HDRFCP_OPEN_XOFF_SENT & associated processing.
                   Added hdrfcp_stream_is_open(). 
08/17/01   vas     Rearchitected to fit in with the hdrds / hdrpac modules.
                   Added full Flow Control functionality
06/20/01   vas     Code cleanup
05/30/01   vas     Created module

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "target.h"
#include "dsm.h"
#include "hdrdebug.h"
#include "err.h"
#include "amssassert.h"
#include "task.h"

#include "hdrhai.h"
#include "hdrfcp.h"
#include "hdrtrace.h"
#include "hdrrmac.h"
#include "hdrhmp.h"
#include "hdrbit.h"
#include "hdrpac_api.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#include "hdrmci.h"
#include "hdrdsm.h"

#include "hdrutil.h"
#include "hdrscmdb.h"
#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#endif /* FEATURE_LTE_TO_HDR_OH*/

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
#define HDRFCP_MESSAGE_PRIORITY       40

/* Response time for Xon & Xoff response message is 1 sec */
#define HDRFCP_MSG_RSP_TIME_MSEC      1000

/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING
             
===========================================================================*/

/* Mesage ID Unpacking routine */
#define HDRFCP_UNPACK_MSG_ID( ptr, val_ptr )  \
           HDRBIT_UNPACK8( ptr, val_ptr, 0,  8 )

/* Unpack the data ready Transaction ID */
#define HDRFCP_UNPACK_DATA_READY_TRANS_ID( msg_ptr, val_ptr ) \
          HDRBIT_UNPACK8( msg_ptr, val_ptr, 8, 8 )

/* Packing XON request */
#define HDRFCP_PACK_XON_MSG_ID( msg_ptr ) \
          HDRBIT_PACK8( msg_ptr, HDRFCP_XON_REQ_MSG_ID, 0, 8 )
 
/* Packing XOFF request */
#define HDRFCP_PACK_XOFF_MSG_ID( msg_ptr ) \
          HDRBIT_PACK8( msg_ptr, HDRFCP_XOFF_REQ_MSG_ID, 0, 8 )

/* Packing Data Ready Ack message */
#define HDRFCP_PACK_DATA_READY_ACK_MSG_ID( msg_ptr ) \
          HDRBIT_PACK8( msg_ptr, HDRFCP_DATA_READY_ACK_MSG_ID, 0, 8 )

#define HDRFCP_PACK_DATA_READY_ACK_TRANS_ID( msg_ptr, val ) \
          HDRBIT_PACK8( msg_ptr, val, 8, 8 )

/* <EJECT> */

/*===========================================================================

                           LOCAL DEFINITIONS 
             
===========================================================================*/

/* FCP runs in the DS task */
#define HDRFCP_TASK_PTR hdrmc_tcb_ptr

/*===========================================================================

                      LOCAL FUNCTION DECLARATIONS

===========================================================================*/

LOCAL void hdrfcp_process_send_xon_cmd
(
  hdrhai_stream_enum_type stream_id
);

LOCAL void hdrfcp_process_send_xoff_cmd
(
  hdrhai_stream_enum_type stream_id
);

LOCAL void hdrfcp_process_xon_timer
(
  uint32 stream_id
);

LOCAL void hdrfcp_process_xoff_timer
(
  uint32 stream_id
);

LOCAL void hdrfcp_send_xon_outcome_cb
( 
  void *fcp_instance_ptr, 
  hdrerrno_enum_type status
);

LOCAL void hdrfcp_send_xoff_outcome_cb
( 
  void *fcp_instance_ptr, 
  hdrerrno_enum_type status
);

LOCAL void hdrfcp_set_stream_to_close 
(
  hdrhai_stream_enum_type stream_id 
);

LOCAL void hdrfcp_timer_cb
(
  uint32 timer_id
);

LOCAL void hdrfcp_send_xon_xoff_req
(
  hdrhai_protocol_name_enum_type sender,
  hdrhai_stream_enum_type stream_id,
  hdrfcp_cmd_name_enum_type cmd_name
);

/* <EJECT> */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Each FCP Instance is associated with a stream. The FCP instance is 
   identified using the following IDs. The ID is an index to an array 
   containing the FCP instance control blocks.  */
typedef enum
{
  HDRFCP_STREAM1_INST = 0,
  HDRFCP_STREAM2_INST = 1,
  HDRFCP_STREAM3_INST = 2
} hdrfcp_instance_id_enum_type;

/* Enumeration for FCP timer */
typedef enum
{
  HDRFCP_XON_TIMER        = 0, 
  HDRFCP_XOFF_TIMER       = 1 
} hdrfcp_timer_enum_type;

/* Enumeration for the states message transimssion can be */
typedef enum
{
  HDRFCP_NO_MSG_TX_PEND        = 0, /* No message transmission pending        */
  HDRFCP_MSG_WAIT_FOR_TX       = 1, /* Msg waiting to be txed (no SLP cb rcvd)*/
  HDRFCP_MSG_WAIT_FOR_RSP      = 2, /* Msg txed, waiting for response         */
  HDRFCP_MSG_WAIT_FOR_RASM_ACK = 3  /* rasm_acked called, waiting for slp cb*/
} hdrfcp_msg_tx_state_enum_type;

/* Max number of times a message transmission is tried before giving up    */
#define HDRFCP_MAX_NUM_MSG_XMIT 3

/* retry interval */
#define HDRMFCP_XONOFF_MSG_RETRY_INTERVAL_MS 1000 

/* There is one FCP instance per stream. Each instance has the following
   control block */
typedef struct
{
  hdrhai_stream_enum_type stream_id;     /* Stream for this instance       */
  rex_timer_type xon_rsp_timer;          /* Xon Response Timer             */
  hdrfcp_msg_tx_state_enum_type xon_tx_state; /* Xon Transmission state    */
  rex_timer_type xoff_rsp_timer;         /* Xoff Response Timer            */
  hdrfcp_msg_tx_state_enum_type xoff_tx_state; /* Xoff transmission state  */
  hdrfcp_state_enum_type state;          /* State of the stream            */
} hdrfcp_instance_type;

LOCAL struct
{
  hdrfcp_instance_type      stream[HDRFCP_NUM_STREAMS];

  /* This is a copy of the flow states for each of the stream, this is used
     read and write flow control states to/from SCMDB */
  hdrfcp_config_struct_type session_data;
} hdrfcp;

/* This macro returns the instance for a particular stream */
#define HDRFCP_GET_STREAM_INST( stream ) ((stream) - 1 )

/* This macro is used to extract stream_id from timer_id. 
 * timer_id:
 *    - argumment passed to timer cb.   
 *    - bit0,1: hdrhai_stream_enum_type, bit2: hdrfcp_timer_enum_type
 */
#define HDRFCP_TIMER_STREAMID_MASK 0x3

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_GET_INUSE_CONFIG

DESCRIPTION
  This function gets the flow control states from SCMDB.

DEPENDENCIES
  None

PARAMETERS
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrfcp_get_inuse_config(void)
{
  hdrfcp_config_struct_type  *sess_cfg = &hdrfcp.session_data;
/*--------------------------------------------------------------------------*/

  /* Get current FCP's session data from SCMDB */
  hdrscmdb_get_inuse_config(HDRHAI_FLOW_CONTROL_PROTOCOL, sess_cfg);

  /* Initialize the Flow Control Protocol states to values read from SCMDB */
  hdrfcp.stream[HDRFCP_STREAM1_INST].stream_id = sess_cfg->stream[0].stream_id;
  hdrfcp.stream[HDRFCP_STREAM2_INST].stream_id = sess_cfg->stream[1].stream_id;
  hdrfcp.stream[HDRFCP_STREAM3_INST].stream_id = sess_cfg->stream[2].stream_id;

  hdrfcp.stream[HDRFCP_STREAM1_INST].state = sess_cfg->stream[0].state;
  hdrfcp.stream[HDRFCP_STREAM2_INST].state = sess_cfg->stream[1].state;
  hdrfcp.stream[HDRFCP_STREAM3_INST].state = sess_cfg->stream[2].state;
} /* hdrfcp_get_inuse_config() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRFCP_POWERUP_INIT

DESCRIPTION
  This function performs powerup initialization for Flow Control protocol.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfcp_powerup_init( void )
{
  /* Get the flow states from SCMDB */
  hdrfcp_get_inuse_config();

  if ( hdrmc_is_first_init() )
  {
    /* Define the Xon and Xoff response timers for each stream 
     * cb parameter is timer_id which is three bits:
     * bit0,1: hdrhai_stream_enum_type, bit2: hdrfcp_timer_enum_type
     */
    rex_def_timer_ex( &hdrfcp.stream[HDRFCP_STREAM1_INST].xon_rsp_timer, 
                      hdrfcp_timer_cb, 
                      (HDRFCP_XON_TIMER << 2) | 
                      hdrfcp.stream[HDRFCP_STREAM1_INST].stream_id );

    rex_def_timer_ex( &hdrfcp.stream[HDRFCP_STREAM2_INST].xon_rsp_timer, 
                      hdrfcp_timer_cb, 
                      (HDRFCP_XON_TIMER << 2) | 
                      hdrfcp.stream[HDRFCP_STREAM2_INST].stream_id );

    rex_def_timer_ex( &hdrfcp.stream[HDRFCP_STREAM3_INST].xon_rsp_timer, 
                      hdrfcp_timer_cb, 
                      (HDRFCP_XON_TIMER << 2) | 
                      hdrfcp.stream[HDRFCP_STREAM3_INST].stream_id );

    rex_def_timer_ex( &hdrfcp.stream[HDRFCP_STREAM1_INST].xoff_rsp_timer, 
                      hdrfcp_timer_cb, 
                      (HDRFCP_XOFF_TIMER << 2) | 
                      hdrfcp.stream[HDRFCP_STREAM1_INST].stream_id );

    rex_def_timer_ex( &hdrfcp.stream[HDRFCP_STREAM2_INST].xoff_rsp_timer, 
                      hdrfcp_timer_cb, 
                      (HDRFCP_XOFF_TIMER << 2) | 
                      hdrfcp.stream[HDRFCP_STREAM2_INST].stream_id );

    rex_def_timer_ex( &hdrfcp.stream[HDRFCP_STREAM3_INST].xoff_rsp_timer, 
                      hdrfcp_timer_cb, 
                      (HDRFCP_XOFF_TIMER << 2) | 
                      hdrfcp.stream[HDRFCP_STREAM3_INST].stream_id );
  }

  /* Initialize all message transmission states to show no message
     transmissions are pending 
  */
  hdrfcp.stream[HDRFCP_STREAM1_INST].xon_tx_state = HDRFCP_NO_MSG_TX_PEND;
  hdrfcp.stream[HDRFCP_STREAM2_INST].xon_tx_state = HDRFCP_NO_MSG_TX_PEND;
  hdrfcp.stream[HDRFCP_STREAM3_INST].xon_tx_state = HDRFCP_NO_MSG_TX_PEND;

  hdrfcp.stream[HDRFCP_STREAM1_INST].xoff_tx_state = HDRFCP_NO_MSG_TX_PEND;
  hdrfcp.stream[HDRFCP_STREAM2_INST].xoff_tx_state = HDRFCP_NO_MSG_TX_PEND;
  hdrfcp.stream[HDRFCP_STREAM3_INST].xoff_tx_state = HDRFCP_NO_MSG_TX_PEND;

} /* hdrfcp_powerup_init() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_STREAM_IS_OPEN

DESCRIPTION
  This function checks the state of the specified stream and returns whether
  it is in Open state

DEPENDENCIES
  None
  
PARAMETERS
  stream_id - Stream being queried.

RETURN VALUE
  TRUE - If the stream state is FCP_OPEN and no Xoff message has been sent or
         is being sent.
  FALSE - If the stream state is FCP_CLOSED or if a Xoff message has been sent
         or is being sent.
  
SIDE EFFECTS
  None
===========================================================================*/
boolean hdrfcp_stream_is_open( hdrhai_stream_enum_type stream_id )
{

  if ( stream_id == HDRHAI_STREAM0 )
  { 
    /* Stream 0 is open by defualt */
    HDR_MSG_PROT( MSG_LEGACY_ERROR,
      "hdrfcp_stream_is_open called with stream_id 0");
    return TRUE;
  }

  /* To check if xoff_tx_state == NO_MSG_TX_PEND is not compliant with standard. 
   * According to standard, the AT should move to Close state only after receiving
   * XoffResponse. However, we only send Xoffrequest while there is no PPP, so 
   * it's safe to say stream is in close state when there is Xoff pending to be sent. 
   */
  if( ( hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].state == HDRFCP_OPEN ) 
      && ( hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].xoff_tx_state ==
           HDRFCP_NO_MSG_TX_PEND ) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* hdrfcp_stream_is_open() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRFCP_COMMIT_SESSION_DATA_TO_DB

DESCRIPTION
  This function commits the FCP's session data to EFS.

DEPENDENCIES
  This function has to be called only in HDRMC context because this in turn
  calls SCMDB routine that writes to EFS/NV, which could be done from HDRMC
  task context only.

PARAMETERS
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrfcp_commit_session_data_to_db( void )
{
  hdrfcp_config_struct_type *curr_cfg = &hdrfcp.session_data;
/*-------------------------------------------------------------------------*/

  curr_cfg->stream[0].stream_id = hdrfcp.stream[HDRFCP_STREAM1_INST].stream_id;
  curr_cfg->stream[1].stream_id = hdrfcp.stream[HDRFCP_STREAM2_INST].stream_id;
  curr_cfg->stream[2].stream_id = hdrfcp.stream[HDRFCP_STREAM3_INST].stream_id;

  curr_cfg->stream[0].state = hdrfcp.stream[HDRFCP_STREAM1_INST].state;
  curr_cfg->stream[1].state = hdrfcp.stream[HDRFCP_STREAM2_INST].state;
  curr_cfg->stream[2].state = hdrfcp.stream[HDRFCP_STREAM3_INST].state;

  hdrscmdb_set_gauped_config(HDRHAI_FLOW_CONTROL_PROTOCOL, 
                             &hdrfcp.session_data);

} /* hdrfcp_commit_session_data_to_db()  */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRFCP_SEND_XON_XOFF_REQ

DESCRIPTION
  This function is used to command the Flow Control protocol to send a XON/XOFF 
  request msg to the Access Network.

  This function ignores the stream_id. It always sends the message for
  Stream 1. It does not queue the message & does not set Xon Response timers.

DEPENDENCIES
  None
  
PARAMETERS
  sender - protocol calling this function
  stream_id - Stream for which this message applies to.
  cmd_name - XOFF/XON

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfcp_send_xon_xoff_req
(
  hdrhai_protocol_name_enum_type sender,
  hdrhai_stream_enum_type stream_id,
  hdrfcp_cmd_name_enum_type cmd_name
)
{
  hdrfcp_cmd_type hdrfcp_cmd;           /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Fill in all members of the fcp_cmd_type
  ---------------------------------------------------------------------------*/
  hdrfcp_cmd.name = cmd_name;
  hdrfcp_cmd.sender = sender;
  hdrfcp_cmd.stream_id = stream_id;

  if (rex_self() == HDRFCP_TASK_PTR)
  {
    /*-------------------------------------------------------------------------
    If some other MC processing results in activating OVHD, then this is an 
    internal command.
    -------------------------------------------------------------------------*/
    hdrfcp_process_cmd( &hdrfcp_cmd );
  }
  else
  {
    /*-------------------------------------------------------------------------
    Queue the command and tag it for the flow control protocol 
    -------------------------------------------------------------------------*/
    hdrmc_queue_cmd( HDRHAI_FLOW_CONTROL_PROTOCOL, &hdrfcp_cmd ,
                     sizeof(hdrfcp_cmd_type) );
  }
 
} /* hdrfcp_send_xon_xoff_req() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_SEND_XON_REQ

DESCRIPTION
  This function is used to command the Flow Control protocol to send a XON 
  request msg to the Access Network.

  This function ignores the stream_id. It always sends the message for
  Stream 1. It does not queue the message & does not set Xon Response timers.

DEPENDENCIES
  None
  
PARAMETERS
  sender - protocol calling this function
  stream_id - Stream for which this message applies to.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfcp_send_xon_req
(
  hdrhai_protocol_name_enum_type sender,
  hdrhai_stream_enum_type stream_id 
)
{
  hdrfcp_send_xon_xoff_req(sender, stream_id, HDRFCP_SEND_XON_CMD);
} /* hdrfcp_send_xon_req() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_SEND_XOFF_REQ

DESCRIPTION
  This function is used to command the Flow Control protocol to send a XOFF
  request msg to the Access Network.

  This function ignores the stream_id. It always sends the message for 
  Stream 1. It does not queue the message & does not set Xon Response timers.

DEPENDENCIES
  None
  
PARAMETERS
  sender - protocol calling this function
  stream_id - Stream for which this message applies to.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfcp_send_xoff_req
(
  hdrhai_protocol_name_enum_type sender ,
  hdrhai_stream_enum_type stream_id 
)
{
  hdrfcp_send_xon_xoff_req(sender, stream_id, HDRFCP_SEND_XOFF_CMD);
} /* hdrfcp_send_xoff_req() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_PROCESS_CMD

DESCRIPTION
  This function processes commands for the FCP

DEPENDENCIES
  None
  
PARAMETERS
  fcp_cmd_ptr - Pointer to the stucture containing the command

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfcp_process_cmd
(
  hdrfcp_cmd_type *fcp_cmd_ptr 
)
{
  HDRTRACE_CMD( HDRHAI_FLOW_CONTROL_PROTOCOL, fcp_cmd_ptr->name, 
                 fcp_cmd_ptr->sender, 0 );

  /*---------------------------------------------------------------------------
  Determine which command has been received.
  ---------------------------------------------------------------------------*/
  switch ( fcp_cmd_ptr->name )
  {
    case HDRFCP_SEND_XON_CMD:
      /*-----------------------------------------------------------------------
      Command to send xon message
      -----------------------------------------------------------------------*/
      hdrfcp_process_send_xon_cmd( fcp_cmd_ptr->stream_id );
      break;

    case HDRFCP_SEND_XOFF_CMD:
      /*-----------------------------------------------------------------------
      Command to send xoff message
      -----------------------------------------------------------------------*/
      hdrfcp_process_send_xoff_cmd( fcp_cmd_ptr->stream_id );
      break;

    default:
      /*-----------------------------------------------------------------------
      Unrecognized command.
      -----------------------------------------------------------------------*/
      ERR("FCP: Received Unrecognized command:%d", fcp_cmd_ptr->name, 0, 0 );
      break;
  }
} /* hdrfcp_process_cmd() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_PROCESS_SEND_XON_CMD

DESCRIPTION
  This function sends the Xon Message

DEPENDENCIES
  None
  
PARAMETERS
  stream_id - Stream for which to send the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfcp_process_send_xon_cmd
(
  hdrhai_stream_enum_type stream_id
)
{
  dsm_item_type *xon_msg_ptr; /* dsm buffer for message */
  uint32 rasm_transaction_id; /* rasm transaction id */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( stream_id == HDRHAI_STREAM0 )
  { 
    /* Stream 0 is only for signaling */
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "XON not allowed on stream 0");
    return;
  }

  /* for the same stream, if there is already an XonRequest msg queued to hmp, 
   * don't need to send a new one because there is no tranId field in the msg
   */
  if (hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].xon_tx_state !=
      HDRFCP_NO_MSG_TX_PEND)
  {
    return;
  }

  /* if there is already an xoff msg queued to hmp while trying to send XonRequest,
   * cancel it by calling rasm_acked()
   */
  if (hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].xoff_tx_state !=
      HDRFCP_NO_MSG_TX_PEND)
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, 
                    "xoff msg tx state is %d while trying to send Xon", 
                    hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].xoff_tx_state );

    /* Clear Xoff Response timer for the appropriate stream. 
     * This is needed for xoff_tx_state == MSG_WAIT_FOR_RSP case
     */
    (void) rex_clr_timer(&hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].xoff_rsp_timer);

    /* Call rasm_acked. This is needed for xoff_tx_state == MSG_WAIT_FOR_TX case */
    hdrhmp_rasm_acked(HDRHAI_FLOW_CONTROL_PROTOCOL, (HDRFCP_XOFF_REQ_MSG_ID << 16)|stream_id );

    HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xoff: msg tx state [%d->%d]", 
                    hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].xoff_tx_state,
                    HDRFCP_NO_MSG_TX_PEND);
    
    hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].xoff_tx_state =
      HDRFCP_NO_MSG_TX_PEND;
  }

/* Form Xon msg and send it */
  /* get new buffer */
  xon_msg_ptr = hdrutil_new_msg_buffer();

  /* pack message */
  HDRFCP_PACK_XON_MSG_ID( xon_msg_ptr ); 

  /* lower 16 bits of rasm_transaction_id is not important becasue there is no
   * transaction_id field in XoffRequest message
   */
  rasm_transaction_id = (HDRFCP_XON_REQ_MSG_ID << 16)|stream_id;
  
  /* don't need to retry XonRequest becasue stream state will move to OPEN after
   * queuing msg to hmp. 
   */
  hdrhmp_app_send_rasm(HDRHAI_FLOW_CONTROL_PROTOCOL,
                       stream_id,
                       0, /* number of retries */
                       HDRMFCP_XONOFF_MSG_RETRY_INTERVAL_MS, /* 1s retry interval */
                       rasm_transaction_id,
                       xon_msg_ptr,/* pointer to sig msg  */
                       hdrfcp_send_xon_outcome_cb,/* callback after message has been sent */
                       &hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)] );

  /* Change the State of the msg */
  HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xon: msg tx state [%d->%d]", 
                  hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].xon_tx_state,
                  HDRFCP_MSG_WAIT_FOR_TX);

  hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].xon_tx_state = 
    HDRFCP_MSG_WAIT_FOR_TX;

  /* Change the State of the stream for which we are sending the Xon message */
  hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].state = HDRFCP_OPEN;

  hdrfcp_commit_session_data_to_db();

} /* hdrfcp_process_send_xon_cmd() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_PROCESS_SEND_XOFF_CMD

DESCRIPTION
  This function sends the Xoff Message

DEPENDENCIES
  None
  
PARAMETERS
  stream_id - Stream for which to send the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfcp_process_send_xoff_cmd
(
  hdrhai_stream_enum_type stream_id
)
{
  dsm_item_type *xoff_msg_ptr; /* dsm buffer for message */
  uint32 rasm_transaction_id; /* rasm transaction id */
  uint8 num_retries; 

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( stream_id == HDRHAI_STREAM0 )
  { 
    /* Stream 0 is only for signaling */
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "XOFF not allowed on stream 0");
    return;
  }

  /* for the same stream, if there is already an XoffRequest msg queued to hmp, 
   * don't need to send a new one because there is no tranId field in the msg
   */
  if (hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].xoff_tx_state !=
      HDRFCP_NO_MSG_TX_PEND)
  {
    return;
  }

  /* get new buffer */
  xoff_msg_ptr = hdrutil_new_msg_buffer();
  /* pack message */
  HDRFCP_PACK_XOFF_MSG_ID( xoff_msg_ptr ); 

  /* lower 16 bits of rasm_transaction_id is not important becasue there is no
   * transaction_id field in XoffRequest message
   */
  rasm_transaction_id = (HDRFCP_XOFF_REQ_MSG_ID << 16)|stream_id;

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled(HDRHAI_FLOW_CONTROL_PROTOCOL) )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, "set num_retries to 0 in tunnel mode" );
    num_retries = 0;
  }
  else 
#endif /* FEATURE_LTE_TO_HDR_OH*/
  {
    num_retries = HDRFCP_MAX_NUM_MSG_XMIT - 1;
  }

  hdrhmp_app_send_rasm(HDRHAI_FLOW_CONTROL_PROTOCOL,
                       stream_id,
                       num_retries, /* number of retries */
                       HDRMFCP_XONOFF_MSG_RETRY_INTERVAL_MS, /* 1s retry interval */
                       rasm_transaction_id,
                       xoff_msg_ptr,/* pointer to sig msg  */
                       hdrfcp_send_xoff_outcome_cb,/* callback after message has been sent */
                       &hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)] );
 
  /* Change the State of the msg */
  HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xoff: msg tx state [%d->%d]", 
                  hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].xoff_tx_state,
                  HDRFCP_MSG_WAIT_FOR_TX);

  hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)].xoff_tx_state = 
    HDRFCP_MSG_WAIT_FOR_TX;

} /* hdrfcp_process_send_xoff_cmd() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_MSG_CB

DESCRIPTION
  This function is called from PAC when it receives a message for FCP.
  The function queues the message on the ds command queue.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr -  Received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrfcp_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  /*-----------------------------------------------------------------------
  Queue the message & tag it for the flow control protocol.
  -----------------------------------------------------------------------*/
  hdrmc_queue_msg( HDRHAI_FLOW_CONTROL_PROTOCOL , HDRHAI_IN_USE, msg_ptr );
} /* hdrfcp_msg_cb() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_SEND_DATA_READY_ACK

DESCRIPTION
  This function sends a data ready ack message in response to a received
  DataReady Message.

  The message is always sent on Stream 1

DEPENDENCIES
  None
  
PARAMETERS
  msg_ptr - Received message, without the Message ID
  stream_id - Stream on which the message was received

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfcp_send_data_ready_ack
(
  dsm_item_type *msg_ptr,
  hdrhai_stream_enum_type stream_id
)
{
  dsm_item_type *data_rdy_ack_msg_ptr; /* dsm buffer for message */
  uint8 trans_id;     /* Received Transaction ID */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the transaction ID from the message */
  if( HDRFCP_UNPACK_DATA_READY_TRANS_ID( msg_ptr, &trans_id ) != E_SUCCESS )
  {
    ERR("Unpacking DataReady Message", 0, 0, 0);
    return;
  }

  /* get new buffer */
  data_rdy_ack_msg_ptr = hdrutil_new_msg_buffer();
  /* pack message */
  HDRFCP_PACK_DATA_READY_ACK_MSG_ID( data_rdy_ack_msg_ptr ); 
  HDRFCP_PACK_DATA_READY_ACK_TRANS_ID( data_rdy_ack_msg_ptr , trans_id );

  /* If Connected, send the message on the RTC else send it on the AC */
  hdrhmp_app_send_msg( HDRHAI_FLOW_CONTROL_PROTOCOL,
                   stream_id, 
                   HDRHAI_AC_OR_RTC,
                   FALSE,
                   HDRFCP_MESSAGE_PRIORITY,
                   HDRHAI_IN_USE,
                   data_rdy_ack_msg_ptr,
                   NULL,
                   NULL );

} /* hdrfcp_send_data_ready_ack() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_PROCESS_MSG

DESCRIPTION
  This function processes received messages for FCP

DEPENDENCIES
  None

PARAMETERS
  msg_ptr -  Received message.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrfcp_process_msg
(
  dsm_item_type *msg_ptr
)
{
  uint8 msg_id;                          /* Message ID */
  hdrhai_stream_enum_type stream_id;     /* Stream for received message */
  hdrfcp_instance_type *stream_inst_ptr; /* Pointer to the FCP instance */
  uint8 protocol_id;                     /* Protocol ID */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Extract the first byte of the received message. This is the message ID.
  ---------------------------------------------------------------------------*/
  if( HDRFCP_UNPACK_MSG_ID( msg_ptr, &msg_id ) != E_SUCCESS )
  {
    ERR( "Could not extract msg_id", 0, 0, 0 );
    dsm_free_packet( &msg_ptr );
    return;
  }

  /*---------------------------------------------------------------------------
  Determine which stream the message is meant for by seeing the Protocol Id
  bits in the dsm item header
  ---------------------------------------------------------------------------*/
  protocol_id = (uint8) HDRDSM_GET_PROT_ID(msg_ptr);
  switch( protocol_id )
  {
    case HDRHAI_N_APP1_TYPE:
      stream_id = HDRHAI_STREAM1;
      break;

    case HDRHAI_N_APP2_TYPE:
      stream_id = HDRHAI_STREAM2;
      break;

    case HDRHAI_N_APP3_TYPE:
      stream_id = HDRHAI_STREAM3;
      break;

    default:
      /*---------------------------------------------------------------------
      Should never happen, since Stream 0 msgs should never be routed here
      ---------------------------------------------------------------------*/
      dsm_free_packet( &msg_ptr );
      ERR_FATAL("Invalid Protocol Id %d",protocol_id, 0, 0 );      
      break;
  }

  /*---------------------------------------------------------------------------
  Set the stream_inst_ptr to the FCP instance for the received stream
  ---------------------------------------------------------------------------*/
  stream_inst_ptr = &hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)];

  HDRTRACE_MSG( HDRHAI_FLOW_CONTROL_PROTOCOL, msg_id, stream_inst_ptr->state );

  /*---------------------------------------------------------------------------
  Determine which message is received
  ---------------------------------------------------------------------------*/
  switch( msg_id )
  {
    case HDRFCP_XON_RSP_MSG_ID :
      /*-----------------------------------------------------------------------
      Xon Response message received
      -----------------------------------------------------------------------*/
      HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "FCP Xon: stream id %d stream_inst_ptr->stream_id %d",
                      stream_id, stream_inst_ptr->stream_id);

      if((stream_inst_ptr->state == HDRFCP_OPEN) &&
         (stream_inst_ptr->xon_tx_state == HDRFCP_MSG_WAIT_FOR_RSP))
      {
        /* Clear Xon Response timer for the appropriate stream. */
        (void) rex_clr_timer( &stream_inst_ptr->xon_rsp_timer );

        HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xon: msg tx state [%d->%d]", 
                        stream_inst_ptr->xon_tx_state,
                        HDRFCP_NO_MSG_TX_PEND);

        stream_inst_ptr->xon_tx_state = HDRFCP_NO_MSG_TX_PEND;
      }
      else if((stream_inst_ptr->state == HDRFCP_OPEN) &&
              (stream_inst_ptr->xon_tx_state == HDRFCP_MSG_WAIT_FOR_TX))
      {
        HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xon: msg tx state [%d->%d]", 
                        stream_inst_ptr->xon_tx_state,
                        HDRFCP_MSG_WAIT_FOR_RASM_ACK);

        stream_inst_ptr->xon_tx_state = HDRFCP_MSG_WAIT_FOR_RASM_ACK;

        HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "FCP Xon: stream id %d stream_inst_ptr->stream_id %d",
                        stream_id, stream_inst_ptr->stream_id);

        hdrhmp_rasm_acked(HDRHAI_FLOW_CONTROL_PROTOCOL, (HDRFCP_XON_REQ_MSG_ID << 16)|stream_inst_ptr->stream_id );

      }
      else
      {
        HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                        "Unexp Xon Rsp. Stream:%d, state:%d, msg_state", 
                        stream_id, 
                        stream_inst_ptr->state, 
                        stream_inst_ptr->xon_tx_state );
      }
      break;

    case HDRFCP_XOFF_RSP_MSG_ID :  
      /*-----------------------------------------------------------------------
      Xoff Response message received
      -----------------------------------------------------------------------*/
      HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "FCP Xoff: stream id %d stream_inst_ptr->stream_id %d",
                      stream_id, stream_inst_ptr->stream_id);

      if(stream_inst_ptr->xoff_tx_state == HDRFCP_MSG_WAIT_FOR_RSP)
      {
        /* Clear Xoff Response timer for the appropriate stream. */
        (void) rex_clr_timer( &stream_inst_ptr->xoff_rsp_timer );

        HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xoff: msg tx state [%d->%d]", 
                        stream_inst_ptr->xoff_tx_state,
                        HDRFCP_NO_MSG_TX_PEND);

        stream_inst_ptr->xoff_tx_state = HDRFCP_NO_MSG_TX_PEND;

        /* change stream to close state after receiving XoffResponse */
        hdrfcp_set_stream_to_close(stream_inst_ptr->stream_id);
      }
      else if(stream_inst_ptr->xoff_tx_state == HDRFCP_MSG_WAIT_FOR_TX)
      {
        HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xoff: msg tx state [%d->%d]", 
                        stream_inst_ptr->xoff_tx_state,
                        HDRFCP_MSG_WAIT_FOR_RASM_ACK);

        stream_inst_ptr->xoff_tx_state = HDRFCP_MSG_WAIT_FOR_RASM_ACK;

        HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "FCP Xoff: stream id %d stream_inst_ptr->stream_id %d",
                        stream_id, stream_inst_ptr->stream_id);

        hdrhmp_rasm_acked(HDRHAI_FLOW_CONTROL_PROTOCOL, (HDRFCP_XOFF_REQ_MSG_ID << 16)|stream_inst_ptr->stream_id );
		
        /* change stream to close state after receiving XoffResponse */
        hdrfcp_set_stream_to_close(stream_inst_ptr->stream_id);
      }
      else
      {
        HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                        "Unexp Xoff Rsp. Stream:%d, state:%d, msg_state",
                        stream_id, 
                        stream_inst_ptr->state, 
                        stream_inst_ptr->xoff_tx_state );
      }
      break;

    case HDRFCP_DATA_READY_MSG_ID :
      /*-----------------------------------------------------------------------
      Data Ready message received
      -----------------------------------------------------------------------*/
 
      /* Send Data Ready Ack Message */
      hdrfcp_send_data_ready_ack( msg_ptr, stream_id );

      /* Give an Indication */
      hdrind_give_ind( HDRIND_FCP_DATA_READY_RCVD, &stream_id );
      break;
    
    default:
      ERR("FCP received unrecognized message: %d", msg_id, 0, 0 );
      break;
  }

  /*-----------------------------------------------------------------------
  Free the dsm item containing message
  -----------------------------------------------------------------------*/
  dsm_free_packet( &msg_ptr );

} /* hdrfcp_process_msg() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_PROCESS_XON_TIMER

DESCRIPTION
  This function is the callback function that gets called when an Xon timer
  response timer expires for a particular stream

  According to the standard, we should resend the message if the timer
  expires while we are waiting for a response. However, currently we do not
  retransmit. The reson for this is we need to handle a case for when to 
  stop. If for some reason, the Xon Requests are not being responded with 
  XonResponse, we will keep retransmitting forever.

DEPENDENCIES
  None

PARAMETERS
  stream_id - Identifies the FCP stream (instance)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrfcp_process_xon_timer
(
  uint32 stream_id
)
{
  uint8 stream_inst = HDRFCP_GET_STREAM_INST(stream_id);
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( stream_inst >= HDRFCP_NUM_STREAMS )
  {
    ERR_FATAL("Invalid stream id- %d rcvd while processing XON timer", 
               stream_id, 0, 0);
  }

  /* We only start timer after xon_tx_state is set to HDRFCP_MSG_WAIT_FOR_RSP
   * and we will cancel timer after receiving XonResponse
   */
  if( hdrfcp.stream[stream_inst].xon_tx_state != HDRFCP_MSG_WAIT_FOR_RSP ) 
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Xon timer expired on wrong state: %d",
                    hdrfcp.stream[stream_inst].xon_tx_state);

  }
  else
  {
    /* received slp cb, but didn't receive XonResponse.
     * because stream state already moved to OPEN, just change 
     * xon_tx_state 
     */ 
    HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xon: msg tx state [%d->%d]", 
                    hdrfcp.stream[stream_inst].xon_tx_state,
                    HDRFCP_NO_MSG_TX_PEND);

    hdrfcp.stream[stream_inst].xon_tx_state = HDRFCP_NO_MSG_TX_PEND;
  }
 
} /* hdrfcp_process_xon_timer() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_XOFF_TIMER_CB

DESCRIPTION
  This function is the callback function that gets called when an Xoff timer
  response timer expires for a particular stream

  According to the standard, we should resend the message if the timer
  expires while we are waiting for a response. However, currently we do not
  retransmit. The reson for this is we need to handle a case for when to 
  stop. If for some reason, the Xoff Requests are not being responded with 
  XoffResponse, we will keep retransmitting forever.

DEPENDENCIES
  None

PARAMETERS
  stream_id - Identifies the FCP stream (instance)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrfcp_process_xoff_timer
(
  uint32 stream_id
)
{
  uint8 stream_inst = HDRFCP_GET_STREAM_INST(stream_id);
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( stream_inst >= HDRFCP_NUM_STREAMS )
  {
    ERR_FATAL("Invalid stream id- %d rcvd while processing XOFF timer", 
               stream_id, 0, 0);
  }

  /* We only start timer after xoff_tx_state is set to HDRFCP_MSG_WAIT_FOR_RSP
   * and we will cancel timer after receiving XonResponse
   */
  if( hdrfcp.stream[stream_inst].xoff_tx_state != HDRFCP_MSG_WAIT_FOR_RSP ) 
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Xoff timer expired on wrong state: %d",
                    hdrfcp.stream[stream_inst].xoff_tx_state);

  }
  else
  {
    /* received slp cb, but didn't receive XoffResponse.
     * Still move stream to Close state becasue PPP has been terminated 
     */ 
    hdrfcp_set_stream_to_close((hdrhai_stream_enum_type)stream_id);
  }

} /* hdrfcp_process_xoff_timer() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_TIMER_CB

DESCRIPTION
  This function is the callback for all fcp state protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC), however
  it runs as an independent thread and thus must queue an event for HDRMC.
  
DEPENDENCIES
  None

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrfcp_timer_cb
(
  uint32 timer_id
)
{
  hdrmc_queue_timer(HDRHAI_FLOW_CONTROL_PROTOCOL, timer_id);
} /* hdrfcp_timer_cb() */

/*===========================================================================

FUNCTION HDRFCP_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the FCP.

DEPENDENCIES
  None

PARAMETERS
 timer_id - The ID of the timer that has expired

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrfcp_process_timer( uint32 timer_id )
{
  hdrfcp_timer_enum_type fcp_timer;   /* timer type */
  hdrhai_stream_enum_type stream_id;  /* Stream for this instance       */

  fcp_timer = (hdrfcp_timer_enum_type) (timer_id >> 2); 
  stream_id  = (hdrhai_stream_enum_type) (timer_id & HDRFCP_TIMER_STREAMID_MASK);

  HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP timer expired:  timer_type %d stream %d", 
                  fcp_timer, stream_id);

  switch ( fcp_timer )
  {
    case HDRFCP_XON_TIMER:
      hdrfcp_process_xon_timer(stream_id);
      break;

    case HDRFCP_XOFF_TIMER:
      hdrfcp_process_xoff_timer(stream_id);
      break;

    default :
      ERR("Invalid FCP timer type!",0,0,0);
      break;
  }
} /* hdrfcp_process_timer() */

/*===========================================================================

FUNCTION HDRFCP_SEND_XON_OUTCOME_CB

DESCRIPTION
  This is the callback function called by SLP after message transmission.
  Based on the outcome of the transmission, if the transmission is successful
  the Xon Response timer is started & state of message transmission is 
  changed. 
  
  If the transmission was not successful, we should handle this case. 
  Currently we do nothing. 

DEPENDENCIES
  None

PARAMETERS
 fcp_instance_ptr - Pointer to the FCP instance for which Xon msg was sent
 status - Out of SLP message transmission. If successful it is set to 
          E_SUCCESS

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrfcp_send_xon_outcome_cb
( 
  void *fcp_instance_ptr, 
  hdrerrno_enum_type status
)
{
  hdrfcp_instance_type *fcp_inst_ptr; /* Ptr to the FCP instance ctl blk   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fcp_inst_ptr = (hdrfcp_instance_type *) fcp_instance_ptr;

  /*-------------------------------------------------------------------------
   Ensure that we are expecting the SLP callback
  -------------------------------------------------------------------------*/
  if((fcp_inst_ptr->xon_tx_state != HDRFCP_MSG_WAIT_FOR_TX) && 
     (fcp_inst_ptr->xon_tx_state != HDRFCP_MSG_WAIT_FOR_RASM_ACK))
  {
    HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                    "Unexpected Xon SLP cb rxed FCP inst:%d stream state: %d msg state: %d",
                    fcp_inst_ptr->stream_id, fcp_inst_ptr->state,
                    fcp_inst_ptr->xon_tx_state);
    return;
  }

  switch(status)
  {
    case E_SUCCESS:
      if (fcp_inst_ptr->xon_tx_state == HDRFCP_MSG_WAIT_FOR_TX)
      {
        /*-----------------------------------------------------------------------
         Transition state to show that we received the SLP Ack & are waiting for
         the Xon Response
        -----------------------------------------------------------------------*/
        HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xon: msg tx state [%d->%d]", 
                        fcp_inst_ptr->xon_tx_state,
                        HDRFCP_MSG_WAIT_FOR_RSP);

        fcp_inst_ptr->xon_tx_state = HDRFCP_MSG_WAIT_FOR_RSP;

        /*-----------------------------------------------------------------------
         Start the timer for receiving the Xon Response
        -----------------------------------------------------------------------*/
        (void) rex_set_timer( &fcp_inst_ptr->xon_rsp_timer, HDRFCP_MSG_RSP_TIME_MSEC );
      }
      else
      {
        /* receive slp cb when xon_tx_state is in HDRFCP_MSG_WAIT_FOR_RASM_ACK:
         * it means hdrfcp has received XonResponse and called hdrhmp_rasm_acked(),
         * Don't care about slp cb 
         */ 
        HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xon: msg tx state [%d->%d]", 
                        fcp_inst_ptr->xon_tx_state,
                        HDRFCP_NO_MSG_TX_PEND);

        fcp_inst_ptr->xon_tx_state = HDRFCP_NO_MSG_TX_PEND;

      }
      break;

    case E_TIMER_EXP:
    case E_PROT_Q_FULL:
    case E_FAILURE:
    case E_AGAIN:
      /* Callback returns != E_SUCCESS, however, stream state has moved to OPEN
       * don't need to do anyting here, just change xon_tx_state 
       * (basically, we don't care about slp cb)
       */
      HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xon: msg tx state [%d->%d]", 
                      fcp_inst_ptr->xon_tx_state,
                      HDRFCP_NO_MSG_TX_PEND);
      fcp_inst_ptr->xon_tx_state = HDRFCP_NO_MSG_TX_PEND;
      break;

    default:
      HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, 
                        "Unexpected err val received for XonRequest msg. stream:%d err: %d",
                        fcp_inst_ptr->stream_id, status);

      HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xon: msg tx state [%d->%d]", 
                      fcp_inst_ptr->xon_tx_state,
                      HDRFCP_NO_MSG_TX_PEND);

      fcp_inst_ptr->xon_tx_state = HDRFCP_NO_MSG_TX_PEND;
      break;
  }

} /* hdrfcp_send_xon_outcome_cb() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRFCP_SEND_XOFF_OUTCOME_CB

DESCRIPTION
  This is the callback function called by SLP after message transmission.
  Based on the outcome of the transmission, if the transmission is successful
  the Xoff Response timer is started & state of message transmission is 
  changed. If the transmission was not successful, a Xoff message is resent.

  If the transmission was not successful, we should handle this case. 
  Currently we do nothing. 

DEPENDENCIES
  None

PARAMETERS
 fcp_instance_ptr - Pointer to the FCP instance for which Xoff msg was sent
 status - Out of SLP message transmission. If successful it is set to 
          E_SUCCESS

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrfcp_send_xoff_outcome_cb
( 
  void *fcp_instance_ptr, 
  hdrerrno_enum_type status
)
{
  hdrfcp_instance_type *fcp_inst_ptr; /* Ptr to the FCP instance ctl blk   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fcp_inst_ptr = (hdrfcp_instance_type *) fcp_instance_ptr;

  /*-------------------------------------------------------------------------
   Ensure that we are expecting the SLP callback
  -------------------------------------------------------------------------*/
  if(fcp_inst_ptr->xoff_tx_state != HDRFCP_MSG_WAIT_FOR_TX)  
  {
    if (fcp_inst_ptr->xoff_tx_state == HDRFCP_NO_MSG_TX_PEND)
    {
      /* In the case of xoff_tx_state == WAIT_FOR_RASM_ACK, we alreay called
       * hdrfcp_set_stream_to_close() after processing XoffResponse msg.
       * Ignore cb here.
       */
    }
    else
    {
      HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                        "Unexpected Xoff SLP cb rxed FCP inst:%d stream state: %d msg state: %d",
                        fcp_inst_ptr->stream_id, fcp_inst_ptr->state,
                        fcp_inst_ptr->xoff_tx_state);
    }
    return;
  }

  switch(status)
  {
    case E_SUCCESS:
      /*-----------------------------------------------------------------------
       Transition state to show that we received the SLP Ack & are waiting for
       the Xoff Response
      -----------------------------------------------------------------------*/
      HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xoff: msg tx state [%d->%d]", 
                      fcp_inst_ptr->xoff_tx_state,
                      HDRFCP_MSG_WAIT_FOR_RSP);

      fcp_inst_ptr->xoff_tx_state = HDRFCP_MSG_WAIT_FOR_RSP;

      /*-----------------------------------------------------------------------
       Start the timer for receiving the Xoff Response
      -----------------------------------------------------------------------*/
      (void) rex_set_timer( &fcp_inst_ptr->xoff_rsp_timer, HDRFCP_MSG_RSP_TIME_MSEC );
    break;

    case E_TIMER_EXP:
    case E_PROT_Q_FULL:
    case E_FAILURE:
      /* Message transmition failed.
       * Still move Stream to Close state becasue ppp has been terminated
       */
      hdrfcp_set_stream_to_close(fcp_inst_ptr->stream_id);

    break;

    default:
      HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, 
                      "Unexpected err val received for XoffRequest msg. stream:%d err: %d",
                      fcp_inst_ptr->stream_id, status);

      hdrfcp_set_stream_to_close(fcp_inst_ptr->stream_id);

    break;

  }

} /* hdrfcp_send_xoff_outcome_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRFCP_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of FCP

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of FCP

SIDE EFFECTS
  None

===========================================================================*/
void* hdrfcp_get_inconfig_handle(void)
{
  return (void*) &hdrfcp.session_data;
} /* hdrfcp_get_inconfig_handle() */

/*===========================================================================

FUNCTION HDRFCP_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function initializes Flow Control Protocol's config parameters
  to default. 

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to Flow Control Protocol's current config
                   structure in SCMDB
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrfcp_set_config_to_default
( 
  hdrfcp_config_struct_type *current_config 
)
{
  /* Initialize the IDs of each stream instance. */
  current_config->stream[0].stream_id = HDRHAI_STREAM1;
  current_config->stream[1].stream_id = HDRHAI_STREAM2;
  current_config->stream[2].stream_id = HDRHAI_STREAM3;

  /* Initialize all streams to be in Close State */
  current_config->stream[0].state = HDRFCP_CLOSED;
  current_config->stream[1].state = HDRFCP_CLOSED;
  current_config->stream[2].state = HDRFCP_CLOSED;
} /* hdrfcp_set_config_to_default() */

/*===========================================================================

FUNCTION HDRFCP_SET_STREAM_TO_CLOSE

DESCRIPTION
  This function  changes Stream state to Closed. 

DEPENDENCIES
  None

PARAMETERS
  stream_id - Stream for which this message applies to.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrfcp_set_stream_to_close
( 
  hdrhai_stream_enum_type stream_id 
)
{
  hdrfcp_instance_type *fcp_inst_ptr; /* Ptr to the FCP instance ctl blk   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( stream_id == HDRHAI_STREAM0 )
  { 
    /* Stream 0 can not be closed */
    HDR_MSG_PROT( MSG_LEGACY_ERROR,
      "hdrfcp_set_stream_to_close called with stream_id 0");
    return;
  }

  fcp_inst_ptr = &hdrfcp.stream[HDRFCP_GET_STREAM_INST(stream_id)];

  /* change stream state to Closed */
  fcp_inst_ptr->state = HDRFCP_CLOSED;

  /* commit_session_data_to_db */
  hdrfcp_commit_session_data_to_db();

  /* change msg_tx_state */
  HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xoff: msg tx state [%d->%d]", 
                  fcp_inst_ptr->xoff_tx_state,
                  HDRFCP_NO_MSG_TX_PEND);

  fcp_inst_ptr->xoff_tx_state = HDRFCP_NO_MSG_TX_PEND;

} /* hdrfcp_set_stream_to_closed() */

/*===========================================================================

FUNCTION HDRFCP_RESET_MSG_TX_STATE

DESCRIPTION
  This function reset xon/xoff tx-state 

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrfcp_reset_msg_tx_state(void)
{
  uint8 stream_id;
  hdrfcp_instance_type *stream_inst_ptr; /* Pointer to the FCP instance */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (stream_id = 0; stream_id < HDRFCP_NUM_STREAMS; stream_id++)
  {
    if (hdrfcp.stream[stream_id].xon_tx_state != HDRFCP_NO_MSG_TX_PEND)
    {
      stream_inst_ptr = &hdrfcp.stream[stream_id];

      HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xon: msg tx state [%d->%d]", 
                      hdrfcp.stream[stream_id].xon_tx_state,
                      HDRFCP_NO_MSG_TX_PEND);

      hdrfcp.stream[stream_id].xon_tx_state = HDRFCP_NO_MSG_TX_PEND;
      (void) rex_clr_timer( &stream_inst_ptr->xon_rsp_timer );

    }

    if (hdrfcp.stream[stream_id].xoff_tx_state != HDRFCP_NO_MSG_TX_PEND)
    {
      stream_inst_ptr = &hdrfcp.stream[stream_id];

      HDR_MSG_PROT_2 (MSG_LEGACY_MED, "FCP Xoff: msg tx state [%d->%d]", 
                      hdrfcp.stream[stream_id].xoff_tx_state,
                      HDRFCP_NO_MSG_TX_PEND);

      hdrfcp.stream[stream_id].xoff_tx_state = HDRFCP_NO_MSG_TX_PEND;
      (void) rex_clr_timer( &stream_inst_ptr->xoff_rsp_timer );

      /* Clear RASM queue */
      hdrhmp_clear_rasm_queue();
    }
  }

} /* hdrfcp_rest_msg_tx_state() */

/*===========================================================================

FUNCTION HDRFCP_PROCESS_IND

DESCRIPTION
  This function processes indications given to FCP

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for FCP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrfcp_process_ind 
(
  hdrind_ind_name_enum_type   ind_name,
  hdrind_ind_data_union_type *ind_data_ptr
)
{

  switch (ind_name)
  {
    case HDRIND_SCM_SESSION_CHANGED:
      /* Get the flow states from SCMDB */
      hdrfcp_get_inuse_config();
      hdrfcp_reset_msg_tx_state();
      break;

    default:
      ERR( "Unknown indication", 0, 0, 0 );
      break;
  }

} /* hdrfcp_process_ind() */
