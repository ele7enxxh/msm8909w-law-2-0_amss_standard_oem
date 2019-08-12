/*! \file rr_channel_mode.c

  Contains Channel Mode support data and handles the Channel Mode Modify RR Procedure

                Copyright (c) 2007-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_channel_mode.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "rr_seg_load.h"
#include "rr_channel_mode.h"
#include "rr_l1_send.h"
#include "rr_l2_send.h"
#include "rr_mm_send.h"
#include "rr_decode_ie.h"
#include "rr_init.h"
#include "cause.h"
#include "msg.h"
#include "rr_log.h"
#include "rr_general.h"
#include "rr_ded_config.h"
#include "geran_multi_sim.h"

#include "ghdi_exp.h"
#include "ghdi_exp_v.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#define CHANNEL_MODE_MODIFY_ACK_LEN 6

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef struct
{
  byte    ms_capability;
  boolean speech_v1_supported;
  boolean speech_v2_supported;
  boolean speech_v3_supported;
  boolean speech_v5_supported;
  boolean data_14400_supported;
  boolean data_9600_supported;
  boolean data_4800_supported;
  boolean data_2400_supported;
  boolean half_rate_data_supported;
} rr_channel_mode_support_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

static rr_channel_mode_support_t rr_channel_mode_support[MAX_AS_IDS];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Returns the data-space index appropriate for the given AS-ID.
 *
 *  Note: As the NV data is stored by AS-ID instead of GAS-ID, the Channel Mode support data is stored
 *  by AS-ID also.
 *
 * \param as_id
 *
 * \return uint8
 */
