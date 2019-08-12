/*============================================================================
  @file rr_g2x_redir.c

  @brief This module implements a state machine that handles Inter-RAT
         Redirection from GSM.

                Copyright (c) 2009-2014 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_g2x_redir.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS_G2T_REDIRECTION)

#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_g2x_redir.h"
#include "gprs_mem.h"
#include "rr_mode.h"
#include "rr_gprs_debug.h"
#include "rr_general.h"
#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
#include "rr_rrc_send.h"
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */
#include "rr_multi_sim.h"
#include "rr_msgr.h"
#include "rr_g2x.h"

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"
#include "rr_tds_send.h"
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/
#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
#include "rr_ded_meas_reporting.h"
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/
#ifdef FEATURE_SGLTE
#include "rr_ps_domain.h"
#include "rr_l1_send.h"
#include "rr_sglte.h"
#endif /*FEATURE_SGLTE*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef enum
{
  RR_G2X_REDIR_STATE_NULL,
  RR_G2X_REDIR_STATE_DEACTIVATING,
  RR_G2X_REDIR_STATE_DEACTIVATING_ABORTED,
  RR_G2X_REDIR_STATE_REDIRECTING,
  RR_G2X_REDIR_STATE_REDIRECTING_ABORTED,
#ifdef FEATURE_SGLTE
  RR_G2X_REDIR_STATE_SGLTE_MODE_CHANGE,
  RR_G2X_REDIR_STATE_SGLTE_FAILED_MODE_CHANGE,
  RR_G2X_REDIR_STATE_SGLTE_ABORTED_MODE_CHANGE,
#endif /*FEATURE_SGLTE*/
  RR_G2X_REDIR_STATE_MAX
} rr_g2x_redir_state_e;

typedef struct
{
  rr_g2x_redir_state_e       state;
  rr_g2x_redir_state_e       old_state;
  rr_cell_selection_target_e type;
  rr_cell_selection_target_u target;
} rr_g2x_redir_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

static rr_g2x_redir_data_t * rr_g2x_redir_data_ptrs[NUM_GERAN_DATA_SPACES];

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
static rr_g2x_redir_data_t *rr_g2x_redir_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return rr_g2x_redir_data_ptrs[as_index];
}

const char * rr_g2x_redir_state_name(rr_g2x_redir_state_e state)
{
  switch (state)
  {
    case RR_G2X_REDIR_STATE_NULL:
      return "NULL";

    case RR_G2X_REDIR_STATE_DEACTIVATING:
      return "DEACTIVATING";

    case RR_G2X_REDIR_STATE_DEACTIVATING_ABORTED:
      return "DEACTIVATING_ABORTED";

    case RR_G2X_REDIR_STATE_REDIRECTING:
      return "REDIRECTING";

    case RR_G2X_REDIR_STATE_REDIRECTING_ABORTED:
      return "REDIRECTING_ABORTED";

#ifdef FEATURE_SGLTE
    case RR_G2X_REDIR_STATE_SGLTE_MODE_CHANGE:
      return "SGLTE_MODE_CHANGE";

    case RR_G2X_REDIR_STATE_SGLTE_FAILED_MODE_CHANGE:
      return "SGLTE_FAILED_MODE_CHANGE";

    case RR_G2X_REDIR_STATE_SGLTE_ABORTED_MODE_CHANGE:
      return "SGLTE_ABORTED_MODE_CHANGE";
#endif /*FEATURE_SGLTE*/

    default:
      MSG_GERAN_ERROR_1("Unexpected state: %d", state);
      return "?";
  }
}


/*!
 * \brief Maps the Redirection Target Type to destination RAT.
 *
 * \param cell_selection_target
 *
 * \return rr_rat_e (The RAT Type)
 */

static rr_rat_e rr_g2x_redir_map_redir_type_to_target_rat( rr_cell_selection_target_e type  )
{
  rr_rat_e rat_type;
  
  switch(type)
  {
    case GSM_TARGET:
      rat_type = RR_RAT_GSM;
      MSG_GERAN_ERROR_1("rr_g2x_redir_map_redir_type_to_target_rat Unexpected rat: %d", rat_type);
    break;

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE    
    case FDD_TARGET:
      rat_type = RR_RAT_WCDMA;
    break;
#endif

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION    
    case TDD_TARGET:
      rat_type = RR_RAT_UTRAN_TDD;        
    break;
#endif

#ifdef FEATURE_LTE    
    case LTE_TARGET:
      rat_type = RR_RAT_LTE;
    break;
#endif

    default:
      rat_type = RR_RAT_UNDEFINED;
      MSG_GERAN_ERROR_1("rr_g2x_redir_map_redir_type_to_target_rat Unexpected rat: %d", rat_type);
    break;
  }

  return(rat_type);
}

