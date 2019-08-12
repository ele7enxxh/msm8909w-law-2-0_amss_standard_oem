#ifndef RR_SYS_INFO_SI_H
#define RR_SYS_INFO_SI_H

/**
  @file rr_sys_info_si.h

  This is the header file for the rr_sys_info_si.c module.

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info_si.h#1 $
*/

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "sys_type.h"
#include "rr_gprs_defs.h"
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

extern rr_event_T rr_gprs_bsi_acquisition_control(
  rr_event_T rr_event_in,
  void * event_in_data,
  const gas_id_t gas_id
);

extern void rr_gprs_build_ba_gprs_from_si2(
  gprs_scell_info_t *,
  const gas_id_t gas_id
);

extern void rr_gprs_gsm_ba_list_updated(
  boolean,
  const gas_id_t gas_id
);

extern void rr_gprs_check_for_cell_capability_change(
  boolean,
  system_information_database_T *
);

extern void rr_read_bcch_ext_sys_info(
  boolean,
  uint16,
  uint16,
  word,
  const gas_id_t gas_id
);

extern void rr_sys_info_msg_processed(
  uint16,
  const gas_id_t gas_id
);

extern void rr_gprs_update_meas_params_from_bcch(
  ARFCN_T channel_number,
  boolean notify_nc_meas_control,
  const gas_id_t gas_id
);

extern void rr_gprs_update_meas_params(
  const gas_id_t gas_id
);

extern void rr_nc_meas_params_merge(
  nc_measurement_params_t * dst,
  nc_meas_params_mods_t   * mods,
  const gas_id_t            gas_id
);

extern void rr_gprs_meas_params_merge(
  gprs_meas_params_t      * dst,
  gprs_meas_params_mods_t * mods
);

extern void rr_gprs_3g_meas_params_merge(
  gprs_3g_meas_params_t      * dst,
  gprs_3g_meas_params_mods_t * mods,
  const gas_id_t               gas_id
);

extern void rr_gprs_update_meas_params_with_modifications(
  ARFCN_T channel_number,
  const gas_id_t gas_id
);

extern void rr_gprs_update_gprs_bsic_description_from_si2quater_instances(
  ARFCN_T channel_number,
  const gas_id_t gas_id
);

extern void rr_gprs_revert_to_broadcast_meas_params_51(
  gprs_scell_info_t *,
  system_information_database_T *,
  const gas_id_t gas_id
);

extern void rr_gprs_notify_sys_info_ia_reject(
  boolean ia_rejected,
  const gas_id_t gas_id
);

/*!
 * \brief Requests L1 to read the BCCH sys info indicated by the bitmask.
 *
 *  Requests L1 to read the BCCH sys info indicated by the bitmask. If the bitmask is empty (all-zero),
 *  then L1 will stop reading BCCH sys info.
 *
 * \param si_required (in) - bitmask indicating SI required (1=yes)
 * \param gas_id (in)
 */
extern void rr_si_send_l1_read_sys_info_req(uint16 si_required, const gas_id_t gas_id);

/*!
 * \brief Returns a bitmask indicating the SI messages which are not being broadcast.
 * 
 * \param cell_info_ptr (in)
 * 
 * \return uint16 - SI bitmask (1=not broadcast)
 */
extern uint16 rr_si_messages_not_broadcast(gprs_scell_info_t *cell_info_ptr);

/*!
 * \brief Prints the SI bitmask to diag.
 * 
 * \param descr_text (in)
 * \param si_bitmask (in)
 */
extern void rr_si_print_bitmask(char *descr_text, uint16 si_bitmask);

/*!
 * \brief Calculates the time required to acquire to indicated SIs.
 * 
 * \param si_required (in)
 * \param si_on_bcch_ext (in)
 * 
 * \return uint32 - time required (millisecs)
 */
extern uint32 rr_si_calc_sys_info_acquisition_time(uint16 si_required, uint16 si_on_bcch_ext);

#endif /* RR_SYS_INFO_SI_H */

/* EOF */


