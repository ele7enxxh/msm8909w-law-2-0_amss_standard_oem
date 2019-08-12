/*============================================================================
  FILE:         rr_resel_if.c

  OVERVIEW:     This module provides functions to send signals to/from the
                RR reselection module (i.e. RR_RESEL_IMSG, RR_RESEL_OMSG).

  DEPENDENCIES: None

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_resel_if.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_resel_if.h"
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
* -------------------------------------------------------------------------*/

const char * rr_resel_imsg_name(
  const rr_resel_imsg_e imsg
)
{
  switch (imsg)
  {
    case RR_RESEL_IMSG_START_RPS_IND:
      return "RR_RESEL_IMSG_START_RPS_IND";

    case RR_RESEL_IMSG_REQUIRED_IND:
      return "RR_RESEL_IMSG_REQUIRED_IND";

    case RR_RESEL_IMSG_ABORT_REQ:
      return "RR_RESEL_IMSG_ABORT_REQ";

    case RR_RESEL_IMSG_C1_LT_ZERO_IND:
      return "RR_RESEL_IMSG_C1_LT_ZERO_IND";

#ifdef FEATURE_LTE
    case RR_RESEL_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND:
      return "RR_RESEL_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND";

    case RR_RESEL_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND:
      return "RR_RESEL_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND";

    case RR_RESEL_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND:
      return "RR_RESEL_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND";
#endif /* FEATURE_LTE */

#ifdef FEATURE_SGLTE
    case RR_RESEL_IMSG_STOP_GSM_MODE_REQ_SGLTE_RAT_CHANGE:
      return "RR_RESEL_IMSG_STOP_GSM_MODE_REQ_SGLTE_RAT_CHANGE";
#endif /* FEATURE_SGLTE */

    default:
      return "?";
  }
}

const char * rr_resel_omsg_name(
  const rr_resel_omsg_e omsg
)
{
  switch (omsg)
  {
    case RR_RESEL_OMSG_STARTED_IND:
      return "RR_RESEL_OMSG_STARTED_IND";
  
    case RR_RESEL_OMSG_FINISHED_IND:
      return "RR_RESEL_OMSG_FINISHED_IND";

    case RR_RESEL_OMSG_ABORT_CNF:
      return "RR_RESEL_OMSG_ABORT_CNF";

    case RR_RESEL_OMSG_MAC_IDLE_IND:
      return "RR_RESEL_OMSG_MAC_IDLE_IND";

    default:
      return "?";
  }
}

void rr_resel_send_start_rps_ind(
  ARFCN_T                              serving_cell,
  const inter_task_BCCH_allocation_T * ba_list,
  const gas_id_t                       gas_id
)
{
  rr_resel_imsg_start_rps_ind_t msg;

  msg.serving_cell = serving_cell;

  if (ba_list != NULL)
  {
    msg.ba_list = *ba_list;
  }
  else
  {
    memset(&msg.ba_list, 0, sizeof(msg.ba_list));
  }

  MSG_GERAN_HIGH_2_G(
    "IMSG_START_RPS_IND: serving_cell=%d ba_list.no_of_entries=%d",
    (int)msg.serving_cell.num,
    (int)msg.ba_list.no_of_entries
  );

  rr_rr_send(
    &msg.header,
    (uint8) RR_RESEL_IMSG_START_RPS_IND,
    sizeof(msg),
    RR_RESEL_IMSG,
    gas_id
  );
}

void rr_resel_send_imsg_required_ind(
  rr_resel_cause_e cause,
  const gas_id_t gas_id
)
{
  rr_resel_imsg_required_ind_t msg;

  msg.cause = cause;

  rr_rr_send(
    &msg.header,
    (uint8) RR_RESEL_IMSG_REQUIRED_IND,
    sizeof(msg),
    RR_RESEL_IMSG,
    gas_id
  );
}

void rr_resel_send_imsg_abort_req(
  rr_resel_abort_reason_e reason,
  const gas_id_t gas_id
)
{
  rr_resel_imsg_abort_req_t msg;

  msg.reason = reason;

  rr_rr_send(
    &msg.header,
    (uint8) RR_RESEL_IMSG_ABORT_REQ,
    sizeof(msg),
    RR_RESEL_IMSG,
    gas_id
  );
}

void rr_resel_send_imsg(
  rr_resel_imsg_e imsg,
  const gas_id_t gas_id
)
{
  rr_message_header_t msg;

  rr_rr_send(
    &msg,
    (uint8) imsg,
    sizeof(msg),
    RR_RESEL_IMSG,
    gas_id
  );
}

void rr_resel_send_omsg_finished_ind(
  rr_resel_status_e status,
  const gas_id_t gas_id
)
{
  rr_resel_omsg_finished_ind_t msg;

  msg.status = status;

  rr_rr_send(
    &msg.header,
    (uint8) RR_RESEL_OMSG_FINISHED_IND,
    sizeof(msg),
    RR_RESEL_OMSG,
    gas_id
  );
}

void rr_resel_send_omsg_started_ind(
  rr_resel_type_e type,
  const gas_id_t gas_id
)
{
  rr_resel_omsg_started_ind_t msg;

  msg.type = type;

  rr_rr_send(
    &msg.header,
    (uint8) RR_RESEL_OMSG_STARTED_IND,
    sizeof(msg),
    RR_RESEL_OMSG,
    gas_id
  );
}

void rr_resel_send_omsg_abort_cnf(
  const gas_id_t gas_id
)
{
  rr_message_header_t header;

  rr_rr_send(
    &header,
    (uint8) RR_RESEL_OMSG_ABORT_CNF,
    sizeof(header),
    RR_RESEL_OMSG,
    gas_id
  );
}

void rr_resel_send_omsg_mac_idle_ind(
  const gas_id_t gas_id
)
{
  rr_message_header_t header;

  rr_rr_send(
    &header,
    (uint8) RR_RESEL_OMSG_MAC_IDLE_IND,
    sizeof(header),
    RR_RESEL_OMSG,
    gas_id
  );
}

/* EOF */

