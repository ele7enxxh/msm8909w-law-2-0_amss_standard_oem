/*!
 * \file rr_qsh_metric.c
 *
 * This module contains functionality to interface to QSH.
 *
 *              Copyright (c) 2015 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_qsh_metric.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"

#ifdef FEATURE_QSH_EVENT_METRIC

#include "qsh.h"
#include "rr_qsh_metric.h"
#include "geran_multi_sim.h"
#include <stringl.h>
#include "rr_general.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  void *data_ptr;
  uint8 size_bytes;
  uint16 sampling_rate;
  uint8 num_of_elements;
} rr_metric_def_s;

typedef struct
{
  qsh_client_metric_context_id_t *context_id;
  uint8 *buffer_ptr;
} rr_metric_qsh_info_s;

typedef struct
{
  grr_qsh_metric_resel_s resel[NUM_GERAN_DATA_SPACES];
  grr_qsh_metric_handover_s handover[NUM_GERAN_DATA_SPACES];
  grr_qsh_metric_rach_s rach[NUM_GERAN_DATA_SPACES];
  grr_qsh_metric_csfb_s csfb[NUM_GERAN_DATA_SPACES];
  grr_qsh_metric_service_s service[NUM_GERAN_DATA_SPACES];
} rr_stored_metric_data_s;

typedef struct
{
  rr_stored_metric_data_s stored_data;
  rr_metric_qsh_info_s qsh_info[NUM_GERAN_DATA_SPACES][GRR_QSH_METRIC_COUNT];
} rr_metric_data_s;

/*----------------------------------------------------------------------------
 * Variable Definitions
 * -------------------------------------------------------------------------*/

// Array of metric config structures (should be kept global)
qsh_ext_metric_cfg_s rr_qsh_metric_cfg_arr[NUM_GERAN_DATA_SPACES][GRR_QSH_METRIC_COUNT];

static rr_metric_data_s rr_metric_data;

/*----------------------------------------------------------------------------
 * Constant Variable Definitions
 * -------------------------------------------------------------------------*/

const rr_metric_def_s rr_metric_def[] =
{
  // GRR_QSH_METRIC_MSG
  {
    NULL,
    sizeof(grr_qsh_metric_msg_s),
    0,
    50
  },
  // GRR_QSH_METRIC_IMSG
  {
    NULL,
    sizeof(grr_qsh_metric_imsg_s),
    0,
    50
  },
  // GRR_QSH_METRIC_OTA_MSG
  {
    NULL,
    sizeof(grr_qsh_metric_ota_msg_s),
    0,
    50
  },
  // GRR_QSH_METRIC_EVENT
  {
    NULL,
    sizeof(grr_qsh_metric_event_s),
    0,
    50
  },
  // GRR_QSH_METRIC_RESEL
  {
    &rr_metric_data.stored_data.resel[0],
    sizeof(grr_qsh_metric_resel_s),
    1000,
    10
  },
  // GRR_QSH_METRIC_HANDOVER
  {
    &rr_metric_data.stored_data.handover[0],
    sizeof(grr_qsh_metric_handover_s),
    1000,
    10
  },
  // GRR_QSH_METRIC_RACH
  {
    &rr_metric_data.stored_data.rach[0],
    sizeof(grr_qsh_metric_rach_s),
    1000,
    10
  },
  // GRR_QSH_METRIC_CSFB
  {
    &rr_metric_data.stored_data.csfb[0],
    sizeof(grr_qsh_metric_csfb_s),
    1000,
    10
  },
  // GRR_QSH_METRIC_SERVICE
  {
    &rr_metric_data.stored_data.service[0],
    sizeof(grr_qsh_metric_service_s),
    1000,
    10
  },
  // GRR_QSH_METRIC_CELL_INFO
  {
    NULL,
    sizeof(grr_qsh_metric_cell_info_s),
    0,
    10
  }
  ,
  // GRR_QSH_METRIC_CONN_END_INFO
  {
    NULL,
    sizeof(grr_qsh_metric_conn_end_info_s),
    0,
    10
  } 
};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns the QSH buffer for a given metric.
 *
 * \param metric_id
 * \param buffer_ptr
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void update_qsh_metric_buffer(
  grr_qsh_metric_e metric_id,
  uint8 *buffer_ptr,
  const gas_id_t gas_id
)
{
  uint8 metric_index = (uint8)metric_id;
  uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  // Incorrect Metric ID ?
  RR_ASSERT_FATAL(metric_index < GRR_QSH_METRIC_COUNT);

  rr_metric_data.qsh_info[sub_index][metric_index].buffer_ptr = buffer_ptr;
}

