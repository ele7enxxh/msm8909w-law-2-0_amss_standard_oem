#ifndef RR_G2X_REDIR_H
#define RR_G2X_REDIR_H

/*============================================================================
  @file rr_g2x_redir.h

  The RR G2X Redirection state machine takes care of stopping L1 and sending
  a redirection request to the target RAT.  An ongoing redirection may be
  aborted by sending the appropriate request to this state machine.

  rr_g2x_redir_init() must be called prior to sending this state machine
  any signal.

  rr_g2x_redir_deinit() must be called after this state machine has indicated
  that it has finished.  This is signalled to the caller via:

    RR_G2X_REDIR_OMSG_REDIR_CNF
  or
    RR_G2X_REDIR_OMSG_ABORT_CNF

  rr_g2x_redir_deinit() must not be called until one of these signals has been
  received.

  It is guaranteed that when this state machine finishes it's procedure, either
  through a successful or unsuccessful redirection or after an ongoing
  redirection was aborted, L1 will be left in a stopped state, with stop reason
  indicated as RR_L1_RAT_CHANGE.

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_g2x_redir.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS_G2T_REDIRECTION)

#include "comdef.h"
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
  @brief Initialise the module, allocating any memory as necessary.

  This function MUST be called before any of this module's other APIs are
  used.
*/
extern void rr_g2x_redir_init(
  const gas_id_t gas_id
);

/**
  @brief Deinitialises the module, freeing any associated memory.
*/
extern void rr_g2x_redir_deinit(
  const gas_id_t gas_id
);

/**
  @brief This is the main state machine for this module.

  @param message An input message for this module.
*/
extern void rr_g2x_redir_control(
  rr_cmd_bdy_type * message,
  const gas_id_t gas_id
);

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
/*===========================================================================

FUNCTION rr_tds_pseudo_fr_populate_freq_list

DESCRIPTION
  Formats a frequency list for Pseudo FR (G to T Redir).
  The frequency list can only contain frequencies supplied in the Pseudo FR 
  frequency list from the T->G HO cmd.  Based on this list the return frequency
  list (G->T Redir) is formatted as follows:
  1) In RSCP order based on TDS measurements greater than the supplied Pseudo FR RSCP threshold.
      (note UARFCN for TDS measurements less than threshold are excluded from the list)
  2) Remaining frequencies in Pesudo FR freq list that have not been measured.

PARAMS
  tdsrrc_target_info_type *freq_list_ptr - Frequency list for population

RETURN VALUE
  None

===========================================================================*/
extern void rr_tds_pseudo_fr_populate_freq_list(
  tdsrrc_target_info_type *freq_list_ptr,
  const gas_id_t gas_id
);
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

#ifdef FEATURE_SGLTE
boolean rr_sglte_g2l_blind_redirection_active(const gas_id_t gas_id);
#endif /*FEATURE_SGLTE*/

#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION */

#endif /* RR_G2X_REDIR_H */

/* EOF */

