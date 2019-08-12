/*****************************************************************************
***
*** TITLE
***
***  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_debug.c#1 $
***
*** DESCRIPTION
***
***  Contains functions to aid debugging
***
*** Copyright (c) 2002-2014 Qualcomm Technologies, Inc.
*** All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
*** software is regulated by the U.S. Government. Diversion contrary to U.S.
*** law prohibited.
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "err.h"
#include "rr_seg_load.h"
#include "rr_gprs_debug.h"
#include "rr_gprs_defs.h"
#include "ms.h"
#include "rr_task.h"
#include "ms_timer.h"
#include "timers.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mac_grr.h"
#include "gmacgrrsig.h"
#include "mm_rr.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "rlc_grr.h"
#include "rr_select_bcch.h"
#ifdef FEATURE_FEMTO_GSM_NL
#include "rr_femto_if.h"
#include "rr_femto.h"
#endif // FEATURE_FEMTO_GSM_NL
#include "rr_ps_if.h"
#ifdef FEATURE_GSM_TDS
#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"
#endif /*FEATURE_GSM_TDS*/
#include "rr_multi_sim.h"
#include "gtmrs_g.h"
#include "stringl/stringl.h"
#ifdef FEATURE_QSH_DUMP
#include "rr_qsh_dump.h"
#endif // FEATURE_QSH_DUMP
#ifdef FEATURE_QSH_EVENT_METRIC
#include "rr_qsh_metric.h"
#endif // FEATURE_QSH_EVENT_METRIC

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#ifdef DEBUG_RR_TRACE_MSG
#define RR_NUM_TRACE_MSGS 100
#endif /* DEBUG_RR_TRACE_MSG */

#define RR_DEBUG_MM_RR_TRACE_BUFFER_SIZE          50
#define RR_DEBUG_RR_L1_TRACE_BUFFER_SIZE          50
#define RR_DEBUG_MAC_RR_TRACE_BUFFER_SIZE         10
#define RR_DEBUG_RR_MAC_TRACE_BUFFER_SIZE         10
#define RR_DEBUG_WCDMA_RRC_RR_TRACE_BUFFER_SIZE   10
#define RR_DEBUG_TDS_RRC_RR_TRACE_BUFFER_SIZE     10
#define RR_DEBUG_MSGR_RR_TRACE_BUFFER_SIZE        10
#define RR_DEBUG_PANIC_RESET_TRACE_BUFFER_SIZE    10

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

#ifdef DEBUG_RR_TRACE_MSG

typedef struct
{
  uint32            timestamp;       // 4 bytes
  rr_grr_state_mc_T sm_id;           // 1 byte
  byte              state;           // 1 byte
  rr_event_T        event;           // 2 bytes
  byte              message_set;     // 1 byte
  byte              message_id;      // 1 byte
  byte              message_frag[2]; // 2 bytes (remaining space that would otherwise be padding)
} rr_trace_msg_T;

#endif /* DEBUG_RR_TRACE_MSG */

typedef struct
{
  uint32                  timestamp;
  mm_rr_message_id_enum_T message_id;
  uint32                  pended_count;
} rr_debug_mm_rr_trace_buffer_entry_t;

typedef struct
{
  uint32                   timestamp_fn;
  timetick_type            timestamp_clk;
  mac_grr_sig_id_t         message_id;
  uint32                   pended_count;
} rr_debug_mac_rr_trace_buffer_entry_t;

typedef struct
{
  uint32                   timestamp_fn;
  timetick_type            timestamp_clk;
  grr_mac_sig_id_t         message_id;
} rr_debug_rr_mac_trace_buffer_entry_t;

typedef struct
{
  uint32                  timestamp;
  rr_l1_message_id_enum_T message_id;
  uint32                  pended_count;
} rr_debug_rr_l1_trace_buffer_entry_t;

#define RR_DEBUG_NAS_TRANSACTION_PAIRS 6
typedef struct
{
  mm_rr_message_id_enum_T incoming_msg;
  timetick_type           incoming_clk;
  mm_rr_message_id_enum_T outgoing_msg;
  timetick_type           outgoing_clk;
  timetick_type           elapsed_time;
  boolean                 valid_record;
} rr_debug_nas_transaction_record_t;

#ifdef FEATURE_WCDMA

typedef struct
{
  uint32            timestamp;
  rrc_rr_cmd_e_type message_id;
  uint32            pended_count;
} rr_debug_wcdma_rrc_rr_trace_buffer_entry_t;

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS

typedef struct
{
  uint32               timestamp;
  tdsrr_rrc_cmd_e_type message_id;
  uint32               pended_count;
} rr_debug_tds_rrc_rr_trace_buffer_entry_t;

#endif /* FEATURE_GSM_TDS */

typedef struct
{
  uint32         timestamp;
  msgr_umid_type message_id;
  uint32         pended_count;
} rr_debug_msgr_rr_trace_buffer_entry_t;

typedef struct
{
#ifdef DEBUG_RR_TRACE_MSG
  rr_trace_msg_T rr_trace_msg_buf[RR_NUM_TRACE_MSGS];
  byte           rr_trace_msg_buf_index;
#endif

  rr_debug_mm_rr_trace_buffer_entry_t          mm_rr_trace_buffer[RR_DEBUG_MM_RR_TRACE_BUFFER_SIZE];
  uint32                                       mm_rr_trace_buffer_index;

  rr_debug_mac_rr_trace_buffer_entry_t         mac_rr_trace_buffer[RR_DEBUG_MAC_RR_TRACE_BUFFER_SIZE];
  uint32                                       mac_rr_trace_buffer_index;

  rr_debug_rr_mac_trace_buffer_entry_t         rr_mac_trace_buffer[RR_DEBUG_RR_MAC_TRACE_BUFFER_SIZE];
  uint32                                       rr_mac_trace_buffer_index;

  rr_debug_rr_l1_trace_buffer_entry_t          rr_l1_trace_buffer[RR_DEBUG_RR_L1_TRACE_BUFFER_SIZE];
  uint32                                       rr_l1_trace_buffer_index;

#ifdef FEATURE_WCDMA
  rr_debug_wcdma_rrc_rr_trace_buffer_entry_t   wcdma_rrc_rr_trace_buffer[RR_DEBUG_WCDMA_RRC_RR_TRACE_BUFFER_SIZE]; 
  uint32                                       wcdma_rrc_rr_trace_buffer_index;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
  rr_debug_tds_rrc_rr_trace_buffer_entry_t     tds_rrc_rr_trace_buffer[RR_DEBUG_TDS_RRC_RR_TRACE_BUFFER_SIZE];
  uint32                                       tds_rrc_rr_trace_buffer_index;
#endif /* FEATURE_GSM_TDS */

  rr_debug_msgr_rr_trace_buffer_entry_t        msgr_rr_trace_buffer[RR_DEBUG_MSGR_RR_TRACE_BUFFER_SIZE];
  uint32                                       msgr_rr_trace_buffer_index;
  
  uint32                                       panic_reset_trace_buffer[RR_DEBUG_PANIC_RESET_TRACE_BUFFER_SIZE];
  uint32                                       panic_reset_trace_buffer_index;

  rr_debug_nas_transaction_record_t            nas_transactions[RR_DEBUG_NAS_TRANSACTION_PAIRS];
} rr_gprs_debug_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_gprs_debug_data_t  rr_gprs_debug_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 * 
 * \param gas_id (in)
 * 
 * \return rr_gprs_debug_data_t* 
 */
static rr_gprs_debug_data_t *rr_gprs_debug_get_data_ptr(const gas_id_t gas_id)
{
    return(&(rr_gprs_debug_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)]));
}

/*!
 * \brief Populate the NAS transactions buffer with the given message pair.
 * 
 * \param rr_gprs_debug_data_ptr (in)
 * \param incoming_msg (in)
 * \param outgoing_msg (in)
 */
static void rr_debug_init_nas_transaction(rr_gprs_debug_data_t    *rr_gprs_debug_data_ptr,
                                          mm_rr_message_id_enum_T  incoming_msg,
                                          mm_rr_message_id_enum_T  outgoing_msg)
{
  if (rr_gprs_debug_data_ptr != NULL)
  {
    int trans_index;

    for (trans_index = 0; trans_index < RR_DEBUG_NAS_TRANSACTION_PAIRS; trans_index++)
    {
      /* find the first uninitialised transaction record and store the details there */
      if (FALSE == rr_gprs_debug_data_ptr->nas_transactions[trans_index].valid_record)
      {
        rr_gprs_debug_data_ptr->nas_transactions[trans_index].incoming_msg = incoming_msg;
        rr_gprs_debug_data_ptr->nas_transactions[trans_index].incoming_clk = 0;
        rr_gprs_debug_data_ptr->nas_transactions[trans_index].outgoing_msg = outgoing_msg;
        rr_gprs_debug_data_ptr->nas_transactions[trans_index].outgoing_clk = 0;
        rr_gprs_debug_data_ptr->nas_transactions[trans_index].elapsed_time = 0;
        rr_gprs_debug_data_ptr->nas_transactions[trans_index].valid_record = TRUE;
        break;
      }
    }
  }

  return;
}

/*!
 * \brief Update the NAS transactions buffer for the given message type.
 * 
 * \param rr_gprs_debug_data_ptr (in)
 * \param msg_id (in)
 */
static void rr_debug_update_nas_transactions(rr_gprs_debug_data_t *rr_gprs_debug_data_ptr, const mm_rr_message_id_enum_T msg_id)
{
  if (rr_gprs_debug_data_ptr != NULL)
  {
    timetick_type msg_time;
    int           trans_index;

    /* retrieve the current slow clock timestamp */
    msg_time = timetick_get();

    for (trans_index = 0; trans_index < RR_DEBUG_NAS_TRANSACTION_PAIRS; trans_index++)
    {
      /* find all transaction records where either the incoming or outgoing primitive matches */
      if (rr_gprs_debug_data_ptr->nas_transactions[trans_index].valid_record)
      {
        if (msg_id == rr_gprs_debug_data_ptr->nas_transactions[trans_index].incoming_msg)
        {
          /* matched against incoming message ID, record start of new transaction */
          rr_gprs_debug_data_ptr->nas_transactions[trans_index].incoming_clk = msg_time;
          rr_gprs_debug_data_ptr->nas_transactions[trans_index].outgoing_clk = 0;
          rr_gprs_debug_data_ptr->nas_transactions[trans_index].elapsed_time = 0;
        }
        else if (msg_id == rr_gprs_debug_data_ptr->nas_transactions[trans_index].outgoing_msg)
        {
          /* matched against outgoing message ID, calculate the response time in milliseconds */
          rr_gprs_debug_data_ptr->nas_transactions[trans_index].outgoing_clk = msg_time;
          rr_gprs_debug_data_ptr->nas_transactions[trans_index].elapsed_time = timetick_diff(rr_gprs_debug_data_ptr->nas_transactions[trans_index].incoming_clk,
                                                                                             rr_gprs_debug_data_ptr->nas_transactions[trans_index].outgoing_clk,
                                                                                             T_MSEC);
        }
      }
    }
  }

  return;
}

#ifdef DEBUG_RR_TRACE_MSG
/*!
 * \brief Provides a filter for messages which shouldn't be logged in the trace buffer.
 * 
 * \param event (in)
 * \param msg_ptr (in)
 * 
 * \return boolean - TRUE if the message should be logged, FALSE if not
 */
static boolean should_msg_be_logged(rr_event_T event, const void *msg_ptr)
{
  switch (event)
  {
    case EV_NO_EVENT:
    case EV_PATH_LOSS_TIMEOUT:
    case EV_RADIO_LINK_FAILURE:
    {
      return FALSE;
    }

    case EV_INPUT_MESSAGE:
    {
      if(NULL != msg_ptr)
      {
        rr_cmd_bdy_type *rr_msg_ptr = (rr_cmd_bdy_type *)msg_ptr;

        switch (rr_msg_ptr->message_header.message_set)
        {
          case MS_RR_RR:
          {
            switch (rr_msg_ptr->rr.header.rr_message_set)
            {
#ifdef FEATURE_GSM_BAND_AVOIDANCE
              case RR_BAND_AVOID_IMSG:
              {
                switch (rr_msg_ptr->rr.band_avoid.imsg.header.imh.message_id)
                {
                  case RR_BAND_AVOID_IMSG_MEAS_RPT_SENT_IND:
                  {
                    return FALSE;
                  }

                  default: ;
                }

                break;
              }
#endif // FEATURE_GSM_BAND_AVOIDANCE

              default: ;
            }

            break;
          }

          default: ;
        }

        break;
      }/* NULL!= msg_ptr */
    }

    default: ;
  }

  return TRUE;
}
#endif // DEBUG_RR_TRACE_MSG

/*!
 * \brief Logs MM->RR and RR->MM messages.
 * 
 * \param message_id (in)
 * \param pended_count (in)
 * \param gas_id (in)
 */
