/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              C O N N E C T E D   S T A T E   P R O T O C O L

GENERAL DESCRIPTION
  The Connected State Protocol (CON) is the main state machine
  controlling the connection layer of the HDR protocol stack.

EXTERNALIZED FUNCTIONS (Global)

  hdrcon_msg_cb()      - Callback registered with HMP to deliver 
                              messages to CON
  hdrcon_ind_cb()   - Callback registered for delivery of indications
  hdrcon_activate()        - Command function to activate CON
  hdrcon_deactivate()      - Command function to deactivate CON
  hdrcon_close_connection() - Command function to close the connection
  hdrcon_powerup_init() - Powerup Initialization command
  hdrcon_get_advertised_suspend_time() - returns advertised suspend time
  hdrcon_get_qsm_time() - returns qsm time

EXTERNALIZED FUNCTIONS (Regional)
  hdrcon_process_cmd()     - Function called by hdrmc task to process
                              a command for CON
  hdrcon_process_ind()     - Function called by hdrmc task to process
                              an indication for CON
  hdrcon_process_msg()     - Function called by hdrmc task to process
                              a message for CON
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrcon.c_v   1.31   09 Apr 2003 09:20:10   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrcon.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
07/27/11   cnx     Merged Optimized Handoff changes.
03/24/10   pxu     Removed FEATURE_HDR_QSM features
12/01/09   suren   Added QChat optimization changes.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/27/09   wsh     Added run-time check for KDDI feature flags
02/18/09   wsh     Updated file to include private header hdrmc_v.h
01/23/09   wsh     Fixed crash in logging
12/10/08   smd     Replaced assert.h with amssassert.h
06/16/08   pba     Clear connection_is_closing flag when CC msg is received in
                   wrong state.
04/09/08   sju     Added support to indicate if CC was received
07/19/07   sju     Added sending TCC+CC(reason0x2) if invalid RUP1 TCA was received
04/27/07   wsh     Changed resend ConnectionClose trace to avoid confusion
04/03/07   wsh     Fixed error where dsm_item is not freed
01/11/07   sju     Added support to check if CC and TCA are in the same 
                   connection layer packet.
11/07/06   wsh     Make sure ConnectionClose message is sent 3 times if AT is
                   attempting to close connection. If replying to AN's ConClose
                   only send 1 time.
11/02/06   pba     Ignore ConnectionClose message when HDCON is in closing state
09/28/06   pba     Fixed QSM timer values.
09/22/06   pba     Changed QSM timer duration to 30 secs for connection close
                   after session negotiation and for rest of the cases to 60secs.
09/22/06   pba     Set QSM timer on hdrcon process deactiavte instead of hdralmp
                   connection close command.
09/05/06   mpa     Added support for FEATURE_HDR_QSM
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
12/06/05   etv     Flush pending ACKs before connection close
08/22/05   hal     Featurized CloseReason 011 (1x handdown) under feature
                   FEATURE_HDR_REVA_L1
07/12/05   hal     Added support for CloseReason 011 (1x handdown)
                   Added parameter hdrhai_abort_reason_enum_type reason to
                   hdrcon_close_connection()
04/04/05   pba     Removed Lint errors
12/13/04   hal     Changed dsm_offset_new_buffer to use hdrutil_new_buffer.
11/29/04   grl     Added support for connected state stat logging.
09/30/03   mpa     Added feature to disable the suspend timer.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 SSID
04/03/03   mpa     Use hdrhmp_send_msg_on_existing_rtc for ConnectionClose msg
03/05/03   dna     Added hdrcon_connection_is_closing
                   Don't send ConnectionClose if PA is not available
                   Clean up powerup_init processing
03/18/02   nmw     No longer writing Suspend Time to NV.
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/18/01   sh      Added protocol logging support and reason code for deactivation
06/06/01   dna     new param for hmp_send_msg()
01/26/01   dna     Fixed bug in hdrcon.suspend_time.
01/26/01   dna     Fixed bug left in hdrcon_close_msg_sent_cb()
01/13/01   dna     Changed close connection msg tx processing.
11/01/00   nmn     Added trace
07/28/00   nmn     Created module.

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
#include "hdrutil.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif


#include "hdrhai.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#include "hdrmci.h"
#include "hdrcon.h"
#include "hdrconi.h"
#include "hdrcons.h"
#include "hdrerrno.h"
#include "hdrhmp.h"
#include "hdrrmac.h"
#include "hdrbit.h"
#include "hdrind.h"
#include "hdrts.h"
#include "qw.h"
#include "hdrtrace.h"
#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif
#include "hdrplog.h"

