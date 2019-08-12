/*! \file rr_ce_if.c 
 
  This module provides functions to send signals to/from the RR Connection Establishment state machine.
 
                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_ce_if.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_ce_if.h"
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

// See header file for documentation
const char *rr_ce_imsg_name(rr_ce_imsg_e imsg)
{
  switch (imsg)
  {
    case RR_CE_IMSG_CS_ABORT_REQ: return "RR_CE_IMSG_CS_ABORT_REQ";
    default: return "?";
  }
}

// See header file for documentation
void rr_ce_send_imsg_cs_abort_req(
  boolean ded_proc_in_progress,
  rr_ce_rel_ded_proc_details_t *ded_proc_details_ptr,
  const gas_id_t gas_id
)
{
  rr_ce_imsg_cs_abort_req_t msg;

  MSG_GERAN_HIGH_2_G(
    "IMSG_CS_ABORT_REQ: ded_proc=%d ded_proc_details=%d",
    (int)ded_proc_in_progress,
    (int)(ded_proc_details_ptr != NULL)
  );

  memset(&msg, 0, sizeof(msg));

  // Assume normal release from dedicated unless overridden below
  msg.l1_in_null = FALSE;

  if (ded_proc_in_progress)
  {
    if (ded_proc_details_ptr != NULL)
    {
      msg.l1_in_null = ded_proc_details_ptr->l1_in_null;
    }
    else
    {
      MSG_GERAN_ERROR_0_G("ded_proc_details_ptr=NULL");
    }
  }

  rr_rr_send(
    &msg.header,
    (uint8) RR_CE_IMSG_CS_ABORT_REQ,
    sizeof(msg),
    RR_CE_IMSG,
    gas_id
  );
}

// See header file for documentation
const char *rr_ce_omsg_name(rr_ce_omsg_e omsg)
{
  switch (omsg)
  {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case RR_CE_OMSG_X2G_CCO_STATUS_IND: return "RR_CE_OMSG_X2G_CCO_STATUS_IND";
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
    case RR_CE_OMSG_CS_ABORT_CNF: return "RR_CE_OMSG_CS_ABORT_CNF";
    case RR_CE_OMSG_CS_RELEASE_IND: return "RR_CE_OMSG_CS_RELEASE_IND";
    default: return "?";
  }
}

#if defined (FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

// See header file for documentation
void rr_ce_send_omsg_x2g_cco_status_ind(
  rr_cco_status_e status,
  const gas_id_t gas_id
)
{
  rr_ce_omsg_x2g_cco_status_ind_t msg;

  memset(&msg, 0, sizeof(msg));

  msg.status = status;

  rr_rr_send(
    &msg.header,
    (uint8) RR_CE_OMSG_X2G_CCO_STATUS_IND,
    sizeof(msg),
    RR_CE_OMSG,
    gas_id
  );
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

// See header file for documentation
void rr_ce_send_omsg_cs_abort_cnf(
  rr_ce_rel_status_e status,
  ARFCN_T arfcn,
  BSIC_T bsic,
  uint8 rxlev_average,
  inter_task_BCCH_allocation_T *ba_sacch_ptr,
  boolean purge_gmm_signalling,
  const gas_id_t gas_id
)
{
  rr_ce_omsg_cs_abort_cnf_t msg;

  memset(&msg, 0, sizeof(msg));

  msg.status = status;
  msg.cell_info.arfcn = arfcn;
  msg.cell_info.bsic = bsic;
  msg.cell_info.rxlev_average = rxlev_average;

  if (ba_sacch_ptr != NULL)
  {
    msg.cell_info.ba_sacch = *ba_sacch_ptr;
  }
  else
  {
    memset(&msg.cell_info.ba_sacch, 0, sizeof(msg.cell_info.ba_sacch));
  }

  msg.purge_gmm_signalling = purge_gmm_signalling;

  MSG_GERAN_HIGH_3_G(
    "OMSG_CS_ABORT_CNF: status=%d purge_gmm=%d ba_sacch=%d",
    (int)msg.status,
    (int)msg.purge_gmm_signalling,
    (int)msg.cell_info.ba_sacch.no_of_entries
  );

  rr_rr_send(
    &msg.header,
    (uint8) RR_CE_OMSG_CS_ABORT_CNF,
    sizeof(msg),
    RR_CE_OMSG,
    gas_id
  );
}

/*!
 * \brief Sends RR_CE_IMSG_CS_RELEASE_IND
 * 
 * \param status (in) - release status which indicates the next action
 * \param arfcn (in) - cell ARFCN
 * \param bsic (in) - cell BSIC
 * \param rxlev_average (in) - cell RxLev (average)
 * \param ba_sacch_ptr (in) - copy of the BA-SACCH which is the cell list for the RPS (optional)
 * \param purge_gmm_signalling (in) - TRUE/FALSE whether to purge GMM signalling PDUs when camping-on
 * \param gas_id (in)
 */
void rr_ce_send_imsg_cs_release_ind(
  rr_ce_rel_status_e status,
  ARFCN_T arfcn,
  BSIC_T bsic,
  uint8 rxlev_average,
  inter_task_BCCH_allocation_T *ba_sacch_ptr,
  boolean purge_gmm_signalling,
  const gas_id_t gas_id
)
{
  rr_ce_omsg_cs_release_ind_t msg;

  memset(&msg, 0, sizeof(msg));

  msg.status = status;
  msg.cell_info.arfcn = arfcn;
  msg.cell_info.bsic = bsic;
  msg.cell_info.rxlev_average = rxlev_average;

  if (ba_sacch_ptr != NULL)
  {
    msg.cell_info.ba_sacch = *ba_sacch_ptr;
  }
  else
  {
    memset(&msg.cell_info.ba_sacch, 0, sizeof(msg.cell_info.ba_sacch));
  }

  msg.purge_gmm_signalling = purge_gmm_signalling;

  MSG_GERAN_HIGH_3_G(
    "OMSG_CS_RELEASE_IND: status=%d purge_gmm=%d ba_sacch=%d",
    (int)msg.status,
    (int)msg.purge_gmm_signalling,
    (int)msg.cell_info.ba_sacch.no_of_entries
  );

  rr_rr_send(
    &msg.header,
    (uint8) RR_CE_OMSG_CS_RELEASE_IND,
    sizeof(msg),
    RR_CE_OMSG,
    gas_id
  );
}

/* EOF */

