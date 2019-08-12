#ifndef PS707_ASYNCI_H
#define PS707_ASYNCI_H
/*===========================================================================

                        P S 7 0 7 _ A S Y N C I

DESCRIPTION
  This file contains functionality for IS707 async/fax mode-specific
  handlers that mainly deals with call control. Internal header file.

EXTERNALIZED FUNCTIONS
  ps707_async_powerup_init()
    Called once at mobile power-up.

  ps707_async_call_init()
    Called as part of per-call init.

Copyright (c) 2002-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE
    $PVCSPath: L:/src/asw/MM_DATA/vcs/ps707_async.h_v   1.4   02 Dec 2002 12:59:50   ajithp  $
    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ps707_asynci.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/11/11    ack    Global Variable Cleanup
07/26/10    ps     Added code changes to fix the broken Async/Fax support.
09/04/09    ss     CMI SU level modifications.
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
03/16/04    atp    Lint fixes.
10/17/03    sv     Added PS707_ASYNC_DEF_MSS declaration.
12/02/02    atp    Added header, comments, etc.
08/22/02    atp    Cleanup for shipping.
08/19/02    atp    Support for Um flow control for low-baud SIO links.
08/13/02    mvl    Removed decalrations for PPP callbacks.
07/25/02    mvl    Updates for PPP renaming.
07/16/02    atp    Renamed to ps707_async_process_send_q_sig_hdlr().
07/15/02    atp    Added ps707_async_process_tcp_send_q().
07/13/02    atp    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707_ASYNC
#include "ds707_async_617.h"
#include "ps_ppp.h"
#include "ps_tcp_async.h"
#include "pstimer.h"
#include "ps707_async.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define PS707_ASYNC_DEF_MSS 536  /* Default maximum segment size           */

/*===========================================================================
                                  TYPEDEFS
===========================================================================*/

/*---------------------------------------------------------------------------
  Enum that represents the state of the async TCP connection from PS
  point of view.
---------------------------------------------------------------------------*/
typedef enum
{
  PS707_ASYNC_PROTOCOL_CLOSED,
  PS707_ASYNC_PROTOCOL_OPENING,
  PS707_ASYNC_PROTOCOL_OPEN,
  PS707_ASYNC_PROTOCOL_CLOSING
} ps707_async_protocol_state_enum_type;

/*---------------------------------------------------------------------------
  Structure that holds state info for async call w.r.t PS task.
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    TCP connection state.   
  -------------------------------------------------------------------------*/
  ps707_async_protocol_state_enum_type protocol_state; 
  /*-------------------------------------------------------------------------
    Indicates if call is mobile-originated.   
  -------------------------------------------------------------------------*/
  boolean mobile_originated;
  /*-------------------------------------------------------------------------
    Handle for ptcl cleanup timer.        
  -------------------------------------------------------------------------*/
  ps_timer_handle_type ptcl_cleanup_timeout_handle;
  /*-------------------------------------------------------------------------
   Queue for storing data to be sent over async TCP conn.
  -------------------------------------------------------------------------*/
  q_type  tcp_send_q; 
  /*-------------------------------------------------------------------------
    Count of bytes in the tcp send q.  
  -------------------------------------------------------------------------*/
  word    tcp_send_q_cnt;
  /*-------------------------------------------------------------------------
    Flag to indicate if the tcp send q is being flow controlled.
  -------------------------------------------------------------------------*/
  boolean tcp_send_q_flow_enable; 

} ps707_async_state_type;


/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION      PS707_ASYNC_POWERUP_INIT

DESCRIPTION   Called once at mobile power-up.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void ps707_async_powerup_init
(
  void
);