/**
  @brief Wrapper function for L1 mode control state machine.

  Requests the L1 mode control state machine to deactivate L1.  If L1 is
  already suspended then this function will result in an
  RR_G2X_REDIR_IMSG_L1_DEACTIVATED_IND being sent immediately.
*/
static void rr_g2x_redir_deactivate_l1(const gas_id_t gas_id)
{
  rr_rat_e target_rat;
  // Obtain a pointer to the module data
  rr_g2x_redir_data_t* rr_g2x_redir_data_ptr = rr_g2x_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_redir_data_ptr);

  target_rat = rr_g2x_redir_map_redir_type_to_target_rat(rr_g2x_redir_data_ptr->type);
  
  if (rr_g2x_lock_exchange_allowed(RR_G2X_OPERATION_TYPE_REDIRECTION, target_rat, gas_id))
  {
    /* advise GL1 to exchange rather than release this client */
    /* which occurs when the GDrivers stop the physical layer */
    geran_set_exchange_on_release(GERAN_EXCHANGE_GTOX, gas_id);
  }
  else
  {
    geran_set_exchange_on_release(GERAN_EXCHANGE_NULL, gas_id);
  }

  /* Set the global stop reason to RR_L1_RAT_CHANGE as we are switching RAT. */
  rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

  /* and now deactivate Layer 1 */
  if (EV_GSM_MODE_DEACTIVATED == rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id))
  {
    rr_g2x_redir_send_imsg(RR_G2X_REDIR_IMSG_L1_DEACTIVATED_IND, gas_id);
  }

  return;
}

static boolean rr_g2x_redir_process_redir_req(
  rr_g2x_redir_imsg_redir_req_t * redir_req,
  const gas_id_t gas_id
)
{
  rr_g2x_redir_data_t *rr_g2x_redir_data_ptr;

  RR_NULL_CHECK_RETURN_PARAM(redir_req, FALSE);

  // Obtain a pointer to the module data
  rr_g2x_redir_data_ptr = rr_g2x_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_redir_data_ptr);

  if (redir_req->csi.valid)
  {
    switch (redir_req->csi.type)
    {
#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
      case FDD_TARGET:
      {
        rr_log_g2w_redir_started(gas_id);
        rr_g2x_redir_data_ptr->type = FDD_TARGET;
        rr_g2x_redir_data_ptr->target.fdd_target = redir_req->csi.target.fdd_target;
        return TRUE;
      } /* FDD_TARGET */
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
      case TDD_TARGET:
      {
        rr_log_g2t_redir_started(gas_id);
        rr_g2x_redir_data_ptr->type = TDD_TARGET;
        rr_g2x_redir_data_ptr->target.tdd_target = redir_req->csi.target.tdd_target;
        return TRUE;
      } /* FDD_TARGET */
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

#ifdef FEATURE_LTE
      case LTE_TARGET:
      {
        rr_log_g2l_redir_started(gas_id);
        rr_g2x_redir_data_ptr->type = LTE_TARGET;
        rr_g2x_redir_data_ptr->target.lte_target = redir_req->csi.target.lte_target;
        return TRUE;
      } /* LTE_TARGET */
#endif /* FEATURE_LTE */

      default:
      {
        MSG_GERAN_HIGH_1_G("Unsupported target type %d in cell selection indicator, redirection failed",
                 redir_req->csi.type);
        return FALSE;
      }
    }
  }
  else
  {
    MSG_GERAN_HIGH_0("Cell selection indicator not valid, redirection failed");
    return FALSE;
  }
}

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
/*===========================================================================

FUNCTION rr_tds_pseudo_fr_uarfcn_in_list

DESCRIPTION
  Searches for a UARFCN match in the supplied list.

PARAMS
  tdsrrc_target_info_type freq_list - list to search
  uint16 uarfcn - UARFCN to find

RETURN VALUE
  TRUE - match found
  FALSE - match not found

===========================================================================*/
static boolean rr_tds_pseudo_fr_uarfcn_in_list(tdsrrc_target_info_type *freq_list_ptr, uint16 uarfcn)
{
  boolean freq_in_list = FALSE;
  uint8 i = 0;

  for (i=0;i<freq_list_ptr->num_of_frequencies;i++)
  {
    if (freq_list_ptr->fdd_cell_list[i].uarfcn == uarfcn)
    {
      freq_in_list = TRUE;
      break;
    }
  }

  return freq_in_list;
}

