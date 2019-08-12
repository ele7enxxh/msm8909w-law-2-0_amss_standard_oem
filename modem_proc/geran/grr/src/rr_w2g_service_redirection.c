/*=====================================================================

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_w2g_service_redirection.c#1 $

RR WCDMA to GSM Service Redirection module.

GENERAL DESCRIPTION

  This module contains the W2G Service Redirection state machine.

  Service redirection is a mechanism by which the network may redirect
  a CS call to a different network type - e.g. If the UE is camped on
  WCDMA and attempts to make a CS call, the network may reject the call
  and indicate to the UE that it should try to establish the call on the
  GSM RAT.

  The behaviour of this procedure is slightly different depending on the search
  reason specified by RRC.  This can take one of the following values:

  RRC_RR_REDIRECTION_SEARCH

  Cell selection will be repeatedly performed until service is found or the
  redirection wait timer expires.  At least one cell selection attempt will be
  completed by RR regardless of whether the redirection wait timer expires.

  Use of is_emergency_call:

  If is_emergency_call is TRUE, RR may camp on any cell (even one in a
  Forbidden PLMN) so that the call may be set up.

  RRC_RR_OUT_OF_SERVICE_SEARCH

  A single cell selection attempt is performed.  If service is not found as a
  result of this, control is immediately returned to RRC.

  If this is a conventional OOS search (rather than UOOS) the timer value
  provided by RRC is ignored and instead a 60 second timer is set.
  This is because a full BCCH list decode of 140 cells can potentially take
  longer than the given RRC timer.

  The RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER should be stopped as soon as a cell
  is found, otherwise it may inhibit G2W reselection after camping.

  Use of is_emergency_call:

  If is_emergency_call is TRUE, RR may camp on any cell except for cells in
  Forbidden PLMNs.


  RRC_RR_SERVICE_SEARCH

  A single cell selection attempt is performed.  Whether service is found or
  not, RR must not camp on a GSM cell.  Instead it should respond to RRC with
  an indication of whether or not service was found.


  RRC_RR_REDIRECTION_AFTER_CONN_RELEASE

  Starts cell selection on the list of cells provided by RRC.
  - If cell selection is successful in GSM, camp and send ACT_IND as normal.
  - If RR is not able to find a suitable cell among the list of cells provided by UTRAN
    within 10s or if the target cell list is empty, it contiues cell selection to find
    any suitable cell on G.
  - If RR fails to find any suitable cell, it sends REDIRECTION_REJ to RRC.

EXTERNALISED FUNCTIONS

  rr_w2g_service_redirection_control()

Copyright (c) 2005-2014 Qualcomm Technologies, Inc.

=====================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "rr_seg_load.h"
#include "rr_w2g_service_redirection.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

#include "comdef.h"
#include "rr_general.h"
#include "ms.h"
#include "rr_mode.h"
#include "rr_cell_selection.h"
#include "rr_mm_send.h"
#include "rr_log.h"
#include "rr_gprs_defs.h"
#include "rr_gprs_debug.h"
#include "rr_rrc_send.h"
#include "rr_shared_cell_selection.h"
#include "rr_control.h"
#include "rr_sys_info_pcell.h"
#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
#include "gprs_mem.h"
#include "dsm_item.h"
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */
#include "rr_ps_domain.h"
#ifdef FEATURE_GSM_TDS
#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"
#include "rr_tds_send.h"
#endif /*FEATURE_GSM_TDS*/
#include "rr_msgr.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"
#include "rr_x2g.h"
#include "rr_nv.h"


/*=====================================================================

  Definitions

=====================================================================*/

#define log_unexpected_state(st) \
  MSG_SPRINTF_1(MSG_SSID_DFLT, \
             MSG_LEGACY_ERROR, \
            "Unexpected state %s", \
            state_name(st));

#define RR_W2G_COOS_SEARCH_TIMER  60000 /* 60 seconds */
#define RR_RXLEV_THRESHOLD_GAP    5     /* 5db        */ 

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT

/* Maximum number of cells for which the other RAT may provide cell
information (tunnelled system information). */
#define RR_X2G_REDIR_CELL_INFO_LIST_MAX_SIZE 32

/* Maximum number of SI messages that may be provided by the other RAT
for each GERAN cell they provide SI information for. */
#define RR_X2G_REDIR_CELL_INFO_SI_MESSAGE_LIST_MAX_SIZE 10

#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */


/*=====================================================================

  Public variables

=====================================================================*/

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT

/* This array defines the order in which tunnelled SI messages will be sorted,
and hence sent in to RR after choosing a cell.  This order is chosen to ensure
that early camping will not be performed as a result of tunnelled system
information.  Also, as SI3 is sent last this means that system information
acquisition will not be completed until all tunnelled system information has
been sent in to RR.  SI1 is always put at the front of the list if it is
included as this eases later checking for it's presence - only the first entry
need be examined, rather than iterating over the whole list. */
static const uint8 rr_x2g_redir_si_message_order[] = {
  SYSTEM_INFORMATION_TYPE_1,
  SYSTEM_INFORMATION_TYPE_13,
  SYSTEM_INFORMATION_TYPE_2,
  SYSTEM_INFORMATION_TYPE_2_BIS,
  SYSTEM_INFORMATION_TYPE_2_TER,
  SYSTEM_INFORMATION_TYPE_2_QUATER,
  SYSTEM_INFORMATION_TYPE_4,
  SYSTEM_INFORMATION_TYPE_3,
};

#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */

/*=====================================================================

  Private types and enums

=====================================================================*/

typedef enum
{
  STATE_NULL,
  STATE_GOING_ACTIVE,
  STATE_GOING_ACTIVE_ABORTED_BY_TIMER_EXPIRY,
  STATE_GOING_ACTIVE_ABORTED_BY_RRC,
  STATE_GOING_ACTIVE_ABORTED_BY_MM,
  STATE_PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_1,
  STATE_PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_N,
  STATE_PERFORMING_CELL_SELECTION_OUT_OF_SERVICE_SEARCH,
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
  STATE_PERFORMING_CELL_SELECTION_SERVICE_SEARCH,
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */
  STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_TIMER_EXPIRY,
  STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC,
  STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_MM,
  STATE_PERFORMING_CELL_SELECTION_ABORTED_NO_BCCH_FOUND,
  STATE_ACQUIRING_SYSTEM_INFO,
  STATE_ACQUIRING_SYSTEM_INFO_ABORTED_BY_RRC,
  STATE_ACQUIRING_SYSTEM_INFO_ABORTED_BY_MM,
  STATE_WAITING_FOR_ACTIVATION_RSP,
  STATE_GOING_INACTIVE_DUE_TO_RRC,
  STATE_GOING_INACTIVE_DUE_TO_MM,
  STATE_GOING_INACTIVE_DUE_TO_TIMER_EXPIRY,
  STATE_GOING_INACTIVE_DUE_TO_NO_BCCH_FOUND
} rr_x2g_redir_state_e;

typedef enum
{
  RR_X2G_REDIR_TYPE_UNDEFINED,
  RR_X2G_REDIR_TYPE_REDIRECTION_SEARCH,
  RR_X2G_REDIR_TYPE_OUT_OF_SERVICE_SEARCH,
  RR_X2G_REDIR_TYPE_SERVICE_SEARCH,
  RR_X2G_REDIR_TYPE_REDIRECTION_AFTER_CONNECTION_RELEASE,
  RR_X2G_REDIR_TYPE_MAX
} rr_x2g_redir_type_e;

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT

typedef struct
{
  ARFCN_T                  arfcn;
  BSIC_T                   bsic;
  uint32                   si_message_list_count;
  geran_grr_si_message_t * si_message_list[RR_X2G_REDIR_CELL_INFO_SI_MESSAGE_LIST_MAX_SIZE];
} rr_x2g_redir_cell_info_t;

#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */

typedef struct
{
  rr_x2g_redir_state_e           state;
  rr_x2g_redir_state_e           old_state;
  uint32                         redirection_wait_time;
  rr_event_T                     abort_event;
  rr_x2g_redir_type_e            type;
  byte                           cell_srch_reason;  
  boolean                        camping_is_possible;
  sys_radio_access_tech_e_type   source_rat;
  boolean                        panic_reset_pending;

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
  uint32                         cell_info_list_count;
  rr_x2g_redir_cell_info_t     * cell_info_list[RR_X2G_REDIR_CELL_INFO_LIST_MAX_SIZE];
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */
#if defined(FEATURE_WCDMA) && defined(FEATURE_LTE)
  lte_irat_dedicated_priority_info_s dedicated_priority_info;
#endif /* FEATURE_WCDMA && FEATURE_LTE */
  boolean                        is_emergency_call;
  boolean                        is_ehplmn_to_be_camped; 
} rr_x2g_redir_data_t;

#define _RR_X2G_REDIRECTION_INIT {STATE_NULL, /*state*/ \
                                 STATE_NULL, /*old_state*/ \
                                 0,          /*redirection_wait_time*/ \
                                 EV_NO_EVENT,/*abort_event*/ \
                                 RR_X2G_REDIR_TYPE_UNDEFINED, /*type*/ \
                                 FALSE, /*camping possible*/ \
                                 SYS_RAT_UMTS_RADIO_ACCESS /*source_rat*/ \
                                }

static rr_x2g_redir_data_t rr_x2g_redir_data_store[NUM_GERAN_DATA_SPACES] =
                                      { INITIAL_VALUE(_RR_X2G_REDIRECTION_INIT) };
#undef _RR_X2G_REDIRECTION_INIT

static rr_reselection_pscan_cell_list_T rr_w2g_redir_cell_list[NUM_GERAN_DATA_SPACES];

/*=====================================================================

  Static functions

=====================================================================*/


static rr_x2g_redir_data_t *rr_x2g_redir_get_data_ptr(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_x2g_redir_data_store[as_index];
}

static rr_reselection_pscan_cell_list_T *rr_x2g_redir_get_cell_list_ptr(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_w2g_redir_cell_list[as_index];
}


/**
 * Returns a state name as char[].
 */
static char *state_name(rr_x2g_redir_state_e s)
{
  switch (s)
  {
    case STATE_NULL:
      return "Null";

    case STATE_GOING_ACTIVE:
      return "GoingActive";

    case STATE_GOING_ACTIVE_ABORTED_BY_TIMER_EXPIRY:
      return "GoingActiveAbortedByTimerExpiry";

    case STATE_GOING_ACTIVE_ABORTED_BY_RRC:
      return "GoingActiveAbortedByRRC";

    case STATE_GOING_ACTIVE_ABORTED_BY_MM:
      return "GoingActiveAbortedByMM";

    case STATE_PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_1:
      return "PerformingCellSelectionRedirectionSearch1";

    case STATE_PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_N:
      return "PerformingCellSelectionRedirectionSearchN";

    case STATE_PERFORMING_CELL_SELECTION_OUT_OF_SERVICE_SEARCH:
      return "PerformingCellSelectionOutOfServiceSearch";

#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION

    case STATE_PERFORMING_CELL_SELECTION_SERVICE_SEARCH:
      return "PerformingCellSelectionServiceSearch";

#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */

    case STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_TIMER_EXPIRY:
      return "PerformingCellSelectionAbortedTimerExp";

    case STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC:
      return "PerformingCellSelectionAbortedRRC";

    case STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_MM:
      return "PerformingCellSelectionAbortedMM";

    case STATE_PERFORMING_CELL_SELECTION_ABORTED_NO_BCCH_FOUND:
      return "PerformingCellSelectionAbortedNoBCCHFound";

    case STATE_ACQUIRING_SYSTEM_INFO:
      return "AcquiringSystemInfo";

    case STATE_ACQUIRING_SYSTEM_INFO_ABORTED_BY_RRC:
      return "AcquiringSystemInfoAbortedByRRC";

    case STATE_ACQUIRING_SYSTEM_INFO_ABORTED_BY_MM:
      return "AcquiringSystemInfoAbortedByMM";

    case STATE_WAITING_FOR_ACTIVATION_RSP:
      return "WaitingForActivationRsp";

    case STATE_GOING_INACTIVE_DUE_TO_RRC:
      return "GoingInactiveDueToRRC";

    case STATE_GOING_INACTIVE_DUE_TO_MM:
      return "GoingInactiveDueToMM";

    case STATE_GOING_INACTIVE_DUE_TO_TIMER_EXPIRY:
      return "GoingInactiveDueToTimerExpiry";

    case STATE_GOING_INACTIVE_DUE_TO_NO_BCCH_FOUND:
      return "GoingInactiveDueToNoBCCHFound";

    default:
      return "Unknown";
  }
}

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT

/**
  @brief Determine the type of the SI message.

  @param si_message The SI message (layer 3 message) to get the type of.
  @param si_message_type Pointer to the variable in which the message
         type should be stored.

  @return TRUE if the value of si_message_type is valid, FALSE if it is
          not.
*/
static boolean rr_x2g_redir_si_message_type(
  geran_grr_si_message_t * si_message,
  uint8                  * si_message_type
)
{
  RR_NULL_CHECK_RETURN_PARAM(si_message, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(si_message_type, FALSE);

  /* For BCCH messages (the only type we get in this container), message type
  is stored in index 2. */
  if (si_message->length >= 3)
  {
    *si_message_type = si_message->data[2];
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
  @brief Determine the order value for the specified SI message.

  This value is used to determine the sort order of SI messages received
  in SI containers and sent in to RR.

  @param si_message the message to get the order value for.

  @return the order value for the message.
*/
static unsigned int rr_x2g_redir_si_message_order_value(
  geran_grr_si_message_t * si_message
)
{
  uint8 si_message_type = 0;
  RR_NULL_CHECK_RETURN_PARAM(si_message, 0);

  if (rr_x2g_redir_si_message_type(si_message, &si_message_type))
  {
    uint32 i;
    for (i = 0; i < sizeof(rr_x2g_redir_si_message_order); ++i)
    {
      if (rr_x2g_redir_si_message_order[i] == si_message_type)
      {
        return i;
      }
    }
  }

  return 0;
}

/**
  @brief Comparison function for qsort.
*/
static int rr_x2g_redir_si_message_order_cmp_fn(
  const void * a,
  const void * b
)
{
  geran_grr_si_message_t ** aa;
  geran_grr_si_message_t ** bb;

  RR_NULL_CHECK_RETURN_PARAM(a, 0);
  RR_NULL_CHECK_RETURN_PARAM(b, 0);

  aa = (geran_grr_si_message_t **) a;
  bb = (geran_grr_si_message_t **) b;

  RR_NULL_CHECK_RETURN_PARAM((*aa), 0);
  RR_NULL_CHECK_RETURN_PARAM((*bb), 0);

  if (rr_x2g_redir_si_message_order_value(*aa) <= rr_x2g_redir_si_message_order_value(*bb))
  {
    return -1;
  }
  else
  {
    return 1;
  }
}

static void rr_x2g_redir_cell_info_free(
  rr_x2g_redir_cell_info_t * cell_info
)
{
  if (cell_info)
  {
    uint32 i;
    for (i = 0;
         i < cell_info->si_message_list_count &&
         i < RR_X2G_REDIR_CELL_INFO_SI_MESSAGE_LIST_MAX_SIZE;
         ++i)
    {
      if (cell_info->si_message_list[i] != NULL)
      {
        GPRS_MEM_FREE(cell_info->si_message_list[i]);
      }
    }
    GPRS_MEM_FREE(cell_info);
  }
}

#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */

/**
 * Reset all static data.
 */
static void reset_internal_variables(
  const gas_id_t gas_id
)
{
#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
  uint32 i;
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr;
  rr_reselection_pscan_cell_list_T *rr_w2g_redir_cell_list_ptr;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  rr_w2g_redir_cell_list_ptr = rr_x2g_redir_get_cell_list_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_w2g_redir_cell_list_ptr);

  rr_x2g_redir_data_ptr->state = STATE_NULL;
  rr_x2g_redir_data_ptr->redirection_wait_time = 0;
  rr_x2g_redir_data_ptr->abort_event = EV_NO_EVENT;
  rr_w2g_redir_cell_list_ptr->valid_data = FALSE;
  rr_x2g_redir_data_ptr->type = RR_X2G_REDIR_TYPE_UNDEFINED;
  rr_x2g_redir_data_ptr->is_emergency_call = FALSE;
  rr_x2g_redir_data_ptr->is_ehplmn_to_be_camped = FALSE;
  rr_x2g_redir_data_ptr->cell_srch_reason = 0xFF;
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
  rr_x2g_redir_data_ptr->camping_is_possible = FALSE;
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */
  rr_x2g_redir_data_ptr->panic_reset_pending = FALSE;

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
  for (i = 0;
       i < rr_x2g_redir_data_ptr->cell_info_list_count &&
       i < RR_X2G_REDIR_CELL_INFO_LIST_MAX_SIZE;
       ++i)
  {
    rr_x2g_redir_cell_info_free(rr_x2g_redir_data_ptr->cell_info_list[i]);
  }
  rr_x2g_redir_data_ptr->cell_info_list_count = 0;
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */

#if defined(FEATURE_WCDMA) && defined(FEATURE_LTE)
  rr_x2g_redir_data_ptr->dedicated_priority_info.rem_validity_timer_is_present = FALSE;
  rr_x2g_redir_data_ptr->dedicated_priority_info.priority_list.num_eutra_freq = 0;
  rr_x2g_redir_data_ptr->dedicated_priority_info.priority_list.num_utra_fdd_freq = 0;
  rr_x2g_redir_data_ptr->dedicated_priority_info.priority_list.num_utra_tdd_freq = 0;
  rr_x2g_redir_data_ptr->dedicated_priority_info.priority_list.num_geran_freq = 0;
  rr_x2g_redir_data_ptr->dedicated_priority_info.priority_list.num_1x_band_class = 0;
  rr_x2g_redir_data_ptr->dedicated_priority_info.priority_list.num_hrdp_band_class = 0;
#endif /* FEATURE_WCDMA && FEATURE_LTE */
}

#ifdef FEATURE_WCDMA
static rr_x2g_redir_type_e rr_x2g_redir_map_from_w2g_cell_srch_reason(
  rrc_rr_cell_srch_reason_e_type cell_srch_reason
)
{
  switch (cell_srch_reason)
  {
    case RRC_RR_REDIRECTION_SEARCH:             return RR_X2G_REDIR_TYPE_REDIRECTION_SEARCH;
    case RRC_RR_OUT_OF_SERVICE_SEARCH:          return RR_X2G_REDIR_TYPE_OUT_OF_SERVICE_SEARCH;
    case RRC_RR_SERVICE_SEARCH:                 return RR_X2G_REDIR_TYPE_SERVICE_SEARCH;
#ifdef FEATURE_WTOG_REDIRECTION_AFTER_CONN_RELEASE
    case RRC_RR_REDIRECTION_AFTER_CONN_RELEASE: return RR_X2G_REDIR_TYPE_REDIRECTION_AFTER_CONNECTION_RELEASE;
#endif
    default:
    {
      MSG_GERAN_ERROR_1("Unexpected cell_srch_reason: %d", cell_srch_reason);
      return RR_X2G_REDIR_TYPE_UNDEFINED;
    }
  }
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS

static rr_x2g_redir_type_e rr_x2g_redir_map_from_t2g_cell_srch_reason(
  tdsrr_cell_srch_reason_e_type cell_srch_reason
)
{
  switch (cell_srch_reason)
  {
    case TDSRR_REDIRECTION_SEARCH:             return RR_X2G_REDIR_TYPE_REDIRECTION_SEARCH;
    case TDSRR_OUT_OF_SERVICE_SEARCH:          return RR_X2G_REDIR_TYPE_OUT_OF_SERVICE_SEARCH;
    case TDSRR_REDIRECTION_AFTER_CONN_RELEASE: return RR_X2G_REDIR_TYPE_REDIRECTION_AFTER_CONNECTION_RELEASE;
    case TDSRR_SERVICE_SEARCH: /*fall through, FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION not required in TDS*/
    default:
    {
      MSG_GERAN_ERROR_1("T->G Redirection Unexpected cell_srch_reason: %d", cell_srch_reason);
      return RR_X2G_REDIR_TYPE_UNDEFINED;
    }
  }
}


/**
 * Process an TDSRR_INTERRAT_SERVICE_REDIRECTION_REQ message received from RRC.
 */
static void process_tds_rr_interrat_redirect_req(
  tdsrr_interrat_redirect_req_type *req,
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T        *rr_PLMN_request_details_ptr;
  rr_x2g_redir_data_t              *rr_x2g_redir_data_ptr;
  rr_reselection_pscan_cell_list_T *rr_w2g_redir_cell_list_ptr;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  rr_w2g_redir_cell_list_ptr = rr_x2g_redir_get_cell_list_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_w2g_redir_cell_list_ptr);

  rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_PLMN_request_details_ptr);

  MSG_GERAN_MED_0_G("process_tds_rr_interrat_redirect_req()");
  MSG_GERAN_MED_3_G("type: %d, network_select_mode: %d, is_emergency_call: %d",
    req->cell_srch_reason,
    req->network_select_mode,
    req->is_emergency_call);
  MSG_GERAN_MED_2_G("wait_timer: %d, gsm_ncell_list.size: %d",
    req->wait_timer,
    req->gsm_ncell_list.size);
  MSG_GERAN_MED_3_G("PLMN: 0x%02x, 0x%02x, 0x%02x",
    req->PLMN.identity[0],
    req->PLMN.identity[1],
    req->PLMN.identity[2]);

  rr_x2g_redir_data_ptr->camping_is_possible = FALSE;

  rr_PLMN_request_details_ptr->network_select_mode = req->network_select_mode;

  rr_store_plmn_id(&req->PLMN, gas_id);

  if (req->is_emergency_call)
  {
    rr_PLMN_request_details_ptr->search_reason = RR_W2G_SERVICE_REDIRECTION_EMERGENCY_CALL;
  }
  else
  {
    rr_PLMN_request_details_ptr->search_reason = RR_W2G_SERVICE_REDIRECTION;
  }

  rr_x2g_redir_data_ptr->is_emergency_call = req->is_emergency_call;
  rr_x2g_redir_data_ptr->redirection_wait_time = req->wait_timer * 1000; /* convert timer to milliseconds */
#if defined(FEATURE_WCDMA) && defined(FEATURE_LTE)
#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
  if (rr_dedicated_priority_info_valid(&req->dedicated_priority_info))
  {
    /* dedicated priorities will be updated in the db after ACTIVATION_RSP */
    rr_x2g_redir_data_ptr->dedicated_priority_info = req->dedicated_priority_info;
  }
  else
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/
  {
    memset(&rr_x2g_redir_data_ptr->dedicated_priority_info, 0, sizeof(rr_x2g_redir_data_ptr->dedicated_priority_info));
  }
#endif /* FEATURE_WCDMA && FEATURE_LTE */

  if (req->gsm_ncell_list.size > 0)
  {
    uint32 i;
    int j;
    rr_w2g_redir_cell_list_ptr->valid_data = TRUE;
    rr_w2g_redir_cell_list_ptr->no_of_entries = 0;

    for (i = 0; i < req->gsm_ncell_list.size; i++)
    {
      boolean unique_arfcn = TRUE;

      for (j = 0; j < rr_w2g_redir_cell_list_ptr->no_of_entries; j++)
      {
        if (ARFCNS_EQUAL(req->gsm_ncell_list.arfcns[i], rr_w2g_redir_cell_list_ptr->cell_list[j]))
        {
          unique_arfcn = FALSE;
          break;
        }
      }

      if (unique_arfcn)
      {
        rr_w2g_redir_cell_list_ptr->cell_list[rr_w2g_redir_cell_list_ptr->no_of_entries++] = req->gsm_ncell_list.arfcns[i];
      }
      else
      {
        MSG_GERAN_MED_2_G("Ignoring duplicate ARFCN: (%d, %d)",
          req->gsm_ncell_list.arfcns[i].num,
          req->gsm_ncell_list.arfcns[i].band);
      }
    }
  }

  rr_PLMN_request_details_ptr->rat_pri_list_info = req->rat_pri_list_info;
  rr_x2g_redir_data_ptr->cell_srch_reason = req->cell_srch_reason;  

  rr_x2g_redir_data_ptr->type = rr_x2g_redir_map_from_t2g_cell_srch_reason(
    req->cell_srch_reason
  );

  rr_x2g_redir_data_ptr->source_rat = SYS_RAT_TDS_RADIO_ACCESS;
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
/**
 * Process an RR_INTERRAT_SERVICE_REDIRECTION_REQ message received from RRC.
 */
static void process_rr_interrat_redirect_req(
  rr_interrat_redirect_req_type *req,
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T        *rr_PLMN_request_details_ptr;
  rr_x2g_redir_data_t              *rr_x2g_redir_data_ptr;
  rr_reselection_pscan_cell_list_T *rr_w2g_redir_cell_list_ptr;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  rr_w2g_redir_cell_list_ptr = rr_x2g_redir_get_cell_list_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_w2g_redir_cell_list_ptr);

  rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_PLMN_request_details_ptr);

  MSG_GERAN_MED_0_G("process_rr_interrat_redirect_req()");
  MSG_GERAN_MED_3_G("type: %d, network_select_mode: %d, is_emergency_call: %d",
    req->cell_srch_reason,
    req->network_select_mode,
    req->is_emergency_call);
  MSG_GERAN_MED_2_G("wait_timer: %d, gsm_ncell_list.size: %d",
    req->wait_timer,
    req->gsm_ncell_list.size);
  MSG_GERAN_MED_3_G("PLMN: 0x%02x, 0x%02x, 0x%02x",
    req->PLMN.identity[0],
    req->PLMN.identity[1],
    req->PLMN.identity[2]);

  rr_x2g_redir_data_ptr->camping_is_possible = FALSE;
  rr_x2g_redir_data_ptr->is_ehplmn_to_be_camped = req->is_ehplmn_to_be_camped;
  MSG_GERAN_HIGH_1_G("is_ehplmn_to_be_camped: %d",req->is_ehplmn_to_be_camped);

  rr_PLMN_request_details_ptr->network_select_mode = req->network_select_mode;

  rr_store_plmn_id(&req->PLMN, gas_id);

  if (req->is_emergency_call)
  {
    rr_PLMN_request_details_ptr->search_reason = RR_W2G_SERVICE_REDIRECTION_EMERGENCY_CALL;
  }
  else
  {
    rr_PLMN_request_details_ptr->search_reason = RR_W2G_SERVICE_REDIRECTION;
  }

  rr_x2g_redir_data_ptr->is_emergency_call = req->is_emergency_call;
  rr_x2g_redir_data_ptr->redirection_wait_time = req->wait_timer * 1000; /* convert timer to milliseconds */
#if defined(FEATURE_WCDMA) && defined(FEATURE_LTE)
  if (rr_dedicated_priority_info_valid(&req->dedicated_priority_info))
  {
    /* dedicated priorities will be updated in the db after ACTIVATION_RSP */
    rr_x2g_redir_data_ptr->dedicated_priority_info = req->dedicated_priority_info;
  }
  else
  {
    memset(&rr_x2g_redir_data_ptr->dedicated_priority_info,0, sizeof(rr_x2g_redir_data_ptr->dedicated_priority_info));
  }
#endif /* FEATURE_WCDMA && FEATURE_LTE */

  if (req->gsm_ncell_list.size > 0)
  {
    uint32 i;
    int j;
    rr_w2g_redir_cell_list_ptr->valid_data = TRUE;
    rr_w2g_redir_cell_list_ptr->no_of_entries = 0;

    for (i = 0; i < req->gsm_ncell_list.size; i++)
    {
      boolean unique_arfcn = TRUE;

      for (j = 0; j < rr_w2g_redir_cell_list_ptr->no_of_entries; j++)
      {
        if (ARFCNS_EQUAL(req->gsm_ncell_list.arfcns[i], rr_w2g_redir_cell_list_ptr->cell_list[j]))
        {
          unique_arfcn = FALSE;
          break;
        }
      }

      if (unique_arfcn)
      {
        rr_w2g_redir_cell_list_ptr->cell_list[rr_w2g_redir_cell_list_ptr->no_of_entries++] = req->gsm_ncell_list.arfcns[i];
      }
      else
      {
        MSG_GERAN_MED_2_G("Ignoring duplicate ARFCN: (%d, %d)",
          req->gsm_ncell_list.arfcns[i].num,
          req->gsm_ncell_list.arfcns[i].band);
      }
    }
  }

  rr_PLMN_request_details_ptr->rat_pri_list_info = req->rat_pri_list_info;
  rr_x2g_redir_data_ptr->cell_srch_reason = req->cell_srch_reason;  

  rr_x2g_redir_data_ptr->type = rr_x2g_redir_map_from_w2g_cell_srch_reason(
    req->cell_srch_reason
  );

#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  rr_x2g_redir_data_ptr->source_rat = SYS_RAT_UMTS_RADIO_ACCESS;
#endif /* FEATURE_LTE || FEATURE_GSM_TDS*/
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT

static void rr_x2g_redir_add_headers_to_si_message(
  geran_grr_si_message_t * si_message
)
{
  RR_NULL_CHECK_RETURN_VOID(si_message);

  if (si_message->length <= GERAN_GRR_SI_MESSAGE_MAX_LENGTH - 2)
  {
    memsmove(&si_message->data[2],sizeof(si_message->data)-2, &si_message->data[0], si_message->length);
    si_message->data[0] = si_message->length << 2; /* L2 Pseudo Length occupies top 6 bits */
    si_message->data[0] |= 0x01; /* L2 Pseudo Length IEI occupies lower 2 bits. */
    si_message->data[1] = RADIO_RESSOURCES;
    si_message->length += 2;
  }
  else
  {
    MSG_GERAN_ERROR_1("si_message->length %d does not allow addtion of headers", si_message->length);
  }
}

/**
  @brief Decodes an SI container received from LTE RRC.

  This function decodes the SI container received from LTE RRC, splitting it
  up in to the individual SI messages that it contains.

  There is some ambiguity in the specifications as to whether the L2 Pseudo
  Length and Protocol Discriminator are included in the messages sent OTA to
  LTE RRC.  This function attempts to detect whether these header bytes are
  present or not, and modifiy the received messages accordingly to add them
  if necessary.

  @param si_container Array containing the SI messages, concatenated.  Each
         message is prefixed with a byte containing it's length.
  @param si_container_length Indicates the total length of the SI container.
  @param si_container_count Indicates the number of entries in the SI container.
*/
static void rr_x2g_redir_process_si_container(
  uint8                    * si_container,
  uint32                     si_container_length,
  uint32                     si_container_count,
  rr_x2g_redir_cell_info_t * dst
)
{
  uint32 i;

  /* Track the index in to the si_container array.  This should never exceed
  si_container_length. */
  uint32 current_index = 0;

  /* Start off assuming that the network has included the header bytes in the
  messages sent OTA.  If at least one is found that does not contain the
  appropriate headers then headers will be added to all messages. */
  boolean headers_already_present = TRUE;

  MSG_GERAN_HIGH_1("Processing SI container with length %d", si_container_length);

  for (i = 0; i < si_container_count && current_index < si_container_length; ++i)
  {
    geran_grr_si_message_t * si_message = GPRS_MEM_CALLOC(sizeof(geran_grr_si_message_t));

    if (si_message != NULL)
    {
      uint32 length = si_container[current_index++];

      /* length variable now contains the indicated length of the SI message that
      follows.  This needs to be validated before we can proceed. */
      if (length > 0)
      {
        /* Our storage space is limited to 23 bytes per message.  Verify that
        we have sufficient si_container buffer left to read MIN(length, 23) from. */
        if ((current_index + MIN(length, GERAN_GRR_SI_MESSAGE_MAX_LENGTH)) <= si_container_length)
        {
          /* Limit the number of bytes that we store if necessary. */
          si_message->length = (uint8) MIN(length, GERAN_GRR_SI_MESSAGE_MAX_LENGTH);
          //Replace memcpy with memscpy
          memscpy(si_message->data,sizeof(si_message->data), &si_container[current_index], si_message->length);


          dst->si_message_list[dst->si_message_list_count++] = si_message;

          /* Increment current_index by the provided in the container itself,
          even if it exceeded our available storage, as otherwise subsequent
          messages may be read incorrectly. */
          current_index += length;

          MSG_GERAN_HIGH_3("SI Message: [ 0x%02x, 0x%02x, 0x%02x ]",
                   si_message->data[0], si_message->data[1], si_message->data[2]);

          /* Track whether headers (L2 Pseudo Length / RR Protocol
          Discriminator) are present in the message. */

          if (si_message->length >= 2)
          {
            if ((si_message->data[0] & 0x03) != 0x01 ||
                si_message->data[1] != RADIO_RESSOURCES)
            {
              headers_already_present = FALSE;
            }
          }
        }
        else
        {
          MSG_GERAN_ERROR_1("End of si_container reached while decoding SI %d", i);
          GPRS_MEM_FREE(si_message);
          break;
        }
      }
      else
      {
        MSG_GERAN_ERROR_0("SI %d has zero length indicated, ignoring");
        GPRS_MEM_FREE(si_message);
      }
    }
    else
    {
      MSG_GERAN_ERROR_0("Memory Allocation Failure");
    }
  }

  if (headers_already_present == FALSE)
  {
    /* At least one of the messages received did not include the L2 Pseudo
    Length IE and Protocol Discriminator.  Assume that all of the messages
    are provided in a consistent format, so add the headers to all
    messages. */

    MSG_GERAN_HIGH_0("Adding L2 Pseudo Length / RR Protocol Discriminator to tunnelled SI messages");
    for (i = 0; i < dst->si_message_list_count; ++i)
    {
      /* Need to add L2 Pseudo Length and RR Protocol Discriminator fields */
      rr_x2g_redir_add_headers_to_si_message(
        dst->si_message_list[i]
      );
    }
  }
}

#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */

#ifdef FEATURE_LTE

/**
  @brief Stores the parameters received in GERAN_GRR_LTE_REDIR_REQ.

  @param lte_redir_req The GERAN_GRR_LTE_REDIR_REQ to process.
*/
static void process_lte_redir_req(
  geran_grr_lte_redir_req_t * lte_redir_req,
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T        *rr_PLMN_request_details_ptr;
  rr_x2g_redir_data_t              *rr_x2g_redir_data_ptr;
  rr_reselection_pscan_cell_list_T *rr_w2g_redir_cell_list_ptr;
  uint32                            i;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  rr_w2g_redir_cell_list_ptr = rr_x2g_redir_get_cell_list_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_w2g_redir_cell_list_ptr);

  rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_PLMN_request_details_ptr);

  rr_x2g_redir_data_ptr->camping_is_possible = FALSE;
  rr_PLMN_request_details_ptr->network_select_mode = lte_redir_req->network_select_mode;
  rr_store_plmn_id(&lte_redir_req->requested_PLMN, gas_id);
  rr_PLMN_request_details_ptr->search_reason = RR_W2G_SERVICE_REDIRECTION;
  rr_PLMN_request_details_ptr->rat_pri_list_info = lte_redir_req->rat_pri_list;

