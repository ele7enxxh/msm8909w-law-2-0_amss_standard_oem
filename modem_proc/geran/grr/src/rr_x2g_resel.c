/*! \file rr_x2g_resel.c

  This module implements a state machine that controls reselection from any other RAT to GERAN.

                Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_x2g_resel.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

#include "comdef.h"
#include "amssassert.h"
#include "gprs_mem.h"
#include "rr_seg_load.h"
#include "rr_x2g_resel.h"
#include "rr_mode.h"
#include "rr_gprs_debug.h"
#include "rr_cell_acq.h"
#include "rr_mm_send.h"
#include "rr_sys_info_pcell.h"
#include "rr_cell_selection.h"
#include "rr_main.h"
#include "rr_control.h"
#include "rr_multi_sim.h"
#include "rr_l1_idle_mode.h"
#include "rr_ps_domain.h"
#include "rr_general.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef enum
{
  /* No IRAT procedure is active. */
  RR_X2G_RESEL_STATE_NULL,

  /* Reselection/CCO procedure to GSM has been initiated, RR is activating
  L1. */
  RR_X2G_RESEL_STATE_ACTIVATING_L1,

  /* Acquisition of the target GSM cell is in progress. */
  RR_X2G_RESEL_STATE_ACQUIRING,

  /* Acquisition of the target GSM cell has completed successfully and RR has
  sent RR_ACTIVATION_IND to GMM - on receipt of RR_ACTIVATION_RSP idle mode
  will be started on the target cell. */
  RR_X2G_RESEL_STATE_ACTIVATING_MM,

  /* Acquisition of the target GSM cell has failed.  L1 is being deactivated
  in preparation for returning control to the calling RAT. */
  RR_X2G_RESEL_STATE_DEACTIVATING_L1,

  /* The IRAT procedure has been aborted by the other RAT during acquisition
  of the target cell.  The RR cell acquisition state machine has been sent
  an abort request and this state machine is awaiting confirmation that the
  acquisition has been aborted successfully.  Once this is received, RR
  will deactivate L1 in preparation for returning control to the
  calling RAT. */
  RR_X2G_RESEL_STATE_ABORTED_ABORTING_ACQUISITION,

  /* The IRAT procedure has been aborted by the other RAT.  L1 is being
  deactivated in preparation for returning control to the calling RAT. */
  RR_X2G_RESEL_STATE_ABORTED_DEACTIVATING_L1,

  RR_X2G_RESEL_STATE_MAX
} rr_x2g_resel_state_e;

typedef struct
{
  rr_x2g_resel_state_e        state;
  rr_x2g_resel_state_e        old_state;
  ARFCN_T                     target_arfcn;
  uint8                       target_rxlev;
  BSIC_T                      target_bsic;
  rr_cell_acq_status_e        cell_acq_status;
  mm_as_status_T              activation_reason;
  boolean                     activation_ind_sent;
  boolean                     any_cell_reselection_mode;
  boolean                     requested_plmn_valid;
  PLMN_id_T                   requested_plmn;
  boolean                     panic_reset_pending;
  rr_x2g_system_information_t system_information;
  rr_l1_gsm_stop_reason_T     l1_stop_reason;
} rr_x2g_resel_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/
static rr_x2g_resel_data_t *rr_x2g_resel_data_ptrs[NUM_GERAN_DATA_SPACES] = {0};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Obtain a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_x2g_resel_data_t*
 */
static rr_x2g_resel_data_t *rr_x2g_resel_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return rr_x2g_resel_data_ptrs[as_index];
}

