/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R   M E S S A G I N G   P R O T O C O L
         ( S I G N A L I N G   N E T W O R K   P R O T O C O L )

GENERAL DESCRIPTION

HMP
The HDR messaging protocol performs a number of different functions.  For
received messages it routes them to the proper protocol.  For transmitted
messages it responds to the sender with a status and gives the message to
SLP when appropriate.  This protocol also performs some high level control
functions, like opening a session.

EXTERNALIZED FUNCTIONS (Global)
  hdrhmp_route_msg - Gives a received message to the protocol it is for
  hdrhmp_powerup_init - powerup initialization
  hdrhmp_enter_init - initialization required every time HDR is started
  hdrhmp_send_msg - For any protocol to send a message on Stream 0
  hdrhmp_app_send_msg - For any protocol to send a message on any stream
  hdrhmp_send_msg_on_existing_rtc - For any protocol to send a RTC only message
                                    that will be discarded if conn is closed
                                    before the message can be sent (stream 0)
  hdrhmp_app_send_msg_on_existing_rtc - For any protocol to send a RTC only
                               message that will be discarded if conn is closed
                               before the message can be sent (any stream)
  hdrhmp_ind_cb - Callback for HMP receiving an indication
  hdrhmp_add_header - For RLP to add the HMP header byte to a message
  hdrhmp_tx_queue_is_empty - Check if HMP is waiting to send a message
  hdrhmp_abort_msg - Stop retransmission of a reliable message

EXTERNALIZED FUNCTIONS (Regional)
  hdrhmp_process_cmd - process command
  hdrhmp_process_ind - process indication
  hdrhmp_process_sig - process signal

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrhmp.c_v   1.81   24 Apr 2003 11:42:00   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrhmp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/31/16   vko     Reduce excessive F3
05/09/16   vko     Reduce excessive F3 logging
04/05/16   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
12/14/15   vko     Fixed KW issues 
07/10/15   vko     Fixed DO not sleeping due to HMQ queue stuck
12/18/14   sjj     Undefining the timer memory before freeing the modem memory.
10/01/14   vko     Free the DSM item if the msg to be sent is being canceled.
07/09/14   cnx     Support NV refresh without reset.
04/01/14   vko     Fixed race between amac start attempt and bundling due to ROR
10/03/14   vko     Increment failure count,when session negotiation failed due to cfg rsp timer expiry
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
05/01/13   cnx     Close session when AT enters tunnel with NAI change.
02/11/13   vko     Fixed race condition for hdrhmp.rtc_msg_list
02/07/13   cnx     Allow UATIrequest during IDLE handoff.
01/25/13   ukl     Fix error in hdrhmp_msg_is_sendable where the chekcs were
                   skipped in non tunnel mode. 
01/10/13   ukl     Fix the race condition where a connection close happens 
                   before sending a pending RUP over RTC
08/14/12   sju     Fix to Re-try open conn cmd after resume if hdrhmp_pending_queue
                   is having pending RTC MSG from HDRHAI_IDLE_STATE_PROTOCOL
10/18/12   wsh     Fixed assert due to xmit RTC msg on AC channel
10/07/12   cnx     Clean up RASM before idle handoff to native HDR network.
10/04/12   cnx     Don't check RASM when pre-reg is updated.
09/20/12   cnx     Save session to EFS when powerdown in tunnel.
09/11/12   cnx     Handle TX and RX messages during active handoff
06/27/12   cnx     Treat UATIComplete and SessionClose as AC msg when virtual 
                   connection setup is in progress.
06/21/12   cnx     Ignore non-pri CC check in tunnel mode.
04/25/12   cnx     Added HDRIND_SAP_RETURNING_TO_CLOSE.
04/24/12   cnx     Fixed compiler warnings.
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
03/14/12   cnx     Don't query SD when reaching max session attempt if HDR is offline.
02/22/12   cnx     Track RTC msg list counter.
01/05/12   sju     Attempt to abort connection only in RASM failure case.
12/05/11   cnx     Support hdrcp_config_info.conf.
11/28/11   cnx     Support NV for session backoff timer.
11/08/11   cnx     Support Optimized Handoff phase 3.
10/19/11   cnx     Move RASM to correct state when sending AC msg in tunnel mode.
10/12/11   cnx     Close non-irat session when entering tunnel mode.
10/03/11   cnx     Cleared flag for UATIAssignment after UATIComplete is queued.
09/24/11   cnx     Check preregistration before sending out msg in tunnel mode.
07/27/11   cnx     Merged Optimized Handoff changes.
06/03/11   cnx     Renamed HDRHAI_CLOSE_EHRPD_CREDENTIALS_CHANGED.
05/18/11   wsh     Added Netowkr Load Balancing support
05/03/11   cnx     Fixed compiler warnings.
05/02/11   cnx     Fixed compiler warnings.
04/27/11   cnx     Supported hdrmc callbacks.
10/24/10   pxu     Fixed compiler warnings. 
09/07/10   pxu     Memory heap changes. 
06/21/10   cnx     Fixed compiler warnings.
06/10/10   pba     IRAT Measurement feature.
05/19/10   pba     Send session close when HMP pending message buffer overflows.
03/30/10   wsh     ASSERT after multiple sys loss report during RASM
12/01/09   suren   Added QChat optimization changes.
10/30/09   pba     Added ehrpd support.
09/18/09   wsh     CMI: support changes from modem DATA
08/28/09   wsh     CMI: Removing conditional defines from API headers
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/24/09   wsh     CMI: updated included headers due to header split
12/10/08   smd     Replaced assert.h with amssassert.h
12/05/08   kss     Removed/replaced intlocks.
12/04/08   pba     Lint and Compiler warning fixes
11/21/08   pba     Added events for SLPQH feature.
11/21/08   wsh     Fixed compiler warning
09/08/08   etv     Registers callbacks for PHY protocol.
05/23/08   wsh     Fixed AT not able to open session if previous session 
                   negotiation is interrupted (DCM)
04/28/08   wsh     Added interface to send bundled AC messages
04/17/08   sju     Added support to bundle AC messages 
04/07/08   wsh     SessionClose should not be blocked during ConnSetup
04/03/08   wsh     Merged fix for waiting UATIAssign onto 2H07
08/17/07   yll     Allow the UATI Complete message to be sent on AC while AT
                   is in Connection Setup state.
09/28/07   sju     In msg queue full case, do not force system lost if AT is 
                   not acquired.
09/11/07   wsh     Removed lint high warnings
08/21/07   wsh     Made changes to support Silent Redial for sending CR+RoR 
08/17/07   yll     Allow the UATI Complete message to be sent on AC while AT
                   is in Connection Setup state.
05/29/07   mpa     Added new HDRHAI_CLOSE_NEW_NAI session close reason.
05/15/07   hal     Updated hdrhmp_check_rx_msg_bundle() to handle multiple TCAs
04/13/07   pba     Added check for session change reason 
                   HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS.
02/12/07   mpa     Fixed RVCT compiler warning.
01/11/07   sju     Added support to check if CC and TCA are in the same 
                   connection layer packet.
12/06/06   sju     Force system lost to gracefully recover run out of slp tx buffer case.
12/05/06   sju     Fixed handling of slp send msg failure.
11/27/06   pba     Do not allow RTC only message when IDLE state protocol is in 
                   suspended state.
11/15/06   sju     Added support to abort pending connection if rasm msg is no longer
                   needed and fixed handling of rasm cb and msg in DONE state.
11/15/06   sju     Added support to bundle ROR with CR and DOS messages.
10/24/06   sju     Fixed printing F3 by using NULL ptr in RASM out_of_buffers case. 
10/23/06   mpa     Fixed F3 message.
10/16/06   etv     Added support for LOG_HDR_SUBTYPE_N_STREAM_CONFIG_C log pkt.
10/05/06   wsh     Added check to make ConnectionReq sendable on AC during 
                   TC setup
09/26/06   sju     Added hdrhmp_clear_rasm_queue() to remove RASM messages from RASM 
                   list. also fixed RASM retransmission probem if hdr srch state moves
                   from 4 -> 0.
09/14/06   sju     Added support to send ReservationOnRequest by using RASM 
                   when ReservationKKIdleState == 2. 
09/05/06   sju     Fixed the default value of overall_timeout in 
                   hdrhmp_app_send_rasm_qos_request() from 0 to 40000
08/25/06   sju     Added new RASM substate to represent waiting for retransmit state and 
                   add new parameter(callback_info_ptr) to hdrhmp_queue_pending_msg 
08/10/06   sju     Need to call dsm_dup_packet during RASM retransmit becasue lower 
                   layer will free the dsm item later. 
08/09/06   sju     While adding to RASM list we should first memcpy the message
                   send command to the RASM buffer and then assign the self_ptr.
07/18/06   etv     Added hdrscmovhd module to support OMP config attribs.
07/18/06   dna     Major RASM changes to allow reliable QoS call setup.
07/05/06   mpa     Use SCM_SESSION_CHANGED ind payload to determine session status
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
05/10/06   yll/dna Don't allow new AC messages to be sent while waiting for TCA
                   Reliable Access Signaling Messages (RASM)
05/09/06   yll     Added support for Minimal UATI Mode.
04/07/06   hal     Changed dsm_pushdown() to use hdrutil_pushdown_head()
03/27/06   hal     Doubled the size of HDRHMP_MSG_BUFS_NUM from 6 to 12
03/22/06   pba     Added support for ConfigurationLock.
03/22/06   pba     Added two new session close reasons.
03/22/06   yll     Fixed a problem that AT doesn't perform the DO session
                   avoidance when subnet changes.
02/27/06   pba     Ignore ConfigRequest messages for InConfig instance of SCP.
02/23/06   hal     Route AMP InConfig messages to SCM callback
02/03/06   pba     Fixed routing of RUP GAUP messages.
01/16/06   yll     When AN sends the connection deny message due to Auth or
                   Billing failure, avoid the system immediately for 10 min. 
12/06/05   etv     Route AMAC,CMAC & GMMCDP config messages to SCM callback.
11/28/05   hal     Route ALMP InConfig messages to SCM callback
11/14/05   yll     When session fails due to no TCA, no TCAck, or AN denying
                   the connection, avoid the system using telecoping 
                   mechanism in SD.
11/02/05   etv     Route FMAC's InUse Config messages to SCM callback.
10/22/05   dna     Don't leak DSM items for invalid messages.
                   Don't re-open a session unless it is completely closed.
10/21/05   pba     Routed BCCP In Config messages to SCM
10/18/05   pba     Give HDRIND_SMP_SESSION_CLOSED indication itself to HMP 
                   instead of HDRIND_SCM_SESSION_CHANGED on session close
10/14/05   pba     Fixed processing of session changed indication upon 
                   session close.
10/10/05   etv     Routed RMAC's Inuse Config messages to SCM.
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/07/05   etv     Routed RMAC's Config messages to SCM.
09/05/05   mpa     Added HDRTRACE_HMP_SESSION_CLOSED event.
08/22/05   etv     Routed FMAC's Inconfig messages to SCM instead of FMAC.
08/09/05   mpa     Fixed routing of of Stream 1, 2, 3 message for InUse 
                   Config msgs
07/19/05   dna     Fixed GAUPing of SlottedMode attribute
07/04/05   dna     Protect hdrhmp.msgs_on_event_q for multi-threaded access.
05/24/05   pba     Look at RUP state instead of RMAC to find if RTC is assigned
05/24/05   mpa     Fixed routing of hdrkep config messages.
04/21/05   pba     Grouped messages with msg_id >= 0x50 and msg_id <= 0x7F as
                   config and others ( 0x00-0x4F and 0x80-0xFF ) as non-config
04/21/05   pba     Fixed incorrect routing of Stream 0 signalling messages to
                   PAC in recent routing table change.
03/24/05   pba     Lint cleanup
03/07/05   pba     Modified HMP routing to route messages based on 
                   protocol_type, msg_id and protocol_instance.
03/07/05   pba     Separated handling of In-Use and In-Config messages for PAC
01/24/05   dna     Avoid if AN acks UATIRequest but doesn't send UATIAssignment
01/17/05   mpa     Allow RTC messages to be sent in smp closing state.
01/10/05   dna     Allow ses_abort to return continue if it occurs
                   while in PWR_SAVE (after max access probes).
11/29/04   dna     Avoid potentially leaving HDRSRCH in idle mode for access.
11/09/04   dna     Allow for ConnectionRequest to be aborted.
10/19/04   mpa     Replaced hdrscp_session_is_open with hdrsmp_session_is_open.
09/09/04   mpa     Replaced streamX callbacks with hdrpacmc central cb.
08/26/04   mpa     Added new HDRHAI_CLOSE_ESN session close reason.
06/30/04   dna     Delayed giving mmoc_rpt_prot_auto_deactd_ind() until           
                   HDRIND_LMAC_DEACTIVATE_DONE is received.  This changed     
                   hdralmp_deactivate(), hdralmp_cmd_params_union_type, and 
                   delayed when and where we give HDRIND_ALMP_EXIT_HDR_MODE.  
07/02/04   mpa     Added final HDR BCMCS message routing support.
                   Added Virtual Stream Protocol support.
06/02/04   kss     Initial support for HDR BCMCS (HDR Gold Multicast)
05/21/04   dna     Abort HMPSA on HDRIND_RUP_CONNECTION_INITIATED instead of 
                   HDRIND_RTCMAC_LINK_ACQUIRED, to avoid getting stuck if we
                   call hdrlmac_access_mode during connection setup but the 
                   connection setup fails.
03/16/04   dna     Re-start HMPSA state machine if it is in the middle of
                   starting another access attempt when one ends.
02/25/04   mpa     Added new case to session close processing
09/10/03   mpa     Converted F3 messages to use new HDR PROT MSG2.0 SSID
06/26/03   mpa     session close due to do connection deny counted towards
                   avoiding the system
06/19/03   mpa     Update range checking in hdrhmp_generic_send_message
04/23/03   mpa     Modified hmp_msg_is_sendable() F3 messages.
04/02/03   mpa     Added hdrhmp_send_msg_on_existing_rtc()
                   Added hdrhmp_app_send_msg_on_existing_rtc()
                   Don't open a new connection for RTC only msgs sent using
                   above functions
                   Added hdrslp_msg_cb to InUse routing table
03/18/03   mpa     Don't avoid system if can't retrieve subnet id
03/12/03   mpa     Use new session close_reason from SMP_CLOSED indication
                   to decide whether to add to session close count
02/04/03   mpa     Removed sd_action ASSERTs
01/09/03   mpa     Replace slp cb with rtc_msg_cb for msgs sent on AC_OR_RTC.
                   Msgs cleared from the pending queue for connection setup
                   failure reasons should be requeued if meant for AC or RTC.
12/10/02   dna     Avoid calling hdrsmp_activate() twice if closed while on TC.
10/30/02   mpa(dna) Don't open a connection to send BE msg on the RTC
09/23/02   dna     Re-set session close count after avoiding the system.
08/01/02   dna     Add F3 msg for receive message protocol ID/message ID
07/10/02   dna     Remove assert on LINK_ACQUIRED when ALMP isn't in connected
                   state. This can happen under some valid conditions.
06/20/02   mpa     Display pid/mid in hex in all msgs.
06/19/02   dna     Don't put UATIRequest on ac_msg_list to avoid queueing
                   multiple identical UATIRequests following AHO.
05/30/02   dna     Ensure msgs_on_event_q does not get out of sync.
05/13/02   nmw     Merged from branch - Changed outcome callback err_val to
                   E_AGAIN for ALMP.ReturningToIdle and IDLE.ConnectionFailed
04/30/02   vas     Added check in msg_is_sendable to see if AMP is inactive.
04/26/02   dna     Check for invalid msg with only a HMP header and no body.
                   Use correct protocol ID when re-xmit application layer msgs.
04/16/02   dna     ALMP can reacquire while a message is still pending
                   Add more RTC_MSG_BUFS due to "many XOFF" case seen in lab
04/12/02   dna     Change session close max to 3 just because there is less
                   of a chance that it will falsely avoid a system that way.
04/11/02   dna     If we run out of buffers on ac_msg_list or rtc_msg_list,
                   Gracefully recover by forcing system lost.
01/21/02   sh      Added abort reason for ALMP deactivate and reacquire
11/30/01   om      Added security layer support
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
8/17/01    vas     Added function hdrhmp_app_send_msg() so that applications
                   can send msgs for Streama 1, 2, & 3. hmp_send_msg() will
                   send msgs on Stream 0.
08/14/01   dna     Increase pending message queue and AC message list sizes
                   Fix an assert and add a debug message.
07/10/01   dna     Added call to hdrhmpsa_abort() if session is closed.
                   Added hdrhmp_abort_msg(), to wrap hdrslp_abort_msg().
                   Don't allow RTC message when we don't have a UATI.
06/26/01   dna     Added call to hdrhmpsa_abort() if RTC link is acquired
06/21/01   vas     Removed hmp delay timer so that pending messages are sent
                   immediately on receiving a RTC mac link acquired indication
06/13/01   vas     Change in hmp_route_msg to try and set the protocol id field
                   in dsm header only if a valid protocol id is extracted..
06/06/01   dna     new param for hmp_send_msg(), initial handling for in-use/
                   in-config bit in protocol type field
05/22/01   dna     Handle messages sendable on either AC or RTC.
05/03/01   dna     Re-naming for consistency.
03/29/01   dna     HMP now controls if a session should be re-opened
                   Also added RLP as callback for streams 2 and 3
02/09/01   dna     Added hdrhmp_tx_queue_is_empty() for idle state to tell if
                   HMP had something to send.
02/07/01   dna     Rename hdrhmp_debug_timer to hdrhmp_delay_after_acq_timer
                   because it is not just for debug any more.
                   Fixed DSM item leaks from hdrhmp_ac_msg_list items,
                   hdrhmp_pending_msg_q items, and send_msg commands that
                   weren't making it to SLP.
02/05/01   nmn     Implemented X6 msg cb lookup table
01/26/01   dna     Don't pushdown HMP header onto idle RUP registration
09/07/00   dna     Split ACMAC activation into its own module, changed send
                   message command processing, made other design changes
03/02/00   dna     Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrdebug.h"
#include "err.h"
#include "hdrhai.h"
#include "dsm.h"
#include "memory.h"
#include "task.h"
#include "sd.h"
#include "modem_mem.h"
#ifdef FEATURE_LTE_TO_HDR_OH
#include "fs_lib.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_CMI
#include "sd_v.h"
#endif


#include "hdrmci.h"
#include "hdrdsm.h"
#include "hdrind.h"
#include "hdralmp.h"
#include "hdralmps.h"
#include "hdramac.h"
#include "hdrcmac.h"
#include "hdrfmac.h"
#include "hdrrmac.h"
#include "hdrovhd.h"
#include "hdrhmpi.h"
#include "hdrhmpsa.h"
#include "hdrrlp_api.h"
#include "hdrslp.h"
#include "hdrsmp.h"
#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#include "hdrsaps.h"
#include "hdrsaptx.h"
#include "hdrsmps.h"
#include "hdrstream.h"
#endif /* FEATURE_LTE_TO_HDR_OH */
#include "hdramp.h"
#include "hdramps.h"
#include "hdrscp.h"
#include "hdrstream.h"
#include "hdrcon.h"
#include "hdrinit.h"
#include "hdridlei.h"
#include "hdridles.h" 
#include "hdrkep.h"
#include "hdrauth.h"
#include "hdraddr.h"
#include "hdraddr_v.h"
#include "hdrpac_api.h"
#include "hdrpacc.h"
#include "hdrpcp.h"
#include "hdrutil.h"
#include "hdrrup.h"
#include "hdrscm.h"
#include "hdrscmdb.h"
#include "hdrampi.h"

#include "amssassert.h"
#include "hdrtrace.h"
#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif

#ifdef FEATURE_HDR_QCHAT
#include "hdrqchatupk.h"
#include "hdrqchatupk_opts.h"
#endif /*FEATURE_HDR_QCHAT*/

#ifdef FEATURE_HDR_BCMCS
#include "hdrbccp.h"
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
extern void hdramp_done_waiting_for_uati_assignment
( 
  hdramp_clear_slp_holdoff_reason_type reason
);
#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRHMP_TASK_PTR hdrmc_tcb_ptr

/* General signature for message callback functions registered with HMP */
typedef void ( hdrhmp_msg_cb_f_type ) ( dsm_item_type * msg_ptr );

typedef enum
{
  HDRHMP_RASMS_FREE    = 0x00,  /* Just for logging */
  HDRHMP_RASMS_WAITING = 0x10,  /* Waiting to be queued to SLP */
  HDRHMP_RASMS_QUEUED  = 0x20,  /* queued to SLP */
  HDRHMP_RASMS_PENDING = 0x30,  /* In the pending_msg_q */
  HDRHMP_RASMS_DONE    = 0x40   /* timed out or acked while queued */
} hdrhmp_rasm_state_enum_type;

typedef enum
{
  HDRHMP_RASMSS_NONE                    = 0x00,
  HDRHMP_RASMSS_WAITING_L3_ACK          = 0x0a,
  HDRHMP_RASMSS_WAITING_CONFIG_UNLOCK   = 0x0b,
  HDRHMP_RASMSS_WAITING_HMP_REXMIT      = 0x0c
} hdrhmp_rasm_substate_enum_type;

typedef enum
{
  HDRHMP_RASM_REMOVE_L3ACKED, 
  HDRHMP_RASM_REMOVE_TIMEDOUT, 
  HDRHMP_RASM_REMOVE_CLEARED  
} hdrhmp_rasm_remove_reason_enum_type;

typedef struct
{
  hdrbuf_hdr_type hdr;         /* header */
  hdrhmp_send_msg_params_type msg_cmd;  /* The pending send message command */
} hdrhmp_msg_buf_type;

typedef struct
{
  hdrhmp_msg_buf_type buf;
  rex_timer_type overall_timer;
  rex_timer_type interval_timer;
  hdrhmp_rasm_state_enum_type state;
  hdrhmp_rasm_substate_enum_type substate;
  uint32 attempts;
  hdrhai_channel_enum_type tx_chan; 
  int log_id; /* RASM list entry number, for logging */
} hdrhmp_rasm_buf_type;

/* Dump the contents of the RASM buffer on each event, for debugging */

#ifdef FEATURE_RASM_LOGGING
#define HDRHMP_RASM_TRACE( state_code ) \
  HDR_MSG_PROT_8 (MSG_LEGACY_FATAL, "RASM #state_code: Trace S%x A[%d of %d] T[I%d/%d O%d/%d]ms TID0x%x", \
                  rasm_ptr->state | rasm_ptr->substate, rasm_ptr->attempts + 1, \
                  buf_ptr->msg_cmd.rasm.max_retries + 1,                        \
                  buf_ptr->msg_cmd.rasm.retry_interval -                        \
                  rex_get_timer( &rasm_ptr->interval_timer ),                   \
                  buf_ptr->msg_cmd.rasm.retry_interval,                         \
                  buf_ptr->msg_cmd.rasm.overall_timeout -                       \
                  rex_get_timer( &rasm_ptr->overall_timer ),                    \
                  buf_ptr->msg_cmd.rasm.overall_timeout,                        \
                  buf_ptr->msg_cmd.rasm.transaction_id)
#else
#define HDRHMP_RASM_TRACE( state_code )
#endif

typedef struct
{
  boolean is_rexmit; /* outcome_tag_ptr points to rasm_list entry */
  uint32  max_retries; /* How many times to retry (if best effort RTC) */
  uint32  retry_interval; /* How long to wait between retries */
  uint32  overall_timeout; /* Time to give up on transmitting */
  uint32  transaction_id; /* For app to inform HMP that layer 3 ACK was rxed */
  boolean is_implicit_on_connection_setup; /* TRUE if ReservationKKIdleState == 2 */
  void *self_ptr; /* Pointer to rasm_list entry holding this command */
} hdrhmp_rasm_info_type;


/* Maximum number of messages queued to be sent on RTC when connection opens.
 * Size is based on how many config requests we queue for initial session
 * config, and a random UATI assignment received when probing our UATI request
 * may cause us to queue two copies of each config msg.
 */
#define HDRHMP_MSG_BUFS_NUM 18

/* Maximum number of undelivered AC messages outstanding at one time.
 * Size is big because of a situation where the AN sends many data ready
 * messages on the control channel when the AT is idle, and we ack each one.
 */
#define HDRHMP_AC_MSG_BUFS_NUM 10

/* Maximum number of undelivered RTC messages that are allowed to also be sent
 * on the AC that can be outstanding at one time.
 */
#define HDRHMP_RTC_MSG_BUFS_NUM 10

/* Maximum number of undelivered application layer messages that are allowed */
#define HDRHMP_RASM_BUFS_NUM 10

/* Timeout for waiting for DOS if bundle flag is set to BUNDLE_WITH_CR_DOS for ROR msg */
#define HDRHMP_BUNDLE_WAITING_TIME_MSEC 10

/* Give up on the system if our session is closed twice in a row */
#define HDRHMP_SCLOSE_MAX 3

/* Used, for example, if the message is not a RASM message */
#define HDRHMP_RASM_TRANSID_NONE 0xFFFFFFFF

typedef enum
{
  HDRHMP_RASM_OVERALL_TIMER,   /* Individual timeout for each message */
  HDRHMP_RASM_INTERVAL_TIMER,  /* Individual timeout to retransmit each message */
  HDRHMP_BUNDLE_WAITING_TIMER, /* Timeout for waiting bundlign msg */
  HDRHMP_BACKOFF_TIMER         /* Timeout for re-open session */

} hdrhmp_timer_name_enum_type;

/* Different temporary message lists in HMP */
typedef enum
{
  HDRHMP_AC_MSG_LIST,   
     /* AC list buffers, AC channel only messages */
  HDRHMP_RTC_MSG_LIST,  
     /* RTC list buffers, messages than can be transmitted either on AC or RTC */
  HDRHMP_RASM_LIST   
     /* APP list buffers, all application layer messages */
} hdrhmp_retry_list_enum_type;

typedef struct
{
  /* Pointer to function called to announce outcome of transmission */
  hdrslp_outcome_cb_type outcome_cb;

  /* A pointer owned by sender, returned with the callback */
  void *outcome_tag_ptr; 
} hdrhmp_callback_info_type;

static void hdrhmp_free_msg
(
  dsm_item_type *msg_ptr,
  const hdrhmp_callback_info_type *callback_info_ptr,
  errno_enum_type failure_reason
);

static void hdrhmp_rasm_retransmit
(
  hdrhmp_msg_buf_type *buf_ptr
);

#ifdef FEATURE_LTE_TO_HDR_OH
#define HDRHMP_BACKOFF_TIMER_CNT_MAX 10

/* Second to ms unit conversion */
#define HDRHMP_SECOND_TO_MS_CONVERSION 1000

/* Session negotiation failure backoff timer (in sec) */
#define HDRHMP_BACKOFF_TIME_SESSION_FAILED 300

/* Session negotiation timeout backoff timer (in sec) */
static uint16 backoff_time_table_sc[HDRHMP_BACKOFF_TIMER_CNT_MAX] = 
{ 30, 60, 120, 240, 240, 240, 480, 480, 480, 480 };

typedef struct
{
  uint8 cnt; /* Number of consecutive session failures */ 
  uint8 colorcode; /* ColorCode last sessoin was closed on  */ 
  rex_timer_type backoff_timer;

} hdrhmp_sc_backoff_type;
#endif /* FEATURE_LTE_TO_HDR_OH */

/* All static data declared in this file must be in the hdrhmp struct.
 * This enforces naming consistency and allows for the data of this
 * file to be easily viewed in the debugger
 */

/* Structure mapping protocol_type, protocol_instance and protocol_msg_grp
 * to message processing function 
 *
 * HMP routing table is a 3 dimensional array with array indices as
 * protocol_type - specifies the type of HDR protocol
 * protocol_instance -  specifies whether it's an in-use or in-config instance
 * protocol_msg_grp - specifies if it is an configuration or non-configuration
 *                    message.
 *
 * Each protocol could have up to 4 registered callback functions for message
 * processing, as shown below
 *
 *                                     HDRHAI_NON_CONFIG,   HDRHAI_CONFIG
 * [PROTOCOL_TYPE]  HDRHAI_IN_USE   [         func1_cb(),   func2_cb() ]
 *                  HDRHAI_IN_CONFIG[         func3_cb(),   func4_cb() ] 
 *
 * func1_cb() - This callback function is called whenever the protocol receives
 *              a message with msg_id < 0x50 in its in-use instance.
 *
 * func2_cb() - This callback function is called whenever the protocol receives 
 *              a message with msg_id >= 0x50 in its in-use instance.
 *
 * func3_cb() - This callback function is called whenever the protocol receives
 *              a message with msg_id < 0x50 in its in-config instance.
 *
 * func4_cb() - This callback function is called whenever the protocol receives
 *              a message with msg_id >= 0x50 in its in-config instance.
 *              A protocol does not need to register a callback here
 *              if its config instance only handles the config request
 *              and it does not have any config attributes specified.
 *              In that case HMP will automatically send an empty 
 *              config response on its behalf.
 *
 * NULL values for func1_cb(), func2_cb(), or func3_cb() of a protocol
 * means it does not have any messages defined for that combination of 
 * protocol_instance and msg_grp.
 * 
 * NULL value for func4_cb() means the protocol does not have any configurable 
 * attributes. 
 *
 * Example: App1 protocol receives the AttributeUpdateRequest message, App1
 *          has following callback functions registered.
 * [HDRHAI_N_APP1_TYPE = 0x15]  [hdrpac_msg_cb,  hdrpacc_config_msg_cb]
 *                              [NULL,           hdrpacc_config_msg_cb]
 *
 * AttributeUpdateRequest is a config msg, in in-use instance of
 * HDRHAI_N_APP1_TYPE protocol. Callback function to process this 
 * message is invoked as
 * 
 *  (*hdrhmp_routing_table[HDRHAI_N_APP1_TYPE][HDRHAI_IN_USE][HDRHAI_CONFIG])
 *  (/parameters to callback function/)
 *
   */