/*===========================================================================

FUNCTION      PS707_ASYNC_CALL_INIT

DESCRIPTION   This function does per call initialization:
              Initialize counters and indices for the EIA 617 and IAC escape
              sequence processing.  Initializes the FDT command seen flag
              for fax flow control.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void ps707_async_call_init
(
  void
);

/*===========================================================================
FUNCTION      PS707_ASYNC_TCP_CTL_OPEN_CMD_HDLR

DESCRIPTION   Called in PS TASK context in response to a  request by DS to
              open TCP (and the protocol stack) for async data transfer

DEPENDENCIES  Called in PS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_tcp_ctl_open_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_PTCL_ABORT_CMD_HDLR

DESCRIPTION   Called in PS TASK context in response to a  request by DS to
              abort protocols.

DEPENDENCIES  Called in PS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ptcl_abort_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_PTCL_CLOSE_CMD_HDLR

DESCRIPTION   Called in PS TASK context in response to a  request by DS to
              close protocols gracefully.

DEPENDENCIES  Called in PS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ptcl_close_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_PPP_DOWN_CMD_HDLR

DESCRIPTION   This function takes action when PPP goes down.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ppp_down_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_PPP_UP_CMD_HDLR

DESCRIPTION   This function will initiate the Opening of TCP for Async/Fax
              service.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ppp_up_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_PPP_RESYNC_CMD_HDLR

DESCRIPTION   This function takes action when there is a PPP resync.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ppp_resync_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_UM_FLOW_ENABLE_CMD_HDLR

DESCRIPTION   This function takes action when SIO is ready to accept data
              from Um. Applicable for low-baud SIO that's slower than Um.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_um_flow_enable_cmd_hdlr
(
  ps_cmd_enum_type cmd,       /* Actual command to be processed            */
  void *user_data_ptr         /* Command specific user parameters(unused)  */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_PROCESS_SEND_Q_SIG_HDLR

DESCRIPTION   This is the handler for PS_707_ASYNC_TCP_RX_Q_SIGNAL.

DEPENDENCIES  None.

RETURN VALUE  TRUE/FALSE depending on whether to relinquish control. We 
              always return TRUE.

SIDE EFFECTS  None.
===========================================================================*/
boolean ps707_async_process_send_q_sig_hdlr
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr  /* Parameter carrying user data (TCP ack piggyback) */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_PTCL_CLEANUP_TIMEOUT_CB

DESCRIPTION   Callback when timer expires for protocol cleanup.  By this
              time if TCP hasn't shut down gracefully, calls function to
              forcibly cleanup things.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_ptcl_cleanup_timeout_cb
