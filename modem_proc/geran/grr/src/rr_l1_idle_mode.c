/*============================================================================
  rr_l1_idle_mode.c

  This file implements the RR-L1-IDLE state machine.
  This provides functionality to start / update L1 in Idle Mode.

                Copyright (c) 2011-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_l1_idle_mode.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $ */

/**
  @file rr_l1_idle_mode.c
  @brief This file implements the RR-L1-IDLE state machine. This provides functionality to start / update L1
         in Idle Mode

  This module uses RR IMSGs and OMSGs during the procedures
  @see rr_l1_idle_imsg_e
  @see rr_l1_idle_omsg_e
 */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "rr_seg_load.h"
#include "rr_l1_idle_mode.h"
#include "rr_l1_idle_mode_if.h"
#include "rr_l1_send.h"
#include "rr_sys_info.h"
#include "rr_sys_info_si.h"
#include "rr_sys_info_pcell.h"
#include "rr_sys_info_ncell.h"
#include "rr_general.h"
#include "rr_cell_selection.h"
#include "rrcrrif.h"
#include "rr_resel.h"
#include "rr_ps_access.h"
#include "rr_nv.h"
#include "rr_sim.h"
#include "rr_mm_send.h"
#include "rr_multi_sim.h"
#include "rr_defs.h"
#ifdef FEATURE_IDLE_DRX_SCALING
#include "rr_idle_drx.h"
#endif /* FEATURE_IDLE_DRX_SCALING */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef enum
{
  RR_L1_STATE_NULL,
  RR_L1_STATE_START_EARLY_CAMP,
  RR_L1_STATE_START_IDLE,
  RR_L1_STATE_CHANGE_IDLE,
  RR_L1_STATE_MAX
} rr_l1_idle_state_e;

typedef struct
{
  rr_l1_idle_state_e   state;
  rr_l1_idle_state_e   old_state;
} rr_l1_idle_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_l1_idle_data_t rr_l1_idle_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_l1_idle_data_t*
 */
static rr_l1_idle_data_t *rr_l1_idle_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_l1_idle_data[as_index];
}

/**
  @brief Returns a character string representing the RR-L1-IDLE-CONTROL state
*/
static char *rr_l1_idle_state_name(rr_l1_idle_state_e state)
{
  switch (state)
  {
    case RR_L1_STATE_NULL:                      return "NULL";
    case RR_L1_STATE_START_EARLY_CAMP:          return "START_EARLY_CAMP";
    case RR_L1_STATE_START_IDLE:                return "START_IDLE";
    case RR_L1_STATE_CHANGE_IDLE:               return "CHANGE_IDLE";

    default:
    {
      MSG_GERAN_ERROR_1("Unknown state: %d", state);
      return "?";
    }
  }
}


/**
  @brief Put L1 in EARLY CAMPING
*/
static void rr_l1_start_early_camp(
  ARFCN_T arfcn,
  BSIC_T bsic,
  LAI_T lai,
  word cell_identity,
  control_channel_description_T control_channel_descr,
  boolean control_channel_descr_valid,
  const gas_id_t gas_id
)
{
  word imsi_mod_1000;
  boolean imsi_valid;

  imsi_valid = rr_get_imsi_mod_1000(
    &imsi_mod_1000,   // imsi_mod_1000
    gas_id            // gas_id
  );

  if (control_channel_descr_valid)
  {
    rr_set_current_band(
      GET_ARFCN_BAND(arfcn),    // band
      gas_id                    // gas_id
    );

    /* set the paging mode to normal */
    rr_ce_set_ota_page_mode(
      NORMAL_PAGING,    // new_page_mode
      gas_id            // gas_id
    );

    rr_send_mph_early_camping_req(
      &control_channel_descr,
      imsi_valid,
      imsi_mod_1000,
      NORMAL_PAGING,
      gas_id
    );

    rr_log_gprs_early_camping(
      arfcn,     // bcch_arfcn
      bsic,      // bsic
      gas_id     // gas_id
    );
  }
  else
  {
    // TODO Remove 'early camping no ccch'.
    // At present this is still used in a few scenarios where SI3 data is not used, but these should be
    // re-worked to use SI3 data to enter early camping.
    // When this is complete, trigger Recovery Restart if no CCD information is available (see #if 0)

    MSG_GERAN_HIGH_0_G("No CCD available - enter Paging Reorg");

    /* set the paging mode to paging reorganization */
    rr_ce_set_ota_page_mode(
      PAGING_REORGANIZATION,    // new_page_mode
      gas_id                    // gas_id
    );

    /* send the request to L1 */
    rr_send_mph_early_camping_req(
      NULL,
      imsi_valid,
      imsi_mod_1000,
      PAGING_REORGANIZATION,
      gas_id
    );

#if 0
    if (geran_get_nv_recovery_restart_enabled())
    {
      MSG_GERAN_ERROR_0_G("No Control Channel Description available");
      geran_initiate_recovery_restart(GERAN_CLIENT_GRR);
    }
    else
    {
      ERR_GERAN_FATAL_0_G("No Control Channel Description available");
    }
#endif
  }

  /* Inform CB task that CBCH is no longer valid till we read SI4 */
  rr_send_cell_change_ind(
    lai,             // location_area_identification
    cell_identity,   // cell_identity
    FALSE,           // cbch_present
    gas_id           // gas_id
  );

  rr_send_cb_info(
    FALSE,    // cbch_present
    gas_id    // gas_id
  );


#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
  rr_set_rr_using_pseudo_si2q_data(FALSE, gas_id);  
#endif /*FEATURE_FAST_G2L_PSEUDO_SI2Q */

  /* Send the Signal Strength Indication to UI */
  rr_send_cm_status_ind(gas_id);
}


