/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      RR  TO  L2  I N T E R F A C E

GENERAL DESCRIPTION
   This module contains the interface functions to send messages to L2.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2014 by QUALCOMM Technologies, Inc. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_l2_send.c_v   1.8   16 May 2002 12:12:32   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_l2_send.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
Sep/24/12  SJW     Aligned with new GERAN L2 interface
May/17/04   VT     In rr_send_dl_establish_req(), in ASSERT for layer3_message
                     if it is NULL, removed "|| SAPI != SAPI0".
11/07/03   guru    Changed message_length check from > 2 to >=2 in rr_send_dl_data_req
02/27/03   RG      SAPI3 messages are not buffered since there is no recovery specified (after handover).
02/18/03   JAC     lint changes
02/14/03   RG      Added support for G2W blind Handover
08/05/02   rmc     Fixed mismatch in dl_suspend_req for forced_suspend flag
07/20/02   Rao     Added InterRAT ( WCDMA To GSM )  Handover support
04/11/02   JAC     added uplink/downlink bit for logging signalling msgs
04/08/02   Rao     Added mdl_release_req sending support
02/20/02   ATM     Merge GS_GSM and GS functions together
02/06/02   rmc     QXDM Log cleanup
11/29/01   ATM     Replaced call to CFA with call to Generic Services (GS) --
                   which will indirectly call CFA.
11/29/01   RMC     Added this header.
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include <string.h>
#include "rr_l2_send.h"
#include "rr_l2.h"
#include "ms.h"
#include "gs.h"
#include "msg.h"
#include "err.h"
#include "gs.h"
#include "rr_log.h"
#include "amssassert.h"
#include "rr_general.h"
#include "rr_sapi3.h"
#include "rr_gprs_debug.h"
#include "rr_multi_sim.h"
#include "rr_ded_config.h"
#include "rr_conn_establish.h"
#include "geran_multi_sim.h"
#include "rr_nv.h"
#include "stringl/stringl.h"

#define RR_SET_MESSAGE_LENGTH(msg)                                  \
  /*lint -e(778) */                                                 \
  PUT_IMH_LEN(sizeof(msg) - sizeof(IMH_T),&((msg).message_header))

#define RR_PREPARE_L2_MESSAGE(msg, msg_id, gas_id)                   \
  RR_ZERO_STRUCTURE(msg);                                            \
  msg.message_header.message_set = MS_RR_L2;                         \
  msg.message_header.message_id  = (byte) msg_id;                    \
  msg.gas_id                     = gas_id;                           \
  RR_SET_MESSAGE_LENGTH(msg);

#define L2_ACK_GTTP  0xff

/*
 *   Function name:  rr_send_message_to_layer_2
 *   ----------------------------------------
 *
 *   Description: Send the message to the layer 2
 *   ------------
 *
 *   Parameters:  Pointer to message
 *   -----------
 *
 *   Returns:     TRUE if the message was successfully passed to L2's queue
 *   --------
 *
 */

static boolean rr_send_message_to_layer_2(
  IMH_T *l2message_ptr,
  const gas_id_t gas_id
)
{
  gs_status_T gs_status;

  RR_NULL_CHECK_FATAL(l2message_ptr);

  rr_show_output_message(
    "L2",
    l2message_ptr,
    l2message_ptr->message_set,
    l2message_ptr->message_id,
    gas_id
  );

  gs_status = gs_send_message(
    GS_QUEUE_LAYER_2,
    (void*)l2message_ptr,
    TRUE
  );

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_2_G("Send to L2 failed(%d,%d)", (int)l2message_ptr->message_set, (int)l2message_ptr->message_id);
    return(FALSE);
  }

  return(TRUE);
}

static void rr_prepare_dl_data_req(
  dl_data_req_T *req_ptr,
  byte *msg_ptr,
  byte msg_len,
  SAPI_T sapi,
  l2_channel_type_T chan_type,
  const gas_id_t gas_id
)
{
  req_ptr->SAPI              = sapi;
  req_ptr->l2_channel_type   = chan_type;
  req_ptr->l3_message_length = msg_len;
    memscpy(req_ptr->layer3_message,sizeof(req_ptr->layer3_message), msg_ptr, msg_len);

  if (msg_ptr[0] == RADIO_RESSOURCES)
  {
    /* log the signalling message sent by RR */
    (void) rr_log_signaling_message(
      chan_type,
      msg_ptr[1],
      msg_len,
      msg_ptr,
      RR_UPLINK_MSG,
      gas_id
    );
  }

  return;
}

