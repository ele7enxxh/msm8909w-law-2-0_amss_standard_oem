/*! \file rr_log.c

  This module contains RR logging functionality.

                Copyright (c) 2002-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_log.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "geran_multi_sim.h"  // For geran_get_diag_sub_id()
#include "customer.h"
#include "comdef.h"
#include <memory.h>
#include <string.h>         // For memcpy()
#include "rr_seg_load.h"
#include "rr_log.h"
#include "rr_decode_ie.h"
#include "gsmdiag_v.h"      /* For gsmdiag_log_alloc() and gsmdiag_log_commit() */
#include "event.h"          // For logging RAR event and event codes
#include "rr_resel.h"
#include "rr_resel_utils.h"
#include "rr_conn_establish.h"
#ifdef FEATURE_WCDMA
#include "rr_g2w.h"
#endif /* FEATURE_WCDMA */
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
#include "rr_g2x.h"
#endif /*  FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
#include "log_codes_gsm_v.h"  // For GSM log codes
#include "rr_defs.h"
#include "rr_general.h"
#include "rr_conn_establish.h"
#include "amssassert.h"
#include "rr_shared_cell_selection.h"
#include "rr_conn_establish.h"
#include "rr_gprs_defs.h"
#include "rr_ded_config.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"
#include "rr_sys_info_cache.h"
#ifdef FEATURE_QSH_EVENT_METRIC
#include "rr_qsh_metric.h"
#endif // FEATURE_QSH_EVENT_METRIC

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

  const uint16 rr_log_codes[] = {
    LOG_GSM_RR_CELL_RESELECTION_PARAMS_C,
    LOG_GSM_RR_RACH_CONTROL_PARAMS_C,
    LOG_GSM_RR_CONTROL_CHANNEL_PARAMS_C,
    LOG_GSM_RR_CELL_OPTIONS_C,
    LOG_GSM_RR_CELL_INFO_C,
    LOG_GSM_RR_CHANNEL_CONFIGURATION_C,
    LOG_GPRS_INDICATORS_C,
    LOG_GPRS_GENERAL_PARAMS_C,
    LOG_GPRS_CELL_OPTIONS_C,
    LOG_GPRS_POWER_CONTROL_PARAMS_C,
    LOG_GPRS_MOBILE_ALLOCATION_C,
    LOG_GPRS_GRR_STATE_C,
    LOG_GPRS_RR_CELL_RESELECTION_PARAMS_C,
    LOG_GPRS_RR_CELL_RESELECTION_MEASMNTS_C,
    LOG_GPRS_RR_3G_MEASUREMENT_PARAMS_C,
    LOG_GPRS_RR_3G_RESELECTION_MEASUREMENTS_C
  };

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef struct
{
  boolean log_on_demand_registered;

  // Incremented when reselection starts; Decremented when reselection ends
  int rr_log_reselection_counter;

  // Incremented when selection starts; Decremented when selection ends
  int rr_log_selection_counter;

  boolean in_service_flag_valid;
  boolean in_service_flag;

  rr_log_reselection_trigger_T rr_log_reselection_trigger;
} rr_log_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

static rr_log_data_t rr_log_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_log_data_t
 */
static rr_log_data_t *rr_log_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_log_data[as_index];
}

/*!
 * \brief Initialises a gsmdiag_pkt_info structure ready for use with logging function.
 *
 * \param info_ptr
 * \param pkt_id
 * \param pkt_size
 * \param gas_id
 */
void rr_log_packet_init(
  gsmdiag_pkt_info_t *info_ptr,
  log_code_type pkt_id,
  unsigned int pkt_size,
  const gas_id_t gas_id
)
{
  if (info_ptr != NULL)
  {
    info_ptr->pkt_id          = pkt_id;
    info_ptr->pkt_size        = pkt_size;
#ifdef FEATURE_DUAL_SIM
    info_ptr->dual_sim_active = TRUE;
    info_ptr->active_sub_id   = 1 + GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
    info_ptr->pkt_version_id  = GSMDIAG_PKT_VERSION_DEFAULT;
#endif /*FEATURE_DUAL_SIM*/
  }

  return;
}

/*!
 * \brief Converts an arfcn structure into a word for logging.
 *
 * Almost a direct copy of L1's arfcn_to_word() in gsm\l1\l1_log.c
 *
 * \param arfcn
 *
 * \return uint16
 *         - Lower 12 bits represents the ARFCN
 *         - Upper 4 bits numerically represents the band
 *           0000 -> current 900/1800 setting if no quadband
 *           1000 -> GSM 900
 *           1001 -> DCS
 *           1010 -> PCS
 *           1011 -> GSM 850
 *           1100 -> GSM 450  (not supported)
 */
static uint16 rr_arfcn_to_word(ARFCN_T arfcn)
{
   uint16             ret_arfcn = arfcn.num & INVALID_LOG_ARFCN;
   rr_internal_band_T ret_band = (rr_internal_band_T)arfcn.band;

   switch (ret_band)
   {
      case BAND_EGSM:
      case BAND_PGSM:
         ret_arfcn |= 0x8000;
         break;

      case BAND_DCS:
         ret_arfcn |= 0x9000;
         break;

      case BAND_PCS:
         ret_arfcn |= 0xA000;
         break;

      case BAND_GSM_850:
         ret_arfcn |= 0xB000;
         break;

      case MAXNO_BANDS:
      case BAND_NONE:
        /* omit band indicator bits */
        break;

      default:
        MSG_GERAN_ERROR_2("Unknown band value %d for ARFCN %d", ret_band, ret_arfcn);
        break;
   }

   return(ret_arfcn);
}

/*!
 * \brief Log-on-demand handler
 *
 * \param log_code (in)
 * \param gas_id (in)
 * \param log_param_value (in)
 *
 * \return boolean - TRUE if successful; FALSE otherwise
 */
static boolean rr_log_on_demand_handler(
  log_code_type log_code,
  gas_id_t gas_id
)
{
  boolean success = FALSE;

  switch (log_code)
  {
    case LOG_GSM_RR_CELL_RESELECTION_PARAMS_C:
    {
      if (rr_cell_si_valid(gas_id))
      {
        gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

        success = rr_log_cell_selection_parameters(
          &(scell_info_ptr->gsm.cell_selection_parameters),
          gas_id
        );
      }
      break;
    }

    case LOG_GSM_RR_RACH_CONTROL_PARAMS_C:
    {
      if (rr_cell_si_valid(gas_id))
      {
        gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

        success = rr_log_rach_control_parameters(
          &(scell_info_ptr->gsm.RACH_control_parameters),
          gas_id
        );
      }
      break;
    }

    case LOG_GSM_RR_CONTROL_CHANNEL_PARAMS_C:
    {
      gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

      if (scell_info_ptr->gsm.control_channel_description_valid)
      {
        success = rr_log_control_channel_description(
          &(scell_info_ptr->gsm.control_channel_description),
          gas_id
        );
      }
      break;
    }

    case LOG_GSM_RR_CELL_OPTIONS_C:
    {
      if (rr_cell_si_valid(gas_id))
      {
        gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

        success = rr_log_cell_options(
          &(scell_info_ptr->gsm.cell_options),
          gas_id
        );
      }
      break;
    }

    case LOG_GSM_RR_CELL_INFO_C:
    {
      if (rr_cell_si_valid(gas_id))
      {
        gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

        success = rr_log_cell_info(
          &(scell_info_ptr->gsm),
          gas_id
        );
      }
      break;
    }

    case LOG_GSM_RR_CHANNEL_CONFIGURATION_C:
    {
      success = rr_log_channel_configuration(gas_id);
      break;
    }

    case LOG_GPRS_INDICATORS_C:
    {
      if (rr_cell_si_valid(gas_id))
      {
        gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

        if (scell_info_ptr->gsm.gprs_indicator)
        {
          success = rr_log_gprs_indicators(
            (uint8) scell_info_ptr->bcch_sys_info.ra_colour,
            (uint8) scell_info_ptr->bcch_sys_info.si13_position,
            gas_id
          );
        }
      }
      break;
    }

    case LOG_GPRS_GENERAL_PARAMS_C:
    {
      if (rr_cell_si_valid(gas_id))
      {
        gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

        if (scell_info_ptr->gsm.gprs_indicator)
        {
          success = rr_log_gprs_general_parameters(
            scell_info_ptr->bcch_sys_info.spgc_ccch_sup,
            scell_info_ptr->bcch_sys_info.priority_access_thr,
            scell_info_ptr->nc_meas_params.network_control_order,
            scell_info_ptr->bcch_sys_info.routing_area_code,
            0xff,
            gas_id
          );
        }
      }
      break;
    }

    case LOG_GPRS_CELL_OPTIONS_C:
    {
      if (rr_cell_si_valid(gas_id))
      {
        gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

        if (scell_info_ptr->gsm.gprs_indicator)
        {
          success = rr_log_gprs_cell_options(
            &scell_info_ptr->bcch_sys_info.gprs_cell_options,
            gas_id
          );
        }
      }
      break;
    }

    case LOG_GPRS_POWER_CONTROL_PARAMS_C:
    {
      if (rr_cell_si_valid(gas_id))
      {
        gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

        if (scell_info_ptr->gsm.gprs_indicator)
        {
          success = rr_log_gprs_power_control_parameters(
            scell_info_ptr->bcch_sys_info.power_control_params.alpha,
            scell_info_ptr->bcch_sys_info.power_control_params.t_avg_w,
            scell_info_ptr->bcch_sys_info.power_control_params.t_avg_t,
            scell_info_ptr->bcch_sys_info.power_control_params.pc_meas_chan,
            scell_info_ptr->bcch_sys_info.power_control_params.n_avg_i,
            gas_id
          );
        }
      }
      break;
    }

    case LOG_GPRS_MOBILE_ALLOCATION_C:
    {
      if (rr_cell_si_valid(gas_id))
      {
        gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

        if (scell_info_ptr->gsm.gprs_indicator)
        {
          success = rr_log_gprs_mobile_allocation_rrps_freq_list(
            &scell_info_ptr->mobile_allocations.gprs_mobile_allocation[MAP_MA_NUMBER_TO_OFFSET(MA_NUMBER_14)],
            gas_id
          );
        }
      }
      break;
    }

    case LOG_GPRS_GRR_STATE_C:
    {
      success = rr_log_gprs_grr_state(get_grr_state(gas_id), gas_id);
      break;
    }

    case LOG_GPRS_RR_CELL_RESELECTION_MEASMNTS_C:
    {
      gprs_reselection_structure_T *gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

      success = rr_log_gprs_cell_reselection_measurements(
        gprs_reselection_list_ptr,
        get_recent_reselection_flag_value(gas_id),
        gas_id
      );
      break;
    }

    case LOG_GPRS_RR_CELL_RESELECTION_PARAMS_C:
    {
      if (rr_cell_si_valid(gas_id))
      {
        gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

        success = rr_log_gprs_cell_selection_parameters(
          scell_info_ptr,
          gas_id
        );
      }
      break;
    }

#ifdef FEATURE_WCDMA
    case LOG_GPRS_RR_3G_RESELECTION_MEASUREMENTS_C:
    {
      success = rr_log_gprs_3g_reselection_measurements(gas_id);
      break;
    }

    case LOG_GPRS_RR_3G_MEASUREMENT_PARAMS_C:
    {
      if (rr_cell_si_valid(gas_id))
      {
        success = rr_log_rr_3g_measurement_params(
          rr_in_dedicated(gas_id),
          gas_id
        );
      }
      break;
    }
#endif /* FEATURE_WCDMA */

    default:
      MSG_GERAN_ERROR_1_G("Invalid=%d log code on demand",(int) log_code);
      break;
  }/* end switch */

  return success;

} /* end rr_log_on_demand_handler() */

#ifdef FEATURE_DUAL_SIM
/*!
 * \brief Call-back function called when log-on-demand (extended) requested
 *
 * \param log_code (in)
 * \param type (in)
 * \param log_param_value (in)
 *
 * \return log_on_demand_status_enum_type - LOG_ON_DEMAND_SENT_S or LOG_ON_DEMAND_FAILED_ATTEMPT_S
 */
static log_on_demand_status_enum_type rr_log_on_demand_ext_cb_fn(
  log_code_type log_code,
  uint8 type,
  uint32 log_param_value
)
{
  sys_modem_as_id_e_type as_id;
  gas_id_t gas_id;

  // type=1 indicates log_param_value contains subscriber ID
  // see document 80-V1294-1 "Log On Demand Extended"
  if (type != 0x01)
  {
    MSG_GERAN_ERROR_1("log on demand - unknown type value (%d)",(int)type);
    return LOG_ON_DEMAND_FAILED_ATTEMPT_S;
  }

  as_id = (sys_modem_as_id_e_type)log_param_value;
  gas_id = geran_map_nas_id_to_gas_id(as_id);

  if (rr_log_on_demand_handler(log_code, gas_id))
  {
    return LOG_ON_DEMAND_SENT_S;
  }

  return LOG_ON_DEMAND_FAILED_ATTEMPT_S;

} /* end rr_log_on_demand_ext_cb_fn() */

#else

/*!
 * \brief Call-back function called when log-on-demand requested
 *
 * \param log_code (in)
 *
 * \return log_on_demand_status_enum_type - LOG_ON_DEMAND_SENT_S or LOG_ON_DEMAND_FAILED_ATTEMPT_S
 */
static log_on_demand_status_enum_type rr_log_on_demand_cb_fn(
  log_code_type log_code
)
{
  // Assume Sub1
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(SYS_MODEM_AS_ID_1);

  if (rr_log_on_demand_handler(log_code, gas_id))
  {
    return LOG_ON_DEMAND_SENT_S;
  }

  return LOG_ON_DEMAND_FAILED_ATTEMPT_S;

} /* end rr_log_on_demand_cb_fn() */

#endif // FEATURE_DUAL_SIM

/*!
 * \brief Prepares channel information for logging.
 *
 * \param log_ptr
 * \param index
 * \param info_ptr
 *
 * \return size_t
 */
static size_t rr_log_channel_info(
  byte *log_ptr,
  size_t index,
  channel_information_T *info_ptr
)
{
  if ((log_ptr != NULL) && (info_ptr != NULL))
  {
    byte i;

    log_ptr[index++] = (byte)info_ptr->channel_type;
    log_ptr[index++] = (byte)info_ptr->subchannel;
    log_ptr[index++] = (byte)info_ptr->TN;
    log_ptr[index++] = (byte)info_ptr->TSC;
    log_ptr[index++] = (byte)info_ptr->hopping_flag;
    log_ptr[index++] = (byte)info_ptr->MAIO;
    log_ptr[index++] = (byte)info_ptr->HSN;
    log_ptr[index++] = (byte)info_ptr->frequency_list.no_of_items;

    for (i = 0; i < info_ptr->frequency_list.no_of_items; i++)
    {
      log_ptr[index++] = (byte)(info_ptr->frequency_list.channel[i].num & 0x00FF);
      log_ptr[index++] = (byte)(info_ptr->frequency_list.channel[i].num >> 8);
    }
  }

  return(index);
}

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

/*!
 * \brief Initialisation function for log-on-demand. The call-backs are registered.
 *
 * \param gas_id
 */
void rr_log_on_demand_init(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_log_data_t *rr_log_data_ptr = rr_log_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_log_data_ptr);

  if (rr_log_data_ptr->log_on_demand_registered)
  {
    return;
  }
  else
  {
    int i;

    // Register log-on-demand handlers

      for (i = 0; i < ARR_SIZE(rr_log_codes); i++)
      {
#ifdef FEATURE_DUAL_SIM
      (void) log_on_demand_ext_register(
           rr_log_codes[i],         // log_code
        rr_log_on_demand_ext_cb_fn   // log_on_demand_ptr
         );
#else
(void) log_on_demand_register(
   rr_log_codes[i],         // log_code
   rr_log_on_demand_cb_fn   // log_on_demand_ptr
 );
#endif // FEATURE_DUAL_SIM
      }

    rr_log_data_ptr->log_on_demand_registered = TRUE;
  }
}

/*!
 * \brief Frees any memory used by the RR-LOG module and unregisters log-on-demand handlers.
 *
 * \param gas_id
 */
void rr_log_on_demand_deinit(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_log_data_t *rr_log_data_ptr = rr_log_get_data_ptr(gas_id);

  if (rr_log_data_ptr == NULL)
  {
    return;
  }

  if (rr_log_data_ptr->log_on_demand_registered)
  {
    int i;

    for (i = 0; i < ARR_SIZE(rr_log_codes); i++)
    {
      (void) log_on_demand_unregister(
         rr_log_codes[i]    // log_code
       );
    }

    rr_log_data_ptr->log_on_demand_registered = FALSE;
  }
} /* end rr_log_on_demand_deinit() */

/*!
 * \brief Outputs LOG_GSM_RR_STATE_C log packet.
 *
 * \param state
 * \param substate
 * \param status
 * \param mode
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_state(
  byte state,
  byte substate,
  byte status,
  byte mode,
  const gas_id_t gas_id
)
{
  LOG_GSM_RR_STATE_C_type  *log_ptr;
  gsmdiag_pkt_info_t       pkt_info;

  /* Try to allocate memory for the log packet */
  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_STATE_C,
    sizeof(LOG_GSM_RR_STATE_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_STATE_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    /* Fill packet and commit it, but map the state to the appropriate value to match
       the ICD */
    switch (state)
    {
      case RR_INACTIVE:
        state = 0;
        break;

      case RR_GOING_ACTIVE:
        state = 1;
        break;

      case RR_GOING_INACTIVE:
        state = 2;
        break;

      case RR_CELL_SELECTION:
        state = 3;
        break;

      case RR_PLMN_LIST_SEARCH:
        state = 4;
        break;

      case RR_IDLE:
        state = 5;
        break;

      case RR_CELL_RESELECTION:
        state = 6;
        break;

      case RR_CONNECTION_PENDING:
        state = 7;
        break;

      case RR_CELL_REESTABLISH:
        state = 20;
        break;

      case RR_DATA_TRANSFER:
        state = 9;
        break;

      case RR_NO_CHANNELS:
        state = 10;
        break;

      case RR_CONNECTION_RELEASE:
        state = 11;
        break;

      case RR_EARLY_CAMPED_WAIT_FOR_SI:
        state = 12;
        break;

      case RR_W2G_INTERRAT_HANDOVER_PROGRESS:
        state = 13;
        break;

      case RR_W2G_INTERRAT_RESELECTION_PROGRESS:
        state = 14;
        break;

      case RR_W2G_INTERRAT_CC_ORDER_PROGRESS:
        state = 15;
        break;

      case RR_G2W_INTERRAT_RESELECTION_PROGRESS:
        state = 16;
        break;

      case RR_WAIT_FOR_EARLY_PSCAN:
        state = 17;
        break;

      case RR_GRR:
        state = 18;
        break;

      case RR_G2W_INTERRAT_HANDOVER_PROGRESS:
        state = 19;
        break;

      case RR_W2G_SERVICE_REDIRECTION_IN_PROGRESS:
        state = 21;
        break;

      case RR_RESET:
        state = 22;
        break;

#ifdef FEATURE_FEMTO_GSM_NL
      case RR_FEMTO:
        state = 29;
        break;
#endif /* FEATURE_FEMTO_GSM_NL */

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
      case RR_X2G_RESEL:
        state = 30;
        break;

      case RR_X2G_RESEL_ABORTED:
        state = 31;
        break;
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */


      case RR_X2G_REDIR:
        state = 32;
        break;

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS_G2T_REDIRECTION)
      case RR_G2X_REDIR:
        state = 33;
        break;
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE  || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION*/

#ifdef FEATURE_LTE_TO_GSM_CGI
      case RR_X2G_CGI:
        state = 34;
        break;
#endif /* FEATURE_LTE_TO_GSM_CGI */

      case RR_X2G_CCO_FAILED:
        state = 35;
        break;

      case RR_X2G_CCO_ABORTED:
        state = 36;
        break;

      case RR_X2G_CCO_FAILED_ABORTED:
        state = 37;
        break;

      case RR_CELL_SELECTED_ACQUIRE_SI:
        state = 40;
        break;

      default:
        MSG_GERAN_ERROR_1_G("State=%d not mapped to log packet",(int) state);
        break;
    } /* switch */

    log_ptr->state    = state;
    log_ptr->substate = substate;
    log_ptr->status   = status;
    log_ptr->mode     = mode;
    gsmdiag_log_commit(&pkt_info);
  }

  return (log_ptr != NULL);
} /* rr_log_state */