/*===========================================================================

FUNCTION rr_tds_pseudo_fr_populate_freq_list

DESCRIPTION
  Formats a frequency list for Pseudo FR (G to T Redir).
  The frequency list can only contain frequencies supplied in the Pseudo FR
  frequency list from the T->G HO cmd.  Based on this list the return frequency
  list (G->T Redir) is formatted as follows:
  1) In RSCP order based on TDS measurements greater than the supplied Pseudo FR RSCP threshold.
      (note UARFCN for TDS measurements less than threshold are excluded from the list)
  2) Remaining frequencies in Pesudo FR freq list that have not been measured.

PARAMS
  tdsrrc_target_info_type *freq_list_ptr - Frequency list for population

RETURN VALUE
  None

===========================================================================*/
void rr_tds_pseudo_fr_populate_freq_list(
  tdsrrc_target_info_type *freq_list_ptr,
  const gas_id_t gas_id
)
{
  rr_ded_irat_measurements_t *tds_ded_meas_ptr = rr_get_tds_ded_meas_ptr(gas_id);
  rr_dedicated_data_T        *dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  uint8 i = 0;

  memset(freq_list_ptr, 0, sizeof (tdsrrc_target_info_type));

  /* sort TDS dedicated measurements in RSCP order (greatest first)*/
  qsort(
    (void *) tds_ded_meas_ptr->entries,
    tds_ded_meas_ptr->no_of_entries,
    sizeof (rr_ded_irat_measurement_t),
    rr_compare_tds_rscp_measurements
  );

  if (tds_ded_meas_ptr->no_of_entries > ARR_SIZE(tds_ded_meas_ptr->entries))
  {
    MSG_GERAN_ERROR_2_G("RR Pseudo FR TDS measurements no:%d greater than stored array:%d",tds_ded_meas_ptr->no_of_entries,
                                                                                 ARR_SIZE(tds_ded_meas_ptr->entries));
    tds_ded_meas_ptr->no_of_entries = ARR_SIZE(tds_ded_meas_ptr->entries);
  }

  /* Add measured TDS UARFCN's in RSCP order */
  for (i=0; i<tds_ded_meas_ptr->no_of_entries; i++)
  {
    if (rr_tds_pseudo_fr_uarfcn_in_list(freq_list_ptr, tds_ded_meas_ptr->entries[i].data.utran_tdd.uarfcn) == FALSE)
    {
      if (tds_ded_meas_ptr->entries[i].data.utran_tdd.rscp > dedic_ptr->tds_pseudo_fr.rscp_threshold)
      {
        if (freq_list_ptr->num_of_frequencies >= ARR_SIZE(freq_list_ptr->fdd_cell_list) )
        {
          MSG_GERAN_ERROR_3_G("RR Pseudo FR Unexpected Freq list full, meas entries:%d, freq list entries:%d, pfr entries:%d",
                                                                                          tds_ded_meas_ptr->no_of_entries,
                                                                                          freq_list_ptr->num_of_frequencies,
                                                                                          dedic_ptr->tds_pseudo_fr.num_freq);
          break;
        }

        /*Add the UARFCN to the list*/
        freq_list_ptr->fdd_cell_list[freq_list_ptr->num_of_frequencies].uarfcn = tds_ded_meas_ptr->entries[i].data.utran_tdd.uarfcn;

        freq_list_ptr->fdd_cell_list[freq_list_ptr->num_of_frequencies].ts0_pos = tds_ded_meas_ptr->entries[i].data.utran_tdd.ts0_pos;

        freq_list_ptr->fdd_cell_list[freq_list_ptr->num_of_frequencies].num_of_cells = 0;

        freq_list_ptr->num_of_frequencies++;
      }
    }
  }

  /* Add Pseudo FR frequencies which have not been measured to the end of the list */
  for (i=0; i<dedic_ptr->tds_pseudo_fr.num_freq; i++)
  {
    if (dedic_ptr->tds_pseudo_fr.freq_list[i].measured == FALSE)
    {
      if (freq_list_ptr->num_of_frequencies >= ARR_SIZE(freq_list_ptr->fdd_cell_list) )
      {
        MSG_GERAN_ERROR_3_G("RR Pseudo FR Unexpected Freq list full, meas entries:%d, freq list entries:%d, pfr entries:%d",
                                                                                          tds_ded_meas_ptr->no_of_entries,
                                                                                          freq_list_ptr->num_of_frequencies,
                                                                                          dedic_ptr->tds_pseudo_fr.num_freq);
        break;
      }

      /*Add the UARFCN to the list*/
      freq_list_ptr->fdd_cell_list[freq_list_ptr->num_of_frequencies].uarfcn = dedic_ptr->tds_pseudo_fr.freq_list[i].uarfcn;

      freq_list_ptr->fdd_cell_list[freq_list_ptr->num_of_frequencies].ts0_pos = TDS_TS0_POSITION_INVALID;

      freq_list_ptr->fdd_cell_list[freq_list_ptr->num_of_frequencies].num_of_cells = 0;

      freq_list_ptr->num_of_frequencies++;
    }
  }

  MSG_GERAN_HIGH_1_G("RR TDS Pseudo FR Added %d UARFCN's to G->T redir freq list",freq_list_ptr->num_of_frequencies);

  /*debug output*/
  for (i=0; i<freq_list_ptr->num_of_frequencies; i++)
  {
    MSG_GERAN_HIGH_2_G("UARFCN %d, TS0_Pos: %d",freq_list_ptr->fdd_cell_list[i].uarfcn,
                                      freq_list_ptr->fdd_cell_list[i].ts0_pos);
  }
}
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE

static void rr_g2x_redir_send_wcdma_redir_req(
  rr_rrc_fdd_target_info_type * fdd_target_info,
  const gas_id_t gas_id
)
{
  rr_send_interrat_gtow_redirection_req(
    fdd_target_info,
    rr_rat_pri_list_info(gas_id),
    gas_id
  );
}

#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_LTE

