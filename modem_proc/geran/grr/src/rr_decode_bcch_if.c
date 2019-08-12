/*============================================================================
  FILE:         rr_resel_if.c

  OVERVIEW:     This module provides functions to send signals to/from the
                RR reselection module (i.e. RR_RESEL_IMSG, RR_RESEL_OMSG).

  DEPENDENCIES: None

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_decode_bcch_if.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_decode_bcch_if.h"
#include "rr_rr.h"
#include "rr_multi_sim.h"

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

void rr_decode_bcch_send_start_ind(
  rr_decode_bcch_start_ind_t decode_bcch_start_ind,
  const gas_id_t gas_id
)
{
  rr_decode_bcch_start_ind_t msg;

  msg = decode_bcch_start_ind;

  rr_rr_send(&msg.header,
             (uint8) RR_DECODE_BCCH_IMSG_START_IND,
             sizeof(msg),
             RR_DECODE_BCCH_IMSG,
             gas_id);
}

void rr_decode_bcch_send_abort_req(const gas_id_t gas_id)
{
  rr_decode_bcch_abort_req_t msg;

  rr_rr_send(&msg.header,
             (uint8) RR_DECODE_BCCH_IMSG_ABORT_REQ,
             sizeof(msg),
             RR_DECODE_BCCH_IMSG,
             gas_id);
}

void rr_decode_bcch_send_candidate_cell_si_ind(const gas_id_t gas_id)
{
  rr_decode_bcch_candidate_cell_si_ind_t msg;

  rr_rr_send(&msg.header,
             (uint8) RR_DECODE_BCCH_IMSG_CANDIDATE_CELL_SI_IND,
             sizeof(msg),
             RR_DECODE_BCCH_IMSG,
             gas_id);
}

void rr_decode_bcch_send_finish_ind(
  rr_arfcn_list_t *detected_frequency_list,
  const gas_id_t gas_id
)
{
  rr_decode_bcch_finished_ind_t msg;

  memset(&msg, 0, sizeof(msg));

  if (detected_frequency_list != NULL)
  {
    MSG_GERAN_MED_1_G("detected_frequency_list.count: %d", detected_frequency_list->count);
    msg.detected_frequency_list = *detected_frequency_list;
  }

  rr_rr_send(&msg.header,
             (uint8) RR_DECODE_BCCH_OMSG_FINISHED_IND,
             sizeof(msg),
             RR_DECODE_BCCH_OMSG,
             gas_id);
}

void rr_decode_bcch_send_abort_cnf(const gas_id_t gas_id)
{
  rr_decode_bcch_abort_cnf_t msg;

  rr_rr_send(&msg.header,
             (uint8) RR_DECODE_BCCH_OMSG_ABORT_CNF,
             sizeof(msg),
             RR_DECODE_BCCH_OMSG,
             gas_id);
}

/* EOF */

