/*===========================================================================

           H D R   C P   A P I   W R A P P E R   M O D U L E 


DESCRIPTION
  This file is the interface exported by HDR CP 

EXTERNALIZED FUNCTIONS (Global)
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.


Copyright (c) 2011 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/src/hdrcp_api.c#1 $ 
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   vko     Fix compilation errors
04/05/16   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
12/10/14   vko     External API support to retrieve PilotPN, Sector ID, 
                           Mac Index & DRC value
07/31/13   sat     Mainlining Modem Statistics Feature
05/06/13   cnx     Added hdrsmp_session_is_valid.
03/15/13   cnx     Fix Triton compiler errors.
02/05/13   ukl     Move more APIs to api header files.
06/15/12   smd     Included hdrpac_api.h.
04/24/12   cnx     Moved more to SU API header files.
04/18/12   smd     Added hdrcp_rx_queue_ind().    
04/06/12   cnx     Force to close HPRD session after LTE attach.
03/20/12   cnx     Fixed compiler errors for Dime.
02/24/12   smd     Removed feature defines from SU API header files.
02/13/12   smd     Added hdrcp_amp_send_irmi_msg.
01/17/12   cnx     Fixed compiler errors.
01/16/12   cnx     Fixed a typo.
12/05/11   smd     Added two more QCHAT functions.
11/30/11   smd     Added QCHAT function as SU API.
10/24/11   smd     Moved more to SU API header files.
09/15/11   smd     Created this file.

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrcp_api.h"
#include "hdrmci.h"
#include "hdrdos.h"
#include "hdrfcp.h"
#include "hdramp.h"
#include "hdrhmp.h"
#include "hdris890_v.h"
#include "hdrlup.h"
#include "hdrsmp.h"
#include "hdrscmidle.h"
#include "hdrrup.h"
#include "hdrscp.h"
#include "hdrovhd.h"
#include "hdrmc_v.h"
#include "hdrslp.h"
#include "hdrstream.h"
#include "hdrpcp.h"
#include "hdrmrlpc.h"
#include "hdrpac_api.h"
#include "hdr1xmc.h"
#ifdef FEATURE_HDR_QCHAT
#include "hdrqchatupk.h"
#include "hdrqchatupk_opts.h"
#endif /* FEATURE_HDR_QCHAT */
#include "hdrrx.h"
#include "hdrsrchstate.h"
#include "hdrsrch.h"
#include "hdrsrchlog.h"
#include "hdralmp.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*===========================================================================

                      FUNCTION  DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRCP_IS890_ACTIVATE

DESCRIPTION
  This is a a wrap function. It is called by PAC when the enhanced test 
  application is negotiated on any stream.

DEPENDENCIES
  None
  
PARAMETERS
  stream_id  - Stream ID on which IS890A is negotiated 

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrcp_is890_activate
( 
  uint8 stream_id 
    /* the stream id at which the ETAP is negotiated */
)
{
   hdris890a_activate( stream_id );
} /* hdrcp_is890_activate */


/*===========================================================================

FUNCTION HDRCP_IS890_DEACTIVATE

DESCRIPTION
  This is a wrapper function. It is called when the session is closed.

DEPENDENCIES
  None
  
PARAMETERS
  stream_id  - Stream ID on which is890a is assigned

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrcp_is890_deactivate
( 
  uint8 stream_id 
    /* The stream ID at which the is890a was negotiated */
)
{
	hdris890a_deactivate( stream_id );
} /* hdrcp_is890_deactivate */

/*===========================================================================

FUNCTION HDRCP_IS890_MSG_CB

DESCRIPTION
  This function is staticly registered to be called whenever HMP receives a
  message for is890a .

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr - DSM item chain containing the message
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrcp_is890_msg_cb
(
  dsm_item_type* msg_ptr
    /* the ptr to the DSM item containing the incoming msg */
)
{
   hdris890a_msg_cb( msg_ptr );
} /* hdrcp_is890_msg_cb */


/*===========================================================================

FUNCTION HDRCP_MC_QUEUE_IND

DESCRIPTION
  This is a wrapper function. 

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  an indication to protocols in the MC task at the same time.

  ind_data_size must not be larger than hdrind_ind_data_union_type, because
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  input_ind_ptr - Pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcp_mc_queue_ind( hdrcom_hai_protocol_name_enum_type protocol_name,
          hdrcom_ind_name_enum_type ind_name,
          void *input_ind_data_ptr
   )
{
  hdrmc_queue_ind( protocol_name, ind_name, input_ind_data_ptr );
} /* hdrcp_mc_queue_ind*/

/*===========================================================================

FUNCTION HDRCP_MC_QUEUE_CMD

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  commands to protocols in the MC task at the same time.

  cmd_size must not be larger than hdrmc_protocol_cmd_union_type, because
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the command is for
  input_cmd_ptr - Pointer to a union of all protocols in HDRMC task
  cmd_size - Size of the command being given

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcp_mc_queue_cmd( hdrcom_hai_protocol_name_enum_type protocol_name,
                          void * input_cmd_ptr, uint32 cmd_size )
{
   hdrmc_queue_cmd( protocol_name, input_cmd_ptr, cmd_size );
}/* hdrcp_mc_queue_cmd */


/*===========================================================================

FUNCTION HDRCP_MC_QUEUE_MSG

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the message is for
  protocol_instance - Instance of the protocol the message is for
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcp_mc_queue_msg( hdrcom_hai_protocol_name_enum_type protocol_name,
                      hdrcom_hai_protocol_instance_enum_type protocol_instance,
                      dsm_item_type * item_ptr )
{
  hdrmc_queue_msg( protocol_name,
                   protocol_instance,
                   item_ptr );
} /* hdrcp_mc_queue_msg */


/*===========================================================================
FUNCTION HDRCP_MC_GET_STATS_INFO 

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  ftd_stats_ptr - Pointer to location where the FTD stats need to be copied.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrcp_mc_get_stats_info 
(
  sys_hdr_information_s_type *ftd_stats_ptr
)
{
  hdrmc_get_stats_info( ftd_stats_ptr );
} /* hdrcp_mc_get_stats_info */

/*===========================================================================

FUNCTION HDRCP_DOS_MSG_CB

DESCRIPTION
  This callback function queues incoming messages for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - pointer to receive message buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrcp_dos_msg_cb 
( 
  dsm_item_type *msg_ptr
)
{
   hdrdos_msg_cb( msg_ptr );
} /* hdrcp_dos_msg_cb */