#if 0
  /* Not used during L2G redirection. */
  rr_x2g_redir_data_ptr->redirection_wait_time = 60000; /* 60 seconds for now. */
#endif

#if defined(FEATURE_WCDMA) && defined(FEATURE_LTE)
  if (rr_dedicated_priority_info_valid(&lte_redir_req->dedicated_priority_info))
  {
    /* dedicated priorities will be updated in the db after ACTIVATION_RSP */
    rr_x2g_redir_data_ptr->dedicated_priority_info = lte_redir_req->dedicated_priority_info;
  }
  else
  {
    memset(&rr_x2g_redir_data_ptr->dedicated_priority_info,0, sizeof(rr_x2g_redir_data_ptr->dedicated_priority_info));
  }
#endif /* FEATURE_WCDMA && FEATURE_LTE */

  rr_x2g_redir_data_ptr->type = RR_X2G_REDIR_TYPE_REDIRECTION_AFTER_CONNECTION_RELEASE;

  rr_w2g_redir_cell_list_ptr->valid_data = FALSE;
  rr_w2g_redir_cell_list_ptr->no_of_entries = 0;

  if (lte_redir_req->redir_info.num_of_freq > 0)
  {
    int j; /* int because we want to compare to
           rr_reselection_pscan_cell_list_T.no_of_entries,
           which is a signed value for some reason. */

    rr_w2g_redir_cell_list_ptr->valid_data = TRUE;

    for (i = 0;
         i < lte_redir_req->redir_info.num_of_freq &&
         i < GERAN_GRR_REDIR_MAX_FREQ;
         ++i)
    {
      boolean unique_arfcn = TRUE;
      ARFCN_T arfcn;

      arfcn.num = lte_redir_req->redir_info.freq_list[i].arfcn;

      if (lte_redir_req->redir_info.freq_list[i].band_ind == GERAN_GRR_BAND_IND_PCS_1900)
      {
        arfcn.band = (sys_band_T) rr_get_band_using_band_indicator(
          arfcn.num,
          SI_PSI_BAND_IND_PCS1900,
          gas_id
        );
      }
      else
      {
        arfcn.band = (sys_band_T) rr_get_band_using_band_indicator(
          arfcn.num,
          SI_PSI_BAND_IND_DCS1800,
          gas_id
        );
      }
      if ((rr_nv_depriotize_gsm_cell_is_enabled(gas_id)) && 
          (rr_is_mtcsfb_call_in_progress(gas_id)) && 
          (rr_is_gsm_cell_in_depriotized_list(arfcn,gas_id))
         )
      {
        //No need to add cells in redirection list
        continue;
      }

      for (j = 0;
           j < rr_w2g_redir_cell_list_ptr->no_of_entries &&
           j < RR_MAX_RES_PSCAN_CELL_LIST_SIZE;
           ++j)
      {
        if (ARFCNS_EQUAL(arfcn, rr_w2g_redir_cell_list_ptr->cell_list[j]))
        {
          unique_arfcn = FALSE;
          break;
        }
      }

      if (unique_arfcn)
      {
        rr_w2g_redir_cell_list_ptr->cell_list[rr_w2g_redir_cell_list_ptr->no_of_entries] = arfcn;
        ++rr_w2g_redir_cell_list_ptr->no_of_entries;
      }
    }
  }
  rr_x2g_redir_data_ptr->source_rat = SYS_RAT_LTE_RADIO_ACCESS;

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
  MSG_GERAN_HIGH_1_G("Tunnelled GSM system information for %d cells is available", lte_redir_req->num_cell_info);

  /* Iterate over the cell info structures in the message - for each cell info
  structure present, there should also be a message router attachment that
  contains the tunnelled systm information. */

  for (i = 0;
       i < MIN(lte_redir_req->num_cell_info, GERAN_GRR_MAX_CELL_INFO);
       ++i)
  {
    rr_x2g_redir_cell_info_t * cell_info = GPRS_MEM_CALLOC(sizeof(rr_x2g_redir_cell_info_t));

    if (cell_info != NULL)
    {
      cell_info->arfcn.num = lte_redir_req->cell_info[i].arfcn;

      if (lte_redir_req->cell_info[i].band_ind == GERAN_GRR_BAND_IND_PCS_1900)
      {
        cell_info->arfcn.band = (sys_band_T) rr_get_band_using_band_indicator(
          cell_info->arfcn.num,
          SI_PSI_BAND_IND_PCS1900,
          gas_id
        );
      }
      else
      {
        cell_info->arfcn.band = (sys_band_T) rr_get_band_using_band_indicator(
          cell_info->arfcn.num,
          SI_PSI_BAND_IND_DCS1800,
          gas_id
        );
      }

      cell_info->bsic.PLMN_colour_code = lte_redir_req->cell_info[i].cell_id.ncc;
      cell_info->bsic.BS_colour_code = lte_redir_req->cell_info[i].cell_id.bcc;

      /* Grab the attachment for this cell info. */
      {
        msgr_attach_struct_type * attachment = NULL;

        attachment = msgr_get_attach(&lte_redir_req->hdr, i);

        if (attachment != NULL)
        {
          dsm_item_type * dsm_ptr = NULL;

          msgr_get_dsm_attach(attachment, &dsm_ptr);

          if (dsm_ptr != NULL)
          {
            uint32  si_container_length = dsm_length_packet(dsm_ptr);
            uint8 * si_container = GPRS_MEM_CALLOC(si_container_length);

            if (si_container != NULL)
            {
              dsm_pullup(&dsm_ptr, si_container, (uint16) si_container_length);
              rr_x2g_redir_process_si_container(
                si_container,
                si_container_length,
                lte_redir_req->cell_info[i].si_cont_count,
                cell_info
              );
              GPRS_MEM_FREE(si_container);
            }
          }
        }
      }

      if (rr_x2g_redir_data_ptr->cell_info_list_count < RR_X2G_REDIR_CELL_INFO_LIST_MAX_SIZE)
      {
        MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "ARFCN: %d (band %d), NCC: %d, BCC: %d, SI count: %d",
              cell_info->arfcn.num,
              cell_info->arfcn.band,
              cell_info->bsic.PLMN_colour_code,
              cell_info->bsic.BS_colour_code,
              cell_info->si_message_list_count);
        rr_x2g_redir_data_ptr->cell_info_list[rr_x2g_redir_data_ptr->cell_info_list_count++] = cell_info;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("Too many cell info, discarding");
        rr_x2g_redir_cell_info_free(cell_info);
      }
    }
  }
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */
}

#endif /* FEATURE_LTE */

static rr_forbidden_lai_check_option_e rr_w2g_redirection_get_forbidden_lai_check_rule(
  const gas_id_t gas_id
)
{
  rr_forbidden_lai_check_option_e checking_flag;
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr;
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_PLMN_request_details_ptr);

  /* always check forbidden_lai in OUT_OF_SERVICE_SEARCH or SERVICE_SEARCH,
   * if network_select_mode is manual or automatic
   */
  if (rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
  {
    if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_SEARCH)
    {
      checking_flag = NO_CHECK;
    }
    else
    {
      checking_flag = CHECK_FORBIDDEN_LAI_LIST;
    }
  }
  else if (rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
  {
    if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_SEARCH)
    {
      checking_flag = NO_CHECK;
    }
    else if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_OUT_OF_SERVICE_SEARCH)
    {
      checking_flag = CHECK_FORBIDDEN_LAI_LIST;
    }
    else /* SERIVCE_SEARCH */
    {
      checking_flag = CHECK_REJECT_LAI_LIST;
    }
  }
  else
  {
    checking_flag = NO_CHECK;
  }

  return checking_flag;
}


static boolean rr_w2g_redirection_should_check_forbidden_plmn(
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr;
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_PLMN_request_details_ptr);

  /* always check forbidden_plmn in RRC_RR_OUT_OF_SERVICE_SEARCH if
   * network_select_mode is AUTOMATIC
   */
  if ((rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
      (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_OUT_OF_SERVICE_SEARCH))
  {
    return TRUE;
  }

  return FALSE;
}

boolean rr_w2g_redirection_check_plmn_lai(
  ARFCN_T arfcn,
  LAI_T *lai_ptr,
  const gas_id_t gas_id
)
{
  rr_forbidden_lai_check_option_e check_forbidden_lai;
  rr_x2g_redir_data_t            *rr_x2g_redir_data_ptr;
  boolean                         check_forbidden_plmn;
  boolean                         check_plmn_mismatch;
  boolean                         forbidden_lai;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  check_forbidden_lai = rr_w2g_redirection_get_forbidden_lai_check_rule(gas_id);

  if (check_forbidden_lai == CHECK_FORBIDDEN_LAI_LIST)
  {
    forbidden_lai = rr_is_forbidden_LAI(lai_ptr, gas_id);
  }
  else if (check_forbidden_lai == CHECK_REJECT_LAI_LIST)
  {
    forbidden_lai = rr_check_lai_in_reject_list(lai_ptr, gas_id);
  }
  else
  {
    forbidden_lai = FALSE;
  }

  check_forbidden_plmn = rr_w2g_redirection_should_check_forbidden_plmn(gas_id);

  if (check_forbidden_plmn && (!forbidden_lai))
  {
    inter_task_lai_T inter_task_lai = rr_convert_rr_lai_to_inter_task_lai(lai_ptr);
#ifdef FEATURE_DUAL_SIM
    forbidden_lai = mm_rr_plmn_lai_forbidden_per_subs(inter_task_lai, geran_map_gas_id_to_nas_id(gas_id));
#else
    forbidden_lai = mm_rr_plmn_lai_forbidden(inter_task_lai);
#endif /* FEATURE_DUAL_SIM */
  }

  if(forbidden_lai)
  {
    MSG_GERAN_HIGH_1_G("Cell(%d): forbidden LAI",(int)arfcn.num);
    return FALSE;
  }

  // don't check for PLMN mismatch if redir was for Emergency Call
  if (rr_x2g_redir_data_ptr->is_emergency_call)
  {
    check_plmn_mismatch = FALSE;
  }
  else
  {
    check_plmn_mismatch = TRUE;
  }

  if (check_plmn_mismatch)
  {
    rr_PLMN_request_details_T *rr_PLMN_request_details_ptr;
    boolean                    PLMN_ok;

    rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
    RR_NULL_CHECK_FATAL(rr_PLMN_request_details_ptr);

    PLMN_ok = rr_cell_selection_PLMN_match(rr_PLMN_request_details_ptr, lai_ptr, gas_id);

    if (!PLMN_ok)
    {
      MSG_GERAN_HIGH_1_G("Cell(%d): plmn mismatch",(int) arfcn.num);
      return FALSE;
    }
  }/* end of checking PLMN and EPLMN */

  return TRUE;
}

/**
 * Called when the system information for a Target GSM cell has been received
 * and has to be checked to see if it is suitable/acceptable.
 *
 * @return rr event for InterRAT W2G  Reselection. One of:<BR>
 *   RR_EV_W2GR_NO_SUITABLE_CELL_FOUND : Target GSM cell is not suitable
 *   RR_EV_W2GR_SUITABLE_CELL_FOUND : Target GSM cell is suitable.
 */