/*!
 * \brief Returns the QSH buffer for a given metric.
 *
 * \param metric_id
 * \param gas_id
 *
 * \return uint8*
 */
__attribute__((section(".uncompressible.text")))
uint8 *get_qsh_metric_buffer(
  grr_qsh_metric_e metric_id,
  const gas_id_t gas_id
)
{
  uint8 metric_index = (uint8)metric_id;
  uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  // Incorrect Metric ID ?
  RR_ASSERT_FATAL(metric_index < GRR_QSH_METRIC_COUNT);

  return rr_metric_data.qsh_info[sub_index][metric_index].buffer_ptr;
}

/*!
 * \brief Returns the QSH metric context id for a given metric.
 *
 * \param metric_id
 * \param gas_id
 *
 * \return uint8*
 */
__attribute__((section(".uncompressible.text")))
qsh_client_metric_context_id_t get_qsh_metric_context_id(
  grr_qsh_metric_e metric_id,
  const gas_id_t gas_id
)
{
  uint8 metric_index = (uint8)metric_id;
  uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  // Incorrect Metric ID ?
  RR_ASSERT_FATAL(metric_index < GRR_QSH_METRIC_COUNT);

  return rr_metric_data.qsh_info[sub_index][metric_index].context_id;
}

/*!
 * \brief Writes the metric data for a given metric to the QSH buffer for that metric.
 *
 * \param metric_id
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void write_stored_metric_data_to_qsh_buffer(
  grr_qsh_metric_e metric_id,
  gas_id_t gas_id
)
{
  uint8 metric_index = (int)metric_id;

  if (rr_metric_def[metric_id].data_ptr != NULL)
  {
    // Get the QSH buffer pointer for this metric
    uint8 *qsh_buffer_ptr = get_qsh_metric_buffer(metric_id, gas_id);

    uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    // Fine the definition of this metric
    const rr_metric_def_s *metric_def_ptr = &rr_metric_def[metric_index];

    // Find the location of the metric data for the first sub
    uint8 *metric_data_ptr = (uint8 *)metric_def_ptr->data_ptr;

    // Now offset the pointer to point to the correct sub data
    metric_data_ptr += (sub_index * metric_def_ptr->size_bytes);

    // If the QSH buffer pointer is NULL, then this metric has either not been configured with a start action
    // or has subsequently been configured with a stop action
    if (qsh_buffer_ptr != NULL)
    {
      // Copy the stored metric data into the QSH buffer
      memscpy(
        qsh_buffer_ptr,
        metric_def_ptr->size_bytes,
        metric_data_ptr,
        metric_def_ptr->size_bytes
      );

      // Clear the stored metric data
      memset(metric_data_ptr, 0, metric_def_ptr->size_bytes);
    }
    else
    {
      MSG_GERAN_ERROR_0_G("QSH: metric is not configured (no buffer)");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("QSH: metric has no associated stored data");
  }

  return;
}

/*!
 * \brief Updates QSH that a metric has been written and updates the buffer associated with that metric.
 *
 * \param metric_id
 * \param gas_id
 * \param log_reason
 *
 * \return uint8*
 */
