/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Cipher Module

GENERAL DESCRIPTION
  Handles the RR Ciphering procedure

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_cipher.c_v   1.6   23 May 2002 14:08:24   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_cipher.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/22/09    sjw     Added support for FEATURE_UMTS_VOICE_CIPHERING_IND
05/13/09    sjw     Start relocation of dedicated mode data to a single structure
02/13/09    sjw     Reworked local storage for ciphering module data
09/02/08    sjw     Clean up legacy code & comments
05/07/04    gc      Enhanced Trace Buffer Code
05/08/03    he      Used separate debug_str buffer for logging.
02/05/03    JAC     lint cleanup
01/23/03    he      Fixed lint errors
08/26/02    rmc     Removed FEATURE_MULTIMODE_GSM feature use
05/22/02    rmc     Changes to MM-RR interface for dual mode
03/28/02    rmc     Temporary solution for CR21280
02/08/02    rmc     QXDM Cleanup
02/04/02    rao     Started using new local rr sim info accessor functions.
01/28/02    RMC     Added support for logging of ciphering information
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include <string.h>
#include "rr_seg_load.h"
#include "rr_cipher.h"
#include "rr_l1_send.h"
#include "rr_l2_send.h"
#include "rr_mm_send.h"
#include "rr_decode_ie.h"
#include "ms.h"
#include "rr_l2.h"
#include "cause.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "rr_general.h"
#include "rr_log.h"
#include "rr_conn_establish.h"
#include "rr_gprs_debug.h"
#include "rr_ded_config.h"
#include "rr_sim.h"
#ifdef FEATURE_WCDMA
#include "rr_rrc_send.h"
#endif /* FEATURE_WCDMA */
#include "rr_multi_sim.h"

#define RR_CIPHER_KEY_LENGTH MM_SIM_EFKC_SIZE /* from mm_as_types.h */
#define RR_OPTIONAL_ID_LENGTH 10

/* enum definition for this module's state machine */
typedef enum
{
  CIPHER_INACTIVE,
  CIPHER_MODE_CHANGE_REQUESTED
} cipher_state_T;

typedef struct
{
  cipher_state_T cipher_state;
  cipher_state_T old_cipher_state;
} cipher_state_data_t;

#define _CIPHER_STATE_DATA_INIT {CIPHER_INACTIVE, CIPHER_INACTIVE}

static cipher_state_data_t  cipher_state_data[NUM_GERAN_DATA_SPACES] =
                               { INITIAL_VALUE(_CIPHER_STATE_DATA_INIT) };
#undef _CIPHER_STATE_DATA_INIT

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return cipher_state_data_t*
 */
static cipher_state_data_t *rr_cipher_get_data_ptr(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &cipher_state_data[as_index];
}

/*===========================================================================

FUNCTION  rr_set_ciphering_mode_command_received
  Updates the cipher_mode_command_received flag
===========================================================================*/
void rr_set_ciphering_mode_command_received(
  boolean cmd_rcvd,
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T *dedic_ptr;

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr != NULL)
  {
    dedic_ptr->cipher_mode_command_received = cmd_rcvd;
  }

  return;
}

/*===========================================================================

FUNCTION  rr_get_ciphering_mode_command_received
  Returns the current value of the cipher_mode_command_received flag
===========================================================================*/
boolean rr_get_ciphering_mode_command_received(
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T *dedic_ptr;

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr != NULL)
  {
    return(dedic_ptr->cipher_mode_command_received);
  }

  return(FALSE);
}

/*===========================================================================

FUNCTION  rr_ciphering_has_started
  Updates the ciphering_started flag
===========================================================================*/
void rr_ciphering_has_started(
  boolean ciphering_started,
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T *dedic_ptr;

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr != NULL)
  {
    dedic_ptr->ciphering_started = ciphering_started;
  }

  return;
}

/*===========================================================================

FUNCTION  rr_has_ciphering_started
  Returns the current value of the ciphering_started flag
===========================================================================*/
boolean rr_has_ciphering_started(
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T *dedic_ptr;

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr != NULL)
  {
    return(dedic_ptr->ciphering_started);
  }

  return(FALSE);
}