/*===========================================================================

FUNCTION HDRCP_FCP_MSG_CB

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr -  Received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrcp_fcp_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  hdrfcp_msg_cb( msg_ptr );
} /* hdrcp_fcp_msg_cb */


/*===========================================================================

FUNCTION HDRCP_FCP_STREAM_IS_OPEN

DESCRIPTION
  This is a wrapper function
 
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
boolean hdrcp_fcp_stream_is_open( hdrcom_hai_stream_enum_type stream_id )
{
  return hdrfcp_stream_is_open( stream_id );
} /* hdrcp_fcp_stream_is_open */

/*===========================================================================

FUNCTION HDRCP_FCP_SEND_XOFF_REQ

DESCRIPTION
  This is a wrapper function.

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
void hdrcp_fcp_send_xoff_req
(
  hdrcom_hai_protocol_name_enum_type sender ,
  hdrcom_hai_stream_enum_type stream_id 
)
{
  hdrfcp_send_xoff_req( sender,stream_id );
}/* hdrcp_fcp_send_xoff_req */


/*===========================================================================

FUNCTION HDRCP_FCP_SEND_XON_REQ

DESCRIPTION
  This is a wrapper function.

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
void hdrcp_fcp_send_xon_req
(
  hdrhai_protocol_name_enum_type sender,
  hdrhai_stream_enum_type stream_id 
)
{
  hdrfcp_send_xon_req( sender, stream_id );
} /* hdrcp_fcp_send_xon_req */
/*===========================================================================

FUNCTION HDRCP_HMP_APP_SEND_MSG

DESCRIPTION
  This is a wrapper function.
  
RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_hmp_app_send_msg
(
  hdrcom_hai_protocol_name_enum_type name,
  hdrcom_hai_stream_enum_type stream,
  hdrcom_hai_channel_enum_type chan,
  boolean rtc_is_reliable,
  uint8 priority,
  hdrcom_hai_protocol_instance_enum_type protocol_instance,
  dsm_item_type * msg_ptr,
  hdrcp_slp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
)
{
  hdrhmp_app_send_msg
  (
    name,
    stream,
    chan,
    rtc_is_reliable,
    priority,
    protocol_instance,
    msg_ptr,
    outcome_cb,
    outcome_tag_ptr
  );
}/* hdrcp_hmp_app_send_msg */


/*===========================================================================

FUNCTION HDRCP_HMP_APP_SEND_RASM

DESCRIPTION
  This is a wrapper function.
 
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
void hdrcp_hmp_app_send_rasm
(
  hdrcom_hai_protocol_name_enum_type name,
  hdrcom_hai_stream_enum_type stream,
  uint32 max_retries,
  uint32 retry_interval,
  uint32 transaction_id,
  dsm_item_type * msg_ptr,
  hdrcp_slp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
)
{
  hdrhmp_app_send_rasm
  (
     name,
     stream,
     max_retries,
     retry_interval,
     transaction_id,
     msg_ptr,
     outcome_cb,
     outcome_tag_ptr
  );
} /* hdrcp_hmp_app_send_rasm */


/*===========================================================================

FUNCTION HDRCP_HMP_APP_SEND_RASM_QOS_RESUME

DESCRIPTION
  This is a wrapper function.
  
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
void hdrcp_hmp_app_send_rasm_qos_resume
(
  hdrcom_hai_protocol_name_enum_type name,
  hdrcom_hai_stream_enum_type stream,
  uint32 max_retries,
  uint32 retry_interval,
  uint32 transaction_id,
  hdrcp_hmp_bundle_preference_enum_type bundle_preference,
  boolean is_implicit_on_connection_setup,
  dsm_item_type * msg_ptr,
  hdrcp_slp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
)
{
  hdrhmp_app_send_rasm_qos_resume
  (
    name,
    stream,
    max_retries,
    retry_interval,
    transaction_id,
    bundle_preference,
    is_implicit_on_connection_setup,
    msg_ptr,
    outcome_cb,
    outcome_tag_ptr
  );
} /* hdrcp_hmp_app_send_rasm_qos_resume */

/*===========================================================================

FUNCTION HDRCP_HMP_RASM_ACKED

DESCRIPTION
  This is a wrapper function.

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
void hdrcp_hmp_rasm_acked
(
  hdrcom_hai_protocol_name_enum_type name,
  uint32 transaction_id
)
{
  hdrhmp_rasm_acked
  (
    name,
    transaction_id
  );
  
} /* hdrcp_hmp_rasm_acked */

/*===========================================================================

FUNCTION HDRCP_HMP_SET_AN_AUTH_STATUS
DESCRIPTION
  This is a wrapper function.

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

void hdrcp_hmp_set_an_auth_status
(
  boolean auth_succeeded,
  hdrcom_hai_protocol_name_enum_type caller
)
{
  hdrhmp_set_an_auth_status
  (
     auth_succeeded,
     caller
  );

} /* hdrcp_hmp_set_an_auth_status */

/*===========================================================================

FUNCTION HDRCP_LUP_MSG_CB

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrcp_lup_msg_cb (dsm_item_type * msg_ptr)
{
   hdrlup_msg_cb ( msg_ptr );
} /* hdrcp_lup_msg_cb */


/*===========================================================================

FUNCTION HDRCP_RUP_CONTINUE_CONNECTION_SETUP

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_rup_continue_connection_setup
(
  hdrcom_hai_protocol_name_enum_type sender
)
{
  hdrrup_continue_connection_setup( sender );
} /* hdrcp_rup_continue_connection_setup */

/*===========================================================================

FUNCTION HDRCP_SMP_DEACTIVATE

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  close_is_silent - TRUE if the session should be closed silently
                    FALSE if the session should be closed explicitely
                 (i.e. by sending a SessionClose message to the AN first)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcp_smp_deactivate
(
  hdrcom_hai_protocol_name_enum_type sender,
  boolean close_is_silent
)
{
  hdrsmp_deactivate( sender, close_is_silent );
} /* hdrcp_smp_deactivate */

/*===========================================================================

FUNCTION HDRCP_ALMP_OPEN_CONNECTION

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  open_reason - The reason the connection is being opened
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcp_almp_open_connection
(
  hdrcom_hai_open_reason_enum_type open_reason,
  hdrcom_hai_protocol_name_enum_type sender
)
{
  hdralmp_open_connection( open_reason, sender );
} /* hdrcp_almp_open_connection */