static uint8 rr_channel_mode_get_data_space_index(const sys_modem_as_id_e_type as_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
#ifdef FEATURE_TRIPLE_SIM
  if (SYS_MODEM_AS_ID_3 == as_id)
  {
    return(GERAN_DATA_SPACE_INDEX_3);
  }
  else
#endif /* FEATURE_TRIPLE_SIM */
  if (SYS_MODEM_AS_ID_2 == as_id)
  {
    return(GERAN_DATA_SPACE_INDEX_2);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(GERAN_DATA_SPACE_INDEX_1);
}

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param as_id
 *
 * \return rr_nv_data_t*
 */
static rr_channel_mode_support_t *rr_channel_mode_get_data_ptr(
  const sys_modem_as_id_e_type as_id
)
{
  uint8 as_index = rr_channel_mode_get_data_space_index(as_id);

  return &rr_channel_mode_support[as_index];
}

/*!
 * \brief Requests transmission of CMM ACK message via L2.
 * 
 * \param cmm_ptr 
 * \param gas_id 
 */
static void rr_send_channel_mode_modify_ack(
  const channel_mode_modify_T *cmm_ptr,
  const gas_id_t gas_id
)
{
  byte layer3_message[CHANNEL_MODE_MODIFY_ACK_LEN];
  byte index;

  /************************************************************/
  /* Encode the Protocol Discriminator and the Skip Indicator */
  /************************************************************/

  layer3_message[0] = RADIO_RESSOURCES;

  /***************************/
  /* Encode the message type */
  /***************************/

  layer3_message[1] = CHANNEL_MODE_MODIFY_ACKNOWLEDGE;

  /*************************************/
  /* Encode the channel_description IE */
  /*************************************/
  for (index = 0; index < RR_CHANNEL_DESC_IE_LENGTH; index++)
  {
    layer3_message[2+index] = cmm_ptr->channel_desc[index];
  }

  /******************************/
  /* Add the channel mode IE    */
  /******************************/
  layer3_message[2+RR_CHANNEL_DESC_IE_LENGTH] = (byte)cmm_ptr->channel_mode;

  MSG_GERAN_HIGH_0_G("OMsg(NW,CHANNEL_MODE_MODIFY_ACKNOWLEDGE)");
  (void) rr_send_dl_data_req(
    layer3_message,
    CHANNEL_MODE_MODIFY_ACK_LEN,
    SAPI0,
    DCCH,
    gas_id
  );

  return;
}

/*!
 * \brief Determines the Channel Mode from the supplied channel information.
 * 
 * \param dedic_ptr 
 * \param chan_info_ptr 
 * \param gas_id 
 * 
 * \return channel_mode_T 
 */
static channel_mode_T rr_get_mode_for_channel(
  rr_dedicated_data_T *dedic_ptr,
  const channel_information_T *chan_info_ptr,
  const gas_id_t gas_id
)
{
  channel_information_T *chan1_ptr;
  channel_spec_T        *chan_spec_ptr;
  channel_mode_T         current_mode = SIGNALLING_ONLY;

  /* establish pointers to RR's global channel configuration */
  chan_spec_ptr = rr_get_channel_spec(dedic_ptr, gas_id);

  if (chan_spec_ptr != NULL)
  {
    chan1_ptr = &(chan_spec_ptr->channel_info_1_after);

    if (chan_spec_ptr->number_of_ded_channels == 1)
    {
      if (chan1_ptr->channel_type == chan_info_ptr->channel_type)
      {
        current_mode = chan_spec_ptr->channel_mode_1;
      }
    }
    else
    {
      channel_information_T *chan2_ptr;

      chan2_ptr = &(chan_spec_ptr->channel_info_2_after);

      if ((chan1_ptr->channel_type == chan_info_ptr->channel_type) &&
          (chan1_ptr->subchannel   == chan_info_ptr->subchannel))
      {
        current_mode = chan_spec_ptr->channel_mode_1;
      }
      else if ((chan2_ptr->channel_type == chan_info_ptr->channel_type) &&
               (chan2_ptr->subchannel   == chan_info_ptr->subchannel))
      {
        current_mode = chan_spec_ptr->channel_mode_2;
      }
    }
  }

  return(current_mode);
}

/*!
 * \brief Decodes the Channel Mode Modify message.
 * 
 * \param dedic_ptr 
 * \param gas_id 
 * 
 * \return boolean - TRUE if the message was decoded successfully, FALSE if there was an error
 */
static boolean rr_decode_channel_mode_modify(
  rr_dedicated_data_T *dedic_ptr,
  const gas_id_t gas_id
)
{
  channel_mode_modify_T *cmm_ptr;
  ie_descriptor_T       *current_IE;
  boolean                decoding_ok = TRUE;
  byte                   index;

  if (dedic_ptr != NULL)
  {
    cmm_ptr = &(dedic_ptr->channel_mode_modify_details);
  }
  else
  {
    return(FALSE);
  }

  /* initialise the output structures */
  rr_invalidate_multirate_config(&(dedic_ptr->next_amr_config));

  /****************************************************/
  /* get a pointer to the first IE of the OTA message */
  /*****************************************************/
  current_IE = rr_get_first_ie_of_message(CHANNEL_MODE_MODIFY, gas_id);

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    decoding_ok = FALSE;
  }

  if (decoding_ok)
  {
    /*************************************************************/
    /* Extract and store the Channel Description, then decode it */
    /*************************************************************/
    for (index = 0; index < RR_CHANNEL_DESC_IE_LENGTH; index++)
    {
      cmm_ptr->channel_desc[index] = current_IE->start_IE[index];
    }

    rr_decode_channel_description_2(
      cmm_ptr->channel_desc,
      &(cmm_ptr->channel_info),
      &decoding_ok,
      gas_id
    );
  }

  /***************************/
  /* Decode the Channel Mode */
  /***************************/
  if (decoding_ok == TRUE)
  {
    boolean mode_ok;

    current_IE = current_IE->next_IE;
    rr_decode_channel_mode(current_IE->start_IE,
                           &(cmm_ptr->channel_mode),
                           &mode_ok);

    if (mode_ok == FALSE)
    {
      /* See 44.018 section 3.4.6.1.3
       * If the mobile station does not support the indicated mode,
       * it shall retain the old mode and return the associated
       * channel mode information in the CHANNEL MODE MODIFY ACKNOWLEDGE message
       */
      MSG_GERAN_HIGH_0_G("Channel mode not supported");
    }
    /* Note that decoding_ok remains TRUE, so that the CMM ACK is sent */
  }

  /******************************************/
  /* Decode the (optional) Multirate Config */
  /******************************************/
  if (decoding_ok == TRUE)
  {
    channel_spec_T *chan_spec_ptr;
    boolean         multirate_present = FALSE;

    chan_spec_ptr = rr_get_channel_spec(dedic_ptr, gas_id);

    /* Extract the multirate config IE if the new channel mode is AMR */
    if ((chan_spec_ptr != NULL) && IS_AMR_CHANNEL_MODE(cmm_ptr->channel_mode))
    {
      byte *start_IE;

      /* find the Multirate config IE */
      start_IE = rr_find_start_of_IE(current_IE, CMM_MULTIRATE_CONFIG_IEI);

      if (start_IE != NULL)
      {
        /* the Multirate Config IE is present, decode it */
        rr_decode_multirate_config_ie(
          start_IE,
          &(dedic_ptr->next_amr_config),
          &decoding_ok,
          cmm_ptr->channel_mode,
          gas_id
        );

        multirate_present = TRUE;
      }
      else if ((dedic_ptr->curr_amr_config.valid == TRUE) &&
               (IS_AMR_CHANNEL_MODE(chan_spec_ptr->channel_mode_1)))
      {


#ifdef FEATURE_VAMOS_II
        if(rr_vamos2_rcvd_chan_mode_matches_curr_chan_mode(cmm_ptr->channel_mode,
                                                           chan_spec_ptr->channel_mode_1) == TRUE) 
#else
        if (cmm_ptr->channel_mode == chan_spec_ptr->channel_mode_1)
#endif /*FEATURE_VAMOS_II*/

        {
          /* missing multirate config can be inherited from the RR global data */
          rr_use_existing_multirate_config(dedic_ptr);
          multirate_present = TRUE;
        }
        else
        {
          /* Channel mode has changed to AMR FR< ->AMR WB, but no multirate config provided */
          decoding_ok = FALSE;
          MSG_GERAN_HIGH_2_G("Current AMR type = %d, new AMR type = %d",
                   (int)chan_spec_ptr->channel_mode_1,
                   (int)cmm_ptr->channel_mode);
          MSG_GERAN_ERROR_0_G("No multirate config in CHANNEL MODE MODIFY");
        }
      }
      else
      {
        /* no valid multirate config from any previous assignment */
        MSG_GERAN_HIGH_0_G("No multirate config in CHANNEL MODE MODIFY");
        decoding_ok = FALSE;
      }
    }

    /* For half-rate the last 2 bits in the codec modes of the Active Codec Set
     * that correspond to the rates of 12.2 and 10.2 should not be set
     */
    if ((multirate_present                  == TRUE) &&
        (cmm_ptr->channel_info.channel_type == HALF_RATE_TRAFFIC))
    {
      if (dedic_ptr->next_amr_config.codec_modes & 0xC0)
      {
        MSG_GERAN_HIGH_0_G("Unsupported codec mode for the assigned channel");
        decoding_ok = FALSE;
      }
    }
#ifdef FEATURE_GSM_AMR_WB

#ifdef FEATURE_VAMOS_II
    if(cmm_ptr->channel_mode == SPEECH_V5 || cmm_ptr->channel_mode == SPEECH_V5_VAMOS2)
#else
    if(cmm_ptr->channel_mode == SPEECH_V5)
#endif /*FEATURE_VAMOS_II*/
    {
      /* For SPEECH_V5 channel mode, channel type should be full rate*/
      if (cmm_ptr->channel_info.channel_type == HALF_RATE_TRAFFIC)
      {
        MSG_GERAN_HIGH_0_G("Unsupported Channel type for AMR WB");
        decoding_ok = FALSE;
      }
    }
#endif /*FEATURE_GSM_AMR_WB*/
  }

  return(decoding_ok);
}

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

