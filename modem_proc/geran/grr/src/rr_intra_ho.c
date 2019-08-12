/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Intra Cell Handover

GENERAL DESCRIPTION
  This module handles the intra cell handover processing

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_intra_ho.c_v   1.7   30 May 2002 14:34:42   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_intra_ho.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/22/09    sjw     Added support for FEATURE_UMTS_VOICE_CIPHERING_IND
05/06/09    sjw     Moved handover_details and assignment_details out of rr_data.c
03/09/09    sjw     Relocated rr_channel_mode_supported() to rr_channel_mode.c
02/13/09    sjw     Reworked local storage for ciphering module data
07/11/08    sjw     Allow SAPI3 control to decide whether release is required
05/07/04    gc      Enhanced Trace Buffer Code
05/08/03    he      Used separate debug_str buffer for logging.
02/27/03    RG      Release SAPI3 link when present during an intra cell handover.
11/07/02    JAC     Added support for multirate config in assignment command
09/03/02    rmc     Need to handle MDL_ERROR_IND as well as DL_RELEASE_IND
08/26/02    rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
08/19/02    rmc     Cleanup to fix CRs associated with Channel Assignments
05/30/02    rmc     Compilation warnings for MULTIBAND_SUPPORT flag
05/22/02    rmc     Changes to MM-RR ICD for dual mode
02/06/02    rmc     QXDM Cleanup
01/30/02    RMC     Added support to log channel configuration when it changes
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "rr_seg_load.h"
#include "rr_intra_ho.h"
#include "ms.h"
#include "rr_general.h"
#include "rr_gprs_debug.h"
#include "rr_l1_send.h"
#include "rr_l2_send.h"
#include "rr_mm_send.h"
#include "rr_decode_ie.h"
#include "rr_init.h"
#include "cause.h"
#include "aconfig.h"
#include <string.h>
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "rr_log.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "rr_cipher.h"
#include "rr_sapi3.h"
#include "rr_channel_mode.h"
#include "rr_sys_info.h"
#include "rr_ded_meas_reporting.h"
#include "rr_ded_config.h"
#include "rr_multi_sim.h"

typedef enum
{
  ASSIGNMENT_INACTIVE,
  AWAITING_LOCAL_RELEASE,
  AWAITING_ASSIGNMENT_CONFIRMATION,
  WAITING_FOR_CONFIRMATION_OF_RESUME,
  AWAITING_PHYSICAL_RECONNECTION,
  AWAITING_RECONNECTION
} assignment_state_T;

typedef struct
{
  assignment_state_T assignment_state;
  assignment_state_T old_assignment_state;
  rr_internal_band_T old_band;
} rr_intra_ho_data_t;

#define RR_INTRA_HO_DATA_INIT { \
                                ASSIGNMENT_INACTIVE, /*assignment_state*/ \
                                ASSIGNMENT_INACTIVE, /*old_assignment_state*/ \
                                BAND_NONE            /*old_band*/ \
                              }

static rr_intra_ho_data_t rr_intra_ho_data[NUM_GERAN_DATA_SPACES] = {
                                                                    RR_INTRA_HO_DATA_INIT
                                                                    #if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                                                                    ,RR_INTRA_HO_DATA_INIT
                                                                    #if defined(FEATURE_TRIPLE_SIM)
                                                                    ,RR_INTRA_HO_DATA_INIT
                                                                    #endif /* FEATURE_TRIPLE_SIM */
                                                                    #endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
                                                                    };

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_intra_ho_data_t*
 */
static rr_intra_ho_data_t *rr_intra_ho_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_intra_ho_data[as_index];
}

/**
 * Returns the name of the assignment state
 *
 * @param assignment_state assignment state
 *
 * @return text associated with the given state
 */
static char * rr_assignment_state_name(assignment_state_T local_state)
{
  char * s = "";

  switch(local_state)
  {
    case ASSIGNMENT_INACTIVE:
      s = "Inactive";
      break;

    case AWAITING_LOCAL_RELEASE:
      s = "AwaitLocalRel";
      break;

    case AWAITING_ASSIGNMENT_CONFIRMATION:
      s = "AwaitAssignConf";
      break;

    case WAITING_FOR_CONFIRMATION_OF_RESUME:
      s = "WaitForResumeConf";
      break;

    case AWAITING_PHYSICAL_RECONNECTION:
      s = "AwaitPhysRecon";
      break;

    case AWAITING_RECONNECTION:
      s = "AwaitRecon";
      break;

    default:
      s = "Unknown";
      break;
  }
  return s;
} /* rr_assignment_state_name */

/**
 * Send an assignment failure message to the network
 *
 * @param RR_cause  cause value
 *
 * @param reconnect indicates if RR should attempt to reconnect to the
 *                  old channel or not
 */
static void rr_perform_assignment_failure(
  byte RR_cause,
  boolean reconnect,
  const gas_id_t gas_id
)
{
  byte layer3_message[3];

  /*******************************************************/
  /* Generate an ASSIGNMENT_FAILURE message with cause   */
  /* as specified                                        */
  /*******************************************************/

  /*************************************/
  /* Fill in the PD and Skip Indicator */
  /*************************************/

  layer3_message[0] = RADIO_RESSOURCES;

  /****************************/
  /* Fill in the message type */
  /****************************/

  layer3_message[1] = ASSIGNMENT_FAILURE;

  /************************/
  /* Fill in the RR_cause */
  /************************/

  layer3_message[2] = RR_cause;

  MSG_GERAN_HIGH_1_G("OMsg(NW,ASSIGNMENT_FAILURE(%d))",(int) RR_cause);

  if (reconnect)
  {
    rr_send_dl_reconnect_req(
      layer3_message,   // layer3_message
      3,                // message_length
      gas_id            // gas_id
    );
  }
  else
  {
    (void) rr_send_dl_data_req(
      layer3_message,   // layer3_message
      3,                // length
      SAPI0,            // SAPI
      DCCH,             // l2_channel_type
      gas_id            // gas_id
    );
  }

} /* rr_perform_assignment_failure */

/**
 * Send the assignment complete message to the network
 * while reestablishing the data link layer
 */
static void rr_complete_channel_assignment(
  rr_dedicated_data_T *dedic_ptr,
  const gas_id_t gas_id
)
{
  byte layer3_message[3];
  byte message_length = 3;

  /***************************************************************************/
  /* The purpose of this module is to create the ASSIGNMENT COMPLETE message */
  /* and to send it to the network via the primitive DL_RESUME_REQ           */
  /***************************************************************************/
  if (dedic_ptr != NULL)
  {
    dedic_ptr->curr_channel_spec = dedic_ptr->assignment_details;
    rr_set_link_status(SAPI0_LINK_IN_PROGRESS, gas_id);
    rr_switch_to_next_multirate_config(dedic_ptr);

    /* log the change to the channel configuration */
    (void)rr_log_channel_configuration(gas_id);
  }

  /************************************************************/
  /* Encode the Protocol Discriminator and the Skip Indicator */
  /************************************************************/
  layer3_message[0] = RADIO_RESSOURCES;

  /***************************/
  /* Encode the message type */
  /***************************/
  layer3_message[1] = ASSIGNMENT_COMPLETE;

  /***************************************/
  /* Encode the RR cause as Normal Event */
  /***************************************/
  layer3_message[2] = RR_NORMAL_EVENT;

  MSG_GERAN_HIGH_0_G("OMsg(NW,ASSIGNMENT_COMPLETE)");
  rr_send_dl_resume_req(
    RR_L1_GSM_GSM,     // resume_reason
    layer3_message,    // layer3_message
    message_length,    // message_length
    gas_id             // gas_id
  );

  return;
} /* rr_complete_channel_assignment */

