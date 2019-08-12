/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   RR GSM To WCDMA Handover Module

GENERAL DESCRIPTION
   This module performs the handshaking between RR and the other layers when
   performing a handover to WCDMA

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_g2w_handover.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
22/01/14    sjw     Use inter-RAT TRM lock exchange for G2W handover
05/05/09    sjw     Reworked interfaces between G2G and G2W handover code
02/13/09    sjw     Reworked local storage for ciphering module data
11/07/08    sjw     Allow SAPI3 control to decide whether release is required
05/07/04    gc      Enhanced Trace Buffer Code
01/22/03    JAC     Started

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_WCDMA
#include "amssassert.h"
#include "rr_seg_load.h"
#include "rr_defs.h"
#include "rr_g2w_handover.h"
#include "rr_general.h"
#include "rr_gprs_debug.h"
#include "rr_log.h"
#include "rr_cipher.h"
#include <string.h>
#include "rr_sapi3.h"
#include "rr_inter_ho.h"
#include "gprs_mem.h"
#include "rr_ded_config.h"
#include "rr_ps_access.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"


typedef struct
{
  boolean                     abort_req_received; /* Keeps track of whether RR_ABORT_REQ was received during the handover */

  /* Handover To UTRAN Command IE, including 1 byte length header. */
  byte                        g2w_ho_msg[MAX_OCTETS_L3_MESSAGE];
}rr_g2w_handover_data_t;

typedef struct
{
  rr_g2w_ho_state_T            g2w_ho_state;
  rr_g2w_ho_state_T            old_g2w_ho_state;
  rr_rrc_failure_cause_e_type  rrc_failure_cause;
  rr_g2w_handover_data_t      *data_ptr;
}rr_g2w_handover_static_data_t;

static rr_g2w_handover_static_data_t  rr_g2w_handover_data[NUM_GERAN_DATA_SPACES];

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_g2w_handover_static_data_t*
 */
static rr_g2w_handover_static_data_t *rr_g2w_handover_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_g2w_handover_data[as_index];
}

/**
 * Initialisation function called when this module is activated
 */
static void rr_g2w_handover_activate_init(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_g2w_handover_static_data_t *rr_g2w_handover_data_ptr = rr_g2w_handover_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_handover_data_ptr);

  RR_ASSERT_FATAL(rr_g2w_handover_data_ptr->data_ptr == NULL);

  rr_g2w_handover_data_ptr->data_ptr = GPRS_MEM_CALLOC(sizeof(rr_g2w_handover_data_t));

} /* rr_g2w_handover_activate_init */

/**
 * Discard function called when this module is dactivated. Any allocated memory should be freed.
 */
void rr_g2w_handover_discard_data(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_g2w_handover_static_data_t *rr_g2w_handover_data_ptr = rr_g2w_handover_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_handover_data_ptr);

  if (rr_g2w_handover_data_ptr->data_ptr != NULL)
  {
    GPRS_MEM_FREE(rr_g2w_handover_data_ptr->data_ptr);
    rr_g2w_handover_data_ptr->data_ptr = NULL;
  }

} /* rr_g2w_handover_discard_data */

/**
 * Returns the name of the GSM to WCDMA handover state in string form
 *
 * @param cell_select_state State
 *
 * @return Name of the state in string form
 */
static char * rr_g2w_ho_state_name( rr_g2w_ho_state_T g2w_ho_state )
{
  char *s = "";
  switch ( g2w_ho_state )
  {
    case RR_G2W_HO_INACTIVE:
      s = "GtoWHOinact";
      break;

    case RR_G2W_HO_AWAITING_L2_SUSPENSION:
      s = "GtoWHOL2susp";
      break;

    case RR_G2W_HO_GOING_INACTIVE:
      s = "GtoWHOgoingInact";
      break;

    case RR_G2W_HO_AWAITING_RRC_RESPONSE:
      s = "GtoWHOwaitRRCRsp";
      break;

    case RR_G2W_HO_GOING_ACTIVE:
      s = "GtoWHOgoingAct";
      break;

    case RR_G2W_HO_AWAITING_RRC_ABORT_COMPLETION:
      s = "GtoWHOwaitRRCAbort";
      break;

    case RR_G2W_HO_AWAITING_COMPLETION:
      s = "GtoWHOawaitCompl";
      break;

    default:
      s = "Unknown";
      break;
  }

  return s;
} /* rr_g2w_ho_state_name */