static hdrhmp_msg_cb_f_type * 
const hdrhmp_routing_table [][HDRHAI_PROTOCOL_INSTANCE_ENUM_MAX]
                             [HDRHAI_MSG_GROUP_ENUM_MAX] =
{
  {{NULL, hdrscm_in_use_config_msg_cb},      /* HDRHAI_N_PHY_TYPE =  0x00 */ 
   {NULL, hdrscm_config_msg_cb}}, 

  {{NULL, NULL},                             /* HDRHAI_N_CCMP_TYPE = 0x01 */
   {NULL, hdrscm_config_msg_cb}}, 

  {{hdramac_msg_cb, hdrscm_in_use_config_msg_cb},       
                                             /* HDRHAI_N_ACMP_TYPE = 0x02 */
   {NULL, hdrscm_config_msg_cb}},

  {{hdrfmac_msg_cb, hdrscm_in_use_config_msg_cb},                   
                                             /* HDRHAI_N_FTMP_TYPE = 0x03 */
   {NULL, hdrscm_config_msg_cb}},

  {{hdrrmac_msg_cb, hdrscm_in_use_config_msg_cb},   
                                             /* HDRHAI_N_RTMP_TYPE = 0x04 */
   {NULL, hdrscm_config_msg_cb}},

  {{NULL, NULL},                             /* HDRHAI_N_KEP_TYPE  = 0x05 */
   {hdrkep_config_msg_cb, hdrkep_config_msg_cb}},

  {{NULL, NULL},                             /* HDRHAI_N_AP_TYPE   = 0x06 */
   {NULL, hdrauth_config_msg_cb}},

  {{NULL, NULL},                             /* HDRHAI_N_EP_TYPE   = 0x07 */
   {NULL, NULL}},

  {{NULL, NULL},                             /* HDRHAI_N_SP_TYPE   = 0x08 */
   {NULL, NULL}},

  {{NULL, NULL},                             /* HDRHAI_N_PCP_TYPE  = 0x09 */
   {NULL, NULL}},

  {{hdralmp_msg_cb, NULL},                   /* HDRHAI_N_ALMP_TYPE = 0x0A */
   {NULL,           hdrscm_config_msg_cb}},

  {{hdrinit_msg_cb, NULL},                   /* HDRHAI_N_ISP_TYPE  = 0x0B */
   {NULL,           NULL}},

  {{hdridle_msg_cb, hdridle_msg_cb},         /* HDRHAI_N_IDP_TYPE  = 0x0C */
   {NULL,           hdridle_config_msg_cb}},

  {{hdrcon_msg_cb,  NULL},                   /* HDRHAI_N_CSP_TYPE  = 0x0D */
   {NULL,           NULL}},

  {{hdrrup_msg_cb,  hdrrup_msg_cb},          /* HDRHAI_N_RUP_TYPE  = 0x0E */
   {NULL,           hdrrup_config_msg_cb}},

  {{hdrovhd_msg_cb, NULL},                   /* HDRHAI_N_OMP_TYPE  = 0x0F */
   {NULL,           hdrscm_config_msg_cb}},

  {{hdrsmp_msg_cb,  NULL},                   /* HDRHAI_N_SMP_TYPE  = 0x10 */
   {NULL,           hdrsmp_config_msg_cb}},

  {{hdramp_msg_cb,  hdrscm_in_use_config_msg_cb},  
                                             /* HDRHAI_N_AMP_TYPE  = 0x11 */
   {NULL,           hdrscm_config_msg_cb}},

  {{hdrscp_msg_cb,  hdrscp_msg_cb},          /* HDRHAI_N_SCP_TYPE  = 0x12 */
   {NULL,           NULL}},

  {{NULL, NULL},                             /* HDRHAI_N_STR_TYPE  = 0x13 */
   {NULL, hdrstream_config_msg_cb}},

  {{hdrslp_msg_cb,  NULL},                   /* HDRHAI_N_APP0_TYPE = 0x14 */
   {NULL,           NULL}}, 

  {{hdrpac_msg_cb,  hdrpacc_msg_cb},         /* HDRHAI_N_APP1_TYPE = 0x15 */
   {NULL,           hdrpacc_config_msg_cb}}, 

  {{hdrpac_msg_cb,  hdrpacc_msg_cb},         /* HDRHAI_N_APP2_TYPE = 0x16 */
   {NULL,           hdrpacc_config_msg_cb}}, 

  {{hdrpac_msg_cb,  hdrpacc_msg_cb},         /* HDRHAI_N_APP3_TYPE = 0x17 */
   {NULL,           hdrpacc_config_msg_cb}}, 

#ifdef FEATURE_HDR_BCMCS
  {{hdrbccp_msg_cb, NULL},                   /* HDRHAI_N_BCMCS_TYPE = 0x18 */
   {NULL,           hdrscm_config_msg_cb}},
#else
  {{NULL, NULL}, 
  {NULL, NULL}}, 
#endif /* FEATURE_HDR_BCMCS */

  {{NULL, NULL},                             /* HDRHAI_N_RESERVED  = 0x19 */
   {NULL, NULL}},

  {{NULL, NULL},                             /* HDRHAI_N_VSP_TYPE  = 0x1A */
   {NULL, NULL}},

  {{NULL, NULL},                             /* HDRHAI_N_GMMCDP_TYPE = 0x1B */
   {NULL, hdrscm_config_msg_cb}},

  {{NULL, NULL},                             /* HDRHAI_N_GMMCDP_TYPE = 0x1C */
   {NULL, NULL}},

#ifdef FEATURE_LTE_TO_HDR_OH
  {{hdrsap_msg_cb,  NULL},                   /* HDRHAI_N_GMMCDP_TYPE = 0x1D */
   {NULL,           NULL}}
#else
  {{NULL, NULL},
   {NULL, NULL}}
#endif /* FEATURE_LTE_TO_HDR_OH */
};

static struct
{
  /* Queue for pending message commands */
  q_type pending_msg_q;

  /* Queue for outstanding AC messages */
  q_type ac_msg_list;

  /* Queue for messages sent on RTC that can also go on AC */
  q_type rtc_msg_list;

  /* Queue for outstanding application layer RASM messages */
  q_type rasm_list;

  struct
  {
    hdraddr_type subnet; /* ID of subnet last session was closed on */
    uint8 count; /* Number of consecutive sessions that have been closed */
    boolean session_opened_successfully; /* Flag if session was opened. */
  } sclose;

  /* Number of send message commands pending on the event queue */
  int msgs_on_event_q;

  int rasm_log_id; /* for rasm logging purpose only */

  /* Timer for waiting DOS if bundle flag is set to BUNDLE_WITH_CR_DOS for ROR
   * One timer is enought. becasue if this timer expires, all msg in SLP queue
   * (including all ROR queued to slp after this timer is running) will be sent out
   */
  rex_timer_type bundle_waiting_timer; 

  /* Used to determine if we are still waiting for more messages before 
   * starting access attempt */
  uint8 waiting_bundled_msg_mask;
  /* Instead of clearing bit in waiting_bundled_msg_mask after receiving  waiting
   * message, we use received_bundled_msg_mask to indicate received messages
   * so we can handle multiple messages with bundle_mask set
   */
  uint8 received_bundled_msg_mask;

  /* if HDR has already reported RASM Q full event (sys lost) to SD */
  boolean buf_full_reacq_pending;

#ifdef FEATURE_LTE_TO_HDR_OH
  /* Used to determine when we can re-open session in tunnel mode */ 
  hdrhmp_sc_backoff_type sc_backoff;
#endif

  rex_crit_sect_type crit_sect; /* Critical Section for hdrhmp.rtc_msg_list */
  
  boolean ignore_cc_msgs; /* Ignore incoming CC msgs */
} hdrhmp;

/* <EJECT> */

/*=============================================================================

                        FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION HDRHMP_MSGS_ON_EVENT_Q

DESCRIPTION
  This function updates hdrhmp.msgs_on_event_q in a thread-safe way.

DEPENDENCIES
  None

PARAMETERS
  modifier - Value to be added to (or subtracted from) hdrhmp.msgs_on_event_q.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrhmp_msgs_on_event_q
( 
  int modifier
)
{
  REX_DISABLE_PREMPTION();
  hdrhmp.msgs_on_event_q += modifier;
  REX_ENABLE_PREMPTION();
  HDR_MSG_PROT_2 (MSG_LEGACY_LOW, "hdrhmp_generic_send_msg :%d,%d",
                  hdrhmp.msgs_on_event_q, q_cnt(&hdrhmp.pending_msg_q));
} /* hdrhmp_msgs_on_event_q() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_TIMER_CB

DESCRIPTION
  This function is the callback for all HMP state protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC), however
  it runs as an independent thread and thus must queue an event for HDRMC.

DEPENDENCIES
  None

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrhmp_timer_cb( uint32 timer_id )
{
  hdrmc_queue_timer(HDRHAI_HDR_MESSAGING_PROTOCOL, timer_id);
} /* hdrhmp_timer_cb() */

/*===========================================================================

FUNCTION HDRHMP_RASM_INITIALIZE_ENTRY

DESCRIPTION
  This function initializes a rasm_list entry.

DEPENDENCIES
  None

PARAMETERS
  rasm_ptr - buffer holding RASM message
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void hdrhmp_rasm_initialize_entry
(
  hdrhmp_rasm_buf_type *rasm_ptr
)  
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rasm_ptr->state = HDRHMP_RASMS_FREE;
  rasm_ptr->substate = HDRHMP_RASMSS_NONE;

  rasm_ptr->attempts = 0;
  rasm_ptr->log_id = 0;

  (void) rex_clr_timer( &rasm_ptr->interval_timer );
  (void) rex_clr_timer( &rasm_ptr->overall_timer );
} /* hdrhmp_rasm_initialize_entry() */


/*===========================================================================

FUNCTION HDRHMP_DELETE_RASM_ENTRY

DESCRIPTION
  This function deletes an entry in the hdrhmp_rasm_list an calls the 
  original sender's callback to inform them of the outcome of their
  message transmission.

DEPENDENCIES
  None

PARAMETERS
 None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrhmp_delete_rasm_entry
( 
  hdrhmp_rasm_buf_type *rasm_ptr,
  hdrerrno_enum_type status  
)
{
  hdrhmp_msg_buf_type *buf_ptr; /* convenience ptr to wrapped buffer */
  hdrhmp_callback_info_type callback_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  buf_ptr = &rasm_ptr->buf;

  HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                  "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                  (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                  (uint32) HDRHMP_RASMS_FREE | (uint32) HDRHMP_RASMSS_NONE);

  hdrhmp_rasm_initialize_entry( rasm_ptr );

  callback_info.outcome_cb = buf_ptr->msg_cmd.outcome_cb;
  callback_info.outcome_tag_ptr = buf_ptr->msg_cmd.outcome_tag_ptr;
  hdrhmp_free_msg( buf_ptr->msg_cmd.msg_ptr, &callback_info, status );
  rex_undef_timer(&rasm_ptr->overall_timer); 
  rex_undef_timer(&rasm_ptr->interval_timer); 
  modem_mem_free( buf_ptr, MODEM_MEM_CLIENT_HDR_CRIT);
  /*lint +esym(613, buf_ptr) buf_ptr is not NULL */
} /* hdrhmp_delete_rasm_entry() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_PROCESS_REMOVE_RASM_ENTRY

DESCRIPTION
  This function takes rasm msg that We no longer want to keep trying to send it.
  Rasm msg is no longer needed could be following reasons:
    - Received L3 Ack.
    - Overall timer expired.
    - hdrhmp_clear_rasm_queue() is called.
  
  This function handle rasm msg based on different rasm state:
    - HDRHMP_RASMS_WAITING: 
                    RASMSS_WAITING_REXMIT:
                            1. move rasm to DONE state. 
                            2. call user cb. 
                   !RASMSS_WAITING_REXMIT:
                            1. delete msg from rasm list.
                            2. call user cb.
    - HDRHMP_RASMS_PENDING: 
                            1. delete msg from pending msg list. 
                            2. delete msg from rasm list. 
                            3. call user cb.
                            4. attempt to abort pending connection.
    - HDRHMP_RASMS_QUEUED:     
                            1. move rasm to DONE state.
                            2. call user cb. 
                            3. attempt to abort pending connection.
    - HDRHMP_RASMS_DONE:     
                            1. Do nothing. 

DEPENDENCIES
  None

PARAMETERS
  rasm_ptr  - pointer to rasm buffer holding RASM message
  reason    - reason to remove RASM message 
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrhmp_process_remove_rasm_entry
(
  hdrhmp_rasm_buf_type *rasm_ptr, /* buffer holding RASM message */
  hdrhmp_rasm_remove_reason_enum_type reason
)
{
  hdrhmp_msg_buf_type *buf_ptr; /* convenience ptr to wrapped buffer */
  hdrhmp_msg_buf_type *pending_buf_ptr; /* buffer holding pending message */
  hdrhmp_msg_buf_type *next_pending_ptr; /* buffer holding pending message */
  hdrerrno_enum_type status;  
  boolean attempt_aborting_pending_connection = FALSE;
  uint8 msg_id; /* for debug only */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  buf_ptr = &rasm_ptr->buf;

  (void) dsmbit_unpack8( buf_ptr->msg_cmd.msg_ptr, &msg_id, 0, 8 );

  if (reason == HDRHMP_RASM_REMOVE_TIMEDOUT)
  {
    status = E_TIMER_EXP;
  }
  else if (reason == HDRHMP_RASM_REMOVE_CLEARED)
  {
    status = E_FAILURE;
  }
  else
  {
    ASSERT(reason == HDRHMP_RASM_REMOVE_L3ACKED);

    status = E_SUCCESS;
  }

  switch (rasm_ptr->state)
  {
    case HDRHMP_RASMS_WAITING:

      if (rasm_ptr->substate == HDRHMP_RASMSS_WAITING_HMP_REXMIT)
      {
        /* It means that msg is already queued to hdrmc.
         * move msg to DONE state becasue it's no longer needed.
         * hdrhmp_process_send_msg_cmd will NOT send msg if it is in DONE state.
         */
        HDR_MSG_PROT_4 (MSG_LEGACY_HIGH, 
                        "RASM %d: RR %d, Rexmit cmd queued alreay (P 0x%x, ID 0x%x)", 
                        rasm_ptr->log_id, reason,
                        buf_ptr->msg_cmd.protocol_name, msg_id);

            HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                        "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                    (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                    (uint32) HDRHMP_RASMS_DONE | (uint32) HDRHMP_RASMSS_NONE);

        rasm_ptr->state = HDRHMP_RASMS_DONE;
        rasm_ptr->substate = HDRHMP_RASMSS_NONE; 

        /* we can call user's cb now, but don't remove rasm from rasm list yet. 
         * Wait for HMP to remove when it processes the send msg cmd (currently
         * on the event q) 
         */ 
        (buf_ptr->msg_cmd.outcome_cb)
          (buf_ptr->msg_cmd.outcome_tag_ptr, status);
      }
      else
      {
        HDR_MSG_PROT_4 (MSG_LEGACY_HIGH, 
                        "RASM %d: RR %d, State: Waiting for L3Ack, unlock or to be queued (P 0x%x, ID 0x%x)", 
                        rasm_ptr->log_id, reason,
                            buf_ptr->msg_cmd.protocol_name, msg_id);

        /* Instead of moving it to DONE state and waiting for slp cb , 
         * just remove rasm message immediately because msg is no longer needed and 
         * hasn't queued to slp yet.
         */
        q_delete( &hdrhmp.rasm_list, &buf_ptr->hdr.link );
        hdrhmp_delete_rasm_entry( rasm_ptr, status );
      }
      break;

    case HDRHMP_RASMS_PENDING:

      HDR_MSG_PROT_4 (MSG_LEGACY_HIGH, 
                      "RASM %d: RR %d, Still pending (P 0x%x, ID 0x%x)", 
                      rasm_ptr->log_id, reason,
                            buf_ptr->msg_cmd.protocol_name, msg_id);

      /* remove rasm message from pending_msg_q if any*/
      pending_buf_ptr = (hdrhmp_msg_buf_type *) q_check( &hdrhmp.pending_msg_q );

      while ( pending_buf_ptr != NULL)
      {
        /* Get next entry now in case we delete the current one */
        next_pending_ptr = q_next( &hdrhmp.pending_msg_q, &pending_buf_ptr->hdr.link);

        if ((pending_buf_ptr->msg_cmd.protocol_name == 
             buf_ptr->msg_cmd.protocol_name) &&
            (pending_buf_ptr->msg_cmd.rasm.transaction_id == 
             buf_ptr->msg_cmd.rasm.transaction_id) &&
             pending_buf_ptr->msg_cmd.rasm.is_enabled)
        {
          /* remove message from pending_msg_q 
           * after calling q_delete and dsm_free_packet, msg is in rasm list only
           */
          q_delete( &hdrhmp.pending_msg_q, &pending_buf_ptr->hdr.link );
          dsm_free_packet(&pending_buf_ptr->msg_cmd.msg_ptr); 
          modem_mem_free( pending_buf_ptr, MODEM_MEM_CLIENT_HDR_CRIT );

          break;
        }
        pending_buf_ptr = next_pending_ptr;
      }

      /* TODO:
       * Should we ASSERT(pending_buf_ptr != NULL)?
       * Is it possible RASM still in PENDING state when pending_q is empty?
       * check pending then CC case. because slp gets returning_to_idle first
       * then hmp, it should be ok.
       */

      /* Instead of waiting for slp cb then remove message from rasm lsit, 
       * remove rasm message immediately because it already timed out and 
       * hasn't queued to slp yet.
       */
      q_delete( &hdrhmp.rasm_list, &buf_ptr->hdr.link );
      hdrhmp_delete_rasm_entry( rasm_ptr, status );

      /* It's possible that we can abort pending connection if it is only  
       * for sending msg. 
       * This can avoid waiting forever for the CR msg to go throught in high 
       * A_Persistence network.
       */ 
      attempt_aborting_pending_connection = TRUE;
      break;

    case HDRHMP_RASMS_QUEUED:

      HDR_MSG_PROT_4 (MSG_LEGACY_HIGH, 
                      "RASM %d: RR %d, Still queued to SLP (P 0x%x, ID 0x%x)", 
                      rasm_ptr->log_id, reason,
                      buf_ptr->msg_cmd.protocol_name, msg_id);

      HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                      "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                      (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                      (uint32) HDRHMP_RASMS_DONE | (uint32) HDRHMP_RASMSS_NONE);

      rasm_ptr->state = HDRHMP_RASMS_DONE;
      rasm_ptr->substate = HDRHMP_RASMSS_NONE; 

      /* If message alreay queued to slp and the overall_timer expires,
       * we can call user's cb now, but don't remove rasm from rasm list yet. 
       * Wait for slp to call rasm cb to remove rasm msg */ 
      (buf_ptr->msg_cmd.outcome_cb)
        (buf_ptr->msg_cmd.outcome_tag_ptr, status);

      /* It's possible that we can abort pending connection if it's only for  
       * sending msg. 
       * This can avoid waiting forever for the rasm msg bundled with CR
       * to go through in high  A_Persistence network. (e.g. ROR + CR)
       */ 
      if (reason == HDRHMP_RASM_REMOVE_TIMEDOUT )
      {
        attempt_aborting_pending_connection = TRUE;
      }
      break;

    case HDRHMP_RASMS_DONE:

      /* Message is in DONE state, it implies that we already called user cb and 
       * are waiting for slp cb to remove msg from rasm list. Do nothing here.
       * There are three reasons that we move RASM to DONE state:
       *   1. received L3 msg
       *   2. timed out
       *   3. called hdrhmp_clear_rasm_queue()
       */
      if (reason == HDRHMP_RASM_REMOVE_TIMEDOUT)
          {
        /* Either already received L3 or called clear_rasm_queue() */
        HDR_MSG_PROT_4 (MSG_LEGACY_HIGH, 
                        "RASM %d: RR %d, Already Acked or Cleared (P 0x%x, ID 0x%x)", 
                        rasm_ptr->log_id, reason,
                        buf_ptr->msg_cmd.protocol_name, msg_id);
      }
      else if (reason == HDRHMP_RASM_REMOVE_CLEARED)
      {
        /* Either already received L3 or timed out*/
       HDR_MSG_PROT_4 (MSG_LEGACY_HIGH, 
                       "RASM %d: RR %d, Already Acked or timed out (P 0x%x, ID 0x%x)", 
                       rasm_ptr->log_id, reason,
                       buf_ptr->msg_cmd.protocol_name, msg_id);
      }
      else
      {
        /* We should NOT receive L3Ack in DONE state becasue before moving 
         * msg to DONE state, we will call user cb. 
         * After user receives slp cb, it shall not call hdrhmp_rasm_acked(). 
         */
        HDR_MSG_PROT_4 (MSG_LEGACY_FATAL, 
                        "RASM %d: RR %d, Unexpected rasm_acked (P 0x%x, ID 0x%x)", 
                        rasm_ptr->log_id, reason,
                            buf_ptr->msg_cmd.protocol_name, msg_id);
          }

      /* Before we move rasm to DONE state, we already attempted to abort pending 
       * connection.
       */
      attempt_aborting_pending_connection = FALSE;
          break;

        default:
      ERR_FATAL( "Try to remove rasm entry while it's in RASMS_FREE state", 0, 0, 0 );
      break;
  }

  if (attempt_aborting_pending_connection)
  {
    if (q_cnt(&hdrhmp.pending_msg_q) == 0 && 
        !( hdralmp_connection_is_open( HDRHAI_HDR_MESSAGING_PROTOCOL ) ))
    {
      hdralmp_close_connection(HDRHAI_ABORT_MSG, HDRHAI_HDR_MESSAGING_PROTOCOL );
    }
  }
} /* hdrhmp_process_remove_rasm_entry() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_RASM_CB

DESCRIPTION
  The following status is expected:

  E_SUCCESS - Message transmitted (best effort) or SLP ACK (reliable)

  E_NOT_ALLOWED	- Queued to SLP for transmission on wrong channel

  E_CANCELED - a) EV-DO not acquired
               b) Held pending connection setup when system was lost
               c) ACMAC deactivated during access e.g. TCA received
               d) System lost
               e) SLP reset

  E_AGAIN -    a) Held pending connection setup; connection setup failed
               b) DUP packet failed (unexpected)
               c) Message given to RMAC but frame was never sent

  E_ABORTED -  a) Access handoff
               b) Connection setup aborted by the user

  E_FAILURE -  a) Session was closed
               b) PWR_SAVE
               c) Max access probes
               d) No session at time message was queued for SLP
               e) Max SLP retries attempted and last retry timed out
               f) For RTC only messages, HMP can fail for other rare 
                  reasons (see hdrhmp_msg_is_sendable)

  E_PROT_Q_FULL - No buffers for the message (unexpected)

  E_NOT_AVAILABLE	- Configuration is locked
  
DEPENDENCIES
  None

PARAMETERS
  data_ptr - The application layer messge those status is being reported
  status - The status of the transmission of the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_rasm_cb
(
  void *data_ptr, 
  hdrerrno_enum_type status 
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdrhmp_cmd_type hdrhmp_cmd;

  /* Queue rasm_cb to HDRMC if it's not from current task. For example,
   * When hdrslptx (TX Task) calls hdrhmp_rasm_cb().
   */ 
  hdrhmp_cmd.name = HDRHMP_RASM_CB_CMD;
  hdrhmp_cmd.params.rasm_cb.data_ptr = data_ptr;
  hdrhmp_cmd.params.rasm_cb.status = status;

  if (rex_self() == HDRHMP_TASK_PTR)
  {
    hdrhmp_cmd.sender = HDRHAI_HDR_MESSAGING_PROTOCOL;
    hdrhmp_cmd.params.rasm_cb.protocol_name = HDRHAI_HDR_MESSAGING_PROTOCOL;
    hdrhmp_process_cmd( &hdrhmp_cmd );
      }
  else
  {
    /* TX task has higher priority than HDRMC so We have to queue cb cmd back to 
     * HDRMC if it is called from SLP.  
     */
    hdrhmp_cmd.sender = HDRHAI_SIGNALING_LINK_PROTOCOL;
    hdrhmp_cmd.params.rasm_cb.protocol_name = HDRHAI_SIGNALING_LINK_PROTOCOL;
    hdrmc_queue_cmd( HDRHAI_HDR_MESSAGING_PROTOCOL, &hdrhmp_cmd,
                     sizeof( hdrhmp_cmd_type ) );
    }
} /* hdrhmp_rasm_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_PROCESS_RASM_OVERALL_TIMEOUT

DESCRIPTION
  The per-message timer expiration indicates that a message has been hung
  up in RASM retransmissions for a long time.  This can happen, for example,
  if the connection setup that is being performed to send an 
  AttributeUpdateRequest message to send ReservationKKQoSRequest is 
  interrupted repeatedly by a fade before an ACAck is received.
  

DEPENDENCIES
  None

PARAMETERS
 None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrhmp_process_rasm_overall_timeout( void )
{
  hdrhmp_rasm_buf_type *rasm_ptr; /* buffer holding RASM message */
  hdrhmp_rasm_buf_type *next_ptr; /* buffer holding RASM message */
  hdrhmp_msg_buf_type *buf_ptr; /* convenience ptr to wrapped buffer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Just loop through all the messages in the list and check their timers.
   * if one is expired, delete it.  If it is still queued for SLP, mark it as 
   * timed out and wait for the SLP callback (hdrhmp_rasm_cb()) to delete it.
   */

  rasm_ptr = (hdrhmp_rasm_buf_type *) q_check( &hdrhmp.rasm_list );

  while (rasm_ptr != NULL)
  {
    buf_ptr = &rasm_ptr->buf;

    /* Get next entry now in case we delete the current one */
    next_ptr = q_next( &hdrhmp.rasm_list, &buf_ptr->hdr.link );

    if (rex_get_timer( &rasm_ptr->overall_timer ) == 0)
    {
      HDRHMP_RASM_TRACE( OT );

      hdrhmp_process_remove_rasm_entry(rasm_ptr, HDRHMP_RASM_REMOVE_TIMEDOUT);
    }
    rasm_ptr = next_ptr;
  }
} /* hdrhmp_process_rasm_overall_timeout() */

/*===========================================================================

FUNCTION HDRHMP_PROCESS_RASM_INTERVAL_TIMEOUT

DESCRIPTION
  The per-message timer expiration indicates that a message has been 
  sent on the RTC and no L3 ACK was received within the specified 
  retransmission time.
  

DEPENDENCIES
  None

PARAMETERS
 None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrhmp_process_rasm_interval_timeout( void )
{
  hdrhmp_rasm_buf_type *rasm_ptr; /* buffer holding RASM message */
  hdrhmp_rasm_buf_type *next_ptr; /* buffer holding RASM message */
  hdrhmp_msg_buf_type *buf_ptr; /* convenience ptr to wrapped buffer */
  uint8 msg_id; /* for debug only */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!hdralmp_system_is_acquired( HDRHAI_HDR_MESSAGING_PROTOCOL ))
  {
    /* If we aren't acquired, we will resend when we reacquire */
    return;
  }

  /* Just loop through all the messages in the list and check their timers.
   * If one is expired, retransmit it.
   */

  rasm_ptr = (hdrhmp_rasm_buf_type *) q_check( &hdrhmp.rasm_list );

  while (rasm_ptr != NULL)
  {
    buf_ptr = &rasm_ptr->buf;

    /* Get next entry now in case we delete the current one */
    next_ptr = q_next( &hdrhmp.rasm_list, &buf_ptr->hdr.link );

    if ((rasm_ptr->state == HDRHMP_RASMS_WAITING) &&
        (rasm_ptr->substate == HDRHMP_RASMSS_WAITING_L3_ACK) &&
        (rex_get_timer( &rasm_ptr->interval_timer ) == 0))
    {
      HDRHMP_RASM_TRACE( IT );

      (void) dsmbit_unpack8( buf_ptr->msg_cmd.msg_ptr, &msg_id, 0, 8 );

      if (rasm_ptr->attempts <= buf_ptr->msg_cmd.rasm.max_retries)
      {
        HDR_MSG_PROT_5 (MSG_LEGACY_HIGH, 
                        "RASM %d IT: Resending (P 0x%x, ID 0x%x) (attempt %d of %d)", 
                        rasm_ptr->log_id,
                        buf_ptr->msg_cmd.protocol_name, msg_id,
                        rasm_ptr->attempts + 1,
                        buf_ptr->msg_cmd.rasm.max_retries + 1);

      HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                      "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                        (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                        (uint32) rasm_ptr->state | (uint32) HDRHMP_RASMSS_WAITING_HMP_REXMIT);

      /* This entry's retransmission timer has expired; Retransmit */
      rasm_ptr->substate = HDRHMP_RASMSS_WAITING_HMP_REXMIT;

        hdrhmp_rasm_retransmit( buf_ptr );
      }
      else
      {
        /* Last interval expired, so the message has failed. */
          HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                          "RASM %d IT: Removing (P 0x%x, ID 0x%x)", 
                          rasm_ptr->log_id,
                          buf_ptr->msg_cmd.protocol_name, msg_id);

          /* Remove item from list. */
          q_delete( &hdrhmp.rasm_list, &buf_ptr->hdr.link );
          hdrhmp_delete_rasm_entry( rasm_ptr, E_FAILURE );
      }
    }

    rasm_ptr = next_ptr;
  }
} /* hdrhmp_process_rasm_interval_timeout() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_STOP_WAITING_FOR_BUNDLING_MSG
DESCRIPTION
  This function is used to reset HDRHMP bundle masks and bundling flag in
  HDRSLP.

  This function will be called when bundle timer expires and we reset all bundle
  related flag(in SLP) and masks(in HMP), so it should not have case that the AC 
  couldn't start later due to flag or mask is not "clear".

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrhmp_stop_waiting_for_bundling_msg (void)
{
  if( rex_get_timer( &hdrhmp.bundle_waiting_timer ) != 0 )
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "BUNDLE: Stop waiting for bundling msg");

    (void) rex_clr_timer(&hdrhmp.bundle_waiting_timer);
  }

  /* We can also only clear flag and masks when bundle timer is running,
   * but it doesn't hurt to always clear them*/
  hdrslp_set_waiting_for_bundling_flag(FALSE);

  hdrhmp.waiting_bundled_msg_mask = HDRHMP_BUNDLE_NONE_MSG;
  hdrhmp.received_bundled_msg_mask = HDRHMP_BUNDLE_NONE_MSG;

} /* hdrhmp_stop_waiting_for_bundling_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_PROCESS_BUNDLE_WAITING_TIMER_EXP

DESCRIPTION
  Timer expiration indicates that ROR message has been delay  
  for HDRHMP_BUNDLE_WAITING_TIME_MSEC to wait for DOS msg.
  ROR message is already in SLP queue, it's time to bundle it with ConnectionRequest
  and send it out.

DEPENDENCIES
  None

PARAMETERS
 None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrhmp_process_bundle_waiting_timer_exp( void )
{

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "BUNDLE: Timer expired." );
  hdrhmp_stop_waiting_for_bundling_msg();
 
  #ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
  if (hdrslp_is_waiting_for_uati_assignment())
  {
    /* Don't need to start access attempt now 
     * Even we start it now, messages won't be sent out due to 
     * hdrslp_waiting_for_uati_assignment flag is TURE*/
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "BUNDLE: bundle timer exp while waiting for UATIAssignment");
  }
  else
  #endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */
  {
    if (hdrslp_tx_queue_is_empty( HDRHAI_HDR_MESSAGING_PROTOCOL ))
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "BUNDLE: No pending msgs so no need to start access.");
    }
    else
    {
      /* If there are message queued, try to start another access. */
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "BUNDLE: Start access to send pending msgs.");

      hdrhmp_send_msg( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                       HDRHAI_AC, FALSE, 255, HDRHAI_IN_USE, NULL, NULL, NULL );
    }
  }

} /* hdrhmp_process_bundle_waiting_timer_exp() */


