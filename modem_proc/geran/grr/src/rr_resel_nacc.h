#ifndef RR_RESEL_NACC_H
#define RR_RESEL_NACC_H

/** @file */

/*============================================================================
  @file rr_resel_nacc.h

  Header file for rr_resel_nacc.c module.

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_resel_nacc.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"


#include "comdef.h"
#include "rr_task.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#define INVALID_CONTAINER_ID 0xFF

/* replacement names for rr_l1_handover_type_T */
#define NACC_GSM_GSM   RR_L1_GSM_GSM
#define NACC_WCDMA_GSM RR_L1_WCDMA_GSM
#define NACC_GSM_WCDMA RR_L1_GSM_WCDMA

/* define container IDs for data that arrives from RRC */
/* the actual values are not important, as long as they are (>4 and <255) */
#define INTERRAT_CONTAINER_ID_BASE (INVALID_CONTAINER_ID-MAX_NUMBER_OF_BEST_CELLS)
#define INTERRAT_CONTAINER_ID_1    (INTERRAT_CONTAINER_ID_BASE+0)
#define INTERRAT_CONTAINER_ID_2    (INTERRAT_CONTAINER_ID_BASE+1)

/* define a length value for "empty" NACC SI messages */
#define EMPTY_NACC_SI_LENGTH 4

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef enum
{
  CCN_RESET,
  CCN_RESPONSE_FROM_NW,
  CCN_REQUESTED_TO_L1,
  CCN_MAC_IN_TRANSFER,
  CCN_MAC_IN_IDLE
} ccn_event_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

extern boolean rr_gprs_nacc_pncd_in_progress(
  const gas_id_t gas_id
);

extern void rr_gprs_nacc_store_trigger_info(
  uint8 container_id,
  const gas_id_t gas_id
);

extern void rr_gprs_nacc_reset(
  const gas_id_t gas_id
);

extern void rr_nacc_init(
  const gas_id_t gas_id
);

extern void rr_gprs_nacc_store_ncell_data(
  mac_grr_ncell_data_ind_t * ncell_ind_ptr,
  const gas_id_t gas_id
);

#ifdef FEATURE_WCDMA
extern void rr_gprs_nacc_store_wtog_data(
  geran_sysinfo_choice_e_type data_type,
  uint8 *                     msg_ptr,
  uint8                       num_octets,
  const gas_id_t              gas_id
);
#endif /* FEATURE_WCDMA */

extern rr_event_T rr_gprs_nacc_handle_timer_expiry(
  rr_timer_id_T  timer_id,
  const gas_id_t gas_id
);

extern boolean rr_gprs_nacc_trigger_confirm(
  uint8           container_id,
  const ARFCN_T * arfcn_ptr,
  uint8           msgs_left,
  const gas_id_t  gas_id
);

extern uint8 rr_gprs_nacc_trigger_ncell_data(
  uint8          container_id,
  ARFCN_T        target_arfcn,
  boolean        new_procedure,
  const gas_id_t gas_id
);

/**
  @brief Determine whether the NACC container referenced by the specified
         container_id is empty.

  @return TRUE if the container is empty, FALSE otherwise.
*/
extern boolean rr_gprs_nacc_container_empty(
  uint8 container_id,
  const gas_id_t gas_id
);

extern rr_l1_handover_type_T rr_gprs_nacc_type(
  const gas_id_t gas_id
);

extern uint16 rr_gprs_nacc_additional_si_required(
  system_information_database_T *si_db_ptr,
  const boolean                  include_3G
);

extern rr_event_T rr_gprs_nacc_handle_ncell_data_cnf(
  rr_cmd_bdy_type * message_ptr,
  const gas_id_t gas_id
);

extern void rr_gprs_nacc_notify_mac_status_changed(
  rr_mac_status_t mac_status,
  const gas_id_t  gas_id
);

extern void rr_gprs_nacc_notify_bcch_msg_received(
  const byte     * l3_msg_ptr,
  uint8            source_msg,
  const gas_id_t   gas_id
);

extern void rr_gprs_nacc_notify_event(
  ccn_event_t event_in,
  const gas_id_t gas_id
);

extern rr_event_T rr_gprs_nacc_exit_ccn_mode(
  const gas_id_t gas_id
);

extern boolean rr_gprs_nacc_is_allowed(
  rr_l1_handover_type_T rat_type,
  const gas_id_t gas_id
);

extern void rr_gprs_nacc_initialise(
  rr_l1_handover_type_T rat_type,
  const gas_id_t gas_id
);

/****************************************************************************
 *   Function name: rr_gprs_is_nacc_active
 *   -------------
 *   Description:
 *   ------------
 *   Parameters:
 *   ----------
 *   Returns: TRUE if NACC procedure is not in the inactive state
 *   ----------
 ****************************************************************************/ 
extern boolean rr_gprs_is_nacc_active(
  const gas_id_t gas_id
);



#endif /* RR_RESEL_NACC_H */

 
/* EOF */