/*===========================================================================

FUNCTION  rr_show_cipher_state
  Displays a change in cipher state
===========================================================================*/

static char *rr_cipher_state_name(
  cipher_state_T log_state
)
{
  char *s = "";

  switch(log_state)
  {
    case (CIPHER_INACTIVE):
      s = "Inactive";
      break;

    case (CIPHER_MODE_CHANGE_REQUESTED):
      s = "ChangeReq";
      break;

    default:
      s = "Unknown";
      break;
  }

  return s;
} /* rr_cipher_state_name */

/*===========================================================================

FUNCTION  rr_get_cipher_key_sequence_number

DESCRIPTION Returns the current CKSN from the local SIM cache

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
byte rr_get_cipher_key_sequence_number(
  const gas_id_t gas_id
)
{
  byte ciph_key_seq_no;

  /******************************************************/
  /* Get the ciphering key sequence number from the SIM */
  /******************************************************/

  if(!rr_siminfo_get_cipher_key_seq(&ciph_key_seq_no, gas_id))
  {
    /*******************************************************/
    /* That means probably that the ciphering key sequence */
    /* number is not available. In that case, it has to be */
    /* coded as 111b (0x07)                                */
    /*******************************************************/
    ciph_key_seq_no = RR_CIPHER_KEY_SEQ_NOT_AVAILABLE;
  }

  return(ciph_key_seq_no);
} /* rr_get_cipher_key_sequence_number */

/*===========================================================================

FUNCTION  rr_get_cipher_key

DESCRIPTION Checks whether a key sequence number is available, and if so,
            returns the cipher key

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static boolean rr_get_cipher_key(byte *ciph_key_ptr, byte destsize_64,
                                 byte *cipher_key_128, byte destsize_128,
                                 const gas_id_t gas_id)
{
  if ((rr_get_cipher_key_sequence_number(gas_id) != RR_CIPHER_KEY_SEQ_NOT_AVAILABLE) &&
      (ciph_key_ptr != NULL))
  {
    return(rr_siminfo_get_cipher_key(ciph_key_ptr,destsize_64,cipher_key_128,destsize_128, gas_id));
  }

  return(FALSE);
}
/*===========================================================================

FUNCTION  rr_algorithm_support_status

DESCRIPTION
  Checks the HW classmark info for A5 algorithm support

DEPENDENCIES
  None

RETURN VALUE
  RR status value as follows:
  - RR_NORMAL_EVENT                  the specified algorithm IS supported
  - RR_PROTOCOL_ERROR_UNSPECIFIED    the algorithm is NOT supported
  - RR_INVALID_MANDATORY_INFORMATION the algorithm ID is a reserved value

SIDE EFFECTS
  None

===========================================================================*/
static byte rr_algorithm_support_status(
  cipher_algorithm_T a5_type,
  const gas_id_t gas_id
)
{
  hw_classmark_T hw_classmark;
  boolean        supported;
  byte           status = RR_PROTOCOL_ERROR_UNSPECIFIED;

  /* Read the HW Classmark from NV */
  /* Note: If the NV cannot be read, then default values are substituted, so */
  /* the return given by the function is always "success" (no need to check) */
  // TODO DSDA what to do here?
  (void)ghdi_read_hw_classmark(&hw_classmark);

  switch(a5_type)
  {
  case ALGORITHM_A5_1:
    supported = hw_classmark.A5_1_supported;
    break;

  case ALGORITHM_A5_2:
    supported = hw_classmark.A5_2_supported;
    /* The A5/2 algorithm was broken the same month it was published, */
    /* so output a warning debug message here. Eventually, RR should  */
    /* probably block any NW command to use this algorithm altogether */
    MSG_GERAN_HIGH_1_G("WARNING: insecure A5_2 algorithm commanded(%1d)",(int)supported);
    break;

  case ALGORITHM_A5_3:
    supported = hw_classmark.A5_3_supported;
    break;

  case ALGORITHM_A5_4:
    supported = hw_classmark.A5_4_supported;
    break;

  case ALGORITHM_A5_5:
    supported = hw_classmark.A5_5_supported;
    break;

  case ALGORITHM_A5_6:
    supported = hw_classmark.A5_6_supported;
    break;

  case ALGORITHM_A5_7:
    supported = hw_classmark.A5_7_supported;
    break;

  case 0x07:
    status = RR_INVALID_MANDATORY_INFORMATION;
    supported = FALSE;
    break;

  default:
    supported = FALSE;
    break;
  }

  if (supported)
  {
    status = RR_NORMAL_EVENT;
  }
  else
  {
#ifndef PERLUTF
    MSG_GERAN_ERROR_1_G("Cipher algorithm %d reserved or not supported", (int)a5_type);
#else
    #error code not present
#endif /* PERLUTF */
  }

  return(status);
}