static void rr_x2g_resel_activate_l1(
  const gas_id_t gas_id
)
{
  rr_event_T event;
  rr_x2g_resel_data_t *rr_x2g_resel_data_ptr;

  // Obtain a pointer to the module data
  rr_x2g_resel_data_ptr = rr_x2g_resel_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_resel_data_ptr);

  switch (rr_x2g_resel_data_ptr->activation_reason)
  {
    case MM_AS_INTERRAT_RESELECTION:
    {
      event = rr_mode_control(EV_ACTIVATE_GSM_MODE_FOR_X2G_RESEL, NULL, gas_id);
      break;
    } /* MM_AS_INTERRAT_RESELECTION */

    case MM_AS_WTOG_INTERRAT_CC_ORDER:
    case MM_AS_LTOG_CCO:
    {
      event = rr_mode_control(EV_ACTIVATE_GSM_MODE_FOR_X2G_CCO, NULL, gas_id);
      break;
    } /* MM_AS_WTOG_INTERRAT_CC_ORDER ... */

    default:
    {
      MSG_GERAN_HIGH_1_G("Unexpected activation reason: %d",
               rr_x2g_resel_data_ptr->activation_reason);

      event = rr_mode_control(EV_ACTIVATE_GSM_MODE_FOR_X2G_RESEL, NULL, gas_id);
    }
  }

  if (event == EV_GSM_MODE_ACTIVATED)
  {
    rr_x2g_resel_send_imsg(RR_X2G_RESEL_IMSG_L1_ACTIVATED_IND, gas_id);
  }
}

static void rr_x2g_resel_deactivate_l1(const gas_id_t gas_id)
{
  rr_x2g_resel_data_t *rr_x2g_resel_data_ptr;

  // Obtain a pointer to the module data
  rr_x2g_resel_data_ptr = rr_x2g_resel_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_resel_data_ptr);

  MSG_GERAN_HIGH_1_G("X2G: l1_stop_reason: %d", rr_x2g_resel_data_ptr->l1_stop_reason);
  rr_l1_gsm_stop_reason_set(rr_x2g_resel_data_ptr->l1_stop_reason, gas_id);

  /* advise the multi-SIM controller of this failure */
  rr_ms_x2g_procedure_failure(RR_X2G_RESELECTION_FAILURE, gas_id);

  if (EV_GSM_MODE_DEACTIVATED == rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id))
  {
    rr_x2g_resel_send_imsg(RR_X2G_RESEL_IMSG_L1_DEACTIVATED_IND, gas_id);
  }

  return;
}

static const char * rr_x2g_resel_state_name(rr_x2g_resel_state_e state)
{
  switch (state)
  {
    case RR_X2G_RESEL_STATE_NULL:
      return "NULL";

    case RR_X2G_RESEL_STATE_ACTIVATING_L1:
      return "ACTIVATING_L1";

    case RR_X2G_RESEL_STATE_ACQUIRING:
      return "ACQUIRING";

    case RR_X2G_RESEL_STATE_ACTIVATING_MM:
      return "ACTIVATING_MM";

    case RR_X2G_RESEL_STATE_DEACTIVATING_L1:
      return "DEACTIVATING_L1";

    case RR_X2G_RESEL_STATE_ABORTED_ABORTING_ACQUISITION:
      return "ABORTED_ABORTING_ACQUISITION";

    case RR_X2G_RESEL_STATE_ABORTED_DEACTIVATING_L1:
      return "ABORTED_DEACTIVATING_L1";

    default:
      MSG_GERAN_ERROR_1("Unexpected state: %d", state);
      return "?";
  }
}

