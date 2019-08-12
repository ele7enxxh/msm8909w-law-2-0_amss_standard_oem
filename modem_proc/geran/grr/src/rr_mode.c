/*============================================================================
  FILE:         rr_mode.c

  OVERVIEW:     This module implements a state machine to manage the control
                (activation and deactivation) of GSM L1.  This state machine
                is robust enough to correctly handle multiple actions being
                requested before an existing action completes, i.e.
                deactivation may be requested even before activation has
                completed, and vice-versa.  This is achieved by allowing a
                'pending request' to be held, i.e. if a request is received
                while a previous request is being actioned, the new request
                will be saved and actioned on completion of the previous
                request.

                This state machine will also track the last stop reason that
                was sent to L1.  If a new request to deactivate L1 is received
                and the new stop reason differs from the old stop reason then
                L1 will be restarted and stopped with the new stop reason.

  DEPENDENCIES: None

                Copyright (c) 2002-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_mode.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_mode.h"
#include "ms.h"
#include "rr_general.h"
#include "rr_gprs_debug.h"
#include "rr_l1_send.h"
#include "rr_gprs_defs.h"
#include "rr_plmn_list.h"
#include "rr_multi_sim.h"
#include "rr_log.h"
#include "gsmdiag.h"
#ifdef FEATURE_IDLE_DRX_SCALING
#include "rr_idle_drx.h"
#endif /* FEATURE_IDLE_DRX_SCALING */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/
#define RR_DUAL_SIM_INACTIVATE_SECONDS (5)

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef enum
{
  RR_MODE_INACTIVE,
  RR_MODE_GOING_ACTIVE,
  RR_MODE_ACTIVE,
  RR_MODE_GOING_INACTIVE,
  RR_MODE_GOING_INACTIVE_RESET
} rr_mode_state_e;

typedef struct
{
  rr_mode_state_e state;
  rr_mode_state_e old_state;
  rr_event_T pending_request;
  /* Stores the last stop reason sent to GL1. */
  rr_l1_gsm_stop_reason_T stop_reason;
} rr_mode_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

#define _RR_MODE_DATA_INIT { \
                            RR_MODE_INACTIVE, \
                            RR_MODE_INACTIVE, \
                            EV_NO_EVENT, \
                            RR_L1_GSM_STOP_REASON_UNDEFINED \
                          }

static rr_mode_data_t rr_mode_data[NUM_GERAN_DATA_SPACES] =
                      { INITIAL_VALUE(_RR_MODE_DATA_INIT)};
#undef _RR_MODE_DATA_INIT

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_ps_access_data_t*
 */
rr_mode_data_t *rr_mode_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_mode_data[as_index];
}

static char *rr_mode_state_name(rr_mode_state_e state)
{
  switch (state)
  {
    case RR_MODE_INACTIVE:
    {
      return "INACTIVE";
    }

    case RR_MODE_GOING_ACTIVE:
    {
      return "GOING_ACTIVE";
    }

    case RR_MODE_ACTIVE:
    {
      return "ACTIVE";
    }

    case RR_MODE_GOING_INACTIVE:
    {
      return "GOING_INACTIVE";
    }

    case RR_MODE_GOING_INACTIVE_RESET:
    {
      return "GOING_INACTIVE_RESET";
    }

    default:
    {
      MSG_GERAN_ERROR_1("Unknown state: %d", state);
      return "?";
    }
  }
}

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION     rr_mode_gsm_active_or_starting

DESCRIPTION  Determines whether this mode control state machine is either
             active or going active at the time the function is called.

PARAMS       GAS ID

DEPENDENCIES rr_mode_data

RETURN VALUE TRUE/FALSE