/*!
 * \brief Outputs LOG_GSM_RR_PROTOCOL_ERROR_C log packet.
 *
 * \param message_type
 * \param protocol_error
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_protocol_error(
  byte message_type,
  rr_protocol_error_enum_type
  protocol_error,
  const gas_id_t gas_id
)
{
  LOG_GSM_RR_PROTOCOL_ERROR_C_type *log_ptr;
  gsmdiag_pkt_info_t                pkt_info;

  /* Try to allocate memory for the log packet */
  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_PROTOCOL_ERROR_C,
    sizeof(LOG_GSM_RR_PROTOCOL_ERROR_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_PROTOCOL_ERROR_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {   // Fill packet and commit it
    log_ptr->message_type   = message_type;
    log_ptr->protocol_error = (byte) protocol_error;
    gsmdiag_log_commit(&pkt_info);
  }
  return (log_ptr != NULL);
} /* rr_log_protocol_MSG_ERROR(or */

/*!
 * \brief Outputs LOG_GSM_RR_SIGNALING_MESSAGE_C log packet.
 *
 * \param channel_type
 * \param message_type
 * \param message_length
 * \param message_data
 * \param message_dir
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_signaling_message(
  byte                              channel_type,
  byte                              message_type,
  byte                              message_length,
  byte                              *message_data,
  rr_signaling_message_direction_T  message_dir,
  const gas_id_t                    gas_id
)
{
  gsmdiag_pkt_info_t  pkt_info;
  byte               *log_ptr;
  size_t              index = 0;

  /* Try to allocate memory for the log packet */
  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_SIGNALING_MESSAGE_C,
    sizeof(log_hdr_type)+3+message_length,
    gas_id
  );

  log_ptr = (byte *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {   // Fill packet and commit it
    index += sizeof(log_hdr_type);// Advance index past log header
    //msb of channel_type is 1 if message is downlink message
    if (message_dir == RR_DOWNLINK_MSG)
    {
      channel_type |= 0x80;
    }
    // otherwise, msb should remain 0
    log_ptr[index++] = channel_type;

    log_ptr[index++] = message_type;
    log_ptr[index++] = message_length;
    memscpy(log_ptr+index, pkt_info.pkt_size-index, message_data, message_length);
    gsmdiag_log_commit(&pkt_info);
  }//endif log_ptr

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_ota_msg(channel_type, message_data, message_length, gas_id);
#endif // FEATURE_QSH_EVENT_METRIC

  return (log_ptr != NULL);
} // rr_log_signaling_message

/*!
 * \brief Outputs LOG_GSM_RR_CELL_RESELECTION_PARAMS_C log packet.
 *
 * \param csp_ptr
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_cell_selection_parameters(
  cell_selection_parameters_T *csp_ptr,
  const gas_id_t gas_id
)
{
  LOG_GSM_RR_CELL_RESELECTION_PARAMS_C_type *log_ptr;    // pointer to log buffer
  gsmdiag_pkt_info_t                         pkt_info;

  /* Try to allocate memory for the log packet */
  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_CELL_RESELECTION_PARAMS_C,
    sizeof(LOG_GSM_RR_CELL_RESELECTION_PARAMS_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_CELL_RESELECTION_PARAMS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {   // Fill packet and commit it
    log_ptr->CELL_RESELECT_HYSTERESIS = csp_ptr->CELL_RESELECT_HYSTERESIS;
    log_ptr->MS_TXPWR_MAX_CCH         = csp_ptr->MS_TXPWR_MAX_CCH;
    log_ptr->RXLEV_ACCESS_MIN         = csp_ptr->RXLEV_ACCESS_MIN;
    log_ptr->NECI                     = csp_ptr->NECI;
    log_ptr->ACS                      = csp_ptr->ACS;
    log_ptr->CELL_RESELECT_PARAM_IND  = csp_ptr->CELL_RESELECT_PARAM_IND;
    log_ptr->CELL_BAR_QUALIFY         = csp_ptr->CELL_BAR_QUALIFY;
    log_ptr->CELL_RESELECT_OFFSET     = csp_ptr->CELL_RESELECT_OFFSET;
    log_ptr->TEMPORARY_OFFSET         = csp_ptr->TEMPORARY_OFFSET;
    log_ptr->PENALTY_TIME             = csp_ptr->PENALTY_TIME;
    log_ptr->power_offset_valid       = csp_ptr->power_offset_valid;
    log_ptr->POWER_OFFSET             = csp_ptr->POWER_OFFSET;
    gsmdiag_log_commit(&pkt_info);
  }//endif log_ptr
  return (log_ptr != NULL);
}

/*!
 * \brief Outputs LOG_GSM_RR_CELL_RESELECTION_PARAMS_C log packet. if the contents have changed.
 *
 * \param csp_ptr
 * \param si_db_ptr
 * \param gas_id
 */
void rr_log_diff_cell_selection_parameters(
  cell_selection_parameters_T *csp_ptr,
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
)
{
  ASSERT(csp_ptr != NULL);
  ASSERT(si_db_ptr != NULL);

  if (memcmp(csp_ptr,&(si_db_ptr->cell_selection_parameters),sizeof(cell_selection_parameters_T))
      != 0)
  {
    // they are different, log the new values
    (void) rr_log_cell_selection_parameters(csp_ptr, gas_id);
  }
} // rr_log_diff_cell_selection_parameters

/*!
 * \brief Outputs LOG_GSM_RR_RACH_CONTROL_PARAMS_C log packet.
 *
 * \param rcp_ptr
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_rach_control_parameters(
  RACH_control_parameters_T *rcp_ptr,
  const gas_id_t gas_id
)
{
  LOG_GSM_RR_RACH_CONTROL_PARAMS_C_type *log_ptr;    // pointer to log buffer
  gsmdiag_pkt_info_t                     pkt_info;

  /* Try to allocate memory for the log packet */
  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_RACH_CONTROL_PARAMS_C,
    sizeof(LOG_GSM_RR_RACH_CONTROL_PARAMS_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_RACH_CONTROL_PARAMS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {   // Fill packet and commit it
    log_ptr->max_retrans              = rcp_ptr->max_retrans;
    log_ptr->tx_integer               = rcp_ptr->tx_integer;
    log_ptr->REESTABLISHMENT_ALLOWED  = rcp_ptr->REESTABLISHMENT_ALLOWED;
    log_ptr->CELL_BAR_ACCESS          = rcp_ptr->CELL_BAR_ACCESS;
    log_ptr->access_control[0]        = rcp_ptr->access_control[0];
    log_ptr->access_control[1]        = rcp_ptr->access_control[1];

    gsmdiag_log_commit(&pkt_info);
  }
  return (log_ptr != NULL);
}

/*!
 * \brief Outputs LOG_GSM_RR_RACH_CONTROL_PARAMS_C log packet if the contents have changed.
 *
 * \param rcp_ptr
 * \param si_db_ptr
 * \param gas_id
 */
void rr_log_diff_rach_control_parameters(
  RACH_control_parameters_T *rcp_ptr,
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
)
{
  ASSERT(rcp_ptr != NULL);
  ASSERT(si_db_ptr != NULL);

  if (memcmp(rcp_ptr,&(si_db_ptr->RACH_control_parameters),sizeof(RACH_control_parameters_T))
      != 0)
  {
    // they are different, log the new values
    (void) rr_log_rach_control_parameters(rcp_ptr, gas_id);
  }
} // rr_log_diff_rach_control_parameters

/*!
 * \brief Outputs LOG_GSM_RR_CONTROL_CHANNEL_PARAMS_C log packet.
 *
 * \param ccd_ptr
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_control_channel_description(
  control_channel_description_T *ccd_ptr,
  const gas_id_t gas_id
)
{
  LOG_GSM_RR_CONTROL_CHANNEL_PARAMS_C_type *log_ptr;    // pointer to log buffer
  gsmdiag_pkt_info_t                        pkt_info;

  /* Try to allocate memory for the log packet */
  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_CONTROL_CHANNEL_PARAMS_C,
    sizeof(LOG_GSM_RR_CONTROL_CHANNEL_PARAMS_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_CONTROL_CHANNEL_PARAMS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {   // Fill packet and commit it
    log_ptr->ATT                = ccd_ptr->ATT;
    log_ptr->BS_AG_BLKS_RES     = ccd_ptr->BS_AG_BLKS_RES;
    log_ptr->BS_CC_CHANS        = ccd_ptr->BS_CC_CHANS;
    log_ptr->BS_CCCH_SDCCH_COMB = ccd_ptr->BS_CCCH_SDCCH_COMB;
    log_ptr->BS_PA_MFRMS        = ccd_ptr->BS_PA_MFRMS;
    log_ptr->T3212_timeout      = ccd_ptr->T3212_timeout;

    gsmdiag_log_commit(&pkt_info);
  }
  return (log_ptr != NULL);
}

/*!
 * \brief Outputs LOG_GSM_RR_CONTROL_CHANNEL_PARAMS_C log packet is the contents have changed.
 *
 * \param ccd_ptr
 * \param si_db_ptr
 * \param gas_id
 */
void rr_log_diff_control_channel_description(
  control_channel_description_T *ccd_ptr,
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
)
{
  ASSERT(ccd_ptr != NULL);
  ASSERT(si_db_ptr != NULL);

  if (memcmp(ccd_ptr,&(si_db_ptr->control_channel_description),sizeof(control_channel_description_T))
      != 0)
  {
    // they are different, log the new values
    (void) rr_log_control_channel_description(ccd_ptr, gas_id);
  }
} // rr_log_diff_control_channel_description

/*!
 * \brief Outputs LOG_GSM_RR_CELL_OPTIONS_C log packet.
 *
 * \param cellop_ptr
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_cell_options(
  cell_options_T *cellop_ptr,
  const gas_id_t gas_id
)
{
  LOG_GSM_RR_CELL_OPTIONS_C_type *log_ptr;    // pointer to log buffer
  gsmdiag_pkt_info_t              pkt_info;

  /* Try to allocate memory for the log packet */
  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_CELL_OPTIONS_C,
    sizeof(LOG_GSM_RR_CELL_OPTIONS_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_CELL_OPTIONS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {   // Fill packet and commit it
    log_ptr->PWRC               = cellop_ptr->PWRC;
    log_ptr->DTX                = cellop_ptr->DTX;
    log_ptr->RADIO_LINK_TIMEOUT = cellop_ptr->RADIO_LINK_TIMEOUT;

    gsmdiag_log_commit(&pkt_info);
  }
  return (log_ptr != NULL);
}

/*!
 * \brief Outputs LOG_GSM_RR_CELL_OPTIONS_C log packet if the contents have changed.
 *
 * \param cellop_ptr
 * \param si_db_ptr
 * \param gas_id
 */
void rr_log_diff_cell_options(
  cell_options_T                *cellop_ptr,
  system_information_database_T *si_db_ptr,
  const gas_id_t                gas_id
)
{
  ASSERT(cellop_ptr != NULL);
  ASSERT(si_db_ptr != NULL);

  if (memcmp(cellop_ptr,&(si_db_ptr->cell_options),sizeof(cell_options_T))
      != 0)
  {
    // they are different, log the new values
    (void) rr_log_cell_options(cellop_ptr, gas_id);
  }
} // rr_log_diff_cell_options

/*!
 * \brief Outputs LOG_GSM_RR_CELL_INFO_C log packet.
 *
 * \param sys_info_ptr
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_cell_info(
  system_information_database_T *sys_info_ptr,
  const gas_id_t gas_id
)
{
 LOG_GSM_RR_CELL_INFO_C_type *log_ptr;    // pointer to log buffer
  gsmdiag_pkt_info_t           pkt_info;
  LAI_T                       *sys_lai_ptr;
  rr_cell_type_T               rr_cell_type;

  rr_cell_type = rr_get_cell_priority(
    sys_info_ptr->cell_selection_parameters.CELL_BAR_QUALIFY,
    sys_info_ptr->RACH_control_parameters.CELL_BAR_ACCESS,
    &sys_info_ptr->location_area_identification,
    sys_info_ptr->RACH_control_parameters.access_control,
    NULL,
    gas_id
  );

  /* Try to allocate memory for the log packet */
  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_CELL_INFO_C,
    sizeof(LOG_GSM_RR_CELL_INFO_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_CELL_INFO_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {   // Fill packet and commit it
    byte priority;
    log_ptr->BCCH_ARFCN = rr_arfcn_to_word(sys_info_ptr->BCCH_ARFCN);
    // Pack the BSIC
    log_ptr->BSIC[0]    = sys_info_ptr->BSIC.PLMN_colour_code;
    log_ptr->BSIC[1]    = sys_info_ptr->BSIC.BS_colour_code;
    log_ptr->cell_id    = (word) sys_info_ptr->cell_identity;
    // Pack the LAI
    sys_lai_ptr = &(sys_info_ptr->location_area_identification);

    /* LAI should be packed in LAI IE format */
    log_ptr->lai[0]  = sys_lai_ptr->plmn_id.identity[0];
    log_ptr->lai[1]  = sys_lai_ptr->plmn_id.identity[1];
    log_ptr->lai[2]  = sys_lai_ptr->plmn_id.identity[2];
    log_ptr->lai[3]  = (byte)((sys_lai_ptr->location_area_code & 0xFF00) >> 8);
    log_ptr->lai[4]  = (byte)(sys_lai_ptr->location_area_code  & 0x00FF);

    if (rr_cell_type == RR_LOW_PRIORITY_CELL)
    {
      priority = 1;
    }
    else if (rr_cell_type == RR_HIGH_PRIORITY_CELL)
    {
      priority = 0;
    }
    else
    {
      priority = 2;
    }

    log_ptr->cell_selection_priority = priority;

    log_ptr->NCC_permitted  = sys_info_ptr->NCC_permitted;
    gsmdiag_log_commit(&pkt_info);
  }
  return (log_ptr != NULL);
}

/*!
 * \brief Outputs LOG_GSM_RR_CHANNEL_CONFIGURATION_C log packet.
 *
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_channel_configuration(
  const gas_id_t gas_id
)
{
  channel_spec_T *chan_spec_ptr;

  chan_spec_ptr = rr_get_channel_spec(
    NULL,     // dedic_ptr
    gas_id    // gas_id
  );

  if (chan_spec_ptr != NULL)
  {
    gsmdiag_pkt_info_t  pkt_info;
    byte               *log_ptr;    // pointer to log buffer
    size_t              index     = 0; // index into buffer
    size_t              pkt_size  = 0; // for calculating packet size

    // Step 1: This is a variable length packet -- figure out how much to allocate
    pkt_size = sizeof(log_hdr_type) +
       10 + // chan_spec_ptr general information
        8  + // required after parameters
       (2 * chan_spec_ptr->channel_info_1_after.frequency_list.no_of_items); // frequency list for after params

    pkt_size += 1; // before parameters valid

    if (chan_spec_ptr->before_parameters_valid)
    {
      pkt_size +=
        8 // optional before parameters
        +
        (2 * chan_spec_ptr->channel_info_1_before.frequency_list.no_of_items); // before freq. list info
    }

    // Step 2: Try to allocate memory for the log packet
    rr_log_packet_init(
      &pkt_info,
      LOG_GSM_RR_CHANNEL_CONFIGURATION_C,
      pkt_size,
      gas_id
    );

    log_ptr = (byte *)gsmdiag_log_alloc(&pkt_info);

    // Step 3: Fill packet and commit it
    if (log_ptr != NULL)
    {
      index += sizeof(log_hdr_type);// Advance index past log header

      /* log the general channel spec information */
      log_ptr[index++] = (byte)chan_spec_ptr->number_of_ded_channels;
      log_ptr[index++] = (byte)chan_spec_ptr->DTX_indicator;
      log_ptr[index++] = (byte)chan_spec_ptr->power_level;
      log_ptr[index++] = (byte)chan_spec_ptr->starting_time_valid;
      log_ptr[index++] = (byte)chan_spec_ptr->starting_time[0];
      log_ptr[index++] = (byte)chan_spec_ptr->starting_time[1];
      log_ptr[index++] = (byte)chan_spec_ptr->cipher_flag;
      log_ptr[index++] = (byte)chan_spec_ptr->cipher_algorithm;
      log_ptr[index++] = (byte)chan_spec_ptr->channel_mode_1;
      log_ptr[index++] = (byte)chan_spec_ptr->channel_mode_2;

      /* log the AFTER channel information */
      index = rr_log_channel_info(log_ptr, index, &(chan_spec_ptr->channel_info_1_after));
      log_ptr[index++] = (byte)chan_spec_ptr->before_parameters_valid;

      /* log the BEFORE channel information, if valid */
      if (chan_spec_ptr->before_parameters_valid)
      {
        index = rr_log_channel_info(log_ptr, index, &(chan_spec_ptr->channel_info_1_before));
      }

      /* verify that the index matches the packet size */
      if (index != pkt_size)
      {
        MSG_GERAN_ERROR_2_G("index(%d) != pkt_size(%d)",(int)index,(int)pkt_size);
      }
      gsmdiag_log_commit(&pkt_info);
    }

    return(log_ptr != NULL);
  }

  return(FALSE);
}

/*!
 * \brief Outputs LOG_GSM_RR_CIPHERING_MODE_C log packet.
 *
 * \param ciphering_flag
 * \param ciphering_algorithm
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_ciphering_mode(
  byte ciphering_flag,
  byte ciphering_algorithm,
  const gas_id_t gas_id
)
{
  LOG_GSM_RR_CIPHERING_MODE_C_type *log_ptr;
  gsmdiag_pkt_info_t                pkt_info;

  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_CIPHERING_MODE_C,
    sizeof(LOG_GSM_RR_CIPHERING_MODE_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_CIPHERING_MODE_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    // Fill packet and commit it.
    log_ptr->ciphering_flag       = ciphering_flag;
    log_ptr->ciphering_algorithm  = ciphering_algorithm;

    // commit the log packet.
    gsmdiag_log_commit(&pkt_info);
  }
  return (log_ptr != NULL);
}

/*!
 * \brief Outputs LOG_GSM_RR_DSC_COUNTER_C log packet.
 *
 * \param counter
 * \param gas_id
 */