__attribute__((section(".uncompressible.text")))
void perform_metric_log_done(
  grr_qsh_metric_e metric_id,
  const gas_id_t gas_id,
  qsh_client_metric_log_reason_e log_reason
)
{
  uint8 *qsh_buffer_ptr;
  qsh_client_metric_log_done_s metric_log_done;

  qsh_client_metric_log_done_init(&metric_log_done);

  metric_log_done.client = QSH_CLT_GRR;
  metric_log_done.metric_id = (qsh_metric_id_t)metric_id;
  metric_log_done.metric_context_id = get_qsh_metric_context_id(metric_id, gas_id);
  metric_log_done.log_reason = log_reason;

  qsh_buffer_ptr = qsh_client_metric_log_done(&metric_log_done);

  update_qsh_metric_buffer(metric_id, qsh_buffer_ptr, gas_id);
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns the definition of a metric
 *
 * \param metric_id (in)
 * \param size_bytes_ptr (out)
 * \param sampling_rate_ptr (out)
 * \param num_of_elements_ptr (out)
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_get_def(
  grr_qsh_metric_e metric_id,
  uint8 *size_bytes_ptr,
  uint16 *sampling_rate_ptr,
  uint8 *num_of_elements_ptr
)
{
  uint8 metric_index = (uint8)metric_id;

  // Incorrect Metric ID ?
  RR_ASSERT_FATAL(metric_index < GRR_QSH_METRIC_COUNT);

  if (size_bytes_ptr != NULL)
  {
    *size_bytes_ptr = rr_metric_def[metric_index].size_bytes;
  }

  if (sampling_rate_ptr != NULL)
  {
    *sampling_rate_ptr = rr_metric_def[metric_index].sampling_rate;
  }

  if (num_of_elements_ptr != NULL)
  {
    *num_of_elements_ptr = rr_metric_def[metric_index].num_of_elements;
  }

  return;
}

/*!
 * \brief Performs the METRIC CONFIG call-back action
 *
 * \param cb_params_ptr (in)
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_config_perform_cb_action(qsh_client_cb_params_s *cb_params_ptr)
{
  // Short-cut to metric config parameters
  qsh_client_metric_cfg_s *metric_config_params_ptr = &cb_params_ptr->action_params.metric_cfg;

  // GRR metric id
  grr_qsh_metric_e metric_id = (grr_qsh_metric_e)metric_config_params_ptr->id;

  // Metric id as a look-up index
  uint8 metric_index = (uint8)metric_id;

  // Convert SUB-ID id into GAS-ID
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(metric_config_params_ptr->subs_id);

  // SUB-ID as a look-up index
  uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  // Incorrect Metric ID ?
  RR_ASSERT_FATAL(metric_index < GRR_QSH_METRIC_COUNT);

  // Check if the metric is being started or stopped
  if (metric_config_params_ptr->action == QSH_METRIC_ACTION_START)
  {
    MSG_GERAN_HIGH_1_G("QSH: Configure metric %d (START)",(int)metric_id);

    // Enable logging of this metric
    rr_metric_data.qsh_info[sub_index][metric_index].context_id = metric_config_params_ptr->metric_context_id;
    rr_metric_data.qsh_info[sub_index][metric_index].buffer_ptr = metric_config_params_ptr->start_addr;
  }
  else
  if (metric_config_params_ptr->action == QSH_METRIC_ACTION_STOP)
  {
    MSG_GERAN_HIGH_1_G("QSH: Configure metric %d (STOP)",(int)metric_id);

    // Disable logging of this metric
    rr_metric_data.qsh_info[sub_index][metric_index].context_id = NULL;
    rr_metric_data.qsh_info[sub_index][metric_index].buffer_ptr = NULL;
  }

  // Indicate call-back action done
  {
    qsh_client_action_done_s action_done;

    qsh_client_action_done_init(&action_done);

    action_done.cb_params_ptr = cb_params_ptr;
    action_done.action_mode_done = QSH_ACTION_MODE_DONE_ASYNC;

    qsh_client_action_done(&action_done);
  }

  /* Log cell info metric as soon as it is enabled if we were already camped */
  if ((metric_config_params_ptr->action == QSH_METRIC_ACTION_START)&&
      (GRR_QSH_METRIC_CELL_INFO == metric_id) &&
      rr_is_camped(gas_id))
  {
    rr_qsh_metric_log_cell_info(gas_id);
  }

  return;
}