static rr_x2g_resel_status_e rr_x2g_resel_map_cell_acq_status_to_x2g_resel_status(
  rr_cell_acq_status_e cell_acq_status
)
{
  switch (cell_acq_status)
  {
    case RR_CELL_ACQ_STATUS_FAILURE_CELL_BARRED:
      return RR_X2G_RESEL_STATUS_FAILURE_CELL_BARRED;

    case RR_CELL_ACQ_STATUS_FAILURE_CELL_LOW_C1:
      return RR_X2G_RESEL_STATUS_FAILURE_CELL_LOW_C1;

    case RR_CELL_ACQ_STATUS_FAILURE_PLMN_MISMATCH:
      return RR_X2G_RESEL_STATUS_FAILURE_CELL_PLMN_MISMATCH;

    case RR_CELL_ACQ_STATUS_FAILURE_FORBIDDEN_LAI:
      return RR_X2G_RESEL_STATUS_FAILURE_CELL_FORBIDDEN;

    case RR_CELL_ACQ_STATUS_FAILURE_BSIC_MISMATCH:
      return RR_X2G_RESEL_STATUS_FAILURE_CELL_BSIC_MISMATCH;

    case RR_CELL_ACQ_STATUS_FAILURE_BCCH_NOT_FOUND:
      return RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_NOT_FOUND;

    case RR_CELL_ACQ_STATUS_FAILURE_BSI_ACQUISITION_FAILURE:
      return RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_DECODE_FAILED;

    default:
    {
      MSG_GERAN_ERROR_1("Unexpected status: %d", cell_acq_status);
      return RR_X2G_RESEL_STATUS_UNDEFINED;
    }
  }
}

/**
  @brief Check if the target cell's PLMN matches the requested PLMN or is
         equivalent to it.

  @param target_cell_plmn_id The PLMN of the cell being reselected.
  @param requested_plmn_id The PLMN requested by NAS.
*/
static boolean rr_x2g_resel_is_plmn_suitable(
  rr_x2g_resel_data_t * data,
  PLMN_id_T             target_cell_plmn_id,
  PLMN_id_T             requested_plmn_id,
  const gas_id_t        gas_id
)
{
  boolean               plmn_is_undefined;
  boolean               mnc_includes_pcs_digit;
  sys_mcc_type          mcc;
  sys_mnc_type          mnc;

  sys_plmn_get_mcc_mnc(target_cell_plmn_id,
                       &plmn_is_undefined,
                       &mnc_includes_pcs_digit,
                       &mcc,
                       &mnc);

  if (data->any_cell_reselection_mode)
  {
    MSG_GERAN_HIGH_0_G("Any cell selection mode, PLMN %d-%d is suitable");
    return TRUE;
  }
  else
  {
    /* Check if the target cell's PLMN matches our requested PLMN. */
    boolean plmn_ok = sys_plmn_match(
      requested_plmn_id,
      target_cell_plmn_id
    );

    if (!plmn_ok)
    {
      /* Check if both the target cell's PLMN and our requested PLMN are in
      the equivalent PLMN list. */
      plmn_ok = rr_eplmn_list_equivalent_plmn(requested_plmn_id, gas_id) &&
                rr_eplmn_list_equivalent_plmn(target_cell_plmn_id, gas_id);
    }

    if (plmn_ok)
    {
      MSG_GERAN_HIGH_3_G("PLMN %d-%d (%d) is suitable", mcc, mnc, mnc_includes_pcs_digit);
      return TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_3_G("PLMN %d-%d (%d) is not suitable", mcc, mnc, mnc_includes_pcs_digit);
      return FALSE;
    }
  }
}

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

void rr_x2g_resel_init(
  const gas_id_t gas_id
)
{
  rr_x2g_resel_data_t *rr_x2g_resel_data_ptr;

  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  if (rr_x2g_resel_data_ptrs[as_index] == NULL)
  {
    rr_x2g_resel_data_t *local_ptr;

    local_ptr = (rr_x2g_resel_data_t *)GPRS_MEM_MALLOC(sizeof(rr_x2g_resel_data_t));

    /* initialise and store the pointer in the array of data spaces */
    rr_x2g_resel_data_ptrs[as_index] = local_ptr;
  }

  // Obtain a pointer to the module data
  rr_x2g_resel_data_ptr = rr_x2g_resel_data_ptrs[as_index];
  RR_NULL_CHECK_FATAL(rr_x2g_resel_data_ptr);

  memset(rr_x2g_resel_data_ptr, 0, sizeof(rr_x2g_resel_data_t));

  /* Set the default L1 stop reason to be used if the procedure fails.
  This may be modified if the procedure is aborted e.g. for RR_DEACT_REQ,
  which may require L1 to be stopped for DEEP SLEEP. */
  rr_x2g_resel_data_ptr->l1_stop_reason = RR_L1_RAT_CHANGE;

  /* reset ACTIVATION_RSP recived during X2G CCO status stored in RR_CONTROL */
  rr_set_x2g_cco_act_rsp_rcvd(FALSE, gas_id);
}

