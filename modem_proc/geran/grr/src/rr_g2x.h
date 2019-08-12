/*!
  @file rr_g2x.h

  @brief
  Exports all types and functions provided by the rr_g2x.c module.

  This module provides the following functionality:

  - Management of databases containing details of IRAT neighbour cells to
    which reselection has previously been attempted but failed for various
    reasons:
    - Cell too weak;
    - Cell barred;
    - Cell in non-equivalent PLMN;
    - Cell in Forbidden LAI;
*/

/*===========================================================================

  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_g2x.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef RR_G2X_H
#define RR_G2X_H

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)

#include "comdef.h"
#include "rr_defs.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef union
{
  lte_earfcn_t lte;
  uint16       wcdma;
  uint16       tdscdma;
} rr_irat_frequency_u;

typedef struct
{
  rr_rat_e            rat;
  rr_irat_frequency_u data;
} rr_irat_frequency_t;

typedef enum
{
  RR_G2X_RESEL_FAILURE_CAUSE_UNDEFINED,
  RR_G2X_RESEL_FAILURE_CAUSE_FORBIDDEN_LA,
  RR_G2X_RESEL_FAILURE_CAUSE_FORBIDDEN_TA,
  RR_G2X_RESEL_FAILURE_CAUSE_PLMN_MISMATCH,
  RR_G2X_RESEL_FAILURE_CAUSE_CELL_QUALITY_FAILURE,
  RR_G2X_RESEL_FAILURE_CAUSE_CELL_BARRED,
  RR_G2X_RESEL_FAILURE_CAUSE_FREQUENCY_BARRED,
  RR_G2X_RESEL_FAILURE_CAUSE_CSG_FORBIDDEN,
  RR_G2X_RESEL_FAILURE_CAUSE_MAX
} rr_g2x_resel_failure_cause_e;

/* generic G2X Operation Type to be used to track IRAT G2X Ongoing Operation */
typedef enum
{
  RR_G2X_OPERATION_TYPE_UNDEFINED,
  RR_G2X_OPERATION_TYPE_RESELECTION,
  RR_G2X_OPERATION_TYPE_REDIRECTION,
  RR_G2X_OPERATION_TYPE_HANDOVER,
  RR_G2X_OPERATION_TYPE_CCO,
  RR_G2X_OPERATION_TYPE_MAX
} rr_g2x_operation_type_e;

typedef struct
{
  int16 squalmin;
  int16 srxlevmin;
} rr_g2x_cell_quality_data_wcdma_t;

typedef union
{
  rr_g2x_cell_quality_data_wcdma_t wcdma;
} rr_g2x_cell_quality_data_u;

/**
  @brief Union of various data elements that may be available, depending on
         the reason that an IRAT reselection failed.
*/
typedef struct
{
  /* Cell quality data is available if read from the SIB of the target cell. */
  rr_g2x_cell_quality_data_u cell_quality_data;

  /* If reselection fails because the target cell is in a forbidden LAI then
  the target cell's LAI should be included. */
  LAI_T                      lai;

  /* If reselection fails because the target cell is in a non-equivalent
  PLMN then the target cell's PLMN should be included. */
  PLMN_id_T                  plmn_id;

  /* More generically, X-RRC may specify time that RR should wait before
  attempting to reselect a specific cell. */
  uint32                     time_to_reselect;

} rr_g2x_resel_failure_data_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/**
  @brief Initialise the module.
*/
extern void rr_g2x_init(const gas_id_t gas_id);

extern void rr_g2x_barred_frequency_db_clear(const gas_id_t gas_id);

extern void rr_g2x_barred_cell_db_clear(const gas_id_t gas_id);

extern void rr_g2x_cell_quality_data_clear(const gas_id_t gas_id);

