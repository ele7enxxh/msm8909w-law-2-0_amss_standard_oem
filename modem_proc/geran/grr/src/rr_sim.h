#ifndef RR_SIM_H
#define RR_SIM_H

/*! \file rr_sim.h
 
  This is the header file for rr_sim.c.
  This module contains SIM data communicated from NAS.
 
                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sim.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "mm_rr.h"
#include "sys_cnst.h"
#include "geran_dual_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
KEYSIZE_64,
KEYSIZE_128
} keysize_e;
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/*!
 * \brief Updates the SIM info (IMSI + TMSI + PTMSI) in the Public Store for MAC
 * 
 * \param gas_id 
 */
extern void rr_sim_update_public_store(
  const gas_id_t gas_id
);

/*!
 * \brief Called when RR_INVALIDATE_SIM_DATA_REQ received. Clears the stored SIM data.
 * 
 * \param msg_ptr (in) - pointer to the RR_INVALIDATE_SIM_DATA_REQ message
 * \param gas_id 
 */
extern void rr_sim_process_invalidate_sim_data_req(
  rr_invalidate_sim_data_req_T *msg_ptr,
  const gas_id_t gas_id
);

/**
  @brief Gets the EF-KC (Cipher Key) from the SIM
 
  @param cipher_key Updated with the value of EF-KC, if available.
 
  @return TRUE if EF-KC is available; FALSE otherwise
*/

extern boolean rr_siminfo_get_cipher_key(
  byte *cipher_key,
  byte destsize_64,
  byte *cipher_key_128,
  byte destsize_128,
  const gas_id_t gas_id
);

/**
  @brief Stores the EF-KC (cipher key).

  @param cipher_key Pointer to the cipher key to store.
*/
extern void rr_siminfo_store_cipher_keys(
  byte *cipher_key,
  byte *cipher_key_128,
  const boolean rr_active,
  const sys_modem_as_id_e_type as_id
);

/**
  @brief Stores the EF-KCSN (Cipher Key Sequence Number)
 
  @param cipher_key_seq Updated with the value of EF-KCSN, if
                        available.
*/
extern void rr_siminfo_store_cipher_key_seq(
  byte cipher_key_seq,
  const boolean rr_active,
  const sys_modem_as_id_e_type as_id
);

/*!
 * \brief Called when RR_SIM_INSERTED_REQ received. Stores the SIM data.
 * 
 * \param msg_ptr (in) - pointer to the RR_SIM_INSERTED_REQ message
 * \param gas_id 
 */
extern void rr_sim_process_sim_inserted_req(
  rr_sim_inserted_req_T *msg_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Called when RR_SIM_UPDATE_REQ received. Updates the stored the SIM data.
 * 
 * \param rr_sim_update_req_ptr (in) - pointer to the RR_SIM_UPDATE_REQ message
 * \param gas_id
 */
extern void rr_sim_process_sim_update_req(
  rr_sim_update_req_T *rr_sim_update_req_ptr,
  const gas_id_t gas_id
);


/*!
 * \brief Gets the EF-KCSN (Cipher Key Sequence Number) from the SIM.
 * 
 * \param cipher_key_seq (out) - Updated with the value of EF-KCSN (if available).
 * \param gas_id 
 * 
 * \return boolean - TRUE if available; FALSE otherwise.
 */
extern boolean rr_siminfo_get_cipher_key_seq(
  byte *cipher_key_seq,
  const gas_id_t gas_id
);

/*!
 * \brief Activates buffered cipher key and cipher key sequence number.
 *
 * This function should called on receipt of RRC_CIPHER_SYNC_IND or
 * TDSRR_CIPHER_SYNC_IND.  It copies the buffered cipher key and cipher
 * key sequence number to the main store.
 */
extern void rr_siminfo_activate_new_cipher_keys(
  const gas_id_t gas_id
);

/*!
 * \brief Gets the EF-ACC (Access Class) from the SIM.
 * 
 * \param access_class (out) - Updated with the Access Class (if available).
 * \param gas_id 
 * 
 * \return boolean - TRUE if available; FALSE otherwise.
 */
extern boolean rr_siminfo_get_access_class(
  byte *access_class,
  byte destsize,
  const gas_id_t gas_id
);

/*!
 * \brief Gets the IMSI from the SIM.
 * 
 * \param imsi (out) - Updated with the IMSI (if available)
 * \param gas_id 
 * 
 * \return boolean - TRUE if available; FALSE otherwise.
 */
extern boolean rr_siminfo_get_imsi(
  byte *imsi,
  byte destsize,
  const gas_id_t gas_id
);

/*!
 * \brief Gets the TMSI from the SIM.
 * 
 * \param tmsi (out) - Updated with the TMSI (if available).
 * \param gas_id 
 * 
 * \return boolean - TRUE if available; FALSE otherwise.
 */
extern boolean rr_siminfo_get_tmsi(
  byte *tmsi,
  byte destsize,
  const gas_id_t gas_id
);

/*!
 * \brief Gets the P-TMSI from the SIM.
 * 
 * \param ptmsi (out) - Updated with the P-TMSI (if available).
 * \param gas_id 
 * 
 * \return boolean - TRUE if available; FALSE otherwise.
 */
extern boolean rr_siminfo_get_ptmsi(
  byte *ptmsi,
  byte destsize,
  const gas_id_t gas_id
);

/*!
 * \brief Gets the MS Operational Mode from the SIM.
 * 
 * \param gas_id 
 * 
 * \return ms_operation_mode_T - the value of MS Operational Mode
 */
extern ms_operation_mode_T rr_siminfo_get_ms_opmode(
  const gas_id_t gas_id
);

/*!
 * \brief Returns TRUE if the inserted SIM is a Type Approval SIM
 * 
 * \param gas_id
 * 
 * \return boolean - TRUE if a Type Approval SIM, FALSE otherise
 */
extern boolean rr_sim_is_type_approval(
  const gas_id_t gas_id
);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Retrieves the MS Operational Mode for all subscriptions.
 *
 * \param op_mode_list (out) - array of operation modes.
 * \param num_entries  (in)  - number of entries in the op_mode_list
 */
void rr_ms_get_sim_operation_modes(byte *op_mode_list, int num_entries);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*!
 * \brief Gets the IMSImod1000 value from the SIM.
 * 
 * \param imsi_mod_1000 (out) - Updated with the value of IMSImod1000
 * \param gas_id 
 * 
 * \return boolean - TRUE if SIM information was available, FALSE otherwise
 */
extern boolean rr_get_imsi_mod_1000(
  word *imsi_mod_1000,
  const gas_id_t gas_id
);

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 *
 *  Note: The primary purpose of this function is to initialise pointers to control/data structures.
 */
extern void rr_sim_task_start_init(
  void
);

#endif /* #ifndef RR_SIM_H */

/* EOF */