void rr_log_dsc_counter(
  int counter,
  const gas_id_t gas_id
)
{
  LOG_GSM_RR_DSC_COUNTER_C_type *log_ptr;       // pointer to log buffer
  gsmdiag_pkt_info_t             pkt_info;

  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_DSC_COUNTER_C,
    sizeof(LOG_GSM_RR_DSC_COUNTER_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_DSC_COUNTER_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    // Fill packet
    log_ptr->DSC_counter = counter;
    // commit the log packet.
    gsmdiag_log_commit(&pkt_info);
  }
}

/*!
 * \brief Logs S_counter.
 *
 * \param counter
 * \param gas_id
 */
void rr_log_s_counter(
  byte counter,
  const gas_id_t gas_id
)
{
  LOG_GSM_RR_S_COUNTER_C_type *log_ptr;       // pointer to log buffer
  gsmdiag_pkt_info_t           pkt_info;

  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_S_COUNTER_C,
    sizeof(LOG_GSM_RR_S_COUNTER_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_S_COUNTER_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    // Fill packet
    log_ptr->S_counter  = counter;
    // commit the log packet.
    gsmdiag_log_commit(&pkt_info);
  }
}

/*!
 * \brief Outputs LOG_GSM_RR_SACCH_RPT_C log packet.
 *
 * \param sys_info_ptr
 * \param surr_meas_ptr
 * \param RXLEV_FULL_average
 * \param RXLEV_SUB_average
 * \param RXQUAL_FULL_average
 * \param RXQUAL_SUB_average
 * \param gas_id
 */
void rr_log_sacch_rpt(
  system_information_database_T *sys_info_ptr,
  surrounding_measurement_T *surr_meas_ptr,
  byte RXLEV_FULL_average,
  byte RXLEV_SUB_average,
  byte RXQUAL_FULL_average,
  byte RXQUAL_SUB_average,
  const gas_id_t gas_id
)
{
  LOG_GSM_RR_SACCH_RPT_C_type  *log_ptr;       /* pointer to log buffer */
  sacch_surround_measurements_T surround_meas; /* local copy of measurements */
  gsmdiag_pkt_info_t            pkt_info;

  memset(&surround_meas, 0, sizeof(sacch_surround_measurements_T));

  if (surr_meas_ptr != NULL)
  {
    int i;

    surround_meas.no_of_entries = (uint16)(surr_meas_ptr->no_of_entries);

    /* logging tools use integer ARFCNs, rather than structures, so convert here */
    for (i = 0; i < surround_meas.no_of_entries; i++)
    {
      surround_meas.measurement[i].BCCH_ARFCN    = rr_arfcn_to_word(surr_meas_ptr->measurement[i].BCCH_ARFCN);
      surround_meas.measurement[i].BSIC          = surr_meas_ptr->measurement[i].BSIC;
      surround_meas.measurement[i].RXLEV_average = surr_meas_ptr->measurement[i].RXLEV_average;
    }
  }
  else
  {
    surround_meas.no_of_entries = 0;
  }

  /* Try to allocate memory for the log packet */
  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_SACCH_RPT_C,
    sizeof(LOG_GSM_RR_SACCH_RPT_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_SACCH_RPT_C_type  *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    /* fill log packet and commit it */
    if (sys_info_ptr != NULL)
    {
      log_ptr->serving_BCCH_ARFCN = rr_arfcn_to_word(sys_info_ptr->BCCH_ARFCN);
    }
    else
    {
      log_ptr->serving_BCCH_ARFCN = INVALID_LOG_ARFCN;
    }
    log_ptr->RXLEV_FULL_average  = RXLEV_FULL_average;
    log_ptr->RXLEV_SUB_average   = RXLEV_SUB_average;
    log_ptr->RXQUAL_FULL_average = RXQUAL_FULL_average;
    log_ptr->RXQUAL_SUB_average  = RXQUAL_SUB_average;

   memscpy((void*)log_ptr->surr_cell_measurements,
       sizeof(sacch_surround_measurements_T),
       (void*)&surround_meas,
       sizeof(sacch_surround_measurements_T));


      gsmdiag_log_commit(&pkt_info);
  }
}

/*!
 * \brief Outputs LOG_GSM_RR_AVAILABLE_PLMN_LIST_C log packet.
 *
 * \param available_plmn_db_ptr
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_available_plmn_list(
  rr_available_plmn_db_T *available_plmn_db_ptr,
  const gas_id_t gas_id
)
{
  LOG_GSM_RR_AVAILABLE_PLMN_LIST_C_type *log_ptr;
  gsmdiag_pkt_info_t                     pkt_info;
  int                                    i;

  if (available_plmn_db_ptr == NULL)
  {
    return FALSE;
  }

  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_AVAILABLE_PLMN_LIST_C,
    sizeof(LOG_GSM_RR_AVAILABLE_PLMN_LIST_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_AVAILABLE_PLMN_LIST_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    /* determine the max number of PLMNs to report */
    int num_plmns = available_plmn_db_ptr->num_plmn;

    if (num_plmns > RR_LOG_MAX_PLMN_LIST_ENTRIES)
    {
      num_plmns = RR_LOG_MAX_PLMN_LIST_ENTRIES;
    }

    /* copy that many PLMNs into the log packet */
    log_ptr->plmn_list_count = (byte) num_plmns;

    /* foreach PLMN to be logged */
    for (i=0; i < num_plmns; ++i)
    {
      rr_log_plmn_list_entry_T *entry_ptr = &log_ptr->available_plmn_list[i];
      rr_plmn_info_T *plmn_info_ptr = &available_plmn_db_ptr->plmn_info[i];
      int num_cells;
      int j;

      /* extract the MCC/MNC information (in BCD format) */
      /*******************************************************************************/
      /* A PLMN ID is defined within a GSM network to be the combination of a Mobile */
      /* Country Code (MCC) and Mobile Network Code (MNC).  A PLMN ID is stored in 3 */
      /* octets as specified in 3G TS 24.008 Section 10.5.1.3 and shown below:       */
      /*                                         Bit                                 */
      /*                  |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |          */
      /*                  |===============================================|          */
      /*          octet 1 |      MCC Digit 2      |      MCC Digit 1      |          */
      /*                  |-----------------------------------------------|          */
      /*          octet 2 |      MNC Digit 3      |      MCC Digit 3      |          */
      /*                  |-----------------------------------------------|          */
      /*          octet 3 |      MNC Digit 2      |      MNC Digit 1      |          */
      /*                  |===============================================|          */
      /*******************************************************************************/
      entry_ptr->MCC[0] = plmn_info_ptr->PLMN_id.identity[0] & 0x0f;
      entry_ptr->MCC[1] = (plmn_info_ptr->PLMN_id.identity[0] & 0xf0) >> 4;
      entry_ptr->MCC[2] = plmn_info_ptr->PLMN_id.identity[1] & 0x0f;

      entry_ptr->MNC[0] = plmn_info_ptr->PLMN_id.identity[2] & 0x0f;
      entry_ptr->MNC[1] = (plmn_info_ptr->PLMN_id.identity[2] & 0xf0) >> 4;
      entry_ptr->MNC[2] = (plmn_info_ptr->PLMN_id.identity[1] & 0xf0) >> 4;

      /* store information for up to 3 cells for the PLMN */
      num_cells = plmn_info_ptr->num_frequencies;
      if (num_cells > RR_LOG_MAX_CELLS_IN_PLMN_LIST_ENTRY)
      {
        num_cells = RR_LOG_MAX_CELLS_IN_PLMN_LIST_ENTRY;
      }

      entry_ptr->num_frequencies = (byte) num_cells;

      for (j=0; j < num_cells; ++j)
      {
        rr_cell_type_T priority;
        entry_ptr->cells[j].ARFCN           = rr_arfcn_to_word(plmn_info_ptr->cell_info[j].meas.arfcn);
        entry_ptr->cells[j].RXLEV_average   = plmn_info_ptr->cell_info[j].meas.RXLEV_average;
        priority = plmn_info_ptr->cell_info[j].priority;

        entry_ptr->cells[j].cell_type       =
          (priority == RR_LOW_PRIORITY_CELL) ? 0 : ((priority == RR_HIGH_PRIORITY_CELL) ? 1 : 2);
        entry_ptr->cells[j].GPRS_indicator  =
          (plmn_info_ptr->cell_info[j].GPRS_indicator) ? 1 : 0;
        entry_ptr->cells[j].location_area_code = plmn_info_ptr->cell_info[j].lai.location_area_code;
      }
    }

    gsmdiag_log_commit(&pkt_info);
  }

  return TRUE;
} /* rr_log_available_plmn_list */

#ifdef FEATURE_WCDMA
/*!
 * \brief Ouptuts LOG_GSM_DED_3G_MEASUREMENTS_C log packet.
 *
 * \param report
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_3g_cell_list_report(
  rr_log_3g_meas_report_T *report,
  const gas_id_t gas_id
)
{
  LOG_GSM_DED_3G_MEASUREMENTS_C_type *log_ptr;
  gsmdiag_pkt_info_t                  pkt_info;
  byte                                i;

  if (report == NULL)
  {
    return FALSE;
  }

  RR_ASSERT_FATAL(RR_LOG_MAX_WCDMA_STORED_DED_MEASUREMENTS == MAX_WCDMA_STORED_DED_MEASUREMENTS);
  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_DED_3G_MEASUREMENTS_C,
    sizeof(LOG_GSM_DED_3G_MEASUREMENTS_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_DED_3G_MEASUREMENTS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    for (i = 0; i < report->NUM_CELLS; ++i)
    {
      log_ptr->rr_3g_report[i].CELL_INDEX = (byte)report->rr_3g_report[i].CELL_INDEX;
      log_ptr->rr_3g_report[i].UARFCN = (word)report->rr_3g_report[i].UARFCN;
      log_ptr->rr_3g_report[i].SC_CODE = (word)report->rr_3g_report[i].SC_CODE;
      log_ptr->rr_3g_report[i].DIVERSITY = (byte)report->rr_3g_report[i].DIVERSITY;
      log_ptr->rr_3g_report[i].PN_POS = (word)report->rr_3g_report[i].PN_POS;
      log_ptr->rr_3g_report[i].RSSI = (int16)report->rr_3g_report[i].RSSI;
      log_ptr->rr_3g_report[i].ENERGY = (int16)report->rr_3g_report[i].ENERGY;
      log_ptr->rr_3g_report[i].EXPIRES = (word)report->rr_3g_report[i].EXPIRES;
      log_ptr->rr_3g_report[i].REPORT_FIELDS = (byte)report->rr_3g_report[i].REPORT_FIELDS;
    }
    log_ptr->NUM_CELLS = report->NUM_CELLS;

    gsmdiag_log_commit(&pkt_info);

    return TRUE;
  }

  return FALSE;
} /* rr_log_3g_cell_list_report */

/*!
 * \brief Outputs LOG_GPRS_RR_3G_MEASUREMENT_PARAMS_C log packet.
 *
 * \param in_dedicated
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_rr_3g_measurement_params(
  boolean in_dedicated,
  const gas_id_t gas_id
)
{
  LOG_GPRS_RR_3G_MEASUREMENT_PARAMS_C_type *log_ptr;
  gprs_scell_info_t                        *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gsmdiag_pkt_info_t                        pkt_info;

  rr_log_packet_init(
    &pkt_info,
    LOG_GPRS_RR_3G_MEASUREMENT_PARAMS_C,
    sizeof(LOG_GPRS_RR_3G_MEASUREMENT_PARAMS_C_type),
    gas_id
  );

  log_ptr = (LOG_GPRS_RR_3G_MEASUREMENT_PARAMS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    /* initialise all the fields to 0xFF */
    log_ptr->qsearch_p = 0xFF;
    log_ptr->qsearch_i = 0xFF;
    log_ptr->qsearch_c_initial = 0xFF;
    log_ptr->fdd_qoffset = 0xFF;
    log_ptr->fdd_gprs_qoffset = 0xFF;
    log_ptr->fdd_qmin = 0xFF;
    log_ptr->_3g_search_prio = 0xFF;
    log_ptr->_3g_ba_ind = 0xFF;
    log_ptr->fdd_rep_quant = 0xFF;
    log_ptr->fdd_multirat_reporting = 0xFF;
    log_ptr->fdd_reporting_offset = 0xFF;
    log_ptr->fdd_reporting_threshold = 0xFF;
    log_ptr->report_type = 0xFF;
    log_ptr->index_start_3g = 0xFF;
    log_ptr->qsearch_c = 0xFF;
    log_ptr->mi_3g_wait = 0xFF;

    if ((in_dedicated == TRUE) && (scell_info_ptr->gsm.wcdma_ded_neighbor_list.valid_data == TRUE))
    {
      log_ptr->_3g_ba_ind = scell_info_ptr->gsm.wcdma_ded_neighbor_list.common_params.BA_IND_3G;
      log_ptr->qsearch_c = scell_info_ptr->gsm.wcdma_ded_neighbor_list.mi_params.QSearch_C;
      log_ptr->fdd_rep_quant = (int) scell_info_ptr->gsm.wcdma_ded_neighbor_list.mi_params.FDD_REP_QUANT;
      log_ptr->fdd_multirat_reporting = scell_info_ptr->gsm.wcdma_ded_neighbor_list.mi_params.FDD_MULTIRAT_REPORTING;
      log_ptr->report_type = (int) scell_info_ptr->gsm.wcdma_ded_neighbor_list.mi_params.REPORT_TYPE;
      log_ptr->index_start_3g = scell_info_ptr->gsm.wcdma_ded_neighbor_list.common_params.Index_Start_3G;
      log_ptr->mi_3g_wait = scell_info_ptr->gsm.wcdma_ded_neighbor_list.mi_params.MI_3G_WAIT;

    }
    else if ((in_dedicated == FALSE) || (scell_info_ptr->gsm.wcdma_ded_neighbor_list.valid_data == FALSE))

    {
      /* if not in dedicated or if the SI2quater information is used in dedicated mode */
      if (scell_info_ptr->gsm.utran_neighbor_list.valid_data)
      {
        log_ptr->qsearch_i = scell_info_ptr->gsm.utran_neighbor_list.si2q_params.QSearch_I;
        log_ptr->qsearch_c_initial = scell_info_ptr->gsm.utran_neighbor_list.si2q_params.QSearch_C_Initial;
        log_ptr->fdd_qoffset = scell_info_ptr->gsm.utran_neighbor_list.si2q_params.FDD_Qoffset;
        log_ptr->fdd_qmin = scell_info_ptr->gsm.utran_neighbor_list.si2q_params.FDD_Qmin;
        log_ptr->fdd_rep_quant = (int) scell_info_ptr->gsm.utran_neighbor_list.si2q_params.FDD_REP_QUANT;
        log_ptr->fdd_multirat_reporting = scell_info_ptr->gsm.utran_neighbor_list.si2q_params.FDD_MULTIRAT_REPORTING;
        log_ptr->report_type = (int) scell_info_ptr->gsm.utran_neighbor_list.si2q_params.REPORT_TYPE;
        log_ptr->_3g_ba_ind = scell_info_ptr->gsm.utran_neighbor_list.common_params.BA_IND_3G;
        log_ptr->index_start_3g = scell_info_ptr->gsm.utran_neighbor_list.common_params.Index_Start_3G;
      }

      /* if not in dedicated mode and if there is no SI2quater data check for SI2ter 3g information */
      else if ((scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.valid_data) && (in_dedicated == FALSE))
      {
        log_ptr->qsearch_i = scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.Qsearch_I;
        log_ptr->fdd_qoffset = scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.FDD_Qoffset;
        log_ptr->fdd_qmin = scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.FDD_Qmin;
      }

    }
    gsmdiag_log_commit(&pkt_info);
  }
  return (NULL != log_ptr);
}

/*!
 * \brief Outputs LOG_GPRS_RR_3G_RESELECTION_MEASUREMENTS_C log packet.
 *
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_gprs_3g_reselection_measurements(
  const gas_id_t gas_id
)
{
  LOG_GPRS_RR_3G_RESELECTION_MEASUREMENTS_C_type *log_ptr;
  gsmdiag_pkt_info_t                              pkt_info;

  rr_log_packet_init(
    &pkt_info,
    LOG_GPRS_RR_3G_RESELECTION_MEASUREMENTS_C,
    sizeof(LOG_GPRS_RR_3G_RESELECTION_MEASUREMENTS_C_type),
    gas_id
  );

  log_ptr = (LOG_GPRS_RR_3G_RESELECTION_MEASUREMENTS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    rr_irat_populate_gprs_3g_reselection_measurements_log_packet(log_ptr, gas_id);

    gsmdiag_log_commit(&pkt_info);
  }
  return (NULL != log_ptr);
}

/*!
 * \brief Ouptuts LOG_GSM_DED_3G_MEASUREMENTS_C log packet.
 *
 * \param gas_id
 */
void rr_log_rrc_interrat_reselect_rej(
  const gas_id_t gas_id
)
{
  LOG_GSM_IDLE_MODE_3G_RESELECT_REJ_C_type *log_ptr;
  gsmdiag_pkt_info_t                        pkt_info;

  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_IDLE_MODE_3G_RESELECT_REJ_C,
    sizeof(LOG_GSM_IDLE_MODE_3G_RESELECT_REJ_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_IDLE_MODE_3G_RESELECT_REJ_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    rr_irat_populate_gsm_idle_mode_3g_reselect_rej_log_packet(log_ptr);
    gsmdiag_log_commit(&pkt_info);
  }
}
#endif /* FEATURE_WCDMA */

/*!
 * \brief Outputs LOG_GSM_RR_BA_LIST_C log packet.
 *
 * \param balist
 * \param gas_id
 */
void rr_log_ba_list(
  inter_task_BCCH_allocation_T *balist,
  gas_id_t gas_id
)
{
  LOG_GSM_RR_BA_LIST_C_type *log_ptr;
  gsmdiag_pkt_info_t         pkt_info;
  int16                      j;

  /* Try to allocate memory for the log packet */
  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_BA_LIST_C,
    sizeof(LOG_GSM_RR_BA_LIST_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_BA_LIST_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    log_ptr->no_of_entries = balist->no_of_entries;
    for (j=0; j<BA_MAX_LEN; j++)
    {
      log_ptr->num[j] = rr_arfcn_to_word(balist->BCCH_ARFCN[j]);
    }
    gsmdiag_log_commit(&pkt_info);
  }
}

/*!
 * \brief Output LOG_GSM_RR_RANDOM_ACCESS_REQUEST_C log packet.
 *
 * \param establishment_cause
 * \param random_access_content
 * \param gas_id
 */
void rr_log_random_access_request(
  uint8 establishment_cause,
  uint8 random_access_content,
  const gas_id_t gas_id
)
{
  LOG_GSM_RR_RANDOM_ACCESS_REQUEST_C_type *log_ptr;
  gsmdiag_pkt_info_t                       pkt_info;

  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_RANDOM_ACCESS_REQUEST_C,
    sizeof(LOG_GSM_RR_RANDOM_ACCESS_REQUEST_C_type),
    gas_id
  );

  log_ptr = (LOG_GSM_RR_RANDOM_ACCESS_REQUEST_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->establishment_cause   = establishment_cause;
    log_ptr->random_access_content = random_access_content;
    gsmdiag_log_commit(&pkt_info);
  }

  return;
}