/*===========================================================================

FUNCTION HDRCP_ALMP_CLOSE_CONNECTION

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcp_almp_close_connection
(
  hdrcom_hai_abort_reason_enum_type reason,
  hdrcom_hai_protocol_name_enum_type sender
)
{
  hdralmp_close_connection( reason, sender );
} /* hdrcp_almp_close_connection */

/*===========================================================================

FUNCTION HDRCP_ALMP_SYSTEM_IS_ACQUIRED

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE if a system is acquired, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrcp_almp_system_is_acquired( hdrcom_hai_protocol_name_enum_type caller )
{
  return hdralmp_system_is_acquired( caller );

} /* hdrcp_almp_system_is_acquired */

/*===========================================================================

FUNCTION HDRCP_IDLE_SET_PKT_DATA_SESS_STATUS

DESCRIPTION 
  This is a wrapper function. 
  
DEPENDENCIES
  None.

PARAMETERS
  is_session_on_hdr - TRUE if whether data session is on HDR or 1x
                    - FALSE otherwise

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrcp_idle_set_pkt_data_sess_status(boolean is_session_on_hdr)
{
  hdridle_set_pkt_data_sess_status( is_session_on_hdr );
} /* hdrcp_idle_set_pkt_data_sess_status */

/*===========================================================================

FUNCTION HDRCP_IDLE_SET_FORCE_LONG_SLEEP

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  force_long_sleep - turn on/off force_long_sleep flag.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_idle_set_force_long_sleep
(
  boolean force_long_sleep
)
{
  hdridle_set_force_long_sleep( force_long_sleep );
} /* hdrcp_idle_set_force_long_sleep */

/*===========================================================================

FUNCTION HDRCP_IDLE_GET_FORCE_LONG_SLEEP

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - force_long_sleep flag is set. AT uses 40 seconds as sleep period in
         SLEEP state. 
  FALSE - force_long_sleep flag is not set.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_idle_get_force_long_sleep( void )
{
  return hdridle_get_force_long_sleep();
} /* hdrcp_idle_get_force_long_sleep */

/*===========================================================================

FUNCTION HDRCP_IDLEC_GET_SLOTTED_MODE_ATTRIB

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_idlec_get_slotted_mode_attrib
( 
  hdrcp_idlec_slotted_mode_type *slotted_mode_attrib_ptr
)
{
  return hdridlec_get_slotted_mode_attrib( slotted_mode_attrib_ptr );
} /* hdrcp_idlec_get_slotted_mode_attrib() */

/*===========================================================================
FUNCTION HDRCP_RUP_SET_TC_HANDDOWN_MODE

DESCRIPTION  : This s a wrapper function. 
 
DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrcp_rup_set_tc_handdown_mode
( 
  hdrl1_srch_tc_handdown_mode_enum_type handdown_mode,
  hdrcom_hai_protocol_name_enum_type sender
)
{
  hdrrup_set_tc_handdown_mode
  ( 
     handdown_mode,
     sender
  );
} /* hdrl1_rup_set_tc_handdown_mode */


/*===========================================================================
FUNCTION HDRCP_RUP_SET_TRAFFIC_MODE

DESCRIPTION
  This function sets the HDR traffic mode.

DEPENDENCIES
  None.

PARAMETERS
  traffic_mode - The HDRSRCH traffic mode to allow applications to tweak,
                 for example, specifics of SHDR operation while connected.
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrcp_rup_set_traffic_mode
( 
  hdrl1_srch_tc_mode_enum_type traffic_mode,
  hdrcom_hai_protocol_name_enum_type sender
)
{
  hdrrup_set_traffic_mode
  ( 
    traffic_mode,
    sender
  );
} /* hdrcp_rup_set_traffic_mode */

/*===========================================================================

FUNCTION HDRCP_SCMIDLE_GAUP_SLOTTED_MODE_ATTRIB

DESCRIPTION
  This function is a wrapper function.
  
DEPENDENCIES
  None.

PARAMETERS
  sm_override -  What type of override to start:
                 HDRSCMIDLE_SM_OVERRIDE_NONE
                 HDRSCMIDLE_SM_OVERRIDE_426_MSECS
                 HDRSCMIDLE_SM_OVERRIDE_213_MSECS

  p_trans_id - Address where the transaction ID is stored to be returned back.

RETURN VALUE
  E_SUCCESS - If a GAUP Update was queued to HDRMC successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer to send this GAUP update.
  E_FAILURE - If attempting to turn off override when no stored config is 
              available i.e. no override was done previously.
              AT couldn't retrieve its current slotted mode config.
              sm_override out of range.

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_scmidle_gaup_slotted_mode_attrib
(
  hdrcp_scmidle_sm_override_enum_type sm_override,
  uint8 *p_trans_id
)
{
  return hdrscmidle_gaup_slotted_mode_attrib ( sm_override, p_trans_id );
} /* hdrcp_scmidle_gaup_slotted_mode_attrib */

/*===========================================================================

FUNCTION HDRCP_SCMIDLE_saved_scmidle_INFO_IS_VALID

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrcp_scmidle_saved_scmidle_info_is_valid ( )
{
  return hdrscmidle_saved_scmidle_info_is_valid();
} /* hdrcp_scmidle_save_sm_info_is_valid */

/*===========================================================================

FUNCTION HDRCP_SCMIDLE_SET_saved_scmidle_INFO_VALID

DESCRIPTION
  This is a wrapper function.  
  
DEPENDENCIES
  None.

PARAMETERS
  saved_scmidle_info_is_valid - if saved slottedmode info is valid 

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrcp_scmidle_set_scmidle_info_valid ( boolean saved_scmidle_info_is_valid )
{
  hdrscmidle_set_scmidle_info_valid( saved_scmidle_info_is_valid );
} /* hdrcp_scmidle_set_scmidle_info_valid */
/*===========================================================================

FUNCTION HDRCP_SCP_GET_CURRENT_SUBTYPE

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  protocol - Protocol type for which the subtype is returned

RETURN VALUE
  Subtype value of the requested protocol, 0xFFFF will be returned if invalid 
  protocol is requested. 

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrcp_scp_get_current_subtype
(
  hdrcom_hai_protocol_name_enum_type protocol
)
{
  return hdrscp_get_current_subtype( protocol );
} /* hdrcp_scp_get_current_subtype */

/*===========================================================================

FUNCTION HDRCP_OVHD_GET_COLOR_CODE

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  color_code_ptr - Pointer to allocated memory where the color code
                   will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the color code has not been received yet

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_ovhd_get_color_code
(
  uint8 *color_code_ptr,
  hdrhai_protocol_name_enum_type caller
)
{
  return hdrovhd_get_color_code( color_code_ptr, caller );
} /* hdrcp_ovhd_get_color_code */