(
  void *user_data_ptr                                          /* unused   */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_DATA_FROM_TCP_UPCALL

DESCRIPTION   This function will be invoked by the TCP input routine upon
              completion of TCP processing of received payload data bytes.
              This function will send the TCP payload for 617 and telnet
              processing and then output to the serial port.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_data_from_tcp_upcall
(
  dsm_item_type  *dsmItemPtr,   /* List head pointer to payload data        */
  uint16         cnt            /* The number of data payload data bytes    */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_TCP_STATE_CHANGE_UPCALL_FUNC

DESCRIPTION   This function will be invoked by the TCP input routine when
              the TCP state changes. This function will use the old and the
              new TCP states to determine the processing requirements.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_tcp_state_change_upcall_func
(
  async_tcp_state_async_type state
);

/*===========================================================================
FUNCTION      PS707_ASYNC_TCP_TX_WINDOW_CHANGE

DESCRIPTION   This function will be invoked by the TCP input routine when
              the TCP TX window changes to allow more bytes to be sent. This
              function will determine if the Lowater mark has been reached
              for the appropriate Watermark item and invoke the Lowater
              function if appropriate.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_tcp_tx_window_change( void );

/*===========================================================================
                             OTHER FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      PS707_ASYNC_CONFIG_PPP_UM_OPTIONS

DESCRIPTION   This function configures PPP options for Um link.

DEPENDENCIES  Assumes default options are already present in options
              parameter passed in.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_config_ppp_um_options
(
  ppp_dev_opts_type *ppp_opts /*Ptr to struct to be filled with ppp options*/
);

/*===========================================================================
FUNCTION      PS707_ASYNC_SETUP_TCP

DESCRIPTION   This function will prepare the TCP and PPP data structures for
              operation.  This includes setting proper TCP port numbers and
              initiating the TCP open.

DEPENDENCIES  This function must be called to initiate protocol operation
              for each data call.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_setup_tcp
(
  word tcp_open_mode           /* type for TCP Open, either Active or Open */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_TCP_DEQUEUE

DESCRIPTION   This function removes the first item off the
              tcp_send_q and passes it back to the calling
              function. It also adjusts the byte count of the queue.

DEPENDENCIES  None.

RETURN VALUE  Pointer to item just dequeued.

SIDE EFFECTS  Changes tcp_send_q and tcp_send_q_cnt.
===========================================================================*/
dsm_item_type * ps707_async_tcp_dequeue
(
void
);

/*===========================================================================
FUNCTION      PS707_ASYNC_PROCESS_DATA_FROM_UM

DESCRIPTION   This function reads the data stream from the protocol stack.

              It strips off the <IAC> compression negotiation codes and
              transparency sequences.  Responses to the compression
              negotiation are generated, and sent back to the base station
              as <IAC> sequences via dsm_enqueue.  The <IAC> sequence will
              be presented in its entirety to dsm_enqueue.  If enabled by
              the AT+CDR command, an unsolicited status message will be sent
              to the user.

              It then strips out all EIA-617 <EM> in-band signals, and <EM>
              transparency codes. The reflected AT codes transmitted as 
              EIA-617 in-band signals are use to call ds_atcop 
              (DS_ATCOP_ONLINE). Any spaces remain.

              EIA-617 modem pin status signals are transmitted to the SIO
              driver via the sio_xxx_procedure calls.

              Response messages are removed from the EIA-617 STATUS messages,
              and passed to the serial port.

              The remainder of the data stream is passed to the serial port
              for transmission to the TE2.

DEPENDENCIES  The "AT" characters and the end of line character are assumed
              not to be present in the reflected AT codes carried in in-band
              signals.

              The necessary transparency codes are assumed to have been
              imbedded at the base station.

              'cnt_617', 'index_617', 'cnt_iac', 'index_iac', and
              'sub_neg_iac' must be set to 0  before starting to process
              the input data stream'.This is done by ps707_async_call_init().

              The output items used by do_em and do_iac may be longer than
              the input item.  This is safe for the following reasons:

              The output items are modified by applying DSM_ITEM_HEAD which
              increases its size by the size of the TCP header and footer.

              The maximum size gain in do_iac is 14 bytes. The maximum
              size gain in do_em is 99 bytes.  (See the discussion of
              the individual functions.)

              NB: As long as DSM_ITEM_HEAD increases the available space
                  in the output item by more than the sum of the maximum
                  expansion in do_em and do_iac (113 bytes),  and
                  as long as the header area has _not_ been used in the
                  input item received from the protocol stack,
                  it is safe to allow the output items to expand.

              Both do_em and do_iac terminate when the output exceeds the
              basic size of a large item, even if there is still input to
              process.  They set em_flag or iac_flag so that processing of
              the input item will be resumed after the output item has been
              processed.

RETURN VALUE  None.

SIDE EFFECTS  No side effects except thru call to ds_atcop.
===========================================================================*/
void  ps707_async_process_data_from_um
(
  dsm_item_type *in_item_ptr  /* pointer to input item from protocol stack */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_DO_EM

DESCRIPTION   This function strips out all EIA-617 <EM> in-band signals, and 
              <EM> transparency codes. The reflected AT codes transmitted as
              EIA-617 in-band signals are used to call ds_atcop
              (DS_ATCOP_ONLINE). Any spaces remain. Response messages are
              removed from the EIA-617 STATUS messages, and output to a new 
              output data item.

              The remainder of the data stream is output to the new output
              data item.

DEPENDENCIES  'cnt_617' and 'index_617' must be set to 0 before starting to
              process the input data stream. This is done by
              ps707_async_call_init().

              The output item may be longer than the input item.
              This is safe for the following reasons:

              Both the input and output items are LARGE items.  With
              exceptions described below, the output item cannot be
              larger than the input item.

              Non EIA-617 data is copied verbatim to the output, so the
              same number of bytes appear at the output as at the input.

              Transparency codes output either 1 or 2 bytes for 2 bytes
              in, so they either shrink or break even.

              STATUS messages have a four byte header which is not output.
              They have two <CRLF> pairs added to the message text (one
              pair before, and one after), so they break even.

              No other EIA-617 messages produce output, so they shrink.

              The only thing that can cause the length of the output item
              to exceed the length of the input item is an EIA-617 message
              which stradles two input items.  In this case, the message
              is accumulated in 'buf_em' until it is complete.  If the
              message is transparency or STATUS, it is output to the output
              item.  The amount of length increase is roughly the length
              of the part of the message which was in the preceeding input
              item.  The upper limit, then is the length of a STATUS message
              text (95 bytes), plus the 4 bytes of <CRLF>.

              The output item is modified by applying DSM_ITEM_HEAD which
              increases its size by the size of the TCP header and footer.

              NB: As long as DSM_ITEM_HEAD increases the available space
                  in the output item by more than 99 bytes,  and
                  as long as the header area has _not_ been used in the
                  input item, it is safe to allow the output item to expand.

              Whenever the size of the output item exceeds the size of the
              unexpanded size of a LARGE item, do_em terminates, so that
              the item can be processed.

RETURN VALUE  Pointer to new output item.

SIDE EFFECTS  'em_flag is set whenever a 617 command is being processed,
              and cleared when the processing is complete.

              Whenever the size of the output item exceeds the size of the
              unexpanded size of a LARGE item, do_em terminates, so that
              the item can be processed. 'em_flag' is set so that do_em
              will be called again to finish processing its input item.
===========================================================================*/
dsm_item_type * ps707_async_do_em
(
  dsm_item_type *in_item_ptr               /*  Pointer to input data item  */
);

/*=========================================================================

FUNCTION      PS707_ASYNC_DO_IAC

DESCRIPTION   This function strips off the <IAC> compression negotiation 
              codes and transparency sequences.  Responses to the
              compression negotiation are generated and sent back to the
              base station as <IAC> sequences via dsm_enqueue.  The <IAC> 
              sequence will be presented in its entirety to dsm_enqueue.

              The remainder of the data stream and any unsolicited status
              messages is output to a new output data item.

DEPENDENCIES  'cnt_iac' and 'index_iac' must be set to 0 before starting
              to process the input data stream.  This is done by 
              ps707_async_call_init().

              The output item may be longer than the input item.
              This is safe for the following reasons:

              Both the input and output items are LARGE items.  With
              exceptions described below, the output item cannot be
              larger than the input item.

              Non IAC data is copied verbatim to the output, so the
              same number of bytes appear at the output as at the input.

              Transparency codes output 1 byte for 2 bytes in, so they
              shrink.

              If the +CDR switch is ON (1), a status message may be
              generated:
              This message is a maximum of 14 bytes long. The IAC
              message generating it is 3 bytes long, so the max gain
              is 11 byte.

              IAC messages stradling two input items can add at most
              2 bytes to the increase caused by the +CDR status message.

              No other IAC messages produce output, so they shrink.

              The output item is modified by applying DSM_ITEM_HEAD which
              increases its size by the size of the TCP header and footer.

              NB: As long as DSM_ITEM_HEAD increases the available space
                  in the output item by more than 14 bytes,  and
                  as long as the header area has _not_ been used in the
                  input item, it is safe for the output item to expand.

RETURN VALUE  Pointer to output data item.

SIDE EFFECTS  'iac_flag is set whenever an IAC command is being processed,
              and cleared when the processing is complete.

              Whenever the size of the output item exceeds the size of the
              unexpanded size of a LARGE item, do_iac terminates, so that
              the item can be processed. 'iac_flag' is set so that do_iac
              will be called again to finish processing its input item.

===========================================================================*/
dsm_item_type * ps707_async_do_iac
(
  dsm_item_type *in_item_ptr               /*  Pointer to input data item  */
);

/*===========================================================================
FUNCTION      PS707_ASYNC_CLEANUP_PTCL

DESCRIPTION   This function performs protocol cleanup operations after
              the end of an IS-707 async data call. It gets called either
              upon expiration of a timer when TCP closes gracefully or
              it's explicitly called when DS asks PS to abort protocols
              forcibly.

DEPENDENCIES  Must be called after the termination of protocol operations
              for an IS-707 async data call.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ps707_async_cleanup_ptcl
(
  void
);

/*===========================================================================
FUNCTION      PS707_GET_ASYNC_STATE_PTR

DESCRIPTION   Accessor method for ps707_async_state structure

DEPENDENCIES  None.

RETURN VALUE  Returns pointer to ps707_async_state structure.

SIDE EFFECTS  None.
===========================================================================*/
ps707_async_state_type* ps707_get_async_state_ptr
(
  void
);

#endif /* FEATURE_DATA_IS707_ASYNC */
#endif /* PS707_ASYNCI_H   */