===========================================================================*/
boolean rr_mode_gsm_active_or_starting(const gas_id_t gas_id)
{
  rr_mode_data_t *rr_mode_data_ptr = rr_mode_get_data_ptr(gas_id);

  if (rr_mode_data_ptr != NULL)
  {
    if ((rr_mode_data_ptr->state == RR_MODE_INACTIVE) ||
        (rr_mode_data_ptr->state == RR_MODE_GOING_INACTIVE))
    {
      return FALSE;
    }
  }

  return (TRUE);
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
static void rr_mode_going_inactive(
  const boolean normal_shutdown,
  const gas_id_t gas_id
)
{
  if ((rr_multi_sim_mode_enabled() == TRUE) &&
      (normal_shutdown               == TRUE))
  {
    /* start a guard timer */
    if (rr_timer_start(RR_DUAL_SIM_INACTIVATE_TIMER, GRR_TIMEOUT_1_SECOND * RR_DUAL_SIM_INACTIVATE_SECONDS, gas_id))
    {
      MSG_GERAN_HIGH_1_G("Started %d second timer for GOING_INACTIVE", RR_DUAL_SIM_INACTIVATE_SECONDS);
    }
  }

  return;
}

void rr_mode_deactivation_guard_expiry(const gas_id_t gas_id)
{
  /* a deactivation has not completed since this guard timer was started */
  MSG_GERAN_ERROR_1_G("GL1 has not sent MPH_STOP_GSM_MODE_CNF for %d seconds",
                       RR_DUAL_SIM_INACTIVATE_SECONDS);

  return;
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

rr_l1_gsm_stop_reason_T rr_mode_get_l1_stop_reason(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_mode_data_t *rr_mode_data_ptr = rr_mode_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_mode_data_ptr);

  return rr_mode_data_ptr->stop_reason;
}

void rr_mode_set_l1_stop_reason(rr_l1_gsm_stop_reason_T l1_stop_reason, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_mode_data_t *rr_mode_data_ptr = rr_mode_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_mode_data_ptr);

  rr_mode_data_ptr->stop_reason = l1_stop_reason;
    
  return;
}