/**
  @brief Put L1 in IDLE / PACKET IDLE
*/
static void rr_l1_start_idle(
  boolean early_camping_performed,
  const gas_id_t gas_id
)
{
  grr_l1_idle_mode_t l1_idle_mode;
  gprs_scell_info_t *cell_info = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(cell_info);
  
  /* Initialize old cell's SI5 BA list status. */
  rr_set_status_of_old_cell_si5_ba_lists(FALSE, gas_id);

  /* If not camped, then an acquisition to the pending database has just been performed. */
  /* Copy this data over to the serving cell database. */
  /* Note: This action clears the pending cell database */
  if ( ! cell_info->gsm.camped_on_flag )
  {
    cell_info = rr_pcell_becomes_scell(gas_id);
  }

  MSG_GERAN_HIGH_0_G("Updating meas params...");
  rr_gprs_update_meas_params(gas_id);

  /* Set the current band based on the ARFCN of the serving cell */
  rr_set_current_band(
    (rr_internal_band_T) GET_ARFCN_BAND(cell_info->gsm.BCCH_ARFCN),   // band
    gas_id                                                            // gas_id
  );

  l1_idle_mode = grr_determine_l1_idle_mode(cell_info);

  if ( l1_idle_mode == GRR_L1_IDLE_MODE_CCCH )
  {
    if ( ! cell_info->gsm.camped_on_flag )
    {
      cell_info->gsm.camped_on_flag = TRUE;

      rr_gprs_build_ba_gprs_from_si2(
        cell_info,    // cell_info
        gas_id        // gas_id
      );

      /* Must have received SI1 band indicator by now (if broadcast).
       * Reassign correct band ind in following freq lists.
       */
      rr_gprs_reassign_band(
        NCELL_FREQ_LIST
        | IDLE_BA_FREQ_LIST
        | CBCH_FREQ
        | CELL_ALLOC_FREQ_LIST
        | MOBILE_ALLOC_FREQ_LIST,   // freq_list_mask
        TRUE,                       // is_camped
        gas_id                      // gas_id
      );
    }

    MSG_GERAN_HIGH_3_G("Start CCCH GPRS Packet Idle (arfcn=%d, band=%d)",
             (int)cell_info->gsm.BCCH_ARFCN.num,
             (int)cell_info->gsm.BCCH_ARFCN.band,
             0);

    /* Instruct Layer 1 to start GPRS-51 idle */
    rr_start_ccch51_idle_mode(
      cell_info,    // cell_info
      gas_id        // gas_id
    );

    cell_info->l1_idle_mode = GRR_L1_IDLE_MODE_CCCH;

    if ( rr_get_nv_egprs_enabled(RR_GAS_ID_TO_AS_ID) &&
         cell_info->bcch_sys_info.gprs_cell_options.
           optional_extension_info_flag &&
         cell_info->bcch_sys_info.gprs_cell_options.
           optional_extension_info.egprs_supported_flag )
    {
      MSG_GERAN_HIGH_0_G("Cell supports EGPRS; enabled in MS");
    }
    else
    {
      MSG_GERAN_HIGH_0_G("Cell supports EGPRS; disabled in MS");
    }
  }
  else
  {
    if ( ! cell_info->gsm.camped_on_flag )
    {
      cell_info->gsm.camped_on_flag = TRUE;

      rr_gprs_build_ba_gprs_from_si2(
        cell_info,    // cell_info
        gas_id        // gas_id
      );

      /* Must have received SI1 band indicator by now if broadcasted. Re-assign band
      ind in following freq lists */
      rr_gprs_reassign_band(
        NCELL_FREQ_LIST
        | IDLE_BA_FREQ_LIST
        | CBCH_FREQ
        | CELL_ALLOC_FREQ_LIST
        | MOBILE_ALLOC_FREQ_LIST,   // freq_list_mask
        TRUE,                       // is_camped
        gas_id                      // gas_id
      );
    }

    MSG_GERAN_HIGH_3_G("Start GSM Idle (arfcn=%d, band=%d)",
             (int)cell_info->gsm.BCCH_ARFCN.num,
             (int)cell_info->gsm.BCCH_ARFCN.band,
             0);

    /* Instruct Layer 1 to start GSM idle */
    rr_gprs_start_gsm_idle_mode(
      cell_info,                 // cell_info
      early_camping_performed,   // early_camp_performed
      gas_id                     // gas_id
    );

    cell_info->l1_idle_mode = GRR_L1_IDLE_MODE_GSM;
  }

  rr_hz_update_zone_info(RR_HZ_UPDATE_INFO_ONLY, gas_id);
  rr_hz_notify_state_change(RR_HZ_STATE_IDLE, gas_id);

  return;
}