#include "hdrslp.h"
#include "hdrslpi.h"
#include "hdrslptx.h"

#ifdef FEATURE_HDR_QCHAT
#include "hdrqchatupk.h"
#include "hdrqchatupk_opts.h"
#endif /*FEATURE_HDR_QCHAT*/

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* Message IDs */
#define HDRCON_CONNECTION_CLOSE_MSG 0x00     /* Connection Close Message ID */

typedef enum
{
  HDRCON_CLOSE_NORMAL = 0, /* Normal Close */
  HDRCON_CLOSE_REPLY = 1,  /* Reply to Close */
  HDRCON_CLOSE_ERROR = 2,   /* Connection Error */
  HDRCON_CLOSE_HANDDOWN = 3 /* 1x Handdown */
} hdrcon_close_reason_enum_type;

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRCON_TASK_PTR hdrmc_tcb_ptr

typedef struct
{
  uint8 message_id;
  hdrcon_close_reason_enum_type close_reason;
  boolean suspend_enable;
  qword suspend_time;
} hdrconi_connection_close_msg_type;

/****************************
** The suspend time is currently hard-coded as five seconds.  This number
** was reached because, after some studies, it becomes clear that if no
** traffic is requested after 9-10 seconds, the likelyhood that traffic is
** requested within the following 10+ seconds drops off.  Since this is not
** started until after the dormant timer expires, only an additional five
** seconds is needed here.  Since this is in units of 80ms, there are 12.5
** units per second.  12.5 multiplied by 5 yields 62.5, rounded down to 62.
****************************/
uint32 HDRCON_SUSPEND_TIME_LENGTH = 0x0000003E;

/****************************
** system time 0x0 is a time far in the past, it is assumed the system time
** will never reach this time.  Therefore, this value is used to indicate
** that no system time has been sent to the AN for this session.
****************************/
qword HDRCON_INVALID_SUSPEND_TIME = {0x00000000,0x00000000};

LOCAL struct
{
  qword suspend_time;
  /* Absolute system time to suspend sleep until */
  boolean connection_is_closing; 
  /* Flag while connection close in progress */
  boolean cc_received;
  /* Flag to check if ConClose is received from AN */
  boolean hho_is_in_progress;
  /* Flag while connection close is for hard handoff (with TCA in the same 
   * connection layer packet)
   */
  uint32 connection_close_count;
  /* number of retries to send connection close */
  dsm_item_type *connection_close_msg_ptr;
  /* duplicated connection_close_msg */
} hdrcon;

/* <EJECT> */ 
/*===========================================================================

                   MESSAGE PACK / UNPACK MACROS

===========================================================================*/

/**********************
** All Pack routines take a pointer to a dsm_item and
** a variable of type hdrconi_connection_close_msg_type
**********************/
#define HDRCON_PACK_CC_MSG_ID( msg_ptr, message_id ) \
          (HDRBIT_PACK8(msg_ptr, (uint8) message_id, 0, 8))
#define HDRCON_PACK_CC_CLOSE_REASON( msg_ptr, close_reason ) \
          (HDRBIT_PACK8(msg_ptr, (uint8) close_reason, 8, 3))
#define HDRCON_PACK_CC_SUSPEND_ENABLE( msg_ptr, suspend_enable ) \
          (HDRBIT_PACK8(msg_ptr, (uint8) suspend_enable, 11, 1))
#define HDRCON_PACK_CC_SUSPEND_TIME( msg_ptr, suspend_time ) \
           HDRBIT_PACK32(msg_ptr, qw_hi(suspend_time), 12, 4); \
           HDRBIT_PACK32(msg_ptr, qw_lo(suspend_time), 16, 32)

/**********************
** Unpack routines.  All take a pointer to a DSM item.  MSG_ID takes
** a uint8, CLOSE_REASON takes a con_close_reason_enum_type, SUSPEND_ENABLE
** takes a boolean, SUSPEND_TIME takes the return code and a qword
**********************/
#define HDRCON_UNPACK_MSG_ID( msg_ptr, msg_id_ptr) \
          (HDRBIT_UNPACK8(msg_ptr, (msg_id_ptr), 0, 8))
#define HDRCON_UNPACK_CC_CLOSE_REASON( msg_ptr, c_r_ptr ) \
          (HDRBIT_UNPACK8(msg_ptr, (uint8 *)(c_r_ptr), 8, 3)) 
