/*============================================================================
  FILE:         rr_resel.c

  OVERVIEW:     RR main reselection state machine.

  DEPENDENCIES: None

                Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_resel.c#1 $ */


/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_resel.h"
#include "rr_resel_utils.h"
#include "rr_task.h"
#include "rr_gprs_defs.h"
#include "rr_general.h"
#include "rr_resel_calcs.h"
#include "rr_g2w.h"
#include "rr_defs.h"
#include "rr_log.h"
#include "rr_resel_if.h"
#include "rr_cell_acq_if.h"
#include "rr_gprs_defs.h"
#include "rr_l1_send.h"
#include "rr_conn_establish.h"
#include "rr_gprs_conn_establish.h"
#include "rr_mac_send.h"
#include "rr_psho.h"
#include "rr_mm_send.h"
#include "rr_pscan.h"
#include "rr_sys_info_i.h"
#include "rr_grr.h"
#include "rr_cell_acq.h"
#include "rr_sys_info_ncell.h"
#include "rr_sys_info_pcell.h"
#include "rr_sys_info_si.h"
#include "rr_resel_nacc.h"
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
#include "rr_resel_g2w.h"
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#ifdef FEATURE_WCDMA
#include "rrcrrif.h"
#endif /* FEATURE_WCDMA */
#include "rr_decode_bcch.h"
#include "rr_g2w.h"
#include <stdlib.h>

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
#include "rr_g2x.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/
#ifdef FEATURE_IDLE_DRX_SCALING
#include "rr_idle_drx.h"
#endif /* FEATURE_IDLE_DRX_SCALING */

#include "rr_multi_sim.h"
#include "rr_ded_meas_reporting.h"
#include "gprs_mem.h"
#include "rr_l1_idle_mode.h"
#include "rr_ps_access.h"
#include "rr_ps_domain.h"
#include "rr_gprs_meas_rpt_control.h"
#include "rr_main.h"

/*----------------------------------------------------------------------------
 * Local Defines
 *--------------------------------------------------------------------------*/

/* bit masks to handle reception of measurements */
#define RR_NO_MEASUREMENTS_RECEIVED 0x00
#define RR_MASK_GSM_SURROUND_MEAS   0x01
#define RR_MASK_GSM_SERV_MEAS       0x02
#define RR_MASK_WCDMA_SURROUND_MEAS 0x04

/* Number of cells included in BCCH decode for each loop after performing
reselection power scan. */
#define MAX_NO_OF_RPS_CELLS_TO_TRY_PER_CYCLE 3

/*----------------------------------------------------------------------------
 * Local Types
 *--------------------------------------------------------------------------*/
typedef enum
{
  RR_RESEL_STATE_IDLE,
  RR_RESEL_STATE_GOING_INACTIVE,

  /* C1 reselection states */
  RR_RESEL_STATE_C1,
  RR_RESEL_STATE_C1_PENDING_L1_IDLE_CNF,
  RR_RESEL_STATE_C1_ABORTING_CELL_ACQ,

  /* C2 reselection states */
  RR_RESEL_STATE_C2_PENDING_MAC_SUSPENSION,
  RR_RESEL_STATE_C2,
  RR_RESEL_STATE_C2_PENDING_L1_IDLE_CNF,
  RR_RESEL_STATE_C2_ABORTING_CELL_ACQ,

  /* NACC related states */
  RR_RESEL_STATE_NACC,
  RR_RESEL_STATE_NACC_PENDING_NW_RESPONSE,
  RR_RESEL_STATE_NACC_ABORTING_CELL_ACQ,
  RR_RESEL_STATE_NACC_PENDING_L1_IDLE_CNF,

  /* Reselection Power Scan related states */
  RR_RESEL_STATE_RPS,
  RR_RESEL_STATE_RPS_ABORTED,
  RR_RESEL_STATE_RPS_PENDING_L1_IDLE_CNF,

  /* PCCO related states */
  RR_RESEL_STATE_PCCO,
  RR_RESEL_STATE_PCCO_ABORTING_CELL_ACQ,
  RR_RESEL_STATE_PCCO_PENDING_L1_IDLE_CNF,

  /* PCCO failure related states */
  RR_RESEL_STATE_PCCO_FAILED,
  RR_RESEL_STATE_PCCO_FAILED_ABORTING_CELL_ACQ,
  RR_RESEL_STATE_PCCO_FAILED_PENDING_L1_IDLE_CNF,

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  /* G2W reselection/PCCO related states */
  RR_RESEL_STATE_G2W,
  RR_RESEL_STATE_G2W_FAILED,
  RR_RESEL_STATE_G2W_FAILED_PENDING_L1_IDLE_CNF,
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  RR_RESEL_STATE_MAX
} rr_resel_state_e;

typedef enum
{
  RR_RESEL_ALGO_UNDEFINED,
  RR_RESEL_ALGO_RANK_BASED,
#ifdef FEATURE_LTE
  RR_RESEL_ALGO_PRIORITY_BASED,
#endif /* FEATURE_LTE */
  RR_RESEL_ALGO_MAX
} rr_resel_algo_e;

typedef enum{
  MS_CONTROL,
  NETWORK_CONTROL
} rr_resel_gprs_resel_cntrl_e;

typedef enum{
  START_C1_RESEL_FAILED,
  START_C1_RESEL_CONTINUE
} rr_resel_start_c1_status_e;

typedef enum{
  NACC_RESEL_FROM_BCCH_CELL,
  RESEL_FROM_BCCH_CELL,
  RESEL_FROM_BCCH_WITH_GPRS_CELL_SELECTION_PARAMS,
  RESEL_NO_CELLS_VALID_FOR_RESEL
} rr_resel_start_c2_status_e;

typedef enum{
  START_C2_TARGET_BEST_CELL,
  START_C2_SELECT_NEXT_BEST_CELL
} rr_resel_start_c2_cause;

typedef enum{
  ABORT_CELL_ACQ_RESEL_TIMEOUT,
  ABORT_CELL_ACQ_COMMANDED_ABORT,
  ABORT_CELL_ACQ_RESEL_REQUEST
} rr_resel_abort_cell_acq_cause_e;

typedef enum
{
  RESEL_NO_RESEL,
  RESEL_C1_CHOOSE_CELL,
  RESEL_C1_TARGET_CELL_ALREADY_SELECTED,
  RESEL_C2_CHOOSE_CELL,
  RESEL_C2_SELECT_NEXT_BEST_CELL,
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  RESEL_G2W_NORMAL,
  RESEL_G2W_PCCO,
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  RESEL_TYPE_MAX
} rr_resel_start_resel_type_T;

typedef enum
{
  RR_RESEL_RPS_IDLE,
  RR_RESEL_RPS_POWER_SCAN,
  RR_RESEL_RPS_DECODE_BCCH,
  RR_RESEL_RPS_CELL_ACQ
} rr_resel_rps_sub_state_e;

typedef enum
{
  PBCCH_ACQUIRED,
  BCCH_ACQUIRED
} start_idle_type_e;

typedef enum
{
  RESEL_COMPLETE_C1,
  RESEL_COMPLETE_C2,
  RESEL_COMPLETE_REACQ_AFTER_G2W_FAILURE
} rr_resel_complete_resel_type_e;

typedef enum
{
  NACC_CELL_SUITABLE_FOR_RESEL,
  NACC_CELL_NOT_SUITABLE_FOR_RESEL
} nacc_surr_check_result_e;
typedef enum
{
  NO_ABORT,
  GRR_CONTROL_ABORTED,
  RR_TIMER_EXPIRY
} rr_reason_for_rps_aborted_e;

typedef enum
{
  PCCO_TRIGGERED_THROUGH_NC2,
  PCCO_TRIGGERED_THROUGH_NACC,
  NO_PCCO_TRIGGERED
} rr_resel_pcco_reason_e;

typedef enum
{
  PCCO_NO_ABORT,
  PCCO_CELL_RESELECTION_TIMER_EXPIRY,
  PCCO_T3174_TIMER_EXPIRY,
  PCCO_T3176_TIMER_EXPIRY,
  PCCO_GRR_CONTROL_ABORTED
} rr_reason_for_pcco_aborted_e;


typedef struct
{
  rr_resel_rps_sub_state_e              rps_sub_state;
  rr_resel_rps_sub_state_e              old_rps_sub_state;
  rr_reselection_pscan_cell_list_T      pscan_cell_list;
  rr_reason_for_rps_aborted_e           reason_for_rps_aborted;
} rr_resel_rps_data_t;

typedef struct
{
  rr_resel_pcco_reason_e                pcco_reason;
  rr_reason_for_pcco_aborted_e          reason_for_pcco_aborted;
  packet_cell_change_failure_t          pccf;
} rr_resel_pcco_data_t;

typedef struct
{
  ARFCN_T                       BCCH_ARFCN;
  BSIC_T                        BSIC;
  boolean                       control_channel_description_valid;
  control_channel_description_T control_channel_description;
} old_scell_t;

typedef struct
{
  rr_resel_state_e      state;
  rr_resel_state_e      old_state;

  /* This is a bitmap of the measurements that are required before
  RR executes it's reselection algorithms. */
  uint8                 measurements_expected;

  /* This is a bitmap of the measurements that have been received. */
  uint8                 measurements_received;

  boolean               gprs_parameters_available_flag;

  /* Indicates whether reselection is currently under the control of
  the network or UE. */
  rr_resel_gprs_resel_cntrl_e rr_gprs_reselection_control;

  old_scell_t           old_scell;

  boolean               old_scell_arfcn_valid;
  ARFCN_T               old_scell_arfcn;
  LAI_T                 old_scell_LAI;

  rr_resel_abort_cell_acq_cause_e abort_cell_acq_cause;

  rr_resel_start_resel_type_T resel_type;

  rr_resel_rps_data_t   rps_data;
  rr_resel_pcco_data_t  pcco_data;

  better_cell_entry_T * rr_gprs_target_cell;
  better_cell_entry_T   rr_gprs_old_scell;

  boolean               recent_reselection_penalty_flag;

  packet_cell_change_notification_t pccn;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  rr_irat_cell_t        g2x_target;
  rr_g2x_resel_type_e   g2x_type;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  boolean nacc_pccc_received; /* Indicates when a PCCC is received in RR_RESEL_STATE_NACC_PENDING_NW_RESPONSE */
  boolean nacc_pcco_received; /* Indicates when a PCCO is received in RR_RESEL_STATE_NACC_PENDING_NW_RESPONSE */

  boolean rr_resel_c2_pend_abort_req_rxd; /*Indicates that a RR_RESEL_IMSG_ABORT_REQ was received in RR_RESEL_STATE_C2_PENDING_MAC_SUSPENSION.
                                            Used to indicate that a RR_RESEL_OMSG_ABORT_CNF should be sent if the reselection fails
                                            before moving to RR_RESEL_STATE_C2 state*/

  boolean pccf_transmission_in_progress;
  rr_resel_cause_e    cause;

} rr_resel_data_t;

typedef enum
{
  RR_RESEL_CLEAR_SI_DB,
  RR_RESEL_DONT_CLEAR_SI_DB
} rr_resel_clear_si_e;

/*----------------------------------------------------------------------------
 * Public Variables
 *--------------------------------------------------------------------------*/

/* Array of possible values of cell reselection offset, the GPRS_RESELECTION_OFFSET value
should be used as the index into the array*/
extern const int GPRS_RESELECT_OFFSET_dB[32];

/*----------------------------------------------------------------------------
 * Local Variables
 *--------------------------------------------------------------------------*/
static rr_resel_data_t rr_resel_data_spaces[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_resel_data_t*
 */
static rr_resel_data_t *rr_resel_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_resel_data_spaces[as_index];
}

/**
  @brief Checks whether the specified state is expected to be transitory or
         not.

  The reselection state machine has a number of states which are only expected
  to be entered for a brief duration, e.g. when starting idle mode and waiting
  for a response from L1.  This function can be used to check whether a
  reselection state is one of these transitory states.

  @param state The state to check.
  @return TRUE if the state is transitory, FALSE if not.
*/
static boolean rr_resel_control_state_is_transitory(
  rr_resel_state_e state
)
{
  switch (state)
  {
    case RR_RESEL_STATE_GOING_INACTIVE:
    case RR_RESEL_STATE_C1_PENDING_L1_IDLE_CNF:
    case RR_RESEL_STATE_C1_ABORTING_CELL_ACQ:
    case RR_RESEL_STATE_C2_PENDING_L1_IDLE_CNF:
    case RR_RESEL_STATE_C2_ABORTING_CELL_ACQ:
    case RR_RESEL_STATE_NACC_ABORTING_CELL_ACQ:
    case RR_RESEL_STATE_NACC_PENDING_L1_IDLE_CNF:
    case RR_RESEL_STATE_RPS_ABORTED:
    case RR_RESEL_STATE_RPS_PENDING_L1_IDLE_CNF:
    case RR_RESEL_STATE_PCCO_ABORTING_CELL_ACQ:
    case RR_RESEL_STATE_PCCO_PENDING_L1_IDLE_CNF:
    case RR_RESEL_STATE_PCCO_FAILED_ABORTING_CELL_ACQ:
    case RR_RESEL_STATE_PCCO_FAILED_PENDING_L1_IDLE_CNF:
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case RR_RESEL_STATE_G2W_FAILED_PENDING_L1_IDLE_CNF:
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
    {
      return TRUE;
    }

    default:
    {
      return FALSE;
    }
  }
}

/**
  @brief Determine the reselection algorithm that RR should use, based on
         system information.

  @return the reselection algorithm that should be used.
*/
static rr_resel_algo_e rr_resel_algo(
  const gas_id_t gas_id
)
{
#ifdef FEATURE_LTE
  if (rr_is_lte_supported(gas_id) &&
      rr_serving_cell_priority_parameters_ie_received(rr_gprs_get_scell_info(gas_id))) 
  {
    return RR_RESEL_ALGO_PRIORITY_BASED;
  }
  else
  {
    return RR_RESEL_ALGO_RANK_BASED;
  }
#else
  return RR_RESEL_ALGO_RANK_BASED;
#endif /* FEATURE_LTE */
}

/*===========================================================================

FUNCTION rr_resel_state_name

DESCRIPTION
  Returns constant string of the rr resele state name.

PARAMS
  state

DEPENDENCIES
  None

RETURN VALUE
  char * - constant string

===========================================================================*/
static char *rr_resel_state_name(
  rr_resel_state_e state
)
{
  switch (state)
  {
    case RR_RESEL_STATE_IDLE:
      return "IDLE";

    case RR_RESEL_STATE_GOING_INACTIVE:
      return "GOING_INACTIVE";

    case RR_RESEL_STATE_C1:
      return "C1";

    case RR_RESEL_STATE_C1_PENDING_L1_IDLE_CNF:
      return "C1_PENDING_L1_IDLE_CNF";

    case RR_RESEL_STATE_C1_ABORTING_CELL_ACQ:
      return "C1_ABORTING_CELL_ACQ";

    case RR_RESEL_STATE_C2_PENDING_MAC_SUSPENSION:
      return "C2_PENDING_MAC_SUSPENSION";

    case RR_RESEL_STATE_C2:
      return "C2";

    case RR_RESEL_STATE_C2_PENDING_L1_IDLE_CNF:
      return "C2_PENDING_L1_IDLE_CNF";

    case RR_RESEL_STATE_C2_ABORTING_CELL_ACQ:
      return "C2_ABORTING_CELL_ACQ";

    case RR_RESEL_STATE_NACC:
      return "NACC";

    case RR_RESEL_STATE_NACC_PENDING_NW_RESPONSE:
      return "NACC_PENDING_NW_RESPONSE";

    case RR_RESEL_STATE_NACC_ABORTING_CELL_ACQ:
      return "NACC_ABORTING_CELL_ACQ";

    case RR_RESEL_STATE_NACC_PENDING_L1_IDLE_CNF:
      return "NACC_PENDING_L1_IDLE_CNF";

    case RR_RESEL_STATE_RPS:
      return "RPS";

    case RR_RESEL_STATE_RPS_ABORTED:
      return "RPS_ABORTED";

    case RR_RESEL_STATE_RPS_PENDING_L1_IDLE_CNF:
      return "RPS_PENDING_L1_IDLE_CNF";

    case RR_RESEL_STATE_PCCO:
      return "PCCO";

    case RR_RESEL_STATE_PCCO_ABORTING_CELL_ACQ:
      return "PCCO_ABORTING_CELL_ACQ";

    case RR_RESEL_STATE_PCCO_PENDING_L1_IDLE_CNF:
      return "PCCO_PENDING_L1_IDLE_CNF";

    case RR_RESEL_STATE_PCCO_FAILED:
      return "PCCO_FAILED";

    case RR_RESEL_STATE_PCCO_FAILED_ABORTING_CELL_ACQ:
      return "PCCO_FAILED_ABORTING_CELL_ACQ";

    case RR_RESEL_STATE_PCCO_FAILED_PENDING_L1_IDLE_CNF:
      return "PCCO_FAILED_PENDING_L1_IDLE_CNF";

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case RR_RESEL_STATE_G2W:
      return "G2W";

    case RR_RESEL_STATE_G2W_FAILED:
      return "G2W_FAILED";

    case RR_RESEL_STATE_G2W_FAILED_PENDING_L1_IDLE_CNF:
      return "G2W_FAILED_PENDING_L1_IDLE_CNF";
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

    default:
      MSG_GERAN_ERROR_1("Unknown reselection state: %d", state);
      return "?";
  }
} /* rr_resel_state_name() */

/*===========================================================================

FUNCTION rr_rps_state_name

DESCRIPTION
  Returns constant string of the rps_sub_state state name.

PARAMS
  state

DEPENDENCIES
  None

RETURN VALUE
  char * - constant string

===========================================================================*/
static char *rr_rps_state_name(
  rr_resel_rps_sub_state_e state
)
{
  switch (state)
  {
    case RR_RESEL_RPS_IDLE:
      return "RR_RESEL_RPS_IDLE";

    case RR_RESEL_RPS_POWER_SCAN:
      return "RR_RESEL_RPS_POWER_SCAN";

    case RR_RESEL_RPS_DECODE_BCCH:
      return "RR_RESEL_RPS_DECODE_BCCH";

    case RR_RESEL_RPS_CELL_ACQ:
      return "RR_RESEL_RPS_CELL_ACQ";

    default:
      MSG_GERAN_ERROR_1("Unknown reselection state: %d", state);
      return "?";
  }
} /* rr_rps_state_name() */


/**
 * Called to suspend GPRS. This uses the RR-PS-ACCESS module to provide this functionality.
 */
static void rr_resel_suspend_gprs(
  const gas_id_t gas_id
)
{
  // Send request to block PS access
  rr_ps_send_imsg_no_ps_access_req(
    FALSE,      // suspend_gmm
    NO_DEL,     // delete_pdu
    gas_id      // gas_id
  );

  // Once PS access is blocked, RR_PS_OMSG_NO_PS_ACCESS_CNF will be sent.
  // This is received in grr_control() and EV_MAC_IDLE_MODE is passed into rr_resel_control().
  // So RR_PS_OMSGs are not received into rr_resel_control().
}

/**
 * Function:     rr_gprs_restore_serving_cell_details;
 * Description:  Restores serving cell details previously stored by the
 *               function rr_gprs_store_serving_cell_details;
 * Parameters:   gprs_scell_info_t * - where to restore the saved details to.
 * Returns:      void;
 */
static void rr_gprs_restore_serving_cell_details(
  rr_resel_data_t *rr_resel_data_ptr,
  gprs_scell_info_t *dst,
  const gas_id_t gas_id
)
{
  if (rr_resel_data_ptr == NULL || dst == NULL)
  {
    MSG_GERAN_ERROR_2_G("rr_resel_data_ptr (%p) == NULL or dst (%p) == NULL", rr_resel_data_ptr, dst);
    return;
  }

  rr_clear_gprs_cell_info(dst);

  rr_init_pcell_data(gas_id);
  //rr_clear_system_information(dst->gsm);
  dst->gsm.BCCH_ARFCN = rr_resel_data_ptr->old_scell.BCCH_ARFCN;
  dst->gsm.BSIC = rr_resel_data_ptr->old_scell.BSIC;

  if (rr_resel_data_ptr->old_scell.control_channel_description_valid)
  {
    dst->gsm.control_channel_description_valid = TRUE;
    dst->gsm.control_channel_description = rr_resel_data_ptr->old_scell.control_channel_description;
  }
  else
  {
    dst->gsm.control_channel_description_valid = FALSE;
  }

  rr_eng_mode_update_scell_config(gas_id);

  dst->gsm.gprs_indicator = TRUE;
  dst->gsm.reselection_info.RXLEV_average = rr_resel_data_ptr->rr_gprs_old_scell.reselection_data.RXLEV_average;
}

static void rr_reselect_old_scell_after_pcco_failure(
  rr_resel_data_t *rr_resel_data_ptr,
  const gas_id_t gas_id
)
{
  if (rr_resel_data_ptr)
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
    gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

    /* Indicate that we are no longer camped on the serving cell.  This ensures
    that system information events are generated correctly when messages
    are received from the pending cell. */
    scell_info_ptr->gsm.camped_on_flag = FALSE;
    
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
      rr_set_rr_using_pseudo_si2q_data(FALSE, gas_id);
#endif

    rr_resel_data_ptr->rr_gprs_target_cell = &rr_resel_data_ptr->rr_gprs_old_scell;
    rr_gprs_restore_serving_cell_details(rr_resel_data_ptr, pcell_info_ptr, gas_id);

    /* hard coding this value so the cell acq starts selecting this cell*/
    rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.RXLEV_average = 63;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rr_resel_data_ptr == NULL");
  }
}

static void rr_reselection_build_power_scan_cell_list(
  rr_resel_data_t *rr_resel_data_ptr,
  const gas_id_t gas_id
)
{
  if (rr_resel_data_ptr)
  {
    gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
    BCCH_frequency_list_T         *ba_list       = NULL;
    inter_task_BCCH_allocation_T  *ba_gprs       = NULL;
    uint32                         index;
    uint32                         no_of_entries = 0;
    ARFCN_T                        ARFCN;
    gprs_better_cell_list_T       *better_cell_list_ptr = rr_get_better_cell_list_ptr(gas_id);

    if (scell_info_ptr)
    {
      /* build reselection pscan cell list from BA GPRS */
      ba_gprs = rr_gprs_get_ba_gprs(gas_id);
      no_of_entries = ba_gprs->no_of_entries;

      MSG_GERAN_MED_0_G("RPS using BA-GPRS");

      for (index = 0; index < no_of_entries; index++)
      {
        byte    better_cell_index;
        boolean include_entry = TRUE;

        if (ba_list != NULL)
        {
          ARFCN = ba_list->BCCH_ARFCN[index];
        }
        else if (ba_gprs != NULL)
        {
          ARFCN = ba_gprs->BCCH_ARFCN[index];
        }
        else
        {
          ARFCN.num = INVALID_ARFCN;
          ARFCN.band = SYS_BAND_NONE;
        }

        /*
        Check each entry in the BA list to see if it is also in the better cell
        list.  If it is, and reselection has already been attempted for this
        ARFCN, don't include it in the reselection power scan.
        */
        for (better_cell_index = 0;
             better_cell_index < better_cell_list_ptr->no_of_entries;
             ++better_cell_index)
        {
          if (ARFCNS_EQUAL(better_cell_list_ptr->candidate_cell[better_cell_index].reselection_data.BCCH_ARFCN, ARFCN) &&
              better_cell_list_ptr->candidate_cell[better_cell_index].cell_reselection_attempted)
          {
            include_entry = FALSE;
          }
        }

        /* Do not add forbidden cells to Reselection Power Scan list */
        if(include_entry && rr_cell_forbidden(ARFCN, gas_id))
        {
          include_entry = FALSE;
        }

        if (include_entry)
        {
          if (ARFCNS_EQUAL(ARFCN, rr_resel_data_ptr->rr_gprs_old_scell.reselection_data.BCCH_ARFCN))
          {
            /* Do not include the ARFCN as it matches the old serving cell's ARFCN.
            This will be included explicitly later on. */
          }
          else
          {
            MSG_GERAN_MED_1_G("adding ARFCN: %d", CONVERT_FROM_ARFCN_T(ARFCN));
            grr_reselection_pscan_cell_list_add_entry(
              &rr_resel_data_ptr->rps_data.pscan_cell_list,
              ARFCN
            );
          }
        }
      }

      /*
       * Include the serving cell in the reselection power scan if ARFCN is valid
       */
      ARFCN = rr_resel_data_ptr->rr_gprs_old_scell.reselection_data.BCCH_ARFCN;
      if (ARFCN.num != INVALID_ARFCN)
      {
        /* Do not add forbidden cells to Reselection Power Scan list */
        if(FALSE == rr_cell_forbidden(ARFCN, gas_id))       
        {
          MSG_GERAN_HIGH_1_G("Adding in the old scell ARFCN: %d", (int)ARFCN.num);
          grr_reselection_pscan_cell_list_add_entry(
            &rr_resel_data_ptr->rps_data.pscan_cell_list,
            ARFCN
          );
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("WARNING: ignoring old scell info: invalid ARFCN");
      }

      rr_resel_data_ptr->rps_data.pscan_cell_list.valid_data = TRUE;
    }
    else
    {
      MSG_GERAN_ERROR_0_G("scell_info_ptr == NULL");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rr_resel_data_ptr == NULL");
  }
}

static void rr_reselection_build_pscan_list_from_cell_select_indicator(
  rr_resel_data_t *rr_resel_data_ptr,
  const gas_id_t gas_id
)
{
  rr_chan_rel_cell_selection_indicator_t *rr_cell_select_indicator_ptr;
  uint32 index;
  rr_gsm_target_info_type *gsm_target;
  ARFCN_T ARFCN;

  // Obtain a pointer to the module data
  RR_NULL_CHECK_FATAL(rr_resel_data_ptr);

  rr_cell_select_indicator_ptr = rr_get_cell_select_indicator_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_select_indicator_ptr);

  if (rr_cell_select_indicator_ptr->type != GSM_TARGET)
  {
    MSG_GERAN_ERROR_1_G("cell_select_ind.type %d is not GSM target",
      rr_cell_select_indicator_ptr->type);

    rr_resel_data_ptr->rps_data.pscan_cell_list.valid_data = FALSE;
    return;
  }

  gsm_target = &rr_cell_select_indicator_ptr->target.gsm_target;
  for (index = 0; index < gsm_target->num_of_cells; index++)
  {
    ARFCN = gsm_target->arfcn[index];

    MSG_GERAN_MED_1_G("adding ARFCN: %d", CONVERT_FROM_ARFCN_T(ARFCN));

    grr_reselection_pscan_cell_list_add_entry(
      &rr_resel_data_ptr->rps_data.pscan_cell_list,
      ARFCN
    );
  }

  rr_resel_data_ptr->rps_data.pscan_cell_list.valid_data = TRUE;

  /* clean up the rr_cell_selection_indicator */
  rr_init_cell_selection_indicator(gas_id);
}


/****************************************************************************
 *   Function name:  grr_perform_reselection_power_scan
 *   -------------
 *
 *   Description:
 *   ------------
 *    A list of ARFCNs is constructed which is basically a copy of the current
 *    scells BA List. The scell may be included in this under certain conditions.
 *    This list is then sent down to L1 in the power scan request.
 *
 *   Parameters: none
 *   ----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
static void grr_perform_reselection_power_scan(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t             *scell_info        = rr_gprs_get_scell_info(gas_id);
  rr_available_plmn_db_T        *available_plmn_db = rr_get_available_plmn_db_ptr(gas_id);
  rr_candidate_cell_db_T        *candidate_cell_db = rr_get_candidate_cell_db_ptr(gas_id);
  gprs_better_cell_list_T       *better_cell_list_ptr = rr_get_better_cell_list_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_data_ptr);

  rr_eng_mode_update_scell_config(gas_id);

  /**********************************************************/
  /* if there were no candidate cells to go to then the old */
  /* scell data was never loaded up. Do this now            */
  /**********************************************************/
  if (better_cell_list_ptr->no_of_entries == 0)
  {
    rr_resel_data_ptr->rr_gprs_old_scell.reselection_data.BCCH_ARFCN = scell_info->gsm.BCCH_ARFCN;
    scell_info->gsm.BCCH_ARFCN.num = INVALID_ARFCN;
    scell_info->gsm.valid_data_flag = FALSE;
  }

  if (!rr_resel_data_ptr->rps_data.pscan_cell_list.valid_data)
  {
    rr_chan_rel_cell_selection_indicator_t *rr_cell_select_indicator_ptr;

    rr_cell_select_indicator_ptr = rr_get_cell_select_indicator_ptr(gas_id);

    if ((rr_cell_select_indicator_ptr->valid == TRUE) && (rr_cell_select_indicator_ptr->type == GSM_TARGET))
    {
      rr_reselection_build_pscan_list_from_cell_select_indicator(rr_resel_data_ptr, gas_id);
    }
    else
    {
      rr_reselection_build_power_scan_cell_list(rr_resel_data_ptr, gas_id);
    }
  }

  /* invalidate the available PLMN database and candidate cell database */
  rr_plmn_db_clear(available_plmn_db, gas_id);
  rr_candidate_db_clear(candidate_cell_db);
}

/**
  @brief Resets an rr_rps_cell_list structure.

  @param cell_list: Pointer to the structure to reset.
*/
static void rr_rps_cell_list_reset(
  rr_reselection_pscan_cell_list_T *cell_list
)
{
  if (cell_list == NULL)
  {
    MSG_GERAN_ERROR_0("cell_list is NULL");
    return;
  }

  memset(cell_list, 0, sizeof(rr_reselection_pscan_cell_list_T));
}

static boolean rr_rps_cell_list_contains(
  const rr_reselection_pscan_cell_list_T *cell_list,
  ARFCN_T arfcn
)
{
  if (cell_list == NULL)
  {
    return FALSE;
  }
  else
  {
    int i;
    for (i = 0; i < cell_list->no_of_entries &&
                i < RR_MAX_RES_PSCAN_CELL_LIST_SIZE; i++)
    {
      if (ARFCNS_EQUAL(arfcn, cell_list->cell_list[i]))
      {
        return TRUE;
      }
    }
    return FALSE;
  }
}

/**
  @brief Adds an ARFCN to the reselection power scan cell list structure.

  This function adds an ARFCN to the reselection power scan cell list structure
  provided, performing bounds checking where appropriate.

  @param cell_list: Pointer to the cell list to add the ARFCN to.
  @param arfcn:     The ARFCN to add to the cell list.
*/
static void rr_rps_cell_list_add(
  rr_reselection_pscan_cell_list_T *cell_list,
  ARFCN_T arfcn,
  const gas_id_t gas_id
)
{
  if (cell_list == NULL)
  {
    MSG_GERAN_ERROR_0_G("cell_list is NULL");
    return;
  }

  if (cell_list->no_of_entries < RR_MAX_RES_PSCAN_CELL_LIST_SIZE)
  {
    /* Only add unique frequencies to the RPS cell list */
    if (rr_rps_cell_list_contains(cell_list, arfcn) == FALSE)
    {
      MSG_GERAN_MED_2_G("Adding arfcn (%d-%d) to RPS cell list",
              arfcn.num,
              arfcn.band);
      cell_list->cell_list[cell_list->no_of_entries] = arfcn;
      cell_list->no_of_entries++;
    }
  }
  else
  {
    MSG_GERAN_ERROR_2_G("Overflow detected attempting to add ARFCN (%d,%d) to RPS cell list",
              arfcn.num,
              arfcn.band);
  }

  return;
}

/**
 Power scan request functions
*/

/* Debug to print out the RPS results to decide the reason for RPS failure */
static void rr_gprs_print_pscan_results(
  const gas_id_t gas_id
)
{
  rr_resel_data_t       *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  rr_pscan_results_db_T *rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);
  uint16                 i;

  /* Dump the results returned by L1: */
  for (i = 0; i < rr_pscan_results_db_ptr->pscan_results.num_frequencies; i++)
  {
    MSG_GERAN_MED_3_G("L1 res-> arfcn:%d band:%d rxlev:%d",
            rr_pscan_results_db_ptr->pscan_results.meas[i].arfcn.num,
            rr_pscan_results_db_ptr->pscan_results.meas[i].arfcn.band,
            rr_pscan_results_db_ptr->pscan_results.meas[i].RXLEV_average);
  }

  /* Dump the data that we based our request on: */
  if ( rr_resel_data_ptr->rps_data.pscan_cell_list.valid_data )
  {
    MSG_GERAN_MED_0_G("BA Pscan list Valid");
  }
  else
  {
    MSG_GERAN_MED_0_G("BA Pscan list Invalid");
  }

  for (i = 0; i < rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries; i++)
  {
    MSG_GERAN_MED_2_G("GRR BA-> arfcn:%d band:%d",
            rr_resel_data_ptr->rps_data.pscan_cell_list.cell_list[i].num,
            rr_resel_data_ptr->rps_data.pscan_cell_list.cell_list[i].band);
  }
}

/****************************************************************************
 *   Function name:  rr_perform_rps
 *   -------------
 *
 *   Description:
 *   ------------
 *    Function used to contruct the power_scan_req message to L1 with relevant
 *    band info.
 *
 *
 *   Parameters: none
 *   ----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
static void rr_perform_rps(
  rr_reselection_pscan_cell_list_T *frequencies_to_scan,
  const gas_id_t gas_id
)
{
  rr_pscan_results_db_T * rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);

  /* clear the candidate database and available plmn database since a new */
  /* power scan is being started from scratch and all the old results would be invalid */
  rr_plmn_db_clear(rr_get_available_plmn_db_ptr(gas_id), gas_id);
  rr_pscan_clear_results(rr_pscan_results_db_ptr);
  rr_candidate_db_clear(rr_get_candidate_cell_db_ptr(gas_id));
  rr_invalidate_power_scan_results(gas_id);
  (void)rr_timer_cancel(RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER, gas_id);

  /* setup the pscan statemachine*/
  rr_pscan_setup(gas_id);

  /* log the start of a power scan */
  rr_log_reselection_power_scan_started(gas_id);

  /* Truncating no_of_entries to RR_MAX_RES_PSCAN_CELL_LIST_SIZE for KW*/
  if(frequencies_to_scan->no_of_entries > RR_MAX_RES_PSCAN_CELL_LIST_SIZE)
  {
    frequencies_to_scan->no_of_entries = RR_MAX_RES_PSCAN_CELL_LIST_SIZE;
    MSG_GERAN_HIGH_2_G("Reselection PSCAN cell list size capped to %d from %d ",
                        RR_MAX_RES_PSCAN_CELL_LIST_SIZE,
                        frequencies_to_scan->no_of_entries
                      );
  }

  rr_pscan_add_frequencies(
    frequencies_to_scan->cell_list,
    (uint16)frequencies_to_scan->no_of_entries,
    gas_id
  );

  rr_pscan_send_start_ind(RR_SEARCH_RESELECTION, rr_band_pref(gas_id), gas_id);
} /* rr_perform_power_scan */


/****************************************************************************
 *   Function name:  grr_place_results_in_best_six_list
 *   -------------
 *
 *   Description:
 *   ------------
 *    Looks for the strongest 'n' frequencies in the pscan results buffer, and
 *    loads them into the best six database. The value of n is defined using
 *    MAX_NO_OF_RPS_CELLS_TO_TRY_PER_CYCLE.
 *
 *
 *   Parameters: none
 *   ----------
 *
 *   Returns:  the number of frequencies it managed to find
 *   --------
 *
 ****************************************************************************/
static uint8 grr_place_results_in_best_six_list(
  const gas_id_t gas_id
 )
{
  rr_resel_data_t        *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  rr_l1_freq_power_T      results_buffer[MAX_NO_OF_RPS_CELLS_TO_TRY_PER_CYCLE];
  uint8                   num_frequencies_to_find = (uint8) MIN(rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries, MAX_NO_OF_RPS_CELLS_TO_TRY_PER_CYCLE);
  uint8                   num_frequencies_found;
  rr_candidate_cell_T    *single_candidate;
  rr_candidate_cell_db_T *candidate_cell_db = rr_get_candidate_cell_db_ptr(gas_id);
  rr_pscan_results_db_T  *rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);

  num_frequencies_found = (uint8) rr_pscan_get_strongest_frequencies_in_list(
    rr_pscan_results_db_ptr,
    num_frequencies_to_find,
    rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries,
    rr_resel_data_ptr->rps_data.pscan_cell_list.cell_list,
    results_buffer
  );

  if( num_frequencies_found != 0 )
  {
    /*
    Start adding the top six ncells to the best 6 database.
    */
    int index;

    rr_remove_all_best_six_ncells(gas_id);
    rr_candidate_db_clear(candidate_cell_db);

    for (index = 0;
         (index < num_frequencies_found) && (index < MAX_NO_OF_RPS_CELLS_TO_TRY_PER_CYCLE);
         ++index)
    {
      ARFCN_T channel_number = results_buffer[index].arfcn;

      /* See if the entry exists already. It should always be there on bcch */
      bcch_ncell_info_t *new_ncell = rr_gprs_get_surround_ncell_sys_info(channel_number, gas_id);

      /* Create an entry for this cell it doesn't yet exist as an entry */
      if ( new_ncell == NULL )
      {
        MSG_GERAN_HIGH_1_G("Create new ncell entry %d",CONVERT_FROM_ARFCN_T(channel_number));

        rr_gprs_add_ncell_to_ba_gprs(channel_number, NULL, NULL, gas_id);

        new_ncell = rr_gprs_get_surround_ncell_sys_info(channel_number, gas_id);
      }

      /*
      remove the cell from the grr cell list regardless of whether it has an ncell entry so it
      cannot be re-attempted.
      */
      grr_reselection_pscan_cell_list_remove_entry(
        &rr_resel_data_ptr->rps_data.pscan_cell_list,
        channel_number
      );

      if (new_ncell != NULL)
      {
        rr_gprs_add_to_best_six(channel_number, new_ncell->gsm.BSIC, FALSE, gas_id);
        /* populate the candidate db with these cells */
        rr_candidate_db_add_frequency(candidate_cell_db, &results_buffer[index]);

        if ( new_ncell->gsm.valid_data_flag == TRUE )
        {
          /* Mark the cell as having valid bcch data */
          single_candidate = rr_candidate_db_get_entry(candidate_cell_db, channel_number);
          if (single_candidate == NULL)
          {
            MSG_GERAN_ERROR_0_G("rr_candidate_db_get_entry() returned NULL");
          }
          else
          {
            single_candidate->type = RR_CANDIDATE_VALID_BCCH;
          }
        }
      }
      else
      {
        MSG_GERAN_HIGH_1_G("Cannot create ncell entry %d",CONVERT_FROM_ARFCN_T(channel_number));
        continue;
      }
    }
  }

  return num_frequencies_found;
}

/****************************************************************************
 *   Function name:  grr_start_bcch_decode_list_procedure
 *   -------------
 *
 *   Description:
 *   ------------
 *    The results returned from a pscan are taken and the top n are loaded into
 *    the best 6 databse already. These are the first used during an attempt to camp.
 *    Once this has been done the chosen top n are checked to see if they have
 *    valid system info. For the ncells withoiut sys info, this is requested
 *    by the bcch_decode start ind.
 *
 *   Parameters: none
 *   ----------
 *
 *   Returns:  Boolean to tell whether bcch start ind sent or not
 *   --------
 *
 ****************************************************************************/
static boolean grr_start_bcch_decode_list_procedure(
  const gas_id_t gas_id
)
{
  boolean send_bcch_decode_list_req = FALSE;
  uint8 best_six_index;
  rr_decode_bcch_start_ind_t decode_bcch_start_ind;
  rr_candidate_cell_db_T * candidate_cell_db = rr_get_candidate_cell_db_ptr(gas_id);
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  RR_NULL_CHECK_FATAL(candidate_cell_db);

  memset((void *)&decode_bcch_start_ind, 0, sizeof(decode_bcch_start_ind));

  /* Clean out the frequency list */
  memset(&decode_bcch_start_ind.bcch_freq_list,0,sizeof(rr_l1_idle_freq_list_T)) ;

  /*
  The best six database has been loaded up with the top n ncells in order of signal strength.
  Look at the ncells to see if they have system info. For the cells without system info
  they must be added to the bcch decode list, this is stored in the variable frequency_list.
  */
  for (best_six_index=0;best_six_index<best_six_ncells_ptr->no_of_entries;best_six_index++)
  {
    if (best_six_ncells_ptr->ncell_entry[best_six_index].ncell_data->ncell_data.bcch.gsm.valid_data_flag == FALSE )
    {
      /* GRR requires L1 to get the sys info for this ncell */
      decode_bcch_start_ind.bcch_freq_list.arfcn[decode_bcch_start_ind.bcch_freq_list.num_frequencies++] =
        best_six_ncells_ptr->ncell_entry[best_six_index].ncell_data->arfcn;

      send_bcch_decode_list_req = TRUE;
    }
  }

  /*
  If sys info exists for all the ncells then GRR can miss out the bcch decode step otherwise
  L1 is instructed to start the bcch decode here
  */
  if  ( send_bcch_decode_list_req == TRUE )
  {
    rr_candidate_db_set_valid(candidate_cell_db, TRUE);
    decode_bcch_start_ind.bcch_decode_type = RR_SEARCH_RESELECTION;
    decode_bcch_start_ind.si3_only = FALSE;
    rr_decode_bcch_send_start_ind(decode_bcch_start_ind, gas_id);
  }
  /**
   * Always invalidate any previous results as we never want
   * to re-use power scan results from a reselection power scan.
   */
  rr_invalidate_power_scan_results(gas_id);

  return send_bcch_decode_list_req;
} /* rr_reselection_process_power_scan_results */

/****************************************************************************
 *   Function name:  rr_gprs_cell_reselection_complete
 *   -------------
 *
 *   Description: Contains calls to various functions needed upon completion
 *   ------------ of a successful reselection or RPS.
 *
 *
 *   Parameters: none
 *   ----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
static void rr_gprs_cell_reselection_complete(
  rr_resel_complete_resel_type_e resel_type,
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  /***************************************************************************************************/
  /* For non-C2 based Immediate reselections The old scell is made forbidden at this point unless    */
  /* grr is reselecting back to the old scell again. This is because a suitable cell has been found. */
  /* See 3gpp TS 05.08 6.6.2                                                                         */
  /***************************************************************************************************/

  if (resel_type == RESEL_COMPLETE_C1)
  {
    ARFCN_T old_arfcn = rr_resel_data_ptr->rr_gprs_old_scell.reselection_data.BCCH_ARFCN;
    ARFCN_T new_arfcn = rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN;

    if (rr_resel_data_ptr->cause != RR_RESEL_CAUSE_RESELECTION_TRM_DENIAL)
    {
      if (!ARFCNS_EQUAL(new_arfcn, old_arfcn) && ARFCN_VALID_CHANNEL(old_arfcn))
      {
        /*Add the old scell to the forbidden list*/
        rr_add_forbidden_cell(old_arfcn, gas_id);
        /*Start the forbidden list timer*/
        rr_start_forbidden_timer(gas_id);
      }
    }

  }

  /*cancel any 5sec timers still running*/
  rr_gprs_cancel_all_five_second_timers(gas_id);

  /* set up the bad snr counter value */
  rr_gprs_init_bad_snr_counter(gas_id);

  /* Stop the reselection timer*/
  if (rr_timer_cancel(CELL_RESELECTION_TIMER, gas_id))
  {
    MSG_GERAN_HIGH_0_G("RESELECTION TIMER STOPPED");
  }

  /* Clean out all but the last entry in the reselection list*/
  rr_gprs_reset_reselection_list(gas_id);

  /* 3GPP 44.060 section 8.8.2 states:

  The CCN mode is only valid in packet transfer mode or in MAC-Shared state.
  If the mobile station is in CCN mode when entering packet idle mode or
  MAC-Idle state, the mobile station shall stop the timers T3206 and T3208,
  stop timer T3210 if still running, leave CCN mode and continue the cell
  reselection procedure according to the NC0/NC1 procedures. If PACKET
  NEIGHBOUR CELL DATA messages are received on the PACCH before entering packet
  idle mode or MAC-Idle state and the cell identity parameters are included,
  this information may then be used at the next cell change.

  This implies that after completing a reselection any PNCDs should be
  released.
  */
  rr_gprs_nacc_reset(gas_id);

  /* Start the path loss timer again */
  if (rr_gprs_parameters_available(gas_id))
  {
    (void)rr_timer_start(PATH_LOSS_TIMER, GPRS_PATH_LOSS_TIMEOUT, gas_id);
    MSG_GERAN_LOW_0_G("Start the PATH LOSS TIMER");
  }

  /* If the recent reselection timer is still running cancel it */
  (void)rr_timer_cancel(RESELECTION_HYSTERESIS, gas_id);

  /* If the surround measurement timer is still running cancel it */
  (void)rr_timer_cancel(GRR_SURROUND_MEAS_TIMER, gas_id);

  /* now restart the recent reselection timer again from the start */
  (void)rr_timer_start(RESELECTION_HYSTERESIS, RESELECTION_HYSTERESIS_TIMEOUT, gas_id);

#ifdef FEATURE_IDLE_DRX_SCALING
  if (rr_idle_drx_start_scaling_suspend_timer(gas_id))
  {
    MSG_GERAN_HIGH_0_G("Started the IDLE_DRX_SCALING_SUSPEND TIMER");
  }
#endif /* FEATURE_IDLE_DRX_SCALING */

  rr_resel_data_ptr->recent_reselection_penalty_flag = TRUE;
  rr_resel_data_ptr->cause = RR_RESEL_CAUSE_MAX;

#ifdef FEATURE_WCDMA
  /*************************************************************/
  /* Initialise wcdma surrounding cell data base               */
  /*************************************************************/
  rr_g2wr_reset_wcdma_surrounding_database(gas_id);
  rr_g2wr_cancel_all_wcdma_five_second_timers(gas_id);
#endif /* FEATURE_WCDMA */
}

/****************************************************************************
 *   Function name:  rr_gprs_prepare_for_reselection_MFRM51
 *   -------------
 *
 *   Description: This function is the initial step to prepare for a reselection.
 *   ------------ If MAC hasn't been made aware that a reselection is about to
 *                take place it is sent the GRR_MAC_CELL_RESEL_IND.
 *                If MAC is then already in IDLE then the reselection is started
 *                by the cell reselection state machine upon reception of the
 *                output event EV_MAC_IDLE_MODE.
 *                Functionality is included to clear the system info databases,
 *                BA list and pending channel data store. Also the old scell may
 *                be added to the surround data store.
 *
 *   Parameters:  TRUE/FALSE flag to add the old scell to the surrounding cell
 *   -----------  database.
 *
 *   Returns:
 *   --------
 *
 ****************************************************************************/
static void rr_gprs_prepare_for_reselection_MFRM51(
  boolean add_scell_to_surrounding_database,
  boolean is_pscan_attempting,
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  gprs_scell_info_t             *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gsm_ncell_list_member_t       *chosen_cell;

  /*********************************************************************/
  /* Clean out BA list and SI received flags from pending channel data */
  /*********************************************************************/
  rr_initialize_SI2q_instances(gas_id);
  rr_initialize_SI2ter_instances(gas_id);

  /********************************************************************/
  /*Load the BSIC of the target cell into the pending channel database*/
  /********************************************************************/
  if ((chosen_cell = rr_gprs_get_best_six_by_arfcn(
         rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
         NULL,
         gas_id
       )) != NULL)
  {
    pcell_info_ptr->gsm.BSIC = chosen_cell->bsic;
  }
  else
  {
    MSG_GERAN_HIGH_1_G("No ncell entry for target cell %d",CONVERT_FROM_ARFCN_T(rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN));
  }

  /**************************************************************/
  /* If on CCCH....                                             */
  /* Remove the target cell from the ncell database. For all    */
  /* types of reselection use the SI3 info to early camp so     */
  /* transfer it using ncell_ccch_to_pcell.                     */
  /* If on PCCCH....                                            */
  /* No surround reads have been performed so there is no data  */
  /* to transfer for early camping. Also no target cell needs to*/
  /* be removed from the ncell bcch database.                   */
  /**************************************************************/
  if (rr_gprs_parameters_available(gas_id) == FALSE)
  {
    /* Always use the SI3 data for early camping */
      rr_ncell_ccch_to_pcell(
        rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
        gas_id
      );

    /* Take the entry out of the best six list */
    rr_gprs_remove_from_best_six(
      pcell_info_ptr->gsm.BCCH_ARFCN, pcell_info_ptr->gsm.BSIC,
      gas_id
    );

  }
  else
  {
    /* when reselecting across mfrms delete the best six completely */
    rr_remove_all_best_six_ncells(gas_id);
  }


  if ( add_scell_to_surrounding_database == TRUE &&
       scell_info_ptr->gsm.valid_data_flag &&
       !is_pscan_attempting )
  {
    bcch_ncell_info_t  *new_surround_db_entry_ptr;

    /**********************************************************/
    /* Add the old serving cell into the surrounding database */
    /**********************************************************/
    rr_gprs_add_ncell_to_ba_gprs(
      scell_info_ptr->gsm.BCCH_ARFCN,
      &scell_info_ptr->gsm.BSIC,
      NULL,
      gas_id
    );

    /* Load up the data for this new cell */
    new_surround_db_entry_ptr = rr_gprs_get_surround_ncell_sys_info(
      scell_info_ptr->gsm.BCCH_ARFCN,
      gas_id
    );

    if (new_surround_db_entry_ptr)
    {
      new_surround_db_entry_ptr->gsm.BSIC = scell_info_ptr->gsm.BSIC;

      /******************************************************************/
      /* Since system information for the old serving cell was current, */
      /* update the system information for the new surrounding entry    */
      /* for the old serving cell                                       */
      /******************************************************************/
      new_surround_db_entry_ptr->gsm.valid_data_flag = TRUE;
      new_surround_db_entry_ptr->gsm.update_message_received = scell_info_ptr->gsm.update_message_received;
      new_surround_db_entry_ptr->gsm.RACH_control_parameters = scell_info_ptr->gsm.RACH_control_parameters;
      new_surround_db_entry_ptr->gsm.cell_selection_parameters = scell_info_ptr->gsm.cell_selection_parameters;
      new_surround_db_entry_ptr->gsm.location_area_identification = scell_info_ptr->gsm.location_area_identification;
      new_surround_db_entry_ptr->gsm.control_channel_description = scell_info_ptr->gsm.control_channel_description;
      new_surround_db_entry_ptr->gsm.control_channel_description_valid = scell_info_ptr->gsm.control_channel_description_valid;

      new_surround_db_entry_ptr->gsm.si_expected = FALSE;
      new_surround_db_entry_ptr->gprs_supported = scell_info_ptr->gsm.gprs_indicator;
      new_surround_db_entry_ptr->gprs.ra_colour = scell_info_ptr->bcch_sys_info.ra_colour;

      new_surround_db_entry_ptr->system_info_timer_count = MAX_SYS_INFO_COUNT_VALUE;

      new_surround_db_entry_ptr->gsm.cell_id_valid = scell_info_ptr->gsm.cell_identity_valid_flag;
      new_surround_db_entry_ptr->gsm.cell_id = scell_info_ptr->gsm.cell_identity;

      /* Put the scell into the best 6 */
      rr_gprs_add_to_best_six(
        scell_info_ptr->gsm.BCCH_ARFCN,
        scell_info_ptr->gsm.BSIC,
        FALSE,
        gas_id
      );

      /*******************************************************************/
      /* Since this is the last serving cell being entered into the      */
      /* surrounding database GSM 05.08 indicates that the penalty timer */
      /* state should be created in an EXPIRED state (see GSM 05.08 6.4)  */
      /*******************************************************************/
      rr_gprs_create_expired_penalty_timer(
        rr_resel_best_six_find(
          scell_info_ptr->gsm.BCCH_ARFCN,
          &scell_info_ptr->gsm.BSIC,
          gas_id
        ),
        gas_id
      );
    }
  } /* add_scell_to_surrounding_database == TRUE */

  /**************************************************************************/
  /* invalidate all SI for new serving cell, and clean out the BA           */
  /**************************************************************************/
  scell_info_ptr->gsm.camped_on_flag = FALSE;

}

/*===========================================================================

FUNCTION rr_cell_resel_set_meas_expected

DESCRIPTION
  Sets the measurement expected bit mask.  If the MS is GSM only and in
  packet transfer mode then WCDMA measurements are not expected.  In all other
  cases WCDMA measurements are expected.

PARAMS
  None

DEPENDENCIES
  Read rr_PLMN_request_details.gsm_only
  Call rr_macinfo_get_mac_status()

RETURN VALUE
  uint8 - measurement expected bit mask

===========================================================================*/
static uint8 rr_cell_resel_set_meas_expected(
  const gas_id_t gas_id
)
{
  uint8 measurement_expected = 0;

#ifdef FEATURE_IDLE_DRX_SCALING
  if (rr_idle_drx_is_nmeas_off(gas_id) && (rr_macinfo_get_mac_status(gas_id) == MAC_IDLE))
  {
    measurement_expected = RR_MASK_GSM_SERV_MEAS;
  }
  else
#endif /* FEATURE_IDLE_DRX_SCALING */
  if (rr_gsm_only(gas_id) || rr_macinfo_get_mac_status(gas_id) == MAC_TRANSFER)
  {
    measurement_expected = RR_MASK_GSM_SURROUND_MEAS | RR_MASK_GSM_SERV_MEAS;
  }
  else
  {
    measurement_expected = RR_MASK_GSM_SURROUND_MEAS | RR_MASK_GSM_SERV_MEAS | RR_MASK_WCDMA_SURROUND_MEAS;
  }

  return measurement_expected;
}

/*===========================================================================

FUNCTION rr_gprs_cache_current_LAI

DESCRIPTION

PARAMS

DEPENDENCIES

RETURN VALUE

===========================================================================*/
static void rr_gprs_cache_current_LAI(
  const gas_id_t gas_id
)
{
  rr_resel_data_t   *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  if (scell_info_ptr)
  {
    MSG_GERAN_MED_1_G("Caching serving LAI (LAC = %d)",
      scell_info_ptr->gsm.location_area_identification.location_area_code);

    rr_resel_data_ptr->old_scell_LAI = scell_info_ptr->gsm.location_area_identification;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("scell_info_ptr == NULL");
  }
}





/*===========================================================================

FUNCTION rr_resel_prepare_for_resel_from_path_loss

DESCRIPTION
  Performs basic reset tasks, connection control, si2q etc prior
  to reselection.

PARAMS
  None

DEPENDENCIES


RETURN VALUE
  None

===========================================================================*/
static void rr_resel_prepare_for_resel_from_path_loss(
  boolean clearSI,
  const gas_id_t gas_id
)
{
  /* If GMM has instructed L1 to enter MM non-DRX mode, it would normally  */
  /* instruct L1 to exit this mode upon completion of the GMM procedure    */
  /* (ATT or RAU). In case this never happens, because of this reselection */
  /* it is probably best to "clean-up" and reset to MM non-DRX state.      */
  rr_gprs_clear_l1_in_mm_non_drx(gas_id);

  /* Stop the sys info refresh timer and reset sys info handling */
  rr_gprs_deactivate_timed_sys_info_refresh(gas_id);

  /* Reset connection establishment. This covers both CS and PS est. */
  /* These calls are to ensure timers are stopped (e.g. T3142, as    */
  /* these are applicable only on the old serving cell), and covers  */
  /* the scenario where the serving cell is barred (forcing a        */
  /* reselection) during estalishment                                */
  (void) rr_connection_establishment_control(EV_RESET_SOFTWARE, NULL, gas_id);
  (void) rr_connection_establishment_control(EV_GPRS_RESET_RR_CE_STATE, NULL, gas_id);
  (void) grr_conn_est_control(EV_RESET_SOFTWARE, NULL, gas_id);

  if (clearSI)
  {  /* as we're now performing a reselection, clear the SI2quater store */
     rr_initialize_SI2q_instances(gas_id);
     rr_initialize_SI2ter_instances(gas_id);
  }	 


  /* Stop the path loss timer if it is running */
  if (rr_timer_cancel(PATH_LOSS_TIMER, gas_id))
  {
    MSG_GERAN_LOW_0_G("Stopped the PATH LOSS TIMER");
  }

  /* cache the current LAI so that changes can be detected after reselection */
  rr_gprs_cache_current_LAI(gas_id);
}


boolean resel_type_C2_based(const gas_id_t gas_id)
{
   rr_resel_data_t   *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
   if(RESEL_C2_CHOOSE_CELL == rr_resel_data_ptr->resel_type)
   {
     MSG_GERAN_HIGH_0_G("resel_type_C2_based is RESEL_C2_CHOOSE_CELL");
     return TRUE;
   }
   else
   {
     MSG_GERAN_HIGH_1_G("resel_type_C2_based is %d ",rr_resel_data_ptr->resel_type);
     return FALSE;
   }   
}


/*===========================================================================

FUNCTION rr_resel_get_next_untried_cell

DESCRIPTION
  This function searches the better cell list for the best cell that is
  (optionally) not forbidden and that RR has not yet attempted reselection to.

  When searching for the best cell, the better cell list is sorted using
  the function cmp_fn.  This is passed through to qsort, so should match
  the qsort criteria.

PARAMS
  cmp_fn Function pointer to pass to qsort to sort the better cell list entries.

  check_forbidden_cells Boolean parameter TRUE if RR should consider forbidden
                        cells when searching for a reselection candidate, FALSE otherwise.

  validate_fn Optional pointer to a function to perform an additional
              validation step on better cells before allowing them to be considered
              for reselection.  An example of the use of this is to verify that an ncell
              has a C31 value greater than zero.

DEPENDENCIES


RETURN VALUE
  Pointer to the better_cell_entry to attempt reselection to, NULL
  if no suitable cell is found.

===========================================================================*/
static better_cell_entry_T *rr_resel_get_next_untried_cell(
  int (*cmp_fn) (const void *, const void *),
  boolean check_forbidden_cells,
  boolean (*validate_fn) (const better_cell_entry_T *),
  const gas_id_t gas_id
)
{
  uint32                   i;
  size_t                   sorted_better_cell_list_size = 0;
  better_cell_entry_T     *sorted_better_cell_list[MAX_NUMBER_OF_BEST_CELLS];
  better_cell_entry_T     *better_cell = NULL;
  gprs_better_cell_list_T *better_cell_list_ptr = rr_get_better_cell_list_ptr(gas_id);

  if (cmp_fn == NULL)
  {
    MSG_GERAN_ERROR_0_G("Comparison function is NULL!");
    return NULL;
  }

  for (i = 0;
       (i < better_cell_list_ptr->no_of_entries) && (i < MAX_NUMBER_OF_BEST_CELLS);
       i++)
  {
    /* Only add cells to which reselection has not already been attempted,
    and optionally which have C31 > 0 */
    if (better_cell_list_ptr->candidate_cell[i].cell_reselection_attempted)
    {
      MSG_GERAN_MED_2_G("Reselection to (%d,%d) already attempted, skipping",
        better_cell_list_ptr->candidate_cell[i].reselection_data.BCCH_ARFCN.num,
        better_cell_list_ptr->candidate_cell[i].reselection_data.BCCH_ARFCN.band);
      continue;
    }
    else if (validate_fn != NULL)
    {
      if (!validate_fn(&better_cell_list_ptr->candidate_cell[i]))
      {
        continue;
      }
    }
    else
    {
      sorted_better_cell_list[sorted_better_cell_list_size++] = &better_cell_list_ptr->candidate_cell[i];
    }
  }

  qsort(sorted_better_cell_list,
        sorted_better_cell_list_size,
        sizeof(better_cell_entry_T *),
        cmp_fn);

  for (i = 0;
       i < sorted_better_cell_list_size && i < MAX_NUMBER_OF_BEST_CELLS;
       ++i)
  {
    if (rr_cell_forbidden(
          sorted_better_cell_list[i]->reselection_data.BCCH_ARFCN,
          gas_id
        ))
    {
      MSG_GERAN_HIGH_2_G("Cell (%d, %d) is currently forbidden, skipping",
        sorted_better_cell_list[i]->reselection_data.BCCH_ARFCN.num,
        sorted_better_cell_list[i]->reselection_data.BCCH_ARFCN.band);
    }
    else
    {
      better_cell = sorted_better_cell_list[i];
      better_cell->cell_reselection_attempted = TRUE;
      break;
    }
  }

  if (better_cell == NULL && check_forbidden_cells)
  {
    MSG_GERAN_HIGH_0_G("No suitable (not forbidden) cells found, checking for forbidden cells");

    for (i = 0; i < sorted_better_cell_list_size; i++)
    {
      if (!sorted_better_cell_list[i]->cell_reselection_attempted)
      {
        better_cell = sorted_better_cell_list[i];
        better_cell->cell_reselection_attempted = TRUE;
        break;
      }
    }
  }

  if (better_cell)
  {
    MSG_GERAN_MED_2_G("Best untried cell: (%d, %d)",
      better_cell->reselection_data.BCCH_ARFCN.num,
      better_cell->reselection_data.BCCH_ARFCN.band);
  }

  return better_cell;
}


/*===========================================================================

FUNCTION compare_better_cell_entries_by_c32_with_crh

DESCRIPTION
  This is a comparison function for use with qsort to sort better_cell_entry_T
  structures in descending order of C32, with cell reselection hysteresis
  applied.

PARAMS

DEPENDENCIES


RETURN VALUE

===========================================================================*/
static int compare_better_cell_entries_by_c32_with_crh(
  const void *ptr1,
  const void *ptr2
)
{
  if (ptr1 != NULL && ptr2 != NULL)
  {
    better_cell_entry_T *cell1 = *(better_cell_entry_T **) ptr1;
    better_cell_entry_T *cell2 = *(better_cell_entry_T **) ptr2;

    if (cell2->reselection_data_with_CRH.C32 >
        cell1->reselection_data_with_CRH.C32)
    {
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    MSG_GERAN_ERROR_2("NULL pointer (%p, %p)", ptr1, ptr2);
    return 0;
  }
}

/*===========================================================================

FUNCTION compare_better_cell_entries_by_c32

DESCRIPTION
  This is a comparison function for use with qsort to sort better_cell_entry_T
  structures in descending order of C32.

PARAMS

DEPENDENCIES


RETURN VALUE

===========================================================================*/
static int compare_better_cell_entries_by_c32(
  const void *ptr1,
  const void *ptr2
)
{
  if (ptr1 != NULL && ptr2 != NULL)
  {
    better_cell_entry_T *cell1 = *(better_cell_entry_T **) ptr1;
    better_cell_entry_T *cell2 = *(better_cell_entry_T **) ptr2;

    if (cell2->reselection_data.C32 > cell1->reselection_data.C32)
    {
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    MSG_GERAN_ERROR_2("NULL pointer (%p, %p)", ptr1, ptr2);
    return 0;
  }
}


/*===========================================================================

FUNCTION compare_better_cell_entries_by_priority_class_and_c32

DESCRIPTION
  This is a comparison function for use with qsort to sort better_cell_entry_T
  structures in order of priority class and C32 when camped on a BCCH.

PARAMS

DEPENDENCIES


RETURN VALUE

===========================================================================*/
static int compare_better_cell_entries_by_priority_class_and_c32(
  const void *ptr1,
  const void *ptr2,
  const gas_id_t gas_id
)
{
  if (ptr1 != NULL && ptr2 != NULL)
  {
    better_cell_entry_T **cell1 = (better_cell_entry_T **) ptr1;
    better_cell_entry_T **cell2 = (better_cell_entry_T **) ptr2;

    uint8 cell1_priority_class = grr_get_ncell_priority_class((*cell1)->reselection_data.BCCH_ARFCN, gas_id);
    uint8 cell2_priority_class = grr_get_ncell_priority_class((*cell2)->reselection_data.BCCH_ARFCN, gas_id);

    if (cell1_priority_class == cell2_priority_class)
    {
      if ((*cell2)->reselection_data.C32 >
          (*cell1)->reselection_data.C32)
      {
        return 1;
      }
      else
      {
        return -1;
      }
    }
    else
    {
      if (cell2_priority_class > cell1_priority_class)
      {
        return 1;
      }
      else
      {
        return -1;
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_2_G("NULL pointer (%p, %p)", ptr1, ptr2);
    return 0;
  }
}

static int compare_better_cell_entries_by_priority_class_and_c32_gas_id_1(
  const void *ptr1,
  const void *ptr2
)
{
  return
    compare_better_cell_entries_by_priority_class_and_c32(
      ptr1,
      ptr2,
      GERAN_ACCESS_STRATUM_ID_1
    );
}

#ifdef FEATURE_DUAL_SIM
static int compare_better_cell_entries_by_priority_class_and_c32_gas_id_2(
  const void *ptr1,
  const void *ptr2
)
{
  return
    compare_better_cell_entries_by_priority_class_and_c32(
      ptr1,
      ptr2,
      GERAN_ACCESS_STRATUM_ID_2
    );
}

#ifdef FEATURE_TRIPLE_SIM
static int compare_better_cell_entries_by_priority_class_and_c32_gas_id_3(
  const void *ptr1,
  const void *ptr2
)
{
  return
    compare_better_cell_entries_by_priority_class_and_c32(
      ptr1,
      ptr2,
      GERAN_ACCESS_STRATUM_ID_3
    );
}
#endif /* FEATURE_TRIPLE_SIM */
#endif /* FEATURE_DUAL_SIM */

/*===========================================================================

FUNCTION compare_better_cell_entries_by_priority_class_and_c32_with_crh

DESCRIPTION
  This is a comparison function for use with qsort to sort better_cell_entry_T
  structures in descending order of priority class and C32 when camped on a
  BCCH.

  If each cell's priority class is equal, the cells are sorted in descending
  order of C32.

  Cell reselection hysteresis is applied.

PARAMS

DEPENDENCIES


RETURN VALUE

===========================================================================*/
static int compare_better_cell_entries_by_priority_class_and_c32_with_crh(
  const void *ptr1,
  const void *ptr2,
  const gas_id_t gas_id
)
{
  if (ptr1 != NULL && ptr2 != NULL)
  {
    better_cell_entry_T *cell1 = *(better_cell_entry_T **) ptr1;
    better_cell_entry_T *cell2 = *(better_cell_entry_T **) ptr2;

    uint8 cell1_priority_class = grr_get_ncell_priority_class(cell1->reselection_data.BCCH_ARFCN, gas_id);
    uint8 cell2_priority_class = grr_get_ncell_priority_class(cell2->reselection_data.BCCH_ARFCN, gas_id);

    if (cell1_priority_class == cell2_priority_class)
    {
      if (cell2->reselection_data_with_CRH.C32 >
          cell1->reselection_data_with_CRH.C32)
      {
        return 1;
      }
      else
      {
        return -1;
      }
    }
    else
    {
      if (cell2_priority_class > cell1_priority_class)
      {
        return 1;
      }
      else
      {
        return -1;
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_2_G("NULL pointer (%p, %p)", ptr1, ptr2);
    return 0;
  }
}

static int compare_better_cell_entries_by_priority_class_and_c32_with_crh_gas_id_1(
  const void *ptr1,
  const void *ptr2
)
{
  return
    compare_better_cell_entries_by_priority_class_and_c32_with_crh(
      ptr1,
      ptr2,
      GERAN_ACCESS_STRATUM_ID_1
    );
}

#ifdef FEATURE_DUAL_SIM
static int compare_better_cell_entries_by_priority_class_and_c32_with_crh_gas_id_2(
  const void *ptr1,
  const void *ptr2
)
{
  return
    compare_better_cell_entries_by_priority_class_and_c32_with_crh(
      ptr1,
      ptr2,
      GERAN_ACCESS_STRATUM_ID_2
    );
}
#ifdef FEATURE_TRIPLE_SIM
static int compare_better_cell_entries_by_priority_class_and_c32_with_crh_gas_id_3(
  const void *ptr1,
  const void *ptr2
)
{
  return
    compare_better_cell_entries_by_priority_class_and_c32_with_crh(
      ptr1,
      ptr2,
      GERAN_ACCESS_STRATUM_ID_3
    );
}
#endif /* FEATURE_TRIPLE_SIM */
#endif /* FEATURE_DUAL_SIM */

/*===========================================================================

FUNCTION compare_better_cell_entries_by_c2_with_crh

DESCRIPTION
  This is a comparison function for use with qsort to sort better_cell_entry_T
  structures in descending order of C2, with cell reselection hysteresis
  applied.

PARAMS

DEPENDENCIES


RETURN VALUE

===========================================================================*/
static int compare_better_cell_entries_by_c2_with_crh(
  const void *ptr1,
  const void *ptr2
)
{
  if (ptr1 != NULL && ptr2 != NULL)
  {
    /* ptr1 and ptr2 are actually pointers to pointers to the objects that we
    wish to compare, as the source array is an array of pointers. */
    better_cell_entry_T *cell1 = *(better_cell_entry_T **) ptr1;
    better_cell_entry_T *cell2 = *(better_cell_entry_T **) ptr2;

    if (cell2->reselection_data_with_CRH.C2 >
        cell1->reselection_data_with_CRH.C2)
    {
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    MSG_GERAN_ERROR_2("NULL pointer (%p, %p)", ptr1, ptr2);
    return 0;
  }
}

/*===========================================================================

FUNCTION compare_better_cell_entries_by_c2

DESCRIPTION
  This is a comparison function for use with qsort to sort better_cell_entry_T
  structures in descending order of C2.

PARAMS

DEPENDENCIES


RETURN VALUE

===========================================================================*/
static int compare_better_cell_entries_by_c2(
  const void *ptr1,
  const void *ptr2
)
{
  if (ptr1 != NULL && ptr2 != NULL)
  {
    better_cell_entry_T **cell1 = (better_cell_entry_T **) ptr1;
    better_cell_entry_T **cell2 = (better_cell_entry_T **) ptr2;

    if ((*cell2)->reselection_data.C2 > (*cell1)->reselection_data.C2)
    {
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    MSG_GERAN_ERROR_2("NULL pointer (%p, %p)", ptr1, ptr2);
    return 0;
  }
}

/*===========================================================================

FUNCTION rr_gprs_get_best_cell

DESCRIPTION


PARAMS
  CRH Indicates whether Hysteresis should be applied to reselection
  parameters.

  Check_forbidden_cells Indicates whether RR should attempt to camp
  on forbidden cells if no other suitable cells are present.

DEPENDENCIES


RETURN VALUE
 Pointer to the best cell, NULL if no cells are available.


===========================================================================*/
static better_cell_entry_T *rr_gprs_get_best_cell(
  boolean CRH,
  boolean check_forbidden_cells,
  const gas_id_t gas_id
)
{
  better_cell_entry_T     *better_cell = NULL;
  gprs_scell_info_t       *scell = rr_gprs_get_scell_info(gas_id);
  gprs_better_cell_list_T *better_cell_list_ptr = rr_get_better_cell_list_ptr(gas_id);

  if (rr_gprs_parameters_available(gas_id))
  {
    /******************************************************/
    /* See if HCS parameters are available and if so then */
    /* check values to see if any cells have C31 > 0.     */
    /******************************************************/
    if (scell->bcch_sys_info.pmo_scell_params_valid &&
        scell->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params_present &&
        better_cell_list_ptr->candidate_cell[0].reselection_data.C31 >= 0)
    {
      if (CRH)
      {
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_TRIPLE_SIM
        if (gas_id == GERAN_ACCESS_STRATUM_ID_3)
        {
          better_cell = rr_resel_get_next_untried_cell(
            compare_better_cell_entries_by_priority_class_and_c32_with_crh_gas_id_3,
            check_forbidden_cells,
            NULL,
            gas_id
          );
        }
        else
#endif /* FEATURE_TRIPLE_SIM */
        if (gas_id == GERAN_ACCESS_STRATUM_ID_2)
        {
          better_cell = rr_resel_get_next_untried_cell(
            compare_better_cell_entries_by_priority_class_and_c32_with_crh_gas_id_2,
            check_forbidden_cells,
            NULL,
            gas_id
          );
        }
        else
#endif /* FEATURE_DUAL_SIM */
        {
          better_cell = rr_resel_get_next_untried_cell(
            compare_better_cell_entries_by_priority_class_and_c32_with_crh_gas_id_1,
            check_forbidden_cells,
            NULL,
            gas_id
          );
        }
      }
      else
      {
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_TRIPLE_SIM
        if (gas_id == GERAN_ACCESS_STRATUM_ID_3)
        {
          better_cell = rr_resel_get_next_untried_cell(
            compare_better_cell_entries_by_priority_class_and_c32_gas_id_3,
            check_forbidden_cells,
            NULL,
            gas_id
          );
        }
        else
#endif /* FEATURE_TRIPLE_SIM */
        if (gas_id == GERAN_ACCESS_STRATUM_ID_2)
        {
          better_cell = rr_resel_get_next_untried_cell(
            compare_better_cell_entries_by_priority_class_and_c32_gas_id_2,
            check_forbidden_cells,
            NULL,
            gas_id
          );
        }
        else
#endif /* FEATURE_DUAL_SIM */
        {
          better_cell = rr_resel_get_next_untried_cell(
            compare_better_cell_entries_by_priority_class_and_c32_gas_id_1,
            check_forbidden_cells,
            NULL,
            gas_id
          );
        }
      }
    }
    else
    {
      /**********************************************************/
      /* Get the best C32 cell as in GSM05.08 section 10.1.3    */
      /* Do not use HCS reselection just find the best C32 value*/
      /* and disregard the PRIORITY_CLASS                       */
      /**********************************************************/
      if (CRH)
      {
        better_cell = rr_resel_get_next_untried_cell(
          compare_better_cell_entries_by_c32_with_crh,
          check_forbidden_cells,
          NULL,
          gas_id
        );
      }
      else
      {
        better_cell = rr_resel_get_next_untried_cell(
          compare_better_cell_entries_by_c32,
          check_forbidden_cells,
          NULL,
          gas_id
        );
      }
    }
  }
  else
  {
    /****************************************************/
    /* Get the best C2 cell as in GSM05.08 section 6.4  */
    /****************************************************/
    if (CRH)
    {
      better_cell = rr_resel_get_next_untried_cell(
        compare_better_cell_entries_by_c2_with_crh,
        check_forbidden_cells,
        NULL,
        gas_id
      );
    }
    else
    {
      better_cell = rr_resel_get_next_untried_cell(
        compare_better_cell_entries_by_c2,
        check_forbidden_cells,
        NULL,
        gas_id
      );
    }
  }

  return better_cell;
}

/*===========================================================================

FUNCTION rr_resel_start_c1

DESCRIPTION


PARAMS
  boolean check_forbidden_cells

DEPENDENCIES


RETURN VALUE


===========================================================================*/
static rr_resel_start_c1_status_e rr_resel_start_c1(
  boolean create_gprs_reselection_list,
  boolean check_forbidden_cells,
  boolean g2w_resel_allowed,
  const gas_id_t gas_id
)
{
  rr_resel_data_t               *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  rr_resel_start_c1_status_e     c1_status = START_C1_RESEL_CONTINUE;
  gprs_scell_info_t             *scell_info = rr_gprs_get_scell_info(gas_id);
  gprs_reselection_structure_T  *gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  MSG_GERAN_MED_3_G(
    "rr_resel_start_c1(create_gprs_reselection_list => %d, check_forbidden_cells => %d, g2w_resel_allowed => %d)",
    create_gprs_reselection_list,
    check_forbidden_cells,
    g2w_resel_allowed
  );

  /********************************************************/
  /* Reselection is due to either DSF, cell BARRED or no  */
  /* response after MAX_RETRANS. Build a list of ncells   */
  /* with C1 > 0. Then from this list select the one with */
  /* the best value of C2                                 */
  /* Reselection is immediate.!                           */
  /********************************************************/

  if (create_gprs_reselection_list)
  {
    if (rr_gprs_parameters_available(gas_id))
    {
      /* Place into the better cell list C32 values with hysteresis. */
      (void) rr_gprs_build_candidate_cell_list_GPRS(gas_id);
    }
    else
    {
      /* Place into the better cell list C2 values with hysteresis. */
      (void) rr_gprs_build_candidate_cell_list_GSM(gas_id);
    }
  }


  /* Take the first cell from the list and start to reselect to it, check
  forbidden cells for C1 resel and for RPS dont check the forbidden cells. */
  rr_resel_data_ptr->rr_gprs_target_cell = rr_gprs_get_best_cell(TRUE, check_forbidden_cells, gas_id);

  if (rr_resel_data_ptr->rr_gprs_target_cell != NULL)
  {
    c1_status = START_C1_RESEL_CONTINUE;

    /*Load up the data store of old scell multiframe details*/
    rr_resel_data_ptr->rr_gprs_old_scell.reselection_data = gprs_reselection_list_ptr->serving_values;
  }
  else
  {
    /* Cannot stay camped on this scell. */
    scell_info->gsm.camped_on_flag = FALSE;

    /* Old scell is no use perform cell selection. */
    c1_status = START_C1_RESEL_FAILED;
  }

  MSG_GERAN_HIGH_1_G("RR start C1 status: 0x%x",c1_status);

  return c1_status;
}


/****************************************************************************
 *   Function name:  rr_gprs_nacc_check_ccch
 *   -------------
 *
 *   Description: Determines whether NACC is supported on a CCCH cell
 *   ------------
 *
 *   Parameters: none
 *   ----------
 *
 *   Returns:  TRUE if NV flag set, MAC in transfer, and cells support NACC.
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_nacc_check_ccch(
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  gprs_scell_info_t *scell_ptr = rr_gprs_get_scell_info(gas_id);
  boolean            perform_nacc_reselection = FALSE;
  boolean            scell_ccn_active = FALSE;

  if (rr_gprs_nacc_is_allowed(NACC_GSM_GSM, gas_id) == FALSE)
  {
    /* Feature Pack 1 is not enabled */
    return(FALSE);
  }

  if (rr_in_dedicated(gas_id))
  {
    /* NACC is not allowed to proceed whilst RR is in dedicated */
    /* mode (or DTM mode) - see 3GPP 44.060 section 5.5.1.1a.2. */
    MSG_GERAN_MED_0_G("NACC prohibited during dedicated mode");
    return(FALSE);
  }

  /* Make sure that MAC is currently in transfer mode */
  if (rr_macinfo_get_mac_status(gas_id) == MAC_TRANSFER)
  {
    gsm_ncell_list_member_t *ncell_ptr = rr_gprs_get_best_six_by_arfcn(
      rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
      NULL,
      gas_id
    );

    /* check for valid ncell pointer */
    if (ncell_ptr == NULL)
    {
      return(FALSE);
    }

    /* determine whether serving cell has CCN support */
    if (scell_ptr->bcch_sys_info.gprs_cell_options.optional_extension_info_flag)
    {
      scell_ccn_active = scell_ptr->bcch_sys_info.gprs_cell_options.optional_extension_info.ccn_active;
    }

    /* determine whether the target cell has explicit CCN support */
    if (ncell_ptr->bcch_or_pmo == BCCH_FORMAT)
    {
      if ((scell_ccn_active == TRUE)  &&
          (ncell_ptr->ncell_data.bcch.gprs.ccn_supported_valid == FALSE))
      {
        perform_nacc_reselection = TRUE;
      }
      else
      if ((scell_ccn_active == TRUE) &&
          (ncell_ptr->ncell_data.bcch.gprs.ccn_supported_valid == TRUE) &&
          (ncell_ptr->ncell_data.bcch.gprs.ccn_supported == TRUE))
      {
        perform_nacc_reselection = TRUE;
      }
    }
  }

  return(perform_nacc_reselection);
}


/*===========================================================================

FUNCTION rr_resel_start_c2

DESCRIPTION


PARAMS
  None

DEPENDENCIES


RETURN VALUE


===========================================================================*/
static rr_resel_start_c2_status_e rr_resel_start_c2(
  rr_resel_start_c2_cause resel_cause,
  const gas_id_t gas_id
)
{
  rr_resel_data_t              *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  gprs_reselection_structure_T *gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  rr_resel_start_c2_status_e c2_status;

  /********************************************************/
  /* A candidate cell list has been built in this case    */
  /* the best cell in the list needs to be targeted for   */
  /* reselection. The first step is to get updates SI3 and*/
  /* SI4 to check that the cell really is the best        */
  /********************************************************/

  /*Forbidden cells are not checked for C2 reselection*/
  rr_resel_data_ptr->rr_gprs_target_cell = rr_gprs_get_best_cell(TRUE, FALSE, gas_id);

  if (rr_resel_data_ptr->rr_gprs_target_cell != NULL)
  {
    if (resel_cause == START_C2_TARGET_BEST_CELL)
    {
      /*Load up the data store for old scell details*/
      rr_resel_data_ptr->rr_gprs_old_scell.reselection_data = gprs_reselection_list_ptr->serving_values;
    }

    /************************************************************/
    /* A decision as to how to reselect cells must be made. A   */
    /* different algorithm is needed for moveing to PBCCH and   */
    /* BCCH                                                     */
    /************************************************************/

    if (rr_gprs_get_surround_ncell_sys_info(
          rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
          gas_id
        ) != NULL)
    {
      /****************************************************************************/
      /* Setup the SI handler to receive ncell SI 3 and 4 for validation purposes */
      /* we simply want to receive SI3 and 4 before reselection takes place. But  */
      /* this only needs to happen for reselection when ncell system info isn't   */
      /* provided by the scell. i.e. not camped on pbcch or given by a PMO.       */
      /* The easiest way to work this out is to check for an entry in the ncell   */
      /* bcch database.                                                           */
      /****************************************************************************/
      if (rr_gprs_nacc_check_ccch(gas_id))
      {
        /* CCN is active, reselect immediately (using NACC) */
        MSG_GERAN_HIGH_0_G("NACC: bypass SI3 reconf, reselect now");
        c2_status = NACC_RESEL_FROM_BCCH_CELL;
      }
      else
      {
        /*TODO is this required?*/
        //rr_gprs_surround_update_SI3_SI4_for_ARFCN(rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN);
        c2_status = RESEL_FROM_BCCH_CELL;
      }

      if (resel_cause == START_C2_TARGET_BEST_CELL)
      {
       gsm_ncell_list_member_t *ncell_ptr;

        /* Ensure the ncell 5 minute timer doesn't expire while waiting for SI3 */
        /* otherwise it will cancel the surround update in L1 - see CR #59952   */
        ncell_ptr = rr_gprs_get_ncell_info(
          rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
          NULL,
          gas_id
        );

        if (ncell_ptr != NULL)
        {
          if (ncell_ptr->ncell_data.bcch.system_info_timer_count != UNINITIALISED_SYS_INFO_COUNT)
          {
            /* incrementing this count is equivalent to extending the timer by 10 seconds */
            ncell_ptr->ncell_data.bcch.system_info_timer_count++;
          }
        }
      }
    }
    else
    {
      /* Camped on BCCH cell but reselecting to a cell with GPRS params available. */
      MSG_GERAN_HIGH_0_G("Starting C2 reselection from BCCH->BCCH(PSI)");
      c2_status = RESEL_FROM_BCCH_WITH_GPRS_CELL_SELECTION_PARAMS;
    }
  }
  else
  {
    c2_status = RESEL_NO_CELLS_VALID_FOR_RESEL;
  }

  MSG_GERAN_HIGH_1_G("RR start C2 status: 0x%x",c2_status);

  return c2_status;
}


/****************************************************************************
 *   Function name:  grr_reset_reselection_power_scan
 *   -------------
 *
 *   Description:
 *   ------------
 *    Resets all the info used in the power scan.
 *
 *   Parameters: none
 *   ----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
static void grr_reset_reselection_power_scan(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_data_ptr);

  MSG_GERAN_MED_0_G("Invalidating RPS Data");

  rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries = 0;
  rr_resel_data_ptr->rps_data.pscan_cell_list.valid_data = FALSE;
  rr_resel_data_ptr->rps_data.reason_for_rps_aborted = NO_ABORT;
}


/*===========================================================================

FUNCTION rr_resel_handle_no_service

DESCRIPTION


PARAMS
  NONE

DEPENDENCIES

RETURN VALUE

===========================================================================*/
static void rr_resel_handle_no_service(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  /************************************************************/
  /* If a cell reselection has been attempted and failed then */
  /* resel all the relevant areas                             */
  /************************************************************/
  if (scell_info_ptr->gsm.camped_on_flag == FALSE)
  {
    bcch_ncell_info_t *bcch_ncell;
    gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

    bcch_ncell = rr_gprs_get_surround_ncell_sys_info(pcell_info_ptr->gsm.BCCH_ARFCN, gas_id);

    if (bcch_ncell)
    {
      /* Invalidate the ncell data */
      bcch_ncell->gsm.update_message_received = 0;
      bcch_ncell->gsm.valid_data_flag = FALSE;
    }

    /* clean out and invalidate the pcell */
    pcell_info_ptr->gsm.BCCH_ARFCN.num = INVALID_ARFCN;
    pcell_info_ptr->gsm.BCCH_ARFCN.band = SYS_BAND_NONE;
    pcell_info_ptr->gsm.valid_data_flag = FALSE;
    pcell_info_ptr->gsm.update_message_received = 0;
  }

  return;
}


/****************************************************************************
 *   Function name: grr_start_nacc_reselection_process
 *   -------------
 *
 *   Description: Starts the 2G NACC procedure off. This means sending the
 *   ------------ PACKET_CELL_CHANGE_NOTIFICATION to MAC, and then starting
 *                timers T3208 and T3210.
 *
 *
 *   Parameters: none
 *   ----------
 *
 *   Returns:  Output event for GRR control
 *   --------
 *
 ****************************************************************************/
static void grr_start_nacc_reselection_process(
  const gas_id_t gas_id
)
{
  rr_resel_data_t               *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  enc_nacc_nc_meas_rprt_strct_t *nacc_nc_meas_rpt = &rr_resel_data_ptr->pccn.ccn_meas_rpt;
  gsm_ncell_list_member_t       *best_six_member  = NULL;
  gprs_reselection_structure_T  *gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  /* clear out the PCCN message store */
  memset(&rr_resel_data_ptr->pccn,0,sizeof(packet_cell_change_notification_t));

  best_six_member = rr_gprs_get_best_six_by_arfcn(
    rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
    NULL,
    gas_id
  );

  if (best_six_member)
  {
    uint8 new_entry;
    uint8 index;

    rr_resel_data_ptr->pccn.arfcn = best_six_member->arfcn;
    rr_resel_data_ptr->pccn.bsic = CONVERT_BSIC_TO_BYTE(best_six_member->bsic);
    rr_resel_data_ptr->pccn.ba_or_psi3_change_mark = ba_present;
    rr_resel_data_ptr->pccn.ba_used = scell_info_ptr->gsm.complete_BA_list.BA_IND;

    /* @@TODO set this value correctly if a PMO or PCCO has altered the BA list */
    rr_resel_data_ptr->pccn.pmo_used = 0;

    /* build the measurement report within the CCN message */
    nacc_nc_meas_rpt->rxlev_srvg_cell = gprs_reselection_list_ptr->serving_values.RXLEV_average;
    nacc_nc_meas_rpt->intrfrnce_srvg_cell_flag = not_valid;
    nacc_nc_meas_rpt->num_nc_meas = 0;

    for (index = 0; index < gprs_reselection_list_ptr->no_of_entries; index++)
    {
      new_entry = nacc_nc_meas_rpt->num_nc_meas;

      best_six_member = rr_gprs_get_best_six_by_arfcn(
        gprs_reselection_list_ptr->surrounding_values[index].BCCH_ARFCN,
        NULL,
        gas_id
      );

      if (best_six_member)
      {
        nacc_nc_meas_rpt->nc_meas[new_entry].rxlev_n = gprs_reselection_list_ptr->surrounding_values[index].RXLEV_average;
        nacc_nc_meas_rpt->nc_meas[new_entry].freq_n = rr_gprs_find_ba_gprs_index(
          best_six_member->arfcn,
          &best_six_member->bsic,
          gas_id
        );
        nacc_nc_meas_rpt->nc_meas[new_entry].bsic_n_flag = (option_status_e)best_six_member->bsic_valid;
        nacc_nc_meas_rpt->nc_meas[new_entry].bsic_n = CONVERT_BSIC_TO_BYTE(best_six_member->bsic);
        nacc_nc_meas_rpt->num_nc_meas++;
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("NO BEST SIX ENTRY FOR NACC TARGET CELL");
  }

  /* initialise 2G NACC */
  rr_gprs_nacc_initialise(NACC_GSM_GSM, gas_id);

  /* re-use the T3208 timer ID, but as a "guard timer" - equivalent to T3206 */
  MSG_GERAN_HIGH_1_G("Starting T3206 for %dms", (int)T3206_TIMEOUT);
  (void)rr_timer_start(T3208, T3206_TIMEOUT, gas_id);

  /* send the PCCN message to MAC */
  rr_send_grr_mac_send_pccn_req(&rr_resel_data_ptr->pccn, gas_id);
}

/****************************************************************************
 *   Function name:  rr_resel_nacc_check_surround_SI3_SI4
 *   --------------------------
 *
 *   Description: This function is used to scrutinize the received system
 *   ------------ information 3 and 4 that were requested. It decides if the
 *                targeted cell is still better after recieving fresh SI3 and
 *                SI4.
 *                Also the fresh SI13 is checked if available to see if PBCCH
 *                exists in the cell.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  NACC_CELL_SUITABLE_FOR_RESEL, NACC_CELL_NOT_SUITABLE_FOR_RESEL
 *   --------
 *
 ****************************************************************************/
static nacc_surr_check_result_e rr_resel_nacc_check_surround_SI3_SI4(
  boolean recent_reselection_flag,
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  nacc_surr_check_result_e       result = NACC_CELL_NOT_SUITABLE_FOR_RESEL;
  int                            C1 = 0;
  int                            C2 = 0;
  boolean                        surrounding_better_than_serving = FALSE;
  bcch_ncell_info_t             *surround_db_entry_ptr = NULL;
  gprs_scell_info_t             *scell_info = rr_gprs_get_scell_info(gas_id);
  uint8                          ncell_priority_class;
  uint8                          scell_priority_class;
  rr_resel_candidate_t          *resel_candidate;
  gprs_reselection_structure_T  *gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  resel_candidate = rr_resel_best_six_find(
    rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
    NULL,
    gas_id
  );

  if (resel_candidate == NULL)
  {
    MSG_GERAN_ERROR_2_G("Unable to locate ARFCN %d (band %d) in best six",
      rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN.num,
      rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN.band);
    return result;
  }

  surround_db_entry_ptr = rr_gprs_get_surround_ncell_sys_info(
    rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
    gas_id
  );

  MSG_GERAN_MED_1_G("Checking reselected cell: %d",CONVERT_FROM_ARFCN_T(rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN));

  if (rr_gprs_resel_suitable_cell(surround_db_entry_ptr, gas_id))
  {
    if (rr_gprs_parameters_available(gas_id) == FALSE)
    {
      /***********************************************************************/
      /* We have to calculate the C1 and C2 parameters again, with the new   */
      /* system information to see if the cell is still the best target      */
      /***********************************************************************/
      /* calculate C1 for the target cell */
      C1 = rr_get_C1(
        surround_db_entry_ptr->gsm.BCCH_ARFCN,
        rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.RXLEV_average,
        surround_db_entry_ptr->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
        surround_db_entry_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
        surround_db_entry_ptr->gsm.cell_selection_parameters.POWER_OFFSET,
        gas_id
      );

      /****************************************************/
      /* Make sure that the cells C1 is greater than 0    */
      /* otherwise it has already failed the suitable cell*/
      /* criteria                                         */
      /****************************************************/
      if (C1 >= 0 || rr_allow_negative_c1(surround_db_entry_ptr->gsm.location_area_identification.plmn_id, gas_id))
      {
        /*calculate C2 for the target cell*/
        C2 = rr_gprs_compute_surrounding_C2(C1, resel_candidate, gas_id);

        MSG_GERAN_LOW_0_G("Comparing serving and updated SI surrounding C2s");

        /****************************************************/
        /* Make sure that the cell is still better than the */
        /* scell.                                           */
        /****************************************************/
        surrounding_better_than_serving = rr_gprs_compare_serving_and_surrounding_C2(
          gprs_reselection_list_ptr->serving_values.C2,
          C2,
          surround_db_entry_ptr,
          recent_reselection_flag,
          &(surround_db_entry_ptr->gsm.location_area_identification),
          gas_id
        );
      }
    }
    else
    {
      gprs_reselection_values_T local_fresh_reselection_values;
      /***********************************************************************/
      /* We have to calculate the C1 and C32 parameters again, with the new  */
      /* system information to see if the cell is still suitable             */
      /***********************************************************************/
      /********************************************/
      /* Convert the BCCH parameters as described */
      /* see table 3a GSM05.08                    */
      /* and calculate C1, C31 and C32            */
      /********************************************/
      int GPRS_TEMPORARY_OFFSET = 0;
      int GPRS_RESELECT_OFFSETn = 16; //this is zero
      int PRIORITY_CLASSs = 0;
      int PRIORITY_CLASSn = 0;

       /****************************************************************/
       /* If the cell selection parameters were present then load them */
       /* otherwise they will stay set as 0 and C32 will in effect be  */
       /* the same as C1.                                              */
       /****************************************************************/
       if (surround_db_entry_ptr->gsm.cell_selection_parameters.CELL_RESELECT_PARAM_IND)
       {
         uint8 ro_index;
         GPRS_TEMPORARY_OFFSET = surround_db_entry_ptr->gsm.cell_selection_parameters.TEMPORARY_OFFSET;
         GPRS_RESELECT_OFFSETn = surround_db_entry_ptr->gsm.cell_selection_parameters.CELL_RESELECT_OFFSET*2 -
                                   scell_info->gsm.cell_selection_parameters.CELL_RESELECT_OFFSET*2;

         /* convert the reselection offset to a index value to represent the gprs_reselect_offset */
         for (ro_index=0; ro_index<32; ro_index++)
         {
           if (GPRS_RESELECT_OFFSET_dB[ro_index] > GPRS_RESELECT_OFFSETn)
           {
             GPRS_RESELECT_OFFSETn = ro_index-1;
             break;
           }
         }
       }

      /********************************************/
      /* Calculate the C1 parameter for the ncell */
      /********************************************/
      local_fresh_reselection_values.C1 = rr_get_C1(
        surround_db_entry_ptr->gsm.BCCH_ARFCN,
        rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.RXLEV_average,
        surround_db_entry_ptr->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
        surround_db_entry_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
        0, /*Not used for GPRS cell reselection 5.08 10.1.2*/
        gas_id
      );

      /****************************************************/
      /* Make sure that the cells C1 is greater than 0    */
      /* otherwise it has already failed the suitable cell*/
      /* criteria                                         */
      /****************************************************/
      if (local_fresh_reselection_values.C1 >= 0 || rr_allow_negative_c1(surround_db_entry_ptr->gsm.location_area_identification.plmn_id, gas_id))
      {
        /********************************************/
        /* On BCCH the C31 parameter is set to zero */
        /* see table 3a GSM05.08                    */
        /********************************************/
        local_fresh_reselection_values.C31 = 0;

        /*********************************************/
        /* Calculate the C32 parameter for the ncell */
        /*********************************************/
        local_fresh_reselection_values.C32 = rr_gprs_surround_calculate_C32(
          local_fresh_reselection_values.C1,
          GPRS_TEMPORARY_OFFSET,
          GPRS_RESELECT_OFFSETn,
          PRIORITY_CLASSs,
          PRIORITY_CLASSn,
          resel_candidate->penalty_timer_id, /*only needed for ncells*/
          surround_db_entry_ptr->gsm.cell_selection_parameters.PENALTY_TIME, /*only needed for ncells*/
          FALSE, /*Doesn't matter on 51 frame - C32_QUAL only*/
          gas_id
        );

        /* Now store the values in a reselection structure in preparation for the comparision */
        local_fresh_reselection_values.RXLEV_average = rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.RXLEV_average;
        local_fresh_reselection_values.BCCH_ARFCN = rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN;

        /*******************************************************************/
        /* Decide weather to include HCS reselection in better cell checks */
        /*******************************************************************/
        if (scell_info->bcch_sys_info.pmo_scell_params_valid &&
            scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params_present)
        {
          ncell_priority_class = 0;
          scell_priority_class = scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params.priority_class;

          /***********************************************************/
          /* Check that the C31 of the scell is greater than or equal*/
          /* to zero. If it isn't then according to GSM05.08 it      */
          /* cannot be amongst the best cells as there is already a  */
          /* ncell that has C31 >= 0                                 */
          /***********************************************************/
          if ((gprs_reselection_list_ptr->serving_values.C31 < 0) && (local_fresh_reselection_values.C31 >= 0))
          {
            /*The scell has C31 < 0 and is therefore not better than the ncell*/
            MSG_GERAN_MED_1_G("Ncell C31 >= 0, Scell C31 < 0.Ncell %d better", CONVERT_FROM_ARFCN_T(local_fresh_reselection_values.BCCH_ARFCN));
            surrounding_better_than_serving = TRUE;
          }

          /**********************************************************/
          /* Compare the PRIORITY_CLASS of the ncell with that of   */
          /* the scells. If greater this alone make the ncell better*/
          /**********************************************************/
          if((scell_priority_class < ncell_priority_class) && surrounding_better_than_serving == FALSE)
          {
            /*The ncell has C31 > 0 and a higher PRIORITY_CLASS than the SCELL so it must
            qualify as a better cell*/
            MSG_GERAN_MED_1_G("Ncell Scell C31>=0, ARFCN%d is higher priority",CONVERT_FROM_ARFCN_T(local_fresh_reselection_values.BCCH_ARFCN));
            surrounding_better_than_serving = TRUE;
          }

          /************************************************************/
          /* Compare the C32 values of the scell and the ncell. If it */
          /* is greater than the C32 of the scell, then the ncell must*/
          /* be of equal PRIORITY CLASS to be a better cell           */
          /************************************************************/
          if((scell_priority_class == ncell_priority_class)
              && surrounding_better_than_serving == FALSE)
          {
            surrounding_better_than_serving = rr_gprs_compare_serving_and_surrounding_C32(
              &local_fresh_reselection_values,
              &gprs_reselection_list_ptr->serving_values,
              recent_reselection_flag,
              gas_id
            );
          }
        }
        else /* don't perform HCS checks */
        {
          /***************************************************************/
          /* The computed C32 values for both the surrounding and serving*/
          /* cells are available, perform the comparison using the values*/
          /* in the current reselection structure                        */
          /***************************************************************/
          surrounding_better_than_serving = rr_gprs_compare_serving_and_surrounding_C32(
            &local_fresh_reselection_values,
            &gprs_reselection_list_ptr->serving_values,
            recent_reselection_flag,
            gas_id
          );
        }
      }
    }

    if (surrounding_better_than_serving)
    {
      /*******************************************************/
      /* This is still the best cell carry on reselecting it */
      /*******************************************************/
      MSG_GERAN_MED_1_G("ncell (%d) better",CONVERT_FROM_ARFCN_T(rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN));
      result = NACC_CELL_SUITABLE_FOR_RESEL;
    }
    else
    {
      /*******************************************/
      /* The cell was not better than the scell  */
      /*******************************************/
      MSG_GERAN_MED_1_G("ncell (%d) no better",CONVERT_FROM_ARFCN_T(rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN));
      result = NACC_CELL_NOT_SUITABLE_FOR_RESEL;
    }
  }
  else /*The target cell was barred*/
  {
    /********************************************************************/
    /* Kill reselection to this target as it is not suitable move on to */
    /* the next best cell stop the sys info handler from waiting for the*/
    /* SI13 from the ncell                                              */
    /********************************************************************/
    result = NACC_CELL_NOT_SUITABLE_FOR_RESEL;
  }

  return result;
}



/****************************************************************************
 *   Function name:  rr_gprs_prepare_for_reselection_with_PNCD
 *   -------------
 *
 *   Description: This function is the initial step to prepare for a reselection.
 *   ------------ using NACC. It prepares the pcell to receive the PNCD that
 *                has been sent in.
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     Void
 *   --------
 *
 ****************************************************************************/
static void rr_gprs_prepare_for_reselection_with_PNCD(
  uint8 container_id,
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  gprs_scell_info_t             *pcell_info = rr_gprs_get_pcell_info(gas_id);
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gsm_ncell_list_member_t       *chosen_cell;
  best_six_ncells_t             *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
  uint8                          index;

  /* may not have called the PNCD trigger yet, so store the details now */
  if (container_id != INVALID_CONTAINER_ID)
  {
    rr_gprs_nacc_store_trigger_info(container_id, gas_id);
  }

  /*********************************************************************/
  /* Clean out BA list and SI received flags from pending channel data */
  /*********************************************************************/
  rr_initialize_SI2q_instances(gas_id);
  rr_initialize_SI2ter_instances(gas_id);
  rr_clear_system_information(&pcell_info->gsm);

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
    rr_set_rr_using_pseudo_si2q_data(FALSE, gas_id);
#endif

  /***************************************************************************/
  /* Set up the pending channel system information database for reselection  */
  /* to the new target cell.                                                 */
  /***************************************************************************/

  /* Load the ARFCN of the target cell into the pending channel database */
  pcell_info->gsm.BCCH_ARFCN = rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN;

  /*********************************************************************/
  /* Load the BSIC of the target cell into the pending channel database*/
  /*********************************************************************/
  if ( (chosen_cell = rr_gprs_get_best_six_by_arfcn(
          rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
          NULL,
          gas_id
        )) != NULL )
  {
    pcell_info->gsm.BSIC = chosen_cell->bsic;
  }
  else
  {
    if (rr_resel_data_ptr->pcco_data.pccf.cause == (uint8)PCCF_STORED_BSIC)
    {
      CONVERT_TO_BSIC_T(pcell_info->gsm.BSIC, rr_resel_data_ptr->pcco_data.pccf.bsic);
      MSG_GERAN_HIGH_2_G("Blind reselection to ARFCN %d BSIC %d", rr_resel_data_ptr->pcco_data.pccf.arfcn.num, rr_resel_data_ptr->pcco_data.pccf.bsic);
    }
    else
    {
      MSG_GERAN_HIGH_1_G("No ncell entry for target cell %d",CONVERT_FROM_ARFCN_T(rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN));
    }
  }

  /**************************************************************/
  /* Take the old scell and place into the BAlist. Then copy    */
  /* the SI3 parameters.                                        */
  /**************************************************************/
  {
    bcch_ncell_info_t  *new_surround_db_entry_ptr;

    /* Always use the SI3 data for early camping */
    rr_ncell_ccch_to_pcell(
      rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
      gas_id
    );

    /* Take the entry out of the best six list */
    rr_gprs_remove_from_best_six(
      pcell_info->gsm.BCCH_ARFCN, pcell_info->gsm.BSIC,
      gas_id
    );

    /**********************************************************/
    /* Add the old serving cell into the surrounding database */
    /**********************************************************/
    rr_gprs_add_ncell_to_ba_gprs(
      rr_gprs_get_scell_arfcn(gas_id),
      &scell_info_ptr->gsm.BSIC,
      NULL,
      gas_id
    );

    /* Load up the data for this new cell */
    new_surround_db_entry_ptr = rr_gprs_get_surround_ncell_sys_info(
      rr_gprs_get_scell_arfcn(gas_id),
      gas_id
    );

    if (new_surround_db_entry_ptr)
    {
      new_surround_db_entry_ptr->gsm.BSIC = rr_gprs_get_scell_bsic(gas_id);

      /******************************************************************/
      /* Since system information for the old serving cell was current, */
      /* update the system information for the new surrounding entry    */
      /* for the old serving cell                                       */
      /******************************************************************/
      new_surround_db_entry_ptr->gsm.valid_data_flag = TRUE;
      new_surround_db_entry_ptr->gsm.update_message_received = scell_info_ptr->gsm.update_message_received;
      new_surround_db_entry_ptr->gsm.RACH_control_parameters = scell_info_ptr->gsm.RACH_control_parameters;
      new_surround_db_entry_ptr->gsm.cell_selection_parameters = scell_info_ptr->gsm.cell_selection_parameters;
      new_surround_db_entry_ptr->gsm.location_area_identification = scell_info_ptr->gsm.location_area_identification;
      new_surround_db_entry_ptr->gsm.control_channel_description = scell_info_ptr->gsm.control_channel_description;
      new_surround_db_entry_ptr->gsm.control_channel_description_valid = scell_info_ptr->gsm.control_channel_description_valid;

      new_surround_db_entry_ptr->gsm.si_expected = FALSE;
      new_surround_db_entry_ptr->gprs_supported = scell_info_ptr->gsm.gprs_indicator;
      new_surround_db_entry_ptr->gprs.ra_colour = scell_info_ptr->bcch_sys_info.ra_colour;

      new_surround_db_entry_ptr->system_info_timer_count = MAX_SYS_INFO_COUNT_VALUE;

      /* Put the scell into the best 6 */
      rr_gprs_add_to_best_six(
        rr_gprs_get_scell_arfcn(gas_id),
        rr_gprs_get_scell_bsic(gas_id),
        FALSE,
        gas_id
      );

      /*******************************************************************/
      /* Since this is the last serving cell being entered into the      */
      /* surrounding database GSM 05.08 indicates that the penalty timer */
      /* state should be created in an EXPIRED state (see GSM 05.08 6.4)  */
      /*******************************************************************/
      rr_gprs_create_expired_penalty_timer(
        rr_resel_best_six_find(
          rr_gprs_get_scell_arfcn(gas_id),
          &new_surround_db_entry_ptr->gsm.BSIC,
          gas_id
        ),
        gas_id
      );
    }

    MSG_GERAN_HIGH_0_G("NACC transition 51 -> 51");

    /*****************************************************************/
    /* Create a pseudo BA list that consists of the scell and ncells */
    /* that GRR had SI3s for. This is because there will be no SI2   */
    /* for a PNCD reselection, and therefore no ncells.              */
    /*****************************************************************/
    for( index = 0; index < best_six_ncells_ptr->no_of_entries ; index++ )
    {
      pcell_info->gsm.complete_BA_list.BCCH_ARFCN[index] = best_six_ncells_ptr->ncell_entry[index].ncell_data->arfcn;
      pcell_info->gsm.complete_BA_list.no_of_entries++;
    }
    pcell_info->gsm.complete_idle_BA_list_received = FALSE;
    pcell_info->gsm.complete_BA_list.BA_IND       |= 0x02; /* add the flag */
    pcell_info->gsm.SI2_BA_list.BA_IND            |= 0x02; /* add the flag */
    pcell_info->gsm.SI2bis_BA_list.BA_IND         |= 0x02; /* add the flag */
    pcell_info->gsm.SI2ter_BA_list.BA_IND         |= 0x02; /* add the flag */
  }
  /* Set up the pcell ready for fresh SI/PSIs */
  pcell_info->bcch_sys_info.si13_data_valid_flag = FALSE;
  pcell_info->gsm.generate_si_received_event    = TRUE;
  pcell_info->gsm.update_message_received       = 0;
  pcell_info->gsm.valid_data_flag               = FALSE;
  pcell_info->gsm.camped_on_flag                = FALSE;

#ifdef FEATURE_WCDMA
  /* need to clear out the SI2quater 3G neighbour cell information as we're now going to read PSIs */
  rr_initialize_wcdma_idle_neighbor_list(&pcell_info->gsm.utran_neighbor_list);

  /* also need to clear out the separate 3G system information store */
  rr_init_sys_info_3g(gas_id);
#endif /* FEATURE_WCDMA */

  /* copy "NCC permitted" - it is likely this will be the same */
  pcell_info->gsm.NCC_permitted = scell_info_ptr->gsm.NCC_permitted;
}

/****************************************************************************
 *   Function name:  rr_gprs_is_pncd_enough_to_camp
 *   -------------
 *
 *   Description:
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     Void
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_is_pncd_enough_to_camp(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *pcell_info = rr_gprs_get_pcell_info(gas_id);
  boolean            camping_allowed = FALSE;

  /*************************************************************************************/
  /* When the target cell is a BCCH cell at least SI3 and SI13 must have been received */
  /* before camping. SI1 should have also been sent if it is broadcast                 */
  /*************************************************************************************/
  if (ARE_SET(MASK_SI_3 | MASK_SI_13, pcell_info->gsm.update_message_received))
  {
    MSG_GERAN_HIGH_1_G("target cell gprs indicator=%d", pcell_info->gsm.gprs_indicator);
    camping_allowed = pcell_info->gsm.gprs_indicator;
  }

  return camping_allowed;
}


/**
 * Function:     rr_gprs_load_pccf_from_pcco
 * Description:  Uses the given PCCO message to populate the fields of the default
 *               PCCF message in case it needs to be sent at some point.
 * Parameters:   pcco - the source PCCO message received OTA.
 * Returns:      void;
 */
static void rr_gprs_load_pccf_from_pcco(
  const packet_cell_change_order_t *pcco,
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  memset(&rr_resel_data_ptr->pcco_data.pccf, 0, sizeof(rr_resel_data_ptr->pcco_data.pccf));

  if (pcco->gsm_3g_cell_type == CSN_PCCO_GSM_TARGET_CELL)
  {
    rr_resel_data_ptr->pcco_data.pccf.arfcn = pcco->cell.gsm.arfcn;
    rr_resel_data_ptr->pcco_data.pccf.bsic = pcco->cell.gsm.bsic;
    rr_resel_data_ptr->pcco_data.pccf.tlli = rr_gmminfo_get_tlli(gas_id);
  }
  else /* CSN_PCCO_3G_TARGET_CELL */
  {
    rr_resel_data_ptr->pcco_data.pccf.arfcn.num = 0;
    rr_resel_data_ptr->pcco_data.pccf.arfcn.band = (sys_band_T) 0;
    rr_resel_data_ptr->pcco_data.pccf.bsic = 0;
    rr_resel_data_ptr->pcco_data.pccf.tlli = rr_gmminfo_get_tlli(gas_id);
    rr_resel_data_ptr->pcco_data.pccf.rel99 = valid;
    if (pcco->cell._3g.fdd_flag == valid)
    {
      rr_resel_data_ptr->pcco_data.pccf.fdd_flag = valid;
      rr_resel_data_ptr->pcco_data.pccf.fdd_arfcn = pcco->cell._3g.fdd_arfcn;
      rr_resel_data_ptr->pcco_data.pccf.fdd_diversity = pcco->cell._3g.fdd_diversity;
      rr_resel_data_ptr->pcco_data.pccf.fdd_bandwidth_flag = pcco->cell._3g.fdd_bandwidth_flag;
      rr_resel_data_ptr->pcco_data.pccf.fdd_bandwidth = pcco->cell._3g.fdd_bandwidth;
      rr_resel_data_ptr->pcco_data.pccf.fdd_scrambling_code = pcco->cell._3g.fdd_scrambling_code;
    }
    if (pcco->cell._3g.tdd_flag == valid)
    {
      rr_resel_data_ptr->pcco_data.pccf.tdd_flag = valid;
      rr_resel_data_ptr->pcco_data.pccf.tdd_arfcn = pcco->cell._3g.tdd_arfcn;
      rr_resel_data_ptr->pcco_data.pccf.tdd_diversity = pcco->cell._3g.tdd_diversity;
      rr_resel_data_ptr->pcco_data.pccf.tdd_bandwidth_flag = pcco->cell._3g.tdd_bandwidth_flag;
      rr_resel_data_ptr->pcco_data.pccf.tdd_bandwidth = pcco->cell._3g.tdd_bandwidth;
      rr_resel_data_ptr->pcco_data.pccf.tdd_cell_param = pcco->cell._3g.tdd_cell_param;
      rr_resel_data_ptr->pcco_data.pccf.tdd_sync_case = pcco->cell._3g.tdd_sync_case;
    }
  }
}

/**
 * Function name: rr_gprs_store_serving_cell_details
 * Description:   Stores serving cell details such as ARFCN, Multiframe and
 *                Mobile Allocation information so that if a PCCO fails it
 *                is possible to return to the old serving cell without needing
 *                to reacquire all SIs.
 * Parameters:    void;
 * Returns:       void;
 */
static void rr_gprs_store_serving_cell_details(
  const gas_id_t gas_id
)
{
  rr_resel_data_t    *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  gprs_scell_info_t  *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(rr_resel_data_ptr);
  RR_NULL_CHECK_FATAL(scell_info_ptr);

  rr_resel_data_ptr->old_scell.BCCH_ARFCN = scell_info_ptr->gsm.BCCH_ARFCN;
  rr_resel_data_ptr->old_scell.BSIC = scell_info_ptr->gsm.BSIC;

  if (scell_info_ptr->gsm.control_channel_description_valid)
  {
    rr_resel_data_ptr->old_scell.control_channel_description_valid = TRUE;
    rr_resel_data_ptr->old_scell.control_channel_description = scell_info_ptr->gsm.control_channel_description;
  }
  else
  {
    rr_resel_data_ptr->old_scell.control_channel_description_valid = FALSE;
  }

  rr_resel_data_ptr->rr_gprs_old_scell.reselection_data.BCCH_ARFCN = rr_gprs_get_scell_arfcn(gas_id);
}



/****************************************************************************
 *   Function name:  rr_gprs_process_PCCO
 *   -------------
 *
 *   Description:   Function to process the PCCO, and extract the parameters
 *                  in order to validate them before reselection takes place
 *
 *   Parameters:  mac_grr_data_ind_t
 *   -----------
 *
 *   Returns:  rr_event_T
 *   --------
 *
 ****************************************************************************/
static rr_event_T rr_gprs_process_PCCO(
  packet_cell_change_order_t *pcco,
  const gas_id_t gas_id
)
{
  rr_resel_data_t                    *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  rr_event_T                          output_event = EV_NO_EVENT;
  gprs_better_cell_list_T            *better_cell_list_ptr = rr_get_better_cell_list_ptr(gas_id);

  gprs_surround_data_t gprs_surround_data;
  memset(&gprs_surround_data, 0, sizeof(gprs_surround_data_t));

  rr_gprs_load_pccf_from_pcco(pcco, gas_id);
  rr_gprs_store_serving_cell_details(gas_id);
  rr_gprs_cache_current_LAI(gas_id);

  if (pcco->gsm_3g_cell_type == CSN_PCCO_GSM_TARGET_CELL)
  {
    /* If MAC is not in transfer then ignore the IMMEDIATE REL flag */
    if (rr_macinfo_get_mac_status(gas_id) == MAC_IDLE)
    {
      pcco->immediate_rel = FALSE;
    }

    /****************************************************/
    /* Check that the frequency is supported by the MS */
    /****************************************************/
    if (!rr_gapi_is_supported_band((sys_band_T)GET_ARFCN_BAND(pcco->cell.gsm.arfcn), gas_id))
    {
      MSG_GERAN_HIGH_1_G("Band not allowed for arfcn %d - PCCF",pcco->cell.gsm.arfcn.num);
      rr_gprs_send_pccf(FREQ_NOT_IMPLEMENTED, gas_id);
    }
    else
    {
      MSG_GERAN_HIGH_2_G("PCCO to ARFCN %d, band %d", pcco->cell.gsm.arfcn.num, pcco->cell.gsm.arfcn.band);
      /* 3GPP Spec CR:
      http://www.3gpp.org/ftp/tsg_geran/TSG_GERAN/GERAN_19_Cancun/Docs/GP-040813.zip

      Blind PCCO is no longer supported or required.  PCCF must be sent instead. */
      if (!rr_gprs_is_cell_in_sync_list(pcco->cell.gsm.arfcn, gas_id))
      {
        MSG_GERAN_HIGH_2_G("Blind PCCO to %d (band %d) not supported, send PCCF", pcco->cell.gsm.arfcn.num, pcco->cell.gsm.arfcn.band);
        rr_gprs_send_pccf(FREQ_NOT_IMPLEMENTED, gas_id);
      }
      else
      {
        /* determine the output event, which may be overwritten if NACC is involved */
        if (pcco->immediate_rel)
        {
          output_event = EV_ACTION_IMMEDIATE_PCCO;
        }
        else
        {
          output_event = EV_ACTION_PCCO;
        }

        if (pcco->cell.gsm.container_flag == valid)
        {
          uint8 container_id = pcco->cell.gsm.container_id;

          /* a container ID has been specified in the PCCO message */
          if (rr_gprs_nacc_container_empty(container_id, gas_id))
          {
            MSG_GERAN_ERROR_1_G("PCCO includes container ID (%d), but no data is available",
                      (int)container_id);

            /* cancel container timer and discard any stored data in GMAC */
            (void)rr_timer_cancel((rr_timer_id_T)(container_id + GRR_NACC_CONTAINER_TIMER_0), gas_id);
            rr_send_grr_mac_nacc_release_req(container_id, NACC_RELEASE_ONE, gas_id);

            /* note that the NACC state machine is left alone, so that it will */
            /* still initiate the sys info status procedure on the target cell */
          }
          else
          {
            /* Set up the pending channel database ARFCN */
            gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

            pcell_info_ptr->gsm.BCCH_ARFCN = pcco->cell.gsm.arfcn;

            /* trigger stored neighbour cell data for this container*/
            (void) rr_gprs_nacc_trigger_ncell_data(
              container_id,
              pcco->cell.gsm.arfcn,
              TRUE,
              gas_id
            );

            /* TODO*/
            /* and update the return event to process it */
            output_event = EV_ACTION_PCCO_WITH_PNCD;
          }
        }

        /*Just use the defaults*/
        gprs_surround_data.bcch_params.si13_normal_or_ext = BCCH_NORMAL;
        /*Request L1 to read the candidate surrounding ncells SI3 and 4*/
        gprs_surround_data.bcch_params.bcch_arfcn = pcco->cell.gsm.arfcn;

        /*******************************************************/
        /* Add the ordered target cell to the better cell list */
        /* this is so that the rr_gprs_target_cell can be set  */
        /* to point to it                                      */
        /*******************************************************/
        better_cell_list_ptr->no_of_entries = 1;
        better_cell_list_ptr->candidate_cell[0].cell_reselection_attempted = FALSE;
        better_cell_list_ptr->candidate_cell[0].reselection_data.BCCH_ARFCN = pcco->cell.gsm.arfcn;

        /*Set the target to point to the better cell entry*/
        rr_resel_data_ptr->rr_gprs_target_cell = &better_cell_list_ptr->candidate_cell[0];
      }
    }

    return output_event;
  }
#ifdef FEATURE_WCDMA
#ifdef FEATURE_INTERRAT_PCCO_GTOW
  else
  {
    /**
     * PCCO has been received with a target 3G cell
     */
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

    if (pcco->cell._3g.fdd_flag)
    {
      MSG_GERAN_HIGH_3_G("PCCO msg wth 3G FDD cell info received.UARFCN %d,PSC %d diversity %d",
                 pcco->cell._3g.fdd_arfcn,
                 pcco->cell._3g.fdd_scrambling_code,
                 pcco->cell._3g.fdd_diversity
               );

#ifdef FEATURE_WCDMA
      if (!rr_is_wcdma_enabled(gas_id))
      {
        MSG_GERAN_ERROR_0_G("WCDMA not supported, ignore PCCO to 3G");
        rr_gprs_send_pccf(FREQ_NOT_IMPLEMENTED, gas_id);
      }
      else if (!rr_g2x_is_uarfcn_valid(
                  pcco->cell._3g.fdd_arfcn,
                  scell_info_ptr->gsm.location_area_identification.plmn_id,
                  gas_id
                ))
      {
        MSG_GERAN_ERROR_1_G("UARFCN invalid, PCCO to 3G failure", pcco->cell._3g.fdd_arfcn);
        rr_gprs_send_pccf(FREQ_NOT_IMPLEMENTED, gas_id);
      }
      else
      {
        rr_irat_resel_target_set(
          pcco->cell._3g.fdd_arfcn,
          pcco->cell._3g.fdd_scrambling_code,
          PN_OFFSET_UNKNOWN,
          pcco->cell._3g.fdd_diversity,
          gas_id
        );

        MSG_GERAN_MED_3_G("PCCO for 3G target: UARFCN %d, SC %d, Diversity %d",
                pcco->cell._3g.fdd_arfcn,
                pcco->cell._3g.fdd_scrambling_code,
                pcco->cell._3g.fdd_diversity);

        output_event = RR_EV_G2W_PCCO_STARTED;
      }
#endif /* FEATURE_WCDMA */
    }
    else if (pcco->cell._3g.tdd_flag)
    {
      MSG_GERAN_ERROR_0_G("PCCO with unsupported 3G TDD target cell, send PCCF");
      rr_gprs_send_pccf(FREQ_NOT_IMPLEMENTED, gas_id);
    }
    else
    {
      MSG_GERAN_ERROR_0_G("PCCO with unknown 3G target cell, send PCCF");
      rr_gprs_send_pccf(FREQ_NOT_IMPLEMENTED, gas_id);
    }
  }
#else
  else
  {
    /**
     * PCCO has been received with a target 3G cell, but we don't support this
     */
    MSG_GERAN_ERROR_0_G("PCCO with unsupported 3G target cell, send PCCF");
    rr_gprs_send_pccf(FREQ_NOT_IMPLEMENTED, gas_id);
  }
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
  /* if the WCDMA feature is not defined, UE should not declare support for this */
#endif /* FEATURE_WCDMA */

  return output_event;
}




#ifdef FEATURE_GPRS_PS_HANDOVER


/*===========================================================================

FUNCTION rr_gprs_reselection_process_psho_command

DESCRIPTION


PARAMS


DEPENDENCIES


RETURN VALUE


===========================================================================*/
static rr_event_T rr_gprs_reselection_process_psho_command(
  ps_handover_command_t *psho,
  const gas_id_t gas_id
)
{
  rr_resel_data_t               *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  ps_handover_radio_resources_t *psho_target = &(psho->payload.a_gb.ps_handover_radio_resources);
  BSIC_T                         bsic;
  gprs_better_cell_list_T       *better_cell_list_ptr = rr_get_better_cell_list_ptr(gas_id);
  gprs_scell_info_t             *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  rr_gprs_load_pccf_from_psho(psho, gas_id);

  if (!rr_gapi_is_supported_band((sys_band_T)GET_ARFCN_BAND(psho_target->arfcn), gas_id))
  {
    MSG_GERAN_HIGH_1_G("Band not allowed for arfcn %d - PCCF",psho_target->arfcn.num);
    rr_gprs_send_pccf(PSHO_FAILURE_REASON_OTHERS, gas_id);
    return EV_NO_EVENT;
  }

  if (rr_gmminfo_get_gmm_status(gas_id) != GMM_READY_STATE)
  {
    MSG_GERAN_HIGH_0_G("Reject PSHO, GMM is not in ready state");
    rr_gprs_send_pccf(PSHO_FAILURE_REASON_OTHERS, gas_id);
    return EV_NO_EVENT;
  }

  if (rr_macinfo_get_mac_status(gas_id) != MAC_TRANSFER)
  {
    MSG_GERAN_HIGH_0_G("Reject PSHO, MAC is not in transfer");
    rr_gprs_send_pccf(PSHO_FAILURE_REASON_OTHERS, gas_id);
    return EV_NO_EVENT;
  }

  if (rr_gprs_nacc_container_empty(psho->container_id, gas_id))
  {
    MSG_GERAN_ERROR_1_G("PSHO includes container ID (%d), but no data is available",
              (int)psho->container_id);

    /* cancel container timer and discard any stored data in GMAC */
    (void)rr_timer_cancel((rr_timer_id_T)(psho->container_id + GRR_NACC_CONTAINER_TIMER_0), gas_id);
    rr_send_grr_mac_nacc_release_req(psho->container_id, NACC_RELEASE_ONE, gas_id);
    rr_gprs_send_pccf(PSHO_FAILURE_REASON_OTHERS, gas_id);
    return EV_NO_EVENT;
  }

  CONVERT_TO_BSIC_T(bsic, psho_target->bsic);
  rr_prepare_sidb_for_camping(&pcell_info_ptr->gsm, psho_target->arfcn, bsic, 0xFF, gas_id);
  pcell_info_ptr->gsm.generate_si_received_event = FALSE;

  better_cell_list_ptr->no_of_entries = 1;
  better_cell_list_ptr->candidate_cell[0].cell_reselection_attempted = FALSE;
  better_cell_list_ptr->candidate_cell[0].reselection_data.BCCH_ARFCN = psho_target->arfcn;

  /*Set the target to point to the better cell entry*/
  rr_resel_data_ptr->rr_gprs_target_cell = &better_cell_list_ptr->candidate_cell[0];

  /* request any stored ncell data now get sent */
  (void)rr_gprs_nacc_trigger_ncell_data(psho->container_id, psho_target->arfcn, TRUE, gas_id);

  /* make this cell the only possible target */
  rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN = psho_target->arfcn;;

  /* Prepare the database for pncd nacc reselection */
  rr_gprs_prepare_for_reselection_with_PNCD(INVALID_CONTAINER_ID, gas_id);

  return EV_ACTION_PSHO;
}
#endif /* FEATURE_GPRS_PS_HANDOVER */


/**
  @brief Returns the reselection state machine's current state as a string.
*/
const char *rr_resel_current_state_name(
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  return rr_resel_state_name(rr_resel_data_ptr->state);
}


/****************************************************************************
 *   Function name:  rr_gprs_set_parameters_available
 *   -------------
 *
 *   Description: This function is called to check whether the GPRS parameters
 *   ------------ have been received whilst camped on BCCH. If they have the
 *                rr_gprs_set_parameters_available_flag is set.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  TRUE/FALSE
 *   --------
 *
 ****************************************************************************/
static void rr_gprs_set_parameters_available(
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  gprs_scell_info_t *scell_info = rr_gprs_get_scell_info(gas_id);
  boolean old_gprs_parameters_available_flag = rr_resel_data_ptr->gprs_parameters_available_flag;

  if (rr_gprs_pmo_pcco_ncells_are_present(gas_id) == TRUE ||
      scell_info->bcch_sys_info.pmo_scell_params_valid)
  {
    rr_resel_data_ptr->gprs_parameters_available_flag = TRUE;
  }
  else
  {
    rr_resel_data_ptr->gprs_parameters_available_flag = FALSE;
  }

  if (old_gprs_parameters_available_flag != rr_resel_data_ptr->gprs_parameters_available_flag)
  {
    rr_remove_all_best_six_ncells(gas_id);
  }
}

static void rr_resel_send_cell_acq_start_ind(
  rr_cell_acq_action_e       action,
  rr_resel_clear_si_e        clear_si,
  const gas_id_t             gas_id
)
{
  rr_resel_data_t  *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  boolean           sync_flag         = FALSE;
  boolean           clear_si_database = FALSE;
  bcch_ncell_info_t *bcch_ncell_info_ptr;
  PLMN_id_T         *plmn_id_ptr;

  RR_NULL_CHECK_FATAL(rr_resel_data_ptr);

  if (rr_gprs_is_cell_in_sync_list(
        rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
        gas_id
      ))
  {
    if (action == RR_CELL_ACQ_ACTION_RESELECT_BCCH_AFTER_SU ||
        action == RR_CELL_ACQ_ACTION_RESELECT_BCCH_WITH_GPRS_CELL_SELECTION_PARAMS)
    {
      sync_flag = TRUE;
    }
  }

  if (clear_si == RR_RESEL_CLEAR_SI_DB)
  {
    clear_si_database = TRUE;
  }

  bcch_ncell_info_ptr = rr_gprs_get_surround_ncell_sys_info(
    rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
    gas_id
  );

  if ((bcch_ncell_info_ptr != NULL) && (!rr_is_any_cell_reselection_mode(gas_id)))
  {
    plmn_id_ptr = &bcch_ncell_info_ptr->gsm.location_area_identification.plmn_id;
  }
  else
  {
    plmn_id_ptr = NULL;
  }

  rr_cell_acq_send_imsg_start_ind(
    rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN,
    rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.RXLEV_average,
    NULL,
    rr_resel_data_ptr->recent_reselection_penalty_flag,
    action,
    sync_flag,
    clear_si_database,
    rr_gprs_get_pcell_info(gas_id),
    plmn_id_ptr,
    gas_id
  );
}

#ifdef FEATURE_LTE

/**
  @brief Determines if reselection to lower priority layers is allowed.

*/
static boolean rr_resel_to_lower_priority_layers_permitted(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

  uint8 thresh_gsm_low = scell->serving_cell_priority_params.thresh_gsm_low; 

  if (thresh_gsm_low == 15)
  {
    MSG_GERAN_HIGH_0_G("THRESH_gsm_low == 15, reselection to lower priority layers permitted");
    return TRUE;
  }
  else
  {
    if (rr_resel_greatest_gsm_s_value(TRUE, gas_id) < (thresh_gsm_low * 2))
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
}

void rr_resel_cancel_all_priority_based_resel_timers(
  const gas_id_t gas_id
)
{
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_1, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_2, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_3, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_4, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_5, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_6, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_7, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_8, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_9, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_10, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_11, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_12, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_1, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_2, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_3, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_4, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_5, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_6, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_7, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_8, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_9, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_10, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_11, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_12, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_1, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_2, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_3, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_4, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_5, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_6, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_7, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_8, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_9, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_10, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_11, gas_id);
  (void)rr_timer_cancel(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_12, gas_id);
}

void rr_resel_cancel_all_rank_based_resel_timers(
  const gas_id_t gas_id
)
{
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_1, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_2, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_3, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_4, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_5, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_6, gas_id);
}

#endif /* FEATURE_LTE */

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 *---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION rr_resel_init

DESCRIPTION
  Resets the cell reselection state machine and variables etc

PARAMS
  NONE

DEPENDENCIES

RETURN VALUE

===========================================================================*/
void rr_resel_init(
  const gas_id_t gas_id
)
{
  rr_resel_data_t * rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  rr_resel_data_ptr->recent_reselection_penalty_flag = FALSE;
  rr_resel_data_ptr->gprs_parameters_available_flag  = FALSE;
  rr_resel_data_ptr->rr_gprs_reselection_control     = MS_CONTROL;
  rr_resel_data_ptr->measurements_expected           = RR_MASK_GSM_SURROUND_MEAS |
                                                   RR_MASK_GSM_SERV_MEAS;

  /* Reset the sync cell list */
  rr_gprs_clear_sync_cell_list(gas_id);

  /* Reset the reselection list structures for the next reporting period */
  rr_gprs_reset_reselection_list(gas_id);

  rr_nacc_init(gas_id);

  grr_reset_reselection_power_scan(gas_id);

  if (rr_timer_cancel(RR_BCCH_DECODE_COMPLETE_TIMER, gas_id))
  {
    MSG_GERAN_LOW_0_G("Stopped RR_BCCH_DECODE_COMPLETE_TIMER");
  }

  /* Stop any potentially running timers */
  if (rr_timer_cancel(CELL_RESELECTION_TIMER, gas_id))
  {
    MSG_GERAN_HIGH_0_G("Stopped the CELL_RESELECTION_TIMER");
  }

  if (rr_timer_cancel(PATH_LOSS_TIMER, gas_id))
  {
    MSG_GERAN_LOW_0_G("Stopped the PATH LOSS TIMER");
  }

  if (rr_timer_cancel(RESELECTION_HYSTERESIS, gas_id))
  {
    MSG_GERAN_LOW_0_G("Stopped the RECENT RESELECTION TIMER");
  }

#ifdef FEATURE_IDLE_DRX_SCALING
  if (rr_timer_cancel(RR_IDLE_DRX_SCALING_SUSPEND_TIMER, gas_id))
  {
    MSG_GERAN_HIGH_0_G("Stopped the IDLE_DRX_SCALING_SUSPEND TIMER");
  }
#endif /* FEATURE_IDLE_DRX_SCALING */

  if (rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id))
  {
    MSG_GERAN_LOW_0_G("Stopped the RR_GPRS_SYS_INFO_TIMER");
  }

  if (rr_timer_cancel(GRR_SURROUND_MEAS_TIMER, gas_id))
  {
    MSG_GERAN_LOW_0_G("Stopped the GRR_SURROUND_MEAS_TIMER");
  }

  if (rr_timer_cancel(T3158, gas_id))
  {
    MSG_GERAN_LOW_0_G("Stopped T3158");
  }

  /* Reset the measurement reporting state machine here as we have
  just cancelled it's timer (T3158).  It is possible that there is
  already a MID_TIMER_EXPIRY(T3158) on RR's queue - if we do not reset
  the measurement reporting state machine here then it is possible that
  this timer expiry message will trigger a fatal error if T3158 is restarted
  due to BS_PA_MFRMS being zero. */
  (void) rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, NULL, gas_id);

  rr_gprs_init_bad_snr_counter(gas_id);

  rr_resel_data_ptr->rr_gprs_old_scell.reselection_data.BCCH_ARFCN.num = INVALID_ARFCN;
  rr_resel_data_ptr->rr_gprs_old_scell.reselection_data.BCCH_ARFCN.band = (sys_band_T)BAND_NONE;
  rr_resel_data_ptr->rr_gprs_old_scell.cell_reselection_attempted  = FALSE;

  rr_remove_all_best_six_ncells(gas_id);

  rr_clear_better_cell_list(gas_id);

  MSG_GERAN_HIGH_0_G("Clearing PCCC/PCCO flags");
  rr_resel_data_ptr->nacc_pccc_received = FALSE;
  rr_resel_data_ptr->nacc_pcco_received = FALSE;
}

/**
 * rr_resel_set_pccf_cause_to_assignment_reject_on_target
 *
 * Sets the cause value for a PCCF to ASSIGNMENT_REJECT_ON_TARGET.
 */
void rr_resel_set_pccf_cause_to_assignment_reject_on_target(
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  rr_resel_data_ptr->pcco_data.pccf.cause = (uint8) ASSIGNMENT_REJECT_ON_TARGET;
}

/**
 * rr_resel_set_pccf_cause_to_no_response_on_target
 *
 * Sets the cause value for a PCCF to NO_RESPONSE_ON_TARGET.
 */
void rr_resel_set_pccf_cause_to_no_response_on_target(
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  rr_resel_data_ptr->pcco_data.pccf.cause = (uint8) NO_RESPONSE_ON_TARGET;
}

/**
 * rr_pcco_in_progress
 *
 * This function returns TRUE if a PCCO reselection to a target cell is in
 * progress, or if RR is camped on a target cell waiting for an initial
 * cell access attempt (e.g. T3174 is running).
 */
boolean rr_pcco_in_progress(
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  if (rr_timer_is_running(T3174, gas_id))
  {
    if (rr_resel_data_ptr->state == RR_RESEL_STATE_PCCO ||
        rr_resel_data_ptr->state == RR_RESEL_STATE_PCCO_ABORTING_CELL_ACQ ||
        rr_resel_data_ptr->state == RR_RESEL_STATE_PCCO_PENDING_L1_IDLE_CNF)
    {
      MSG_GERAN_MED_0_G("T3174 running and rr_resel_control() state is PCCO / PCCO_ABORTING_CELL_ACQ / PCCO_PENDING_L1_IDLE_CNF, PCCO in progress");
      return TRUE;
    }
    else if (rr_resel_data_ptr->state == RR_RESEL_STATE_IDLE)
    {
      MSG_GERAN_MED_0_G("T3174 running and rr_resel_control() state is IDLE, PCCO in progress");
      return TRUE;
    }
    else
    {
      MSG_GERAN_ERROR_1_G("T3174 is running, but reselection_control() is not in an appropriate state (%d)!",
        rr_resel_data_ptr->state);
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}

/**
 * rr_failed_pcco_in_progress
 *
 * This function returns TRUE if a PCCO reselection has failed and RR is
 * in the process of camping on the old serving cell (e.g. T3176 is running).
 */
boolean rr_failed_pcco_in_progress(
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  if (rr_timer_is_running(T3176, gas_id))
  {
    if (rr_resel_data_ptr->state == RR_RESEL_STATE_PCCO_FAILED||
        rr_resel_data_ptr->state == RR_RESEL_STATE_PCCO_FAILED_ABORTING_CELL_ACQ ||
        rr_resel_data_ptr->state == RR_RESEL_STATE_PCCO_FAILED_PENDING_L1_IDLE_CNF)
    {
      return TRUE;
    }
    else if (rr_resel_data_ptr->state == RR_RESEL_STATE_IDLE)
    {
      return TRUE;
    }
    else
    {
      MSG_GERAN_ERROR_1_G("T3176 is running, but reselection_control() is not in an appropriate state (%d)!",
        rr_resel_data_ptr->state);
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}


#ifdef FEATURE_GPRS_PS_HANDOVER

/*===========================================================================

FUNCTION rr_gprs_load_pccf_from_psho

DESCRIPTION


PARAMS


DEPENDENCIES


RETURN VALUE


===========================================================================*/
void rr_gprs_load_pccf_from_psho(
  ps_handover_command_t *psho,
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  memset(&rr_resel_data_ptr->pcco_data.pccf, 0, sizeof(rr_resel_data_ptr->pcco_data.pccf));

  if (psho->target_type == CSN_PSHO_GSM_TARGET_CELL)
  {
    ps_handover_radio_resources_t *psho_target =
      &(psho->payload.a_gb.ps_handover_radio_resources);
    rr_resel_data_ptr->pcco_data.pccf.arfcn = psho_target->arfcn;
    rr_resel_data_ptr->pcco_data.pccf.bsic = psho_target->bsic;
    rr_resel_data_ptr->pcco_data.pccf.tlli = rr_gmminfo_get_tlli(gas_id);
  }
  else /* CSN_PCCO_3G_TARGET_CELL */
  {
    rr_resel_data_ptr->pcco_data.pccf.arfcn.num = 0;
    rr_resel_data_ptr->pcco_data.pccf.arfcn.band = (sys_band_T) 0;
    rr_resel_data_ptr->pcco_data.pccf.bsic = 0;
    rr_resel_data_ptr->pcco_data.pccf.tlli = rr_gmminfo_get_tlli(gas_id);
    rr_resel_data_ptr->pcco_data.pccf.rel99 = valid;
    rr_resel_data_ptr->pcco_data.pccf.fdd_flag = not_valid;
  }
}

#endif /* FEATURE_GPRS_PS_HANDOVER */

/****************************************************************************
 *   Function name:  rr_gprs_send_pccf
 *   -------------
 *
 *   Description:   sets up and sends the pccf message to MAC
 *
 *   Parameters:  arfcn and CAUSE
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_send_pccf(
  pccf_cause_t cause,
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  if (cause != USE_PRELOADED_CAUSE)
  {
    rr_resel_data_ptr->pcco_data.pccf.cause = (uint8)cause;
  }

  if ((cause == ONGOING_CS_CONNECTION) || (cause == ANONYMOUS_ACCESS))
  {
    /* these cause values are not used anywhere in the code */
    MSG_GERAN_ERROR_1_G("WARNING: unexpected PCCF cause value %d", (int)cause);
  }

  /* reset the NACC state machine, no need to bother sending any  */
  /* sys info status after returning to the original serving cell */
  rr_gprs_nacc_notify_event(CCN_RESET, gas_id);

  rr_send_grr_mac_send_pccf(rr_resel_data_ptr->pcco_data.pccf, gas_id);

  rr_resel_data_ptr->pccf_transmission_in_progress = TRUE;
}

/****************************************************************************
 *   Function name:  rr_gprs_parameters_available
 *   -------------
 *
 *   Description: This function is called to check whether the GPRS parameters
 *   ------------ have been received whilst camped on BCCH. If they have the
 *                reselection criteria and algorithm must change to the GPRS
 *                algorithm in GSM05.08 10.1.2 & 10.1.3. This is called in many
 *                places throunghout the cell reselection code.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  TRUE/FALSE
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_parameters_available(
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  return rr_resel_data_ptr->gprs_parameters_available_flag;
}

/**
  @brief Sets the recent_reselection_penalty_flag to TRUE and starts
         the RESELECTION_HYSTERESIS timer.
*/
void rr_resel_w2g_reselection_performed(
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  rr_resel_data_ptr->recent_reselection_penalty_flag = TRUE;

  MSG_GERAN_HIGH_3_G("Starting the RESELECTION_HYSTERESIS timer (%d ms)",
    RESELECTION_HYSTERESIS_TIMEOUT,
    0,
    0
  );

  (void)rr_timer_start(RESELECTION_HYSTERESIS, RESELECTION_HYSTERESIS_TIMEOUT, gas_id);
}



/****************************************************************************
 *   Function name:  rr_gprs_reset_reselection_list
 *   -------------
 *
 *   Description:   cleans out the reselection measurements from the list,
 *                  and sets the measurements received flag to none.
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_reset_reselection_list(
  const gas_id_t gas_id
)
{
  rr_resel_data_t              *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  gprs_reselection_structure_T *gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  rr_resel_data_ptr->measurements_received = RR_NO_MEASUREMENTS_RECEIVED;

  gprs_reselection_list_ptr->no_of_entries = 0;
  gprs_reselection_list_ptr->serving_values_valid = FALSE;
  gprs_reselection_list_ptr->serving_values.BCCH_ARFCN.num = INVALID_ARFCN;

  gprs_reselection_list_ptr->transfer_serving_snr_is_bad = FALSE;
  gprs_reselection_list_ptr->idle_serving_snr_is_bad = FALSE;

  gprs_reselection_list_ptr->serving_values.BCCH_ARFCN.band = (sys_band_T)BAND_NONE;
}

#ifdef FEATURE_IDLE_DRX_SCALING
/****************************************************************************
 *   Function name:  rr_gprs_reset_nmeas_in_reselection_list
 *   -------------
 *
 *   Description:   cleans out the ncell reselection measurements from the list
 *                  and sets the measurements received flag to none.
 *
 *   Parameters:  GAS ID
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_reset_nmeas_in_reselection_list(const gas_id_t gas_id)
{
  rr_resel_data_t              *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  gprs_reselection_structure_T *gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  gprs_reselection_list_ptr->no_of_entries = 0;
  rr_resel_data_ptr->measurements_received = RR_NO_MEASUREMENTS_RECEIVED;
}
#endif /* FEATURE_IDLE_DRX_SCALING */

/*Access function to return the value of the recent reselection flag*/
boolean get_recent_reselection_flag_value(
  const gas_id_t gas_id
)
{
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);

  return rr_resel_data_ptr->recent_reselection_penalty_flag;
}



/**
  @brief Determine whether or not the reselection state machine is in it's
         CCN reselection state.

  @return TRUE if the reselection state machine is in it's CCN reselection
          state.
  @return FALSE if the reselection state machine is not in it's CCN reselection
          state.
*/
boolean rr_resel_is_in_ccn_reselection_state(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_data_ptr);

  if (rr_resel_data_ptr->state == RR_RESEL_STATE_NACC_PENDING_NW_RESPONSE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

boolean rr_gprs_nacc_check_pccn_flag(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_data_ptr);

  return ((rr_resel_data_ptr->pccn.pccn_sending != 0));
}

#ifdef FEATURE_WCDMA

/****************************************************************************
 *   Function name:  rr_gprs_nacc_prepare_for_wtog_cco
 *   -------------
 *
 *   Description: Prepares the appropriate databases for W->G NACC
 *   ------------
 *
 *   Parameters: target_type  - type of GERAN sys info (BCCH_PD or PBCCH_PD)
 *
 *   -----------
 *
 *   Returns:    void
 *   --------
 *
 ****************************************************************************/
void rr_gprs_nacc_prepare_for_wtog_cco(
  byte target_type,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *pcell_ptr = rr_gprs_get_pcell_info(gas_id);

  /* set a mask to cover all SIs except 1 and 3 */
  pcell_ptr->gsm.update_message_received = (MASK_SI_8     |
                                            MASK_SI_7     |
                                            MASK_SI_4     |
                                            MASK_SI_2_BIS |
                                            MASK_SI_2_TER |
                                            MASK_SI_2);

  /* for all target cell types, make sure the GSM BA list is initialised */
  pcell_ptr->gsm.complete_BA_list.no_of_entries = 0;
  pcell_ptr->gsm.complete_idle_BA_list_received = FALSE;
  pcell_ptr->gsm.complete_BA_list.BA_IND        = 0x02;
  pcell_ptr->gsm.SI2_BA_list.BA_IND             = 0x02;
  pcell_ptr->gsm.SI2bis_BA_list.BA_IND          = 0x02;
  pcell_ptr->gsm.SI2ter_BA_list.BA_IND          = 0x02;
}

#endif /* FEATURE_WCDMA */



/*===========================================================================

FUNCTION rr_resel_can_process_rr_resel_abort_req

DESCRIPTION
  Returns confirmation that rr_resel_abort_req can be processed.  This is
  state dependent.

PARAMS

DEPENDENCIES
  None

RETURN VALUE
  boolean

===========================================================================*/
boolean rr_resel_can_process_rr_resel_abort_req(
  const gas_id_t gas_id
)
{
  boolean can_process_signal = FALSE;

  // Obtain a pointer to the module data
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_data_ptr);

  switch (rr_resel_data_ptr->state)
  {
    /*fall through*/
    case RR_RESEL_STATE_GOING_INACTIVE:
    case RR_RESEL_STATE_C1_PENDING_L1_IDLE_CNF:
    case RR_RESEL_STATE_C1_ABORTING_CELL_ACQ:
    case RR_RESEL_STATE_C2_PENDING_L1_IDLE_CNF:
    case RR_RESEL_STATE_C2_ABORTING_CELL_ACQ:
    case RR_RESEL_STATE_NACC_ABORTING_CELL_ACQ:
    case RR_RESEL_STATE_NACC_PENDING_L1_IDLE_CNF:
    {
      can_process_signal = FALSE;
    }
    break;

    default:
    {
      can_process_signal = TRUE;
    }
    break;
  }

  return can_process_signal;
}

boolean rr_resel_reselection_is_pending(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_data_ptr);

  if (rr_resel_data_ptr->state == RR_RESEL_STATE_C2_PENDING_MAC_SUSPENSION)
  {
    MSG_GERAN_MED_0_G("C2 reselection is pending");
    return TRUE;
  }
  else if (rr_resel_data_ptr->state == RR_RESEL_STATE_C1 && rr_cell_acq_pending_mac_suspension(gas_id))
  {
    MSG_GERAN_MED_0_G("C1 reselection is pending");
    return TRUE;
  }
  else if (rr_resel_data_ptr->state == RR_RESEL_STATE_NACC && rr_cell_acq_pending_mac_suspension(gas_id))
  {
    MSG_GERAN_MED_0_G("NACC based  reselection is pending");
    return TRUE;
  }  
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION rr_resel_control

DESCRIPTION
  Main rr cell reselection state machine.

PARAMS
  event   - Event to be acted on.
  message - Message content

DEPENDENCIES
  None

RETURN VALUE
  rr_event_T - return control event

===========================================================================*/
rr_event_T rr_resel_control(
  rr_event_T event,
  rr_cmd_bdy_type *message,
  const gas_id_t gas_id
)
{
  byte                                message_id = 0;
  byte                                message_set = 0;
  rr_event_T                          new_event = EV_NO_EVENT;
  rr_event_T                          output_event = EV_NO_EVENT;
  gprs_reselection_structure_T       *gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
  rr_pscan_results_db_T              *rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_resel_data_t *rr_resel_data_ptr = rr_resel_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_data_ptr);

  while (event != EV_NO_EVENT)
  {
    event = rr_extract_message_header_details(
      event,
      message,
      &message_set,
      &message_id
    );

    /* State independent event/signal handler */
    switch (event)
    {
      case EV_NO_EVENT:
        continue;

      case EV_RESET_SOFTWARE:
      {
        rr_resel_init(gas_id);
        rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
        rr_resel_data_ptr->old_state = RR_RESEL_STATE_IDLE;
        event = EV_NO_EVENT;
        return EV_NO_EVENT;
      }

      case EV_INPUT_MESSAGE:
      {
        switch (message_set)
        {
          case MS_RR_L1:
          {
            switch ((rr_l1_message_id_enum_T) message_id)
            {
              case MPH_SURROUND_UPDATE_FAILURE_IND:
              case MPH_RESELECTION_CNF:
              case MPH_SELECT_SPECIFIC_BCCH_CNF:
                rr_cell_acq_control(event, message, gas_id);
                return output_event;

              default:
                /* No Action */
              break;
            }
          }
          break;

          default:
            /* no action */
          break;
        }
      }
      break;

      /****************************************************************/
      /* A PMO has been received by the measurement reporting handler */
      /* that has the NCO set to autonomous cell reselection. Upon    */
      /* reception we must check to see any new frequencies have been */
      /* added to the BA list, if so these will have GPRS reselection */
      /* parameters sent in the PMO. We must use these and from now on*/
      /* use GPRS reselection criteria and algorithms                 */
      /****************************************************************/
      case EV_CELL_RESELECTION_ACT:
      {
        if(rr_gprs_camped_on_ccch(gas_id))
        {
          /* Look to see if GPRS parameters have been received in a PMO */
          rr_gprs_set_parameters_available(gas_id);
        }
        /*Set autonomous reselection ON*/
        rr_resel_data_ptr->rr_gprs_reselection_control = MS_CONTROL;
        event = EV_NO_EVENT;
        return EV_NO_EVENT;
      }

      /********************************************************************/
      /* A PMO has been received that has NCO == 2. This enforces network */
      /* controlled cell reselection. However the MS must still take the  */
      /* measurements from L1 and calculate the GPRS C1, C31 and C32      */
      /* if GPRS reselection parameters exist as it is still required to  */
      /* use the algorithms in cases of DSF and RACH failure after max    */
      /* retrans                                                          */
      /********************************************************************/
      case EV_CELL_RESELECTION_DEACT:
      {
        if(rr_gprs_camped_on_ccch(gas_id))
        {
          /* Look to see if GPRS parameters have been received in a PMO */
          rr_gprs_set_parameters_available(gas_id);
        }

        /*Set network controlled reselection ON*/
        rr_resel_data_ptr->rr_gprs_reselection_control = NETWORK_CONTROL;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
        rr_g2wr_cancel_all_wcdma_five_second_timers(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

        if ( rr_resel_data_ptr->state != RR_RESEL_STATE_NACC_PENDING_NW_RESPONSE )
        {
          /*clear out the event */
          event = EV_NO_EVENT;
          return EV_NO_EVENT;
        }
      }
      break;

      case EV_RECENT_RESELECTION_TIMEOUT:
      {
        /*Reselection occurred over 15 seconds ago now - remove any restriction*/
        MSG_GERAN_MED_0_G("RECENT RESELECTION TIMEOUT");
        event = EV_NO_EVENT;
        rr_resel_data_ptr->recent_reselection_penalty_flag = FALSE;
        rr_cell_acq_control(EV_RECENT_RESELECTION_TIMEOUT, NULL, gas_id);
        return EV_NO_EVENT;
      }

      default:
       /*no action*/
      break;
    }

    /* State dependent event/signal handler */
    switch (rr_resel_data_ptr->state)
    {
     /**
      * RR_RESEL_STATE_IDLE
      * Idle state, no reselection is taking place.
      *
      * In this state:
      *   Measurements are used to detect path loss.
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_IDLE:
      {
        /****************************************************************/
        /* Message Handlers                                             */
        /*                                                              */
        /****************************************************************/
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch ((rr_l1_message_id_enum_T) message_id)
                {
                  case MPH_SURROUND_MEAS_IND:
                  {
                    rr_resel_data_ptr->measurements_expected = rr_cell_resel_set_meas_expected(gas_id);

                    /* stop and restart the measurement timer */
                    (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);

                    /* Process the message */
                    rr_gprs_store_surround_meas(&message->mph_surround_meas_ind, gas_id);

                    if (message->mph_surround_meas_ind.best_six_info.no_of_entries == 0)
                    {
                      MSG_GERAN_MED_0_G("Empty Meas Report");
                    }

                    /*
                     Set the surround meas received mask, so even if ncells ignored the
                     scell still gets checked for path loss.
                    */
                    if ( (rr_gprs_camped_on_ccch(gas_id) || rr_gsm_camped_on_ccch(gas_id) )
                       && ( rr_gprs_parameters_available(gas_id) == FALSE))
                    {
                      rr_resel_data_ptr->measurements_received |= RR_MASK_GSM_SURROUND_MEAS;

                      if ( ARE_ALL_BITS_SET(rr_resel_data_ptr->measurements_received, rr_resel_data_ptr->measurements_expected ))
                      {
                        new_event = EV_PATH_LOSS_TIMEOUT;
                      }
                    }
                  }
                  break;

                  case MPH_SERVING_MEAS_IND:
                  {
                    rr_resel_data_ptr->measurements_expected = rr_cell_resel_set_meas_expected(gas_id);

                    rr_gprs_store_serving_meas(&message->mph_serving_meas_ind, gas_id);

                    /**************************************************/
                    /* Start the path loss timer if it isn't running  */
                    /**************************************************/
                    if (rr_gprs_parameters_available(gas_id))
                    {
                      if (!rr_timer_is_running(PATH_LOSS_TIMER, gas_id))
                      {
                        (void)rr_timer_start( PATH_LOSS_TIMER, GPRS_PATH_LOSS_TIMEOUT, gas_id);
                        MSG_GERAN_LOW_0_G("Start the PATH LOSS TIMER");
                      }
                    }
                    else
                    {
                      rr_resel_data_ptr->measurements_received |= RR_MASK_GSM_SERV_MEAS;

                      if ( ARE_ALL_BITS_SET(rr_resel_data_ptr->measurements_received, rr_resel_data_ptr->measurements_expected ))
                      {
                        new_event = EV_PATH_LOSS_TIMEOUT;
                      }
                    }
                  }
                  break;

                  case MPH_SERVING_IDLE_MEAS_IND:
                  {
                    rr_resel_data_ptr->measurements_expected = rr_cell_resel_set_meas_expected(gas_id);

                    if (rr_gprs_is_scell_bsic_unchanged(&message->mph_serving_idle_meas_ind, gas_id))
                    {
                      rr_gprs_store_serving_idle_meas(&message->mph_serving_idle_meas_ind, gas_id);

                      /**************************************************/
                      /* Start the path loss timer if it isn't running  */
                      /**************************************************/
                      if (rr_gprs_parameters_available(gas_id))
                      {
                        if (!rr_timer_is_running(PATH_LOSS_TIMER, gas_id))
                        {
                          (void)rr_timer_start(PATH_LOSS_TIMER,GPRS_PATH_LOSS_TIMEOUT, gas_id);
                          MSG_GERAN_LOW_0_G("Start the PATH LOSS TIMER");
                        }
                      }
                      else
                      {
                        rr_resel_data_ptr->measurements_received |= RR_MASK_GSM_SERV_MEAS;

                        if ( ARE_ALL_BITS_SET(rr_resel_data_ptr->measurements_received, rr_resel_data_ptr->measurements_expected ))
                        {
                          new_event = EV_PATH_LOSS_TIMEOUT;
                        }
                      }
                    }
                    else
                    {
                      /*
                      force and immediate reselection as the cell we are camped onto
                      is no longer the intended scell. So all system info is not relevant
                      */
                      MSG_GERAN_HIGH_0_G("Scell BSIC changed - force reselection");

                      rr_resel_data_ptr->resel_type = RESEL_C1_CHOOSE_CELL;
                      new_event = EV_CELL_RESELECTION;
                    }
                  }
                  break;

                  case MPH_NC_MEASUREMENT_IND:
                  {
                    if (rr_meas_reporting_is_active(gas_id))
                    {
                      rr_resel_data_ptr->measurements_expected = rr_cell_resel_set_meas_expected(gas_id);

                      /* stop and restart the measurement timer */
                      (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);

                      /*If ncell measurements exist then store them*/
                      rr_gprs_process_nc_meas_ind(&message->mph_nc_measurement_ind, gas_id);

                      if ( ( rr_gprs_camped_on_ccch(gas_id) || rr_gsm_camped_on_ccch(gas_id) )
                          && ( rr_gprs_parameters_available(gas_id) == FALSE))
                      {
                        rr_resel_data_ptr->measurements_received |= RR_MASK_GSM_SURROUND_MEAS;

                        if ( ARE_ALL_BITS_SET(rr_resel_data_ptr->measurements_received, rr_resel_data_ptr->measurements_expected ))
                        {
                          new_event = EV_PATH_LOSS_TIMEOUT;
                        }
                      }
                    }
                  }
                  break;

                  case MPH_SERVING_AUX_MEAS_IND:
                  {
                    rr_gprs_process_serving_aux_meas_ind(&message->mph_serving_aux_meas_ind, gas_id);
                    break;
                  }

#ifdef FEATURE_WCDMA
                  case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
                  {
                    rr_resel_data_ptr->measurements_expected = rr_cell_resel_set_meas_expected(gas_id);

                    /******************************************************************/
                    /* Receive the measurements of the 6 best surrounding wcdma cells */
                    /******************************************************************/
                    rr_g2wr_store_wcdma_surrounding_measurement(
                      &message->mph_surround_wcdma_idle_meas_ind.best_six_wcdma_info,
                      gas_id
                    );

                    /* stop and restart the measurement timer */
                    (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);

                    /* if all the expected measurements have been received */
                    if ( ( rr_gprs_camped_on_ccch(gas_id) || rr_gsm_camped_on_ccch(gas_id) )
                        && (rr_gprs_parameters_available(gas_id) == FALSE))
                    {
                      /* received the wcdma surrounding measurements */
                      rr_resel_data_ptr->measurements_received |= RR_MASK_WCDMA_SURROUND_MEAS;

                      if ( ARE_ALL_BITS_SET(rr_resel_data_ptr->measurements_received, rr_resel_data_ptr->measurements_expected ))
                      {
                        /* generate the appropriate event */
                        new_event = EV_PATH_LOSS_TIMEOUT;
                      }
                    }
                  }
                  break;

                  case MPH_SURROUND_WCDMA_XFER_MEAS_IND:
                  {
                    rr_resel_data_ptr->measurements_expected = rr_cell_resel_set_meas_expected(gas_id);

                    rr_gprs_store_wcdma_xfer_measurements(
                      &message->mph_surround_wcdma_xfer_meas_ind,
                      gas_id
                    );

                    /* stop and restart the measurement timer */
                    (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);

                    if (rr_gprs_camped_on_ccch(gas_id) && (rr_gprs_parameters_available(gas_id) == FALSE))
                    {
                      /* received the wcdma surrounding measurements */
                      rr_resel_data_ptr->measurements_received |= RR_MASK_WCDMA_SURROUND_MEAS;

                      if ( ARE_ALL_BITS_SET(rr_resel_data_ptr->measurements_received, rr_resel_data_ptr->measurements_expected ))
                      {
                        /* generate the appropriate event */
                        new_event = EV_PATH_LOSS_TIMEOUT;
                      }
                    }
                  }
                  break;
#endif /* FEATURE_WCDMA */

#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
                  case MPH_SURROUND_IRAT_MEAS_IND:
                  {
                    rr_resel_data_ptr->measurements_expected = rr_cell_resel_set_meas_expected(gas_id);

                    rr_irat_process_surround_irat_meas_ind(
                      &message->mph.surround_irat_meas_ind,
                      gas_id
                    );

                    if ( ( rr_gprs_camped_on_ccch(gas_id) || rr_gsm_camped_on_ccch(gas_id) )
                        && (rr_gprs_parameters_available(gas_id) == FALSE))
                    {
                      /* received the wcdma surrounding measurements */
                      rr_resel_data_ptr->measurements_received |= RR_MASK_WCDMA_SURROUND_MEAS;

                      if ( ARE_ALL_BITS_SET(rr_resel_data_ptr->measurements_received, rr_resel_data_ptr->measurements_expected ))
                      {
                        /* generate the appropriate event */
                        new_event = EV_PATH_LOSS_TIMEOUT;
                      }
                    }
                    break;
                  } /* MPH_SURROUND_IRAT_MEAS_IND */
#endif /* FEATURE_LTE || FEATURE_GSM_TDS */

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  break;
                }
              }
              break;

              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);
                        if (message->rr.resel.imsg.abort_req.reason == RR_RESEL_ABORT_REASON_MM_REQ)
                        {
                          (void)rr_timer_cancel(T3174, gas_id);
                        }
                        rr_resel_send_omsg_abort_cnf(gas_id);
                      }
                      break;

                      case RR_RESEL_IMSG_REQUIRED_IND:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_REQUIRED_IND cause:0x%x rxd",message->rr.resel.imsg.required_ind.cause);

                        /* If GMM has instructed L1 to enter MM non-DRX mode, it would normally  */
                        /* instruct L1 to exit this mode upon completion of the GMM procedure    */
                        /* (ATT or RAU). In case this never happens, because of this reselection */
                        /* it is probably best to "clean-up" and reset to MM non-DRX state.      */
                        rr_gprs_clear_l1_in_mm_non_drx(gas_id);

                        /* Stop the sys info refresh timer and reset sys info handling */
                        rr_gprs_deactivate_timed_sys_info_refresh(gas_id);

                        /* Reset connection establishment. This covers both CS and PS est. */
                        /* These calls are to ensure timers are stopped (e.g. T3142, as    */
                        /* these are applicable only on the old serving cell), and covers  */
                        /* the scenario where the serving cell is barred (forcing a        */
                        /* reselection) during estalishment                                */
                        (void) rr_connection_establishment_control(EV_RESET_SOFTWARE, NULL, gas_id);
                        (void) rr_connection_establishment_control(EV_GPRS_RESET_RR_CE_STATE, NULL, gas_id);
                        (void) grr_conn_est_control(EV_RESET_SOFTWARE, NULL, gas_id);

                        /* as we're now performing a reselection, clear the SI2quater store */
                        rr_initialize_SI2q_instances(gas_id);
                        rr_initialize_SI2ter_instances(gas_id);

                        /* Stop the path loss timer if it is running */
                        if (rr_timer_cancel(PATH_LOSS_TIMER, gas_id))
                        {
                          MSG_GERAN_LOW_0_G("Stopped the PATH LOSS TIMER");
                        }

                        /* cache the current LAI so that changes can be detected after reselection */
                        rr_gprs_cache_current_LAI(gas_id);
                        rr_resel_data_ptr->cause  =  message->rr.resel.imsg.required_ind.cause;

                        switch (message->rr.resel.imsg.required_ind.cause)
                        {
                          case RR_RESEL_CAUSE_RPS:
                          {
                            /* suspend MAC here and move to RPS state and receive GMAC IDLE in RPS state and start
                               RPS*/
                            rr_resel_suspend_gprs(gas_id);

                            rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                            rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                            break;
                          }

                          case RR_RESEL_CAUSE_RESELECTION:
                          case RR_RESEL_CAUSE_RESELECTION_TRM_DENIAL:
                          {
                            if (rr_timer_is_running(T3174, gas_id))
                            {
                              /**
                              * If a PCCO is in progress (T3174 running) and cell reselection is
                              * forced, this is an indication that the PCCO has failed.
                              */
                              new_event = EV_PCCO_FAILED;
                            }
                            else
                            {
#ifdef FEATURE_LTE
                              if (rr_irat_check_for_priority_based_reselection(rr_resel_to_lower_priority_layers_permitted(gas_id),TRUE, gas_id))
                              {
                                 MSG_GERAN_HIGH_0_G("Priorty based reselction performed after DSF/Forced reselction");
                                 rr_resel_data_ptr->resel_type = RESEL_G2W_NORMAL;
                              }
                              else 
#endif /* FEATURE_LTE */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
                              if (rr_irat_check_for_rank_based_reselection(FALSE, TRUE, TRUE, FALSE, FALSE, gas_id))
                              {
                                MSG_GERAN_HIGH_0_G("Rank based reselction performed after DSF/Forced reselction");
                                rr_resel_data_ptr->resel_type = RESEL_G2W_NORMAL;
                              }
                              else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
                              {
                                /**
                                 * Otherwise, treat this exactly like a cell reselection trigger.
                                 */
                                rr_resel_data_ptr->resel_type = RESEL_C1_CHOOSE_CELL;
                              }

                              new_event = EV_CELL_RESELECTION;
                            }
                            break;
                          }

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad resel_cause: 0x%x",message->rr.resel.imsg.required_ind.cause);
                          }
                        }
                        break;
                      } /* RR_RESEL_IMSG_REQUIRED_IND */

#ifdef FEATURE_LTE

                      case RR_RESEL_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND:
                      case RR_RESEL_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND:
                      {
                        rr_resel_algo_e resel_algo = rr_resel_algo(gas_id);

                        switch (resel_algo)
                        {
                          case RR_RESEL_ALGO_RANK_BASED:
                          {
                            rr_resel_cancel_all_priority_based_resel_timers(gas_id);
                            break;
                          } /* RR_RESEL_ALGO_RANK_BASED */

                          case RR_RESEL_ALGO_PRIORITY_BASED:
                          {
                            rr_resel_cancel_all_rank_based_resel_timers(gas_id);
                            break;
                          } /* RR_RESEL_ALGO_THRESHOLD_BASED */

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("Unexpected resel_algo: %d", resel_algo);
                          }
                        }
                        break;
                      } /* RR_RESEL_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND ... */

                      case RR_RESEL_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND:
                      {
                        /* If G2L reselection is now disallowed then any
                        running G2L timers must be stopped, and LTE neighbour
                        cell measurements cleared. */
                        if (!rr_g2l_resel_allowed(gas_id))
                        {
                          rr_irat_lte_measurements_delete(gas_id);
                        }
                        break;
                      } /* RR_RESEL_ISMG_G2L_RESEL_ALLOWED_UPDATED_IND */

#endif /* FEATURE_LTE */

                      case RR_RESEL_IMSG_START_RPS_IND:
                      {
                        uint16 i;

                        MSG_GERAN_HIGH_0_G("RR_RESEL_IMSG_START_RPS_IND rxd");

                        rr_rps_cell_list_reset(&rr_resel_data_ptr->rps_data.pscan_cell_list);

                        rr_rps_cell_list_add(
                         &rr_resel_data_ptr->rps_data.pscan_cell_list,
                         message->rr.resel.imsg.start_rps_ind.serving_cell,
                         gas_id
                        );

                        for (i = 0;
                             i < message->rr.resel.imsg.start_rps_ind.ba_list.no_of_entries &&
                             i < BA_MAX_LEN;
                             i++)
                        {
                          rr_rps_cell_list_add(
                            &rr_resel_data_ptr->rps_data.pscan_cell_list,
                            message->rr.resel.imsg.start_rps_ind.ba_list.BCCH_ARFCN[i],
                            gas_id
                          );
                        }

                        rr_perform_rps(&rr_resel_data_ptr->rps_data.pscan_cell_list, gas_id);

                        rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                        rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;

                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      }
                    }
                    break;
                  } /* RR_RESEL_IMSG */


                  case RR_CELL_ACQ_IMSG:
                  case RR_CELL_ACQ_OMSG:
                  {
                    /* No action necessary. */
                    break;
                  }

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  break;
                }
                break;
              }

              case MS_MAC_RR:
              {
                switch ((mac_grr_sig_id_t) message_id)
                {
                  case MAC_GRR_DATA_IND:
                  {
                    /* Load the message into a local store */
                    mac_grr_data_ind_t *mac_grr_data_ind = &message->mac_grr_sig.mac_grr_data_ind;

                    switch (mac_grr_data_ind->data_ptr.header_ptr->message_type)
                    {
                      case PACKET_CELL_CHANGE_ORDER:
                      {
                        void *data_ptr = mac_grr_data_ind->data_ptr.data_ptr;
                        packet_cell_change_order_t *pcco = (packet_cell_change_order_t *) data_ptr;

                        MSG_GERAN_HIGH_0_G("PACKET_CELL_CHANGE_ORDER rxd");

                        /**
                         * Reject the PCCO if the GMM state is not compatible with the NC mode
                         */
                        if (rr_gmminfo_get_gmm_status(gas_id) == GMM_READY_STATE)
                        {
                          new_event = rr_gprs_process_PCCO(pcco, gas_id);

                          if (new_event == EV_ACTION_PCCO ||
                              new_event == EV_ACTION_IMMEDIATE_PCCO ||
                              new_event == EV_ACTION_PCCO_WITH_PNCD)
                          {
                            rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO;
                          }
                        }
                        else
                        {
                          /* send the PCCF to MAC */
                          MSG_GERAN_ERROR_0_G("GMM not in READY state, reject PCCO");
                          rr_gprs_load_pccf_from_pcco(pcco, gas_id);

                          /* determine PCCF cause value - see 3GPP 44.060 section 8.4.2 */
                          if (rr_gmminfo_get_neg_ready_timer_zero(gas_id))
                          {
                            rr_gprs_send_pccf(FORCED_TO_STANDBY, gas_id);
                          }
                          else
                          {
                            rr_gprs_send_pccf(MS_IN_GMM_STANDBY, gas_id);
                          }
                        }
                      }
                      break;

#ifdef FEATURE_GPRS_PS_HANDOVER
                      case PACKET_PS_HANDOVER:
                      {
                        void *data_ptr = mac_grr_data_ind->data_ptr.data_ptr;
                        ps_handover_command_t *psho = (ps_handover_command_t *) data_ptr;

                        if (rr_gprs_reselection_process_psho_command(psho, gas_id) == EV_ACTION_PSHO)
                        {
                          output_event = EV_PSHO_STARTED;

                          if (rr_resel_data_ptr->nacc_pccc_received || rr_resel_data_ptr->nacc_pcco_received)
                          {
                            MSG_GERAN_ERROR_2_G("nacc_pccc_received: %d, nacc_pcco_received: %d",
                                      rr_resel_data_ptr->nacc_pccc_received,
                                      rr_resel_data_ptr->nacc_pcco_received);
                            MSG_GERAN_HIGH_0_G("Clearing PCCC/PCCO flags");
                            rr_resel_data_ptr->nacc_pccc_received = FALSE;
                            rr_resel_data_ptr->nacc_pcco_received = FALSE;
                          }

                          rr_resel_data_ptr->state = RR_RESEL_STATE_NACC_PENDING_NW_RESPONSE;
                        }
                      }
                      break;
#endif /* FEATURE_GPRS_PS_HANODVER */

                      default:
                      {
                        MSG_GERAN_ERROR_1_G("Unknown message type: %d",
                          mac_grr_data_ind->data_ptr.header_ptr->message_type);
                      }
                    }
                  }
                  break;

                  /**
                   * MAC_GRR_PCCF_SENT
                   *
                   * MAC sends this signal to indicate whether or not it
                   * successfully sent a PCCF message.  If successful the PCCO
                   * procedure is considered complete and T3176 should be stopped.
                   * If it failed, RR should try and send the PCCF again (so long
                   * as T3176 is still running - it is possible that if GMM left
                   * READY state T3176 will have been cancelled as the PCCF does
                   * not need to be sent any more - if this is the case, RR does
                   * not need to take any action here).
                   */
                  case MAC_GRR_PCCF_SENT:
                  {
                    mac_grr_pccf_sent_t *pccf_sent = &message->mac_grr_sig.mac_grr_pccf_sent;

                    MSG_GERAN_HIGH_0_G("MAC_GRR_PCCF_SENT rxd");

                    rr_resel_data_ptr->pccf_transmission_in_progress = FALSE;

                    if (rr_timer_is_running(T3176, gas_id))
                    {
                      if (pccf_sent->pccf_status == SUCCESSFULLY_SENT_PCCF)
                      {
                        MSG_GERAN_HIGH_0_G("PCCF successfully sent, cancelling T3176");
                        (void)rr_timer_cancel(T3176, gas_id);
                      }
                      else
                      {
                        MSG_GERAN_HIGH_0_G("PCCF not successfully sent, retrying");
                        rr_gprs_send_pccf(USE_PRELOADED_CAUSE, gas_id);
                      }
                    }
#ifdef FEATURE_GPRS_PS_HANDOVER
                    else
                    {
                      MSG_GERAN_HIGH_1_G("MAC_GRR_PCCF_SENT(pccf_status=%d) received",
                        pccf_sent->pccf_status);
                    }
#else
                    else
                    {
                      MSG_GERAN_HIGH_1_G("MAC_GRR_PCCF_SENT(pccf_status=%d) received but T3176 not running",
                        pccf_sent->pccf_status);
                    }
#endif /* FEATURE_GPRS_PS_HANDOVER */
                  }
                  break;

                  case MAC_GRR_CELL_RESEL_STATUS:
                  {
                    MSG_GERAN_HIGH_0_G("MAC_GRR_CELL_RESEL_STATUS rxd");

                    switch (message->mac_grr_sig.mac_grr_cell_resel_status.status)
                    {
                      case PCCO_SUCCESS:
                      {
                        MSG_GERAN_HIGH_0_G("PCCO procedure completed successfully!");
                        break;
                      }

                      case PCCO_FAILURE_NO_RESPONSE:
                      {
                        MSG_GERAN_HIGH_0_G("PCCO procedure failed, no response on target cell");
                        rr_resel_data_ptr->pcco_data.pccf.cause = (uint8) NO_RESPONSE_ON_TARGET;
                        break;
                      }

                      case PCCO_FAILURE_ACCESS_REJ:
                      {
                        MSG_GERAN_HIGH_0_G("PCCO procedure failed, access reject on target cell");
                        rr_resel_data_ptr->pcco_data.pccf.cause = (uint8) ASSIGNMENT_REJECT_ON_TARGET;
                        break;
                      }

                      default:
                      {
                        MSG_GERAN_ERROR_1_G("Unknown status %d in MAC_GRR_CELL_RESEL_STATUS, setting PCCF cause to 'no response'",
                          message->mac_grr_sig.mac_grr_cell_resel_status.status);
                        rr_resel_data_ptr->pcco_data.pccf.cause = (uint8) NO_RESPONSE_ON_TARGET;
                      }
                    }
                    (void)rr_timer_cancel(T3174, gas_id);

                    if (message->mac_grr_sig.mac_grr_cell_resel_status.status != PCCO_SUCCESS)
                    {
                      rr_gprs_deactivate_timed_sys_info_refresh(gas_id);

                      /* If GMM is still in READY state, follow PCCO procedures and return
                      to the old cell to send a rr_resel_data_ptr->pcco_data.pccf.  Otherwise, GMM has left READY state
                      while the PCCO was failing (probably while waiting for L1 to resume
                      idle mode after a connection establishment failure).  As PCCO
                      procedures no longer need to be followed, just trigger a reselection
                      power scan to regain service. */
                      if (rr_gmminfo_get_gmm_status(gas_id) == GMM_READY_STATE)
                      {
                        (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);
                        rr_reselect_old_scell_after_pcco_failure(rr_resel_data_ptr, gas_id);
                        rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;

                        rr_resel_send_cell_acq_start_ind(
                          RR_CELL_ACQ_ACTION_RESELECT_BCCH_PCCO_FAILED,
                          RR_RESEL_DONT_CLEAR_SI_DB,
                          gas_id
                        );

                        rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_FAILED;
                      }
                      else
                      {
                        /* suspend MAC here and move to RPS state and receive GMAC IDLE in RPS state and start
                           RPS*/
                        rr_resel_suspend_gprs(gas_id);

                        rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                        rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                      }
                    }
                  }
                  break;

                  case MAC_GRR_MAC_STATUS:
                  {
                    /* ignore this signal*/
                  }
                  break;

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  break;
                }
              }
              break;

              default:
                LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
              break;
            }
          }
          break;

          /****************************************************************/
          /* Event Handlers                                               */
          /*                                                              */
          /****************************************************************/


          /****************************************************************/
          /*                     Reselection Decision                     */
          /*                     --------------------                     */
          /*                                                              */
          /* ...Check for Better cells or Poor Scell                      */
          /****************************************************************/
          case EV_PATH_LOSS_TIMEOUT:
          {
            (void) rr_log_gprs_cell_reselection_measurements(
              gprs_reselection_list_ptr,
              get_recent_reselection_flag_value(gas_id),
              gas_id
            );

            MSG_GERAN_MED_1_G("ncells %d", gprs_reselection_list_ptr->no_of_entries);

            /* If W2G PCCO is ongoing, avoid cell reselection because the cell reselection could
             * be twisted with PCCO, which could cause trouble when handling PCCO failure
             * in cell reselection states
             */
            if (rr_resel_data_ptr->rr_gprs_reselection_control == MS_CONTROL
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                && !rr_x2g_cco_in_progress(gas_id)
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                )
            {
              rr_resel_algo_e resel_algo = rr_resel_algo(gas_id);

              switch (resel_algo)
              {
                case RR_RESEL_ALGO_RANK_BASED:
                {
                  /*****************************************************/
                  /*              MS CONTROLLED RESELECTION            */
                  /*                                                   */
                  /* Perform checks for better cells and path loss.    */
                  /*****************************************************/
                  if (rr_gprs_check_for_path_loss(gas_id))
                  {
                    rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_C1, gas_id);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
                    if (rr_irat_check_for_rank_based_reselection(
                          get_recent_reselection_flag_value(gas_id),
                          TRUE,
                          FALSE,
                          FALSE,
                          FALSE,
                          gas_id))
                    {
                      rr_resel_data_ptr->resel_type = RESEL_G2W_NORMAL;
                    }
                    else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
                    {
                      rr_resel_data_ptr->resel_type = RESEL_C1_CHOOSE_CELL;
                    }
                    new_event = EV_CELL_RESELECTION;
                  }
                  else if (rr_gprs_check_for_better_ncell(
                             rr_resel_data_ptr->recent_reselection_penalty_flag,
                             gas_id
                           ))
                  {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
                    /*Optimisation: 
                      do a C2 based reselection (power level meets criteria but timer has not expired) to IRAT as priority over 
                      better GSM cell*/
                    MSG_GERAN_HIGH_0_G("GERAN C2 resel candidate found, optimisation: check for suitable IRAT cell");

                    if (rr_irat_check_for_rank_based_reselection(
                          get_recent_reselection_flag_value(gas_id),
                          FALSE,
                          FALSE,
                          rr_inhibit_utran_reselection(gas_id),
                          TRUE,  
                          gas_id))
                    {
                      rr_resel_data_ptr->resel_type = RESEL_G2W_NORMAL;
                    }
                    else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
                    {
                      rr_resel_data_ptr->resel_type = RESEL_C2_CHOOSE_CELL;
                    }
                    new_event = EV_CELL_RESELECTION;
                  }
                  else if ( rr_gprs_check_for_bad_scell(gas_id) )
                  {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
                      /*Optimisation: 
                      do a forced reselection (power level meets criteria (TDS uses default offset) but timer has not expired) to IRAT
                      as priority over SNR based GSM reselection*/
                    MSG_GERAN_HIGH_0_G("GERAN SNR based resel candidate found, optimisation: check for suitable IRAT cell");

                    if (rr_irat_check_for_rank_based_reselection(
                          get_recent_reselection_flag_value(gas_id),
                          TRUE,
                          FALSE,
                          FALSE,
                          FALSE,
                          gas_id))
                    {
                      rr_resel_data_ptr->resel_type = RESEL_G2W_NORMAL;
                    }
                    else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
                    {
                      rr_resel_data_ptr->resel_type = RESEL_C1_CHOOSE_CELL;
                    }
                    new_event = EV_CELL_RESELECTION;
                  }
                  else
                  {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
                    if (rr_irat_check_for_rank_based_reselection(
                          get_recent_reselection_flag_value(gas_id),
                          FALSE,
                          FALSE,
                          rr_inhibit_utran_reselection(gas_id),
                          FALSE,  
                          gas_id
                        ))
                    {
                      rr_resel_data_ptr->resel_type = RESEL_G2W_NORMAL;
                      new_event = EV_CELL_RESELECTION;
                    }
                    else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
                    {
                      /*No reselection required*/
                      rr_resel_data_ptr->resel_type = RESEL_NO_RESEL;
                      new_event = EV_NO_EVENT;
                    }
                  }
                  break;
                } /* RR_RESEL_ALGO_RANK_BASED */

#ifdef FEATURE_LTE
                case RR_RESEL_ALGO_PRIORITY_BASED:
                {
                  if (rr_irat_check_for_priority_based_reselection(
                        rr_resel_to_lower_priority_layers_permitted(gas_id),
                        FALSE,           
                        gas_id
                      ))
                  {
                    rr_resel_data_ptr->resel_type = RESEL_G2W_NORMAL;
                    new_event = EV_CELL_RESELECTION;
                  }
                  else if (rr_gprs_check_for_path_loss(gas_id))
                  {
                    rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_C1, gas_id);
                    /*Optimisation: 
                      do a forced priority reselection (servicg cell C1<0) to IRAT */
                    MSG_GERAN_HIGH_0_G("Serving cell C1<0(too low) , optimisation: check for suitable IRAT cell");

                    if (rr_irat_check_for_priority_based_reselection(rr_resel_to_lower_priority_layers_permitted(gas_id),TRUE, gas_id))
                    {
                      rr_resel_data_ptr->resel_type = RESEL_G2W_NORMAL;
                      new_event = EV_CELL_RESELECTION;
                    }
                    else
                    {
                      rr_resel_data_ptr->resel_type = RESEL_C1_CHOOSE_CELL;
                      new_event = EV_CELL_RESELECTION;
                    }
                  }
                  else if (rr_gprs_check_for_better_ncell(
                             rr_resel_data_ptr->recent_reselection_penalty_flag,
                             gas_id
                           ))
                  {
                    if (rr_irat_check_for_priority_based_reselection(
                          rr_resel_to_lower_priority_layers_permitted(gas_id),
                          TRUE,           
                          gas_id
                        ))
                    {
                      rr_resel_data_ptr->resel_type = RESEL_G2W_NORMAL;
                      new_event = EV_CELL_RESELECTION;
                    }
                    else
                    {
                      rr_resel_data_ptr->resel_type = RESEL_C2_CHOOSE_CELL;
                      new_event = EV_CELL_RESELECTION;
                    }
                  }
                  else if (rr_gprs_check_for_bad_scell(gas_id))
                  {

                    /*Optimisation: 
                      do a forced reselection (power level meets criteria but timer has not expired) to IRAT as priority over
                      SNR based GSM reselection*/
                    MSG_GERAN_HIGH_0_G("GERAN SNR based resel candidate found, optimisation: check for suitable IRAT cell");

                    if (rr_irat_check_for_priority_based_reselection(rr_resel_to_lower_priority_layers_permitted(gas_id),TRUE, gas_id))
                    {
                      rr_resel_data_ptr->resel_type = RESEL_G2W_NORMAL;
                      new_event = EV_CELL_RESELECTION;
                    }
                    else
                    {
                      rr_resel_data_ptr->resel_type = RESEL_C1_CHOOSE_CELL;
                      new_event = EV_CELL_RESELECTION;
                    }
                  }
                  else
                  {
                    rr_resel_data_ptr->resel_type = RESEL_NO_RESEL;
                    new_event = EV_NO_EVENT;
                  }
                  break;
                } /* RR_RESEL_ALGO_PRIORITY_BASED */
#endif /* FEATURE_LTE */

                default:
                {
                  MSG_GERAN_ERROR_1_G("Unexpected reselection algorithm: %d", resel_algo);
                }
              }
            }
            else
            {
              /*****************************************************/
              /*          NETWORK CONTROLLED RESELECTION           */
              /*                                                   */
              /* No reselection checks or action is required using */
              /* the collected measurements, this event will clean */
              /* out the reselection list to start again           */
              /*****************************************************/
              rr_resel_data_ptr->resel_type = RESEL_NO_RESEL;
              new_event = EV_NO_EVENT;
            }
            rr_resel_data_ptr->measurements_received = RR_NO_MEASUREMENTS_RECEIVED;
          }
          break;

          case EV_CELL_RESELECTION:
          {
            MSG_GERAN_HIGH_0_G("EV_CELL_RESELECTION rxd");

            switch (rr_resel_data_ptr->resel_type)
            {
              case RESEL_NO_RESEL:
                /* Do Nothing, Reselection not necessary */
              break;

              case RESEL_C1_CHOOSE_CELL:
              {
                rr_resel_start_c1_status_e c1_resel_status = START_C1_RESEL_FAILED;

                c1_resel_status = rr_resel_start_c1(TRUE, TRUE, TRUE, gas_id);

                switch (c1_resel_status)
                {
                  case START_C1_RESEL_FAILED:

                    rr_gprs_clear_l1_in_mm_non_drx(gas_id);
                    /* Stop the sys info refresh timer and reset sys info handling */
                    rr_gprs_deactivate_timed_sys_info_refresh(gas_id);
                    
                    /* suspend MAC here and move to RPS state and receive GMAC IDLE in RPS state and start
                       RPS*/
                    rr_resel_suspend_gprs(gas_id);

                    /* Inform GRR that reselection has started */
                    rr_resel_send_omsg_started_ind(RR_RESEL_TYPE_NORMAL, gas_id);

                    rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                    rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                  break;

                  case START_C1_RESEL_CONTINUE:
                    rr_resel_prepare_for_resel_from_path_loss(TRUE,gas_id);
                    rr_resel_send_cell_acq_start_ind(
                      RR_CELL_ACQ_ACTION_RESELECT_BCCH,
                      RR_RESEL_CLEAR_SI_DB,
                      gas_id
                    );

                    /* Inform GRR that reselection has started */
                    rr_resel_send_omsg_started_ind(RR_RESEL_TYPE_NORMAL, gas_id);

                    rr_resel_data_ptr->state = RR_RESEL_STATE_C1;
                  break;

                  default:
                    MSG_GERAN_ERROR_1_G("RR Resel Invalid start_c1: 0x%x",c1_resel_status);
                  break;
                }
              }
              break;

              case RESEL_C1_TARGET_CELL_ALREADY_SELECTED:
              {
                rr_gprs_clear_l1_in_mm_non_drx(gas_id);
                rr_resel_send_cell_acq_start_ind(
                  RR_CELL_ACQ_ACTION_RESELECT_BCCH,
                  RR_RESEL_CLEAR_SI_DB,
                  gas_id
                );

                rr_resel_data_ptr->state = RR_RESEL_STATE_C1;
              }
              break;

              case RESEL_C2_CHOOSE_CELL:
              {
                rr_resel_start_c2_status_e c2_resel_status = RESEL_NO_CELLS_VALID_FOR_RESEL;

                c2_resel_status = rr_resel_start_c2(START_C2_TARGET_BEST_CELL, gas_id);

                switch (c2_resel_status)
                {
                  case NACC_RESEL_FROM_BCCH_CELL:
                  {
                    nacc_surr_check_result_e surround_SI3_SI4_result;

                    rr_resel_prepare_for_resel_from_path_loss(TRUE,gas_id);

                    surround_SI3_SI4_result = rr_resel_nacc_check_surround_SI3_SI4(
                      rr_resel_data_ptr->recent_reselection_penalty_flag,
                      gas_id
                    );

                    switch( surround_SI3_SI4_result )
                    {
                      case NACC_CELL_NOT_SUITABLE_FOR_RESEL:
                         rr_resel_data_ptr->resel_type = RESEL_C2_SELECT_NEXT_BEST_CELL;
                         new_event = EV_CELL_RESELECTION;
                      break;

                      case NACC_CELL_SUITABLE_FOR_RESEL:
                        grr_start_nacc_reselection_process(gas_id);

                        if (rr_resel_data_ptr->nacc_pccc_received || rr_resel_data_ptr->nacc_pcco_received)
                        {
                          MSG_GERAN_ERROR_2_G("nacc_pccc_received: %d, nacc_pcco_received: %d",
                                    rr_resel_data_ptr->nacc_pccc_received,
                                    rr_resel_data_ptr->nacc_pcco_received);
                          MSG_GERAN_HIGH_0_G("Clearing PCCC/PCCO flags");
                          rr_resel_data_ptr->nacc_pccc_received = FALSE;
                          rr_resel_data_ptr->nacc_pcco_received = FALSE;
                        }

                        rr_resel_data_ptr->state = RR_RESEL_STATE_NACC_PENDING_NW_RESPONSE;
                      break;

                      default:
                        MSG_GERAN_ERROR_1_G("RR RESEL Handle SI3,4 unexpected return event:0x%x",surround_SI3_SI4_result);
                      break;
                    }
                  }
                  break;

                  case RESEL_FROM_BCCH_CELL:
                    MSG_GERAN_HIGH_0_G("Prepare for path loss without clearing SI2q instance");				  	
                    rr_resel_prepare_for_resel_from_path_loss(FALSE,gas_id);

                    rr_resel_send_cell_acq_start_ind(
                      RR_CELL_ACQ_ACTION_RESELECT_BCCH_AFTER_SU,
                      RR_RESEL_CLEAR_SI_DB,
                      gas_id
                    );

                    rr_resel_data_ptr->state = RR_RESEL_STATE_C2_PENDING_MAC_SUSPENSION;
                  break;

                  case RESEL_FROM_BCCH_WITH_GPRS_CELL_SELECTION_PARAMS:
                   MSG_GERAN_HIGH_0_G("Prepare for path loss without clearing SI2q instance");				  	
                    rr_resel_prepare_for_resel_from_path_loss(FALSE,gas_id);
                    rr_resel_send_cell_acq_start_ind(
                      RR_CELL_ACQ_ACTION_RESELECT_BCCH_WITH_GPRS_CELL_SELECTION_PARAMS,
                      RR_RESEL_CLEAR_SI_DB,
                      gas_id
                    );
                    rr_resel_data_ptr->state = RR_RESEL_STATE_C2_PENDING_MAC_SUSPENSION;
                  break;


                  case RESEL_NO_CELLS_VALID_FOR_RESEL:
                    /*This is an exception that should not happen*/
                    MSG_GERAN_ERROR_0_G("RR Internal Error No Candidates for C2 Resel found after Path loss");
                  break;

                  default:
                    MSG_GERAN_ERROR_1_G("RR Resel Invalid c2_resel_status: 0x%x",c2_resel_status);
                  break;
                }
              }
              break;

              case RESEL_C2_SELECT_NEXT_BEST_CELL:
              {
                /* Find the next suitable neighbour cell and do C2 resel */
                rr_resel_start_c2_status_e c2_resel_status = RESEL_NO_CELLS_VALID_FOR_RESEL;

                c2_resel_status = rr_resel_start_c2(
                  START_C2_SELECT_NEXT_BEST_CELL,
                  gas_id
                );

                switch (c2_resel_status)
                {
                  case NACC_RESEL_FROM_BCCH_CELL:
                  {
                    nacc_surr_check_result_e surround_SI3_SI4_result;

                    surround_SI3_SI4_result = rr_resel_nacc_check_surround_SI3_SI4(
                      rr_resel_data_ptr->recent_reselection_penalty_flag,
                      gas_id
                    );

                    switch( surround_SI3_SI4_result )
                    {
                      case NACC_CELL_NOT_SUITABLE_FOR_RESEL:
                        rr_resel_data_ptr->resel_type = RESEL_C2_SELECT_NEXT_BEST_CELL;
                        new_event = EV_CELL_RESELECTION;
                      break;

                      case NACC_CELL_SUITABLE_FOR_RESEL:
                        grr_start_nacc_reselection_process(gas_id);

                        if (rr_resel_data_ptr->nacc_pccc_received || rr_resel_data_ptr->nacc_pcco_received)
                        {
                          MSG_GERAN_ERROR_2_G("nacc_pccc_received: %d, nacc_pcco_received: %d",
                                    rr_resel_data_ptr->nacc_pccc_received,
                                    rr_resel_data_ptr->nacc_pcco_received);
                          MSG_GERAN_HIGH_0_G("Clearing PCCC/PCCO flags");
                          rr_resel_data_ptr->nacc_pccc_received = FALSE;
                          rr_resel_data_ptr->nacc_pcco_received = FALSE;
                        }

                        rr_resel_data_ptr->state = RR_RESEL_STATE_NACC_PENDING_NW_RESPONSE;
                      break;

                      default:
                        MSG_GERAN_ERROR_1_G("RR RESEL Handle SI3,4 unexpected return event:0x%x",surround_SI3_SI4_result);
                      break;
                    }

                    rr_resel_data_ptr->rr_resel_c2_pend_abort_req_rxd = FALSE;
                  }
                  break;


                  case RESEL_FROM_BCCH_CELL:
                    rr_resel_send_cell_acq_start_ind(
                      RR_CELL_ACQ_ACTION_RESELECT_BCCH_AFTER_SU,
                      RR_RESEL_CLEAR_SI_DB,
                      gas_id
                    );

                    rr_resel_data_ptr->state = RR_RESEL_STATE_C2_PENDING_MAC_SUSPENSION;
                  break;

                  case RESEL_NO_CELLS_VALID_FOR_RESEL:
                    /*No suitable neighbours found, no action*/
                    if (rr_resel_data_ptr->rr_resel_c2_pend_abort_req_rxd)
                    {
                      rr_resel_data_ptr->rr_resel_c2_pend_abort_req_rxd = FALSE;
                      rr_resel_send_omsg_abort_cnf(gas_id);
                    }
                    else
                    {
                      rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_IN_SERVICE, gas_id);
                    }
                  break;

                  default:
                    MSG_GERAN_ERROR_1_G("RR Resel Invalid c2_resel_status: 0x%x",c2_resel_status);
                  break;
                }
              }
              break;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
              case RESEL_G2W_NORMAL:
              case RESEL_G2W_PCCO:
              {
                gprs_scell_info_t         * scell_info_ptr;
                rr_PLMN_request_details_T * rr_PLMN_request_details_ptr;

                scell_info_ptr = rr_gprs_get_scell_info(gas_id);
                rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

                rr_resel_data_ptr->rr_resel_c2_pend_abort_req_rxd = FALSE;
                rr_resel_data_ptr->g2x_target = rr_irat_resel_target_get(gas_id);

                if (rr_resel_data_ptr->resel_type == RESEL_G2W_NORMAL)
                {
                  rr_resel_data_ptr->g2x_type = RR_G2X_RESEL_TYPE_RESELECTION;
                }
                else
                {
                  rr_resel_data_ptr->g2x_type = RR_G2X_RESEL_TYPE_PACKET_CELL_CHANGE_ORDER;
                }

                /* Store the current serving cell's ARFCN for later re-acquisition if
                G2W reselection fails. */
                rr_resel_data_ptr->old_scell_arfcn_valid = TRUE;
                rr_resel_data_ptr->old_scell_arfcn = rr_gprs_get_scell_arfcn(gas_id);

                /* If GMM has instructed L1 to enter MM non-DRX mode, it would normally  */
                /* instruct L1 to exit this mode upon completion of the GMM procedure    */
                /* (ATT or RAU). In case this never happens, because of this reselection */
                /* it is probably best to "clean-up" and reset to MM non-DRX state.      */
                rr_gprs_clear_l1_in_mm_non_drx(gas_id);

                /* Stop the sys info refresh timer and reset sys info handling */
                rr_gprs_deactivate_timed_sys_info_refresh(gas_id);

                /* Clear the best-six entries */
                rr_remove_all_best_six_ncells(gas_id);

                /* cache the current LAI so that changes can be detected after reselection
                 * this is so that if the reselection fails and we return to the old serving cell,
                 * GRR does not think that an LAU is pending so will handle any pages.
                 */
                rr_gprs_cache_current_LAI(gas_id);

                if (rr_resel_data_ptr->resel_type == RESEL_G2W_NORMAL)
                {
                  rr_resel_g2w_send_imsg_resel_req(
                    rr_resel_data_ptr->g2x_target,
                    RR_G2X_RESEL_TYPE_RESELECTION,
                    rr_PLMN_request_details_ptr->network_select_mode,
                    scell_info_ptr->gsm.location_area_identification.plmn_id,
                    scell_info_ptr->gsm.trk_lo_adj,
                    gas_id
                  );
                }
                else
                {
                  rr_resel_g2w_send_imsg_resel_req(
                    rr_resel_data_ptr->g2x_target,
                    RR_G2X_RESEL_TYPE_PACKET_CELL_CHANGE_ORDER,
                    rr_PLMN_request_details_ptr->network_select_mode,
                    scell_info_ptr->gsm.location_area_identification.plmn_id,
                    scell_info_ptr->gsm.trk_lo_adj,
                    gas_id
                  );
                }
                rr_resel_send_omsg_started_ind(RR_RESEL_TYPE_G2W, gas_id);
                rr_resel_data_ptr->state = RR_RESEL_STATE_G2W;
              }
              break;

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

              default:
                MSG_GERAN_ERROR_1_G("RR Resel Invalid rr_resel_data_ptr->resel_type: 0x%x",rr_resel_data_ptr->resel_type);
              break;
            }

            rr_resel_data_ptr->resel_type = RESEL_NO_RESEL;

          }
          break;

#ifdef FEATURE_WCDMA
          case RR_EV_G2W_PCCO_STARTED:
          {
            rr_resel_data_ptr->resel_type = RESEL_G2W_PCCO;
            new_event = EV_CELL_RESELECTION;
            break;
          }
#endif /* FEATURE_WCDMA */

          case EV_RR_EST_IND:
          {
            if (rr_timer_cancel(T3174, gas_id))
            {
              /**
               * This is an indication that CS contention resolution has been
               * successful - a CS connection has been established and the PCCO
               * procedure is completed successfully.
               */
              MSG_GERAN_HIGH_0_G("PCCO procedure completed successfully!");
            }
          }
          break;

          case EV_T3176_TIMEOUT:
          {
            rr_resel_data_ptr->pccf_transmission_in_progress = FALSE;
            MSG_GERAN_HIGH_0_G("T3176 expired, triggering reselection power scan");

            rr_gprs_deactivate_timed_sys_info_refresh(gas_id);

            /* suspend MAC here and move to RPS state and receive GMAC IDLE in RPS state and start
               RPS*/
            rr_resel_suspend_gprs(gas_id);

            rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
            rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;

          }
          break;

          case EV_PCCO_FAILED:
          {
            MSG_GERAN_HIGH_0_G("EV_PCCO_FAILED rxd");

            (void)rr_timer_cancel(T3174, gas_id);

            rr_gprs_deactivate_timed_sys_info_refresh(gas_id);

            /* If GMM is still in READY state, follow PCCO procedures and return
            to the old cell to send a rr_resel_data_ptr->pcco_data.pccf.  Otherwise, GMM has left READY state
            while the PCCO was failing (probably while waiting for L1 to resume
            idle mode after a connection establishment failure).  As PCCO
            procedures no longer need to be followed, just trigger a reselection
            power scan to regain service. */
            if (rr_gmminfo_get_gmm_status(gas_id) == GMM_READY_STATE)
            {
              (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);
              rr_reselect_old_scell_after_pcco_failure(rr_resel_data_ptr, gas_id);
              rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;

              rr_resel_send_cell_acq_start_ind(
                RR_CELL_ACQ_ACTION_RESELECT_BCCH,
                RR_RESEL_CLEAR_SI_DB,
                gas_id
              );

              rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_FAILED;
            }
            else
            {
              /* suspend MAC here and move to RPS state and receive GMAC IDLE in RPS state and start
                 RPS*/
              rr_resel_suspend_gprs(gas_id);

              rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
              rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;

            }
          }
          break;

          case EV_T3174_TIMEOUT:
          {
            MSG_GERAN_HIGH_0_G("EV_T3174_TIMEOUT rxd");

            rr_gprs_deactivate_timed_sys_info_refresh(gas_id);
            rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_PCCO_FAILED, gas_id);
            if (rr_gmminfo_get_gmm_status(gas_id) == GMM_READY_STATE)
            {
              (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);
              MSG_GERAN_HIGH_0_G("RR_RESEL_STATE_IDLE: T3174 expired, starting T3176");
              MSG_GERAN_HIGH_0_G("Suspending MAC (if necessary) before reselecting old serving cell");
              rr_reselect_old_scell_after_pcco_failure(rr_resel_data_ptr, gas_id);

              rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;

              rr_resel_data_ptr->pcco_data.pccf.cause = (uint8) NO_RESPONSE_ON_TARGET;

              rr_resel_send_cell_acq_start_ind(
                RR_CELL_ACQ_ACTION_RESELECT_BCCH,
                RR_RESEL_CLEAR_SI_DB,
                gas_id
              );

              rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_FAILED;
            }
            else
            {
              /* suspend MAC here and move to RPS state and receive GMAC IDLE in RPS state and start
                 RPS*/
              rr_resel_suspend_gprs(gas_id);

              rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
              rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
            }
          }
          break;

          case EV_GMM_LEFT_READY_STATE:
          {
            MSG_GERAN_HIGH_0_G("EV_GMM_LEFT_READY_STATE rxd");

            if (rr_timer_cancel(T3174, gas_id))
            {
              MSG_GERAN_MED_0_G("Cancelled T3174");
            }

            if (rr_timer_cancel(T3176, gas_id))
            {
              MSG_GERAN_MED_0_G("Cancelled T3176");
            }

            rr_resel_data_ptr->pccf_transmission_in_progress = FALSE;
          }
          break;

          case EV_PS_ACCESS_IND:
          {
            MSG_GERAN_HIGH_0_G("EV_PS_ACCESS_IND rxd");

            MSG_GERAN_HIGH_0_G("GMM is resumed");
            if (rr_timer_is_running(T3176, gas_id))
            {
              if (rr_resel_data_ptr->pccf_transmission_in_progress == FALSE)
              {
                rr_gprs_send_pccf(USE_PRELOADED_CAUSE, gas_id);
              }
              else
              {
                MSG_GERAN_HIGH_0_G("Not re-sending PCCF, transmission in progress");
              }
            }
          }
          break;

          case EV_MAC_IDLE_MODE:
          {
            ;  // ignore - wait for EV_PS_ACCESS_BLOCKED
          }
          break;

          case EV_PS_ACCESS_BLOCKED:
          {
            rr_resel_send_omsg_mac_idle_ind(gas_id);
          }
          break;

          case EV_CR_G2W_TIMER_EXPIRED:
          {
            MSG_GERAN_HIGH_0_G("EV_CR_G2W_TIMER_EXPIRED rxd");

            if (rr_resel_data_ptr->rr_gprs_reselection_control == MS_CONTROL
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                && !rr_x2g_cco_in_progress(gas_id)
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                )
            {
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
              rr_resel_algo_e resel_algo = rr_resel_algo(gas_id);

              switch (resel_algo)
              {
                case RR_RESEL_ALGO_RANK_BASED:
                {
                  if (rr_irat_check_for_rank_based_reselection(
                        get_recent_reselection_flag_value(gas_id),
                        FALSE,
                        FALSE,
                        FALSE,
                        FALSE,
                        gas_id
                      ))
                  {
                    rr_resel_data_ptr->resel_type = RESEL_G2W_NORMAL;
                    new_event = EV_CELL_RESELECTION;
                  }
                  break;
                }/* RR_RESEL_ALGO_RANK_BASED */

#ifdef FEATURE_LTE
                case RR_RESEL_ALGO_PRIORITY_BASED:
                {
                  if (rr_irat_check_for_priority_based_reselection(
                        rr_resel_to_lower_priority_layers_permitted(gas_id),
                        FALSE,   
                        gas_id
                      ))
                  {
                    rr_resel_data_ptr->resel_type = RESEL_G2W_NORMAL;
                    new_event = EV_CELL_RESELECTION;
                  }
                  break;
                } /* RR_RESEL_ALGO_PRIORITY_BASED */
#endif /* FEATURE_LTE */

                default:
                {
                  new_event = EV_NO_CELL_RESELECTION;
                }
              }
#else /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */
              new_event = EV_NO_CELL_RESELECTION;
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */
            }
            else
            {
              MSG_GERAN_HIGH_0_G("no autonomous reselection in NC2");
              new_event = EV_NO_CELL_RESELECTION;
            }
          } /* EV_CR_G2W_TIMER_EXPIRED */
          break;

          case EV_ALL_NACC_PNCD_DISPATCHED:
          {
            /* No action necessary. */
            break;
          } /* EV_ALL_NACC_PNCD_DISPATCHED */

          default:
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_resel_state_name(rr_resel_data_ptr->state));
          break;
        }
      }
      break;


     /**
       * RR_RESEL_STATE_GOING_INACTIVE
       *
       *
       * In this state:
       *
       *
       *
       * This state handles the following messages and events:
       *
       * Messages:
       *
       *
       * Events:
       *
       **/
      case RR_RESEL_STATE_GOING_INACTIVE:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_PS_IMSG:
                  {
                    rr_ps_access_control(message, gas_id);
                    break;
                  } /* RR_PS_IMSG */

                  case RR_PS_OMSG:
                  {
                    switch (message->rr.header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        new_event = EV_PS_ACCESS_BLOCKED;
                        break;
                      } /* RR_PS_OMSG_NO_PS_ACCESS_CNF */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      }
                    }
                    break;
                  } /* RR_PS_OMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_MAC_IDLE_MODE:
          {
            break;  // ignore - wait for EV_PS_ACCESS_BLOCKED
          } /* EV_MAC_IDLE_MODE */

          case EV_PS_ACCESS_BLOCKED:
          {
            /*Set RR/GRR as no longer camped*/
            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

            scell_info_ptr->gsm.camped_on_flag = FALSE;

            MSG_GERAN_HIGH_0_G("EV_MAC_IDLE_MODE rxd in RR_RESEL_STATE_GOING_INACTIVE");

            /* cancel any remaining timers */
            rr_gprs_cancel_all_five_second_timers(gas_id);

            /* PS access is already blocked, but ensure any remaining PDUs are deleted */
            rr_ps_no_ps_access_del_all(gas_id);

            /*Reinitialise cell reselection */
            rr_resel_init(gas_id);

            /*Change the reselection state back to the begining*/
            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;

            /*TODO Check these return causes, Abort Cnf here?*/
            /*Force the grr state back to NULL and start cell selection in rr_control*/
            output_event = RR_EV_TRIGGER_CELL_SELECTION;
            break;
          } /* EV_PS_ACCESS_BLOCKED */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_resel_state_name(rr_resel_data_ptr->state));
          }
        }
      }
      break;

     /**
      * RR_RESEL_STATE_C1
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_C1:
      {
        /****************************************************************/
        /* Message Handlers                                             */
        /*                                                              */
        /****************************************************************/
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.cell_acq.omsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_OMSG_FINISHED_IND:
                      {
                        MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_OMSG_FINISHED_IND status:0x%x rxd",
                          message->rr.cell_acq.omsg.finished_ind.cell_acq_status);

                        if (rr_timer_cancel(CELL_RESELECTION_TIMER, gas_id))
                        {
                          MSG_GERAN_HIGH_0_G("Stopped the CELL_RESELECTION_TIMER");
                        }

                        switch (message->rr.cell_acq.omsg.finished_ind.cell_acq_status)
                        {
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED:
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH:
                          {
                            rr_gprs_cell_reselection_complete(RESEL_COMPLETE_C1, gas_id);
                            rr_l1_idle_send_imsg_start_idle_req(gas_id);
                            rr_resel_data_ptr->state = RR_RESEL_STATE_C1_PENDING_L1_IDLE_CNF;
                            break;
                          } /* RR_CELL_ACQ_STATUS_SUCCESS_BCCH */

                          default:
                          {
                            grr_perform_reselection_power_scan(gas_id);

                            if (rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries != 0)
                            {
                              rr_perform_rps(&rr_resel_data_ptr->rps_data.pscan_cell_list, gas_id);

                              (void)rr_timer_start(CELL_RESELECTION_TIMER, CELL_RESELECTION_TIMEOUT, gas_id);
                              MSG_GERAN_HIGH_0_G("RESELECTION TIMER STARTED");

                              rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                              rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;
                            }
                            else
                            {
                              rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                              /* cancel any remaining timers */
                              rr_gprs_cancel_all_five_second_timers(gas_id);

                              /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                              rr_ps_no_ps_access_del_all(gas_id);

                              /*Reinitialise cell reselection */
                              rr_resel_init(gas_id);

                              rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                              rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                            }
                          } /* default */
                        }
                        break;
                      } /* RR_CELL_ACQ_OMSG_FINISHED_IND */


                      case RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND rxd");

                        rr_resel_send_omsg_started_ind(RR_RESEL_TYPE_NORMAL, gas_id);
                        rr_gprs_prepare_for_reselection_MFRM51(TRUE, FALSE, gas_id);
                        (void)rr_timer_start( CELL_RESELECTION_TIMER, CELL_RESELECTION_TIMEOUT, gas_id);
                        MSG_GERAN_HIGH_0_G("RR Started CELL_RESELECTION_TIMER");
                        break;
                      } /* RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND */


                      case RR_CELL_ACQ_OMSG_RESELECTED_IND:
                      {
                        /* No action necessary. */
                        break;
                      } /* RR_CELL_ACQ_RESELECTED_IND */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_OMSG */

                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                        switch (message->rr.resel.imsg.abort_req.reason)
                        {
                          case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                          case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                          {
                            /* if MAC has requested establishment then no need to act because MAC will be supended
                               on all exit points from RR_RESEL_STATE_C1:
                                 1) RR_RESEL_ABORT_REQ (MM_ABORT) is rxd. MAC will be suspended for the MM procedure.
                                 2) EV_AUTON_RESEL_TIMEOUT expiry, MAC will already be suspended if this is running because
                                    resel timer is started on reception of RR_CELL_ACQ_MAC_SUSPENDED_IND.
                                 3) RR_CELL_ACQ_FINISHED, RR_CELL_ACQ_MAC_SUSPENDED_IND is received before this.*/

                            /* if RR has requested establishment then this will handled after cell acq has aborted */
                            break;
                          } /* RR_RESEL_ABORT_REASON_MAC_RR_EST ... */

                          case RR_RESEL_ABORT_REASON_MM_REQ:
                          {
                            if (rr_timer_cancel(CELL_RESELECTION_TIMER, gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("Stopped the CELL_RESELECTION_TIMER");
                            }

                            rr_cell_acq_send_imsg_abort_req(gas_id);
                            rr_resel_data_ptr->abort_cell_acq_cause = ABORT_CELL_ACQ_COMMANDED_ABORT;
                            rr_resel_data_ptr->state = RR_RESEL_STATE_C1_ABORTING_CELL_ACQ;
                            break;
                          } /* RR_RESEL_ABORT_REASON_MM_REQ */

                          case RR_RESEL_ABORT_REASON_NACC_NC2:
                          {
                            MSG_GERAN_ERROR_0_G("RR NACC_NC2 cause not expected");
                            break;
                          } /* RR_RESEL_ABORT_REASON_NACC_NC2 */

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                        }
                        break;
                      } /* RR_RESEL_IMSG_ABORT_REQ */


                      case RR_RESEL_IMSG_REQUIRED_IND:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_REQUIRED_IND cause:0x%x rxd",message->rr.resel.imsg.required_ind.cause);
                        /* this signal can be ignored as we are already doing C1 based reselection*/
                        break;
                      } /* RR_RESEL_IMSG_REQUIRED_IND */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      }
                    }
                    break;
                  } /* RR_RESEL_IMSG */

                  case RR_L1_IDLE_IMSG:
                  case RR_L1_IDLE_OMSG:
                  case RR_PS_IMSG:
                  case RR_PS_OMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_RR_L1:
              {
                switch ((rr_l1_message_id_enum_T) message_id)
                {
                  case MPH_SURROUND_MEAS_IND:
#ifdef FEATURE_WCDMA
                  case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
#endif /* FEATURE_WCDMA */
                  {
                    /* No action necessary. */
                    break;
                  } /* MPH_SURROUND_MEAS_IND ... */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_resel_state_name(rr_resel_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_L1 */

              case MS_TIMER:
              {
                switch (message_id)
                {
                  case MID_TIMER_EXPIRY:
                  {
                    timer_expiry_T * timer_expiry = (timer_expiry_T *) message;

                    switch (timer_expiry->timer_id)
                    {
                      case RR_GPRS_SYS_INFO_TIMER:
                      {
                        rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                        break;
                      } /* RR_GPRS_SYS_INFO_TIMER */

                      case CELL_RESELECTION_TIMER:
                      {
                        MSG_GERAN_HIGH_0_G("CELL_RESELECTION_TIMER rxd");
                        rr_cell_acq_send_imsg_abort_req(gas_id);
                        rr_resel_data_ptr->abort_cell_acq_cause = ABORT_CELL_ACQ_RESEL_TIMEOUT;
                        rr_resel_data_ptr->state = RR_RESEL_STATE_C1_ABORTING_CELL_ACQ;
                        break;
                      } /* CELL_RESELECTION_TIMER */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      }
                    }
                    break;
                  } /* MID_TIMER_EXPIRY */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  }
                }
                break;
              } /* MS_TIMER */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_MAC_IDLE_MODE:
          {
            break;  // ignore - wait for EV_PS_ACCESS_BLOCKED
          }

          case EV_PS_ACCESS_BLOCKED:
          {
            rr_cell_acq_control(EV_MAC_IDLE_MODE, NULL, gas_id);
            break;
          } /* EV_MAC_IDLE_MODE */

          default:
          {
            LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
          }
        }
        break;
      } /* RR_RESEL_STATE_C1 */

     /**
      * RR_RESEL_STATE_C1_PENDING_L1_IDLE_CNF
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_C1_PENDING_L1_IDLE_CNF:
      {
        /****************************************************************/
        /* Message Handlers                                             */
        /*                                                              */
        /****************************************************************/
        switch (EV_INPUT_MESSAGE)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_L1_IDLE_IMSG:
                  {
                    rr_l1_idle_control(message, gas_id);
                    break;
                  } /* RR_L1_IDLE_IMSG */

                  case RR_L1_IDLE_OMSG:
                  {
                    switch (message->rr.l1_idle.omsg.header.imh.message_id)
                    {
                      case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                      {
                        rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_IN_SERVICE, gas_id);
                        rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;

                        rr_log_reselect_success(
                          rr_gprs_get_scell_arfcn(gas_id),
                          rr_gprs_get_scell_bsic(gas_id),
                          gas_id
                        );
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_resel_state_name(rr_resel_data_ptr->state)
                    );
                  }
                }
                break;
              }

              default:
                LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
              break;
            }
          }
          break;

          default:
            LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
          break;
        }
      }
      break;

     /**
      * RR_RESEL_STATE_C1_ABORTING_CELL_ACQ
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_C1_ABORTING_CELL_ACQ:
      {
        /****************************************************************/
        /* Message Handlers                                             */
        /*                                                              */
        /****************************************************************/
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  /*This is for the scenario seen in CR513743*/	
                  case RR_PS_OMSG:	
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.cell_acq.omsg.header.imh.message_id)
                    {
                      /* Fallthrough:
                       * RR_CELL_ACQ_FINISHED_IND is a race condition in this state.  Handle as
                       * if RR_CELL_ACQ_ABORT_CNF were received
                       */
                      case RR_CELL_ACQ_OMSG_FINISHED_IND:
                      case RR_CELL_ACQ_OMSG_ABORT_CNF:
                      {
                        if ((rr_cell_acq_omsg_e) message->rr.cell_acq.omsg.header.imh.message_id == RR_CELL_ACQ_OMSG_FINISHED_IND)
                        {
                          MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_OMSG_FINISHED_IND status:0x%x rxd",message->rr.cell_acq.omsg.finished_ind.cell_acq_status);
                        }
                        else if ((rr_cell_acq_omsg_e) message->rr.cell_acq.omsg.header.imh.message_id == RR_CELL_ACQ_OMSG_ABORT_CNF)
                        {
                          MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_ABORT_CNF rxd");
                        }

                        switch (rr_resel_data_ptr->abort_cell_acq_cause)
                        {
                          case ABORT_CELL_ACQ_RESEL_TIMEOUT:
                          {
                            //TODO Move resel finished to IDLE reception in going inactive state, should be abort cnf i think?
                            //rr_resel_send_resel_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE);
                            rr_resel_handle_no_service(gas_id);
                            rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                            // Send request to suspend GPRS (it probably is already)
                            // This will return EV_MAC_IDLE_MODE
                            rr_resel_suspend_gprs(gas_id);

                            rr_resel_data_ptr->state = RR_RESEL_STATE_GOING_INACTIVE;
                          }
                          break;

                          case ABORT_CELL_ACQ_COMMANDED_ABORT:
                          {
                            rr_resel_send_omsg_abort_cnf(gas_id);
                            rr_log_reselect_failure(RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ,gas_id);
                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                          }
                          break;

                          case ABORT_CELL_ACQ_RESEL_REQUEST:
                          {
                            /* Should not receive this cause during C1 reselection */
                            MSG_GERAN_ERROR_1_G("RR Unexpected abort_cell_acq_cause: 0x%x", rr_resel_data_ptr->abort_cell_acq_cause);
                          }
                          break;

                          default:
                            LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                          break;
                        }
                      }
                      break;

                      default:
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      break;
                    }
                  }
                  break;

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  break;
                }
              }
              break;

              default:
                LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
              break;
            }
          }
          break;

          case EV_MAC_IDLE_MODE:
          {
            ;  // ignore - wait for EV_PS_ACCESS_BLOCKED
          }
          break;

          case EV_PS_ACCESS_BLOCKED:
          {
            rr_cell_acq_control(EV_MAC_IDLE_MODE, NULL, gas_id);
          }
          break;

          default:
            LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
          break;
        }
      }
      break;

     /**
      * RR_RESEL_STATE_C2_PENDING_MAC_SUSPENSION
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_C2_PENDING_MAC_SUSPENSION:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.cell_acq.imsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_OMSG_FINISHED_IND:
                      {
                        MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_OMSG_FINISHED_IND status:0x%x rxd",message->rr.cell_acq.omsg.finished_ind.cell_acq_status);

                        switch (message->rr.cell_acq.omsg.finished_ind.cell_acq_status)
                        {
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH:
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED:
                          {
                            /* TODO: Unexpected status, panic reset? */
                            MSG_GERAN_ERROR_1_G("RR Cannot handle success status:0x%x",message->rr.cell_acq.omsg.finished_ind.cell_acq_status);
                            break;
                          } /* RR_CELL_ACQ_STATUS_SUCCESS_BCCH ... */

                          default:
                          {
                            rr_resel_data_ptr->resel_type = RESEL_C2_SELECT_NEXT_BEST_CELL;
                            new_event = EV_CELL_RESELECTION;
                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                          }
                        }
                        break;
                      } /* RR_CELL_ACQ_OMSG_FINISHED_IND */

                      case RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND rxd");
                        rr_resel_send_omsg_started_ind(RR_RESEL_TYPE_NORMAL, gas_id);
                        rr_gprs_prepare_for_reselection_MFRM51(TRUE, FALSE, gas_id);
                        (void)rr_timer_start( CELL_RESELECTION_TIMER, CELL_RESELECTION_TIMEOUT, gas_id);
                        MSG_GERAN_HIGH_0_G("RR Started CELL_RESELECTION_TIMER");
                        rr_resel_data_ptr->rr_resel_c2_pend_abort_req_rxd = FALSE; /*No need to send abort cnf because we have started reselection now.
                                                                                 Any saved messages will be handled when GRR moves from resel->camped state*/
                        rr_resel_data_ptr->state = RR_RESEL_STATE_C2;
                        break;
                      } /* RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_OMSG */

                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_REQUIRED_IND:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_REQUIRED_IND cause:0x%x rxd",message->rr.resel.imsg.required_ind.cause);

                        switch (message->rr.resel.imsg.required_ind.cause)
                        {
                          case RR_RESEL_CAUSE_RPS:
                            grr_perform_reselection_power_scan(gas_id);

                            rr_resel_data_ptr->rr_resel_c2_pend_abort_req_rxd = FALSE;

                            if (rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries != 0)
                            {
                              rr_perform_rps(&rr_resel_data_ptr->rps_data.pscan_cell_list, gas_id);

                              (void)rr_timer_start( CELL_RESELECTION_TIMER, CELL_RESELECTION_TIMEOUT, gas_id);
                              MSG_GERAN_HIGH_0_G("RESELECTION TIMER STARTED");

                              rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                              rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;

                            }
                            else
                            {
                              rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                              /* cancel any remaining timers */
                              rr_gprs_cancel_all_five_second_timers(gas_id);

                              /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                              rr_ps_no_ps_access_del_all(gas_id);

                              /*Reinitialise cell reselection */
                              rr_resel_init(gas_id);

                              rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                              rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                            }
                          break;

                          case RR_RESEL_CAUSE_RESELECTION:
                            rr_resel_data_ptr->rr_resel_c2_pend_abort_req_rxd = FALSE;

                            /*cancel any 5sec timers still running*/
                            rr_gprs_cancel_all_five_second_timers(gas_id);

                             /*Stop the sys info timer*/
                            (void)rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id);

                            rr_cell_acq_send_imsg_abort_req(gas_id);
                            rr_resel_data_ptr->abort_cell_acq_cause = ABORT_CELL_ACQ_RESEL_REQUEST;
                            rr_resel_data_ptr->state = RR_RESEL_STATE_C2_ABORTING_CELL_ACQ;
                          break;

                          default:
                            MSG_GERAN_ERROR_1_G("RR bad resel_cause: 0x%x",message->rr.resel.imsg.required_ind.cause);
                          break;
                        }
                      }
                      break;

                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                        switch (message->rr.resel.imsg.abort_req.reason)
                        {
                          case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                          {
                            /* Ignore Request, handle after resel has completed */
                            rr_resel_data_ptr->rr_resel_c2_pend_abort_req_rxd = TRUE;
                            break;
                          } /* RR_RESEL_ABORT_REASON_MAC_RR_EST */

                          /* Abort Reselection if MAC or RR est is for page response */
                          case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                          case RR_RESEL_ABORT_REASON_MM_REQ:
                          {
                            rr_resel_data_ptr->rr_resel_c2_pend_abort_req_rxd = FALSE;

                            if (rr_timer_cancel(CELL_RESELECTION_TIMER, gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("Stopped the CELL_RESELECTION_TIMER");
                            }

                            rr_cell_acq_send_imsg_abort_req(gas_id);
                            rr_resel_data_ptr->abort_cell_acq_cause = ABORT_CELL_ACQ_COMMANDED_ABORT;
                            rr_resel_data_ptr->state = RR_RESEL_STATE_C2_ABORTING_CELL_ACQ;
                            break;
                          } /* RR_RESEL_ABORT_REASON_PAGE_RESPONSE ... */

                          case RR_RESEL_ABORT_REASON_NACC_NC2:
                          {
                            /*TODO check that this is unexpected */
                            MSG_GERAN_ERROR_0_G("RR NACC_NC2 cause not expected");
                            break;
                          } /* RR_RESEL_ABORT_REASON_NACC_NC2 */

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                          break;
                        }
                        break;
                      } /* RR_RESEL_IMSG_ABORT_REQ */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      }
                    }
                    break;
                  } /* RR_RESEL_IMSG */

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  break;
                }
              }
              break;

              case MS_RR_L1:
              {
                switch ((rr_l1_message_id_enum_T) message_id)
                {
                  case MPH_SERVING_MEAS_IND:
                  case MPH_SERVING_IDLE_MEAS_IND:
                  case MPH_SERVING_AUX_MEAS_IND:
                  case MPH_SURROUND_MEAS_IND:
#ifdef FEATURE_WCDMA
                  case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
                  case MPH_SURROUND_WCDMA_XFER_MEAS_IND:
#endif /* #ifdef FEATURE_WCDMA */
                  case MPH_FCCH_SCH_DECODE_IND:
                  case MPH_NC_MEASUREMENT_IND:
#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
                  case MPH_SURROUND_IRAT_MEAS_IND:
#endif /* FEATURE_LTE || FEATURE_GSM_TDS*/
                  {
                    /* these signals can be ignored in this state*/
                  }
                  break;

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  break;
                }
              }
              break;

              default:
              {
                rr_cell_acq_control(event, message, gas_id);
              }
            }
          }
          break;

          case EV_MAC_IDLE_MODE:
          {
            ;  // ignore - wait for EV_PS_ACCESS_BLOCKED
          }
          break;

          case EV_PS_ACCESS_BLOCKED:
          {
            rr_cell_acq_control(EV_MAC_IDLE_MODE, NULL, gas_id);
          }
          break;

          default:
            LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
          break;
        }
      }
      break;

     /**
      * RR_RESEL_STATE_C2
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_C2:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                        switch (message->rr.resel.imsg.abort_req.reason)
                        {
                          case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                            /*Fall Through */
                          case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                            /* Ignore Request, not expected.
                             * MAC should already be suspended.
                             * GRR control will not pass on RR_EST_REQ in Reselection state
                             */
                             MSG_GERAN_ERROR_1_G("RR Unexpected Resel Abort Req, abort_type: 0x%x:",message->rr.resel.imsg.abort_req.reason);
                          break;

                          case RR_RESEL_ABORT_REASON_MM_REQ:
                          {
                            if (rr_timer_cancel(CELL_RESELECTION_TIMER, gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("Stopped the CELL_RESELECTION_TIMER");
                            }

                            rr_cell_acq_send_imsg_abort_req(gas_id);
                            rr_resel_data_ptr->abort_cell_acq_cause = ABORT_CELL_ACQ_COMMANDED_ABORT;
                            rr_resel_data_ptr->state = RR_RESEL_STATE_C2_ABORTING_CELL_ACQ;
                          }
                          break;

                          case RR_RESEL_ABORT_REASON_NACC_NC2:
                          {
                            /*TODO check that this is unexpected */
                            MSG_GERAN_ERROR_0_G("RR NACC_NC2 cause not expected");
                          }
                          break;

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                          break;
                        }
                      }
                      break;

                      default:
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      break;
                    }
                  }
                  break;

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.cell_acq.omsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_OMSG_FINISHED_IND:
                      {
                        MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_OMSG_FINISHED_IND status:0x%x rxd",message->rr.cell_acq.omsg.finished_ind.cell_acq_status);
                        if( resel_type_C2_based(gas_id))
                        {
                          MSG_GERAN_HIGH_0_G("C2 based reselction finished now clear SI2q insnatce");						
                          rr_initialize_SI2q_instances(gas_id);
                          rr_initialize_SI2ter_instances(gas_id);
                        }

                        if (rr_timer_cancel(CELL_RESELECTION_TIMER, gas_id))
                        {
                          MSG_GERAN_HIGH_0_G("Stopped the CELL_RESELECTION_TIMER");
                        }

                        switch(message->rr.cell_acq.omsg.finished_ind.cell_acq_status)
                        {
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED:
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH:
                          {
                            rr_gprs_cell_reselection_complete(RESEL_COMPLETE_C2, gas_id);
                            rr_l1_idle_send_imsg_start_idle_req(gas_id);
                            rr_resel_data_ptr->state = RR_RESEL_STATE_C2_PENDING_L1_IDLE_CNF;
                            break;
                          } /* RR_CELL_ACQ_STATUS_SUCCESS_BCCH */

                          default:
                          {
                            grr_perform_reselection_power_scan(gas_id);

                            if(rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries != 0)
                            {
                              rr_perform_rps(&rr_resel_data_ptr->rps_data.pscan_cell_list, gas_id);

                              (void)rr_timer_start( CELL_RESELECTION_TIMER, CELL_RESELECTION_TIMEOUT, gas_id);
                              MSG_GERAN_HIGH_0_G("RESELECTION TIMER STARTED");

                              rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                              rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;

                            }
                            else
                            {
                              rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                              /* cancel any remaining timers */
                              rr_gprs_cancel_all_five_second_timers(gas_id);

                              /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                              rr_ps_no_ps_access_del_all(gas_id);

                              /*Reinitialise cell reselection */
                              rr_resel_init(gas_id);

                              rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                              rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                            }
                          } /* default */
                        }
                        break;
                      } /* RR_CELL_ACQ_OMSG_FINISHED_IND */

                      case RR_CELL_ACQ_OMSG_RESELECTED_IND:
                      {
                        /* Ignore only required for NACC */
                        break;
                      } /* RR_CELL_ACQ_OMSG_RESELECTED_IND */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_OMSG */

                  case RR_L1_IDLE_IMSG:
                  case RR_L1_IDLE_OMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_TIMER:
              {
                switch (message_id)
                {
                  case MID_TIMER_EXPIRY:
                  {
                    timer_expiry_T * timer_expiry = (timer_expiry_T *) message;

                    switch (timer_expiry->timer_id)
                    {
                      case RR_GPRS_SYS_INFO_TIMER:
                      {
                        rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                        break;
                      } /* RR_GPRS_SYS_INFO_TIMER */

                      case CELL_RESELECTION_TIMER:
                      {
                        MSG_GERAN_HIGH_0_G("CELL_RESELECTION_TIMER rxd");
                        rr_cell_acq_send_imsg_abort_req(gas_id);
                        rr_resel_data_ptr->abort_cell_acq_cause = ABORT_CELL_ACQ_RESEL_TIMEOUT;
                        rr_resel_data_ptr->state = RR_RESEL_STATE_C2_ABORTING_CELL_ACQ;
                        break;
                      } /* CELL_RESELECTION_TIMER */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      }
                    }
                    break;
                  } /* MID_TIMER_EXPIRY */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  }
                }
                break;
              } /* MS_TIMER */


              default:
              {
                rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          default:
          {
            LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
          }
        }
        break;
      } /* RR_RESEL_STATE_C2 */


     /**
      * RR_RESEL_STATE_C2_PENDING_L1_IDLE_CNF
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_C2_PENDING_L1_IDLE_CNF:
      {
        /****************************************************************/
        /* Message Handlers                                             */
        /*                                                              */
        /****************************************************************/
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_L1_IDLE_IMSG:
                  {
                    rr_l1_idle_control(message, gas_id);
                    break;
                  } /* RR_L1_IDLE_IMSG */

                  case RR_L1_IDLE_OMSG:
                  {
                    switch (message->rr.l1_idle.omsg.header.imh.message_id)
                    {
                      case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                      {
                        rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_IN_SERVICE, gas_id);
                        rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;

                        rr_log_reselect_success(
                          rr_gprs_get_scell_arfcn(gas_id),
                          rr_gprs_get_scell_bsic(gas_id),
                          gas_id
                        );
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_resel_state_name(rr_resel_data_ptr->state)
                    );
                  }
                }
                break;
              }

              default:
                LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
              break;
            }
          }
          break;

          default:
            LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
          break;
        }
      }
      break;

     /**
      * RR_RESEL_STATE_C2_ABORTING_CELL_ACQ
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_C2_ABORTING_CELL_ACQ:
      {
       /****************************************************************/
       /* Message Handlers                                             */
       /*                                                              */
       /****************************************************************/
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.cell_acq.omsg.header.imh.message_id)
                    {
                      /* Fallthrough:
                       * RR_CELL_ACQ_FINISHED_IND is a race condition in this state.  Handle as
                       * if RR_CELL_ACQ_ABORT_CNF were received
                       */
                      case RR_CELL_ACQ_OMSG_FINISHED_IND:
                      case RR_CELL_ACQ_OMSG_ABORT_CNF:
                      {
                        if ((rr_cell_acq_omsg_e) message->rr.cell_acq.omsg.header.imh.message_id == RR_CELL_ACQ_OMSG_FINISHED_IND)
                        {
                          MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_OMSG_FINISHED_IND status:0x%x rxd",message->rr.cell_acq.omsg.finished_ind.cell_acq_status);
                        }
                        else if ((rr_cell_acq_omsg_e) message->rr.cell_acq.omsg.header.imh.message_id == RR_CELL_ACQ_OMSG_ABORT_CNF)
                        {
                          MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_ABORT_CNF rxd");
                        }

                        switch (rr_resel_data_ptr->abort_cell_acq_cause)
                        {
                          case ABORT_CELL_ACQ_RESEL_TIMEOUT:
                          {
                            //TODO Move resel finished to IDLE reception in going inactive state, should be abort cnf i think?
                            //rr_resel_send_resel_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE);
                            rr_resel_handle_no_service(gas_id);

                            /* Reselection may not have been started when abort request was received */
                            if (rr_log_reselection_log_complete(gas_id) == 0)
                            {
                              rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);
                            }

                            // Send request to suspend GPRS (it probably is already)
                            // This will return EV_MAC_IDLE_MODE
                            rr_resel_suspend_gprs(gas_id);

                            rr_resel_data_ptr->state = RR_RESEL_STATE_GOING_INACTIVE;
                          }
                          break;

                          case ABORT_CELL_ACQ_COMMANDED_ABORT:
                          {
                            rr_resel_send_omsg_abort_cnf(gas_id);

                            /* Reselection may not have been started when abort request was received */
                            if (rr_log_reselection_log_complete(gas_id) == 0)
                            {
                              rr_log_reselect_failure(RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ, gas_id);
                            }

                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                          }
                          break;

                          case ABORT_CELL_ACQ_RESEL_REQUEST:
                          {
                            /* The serving cell has gone bad so do an immediate C1 resel to the same target cell*/
                            rr_resel_data_ptr->resel_type = RESEL_C1_TARGET_CELL_ALREADY_SELECTED;
                            new_event = EV_CELL_RESELECTION;

                            /* Reselection may not have been started when abort request was received */
                            if (rr_log_reselection_log_complete(gas_id) == 0)
                            {
                              rr_log_reselect_failure(RR_LOG_RES_FAILURE_CELL_SELECTION, gas_id);
                            }

                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                          }
                          break;

                          default:
                            MSG_GERAN_ERROR_1_G("RR RESEL Unknown abort cause: 0x%x", rr_resel_data_ptr->abort_cell_acq_cause);
                          break;
                        }
                      }
                      break;

                      default:
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      break;
                    }
                  }
                  break;

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  break;
                }
              }
              break;

              default:
                LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
              break;
            }
          }
          break;

          case EV_MAC_IDLE_MODE:
          {
            /* Ignore, wait for EV_PS_ACCESS_BLOCKED */
            break;
          } /* EV_MAC_IDLE_MODE */

          case EV_PS_ACCESS_BLOCKED:
          {
            rr_cell_acq_control(EV_MAC_IDLE_MODE, NULL, gas_id);
            break;
          } /* EV_PS_ACCESS_BLOCKED */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_resel_state_name(rr_resel_data_ptr->state));
          }
        }
        break;
      } /* RR_RESEL_STATE_C2_ABORTING_CELL_ACQ */



     /**
      * RR_RESEL_STATE_NACC
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_NACC:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.cell_acq.omsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND rxd");

                        if ( rr_log_reselection_log_complete(gas_id) )
                        {
                          rr_log_reselect_start(RR_RESELECTION_TRIGGER_C2, gas_id);
                        }

                        rr_resel_send_omsg_started_ind(RR_RESEL_TYPE_NORMAL, gas_id);
                        (void)rr_timer_start( CELL_RESELECTION_TIMER, CELL_RESELECTION_TIMEOUT, gas_id);
                        MSG_GERAN_HIGH_0_G("RR Started CELL_RESELECTION_TIMER");
                      }
                      break;

                      case RR_CELL_ACQ_OMSG_RESELECTED_IND:
                      {
                        boolean pncd_ok;

                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_RESELECTED_IND rxd");

                        /* determine whether we have enough ncell data to camp on */
                        pncd_ok = rr_gprs_is_pncd_enough_to_camp(gas_id);

                        if (pncd_ok)
                        {
                          rr_cell_acq_send_imsg_nacc_acq_complete_ind(gas_id);
                        }
                        else
                        {
                          /* All reselections must be CCCH based at this point, as */
                          /* there is insufficient data for a transition to PCCCH  */
                          MSG_GERAN_MED_0_G("NACC: insufficient neighbour cell data");

                          /* Wait in state for acquisition state machine to acquire sys info */

                          /* reset the utility module (PNCD storage etc.) */
                          rr_gprs_nacc_reset(gas_id);
                        }
                      }
                      break;

                      case RR_CELL_ACQ_OMSG_FINISHED_IND:
                      {
                        MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_OMSG_FINISHED_IND status:0x%x rxd",message->rr.cell_acq.omsg.finished_ind.cell_acq_status);

                        if (rr_timer_cancel(CELL_RESELECTION_TIMER, gas_id))
                        {
                          MSG_GERAN_HIGH_0_G("Stopped the CELL_RESELECTION_TIMER");
                        }

                        switch(message->rr.cell_acq.omsg.finished_ind.cell_acq_status)
                        {
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED:
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH:
                          {
                            rr_gprs_cell_reselection_complete(RESEL_COMPLETE_C2, gas_id);
                            rr_l1_idle_send_imsg_start_idle_req(gas_id);
                            rr_resel_data_ptr->state = RR_RESEL_STATE_NACC_PENDING_L1_IDLE_CNF;
                            break;
                          } /* RR_CELL_ACQ_STATUS_SUCCESS_BCCH */

                          default:
                          {
                            if (rr_resel_data_ptr->nacc_pcco_received)
                            {
                              (void)rr_timer_cancel(T3174, gas_id);

                              MSG_GERAN_HIGH_1_G("SI ACQ failed, starting T3176 in state %s", rr_resel_state_name(rr_resel_data_ptr->state));
                              (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);

                              rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_PCCO_FAILED, gas_id);

                              rr_reselect_old_scell_after_pcco_failure(rr_resel_data_ptr, gas_id);

                              rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;
                              rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_FAILED;

                              rr_resel_send_cell_acq_start_ind(
                                RR_CELL_ACQ_ACTION_RESELECT_BCCH,
                                RR_RESEL_CLEAR_SI_DB,
                                gas_id
                              );
                            }
                            else
                            {
                              grr_perform_reselection_power_scan(gas_id);

                              if (rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries != 0)
                              {
                                rr_perform_rps(&rr_resel_data_ptr->rps_data.pscan_cell_list, gas_id);

                                (void) rr_timer_start(
                                  CELL_RESELECTION_TIMER,
                                  CELL_RESELECTION_TIMEOUT,
                                  gas_id
                                );

                                MSG_GERAN_HIGH_0_G("RESELECTION TIMER STARTED");

                                rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                                rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;
                              }
                              else
                              {
                                rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                                /* cancel any remaining timers */
                                rr_gprs_cancel_all_five_second_timers(gas_id);

                                /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                                rr_ps_no_ps_access_del_all(gas_id);

                                /* Reinitialise cell reselection */
                                rr_resel_init(gas_id);

                                rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                                rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                              }
                            }
                          } /* default */
                        }

                        MSG_GERAN_HIGH_2_G("Clearing PCCC/PCCO flags (%d, %d)", rr_resel_data_ptr->nacc_pccc_received, rr_resel_data_ptr->nacc_pcco_received);
                        rr_resel_data_ptr->nacc_pccc_received = FALSE;
                        rr_resel_data_ptr->nacc_pcco_received = FALSE;

                        break;
                      } /* RR_CELL_ACQ_OMSG_FINISHED_IND */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_OMSG */

                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                        switch (message->rr.resel.imsg.abort_req.reason)
                        {
                          /*Fall through*/
                          case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                          case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                          case RR_RESEL_ABORT_REASON_NACC_NC2:
                            MSG_GERAN_ERROR_1_G("RR Unexpected Abort Cause: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          break;

                          case RR_RESEL_ABORT_REASON_MM_REQ:
                          {
                            rr_gprs_cancel_all_five_second_timers(gas_id);
                            rr_gprs_reset_reselection_list(gas_id);
                            rr_gprs_nacc_reset(gas_id);
                            rr_cell_acq_send_imsg_abort_req(gas_id);
                            rr_resel_data_ptr->abort_cell_acq_cause = ABORT_CELL_ACQ_COMMANDED_ABORT;
                            rr_resel_data_ptr->state = RR_RESEL_STATE_NACC_ABORTING_CELL_ACQ;
                          }
                          break;

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                          break;
                        }
                      }
                      break;

                      default:
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      break;
                    }
                  }
                  break;

                  case RR_L1_IDLE_IMSG:
                  case RR_L1_IDLE_OMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  }

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  break;
                }
              }
              break;

              case MS_TIMER:
              {
                switch (message_id)
                {
                  case MID_TIMER_EXPIRY:
                  {
                    timer_expiry_T *timer_expiry = (timer_expiry_T *) message;

                    switch (timer_expiry->timer_id)
                    {
                      case RR_GPRS_SYS_INFO_TIMER:
                      {
                        rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                        break;
                      } /* RR_GPRS_SYS_INFO_TIMER */

                      case CELL_RESELECTION_TIMER:
                      {
                        MSG_GERAN_HIGH_0_G("CELL_RESELECTION_TIMER rxd");

                        rr_cell_acq_send_imsg_abort_req(gas_id);
                        rr_resel_data_ptr->abort_cell_acq_cause = ABORT_CELL_ACQ_RESEL_TIMEOUT;
                        rr_resel_data_ptr->state = RR_RESEL_STATE_C1_ABORTING_CELL_ACQ;
                        break;
                      } /* CELL_RESELECTION_TIMER */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      }
                    }
                    break;
                  } /* MID_TIMER_EXPIRY */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  }
                }
                break;
              } /* MS_TIMER */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
              }
              break;
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_T3174_TIMEOUT:
          {
            MSG_GERAN_HIGH_0_G("EV_T3174_TIMEOUT rxd");

            rr_log_reselect_failure(RR_LOG_RES_FAILURE_T3174_TIMEOUT, gas_id);

            MSG_GERAN_HIGH_1_G("T3174 expired, starting T3176 in state %s", rr_resel_state_name(rr_resel_data_ptr->state));
            (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);

            rr_cell_acq_send_imsg_abort_req(gas_id);

            rr_resel_data_ptr->pcco_data.pccf.cause = (uint8) NO_RESPONSE_ON_TARGET;

            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_T3174_TIMER_EXPIRY;

            rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_ABORTING_CELL_ACQ;
            break;
          } /* EV_T3174_TIMEOUT */

          case EV_MAC_IDLE_MODE:
          {
            ;  // ignore - wait for EV_PS_ACCESS_BLOCKED
          } /* EV_MAC_IDLE_MODE */
          break;

          case EV_PS_ACCESS_BLOCKED:
          {
            rr_cell_acq_control(EV_MAC_IDLE_MODE, NULL, gas_id);
            break;
          } /* EV_PS_ACCESS_BLOCKED */

          default:
          {
            LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
          }
        }
        break;
      } /* RR_RESEL_STATE_NACC */

     /**
      * RR_RESEL_STATE_NACC_PENDING_NW_RESPONSE
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_NACC_PENDING_NW_RESPONSE:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch ((rr_l1_message_id_enum_T) message_id)
                {
                  case MPH_SERVING_MEAS_IND:
                  {
                    gprs_reselection_structure_T *local_gprs_reselection_list_ptr;

                    local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

                    rr_gprs_store_serving_meas(&message->mph_serving_meas_ind, gas_id);

                    if (local_gprs_reselection_list_ptr->serving_values.C1 < 0 &&
                        !rr_allow_negative_c1(rr_gprs_get_scell_plmn_id(gas_id), gas_id))
                    {
                      MSG_GERAN_HIGH_1_G("Serving cell C1 value: %d", local_gprs_reselection_list_ptr->serving_values.C1);
                      rr_resel_send_imsg(RR_RESEL_IMSG_C1_LT_ZERO_IND, gas_id);
                    }
                    break;
                  } /* MPH_SERVING_MEAS_IND */

                  case MPH_SURROUND_MEAS_IND:
#ifdef FEATURE_WCDMA
                  case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
                  case MPH_SURROUND_WCDMA_XFER_MEAS_IND:
#endif /* FEATURE_WCDMA */
#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
                  case MPH_SURROUND_IRAT_MEAS_IND:
#endif /* FEATURE_LTE || FEATURE_GSM_TDS*/
                    /*ignore*/
                  break;

                  default:
                    LOG_UNEXPECTED_MESSAGE(message,
                                           rr_resel_state_name(rr_resel_data_ptr->state));
                  break;
                }
              }
              break;

              case MS_MAC_RR:
              {
                switch ((mac_grr_sig_id_t) message_id)
                {
                  case MAC_GRR_DATA_IND:
                  {
                    mac_grr_data_ind_t *mac_grr_data_ind;

                    mac_grr_data_ind = &message->mac_grr_sig.mac_grr_data_ind;

                    /**************************************************************/
                    /* A PCCO message has been received. Leave CCN state and obey */
                    /* standard PCCO procedures                                   */
                    /**************************************************************/
                    switch (mac_grr_data_ind->data_ptr.header_ptr->message_type)
                    {
                      case PACKET_CELL_CHANGE_ORDER:
                      {
                        void *                       data_ptr = mac_grr_data_ind->data_ptr.data_ptr;
                        packet_cell_change_order_t * pcco = (packet_cell_change_order_t *) data_ptr;
                        ARFCN_T                      nacc_target = rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN;

                        MSG_GERAN_HIGH_0_G("PACKET_CELL_CHANGE_ORDER rxd");

                        (void)rr_timer_cancel(T3208, gas_id);
                        (void)rr_timer_cancel(T3210, gas_id);

                        MSG_GERAN_HIGH_1_G("A PCCO has been received in the CCN state(%d)",(int)nacc_target.num);

                        /*

                        According to 3GPP 44.060 section 8.8.3:

                        "When the PACKET CELL CHANGE ORDER message is received, the
                        mobile station shall stop timer T3208, leave CCN mode and
                        follow the procedures as specified for the PACKET CELL CHANGE
                        ORDER message (sub-clause 8.4) and in sub-clause 8.8.1."

                        So if we receive a PCCO and GMM is not in READY state then we
                        should exit CCN mode and behave as we would normally if we
                        received a PCCO with GMM not in READY state, i.e. send a PCCF
                        and go back to ALLOWED_CELL_RESELECTION state.

                        */

                        if (rr_gmminfo_get_gmm_status(gas_id) == GMM_READY_STATE)
                        {
                          rr_gprs_nacc_notify_event(CCN_RESPONSE_FROM_NW, gas_id);

                          new_event = rr_gprs_process_PCCO(pcco, gas_id);

                          if (new_event == EV_ACTION_PCCO_WITH_PNCD)
                          {
                            MSG_GERAN_MED_0_G("PCCO with PNCD actioned...");
                            new_event = EV_NO_EVENT;

                            rr_gprs_nacc_check_pcco_reject(pcco->cell.gsm.arfcn, nacc_target, gas_id);

                            /* make this cell the only possible target */
                            rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN = pcco->cell.gsm.arfcn;

                            /* Prepare the database for pncd nacc reselection */
                            rr_gprs_prepare_for_reselection_with_PNCD(INVALID_CONTAINER_ID, gas_id);

                            rr_resel_data_ptr->nacc_pcco_received = TRUE;

                            /* Start the PCCO timer */
                            (void)rr_timer_start(T3174, T3174_TIMEOUT, gas_id);
                          }
                          else if (new_event == EV_ACTION_PCCO || new_event == EV_ACTION_IMMEDIATE_PCCO)
                          {
                            rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO;
                          }
                          else
                          {
                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                          }
                        }
                        else
                        {
                          MSG_GERAN_HIGH_0_G("PCCO received but GMM not in READY state, reject PCCO");

                          rr_gprs_nacc_notify_event(CCN_RESET, gas_id);

                          rr_gprs_load_pccf_from_pcco(pcco, gas_id);

                          /* determine PCCF cause value - see 3GPP 44.060 section 8.4.2 */
                          if (rr_gmminfo_get_neg_ready_timer_zero(gas_id))
                          {
                            rr_gprs_send_pccf(FORCED_TO_STANDBY, gas_id);
                          }
                          else
                          {
                            rr_gprs_send_pccf(MS_IN_GMM_STANDBY, gas_id);
                          }

                          rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        }
                      }
                      break;

#ifdef FEATURE_GPRS_PS_HANDOVER
                      case PACKET_PS_HANDOVER:
                      {
                        void * data_ptr = mac_grr_data_ind->data_ptr.data_ptr;
                        ps_handover_command_t *psho = (ps_handover_command_t *) data_ptr;
                        MSG_GERAN_HIGH_0_G("A PSHO has been received in the CCN state");
                        rr_gprs_nacc_notify_event(CCN_RESPONSE_FROM_NW, gas_id);

                        if (rr_gprs_reselection_process_psho_command(psho, gas_id) == EV_ACTION_PSHO)
                        {
                          output_event = EV_PSHO_STARTED;
                        }
                      }
                      break;
#endif /* FEATURE_GPRS_PS_HANODVER */
                      /**************************************************************/
                      /* A PCCC message has been received. Leave CCN state and begin*/
                      /* the reselection procedures                                 */
                      /**************************************************************/
                      case PACKET_CELL_CHANGE_CONTINUE:
                      {
                        void * data_ptr = mac_grr_data_ind->data_ptr.data_ptr;
                        packet_cell_change_continue_t *pccc = (packet_cell_change_continue_t *) data_ptr;

                        MSG_GERAN_HIGH_0_G("PACKET_CELL_CHANGE_CONTINUE rxd");

                        (void)rr_timer_cancel(T3208, gas_id);
                        (void)rr_timer_cancel(T3210, gas_id);

                        MSG_GERAN_HIGH_1_G("A PCCC(%1d) has been received in the CCN state",pccc->data_valid);
                        rr_gprs_nacc_notify_event(CCN_RESPONSE_FROM_NW, gas_id);

                        if ( pccc->data_valid == valid )
                        {
                          /* check whether the target cell is the same as our original */
                          if (!ARFCNS_EQUAL(rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN, pccc->arfcn))
                          {
                            /* update the target information with the new ARFCN */
                            rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.BCCH_ARFCN = pccc->arfcn;

                            /* store the new target info in the PCCF store */
                            rr_resel_data_ptr->pcco_data.pccf.arfcn = pccc->arfcn;
                            rr_resel_data_ptr->pcco_data.pccf.bsic  = pccc->bsic;
                            rr_resel_data_ptr->pcco_data.pccf.cause = (uint8)PCCF_STORED_BSIC;

                            MSG_GERAN_HIGH_1_G("PCCC has different target ARFCN %d", pccc->arfcn.num);
                          }

                          /* check whether any PNCD has been received for the specified container */
                          if (rr_gprs_nacc_container_empty(pccc->container_id, gas_id))
                          {
                            MSG_GERAN_HIGH_2_G("No PNCD found for container ID %d, %d",pccc->container_id, pccc->arfcn.num);

                            /* No data found in the data store, resume normal reselection */
                            rr_resel_data_ptr->resel_type = RESEL_C1_TARGET_CELL_ALREADY_SELECTED;
                            new_event = EV_CELL_RESELECTION;
                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                          }
                          else
                          {
                            /* prepare the database for reselection using stored PNCD */
                            rr_gprs_prepare_for_reselection_with_PNCD(pccc->container_id, gas_id);

                            /* trigger dispatch of the stored PNCD contents */
                            (void) rr_gprs_nacc_trigger_ncell_data(
                              pccc->container_id,
                              pccc->arfcn,
                              TRUE,
                              gas_id
                            );

                            rr_resel_data_ptr->nacc_pccc_received = TRUE;
                          }
                        }
                        else
                        {
                          rr_event_T exit_ccn_event;

                          /* resume the reselection, no PNCD has been sent to GRR */
                          exit_ccn_event = rr_gprs_nacc_exit_ccn_mode(gas_id);

                          switch (exit_ccn_event)
                          {
                            case EV_RESUME_CELL_RESELECTION:
                              /*NACC resel cannot be completed, reselect to target cell immediately (C1) */
                              rr_resel_data_ptr->resel_type = RESEL_C1_TARGET_CELL_ALREADY_SELECTED;
                              new_event = EV_CELL_RESELECTION;
                              rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                            break;

                            case RR_EV_FORCE_WCDMA_CELL_RESELECTION:
                              /*TODO modify as part of W2G work*/
                            break;

                            default:
                            {
                              MSG_GERAN_ERROR_1_G("Unexpected CCN exit event: %d", exit_ccn_event);
                            }
                          }
                        }
                      }
                      break;

                      default:
                         LOG_UNEXPECTED_MESSAGE(message,
                                                rr_resel_state_name(rr_resel_data_ptr->state));
                      break;
                    }
                  }
                  break;

                  default:
                    LOG_UNEXPECTED_MESSAGE(message,
                                           rr_resel_state_name(rr_resel_data_ptr->state));
                  break;
                }
              }
              break;

              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                        switch (message->rr.resel.imsg.abort_req.reason)
                        {
                          /*Fall through*/
                          case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                          case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                          case RR_RESEL_ABORT_REASON_MM_REQ:
                          {
                            rr_gprs_cancel_all_five_second_timers(gas_id);
                            rr_gprs_reset_reselection_list(gas_id);
                            rr_gprs_nacc_reset(gas_id);
                            rr_resel_send_omsg_abort_cnf(gas_id);

                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                          }
                          break;

                          case RR_RESEL_ABORT_REASON_NACC_NC2:
                          {
                            MSG_GERAN_HIGH_0_G("A PMO has changed the NC mode to 2 - leave CCN state");
                            rr_gprs_nacc_notify_event(CCN_RESET, gas_id);

                            (void)rr_timer_cancel(T3208, gas_id);
                            (void)rr_timer_cancel(T3210, gas_id);

                            rr_clear_better_cell_list(gas_id);

                            rr_resel_send_omsg_abort_cnf(gas_id);

                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                          }
                          break;

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                          break;
                        }
                        break;
                      } /* RR_RESEL_IMSG_ABORT_REQ */

                      case RR_RESEL_IMSG_C1_LT_ZERO_IND:
                      {
                        rr_event_T exit_ccn_event;

                        rr_gprs_nacc_notify_event(CCN_RESET, gas_id);

                        (void)rr_timer_cancel(T3208, gas_id);
                        (void)rr_timer_cancel(T3210, gas_id);

                        exit_ccn_event = rr_gprs_nacc_exit_ccn_mode(gas_id);

                        switch (exit_ccn_event)
                        {
                          case EV_RESUME_CELL_RESELECTION:
                            /*NACC resel cannot be completed, reselect to target cell immediately (C1) */
                            rr_resel_data_ptr->resel_type = RESEL_C1_TARGET_CELL_ALREADY_SELECTED;
                            new_event = EV_CELL_RESELECTION;
                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                          break;

                          case RR_EV_FORCE_WCDMA_CELL_RESELECTION:
                            /*TODO modify as part of W2G work*/
                          break;

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("Unexpected CCN exit event: %d", exit_ccn_event);
                          }
                        }
                        break;
                      } /* RR_RESEL_IMSG_C1_LT_ZERO_IND */

                      default:
                        LOG_UNEXPECTED_MESSAGE(message,
                                               rr_resel_state_name(rr_resel_data_ptr->state));
                      break;
                    }
                  }
                  break;

                  default:
                    LOG_UNEXPECTED_MESSAGE(message,
                                           rr_resel_state_name(rr_resel_data_ptr->state));
                  break;
                }
              }
              break;

              default:
                LOG_UNEXPECTED_MESSAGE(message,
                                       rr_resel_state_name(rr_resel_data_ptr->state));
              break;
            }
          } /* EV_INPUT_MESSAGE */
          break;

          case EV_MAC_IDLE_MODE:
          case EV_T3208_EXPIRY:
          {
            rr_event_T exit_ccn_event;

            if (event == EV_MAC_IDLE_MODE)
            {
              MSG_GERAN_HIGH_0_G("EV_MAC_IDLE_MODE rxd");
            }
            else if (event == EV_T3208_EXPIRY)
            {
              MSG_GERAN_HIGH_0_G("EV_T3208_EXPIRY rxd");
            }

            rr_gprs_nacc_notify_event(CCN_RESET, gas_id);

            (void)rr_timer_cancel(T3208, gas_id);
            (void)rr_timer_cancel(T3210, gas_id);

            exit_ccn_event = rr_gprs_nacc_exit_ccn_mode(gas_id);

            switch (exit_ccn_event)
            {
              case EV_RESUME_CELL_RESELECTION:
                /*NACC resel cannot be completed, reselect to target cell immediately (C1) */
                rr_resel_data_ptr->resel_type = RESEL_C1_TARGET_CELL_ALREADY_SELECTED;
                new_event = EV_CELL_RESELECTION;
                rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
              break;

              case RR_EV_FORCE_WCDMA_CELL_RESELECTION:
                /*TODO modify as part of W2G work*/
              break;

              default:
              {
                MSG_GERAN_ERROR_1_G("Unexpected CCN exit event: %d", exit_ccn_event);
              }
            }
          } /* EV_MAC_IDLE_MODE ... */
          break;

          case EV_CELL_RESELECTION_DEACT:
          {
            MSG_GERAN_HIGH_0_G("A PMO has changed the NC mode to 2 - leave CCN state");
            rr_gprs_nacc_notify_event(CCN_RESET, gas_id);

            (void)rr_timer_cancel(T3208, gas_id);
            (void)rr_timer_cancel(T3210, gas_id);

            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;

            rr_clear_better_cell_list(gas_id);
            break;
          }

          /**************************************************/
          /* T3210 has expired. re-send the PCCN message    */
          /**************************************************/
          case EV_T3210_EXPIRY:
          {
            MSG_GERAN_HIGH_0_G("EV_T3210_EXPIRY rxd");

            {
              MSG_GERAN_HIGH_1_G("Resend PCCN (flag=%1d)",rr_resel_data_ptr->pccn.pccn_sending);

              /* send the CCN message to MAC */
              rr_send_grr_mac_send_pccn_req(&rr_resel_data_ptr->pccn, gas_id);
            }
          } /* EV_T3210_EXPIRY */
          break;

          /**************************************************/
          /* EV_ALL_NACC_PNCD_DISPATCHED                    */
          /* All PNCD that was received has now been placed */
          /* into the pending store                         */
          /**************************************************/
          case EV_ALL_NACC_PNCD_DISPATCHED:
          {
            gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

            MSG_GERAN_HIGH_0_G("EV_ALL_NACC_PNCD_DISPATCHED rxd");

#ifdef FEATURE_GPRS_PS_HANDOVER
            if (rr_psho_in_progress(gas_id))
            {
              if (rr_gprs_is_pncd_enough_to_camp(gas_id) &&
                  !rr_is_forbidden_LAI(&(pcell_info_ptr->gsm.location_area_identification), gas_id))
              {
                output_event = EV_PSHO_CONTINUE;
              }
              else
              {
                output_event = EV_PSHO_REJECTED;
              }

              pcell_info_ptr->gsm.valid_data_flag = TRUE;

              rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
            }
            else
#endif /* FEATURE_GPRS_PS_HANDOVER */
            {
              /* No set the camped on flag to FALSE - setting this prior to this
              may mean an unwanted pbcch_description_ind gets sent to L1 when collecting
              a PNCD PSI1 */
              scell_info_ptr->gsm.camped_on_flag = FALSE;

              /* determine whether we have enough ncell data to camp on */
              if (rr_gprs_is_pncd_enough_to_camp(gas_id))
              {
                /* set the valid data flag to true, RR is ready to camp on this cell */
                pcell_info_ptr->gsm.valid_data_flag = TRUE;
              }
              else
              {
                /* All reselections must be CCCH based at this point, as */
                /* there is insufficient data for a transition to PCCCH  */
                MSG_GERAN_MED_0_G("NACC: insufficient neighbour cell data");
              }

              if (rr_timer_start( CELL_RESELECTION_TIMER, CELL_RESELECTION_TIMEOUT, gas_id))
              {
                MSG_GERAN_HIGH_0_G("CELL_RESELECTION_TIMER started");
              }

              /* Start the reselection then move to the correct state for completion */
              if (rr_resel_data_ptr->nacc_pccc_received ||
                  rr_resel_data_ptr->nacc_pcco_received)
              {
                rr_resel_send_cell_acq_start_ind(
                  RR_CELL_ACQ_ACTION_RESELECT_BCCH_NACC_PCCO_PCCC,
                  RR_RESEL_DONT_CLEAR_SI_DB,
                  gas_id
                );
              }
              else
              {
                rr_resel_send_cell_acq_start_ind(
                  RR_CELL_ACQ_ACTION_RESELECT_BCCH,
                  RR_RESEL_CLEAR_SI_DB,
                  gas_id
                );
              }

              rr_resel_data_ptr->state = RR_RESEL_STATE_NACC;
            }
          } /* EV_ALL_NACC_PNCD_DISPATCHED */
          break;

          default:
          {
            LOG_UNEXPECTED_EVENT(
              rr_event_name(event),
              rr_resel_state_name(rr_resel_data_ptr->state)
            );
          }

        } /* switch (event) */

      } /* RR_RESEL_STATE_NACC_PENDING_NW_RESPONSE */
      break;

     /**
      * RR_RESEL_STATE_NACC_ABORTING_CELL_ACQ
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_NACC_ABORTING_CELL_ACQ:
      {
        /****************************************************************/
        /* Message Handlers                                             */
        /*                                                              */
        /****************************************************************/
        switch (event)
        {
          case  EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.cell_acq.omsg.header.imh.message_id)
                    {
                      /* Fallthrough:
                       * RR_CELL_ACQ_FINISHED_IND is a race condition in this state.  Handle as
                       * if RR_CELL_ACQ_ABORT_CNF were received
                       */
                      case RR_CELL_ACQ_OMSG_FINISHED_IND:
                      case RR_CELL_ACQ_OMSG_ABORT_CNF:
                      {
                        MSG_GERAN_HIGH_0_G("Clearing PCCC/PCCO flags");
                        rr_resel_data_ptr->nacc_pccc_received = FALSE;
                        rr_resel_data_ptr->nacc_pcco_received = FALSE;

                        if ((rr_cell_acq_omsg_e) message->rr.cell_acq.omsg.header.imh.message_id == RR_CELL_ACQ_OMSG_FINISHED_IND)
                        {
                          MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_OMSG_FINISHED_IND status:0x%x rxd",message->rr.cell_acq.omsg.finished_ind.cell_acq_status);
                        }
                        else if ((rr_cell_acq_omsg_e) message->rr.cell_acq.omsg.header.imh.message_id == RR_CELL_ACQ_OMSG_ABORT_CNF)
                        {
                          MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_ABORT_CNF rxd");
                        }

                        switch (rr_resel_data_ptr->abort_cell_acq_cause)
                        {
                          case ABORT_CELL_ACQ_RESEL_TIMEOUT:
                          {
                            //TODO Move resel finished to IDLE reception in going inactive state, should be abort cnf i think?
                            //rr_resel_send_resel_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE);
                            rr_resel_handle_no_service(gas_id);

                            // Send request to suspend GPRS (it probably is already)
                            // This will return EV_MAC_IDLE_MODE
                            rr_resel_suspend_gprs(gas_id);

                            rr_resel_data_ptr->state = RR_RESEL_STATE_GOING_INACTIVE;
                          }
                          break;

                          case ABORT_CELL_ACQ_COMMANDED_ABORT:
                          {
                            rr_resel_send_omsg_abort_cnf(gas_id);
                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                          }
                          break;

                          case ABORT_CELL_ACQ_RESEL_REQUEST:
                          {
                            /* Should not receive this cause during NACC reselection */
                            MSG_GERAN_ERROR_1_G("RR Unexpected abort_cell_acq_cause: 0x%x",rr_resel_data_ptr->abort_cell_acq_cause);
                          }
                          break;

                          default:
                            MSG_GERAN_ERROR_1_G("RR Resel invalid cell acq cause: 0x%x",rr_resel_data_ptr->abort_cell_acq_cause);
                          break;
                        }
                      }
                      break;

                      default:
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      break;
                    }
                  }
                  break;

                  default:
                    LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                  break;
                }
              }
              break;

              default:
                LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
              break;
            }
          }
          break;

          case EV_MAC_IDLE_MODE:
          {
            break;  // ignore - wait for EV_PS_ACCESS_BLOCKED
          } /* EV_MAC_IDLE_MODE */

          case EV_PS_ACCESS_BLOCKED:
          {
            rr_cell_acq_control(EV_MAC_IDLE_MODE, NULL,gas_id);
          }
          break;

          default:
            LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
          break;
        }
      }
      break;

     /**
      * RR_RESEL_STATE_NACC_PENDING_L1_IDLE_CNF
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_NACC_PENDING_L1_IDLE_CNF:
      {
        /****************************************************************/
        /* Message Handlers                                             */
        /*                                                              */
        /****************************************************************/
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_L1_IDLE_IMSG:
                  {
                    rr_l1_idle_control(message, gas_id);
                    break;
                  } /* RR_L1_IDLE_IMSG */

                  case RR_L1_IDLE_OMSG:
                  {
                    switch (message->rr.l1_idle.omsg.header.imh.message_id)
                    {
                      case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                      {
                        rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_IN_SERVICE, gas_id);

                        rr_log_reselect_success(
                          rr_gprs_get_scell_arfcn(gas_id),
                          rr_gprs_get_scell_bsic(gas_id),
                          gas_id
                        );

                        MSG_GERAN_HIGH_0_G("Clearing PCCC/PCCO flags");
                        rr_resel_data_ptr->nacc_pccc_received = FALSE;
                        rr_resel_data_ptr->nacc_pcco_received = FALSE;

                        rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_resel_state_name(rr_resel_data_ptr->state)
                    );
                  }
                }
                break;
              }

              default:
                LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
              break;
            }
          }
          break;

          default:
            LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
          break;
        }
      }
      break;


      /**
      * RR_RESEL_STATE_RPS
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_RPS:
      {
        switch (rr_resel_data_ptr->rps_data.rps_sub_state)
        {
          case RR_RESEL_RPS_IDLE:
          {
            switch ( event )
            {
              case EV_INPUT_MESSAGE:
              {
                if (message == NULL)
                {
                  MSG_GERAN_ERROR_0_G("message == NULL");
                  break;
                }

                switch (message_set)
                {
                  case MS_RR_RR:
                  {
                    switch (message->rr.header.rr_message_set)
                    {
                      case RR_PSCAN_IMSG:
                      {
                        rr_pscan_control(EV_INPUT_MESSAGE, message, gas_id);
                        break;
                      }

                      case RR_RESEL_IMSG:
                      {
                        switch (message->rr.resel.imsg.header.imh.message_id)
                        {
                          case RR_RESEL_IMSG_REQUIRED_IND:
                            /* Ignore RPS already started. */
                          break;

                          default:
                          {
                            LOG_UNEXPECTED_MESSAGE(
                              message,
                              rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                            );
                          }
                        }
                        break;
                      } /* RR_RESEL_IMSG */

                      case RR_PS_IMSG:
                      {
                        rr_ps_access_control(message, gas_id);
                        break;
                      }

                      case RR_PS_OMSG:
                      {
                        switch (message->rr.header.imh.message_id)
                        {
                          case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                          {
                            new_event = EV_PS_ACCESS_BLOCKED;
                            break;
                          }

                          default:
                          {
                            LOG_UNEXPECTED_MESSAGE(
                              message,
                              rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                            );
                          }
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                        );
                      }
                    }
                    break;
                  } /*MS_RR_RR*/

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                     message,
                     rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                    );
                  }
                }
                break;
              }

              case EV_MAC_IDLE_MODE:
              {
                break;  // ignore - wait for EV_PS_ACCESS_BLOCKED
              }

              case EV_PS_ACCESS_BLOCKED:
              {
                gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);

                MSG_GERAN_HIGH_0_G("EV_MAC_IDLE_MODE rxd");

                rr_log_reselect_start(RR_RESELECTION_TRIGGER_DSF, gas_id);
                rr_resel_send_omsg_started_ind(RR_RESEL_TYPE_NORMAL, gas_id);

                scell->gsm.camped_on_flag = FALSE;

                grr_perform_reselection_power_scan(gas_id);

                if(rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries != 0)
                {
                  rr_perform_rps(&rr_resel_data_ptr->rps_data.pscan_cell_list, gas_id);

                  (void)rr_timer_start(CELL_RESELECTION_TIMER, CELL_RESELECTION_TIMEOUT, gas_id);
                  MSG_GERAN_HIGH_0_G("RESELECTION TIMER STARTED");

                  rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;
                }
                else
                {
                  rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                  /* cancel any remaining timers */
                  rr_gprs_cancel_all_five_second_timers(gas_id);

                  /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                  rr_ps_no_ps_access_del_all(gas_id);

                  /*Reinitialise cell reselection */
                  rr_resel_init(gas_id);

                  rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                  rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_EVENT(rr_event_name(event),
                                     rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state));
              }
            }
            break;
          }

          case RR_RESEL_RPS_POWER_SCAN:
          {
#ifdef FEATURE_SGLTE
            /*Optimisation: When SGLTE mode is active, Upon entry to RR_RESEL_RPS_POWER_SCAN state 
              reject any pending X2G reselection.  This prevents PS domain being stuck on G whilst G
              tries to find service via RPS*/
            (void)rr_sglte_reject_pending_x2g_req(FALSE, gas_id);
#endif /*FEATURE_SGLTE*/		  
            switch ( event )
            {
              case EV_INPUT_MESSAGE:
              {
                if (message == NULL)
                {
                  MSG_GERAN_ERROR_0_G("message == NULL");
                  break;
                }

                switch (message_set)
                {
                  case MS_RR_RR:
                  {
                    switch (message->rr.header.rr_message_set)
                    {
                      case RR_PSCAN_IMSG:
                      {
                        rr_pscan_control(EV_INPUT_MESSAGE, message, gas_id);
                        break;
                      }

                      case RR_PSCAN_OMSG:
                      {
                        switch (message->rr.pscan.omsg.header.imh.message_id)
                        {
                          case RR_PSCAN_OMSG_FINISHED_IND:
                          {
                            rr_resel_start_c1_status_e rps_resel_status = START_C1_RESEL_FAILED;

                            MSG_GERAN_HIGH_0_G("RR_PSCAN_OMSG_FINISHED_IND rxd");

                            if (grr_place_results_in_best_six_list(gas_id) == 0)
                            {
                              MSG_GERAN_HIGH_0_G("No BA cells in pscan, RR lost service");

                              /* Print out the results database */
                              rr_gprs_print_pscan_results(gas_id);

                              if (rr_pscan_get_num_frequencies(rr_pscan_results_db_ptr) != 0)
                              {
                                rr_pscan_clear_results(rr_pscan_results_db_ptr);
                              }

                              rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                              /* cancel any remaining timers */
                              rr_gprs_cancel_all_five_second_timers(gas_id);

                              /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                              rr_ps_no_ps_access_del_all(gas_id);

                              /*Reinitialise cell reselection */
                              rr_resel_init(gas_id);

                              rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                              rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;

                              rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;

                            }
                            else
                            {
                              if (grr_start_bcch_decode_list_procedure(gas_id) == TRUE)
                              {
                                /* Triggered bcch list procedure */
                                rr_decode_bcch_list_setup(gas_id);
                                rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_DECODE_BCCH;
                              }
                              else
                              {
                                /*
                                * As all required SIs exist send in a spoof measurement report which
                                * will be used as the basis for the subsequent reselection
                                */
                                rr_gprs_generate_rps_measurement_report(
                                  rr_get_candidate_cell_db_ptr(gas_id),
                                  gas_id
                                );
                                /* find target cell and if there is a target cell then send cell acq start ind here
                                if there is no target cell then do the next set of bcch decode */

                                rps_resel_status = rr_resel_start_c1(TRUE, TRUE, FALSE, gas_id);

                                switch (rps_resel_status)
                                {
                                  case START_C1_RESEL_FAILED:
                                  {
                                    if (rr_gprs_any_more_rps_cells_to_try(
                                          &rr_resel_data_ptr->rps_data.pscan_cell_list,
                                          gas_id
                                        ))
                                    {
                                      MSG_GERAN_HIGH_0_G("RPS LOOP again");
                                      new_event = EV_INPUT_MESSAGE;
                                    }
                                    else
                                    {
                                      rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                                      /* cancel any remaining timers */
                                      rr_gprs_cancel_all_five_second_timers(gas_id);

                                      /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                                      rr_ps_no_ps_access_del_all(gas_id);

                                      /*Reinitialise cell reselection */
                                      rr_resel_init(gas_id);

                                      rr_log_reselect_failure(
                                        RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT,
                                        gas_id
                                      );

                                      rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                                      rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                                    }
                                    break;
                                  } /* START_C1_RESEL_FAILED */

                                  case START_C1_RESEL_CONTINUE:
                                  {
                                    rr_resel_send_cell_acq_start_ind(
                                      RR_CELL_ACQ_ACTION_SELECT_BCCH,
                                      RR_RESEL_CLEAR_SI_DB,
                                      gas_id
                                    );

                                    rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_CELL_ACQ;
                                    break;
                                  } /* START_C1_RESEL_CONTINUE */

                                  default:
                                  {
                                    MSG_GERAN_ERROR_1_G("RR Resel Invalid start_c1: 0x%x",rps_resel_status);
                                  }
                                }
                              }
                            }
                            break;
                          } /* RR_PSCAN_OMSG_FINISHED_IND */

                          default:
                          {
                            LOG_UNEXPECTED_MESSAGE(
                              message,
                              rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                            );
                          }
                        }
                        break;
                      } /* RR_PSCAN_IMSG */

                      case RR_RESEL_IMSG:
                      {
                        switch (message->rr.resel.imsg.header.imh.message_id)
                        {
                          case RR_RESEL_IMSG_ABORT_REQ:
                          {
                            MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                            switch (message->rr.resel.imsg.abort_req.reason)
                            {
                              case RR_RESEL_ABORT_REASON_MM_REQ:
                              {
                                /* abort power scan and moves to RPS ABORTED state and wait for ABORT CNF there */
                                rr_pscan_send_abort_req(gas_id);

                                rr_resel_data_ptr->rps_data.reason_for_rps_aborted = GRR_CONTROL_ABORTED;

                                rr_resel_data_ptr->state = RR_RESEL_STATE_RPS_ABORTED;

                                rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                              }
                              break;

                              case RR_RESEL_ABORT_REASON_NACC_NC2:
                              case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                              case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                              {
                                MSG_GERAN_HIGH_1_G("Ignore abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                              }
                              break;

                              default:
                              {
                                MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                              }
                              break;
                            }

                            break;
                          }

                          default:
                          {
                            LOG_UNEXPECTED_MESSAGE(
                              message,
                              rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                            );
                          }
                        }
                        break;
                      } /* RR_RESEL_IMSG */


                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                        );
                      }
                    }
                    break;
                  } /*MS_RR_RR*/

                  case MS_RR_L1:
                  {
                    rr_pscan_control(event, message, gas_id);
                    break;
                  } /*MS_RR_L1*/

#ifdef FEATURE_DUAL_SIM
                  case MS_TIMER:
                  {
                    if (message_id == (byte)MID_TIMER_EXPIRY)
                    {
                      timer_expiry_T *timer_expiry = (timer_expiry_T *)message;

                      if (timer_expiry->timer_id == CELL_RESELECTION_TIMER)
                      {
                        MSG_GERAN_HIGH_0_G("CELL_RESELECTION_TIMER expired during power scan");

                        rr_pscan_send_abort_req(gas_id);

                        rr_resel_data_ptr->rps_data.reason_for_rps_aborted = RR_TIMER_EXPIRY;

                        rr_resel_data_ptr->state = RR_RESEL_STATE_RPS_ABORTED;

                        rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                      }
                    }
                    break;
                  }
#endif /* FEATURE_DUAL_SIM */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                     message,
                     rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                    );
                  }
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_EVENT(rr_event_name(event),
                                     rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state));
              }
            }
            break;
          }

          case RR_RESEL_RPS_DECODE_BCCH:
          {
            switch ( event )
            {
              case EV_INPUT_MESSAGE:
              {
                if (message == NULL)
                {
                  MSG_GERAN_ERROR_0_G("message == NULL");
                  break;
                }

                switch (message_set)
                {
                  case MS_RR_RR:
                  {
                    switch (message->rr.header.rr_message_set)
                    {
                      case RR_DECODE_BCCH_IMSG:
                      {
                        rr_decode_bcch_list_control(EV_INPUT_MESSAGE, message, gas_id);
                        break;
                      }

                      case RR_DECODE_BCCH_OMSG:
                      {
                        switch (message->rr.decode_bcch.omsg.header.imh.message_id)
                        {
                          case RR_DECODE_BCCH_OMSG_FINISHED_IND:
                          {
                            rr_resel_start_c1_status_e rps_resel_status = START_C1_RESEL_FAILED;

                            MSG_GERAN_HIGH_0_G("RR_DECODE_BCCH_OMSG_FINISHED_IND rxd");

                            /* All the required ncells have been processed by L1 */
                            rr_gprs_generate_rps_measurement_report(
                              rr_get_candidate_cell_db_ptr(gas_id),
                              gas_id
                            );

                            /* find target cell and if there is a target cell then send cell acq start ind here
                               if there is no target cell then do the next set of bcch decode */

                            rps_resel_status = rr_resel_start_c1(TRUE, FALSE, FALSE, gas_id);

                            switch (rps_resel_status)
                            {
                              case START_C1_RESEL_FAILED:
                                if (rr_gprs_any_more_rps_cells_to_try(
                                      &rr_resel_data_ptr->rps_data.pscan_cell_list,
                                      gas_id
                                    ))
                                {
                                  MSG_GERAN_HIGH_0_G("RPS LOOP again");
								  
                                  if (!rr_timer_is_running(CELL_RESELECTION_TIMER, gas_id))
                                  {
                                    if (rr_timer_start(CELL_RESELECTION_TIMER, CELL_RESELECTION_TIMEOUT, gas_id))
                                    {
                                      MSG_GERAN_MED_0_G("Restarted the CELL_RESELECTION_TIMER for RPS LOOP");
                                    }
                                  }							  
                                  /* send pscan finished ind again internally then RPS loop will start again*/
                                  rr_pscan_send_finish_ind(FALSE, gas_id);
                                  rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;
                                }
                                else
                                {
                                  rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                                  /* cancel any remaining timers */
                                  rr_gprs_cancel_all_five_second_timers(gas_id);

                                  /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                                  rr_ps_no_ps_access_del_all(gas_id);

                                  /*Reinitialise cell reselection */
                                  rr_resel_init(gas_id);

                                  rr_log_reselect_failure(
                                    RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT,
                                    gas_id
                                  );

                                  rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                                  rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                                }
                                break;

                              case START_C1_RESEL_CONTINUE:

                                rr_resel_send_cell_acq_start_ind(
                                  RR_CELL_ACQ_ACTION_SELECT_BCCH,
                                  RR_RESEL_CLEAR_SI_DB,
                                  gas_id
                                );

                                rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_CELL_ACQ;
                              break;

                              default:
                                MSG_GERAN_ERROR_1_G("RR Resel Invalid start_c1: 0x%x",rps_resel_status);
                              break;
                            }
                            break;
                          }

                          default:
                          {
                            LOG_UNEXPECTED_MESSAGE(
                              message,
                              rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                            );
                          }
                        }
                        break;
                      } /* RR_DECODE_BCCH_IMSG */

                      case RR_RESEL_IMSG:
                      {
                        switch (message->rr.resel.imsg.header.imh.message_id)
                        {
                          case RR_RESEL_IMSG_ABORT_REQ:
                          {
                            MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                            switch (message->rr.resel.imsg.abort_req.reason)
                            {
                              case RR_RESEL_ABORT_REASON_MM_REQ:
                              {
                                /* abort decode bcch and moves to RPS ABORTED state and wait for ABORT CNF there */
                                rr_decode_bcch_send_abort_req(gas_id);

                                rr_resel_data_ptr->rps_data.reason_for_rps_aborted = GRR_CONTROL_ABORTED;

                                rr_resel_data_ptr->state = RR_RESEL_STATE_RPS_ABORTED;

                                rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                              }
                              break;

                              case RR_RESEL_ABORT_REASON_NACC_NC2:
                              case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                              case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                              {
                                MSG_GERAN_HIGH_1_G("Ignore abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                              }
                              break;

                              default:
                              {
                                MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                              }
                              break;
                            }

                            break;
                          }

                          default:
                          {
                            LOG_UNEXPECTED_MESSAGE(
                              message,
                              rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                            );
                          }
                        }
                        break;
                      } /* RR_RESEL_IMSG */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                        );
                      }
                    }
                    break;
                  } /*MS_RR_RR*/

                  case MS_RR_L1:
                  {
                    rr_decode_bcch_list_control(event, message, gas_id);
                    break;
                  } /*MS_RR_L1*/

                  case MS_TIMER:
                  {
                    switch (message_id)
                    {
                      case MID_TIMER_EXPIRY:
                      {
                        timer_expiry_T *timer_expiry = (timer_expiry_T *) message;
                        if(timer_expiry->timer_id == RR_BCCH_DECODE_COMPLETE_TIMER)
                        {
                          MSG_GERAN_HIGH_0_G("RR_BCCH_DECODE_COMPLETE_TIMER rxd");
                          rr_decode_bcch_list_control(event, message, gas_id);
                        }
                        else if(timer_expiry->timer_id == CELL_RESELECTION_TIMER)
                        {
                          MSG_GERAN_HIGH_0_G("CELL_RESELECTION_TIMER rxd");
                          /* abort decode bcch and moves to RPS ABORTED state and wait for ABORT CNF there */
                          rr_decode_bcch_send_abort_req(gas_id);

                          rr_resel_data_ptr->rps_data.reason_for_rps_aborted = RR_TIMER_EXPIRY;

                          rr_resel_data_ptr->state = RR_RESEL_STATE_RPS_ABORTED;

                          rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                        }
                        else
                        {
                          MSG_GERAN_ERROR_1_G("Unexpected timer expiry, timer_id=%d",timer_expiry->timer_id);
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state));
                      }
                    }

                    break;
                  } /*MS_TIMER_L1*/

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                     message,
                     rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                    );
                  }
                }
                break;
              }

              case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
              {
                // rr_decode_bcch_list_control has already been notified
                // see rr_store_system_info [rr_sys_info.c]
                break;
              }

              case RR_EV_DECODE_BCCH_COMPLETE_TIMEOUT:
              {
                rr_decode_bcch_list_control(event, message, gas_id);
                break;
              }

              default:
              {
                LOG_UNEXPECTED_EVENT(rr_event_name(event),
                                     rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state));
              }
            }
            break;
          }

          case RR_RESEL_RPS_CELL_ACQ:
          {
            switch (event)
            {
              case EV_INPUT_MESSAGE:
              {
                if (message == NULL)
                {
                  MSG_GERAN_ERROR_0_G("message == NULL");
                  break;
                }

                switch (message_set)
                {
                  case MS_RR_RR:
                  {
                    switch (message->rr.header.rr_message_set)
                    {
                      case RR_CELL_ACQ_IMSG:
                      case RR_PS_IMSG:
                      case RR_PS_OMSG:
                      case RR_L1_IDLE_IMSG:
                      case RR_L1_IDLE_OMSG:
                      {
                        rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                        break;
                      } /* RR_CELL_ACQ_IMSG ... */

                      case RR_CELL_ACQ_OMSG:
                      {
                        switch (message->rr.cell_acq.omsg.header.imh.message_id)
                        {
                          case RR_CELL_ACQ_OMSG_FINISHED_IND:
                          {
                            MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_OMSG_FINISHED_IND status:0x%x rxd",message->rr.cell_acq.omsg.finished_ind.cell_acq_status);

                            if (rr_timer_cancel(CELL_RESELECTION_TIMER, gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("Stopped the CELL_RESELECTION_TIMER");
                            }

                            switch(message->rr.cell_acq.omsg.finished_ind.cell_acq_status)
                            {
                              case RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED:
                              case RR_CELL_ACQ_STATUS_SUCCESS_BCCH:
                              {
                                grr_reset_reselection_power_scan(gas_id);
                                rr_gprs_cell_reselection_complete(RESEL_COMPLETE_C1, gas_id);
                                rr_l1_idle_send_imsg_start_idle_req(gas_id);

                                rr_resel_data_ptr->state = RR_RESEL_STATE_RPS_PENDING_L1_IDLE_CNF;
                                rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                                break;
                              } /* RR_CELL_ACQ_STATUS_SUCCESS_BCCH */

                              default:
                              {
                                rr_resel_start_c1_status_e rps_resel_status = START_C1_RESEL_FAILED;
                                /* find next target cell and if there is a target cell then send cell acq start ind here
                                if there is no target cell then do the next set of bcch decode */

                                rps_resel_status = rr_resel_start_c1(FALSE, TRUE, FALSE, gas_id);

                                switch (rps_resel_status)
                                {
                                  case START_C1_RESEL_FAILED:
                                  {
                                    if (rr_gprs_any_more_rps_cells_to_try(
                                          &rr_resel_data_ptr->rps_data.pscan_cell_list,
                                          gas_id
                                        ))
                                    {
                                      MSG_GERAN_HIGH_0_G("RPS LOOP again");
                                      if (!rr_timer_is_running(CELL_RESELECTION_TIMER, gas_id))
                                      {
                                        if (rr_timer_start(CELL_RESELECTION_TIMER, CELL_RESELECTION_TIMEOUT, gas_id))
                                        {
                                          MSG_GERAN_MED_0_G("Restarted the CELL_RESELECTION_TIMER for RPS LOOP");
                                        }
                                      }										  
                                      /* send pscan finished ind again internally then RPS loop will start again*/
                                      rr_pscan_send_finish_ind(FALSE, gas_id);
                                      rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;
                                    }
                                    else
                                    {
                                      rr_resel_send_omsg_finished_ind(
                                        RR_RESEL_STATUS_OUT_OF_SERVICE,
                                        gas_id
                                      );

                                      /* cancel any remaining timers */
                                      rr_gprs_cancel_all_five_second_timers(gas_id);

                                      /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                                      rr_ps_no_ps_access_del_all(gas_id);

                                      /*Reinitialise cell reselection */
                                      rr_resel_init(gas_id);

                                      rr_log_reselect_failure(
                                        RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT,
                                        gas_id
                                      );

                                      rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                                      rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                                    }
                                    break;
                                  } /* START_C1_RESEL_FAILED */

                                  case START_C1_RESEL_CONTINUE:
                                  {
                                    rr_resel_send_cell_acq_start_ind(
                                      RR_CELL_ACQ_ACTION_SELECT_BCCH,
                                      RR_RESEL_CLEAR_SI_DB,
                                      gas_id
                                    );

                                    if (!rr_timer_is_running(CELL_RESELECTION_TIMER, gas_id))
                                    {
                                      rr_timer_start(CELL_RESELECTION_TIMER, CELL_RESELECTION_TIMEOUT, gas_id);
                                      MSG_GERAN_MED_0_G("Restarted the CELL_RESELECTION_TIMER for cell acq");
                                    }
                                    
                                    rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_CELL_ACQ;
                                    break;
                                  } /* START_C1_RESEL_CONTINUE */

                                  default:
                                  {
                                    MSG_GERAN_ERROR_1_G("RR Resel Invalid start_c1: 0x%x",rps_resel_status);
                                  }
                                }
                                break;
                              } /* default */
                            }
                            break;
                          } /* RR_CELL_ACQ_OMSG_FINISHED_IND */

                          case RR_CELL_ACQ_OMSG_RESELECTED_IND:
                          {
                            MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_RESELECTED_IND rxd");
                            /* ignore this signal */

                            break;
                          } /* RR_CELL_ACQ_OMSG_RESELECTED_IND */

                          case RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND:
                          {
                            MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND rxd");
                            rr_resel_send_omsg_started_ind(RR_RESEL_TYPE_NORMAL, gas_id);
                            rr_gprs_prepare_for_reselection_MFRM51(TRUE, TRUE, gas_id);
                            break;
                          } /* RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND */

                          default:
                          {
                            LOG_UNEXPECTED_MESSAGE(
                              message,
                              rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                            );
                          }
                        }
                        break;
                      } /* RR_CELL_ACQ_OMSG */

                      case RR_RESEL_IMSG:
                      {
                        switch (message->rr.resel.imsg.header.imh.message_id)
                        {
                          case RR_RESEL_IMSG_ABORT_REQ:
                          {
                            MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                            switch (message->rr.resel.imsg.abort_req.reason)
                            {
                              case RR_RESEL_ABORT_REASON_MM_REQ:
                              {
                                /* abort cell acq and moves to RPS ABORTED state and wait for ABORT CNF there */
                                rr_cell_acq_send_imsg_abort_req(gas_id);

                                rr_resel_data_ptr->rps_data.reason_for_rps_aborted = GRR_CONTROL_ABORTED;

                                /*Stop the reselection timer*/
                                if (rr_timer_cancel(CELL_RESELECTION_TIMER, gas_id))
                                {
                                  MSG_GERAN_HIGH_0_G("RESELECTION TIMER STOPPED");
                                }
                                rr_resel_data_ptr->state = RR_RESEL_STATE_RPS_ABORTED;

                                rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;;
                              }
                              break;

                              case RR_RESEL_ABORT_REASON_NACC_NC2:
                              case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                              case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                              {
                                MSG_GERAN_HIGH_1_G("Ignore abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                              }
                              break;

                              default:
                              {
                                MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                              }
                              break;
                            }
                            break;
                          }

                          default:
                          {
                            LOG_UNEXPECTED_MESSAGE(
                              message,
                              rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                            );
                          }
                        }
                        break;
                      } /* RR_RESEL_IMSG */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                        );
                      }
                    }
                    break;
                  } /*MS_RR_RR*/

                  case MS_RR_L1:
                  {
                    rr_cell_acq_control(event, message, gas_id);
                    break;
                  } /*MS_RR_L1*/

                  case MS_TIMER:
                  {
                    switch (message_id)
                    {
                      case MID_TIMER_EXPIRY:
                      {
                        timer_expiry_T *timer_expiry = (timer_expiry_T *) message;

                        switch (timer_expiry->timer_id)
                        {
                          case RR_GPRS_SYS_INFO_TIMER:
                          {
                            rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                            break;
                          } /* RR_GPRS_SYS_INFO_TIMER */

                          case CELL_RESELECTION_TIMER:
                          {
                            MSG_GERAN_HIGH_0_G("CELL_RESELECTION_TIMER rxd");

                            /* abort cell acq and moves to RPS ABORTED state and wait for ABORT CNF there */
                            rr_cell_acq_send_imsg_abort_req(gas_id);

                            rr_resel_data_ptr->rps_data.reason_for_rps_aborted = RR_TIMER_EXPIRY;
                            rr_resel_data_ptr->state = RR_RESEL_STATE_RPS_ABORTED;
                            rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                            break;
                          } /* CELL_RESELECTION_TIMER */

                          default:
                          {
                            LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                          }
                        }
                        break;
                      } /* MID_TIMER_EXPIRY */

                      default:
                      {
                        LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state));
                      }
                    }
                    break;
                  } /* MS_TIMER */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                     rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                    );
                  }
                }
                break;
              } /* EV_INPUT_MESSAGE */

              case EV_MAC_IDLE_MODE:
              case EV_MAC_GUARD_TIMER_EXPIRED:
              {
                rr_cell_acq_control(event, message, gas_id);
                break;
              } /* EV_MAC_IDLE_MODE ... */

              default:
              {
                LOG_UNEXPECTED_EVENT(rr_event_name(event),
                                     rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state));
              }
            }
            break;
          } /* RR_RESEL_RPS_CELL_ACQ */

          default:
          {
            MSG_GERAN_ERROR_1_G("Unexpected state: %d", rr_resel_data_ptr->rps_data.rps_sub_state);
          }
        }
        break;
      } /* RR_RESEL_STATE_RPS */

      /**
      * RR_RESEL_STATE_RPS_ABORTED
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_RPS_ABORTED:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_DECODE_BCCH_IMSG:
                  {
                    rr_decode_bcch_list_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_DECODE_BCCH_IMSG */

                  case RR_PSCAN_IMSG:
                  {
                    rr_pscan_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_PSCAN_IMSG */

                  case RR_CELL_ACQ_IMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.cell_acq.omsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_OMSG_FINISHED_IND:
                      case RR_CELL_ACQ_OMSG_ABORT_CNF:
                      {
                        if ((rr_cell_acq_omsg_e) message->rr.cell_acq.omsg.header.imh.message_id == RR_CELL_ACQ_OMSG_FINISHED_IND)
                        {
                          MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_OMSG_FINISHED_IND status:0x%x rxd",message->rr.cell_acq.omsg.finished_ind.cell_acq_status);
                        }
                        else if ((rr_cell_acq_omsg_e) message->rr.cell_acq.omsg.header.imh.message_id == RR_CELL_ACQ_OMSG_ABORT_CNF)
                        {
                          MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_ABORT_CNF rxd");
                        }

                        /* cancel any remaining timers */
                        rr_gprs_cancel_all_five_second_timers(gas_id);

                        /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                        rr_ps_no_ps_access_del_all(gas_id);

                        if (rr_resel_data_ptr->rps_data.reason_for_rps_aborted == GRR_CONTROL_ABORTED)
                        {
                          rr_resel_send_omsg_abort_cnf(gas_id);
                        }
                        else
                        {
                          rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);
                        }

                        rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                        /* Reinitialise cell reselection. */
                        rr_resel_init(gas_id);

                        rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                        rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        break;
                      } /* RR_CELL_ACQ_OMSG_FINISHED_IND ... */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_OMSG */

                  case RR_PSCAN_OMSG:
                  {
                    switch (message->rr.pscan.omsg.header.imh.message_id)
                    {
                      case RR_PSCAN_OMSG_FINISHED_IND:
                      case RR_PSCAN_OMSG_ABORT_CNF:
                      {
                        if ((rr_pscan_omsg_e) message->rr.pscan.omsg.header.imh.message_id == RR_PSCAN_OMSG_FINISHED_IND)
                        {
                          MSG_GERAN_HIGH_0_G("RR_PSCAN_OMSG_FINISHED_IND rxd");
                        }
                        else if ((rr_pscan_omsg_e) message->rr.pscan.omsg.header.imh.message_id == RR_PSCAN_OMSG_ABORT_CNF)
                        {
                          MSG_GERAN_HIGH_0_G("RR_PSCAN_OMSG_ABORT_CNF rxd");
                        }

                        /* cancel any remaining timers */
                        rr_gprs_cancel_all_five_second_timers(gas_id);

                        /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                        rr_ps_no_ps_access_del_all(gas_id);

                        if (rr_resel_data_ptr->rps_data.reason_for_rps_aborted == GRR_CONTROL_ABORTED)
                        {
                          rr_resel_send_omsg_abort_cnf(gas_id);
                        }
                        else
                        {
                          rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);
                        }

                        rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                        /*Reinitialise cell reselection */
                        rr_resel_init(gas_id);

                        rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                        rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        break;
                      } /* RR_PSCAN_OMSG_FINISHED_IND ... */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_PSCAN_OMSG */

                  case RR_DECODE_BCCH_OMSG:
                  {
                    switch (message->rr.decode_bcch.omsg.header.imh.message_id)
                    {
                      case RR_DECODE_BCCH_OMSG_FINISHED_IND:
                      case RR_DECODE_BCCH_OMSG_ABORT_CNF:
                      {
                        if ((rr_decode_bcch_omsg_e) message->rr.decode_bcch.omsg.header.imh.message_id == RR_DECODE_BCCH_OMSG_FINISHED_IND)
                        {
                          MSG_GERAN_HIGH_0_G("RR_DECODE_BCCH_OMSG_FINISHED_IND rxd");
                        }
                        else if ((rr_decode_bcch_omsg_e) message->rr.decode_bcch.omsg.header.imh.message_id == RR_DECODE_BCCH_OMSG_ABORT_CNF)
                        {
                          MSG_GERAN_HIGH_0_G("RR_DECODE_BCCH_OMSG_ABORT_CNF rxd");
                        }

                        /* cancel any remaining timers */
                        rr_gprs_cancel_all_five_second_timers(gas_id);

                        /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                        rr_ps_no_ps_access_del_all(gas_id);

                        if (rr_resel_data_ptr->rps_data.reason_for_rps_aborted == GRR_CONTROL_ABORTED)
                        {
                          rr_resel_send_omsg_abort_cnf(gas_id);
                        }
                        else
                        {
                          rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);
                        }

                        rr_log_reselect_failure(RR_LOG_RES_FAILURE_CELL_SELECTION, gas_id);

                        /* Reinitialise cell reselection */
                        rr_resel_init(gas_id);

                        rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_IDLE;
                        rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        break;
                      } /* RR_DECODE_BCCH_OMSG_FINISHED_IND ... */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_DECODE_BCCH_OMSG */

                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                        switch (message->rr.resel.imsg.abort_req.reason)
                        {
                          case RR_RESEL_ABORT_REASON_MM_REQ:
                          {
                            rr_resel_data_ptr->rps_data.reason_for_rps_aborted = GRR_CONTROL_ABORTED;
                            break;
                          } /* RR_RESEL_ABORT_REASON_MM_REQ */

                          case RR_RESEL_ABORT_REASON_NACC_NC2:
                          case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                          case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                          {
                            MSG_GERAN_HIGH_1_G("Ignore abort type: 0x%x", message->rr.resel.imsg.abort_req.reason);
                            break;
                          } /* RR_RESEL_ABORT_REASON_NACC_NC2 ... */

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                        }
                        break;
                      } /* RR_RESEL_IMSG_ABORT_REQ */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                        );
                      }
                    }
                    break;
                  } /* RR_RESEL_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                    );
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_POWER_SCAN_CNF:
                  {
                    rr_pscan_control(event, message, gas_id);
                    break;
                  } /* MPH_POWER_SCAN_CNF */

                  default:
                  {
                    rr_decode_bcch_list_control(event, message, gas_id);
                  }
                }
                break;
              } /* MS_RR_L1 */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                );
                break;
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_MAC_IDLE_MODE:
          case EV_MAC_GUARD_TIMER_EXPIRED:
          {
            rr_cell_acq_control(event, NULL, gas_id);
            break;
          } /* EV_MAC_IDLE_MODE ... */

          default:
          {
            LOG_UNEXPECTED_EVENT(
              rr_event_name(event),
              rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
            );
          }
        }
        break;
      } /* RR_RESEL_STATE_RPS_ABORTED */

      /**
      * RR_RESEL_STATE_RPS_PENDING_L1_IDLE_CNF
      *
      *
      * In this state:
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      *
      **/
      case RR_RESEL_STATE_RPS_PENDING_L1_IDLE_CNF:
      {
        if ((event == EV_INPUT_MESSAGE) && (message != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (message->rr.header.rr_message_set)
            {
              case RR_RESEL_IMSG:
              {
                switch (message->rr.resel.imsg.header.imh.message_id)
                {
                  case RR_RESEL_IMSG_ABORT_REQ:
                  {
                    MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                    switch (message->rr.resel.imsg.abort_req.reason)
                    {
                      case RR_RESEL_ABORT_REASON_MM_REQ:
                      {
                        rr_resel_data_ptr->rps_data.reason_for_rps_aborted = GRR_CONTROL_ABORTED;
                      }
                      break;

                      case RR_RESEL_ABORT_REASON_NACC_NC2:
                      case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                      case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                      {
                        MSG_GERAN_HIGH_1_G("Ignore abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                      }
                      break;

                      default:
                      {
                        MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                      }
                      break;
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                    );
                  }
                }
                break;
              }

              case RR_L1_IDLE_IMSG:
              {
                rr_l1_idle_control(message, gas_id);
                break;
              } /* RR_L1_IDLE_IMSG */

              case RR_L1_IDLE_OMSG:
              {
                switch (message->rr.l1_idle.omsg.header.imh.message_id)
                {
                  case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                  {
                    if (rr_resel_data_ptr->rps_data.reason_for_rps_aborted == NO_ABORT)
                    {
                      rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_IN_SERVICE, gas_id);
                      rr_log_reselect_success(
                        rr_gprs_get_scell_arfcn(gas_id),
                        rr_gprs_get_scell_bsic(gas_id),
                        gas_id
                      );
                    }
                    else if (rr_resel_data_ptr->rps_data.reason_for_rps_aborted == GRR_CONTROL_ABORTED)
                    {
                      /* cancel any remaining timers */
                      rr_gprs_cancel_all_five_second_timers(gas_id);
                      /* Reinitialise cell reselection */
                      rr_resel_init(gas_id);

                      rr_resel_send_omsg_abort_cnf(gas_id);
                      rr_log_reselect_failure(RR_LOG_RES_FAILURE_CELL_SELECTION, gas_id);
                    }

                    rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                    );
                  }
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
                );
              }
            }
          }

        }
      }
      break;

      case RR_RESEL_STATE_PCCO:
      {
        switch ( event )
        {
          case EV_ACTION_PCCO:
          case EV_ACTION_IMMEDIATE_PCCO:
          {
            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

            if (event == EV_ACTION_PCCO)
            {
              MSG_GERAN_HIGH_0_G("EV_ACTION_PCCO rxd");
            }
            else if (event == EV_ACTION_IMMEDIATE_PCCO)
            {
              MSG_GERAN_HIGH_0_G("EV_ACTION_IMMEDIATE_PCCO rxd");
            }

            rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_PCCO, gas_id);

            MSG_GERAN_HIGH_0_G("Deactivating timed sys info refresh...");
            rr_gprs_deactivate_timed_sys_info_refresh(gas_id);

            /*Start the PCCO timer*/
            MSG_GERAN_HIGH_1_G("Starting T3174 (%d ms)", T3174_TIMEOUT);
            (void)rr_timer_start(T3174, T3174_TIMEOUT, gas_id);

            /* hard coding this value so the cell acq starts selecting this cell*/
            rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.RXLEV_average = 63;

            rr_resel_send_cell_acq_start_ind(
              RR_CELL_ACQ_ACTION_RESELECT_BCCH,
              RR_RESEL_CLEAR_SI_DB,
              gas_id
            );

            /* In PCCO case always set camped on flag to false from the starting*/
            scell_info_ptr->gsm.camped_on_flag = FALSE;

            rr_resel_data_ptr->pcco_data.pcco_reason = PCCO_TRIGGERED_THROUGH_NC2;
            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;
            break;
          }

          case EV_ACTION_PCCO_WITH_PNCD:
          {
            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

            MSG_GERAN_HIGH_0_G("EV_ACTION_PCCO_WITH_PNCD rxd");

            /* No set the camped on flag to FALSE - setting this prior to this
            may mean an unwanted pbcch_description_ind gets sent to L1 when collecting
            a PNCD PSI1 */
            scell_info_ptr->gsm.camped_on_flag = FALSE;

            /* Inform the NACC state tracking module that a NACC PCCO is
            in progress. */
            rr_gprs_nacc_notify_event(CCN_RESPONSE_FROM_NW, gas_id);

            MSG_GERAN_HIGH_0_G("Deactivating timed sys info refresh...");
            rr_gprs_deactivate_timed_sys_info_refresh(gas_id);

            rr_gprs_prepare_for_reselection_with_PNCD(INVALID_CONTAINER_ID, gas_id);

            /*Start the PCCO timer*/
            MSG_GERAN_HIGH_1_G("Starting T3174 (%d ms)", T3174_TIMEOUT);
            (void)rr_timer_start(T3174, T3174_TIMEOUT, gas_id);

            rr_resel_data_ptr->pcco_data.pcco_reason = PCCO_TRIGGERED_THROUGH_NACC;
            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;

            break;
          }

          case EV_ALL_NACC_PNCD_DISPATCHED:
          {
            gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

            MSG_GERAN_HIGH_0_G("EV_ALL_NACC_PNCD_DISPATCHED rxd");

            /* set the valid data flag to true, RR is ready to camp on this cell */
            pcell_info_ptr->gsm.valid_data_flag = TRUE;

            /* determine whether we have enough ncell data to camp on */
            if (!rr_gprs_is_pncd_enough_to_camp(gas_id))
            {
              /* All reselections must be CCCH based at this point, as */
              /* there is insufficient data for a transition to PCCCH  */
              MSG_GERAN_MED_0_G("NACC: insufficient neighbour cell data");

              /* reset the utility module (PNCD storage etc.) */
              rr_gprs_nacc_reset(gas_id);
            }

            /* hard coding this value so the cell acq starts selecting this cell*/
            rr_resel_data_ptr->rr_gprs_target_cell->reselection_data.RXLEV_average = 63;

            rr_log_reselect_start(RR_RESELECTION_TRIGGER_PCCO, gas_id);

            rr_resel_send_cell_acq_start_ind(
              RR_CELL_ACQ_ACTION_RESELECT_BCCH_NACC_PCCO_PCCC,
              RR_RESEL_DONT_CLEAR_SI_DB,
              gas_id
            );

            break;
          }

          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  case RR_L1_IDLE_IMSG:
                  case RR_L1_IDLE_OMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.cell_acq.omsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_OMSG_FINISHED_IND:
                      {
                        MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_OMSG_FINISHED_IND status:0x%x rxd",message->rr.cell_acq.omsg.finished_ind.cell_acq_status);

                        switch(message->rr.cell_acq.omsg.finished_ind.cell_acq_status)
                        {
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED:
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH:
                          {
                            /* now restart the recent reselection timer again from the start */
                            (void) rr_timer_start(
                              RESELECTION_HYSTERESIS,
                              RESELECTION_HYSTERESIS_TIMEOUT,
                              gas_id
                            );

                            MSG_GERAN_HIGH_0_G("Start the RECENT RESELECTION TIMER");

#ifdef FEATURE_IDLE_DRX_SCALING
                            if (rr_idle_drx_start_scaling_suspend_timer(gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("Started the IDLE_DRX_SCALING_SUSPEND TIMER");
                            }
#endif /* FEATURE_IDLE_DRX_SCALING */

                            /* set up the bad snr counter value */
                            rr_gprs_init_bad_snr_counter(gas_id);

                            rr_resel_data_ptr->recent_reselection_penalty_flag = TRUE;

                            /*Clean out all measurements in the reselection list*/
                            rr_gprs_reset_reselection_list(gas_id);

                            rr_gprs_nacc_reset(gas_id);

                            rr_l1_idle_send_imsg_start_idle_req(gas_id);

                            rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_PENDING_L1_IDLE_CNF;
                            break;
                          } /* RR_CELL_ACQ_STATUS_SUCCESS_BCCH */

                          default:
                          {
                            rr_log_reselect_failure(RR_LOG_RES_FAILURE_T3174_TIMEOUT, gas_id);

                            if (rr_timer_is_running(T3174, gas_id))
                            {
                              (void)rr_timer_cancel(T3174, gas_id);

                              MSG_GERAN_HIGH_1_G("SI ACQ failed, starting T3176 in state %s", rr_resel_state_name(rr_resel_data_ptr->state));
                              (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);

                              rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_PCCO_FAILED, gas_id);

                              rr_reselect_old_scell_after_pcco_failure(rr_resel_data_ptr, gas_id);

                              rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;
                              rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_FAILED;

                              rr_resel_send_cell_acq_start_ind(
                                RR_CELL_ACQ_ACTION_RESELECT_BCCH,
                                RR_RESEL_CLEAR_SI_DB,
                                gas_id
                              );
                            }
                            else
                            {
                              grr_perform_reselection_power_scan(gas_id);

                              if (rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries != 0)
                              {
                                rr_perform_rps(&rr_resel_data_ptr->rps_data.pscan_cell_list, gas_id);

                                rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                                rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;

                              }
                              else
                              {
                                /* cancel any remaining timers */
                                rr_gprs_cancel_all_five_second_timers(gas_id);

                                /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                                rr_ps_no_ps_access_del_all(gas_id);

                                /* Reinitialise cell reselection */
                                rr_resel_init(gas_id);
                                rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);
                                rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                              }
                            }
                          } /* default */
                        }
                        break;
                      } /* RR_CELL_ACQ_OMSG_FINISHED_IND */

                      case RR_CELL_ACQ_OMSG_RESELECTED_IND:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_RESELECTED_IND rxd");

                        if (rr_resel_data_ptr->pcco_data.pcco_reason == PCCO_TRIGGERED_THROUGH_NACC)
                        {
                          if (rr_gprs_is_pncd_enough_to_camp(gas_id))
                          {
                            /* send nacc complete ind here then cell acq control sends back finished ind*/
                            rr_cell_acq_send_imsg_nacc_acq_complete_ind(gas_id);
                          }
                        }
                        break;
                      } /* RR_CELL_ACQ_OMSG_RESELECTED_IND */

                      case RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND rxd");

                        rr_resel_send_omsg_started_ind(RR_RESEL_TYPE_NORMAL, gas_id);
                        if (rr_resel_data_ptr->pcco_data.pcco_reason == PCCO_TRIGGERED_THROUGH_NC2)
                        {
                          rr_gprs_prepare_for_reselection_MFRM51(TRUE, FALSE, gas_id);
                        }
                        break;
                      } /* RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_OMSG */

                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                        switch (message->rr.resel.imsg.abort_req.reason)
                        {
                          case RR_RESEL_ABORT_REASON_MM_REQ:
                          {
                            /* abort cell acq and moves to PCCO ABORTED state and wait for ABORT CNF there */
                            rr_cell_acq_send_imsg_abort_req(gas_id);

                            /*Stop the reselection timer*/
                            if (rr_timer_cancel(CELL_RESELECTION_TIMER, gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("RESELECTION TIMER STOPPED");
                            }

                            /*Stop the reselection timer*/
                            if (rr_timer_cancel(T3174, gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("T3174 TIMER STOPPED");
                            }

                            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_GRR_CONTROL_ABORTED;

                            rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_ABORTING_CELL_ACQ;
                          }
                          break;

                          case RR_RESEL_ABORT_REASON_NACC_NC2:
                          case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                          case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                          {
                            MSG_GERAN_HIGH_1_G("Ignore abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                          break;

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                          break;
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                            message,
                            rr_resel_state_name(rr_resel_data_ptr->state)
                          );
                      }
                    }
                    break;
                  } /* RR_RESEL_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_resel_state_name(rr_resel_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_RR_L1:
              {
                rr_cell_acq_control(event, message, gas_id);
                break;
              } /* MS_RR_L1 */


              case MS_TIMER:
              {
                switch (message_id)
                {
                  case MID_TIMER_EXPIRY:
                  {
                    timer_expiry_T *timer_expiry = (timer_expiry_T *) message;

                    switch (timer_expiry->timer_id)
                    {
                      case RR_GPRS_SYS_INFO_TIMER:
                      {
                        /* Ignore this timer during a PCCO procedure - T3174 is
                        used to guard against acquisition failures in this
                        case. */
                        break;
                      } /* RR_GPRS_SYS_INFO_TIMER */

                      case CELL_RESELECTION_TIMER:
                      {
                        MSG_GERAN_HIGH_0_G("CELL_RESELECTION_TIMER rxd");

                        /* abort cell acq and moves to RPS ABORTED state and wait for ABORT CNF there */
                        rr_cell_acq_send_imsg_abort_req(gas_id);

                        rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_CELL_RESELECTION_TIMER_EXPIRY;

                        rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_ABORTING_CELL_ACQ;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      }
                    }
                    break;
                  } /* MID_TIMER_EXPIRY */

                  default:
                  {
                    LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_resel_state_name(rr_resel_data_ptr->state));
                  }
                }
                break;
              } /* MS_TIMER_L1 */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_resel_state_name(rr_resel_data_ptr->state)
                );
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_T3174_TIMEOUT:
          {
            MSG_GERAN_HIGH_0_G("EV_T3174_TIMEOUT rxd");

            rr_log_reselect_failure(RR_LOG_RES_FAILURE_T3174_TIMEOUT, gas_id);

            MSG_GERAN_HIGH_1_G("T3174 expired, starting T3176 in state %s", rr_resel_state_name(rr_resel_data_ptr->state));
            (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);

            rr_cell_acq_send_imsg_abort_req(gas_id);

            rr_resel_data_ptr->pcco_data.pccf.cause = (uint8) NO_RESPONSE_ON_TARGET;

            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_T3174_TIMER_EXPIRY;

            rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_ABORTING_CELL_ACQ;
            break;
          } /* EV_T3174_TIMEOUT */

          case EV_MAC_IDLE_MODE:
          {
            break;  // ignore - wait for EV_PS_ACCESS_BLOCKED
          }

          case EV_PS_ACCESS_BLOCKED:
          {
            rr_cell_acq_control(EV_MAC_IDLE_MODE, NULL, gas_id);
            break;
          }

          case EV_MAC_GUARD_TIMER_EXPIRED:
          {
            rr_cell_acq_control(event, message, gas_id);
            break;
          } /* EV_MAC_IDLE_MODE ... */

          case EV_GMM_LEFT_READY_STATE:
          {
            MSG_GERAN_MED_0_G("RR_RESEL_STATE_PCCO: GMM has left READY state, continue with NC0 reselection");

            (void)rr_timer_cancel(T3174, gas_id);
            (void)rr_timer_start(CELL_RESELECTION_TIMER, CELL_RESELECTION_TIMEOUT, gas_id);
            break;
          } /* EV_GMM_LEFT_READY_STATE */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event),
                                 rr_resel_state_name(rr_resel_data_ptr->state));
          }
        }
        break;
      } /* RR_RESEL_STATE_PCCO */

      case RR_RESEL_STATE_PCCO_ABORTING_CELL_ACQ:
      {
        switch ( event )
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.cell_acq.omsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_OMSG_ABORT_CNF:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_ABORT_CNF rxd");

                        if(rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted == PCCO_T3174_TIMER_EXPIRY)
                        {
                          rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_PCCO_FAILED, gas_id);

                          rr_reselect_old_scell_after_pcco_failure(rr_resel_data_ptr, gas_id);

                          rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;

                          rr_resel_send_cell_acq_start_ind(
                            RR_CELL_ACQ_ACTION_RESELECT_BCCH,
                            RR_RESEL_CLEAR_SI_DB,
                            gas_id
                          );

                          rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_FAILED;
                        }
                        else if (rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted == PCCO_GRR_CONTROL_ABORTED)
                        {
                          /* cancel any remaining timers */
                          rr_gprs_cancel_all_five_second_timers(gas_id);

                          /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                          rr_ps_no_ps_access_del_all(gas_id);

                          /* Reinitialise cell reselection */
                          rr_resel_init(gas_id);

                          rr_resel_send_omsg_abort_cnf(gas_id);

                          /* Reselection may not have been started when abort request was received */
                          if (rr_log_reselection_log_complete(gas_id) == 0 )
                          {
                            rr_log_reselect_failure(RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ, gas_id);
                          }

                          rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;
                          rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        }
                        else if (rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted == PCCO_CELL_RESELECTION_TIMER_EXPIRY)
                        {
                          /* cancel any remaining timers */
                          rr_gprs_cancel_all_five_second_timers(gas_id);

                          /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                          rr_ps_no_ps_access_del_all(gas_id);

                          /* Reinitialise cell reselection */
                          rr_resel_init(gas_id);

                          /* reselection timeout and this means MS went to NO SERVICE so send finished ind with out of service*/

                          rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                          /* Reselection may not have been started when abort request was received */
                          if (rr_log_reselection_log_complete(gas_id) == 0)
                          {
                            rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);
                          }

                          rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;
                          rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        }
                        break;
                      } /* RR_CELL_ACQ_OMSG_ABORT_CNF */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_OMSG */

                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                        switch (message->rr.resel.imsg.abort_req.reason)
                        {
                          case RR_RESEL_ABORT_REASON_MM_REQ:
                          {
                            /* already sent ABORT req so wait for abort cnf*/
                            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_GRR_CONTROL_ABORTED;
                            break;
                          } /* RR_RESEL_ABORT_REASON_MM_REQ */

                          case RR_RESEL_ABORT_REASON_NACC_NC2:
                          case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                          case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                          {
                            MSG_GERAN_HIGH_1_G("Ignore abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                            break;
                          } /* RR_RESEL_ABORT_REASON_NACC_NC2 ... */

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                        }
                        break;
                      } /* RR_RESEL_IMSG_ABORT_REQ */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_RESEL_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_resel_state_name(rr_resel_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_RR_L1:
              {
                rr_cell_acq_control(event, message, gas_id);
                break;
              } /* MS_RR_L1 */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_resel_state_name(rr_resel_data_ptr->state)
                );
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_T3174_TIMEOUT:
          {
            MSG_GERAN_HIGH_0_G("EV_T3174_TIMEOUT rxd");

            MSG_GERAN_HIGH_1_G("T3174 expired, starting T3176 in state %s", rr_resel_state_name(rr_resel_data_ptr->state));
            (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);

            rr_resel_data_ptr->pcco_data.pccf.cause = (uint8) NO_RESPONSE_ON_TARGET;

            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_T3174_TIMER_EXPIRY;

            break;
          } /* EV_T3174_TIMEOUT */

          case EV_MAC_IDLE_MODE:
          {
            break;  // ignore - wait for EV_PS_ACCESS_BLOCKED
          }

          case EV_PS_ACCESS_BLOCKED:
          {
            rr_cell_acq_control(EV_MAC_IDLE_MODE, NULL, gas_id);
            break;
          }

          case EV_MAC_GUARD_TIMER_EXPIRED:
          {
            rr_cell_acq_control(event, message, gas_id);
            break;
          } /* EV_MAC_IDLE_MODE ... */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event),
                                 rr_resel_state_name(rr_resel_data_ptr->state));
          }
        }
        break;
      } /* RR_RESEL_STATE_PCCO_ABORTING_CELL_ACQ */

      case RR_RESEL_STATE_PCCO_PENDING_L1_IDLE_CNF:
      {
        switch ( event )
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type: 0x%x rxd",
                                 message->rr.resel.imsg.abort_req.reason);

                        switch (message->rr.resel.imsg.abort_req.reason)
                        {
                          case RR_RESEL_ABORT_REASON_MM_REQ:
                          {
                            /* already sent ABORT req so wait for abort cnf*/

                            MSG_GERAN_HIGH_0_G("Abort received during PCCO, cancelling T3174");
                            (void)rr_timer_cancel(T3174, gas_id);

                            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_GRR_CONTROL_ABORTED;
                          }
                          break;

                          case RR_RESEL_ABORT_REASON_NACC_NC2:
                          case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                          case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                          {
                            MSG_GERAN_HIGH_1_G("Ignore abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                          break;

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                          break;
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_RESEL_IMSG */

                  case RR_L1_IDLE_IMSG:
                  {
                    rr_l1_idle_control(message, gas_id);
                    break;
                  } /* RR_L1_IDLE_IMSG */

                  case RR_L1_IDLE_OMSG:
                  {
                    switch (message->rr.l1_idle.omsg.header.imh.message_id)
                    {
                      case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                      {
                        if(rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted == PCCO_NO_ABORT)
                        {
                          rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_IN_SERVICE, gas_id);
                          rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;

                          rr_log_reselect_success(
                            rr_gprs_get_scell_arfcn(gas_id),
                            rr_gprs_get_scell_bsic(gas_id),
                            gas_id
                          );

                          rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        }
                        else if(rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted == PCCO_GRR_CONTROL_ABORTED)
                        {
                          rr_resel_send_omsg_abort_cnf(gas_id);
                          rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;
                          rr_log_reselect_failure(RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ, gas_id);
                          rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        }
                        else if(rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted == PCCO_T3174_TIMER_EXPIRY)
                        {
                          rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_PCCO_FAILED, gas_id);

                          rr_reselect_old_scell_after_pcco_failure(
                            rr_resel_data_ptr,
                            gas_id
                          );

                          rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;

                          rr_resel_send_cell_acq_start_ind(
                            RR_CELL_ACQ_ACTION_RESELECT_BCCH,
                            RR_RESEL_CLEAR_SI_DB,
                            gas_id
                          );

                          rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_FAILED;
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_resel_state_name(rr_resel_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_resel_state_name(rr_resel_data_ptr->state)
                );
              }
            }
            break;
          }

          case EV_T3174_TIMEOUT:
          {
            MSG_GERAN_HIGH_0_G("EV_T3174_TIMEOUT rxd");

            MSG_GERAN_HIGH_1_G("T3174 expired, starting T3176 in state %s", rr_resel_state_name(rr_resel_data_ptr->state));
            (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);

            rr_resel_data_ptr->pcco_data.pccf.cause = (uint8) NO_RESPONSE_ON_TARGET;

            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_T3174_TIMER_EXPIRY;

            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event),
                                 rr_resel_state_name(rr_resel_data_ptr->state));
          }
        }

        break;
      }

      case RR_RESEL_STATE_PCCO_FAILED:
      {
        switch ( event )
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  case RR_PS_IMSG:
                  case RR_PS_OMSG:
                  case RR_L1_IDLE_IMSG:
                  case RR_L1_IDLE_OMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG ... */

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.cell_acq.omsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_OMSG_FINISHED_IND:
                      {
                        MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_OMSG_FINISHED_IND status:0x%x rxd",message->rr.cell_acq.omsg.finished_ind.cell_acq_status);

                        switch (message->rr.cell_acq.omsg.finished_ind.cell_acq_status)
                        {
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED:
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH:
                          {
                            /* now restart the recent reselection timer again from the start */
                            (void) rr_timer_start(
                              RESELECTION_HYSTERESIS,
                              RESELECTION_HYSTERESIS_TIMEOUT,
                              gas_id
                            );

                            MSG_GERAN_HIGH_0_G("Start the RECENT RESELECTION TIMER");

#ifdef FEATURE_IDLE_DRX_SCALING
                            if (rr_idle_drx_start_scaling_suspend_timer(gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("Started the IDLE_DRX_SCALING_SUSPEND TIMER");
                            }
#endif /* FEATURE_IDLE_DRX_SCALING */

                            /* set up the bad snr counter value */
                            rr_gprs_init_bad_snr_counter(gas_id);

                            rr_resel_data_ptr->recent_reselection_penalty_flag = TRUE;

                            /*Clean out all measurements in the reselection list*/
                            rr_gprs_reset_reselection_list(gas_id);

                            rr_gprs_nacc_reset(gas_id);

                            rr_l1_idle_send_imsg_start_idle_req(gas_id);

                            rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_FAILED_PENDING_L1_IDLE_CNF;
                            break;
                          }

                          default:
                          {
                            if (rr_timer_cancel(T3176, gas_id))
                            {
                              MSG_GERAN_HIGH_1_G("SI ACQ failed, %s", rr_resel_state_name(rr_resel_data_ptr->state));
                            }

                            if (!rr_timer_is_running(CELL_RESELECTION_TIMER, gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("RESELECTION TIMER STARTED");
                              (void) rr_timer_start(
                                CELL_RESELECTION_TIMER,
                                CELL_RESELECTION_TIMEOUT,
                                gas_id
                              );
                            }

                            grr_perform_reselection_power_scan(gas_id);

                            if (rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries != 0)
                            {
                              rr_perform_rps(&rr_resel_data_ptr->rps_data.pscan_cell_list, gas_id);

                              rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                              rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;
                            }
                            else
                            {
                              /* cancel any remaining timers */
                              rr_gprs_cancel_all_five_second_timers(gas_id);

                              /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                              rr_ps_no_ps_access_del_all(gas_id);

                              /*Reinitialise cell reselection */
                              rr_resel_init(gas_id);

                              rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                              rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                              rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                            }
                          } /* default */
                        }
                        break;
                      } /* RR_CELL_ACQ_OMSG_FINISHED_IND */

                      case RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND:
                      {
                        gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND rxd");

                        rr_resel_send_omsg_started_ind(RR_RESEL_TYPE_NORMAL, gas_id);
                        rr_gprs_prepare_for_reselection_MFRM51(TRUE, FALSE, gas_id);
                        pcell_info_ptr->gsm.BSIC = rr_resel_data_ptr->old_scell.BSIC;
                        break;
                      } /* RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND */


                      case RR_CELL_ACQ_OMSG_RESELECTED_IND:
                      {
                        /* ignore this signal */
                        break;
                      } /* RR_CELL_ACQ_OMSG_RESELECTED_IND */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                        message,
                        rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_OMSG */

                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                        switch (message->rr.resel.imsg.abort_req.reason)
                        {
                          case RR_RESEL_ABORT_REASON_MM_REQ:
                          {
                            /* abort cell acq and moves to PCCO FAILED ABORTED state and wait for ABORT CNF there */
                            rr_cell_acq_send_imsg_abort_req(gas_id);

                            /*Stop the reselection timer*/
                            if (rr_timer_cancel(CELL_RESELECTION_TIMER, gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("RESELECTION TIMER STOPPED");
                            }

                            /*Stop the reselection timer*/
                            if (rr_timer_cancel(T3176, gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("T3176 TIMER STOPPED");
                            }

                            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_GRR_CONTROL_ABORTED;

                            rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_FAILED_ABORTING_CELL_ACQ;
                          }
                          break;

                          case RR_RESEL_ABORT_REASON_NACC_NC2:
                          case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                          case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                          {
                            MSG_GERAN_HIGH_1_G("Ignore abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                          break;

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                          break;
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_RESEL_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_resel_state_name(rr_resel_data_ptr->state)
                    );
                  }
                }
                break;
              } /*MS_RR_RR*/

              case MS_RR_L1:
              {
                rr_cell_acq_control(event, message, gas_id);
                break;
              } /*MS_RR_L1*/

              case MS_TIMER:
              {
                switch (message_id)
                {
                  case MID_TIMER_EXPIRY:
                  {
                    timer_expiry_T *timer_expiry = (timer_expiry_T *) message;

                    switch (timer_expiry->timer_id)
                    {
                      case RR_GPRS_SYS_INFO_TIMER:
                      {
                        /* */
                        break;
                      } /* RR_GPRS_SYS_INFO_TIMER */

                      case CELL_RESELECTION_TIMER:
                      {
                        MSG_GERAN_HIGH_0_G("CELL_RESELECTION_TIMER rxd");

                        /* abort cell acq and moves to RPS ABORTED state and wait for ABORT CNF there */
                        rr_cell_acq_send_imsg_abort_req(gas_id);

                        rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_CELL_RESELECTION_TIMER_EXPIRY;

                        rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_FAILED_ABORTING_CELL_ACQ;
                        break;
                      } /* CELL_RESELECTION_TIMER */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, rr_resel_state_name(rr_resel_data_ptr->state));
                      }
                    }
                    break;
                  } /* MID_TIMER_EXPIRY */

                  default:
                  {
                    LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_resel_state_name(rr_resel_data_ptr->state));
                  }
                }

                break;
              } /* MS_TIMER */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_resel_state_name(rr_resel_data_ptr->state)
                );
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_T3176_TIMEOUT:
          {
            MSG_GERAN_HIGH_0_G("EV_T3176_TIMEOUT rxd");

            rr_cell_acq_send_imsg_abort_req(gas_id);

            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_T3176_TIMER_EXPIRY;

            rr_resel_data_ptr->state = RR_RESEL_STATE_PCCO_FAILED_ABORTING_CELL_ACQ;

            break;
          } /* EV_T3176_TIMEOUT */

          case EV_GMM_LEFT_READY_STATE:
          {
            MSG_GERAN_MED_0_G("RR_RESEL_STATE_PCCO_FAILED: GMM has left READY state, continue with NC0 reselection");

            /* If T3176 is not running then ensure that the reselection timer is running */
            if (!rr_timer_is_running(T3176, gas_id))
            {
              MSG_GERAN_HIGH_0_G("RR CELL_RESELECTION_TIMER timer started");
              (void) rr_timer_start(
                CELL_RESELECTION_TIMER,
                CELL_RESELECTION_TIMEOUT,
                gas_id
              );
            }
            break;
          } /* EV_GMM_LEFT_READY_STATE */

          case EV_MAC_IDLE_MODE:
          {
            break;  // ignore - wait for EV_PS_ACCESS_BLOCKED
          }

          case EV_PS_ACCESS_BLOCKED:
          {
            rr_cell_acq_control(EV_MAC_IDLE_MODE, NULL, gas_id);
            break;
          }

          case EV_MAC_GUARD_TIMER_EXPIRED:
          {
            rr_cell_acq_control(event, message, gas_id);
            break;
          } /* EV_MAC_IDLE_MODE ... */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event),
                                 rr_resel_state_name(rr_resel_data_ptr->state));
          }
        }
        break;
      } /* RR_RESEL_STATE_PCCO_FAILED */

      case RR_RESEL_STATE_PCCO_FAILED_ABORTING_CELL_ACQ:
      {
        switch ( event )
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.cell_acq.omsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_OMSG_ABORT_CNF:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_OMSG_ABORT_CNF rxd");

                        if (rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted == PCCO_T3176_TIMER_EXPIRY)
                        {
                          grr_perform_reselection_power_scan(gas_id);

                          if(rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries != 0)
                          {
                            rr_perform_rps(&rr_resel_data_ptr->rps_data.pscan_cell_list, gas_id);

                            (void) rr_timer_start(
                              CELL_RESELECTION_TIMER,
                              CELL_RESELECTION_TIMEOUT,
                              gas_id
                            );

                            MSG_GERAN_HIGH_0_G("RESELECTION TIMER STARTED");

                            rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                            rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;

                          }
                          else
                          {
                            /* cancel any remaining timers */
                            rr_gprs_cancel_all_five_second_timers(gas_id);

                            /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                            rr_ps_no_ps_access_del_all(gas_id);

                            /*Reinitialise cell reselection */
                            rr_resel_init(gas_id);

                            rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                            rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                          }
                        }
                        else if (rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted == PCCO_GRR_CONTROL_ABORTED)
                        {
                          /* cancel any remaining timers */
                          rr_gprs_cancel_all_five_second_timers(gas_id);

                          /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                          rr_ps_no_ps_access_del_all(gas_id);

                          /*Reinitialise cell reselection */
                          rr_resel_init(gas_id);

                          rr_resel_send_omsg_abort_cnf(gas_id);

                          rr_log_reselect_failure(RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ, gas_id);

                          rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;

                          rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        }
                        else if (rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted == PCCO_CELL_RESELECTION_TIMER_EXPIRY)
                        {
                          /* cancel any remaining timers */
                          rr_gprs_cancel_all_five_second_timers(gas_id);

                          /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                          rr_ps_no_ps_access_del_all(gas_id);

                          /*Reinitialise cell reselection */
                          rr_resel_init(gas_id);

                          /* reselection timeout and this means MS went to NO SERVICE so send finished ind with out of service*/

                          rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                          rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;

                          rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                          rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                        message,
                        rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_OMSG */

                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                        switch (message->rr.resel.imsg.abort_req.reason)
                        {
                          case RR_RESEL_ABORT_REASON_MM_REQ:
                          {
                            /* already sent ABORT req so wait for abort cnf*/
                            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_GRR_CONTROL_ABORTED;
                            break;
                          } /* RR_RESEL_ABORT_REASON_MM_REQ */

                          case RR_RESEL_ABORT_REASON_NACC_NC2:
                          case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                          case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                          {
                            MSG_GERAN_HIGH_1_G("Ignore abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                            break;
                          } /* RR_RESEL_ABORT_REASON_NACC_NC2 ... */

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                        }
                        break;
                      } /* RR_RESEL_IMSG_ABORT_REQ */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                            message,
                            rr_resel_state_name(rr_resel_data_ptr->state)
                          );
                      }
                    }
                    break;
                  } /* RR_RESEL_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_resel_state_name(rr_resel_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_RR_L1:
              {
                rr_cell_acq_control(event, message, gas_id);
                break;
              } /* MS_RR_L1 */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_resel_state_name(rr_resel_data_ptr->state)
                );
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_T3176_TIMEOUT:
          {
            MSG_GERAN_HIGH_0_G("EV_T3176_TIMEOUT rxd");

            rr_log_reselect_failure(RR_LOG_RES_FAILURE_T3174_TIMEOUT, gas_id);

            MSG_GERAN_HIGH_1_G("T3176 expired in state %s", rr_resel_state_name(rr_resel_data_ptr->state));

            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_T3176_TIMER_EXPIRY;

            break;
          } /* EV_T3176_TIMEOUT */

          case EV_MAC_IDLE_MODE:
          case EV_MAC_GUARD_TIMER_EXPIRED:
          {
            rr_cell_acq_control(event, message, gas_id);
            break;
          } /* EV_MAC_IDLE_MODE ... */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event),
                                 rr_resel_state_name(rr_resel_data_ptr->state));
          }
        }
        break;
      } /* RR_RESEL_STATE_PCCO_FAILED_ABORTING_CELL_ACQ */

      case RR_RESEL_STATE_PCCO_FAILED_PENDING_L1_IDLE_CNF:
      {
        switch ( event )
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.resel.imsg.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                        switch (message->rr.resel.imsg.abort_req.reason)
                        {
                          case RR_RESEL_ABORT_REASON_MM_REQ:
                          {
                            /* already sent ABORT req so wait for abort cnf*/

                            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_GRR_CONTROL_ABORTED;
                          }
                          break;

                          case RR_RESEL_ABORT_REASON_NACC_NC2:
                          case RR_RESEL_ABORT_REASON_PAGE_RESPONSE:
                          case RR_RESEL_ABORT_REASON_MAC_RR_EST:
                          {
                            MSG_GERAN_HIGH_1_G("Ignore abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                          break;

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("RR bad abort type: 0x%x",message->rr.resel.imsg.abort_req.reason);
                          }
                          break;
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_RESEL_IMSG */

                  case RR_L1_IDLE_IMSG:
                  {
                    rr_l1_idle_control(message, gas_id);
                    break;
                  } /* RR_L1_IDLE_IMSG */

                  case RR_L1_IDLE_OMSG:
                  {
                    switch (message->rr.l1_idle.omsg.header.imh.message_id)
                    {
                      case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                      {
                        if(rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted == PCCO_NO_ABORT)
                        {
                          rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_IN_SERVICE, gas_id);
                          rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;

                          rr_log_reselect_success(
                            rr_gprs_get_scell_arfcn(gas_id),
                            rr_gprs_get_scell_bsic(gas_id),
                            gas_id
                          );

                          rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        }
                        else if(rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted == PCCO_GRR_CONTROL_ABORTED)
                        {
                          /* cancel any remaining timers */
                          rr_gprs_cancel_all_five_second_timers(gas_id);

                          /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                          rr_ps_no_ps_access_del_all(gas_id);

                          /* Reinitialise cell reselection */
                          rr_resel_init(gas_id);

                          rr_resel_send_omsg_abort_cnf(gas_id);
                          rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_NO_ABORT;
                          rr_log_reselect_failure(RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ, gas_id);
                          rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        }
                        else if(rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted == PCCO_T3176_TIMER_EXPIRY)
                        {
                          grr_perform_reselection_power_scan(gas_id);

                          if(rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries != 0)
                          {
                            rr_perform_rps(&rr_resel_data_ptr->rps_data.pscan_cell_list, gas_id);

                            (void) rr_timer_start(
                              CELL_RESELECTION_TIMER,
                              CELL_RESELECTION_TIMEOUT,
                              gas_id
                            );

                            MSG_GERAN_HIGH_0_G("RESELECTION TIMER STARTED");

                            rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                            rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;

                          }
                          else
                          {
                            /* cancel any remaining timers */
                            rr_gprs_cancel_all_five_second_timers(gas_id);

                            /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                            rr_ps_no_ps_access_del_all(gas_id);

                            /* Reinitialise cell reselection */
                            rr_resel_init(gas_id);

                            rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                            rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                          }
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_resel_state_name(rr_resel_data_ptr->state)
                    );
                  }
                }
                break;
              } /*MS_RR_RR*/

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_resel_state_name(rr_resel_data_ptr->state)
                );
              }
            }
            break;
          }

          case EV_T3176_TIMEOUT:
          {
            MSG_GERAN_HIGH_0_G("EV_T3176_TIMEOUT rxd");

            rr_log_reselect_failure(RR_LOG_RES_FAILURE_T3174_TIMEOUT, gas_id);

            MSG_GERAN_HIGH_1_G("T3176 expired in state %s", rr_resel_state_name(rr_resel_data_ptr->state));

            rr_resel_data_ptr->pcco_data.reason_for_pcco_aborted = PCCO_T3176_TIMER_EXPIRY;

            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event),
                                 rr_resel_state_name(rr_resel_data_ptr->state));
          }
        }

        break;
      }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
      case RR_RESEL_STATE_G2W:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_RESEL_IMSG:
                  {
                    switch (message->rr.header.imh.message_id)
                    {
                      case RR_RESEL_IMSG_ABORT_REQ:
                      {
                        rr_resel_imsg_abort_req_t *abort_req;
                        abort_req = &message->rr.resel.imsg.abort_req;

                        MSG_GERAN_HIGH_1_G("RR_RESEL_IMSG_ABORT_REQ abort_type:0x%x rxd",message->rr.resel.imsg.abort_req.reason);

                        if (rr_resel_data_ptr->g2x_type == RR_G2X_RESEL_TYPE_RESELECTION)
                        {
                          if (abort_req->reason == RR_RESEL_ABORT_REASON_MM_REQ)
                          {
                            rr_resel_g2w_send_imsg_abort_ind(RR_RESEL_G2W_ABORT_CAUSE_MM_REQ, gas_id);
                          }
                          else if (abort_req->reason == RR_RESEL_ABORT_REASON_MAC_RR_EST ||
                                   abort_req->reason == RR_RESEL_ABORT_REASON_PAGE_RESPONSE)
                          {
                            rr_resel_g2w_send_imsg_abort_ind(RR_RESEL_G2W_ABORT_CAUSE_RR_EST_REQ, gas_id);
                          }
                        }
                        else
                        {
                          if (abort_req->reason == RR_RESEL_ABORT_REASON_MM_REQ)
                          {
                            rr_resel_g2w_send_imsg_abort_ind(RR_RESEL_G2W_ABORT_CAUSE_MM_REQ, gas_id);
                          }
                          else
                          {
                            MSG_GERAN_HIGH_1_G("Ignoring abort request (%d), G2W PCCO in progress", abort_req->reason);
                          }
                        }
                        break;
                      } /* RR_RESEL_IMSG_ABORT_REQ */

#ifdef FEATURE_SGLTE
                      case RR_RESEL_IMSG_STOP_GSM_MODE_REQ_SGLTE_RAT_CHANGE:
                      {
                        rr_resel_g2w_send_imsg(RR_RESEL_G2W_IMSG_STOP_GSM_MODE_REQ_SGLTE_RAT_CHANGE, gas_id);
                        break;
                      } /* RR_RESEL_IMSG_STOP_GSM_MODE_REQ_SGLTE_RAT_CHANGE */
#endif /* FEATURE_SGLTE */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_RESEL_IMSG */

                  case RR_RESEL_G2W_OMSG:
                  {
                    switch (message->rr.header.imh.message_id)
                    {
                      case RR_RESEL_G2W_OMSG_RESEL_CNF:
                      {
                        rr_resel_g2w_omsg_resel_cnf_t * resel_cnf;
                        resel_cnf = &message->rr.resel_g2w.omsg.resel_cnf;

                        MSG_GERAN_HIGH_0_G("RR_RESEL_G2W_OMSG_RESEL_CNF rxd");

                        switch (resel_cnf->status)
                        {
                          /* G2W reselection is completed.  Reset this state machine
                          and inform GRR control, which should shut down. */
                          case RR_RESEL_G2W_STATUS_RESELECTION_SUCCESS:
                          {
                            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

#ifdef FEATURE_WCDMA
                            if ((rr_log_reselection_log_complete(gas_id) == 0 ) &&
                                           (rr_resel_data_ptr->g2x_target.rat == RR_RAT_WCDMA))

                            {
                              rr_log_g2w_reselect_success(
                                rr_resel_data_ptr->g2x_target.data.wcdma.uarfcn,
                                rr_resel_data_ptr->g2x_target.data.wcdma.scrambling_code,
                                gas_id
                              );
                            }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_SGLTE
                            if (rr_is_sglte_mode_active(gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("SGLTE G2X reselection successful");
                              rr_g2wr_reset_wcdma_surrounding_database(gas_id);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
                              rr_g2wr_cancel_all_wcdma_five_second_timers(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
                            }
                            else
#endif /* FEATURE_SGLTE */
                            {
                              scell_info_ptr->gsm.camped_on_flag = FALSE;
                            }

                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                            rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_G2W_RESELECTION_SUCCESS, gas_id);
                            break;
                          }

                          /* G2W reselection failed.  Reacquire the old serving cell. */
                          case RR_RESEL_G2W_STATUS_RESELECTION_FAILURE:
                          {
                            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
#ifdef FEATURE_SGLTE
                            if (rr_is_sglte_mode_active(gas_id))
                            {
                              MSG_GERAN_HIGH_0_G("SGLTE G2X reselection Failed");
                              rr_g2wr_reset_wcdma_surrounding_database(gas_id);
                              rr_g2wr_cancel_all_wcdma_five_second_timers(gas_id);                              
                              rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                              rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_G2W_RESELECTION_FAILURE, gas_id);                                                                                          
                            }
                            else
#endif
                            {
                              scell_info_ptr->gsm.camped_on_flag = FALSE;

                              rr_resel_data_ptr->rr_gprs_target_cell = &rr_resel_data_ptr->rr_gprs_old_scell;

                              if (rr_resel_data_ptr->old_scell_arfcn_valid)
                              {
                                /* Need to reacquire the old serving cell. */
                                rr_cell_acq_send_imsg_start_ind(
                                rr_resel_data_ptr->old_scell_arfcn,
                                63, /* hard-coded to value that guarantees that C1 > 0 */
                                NULL,
                                rr_resel_data_ptr->recent_reselection_penalty_flag,
                                RR_CELL_ACQ_ACTION_SELECT_BCCH,
                                FALSE, /* TODO : sync_flag */
                                TRUE, /* clear_si_database */
                                rr_gprs_get_pcell_info(gas_id),
                                NULL, /* Reselection module does it's own PLMN checking. */
                                gas_id
                              );

                                rr_log_reselect_start(RR_RESELECTION_TRIGGER_GSM_TO_WCDMA_FAILED, gas_id);

                                rr_resel_data_ptr->state = RR_RESEL_STATE_G2W_FAILED;
                              }
                              else
                              {
                                /* TODO */
                                MSG_GERAN_ERROR_0_G("TODO");
                              }
                            }
                            break;
                          }

                          /* G2W PCCO is completed.  Reset this state machine
                          and inform GRR control, which should shut down. */
                          case RR_RESEL_G2W_STATUS_PCCO_SUCCESS:
                          {
                            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

#ifdef FEATURE_WCDMA
                            if ((rr_log_reselection_log_complete(gas_id) == 0) &&
                                           (rr_resel_data_ptr->g2x_target.rat == RR_RAT_WCDMA))

                            {
                              rr_log_g2w_reselect_success(
                                rr_resel_data_ptr->g2x_target.data.wcdma.uarfcn,
                                rr_resel_data_ptr->g2x_target.data.wcdma.scrambling_code,
                                gas_id
                              );
                            }
#endif /* FEATURE_WCDMA */

                            scell_info_ptr->gsm.camped_on_flag = FALSE;
                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                            rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_G2W_PCCO_SUCCESS, gas_id);
                            break;
                          }

                          /* G2W PCCO failed.  Start T3176, set the PCCF failure reason and
                          trigger reacquisition of the old serving cell. */
                          case RR_RESEL_G2W_STATUS_PCCO_FAILURE_NO_RESPONSE:
                          {
                            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

#ifdef FEATURE_WCDMA
                            if ((rr_log_reselection_log_complete(gas_id) == 0 ) &&
                                            (rr_resel_data_ptr->g2x_target.rat == RR_RAT_WCDMA))
                            {
                              rr_log_g2w_reselect_failure(
                                rr_resel_data_ptr->g2x_target.data.wcdma.uarfcn,
                                rr_resel_data_ptr->g2x_target.data.wcdma.scrambling_code,
                                RR_LOG_G2WR_WCDMA_CELL_CHANGE_FAILURE,
                                gas_id
                              );
                            }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                            if (rr_resel_data_ptr->g2x_target.rat == RR_RAT_UTRAN_TDD)
                            {
                              rr_log_g2t_reselect_failure(
                              rr_resel_data_ptr->g2x_target.data.utran_tdd.uarfcn, 
                              rr_resel_data_ptr->g2x_target.data.utran_tdd.cpid, 
                              RR_LOG_G2WR_WCDMA_CELL_CHANGE_FAILURE,
                              gas_id);
                            }
#endif /* FEATURE_GSM_TDS */

                            rr_resel_data_ptr->rr_gprs_target_cell = &rr_resel_data_ptr->rr_gprs_old_scell;

                            scell_info_ptr->gsm.camped_on_flag = FALSE;

                            (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);

                            rr_resel_data_ptr->pcco_data.pccf.cause = (uint8) NO_RESPONSE_ON_TARGET;

                            if (rr_resel_data_ptr->old_scell_arfcn_valid)
                            {
                              /* Need to reacquire the old serving cell. */
                              rr_cell_acq_send_imsg_start_ind(
                                rr_resel_data_ptr->old_scell_arfcn,
                                63, /* hard-coded to value that guarantees that C1 > 0 */
                                NULL,
                                rr_resel_data_ptr->recent_reselection_penalty_flag,
                                RR_CELL_ACQ_ACTION_SELECT_BCCH,
                                FALSE, /* TODO: sync_flag */
                                TRUE, /* clear_si_database */
                                rr_gprs_get_pcell_info(gas_id),
                                NULL, /* Reselection module does it's own PLMN checking. */
                                gas_id
                              );

                              rr_log_reselect_start(RR_RESELECTION_TRIGGER_GSM_TO_WCDMA_FAILED, gas_id);

                              rr_resel_data_ptr->state = RR_RESEL_STATE_G2W_FAILED;
                            }
                            else
                            {
                              /* TODO */
                              MSG_GERAN_ERROR_0_G("TODO");
                            }
                            break;
                          } /* RR_RESEL_G2W_STATUS_PCCO_FAILURE_NO_RESPONSE */

                          /* G2W PCCO failed due to assignment reject on the target cell.
                          Attempt reacquisition of old serving cell. */
                          case RR_RESEL_G2W_STATUS_PCCO_FAILURE_ASSIGNMENT_REJECT:
                          {
                            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

#ifdef FEATURE_WCDMA
                            if ((rr_log_reselection_log_complete(gas_id) == 0) &&
                                            (rr_resel_data_ptr->g2x_target.rat == RR_RAT_WCDMA))

                            {
                              rr_log_g2w_reselect_failure(
                                rr_resel_data_ptr->g2x_target.data.wcdma.uarfcn,
                                rr_resel_data_ptr->g2x_target.data.wcdma.scrambling_code,
                                RR_LOG_G2WR_WCDMA_CELL_CHANGE_FAILURE,
                                gas_id
                              );
                            }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS                            
                            if (rr_resel_data_ptr->g2x_target.rat == RR_RAT_UTRAN_TDD)
                            {
                              rr_log_g2t_reselect_failure(
                              rr_resel_data_ptr->g2x_target.data.utran_tdd.uarfcn, 
                              rr_resel_data_ptr->g2x_target.data.utran_tdd.cpid, 
                              RR_LOG_G2WR_WCDMA_CELL_CHANGE_FAILURE,
                              gas_id);
                            }
#endif /* FEATURE_GSM_TDS */

                            scell_info_ptr->gsm.camped_on_flag = FALSE;

                            (void)rr_timer_start(T3176, T3176_TIMEOUT, gas_id);

                            rr_resel_data_ptr->pcco_data.pccf.cause = (uint8) ASSIGNMENT_REJECT_ON_TARGET;

                            rr_resel_data_ptr->rr_gprs_target_cell = &rr_resel_data_ptr->rr_gprs_old_scell;

                            if (rr_resel_data_ptr->old_scell_arfcn_valid)
                            {
                              /* Need to reacquire the old serving cell. */
                              rr_cell_acq_send_imsg_start_ind(
                                rr_resel_data_ptr->old_scell_arfcn,
                                63, /* hard-coded to value that guarantees that C1 > 0 */
                                NULL,
                                rr_resel_data_ptr->recent_reselection_penalty_flag,
                                RR_CELL_ACQ_ACTION_SELECT_BCCH,
                                FALSE, /* TODO: sync_flag */
                                TRUE, /* clear_si_database */
                                rr_gprs_get_pcell_info(gas_id),
                                NULL, /* Reselection module does it's own PLMN checking. */
                                gas_id
                              );

                              rr_log_reselect_start(RR_RESELECTION_TRIGGER_GSM_TO_WCDMA_FAILED, gas_id);

                              rr_resel_data_ptr->state = RR_RESEL_STATE_G2W_FAILED;
                            }
                            else
                            {
                              /* TODO */
                              MSG_GERAN_ERROR_0_G("TODO");
                            }
                            break;
                          } /* RR_RESEL_G2W_STATUS_PCCO_FAILURE_ASSIGNMENT_REJECT */

                          case RR_RESEL_G2W_STATUS_ABORTED_IN_SERVICE:
                          {
                            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

#ifdef FEATURE_WCDMA
                            if ((rr_log_reselection_log_complete(gas_id) == 0 ) &&
                                           (rr_resel_data_ptr->g2x_target.rat == RR_RAT_WCDMA))

                            {
                              rr_log_g2w_reselect_failure(
                                rr_resel_data_ptr->g2x_target.data.wcdma.uarfcn,
                                rr_resel_data_ptr->g2x_target.data.wcdma.scrambling_code,
                                RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ,
                                gas_id
                              );
                            }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                            if (rr_resel_data_ptr->g2x_target.rat == RR_RAT_UTRAN_TDD)
                            {
                              rr_log_g2t_reselect_failure(
                              rr_resel_data_ptr->g2x_target.data.utran_tdd.uarfcn, 
                              rr_resel_data_ptr->g2x_target.data.utran_tdd.cpid, 
                              RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ,
                              gas_id);
                            }
#endif /* FEATURE_GSM_TDS */

                            scell_info_ptr->gsm.camped_on_flag = TRUE;

                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;

                            rr_resel_send_omsg_abort_cnf(gas_id);
                            break;
                          }

                          case RR_RESEL_G2W_STATUS_ABORTED_OUT_OF_SERVICE:
                          {
                            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

#ifdef FEATURE_WCDMA
                            if ((rr_log_reselection_log_complete(gas_id) == 0) &&
                                           (rr_resel_data_ptr->g2x_target.rat == RR_RAT_WCDMA))
                            {
                              rr_log_g2w_reselect_failure(
                                rr_resel_data_ptr->g2x_target.data.wcdma.uarfcn,
                                rr_resel_data_ptr->g2x_target.data.wcdma.scrambling_code,
                                RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ,
                                gas_id
                              );
                            }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                            if (rr_resel_data_ptr->g2x_target.rat == RR_RAT_UTRAN_TDD)
                            {
                              rr_log_g2t_reselect_failure(
                              rr_resel_data_ptr->g2x_target.data.utran_tdd.uarfcn, 
                              rr_resel_data_ptr->g2x_target.data.utran_tdd.cpid, 
                              RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ,
                              gas_id);
                            }
#endif /* FEATURE_GSM_TDS */

                            scell_info_ptr->gsm.camped_on_flag = FALSE;

                            rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;

                            rr_resel_send_omsg_abort_cnf(gas_id);
                            break;
                          }

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("Unexpected status: %d", resel_cnf->status);
                          }
                        }
                        break;
                      } /* RR_RESEL_G2W_OMSG_RESEL_CNF */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_RESEL_G2W_OMSG */

                  default:
                  {
                    rr_resel_g2w_control(message, gas_id);
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                rr_resel_g2w_control(message, gas_id);
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_GSM_MODE_ACTIVATED:
          {
            MSG_GERAN_HIGH_0_G("EV_GSM_MODE_ACTIVATED rxd");
            rr_resel_g2w_send_imsg_l1_activated_ind(gas_id);
            break;
          } /* EV_GSM_MODE_ACTIVATED */

          case EV_GSM_MODE_DEACTIVATED:
          {
            MSG_GERAN_HIGH_0_G("EV_GSM_MODE_DEACTIVATED rxd");
            rr_resel_g2w_send_imsg_l1_deactivated_ind(gas_id);
            break;
          } /* EV_GSM_MODE_DEACTIVATED */

          case EV_MAC_IDLE_MODE:
          {
            break;  // ignore - wait for RR_PS_OMSG_NO_PS_ACCESS_CNF
          } /* EV_MAC_IDLE_MODE */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event),
                                 rr_resel_state_name(rr_resel_data_ptr->state));
          }
        }
        break;
      } /* RR_RESEL_STATE_G2W */

      case RR_RESEL_STATE_G2W_FAILED:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_RESEL_IMSG:
                  {
                    if (message->rr.resel.imsg.header.imh.message_id == RR_RESEL_IMSG_ABORT_REQ)
                    {
                      /* log the abort and advise the cell acquisition state machine */
                      rr_log_reselect_failure(RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ, gas_id);
                      rr_cell_acq_send_imsg_abort_req(gas_id);
                    }
                    else
                    {
                      LOG_UNEXPECTED_MESSAGE(message, "RR_RESEL_STATE_G2W_FAILED");
                    }
                    break;
                  }

                  case RR_CELL_ACQ_IMSG:
                  {
                    rr_cell_acq_control(EV_INPUT_MESSAGE, message, gas_id);
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_CELL_ACQ_OMSG:
                  {
                    switch (message->rr.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_OMSG_ABORT_CNF:
                      {
                        gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

                        /* mark the serving cell as no longer camped */
                        scell_info_ptr->gsm.camped_on_flag = FALSE;

                        /* cancel any remaining timers */
                        rr_gprs_cancel_all_five_second_timers(gas_id);

                        /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                        rr_ps_no_ps_access_del_all(gas_id);

                        /* clean up and go back to idle */
                        rr_resel_init(gas_id);
                        rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        rr_resel_send_omsg_abort_cnf(gas_id);
                        break;
                      }

                      case RR_CELL_ACQ_OMSG_FINISHED_IND:
                      {
                        rr_cell_acq_omsg_finished_ind_t *finished_ind;
                        finished_ind = &message->rr.cell_acq.omsg.finished_ind;

                        MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_OMSG_FINISHED_IND status:0x%x rxd",message->rr.cell_acq.omsg.finished_ind.cell_acq_status);

                        switch (finished_ind->cell_acq_status)
                        {
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED:
                          case RR_CELL_ACQ_STATUS_SUCCESS_BCCH:
                          {
                            rr_gprs_cell_reselection_complete(RESEL_COMPLETE_REACQ_AFTER_G2W_FAILURE, gas_id);
                            rr_l1_idle_send_imsg_start_idle_req(gas_id);

                            rr_resel_data_ptr->state = RR_RESEL_STATE_G2W_FAILED_PENDING_L1_IDLE_CNF;
                            /* TODO: Inhibit RR_RESEL_ABORT_REQ in this state. */
                            break;
                          }

                          default:
                          {
                            MSG_GERAN_HIGH_0_G("Failed to reacquire old serving cell");

                            (void)rr_timer_cancel(T3176, gas_id);

                            grr_perform_reselection_power_scan(gas_id);

                            if (rr_resel_data_ptr->rps_data.pscan_cell_list.no_of_entries != 0)
                            {
                              rr_perform_rps(&rr_resel_data_ptr->rps_data.pscan_cell_list, gas_id);
                              rr_resel_data_ptr->state = RR_RESEL_STATE_RPS;
                              rr_resel_data_ptr->rps_data.rps_sub_state = RR_RESEL_RPS_POWER_SCAN;

                            }
                            else
                            {
                              rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_OUT_OF_SERVICE, gas_id);

                              /* cancel any remaining timers */
                              rr_gprs_cancel_all_five_second_timers(gas_id);

                              /* PS access is already blocked, but ensure any remaining PDUs are deleted */
                              rr_ps_no_ps_access_del_all(gas_id);

                              /* Reinitialise cell reselection */
                              rr_resel_init(gas_id);

                              rr_log_reselect_failure(RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT, gas_id);

                              rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                            }
                            break;
                          } /* default */
                        }
                        break;
                      } /* RR_CELL_ACQ_OMSG_FINISHED_IND */

                      case RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND:
                      {
                        break;
                      } /* RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND */

                      case RR_CELL_ACQ_OMSG_RESELECTED_IND:
                      {
                        /* No action necessary. */
                        break;
                      } /* RR_CELL_ACQ_OMSG_RESELECTED_IND */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message, "RR_RESEL_STATE_G2W_FAILED");
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_OMSG */

                  default:
                  {
                    rr_cell_acq_control(event, message, gas_id);
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                rr_cell_acq_control(event, message, gas_id);
              }
            }
            break;
          }

          default:
          {
            rr_cell_acq_control(event, message, gas_id);
          }
        }
        break;
      } /* RR_RESEL_STATE_G2W_FAILED */

      case RR_RESEL_STATE_G2W_FAILED_PENDING_L1_IDLE_CNF:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_L1_IDLE_IMSG:
                  {
                    rr_l1_idle_control(message, gas_id);
                    break;
                  } /* RR_L1_IDLE_IMSG */

                  case RR_L1_IDLE_OMSG:
                  {
                    switch (message->rr.l1_idle.omsg.header.imh.message_id)
                    {
                      case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                      {
                        rr_resel_send_omsg_finished_ind(RR_RESEL_STATUS_IN_SERVICE, gas_id);
                        rr_resel_data_ptr->state = RR_RESEL_STATE_IDLE;
                        rr_log_reselect_success(
                          rr_gprs_get_scell_arfcn(gas_id),
                          rr_gprs_get_scell_bsic(gas_id),
                          gas_id
                        );
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_resel_state_name(rr_resel_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_resel_state_name(rr_resel_data_ptr->state)
                    );
                  }
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_resel_state_name(rr_resel_data_ptr->state)
                );
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          default:
          {
            LOG_UNEXPECTED_EVENT(
              rr_event_name(event),
              rr_resel_state_name(rr_resel_data_ptr->state)
            );
          }
        }
        break;
      }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

      default:
      {
        MSG_GERAN_ERROR_1_G("Unexpected state in rr_resel_control(): %d",
          rr_resel_data_ptr->state);
      }
    }

    /* Check for a change in state and log appropriately. */
    if (rr_resel_data_ptr->state != rr_resel_data_ptr->old_state)
    {
      /* pccf_transmission_in_progress flag is only valid in
      RR_RESEL_STATE_IDLE, so is cleared on entry to and exit from that
      state. */
      if (rr_resel_data_ptr->state == RR_RESEL_STATE_IDLE ||
          rr_resel_data_ptr->old_state == RR_RESEL_STATE_IDLE)
      {
        if (rr_resel_data_ptr->pccf_transmission_in_progress)
        {
#ifdef PERLUTF
          #error code not present
#else
          MSG_GERAN_HIGH_0_G("Clearing pccf_transmission_in_progress flag");
#endif /* PERLUTF */
          rr_resel_data_ptr->pccf_transmission_in_progress = FALSE;
        }
      }

      MSG_SPRINTF_2(MSG_SSID_DFLT,
                    MSG_LEGACY_HIGH,
                    "rr_resel_control(%s->%s)",
                    rr_resel_state_name(rr_resel_data_ptr->old_state),
                    rr_resel_state_name(rr_resel_data_ptr->state));

      /* If the old state is a transitory state, cancel the state transition
      guard timer. */
      if (rr_resel_control_state_is_transitory(rr_resel_data_ptr->old_state))
      {
        MSG_GERAN_MED_1_G("Cancelling state transition guard timer (left state %d)",
                rr_resel_data_ptr->old_state);
        rr_timer_cancel(RR_RESEL_CONTROL_STATE_TRANSITION_GUARD_TIMER, gas_id);
      }

      /* If the new state is a transitory state, start the state transition
      guard timer. */
      if (rr_resel_control_state_is_transitory(rr_resel_data_ptr->state))
      {
        MSG_GERAN_MED_2_G("Starting state transition guard timer (entered state %d, duration %d ms)",
                rr_resel_data_ptr->state, RR_RESEL_CONTROL_STATE_TRANSITION_TIMEOUT);

        rr_timer_start(
          RR_RESEL_CONTROL_STATE_TRANSITION_GUARD_TIMER,
          RR_RESEL_CONTROL_STATE_TRANSITION_TIMEOUT,
          gas_id
        );
      }

      rr_resel_data_ptr->old_state = rr_resel_data_ptr->state;
    }
    else if (rr_resel_data_ptr->state == RR_RESEL_STATE_IDLE)
    {
      /* If this state machine is reset and the old_state variable is cleared
      while the state transition guard timer is running, the timer will not be
      cancelled by the above code, as no state transition will be detected.  To
      avoid this false detection of an error, we explicitly check for the guard
      timer running if the current state is RR_RESEL_STATE_IDLE (which should
      never be the case) and cancel the timer if necessary. */
      if (rr_timer_is_running(RR_RESEL_CONTROL_STATE_TRANSITION_GUARD_TIMER, gas_id))
      {
        MSG_GERAN_MED_1_G("Cancelling state transition guard timer (state %d)",
                rr_resel_data_ptr->state);

        rr_timer_cancel(RR_RESEL_CONTROL_STATE_TRANSITION_GUARD_TIMER, gas_id);
      }
    }

    if (rr_resel_data_ptr->rps_data.rps_sub_state != rr_resel_data_ptr->rps_data.old_rps_sub_state)
    {
      MSG_SPRINTF_2(
        MSG_SSID_DFLT,
        MSG_LEGACY_HIGH,
        "rr_resel_rps_control(%s->%s)",
        rr_rps_state_name(rr_resel_data_ptr->rps_data.old_rps_sub_state),
        rr_rps_state_name(rr_resel_data_ptr->rps_data.rps_sub_state)
      );

      rr_resel_data_ptr->rps_data.old_rps_sub_state = rr_resel_data_ptr->rps_data.rps_sub_state;
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((event != new_event) && (new_event != EV_NO_EVENT) && (new_event != EV_PATH_LOSS_TIMEOUT)) 
                                     || (rr_resel_data_ptr->old_state != rr_resel_data_ptr->state))
    {
      rr_store_trace_msg_buf(
        GRR_CELL_RESEL_SM,
        new_event,
        (byte) rr_resel_data_ptr->state,
        message,
        gas_id
      );
    }
#endif

    if (new_event != EV_NO_EVENT)
    {
      event = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event = EV_NO_EVENT;
    }

    rr_resel_data_ptr->old_state = rr_resel_data_ptr->state;
  } /* while (event != EV_NO_EVENT) */

  return output_event;
}


/**
  @brief Initialisation function called at RR task start

  Note: The primary purpose of this function is to initialise
  pointers to control/data structures. This should also take
  into account any second instance of data, as this is a
  one-off initialisation.
*/
void rr_resel_task_start_init(
  void
)
{
  int i;
  
  // Zero the control/data structure(s)
  memset(rr_resel_data_spaces, 0, sizeof(rr_resel_data_spaces));

  for (i = 0; i < NUM_GERAN_DATA_SPACES; i++)
  {
  // Setup any specific values
    rr_resel_data_spaces[i].state = RR_RESEL_STATE_IDLE;
    rr_resel_data_spaces[i].old_state = RR_RESEL_STATE_IDLE;
  }
}

/**
  @brief Used to inhibit rank based reselection to UTRAN.  If 
         all instances of SI2q have not been received then we
         will inhibit rank based reselection to UTRAN.  This is
         to prevent better cell reselection to UTRAN where
         priority based reselection is available in the NW and
         SI2q indicating this have not been received.
 
*/
boolean rr_inhibit_utran_reselection(const gas_id_t gas_id)
{
  boolean             inhibit_utran_reselection = FALSE;

#ifdef FEATURE_LTE
  gprs_scell_info_t  *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  
  if (rr_resel_algo(gas_id) == RR_RESEL_ALGO_RANK_BASED)
  {
    if (scell_info_ptr->gsm.two_quater_position != SI2Q_POSITION_NONE)
    {
      if (rr_si2quater_all_instances_received(rr_get_instances_ptr(SI2Q_INSTANCE,gas_id)))
      {
        inhibit_utran_reselection = FALSE;
      }
      else
      {
        inhibit_utran_reselection = TRUE;
      }
    }
  }
#endif /*FEATURE_LTE*/

  return inhibit_utran_reselection;
}

/* EOF */

