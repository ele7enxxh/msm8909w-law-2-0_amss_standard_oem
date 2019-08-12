/*!
  @file rr_sglte.h

  @brief This header file provides access to public APIs contained in the
  rr_sglte.c module.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sglte.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef RR_SGLTE_H
#define RR_SGLTE_H

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_SGLTE

#include "comdef.h"
#include "rr_defs.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/**
  @brief Initialise the module.
*/
extern void rr_sglte_init(void);

/**
  @brief Sends GERAN_GRR_LTE_SGLTE_SERVING_MEAS_RSP indicating that service
         is not available.
*/
extern void rr_send_lte_sglte_serving_meas_rsp_no_service(const gas_id_t gas_id);
extern void rr_send_lte_sglte_neighbor_meas_rsp_no_service(const gas_id_t gas_id);

/**
  @brief Sends GERAN_GRR_TDS_SGLTE_SERVING_MEAS_RSP indicating that service
         is not available.
*/
extern void rr_send_tds_sglte_serving_meas_rsp_no_service(const gas_id_t gas_id);

/**
  @brief Sends GERAN_GRR_LTE_SGLTE_SERVING_MEAS_RSP.

  @param arfcn The ARFCN of the serving cell.
  @param bsic_known TRUE if the BSIC of the serving cell is known, FALSE
         otherwise.
  @param bsic  The BSIC of the serving cell, if known.
  @param rxlev The RXLEV of the serving cell.
*/
extern void rr_send_lte_sglte_serving_meas_rsp(
  ARFCN_T arfcn,
  boolean bsic_known,
  BSIC_T  bsic,
  uint8   rxlev,
  const gas_id_t gas_id
);

extern void rr_send_lte_sglte_neighbor_meas_rsp(
  ARFCN_T arfcn,
  boolean bsic_known,
  BSIC_T  bsic,
  uint8   rxlev,
  const gas_id_t gas_id
);


/**
  @brief Sends GERAN_GRR_TDS_SGLTE_SERVING_MEAS_RSP.

  @param arfcn The ARFCN of the serving cell.
  @param bsic_known TRUE if the BSIC of the serving cell is known, FALSE
         otherwise.
  @param bsic  The BSIC of the serving cell, if known.
  @param rxlev The RXLEV of the serving cell.
*/
extern void rr_send_tds_sglte_serving_meas_rsp(
  ARFCN_T arfcn,
  boolean bsic_known,
  BSIC_T  bsic,
  uint8   rxlev,
  const gas_id_t gas_id
);

extern void rr_sglte_set_rat_priority_list_gsm_only(const gas_id_t gas_id);

extern void rr_sglte_reject_lte_resel_req(const gas_id_t gas_id);

extern void rr_sglte_reject_lte_redir_req(const gas_id_t gas_id);

extern void rr_sglte_reject_lte_plmn_srch_req(const gas_id_t gas_id);

extern void rr_sglte_reject_tds_resel_req(const gas_id_t gas_id);

extern void rr_sglte_reject_tds_redir_req(const gas_id_t gas_id);

extern void rr_sglte_reject_tds_plmn_srch_req(const gas_id_t gas_id);

extern void rr_sglte_send_lte_plmn_srch_rsp_in_dedicated(
  sys_plmn_id_s_type        plmn,
  uint8                     rxlev,
  sys_srv_capability_e_type service_capability,
  const gas_id_t            gas_id
);

extern void rr_sglte_send_tds_plmn_srch_rsp_in_dedicated(
  sys_plmn_id_s_type        plmn,
  uint8                     rxlev,
  sys_srv_capability_e_type service_capability,
  gas_id_t gas_id
);

extern void rr_sglte_send_irat_plmn_srch_abort_cnf(
  rr_rat_e rat,
  gas_id_t gas_id
);

extern boolean rr_sglte_multimode_using_g_only_device(const gas_id_t gas_id);

#endif /* FEATURE_SGLTE */

#endif /* RR_SGLTE_H */