/*!
 * \brief Sets the channel modes supported.
 * 
 * \param voice_calls_supported 
 * \param csdata_calls_supported 
 * \param as_id 
 */
void rr_channel_mode_set_supported(
  boolean voice_calls_supported,
  boolean csdata_calls_supported,
  sys_modem_as_id_e_type as_id
)
{
  rr_channel_mode_support_t *rr_channel_mode_support_ptr;

  // Obtain a pointer to the module data
  rr_channel_mode_support_ptr = rr_channel_mode_get_data_ptr(as_id);
  RR_NULL_CHECK_FATAL(rr_channel_mode_support_ptr);

#ifndef FEATURE_DATA_GCSD
  // If FEATURE_DATA_GCSD is not defined, then CSDATA calls are not allowed, so overrule
  csdata_calls_supported = FALSE;
#endif // FEATURE_DATA_GCSD

  /* Set supported GSM circuit-switched voice codecs */
  rr_channel_mode_support_ptr->speech_v1_supported = voice_calls_supported;
  rr_channel_mode_support_ptr->speech_v2_supported = voice_calls_supported;
#ifdef FEATURE_GSM_AMR
  rr_channel_mode_support_ptr->speech_v3_supported = voice_calls_supported;
#else
  rr_channel_mode_support_ptr->speech_v3_supported = FALSE;
#endif /* FEATURE_GSM_AMR */
#ifdef FEATURE_GSM_AMR_WB
  rr_channel_mode_support_ptr->speech_v5_supported = voice_calls_supported;
#else
  rr_channel_mode_support_ptr->speech_v5_supported = FALSE;
#endif /* FEATURE_GSM_AMR_WB */

  /* Set supported GSM circuit-switched data rates */
  rr_channel_mode_support_ptr->data_14400_supported     = csdata_calls_supported;
  rr_channel_mode_support_ptr->data_9600_supported      = csdata_calls_supported;
  rr_channel_mode_support_ptr->data_4800_supported      = FALSE; /* never supported */
  rr_channel_mode_support_ptr->data_2400_supported      = FALSE; /* never supported */
  rr_channel_mode_support_ptr->half_rate_data_supported = FALSE; /* never supported */

  if (voice_calls_supported)
  {
#ifdef FEATURE_GOBI
    // GOBI products advertise FULL RATE only
    rr_channel_mode_support_ptr->ms_capability = FULL_RATE_ONLY;
#else
    // Advertise FULL RATE and HALF RATE
    rr_channel_mode_support_ptr->ms_capability = DUAL_RATE_SUPPORTED;
#endif // FEATURE_GOBI
  }
  else
  {
    // If no voice support, advertise SDCCH only
    rr_channel_mode_support_ptr->ms_capability = SDCCH_ONLY;
  }

  // Note that the combination of voice not supported + CSD supported is not valid.
  // It is the voice support that is the primary driver for MS capability advertisment

  return;
}