/*!
 * \brief Logs GPRS indicators
 *
 * \param ra_colour
 * \param si13_position
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_gprs_indicators(
  uint8 ra_colour,
  uint8 si13_position,
  const gas_id_t gas_id
)
{
  LOG_GPRS_INDICATORS_C_type *log_ptr;
  gsmdiag_pkt_info_t          pkt_info;
  boolean                     retVal = FALSE;

  rr_log_packet_init(
    &pkt_info,
    LOG_GPRS_INDICATORS_C,
    sizeof(LOG_GPRS_INDICATORS_C_type),
    gas_id
  );

  log_ptr = (LOG_GPRS_INDICATORS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    retVal = TRUE;
    log_ptr->ra_colour = ra_colour;
    log_ptr->si13_position = si13_position;
    gsmdiag_log_commit(&pkt_info);
  }
#ifndef PERLUTF
  else if (log_status(LOG_GPRS_INDICATORS_C))
  {
    MSG_GERAN_HIGH_0_G("GPRS Indicators enabled, but not logged");
  }
#endif
  return retVal;
}

/*!
 * \brief Outputs LOG_GPRS_INDICATORS_C log packet is the content has changed.
 *
 * \param gprs_indicator
 * \param ra_colour
 * \param si13_position
 * \param cell_info
 * \param gas_id
 */
void rr_log_diff_gprs_indicators(
  boolean gprs_indicator,
  uint8 ra_colour,
  uint8 si13_position,
  gprs_scell_info_t *cell_info,
  const gas_id_t gas_id
)
{
  if (gprs_indicator)
  {
    if ( (cell_info->gsm.gprs_indicator == FALSE)
         ||
         ( cell_info->gsm.gprs_indicator
           &&
           ( (cell_info->bcch_sys_info.ra_colour != ra_colour)
             ||
             (cell_info->bcch_sys_info.si13_position != si13_position) ) ) )
    {
      (void) rr_log_gprs_indicators(
        (uint8) ra_colour,
        (uint8) si13_position,
        gas_id
      );
    }
  }
}

/*!
 * \brief Log general GPRS parameters
 *
 * \param spgc_ccch_sup
 * \param priority_access_thr
 * \param network_control_order
 * \param routing_area_code
 * \param sgsnr
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_gprs_general_parameters(
  uint8 spgc_ccch_sup,
  uint8 priority_access_thr,
  uint8 network_control_order,
  uint8 routing_area_code,
  uint8 sgsnr,
  const gas_id_t gas_id)
{
  LOG_GPRS_GENERAL_PARAMS_C_type *log_ptr;
  gsmdiag_pkt_info_t              pkt_info;
  boolean                         retVal = FALSE;

  rr_log_packet_init(
    &pkt_info,
    LOG_GPRS_GENERAL_PARAMS_C,
    sizeof(LOG_GPRS_GENERAL_PARAMS_C_type),
    gas_id
  );

  log_ptr = (LOG_GPRS_GENERAL_PARAMS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->spgc_ccch_sup         = spgc_ccch_sup;
    log_ptr->priority_access_thr   = priority_access_thr;
    log_ptr->network_control_order = network_control_order;
    log_ptr->rac                   = routing_area_code;
    log_ptr->sgsnr                 = sgsnr;
    gsmdiag_log_commit(&pkt_info);
  }
  return retVal;
}

/*!
 * \brief Log GPRS Cell Options
 *
 * \param gprs_cell_options
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_gprs_cell_options(
  gprs_cell_options_t *gprs_cell_options,
  const gas_id_t gas_id
)
{
  LOG_GPRS_CELL_OPTIONS_C_type *log_ptr;
  gsmdiag_pkt_info_t            pkt_info;
  boolean                       retVal = FALSE;

  rr_log_packet_init(
    &pkt_info,
    LOG_GPRS_CELL_OPTIONS_C,
    sizeof(LOG_GPRS_CELL_OPTIONS_C_type),
    gas_id
  );

  log_ptr = (LOG_GPRS_CELL_OPTIONS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    retVal = TRUE;

    log_ptr->nmo                     = gprs_cell_options->nmo;
    log_ptr->t3168                   = gprs_cell_options->t3168;
    log_ptr->t3192                   = gprs_cell_options->t3192;
    log_ptr->drx_timer_max           = gprs_cell_options->drx_timer_max;
    log_ptr->access_burst_type       = gprs_cell_options->access_burst_type;
    log_ptr->control_ack_type        = gprs_cell_options->ctrl_ack_type;
    log_ptr->bs_cv_max               = gprs_cell_options->bs_cv_max;
    log_ptr->pan_dec                 = gprs_cell_options->pan_dec;
    log_ptr->pan_inc                 = gprs_cell_options->pan_inc;
    log_ptr->pan_max                 = gprs_cell_options->pan_max;
    log_ptr->bss_paging_coordination = 0xFF;

    gsmdiag_log_commit(&pkt_info);
  }
  return retVal;
}

/*!
 * \brief Log GPRS Power Control Parameters
 *
 * \param alpha
 * \param t_avg_w
 * \param t_avg_t
 * \param pc_meas_chan
 * \param n_avg_i
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_gprs_power_control_parameters(
  uint8 alpha,
  uint8 t_avg_w,
  uint8 t_avg_t,
  boolean pc_meas_chan,
  uint8 n_avg_i,
  const gas_id_t gas_id)
{
  LOG_GPRS_POWER_CONTROL_PARAMS_C_type *log_ptr;
  gsmdiag_pkt_info_t                    pkt_info;
  boolean                               retVal = FALSE;

  rr_log_packet_init(
    &pkt_info,
    LOG_GPRS_POWER_CONTROL_PARAMS_C,
    sizeof(LOG_GPRS_POWER_CONTROL_PARAMS_C_type),
    gas_id
  );

  log_ptr = (LOG_GPRS_POWER_CONTROL_PARAMS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    retVal = TRUE;
    log_ptr->alpha        = alpha;
    log_ptr->t_avg_w      = t_avg_w;
    log_ptr->t_avg_t      = t_avg_t;
    log_ptr->pc_meas_chan = pc_meas_chan;
    log_ptr->n_avg_i      = n_avg_i;
    gsmdiag_log_commit(&pkt_info);
  }
  return retVal;
}

/*!
 * \brief Log GPRS Mobile Allocation
 *
 * \param mob_alloc
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_gprs_mobile_allocation(
  gprs_mobile_allocation_t *mob_alloc,
  const gas_id_t gas_id
)
{
  gsmdiag_pkt_info_t  pkt_info;
  unsigned int        packet_size;
  unsigned int        index = sizeof(log_hdr_type);
  unsigned int        j;
  boolean             retVal = FALSE;
  uint16              encoded_arfcn;
  uint8              *log_ptr;

  packet_size = sizeof(log_hdr_type) + 1 + 1 + (2 * mob_alloc->num_of_arfcns);

  rr_log_packet_init(
    &pkt_info,
    LOG_GPRS_MOBILE_ALLOCATION_C,
    packet_size,
    gas_id
  );

  log_ptr = (uint8 *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    retVal = TRUE;
    log_ptr[index++] = mob_alloc->hsn;
    log_ptr[index++] = mob_alloc->num_of_arfcns;
    for (j = 0; j < mob_alloc->num_of_arfcns; j++)
    {
      encoded_arfcn = rr_arfcn_to_word(mob_alloc->arfcns[j]);
      log_ptr[index++] = encoded_arfcn & 0xff;
      log_ptr[index++] = (encoded_arfcn >> 8) & 0xff;
    }
    if (index != packet_size)
    {
      MSG_GERAN_ERROR_2_G("index(%d) != packet_size(%d)",(int) index,(int) packet_size);
    }
    gsmdiag_log_commit(&pkt_info);
  }

  return retVal;
}

/*!
 * \brief Log GPRS Mobile Allocation
 *
 * \param mob_alloc
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_gprs_mobile_allocation_rrps_freq_list(
  rrps_frequency_list_t *mob_alloc,
  const gas_id_t gas_id
)
{
  gsmdiag_pkt_info_t  pkt_info;
  unsigned int        packet_size;
  unsigned int        index = sizeof(log_hdr_type);
  unsigned int        arfcn_index;
  boolean             retVal = FALSE;
  uint16              encoded_arfcn;
  uint8              *log_ptr;

  packet_size = sizeof(log_hdr_type) + 2 + (2 * mob_alloc->list_length);

  rr_log_packet_init(
    &pkt_info,
    LOG_GPRS_MOBILE_ALLOCATION_C,
    packet_size,
    gas_id
  );

  log_ptr = (uint8 *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    retVal = TRUE;
    log_ptr[index++] = mob_alloc->hsn;
    log_ptr[index++] = mob_alloc->list_length;
    for (arfcn_index = 0; arfcn_index < mob_alloc->list_length; arfcn_index++)
    {
      encoded_arfcn = rr_arfcn_to_word(mob_alloc->ma_number[arfcn_index]);
      log_ptr[index++] = (byte)(encoded_arfcn & 0x00ff);
      log_ptr[index++] = (byte)(encoded_arfcn >> 8);
    }
    if (index != packet_size)
    {
      MSG_GERAN_ERROR_2_G("index(%d) != packet_size(%d)", index, packet_size);
    }
    gsmdiag_log_commit(&pkt_info);
  }

  return retVal;
}

/*!
 * \brief Log GRR-CONTROL state
 *
 * \param grr_state
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_gprs_grr_state(
  grr_state_t grr_state,
  const gas_id_t gas_id
)
{
  LOG_GPRS_GRR_STATE_C_type *log_ptr;
  gsmdiag_pkt_info_t         pkt_info;
  boolean                    retVal = FALSE;

  rr_log_packet_init(
    &pkt_info,
    LOG_GPRS_GRR_STATE_C,
    sizeof(LOG_GPRS_GRR_STATE_C_type),
    gas_id
  );

  log_ptr = (LOG_GPRS_GRR_STATE_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    retVal = TRUE;
    log_ptr->active_state = 0;

    switch (grr_state)
    {
      case GRR_NULL:
        log_ptr->grr_state = 0;
        break;

      case GRR_ACQUIRE:
        log_ptr->grr_state = 1;
        break;

      case GRR_CAMPED:
        log_ptr->grr_state = 2;
        break;

      case GRR_CONNECTION_PENDING:
        log_ptr->grr_state = 3;
        break;

      case GRR_CELL_RESELECTION:
        log_ptr->grr_state = 4;
        break;

      case GRR_CELL_RESELECTION_G2W:
        log_ptr->grr_state = 6;
        break;

/*    case GRR_BACKGROUND_PLMN_SEARCH:  (legacy)
**      log_ptr->grr_state = 7;
        break; */

#ifdef FEATURE_GPRS_PS_HANDOVER
      case GRR_PSHO:
        log_ptr->grr_state = 8;
        break;
#endif /* FEATURE_GPRS_PS_HANODVER */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
      case GRR_X2G_CCO_FAILED:
        log_ptr->grr_state = 10;
        break;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

      case GRR_STARTING_L1_IDLE:
        log_ptr->grr_state = 11;
        break;

      case GRR_CAMPED_ENABLING_PS_ACCESS:
        log_ptr->grr_state = 12;
        break;

      case GRR_CAMPED_DISABLING_PS_ACCESS:
        log_ptr->grr_state = 13;
        break;

      case GRR_CAMPED_MSC_CHANGE:
        log_ptr->grr_state = 14;
        break;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
      case GRR_X2G_CCO_ABORTED:
        log_ptr->grr_state = 15;
        break;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

      default:
        MSG_GERAN_HIGH_0_G("GRR state not defined");
        log_ptr->grr_state = (byte) grr_state;
        break;
    }

    gsmdiag_log_commit(&pkt_info);
  }

  return retVal;
}

/*!
 * \brief Outputs LOG_GPRS_RR_CELL_RESELECTION_PARAMS_C log packet.
 *
 * \param scell_ptr
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_gprs_cell_selection_parameters(
  gprs_scell_info_t *scell_ptr,
  const gas_id_t gas_id
)
{
  LOG_GPRS_RR_CELL_RESELECTION_PARAMS_C_type *log_ptr;
  gsmdiag_pkt_info_t                          pkt_info;

  /* Try to allocate memory for the log packet */
  rr_log_packet_init(
    &pkt_info,
    LOG_GPRS_RR_CELL_RESELECTION_PARAMS_C,
    sizeof(LOG_GPRS_RR_CELL_RESELECTION_PARAMS_C_type),
    gas_id
  );

  log_ptr = (LOG_GPRS_RR_CELL_RESELECTION_PARAMS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    // Fill packet and commit it
    if (scell_ptr->gsm.gprs_indicator)
    {
      /* GRR51 config */
      log_ptr->PBCCH_PRESENT            = 0;
      log_ptr->CELL_BAR_ACCESS_2        = 0;
      log_ptr->RXLEV_ACCESS_MIN         = scell_ptr->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN;
      log_ptr->MS_TXPWR_MAX_CCH         = scell_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH;
      log_ptr->HSC_PARAM_PRESENT        = 0;
      log_ptr->HCS_THR                  = 0;
      log_ptr->PRIORITY_CLASS           = 0;
      log_ptr->CELL_RESELECT_HYST       = scell_ptr->gsm.cell_selection_parameters.CELL_RESELECT_HYSTERESIS;
      log_ptr->C31_HYST                 = 0;
      log_ptr->C31_QUAL                 = 0;
      log_ptr->RA_RESELECT_HYST_PRESENT = 0;
      log_ptr->RA_RESELECT_HYSTERESIS   = 0;

      log_ptr->RA_CODE                  = scell_ptr->bcch_sys_info.routing_area_code;
      log_ptr->RA_COLOUR                = scell_ptr->bcch_sys_info.ra_colour;
      log_ptr->NECI                     = scell_ptr->gsm.cell_selection_parameters.NECI;
      log_ptr->ACS                      = scell_ptr->gsm.cell_selection_parameters.ACS;
      log_ptr->OPT_RESELECT_PARAM_IND   = scell_ptr->gsm.cell_selection_parameters.CELL_RESELECT_PARAM_IND;
      log_ptr->CELL_BAR_QUALIFY         = scell_ptr->gsm.cell_selection_parameters.CELL_BAR_QUALIFY;
      log_ptr->POWER_OFFSET_PRESENT     = scell_ptr->gsm.cell_selection_parameters.power_offset_valid;
      log_ptr->POWER_OFFSET             = scell_ptr->gsm.cell_selection_parameters.POWER_OFFSET;
      log_ptr->CELL_BAR_ACCESS          = scell_ptr->gsm.RACH_control_parameters.CELL_BAR_ACCESS;
    } /* gprs_indicator */
    else /* gprs_indicator */
    {
    /* GSM config */
      log_ptr->PBCCH_PRESENT            = 0;
      log_ptr->CELL_BAR_ACCESS_2        = 0;
      log_ptr->RXLEV_ACCESS_MIN         = scell_ptr->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN;
      log_ptr->MS_TXPWR_MAX_CCH         = scell_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH;
      log_ptr->HSC_PARAM_PRESENT        = 0;
      log_ptr->HCS_THR                  = 0;
      log_ptr->PRIORITY_CLASS           = 0;
      log_ptr->CELL_RESELECT_HYST       = scell_ptr->gsm.cell_selection_parameters.CELL_RESELECT_HYSTERESIS;
      log_ptr->C31_HYST                 = 0;
      log_ptr->C31_QUAL                 = 0;
      log_ptr->RA_RESELECT_HYST_PRESENT = 0;
      log_ptr->RA_RESELECT_HYSTERESIS   = 0;

      log_ptr->RA_CODE                  = 0;
      log_ptr->RA_COLOUR                = 0;
      log_ptr->NECI                     = scell_ptr->gsm.cell_selection_parameters.NECI;
      log_ptr->ACS                      = scell_ptr->gsm.cell_selection_parameters.ACS;
      log_ptr->OPT_RESELECT_PARAM_IND   = scell_ptr->gsm.cell_selection_parameters.CELL_RESELECT_PARAM_IND;
      log_ptr->CELL_BAR_QUALIFY         = scell_ptr->gsm.cell_selection_parameters.CELL_BAR_QUALIFY;
      log_ptr->POWER_OFFSET_PRESENT     = scell_ptr->gsm.cell_selection_parameters.power_offset_valid;
      log_ptr->POWER_OFFSET             = scell_ptr->gsm.cell_selection_parameters.POWER_OFFSET;
      log_ptr->CELL_BAR_ACCESS          = scell_ptr->gsm.RACH_control_parameters.CELL_BAR_ACCESS;
    } /* gprs_indicator */

      /* commit the log packet. */
    gsmdiag_log_commit(&pkt_info);
  } /* endif log_ptr */

  return (log_ptr != NULL);
}