/*===========================================================================

FUNCTION HDRCP_MC_CLIENT_CB_REG

DESCRIPTION
  This is a wrapper function.

  This function allows a client to register/deregister a callback with HDR to
  receive registered HDR events.
  
  Only one event can be registered/deregistered once. To register/deregister
  multiple events, call this function several times.

DEPENDENCIES
  None.

PARAMETERS
  client - Modules which will register the callbacks  
  cb_event - HDR event types to be registered
  reg_type - register or deregister
  cb_func_ptr - pointer to the callback functions
  client_data_ptr - pointer to the callback client data
 
RETURN VALUE
  E_NOT_ALLOWED - parameters are invalid
  E_SUCCESS - register callbacks successfully

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mc_client_cb_reg
(
  hdrcp_mc_cb_client_id_enum_type      client,
    /* client id */
  
  hdrcp_mc_cb_event_enum_type          cb_event,
    /* Events that client wants to register */ 
       
  hdrcp_mc_cb_reg_enum_type            reg_type,
    /* Register or deregister */      

  hdrcp_mc_cb_f_type                   cb_func_ptr,
    /* Pointer to a callback function to notify the client */    

  void                              *client_data_ptr
    /* Pointer to the client data */  
)
{
  return hdrmc_client_cb_reg( client, cb_event, 
                              reg_type, cb_func_ptr, 
			      client_data_ptr );
} /* hdrcom_errno_enum_type hdrcp_mc_client_cb_reg */

/*===========================================================================

FUNCTION HDRCP_SCP_LTE_IS_ATTACHED

DESCRIPTION
  This function is a wrapper function of hdrscp_lte_is_attached().
 
  It resets ehrpd_was_advertised flag after LTE successful attach
  so that session will be renegotiated if current session is HRPD.
 
  This function should be called only after a successful LTE attach.
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_scp_lte_is_attached( void ) 
{
  hdrscp_lte_is_attached();
} /* hdrcp_scp_lte_is_attached() */

/*===========================================================================

FUNCTION HDRCP_SCP_SET_DATA_EHRPD_CAPABILITY

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  TRUE - upper layer's eHRPD capability is valid. 
  FALSE - upper layer's eHRPD capability is invalid.

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_scp_set_data_ehrpd_capability
( 
  boolean data_ehrpd_capability
) 
{
  hdrscp_set_data_ehrpd_capability( data_ehrpd_capability );
}/* hdrcp_scp_set_data_ehrpd_capability */

/*===========================================================================

FUNCTION HDRCP_SLP_SEND_MESSAGE

DESCRIPTION
  This is wrapper function.
 
 
DEPENDENCIES
  None

PARAMETERS
  channel            - Channel SDU has to be transmitted on
  reliable           - Set to true if reliable delivery transport is required
  priority           - Priority of the message
  message_body       - dsm item chain containing the signaling message
  slp_outcome_cbPtr  - Function to be called after message has been
                       transmitted successfully aor transmission was aborted
  outcome_tag_ptr    - User data for callback function

RETURN VALUE
  Enqueueing status (E_SUCCESS or E_PROT_Q_FULL)

SIDE EFFECTS

===========================================================================*/

hdrcom_errno_enum_type hdrcp_slp_send_msg
(
  hdrcom_hai_channel_enum_type  channel,
  boolean                   reliable,
  uint8                     priority,
  dsm_item_type*            msg_ptr,
  hdrcp_slp_outcome_cb_type    slp_outcome_cb_ptr,
  void*                     outcome_tag_ptr 
)
{
	return hdrslp_send_msg
          (
            channel,
            reliable,
            priority,
            msg_ptr,
            slp_outcome_cb_ptr,
            outcome_tag_ptr 
          );
} /* hdrcp_slp_send_msg */

/*===========================================================================

FUNCTION HDRCP_SMP_SESSION_IS_OPEN

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the session is open
  FALSE - the session is not open

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrcp_smp_session_is_open
(
  hdrcom_hai_protocol_name_enum_type sender
)
{
  return hdrsmp_session_is_open( sender );
} /* hdrcp_smp_session_is_open */

/*===========================================================================

FUNCTION HDRCP_SMP_SESSION_IS_VALID

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the session is valid
  FALSE - the session is not valid

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrcp_smp_session_is_valid
(
  hdrcom_hai_protocol_name_enum_type sender
)
{
  return hdrsmp_session_is_valid( sender );
} /* hdrcp_smp_session_is_valid */

/*===========================================================================

FUNCTION HDRCP_STREAM_GET_STREAM

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES

PARAMETERS
  app_subtype - The application subtype who's stream is desired.
  stream_id - Pointer to location in which to return the stream_id, if 
              one is found.

RETURN VALUE
  TRUE - There is a stream configured with the Service Network
  FALSE - There is no stream configured with the Service Network

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrcp_stream_get_stream  
( 
  hdrcom_hai_app_subtype_enum_type app_subtype,
  hdrcom_hai_stream_enum_type *stream_id
)
{
  return hdrstream_get_stream  
         ( 
           app_subtype,
           stream_id
         );
} /* hdrcp_stream_get_stream */


/*===========================================================================

FUNCTION HDRCP_STREAM_GET_STREAM_CONFIG

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None
 
PARAMETERS
  stream_ptr - first element of a four-application array.

RETURN VALUE
  TRUE - streams were returned
  FALSE - streams were not returned
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_stream_get_stream_config 
(
  hdrcom_hai_app_subtype_enum_type * stream_ptr
)
{
  return hdrstream_get_stream_config( stream_ptr );
} /* hdrcp_stream_get_stream_config */


/*===========================================================================

FUNCTION HDRCP_STREAM_STREAM_IS_ASSIGNED

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  app_subtype -  Application sub-type to check streams for

RETURN VALUE
  TRUE  - Stream has been allocation for a Packet Application
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_stream_stream_is_assigned
(
  hdrcom_hai_app_subtype_enum_type   app_subtype
)
{
  return hdrstream_stream_is_assigned( app_subtype );
} /* hdrcp_stream_stream_is_assigned */

/*===========================================================================
FUNCTION HDRCP_PAC_FLOW_ENABLE

DESCRIPTION
  This is a wrapper function .

DEPENDENCIES
  None

PARAMETERS
   app_subtype - Application Subtype whose flow is to be enabled

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_pac_flow_enable
(
hdrcom_hai_app_subtype_enum_type app_subtype
)
{
  hdrpac_flow_enable( app_subtype );
} /* hdrcp_pac_flow_enable */


