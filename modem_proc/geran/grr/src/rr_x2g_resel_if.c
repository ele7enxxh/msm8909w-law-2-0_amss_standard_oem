/*============================================================================
  FILE:         rr_x2g_resel_if.c

  OVERVIEW:

  DEPENDENCIES: None

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_x2g_resel_if.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_x2g_resel_if.h"
#include "rr_rr.h"
#include "geran_dual_sim.h"

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

void rr_x2g_resel_send_imsg(
  rr_x2g_resel_imsg_e imsg,
  const gas_id_t gas_id
)
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) imsg,
             sizeof(header),
             RR_X2G_RESEL_IMSG,
             gas_id);
}

void rr_x2g_resel_send_imsg_resel_req(
  ARFCN_T                       arfcn,
  uint8                         rxlev,
  BSIC_T                        bsic,
  mm_as_status_T                activation_reason,
  boolean                       any_cell_reselection_mode,
  PLMN_id_T                   * requested_plmn,
  rr_x2g_system_information_t * system_information,
  const gas_id_t                gas_id
)
{
  rr_x2g_resel_imsg_resel_req_t message;

  memset(&message, 0, sizeof(message));

  message.arfcn                     = arfcn;
  message.rxlev                     = rxlev;
  message.bsic                      = bsic;
  message.activation_reason         = activation_reason;
  message.any_cell_reselection_mode = any_cell_reselection_mode;
  if (requested_plmn != NULL)
  {
    message.requested_plmn_valid    = TRUE;
    message.requested_plmn          = *requested_plmn;
  }
  else
  {
    message.requested_plmn_valid    = FALSE;
  }
  if (system_information != NULL)
  {
    message.system_information      = *system_information;
  }

  MSG_GERAN_HIGH_0_G("RR_X2G_RESEL_IMSG_RESEL_REQ:");
  MSG_GERAN_HIGH_2_G("  arfcn:                     %d (%d)", arfcn.num, arfcn.band);
  MSG_GERAN_HIGH_1_G("  rxlev:                     %d", rxlev);
  MSG_GERAN_HIGH_2_G("  bsic:                      %d, %d", bsic.PLMN_colour_code, bsic.BS_colour_code);
  MSG_GERAN_HIGH_1_G("  any_cell_reselection_mode: %d", any_cell_reselection_mode);
  MSG_GERAN_HIGH_1_G("  activation_reason:         %d", activation_reason);
  if (message.requested_plmn_valid)
  {
    boolean      plmn_is_undefined;
    boolean      mnc_includes_pcs_digit;
    sys_mcc_type mcc;
    sys_mnc_type mnc;

    sys_plmn_get_mcc_mnc(message.requested_plmn,
                         &plmn_is_undefined,
                         &mnc_includes_pcs_digit,
                         &mcc,
                         &mnc);

    if (plmn_is_undefined)
    {
      MSG_GERAN_HIGH_0_G("  requested_plmn:            undefined");
    }
    else
    {
      MSG_GERAN_HIGH_3_G("  requested_plmn:            %d-%d (%d)", mcc, mnc, mnc_includes_pcs_digit);
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("  requested_plmn:            not present");
  }

  rr_rr_send(&message.header,
             (uint8) RR_X2G_RESEL_IMSG_RESEL_REQ,
             sizeof(message),
             RR_X2G_RESEL_IMSG,
             gas_id);
}

void rr_x2g_resel_send_imsg_abort_req(
  rr_l1_gsm_stop_reason_T l1_stop_reason,
  const gas_id_t gas_id
)
{
  rr_x2g_resel_imsg_abort_req_t message;

  memset(&message, 0, sizeof(message));

  MSG_GERAN_HIGH_1_G("RR_X2G_RESEL_IMSG_ABORT_REQ: l1_stop_reason: %d", l1_stop_reason);
  message.l1_stop_reason = l1_stop_reason;

  rr_rr_send(&message.header,
             (uint8) RR_X2G_RESEL_IMSG_ABORT_REQ,
             sizeof(message),
             RR_X2G_RESEL_IMSG,
             gas_id);
}

void rr_x2g_resel_send_omsg(
  rr_x2g_resel_omsg_e omsg,
  const gas_id_t gas_id
)
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) omsg,
             sizeof(header),
             RR_X2G_RESEL_OMSG,
             gas_id);
}

void rr_x2g_resel_send_omsg_resel_cnf(
  rr_x2g_resel_status_e status,
  const gas_id_t gas_id
)
{
  rr_x2g_resel_omsg_resel_cnf_t message;

  MSG_GERAN_MED_1_G("Sending RR_X2G_RESEL_OMSG_RESEL_CNF(status => %d)", status);

  message.status = status;

  rr_rr_send(&message.header,
             (uint8) RR_X2G_RESEL_OMSG_RESEL_CNF,
             sizeof(message),
             RR_X2G_RESEL_OMSG,
             gas_id);
}

void rr_x2g_resel_send_omsg_abort_cnf(
  boolean activation_ind_sent,
  const gas_id_t gas_id
)
{
  rr_x2g_resel_omsg_abort_cnf_t message;

  MSG_GERAN_MED_1_G("Sending RR_X2G_RESEL_OMSG_ABORT_CNF(activation_ind_sent => %d)", activation_ind_sent);

  message.activation_ind_sent = activation_ind_sent;

  rr_rr_send(&message.header,
             (uint8) RR_X2G_RESEL_OMSG_ABORT_CNF,
             sizeof(message),
             RR_X2G_RESEL_OMSG,
             gas_id);
}

void rr_x2g_resel_send_omsg_panic_reset_cnf(
  boolean activation_ind_sent,
  const gas_id_t gas_id
)
{
  rr_x2g_resel_omsg_panic_reset_cnf_t message;

  MSG_GERAN_MED_1_G("Sending RR_X2G_RESEL_OMSG_PANIC_RESET_CNF(activation_ind_sent => %d)", activation_ind_sent);

  message.activation_ind_sent = activation_ind_sent;

  rr_rr_send(&message.header,
             (uint8) RR_X2G_RESEL_OMSG_PANIC_RESET_CNF,
             sizeof(message),
             RR_X2G_RESEL_OMSG,
             gas_id);
}

#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

/* EOF */