/*!
 * \brief Outputs LOG_GPRS_RR_CELL_RESELECTION_MEASMNTS_C log packet.
 *
 * \param gprs_reselection_info_ptr
 * \param recent_reselection
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_gprs_cell_reselection_measurements(
  gprs_reselection_structure_T    *gprs_reselection_info_ptr,
  boolean                         recent_reselection,
  const gas_id_t                  gas_id
)
{
  LOG_GPRS_RR_CELL_RESELECTION_MEASMNTS_C_type *log_ptr;
  gsmdiag_pkt_info_t                            pkt_info;
  gprs_scell_info_t       *scell_info_ptr       = rr_gprs_get_scell_info(gas_id);
  rr_cell_type_T           serv_cell_type;
  LAI_T                    serv_lai;
  byte                     serv_ra_colour;
  byte  i;

  rr_log_packet_init(
    &pkt_info,
    LOG_GPRS_RR_CELL_RESELECTION_MEASMNTS_C,
    sizeof(LOG_GPRS_RR_CELL_RESELECTION_MEASMNTS_C_type),
    gas_id
  );

  log_ptr = (LOG_GPRS_RR_CELL_RESELECTION_MEASMNTS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr == NULL)
  {
    return FALSE;
  }

  serv_lai = scell_info_ptr->gsm.location_area_identification;
  serv_ra_colour = scell_info_ptr->bcch_sys_info.ra_colour;

  // Fill log packet
  log_ptr->serving_BCCH_ARFCN       = rr_arfcn_to_word(gprs_reselection_info_ptr->serving_values.BCCH_ARFCN);
  log_ptr->serving_RXLEV_average    = gprs_reselection_info_ptr->serving_values.RXLEV_average;
  log_ptr->serving_C1               = gprs_reselection_info_ptr->serving_values.C1;
  log_ptr->serving_C2               = gprs_reselection_info_ptr->serving_values.C2;
  log_ptr->serving_C31              = gprs_reselection_info_ptr->serving_values.C31;
  log_ptr->serving_C32              = gprs_reselection_info_ptr->serving_values.C32;
  log_ptr->recent_cell_reselection = (byte) recent_reselection;
  log_ptr->serving_PBCCH_ARFCN    = 0;
  log_ptr->serving_PRIORITY_CLASS = 0;

  serv_cell_type = rr_get_cell_priority(
    scell_info_ptr->gsm.cell_selection_parameters.CELL_BAR_QUALIFY,
    scell_info_ptr->gsm.RACH_control_parameters.CELL_BAR_ACCESS,
    &(scell_info_ptr->gsm.location_area_identification),
    scell_info_ptr->gsm.RACH_control_parameters.access_control,
    NULL,
    gas_id
  );

  log_ptr->cell_reselect_status = (serv_cell_type == RR_CELL_BARRED ? 0x1 : 0x0);

  if (scell_info_ptr->gsm.reselection_info.five_second_timer_id != 0)
  {
    log_ptr->serving_five_second_timer_state =
      (byte)rr_timer_get_state(scell_info_ptr->gsm.reselection_info.five_second_timer_id, gas_id);
  }
  else
  {
    log_ptr->serving_five_second_timer_state = (byte)RR_TIMER_STOPPED;
  }

  /* ASSERT(gprs_reselection_info_ptr->no_of_entries <= RR_LOG_MAX_NUM_NEIGHBORS);  */
  if (gprs_reselection_info_ptr->no_of_entries > RR_LOG_MAX_NUM_NEIGHBORS)
  {
    MSG_GERAN_HIGH_0_G("Some meas may be dropped since gprs_reselection_info_ptr->no_of_entries > RR_LOG_MAX_NUM_NEIGHBORS");
  }

  /* Fill Surrounding Cell details next */
  for (i = 0; i < gprs_reselection_info_ptr->no_of_entries && i < RR_LOG_MAX_NUM_NEIGHBORS; i++)
  {
    rr_resel_candidate_t *    resel_candidate;
    gsm_ncell_list_member_t * ncell_data_ptr;

    resel_candidate = rr_resel_best_six_find(
      gprs_reselection_info_ptr->surrounding_values[i].BCCH_ARFCN,
      NULL,
      gas_id
    );

    if (resel_candidate == NULL)
    {
      MSG_GERAN_HIGH_3_G("Cannot find resel_candidate in best six for ARFCN %d (band %d)",
               gprs_reselection_info_ptr->surrounding_values[i].BCCH_ARFCN.num,
               gprs_reselection_info_ptr->surrounding_values[i].BCCH_ARFCN.band,
               0);

      memset((void *) &(log_ptr->neighbor_cell_info[i]),0,sizeof(rr_log_gprs_surround_info_T));
      continue;
    }

    ncell_data_ptr = resel_candidate->ncell_data;

    if (ncell_data_ptr == NULL)
    {
      MSG_GERAN_HIGH_1_G("Cannot find ncell entry %d",gprs_reselection_info_ptr->surrounding_values[i].BCCH_ARFCN.num);
      memset((void *) &(log_ptr->neighbor_cell_info[i]),0,sizeof(rr_log_gprs_surround_info_T));
      continue;
    }

    log_ptr->neighbor_cell_info[i].BCCH_ARFCN    = rr_arfcn_to_word(gprs_reselection_info_ptr->surrounding_values[i].BCCH_ARFCN);
    log_ptr->neighbor_cell_info[i].RXLEV_average = gprs_reselection_info_ptr->surrounding_values[i].RXLEV_average;
    log_ptr->neighbor_cell_info[i].C1            = gprs_reselection_info_ptr->surrounding_values[i].C1;
    log_ptr->neighbor_cell_info[i].C2            = gprs_reselection_info_ptr->surrounding_values[i].C2;
    log_ptr->neighbor_cell_info[i].C31           = gprs_reselection_info_ptr->surrounding_values[i].C31;
    log_ptr->neighbor_cell_info[i].C32           = gprs_reselection_info_ptr->surrounding_values[i].C32;

    {
      rr_cell_type_T surr_cell_type;

      surr_cell_type = rr_get_cell_priority(
        ncell_data_ptr->ncell_data.bcch.gsm.cell_selection_parameters.CELL_BAR_QUALIFY,
        ncell_data_ptr->ncell_data.bcch.gsm.RACH_control_parameters.CELL_BAR_ACCESS,
        &ncell_data_ptr->ncell_data.bcch.gsm.location_area_identification,
        ncell_data_ptr->ncell_data.bcch.gsm.RACH_control_parameters.access_control,
        NULL,
        gas_id
      );

      log_ptr->neighbor_cell_info[i].PBCCH_ARFCN = 0;
      log_ptr->neighbor_cell_info[i].Priority_Class = 0;
      log_ptr->neighbor_cell_info[i].cell_reselect_status = (surr_cell_type == RR_CELL_BARRED ? 0x1 : 0x0);

      if (resel_candidate->reselection_info.five_second_timer_id != 0 )
      {
        log_ptr->neighbor_cell_info[i].five_second_timer_state = (byte) rr_timer_get_state(
          resel_candidate->reselection_info.five_second_timer_id,
          gas_id
        );
      }

      if (ncell_data_ptr->ncell_data.bcch.gprs_supported)
      {
        log_ptr->neighbor_cell_info[i].serving_lai_or_ra = ((ncell_data_ptr->ncell_data.bcch.gprs.ra_colour == serv_ra_colour ) ? 0x1 : 0x0 );
      }
      else
      {
        // Not in serving RA but in serving LAI if cell does not support GPRS
        log_ptr->neighbor_cell_info[i].serving_lai_or_ra = (rr_same_lai(&(ncell_data_ptr->ncell_data.bcch.gsm.location_area_identification),&serv_lai ) ? 0x1 : 0x0 );
      }
    }
  }

  log_ptr->neighbor_cell_count = i;


  // commit the log packet.
  gsmdiag_log_commit(&pkt_info);

  return (TRUE);
} /* rr_log_cell_reselection_params */

/**
 * 1) Holds the last reselection trigger value
 * 2) This is also used as a flag to send RR abort indication to MM when a cell reselection is forced
 * due to serving cell getting barred during connection establishment.
 */
rr_log_reselection_trigger_T rr_log_reselection_trigger;

/*!
 * \brief Sets the reselection trigger.
 *
 * \param trigger
 * \param gas_id
 */
void rr_log_set_reselection_trigger(
  rr_log_reselection_trigger_T trigger,
  const gas_id_t gas_id
)
{
  rr_log_reselection_trigger = trigger;
} // rr_log_set_reselection_trigger

/*!
 * \brief Returns the last stored reselection trigger.
 *
 * \param gas_id
 *
 * \return rr_log_reselection_trigger_T
 */
rr_log_reselection_trigger_T rr_log_get_reselection_trigger(const gas_id_t gas_id)
{
  return rr_log_reselection_trigger;
} // rr_log_get_reselection_trigger

/*!
 * \brief Outputs EVENT_GSM_LINK_FAILURE event.
 *
 * \param gas_id
 *
 * \return boolean
 */
void rr_log_link_failure(
  const gas_id_t gas_id
)
{
  typedef PACKED struct PACKED_POST
  {
    byte  sub_id;
    byte  dedicated_link_failure;
  } payload_T;
  payload_T payload;

  payload.sub_id        = geran_get_diag_sub_id(gas_id);
  payload.dedicated_link_failure = 1;

  geran_diag_event(EVENT_GSM_LINK_FAILURE, EVENTS_DS_GSM_LINK_FAILURE, sizeof(payload), (byte *)&payload);

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_rlf(gas_id);
#endif // FEATURE_QSH_EVENT_METRIC

  return;
} /* rr_log_link_failure */

/*!
 * \brief Outputs EVENT_GSM_CELL_SELECTED event.
 * 
 * \param gas_id 
 * 
 */
void rr_log_cell_camp_status(
  const gas_id_t gas_id
)
{
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    byte netw_sel_mode;
  } payload_T;
  payload_T payload;

  payload.sub_id        = geran_get_diag_sub_id(gas_id);

  if (rr_is_limited_plmn_selection_mode(gas_id))
  {
    payload.netw_sel_mode = 1;
  }
  else
  {
    // Default NETWORK SELECTION MODE is set to Automatic
    payload.netw_sel_mode = 0;
  }

  geran_diag_event(EVENT_GSM_CELL_SELECTED, EVENT_GSM_DS_CELL_SELECTED, sizeof(payload), (byte *)&payload);
  
  return;
} /* rr_log_cell_camp_status */

/*!
 * \brief Log RACH requests.
 *
 * \param attempt
 * \param frameNum
 * \param reason
 * \param gas_id
 */
void rr_log_random_access_req(
  byte attempt,
  word frameNum,
  byte reason,
  const gas_id_t gas_id
)
{
  typedef PACKED struct PACKED_POST
  {
    byte  sub_id;
    byte  attempt;
    word  FNmod42432;
    byte  access_reason;
  } payload_T;
  payload_T payload;

  payload.sub_id        = geran_get_diag_sub_id(gas_id);
  payload.attempt = attempt;
  payload.FNmod42432 = frameNum;
  payload.access_reason = reason;  

  geran_diag_event(EVENT_RANDOM_ACCESS_REQUEST, EVENTS_DS_RANDOM_ACCESS_REQUEST, sizeof(payload), (byte *)&payload);

  return;
}

/*!
 * \brief Outputs EVENT_GSM_HANDOVER_START event.
 *
 * \param handover_type (in) - type of handover
 * \param local_arfcn (in)   - ARFCN of the target cell
 * \param bsic (in)          - BSIC of the target cell
 * \param handover_ref (in)  - handover reference (0 is not available)
 * \param gas_id
 */
void rr_log_handover_start(
  rr_log_handover_type_T  handover_type,
  ARFCN_T                 local_arfcn,
  BSIC_T                  bsic,
  byte                    handover_ref,
  const gas_id_t          gas_id
)
{
    typedef PACKED struct PACKED_POST
    {
      byte  sub_id;
      byte  handover_type;
      word  arfcn;
      byte  bsic;
      byte  handover_ref;
    } payload_T;
    payload_T payload;

  payload.sub_id        = geran_get_diag_sub_id(gas_id);
    payload.handover_type = (byte) handover_type;
    payload.arfcn         = rr_arfcn_to_word(local_arfcn);
    payload.bsic          = (byte) (bsic.PLMN_colour_code<<3)+bsic.BS_colour_code;
    payload.handover_ref  = handover_ref;

  geran_diag_event(EVENT_GSM_HANDOVER_START, EVENTS_DS_GSM_HANDOVER_START, sizeof(payload), (byte *)&payload);

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_handover(handover_type, gas_id);
#endif // FEATURE_QSH_EVENT_METRIC

  return;
} /* rr_log_handover_start */

static void rr_log_handover_end(byte failure_cause, const gas_id_t gas_id)
{
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    byte success;
    byte err_code;
  } ho_end_payload_T;
  ho_end_payload_T payload;

  payload.sub_id   = geran_get_diag_sub_id(gas_id);

  if (failure_cause == 0xFF)
  {
    payload.success = 1; /* SUCCESS = true */
    payload.err_code = 0xFF;
  }
  else
  {
    payload.success = 0; /* SUCCESS = false */
    payload.err_code = failure_cause;
  }

  geran_diag_event(EVENT_GSM_HANDOVER_END, EVENTS_DS_GSM_HANDOVER_END, sizeof(payload), (byte *)&payload);

  return;
}

/*!
 * \brief Outputs HANDOVER_END event indicating success.
 *
 * \param gas_id
 */
void rr_log_handover_success(const gas_id_t gas_id)
{
  rr_log_handover_end(0xFF, gas_id);
} /* rr_log_handover_success */

/*!
 * \brief Outputs HANDOVER_END event indicating failure (with cause).
 *
 * \param failure_reason (in) - reason for failure
 * \param gas_id
 */
void rr_log_handover_failure(rr_log_handover_failure_T failure_reason, const gas_id_t gas_id)
  {
  rr_log_handover_end(failure_reason, gas_id);

#ifdef FEATURE_QSH_EVENT_METRIC
  {
    rr_dedicated_data_T *dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
    if (dedic_ptr != NULL)
    {
      rr_qsh_metric_log_conn_end_info(
        SYS_CALL_END_EVENT_HANDOVER_FAIL, 
        (uint8)rr_qsh_metric_get_sys_ho_type(dedic_ptr->handover_details.handover_type), 
        gas_id
      );
    }
  }
#endif /* FEATURE_QSH_EVENT_METRIC */
} /* rr_log_handover_failure */

/*!
 * \brief Outputs EVENTS_DS_GSM_RESELECT_START or EVENT_GSM_RESELECT_START event.
 *
 * \param trigger - reason why the reselection started
 * \param gas_id
 */
void rr_log_reselect_start(rr_log_reselection_trigger_T trigger, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_log_data_t *rr_log_data_ptr = rr_log_get_data_ptr(gas_id);

  if (rr_log_data_ptr != NULL)
  {
    typedef PACKED struct PACKED_POST
    {
      byte sub_id;
      byte cause;
    } reselect_start_payload_T;
    reselect_start_payload_T payload;

    payload.sub_id = geran_get_diag_sub_id(gas_id);
    payload.cause  = (byte)trigger;

    geran_diag_event(EVENT_GSM_RESELECT_START, EVENTS_DS_GSM_RESELECT_START, sizeof(payload), (byte *)&payload);  

    if (rr_log_data_ptr->rr_log_reselection_counter != 0)
    {
      MSG_GERAN_ERROR_0_G("Reselection log counter mismatch");
    }
    rr_log_data_ptr->rr_log_reselection_counter = 1;
  }

#ifdef FEATURE_QSH_EVENT_METRIC
  if (trigger == RR_RESELECTION_TRIGGER_GSM_TO_WCDMA)
  {
    rr_qsh_metric_log_resel_g2wt(gas_id);
  }
  else
  {
    rr_qsh_metric_log_resel_g2g(gas_id);
  }
#endif // FEATURE_QSH_EVENT_METRIC

  return;
} /* rr_log_reselect_start */

/*!
 * \brief Outputs EVENT_GSM_RESELECT_END and EVENT_GSM_RESELECT_END_V2 events indicating success.
 *          Outputs the DS versions of these events for Multisim.
 *
 * \param arfcn (in) - ARFCN of the new cell
 * \param bsic (in)  - BSIC of the new cell
 * \param gas_id
 */
void rr_log_reselect_success(
  ARFCN_T arfcn,
  BSIC_T bsic,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_log_data_t *rr_log_data_ptr = rr_log_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_log_data_ptr);

  if (rr_log_data_ptr->rr_log_reselection_counter == 1)
  {
    typedef PACKED struct PACKED_POST
    {
      byte sub_id;
      word arfcn;
      byte bsic;
      byte failure_reason;
    } reselect_end_payload_T;
    reselect_end_payload_T payload;

    typedef PACKED struct PACKED_POST
    {
      byte  sub_id;
      uint8 band;
      word  arfcn;
      byte  bsic;
      byte  failure_reason;
    } reselect_end_payload_v2_T;
    reselect_end_payload_v2_T payload_v2;

    payload.sub_id         = geran_get_diag_sub_id(gas_id);
    payload.arfcn          = rr_arfcn_to_word(arfcn);
    payload.bsic           = (byte) (bsic.PLMN_colour_code<<3)+bsic.BS_colour_code;
    payload.failure_reason = 0xFF;  /* FAILURE_REASON = NA */

    geran_diag_event(EVENT_GSM_RESELECT_END, EVENTS_DS_GSM_RESELECT_END, sizeof(payload), (byte *)&payload);  

    payload_v2.sub_id         = geran_get_diag_sub_id(gas_id);
    payload_v2.arfcn          = arfcn.num;
    payload_v2.band           = GET_ARFCN_BAND(arfcn);
    payload_v2.bsic           = (byte) (bsic.PLMN_colour_code<<3)+bsic.BS_colour_code;
    payload_v2.failure_reason = 0xFF;  /* FAILURE_REASON = NA */
  
    geran_diag_event(EVENT_GSM_RESELECT_END_V2, EVENTS_DS_GSM_RESELECT_END_V2, sizeof(payload_v2), (byte *)&payload_v2);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Non-matching reselection end event");
  }

  rr_log_data_ptr->rr_log_reselection_counter = 0;

  return;
} /* rr_log_reselect_success */

/*!
 * \brief Outputs EVENT_GSM_RESELECT_END and EVENT_GSM_RESELECT_END_V2 events indicating failure (with cause).
 *          Outputs the DS versions of these events for Multisim.
 *
 * \param failure_reason - reason for the failure
 * \param gas_id
 */
void rr_log_reselect_failure(
  rr_log_reselection_failure_T failure_reason,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_log_data_t *rr_log_data_ptr = rr_log_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_log_data_ptr);

  if (rr_log_data_ptr->rr_log_reselection_counter == 1)
  {
    typedef PACKED struct PACKED_POST
    {
      byte sub_id;
      word arfcn;
      byte bsic;
      byte failure_reason;
    } reselect_end_payload_T;
    reselect_end_payload_T payload;

    typedef PACKED struct PACKED_POST
    {
      byte  sub_id;
      word  arfcn;
      uint8 band;
      byte  bsic;
      byte  failure_reason;
    } reselect_end_payload_v2_T;
    reselect_end_payload_v2_T payload_v2;

    payload.sub_id         = geran_get_diag_sub_id(gas_id);
    payload.arfcn          = INVALID_LOG_ARFCN;
    payload.bsic           = 0xFF;  /* BSIC NA  */
    payload.failure_reason = (byte) failure_reason;

    geran_diag_event(EVENT_GSM_RESELECT_END, EVENTS_DS_GSM_RESELECT_END, sizeof(payload), (byte *)&payload);  

    payload_v2.sub_id         = geran_get_diag_sub_id(gas_id);
    payload_v2.arfcn          = INVALID_LOG_ARFCN;
    payload_v2.band           = 0xFF;  /* Band None */
    payload_v2.bsic           = 0xFF;  /* BSIC NA  */
    payload_v2.failure_reason = (byte) failure_reason;

    geran_diag_event(EVENT_GSM_RESELECT_END_V2, EVENTS_DS_GSM_RESELECT_END_V2, sizeof(payload_v2), (byte *)&payload_v2);  
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Non-matching reselection end event");
  }
  rr_log_data_ptr->rr_log_reselection_counter = 0;

  return;
} /* rr_log_reselect_failure */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
/*!
 * \brief Outputs EVENT_GSM_TO_WCDMA_RESELECT_END(for single sim) or EVENT_DS_GSM_TO_WCDMA_RESELECT_END(for dual sim) event
 *  indicating success or failure (with cause).
 *
 * \param uarfcn (in)         - W cell UARFCN
 * \param scr_code (in)       - W cell PSC
 * \param failure_reason (in) - reason for the failure or 0xFF for success
 * \param gas_id
 */
static void rr_log_g2w_reselect_end(
  word uarfcn,
  word scr_code,
  rr_log_reselection_failure_T failure_reason,
  const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_log_data_t *rr_log_data_ptr = rr_log_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_log_data_ptr);

  if(RR_LOG_G2W_RESEL_STATUS_SUCCESS == failure_reason)
  {
    MSG_GERAN_HIGH_0_G("Reselection to WCDMA is successful");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Reselection to WCDMA failed");
  }

  if (rr_log_data_ptr->rr_log_reselection_counter == 1)
  {
    typedef PACKED struct PACKED_POST
    {
      byte sub_id;
      word uarfcn;
      word scrambling_code;
      byte failure_reason;
    } g2w_reselect_end_payload_T;
    g2w_reselect_end_payload_T payload;

    payload.sub_id          = geran_get_diag_sub_id(gas_id);
    payload.uarfcn          = uarfcn;  /* UARFCN */
    payload.scrambling_code = scr_code;  /* SC  */
    payload.failure_reason  = (byte) failure_reason;

    geran_diag_event(EVENT_GSM_TO_WCDMA_RESELECT_END, EVENTS_DS_GSM_TO_WCDMA_RESELECT_END, sizeof(payload), (byte *)&payload);  
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Non-matching G2W reselection end event");
  }

  rr_log_data_ptr->rr_log_reselection_counter = 0;

  return;
}/* rr_log_g2w_reselect_end */
/*!
 * \brief Outputs EVENT_GSM_TO_WCDMA_RESELECT_END event indicating failure (with cause).
 *
 * \param uarfcn (in)         - W cell UARFCN
 * \param scr_code (in)       - W ccell PSC
 * \param failure_reason (in) - reason for the failure
 * \param gas_id
 */