#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRHMP_PROCESS_BACKOFF_TIMER_EXP

DESCRIPTION
  Timer expiration indicates that device can try to re-open session now.  

DEPENDENCIES
  None

PARAMETERS
 None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrhmp_process_backoff_timer_exp( void )
{
  HDR_MSG_PROT( MSG_LEGACY_MED, "Backoff Timer expired." );
 
  if ( !hdrsap_tunnel_mode_is_enabled( HDRHAI_HDR_MESSAGING_PROTOCOL ) )
  {
    /* Ignore backoff timer expired event if tunnel mode is not enabled */
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Tunnel mode is not enabled" );
    return;
  }

  if ( hdrovhd_prereg_is_allowed( HDRHAI_HDR_MESSAGING_PROTOCOL ) )
  {
    if ( hdrsmp_is_to_maintain_evdo_session() )
    {
      /* We are idle on the system and can try re-open the session now */
      HDR_MSG_PROT( MSG_LEGACY_HIGH, " HMP: Opening new session ..." );
      hdrsmp_activate( HDRHAI_HDR_MESSAGING_PROTOCOL );
    }
    else
    {
      /* Wait until we actually want to use data services 
       * before opening the session. */
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    " MUM: data call is required to open session" );
    }
  }
  else
  {
    /* We want to try to re-open the session on this system but need
     * to wait for PreRegAllowed  == TRUE 
     */
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "HMP: Waiting for PreRegAllowed to open new session ..." );
  }

} /* hdrhmp_process_backoff_timer_exp() */
#endif /* FEATURE_LTE_TO_HDR_OH */


/*===========================================================================

FUNCTION HDRHMP_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the HMP.

DEPENDENCIES
  None

PARAMETERS
 timer_id - The ID of the timer that has expired

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_process_timer( uint32 timer_id )
{
  switch ( timer_id )
  {
    case HDRHMP_RASM_OVERALL_TIMER:
      hdrhmp_process_rasm_overall_timeout();
      break;

    case HDRHMP_RASM_INTERVAL_TIMER:
      hdrhmp_process_rasm_interval_timeout();
      break;

    case HDRHMP_BUNDLE_WAITING_TIMER:
      hdrhmp_process_bundle_waiting_timer_exp();
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRHMP_BACKOFF_TIMER:
      hdrhmp_process_backoff_timer_exp();
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default :
      ERR("Invalid HMP timer id!",0,0,0);
      break;
  }
} /* hdrhmp_process_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_POWERUP_INIT

DESCRIPTION
  This function performs one-time powerup initialization required for HMP

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrhmp_powerup_init( void )
{
  if ( hdrmc_is_first_init() )
  {
    /* Initialize queues. */
    ( void ) q_init( &hdrhmp.pending_msg_q );
    ( void ) q_init( &hdrhmp.ac_msg_list );
    ( void ) q_init( &hdrhmp.rtc_msg_list );
    ( void ) q_init( &hdrhmp.rasm_list );
    
    rex_init_crit_sect( &hdrhmp.crit_sect ); /* Initialize the critical section */
    
    rex_def_timer_ex( &hdrhmp.bundle_waiting_timer,
                      hdrhmp_timer_cb,
                      HDRHMP_BUNDLE_WAITING_TIMER );
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrmc_is_first_init() )
  {
    rex_def_timer_ex( &hdrhmp.sc_backoff.backoff_timer,
                      hdrhmp_timer_cb,
                      HDRHMP_BACKOFF_TIMER );
  }

  hdrhmp.sc_backoff.cnt = 0;
  hdrhmp.sc_backoff.colorcode = 0;
#endif /* FEATURE_LTE_TO_HDR_OH */

  hdrhmp.sclose.count = 0;
  hdrhmp.sclose.session_opened_successfully = FALSE;

  hdrhmp.msgs_on_event_q = 0;
  hdrhmp.waiting_bundled_msg_mask = HDRHMP_BUNDLE_NONE_MSG;
  hdrhmp.received_bundled_msg_mask = HDRHMP_BUNDLE_NONE_MSG;

#ifdef FEATURE_LTE_TO_HDR_OH
  /* Read session failure backoff time table from EFS NV. */
  if ( (int)efs_get( HDRHMP_BACKOFF_TIME_TABLE_SESSION_FAILED,
                     &backoff_time_table_sc[0],
                     ( sizeof(uint16)*HDRHMP_BACKOFF_TIMER_CNT_MAX ) ) 
         == (int)( sizeof(uint16)*HDRHMP_BACKOFF_TIMER_CNT_MAX ) )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Read session failure backoff time table successfully" );
  }
  else 
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Read session failure backoff time table failed. Use default" );
  }
#endif /* FEATURE_LTE_TO_HDR_OH */
  
} /* hdrhmp_powerup_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_INIT

DESCRIPTION
  This function performs initialization required for HMP every time HDR
  mode is entered.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrhmp_init( void )
{

  HDRTRACE_INIT_DEFAULT(HDRHAI_HDR_MESSAGING_PROTOCOL);
  hdrhmpsa_init();

  /* reset flag when HDR is activated */
  hdrhmp.buf_full_reacq_pending = FALSE;
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "hdrhmp_init Resetting the current value %d",hdrhmp.msgs_on_event_q);
  hdrhmp.msgs_on_event_q = 0;
} /* hdrhmp_init() */

/*===========================================================================

FUNCTION HDRHMP_PROCESS_RASM_CB

DESCRIPTION
  RASM CB may be called by either hdrhmp or hdrslp. This function process 
  rasm cb based on callback status and current RASM message infomation. 
  
DEPENDENCIES

PARAMETERS
  rasm_cb_cmd_ptr - Pointer to the "RASM CB" command for HMP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void hdrhmp_process_rasm_cb 
( 
  hdrhmp_rasm_cb_params_type *rasm_cb_cmd_ptr
)
{

  hdrhmp_rasm_buf_type *rasm_ptr; /* buffer holding RASM message */
  hdrhmp_msg_buf_type *buf_ptr; /* convenience ptr to wrapped buffer */
  uint8 msg_id; /* for debug message */
  hdrhmp_rasm_state_enum_type next_state;
  hdrhmp_rasm_substate_enum_type next_substate;
  enum 
  {
    HDRHMP_RASM_RETRY_NONE,
    HDRHMP_RASM_RETRY_NOW,
    HDRHMP_RASM_RETRY_AFTER_INTERVAL_TIMEOUT,
    HDRHMP_RASM_RETRY_AFTER_CONFIG_UNLOCKED
  } retry = HDRHMP_RASM_RETRY_NONE;            /* Assume no retry */
  hdrerrno_enum_type status = rasm_cb_cmd_ptr->status; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (rasm_cb_cmd_ptr->data_ptr != NULL);
  rasm_ptr = (hdrhmp_rasm_buf_type *) rasm_cb_cmd_ptr->data_ptr;
  buf_ptr = &rasm_ptr->buf;
  /*lint -esym(613, buf_ptr) buf_ptr is not NULL */

  ASSERT (rasm_ptr->state != HDRHMP_RASMS_FREE);

  /* rasm_cb can be called for a WAITING message if it was not sendable at
   * the time it was first given to HMP. */

  HDRHMP_RASM_TRACE( CB );

  if (buf_ptr->msg_cmd.bundle_mask != HDRHMP_BUNDLE_NONE_MSG)
  {
     /* The bundling design is under one assumption:
     * No two different messages with bundle_mask set will be queued to SLP in the same time.
     *
     * If two RORs are in SLP queue and rasm_cb() happens when bundle timer is still running, 
     * it means slp tx failed or system lost,  clear flag and timer for all RORs should be
     * fine */
    /* TODO: handle concurrent bundling requests (by adding bundle id in slp params?) */
    hdrhmp_stop_waiting_for_bundling_msg();
  }

  if (dsm_extract(buf_ptr->msg_cmd.msg_ptr, 0, &msg_id, 1 ) == 1)
  {
    HDR_MSG_PROT_6 (MSG_LEGACY_HIGH, "RASM %d CB: (P 0x%x, ID 0x%x) errno %d, TxCh/MsgCh %d/%d (1=AC 3=RTC 4=AC_OR_RTC)", 
                    rasm_ptr->log_id,
                    buf_ptr->msg_cmd.protocol_name, msg_id, status,
                    rasm_ptr->tx_chan, buf_ptr->msg_cmd.chan);
  }
  else
  {
    HDR_MSG_PROT_5 (MSG_LEGACY_HIGH, "RASM %d CB: (P 0x%x, ID N/A) errno %d, TxCh/MsgCh %d/%d (1=AC 3=RTC 4=AC_OR_RTC)", 
                    rasm_ptr->log_id,
                    buf_ptr->msg_cmd.protocol_name, status,
                    rasm_ptr->tx_chan, buf_ptr->msg_cmd.chan);
  }

  if (rasm_ptr->state == HDRHMP_RASMS_DONE)
  {
    /* If msg has moved to DONE state (L3Acked, Timedout, Cleared),
     * hdrhmp already called user callback. Now just remove msg from rasm list */
    q_delete( &hdrhmp.rasm_list, &buf_ptr->hdr.link );

    HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                    "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                    (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                    (uint32) HDRHMP_RASMS_FREE | (uint32) HDRHMP_RASMSS_NONE);

    hdrhmp_rasm_initialize_entry( rasm_ptr );

    dsm_free_packet(&buf_ptr->msg_cmd.msg_ptr); 
	rex_undef_timer(&rasm_ptr->overall_timer);    
	rex_undef_timer(&rasm_ptr->interval_timer);
    modem_mem_free( buf_ptr, MODEM_MEM_CLIENT_HDR_CRIT );

    return;
  }
  else if (status == E_NOT_ALLOWED)
  {
    retry = HDRHMP_RASM_RETRY_NOW;

    HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                    "RASM %d CB: Resend. %d not allowed on %d SLP wasn't ready", 
                    rasm_ptr->log_id,
                    buf_ptr->msg_cmd.chan, rasm_ptr->tx_chan);
  }
  else if (status == E_NOT_AVAILABLE)
  {
    retry = HDRHMP_RASM_RETRY_AFTER_CONFIG_UNLOCKED;

    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "RASM %d CB: Waiting for ConfigurationUnlock", 
                    rasm_ptr->log_id );
  }
  else if (status == E_CANCELED)
  {
    retry = HDRHMP_RASM_RETRY_NOW;

    HDR_MSG_PROT_1 ( MSG_LEGACY_HIGH, "RASM %d CB: Resend. msg canceled", 
                     rasm_ptr->log_id );
  }
  else if (status == E_AGAIN)
  {
    if (buf_ptr->msg_cmd.chan == HDRHAI_AC_OR_RTC)
    {
      retry = HDRHMP_RASM_RETRY_NOW;

      HDR_MSG_PROT_1 ( MSG_LEGACY_HIGH,
                       "RASM %d CB: Resend. Con setup fail for AC_OR_RTC msg", 
                       rasm_ptr->log_id);
    }
    else
    {
      /* E_AGAIN represents connection setup failure, which for RTC only 
       * messages could cause a lot of back-to-back connection setup attempts.  
       * So for now this failure is passed up to the caller to handle for 
       * RTC only msgs like AttributeUpdateRequest for ReservationKKQoSRequest.
       */
    }
  }
  else if ((buf_ptr->msg_cmd.chan == HDRHAI_AC_OR_RTC) &&
           (status == E_ABORTED))
  {
    retry = HDRHMP_RASM_RETRY_NOW;

    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "RASM %d CB: Resend. Access Handoff AHO", 
                    rasm_ptr->log_id);
  }

  if ((status == E_SUCCESS) && 
      (rasm_ptr->tx_chan == HDRHAI_RTC) &&
      (buf_ptr->msg_cmd.rtc_is_reliable == FALSE))
  {
    /* We transmitted the frame with a best effort RTC message that has retries
     * requested.  This isn't our idea of "SUCCESS" unless there has also
     * been a L3 ack received.
     */
    /* We leave msg in RASM queue even after last retransmission, 
     * to wait for the L3 ACK
     */
    (void) rex_set_timer( &rasm_ptr->interval_timer, 
                            buf_ptr->msg_cmd.rasm.retry_interval );

    retry = HDRHMP_RASM_RETRY_AFTER_INTERVAL_TIMEOUT;

    if (rasm_ptr->attempts <= buf_ptr->msg_cmd.rasm.max_retries)
    {
      HDR_MSG_PROT_2 (MSG_LEGACY_HIGH,  "RASM %d CB: Waiting %d ms for retry",
                        rasm_ptr->log_id,
                        buf_ptr->msg_cmd.rasm.retry_interval);
    }
    else
    {
      HDR_MSG_PROT_2 (MSG_LEGACY_HIGH,  "RASM %d CB: Waiting %d ms for last chance ACK",
                        rasm_ptr->log_id,
                        buf_ptr->msg_cmd.rasm.retry_interval);
    }
  }

  /* --------------------------------------------------------------------------
   *                               CHANGE STATE
   * ------------------------------------------------------------------------*/

  if ( retry != HDRHMP_RASM_RETRY_NONE )
  {
    /* Set the state back to "waiting".  If the message is transmitted
     * again its state will be changed at that point.
     */

    next_state = HDRHMP_RASMS_WAITING;

    if (retry == HDRHMP_RASM_RETRY_AFTER_INTERVAL_TIMEOUT)
    {
      next_substate = HDRHMP_RASMSS_WAITING_L3_ACK;
    }
    else if (retry == HDRHMP_RASM_RETRY_AFTER_CONFIG_UNLOCKED)
    {
      next_substate = HDRHMP_RASMSS_WAITING_CONFIG_UNLOCK;
    }
    else if ((retry == HDRHMP_RASM_RETRY_NOW) &&
             (hdralmp_system_is_acquired( HDRHAI_HDR_MESSAGING_PROTOCOL )))
    {
      next_substate = HDRHMP_RASMSS_WAITING_HMP_REXMIT; 
    }
    else
    {
      next_substate = HDRHMP_RASMSS_NONE;
    }

    HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                    "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                    (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                    (uint32) next_state | (uint32) next_substate);

    rasm_ptr->state = next_state;
    rasm_ptr->substate = next_substate;

    /* The message is retried or failed based on the status passed into this
     * callback.  If the status was E_SUCCESS and the interval_timer was 
     * started, leave it running.  Otherwise some other action (leave waiting,
     * retry or fail) was decided, and the interval_timer is no longer needed.
     */
    if (retry != HDRHMP_RASM_RETRY_AFTER_INTERVAL_TIMEOUT)
    {
      (void) rex_clr_timer( &rasm_ptr->interval_timer );
    }
  }

  switch (retry)
  {
    case HDRHMP_RASM_RETRY_NONE:
      /* We no longer want to keep trying to send this message */
      /* Remove item from list. */
      q_delete( &hdrhmp.rasm_list, &buf_ptr->hdr.link );
      hdrhmp_delete_rasm_entry( rasm_ptr, status );
      break;

    case HDRHMP_RASM_RETRY_NOW:
      if ((rasm_ptr->state == HDRHMP_RASMS_WAITING) &&
          (rasm_ptr->substate == HDRHMP_RASMSS_WAITING_HMP_REXMIT))
      {
        hdrhmp_rasm_retransmit( buf_ptr );
      }
      break;

    case HDRHMP_RASM_RETRY_AFTER_INTERVAL_TIMEOUT:
    case HDRHMP_RASM_RETRY_AFTER_CONFIG_UNLOCKED:
    default:
      /* If we are waiting for some future event to resend, keep
       * the message on the RASM queue.
       */
      break;
  }

} /* hdrhmp_process_rasm_cb() */

/*===========================================================================

FUNCTION HDRHMP_AC_MSG_CB

DESCRIPTION
  This function is substituted as the SLP callback for access channel
  messages.  If the callback indicates the error E_ABORTED, that means there
  was handoff during the access attempt and the message must be re-sent.
  If the message is also allowed to go out on the RTC and the error
  E_NOT_ALLOWED is received, that also means the message should be re-sent
  because by the time SLP got it, the RTC was already active.
  Otherwise the original callback included with the message is called to
  handle the error.

DEPENDENCIES
  None

PARAMETERS
  data_ptr - The access channel messge those status is being reported
  status - The status of the transmission of the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void hdrhmp_ac_msg_cb ( void *data_ptr, hdrerrno_enum_type status )
{

  hdrhmp_msg_buf_type *buf_ptr; /* buffer holding AC message */
  uint8 msg_id; /* for debug message */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (data_ptr != NULL);
  buf_ptr = (hdrhmp_msg_buf_type *) data_ptr;
  /*lint -esym(613, buf_ptr) buf_ptr is not NULL */
  ASSERT (( buf_ptr->msg_cmd.chan == HDRHAI_AC ) ||
          ( buf_ptr->msg_cmd.chan == HDRHAI_AC_OR_RTC ));

  /* Remove item from list.  If we re-send, it will be re-queued */
  q_delete( &hdrhmp.ac_msg_list, &buf_ptr->hdr.link );

  if (status == E_ABORTED)
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH,  "Re-sending msg after AHO");
  }
  else if ((buf_ptr->msg_cmd.chan == HDRHAI_AC_OR_RTC) &&
           (status == E_NOT_ALLOWED))
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "AC_OR_RTC msg not allowed on AC, resend");
  }
  else if ((buf_ptr->msg_cmd.chan == HDRHAI_AC_OR_RTC) &&
           (status == E_CANCELED))
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH,  
      "AC_OR_RTC msg canceled for AC transmission, resend");
  }

  if (dsm_extract(buf_ptr->msg_cmd.msg_ptr, 0, &msg_id, 1 ) == 1)
  {
    HDR_MSG_PROT_2 (MSG_LEGACY_MED, "Removing pid 0x%x mid 0x%x",
                    buf_ptr->msg_cmd.protocol_name, msg_id);
  }
  else
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_MED, "Removing pid 0x%x mid NULL",
                    buf_ptr->msg_cmd.protocol_name);
  }

  if ((status == E_ABORTED) ||
      ((buf_ptr->msg_cmd.chan == HDRHAI_AC_OR_RTC) &&
       ((status == E_NOT_ALLOWED) || (status == E_CANCELED))))
    {
      hdrhmp_app_send_msg(
               buf_ptr->msg_cmd.protocol_name,
               buf_ptr->msg_cmd.stream,
               buf_ptr->msg_cmd.chan,
               buf_ptr->msg_cmd.rtc_is_reliable,
               buf_ptr->msg_cmd.priority,
               buf_ptr->msg_cmd.protocol_instance,
               buf_ptr->msg_cmd.msg_ptr,
               buf_ptr->msg_cmd.outcome_cb,
               buf_ptr->msg_cmd.outcome_tag_ptr);
      /* the dup now becomes the original, to be freed by SLP, or by HMP
       * if it can't be given to SLP for any reason.
       */
    }
  else
  {
    if (buf_ptr->msg_cmd.outcome_cb != NULL)
    {
      /* Call the originating function's callback to relay the result */
      (buf_ptr->msg_cmd.outcome_cb)( buf_ptr->msg_cmd.outcome_tag_ptr,
                                     status );
      HDR_MSG_PROT_1 (MSG_LEGACY_MED, "calling AC callback w/reason:%d",
                      status);
    }
    else
    {
      HDR_MSG_PROT_1 (MSG_LEGACY_LOW, "No callback for AC msg, status %d",
                      status);
    }
    /* Since we aren't re-sending the message, free it. */
    dsm_free_packet( &buf_ptr->msg_cmd.msg_ptr );
  }
  modem_mem_free( buf_ptr, MODEM_MEM_CLIENT_HDR_CRIT );
  /*lint +esym(613, buf_ptr) buf_ptr is not NULL */
} /* hdrhmp_ac_msg_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_RTC_MSG_CB

DESCRIPTION
  This function is substituted as the SLP callback for RTC messages that can
  also be sent on the AC.  If the callback indicates the error E_CANCELED,
  that means the RTC was lost before the message could be sent, so
  this function will re-send the message.  After performing its processing,
  if any, this callback calls the original callback to pass on the
  outcome of the message transmission.

DEPENDENCIES
  None

PARAMETERS
  data_ptr - The access channel messge those status is being reported
  status - The status of the transmission of the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void hdrhmp_rtc_msg_cb ( void *data_ptr, hdrerrno_enum_type status )
{

  hdrhmp_msg_buf_type *buf_ptr; /* buffer holding RTC message */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (data_ptr != NULL);
  buf_ptr = (hdrhmp_msg_buf_type *) data_ptr;
  /*lint -esym(613, buf_ptr) buf_ptr is not NULL */
  ASSERT ( buf_ptr->msg_cmd.chan == HDRHAI_AC_OR_RTC );

  rex_enter_crit_sect( &hdrhmp.crit_sect ); /* Enter critical section */
  
  /* Remove item from list.  If we re-send, it will be re-queued */
  q_delete( &hdrhmp.rtc_msg_list, &buf_ptr->hdr.link );
  
  if (status == E_CANCELED)
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH,  "AC_OR_RTC msg canceled on RTC, resend");
  }
  else if (status == E_NOT_ALLOWED)
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH,  
      "AC_OR_RTC msg not allowed on RTC, resend");
  }

  if ((status == E_CANCELED) || (status == E_NOT_ALLOWED))
  {
    hdrhmp_app_send_msg(
             buf_ptr->msg_cmd.protocol_name,
             buf_ptr->msg_cmd.stream,
             buf_ptr->msg_cmd.chan,
             buf_ptr->msg_cmd.rtc_is_reliable,
             buf_ptr->msg_cmd.priority,
             buf_ptr->msg_cmd.protocol_instance,
             buf_ptr->msg_cmd.msg_ptr,
             buf_ptr->msg_cmd.outcome_cb,
             buf_ptr->msg_cmd.outcome_tag_ptr);
    /* the dup now becomes the original, to be freed by SLP, or by HMP
     * if it can't be given to SLP for any reason.
     */
  }
  else
  {
    if (buf_ptr->msg_cmd.outcome_cb != NULL)
    {
      /* Call the originating function's callback to relay the result */
      (buf_ptr->msg_cmd.outcome_cb)( buf_ptr->msg_cmd.outcome_tag_ptr,
                                     status );
      HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "calling callback w/reason:%d",status);
    }
    else
    {
      HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "No callback for RTC msg, status %d",
                      status);
    }
    /* Since we aren't re-sending the message, free it. */
    dsm_free_packet( &buf_ptr->msg_cmd.msg_ptr );
  }
  modem_mem_free( buf_ptr, MODEM_MEM_CLIENT_HDR_CRIT );
  /*lint +esym(613, buf_ptr) buf_ptr is not NULL */

  rex_leave_crit_sect( &hdrhmp.crit_sect ); /* Leave critical section */
} /* hdrhmp_rtc_msg_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_QUEUE_PENDING_MSG

DESCRIPTION
  This function queues a message that can't be sent out until a connection
  is open.

DEPENDENCIES
  None

PARAMETERS
  send_msg_cmd_ptr - Pointer to the "Send Message" command for HMP to process
  callback_info_ptr - Callback to store in the pending message queue

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static boolean hdrhmp_queue_pending_msg
(
  hdrhmp_send_msg_params_type * send_msg_cmd_ptr,
  hdrhmp_callback_info_type *callback_info_ptr 
)
{
  hdrhmp_msg_buf_type *buf_ptr;        /* buffer for pending message cmd */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "hdrhmp_queue_pending_msg current cnt %d",q_cnt(&hdrhmp.pending_msg_q));

  /* Get a buffer from the heap */
  if ( ( buf_ptr = ( hdrhmp_msg_buf_type * ) modem_mem_alloc( 
    sizeof( hdrhmp_msg_buf_type ), MODEM_MEM_CLIENT_HDR_CRIT ) ) == NULL )
  {
    ERR_FATAL( "No memory left in heap.", 0, 0, 0 );
  }
  /*lint -esym(613, buf_ptr) buf_ptr is not NULL */

  if (q_cnt(&hdrhmp.pending_msg_q) >= ((HDRHMP_MSG_BUFS_NUM/3)*2))
  {
    HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
        "hdrhmp.pending_msg_q= %d, threshold= %d reached", 
         q_cnt(&hdrhmp.pending_msg_q), ((HDRHMP_MSG_BUFS_NUM/3)*2));
  }

  /* Copy the fields of the command to the queue item buffer */
#ifdef FEATURE_MEMCPY_REMOVAL
    buf_ptr->msg_cmd = *send_msg_cmd_ptr;
#else
  ( void ) memcpy( &buf_ptr->msg_cmd, send_msg_cmd_ptr,
       sizeof( hdrhmp_send_msg_params_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* Store the callback.  
   * If the message was put onto a HMP message list (e.g RASM_LIST),
   * this is the callback for that list(e.g rasm_cb).
   * Otherwise it is the senders cb.
   */
  buf_ptr->msg_cmd.outcome_cb = callback_info_ptr->outcome_cb;
  buf_ptr->msg_cmd.outcome_tag_ptr = callback_info_ptr->outcome_tag_ptr;

  ( void ) q_link( buf_ptr, &buf_ptr->hdr.link );
  q_put( &hdrhmp.pending_msg_q, &buf_ptr->hdr.link );
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "hdrhmp_queue_pending_msg updated cnt %d",q_cnt(&hdrhmp.pending_msg_q));
  return FALSE;
  /*lint +esym(613, buf_ptr) buf_ptr is not NULL */
} /* hdrhmp_queue_pending_msg() */

/*===========================================================================

FUNCTION HDRHMP_ADD_TO_MSG_LIST

DESCRIPTION
  This function adds a message that has been sent on the access channel to
  a list for future use if an access handoff occurs.

DEPENDENCIES
  None

PARAMETERS
  msg_retry_list - List message is to be added to
  callback_info_ptr - Callback to store and repalce with new HMP callback
  msg_cmd_ptr - Pointer to the "Send Message" command given to HMP.

RETURN VALUE
  TRUE if the message is added to the list
  FALSE if there are no buffers available in the list
  
SIDE EFFECTS
  None

===========================================================================*/

static boolean hdrhmp_add_to_msg_list
(
  hdrhmp_retry_list_enum_type  msg_retry_list,
  hdrhmp_callback_info_type *callback_info_ptr, /*in/out param */
  hdrhmp_send_msg_params_type *msg_cmd_ptr
)
{
  void *message_list_entry_ptr; /* Generic entry from message list */
  hdrhmp_msg_buf_type *buf_ptr; /* buffer for pending message cmd */
  hdrhmp_rasm_buf_type *rasm_ptr = NULL; /* buffer holding RASM message */
  dsm_item_type *dup_pkt_ptr; /* Duplicate of AC message */
  q_type *msg_list_ptr; /* AC or RTC msg list */
  hdrslp_outcome_cb_type outcome_cb; /* newest intercepting callback */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* A NULL message pointer is for RUP registration, which should not be
   * added to the list.
   */
  ASSERT (msg_cmd_ptr->msg_ptr != NULL);  

  /* Set the list and free queue based on channel message is going out on */
  if (msg_retry_list == HDRHMP_AC_MSG_LIST)
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "Add to AC msg list %d",
                  hdrhmp.ac_msg_list.cnt+1);
    msg_list_ptr = &hdrhmp.ac_msg_list;
    outcome_cb = hdrhmp_ac_msg_cb;
  }
  else if (msg_retry_list == HDRHMP_RTC_MSG_LIST)
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "Add to RTC msg list %d",
                    hdrhmp.rtc_msg_list.cnt+1);
    msg_list_ptr = &hdrhmp.rtc_msg_list;
    outcome_cb = hdrhmp_rtc_msg_cb;
  }
  else if ( msg_retry_list == HDRHMP_RASM_LIST )
  {
    msg_list_ptr = &hdrhmp.rasm_list;
    outcome_cb = hdrhmp_rasm_cb;
  }
  else
  {
    /* Avoid compiler warnings */
    msg_list_ptr = NULL;
    outcome_cb = NULL;
    ERR_FATAL("Unknown msg_retry_list!",0,0,0);
  }


  /* Get a buffer from the heap */
    if ( msg_retry_list == HDRHMP_RASM_LIST )
  {
    message_list_entry_ptr =  modem_mem_alloc( sizeof( hdrhmp_rasm_buf_type ),
                                                MODEM_MEM_CLIENT_HDR_CRIT );
  }
  else
  {
    message_list_entry_ptr = modem_mem_alloc( sizeof( hdrhmp_msg_buf_type ),
                                                MODEM_MEM_CLIENT_HDR_CRIT );
  }
   
  if( message_list_entry_ptr == NULL ) 
  {
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR,  "No bufs on msg list %d", 
                    msg_retry_list );
    return FALSE;
  } 
   
  if ( msg_retry_list == HDRHMP_RASM_LIST )
  {
    rasm_ptr = (hdrhmp_rasm_buf_type *) message_list_entry_ptr;

    /* Initialize the RASM entry */
    rex_def_timer_ex( &rasm_ptr->overall_timer,
                      hdrhmp_timer_cb,
                      HDRHMP_RASM_OVERALL_TIMER );

    rex_def_timer_ex( &rasm_ptr->interval_timer,
                      hdrhmp_timer_cb,
                      HDRHMP_RASM_INTERVAL_TIMER );

    buf_ptr = &rasm_ptr->buf;
  }
  else
  {
    buf_ptr = (hdrhmp_msg_buf_type *) message_list_entry_ptr;
  }

  /*lint -esym(613, buf_ptr) buf_ptr is not NULL */

  /* Copy the fields of the command to the list item buffer */