/*!
 * \brief Performs the METRIC TIMER EXPIRY call-back action
 *
 * \param cb_params_ptr (in)
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_timer_expiry_perform_cb_action(qsh_client_cb_params_s *cb_params_ptr)
{
  // Short-cut to metric timer expiry parameters
  qsh_client_metric_timer_expiry_s *params_ptr = &cb_params_ptr->action_params.metric_timer_expiry;

  uint8 num_metrics = MIN((uint8)params_ptr->metric_id_count, QSH_CLIENT_METRIC_TIMER_EXPIRY_ARR_MAX);

  if (num_metrics > 0)
  {
    uint16 metric_count;

    // Loop through the array of metric IDs
    for (metric_count = 0; metric_count < num_metrics; metric_count++)
    {
      qsh_metric_id_t qsh_metric_id = params_ptr->params[metric_count].metric_id;
      grr_qsh_metric_e metric_id = (grr_qsh_metric_e)qsh_metric_id;
      gas_id_t gas_id = geran_map_nas_id_to_gas_id(params_ptr->params[metric_count].subs_id);

      write_stored_metric_data_to_qsh_buffer(metric_id, gas_id);

      perform_metric_log_done(
        metric_id,
        gas_id,
        QSH_CLIENT_METRIC_LOG_REASON_TIMER_EXPIRY
      );
    }
  }

  // Indicate call-back action done
  {
    qsh_client_action_done_s action_done;

    qsh_client_action_done_init(&action_done);

    action_done.cb_params_ptr = cb_params_ptr;
    action_done.action_mode_done = QSH_ACTION_MODE_DONE_ASYNC;

    qsh_client_action_done(&action_done);
  }

  return;
}

/*!
 * \brief Mapping table to convert from rr_l1_handover_type_T to sys_handover_failure_e_type.
 */
sys_handover_failure_e_type rr_to_sys_handover_type_map[RR_L1_HANDOVER_TYPE_COUNT] =
{
  /* RR_L1_GSM_GSM */     SYS_HANDOVER_FAILURE_G_TO_G_FAILURE,
  /* RR_L1_WCDMA_GSM */   SYS_HANDOVER_FAILURE_W_TO_G_FAILURE,
  /* RR_L1_GSM_WCDMA */   SYS_HANDOVER_FAILURE_G_TO_W_FAILURE,
  /* RR_L1_TDS_GSM */     SYS_HANDOVER_FAILURE_T_TO_G_FAILURE,
  /* RR_L1_LTE_GSM*/      SYS_HANDOVER_FAILURE_L_TO_G_FAILURE
};

/*!
 * \brief Maps rr_l1_handover_type_T to sys_handover_failure_e_type.
 *
 * \param rr_l1_handover_type_T
 * \return sys_handover_failure_e_type
 */
sys_handover_failure_e_type rr_qsh_metric_get_sys_ho_type(rr_l1_handover_type_T rr_l1_ho_type)
{
  return (rr_l1_ho_type < RR_L1_HANDOVER_TYPE_COUNT) ? 
         rr_to_sys_handover_type_map[rr_l1_ho_type] : 
         SYS_HANDOVER_FAILURE_MAX;
}

/* ---------------------  Event Metrics logging APIs  ---------------------- */