void rr_log_g2w_reselect_failure(
  word uarfcn,
  word scr_code,
  rr_log_reselection_failure_T failure_reason,
  const gas_id_t gas_id
)
{
  rr_log_g2w_reselect_end(
    uarfcn,
    scr_code,
    failure_reason,
    gas_id);

  return;
} /* rr_log_g2w_reselect_failure */

/*!
 * \brief Outputs EVENT_GSM_TO_WCDMA_RESELECT_END event indicating success.
 *
 * \param uarfcn (in)   - UARFCN of the new W cell
 * \param scr_code (in) - PSC of the new W cell
 * \param gas_id
 */
void rr_log_g2w_reselect_success(
  word uarfcn,
  word scr_code,
  const gas_id_t gas_id
)
{
  rr_log_g2w_reselect_end(
    uarfcn,
    scr_code,
    RR_LOG_G2W_RESEL_STATUS_SUCCESS,
    gas_id);

  return;
} /* rr_log_g2w_reselect_success */

/*!
 * \brief Outputs EVENT_GSM_TO_WCDMA_REDIR_STARTED event.
 *
 * \param gas_id
 */
void rr_log_g2w_redir_started(
  const gas_id_t gas_id
)
{
  byte payload = geran_get_diag_sub_id(gas_id);
  geran_diag_event(EVENT_GSM_TO_WCDMA_REDIR_STARTED, EVENTS_DS_GSM_TO_WCDMA_REDIR_STARTED, sizeof(payload), &payload);

  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_WCDMA_REDIR_ENDED(single sim) or 
 * EVENT_GSM_TO_WCDMA_REDIR_ENDED(multisim) event indicating success or failure.
 *
 * \param status - G2W redirection status
 * \param gas_id - subscription id
 */
void rr_log_g2w_redir_end(
  byte status, 
  const gas_id_t gas_id
)
{
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    uint8 status; /* Redirection status */
  } g2w_redir_ended_payload_T;
  g2w_redir_ended_payload_T payload;

  payload.sub_id          = geran_get_diag_sub_id(gas_id);
  payload.status          = status;
  
  geran_diag_event(EVENT_GSM_TO_WCDMA_REDIR_ENDED, EVENTS_DS_GSM_TO_WCDMA_REDIR_ENDED, sizeof(payload), (byte *)&payload);  

  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_WCDMA_REDIR_ENDED(single sim) or  
 * EVENTS_DS_GSM_TO_WCDMA_REDIR_ENDED(multi sim)event indicating success.
 *
 * \param gas_id
 */
void rr_log_g2w_redir_success(
  const gas_id_t gas_id
)
{
  rr_log_g2w_redir_end(RR_LOG_G2W_REDIR_STATUS_SUCCESS, gas_id);
  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_WCDMA_REDIR_ENDED(single sim) or  
 * EVENTS_DS_GSM_TO_WCDMA_REDIR_ENDED(multi sim)event indicating redirection was aborted.
 *
 * \param gas_id
 */
void rr_log_g2w_redir_aborted(
  const gas_id_t gas_id
)
{
  rr_log_g2w_redir_end(RR_LOG_G2W_REDIR_STATUS_ABORTED, gas_id);
  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_WCDMA_REDIR_ENDED(single sim) or  
 * EVENTS_DS_GSM_TO_WCDMA_REDIR_ENDED(multi sim)event indicating failure.
 *
 * \param gas_id
 */
void rr_log_g2w_redir_failure(
  const gas_id_t gas_id
)
{
  rr_log_g2w_redir_end(RR_LOG_G2W_REDIR_STATUS_FAILURE, gas_id);
  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_TDSCDMA_REDIR_STARTED event.
 *
 * \param gas_id
 */
void rr_log_g2t_redir_started(
  const gas_id_t gas_id
)
{
  byte payload = geran_get_diag_sub_id(gas_id);
  geran_diag_event(EVENT_GSM_TO_TDSCDMA_REDIR_STARTED, EVENTS_DS_GSM_TO_TDSCDMA_REDIR_STARTED, sizeof(payload), &payload);

  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_TDSCDMA_REDIR_ENDED(single sim) or 
 * EVENT_GSM_TO_TDSCDMA_REDIR_ENDED(multisim) event indicating success or failure.
 *
 * \param status - G2T redirection status
 * \param gas_id - subscription id
 */
void rr_log_g2t_redir_end(
  byte status, 
  const gas_id_t gas_id
)
{
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    uint8 status; /* Redirection status */
  } g2t_redir_ended_payload_T;
  g2t_redir_ended_payload_T payload;

  payload.sub_id          = geran_get_diag_sub_id(gas_id);
  payload.status          = status;
  
  geran_diag_event(EVENT_GSM_TO_TDSCDMA_REDIR_ENDED, EVENTS_DS_GSM_TO_TDSCDMA_REDIR_ENDED, sizeof(payload), (byte *)&payload);  

  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_TDSCDMA_REDIR_ENDED(single sim) or 
 * EVENT_GSM_TO_TDSCDMA_REDIR_ENDED(multisim) event indicating success.
 *
 * \param gas_id
 */
void rr_log_g2t_redir_success(
  const gas_id_t gas_id
)
{
  rr_log_g2t_redir_end(RR_LOG_G2T_REDIR_STATUS_SUCCESS, gas_id);
  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_TDSCDMA_REDIR_ENDED(single sim) or 
 * EVENT_GSM_TO_TDSCDMA_REDIR_ENDED(multisim) event indicating it was aborted.
 *
 * \param gas_id
 */
void rr_log_g2t_redir_aborted(
  const gas_id_t gas_id
)
{
  rr_log_g2t_redir_end(RR_LOG_G2T_REDIR_STATUS_ABORTED, gas_id);
  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_TDSCDMA_REDIR_ENDED(single sim) or 
 * EVENT_GSM_TO_TDSCDMA_REDIR_ENDED(multisim) event indicating failure.
 *
 * \param gas_id
 */
void rr_log_g2t_redir_failure(
  const gas_id_t gas_id
)
{
  rr_log_g2t_redir_end(RR_LOG_G2T_REDIR_STATUS_FAILURE, gas_id);
  return;
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

#ifdef FEATURE_GSM_TO_LTE

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_RESEL_STARTED(single sim) or EVENTS_DS_GSM_TO_LTE_RESEL_STARTED(multisim) event.
 *
 * \param earfcn
 * \param pcid
 * \param gas_id
 */
void rr_log_g2l_resel_started(
  lte_earfcn_t earfcn,
  uint16 pcid,
  const gas_id_t gas_id
)
{
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;  
    uint16 earfcn;
    uint16 pcid;
  } g2l_resel_started_payload_T;
  g2l_resel_started_payload_T payload;

  payload.sub_id = geran_get_diag_sub_id(gas_id);
  payload.earfcn = (uint16)earfcn;
  payload.pcid = pcid;

  geran_diag_event(EVENT_GSM_TO_LTE_RESEL_STARTED, EVENTS_DS_GSM_TO_LTE_RESEL_STARTED, sizeof(payload), (byte *)&payload);  

  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_RESEL_ENDED(for single sim) or EVENTS_DS_GSM_TO_LTE_RESEL_ENDED(for dual sim) event
 *  indicating success or failure (with cause).
 *
 * \param earfcn (in)         - L cell EARFCN
 * \param pc_id (in)       - L Cell id 
 * \param status (in) - failure cause or success status value
 * \param gas_id
 */
static void rr_log_g2l_reselect_end(
  lte_earfcn_t earfcn,
  uint16       pcid,
  rr_log_g2l_resel_status_e status,
  const gas_id_t gas_id
)
{
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    uint16 earfcn;
    uint16 pcid;
    uint8  status;
  } g2l_resel_ended_payload_T;
  g2l_resel_ended_payload_T payload;

  payload.sub_id = geran_get_diag_sub_id(gas_id);
  payload.earfcn = (uint16)earfcn;
  payload.pcid = pcid;
  payload.status = (uint8) status;

  geran_diag_event(EVENT_GSM_TO_LTE_RESEL_ENDED, EVENTS_DS_GSM_TO_LTE_RESEL_ENDED, sizeof(payload), (byte *)&payload);  

  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_RESEL_ENDED(single sim) or EVENTS_DS_GSM_TO_LTE_RESEL_ENDED(multisim) event with success status.
 *
 * \param earfcn
 * \param pcid
 * \param gas_id
 */
void rr_log_g2l_resel_success(
  lte_earfcn_t earfcn,
  uint16 pcid,
  const gas_id_t gas_id
)
{
  rr_log_g2l_reselect_end(earfcn, pcid, RR_LOG_G2L_RESEL_STATUS_SUCCESS, gas_id);

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_resel_g2l(gas_id);
#endif // FEATURE_QSH_EVENT_METRIC

  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_RESEL_ENDED(single sim) or EVENTS_DS_GSM_TO_LTE_RESEL_ENDED(multisim) event indicating it was aborted.
 *
 * \param earfcn
 * \param pcid
 * \param gas_id
 */
void rr_log_g2l_resel_aborted(
  lte_earfcn_t earfcn,
  uint16       pcid,
  const gas_id_t gas_id
)
{
  rr_log_g2l_reselect_end(earfcn, pcid, RR_LOG_G2L_RESEL_STATUS_ABORTED, gas_id);
  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_RESEL_ENDED(single sim) or EVENTS_DS_GSM_TO_LTE_RESEL_ENDED(multisim) event indicating failure.
 *
 * \param earfcn
 * \param pcid
 * \param status
 * \param gas_id
 */
void rr_log_g2l_resel_failure(
  lte_earfcn_t earfcn,
  uint16       pcid,
  rr_log_g2l_resel_status_e status,
  const gas_id_t gas_id
)
{
  rr_log_g2l_reselect_end(earfcn, pcid, status, gas_id);
  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_REDIR_STARTED(single sim) or EVENTS_DS_GSM_TO_LTE_REDIR_STARTED(multisim) event.
 *
 * \param gas_id
 */
void rr_log_g2l_redir_started(
  const gas_id_t gas_id
)
{
  byte payload = geran_get_diag_sub_id(gas_id);
  geran_diag_event(EVENT_GSM_TO_LTE_REDIR_STARTED, EVENTS_DS_GSM_TO_LTE_REDIR_STARTED, sizeof(payload), &payload);  

  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_REDIR_ENDED(for single sim) or EVENTS_DS_GSM_TO_LTE_REDIR_ENDED(for multisim) event
 *  indicating success or failure.
 *
 * \param status (in) - failure or success status value
 * \param gas_id
 */
static void rr_log_g2l_redir_end(
  byte status,
  const gas_id_t gas_id
)
{
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    uint8  status;
  } g2l_resel_ended_payload_T;
  g2l_resel_ended_payload_T payload;

  payload.sub_id = geran_get_diag_sub_id(gas_id);
  payload.status = (uint8) status;

  geran_diag_event(EVENT_GSM_TO_LTE_REDIR_ENDED, EVENTS_DS_GSM_TO_LTE_REDIR_ENDED, sizeof(payload), (byte *)&payload);  

  return;
}

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_REDIR_ENDED/EVENTS_DS_GSM_TO_LTE_REDIR_ENDED event indicating success.
 *
 * \param gas_id
 */
void rr_log_g2l_redir_success(
  const gas_id_t gas_id
)
{
  rr_log_g2l_redir_end(RR_LOG_G2L_REDIR_STATUS_SUCCESS, gas_id);
  return;
}

/*!
 * \brief Output EVENT_GSM_TO_LTE_REDIR_ENDED/EVENTS_DS_GSM_TO_LTE_REDIR_ENDED event with status indicating it was aborted.
 *
 * \param gas_id
 */
void rr_log_g2l_redir_aborted(
  const gas_id_t gas_id
)
{
  rr_log_g2l_redir_end(RR_LOG_G2L_REDIR_STATUS_ABORTED, gas_id);
  return;
}

/*!
 * \brief Output EVENT_GSM_TO_LTE_REDIR_ENDED/EVENTS_DS_GSM_TO_LTE_REDIR_ENDED event with status indicating failure.
 *
 * \param gas_id
 */
void rr_log_g2l_redir_failure(
  const gas_id_t gas_id
)
{
  rr_log_g2l_redir_end(RR_LOG_G2L_REDIR_STATUS_FAILURE, gas_id);
  return;
}

#endif /* FEATURE_GSM_TO_LTE */

#ifdef FEATURE_GSM_TDS

void rr_log_g2t_reselect_started(word uarfcn, word cpid, const gas_id_t gas_id)
{
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    word uarfcn;
    word cpid;
  } g2t_resel_started_payload_T;
  g2t_resel_started_payload_T payload;
  
  payload.sub_id = geran_get_diag_sub_id(gas_id);
  payload.uarfcn = uarfcn;
  payload.cpid = cpid;

  geran_diag_event(EVENT_GSM_TO_TDSCDMA_RESEL_START, EVENTS_DS_GSM_TO_TDSCDMA_RESEL_START, 
    sizeof(payload), (byte *)&payload);
}

/*!
 * \brief Outputs EVENTS_DS_GSM_TO_TDSCDMA_RESEL_END(for single sim) or EVENTS_DS_GSM_TO_TDSCDMA_RESEL_END(for dual sim) event
 *  indicating success or failure (with cause).
 *
 * \param uarfcn (in)         - TD cell UARFCN
 * \param cpid (in)       - TD cell id
 * \param failure_reason (in) - reason for the failure or success code
 * \param gas_id
 */
static void rr_log_g2t_reselect_end(
  word uarfcn,
  word cpid,
  rr_log_reselection_failure_T failure_reason,
  const gas_id_t gas_id)
{
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    word uarfcn;
    word cpid;
    byte status;
  } g2t_resel_ended_payload_T;
  g2t_resel_ended_payload_T payload;
  
  payload.sub_id = geran_get_diag_sub_id(gas_id);
  payload.uarfcn = uarfcn;
  payload.cpid = cpid;
  payload.status = (byte)failure_reason;

  geran_diag_event(EVENT_GSM_TO_TDSCDMA_RESEL_END, EVENTS_DS_GSM_TO_TDSCDMA_RESEL_END, sizeof(payload), (byte *)&payload);  

  return;
}/* rr_log_g2t_reselect_end */

void rr_log_g2t_reselect_success(word uarfcn, word cpid, const gas_id_t gas_id)
{
  rr_log_g2t_reselect_end(uarfcn, cpid, RR_LOG_G2T_RESEL_STATUS_SUCCESS, gas_id);

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_resel_g2wt(gas_id);
#endif // FEATURE_QSH_EVENT_METRIC

  return;
}

void rr_log_g2t_reselect_failure(word uarfcn, word cpid, rr_log_reselection_failure_T failure_reason, const gas_id_t gas_id)
{

  rr_log_g2t_reselect_end(uarfcn, cpid, failure_reason, gas_id);
  return;
}

#endif

/*!
 * \brief Outputs EVENT_GSM_MESSAGE_RECEIVED event.
 *
 * \param message_id
 * \param pd_id
 * \param channel_id
 * \param gas_id
 */
void rr_log_gsm_message_received(
  byte message_id,
  byte pd_id,
  byte channel_id,
  const gas_id_t gas_id
)
{
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    byte message_id;
    byte pd_id;
    byte channel_id;
  } gsm_message_received_payload_T;
  gsm_message_received_payload_T payload;

  payload.sub_id = geran_get_diag_sub_id(gas_id);
  payload.message_id          = message_id;  /* UARFCN */
  payload.pd_id               = pd_id;  /* SC  */
  payload.channel_id          = channel_id;
  geran_diag_event(EVENT_GSM_MESSAGE_RECEIVED, EVENTS_DS_GSM_MESSAGE_RECEIVED, sizeof(payload), (byte *)&payload);  

  return;
} /* rr_log_reselect_failure */

/*!
 * \brief Returns TRUE if there is no outstanding reselection end event, FALSE otherwise.
 *
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_log_reselection_log_complete(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_log_data_t *rr_log_data_ptr = rr_log_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_log_data_ptr);

  return (rr_log_data_ptr->rr_log_reselection_counter == 0);
} /* rr_log_reselection_log_complete */

/*!
 * \brief Outputs a in-service or out-of-service event.
 *
 * \param in_service (in) - TRUE if in service; FALSE otherwise
 * \param gas_id
 */
void rr_log_service_change(
  boolean in_service,
  const gas_id_t gas_id
)
{
  byte payload = geran_get_diag_sub_id(gas_id);
  // Obtain a pointer to the module data
  rr_log_data_t *rr_log_data_ptr = rr_log_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_log_data_ptr);

  /* only generate an event if there really is a change in service state */
  if ((in_service != rr_log_data_ptr->in_service_flag) || !rr_log_data_ptr->in_service_flag_valid)
  {
    if (in_service)
    {
      geran_diag_event(EVENT_GSM_RR_IN_SERVICE, EVENTS_DS_GSM_RR_IN_SERVICE, sizeof(payload), &payload);
    }
    else
    {
      geran_diag_event(EVENT_GSM_RR_OUT_OF_SERVICE, EVENTS_DS_GSM_RR_OUT_OF_SERVICE, sizeof(payload), &payload); 
    
#ifdef FEATURE_QSH_EVENT_METRIC
      rr_qsh_metric_log_oos(gas_id);
#endif // FEATURE_QSH_EVENT_METRIC
    }
  }

  rr_log_data_ptr->in_service_flag = in_service;
  rr_log_data_ptr->in_service_flag_valid = TRUE;
} /* rr_log_service_change */

/*!
 * \brief Logs a page received addressed to the mobile.
 *
 * \param gas_id
 */
void rr_log_page_received(
  const gas_id_t gas_id
)
{
  byte payload = geran_get_diag_sub_id(gas_id);

  geran_diag_event(EVENT_GSM_PAGE_RECEIVED, EVENTS_DS_GSM_PAGE_RECEIVED, sizeof(payload), &payload);  
  return;
} /* rr_log_page_received */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)

/*!
 * \brief Logs a page missed by the mobile.
 *
 * \param gas_id
 */
void rr_log_page_missed(
  const gas_id_t gas_id
)
{
  /* single byte payload is the subscription ID */
  byte payload = geran_get_diag_sub_id(gas_id);

  /* Page miss is applicable only on multi sim config */
  geran_diag_event(EVENT_MAX_ID, EVENTS_DS_GSM_PAGE_MISSED, sizeof(payload), &payload);  
  return;
} /* rr_log_page_missed */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM*/

/*!
 * \brief Outputs EVENT_GSM_CAMP_ATTEMPT_START & EVENT_GSM_CAMP_ATTEMPT_START_V2 events.
 *          EVENT_GSM_CAMP_ATTEMPT_START_V2 is the new event created to output ARFCN band separately in the payload.
 *
 * \param local_arfcn
 * \param bsic
 * \param gas_id
 */
void rr_log_camping_attempt_start(
  ARFCN_T local_arfcn,
  BSIC_T bsic,
  const gas_id_t gas_id
)
{
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    word arfcn;
    byte bsic;
  } payload_T;
  payload_T payload;

  typedef PACKED struct PACKED_POST
  {
    byte  sub_id;
    word  arfcn;
    uint8 band;
    byte  bsic;
  } payload_v2_T;
  payload_v2_T payload_v2;

  payload.sub_id         = geran_get_diag_sub_id(gas_id);
  payload.arfcn          = rr_arfcn_to_word(local_arfcn);
  payload.bsic           = (byte) (bsic.PLMN_colour_code<<3)+bsic.BS_colour_code;
  geran_diag_event(EVENT_GSM_CAMP_ATTEMPT_START, EVENTS_DS_GSM_CAMP_ATTEMPT_START, sizeof(payload), (byte*)&payload);

  payload_v2.sub_id         = geran_get_diag_sub_id(gas_id);
  payload_v2.arfcn          = local_arfcn.num;
  payload_v2.band           = GET_ARFCN_BAND(local_arfcn);
  payload_v2.bsic           = (byte) (bsic.PLMN_colour_code<<3)+bsic.BS_colour_code;
  geran_diag_event(EVENT_GSM_CAMP_ATTEMPT_START_V2, EVENTS_DS_GSM_CAMP_ATTEMPT_START_V2, sizeof(payload_v2), (byte*)&payload_v2);

  return;
} /* rr_log_camping_attempt_start */

/*!
 * \brief generic function for both success/failure case that outputs 
 * EVENT_GSM_CAMP_ATTEMPT_END/EVENTS_DS_GSM_CAMP_ATTEMPT_END event.
 *
 * \param gas_id
 */
void rr_log_camping_attempt_end(
  rr_log_camping_attempt_failure_T failure_reason,
  const gas_id_t gas_id
)
{
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    byte success;
    byte failure_reason;
  } payload_T;
  payload_T payload;

  payload.sub_id         = geran_get_diag_sub_id(gas_id);
  payload.success        = (0xFF == (byte)failure_reason) ? 1 : 0;
  payload.failure_reason = (byte) failure_reason;

  geran_diag_event(EVENT_GSM_CAMP_ATTEMPT_END, EVENTS_DS_GSM_CAMP_ATTEMPT_END, sizeof(payload), (byte*)&payload);

  return;
} /* rr_log_camping_attempt_end */

/*!
 * \brief Called when a camping attempt fails
 *
 * \param failure_reason (in) - why the camp attempt failed
 * \param gas_id
 */
void rr_log_camping_attempt_failure(
  rr_log_camping_attempt_failure_T failure_reason,
  const gas_id_t gas_id
)
{
  rr_log_camping_attempt_end(failure_reason, gas_id);
  return;
} /* rr_log_camping_attempt_failure */

/*!
 * \brief Outputs EVENT_GSM_CAMP_ATTEMPT_END/EVENTS_DS_GSM_CAMP_ATTEMPT_END event.
 *
 * \param gas_id
 */
void rr_log_camping_attempt_success(
  const gas_id_t gas_id
)
{
  rr_log_camping_attempt_end(0xFF, gas_id);
  return;
} /* rr_log_camping_attempt_success */

/*!
 * \brief Logs a call drop
 *
 * \param call_drop_reason (in)
 * \param access_reason (in)
 * \param gas_id (in)
 */
void rr_log_gsm_call_drop(
  rr_log_call_dropped_reason_T call_drop_reason,
  uint8 access_reason,
  const gas_id_t gas_id
)
{
  rr_log_call_type_T call_type = RR_LOG_VOICE_CALL;
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    byte call_drop_reason;
    byte call_type;
  } payload_T;
  payload_T payload;

  if (access_reason == (uint8) MM_RR_MO_FULL_SPEECH ||
      access_reason == (uint8) MM_RR_MO_HALF_SPEECH ||
      access_reason == (uint8) MM_RR_EMERGENCY_CALL ||
      access_reason == (uint8) MM_RR_CALL_REEST)
  {
    call_type = RR_LOG_VOICE_CALL;
  }
  else if (access_reason == (uint8) MM_RR_MO_DATA_9600 ||
           access_reason == (uint8) MM_RR_MO_DATA_4800 ||
           access_reason == (uint8) MM_RR_MO_DATA_2400)
  {
    call_type = RR_LOG_DATA_CALL;
  }
  else if (access_reason == (uint8) MM_RR_SHORT_MESSAGE)
  {
    call_type = RR_LOG_SMS_CALL;
  }
  else if (access_reason == (uint8) MM_RR_SUPPLEMENTARY_SERVICE ||
           access_reason == (uint8) MM_RR_LOCATION_UPDATE ||
           access_reason == (uint8) MM_RR_IMSI_DETACH ||
           access_reason == (uint8) MM_RR_ANSWER_TO_PAGING )
  {
    call_type = RR_LOG_SIGNALLING_CALL;
  }

  payload.sub_id           = geran_get_diag_sub_id(gas_id);
  payload.call_drop_reason = (byte) call_drop_reason;
  payload.call_type        = (byte) call_type;

  geran_diag_event(EVENT_GSM_CALL_DROP, EVENTS_DS_GSM_CALL_DROP, sizeof(payload), (byte*)&payload);

#ifdef FEATURE_QSH_EVENT_METRIC
  {
    sys_rlf_weak_signal_e_type cause = SYS_RLF_WEAK_SIGNAL_OTHER;

    if(RR_LOG_CALL_DROPPED_DUE_TO_RADIO_LINK_FAILURE == call_drop_reason)
    {
      cause = SYS_RLF_WEAK_SIGNAL_RLF_TIMER_EXP;
    }
    else if(RR_LOG_CALL_DROPPED_DUE_TO_L2_LINK_FAILURE == call_drop_reason)
    {
      cause = SYS_RLF_WEAK_SIGNAL_L2_FAILURE;
    }

    rr_qsh_metric_log_conn_end_info(
      SYS_CALL_END_EVENT_RLF_OR_WEAK_SIGNAL, 
      (uint8) cause, 
      gas_id
    );
  }
#endif /* FEATURE_QSH_EVENT_METRIC */

  return;
} /* rr_log_gsm_call_drop */

/*!
 * \brief Logs an access failure
 *
 * \param access_failure_reason
 * \param access_reason
 * \param no_of_rach_attemps
 * \param gas_id
 */
void rr_log_gsm_access_failure(
  rr_log_access_failure_reason_T access_failure_reason,
  uint8 access_reason,
  uint8 no_of_rach_attemps,
  const gas_id_t gas_id
)
{
  rr_log_call_type_T call_type = RR_LOG_VOICE_CALL;
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    byte access_failure_reason;
    byte call_type;
    byte no_of_rach_attemps;
  } payload_T;
  payload_T payload;

  if (!rr_multi_sim_api_valid(gas_id))
  {
    return;
  }

  if (access_reason == (uint8) MM_RR_MO_FULL_SPEECH ||
      access_reason == (uint8) MM_RR_MO_HALF_SPEECH ||
      access_reason == (uint8) MM_RR_EMERGENCY_CALL ||
      access_reason == (uint8) MM_RR_CALL_REEST)
  {
    call_type = RR_LOG_VOICE_CALL;
  }
  else if (access_reason == (uint8) MM_RR_MO_DATA_9600 ||
           access_reason == (uint8) MM_RR_MO_DATA_4800 ||
           access_reason == (uint8) MM_RR_MO_DATA_2400)
  {
    call_type = RR_LOG_DATA_CALL;
  }
  else if (access_reason == (uint8) MM_RR_SHORT_MESSAGE)
  {
    call_type = RR_LOG_SMS_CALL;
  }
  else if (access_reason == (uint8) MM_RR_SUPPLEMENTARY_SERVICE ||
           access_reason == (uint8) MM_RR_LOCATION_UPDATE ||
           access_reason == (uint8) MM_RR_IMSI_DETACH ||
           access_reason == (uint8) MM_RR_ANSWER_TO_PAGING )
  {
    call_type = RR_LOG_SIGNALLING_CALL;
  }

  payload.sub_id  = geran_get_diag_sub_id(gas_id);
  payload.access_failure_reason = (byte) access_failure_reason;
  payload.call_type = (byte) call_type;
  payload.no_of_rach_attemps = (byte) no_of_rach_attemps;

  geran_diag_event(EVENT_GSM_ACCESS_FAILURE, EVENTS_DS_GSM_ACCESS_FAILURE, sizeof(payload), (byte*)&payload);

#ifdef FEATURE_QSH_EVENT_METRIC
  {
    sys_rlf_weak_signal_e_type sys_cause = SYS_RLF_WEAK_SIGNAL_OTHER;

    if(RR_LOG_ACCESS_FAILURE_DUE_TO_RACH_FAILURE == access_failure_reason)
    {
      sys_cause = SYS_RLF_WEAK_SIGNAL_RANDOM_ACCESS_FAILURE;
    }
    else if(RR_LOG_ACCESS_FAILURE_DUE_L2_LINK_ESTABLISHMENT_FAILURE == access_failure_reason)
    {
      sys_cause = SYS_RLF_WEAK_SIGNAL_L2_FAILURE;
    }

    rr_qsh_metric_log_conn_end_info(
      SYS_CALL_END_EVENT_RLF_OR_WEAK_SIGNAL,
      sys_cause,
      gas_id
    );
      
  }
#endif

  return;
} /* rr_log_gsm_access_failure */

/*!
 * \brief Logs the start of cell selection
 *
 * \param start_reason
 * \param any_cell_selection_mode
 * \param automatic_PLMN_selection_criteria
 * \param gas_id
 */
void rr_log_cell_selection_start(
  rr_log_cell_selection_start_reason_T start_reason,
  boolean any_cell_selection_mode,
  boolean automatic_PLMN_selection_criteria,
  const gas_id_t gas_id
)
{
  byte payload[4];

  // Obtain a pointer to the module data
  rr_log_data_t *rr_log_data_ptr = rr_log_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_log_data_ptr);

  payload[0] = geran_get_diag_sub_id(gas_id);
  payload[1] = (byte) start_reason;
  payload[2] = (byte) any_cell_selection_mode;
  payload[3] = (byte) automatic_PLMN_selection_criteria;
  geran_diag_event(EVENT_GSM_CELL_SELECTION_START, EVENTS_DS_GSM_CELL_SELECTION_START, sizeof(payload), payload);

  if (rr_log_data_ptr->rr_log_selection_counter != 0)
  {
    MSG_GERAN_ERROR_0_G("Cell selection log counter mismatch");
  }
  rr_log_data_ptr->rr_log_selection_counter = 1;

  return;
} /* rr_log_cell_selection_start */

/*!
 * \brief Logs the end of cell selection
 *
 * \param end_reason
 * \param gas_id
 */
void rr_log_cell_selection_end(
  rr_log_cell_selection_end_reason_T end_reason,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_log_data_t *rr_log_data_ptr = rr_log_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_log_data_ptr);

  if (rr_log_data_ptr->rr_log_selection_counter == 1)
  {
    byte payload[2];

    if (rr_multi_sim_api_valid(gas_id))
    {
      payload[0] = geran_get_diag_sub_id(gas_id);
      payload[1] = (byte) end_reason;
      geran_diag_event(EVENT_GSM_CELL_SELECTION_END, EVENTS_DS_GSM_CELL_SELECTION_END, sizeof(payload), payload);
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Mismatched selection end event");
  }
  rr_log_data_ptr->rr_log_selection_counter = 0;

  return;
} /* rr_log_cell_selection_end */

/*!
 * \brief Indicates if there are no outstanding cell selection log events
 *
 * \param gas_id
 *
 * \return boolean - TRUE if selection logging counter is zero
 */
boolean rr_log_cell_selection_log_complete(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_log_data_t *rr_log_data_ptr = rr_log_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_log_data_ptr);

  return (rr_log_data_ptr->rr_log_selection_counter == 0);
} /* rr_log_cell_selection_log_complete */