static rr_event_T process_pending_channel_data(
  const gas_id_t gas_id
)
{
  int                C1;
  rr_cell_type_T     cell_type;
  gprs_scell_info_t *pcell_info_ptr;

  pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
  RR_NULL_CHECK_FATAL(pcell_info_ptr);

  (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);

  cell_type = rr_get_cell_priority(
    pcell_info_ptr->gsm.cell_selection_parameters.CELL_BAR_QUALIFY,
    pcell_info_ptr->gsm.RACH_control_parameters.CELL_BAR_ACCESS,
    &(pcell_info_ptr->gsm.location_area_identification),
    pcell_info_ptr->gsm.RACH_control_parameters.access_control,
    NULL,
    gas_id
  );

  if (cell_type == RR_CELL_BARRED)
  {
    MSG_GERAN_HIGH_1_G("Cell(%d): is barred", pcell_info_ptr->gsm.BCCH_ARFCN.num);
    return RR_EV_W2G_NO_SUITABLE_CELL_FOUND;
  }

  C1= rr_get_C1(
    pcell_info_ptr->gsm.BCCH_ARFCN,
    0xff, /* TODO get the correct RXLEV_average for the candidate cell */
    pcell_info_ptr->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
    pcell_info_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
    pcell_info_ptr->gsm.cell_selection_parameters.POWER_OFFSET,
    gas_id
  );

  MSG_GERAN_HIGH_2_G("Cell(%d): C1 = %d",(int) pcell_info_ptr->gsm.BCCH_ARFCN.num, (int) C1);

  if (C1 <= 0 && !rr_allow_negative_c1(pcell_info_ptr->gsm.location_area_identification.plmn_id, gas_id))
  {
    return RR_EV_W2G_NO_SUITABLE_CELL_FOUND;
  }

  if (!rr_w2g_redirection_check_plmn_lai(
         pcell_info_ptr->gsm.BCCH_ARFCN,
         &(pcell_info_ptr->gsm.location_area_identification),
         gas_id
       )
     )
  {
    return RR_EV_W2G_NO_SUITABLE_CELL_FOUND;
  }

  MSG_GERAN_HIGH_2_G("Cell(%d,%d): Suitable cell acquired",
    pcell_info_ptr->gsm.BCCH_ARFCN.num,
    pcell_info_ptr->gsm.BCCH_ARFCN.band);

  return RR_EV_W2G_SUITABLE_CELL_FOUND;
}

/**
 * Processes an RR_ACTIVATION_RSP received from MM.  This message contains
 * various pieces of information that must be stored and acted on.  Most of the
 * information is used to populate the rr_PLMN_request_details database.
 */
static void process_rr_activation_rsp(
  const rr_activation_rsp_T *rr_activation_rsp,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t         *pcell_info_ptr;
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr;
  rr_x2g_redir_data_t       *rr_x2g_redir_data_ptr;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
  RR_NULL_CHECK_FATAL(pcell_info_ptr);

  rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_PLMN_request_details_ptr);

  MSG_GERAN_MED_0_G("Processing RR_ACTIVATION_RSP from MM");

  rr_PLMN_request_details_ptr->requested_PLMN_id = rr_activation_rsp->PLMN_id;
  rr_store_service_domain(rr_activation_rsp->service_domain, gas_id);
  //rr_PLMN_request_details.search_reason = RR_AUTONOMOUS_CELL_SELECTION;

  /* cache the LAI from the wideband cell */
  rr_set_previous_LAI(
    rr_activation_rsp->PLMN_id,
    rr_activation_rsp->location_area_code,
    gas_id
  );

  /* Pass the info from activation_rsp to GRR to be used in suspension req */
  if (rr_activation_rsp->PS_Attached)
  {
    grr_set_ota_gprs_suspension_parameters(
      rr_activation_rsp->registered_rai,
      gas_id
    );
  }

  /* Set local equivalent PLMN id if camped on a suitable cell.
   * If the search was for Emergency Call, need to check if the PLMN match,
   * otherwise, the PLMN has been checked in process_pending_channel_data().
   */
  if (rr_x2g_redir_data_ptr->is_emergency_call)
  {
    if (rr_cell_selection_PLMN_match(
          rr_PLMN_request_details_ptr,
          &pcell_info_ptr->gsm.location_area_identification,
          gas_id
        )
       )
    {
      rr_set_equivalent_plmn(
        &(pcell_info_ptr->gsm.location_area_identification.plmn_id),
        gas_id
      );
    }
  }
  else
  {
    rr_set_equivalent_plmn(
      &(pcell_info_ptr->gsm.location_area_identification.plmn_id),
      gas_id
    );
  }
}

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT

/**
  @brief Determines if SI1 has been provided in tunnelled SI messages.

  If SI1 is not provided, the UE should assume it is not broadcast.  This is
  handled in our implementation by sending in all of the other tunnelled SI
  messages with TC set to zero - this triggers System Info handlers to
  decide that SI1 is not broadcast and act appropriately.

  @param arfcn The ARFCN of the cell to check.

  @return TRUE if SI1 is available, FALSE if not.
*/
static boolean rr_x2g_redir_is_si1_available(
  ARFCN_T arfcn,
  const gas_id_t gas_id
)
{
  uint32 i;
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  /* Search through our cell info for an entry for the specific cell, then
  search for an SI message that matches our requested type. */

  for (i = 0;
       i < rr_x2g_redir_data_ptr->cell_info_list_count &&
       i < RR_X2G_REDIR_CELL_INFO_LIST_MAX_SIZE;
       ++i)
  {
    uint32 j;

    if (ARFCNS_EQUAL(rr_x2g_redir_data_ptr->cell_info_list[i]->arfcn, arfcn))
    {
      /* We've found the cell, now we just need to search through it's message
      list for the appropriate message.  All SI message have the message type
      in the byte at index 2. */
      for (j = 0;
           j < rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list_count &&
           j < RR_X2G_REDIR_CELL_INFO_SI_MESSAGE_LIST_MAX_SIZE;
           ++j)
      {
        if (rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list[j]->length >= 3 &&
            rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list[j]->data[2] == SYSTEM_INFORMATION_TYPE_1)
        {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

static boolean rr_x2g_redir_are_si_messages_available(
  ARFCN_T arfcn,
  const gas_id_t gas_id
)
{
  uint32 i;
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  MSG_GERAN_MED_2_G("Checking for stored SI for ARFCN %d (band %d)",
          arfcn.num, arfcn.band);

  for (i = 0;
       i < rr_x2g_redir_data_ptr->cell_info_list_count &&
       i < RR_X2G_REDIR_CELL_INFO_LIST_MAX_SIZE;
       ++i)
  {
    if (rr_x2g_redir_data_ptr->cell_info_list[i] != NULL)
    {
      if (ARFCNS_EQUAL(arfcn, rr_x2g_redir_data_ptr->cell_info_list[i]->arfcn))
      {
        if (rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list_count > 0)
        {
          MSG_GERAN_HIGH_0_G("  SI available");
          return TRUE;
        }
        else
        {
          /* Cell found, but no SI available - just break out of the loop so
          we can emit some debug. */
          break;
        }
      }
    }
  }

  MSG_GERAN_HIGH_0_G("  No SI available");
  return FALSE;
}

boolean rr_x2g_redir_send_specific_si(
  ARFCN_T arfcn,
  uint32 si,
  const gas_id_t gas_id
)
{
  uint32 i;
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  MSG_GERAN_MED_3_G("Attempting to send tunnelled SI message %d for ARFCN %d (band %d)",
          si, arfcn.num, arfcn.band);

  /* Search through our cell info for an entry for the specific cell, then
  search for an SI message that matches our requested type. */

  for (i = 0;
       i < rr_x2g_redir_data_ptr->cell_info_list_count &&
       i < RR_X2G_REDIR_CELL_INFO_LIST_MAX_SIZE;
       ++i)
  {
    if (ARFCNS_EQUAL(rr_x2g_redir_data_ptr->cell_info_list[i]->arfcn, arfcn))
    {
      uint32 j;
      uint8 tc;

      if (rr_x2g_redir_is_si1_available(arfcn, gas_id))
      {
        tc = 8;
      }
      else
      {
        tc = 0;
      }

      /* We've found the cell, now we just need to search through it's message
      list for the appropriate message.  All SI message have the message type
      in the byte at index 2. */
      for (j = 0;
           j < rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list_count &&
           j < RR_X2G_REDIR_CELL_INFO_SI_MESSAGE_LIST_MAX_SIZE;
           ++j)
      {
        if (rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list[j]->length >= 3 &&
            rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list[j]->data[2] == si)
        {
          /* Send the message in a DL_UNIT_DATA_IND, as if it arrived on the
          appropriate channel. */
          dl_unit_data_ind_T dl_unit_data_ind;
          dl_unit_data_ind.message_header.message_set = MS_RR_L2;
          dl_unit_data_ind.message_header.message_id = DL_UNIT_DATA_IND;
          dl_unit_data_ind.SAPI = SAPI0;
          dl_unit_data_ind.l2_channel_type = BCCH;
          dl_unit_data_ind.own_paging_block = FALSE;
          dl_unit_data_ind.tc = tc;
          dl_unit_data_ind.channel_number = arfcn;
          dl_unit_data_ind.l3_message_length = rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list[j]->length;
          dl_unit_data_ind.short_header = FALSE;
          dl_unit_data_ind.gas_id = gas_id;

          memscpy(
             &dl_unit_data_ind.layer3_message[0],
             sizeof(dl_unit_data_ind.layer3_message),
             rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list[j]->data,
             MIN(rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list[j]->length, N201_MAX)
          );

          PUT_IMH_LEN(sizeof(dl_unit_data_ind) - sizeof(IMH_T), &dl_unit_data_ind.message_header)

          if (gs_send_message(GS_QUEUE_RR, &dl_unit_data_ind, TRUE) != GS_SUCCESS)
          {
            return FALSE;
          }
          else
          {
            return TRUE;
          }
        }
      }

      /* SI was not found. */
      return FALSE;
    }
  }
  return FALSE;
}

#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */


/*=====================================================================

  External functions

=====================================================================*/

boolean rr_w2g_redirection_in_progress(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);

  if (rr_x2g_redir_data_ptr != NULL)
  {
    return (rr_x2g_redir_data_ptr->state != STATE_NULL);
  }

  return FALSE;
}

#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION

boolean rr_w2g_service_search_in_progress(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);

  if (rr_x2g_redir_data_ptr != NULL)
  {
    return ((rr_x2g_redir_data_ptr->state != STATE_NULL) &&
            (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_SERVICE_SEARCH));
  }

  return FALSE;
}

boolean rr_l2g_redirection_in_progress(const gas_id_t gas_id)
{

  // Obtain a pointer to the module data
  rr_x2g_redir_data_t *data = rr_x2g_redir_get_data_ptr(gas_id);

  if((data->state != STATE_NULL) && (SYS_RAT_LTE_RADIO_ACCESS == data->source_rat) )
  {
    return TRUE;
  } 
  else
  {
    return FALSE;
  }
}

boolean rr_t2g_redirection_in_progress(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_x2g_redir_data_t *data = rr_x2g_redir_get_data_ptr(gas_id);

  if((data->state != STATE_NULL) && (SYS_RAT_TDS_RADIO_ACCESS == data->source_rat) )
  {
    return TRUE;
  } 
  else
  {
    return FALSE;
  }
}

#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */

boolean rr_w2g_redirection_search_in_progress(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);

  if (rr_x2g_redir_data_ptr != NULL)
  {
    return ((rr_x2g_redir_data_ptr->state != STATE_NULL) &&
            (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_SEARCH));
  }

  return FALSE;
}

boolean rr_w2g_redirection_is_ehplmn_search_allowed(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);

  if (rr_x2g_redir_data_ptr != NULL)
  {
    return ((rr_x2g_redir_data_ptr->is_ehplmn_to_be_camped));
  }

  return FALSE;
}

boolean rr_x2g_redir_search_for_emergency_call_in_progress(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);

  if (rr_x2g_redir_data_ptr != NULL)
  {
    return ((rr_x2g_redir_data_ptr->state != STATE_NULL) &&
            (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_SEARCH) &&
            (rr_x2g_redir_data_ptr->is_emergency_call));
  }

  return FALSE;
}

boolean rr_w2g_oos_search_in_progress(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);

  if (rr_x2g_redir_data_ptr != NULL)
  {
    return ((rr_x2g_redir_data_ptr->state != STATE_NULL) &&
            (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_OUT_OF_SERVICE_SEARCH));
}

  return FALSE;
}

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT

boolean rr_x2g_redir_with_si_in_progress(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);

  if (rr_x2g_redir_data_ptr != NULL)
  {
    return ((rr_x2g_redir_data_ptr->state != STATE_NULL) &&
            (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_AFTER_CONNECTION_RELEASE) &&
            (rr_x2g_redir_data_ptr->cell_info_list_count > 0));
  }

  return FALSE;
}

/**
  @brief Prioritise power scan results so that any frequencies with cell
         info available are moved to the front of the list.
*/
void rr_x2g_redir_prioritise_pscan_results(
  rr_l1_pscan_results_T * pscan_results,
  const gas_id_t gas_id
)
{
  int32 i;
  uint16 guard = 0;
  rr_l1_freq_power_T tmp;
  rr_l1_freq_power_T first_cell_in_pscan;

  MSG_GERAN_MED_0_G("rr_x2g_redir_prioritise_pscan_results");

  /* Here, the guard variable basically tracks the number of elements that we
  have actually moved to the front of the array.  This is used as the for-
  statement terminate condition as we do not wish to reorder these elements
  more than once, so when i finally moves towards the reordered elements we
  can terminate the sorting. */
  if (pscan_results->num_frequencies > 1)
  {

    first_cell_in_pscan = pscan_results->meas[0]; 
    MSG_GERAN_MED_2_G("First cell in PSCAN list is %d with Rxlev as %d",first_cell_in_pscan.arfcn.num,first_cell_in_pscan.RXLEV_average);

    for (i = pscan_results->num_frequencies - 1;
         (i >= guard) && (i < RR_MAX_PSCAN_FREQ_SCAN);
         --i)
    {
      tmp = pscan_results->meas[i];

      /* Check first for tunnelled SIs , if yes then check for the difference in Rxlev with the best cell in list
              if the difference in less than 5dB the priortise else continue. */             
      if ((rr_x2g_redir_are_si_messages_available(tmp.arfcn, gas_id)) &&
          ((first_cell_in_pscan.RXLEV_average - tmp.RXLEV_average) < RR_RXLEV_THRESHOLD_GAP)&&
          (first_cell_in_pscan.RXLEV_average > tmp.RXLEV_average))
      {
        MSG_GERAN_MED_3_G("Prioritising ARFCN (%d, %d) with tunnelled SI information available and Rxlev as %d",
                           tmp.arfcn.num,
                           tmp.arfcn.band,
                           tmp.RXLEV_average);

        /* Move the entry to the front of the list.  To achieve this, we need to
        move all entries from 0 to i-1 along by one entry (to make some space
        at the front of the list for the entry being moved). */

        memsmove(&pscan_results->meas[1],
                 ((sizeof(pscan_results->meas))-(sizeof(pscan_results->meas[0]))),
                 &pscan_results->meas[0],
                 (unsigned int) (sizeof(rr_l1_freq_power_T) * (unsigned int) i));

        pscan_results->meas[0] = tmp;

        /* Increment i because an element has been inserted at the front of the
        list, so our 'current' element has been moved. */
        i++;
        guard++;
      }
    }
  }
}

