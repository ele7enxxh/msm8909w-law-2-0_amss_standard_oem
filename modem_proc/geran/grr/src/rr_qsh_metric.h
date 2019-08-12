#ifndef RR_QSH_METRIC_H
#define RR_QSH_METRIC_H

/*!
 * \file rr_qsh_metric.h 
 *  
 * This module contains functionality to interface to QSH. 
 *  
 *              Copyright (c) 2015 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_qsh_metric.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"

#ifdef FEATURE_QSH_EVENT_METRIC

#include "qsh.h"
#include "grr_qsh_ext.h"
#include "geran_dual_sim.h"
#include "rr_log_types.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "sys.h"

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns the definition of a metric
 * 
 * \param metric_id (in)
 * \param size_bytes_ptr (out) 
 * \param sampling_rate_ptr (out)
 * \param num_of_elements_ptr (out)
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_get_def(
  grr_qsh_metric_e metric_id,
  uint8 *size_bytes_ptr,
  uint16 *sampling_rate_ptr,
  uint8 *num_of_elements_ptr
);

/*!
 * \brief Performs the METRIC CONFIG call-back action
 * 
 * \param cb_params_ptr (in)
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_config_perform_cb_action(qsh_client_cb_params_s *cb_params_ptr);

/*!
 * \brief Performs the METRIC TIMER EXPIRY call-back action
 * 
 * \param params_ptr (in)
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_timer_expiry_perform_cb_action(qsh_client_cb_params_s *cb_params_ptr);

/*!
 * \brief Logs GRR_QSH_METRIC_MSG.
 * 
 * \param msg_set 
 * \param msg_id 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_msg(uint8 msg_set, uint32 msg_id, const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_IMSG.
 * 
 * \param msg_set 
 * \param msg_id 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_imsg(uint8 msg_set, uint8 msg_id, const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_OTA_MSG.
 * 
 * \param channel_type
 * \param msg_ptr
 * \param len 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_ota_msg(uint8 channel_type, uint8 *msg_ptr, uint8 msg_len, const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_EVENT.
 * 
 * \param event 
 * \param sm 
 * \param state 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_event(uint16 event, uint8 sm, uint8 state, const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_CELL_INFO.
 * 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_cell_info(const gas_id_t gas_id);

/*!
 * \brief Maps rr_l1_handover_type_T to sys_handover_failure_e_type.
 *
 * \param rr_l1_handover_type_T
 * \return sys_handover_failure_e_type
 */
__attribute__((section(".uncompressible.text")))
extern sys_handover_failure_e_type rr_qsh_metric_get_sys_ho_type(rr_l1_handover_type_T rr_l1_ho_type);

/*!
 * \brief Logs GRR_QSH_METRIC_CONN_END_INFO.
 * 
 * \param  sys_call_end_event_e_type type
 * \param uint8 cause  
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_conn_end_info(sys_call_end_event_e_type type, uint8 cause, const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_RESEL (G2G member).
 * 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_resel_g2g(const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_RESEL (G2WT member).
 * 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_resel_g2wt(const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_RESEL (G2L member).
 * 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_resel_g2l(const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_HANDOVER (G2G member).
 * 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_handover(rr_log_handover_type_T type, const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_RACH (num_of_rach member).
 * 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_rach(const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_RACH (num_of_ia_rcvd member).
 * 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_ia_rcvd(const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_RACH (num_of_ia_rej_rcvd member).
 * 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_ia_rej_rcvd(const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_CSFB.
 * 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_csfb(const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_SERVICE (num_of_dsf).
 * 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_dsf(const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_SERVICE (num_of_rlf).
 * 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_rlf(const gas_id_t gas_id);

/*!
 * \brief Logs GRR_QSH_METRIC_SERVICE (num_of_oos).
 * 
 * \param gas_id 
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_metric_log_oos(const gas_id_t gas_id);

/*!
 * \brief Initialisation function called from RR-QSH when metric functionality is required.
 */
__attribute__((section(".uncompressible.text")))
extern qsh_client_metric_info_s rr_qsh_metric_init(void);

#endif // FEATURE_QSH_EVENT_METRIC

#endif /* #ifndef RR_QSH_METRIC_H */

/* EOF */