/**
  @brief Indicate that an IRAT reselection has failed, along with the failure
         cause.

  If the failure cause indicates a PLMN mismatch, reselection to all cells
  on the cell's frequency will be blocked for 20 minutes.

  @param cell The cell to which reselection failed.
  @param failure_cause The cause of the failure.
  @param failure_data Data associated with the failure cause.  May be NULL if
         not present / available.
*/
extern void rr_g2x_resel_failed(
  rr_irat_cell_t               * cell,
  rr_g2x_resel_failure_cause_e   cause,
  rr_g2x_resel_failure_data_t  * data,
  const gas_id_t                 gas_id
);

/**
  @brief Inform the G2X module that a timer associated with the non-EPLMN
         database has expired.

  @param timer_id The id of the timer that has expired.
*/
extern void rr_g2x_non_eplmn_db_handle_timer_expiry(
  rr_timer_id_e timer_id,
  const gas_id_t gas_id
);

/**
  @brief Determines if reselection to the specified WCDMA UARFCN is allowed.

  This function determinse if the WCDMA UARFCN belongs to a band which is not
  supported or the UARFCN may not be reselected for some other reason.

  @param earfcn The UARFCN being enquired about.
  @param band_pref_lte Bitmask describing the LTE bands that are currently
                       supported by the UE.
  @param any_cell_selection_mode_valid indicates whether the parameter
         any_cell_selection_mode should be considered when determining
  @param any_cell_selection_mode TRUE if RR is operating in any cell selection
         mode, FALSE otherwise.  TRUE generally indicates that the network
         selection mode is set to LIMITED SERVICE, FALSE indicates AUTOMATIC
         MODE.
  @return TRUE if reselection to the UARFCN is allowed, FALSE if not.
*/
extern boolean rr_g2x_resel_to_wcdma_frequency_is_allowed(
  uint16             uarfcn,
  sys_band_mask_type band_pref_wcdma,
  boolean            any_cell_selection_mode_valid,
  boolean            any_cell_selection_mode,
  const gas_id_t     gas_id
);

#ifdef FEATURE_GSM_TDS
/**
  @brief Determines if reselection to the specified UTRAN TDD
         UARFCN is allowed.

  This function determinse if the UTRAN TDD UARFCN belongs to a 
  band which is not supported or the UARFCN may not be 
  reselected for some other reason. 

  @param earfcn The UARFCN being enquired about.
  @param band_pref_lte Bitmask describing the TDD bands that are
                       currently supported by the UE.
  @param any_cell_selection_mode_valid indicates whether the parameter
         any_cell_selection_mode should be considered when determining
  @param any_cell_selection_mode TRUE if RR is operating in any cell selection
         mode, FALSE otherwise.  TRUE generally indicates that the network
         selection mode is set to LIMITED SERVICE, FALSE indicates AUTOMATIC
         MODE.
  @return TRUE if reselection to the UARFCN is allowed, FALSE if not.
*/
boolean rr_g2x_resel_to_utran_tdd_frequency_is_allowed(
  uint16             uarfcn,
  sys_band_mask_type band_pref_utran_tdd,
  boolean            any_cell_selection_mode_valid,
  boolean            any_cell_selection_mode,
  const gas_id_t     gas_id
);
#endif /*FEATURE_GSM_TDS*/

/**
  @brief Determines if reselection to the specified LTE EARFCN is allowed.

  This function determinse if the LTE EARFCN belongs to a band which is not
  supported or the EARFCN may not be reselected for some other reason.

  @param earfcn The EARFCN being enquired about.
  @param band_pref_lte Bitmask describing the LTE bands that are currently
                       supported by the UE.
  @return TRUE if reselection to the EARFCN is allowed, FALSE if not.
*/
extern boolean rr_g2x_resel_to_lte_frequency_is_allowed(
  lte_earfcn_t             earfcn,
  sys_lte_band_mask_e_type band_pref_lte,
  boolean                  any_cell_selection_mode_valid,
  boolean                  any_cell_selection_mode,
  const gas_id_t           gas_id
);