/*===========================================================================
FUNCTION HDRCP_PAC_FLOW_DISABLE

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
   app_subtype - Application Subtype whose flow is to be disabled

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_pac_flow_disable
(
hdrcom_hai_app_subtype_enum_type app_subtype
)
{
  hdrpac_flow_disable( app_subtype );
}/* hdrcp_pac_flow_disable */


/*===========================================================================
FUNCTION HDRCP_PAC_APP_FLOW_IS_ENABLED

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If the stream corresponding to the RLP instance is Open
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_pac_app_flow_is_enabled
(
hdrcom_hai_app_subtype_enum_type app_subtype
)
{
  return hdrpac_app_flow_is_enabled( app_subtype );
} /* hdrcp_pac_app_flow_is_enabled */
/*===========================================================================

FUNCTION HDRCP_PCP_NOTIFY_APP_HAS_HP_DATA

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  substream_id   - The substream ID of the packet app
  stream_id      - The stream ID of the packet app
  user_data_ptr  - Pointer to the packet app's control block

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrcp_pcp_notify_app_has_hp_data
(
  uint8 substream_id,
  uint8 stream_id,
  void *user_data_ptr
)
{
  hdrpcp_notify_app_has_hp_data( substream_id, stream_id, user_data_ptr );
} /* hdrcp_pcp_notify_app_has_hp_data */


/*=============================================================================

FUNCTION HDRCP_PCP_REGISTER_RECEIVE_CALLBACKS

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None.

PARAMETERS
  stream_id           - The stream associated with the packet app
  put_data_func_ptr   - The callback to handle received data
  eos_notif_func_ptr  - The callback to give an End-of-Slot indication
  user_data_ptr       - Pointer to the packet app's control block

RETURN VALUE
  TRUE if the packet app/stream is successfully registered; FALSE otherwise.

SIDE EFFECTS
  None.
  
=============================================================================*/
boolean hdrcp_pcp_register_receive_callbacks
(
  uint8 stream_id,
  hdrcp_pcp_put_data_func_type put_data_func_ptr,
  hdrcp_pcp_eos_notification_func_type eos_notif_func_ptr,
  hdrcp_pcp_rx_buf_empty_notification_func_type rx_buf_empty_notif_func_ptr,
  void *user_data_ptr
)
{
  return hdrpcp_register_receive_callbacks
         (
           stream_id,
           put_data_func_ptr,
           eos_notif_func_ptr,
           rx_buf_empty_notif_func_ptr,
           user_data_ptr
         );
} /* hdrcp_pcp_register_receive_callbacks */

/*=============================================================================

FUNCTION HDRCP_PCP_UNREGISTER_RECEIVE_CALLBACKS

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None.

PARAMETERS
  stream_id  - The ID of the stream being unregistered.

RETURN VALUE
  A boolean TRUE indicates the unregistration was successful; a boolean FALSE
  indicates the unregistration failed.

SIDE EFFECTS
  After a successful unregistration, the callbacks for the unregistered stream
  will be set to NULL.
  
=============================================================================*/
boolean hdrcp_pcp_unregister_receive_callbacks
(
  uint8 stream_id
)
{
  return hdrpcp_unregister_receive_callbacks( stream_id );
} /* hdrcp_pcp_unregister_receive_callbacks */

/*=============================================================================

FUNCTION HDRCP_PCP_UNREGISTER_TRANSMIT_CALLBACKS

DESCRIPTION
  This function is a wrapper function.
 
DEPENDENCIES
  None.

PARAMETERS
  stream_id     - The stream ID of the registered packet app to unregister
  substream_id  - The substream ID of the registered packet app to unregister

RETURN VALUE
  A boolean TRUE indicates the unregistration was successful; a boolean FALSE
  indicates the unregistration failed.

SIDE EFFECTS
  None.
  
=============================================================================*/
boolean hdrcp_pcp_unregister_transmit_callbacks
(
  uint8 stream_id,
  uint8 substream_id
)
{
  return hdrpcp_unregister_transmit_callbacks( stream_id, substream_id );
} /* hdrcp_pcp_unregister_transmit_callbacks */

/*=============================================================================

FUNCTION HDRCP_PCP_REGISTER_TRANSMIT_CALLBACKS

DESCRIPTION
  This is a wrapper function. 
  
DEPENDENCIES
  None.

PARAMETERS
  substream_id   - The substream ID of the packet app
  mac_flow_id    - The MAC flow ID of the registering packet app
  stream_id      - The stream ID of the registering packet app
  get_data_size_func_ptr  - The callback to determine the data pending
  get_hp_data_func_ptr    - The callback to request high priority data
  get_lp_data_func_ptr    - The callback to request low priority data
  tx_status_func_ptr      - The callback to report the status of a tranmission
  tx_failed_func_ptr      - The callback to give an M-ARQ indication
  always_notify_tx_status - Set if the packet app always wants the status
                            notification regardless of transmitting data
  user_data_ptr           - Pointer to the packet app's control block
  
RETURN VALUE
  TRUE if the packet app is sucessfully registered; FALSE otherwise.

SIDE EFFECTS
  None.
  
=============================================================================*/
boolean hdrcp_pcp_register_transmit_callbacks
(
  uint8 substream_id,
  uint8 mac_flow_id,
  uint8 stream_id,
  hdrcp_pcp_get_data_size_func_type get_data_size_func_ptr,
  hdrcp_pcp_get_data_func_type get_hp_data_func_ptr,
  hdrcp_pcp_get_data_func_type get_lp_data_func_ptr,
  hdrcp_pcp_tx_status_func_type tx_status_func_ptr,
  hdrcp_pcp_tx_failed_func_type tx_failed_func_ptr,
  boolean always_notify_tx_status,
  void *user_data_ptr
)
{
 return hdrpcp_register_transmit_callbacks(
      substream_id,mac_flow_id,stream_id,get_data_size_func_ptr,
	  get_hp_data_func_ptr,get_lp_data_func_ptr,tx_status_func_ptr,
	  tx_failed_func_ptr,always_notify_tx_status,user_data_ptr );

} /* hdrcp_pcp_register_transmit_callbacks */

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_SIMPLE_ATTRIB

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None.
  