void rr_x2g_redir_with_si_send_si3_si4_if_available(
  rr_candidate_cell_db_T * candidate_cell_db,
  const gas_id_t gas_id
)
{
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  uint32 i;

  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  for (i = 0;
       i < candidate_cell_db->num_frequencies &&
       i < RR_MAX_CANDIDATE_FREQ_LIST;
       ++i)
  {
    rr_candidate_cell_T * cell = &candidate_cell_db->candidate_cells[i];
    boolean si_found = FALSE;

    if (rr_x2g_redir_send_specific_si(cell->meas.arfcn, SYSTEM_INFORMATION_TYPE_3, gas_id))
    {
      si_found = TRUE;
      MSG_GERAN_MED_2_G("Sent tunnelled SI3 for ARFCN %d (band %d)",
              cell->meas.arfcn.num, cell->meas.arfcn.band);
    }
    else if (rr_x2g_redir_send_specific_si(cell->meas.arfcn, SYSTEM_INFORMATION_TYPE_4, gas_id))
    {
      si_found = TRUE;
      MSG_GERAN_MED_2_G("Sent tunnelled SI4 for ARFCN %d (band %d)",
              cell->meas.arfcn.num, cell->meas.arfcn.band);
    }

    if (si_found)
    {
      uint8 j;
      
      for (j = 0; (j < rr_x2g_redir_data_ptr->cell_info_list_count) && (j < RR_X2G_REDIR_CELL_INFO_LIST_MAX_SIZE); ++j)
      {
        if (ARFCNS_EQUAL(rr_x2g_redir_data_ptr->cell_info_list[j]->arfcn, cell->meas.arfcn))
        { 
          MSG_GERAN_MED_4_G("BSIC of candidate cell updated from (%d, %d) to (%d, %d)",
            cell->bsic.PLMN_colour_code,
            cell->bsic.BS_colour_code,
            rr_x2g_redir_data_ptr->cell_info_list[j]->bsic.PLMN_colour_code,
            rr_x2g_redir_data_ptr->cell_info_list[j]->bsic.BS_colour_code);
          
          cell->bsic.BS_colour_code   = rr_x2g_redir_data_ptr->cell_info_list[j]->bsic.BS_colour_code;
          cell->bsic.PLMN_colour_code = rr_x2g_redir_data_ptr->cell_info_list[j]->bsic.PLMN_colour_code;      
        }
      }
    }
  }
}

void rr_x2g_redir_send_all_si(
  ARFCN_T arfcn,
  BSIC_T bsic,
  const gas_id_t gas_id
)
{
  uint32 i;
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  for (i = 0;
       i < rr_x2g_redir_data_ptr->cell_info_list_count &&
       i < RR_X2G_REDIR_CELL_INFO_LIST_MAX_SIZE;
       ++i)
  {
    if (ARFCNS_EQUAL(rr_x2g_redir_data_ptr->cell_info_list[i]->arfcn, arfcn) &&
        rr_x2g_redir_data_ptr->cell_info_list[i]->bsic.PLMN_colour_code == bsic.PLMN_colour_code &&
        rr_x2g_redir_data_ptr->cell_info_list[i]->bsic.BS_colour_code == bsic.BS_colour_code)
    {
      if (rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list_count > 0)
      {
        uint32 j;
        /* By default, we use TC=8 for all tunnelled SIs, however if SI1 has
        not been included in the SI container then we are allowed to assume
        that the cell does not broadcast it.  If this is the case, we set
        all TCs to 0 - this will trigger the SI handlers to detect that SI1
        is not broadcast. */
        uint8 tc = 8;
        uint8 first_si_message_type = 0;

        MSG_GERAN_HIGH_2_G("Sending all tunnelled system information for ARFCN %d (band %d)",
             arfcn.num, arfcn.band);

        /* Sort our SIs first - we want them in a predictable order. */
        qsort(&rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list[0],
              rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list_count,
              sizeof(geran_grr_si_message_t *),
              rr_x2g_redir_si_message_order_cmp_fn);

        /* Now we know that if SI1 is available it should be in the first
        element of the array. */
        if (rr_x2g_redir_si_message_type(
              rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list[0],
              &first_si_message_type))
        {
          if (first_si_message_type == SYSTEM_INFORMATION_TYPE_1)
          {
            MSG_GERAN_HIGH_0_G("SI1 is broadcast");
          }
          else
          {
            MSG_GERAN_HIGH_0_G("SI1 not broadcast");
            tc = 0;
          }
        }

        for (j = 0;
             j < rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list_count &&
             j < RR_X2G_REDIR_CELL_INFO_SI_MESSAGE_LIST_MAX_SIZE;
             ++j)
        {
          /* Send the message in a DL_UNIT_DATA_IND, as if it arrived on the
          appropriate channel. */
          dl_unit_data_ind_T dl_unit_data_ind;
          dl_unit_data_ind.message_header.message_set = MS_RR_L2;
          dl_unit_data_ind.message_header.message_id = DL_UNIT_DATA_IND;
          dl_unit_data_ind.SAPI = SAPI0;
          dl_unit_data_ind.l2_channel_type = BCCH;
          dl_unit_data_ind.own_paging_block = FALSE;
          dl_unit_data_ind.tc = tc; /* Ignored value */
          dl_unit_data_ind.channel_number = arfcn;
          dl_unit_data_ind.l3_message_length = rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list[j]->length;
          dl_unit_data_ind.short_header = FALSE;
          dl_unit_data_ind.gas_id = gas_id;

          memscpy(
             &dl_unit_data_ind.layer3_message[0],
             sizeof(dl_unit_data_ind.layer3_message),
             rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list[j]->data,
             MIN(rr_x2g_redir_data_ptr->cell_info_list[i]->si_message_list[j]->length, N201_MAX)
          );

          PUT_IMH_LEN(sizeof(dl_unit_data_ind) - sizeof(IMH_T), &dl_unit_data_ind.message_header)

          (void) gs_send_message(GS_QUEUE_RR, &dl_unit_data_ind, TRUE);
        }
      }
      else
      {
        MSG_GERAN_HIGH_2_G("No tunnelled SI available for ARFCN %d (band %d)", arfcn.num, arfcn.band);
      }
      return;
    }
  }
  MSG_GERAN_HIGH_2_G("No tunnelled SI available for ARFCN %d (band %d)", arfcn.num, arfcn.band);
}

#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */

rr_event_T rr_w2g_service_redirection_control(
  rr_event_T event,
  void *new_message,
  const gas_id_t gas_id
)
{
  IMH_T *message_header;
  byte message_id  = 0;
  byte message_set = 0;
  rr_event_T output_event = EV_NO_EVENT;
  rr_event_T new_event = EV_NO_EVENT;
  rr_x2g_redir_data_t *rr_x2g_redir_data_ptr;
  rr_reselection_pscan_cell_list_T *rr_w2g_redir_cell_list_ptr;

  // Obtain a pointer to the module data
  rr_x2g_redir_data_ptr = rr_x2g_redir_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_x2g_redir_data_ptr);

  rr_w2g_redir_cell_list_ptr = rr_x2g_redir_get_cell_list_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_w2g_redir_cell_list_ptr);

  while (event != EV_NO_EVENT)
  {
    if (event == EV_INPUT_MESSAGE)
    {
      if (new_message != NULL)
      {
        message_header = (IMH_T *) new_message;

        message_id = message_header->message_id;
        message_set = message_header->message_set;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("NULL message pointer passed to rr_w2g_service_redirection_control()");
        event = EV_NO_EVENT;
      }
    }

    if (event == EV_RESET_SOFTWARE)
    {
      reset_internal_variables(gas_id);
      (void)rr_timer_cancel(RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER, gas_id);
      (void)rr_timer_cancel(RR_MM_ACTIVATION_RSP_TIMER, gas_id);	  
      event = EV_NO_EVENT;
    }

    if (event == EV_NO_EVENT)
    {
      continue;
    }

    if (event == EV_INPUT_MESSAGE)
    {
      if (message_set == MS_RRC_RR)
      {
#ifdef FEATURE_WCDMA
        if ((rrc_rr_cmd_e_type) message_id == RR_INTERRAT_REDIRECT_ABORT_REQ)
        {
          MSG_GERAN_HIGH_0_G("RR_INTERRAT_REDIRECT_ABORT_REQ received");
          rr_save_msg_for_recovery_after_panic_reset(new_message, gas_id);
          event = RR_EV_ABORT_PROCEDURE;
        }
#endif /* FEATURE_WCDMA */
      }
#ifdef FEATURE_GSM_TDS
      else if (message_set == MS_TDSRRC_RR)
      {
        if ((tdsrr_rrc_cmd_e_type) message_id == TDSRR_INTERRAT_REDIRECT_ABORT_REQ)
        {
          MSG_GERAN_HIGH_0_G("TDSRR_INTERRAT_REDIRECT_ABORT_REQ received");
          rr_save_msg_for_recovery_after_panic_reset(new_message, gas_id);
          event = RR_EV_T2G_REDIR_ABORT_PROCEDURE;
        }
      }
#endif /*FEATURE_GSM_TDS*/
      else if (message_set == MS_TIMER && message_id == (uint8) MID_TIMER_EXPIRY)
      {
        rr_timer_id_T timer_id;

        if (new_message != NULL)
        {
          timer_id = (rr_timer_id_T) ((rr_cmd_bdy_type *) new_message)->mid_timer_expiry.timer_id;

          if (timer_id == RR_GSM_SYS_INFO_TIMER)
          {
            event = RR_EV_SYS_INFO_TIMEOUT;
          }
          else if (timer_id == RR_CELL_SELECTION_ABORT_TIMER)
          {
           event = RR_EV_CELL_SELECTION_ABORT_TIMEOUT;
          }
          else if (timer_id == RR_GPRS_SYS_INFO_TIMER)
          {
            event = RR_EV_GPRS_SYS_INFO_TIMEOUT;
          }
          else if (timer_id == RR_BCCH_DECODE_COMPLETE_TIMER)
          {
            event = RR_EV_DECODE_BCCH_COMPLETE_TIMEOUT;
          }
          /* Allow all other timers to be passed through in to the state machine. */
        }
        else
        {
          MSG_GERAN_ERROR_0_G("Null pointer!");
        }
      }
      else if (message_set == MS_MSGR_RR)
      {
#ifdef FEATURE_LTE
        if (((rr_cmd_bdy_type *) new_message)->rr_msgr_msg.message.hdr.id == GERAN_GRR_LTE_ABORT_REDIR_REQ)
        {
          rr_save_msg_for_recovery_after_panic_reset(new_message, gas_id);
          event = RR_EV_ABORT_PROCEDURE;
        }
#endif /* FEATURE_LTE */
      }
    }

    switch (rr_x2g_redir_data_ptr->state)
    {
      /**
       * STATE_NULL
       *
       * Entry point to the service redirection state machine.  The only event
       * handled here is reception of an RR_INTERRAT_REDIRECT_REQ
       * from RRC to start the procedure.
       */
      case STATE_NULL:
      {
        if (event == EV_INPUT_MESSAGE && new_message != NULL)
        {
          if (message_set == MS_RRC_RR)
          {
#ifdef FEATURE_WCDMA
            rrc_rr_cmd_e_type rrc_rr_cmd = (rrc_rr_cmd_e_type) message_id;

            if (rrc_rr_cmd == RR_INTERRAT_REDIRECT_REQ)
            {
              /* Set the global stop reason to RR_L1_RAT_CHANGE - if this
              procedure fails RR will stop L1 and return control to WCDMA RRC.
              This stop reason is sent to L1 in MPH_STOP_GSM_MODE_REQ and must
              be configured correctly or clock regime failures occur on
              resumption of WCDMA. */
              rr_l1_gsm_stop_reason_set(
                RR_L1_RAT_CHANGE,   // gsm_stop_reason
                gas_id              // gas_id
              );

              process_rr_interrat_redirect_req(
                (rr_interrat_redirect_req_type *) new_message,    // req
                gas_id                                            // gas_id
              );

              if (rr_search_band_pref(gas_id) != SYS_BAND_MASK_EMPTY)
              {
                if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_OUT_OF_SERVICE_SEARCH)
                {
                  MSG_GERAN_HIGH_1_G("Performing COOS search, using %dms timer", RR_W2G_COOS_SEARCH_TIMER);
                  (void) rr_timer_start(
                    RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER,
                    RR_W2G_COOS_SEARCH_TIMER,
                    gas_id
                  );
                }
                else
                {
                  MSG_GERAN_HIGH_1_G("Starting redirection wait timer, %dms",
                           rr_x2g_redir_data_ptr->redirection_wait_time);
                  (void) rr_timer_start(
                    RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER,
                    rr_x2g_redir_data_ptr->redirection_wait_time,
                    gas_id
                  );
                }

                new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                rr_x2g_redir_data_ptr->state = STATE_GOING_ACTIVE;
              }
              else
              {
                MSG_GERAN_ERROR_0_G("No GSM bands enabled, aborting W2G redir");
                rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_NO_BCCH_FOUND;
                new_event = EV_GSM_MODE_DEACTIVATED;
              }
            }
            else
            {
              LOG_UNEXPECTED_MESSAGE(((rr_cmd_bdy_type *) new_message),
                                     state_name(rr_x2g_redir_data_ptr->state));
            }
#endif /* FEATURE_WCDMA */
          }
#ifdef FEATURE_GSM_TDS
          else if (message_set == MS_TDSRRC_RR)
          {
            tdsrr_rrc_cmd_e_type rrc_rr_cmd = (rrc_rr_cmd_e_type) message_id;

            if (rrc_rr_cmd == TDSRR_INTERRAT_REDIRECT_REQ)
            {
              /* Set the global stop reason to RR_L1_RAT_CHANGE - if this
              procedure fails RR will stop L1 and return control to WCDMA RRC.
              This stop reason is sent to L1 in MPH_STOP_GSM_MODE_REQ and must
              be configured correctly or clock regime failures occur on
              resumption of WCDMA. */
              rr_l1_gsm_stop_reason_set(
                RR_L1_RAT_CHANGE,   // gsm_stop_reason
                gas_id              // gas_id
              );

              process_tds_rr_interrat_redirect_req(
                (tdsrr_interrat_redirect_req_type *) new_message,   // req
                gas_id                                              // gas_id
              );

              if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_OUT_OF_SERVICE_SEARCH)
              {
                MSG_GERAN_HIGH_1_G("Performing COOS search, using %dms timer", RR_W2G_COOS_SEARCH_TIMER);
                (void) rr_timer_start(
                  RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER,
                  RR_W2G_COOS_SEARCH_TIMER,
                  gas_id
                );
              }
              else
              {
                MSG_GERAN_HIGH_1_G("Starting redirection wait timer, %dms",
                         rr_x2g_redir_data_ptr->redirection_wait_time);
                (void) rr_timer_start(
                  RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER,
                  rr_x2g_redir_data_ptr->redirection_wait_time,
                  gas_id
                );
              }

              new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
              rr_x2g_redir_data_ptr->state = STATE_GOING_ACTIVE;
            }
            else
            {
              LOG_UNEXPECTED_MESSAGE(((rr_cmd_bdy_type *) new_message),
                                     state_name(rr_x2g_redir_data_ptr->state));
            }
          }
#endif /*FEATURE_GSM_TDS*/
          else if (message_set == MS_MSGR_RR)
          {
            switch (((rr_cmd_bdy_type *) new_message)->rr_msgr_msg.message.hdr.id)
            {
#ifdef FEATURE_LTE
              case GERAN_GRR_LTE_REDIR_REQ:
              {
                process_lte_redir_req(
                  &((rr_cmd_bdy_type *) new_message)->rr_msgr_msg.message.geran_grr_msg_req.lte_redir_req,
                  gas_id
                );

                if (rr_search_band_pref(gas_id) != SYS_BAND_MASK_EMPTY)
                {
                  new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                  rr_x2g_redir_data_ptr->state = STATE_GOING_ACTIVE;
                }
                else
                {
                  MSG_GERAN_ERROR_0_G("No GSM bands enabled, aborting L2G redir");
                  rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_NO_BCCH_FOUND;
                  new_event = EV_GSM_MODE_DEACTIVATED;
                }
                break;
              } /* GERAN_GRR_LTE_REDIR_REQ */
#endif /* FEATURE_LTE */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(((rr_cmd_bdy_type *) new_message),
                                       state_name(rr_x2g_redir_data_ptr->state));
              }
            }
          }
          else
          {
            LOG_UNEXPECTED_MESSAGE(((rr_cmd_bdy_type *) new_message),
                                   state_name(rr_x2g_redir_data_ptr->state));
          }
        }
        else if (event == RR_EV_SERVICE_REDIRECTION_WAIT_TIMER_EXPIRY)
        {
          MSG_GERAN_HIGH_0_G("Ignoring IRAT SR wait timer expiry in NULL state");
        }