#ifdef FEATURE_MEMCPY_REMOVAL
    buf_ptr->msg_cmd = *msg_cmd_ptr;
#else
  ( void ) memcpy( &buf_ptr->msg_cmd, msg_cmd_ptr,
       sizeof( hdrhmp_send_msg_params_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */
  (void) dsm_dup_packet( &dup_pkt_ptr, msg_cmd_ptr->msg_ptr,
                         0, dsm_length_packet (msg_cmd_ptr->msg_ptr));

  if ( dup_pkt_ptr == NULL )
  {
    ERR_FATAL("No more duplicate dsm items available!",0,0,0);
  }

  if ( msg_retry_list == HDRHMP_RASM_LIST )
  {
    hdrhmp_rasm_initialize_entry( rasm_ptr );

    if ( hdrhmp.rasm_list.cnt == 0)
    {
      /* if rasm list is empty, it's the first rasm msg. Reset the log_id to 1 */
      hdrhmp.rasm_log_id = 1;
    }
    else
    {
      hdrhmp.rasm_log_id++;
    }
    rasm_ptr->log_id = hdrhmp.rasm_log_id;

    HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                    "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                    (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                    (uint32) HDRHMP_RASMS_WAITING | (uint32) HDRHMP_RASMSS_NONE);

    rasm_ptr->state = HDRHMP_RASMS_WAITING;
    rasm_ptr->substate = HDRHMP_RASMSS_NONE;

    if (buf_ptr->msg_cmd.rasm.overall_timeout > 0)
    {
      (void) rex_set_timer( &rasm_ptr->overall_timer, 
                            buf_ptr->msg_cmd.rasm.overall_timeout );
    }

    /* For RASM retransmits, the send_msg_cmd itself is the only thing
     * passed back to hmp_send_msg(), so this pointer needs to be in it.
     */
    buf_ptr->msg_cmd.rasm.self_ptr = message_list_entry_ptr;
  }

  /* We store the dup of the message in the list and pass the original
   * to SLP.
   */
  buf_ptr->msg_cmd.msg_ptr = dup_pkt_ptr;

  /* We also need to intercept the callback.  For HMP message list 
   * callbacks, the outcome_tag_ptr is the pointer to the list entry */

  buf_ptr->msg_cmd.outcome_cb = callback_info_ptr->outcome_cb;
  buf_ptr->msg_cmd.outcome_tag_ptr = callback_info_ptr->outcome_tag_ptr;
  callback_info_ptr->outcome_cb = outcome_cb;
  callback_info_ptr->outcome_tag_ptr = message_list_entry_ptr;

  ( void ) q_link( buf_ptr, &buf_ptr->hdr.link );
  q_put( msg_list_ptr, &buf_ptr->hdr.link );

  return TRUE;

} /* hdrhmp_add_to_msg_list() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_MSG_IS_SENDABLE

DESCRIPTION
  This function determines if a message can be sent or not

DEPENDENCIES
  None

PARAMETERS
  send_msg_cmd_ptr - Pointer to the "Send Message" command
  msg_id - For trace messaging, the messge ID of the message being checked
  errno - Used to determine action to take for a message queued up to HMP

RETURN VALUE
  TRUE if the message can be sent, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean hdrhmp_msg_is_sendable
(
  hdrhmp_send_msg_params_type * send_msg_cmd_ptr,
  uint8 msg_id, /* for trace only */
  hdrerrno_enum_type *errno_ptr
)
{
  boolean msg_is_sendable = TRUE;  /* Flag if message can be sent */
#ifdef FEATURE_LTE_TO_HDR_OH
  /* If tunnel mode is enabled */
  boolean tunnel_mode_is_enabled = hdrsap_tunnel_mode_is_enabled(HDRHAI_HDR_MESSAGING_PROTOCOL);
#endif /* FEATURE_LTE_TO_HDR_OH*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *errno_ptr = E_FAILURE;

  #if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
  if (!hdrsmp_is_to_maintain_evdo_session())
  {
    HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
                   "DCM: ignore hmp_send_msg when on 1x, p0x%x m0x%x",
                   send_msg_cmd_ptr->protocol_name, msg_id);
    msg_is_sendable = FALSE;
  }
  else 
  #endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */
  /* If the AMP is Inactive, then we do not have a transmit ATI and the
   * message cannot be sent.
   */
  if( hdramps_get_state() == HDRAMP_INACTIVE )
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH,  "Msg queued when AMP Inactive!");
    msg_is_sendable = FALSE;
  }

  if ( !hdralmp_system_is_acquired( HDRHAI_HDR_MESSAGING_PROTOCOL )
#ifdef FEATURE_LTE_TO_HDR_OH
     && ( !tunnel_mode_is_enabled )
    /* Tunnel mode is off. */
#endif /* FEATURE_LTE_TO_HDR_OH */
     )
  {
    HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "Msg Qd for xmit before acq p0x%x m0x%x",
      send_msg_cmd_ptr->protocol_name, msg_id);
    msg_is_sendable = FALSE;
    *errno_ptr = E_CANCELED; 
  }
#ifdef FEATURE_LTE_TO_HDR_OH
  /* Do not send message in tunnel mode if pre-registration is not allowed. */
  else if ( tunnel_mode_is_enabled &&  
            !hdrovhd_prereg_is_allowed( HDRHAI_HDR_MESSAGING_PROTOCOL ) ) 
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "Msg Qd for xmit but prereg is not allowed p0x%x m0x%x",
                    send_msg_cmd_ptr->protocol_name, msg_id );
    HDRTRACE_TXMSG_DROPPED( send_msg_cmd_ptr->protocol_name, msg_id );
    msg_is_sendable = FALSE;
  }
  /* ConnectionRequest should be the only msg sendable during active handoff */
  else if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() &&
            !(( send_msg_cmd_ptr->protocol_name == HDRHAI_IDLE_STATE_PROTOCOL ) &&
              ( msg_id == 1 )) )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "active handoff in progress, drop p0x%x m0x%x",
                    send_msg_cmd_ptr->protocol_name, msg_id );
    HDRTRACE_TXMSG_DROPPED( send_msg_cmd_ptr->protocol_name, msg_id );
    msg_is_sendable = FALSE;
  }
  /* We are not expected to send any message while L2D idle handoff/CGI/RTT is
   * in progress except for UATIrequest. */
  else if ( (hdrmc_hdr_to_lte_time_transfer_is_in_progress() ||
             hdrmc_lte_to_hdr_get_cgi_is_in_progress() ||
             hdrmc_lte_to_hdr_redir_is_in_progress() ||
             hdrmc_lte_to_hdr_resel_is_in_progress()) && 
            !(( send_msg_cmd_ptr->protocol_name == HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) &&
              ( msg_id == 0 )) )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "Idle handoff/CGI/RTT in progress, drop p0x%x m0x%x",
                    send_msg_cmd_ptr->protocol_name, msg_id );
    HDRTRACE_TXMSG_DROPPED( send_msg_cmd_ptr->protocol_name, msg_id );
    msg_is_sendable = FALSE;
  }
  /* If SAP is in OPEN state, there is a connection opened.
   * The message is considered not sendable on AC except for SAP msg.
   */
  else if ( tunnel_mode_is_enabled &&  
            hdrsaps_get_state() == HDRSAPS_OPEN &&
            send_msg_cmd_ptr->chan == HDRHAI_AC &&
            send_msg_cmd_ptr->protocol_name != HDRHAI_SIGNALING_ADAPTATION_PROTOCOL
          ) 
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "HDR SAP open, drop p0x%x m0x%x",
                    send_msg_cmd_ptr->protocol_name, msg_id );
    HDRTRACE_TXMSG_DROPPED (send_msg_cmd_ptr->protocol_name, msg_id);
    msg_is_sendable = FALSE;
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* If the message is on the AC but we have a connection open, 
   * the message is considered not sendable on AC 
   */
  else if ( hdrrup_is_tc_assigned( HDRHAI_HDR_MESSAGING_PROTOCOL ) &&
            ( send_msg_cmd_ptr->chan == HDRHAI_AC ) 
          )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "HDR in Traffic State, drop p0x%x m0x%x",
                    send_msg_cmd_ptr->protocol_name, msg_id );
    HDRTRACE_TXMSG_DROPPED (send_msg_cmd_ptr->protocol_name, msg_id);
    msg_is_sendable = FALSE;
  }
  /* If the message is for the AC but we are in the process setting up the 
   * connection, only allow ConnectionRequest to go thru. No protocol should 
   * need to send someting on the AC during connection setup, but if one tried
   * it would be wrong to activate the ACMAC while the RTCMAC is active.
   * Update: Due to the requirement in 8.7.6.1.5.4, the AT can't send the AC
   * message even while waiting for the TCA.
   */
  else if ( hdridle_is_connecting() &&
            ( send_msg_cmd_ptr->chan == HDRHAI_AC ) &&
            ! ( ( send_msg_cmd_ptr->protocol_name == HDRHAI_IDLE_STATE_PROTOCOL ) &&
                ( msg_id == 1)  /* HDRIDLE_CONN_REQ_ID */
              ) 
          )
  {
    /* The AT is trying to open a connection.  Any other messages required to
     * be bundled with the connection request need to have been queued to SLP
     * before IDLE state enters connection setup state.  This way the only 
     * message that needs to be permitted during this state is the 
     * ConnectionRequest.
     */
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "HDR in Conn Setup State, drop p0x%x m0x%x",
                    send_msg_cmd_ptr->protocol_name, msg_id );
    HDRTRACE_TXMSG_DROPPED (send_msg_cmd_ptr->protocol_name, msg_id);
    msg_is_sendable = FALSE;
  }

  /* If the connection is open but the RTCMAC has already been deactivated,
   * we can't really send this message, so reject it.
   * This is a race condition that should not really happen.
   */
  else if ( ( hdralmp_connection_is_open( HDRHAI_HDR_MESSAGING_PROTOCOL ) ) &&
            ( !hdrrup_is_tc_assigned( HDRHAI_HDR_MESSAGING_PROTOCOL ) ) &&
            ( send_msg_cmd_ptr->chan == HDRHAI_RTC ) )
  {
    ERR( "RTC msg Qd after close p0x%x m0x%x",
      send_msg_cmd_ptr->protocol_name, msg_id, 0 );
    /* Priority dependency - RTCMAC is in a higher priority task than HMP */
    msg_is_sendable = FALSE;
  }

  /* If the message is for the RTC but no session is open we will not
   * be able to open the connection in order to send it.
   */
  else if ( ( !hdrsmp_session_is_open( HDRHAI_HDR_MESSAGING_PROTOCOL ) &&
              !hdrsmp_session_is_closing( HDRHAI_HDR_MESSAGING_PROTOCOL) ) &&
            ( send_msg_cmd_ptr->chan == HDRHAI_RTC ) )
  {
    HDR_MSG_PROT_2 (MSG_LEGACY_HIGH,  "RTC msg Qd no session p0x%x m0x%x",
      send_msg_cmd_ptr->protocol_name, msg_id);
    /* Priority dependency - RTCMAC is in a higher priority task than HMP */
    msg_is_sendable = FALSE;
  }
  /* For some specific RTC only messages, we should not bother to open a
     connection just to send a message on the RTC (e.g. ConnectionClose message,
     RouteUpdate for pilot report, TCC ) */
#ifdef FEATURE_LTE_TO_HDR_OH
  else if ( tunnel_mode_is_enabled &&
            hdrsaps_get_state() != HDRSAPS_OPEN &&
            send_msg_cmd_ptr->chan == HDRHAI_RTC &&
            send_msg_cmd_ptr->send_on_curr_rtc_only  
          ) 
  {
    HDRTRACE_TXMSG_DROPPED (send_msg_cmd_ptr->protocol_name, msg_id);
    HDR_MSG_PROT_2 (MSG_LEGACY_MED, "RTC msg Qd conn already closed p0x%x m0x%x",
      send_msg_cmd_ptr->protocol_name, msg_id);
    msg_is_sendable = FALSE;
  }
  else if ( !tunnel_mode_is_enabled &&
#else
  else if ( 
#endif /* FEATURE_LTE_TO_HDR_OH */
            !( hdralmp_connection_is_open( HDRHAI_HDR_MESSAGING_PROTOCOL) ) &&
            ( send_msg_cmd_ptr->chan == HDRHAI_RTC ) &&
            ( send_msg_cmd_ptr->send_on_curr_rtc_only ) )
  {
    HDRTRACE_TXMSG_DROPPED (send_msg_cmd_ptr->protocol_name, msg_id);
    HDR_MSG_PROT_2 (MSG_LEGACY_MED, "RTC msg Qd conn already closed p0x%x m0x%x",
      send_msg_cmd_ptr->protocol_name, msg_id);
    msg_is_sendable = FALSE;
  }

  else if (
#ifdef FEATURE_LTE_TO_HDR_OH
           !tunnel_mode_is_enabled &&
#endif /* FEATURE_LTE_TO_HDR_OH */
           (send_msg_cmd_ptr->protocol_name == HDRHAI_IDLE_STATE_PROTOCOL) &&
           (msg_id == 1) /* HDRIDLE_CONN_REQ_ID */ &&
           (!hdridle_is_connection_needed()))
  {
    /* This is to account for the sequence HDRMC_ORIGINATION_F followed by
     * HDRMC_END_F, before the ConnectionRequest message is sent.  Because the 
     * event queue is always checked before the CM command queue, this is 
     * actually not possible, but leaving this guards againsted future bugs.
     */
     
    HDR_MSG_PROT (MSG_LEGACY_ERROR,  "ConnRequest message no longer needed");
    msg_is_sendable = FALSE;
  }
  else if ((msg_id == 0x52) && hdrscp_config_is_locked())
  {
    /* When Config is locked AT is not allowed to send Configuration messages.
     * Here we only check for AUR message and not for ConfigRequest messages 
     * because IDLE is the only protocol that can start a AT initiated Config
     * which we already check in SCP and not allow a AT Initiated negotiation 
     * when config is locked.
     */
    HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, "Config is Locked dropping p0x%x m0x%x",
                    send_msg_cmd_ptr->protocol_name, msg_id);
    msg_is_sendable = FALSE;
    *errno_ptr = E_NOT_AVAILABLE; 
  }
  else if (hdridle_is_suspended() && (send_msg_cmd_ptr->chan == HDRHAI_RTC))
  {
    HDRTRACE_TXMSG_DROPPED (send_msg_cmd_ptr->protocol_name, msg_id);
    msg_is_sendable = FALSE;

    /* While HDR is suspended if any of the protocol tries to send a RTC 
     * only message, the message will be held in HMP pending queue until
     * the connection could be opened. However in suspended state the 
     * event to open the connection would have been dropped, so the
     * message would dangle in the pending queue, for this reason we 
     * immediately fail the message.
     */
    HDR_MSG_PROT_2 (MSG_LEGACY_MED, 
      "Do not try to send RTC msg while IDLE is in suspended state p0x%x m0x%x",
      send_msg_cmd_ptr->protocol_name, msg_id);
  }

  return (msg_is_sendable);

} /* hdrhmp_msg_is_sendable */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_FREE_MSG

DESCRIPTION
  This function aborts a message given to HMP for transmission before it
  is ever given to SLP.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr           - The message to be freed
  callback_info_ptr - Callback to be called
  failure_reason    - Reason to give the sending protocol in the callback

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrhmp_free_msg
(
  dsm_item_type *msg_ptr,
  const hdrhmp_callback_info_type *callback_info_ptr,
  errno_enum_type failure_reason
)
{
  /* When discarding a send_msg_cmd without passing the msg_ptr on to SLP,
   * it is HMP's responsibility to free the DSM item allocated by the
   * sending protocol.
   */
  dsm_free_packet( &msg_ptr );

  if ((callback_info_ptr != NULL) && 
      (callback_info_ptr->outcome_cb != NULL ))
  {
    /* Call the callback.  If the message was put onto a HMP message list,
     * this is the callback for that list.  Otherwise it is the senders cb.
     */
    ( *callback_info_ptr->outcome_cb )
      ( callback_info_ptr->outcome_tag_ptr, failure_reason );
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "freeing msg w/reason:%d",
                    failure_reason);
  }
  else
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "no cb for msg w/reason:%d",
                    failure_reason);
  }
} /* hdrhmp_free_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_AUTO_RESEND_ENABLED

DESCRIPTION
  This function determines of the sending protocol wishes for their message
  to be automatically retransmitted if the transmission attempt is aborted
  by SLP.

DEPENDENCIES
  None

PARAMETERS
  send_msg_cmd_ptr - Pointer to the "Send Message" command
  tx_chan          - Channel to send message out on
  msg_id           - The message ID of the message being sent

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static boolean hdrhmp_auto_resend_enabled
(
  hdrhmp_send_msg_params_type * send_msg_cmd_ptr,
  hdrhai_channel_enum_type tx_chan,
  uint8 msg_id
)
{
  boolean resend_enabled = TRUE; /* if msg should be re-sent automatically */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( tx_chan == HDRHAI_AC )
  {
    if ( send_msg_cmd_ptr->msg_ptr == NULL )
    {
      /* A NULL msg_ptr on the AC is a RUP registration, but should not be
       * re-transmitted because RUP will re-register for itself, if needed.
       */
      resend_enabled = FALSE;
    }
    else if (( send_msg_cmd_ptr->protocol_name ==
               HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) &&
             ( msg_id == 0 )) /* HDRAMP_UATI_REQ_MSG */
    {
      /* Do not automatically re-send the UATI Request.  Following an access
       * handoff AMP will always get the HDRIND_OVHD_INFO_UPDATED indication,
       * and will re-send the UATI request if necessary.
       */
      resend_enabled = FALSE;
    }
    else if (( send_msg_cmd_ptr->protocol_name ==
               HDRHAI_IDLE_STATE_PROTOCOL ) &&
             ( msg_id == 1 )) /* HDRIDLE_CONN_REQ_ID */
    {
      /* Do not automatically re-send the ConnectionRequest as it may have
       * been aborted
       */
      resend_enabled = FALSE;
    }
#ifdef FEATURE_LTE_TO_HDR_OH
    else if ( send_msg_cmd_ptr->protocol_name ==
              HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) /* SAP msg */
    {
      /* Do not automatically re-send the SAP msg as it has own re-send
       * mechanism.
       */
      resend_enabled = FALSE;
    }
#endif /* FEATURE_LTE_TO_HDR_OH */
  }
  else if ( send_msg_cmd_ptr->chan == HDRHAI_RTC )
  {
    /* We never do auto-resend on messages that are only for the RTC. */
    resend_enabled = FALSE;
  }

  return (resend_enabled);
} /* hdrhmp_auto_resend_enabled() */

/*===========================================================================

FUNCTION HDRHMP_FAIL_NO_MSG_LIST_BUFS

DESCRIPTION
  This function gracefully recovers by forcing a system lost if we run 
  out of buffers.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrhmp_fail_no_msg_list_bufs (void)
{
  sd_ss_act_e_type sd_action;/* Acquire HDR, acquire 1x or UMTS, etc. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!hdralmp_system_is_acquired(HDRHAI_HDR_MESSAGING_PROTOCOL) ||
      hdrhmp.buf_full_reacq_pending)
  {
    /* Only report sys loss once and only when HDR is acquired */
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Not acquired or sys loss already reported during RASM");
    return;
  }

  /* We could not queue the buffer on the ac_msg_list, rtc_msg_list,
   * or rasm_list, which means we have more messages pending transmission 
   * than we can handle.  Gracefully recover by forcing re-acquisition.
   */
  sd_action = sd_ss_ind_hdr_opr_sys_lost( NULL );
  HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "RASM Q full, SD Action %d", sd_action);

  ASSERT(sd_action != SD_SS_ACT_CONTINUE);

  hdrhmp.buf_full_reacq_pending = TRUE;

  if ( sd_action == SD_SS_ACT_ACQ_HDR )
  {
    hdralmp_reacquire( HDRHAI_ABORT_UNSPECIFIED,
                       HDRHAI_HDR_MESSAGING_PROTOCOL);
  }
  else
  {
    hdralmp_deactivate( HDRHAI_ABORT_UNSPECIFIED, FALSE, 0,
                        HDRHAI_HDR_MESSAGING_PROTOCOL);
  }
} /* hdrhmp_fail_no_msg_list_bufs() */

/*===========================================================================

FUNCTION HDRHMP_PROCESS_SEND_MSG_CMD

DESCRIPTION
  This function processes the "Send Message" command given to HMP.  The
  message may be a) passed to SLP, b) freed because something prevents it
  from being given to SLP, c) put on the hmp_pending_msg_q until a TC is
  established, d) passed to SLP and duped into the ac_msg_list, or
  e) passed to SLP and duped into the rtc_msg_list.

  a) If the message is successfully sent to SLP, it is the responsibility of
     the lower layers to free the DSM item.

  b) In the event of an error preventing the message from being sent to SLP,
     the DSM item pointer is freed immediately.

  c) If the message is put on the hmp_pending_msg_q, it is either passed back
     to this function when the traffic channel is acquired, or freed if the
     channel can not be acquired.

  d) Messages to be sent on the access channel are duped and put on the
     ac_msg_list in case they need to be re-sent after an access handoff,
     or in case they can be re-sent on the RTC after acquisition.
     When the message outcome callback (hdrhmp_ac_msg_cb) is called by
     SLP, the callback sends the dup back to this function to be re-sent
     if the callback reason indicates an access handoff occurred or that the
     specified channel was not available and RTC can be tried.  Otherwise the
     callback frees the pointer.

  e) Messages to be sent on the reverse traffic channel that are also allowed
     to be sent on the access channel are duped and put on the rtc_msg_list
     in case the RTC is lost and we want to send the message again on the AC.
     When the message outcome callback (hdrhmp_rtc_msg_cb) is called by
     SLP, the callback sends the dup back to this function to be re-sent
     if the callback reason indicates thespecified channel was not available
     and the AC can be tried.  Otherwise the callback frees the pointer.

DEPENDENCIES
  None

PARAMETERS
  send_msg_cmd_ptr - Pointer to the "Send Message" command for HMP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrhmp_process_send_msg_cmd
(
  hdrhmp_send_msg_params_type * send_msg_cmd_ptr
)
{
  hdrerrno_enum_type slp_errno;
  hdrerrno_enum_type hmp_errno;
  uint8 msg_id; /* for trace only */
  /* Address where AC message is stored */
  hdrhai_channel_enum_type tx_chan; /* Channel to send message out on */
  boolean tx_is_reliable; /* is_reliable value to give to SLP with message */
  uint8 protocol_id; /* Value of HMP header */
  boolean auto_resend_is_enabled;/* Flag if msg added to ac or rtc msg lst */
  hdrhmp_callback_info_type intercepting_callback;
  hdrhmp_rasm_buf_type *rasm_ptr = NULL; /* buffer holding RASM message */
  hdrhmp_msg_buf_type *buf_ptr; /* convenience ptr to wrapped buffer */
  boolean out_of_buffers = FALSE; /* If we ran out of msg_list buffers */
  boolean start_access_attempt_now = TRUE; /* start now or wait for hdralmp_open_conn() */ 
  boolean transmit_rasm_is_needed = FALSE;   /* If rasm msg is still needed to be transmitted */
#ifdef FEATURE_LTE_TO_HDR_OH
  /* If tunnel mode is enabled */
  boolean tunnel_mode_is_enabled = hdrsap_tunnel_mode_is_enabled(HDRHAI_HDR_MESSAGING_PROTOCOL);
