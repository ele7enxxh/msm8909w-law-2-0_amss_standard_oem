#ifndef HDRHMP_H
#define HDRHMP_H
/*===========================================================================

                 H D R   M E S S A G I N G   P R O T O C O L
           ( S I G N A L I N G   N E T W O R K   P R O T O C O L )
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for HMP (SNP in IS-856).

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrhmp.h_v   1.15   09 Apr 2003 09:15:52   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrhmp.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/19/14   vko     Fixed race between amac start attempt and bundling due to ROR
08/24/12   sju     Fix to Re-try open conn cmd after resume if hdrhmp_pending_queue
                   is having pending RTC MSG from HDRHAI_IDLE_STATE_PROTOCOL
01/18/12   wsh     Merged changes to fix compile errors.
12/05/11   cnx     Support hdrcp_config_info.conf.
09/23/11   smd     SU API Cleanup.
05/19/10   pba     Send session close when HMP pending message buffer overflows.
04/17/08   sju     Added support to bundle AC messages 
09/26/06   sju     Add hdrhmp_clear_rasm_queue() to remove all RASM messages 
                   from RASM list.
09/14/06   sju     Added support to send ReservationOnRequest by using RASM 
                   when ReservationKKIdleState == 2. 
07/18/06   dna     Major RASM changes to allow reliable QoS call setup.
05/10/06   yll/dna Reliable Access Signaling Messages (RASM)
07/04/05   dna     Protect hdrhmp.msgs_on_event_q for multi-threaded access.
04/02/03   mpa     Added hdrhmp_send_msg_on_existing_rtc()
                   Added hdrhmp_app_send_msg_on_existing_rtc()
05/30/02   dna     Ensure msgs_on_event_q does not get out of sync.
03/18/02   dna/vas Added hdrhmp_set_an_auth_status()
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/17/01   vas     Added function hdrhmp_app_send_msg()
07/10/01   dna     Added hdrhmp_abort_msg(), to wrap hdrslp_abort_msg().
06/06/01   dna     new param for hmp_send_msg()
05/22/01   dna     Handle messages sendable on either AC or RTC.
02/09/01   dna     Added hdrhmp_tx_queue_is_empty() for idle state to tell if
                   HMP had something to send.
05/01/00   dna     Created Module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "hdrerrno.h"
#include "hdrslp.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "hdrcp_api.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRHMP_HEADER_SIZE     1    /* header size in byte */

#ifdef FEATURE_LTE_TO_HDR_OH
#define HDRHMP_BACKOFF_TIME_TABLE_SESSION_FAILED "/nv/item_files/modem/hdr/cp/hmp/backofftimesession"
#endif /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

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

extern void hdrhmp_msgs_on_event_q
( 
  int modifier
);

/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

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

extern void hdrhmp_powerup_init( void );

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

extern void hdrhmp_init( void );

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
extern void hdrhmp_send_msg
(
  hdrhai_protocol_name_enum_type name,
  hdrhai_channel_enum_type chan,
  boolean rtc_is_reliable,
  uint8 priority,
  hdrhai_protocol_instance_enum_type protocol_instance,
  dsm_item_type * msg_ptr,
  hdrslp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
);

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
  a new connection be opened to send the message out (Config messages, etc.)
 
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
extern void hdrhmp_send_msg_on_existing_rtc
(
  hdrhai_protocol_name_enum_type name,
  boolean rtc_is_reliable,
  uint8 priority,
  hdrhai_protocol_instance_enum_type protocol_instance,
  dsm_item_type * msg_ptr,
  hdrslp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
);

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

  The transaction_id is per protocol, not per message.  It is ignored if 
  max_retries == 0.  Otherwise it must meet the following requirements:
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
);

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
);

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

E_AGAIN       - If bundle_preference is not HDRHMP_BUNDLE_NONE, this is an 
                AC_OR_RTC message and E_AGAIN is intercepted and retried by RASM.  
                Otherwise it is returned to the caller here.
                a) Connection setup failed due to no TCA
                b) Connection setup failed due to no RTCAck
                c) Connection closes before all retries are attempted

E_TIMER_EXP   - Total time to send message exceeded 40s

E_PROT_Q_FULL - There were no buffers to send the message

  The transaction_id is per protocol, not per message.  It is ignored if 
  max_retries == 0.  Otherwise it must meet the following requirements:
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
  bundle_preference - message bundle preference. 
  is_implicit_on_connection_setup - TRUE if ReservationKKIdleState == 2, FALSE otherwise
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
);

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
extern void hdrhmp_rasm_acked
(
  hdrhai_protocol_name_enum_type name,
  uint32 transaction_id
);

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
  void

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrhmp_clear_rasm_queue(void);


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
);
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
extern void hdrhmp_app_send_msg_on_existing_rtc
(
  hdrhai_protocol_name_enum_type name,
  hdrhai_stream_enum_type stream,
  boolean rtc_is_reliable,
  uint8 priority,
  hdrhai_protocol_instance_enum_type protocol_instance,
  dsm_item_type * msg_ptr,
  hdrslp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
);

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

extern hdrerrno_enum_type hdrhmp_abort_msg
( 
  hdrslp_outcome_cb_type     slp_outcome_cb,
  void*                      outcome_tag_ptr 
);

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

extern void hdrhmp_route_msg( dsm_item_type * item_ptr );

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
  ind_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

extern void hdrhmp_ind_cb (hdrind_ind_name_enum_type ind_name, void *ind_data);

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

extern void hdrhmp_add_header( hdrhai_protocol_name_enum_type protocol_name, 
                        dsm_item_type *msg_ptr );

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

extern boolean hdrhmp_tx_queue_is_empty 
(
  hdrhai_protocol_name_enum_type caller
);

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
extern boolean hdrhmp_pending_queue_is_empty
(
  hdrhai_protocol_name_enum_type caller
);

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
extern void hdrhmp_set_an_auth_status 
(
  boolean auth_succeeded,
  hdrhai_protocol_name_enum_type caller
);

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
extern void hdrhmp_process_timer( uint32 timer_id );

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
);

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

extern void hdrhmp_clear_pending_msg_queue
(
  errno_enum_type failure_reason
);

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
extern void hdrhmp_stop_waiting_for_bundling_msg (void);

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

void hdrhmp_set_ignore_cc_msgs( boolean value );

#endif /* HDRHMP_H */