/*===========================================================================

FUNCTION  rr_cipher_mode_supported

DESCRIPTION
   Determines whether the classmark supports the specified ciphering mode

DEPENDENCIES

RETURN VALUE
   TRUE if the algorithm is supported, FALSE otherwise

SIDE EFFECTS
   Indirectly calls classmark read function which involves INTLOCK()

===========================================================================*/
boolean rr_cipher_mode_supported(
  cipher_algorithm_T cipher_mode,
  const gas_id_t gas_id
)
{
  boolean supported;

  if (rr_algorithm_support_status(cipher_mode, gas_id) == RR_NORMAL_EVENT)
  {
    supported = TRUE;
  }
  else
  {
    supported = FALSE;
  }

  return(supported);
}

/*===========================================================================

FUNCTION  rr_respond_to_cipher_command

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static rr_event_T rr_respond_to_cipher_command(
  const gas_id_t gas_id
)
{
  ie_descriptor_T *current_IE;
  rr_event_T       cipher_event = EV_NO_EVENT;

  /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(CIPHERING_MODE_COMMAND, gas_id);

  if (current_IE != NULL)
  {
    byte rr_status = RR_NORMAL_EVENT;

    // IE is MANDATORY
    if (current_IE->start_IE != NULL)
    {
      rr_dedicated_data_T *dedic_ptr;
      cipher_algorithm_T   algorithm_identifier;
      boolean              key_available = FALSE;
      boolean              start_ciphering = FALSE;
      byte                 cipher_key[RR_CIPHER_KEY_LENGTH];
      byte                 cipher_key_128[MM_GSM_128BIT_KC_SIZE];
      byte                 decoding_ok = TRUE;

      /* Initialise algorithm_identifier to an 'undefined' value. */
      GS_SET_UNDEFINED(algorithm_identifier);

      if ((*(current_IE->start_IE) & 0x01) == 0x01)
      {
        /* Start of ciphering has been ordered: decode the algorithm identifier */
        algorithm_identifier = ((byte) (*(current_IE->start_IE) & 0x0E) >> 1);

        /* check for MS support of the specified algorithm */
        rr_status = rr_algorithm_support_status(algorithm_identifier, gas_id);

        if (rr_status == RR_NORMAL_EVENT)
        {
          MSG_GERAN_HIGH_1_G("Start ciphering with A5_%1d",(int)algorithm_identifier+1);
          start_ciphering = TRUE;
        }
        else
        {
          decoding_ok = FALSE;
        }

        {
          geran_eng_mode_gsm_cipher_info_t gsm_cipher_info;

          switch (algorithm_identifier)
          {
            case ALGORITHM_A5_1:
              gsm_cipher_info.cipher_alg = ENG_MODE_GSM_CIPHER_A5_1;
            break;
            case ALGORITHM_A5_2:
              gsm_cipher_info.cipher_alg = ENG_MODE_GSM_CIPHER_A5_2;
            break;
            case ALGORITHM_A5_3:
              gsm_cipher_info.cipher_alg = ENG_MODE_GSM_CIPHER_A5_3;
            break;
            case ALGORITHM_A5_4:
              gsm_cipher_info.cipher_alg = ENG_MODE_GSM_CIPHER_A5_4;
            break;
            case ALGORITHM_A5_5:
              gsm_cipher_info.cipher_alg = ENG_MODE_GSM_CIPHER_A5_5;
            break;
            case ALGORITHM_A5_6:
              gsm_cipher_info.cipher_alg = ENG_MODE_GSM_CIPHER_A5_6;
            break;
            case ALGORITHM_A5_7:
              gsm_cipher_info.cipher_alg = ENG_MODE_GSM_CIPHER_A5_7;
            break;
            default:
              gsm_cipher_info.cipher_alg = ENG_MODE_GSM_CIPHER_NONE;
            break;
          }

          geran_eng_mode_data_write(ENG_MODE_GSM_CIPHER_INFO,(void*)&gsm_cipher_info, TRUE, gas_id);
        }
      }
      else
      {
        MSG_GERAN_HIGH_0_G("Cipher mode command with SC=0 (no ciphering)");
        {
          geran_eng_mode_gsm_cipher_info_t gsm_cipher_info;

          gsm_cipher_info.cipher_alg = ENG_MODE_GSM_CIPHER_NONE;

          geran_eng_mode_data_write(ENG_MODE_GSM_CIPHER_INFO,(void*)&gsm_cipher_info, TRUE, gas_id);
        }
      }

      if (decoding_ok)
      {
        /* record the current ciphering status for this connection */
        rr_ciphering_has_started(start_ciphering, gas_id);

        /* obtain pointer to global dedicated mode storage */
        dedic_ptr = rr_get_dedicated_data_ptr(gas_id);

        RR_NULL_CHECK_FATAL(dedic_ptr);

        if ((byte)(*(current_IE->start_IE) & 0x10) >> 4)
        {
          dedic_ptr->IMEI_requested_by_nw_command = TRUE;
        }
        else
        {
          dedic_ptr->IMEI_requested_by_nw_command = FALSE;
        }

        if (((dedic_ptr->curr_channel_spec.cipher_flag == FALSE) &&
             (start_ciphering == TRUE)) ||
            ((dedic_ptr->curr_channel_spec.cipher_flag == FALSE) &&
             (start_ciphering == FALSE)) ||
            ((dedic_ptr->curr_channel_spec.cipher_flag == TRUE)  &&
             (start_ciphering == FALSE)))
        {

          key_available = rr_get_cipher_key(cipher_key,sizeof(cipher_key),cipher_key_128,sizeof(cipher_key_128), gas_id);

          if (key_available == FALSE)
          {
            if (start_ciphering == TRUE)
            {
              rr_send_status(RR_PROTOCOL_ERROR_UNSPECIFIED, gas_id);
            }

            MSG_GERAN_HIGH_0_G("Cipher key or seq number not available");
          }
          else
          {
#ifdef FEATURE_WCDMA
            /* key is available, advise RRC */
            rr_send_cipher_sync_ind(gas_id);
#endif /* FEATURE_WCDMA */
          }

          /*
           * if not ciphering, can still send message to L1 so that response
           * is elicited in order to respond to network
           */
          if ((key_available == TRUE) || (start_ciphering == FALSE))
          {
            (void)rr_log_ciphering_mode((byte) start_ciphering,(byte) algorithm_identifier, gas_id);

            rr_send_mph_set_ciphering_req(start_ciphering,algorithm_identifier,
                                           cipher_key,cipher_key_128, gas_id);

            dedic_ptr->curr_channel_spec.cipher_flag = start_ciphering;
            dedic_ptr->curr_channel_spec.cipher_algorithm = algorithm_identifier;
            dedic_ptr->cipher_mode_command_received = TRUE;

            cipher_event = EV_CIPHER_COMMAND_CHECKED;
          }

          rr_status = RR_NORMAL_EVENT;
        }
        else
        {
          /******************************************************************/
          /* The message is regarded as erroneous. An error status message  */
          /* with cause "Protocol Error Unspecified" has to be sent         */
          /******************************************************************/
          rr_status = RR_PROTOCOL_ERROR_UNSPECIFIED;
        }
      }
    }
    else
    {
      rr_status = RR_INVALID_MANDATORY_INFORMATION;
    }

    if (rr_status != RR_NORMAL_EVENT)
    {
      MSG_GERAN_HIGH_1_G("OMsg(NW,RR_STATUS(0x%02X))",(int)rr_status);
      rr_send_status(rr_status, gas_id);
    }

    (void)rr_log_channel_configuration(gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
  }

  return(cipher_event);
}