#endif /* FEATURE_LTE_TO_HDR_OH*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT ((( send_msg_cmd_ptr->chan == HDRHAI_AC_OR_RTC ) ||
          ( send_msg_cmd_ptr->chan == HDRHAI_AC ) ||
          ( send_msg_cmd_ptr->chan == HDRHAI_RTC )) &&
    /* NULL message pointer only allowed on access channel */
          (( send_msg_cmd_ptr->chan == HDRHAI_AC ) ||
           ( send_msg_cmd_ptr->msg_ptr != NULL )));

  /* Extract the msg_id for debug purposes only if we fail to extract the
   * msg_id, the only consequence is an incorrect debug message.
   */
  if (send_msg_cmd_ptr->msg_ptr != NULL)
  {
    (void) dsmbit_unpack8( send_msg_cmd_ptr->msg_ptr, &msg_id, 0, 8 );
  }
  else
  {
    ASSERT( send_msg_cmd_ptr->protocol_name == HDRHAI_ROUTE_UPDATE_PROTOCOL);
    msg_id = 0; /* This is a Route Update message */
  }

  /* In some cases HMP chooses to intercept the message callback rather 
   * than letting SLP call the sender's callback directly.  This allows HMP 
   * to resend the message on behalf of the sending protocol without 
   * duplicating code. For example: access handoff, AC_OR_RTC, and RASM.
   */
  intercepting_callback.outcome_cb = send_msg_cmd_ptr->outcome_cb;
  intercepting_callback.outcome_tag_ptr = 
    send_msg_cmd_ptr->outcome_tag_ptr;

  if (send_msg_cmd_ptr->rasm.is_enabled)
  {
    transmit_rasm_is_needed = TRUE;

    if (send_msg_cmd_ptr->rasm.is_rexmit)
    {
      /* Don't add retransmit to rasm_list because it's already in the list. */

      /* In the case of RASM retransmits, the intercepting_callback is rasm_cb,
       * and the original sender's callback is being kept in the rasm_list.
       * So all that needs to be done here is to change the incercepting
       * callback to be RASM.  However, the only way to know the correct
       * outcome_tag_ptr (which is the pointer to the entry in the rasm_list)
       * was to have stored it inside the send_msg_cmd itself at the time
       * the command was put on the rasm_list.  Yuck.
       */
      rasm_ptr = (hdrhmp_rasm_buf_type *) send_msg_cmd_ptr->rasm.self_ptr;
      buf_ptr = &rasm_ptr->buf;

      /* Need to duplicate packet during rexmit because we don't call hdrhmp_add_to_msg_list 
       * but lower layer (MAC) will free  packet after tx
       */
      (void) dsm_dup_packet( &buf_ptr->msg_cmd.msg_ptr, 
                       send_msg_cmd_ptr->msg_ptr,
                       0, dsm_length_packet (send_msg_cmd_ptr->msg_ptr));
      HDRDSM_TOUCH_ITEM(send_msg_cmd_ptr->msg_ptr); /* DSM_TOUCH_ITEM to track that DSM item has come till here */
      intercepting_callback.outcome_cb = hdrhmp_rasm_cb;
      intercepting_callback.outcome_tag_ptr = send_msg_cmd_ptr->rasm.self_ptr;

      if (rasm_ptr->state != HDRHMP_RASMS_DONE)
      {
        ASSERT ((rasm_ptr->state == HDRHMP_RASMS_WAITING) &&
                (rasm_ptr->substate == HDRHMP_RASMSS_WAITING_HMP_REXMIT));

        HDR_MSG_PROT_6 (MSG_LEGACY_HIGH, 
                        "RASM %d TX: Retransmit (P 0x%x, ID 0x%x) S %x (attempt %d of %d)", 
                        rasm_ptr->log_id,
                        send_msg_cmd_ptr->protocol_name, msg_id, 
                        (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                        rasm_ptr->attempts + 1,
                        buf_ptr->msg_cmd.rasm.max_retries + 1);
      }
      else
      {
        /* Msg has moved to DONE state for one of the following reasons, 
         * and we called user cb already.
         *   - L3Acked
         *   - overall timer expired
         *   - hdrhmp_clear_rasm_queue() was called (e.g. session clsoe)
         */
        transmit_rasm_is_needed = FALSE;
      }
    }
    else
    {
      /* Message is designated for app layer retry.  Keep it in the RASM_LIST
       * so it can be resent if necessary.
       */
      out_of_buffers = !hdrhmp_add_to_msg_list( HDRHMP_RASM_LIST, 
                                                &intercepting_callback,
                                                send_msg_cmd_ptr );

      if (!out_of_buffers)
      {
        rasm_ptr = (hdrhmp_rasm_buf_type *) intercepting_callback.outcome_tag_ptr;
        buf_ptr = &rasm_ptr->buf;

        HDR_MSG_PROT_6 (MSG_LEGACY_HIGH, 
                        "RASM %d TX: New (P 0x%x, ID 0x%x) S %x (attempt %d of %d)", 
                        rasm_ptr->log_id,
                        send_msg_cmd_ptr->protocol_name, msg_id, 
                        (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                        rasm_ptr->attempts + 1,
                        buf_ptr->msg_cmd.rasm.max_retries + 1);
      }
    }
  }

  if (out_of_buffers)
  {
    hdrhmp_free_msg( send_msg_cmd_ptr->msg_ptr, 
                     &intercepting_callback, E_PROT_Q_FULL );
    /* Before we force system lost to recover no buf on msg list case, we also 
     * need to clear RASM queue, otherwise messages are still in RASM queue
     * and will be retransmitted after re-acq 
     */
    hdrhmp_clear_rasm_queue();
    hdrhmp_fail_no_msg_list_bufs();
    return;
  }

  if (send_msg_cmd_ptr->rasm.is_enabled && !transmit_rasm_is_needed)
  {
    dsm_free_packet(&send_msg_cmd_ptr->msg_ptr);
    /* RASM msg is already in DONE state, call rasm cb to remove it from rasm list*/
    ( *intercepting_callback.outcome_cb )
      ( intercepting_callback.outcome_tag_ptr, E_CANCELED );
    return;
  }

  if ( !hdrhmp_msg_is_sendable( send_msg_cmd_ptr, msg_id, &hmp_errno ) ) 
  {
    hdrhmp_free_msg( send_msg_cmd_ptr->msg_ptr, 
                     &intercepting_callback, hmp_errno );
    return;
  }

  /* We need to determine what channel to send this message on. */
  if (send_msg_cmd_ptr->chan == HDRHAI_AC_OR_RTC)
  {
    /* Because section 8.7.6.1.5.4 says, "The access terminal shall not send a 
     * RouteUpdate message if the state timer of the Connection Setup State in 
     * the Idle State Protocol is active," we can't send the message on the AC
     * if IDLE is in connection setup state. An exception here is the UATI
     * Complete message, because in certain situation, for example, a data
     * origination during the inter-RNC idle handoff, AT will bundle the CR+
     * UatiRequest message. If AN responds with the UatiAssignment message, 
     * we don't want to delay sending UatiComplete message to AN until the 
     * Connection Setup timer expires. So we allow UatiComplete message
     * to be sent on Access channel during Connection Setup state as an 
     * enhancement.
     */

#ifdef FEATURE_LTE_TO_HDR_OH
    if ( tunnel_mode_is_enabled )
    {          
      if ( (hdrsaps_get_state() == HDRSAPS_OPEN) ||
           ( (hdrsaps_get_state() == HDRSAPS_SETUP)  &&
             /* UATIComplete */
             !(((send_msg_cmd_ptr->protocol_name == 
                 HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL) && 
                (msg_id == 0x02)) ||
             /* Session Close message */
               ((send_msg_cmd_ptr->protocol_name ==
                 HDRHAI_SESSION_MANAGEMENT_PROTOCOL) &&
                (msg_id == 0x01))
              )
           )
         )
      {
        tx_chan = HDRHAI_RTC;
      }
      else
      {
        tx_chan = HDRHAI_AC;
      }
    }
    else
#endif /* FEATURE_LTE_TO_HDR_OH */
    if ( hdrrup_is_tc_assigned( HDRHAI_HDR_MESSAGING_PROTOCOL ) ||
         /* ConnSetup and not sending UATIComplete/SessClose */
         ( hdridle_is_connecting() && 
           !(
              /* UATI Complete message */
             ((send_msg_cmd_ptr->protocol_name ==
               HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL) && 
              (msg_id == 0x02)) || 
              /* Session Close message */
             ((send_msg_cmd_ptr->protocol_name ==
               HDRHAI_SESSION_MANAGEMENT_PROTOCOL) &&
              (msg_id == 0x01))
            )
          )
        )
    {
      if (!hdrrup_is_tc_assigned( HDRHAI_HDR_MESSAGING_PROTOCOL ))         
      {
        HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                     "Allow UATIComp/SessClose on AC while Conn setup");
      }
      tx_chan = HDRHAI_RTC;
    }
    else
    {
      tx_chan = HDRHAI_AC;
    }
  }
  else
  {
    tx_chan = send_msg_cmd_ptr->chan;
  }

  if((tx_chan == HDRHAI_AC) && (send_msg_cmd_ptr->rasm.is_enabled) &&
     (send_msg_cmd_ptr->rasm.is_implicit_on_connection_setup))
  {
    /* We are resuming QoS, KKIdleState == 2, and we are opening a connection, 
     * so the ReservationOnRequest is implicit and should not be included in   
     * the access attempt.  Note that bundle flag is either WITH_CR or WITH_CR_DOS, 
     * so even without including the message the connection setup will be started.     
     */                                                                        
    dsm_free_packet( &send_msg_cmd_ptr->msg_ptr ); 

    /* Need to set NULL message ptr(RoR for KKIdleState == 2) to lower priority 
     * than ConnectionRequest message(priority 40), otherwise when PCP tries to get msg 
     * from slp tx queue, instead of bunding NULL msg with ConnectionRequest msg,
     * it will send out RUP(+ NULL msg) immediately */
    send_msg_cmd_ptr->priority = 60;
  }

  /* AC messages on best effort by default */
  tx_is_reliable = ( tx_chan == HDRHAI_AC ) ?
    FALSE : send_msg_cmd_ptr->rtc_is_reliable;

  /* To avoid a corner race condition where the ALMP connection was open for a
  RUP message when hdrhmp_msg_is_sendable() was called but connection got 
  closed before the message was queued to SLP, we check if the message is to be 
  sent on current rtc only  */
  if(!( hdralmp_connection_is_open( HDRHAI_HDR_MESSAGING_PROTOCOL) ) &&
      ( send_msg_cmd_ptr->chan == HDRHAI_RTC ) &&
      ( send_msg_cmd_ptr->send_on_curr_rtc_only ) )
  {
    HDRTRACE_TXMSG_DROPPED (send_msg_cmd_ptr->protocol_name, msg_id);
    HDR_MSG_PROT_2 (MSG_LEGACY_ERROR, "RTC msg Qd conn already closed p0x%x m0x%x",
                    send_msg_cmd_ptr->protocol_name, msg_id);
    hdrhmp_free_msg( send_msg_cmd_ptr->msg_ptr, 
                    &intercepting_callback, E_FAILURE );
    return;
  }

  /* If the message is for the RTC but we do not have a connection open,
   * save the message until we can open the connection, then send it.
   * Either the connection is opened (HDRIND_RTCMAC_LINK_ACQUIRED)
   * and the pending messages are given to SLP at that point, or the
   * connection setup fails (HDRIND_IDLE_CONNECTION_FAILED or
   * HDRIND_ALMP_RETURNING_TO_IDLE) and the pending messages are cleared.
   */
  if ( 
#ifdef FEATURE_LTE_TO_HDR_OH
         /* Tunnel mode is on. */ 
         ( tunnel_mode_is_enabled &&
           hdrsaps_get_state() != HDRSAPS_OPEN && 
           tx_chan == HDRHAI_RTC ) ||
         ( tunnel_mode_is_enabled &&
           hdrsaps_get_state() == HDRSAPS_OPEN && 
           !hdrsap_alt_link_open_comp_tx_cb_is_received() &&
           tx_chan == HDRHAI_RTC ) ||
         /* Tunnel mode is off. */
         ( !tunnel_mode_is_enabled &&
#endif /* FEATURE_LTE_TO_HDR_OH */
           !( hdralmp_connection_is_open( HDRHAI_HDR_MESSAGING_PROTOCOL ) ) &&
           ( tx_chan == HDRHAI_RTC ) 
#ifdef FEATURE_LTE_TO_HDR_OH
         )
#endif
     )
  {
    HDRTRACE_HMP_QUEUED_MSG (send_msg_cmd_ptr->protocol_name, msg_id);

    /* Queue this command onto a special queue of pending messages */
    out_of_buffers = hdrhmp_queue_pending_msg( send_msg_cmd_ptr, 
                                               &intercepting_callback );

    if (out_of_buffers)
    {
      hdrhmp_free_msg( send_msg_cmd_ptr->msg_ptr, 
                       &intercepting_callback, E_PROT_Q_FULL );

      /* In a rare case hdrhmp.pending_msg_free_q can be out of buffers. 
         In order for the AT and AN to get back in sync we send session 
         close so that any stale state is cleaned up. As part of session
         close processing RASM and HMP pending queues are cleared */

      hdrsmp_deactivate(HDRHAI_HDR_MESSAGING_PROTOCOL, FALSE);
      return;
    }

    hdralmp_open_connection( HDRHAI_OPEN_SEND_RTC_MSG,
                             HDRHAI_HDR_MESSAGING_PROTOCOL );

    if (send_msg_cmd_ptr->rasm.is_enabled)
    {
      ASSERT (intercepting_callback.outcome_cb == hdrhmp_rasm_cb);
      rasm_ptr = (hdrhmp_rasm_buf_type *) 
                 intercepting_callback.outcome_tag_ptr;
      buf_ptr = &rasm_ptr->buf;

      HDR_MSG_PROT_4 (MSG_LEGACY_HIGH, 
                      "RASM %d TX: Pending TCA (P 0x%x, ID 0x%x) S %x",
                      rasm_ptr->log_id,
                      send_msg_cmd_ptr->protocol_name, msg_id, 
                      (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate);

      ASSERT ((rasm_ptr->state == HDRHMP_RASMS_WAITING) ||
              (rasm_ptr->state == HDRHMP_RASMS_PENDING));

      HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                      "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                      (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                      (uint32) HDRHMP_RASMS_PENDING | (uint32) HDRHMP_RASMSS_NONE);

      rasm_ptr->state = HDRHMP_RASMS_PENDING;
      rasm_ptr->substate = HDRHMP_RASMSS_NONE;
    }
    return;
  }

  auto_resend_is_enabled = hdrhmp_auto_resend_enabled( send_msg_cmd_ptr,
                                                       tx_chan,
                                                       msg_id );

  if (auto_resend_is_enabled && !send_msg_cmd_ptr->rasm.is_enabled)
  {
    /* If the message is already on the HDRHMP_RASM_LIST, don't put 
     * it onto the HDRHMP_AC_MSG_LIST or HDRHMP_RTC_MSG_LIST.
     * This way the hdrhmp_rasm_cb is called directly when SLP is done 
     * with a message, and RASM can decide whether to resend it or not i.e.
     * if a L3 ACK was received or it timed out, no need to resend.
     */

    if ( tx_chan == HDRHAI_AC )
    {
      /* Prior to adding the HMP header, queue this AC message so it can be
       * automatically re-sent in the event of an access handoff.
       */
      out_of_buffers = !hdrhmp_add_to_msg_list( HDRHMP_AC_MSG_LIST, 
                                                &intercepting_callback,
                                                send_msg_cmd_ptr );
    }
    else
    {
      ASSERT ( send_msg_cmd_ptr->chan == HDRHAI_AC_OR_RTC );

      /* If this message can go on either channel and we are choosing to
       * send it on the RTC, we queue it and send it again (on the other
       * channel) if necessary.
       */
      out_of_buffers = !hdrhmp_add_to_msg_list( HDRHMP_RTC_MSG_LIST, 
                                                &intercepting_callback,
                                                send_msg_cmd_ptr );
    }
  }

  if ( out_of_buffers )
  {
    /* We could not queue the buffer on the ac_msg_list, rtc_msg_list,
     * or rasm_list, which means we have more messages pending transmission 
     * than we can handle.  Gracefully recover by forcing re-acquisition.
     */
    hdrhmp_free_msg( send_msg_cmd_ptr->msg_ptr, 
                     &intercepting_callback, E_PROT_Q_FULL );
    hdrhmp_fail_no_msg_list_bufs();
    return;
  }

  /* Add the HMP header and give the command to SLP.
   * hdrhdrhai_protocol_name_enum_type has been carefully defined so
   * no mapping is needed between protocol name and protocol type.
   * The HMP header is not added to RUP registration message.
   */
  HDRTRACE_HMP_SENT_MSG(send_msg_cmd_ptr->protocol_name, msg_id,
    tx_chan, tx_is_reliable);

  if ( send_msg_cmd_ptr->msg_ptr == NULL )
  {
    /* No need to add the HMP header to a RUP registration */
  }
  else
  {
    /* If the message is sent for Stream 0, the protocol_id is given by
    ** the protocol name. Otherwise determine the protocol_id based on the
    ** Stream for which the message is sent
    */
    switch( send_msg_cmd_ptr->stream )
    {
      case HDRHAI_STREAM0:
        protocol_id = send_msg_cmd_ptr->protocol_name;
        break;

      case HDRHAI_STREAM1:
        protocol_id = HDRHAI_N_APP1_TYPE;
        break;

      case HDRHAI_STREAM2:
        protocol_id = HDRHAI_N_APP2_TYPE;
        break;

      case HDRHAI_STREAM3:
        protocol_id = HDRHAI_N_APP3_TYPE;
        break;

      default:
        ERR("Unrecognized stream %d", send_msg_cmd_ptr->stream, 0, 0 );
        /* When discarding a send_msg_cmd without passing the msg_ptr on to
         * SLP, it is HMP's responsibility to free the DSM item allocated by
         * the sending protocol.
         */
        hdrhmp_free_msg( send_msg_cmd_ptr->msg_ptr, 
                         &intercepting_callback, E_FAILURE );
        return;
    }

    if (send_msg_cmd_ptr->protocol_instance == HDRHAI_IN_CONFIG)
    {
      /* in-config means set the upper bit of the protocol ID */
      protocol_id |= 0x80;
    }

    if (!(hdrutil_pushdown_head( &(send_msg_cmd_ptr->msg_ptr), &protocol_id,
                                 HDRHMP_HEADER_SIZE )))
    {
      ERR_FATAL("Pushdown failed",0,0,0);
    }
  }

  if ((tx_chan == HDRHAI_AC) && 
      (send_msg_cmd_ptr->bundle_mask != HDRHMP_BUNDLE_NONE_MSG))
  {
       if((hdridles_get_csetup_substate() == HDRIDLES_CSETUP_CREQ_SLP) &&
	       (hdrhmpsa_amac_given_attempt() == TRUE) && 
           (send_msg_cmd_ptr->protocol_name == HDRHAI_RESERVATION_PROTOCOL)&&
           (msg_id == 0x16))
	    {
		    /* CR is already queued in SLP, and amac started attempt and it is an ROR msg. 
                      If the msg is hold off due to bundling, it shall result in system  loss. 
                      Better to continue without bundling. In the worst case ROR may go unbundled, 
                      atleast it is better than system loss */
	       hdrhmp_stop_waiting_for_bundling_msg(); 
		   /* As the amac attempt already is in progress,
              there is no need to send another RUP to start
              access as during bundle timer expiry */
           send_msg_cmd_ptr->bundle_mask = HDRHMP_BUNDLE_NONE_MSG;
           HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                    "ROR recieved. Setting bundle flag to false and resetting bundle mask ");
	    }
	    else
	    {
		       HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, 
                    "Bundling flag set. Pid %x msgid %x ",send_msg_cmd_ptr->protocol_name,msg_id);
    /* Ask SLP to hold off giving message to ACMAC until bundling is complete*/
    hdrslp_set_waiting_for_bundling_flag(TRUE);
  }
  }

  slp_errno = hdrslp_send_msg(
               tx_chan, tx_is_reliable,
               send_msg_cmd_ptr->priority,
               send_msg_cmd_ptr->msg_ptr,
               intercepting_callback.outcome_cb,
               intercepting_callback.outcome_tag_ptr );

  if (slp_errno != E_SUCCESS)
  {
    ERR("Failed to queue message to SLP",0,0,0);
    /* When discarding a send_msg_cmd without passing the msg_ptr on to SLP,
     * it is HMP's responsibility to free the DSM item allocated by the
     * sending protocol.
     */
    hdrhmp_free_msg( send_msg_cmd_ptr->msg_ptr, 
                     &intercepting_callback, E_FAILURE );

    /* Before we force system lost to recover no buf on msg list case, we also 
     * need to clear RASM queue, otherwise messages are still in RASM queue
     * and will be retransmitted after re-acq 
     */
    hdrhmp_clear_rasm_queue();
    hdrhmp_fail_no_msg_list_bufs();
    return;
  }

  if (send_msg_cmd_ptr->rasm.is_enabled)
  {
    ASSERT (intercepting_callback.outcome_cb == hdrhmp_rasm_cb);

    rasm_ptr = (hdrhmp_rasm_buf_type *) intercepting_callback.outcome_tag_ptr;
    buf_ptr = &rasm_ptr->buf;

    ASSERT (rasm_ptr->state == HDRHMP_RASMS_WAITING);

    HDRHMP_RASM_TRACE( TX );

    HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                    "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                    (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                    (uint32) HDRHMP_RASMS_QUEUED | (uint32) HDRHMP_RASMSS_NONE);

    /* This is a RASM message.  Change it state to "queued" */
    rasm_ptr->state = HDRHMP_RASMS_QUEUED;
    rasm_ptr->substate = HDRHMP_RASMSS_NONE;
    rasm_ptr->attempts++;
    rasm_ptr->tx_chan = tx_chan;
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( tunnel_mode_is_enabled &&
       tx_chan == HDRHAI_AC )
  {
    if ( ( send_msg_cmd_ptr->protocol_name == HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) &&
         ( msg_id == 0x02 /* HDRAMP_UATI_COMP_MSG */) )
    {
      /* HMP is receiving UATIComplete while waiting for UATIAssignment flag is still set
       * it means that the AT received UATIAssignment already
       * Stop wating for UATIAssignment and start access attempt now
       * We clear the SLP UATIAssignment flag after UATIComplete was queued to SLP to 
       * make sure that UATIComplete will bundled with messages in the current SLP queue
       * (e.g. CR, ROR) 
       * hdramp.uati_assignment_slp_holdoff_timer was clear during process UATIAssignment
       * in hdramp
       */
      hdrslp_stop_waiting_for_uati_assignment();

      HDR_MSG_PROT( MSG_LEGACY_HIGH, "SLPQH: Sending UATICompelte." );
    }

    hdrsaptx_start_tx(HDRHAI_HDR_MESSAGING_PROTOCOL, HDRHAI_AC);
    return;
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  if (tx_chan == HDRHAI_AC) 
  {
    start_access_attempt_now = TRUE;

    if (send_msg_cmd_ptr->bundle_mask != HDRHMP_BUNDLE_NONE_MSG)
    {
      HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, 
                      "BUNDLE: waiting mask 0x%x, rx msg with mask 0x%x",
                      hdrhmp.waiting_bundled_msg_mask,
                      send_msg_cmd_ptr->bundle_mask);
      /* Bundle with other message(s) is needed,
       * update hdrhmp.waiting_bundled_msg_mask */
      hdrhmp.waiting_bundled_msg_mask |= send_msg_cmd_ptr->bundle_mask;

      /* Start timer waiting for bundling messages*/
      HDR_MSG_PROT (MSG_LEGACY_MED, "BUNDLE: start bundle timer");
#ifdef FEATURE_HDR_QCHAT
      if (hdrqchatupk_mod_enabled()) 
      {
        hdrqchatupk_process_send_msg(&hdrhmp.bundle_waiting_timer);
      }
      else
      {
      (void) rex_set_timer( &hdrhmp.bundle_waiting_timer, 
                            HDRHMP_BUNDLE_WAITING_TIME_MSEC );
      }
#else      
      (void) rex_set_timer( &hdrhmp.bundle_waiting_timer, 
                            HDRHMP_BUNDLE_WAITING_TIME_MSEC );
#endif /*FEATURE_HDR_QCHAT*/

      /* Actions needed from HMP to complete bundling*/
      if (send_msg_cmd_ptr->bundle_mask & HDRHMP_BUNDLE_CR_MSG)
      {
        /* Message needs to be bundled with CR,
         * check if CR is currently in HDRMC or HDRSLP queue 
         */
        if (hdridles_get_state() != HDRIDLES_CONN_SETUP_STATE) 
        {
          /* Need to call hdralmp_open_connection() to generate CR to be bundled
           * with this message for following two reasons:
           * 1. We already check there is no CR given to ACMAC yet(by checking
           *    hdridle_is_connecting() when we decide this AC_OR_RTC msg can 
           *    be sent on AC), and  
           * 2. IDLE is not currently in CONN_SETUP_STATE means there is no CR 
           *    in the HMP or SLP queue yet.
           * Note: If it's AC only message and needs to be bundle with CR, it will be 
           *       be dropped already when we check hdridle_is_connecting()
           */
          hdralmp_open_connection( HDRHAI_OPEN_SEND_RTC_MSG,
                                   HDRHAI_HDR_MESSAGING_PROTOCOL );
        }
        else
        {
          /* The AT is in IDLE conn setup state but the CR hasn't been givien to 
           * AC MAC yet
           * How to clear CR bit in the waiting_bundled_msg_mask ?
           * case1:
           * hdrmc queue:                  slp queue
           * --------                ----------
           *     CR |                  ROR    |
           * --------                 ---------
           *       Coming CR will clear CR bit in waiting_bundled_msg_mask 
           * case2:
           * hdrmc queue:                  slp queue
           * --------                ----------
           *     ROR |                  CR    |
           * --------                 ---------
           *     When we receive ROR, CR is currently in SLP queue, 
           *     how to clear CR bit?  
           *     - corner case
           *     - CR will still be bundled with ROR after bundle timer expires
           *     
           */
        }
      }

      start_access_attempt_now = FALSE;
    }


    if (hdrhmp.waiting_bundled_msg_mask != 0)
    {
      if ((send_msg_cmd_ptr->protocol_name == HDRHAI_IDLE_STATE_PROTOCOL) &&
          msg_id == 0x01)
      {
        /* Received ConnectionRequest msg*/
        hdrhmp.received_bundled_msg_mask |= HDRHMP_BUNDLE_CR_MSG;
      }
      else if ((send_msg_cmd_ptr->protocol_name == HDRHAI_DATA_OVER_SIGNALING_PROTOCOL) &&
               msg_id == 0x14)
      {
        /* Received DOS msg*/
        hdrhmp.received_bundled_msg_mask |= HDRHMP_BUNDLE_DOS_MSG;
      }

      HDR_MSG_PROT_4 (MSG_LEGACY_HIGH, 
                      "BUNDLE: rx msg: pid 0x%x, msg_id 0x%x, "
                      "waiting msg mask: 0x%x, rx msg mask: 0x%x", 
                      send_msg_cmd_ptr->protocol_name, msg_id,
                      hdrhmp.waiting_bundled_msg_mask,
                      hdrhmp.received_bundled_msg_mask);

      #ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
      if ((send_msg_cmd_ptr->protocol_name == HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL) &&
               (msg_id == 0 /* HDRAMP_UATI_REQ_MSG */)) 
      {
        /* If receive UATIRequest while waiting for bundling msg, We should start 
         * access attempt without calling hdrhmp_stop_waiting_for_bundling_msg*/
        start_access_attempt_now = TRUE;

      }
      else
      #endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */
      if (hdrhmp.waiting_bundled_msg_mask  == hdrhmp.received_bundled_msg_mask )
      {
        /* Received all waiting messages */
        hdrhmp_stop_waiting_for_bundling_msg();

        start_access_attempt_now = TRUE;
        HDR_MSG_PROT (MSG_LEGACY_HIGH, "BUNDLE: Completed! Start access attempt now");
      }
      else
      {
        /* Do not start access if we are still waiting for message(s)*/
        start_access_attempt_now = FALSE;
      }
    }

    #ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
    if (hdrslp_is_waiting_for_uati_assignment())
    {
      /* The UATIRequest has been sent but no UATIAssignment received yet and
         another AC message is being sent.  Typically the message is either
         RoR, DoS or CR.

         While waiting for UATIAssignment SLP does not give any messages to PCP
         if there is an access attempt in progress and does not start a new
         access attempt, no matter what.  If RoR is received that requests to
         bundle with CR the AT immediately calls hdralmp_open_connection() to
         queue the ConnectionRequest to SLP.

         The bundle_waiting_timer is used to delay calling
         hdralmp_open_connection() for 10ms after receiving RoR if the
         application specified it wanted to bundle a DoS message.  For
         simplicity this timer is not used when waiting for UATIAssignment.  In
         the rare event that the AT receives UATIAssignment after RoR but before
         DoS, the AT will not bundle the DoS message.  This is expected to be
         very rare and not noticeable to the user.

         When AMP receives a UATIAssignment (or times out waiting) it clears the
         flag in SLP so hdrslp_is_waiting_for_uati_assignment() will be false,
         and also ensures another message (or RUP Registration) is sent so
         that an access attempt will be started to send any messages still
         queued to SLP. */

      start_access_attempt_now = FALSE;

      if (send_msg_cmd_ptr->bundle_mask != HDRHMP_BUNDLE_NONE_MSG)
      {
        if (hdramac_is_in_attempt(HDRHAI_HDR_MESSAGING_PROTOCOL))
        {
          HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, 
                        "SLPQH: msg with bundle mask 0x%x queued < 1s after UATIReq.",
                          send_msg_cmd_ptr->bundle_mask);
        }
        else
        {
          HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, 
                        "SLPQH: msg with bundle mask 0x%x queued > 1s after UATIReq.",
                          send_msg_cmd_ptr->bundle_mask);
        }

        HDR_MSG_PROT (MSG_LEGACY_HIGH, "SLPQH: Still waiting for UATIAssign.");
      }
      else if ((send_msg_cmd_ptr->protocol_name == HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL) &&
               (msg_id == 0 /* HDRAMP_UATI_REQ_MSG */))
      {
        /* In the case of idle handoff to another sector in the new subnet the
           AT will send another UATIRequest but leaves the SLP flag true (i.e.
           hdrslp_is_waiting_for_uati_assignment() stays TRUE.  This is
           required because clearing the flag while in an attempt would cause
           other queued messages e.g. RoR, DoS or CR to be given to PCP which is
           wrong.  Just re-starting the attempt after the UATIRequest is queued
           results in SLP only giving the new UATIRequest to PCP and continuing
           to hold all other messages */
        start_access_attempt_now = TRUE;

        if (hdramac_is_in_attempt(HDRHAI_HDR_MESSAGING_PROTOCOL))
        {
          HDR_MSG_PROT (MSG_LEGACY_HIGH, 
             "SLPQH: IdleHO/AHO within new subnet < 1s after previous UATIReq."
             "Sending another UATIReq.");
        }
        else
        {
          HDR_MSG_PROT (MSG_LEGACY_HIGH, 
             "SLPQH: IdleHO/AHO within new subnet > 1s after previous UATIReq."
             "Starting another access to send another UATIReq.");
        }
      }
      else if ((send_msg_cmd_ptr->protocol_name == HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL) &&
               (msg_id == 0x02 /* HDRAMP_UATI_COMP_MSG */))
      {
        /* HMP is receiving UATIComplete while waiting for UATIAssignment flag is still set
         * it means that the AT received UATIAssignment already
         * stop wating for UATIAssignment and start access attempt now
         * We clear the SLP uati assignment flag after UATIComplete was queued to SLP to 
         * make sure that UATIComplete will bundled with messages in the current SLP queue
         * (e.g. CR, ROR) 
         * hdramp.uati_assignment_slp_holdoff_timer was clear during process UATIAssignment
         * in hdramp
         */
        hdrslp_stop_waiting_for_uati_assignment();

        start_access_attempt_now = TRUE;

        if (hdramac_is_in_attempt(HDRHAI_HDR_MESSAGING_PROTOCOL))
        {
          HDR_MSG_PROT (MSG_LEGACY_HIGH, 
             "SLPQH: received UATIAssignment < 1s after previous UATIReq."
             "Sending UATIComplete.");
        }
        else
        {
          HDR_MSG_PROT (MSG_LEGACY_HIGH, 
             "SLPQH: received UATIAssignment > 1s after previous UATIReq."
             "Starting another access to send UATICompelte.");
        }
      }
      else
      {
        if (hdramac_is_in_attempt(HDRHAI_HDR_MESSAGING_PROTOCOL))
        {
          HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, 
                          "SLPQH: Msg (P 0x%x, ID 0x%x) queued < 1s after UATIReq. Still waiting for UATIAssign.",
             send_msg_cmd_ptr->protocol_name, msg_id );
        }
        else
        {
          HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, 
                          "SLPQH: Msg (P 0x%x, ID 0x%x) queued > 1s after UATIReq. Still waiting for UATIAssign.",
             send_msg_cmd_ptr->protocol_name, msg_id );
        }
      }
    }
    #endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

    if (start_access_attempt_now)
    {
      /* bundle completed  or don't need to wait messages*/

      /* If the message is to go out on the Access Channel, HMP needs to
       * first wait for the LMAC to transition to the access mode
       */
      hdrhmpsa_start();
    }
  }
} /* hdrhmp_process_send_msg_cmd() */

/*===========================================================================

FUNCTION HDRHMP_FIND_RASM

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS
  send_msg_cmd_ptr - Pointer to the "Send Message" command for HMP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static hdrhmp_rasm_buf_type* hdrhmp_find_rasm
(
  hdrhai_protocol_name_enum_type name,
  uint32 transaction_id
)
{
  hdrhmp_rasm_buf_type *rasm_ptr; /* buffer holding RASM message */
  hdrhmp_msg_buf_type *buf_ptr; /* convenience ptr to wrapped buffer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rasm_ptr = (hdrhmp_rasm_buf_type *) q_check( &hdrhmp.rasm_list );

  while (rasm_ptr != NULL)
  {
    buf_ptr = &rasm_ptr->buf;

    if ((buf_ptr->msg_cmd.protocol_name == name) &&
        (buf_ptr->msg_cmd.rasm.transaction_id == transaction_id ))
    {
      return rasm_ptr;
    }
    else
    {
      rasm_ptr = q_next( &hdrhmp.rasm_list, 
                         &buf_ptr->hdr.link );
    }
  }

  return NULL;

} /* hdrhmp_find_rasm() */

/*===========================================================================

FUNCTION HDRHMP_PROCESS_RASM_ACKED

DESCRIPTION
  Only best effort messages being sent on the RTC require a L3 Ack.
  If the message is currently queued to SLP, the SLP callback may occur
  after the message has already been Acked.

DEPENDENCIES

PARAMETERS
  send_msg_cmd_ptr - Pointer to the "Send Message" command for HMP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdrhmp_process_rasm_acked
(
  hdrhmp_rasm_acked_params_type * rasm_acked_cmd_ptr
)
{
  hdrhmp_rasm_buf_type *rasm_ptr; /* buffer holding RASM message */
  hdrhmp_msg_buf_type *buf_ptr; /* convenience ptr to wrapped buffer */
  uint8 msg_id; /* for debug only */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rasm_ptr = hdrhmp_find_rasm( rasm_acked_cmd_ptr->protocol_name,
                                  rasm_acked_cmd_ptr->transaction_id );

  if (rasm_ptr == NULL)
  {
    HDR_MSG_PROT_2 (MSG_LEGACY_MED, 
                    "RASM L3ACK: Ack for msg that was not outstanding P 0x%x, T 0x%x", 
                    rasm_acked_cmd_ptr->protocol_name,
                    rasm_acked_cmd_ptr->transaction_id);
  }
  else
  {
    buf_ptr = &rasm_ptr->buf;

    HDRHMP_RASM_TRACE( L3ACK );

    (void) dsmbit_unpack8( buf_ptr->msg_cmd.msg_ptr, &msg_id, 0, 8 );

    HDR_MSG_PROT_4 (MSG_LEGACY_HIGH, 
                    "RASM %d L3ACK: L3 Ack P 0x%x, ID 0x%x, T 0x%x", 
                    rasm_ptr->log_id,
                    buf_ptr->msg_cmd.protocol_name, msg_id, 
                    rasm_acked_cmd_ptr->transaction_id );

    hdrhmp_process_remove_rasm_entry(rasm_ptr, HDRHMP_RASM_REMOVE_L3ACKED);

  }
} /* hdrhmp_process_rasm_acked() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to HMP.

DEPENDENCIES
  None

PARAMETERS
  hmp_cmd_ptr - Pointer to the command for HMP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_process_cmd( hdrhmp_cmd_type * hmp_cmd_ptr )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_CMD( HDRHAI_HDR_MESSAGING_PROTOCOL, hmp_cmd_ptr->name,
    hmp_cmd_ptr->sender, 0 );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch ( hmp_cmd_ptr->name )
  {
    case HDRHMP_SEND_MSG_CMD: /* Sends a message down to SLP for transmit */
      hdrhmp_process_send_msg_cmd( &hmp_cmd_ptr->params.send_msg );
      hdrhmp_msgs_on_event_q( -1 );
      break;

    case HDRHMP_RASM_ACKED_CMD: 
      hdrhmp_process_rasm_acked( &hmp_cmd_ptr->params.rasm_acked );
      break;

  case HDRHMP_RASM_CB_CMD: 
    hdrhmp_process_rasm_cb( &hmp_cmd_ptr->params.rasm_cb );
    break;

    default:
      ERR_FATAL( "Unknown command to HMP %d", hmp_cmd_ptr->name, 0, 0 );
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrhmp_process_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_CLEAR_PENDING_MSG_QUEUE

