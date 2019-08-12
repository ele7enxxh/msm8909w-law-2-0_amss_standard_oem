#ifndef RR_PROCESS_PMO_PCCO_H
#define RR_PROCESS_PMO_PCCO_H

/*! \file rr_process_pmo_pcco.h
 
  This is the header file for the rr_process_pmo_pcco.c module.
 
                Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_defs.h"
#include "rr_gprs_defs.h"
#include "rr_sys_info_i.h"
#include "csn_apt.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

#define MAX_PMO_INSTANCES                       0x08

typedef struct
{
  uint16                     indexes_expected_mask;
  uint8                      pmo_count;

  uint8                      nc_meas_params_index;
  nc_meas_params_mods_t      nc_meas_params;

  uint8                      gprs_meas_params_index;
  gprs_meas_params_mods_t    gprs_meas_params;

  /* TODO: GPRS REP PRIORITY */

#ifdef FEATURE_WCDMA
  uint8                      gprs_3g_meas_params_index;
  gprs_3g_meas_params_mods_t gprs_3g_meas_params;

  boolean                    nlist_3g_valid;
  grr_3g_nlist_t             nlist_3g;
#endif /* FEATURE_WCDMA */

  pmo_pcco_nacc_params_t     nacc_params;
  uint8                      three_g_ccn_active;

  boolean                          priority_and_eutran_params_valid;
  struct
  {
    boolean                          serving_cell_priority_params_valid;
    uint8                            serving_cell_priority_params_index;
    csn_gsm_priority_params_ie_t     serving_cell_priority_params;

    uint8                            utran_priority_params_valid_bitmap;
    csn_utran_priority_params_ie_t * utran_priority_params[MAX_PMO_INSTANCES];

    uint8                            eutran_params_valid_bitmap;
    csn_eutran_params_ie_t         * eutran_params[MAX_PMO_INSTANCES];
  } priority_and_eutran_params;

  boolean                          individual_priorities_valid;
  uint8                            individual_priorities_index;
  csn_individual_priorities_ie_t   individual_priorities;

} pmo_pcco_data_t;


/*!
 * \brief Returns the PMO 3G measurement parameters.
 * 
 * \param gas_id 
 * 
 * \return gprs_3g_meas_params_mods_t* 
 */
extern gprs_3g_meas_params_mods_t *rr_gprs_get_pmo_gprs_3g_meas_params(
  const gas_id_t gas_id
);

/*!
 * \brief Clears the PMO/PCCO data 
 *
 * \param gas_id 
 *
 * \return void* 
 */
extern void rr_psi_clear_pmo_data_going_inactive(const gas_id_t gas_id);


/*!
 * \brief Returns the PCCO 3G measurement parameters.
 * 
 * \param gas_id 
 * 
 * \return gprs_3g_meas_params_mods_t* 
 */
extern gprs_3g_meas_params_mods_t *rr_gprs_get_pcco_gprs_3g_meas_params(
  const gas_id_t gas_id
);

/*!
 * \brief Returns the PMO NC measurement parameters.
 * 
 * \param gas_id 
 * 
 * \return nc_meas_params_mods_t* 
 */
extern nc_meas_params_mods_t *rr_gprs_get_pmo_nc_meas_params(
  const gas_id_t gas_id
);


/**
  @brief Determine whether Priority and E-UTRAN Parameters Description has
         been received in a (set of) PMO messages.

  The return value of this function can be used to further determine whether
  previously received Serving Cell Priority Parameters IE, 3G (UTRAN) Priority
  Parameters IE and/or E-UTRAN Parameters IE should be deleted, in accordance
  with 3GPP 44.060 section 5.5.1.10.

  @return TRUE if Priority and E-UTRAN Parameters Description has been
          received, FALSE otherwise.
*/
extern boolean rr_get_pmo_priority_and_eutran_params_valid(const gas_id_t gas_id);

/**
  @brief Gets a copy of the Serving Cell Priority Parameters Description
         if received in a (set of) PMO messages.

  @param[out] dst Pointer to the destination buffer in to which the Serving
                  Cell Priority Parameters Description should be copied.
  @return TRUE if Serving Cell Priority Parameters Description is valid,
          FALSE if not.
*/
extern boolean rr_get_pmo_serving_cell_priority_params(
  csn_gsm_priority_params_ie_t * dst,
  const gas_id_t gas_id
);