/**
  @brief Determines if reselection to a specific IRAT cell is allowed or not.

  This function determines if reselection to a specific IRAT cell is allowed
  or not.

  Reselection is not allowed if (in the case of WCDMA cells) WCDMA RRC has
  previously indicated a time_to_reselect greater than zero for the cell
  in question, and that time has not yet expired.

  Reselection is not allowed if (in the case of LTE cells) LTE RRC has
  previously indicated that the cell is barred, in which case RR will not
  attempt reselection to the cell for 20 minutes.

  @param cell The cell to which reselection may be attempted.

  @return TRUE if reselection to the cell is allowed, FALSE if it is not.
*/
extern boolean rr_g2x_resel_to_irat_cell_is_allowed(
  rr_irat_cell_t * cell,
  const gas_id_t gas_id
);

/**
  @brief Indicate to the G2X module that cell selection has been triggered.

  When cell selection is triggered RR should remove any limitations on
  reselection to IRAT frequencies that were previously barred or otherwise
  unsuitable.  Calling this function will achieve that.
*/
extern void rr_g2x_cell_selection_triggered(const gas_id_t gas_id);

/**
  @brief Indicate to the G2X module that PLMN selection has been triggered.

  PLMN selection is differentiated from cell selection by being performed
  only at the request of NAS (for PLMN selection) rather than autonomously
  (for cell selection).  When PLMN selection is triggered RR should wipe it's
  WCDMA cell quality data (Srxlevmin and Squalmin).

  In addition, this function will call rr_g2x_cell_selection_triggered.
*/
extern void rr_g2x_plmn_selection_triggered(const gas_id_t gas_id);

#ifdef FEATURE_WCDMA
/**
  @brief Populate the LOG_GSM_IDLE_MODE_3G_RESELECT_REJ_C log packet with
         appropriate information.

  @param log_ptr Pointer to the log packet to fill in.
*/
extern void rr_irat_populate_gsm_idle_mode_3g_reselect_rej_log_packet(
  LOG_GSM_IDLE_MODE_3G_RESELECT_REJ_C_type * log_ptr
);

extern boolean rr_g2x_wcdma_srxlevmin(
  int16 * srxlevmin,
  const gas_id_t gas_id
);

extern boolean rr_g2x_wcdma_squalmin(
  int16 * squalmin,
  const gas_id_t gas_id
);
#endif /*FEATURE_WCDMA*/

/**
  @brief Indicate to the G2X module that redirection to a specified IRAT
         frequency has been commanded by the network.

  This function will remove any limitations on reselection to the specified
  frequency, in accordance with 45.008 section 6.6.5 / 6.6.6.
*/
extern void rr_g2x_redir_triggered(
  rr_irat_frequency_t frequency,
  const gas_id_t gas_id
);

extern void rr_g2x_barred_frequency_timer_expired(
  rr_timer_id_e timer_id,
  const gas_id_t gas_id
);

extern void rr_g2x_barred_cell_timer_expired(
  rr_timer_id_e timer_id,
  const gas_id_t gas_id
);

extern void rr_g2x_forbidden_lai_list_updated(const gas_id_t gas_id);

extern void rr_g2x_equivalent_plmn_list_updated(const gas_id_t gas_id);

#ifdef FEATURE_WCDMA
extern boolean rr_g2x_is_uarfcn_valid(uint32 uarfcn, PLMN_id_T plmn_id, const gas_id_t gas_id);
#endif /* FEATURE_WCDMA */

extern void rr_g2x_csg_white_list_updated(const gas_id_t gas_id);

extern boolean rr_g2x_lock_exchange_allowed( const rr_g2x_operation_type_e rr_g2x_operation_type, const rr_rat_e target_rat, const gas_id_t gas_id);

extern boolean rr_g2x_rat_supported(const rr_rat_e target_rat, const gas_id_t gas_id);

extern boolean rr_g2x_allowed_by_gprs_activity(const rr_rat_e target_rat, const gas_id_t gas_id);


#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

#endif /* RR_G2X_H */