#ifdef FEATURE_GSM_TDS
        else if (event == RR_EV_T2G_REDIR_ABORT_PROCEDURE)
        {
          reset_internal_variables(gas_id);

          if (rr_timer_is_running(RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER, gas_id))
          {
            (void) rr_timer_cancel(
              RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER,
              gas_id
            );
          }

          rr_send_tds_interrat_redirect_abort_cnf(
            TDSRRC_RR_SUCCESS,    // abort_status
            gas_id                // gas_id
          );
        }
#endif /*FEATURE_GSM_TDS*/
        else if (event == RR_EV_ABORT_PROCEDURE)
        {
          reset_internal_variables(gas_id);
          (void) rr_timer_cancel(
            RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER,
            gas_id
          );
#ifdef FEATURE_WCDMA
          if (rr_x2g_redir_data_ptr->source_rat == SYS_RAT_UMTS_RADIO_ACCESS)
          {
              rr_send_interrat_redirect_abort_cnf(
              RRC_RR_SUCCESS,   // abort_status
              gas_id            // gas_id
            );
          }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
          if (rr_x2g_redir_data_ptr->source_rat == SYS_RAT_LTE_RADIO_ACCESS)
          {
            rr_x2g_send_x2g_redir_abort_rsp(RR_RAT_LTE, gas_id);
          }
#endif /* FEATURE_LTE */
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event), state_name(rr_x2g_redir_data_ptr->state));
        }
      }
      break;

      /**
       * STATE_GOING_ACTIVE
       *
       * In this state, RR has sent an MPH_START_GSM_MODE_REQ to L1 and is
       * waiting for a response.  When L1 is activated, RR will trigger cell
       * selection.
       */
      case STATE_GOING_ACTIVE:
      {
        if (event == EV_INPUT_MESSAGE && new_message != NULL)
        {
          LOG_UNEXPECTED_MESSAGE(((rr_cmd_bdy_type *) new_message),
                                 state_name(rr_x2g_redir_data_ptr->state));
        }

        else if (event == EV_GSM_MODE_ACTIVATED)
        {
          new_event = rr_selection_control(RR_EV_TRIGGER_CELL_SELECTION_W2G, NULL, gas_id);
        }

        else if (event == EV_CELL_SELECTION_TRIGGERED)
        {
          if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_SEARCH ||
              rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_AFTER_CONNECTION_RELEASE)
          {
            rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_1;
          }

          else if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_OUT_OF_SERVICE_SEARCH)
          {
            rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_OUT_OF_SERVICE_SEARCH;
          }

#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
          else if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_SERVICE_SEARCH)
          {
            rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_SERVICE_SEARCH;
          }
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */

          else
          {
            MSG_GERAN_ERROR_1_G("Unknown type: %d", rr_x2g_redir_data_ptr->type);
          }
        }

        else if (event == RR_EV_SERVICE_REDIRECTION_WAIT_TIMER_EXPIRY)
        {
          if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_OUT_OF_SERVICE_SEARCH)
          {
            MSG_GERAN_HIGH_0_G("W2G redirection (OOS search) aborted by timer expiry!");
            rr_x2g_redir_data_ptr->state = STATE_GOING_ACTIVE_ABORTED_BY_TIMER_EXPIRY;
          }
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
          else if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_SERVICE_SEARCH)
          {
            MSG_GERAN_HIGH_0_G("W2G redirection (service search) aborted by timer expiry!");
            rr_x2g_redir_data_ptr->state = STATE_GOING_ACTIVE_ABORTED_BY_TIMER_EXPIRY;
          }
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */

          /* For REDIRECTION_SEARCH, redirection
          wait timer expiry should not be processed until at least one cell
          selection attempt has been made.  The timer should be left in the
          'expired' state so that it may be checked later. */
        }

        else if (event == RR_EV_ABORT_PROCEDURE)
        {
          MSG_GERAN_HIGH_0_G("W2G redirection aborted by RRC!");
          rr_x2g_redir_data_ptr->state = STATE_GOING_ACTIVE_ABORTED_BY_RRC;
        }

        else if (event == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
          MSG_GERAN_HIGH_0_G("W2G redirection aborted by MM!");
          rr_x2g_redir_data_ptr->state = STATE_GOING_ACTIVE_ABORTED_BY_MM;
        }
#ifdef FEATURE_GSM_TDS
        else if (event == RR_EV_T2G_REDIR_ABORT_PROCEDURE)
        {
          MSG_GERAN_HIGH_0_G("T2G redirection aborted by TDS-RRC!");
          rr_x2g_redir_data_ptr->state = STATE_GOING_ACTIVE_ABORTED_BY_RRC;
        }
#endif /*FEATURE_GSM_TDS*/
        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in GOING_ACTIVE");
          output_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event),
                               state_name(rr_x2g_redir_data_ptr->state));
        }
      }
      break;

      /**
       * STATE_GOING_ACTIVE_ABORTED_BY_RRC
       *
       * RR_INTERRAT_REDIRECT_ABORT_REQ was received in state GOING_ACTIVE.
       * When L1 has finished activating, the procedure should be aborted.
       */
      case STATE_GOING_ACTIVE_ABORTED_BY_RRC:
      {
        if (event == EV_INPUT_MESSAGE && new_message != NULL)
        {
          LOG_UNEXPECTED_MESSAGE(((rr_cmd_bdy_type *) new_message),
                                 state_name(rr_x2g_redir_data_ptr->state));
        }

        else if (event == EV_GSM_MODE_ACTIVATED)
        {
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_RRC;
        }

        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in GOING_ACTIVE_ABORTED_BY_RRC");
          output_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event), state_name(rr_x2g_redir_data_ptr->state));
        }
      }
      break;

      /**
       * STATE_GOING_ACTIVE_ABORTED_BY_MM
       *
       * RR_EV_MM_REQ_RECEIVED_ABORT was received in rr_x2g_redir_data_ptr->state GOING_ACTIVE. When L1 has
       * finished activating, the procedure should be aborted.
       */
      case STATE_GOING_ACTIVE_ABORTED_BY_MM:
      {
        if (event == EV_INPUT_MESSAGE && new_message != NULL)
        {
          LOG_UNEXPECTED_MESSAGE(((rr_cmd_bdy_type *) new_message),
                                 state_name(rr_x2g_redir_data_ptr->state));
        }

        else if (event == EV_GSM_MODE_ACTIVATED)
        {
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_MM;
        }

        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in GOING_ACTIVE_ABORTED_BY_MM");
          output_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event), state_name(rr_x2g_redir_data_ptr->state));
        }
      }
      break;

      /**
       * STATE_GOING_ACTIVE_ABORTED_BY_TIMER_EXPIRY
       *
       * Redirection wait timer has expired in GOING_ACTIVE state and
       * the procedure is an out-of-service search so should be aborted
       * immediately.
       */
      case STATE_GOING_ACTIVE_ABORTED_BY_TIMER_EXPIRY:
      {
        if (event == EV_INPUT_MESSAGE && new_message != NULL)
        {
          LOG_UNEXPECTED_MESSAGE(((rr_cmd_bdy_type *) new_message),
                                 state_name(rr_x2g_redir_data_ptr->state));
        }

        else if (event == EV_GSM_MODE_ACTIVATED)
        {
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_TIMER_EXPIRY;
        }

        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in GOING_ACTIVE_ABORTED_BY_TIMER_EXPIRY");
          output_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event), state_name(rr_x2g_redir_data_ptr->state));
        }
      }
      break;

      /**
       * This state handles the initial cell selection attempt for a service
       * redirection attempt.  In this state, if the wait timer expires it is
       * not handled immediately, but the cell selection is allowed to continue
       * until completion.  If a cell is found, the wait timer expiry is
       * ignored, but if service is not found the timer expiry is handled and
       * control is returned to RRC.
       */
      case STATE_PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_1:
      {
        if (event == EV_INPUT_MESSAGE && new_message != NULL)
        {
          boolean message_handled = FALSE;

          if (message_set == MS_RR_L1)
          {
            rr_l1_message_id_enum_T l1_message_id = (rr_l1_message_id_enum_T) message_id;

            if (l1_message_id == MPH_SELECT_SPECIFIC_BCCH_CNF)
            {
              new_event = rr_selection_process_select_specific_bcch_cnf(
                (mph_select_specific_bcch_cnf_T *) new_message,   // mph_select_specific_bcch_cnf
                gas_id                                            // gas_id
              );
              message_handled = TRUE;
            }
          }

          if (!message_handled)
          {
            new_event = rr_selection_control(event, new_message, gas_id);
          }
        }

        /**
         * RR_EV_CHANNEL_FOUND is returned by
         * rr_selection_process_select_specific_bcch_cnf() if the cell is
         * suitable for camping.
         */
        else if (event == RR_EV_CHANNEL_FOUND)
        {
          rr_x2g_redir_data_ptr->state = STATE_ACQUIRING_SYSTEM_INFO;
          new_event = rr_selection_control(RR_EV_CHANNEL_FOUND, NULL, gas_id);
        }

        /**
         * EV_NO_BCCH_FOUND is returned by
         * rr_selection_process_select_specific_bcch_cnf() if the cell is
         * not suitable for camping.  If the wait timer has not expired,
         * move to the appropriate state and start a new cell selection.  If
         * the wait timer has expired, stop L1 in preparation for returning
         * control to RRC.
         */
        else if (event == EV_NO_BCCH_FOUND)
        {
          /* EV_NO_BCCH_FOUND must be passed in to rr_selection_control() to
          allow it to reset it's state appropriately before we move on. */
          (void) rr_selection_control(EV_NO_BCCH_FOUND, NULL, gas_id);

          if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_AFTER_CONNECTION_RELEASE)
          {
            new_event = EV_NO_BCCH_FOUND;
            rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_N;
          }
          else
          {
            if (rr_timer_has_expired(RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER, gas_id))
            {
              new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
              rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_TIMER_EXPIRY;
            }
            else
            {
              new_event = EV_TRIGGER_CELL_SELECTION;
              rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_N;
            }
          }
        }

        else if (event == RR_EV_SERVICE_REDIRECTION_WAIT_TIMER_EXPIRY)
        {
          /* This event should be ignored in this state - if RR fails to find
          service during then this timer will be checked before commencing
          another round of cell selection. */
        }

        /**
         * RR_EV_ABORT_PROCEDURE is generated if RRC aborts the procedure.
         */
        else if (event == RR_EV_ABORT_PROCEDURE)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC;
        }

#ifdef FEATURE_GSM_TDS
        /**
         * RR_EV_T2G_REDIR_ABORT_PROCEDURE is generated if TDS-RRC aborts the procedure.
         */
        else if (event == RR_EV_T2G_REDIR_ABORT_PROCEDURE)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC;
        }
#endif /*FEATURE_GSM_TDS*/

        /* RR_EV_MM_REQ_RECEIVED_ABORT should not be received in this state -
        MM has not yet been informed that RR is active.  Would a manual mode
        change perhaps cause this to be received? */
        else if (event == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_MM;
        }

        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_1");
          output_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          MSG_GERAN_MED_1_G("Forwarding event 0x%02x to rr_selection_control()", event);
          new_event = rr_selection_control(event, new_message, gas_id);
        }
      }
      break;

      /**
       * This state is entered during a Service Redirection procedure if
       * the initial cell selection attempt fails.  In this state, the
       * cell selection will be repeatedly attempted until the wait timer
       * expires, when control will be immediately returned to RRC.
       */
      case STATE_PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_N:
      {
        if (event == EV_INPUT_MESSAGE && new_message != NULL)
        {
          boolean message_handled = FALSE;

          if (message_set == MS_RR_L1)
          {
            rr_l1_message_id_enum_T l1_message_id = (rr_l1_message_id_enum_T) message_id;

            if (l1_message_id == MPH_SELECT_SPECIFIC_BCCH_CNF)
            {
              new_event = rr_selection_process_select_specific_bcch_cnf(
                (mph_select_specific_bcch_cnf_T *) new_message,   // mph_select_specific_bcch_cnf
                gas_id                                            // gas_id
              );
              message_handled = TRUE;
            }
          }

          if (!message_handled)
          {
            new_event = rr_selection_control(event, new_message, gas_id);
          }
        }

        else if (event == EV_TRIGGER_CELL_SELECTION)
        {
          new_event = rr_selection_control(RR_EV_TRIGGER_CELL_SELECTION_W2G, NULL, gas_id);
        }

        else if (event == EV_CELL_SELECTION_TRIGGERED)
        {
          ; /* This event is generated when RR_EV_TRIGGER_CELL_SELECTION_W2G is
            sent in to rr_selection_control(). */
        }

        /**
         * The cell selection attempt has failed.  Cell selection should be
         * attempted until wait timer expiry, so try again.
         */
        else if (event == EV_NO_BCCH_FOUND)
        {
          /* EV_NO_BCCH_FOUND must be passed in to rr_selection_control() to
          allow it to reset it's state appropriately before we move on. */
          (void) rr_selection_control(EV_NO_BCCH_FOUND, NULL, gas_id);

          if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_AFTER_CONNECTION_RELEASE)
          {
            if (rr_w2g_redir_cell_list_ptr->valid_data)
            {
              /* invalid rr_w2g_redir_cell_list to start search
               * on any suitable cell
               */
              MSG_GERAN_HIGH_0_G("no suitable cell in the given list, continue search ...");
              rr_w2g_redir_cell_list_ptr->valid_data = FALSE;
              new_event = EV_TRIGGER_CELL_SELECTION;
            }
            else
            {
              /* advise the multi-SIM controller of this failure */
              rr_ms_x2g_procedure_failure(RR_X2G_REDIRECTION_FAILURE, gas_id);

              /* abort redirection */
              MSG_GERAN_HIGH_0_G("no suitable search found, abort redirection ...");
              new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
              rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_TIMER_EXPIRY;
            }
          }
          else
          {
            rr_w2g_redir_cell_list_ptr->valid_data = FALSE;
            new_event = EV_TRIGGER_CELL_SELECTION;
          }
        }

        else if (event == RR_EV_ABORT_PROCEDURE)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC;
        }

#ifdef FEATURE_GSM_TDS
        else if (event == RR_EV_T2G_REDIR_ABORT_PROCEDURE)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC;
        }