static void rr_g2x_redir_send_lte_redir_req(
  rr_lte_target_info_type * lte_target_info,
  const gas_id_t gas_id
)
{
  lte_rrc_G_redir_req_s          redir_req;
  errno_enum_type                result;
  rr_PLMN_request_details_T    * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  sys_rat_pri_list_info_s_type * rat_pri_list_info = rr_rat_pri_list_info(gas_id);
  uint32                         i;
  uint8                          redir_info_list_count = 0;
  lte_rrc_geran_redir_info_s   * geran_redir_info;

  memset(&redir_req, 0, sizeof(redir_req));

  msgr_init_hdr(
    &redir_req.msg_hdr,
    MSGR_GERAN_GRR,
    LTE_RRC_G_REDIR_REQ
  );

  /* redir_list_info */
  for (i = 0;
       i < lte_target_info->eutran_descriptions_count &&
       i < MAX_CSI_EUTRAN_DESCRIPTIONS_COUNT;
       ++i)
  {
    rr_csi_eutran_description_t * eutran_description = &lte_target_info->eutran_descriptions[i];

    if (redir_req.redir_list_info.redir_info_list_count < LTE_RRC_IRAT_MAX_REDIR_FREQ)
    {
      geran_redir_info = &redir_req.redir_list_info.redir_info[redir_info_list_count++];

      if (eutran_description->measurement_bandwidth_valid)
      {
        geran_redir_info->meas_bandwidth_is_present = TRUE;
        geran_redir_info->meas_bandwidth = eutran_description->measurement_bandwidth;
      }
      else
      {
        geran_redir_info->meas_bandwidth_is_present = FALSE;
      }

      geran_redir_info->earfcn = eutran_description->earfcn;

      if (eutran_description->target_pcid_valid)
      {
        geran_redir_info->phy_cell_id_is_present = TRUE;
        geran_redir_info->phy_cell_id = eutran_description->target_pcid;
      }
      else
      {
        geran_redir_info->phy_cell_id_is_present = FALSE;
      }

      if (eutran_description->not_allowed_cells_valid)
      {
        int i;
        for (i = 0;
             i <= RR_LTE_PCID_MAX &&
             geran_redir_info->blacklisted_cell_cnt < LTE_RRC_IRAT_MAX_EUTRA_CELLS_PER_FREQ;
             ++i)
        {
          if (RR_PCID_GROUP_IE_GET(eutran_description->not_allowed_cells.bitmap, i))
          {
            geran_redir_info->blacklisted_cell_list[geran_redir_info->blacklisted_cell_cnt++] = i;
          }
        }
      }
    }
  }
  redir_req.redir_list_info.redir_info_list_count = redir_info_list_count;

  /* nbr_list_info */
  /* TODO */
  redir_req.nbr_list_info.earfcn_count = 0;

  /* nas_params */
  redir_req.nas_params.rat_pri_list = *rat_pri_list_info;
  if (!rr_is_any_cell_selection_mode(gas_id))
  {
    redir_req.nas_params.req_plmn = plmn_request_details->requested_PLMN_id;
  }
  redir_req.nas_params.network_select_mode = plmn_request_details->network_select_mode;

  /* dedicated_priority_info */
  rr_dedicated_priority_info_get(&redir_req.dedicated_priority_info, gas_id);

  if (lte_target_info->blind_redir)
  {
    redir_req.redir_is_nw_initiated = FALSE;
  }
  else
  {
    redir_req.redir_is_nw_initiated = TRUE;
  }

  result = rr_msgr_send(&redir_req.msg_hdr, sizeof(redir_req), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}

#endif /* FEATURE_LTE */

/**
  @brief Sends an appropriate diag event to indicate that the redirection
         has been successful.
*/
static void rr_g2x_redir_log_success(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_g2x_redir_data_t *rr_g2x_redir_data_ptr = rr_g2x_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_redir_data_ptr);

  switch (rr_g2x_redir_data_ptr->type)
  {
#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
    case FDD_TARGET:
    {
      rr_log_g2w_redir_success(gas_id);
      break;
    } /* FDD_TARGET */
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
    case TDD_TARGET:
    {
      rr_log_g2t_redir_success(gas_id);
      break;
    } /* TDD_TARGET */
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

#ifdef FEATURE_LTE
    case LTE_TARGET:
    {
      rr_log_g2l_redir_success(gas_id);
      break;
    } /* LTE_TARGET */
#endif /* FEATURE_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected target type: %d",
                rr_g2x_redir_data_ptr->type);
    }
  }
}

/**
  @brief Sends an appropriate diag event to indicate that the redirection
         has been aborted.
*/
static void rr_g2x_redir_log_aborted(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_g2x_redir_data_t *rr_g2x_redir_data_ptr = rr_g2x_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_redir_data_ptr);

  switch (rr_g2x_redir_data_ptr->type)
  {
#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
    case FDD_TARGET:
    {
      rr_log_g2w_redir_aborted(gas_id);
      break;
    } /* FDD_TARGET */
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
    case TDD_TARGET:
    {
      rr_log_g2t_redir_aborted(gas_id);
      break;
    } /* TDD_TARGET */
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

#ifdef FEATURE_LTE
    case LTE_TARGET:
    {
      rr_log_g2l_redir_aborted(gas_id);
      break;
    } /* LTE_TARGET */
#endif /* FEATURE_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected target type: %d",
                rr_g2x_redir_data_ptr->type);
    }
  }
}

/**
  @brief Sends an appropriate diag event to indicate that the redirection
         has failed.
*/
static void rr_g2x_redir_log_failure(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_g2x_redir_data_t *rr_g2x_redir_data_ptr = rr_g2x_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_redir_data_ptr);

  switch (rr_g2x_redir_data_ptr->type)
  {
#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
    case FDD_TARGET:
    {
      rr_log_g2w_redir_failure(gas_id);
      break;
    } /* FDD_TARGET */
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
    case TDD_TARGET:
    {
      rr_log_g2t_redir_failure(gas_id);
      break;
    } /* TDD_TARGET */
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

#ifdef FEATURE_LTE
    case LTE_TARGET:
    {
      rr_log_g2l_redir_failure(gas_id);
      break;
    } /* LTE_TARGET */
#endif /* FEATURE_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected target type: %d",
                rr_g2x_redir_data_ptr->type);
    }
  }
}

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

void rr_g2x_redir_init(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  if (rr_g2x_redir_data_ptrs[as_index] == NULL)
  {
    rr_g2x_redir_data_t * local_ptr;

    local_ptr = (rr_g2x_redir_data_t *) GPRS_MEM_CALLOC(sizeof(rr_g2x_redir_data_t));
    RR_NULL_CHECK_FATAL(local_ptr);

    /* initialise and store the pointer in the array of data spaces */
    rr_g2x_redir_data_ptrs[as_index] = local_ptr;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rr_g2x_redir_data already allocated, clearing");
    memset(rr_g2x_redir_data_ptrs[as_index], 0, sizeof(rr_g2x_redir_data_t));
  }
}