/*===========================================================================

FUNCTION  rr_confirm_ciphering_completed

DESCRIPTION Sends a RR_SYNC_IND to NAS with cause MM_RR_CIPHERING

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS See 3GPP 24.008 section 4.5.1.1

===========================================================================*/

void rr_confirm_ciphering_completed(
  const gas_id_t gas_id
)
{
  channel_spec_T *chan_spec_ptr;

  chan_spec_ptr = rr_get_channel_spec(NULL, gas_id);

  if (chan_spec_ptr != NULL)
  {
    /* According to 3GPP 24.008 4.5.1.1, NAS may use this signal */
    /* as an explicit acceptance of its last CM_SERVICE_REQUEST  */
    rr_send_sync_ind(
      MM_RR_CIPHERING,
      chan_spec_ptr->channel_info_1_after.channel_type,
      chan_spec_ptr->channel_mode_1,
      chan_spec_ptr->channel_info_1_after.subchannel,
      gas_id
    );
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Unable to determine current RR channel spec");
  }

  return;
}

/*===========================================================================

FUNCTION  rr_complete_ciphering

DESCRIPTION Sends a CIPHERING MODE COMPLETE message to the network (via L2)

DEPENDENCIES

RETURN VALUE TRUE if the caller should confirm ciphering with NAS

SIDE EFFECTS

===========================================================================*/
static boolean rr_complete_ciphering(
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T *dedic_ptr;
  boolean              send_sync_now = TRUE;
  byte                 layer3_message[N201_DCCH];
  byte                 msg_len = 0;

  /************************************************************/
  /* Encode the Protocol Discriminator and the Skip Indicator */
  /************************************************************/
  layer3_message[msg_len] = RADIO_RESSOURCES;
  msg_len++;

  /***************************/
  /* Encode the message type */
  /***************************/
  layer3_message[msg_len] = CIPHERING_MODE_COMPLETE;
  msg_len++;

  /**************************************************************************/
  /* If the NW requested our IMEI in the command, encode it in the response */
  /**************************************************************************/
  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr != NULL)
  {
    if (dedic_ptr->IMEI_requested_by_nw_command == TRUE)
    {
      byte imeisv[16];
      byte imei_index;

      layer3_message[msg_len] = OPTIONAL_MOBILE_IDENTITY_IEI;
      msg_len++;

      // TODO DSDA
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
      ghdi_ds_read_imeisv(imeisv, geran_map_gas_id_to_nas_id(gas_id));
#else
      read_imeisv(imeisv);
#endif /* FEATURE_DUAL_SIM && FEATURE_DUAL_IMEI */

      for (imei_index = 0; imei_index < RR_OPTIONAL_ID_LENGTH; imei_index++)
      {
        layer3_message[msg_len] = imeisv[imei_index];
        msg_len++;
      }
    }
  }

  MSG_GERAN_HIGH_0_G("OMsg(NW,CIPHERING_MODE_COMPLETE)");

  /* for some connection types, RR signalling needs to complete first, */
  /* so ciphering should only trigger the RR_SYNC_IND to NAS when L2   */
  /* has sent DL_DATA_ACK_IND for this CIPHERING_MODE_COMPLETE message */
  if (rr_ce_access_reason(gas_id) == (byte)MM_RR_SHORT_MESSAGE)
  {
    if (rr_send_signalling_msg_with_ack(layer3_message, msg_len, RR_MSG_ID_CIPHER_MODE_COMPLETE, gas_id))
    {
      /* wait for the ack from L2 and then send the sync_ind */
      send_sync_now = FALSE;
    }
  }
  else
  {
    /* no need to wait for the L2 ack, but same interface can be used */
    (void)rr_send_signalling_msg_with_ack(layer3_message, msg_len, 0, gas_id);
  }

  return(send_sync_now);
}