DESCRIPTION
  This function removes all the messages left on the pending message queue

DEPENDENCIES
  None

PARAMETERS
  failure_reason - connection setup failure reason

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrhmp_clear_pending_msg_queue
(
  errno_enum_type failure_reason
)

{
  hdrhmp_msg_buf_type *buf_ptr;          /* buffer for pending message cmd */
  uint8 msg_id; /* for trace only */
  hdrhmp_callback_info_type callback_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT (MSG_LEGACY_HIGH,  "hdrhmp_clear_pending_msg_queue");
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "msg in pending queue:%d",
                  q_cnt(&hdrhmp.pending_msg_q));
				  
  while ( (buf_ptr =
           (hdrhmp_msg_buf_type *) q_get( &hdrhmp.pending_msg_q )) != NULL)
  {
    /* Extract the msg_id for debug purposes only. If we fail to extract the
     * msg_id, the only consequence is an incorrect debug message.
     */
    if (buf_ptr->msg_cmd.msg_ptr != NULL)
    {
      (void) dsmbit_unpack8( buf_ptr->msg_cmd.msg_ptr, &msg_id, 0, 8 );
    }
    else
    {
      ASSERT( buf_ptr->msg_cmd.protocol_name ==
        HDRHAI_ROUTE_UPDATE_PROTOCOL);
      msg_id = 0; /* This is a Route Update message */
    }

    HDR_MSG_PROT_3(MSG_LEGACY_ERROR,"Removing P=%d, M=%d, Chan=%d from hdrhmp.pending_msg_q", 
                   buf_ptr->msg_cmd.protocol_name,
                   msg_id,
                   buf_ptr->msg_cmd.chan);

    /* If a message on the pending queue can be send on the AC or RTC, it should
    be requeued if the connection setup failed */
    if ((failure_reason == E_AGAIN) && 
        (buf_ptr->msg_cmd.chan == HDRHAI_AC_OR_RTC) &&
        !buf_ptr->msg_cmd.rasm.is_enabled) /* RASM handles E_AGAIN itself */
    {
      hdrhmp_app_send_msg(
        buf_ptr->msg_cmd.protocol_name,
        buf_ptr->msg_cmd.stream,
        buf_ptr->msg_cmd.chan,
        buf_ptr->msg_cmd.rtc_is_reliable,
        buf_ptr->msg_cmd.priority,
        buf_ptr->msg_cmd.protocol_instance,
        buf_ptr->msg_cmd.msg_ptr,
        buf_ptr->msg_cmd.outcome_cb,
        buf_ptr->msg_cmd.outcome_tag_ptr);
    }
    else
    {
      HDRTRACE_TXMSG_DROPPED (buf_ptr->msg_cmd.protocol_name, msg_id);
      callback_info.outcome_cb = buf_ptr->msg_cmd.outcome_cb;
      callback_info.outcome_tag_ptr = buf_ptr->msg_cmd.outcome_tag_ptr;
      hdrhmp_free_msg( buf_ptr->msg_cmd.msg_ptr, 
                       &callback_info, failure_reason);
    }
    modem_mem_free( buf_ptr, MODEM_MEM_CLIENT_HDR_CRIT );
  }
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "hdrhmp_clear_pending_msg_queue updated cnt %d",q_cnt(&hdrhmp.pending_msg_q));
} /* hdrhmp_clear_pending_msg_queue() */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_CLEAR_RASM_QUEUE

DESCRIPTION
  This function removes all the messages left on the RASM queue. This function 
  could be called when protocol decides that there is no need to retrasnsmit  
  messages from RASM list.( e.g. session is not open )

DEPENDENCIES
  None

PARAMETERS
 None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_clear_rasm_queue( void )
{
  hdrhmp_rasm_buf_type *rasm_ptr; /* buffer holding RASM message */
  hdrhmp_rasm_buf_type *next_ptr; /* buffer holding RASM message */
  hdrhmp_msg_buf_type *buf_ptr; /* convenience ptr to wrapped buffer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Just loop through all the messages in the list and check their state
   * If it is still queued for SLP or pending for connection setup, mark it 
   * as RASM_DONE state and wait for the SLP callback (hdrhmp_rasm_cb()) 
   * to delete it. 
   */

  rasm_ptr = (hdrhmp_rasm_buf_type *) q_check( &hdrhmp.rasm_list );

  while (rasm_ptr != NULL)
  {
    buf_ptr = &rasm_ptr->buf;

    /* Get next entry now in case we delete the current one */
    next_ptr = q_next( &hdrhmp.rasm_list, &buf_ptr->hdr.link );

    HDRHMP_RASM_TRACE( CLR );

    hdrhmp_process_remove_rasm_entry(rasm_ptr, HDRHMP_RASM_REMOVE_CLEARED);

    rasm_ptr = next_ptr;
  }
} /* hdrhmp_clear_rasm_queue() */


#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRHMP_PROCESS_TUNNEL_SESSION_NEGOTIATION_FAILED

DESCRIPTION
   This function is only used in tunnel mode to handle consecutive  session 
   negotiation failures. This function mimics SD avoidance behavior when AT 
   is in tunnel.

DEPENDENCIES
  None

PARAMETERS
  failure_reason - session negotiation failure reason  

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_process_tunnel_session_negotiation_failed
(
  hdrhai_session_close_reason_enum_type close_reason 
)
{
  rex_timer_cnt_type backoff_time;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( close_reason == HDRHAI_CLOSE_CONN_OPEN_FAIL )
  {
    /* We want to try to re-open the session on this system but need
     * to wait for the session backoff timer expires before activating SMP 
     * again.
     * scenarios: After HDRSCP_CON_FAIL_RETRY_COUNT consecutive failures on 
     *    1. rxed AltLinkOpenFail, or 
     *    2. Timed out waiting for ALtLinkOpenCnf
     */
    HDR_MSG_PROT( MSG_LEGACY_MED, "backoff using telescoping mechanism" );

    if ( hdrhmp.sc_backoff.cnt < HDRHMP_BACKOFF_TIMER_CNT_MAX )
    {
      backoff_time = backoff_time_table_sc[hdrhmp.sc_backoff.cnt] *
                       HDRHMP_SECOND_TO_MS_CONVERSION;
    }
    else
    {
      backoff_time = backoff_time_table_sc[HDRHMP_BACKOFF_TIMER_CNT_MAX - 1] *
                       HDRHMP_SECOND_TO_MS_CONVERSION;
    }
    hdrhmp.sc_backoff.cnt++;
  }
  else
  {
    /* Assume we can not establish a session on this sector */
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "backoff for %d seconds",
                    HDRHMP_BACKOFF_TIME_SESSION_FAILED );

    backoff_time = HDRHMP_BACKOFF_TIME_SESSION_FAILED * 
                     HDRHMP_SECOND_TO_MS_CONVERSION;
  }

  /* Re-open the session if 
   * 1. backoff timer expires
   * 2. new Color Code
   * 3. enter native HRPD
   */
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "Start backoff timer %dms", backoff_time );
  ( void ) rex_set_timer( &hdrhmp.sc_backoff.backoff_timer, backoff_time );

} /* hdrhmp_process_tunnel_session_negotiation_failed() */
#endif /* FEATURE_LTE_TO_HDR_OH*/


/*===========================================================================

FUNCTION HDRHMP_PROCESS_SESSION_CLOSED

DESCRIPTION
  This function processes session closed indication.

DEPENDENCIES
  None

PARAMETERS
  close_reason - reason session is closed 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrhmp_process_session_closed
( 
  hdrhai_session_close_reason_enum_type close_reason 
)
{
  sd_ss_act_e_type sd_action;
    /* Continue, acquire HDR, acquire IS-95, etc. */
  hdraddr_type subnet;
    /* sector ID of sector on which session closed */
  hdrmc_cb_params_rsp_union_type cb_param;  
    /* Parameters for Session Close callback */

#ifdef FEATURE_LTE_TO_HDR_OH
  uint8 colorcode = 0;
    /* colorcode of sector on which session closed */
#endif /* FEATURE_LTE_TO_HDR_OH */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we were attempting to start an access attempt when the session
   * was closed, that access is no longer required.
   */
  hdrhmpsa_abort();
  hdrhmp_stop_waiting_for_bundling_msg();

  /* Our attempt to open a session resulted in some abnormal abort.
   * Increment session close count and tell SD if count exceeds max.
   */
#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_HDR_MESSAGING_PROTOCOL ) )
  {
    /* In tunnel mode, Check ColorCode instead of SubnetID */
    if ( hdrovhd_get_color_code( &colorcode, HDRHAI_HDR_MESSAGING_PROTOCOL ) !=
           E_SUCCESS )
    {
      hdrhmp.sclose.count = 0;
      hdrhmp.sc_backoff.cnt = 0; /*Reset backoff counter */
      HDR_MSG_PROT( MSG_LEGACY_ERROR,
                    "HMP: Reset SC count (can't get colorcode)" );
    }
    else if ( colorcode != hdrhmp.sc_backoff.colorcode )
    {
      /* The close occurred on a new colorcode, so start the count over */
      hdrhmp.sc_backoff.colorcode = colorcode;
      hdrhmp.sclose.count = 0;
      hdrhmp.sc_backoff.cnt = 0; /*Reset backoff counter */
      HDR_MSG_PROT( MSG_LEGACY_MED, "HMP: Reset SC count (new ColorCode)" );
    }
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    if (hdrovhd_get_subnet(subnet, HDRHAI_HDR_MESSAGING_PROTOCOL) !=
        E_SUCCESS)
    {
      hdrhmp.sclose.count = 0;
      HDR_MSG_PROT (MSG_LEGACY_MED, "HMP: Reset SC count (can't get subnet)");
    }
    else if (!hdraddr_is_equal(subnet, hdrhmp.sclose.subnet))
    {
      /* The close occurred on a new subnet, so start the count over */
      hdraddr_set(hdrhmp.sclose.subnet, subnet);
      hdrhmp.sclose.count = 0;
      HDR_MSG_PROT (MSG_LEGACY_MED, "HMP: Reset SC count (new subnet)");
    }
  }

  switch (close_reason)
  {
    case HDRHAI_CLOSE_CFG_RSP_EXP: 
	/* When AN does not respond for cfg req for HDRHMP_SCLOSE_MAX times, 
	   that system is avoided, instead of continuosly trying on the 
	   same system. This is implemented after checking with System folks */  
    case HDRHAI_CLOSE_PROT_NEG_FAIL:
    case HDRHAI_CLOSE_AN_GAUP_FAIL:
    case HDRHAI_CLOSE_PERSONALITY_INDEX_INVALID:
      /* - the AN sent out invalid data */
      hdrhmp.sclose.count++;
      HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "HMP: SC count = %d (reason %d)",
                      hdrhmp.sclose.count, close_reason);
      break;

    case HDRHAI_CLOSE_CONN_OPEN_DENY:
      /* - we unsuccessfully tried to open the connection 5x for
           the purpose of sending a config message and the AN
           replied at least once with a Connection Deny due to
           Authentication or Billing failures.
         Immediately avoid the system for 60 seconds in System 
         Determination. */
      hdrhmp.sclose.count = HDRHMP_SCLOSE_MAX;
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "HMP: AN sends connection deny!");
      break;

    case HDRHAI_CLOSE_CONN_OPEN_FAIL:
      /* no TCA or no RTCAck
        Immediately avoid the system using telescoping mechanism
        in System Determination. */
      hdrhmp.sclose.count = HDRHMP_SCLOSE_MAX;
#ifdef FEATURE_LTE_TO_HDR_OH
      if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_HDR_MESSAGING_PROTOCOL ) )
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                      "HMP: No AltLinkOpenCnf or Rxed AltLinkOpenFail !" );
      }
      else
#endif /* FEATURE_LTE_TO_HDR_OH */
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "HMP: No TCA or no RTC Ack !" );
      }

      break;

    case HDRHAI_CLOSE_REPLY:
      /* Receiving a session close msg from the AN should only
         be counted as a session failure if we received the msg
         before a session is successfully opened */
      if (!hdrhmp.sclose.session_opened_successfully)
      {
        hdrhmp.sclose.count++;
        HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "HMP: SC count = %d (reason %d)",
                        hdrhmp.sclose.count,close_reason);
      }
      break;

    case HDRHAI_CLOSE_UATI_FAIL:
      /* If the AN consistently acks our UATIRequest but does not give
       * A UATIAssignment for all 5 retries, immediately avoid the
       * system because the AN is obviously broken and we don't want to
       * get stuck sending a UATIRequest every 5 seconds forever.
       */
      hdrhmp.sclose.count = HDRHMP_SCLOSE_MAX;
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "HMP: 5 UATIAssignment timeouts!");
      break;

    case HDRHAI_CLOSE_NEW_NETWORK:
    case HDRHAI_CLOSE_NEW_ESN:
    case HDRHAI_CLOSE_NEW_NAI:
    case HDRHAI_CLOSE_NOT_MAINT_UATI:
    case HDRHAI_CLOSE_KA_EXP:
    case HDRHAI_CLOSE_DEACTIVATE:
    case HDRHAI_CLOSE_CFG_MSG_FAIL:
    case HDRHAI_CLOSE_AN_INIT_EXP:
    case HDRHAI_CLOSE_QUICK_FAILURE:
    case HDRHAI_CLOSE_SILENT_DEACTIVATE:
#ifdef FEATURE_EHRPD
    case HDRHAI_CLOSE_EHRPD_AVAILABILITY_CHANGED:
#endif /* FEATURE_EHRPD */
#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRHAI_CLOSE_IRAT_CHANGED:
#endif /* FEATURE_LTE_TO_HDR_OH */

      /* Do nothing: these are legitimate failures.  They will not
      be counted against the avoidance timer*/
      HDR_MSG_PROT_1 (MSG_LEGACY_MED,
                      "HMP: SC count not incremented (%d)",
                      close_reason);
      break;

    default:
      HDR_MSG_PROT_1 (MSG_LEGACY_ERROR,
                      "Invalid session close reason (%d)",
                      close_reason);
      break;
  }

  /* Callback functions to inform registered clients. */
  cb_param.session_close.cb_type = HDRMC_CB_EVENT_SESSION_CLOSE;
  cb_param.session_close.close_reason = close_reason;
  hdrmc_execute_cb(&cb_param);

  if (hdrhmp.sclose.session_opened_successfully)
  {
    /* If we opened the session successfully, re-set our failure count */
    if (hdrhmp.sclose.count != 0)
    {
      hdrhmp.sclose.count = 0;
      HDR_MSG_PROT (MSG_LEGACY_MED, "HMP: Reset SC count (first close)");
    }
    hdrhmp.sclose.session_opened_successfully = FALSE;
#ifdef FEATURE_LTE_TO_HDR_OH
    hdrhmp.sc_backoff.cnt = 0;
#endif /* FEATURE_LTE_TO_HDR_OH*/
  }

  HDRTRACE_HMP_SESSION_CLOSED(close_reason, hdrhmp.sclose.count);

  if (hdrhmp.sclose.count >= HDRHMP_SCLOSE_MAX)
  {
#ifdef FEATURE_LTE_TO_HDR_OH
    if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_HDR_MESSAGING_PROTOCOL ) )
    {
      hdrhmp_process_tunnel_session_negotiation_failed( close_reason );

      hdrhmp.sclose.count = 0;
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                      "HMP: Couldn't open session in tunnel, reason: %d", 
                      close_reason );
      return;
    }
#endif /* FEATURE_LTE_TO_HDR_OH */

    if ( hdralmps_get_state() != HDRALMPS_INACTIVE )
    {
      if (close_reason == HDRHAI_CLOSE_CONN_OPEN_FAIL)
      {
        sd_action = sd_ss_ind_hdr_opr_ses_nego_timeout( NULL );
        ERR("Avoiding System using telescoping mechanism", 0, 0, 0);
      }
      else
      {
        /* Assume we can not establish a session on this sector */
        sd_action = sd_ss_ind_hdr_opr_ses_abort( NULL );
        ERR("Avoiding System for 10 Minutes",0,0,0);
      }
  
      HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "SD Action %d", sd_action);
  
      if ( sd_action == SD_SS_ACT_ACQ_HDR )
      {
        hdralmp_reacquire(HDRHAI_ABORT_UNSPECIFIED,
                          HDRHAI_HDR_MESSAGING_PROTOCOL);
      }
      else
      {
        hdralmp_deactivate(HDRHAI_ABORT_UNSPECIFIED, FALSE, 0,
                           HDRHAI_HDR_MESSAGING_PROTOCOL);
      }
    }
    else
    {
      /* If HDR is already INACTIVE state, don't query SD for next action.
       * This is to avoid race condition where HDR and other tasks access
       * SD library in the same time. (With current design, only one task
       * can access SD library?) */
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "HMP: ALMP is INACTIVE, don't query SD" );
    }

    if (hdrhmp.sclose.count != 0)
    {
      hdrhmp.sclose.count = 0;
      HDR_MSG_PROT (MSG_LEGACY_MED, "HMP: Reset SC count (end avoid)");
    }
  }
  else if ( hdralmp_system_is_acquired(HDRHAI_HDR_MESSAGING_PROTOCOL))
  {
    if(hdralmp_connection_is_open(HDRHAI_HDR_MESSAGING_PROTOCOL))
    {
       ERR_FATAL( "Connection is open", 0, 0, 0 );
    }

    if ( hdrsmp_is_to_maintain_evdo_session() )
    {
      /* We are idle on the system and can try re-open the session now */
      HDR_MSG_PROT (MSG_LEGACY_HIGH,  " HMP: Opening new session ...");
      hdrsmp_activate(HDRHAI_HDR_MESSAGING_PROTOCOL);
    }
    else
    {
      /* Wait until we actually want to use data services 
       * before opening the session. */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                    " MUM: data call is required to open session");
    }
  }
  else
  {
    /* We want to try to re-open the session on this system but need
     * to wait for the HDRIND_INIT_NETWORK_ACQUIRED indication or
     * HDRIND_ALMP_EXIT_HDR_MODE indication before activating SMP again.
     */
  }
}/* hdrhmp_process_session_closed() */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_PROCESS_IND

DESCRIPTION
  This function processes indications given to HMP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for HMP to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrhmp_process_ind( hdrind_ind_name_enum_type ind_name,
       hdrind_ind_data_union_type * ind_data_ptr )
{
  hdrhmp_rasm_buf_type *rasm_ptr; /* buffer holding RASM message */
  hdrhmp_rasm_buf_type *next_ptr; /* buffer holding RASM message */
  hdrhmp_msg_buf_type *buf_ptr; /* convenience ptr to wrapped buffer */
  uint8 msg_id = 0; /* for trace only */
#ifdef FEATURE_LTE_TO_HDR_OH
  /* If tunnel mode is enabled */
  boolean tunnel_mode_is_enabled = hdrsap_tunnel_mode_is_enabled(HDRHAI_HDR_MESSAGING_PROTOCOL);
  uint8 colorcode = 0;
#endif /* FEATURE_LTE_TO_HDR_OH*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_IND( HDRHAI_HDR_MESSAGING_PROTOCOL, ind_name, 0 );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch ( ind_name )
  {
    case HDRIND_OVHD_INFO_UPDATED:
    case HDRIND_OVHD_AP_UPDATED:
      hdrhmpsa_check_ovhd(  );
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRIND_OVHD_PREREG_INFO_UPDATED:
      
      if ( !ind_data_ptr->prereg_is_allowed )
      {
        HDR_MSG_PROT( MSG_LEGACY_MED, "HMP: pre-reg is not allowed ..." );
        hdrhmp_clear_rasm_queue();
        break;
      }

      HDR_MSG_PROT( MSG_LEGACY_HIGH,
                    "HMP: pre-reg is allowed, check session status..." );

      if ( hdrsmp_session_is_open( HDRHAI_HDR_MESSAGING_PROTOCOL ) &&
           hdramp_is_session_close_nai_pending() )
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "Close session because NAI changes" );
        hdrsmp_deactivate( HDRHAI_HDR_MESSAGING_PROTOCOL, TRUE );
        hdramp_set_session_close_nai_pending(FALSE);
        break;
      }

      if ( hdramp_is_session_close_nai_pending() )
      {
        hdramp_set_session_close_nai_pending(FALSE);
      }

      if ( hdrstream_stream_is_assigned( HDRHAI_APP_PACKET_APP_ANY ) &&
           !hdrscp_ehrpd_status_is_valid() )
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
          "eHRPD availability of current session is invalid. Close session" );
        hdrsmp_deactivate( HDRHAI_HDR_MESSAGING_PROTOCOL, TRUE );
        break;
      }

      if ( hdrstream_stream_is_assigned( HDRHAI_APP_PACKET_APP_ANY ) && 
           !hdrscp_is_personality_irat() )
      {
        /* Current personality is not IRAT and prereg is allowed in this LTE cell.
         * Need to close current session internally (not sending SessionClose msg). 
         */
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "No IRAT personality. Close session" );
        hdrsmp_deactivate( HDRHAI_HDR_MESSAGING_PROTOCOL, TRUE );
        break;
      }

      if( rex_get_timer( &hdrhmp.sc_backoff.backoff_timer ) != 0 )
      {
        ( void ) hdrovhd_get_color_code( &colorcode, 
                                         HDRHAI_HDR_MESSAGING_PROTOCOL );

        if ( colorcode == hdrhmp.sc_backoff.colorcode )
        {
          /* If session backoff timer is still running, we do not want to open 
           * a new sessoin now if color code does not change ...*/
          HDR_MSG_PROT( MSG_LEGACY_HIGH,
                        "HMP: same color code. "
                        "Session backoff timer is still running..." );
          break;
        }
        else
        {
          HDR_MSG_PROT( MSG_LEGACY_HIGH,
                        "HMP: diff color code. reset backoff timer" );
          (void) rex_clr_timer( &hdrhmp.sc_backoff.backoff_timer );
          hdrhmp.sc_backoff.cnt = 0;
          hdrhmp.sclose.count = 0;
        }
      }

      if ( !hdrsmp_session_is_open( HDRHAI_HDR_MESSAGING_PROTOCOL ) &&
           !hdrsmp_session_is_closing( HDRHAI_HDR_MESSAGING_PROTOCOL) 
#ifdef FEATURE_LTE_TO_HDR
           && !hdrmc_lte_to_hdr_get_cgi_is_in_progress()
#endif /* FEATURE_LTE_TO_HDR */
         )
      {
        if ( hdrsmp_is_to_maintain_evdo_session() )
        {
          /* The AT software requirment is to never just sit idle on an EV-DO
           * channel without opening a session.  This a) allows for fast 
           * connection setup and b) ensures that the service indicator
           * reflects "usable" HDR service and not just EV-DO acquisition.
           * The EV-DO network should expect every EV-DO provisioned AT
           * to have a valid session at all times.
           */
          HDR_MSG_PROT (MSG_LEGACY_HIGH,  " HMP: Opening new session ...");
          hdrsmp_activate(HDRHAI_HDR_MESSAGING_PROTOCOL);
        }
        else
        {
          /* Wait until we actually want to use data services before 
           * opening the session.
           */
          HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                        " MUM: data call is required to open session");
        }
      }
      else
      {
        /* If a session is open already... just log the protocol subtype info
           log packet. */
        hdrscmdb_log_pers_subtype_stream_config();
      }

      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    case HDRIND_INIT_NETWORK_ACQUIRED:
      /* reset flag after HDR has acquired */
      hdrhmp.buf_full_reacq_pending = FALSE;

      if ( !hdrsmp_session_is_open( HDRHAI_HDR_MESSAGING_PROTOCOL ) &&
           !hdrsmp_session_is_closing( HDRHAI_HDR_MESSAGING_PROTOCOL) 
#ifdef FEATURE_LTE_TO_HDR
           && !hdrmc_lte_to_hdr_get_cgi_is_in_progress()
#endif /* FEATURE_LTE_TO_HDR */
         )
      {
        if ( hdrsmp_is_to_maintain_evdo_session() )
        {
          /* The AT software requirment is to never just sit idle on an EV-DO
           * channel without opening a session.  This a) allows for fast 
           * connection setup and b) ensures that the service indicator
           * reflects "usable" HDR service and not just EV-DO acquisition.
           * The EV-DO network should expect every EV-DO provisioned AT
           * to have a valid session at all times.
           */
          HDR_MSG_PROT (MSG_LEGACY_HIGH,  " HMP: Opening new session ...");
          hdrsmp_activate(HDRHAI_HDR_MESSAGING_PROTOCOL);
        }
        else
        {
          /* Wait until we actually want to use data services before 
           * opening the session.
           */
          HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                        " MUM: data call is required to open session");
        }
      }
      else
      {
        /* If a session is open already... just log the protocol subtype info
           log packet. */
        hdrscmdb_log_pers_subtype_stream_config();
      }


      /* Loop through the RASM message list and resend any messages 
       * that we were trying to send before or during the fade.
       */

      /* First send any messages with bundling required.  Only the first
       * one will be bundled if we have current overhead, unless we change
       * how ACMAC gets messages for the probe when it is first started
       */
      rasm_ptr = (hdrhmp_rasm_buf_type *) q_check( &hdrhmp.rasm_list );

      while (rasm_ptr != NULL)
      {
        ASSERT (rasm_ptr->state == HDRHMP_RASMS_WAITING);

        buf_ptr = &rasm_ptr->buf;

        if (rasm_ptr->buf.msg_cmd.bundle_mask != HDRHMP_BUNDLE_NONE_MSG)
        {
          HDRHMP_RASM_TRACE( ACQ );

          (void) dsmbit_unpack8( buf_ptr->msg_cmd.msg_ptr, &msg_id, 0, 8 );

          (void) rex_clr_timer( &rasm_ptr->interval_timer );

          HDR_MSG_PROT_5 (MSG_LEGACY_HIGH, 
                          "RASM %d ACQ: Resending (P 0x%x, ID 0x%x) (attempt %d of %d)", 
                          rasm_ptr->log_id,
                          buf_ptr->msg_cmd.protocol_name, msg_id,
                          rasm_ptr->attempts + 1,
                          buf_ptr->msg_cmd.rasm.max_retries + 1);

          /* On reacq it is as if all RASM messages were just started,
           * except for the overall timer and attempt count
           */
          HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                          "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                          (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                          (uint32) HDRHMP_RASMS_WAITING | (uint32) HDRHMP_RASMSS_WAITING_HMP_REXMIT);

          rasm_ptr->state = HDRHMP_RASMS_WAITING;
          rasm_ptr->substate = HDRHMP_RASMSS_WAITING_HMP_REXMIT;

          hdrhmp_rasm_retransmit( buf_ptr );
        }
        rasm_ptr = q_next( &hdrhmp.rasm_list, &rasm_ptr->buf.hdr.link );
      }

      /* Then send the other messages */
      rasm_ptr = (hdrhmp_rasm_buf_type *) q_check( &hdrhmp.rasm_list );

      while (rasm_ptr != NULL)
      {
        buf_ptr = &rasm_ptr->buf;

        if (rasm_ptr->buf.msg_cmd.bundle_mask == HDRHMP_BUNDLE_NONE_MSG)
        {
          HDRHMP_RASM_TRACE( ACQ );

          (void) dsmbit_unpack8( buf_ptr->msg_cmd.msg_ptr, &msg_id, 0, 8 );

          (void) rex_clr_timer( &rasm_ptr->interval_timer );

          HDR_MSG_PROT_5 (MSG_LEGACY_HIGH, 
                          "RASM %d ACQ: Resending (P 0x%x, ID 0x%x) (attempt %d of %d)", 
                          rasm_ptr->log_id,
                          buf_ptr->msg_cmd.protocol_name, msg_id,
                          rasm_ptr->attempts + 1,
                          buf_ptr->msg_cmd.rasm.max_retries + 1);

          /* On reacq it is as if all RASM messages were just started,
           * except for the overall timer and attempt count
           */
          HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                          "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                          (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                          (uint32) HDRHMP_RASMS_WAITING | (uint32) HDRHMP_RASMSS_WAITING_HMP_REXMIT);
          rasm_ptr->state = HDRHMP_RASMS_WAITING;
          rasm_ptr->substate = HDRHMP_RASMSS_WAITING_HMP_REXMIT;

          hdrhmp_rasm_retransmit( buf_ptr );
        }
        rasm_ptr = q_next( &hdrhmp.rasm_list, &rasm_ptr->buf.hdr.link );
      }
      break;

    case HDRIND_RUP_CONNECTION_INITIATED:
      /* If we were preparing to start an access attempt when a connection was
       * started, there is no need to continue.
       */
      hdrhmpsa_abort();

      /* Reset bundling mask/flag after receiving TCA, this may not be necessary*/
      hdrhmp_stop_waiting_for_bundling_msg();
      break;

    case HDRIND_RTCMAC_LINK_ACQUIRED:
    case HDRIND_SAP_ALT_LINK_COMP_CB_RECEIVED:
      /* Send pending messages
      */
      if (
#ifdef FEATURE_LTE_TO_HDR_OH
           ( tunnel_mode_is_enabled &&
             hdrsap_alt_link_open_comp_tx_cb_is_received()) ||
           ( !tunnel_mode_is_enabled &&
#endif /* FEATURE_LTE_TO_HDR_OH */                      
             hdralmp_connection_is_open( HDRHAI_HDR_MESSAGING_PROTOCOL ) 
#ifdef FEATURE_LTE_TO_HDR_OH
           )
#endif /* FEATURE_LTE_TO_HDR_OH */  
         )
      {
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "msg in pending queue:%d",
                        q_cnt(&hdrhmp.pending_msg_q));
        while ( (buf_ptr =
               (hdrhmp_msg_buf_type *) q_get( &hdrhmp.pending_msg_q )) != NULL)
        {
          ASSERT(( buf_ptr->msg_cmd.chan == HDRHAI_RTC ) ||
                 ( buf_ptr->msg_cmd.chan == HDRHAI_AC_OR_RTC));

          if (buf_ptr->msg_cmd.rasm.is_enabled)
          {
            ASSERT (buf_ptr->msg_cmd.outcome_cb == hdrhmp_rasm_cb);
            rasm_ptr = (hdrhmp_rasm_buf_type *) 
                        buf_ptr->msg_cmd.outcome_tag_ptr;

            HDRHMP_RASM_TRACE( PEND );

            /* We shouldn't have msg in pending_msg_q with DONE state because
             * when the msg is no longer needed and is in PENDING state,
             * we delete it from both pending_msg_q and rasm list, and call user cb.
             */
            ASSERT (rasm_ptr->state == HDRHMP_RASMS_PENDING); 

            (void) dsmbit_unpack8( buf_ptr->msg_cmd.msg_ptr, &msg_id, 0, 8 );

            HDR_MSG_PROT_5 (MSG_LEGACY_MED, 
                            "RASM %d PEND: Send Pending (P 0x%x, ID 0x%x) (attempt %d of %d)", 
                            rasm_ptr->log_id,
                            buf_ptr->msg_cmd.protocol_name, msg_id,
                            rasm_ptr->attempts + 1,
                            buf_ptr->msg_cmd.rasm.max_retries + 1);

              /* free the message in the pending message queue then call hdrhmp_rasm_retransmit
               * to retransmit rasm message
               * before calling dsm_free_packet, msg_ptr is stored in both pending message queue and rasm list
               * after calling dsm_free_packet, msg_ptr is stored in rasm list only
               */
              dsm_free_packet( &buf_ptr->msg_cmd.msg_ptr );

              HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                              "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                              (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                              (uint32) HDRHMP_RASMS_WAITING | (uint32) HDRHMP_RASMSS_WAITING_HMP_REXMIT);

              (void) rex_clr_timer( &rasm_ptr->interval_timer );

              rasm_ptr->state = HDRHMP_RASMS_WAITING;
              rasm_ptr->substate = HDRHMP_RASMSS_WAITING_HMP_REXMIT;

              hdrhmp_rasm_retransmit( &rasm_ptr->buf );

          } /* End rasm.is_enabled */
          else
          {
            hdrhmp_process_send_msg_cmd( &buf_ptr->msg_cmd );
          }
          modem_mem_free( buf_ptr, MODEM_MEM_CLIENT_HDR_CRIT );
        }
      }
      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "msg in pending queue:%d",
                      q_cnt(&hdrhmp.pending_msg_q));
      break;

    case HDRIND_IDLE_CONNECTION_FAILED:
    case HDRIND_ALMP_RETURNING_TO_IDLE:
#ifdef FEATURE_LTE_TO_HDR_OH            
    case HDRIND_SAP_RETURNING_TO_CLOSE:
#endif /* FEATURE_LTE_TO_HDR_OH */
      /* HDRIND_ALMP_RETURNING_TO_IDLE is checked here because we delay the
       * processing of the pending messages until the TC Complete message is
       * received.  If we return to idle before the message is received we
       * don't want buffers left on the pending_msg_q.
       */
      hdrhmp_clear_pending_msg_queue(E_AGAIN);

      if ((q_cnt(&hdrhmp.ac_msg_list) != 0) ||
          (q_cnt(&hdrhmp.rtc_msg_list) != 0))
      {
        /* One scenario is supervision failure while probing, where ALMP
         * deactivates IDLE before giving HDRIND_ALMP_REACQUIRING, so the
         * callback for the message being probed doesn't get called until
         * after we come here, so the messge is still on the ac_msg_list.
         */
        HDR_MSG_PROT_2 (MSG_LEGACY_HIGH,
                        "%d AC, %d RTC msgs still awaiting callback",
                        q_cnt(&hdrhmp.ac_msg_list),
                        q_cnt(&hdrhmp.rtc_msg_list));

        /* If a buffer is left on the ac_msg_list, get it and print info */
        if ( ( buf_ptr = ( hdrhmp_msg_buf_type * )
               q_check( &hdrhmp.ac_msg_list ) ) != NULL )
        {
          /* Extract the msg_id for debug purposes only if we fail to extract the
           * msg_id, the only consequence is an incorrect debug message.
           */
          if (buf_ptr->msg_cmd.msg_ptr != NULL)
          {
            (void) dsmbit_unpack8( buf_ptr->msg_cmd.msg_ptr, &msg_id, 0, 8 );
          }
          else
          {
            HDR_MSG_PROT (MSG_LEGACY_FATAL, "AC buf has no message");
            msg_id = 0;
          }
          HDR_MSG_PROT_2 (MSG_LEGACY_FATAL, "AC pid 0x%x, mid 0x%x",
                          buf_ptr->msg_cmd.protocol_name, msg_id);
        }
      }
      break;

    case HDRIND_ALMP_REACQUIRING:
    case HDRIND_ALMP_EXIT_HDR_MODE:
      /* HMP only starts to set up an access attempt if a system is acquired.
       * ALMP gives one of these two indications if it leaves the acquired
       * system, so by aborting the setup here we are sure to not leave an
       * access attempt setup in progress when it should not be.
       */
      hdrhmpsa_abort();
      if ( q_cnt(&hdrhmp.rtc_msg_list) != 0 )
      {
        /* Ideally SLP processes REACQUIRING or EXIT_HDR_MODE indication first.
         * There should be no msg left on RTC list by now. However, if HDRMC
         * and HDRTX task are running in two parallel threads, it is possible 
         * RTC list is not empty when HMP starts processing the indication. 
         * But eventually SLP will clear up the RCT list. */
        HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                        "%d RTC msgs still awaiting callback",
                        q_cnt(&hdrhmp.rtc_msg_list) );
      }
      hdrhmp_clear_pending_msg_queue(E_CANCELED);
      hdrhmp_stop_waiting_for_bundling_msg();
      break;

    case HDRIND_LMAC_ACCESS_MODE_ENTERED:
      hdrhmpsa_lmac_ready();
      break;

    case HDRIND_SCM_SESSION_CHANGED:
      if ( (ind_data_ptr->change_reason == HDRHAI_SCHG_OPENED_NORMAL) ||
           (ind_data_ptr->change_reason == HDRHAI_SCHG_OPENED_RESTORE_PRIOR) ||
           (ind_data_ptr->change_reason == HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS)
         )
      {
        /* Received session changed indication upon opening a new or previous
         * session. This indicates that session negotiation has succeeded. Assume
         * for now that the session opened successfully, but if we later fail
         * session negotiation, we will change this. */
        hdrhmp.sclose.session_opened_successfully = TRUE;
      }
      break;

    case HDRIND_SMP_SESSION_CLOSED:
      ASSERT(ind_data_ptr != NULL);
      hdrhmp_process_session_closed( ind_data_ptr->close_reason );
      break;

    case HDRIND_IDLE_CONNECTION_ABORTED:
    case HDRIND_LMAC_ACCESS_ENTRY_HANDOFF:
    case HDRIND_ACMAC_TX_ENDED:
      /* It is possible that HMPSA is in the middle of starting an access
       * attempt when an Access Handoff (AHO) occurs.  In this case 
       * HDRSRCH returns to idle mode, so HMPSA must start over to ensure
       * HDRSRCH is back in access mode before the ACMAC is commanded
       * to start probing again.
       */
	  hdrhmpsa_reset_amac_given_attempt(); /* even if access attempt success/fail
                                              HDRIND_ACMAC_TX_ENDED is recieved */
      hdrhmpsa_restart();
      break;

    case HDRIND_ALMP_CONNECTION_CLOSED:
      /* Loop through the RASM message list and clean up any messages that 
       * were waiting for a L3 ACK or Configuration Unlock, because those
       * events will not happen now that the connection is closed.
       */
      rasm_ptr = (hdrhmp_rasm_buf_type *) q_check( &hdrhmp.rasm_list );

      while (rasm_ptr != NULL)
      {
        buf_ptr = &rasm_ptr->buf;

        /* Get next entry now in case we delete the current one */
        next_ptr = q_next( &hdrhmp.rasm_list, &buf_ptr->hdr.link );

        if (rasm_ptr->state == HDRHMP_RASMS_WAITING) 
        {
          if ( (!hdralmp_system_is_acquired( HDRHAI_HDR_MESSAGING_PROTOCOL ) ||
              (rasm_ptr->substate == HDRHMP_RASMSS_WAITING_HMP_REXMIT)))
          {
            /* do nothing here
             * RAMS message is either already in the retransmit queue or 
             * it will be retransmitted after re-acq
             */
          }
          else if ((rasm_ptr->substate == HDRHMP_RASMSS_WAITING_L3_ACK) &&
                   (buf_ptr->msg_cmd.chan == HDRHAI_AC_OR_RTC) &&
                   (rasm_ptr->attempts <= buf_ptr->msg_cmd.rasm.max_retries))
          {
            /* The check for AC_OR_RTC covers the case of a 
             * ReservationOnRequest with bundle_mask == HDRHMP_BUNDLE_NONE_MSG, which
             * is a RTC only message.  E_AGAIN is not intercepted by RASM for
             * RTC only messages, so it will be handled by the else case below.
             */

            HDRHMP_RASM_TRACE( CC );

            (void) dsmbit_unpack8( buf_ptr->msg_cmd.msg_ptr, &msg_id, 0, 8 );

            HDR_MSG_PROT_5 (MSG_LEGACY_HIGH, 
                            "RASM %d CC: Resending (P 0x%x, ID 0x%x) (attempt %d of %d)", 
                            rasm_ptr->log_id,
                            buf_ptr->msg_cmd.protocol_name, msg_id,
                            rasm_ptr->attempts + 1,
                            buf_ptr->msg_cmd.rasm.max_retries + 1);

            HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                            "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                            (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                            (uint32) rasm_ptr->state | (uint32) HDRHMP_RASMSS_WAITING_HMP_REXMIT);

            (void) rex_clr_timer( &rasm_ptr->interval_timer );

            rasm_ptr->substate = HDRHMP_RASMSS_WAITING_HMP_REXMIT;

            hdrhmp_rasm_retransmit( buf_ptr );
          }
          else
          {
            /* 1. RASM is in HDRHMP_RASMSS_WAITING_CONFIG_UNLOCK substate 
             *    after receiving ConnectionClose msg, there is no way to 
             *    receive UnlockConfiguration msg because it's FTC only. 
             * 2. Last interval expired, so the message has failed. 
             */
            HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                            "RASM %d CC: Removing (P 0x%x, ID 0x%x)", 
                            rasm_ptr->log_id,
                            buf_ptr->msg_cmd.protocol_name, msg_id);

            /* Remove item from list. */
            q_delete( &hdrhmp.rasm_list, &buf_ptr->hdr.link );
            hdrhmp_delete_rasm_entry( rasm_ptr, E_AGAIN );
          }
        }
        rasm_ptr = next_ptr;
      }
      break;

    case HDRIND_SCP_CONFIG_UNLOCKED:
      /* Loop through the RASM message list and resend any messages 
       * that we were waiting for the AN to allow GAUP again.
       */

      rasm_ptr = (hdrhmp_rasm_buf_type *) q_check( &hdrhmp.rasm_list );

      while (rasm_ptr != NULL)
      {
        buf_ptr = &rasm_ptr->buf;

        if ((rasm_ptr->state == HDRHMP_RASMS_WAITING) &&
            (rasm_ptr->substate == HDRHMP_RASMSS_WAITING_CONFIG_UNLOCK))
        {
          HDRHMP_RASM_TRACE( UNLK );

          (void) dsmbit_unpack8( buf_ptr->msg_cmd.msg_ptr, &msg_id, 0, 8 );

          HDR_MSG_PROT_5 (MSG_LEGACY_HIGH, 
                          "RASM %d UNLK: Resending (P 0x%x, ID 0x%x) (attempt %d of %d)", 
                          rasm_ptr->log_id,
                          buf_ptr->msg_cmd.protocol_name, msg_id,
                          rasm_ptr->attempts + 1,
                          buf_ptr->msg_cmd.rasm.max_retries + 1);

          HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                          "RASM %d: Change state [%x->%x]", rasm_ptr->log_id,
                          (uint32) rasm_ptr->state | (uint32) rasm_ptr->substate,
                          (uint32) rasm_ptr->state | (uint32) HDRHMP_RASMSS_WAITING_HMP_REXMIT);

          rasm_ptr->substate = HDRHMP_RASMSS_WAITING_HMP_REXMIT;

          hdrhmp_rasm_retransmit( buf_ptr );
        }
        
        rasm_ptr = q_next( &hdrhmp.rasm_list, &rasm_ptr->buf.hdr.link );
      }
      break;

    default:
      ERR_FATAL( "Unknown indication to HMP %d", ind_name, 0, 0 );
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrhmp_process_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_PROCESS_SIG

DESCRIPTION
  This function processes signals for HMP.

DEPENDENCIES
  Only one signal is sent at a time, so only one bit of the sig input
  parameter is set.  If multiple signals need to be delivered, they must
  be delivered by multiple calls.

PARAMETERS
  sig - The signal that belongs to this protocol and was set.  This is not
        a signal mask.  Only one bit of the value is set

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_process_sig( rex_sigs_type sig )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "Signal %d received", sig);

  ERR_FATAL( "Unknown signal to HMP %d", sig, 0, 0 );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrhmp_process_sig() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_RASM_ACKED

DESCRIPTION
  This function is called by the application layer to n	acknowledge the
  receipt of the layer 3 resposne to the best effort message sent that 
  required more than zero retries.  For example, if ReservationOnRequest is 
  sent by protocol HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL with transaction_id 1
  requiring 3 retries at 1s intervals, HMP will try 3 times until
  MRLP calls this function to report that transaction_id 1 has been acked,
  which would include MRLP receiving the ReservationAccept or ReservationReject
  message.

DEPENDENCIES
  Each protocol with a unique hdrhai_protocol_name_enum_type must ensure the 
  transaction_id is unique.  In other words, two outstanding best effort
  messages (regardless of MessageID) sent by the same protocol with retries 
  requested should not have the same transaction_id.

PARAMETERS
  protocol_name  - Name of Protocol sending the message.
  transaction_id - For app to inform HMP that layer 3 ACK was received.
                   TransactionID is per protocol, not per message!
RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_rasm_acked
(
  hdrhai_protocol_name_enum_type name,
  uint32 transaction_id
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdrhmp_cmd_type hdrhmp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Always queue HDRHMP_RASM_ACKED_CMD command because we already always
   * queue HDRHMP_SEND_MSG commands so we can't risk getting the ACK
   * before we prodess the command to send it.
   */
  hdrhmp_cmd.name = HDRHMP_RASM_ACKED_CMD;
  hdrhmp_cmd.sender = name;
  hdrhmp_cmd.params.rasm_acked.protocol_name = name;
  hdrhmp_cmd.params.rasm_acked.transaction_id = transaction_id;
  HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"hdrhmp_rasm_acked txid 0x%x",hdrhmp_cmd.params.rasm_acked.transaction_id);
  hdrmc_queue_cmd( HDRHAI_HDR_MESSAGING_PROTOCOL, &hdrhmp_cmd,
                   sizeof( hdrhmp_cmd_type ) );

} /* hdrhmp_rasm_acked() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_GENERIC_SEND_MSG

DESCRIPTION
  This helper function is called by HMP send message wrapper functions so
  protocols at all layers could send a signaling message to their peer.
  The command is always queued and processed later to prevents complicated
  threads of processing that otherwise occur when protocols in the HDRMC
  task send messages in the middle of processing other events.

  This function can be used to send messages for any Stream - 0, 1, 2 or 3.

DEPENDENCIES
  This function may only be called from within HDRHMP

PARAMETERS
  protocol_name - Name of Protocol sending the message.
  stream - Stream associated with protocol
  chan - Channel the message must go out on (AC or RTC)
  rtc_is_reliable - Whether or not the message is reliable
  send_on_curr_rtc_only - Whether or not the message should only be sent on
                          the existing RTC
  priority - Priority of the message, as defined in the HAI
  protocol_instance - in_use or in_config
  rasm_info_ptr - More info for sending RASM messages
  *msg_ptr - The message to be sent
  outcome_cb - Pointer to function called to announce outcome of
                    transmission
  *outcome_tag_ptr - A pointer owned by sender, returned with the callback

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
static void hdrhmp_generic_send_msg
(
  hdrhai_protocol_name_enum_type name,
  hdrhai_stream_enum_type stream,
  hdrhai_channel_enum_type chan,
  boolean rtc_is_reliable,
  boolean send_on_curr_rtc_only,
  uint8 priority,
  hdrhai_protocol_instance_enum_type protocol_instance,
  hdrhmp_rasm_info_type const *rasm_info_ptr,
  uint32 bundle_mask,
  dsm_item_type * msg_ptr,
  hdrslp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdrhmp_cmd_type hdrhmp_cmd;

  hdrhmp_send_msg_params_type *params_ptr; /* Convenience ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT (MSG_LEGACY_LOW,  "hdrhmp_send_msg");

  /* ASSERT that all parameters given are within their permissible range */
  ASSERT(( name < HDRHAI_PROTOCOL_NAME_ENUM_MAX ) &&
      ( chan == HDRHAI_AC || chan == HDRHAI_RTC || chan == HDRHAI_AC_OR_RTC) &&
      ( rtc_is_reliable == TRUE || rtc_is_reliable == FALSE ) &&
      ( send_on_curr_rtc_only == TRUE || send_on_curr_rtc_only == FALSE ) &&
      (!(send_on_curr_rtc_only == TRUE && chan != HDRHAI_RTC)) &&
      ( protocol_instance == HDRHAI_IN_USE ||
      protocol_instance == HDRHAI_IN_CONFIG ) && 
      /*lint -e{685} suppress comparision always true */
      ( stream <= HDRHAI_STREAM3 ));

  params_ptr = &hdrhmp_cmd.params.send_msg;

  /* Fill in all members of the hdrhmp_cmd_type for the message to be sent */
  hdrhmp_cmd.name = HDRHMP_SEND_MSG_CMD;
  hdrhmp_cmd.sender = name;
  params_ptr->protocol_name = name;
  params_ptr->chan = chan;
  params_ptr->rtc_is_reliable = rtc_is_reliable;
  params_ptr->send_on_curr_rtc_only = send_on_curr_rtc_only;
  params_ptr->priority = priority;
  params_ptr->protocol_instance = protocol_instance;

  if (rasm_info_ptr == NULL)
  {
    params_ptr->rasm.is_enabled = FALSE;
  }
  else
  {
    params_ptr->rasm.is_enabled = TRUE;
    params_ptr->rasm.is_rexmit = rasm_info_ptr->is_rexmit;
    params_ptr->rasm.self_ptr = rasm_info_ptr->self_ptr;
    params_ptr->rasm.max_retries = rasm_info_ptr->max_retries;
    params_ptr->rasm.retry_interval = rasm_info_ptr->retry_interval;
    params_ptr->rasm.overall_timeout = rasm_info_ptr->overall_timeout;
    params_ptr->rasm.transaction_id = rasm_info_ptr->transaction_id;
    params_ptr->rasm.is_implicit_on_connection_setup = rasm_info_ptr->is_implicit_on_connection_setup;
  }

  params_ptr->bundle_mask = bundle_mask;
  params_ptr->msg_ptr = msg_ptr;
  params_ptr->outcome_cb = outcome_cb;
  params_ptr->outcome_tag_ptr = outcome_tag_ptr;
  params_ptr->stream = stream;

  /* Always queue messages to be sent, to avoid re-entering this function
   * in the event that sending one message causes the sending of another.
   */
  hdrmc_queue_cmd( HDRHAI_HDR_MESSAGING_PROTOCOL, &hdrhmp_cmd,
                   sizeof( hdrhmp_cmd_type ) );
  /* Flag that we have queued a messge in case IDLE wants to know if
   * it can sleep (it can't sleep if we have a message to send)
   */
  hdrhmp_msgs_on_event_q( +1 );
} /* hdrhmp_generic_send_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_SEND_MSG

DESCRIPTION
  This function is called by protocols at all layers to send a signaling
  message to their peer.  The command is always queued and processed later,
  even if the sending protocol is in the task context of HMP.  This prevents
  complicated threads of processing that otherwise occur when protocols in
  the HDRMC task send messages in the middle of processing other events.

  This function should only be called for Stream 0 Application.
  Stream 1, 2 and 3 Application protocols should not call this function. They
  should use hdrhmp_app_send_msg() instead.


DEPENDENCIES
  None

PARAMETERS
  protocol_name - Protocol sending the message
  chan - Channel the message must go out on (AC or RTC)
  rtc_is_reliable - Whether or not the message is reliable
  priority - Priority of the message, as defined in the HAI
  *msg_ptr - The message to be sent
  outcome_cb - Pointer to function called to announce outcome of
                    transmission
  *outcome_tag_ptr - A pointer owned by sender, returned with the callback

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_send_msg
(
  hdrhai_protocol_name_enum_type name,
  hdrhai_channel_enum_type chan,
  boolean rtc_is_reliable,
  uint8 priority,
  hdrhai_protocol_instance_enum_type protocol_instance,
  dsm_item_type * msg_ptr,
  hdrslp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
)
{

  /* Call hdrhmp_generic_send_msg() with the Stream set to STREAM 0 */
  hdrhmp_generic_send_msg( name,
                           HDRHAI_STREAM0,
                           chan,
                           rtc_is_reliable,
                           FALSE, /* send_on_curr_rtc_only = FALSE */
                           priority,
                           protocol_instance,
                           NULL, /* not using RASM */
                           HDRHMP_BUNDLE_NONE_MSG, /* bundle msg is not needed */
                           msg_ptr,
                           outcome_cb,
                           outcome_tag_ptr );

} /* hdrhmp_send_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_SEND_MSG_ON_EXISTING_RTC

DESCRIPTION
  This function is called by protocols at all layers to send a signaling
  message to their peer.  The command is always queued and processed later,
  even if the sending protocol is in the task context of HMP.  This prevents
  complicated threads of processing that otherwise occur when protocols in
  the HDRMC task send messages in the middle of processing other events.

  This function should only be called for sending RTC only messages
  the existing RTC channel.  That is if the connection is closed before
  the message can be sent out, then the message will be dropped and HMP will
  not attempt to open a new connection to send that message out.

  hdrhmp_send_msg() should be used instead for RTC only messages requiring that
  a new connection be opened to send the message out (Config messages, etc)

  This function should only be called for Stream 0 application
  Stream 1, 2 and 3 Application protocols should not call this function. They
  should use hdrhmp_app_send_msg_on_existing_rtc() instead.


DEPENDENCIES
  None

PARAMETERS
  protocol_name - Protocol sending the message
  rtc_is_reliable - Whether or not the message is reliable
  priority - Priority of the message, as defined in the HAI
  *msg_ptr - The message to be sent
  outcome_cb - Pointer to function called to announce outcome of
               transmission
  *outcome_tag_ptr - A pointer owned by sender, returned with the callback

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_send_msg_on_existing_rtc
(
  hdrhai_protocol_name_enum_type name,
  boolean rtc_is_reliable,
  uint8 priority,
  hdrhai_protocol_instance_enum_type protocol_instance,
  dsm_item_type * msg_ptr,
  hdrslp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
)
{

  /* Call hdrhmp_generic_send_msg() with the Stream set to STREAM 0 and the
     send_on_curr_rtc_only field set to TRUE */
  hdrhmp_generic_send_msg( name,
                           HDRHAI_STREAM0,
                           HDRHAI_RTC,
                           rtc_is_reliable,
                           TRUE, /* send_on_curr_rtc_only */
                           priority,
                           protocol_instance,
                           NULL, /* not using RASM */
                           HDRHMP_BUNDLE_NONE_MSG, /* bundle msg is not needed */
                           msg_ptr,
                           outcome_cb,
                           outcome_tag_ptr );

} /* hdrhmp_send_msg_on_existing_rtc() */

/*===========================================================================

FUNCTION HDRHMP_APP_SEND_RASM

DESCRIPTION

  RASM = Reliable Application Signaling Message 

  Even best effort SLP messages can be sent using RASM
  
  This API allows for application layer protocols to send IS-856 air interface
  signaling messages more reliably in poor RF conditions.  HMP will continue
  trying to send the message for 40s (a value that could be passed in if 
  needed).  If it times out, the outcome_cb is called with E_TIMER_EXP.

  The callback given by the sender may be called with the following reasons:
  E_SUCCESS     - a) ACAck was receive for AC transmission
                  b) RTC frame was sent with message with max_retries = 0
                  c) hdrhmp_rasm_acked( transaction_id ) was called for 
                     RTC message with max_retries > 0  
                     
  E_FAILURE     - a) Max access probes were attempted on the access channel
                  b) If max_retries > 0, max_retries + 1 attempts failed
                  c) Other unexpected failures (no session open at time of 
                     transmission, Session closed during transmission, 
                     AT enters PWR_SAVE during transmission)
                     
  E_TIMER_EXP   - 40s overall per-message timer expires
  
  E_PROT_Q_FULL - There were no buffers to send the message (unexpected)

  The transaction_id is per protocol, not per message.   
  it must meet the following requirements:
  1.  It can not be set to HDRHMP_RASM_TRANSID_NONE (0xFFFFFFFF)
  2.  The (protocol_name, transaction_id) key must be unique among all 
      messages sent with max_retries > 0 that are still outstanding, which 
      includes the time between when hdrhmp_rasm_acked() is called and when
      the callback is called with E_SUCCESS.
  
DEPENDENCIES
  None

PARAMETERS
  name            - Name of Protocol sending the message
  stream          - Stream associated with protocol
  max_retries     - How many times to try sending the best effort msg
  retry_interval  - Time to wait for layer 3 ACK before retry, In milliseconds
  transaction_id  - For app to inform HMP that layer 3 ACK was received
  *msg_ptr        - The message to be sent
  outcome_cb      - Pointer to function called to announce outcome of
                   transmission
  *outcome_tag_ptr - A pointer owned by sender, returned with the callback

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_app_send_rasm
(
  hdrhai_protocol_name_enum_type name,
  hdrhai_stream_enum_type stream,
  uint32 max_retries,
  uint32 retry_interval,
  uint32 transaction_id,
  dsm_item_type * msg_ptr,
  hdrslp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
)
{
  hdrhmp_rasm_info_type rasm_info; /* Extra info for sending RASM messages */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rasm_info.is_rexmit = FALSE;
  rasm_info.max_retries = max_retries;
  rasm_info.retry_interval = retry_interval;
  rasm_info.overall_timeout = 40000; /* Could be passed in later */
  rasm_info.transaction_id = transaction_id;
  rasm_info.is_implicit_on_connection_setup = FALSE;
  rasm_info.self_ptr = NULL; /* Only populated during retransmissions */

  /* If there are no retries, the transaction_id is still needed, 
   * because we will wait retry_interval for L3Ack before calling outcome_cb */
    ASSERT( transaction_id != HDRHMP_RASM_TRANSID_NONE );

  /* Call hdrhmp_generic_send_msg() with the
     send_on_curr_rtc_only field set to FALSE */
  hdrhmp_generic_send_msg( name,
                           stream,
                           HDRHAI_AC_OR_RTC,
                           FALSE,
                           FALSE, /* send_on_curr_rtc_only = FALSE */
                           40,
                           HDRHAI_IN_USE,
                           &rasm_info,
                           HDRHMP_BUNDLE_NONE_MSG, /* bundle msg is not needed */
                           msg_ptr,
                           outcome_cb,
                           outcome_tag_ptr );

} /* hdrhmp_app_send_rasm() */

/*===========================================================================

FUNCTION HDRHMP_APP_SEND_RASM_QOS_REQUEST

DESCRIPTION

  RASM = Reliable Application Signaling Message 

  This API allows for MFPA and EMPA to send the AttributeUpdateRequest for 
  ReservationKKQoSRequest attribute reliably.

  The callback given by the sender may be called with the following outcomes:

  E_SUCCESS - SLP Ack was received from the AN in response to transmission 
              on the RTC

  E_FAILURE - a) All reliable delivery SLP retries were attempted and failed
              b) Other unexpected failures (no session open at time of 
                 transmission, Session closed during transmission, AT enters 
                 PWR_SAVE during transmission) 

  E_AGAIN -   a) The message was held pending connection setup and the 
                 connection setup failed.
              b) The connection closed before all 3 SLP retries were attempted

  E_TIMER_EXP - Overall message timer expired (40s)

  E_PROT_Q_FULL - No buffers for the message (unexpected)

DEPENDENCIES
  None

PARAMETERS
  name             - Name of Protocol sending the message
  stream           - Stream associated with protocol
  *msg_ptr         - The message to be sent
  outcome_cb       - Pointer to function called to announce outcome of
                     transmission
  *outcome_tag_ptr - A pointer owned by sender, returned with the callback

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_app_send_rasm_qos_request
(
  hdrhai_protocol_name_enum_type name,
  hdrhai_stream_enum_type stream,
  dsm_item_type * msg_ptr,
  hdrslp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
)
{
  hdrhmp_rasm_info_type rasm_info; /* Extra info for sending RASM messages */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rasm_info.is_rexmit = FALSE;
  rasm_info.max_retries = 0;
  rasm_info.retry_interval = 0;
  rasm_info.overall_timeout = 40000;
  rasm_info.transaction_id = HDRHMP_RASM_TRANSID_NONE;
  rasm_info.is_implicit_on_connection_setup = FALSE;
  rasm_info.self_ptr = NULL; /* Only populated during retransmissions */

  /* Call hdrhmp_generic_send_msg() with the
     send_on_curr_rtc_only field set to FALSE */
  hdrhmp_generic_send_msg( name,
                           stream,
                           HDRHAI_RTC,
                           TRUE, /* AttributeUpdateRequest is reliable */
                           FALSE, /* send_on_curr_rtc_only = FALSE */
                           40, /* AttributeUpdateRequest is priority 40 */
                           HDRHAI_IN_USE,
                           &rasm_info,
                           HDRHMP_BUNDLE_NONE_MSG, /* bundle msg is not needed */
                           msg_ptr,
                           outcome_cb,
                           outcome_tag_ptr );

} /* hdrhmp_app_send_rasm_qos_request() */