void rr_g2x_redir_deinit(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  if (rr_g2x_redir_data_ptrs[as_index] != NULL)
  {
    GPRS_MEM_FREE(rr_g2x_redir_data_ptrs[as_index]);
    rr_g2x_redir_data_ptrs[as_index] = NULL;
  }
}

void rr_g2x_redir_control(
  rr_cmd_bdy_type * message,
  const gas_id_t gas_id
)
{
  rr_g2x_redir_data_t *rr_g2x_redir_data_ptr;

  /* Validate input */
  RR_NULL_CHECK_RETURN_VOID(message);

  // Obtain a pointer to the module data
  rr_g2x_redir_data_ptr = rr_g2x_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_redir_data_ptr);

  switch (rr_g2x_redir_data_ptr->state)
  {
    case RR_G2X_REDIR_STATE_NULL:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_G2X_REDIR_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_G2X_REDIR_IMSG_REDIR_REQ:
                {
                  boolean redir_ok = rr_g2x_redir_process_redir_req(
                    &message->rr.g2x_redir.imsg.redir_req,
                    gas_id
                  );

                  if (redir_ok)
                  {
#ifdef FEATURE_SGLTE
                    if (rr_g2x_redir_data_ptr->target.lte_target.redirection_for_sglte)
                    {
                      MSG_GERAN_HIGH_0_G("Performing redirection in SGLTE mode");
                      rr_send_mph_ue_mode_change_req(TRUE, gas_id);
                      rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_SGLTE_MODE_CHANGE;
                    }
                    else
#endif /*FEATURE_SGLTE*/
                    {
		      rr_g2x_redir_deactivate_l1(gas_id);
                      rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_DEACTIVATING;
                    }
                  }
                  else
                  {
                    rr_g2x_redir_send_omsg_redir_cnf(
                      RR_G2X_REDIR_STATUS_FAILURE,
                      RR_G2X_REDIR_FAILURE_ACTION_OOS,
                      gas_id
                    );
                  }
                  break;
                } /* RR_G2X_REDIR_IMSG_REDIR_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "NULL");
                }
              }
              break;
            } /* RR_G2X_REDIR_IMSG */

#ifdef FEATURE_SGLTE
            case RR_IMSG:
            {
              if (message->message_header.message_id == RR_IMSG_PS_ACCESS_IND)
              {
                /*ignore*/
              }
              break;
            }
#endif /*FEATURE_SGLTE*/

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
    } /* RR_G2X_REDIR_STATE_NULL */