/*===========================================================================

FUNCTION  rr_cipher_control

DESCRIPTION Main state machine for dedicated mode ciphering procedures

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
rr_event_T rr_cipher_control(
  rr_event_T cipher_event,
  void *new_message,
  const gas_id_t gas_id
)
{
  rr_event_T  new_cipher_event    = EV_NO_EVENT;
  rr_event_T  proc_dispatch_event = EV_NO_EVENT;
  byte        message_id          = 0;
  byte        message_set         = 0;
  IMH_T      *message_header;

  // Obtain a pointer to the module data
  cipher_state_data_t *cipher_state_data_ptr = rr_cipher_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(cipher_state_data_ptr);

  if (cipher_event == EV_RESET_SOFTWARE)
  {
    cipher_state_data_ptr->cipher_state = CIPHER_INACTIVE;
    cipher_state_data_ptr->old_cipher_state = CIPHER_INACTIVE;
    proc_dispatch_event = EV_CIPHERING_TERMINATED;
    cipher_event = EV_NO_EVENT;
  }

  while (cipher_event != EV_NO_EVENT)
  {
    if (cipher_event == EV_INPUT_MESSAGE)
    {
      message_header = (IMH_T *) new_message;
      message_set    = message_header->message_set;
      message_id     = message_header->message_id;
    }

    switch (cipher_state_data_ptr->cipher_state)
    {
      case CIPHER_INACTIVE :
      {
        if (cipher_event == EV_INPUT_MESSAGE)
        {
          if ((message_set == MS_RR_L2) && (message_id == (int) DL_DATA_IND))
          {
            dl_data_ind_T *data_ind_ptr = (dl_data_ind_T *)new_message;

            if (data_ind_ptr->layer3_message[1] == CIPHERING_MODE_COMMAND)
            {
              /**********************************************/
              /* A CIPHERING MODE COMMAND has been received */
              /**********************************************/
              new_cipher_event = rr_respond_to_cipher_command(gas_id);
            }
          }
        }
        else if (cipher_event == EV_CIPHER_COMMAND_CHECKED)
        {
          proc_dispatch_event = EV_CIPHERING;
          cipher_state_data_ptr->cipher_state = CIPHER_MODE_CHANGE_REQUESTED;
        }
        break;
      }
      case CIPHER_MODE_CHANGE_REQUESTED :
      {
        if (cipher_event == EV_INPUT_MESSAGE)
        {
          if ((message_set == MS_RR_L1) && (message_id == (byte) MPH_SET_CIPHERING_CNF))
          {
            if (((mph_set_ciphering_cnf_T *)new_message)->error_code == NO_ERROR )
            {
              if (rr_complete_ciphering(gas_id) == TRUE)
              {
                /* advise NAS that the ciphering was completed */
                rr_confirm_ciphering_completed(gas_id);
              }
            }
            else
            {
              /*
               * l1 failed to engage ciphering, so dont send ciphering
               * complete to network
               */
              MSG_GERAN_ERROR_0_G("L1 failed to set ciphering");
            }
            proc_dispatch_event = EV_CIPHERING_TERMINATED;
            cipher_state_data_ptr->cipher_state = CIPHER_INACTIVE;
          }
        }
        break;
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((cipher_event != new_cipher_event) && (new_cipher_event != EV_NO_EVENT)) ||
        (cipher_state_data_ptr->old_cipher_state != cipher_state_data_ptr->cipher_state))
    {
       rr_store_trace_msg_buf(
         RR_CIPHER_CONTROL_SM,
         new_cipher_event,
         (byte) cipher_state_data_ptr->cipher_state,
         new_message,
         gas_id
       );
    }
#endif

    if (new_cipher_event != EV_NO_EVENT)
    {
      cipher_event = new_cipher_event;
      new_cipher_event = EV_NO_EVENT;
    }
    else
    {
      cipher_event = EV_NO_EVENT;
    }

    if (cipher_state_data_ptr->old_cipher_state != cipher_state_data_ptr->cipher_state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_cipher_control",
        cipher_state_data_ptr->old_cipher_state,
        cipher_state_data_ptr->cipher_state,
        rr_cipher_state_name,
        gas_id
      );

      cipher_state_data_ptr->old_cipher_state = cipher_state_data_ptr->cipher_state;
    }
  }

  return proc_dispatch_event;
}

/* EOF */