/**
  @brief Called to post RR_L1_IDLE_OMSG_START_IDLE_CNF to GRR itself
*/
static void rr_send_internal_start_idle_cnf(const gas_id_t gas_id)
{
  gprs_scell_info_t *cell_info = rr_gprs_get_scell_info(gas_id);

  rr_l1_idle_send_omsg_start_idle_cnf(cell_info->gsm.gprs_indicator, gas_id);

  return;
}

/**
  @brief Called when L1 confirms transition to IDLE
*/
static void rr_l1_idle_confirmed(const gas_id_t gas_id)
{
  MSG_GERAN_HIGH_0_G("L1 in Idle");

  rr_pending_mode_change_data_action(gas_id);
  rr_pending_mode_change_data_clear(gas_id);

  if (!rr_gsm_only(gas_id))
  {
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
    if (rr_default_irat_params_update_required(gas_id))
    {
      if (rr_check_and_populate_default_irat_prior_params(gas_id))
      {
        MSG_GERAN_HIGH_0_G("PSEUDO SI2Q Setting is a Success");
      }
    }
#endif /* FEATURE_FAST_G2L_PSEUDO_SI2Q */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    rr_l1_update_wcdma_neighbor_list(gas_id);
#else
    MSG_GERAN_HIGH_0_G("GSM-only is FALSE");
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  }

  /* send RR_L1_IDLE_OMSG_START_IDLE_CNF */
  rr_send_internal_start_idle_cnf(gas_id);

  return;
}


/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/

grr_l1_idle_mode_t grr_determine_l1_idle_mode(
  gprs_scell_info_t *cell_info
)
{
  /* If the cell supports GPRS and SI13 has been received, packet idle mode
  is started in L1.  Otherwise, GSM idle mode is started. */
  if (cell_info->gsm.gprs_indicator && cell_info->bcch_sys_info.si13_data_valid_flag)
  {
    return GRR_L1_IDLE_MODE_CCCH;
  }
  else
  {
    return GRR_L1_IDLE_MODE_GSM;
  }
}


boolean  rr_is_l1_idle_state_start_idle(const gas_id_t gas_id)
{

  rr_l1_idle_data_t *rr_l1_idle_data_ptr;

  // Obtain a pointer to the module data
  rr_l1_idle_data_ptr = rr_l1_idle_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_l1_idle_data_ptr);

  if( rr_l1_idle_data_ptr->state == RR_L1_STATE_START_IDLE )
  {
   return TRUE;
  }
  else
  {
   return FALSE;
  }
}