PARAMETERS
  attrib_name - The Attribute's Name (per type)
  attrib_idx  - FlowNN or ResKK (if applicable, 
                HDRMLPC_ATTRIB_INVALID passed in otherwise)
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
           
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_simple_attrib
(
  hdrcp_mrlpc_attrib_name_enum_type attrib_name,
  uint8 attrib_idx,
  uint32 * value,
  uint8 * verno
)
{
 return hdrmrlpc_get_simple_attrib( attrib_name,attrib_idx,value,verno );
} /* hdrcp_mrlpc_get_simple_attrib */
/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_ACTIVE_FWD_RLPS

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  The module calling this function needs to make sure that memory is allocated
  for listing upto the maximum supported RLPs.
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the RLP IDs of all the forward link 
              multi-flow RLPs
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_active_fwd_rlps
(
  hdrcom_hai_app_subtype_enum_type   app_subtype,
  uint8 *num_active_fwd_rlps,
  uint8 *rlp_flow
)
{
  return hdrmrlpc_get_active_fwd_rlps
         (
            app_subtype,
            num_active_fwd_rlps,
            rlp_flow
         );
} /* hdrcp_mrlpc_get_active_fwd_rlps */

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_ACTIVE_REV_RLPS

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  The module calling this function needs to make sure that memory is allocated
  for listing upto the maximum supported RLPs.
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the RLP IDs of all the forward link 
              multi-flow RLPs
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_active_rev_rlps
(
  hdrcom_hai_app_subtype_enum_type   app_subtype,
  uint8 *num_active_rev_rlps,
  uint8 *rlp_flow
)
{
  return hdrmrlpc_get_active_rev_rlps
         (
            app_subtype,
            num_active_rev_rlps,
            rlp_flow
         );
} /* hdrcp_mrlpc_get_active_rev_rlps */

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_AN_SUP_QOS_PROF_ATTRIB

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.
  
PARAMETERS
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_an_sup_qos_prof_attrib
(
  hdrcp_mrlpc_supported_qos_profiles * value,
  uint8 * verno
)
{
  return hdrmrlpc_get_an_sup_qos_prof_attrib
         (
            value,
            verno
         );
} /* hdrcp_mrlpc_get_an_sup_qos_prof_attrib */

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_FLOW_PROT_PARM_FWD_ATTRIB

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_flow_prot_parm_fwd_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_flow_prot_parm_fwd_type * value,
  uint8 * verno
)
{
  return hdrmrlpc_get_flow_prot_parm_fwd_attrib
         (
            rlp_flow_nn,
            value,
            verno
         );
} /* hdrcp_mrlpc_get_flow_prot_parm_fwd_attrib */

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_FLOW_PROT_PARM_REV_ATTRIB

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_flow_prot_parm_rev_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_flow_prot_parm_rev_type * value,
  uint8 * verno
)
{
  return hdrmrlpc_get_flow_prot_parm_rev_attrib
         (
           rlp_flow_nn,
           value,
           verno
         );
} /* hdrcp_mrlpc_get_flow_prot_parm_rev_attrib */

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_IDENTIFICATION_ATTRIB

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute) 
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_identification_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_ver_enum_type direction,
  hdrcp_mrlpc_flow_identification_type * value,
  uint8 * verno
)
{
  return hdrmrlpc_get_identification_attrib
         (
            rlp_flow_nn,
            direction,
            value,
            verno
         );
} /* hdrcp_mrlpc_get_identification_attrib */

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_MAX_RESERVATIONS_ATTRIB

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.
  
PARAMETERS
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_max_reservations_attrib
(
  hdrcp_mrlpc_max_reservations_type * value,
  uint8 * verno
)
{
  return hdrmrlpc_get_max_reservations_attrib( value, verno );
} /* hdrcp_rmrlpc_get_max_reservations_attrib() */

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_RESERVATION_ATTRIB

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                 
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_reservation_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_ver_enum_type direction,
  hdrcp_mrlpc_flow_reservation_type * value,
  uint8 * verno
)
{
  return hdrmrlpc_get_reservation_attrib
         (
           rlp_flow_nn,
           direction,
           value,
           verno
         );
} /* hdrcp_mrlpc_get_reservation_attrib */


/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_RESKK_QOS_REQUEST_ATTRIB

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.
  
PARAMETERS
  res_kk  - ReservationKK of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                   
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_reskk_qos_request_attrib
(
  uint8 res_kk,
  hdrcp_mrlpc_ver_enum_type direction,
  hdrcp_mrlpc_res_qos_profile_config_type * value,
  uint8 * verno
)
{
  return hdrmrlpc_get_reskk_qos_request_attrib
         (
           res_kk,
           direction,
           value,
           verno
         );
} /* hdrcp_mrlpc_get_reskk_qos_request_attrib */

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_RESKK_QOS_RESPONSE_ATTRIB

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None.
  
PARAMETERS
  res_kk  - ReservationKK of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                   
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_reskk_qos_response_attrib
(
  uint8 res_kk,
  hdrcp_mrlpc_ver_enum_type direction,
  hdrcp_mrlpc_res_qos_profile_config_type * value,
  uint8 * verno
)
{
  return hdrmrlpc_get_reskk_qos_response_attrib
        (
          res_kk,
          direction,
          value,
          verno
        );
} /* hdrcp_mrlpc_get_reskk_qos_response_attrib */


/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_ROUTE_PROT_PARM_FWD_ATTRIB

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_route_prot_parm_fwd_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_route_prot_parm_fwd_type * value,
  uint8 * verno
)
{
  return hdrmrlpc_get_route_prot_parm_fwd_attrib
         (
           rlp_flow_nn,
           value,
           verno
         );
} /* hdrcp_mrlpc_get_route_prot_parm_fwd_attrib */


/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_ROUTE_PROT_PARM_REV_ATTRIB

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_route_prot_parm_rev_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_route_prot_parm_rev_type * value,
  uint8 * verno
)
{
  return hdrmrlpc_get_route_prot_parm_rev_attrib
         (
           rlp_flow_nn,
           value,
           verno
         );
} /* hdrcp_mrlpc_get_route_prot_parm_rev_attrib */


/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_SEQLEN_ATTRIB

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                   
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_seqlen_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_ver_enum_type direction,
  hdrcp_mrlpc_flow_sequence_length_type * value,
  uint8 * verno
)
{
  return hdrmrlpc_get_seqlen_attrib
         (
           rlp_flow_nn,
           direction,
           value,
           verno
         );
} /* hdrcp_mrlpc_get_seqlen_attrib */

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_TIMERS_ATTRIB

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                   
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_timers_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_ver_enum_type direction,
  hdrcp_mrlpc_flow_timers_type * value,
  uint8 * verno
)
{
  return hdrmrlpc_get_timers_attrib
         (
           rlp_flow_nn,
           direction,
           value,
           verno
         );
} /* hdrcp_mrlpc_get_timers_attrib */