/*!
 * \brief Logs GRR_QSH_METRIC_MSG.
 *
 * \param msg_set
 * \param msg_id
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_msg(uint8 msg_set, uint32 msg_id, const gas_id_t gas_id)
{
  // Get latest QSH buffer pointer to write metric data
  uint8 *qsh_buffer_ptr = get_qsh_metric_buffer(GRR_QSH_METRIC_MSG, gas_id);

  if (qsh_buffer_ptr != NULL)
  {
    // Cast the QSH buffer pointer to point to the metric type
    grr_qsh_metric_msg_s *metric_ptr = (grr_qsh_metric_msg_s *)qsh_buffer_ptr;

    metric_ptr->msg_set = msg_set;
    metric_ptr->msg_id = msg_id;

    perform_metric_log_done(
      GRR_QSH_METRIC_MSG,
      gas_id,
      QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL
    );
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_IMSG.
 *
 * \param msg_set
 * \param msg_id
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_imsg(uint8 msg_set, uint8 msg_id, const gas_id_t gas_id)
{
  // Get latest QSH buffer pointer to write metric data
  uint8 *qsh_buffer_ptr = get_qsh_metric_buffer(GRR_QSH_METRIC_IMSG, gas_id);

  if (qsh_buffer_ptr != NULL)
  {
    // Cast the QSH buffer pointer to point to the metric type
    grr_qsh_metric_imsg_s *metric_ptr = (grr_qsh_metric_imsg_s *)qsh_buffer_ptr;

    metric_ptr->msg_set = msg_set;
    metric_ptr->msg_id = msg_id;

    perform_metric_log_done(
      GRR_QSH_METRIC_IMSG,
      gas_id,
      QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL
    );
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_OTA_MSG.
 *
 * \param channel_type
 * \param msg
 * \param len
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_ota_msg(uint8 channel_type, uint8 *msg_ptr, uint8 msg_len, const gas_id_t gas_id)
{
  // Get latest QSH buffer pointer to write metric data
  uint8 *qsh_buffer_ptr = get_qsh_metric_buffer(GRR_QSH_METRIC_OTA_MSG, gas_id);

  if (qsh_buffer_ptr != NULL)
  {
    // Cast the QSH buffer pointer to point to the metric type
    grr_qsh_metric_ota_msg_s *metric_ptr = (grr_qsh_metric_ota_msg_s *)qsh_buffer_ptr;

    // Clear the OTA message buffer
    memset(metric_ptr->msg_buf, 0, sizeof(metric_ptr->msg_buf));

    // Limit the message size to that of the metric buffer
    msg_len = MIN(sizeof(metric_ptr->msg_buf), msg_len);

    // Populate the metric content
    metric_ptr->channel_type = channel_type;
    memscpy(metric_ptr->msg_buf, sizeof(metric_ptr->msg_buf), msg_ptr, msg_len);

    perform_metric_log_done(
      GRR_QSH_METRIC_OTA_MSG,
      gas_id,
      QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL
    );
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_EVENT.
 *
 * \param event
 * \param sm
 * \param state
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_event(uint16 event, uint8 sm, uint8 state, const gas_id_t gas_id)
{
  // Get latest QSH buffer pointer to write metric data
  uint8 *qsh_buffer_ptr = get_qsh_metric_buffer(GRR_QSH_METRIC_EVENT, gas_id);

  if (qsh_buffer_ptr != NULL)
  {
    // Cast the QSH buffer pointer to point to the metric type
    grr_qsh_metric_event_s *metric_ptr = (grr_qsh_metric_event_s *)qsh_buffer_ptr;

    // Populate the metric content
    metric_ptr->event = event;
    metric_ptr->sm = sm;
    metric_ptr->state = state;

    perform_metric_log_done(
      GRR_QSH_METRIC_EVENT,
      gas_id,
      QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL
    );
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_CELL_INFO.
 *
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_cell_info(const gas_id_t gas_id)
{
  // Get latest QSH buffer pointer to write metric data
  uint8 *qsh_buffer_ptr = get_qsh_metric_buffer(GRR_QSH_METRIC_CELL_INFO, gas_id);

  if (qsh_buffer_ptr != NULL)
  {
    ARFCN_T arfcn = rr_scell_arfcn(gas_id);
    LAI_T lai = rr_scell_lai(gas_id);
    uint8 rac = rr_scell_rac(gas_id);
    uint16 cell_identity = rr_scell_cell_identity(gas_id);

    // Cast the QSH buffer pointer to point to the metric type
    grr_qsh_metric_cell_info_s *metric_ptr = (grr_qsh_metric_cell_info_s *)qsh_buffer_ptr;

    metric_ptr->arfcn = arfcn.num;
    metric_ptr->plmn_id[0] = (uint8)lai.plmn_id.identity[0];
    metric_ptr->plmn_id[1] = (uint8)lai.plmn_id.identity[1];
    metric_ptr->plmn_id[2] = (uint8)lai.plmn_id.identity[2];
    metric_ptr->lac = (uint16)lai.location_area_code;
    metric_ptr->rac = rac;
    metric_ptr->cell_identity = cell_identity;

    perform_metric_log_done(
      GRR_QSH_METRIC_CELL_INFO,
      gas_id,
      QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL
    );
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_CONN_END_INFO.
 *
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_conn_end_info(sys_call_end_event_e_type type, uint8 cause, const gas_id_t gas_id)
{
  // Get latest QSH buffer pointer to write metric data
  uint8 *qsh_buffer_ptr = get_qsh_metric_buffer(GRR_QSH_METRIC_CONN_END_INFO, gas_id);

  if (qsh_buffer_ptr != NULL)
  {
    // Cast the QSH buffer pointer to point to the metric type
    grr_qsh_metric_conn_end_info_s *metric_ptr = (grr_qsh_metric_conn_end_info_s *)qsh_buffer_ptr;

    metric_ptr->type = type;
    metric_ptr->cause = cause;

    perform_metric_log_done(
      GRR_QSH_METRIC_CONN_END_INFO,
      gas_id,
      QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL
    );
  }
}

/* --------------------  Sampled Metrics logging APIs  -------------------- */

