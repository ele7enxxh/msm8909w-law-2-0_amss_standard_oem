/*============================================================================
  FILE:         rr_resel_g2w_if.c

  OVERVIEW:     This module provides functions to send signals to/from the
                RR G2W reselection module (i.e. RR_RESEL_G2W_IMSG,
                RR_RESEL_G2W_OMSG).

  DEPENDENCIES: None

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_resel_g2w_if.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_resel_g2w_if.h"
#include "rr_rr.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* --------------------------------------------------------------------------*/

void rr_resel_g2w_send_imsg_resel_req(
  rr_irat_cell_t                    target,
  rr_g2x_resel_type_e               type,
  sys_network_selection_mode_e_type network_selection_mode,
  PLMN_id_T                         serving_cell_plmn_id,
  uint16                            trk_lo_adj,
  const gas_id_t                    gas_id
)
{
  rr_resel_g2w_imsg_resel_req_t msg;

  msg.target                 = target;
  msg.type                   = type;
  msg.network_selection_mode = network_selection_mode;
  msg.serving_cell_plmn_id   = serving_cell_plmn_id;
  msg.trk_lo_adj             = trk_lo_adj;

  rr_rr_send(
    &msg.header,
    (uint8) RR_RESEL_G2W_IMSG_RESEL_REQ,
    sizeof(msg),
    RR_RESEL_G2W_IMSG,
    gas_id
  );
}

void rr_resel_g2w_send_imsg_abort_ind(
  rr_resel_g2w_abort_cause_e abort_cause,
  const gas_id_t gas_id
)
{
  rr_resel_g2w_imsg_abort_ind_t msg;

  msg.abort_cause = abort_cause;

  rr_rr_send(
    &msg.header,
    (uint8) RR_RESEL_G2W_IMSG_ABORT_IND,
    sizeof(msg),
    RR_RESEL_G2W_IMSG,
    gas_id
  );
}

void rr_resel_g2w_send_imsg_l1_deactivated_ind(
  const gas_id_t gas_id
)
{
  rr_message_header_t header;

  rr_rr_send(
    &header,
    (uint8) RR_RESEL_G2W_IMSG_L1_DEACTIVATED_IND,
    sizeof(header),
    RR_RESEL_G2W_IMSG,
    gas_id
  );
}

void rr_resel_g2w_send_imsg_l1_activated_ind(
  const gas_id_t gas_id
)
{
  rr_message_header_t header;

  rr_rr_send(
    &header,
    (uint8) RR_RESEL_G2W_IMSG_L1_ACTIVATED_IND,
    sizeof(header),
    RR_RESEL_G2W_IMSG,
    gas_id
  );
}

void rr_resel_g2w_send_omsg_resel_cnf(
  rr_resel_g2w_status_e status,
  const gas_id_t gas_id
)
{
  rr_resel_g2w_omsg_resel_cnf_t msg;

  msg.status = status;

  rr_rr_send(
    &msg.header,
    (uint8) RR_RESEL_G2W_OMSG_RESEL_CNF,
    sizeof(msg),
    RR_RESEL_G2W_OMSG,
    gas_id
  );
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_SGLTE

void rr_resel_g2w_send_imsg(
  rr_resel_g2w_imsg_e imsg,
  const gas_id_t gas_id
)
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) imsg,
             sizeof(header),
             RR_RESEL_G2W_IMSG,
             gas_id);
}

#endif /* FEATURE_SGLTE */

/* EOF */