void rr_x2g_resel_deinit(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  if (rr_x2g_resel_data_ptrs[as_index] != NULL)
  {
    GPRS_MEM_FREE(rr_x2g_resel_data_ptrs[as_index]);
    rr_x2g_resel_data_ptrs[as_index] = NULL;
  }
}

boolean rr_x2g_resel_with_nacc_in_progress(
  const gas_id_t gas_id
)
{
  rr_x2g_resel_data_t *rr_x2g_resel_data_ptr;

  // Obtain a pointer to the module data
  rr_x2g_resel_data_ptr = rr_x2g_resel_get_data_ptr(gas_id);

  if (rr_x2g_resel_data_ptr == NULL)
  {
    return FALSE;
  }
  else
  {
    if (rr_x2g_resel_data_ptr->system_information.count > 0)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
}

void rr_x2g_resel_control(
  rr_cmd_bdy_type * message,
  const gas_id_t gas_id
)
{
  rr_x2g_resel_data_t *rr_x2g_resel_data_ptr;

  /* Validate input */
  RR_NULL_CHECK_RETURN_VOID(message);

  // Obtain a pointer to the module data
  rr_x2g_resel_data_ptr = rr_x2g_resel_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_resel_data_ptr);

  switch (rr_x2g_resel_data_ptr->state)
  {
    case RR_X2G_RESEL_STATE_NULL:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_RESEL_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_RESEL_IMSG_RESEL_REQ:
                {
                  rr_x2g_resel_imsg_resel_req_t * resel_req = &message->rr.x2g_resel.imsg.resel_req;

                  rr_x2g_resel_data_ptr->target_arfcn              = resel_req->arfcn;
                  rr_x2g_resel_data_ptr->target_rxlev              = resel_req->rxlev;
                  rr_x2g_resel_data_ptr->target_bsic               = resel_req->bsic;
                  rr_x2g_resel_data_ptr->activation_reason         = resel_req->activation_reason;
                  rr_x2g_resel_data_ptr->any_cell_reselection_mode = resel_req->any_cell_reselection_mode;
                  rr_x2g_resel_data_ptr->requested_plmn_valid      = resel_req->requested_plmn_valid;
                  rr_x2g_resel_data_ptr->requested_plmn            = resel_req->requested_plmn;
                  rr_x2g_resel_data_ptr->system_information        = resel_req->system_information;

                  rr_x2g_resel_activate_l1(gas_id);

                  rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_ACTIVATING_L1;
                  break;
                } /* RR_X2G_RESEL_IMSG_RESEL_REQ */

                case RR_X2G_RESEL_IMSG_PANIC_RESET_REQ:
                {
                  rr_x2g_resel_send_omsg_panic_reset_cnf(
                    rr_x2g_resel_data_ptr->activation_ind_sent,
                    gas_id
                  );
                  break;
                } /* RR_X2G_RESEL_IMSG_PANIC_RESET_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "NULL");
                }
              }
              break;
            } /* RR_X2G_RESEL_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "NULL");
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "NULL");
        }
      }
      break;
    } /* RR_X2G_RESEL_STATE_NULL */

    case RR_X2G_RESEL_STATE_ACTIVATING_L1:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_RESEL_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_RESEL_IMSG_L1_ACTIVATED_IND:
                {
                  PLMN_id_T * requested_plmn = NULL;

                  if (rr_x2g_resel_data_ptr->any_cell_reselection_mode)
                  {
                    MSG_GERAN_HIGH_0_G("any_cell_reselection_mode = TRUE, no PLMN check required");
                  }
                  else if (rr_x2g_resel_data_ptr->requested_plmn_valid)
                  {
                    requested_plmn = &rr_x2g_resel_data_ptr->requested_plmn;
                  }

                  rr_cell_acq_send_imsg_start_ind(
                    rr_x2g_resel_data_ptr->target_arfcn,
                    rr_x2g_resel_data_ptr->target_rxlev,
                    &rr_x2g_resel_data_ptr->target_bsic,
                    FALSE,
                    RR_CELL_ACQ_ACTION_SELECT_BCCH,
                    FALSE,
                    TRUE,
                    rr_gprs_get_pcell_info(gas_id),
                    requested_plmn,
                    gas_id
                  );
                  rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_ACQUIRING;
                  break;
                } /* RR_X2G_RESEL_IMSG_L1_DEACTIVATED_IND */

                case RR_X2G_RESEL_IMSG_ABORT_REQ:
                {
                  rr_x2g_resel_data_ptr->l1_stop_reason = message->rr.x2g_resel.imsg.abort_req.l1_stop_reason;
                  rr_x2g_resel_deactivate_l1(gas_id);
                  rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_ABORTED_DEACTIVATING_L1;
                  break;
                } /* RR_X2G_RESEL_IMSG_ABORT_REQ */

                case RR_X2G_RESEL_IMSG_PANIC_RESET_REQ:
                {
                  rr_x2g_resel_send_omsg_panic_reset_cnf(
                    rr_x2g_resel_data_ptr->activation_ind_sent,
                    gas_id
                  );
                  break;
                } /* RR_X2G_RESEL_IMSG_PANIC_RESET_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1");
                }
              }
              break;
            } /* RR_X2G_RESEL_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1");
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_L1");
        }
      }
      break;
    } /* RR_X2G_RESEL_STATE_ACTIVATING_L1 */

    case RR_X2G_RESEL_STATE_ACQUIRING:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_RESEL_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_RESEL_IMSG_ABORT_REQ:
                {
                  rr_x2g_resel_data_ptr->l1_stop_reason = message->rr.x2g_resel.imsg.abort_req.l1_stop_reason;
                  rr_cell_acq_send_imsg_abort_req(gas_id);
                  rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_ABORTED_ABORTING_ACQUISITION;
                  break;
                } /* RR_X2G_RESEL_IMSG_ABORT_REQ */

                case RR_X2G_RESEL_IMSG_PANIC_RESET_REQ:
                {
                  rr_x2g_resel_send_omsg_panic_reset_cnf(
                    rr_x2g_resel_data_ptr->activation_ind_sent,
                    gas_id
                  );
                  break;
                } /* RR_X2G_RESEL_IMSG_PANIC_RESET_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ACQUIRING");
                }
              }
              break;
            } /* RR_X2G_RESEL_IMSG */

            case RR_CELL_ACQ_OMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_CELL_ACQ_OMSG_FINISHED_IND:
                {
                  rr_cell_acq_omsg_finished_ind_t * finished_ind = &message->rr.cell_acq.omsg.finished_ind;
                  /* Cell acquisition has completed, either successfully or
                  unsuccessfully.  Store the status indicator as this is
                  required later.  If the acquisition was successful, send an
                  RR_ACTIVATION_IND to MM with the appropriate cause, if the
                  acquisition was unsuccessful initiate the deactivation of
                  L1. */
                  rr_x2g_resel_data_ptr->cell_acq_status = finished_ind->cell_acq_status;

                  switch (rr_x2g_resel_data_ptr->cell_acq_status)
                  {
                    case RR_CELL_ACQ_STATUS_SUCCESS_BCCH:
                    case RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED:
                    {
#ifdef FEATURE_SGLTE
                      if (rr_is_sglte_mode_active(gas_id))
                      {
                        if (rr_x2g_resel_data_ptr->activation_reason == MM_AS_INTERRAT_RESELECTION)
                        {                        
                          rr_send_activation_ind(MM_AS_SGLTE_INTER_RAT_RESELECTION, gas_id); 
                        }
                        else
                        { /*note CCO does not have SGLTE specifc cause, hence no conversion*/
                          rr_send_activation_ind(rr_x2g_resel_data_ptr->activation_reason, gas_id); 
                        }
                      }
                      else
#endif /*FEATURE_SGLTE*/
                      {
                        rr_send_activation_ind(rr_x2g_resel_data_ptr->activation_reason, gas_id);
                      }
                      rr_x2g_resel_data_ptr->activation_ind_sent = TRUE;
                      rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_ACTIVATING_MM;
                      break;
                    } /* RR_CELL_ACQ_STATUS_SUCCESS_BCCH ... */

                    default:
                    {
                      rr_x2g_resel_deactivate_l1(gas_id);
                      rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_DEACTIVATING_L1;
                    }
                  }
                  break;
                } /* RR_CELL_ACQ_OMSG_FINISHED_IND */

                case RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND:
                {
                  /* Ignore this signal, no action necessary. */
                  break;
                } /* RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND */

                case RR_CELL_ACQ_OMSG_RESELECTED_IND:
                {
                  /* Send any NACC system information if available. */
                  MSG_GERAN_MED_1_G("Tunnelled system information count: %d",
                          rr_x2g_resel_data_ptr->system_information.count);

                  if (rr_x2g_resel_data_ptr->system_information.count > 0)
                  {
                    gprs_scell_info_t * pcell = rr_gprs_get_pcell_info(gas_id);

                    /* Set up BA_INDs with value of 2.  This is a value that can not
                    be received OTA, so allows us to detect after camping whether or
                    not SI2 & friends have been received or not, which lets us trigger
                    their acquisition if necessary. */
                    pcell->gsm.complete_BA_list.no_of_entries = 0;
                    pcell->gsm.complete_idle_BA_list_received = FALSE;
                    pcell->gsm.complete_BA_list.BA_IND        = 0x02;
                    pcell->gsm.SI2_BA_list.BA_IND             = 0x02;
                    pcell->gsm.SI2bis_BA_list.BA_IND          = 0x02;
                    pcell->gsm.SI2ter_BA_list.BA_IND          = 0x02;

                    rr_x2g_system_information_send(
                      rr_x2g_resel_data_ptr->target_arfcn,
                      &rr_x2g_resel_data_ptr->system_information,
                      gas_id
                    );
                  }
                  break;
                } /* RR_CELL_ACQ_OMSG_RESELECTED_IND */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ACQUIRING");
                }
              }
              break;
            } /* RR_CELL_ACQ_OMSG */

            default:
            {
              /* All unhandled messages are forwarded on to the cell
              acquisition state machine. */
              rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          /* All unhandled messages are forwarded on to the cell
          acquisition state machine. */
          rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
        }
      }
      break;
    } /* RR_X2G_RESEL_STATE_ACQUIRING */

    case RR_X2G_RESEL_STATE_ACTIVATING_MM:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_RESEL_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_RESEL_IMSG_ABORT_REQ:
                {
                  rr_x2g_resel_data_ptr->l1_stop_reason = message->rr.x2g_resel.imsg.abort_req.l1_stop_reason;

                  if (rr_x2g_resel_data_ptr->panic_reset_pending)
                  {
                    rr_x2g_resel_send_omsg_panic_reset_cnf(
                      rr_x2g_resel_data_ptr->activation_ind_sent,
                      gas_id
                    );
                  }
                  else
                  {
                    /* If an abort is received after having sent an
                    RR_ACTIVATION_IND the assumption is that MM will ignore the
                    RR_ACTIVATION_IND and RR should honour the abort. */
                    rr_x2g_resel_deactivate_l1(gas_id);
                    rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_ABORTED_DEACTIVATING_L1;
                  }
                  break;
                } /* RR_X2G_RESEL_IMSG_ABORT_REQ */

                case RR_X2G_RESEL_IMSG_PANIC_RESET_REQ:
                {
                  MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in ACTIVATING_MM (buffered)");
                  rr_x2g_resel_data_ptr->panic_reset_pending = TRUE;
                  break;
                } /* RR_X2G_RESEL_IMSG_PANIC_RESET_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_MM");
                }
              }
              break;
            } /* RR_X2G_RESEL_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_MM");
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_MM_RR:
        {
          switch (message->message_header.message_id)
          {
            case RR_ACTIVATION_RSP:
            {
              boolean plmn_suitable;

              rr_set_x2g_cco_act_rsp_rcvd(TRUE, gas_id);
              plmn_suitable = rr_x2g_resel_is_plmn_suitable(
                rr_x2g_resel_data_ptr,
                rr_gprs_get_pcell_info(gas_id)->gsm.location_area_identification.plmn_id,
                message->rr_activation_rsp.PLMN_id,
                gas_id
              );

              if (plmn_suitable)
              {
                rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);

                /* As far as NAS is concerned GERAN is now the active RAT, so
                we can clear out our saved panic reset message (which should
                be an IRAT reselection request). */
                rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);
                rr_store_service_domain(message->rr_activation_rsp.service_domain,gas_id);
                plmn_request_details->search_reason = RR_AUTONOMOUS_CELL_SELECTION;

                rr_store_plmn_id(&message->rr_activation_rsp.PLMN_id, gas_id);
                rr_set_equivalent_plmn(
                  &rr_gprs_get_pcell_info(gas_id)->gsm.location_area_identification.plmn_id,
                  gas_id
                );

                rr_set_previous_LAI(
                  message->rr_activation_rsp.PLMN_id,
                  message->rr_activation_rsp.location_area_code,
                  gas_id
                );

                if (message->rr_activation_rsp.PS_Attached)
                {
                  grr_set_ota_gprs_suspension_parameters(
                    message->rr_activation_rsp.registered_rai,
                    gas_id
                  );
                }

                if (rr_x2g_resel_data_ptr->panic_reset_pending)
                {
                  rr_x2g_resel_send_omsg_panic_reset_cnf(
                    rr_x2g_resel_data_ptr->activation_ind_sent,
                    gas_id
                  );
                }
                else
                {
                  rr_x2g_resel_send_omsg_resel_cnf(RR_X2G_RESEL_STATUS_SUCCESS, gas_id);

                  rr_log_reselect_success(
                    rr_gprs_get_scell_arfcn(gas_id),
                    rr_gprs_get_scell_bsic(gas_id),
                    gas_id
                  );
                  rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_NULL;
                }
              }
              else
              {
                if (rr_x2g_resel_data_ptr->panic_reset_pending)
                {
                  rr_x2g_resel_send_omsg_panic_reset_cnf(
                    rr_x2g_resel_data_ptr->activation_ind_sent,
                    gas_id
                  );
                }
                else
                {
                  rr_x2g_resel_data_ptr->cell_acq_status = RR_CELL_ACQ_STATUS_FAILURE_PLMN_MISMATCH;
                  rr_x2g_resel_deactivate_l1(gas_id);
                  rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_DEACTIVATING_L1;
                }
              }
              break;
            } /* RR_ACTIVATION_RSP */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_MM");
            }
          }
          break;
        }

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "ACTIVATING_MM");
        }
      }
      break;

    } /* RR_X2G_RESEL_STATE_ACTIVATING_MM */

    case RR_X2G_RESEL_STATE_DEACTIVATING_L1:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_RESEL_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_RESEL_IMSG_L1_DEACTIVATED_IND:
                {
                  rr_x2g_resel_send_omsg_resel_cnf(
                    rr_x2g_resel_map_cell_acq_status_to_x2g_resel_status(
                      rr_x2g_resel_data_ptr->cell_acq_status
                    ),
                    gas_id
                  );
                  rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_NULL;
                  break;
                } /* RR_X2G_RESEL_IMSG_L1_DEACTIVATED_IND */

                case RR_X2G_RESEL_IMSG_ABORT_REQ:
                {
                  /* L1 is already deactivating as the acquisition failed, just
                  need to switch state so an abort confirmation is sent back
                  rather than a failure indication. */
                  rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_ABORTED_DEACTIVATING_L1;
                  break;
                } /* RR_X2G_RESEL_IMSG_ABORT_REQ */

                case RR_X2G_RESEL_IMSG_PANIC_RESET_REQ:
                {
                  rr_x2g_resel_send_omsg_panic_reset_cnf(
                    rr_x2g_resel_data_ptr->activation_ind_sent,
                    gas_id
                  );
                  break;
                } /* RR_X2G_RESEL_IMSG_PANIC_RESET_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING_L1");
                }
              }
              break;
            } /* RR_X2G_RESEL_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING_L1");
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING_L1");
        }
      }
      break;
    } /* RR_X2G_RESEL_STATE_DEACTIVATING_L1 */

    case RR_X2G_RESEL_STATE_ABORTED_ABORTING_ACQUISITION:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_RESEL_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_RESEL_IMSG_PANIC_RESET_REQ:
                {
                  rr_x2g_resel_send_omsg_panic_reset_cnf(
                    rr_x2g_resel_data_ptr->activation_ind_sent,
                    gas_id
                  );
                  break;
                } /* RR_X2G_RESEL_IMSG_PANIC_RESET_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ABORTED_ABORTING_ACQUISITION");
                }
              }
              break;
            } /* RR_X2G_RESEL_IMSG */

            case RR_CELL_ACQ_OMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_CELL_ACQ_OMSG_ABORT_CNF:
                {
                  rr_x2g_resel_deactivate_l1(gas_id);
                  rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_ABORTED_DEACTIVATING_L1;
                  break;
                } /* RR_CELL_ACQ_OMSG_ABORT_CNF */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ABORTED_ABORTING_ACQUISITION");
                }
              }
              break;
            } /* RR_CELL_ACQ_OMSG */

            default:
            {
              rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
        }
      }
      break;
    } /* RR_X2G_RESEL_STATE_ABORTED_ABORTING_ACQUISITION */

    case RR_X2G_RESEL_STATE_ABORTED_DEACTIVATING_L1:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_X2G_RESEL_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_X2G_RESEL_IMSG_L1_DEACTIVATED_IND:
                {
                  rr_x2g_resel_send_omsg_abort_cnf(rr_x2g_resel_data_ptr->activation_ind_sent, gas_id);
                  rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_NULL;
                  break;
                }

                case RR_X2G_RESEL_IMSG_PANIC_RESET_REQ:
                {
                  rr_x2g_resel_send_omsg_panic_reset_cnf(
                    rr_x2g_resel_data_ptr->activation_ind_sent,
                    gas_id
                  );
                  break;
                } /* RR_X2G_RESEL_IMSG_PANIC_RESET_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "ABORTED_DEACTIVATING_L1");
                }
              }
              break;
            } /* RR_X2G_RESEL_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "ABORTED_DEACTIVATING_L1");
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "ABORTED_DEACTIVATING_L1");
        }
      }
      break;
    } /* RR_X2G_RESEL_STATE_ABORTED_DEACTIVATING_L1 */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected state %d, resetting to NULL", rr_x2g_resel_data_ptr->state);
      rr_x2g_resel_data_ptr->state = RR_X2G_RESEL_STATE_NULL;
    }
  }

#ifdef DEBUG_RR_TRACE_MSG
  rr_store_trace_msg_buf(
    RR_X2G_RESEL_SM,
    EV_INPUT_MESSAGE,
    (byte) rr_x2g_resel_data_ptr->state,
    message,
    gas_id
  );
#endif

  if (rr_x2g_resel_data_ptr->state != rr_x2g_resel_data_ptr->old_state)
  {
    RR_LOG_STATE_TRANSITION(
      "rr_x2g_resel_control",
      rr_x2g_resel_data_ptr->old_state,
      rr_x2g_resel_data_ptr->state,
      rr_x2g_resel_state_name,
      gas_id
    );

    rr_x2g_resel_data_ptr->old_state = rr_x2g_resel_data_ptr->state;
  }
}

#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

/* EOF */