#define HDRCON_UNPACK_CC_SUSPEND_ENABLE( msg_ptr, s_e_ptr ) \
          (HDRBIT_UNPACK8(msg_ptr, (s_e_ptr), 11, 1))

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

LOCAL void hdrcon_close_msg_sent_cb
( 
  void * data_ptr,
  hdrerrno_enum_type err_val
);

LOCAL void hdrcon_process_resend_con_close
(
  hdrcon_cmd_type * con_cmd_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_SEND_CON_CLOSE_MSG

DESCRIPTION
  This function sends a connection close message

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrcon_send_con_close_msg
(
  hdrcon_close_reason_enum_type close_reason
)
{
  /* points to the DSM item where the packed data is kept */
  dsm_item_type * msg_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* While in the process of closing a connection, all other request to
   * close connection should be ignored. */
  if (hdrcon.connection_close_count != 0)
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "Rcvd ConnectionClose while closing, ignore");
    return;
  }

  if (hdrrmac_pa_is_available(HDRHAI_CONNECTED_STATE_PROTOCOL))
  {
    /* Flush pending ACKs for all reliable messages sent by AN before 
       closing the connection */
    hdrslp_flush_pending_acks();

    /* Set suspend time to current time + suspend period */
    hdrts_get_80ms_time ( hdrcon.suspend_time );

    msg_ptr = hdrutil_new_msg_buffer();
    HDRCON_PACK_CC_MSG_ID ( msg_ptr, HDRCON_CONNECTION_CLOSE_MSG );
    HDRCON_PACK_CC_CLOSE_REASON ( msg_ptr, close_reason );

    if (hdrmc_feature_is_enabled(
          HDRMC_FEATURE_HDR_DISABLE_SUSPEND_TIMER))
    {      
      HDRCON_PACK_CC_SUSPEND_ENABLE ( msg_ptr, FALSE );
    }
    else
    {
      qw_inc ( hdrcon.suspend_time, HDRCON_SUSPEND_TIME_LENGTH );
      HDRCON_PACK_CC_SUSPEND_ENABLE ( msg_ptr, TRUE );
      HDRCON_PACK_CC_SUSPEND_TIME ( msg_ptr, hdrcon.suspend_time );
    }

    if (close_reason == HDRCON_CLOSE_REPLY)
    {
      /* if we are replying to a ConCloseMsg from AN,
       * we don't need to send it 3 times 
       */
      hdrcon.connection_close_count = 3;
    }
    else
    {
      /* otherwise, we need to send ConCloseMsg 3 times
       * per carrier requirement, to ensure AN got it
       */
      hdrcon.connection_close_count = 1;
    }

    /* if this is the first attempt to send connection close,
     * construct the message and keeps a copy for retry */

    (void) dsm_dup_packet(&hdrcon.connection_close_msg_ptr, 
            msg_ptr, 0, dsm_length_packet(msg_ptr));

    hdrhmp_send_msg_on_existing_rtc( HDRHAI_CONNECTED_STATE_PROTOCOL,
                     FALSE,
                     40,
                     HDRHAI_IN_USE, /* Instance of protocol sending message */
                     msg_ptr,
                     (hdrslp_outcome_cb_type) hdrcon_close_msg_sent_cb,
                     NULL );    
  }
  else
  {
    /* Just fail the ConnectionClose message immediately rather than
     * potentially waiting for 5s for supervision to fail.  This 
     * allows the connectino to close quicker when the link is bad.
     */
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "DRC Supervision:No ConnectionClose sent!");
    hdrrmac_shutdown ( HDRHAI_CONNECTED_STATE_PROTOCOL );
  }

 if (close_reason == HDRCON_CLOSE_REPLY)
  {
    hdrplog_set_normal_conn_rel_reason(HDRPLOG_CONN_REL_AN);

    /* Count the number of times the AN terminates the connection
     * for logging purposes.
     */
    hdrplog_inc_conn_term_stats_counter(
      HDRPLOG_CONNECTION_CLOSE_RECEIVED_COUNT, 1 );
  }
  else
  {
    hdrplog_set_normal_conn_rel_reason(HDRPLOG_CONN_REL_AT);

    /* Count the number of times the AT terminates the connection
     * for logging purposes.
     */
    hdrplog_inc_conn_term_stats_counter(
      HDRPLOG_CONNECTION_CLOSE_SENT_COUNT, 1 );
  }

  /* Flag that we are in the process of closing the connection. */
  hdrcon.connection_is_closing = TRUE;

} /* hdrcon_send_con_close_msg() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_GET_ADVERTISED_SUSPEND_TIME

DESCRIPTION
  This function returns the suspend time advertised last time we closed a
  connection.  If the CON is in the open state when this function is
  called, it will return an error.  Nobody should ever need the suspend
  time while the connection is open.

DEPENDENCIES
  None

PARAMETERS
  suspend - the value of the suspend time sent over the air in the
            ConnectionClose Message
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - a valid suspend time is returned
  E_NOT_AVAILABLE - No suspend time has been sent, so the requested data
                    is not available.
  E_NOT_ALLOWED - the Connected State Protocol is in Open State (thus making
              the request invalid)

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrcon_get_advertised_suspend_time
(
  qword suspend,
  hdrhai_protocol_name_enum_type caller
)
{
  hdrerrno_enum_type ret_val = E_SUCCESS; /* Assume success */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_CON_GET_ADVERTISED_SUSPEND_TIME( caller, hdrcons_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  if (HDRCONS_OPEN == hdrcons_get_state())
  {
    ret_val = E_NOT_ALLOWED;
  }
  /* similar to the C++ string compare functions, this function returns
     0 if the two words are equal. */
  else if (qw_cmp(HDRCON_INVALID_SUSPEND_TIME,hdrcon.suspend_time) == 0)
  {
    ret_val = E_NOT_AVAILABLE;
  }
  else
  {
    qw_equ ( suspend, hdrcon.suspend_time );
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  return ret_val;
} /* hdrcon_get_advertised_suspend_time */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_CONNECTION_IS_CLOSING

DESCRIPTION
  This accessor function tells if the connection is closing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if the connection is in the process of closing (i.e. a connection
       close message is being sent)
  FALSE otherwise     

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrcon_connection_is_closing
( 
  hdrhai_protocol_name_enum_type caller
)
{
  SYS_ARG_NOT_USED(caller); /* remove this line when caller is used */

  return(hdrcon.connection_is_closing);
} /* hdrcon_connection_is_closing() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_CC_RECEIVED

DESCRIPTION
  This accessor function tells if AN has sent a ConnectionClose from AT

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if AN has sent a connection to AT
  FALSE otherwise     

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrcon_cc_received
( 
  hdrhai_protocol_name_enum_type caller
)
{
  SYS_ARG_NOT_USED(caller); /* remove this line when caller is used */

  return(hdrcon.cc_received);
} /* hdrcon_cc_received() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_PROCESS_CONNECTION_CLOSE_MSG

DESCRIPTION
  This function processes the Connection Close Message

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the dsm item containing the message

RETURN VALUE
  None.

SIDE EFFECTS
  Changes State.

===========================================================================*/
LOCAL void hdrcon_process_connection_close_msg
(
  dsm_item_type * item_ptr
)
{
  /* Holds the value of the received suspend_enable field of the msg */
  boolean cc_received_enable;
  /* Holds the value of the received close_reason field of the msg */
  hdrcon_close_reason_enum_type cc_received_close_reason;
  /* used to keep track of the return codes from unpacking */
  uint8 return_codes = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (HDRCONS_OPEN != hdrcons_get_state())  
  {
    HDRTRACE_RXMSG_IGNORED_STATE( HDRHAI_CONNECTED_STATE_PROTOCOL,
      HDRCON_CONNECTION_CLOSE_MSG, hdrcons_get_state() ); //lint !e506

    /* CC msg is received in wrong state, so reset the connection_is_closing 
       flag because we are going to ignore connection close altogther. If we 
       recvd multiple CC msgs but in correct state then we should not reset 
       because connection is actually going to be closed, only that subsequent 
       (2nd, 3rd...) CC msgs will be ignored */
    hdrcon.connection_is_closing = FALSE;
    hdrcon.cc_received = FALSE;

    return;
  }

  /* Check if the AT is in the process of sending AT initiated CC*/
  if (hdrcon.connection_close_count != 0)
  {
    /* This Check is due to issue reported earlier(ignore AN initiated CC if the AT 
     * is in the process of sending CC) and may need to be revised */
    HDRTRACE_RXMSG_IGNORED_STATE( HDRHAI_CONNECTED_STATE_PROTOCOL,
      HDRCON_CONNECTION_CLOSE_MSG, hdrcons_get_state() ); //lint !e506

    /* if received a ConnectionClose message from AN, need not resend 
       ConnectionClose, set retry to max value. */
    HDR_MSG_PROT_1(MSG_LEGACY_MED, 
                   "Recv. ConClose while closing. Increase ConClose retry count %d->3",
                   hdrcon.connection_close_count);
    hdrcon.connection_close_count = 3;
    return;
  }


  return_codes |= HDRCON_UNPACK_CC_CLOSE_REASON ( item_ptr, 
                  &cc_received_close_reason);
  if (HDRCON_CLOSE_REPLY == cc_received_close_reason)
  {
    ERR ("AN send close reply in Open State", 0, 0, 0);
  }

  return_codes |= HDRCON_UNPACK_CC_SUSPEND_ENABLE( item_ptr,
                    &cc_received_enable);

  if (E_SUCCESS != return_codes)
  {
    ERR ("Failure unpacking Connection Close Msg", 0, 0, 0);
  }

  else if (cc_received_enable)
  {
    ERR ("AN sent suspend time", 0, 0, 0);
  }
  
  hdrcon_send_con_close_msg( HDRCON_CLOSE_REPLY );

  /* Note: we will not change state to inactive until we receive
     the Deactivate Command.  */

} /* hdrcon_process_connection_close_msg */

