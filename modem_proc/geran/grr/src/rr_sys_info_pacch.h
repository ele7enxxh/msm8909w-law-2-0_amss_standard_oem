#ifndef RR_SYS_INFO_PACCH_H
#define RR_SYS_INFO_PACCH_H

/*! \file rr_sys_info_pacch.h 
 
  This is the header file for rr_sys_info_pacch.c.
  The functionality in this module is related to decoding & processing of PCCO, PMO and PSI messages.
 
                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info_pacch.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_sys_info_i.h"
#include "rr_gprs_defs.h"
#include "csn_apt.h"
#include "mac_grr.h"
#include "rr_events.h"
#include "rr_timer.h"
#include "geran_dual_sim.h"

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

/*!
 * \brief Store a GPRS mobile allocation.
 * 
 * \param ma_number (in)        - MA number (MA_NUMBER_14 or MA_NUMBER_15).
 * \param new_mobile_alloc (in) - Mobile allocation to be stored.
 * \param cell_info (in)        - Serving cell info.
 * \param gas_id (in)
 */
extern void rr_gprs_store_mobile_alloc(
  uint8                     ma_number,
  gprs_mobile_allocation_t *new_mobile_alloc,
  gprs_scell_info_t        *cell_info,
  const gas_id_t            gas_id
);

/*!
 * \brief Updated sys info parameters have been received from GMAC.
 * 
 * \param mac_power_control_params (in) - Updated power control parameters.
 * \param gas_id (in)
 */
extern void rr_gprs_updated_sys_info_params_from_mac(
  global_power_control_params_t *mac_power_control_params,
  const gas_id_t gas_id
);

/*!
 * \brief Called when a PSI message is received.
 * 
 * \param header (in)     - Indication of the message type
 * \param data (in)       - PSI message data
 * \param bcch_arfcn (in) - Channel received on
 * \param gas_id (in)
 */
extern void rr_gprs_new_psi(
  rlc_mac_cntrl_t *header,
  uint8 *data,
  ARFCN_T bcch_arfcn,
  const gas_id_t gas_id
);

/*!
 * \brief Converts PSI14 to PSI13 by converting the data format to that of PSI3.
 * 
 * \param psi14_ptr (in) - Pointer to PSI14 message (as output from the CSN decoder).
 * \param si13_ptr (out) - Updated with PSI13 message created from PSI14.
 * 
 * \return boolean - TRUE if successful; FALSE otherwise.
 */
extern boolean rr_gprs_convert_psi14_to_si13_data(
  packet_psi14_t *psi14_ptr,
  si13_data_t *si13_ptr
);

/*!
 * \brief Routes BCCH sys info from the PACCH to RR modules for processing (e.g. NACC, EDTM).
 * 
 * \param mac_data_ind_ptr (in) - Pointer to the MAC_GRR_DATA_IND message (as output from the CSN decoder).
 * \param gas_id (in)
 */
extern void rr_gprs_process_sys_info_from_pacch(
  mac_grr_data_ind_t *mac_data_ind_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Send Pkt SI Status requesting N/W to send any outstanding SIs.
 *
 *  On entering pkt transfer check if it's applicable to send Pkt SI status requesting N/W to send any
 *  outstanding SI's that the MS requires via the PACCH of the active TBF.
 *  See 44.060 5.5.1.3.4.
 * 
 * \param mac_status (in) - Current MAC status (as received in MAC_GRR_MAC_STATUS)
 * \param gas_id (in)
 */
extern void rr_gprs_send_si_status_during_si_acquisition(
  rr_mac_status_t mac_status,
  const gas_id_t gas_id
);

/*!
 * \brief Cancels the SI STATUS timers if GMAC is not in Packet Transfer.
 * 
 * \param mac_status (in) - Current MAC status (as received in MAC_GRR_MAC_STATUS)
 * \param gas_id (in)
 */
extern void rr_gprs_reset_si_status_timers(
  rr_mac_status_t mac_status,
  const gas_id_t gas_id
);

/*!
 * \brief Sends Pkt SI Status if SI13 indicates that BCCH messages have changed during transfer mode.
 *
 *  Sends Pkt SI Status if SI13 indicates that BCCH messages have changed during transfer mode.
 *  Note: Must NOT be during GCF testing, as testcases may not expect the additional RLC/MAC control message
 *  on the uplink PACCH.
 * 
 * \param scell_ptr (in) - serving cell info
 * \param gas_id (in)
 */
extern void rr_send_sysinfo_status_on_bcch_change_mark_update(
  gprs_scell_info_t *scell_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Called when either of the two sys info status timers has expired.
 *
 *  Called when either of the two sys info status timers has expired. Attempts a retransmission of the last
 *  status message if required. Further sendings shall take place at the first suitable opportunity at least
 *  2 secs after the message was sent the previous time.
 * 
 * \param timer_id (in)
 * \param gas_id (in)
 * 
 * \return rr_event_T 
 */
rr_event_T rr_gprs_sysinfo_status_timer_expired(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
);

/*!
 * \brief Sets the SI masks and PS_REL_REQ flags for the next SI Status.
 * 
 * \param bsi_mask (in)     - Mask of SIs that should be marked as received
 * \param ps_release (in)   - Indicate whether PS_REL_REQ should be set
 * \param enh_priority (in) - Indicate use of enhanced priority order (EDTM)
 * \param gas_id (in)
 */
extern void rr_gprs_set_sysinfo_status_params(
  word bsi_mask,
  boolean ps_release,
  boolean enh_priority,
  const gas_id_t gas_id
);

/*!
 * \brief Retrieves the current SI Status mask and PS release flag.
 * 
 * \param bsi_mask_ptr (out) - SI mask
 * \param gas_id (in)
 * 
 * \return boolean - Current value of PS_REL_REQ parameter
 */
extern boolean rr_gprs_get_sysinfo_status_params(
  word *bsi_mask_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Determines whether the sys info status enhanced priority should be used.
 * 
 * \param gas_id (in)
 * 
 * \return boolean - Current value of enhanced priority parameter
 */
extern boolean rr_gprs_use_sysinfo_status_enhanced_priority(
  const gas_id_t gas_id
);

/*!
 * \brief Creates a Pkt SI Status msg, and if the NW supports it, sends it
 * 
 * \param resend_timeout (in) - Duration of resend timer (0 == should not be started)
 * \param gas_id (in)
 * 
 * \return boolean - TRUE if the NW supports the sending of sys info status
 */
extern boolean rr_gprs_send_sysinfo_status(
  uint16 resend_timeout,
  const gas_id_t gas_id
);

/*!
 * \brief Initialisation function.
 *
 * \param gas_id (in)
 */
extern void rr_sys_info_pacch_init(
  const gas_id_t gas_id
);

/*!
 * \brief Initialisation function called during RR task start.
 */
extern void rr_sys_info_pacch_task_start_init(
  void
);

#endif /* RR_SYS_INFO_PACCH_H */

/* EOF */