/*!
 * \brief Gets the current MS capability and half-rate support.
 * 
 * \param ms_caps_ptr (out) - updated with MS capabilities
 * \param hr_data_ptr (out) - updated with HF data support
 * \param gas_id (in)
 */
void rr_get_ms_capabilities(
  byte *ms_caps_ptr,
  boolean *hr_data_ptr,
  const gas_id_t gas_id
)
{
  rr_channel_mode_support_t *rr_channel_mode_support_ptr;

  // Obtain a pointer to the module data
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;
  rr_channel_mode_support_ptr = rr_channel_mode_get_data_ptr(as_id);
  RR_NULL_CHECK_FATAL(rr_channel_mode_support_ptr);

  if (ms_caps_ptr != NULL)
  {
    *ms_caps_ptr = rr_channel_mode_support_ptr->ms_capability;
  }

  if (hr_data_ptr != NULL)
  {
    *hr_data_ptr = rr_channel_mode_support_ptr->half_rate_data_supported;
  }

  return;
}

/*!
 * \brief Determines whether the specified channel mode is supported.
 * 
 * \param channel_mode (in)
 * \param gas_id (in)
 * 
 * \return boolean - TRUE is supported, FALSE if not supported
 */
boolean rr_channel_mode_supported(
  channel_mode_T channel_mode,
  const gas_id_t gas_id
)
{
  boolean is_supported;
  rr_channel_mode_support_t *rr_channel_mode_support_ptr;

  // Obtain a pointer to the module data
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;
  rr_channel_mode_support_ptr = rr_channel_mode_get_data_ptr(as_id);
  RR_NULL_CHECK_FATAL(rr_channel_mode_support_ptr);

  switch (channel_mode)
  {
    case SIGNALLING_ONLY:
      is_supported = TRUE; /* signalling only is always supported */
      break;

    case SPEECH_V1:
#ifdef FEATURE_VAMOS_II
    case SPEECH_V1_VAMOS2:
#endif /*FEATURE_VAMOS_II*/
      is_supported = rr_channel_mode_support_ptr->speech_v1_supported;
      break;

    case SPEECH_V2:
#ifdef FEATURE_VAMOS_II
    case SPEECH_V2_VAMOS2:
#endif /*FEATURE_VAMOS_II*/
      is_supported = rr_channel_mode_support_ptr->speech_v2_supported;
      break;

    case SPEECH_V3:
#ifdef FEATURE_VAMOS_II
    case SPEECH_V3_VAMOS2:
#endif /*FEATURE_VAMOS_II*/
      is_supported = rr_channel_mode_support_ptr->speech_v3_supported;
      break;

    case SPEECH_V5:
#ifdef FEATURE_VAMOS_II
    case SPEECH_V5_VAMOS2:
#endif /*FEATURE_VAMOS_II*/
      is_supported = rr_channel_mode_support_ptr->speech_v5_supported;
      break;

    case DATA_14400:
      is_supported = rr_channel_mode_support_ptr->data_14400_supported;
      break;

    case DATA_9600:
      is_supported = rr_channel_mode_support_ptr->data_9600_supported;
      break;

    case DATA_4800:
      is_supported = rr_channel_mode_support_ptr->data_4800_supported;
      break;

    case DATA_2400:
      is_supported = rr_channel_mode_support_ptr->data_2400_supported;
      break;

    default:
      MSG_GERAN_ERROR_1("Unknown codec/channel mode 0x%02X", (int)channel_mode);
      is_supported = FALSE;
      break;
  }

  return(is_supported);
} /* rr_channel_mode_supported */

/*!
 * \brief Process the Channel Mode Modify message received OTA.
 * 
 * \param gas_id 
 */