/*!
 * \brief Outputs EVENT_GSM_POWER_SCAN_STATUS event.
 *
 * \param gas_id
 * \param status
 */
void rr_log_power_scan_status(
  rr_log_power_scan_status_T status, 
  const gas_id_t gas_id
)
{
  byte payload[2];

  payload[0] = geran_get_diag_sub_id(gas_id);
  payload[1] = (byte) status;
  geran_diag_event(EVENT_GSM_POWER_SCAN_STATUS, EVENTS_DS_GSM_POWER_SCAN_STATUS, sizeof(payload), payload);

  return;
} /* rr_log_power_scan_status */

/*!
 * \brief Outputs EVENT_GSM_POWER_SCAN_STATUS event.
 *
 * \param gas_id
 */
void rr_log_power_scan_started(
  const gas_id_t gas_id
)
{
  rr_log_power_scan_status(RR_LOG_PSCAN_STARTED, gas_id);
  return;
} /* rr_log_power_scan_started */

/*!
 * \brief Logs the completion of a power-scan
 *
 * \param gas_id
 */
void rr_log_power_scan_completed(
  const gas_id_t gas_id
)
{
  rr_log_power_scan_status(RR_LOG_PSCAN_COMPLETED, gas_id);
  return;
} /* rr_log_power_scan_completed */

/*!
 * \brief Logs the start of a reselection power-scan
 *
 * \param gas_id
 */
void rr_log_reselection_power_scan_started(
  const gas_id_t gas_id
)
{
  rr_log_power_scan_status(RR_LOG_RESELECTION_PSCAN_STARTED, gas_id);
  return;
} /* rr_log_power_scan_started */

/*!
 * \brief Logs the completion of a reselection power-scan
 *
 * \param gas_id
 */
void rr_log_reselection_power_scan_completed(
  const gas_id_t gas_id
)
{
  rr_log_power_scan_status(RR_LOG_RESELECTION_PSCAN_COMPLETED, gas_id);
  return;
} /* rr_log_power_scan_completed */

/*!
 * \brief Logs the start of PLMN list construction
 *
 * \param search_mode
 * \param gas_id
 */
void rr_log_plmn_list_started(
  rr_log_plmn_list_search_mode_T search_mode,
  const gas_id_t gas_id
)
{
  byte payload[2];

  payload[0] = geran_get_diag_sub_id(gas_id);
  payload[1] = (byte) search_mode;
  geran_diag_event(EVENT_GSM_PLMN_LIST_START, EVENTS_DS_GSM_PLMN_LIST_START, sizeof(payload), payload);

  return;
} /* rr_log_plmn_list_started */

/*!
 * \brief Logs the completion of PLMN list construction
 *
 * \param gas_id
 */
void rr_log_plmn_list_completed(
  const gas_id_t gas_id
)
{
  byte payload;
  
  payload = geran_get_diag_sub_id(gas_id);
  geran_diag_event(EVENT_GSM_PLMN_LIST_END, EVENTS_DS_GSM_PLMN_LIST_END, sizeof(payload), &payload);

  return;
} /* rr_log_plmn_list_completed */

/*!
 * \brief Logs the multi-rate parameters
 *
 * \param config_ptr
 * \param no_of_codec_modes
 * \param gas_id
 */
void rr_log_multirate_parameters(
  multirate_config_T *config_ptr,
  byte no_of_codec_modes,
  const gas_id_t gas_id
)
{
  byte i;
  typedef PACKED struct PACKED_POST
  {
    byte sub_id;
    boolean start_mode;
    boolean start_mode_valid;
    byte    codec_modes;
    byte    thresholds[3];
    byte    hysteresis[3];
  } multirate_log_parameters_T;

  multirate_log_parameters_T payload;

  payload.sub_id = geran_get_diag_sub_id(gas_id);
  payload.start_mode    = config_ptr->start_mode;
  payload.start_mode_valid  = config_ptr->start_mode_valid;
  payload.codec_modes   = config_ptr->codec_modes;

  for(i = 0; i < (no_of_codec_modes - 1); i++)
  {
    payload.thresholds[i] = config_ptr->thresholds[i];
    payload.hysteresis[i] = config_ptr->hysteresis[i];
  }

  geran_diag_event(EVENT_GSM_AMR_MULTIRATE_IE, EVENTS_DS_GSM_AMR_MULTIRATE_IE, sizeof(payload), (byte*)&payload);

  return;
} /* rr_log_multirate_parameters */

/*!
 * \brief Outputs EVENT_GPRS_PAGE_RECEIVED/EVENTS_DS_GPRS_PAGE_RECEIVED event.
 *
 * \param gas_id
 */
void rr_log_gprs_page_received(
  const gas_id_t gas_id
)
{
  byte payload = geran_get_diag_sub_id(gas_id);
  
  geran_diag_event(EVENT_GPRS_PAGE_RECEIVED, EVENTS_DS_GPRS_PAGE_RECEIVED, sizeof(payload), &payload);
  return;
} /* rr_log_gprs_page_received */

/*!
 * \brief Outputs EVENT_GPRS_SURROUND_SEARCH_START event.
 *
 * \param bcch_arfcn
 * \param pbcch_arfcn
 * \param gas_id
 */
void rr_log_gprs_surround_search_start(
  ARFCN_T bcch_arfcn,
  ARFCN_T pbcch_arfcn,
  const gas_id_t gas_id
)
{
  PACKED struct PACKED_POST
  {
    byte sub_id;
    word bcch_arfcn;
    word pbcch_arfcn;
  } payload;

  payload.sub_id = geran_get_diag_sub_id(gas_id);
  payload.bcch_arfcn   = rr_arfcn_to_word(bcch_arfcn);
  payload.pbcch_arfcn  = rr_arfcn_to_word(pbcch_arfcn);

  geran_diag_event(EVENT_GPRS_SURROUND_SEARCH_START, EVENTS_DS_GPRS_SURROUND_SEARCH_START, sizeof(payload), (byte*)&payload);

  return;
} /* rr_log_gprs_surround_search_start */

/*!
 * \brief Outputs EVENT_GPRS_SURROUND_SEARCH_END for either a success/failure case.
 *
 * \param failure_reason - for success case 0xFF
 * \param gas_id
 */
void rr_log_gprs_surround_search_end(
  rr_log_gprs_surround_search_failure_T failure_reason,
  const gas_id_t gas_id
)
{
  byte payload[3];

  payload[0] = geran_get_diag_sub_id(gas_id);
  payload[1] = (0xFF == (byte)failure_reason)? 0 : 1;  /* SUCCESS = 1, FAILURE = 0 */
  payload[2] = (byte) failure_reason;

  geran_diag_event(EVENT_GPRS_SURROUND_SEARCH_END, EVENTS_DS_GPRS_SURROUND_SEARCH_END, sizeof(payload), (byte*)&payload);
  
  return;
} /* rr_log_gprs_surround_search_end */

/*!
 * \brief Outputs EVENT_GPRS_SURROUND_SEARCH_END with a failure reason.
 *
 * \param failure_reason
 * \param gas_id
 */
void rr_log_gprs_surround_search_failure(
  rr_log_gprs_surround_search_failure_T failure_reason,
  const gas_id_t gas_id
)
{
  rr_log_gprs_surround_search_end(failure_reason, gas_id);
  return;
} /* rr_log_gprs_surround_search_failure */

/*!
 * \brief Outputs EVENT_GPRS_SURROUND_SEARCH_END event indicating success.
 *
 * \param gas_id
 */
void rr_log_gprs_surround_search_success(
  const gas_id_t gas_id
)
{
  rr_log_gprs_surround_search_end(0xFF, gas_id);
  return;
} /* rr_log_gprs_surround_search_success */