#endif /*FEATURE_GSM_TDS*/

        else if (event == RR_EV_SERVICE_REDIRECTION_WAIT_TIMER_EXPIRY)
        {
          if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_AFTER_CONNECTION_RELEASE)
          {
            /* for redirection after connection release / connection reject
             * if timer expires, RR should continue to search on any
             * suitable cell, so allow the cell selection procedure continue
             */
          }
          else
          {
            new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
            rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_TIMER_EXPIRY;
          }
        }

        /* RR_EV_MM_REQ_RECEIVED_ABORT should not be received in this state -
        MM has not yet been informed that RR is active.  Would a manual mode
        change perhaps cause this to be received? */
        else if (event == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_MM;
        }

        else if (event == RR_EV_CHANNEL_FOUND)
        {
          rr_x2g_redir_data_ptr->state = STATE_ACQUIRING_SYSTEM_INFO;
          new_event = rr_selection_control(RR_EV_CHANNEL_FOUND, NULL, gas_id);
        }

        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_N");
          output_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          MSG_GERAN_MED_1_G("Forwarding event 0x%02x to rr_selection_control()", event);
          new_event = rr_selection_control(event, new_message, gas_id);
        }

      }
      break;

      /**
       * This state is entered during an Out Of Service search.  In this
       * case, RR will perform a maximum of one cell selection, and will
       * abort immediately if the wait timer expires.
       */
      case STATE_PERFORMING_CELL_SELECTION_OUT_OF_SERVICE_SEARCH:
      {
        if (event == EV_INPUT_MESSAGE && new_message != NULL)
        {
          boolean message_handled = FALSE;

          if (message_set == MS_RR_L1)
          {
            rr_l1_message_id_enum_T l1_message_id = (rr_l1_message_id_enum_T) message_id;

            if (l1_message_id == MPH_SELECT_SPECIFIC_BCCH_CNF)
            {
              new_event = rr_selection_process_select_specific_bcch_cnf(
                (mph_select_specific_bcch_cnf_T *) new_message,   // mph_select_specific_bcch_cnf
                gas_id                                            // gas_id
              );
              message_handled = TRUE;
            }
          }

          if (!message_handled)
          {
            new_event = rr_selection_control(event, new_message, gas_id);
          }
        }

        /**
         * EV_NO_BCCH_FOUND indicates that cell selection has failed.  For an
         * OOS redirection search, RR only makes one cell selection attempt
         * before returning control to RRC.
         */
        else if (event == EV_NO_BCCH_FOUND)
        {
          (void) rr_timer_cancel(
            RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER,    // timer_id
            gas_id                                    // gas_id
          );
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_NO_BCCH_FOUND;
        }

        else if (event == RR_EV_ABORT_PROCEDURE)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC;
        }

#ifdef FEATURE_GSM_TDS
        else if (event == RR_EV_T2G_REDIR_ABORT_PROCEDURE)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC;
        }
#endif /*FEATURE_GSM_TDS*/

        else if (event == RR_EV_SERVICE_REDIRECTION_WAIT_TIMER_EXPIRY)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_TIMER_EXPIRY;
        }

        /* RR_EV_MM_REQ_RECEIVED_ABORT should not be received in this state -
        MM has not yet been informed that RR is active.  Would a manual mode
        change perhaps cause this to be received? */
        else if (event == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
          MSG_GERAN_ERROR_1_G("RR_EV_MM_REQ_RECEIVED_ABORT not expected in state %d",
                    rr_x2g_redir_data_ptr->state);
        }

        else if (event == RR_EV_CHANNEL_FOUND)
        {
          /* No need to keep the timer running, a cell has been found and
          if we leave the timer running it will block G2W reselections until
          it's expiration. */
          (void) rr_timer_cancel(
            RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER,    // timer_id
            gas_id                                    // gas_id
          );
          rr_x2g_redir_data_ptr->state = STATE_ACQUIRING_SYSTEM_INFO;
          new_event = rr_selection_control(RR_EV_CHANNEL_FOUND, NULL, gas_id);
        }

        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in PERFORMING_CELL_SELECTION_OUT_OF_SERVICE_SEARCH");
          output_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          MSG_GERAN_HIGH_1_G("Forwarding event 0x%02x to rr_selection_control()", event);
          new_event = rr_selection_control(event, new_message, gas_id);
        }
      }
      break;

#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION

      /**
       * This state is entered during a Service Service search.  In this
       * case, RR will perform a maximum of one cell selection, and will
       * abort immediately if the wait timer expires.  RR does not actually
       * camp on a cell, but indicates to RRC whether or not it would have
       * been able to.
       *
       * We need to be able to intercept an event of some sort generated
       * when the cell selection state machine decides to camp on a cell.
       * Unfortunately, there is no output event when this happens, so
       * we have to add one.  What normally happens is that cell selection
       * sends an MPH_SELECT_SPECIFIC_BCCH_REQ to L1.  Instead, if there
       * is a W2G Service Search in progress this state machine will be
       * made to abort, then output an event RR_EV_CHANNEL_FOUND.
       */
      case STATE_PERFORMING_CELL_SELECTION_SERVICE_SEARCH:
      {
        if (event == EV_NO_BCCH_FOUND)
        {
          (void) rr_timer_cancel(
            RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER,    // timer_id
            gas_id                                    // gas_id
          );
          rr_x2g_redir_data_ptr->camping_is_possible = FALSE;
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_NO_BCCH_FOUND;
        }

        else if (event == RR_EV_CHANNEL_FOUND)
        {
          /* No need to keep the timer running, a cell has been found and */
          /* if we leave the timer running it will block G2W reselections until*/
          /* it's expiration. */ 
		  (void) rr_timer_cancel(
            RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER,    // timer_id
            gas_id                                    // gas_id
          );                  
          rr_x2g_redir_data_ptr->camping_is_possible = TRUE;
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_NO_BCCH_FOUND;
        }

        else if (event == RR_EV_ABORT_PROCEDURE)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC;
        }

#ifdef FEATURE_GSM_TDS
        else if (event == RR_EV_T2G_REDIR_ABORT_PROCEDURE)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC;
        }
#endif /*FEATURE_GSM_TDS*/

        else if (event == RR_EV_SERVICE_REDIRECTION_WAIT_TIMER_EXPIRY)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_TIMER_EXPIRY;
        }

        else if (event == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
          new_event = rr_selection_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_MM;
        }

        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in PERFORMING_CELL_SELECTION_SERVICE_SEARCH");
          output_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          MSG_GERAN_HIGH_1_G("Forwarding event 0x%02x to rr_selection_control()", event);
          new_event = rr_selection_control(event, new_message, gas_id);
        }
      }
      break;

#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */

      /* RRC has sent an abort request, cell selection has been forwarded the
      abort request.  Wait here for indication from cell selection that it has
      finished aborting. */
      case STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC:
      {
        if (event == EV_CELL_SELECTION_ABORTED)
        {
          MSG_GERAN_HIGH_0_G("Cell selection is aborted");
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_RRC;
        }
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
        else if (event == EV_NO_BCCH_FOUND ||
                 event == RR_EV_CHANNEL_FOUND)
        {
          MSG_GERAN_HIGH_0_G("Cell selection for service search is aborted");
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_RRC;
        }
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */

        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in PERFORMING_CELL_SELECTION_ABORTED_BY_RRC");
          output_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          new_event = rr_selection_control(event, new_message, gas_id);
        }
      }
      break;

      /* MM has sent an abort request, cell selection has been forwarded the
      abort request.  Wait here for indication from cell selection that it has
      finished aborting. */
      case STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_MM:
      {
        if (event == EV_NO_BCCH_FOUND)
        {
          /* Don't think this event should ever be received in this state,
          but handle it just in case. */
          MSG_GERAN_HIGH_0_G("Cell selection is aborted");
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_MM;
        }
        else if (event == EV_CELL_SELECTION_ABORTED)
        {
          MSG_GERAN_HIGH_0_G("Cell selection is aborted");
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_MM;
        }
        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in PERFORMING_CELL_SELECTION_ABORTED_BY_MM");
          output_event = EV_PANIC_RESET_CNF;
        }
        else
        {
          new_event = rr_selection_control(event, new_message, gas_id);
        }
      }
      break;

      /* timer expiry aborts cell selection, cell selection has been forwarded the
      abort request.  Wait here for indication from cell selection that it has
      finished aborting. */
      case STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_TIMER_EXPIRY:
      {
        if (event == EV_CELL_SELECTION_ABORTED)
        {
          MSG_GERAN_HIGH_0_G("Cell selection is aborted");
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_TIMER_EXPIRY;
        }
        else if (event == RR_EV_CHANNEL_FOUND ||
                 event == EV_NO_BCCH_FOUND)
        {
          /* If a Service Search was in progress but aborted due to timer expiry,
          one of these events (most likely EV_NO_BCCH_FOUND) will be returned by
          the rr_selection_complete_abort() function.  It can be treated exactly
          as if the cell selection has been aborted correctly. */
          new_event = EV_CELL_SELECTION_ABORTED;
        }
        else if (event == RR_EV_ABORT_PROCEDURE)
        {
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC;
        }
#ifdef FEATURE_GSM_TDS
        else if (event == RR_EV_T2G_REDIR_ABORT_PROCEDURE)
        {
          rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_ABORTED_BY_RRC;
        }
#endif /*FEATURE_GSM_TDS*/
        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in PERFORMING_CELL_SELECTION_ABORTED_BY_TIMER_EXPIRY");
          output_event = EV_PANIC_RESET_CNF;
        }
        else
        {
          new_event = rr_selection_control(event, new_message, gas_id);
        }
      }
      break;

      case STATE_PERFORMING_CELL_SELECTION_ABORTED_NO_BCCH_FOUND:
      {
        if (event == EV_CELL_SELECTION_ABORTED)
        {
          MSG_GERAN_HIGH_0_G("Cell selection is aborted");
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_NO_BCCH_FOUND;
        }
        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in PERFORMING_CELL_SELECTION_ABORTED_NO_BCCH_FOUND");
          output_event = EV_PANIC_RESET_CNF;
        }
        else
        {
          new_event = rr_selection_control(event, new_message, gas_id);
        }
      }
      break;


      /**
       * STATE_ACQUIRING_SYSTEM_INFO
       *
       * In this state, a suitable cell has been found and L1 has selected
       * it successfully.  RR must wait for GSM system information to be
       * received before it sends the Activation Indication to MM.  It will
       * then change to STATE_WAITING_FOR_ACTIVATION_RSP.
       */
      case STATE_ACQUIRING_SYSTEM_INFO:
      {
        if (event == RR_EV_W2G_SUITABLE_CELL_FOUND)
        {
          rr_log_camping_attempt_success(gas_id);

          /* Tell the cell selection module that the process is complete */
          (void) rr_selection_control(EV_GO_TO_IDLE, NULL, gas_id);

#ifdef FEATURE_SGLTE
          if (rr_is_sglte_mode_active(gas_id))
          {
            rr_send_activation_ind(MM_AS_SGLTE_INTER_RAT_REDIRECTION, gas_id);
          }
          else
#endif /*FEATURE_SGLTE*/
          {          
            rr_send_activation_ind(MM_AS_INTERRAT_RESELECTION, gas_id);
          }

          rr_x2g_redir_data_ptr->state = STATE_WAITING_FOR_ACTIVATION_RSP;
        }

        else if (event == RR_EV_W2G_NO_SUITABLE_CELL_FOUND)
        {
          /* The cell cannot be camped on for some reason - decide what action
          to take. */
          new_event = rr_selection_control(RR_EV_ACQUISITION_FAILURE, NULL, gas_id);
        }

        else if (event == RR_EV_SERVICE_REDIRECTION_WAIT_TIMER_EXPIRY)
        {
          MSG_GERAN_HIGH_0_G("Ignoring redirection wait timer expiry, camping on a suitable cell");
        }

        else if (event == RR_EV_ABORT_PROCEDURE)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_ACQUIRING_SYSTEM_INFO_ABORTED_BY_RRC;
        }

        else if (event == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_ACQUIRING_SYSTEM_INFO_ABORTED_BY_MM;
        }

#ifdef FEATURE_GSM_TDS
        else if (event == RR_EV_T2G_REDIR_ABORT_PROCEDURE)
        {
          new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_ACQUIRING_SYSTEM_INFO_ABORTED_BY_RRC;
        }
#endif /*FEATURE_GSM_TDS*/

        else if (event == RR_EV_GSM_SI_RECEIVED)
        {
          gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

          /* Received the GSM SIs. Need to check if SI13 is being broadcast, and if received yet */
          if (pcell_info_ptr->gsm.gprs_indicator)
          {
            if (pcell_info_ptr->bcch_sys_info.si13_data_valid_flag)
            {
              /* All SIs now received */
              new_event = process_pending_channel_data(gas_id);
            }
            else
            {
              boolean wait_for_si13 = TRUE;
              boolean is_fast_si_acq_enabled = rr_is_fast_si_acq_enabled(&wait_for_si13, gas_id);

              if (is_fast_si_acq_enabled && !wait_for_si13)
              {
                MSG_GERAN_HIGH_0_G("L2G Redir in progress, SI13 not yet received, start GSM idle mode");
                new_event = process_pending_channel_data(gas_id);
              }
              else
              {
                MSG_GERAN_HIGH_0_G("Wait for SI13 ...");
                MSG_GERAN_HIGH_0_G("Start RR_GPRS_SYS_INFO_TIMEOUT (4s)");
                (void)rr_timer_start(RR_GPRS_SYS_INFO_TIMER, RR_GPRS_SYS_INFO_TIMEOUT, gas_id);
              }
            }
          }
          else
          {
            /* All SIs now received */
            new_event = process_pending_channel_data(gas_id);
          }
        }

        else if (event == RR_EV_GPRS_SI13_RECEIVED)
        {
          /* All SIs now received */
          new_event = process_pending_channel_data(gas_id);
        }

        else if (event == EV_NO_BCCH_FOUND)
        {
          if((rr_timer_is_running(RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER, gas_id) == TRUE) && 
		  	 ((rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_SEARCH) ||
              (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_AFTER_CONNECTION_RELEASE)
             )
            )  
          {
            MSG_GERAN_MED_0_G(" X2G Redir: redirection timer still runing continue search ");
            new_event = EV_NO_BCCH_FOUND;
            rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_N;
          }
          else if( (rr_x2g_redir_data_ptr->source_rat == SYS_RAT_LTE_RADIO_ACCESS ) &&
                   (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_AFTER_CONNECTION_RELEASE)
                 )
          {
            MSG_GERAN_MED_0_G(" L2G Redir: No strongest cell found search next if avaiable ");
            new_event = EV_NO_BCCH_FOUND;
            rr_x2g_redir_data_ptr->state = STATE_PERFORMING_CELL_SELECTION_REDIRECTION_SEARCH_N;
          }
          else
          {
            new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
            rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_NO_BCCH_FOUND;
          }
        }

        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in ACQUIRING_SYSTEM_INFO");
          output_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          new_event = rr_selection_control(event, new_message, gas_id);
        }
      }
      break;

      case STATE_ACQUIRING_SYSTEM_INFO_ABORTED_BY_RRC:
      {
        if (event == EV_CELL_SELECTION_ABORTED)
        {
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_RRC;
        }
        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in ACQUIRING_SYSTEM_INFO_ABORTED_BY_RRC");
          output_event = EV_PANIC_RESET_CNF;
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event), state_name(rr_x2g_redir_data_ptr->state));
        }
      }
      break;

      case STATE_ACQUIRING_SYSTEM_INFO_ABORTED_BY_MM:
      {
        if (event == EV_CELL_SELECTION_ABORTED)
        {
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_MM;
        }
        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in ACQUIRING_SYSTEM_INFO_ABORTED_BY_MM");
          output_event = EV_PANIC_RESET_CNF;
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event), state_name(rr_x2g_redir_data_ptr->state));
        }
      }
      break;

      /**
       * STATE_WAITING_FOR_ACTIVATION_RSP
       *
       * In this state, RR is waiting for an Activation Response from MM.  The
       * Activation Response contains details of service domain, so must be
       * received before RR can decide whether it needs to camp on a BCCH or
       * PBCCH.
       *
       * If SI13 is received while in this state, it will be ignored as it will
       * be processed in the next state.
       *
       * When the Activation Response is received, the procedure is complete
       */
      case STATE_WAITING_FOR_ACTIVATION_RSP:
      {
        if (event == EV_INPUT_MESSAGE && new_message != NULL)
        {
          if (message_set == MS_MM_RR)
          {
            /* If there is a pending panic reset then we need to handle it
            appropriately here.  If we have an abort pending then we must
            not clear out the saved panic reset message as it dictates what
            our response should be when the panic reset is completed.  If
            we do not have an abort pending then at this point, as far as
            NAS is concerned, GERAN is now the active RAT - we should clear
            out the saved panic reset message so that after the reset is
            complete RR re-acquires service normally.  For this reason,
            we must perform normal processing of the RR_ACTIVATION_RSP so
            that our PLMN request details are stored correctly. */
            if ((mm_rr_message_id_enum_T) message_id == RR_ACTIVATION_RSP)
            {
              if (rr_x2g_redir_data_ptr->abort_event == RR_EV_ABORT_PROCEDURE)
              {
                if (rr_x2g_redir_data_ptr->panic_reset_pending)
                {
                  output_event = EV_PANIC_RESET_CNF;
                }
                else
                {
                  new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
                  rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_MM;
                }
              }
#ifdef FEATURE_GSM_TDS
              else if (rr_x2g_redir_data_ptr->abort_event == RR_EV_T2G_REDIR_ABORT_PROCEDURE)
              {
                new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
                rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_MM;
              }
#endif /*FEATURE_GSM_TDS*/
              else
              {
                rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);

                process_rr_activation_rsp((rr_activation_rsp_T *) new_message, gas_id);

                if (rr_x2g_redir_data_ptr->panic_reset_pending)
                {
                  output_event = EV_PANIC_RESET_CNF;
                }
                else
                {
#if defined(FEATURE_WCDMA) && defined(FEATURE_LTE)
                  rr_activation_rsp_T *rr_activation_rsp;
				  gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

                  RR_NULL_CHECK_FATAL(pcell_info_ptr);
                  rr_activation_rsp = (&((rr_cmd_bdy_type *) new_message)->rr_activation_rsp);

                  rr_dedicated_priority_info_store(
                    &rr_x2g_redir_data_ptr->dedicated_priority_info,
                    &pcell_info_ptr->gsm.BCCH_ARFCN,
                    rr_activation_rsp->PLMN_id,
                    gas_id);
#endif /* FEATURE_WCDMA && FEATURE_LTE */
				  output_event = EV_GO_TO_IDLE;
                  reset_internal_variables(gas_id);
                }
              }
            }
            else
            {
              LOG_UNEXPECTED_MESSAGE(((rr_cmd_bdy_type *) new_message),
                                     state_name(rr_x2g_redir_data_ptr->state));
            }
          }
          else
          {
            LOG_UNEXPECTED_MESSAGE(((rr_cmd_bdy_type *) new_message),
                                   state_name(rr_x2g_redir_data_ptr->state));
          }
        }
        else if (event == RR_EV_ABORT_PROCEDURE)
        {
          /* Now process Source RAT Abort Request, Waiting for Activation Response from MM */
          /* Abort Confirm will be sent to souirce RAT, once the GSM Mode Deactivation is confirmed 
             in state STATE_GOING_INACTIVE_DUE_TO_RRC */
             
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_RRC;
        }