void rr_handle_channel_mode_modify_msg(
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T   *dedic_ptr;
  channel_mode_modify_T *cmm_ptr;

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("No dedicated mode data is available for CMM");
    return;
  }
  else
  {
    cmm_ptr = &(dedic_ptr->channel_mode_modify_details);
  }

  /* attempt to decode the Channel Mode Modify message */
  if (rr_decode_channel_mode_modify(dedic_ptr, gas_id) == TRUE)
  {
    channel_spec_T *chan_spec_ptr;
    rr_event_T      mode_modify_event;

    chan_spec_ptr = rr_get_channel_spec(dedic_ptr, gas_id);

    /*****************************/
    /* Validate the channel type */
    /*****************************/
    if ((cmm_ptr->channel_info.channel_type != FULL_RATE_TRAFFIC) &&
        (cmm_ptr->channel_info.channel_type != HALF_RATE_TRAFFIC))
    {
      /* determine the existing mode of channel 1 */
      cmm_ptr->channel_mode = chan_spec_ptr->channel_mode_1;

      /* this is an error condition, no mode change, but a CMM ACK will be sent */
      mode_modify_event = EV_NO_EVENT;
    }
    /*****************************/
    /* Validate the channel mode */
    /*****************************/
    else if (rr_channel_mode_supported(cmm_ptr->channel_mode, gas_id) == FALSE)
    {
      /* determine channel mode for the channel identified by the supplied type */
      cmm_ptr->channel_mode = rr_get_mode_for_channel(
        dedic_ptr,
        &(cmm_ptr->channel_info),
        gas_id
      );

      /* this is an error condition, no mode change, but a CMM ACK will be sent */
      mode_modify_event = EV_NO_EVENT;
    }
    else
    {
      /* validation successful */
      mode_modify_event = EV_MODE_MODIFY_CHECKED;
    }

    /* request L2 to transmit the appropriate CMM ACK immediately */
    /* note that this happens even if the validation fails above, */
    /* and that the channel mode IE indicates the mode in use NOW */
    rr_send_channel_mode_modify_ack(cmm_ptr, gas_id);

    /* continue if the validation has succeeded */
    if (mode_modify_event == EV_MODE_MODIFY_CHECKED)
    {
      /* L1 stops using DTX on receipt of CHANNEL MODE MODIFY command, */
      /* keep the DTX indication FALSE until the procedure is complete */
      rr_dtx_rr_override_on(gas_id);

      /* send request L1 to modify the channel */
      rr_send_mph_channel_mode_modify_req(
        cmm_ptr->channel_mode,
        cmm_ptr->channel_info.channel_type,
        cmm_ptr->channel_info.subchannel,
        &(dedic_ptr->next_amr_config),
        gas_id
      );

      rr_eng_mode_update_codec_ho_info(
        dedic_ptr->curr_channel_spec.channel_mode_1,
        cmm_ptr->channel_mode,
        cmm_ptr->channel_info.channel_type,
        FALSE,
        SYS_HO_START,
        SYS_HO_TYPE_GSM_GSM, 
        gas_id
      );

      rr_eng_mode_update_codec_ho_info(
        dedic_ptr->curr_channel_spec.channel_mode_1,
        cmm_ptr->channel_mode,
        cmm_ptr->channel_info.channel_type,
        FALSE,
        SYS_SPEECH_CODEC_INFO,
        SYS_HO_TYPE_GSM_GSM, 
        gas_id
      );
    }
  }
  else /* decoding failure */
  {
    /********************************************************/
    /* The MS ignores the message, and, as an RR connection */
    /* exists, sends an RR_STATUS with cause #96, "invalid  */
    /* mandatory information" (via DL_DATA_REQ to Layer 2)  */
    /********************************************************/
    MSG_GERAN_HIGH_0_G("OMsg(NW,STATUS(MAND_INF)");
    rr_send_status(RR_INVALID_MANDATORY_INFORMATION, gas_id);
  }

  return;
} /* rr_respond_to_channel_mode_modify */

/*!
 * \brief Processes the MPH_CHANNEL_MODE_MODIFY_CNF message from L1.
 * 
 * \param cmd_ptr 
 * \param gas_id 
 */