/*===========================================================================

FUNCTION HDRHMP_APP_SEND_RASM_QOS_RESUME

DESCRIPTION

  RASM = Reliable Application Signaling Message 

  This API allows MFPA or EMPA to send the ReservationOnRequest message
  reliably.

  The callback given by the sender may be called with the following outcomes:

E_SUCCESS     - RoAccept or RoReject was received and this module called
                hdrhmp_rasm_acked().  This callback is just a formality.

E_FAILURE     - a) max_retries were all attempted with no layer 3 response
                b) max access probes were attempted on the AC (either to send
                   RoR or to send CReq if RoR was not being bundled)
                c) others - session close, no session, PWR_SAVE   

E_AGAIN       - If bundle_mask != BUNDLE_NONE, this is an AC_OR_RTC message and 
                E_AGAIN is intercepted and retried by RASM.  Otherwise it is 
                returned to the caller here.
                a) Connection setup failed due to no TCA
                b) Connection setup failed due to no RTCAck
                c) Connection closes before all retries are attempted

E_TIMER_EXP   - Total time to send message exceeded 40s

E_PROT_Q_FULL - There were no buffers to send the message

  The transaction_id is per protocol, not per message.  
  it must meet the following requirements:
  1.  It can not be set to HDRHMP_RASM_TRANSID_NONE (0xFFFFFFFF)
  2.  The (protocol_name, transaction_id) key must be unique among all 
      messages sent with max_retries >= 0 that are still outstanding, which 
      includes the time between when hdrhmp_rasm_acked() is called and when
      the callback is called with E_SUCCESS.
  
DEPENDENCIES
  None

PARAMETERS
  name            - Name of Protocol sending the message
  stream          - Stream associated with protocol
  max_retries     - How many times to retry the best effort msg if no layer3 Ack
  retry_interval  - Time to wait for layer 3 ACK before retry, In milliseconds
  transaction_id  - For app to inform HMP that layer 3 ACK was received
  bundle_mask     - message bundle mask. 
  is_implicit_on_connection_setup - TRUE if ReservationKKIdleState == 2, FALSE otherwise
                                    if this argument is set to TRUE,
                                    if bundle_mask is set to BUNDLE_NONE, it 
                                    needs to be set to BUNDLE_WITH_CR 
  *msg_ptr        - The message to be sent
  outcome_cb      - Pointer to function called to announce outcome of
                    transmission
  *outcome_tag_ptr - A pointer owned by sender, returned with the callback

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_app_send_rasm_qos_resume
(
  hdrhai_protocol_name_enum_type name,
  hdrhai_stream_enum_type stream,
  uint32 max_retries,
  uint32 retry_interval,
  uint32 transaction_id,
  hdrhmp_bundle_preference_enum_type bundle_preference,
  boolean is_implicit_on_connection_setup,
  dsm_item_type * msg_ptr,
  hdrslp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
)
{
  hdrhai_channel_enum_type chan = HDRHAI_RTC; /* unless it is bundled */
  hdrhmp_rasm_info_type rasm_info; /* Extra info for sending RASM messages */
  uint32 bundle_mask = HDRHMP_BUNDLE_NONE_MSG;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(( msg_ptr != NULL ) &&
         ( max_retries != 0 ) &&
         ( transaction_id != HDRHMP_RASM_TRANSID_NONE ));
  bundle_mask = bundle_preference;

  rasm_info.is_rexmit = FALSE;
  rasm_info.max_retries = max_retries;
  rasm_info.retry_interval = retry_interval;
  rasm_info.overall_timeout = 40000; /* 40s, could be a parameter. */
  rasm_info.transaction_id = transaction_id;
  rasm_info.is_implicit_on_connection_setup = is_implicit_on_connection_setup;
  rasm_info.self_ptr = NULL; /* Only populated during retransmissions */

  if (is_implicit_on_connection_setup)     
  {
    /* Special case for ReservationKKIdleState == 2.  It is the same as 
     * a bundled ReservationOnRequest except there is no actual message
     * sent if a connection setup is needed.
     */
    if (bundle_mask == HDRHMP_BUNDLE_NONE_MSG)
    {
      bundle_mask |= HDRHMP_BUNDLE_CR_MSG;
    }

    chan = HDRHAI_AC_OR_RTC;                                             
  }
  else if (bundle_mask != HDRHMP_BUNDLE_NONE_MSG)
  {
    /* If bundling is desired, the channel is AC_OR_RTC but if sent on
     * AC a connection setup is also initiated at the same time.
     */
    chan = HDRHAI_AC_OR_RTC;
  }

  /* Call hdrhmp_generic_send_msg() with the
     send_on_curr_rtc_only field set to FALSE */
  hdrhmp_generic_send_msg( name,
                           stream,
                           chan,
                           FALSE, /* ReservationOnRequest isn't reliable */
                           FALSE, /* send_on_curr_rtc_only = FALSE */
                           39, /* ReservationOnRequest is priority 40,
                                * in the case of SLP queue contains many 
                                * priority 40 AC msg(for example, HWIDRsp,
                                * LocationNotification), ROR may not be bundled 
                                * with CR. 
                                * To make sure that ROR will be bundled with CR
                                * we change priority to 39.*/  
                           HDRHAI_IN_USE,
                           &rasm_info,
                           bundle_mask, /* bundle msg */
                           msg_ptr,
                           outcome_cb,
                           outcome_tag_ptr );

} /* hdrhmp_app_send_rasm_qos_resume() */

/*===========================================================================

FUNCTION HDRHMP_RASM_RETRANSMIT

DESCRIPTION
  
DEPENDENCIES
  None

PARAMETERS
  buf_ptr - RASM message to retransmit

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
static void hdrhmp_rasm_retransmit
(
  hdrhmp_msg_buf_type *buf_ptr
)
{
  hdrhmp_rasm_info_type rasm_info; /* Extra info for sending RASM messages */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( buf_ptr->msg_cmd.rasm.is_enabled );

  rasm_info.is_rexmit = TRUE;
  rasm_info.max_retries = buf_ptr->msg_cmd.rasm.max_retries;
  rasm_info.retry_interval = buf_ptr->msg_cmd.rasm.retry_interval;
  rasm_info.overall_timeout = buf_ptr->msg_cmd.rasm.overall_timeout;
  rasm_info.transaction_id = buf_ptr->msg_cmd.rasm.transaction_id;
  rasm_info.is_implicit_on_connection_setup = buf_ptr->msg_cmd.rasm.is_implicit_on_connection_setup;
  rasm_info.self_ptr = buf_ptr->msg_cmd.rasm.self_ptr;

  /* Call hdrhmp_generic_send_msg() with the
     send_on_curr_rtc_only field set to FALSE */
  hdrhmp_generic_send_msg( buf_ptr->msg_cmd.protocol_name,    
                           buf_ptr->msg_cmd.stream,           
                           buf_ptr->msg_cmd.chan,             
                           buf_ptr->msg_cmd.rtc_is_reliable,  
                           buf_ptr->msg_cmd.send_on_curr_rtc_only,         
                           buf_ptr->msg_cmd.priority,         
                           buf_ptr->msg_cmd.protocol_instance,
                           &rasm_info,
                           buf_ptr->msg_cmd.bundle_mask,
                           buf_ptr->msg_cmd.msg_ptr,          
                           buf_ptr->msg_cmd.outcome_cb,       
                           buf_ptr->msg_cmd.outcome_tag_ptr ); 
} /* hdrhmp_rasm_retransmit() */


/*===========================================================================

FUNCTION HDRHMP_APP_SEND_MSG

DESCRIPTION
  This function is called by protocols at all layers to send a signaling
  message to their peer. The command is always queued and processed later,
  even if the sending protocol is in the task context of HMP.  This prevents
  complicated threads of processing that otherwise occur when protocols in
  the HDRMC task send messages in the middle of processing other events.

  This function can be used to send messages for any Stream - 0, 1, 2 or 3.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Name of Protocol sending the message.
  stream - Stream associated with protocol
  chan - Channel the message must go out on (AC or RTC)
  rtc_is_reliable - Whether or not the message is reliable
  priority - Priority of the message, as defined in the HAI
  *msg_ptr - The message to be sent
  outcome_cb - Pointer to function called to announce outcome of
                    transmission
  *outcome_tag_ptr - A pointer owned by sender, returned with the callback

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_app_send_msg
(
  hdrhai_protocol_name_enum_type name,
  hdrhai_stream_enum_type stream,
  hdrhai_channel_enum_type chan,
  boolean rtc_is_reliable,
  uint8 priority,
  hdrhai_protocol_instance_enum_type protocol_instance,
  dsm_item_type * msg_ptr,
  hdrslp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
)
{

  /* Call hdrhmp_generic_send_msg() with the
     send_on_curr_rtc_only field set to FALSE */
  hdrhmp_generic_send_msg( name,
                           stream,
                           chan,
                           rtc_is_reliable,
                           FALSE, /* send_on_curr_rtc_only = FALSE */
                           priority,
                           protocol_instance,
                           NULL, /* not using RASM */
                           HDRHMP_BUNDLE_NONE_MSG, /* bundle msg is not needed */
                           msg_ptr,
                           outcome_cb,
                           outcome_tag_ptr );

} /* hdrhmp_app_send_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_APP_SEND_MSG_ON_EXISTING_RTC

DESCRIPTION
  This function is called by protocols at all layers to send a signaling
  message to their peer.  The command is always queued and processed later,
  even if the sending protocol is in the task context of HMP.  This prevents
  complicated threads of processing that otherwise occur when protocols in
  the HDRMC task send messages in the middle of processing other events.

  This function should only be called for sending RTC only messages
  the existing RTC channel.  That is if the connection is closed before
  the message can be sent out, then the message will be dropped and HMP will
  not attempt to open a new connection to send that message out.

  This function can be used to send messages for any Stream - 0, 1, 2 or 3.

  hdrhmp_app_send_msg() should be used instead for RTC only messages requiring
  that a new connection be opened to send the message out (Config messages, ..)

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Protocol sending the message
  stream - Stream associated with protocol
  rtc_is_reliable - Whether or not the message is reliable
  priority - Priority of the message, as defined in the HAI
  *msg_ptr - The message to be sent
  outcome_cb - Pointer to function called to announce outcome of
               transmission
  *outcome_tag_ptr - A pointer owned by sender, returned with the callback

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_app_send_msg_on_existing_rtc
(
  hdrhai_protocol_name_enum_type name,
  hdrhai_stream_enum_type stream,
  boolean rtc_is_reliable,
  uint8 priority,
  hdrhai_protocol_instance_enum_type protocol_instance,
  dsm_item_type * msg_ptr,
  hdrslp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
)
{

  /* Call hdrhmp_generic_send_msg() with the
     send_on_curr_rtc_only field set to TRUE */
  hdrhmp_generic_send_msg( name,
                           stream,
                           HDRHAI_RTC,
                           rtc_is_reliable,
                           TRUE, /* send_on_curr_rtc_only */
                           priority,
                           protocol_instance,
                           NULL, /* not using RASM */
                           HDRHMP_BUNDLE_NONE_MSG, /* bundle msg is not needed */
                           msg_ptr,
                           outcome_cb,
                           outcome_tag_ptr );

} /* hdrhmp_app_send_msg_on_existing_rtc() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRHMP_ABORT_MSG

DESCRIPTION
  This function is a wrapper for hdrslp_abort_msg.  It is required because
  SLP uses the outcome callback address to identify messages being
  transmitted, and in some cases HMP changes this callback.

DEPENDENCIES
  None

PARAMETERS
  slp_outcome_cb  - Function to be called after message has been
                    transmitted successfully or transmission was aborted
  outcome_tag_ptr - User data for callback function

RETURN VALUE
  Enqueueing status (E_SUCCESS, E_PROT_MSG_Q_FULL)

SIDE EFFECTS

===========================================================================*/

hdrerrno_enum_type hdrhmp_abort_msg
(
  hdrslp_outcome_cb_type     slp_outcome_cb,
  void*                      outcome_tag_ptr
)
{
  hdrhmp_msg_buf_type *buf_ptr;          /* buffer for rtc_msg_list entry  */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_enter_crit_sect( &hdrhmp.crit_sect ); /* Enter critical section, so that no element is dequeued from rtc_msg_list during checking for 
                                               intercepted callback */
  buf_ptr = q_check( &hdrhmp.rtc_msg_list );

  while (buf_ptr != NULL)
  {
    if(( buf_ptr->msg_cmd.outcome_cb == slp_outcome_cb ) &&
       ( buf_ptr->msg_cmd.outcome_tag_ptr == outcome_tag_ptr))
    {
      rex_leave_crit_sect( &hdrhmp.crit_sect ); /* Leave critical section once intercepted callback is found */
      return (hdrslp_abort_msg( hdrhmp_rtc_msg_cb, (void *) buf_ptr ));
    }
    buf_ptr = q_next( &hdrhmp.rtc_msg_list, &buf_ptr->hdr.link );
  }
  
  rex_leave_crit_sect( &hdrhmp.crit_sect ); /* Leave critical section, when there is no intercepted callback found */

  return (hdrslp_abort_msg( slp_outcome_cb, outcome_tag_ptr ));

} /* hdrhmp_abort_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_CHECK_RX_MSG_BUNDLE

DESCRIPTION
  Although not explicitly defined in the standard, sometimes there is a particular
  meaning assigned when two or more messages are sent in the same connection layer
  packet. This function runs in the context of HDRRX to check for meaningful bundling 
  and report it to the lower priority thread (i.e. HDRMC), so that the lower priority 
  thread can know when it gets the first message off its queue that another message 
  was bundled with it and is also waiting in the queue.  
  For example, Connected State protocol can know when it gets a ConnectionClose 
  message that a TCA was bundled, indicating hard handoff.  

DEPENDENCIES
  None

PARAMETERS
  protocol_type    - Protocol the message is for
  msg_id           - Message identifier 

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static void hdrhmp_check_rx_msg_bundle
(
  uint8 protocol_type,   /* Protocol the message is for */
  uint8 msg_id          /* Message identifier */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Case1: CC+TCA */
  if (protocol_type == HDRHAI_CONNECTED_STATE_PROTOCOL && msg_id == 0)
  {
    /* It's Connection Close message */
    hdrpcp_add_msg_mask( HDRPCP_BUNDLE_CON_CLOSE_MSG );
  }
  else if (protocol_type == HDRHAI_ROUTE_UPDATE_PROTOCOL && msg_id == 1)
  {
    /* Increment the number of TCAs received to determine if we need to handle
       the case where the AN sends multiple TCA messages in one bundle */
    hdrrup_increment_tca_count();

    /* Don't need to add msg bit for Case1, but add it anyway*/
    hdrpcp_add_msg_mask( HDRPCP_BUNDLE_TCA_MSG);

    /* It's TCA message, check if CC is also in the current connection layer packet*/
    if ( (HDRPCP_BUNDLE_CON_CLOSE_MSG  & hdrpcp_get_msg_mask() ) ) 
    {
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Conn Close and TCA are bundled.");

      /*TCA and CC are in the same connection layer packet. CC is for HHO*/
      hdrcon_set_hho_is_in_progress_flag( TRUE ); 
    }
  }
  /* End of case1 */

  return;
                                                    
} /* hdrhmp_check_rx_msg_bundle() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_ROUTE_MSG

DESCRIPTION
  This function routes signaling messages to the appropriate protocol.
  This includes use of a table where each protocol registers the function
  it wishes to be called to process messages for it.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrhmp_route_msg( dsm_item_type * item_ptr )
{
  uint8 protocol_type;           /* Protocol the message is for */
  hdrhai_protocol_instance_enum_type protocol_instance; /* in_use/in_config */
  uint8 msg_id;   /* Message identifier, to look for config req */
  uint8 trans_id; /* Transaction ID of config req, to send empty response */
  hdrhai_msg_group_enum_type msg_group; /* Is it a configuration msg? */
  boolean message_was_routed = FALSE; /* If message was able to be routed */

#ifdef FEATURE_HDR_REVC
  hdrsrch_link_id_type link_id;
#endif /* FEATURE_HDR_REVC */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT (MSG_LEGACY_LOW,  "hdrhmp_route_msg");

  /* The HMP header is only the first uint8, the protocol type */
  if ( dsm_pullup( &item_ptr, &protocol_type, 1 ) == 0 )
  {
    ERR( "Could not extract protocol_type", 0, 0, 0 );
    dsm_free_packet(&item_ptr);
    return;
  }

  if (item_ptr == NULL)
  {
    ERR( "Msg w/HMP header only (pid %d)", protocol_type, 0, 0 );
    return;
  }

  /* If the upper most bit of protocol_type is 0, this is the in use instance
   * of the protocol.  Otherwise this is the in config instance.
   */
  protocol_instance = ((protocol_type & 0x80) == 0)? HDRHAI_IN_USE :
                                                     HDRHAI_IN_CONFIG;
  protocol_type &=0x7f;

  /* Use macro to set the protocol field of the DSM item header */
  HDRDSM_SET_PROT_ID( item_ptr, protocol_type );

  if ( dsm_extract( item_ptr, 0, &msg_id, 1 ) == 0 )
  {
    ERR( "Could not extract message_id", 0, 0, 0 );
    msg_id = 0xFF;
  }

  /* A message is a "configuration message" if its msg_id is >= 0x50 and <= 0x7F
   * This includes, for example, the AttributeUpdateRequest message.
   */
  if ((msg_id >= 0x50) && (msg_id <= 0x7F ))
  {
    msg_group = HDRHAI_CONFIG;
  }
  else
  /* A message is "non-config message" if msg_id is < 0x50 or in range 0x80 and
   * 0xFF. This includes Test App protocols that have in its IN-USE instance,
   * Non-Config message with msg_ids in range 0x80-0xFF.
   */
  {
    msg_group = HDRHAI_NON_CONFIG;
  }

  if ( HDRDSM_GET_CHAN_BIT( item_ptr ) )
  {
#ifdef FEATURE_HDR_REVC
#ifdef FEATURE_LTE_TO_HDR_OH
    if ( !hdrsap_tunnel_mode_is_enabled(HDRHAI_HDR_MESSAGING_PROTOCOL) )
#endif /* FEATURE_LTE_TO_HDR_OH*/
    {
      link_id = HDRDSM_GET_LINK_ID( item_ptr );
      if (!hdrlmac_is_linkid_on_cc_monitoring_chan(link_id))
      {
        if (protocol_instance == HDRHAI_IN_USE &&
            ((protocol_type == HDRHAI_N_OMP_TYPE &&
              (msg_id == 0x00 || msg_id == 0x01)) || /* QC & SP */
             (protocol_type == HDRHAI_N_FTMP_TYPE &&
              msg_id == 0x02))) /* LoadInfo*/
        {
          HDR_MSG_PROT_2(MSG_LEGACY_MED, "non-pri CC P 0x%x M 0x%x",
                         protocol_type, msg_id);
        }
        else
        {
          HDR_MSG_PROT_2(MSG_LEGACY_MED, "Drop non-pri CC P 0x%x M 0x%x",
                         protocol_type, msg_id);
          dsm_free_packet(&item_ptr);
          return;
        }
      }
    }
#endif /* FEATURE_HDR_REVC */    

    if (!HDRDSM_GET_BCAST_BIT( item_ptr ))
    {
      HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, "RX CC msg (P 0x%x, ID 0x%x, %d)",
                      protocol_type, msg_id, protocol_instance);
    }
  }
  else
  {
    HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, "RX FTC msg (P 0x%x, ID 0x%x, %d)",
                    protocol_type, msg_id, protocol_instance);
  }

  /* Check if this message is part of a message bundle that has a specific meaning 
   * e.g. CC+TCA=HHO
   */
  hdrhmp_check_rx_msg_bundle(protocol_type, msg_id);

  if ( hdrhmp.ignore_cc_msgs == TRUE )
  {
	  /* Drop CC messages, if ignore flag is set */
	  ERR( "Ignore CC msgs !! ",0,0,0);
  }
  else if ( (msg_id == 0x52) &&  hdrscp_config_is_locked() )
  {
    /* Drop GAUP messages from AN when AT's Config is locked */
    ERR( "AT Config is Locked GAUP Messages are not allowed ",0,0,0);
  }
  else
  {
    if (protocol_type >= HDRHAI_PROTOCOL_TYPE_ENUM_MAX)
    {
      ERR( "Message for unknown protocol ID %d:%d",
            protocol_instance, protocol_type, 0 );
    }
    else if (hdrhmp_routing_table[protocol_type    ]
                                 [protocol_instance]
                                 [msg_group        ] != NULL)
    {
      HDR_MSG_PROT_2 (MSG_LEGACY_LOW,  
                      "HMP --> rx msg p:%d g%d (0:in_use 1:in_config)",
                      protocol_type, msg_group);

      /* Call the appropriate function from the table to handle this message */
      ( *hdrhmp_routing_table[protocol_type][protocol_instance][msg_group] )
      ( item_ptr );
      message_was_routed = TRUE;
    }
    else if ((protocol_instance == HDRHAI_IN_CONFIG) &&
             (msg_group         == HDRHAI_CONFIG))
    {
      if ( protocol_type == HDRHAI_SESSION_CONFIGURATION_PROTOCOL )
      {
        /* Since SCP does not have any message defined for its InConfig
         * instance sending an empty response is invalid so just ignore
         * and free the message. */
        ERR("No msg defined for InConfig instance of SCP ignoring msg ID=%d",
             msg_id, 0, 0);
      }
      else
      {
        /* For ConfigurationRequest message of all other protocols that
         * do not have any configuration attributes, send an empty 
         * Configuration Response message on its behalf. */
        if ( dsm_extract( item_ptr, 1, &trans_id, 1 ) == 0 )
        {
          ERR("Error extracting TransactionID from ConfigReq Msg", 0, 0, 0 );
          /* Without the transaction ID, it makes no sense to send a response*/
        }
        else
        {
          HDR_MSG_PROT (MSG_LEGACY_HIGH, "Sending empty config response");
            hdrutil_send_empty_config_rsp( (hdrhai_protocol_name_enum_type)protocol_type, 
                                           trans_id );
        }
      }
    }
    else
    {
      ERR( "Message for unregistered protocol ID %d:%d",
            protocol_instance, protocol_type, 0 );
    }
  }

  if (!message_was_routed)
  {
    /* If we can't route the message, free the buffer */
    dsm_free_packet(&item_ptr);
  }
  #ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
  else if (((protocol_type == HDRHAI_AC_MAC_PROTOCOL) &&
           (msg_id ==0)) ||
           HDRDSM_GET_BCAST_BIT( item_ptr ) )
  {
    /* Receive ACAck or address with BATI, 
     * Do not stop waiting for UATI Assignment */
  }
  else if (((protocol_type != HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL) ||
           (msg_id != 0x01))  /* UATIAssignment */ &&
           hdrslp_is_waiting_for_uati_assignment())
  {
    /* If receive msg other than UATIAssignment, it means that we 
     * don't want to wait for UATIAssignment before starting access 
     * attempt because it means the AN knows how to reach AT (A13 
     * transfer completed) and AN should be able to handle messages
     * sent by the AT UATI address
     */ 
    hdramp_done_waiting_for_uati_assignment( 
        HDRAMP_CLEAR_SLP_HOLDOFF_RCVD_UNICAST_MSG );

    /* But if we receive UATIAssignment message, we don't want to 
     * clear hdrslp_waiting_for_uati_assignment flag, because if we clear 
     * this flag before queuing UATIComplete to SLP and if Access is in
     * progress(for example, AT receives UATIAssignment within 1 sec of 
     * receiving ACAck of UATIRequest), the UATICompelte may not be bundled
     * with messages that are currently in SLP queue (e.g. CR, ROR)
     * For this reason, we only clear waiting_for_uati_assignment flag 
     * after UATIComplete is queued to SLP (in the hdrhmp_process_send_msg_cmd()
     */
  }
  #endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

} /* hdrhmp_route_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HMP.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data_ptr - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrhmp_ind_cb( hdrind_ind_name_enum_type ind_name, void *ind_data_ptr )
{
  HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "hdrhmp_ind_cb %d", ind_name);

  /* This protocol runs in the HDRMC task. */
  if ( rex_self(  ) == HDRHMP_TASK_PTR )
  {
    /* Indication is from another protocol in MC task, so consider it
     * an internal indication
     */
    hdrhmp_process_ind( ind_name, (hdrind_ind_data_union_type *)ind_data_ptr );
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_HDR_MESSAGING_PROTOCOL, ind_name, ind_data_ptr );
  }
} /* hdrhmp_ind_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_ADD_HEADER

DESCRIPTION
  This function adds the HMP header value into a DSM item.

DEPENDENCIES
  This function is currently only to be used by Route Update Protocol and
  RLP when they directly give messages to SLP without using hdrhmp_send_msg.

PARAMETERS
  protocol_name - The name of the protocol who's ID must be pre-pended
  msg_ptr - Pointer to the message to pre-pend the HMP header to

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrhmp_add_header( hdrhai_protocol_name_enum_type protocol_name,
                        dsm_item_type *msg_ptr )
{
  /* Add the HMP header.  hdrhdrhai_protocol_name_enum_type has been carefully
   * defined so no mapping is needed between protocol name and protocol type
   */
  if (!(hdrutil_pushdown_head( &msg_ptr, &protocol_name, 1 )))
  {
    ERR_FATAL("Pushdown failed",0,0,0);
  }
} /* hdrhmp_add_header() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_TX_Q_IS_EMPTY
DESCRIPTION
  This function tells if there are any send message commands pending for HMP.

DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this function, for debug

RETURN VALUE
  TRUE if there are any send message commands pending for HMP,
  FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

/*lint -efunc(715, hdrhmp_tx_queue_is_empty) OK if "caller" not used */
boolean hdrhmp_tx_queue_is_empty
(
  hdrhai_protocol_name_enum_type caller
)
{
  HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "hdrhmp_tx_queue_is_empty:%d,%d",
                  hdrhmp.msgs_on_event_q, q_cnt(&hdrhmp.pending_msg_q));

  return (( hdrhmp.msgs_on_event_q == 0 ) &&
          (q_cnt(&hdrhmp.pending_msg_q) == 0));
} /* hdrhmp_tx_queue_is_empty() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_PENDING_QUEUE_IS_EMPTY
DESCRIPTION
  This function tells if there are any message in pending queue.

DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this function, for debug

RETURN VALUE
  TRUE if there are any message in pending queue. 

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrhmp_pending_queue_is_empty
(
  hdrhai_protocol_name_enum_type caller
)
{
  HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "msg in pending queue:%d event queue:%d",
                  q_cnt(&hdrhmp.pending_msg_q), hdrhmp.msgs_on_event_q);

  return ( q_cnt(&hdrhmp.pending_msg_q) == 0);
} /* hdrhmp_pending_queue_is_empty() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_SET_AN_AUTH_STATUS
DESCRIPTION
  This function is used by PAC to inform HMP when AN authentication fails.
  This information is needed so we don't get stuck forever trying to open
  a session on a system on which we are failing CHAP with the AN.

DEPENDENCIES
  None

PARAMETERS
  auth_succeeded - TRUE if CHAP authentication passed, FALSE if it failed.
  caller - The protocol calling this function, for debug

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

/*lint -efunc(715, hdrhmp_set_an_auth_status) OK if "caller" not used */
void hdrhmp_set_an_auth_status
(
  boolean auth_succeeded,
  hdrhai_protocol_name_enum_type caller
)
{
  HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, "hdrhmp_set_an_auth_status:%d->%d (%d)",
    hdrhmp.sclose.session_opened_successfully, auth_succeeded, caller);
  if (hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY))
  {
    hdrhmp.sclose.session_opened_successfully = auth_succeeded;
  }
  else
  {
    ERR("AN AUTH occurred before session opened",0,0,0);
  }
} /* hdrhmp_set_an_auth_status() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRHMP_SEND_MSG_WITH_BUNDLE_REQUEST

DESCRIPTION
  This function is called by protocols at all layers to send an AC message
  that requires bundling. The first of the group of messages to be bundled
  should set its bundle_mask to indicate the rest of group. AT will hold
  off AC Probe until the group of message has been received or the bundle
  timer has expired. Currently supported group of messages include:
  1. CR
  2  ROR
  3. DOS

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Protocol sending the message
  chan - Channel the message must go out on (AC or RTC)
  rtc_is_reliable - Whether or not the message is reliable
  priority - Priority of the message, as defined in the HAI
  protocol_instance - Whether it is IN_CONFIG or IN_USE message
  bundle_mask - Applies to AC message only, a bit mask controlling
                if AT should attempt to bundle all specified message
                in a AC message.
  *msg_ptr - The message to be sent
  outcome_cb - Pointer to function called to announce outcome of
                    transmission
  *outcome_tag_ptr - A pointer owned by sender, returned with the callback

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrhmp_send_msg_with_bundle_request
(
  hdrhai_protocol_name_enum_type name,
  hdrhai_channel_enum_type chan,
  boolean rtc_is_reliable,
  uint8 priority,
  hdrhai_protocol_instance_enum_type protocol_instance,
  uint32 bundle_mask,
  dsm_item_type * msg_ptr,
  hdrslp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
)
{
  /* Call hdrhmp_generic_send_msg() with the Stream set to STREAM 0 */
  hdrhmp_generic_send_msg( name,
                           HDRHAI_STREAM0, /* for signaling */
                           chan,
                           rtc_is_reliable,
                           FALSE, /* send_on_curr_rtc_only = FALSE */
                           priority,
                           protocol_instance,
                           NULL, /* not using RASM */
                           bundle_mask,
                           msg_ptr,
                           outcome_cb,
                           outcome_tag_ptr );
} /* hdrhmp_send_bundled_msg() */


/*===========================================================================

FUNCTION HDRHMP_SET_IGNORE_CC_MSGS

DESCRIPTION
  This function sets if incoming CC msgs need to be ignored

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrhmp_set_ignore_cc_msgs( boolean value )
{
  hdrhmp.ignore_cc_msgs = value;
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "ignore_cc_msgs : (%d)",hdrhmp.ignore_cc_msgs);
}

