/*! \file rr_x2g_cgi.c

  This module implements a state machine that handles IRAT CGI requests.

                Copyright (c) 2011-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_x2g_cgi.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_LTE_TO_GSM_CGI

#include "comdef.h"
#include "amssassert.h"
#include "rr_seg_load.h"
#include "gprs_mem.h"
#include "rr_x2g_cgi.h"
#include "rr_mode.h"
#include "rr_gprs_debug.h"
#include "rr_select_bcch.h"
#include "rr_gprs_defs.h"
#include "rr_sys_info_pcell.h"
#include "rr_l1_send.h"
#include "rr_pscan.h"
#include "rr_log.h"
#include "rr_general.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#define RR_X2G_CGI_OVERHEAD 20 /* Offset that CGI timer is reduced by to
                               account for time taken to stop GL1. */

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef enum
{
  RR_X2G_CGI_STATE_NULL,
  RR_X2G_CGI_STATE_ACTIVATING_L1,
  RR_X2G_CGI_STATE_ACTIVATING_L1_ABORTED,
  RR_X2G_CGI_STATE_PSCAN,
  RR_X2G_CGI_STATE_PSCAN_ABORTED,
  RR_X2G_CGI_STATE_SELECTING,
  RR_X2G_CGI_STATE_ACQUIRING,
  RR_X2G_CGI_STATE_ACQUIRING_ABORTED,
  RR_X2G_CGI_STATE_DEACTIVATING_L1,
  RR_X2G_CGI_STATE_DEACTIVATING_L1_ABORTED,
  RR_X2G_CGI_STATE_MAX
} rr_x2g_cgi_state_e;

typedef struct
{
  rr_x2g_cgi_state_e      state;
  rr_x2g_cgi_state_e      old_state;

  /* List of ARFCNs to iterate over when searching for BSIC. */
  rr_x2g_cgi_arfcn_list_t arfcn_list;

  /* BSIC that we are searching for. */
  BSIC_T                  bsic;

  /* Index in pscan_results of the current ARFCN being selected / acquired. */
  uint32                  current_arfcn;

  /* */
  ARFCN_T                 cgi_arfcn;
  rr_x2g_cgi_info_t       info;


} rr_x2g_cgi_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

static rr_x2g_cgi_data_t * rr_x2g_cgi_data_ptr;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

static void rr_x2g_cgi_activate_l1(
  const gas_id_t gas_id
)
{
  rr_event_T event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
  if (event == EV_GSM_MODE_ACTIVATED)
  {
    rr_x2g_cgi_send_imsg(RR_X2G_CGI_IMSG_L1_ACTIVATED_IND, gas_id);
  }
}

static void rr_x2g_cgi_deactivate_l1(
  const gas_id_t gas_id
)
{
  rr_event_T event;

  /* TODO: It would be much better if the stop reason could be sent in to
  rr_mode_control(), rather than setting up a global that it later reads. */

  /* Set the global stop reason to RR_L1_RAT_CHANGE. */
  rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

  event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
  if (event == EV_GSM_MODE_DEACTIVATED)
  {
    rr_x2g_cgi_send_imsg(RR_X2G_CGI_IMSG_L1_DEACTIVATED_IND, gas_id);
  }
}

static const char * rr_x2g_cgi_state_name(rr_x2g_cgi_state_e state)
{
  switch (state)
  {
    case RR_X2G_CGI_STATE_NULL:
      return "NULL";

    case RR_X2G_CGI_STATE_ACTIVATING_L1:
      return "ACTIVATING_L1";

    case RR_X2G_CGI_STATE_ACTIVATING_L1_ABORTED:
      return "ACTIVATING_L1_ABORTED";

    case RR_X2G_CGI_STATE_PSCAN:
      return "PSCAN";

    case RR_X2G_CGI_STATE_PSCAN_ABORTED:
      return "PSCAN_ABORTED";

    case RR_X2G_CGI_STATE_SELECTING:
      return "SELECTING";

    case RR_X2G_CGI_STATE_ACQUIRING:
      return "ACQUIRING";

    case RR_X2G_CGI_STATE_ACQUIRING_ABORTED:
      return "ACQUIRING_ABORTED";

    case RR_X2G_CGI_STATE_DEACTIVATING_L1:
      return "DEACTIVATING_L1";

    case RR_X2G_CGI_STATE_DEACTIVATING_L1_ABORTED:
      return "DEACTIVATING_L1_ABORTED";

    default:
      MSG_GERAN_ERROR_1("Unknown state: %d", state);
      return "?";
  }
}

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