/*!
 * \brief Outputs EVENT_GPRS_EARLY_CAMPING event.
 *
 * \param bcch_arfcn
 * \param bsic
 * \param gas_id
 */
void rr_log_gprs_early_camping(
  ARFCN_T bcch_arfcn,
  BSIC_T bsic,
  const gas_id_t gas_id
)
{
  PACKED struct PACKED_POST
  {
    byte sub_id;
    word bcch_arfcn;
    word pbcch_arfcn;
    byte bsic;
  } payload;

  payload.sub_id     = geran_get_diag_sub_id(gas_id);
  payload.bcch_arfcn = rr_arfcn_to_word(bcch_arfcn); ;
  payload.bsic       = (byte)CONVERT_BSIC_TO_BYTE(bsic);

  payload.pbcch_arfcn = INVALID_LOG_ARFCN;  // DMB what is this for ?

  geran_diag_event(EVENT_GPRS_EARLY_CAMPING, EVENTS_DS_GPRS_EARLY_CAMPING, sizeof(payload), (byte*)&payload);

  return;
}

/*!
 * \brief Outputs event EVENT_GERAN_GRR_PLMN_LIST_RAT_SEARCH_STARTED.
 *
 * \param rat
 * \param gas_id
 */
void rr_log_plmn_list_rat_search_started(
  rr_log_rat_e rat,
  const gas_id_t gas_id
)
{
  byte payload[2];

  payload[0] = geran_get_diag_sub_id(gas_id);
  payload[1] = (byte) rat;
  geran_diag_event(EVENT_GERAN_GRR_PLMN_LIST_RAT_SEARCH_STARTED, EVENTS_DS_GERAN_GRR_PLMN_LIST_RAT_SEARCH_STARTED, sizeof(payload), payload);

  return;
}

/*!
 * \brief Outputs event EVENT_GERAN_GRR_PLMN_LIST_RAT_SEARCH_COMPLETED.
 *
 * \param gas_id
 */
void rr_log_plmn_list_rat_search_completed(
  const gas_id_t gas_id
)
{
  byte payload = geran_get_diag_sub_id(gas_id);

  geran_diag_event(EVENT_GERAN_GRR_PLMN_LIST_RAT_SEARCH_COMPLETED, EVENTS_DS_GERAN_GRR_PLMN_LIST_RAT_SEARCH_COMPLETED, sizeof(payload), &payload);
  return;
}

/*!
 * \brief Outputs event EVENT_GERAN_GRR_PLMN_LIST_RAT_SEARCH_ABORTED.
 *
 * \param gas_id
 */
void rr_log_plmn_list_rat_search_aborted(
  const gas_id_t gas_id
)
{
  byte payload = geran_get_diag_sub_id(gas_id);

  geran_diag_event(EVENT_GERAN_GRR_PLMN_LIST_RAT_SEARCH_ABORTED, EVENTS_DS_GERAN_GRR_PLMN_LIST_RAT_SEARCH_ABORTED, sizeof(payload), &payload);
  return;
}

/*!
 * \brief Outputs event EVENT_GERAN_GRR_REDIR_STARTED.
 *
 * \param rat
 * \param gas_id
 */
void rr_log_redir_started(
  rr_log_rat_e rat,
  const gas_id_t gas_id
)
{
  byte payload[2];

  payload[0] = geran_get_diag_sub_id(gas_id);
  payload[1] = (uint8) rat;
  geran_diag_event(EVENT_GERAN_GRR_REDIR_STARTED, EVENTS_DS_GERAN_GRR_REDIR_STARTED, sizeof(payload), payload);

  return;
}

/*!
 * \brief Outputs event EVENT_GERAN_GRR_REDIR_COMPLETED.
 *
 * \param gas_id
 */
void rr_log_redir_completed(
  const gas_id_t gas_id
)
{
  byte payload = geran_get_diag_sub_id(gas_id);

  geran_diag_event(EVENT_GERAN_GRR_REDIR_COMPLETED, EVENTS_DS_GERAN_GRR_REDIR_COMPLETED, sizeof(payload), &payload);
  return;
}

/*!
 * \brief Outputs event EVENT_GERAN_GRR_REDIR_ABORTED.
 *
 * \param gas_id
 */
void rr_log_redir_aborted(
  const gas_id_t gas_id
)
{
  byte payload = geran_get_diag_sub_id(gas_id);

  geran_diag_event(EVENT_GERAN_GRR_REDIR_ABORTED, EVENTS_DS_GERAN_GRR_REDIR_ABORTED, sizeof(payload), &payload);
  return;
}

#ifdef FEATURE_WCDMA
/*!
 * \brief Logs WCDMA measurements.
 *
 * \param wcdma_measurements
 * \param gas_id
 */
static void rr_log_wcdma_measurements(
  const rr_l1_wcdma_measurements_t *wcdma_measurements,
  const gas_id_t gas_id
)
{
  gsmdiag_pkt_info_t           pkt_info;
  rr_log_irat_measurements_t * log_ptr;

  RR_NULL_CHECK_RETURN_VOID(wcdma_measurements);

  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_IRAT_MEASUREMENTS_C,
    sizeof(rr_log_irat_measurements_t),
    gas_id
  );

  log_ptr = (rr_log_irat_measurements_t *) gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    uint8 i;

    log_ptr->version = 2;
    log_ptr->rat = RR_LOG_IRAT_RAT_WCDMA;
    if( (MAC_IDLE == rr_macinfo_get_mac_status(gas_id)) || (MAC_IDLE_BLOCKED == rr_macinfo_get_mac_status(gas_id)) )
    {
      log_ptr->mac_status = MAC_IDLE;
    }
    else
    {
      log_ptr->mac_status = MAC_TRANSFER;
    }
    log_ptr->measurements.wcdma.uarfcn = wcdma_measurements->uarfcn;

    for (i = 0;
         i < wcdma_measurements->no_of_entries &&
         i < MAX_WCDMA_MEASUREMENTS_PER_FREQUENCY &&
         i < RR_LOG_MAX_WCDMA_MEASUREMENTS_PER_FREQUENCY;
         ++i)
    {
      log_ptr->measurements.wcdma.entries[i].scrambling_code = wcdma_measurements->entries[i].scrambling_code;
      log_ptr->measurements.wcdma.entries[i].diversity = wcdma_measurements->entries[i].diversity;
      log_ptr->measurements.wcdma.entries[i].rscp = wcdma_measurements->entries[i].rscp;
      log_ptr->measurements.wcdma.entries[i].ecnox2 = wcdma_measurements->entries[i].ecno;
      log_ptr->measurements.wcdma.entries[i].rssi = wcdma_measurements->entries[i].rssi;
      log_ptr->measurements.wcdma.entries[i].pn_offset = wcdma_measurements->entries[i].pn_offset;
    }

    log_ptr->measurements.wcdma.no_of_entries = i;

    gsmdiag_log_commit(&pkt_info);
  }
}
#endif /*FEATURE_WCDMA */

#ifdef FEATURE_LTE
/*!
 * \brief Logs LTE measurements.
 *
 * \param lte_measurements
 * \param gas_id
 */
static void rr_log_lte_measurements(
  const rr_l1_lte_measurements_t *lte_measurements,
  const gas_id_t gas_id
)
{
  gsmdiag_pkt_info_t           pkt_info;
  rr_log_irat_measurements_t * log_ptr;

  RR_NULL_CHECK_RETURN_VOID(lte_measurements);

  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_IRAT_MEASUREMENTS_C,
    sizeof(rr_log_irat_measurements_t),
    gas_id
  );

  log_ptr = (rr_log_irat_measurements_t *) gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    uint8 i;

    log_ptr->version = 2;
    log_ptr->rat = RR_LOG_IRAT_RAT_LTE;
    if( (MAC_IDLE == rr_macinfo_get_mac_status(gas_id)) || (MAC_IDLE_BLOCKED == rr_macinfo_get_mac_status(gas_id)) )
    {
      log_ptr->mac_status = MAC_IDLE;
    }
    else
    {
      log_ptr->mac_status = MAC_TRANSFER;
    }
    log_ptr->measurements.lte.earfcn = lte_measurements->earfcn;

    for (i = 0;
         i < lte_measurements->no_of_entries &&
         i < MAX_LTE_MEASUREMENTS_PER_FREQUENCY &&
         i < RR_LOG_MAX_LTE_MEASUREMENTS_PER_FREQUENCY;
         ++i)
    {
      log_ptr->measurements.lte.entries[i].pcid = lte_measurements->entries[i].pcid;
      log_ptr->measurements.lte.entries[i].rsrp = lte_measurements->entries[i].rsrp;
      log_ptr->measurements.lte.entries[i].rsrq = lte_measurements->entries[i].rsrq;
    }

    log_ptr->measurements.lte.no_of_entries = i;

    gsmdiag_log_commit(&pkt_info);
  }
}
#endif /* FEATURE_LTE */

#if defined (FEATURE_WCDMA) || defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
/*!
 * \brief Logs IRAT measurements.
 *
 * \param irat_measurements
 * \param gas_id
 */
void rr_log_irat_measurements(
  const mph_surround_irat_meas_ind_t *irat_measurements,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(irat_measurements);

  switch (irat_measurements->rat)
  {
#ifdef FEATURE_WCDMA
    case RR_L1_IRAT_RAT_WCDMA:
    {
      rr_log_wcdma_measurements(&irat_measurements->measurements.wcdma, gas_id);
      break;
    } /* RR_IRAT_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
    case RR_L1_IRAT_RAT_LTE:
    {
      rr_log_lte_measurements(&irat_measurements->measurements.lte, gas_id);
      break;
    } /* RR_IRAT_RAT_LTE */
#endif /* FEATURE_LTE */

    default:
    {

    }
  }
}
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

/*!
 * \brief Logs SI Cache flushes.
 *
 * \param 
 * \param gas_id
 */
void rr_log_csi_flush_data(csi_entry_t *entry_ptr,
  const rr_si_cache_flush_cause_T csi_flush_cause,
  const gas_id_t gas_id)
{

  typedef PACKED struct PACKED_POST
  {
    uint16 ARFCN;
    uint32 MCC;
    uint32 MNC;
    uint16 LAC;
    uint16 Cell_Id;
    rr_si_cache_flush_cause_T Flush_cause;
  } rr_log_csi_flush_data_T;

  rr_log_csi_flush_data_T payload;
  boolean plmn_is_undefined;
  boolean mnc_includes_pcs_digit;
  sys_mcc_type mcc = 0;
  sys_mnc_type mnc = 0;

  RR_NULL_CHECK_RETURN_VOID(entry_ptr);

  /* Get the MCC & MNC from PLMN_Id */
  sys_plmn_get_mcc_mnc(entry_ptr->cgi.lai.plmn_id,
                         &plmn_is_undefined,
                         &mnc_includes_pcs_digit,
                         &mcc,
                         &mnc);

  payload.ARFCN = entry_ptr->si_arfcn.num;
  payload.MCC = mcc;
  payload.MNC = mnc;
  payload.LAC = entry_ptr->cgi.lai.location_area_code;
  payload.Cell_Id = entry_ptr->cgi.ci;
  payload.Flush_cause = csi_flush_cause;

  event_report_payload(
    EVENT_GSM_SI_CACHE_FLUSH,
    sizeof(payload),
    (void*) &payload
  );

  return;
}

/*!
 * \brief Logs SI Cache entry updates.
 *
 * \param 
 * \param gas_id
 */
void rr_log_csi_update_entry(csi_entry_t *entry_ptr,
  const rr_si_cache_update_status_T status,
  const gas_id_t gas_id)
{
  typedef PACKED struct PACKED_POST
  {
    uint8    si_type;
    uint8    si_index;
    uint16   ARFCN;
    uint32   MCC;
    uint32   MNC;
    uint16   LAC;
    uint16   Cell_Id;
    rr_si_cache_update_status_T Status;
    uint8    Current_cached_SI_Length;
    uint8    Current_cached_SI[MAX_CSI_PER_CGI];
    uint16   Current_cached_SI2quater;
    uint16   Current_cached_SI2ter;
  } rr_log_csi_update_entry_T;


  rr_log_csi_update_entry_T payload;

  boolean plmn_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint8 si_index;
  sys_mcc_type       mcc;
  sys_mnc_type       mnc;
  uint8 cached_si[MAX_CSI_PER_CGI];
  uint8 si_length = 0;
  uint16 si2q_bitmap = 0;
  uint16 si2ter_bitmap = 0;
  byte i;

  RR_NULL_CHECK_RETURN_VOID(entry_ptr);

  for(i = 0;i < MAX_CSI_PER_CGI;i++)
  {
    cached_si[i] = 0xFF;
  }

  /* Get the MCC & MNC from PLMN_Id */
  (void)sys_plmn_get_mcc_mnc(entry_ptr->cgi.lai.plmn_id,
                       &plmn_is_undefined,
                       &mnc_includes_pcs_digit,
                       &mcc,
                       &mnc);

  (void)rr_csi_get_si_index(entry_ptr,&si_index);

  rr_csi_get_si_cache_summary(entry_ptr,
    &si_length,
     cached_si,
    &si2q_bitmap,
    &si2ter_bitmap);

  payload.si_type = entry_ptr->msg_type;
  payload.si_index = si_index;
  payload.ARFCN = entry_ptr->si_arfcn.num;
  payload.MCC = mcc;
  payload.MNC = mnc;
  payload.LAC = entry_ptr->cgi.lai.location_area_code;
  payload.Cell_Id = entry_ptr->cgi.ci;
  payload.Status = status;
  payload.Current_cached_SI_Length = si_length;
  for(i = 0;i < MAX_CSI_PER_CGI;i++)
  {
    payload.Current_cached_SI[i] = cached_si[i];
  }
  payload.Current_cached_SI2quater = si2q_bitmap;
  payload.Current_cached_SI2ter = si2ter_bitmap;

  event_report_payload(
    EVENT_GSM_SI_CACHE_UPDATE_ENTRY,
    sizeof(payload),
    (void*) &payload
  );

  return;
}

/*!
 * \brief Logs SI Cache retrieve entry.
 *
 * \param 
 * \param gas_id
 */
void rr_log_csi_retrieve_entry(csi_entry_t *entry_ptr,
  const gas_id_t gas_id
)
{
  typedef PACK(struct)
  {
    uint16        ARFCN;
    uint32        MCC;
    uint32        MNC;
    uint16        LAC;
    uint16        Cell_Id;
    uint8         length_of_SIs_retrieved;
    uint8         SIs_retrieved[MAX_CSI_PER_CGI];
    uint16        SI_2quater_indices;
    uint16        SI_2ter_indices;
  } rr_log_csi_retrieve_entry_T;

  rr_log_csi_retrieve_entry_T payload;

  boolean plmn_is_undefined;
  boolean mnc_includes_pcs_digit;
  sys_mcc_type       mcc;
  sys_mnc_type       mnc;
  uint8 retrieved_si[MAX_CSI_PER_CGI];
  uint8 si_length = 0;
  uint16 si2q_bitmap = 0;
  uint16 si2ter_bitmap =0;
  byte i;  

  RR_NULL_CHECK_RETURN_VOID(entry_ptr);

  for(i = 0;i < MAX_CSI_PER_CGI;i++)
  {
    retrieved_si[i] = 0xFF;
  }

  /* Get the MCC & MNC from PLMN_Id */
  sys_plmn_get_mcc_mnc(entry_ptr->cgi.lai.plmn_id,
                       &plmn_is_undefined,
                       &mnc_includes_pcs_digit,
                       &mcc,
                       &mnc);

  rr_csi_get_si_cache_summary(entry_ptr,
    &si_length,
     retrieved_si,
    &si2q_bitmap,
    &si2ter_bitmap);

  payload.ARFCN = entry_ptr->si_arfcn.num;
  payload.MCC = mcc;
  payload.MNC = mnc;
  payload.LAC = entry_ptr->cgi.lai.location_area_code;
  payload.Cell_Id = entry_ptr->cgi.ci;
  payload.length_of_SIs_retrieved = si_length;
  for(i = 0;i < MAX_CSI_PER_CGI;i++)
  {
    payload.SIs_retrieved[i] = retrieved_si[i];
  }
  payload.SI_2quater_indices = si2q_bitmap;
  payload.SI_2ter_indices = si2ter_bitmap;

  event_report_payload(
    EVENT_GSM_SI_CACHE_RETRIEVE,
    sizeof(payload),
    (void*) &payload
  );

  return;
}

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 *
 *  Note: This intialises all data-spaces.
 */
void rr_log_task_start_init(
  void
)
{
  // Zero the control/data structure(s)
  memset(rr_log_data, 0, sizeof(rr_log_data));

  // Setup any specific values not setup by the general module initialisation function
}

#ifdef FEATURE_FAKE_BTS_CELL_INFO
void rr_log_fake_cell_data( geran_grr_qmi_cell_info_ind_t *entry_ptr, const gas_id_t gas_id )
{
  rr_log_fake_cell_data_t *log_ptr; // pointer to log buffer
  gsmdiag_pkt_info_t       pkt_info;
  uint16                   log_size;  
  byte                     count;
  byte                     index =0;

  if(entry_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("Invalid parameters passed to entry_ptr");
    return;
  }
  count = entry_ptr->si1_ca_length;
  if (count > LOG_MAX_CA_LEN)
  {
    count = LOG_MAX_CA_LEN;
  }
  log_size = sizeof(rr_log_fake_cell_data_t) - ((LOG_MAX_CA_LEN - count)*sizeof(uint16));

  rr_log_packet_init(
    &pkt_info,
    LOG_GSM_RR_FAKE_CELL_DATA_C,
    log_size,
    gas_id
  );
  
  log_ptr = (rr_log_fake_cell_data_t*)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    log_ptr->version = 1;
    log_ptr->arfcn = entry_ptr->uid_camped_cell.arfcn;
    log_ptr->bsic = entry_ptr->uid_camped_cell.bsic;
    log_ptr->plmn_id[0] = entry_ptr->uid_camped_cell.plmn_id.identity[0];
    log_ptr->plmn_id[1] = entry_ptr->uid_camped_cell.plmn_id.identity[1];
    log_ptr->plmn_id[2] = entry_ptr->uid_camped_cell.plmn_id.identity[2];
    log_ptr->cell_id = entry_ptr->uid_camped_cell.cell_id;
    log_ptr->location_area_code = (uint16)entry_ptr->uid_camped_cell.location_area_code;
    log_ptr->si_mask_valid = entry_ptr->si_mask_valid;
    log_ptr->broadcast_si_mask = entry_ptr->broadcast_si_mask;
    log_ptr->ra_color =  entry_ptr->ra_color;
    log_ptr->si13_position = entry_ptr->si13_position;
    log_ptr->reselection_offset =  entry_ptr->reselection_offset;	
    log_ptr->t3212 = entry_ptr->t3212;
    log_ptr->rx_lev_min = entry_ptr->rx_lev_min;
    log_ptr->MSCR = entry_ptr->MSCR;
    log_ptr->si2_received = entry_ptr->mandatory_si_status;
    log_ptr->si_ca_length = entry_ptr->si1_ca_length;
    for (index = 0; index < count; index++)
    {
      log_ptr->si1_cell_allocation[index]= entry_ptr->si1_cell_allocation[index];
    }

    gsmdiag_log_commit(&pkt_info);
  }
  
}

#endif /*FEATURE_FAKE_BTS_CELL_INFO*/


/* EOF */
