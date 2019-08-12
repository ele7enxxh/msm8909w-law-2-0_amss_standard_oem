#ifndef RR_MODE_H
#define RR_MODE_H

/*============================================================================
  @file rr_mode.h

  This module provides a state machine to manage the activation and
  deactivation of GSM L1.  Callers may request that GSM L1 is activated,
  or deactivated by passing in events EV_ACTIVATE_GSM_MODE or
  EV_DEACTIVATE_GSM_MODE respectively.

  Alternatively, GSM L1 may be activated for W2G BPLMN search by passing in
  event EV_ACTIVATE_GSM_MODE_FOR_BPLMN.

  When activation or deactivation is complete, this state machine will output
  either EV_GSM_MODE_ACTIVATED or EV_GSM_MODE_DEACTIVATED as appropriate.

  This modules also handles events related to panic reset functionality.  By
  sending in an event EV_PANIC_RESET this will trigger the resetting of GSM
  L1 via MPH_RESET_REQ.

  Sending in EV_GSM_MODE_DEACTIVATED will indicate to this state machine that
  GSM L1 has performed an internal reset and should be assumed to be in RESET
  state (equivalent of INACTIVE) where it will require activation before any
  further activity is possible.

                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_mode.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "rr_events.h"
#include "rr_task.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Returns the last stop reason that was sent to GL1.

  This function will return the last stop reason sent to GL1.  This may be
  used by other modules to check whether GL1 needs to be moved in to deep sleep mode 
  in response to an RR_STOP_GSM_MODE_REQ or RR_DEACT_REQ.

  @return the last stop reason sent to GL1.
*/
extern rr_l1_gsm_stop_reason_T rr_mode_get_l1_stop_reason(const gas_id_t gas_id);

/**
  @brief Sets the last stop reason that was sent to GL1.

  This function will set the last stop reason sent to GL1.
  
  @return the last stop reason sent to GL1.
*/
extern void rr_mode_set_l1_stop_reason(rr_l1_gsm_stop_reason_T l1_stop_reason, const gas_id_t gas_id);

/**
  @brief This function implements a state machine to control the activation
         and deactivation of GSM L1.

  GSM L1 may be activated, activated for W2G BPLMN search or deactivated
  by sending specific requests to this state machine, passed in as event
  parameters.  This state machine also handles various events related to panic
  reset functionality.  

  @param event  : EV_RESET_SOFTWARE - usually sent at task initialisation, the
                  state machine will reset it's internal state variables.
                  EV_ACTIVATE_GSM_MODE - request that the state machine send an
                  activation request to GSM L1.
                  EV_ACTIVATE_GSM_MODE_FOR_BPLMN - request that the state
                  machine send an activation request to GSM L1 for BPLMN mode.
                  This event also requires a message of type
                  rr_interrat_plmn_srch_req be included as the 'message'
                  parameter for this function call.
                  EV_DEACTIVATE_GSM_MODE - request that the state machine send
                  a deactivation request to GSM L1.
                  EV_INPUT_MESSAGE - indicate that 'message' parameter is set
                  and points to an RR input message.
                  EV_GSM_MODE_DEACTIVATED - indicates that GSM has autonomously
                  moved to 'RESET' state and will require reactivation before
                  further activity is possible.
                  EV_PANIC_RESET - indicates that a panic reset has occurred and
                  L1 must be reset before further activity is possible.

  @param message: This function may optionally be passed a pointer to an RR
                  message, as indicated by 'event' parameter being set to either
                  EV_INPUT_MESSAGE or EV_ACTIVATE_GSM_MODE_FOR_BPLMN.

  @return EV_NO_EVENT - returned when the state machine is waiting for GSM L1
          to complete some requested action.
          EV_GSM_MODE_ACTIVATED - GSM L1 has finished activating.
          EV_GSM_MODE_DEACTIVATED - GSM L1 has finished deactivating.

*/
extern rr_event_T rr_mode_control(rr_event_T, rr_cmd_bdy_type *, const gas_id_t gas_id);
extern boolean rr_mode_gsm_active_or_starting(const gas_id_t);

#ifdef FEATURE_DUAL_SIM
extern void rr_mode_deactivation_guard_expiry(const gas_id_t);
#endif /* FEATURE_DUAL_SIM */

#endif /* #ifndef RR_MODE_H */