static uint16 rr_get_PN_Offset(wcdma_cell_T *input_cell, surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr)
{
  byte i;
  byte num_entries;

  ASSERT (wcdma_ded_meas_ptr != NULL);

  num_entries = wcdma_ded_meas_ptr->num_entries;

  for (i = 0; i < num_entries; ++i)
  {
    wcdma_cell_T *cell = &wcdma_ded_meas_ptr->meas[i].cell;
    if (
        (cell->uarfcn == input_cell->uarfcn) &&
        (cell->scrambling_code == input_cell->scrambling_code) &&
        (cell->diversity == input_cell->diversity)
       )
    {
      return wcdma_ded_meas_ptr->meas[i].PN_Offset;
    }
  }

  // use PN_OFFSET_UNKNOWN; when defined at RRC
  return 0xffff;
}


void rr_prepare_rr_rrc_interrat_measurement_data(
       rr_rrc_interrat_measurement_data_type *rr_rrc_interrat_meas_data,
       surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr,
       wcdma_neighbor_cell_list_T *wcdma_neighbor_cell_list_ptr
       )
{
  byte i;
  byte meas_data_index = 0;

  for (i = 0; i < MAX_UTRAN_CELLS + MAX_UTRAN_FREQ; ++i)
  {
    wcdma_neighbor_cell_list_T *cell_list_ptr = &wcdma_neighbor_cell_list_ptr[i];
    wcdma_cell_T *cell_ptr = &cell_list_ptr->cell;
    if (cell_list_ptr->valid_data)
    {
      /* Do not include frequency-only-s */
      if (cell_list_ptr->frequency_only == FREQ_SC)
      {
        if (meas_data_index < RR_RRC_MAX_INTERRAT_MEASUREMENTS)
        {
          rr_rrc_interrat_meas_data->wcdma_cell_measurements[meas_data_index].uarfcn =
            cell_ptr->uarfcn;
          rr_rrc_interrat_meas_data->wcdma_cell_measurements[meas_data_index].primary_scrambling_code =
            cell_ptr->scrambling_code >> 4; /* convert to primary SC */
          rr_rrc_interrat_meas_data->wcdma_cell_measurements[meas_data_index].diversity =
            (diversity_T) cell_ptr->diversity;
          rr_rrc_interrat_meas_data->wcdma_cell_measurements[meas_data_index].PN_Offset =
            rr_get_PN_Offset(cell_ptr, wcdma_ded_meas_ptr);

          meas_data_index++;

          rr_rrc_interrat_meas_data->number_of_cells = meas_data_index;
        }
        else
        {
          MSG_GERAN_ERROR_1("meas_data_index (%d) out of range", meas_data_index);
          break;
        }
      }
    }
  }
}


/**
 * Implements the GSM to WCDMA handover state machine
 *
 * @param rr_g2w_handover_event Event for the state machine
 *
 * @param new_message           Input message (only valid when the event is EV_INPUT_MESSAGE or RR_EV_G2W_HO_STARTED)
 *
 * @return New event for RR state machine. One of:
 *         EV_NO_EVENT          - No action should be taken
 *         RR_EV_G2W_HO_ABORTED - Handover aborted by MM, call should be taken down
 *         RR_EV_G2W_HO_FAIL    - Handover failed, a reconnect to the old channel should be attempted
 *         RR_EV_G2W_HO_SUCCESS - Handover successful
 */