/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRCON_PROCESS_DEACTIVATE

DESCRIPTION
  This function processes the Deactivate Command

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Changes State.

===========================================================================*/
LOCAL void hdrcon_process_deactivate 
( 
  hdrcon_cmd_type * con_cmd_ptr 
)
{
  if ( HDRCONS_OPEN == hdrcons_get_state() )
  {
    hdrplog_log_conn_rel(hdrplog_trans_abort_reason_for_rel
                           (con_cmd_ptr->data.abort_reason));
    hdrcons_change_state(HDRCONS_INACTIVE);
  }
  else // command is ignored, incorrect state
  {
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_CONNECTED_STATE_PROTOCOL,
      con_cmd_ptr->name, con_cmd_ptr->sender, hdrcons_get_state() );
  }
      
  /* The connection is closed */
  hdrcon.connection_is_closing = FALSE;
  hdrcon.cc_received = FALSE;
  hdrcon.connection_close_count = 0;

} /* hdrcon_process_deactivate() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_PROCESS_CLOSE_CONNECTION

DESCRIPTION
  This function processes the Close Connection Command

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Changes State.

===========================================================================*/
LOCAL void hdrcon_process_close_connection 
(
  hdrcon_cmd_type * con_cmd_ptr
)
{

  if (HDRCONS_OPEN != hdrcons_get_state())
  {
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_CONNECTED_STATE_PROTOCOL,
      con_cmd_ptr->name, con_cmd_ptr->sender, hdrcons_get_state() );

    return;
  }

  if (con_cmd_ptr->data.abort_reason == HDRHAI_ABORT_ERROR)
  {
    hdrcon_send_con_close_msg( HDRCON_CLOSE_ERROR );
  }
  #ifdef FEATURE_HDR_REVA_L1
  else if( con_cmd_ptr->data.abort_reason == HDRHAI_ABORT_RESELECT )
  {
    hdrcon_send_con_close_msg( HDRCON_CLOSE_HANDDOWN );
  }
  #endif /* FEATURE_HDR_REVA_L1 */
  else
  {
    hdrcon_send_con_close_msg( HDRCON_CLOSE_NORMAL );
  }
  /* Note: we will not change state to inactive until we receive
     the Deactivate Command */

} /* hdrcon_process_close_connection() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_PROCESS_SESSION_CLOSE

DESCRIPTION
  This function processes the Session Close Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Changes state.

===========================================================================*/
LOCAL void hdrcon_process_session_close ( void )
{
  qw_equ (hdrcon.suspend_time, HDRCON_INVALID_SUSPEND_TIME);
} /* hdrcon_process_session_close */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_PROCESS_ACTIVATE

