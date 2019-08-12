#ifndef RR_QSH_DUMP_H
#define RR_QSH_DUMP_H

/*!
 * \file rr_qsh_dump.h 
 *  
 * This module contains functionality to interface to QSH. 
 *  
 *              Copyright (c) 2015 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_qsh_dump.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"

#ifdef FEATURE_QSH_DUMP

#include "qsh.h"

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Performs the DUMP COLLECT call-back action
 * 
 * \param params_ptr (in)
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_dump_perform_cb_action(qsh_client_cb_params_s *cb_params_ptr);

/*!
 * \brief Add a message entry into the QSH dump data
 * 
 * \param timestamp
 * \param msg_set 
 * \param msg_id 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_dump_log_msg(timetick_type timestamp, uint8 msg_set, uint32 msg_id, const gas_id_t gas_id);

/*!
 * \brief Add an internal message entry into the QSH dump data
 * 
 * \param timestamp
 * \param msg_set 
 * \param msg_id 
 * \param gas_id 
 */;
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_dump_log_imsg(timetick_type timestamp, uint8 msg_set, uint8 msg_id, const gas_id_t gas_id);

/*!
 * \brief Add an event entry into the QSH dump data
 * 
 * \param timestamp
 * \param event 
 * \param sm 
 * \param gas_id
 */;
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_dump_log_event(
  timetick_type timestamp,
  uint16 event,
  uint8 sm,
  uint8 state, const gas_id_t gas_id);

/*!
 * \brief Initialisation function called from RR-QSH when the dump functionality is required.
 */
__attribute__((section(".uncompressible.text")))
extern qsh_client_dump_info_s rr_qsh_dump_init(void);

#endif // FEATURE_QSH_DUMP

#endif /* #ifndef RR_QSH_DUMP_H */

/* EOF */