/**
  @brief Returns TRUE if RR-L1-IDLE-CONTROL is ready to process RR_L1_IDLE_IMSG_START_IDLE_REQ
*/
boolean rr_l1_idle_can_process_start_idle_req(const gas_id_t gas_id)
{
  boolean can_process = TRUE;
  rr_l1_idle_data_t *rr_l1_idle_data_ptr;

  // Obtain a pointer to the module data
  rr_l1_idle_data_ptr = rr_l1_idle_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_l1_idle_data_ptr);

  switch (rr_l1_idle_data_ptr->state)
  {
    case RR_L1_STATE_START_EARLY_CAMP:
    {
      can_process = FALSE;
      break;
    }

    default: ;
  }

  return can_process;
}


/**
  @brief Main RR-L1-IDLE-CONTROL state machine
*/
void rr_l1_idle_control(
  const rr_cmd_bdy_type *msg_ptr,
  const gas_id_t gas_id
)
{
  rr_l1_idle_data_t *rr_l1_idle_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module data
  rr_l1_idle_data_ptr = rr_l1_idle_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_l1_idle_data_ptr);

  /*
  Before performing the normal message processing per state, just check that the current state machine
  state matches the actual state of L1. These could be different if e.g. after starting early camping
  via this module, the procedure being performed subsequently failed and a power-scan triggered
  */

  switch (msg_ptr->message_header.message_set)
  {
    case MS_RR_RR:
    {
      switch (msg_ptr->rr.header.rr_message_set)
      {
        case RR_L1_IDLE_IMSG:
        {
          switch (msg_ptr->message_header.message_id)
          {
            case RR_L1_IDLE_IMSG_START_EARLY_CAMP_REQ:
            case RR_L1_IDLE_IMSG_START_IDLE_REQ:
            {
              rr_l1_idle_status_e l1_idle_status = rr_l1_send_get_l1_idle_status(gas_id);

              if (l1_idle_status == RR_L1_IDLE_STATUS_NOT_IDLE)
              {
                if (rr_l1_idle_data_ptr->state != RR_L1_STATE_NULL)
                {
                  MSG_GERAN_HIGH_0_G("Correcting RR-L1-IDLE-CONTROL state");
                  rr_l1_idle_data_ptr->state = RR_L1_STATE_NULL;
                }
              }
              break;
            }

            default: ;
          } // message_id
          break;
        } /* RR_L1_IDLE_IMSG */

        default: ;
      } // message_set
      break;
    } /* MS_RR_RR */
  }

  // Now perform normal message processing per state

  switch (rr_l1_idle_data_ptr->state)
  {
    case RR_L1_STATE_NULL:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_L1_IDLE_IMSG:
            {
              switch (msg_ptr->message_header.message_id)
              {
                case RR_L1_IDLE_IMSG_START_EARLY_CAMP_REQ:
                {
                  const rr_l1_idle_imsg_start_early_camp_req_t *imsg = &msg_ptr->rr.l1_idle.imsg.early_camp_req;

                  rr_l1_start_early_camp(
                    imsg->arfcn,
                    imsg->bsic,
                    imsg->lai,
                    imsg->cell_identity,
                    imsg->control_channel_descr,
                    imsg->control_channel_descr_valid,
                    gas_id
                  );

                  rr_l1_idle_data_ptr->state = RR_L1_STATE_START_EARLY_CAMP;
                  break;
                } // RR_L1_MODE_IMSG_EARLY_CAMP_REQ

                case RR_L1_IDLE_IMSG_START_IDLE_REQ:
                {
                  boolean is_early_camped;

                  if (rr_l1_send_get_l1_idle_status(gas_id) == RR_L1_IDLE_STATUS_EARLY_CAMP_IDLE)
                  {
                    is_early_camped = TRUE;
                  }
                  else
                  {
                    is_early_camped = FALSE;
                  }

                  rr_l1_start_idle(is_early_camped, gas_id);

                  // Wait for MPH_START_IDLE_MODE_CNF
                  rr_l1_idle_data_ptr->state = RR_L1_STATE_START_IDLE;
                  break;
                } // RR_L1_MODE_IMSG_START_IDLE_REQ

                case RR_L1_IDLE_IMSG_CHANGE_IDLE_REQ:
                {
                  // Abort any pending reselection activity
                  rr_resel_send_imsg_abort_req(
                    RR_RESEL_ABORT_REASON_MM_REQ,   // reason
                    gas_id                          // gas_id
                  );

                  rr_l1_idle_data_ptr->state = RR_L1_STATE_CHANGE_IDLE;
                  break;
                } // RR_L1_IDLE_IMSG_CHANGE_IDLE_REQ

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_l1_idle_state_name(rr_l1_idle_data_ptr->state)
                  );
                }
              } // message_id
              break;
            } // RR_L1_MODE_IMSG

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_l1_idle_state_name(rr_l1_idle_data_ptr->state)
              );
            }
          } // rr_message_set
          break;
        } // MS_RR_RR

        case MS_RR_L1:
        {
          switch (msg_ptr->message_header.message_id)
          {
            case MPH_START_IDLE_MODE_CNF:
            {
              MSG_GERAN_HIGH_0_G("MPH_START_IDLE_MODE_CNF in NULL state");
              break;
            }

            case MPH_ABORT_RA_CNF:
            {
              if (GL1_IN_IDLE_MODE == msg_ptr->mph_abort_ra_cnf.l1_mode)
              {
                /* send RR_L1_IDLE_OMSG_START_IDLE_CNF */
                rr_send_internal_start_idle_cnf(gas_id);
              }
              else
              {
                MSG_GERAN_HIGH_1_G("Received MPH_ABORT_RA_CNF with mode %d",
                                   (int)(msg_ptr->mph_abort_ra_cnf.l1_mode));
              }
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_l1_idle_state_name(rr_l1_idle_data_ptr->state)
              );
            }
          } // message_id
          break;
        } // MS_RR_L1

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_l1_idle_state_name(rr_l1_idle_data_ptr->state)
          );
        }
      } // message_set
      break;
    } // RR_L1_IDLE_NULL

    case RR_L1_STATE_START_EARLY_CAMP:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_L1:
        {
          switch (msg_ptr->message_header.message_id)
          {
            case MPH_ABORT_RA_CNF:
            {
              if (GL1_IN_IDLE_MODE != msg_ptr->mph_abort_ra_cnf.l1_mode)
              {
                MSG_GERAN_HIGH_1_G(
                  "Received MPH_ABORT_RA_CNF with mode %d",
                  (int)(msg_ptr->mph_abort_ra_cnf.l1_mode)
                );
                return;
              }
            }
            /* deliberate fallthrough, GL1_IN_IDLE_MODE handled same as START_IDLE_MODE_CNF */
            case MPH_START_IDLE_MODE_CNF:
            {
              MSG_GERAN_MED_0_G("L1 confirms EARLY CAMP");

              rr_l1_idle_send_omsg_start_early_camp_cnf(gas_id);
              rr_l1_idle_data_ptr->state = RR_L1_STATE_NULL;
              break;
            } // MPH_START_IDLE_MODE_CNF

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_l1_idle_state_name(rr_l1_idle_data_ptr->state)
              );
            }
          } // message_id
          break;
        } // MS_RR_L1

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_l1_idle_state_name(rr_l1_idle_data_ptr->state)
          );
        }
      } // message_set
      break;
    } // RR_L1_STATE_EARLY_CAMPED

    case RR_L1_STATE_START_IDLE:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_L1:
        {
          switch (msg_ptr->message_header.message_id)
          {
            case MPH_ABORT_RA_CNF:
            {
              if (GL1_IN_IDLE_MODE != msg_ptr->mph_abort_ra_cnf.l1_mode)
              {
                MSG_GERAN_HIGH_1_G(
                  "Received MPH_ABORT_RA_CNF with mode %d",
                  (int)(msg_ptr->mph_abort_ra_cnf.l1_mode)
                );
                return;
              }
            } // MPH_ABORT_RA_CNF
            /* deliberate fallthrough, GL1_IN_IDLE_MODE handled same as START_IDLE_MODE_CNF */
            case MPH_START_IDLE_MODE_CNF:
            {
              // Perform actions resulting from IDLE confirmation and send
              // RR_L1_IDLE_OMSG_START_IDLE_CNF
              rr_l1_idle_confirmed(gas_id);

              // Reset internal variables and change state to RR_L1_STATE_NULL
              rr_l1_idle_init(gas_id);

#ifdef FEATURE_IDLE_DRX_SCALING
              rr_idle_drx_control(RR_EV_CAMPED_ON_CELL, NULL, gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */
              break;
            } // MPH_START_IDLE_MODE_CNF

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_l1_idle_state_name(rr_l1_idle_data_ptr->state)
              );
            }
          } // message_id
          break;
        } // MS_RR_L1

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_l1_idle_state_name(rr_l1_idle_data_ptr->state)
          );
        }
      } // message_set
      break;
    } // RR_L1_STATE_START_IDLE

    case RR_L1_STATE_CHANGE_IDLE:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_RESEL_OMSG:
            {
              switch (msg_ptr->message_header.message_id)
              {
                case RR_RESEL_OMSG_ABORT_CNF:
                {
                  // Any pending reselection has been aborted.
                  // GRR main state is GRR_START_L1_IDLE, where measurements from L1 are ignored, so no new
                  // reselection activity will be triggered

                  // Block PS access (which may already be blocked)
                  rr_ps_send_imsg_no_ps_access_req(
                    FALSE,     // suspend_gmm
                    DEL_ALL,   // delete_pdu
                    gas_id     // gas_id
                  );
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_l1_idle_state_name(rr_l1_idle_data_ptr->state)
                  );
                }
              } // message_id
              break;
            } // RR_RESEL_OMSG

            case RR_PS_OMSG:
            {
              switch (msg_ptr->message_header.message_id)
              {
                case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                {
                  // PS access has been blocked

                  boolean is_early_camped = FALSE;

                  if (rr_l1_send_get_l1_idle_status(gas_id) == RR_L1_IDLE_STATUS_EARLY_CAMP_IDLE)
                  {
                    is_early_camped = TRUE;
                  }

                  // Start Idle / Packet Idle in L1
                  rr_l1_start_idle(
                    is_early_camped,   // early_camping_performed
                    gas_id             // gas_id
                  );

                  // Wait for MPH_START_IDLE_MODE_CNF
                  rr_l1_idle_data_ptr->state = RR_L1_STATE_START_IDLE;

                  // Note: It is up to the receiver of the RR_L1_IDLE_OMSG_START_IDLE_CNF to update MM
                  // with an updated cell capability, if required
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_l1_idle_state_name(rr_l1_idle_data_ptr->state)
                  );
                }
              } // message_id
              break;
            } // RR_PS_OMSG

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_l1_idle_state_name(rr_l1_idle_data_ptr->state)
              );
            }
          } // rr_message_set
          break;
        } // MS_RR_RR

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_l1_idle_state_name(rr_l1_idle_data_ptr->state)
          );
        }
      } // message_set
      break;
    } // RR_L1_STATE_CHANGE_IDLE

    default:
    {
      ERR_GERAN_FATAL_1_G("rr_l1_idle_control undefined state %d", rr_l1_idle_data_ptr->state);
    }
  }