#ifdef FEATURE_SGLTE
    case RR_G2X_REDIR_STATE_SGLTE_MODE_CHANGE:
    {
      if (message->message_header.message_set == MS_RR_L1 &&
          message->message_header.message_id == MPH_UE_MODE_CHANGE_CNF)
      {

        MSG_HIGH("MPH_UE_MODE_CHANGE_CNF rxd for SGLTE redirection",0,0,0);

        rr_g2wr_reset_wcdma_surrounding_database(gas_id);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
        rr_g2wr_cancel_all_wcdma_five_second_timers(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

        rr_g2x_redir_send_lte_redir_req(
          &rr_g2x_redir_data_ptr->target.lte_target,
          gas_id 
        );

        rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_REDIRECTING;
      } /* MPH_UE_MODE_CHANGE_CNF */
      else
      {
        LOG_UNEXPECTED_MESSAGE(message, "SGLTE_MODE_CHANGE");
      }
      break;
    }
#endif /*FEATURE_SGLTE*/

    case RR_G2X_REDIR_STATE_DEACTIVATING:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_G2X_REDIR_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_G2X_REDIR_IMSG_L1_DEACTIVATED_IND:
                {
                  switch (rr_g2x_redir_data_ptr->type)
                  {
#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
                    case FDD_TARGET:
                    {
                      rr_g2x_redir_send_wcdma_redir_req(
                        &rr_g2x_redir_data_ptr->target.fdd_target,
                        gas_id
                      );
                      rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_REDIRECTING;
                      break;
                    } /* FDD_TARGET */
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
                    case TDD_TARGET:
                    {
                      boolean pseudo_fr = FALSE;

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
                      pseudo_fr = rr_g2x_redir_data_ptr->target.tdd_target.pseudo_fr_active;
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

                      rr_send_tds_interrat_redirection_req(
                        &rr_g2x_redir_data_ptr->target.tdd_target.target,
                        rr_rat_pri_list_info(gas_id),
                        pseudo_fr,
                        gas_id
                      );
                      rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_REDIRECTING;
                      break;
                    } /* FDD_TARGET */
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

#ifdef FEATURE_LTE
                    case LTE_TARGET:
                    {
                      rr_g2x_redir_send_lte_redir_req(
                        &rr_g2x_redir_data_ptr->target.lte_target,
                        gas_id
                      );
                      rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_REDIRECTING;
                      break;
                    } /* LTE_TARGET */
#endif /* FEATURE_LTE */

                    default:
                    {
                      MSG_GERAN_ERROR_1_G("Unexpected target type: %d",
                                rr_g2x_redir_data_ptr->type);
                      rr_g2x_redir_send_omsg_redir_cnf(
                        RR_G2X_REDIR_STATUS_FAILURE,
                        RR_G2X_REDIR_FAILURE_ACTION_OOS,
                        gas_id
                      );
                      rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;
                    }
                  }
                  break;
                } /* RR_G2X_REDIR_IMSG_L1_DEACTIVATED_IND */

                case RR_G2X_REDIR_IMSG_ABORT_REQ:
                {
                  rr_g2x_redir_log_aborted(gas_id);
                  rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_DEACTIVATING_ABORTED;
                  break;
                } /* RR_G2X_REDIR_IMSG_ABORT_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING");
                }
              }
              break;
            } /* RR_G2X_REDIR_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING");
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_MM_RR:
        {
          switch (message->message_header.message_id)
          {
            case RR_STOP_GSM_MODE_REQ:
            {
              rr_g2x_redir_log_aborted(gas_id);
              rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_DEACTIVATING_ABORTED;
              break;
            } /* RR_STOP_GSM_MODE_REQ */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING");
            }
          }
          break;
        } /* MS_MM_RR */

        case MS_RR_L1:
        {
          switch (message->message_header.message_id)
          {
            case MPH_SERVING_IDLE_MEAS_IND:
            case MPH_SURROUND_MEAS_IND:
            case MPH_SURROUND_IRAT_MEAS_IND:
              /*ignore in this state*/
            break;

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING");
            }
          }
          break;
        } /*MS_RR_L1*/
        
        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING");
        }
      }
      break;
    } /* RR_G2X_REDIR_STATE_DEACTIVATING */

    case RR_G2X_REDIR_STATE_DEACTIVATING_ABORTED:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_G2X_REDIR_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_G2X_REDIR_IMSG_L1_DEACTIVATED_IND:
                {
                  geran_abandon_lock_exchange(GERAN_EXCHANGE_GTOX, gas_id);
                  rr_g2x_redir_send_omsg(RR_G2X_REDIR_OMSG_ABORT_CNF, gas_id);
                  rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;
                  break;
                } /* RR_G2X_REDIR_IMSG_L1_DEACTIVATED_IND */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING");
                }
              }
              break;
            } /* RR_G2X_REDIR_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING");
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "DEACTIVATING");
        }
      }
      break;
    } /* RR_G2X_REDIR_STATE_DEACTIVATING_ABORTED */

    case RR_G2X_REDIR_STATE_REDIRECTING:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_G2X_REDIR_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_G2X_REDIR_IMSG_ABORT_REQ:
                {
                  rr_g2x_redir_log_aborted(gas_id);

                  switch (rr_g2x_redir_data_ptr->type)
                  {
#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
                    case FDD_TARGET:
                    {
                      rr_send_interrat_gtow_redirection_abort_req(gas_id);
                      rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_REDIRECTING_ABORTED;
                      break;
                    } /* FDD_TARGET */
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
                    case TDD_TARGET:
                    {
                      rr_send_tds_interrat_redirection_abort_req(gas_id);
                      rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_REDIRECTING_ABORTED;
                      break;
                    } /* FDD_TARGET */
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

#ifdef FEATURE_LTE
                    case LTE_TARGET:
                    {
                      lte_rrc_G_abort_redir_req_s abort_redir_req;
                      errno_enum_type             result;

                      memset(&abort_redir_req, 0, sizeof(abort_redir_req));

                      msgr_init_hdr(
                        &abort_redir_req.msg_hdr,
                        MSGR_GERAN_GRR,
                        LTE_RRC_G_ABORT_REDIR_REQ
                      );
                      result = rr_msgr_send(&abort_redir_req.msg_hdr, sizeof(abort_redir_req), gas_id);
                      if (result != E_SUCCESS)
                      {
                        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                      }
                      rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_REDIRECTING_ABORTED;
                      break;
                    } /* LTE_TARGET */
#endif /* FEATURE_LTE */

                    default:
                    {
                      /* No way to recover here if this data is corrupted, even
                      a recovery restart will not help as we do not know which
                      RAT to abort. */
                      MSG_GERAN_FATAL_1_G("Unexpected type: %d",
                                rr_g2x_redir_data_ptr->type);
                    }
                  }
                  break;
                } /* RR_G2X_REDIR_IMSG_ABORT_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING");
                }
              }
              break;
            } /* RR_G2X_REDIR_IMSG */

#ifdef FEATURE_SGLTE
            case RR_IMSG:
            {
              if (message->message_header.message_id == RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ)
              {
                 rr_sglte_set_rat_priority_list_gsm_only(gas_id);

                 MSG_HIGH("RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ rxd for SGLTE redirection",0,0,0);

                 rr_ps_sglte_handle_ps_removal(rr_sglte_confirm_rat_change_cb_fn, gas_id);

                 /* Redirection is successfully completed. */
                 rr_g2x_redir_log_success(gas_id);
                 rr_g2x_redir_send_omsg_redir_cnf(
                   RR_G2X_REDIR_STATUS_SGLTE_SUCCESS,
                   RR_G2X_REDIR_FAILURE_ACTION_UNDEFINED,
                   gas_id
                 );

                 rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;
              }
              else if (message->message_header.message_id == RR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND)
              {
                /*no action*/
              }			  
              else
              {
                LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING");
              }
              break;
            }