void rr_handle_channel_mode_modify_cnf(
  void *cmd_ptr,
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T   *dedic_ptr;
  channel_information_T *chan1_ptr;
  channel_spec_T        *chan_spec_ptr;
  channel_mode_modify_T *cmm_ptr;
  byte                   error_code;
  mph_channel_mode_modify_cnf_T *cmm_cnf_ptr;
  /* obtain pointer to global dedicated storage */
  dedic_ptr     = rr_get_dedicated_data_ptr(gas_id);
  chan_spec_ptr = rr_get_channel_spec(dedic_ptr, gas_id);
  chan1_ptr     = &(chan_spec_ptr->channel_info_1_after);

  /* determine whether the mode change completed in L1 successfully */
  
  RR_ASSERT_RETURN_VOID(cmd_ptr != NULL);  //Assert if the pointer is NULL
  RR_ASSERT_RETURN_VOID(dedic_ptr != NULL);

  cmm_cnf_ptr = (mph_channel_mode_modify_cnf_T *)cmd_ptr;
  error_code  = cmm_cnf_ptr->error_code;
  cmm_ptr     = &(dedic_ptr->channel_mode_modify_details);
  RR_ASSERT_RETURN_VOID(cmm_ptr != NULL);
  

  /* Revert the DTX indication to match that broadcast by the NW */
  /* This will take effect on the next reception of SI5 or SI6   */
  rr_dtx_rr_override_off(gas_id);

  if (error_code != CHANNEL_MODE_CHANGED)
  {
    MSG_GERAN_HIGH_1_G("Unable to complete Channel Mode Modify (error=%d)", (int)error_code);

    rr_eng_mode_update_codec_ho_info(
      dedic_ptr->curr_channel_spec.channel_mode_1,
      cmm_ptr->channel_mode,
      cmm_ptr->channel_info.channel_type,
      FALSE,
      SYS_HO_FAIL,
      SYS_HO_TYPE_GSM_GSM, 
      gas_id
    );
  }
  else
  {
#ifdef FEATURE_GSM_BAND_AVOIDANCE
    /*update band avoid with freq list when changing to a voice mode.  Freq list is not updated 
      by channel mode modify so updates are not required when not going in to a voice mode*/
    if (rr_channel_mode_is_voice(cmm_ptr->channel_mode) &&
        !rr_channel_mode_is_voice(dedic_ptr->curr_channel_spec.channel_mode_1))
    {
      rr_band_avoid_ded_freq_upd_send_imsg(&dedic_ptr->curr_channel_spec.channel_info_1_after.frequency_list, gas_id);
    }
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

#ifdef FEATURE_LTE
    /* If the call was originally established for LAU but the connection
     * is being retained for a voice call then we also need to trigger G2L
     * blind redirection on connection release. */
    if (dedic_ptr->curr_channel_spec.channel_mode_1 == SIGNALLING_ONLY &&
        rr_channel_mode_is_voice(cmm_ptr->channel_mode))
    {
      if (rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(gas_id))
      {
        MSG_GERAN_HIGH_0_G("Channel mode changed from Signaling to Voice, enabling G2L blind redirection");
        rr_connection_release_set_g2l_blind_redir_flag(gas_id);
      }
    }
#endif /* FEATURE_LTE */

    rr_eng_mode_update_codec_ho_info(
      dedic_ptr->curr_channel_spec.channel_mode_1,
      cmm_ptr->channel_mode,
      cmm_ptr->channel_info.channel_type,
      FALSE,
      SYS_HO_COMPLETE,
      SYS_HO_TYPE_GSM_GSM,
      gas_id
    );

    /********************************************/
    /* Update RR's global channel configuration */
    /********************************************/
    if (chan_spec_ptr->number_of_ded_channels == 1)
    {
      if (chan1_ptr->channel_type == cmm_ptr->channel_info.channel_type)
      {
        chan_spec_ptr->channel_mode_1 = cmm_ptr->channel_mode;
      }
    }
    else
    {
      channel_information_T *chan2_ptr;

      chan2_ptr = &(chan_spec_ptr->channel_info_2_after);

      if ((chan1_ptr->channel_type == cmm_ptr->channel_info.channel_type) &&
          (chan1_ptr->subchannel   == cmm_ptr->channel_info.subchannel))
      {
        chan_spec_ptr->channel_mode_1 = cmm_ptr->channel_mode;
      }
      else if ((chan2_ptr->channel_type == cmm_ptr->channel_info.channel_type) &&
               (chan2_ptr->subchannel   == cmm_ptr->channel_info.subchannel))
      {
        chan_spec_ptr->channel_mode_2 = cmm_ptr->channel_mode;
      }
    }

    /* Update the global multirate config with the pending details */
    rr_switch_to_next_multirate_config(dedic_ptr);

    /* output logging info */
    MSG_GERAN_HIGH_3_G("Channel Mode Modify successful (%d,%d,%d)",
              (int)cmm_ptr->channel_info.channel_type,
              (int)cmm_ptr->channel_info.subchannel,
              (int)cmm_ptr->channel_mode);

    (void)rr_log_channel_configuration(gas_id);

    /* advise NAS of the change in dedicated channels */
    rr_send_sync_ind(
      MM_RR_CHANNEL_MODE_MODIFIED,
      cmm_ptr->channel_info.channel_type,
      cmm_ptr->channel_mode,
      cmm_ptr->channel_info.subchannel,
      gas_id
    );
  }

  return;
}

/*!
 * \brief Determines if the supplied mode is for voice.
 * 
 * \param curr_chan_mode 
 * 
 * \return boolean - TRUE if voice mode, FALSE if not voice mode
 */
boolean rr_channel_mode_is_voice(
  channel_mode_T curr_chan_mode
)
{
  boolean voice_mode = FALSE;

  switch (curr_chan_mode)
  {
    case SPEECH_V1:
    case SPEECH_V2:
#ifdef FEATURE_GSM_AMR
    case SPEECH_V3:
#endif /*FEATURE_GSM_AMR*/

#ifdef FEATURE_GSM_AMR_WB
    case SPEECH_V5:
#endif /*FEATURE_GSM_AMR_WB*/

#ifdef FEATURE_VAMOS_II
    case SPEECH_V1_VAMOS2:
    case SPEECH_V2_VAMOS2: 
#ifdef FEATURE_GSM_AMR
    case SPEECH_V3_VAMOS2:
#endif /*FEATURE_GSM_AMR*/

#ifdef FEATURE_GSM_AMR_WB
    case SPEECH_V5_VAMOS2:
#endif /*FEATURE_GSM_AMR_WB  */

#endif /*FEATURE_VAMOS_II*/

      voice_mode = TRUE;
    break;

    default:
      voice_mode = FALSE;
    break;
  }

  return voice_mode;
}