/*===========================================================================

FUNCTION  HDRCP_MRLPC_PROP_GAUP_QOS_REQUEST

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS     - AT is sending the AttributeUpdateRequest message
                  transaction_id of the message is passed back.
  
  E_NOT_ALLOWED - AT is not allowed to send a GAUP message while it is waiting
                  for a response to AttributeUpdateRequest message.
                                   
   "While the initiator is waiting for a response to an AttributeUpdateRequest 
    message, it shall not transmit another AttributeUpdateRequest message with 
    a different TransactionID field that request reconfiguration of an 
    attribute included in the original AttributeUpdateRequest message. "
  
  E_NOT_AVAILABLE - MRLP not bound to any stream
  
  E_DATA_TOO_LARGE - num_profiles greater than MAX reservations allowed
                     (20 fwd + 20 rev)
    
SIDE EFFECTS
  None
  
===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_prop_gaup_qos_request
(
  uint8 num_profile_blobs,
  hdrcp_mrlpc_gaup_qos_request_type *profile_ptr,
  uint8 *transaction_id_ptr,
  uint8 preallocated_trans_id  
)
{
  return hdrmrlpc_prop_gaup_qos_request
         (
           num_profile_blobs,
           profile_ptr,
           transaction_id_ptr,
           preallocated_trans_id  
         );
} /* hdrcp_mrlpc_prop_gaup_qos_request */


/*===========================================================================

FUNCTION HDRCP_1XMC_GET_LOCATION

DESCRIPTION
  This is a wrapper function

DEPENDENCIES
  None

PARAMETERS
  location_ptr - A pointer to the location value of the current HDR system

RETURN VALUE
  TRUE - The location is valid and returned in location_ptr
  FALSE - The location is unknown or in a format unknown to IS2000-A

SIDE EFFECTS
  None
===========================================================================*/

boolean hdrcp_1xmc_get_location ( hdrcp_1xmc_location_type * location_ptr )
{
  return hdr1xmc_get_location( location_ptr );
} /* hdrcp_1xmc_get_location */

/*===========================================================================

FUNCTION HDRCP_QCHATUPK_PAC_MDM_HANDLE_CASE

DESCRIPTION
  This is a wrapper function

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if QChat optimization procedure was successfully completed, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrcp_qchatupk_pac_mdm_handle_case(uint16 msg_type, dsm_item_type* msg_ptr)
{
#ifdef FEATURE_HDR_QCHAT
  return hdrqchatupk_pac_mdm_handle_case( msg_type, msg_ptr);
#else
  return FALSE;
#endif /* FEATURE_HDR_QCHAT */
} /* hdrcp_qchatupk_pac_mdm_handle_case */


/*===========================================================================

FUNCTION HDRCP_QCHATUPK_HANDLE_DATA_OVER_SIGNALLING_MSG_ID

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void hdrcp_qchatupk_handle_data_over_signalling_msg_id (void)
{
#ifdef FEATURE_HDR_QCHAT
  hdrqchatupk_handle_data_over_signalling_msg_id();
#endif /* FEATURE_HDR_QCHAT */
} /* hdrcp_qchatupk_handle_data_over_signalling_msg_id */

/*===========================================================================

FUNCTION HDRCP_QCHATUPK_MCS_ENABLED

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_qchatupk_mcs_enabled(void)
{
#ifdef FEATURE_HDR_QCHAT
  return hdrqchatupk_mcs_enabled();
#else
  return FALSE;
#endif /* FEATURE_HDR_QCHAT */

} /* hdrcp_qchatupk_mcs_enabled */

/*===========================================================================

FUNCTION HDRCP_QCHATUPK_MOD_ENABLED

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_qchatupk_mod_enabled(void)
{
#ifdef FEATURE_HDR_QCHAT
  return hdrqchatupk_mod_enabled();
#else
  return FALSE;
#endif /* FEATURE_HDR_QCHAT */

} /* hdrcp_qchatupk_mod_enabled */

/*===========================================================================
 
FUNCTION HDRCP_AMP_SEND_IRMI_MSG

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If IRMI is allowed to be sent
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_amp_send_irmi_msg
( 
  hdrcom_hai_protocol_name_enum_type caller
)
{
#ifdef FEATURE_LTE_TO_HDR_OH
  return hdramp_send_irmi_msg( caller );
#else
  return FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH */

}/* hdrcp_amp_send_irmi_msg */

/*===========================================================================

FUNCTION HDRCP_RX_QUEUE_IND

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  input_ind_ptr - Pointer to the indication data
  ind_data_size - The size of the data associated with this indication.
                  This value is undefined if the input_ind_ptr
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_rx_queue_ind
(
  hdrcom_hai_protocol_name_enum_type  protocol_name,
  hdrcom_ind_name_enum_type           ind_name,
  void                                *input_ind_data_ptr
)
{
  hdrrx_queue_ind( protocol_name, ind_name, input_ind_data_ptr );
} /* hdrcp_rx_queue_ind */

/*=============================================================================

FUNCTION HDRCP_OVHD_GET_SUBNET

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None
  
PARAMETERS
  subnet_ptr - Pointer to allocated memory where the subnet will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the color code has changed and we have not been received yet
              the new subnet from the SP message yet.
  
SIDE EFFECTS
  None.
=============================================================================*/
hdrcom_errno_enum_type hdrcp_ovhd_get_subnet
( 
  hdraddr_type subnet,
  hdrcom_hai_protocol_name_enum_type caller
)
{
  return hdrovhd_get_subnet( subnet, caller );
} /* hdrcp_ovhd_get_subnet */

/*=============================================================================
 
FUNCTION HDRCP_OVHD_PREREG_IS_ALLOWED

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  caller - The name of the calling protocol (used in software trace).

RETURN VALUE
  TRUE - pre-registration is allowed 
  FALSE - pre-registration is not allowed

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrcp_ovhd_prereg_is_allowed 
( 
  hdrcom_hai_protocol_name_enum_type caller
)
{
#ifdef FEATURE_LTE_TO_HDR_OH
  return hdrovhd_prereg_is_allowed( caller );
#else
  return FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH */

} /* hdrcp_ovhd_prereg_is_allowed */