#endif /*FEATURE_SGLTE*/

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING");
            }
          }
          break;
        } /* MS_RR_RR */

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
        case MS_RRC_RR:
        {
          switch (message->message_header.message_id)
          {
            case RRC_INTERRAT_REDIRECT_REJ:
            {
              rr_g2x_redir_log_failure(gas_id);
              rr_g2x_redir_send_omsg_redir_cnf(
                RR_G2X_REDIR_STATUS_FAILURE,
                RR_G2X_REDIR_FAILURE_ACTION_CELL_SELECTION,
                gas_id
              );

              /*reset Scell info in CM as it has been reset 
                by higher layers during the redirection*/
              geran_multisim_eng_mode_refresh_cm(ENG_MODE_SCELL_INFO, gas_id);

              rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;
              break;
            } /* RRC_INTERRAT_REDIRECT_REJ */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING");
            }
          }
          break;
        } /* MS_RRC_RR */
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
        case MS_TDSRRC_RR:
        {
          switch (message->message_header.message_id)
          {
            case TDSRRC_INTERRAT_REDIRECT_REJ:
            {
              rr_g2x_redir_log_failure(gas_id);

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
              if (rr_g2x_redir_data_ptr->target.tdd_target.pseudo_fr_active)
              {
                rr_g2x_redir_send_omsg_redir_cnf(
                  RR_G2X_REDIR_STATUS_FAILURE,
                  RR_G2X_REDIR_FAILURE_ACTION_CELL_SELECTION,
                  gas_id
                );
              }
              else
              {
                rr_g2x_redir_send_omsg_redir_cnf(
                  RR_G2X_REDIR_STATUS_FAILURE,
                  RR_G2X_REDIR_FAILURE_ACTION_OOS,
                  gas_id
                );
              }
#else
              rr_g2x_redir_send_omsg_redir_cnf(
                RR_G2X_REDIR_STATUS_FAILURE,
                RR_G2X_REDIR_FAILURE_ACTION_OOS,
                gas_id
              );
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

              /*reset Scell info in CM as it has been reset 
                by higher layers during the redirection*/
              geran_multisim_eng_mode_refresh_cm(ENG_MODE_SCELL_INFO, gas_id);

              rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;
              break;
            } /* TDSRRC_INTERRAT_REDIRECT_REJ */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING");
            }
          }
          break;
        } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

        case MS_MSGR_RR:
        {
          switch (message->rr_msgr_msg.message.hdr.id)
          {
#ifdef FEATURE_LTE
            case LTE_RRC_G_REDIR_FAILED_RSP:
            {
              rr_g2x_redir_log_failure(gas_id);
#ifdef FEATURE_SGLTE
              if (rr_g2x_redir_data_ptr->target.lte_target.redirection_for_sglte)
              {
                rr_send_mph_ue_mode_change_req(FALSE, gas_id);
                rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_SGLTE_FAILED_MODE_CHANGE;                
              } 
              else
#endif /*FEATURE_SGLTE*/    
              {
                 rr_g2x_redir_send_omsg_redir_cnf(
                 RR_G2X_REDIR_STATUS_FAILURE,
                 RR_G2X_REDIR_FAILURE_ACTION_CELL_SELECTION,
                 gas_id
                 );
                 rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;
              }

              /*reset Scell info in CM as it has been reset 
                by higher layers during the redirection*/
              geran_multisim_eng_mode_refresh_cm(ENG_MODE_SCELL_INFO, gas_id);

              break;
            } /* LTE_RRC_G_REDIR_FAILED_RSP */
#endif /* FEATURE_LTE */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING");
            }
          }
          break;
        } /* MS_MSGR_RR */

        case MS_MM_RR:
        {
          switch (message->message_header.message_id)
          {
            case RR_STOP_GSM_MODE_REQ:
            {
              switch (message->mm.rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason)
              {
                case RR_MM_RAT_CHANGE:
                {
                  /* Redirection is successfully completed. */
                  rr_g2x_redir_log_success(gas_id);
                  rr_g2x_redir_send_omsg_redir_cnf(
                    RR_G2X_REDIR_STATUS_SUCCESS,
                    RR_G2X_REDIR_FAILURE_ACTION_UNDEFINED,
                    gas_id
                  );
                  rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;
                  break;
                } /* RR_MM_RAT_CHANGE */

                default:
                {
                  /* Any other cause indicates that the procedure is to be
                  aborted. */
                  rr_g2x_redir_send_imsg(RR_G2X_REDIR_IMSG_ABORT_REQ, gas_id);
                }
              }
              break;
            } /* RR_STOP_GSM_MODE_REQ */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING");
            }
          }
          break;
        } /* MS_MM_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING");
        }
      }
      break;
    } /* RR_G2X_REDIR_STATE_REDIRECTING */

    /**
      REDIRECTING_ABORTED

      rr_g2x_redir_control() received and abort request while in REDIRECTING
      state.  An abort request has been forwarded to the other RAT and
      rr_g2x_redir_control() is waiting for the response.
    */
    case RR_G2X_REDIR_STATE_REDIRECTING_ABORTED:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_G2X_REDIR_IMSG:
            {
              switch (message->message_header.message_id)
              {
                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING_ABORTED");
                }
              }
              break;
            } /* RR_G2X_REDIR_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING_ABORTED");
            }
          }
          break;
        } /* MS_RR_RR */

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
        case MS_RRC_RR:
        {
          switch (message->message_header.message_id)
          {
            case RRC_INTERRAT_REDIRECT_ABORT_CNF:
            {
              rr_g2x_redir_send_omsg(RR_G2X_REDIR_OMSG_ABORT_CNF, gas_id);
              rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;
              break;
            } /* RRC_INTERRAT_REDIRECT_ABORT_CNF */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING");
            }
          }
          break;
        } /* MS_RRC_RR */
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
        case MS_TDSRRC_RR:
        {
          switch (message->message_header.message_id)
          {
            case TDSRRC_INTERRAT_REDIRECT_ABORT_CNF:
            {
              geran_abandon_lock_exchange(GERAN_EXCHANGE_XTOG, gas_id);
              rr_g2x_redir_send_omsg(RR_G2X_REDIR_OMSG_ABORT_CNF, gas_id);
              rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;
              break;
            } /* TDSRRC_INTERRAT_REDIRECT_ABORT_CNF */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING");
            }
          }
          break;
        } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

        case MS_MSGR_RR:
        {
          switch (message->rr_msgr_msg.message.hdr.id)
          {
#ifdef FEATURE_LTE
            case LTE_RRC_G_ABORT_REDIR_RSP:
            {
#ifdef FEATURE_SGLTE
              if (rr_g2x_redir_data_ptr->target.lte_target.redirection_for_sglte)
              {
                rr_send_mph_ue_mode_change_req(FALSE, gas_id);
                rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_SGLTE_ABORTED_MODE_CHANGE;
              }
              else
#endif /*FEATURE_SGLTE*/
              {
                geran_abandon_lock_exchange(GERAN_EXCHANGE_XTOG, gas_id);
                rr_g2x_redir_send_omsg(RR_G2X_REDIR_OMSG_ABORT_CNF, gas_id);
                rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;
              }              
              break;
            } /* LTE_RRC_G_ABORT_REDIR_RSP */
#endif /* FEATURE_LTE */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING");
            }
          }
          break;
        } /* MS_MSGR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, "REDIRECTING_ABORTED");
        }
      }
      break;
    } /* RR_G2X_REDIR_STATE_REDIRECTING_ABORTED */