DESCRIPTION
  This function processes the Activate command

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Changes state.

===========================================================================*/
LOCAL void hdrcon_process_activate 
( 
  hdrcon_cmd_type * con_cmd_ptr 
)
{
  if (HDRCONS_INACTIVE == hdrcons_get_state())
  {
    hdrcons_change_state (HDRCONS_OPEN);
  }
  else // Command ignored, incorrect state
  {
    HDRTRACE_CMD_IGNORED_STATE(HDRHAI_CONNECTED_STATE_PROTOCOL,
      con_cmd_ptr->name, con_cmd_ptr->sender, hdrcons_get_state() );
  }
} /* hdrcon_process_activate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRCON.

DEPENDENCIES
  None
  
PARAMETERS
  msg - Pointer to the DSM item chain holding the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

void hdrcon_msg_cb( dsm_item_type * msg_ptr )
{
  uint8 msg_id;           /* Message identifier */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* MessageID is the first uint8 */
  if (E_SUCCESS != HDRCON_UNPACK_MSG_ID(msg_ptr, &msg_id) )
  {
    ERR( "Could not extract message_id", 0, 0, 0 );
    dsm_free_packet( &msg_ptr );
    return;
  }

  if ((msg_id == HDRCON_CONNECTION_CLOSE_MSG))
  {
    /* We set this flag to TRUE in two places( the other place is after
     * queue ConnectionClose to hdrhmp)
     * Setting flag to TRUE here so hdrscp can decide if personality 
     * switch is pending or not( if AT receive AUR(new session token) and CC
     * it means PS is pending.),Otherwise, the SLP may not get the correct 
     * subtype from SCP to parse the incoming messages sent after CC.
     */ 
    hdrcon.connection_is_closing = TRUE;
    hdrcon.cc_received = TRUE;
  }

  hdrmc_queue_msg( HDRHAI_CONNECTED_STATE_PROTOCOL, 
    HDRHAI_IN_USE, msg_ptr );
}

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRCON.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