rr_event_T rr_g2w_handover_control(
  rr_event_T g2w_handover_event,
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T  rr_event = EV_NO_EVENT;
  rr_event_T  new_g2w_handover_event = EV_NO_EVENT;
  byte        message_id  = 0;
  byte        message_set = 0;
  rr_g2w_handover_static_data_t *rr_g2w_handover_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_handover_data_ptr = rr_g2w_handover_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_handover_data_ptr);

  while (g2w_handover_event != EV_NO_EVENT)
  {
    if ( g2w_handover_event == EV_RESET_SOFTWARE )
    {
      /* reset state machine */
      rr_g2w_handover_discard_data(gas_id);
      rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_INACTIVE;

      /* don't process in state handlers */
      g2w_handover_event = EV_NO_EVENT;
    }
    else if ( g2w_handover_event == EV_INPUT_MESSAGE )
    {
      if (message_ptr != NULL)
      {
        /* set the message_set & message_id for all input messages */
        message_set = message_ptr->message_header.message_set;
        message_id =  message_ptr->message_header.message_id;

        if (message_set == MS_MM_RR)
        {
          if (message_id == (byte) RR_STOP_GSM_MODE_REQ)
          {
            if (rr_g2w_handover_data_ptr->g2w_ho_state != RR_G2W_HO_AWAITING_COMPLETION)
            {
              /* Cannot stop gsm mode until handover completes */
              rr_send_stop_gsm_mode_cnf(MM_AS_CONNECTED, gas_id);
              g2w_handover_event = EV_NO_EVENT;
            }
          }
          else if (message_id == (byte) RR_DATA_REQ)
          {
            if (rr_g2w_handover_data_ptr->g2w_ho_state != RR_G2W_HO_AWAITING_COMPLETION)
            {
              /* Send to L2 since we haven't finished the handover yet */
              rr_process_rr_data_req(&message_ptr->rr_data_req, gas_id);
              g2w_handover_event = EV_NO_EVENT;
            }
          }
        } /* (message_set == MS_MM_RR) */
      }
      else
      {
        ERR("Empty msg",0,0,0);
        g2w_handover_event = EV_NO_EVENT;

      }
    }

    /* an EV_NO_EVENT should not be processed by state handlers */
    if (g2w_handover_event == EV_NO_EVENT)
    {
      continue;
    }

    switch (rr_g2w_handover_data_ptr->g2w_ho_state)
    {
      case RR_G2W_HO_INACTIVE:
      {
        if (g2w_handover_event == RR_EV_G2W_HO_STARTED)
        {
          if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
          {
            MSG_GERAN_ERROR_0_G("NW command G2W HO when W segment not loaded");
            g2w_handover_event = EV_NO_EVENT;
            continue;
          }

          if (message_ptr != NULL)
          {
            uint8 handover_to_utran_command_ie_length = 0;

            BSIC_T  dummy_bsic;
            ARFCN_T local_arfcn;

            dummy_bsic.BS_colour_code   = INVALID_BSIC_CC;
            dummy_bsic.PLMN_colour_code = INVALID_PLMN_CC;

            local_arfcn.num             = INVALID_ARFCN;
            local_arfcn.band            = INVALID_BAND;

            rr_log_handover_start(
              RR_LOG_HO_TYPE_GSM_TO_WCDMA,
              local_arfcn,
              dummy_bsic,
              0xFF,
              gas_id
            );

            MSG_GERAN_HIGH_0_G("GTOW handover triggered");

            // Allocate and initialise the control/data structure
            rr_g2w_handover_activate_init(gas_id);

            /* The first 2 octets are the RR protocol Id and the L3 message id. The
            handover command message to be sent to RRC starts from octet 4.
            Octet 3 indicates the length of the handover command info starting
            from octet 4. Eg.
            byte msg_ptr[] =
            {
              0x06, 0x63, 0x10, 0x00, 0x00, 0x00, 0x38, 0xC0, 0x13, 0xB8, 0x8A,
              0x6A, 0x40, 0x08, 0x34, 0xCC, 0x29, 0xC1, 0x94
            };
            The total length of the message that has to be passed to RRC is 1 + the length
            specified in octet 3.
            */

            /* The maximum allowed value for the 'Length of Handover to UTRAN Command contents'
            octet is Maximum L3 message length minus 3:

            Octet   1: Protocol Discriminator / Skip Indicator
            Octet   2: Message Type
            Octet   3: Length of Handover to UTRAN Command contents
            Octet   4: Handover to UTRAN Command value part
            Octet   5: ...
            Octet 251: Handover to UTRAN Command value part

            So maximum value for length of value part is 248.
            */
            handover_to_utran_command_ie_length = MIN(message_ptr->dl_data_ind.layer3_message[2],
                                                      MAX_OCTETS_L3_MESSAGE - 3);

            rr_g2w_handover_data_ptr->data_ptr->g2w_ho_msg[0] = handover_to_utran_command_ie_length;

			//Replace memcpy with memscpy
            memscpy(&rr_g2w_handover_data_ptr->data_ptr->g2w_ho_msg[1],
                    sizeof(rr_g2w_handover_data_ptr->data_ptr->g2w_ho_msg)-1,
                   &message_ptr->dl_data_ind.layer3_message[3],
                   handover_to_utran_command_ie_length);


            /* SAPI3 link may be pending or established, but any action */
            /* required must be decided by the separate RR SAPI3 module */
            rr_release_sapi3(gas_id);

            /* suspend SAPI0 for this inter-RAT handover */
            rr_send_dl_suspend_req(RR_L1_GSM_WCDMA, gas_id);

            rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_AWAITING_L2_SUSPENSION;
            rr_event = EV_G2W_HANDOVER; /* handled in handle_procedures */
          }
          else
          {
            ERR("GTOW handover msg empty",0,0,0);
          }
        }
      }
      break;

      case RR_G2W_HO_AWAITING_L2_SUSPENSION:
      {
        if ((g2w_handover_event == EV_INPUT_MESSAGE) && (message_ptr!=NULL))
        {
          if ((message_set == MS_RR_L2) && (message_id == (byte) DL_SUSPEND_CNF))
          {
            /*********************************************************************************/
            /* Move to next state RR_G2W_HO_GOING_INACTIVE after initiating GSM deactivation */
            /* and also saving the old channel details. The old channel details is needed    */
            /* if we attempt to reconnect.                                                   */
            /*********************************************************************************/
            rr_set_handover_type(RR_L1_GSM_WCDMA, gas_id);

            /* Save the current channel details in case L2 resumption fails, */
            /* and we have to return to the old channel, and resume there    */
            rr_preserve_current_dedicated_config(gas_id);

            rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_GOING_INACTIVE;

            /* advise GL1 to exchange rather than release this client */
            geran_set_exchange_on_release(GERAN_EXCHANGE_GTOX, gas_id);

            new_g2w_handover_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          }
          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_RELEASE_IND || message_id == (byte) MDL_ERROR_IND))
          {
            /* Cannot suspend L2, so fail the HO */
            rr_log_handover_failure(RR_LOG_G2WHO_CANNOT_SUSPEND_GL2, gas_id);
            rr_g2w_handover_discard_data(gas_id);
            rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_INACTIVE;
            rr_event = EV_RADIO_LINK_FAILURE; /* handled in rr_control */
          }
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            rr_log_handover_failure(RR_LOG_HO_FAILURE_HANDOVER_ABORTED, gas_id);
            rr_g2w_handover_discard_data(gas_id);
            rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_INACTIVE;
            rr_event = RR_EV_G2W_HO_ABORTED; /* handled in rr_control */
          }
          else
          {
            ERR("Unex.Msg=%d,%d in State=%d",(int) message_set,(int) message_id, (int) rr_g2w_handover_data_ptr->g2w_ho_state);
          }
        }
        else
        {
          ERR("Unex.EV=%d in State=%d",(int) g2w_handover_event, (int) rr_g2w_handover_data_ptr->g2w_ho_state,0);
        }
      }
      break;

      case RR_G2W_HO_GOING_INACTIVE:
      {
        if ((g2w_handover_event== EV_INPUT_MESSAGE) && (message_ptr!=NULL))
        {
          if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            /*****************************************************/
            /* Need to abort call but L1 was sent a stop req     */
            /* must bring it back up again                       */
            /*****************************************************/
            rr_g2w_handover_data_ptr->data_ptr->abort_req_received = TRUE;
            new_g2w_handover_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
            rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_GOING_ACTIVE;
          }
          else
          {
            ERR("Unex.Msg=%d,%d in State=%d",(int) message_set,(int) message_id, (int) rr_g2w_handover_data_ptr->g2w_ho_state);
          }
        }
        else if (g2w_handover_event == EV_GSM_MODE_DEACTIVATED)
        {
          rr_rrc_interrat_measurement_data_type  rr_rrc_interrat_measurement_data;
          wcdma_ncell_list_data_t               *wcdma_ncell_list_data_ptr = rr_get_wcdma_ncell_list_data_ptr(gas_id);
          surr_wcdma_ded_measurements_T         *wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);
          boolean                                csfb_call_status = FALSE;          

          /* initialize the local data */
          memset((void *)&rr_rrc_interrat_measurement_data, 0, sizeof(rr_rrc_interrat_measurement_data_type));