#ifdef FEATURE_SGLTE
    case RR_G2X_REDIR_STATE_SGLTE_ABORTED_MODE_CHANGE:
    {
      
      if (message->message_header.message_set == MS_RR_L1 &&
          message->message_header.message_id == MPH_UE_MODE_CHANGE_CNF)
      {
         MSG_HIGH("MPH_UE_MODE_CHANGE_CNF rxd for SGLTE redirection",0,0,0);

         rr_g2x_redir_data_ptr->target.lte_target.redirection_for_sglte = FALSE;
         rr_l1_update_wcdma_neighbor_list(gas_id);
         rr_g2x_redir_send_omsg(RR_G2X_REDIR_OMSG_SGLTE_ABORT_CNF, gas_id);
         rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;

      } /* MPH_UE_MODE_CHANGE_CNF */
      else
      {
        LOG_UNEXPECTED_MESSAGE(message, "SGLTE_ABORTED_MODE_CHANGE");
      }
      break;
    }

    case RR_G2X_REDIR_STATE_SGLTE_FAILED_MODE_CHANGE:
    {
      
      if (message->message_header.message_set == MS_RR_L1 &&
          message->message_header.message_id == MPH_UE_MODE_CHANGE_CNF)
      {
         MSG_HIGH("MPH_UE_MODE_CHANGE_CNF rxd for SGLTE redirection",0,0,0);

         rr_g2x_redir_data_ptr->target.lte_target.redirection_for_sglte = FALSE;
         rr_l1_update_wcdma_neighbor_list(gas_id);
         rr_g2x_redir_send_omsg_redir_cnf(
           RR_G2X_REDIR_STATUS_FAILURE,
           RR_G2X_REDIR_FAILURE_ACTION_SGLTE_RETURN_TO_CAMPED,
           gas_id
         );
         rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;

      } /* MPH_UE_MODE_CHANGE_CNF */
      else
      {
        LOG_UNEXPECTED_MESSAGE(message, "SGLTE_ABORTED_MODE_CHANGE");
      }
      break;
    }
#endif /*FEATURE_SGLTE*/

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected state %d, resetting to NULL", rr_g2x_redir_data_ptr->state);
      rr_g2x_redir_data_ptr->state = RR_G2X_REDIR_STATE_NULL;
    }
  }

#ifdef DEBUG_RR_TRACE_MSG
  rr_store_trace_msg_buf(
    RR_G2X_REDIR_SM,
    EV_INPUT_MESSAGE,
    (byte) rr_g2x_redir_data_ptr->state,
    message,
    gas_id
  );
#endif

  if (rr_g2x_redir_data_ptr->state != rr_g2x_redir_data_ptr->old_state)
  {
    RR_LOG_STATE_TRANSITION(
      "rr_g2x_redir_control",
      rr_g2x_redir_data_ptr->old_state,
      rr_g2x_redir_data_ptr->state,
      rr_g2x_redir_state_name,
      gas_id
    );

    rr_g2x_redir_data_ptr->old_state = rr_g2x_redir_data_ptr->state;
  }
}

#ifdef FEATURE_SGLTE
boolean rr_sglte_g2l_blind_redirection_active(const gas_id_t gas_id)
{
  rr_g2x_redir_data_t *rr_g2x_redir_data_ptr;
  boolean              redirection_active = FALSE;

  // Obtain a pointer to the module data
  rr_g2x_redir_data_ptr = rr_g2x_redir_get_data_ptr(gas_id);
  
  if (rr_g2x_redir_data_ptr != NULL)
  {
    if ( rr_is_sglte_mode_active(gas_id) &&
         rr_g2x_redir_data_ptr->target.lte_target.redirection_for_sglte &&
         (rr_g2x_redir_data_ptr->state == RR_G2X_REDIR_STATE_REDIRECTING ||
          rr_g2x_redir_data_ptr->state == RR_G2X_REDIR_STATE_REDIRECTING_ABORTED)
       )
    {
      redirection_active = TRUE;
    }
  }

  return redirection_active;
}
#endif /* FEATURE_SGLTE */
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION */

/* EOF */