/*==========================================================================

  FUNCTION    rr_is_voice_call_active

  DESCRIPTION

    Checks if GERAN has an active voice call

  PARAMETERS

    none

  DEPENDENCIES
    Calls rr_in_dedicated(), rr_get_dedicated_data_ptr()

  RETURN VALUE
    boolean - TRUE - voice call active, FALSE - voice call not active

  SIDE EFFECTS
    None.

===========================================================================*/
boolean rr_is_voice_call_active(const gas_id_t gas_id)
{
  boolean in_a_voice_call = FALSE;
  rr_dedicated_data_T *dedic_ptr = rr_get_dedicated_data_ptr(gas_id);

  if (rr_in_dedicated(gas_id) == TRUE)
  {
    if (rr_channel_mode_is_voice(dedic_ptr->curr_channel_spec.channel_mode_1) == TRUE)
    {
      in_a_voice_call = TRUE;
    }
  }

  return in_a_voice_call;
}

/*==========================================================================

  FUNCTION    rr_set_traffic_reason_from_channel_mode

  DESCRIPTION

    Updates the GCOMMON traffic_reason based on the specified channel mode

  PARAMETERS

    channel mode

  DEPENDENCIES
    None

  RETURN VALUE
    VOID

  SIDE EFFECTS
    None

===========================================================================*/
void rr_set_traffic_reason_from_channel_mode(const channel_mode_T channel_mode, const gas_id_t gas_id)
{
  switch(channel_mode)
  {
    case SIGNALLING_ONLY:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_SIGNALLING, gas_id);
      break;

    case SPEECH_V1:
    case SPEECH_V2:
    case SPEECH_V3:
    case SPEECH_V5:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_VOICE, gas_id);
      break;

    case DATA_14400:
    case DATA_9600:
    case DATA_4800:
    case DATA_2400:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_DATA, gas_id);
      break;

    default:
      geran_set_traffic_reason(GERAN_TRAFFIC_DEFAULT, gas_id);
      break;
  }

  return;
}

/*==========================================================================

  FUNCTION    rr_eng_mode_get_speech_sample_frequency

  DESCRIPTION

    From the speech codec mode work out the sample frequency.

  PARAMETERS

    speech_codec - codec mode

  DEPENDENCIES
    Calls ghdi_get_sampling_rate()

  RETURN VALUE
    uint32 - speech encoded sample frequency

  SIDE EFFECTS
    None.

===========================================================================*/
uint32 rr_eng_mode_get_speech_sample_frequency(sys_speech_codec_e_type speech_codec)
{
  uint32 speech_enc_samp_freq = 0; /*unknown value in eng mode*/

  /* Set Speech Encoded Sample Frequency */
  switch (speech_codec)
  {
#ifdef FEATURE_GSM_AMR_WB
    case SYS_SPEECH_CODEC_AMR_WB:
      speech_enc_samp_freq = 16000; /*WB*/
    break;
#endif /*FEATURE_GSM_AMR_WB*/

#ifdef FEATURE_GSM_AMR
    case SYS_SPEECH_CODEC_AMR_NB:
#ifdef FEATURE_ENHANCED_AMR
      speech_enc_samp_freq = ghdi_get_sampling_rate();
#else
      speech_enc_samp_freq = 8000; /*NB*/
#endif /*FEATURE_ENHANCED_AMR*/
    break;
#endif /*FEATURE_GSM_AMR*/
    case SYS_SPEECH_CODEC_GSM_EFR:
    case SYS_SPEECH_CODEC_GSM_FR:
    case SYS_SPEECH_CODEC_GSM_HR:
      speech_enc_samp_freq = 8000; /*All other GSM codecs are NB*/
      break;
    
    default:
      MSG_ERROR("Speech_enc_samp_freq not set, invalid codec:%d",speech_codec,0,0);
    break;
  }

  MSG_HIGH("Speech_enc_samp_freq set to:%d",speech_enc_samp_freq,0,0);
  
  return speech_enc_samp_freq;
}


/*!
 * \brief Retrieves and formats the data for ENG_MODE_CODEC_HO_INFO, and updates the Eng Mode data.
 * 
 * \param curr_chan_mode 
 * \param new_chan_mode 
 * \param new_chan_type 
 * \param irat_ho 
 * \param cmd 
 * \param gas_id 
 */