void rr_x2g_cgi_init(
  const gas_id_t gas_id
)
{
  rr_x2g_cgi_data_ptr = (rr_x2g_cgi_data_t *) GPRS_MEM_MALLOC(sizeof(rr_x2g_cgi_data_t));
  RR_NULL_CHECK_FATAL(rr_x2g_cgi_data_ptr);
  memset(rr_x2g_cgi_data_ptr, 0, sizeof(rr_x2g_cgi_data_t));
}

void rr_x2g_cgi_deinit(
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(rr_x2g_cgi_data_ptr);
  GPRS_MEM_FREE(rr_x2g_cgi_data_ptr);
  rr_x2g_cgi_data_ptr = NULL;
}

/**
  @brief Sends MPH_SELECT_SPECIFIC_BCCH_REQ to L1 for the first ARFCN in the
         list requested.

  @return TRUE if a request was sent to L1, FALSE if the list was empty and
          no request was sent.
*/
boolean rr_x2g_cgi_select_first_arfcn(
  const gas_id_t gas_id
)
{
  rr_pscan_results_db_T * rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);
  BSIC_T BSIC={INVALID_PLMN_CC,INVALID_BSIC_CC};

  RR_NULL_CHECK_RETURN_PARAM(rr_x2g_cgi_data_ptr, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(rr_pscan_results_db_ptr, FALSE);

  if (rr_pscan_results_db_ptr->pscan_results.num_frequencies > 0)
  {
    rr_x2g_cgi_data_ptr->current_arfcn = 0;
    rr_send_mph_select_specific_channel(
      rr_pscan_results_db_ptr->pscan_results.meas[0].arfcn,
      BSIC,
      gas_id
    );
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
  @brief Sends MPH_SELECT_SPECIFIC_BCCH_REQ to L1 for the next ARFCN in the
         list requested.  Increments the list counter.

  @return TRUE if a request was sent to L1, FALSE if the list was empty and
          no request was sent.
*/
boolean rr_x2g_cgi_select_next_arfcn(
  const gas_id_t gas_id
)
{
  rr_pscan_results_db_T * rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);
  BSIC_T BSIC={INVALID_PLMN_CC,INVALID_BSIC_CC};

  RR_NULL_CHECK_RETURN_PARAM(rr_x2g_cgi_data_ptr, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(rr_pscan_results_db_ptr, FALSE);

  ++rr_x2g_cgi_data_ptr->current_arfcn;

  if (rr_x2g_cgi_data_ptr->current_arfcn < rr_pscan_results_db_ptr->pscan_results.num_frequencies)
  {
    rr_send_mph_select_specific_channel(
      rr_pscan_results_db_ptr->pscan_results.meas[rr_x2g_cgi_data_ptr->current_arfcn].arfcn,
      BSIC,
      gas_id
    );
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

void rr_x2g_cgi_control(
  rr_cmd_bdy_type * message,
  const gas_id_t gas_id
)
{
  /* Validate module-global data */
  RR_NULL_CHECK_RETURN_VOID(rr_x2g_cgi_data_ptr);

  /* Validate input */
  RR_NULL_CHECK_RETURN_VOID(message);

  switch (rr_x2g_cgi_data_ptr->state)
  {
    case RR_X2G_CGI_STATE_NULL:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_CGI_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_CGI_IMSG_CGI_REQ:
                {
                  int time_available = (int) message->rr.x2g_cgi.imsg.cgi_req.time_available;

                  rr_x2g_cgi_data_ptr->arfcn_list   = message->rr.x2g_cgi.imsg.cgi_req.arfcn_list;
                  rr_x2g_cgi_data_ptr->bsic         = message->rr.x2g_cgi.imsg.cgi_req.bsic;
                  rr_x2g_cgi_data_ptr->info         = message->rr.x2g_cgi.imsg.cgi_req.info;

                  if (rr_x2g_cgi_data_ptr->info.mask != RR_X2G_CGI_INFO_MASK_NONE)
                  {
                    /* This is a follow-up request to acquire more information than
                    was previously acquired in a previous request.  This means that
                    the ARFCN to search is already identified and should be the
                    first in the list given to us. */
                    rr_x2g_cgi_data_ptr->cgi_arfcn = rr_x2g_cgi_data_ptr->arfcn_list.entries[0];
                  }

                  time_available -= RR_X2G_CGI_OVERHEAD;

                  if (time_available > 0)
                  {
                    rr_timer_start(RR_X2G_CGI_TIMER, time_available, gas_id);
                    rr_x2g_cgi_activate_l1(gas_id);

                    rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_ACTIVATING_L1;
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("Not enough time to obtain CGI info");

                    rr_x2g_cgi_send_omsg_cgi_cnf(
                      rr_x2g_cgi_data_ptr->cgi_arfcn,
                      rr_x2g_cgi_data_ptr->bsic,
                      &rr_x2g_cgi_data_ptr->info,
                      gas_id
                    );
                  }

                  break;
                } /* RR_X2G_CGI_IMSG_CGI_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "NULL");
                }
              }
              break;
            } /* RR_X2G_CGI_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "NULL");
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "NULL");
        }
      }
      break;
    } /* RR_X2G_CGI_STATE_NULL */

    case RR_X2G_CGI_STATE_ACTIVATING_L1:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_CGI_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_CGI_IMSG_L1_ACTIVATED_IND:
                {
                  if (rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_RXLEV)
                  {
                    BSIC_T BSIC={INVALID_PLMN_CC,INVALID_BSIC_CC};
                    /* We already have RXLEV for the cell, so just need to go
                    on to select it immediately.  Assume that the first cell
                    in the list is the correct one. */
                    rr_send_mph_select_specific_channel(
                      rr_x2g_cgi_data_ptr->arfcn_list.entries[0],
                      BSIC,
                      gas_id
                    );
                    rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_SELECTING;
                  }
                  else
                  {
                    rr_pscan_results_db_T * rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);

                    /* clear the candidate database and available plmn database since a new */
                    /* power scan is being started from scratch and all the old results would be invalid */
                    rr_plmn_db_clear(rr_get_available_plmn_db_ptr(gas_id), gas_id);
                    rr_pscan_clear_results(rr_pscan_results_db_ptr);
                    rr_candidate_db_clear(rr_get_candidate_cell_db_ptr(gas_id));
                    rr_invalidate_power_scan_results(gas_id);
                    (void)rr_timer_cancel(RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER, gas_id);

                    /* setup the pscan statemachine*/
                    rr_pscan_setup(gas_id);

                    /* log the start of a power scan */
                    rr_log_reselection_power_scan_started(gas_id);

                    rr_pscan_add_frequencies(
                      rr_x2g_cgi_data_ptr->arfcn_list.entries,
                      (uint16) MIN(rr_x2g_cgi_data_ptr->arfcn_list.count, RR_MAX_PSCAN_FREQ_SCAN_ARRAY_SIZE),
                      gas_id
                    );

                    rr_pscan_send_start_ind(RR_SEARCH_RESELECTION, rr_band_pref(gas_id), gas_id);

                    rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_PSCAN;
                  }
                  break;
                } /* RR_X2G_CGI_IMSG_L1_DEACTIVATED_IND */

                case RR_X2G_CGI_IMSG_ABORT_REQ:
                {
                  rr_timer_cancel(RR_X2G_CGI_TIMER, gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_ACTIVATING_L1_ABORTED;
                  break;
                } /* RR_X2G_CGI_IMSG_ABORT_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1");
                }
              }
              break;
            } /* RR_X2G_CGI_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1");
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_TIMER:
        {
          switch (message->message_header.message_id)
          {
            case MID_TIMER_EXPIRY:
            {
              switch (message->mid_timer_expiry.timer_id)
              {
                case RR_X2G_CGI_TIMER:
                {
                  rr_x2g_cgi_deactivate_l1(gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1;
                  break;
                } /* RR_X2G_CGI_TIMER */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1");
                }
              }
              break;
            } /* MID_TIMER_EXPIRY */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1");
            }
          }
          break;
        } /* MS_TIMER */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1");
        }
      }
      break;
    } /* RR_X2G_CGI_STATE_ACTIVATING_L1 */

    case RR_X2G_CGI_STATE_ACTIVATING_L1_ABORTED:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_CGI_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_CGI_IMSG_L1_ACTIVATED_IND:
                {
                  rr_x2g_cgi_deactivate_l1(gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1_ABORTED;
                  break;
                } /* RR_X2G_CGI_IMSG_L1_DEACTIVATED_IND */

                case RR_X2G_CGI_IMSG_ABORT_REQ:
                {
                  break;
                } /* RR_X2G_CGI_IMSG_ABORT_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1_ABORTED");
                }
              }
              break;
            } /* RR_X2G_CGI_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1_ABORTED");
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1_ABORTED");
        }
      }
      break;
    } /* RR_X2G_CGI_STATE_ACTIVATING_L1_ABORTED */

    case RR_X2G_CGI_STATE_PSCAN:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_PSCAN_OMSG:
            {
              switch (message->rr.header.imh.message_id)
              {
                case RR_PSCAN_OMSG_FINISHED_IND:
                {
                  rr_pscan_results_db_T * rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);

                  MSG_GERAN_MED_1_G("rr_pscan_results_db_ptr->pscan_results.num_frequencies: %d",
                          rr_pscan_results_db_ptr->pscan_results.num_frequencies);

                  if (rr_x2g_cgi_select_first_arfcn(gas_id))
                  {
                    rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_SELECTING;
                  }
                  else
                  {
                    rr_timer_cancel(RR_X2G_CGI_TIMER, gas_id);
                    rr_x2g_cgi_deactivate_l1(gas_id);
                    rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1;
                  }
                  break;
                } /* RR_PSCAN_OMSG_FINISHED_IND */

                case RR_PSCAN_OMSG_ABORT_CNF:
                {
                  rr_x2g_cgi_deactivate_l1(gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1;
                  break;
                } /* RR_PSCAN_OMSG_ABORT_CNF */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "PSCAN");
                }
              }
              break;
            } /* RR_PSCAN_OMSG */

            case RR_X2G_CGI_IMSG:
            {
              switch (message->rr.header.imh.message_id)
              {
                case RR_X2G_CGI_IMSG_ABORT_REQ:
                {
                  rr_timer_cancel(RR_X2G_CGI_TIMER, gas_id);
                  rr_pscan_send_abort_req(gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_PSCAN_ABORTED;
                  break;
                } /* RR_X2G_CGI_IMSG_ABORT_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "PSCAN");
                }
              }
              break;
            } /* RR_X2G_CGI_IMSG */

            default:
            {
              rr_pscan_control(EV_INPUT_MESSAGE, message, gas_id);
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_TIMER:
        {
          switch (message->message_header.message_id)
          {
            case MID_TIMER_EXPIRY:
            {
              switch (message->mid_timer_expiry.timer_id)
              {
                case RR_X2G_CGI_TIMER:
                {
                  rr_pscan_send_abort_req(gas_id);
                  /* Remain in this state - rr_pscan_control() will return an
                  RR_PSCAN_OMSG_ABORT_CNF rather than RR_PSCAN_OMSG_FINISHED_IND
                  which is sufficient to allow this state handler to determine
                  which state to move to next. */
                  break;
                } /* RR_X2G_CGI_TIMER */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1");
                }
              }
              break;
            } /* MID_TIMER_EXPIRY */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1");
            }
          }
          break;
        } /* MS_TIMER */

        default:
        {
          rr_pscan_control(EV_INPUT_MESSAGE, message, gas_id);
        }
      }
      break;
    } /* RR_X2G_CGI_STATE_PSCAN */

    case RR_X2G_CGI_STATE_PSCAN_ABORTED:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_PSCAN_OMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_PSCAN_OMSG_ABORT_CNF:
                {
                  rr_x2g_cgi_deactivate_l1(gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1_ABORTED;
                  break;
                } /* RR_PSCAN_OMSG_FINISHED_IND */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "PSCAN_ABORTED");
                }
              }
              break;
            } /* RR_PSCAN_OMSG */

            case RR_X2G_CGI_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_CGI_IMSG_ABORT_REQ:
                {
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_PSCAN_ABORTED;
                  break;
                } /* RR_X2G_CGI_IMSG_ABORT_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "PSCAN_ABORTED");
                }
              }
              break;
            } /* RR_X2G_CGI_IMSG */

            default:
            {
              rr_pscan_control(EV_INPUT_MESSAGE, message, gas_id);
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          rr_pscan_control(EV_INPUT_MESSAGE, message, gas_id);
        }
      }
      break;
    } /* RR_X2G_CGI_STATE_PSCAN_ABORTED */

    case RR_X2G_CGI_STATE_SELECTING:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_CGI_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_CGI_IMSG_ABORT_REQ:
                {
                  rr_timer_cancel(RR_X2G_CGI_TIMER, gas_id);
                  rr_x2g_cgi_deactivate_l1(gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1_ABORTED;
                  break;
                } /* RR_X2G_CGI_IMSG_ABORT_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "SELECTING");
                }
              }
              break;
            } /* RR_X2G_CGI_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "SELECTING");
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_RR_L1:
        {
          switch (message->message_header.message_id)
          {
            case MPH_SELECT_SPECIFIC_BCCH_CNF:
            {
              if (message->mph.select_specific_bcch_cnf.BCCH_found)
              {
                /* If the BSIC of the channel we have just decoded matches the
                one requested then we should start acquiring the CGI info,
                otherwise we should move on to the next ARFCN. */
                uint8 PLMN_colour_code = DECODE_NCC((&message->mph.select_specific_bcch_cnf));
                uint8 BS_colour_code = DECODE_BCC((&message->mph.select_specific_bcch_cnf));

                if (rr_x2g_cgi_data_ptr->bsic.PLMN_colour_code == PLMN_colour_code &&
                    rr_x2g_cgi_data_ptr->bsic.BS_colour_code == BS_colour_code)
                {
                  uint16 notify_on_si_mask = 0;
                  uint16 wait_for_si_mask = 0;

                  if ((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_RXLEV) == 0)
                  {
                    rr_pscan_results_db_T * rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);
                    rr_l1_freq_power_T * cell;

                    cell = &rr_pscan_results_db_ptr->pscan_results.meas[rr_x2g_cgi_data_ptr->current_arfcn];

                    /* As we have found the requested cell, we can at least return
                    the Cell ID & RXLEV to the requestor, even if we don't get time to read
                    the CGI data. */
                    rr_x2g_cgi_data_ptr->cgi_arfcn = cell->arfcn;
                    rr_x2g_cgi_data_ptr->info.mask |= RR_X2G_CGI_INFO_MASK_RXLEV;
                    rr_x2g_cgi_data_ptr->info.rxlev = cell->RXLEV_average;
                  }
                  else
                  {
                    rr_x2g_cgi_data_ptr->cgi_arfcn = rr_x2g_cgi_data_ptr->arfcn_list.entries[0];
                  }

                  if ((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_LAI) == 0)
                  {
                    MSG_GERAN_MED_0_G("Requesting SI3");
                    notify_on_si_mask |= MASK_SI_3;
                    wait_for_si_mask |= MASK_SI_3;
                  }

                  if ((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_RAC) == 0)
                  {
                    MSG_GERAN_MED_0_G("Requesting SI13");
                    wait_for_si_mask |= MASK_SI_13;
                  }

                  rr_select_bcch_send_imsg_acquire_si_req(
                    rr_x2g_cgi_data_ptr->cgi_arfcn,   // arfcn
                    0xff,                             // rxlev_average
                    rr_x2g_cgi_data_ptr->bsic,        // bsic
                    NULL,                             // cell_suitable_cb_fn
                    FALSE,                            // early_camp_required
                    notify_on_si_mask,                // notify_on_si_mask
                    wait_for_si_mask,                 // wait_for_si_mask
                    NULL,                             // init_params_ptr
                    0,                                // max_allowed_time_ms
                    gas_id                            // gas_id
                  );

                  /**
                   * Note: max_allowed_time_ms=0 means that RR-SELECT-BCCH will not start/stop 
                   * RR_GSM_SYS_INFO_TIMER to guard this acquisition. 
                   * The timer RR_X2G_CGI_TIMER is use to guard this procedure. 
                   */

                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_ACQUIRING;
                }
                else
                {
                  MSG_GERAN_HIGH_0_G("BSIC does not match requested");
                  if (rr_x2g_cgi_select_next_arfcn(gas_id))
                  {
                    ; /* remain in this state */
                  }
                  else
                  {
                    rr_timer_cancel(RR_X2G_CGI_TIMER, gas_id);
                    rr_x2g_cgi_deactivate_l1(gas_id);
                    rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1;
                  }
                }
              }
              else
              {
                if (rr_x2g_cgi_select_next_arfcn(gas_id))
                {
                  ; /* remain in this state */
                }
                else
                {
                  rr_timer_cancel(RR_X2G_CGI_TIMER, gas_id);
                  rr_x2g_cgi_deactivate_l1(gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1;
                }
              }
              break;
            } /* MPH_SELECT_SPECIFIC_BCCH_CNF */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "SELECTING");
            }
          }
          break;
        } /* MS_RR_L1 */

        case MS_TIMER:
        {
          switch (message->message_header.message_id)
          {
            case MID_TIMER_EXPIRY:
            {
              switch (message->mid_timer_expiry.timer_id)
              {
                case RR_X2G_CGI_TIMER:
                {
                  rr_x2g_cgi_deactivate_l1(gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1;
                  break;
                } /* RR_X2G_CGI_TIMER */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "SELECTING");
                }
              }
              break;
            } /* MID_TIMER_EXPIRY */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "SELECTING");
            }
          }
          break;
        } /* MS_TIMER */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "SELECTING");
        }
      }
      break;
    } /* RR_X2G_CGI_STATE_SELECTING */

    case RR_X2G_CGI_STATE_ACQUIRING:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_SELECT_BCCH_OMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_SELECT_BCCH_OMSG_SELECT_CNF:
                {
                  gprs_scell_info_t * pcell = rr_gprs_get_pcell_info(gas_id);

                  rr_timer_cancel(RR_X2G_CGI_TIMER, gas_id);

                  if((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_CELL_ID) && 
                     (rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_LAI))
                  {
                    /*Will hit this case, if SI3 was received in a previous iteration of CGI_REQ/RSP and SI13 is received now. */
                    rr_csi_copy_pend_store_to_cache(pcell->gsm.BCCH_ARFCN, 
                                                  &(rr_x2g_cgi_data_ptr->info.lai),
                                                  &(rr_x2g_cgi_data_ptr->info.cell_id),
                                                    gas_id);
                  }
                  else if(pcell->gsm.cell_identity_valid_flag)
                  {
                    /* This is the case where SI13 is received first followed by SI3 */
                    rr_csi_copy_pend_store_to_cache(pcell->gsm.BCCH_ARFCN, 
                                                  &(pcell->gsm.location_area_identification),
                                                  &(pcell->gsm.cell_identity),
                                                    gas_id);
                  }

                  rr_csi_deactivate_pend_store(gas_id);

                  /* SI3 is now guaranteed to have been received, however we
                  need to check explicitly for SI13 as it may not be broadcast
                  by the cell. */
                  if ((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_CELL_ID) == 0)
                  {
                    rr_x2g_cgi_data_ptr->info.mask |= RR_X2G_CGI_INFO_MASK_CELL_ID;
                    rr_x2g_cgi_data_ptr->info.cell_id = pcell->gsm.cell_identity;
                  }

                  if ((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_LAI) == 0)
                  {
                    rr_x2g_cgi_data_ptr->info.mask |= RR_X2G_CGI_INFO_MASK_LAI;
                    rr_x2g_cgi_data_ptr->info.lai = pcell->gsm.location_area_identification;
                  }

                  /* SI13 is expected, so only indicate it as present if it has
                  been received. */
                  if (pcell->bcch_sys_info.si13_data_valid_flag)
                  {
                    if ((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_RAC) == 0)
                    {
                      rr_x2g_cgi_data_ptr->info.mask |= RR_X2G_CGI_INFO_MASK_RAC;
                      rr_x2g_cgi_data_ptr->info.rac_present = TRUE;
                      rr_x2g_cgi_data_ptr->info.rac = pcell->bcch_sys_info.routing_area_code;
                    }
                  }
                  else
                  {
                    if (pcell->gsm.gprs_indicator)
                    {
                    }
                    else
                    {
                      if ((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_RAC) == 0)
                      {
                        /* TODO: Check this. */
                        /* SI13 is not broadcast by the cell, so flag RAC as known, but
                        not present. */
                        rr_x2g_cgi_data_ptr->info.mask |= RR_X2G_CGI_INFO_MASK_RAC;
                        rr_x2g_cgi_data_ptr->info.rac_present = FALSE;
                      }
                    }
                  }

                  rr_x2g_cgi_deactivate_l1(gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1;
                  break;
                } /* RR_SELECT_BCCH_OMSG_SELECT_CNF */

                /* SI_NOTIFY_IND is sent when SI3 is received. */
                case RR_SELECT_BCCH_OMSG_SI_NOTIFY_IND:
                {
                  gprs_scell_info_t * pcell = rr_gprs_get_pcell_info(gas_id);

                  /* SI3 is now guaranteed to have been received, however we
                  need to check explicitly for SI13 as it may not be broadcast
                  by the cell. */
                  if ((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_CELL_ID) == 0)
                  {
                    MSG_GERAN_MED_0_G("X2G CGI: Cell ID received");
                    rr_x2g_cgi_data_ptr->info.mask |= RR_X2G_CGI_INFO_MASK_CELL_ID;
                    rr_x2g_cgi_data_ptr->info.cell_id = pcell->gsm.cell_identity;
                  }

                  if ((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_LAI) == 0)
                  {
                    MSG_GERAN_MED_0_G("X2G CGI: LAI received");
                    rr_x2g_cgi_data_ptr->info.mask |= RR_X2G_CGI_INFO_MASK_LAI;
                    rr_x2g_cgi_data_ptr->info.lai = pcell->gsm.location_area_identification;
                  }

                  if(pcell->gsm.cell_identity_valid_flag)
                  {
                    MSG_GERAN_MED_3_G("X2G CGI: SI13 not received yet, retrieve SIs from cache: si13_data_valid_flag=%d, info.mask=%d",
                                                             pcell->bcch_sys_info.si13_data_valid_flag,rr_x2g_cgi_data_ptr->info.mask,0);

                    rr_csi_retrieve_all_SIs( pcell->gsm.BCCH_ARFCN,
                                           &(pcell->gsm.location_area_identification),
                                           &(pcell->gsm.cell_identity),
                                             CSI_RETRIEVE_ALL_SIS,
                                             gas_id);
                  }
                  break;
                } /* RR_SELECT_BCCH_OMSG_SI_NOTIFY_IND */

                case RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND:
                {
                  gprs_scell_info_t * pcell = rr_gprs_get_pcell_info(gas_id);

                  rr_timer_cancel(RR_X2G_CGI_TIMER, gas_id);

                  if((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_CELL_ID) && 
                     (rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_LAI))
                  {
                    rr_csi_copy_pend_store_to_cache(pcell->gsm.BCCH_ARFCN, 
                                                  &(rr_x2g_cgi_data_ptr->info.lai),
                                                  &(rr_x2g_cgi_data_ptr->info.cell_id),
                                                    gas_id);
                  }

                  rr_csi_deactivate_pend_store(gas_id);

                  /* SI13 is expected, so only indicate it as present if it has
                  been received. */
                  if (pcell->bcch_sys_info.si13_data_valid_flag)
                  {
                    if ((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_RAC) == 0)
                    {
                      rr_x2g_cgi_data_ptr->info.mask |= RR_X2G_CGI_INFO_MASK_RAC;
                      rr_x2g_cgi_data_ptr->info.rac_present = TRUE;
                      rr_x2g_cgi_data_ptr->info.rac = pcell->bcch_sys_info.routing_area_code;
                    }
                  }

                  rr_x2g_cgi_deactivate_l1(gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1;
                  break;
                } /* RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ACQUIRING");
                }
              }
              break;
            } /* RR_SELECT_BCCH_OMSG */

            case RR_X2G_CGI_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_CGI_IMSG_ABORT_REQ:
                {
                  rr_timer_cancel(RR_X2G_CGI_TIMER, gas_id);
                  rr_select_bcch_send_imsg_abort_req(gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_ACQUIRING_ABORTED;
                  break;
                } /* RR_X2G_CGI_IMSG_ABORT_REQ */

                default:
                {
                  rr_select_bcch_control(message, gas_id);
                }
              }
              break;
            } /* RR_X2G_CGI_IMSG */

            default:
            {
              rr_select_bcch_control(message, gas_id);
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_TIMER:
        {
          switch (message->message_header.message_id)
          {
            case MID_TIMER_EXPIRY:
            {
              switch (message->mid_timer_expiry.timer_id)
              {
                case RR_X2G_CGI_TIMER:
                {
                  rr_select_bcch_send_imsg_abort_req(gas_id);
                  break;
                } /* RR_X2G_CGI_TIMER */

                default:
                {
                  rr_select_bcch_control(message, gas_id);
                }
              }
              break;
            } /* MID_TIMER_EXPIRY */

            default:
            {
              rr_select_bcch_control(message, gas_id);
            }
          }
          break;
        } /* MS_TIMER */

        default:
        {
          rr_select_bcch_control(message,gas_id);
        }
      }
      break;
    } /* RR_X2G_CGI_STATE_ACQUIRING */

    case RR_X2G_CGI_STATE_ACQUIRING_ABORTED:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_SELECT_BCCH_OMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND:
                {
                  gprs_scell_info_t * pcell = rr_gprs_get_pcell_info(gas_id);

                  rr_csi_deactivate_pend_store(gas_id);

                  /* SI13 is expected, so only indicate it as present if it has
                  been received. */
                  if (pcell->bcch_sys_info.si13_data_valid_flag)
                  {
                    if ((rr_x2g_cgi_data_ptr->info.mask & RR_X2G_CGI_INFO_MASK_RAC) == 0)
                    {
                      rr_x2g_cgi_data_ptr->info.mask |= RR_X2G_CGI_INFO_MASK_RAC;
                      rr_x2g_cgi_data_ptr->info.rac_present = TRUE;
                      rr_x2g_cgi_data_ptr->info.rac = pcell->bcch_sys_info.routing_area_code;
                    }
                  }

                  rr_x2g_cgi_deactivate_l1(gas_id);
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1_ABORTED;
                  break;
                } /* RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ACQUIRING_ABORTED");
                }
              }
              break;
            } /* RR_SELECT_BCCH_OMSG */

            default:
            {
              rr_select_bcch_control(message, gas_id);
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          rr_select_bcch_control(message, gas_id);
        }
      }
      break;
    } /* RR_X2G_CGI_STATE_ACQUIRING_ABORTED */

    case RR_X2G_CGI_STATE_DEACTIVATING_L1:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_CGI_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_CGI_IMSG_L1_DEACTIVATED_IND:
                {
                   rr_x2g_cgi_send_omsg_cgi_cnf(
                    rr_x2g_cgi_data_ptr->cgi_arfcn,
                    rr_x2g_cgi_data_ptr->bsic,
                    &rr_x2g_cgi_data_ptr->info,
                    gas_id
                  );
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_NULL;
                  break;
                } /* RR_X2G_CGI_IMSG_L1_DEACTIVATED_IND */

                case RR_X2G_CGI_IMSG_ABORT_REQ:
                {
                  /* L1 is already deactivating - just switch to aborted state
                  so the correct response is sent back. */
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_DEACTIVATING_L1_ABORTED;
                  break;
                } /* RR_X2G_CGI_IMSG_ABORT_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING_L1");
                }
              }
              break;
            } /* RR_X2G_CGI_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING_L1");
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING_L1");
        }
      }
      break;
    } /* RR_X2G_CGI_STATE_DEACTIVATING_L1 */

    case RR_X2G_CGI_STATE_DEACTIVATING_L1_ABORTED:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_CGI_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_CGI_IMSG_L1_DEACTIVATED_IND:
                {
                  rr_x2g_cgi_send_omsg_abort_cnf(
                    rr_x2g_cgi_data_ptr->cgi_arfcn,
                    rr_x2g_cgi_data_ptr->bsic,
                    &rr_x2g_cgi_data_ptr->info,
                    gas_id
                  );
                  rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_NULL;
                  break;
                } /* RR_X2G_CGI_IMSG_L1_DEACTIVATED_IND */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING_L1_ABORTED");
                }
              }
              break;
            } /* RR_X2G_CGI_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING_L1_ABORTED");
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING_L1_ABORTED");
        }
      }
      break;
    } /* RR_X2G_CGI_STATE_ABORTED_DEACTIVATING_L1 */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected state %d, resetting to NULL", rr_x2g_cgi_data_ptr->state);
      rr_x2g_cgi_data_ptr->state = RR_X2G_CGI_STATE_NULL;
    }
  }

#ifdef DEBUG_RR_TRACE_MSG
  rr_store_trace_msg_buf(
    RR_X2G_CGI_SM,
    EV_INPUT_MESSAGE,
    (byte) rr_x2g_cgi_data_ptr->state,
    message,
    gas_id
  );
#endif

  if (rr_x2g_cgi_data_ptr->state != rr_x2g_cgi_data_ptr->old_state)
  {
    RR_LOG_STATE_TRANSITION(
      "rr_x2g_cgi_control",
      rr_x2g_cgi_data_ptr->old_state,
      rr_x2g_cgi_data_ptr->state,
      rr_x2g_cgi_state_name, 
      gas_id
    );
    rr_x2g_cgi_data_ptr->old_state = rr_x2g_cgi_data_ptr->state;
  }
}

#endif /* FEATURE_LTE_TO_GSM_CGI */

/* EOF */