#ifdef FEATURE_LTE
          csfb_call_status = rr_connection_release_get_g2l_blind_redir_after_csfb_flag(gas_id);
#endif /* FEATURE_LTE */

          /* L1 is deactivated; Set channel_configuration to indicate that */
          rr_ded_set_l1_chan_is_not_connected(gas_id);

          /* The following routine adds all the cells in neighbor cell list to the rr_rrc_interrat_measurement_data.
          If a valid PN Offset is present it will be added; otherwise an unknown PN offset value will be used */
          rr_prepare_rr_rrc_interrat_measurement_data(
            &rr_rrc_interrat_measurement_data,
            wcdma_ded_meas_ptr,
            wcdma_ncell_list_data_ptr->wcdma_neighbor_cell_list);

          /* Reinitialise the measurements so that if the HO fails we do not send
          old measurements to the network when we return to DEDICATED mode on GSM */
          rr_initialize_wcdma_ded_measurements(wcdma_ded_meas_ptr);

          /* Send the handover msg to RRC*/
          rr_send_interrat_handover_req(
            rr_get_ciphering_mode_command_received(gas_id),
            rr_g2w_handover_data_ptr->data_ptr->g2w_ho_msg,
            &rr_rrc_interrat_measurement_data,
            rr_rat_pri_list_info(gas_id),
            csfb_call_status,
            gas_id
          );

          rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_AWAITING_RRC_RESPONSE;
        }
        else
        {
          ERR("Unex.EV=%d in State=%d",(int) g2w_handover_event, (int) rr_g2w_handover_data_ptr->g2w_ho_state,0);
        }
      }
      break;

      case RR_G2W_HO_AWAITING_RRC_RESPONSE:
      {
        if ((g2w_handover_event== EV_INPUT_MESSAGE) && (message_ptr!=NULL))
        {
          if ((message_set == MS_RRC_RR) && (message_id == (byte) RRC_INTERRAT_HANDOVER_CNF))
          {
            if (message_ptr->rrc_interrat_handover_cnf.handover_status == RR_RRC_SUCCESS)
            {
              /* Handover was okay, start shipping off the buffered msgs */
              rr_send_buffered_nas_messages(gas_id);
              /* Initialize the rr_nas_message_buffer */
              rr_initialize_nas_message_buffer(gas_id);
              rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_AWAITING_COMPLETION;
            }
            else
            {
              rr_log_handover_failure(
                (rr_log_handover_failure_T)((int) message_ptr->rrc_interrat_handover_cnf.failure_cause +
                                            (int) RR_LOG_G2WHO_FAILURE_REASON_OFFSET + 1),
                gas_id
              );

              rr_g2w_handover_data_ptr->rrc_failure_cause = message_ptr->rrc_interrat_handover_cnf.failure_cause;

              /* Handover failed, bring gsm back up*/
              rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_GOING_ACTIVE;
              new_g2w_handover_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
            }
          }
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            /* Need to abort handover, wait for their response*/
            rr_g2w_handover_data_ptr->data_ptr->abort_req_received = TRUE;
            rr_send_interrat_handover_abort_req(gas_id);
            rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_AWAITING_RRC_ABORT_COMPLETION;
          }
          else
          {
            ERR("Unex.Msg=%d,%d in State=%d",(int) message_set,(int) message_id, (int) rr_g2w_handover_data_ptr->g2w_ho_state);
          }
        }
        else
        {
          ERR("Unex.EV=%d in State=%d",(int) g2w_handover_event, (int) rr_g2w_handover_data_ptr->g2w_ho_state,0);
        }
      }
      break;

      case RR_G2W_HO_GOING_ACTIVE:
      {
        if ((g2w_handover_event == EV_INPUT_MESSAGE) && (message_ptr!=NULL))
        {
          if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            /* Once we activated gsm we will process the abort*/
            rr_g2w_handover_data_ptr->data_ptr->abort_req_received = TRUE;
          }
          else
          {
            ERR("Unex.Msg=%d,%d in State=%d",(int) message_set,(int) message_id, (int) rr_g2w_handover_data_ptr->g2w_ho_state);
          }
        }
        else if (g2w_handover_event == EV_GSM_MODE_ACTIVATED)
        {
          /* Update L1 with the 3G Ncell list whenever the GSM L1 is re-activated and if the
          phone is automatic mode */
//          if (rr_PLMN_request_details.gsm_only == FALSE)
//          {
//            rr_send_mph_dedicated_wcdma_cell_update_list_req (NULL, &system_information_database.wcdma_ded_neighbor_list);
//          }
//      else
//      {
      /* if in GSM only mode flush the list at L1 */
//            rr_send_mph_dedicated_wcdma_cell_update_list_req (NULL,NULL);
//      }

          /******************************************************************************/
          /* Check if we have received an abort req. If we have reset the state machine */
          /* and return a RR_EV_G2W_HO_ABORTED event. Otherwise reset the state machine */
          /* and return a RR_EV_G2W_HO_FAIL event.                                      */
          /******************************************************************************/
          if (rr_g2w_handover_data_ptr->data_ptr->abort_req_received == TRUE)
          {
            rr_log_handover_failure(RR_LOG_HO_FAILURE_HANDOVER_ABORTED, gas_id);
            rr_event = RR_EV_G2W_HO_ABORTED; /* handled in rr_control */
          }
          else
          {
            /* event logged when RRC_INTERRAT_HANDOVER_CNF was received */
            rr_event = RR_EV_G2W_HO_FAIL; /* handled in rr_control */
          }

          rr_g2w_handover_discard_data(gas_id);
          rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_INACTIVE;
        }
        else
        {
          ERR("Unex.EV=%d in State=%d",(int) g2w_handover_event, (int) rr_g2w_handover_data_ptr->g2w_ho_state,0);
        }
      }
      break;

      case RR_G2W_HO_AWAITING_RRC_ABORT_COMPLETION:
      {
        if ((g2w_handover_event == EV_INPUT_MESSAGE) && (message_ptr!=NULL))
        {
          if ((message_set == MS_RRC_RR) && (message_id == (byte) RRC_INTERRAT_HANDOVER_ABORT_CNF))
          {
            /* Handover failed, bring gsm back up*/
            /* abort_req_received will be true; Failure event logged in
            the RR_G2W_HO_GOING_ACTIVE state */
            rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_GOING_ACTIVE;
            new_g2w_handover_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
          }
          else if ((message_set == MS_RRC_RR) && (message_id == (byte) RRC_INTERRAT_HANDOVER_CNF))
          {
            if (message_ptr->rrc_interrat_handover_cnf.handover_status == RR_RRC_FAILURE)
            {
              /******************************************************************/
              /* There may be a race condition between the handover failure and */
              /* abort req, if we receive a handover failure we can assume that */
              /* WCDMA has deactivated and we can bring up GSM                  */
              /******************************************************************/

              /* Handover failed, bring gsm back up*/
              rr_log_handover_failure(
                (rr_log_handover_failure_T)((int) message_ptr->rrc_interrat_handover_cnf.failure_cause +
                                            (int) RR_LOG_G2WHO_FAILURE_REASON_OFFSET + 1),
                gas_id
              );
              rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_GOING_ACTIVE;
              new_g2w_handover_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
            }
          }
          else
          {
            ERR("Unex.Msg=%d,%d in State=%d",(int) message_set,(int) message_id, (int) rr_g2w_handover_data_ptr->g2w_ho_state);
          }
        }
        else
        {
          ERR("Unex.EV=%d in State=%d",(int) g2w_handover_event, (int) rr_g2w_handover_data_ptr->g2w_ho_state,0);
        }
      }
      break;

      case RR_G2W_HO_AWAITING_COMPLETION:
      {
        if (g2w_handover_event == EV_INPUT_MESSAGE)
        {
          if (message_set == MS_MM_RR)
          {
            if (message_id == (byte) RR_DATA_REQ)
            {
              if (message_ptr != NULL)
              {
                rr_data_req_T *rr_data_req;
                SAPI_T         SAPI;
                byte           protocol_discriminator;

                rr_data_req = &message_ptr->rr_data_req;

                protocol_discriminator = rr_data_req->layer3_message[0] & 0x0F;

                if (protocol_discriminator == SMS_MESSAGES)
                {
                  SAPI = SAPI3;
                }
                else
                {
                  SAPI = SAPI0;
                }

                rr_send_nas_data_ind(
                  SAPI,
                  rr_data_req->l3_message_length,
                  rr_data_req->layer3_message,
                  gas_id
                );
              }
            }
            else if (message_id == (byte) RR_ABORT_REQ)
            {
              MSG_GERAN_MED_0_G("Got abort req in RR_G2W_HO_AWAITING_COMPLETION");
              rr_send_abort_cnf(MM_AS_INTERRAT_HANDOVER, gas_id);
            }
            else if (message_id == (byte) RR_STOP_GSM_MODE_REQ)
            {
              rr_log_handover_success(gas_id);
              rr_g2w_handover_discard_data(gas_id);

              rr_send_dl_release_req(
                LOCAL_END_RELEASE,
                SAPI0,
                rr_get_l2_channel_type(gas_id),
                gas_id
              );

              rr_g2w_handover_data_ptr->g2w_ho_state = RR_G2W_HO_INACTIVE;
              rr_event = RR_EV_G2W_HO_SUCCESS; /* handled in rr_control */
            }
            else
            {
              ERR("Unexpected message=(%d,%d)",(int) message_set, (int) message_id,0);
            }
          } /* (message_set == MS_MM_RR) */
          else
          {
            ERR("Unexpected message=(%d,%d)",(int) message_set, (int) message_id,0);
          }
        } /* (g2w_handover_event == EV_INPUT_MESSAGE) */
        else
        {
          ERR("Unex.EV=%d in State=%d",(int) g2w_handover_event, (int) rr_g2w_handover_data_ptr->g2w_ho_state,0);
        }
      }
      break;

      default:
      {
        ERR("Unknown state %d",rr_g2w_handover_data_ptr->g2w_ho_state,0,0);
        break;
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((g2w_handover_event != new_g2w_handover_event) && (new_g2w_handover_event != EV_NO_EVENT)) || (rr_g2w_handover_data_ptr->old_g2w_ho_state != rr_g2w_handover_data_ptr->g2w_ho_state))
    {
       rr_store_trace_msg_buf(
         RR_G2W_HO_SM,
         new_g2w_handover_event,
         (byte) rr_g2w_handover_data_ptr->old_g2w_ho_state,
         message_ptr,
         gas_id
       );
    }