void hdrcon_ind_cb( hdrind_ind_name_enum_type ind_name,
                               void * ind_data_ptr )
{
  if (rex_self() == HDRCON_TASK_PTR)
  {
    hdrcon_process_ind(ind_name, (hdrind_ind_data_union_type *)ind_data_ptr);
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_CONNECTED_STATE_PROTOCOL, ind_name, ind_data_ptr );
  }
} /* hdrcon_ind_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_POWERUP_INIT

DESCRIPTION
  This function commands CON to Initialize.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcon_powerup_init ( void )
{
  qw_equ (hdrcon.suspend_time, HDRCON_INVALID_SUSPEND_TIME);

  /* The connection is closed */
  hdrcon.connection_is_closing = FALSE;
  hdrcon.cc_received = FALSE;
  hdrcon.connection_close_msg_ptr = NULL;
  hdrcon.connection_close_count = 0;
  hdrcon.hho_is_in_progress = FALSE;
} /* hdrcon_init */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_CLOSE_MSG_SENT_CB

DESCRIPTION
  This function tells CON that the Connection Close Message was sent.
  Since this message is best effort on the RTC, this function is called
  either when the message is transmitted or failed. We don't care which.

DEPENDENCIES
  None

PARAMETERS
  data_ptr - NULL
  err_val - ERRNO describing the result of the message transmission. Ignored.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrcon_close_msg_sent_cb
( 
  void *data_ptr,
  hdrerrno_enum_type err_val
)
{
  hdrcon_cmd_type hdrcon_cmd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "hdrcon_close_msg_sent_cb() retry count%d", 
                  hdrcon.connection_close_count);

  hdrcon_cmd.name = HDRCON_RESEND_CON_CLOSE_CMD;
  hdrcon_cmd.sender = HDRHAI_CONNECTED_STATE_PROTOCOL;

  hdrmc_queue_cmd( HDRHAI_CONNECTED_STATE_PROTOCOL, &hdrcon_cmd,
                   sizeof (hdrcon_cmd) );
} /* hdrcon_close_msg_sent_cb */ //lint !e715 data_ptr and err_val not used

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_SET_HHO_IS_IN_PROGRESS_FLAG

DESCRIPTION
  This function set the hdrcon.hho_is_in_progress flag  

DEPENDENCIES
  None

PARAMETERS
  hho_is_in_progress   - TRUE or FALSE

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcon_set_hho_is_in_progress_flag
( 
  boolean hho_is_in_progress
)
{    
  hdrcon.hho_is_in_progress = hho_is_in_progress;

  if (hdrcon.hho_is_in_progress == TRUE)
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH,"Next ConnectionClose is for HHO");
  }

} /*hdrcon_set_hho_is_in_progress_flag*/

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_HHO_IS_IN_PROGRESS

DESCRIPTION
  This function retruns if HHO  is in progress 

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  TRUE/FALSE - if HHO is in progress of not 

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrcon_hho_is_in_progress(void)
{    
  return hdrcon.hho_is_in_progress;

} /* hdrcon_hho_is_in_progress */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_ACTIVATE