rr_event_T rr_mode_control(
  rr_event_T event,
  rr_cmd_bdy_type *message,
  const gas_id_t gas_id
)
{
  rr_event_T output_event = EV_NO_EVENT;
  rr_event_T new_event = EV_NO_EVENT;
  byte message_id   = 0;
  byte message_set  = 0;
  rr_mode_data_t *rr_mode_data_ptr;

  // Obtain a pointer to the module data
  rr_mode_data_ptr = rr_mode_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_mode_data_ptr);

  event = rr_extract_message_header_details(
    event,
    message,
    &message_set,
    &message_id
  );

  /* EV_RESET_SOFTWARE, EV_GSM_MODE_DEACTIVATED and EV_PANIC_RESET are
  all handled in a stateless manner. */
  switch (event)
  {
    case EV_RESET_SOFTWARE:
    {
      rr_mode_data_ptr->state = RR_MODE_INACTIVE;
      rr_mode_data_ptr->old_state = RR_MODE_INACTIVE;
      rr_mode_data_ptr->pending_request = EV_NO_EVENT;
      event = EV_NO_EVENT;
      break;
    }

    /* This is sent in when RR receives MPH_RESET_IND from L1, which
    indicatest that L1 has detected an error condition and autonomously
    reset itself. */
    case EV_GSM_MODE_DEACTIVATED:
    {
      rr_timer_cancel(RR_L1_GUARD_TIMER, gas_id);
      rr_mode_data_ptr->state = RR_MODE_INACTIVE;
      rr_mode_data_ptr->pending_request = EV_NO_EVENT;
      output_event = EV_GSM_MODE_DEACTIVATED;
      event = EV_NO_EVENT;
      break;
    }

    case EV_PANIC_RESET:
    {
      rr_timer_cancel(RR_L1_GUARD_TIMER, gas_id);
      rr_mode_data_ptr->state = RR_MODE_GOING_INACTIVE_RESET;
      rr_mode_data_ptr->pending_request = EV_NO_EVENT;
      rr_send_mph_reset_req(gas_id);
      event = EV_NO_EVENT;
      break;
    }

    default:
    {
      ;
    }
  }

  while (event != EV_NO_EVENT)
  {
    switch (rr_mode_data_ptr->state)
    {
      case RR_MODE_INACTIVE:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            LOG_UNEXPECTED_MESSAGE(message, rr_mode_state_name(rr_mode_data_ptr->state));
            break;
          }

          case EV_ACTIVATE_GSM_MODE:
          {
            rr_send_mph_start_gsm_mode_req(FALSE, FALSE, gas_id);
            rr_mode_data_ptr->stop_reason = RR_L1_GSM_STOP_REASON_UNDEFINED;
            rr_mode_data_ptr->state = RR_MODE_GOING_ACTIVE;
            rr_timer_start(RR_L1_GUARD_TIMER, RR_L1_GUARD_TIMEOUT, gas_id);
            break;
          }

#if (defined FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
          case EV_ACTIVATE_GSM_MODE_FOR_X2G_RESEL:
          {
            rr_send_mph_start_gsm_mode_req(TRUE, FALSE, gas_id);
            rr_mode_data_ptr->stop_reason = RR_L1_GSM_STOP_REASON_UNDEFINED;
            rr_mode_data_ptr->state = RR_MODE_GOING_ACTIVE;
            break;
          }

          case EV_ACTIVATE_GSM_MODE_FOR_X2G_CCO:
          {
            rr_send_mph_start_gsm_mode_req(FALSE, TRUE, gas_id);
            rr_mode_data_ptr->stop_reason = RR_L1_GSM_STOP_REASON_UNDEFINED;
            rr_mode_data_ptr->state = RR_MODE_GOING_ACTIVE;
            break;
          }

          case EV_ACTIVATE_GSM_MODE_FOR_BPLMN:
          {
            uint32 timer_value = rr_plc_get_wtog_search_timer_value(gas_id);
            rr_send_mph_start_gsm_mode_for_bplmn_req(timer_value, gas_id);
            rr_mode_data_ptr->stop_reason = RR_L1_GSM_STOP_REASON_UNDEFINED;
            rr_mode_data_ptr->state = RR_MODE_GOING_ACTIVE;
            rr_timer_start(RR_L1_GUARD_TIMER, RR_L1_GUARD_TIMEOUT, gas_id);
            break;
          }
#endif /* FEATURE_WCDMA || FEATURE_LTE || defined (FEATURE_GSM_TDS)*/

          case EV_DEACTIVATE_GSM_MODE:
          {
            /* If L1 is already stopped for DEEP SLEEP then no action is
            necessary here, however if L1 is stopped for RAT CHANGE and
            the request is for DEEP SLEEP then send MPH_DEEP_SLEEP_IND
            to effect a correct deactivation of hardware. */
            if (rr_mode_data_ptr->stop_reason == RR_L1_DEEP_SLEEP)
            {
              MSG_GERAN_HIGH_0_G("GSM stopped for DEEP SLEEP, no action necessary");
            }
            else
            {
              if (rr_mode_data_ptr->stop_reason != rr_l1_gsm_stop_reason_get(gas_id))
              {
                MSG_GERAN_HIGH_2_G("GSM earlier stopped with reason %d, now stopped with reason %d",
                         rr_mode_data_ptr->stop_reason,
                         rr_l1_gsm_stop_reason_get(gas_id));

                /* Remember L1 is put in DEEP SLEEP */
                rr_mode_data_ptr->stop_reason = RR_L1_DEEP_SLEEP;

                /*  Put GL1 in LPM */
                rr_l1_send_mph_deep_sleep_ind(gas_id);
              }
              else
              {
                MSG_GERAN_HIGH_1_G("GSM stopped with stop reason %d, no action necessary",
                         rr_mode_data_ptr->stop_reason);
              }
            }
            output_event = EV_GSM_MODE_DEACTIVATED;
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_mode_state_name(rr_mode_data_ptr->state));
          }
        }
        break;
      }

      case RR_MODE_GOING_ACTIVE:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch ((rr_l1_message_id_enum_T) message_id)
                {
                  case MPH_START_GSM_MODE_CNF:
                    /* for "normal" GSM mode, start the system info timer */
                    (void)rr_timer_start(GRR_SYSTEM_INFO_TIMER, SYSTEM_INFO_CLOCK_TIMEOUT, gas_id);
                    /* fallthrough */
                  case MPH_START_GSM_MODE_FOR_BPLMN_CNF:
                  {
                    rr_mode_data_ptr->state = RR_MODE_ACTIVE;
                    rr_timer_cancel(RR_L1_GUARD_TIMER, gas_id);

                    if (rr_mode_data_ptr->pending_request != EV_NO_EVENT)
                    {
                      new_event = rr_mode_data_ptr->pending_request;
                      rr_mode_data_ptr->pending_request = EV_NO_EVENT;
                    }
                    else
                    {
                      output_event = EV_GSM_MODE_ACTIVATED;
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message, rr_mode_state_name(rr_mode_data_ptr->state));
                  }
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message, rr_mode_state_name(rr_mode_data_ptr->state));
              }
            }
            break;
          }

          case EV_ACTIVATE_GSM_MODE:
          case EV_DEACTIVATE_GSM_MODE:
          {
            rr_mode_data_ptr->pending_request = event;
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_mode_state_name(rr_mode_data_ptr->state));
          }
        }
        break;
      }

      case RR_MODE_ACTIVE:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            LOG_UNEXPECTED_MESSAGE(message, rr_mode_state_name(rr_mode_data_ptr->state));
            break;
          }

          case EV_ACTIVATE_GSM_MODE:
          {
            output_event = EV_GSM_MODE_ACTIVATED;
            break;
          }

          case EV_DEACTIVATE_GSM_MODE:
          {
#ifdef FEATURE_DUAL_SIM
            rr_mode_going_inactive(TRUE, gas_id);
#endif /* FEATURE_DUAL_SIM */
            rr_mode_data_ptr->stop_reason = rr_l1_gsm_stop_reason_get(gas_id);
            rr_send_mph_stop_gsm_mode_req(
              rr_mode_data_ptr->stop_reason,
              gas_id
            );
            rr_mode_data_ptr->state = RR_MODE_GOING_INACTIVE;
            rr_timer_start(RR_L1_GUARD_TIMER, RR_L1_GUARD_TIMEOUT, gas_id);
#ifdef FEATURE_IDLE_DRX_SCALING
            rr_idle_drx_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_mode_state_name(rr_mode_data_ptr->state));
          }
        }
      }
      break;

      case RR_MODE_GOING_INACTIVE:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_STOP_GSM_MODE_CNF:
                  {
                    rr_mode_data_ptr->state = RR_MODE_INACTIVE;

                    /* ensure the system info timer is cleared */
                    (void)rr_timer_cancel(GRR_SYSTEM_INFO_TIMER, gas_id);
                    (void)rr_timer_cancel(RR_L1_GUARD_TIMER, gas_id);

                    if (rr_mode_data_ptr->pending_request != EV_NO_EVENT)
                    {
                      new_event = rr_mode_data_ptr->pending_request;
                      rr_mode_data_ptr->pending_request = EV_NO_EVENT;
                    }
                    else
                    {
                      output_event = EV_GSM_MODE_DEACTIVATED;
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message, rr_mode_state_name(rr_mode_data_ptr->state));
                  }
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message, rr_mode_state_name(rr_mode_data_ptr->state));
              }
            }
            break;
          }

          case EV_ACTIVATE_GSM_MODE:
          case EV_DEACTIVATE_GSM_MODE:
          {
            rr_mode_data_ptr->pending_request = event;
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_mode_state_name(rr_mode_data_ptr->state));
          }
        }
        break;
      }

      case RR_MODE_GOING_INACTIVE_RESET:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_RESET_CNF:
                  {
                    rr_mode_data_ptr->state = RR_MODE_INACTIVE;
                    output_event = EV_GSM_MODE_DEACTIVATED;
                    break;
                  }

                  default: ;   // ignore any other messages from L1 (possible race conditions)
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message, rr_mode_state_name(rr_mode_data_ptr->state));
              }
            }
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_mode_state_name(rr_mode_data_ptr->state));
          }
        }
        break;
      }

      default:
      {
        LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_mode_state_name(rr_mode_data_ptr->state));
      }
      break;
    }

    if (new_event != EV_NO_EVENT)
    {
      event = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event = EV_NO_EVENT;
    }

    if (rr_mode_data_ptr->old_state != rr_mode_data_ptr->state)
    {
#ifndef PERLUTF       
      /* state has changed: update GDiag module if appropriate */
      if (RR_MODE_INACTIVE == rr_mode_data_ptr->state)
      { 
        /* this GERAN access stratum is now offline */
        gsmdiag_set_geran_state(GSM_STATE_OFFLINE, gas_id);
      }
      else if (RR_MODE_ACTIVE == rr_mode_data_ptr->state)
      {   
        /* this GERAN access stratum is now online */
        gsmdiag_set_geran_state(GSM_STATE_ONLINE, gas_id);
      }
#endif /*PERLUTF*/  

      rr_store_trace_msg_buf(
        RR_MODE_SM,
        event,
        (byte) rr_mode_data_ptr->state,
        message,
        gas_id
      );

      RR_LOG_STATE_TRANSITION(
        "rr_mode_control",
        rr_mode_data_ptr->old_state,
        rr_mode_data_ptr->state,
        rr_mode_state_name,
        gas_id
      );

      if (rr_mode_data_ptr->state == RR_MODE_GOING_ACTIVE)
      {
        // Enable log-on-demand queries
        rr_log_on_demand_init(gas_id);
      }
      else
      if ((rr_mode_data_ptr->state == RR_MODE_INACTIVE) &&
          (rr_mode_data_ptr->stop_reason == RR_L1_DEEP_SLEEP))
      {
        // Disable log-on-demand queries
        rr_log_on_demand_deinit(gas_id);
      }

      rr_mode_data_ptr->old_state = rr_mode_data_ptr->state;
    }
  }

  return output_event;
}

/* EOF */

