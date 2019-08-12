#ifndef RR_X2G_H
#define RR_X2G_H

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

#include "comdef.h"
#include "rr_defs.h"
#include "rr_x2g_resel_if.h"
#ifdef FEATURE_WCDMA
#include "rrc_rr_types.h"
#endif /* FEATURE_WCMDA */
#ifdef FEATURE_GSM_TDS
#include "tdsrrc_rr_if.h"
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
#include "geran_grr_msg.h"
#endif /* FEATURE_LTE */

extern void rr_x2g_send_x2g_resel_abort_rsp(
  rr_rat_e rat,
  const gas_id_t gas_id
);

extern void rr_x2g_send_x2g_redir_abort_rsp(
  rr_rat_e rat,
  const gas_id_t gas_id
);

extern void rr_x2g_send_x2g_cco_rsp_success(
  rr_rat_e rat,
  const gas_id_t gas_id
);

extern void rr_x2g_send_x2g_cco_rsp_rejection(
  rr_rat_e rat,
  const gas_id_t gas_id
);

extern void rr_x2g_send_x2g_cco_rsp_aborted_by_nas(
  rr_rat_e rat,
  const gas_id_t gas_id
);

extern void rr_x2g_send_x2g_cco_rsp_failure_before_act_ind(
  rr_rat_e rat,
  rr_x2g_resel_status_e x2g_resel_status,
  const gas_id_t gas_id
);

extern void rr_x2g_send_x2g_cco_rsp_failure_after_act_ind(
  rr_rat_e rat,
  rr_cco_status_e cco_status,
  const gas_id_t gas_id
);

extern void rr_x2g_send_x2g_cco_abort_rsp(
  rr_rat_e rat,
  const gas_id_t gas_id
);

extern void rr_x2g_send_x2g_IRAT_rej(
  rr_x2g_resel_type_e x2g_resel_type,
  const gas_id_t gas_id
);

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
extern rr_interrat_cc_order_fail_reason_type rr_x2g_map_resel_status_to_w2g_cco_status(
  rr_x2g_resel_status_e resel_status
);

rr_interrat_cc_order_fail_reason_type rr_x2g_map_cco_status_to_w2g_cco_status(
  rr_cco_status_e status
);
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
tdsrr_interrat_cc_order_fail_reason_type rr_x2g_map_resel_status_to_t2g_cco_status(
  rr_x2g_resel_status_e resel_status
);

extern tdsrr_interrat_cc_order_fail_reason_type rr_x2g_map_cco_status_to_t2g_cco_status(
  rr_cco_status_e status
);
#endif /* FEATURE_GSM_TDS */

#endif /* RR_X2G_H */