static void rr_debug_log_mm_rr_message(
  mm_rr_message_id_enum_T message_id,
  uint32                  pended_count,
  const gas_id_t          gas_id
)
{
  rr_debug_mm_rr_trace_buffer_entry_t * entry;
  uint32 timestamp = GSTMR_GET_FN_GERAN(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_debug_data_t *rr_gprs_debug_data_ptr = rr_gprs_debug_get_data_ptr(gas_id);

  /* update the NAS transaction pair trace */
  rr_debug_update_nas_transactions(rr_gprs_debug_data_ptr, message_id);

  if (rr_gprs_debug_data_ptr->mm_rr_trace_buffer_index >= RR_DEBUG_MM_RR_TRACE_BUFFER_SIZE)
  {
    rr_gprs_debug_data_ptr->mm_rr_trace_buffer_index = 0;
  }

  entry = &rr_gprs_debug_data_ptr->mm_rr_trace_buffer[rr_gprs_debug_data_ptr->mm_rr_trace_buffer_index];

  entry->timestamp = timestamp;
  entry->message_id = message_id;
  entry->pended_count = pended_count;

  ++rr_gprs_debug_data_ptr->mm_rr_trace_buffer_index;

#ifdef FEATURE_QSH_DUMP
  {
    timetick_type timestamp_clk = timetick_get();
    rr_qsh_dump_log_msg(timestamp_clk, MS_MM_RR, (uint32)message_id, gas_id);
  }
#endif // FEATURE_QSH_DUMP

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_msg(MS_MM_RR, (uint32)message_id, gas_id);
#endif // FEATURE_QSH_EVENT_METRIC
}

/*!
 * \brief Logs MAC->RR messages.
 * 
 * \param message_id (in)
 * \param pended_count (in)
 * \param gas_id (in)
 */
static void rr_debug_log_mac_rr_message(
  mac_grr_sig_id_t        message_id,
  uint32                  pended_count,
  const gas_id_t          gas_id
)
{
  rr_debug_mac_rr_trace_buffer_entry_t * entry;
  uint32 timestamp_fn = GSTMR_GET_FN_GERAN(gas_id);
  timetick_type timestamp_clk = timetick_get();

  // Obtain a pointer to the module data
  rr_gprs_debug_data_t *rr_gprs_debug_data_ptr = rr_gprs_debug_get_data_ptr(gas_id);

  if (rr_gprs_debug_data_ptr->mac_rr_trace_buffer_index >= RR_DEBUG_MAC_RR_TRACE_BUFFER_SIZE)
  {
    rr_gprs_debug_data_ptr->mac_rr_trace_buffer_index = 0;
  }

  entry = &rr_gprs_debug_data_ptr->mac_rr_trace_buffer[rr_gprs_debug_data_ptr->mac_rr_trace_buffer_index];

  entry->timestamp_fn = timestamp_fn;
  entry->timestamp_clk = timestamp_clk;
  entry->message_id = message_id;
  entry->pended_count = pended_count;

  ++rr_gprs_debug_data_ptr->mac_rr_trace_buffer_index;

#ifdef FEATURE_QSH_DUMP
  rr_qsh_dump_log_msg(timestamp_clk, MS_MAC_RR, (uint32)message_id, gas_id);
#endif // FEATURE_QSH_DUMP

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_msg(MS_MAC_RR, (uint32)message_id, gas_id);
#endif // FEATURE_QSH_EVENT_METRIC
}

/*!
 * \brief Logs L1->RR and RR->L1 messages. Note that some messages are filtered.
 * 
 * \param message_id (in)
 * \param pended_count (in)
 * \param gas_id (in)
 */
static void rr_debug_log_rr_l1_message(
  rr_l1_message_id_enum_T message_id,
  uint32                  pended_count,
  const gas_id_t          gas_id
)
{
  switch (message_id)
  {
    /* Various messages are not of interest in the trace buffer, so are
    ignored.  These generally relate to period events that occur in idle mode
    such as serving / neighbour cell measurements. */
    case MPH_BLOCK_QUALITY_IND:
    case MPH_SERVING_DED_MEAS_IND:
    case MPH_SERVING_IDLE_MEAS_IND:
    case MPH_SURROUND_MEAS_IND:
    case MPH_DECODE_BCCH_LIST_FAILURE_IND: /* Up to 140 sent during cell selection, would quickly fill the buffer */
    case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
    case MPH_SERVING_AUX_MEAS_IND:
    case MPH_FCCH_SCH_DECODE_IND: /* Up to 140 sent during cell selection, would quickly fill the buffer */
    case MPH_NC_MEASUREMENT_IND:
    case MPH_SERVING_MEAS_IND:
    case MPH_SURROUND_WCDMA_DEDICATED_MEAS_IND:
    case MPH_SURROUND_WCDMA_XFER_MEAS_IND:
    case MPH_SURROUND_IRAT_MEAS_IND:
    case MPH_SET_PRIORITY_IND:
    case MPH_DSC_THRESHOLD_IND:
    {
      break;
    } /* MPH_BLOCK_QUALITY_IND ... */

    default:
    {
      rr_debug_rr_l1_trace_buffer_entry_t * entry;
      uint32 timestamp = GSTMR_GET_FN_GERAN(gas_id);

      // Obtain a pointer to the module data
      rr_gprs_debug_data_t *rr_gprs_debug_data_ptr = rr_gprs_debug_get_data_ptr(gas_id);

      if (rr_gprs_debug_data_ptr->rr_l1_trace_buffer_index >= RR_DEBUG_RR_L1_TRACE_BUFFER_SIZE)
      {
        rr_gprs_debug_data_ptr->rr_l1_trace_buffer_index = 0;
      }

      entry = &rr_gprs_debug_data_ptr->rr_l1_trace_buffer[rr_gprs_debug_data_ptr->rr_l1_trace_buffer_index];

      entry->timestamp = timestamp;
      entry->message_id = message_id;
      entry->pended_count = pended_count;

      ++rr_gprs_debug_data_ptr->rr_l1_trace_buffer_index;
    }
  }

#ifdef FEATURE_QSH_DUMP
  {
    timetick_type timestamp_clk = timetick_get();
    rr_qsh_dump_log_msg(timestamp_clk, MS_RR_L1, (uint32)message_id, gas_id);
  }
#endif // FEATURE_QSH_DUMP

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_msg(MS_RR_L1, (uint32)message_id, gas_id);
#endif // FEATURE_QSH_EVENT_METRIC
}

#ifdef FEATURE_WCDMA
/*!
 * \brief Logs WRRC->RR and RR->WRRC messages.
 * 
 * \param message_id (in)
 * \param pended_count (in)
 * \param gas_id (in)
 */
static void rr_debug_log_rrc_rr_message(
  rrc_rr_cmd_e_type message_id,
  uint32            pended_count,
  const gas_id_t    gas_id
)
{
  rr_debug_wcdma_rrc_rr_trace_buffer_entry_t * entry;
  uint32 timestamp = GSTMR_GET_FN_GERAN(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_debug_data_t *rr_gprs_debug_data_ptr = rr_gprs_debug_get_data_ptr(gas_id);

  if (rr_gprs_debug_data_ptr->wcdma_rrc_rr_trace_buffer_index >= RR_DEBUG_WCDMA_RRC_RR_TRACE_BUFFER_SIZE)
  {
    rr_gprs_debug_data_ptr->wcdma_rrc_rr_trace_buffer_index = 0;
  }

  entry = &rr_gprs_debug_data_ptr->wcdma_rrc_rr_trace_buffer[rr_gprs_debug_data_ptr->wcdma_rrc_rr_trace_buffer_index];

  entry->timestamp = timestamp;
  entry->message_id = message_id;
  entry->pended_count = pended_count;

  ++rr_gprs_debug_data_ptr->wcdma_rrc_rr_trace_buffer_index;

#ifdef FEATURE_QSH_DUMP
  {
    timetick_type timestamp_clk = timetick_get();
    rr_qsh_dump_log_msg(timestamp_clk, MS_RRC_RR, (uint32)message_id, gas_id);
  }
#endif // FEATURE_QSH_DUMP

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_msg(MS_RRC_RR, (uint32)message_id, gas_id);
#endif // FEATURE_QSH_EVENT_METRIC
}

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
/*!
 * \brief Logs TDSRRC->RR and RR->TDSRRC messages.
 * 
 * \param message_id (in)
 * \param pended_count (in)
 * \param gas_id (in)
 */
static void rr_debug_log_tdsrrc_rr_message(
  tdsrr_rrc_cmd_e_type message_id,
  uint32               pended_count,
  const gas_id_t       gas_id
)
{
  rr_debug_tds_rrc_rr_trace_buffer_entry_t * entry;
  uint32 timestamp = GSTMR_GET_FN_GERAN(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_debug_data_t *rr_gprs_debug_data_ptr = rr_gprs_debug_get_data_ptr(gas_id);

  if (rr_gprs_debug_data_ptr->tds_rrc_rr_trace_buffer_index >= RR_DEBUG_TDS_RRC_RR_TRACE_BUFFER_SIZE)
  {
    rr_gprs_debug_data_ptr->tds_rrc_rr_trace_buffer_index = 0;
  }

  entry = &rr_gprs_debug_data_ptr->tds_rrc_rr_trace_buffer[rr_gprs_debug_data_ptr->tds_rrc_rr_trace_buffer_index];

  entry->timestamp = timestamp;
  entry->message_id = message_id;
  entry->pended_count = pended_count;

  ++rr_gprs_debug_data_ptr->tds_rrc_rr_trace_buffer_index;

#ifdef FEATURE_QSH_DUMP
  {
    timetick_type timestamp_clk = timetick_get();
    rr_qsh_dump_log_msg(timestamp_clk, MS_TDSRRC_RR, (uint32)message_id, gas_id);
  }
#endif // FEATURE_QSH_DUMP

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_msg(MS_TDSRRC_RR, (uint32)message_id, gas_id);
#endif // FEATURE_QSH_EVENT_METRIC
}

#endif /* FEATURE_GSM_TDS */

/*!
 * \brief Logs LRRC->RR and RR->LRRC messages.
 * 
 * \param message_id (in)
 * \param pended_count (in)
 * \param gas_id (in)
 */
static void rr_debug_log_msgr_rr_message(
  msgr_umid_type message_id,
  uint32         pended_count,
  const gas_id_t gas_id
)
{
  rr_debug_msgr_rr_trace_buffer_entry_t * entry;
  uint32 timestamp = GSTMR_GET_FN_GERAN(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_debug_data_t *rr_gprs_debug_data_ptr = rr_gprs_debug_get_data_ptr(gas_id);

  if (rr_gprs_debug_data_ptr->msgr_rr_trace_buffer_index >= RR_DEBUG_MSGR_RR_TRACE_BUFFER_SIZE)
  {
    rr_gprs_debug_data_ptr->msgr_rr_trace_buffer_index = 0;
  }

  entry = &rr_gprs_debug_data_ptr->msgr_rr_trace_buffer[rr_gprs_debug_data_ptr->msgr_rr_trace_buffer_index];

  entry->timestamp = timestamp;
  entry->message_id = message_id;
  entry->pended_count = pended_count;

  ++rr_gprs_debug_data_ptr->msgr_rr_trace_buffer_index;

#ifdef FEATURE_QSH_DUMP
  {
    timetick_type timestamp_clk = timetick_get();
    rr_qsh_dump_log_msg(timestamp_clk, MS_MSGR_RR, (uint32)message_id, gas_id);
  }
#endif // FEATURE_QSH_DUMP

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_msg(MS_MSGR_RR, (uint32)message_id, gas_id);
#endif // FEATURE_QSH_EVENT_METRIC
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Module initialisation function.
 * 
 * \param gas_id (in)
 */
void rr_gprs_debug_init(const gas_id_t gas_id)
{
  rr_gprs_debug_data_t *rr_gprs_debug_data_ptr;

  rr_gprs_debug_data_ptr = rr_gprs_debug_get_data_ptr(gas_id);

  if (rr_gprs_debug_data_ptr != NULL)
  {
    /* reset all the transaction records */
    memset(rr_gprs_debug_data_ptr->nas_transactions, 0, sizeof(rr_gprs_debug_data_ptr->nas_transactions));

    /* and then add transaction tracing for each pair of messages */
    rr_debug_init_nas_transaction(rr_gprs_debug_data_ptr, RR_STOP_GSM_MODE_REQ, RR_STOP_GSM_MODE_CNF);
    rr_debug_init_nas_transaction(rr_gprs_debug_data_ptr, RR_PLMN_SELECT_REQ,   RR_PLMN_SELECT_CNF);
    rr_debug_init_nas_transaction(rr_gprs_debug_data_ptr, RR_ABORT_REQ,         RR_ABORT_CNF);
    rr_debug_init_nas_transaction(rr_gprs_debug_data_ptr, RR_DEACT_REQ,         RR_DEACT_CNF);
    rr_debug_init_nas_transaction(rr_gprs_debug_data_ptr, RR_EST_REQ,           RR_EST_CNF);
    rr_debug_init_nas_transaction(rr_gprs_debug_data_ptr, RR_EST_REQ,           RR_ABORT_IND);
  }

  return;
}

#ifdef DEBUG_RR_TRACE_MSG

/*!
 * \brief Store events & messages in the RR trace buffer.
 *
 * \param sm_id (in)
 * \param event (in)
 * \param state (in)
 * \param msg_ptr (in)
 * \param gas_id (in)
 */
void rr_store_trace_msg_buf(
  const rr_grr_state_mc_T sm_id,
  const rr_event_T        event,
  const byte              state,
  const void *            msg_ptr,
  const gas_id_t          gas_id
)
{
  rr_trace_msg_T * entry;
  rr_gprs_debug_data_t *rr_gprs_debug_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_debug_data_ptr = rr_gprs_debug_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_debug_data_ptr);

  // Exit early if this event/message is not to be logged
  if (FALSE == should_msg_be_logged(event, msg_ptr)) return;

  entry = &rr_gprs_debug_data_ptr->rr_trace_msg_buf[rr_gprs_debug_data_ptr->rr_trace_msg_buf_index];

  entry->timestamp = GSTMR_GET_FN_GERAN(gas_id);
  entry->sm_id = sm_id;
  entry->state = state;
  entry->event = event;

  if ((event == EV_INPUT_MESSAGE) && (msg_ptr != NULL))
  {
    entry->message_set = ((IMH_T *) msg_ptr)->message_set;
    entry->message_id = ((IMH_T *) msg_ptr)->message_id;

    /* Grab 2 Bytes of Message */
    memscpy(entry->message_frag,
		    sizeof(entry->message_frag),
            ((byte *)msg_ptr + sizeof(IMH_T)),
            sizeof(entry->message_frag)
           );	
  }
  else
  {
    memset(entry->message_frag, 0, sizeof(entry->message_frag));
  }

  rr_gprs_debug_data_ptr->rr_trace_msg_buf_index++;

  if (rr_gprs_debug_data_ptr->rr_trace_msg_buf_index >= RR_NUM_TRACE_MSGS)
  {
    rr_gprs_debug_data_ptr->rr_trace_msg_buf_index = 0;
  }

#ifdef FEATURE_QSH_DUMP
  {
    timetick_type timestamp_clk = timetick_get();
    rr_qsh_dump_log_event(timestamp_clk, event, sm_id, state, gas_id);
  }
#endif // FEATURE_QSH_DUMP

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_event(event, sm_id, state, gas_id);
#endif // FEATURE_QSH_EVENT_METRIC

}
#endif /* DEBUG_RR_TRACE_MSG */

/*!
 * \brief Returns a text string representation of a MSGR UMID.
 * 
 * \param umid (in)
 * 
 * \return const char* 
 */
const char * rr_gprs_debug_msgr_umid_name(msgr_umid_type umid)
{
  switch (umid)
  {
    /* REQ */
#ifdef FEATURE_LTE
    case GERAN_GRR_LTE_CS_CAPABILITIES_REQ:      return "GERAN_GRR_LTE_CS_CAPABILITIES_REQ";
    case GERAN_GRR_LTE_PS_CAPABILITIES_REQ:      return "GERAN_GRR_LTE_PS_CAPABILITIES_REQ";
    case GERAN_GRR_LTE_RESEL_REQ:                return "GERAN_GRR_LTE_RESEL_REQ";
    case GERAN_GRR_LTE_ABORT_RESEL_REQ:          return "GERAN_GRR_LTE_ABORT_RESEL_REQ";
    case GERAN_GRR_LTE_REDIR_REQ:                return "GERAN_GRR_LTE_REDIR_REQ";
    case GERAN_GRR_LTE_ABORT_REDIR_REQ:          return "GERAN_GRR_LTE_ABORT_REDIR_REQ";
    case GERAN_GRR_LTE_CCO_REQ:                  return "GERAN_GRR_LTE_CCO_REQ";
    case GERAN_GRR_LTE_ABORT_CCO_REQ:            return "GERAN_GRR_LTE_ABORT_CCO_REQ";
    case GERAN_GRR_LTE_PLMN_SRCH_REQ:            return "GERAN_GRR_LTE_PLMN_SRCH_REQ";
    case GERAN_GRR_LTE_ABORT_PLMN_SRCH_REQ:      return "GERAN_GRR_LTE_ABORT_PLMN_SRCH_REQ";
    case GERAN_GRR_LTE_DEDICATED_PRIORITIES_REQ: return "GERAN_GRR_LTE_DEDICATED_PRIORITIES_REQ";
#ifdef FEATURE_LTE_TO_GSM_CGI
    case GERAN_GRR_LTE_GET_CGI_REQ:              return "GERAN_GRR_LTE_GET_CGI_REQ";
    case GERAN_GRR_LTE_ABORT_CGI_REQ:            return "GERAN_GRR_LTE_ABORT_CGI_REQ";
#endif /* FEATURE_LTE_TO_GSM_CGI */
    case GERAN_GRR_LTE_HO_REQ:                   return "GERAN_GRR_LTE_HO_REQ";
    case GERAN_GRR_LTE_ABORT_HO_REQ:             return "GERAN_GRR_LTE_ABORT_HO_REQ";
#ifdef FEATURE_SGLTE
    case GERAN_GRR_LTE_SGLTE_SERVING_MEAS_REQ:   return "GERAN_GRR_LTE_SGLTE_SERVING_MEAS_REQ";
    case GERAN_GRR_LTE_SGLTE_NEIGHBOR_MEAS_REQ:   return "GERAN_GRR_LTE_SGLTE_NEIGHBOR_MEAS_REQ";
#endif /* FEATURE_SGLTE */
#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
    case GERAN_GRR_LTE_GET_PLMN_PRTL_RESULTS_REQ:return "GERAN_GRR_LTE_GET_PLMN_PRTL_RESULTS_REQ";
#endif /* FEATURE_MPLMN_PARTIAL_UPDATING */

#ifdef FEATURE_LTE_REL11
    case GERAN_GRR_LTE_DEPRI_FREQ_REQ:           return "GERAN_GRR_LTE_DEPRI_FREQ_REQ";
#endif /*FEATURE_LTE_REL11*/

    /* RSP */
    case GERAN_GRR_LTE_CS_CAPABILITIES_RSP:      return "GERAN_GRR_LTE_CS_CAPABILITIES_RSP";
    case GERAN_GRR_LTE_PS_CAPABILITIES_RSP:      return "GERAN_GRR_LTE_PS_CAPABILITIES_RSP";
    case GERAN_GRR_LTE_RESEL_FAILED_RSP:         return "GERAN_GRR_LTE_RESEL_RSP";
    case GERAN_GRR_LTE_ABORT_RESEL_RSP:          return "GERAN_GRR_LTE_ABORT_RESEL_RSP";
    case GERAN_GRR_LTE_REDIR_FAILED_RSP:         return "GERAN_GRR_LTE_REDIR_RSP";
    case GERAN_GRR_LTE_ABORT_REDIR_RSP:          return "GERAN_GRR_LTE_ABORT_REDIR_RSP";
    case GERAN_GRR_LTE_CCO_RSP:                  return "GERAN_GRR_LTE_CCO_RSP";
    case GERAN_GRR_LTE_ABORT_CCO_RSP:            return "GERAN_GRR_LTE_ABORT_CCO_RSP";
    case GERAN_GRR_LTE_PLMN_SRCH_RSP:            return "GERAN_GRR_LTE_PLMN_SRCH_RSP";
    case GERAN_GRR_LTE_ABORT_PLMN_SRCH_RSP:      return "GERAN_GRR_LTE_ABORT_PLMN_SRCH_RSP";
    case GERAN_GRR_LTE_DEDICATED_PRIORITIES_RSP: return "GERAN_GRR_LTE_DEDICATED_PRIORITIES_RSP";
#ifdef FEATURE_LTE_TO_GSM_CGI
    case GERAN_GRR_LTE_GET_CGI_RSP:              return "GERAN_GRR_LTE_GET_CGI_RSP";
    case GERAN_GRR_LTE_ABORT_CGI_RSP:            return "GERAN_GRR_LTE_ABORT_CGI_RSP";
#endif /* FEATURE_LTE_TO_GSM_CGI */
    case GERAN_GRR_LTE_HO_FAILED_RSP:            return "GERAN_GRR_LTE_HO_FAILED_RSP";
    case GERAN_GRR_LTE_ABORT_HO_RSP:             return "GERAN_GRR_LTE_ABORT_HO_RSP";
#ifdef FEATURE_SGLTE
    case GERAN_GRR_LTE_SGLTE_SERVING_MEAS_RSP:   return "GERAN_GRR_LTE_SGLTE_SERVING_MEAS_RSP";
    case GERAN_GRR_LTE_SGLTE_NEIGHBOR_MEAS_RSP:   return "GERAN_GRR_LTE_SGLTE_NEIGHBOR_MEAS_RSP";	
#endif /* FEATURE_SGLTE */

    case GERAN_GRR_LTE_ABORT_CCO_FAILURE_PROCEDURE_IND: return "GERAN_GRR_LTE_ABORT_CCO_FAILURE_PROCEDURE_IND";
    case GERAN_GRR_LTE_SUSPEND_PLMN_SRCH_IND:           return "GERAN_GRR_LTE_SUSPEND_PLMN_SRCH_IND"; 

    /* G2L Resel messages */
    case LTE_RRC_G_RESEL_REQ:                    return "LTE_RRC_G_RESEL_REQ";
    case LTE_RRC_G_RESEL_FAILED_RSP:             return "LTE_RRC_G_RESEL_FAILED_RSP";
    case LTE_RRC_G_ABORT_RESEL_REQ:              return "LTE_RRC_G_ABORT_RESEL_REQ";
    case LTE_RRC_G_ABORT_RESEL_RSP:              return "LTE_RRC_G_ABORT_RESEL_RSP";

    /* G2L Redir messages */
    case LTE_RRC_G_REDIR_REQ:                    return "LTE_RRC_G_REDIR_REQ";
    case LTE_RRC_G_REDIR_FAILED_RSP:             return "LTE_RRC_G_REDIR_FAILED_RSP";
    case LTE_RRC_G_ABORT_REDIR_REQ:              return "LTE_RRC_G_REDIR_REQ";
    case LTE_RRC_G_ABORT_REDIR_RSP:              return "LTE_RRC_G_ABORT_REDIR_RSP";

    /* G2L PLMN Search messages */
    case LTE_RRC_G_PLMN_SRCH_REQ:                return "LTE_RRC_G_PLMN_SRCH_REQ";
    case LTE_RRC_G_PLMN_SRCH_RSP:                return "LTE_RRC_G_PLMN_SRCH_RSP";
    case LTE_RRC_G_ABORT_PLMN_SRCH_REQ:          return "LTE_RRC_G_ABORT_PLMN_SRCH_REQ";
    case LTE_RRC_G_ABORT_PLMN_SRCH_RSP:          return "LTE_RRC_G_ABORT_PLMN_SRCH_RSP";

    /* LTE RRC messages that we receive */
    case LTE_RRC_CCO_NACC_COMPLETED_IND:         return "LTE_RRC_CCO_NACC_COMPLETED_IND";
    case GERAN_GRR_LTE_GERAN_ACTIVITY_IND:       return "GERAN_GRR_LTE_GERAN_ACTIVITY_IND";
#endif /* FEATURE_LTE */

#ifdef FEATURE_SGLTE
    case GERAN_GRR_TDS_SGLTE_SERVING_MEAS_REQ:   return "GERAN_GRR_TDS_SGLTE_SERVING_MEAS_REQ";
    case GERAN_GRR_TDS_SGLTE_SERVING_MEAS_RSP:   return "GERAN_GRR_TDS_SGLTE_SERVING_MEAS_RSP";
    case GERAN_GRR_UPDATE_FORBIDDEN_BANDS_IND:   return "GERAN_GRR_UPDATE_FORBIDDEN_BANDS_IND";
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_GERAN_X2G_ACQ_DB_SCAN
    case GERAN_GRR_LTE_GERAN_FREQ_INFO_IND:      return "GERAN_GRR_LTE_GERAN_FREQ_INFO_IND";
#endif /*FEATURE_GERAN_X2G_ACQ_DB_SCAN*/

#ifdef FEATURE_GSM_BAND_AVOIDANCE
    case MCS_CXM_BAND_AVOID_BLIST_GSM1_IND:      return "MCS_CXM_BAND_AVOID_BLIST_GSM1_IND";
    case MCS_CXM_BAND_AVOID_BLIST_GSM2_IND:      return "MCS_CXM_BAND_AVOID_BLIST_GSM2_IND";
    case MCS_CXM_BAND_AVOID_BLIST_RSP:           return "MCS_CXM_BAND_AVOID_BLIST_RSP";
    case MCS_CXM_BAND_AVOID_FREQ_IND:            return "MCS_CXM_BAND_AVOID_FREQ_IND";
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

#ifdef FEATURE_LTE_REL11
    case LTE_RRC_DEPRI_REQUESTED_PLMN_IND: return "LTE_RRC_DEPRI_REQUESTED_PLMN_IND";
#endif /*FEATURE_LTE_REL11*/

#ifdef FEATURE_IDLE_DRX_SCALING
    case GERAN_GRR_MTC_CFG_REQ:                  return "GERAN_GRR_MTC_CFG_REQ";
    case GERAN_GRR_MTC_CFG_RSP:                  return "GERAN_GRR_MTC_CFG_RSP";
#endif /* FEATURE_IDLE_DRX_SCALING */

#ifdef FEATURE_FAKE_BTS_CELL_INFO
    case GERAN_GRR_QMI_CELL_INFO_IND:            return "GERAN_GRR_QMI_CELL_INFO_IND";
    case GERAN_GRR_QMI_CELL_INFO_RSP:            return "GERAN_GRR_QMI_CELL_INFO_RSP";
    case GERAN_GRR_QMI_CELL_INFO_REQ:            return "GERAN_GRR_QMI_CELL_INFO_REQ";
#endif /*FEATURE_FAKE_BTS_CELL_INFO*/

#ifdef FEATURE_FAKE_BTS_CELL_BARRING
    case GERAN_GRR_FAKE_GCELL_ACTION_REQ:        return "GERAN_GRR_FAKE_GCELL_ACTION_REQ";
#endif /*FEATURE_FAKE_BTS_CELL_BARRING*/

    default: return "?";
  }
}

/*!
 * \brief Returns a text string representation of the message passed in.
 * 
 * \param msg_ptr (in)
 * \param message_set (in)
 * \param message_id (in)
 * \param level (in)
 * 
 * \return const char* - character string (or NULL if message unknown)
 */
const char * rr_gprs_message_name
(
  const rr_cmd_bdy_type * msg_ptr,
  const uint8             message_set,
  const uint32            message_id,
  uint32                * level
)
{
  const char * s = "";

  // By default, log messages at HIGH level
  if (level)
  {
    *level = MSG_LEGACY_HIGH;
  }

  /**********************************************************************/
  /* For OTA messages, it is more helpful to log the OTA message rather */
  /* than just the transport message. If this is an OTA message         */
  /**********************************************************************/
  if (msg_ptr != NULL && message_set == MS_RR_L2 &&
      ((message_id == (byte) DL_UNIT_DATA_IND) || (message_id == (byte) DL_DATA_IND)))
  {
    /* the message is an OTA message */
    byte message_type = 0;
    byte protocol_discriminator;
    byte skip_indicator = 0;

    boolean log_message = FALSE;

    /* extract the message_type & protocol_discriminator from the message */
    switch (message_id)
    {
      case DL_DATA_IND:
      {
        s = "DlDataInd";
        skip_indicator = msg_ptr->dl_data_ind.layer3_message[0] & 0xF0;
        protocol_discriminator = msg_ptr->dl_data_ind.layer3_message[0] & 0x0F;
        message_type = msg_ptr->dl_data_ind.layer3_message[1] & 0xFF;

        switch (protocol_discriminator)
        {
          case RADIO_RESSOURCES :
          {
            if (skip_indicator == 0x00)
            {
              /* An RR peer-to-peer message that should be logged */
              log_message = TRUE;
            }
            else
            {
              s = "Unexpected Skip Indicator";
            }
          }
          break;

          default:
          {
            /* for all other cases, don't log the message directly */
            log_message = FALSE;
          }
          break;
        }
      } /* DL_DATA_IND */
      break;

      case DL_UNIT_DATA_IND:
      {
        byte l2_channel_type = msg_ptr->dl_unit_data_ind.l2_channel_type;
        s = "DlUnitDataInd";

        if ((l2_channel_type == BCCH) || (l2_channel_type == CCCH))
        {
          skip_indicator = msg_ptr->dl_unit_data_ind.layer3_message[1] & 0xF0;
          protocol_discriminator = msg_ptr->dl_unit_data_ind.layer3_message[1] & 0x0F;
          message_type = msg_ptr->dl_unit_data_ind.layer3_message[2] & 0xFF;
        }
        else
        {
          skip_indicator = msg_ptr->dl_unit_data_ind.layer3_message[0] & 0xF0;
          protocol_discriminator = msg_ptr->dl_unit_data_ind.layer3_message[0] & 0x0F;
          message_type = msg_ptr->dl_unit_data_ind.layer3_message[1] & 0xFF;
        }

        switch (protocol_discriminator)
        {
          case RADIO_RESSOURCES :
          {
            if (skip_indicator == 0x00)
            {
              log_message = TRUE;
            }
            else
            {
              s = "Unexpected Skip Indicator";
            }
          }
          break;

          default:
          {
            /* for all other cases, don't log the message directly */
            log_message = FALSE;
          }
          break;
        }
      } /* DL_UNIT_DATA_IND */
      break;

      default:
      {
        // should never happen
        MSG_GERAN_ERROR_0("Unexpected condition");
      }
      break;
    } /* switch (message_id) */

    if (log_message == TRUE)
    {
      /* log the OTA message, not the L2 message */
      switch (message_type)
      {
        case ADDITIONAL_ASSIGNMENT:
          s = "ADDITIONAL_ASSIGNMENT";
          break;

        case IMMEDIATE_ASSIGNMENT:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "IMMEDIATE_ASSIGNMENT";
          break;

        case IMMEDIATE_ASSIGNMENT_EXTENDED:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "IMMEDIATE_ASSIGNMENT_EXTENDED";
          break;

        case IMMEDIATE_ASSIGNMENT_REJECT:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "IMMEDIATE_ASSIGNMENT_REJECT";
          break;

        case CIPHERING_MODE_COMMAND:
          s = "CIPHERING_MODE_COMMAND";
          break;

        case CIPHERING_MODE_COMPLETE:
          s = "CIPHERING_MODE_COMPLETE";
          break;

        case ASSIGNMENT_COMMAND:
          s = "ASSIGNMENT_COMMAND";
          break;

        case ASSIGNMENT_COMPLETE:
          s = "ASSIGNMENT_COMPLETE";
          break;

        case ASSIGNMENT_FAILURE:
          s = "ASSIGNMENT_FAILURE";
          break;

        case HANDOVER_COMMAND:
          s = "HANDOVER_COMMAND";
          break;

        case INTERSYSTEM_TO_UTRAN_HANDOVER_COMMAND:
          s = "INTERSYSTEM_TO_UTRAN_HANDOVER_COMMAND";
          break;

        case HANDOVER_COMPLETE:
          s = "HANDOVER_COMPLETE";
         break;

        case HANDOVER_FAILURE:
          s = "HANDOVER_FAILURE";
          break;

        case PHYSICAL_INFORMATION:
          s = "PHYSICAL_INFORMATION";
          break;

        case CHANNEL_RELEASE:
          s = "CHANNEL_RELEASE";
          break;

        case PARTIAL_RELEASE:
          s = "PARTIAL_RELEASE";
          break;

        case PARTIAL_RELEASE_COMPLETE:
          s = "PARTIAL_RELEASE_COMPLETE";
          break;

        case PAGING_REQUEST_TYPE_1:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "PAGING_REQUEST_TYPE_1";
          break;

        case PAGING_REQUEST_TYPE_2:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "PAGING_REQUEST_TYPE_2";
          break;

        case PAGING_REQUEST_TYPE_3:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "PAGING_REQUEST_TYPE_3";
          break;

        case PAGING_RESPONSE:
          s = "PAGING_RESPONSE";
          break;

        case SYSTEM_INFORMATION_TYPE_1:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "SI1";
          break;

        case SYSTEM_INFORMATION_TYPE_2:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "SI2";
          break;

        case SYSTEM_INFORMATION_TYPE_2_BIS:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "SI2bis";
          break;

        case SYSTEM_INFORMATION_TYPE_2_TER:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "SI2ter";
          break;

        case SYSTEM_INFORMATION_TYPE_2_QUATER:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "SI2quater";
          break;

        case SYSTEM_INFORMATION_TYPE_3:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "SI3";
          break;

        case SYSTEM_INFORMATION_TYPE_4:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "SI4";
          break;

        case SYSTEM_INFORMATION_TYPE_5:
          if (level)
          {
            *level = MSG_LEGACY_LOW;
          }
          s = "SI5";
          break;

        case SYSTEM_INFORMATION_TYPE_5_BIS:
          if (level)
          {
            *level = MSG_LEGACY_LOW;
          }
          s = "SI5bis";
          break;

        case SYSTEM_INFORMATION_TYPE_5_TER:
          if (level)
          {
            *level = MSG_LEGACY_LOW;
          }
          s = "SI5Ter";
          break;

        case SYSTEM_INFORMATION_TYPE_6:
          if (level)
          {
            *level = MSG_LEGACY_LOW;
          }
          s = "SI6";
          break;

        case SYSTEM_INFORMATION_TYPE_7:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "SI7";
          break;

        case SYSTEM_INFORMATION_TYPE_8:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "SI8";
          break;

#if 0
        case SYSTEM_INFORMATION_TYPE_10:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "SI10";
          break;
#endif

        case SYSTEM_INFORMATION_TYPE_13:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "SI13";
          break;

        case ENHANCED_MEASUREMENT_REPORT:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "ENHANCED_MEASUREMENT_REPORT";
          break;

#if 0
        case MEASUREMENT_INFORMATION:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "MEASUREMENT_INFORMATION";
          break;
#endif

        case CHANNEL_MODE_MODIFY:
          s = "CHANNEL_MODE_MODIFY";
          break;

        case RR_STATUS:
          s = "RR_STATUS";
          break;

        case CHANNEL_MODE_MODIFY_ACKNOWLEDGE:
          s = "CHANNEL_MODE_MODIFY_ACKNOWLEDGE";
          break;

        case FREQUENCY_REDEFINITION:
          s = "FREQUENCY_REDEFINITION";
          break;

        case MEASUREMENT_REPORT:
          s = "MEASUREMENT_REPORT";
          break;

        case CLASSMARK_CHANGE:
          s = "CLASSMARK_CHANGE";
          break;

        case CLASSMARK_ENQUIRY:
          s = "CLASSMARK_ENQUIRY";
          break;

        case UTRAN_CLASSMARK_CHANGE:
          s = "UTRAN_CLASSMARK_CHANGE";
          break;

#ifdef FEATURE_CGPS_UMTS_CP_GSM
        case APPLICATION_INFORMATION:
          s = "APPLICATION_INFORMATION";
          break;
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

        #ifdef FEATURE_GSM_DTM

        case DTM_ASSIGNMENT_COMMAND:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "DTM_ASSIGNMENT_COMMAND";
          break;

        case DTM_ASSIGNMENT_FAILURE:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "DTM_ASSIGNMENT_FAILURE";
          break;

        case DTM_REQUEST:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "DTM_REQUEST";
          break;

        case DTM_REJECT:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "DTM_REJECT";
          break;

        case DTM_INFORMATION:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "DTM_INFORMATION";
          break;

        case PACKET_ASSIGNMENT:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "PACKET_ASSIGNMENT";
          break;

        case PACKET_NOTIFICATION:
          if (level)
          {
            *level = MSG_LEGACY_MED;
          }
          s = "PACKET_NOTIFICATION";
          break;

        #endif /* FEATURE_GSM_DTM */

        default:
          s = "Unknown OTA message";

      } /* switch (message_type) */
    } /* if (log_message == TRUE) */
  }
  else /* not an OTA message */
  {
    switch (message_set)
    {
#ifdef FEATURE_WCDMA
      /* RRC-RR messages */
      case MS_RRC_RR:
      {
        switch (message_id)
        {
          case RR_INTERRAT_HANDOVER_REQ:
            s = "RR_INTERRAT_HANDOVER_REQ";
            break;

          case RR_INTERRAT_HANDOVER_ABORT_REQ:
            s = "RR_INTERRAT_HANDOVER_ABORT_REQ";
            break;

          case RR_NAS_DATA_IND:
            s = "RR_NAS_DATA_IND";
            break;

          case RR_INTERRAT_RESELECTION_REQ:
            s = "RR_INTERRAT_RESELECTION_REQ";
            break;

          case RR_INTERRAT_CC_ORDER_REQ:
            s = "RR_INTERRAT_CC_ORDER_REQ";
            break;

          case RR_INTERRAT_CC_ORDER_ABORT_REQ:
            s = "RR_INTERRAT_CC_ORDER_ABORT_REQ";
            break;

		  case RRC_INTERRAT_CC_ORDER_ABORT_REQ:
            s = "RRC_INTERRAT_CC_ORDER_ABORT_REQ";
            break;

          case RRC_INTERRAT_RESELECTION_REJ:
            s = "RRC_INTERRAT_RESELECTION_REJ";
            break;

          case RRC_INTERRAT_HANDOVER_CNF:
            s = "RRC_INTERRAT_HANDOVER_CNF";
            break;

          case RRC_INTERRAT_HANDOVER_ABORT_CNF:
            s = "RRC_INTERRAT_HANDOVER_ABORT_CNF";
            break;

          case RRC_INTERRAT_HANDOVER_ABORT_REQ:
            s = "RRC_INTERRAT_HANDOVER_ABORT_REQ";
            break;

          case RRC_INTERRAT_HANDOVER_REQ:
            s = "RRC_INTERRAT_HANDOVER_REQ";
            break;

          case RR_INTERRAT_REDIRECT_REQ:
            s = "RR_INTERRAT_REDIRECT_REQ";
            break;

          case RR_INTERRAT_REDIRECT_ABORT_REQ:
            s = "RR_INTERRAT_REDIRECT_ABORT_REQ";
            break;

          case RR_INTERRAT_RESELECTION_ABORT_REQ:
            s = "RR_INTERRAT_RESELECTION_ABORT_REQ";
            break;

          case RRC_INTERRAT_RESELECTION_ABORT_CNF:
            s = "RRC_INTERRAT_RESELECTION_ABORT_CNF";
            break;

          case RR_INTERRAT_HANDOVER_ABORT_CNF:
            s = "RR_INTERRAT_HANDOVER_ABORT_CNF";
            break;

          case RR_INTERRAT_HANDOVER_CNF:
            s = "RR_INTERRAT_HANDOVER_CNF";
            break;

          case RR_INTERRAT_RESELECTION_REJ:
            s = "RR_INTERRAT_RESELECTION_REJ";
            break;

          case RR_INTERRAT_RESELECTION_ABORT_CNF:
            s = "RR_INTERRAT_RESELECTION_ABORT_CNF";
            break;

          case RR_INTERRAT_CC_ORDER_REJ:
            s = "RR_INTERRAT_CC_ORDER_REJ";
            break;

          case RRC_INTERRAT_RESELECTION_REQ:
            s = "RRC_INTERRAT_RESELECTION_REQ";
            break;

          case RRC_INTERRAT_RESELECTION_ABORT_REQ:
            s = "RRC_INTERRAT_RESELECTION_ABORT_REQ";
            break;
						
          case RRC_INTERRAT_CC_ORDER_REQ:
            s = "RRC_INTERRAT_CC_ORDER_REQ";
            break;

          case RRC_NAS_DATA_IND:
            s = "RRC_NAS_DATA_IND";
            break;

          case RR_INTERRAT_REDIRECT_REJ:
            s = "RR_INTERRAT_REDIRECT_REJ";
            break;

          case RR_INTERRAT_REDIRECT_ABORT_CNF:
            s = "RR_INTERRAT_REDIRECT_ABORT_CNF";
            break;

          case RRC_INTERRAT_PLMN_SRCH_CNF:
            s = "RRC_INTERRAT_PLMN_SRCH_CNF";
            break;

          case RRC_INTERRAT_PLMN_SRCH_ABORT_CNF:
            s = "RRC_INTERRAT_PLMN_SRCH_ABORT_CNF";
            break;

          case RRC_INTERRAT_PLMN_SRCH_REQ:
            s = "RRC_INTERRAT_PLMN_SRCH_REQ";
            break;

          case RRC_INTERRAT_PLMN_SRCH_ABORT_REQ:
            s = "RRC_INTERRAT_PLMN_SRCH_ABORT_REQ";
            break;

          case RR_INTERRAT_PLMN_SRCH_REQ:
            s = "RR_INTERRAT_PLMN_SRCH_REQ";
            break;

          case RR_INTERRAT_PLMN_SRCH_CNF:
            s = "RR_INTERRAT_PLMN_SRCH_CNF";
            break;

          case RR_INTERRAT_PLMN_SRCH_ABORT_REQ:
            s = "RR_INTERRAT_PLMN_SRCH_ABORT_REQ";
            break;

          case RR_INTERRAT_PLMN_SRCH_SUSPEND_IND:
            s = "RR_INTERRAT_PLMN_SRCH_SUSPEND_IND";
            break;

          case RR_INTERRAT_PLMN_SRCH_ABORT_CNF:
            s = "RR_INTERRAT_PLMN_SRCH_ABORT_CNF";
            break;

#ifdef FEATURE_INTERRAT_PCCO_GTOW
          case RRC_INTERRAT_CC_ORDER_CNF:
            s = "RRC_INTERRAT_CC_ORDER_CNF";
            break;
#endif /* FEATURE_INTERRAT_PCCO_GTOW */

          case RR_INTERRAT_CC_ORDER_CNF:
            s = "RR_INTERRAT_CC_ORDER_CNF";
            break;

          case RR_INTERRAT_CC_ORDER_ABORT_CNF:
            s = "RR_INTERRAT_CC_ORDER_ABORT_CNF";
            break;

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
          case RRC_INTERRAT_REDIRECT_REQ:
            s = "RRC_INTERRAT_REDIRECT_REQ";
            break;

          case RRC_INTERRAT_REDIRECT_ABORT_REQ:
            s = "RRC_INTERRAT_REDIRECT_ABORT_REQ";
            break;

          case RRC_INTERRAT_REDIRECT_REJ:
            s = "RRC_INTERRAT_REDIRECT_REJ";
            break;

          case RRC_INTERRAT_REDIRECT_ABORT_CNF:
            s = "RRC_INTERRAT_REDIRECT_ABORT_CNF";
            break;

#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

          case RR_CIPHER_SYNC_IND:
            s = "RR_CIPHER_SYNC_IND";
            break;

          case RRC_RR_STOP_WCDMA_REQ:
            s = "RRC_RR_STOP_WCDMA_REQ";
            break;

          case RRC_RR_STOP_WCDMA_CNF:
            s = "RRC_RR_STOP_WCDMA_CNF";
            break;

          case RR_RRC_STOP_GSM_REQ:
            s = "RR_RRC_STOP_GSM_REQ";
            break;

          case RR_RRC_STOP_GSM_CNF:
            s = "RR_RRC_STOP_GSM_CNF";
            break;

#ifdef FEATURE_FEMTO_GSM_NL
          case RRC_RR_NBR_DISC_REQ:
            s = "RRC_RR_NBR_DISC_REQ";
            break;

          case RR_RRC_NBR_DISC_CNF:
            s = "RR_RRC_NBR_DISC_CNF";
            break;

          case RRC_RR_START_COSC_REQ:
            s = "RR_RRC_START_COSC_REQ";
            break;

          case RR_RRC_START_COSC_CNF:
            s = "RR_RRC_START_COSC_CNF";
            break;

          case RRC_RR_STOP_COSC_REQ:
            s = "RR_RRC_STOP_COSC_REQ";
            break;

          case RR_RRC_STOP_COSC_CNF:
            s = "RR_RRC_STOP_COSC_CNF";
            break;

          case RR_RRC_SERVICE_LOST_IND:
            s = "RR_RRC_SERVICE_LOST_IND";
            break;
#endif /* FEATURE_FEMTO_GSM_NL */

          case RRC_CLEAR_DEDICATED_PRIORITIES_IND:
            s = "RRC_CLEAR_DEDICATED_PRIORITIES_IND";
            break;

          case RR_CLEAR_DEDICATED_PRIORITIES_IND:
            s = "RR_CLEAR_DEDICATED_PRIORITIES_IND";
            break;

          case RR_DEDICATED_PRIORITIES_REQ:
            s = "RR_DEDICATED_PRIORITIES_REQ";
            break;

          case RR_DEDICATED_PRIORITIES_RSP:
            s = "RR_DEDICATED_PRIORITIES_RSP";
            break;

          case RRC_DEDICATED_PRIORITIES_REQ:
            s = "RRC_DEDICATED_PRIORITIES_REQ";
            break;

          case RRC_DEDICATED_PRIORITIES_RSP:
            s = "RRC_DEDICATED_PRIORITIES_RSP";
            break;

          case RRC_CIPHER_SYNC_IND:
            s = "RRC_CIPHER_SYNC_IND";
            break;

          default:
            MSG_GERAN_ERROR_1("Unknown message %d receive from RRC", message_id);
            s = "?";
        }
      } /* MS_RRC_RR */
      break;
#endif /* FEATURE_WCDMA */

      /* MM-RR messages */
      case MS_MM_RR:
      {
        switch (message_id)
        {
          /* MM -> RR */

          case RR_ABORT_REQ:
            s = "RR_ABORT_REQ";
            break;

#ifdef FEATURE_DUAL_SIM
          case RR_PS_ABORT_REQ:
            s = "RR_PS_ABORT_REQ";
            break;

          case RR_PS_RESUME_IND:
            s = "RR_PS_RESUME_IND";
            break;
#endif /* FEATURE_DUAL_SIM */

          case RR_DATA_REQ:
            s = "RR_DATA_REQ";
            break;

          case RR_DEACT_REQ:
            s = "RR_DEACT_REQ";
            break;

          case RR_EST_REQ:
            s = "RR_EST_REQ";
            break;

          case RR_PLMN_LIST_REQ:
            s = "RR_PLMN_LIST_REQ";
            break;

          case RR_PLMN_SELECT_REQ:
            s = "RR_PLMN_SELECT_REQ";
            break;

          case RR_FORBIDDEN_LAI_UPDATE_REQ:
            s = "RR_FORBIDDEN_LAI_UPDATE_REQ";
            break;

          case RR_SIM_INSERTED_REQ:
            s = "RR_SIM_INSERTED_REQ";
            break;

          case RR_INVALIDATE_SIM_DATA_REQ:
            s = "RR_INVALIDATE_SIM_DATA_REQ";
            break;

          case RR_STOP_GSM_MODE_REQ:
            s = "RR_STOP_GSM_MODE_REQ";
            break;

          case RR_SIM_UPDATE_REQ:
            s = "RR_SIM_UPDATE_REQ";
            break;

          case RR_ACT_REQ:
            s = "RR_ACT_REQ";
            break;

          case RR_EQ_PLMN_LIST_CHANGE_IND:
            s = "RR_EQ_PLMN_LIST_CHANGE_IND";
            break;

          case RR_ACTIVATION_RSP:
            s = "RR_ACTIVATION_RSP";
            break;

          case RR_GMM_GPRS_AUTH_FAIL_IND:
            s = "RR_GMM_GPRS_AUTH_FAIL_IND";
            break;

          case RR_GMM_GPRS_STATE_CHANGE_REQ:
            s = "RR_GMM_GPRS_STATE_CHANGE_REQ";
            break;

          case RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ:
            s = "RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ";
            break;

          case RR_GMM_READY_TIMER_STATE_REQ:
            s = "RR_GMM_READY_TIMER_STATE_REQ";
            break;

          case RR_GMM_GPRS_MS_RA_CAP_IE_IND:
            if (level)
            {
              (* level) = MSG_LEGACY_MED;
            }
            s = "RR_GMM_GPRS_MS_RA_CAP_IE_IND";
            break;

          case RR_GMM_START_MM_NON_DRX_IND:
            if (level)
            {
              (* level) = MSG_LEGACY_MED;
            }
            s = "RR_GMM_START_MM_NON_DRX_IND";
            break;

          case RR_GMM_STOP_MM_NON_DRX_IND:
            if (level)
            {
              (* level) = MSG_LEGACY_MED;
            }
            s = "RR_GMM_STOP_MM_NON_DRX_IND";
            break;

          case RR_GMM_GPRS_NV_PARAMS_IND:
            if (level)
            {
              (* level) = MSG_LEGACY_MED;
            }
            s = "RR_GMM_GPRS_NV_PARAMS_IND";
            break;

          case RR_GMM_GPRS_SERVICE_REQ:
            s = "RR_GMM_GPRS_SERVICE_REQ";
            break;

          case RR_CSFB_CALL_STATUS_IND:
            s = "RR_CSFB_CALL_STATUS_IND";
            break;

          case RR_CSG_WHITE_LIST_UPDATE_IND:
            s = "RR_CSG_WHITE_LIST_UPDATE_IND";
            break;

#ifdef FEATURE_GPRS_PS_HANDOVER
          case RR_PSHO_CNF:
            s = "RR_PSHO_CNF";
            break;
#endif /* FEATURE_GPRS_PS_HANDOVER */

          case RR_LAI_REJECT_LIST_IND:
            s = "RR_LAI_REJECT_LIST_IND";
            break;

#ifdef FEATURE_SGLTE
          case RR_UE_MODE_IND:
            s = "RR_UE_MODE_IND";
            break;
#endif /* FEATURE_SGLTE */

          case RR_SERVICE_STATUS_IND:
            s = "RR_SERVICE_STATUS_IND";
            break;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
          case RR_DS_STATUS_CHANGE_IND:
            s = "RR_DS_STATUS_CHANGE_IND";
            break;

          case RR_MULTIMODE_SUBS_CHGD_REQ:
            s = "RR_MULTIMODE_SUBS_CHGD_REQ";
            break;
#endif /* FEATURE_DUAL_SIM) || FEATURE_TRIPLE_SIM */

          /* RR -> MM */

          case RR_ABORT_CNF:
            s = "RR_ABORT_CNF";
            break;

          case RR_ABORT_IND:
            s = "RR_ABORT_IND";
            break;

          case RR_DATA_IND:
            s = "RR_DATA_IND";
            break;

#ifdef FEATURE_DUAL_SIM
          case RR_DS_DATA_IND:
            s = "RR_DS_DATA_IND";
            break;

          case RR_PS_ABORT_CNF:
            s = "RR_PS_ABORT_CNF";
            break;
#endif /* FEATURE_DUAL_SIM */

          case RR_DEACT_CNF:
            s = "RR_DEACT_CNF";
            break;

          case RR_EST_CNF:
            s = "RR_EST_CNF";
            break;

          case RR_PAGING_IND:
            s = "RR_PAGING_IND";
            break;

          case RR_PLMN_LIST_CNF:
            s = "RR_PLMN_LIST_CNF";
            break;

          case RR_PLMN_SELECT_CNF:
            s = "RR_PLMN_SELECT_CNF";
            break;

          case RR_REL_IND:
            s = "RR_REL_IND";
            break;

          case RR_SERVICE_IND:
            s = "RR_SERVICE_IND";
            break;

          case RR_SYNC_IND:
            s = "RR_SYNC_IND";
            break;

          case RR_STOP_GSM_MODE_CNF:
            s = "RR_STOP_GSM_MODE_CNF";
            break;

          case RR_ACTIVATION_IND:
            s = "RR_ACTIVATION_IND";
            break;

          case RR_GMM_GPRS_SUSPENSION:
            s = "RR_GMM_GPRS_SUSPENSION";
            break;

          case RR_GMM_GPRS_RESUMPTION_IND:
            s = "RR_GMM_GPRS_RESUMPTION_IND";
            break;

          case RR_RESELECTION_IND:
            s = "RR_RESELECTION_IND";
            break;

          case RR_GMM_GPRS_SERVICE_CNF:
            s = "RR_GMM_GPRS_SERVICE_CNF";
            break;

#ifdef FEATURE_INTERRAT_PCCO_GTOW
          case RR_GTOW_CCO_COMPLETE_IND:
            s = "RR_GTOW_CCO_COMPLETE_IND";
            break;
#endif /* FEATURE_INTERRAT_PCCO_GTOW */

#ifdef FEATURE_GSM_DTM
          case RR_SERVICE_IND_FROM_OTA_MSG:
            s = "RR_SERVICE_IND_FROM_OTA_MSG";
            break;
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GSM_EDTM
          case RR_EST_IND:
            s = "RR_EST_IND";
            break;
#endif /* FEATURE_GSM_EDTM */

          case RR_NW_SEL_MODE_RESET_IND:
            s = "RR_NW_SEL_MODE_RESET_IND";
            break;

          case RR_BPLMN_SEARCH_ABORT_REQ:
            s = "RR_BPLMN_SEARCH_ABORT_REQ";
            break;

          case RR_CHANGE_MODE_IND:
            s = "RR_CHANGE_MODE_IND";
            break;

#ifdef FEATURE_LTE
          case RR_LTE_RESELECTION_ALLOWED_STATUS_IND:
            s = "RR_LTE_RESELECTION_ALLOWED_STATUS_IND";
            break;
#endif /* FEATURE_LTE */

#ifdef FEATURE_GPRS_PS_HANDOVER
          case RR_PSHO_IND:
            s = "RR_PSHO_IND";
            break;
#endif /* FEATURE_GPRS_PS_HANDOVER */

#ifdef FEATURE_LTE
          case RR_CSFB_BLIND_REDIR_TO_LTE_REQ:
            s = "RR_CSFB_BLIND_REDIR_TO_LTE_REQ";
            break;
#endif /* FEATURE_LTE */

          case RR_GMM_GPRS_MULTISLOT_CLASS_IND:
            s = "RR_GMM_GPRS_MULTISLOT_CLASS_IND";
            break;
           
          case RR_SRVCC_HANDOVER_FAILURE_IND:
            s = "RR_SRVCC_HANDOVER_FAILURE_IND";
            break;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
          case RR_MULTIMODE_SUBS_CHGD_CNF:
            s = "RR_MULTIMODE_SUBS_CHGD_CNF";
            break;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

          case RR_GMM_GPRS_MS_RA_CAP_REQ:
            s = "RR_GMM_GPRS_MS_RA_CAP_REQ";
            break;

#ifdef FEATURE_GERAN_BSR_PS_STATUS
          case RR_PS_STATUS_CHANGE_IND:
            s = "RR_PS_STATUS_CHANGE_IND";
            break;
#endif

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q 
          case RR_PSEUDO_LTE_LIST_UPDATE_IND:
            s = "RR_PSEUDO_LTE_LIST_UPDATE_IND";
          break;
#endif
          
          case RR_MT_CSFB_FAILURE_IND:
            s = "RR_MT_CSFB_FAILURE_IND";
          break;
          default:
            s = "Unknown MM<->RR message";
        }
      } /* MS_MM_RR */
      break;

      /* L2-RR messages */

      case MS_RR_L2:
      {
        switch (message_id)
        {
          /* RR -> L2 */

          case DL_DATA_REQ:
            if (level)
            {
              *level = MSG_LEGACY_MED;
            }
            s = "DL_DATA_REQ";
            break;

          case DL_ESTABLISH_REQ:
            s = "DL_ESTABLISH_REQ";
            break;

          case DL_RANDOM_ACCESS_REQ:
            s = "DL_RANDOM_ACCESS_REQ";
            break;

          case DL_RECONNECT_REQ:
            s = "DL_RECONNECT_REQ";
            break;

          case DL_RELEASE_REQ:
            s = "DL_RELEASE_REQ";
            break;

          case DL_RESUME_REQ:
            s = "DL_RESUME_REQ";
            break;

          case DL_SUSPEND_REQ:
            s = "DL_SUSPEND_REQ";
            break;

          case DL_UNIT_DATA_REQ:
            if (level)
            {
              *level = MSG_LEGACY_LOW;
            }
            s = "DL_UNIT_DATA_REQ";
            break;

          case MDL_RELEASE_REQ:
            s = "MDL_RELEASE_REQ";
            break;

          /* L2 -> RR */

          case DL_DATA_IND:
            if (level)
            {
              *level = MSG_LEGACY_MED;
            }
            s = "DL_DATA_IND";
            break;

          case DL_DATA_ACK_IND:
            if (level)
            {
              *level = MSG_LEGACY_MED;
            }
            s = "DL_DATA_ACK_IND";
            break;

          case DL_ESTABLISH_CNF:
            s = "DL_ESTABLISH_CNF";
            break;

          case DL_ESTABLISH_IND:
            s = "DL_ESTABLISH_IND";
            break;

          case DL_RANDOM_ACCESS_CNF:
            s = "DL_RANDOM_ACCESS_CNF";
            break;

          case DL_RECONNECT_CNF:
            s = "DL_RECONNECT_CNF";
            break;

          case DL_RELEASE_CNF:
            s = "DL_RELEASE_CNF";
            break;

          case DL_RELEASE_IND:
            s = "DL_RELEASE_IND";
            break;

          case DL_RESUME_CNF:
            s = "DL_RESUME_CNF";
            break;

          case DL_SUSPEND_CNF:
            s = "DL_SUSPEND_CNF";
            break;

          case DL_UNIT_DATA_IND:
            if (level)
            {
              *level = MSG_LEGACY_LOW;
            }
            s = "DL_UNIT_DATA_IND";
            break;

          case MDL_ERROR_IND:
            s = "MDL_ERROR_IND";
            break;

          default:
            s = "Unknown L2<->RR message";
        }
      } /* MS_RR_L2 */
      break;

      /* L1-RR messages */

      case MS_RR_L1:
      {
        switch (message_id)
        {
          /* RR -> L1 */

          case MPH_CHANGE_PAGE_MODE_REQ:
            s = "MPH_CHANGE_PAGE_MODE_REQ";
            break;

          case MPH_CHANNEL_ASSIGN_REQ:
            s = "MPH_CHANNEL_ASSIGN_REQ";
            break;

          case MPH_CHANNEL_MODE_MODIFY_REQ:
            s = "MPH_CHANNEL_MODE_MODIFY_REQ";
            break;

          case MPH_CHANNEL_RELEASE_REQ:
            s = "MPH_CHANNEL_RELEASE_REQ";
            break;

          case MPH_DECODE_BCCH_LIST_ABORT_REQ:
            s = "MPH_DECODE_BCCH_LIST_ABORT_REQ";
            break;

          case MPH_DECODE_BCCH_LIST_REQ:
            s = "MPH_DECODE_BCCH_LIST_REQ";
            break;

          case MPH_DEDICATED_UPDATE_REQ:
            s = "MPH_DEDICATED_UPDATE_REQ";
            break;

          case MPH_POWER_SCAN_REQ:
            s = "MPH_POWER_SCAN_REQ";
            break;

          case MPH_FREQUENCY_REDEFINITION_REQ:
            s = "MPH_FREQUENCY_REDEFINITION_REQ";
            break;

          case MPH_HANDOVER_REQ:
            s = "MPH_HANDOVER_REQ";
            break;

          case MPH_IDLE_UPDATE_REQ:
            s = "MPH_IDLE_UPDATE_REQ";
            break;

          case MPH_IMMED_ASSIGN_REQ:
            s = "MPH_IMMED_ASSIGN_REQ";
            break;

          case MPH_READ_SYS_INFO_REQ:
            s = "MPH_READ_SYS_INFO_REQ";
            break;

          case MPH_RECONNECT_CHANNEL_REQ:
            s = "MPH_RECONNECT_CHANNEL_REQ";
            break;

          case MPH_SELECT_SPECIFIC_BCCH_REQ:
            s = "MPH_SELECT_SPECIFIC_BCCH_REQ";
            break;

          case MPH_SET_CIPHERING_MODE_REQ:
            s = "MPH_SET_CIPHERING_MODE_REQ";
            break;

          case MPH_START_IDLE_REQ:
            s = "MPH_START_IDLE_REQ";
            break;

          case MPH_STOP_HANDOVER_ACCESS_REQ:
            s = "MPH_STOP_HANDOVER_ACCESS_REQ";
            break;

          case MPH_WCDMA_CELL_UPDATE_LIST_REQ:
            s = "MPH_WCDMA_CELL_UPDATE_LIST_REQ";
            break;

          case MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ:
            s = "MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ";
            break;

          case MPH_START_GSM_MODE_REQ:
            s = "MPH_START_GSM_MODE_REQ";
            break;

          case MPH_STOP_GSM_MODE_REQ:
            s = "MPH_STOP_GSM_MODE_REQ";
            break;

          case MPH_ABORT_RA_REQ:
            s = "MPH_ABORT_RA_REQ";
            break;

          case MPH_POWER_SCAN_ABORT_REQ:
            s = "MPH_POWER_SCAN_ABORT_REQ";
            break;

          case MPH_EARLY_CAMPING_REQ:
            s = "MPH_EARLY_CAMPING_REQ";
            break;

          case MPH_IGNORE_WCDMA_NEIGHBORS_REQ:
            s = "MPH_IGNORE_WCDMA_NEIGHBORS_REQ";
            break;

          case MPH_UE_MODE_CHANGE_REQ:
            s = "MPH_UE_MODE_CHANGE_REQ";
            break;

          case MPH_RESET_REQ:
            s = "MPH_RESET_REQ";
            break;

          case MPH_START_GPRS_IDLE_MODE_REQ:
            s = "MPH_START_GPRS_IDLE_MODE_REQ";
            break;

          case MPH_GPRS_L1PARAM_UPDATE_REQ:
            s = "MPH_GPRS_L1PARAM_UPDATE_REQ";
            break;

          case MPH_NC_MEASUREMENTS_REQ:
            s = "MPH_NC_MEASUREMENTS_REQ";
            break;

          case MPH_STOP_NC_MEASUREMENTS_REQ:
            s = "MPH_STOP_NC_MEASUREMENTS_REQ";
            break;

          case MPH_GPRS_SURROUND_UPDATE_REQ:
            s = "MPH_GPRS_SURROUND_UPDATE_REQ";
            break;

          case MPH_STOP_SURROUND_UPDATE_REQ:
            s = "MPH_STOP_SURROUND_UPDATE_REQ";
            break;

          case MPH_GPRS_RESELECTION_REQ:
            s = "MPH_GPRS_RESELECTION_REQ";
            break;

          case MPH_START_MM_NON_DRX_IND:
            if (level)
            {
              (* level) = MSG_LEGACY_MED;
            }
            s = "MPH_START_MM_NON_DRX_IND";
            break;

          case MPH_STOP_MM_NON_DRX_IND:
            if (level)
            {
              (* level) = MSG_LEGACY_MED;
            }
            s = "MPH_STOP_MM_NON_DRX_IND";
            break;

          case MPH_INTERRAT_LIST_SRCH_TIME_REQ:
            s = "MPH_INTERRAT_LIST_SRCH_TIME_REQ";
            break;

          case MPH_SUSPEND_GSM_MODE_REQ:
            s = "MPH_SUSPEND_GSM_MODE_REQ";
            break;

          case MPH_RESUME_GSM_MODE_REQ:
            s = "MPH_RESUME_GSM_MODE_REQ";
            break;

          case MPH_START_GSM_MODE_FOR_BPLMN_REQ:
            s = "MPH_START_GSM_MODE_FOR_BPLMN_REQ";
            break;

          case MPH_RESUME_GSM_MODE_FOR_BPLMN_REQ:
            s = "MPH_RESUME_GSM_MODE_FOR_BPLMN_REQ";
            break;

#ifdef FEATURE_GSM_DTM
          case MPH_DTM_CHANNEL_RELEASE_REQ:
            s = "MPH_DTM_CHANNEL_RELEASE_REQ";
            break;

#ifdef FEATURE_GSM_EDTM
          case MPH_DTM_L1PARAM_UPDATE_REQ:
            s = "MPH_DTM_L1PARAM_UPDATE_REQ";
            break;
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
          case MPH_NV_REFRESH_REQ:
            s = "MPH_NV_REFRESH_REQ";
            break;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#ifdef FEATURE_DUAL_SIM
          case MPH_L1_DS_ABORT_IND:
            s = "MPH_L1_DS_ABORT_IND";
            break;
#endif /* FEATURE_DUAL_SIM */

          case MPH_RAT_CHANGE_IND:
            s = "MPH_RAT_CHANGE_IND";
            break;

          case MPH_ENABLE_SLEEP_IND:
            s = "MPH_ENABLE_SLEEP_IND";
            break;

          case MPH_SPECIFIC_IRAT_FREQ_UPDATE_REQ:
            s = "MPH_SPECIFIC_IRAT_FREQ_UPDATE_REQ";
            break;

          case MPH_DEEP_SLEEP_IND:
            s = "MPH_DEEP_SLEEP_IND";
            break;

#ifdef FEATURE_IDLE_DRX_SCALING
          case MPH_UPDATE_IDLE_DRX_IND:
            s = "MPH_UPDATE_IDLE_DRX_IND";
            break;
#endif /* FEATURE_IDLE_DRX_SCALING */

          /* L1 -> RR */

          case MPH_DECODE_BCCH_LIST_CNF:
            s = "MPH_DECODE_BCCH_LIST_CNF";
            break;

          case MPH_DECODE_BCCH_LIST_FAILURE_IND:
            s = "MPH_DECODE_BCCH_LIST_FAILURE_IND";
            break;

          case MPH_SURROUND_UPDATE_FAILURE_IND:
            s = "MPH_SURROUND_UPDATE_FAILURE_IND";
            break;

          case MPH_BLOCK_QUALITY_IND:
            if (level)
            {
              *level = MSG_LEGACY_LOW;
            }
            s = "MPH_BLOCK_QUALITY_IND";
            break;

          case MPH_CHANNEL_ASSIGN_CNF:
            s = "MPH_CHANNEL_ASSIGN_CNF";
            break;

          case MPH_CHANNEL_MODE_MODIFY_CNF:
            s = "MPH_CHANNEL_MODE_MODIFY_CNF";
            break;

          case MPH_CHANNEL_RELEASE_CNF:
            s = "MPH_CHANNEL_RELEASE_CNF";
            break;

          case MPH_POWER_SCAN_CNF:
            s = "MPH_POWER_SCAN_CNF";
            break;

          case MPH_FREQUENCY_REDEFINITION_IND:
            s = "MPH_FREQUENCY_REDEFINITION_IND";
            break;

          case MPH_HANDOVER_CNF:
            s = "MPH_HANDOVER_CNF";
            break;

          case MPH_HANDOVER_IND:
            s = "MPH_HANDOVER_IND";
            break;

          case MPH_IMMED_ASSIGN_CNF:
            s = "MPH_IMMED_ASSIGN_CNF";
            break;

          case MPH_RECONNECT_CHANNEL_CNF:
            s = "MPH_RECONNECT_CHANNEL_CNF";
            break;

          case MPH_SELECT_SPECIFIC_BCCH_CNF:
            if (msg_ptr != NULL)
            {
              if (msg_ptr->mph_select_specific_bcch_cnf.BCCH_found)
              {
                s = "MPH_SELECT_SPECIFIC_BCCH_CNF(BCCH_found = TRUE)";
              }
              else
              {
                s = "MPH_SELECT_SPECIFIC_BCCH_CNF(BCCH_found = FALSE)";
              }
            }
            else
            {
              s = "MPH_SELECT_SPECIFIC_BCCH_CNF";
            }
            break;

          case MPH_SERVING_DED_MEAS_IND:
            if (level)
            {
              *level = MSG_LEGACY_LOW;
            }
            s = "MPH_SERVING_DED_MEAS_IND";
            break;

          case MPH_SERVING_IDLE_MEAS_IND:
            if (level)
            {
              *level = MSG_LEGACY_LOW;
            }
            s = "MPH_SERVING_IDLE_MEAS_IND";
            break;

          case MPH_SET_CIPHERING_CNF:
            s = "MPH_SET_CIPHERING_CNF";
            break;

          case MPH_STOP_HANDOVER_ACCESS_CNF:
            s = "MPH_STOP_HANDOVER_ACCESS_CNF";
            break;

          case MPH_SURROUND_MEAS_IND:
            if (level)
            {
              *level = MSG_LEGACY_LOW;
            }
            s = "MPH_SURROUND_MEAS_IND";
            break;

#ifdef FEATURE_WCDMA
          case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
            s = "MPH_SURROUND_WCDMA_IDLE_MEAS_IND";
            break;

          case MPH_SURROUND_WCDMA_DEDICATED_MEAS_IND:
            s = "MPH_SURROUND_WCDMA_DEDICATED_MEAS_IND";
            break;
#endif /* FEATURE_WCDMA */

          case MPH_SYS_INFO_1_ABSENT_IND:
            s = "MPH_SYS_INFO_1_ABSENT_IND";
            break;

         case MPH_START_GSM_MODE_CNF:
            s = "MPH_START_GSM_MODE_CNF";
            break;

         case MPH_STOP_GSM_MODE_CNF:
            s = "MPH_STOP_GSM_MODE_CNF";
            break;

          case MPH_RESELECTION_CNF:
            if (msg_ptr != NULL)
            {
              if (msg_ptr->mph_reselection_cnf.BCCH_found)
              {
                s = "MPH_RESELECTION_CNF(BCCH_found = TRUE)";
              }
              else
              {
                s = "MPH_RESELECTION_CNF(BCCH_found = FALSE)";
              }
            }
            else
            {
              s = "MPH_RESELECTION_CNF";
            }
            break;

          case MPH_START_IDLE_MODE_CNF:
            s = "MPH_START_IDLE_MODE_CNF";
            break;

          case MPH_ABORT_RA_CNF:
            s = "MPH_ABORT_RA_CNF";
            break;

          case MPH_RESET_IND:
            s = "MPH_RESET_IND";
            break;

          case MPH_RESET_CNF:
            s = "MPH_RESET_CNF";
            break;


          case MPH_SERVING_AUX_MEAS_IND:
            s = "MPH_SERVING_AUX_MEAS_IND";
            break;


          case MPH_NC_MEASUREMENT_IND:
            if (level)
            {
              (* level) = MSG_LEGACY_MED;
            }
            s = "MPH_NC_MEASUREMENT_IND";
            break;

          case MPH_SERVING_MEAS_IND:
            if (level)
            {
              (* level) = MSG_LEGACY_LOW;
            }
            s = "MPH_SERVING_MEAS_IND";
            break;

          case MPH_SURROUND_WCDMA_XFER_MEAS_IND:
            s = "MPH_SURROUND_WCDMA_XFER_MEAS_IND";
            break;


          case MPH_L1_TEST_MODE_STOP_IND:
            s = "MPH_L1_TEST_MODE_STOP_IND";
            break;

          case MPH_L1_TEST_MODE_START_IND:
            s = "MPH_L1_TEST_MODE_START_IND";
            break;


          case MPH_READ_SYS_INFO_FAILURE_IND:
            s = "MPH_READ_SYS_INFO_FAILURE_IND";
            break;

          case MPH_FCCH_SCH_DECODE_IND:
            s = "MPH_FCCH_SCH_DECODE_IND";
            break;

          case MPH_START_GSM_MODE_FOR_BPLMN_CNF:
            s = "MPH_START_GSM_MODE_FOR_BPLMN_CNF";
            break;

          case MPH_RESUME_GSM_MODE_FOR_BPLMN_CNF:
            s = "MPH_RESUME_GSM_MODE_FOR_BPLMN_CNF";
            break;

          case MPH_SUSPEND_GSM_MODE_IND:
            s = "MPH_SUSPEND_GSM_MODE_IND";
            break;

          case MPH_INTERRAT_LIST_SRCH_TIME_IND:
            s = "MPH_INTERRAT_LIST_SRCH_TIME_IND";
            break;

          case MPH_SUSPEND_GSM_MODE_CNF:
            s = "MPH_SUSPEND_GSM_MODE_CNF";
            break;

          case MPH_RESUME_GSM_MODE_CNF:
            s = "MPH_RESUME_GSM_MODE_CNF";
            break;

#ifdef FEATURE_GSM_DTM

          case MPH_DTM_CHANNEL_RELEASE_IND:
            s = "MPH_DTM_CHANNEL_RELEASE_IND";
            break;

          case MPH_DTM_CHANNEL_ASSIGN_CNF:
            s = "MPH_DTM_CHANNEL_ASSIGN_CNF";
            break;

#endif /* FEATURE_GSM_DTM */

#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
          case MPH_SURROUND_IRAT_MEAS_IND:
            s = "MPH_SURROUND_IRAT_MEAS_IND";
            break;
#endif /* FEATURE_LTE || FEATURE_GSM_TDS */

#ifdef FEATURE_SGLTE
          case MPH_UE_MODE_CHANGE_CNF:
            s = "MPH_UE_MODE_CHANGE_CNF";
            break;
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
          case MPH_NV_REFRESH_CNF:
            s = "MPH_NV_REFRESH_CNF";
            break;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

          case MPH_DEVICE_MODE_IND:
            s = "MPH_DEVICE_MODE_IND";
            break;

          case MPH_SET_PRIORITY_IND:
            s = "MPH_SET_PRIORITY_IND";
            break;

          case MPH_DSC_THRESHOLD_IND:
            s = "MPH_DSC_THRESHOLD_IND";
            break;

#ifdef FEATURE_DUAL_SIM
          case MPH_COEX_MSC_CHANGE_IND:
            s = "MPH_COEX_MSC_CHANGE_IND";
            break;
#endif /* FEATURE_DUAL_SIM */

          default:
            s = "Unknown L1<->RR message";
        }
      } /* MS_RR_L1 */
      break;

#ifdef FEATURE_CGPS_UMTS_CP_GSM
      /* GPS-RR messages */

      case MS_CGPS_RR:
      {
        switch(message_id)
        {
          case RR_APDU_REQ:
            s = "RR_APDU_REQ";
            break;

          case RR_CGPS_EVENT_REQ:
            s = "RR_CGPS_EVENT_REQ";
            break;

          default:
            s = "Unknown GPS<->RR message";
        }
      } /* MS_CGPS_RR */
      break;
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

      /* MAC-RR messages */

      case MS_MAC_RR:
      {
        switch ( message_id )
        {
          /* GRR -> MAC */

          case GRR_MAC_SEND_MEAS_RPT:
            s = "GRR_MAC_SEND_MEAS_RPT";
            break;

          case GRR_MAC_SEND_PCCF:
            s = "GRR_MAC_SEND_PCCF";
            break;

          case GRR_MAC_DL_TBF_EST:
            s = "GRR_MAC_DL_TBF_EST";
            break;

          case GRR_MAC_DL_SB_EST:
            s = "GRR_MAC_DL_SB_EST";
            break;

          case GRR_MAC_UL_TBF_EST:
            s = "GRR_MAC_UL_TBF_EST";
            break;

          case GRR_MAC_UL_SB_EST:
            s = "GRR_MAC_UL_SB_EST";
            break;

          case GRR_MAC_RACH_REQ:
            if (level)
            {
              (* level) = MSG_LEGACY_LOW;
            }
            s = "GRR_MAC_RACH_REQ";
            break;


          case GRR_MAC_SEND_PCCN_REQ:
            s = "GRR_MAC_SEND_PCCN_REQ";
            break;

          case GRR_MAC_NACC_RELEASE_REQ:
            s = "GRR_MAC_NACC_RELEASE_REQ";
            break;

#ifdef FEATURE_GPRS_PS_HANDOVER
          case GRR_MAC_PSHO_REQ:
            s = "GRR_MAC_PSHO_REQ";
            break;

          case GRR_MAC_PSHO_COMPLETE_IND:
            s = "GRR_MAC_PSHO_COMPLETE_IND";
            break;
#endif /* FEATURE_GPRS_PS_HANDOVER */


          case GRR_MAC_SEND_PSS_REQ:
            s = "GRR_MAC_SEND_PSS_REQ";
            break;

#ifdef FEATURE_GSM_DTM

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            s = "GRR_MAC_DTM_CHANNEL_ASSIGN_REQ";
            break;

          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            s = "GRR_MAC_DTM_CHANNEL_RELEASE_IND";
            break;

          case GRR_MAC_DTM_UL_DCCH_CNF:
            s = "GRR_MAC_DTM_UL_DCCH_CNF";
            break;

          case GRR_MAC_DTM_UL_DCCH_IND:
            s = "GRR_MAC_DTM_UL_DCCH_IND";
            break;

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            s = "GRR_MAC_EDTM_PKT_CS_EST_REQ";
            break;
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */


          case GRR_MAC_RESET_REQ:
            s = "GRR_MAC_RESET_REQ";
            break;


          case GRR_MAC_PS_ACCESS_REQ:
            s = "GRR_MAC_PS_ACCESS_REQ";
            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            s = "GRR_MAC_NO_PS_ACCESS_IND";
            break;

          /* MAC -> GRR */

          case MAC_GRR_DATA_IND:
            if (level)
            {
              (* level) = MSG_LEGACY_MED;
            }
            s = "MAC_GRR_DATA_IND";
            break;

          case MAC_GRR_CELL_RESEL_STATUS:
            s = "MAC_GRR_CELL_RESEL_STATUS";
            break;

          case MAC_GRR_MAC_STATUS:
            s = "MAC_GRR_MAC_STATUS";
            break;

          case MAC_GRR_PAGING_NOTIFICATION:
            s = "MAC_GRR_PAGING_NOTIFICATION";
            break;

          case MAC_GRR_ABNORMAL_RELEASE:
            s = "MAC_GRR_ABNORMAL_RELEASE";
            break;

          case MAC_GRR_MEAS_RPT_CNF:
            s = "MAC_GRR_MEAS_RPT_CNF";
            break;

          case MAC_GRR_PCCF_SENT:
            s = "MAC_GRR_PCCF_SENT";
            break;

          case MAC_GRR_TBF_REJ:
            s = "MAC_GRR_TBF_REJ";
            break;

          case MAC_GRR_TBF_ACCEPT:
            s = "MAC_GRR_TBF_ACCEPT";
            break;

          case MAC_GRR_UL_TBF_REQ:
            s = "MAC_GRR_UL_TBF_REQ";
            break;

          case MAC_GRR_STORE_UPDATE:
            s = "MAC_GRR_STORE_UPDATE";
            break;

          case MAC_GRR_RACH_CNF:
            if (level)
            {
              (* level) = MSG_LEGACY_LOW;
            }
            s = "MAC_GRR_RACH_CNF";
            break;

          case MAC_GRR_RACH_REJ:
            if (level)
            {
              (* level) = MSG_LEGACY_MED;
            }
            s = "MAC_GRR_RACH_REJ";
            break;


          case MAC_GRR_NCELL_DATA_IND:
            if (level)
            {
              (* level) = MSG_LEGACY_MED;
            }
            s = "MAC_GRR_NCELL_DATA_IND";
            break;

          case MAC_GRR_NCELL_DATA_CNF:
            if (level)
            {
              (* level) = MSG_LEGACY_MED;
            }
            s = "MAC_GRR_NCELL_DATA_CNF";
            break;

#ifdef FEATURE_GPRS_PS_HANDOVER
          case MAC_GRR_PSHO_CNF:
            s = "MAC_GRR_PSHO_CNF";
            break;
#endif /* FEATURE_GPRS_PS_HANDOVER */


#ifdef FEATURE_GSM_DTM

          case MAC_GRR_DTM_CHANNEL_ASSIGN_REJ:
            s = "MAC_GRR_DTM_CHANNEL_ASSIGN_REJ";
            break;

#ifdef FEATURE_GSM_EDTM

          case MAC_GRR_EDTM_PKT_CS_EST_REJ:
            s = "MAC_GRR_EDTM_PKT_CS_EST_REJ";
            break;

          case MAC_GRR_EDTM_CS_RELEASE_IND:
            s = "MAC_GRR_EDTM_CS_RELEASE_IND";
            break;

#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */


          case MAC_GRR_RESET_CNF:
            s = "MAC_GRR_RESET_CNF";
            break;


          case MAC_GRR_PS_ACCESS_CNF:
            s = "MAC_GRR_PS_ACCESS_CNF";
            break;

          default:
            s = "Unknown MAC<->RR message";
        }
      }
      break;

#ifdef FEATURE_GSM_DTM

      /* RLC-RR messages */

      case MS_RLC_RR:
      {
        switch(message_id)
        {
          case RG_UL_DCCH_REQ:
          {
            s = "RG_UL_DCCH_REQ";
            break;
          }

          default:
            s = "Unknown RLC<->RR message";
        }
      } /* MS_RLC_RR */
      break;

#endif /* FEATURE_GSM_DTM */

      /* Timers */

      case MS_TIMER:
      {
        timer_expiry_T *timer_expiry_ptr;
        rr_timer_id_T timer_id = RR_TIMER_UNDEFINED;

        timer_expiry_ptr = (timer_expiry_T *) msg_ptr;

        if (timer_expiry_ptr != NULL)
        {
          timer_id = (rr_timer_id_T) timer_expiry_ptr->timer_id;
        }

        s = rr_timer_name(timer_id);
      } /* MS_TIMER */
      break;

      case MS_GHDI_EXPORT:
      {
        switch(message_id)
        {
          case GHDI_CLASSMARK_CHANGED_IND:
            s = "GHDI_CLASSMARK_CHANGED_IND";
            break;

          default:
            s = "Unknown GHDI<->RR message";
            break;
        }
      }
      break;

      case MS_RR_RR:
      case MS_RR_RR_EXT:
      {
        if (msg_ptr != NULL)
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_IMSG:
            {
              s = rr_imsg_name((rr_imsg_e) msg_ptr->rr.grr.imsg.header.imh.message_id);
              break;
            } /* RR_IMSG */

            case RR_GRR_IMSG:
            {
              s = rr_grr_imsg_name((rr_grr_imsg_e) msg_ptr->rr.grr.imsg.header.imh.message_id);
              break;
            }

            case RR_GRR_OMSG:
            {
              s = rr_grr_omsg_name((rr_grr_omsg_e) msg_ptr->rr.grr.omsg.header.imh.message_id);
              break;
            }

            case RR_RESEL_IMSG:
            {
              s = rr_resel_imsg_name((rr_resel_imsg_e) msg_ptr->rr.resel.imsg.header.imh.message_id);
              break;
            }

            case RR_RESEL_OMSG:
            {
              s = rr_resel_omsg_name((rr_resel_omsg_e) msg_ptr->rr.resel.omsg.header.imh.message_id);
              break;
            }

            case RR_DECODE_BCCH_IMSG:
            {
              switch (msg_ptr->rr.decode_bcch.imsg.header.imh.message_id)
              {
                case RR_DECODE_BCCH_IMSG_START_IND:
                {
                  s = "RR_DECODE_BCCH_IMSG_START_IND";
                  break;
                }

                case RR_DECODE_BCCH_IMSG_ABORT_REQ:
                {
                  s = "RR_DECODE_BCCH_IMSG_ABORT_REQ";
                  break;
                }

                case RR_DECODE_BCCH_IMSG_CANDIDATE_CELL_SI_IND:
                {
                  s = "RR_DECODE_BCCH_IMSG_CANDIDATE_CELL_SI_IND";
                  break;
                }

                default:
                {
                  MSG_GERAN_ERROR_1(
                    "Unknown RR_DECODE_BCCH_IMSG: 0x%02x",
                    msg_ptr->rr.decode_bcch.imsg.header.imh.message_id
                  );
                  s = "?";
                }
              }
              break;
            }

            case RR_DECODE_BCCH_OMSG:
            {
              switch (msg_ptr->rr.decode_bcch.omsg.header.imh.message_id)
              {
                case RR_DECODE_BCCH_OMSG_FINISHED_IND:
                {
                  s = "RR_DECODE_BCCH_OMSG_FINISHED_IND";
                  break;
                }

                case RR_DECODE_BCCH_OMSG_ABORT_CNF:
                {
                  s = "RR_DECODE_BCCH_OMSG_ABORT_CNF";
                  break;
                }

                default:
                {
                  MSG_GERAN_ERROR_1(
                    "Unknown RR_DECODE_BCCH_OMSG: 0x%02x",
                    msg_ptr->rr.decode_bcch.omsg.header.imh.message_id
                  );
                  s = "?";
                }
              }
              break;
            }

            case RR_PSCAN_IMSG:
            {
              switch (msg_ptr->rr.pscan.imsg.header.imh.message_id)
              {
                case RR_PSCAN_IMSG_START_IND:
                {
                  s = "RR_PSCAN_IMSG_START_IND";
                  break;
                }

                case RR_PSCAN_IMSG_ABORT_REQ:
                {
                  s = "RR_PSCAN_IMSG_ABORT_REQ";
                  break;
                }

                default:
                {
                  MSG_GERAN_ERROR_1(
                    "Unknown RR_PSCAN_IMSG: 0x%02x",
                    msg_ptr->rr.pscan.imsg.header.imh.message_id
                  );
                  s = "?";
                }
              }
              break;
            }

            case RR_PSCAN_OMSG:
            {
              switch (msg_ptr->rr.pscan.omsg.header.imh.message_id)
              {
                case RR_PSCAN_OMSG_FINISHED_IND:
                {
                  s = "RR_PSCAN_OMSG_FINISHED_IND";
                  break;
                }

                case RR_PSCAN_OMSG_ABORT_CNF:
                {
                  s = "RR_PSCAN_OMSG_ABORT_CNF";
                  break;
                }

                default:
                {
                  MSG_GERAN_ERROR_1(
                    "Unknown RR_PSCAN_OMSG: 0x%02x",
                    msg_ptr->rr.pscan.omsg.header.imh.message_id
                  );
                  s = "?";
                }
              }
              break;
            }



            case RR_INTER_HO_IMSG:
            {
              switch (msg_ptr->rr.inter_ho.imsg.header.imh.message_id)
              {
                default:
                {
                  MSG_GERAN_ERROR_1(
                    "Unknown RR_INTER_HO_IMSG: 0x%02x",
                    msg_ptr->rr.inter_ho.imsg.header.imh.message_id
                  );
                  s = "?";
                }
              }
              break;
            }

            case RR_INTER_HO_OMSG:
            {
              switch (msg_ptr->rr.inter_ho.omsg.header.imh.message_id)
              {
                default:
                {
                  MSG_GERAN_ERROR_1(
                    "Unknown RR_INTER_HO_OMSG: 0x%02x",
                    msg_ptr->rr.inter_ho.omsg.header.imh.message_id
                  );
                  s = "?";
                }
              }
              break;
            }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
            case RR_RESEL_G2W_IMSG:
            {
              switch (msg_ptr->rr.resel_g2w.omsg.header.imh.message_id)
              {
                case RR_RESEL_G2W_IMSG_RESEL_REQ:
                {
                  s = "RR_RESEL_G2W_IMSG_RESEL_REQ";
                  break;
                }

                case RR_RESEL_G2W_IMSG_L1_DEACTIVATED_IND:
                {
                  s = "RR_RESEL_G2W_IMSG_L1_DEACTIVATED_IND";
                  break;
                }

                case RR_RESEL_G2W_IMSG_L1_ACTIVATED_IND:
                {
                  s = "RR_RESEL_G2W_IMSG_L1_ACTIVATED_IND";
                  break;
                }

                case RR_RESEL_G2W_IMSG_ABORT_IND:
                {
                  s = "RR_RESEL_G2W_IMSG_ABORT_IND";
                  break;
                }

#ifdef FEATURE_SGLTE
                case RR_RESEL_G2W_IMSG_STOP_GSM_MODE_REQ_SGLTE_RAT_CHANGE:
                {
                  s = "RR_RESEL_G2W_IMSG_STOP_GSM_MODE_REQ_SGLTE_RAT_CHANGE";
                  break;
                }
#endif /* FEATURE_SGLTE */

                default:
                {
                  MSG_GERAN_ERROR_1(
                    "Unknown RR_RESEL_G2W_IMSG: 0x%02x",
                    msg_ptr->rr.resel_g2w.omsg.header.imh.message_id
                  );
                  s = "?";
                }
              }
              break;
            }

            case RR_RESEL_G2W_OMSG:
            {
              switch (msg_ptr->rr.resel_g2w.omsg.header.imh.message_id)
              {
                case RR_RESEL_G2W_OMSG_RESEL_CNF:
                {
                  switch (msg_ptr->rr.resel_g2w.omsg.resel_cnf.status)
                  {
                    case RR_RESEL_G2W_STATUS_UNDEFINED:
                      s = "RR_RESEL_G2W_OMSG_RESEL_CNF(UNDEFINED)";
                      break;

                    case RR_RESEL_G2W_STATUS_RESELECTION_SUCCESS:
                      s = "RR_RESEL_G2W_OMSG_RESEL_CNF(RESELECTION_SUCCESS)";
                      break;

                    case RR_RESEL_G2W_STATUS_RESELECTION_FAILURE:
                      s = "RR_RESEL_G2W_OMSG_RESEL_CNF(RESELECTION_FAILURE)";
                      break;

                    case RR_RESEL_G2W_STATUS_PCCO_SUCCESS:
                      s = "RR_RESEL_G2W_OMSG_RESEL_CNF(PCCO_SUCCESS)";
                      break;

                    case RR_RESEL_G2W_STATUS_PCCO_FAILURE_NO_RESPONSE:
                      s = "RR_RESEL_G2W_OMSG_RESEL_CNF(PCCO_FAILURE_NO_RESPONSE)";
                      break;

                    case RR_RESEL_G2W_STATUS_PCCO_FAILURE_ASSIGNMENT_REJECT:
                      s = "RR_RESEL_G2W_OMSG_RESEL_CNF(PCCO_FAILURE_ASSIGNMENT_REJECT)";
                      break;

                    case RR_RESEL_G2W_STATUS_ABORTED_IN_SERVICE:
                      s = "RR_RESEL_G2W_OMSG_RESEL_CNF(ABORTED_IN_SERVICE";
                      break;

                    case RR_RESEL_G2W_STATUS_ABORTED_OUT_OF_SERVICE:
                      s = "RR_RESEL_G2W_OMSG_RESEL_CNF(ABORTED_OUT_OF_SERVICE)";
                      break;

                    case RR_RESEL_G2W_STATUS_MAX:
                      s = "RR_RESEL_G2W_OMSG_RESEL_CNF(MAX)";
                      break;
                  }
                  break;
                } /* RR_RESEL_G2W_OMSG_RESEL_CNF */

                default:
                {
                  MSG_GERAN_ERROR_1(
                    "Unknown RR_RESEL_G2W_OMSG: 0x%02x",
                    msg_ptr->rr.resel_g2w.omsg.header.imh.message_id
                  );
                  s = "?";
                }
              }
              break;
            }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

            case RR_CELL_ACQ_IMSG:
            {
              switch (msg_ptr->rr.cell_acq.imsg.header.imh.message_id)
              {
                case RR_CELL_ACQ_IMSG_START_IND:
                {
                  s = "RR_CELL_ACQ_IMSG_START_IND";
                  break;
                }

                case RR_CELL_ACQ_IMSG_ABORT_REQ:
                {
                  s = "RR_CELL_ACQ_IMSG_ABORT_REQ";
                  break;
                }

                case RR_CELL_ACQ_IMSG_SURROUNDING_SI_RECEIVED_IND:
                {
                  s = "RR_CELL_ACQ_IMSG_SURROUNDING_SI_RECEIVED_IND";
                  break;
                }

                case RR_CELL_ACQ_IMSG_EARLY_CAMPING_BSI_RECEIVED_IND:
                {
                  s = "RR_CELL_ACQ_IMSG_EARLY_CAMPING_BSI_RECEIVED_IND";
                  break;
                }

                case RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND:
                {
                  s = "RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND";
                  break;
                }

                case RR_CELL_ACQ_IMSG_GPRS_BSI_RECEIVED_IND:
                {
                  s = "RR_CELL_ACQ_IMSG_GPRS_BSI_RECEIVED_IND";
                  break;
                }

                case RR_CELL_ACQ_IMSG_NACC_ACQ_COMPLETE_IND:
                {
                  s = "RR_CELL_ACQ_IMSG_NACC_ACQ_COMPLETE_IND";
                  break;
                }

                default:
                {
                  MSG_GERAN_ERROR_1(
                    "Unknown RR_CELL_ACQ_IMSG: 0x%02x",
                    msg_ptr->rr.cell_acq.imsg.header.imh.message_id
                  );
                  s = "?";
                }

              }

              break;
            }

            case RR_CELL_ACQ_OMSG:
            {
              switch (msg_ptr->rr.cell_acq.omsg.header.imh.message_id)
              {
                case RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND:
                {
                  s = "RR_CELL_ACQ_MAC_SUSPENDED_IND";
                  break;
                }

                case RR_CELL_ACQ_OMSG_RESELECTED_IND:
                {
                  s = "RR_CELL_ACQ_RESELECTED_IND";
                  break;
                }

                case RR_CELL_ACQ_OMSG_FINISHED_IND:
                {
                  s = "RR_CELL_ACQ_FINISHED_IND";
                  break;
                }

                case RR_CELL_ACQ_OMSG_ABORT_CNF:
                {
                  s = "RR_CELL_ACQ_ABORT_CNF";
                  break;
                }

                default:
                {
                  MSG_GERAN_ERROR_1(
                    "Unknown RR_CELL_ACQ_OMSG: 0x%02x",
                    msg_ptr->rr.cell_acq.omsg.header.imh.message_id
                  );
                  s = "?";
                }

              }

              break;
            }

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

            case RR_X2G_RESEL_IMSG:
            {
              switch (msg_ptr->rr.x2g_resel.imsg.header.imh.message_id)
              {
                case RR_X2G_RESEL_IMSG_RESEL_REQ:
                {
                  s = "RR_X2G_RESEL_IMSG_RESEL_REQ";
                  break;
                }

                case RR_X2G_RESEL_IMSG_ABORT_REQ:
                {
                  s = "RR_X2G_RESEL_IMSG_ABORT_REQ";
                  break;
                }

                case RR_X2G_RESEL_IMSG_L1_DEACTIVATED_IND:
                {
                  s = "RR_X2G_RESEL_IMSG_L1_DEACTIVATED_IND";
                  break;
                }

                case RR_X2G_RESEL_IMSG_L1_ACTIVATED_IND:
                {
                  s = "RR_X2G_RESEL_IMSG_L1_ACTIVATED_IND";
                  break;
                }

                case RR_X2G_RESEL_IMSG_PANIC_RESET_REQ:
                {
                  s = "RR_X2G_RESEL_IMSG_PANIC_RESET_REQ";
                  break;
                }

                default:
                {
                  MSG_GERAN_ERROR_1(
                    "Unknown RR_X2G_RESEL_IMSG: 0x%02x",
                    msg_ptr->rr.x2g_resel.imsg.header.imh.message_id
                  );
                  s = "?";
                }
              }
              break;
            } /* RR_X2G_RESEL_IMSG */

            case RR_X2G_RESEL_OMSG:
            {
              switch (msg_ptr->rr.x2g_resel.omsg.header.imh.message_id)
              {
                case RR_X2G_RESEL_OMSG_RESEL_CNF:
                {
                  s = "RR_X2G_RESEL_OMSG_RESEL_CNF";
                  break;
                }

                case RR_X2G_RESEL_OMSG_ABORT_CNF:
                {
                  s = "RR_X2G_RESEL_OMSG_ABORT_CNF";
                  break;
                }

                case RR_X2G_RESEL_OMSG_PANIC_RESET_CNF:
                {
                  if (msg_ptr->rr.x2g_resel.omsg.panic_reset_cnf.activation_ind_sent)
                  {
                    s = "RR_X2G_RESEL_OMSG_PANIC_RESET_CNF(activation_ind_sent => 1)";
                  }
                  else
                  {
                    s = "RR_X2G_RESEL_OMSG_PANIC_RESET_CNF(activation_ind_sent => 0)";
                  }
                  break;
                }

                default:
                {
                  MSG_GERAN_ERROR_1(
                    "Unknown RR_X2G_RESEL_OMSG: 0x%02x",
                    msg_ptr->rr.x2g_resel.omsg.header.imh.message_id
                  );
                  s = "?";
                }
              }
              break;
            } /* RR_X2G_RESEL_OMSG */

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

            case RR_SELECT_BCCH_IMSG:
            case RR_SELECT_BCCH_OMSG:
            {
              s = rr_select_bcch_message_name(msg_ptr);
              break;
            }

#ifdef FEATURE_FEMTO_GSM_NL
            case RR_FEMTO_IMSG:
            case RR_FEMTO_OMSG:
            {
              s = rr_femto_message_name(msg_ptr,message_set,message_id);
              break;
            }
#endif /* FEATURE_FEMTO_GSM_NL */

            case RR_CE_IMSG:
            {
              s = rr_ce_imsg_name((rr_ce_imsg_e) message_id);
              break;
            } /* RR_CE_IMSG */

            case RR_CE_OMSG:
            {
              s = rr_ce_omsg_name((rr_ce_omsg_e) message_id);
              break;
            } /* RR_CE_OMSG */

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE)
            case RR_G2X_REDIR_IMSG:
            {
              s = rr_g2x_redir_imsg_name((rr_g2x_redir_imsg_e) message_id);
              break;
            } /* RR_G2X_REDIR_IMSG */

            case RR_G2X_REDIR_OMSG:
            {
              s = rr_g2x_redir_omsg_name((rr_g2x_redir_omsg_e) message_id);
              break;
            } /* RR_G2X_REDIR_OMSG */
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE */

#ifdef FEATURE_LTE_TO_GSM_CGI
            case RR_X2G_CGI_IMSG:
            {
              s = rr_x2g_cgi_imsg_name((rr_x2g_cgi_imsg_e) message_id);
              break;
            }

            case RR_X2G_CGI_OMSG:
            {
              s = rr_x2g_cgi_omsg_name((rr_x2g_cgi_omsg_e) message_id);
              break;
            }
#endif /* FEATURE_LTE_TO_GSM_CGI */

            case RR_L1_IDLE_IMSG:
            {
              s = rr_l1_idle_imsg_name((rr_l1_idle_imsg_e) message_id);
              break;
            }

            case RR_L1_IDLE_OMSG:
            {
              s = rr_l1_idle_omsg_name((rr_l1_idle_omsg_e) message_id);
              break;
            }

            case RR_PS_IMSG:
            {
              s = rr_ps_imsg_name((rr_ps_imsg_e) message_id);
              break;
            }

            case RR_PS_OMSG:
            {
              s = rr_ps_omsg_name((rr_ps_omsg_e) message_id);
              break;
            }

            case RR_DTM_IMSG:
            {
              s = rr_dtm_imsg_name((rr_dtm_imsg_e) message_id);
              break;
            }

            case RR_DTM_OMSG:
            {
              s = rr_dtm_omsg_name((rr_dtm_omsg_e) message_id);
              break;
            }

#ifdef FEATURE_SGLTE
            case RR_PLMN_LIST_IMSG:
            {
              s = rr_plmn_list_imsg_name((rr_sglte_band_coex_imsg_e) message_id);
              break;
            }

            case RR_PLMN_LIST_OMSG:
            {
              s = rr_plmn_list_omsg_name((rr_sglte_band_coex_omsg_e) message_id);
              break;
            }

            case RR_SGLTE_X2G_IMSG:
            {
              s = rr_sglte_x2g_imsg_name((rr_sglte_x2g_imsg_e) message_id);
              break;
            }

            case RR_SGLTE_X2G_OMSG:
            {
              s = rr_sglte_x2g_omsg_name((rr_sglte_x2g_omsg_e) message_id);
              break;
            }
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_GSM_BAND_AVOIDANCE
            case RR_BAND_AVOID_IMSG:
            {
              s = rr_band_avoid_imsg_name((rr_band_avoid_imsg_e) message_id);
              break;
            }
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

            default:
            {
              MSG_GERAN_ERROR_3(
                "Unknown RR message set: %d, message ID: 0x%02x",
                msg_ptr->rr.header.rr_message_set,
                msg_ptr->rr.header.imh.message_id,
                0
              );
              s = "?";
            }
          }
        }
        else
        {
          MSG_GERAN_ERROR_0("msg_ptr == NULL");
        }
        break;
      }

      case MS_MSGR_RR:
      {
        if (msg_ptr != NULL)
        {
        s = rr_gprs_debug_msgr_umid_name(msg_ptr->rr_msgr_msg.message.hdr.id);
        }
        break;
      }

#ifdef FEATURE_GSM_TDS
      case MS_TDSRRC_RR:
      {
        switch (message_id)
        {
          case TDSRR_INTERRAT_HANDOVER_REQ:
            s = "TDSRR_INTERRAT_HANDOVER_REQ";
            break;
          case TDSRR_INTERRAT_HANDOVER_ABORT_REQ:
            s = "TDSRR_INTERRAT_HANDOVER_ABORT_REQ";
            break;
          case TDSRR_NAS_DATA_IND:
            s = "TDSRR_NAS_DATA_IND";
            break;
          case TDSRR_INTERRAT_RESELECTION_REQ:
            s = "TDSRR_INTERRAT_RESELECTION_REQ";
            break;
          case TDSRR_INTERRAT_RESELECTION_ABORT_REQ:
            s = "TDSRR_INTERRAT_RESELECTION_ABORT_REQ";
            break;
          case TDSRR_INTERRAT_PLMN_SRCH_REQ:
            s = "TDSRR_INTERRAT_PLMN_SRCH_REQ";
            break;
          case TDSRR_INTERRAT_PLMN_SRCH_ABORT_REQ:
            s = "TDSRR_INTERRAT_PLMN_SRCH_ABORT_REQ";
            break;
          case TDSRR_INTERRAT_CC_ORDER_REQ:
            s = "TDSRR_INTERRAT_CC_ORDER_REQ";
            break;
          case TDSRR_INTERRAT_CC_ORDER_ABORT_REQ:
            s = "TDSRR_INTERRAT_CC_ORDER_ABORT_REQ";
            break;
          case TDSRR_INTERRAT_REDIRECT_REQ:
            s = "TDSRR_INTERRAT_REDIRECT_REQ";
            break;
          case TDSRR_INTERRAT_REDIRECT_ABORT_REQ:
            s = "TDSRR_INTERRAT_REDIRECT_ABORT_REQ";
            break;
          case TDSRR_STOP_GSM_REQ:
            s = "TDSRR_STOP_GSM_REQ";
            break;
          case TDSRR_CIPHER_SYNC_IND:
            s = "TDSRR_CIPHER_SYNC_IND";
          break;
          case TDSRR_INTERRAT_TL1_STOP_IND:
            s = "TDSRR_INTERRAT_TL1_STOP_IND";
            break;
          case TDSRRC_INTERRAT_HANDOVER_CNF:
            s = "TDSRRC_INTERRAT_HANDOVER_CNF";
            break;
          case TDSRRC_INTERRAT_HANDOVER_ABORT_CNF:
            s = "TDSRRC_INTERRAT_HANDOVER_ABORT_CNF";
            break;
          case TDSRRC_INTERRAT_RESELECTION_REJ:
            s = "TDSRRC_INTERRAT_RESELECTION_REJ";
            break;
          case TDSRRC_INTERRAT_RESELECTION_ABORT_CNF:
            s = "TDSRRC_INTERRAT_RESELECTION_ABORT_CNF";
            break;
          case TDSRRC_INTERRAT_PLMN_SRCH_CNF:
            s = "TDSRRC_INTERRAT_PLMN_SRCH_CNF";
            break;
          case TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF:
            s = "TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF";
            break;
          case TDSRRC_INTERRAT_CC_ORDER_CNF:
            s = "TDSRRC_INTERRAT_CC_ORDER_CNF";
            break;
          case TDSRRC_INTERRAT_REDIRECT_REJ:
            s = "TDSRRC_INTERRAT_REDIRECT_REJ";
            break;
          case TDSRRC_INTERRAT_REDIRECT_ABORT_CNF:
            s = "TDSRRC_INTERRAT_REDIRECT_ABORT_CNF";
            break;
          case TDSRRC_STOP_TDS_CNF:
            s = "TDSRRC_STOP_TDS_CNF";
            break;
          case TDSRR_INTERRAT_RESELECTION_REJ:
            s = "TDSRR_INTERRAT_RESELECTION_REJ";
            break;
          case TDSRR_INTERRAT_RESELECTION_ABORT_CNF:
            s = "TDSRR_INTERRAT_RESELECTION_ABORT_CNF";
            break;
          case TDSRR_INTERRAT_HANDOVER_CNF:
            s = "TDSRR_INTERRAT_HANDOVER_CNF";
            break;
          case TDSRR_INTERRAT_HANDOVER_ABORT_CNF:
            s = "TDSRR_INTERRAT_HANDOVER_ABORT_CNF";
            break;
          case TDSRR_INTERRAT_CC_ORDER_REJ:
            s = "TDSRR_INTERRAT_CC_ORDER_REJ";
            break;
          case TDSRR_INTERRAT_CC_ORDER_CNF:
            s = "TDSRR_INTERRAT_CC_ORDER_CNF";
            break;
          case TDSRR_INTERRAT_CC_ORDER_ABORT_CNF:
            s = "TDSRR_INTERRAT_CC_ORDER_ABORT_CNF";
            break;
          case TDSRR_INTERRAT_PLMN_SRCH_CNF:
            s = "TDSRR_INTERRAT_PLMN_SRCH_CNF";
            break;
          case TDSRR_INTERRAT_PLMN_SRCH_ABORT_CNF:
            s = "TDSRR_INTERRAT_PLMN_SRCH_ABORT_CNF";
            break;
          case TDSRR_INTERRAT_REDIRECT_REJ:
            s = "TDSRR_INTERRAT_REDIRECT_REJ";
            break;
          case TDSRR_INTERRAT_REDIRECT_ABORT_CNF:
            s = "TDSRR_INTERRAT_REDIRECT_ABORT_CNF";
            break;
          case TDSRR_STOP_GSM_CNF:
            s = "TDSRR_STOP_GSM_CNF";
            break;
          case TDSRRC_INTERRAT_RESELECTION_REQ:
            s = "TDSRRC_INTERRAT_RESELECTION_REQ";
            break;
          case TDSRRC_INTERRAT_RESELECTION_ABORT_REQ:
            s = "TDSRRC_INTERRAT_RESELECTION_ABORT_REQ";
            break;
          case TDSRRC_INTERRAT_HANDOVER_REQ:
            s = "TDSRRC_INTERRAT_HANDOVER_REQ";
            break;
          case TDSRRC_INTERRAT_HANDOVER_ABORT_REQ:
            s = "TDSRRC_INTERRAT_HANDOVER_ABORT_REQ";
            break;
          case TDSRRC_NAS_DATA_IND:
            s = "TDSRRC_NAS_DATA_IND";
            break;
          case TDSRRC_INTERRAT_CC_ORDER_REQ:
            s = "TDSRRC_INTERRAT_CC_ORDER_REQ";
            break;
          case TDSRRC_INTERRAT_PLMN_SRCH_REQ:
            s = "TDSRRC_INTERRAT_PLMN_SRCH_REQ";
            break;
          case TDSRRC_INTERRAT_PLMN_SRCH_ABORT_REQ:
            s = "TDSRRC_INTERRAT_PLMN_SRCH_ABORT_REQ";
            break;
          case TDSRRC_INTERRAT_REDIRECT_REQ:
            s = "TDSRRC_INTERRAT_REDIRECT_REQ";
            break;
          case TDSRRC_INTERRAT_REDIRECT_ABORT_REQ:
            s = "TDSRRC_INTERRAT_REDIRECT_ABORT_REQ";
            break;
          case TDSRRC_STOP_TDS_REQ:
            s = "TDSRRC_STOP_TDS_REQ";
            break;
          case TDSRR_CIPHER_SYNC:
            s = "TDSRR_CIPHER_SYNC";
            break;
          case TDSRR_INTERRAT_PLMN_SRCH_SUSPEND_IND:
            s = "TDSRR_INTERRAT_PLMN_SRCH_SUSPEND_IND";
            break;
          case TDSRR_DEDICATED_PRIORITIES_REQ:
            s = "TDSRR_DEDICATED_PRIORITIES_REQ";
            break;
          case TDSRRC_DEDICATED_PRIORITIES_RSP:
            s = "TDSRRC_DEDICATED_PRIORITIES_RSP";
            break;
          case TDSRR_CLEAR_DEDICATED_PRIORITIES_IND:
            s = "TDSRR_CLEAR_DEDICATED_PRIORITIES_IND";
            break;
          case TDSRRC_DEDICATED_PRIORITIES_REQ:
            s = "TDSRRC_DEDICATED_PRIORITIES_REQ";
            break;
          case TDSRR_DEDICATED_PRIORITIES_RSP:
            s = "TDSRR_DEDICATED_PRIORITIES_RSP";
            break;
          case TDSRRC_CLEAR_DEDICATED_PRIORITIES_IND:
            s = "TDSRRC_CLEAR_DEDICATED_PRIORITIES_IND";
            break;
          default:
            MSG_ERROR("Unknown message: 0x%04x", message_id, 0, 0);
            s = "?";
        }
        break;
      }
#endif /*FEATURE_GSM_TDS*/

      default:
        MSG_GERAN_ERROR_2("Unknown message_set: %d, message_id %d", message_set, message_id);
        s = "?";

    } /* switch (message_set) */
  } /* else (not an OTA message */

  return s;

} /* end rr_gprs_message_name */

/*!
 * \brief Logs the receipt of an input message received by the RR task.
 * 
 * \param type (in)
 * \param message_ptr (in)
 * \param message_set (in)
 * \param message_id (in)
 * \param state_name (in)
 * \param state (in)
 * \param gas_id (in)
 */
void rr_show_input_message(
  const rr_message_type_e   type,
  const rr_cmd_bdy_type   * message_ptr,
  const int                 message_set,
  const byte                message_id,
  const char              * state_name,
  const uint32              state,
  const gas_id_t            gas_id
)
{
  uint32 msg_level;
  const char *message_name = rr_gprs_message_name(message_ptr, message_set, message_id, &msg_level);

  if (message_set == MS_TIMER)
  {
    switch(msg_level)
    {
      case MSG_LEGACY_LOW:
        MSG_SPRINTF_3(
          MSG_SSID_DFLT,
          MSG_LEGACY_LOW,
          "gs%d:Timer %s expired in state %s",
          (gas_id+1),
          message_name,
          state_name
        );
        break;

      case MSG_LEGACY_MED:
        MSG_SPRINTF_3(
          MSG_SSID_DFLT,
          MSG_LEGACY_MED,
          "gs%d:Timer %s expired in state %s",
          (gas_id+1),
          message_name,
          state_name
        );
        break;

      case MSG_LEGACY_HIGH:
        MSG_SPRINTF_3(
          MSG_SSID_DFLT,
          MSG_LEGACY_HIGH,
          "gs%d:Timer %s expired in state %s",
          (gas_id+1),
          message_name,
          state_name
        );
        break;

      default:
      {
      }
    }
  }
  else /* All other messages received by RR */
  {
    switch (msg_level)
    {
      case MSG_LEGACY_LOW:
      {
        /* Text debug is printed at MSG_MASK_5 so that it can be selectively
        enabled or disabled. */
        switch (type)
        {
          case RR_MESSAGE_TYPE_NEW:
          {
            MSG_SPRINTF_3(
              MSG_SSID_DFLT,
              MSG_MASK_5,
              "gs%d:IMsg: %s state %s",
              (gas_id+1),
              message_name,
              state_name
            );
            break;
          } /* RR_MESSAGE_TYPE_NEW */

          case RR_MESSAGE_TYPE_SAVED:
          {
            MSG_SPRINTF_3(
              MSG_SSID_DFLT,
              MSG_MASK_5,
              "gs%d:IMsg: %s state %s (Saved)",
              (gas_id+1),
              message_name,
              state_name
            );
            break;
          } /* RR_MESSAGE_TYPE_SAVED */

          case RR_MESSAGE_TYPE_PENDED:
          {
            MSG_SPRINTF_3(
              MSG_SSID_DFLT,
              MSG_MASK_5,
              "gs%d:IMsg: %s state %s (Pended)",
              (gas_id+1),
              message_name,
              state_name
            );
            break;
          } /* RR_MESSAGE_TYPE_SAVED */

          default:
          {
          }
        }

        /* Numeric debug is printed at MSG_LEGACY_LOW. */
        if ((message_ptr != NULL) && (!msg_status(MSG_SSID_DFLT, MSG_MASK_5)))
        {
          if (message_ptr->message_header.message_set == MS_RR_RR)
          {
            switch (type)
            {
              case RR_MESSAGE_TYPE_NEW:
              {
                MSG_5(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_LOW,
                  "gs%d:IMsg: (%d,%d,%d) state %d",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr.header.rr_message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_NEW */

              case RR_MESSAGE_TYPE_SAVED:
              {
                MSG_5(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_LOW,
                  "gs%d:IMsg: (%d,%d,%d) state %d (Saved)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr.header.rr_message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_SAVED */

              case RR_MESSAGE_TYPE_PENDED:
              {
                MSG_5(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_LOW,
                  "gs%d:IMsg: (%d,%d,%d) state %d (Pended)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr.header.rr_message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              }

              default:
              {
              }
            }
          }
          else if (message_ptr->message_header.message_set == MS_MSGR_RR)
          {
            switch (type)
            {
              case RR_MESSAGE_TYPE_NEW:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_LOW,
                  "gs%d:IMsg: (%d,%d) state %d",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr_msgr_msg.message.hdr.id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_NEW */

              case RR_MESSAGE_TYPE_SAVED:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_LOW,
                  "gs%d:IMsg: (%d,%d) state %d (Saved)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr_msgr_msg.message.hdr.id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_SAVED */

              case RR_MESSAGE_TYPE_PENDED:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_LOW,
                  "gs%d:IMsg: (%d,%d) state %d (Pended)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr_msgr_msg.message.hdr.id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_PENDED */

              default:
              {
              }
            }
          }
          else
          {
            switch (type)
            {
              case RR_MESSAGE_TYPE_NEW:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_LOW,
                  "gs%d:IMsg: (%d,%d) in state %d",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_NEW */

              case RR_MESSAGE_TYPE_SAVED:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_LOW,
                  "gs%d:IMsg: (%d,%d) state %d (Saved)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_SAVED */

              case RR_MESSAGE_TYPE_PENDED:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_LOW,
                  "gs%d:IMsg: (%d,%d) state %d (Pended)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              }

              default:
              {
              }
            }
          }
        }
        break;
      } /* MSG_LEGACY_LOW */

      case MSG_LEGACY_MED:
      {
        /* Text debug is printed at MSG_MASK_6 so that it can be selectively
        enabled or disabled. */
        switch (type)
        {
          case RR_MESSAGE_TYPE_NEW:
          {
            MSG_SPRINTF_3(
              MSG_SSID_DFLT,
              MSG_MASK_6,
              "gs%d:IMsg: %s state %s",
              (gas_id+1),
              message_name,
              state_name
            );
            break;
          } /* RR_MESSAGE_TYPE_NEW */

          case RR_MESSAGE_TYPE_SAVED:
          {
            MSG_SPRINTF_3(
              MSG_SSID_DFLT,
              MSG_MASK_6,
              "gs%d:IMsg: %s state %s (Saved)",
              (gas_id+1),
              message_name,
              state_name
            );
            break;
          } /* RR_MESSAGE_TYPE_SAVED */

          case RR_MESSAGE_TYPE_PENDED:
          {
            MSG_SPRINTF_3(
              MSG_SSID_DFLT,
              MSG_MASK_6,
              "gs%d:IMsg: %s state %s (Pended)",
              (gas_id+1),
              message_name,
              state_name
            );
            break;
          } /* RR_MESSAGE_TYPE_SAVED */

          default:
          {
          }
        }

        /* Numeric debug is printed at MSG_LEGACY_MED. */
        if ((message_ptr != NULL) && (!msg_status(MSG_SSID_DFLT, MSG_MASK_6)))
        {
          if (message_ptr->message_header.message_set == MS_RR_RR)
          {
            switch (type)
            {
              case RR_MESSAGE_TYPE_NEW:
              {
                MSG_5(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_MED,
                  "gs%d:IMsg: (%d,%d,%d) state %d",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr.header.rr_message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_NEW */

              case RR_MESSAGE_TYPE_SAVED:
              {
                MSG_5(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_MED,
                  "gs%d:IMsg: (%d,%d,%d) state %d (Saved)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr.header.rr_message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_SAVED */

              case RR_MESSAGE_TYPE_PENDED:
              {
                MSG_5(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_MED,
                  "gs%d:IMsg: (%d,%d,%d) state %d (Pended)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr.header.rr_message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              }

              default:
              {
              }
            }
          }
          else if (message_ptr->message_header.message_set == MS_MSGR_RR)
          {
            switch (type)
            {
              case RR_MESSAGE_TYPE_NEW:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_MED,
                  "gs%d:IMsg: (%d,%d) state %d",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr_msgr_msg.message.hdr.id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_NEW */

              case RR_MESSAGE_TYPE_SAVED:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_MED,
                  "gs%d:IMsg: (%d,%d) state %d (Saved)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr_msgr_msg.message.hdr.id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_SAVED */

              case RR_MESSAGE_TYPE_PENDED:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_MED,
                  "gs%d:IMsg: (%d,%d) state %d (Pended)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr_msgr_msg.message.hdr.id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_PENDED */

              default:
              {
              }
            }
          }
          else
          {
            switch (type)
            {
              case RR_MESSAGE_TYPE_NEW:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_MED,
                  "gs%d:IMsg: (%d,%d) in state %d",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_NEW */

              case RR_MESSAGE_TYPE_SAVED:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_MED,
                  "gs%d:IMsg: (%d,%d) state %d (Saved)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_SAVED */

              case RR_MESSAGE_TYPE_PENDED:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_MED,
                  "gs%d:IMsg: (%d,%d) state %d (Pended)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              }

              default:
              {
              }
            }
          }
        }
        break;
      } /* MSG_LEGACY_MED */

      case MSG_LEGACY_HIGH:
      {
        /* Text debug is printed at MSG_MASK_7 so that it can be selectively
        enabled or disabled. */
        switch (type)
        {
          case RR_MESSAGE_TYPE_NEW:
          {
            MSG_SPRINTF_3(
              MSG_SSID_DFLT,
              MSG_MASK_7,
              "gs%d:IMsg: %s state %s",
              (gas_id+1),
              message_name,
              state_name
            );
            break;
          } /* RR_MESSAGE_TYPE_NEW */

          case RR_MESSAGE_TYPE_SAVED:
          {
            MSG_SPRINTF_3(
              MSG_SSID_DFLT,
              MSG_MASK_7,
              "gs%d:IMsg: %s state %s (Saved)",
              (gas_id+1),
              message_name,
              state_name
            );
            break;
          } /* RR_MESSAGE_TYPE_SAVED */

          case RR_MESSAGE_TYPE_PENDED:
          {
            MSG_SPRINTF_3(
              MSG_SSID_DFLT,
              MSG_MASK_7,
              "gs%d:IMsg: %s state %s (Pended)",
              (gas_id+1),
              message_name,
              state_name
            );
            break;
          } /* RR_MESSAGE_TYPE_SAVED */

          default:
          {
          }
        }

        /* Numeric debug is printed at MSG_LEGACY_HIGH. */
        if ((message_ptr != NULL) && (!msg_status(MSG_SSID_DFLT, MSG_MASK_7)))
        {
          if (message_ptr->message_header.message_set == MS_RR_RR)
          {
            switch (type)
            {
              case RR_MESSAGE_TYPE_NEW:
              {
                MSG_5(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_HIGH,
                  "gs%d:IMsg: (%d,%d,%d) state %d",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr.header.rr_message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_NEW */

              case RR_MESSAGE_TYPE_SAVED:
              {
                MSG_5(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_HIGH,
                  "gs%d:IMsg: (%d,%d,%d) state %d (Saved)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr.header.rr_message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_SAVED */

              case RR_MESSAGE_TYPE_PENDED:
              {
                MSG_5(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_HIGH,
                  "gs%d:IMsg: (%d,%d,%d) state %d (Pended)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr.header.rr_message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              }

              default:
              {
              }
            }
          }
          else if (message_ptr->message_header.message_set == MS_MSGR_RR)
          {
            switch (type)
            {
              case RR_MESSAGE_TYPE_NEW:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_HIGH,
                  "gs%d:IMsg: (%d,%d) state %d",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr_msgr_msg.message.hdr.id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_NEW */

              case RR_MESSAGE_TYPE_SAVED:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_HIGH,
                  "gs%d:IMsg: (%d,%d) state %d (Saved)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr_msgr_msg.message.hdr.id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_SAVED */

              case RR_MESSAGE_TYPE_PENDED:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_HIGH,
                  "gs%d:IMsg: (%d,%d) state %d (Pended)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->rr_msgr_msg.message.hdr.id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_PENDED */

              default:
              {
              }
            }
          }
          else
          {
            switch (type)
            {
              case RR_MESSAGE_TYPE_NEW:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_HIGH,
                  "gs%d:IMsg: (%d,%d) in state %d",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_NEW */

              case RR_MESSAGE_TYPE_SAVED:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_HIGH,
                  "gs%d:IMsg: (%d,%d) state %d (Saved)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              } /* RR_MESSAGE_TYPE_SAVED */

              case RR_MESSAGE_TYPE_PENDED:
              {
                MSG_4(
                  MSG_SSID_DFLT,
                  MSG_LEGACY_HIGH,
                  "gs%d:IMsg: (%d,%d) state %d (Pended)",
                  (gas_id+1),
                  message_ptr->message_header.message_set,
                  message_ptr->message_header.message_id,
                  state
                );
                break;
              }

              default:
              {
              }
            }
          }
        }
        break;
      } /* MSG_LEGACY_HIGH */

      default:
      {
        MSG_GERAN_ERROR_1("Unexpected msg_level: %d", msg_level);
      }
    }
  }

#ifdef FEATURE_QSH_DUMP
  if ((type == RR_MESSAGE_TYPE_NEW) && (message_ptr->message_header.message_set == MS_RR_RR))
  {
    timetick_type timestamp_clk = timetick_get();

    rr_qsh_dump_log_imsg(
     timestamp_clk,
      message_ptr->rr.header.rr_message_set,
      message_ptr->rr.header.imh.message_id,
      gas_id
    );
  }
#endif // FEATURE_QSH_DUMP

#ifdef FEATURE_QSH_METRIC
  if ((message_type == RR_MESSAGE_TYPE_NEW) && (message_ptr->message_header.message_set == MS_RR_RR))
  {
    rr_qsh_metric_log_imsg(
      message_ptr->rr.header.rr_message_set,
      message_ptr->rr.header.imh.message_id,
      gas_id
    );
  }
#endif // FEATURE_QSH_METRIC

} /* rr_show_input_message */

/*!
 * \brief Logs the sending of an output message sent from the RR task.
 * 
 * \param destination (in)
 * \param message_ptr (in)
 * \param message_set (in)
 * \param message_id (in)
 * \param gas_id (in)
 */
void rr_show_output_message(
  const char     * destination,
  const IMH_T    * message_ptr,
  const uint8      message_set,
  const uint32     message_id,
  const gas_id_t   gas_id
)
{
  uint32 msg_level;

  const char * message_name;

  // RR-RR internal messages are not printed here, as they will be printed when processed as input messages
  if (message_set == MS_RR_RR)
  {
    return;
  }

  if (message_set == MS_MSGR_RR)
  {
    message_name = rr_gprs_debug_msgr_umid_name(message_id);
    msg_level = MSG_LEGACY_HIGH;
  }
  else
  {
    message_name = rr_gprs_message_name(
      (rr_cmd_bdy_type *) ((void *) message_ptr),
      message_set,
      message_id,
      &msg_level
    );
  }

  switch(msg_level)
  {
    case MSG_LEGACY_LOW:
    {
      MSG_SPRINTF_3(
        MSG_SSID_DFLT,
        MSG_MASK_5,
        "gs%d:OMsg: %s sent to %s",
        (gas_id+1),
        message_name,
        destination
      );

        MSG_3(
          MSG_SSID_DFLT,
          MSG_LEGACY_LOW,
          "gs%d:OMsg: (%d,%d)",
          (gas_id+1),
          message_set,
          message_id
        );
      break;
    }

    case MSG_LEGACY_MED:
    {
      MSG_SPRINTF_3(
        MSG_SSID_DFLT,
        MSG_MASK_6,
        "gs%d:OMsg: %s sent to %s",
        (gas_id+1),
        message_name,
        destination
      );

        MSG_3(
          MSG_SSID_DFLT,
          MSG_LEGACY_MED,
          "gs%d:OMsg: (%d,%d)",
          (gas_id+1),
          message_set,
          message_id
        );
      break;
    }

    case MSG_LEGACY_HIGH:
    {
      MSG_SPRINTF_3(
        MSG_SSID_DFLT,
        MSG_MASK_7,
        "gs%d:OMsg: %s sent to %s",
        (gas_id+1),
        message_name,
        destination
      );

       MSG_3(
          MSG_SSID_DFLT,
          MSG_LEGACY_HIGH,
          "gs%d:OMsg: (%d,%d)",
          (gas_id+1),
          message_set,
          message_id
        );
      break;
    }

    default:
      break;
  }
} /* rr_show_output_message */

/*!
 * \brief Returns a text string representation of the event passed in.
 * 
 * \param event (in)
 * 
 * \return char* 
 */
char * rr_event_name(const rr_event_T event)
{
  char * s;

  switch (event)
  {
    /**********************************************/
    /* Generic state machine events               */
    /**********************************************/

    case EV_RESET_SOFTWARE:
      s = "EV_RESET_SOFTWARE";
      break;

    case EV_NO_EVENT:
      s = "EV_NO_EVENT";
      break;

    case EV_INPUT_MESSAGE:
      s = "EV_INPUT_MESSAGE";
      break;

    /**********************************************/
    /* Events generated during sys info processing*/
    /**********************************************/

    case RR_EV_GSM_SI_RECEIVED:
      s = "RR_EV_GSM_SI_RECEIVED";
      break;

    case RR_EV_EARLY_CAMP_SI_RECEIVED:
      s = "RR_EV_EARLY_CAMP_SI_RECEIVED";
      break;

    case RR_EV_SYS_INFO_TIMEOUT:
      s = "RR_EV_SYS_INFO_TIMEOUT";
      break;

    case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
      s = "RR_EV_CANDIDATE_CELL_SI_RECEIVED";
      break;

    case RR_EV_GPRS_SI13_RECEIVED:
      s = "RR_EV_GPRS_SI13_RECEIVED";
      break;

    case RR_EV_GPRS_SYS_INFO_TIMEOUT:
      s = "RR_EV_GPRS_SYS_INFO_TIMEOUT";
      break;

    case RR_EV_ACCESS_CLASS_CHANGED:
      s = "RR_EV_ACCESS_CLASS_CHANGED";
      break;

    /**********************************************/
    /* Cell selection events                      */
    /**********************************************/

    case RR_EV_PERFORM_ACQ_DB_POWER_SCAN:
      s = "RR_EV_PERFORM_ACQ_DB_POWER_SCAN";
      break;

    case RR_EV_PERFORM_POWER_SCAN:
      s = "RR_EV_PERFORM_POWER_SCAN";
      break;

    case RR_EV_SPECIFIC_CHANNEL_SEARCH:
      s = "RR_EV_SPECIFIC_CHANNEL_SEARCH";
      break;

    case RR_EV_DECODING_BCCH:
      s = "RR_EV_DECODING_BCCH";
      break;

    case RR_EV_CHANNEL_FOUND:
      s = "RR_EV_CHANNEL_FOUND";
      break;

    case RR_EV_NO_CELL_SELECTION_REQUIRED:
      s = "RR_EV_NO_CELL_SELECTION_REQUIRED";
      break;

    case RR_EV_CELL_SELECTION_ABORT_TIMEOUT:
      s = "RR_EV_CELL_SELECTION_ABORT_TIMEOUT";
      break;

    case RR_EV_ACQUISITION_FAILURE:
      s = "RR_EV_ACQUISITION_FAILURE";
      break;

    case RR_EV_CAMPED_ON_CELL:
      s = "RR_EV_CAMPED_ON_CELL";
      break;

    case RR_EV_CAMPED_ON_CELL_W2G_CCO:
      s = "RR_EV_CAMPED_ON_CELL_W2G_CCO";
      break;

    case EV_CHECK_GPRS:
      s = "EV_CHECK_GPRS";
      break;

    case RR_EV_TRIGGER_CELL_SELECTION:
      s = "RR_EV_TRIGGER_CELL_SELECTION";
      break;

    case EV_CELL_SELECTION_TRIGGERED:
      s = "EV_CELL_SELECTION_TRIGGERED";
      break;

    case EV_L1_IDLE_MODE_CNF_RECEIVED:
      s = "EV_L1_IDLE_MODE_CNF_RECEIVED";
      break;

    case RR_EV_PROCESS_PLMN_SELECT_REQ:
      s = "RR_EV_PROCESS_PLMN_SELECT_REQ";
      break;

    case EV_CELL_SELECTION_ABORTED:
      s = "EV_CELL_SELECTION_ABORTED";
      break;

    case RR_EV_ABORT_CELL_SELECTION:
      s = "RR_EV_ABORT_CELL_SELECTION";
      break;

    case RR_EV_REPROCESS_CANDIDATE_CELL_DB:
      s = "RR_EV_REPROCESS_CANDIDATE_CELL_DB";
      break;

    case RR_EV_DECODING_ACQ_DB_BCCH:
      s = "RR_EV_DECODING_ACQ_DB_BCCH";
      break;

    case EV_USE_EXISTING_POWER_SCAN_RESULTS:
      s = "EV_USE_EXISTING_POWER_SCAN_RESULTS";
      break;

    /**********************************************/
    /* RR/GRR control events                      */
    /**********************************************/

    case EV_GO_TO_IDLE:
      s = "EV_GO_TO_IDLE";
      break;

    case EV_NO_BCCH_FOUND:
      s = "EV_NO_BCCH_FOUND";
      break;

    case RR_EV_DECODE_BCCH_COMPLETE_TIMEOUT:
      s = "RR_EV_DECODE_BCCH_COMPLETE_TIMEOUT";
      break;

    case EV_RR_EST_REQ:
      s = "EV_RR_EST_REQ";
      break;

    case EV_TRIGGER_CELL_SELECTION:
      s = "EV_TRIGGER_CELL_SELECTION";
      break;

    case EV_FORCE_CELL_RESELECTION:
      s = "EV_FORCE_CELL_RESELECTION";
      break;

    case EV_GRR_EST_REQ:
      s = "EV_GRR_EST_REQ";
      break;

    case EV_RR_EST_CNF:
      s = "EV_RR_EST_CNF";
      break;

    case EV_RR_EST_IND:
      s = "EV_RR_EST_IND";
      break;

    case EV_CHOOSE_CELL:
      s = "EV_CHOOSE_CELL";
      break;

    case EV_ABNORMAL_RELEASE_PART_SI:
      s = "EV_ABNORMAL_RELEASE_PART_SI";
      break;

    case EV_ABNORMAL_RELEASE_FULL_SI:
      s = "EV_ABNORMAL_RELEASE_FULL_SI";
      break;

    case RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED:
      s = "RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED";
      break;

    case EV_GPRS_SUSPEND:
      s = "EV_GPRS_SUSPEND";
      break;

    case EV_GPRS_SUSPENDED:
      s = "EV_GPRS_SUSPENDED";
      break;

    case EV_GPRS_SUSPEND_FOR_SI_ACQUISITION:
      s = "EV_GPRS_SUSPEND_FOR_SI_ACQUISITION";
      break;

    case EV_GPRS_SUSPEND_FOR_SI_ACQUISITION_NO_SI13:
      s = "EV_GPRS_SUSPEND_FOR_SI_ACQUISITION_NO_SI13";
      break;

    case EV_GPRS_SUSPENDED_FOR_SI_ACQUISITION:
      s = "EV_GPRS_SUSPENDED_FOR_SI_ACQUISITION";
      break;

    case EV_PROCESS_SAVED_MESSAGE:
      s = "EV_PROCESS_SAVED_MESSAGE";
      break;

    case RR_EV_PROCESS_SAVED_MESSAGE:
      s = "RR_EV_PROCESS_SAVED_MESSAGE";
      break;

    case RR_EV_MM_REQ_RECEIVED_ABORT:
      s = "RR_EV_MM_REQ_RECEIVED_ABORT";
      break;

    case EV_NON_CAMPED_SYS_INFO_ACQUISITION:
      s = "EV_NON_CAMPED_SYS_INFO_ACQUISITION";
      break;

    /**********************************************/
    /* Cell reselection events                    */
    /**********************************************/

    case EV_CELL_RESELECTION:
      s = "EV_CELL_RESELECTION";
      break;

    case EV_NO_CELL_RESELECTION:
      s = "EV_NO_CELL_RESELECTION";
      break;

    case EV_PATH_LOSS_TIMEOUT:
      s = "EV_PATH_LOSS_TIMEOUT";
      break;

    case EV_ABANDON_RESELECTION:
      s = "EV_ABANDON_RESELECTION";
      break;

    case EV_MAC_IDLE_MODE:
      s = "EV_MAC_IDLE_MODE";
      break;

    case EV_RECENT_RESELECTION_TIMEOUT:
      s = "EV_RECENT_RESELECTION_TIMEOUT";
      break;

    case EV_CELL_RESELECTION_ACT:
      s = "EV_CELL_RESELECTION_ACT";
      break;

    case EV_CELL_RESELECTION_DEACT:
      s = "EV_CELL_RESELECTION_DEACT";
      break;

    case EV_ACTION_PCCO:
      s = "EV_ACTION_PCCO";
      break;

    case EV_ACTION_IMMEDIATE_PCCO:
      s = "EV_ACTION_IMMEDIATE_PCCO";
      break;

    case EV_T3174_TIMEOUT:
      s = "EV_T3174_TIMEOUT";
      break;

    case EV_T3176_TIMEOUT:
      s = "EV_T3176_TIMEOUT";
      break;

    case EV_DSF_RESELECTION:
      s = "EV_DSF_RESELECTION";
      break;

    case EV_DSF_FOR_DENIALS:
      s = "EV_DSF_FOR_DENIALS";
      break;

    case EV_MAC_GUARD_TIMER_EXPIRED:
      s = "EV_MAC_GUARD_TIMER_EXPIRED";
      break;

    case EV_CAMP_ON_PCCF_OLD_SCELL:
      s = "EV_CAMP_ON_PCCF_OLD_SCELL";
      break;

    case RR_EV_START_RESELECTION_POWER_SCAN:
      s = "RR_EV_START_RESELECTION_POWER_SCAN";
      break;

    case EV_START_CCN_CELL_RESELECTION:
      s = "EV_START_CCN_CELL_RESELECTION";
      break;

    case EV_RESUME_CELL_RESELECTION:
      s = "EV_RESUME_CELL_RESELECTION";
      break;

    case EV_T3208_EXPIRY:
      s = "EV_T3208_EXPIRY";
      break;

    case EV_T3210_EXPIRY:
      s = "EV_T3210_EXPIRY";
      break;

    case EV_ALL_NACC_PNCD_DISPATCHED:
      s = "EV_ALL_NACC_PNCD_DISPATCHED";
      break;

    case EV_ACTION_PCCO_WITH_PNCD:
      s = "EV_ACTION_PCCO_WITH_PNCD";
      break;

    case EV_PS_ACCESS_BLOCKED:
      s = "EV_PS_ACCESS_BLOCKED";
      break;

    /**********************************************/
    /* Measurement reporting events               */
    /**********************************************/

    /* External input/output events */
    case RR_EV_DEACTIVATE_MEASUREMENTS:
      s = "RR_EV_DEACTIVATE_MEASUREMENTS";
      break;

    case RR_EV_MEASUREMENT_PARAMS_UPDATED:
      s = "RR_EV_MEASUREMENT_PARAMS_UPDATED";
      break;

    case RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13:
      s = "RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13";
      break;

    /* Internal events */
    case EV_T3158_TIMEOUT:
      s = "EV_T3158_TIMEOUT";
      break;

    case EV_PMO_MESSAGE:
      s = "EV_PMO_MESSAGE";
      break;

    case EV_PCCO_MESSAGE:
      s = "EV_PCCO_MESSAGE";
      break;

    case EV_MAC_TRANSFER:
      s = "EV_MAC_TRANSFER";
      break;

    case EV_MAC_IDLE:
      s = "EV_MAC_IDLE";
      break;

    case EV_MAC_IDLE_BLOCKED:
      s = "EV_MAC_IDLE_BLOCKED";
      break;

    case EV_NCO_MODE_0:
      s = "EV_NCO_MODE_0";
      break;

    case EV_NCO_MODE_1:
      s = "EV_NCO_MODE_1";
      break;

    case EV_NCO_MODE_2:
      s = "EV_NCO_MODE_2";
      break;

    /**
    * Additional G2W cell reselection events
    */
    case EV_CR_G2W_TIMER_EXPIRED:
      s = "EV_CR_G2W_TIMER_EXPIRED";
      break;

    /**********************************************/
    /* Connection Establishment events            */
    /**********************************************/

    case EV_INIT_ACCESS_PROCEDURE:
      s = "EV_INIT_ACCESS_PROCEDURE";
      break;

    case EV_ACCESS_CLASS_NOT_ALLOWED:
      s = "EV_ACCESS_CLASS_NOT_ALLOWED";
      break;

    case EV_IMMEDIATE_ASSIGNMENT:
      s = "EV_IMMEDIATE_ASSIGNMENT";
      break;

    case EV_MAX_RETRIES_ATTEMPTED:
      s = "EV_MAX_RETRIES_ATTEMPTED";
      break;

    case EV_ASSIGNMENT_REJECTION:
      s = "EV_ASSIGNMENT_REJECTION";
      break;

    case EV_CHANNEL_DEACTIVATED:
      s = "EV_CHANNEL_DEACTIVATED";
      break;

    case EV_T3126_EXPIRY:
      s = "EV_T3126_EXPIRY";
      break;

    case EV_RACH_FOR_CS_FAILURE:
      s = "EV_RACH_FOR_CS_FAILURE";
      break;

    case EV_RACH_FOR_PS_FAILURE:
      s = "EV_RACH_FOR_PS_FAILURE";
      break;

    case EV_CONNECTION_ABORTION:
      s = "EV_CONNECTION_ABORTION";
      break;

    case EV_CONNECTION_RELEASED:
      s = "EV_CONNECTION_RELEASED";
      break;

    case EV_GPRS_IMMEDIATE_ASSIGNMENT:
      s = "EV_GPRS_IMMEDIATE_ASSIGNMENT";
      break;

    case EV_GPRS_INIT_ACCESS_PROCEDURE:
      s = "EV_GPRS_INIT_ACCESS_PROCEDURE";
      break;

    case EV_GPRS_ACCESS_NOT_POSSIBLE:
      s = "EV_GPRS_ACCESS_NOT_POSSIBLE";
      break;

    case EV_T3146_EXPIRY:
      s = "EV_T3146_EXPIRY";
      break;

    case EV_TMULTIFRM_EXPIRY:
      s = "EV_TMULTIFRM_EXPIRY";
      break;

    case EV_GPRS_RESET_RR_CE_STATE:
      s = "EV_GPRS_RESET_RR_CE_STATE";
      break;

    case EV_GPRS_CONNECTION_ABORTION:
      s = "EV_GPRS_CONNECTION_ABORTION";
      break;

    case EV_GRR51_GO_TO_PACKET_IDLE:
      s = "EV_GRR51_GO_TO_PACKET_IDLE";
      break;

    case EV_REPEAT_ASSIGNMENT_PROCEDURE:
      s = "EV_REPEAT_ASSIGNMENT_PROCEDURE";
      break;

    case EV_T3174_EXPIRY:
      s = "EV_T3174_EXPIRY";
      break;

    case EV_T3176_EXPIRY:
      s = "EV_T3176_EXPIRY";
      break;

    case EV_TBF_CONN_EST_FAILURE:
      s = "EV_TBF_CONN_EST_FAILURE";
      break;

    case EV_TBF_CONN_EST_ABORTED_CAMPED:
      s = "EV_TBF_CONN_EST_ABORTED_CAMPED";
      break;

    case EV_TBF_CONN_EST_ABORTED_NON_CAMPED:
      s = "EV_TBF_CONN_EST_ABORTED_NON_CAMPED";
      break;

    /**********************************************/
    /* Connection release events                  */
    /**********************************************/

    case EV_CONNECTION_EXISTS:
      s = "EV_CONNECTION_EXISTS";
      break;

    case EV_T3110_TIMEOUT:
      s = "EV_T3110_TIMEOUT";
      break;

    case RR_EV_CONNECTION_RELEASE_IN_PROGRESS:
      s = "RR_EV_CONNECTION_RELEASE_IN_PROGRESS";
      break;

    case EV_CHOOSE_CELL_FOR_REEST:
      s = "EV_CHOOSE_CELL_FOR_REEST";
      break;

    case EV_CHOOSE_CELL_FOR_CAMP:
      s = "EV_CHOOSE_CELL_FOR_CAMP";
      break;

    case RR_EV_CELL_REEST_TRIGGERED:
      s = "RR_EV_CELL_REEST_TRIGGERED";
      break;

    /**********************************************/
    /* Dispatch events                            */
    /**********************************************/

    case EV_NEW_MESSAGE:
      s = "EV_NEW_MESSAGE";
      break;

    case EV_INTER_CELL_HANDOVER:
      s = "EV_INTER_CELL_HANDOVER";
      break;

    case EV_INTRA_CELL_HANDOVER:
      s = "EV_INTRA_CELL_HANDOVER";
      break;

    case EV_G2W_HANDOVER:
      s = "EV_G2W_HANDOVER";
      break;

    case EV_CIPHERING:
      s = "EV_CIPHERING";
      break;

    case EV_HANDOVER_TERMINATED:
      s = "EV_HANDOVER_TERMINATED";
      break;

    case EV_CIPHERING_TERMINATED:
      s = "EV_CIPHERING_TERMINATED";
      break;

    case EV_ASSIGNMENT_COMMAND_CHECKED:
      s = "EV_ASSIGNMENT_COMMAND_CHECKED";
      break;

    case EV_HANDOVER_COMMAND_CHECKED:
      s = "EV_HANDOVER_COMMAND_CHECKED";
      break;

    case EV_T3124_TIMEOUT:
      s = "EV_T3124_TIMEOUT";
      break;

    case EV_HANDOVER_COMPLETED:
      s = "EV_HANDOVER_COMPLETED";
      break;

    /**********************************************/
    /* Other Dedicated events                     */
    /**********************************************/

    case EV_CIPHER_COMMAND_CHECKED:
      s = "EV_CIPHER_COMMAND_CHECKED";
      break;

    case EV_RADIO_LINK_FAILURE:
      s = "EV_RADIO_LINK_FAILURE";
      break;

    case EV_PROCEDURE_COMPLETE:
      s = "EV_PROCEDURE_COMPLETE";
      break;

    /**********************************************/
    /* RR Mode control events                     */
    /**********************************************/

    case EV_ACTIVATE_GSM_MODE:
      s = "EV_ACTIVATE_GSM_MODE";
      break;

    case EV_ACTIVATE_GSM_MODE_FOR_X2G_RESEL:
      s = "EV_ACTIVATE_GSM_MODE_FOR_X2G_RESEL";
      break;

    case EV_DEACTIVATE_GSM_MODE:
      s = "EV_DEACTIVATE_GSM_MODE";
      break;

    case EV_GSM_MODE_ACTIVATED:
      s = "EV_GSM_MODE_ACTIVATED";
      break;

    case EV_GSM_MODE_DEACTIVATED:
      s = "EV_GSM_MODE_DEACTIVATED";
      break;

    case EV_PANIC_RESET:
      s = "EV_PANIC_RESET";
      break;

    /**********************************************/
    /* InterRAT WCDMA To GSM Handover events      */
    /**********************************************/

    case RR_EV_W2G_HO_ABORTED:
      s = "RR_EV_W2G_HO_ABORTED";
      break;

    case RR_EV_W2G_HO_COMPLETED:
      s = "RR_EV_W2G_HO_COMPLETED";
      break;

    case RR_EV_W2G_GO_TO_INACTIVE:
      s = "RR_EV_W2G_GO_TO_INACTIVE";
      break;

    case RR_EV_W2G_HO_PROTOCOL_ERROR:
      s = "RR_EV_W2G_HO_PROTOCOL_ERROR";
      break;

    /*********************************************/
    /* InterRAT WCDMA to GSM Reselection events  */
    /*********************************************/

    case RR_EV_W2G_NO_SUITABLE_CELL_FOUND:
      s = "RR_EV_W2G_NO_SUITABLE_CELL_FOUND";
      break;

    case RR_EV_W2G_SUITABLE_CELL_FOUND:
      s = "RR_EV_W2G_SUITABLE_CELL_FOUND";
      break;

    case RR_EV_W2G_PCCO_FAILED:
      s = "RR_EV_W2G_PCCO_FAILED";
      break;

    /****************************************************/
    /* InterRAT WCDMA to GSM Service Redirection events */
    /****************************************************/
    case RR_EV_TRIGGER_CELL_SELECTION_W2G:
      s = "RR_EV_TRIGGER_CELL_SELECTION_W2G";
      break;

    case RR_EV_SERVICE_REDIRECTION_WAIT_TIMER_EXPIRY:
      s = "RR_EV_SERVICE_REDIRECTION_WAIT_TIMER_EXPIRY";
      break;

    case RR_EV_ABORT_PROCEDURE:
      s = "RR_EV_ABORT_PROCEDURE";
      break;

    /*********************************************/
    /* InterRAT WCDMA to GSM PLMN SEARCH events  */
    /*********************************************/
    case EV_ACTIVATE_GSM_MODE_FOR_BPLMN:
      s = "EV_ACTIVATE_GSM_MODE_FOR_BPLMN";
      break;

    /*********************************************/
    /* InterRAT GSM to WCDMA Reselection events  */
    /*********************************************/

    case RR_EV_FORCE_WCDMA_CELL_RESELECTION:
      s = "RR_EV_FORCE_WCDMA_CELL_RESELECTION";
      break;

    case RR_EV_G2WR_RESELECTION_PSCAN_STARTED:
      s = "RR_EV_G2WR_RESELECTION_PSCAN_STARTED";
      break;

    case RR_EV_RESELECT_WCDMA_CELL:
      s = "RR_EV_RESELECT_WCDMA_CELL";
      break;

    case RR_EV_G2WR_GO_TO_INACTIVE:
      s = "RR_EV_G2WR_GO_TO_INACTIVE";
      break;

    case RR_EV_G2WR_GOING_INACTIVE:
      s = "RR_EV_G2WR_GOING_INACTIVE";
      break;

    /**********************************************/
    /* InterRAT GSM To WCDMA Handover events      */
    /**********************************************/

    case RR_EV_G2W_HO_STARTED:
      s = "RR_EV_G2W_HO_STARTED";
      break;

    case RR_EV_G2W_HO_ABORTED:
      s = "RR_EV_G2W_HO_ABORTED";
      break;

    case RR_EV_G2W_HO_FAIL:
      s = "RR_EV_G2W_HO_FAIL";
      break;

    case RR_EV_G2W_HO_SUCCESS:
      s = "RR_EV_G2W_HO_SUCCESS";
      break;

    #ifdef FEATURE_INTERRAT_PCCO_GTOW
    case RR_EV_G2W_PCCO_STARTED:
      s = "RR_EV_G2W_PCCO_STARTED";
      break;

    case RR_EV_G2W_PCCO_FAILED:
      s = "RR_EV_G2W_PCCO_FAILED";
      break;

    #endif /* FEATURE_INTERRAT_PCCO_GTOW */

    case EV_CELL_CHOSEN:
      s = "EV_CELL_CHOSEN";
      break;

    /**********************************************/
    /* DTM events                                 */
    /**********************************************/

    #ifdef FEATURE_GSM_DTM
    case EV_DTM_CS_RELEASED:
      s = "EV_DTM_CS_RELEASED";
      break;

    case EV_DTM_CS_RELEASE_REQUIRED:
      s = "EV_DTM_CS_RELEASE_REQUIRED";
      break;

    case EV_DTM_PERFORM_GSM_RELEASE:
      s = "EV_DTM_PERFORM_GSM_RELEASE";
      break;

    case EV_DTM_PERFORM_CSPS_RELEASE:
      s = "EV_DTM_PERFORM_CSPS_RELEASE";
      break;

    case EV_DTM_PERFORM_GSM_RELEASE_GTTP:
      s = "EV_DTM_PERFORM_GSM_RELEASE_GTTP";
      break;

    case EV_DTM_CS_EST_CNF:
      s = "EV_DTM_CS_EST_CNF";
      break;

    case EV_DTM_PS_EST_CNF:
      s = "EV_DTM_PS_EST_CNF";
      break;

    case EV_DTM_CSPS_EST_CNF:
      s = "EV_DTM_CSPS_EST_CNF";
      break;

    case EV_DTM_RR_EST_CNF:
      s = "EV_DTM_RR_EST_CNF";
      break;

    case EV_DTM_RR_EST_FAIL:
      s = "EV_DTM_RR_EST_FAIL";
      break;

    case EV_DTM_ASSIGNMENT_COMMAND_RECEIVED:
      s = "EV_DTM_ASSIGNMENT_COMMAND_RECEIVED";
      break;

    case EV_DTM_PACKET_ASSIGNMENT_RECEIVED:
      s = "EV_DTM_PACKET_ASSIGNMENT_RECEIVED";
      break;

    case EV_DTM_REJECT_RECEIVED:
      s = "EV_DTM_REJECT_RECEIVED";
      break;

    case EV_DTM_ASSIGNMENT_COMMAND_CHECKED:
      s = "EV_DTM_ASSIGNMENT_COMMAND_CHECKED";
      break;

    case EV_DTM_PACKET_ASSIGNMENT_CHECKED:
      s = "EV_DTM_PACKET_ASSIGNMENT_CHECKED";
      break;

    case EV_DTM_ASSIGNMENT_ERROR:
      s = "EV_DTM_ASSIGNMENT_ERROR";
      break;

    case EV_DTM_ASSIGNMENT_FAILURE:
      s = "EV_DTM_ASSIGNMENT_FAILURE";
      break;

    case EV_DTM_ACCESS_REJECT:
      s = "EV_DTM_ACCESS_REJECT";
      break;

    case EV_DTM_ASSIGNMENT_RECEIVED:
      s = "EV_DTM_ASSIGNMENT_RECEIVED";
      break;

    case EV_DTM_ASSIGNMENT_COMPLETE:
      s = "EV_DTM_ASSIGNMENT_COMPLETE";
      break;

    case EV_DTM_T3148_EXPIRY:
      s = "EV_DTM_T3148_EXPIRY";
      break;

    case EV_DTM_SI6_RECEIVED:
      s = "EV_DTM_SI6_RECEIVED";
      break;

    case EV_DTM_INFORMATION_RECEIVED:
      s = "EV_DTM_INFORMATION_RECEIVED";
      break;

    case EV_DTM_CELL_SUPPORTS_DTM:
      s = "EV_DTM_CELL_SUPPORTS_DTM";
      break;

    case EV_DTM_CELL_DOES_NOT_SUPPORT_DTM:
      s = "EV_DTM_CELL_DOES_NOT_SUPPORT_DTM";
      break;

    case EV_DTM_ACCESS_REJECT_T3142_RUNNING:
      s = "EV_DTM_ACCESS_REJECT_T3142_RUNNING";
      break;

    case EV_DTM_L2_DATA_ACK_IND:
      s = "EV_DTM_L2_DATA_ACK_IND";
      break;

#ifdef FEATURE_WCDMA
    case EV_DTM_W2G_HO:
      s = "EV_DTM_W2G_HO";
      break;

    case EV_DTM_W2G_HO_PS_ATTACHED:
      s = "EV_DTM_W2G_HO_PS_ATTACHED";
      break;
#endif // FEATURE_WCDMA

    case EV_DTM_UL_TBF_REQ:
      s = "EV_DTM_UL_TBF_REQ";
      break;

    case EV_DTM_NO_RESPONSE_TO_DTM_REQUEST:
      s = "EV_DTM_NO_RESPONSE_TO_DTM_REQUEST";
      break;

    case EV_DTM_ASSIGNMENT_READY:
      s = "EV_DTM_ASSIGNMENT_READY";
      break;

    case EV_DTM_PS_ACCESS_REQ:
      s = "EV_DTM_PS_ACCESS_REQ";
      break;

    case EV_DTM_PS_ACCESS_REQ_PURGE:
      s = "EV_DTM_PS_ACCESS_REQ_PURGE";
      break;

    case EV_DTM_NO_PS_ACCESS_REQ:
      s = "EV_DTM_NO_PS_ACCESS_REQ";
      break;

    #ifdef FEATURE_GSM_EDTM
    case EV_EDTM_DTM_ASSIGNMENT_RECEIVED:
      s = "EV_EDTM_DTM_ASSIGNMENT_RECEIVED";
      break;

    case EV_EDTM_IMM_ASSIGNMENT_RECEIVED:
      s = "EV_EDTM_IMM_ASSIGNMENT_RECEIVED";
      break;

    case EV_EDTM_IMM_ASSIGNMENT_REJECT_RECEIVED:
      s = "EV_EDTM_IMM_ASSIGNMENT_REJECT_RECEIVED";
      break;

    case EV_EDTM_ESTABLISHMENT_STARTED:
      s = "EV_EDTM_ESTABLISHMENT_STARTED";
      break;

    case EV_EDTM_ESTABLISHMENT_REJECTED:
      s = "EV_EDTM_ESTABLISHMENT_REJECTED";
      break;

    case EV_EDTM_ESTABLISHMENT_COMPLETED:
      s = "EV_EDTM_ESTABLISHMENT_COMPLETED";
      break;

    case EV_EDTM_ESTABLISHMENT_ABORTED:
      s = "EV_EDTM_ESTABLISHMENT_ABORTED";
      break;

    case EV_EDTM_T3196_EXPIRY:
      s = "EV_EDTM_T3196_EXPIRY";
      break;

    #endif /* FEATURE_GSM_EDTM */
    #endif /* FEATURE_GSM_DTM */

    /**
     * PCCO events.
     */
    case EV_PCCO_FAILED:
      s = "EV_PCCO_FAILED";
      break;

    case EV_PS_ACCESS_IND:
      s = "EV_PS_ACCESS_IND";
      break;

    case EV_GMM_LEFT_READY_STATE:
      s = "EV_GMM_LEFT_READY_STATE";
      break;

    case EV_SI_ACTIVATE_CAMPED_HANDLING:
      s = "EV_SI_ACTIVATE_CAMPED_HANDLING";
      break;

    case EV_SI_SYS_INFO_MESSAGE_RECEIVED:
      s = "EV_SI_SYS_INFO_MESSAGE_RECEIVED";
      break;

#ifdef FEATURE_GPRS_PS_HANDOVER
    case EV_ACTION_PSHO:
      s = "EV_ACTION_PSHO";
      break;

    case EV_PSHO_STARTED:
      s = "EV_PSHO_STARTED";
      break;

    case EV_PSHO_CONTINUE:
      s = "EV_PSHO_CONTINUE";
      break;

    case EV_PSHO_REJECTED:
      s = "EV_PSHO_REJECTED";
      break;

    case EV_PSHO_COMPLETE:
      s = "EV_PSHO_COMPLETE";
      break;
#endif  /* FEATURE_GPRS_PS_HANDOVER */

    case EV_CHOOSE_CELL_BY_CELL_SELECTION_INDICATOR:
      s = "EV_CHOOSE_CELL_BY_CELL_SELECTION_INDICATOR";
      break;

    case EV_START_G2W_REDIRECTION:
      s = "EV_START_G2W_REDIRECTION";
      break;

    case EV_TRIGGER_CELL_SELECTION_BY_REL_IND:
      s = "EV_TRIGGER_CELL_SELECTION_BY_REL_IND";
      break;

#ifdef FEATURE_LTE
    case EV_G2L_BLIND_REDIR_REQUIRED:
      s = "EV_G2L_BLIND_REDIR_REQUIRED";
      break;
#endif /* FEATURE_LTE */

    case EV_PERFORM_MULTIPASS_SEARCH:
      s = "EV_PERFORM_MULTIPASS_SEARCH";
      break;

    case EV_MULTIPASS_SEARCH_FAILED:
      s = "EV_MULTIPASS_SEARCH_FAILED";
      break;

    case EV_PLMN_SEARCH_START:
      s = "EV_PLMN_SEARCH_START";
      break;

    case EV_PLMN_SEARCH_ABORT:
      s = "EV_PLMN_SEARCH_ABORT";
      break;

    case EV_PLMN_SEARCH_COMPLETE:
      s = "EV_PLMN_SEARCH_COMPLETE";
      break;

    case EV_PLMN_SEARCH_SUSPEND:
      s = "EV_PLMN_SEARCH_SUSPEND";
      break;

#ifdef FEATURE_GSM_TDS
    case EV_PLMN_SEARCH_START_TTOG:
      s = "EV_PLMN_SEARCH_START_TTOG";
      break;
#endif /*FEATURE_GSM_TDS*/

    case EV_PLMN_LIST_SEARCH_GUARD_TIMER_EXPIRED:
      s = "EV_PLMN_LIST_SEARCH_GUARD_TIMER_EXPIRED";
      break;

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
    case EV_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER_EXPIRED:
      s = "EV_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER_EXPIRED";
      break;
#endif // FEATURE_MPLMN_PARTIAL_UPDATING

     case EV_DECODE_BCCH_LIST_UPDATE:
      s = "EV_DECODE_BCCH_LIST_UPDATE";
      break;


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case EV_X2G_CCO_FAILURE_CCO_TIMER_EXPIRY:
      s = "EV_X2G_CCO_FAILURE_CCO_TIMER_EXPIRY";
      break;

    case EV_X2G_CCO_FAILURE_NO_RESPONSE:
      s = "EV_X2G_CCO_FAILURE_NO_RESPONSE";
      break;

    case EV_X2G_CCO_FAILURE_IA_REJECT:
      s = "EV_X2G_CCO_FAILURE_IA_REJECT";
      break;

    case EV_X2G_CCO_FAILURE_CONTENTION_RESOLUTION_FAILURE:
      s = "EV_X2G_CCO_FAILURE_CONTENTION_RESOLUTION_FAILURE";
      break;

    case EV_X2G_CCO_FAILURE_DSF:
      s = "EV_X2G_CCO_FAILURE_DSF";
      break;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

    case EV_PANIC_RESET_REQ:
      s = "EV_PANIC_RESET_REQ";
      break;

    case EV_PANIC_RESET_CNF:
      s = "EV_PANIC_RESET_CNF";
      break;

    case EV_PANIC_RESET_CNF_RR_ACTIVATION_IND_SENT:
      s = "EV_PANIC_RESET_CNF_RR_ACTIVATION_IND_SENT";
      break;

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
    case EV_G2T_PSEUDO_FR_REQUIRED:
      s = "EV_G2T_PSEUDO_FR_REQUIRED";
      break;
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

#ifdef FEATURE_SGLTE
    case EV_SGLTE_X2G_CCO_FAILED_GO_TO_IDLE:
      s = "EV_SGLTE_X2G_CCO_FAILED_GO_TO_IDLE";
      break;
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_IDLE_DRX_SCALING
    case EV_IDLE_DRX_DSC_BELOW_TH_LOW:
      s = "EV_IDLE_DRX_DSC_BELOW_TH_LOW";
      break;

    case EV_IDLE_DRX_DSC_ABOVE_TH_HIGH:
      s = "EV_IDLE_DRX_DSC_ABOVE_TH_HIGH";
      break;

    case EV_IDLE_DRX_SCALING_SUSPEND_TIMEOUT:
      s = "EV_IDLE_DRX_SCALING_SUSPEND_TIMEOUT";
      break;

    case EV_IDLE_DRX_BS_PA_MFRMS_CHANGED:
      s = "EV_IDLE_DRX_BS_PA_MFRMS_CHANGED";
      break;
#endif /* FEATURE_IDLE_DRX_SCALING */

    default:
      MSG_GERAN_ERROR_1("Unknown event: 0x%04x", event);
      s = "(Unknown EV)";
  }

  return s;

} /* end rr_event_name */

/*!
 * \brief Returns a text string representation if the L2 channel indicated.
 * 
 * \param l2_channel 
 * 
 * \return const char* 
 */
const char *rr_gprs_l2_channel_name(const l2_channel_type_T l2_channel)
{
  switch (l2_channel)
  {
    case DCCH: return "DCCH";
    case BCCH: return "BCCH";
    case L2_RACH: return "L2_RACH";
    case CCCH: return "CCCH";
    case SACCH: return "SACCH";
    case SDCCH: return "SDCCH";
    case FACCH_F: return "FACCH_F";
    case FACCH_H: return "FACCH_H";
    default: return "Unknown channel";
  }
}

/*!
 * \brief Log a message received from MM, MAC, L1, WRRC, TDSRRC & MSGR (e.g. LRRC).
 * 
 * \param message_set (in)
 * \param message_id (in)
 * \param pended_count (in)
 * \param gas_id (in)
 */
void rr_debug_log_imsg(
  uint8          message_set,
  uint32         message_id,
  uint32         pended_count,
  const gas_id_t gas_id
)
{
  switch (message_set)
  {
    case MS_MM_RR:
    {
      rr_debug_log_mm_rr_message(
        (mm_rr_message_id_enum_T) message_id,
        pended_count,
        gas_id
      );
      break;
    } /* MS_MM_RR */

    case MS_MAC_RR:
    {
      rr_debug_log_mac_rr_message(
        (mac_grr_sig_id_t) message_id,
        pended_count,
        gas_id
      );
      break;
    } /* MS_MAC_RR */

    case MS_RR_L1:
    {
      rr_debug_log_rr_l1_message(
        (rr_l1_message_id_enum_T) message_id,
        pended_count,
        gas_id
      );
      break;
    } /* MS_RR_L1 */

#ifdef FEATURE_WCDMA
    case MS_RRC_RR:
    {
      rr_debug_log_rrc_rr_message(
        (rrc_rr_cmd_e_type) message_id,
        pended_count,
        gas_id
      );
      break;
    } /* MS_RRC_RR */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case MS_TDSRRC_RR:
    {
      rr_debug_log_tdsrrc_rr_message(
        (tdsrr_rrc_cmd_e_type) message_id,
        pended_count,
        gas_id
      );
      break;
    } /* MS_TDSRRC_RR */
#endif /* FEATURE_GSM_TDS */

    case MS_MSGR_RR:
    {
      rr_debug_log_msgr_rr_message(
        (msgr_umid_type) message_id,
        pended_count,
        gas_id
      );
      break;
    } /* MS_MSGR_RR */

    default:
    {
    }
  }
}

/*!
 * \brief Log a message sent to L1 to MM. (Note: Only messages with IMH_T format can be logged here).
 * 
 * \param message_header (in)
 * \param gas_id (in)
 */
void rr_debug_log_omsg(
  IMH_T          * message_header,
  const gas_id_t   gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(message_header);

  switch (message_header->message_set)
  {
    case MS_RR_L1:
    {
      rr_debug_log_rr_l1_message(
        (rr_l1_message_id_enum_T) message_header->message_id,   // message_id
        0,                                                      // pended_count
        gas_id                                                  // gas_id
      );
      break;
    } /* MS_RR_L1 */

    case MS_MM_RR:
    {
      rr_debug_log_mm_rr_message(
        (mm_rr_message_id_enum_T) message_header->message_id,   // message_id
        0,                                                      // pended_count
        gas_id                                                  // gas_id
      );
      break;
    } /* MS_MM_RR */

    default:
    {
    }
  }
}

#ifdef FEATURE_WCDMA

/*!
 * \brief Logs a message sent to WCDMA RRC.
 * 
 * \param rrc_rr_cmd (in)
 * \param gas_id (in)
 */
void rr_debug_log_rrc_rr_omsg(
  rrc_rr_cmd_type * rrc_rr_cmd,
  const gas_id_t    gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(rrc_rr_cmd);
  rr_debug_log_rrc_rr_message(
    rrc_rr_cmd->cmd_hdr.cmd_id,   // message_id
    0,                             // pended_count
    gas_id
  );
}

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS

/*!
 * \brief Logs a message sent to TDS RRC.
 * 
 * \param tdsrrc_rr_cmd (in)
 * \param gas_id (in
 */
void rr_debug_log_tdsrrc_rr_omsg(
  tdsrrc_rr_cmd_type * tdsrrc_rr_cmd,
  const gas_id_t       gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(tdsrrc_rr_cmd);
  rr_debug_log_tdsrrc_rr_message(
    tdsrrc_rr_cmd->cmd_hdr.cmd_id,  // message_id
    0,                              // pended_count
    gas_id
  );
}

#endif /* FEATURE_GSM_TDS */

/*!
 * \brief Logs a message sent via MSGR (e.g. to LTE RRC).
 * 
 * \param msgr_hdr (in)
 * \param gas_id (in
 */
void rr_debug_log_msgr_rr_omsg(
  msgr_hdr_struct_type * msgr_hdr,
  const gas_id_t         gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(msgr_hdr);
  rr_debug_log_msgr_rr_message(
    msgr_hdr->id,                 // message_id
    0,                            // pended_count
    gas_id
  );
}

/*!
 * \brief Logs a message sent to MAC.
 * 
 * \param msg_id (in)
 * \param gas_id (in)
 */
void rr_debug_log_rr_mac_omsg(grr_mac_sig_id_t msg_id, const gas_id_t gas_id)
{
  rr_debug_rr_mac_trace_buffer_entry_t * entry;
  uint32 timestamp_fn = GSTMR_GET_FN_GERAN(gas_id);
  timetick_type timestamp_clk = timetick_get();

  // Obtain a pointer to the module data
  rr_gprs_debug_data_t *rr_gprs_debug_data_ptr = rr_gprs_debug_get_data_ptr(gas_id);

  if (rr_gprs_debug_data_ptr->rr_mac_trace_buffer_index >= RR_DEBUG_RR_MAC_TRACE_BUFFER_SIZE)
  {
    rr_gprs_debug_data_ptr->rr_mac_trace_buffer_index = 0;
  }

  entry = &rr_gprs_debug_data_ptr->rr_mac_trace_buffer[rr_gprs_debug_data_ptr->rr_mac_trace_buffer_index];

  entry->timestamp_fn = timestamp_fn;
  entry->timestamp_clk = timestamp_clk;
  entry->message_id = msg_id;

  ++rr_gprs_debug_data_ptr->rr_mac_trace_buffer_index;
}

/*!
 * \brief Logs an instance of panic reset.
 * 
 * \param gas_id 
 */
void rr_debug_log_panic_reset(const gas_id_t gas_id)
{
  uint32 timestamp = GSTMR_GET_FN_GERAN(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_debug_data_t *rr_gprs_debug_data_ptr = rr_gprs_debug_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_debug_data_ptr);

  if (rr_gprs_debug_data_ptr->panic_reset_trace_buffer_index >= RR_DEBUG_PANIC_RESET_TRACE_BUFFER_SIZE)
  {
    rr_gprs_debug_data_ptr->panic_reset_trace_buffer_index = 0;
  }

  rr_gprs_debug_data_ptr->panic_reset_trace_buffer[rr_gprs_debug_data_ptr->panic_reset_trace_buffer_index] = timestamp;

  ++rr_gprs_debug_data_ptr->panic_reset_trace_buffer_index;
}

/* EOF */