/*===========================================================================
 
FUNCTION HDRCP_SAP_GET_CURRENT_TUNNEL_MODE_VALUE

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  caller - protocol that is invoking the function

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
hdrcom_hai_tunnel_mode_type hdrcp_sap_get_current_tunnel_mode_value 
( 
  hdrcom_hai_protocol_name_enum_type caller 
)
{
#ifdef FEATURE_LTE_TO_HDR_OH
  return hdrsap_get_current_tunnel_mode_value( caller );
#else
  return HDRHAI_TUNNEL_MODE_DISABLED;
#endif /* FEATURE_LTE_TO_HDR_OH */

} /* hdrcp_sap_get_current_tunnel_mode_value */

/*===========================================================================
FUNCTION HDRCP_SAP_SET_PREREG_STATUS

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  hdrhai_prereg_status_event_enum_type - what type of event it is

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrcp_sap_set_prereg_status
( 
  hdrcom_prereg_status_event_enum_type prereg_status_event
)
{
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrsap_set_prereg_status( prereg_status_event );
#endif /* FEATURE_LTE_TO_HDR_OH */

} /* hdrcp_sap_set_prereg_status */



/*===========================================================================
 
FUNCTION HDCP_MC_GET_SID_PN_MAC

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  hdrmc_sid_pn_mac_info - info holding sector id, pilot PN, Mac index

RETURN VALUE
  E_SUCCESS - if API executed succesfully
  E_FAILURE - Failure cases like NULL pointer being passed

SIDE EFFECTS
  None
===========================================================================*/
	hdrerrno_enum_type hdrcp_mc_get_sid_pn_mac ( hdrmc_sid_pn_mac_info* sid_pn_mac )
	{
	  sys_link_type link_info;
	  int i = 0;
	
	  if(!sid_pn_mac)
	  {
		 HDR_MSG_PROT (MSG_LEGACY_HIGH, "returning failure due to null parameter passed");
		 return E_FAILURE;
	  }
	  
	  sid_pn_mac->is_sector_id_valid = FALSE;
	  sid_pn_mac->is_pilot_pn_valid = FALSE;
	  sid_pn_mac->is_mac_index_valid = FALSE;
	
	  /* Retrieve sector ID */
	  if(hdrovhd_get_sector_id(sid_pn_mac->sector_id, NULL, HDRHAI_LOGGING_MODULE) == E_SUCCESS)
	  {
		 HDR_MSG_PROT (MSG_LEGACY_LOW, "DEBUG: Sector ID returned");
		 sid_pn_mac->is_sector_id_valid = TRUE;
	  }
	  else
	  {
	  HDR_MSG_PROT (MSG_LEGACY_HIGH, "sector ID returned FAILURE");
	  }
	  /* Retrieve Pilot PN */  
	  if( hdrsrch_get_link_info(HDRSRCH_MON_LINK_ID, &link_info) == E_SUCCESS)
	  {
		  sid_pn_mac->pilot_pn = link_info.pilot_pn;
		  sid_pn_mac->is_pilot_pn_valid = TRUE;
		  HDR_MSG_PROT_1 (MSG_LEGACY_LOW, "DEBUG: Pilot PN returned %d",sid_pn_mac->pilot_pn);
	  }
	  else
	  {
	  HDR_MSG_PROT (MSG_LEGACY_HIGH, "link info returned FAILURE");
	  }
	  
	  /* Retrieve MAC Index */
	  if  ( hdrsrch_get_current_state() == HDRSRCH_CONNECTED_STATE )
	  {
		  sid_pn_mac->mac_index = (uint16) ( hdrsrch_get_mac_index( HDRSRCH_MON_LINK_ID ) );
		  sid_pn_mac->is_mac_index_valid = TRUE;
		  /* Get the mac index of the cc monitoring pilot */
		  HDR_MSG_PROT_1 (MSG_LEGACY_LOW, "DEBUG: mac_index returned %d",sid_pn_mac->mac_index);
	  }
	  else
	  {
		 HDR_MSG_PROT_1 (MSG_LEGACY_LOW, "DEBUG: hdrsrch_get_current_state is %d",hdrsrch_get_current_state());
	  }
	
	  return E_SUCCESS;
	}


/*===========================================================================
 
FUNCTION HDRCP_MC_GET_SERV_DRC_VALUE

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  hdrmc_serv_drc_rate_info - info holding the value of serv drc rate

RETURN VALUE
  E_SUCCESS - if API executed succesfully
  E_FAILURE - Failure cases like NULL pointer being passed

SIDE EFFECTS
  None
===========================================================================*/
	hdrerrno_enum_type hdrcp_mc_get_serv_drc_rate
	( 
	  hdrmc_serv_drc_rate_info* serv_drc_rate
	)
	{
	
	   if(!serv_drc_rate)
	   {
	     HDR_MSG_PROT (MSG_LEGACY_HIGH, "returning failure due to null parameter passed");
		 return E_FAILURE;
	   }
	   serv_drc_rate->is_drc_rate_valid = FALSE;
	   if  ( hdrsrch_get_current_state() == HDRSRCH_CONNECTED_STATE )
	   {
		 serv_drc_rate->drc_rate = hdrsrchlog_get_serv_drc_rate();
		 serv_drc_rate->is_drc_rate_valid = TRUE;
	   }
		  HDR_MSG_PROT_1 (MSG_LEGACY_LOW, "drc rate is %d",serv_drc_rate->drc_rate);
		 return E_SUCCESS;
	}


  /*=============================================================================
  
  FUNCTION HDRCP_ALMP_CHECK_REDIR_TO_LTE_ALLOWED
  
  DESCRIPTION
    This function checks if it is ok to redirect to LTE. 
  
  DEPENDENCIES
    None.
  
  PARAMETERS
    None
    
  RETURN VALUE
    TRUE is ok to redirect to LTE. Otherwise FALSE.
  
  SIDE EFFECTS
    None.
  =============================================================================*/
  boolean hdrcp_almp_check_redir_to_lte_allowed(void)
  {
    sys_rat_pri_list_info_s_type d2l_rat_pri_list;
    uint16 supported_band_index;
    boolean ret_val = FALSE;

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
    ret_val = hdralmp_check_ok_to_redir_to_lte(&d2l_rat_pri_list, 
                                                &supported_band_index);
#else
    HDR_MSG_PROT (MSG_LEGACY_MED, "D2L redirection feature NOT defined");
#endif

    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "hdrcp_almp_check_redir_to_lte_allowed: return value %d",ret_val);
    return (ret_val);
  }