#endif

    if (new_g2w_handover_event != EV_NO_EVENT)
    {
      g2w_handover_event = new_g2w_handover_event;
      new_g2w_handover_event = EV_NO_EVENT;
    }
    else
    {
      g2w_handover_event = EV_NO_EVENT;
    }
  }

  if (rr_g2w_handover_data_ptr->old_g2w_ho_state != rr_g2w_handover_data_ptr->g2w_ho_state)
  {
    RR_LOG_STATE_TRANSITION(
      "rr_g2w_handover_control",
      rr_g2w_handover_data_ptr->old_g2w_ho_state,
      rr_g2w_handover_data_ptr->g2w_ho_state,
      rr_g2w_ho_state_name,
      gas_id
    );

    rr_g2w_handover_data_ptr->old_g2w_ho_state = rr_g2w_handover_data_ptr->g2w_ho_state;
  }

  return rr_event;
}

/* Map RRC failure cause to the handover failure cause sent over the air */
byte rr_g2w_map_handover_failure_cause(
  const gas_id_t gas_id
)
{
  byte failure_cause;
  rr_g2w_handover_static_data_t *rr_g2w_handover_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_handover_data_ptr = rr_g2w_handover_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_handover_data_ptr);

  switch(rr_g2w_handover_data_ptr->rrc_failure_cause)
  {
    case RR_RRC_INVALID_STATE:
      failure_cause = RR_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE;
      break;

    case RR_RRC_INVALID_CONFIGURATION:
      failure_cause = RR_UTRAN_CONFIGURATION_UNKNOWN;
      break;

    case RR_RRC_PHYSICAL_CHANNEL_FAILURE:
      failure_cause = RR_LOWER_LAYER_FAILURE;
      break;

    case RR_RRC_FREQUENCY_NOT_IMPLEMENTED:
      failure_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
      break;

    case RR_RRC_WCDMA_PROTOCOL_ERROR:
    case RR_RRC_INVALID_CIPHER_ALGORITHM:
    default:
      failure_cause = RR_PROTOCOL_ERROR_UNSPECIFIED;
      break;
  } /* end switch */

  return(failure_cause);
}


/**
 * Initialisation function called when RR task is started.
 * Note: The primary purpose of this function is to initialise
 * pointers to control/data structures. This should also take into
 * account any second instance of data, as this is a one-off
 * initialisation.
 */
void rr_g2w_handover_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(rr_g2w_handover_data, 0, sizeof(rr_g2w_handover_data));

} /* end rr_g2w_handover_task_start_init() */


#else
/* no WCDMA support in this build */
int rr_g2w_handover_supported(void)
{
  return(0);
}
#endif /* FEATURE_WCDMA */