/**
  @brief Gets a copy of the UTRAN Priority Parameters Description if received
         in a (set of) PMO messages.

  @param[out] dst Pointer to the destination buffer in to which the UTRAN
                  Priority Parameters Description should be copied.
  @return TRUE if UTRAN Priority Parameters Description is valid,
          FALSE if not.
*/
extern boolean rr_get_pmo_utran_priority_params(
  csn_utran_priority_params_ie_t * dst,
  const gas_id_t gas_id
);

/**
  @brief Gets a copy of the E-UTRAN Parameters Description if received in a
         (set of) PMO messages.

  @param[out] dst Pointer to the destination buffer in to which the E-UTRAN
                  Parameters Description should be copied.
  @return TRUE if UTRAN Priority Parameters Description is valid,
          FALSE if not.
*/
extern boolean rr_get_pmo_eutran_params(
  csn_eutran_params_ie_t * dst,
  const gas_id_t gas_id
);



/*!
 * \brief Returns the PMO measurement paramters.
 * 
 * \param gas_id 
 * 
 * \return gprs_meas_params_mods_t* 
 */
extern gprs_meas_params_mods_t *rr_gprs_get_pmo_gprs_meas_params(
  const gas_id_t gas_id
);

/*!
 * \brief Returns the PCCO NC measurement parameters.
 * 
 * \param gas_id 
 * 
 * \return nc_meas_params_mods_t* 
 */
extern nc_meas_params_mods_t *rr_gprs_get_pcco_nc_meas_params(
  const gas_id_t gas_id
);

/*!
 * \brief Returns the PCCO measurement parameters.
 * 
 * \param gas_id 
 * 
 * \return gprs_meas_params_mods_t* 
 */
extern gprs_meas_params_mods_t *rr_gprs_get_pcco_gprs_meas_params(
  const gas_id_t gas_id
);

/*!
 * \brief Returns the PCCO parameters related to NACC.
 * 
 * \param gas_id 
 * 
 * \return pmo_pcco_nacc_params_t* 
 */
extern pmo_pcco_nacc_params_t *rr_gprs_get_pcco_nacc_params(
  const gas_id_t gas_id
);

/*!
 * \brief Returns the PMO parameters related to NACC.
 * 
 * \param gas_id 
 * 
 * \return pmo_pcco_nacc_params_t* 
 */
extern pmo_pcco_nacc_params_t *rr_gprs_get_pmo_nacc_params(
  const gas_id_t gas_id
);

/*!
 * \brief Discards all PCCO-related data.
 * 
 * \param gas_id 
 */
extern void rr_psi_discard_pcco_data(
  const gas_id_t gas_id
);

/*!
 * \brief Discards all PMO-related data.
 * 
 * \param gas_id 
 */
extern void rr_psi_discard_pmo_data(
  const gas_id_t gas_id
);

/*!
 * \brief Clears the cell list received in the previous PCCO.
 * 
 * \param gas_id 
 */
extern void rr_gprs_reset_pcco_nc_frequency_list(
  const gas_id_t gas_id
);

/*!
 * \brief Decodes and processes a PCCO.
 * 
 * \param pcco - PCCO message (as output from the CSN decoder)
 * \param gas_id 
 */
extern void rr_gprs_pcco_decode_params(
  packet_cell_change_order_t *pcco,
  const gas_id_t gas_id
);

/*!
 * \brief Decodes and processes a PMO.
 * 
 * \param pmo - PMO message (as output from the CSN decoder)
 * \param gas_id 
 * 
 * \return rr_event_T 
 */
extern rr_event_T rr_gprs_pmo_decode_params(
  packet_measurement_order_t *pmo,
  const gas_id_t gas_id
);

/*!
 * \brief Adds the cells supplied in the PCCO to the BA(GPRS) list.
 * 
 * \param gas_id 
 */
extern void rr_gprs_pcco_apply_ba_gprs_deltas(
  const gas_id_t gas_id
);

/*!
 * \brief Initialisation function called during RR task start.
 */
extern void rr_process_pmo_pcco_task_start_init(
  void
);

/*CR550108*/
extern boolean rr_is_pmo_nc_frequency_list_valid(const gas_id_t gas_id);
extern pmo_pcco_data_t *rr_get_pmo_data(const gas_id_t gas_id);
extern pmo_pcco_data_t *rr_psi_get_pmo_store_ptr(const gas_id_t gas_id);


/*CR550108*/

#endif /* RR_PROCESS_PMO_PCCO_H */

/* EOF */

