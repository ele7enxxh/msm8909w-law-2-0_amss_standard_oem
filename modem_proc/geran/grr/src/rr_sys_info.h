#ifndef RR_SYS_INFO_H
#define RR_SYS_INFO_H

/*! \file rr_sys_info.h 
 
  This is the external API for the rr_sys_info.c module, which contains functionality related to
  receipt, storage and processing of system information messages.
 
                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_task.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define RR_SI5_COUNT_BEFORE_RESET  2

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Store a system information message received in the BCCH.
 * 
 * \param message_ptr (in) - Pointer to the received message (DL_UNIT_DATA_IND)
 * \param gas_id (in)
 */
extern void rr_store_system_info(
  dl_unit_data_ind_T  *message_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Swap the old SI5 and SI5ter BA lists.
 * 
 * \param gas_id (in)
 */
extern void rr_swap_old_si5_and_si5ter_ba_lists(
  const gas_id_t gas_id
);

/*!
 * \brief Resets requested si5 ba lists.
 * 
 * \param ba_list_mask (in)
 * \param gas_id (in)
 */
extern void rr_reset_old_si5_ba_list(
  uint16 ba_list_mask,
  const gas_id_t gas_id
);

/*!
 * \brief Updates si5 variant ba list.
 * 
 * \param si5_ba_list_ptr (in)
 * \param si5bis_ba_list_ptr (in)
 * \param si5ter_ba_list_ptr (in)
 * \param gas_id (in)
 */
extern void rr_update_old_si5_ba_lists(
  BCCH_frequency_list_T *si5_ba_list_ptr,
  BCCH_frequency_list_T *si5bis_ba_list_ptr,
  BCCH_frequency_list_T *si5ter_ba_list_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Get si5 variant ba list.
 * 
 * \param si5_variant (in)
 * \param gas_id (in)
 * 
 * \return BCCH_frequency_list_T 
 */
extern BCCH_frequency_list_T rr_get_old_si5_ba_list(
  uint16 si5_variant,
  const gas_id_t gas_id
);

/*!
 * \brief Increments si5_count value by one.
 * 
 * \param gas_id (in)
 */
extern void rr_increment_si5_count(
  const gas_id_t gas_id
);

/*!
 * \brief Returns si5_count value.
 * 
 * \param gas_id (in)
 * 
 * \return uint8 
 */
extern uint8 rr_get_si5_count(
  const gas_id_t gas_id
);

/*!
 * \brief Resets si5_count to 0.
 * 
 * \param gas_id (in)
 */
extern void rr_reset_si5_count(
  const gas_id_t gas_id
);

/*!
 * \brief Sets status of old cell's (cell before handover) si5 ba list status. 
 *  
 *  Sets status of old cell's (cell before handover) si5 ba list status. Set to TRUE after receiving a
 *  MPH_HANDOVER_IND. Set to FALSE once RR_SI5_COUNT_BEFORE_RESET no of SI5s have been received after a
 *  handover.
 * 
 * \param flag 
 * \param gas_id 
 */
extern void rr_set_status_of_old_cell_si5_ba_lists(
  boolean flag,
  const gas_id_t gas_id
);

/*!
 * \brief Get status of old cell's (cell before handover) si5 ba lists.
 * 
 * \param gas_id (in)
 * 
 * \return boolean 
 */
extern boolean rr_get_status_of_old_cell_si5_ba_lists(
  const gas_id_t gas_id
);

/*!
 * \brief Posts caches SIs back to the RR task queue.
 * 
 * \param serving_ARFCN (in)
 * \param gas_id (in)
 */
extern void rr_restore_dedicated_sys_info(
  ARFCN_T serving_ARFCN,
  const gas_id_t gas_id
);

/*!
 * \brief Reset the dedicated SI cache in this module.
 * 
 * \param gas_id (in)
 */
extern void rr_clear_dedicated_sys_info_cache(
  const gas_id_t gas_id
);

/*!
 * \brief Notifies MM or L1 (if required based on flags) of a change in the SI for the serving cell.
 * 
 * \param notify_L1 (in) - TRUE if L1 needs to be notified
 * \param notify_MM (in) - TRUE if MM needs to be notified
 * \param gas_id (in)
 */
extern void rr_perform_MM_or_L1_notification_on_serving_SI_change(
  boolean notify_L1,
  boolean notify_MM,
  const gas_id_t gas_id
);

#ifdef FEATURE_FAKE_BTS_CELL_INFO
extern void rr_handle_qmi_cell_info_req( const gas_id_t gas_id );
#endif /*FEATURE_FAKE_BTS_CELL_INFO*/

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

extern boolean rr_ms_3g_sys_info_mandatory(const gas_id_t gas_id);

#endif /* #ifndef RR_SYS_INFO_H */

/* EOF */