DESCRIPTION
  This function commands CON to activate.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcon_activate( hdrhai_protocol_name_enum_type sender )
{
  hdrcon_cmd_type hdrcon_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDRCON_ACTIVATE_CMD received %d", 
                  hdrcons_get_state());

  /* Fill in all members of the hdrcon_cmd_type for the Activate command */
  hdrcon_cmd.name = HDRCON_ACTIVATE_CMD;
  hdrcon_cmd.sender = sender;

  if (rex_self() == HDRCON_TASK_PTR)
  {
    hdrcon_process_activate( &hdrcon_cmd );
  }
  else
  {
    /* This function copies the hdrcon_cmd into a buffer before it returns */
    hdrmc_queue_cmd( HDRHAI_CONNECTED_STATE_PROTOCOL, &hdrcon_cmd, 
                     sizeof (hdrcon_cmd) );
  }
} /* hdrcon_activate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_CLOSE_CONNECTION

DESCRIPTION
  This function commands HDRCON to close the connection

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  reason - The reason the connection is being closed

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcon_close_connection
( 
  hdrhai_protocol_name_enum_type sender,
  hdrhai_abort_reason_enum_type reason
)
{
  hdrcon_cmd_type hdrcon_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDRCON_CLOSE_CONNECTION_CMD received %d", 
                  hdrcons_get_state());

  /* Fill in all members of the hdrcon_cmd_type for the Activate command */
  hdrcon_cmd.name = HDRCON_CLOSE_CONNECTION_CMD;
  hdrcon_cmd.sender = sender;
  hdrcon_cmd.data.abort_reason = reason;

  if (rex_self() == HDRCON_TASK_PTR)
  {
    hdrcon_process_close_connection( &hdrcon_cmd );
  }
  else
  {
    /* This function copies the hdrcon_cmd into a buffer before it returns */
    hdrmc_queue_cmd( HDRHAI_CONNECTED_STATE_PROTOCOL, &hdrcon_cmd,
                     sizeof (hdrcon_cmd) );
  }
} /* hdrcon_close_connection */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_DEACTIVATE

DESCRIPTION
  This function commands HDRCON to deactivate

DEPENDENCIES
  None

PARAMETERS
  reason - abort reason for deactivattion
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcon_deactivate
( 
  hdrhai_abort_reason_enum_type reason,
  hdrhai_protocol_name_enum_type sender
)
{
  hdrcon_cmd_type hdrcon_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDRCON_DEACTIVATE_CMD received %d", 
                  hdrcons_get_state());

  /* Fill in all members of the hdrcon_cmd_type for the Activate command */
  hdrcon_cmd.name = HDRCON_DEACTIVATE_CMD;
  hdrcon_cmd.sender = sender;
  hdrcon_cmd.data.abort_reason = reason;

  if (rex_self() == HDRCON_TASK_PTR)
  {
    hdrcon_process_deactivate( &hdrcon_cmd );
  }
  else
  {
    /* This function copies the hdrcon_cmd into a buffer before it returns */
    hdrmc_queue_cmd( HDRHAI_CONNECTED_STATE_PROTOCOL, &hdrcon_cmd,
                     sizeof (hdrcon_cmd) );
  }
} /* hdrcon_deactivate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_PROCESS_CMD

DESCRIPTION
  This function processes commands given to CON.

DEPENDENCIES
  None

PARAMETERS
  con_cmd_ptr - Pointer to the command for CON to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcon_process_cmd( hdrcon_cmd_type * con_cmd_ptr )
{
  HDRTRACE_CMD( HDRHAI_CONNECTED_STATE_PROTOCOL, con_cmd_ptr->name, 
    con_cmd_ptr->sender, hdrcons_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch ( con_cmd_ptr->name )
  {
    case HDRCON_ACTIVATE_CMD:        /* Connection is Active */
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Activate Command");
      hdrcon_process_activate( con_cmd_ptr);
      break;

    case HDRCON_DEACTIVATE_CMD:      /* Deactivate the protocol */
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Deactivate Command");
      hdrcon_process_deactivate( con_cmd_ptr);
      break;

    case HDRCON_CLOSE_CONNECTION_CMD: /* Close the connection */
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Close Connection Comand");
      hdrcon_process_close_connection( con_cmd_ptr );
      break;

    case HDRCON_RESEND_CON_CLOSE_CMD:   /* resend connection close */
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Resend Close Connection Comand");
      hdrcon_process_resend_con_close( con_cmd_ptr );
      break;

    default:
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrcon_process_cmd */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_PROCESS_IND