#ifdef DEBUG_RR_TRACE_MSG
  rr_store_trace_msg_buf(
    RR_L1_IDLE_SM,
    EV_INPUT_MESSAGE,
    (byte) rr_l1_idle_data_ptr->state,
    msg_ptr,
    gas_id
  );
#endif

  if (rr_l1_idle_data_ptr->state != rr_l1_idle_data_ptr->old_state)
  {
    RR_LOG_STATE_TRANSITION(
      "rr_l1_idle_control",
      rr_l1_idle_data_ptr->old_state,
      rr_l1_idle_data_ptr->state,
      rr_l1_idle_state_name,
      gas_id
    );

    rr_l1_idle_data_ptr->old_state = rr_l1_idle_data_ptr->state;
  }
}


/**
  @brief Initialisation function. Resets internal variables and the state machine
*/
void rr_l1_idle_init(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_l1_idle_data_t *rr_l1_idle_data_ptr = rr_l1_idle_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_l1_idle_data_ptr);

  rr_l1_idle_data_ptr->state = RR_L1_STATE_NULL;
  rr_l1_idle_data_ptr->old_state = RR_L1_STATE_NULL;
}


/**
  @brief Initialisation function. Should be called once at RR task start
*/
void rr_l1_idle_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(rr_l1_idle_data, 0, sizeof(rr_l1_idle_data));

  // Setup any specific values not setup by the general module initialisation function
}

/* EOF */