/*!
 * \brief Logs GRR_QSH_METRIC_RESEL (G2G member).
 *
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_resel_g2g(const gas_id_t gas_id)
{
  if (get_qsh_metric_buffer(GRR_QSH_METRIC_RESEL, gas_id) != NULL)
  {
    uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    rr_metric_data.stored_data.resel[sub_index].num_of_resel_g2g++;
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_RESEL (G2WT member).
 *
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_resel_g2wt(const gas_id_t gas_id)
{
  if (get_qsh_metric_buffer(GRR_QSH_METRIC_RESEL, gas_id) != NULL)
  {
    uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    rr_metric_data.stored_data.resel[sub_index].num_of_resel_g2wt++;
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_RESEL (G2L member).
 *
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_resel_g2l(const gas_id_t gas_id)
{
  if (get_qsh_metric_buffer(GRR_QSH_METRIC_RESEL, gas_id) != NULL)
  {
    uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    rr_metric_data.stored_data.resel[sub_index].num_of_resel_g2l++;
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_HANDOVER.
 *
 * \param type
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_handover(rr_log_handover_type_T type, const gas_id_t gas_id)
{
  if (get_qsh_metric_buffer(GRR_QSH_METRIC_HANDOVER, gas_id) != NULL)
  {
    uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    switch (type)
    {
      case RR_LOG_HO_TYPE_GSM_TO_GSM:
      {
        rr_metric_data.stored_data.handover[sub_index].num_of_handover_g2g++;
        break;
      }

      case RR_LOG_HO_TYPE_GSM_TO_WCDMA:
      {
        rr_metric_data.stored_data.handover[sub_index].num_of_handover_g2w++;
        break;
      }

      case RR_LOG_HO_TYPE_WCDMA_TO_GSM:
      {
        rr_metric_data.stored_data.handover[sub_index].num_of_handover_w2g++;
        break;
      }

      case RR_LOG_HO_TYPE_LTE_TO_GSM:
      {
        rr_metric_data.stored_data.handover[sub_index].num_of_handover_l2g++;
        break;
      }

      default: ;
    }
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_RACH (num_of_rach member).
 *
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_rach(const gas_id_t gas_id)
{
  if (get_qsh_metric_buffer(GRR_QSH_METRIC_RACH, gas_id) != NULL)
  {
    uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    rr_metric_data.stored_data.rach[sub_index].num_of_rach++;
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_RACH (num_of_ia_rcvd member).
 *
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_ia_rcvd(const gas_id_t gas_id)
{
  if (get_qsh_metric_buffer(GRR_QSH_METRIC_RACH, gas_id) != NULL)
  {
    uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    rr_metric_data.stored_data.rach[sub_index].num_of_ia_rcvd++;
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_RACH (num_of_ia_rej_rcvd member).
 *
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_ia_rej_rcvd(const gas_id_t gas_id)
{
  if (get_qsh_metric_buffer(GRR_QSH_METRIC_RACH, gas_id) != NULL)
  {
    uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    rr_metric_data.stored_data.rach[sub_index].num_of_ia_rej_rcvd++;
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_CSFB.
 *
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_csfb(const gas_id_t gas_id)
{
  if (get_qsh_metric_buffer(GRR_QSH_METRIC_CSFB, gas_id) != NULL)
  {
    uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    rr_metric_data.stored_data.csfb[sub_index].num_of_csfb++;
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_SERVICE (num_of_dsf).
 *
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_dsf(const gas_id_t gas_id)
{
  if (get_qsh_metric_buffer(GRR_QSH_METRIC_SERVICE, gas_id) != NULL)
  {
    uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    rr_metric_data.stored_data.service[sub_index].num_of_dsf++;
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_SERVICE (num_of_rlf).
 *
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_rlf(const gas_id_t gas_id)
{
  if (get_qsh_metric_buffer(GRR_QSH_METRIC_SERVICE, gas_id) != NULL)
  {
    uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    rr_metric_data.stored_data.service[sub_index].num_of_dsf++;
  }
}

/*!
 * \brief Logs GRR_QSH_METRIC_SERVICE (num_of_oos).
 *
 * \param gas_id
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_metric_log_oos(const gas_id_t gas_id)
{
  if (get_qsh_metric_buffer(GRR_QSH_METRIC_SERVICE, gas_id) != NULL)
  {
    uint8 sub_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    rr_metric_data.stored_data.service[sub_index].num_of_oos++;
  }
}

/*!
 * \brief Initialisation function called from RR-QSH when metric functionality is required.
 */