DESCRIPTION
  This function processes indications given to CON.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for CON to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcon_process_ind( hdrind_ind_name_enum_type ind_name, 
                          hdrind_ind_data_union_type * ind_data_ptr )
{
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrind_ind_data_union_type ind_data;
  ind_data.hho_is_in_progress = hdrcon_hho_is_in_progress();
#endif /* FEATURE_LTE_TO_HDR_OH */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_IND( HDRHAI_CONNECTED_STATE_PROTOCOL, ind_name, 
    hdrcons_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch ( ind_name )
  {
    case HDRIND_SMP_SESSION_CLOSED:
      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Sesson Closed Indication received");
      hdrcon_process_session_close ();
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRIND_SAP_CONNECTION_CLOSED:
      hdrind_give_ind( HDRIND_CONNECTED_CONNECTION_CLOSED, &ind_data);
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default:
      ERR ("Connected State does not receive indications", 0, 0, 0);
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrcon_process_ind */ //lint !e715 ind_data_ptr not used

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_PROCESS_MSG

DESCRIPTION
  This function processes messages for CON.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcon_process_msg
( 
  dsm_item_type * item_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;           /* Message identifier */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG)); 

  /* MessageID is the first uint8 */
  if (E_SUCCESS != HDRCON_UNPACK_MSG_ID(item_ptr, &msg_id) )
  {
    ERR( "Could not extract message_id", 0, 0, 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  /* Currently only the in use instance receives any messages */
  if (protocol_instance != HDRHAI_IN_USE)
  {
    ERR("Msg for wrong instance %d", msg_id, 0, 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  HDRTRACE_MSG( HDRHAI_CONNECTED_STATE_PROTOCOL, msg_id, 
    hdrcons_get_state() );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch ( msg_id )
  {
    case HDRCON_CONNECTION_CLOSE_MSG:
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Connection close message received");
#ifdef FEATURE_HDR_QCHAT
      if (hdrqchatupk_mod_enabled()) 
      {
        hdrqchatupk_close_connection();
      }
#endif /*FEATURE_HDR_QCHAT*/       
      hdrcon_process_connection_close_msg(item_ptr);
      break;

    default:
      ERR ("HDRCON unknown in use message %d", msg_id, 0, 0);
      break;
  }
  dsm_free_packet( &item_ptr );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrcon_process_msg */

/*===========================================================================

FUNCTION HDRCON_PROCESS_RESEND_CON_CLOSE

DESCRIPTION
  This function sends the connection close message a subsequent time if 
  appropriate

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrcon_process_resend_con_close
(
  hdrcon_cmd_type * con_cmd_ptr
)
{
  /* points to the DSM item where the packed data is kept */
  dsm_item_type * msg_ptr = NULL;
  boolean send_msg_again = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (HDRCONS_OPEN != hdrcons_get_state())
  {
    HDRTRACE_CMD_IGNORED_STATE( HDRHAI_CONNECTED_STATE_PROTOCOL,
        con_cmd_ptr->name, con_cmd_ptr->sender, hdrcons_get_state() );
  }
  /* While in the process of closing a connection, all other request to
   * close connection should be ignored. */
  else if (hdrcon.connection_close_msg_ptr == NULL) 
  {
    HDR_MSG_PROT (MSG_LEGACY_ERROR, "Attempt to re-send null con close");
  }
  else if (!hdrrmac_pa_is_available(HDRHAI_CONNECTED_STATE_PROTOCOL))
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "PA is off, can't resend");
  }
  else if (hdrcon.connection_close_count >= 3)
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "All retries have been sent.  Closing");
  } 
  else 
  {
    send_msg_again = TRUE;
  }

  if (send_msg_again)
  {
    (void) dsm_dup_packet(&msg_ptr, hdrcon.connection_close_msg_ptr, 0,
                   dsm_length_packet(hdrcon.connection_close_msg_ptr));
    hdrcon.connection_close_count++;
    hdrhmp_send_msg_on_existing_rtc( HDRHAI_CONNECTED_STATE_PROTOCOL,
                     FALSE,
                     40,
                     HDRHAI_IN_USE, /* Instance of protocol sending message */
                     msg_ptr,
                     (hdrslp_outcome_cb_type) hdrcon_close_msg_sent_cb,
                     NULL );    
  }
  else
  {
    /* Command the RTCMAC to shut down.  It will indicate ALMP when finished.
     * This step is required to ensure the frame containing the close message
     * is transmitted before RTCMAC is deactivated. Also frees the duplicated
     * connection_close_msg
     */  
    dsm_free_packet(&hdrcon.connection_close_msg_ptr);
    hdrrmac_shutdown ( HDRHAI_CONNECTED_STATE_PROTOCOL );
  }
} /* hdrcon_send_con_close_msg() */