#ifdef FEATURE_GSM_TDS
        else if (event == RR_EV_T2G_REDIR_ABORT_PROCEDURE)
        {
          /* Now process Source RAT Abort Request, Waiting for Activation Response from MM */
          /* Abort Confirm will be sent to souirce RAT, once the GSM Mode Deactivation is confirmed 
             in state STATE_GOING_INACTIVE_DUE_TO_RRC */
        
          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_RRC;
        }
#endif /*FEATURE_GSM_TDS*/
        else if (event == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
          if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_SEARCH)
          {
            new_event = rr_selection_control(RR_EV_ABORT_PROCEDURE, NULL, gas_id);
            rr_x2g_redir_data_ptr->state = STATE_ACQUIRING_SYSTEM_INFO_ABORTED_BY_MM;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Abort received from MM but Redirection Search not in progress!");
          }
        }
        else if (event == RR_EV_CANDIDATE_CELL_SI_RECEIVED)
        {
          /* ignore this event */
        }
        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in ACQUIRING_SYSTEM_INFO_ABORTED_BY_MM (buffered)");
          rr_x2g_redir_data_ptr->panic_reset_pending = TRUE;
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event), state_name(rr_x2g_redir_data_ptr->state));
        }
      }
      break;


      case STATE_GOING_INACTIVE_DUE_TO_RRC:
      {
        if (event == EV_GSM_MODE_DEACTIVATED)
        {
#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
          if (rr_x2g_redir_data_ptr->source_rat == SYS_RAT_UMTS_RADIO_ACCESS)
#endif /* FEATURE_LTE || FEATURE_GSM_TDS*/
          {
#ifdef FEATURE_WCDMA
            rr_send_interrat_redirect_abort_cnf(
              RRC_RR_SUCCESS,   // abort_status
              gas_id            // gas_id
            );
#endif /* FEATURE_WCDMA */
          }
#ifdef FEATURE_GSM_TDS
          else if (rr_x2g_redir_data_ptr->source_rat == SYS_RAT_TDS_RADIO_ACCESS)
          {
            rr_send_tds_interrat_redirect_abort_cnf(
              TDSRRC_RR_SUCCESS,    // abort_status
              gas_id                // gas_id
            );
          }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
          else if (rr_x2g_redir_data_ptr->source_rat == SYS_RAT_LTE_RADIO_ACCESS)
          {
            msgr_hdr_struct_type lte_abort_redir_rsp;

            memset(&lte_abort_redir_rsp, 0, sizeof(lte_abort_redir_rsp));

            msgr_init_hdr(
              &lte_abort_redir_rsp,
              MSGR_GERAN_GRR,
              GERAN_GRR_LTE_ABORT_REDIR_RSP
            );

            rr_msgr_send(&lte_abort_redir_rsp, sizeof(lte_abort_redir_rsp), gas_id);
          }
#endif /* FEATURE_LTE */
          output_event = RR_EV_W2G_GO_TO_INACTIVE;
          reset_internal_variables(gas_id);
        }

        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in GOING_INACTIVE_DUE_TO_RRC");
          output_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event), state_name(rr_x2g_redir_data_ptr->state));
        }
      }
      break;

      case STATE_GOING_INACTIVE_DUE_TO_MM:
      {
        if (event == EV_GSM_MODE_DEACTIVATED)
        {
          output_event = EV_PROCESS_SAVED_MESSAGE;
          reset_internal_variables(gas_id);
        }

        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in GOING_INACTIVE_DUE_TO_MM");
          output_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event), state_name(rr_x2g_redir_data_ptr->state));
        }
      }
      break;

      case STATE_GOING_INACTIVE_DUE_TO_TIMER_EXPIRY:
      {
        if (event == EV_GSM_MODE_DEACTIVATED)
        {
#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
          if (rr_x2g_redir_data_ptr->source_rat == SYS_RAT_UMTS_RADIO_ACCESS)
#endif /* FEATURE_LTE || FEATURE_GSM_TDS*/
          {
#ifdef FEATURE_WCDMA
            rr_send_interrat_redirect_rej(
              RR_RRC_REDIRECT_FAIL_WAIT_TIMER_EXPIRY,   // failure_reason
              NULL,                                     // available_plmn_db
              gas_id                                    // gas_id
            );
#endif /* FEATURE_WCDMA */
          }
#ifdef FEATURE_GSM_TDS
          else if (rr_x2g_redir_data_ptr->source_rat == SYS_RAT_TDS_RADIO_ACCESS)
          {
            rr_send_tds_interrat_redirect_rej(
              TDSRR_REDIRECT_FAIL_WAIT_TIMER_EXPIRY,    // failure_reason
              NULL,                                     // available_plmn_db
              gas_id                                    // gas_id
            );
          }
#endif /* FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
          else if (rr_x2g_redir_data_ptr->source_rat == SYS_RAT_LTE_RADIO_ACCESS)
          {
            rr_x2g_send_lte_redir_failure(gas_id);
          }
#endif /* FEATURE_LTE */
          output_event = RR_EV_W2G_GO_TO_INACTIVE;
          reset_internal_variables(gas_id);
        }
        else if (event == RR_EV_ABORT_PROCEDURE)
        {
          new_event = EV_NO_EVENT;
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_RRC;
        }
#ifdef FEATURE_GSM_TDS
        else if (event == RR_EV_T2G_REDIR_ABORT_PROCEDURE)
        {
          new_event = EV_NO_EVENT;
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_RRC;
        }
#endif /*FEATURE_GSM_TDS*/
        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in GOING_INACTIVE_DUE_TO_TIMER_EXPIRY");
          output_event = EV_PANIC_RESET_CNF;
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event), state_name(rr_x2g_redir_data_ptr->state));
        }
      }
      break;

      case STATE_GOING_INACTIVE_DUE_TO_NO_BCCH_FOUND:
      {
        if (event == EV_GSM_MODE_DEACTIVATED)
        {
#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
          if (rr_x2g_redir_data_ptr->source_rat == SYS_RAT_UMTS_RADIO_ACCESS)
#endif /* FEATURE_LTE || FEATURE_GSM_TDS*/
          {
#ifdef FEATURE_WCDMA
            if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_SEARCH ||
                rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_OUT_OF_SERVICE_SEARCH)
            {
              rr_send_interrat_redirect_rej(
                RR_RRC_REDIRECT_FAIL_NO_SUITABLE_CELL_FOUND,    // failure_reason
                rr_get_available_plmn_db_ptr(gas_id),           // available_plmn_db
                gas_id                                          // gas_id
              );
            }
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
            else if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_SERVICE_SEARCH)
            {
              if (rr_x2g_redir_data_ptr->camping_is_possible)
              {
                rr_send_interrat_redirect_rej_service_search_success(gas_id);
              }
              else
              {
                rr_send_interrat_redirect_rej_service_search_failure(gas_id);
              }
            }
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */
#endif /* FEATURE_WCDMA */
          }
#ifdef FEATURE_GSM_TDS
          else if (rr_x2g_redir_data_ptr->source_rat == SYS_RAT_TDS_RADIO_ACCESS)
          {
            if (rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_REDIRECTION_SEARCH ||
                rr_x2g_redir_data_ptr->type == RR_X2G_REDIR_TYPE_OUT_OF_SERVICE_SEARCH)
            {
              rr_send_tds_interrat_redirect_rej(
                TDSRR_REDIRECT_FAIL_NO_SUITABLE_CELL_FOUND,   // failure_reason
                rr_get_available_plmn_db_ptr(gas_id),         // available_plmn_db
                gas_id
              );
            }
          }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
          else if (rr_x2g_redir_data_ptr->source_rat == SYS_RAT_LTE_RADIO_ACCESS)
          {
            rr_x2g_send_lte_redir_failure(gas_id);
          }
#endif /* FEATURE_LTE */

          output_event = RR_EV_W2G_GO_TO_INACTIVE;
          reset_internal_variables(gas_id);
        }
        else if (event == RR_EV_ABORT_PROCEDURE)
        {
          new_event = EV_NO_EVENT;
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_RRC;
        }
#ifdef FEATURE_GSM_TDS
        else if (event == RR_EV_T2G_REDIR_ABORT_PROCEDURE)
        {
          new_event = EV_NO_EVENT;
          rr_x2g_redir_data_ptr->state = STATE_GOING_INACTIVE_DUE_TO_RRC;
        }
#endif /*FEATURE_GSM_TDS*/
        else if (event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("X2G Redir: Panic Reset in GOING_INACTIVE_DUE_TO_NO_BCCH_FOUND");
          output_event = EV_PANIC_RESET_CNF;
        }
        else if (event == RR_EV_CANDIDATE_CELL_SI_RECEIVED)
        {
          ;  // This is received here if SI3/SI4 was received indicating that the cell is barred
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event), state_name(rr_x2g_redir_data_ptr->state));
        }
      }
      break;

      default:
      {
        log_unexpected_state(rr_x2g_redir_data_ptr->state)
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((event != new_event) && (new_event != EV_NO_EVENT)) ||
        (rr_x2g_redir_data_ptr->old_state != rr_x2g_redir_data_ptr->state))
    {
      rr_store_trace_msg_buf(
        RR_W2G_SERVICE_REDIRECTION_SM,
        new_event,
        (byte) rr_x2g_redir_data_ptr->state,
        new_message,
        gas_id
      );
    }
#endif /* DEBUG_RR_TRACE_MSG */

    if (rr_x2g_redir_data_ptr->old_state != rr_x2g_redir_data_ptr->state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_x2g_redir_control",
        rr_x2g_redir_data_ptr->old_state,
        rr_x2g_redir_data_ptr->state,
        state_name,
        gas_id
      );
      if(rr_x2g_redir_data_ptr->state == STATE_WAITING_FOR_ACTIVATION_RSP)
      {
        if(!rr_timer_is_running(RR_MM_ACTIVATION_RSP_TIMER, gas_id))
        {
          MSG_GERAN_HIGH_0_G("Start a timer for RR_ACTIVATION_RSP ");
		  (void)rr_timer_start(RR_MM_ACTIVATION_RSP_TIMER, RR_MM_ACTIVATION_RSP_TIMEOUT, gas_id);          
        }		  
      }
	  else if(rr_timer_is_running(RR_MM_ACTIVATION_RSP_TIMER, gas_id))
	  {
        MSG_GERAN_HIGH_0_G("Timer stopped RR_ACTIVATION_RSP rcvd ");	  
	    (void)rr_timer_cancel(RR_MM_ACTIVATION_RSP_TIMER, gas_id);
	  }
      rr_x2g_redir_data_ptr->old_state = rr_x2g_redir_data_ptr->state;
    }

    if (new_event != EV_NO_EVENT)
    {
      event = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event = EV_NO_EVENT;
    }
  }

  return output_event;
}

#ifdef FEATURE_LTE
/*!
 * \brief Sends redirection failure to LTE
 *
 * \param gas_id
 *
 * \return None
 */
void rr_x2g_send_lte_redir_failure(const gas_id_t gas_id)
{
  geran_grr_lte_redir_failed_rsp_t lte_redir_failed_rsp;
  errno_enum_type                  result;

  memset(&lte_redir_failed_rsp, 0, sizeof(lte_redir_failed_rsp));

  msgr_init_hdr(&lte_redir_failed_rsp.hdr,
                MSGR_GERAN_GRR,
                GERAN_GRR_LTE_REDIR_FAILED_RSP);

  result = rr_msgr_send(&lte_redir_failed_rsp.hdr, sizeof(lte_redir_failed_rsp), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
  return;
}
#endif /* FEATURE_LTE */

/*!
 * \brief Returns a pointer to rr_w2g_redir_cell_list.
 *
 * \param gas_id
 *
 * \return rr_reselection_pscan_cell_list_T*
 */
rr_reselection_pscan_cell_list_T *rr_get_w2g_redirection_cell_list_ptr(
  const gas_id_t gas_id
)
{
  return rr_x2g_redir_get_cell_list_ptr(gas_id);
}

#else

void rr_w2g_service_redirection_dummy_func(void)
{
  return;
}

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