__attribute__((section(".uncompressible.text")))
qsh_client_metric_info_s rr_qsh_metric_init(void)
{
  uint8 sub_index;
  uint8 metric_count;
  qsh_client_metric_info_s rr_qsh_client_metric_info;

  // Zero data structure across all subs
  memset(&rr_metric_data, 0, sizeof(rr_metric_data));

  for (sub_index = 0; sub_index < NUM_GERAN_DATA_SPACES; sub_index++)
  {
    qsh_client_metric_cfg_init(rr_qsh_metric_cfg_arr[sub_index], GRR_QSH_METRIC_COUNT);

    for (metric_count = 0; metric_count < GRR_QSH_METRIC_COUNT; metric_count++)
    {
      qsh_ext_metric_cfg_s *metric_cfg_ptr = &rr_qsh_metric_cfg_arr[sub_index][metric_count];

      metric_cfg_ptr->id = (qsh_metric_id_t)metric_count;
      metric_cfg_ptr->sampling_period_ms = rr_metric_def[metric_count].sampling_rate;
      metric_cfg_ptr->fifo.element_size_bytes = rr_metric_def[metric_count].size_bytes;
      metric_cfg_ptr->fifo.element_count_total = rr_metric_def[metric_count].num_of_elements;
      metric_cfg_ptr->subs_id = geran_map_gas_id_to_nas_id((gas_id_t)sub_index);
    }
  }

  rr_qsh_client_metric_info.metric_cfg_arr_ptr = &rr_qsh_metric_cfg_arr[0][0];
  rr_qsh_client_metric_info.metric_cfg_count = NUM_GERAN_DATA_SPACES * GRR_QSH_METRIC_COUNT;

  return rr_qsh_client_metric_info;
}

#endif // FEATURE_QSH_EVENT_METRIC

/* EOF */