/**
 * Triggers the sending of the assignment failure
 * response to the network
 *
 * @param reconnect indicates if RR should reconnect or not with
 *                  the old channel
 */
static void rr_trigger_assignment_failure_response(const gas_id_t gas_id)
{
  /* Restore previously-saved channel spec and AMR config */
  rr_restore_previous_dedicated_config(gas_id);

  /*
   * generate an ASSIGNMENT_FAIL message, with cause
   * PROTOCOL_ERROR_UNSPECIFIED
   * and send to l2 vi DL_RECONNECT_REQ
   */
  rr_perform_assignment_failure(
    RR_PROTOCOL_ERROR_UNSPECIFIED,    // RR_cause
    TRUE,                             // reconnect
    gas_id                            // gas_id
  );

  /* log the change to the channel configuration */
  (void)rr_log_channel_configuration(gas_id);
} /* rr_trigger_assignment_failure_response */

/**
 * Handle an assignment command message received
 * from the network
 *
 * @return Event to be processed by calling state machine
 */
static rr_event_T rr_respond_to_assignment_command(
  rr_dedicated_data_T *dedic_ptr,
  const gas_id_t gas_id
)
{
  assignment_details_T      *assignment_details_ptr;
  rr_event_T                 new_event = EV_NO_EVENT;
  byte                       index;
  byte                       ca_index;
  ie_descriptor_T            *current_IE;
  boolean                    decoding_ok;
  byte                       power_level;
  frequency_list_T           frequency_list_after = {0};
  frequency_list_T           frequency_list_before = {0};
  mobile_allocation_T        ma_before;
  mobile_allocation_T        ma_after;
  boolean                    assignment_failure = FALSE;
  boolean                    start_ciphering    = FALSE;
  byte                       RR_cause = 0xff;  /* unset value */
  boolean                    first_chan_desc_before_received;
  boolean                    second_chan_desc_before_received;
  cipher_algorithm_T         cipher_algorithm;
  boolean                    frequency_list_after_time = FALSE;
  boolean                    ma_after_time = FALSE;
  boolean                    frequency_list_before_time = FALSE;
  boolean                    ma_before_time = FALSE;
  boolean                    freq_chan_seq_before_time = FALSE;
  byte                       no_of_entries;
  byte                       rr_status = RR_PROTOCOL_ERROR_UNSPECIFIED;
  boolean                    multirate_ie_present = FALSE;
  gprs_scell_info_t         *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(dedic_ptr);

  /* Initialise the assignment details storage */
  assignment_details_ptr = &(dedic_ptr->assignment_details);
  memset((void *)assignment_details_ptr, 0, sizeof(assignment_details_T));

  first_chan_desc_before_received = FALSE;
  second_chan_desc_before_received = FALSE;

  assignment_details_ptr->number_of_ded_channels = 1;
  assignment_details_ptr->starting_time_valid = FALSE;
  assignment_details_ptr->before_parameters_valid = FALSE;

  /***************************************************************/
  /* By default the channel mode values for the new channel(s)   */
  /* are based on the channel mode values for the old channel(s) */
  /* unless they are explicitly changed below by the assignment  */
  /* command, same for ciphering                                 */
  /***************************************************************/
  assignment_details_ptr->channel_mode_1 = dedic_ptr->curr_channel_spec.channel_mode_1;
  assignment_details_ptr->channel_mode_2 = dedic_ptr->curr_channel_spec.channel_mode_2;

  assignment_details_ptr->cipher_flag = dedic_ptr->curr_channel_spec.cipher_flag;
  assignment_details_ptr->cipher_algorithm = dedic_ptr->curr_channel_spec.cipher_algorithm;

  /* Invalidate the multirate config    */
  rr_invalidate_multirate_config(&(dedic_ptr->next_amr_config));

  /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(ASSIGNMENT_COMMAND, gas_id);

  if (current_IE != NULL)
  {
    channel_information_T channel_info_1_after;
    memset((void *) &channel_info_1_after,0,sizeof(channel_info_1_after));

    /***********************************************************/
    /* Decode the Description of the first channel, after time */
    /***********************************************************/
    rr_decode_channel_description_2(
      current_IE->start_IE,
      &channel_info_1_after,
      &decoding_ok,
      gas_id
    );

    if (decoding_ok == TRUE)
    {
      /************************************************************************/
      /* NOTE: from this point forward, only the assignment_details's channel */
      /* info 1 after should be used                                          */
      /************************************************************************/
      assignment_details_ptr->channel_info_1_after = channel_info_1_after;


      if (assignment_details_ptr->channel_info_1_after.frequency_list.no_of_items == 1)
      {
        /*
         * Check to see if the ARFCN is valid for our configuration
         * only do if no hopping as frequencies used for hopping are
         * checked elsewhere
         */
        ARFCN_T ARFCN = assignment_details_ptr->channel_info_1_after.frequency_list.channel[0];

        // WARNING: need band info for assigment command
        if (rr_get_band(ARFCN.num, gas_id) == BAND_NONE)
        {
          RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
          assignment_failure = TRUE;
          MSG_GERAN_HIGH_1_G("Channel number=%d not supported",(int) ARFCN.num);
        }
      }
    }
    current_IE = current_IE->next_IE;
  }
  else
  {
    decoding_ok = FALSE;
    MSG_GERAN_ERROR_0_G("message_list parsing error");
  }

  /****************************/
  /* Decode the power command */
  /****************************/

  if ((decoding_ok == TRUE) && (assignment_failure == FALSE))
  {
    if (current_IE != NULL)
    {
      power_level = *(current_IE->start_IE) & 0x1F;
      assignment_details_ptr->power_level = power_level;
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  /***************************************************************************/
  /* The following IE are optionals, so we have to check the IEI to find out */
  /* what they really are                                                    */
  /***************************************************************************/

  while ((decoding_ok == TRUE) && (assignment_failure == FALSE) &&
         (current_IE != NULL) && (current_IE->next_IE != NULL))
  {
    current_IE = current_IE->next_IE;

    switch (current_IE->IEI)
    {
      case 0x05 :
      {
        /*****************************/
        /* Decode the Frequency List */
        /* after time                */
        /*****************************/

        if (current_IE->start_IE != NULL)
        {
          /* this IE is conditional, if it cannot be decoded this is an error */
          rr_decode_frequency_list(
            current_IE->start_IE,
            scell_info_ptr->gsm.BCCH_ARFCN,
            &frequency_list_after,
            &decoding_ok,
            gas_id
          );


          assignment_failure = rr_check_frequency_list(
            &frequency_list_after,
            gas_id
          );

          if (assignment_failure)
          {
            /*
             * if channels not all in one band
             */
            RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
            MSG_GERAN_HIGH_0_G("ASSIGN:Mixed bands in frequency list");
          }
          else
          {
            frequency_list_after_time = TRUE;
          }
        }
        break;
      }

      case 0x62 :
      {
        /*****************************************************************/
        /* Decode the Cell Channel Description, ie the Cell Allocation   */
        /* (CA) used for the decoding of the Mobile Allocation (MA)      */
        /* information                                                   */
        /*****************************************************************/

        if (current_IE->start_IE != NULL)
        {
          cell_channel_description_T cell_allocation = {0};

          rr_decode_cell_channel_description(
            current_IE->start_IE,
            &cell_allocation,
            &decoding_ok,
            gas_id
          );

          if (decoding_ok == TRUE)
          {
            scell_info_ptr->gsm.valid_CA_received = RR_CA_RECEIVED;
            scell_info_ptr->gsm.cell_channel_description.no_of_entries =
              cell_allocation.no_of_entries;

            for (ca_index = 0;ca_index < cell_allocation.no_of_entries; ca_index++)
            {
              scell_info_ptr->gsm.cell_channel_description.channel_number[ca_index] =
               cell_allocation.channel_number[ca_index];
            }
          }
        }
        break;
      }
      case 0x63 :
      {
        /***************************************/
        /* Decode the Channel Mode Information */
        /* for the first channel               */
        /***************************************/
        if (current_IE->start_IE != NULL)
        {
          channel_mode_T channel_mode_1;
          /*+CR444894 ASR*/
          boolean channel_mode_supported = TRUE;

          rr_decode_channel_mode(current_IE->start_IE,&channel_mode_1,&channel_mode_supported);

          if (channel_mode_supported == FALSE)
          {
            /********************************************************/
            /* If channel mode is not supported then send Assignment failure but before that */
            /* try to proccess all the paramtres in assignment command hence decoding OK is still TRUE*/
            /********************************************************/
            assignment_failure = TRUE;
            RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
           /*-CR444894 ASR*/
          }
          else
          {
            assignment_details_ptr->channel_mode_1 = channel_mode_1;
          }
        }
        break;
      }
      case 0x64 :
      {
        /**********************************************/
        /* Decode the channel description information */
        /* for the second channel after time          */
        /**********************************************/

        if (current_IE->start_IE != NULL)
        {
          channel_information_T channel_info_2_after;
          memset((void*) &channel_info_2_after,0,sizeof(channel_info_2_after));

          rr_decode_channel_description(
            current_IE->start_IE,
            &channel_info_2_after,
            &decoding_ok,
            gas_id
          );

          if (decoding_ok == FALSE)
          {
            /********************************************************/
            /* As the Information Element is OPTIONAL, we ignore it */
            /********************************************************/
            decoding_ok = TRUE;
          }
          else
          {
            assignment_details_ptr->number_of_ded_channels = 2;
            assignment_details_ptr->channel_info_2_after   = channel_info_2_after;
          }
        }
        break;
      }
      case 0x66 :
      {
        /***************************************/
        /* Decode the Channel Mode Information */
        /* for the second channel              */
        /***************************************/
        if (current_IE->start_IE != NULL)
        {
          channel_mode_T channel_mode_2;
          /*+CR444894 ASR*/
          boolean channel_mode_supported = TRUE;

          rr_decode_channel_mode(current_IE->start_IE,&channel_mode_2,&channel_mode_supported);

          if (channel_mode_supported == FALSE)
          {
            /********************************************************/
            /* If channel mode is not supported then send Assignment failure but beore that */
            /* try to proccess all the paramtres in assignment command hence decoding OK is still TRUE*/
            /********************************************************/
            assignment_failure = TRUE;
            RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
            /*-CR444894 ASR*/
          }
          else
          {
            assignment_details_ptr->channel_mode_2 = channel_mode_2;
          }
        }
        break;
      }
      case 0x72 :
      {
        /***************************************************/
        /* Decode the Mobile Allocation (MA) after time    */
        /***************************************************/
        if (current_IE->start_IE != NULL)
        {
          rr_decode_mobile_allocation(
            current_IE->start_IE,
            &ma_after
          );

          ma_after_time = TRUE;
        }
        break;
      }
      case 0x7C :
      {
        /****************************************/
        /* Decode the Starting Time Information */
        /****************************************/

        if (current_IE->start_IE != NULL)
        {
          word starting_time = 0;

          rr_decode_starting_time(
            current_IE->start_IE,
            &starting_time,
            &decoding_ok
          );

          assignment_details_ptr->starting_time_valid = TRUE;
          assignment_details_ptr->starting_time[0] = starting_time >> 8;
          assignment_details_ptr->starting_time[1] = starting_time & 255;
        }
        break;
      }
      case 0x19 :
      {
        /*****************************************/
        /* Decode the frequency list before time */
        /*****************************************/

        if ((current_IE->start_IE != NULL) && (assignment_details_ptr->starting_time_valid == TRUE))
        {
          rr_decode_frequency_list(
            current_IE->start_IE,
            scell_info_ptr->gsm.BCCH_ARFCN,
            &frequency_list_before,
            &decoding_ok,
            gas_id
          );

          assignment_failure = rr_check_frequency_list(
            &frequency_list_before,
            gas_id
          );

          if (assignment_failure)
          {
            /*
             * if channels not all in one band
             */
            RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
            MSG_GERAN_HIGH_0_G("ASSIGN:Mixed bands in frequency list");
          }
          else
          {
            frequency_list_before_time = TRUE;
            assignment_details_ptr->before_parameters_valid = TRUE;
          }

        }
        break;
      }
      case 0x1C :
      {
        /***********************************************************/
        /* Decode the description of the first channel before time */
        /***********************************************************/

        if ((current_IE->start_IE != NULL) && (assignment_details_ptr->starting_time_valid == TRUE))
        {
          channel_information_T channel_info_1_before;
          memset((void *) &channel_info_1_before,0,sizeof(channel_info_1_before));

          rr_decode_channel_description_2(
            current_IE->start_IE,
            &channel_info_1_before,
            &decoding_ok,
            gas_id
          );

          if (decoding_ok == FALSE)
          {
            /********************************************************/
            /* As the Information Element is OPTIONAL, we ignore it */
            /********************************************************/
            decoding_ok = TRUE;
          }
          else
          {
            assignment_details_ptr->channel_info_1_before = channel_info_1_before;
            assignment_details_ptr->before_parameters_valid = TRUE;
            first_chan_desc_before_received = TRUE;
          }
        }
        break;
      }
      case 0x1D :
      {
        /*************************************************/
        /* Description of the second channel before time */
        /*************************************************/
        if (current_IE->start_IE != NULL)
        {
          channel_information_T channel_info_2_before;
          memset((void *) &channel_info_2_before,0,sizeof(channel_info_2_before));

          rr_decode_channel_description(
            current_IE->start_IE,
            &channel_info_2_before,
            &decoding_ok,
            gas_id
          );

          if (decoding_ok == FALSE)
          {
            /********************************************************/
            /* As the Information Element is OPTIONAL, we ignore it */
            /********************************************************/
            decoding_ok = TRUE;
          }
          else
          {
            assignment_details_ptr->channel_info_2_before = channel_info_2_before;
            assignment_details_ptr->before_parameters_valid = TRUE;
            second_chan_desc_before_received = TRUE;
          }
        }
        break;
      }
      case 0x1E :
      {
        /*****************************************************/
        /* Decode the frequency channel sequence before time */
        /*****************************************************/
        if (current_IE->start_IE != NULL)
        {
          rr_decode_frequency_channel_sequence(
            current_IE->start_IE,
            &frequency_list_before,
            &decoding_ok,
            gas_id
          );

          assignment_details_ptr->before_parameters_valid = TRUE;
          freq_chan_seq_before_time = TRUE;
        }
        break;
      }
      case 0x21 :
      {
        /********************************************/
        /* Decode the mobile allocation before time */
        /********************************************/
        if (current_IE->start_IE != NULL)
        {
          rr_decode_mobile_allocation(
            current_IE->start_IE,
            &ma_before
          );

          ma_before_time = TRUE;
        }
        break;
      }
      case 0x03 :
      {
        /********************************************/
        /* Decode the multirate configuration       */
        /********************************************/
        if (current_IE->start_IE != NULL)
        {
          multirate_ie_present = TRUE;

          rr_decode_multirate_config_ie(
            current_IE->start_IE,
            &(dedic_ptr->next_amr_config),
            &decoding_ok,
            assignment_details_ptr->channel_mode_1,
            gas_id
          );

          if (!decoding_ok)
          {
            assignment_failure = TRUE;
            RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
          }
        }
        break;
      }
      case CIPHER_MODE_SETTING_IEI:
      {
        if (current_IE->start_IE != NULL)
        {
          start_ciphering = *(current_IE->start_IE) & 0x01;

          if (start_ciphering == TRUE)
          {
            cipher_algorithm = (cipher_algorithm_T) (*(current_IE->start_IE) & 0x0E) >> 1;

            /* check that the commanded ciphering mode is supported */
            assignment_details_ptr->cipher_flag = rr_cipher_mode_supported(
              cipher_algorithm,
              gas_id
            );

            if (assignment_details_ptr->cipher_flag == TRUE)
            {
              assignment_details_ptr->cipher_algorithm = cipher_algorithm;
            }
            else
            {
              /* the UE does not support the indicated ciphering algorithm */
              assignment_failure = TRUE;
              decoding_ok        = FALSE;
              RR_cause           = RR_CHANNEL_MODE_UNACCEPTABLE;
            }
          }
          else
          {
            assignment_details_ptr->cipher_flag = FALSE;
          }
        }
        break;
      }
      default :
      {
        MSG_GERAN_MED_1_G("Unhandled IEI%d",current_IE->IEI);
        break;
      }
    }
  } /* while */

  if ((decoding_ok == TRUE) && (assignment_failure == FALSE))
  {
    /******************************************************************/
    /* Take care of the channel mode 1 and 2 information              */
    /******************************************************************/
    if (rr_channel_mode_supported(assignment_details_ptr->channel_mode_1, gas_id) == FALSE)
    {
      assignment_failure = TRUE;
      RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
      MSG_GERAN_HIGH_1_G("Channel mode1 is unacceptable=%d",(int) assignment_details_ptr->channel_mode_1);
    }
    else if (assignment_details_ptr->number_of_ded_channels == 2)
    {
      if (rr_channel_mode_supported(assignment_details_ptr->channel_mode_2, gas_id) == FALSE)
      {
        assignment_failure = TRUE;
        RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
        MSG_GERAN_HIGH_1_G("Channel mode2 is unacceptable=%d",(int) assignment_details_ptr->channel_mode_2);
      }
    }

    /******************************************************************/
    /* Check if multirate config valid for channel mode 1             */
    /******************************************************************/
    if ((IS_AMR_CHANNEL_MODE(assignment_details_ptr->channel_mode_1)) &&
        (multirate_ie_present == FALSE))
    {
      if ((IS_AMR_CHANNEL_MODE(dedic_ptr->curr_channel_spec.channel_mode_1)) &&
          (dedic_ptr->curr_amr_config.valid == TRUE))
      {

#ifdef FEATURE_VAMOS_II 
        if(rr_vamos2_rcvd_chan_mode_matches_curr_chan_mode(assignment_details_ptr->channel_mode_1,
                                                             dedic_ptr->curr_channel_spec.channel_mode_1) == TRUE)  
#else
        if (assignment_details_ptr->channel_mode_1 == dedic_ptr->curr_channel_spec.channel_mode_1)
#endif /*FEATURE_VAMOS_II*/
        {
          /* multirate config can be inherited from the dedicated data */
          rr_use_existing_multirate_config(dedic_ptr);
        }
        else
        {
          /* AMR channel mode has changed but no multirate config was provided */
          assignment_failure = TRUE;
          RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
          MSG_GERAN_HIGH_2_G("current AMR type = %d New AMR type = %d",(int) dedic_ptr->curr_channel_spec.channel_mode_1,
                    (int)assignment_details_ptr->channel_mode_1);
          MSG_GERAN_ERROR_0_G("No multirate config in ASSIGNMENT COMMAND");
        }
      }
      else
      {
        /* existing channel config has no valid multirate config */
        MSG_GERAN_MED_0_G("Multirate config not available from previous assignment");
        assignment_failure = TRUE;
        RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
      }
    }
#ifdef FEATURE_VAMOS_II
    if (assignment_details_ptr->channel_mode_1 == SPEECH_V3 || 
        assignment_details_ptr->channel_mode_1 == SPEECH_V3_VAMOS2)
#else
    if (assignment_details_ptr->channel_mode_1 == SPEECH_V3)
#endif /*FEATURE_VAMOS_II*/
    {
      /* For half rate the last 2 bits in the codec modes of the Active Codec Set that correspond
      to the rates of 12.2 and 10.2 should not be set  */
      if ((dedic_ptr->next_amr_config.valid == TRUE) &&
          (assignment_details_ptr->channel_info_1_after.channel_type == HALF_RATE_TRAFFIC))
      {
        if (dedic_ptr->next_amr_config.codec_modes & 0xC0)
        {
          assignment_failure = TRUE;
          RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
          MSG_GERAN_HIGH_0_G("Unsupported Codec modes for Half-rate");
        }
      }
    }

#ifdef FEATURE_GSM_AMR_WB

#ifdef FEATURE_VAMOS_II
    if (assignment_details_ptr->channel_mode_1 == SPEECH_V5 ||
        assignment_details_ptr->channel_mode_1 == SPEECH_V5_VAMOS2)
#else
    if (assignment_details_ptr->channel_mode_1 == SPEECH_V5)
#endif /*FEATURE_VAMOS_II*/
    {
      /* For SPEECH_V5 channel mode, channel type should be full rate*/
      if (assignment_details_ptr->channel_info_1_after.channel_type == HALF_RATE_TRAFFIC)
      {
        assignment_failure = TRUE;
        RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
        MSG_GERAN_HIGH_0_G("Unsupported Channel type for AMR WB");
      }
    }
#endif /*FEATURE_GSM_AMR_WB*/

    /* If the assignment is related to an intra-cell handover from a multi-rate speech codec to a
    multi-rate speech codec, the MultiRate Configuration IE shall be included in the case of
    full rate to half rate. */

#ifdef FEATURE_VAMOS_II
    if( ((dedic_ptr->curr_channel_spec.channel_mode_1 == SPEECH_V3) &&
         (assignment_details_ptr->channel_mode_1      == SPEECH_V3))
        ||
        ((dedic_ptr->curr_channel_spec.channel_mode_1 == SPEECH_V3_VAMOS2) &&
         (assignment_details_ptr->channel_mode_1      == SPEECH_V3_VAMOS2))
      )
#else
    if ((dedic_ptr->curr_channel_spec.channel_mode_1 == SPEECH_V3) &&
        (assignment_details_ptr->channel_mode_1      == SPEECH_V3))
#endif /*FEATURE_VAMOS_II*/
    {
      if ((dedic_ptr->curr_channel_spec.channel_info_1_after.channel_type == FULL_RATE_TRAFFIC) &&
          (assignment_details_ptr->channel_info_1_after.channel_type      == HALF_RATE_TRAFFIC))
      {
        if (multirate_ie_present == FALSE)
        {
          assignment_failure = TRUE;
          RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
          MSG_GERAN_HIGH_0_G("Multi-rate IE should be present");
        }
      }
    }

    /******************************************************************/
    /* Take care of the channel information 1 after time              */
    /******************************************************************/

    if (!assignment_failure)
    {
      if (rr_channel_type_supported(
            assignment_details_ptr->channel_info_1_after.channel_type,
            gas_id
          ) == FALSE)
      {
        assignment_failure = TRUE;
        RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
        /* channel mode unacceptable */
        MSG_GERAN_HIGH_1_G("Channel type is unacceptable=%d",(int) assignment_details_ptr->channel_info_1_after.channel_type);
      }
      /* channel type is acceptable, check the hopping requirements */
      else if (assignment_details_ptr->channel_info_1_after.hopping_flag)
      {
        if (ma_after_time)
        {
          if ((scell_info_ptr->gsm.valid_CA_received == RR_CA_RECEIVED)||
              (scell_info_ptr->gsm.valid_CA_received == RR_CA_RECEIVED_BUT_INVALID))
          {
            boolean result;
            /******************************************************/
            /* set the frequency list from the mobile allocation  */
            /* using the cell channel description received in the */
            /* HANDOVER COMMAND message itself                    */
            /******************************************************/
            result = rr_set_channel_frequency_list_from_mobile_allocation(
              &assignment_details_ptr->channel_info_1_after,
              &ma_after,
              &scell_info_ptr->gsm.cell_channel_description,
              gas_id
            );

            /* if there was a problem setting the frequency list */
            if (!result)
            {
              assignment_failure = TRUE;
              RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
              MSG_GERAN_HIGH_0_G("After MA freq not in same band");
            }
          }
          else /* valid CA not available */
          {
            /**********************************************/
            /* No CA is available. We can't decode the MA */
            /**********************************************/

            assignment_failure = TRUE;
            RR_cause = RR_NO_CELL_ALLOCATION_AVAILABLE;
            MSG_GERAN_HIGH_0_G("No CA Rx'd");
          }
        }
        else if (frequency_list_after_time)
        {
          assignment_details_ptr->channel_info_1_after.frequency_list = frequency_list_after;
        }
        else
        {
          MSG_GERAN_HIGH_0_G("CondIE: Freq.Info. not received for hopping channel");
          decoding_ok = FALSE;
          rr_status = RR_CONDITIONAL_IE_ERROR;
        }
      } /* hopping channel */
    } /* not failed yet */

    /******************************************************************/
    /* Take care of the channel information 2 after time              */
    /******************************************************************/
    if (!assignment_failure && decoding_ok)
    {
      if (assignment_details_ptr->number_of_ded_channels == 2)
      {
        if (rr_channel_type_supported(
              assignment_details_ptr->channel_info_2_after.channel_type,
              gas_id
            ) == FALSE)
        {
          assignment_failure = TRUE;
          RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
          MSG_GERAN_HIGH_1_G("Channel type is unacceptable=%d",(int) assignment_details_ptr->channel_info_2_after.channel_type);
        }
        else if (assignment_details_ptr->channel_info_2_after.hopping_flag)
        {
          if (ma_after_time == TRUE)
          {
            if ((scell_info_ptr->gsm.valid_CA_received == RR_CA_RECEIVED)||
                 (scell_info_ptr->gsm.valid_CA_received == RR_CA_RECEIVED_BUT_INVALID))
            {
              boolean result;
              /******************************************************/
              /* set the frequency list from the mobile allocation  */
              /* using the cell channel description received in the */
              /* HANDOVER COMMAND message itself                    */
              /******************************************************/
              result = rr_set_channel_frequency_list_from_mobile_allocation(
                &assignment_details_ptr->channel_info_2_after,
                &ma_after,
                &scell_info_ptr->gsm.cell_channel_description,
                gas_id
              );

              /* if there was a problem setting the frequency list */
              if (!result)
              {
                assignment_failure = TRUE;
                RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
                MSG_GERAN_HIGH_0_G("After2 MA freq not in same band");
              }
            }
            else
            {
              /**********************************************/
              /* No CA is available. We can't decode the MA */
              /**********************************************/
              assignment_failure = TRUE;
              RR_cause = RR_NO_CELL_ALLOCATION_AVAILABLE;
              MSG_GERAN_HIGH_0_G("No CA (2)");
            }
          }
          else if (frequency_list_after_time)
          {
            assignment_details_ptr->channel_info_2_after.frequency_list = frequency_list_after;
          }
          else
          {
            MSG_GERAN_HIGH_0_G("CondIE: FreqInfo not received for hopping channel");
            decoding_ok = FALSE;
            rr_status = RR_CONDITIONAL_IE_ERROR;
          }
        } /* hopping channel */
      } /* 2 ded channels */
    }

    if (!assignment_failure && decoding_ok && assignment_details_ptr->starting_time_valid)
    {
      if ((first_chan_desc_before_received == FALSE) &&
          (assignment_details_ptr->before_parameters_valid == TRUE))
      {
        /*****************************************************************/
        /* Copy the channel description of the first channel after time  */
        /* in the assignment_details_ptr->channel_information_1_before_time   */
        /*****************************************************************/
        assignment_details_ptr->channel_info_1_before = assignment_details_ptr->channel_info_1_after;
      }

      if ((second_chan_desc_before_received == FALSE) &&
          (assignment_details_ptr->before_parameters_valid == TRUE) &&
          (assignment_details_ptr->number_of_ded_channels == 2))
      {
        /*****************************************************************/
        /* Copy the channel description of the second channel after time */
        /* in the assignment_details_ptr->channel_information_2_before_time   */
        /*****************************************************************/
        assignment_details_ptr->channel_info_2_before = assignment_details_ptr->channel_info_2_after;
      }

      /******************************************************************/
      /* Take care of the channel information 1 before time             */
      /******************************************************************/
      if (rr_channel_type_supported(
            assignment_details_ptr->channel_info_1_before.channel_type,
            gas_id
          ) == FALSE)
      {
        assignment_failure = TRUE;
        RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
        MSG_GERAN_HIGH_1_G("Channel type is unacceptable=%d",(int) assignment_details_ptr->channel_info_1_before.channel_type);
      }
      else
      {
        if (assignment_details_ptr->channel_info_1_before.hopping_flag)
        {
          if (ma_before_time)
          {
            if ((scell_info_ptr->gsm.valid_CA_received == RR_CA_RECEIVED)||
                 (scell_info_ptr->gsm.valid_CA_received == RR_CA_RECEIVED_BUT_INVALID))
            {
              boolean result;
              /******************************************************/
              /* set the frequency list from the mobile allocation  */
              /* using the cell channel description received in the */
              /* HANDOVER COMMAND message itself                    */
              /******************************************************/
              result = rr_set_channel_frequency_list_from_mobile_allocation(
                &assignment_details_ptr->channel_info_1_before,
                &ma_before,
                &scell_info_ptr->gsm.cell_channel_description,
                gas_id
              );

              /* if there was a problem setting the frequency list */
              if (!result)
              {
                assignment_failure = TRUE;
                RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
                MSG_GERAN_HIGH_0_G("Before MA freq not in same band");
              }
            }
            else
            {
              /**********************************************/
              /* No CA is available. We can't decode the MA */
              /**********************************************/
              assignment_failure = TRUE;
              RR_cause = RR_NO_CELL_ALLOCATION_AVAILABLE;
              MSG_GERAN_HIGH_0_G("No CA(before) V1");
            }
          }
          else if (freq_chan_seq_before_time == TRUE)
          {
            assignment_details_ptr->channel_info_1_before.frequency_list = frequency_list_before;
          }
          else if (frequency_list_before_time == TRUE)
          {
            assignment_details_ptr->channel_info_1_before.frequency_list = frequency_list_before;
          }
          else
          {
            /************************************************************/
            /* No frequency parameters before time were present in the  */
            /* message                                                  */
            /* Pick up the after parameters                             */
            /************************************************************/
            no_of_entries = assignment_details_ptr->channel_info_1_after.frequency_list.no_of_items;

            for (index = 0; index < no_of_entries; index++)
            {
              assignment_details_ptr->channel_info_1_before.frequency_list.channel[index] =
               assignment_details_ptr->channel_info_1_after.frequency_list.channel[index];
            }
            assignment_details_ptr->channel_info_1_before.frequency_list.no_of_items = no_of_entries;
          }
        }
      }

      /******************************************************************/
      /* Take care of the channel information 2 before time             */
      /******************************************************************/
      if (assignment_details_ptr->number_of_ded_channels == 2)
      {
        if (rr_channel_type_supported(
              assignment_details_ptr->channel_info_2_before.channel_type,
              gas_id
            ) == FALSE)
        {
          assignment_failure = TRUE;
          RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
          MSG_GERAN_HIGH_1_G("Channel type is unacceptable=%d",(int) assignment_details_ptr->channel_info_2_before.channel_type);
        }
        else if (assignment_details_ptr->channel_info_2_before.hopping_flag)
        {
          if (ma_before_time)
          {
            if ((scell_info_ptr->gsm.valid_CA_received == RR_CA_RECEIVED)||
                 (scell_info_ptr->gsm.valid_CA_received == RR_CA_RECEIVED_BUT_INVALID))
            {
              boolean result;
              /******************************************************/
              /* set the frequency list from the mobile allocation  */
              /* using the cell channel description received in the */
              /* HANDOVER COMMAND message itself                    */
              /******************************************************/
              result = rr_set_channel_frequency_list_from_mobile_allocation(
                &assignment_details_ptr->channel_info_2_before,
                &ma_before,
                &scell_info_ptr->gsm.cell_channel_description,
                gas_id
              );

              /* if there was a problem setting the frequency list */
              if (!result)
              {
                assignment_failure = TRUE;
                RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
                MSG_GERAN_HIGH_0_G("Before2 MA freq not in same band");
              }
            }
            else
            {
              /**********************************************/
              /* No CA is available. We can't decode the MA */
              /**********************************************/
              assignment_failure = TRUE;
              RR_cause = RR_NO_CELL_ALLOCATION_AVAILABLE;
              MSG_GERAN_HIGH_0_G("No CA (2 before) V1");
            }
          }
          else if (freq_chan_seq_before_time == TRUE)
          {
            assignment_details_ptr->channel_info_2_before.frequency_list = frequency_list_before;
          }
          else if (frequency_list_before_time == TRUE)
          {
            assignment_details_ptr->channel_info_2_before.frequency_list = frequency_list_before;
          }
          else
          {
            /************************************************************/
            /* No frequency parameters before time were present in the  */
            /* message                                                  */
            /* Pick up the after parameters                             */
            /************************************************************/
            no_of_entries = assignment_details_ptr->channel_info_2_after.frequency_list.no_of_items;

            for (index = 0; index < no_of_entries; index++)
            {
              assignment_details_ptr->channel_info_2_before.frequency_list.channel[index] =
               assignment_details_ptr->channel_info_2_after.frequency_list.channel[index];
            }
            assignment_details_ptr->channel_info_2_before.frequency_list.no_of_items = no_of_entries;
          }
        }
      } /* number of dedicated channels was 2 */
    } /* starting time was present */

    /*********************************************************************/
    /* We have now to check the information embedded in the ASSIGNMENT   */
    /* COMMAND message to perform the according actions                  */
    /*********************************************************************/
    if ((assignment_failure == FALSE) && (decoding_ok == TRUE))
    {
      if (start_ciphering == TRUE)
      {
        if (rr_get_ciphering_mode_command_received(gas_id))
        {
          rr_ciphering_has_started(TRUE, gas_id);
        }
        else
        {
          assignment_failure = TRUE;
          RR_cause = RR_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE;
          MSG_GERAN_HIGH_0_G("No previous Ciphering command");
        }
      }

      if (!rr_check_assignment_frequency_lists_supported(assignment_details_ptr, gas_id))
      {
        MSG_GERAN_HIGH_0_G("Frequency not implemented in channel assignment");
        assignment_failure = TRUE;
        RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
      }
    }

    /* if there was a failure or decoding not OK */
    if (assignment_failure || !decoding_ok)
    {
      /************************************************************************************/
      /* check for decoding OK flag, if set to FALSE there may have been a decoding error */
      /* while processing the assignment details                                          */
      /************************************************************************************/
      if (assignment_failure)
      {
        /******************************************************/
        /* Generate a ASSIGNMENT_FAILURE message with cause   */
        /* identified above, using DL_DATA_REQ                */
        /******************************************************/
        rr_perform_assignment_failure(
          RR_cause,   // RR_cause
          FALSE,      // reconnect
          gas_id      // gas_id
        );
      }
      else /* decoding was not OK */
      {
        /************************************************************/
        /* The MS ignores the message, and, as an RR connection     */
        /* exists, sends an RR_STATUS with appropriate cause value  */
        /************************************************************/
        MSG_GERAN_HIGH_1_G("OMsg(NW,STATUS(%d))",(int) rr_status);

        rr_send_status(
          rr_status,    // RR_cause
          gas_id        // gas_id
        );
      }
    }
    else /* no failure and decoding OK */
    {
      /* SAPI3 link may be pending or established, but any action */
      /* required must be decided by the separate RR SAPI3 module */
      rr_release_sapi3(gas_id);

      /* suspend SAPI0 for this intra-cell handover */
      rr_send_dl_suspend_req(RR_L1_GSM_GSM, gas_id);

      /* Set current_dedicated_channel_band. Channel info for first channel
         after start time is always present. After time and before time band
         for first and second channels, if present, are considered as the same
         whether it is hopping or not. Therefore, the band of first element
         in frequency list for first channel after time is taken as the current
         dedicated band */
      rr_set_current_dedicated_band(
        (rr_internal_band_T) assignment_details_ptr->channel_info_1_after.frequency_list.channel[0].band,
        gas_id
      );

      /* Debug messages */
      if ((assignment_details_ptr->before_parameters_valid == TRUE) &&
          (assignment_details_ptr->channel_info_1_after.frequency_list.channel[0].band
           != assignment_details_ptr->channel_info_1_before.frequency_list.channel[0].band)
         )
      {
        MSG_GERAN_HIGH_3_G("chan 1: before band %d, after band %d",
        assignment_details_ptr->channel_info_1_before.frequency_list.channel[0].band,
        assignment_details_ptr->channel_info_1_after.frequency_list.channel[0].band,
        0);
      }

      if ((assignment_details_ptr->before_parameters_valid == TRUE) &&
          (assignment_details_ptr->channel_info_2_after.frequency_list.channel[0].band
           != assignment_details_ptr->channel_info_2_before.frequency_list.channel[0].band)
         )
      {
        MSG_GERAN_HIGH_3_G("chan 2: before band %d, after band %d",
        assignment_details_ptr->channel_info_2_before.frequency_list.channel[0].band,
        assignment_details_ptr->channel_info_2_after.frequency_list.channel[0].band,
        0);
      }


      new_event = EV_ASSIGNMENT_COMMAND_CHECKED;
    }
  }
  else /* decoding not ok, but syntactically errors found */
  {
    if (assignment_failure == FALSE)
    {
      /*****************************************************************/
      /* one of the Information Element has is syntactically incorrect */
      /* The action taken depends on the Presence parameter of the IE  */
      /*****************************************************************/

      // Default action - send a RR Status message with cause #111 "protocol error unspecified"
      RR_cause = RR_PROTOCOL_ERROR_UNSPECIFIED;

      if (current_IE != NULL)
      {
        if (MANDATORY_IE == current_IE->presence)
        {
          MSG_GERAN_HIGH_0_G("OMsg(NW,STATUS(InvalidMandatory");

          // Send a RR Status message with cause #96 "invalid mandatory information"
          RR_cause = RR_INVALID_MANDATORY_INFORMATION;
        }
        else
        if (CONDITIONAL_IE == current_IE->presence)
        {
          MSG_GERAN_HIGH_0_G("OMsg(NW,STATUS(ConditionalIE");

          // Send a RR Status message with cause #100 "conditional IE error"
          RR_cause = RR_CONDITIONAL_IE_ERROR;
        }
      }

      rr_send_status(RR_cause, gas_id);
    }
    else
    {
      /******************************************************/
      /* assignment_failure is TRUE, and RR_cause contains  */
      /* the reason, so....                                 */
      /* Generate a ASSIGNMENT_FAILURE message with cause   */
      /* identified above, using DL_DATA_REQ                */
      /******************************************************/
      rr_perform_assignment_failure(
        RR_cause,   // RR_cause
        FALSE,      // reconnect
        gas_id      // gas_id
      );
    }
  }

  return new_event;
} /* rr_respond_to_assignment_command */


/**
 * State machine for intra cell handovers
 *
 * @param assignment_event event to be processed by the state machine
 *
 * @param new_message      pointer to received message
 *
 * @return event to be processed by calling state machine
 */
rr_event_T rr_intra_cell_handover_control(
  rr_event_T assignment_event,
  void *new_message,
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T  *dedic_ptr;
  assignment_details_T *assignment_details_ptr;
  byte                  message_id   = 0;
  byte                  message_set  = 0;
  rr_event_T            new_assignment_event = EV_NO_EVENT;
  rr_event_T            proc_dispatch_event = EV_NO_EVENT;
  rr_intra_ho_data_t   *rr_intra_ho_data_ptr;

  // Obtain a pointer to the module data
  rr_intra_ho_data_ptr = rr_intra_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_intra_ho_data_ptr);

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr == NULL)
  {
    return(EV_NO_EVENT);
  }
  assignment_details_ptr = &(dedic_ptr->assignment_details);

  assignment_event = rr_extract_message_header_details(
    assignment_event,
    new_message,
    &message_set,
    &message_id
  );

  while (assignment_event != EV_NO_EVENT)
  {
    if (assignment_event == EV_RESET_SOFTWARE)
    {
      if ( rr_intra_ho_data_ptr->assignment_state != ASSIGNMENT_INACTIVE )
      {
        MSG_GERAN_HIGH_1_G("Intra-cell HO aborted (state=%d)",(int)rr_intra_ho_data_ptr->assignment_state);
      }
      rr_intra_ho_data_ptr->assignment_state = ASSIGNMENT_INACTIVE;
      assignment_event = EV_NO_EVENT;
      rr_intra_ho_data_ptr->old_band = BAND_NONE;
    }

    switch (rr_intra_ho_data_ptr->assignment_state)
    {
      case ASSIGNMENT_INACTIVE :
      {
        if ( (assignment_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if ((message_set == MS_RR_L2 ) && (message_id == (byte) DL_DATA_IND))
          {
            dl_data_ind_T *dl_data_ind_ptr = (dl_data_ind_T *)new_message;

            /*********************************************************/
            /* A message has been received from the network in an    */
            /* acknowledge mode . It should be an ASSIGNMENT COMMAND */
            /*********************************************************/
            if (dl_data_ind_ptr->layer3_message[1] == ASSIGNMENT_COMMAND)
            {
              /* Save the current dedicated band - if the dedicated band does
              not change as a result of the assignment command, SACCH SIs
              remain valid and do not need to be reacquired. */
              rr_intra_ho_data_ptr->old_band = rr_get_current_dedicated_band(gas_id);

              new_assignment_event = rr_respond_to_assignment_command(
                dedic_ptr,
                gas_id
              );

              /*
               * returns NO_EVENT if an error, having sent the response
               * to the network, or EV_ASSIGNMENT_COMMAND_CHECKED if OK
               * and the DL_SUSPEND_REQ has been sent to L2
               */
            }
          }
        }
        else if (assignment_event == EV_ASSIGNMENT_COMMAND_CHECKED)
        {
          /*
           * notify the procedures dispatcher that we are in the process
           * of performing an intra cell handover
           */
          rr_eng_mode_update_codec_ho_info(
            dedic_ptr->curr_channel_spec.channel_mode_1,
            assignment_details_ptr->channel_mode_1,
            assignment_details_ptr->channel_info_1_after.channel_type,
            FALSE,
            SYS_HO_START,
            SYS_HO_TYPE_GSM_GSM,
            gas_id
          );

          rr_eng_mode_update_codec_ho_info(
            dedic_ptr->curr_channel_spec.channel_mode_1,
            assignment_details_ptr->channel_mode_1,
            assignment_details_ptr->channel_info_1_after.channel_type,
            FALSE,
            SYS_SPEECH_CODEC_INFO,
            SYS_HO_TYPE_GSM_GSM,
            gas_id
          );

          proc_dispatch_event = EV_INTRA_CELL_HANDOVER;
          rr_intra_ho_data_ptr->assignment_state = AWAITING_LOCAL_RELEASE;
        }
        break;
      }
      case AWAITING_LOCAL_RELEASE :
      {
        if ( (assignment_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          /************************************************************/
          /* The Layer 2 confirms that it is now in a suspended state */
          /************************************************************/

          if ( message_set == MS_RR_L2 )
          {
            if (message_id == (byte) DL_SUSPEND_CNF)
            {
              /* Save the current channel details in case L2 resumption fails, */
              /* and we have to return to the old channel, and resume there    */
              rr_preserve_current_dedicated_config(gas_id);

              rr_send_mph_channel_assign_req(
                assignment_details_ptr,
                &(dedic_ptr->next_amr_config),
                gas_id
              );

              /* L1 stops using DTX on receipt of an ASSIGNMENT. Keep the DTX */
              /* indication FALSE until the procedure is complete */
              rr_dtx_rr_override_on(gas_id);

              rr_intra_ho_data_ptr->assignment_state = AWAITING_ASSIGNMENT_CONFIRMATION;
            }
            else if ((message_id == (byte) DL_RELEASE_IND) ||
                     (message_id == (byte) MDL_ERROR_IND)
                    )
            {
              /*
               * a lower layer failure has occurred while we are
               * waiting for l2 suspension, so give up the assignment
               * and go back to idle
               */
              rr_intra_ho_data_ptr->assignment_state = ASSIGNMENT_INACTIVE;
              proc_dispatch_event = EV_RADIO_LINK_FAILURE;
            }
          }
        }
        break;
      }
      case AWAITING_ASSIGNMENT_CONFIRMATION :
      {
        if ( (assignment_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          /***********************************************************/
          /* The layer 1 was asked to switch on the new channels by  */
          /* the MPH_CHANNEL_ASSIGN_REQ primitive                    */
          /***********************************************************/

          if ( message_set == MS_RR_L1)
          {
            if (message_id == (byte) MPH_CHANNEL_ASSIGN_CNF)
            {
              mph_channel_assign_cnf_T *mph_channel_assign_cnf =
                               (mph_channel_assign_cnf_T *)new_message;

              if ( mph_channel_assign_cnf->error_code == NO_ERROR )
              {
                rr_complete_channel_assignment(dedic_ptr, gas_id);
                rr_intra_ho_data_ptr->assignment_state = WAITING_FOR_CONFIRMATION_OF_RESUME;
              }
              else
              {
                rr_trigger_assignment_failure_response(gas_id);
                rr_intra_ho_data_ptr->assignment_state = AWAITING_RECONNECTION;
              }
            }
          }
          else if ( message_set == MS_RR_L2 )
          {
            if ((message_id == (byte) DL_RELEASE_IND) ||
                (message_id == (byte) MDL_ERROR_IND)
               )
            {
              /*
               * a lower layer failure has occurred while we are
               * trying to do a channel assignment, so give up the
               * assignment and go back to idle
               */
              rr_intra_ho_data_ptr->assignment_state = ASSIGNMENT_INACTIVE;
              proc_dispatch_event = EV_RADIO_LINK_FAILURE;
            }
          }
        }
        break;
      }
      case WAITING_FOR_CONFIRMATION_OF_RESUME :
      {
        if ( (assignment_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if (message_set == MS_RR_L2 )
          {
            if  (message_id == (byte) DL_RESUME_CNF)
            {
              rr_set_link_status(SAPI0_LINK, gas_id);

              dedic_ptr->curr_channel_spec.cipher_flag = assignment_details_ptr->cipher_flag;
              dedic_ptr->curr_channel_spec.cipher_algorithm = assignment_details_ptr->cipher_algorithm;

              rr_send_handover_sync_ind(dedic_ptr, gas_id);

              /* Revert the DTX indication to match that broadcast by the network */
              /* (This will take effect on the next reception of SI5/6) */
              rr_dtx_rr_override_off(gas_id);

              /* Reset the S_counter to RADIO_LINK_TIMEOUT */
              rr_reset_S_counter(gas_id);

              proc_dispatch_event = EV_HANDOVER_TERMINATED;

              /* log the change to the channel configuration */
              (void)rr_log_channel_configuration(gas_id);

              /**
               * See 3GPP TS 44.018 section 3.4.1.2:
               *
               * "After intracell change of channel (i.e. via ASSIGNMENT
               * COMMAND, DTM ASSIGNMENT COMMAND, or HANDOVER COMMAND
               * message) where the frequency band is not changed (GSM and
               * E-GSM are considered to belong to the same frequency band),
               * the MS may use the BA (list) received on SACCH in system
               * information 5 and optionally 5bis and/or 5ter messages in
               * the previous channel configuration during the time it has
               * not yet received a new list in the new channel. In case the
               * BCCH allocation sequence number (BA_IND) received in the new
               * channel indicates the same value as the one already being
               * used by the mobile station, the mobile station is not
               * required to re-build the BA(list) in the new channel."
               */
              if (!rr_bands_equal(rr_intra_ho_data_ptr->old_band, rr_get_current_dedicated_band(gas_id)))
              {
                MSG_GERAN_MED_0_G("Frequency band changed after ASSIGNMENT_COMMAND, clearing SACCH system information");
                rr_clear_si_sacch(gas_id);
                rr_ded_set_measurement_reports_invalid(TRUE, gas_id);

                /* If the old band and new band are different, we need to swap
                the cached SI5 and SI5ter BA lists around. */
                rr_swap_old_si5_and_si5ter_ba_lists(gas_id);
              }
              else
              {
                MSG_GERAN_MED_0_G("Frequency band not changed after ASSIGNMENT_COMMAND, preserving SACCH system information");
              }

#ifdef FEATURE_GSM_BAND_AVOIDANCE
              if (rr_channel_mode_is_voice(assignment_details_ptr->channel_mode_1))
              {
                rr_band_avoid_ded_freq_upd_send_imsg(&assignment_details_ptr->channel_info_1_after.frequency_list, gas_id);
              }
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

              /* curr_channel_spec was assign to prev_channel_spec before HO complete in AWAITING_ASSIGNMENT_CONFIRMATION*/
              /* use prev_channel_spec here to ensure that there is a diff between the assignment and current mode*/
              rr_eng_mode_update_codec_ho_info(
                dedic_ptr->prev_channel_spec.channel_mode_1,
                assignment_details_ptr->channel_mode_1,
                assignment_details_ptr->channel_info_1_after.channel_type,
                FALSE,
                SYS_HO_COMPLETE,
                SYS_HO_TYPE_GSM_GSM,
                gas_id
              );

              rr_intra_ho_data_ptr->assignment_state = ASSIGNMENT_INACTIVE;
            }
            else if ((message_id == (byte) DL_RELEASE_IND) ||
                     (message_id == (byte) MDL_ERROR_IND)
                    )
            {
              rr_send_mph_reconnect_channel_req(gas_id);
              rr_intra_ho_data_ptr->assignment_state = AWAITING_PHYSICAL_RECONNECTION;
            }
          }
        }
        break;
      }
      case AWAITING_PHYSICAL_RECONNECTION :
      {
        if ( (assignment_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if ( message_set == MS_RR_L1 )
          {
            if (message_id == (byte) MPH_RECONNECT_CHANNEL_CNF)
            {
              rr_trigger_assignment_failure_response(gas_id);
              rr_intra_ho_data_ptr->assignment_state = AWAITING_RECONNECTION;
            }
          }
          else if ( message_set == MS_RR_L2 )
          {
            if ((message_id == (byte) DL_RELEASE_IND) ||
                (message_id == (byte) MDL_ERROR_IND)
               )
            {
              /*
               * a lower layer failure has occurred while we are
               * waiting for l1 to perform physical reconnection, so
               * give up and go back to idle
               */
              rr_intra_ho_data_ptr->assignment_state = ASSIGNMENT_INACTIVE;
              proc_dispatch_event = EV_RADIO_LINK_FAILURE;
            }
          }
        }
        break;
      }
      case AWAITING_RECONNECTION :
      {
        if ( (assignment_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if ( message_set == MS_RR_L2 )
          {
            if (message_id == (byte) DL_RECONNECT_CNF)
            {
              /* Reset the S_counter to RADIO_LINK_TIMEOUT */
              rr_reset_S_counter(gas_id);

              proc_dispatch_event = EV_HANDOVER_TERMINATED;
              rr_intra_ho_data_ptr->assignment_state = ASSIGNMENT_INACTIVE;

              /* SAPI0 link has been reconnected */
              rr_set_link_status(SAPI0_LINK, gas_id);

              /* Revert the DTX indication to match that broadcast by the network */
              /* (This will take effect on the next reception of SI5/6) */
              rr_dtx_rr_override_off(gas_id);

              rr_eng_mode_update_codec_ho_info(
                dedic_ptr->curr_channel_spec.channel_mode_1,
                assignment_details_ptr->channel_mode_1,
                assignment_details_ptr->channel_info_1_after.channel_type,
                FALSE,
                SYS_HO_FAIL,
                SYS_HO_TYPE_GSM_GSM,
                gas_id
              );
            }
            else if ((message_id == (byte) DL_RELEASE_IND) ||
                     (message_id == (byte) MDL_ERROR_IND)
                    )
            {
              /*
               * a lower layer failure has occurred while we are
               * waiting for l2 reconnection, so give up
               * and go back to idle
               */
              rr_intra_ho_data_ptr->assignment_state = ASSIGNMENT_INACTIVE;
              proc_dispatch_event = EV_RADIO_LINK_FAILURE;
            }
          }
        }
        break;
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    if ( ( (assignment_event != new_assignment_event) &&
           (new_assignment_event != EV_NO_EVENT) ) ||
         (rr_intra_ho_data_ptr->old_assignment_state != rr_intra_ho_data_ptr->assignment_state) )
    {
       rr_store_trace_msg_buf(
         RR_INTRA_CELL_HO_SM,
         new_assignment_event,
         (byte) rr_intra_ho_data_ptr->assignment_state,
         new_message,
         gas_id
       );
    }
#endif
    if (new_assignment_event != EV_NO_EVENT)
    {
      assignment_event = new_assignment_event;
      new_assignment_event = EV_NO_EVENT;
    }
    else
    {
      assignment_event = EV_NO_EVENT;
    }
  }

  if ( rr_intra_ho_data_ptr->old_assignment_state != rr_intra_ho_data_ptr->assignment_state )
  {
    RR_LOG_STATE_TRANSITION(
      "rr_intra_cell_handover_control",
      rr_intra_ho_data_ptr->old_assignment_state,
      rr_intra_ho_data_ptr->assignment_state,
      rr_assignment_state_name,
      gas_id
    );

    rr_intra_ho_data_ptr->old_assignment_state = rr_intra_ho_data_ptr->assignment_state;
  }

  return proc_dispatch_event;
} /* rr_intra_cell_handover_control */

/* EOF */

