#ifndef RR_GRR_H
#define RR_GRR_H

/*============================================================================
  @file rr_grr.h

  Header file for the rr_grr51_grr52_control.c module.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_grr.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
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
  @brief This is the main GRR state machine that handles all idle-mode 
         procedures.

  @param grr_control_event: An event passed in to this state machine.
  @param new_message: Optional message either from RR or another task.
  @returns A new event to the caller.
*/
extern rr_event_T grr_control(
  rr_event_T grr_control_event,
  rr_cmd_bdy_type *new_message,
  const gas_id_t gas_id
);


/**
  @brief Determine whether GRR is active or not.

  GRR is considered to be active if the main grr_control() state machine is
  in any state other than GRR_NULL.

  @return TRUE if GRR is active, FALSE otherwise.
*/
extern boolean rr_gprs_grr_is_active(const gas_id_t gas_id);


/**
  @brief Determine whether GRR is idle-camped or not.

  GRR is considered to be idle-camped if the main grr_control()
  state machine is in GRR_CAMPED state.

  @return TRUE if GRR is idle-camped, FALSE otherwise.
*/
extern boolean grr_is_idle_camped(const gas_id_t gas_id);

/**
  @brief Determine whether GRR is in camped state or not.

  GRR is considered to be in camped state if the main grr_control()
  state machine is in GRR_CAMPED or GRR_CAMPED_* states.

  @return TRUE if GRR is in camped state, FALSE otherwise.
*/
extern boolean grr_in_camped_state(const gas_id_t gas_id);


/**
  @brief Determine whether a TBF establishment is in progress.

  @return TRUE if a TBF establishment is in progress, FALSE
          otherwise.
*/
extern boolean grr_tbf_est_in_progress(const gas_id_t gas_id);


/**
  @brief Determine whether a CS connection establishment is in progress.

  @return TRUE if a CS connection establishment is in progress, FALSE otherwise.
*/
extern boolean grr_cs_conn_est_in_progress(const gas_id_t gas_id);


/**
  @brief Determine whether a cell reselection is in progress.

  @return TRUE if a cell reselection is in progress, FALSE otherwise.
*/
extern boolean grr_reselection_in_progress(const gas_id_t gas_id);


/**
  @brief Verifies whether RR currently has a saved message from L2 that matches
         the specified message_id.

  @param message_id The message ID to check for.
  @returns TRUE if RR does have a saved message from L2 whose message ID matches
           that specified, FALSE otherwise.
*/
extern boolean grr_verify_saved_l2_message(
  byte message_id,
  const gas_id_t gas_id
);


/**
  @brief Verifies whether RR currently has a saved message from MM that matches
         the specified message_id.

  @param message_id The message ID to check for.
  @returns TRUE if RR does have a saved message from MM whose message ID matches
           that specified, FALSE otherwise.
*/
extern boolean grr_verify_saved_mm_message(
  mm_rr_message_id_enum_T message_id,
  const gas_id_t gas_id
);


/**
  @brief Verifies whether RR currently has a saved message from MAC that matches
         the specified message_id.

  @param message_id The message ID to check for.
  @returns TRUE if RR does have a saved message from MAC whose message ID matches
           that specified, FALSE otherwise.
*/
extern boolean grr_verify_saved_mac_message(
  byte message_id,
  const gas_id_t gas_id
);


/**
  @brief Checks whether RR currently has a message from MM saved in it's saved
         message store and optionally returns the message ID of the saved message
         if present.

  @param message_id If this parameter is non-NULL the function will write the
                    message ID of the current saved message, if it is present
                    and is a message from MM.
  @returns TRUE if RR does have a saved message from MM, FALSE otherwise.
*/
extern boolean grr_examine_saved_mm_message(
  byte * message_id,
  const gas_id_t gas_id
);



/**
  @brief Determine the current state of the grr_control() state machine.

  @return The current state of the grr_control() state machine.
*/
extern grr_state_t get_grr_state(const gas_id_t gas_id);


/**
  @brief Returns the current GRR control state name as a string.

  @return String containing the current GRR control state.
*/
extern const char *get_grr_control_state_name(grr_state_t grr_state); 

/**
  @brief Returns the current GRR control state name as a string.

  @return String containing the current GRR control state.
*/
extern const char *grr_control_current_state_name(const gas_id_t gas_id);

/**
  @brief Determine whether grr_control() is in ACQUIRE state.

  @return TRUE if grr_control() is in ACQUIRE state, FALSE otherwise.
*/
extern boolean grr_main_state_is_acquire(const gas_id_t gas_id);

/**
  @brief General module initialisation function
*/
extern void rr_grr_control_init(const gas_id_t gas_id);

/**
  @brief Initialisation function. Should be called once at RR task start 
   
  Note: The primary purpose of this function is to initialise pointers to control/data structures.
*/ 
extern void rr_grr_control_task_start_init(void);

/**
  @brief Determines whather grr_control() is able to process an
         RR_STOP_GSM_MODE_REQ with stop reason RR_SGLTE_RAT_CHANGE.

  @return TRUE if grr_control() is able to process the message, FALSE if it
          should be buffer in the RR pending command queue.
*/
extern boolean grr_can_process_stop_gsm_mode_req_sglte_rat_change(const gas_id_t gas_id);

extern boolean grr_can_process_sglte_x2g_req(const gas_id_t gas_id);

extern boolean grr_is_sglte_x2g_resel_in_progress(const gas_id_t gas_id);

extern boolean grr_can_process_sglte_band_coex_omsg_supported_bands_updated_ind(const gas_id_t gas_id);

#ifdef FEATURE_SGLTE
boolean grr_can_process_sglte_plmn_search(const gas_id_t gas_id);
#endif /*FEATURE_SGLTE*/


#endif /* RR_GRR_H */

/* EOF */