static byte rr_get_data_req_msg_number(
  byte *msg_ptr,
  byte msg_len,
  SAPI_T sapi,
  const gas_id_t gas_id
)
{
  byte  msg_num;

  if (msg_ptr != NULL)
  {
    byte protocol_discriminator;

    if (sapi == SAPI3)
    {
      protocol_discriminator = SMS_MESSAGES;
    }
    else
    {
      protocol_discriminator = msg_ptr[0] & 0x0F;
    }

    switch (protocol_discriminator)
    {
      case RADIO_RESSOURCES:
      case TEST_MANAGEMENT:
      {
        msg_num = 0;
        break;
      }

      case SMS_MESSAGES:
      {
        msg_num = rr_sapi3_get_next_message_number(gas_id);
        break;
      }

#ifdef FEATURE_GSM_DTM
      case GTTP:
      {
        msg_num = RR_MSG_ID_GTTP;
        break;
      }
#endif // FEATURE_GSM_DTM

      default:
      {
        msg_num = rr_add_to_nas_message_buffer(
          sapi,       // SAPI
          msg_len,    // l3_message_length
          msg_ptr,    // l3_message
          gas_id      // gas_id
        );
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Invalid L3 message buffer for L2");
    msg_num = 0;
  }

  return(msg_num);
}

/*
 *   Function name:  rr_send_dl_data_req
 *   -----------------------------------
 *
 *   Description: Send the DL_DATA_REQ primitive to layer 2
 *   ------------
 *
 *
 *   Parameters: L3 message, message length, SAPI and channel
 *   -----------
 *
 *
 *
 *   Returns:     byte - the L3 message number passed to Layer 2
 *   --------
 *
 *
 */

byte rr_send_dl_data_req(
  byte *layer3_message,
  byte length,
  SAPI_T SAPI,
  l2_channel_type_T
  l2_channel_type,
  const gas_id_t gas_id
)
{
  if ((layer3_message != NULL) && (length >= 2) && (length <= MAX_OCTETS_L3_MESSAGE))
  {
    dl_data_req_T dl_data_req;

    RR_PREPARE_L2_MESSAGE(dl_data_req, DL_DATA_REQ, gas_id);

    /* prepare the DL_DATA_REQ with the message payload */
    rr_prepare_dl_data_req(
      &dl_data_req,
      layer3_message,
      length,
      SAPI,
      l2_channel_type,
      gas_id
    );

    /* set a L3 message number which L2 will return in DL_DATA_ACK_IND */
    dl_data_req.l3_message_number = rr_get_data_req_msg_number(
      layer3_message,
      length,
      SAPI,
      gas_id
    );

    /* send the message to Layer 2 */
    (void) rr_send_message_to_layer_2(
      (IMH_T *)&dl_data_req,    // l2message_ptr
      gas_id                    // gas_id
    );

    return (dl_data_req.l3_message_number);
  }

  MSG_GERAN_ERROR_3_G("Unable to send SAPI%1d msg to L2 (0x%08X, %d)", (int)SAPI, (int)layer3_message, (int)length);

  return (0);
}

/*
 *   Function name:  rr_send_signalling_msg_with_ack
 *   ----------------------------------------
 *
 *   Description: Sends a SAPI0 DL_DATA_REQ primitive to Layer 2 and requests an ack
 *                Can *only* be used for RR protocol (0x06) L3 signalling messages
 *   ------------
 *
 *   Parameters:  L3 message, message length, and message ID
 *   -----------
 *
 *   Returns:     TRUE if the L3 message was passed to L2 for transmission
 *   --------
 *
 */
boolean rr_send_signalling_msg_with_ack(
  byte *layer3_message,
  byte length,
  byte msg_id,
  const gas_id_t gas_id
)
{
  boolean bSent = FALSE;

  if ((layer3_message != NULL) && (length >= 2) && (length <= MAX_OCTETS_L3_MESSAGE))
  {
    /* set the L3 message number which L2 will return in the DL_DATA_ACK_IND */
    if ((layer3_message[0] == RADIO_RESSOURCES) && IS_RR_SIGNALLING_MSG(msg_id))
    {
      dl_data_req_T dl_data_req;

      RR_PREPARE_L2_MESSAGE(dl_data_req, DL_DATA_REQ, gas_id);

      /* prepare the DL_DATA_REQ with the message payload */
      rr_prepare_dl_data_req(
        &dl_data_req,     // req_ptr
        layer3_message,   // msg_ptr
        length,           // msg_len
        SAPI0,            // sapi
        DCCH,             // l2_channel_type
        gas_id            // gas_id
      );

      /* tag the request with the signalling message ID */
      dl_data_req.l3_message_number = msg_id;

      /* send the message to Layer 2 */
      bSent = rr_send_message_to_layer_2(
        (IMH_T *)&dl_data_req,    // l2message_ptr
        gas_id                    // gas_id
      );
    }
    else
    {
      MSG_GERAN_ERROR_1_G("Invalid msg ID %d in L2 data request", (int)msg_id);
    }
  }
  else
  {
    MSG_GERAN_ERROR_3_G("Unable to send SAPI0 msg to L2 (0x%08X, %d, %d)", (int)layer3_message, (int)length, (int)msg_id);
  }

  return(bSent);
}

/*
 *   Function name:  rr_send_dl_establish_req
 *   ----------------------------------------
 *
 *   Description: Send the DL_ESTABLISH_REQ primitive to layer 2
 *   ------------
 *
 *
 *   Parameters: See 04.06
 *   -----------
 *
 *
 *
 *   Returns:     none
 *   --------
 *
 *
 */


void rr_send_dl_establish_req
(
  SAPI_T SAPI,
  l2_channel_type_T l2_channel_type,
  byte *layer3_message,
  byte message_length,
  establish_mode_T establish_mode,
  const gas_id_t gas_id
)
{
  dl_establish_req_T dl_establish_req;

  RR_PREPARE_L2_MESSAGE(dl_establish_req, DL_ESTABLISH_REQ, gas_id);

  ASSERT(layer3_message != NULL || SAPI != SAPI0);
  if ((layer3_message != NULL) && (message_length > 2))
  {
    byte protocol_discriminator = layer3_message[0] & 0x0F;
    if (protocol_discriminator == RADIO_RESSOURCES)
    {
      // log the signaling message sent by RR
      (void) rr_log_signaling_message(
        l2_channel_type,
        layer3_message[1],
        message_length,
        layer3_message,
        RR_UPLINK_MSG,
        gas_id
      );
    }
  }

  if (SAPI == SAPI0)
  {
    /*****************************************************/
    /* The DL_ESTABLISH_REQ primitice contains a message */
    /* to send with the SABM command                     */
    /*****************************************************/

    dl_establish_req.l3_message_length = MIN(message_length, N201_MAX);

    if ( layer3_message != NULL )
    {
     memscpy(dl_establish_req.layer3_message,
	 	     sizeof(dl_establish_req.layer3_message), 
	 	     layer3_message, 
	 	     dl_establish_req.l3_message_length
	 	     );
    }
    else
    {
      MSG_GERAN_ERROR_0_G("layer3_message is NULL");
    }
  }
  else
  {
    dl_establish_req.l3_message_length = 0;
  }

  dl_establish_req.establish_mode = establish_mode;
  dl_establish_req.l2_channel_type = l2_channel_type;
  dl_establish_req.SAPI = SAPI;

  (void) rr_send_message_to_layer_2(
    (IMH_T *)&dl_establish_req,   // l2message_ptr
    gas_id                        // gas_id
  );
}

/*
 *   Function name:  rr_send_dl_random_access_req
 *   --------------------------------------------
 *
 *   Description: Send the DL_RANDOM_ACCESS_REQ primitive to layer 2
 *   ------------
 *
 *
 *   Parameters: access reference, L2 channel type, retry flag
 *   -----------
 *
 *
 *
 *   Returns:     none
 *   --------
 *
 *
 */

void rr_send_dl_random_access_req(
  byte access_reference,
  l2_channel_type_T l2_channel_type,
  const gas_id_t gas_id
)
{
  dl_random_access_req_T dl_random_access_req;

  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  RR_PREPARE_L2_MESSAGE(dl_random_access_req, DL_RANDOM_ACCESS_REQ, gas_id);

  dl_random_access_req.egprs_ra_content_present = FALSE;
  dl_random_access_req.egprs_ra_content         = 0;
  dl_random_access_req.egprs_training_seq       = 0;

  dl_random_access_req.random_access_content    = access_reference;
  dl_random_access_req.l2_channel_type          = l2_channel_type;
  dl_random_access_req.SAPI                     = SAPI0;

  (void)geran_get_traffic_reason(&(dl_random_access_req.rach_for_ps_traffic),gas_id);

  dl_random_access_req.pwr_reduction_valid = scell_info_ptr->pwr_reduction_valid;
  dl_random_access_req.pwr_reduction_value = scell_info_ptr->int_pwr_red_value;

  /* check random_access_content to see if it is emergency call access type.: 101xxxxx*/
  if (((access_reference & 0xe0) >> 5) == 0x05)
  {
    dl_random_access_req.emergency_call = TRUE;
  }
  else
  {
    dl_random_access_req.emergency_call = FALSE;
  }

  (void) rr_send_message_to_layer_2(
    (IMH_T *)&dl_random_access_req,   // l2message_ptr
    gas_id                            // gas_id
  );

  (void) rr_log_random_access_request(
    rr_ce_access_reason(gas_id),
    access_reference,
    gas_id
  );

  return;
}


/*
 *   Function name:  rr_send_dl_random_access_req_for_egprs
 *   ------------------------------------------------------
 *
 *   Description: Send the DL_RANDOM_ACCESS_REQ primitive to layer 2
 *   ------------
 *
 *
 *   Parameters: access reference, L2 channel, retry flag
 *   -----------
 *
 *
 *
 *   Returns:     none
 *   --------
 *
 *
 */

void rr_send_dl_random_access_req_for_egprs(
  word egprs_access_reference,
  l2_channel_type_T l2_channel_type,
  const gas_id_t gas_id
)
{
  dl_random_access_req_T dl_random_access_req;

  RR_PREPARE_L2_MESSAGE(dl_random_access_req, DL_RANDOM_ACCESS_REQ, gas_id);

  dl_random_access_req.egprs_ra_content_present = TRUE;
  dl_random_access_req.l2_channel_type          = l2_channel_type;
  dl_random_access_req.SAPI                     = SAPI0;
  dl_random_access_req.rach_for_ps_traffic      = TRUE;

  /*Change the order of bit transmission

    44.004 sect 7.7 specifies the PRACH uplink block format.
    44.060 sect 10.0b.3.1 specifies the field mapping of the 11-bit control message on to a physical block

    The 11-bit EGPRS PCR is mapped on to two octets:

    Octet 1 - bit 11 - bit 3 of the EGPRS PCR. -  Lowest order bit of block format is the lowest order bit of EGPRS PCR
    Octet 2 - bit 2 - bit 0 of EGPRS PCR. -  Lowest order bit of block format is the lowest order bit of EGPRS PCR

    The order of transmission from 44.004, sect 7.7 is the lowest to highest order bit of octet 0, then octet 1 etc.

    L1 will transmit the supplied 11-bit egprs_ra_content from the lowest order bit to the highest.

    The EGPR PCR shall therefore be mapped on to the egprs_ra_content as follows:

    egprs ra content
    bit 15 - 11 - not used
    bit 10 - 8  - bit 2-0 of EGPRS PCR
    bit 7  - 0  - bit 3-10 of EGPRS PCR */

  dl_random_access_req.egprs_ra_content = ( ( egprs_access_reference & 0x7f8 ) >> 3 ) + ( ( egprs_access_reference & 0x07 ) << 8 );

  /* determine training seq, 04.60 sect 11.2.5a */
  if ( rr_get_nv_egprs_8psk_ul_enabled(RR_GAS_ID_TO_AS_ID) )
  {
    dl_random_access_req.egprs_training_seq = 1; /* TS 1 - 8PSK avail in UL*/
  }
  else
  {
    dl_random_access_req.egprs_training_seq = 2; /* TS 2 - No 8PSK in UL*/
  }

  (void) rr_send_message_to_layer_2(
    (IMH_T *)&dl_random_access_req,   // l2message_ptr
    gas_id                            // gas_id
  );

  return;
}



/*
 *   Function name:  rr_send_dl_reconnect_req
 *   ----------------------------------------
 *
 *   Description: Send the DL_RECONNECT_REQ primitive to the layer 2
 *   ------------
 *
 *
 *   Parameters: The layer 3 message
 *   -----------
 *
 *
 *
 *   Returns:     none
 *   --------
 *
 *
 */

void rr_send_dl_reconnect_req(
  byte *layer3_message,
  byte message_length,
  const gas_id_t gas_id
)
{
  dl_reconnect_req_T dl_reconnect_req;

  RR_NULL_CHECK_FATAL(layer3_message);

  RR_PREPARE_L2_MESSAGE(dl_reconnect_req, DL_RECONNECT_REQ, gas_id);

  dl_reconnect_req.SAPI = SAPI0;
  dl_reconnect_req.l2_channel_type = DCCH;
  dl_reconnect_req.l3_message_length = message_length;

  memscpy(dl_reconnect_req.layer3_message,sizeof(dl_reconnect_req.layer3_message),layer3_message,message_length);


  (void) rr_send_message_to_layer_2(
    (IMH_T *)&dl_reconnect_req,   // l2message_ptr
    gas_id                        // gas_id
  );

  if (message_length > 2)
  {
    byte protocol_discriminator = layer3_message[0] & 0x0F;
    if (protocol_discriminator == RADIO_RESSOURCES)
    {
      /* log the signalling message */
      (void) rr_log_signaling_message(
        DCCH,               // channel_type
        layer3_message[1],  // message_type
        message_length,     // message_length
        layer3_message,     // message_data
        RR_UPLINK_MSG,      // onDownlink
        gas_id              // gas_id
      );
    }
  }
}

/*
 *   Function name:  rr_send_dl_release_req
 *   --------------------------------------
 *
 *   Description: Send the DL_RELEASE_REQ primitive to layer 2
 *   ------------
 *
 *
 *   Parameters: See 04.06
 *   -----------
 *
 *
 *
 *   Returns:     none
 *   --------
 *
 *
 */

void rr_send_dl_release_req(
  release_mode_T release_mode,
  byte SAPI,
  byte l2_channel_type,
  const gas_id_t gas_id
)
{
  dl_release_req_T dl_release_req;

  RR_PREPARE_L2_MESSAGE(dl_release_req, DL_RELEASE_REQ, gas_id);

  dl_release_req.release_mode = release_mode;
  dl_release_req.l2_channel_type = l2_channel_type;
  dl_release_req.SAPI = SAPI;

  (void) rr_send_message_to_layer_2(
    (IMH_T *)&dl_release_req,   // l2message_ptr
    gas_id                      // gas_id
  );
}

/*
 *   Function name:  rr_send_mdl_release_req
 *   --------------------------------------
 *
 *   Description: Send the MDL_RELEASE_REQ primitive to layer 2
 *   ------------
 *
 *
 *   Parameters: See 04.06
 *   -----------
 *
 *
 *
 *   Returns:     none
 *   --------
 *
 *
 */

void rr_send_mdl_release_req(
  byte SAPI,
  byte l2_channel_type,
  const gas_id_t gas_id
)
{
  mdl_release_req_T mdl_release_req;

  RR_PREPARE_L2_MESSAGE(mdl_release_req, MDL_RELEASE_REQ, gas_id);

  mdl_release_req.l2_channel_type = l2_channel_type;
  mdl_release_req.SAPI = SAPI;

  (void) rr_send_message_to_layer_2(
    (IMH_T *)&mdl_release_req,    // l2message_ptr
    gas_id                        // gas_id
  );
}

/*
 *   Function name:  rr_get_resumption_channel_type
 *   --------------------------------------
 *
 *   Description: Determine which channel type L2 should use for resumption
 *   ------------
 *
 *
 *   Parameters: None
 *   -----------
 *
 *
 *   Returns:    FACCH_F (full-rate), or
 *               FACCH_H (half-rate)  or
 *               DCCH    (signalling only)
 *   --------
 *
 *
 */

static l2_channel_type_T rr_get_resumption_channel_type(const gas_id_t gas_id)
{
  l2_channel_type_T l2_channel_type;
  channel_spec_T   *channel_spec_ptr;
  channel_type_T    rr_channel_type;

  /* determine which channel type was specified */
  /* in the last HANDOVER or ASSIGNMENT command */
  channel_spec_ptr = rr_get_channel_spec(NULL, gas_id);
  if (channel_spec_ptr != NULL)
  {
#ifndef PERLUTF
    rr_channel_type = channel_spec_ptr->channel_info_1_after.channel_type;
#else
    #error code not present
#endif /* PERLUTF */
  }
  else
  {
    /* can't access the channel spec, default to a signalling-only type */
    rr_channel_type = SDCCH_4;
    MSG_GERAN_ERROR_0_G("RR unable to determine channel type for L2");
  }

  /* now map this RR channel type to an appropriate value for L2 */
  if (rr_channel_type == FULL_RATE_TRAFFIC)
  {
    /* resumption occurs on a full-rate traffic channel (FACCH) */
    l2_channel_type = FACCH_F;
  }
  else if (rr_channel_type == HALF_RATE_TRAFFIC)
  {
    /* resumption occurs on a half-rate traffic channel (FACCH) */
    l2_channel_type = FACCH_H;
  }
  else
  {
    /* resumption occurs on a signalling-only channel */
    l2_channel_type = DCCH;
  }

  return(l2_channel_type);
}

/*
 *   Function name:  send_dl_resume_req
 *   --------------------------------------
 *
 *   Description: Send the DL_RESUME_REQ primitive to Layer 2
 *   ------------
 *
 *
 *   Parameters: The Layer 3 message
 *   -----------
 *
 *
 *
 *   Returns:    None
 *   --------
 *
 *
 */

static void send_dl_resume_req(
  rr_l1_handover_type_T resume_reason,
  #ifdef FEATURE_GSM_DTM
  boolean l1_already_connected,
  #endif /* FEATURE_GSM_DTM */
  byte *layer3_message,
  byte message_length,
  const gas_id_t gas_id
)
{
  dl_resume_req_T dl_resume_req;

  RR_PREPARE_L2_MESSAGE(dl_resume_req, DL_RESUME_REQ, gas_id);

  dl_resume_req.l3_message_length = message_length;
  dl_resume_req.SAPI = SAPI0;
  dl_resume_req.l2_channel_type = rr_get_resumption_channel_type(gas_id);

  if (resume_reason == RR_L1_WCDMA_GSM)
  {
    /* force the resumption for handovers where GSM was previously inactive */
    dl_resume_req.forced_resume = TRUE;
  }
#ifdef FEATURE_GSM_TDS
  else if (resume_reason == RR_L1_TDS_GSM)
  {
    /* force the resumption for handovers where GSM was previously inactive */
    dl_resume_req.forced_resume = TRUE;
  }
#endif /*FEATURE_GSM_TDS*/
  else if (resume_reason == RR_L1_LTE_GSM)
  {
    /* force the resumption for handovers where GSM was previously inactive */
    dl_resume_req.forced_resume = TRUE;
  }
  else
  {
    /* do not force the resumption for other handovers */
    dl_resume_req.forced_resume = FALSE;
  }
   memscpy(dl_resume_req.layer3_message,sizeof(dl_resume_req.layer3_message),layer3_message,message_length);

#ifdef FEATURE_GSM_DTM
  dl_resume_req.l1_already_connected = l1_already_connected;
#endif /* FEATURE_GSM_DTM */

  (void) rr_send_message_to_layer_2(
    (IMH_T *)&dl_resume_req,    // l2message_ptr
    gas_id                      // gas_id
  );

  if (message_length > 2)
  {
    byte protocol_discriminator = layer3_message[0] & 0x0F;
    if (protocol_discriminator == RADIO_RESSOURCES)
    {
      // log the signaling message sent by RR
      (void) rr_log_signaling_message(
        DCCH,               // channel_type
        layer3_message[1],  // message_type
        message_length,     // message_length
        layer3_message,     // message_data
        RR_UPLINK_MSG,      // onDownlink
        gas_id              // gas_id
      );
    }
  }

} /* end send_dl_resume_req() */


void rr_send_dl_resume_req(
  rr_l1_handover_type_T resume_reason,
  byte *layer3_message,
  byte message_length,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_FATAL(layer3_message);

  send_dl_resume_req(
    resume_reason,      // resume_reason
#ifdef FEATURE_GSM_DTM
    FALSE,              // l1_already_connected
#endif /* FEATURE_GSM_DTM */
    layer3_message,     // layer3_message
    message_length,     // message_length
    gas_id              // gas_id
  );

} /* end rr_send_dl_resume_req() */


/*
 *   Function name:  rr_send_dl_suspend_req
 *   --------------------------------------
 *
 *   Description: Send the DL_SUSPEND_REQ primitive to the layer 2
 *   ------------
 *
 *
 *   Parameters: suspend_reason ( applicable only when InterRAT HO is supported )
 *   -----------
 *
 *
 *
 *   Returns:   none
 *   --------
 *
 *
 */

void rr_send_dl_suspend_req(
  rr_l1_handover_type_T suspend_reason,
  const gas_id_t gas_id
)
{
  dl_suspend_req_T dl_suspend_req;

  RR_PREPARE_L2_MESSAGE(dl_suspend_req, DL_SUSPEND_REQ, gas_id);

  if (suspend_reason == RR_L1_WCDMA_GSM)
  {
    /* force the suspension for handovers where GSM was previously inactive */
    dl_suspend_req.forced_suspend = TRUE;
  }
#ifdef FEATURE_GSM_TDS
  else if (suspend_reason == RR_L1_TDS_GSM)
  {
    /* force the suspension for handovers where GSM was previously inactive */
    dl_suspend_req.forced_suspend = TRUE;
  }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
  else if (suspend_reason == RR_L1_LTE_GSM)
  {
    /* force the suspension for handovers where GSM was previously inactive */
    dl_suspend_req.forced_suspend = TRUE;
  }
#endif /* FEATURE_LTE */
  else
  {
    /* do not force the suspension for other handovers */
    dl_suspend_req.forced_suspend = FALSE;
  }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (dl_suspend_req.forced_suspend == TRUE)
  {
    /* advise the multi-SIM controller that a CS connection exists */
    rr_ms_cs_connected(TRUE, gas_id);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  /* avoid RLT occuring during suspension of Layer 2 */
  if (suspend_reason == RR_L1_GSM_GSM)
  {
    uint8 S_counter;

    S_counter = rr_increment_S_counter(gas_id);
    rr_log_s_counter(S_counter, gas_id);
  }

  dl_suspend_req.SAPI = SAPI0;
  dl_suspend_req.l2_channel_type = DCCH;

  (void) rr_send_message_to_layer_2(
    (IMH_T *)&dl_suspend_req,   // l2message_ptr
    gas_id                      // gas_id
  );
}

/*
 *   Function name:  rr_send_dl_unit_data_req
 *   ----------------------------------------
 *
 *   Description: Send the DL_UNIT_DATA_REQ primitive to the layer 2
 *   ------------
 *
 *
 *   Parameters: See 04.06
 *   -----------
 *
 *
 *
 *   Returns:     none
 *   --------
 *
 *
 */

void rr_send_dl_unit_data_req(
  byte *layer3_message,
  byte message_length,
  byte SAPI,
  byte l2_channel_type,
  boolean short_header,
  const gas_id_t gas_id
)
{
  dl_unit_data_req_T dl_unit_data_req;

  RR_PREPARE_L2_MESSAGE(dl_unit_data_req, DL_UNIT_DATA_REQ, gas_id);

  dl_unit_data_req.SAPI = SAPI;
  dl_unit_data_req.l2_channel_type = l2_channel_type;
  dl_unit_data_req.l3_message_length = message_length;
  dl_unit_data_req.short_header = short_header;
  memscpy(dl_unit_data_req.layer3_message,
  	      sizeof(dl_unit_data_req.layer3_message),
  	      layer3_message,
  	      message_length
  	      );


  (void) rr_send_message_to_layer_2(
    (IMH_T *)&dl_unit_data_req,   // l2message_ptr
    gas_id                        // gas_id
  );

  if (short_header)
  {
    /* For short header (EMR) message type is in the first byte,
    0 - RR PD
    00100 - Message Type (EMR)
    00 - L2 short header
    */
    (void) rr_log_signaling_message(
      l2_channel_type,                          // channel_type
      (byte)((layer3_message[0] & 0x7c) >> 2),  // message_type
      message_length,                           // message_length
      layer3_message,                           // message_data
      RR_UPLINK_MSG,                            // onDownlink
      gas_id                                    // gas_id
    );
  }
  else
  {
    (void) rr_log_signaling_message(
      l2_channel_type,      // channel_type
      layer3_message[1],    // message_type
      message_length,       // message_length
      layer3_message,       // message_data
      RR_UPLINK_MSG,        // onDownlink
      gas_id                // gas_id
    );
  }
}


void rr_l2_send_l3_msg_ota(
  byte *l3_msg,
  uint8 l3_msg_len,
  const gas_id_t gas_id
)
{
  (void) rr_send_dl_data_req(
    l3_msg,       // layer3_message
    l3_msg_len,   // length
    SAPI0,        // SAPI
    DCCH,         // l2_channel_type
    gas_id        // gas_id
  );
}


void rr_l2_resume_l2_and_send_l3_msg_ota(
  byte *l3_msg,
  uint8 l3_msg_len,
  boolean l1_already_connected,
  const gas_id_t gas_id
)
{
  send_dl_resume_req(
    RR_L1_GSM_GSM,          // resume_reason
#ifdef FEATURE_GSM_DTM
    l1_already_connected,   // l1_already_connected
#endif /* FEATURE_GSM_DTM */
    l3_msg,                 // layer3_message
    l3_msg_len,             // message_length
    gas_id                  // gas_id
  );
}


void rr_l2_reconnect_l2_and_send_l3_msg_ota(
  byte *l3_msg,
  uint8 l3_msg_len,
  const gas_id_t gas_id
)
{
  rr_send_dl_reconnect_req(l3_msg, l3_msg_len, gas_id);
}


void rr_l2_restart_l2_and_send_l3_msg_ota(
  rr_l2_resumption_method_t  method,
  byte *l3_msg,
  uint8 l3_msg_len,
  const gas_id_t gas_id
)
{
  switch (method)
  {
    case RR_L2_RECONNECT_REQUIRED:
    {
      rr_l2_reconnect_l2_and_send_l3_msg_ota(
        l3_msg,       // l3_msg
        l3_msg_len,   // l3_msg_len
        gas_id        // gas_id
      );
      break;
    }

    case RR_L2_RESUME_REQUIRED:
    {
      rr_l2_resume_l2_and_send_l3_msg_ota(
        l3_msg,       // l3_msg
        l3_msg_len,   // l3_msg_len
        FALSE,        // l1_already_connected
        gas_id        // gas_id
      );
      break;
    }

    case RR_L2_RESUME_REQUIRED_L1_ALREADY_CONNECTED:
    {
      rr_l2_resume_l2_and_send_l3_msg_ota(
        l3_msg,       // l3_msg
        l3_msg_len,   // l3_msg_len
        TRUE,         // l1_already_connected
        gas_id        // gas_id
      );
      break;
    }

    default:
    {
      MSG_GERAN_ERROR_1_G("Invalid rr_l2_resumption_method (%d)",method);
    }
  }
}

/*
 *   Function name:  rr_send_status
 *   -----------------------------
 *
 *   Description: Send the RR_STATUS peer to peer message to the network
 *   ------------
 *
 *
 *   Parameters: the RR Cause
 *   -----------
 *
 *
 *
 *   Returns:     none
 *   --------
 *
 *
 */

void rr_send_status(
  byte RR_cause,
  const gas_id_t gas_id
)
{
  byte layer3_message[RR_STATUS_LEN];

  /*************************************/
  /* Fill in the PD and Skip Indicator */
  /*************************************/
  layer3_message[0] = RADIO_RESSOURCES;

  /****************************/
  /* Fill in the message type */
  /****************************/
  layer3_message[1] = RR_STATUS;

  /************************/
  /* Fill in the RR_cause */
  /************************/
  layer3_message[2] = RR_cause;

  (void) rr_send_dl_data_req(
    layer3_message,   // layer3_message
    RR_STATUS_LEN,    // length
    SAPI0,            // SAPI
    DCCH,             // l2_channel_type
    gas_id            // gas_id
  );
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*
 *   Function name:  rr_send_dl_data_mux_ind
 *   ----------------------------------------
 *
 *   Description: Send the DL_DATA_MUX_IND to GL2 for registering call backs for channel sharing.
 *   ------------
 *
 *
 *   Parameters: CCCH callback, CBCH callback, GAS ID
 *   -----------
 *
 *
 *
 *   Returns:     none
 *   --------
 *
 *
 */

void rr_send_dl_data_mux_ind(
  l2_mux_cb_fn_t ccch_callback,
  l2_mux_cb_fn_t cbch_callback,
  const gas_id_t gas_id
)
{
  dl_data_mux_ind_T dl_data_mux_ind;

  dl_data_mux_ind.message_header.message_set = MS_RR_L2;
  dl_data_mux_ind.message_header.message_id  = (byte)DL_DATA_MUX_IND;

  PUT_IMH_LEN(sizeof(dl_data_mux_ind_T) - sizeof(IMH_T), &(dl_data_mux_ind.message_header));

  dl_data_mux_ind.ccch_callback = ccch_callback;
  dl_data_mux_ind.cbch_callback = cbch_callback;
  dl_data_mux_ind.gas_id        = gas_id;
  
  if (GS_SUCCESS != gs_send_message(GS_QUEUE_LAYER_2, (void *)&dl_data_mux_ind, FALSE))
  {
    MSG_GERAN_ERROR_0_G("Unable to configure demultiplexer in Layer 2");
  }

  return;
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */


/* EOF */