void rr_eng_mode_update_codec_ho_info(
  channel_mode_T curr_chan_mode,
  channel_mode_T new_chan_mode,
  channel_type_T new_chan_type,
  boolean        irat_ho,
  sys_codec_ho_cmd cmd,
  sys_ho_type_e_type ho_type, 
  const gas_id_t gas_id
)
{
  boolean update_eng_mode = TRUE;

  sys_codec_ho_info ho_codec_info = {0};

#ifdef FEATURE_VAMOS_II
  curr_chan_mode =  rr_remove_vamos2_indication_from_channel_mode(curr_chan_mode);
  new_chan_mode = rr_remove_vamos2_indication_from_channel_mode(new_chan_mode);
#endif /*FEATURE_VAMOS_II*/

  MSG_GERAN_HIGH_3_G("RR NBWB curr_chan_mode:%d, new_chan_mode:%d, new_chan_type:%d",curr_chan_mode,new_chan_mode,new_chan_type);
  MSG_GERAN_HIGH_2_G("RR NBWB irat_ho:%d, cmd:%d",irat_ho,cmd);

  ho_codec_info.cmd = cmd;
  ho_codec_info.ho_type = ho_type; 

  if (rr_channel_mode_is_voice(new_chan_mode) == FALSE)
  {
    /* new mode is not a voice codec do not send an update*/
    return;
  }

  if (cmd == SYS_SPEECH_CODEC_INFO)
  {
    /* G<->G HO, only send codec update when voice codec has changed,
       W<->G HO always update codec because current codec is unknown */
    if ( (irat_ho == FALSE) &&
         (new_chan_mode == curr_chan_mode) )
    {
      /*new and current mode are the same no need to send an update*/
      return;
    }

    switch (new_chan_mode)
    {
      case SPEECH_V1:
        if (new_chan_type == FULL_RATE_TRAFFIC)
        {
          ho_codec_info.speech_codec = SYS_SPEECH_CODEC_GSM_FR;
        }
        else if (new_chan_type == HALF_RATE_TRAFFIC)
        {
          ho_codec_info.speech_codec = SYS_SPEECH_CODEC_GSM_HR;
        }
        else
        {
          MSG_GERAN_ERROR_1_G("RR SPEECH_V1 unexpected channel type:%d",new_chan_type);
          ho_codec_info.speech_codec = SYS_SPEECH_CODEC_GSM_HR;
        }
      break;

      case SPEECH_V2:
        ho_codec_info.speech_codec = SYS_SPEECH_CODEC_GSM_EFR;
      break;

#ifdef FEATURE_GSM_AMR
      case SPEECH_V3:
        ho_codec_info.speech_codec = SYS_SPEECH_CODEC_AMR_NB;
      break;
#endif /*FEATURE_GSM_AMR*/

#ifdef FEATURE_GSM_AMR_WB
      case SPEECH_V5:
        ho_codec_info.speech_codec = SYS_SPEECH_CODEC_AMR_WB;
      break;
#endif /*FEATURE_GSM_AMR_WB*/

      default:
        /* Not a speech codec, do not update eng mode*/
        MSG_GERAN_ERROR_1_G("RR unexpected chan mode (not a voice mode):%d",new_chan_mode);
        update_eng_mode = FALSE;
        break;
    }
 
    ho_codec_info.speech_enc_samp_freq = rr_eng_mode_get_speech_sample_frequency(ho_codec_info.speech_codec);
  }
  else /*HO cmd*/
  {
    /* if a voice call is already active or doing an IRAT HO send the HO cmds*/
    if ( (rr_channel_mode_is_voice(curr_chan_mode) == TRUE) ||
         (irat_ho == TRUE) )
    {
      ho_codec_info.speech_codec = SYS_SPEECH_CODEC_NONE;
      ho_codec_info.speech_enc_samp_freq = 0;
    }
    else
    {
      update_eng_mode = FALSE;
    }
  }
 
#ifdef FEATURE_ENHANCED_AMR
  /* if the HO complete successfuly then reset the sampling rate if
     the new codec is non AMR-NB*/
  if (update_eng_mode)
  {
    if (cmd == SYS_HO_COMPLETE)
    {
      switch (new_chan_mode)
      {
        case SPEECH_V1:
        case SPEECH_V2:
#ifdef FEATURE_GSM_AMR_WB
        case SPEECH_V5:
#endif /*FEATURE_GSM_AMR_WB*/
          MSG_HIGH("RR reset the sampling rate, new_chan_mode:%d",new_chan_mode,0,0);
          ghdi_reset_sampling_rate();
        break;

        default:
          /*do nothing*/
        break;
      }      
    }
  }
#endif /*FEATURE_ENHANCED_AMR*/

  if (update_eng_mode)
  {
    geran_eng_mode_data_write(
      ENG_MODE_CODEC_HO_INFO,   // geran_eng_mode_info_type
      &ho_codec_info,           // param_copy
      TRUE,                     // valid_flag
      gas_id                    // gas_id
    );
  }
}

/*!
 * \brief Module initialisation function. This is called once at RR task start. 
 * N.B. All data-spaces are initialised.
 */
void rr_channel_mode_task_start_init(
  void
)
{
  // Zero the control/data structure(s)
  memset(rr_channel_mode_support, 0, sizeof(rr_channel_mode_support));

  // Setup any specific values not setup by the general module initialisation function
  {
    int i;

    for (i = 0; i < MAX_AS_IDS; i++)
    {
      rr_channel_mode_set_supported(
        TRUE,                         // voice_calls_supported
        TRUE,                         // csdata_calls_supported
        (sys_modem_as_id_e_type) i    // as_id
      );
    }
  }
}

/* EOF */

