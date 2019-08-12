/*! \file rr_sys_info_main.c

  Contains functionality related to System Information.

                Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info_main.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_general.h"
#include "rr_gprs_defs.h"
#include "rr_sys_info_i.h"
#include "rr_control.h"
#include "rr_l1_send.h"
#include "rr_mm_send.h"
#include "rr_sys_info_3g.h"
#include "rr_mac_send.h"
#include "rr_sys_info_si.h"
#include "rr_sys_info_pacch.h"
#include "rr_sys_info_pcell.h"
#include "aconfig.h"
#include "rr_resel_utils.h"
#include "gprs_mem.h"
#include "rr_sys_info_ncell.h"
#include "rr_resel_nacc.h"
#include "rr_multi_sim.h"
#include "rr_lsm_if.h"
#include "rr_cell_selection.h"
#include "rr_l1_idle_mode.h"
#include "rr_ps_access.h"
#include "rr_nv.h"
#include "grr_mutex.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  SIST_NOT_CAMPED,
  SIST_BSI,
  SIST_DISABLED
} si_acquisition_state_t;

#define MAX_PENDING_EVENTS 8

typedef struct
{
  si_acquisition_state_t            state;
  si_acquisition_state_t            old_state;
  gprs_sys_info_update_identities_t sys_info_update_identities_blocked;
  rr_event_T                        pending_events[MAX_PENDING_EVENTS];
  uint8                             num_of_pending_events;
} rr_sys_info_main_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

#define _RR_SYS_INFO_MAIN_INIT { \
                                SIST_NOT_CAMPED, /*state*/ \
                                SIST_NOT_CAMPED  /*old_state*/ \
                              }

static rr_sys_info_main_data_t rr_sys_info_main_data[NUM_GERAN_DATA_SPACES] =
                                          { INITIAL_VALUE(_RR_SYS_INFO_MAIN_INIT) };
#undef _RR_SYS_INFO_MAIN_INIT

static gprs_system_info_database_t *gprs_system_info_database_ptrs[NUM_GERAN_DATA_SPACES] = {0};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id (in)
 *
 * \return rr_sys_info_main_data_t*
 */
static rr_sys_info_main_data_t *rr_sys_info_main_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_sys_info_main_data[as_index];
}

/*!
 * \brief Returns a text string representing the given module state.
 *
 * \param state (in)
 *
 * \return char*
 */
static char *get_sys_info_acquisition_control_state_name(
  si_acquisition_state_t state
)
{
  char *s;

  switch (state)
  {
    case SIST_NOT_CAMPED:
    {
      s = "NOT_CAMPED";
      break;
    }

    case SIST_BSI:
    {
      s = "BSI";
      break;
    }

    case SIST_DISABLED:
    {
      s = "DISABLED";
      break;
    }

    default:
    {
      s = "?";
    }
  }

  return s;

} /* end get_sys_info_acquisition_control_state_name() */

/*!
 * \brief Indicates if updates to the specified layer are allowed during a sys info refresh.
 *
 * \param update_identity (in)
 * \param gas_id (in)
 *
 * \return boolean - TRUE if updates to the specified layer are allowed during a sys info refresh
 */
static boolean rr_gprs_sys_info_update_allowed(
  gprs_sys_info_update_identities_t update_identity,
  const gas_id_t gas_id
)
{
  rr_sys_info_main_data_t *rr_sys_info_main_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_main_data_ptr = rr_sys_info_main_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_main_data_ptr);

  if (rr_sys_info_main_data_ptr->sys_info_update_identities_blocked & update_identity)
  {
    return FALSE;
  }

  return TRUE;

} /* end rr_gprs_sys_info_updates_allowed() */

/*!
 * \brief locks updates from sys info changes to the specified layer during a sys info refresh.
 *
 * \param update_identities (in)
 * \param gas_id (in)
 */
static void rr_gprs_sys_info_block_updates(
  gprs_sys_info_update_identities_t update_identities,
  const gas_id_t gas_id
)
{
  rr_sys_info_main_data_t *rr_sys_info_main_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_main_data_ptr = rr_sys_info_main_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_main_data_ptr);

  rr_sys_info_main_data_ptr->sys_info_update_identities_blocked = update_identities;

} /* rr_gprs_sys_info_block_updates() */

/*!
 * \brief Indicates if the L1 Idle Mode needs to change to match the current cell config.
 *
 * \param gas_id (in)
 *
 * \return grr_siev_t - {SIEV_NO_EVENT, SIEV_CHANGE_L1_IDLE_MODE}
 */
static grr_siev_t check_for_l1_idle_mode_change(
  const gas_id_t gas_id
)
{
  grr_l1_idle_mode_t  l1_idle_mode, new_l1_idle_mode;

  grr_siev_t          siev = SIEV_NO_EVENT;
  gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

  // L1's current idle mode is NONE, GSM, or GPRS CCCH
  l1_idle_mode = cell_info->l1_idle_mode;

  // This will return GRR_L1_MODE_GSM or GRR_L1_IDLE_MODE_CCCH depending if the cell supports GPRS
  new_l1_idle_mode = grr_determine_l1_idle_mode(cell_info);

  if (new_l1_idle_mode != l1_idle_mode)
  {
    MSG_GERAN_HIGH_0_G("Change in L1 idle mode required");

    siev = SIEV_CHANGE_L1_IDLE_MODE;
  }

  return siev;

} /* end check_for_l1_idle_mode_change() */

/****************************************************************************
 *   Function name:  rr_data_free_scell_dedicated_mode_params
 *   --------------------------
 *
 *   Description: De allocate the heap assigned for GPRS scell dedicated meas
 *                parameters.
 *
 *   Parameters: gprs_scell_info_t * scell_info_ptr,gas_id
 *   -----------
 *
 *   Returns:
 *   --------
 *
 ****************************************************************************/
static void rr_data_free_scell_dedicated_mode_params(gprs_scell_info_t * scell_info_ptr,const gas_id_t gas_id)
{
  if (scell_info_ptr->gsm.dedicated_mode_params != NULL)
  { 
    // de allocate the heap assigned for dedicated  mode params
    GPRS_MEM_FREE(scell_info_ptr->gsm.dedicated_mode_params);
    scell_info_ptr->gsm.dedicated_mode_params = NULL;
    MSG_GERAN_HIGH_1_G("Freeing %d bytes for scell dedicated mode parameters",
      sizeof(rr_dedicated_mode_params_t)
    );
  }
} /* end rr_data_free_scell_dedicated_mode_params() */

/*-----------------------------------------------------------------------------
 * Externalized Function Definitions
 * --------------------------------------------------------------------------*/

/*!
 * \brief Initialises the system information database.
 *
 * \param gas_id (in)
 */
void rr_init_gprs_sys_info_database(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr;
  gprs_scell_info_t *pcell_info_ptr;
  gprs_ncells_info_t *ncell_info_ptr;
  rr_sys_info_main_data_t *rr_sys_info_main_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_main_data_ptr = rr_sys_info_main_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_main_data_ptr);

  scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_info_ptr);

  pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
  RR_NULL_CHECK_FATAL(pcell_info_ptr);

  ncell_info_ptr = rr_gprs_get_ncells_info(gas_id);
  RR_NULL_CHECK_FATAL(ncell_info_ptr);

  rr_clear_gprs_cell_info(scell_info_ptr);
  rr_clear_gprs_cell_info(pcell_info_ptr);
  memset( ncell_info_ptr, 0, sizeof( gprs_ncells_info_t ) );

  rr_init_pcell_data(gas_id);
  rr_init_ncell_data(gas_id);
  rr_init_sys_info_si(gas_id);
#ifdef FEATURE_WCDMA
  rr_init_sys_info_3g(gas_id);
#endif /* FEATURE_WCDMA */
  rr_sys_info_pacch_init(gas_id);

  rr_sys_info_main_data_ptr->sys_info_update_identities_blocked = 0;

  /* Reset all the sys info state machines */
  (void) rr_gprs_sys_info_acquisition_control(EV_RESET_SOFTWARE, NULL, gas_id);

} /* end rr_init_gprs_sys_info_database() */

/*!
 * \brief Converts RR events to SI events.
 *
 * \param rr_event
 *
 * \return grr_siev_t
 */
grr_siev_t grr_rrev_to_siev(
  rr_event_T rr_event
)
{
  switch (rr_event)
  {
    case EV_RESET_SOFTWARE:                           return SIEV_RESET_SOFTWARE;
    case EV_INPUT_MESSAGE:                            return SIEV_INPUT_MESSAGE;
    case EV_GPRS_SUSPENDED_FOR_SI_ACQUISITION:        return SIEV_GPRS_SUSPENDED;
    case RR_EV_CAMPED_ON_CELL:                        return SIEV_CAMPED_ON_CELL;
    case EV_SI_SYS_INFO_MESSAGE_RECEIVED:             return SIEV_SYS_INFO_MESSAGE_RECEIVED;

    default: break;
  }

  return SIEV_NO_EVENT;

} /* end rrev_to_siev() */

/*!
 * \brief Converts SI events to RR events.
 *
 * \param siev
 *
 * \return rr_event_T
 */
rr_event_T grr_siev_to_rrev(
  grr_siev_t siev
)
{
  switch (siev)
  {
    case SIEV_GPRS_SUSPEND:                      return EV_GPRS_SUSPEND_FOR_SI_ACQUISITION;
    case SIEV_GPRS_SUSPEND_NO_SI13:              return EV_GPRS_SUSPEND_FOR_SI_ACQUISITION_NO_SI13;
    case SIEV_SYS_INFO_ACQUISITION_FAILURE:      return RR_EV_ACQUISITION_FAILURE;
    case SIEV_CAMPED_ON_CELL:                    return RR_EV_CAMPED_ON_CELL;
    case SIEV_CHANGE_L1_IDLE_MODE:               return RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED;
    case SIEV_SYS_INFO_EXPIRY_DURING_CS_CONN:    return RR_EV_SYS_INFO_EXPIRY_DURING_CS_CONN;

    default: break;
  }

  return EV_NO_EVENT;

} /* end grr_siev_to_rrev() */

/*!
 * \brief Converts GRR timer IDs to SI events (for when timers expire).
 *
 * \param timer_id
 *
 * \return grr_siev_t
 */
grr_siev_t grr_timerid_to_siev(
  rr_timer_id_T timer_id
)
{
  switch (timer_id)
  {
    case GRR_SYS_INFO_REFRESH_TIMER:        return SIEV_SYS_INFO_REFRESH_TIMER_EXPIRY;
    case GRR_SYS_INFO_RETRY_TIMER:          return SIEV_SYS_INFO_RETRY_TIMER_EXPIRY;
    case GRR_SYS_INFO_ACQUISITION_TIMER:    return SIEV_SYS_INFO_ACQUISITION_TIMER_EXPIRY;

    default: break;
  }

  return SIEV_NO_EVENT;

} /* end grr_timerid_to_siev() */

/*!
 * \brief Starts the sys info refresh timer.
 *
 * \param gas_id
 */
void rr_gprs_start_sys_info_refresh_timer(const gas_id_t gas_id)
{
  rex_timer_cnt_type timer_duration;

  if (rr_timer_is_running(GRR_SYS_INFO_REFRESH_TIMER, gas_id))
  {
    (void)rr_timer_cancel(GRR_SYS_INFO_REFRESH_TIMER, gas_id);
  }

  if (rr_is_sglte_mode_active(gas_id))
  {
    /* for SGLTE (or SxLTE+G), use 120 seconds */
    timer_duration = GRR_TIMEOUT_120_SECONDS;
  }
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  else if (geran_in_multi_standby_mode())
  {
    /* DSDS or TSTS mode. */
    timer_duration = rr_nv_get_multisim_si_refresh_duration_milliseconds(RR_GAS_ID_TO_AS_ID);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  else
  {
    /* single standby or DSDA, without SGLTE, use 30 seconds as per spec */
    timer_duration = GRR_TIMEOUT_30_SECONDS;
  }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (geran_in_multi_standby_mode() &&
      rr_ms_sys_info_refresh_blocked(gas_id))
  {
    MSG_GERAN_MED_0_G("Sys info refresh timer not started, disabled");
  }
  else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  {
    if (rr_timer_start(GRR_SYS_INFO_REFRESH_TIMER, timer_duration, gas_id))
    {
      MSG_GERAN_MED_1_G("Started sys info refresh timer (%d)", (int)timer_duration);
    }
  }

  return;
} /* end rr_gprs_start_sys_info_refresh_timer() */

/*!
 * \brief Stops the sys info refresh timer.
 *
 * \param gas_id (in)
 */
void rr_gprs_stop_sys_info_refresh_timer(const gas_id_t gas_id)
{
#ifdef FEATURE_FAKE_BTS_CELL_INFO
  gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);
#endif /*FEATURE_FAKE_BTS_CELL_INFO*/
  if (rr_timer_cancel(GRR_SYS_INFO_REFRESH_TIMER, gas_id))
  {
    MSG_GERAN_MED_0_G("Stopped sys info refresh timer");
#ifdef FEATURE_FAKE_BTS_CELL_INFO
  if (rr_get_fake_bs_detection_enabled(gas_id) == TRUE)
  {
    scell->bcch_sys_info.si_refresh_ongoing = FALSE;
  }
#endif /*FEATURE_FAKE_BTS_CELL_INFO*/
  }

  return;
} /* end rr_gprs_stop_sys_info_refresh_timer() */

/*!
 * \brief Starts the sys info acquisition timer.
 *
 * Note: If the timer is already running, it is NOT restarted from zero, but will continue to run from its
 * current value
 *
 * \param timeout_ms (in)
 * \param gas_id (in)
 */
void rr_gprs_start_sys_info_acquisition_timer(
  uint32 timeout_ms,
  const gas_id_t gas_id
)
{
  if (!rr_timer_is_running(GRR_SYS_INFO_ACQUISITION_TIMER, gas_id))
  {
    MSG_GERAN_MED_1_G("Start sys info acquisition timer (%d)",timeout_ms);

    (void)rr_timer_start(GRR_SYS_INFO_ACQUISITION_TIMER, timeout_ms, gas_id);
  }

} /* end rr_gprs_start_sys_info_acquisition_timer() */

/*!
 * \brief Stops the sys info acquisition timer.
 *
 * \param gas_id
 */
void rr_gprs_stop_sys_info_acquisition_timer(
  const gas_id_t gas_id
)
{
  if (rr_timer_cancel(GRR_SYS_INFO_ACQUISITION_TIMER, gas_id))
  {
    MSG_GERAN_MED_0_G("Stopped sys info acquisition timer");
  }
} /* end rr_gprs_stop_sys_info_acquisition_timer() */

/*!
 * \brief Starts the sys info retry timer. If the timer is running, it is stopped and restarted.
 *
 * \param timeout_ms (in)
 * \param gas_id (in)
 */
void rr_gprs_start_sys_info_retry_timer(
  uint32 timeout_ms,
  const gas_id_t gas_id
)
{
  if (rr_timer_cancel(GRR_SYS_INFO_RETRY_TIMER, gas_id))
  {
    MSG_GERAN_MED_1_G("Restarting sys info retry timer (%d)",timeout_ms);
  }
  else
  {
    MSG_GERAN_MED_1_G("Start sys info retry timer (%d)",timeout_ms);
  }

  (void)rr_timer_start(GRR_SYS_INFO_RETRY_TIMER, timeout_ms, gas_id);

} /* end rr_gprs_start_sys_info_retry_timer() */

/*!
 * \brief Stops the sys info refresh timer.
 *
 * \param gas_id
 */
void rr_gprs_stop_sys_info_retry_timer(
  const gas_id_t gas_id
)
{
  if (rr_timer_cancel(GRR_SYS_INFO_RETRY_TIMER, gas_id))
  {
    MSG_GERAN_MED_0_G("Stopped sys info retry timer");
  }
} /* end rr_gprs_stop_sys_info_refresh_timer() */

/*!
 * \brief Activates idle mode sys info refresh cycle.
 *
 * \param gas_id
 */
void rr_gprs_activate_camped_sys_info_handling(
  const gas_id_t gas_id
)
{
  (void) rr_gprs_sys_info_acquisition_control(RR_EV_CAMPED_ON_CELL, NULL, gas_id);
  (void) rr_gprs_sys_info_acquisition_control(EV_SI_ACTIVATE_CAMPED_HANDLING, NULL, gas_id);

} /* rr_gprs_activate_camped_sys_info_handling() */

/*!
 * \brief Suspends the idle mode sys info refresh cycle. Any acquisition in progress is aborted.
 *
 * \param auto_retry (in) - if TRUE sys info should be re-requested automatically after 5s
 * \param gas_id (in)
 */
void rr_gprs_suspend_camped_sys_info_handling(
  boolean auto_retry,
  const gas_id_t gas_id
)
{
  if ( auto_retry )
  {
    rr_gprs_start_sys_info_retry_timer(GRR_TIMEOUT_1_SECOND * 5, gas_id);
    (void) rr_gprs_sys_info_acquisition_control(EV_RR_EST_REQ, NULL, gas_id );
  }
  else
  {
    rr_gprs_stop_sys_info_retry_timer(gas_id);
    rr_gprs_stop_sys_info_acquisition_timer(gas_id);
    (void) rr_gprs_sys_info_acquisition_control(EV_RR_EST_REQ, NULL, gas_id);
  }

} /* rr_gprs_suspend_camped_sys_info_handling() */

/*!
 * \brief Resume idle mode sys info refresh cycle, after it had been previously suspended (see above).
 *
 * \param gas_id
 */
void rr_gprs_resume_camped_sys_info_handling(
  const gas_id_t gas_id
)
{
  if (rr_timer_is_running(GRR_SYS_INFO_RETRY_TIMER, gas_id))
  {
    MSG_GERAN_HIGH_0_G("Resuming sys info handler early");

    /* "Expire" the GRR_SYS_INFO_RETRY_TIMER - so stop it, and then */
    /* call the timer expiry handler as if it had expired */
    rr_gprs_stop_sys_info_retry_timer(gas_id);
    (void) rr_gprs_sys_info_timer_handler(GRR_SYS_INFO_RETRY_TIMER, gas_id);
  }

} /* end rr_gprs_resume_camped_sys_info_handling() */

/*!
 * \brief Deactivate the idle mode sys info refresh cycle.
 *
 * \param gas_id (in)
 */
void rr_gprs_deactivate_timed_sys_info_refresh(
  const gas_id_t gas_id
)
{
  rr_gprs_stop_sys_info_refresh_timer(gas_id);
  rr_gprs_stop_sys_info_acquisition_timer(gas_id);
  rr_gprs_stop_sys_info_retry_timer(gas_id);

  (void) bsi_acquisition_control(SIEV_STOP_AND_RESET_SOFTWARE, NULL, gas_id);
  (void) rr_gprs_sys_info_acquisition_control(EV_RESET_SOFTWARE, NULL, gas_id);

  /*
  If L1 is in EGPRS Test Mode, then disable the Sys Info Handler by indicating camping,
  which makes it check the L1 Test Mode status, and it moves to DISABLED
  */
  if (grr_get_l1_in_test_mode(gas_id))
  {
    (void) rr_gprs_sys_info_acquisition_control(RR_EV_CAMPED_ON_CELL, NULL, gas_id);
  }

} /* end rr_gprs_deactivate_timed_sys_info_refresh() */

/*!
 * \brief Setup sys info data and handlers following a handover.
 *
 * \param arfcn (in)
 * \param bsic (in)
 * \param gas_id (in)
 */
void rr_gprs_setup_sys_info_after_handover(
  ARFCN_T arfcn,
  BSIC_T bsic,
  const gas_id_t gas_id
)
{
  byte rac;
#ifdef FEATURE_WCDMA
  gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
#endif /* FEATURE_WCDMA */
  gprs_scell_info_t *gprs_info = rr_gprs_get_scell_info(gas_id);

  /*
  Decativate the sys info handlers - this will stop timers, and reset any sys info
  message tracking data
  Note: This does not mark any of the GSM or GPRS sys info invalid
  */
  rr_gprs_deactivate_timed_sys_info_refresh(gas_id);

  /* Mark GSM idle & dedicated sys info as no longer valid (due to the handover) */
  gprs_info->gsm.valid_data_flag = FALSE;
  gprs_info->gsm.valid_dedicated_data_flag = FALSE;
  gprs_info->gsm.invalidate_si_reason = RR_SI_HANDOVER;
  gprs_info->gsm.cell_identity_valid_flag = FALSE;
  
  /*
   By default GRR should use the default GPRS INdicator flag as CS+PS, so as to make sure that
   when SI6 is received RR_SERVICE_IND_OTA will have the cap as CS+PS, if DTM support is there
   then GPRS SYINFO present will have right value else it will be set to FALSE 
   */
  gprs_info->gsm.gprs_indicator = TRUE;
  
  rr_initialize_SI2ter_instances(gas_id);

 #ifdef FEATURE_SGLTE
 if(rr_is_sglte_mode_active(gas_id) &&
  (ARFCNS_EQUAL(arfcn,rr_sglte_get_stored_serv_ARFCN(gas_id))) &&
    rr_sglte_saved_l2g_ho_being_processed(gas_id)
   )
  {    
    gprs_info->gsm.gprs_indicator = rr_sglte_get_stored_serv_gprs_ind(gas_id);
    MSG_GERAN_HIGH_1_G("SGLTE SRVCC SI2q instance will not be cleared , GPRS ind = %d",gprs_info->gsm.gprs_indicator);
  }
  else
 #endif /*FEATURE_SGLTE*/
  {
    rr_initialize_SI2q_instances(gas_id);
  }

  /* Clear the GSM BSIC Description store as it is no longer valid. */
  memset(
    &gprs_info->gsm.gsm_bsic_description,
    0,
    sizeof(rr_gsm_bsic_description_T)
  );

  /* Setup the ARFCN & BSIC of the new serving cell */
  gprs_info->gsm.BSIC = bsic;
  gprs_info->gsm.BCCH_ARFCN = arfcn;
  rr_set_current_band(GET_ARFCN_BAND(arfcn), gas_id);

  /* Note the previous RAC value */
  rac = gprs_info->bcch_sys_info.routing_area_code;

  /**
   * The GPRS parameters for the new cell are not known, until SI6 / DTM INFORMATION is received (if the cell 
   * supports DTM). Clearing the GPRS Cell Options also means that the EGPRS indication doesn't get "stuck" 
   * indicating EGPRS are supported. 
   */
  gprs_info->bcch_sys_info.si13_data_valid_flag = FALSE;
  RR_ZERO_STRUCTURE(gprs_info->bcch_sys_info.gprs_cell_options);

  /**
   * In case the handover is from a DTM-cell to a non-DTM cell, the RAC is preserved, so that the 
   * RR_SERVICE_IND_FROM_OTA_MSG contains a valid RAC value in the gprs_sys_info structure, as this structure 
   * remains valid even if the GPRS paramters are not known, so that the dtm_supported flag can be accessed. 
   */
  gprs_info->bcch_sys_info.routing_area_code = rac;

  /*
  Set the l1_idle_mode to indicate GSM. This handover could have been W->G, in which
  case the l1_idle_mode would be NONE, or could be G->G, in which case l1_idle_mode would
  be set according to the idle mode used on the cell which the CS conn was established.
  The use of GSM over GPRS isn't really important - but this should be set to
  something other than NONE or PCCCH. This is because NONE will prevent any
  GPRS_SERVICE_CNF messages from GMM being serviced, and PCCCH doesn't really make
  sense as there is no way of knowing if the new cell supports PBCCH
  */
  gprs_info->l1_idle_mode = GRR_L1_IDLE_MODE_GSM;

  rr_eng_mode_update_scell_config(gas_id);


#ifdef FEATURE_GSM_TDS
  {
    rr_ded_irat_measurements_t *tds_ded_meas_ptr = rr_get_tds_ded_meas_ptr(gas_id);
#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
    rr_dedicated_data_T *dedic_ptr = rr_get_dedicated_data_ptr(gas_id);

    /* If Pseudo FR is active keep the TDS measurements during G->G HO as the Pseudo FR is not
       updated during HO*/
    if (dedic_ptr->tds_pseudo_fr.active == FALSE)
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/
    {
      rr_initialize_tds_ded_measurements(tds_ded_meas_ptr);
    }
  }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
  {
    wcdma_ncell_list_data_t       *wcdma_ncell_list_data_ptr = rr_get_wcdma_ncell_list_data_ptr(gas_id);
    surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);

    /* Since G2G handover is successful wipe out the WCDMA neighbors in RR
       L1 will retain its list and later when the MI is received RR will
       send the new list to L1. */
    rr_initialize_wcdma_ded_measurements(wcdma_ded_meas_ptr);
    rr_initialize_wcdma_neighbor_cell_list(wcdma_ncell_list_data_ptr->wcdma_neighbor_cell_list);

    /* Idle neighbor list based on SI2quater should also be made null since all
    the measurements may be happening based on this (before MI was received) */

    rr_initialize_wcdma_idle_neighbor_list(&gprs_info->gsm.utran_neighbor_list);
    rr_initialize_wcdma_idle_neighbor_list(&pcell_info_ptr->gsm.utran_neighbor_list);
  }
#endif /* FEATURE_WCDMA */

} /* rr_gprs_setup_sys_info_after_handover() */

/*!
 * \brief Invalidate the current sys info and records the invalidation reason.
 *
 * \param invalidate_reason (in)
 * \param gas_id (in)
 */
void rr_gprs_invalidate_sys_info_during_cs_conn(
  rr_invalidate_si_reason_t invalidate_reason,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

  MSG_GERAN_HIGH_1_G("Serving cell sys info invalidated (%d)",(int)invalidate_reason);

  cell_info->gsm.valid_data_flag = FALSE;
  cell_info->gsm.invalidate_si_reason = invalidate_reason;

  cell_info->bcch_sys_info.si13_data_valid_flag = FALSE;

  rr_initialize_SI2ter_instances(gas_id);
  rr_initialize_SI2q_instances(gas_id);

} /* end rr_gprs_invalidate_sys_info_during_cs_conn() */

/*!
 * \brief Checks whether the sys info is consistent with the L1 idle mode required.
 *
 * \param gas_id (in)
 *
 * \return boolean - TRUE if the sys info is valid & consistent, FALSE otherwise
 */
boolean grr_verify_sys_info_is_valid_for_l1_camp(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

  if ( cell_info->gsm.valid_data_flag )
  {
    if ( cell_info->gsm.gprs_indicator )
    {
      if ( ! cell_info->bcch_sys_info.si13_data_valid_flag )
      {
        cell_info->gsm.valid_data_flag = FALSE;
      }
    }

    if ( ! cell_info->gsm.valid_data_flag )
    {
      cell_info->gsm.invalidate_si_reason = RR_SI_INCONSISTENT;
    }
  }

  return cell_info->gsm.valid_data_flag;

} /* end grr_verify_sys_info_is_valid_for_l1_camp() */

/*!
 * \brief Called when idle mode sys info refresh acquisition completes.
 *
 * \param gas_id (in)
 *
 * \return grr_siev_t
 */
grr_siev_t rr_gprs_camped_sys_info_acquisition_complete(
  const gas_id_t gas_id
)
{
  grr_siev_t  siev;

  /* Stop the acquisition timer (if it's running) */
  rr_gprs_stop_sys_info_acquisition_timer(gas_id);

  siev = check_for_l1_idle_mode_change(gas_id);

  if (siev == SIEV_NO_EVENT)
  {
    // Unblock PS access (if required)
    if (rr_mac_ul_tbf_est_is_allowed(gas_id))
    {
      rr_grr_send_imsg_ps_access_req(
        FALSE,   // purge_gmm_signalling
        NULL,    // confirm_cb_fn
        gas_id   // gas_id
      );
    }
  }

  return siev;

} /* end rr_gprs_camped_sys_info_acquisition_complete() */

/*!
 * \brief Sys info timer handler.
 *
 * \param timer_id (in)
 * \param gas_id (in)
 *
 * \return rr_event_T
 */
rr_event_T rr_gprs_sys_info_timer_handler(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  rr_event_T  event_out = EV_NO_EVENT;

  if (rr_is_inactive(gas_id))
  {
    MSG_GERAN_ERROR_1_G("Ignoring timer expiry: %d, RR is not active", timer_id);
  }
  else
  {
    event_out = rr_gprs_bsi_acquisition_timer_handler(timer_id, gas_id);
  }

  if (event_out == RR_EV_SYS_INFO_EXPIRY_DURING_CS_CONN)
  {
    event_out = EV_NO_EVENT;
    rr_gprs_deactivate_timed_sys_info_refresh(gas_id);
  }

  return event_out;

} /* end rr_gprs_sys_info_timer_handler() */

/*!
 * \brief Initiates continuous reading of sys info if the paging mode is changed to PAGING REORGANISATION.
 *
 * \param old_page_mode (in)
 * \param new_page_mode (in)
 * \param gas_id (in)
 */
void rr_gprs_sys_info_new_paging_mode(
  paging_mode_T old_page_mode,
  paging_mode_T new_page_mode,
  const gas_id_t gas_id
)
{
  if ( new_page_mode == PAGING_REORGANIZATION )
  {
    bsi_enter_paging_reorg(gas_id);
  }
  else
  if ( old_page_mode == PAGING_REORGANIZATION )
  {
    bsi_exit_paging_reorg(gas_id);
  }

} /* end rr_gprs_sys_info_new_paging_mode() */

/*!
 * \brief Indicates if a idle mode sys info refresh is in progress.
 *
 * \param gas_id
 *
 * \return boolean - TRUE if a idle mode sys info refresh is in progress, FALSE otherwise.
 */
boolean rr_gprs_scell_sys_info_acquisition_in_progress(
  const gas_id_t gas_id
)
{
  boolean in_progress = rr_timer_is_running(GRR_SYS_INFO_ACQUISITION_TIMER, gas_id);

  /* If a sys info acquisition is currently temporarily suspended, then */
  /* treat this as if the acquisition is not in progress - this will allow */
  /* a surround read to be made, which otherwise may have been blocked */
  if (in_progress && rr_timer_is_running(GRR_SYS_INFO_RETRY_TIMER, gas_id))
  {
    in_progress = FALSE;
  }

  return in_progress;

} /* end rr_gprs_scell_sys_info_acquisition_in_progress() */

/*!
 * \brief Main sys info acquisition state machine.
 *
 * \param rr_event_in (in)
 * \param event_in_data (in)
 * \param gas_id (in)
 *
 * \return rr_event_T
 */
rr_event_T rr_gprs_sys_info_acquisition_control(
  rr_event_T rr_event_in,
  void *event_in_data,
  const gas_id_t gas_id
)
{
  rr_sys_info_main_data_t *rr_sys_info_main_data_ptr;
  rr_event_T  rr_event_out = EV_NO_EVENT;

  // Obtain a pointer to the module data
  rr_sys_info_main_data_ptr = rr_sys_info_main_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_main_data_ptr);

  /*****************************************************************/
  /* EV_RESET_SOFTWARE                                             */
  /*****************************************************************/
  if (rr_event_in == EV_RESET_SOFTWARE)
  {
    (void) bsi_acquisition_control(SIEV_RESET_SOFTWARE, NULL, gas_id);
    rr_sys_info_main_data_ptr->state = SIST_NOT_CAMPED;
    rr_event_in = EV_NO_EVENT;
  }

  switch (rr_sys_info_main_data_ptr->state)
  {
    /*********************************************************************/
    /* SIST_NOT_CAMPED                                                   */
    /* a) Initial state                                                  */
    /* b) Cell selection / reselection in progress                       */
    /* c) Acquiring PSIs after reselection to a cell with PBCCH          */
    /* d) Acquiring PSIs after PBCCH activated in the current cell       */
    /*********************************************************************/
    case SIST_NOT_CAMPED:

      /*****************************************************************/
      /* RR_EV_CAMPED_ON_CELL                                          */
      /* L1 has confirmed that it has started idle mode procedures.    */
      /* Change the state to reflect whether camped on BCCH or PBCCH   */
      /*****************************************************************/
      if (rr_event_in == RR_EV_CAMPED_ON_CELL)
      {
        if (!grr_get_l1_in_test_mode(gas_id))
        {
          MSG_GERAN_HIGH_0_G("Sys info acquisition handler camped-on");

          rr_sys_info_main_data_ptr->state = SIST_BSI;
        }
        else
        {
          MSG_GERAN_HIGH_0_G("Sys info acquisition handler disabled (test mode)");

          rr_sys_info_main_data_ptr->state = SIST_DISABLED;
        }
      }

      else
      /*****************************************************************/
      /* EV_RR_EST_REQ                                                 */
      /*****************************************************************/
      if (rr_event_in == EV_RR_EST_REQ)
      {
        ;  // this can be ignored
      }

      else
      /*****************************************************************/
      /* Pass all event/messages thru to cell selection                */
      /*****************************************************************/
      {
        rr_event_out = rr_selection_control(rr_event_in, event_in_data, gas_id);
      }

      break;

    /*********************************************************************/
    /* SIST_BSI                                                          */
    /* a) Camped on BCCH of cell (PBCCH not active, or GSM-only)         */
    /* b) Acquiring SIs after PBCCH deactivated in the current cell      */
    /* (Note: Abnormal Release with full sys info, sys info timeout, etc,*/
    /* on a cell with PBCCH is treated like PBCCH deactivation)          */
    /*********************************************************************/
    case SIST_BSI:

      /*****************************************************************/
      /* EV_SI_ACTIVATE_CAMPED_HANDLING                                */
      /*****************************************************************/
      if (rr_event_in == EV_SI_ACTIVATE_CAMPED_HANDLING)
      {
        bsi_activate_camped_sys_info_handling(gas_id);
      }

      else
      /*****************************************************************/
      /* EV_RR_EST_REQ                                                 */
      /*****************************************************************/
      if (rr_event_in == EV_RR_EST_REQ)
      {
        (void) bsi_acquisition_control(SIEV_SUSPEND_SYS_INFO_ACQUISITION, NULL, gas_id);
      }

      else
      /*****************************************************************/
      /* EV_ABNORMAL_RELEASE_PART_SI                                   */
      /*****************************************************************/
      if (rr_event_in == EV_ABNORMAL_RELEASE_PART_SI)
      {
        (void) bsi_acquisition_control(SIEV_SYS_INFO_PART_REFRESH, NULL, gas_id);
      }

      else
      /*****************************************************************/
      /* EV_ABNORMAL_RELEASE_FULL_SI                                   */
      /*****************************************************************/
      if (rr_event_in == EV_ABNORMAL_RELEASE_FULL_SI)
      {
        (void) bsi_acquisition_control(SIEV_SYS_INFO_FULL_REFRESH, NULL, gas_id);
      }

      else
      /*****************************************************************/
      /* Pass all other events/messages into BSI acquisiton control    */
      /*****************************************************************/
      {
        rr_event_out = rr_gprs_bsi_acquisition_control(rr_event_in, event_in_data, gas_id);
      }

      break;

    /*********************************************************************/
    /* SIST_DISABLED                                                     */
    /* No sys info acquisitions allowed                                  */
    /*********************************************************************/
    case SIST_DISABLED:

      /*****************************************************************/
      /* RR_EV_CAMPED_ON_CELL                                          */
      /*****************************************************************/
      if (rr_event_in == RR_EV_CAMPED_ON_CELL)
      {
        if (!grr_get_l1_in_test_mode(gas_id))
        {
          MSG_GERAN_HIGH_0_G("Sys info acquisition handler camped-on");

          rr_sys_info_main_data_ptr->state = SIST_BSI;
        }
      }
      break;

    default:
      MSG_GERAN_ERROR_1_G("Unknown SI state %d",(int)rr_sys_info_main_data_ptr->state);
      break;
  }

#ifdef DEBUG_RR_TRACE_MSG
  /* no old_state in this state machine */
  if ((rr_event_in != rr_event_out) && (rr_event_out != EV_NO_EVENT))
  {
    rr_store_trace_msg_buf(
      GRR_SYS_INFO_ACQ_SM,
      rr_event_out,
      (byte)rr_sys_info_main_data_ptr->state,
      event_in_data,
      gas_id
    );
  }
#endif

  if (rr_sys_info_main_data_ptr->state != rr_sys_info_main_data_ptr->old_state)
  {
    RR_LOG_STATE_TRANSITION(
      "sys_info_acquisition_control",
      rr_sys_info_main_data_ptr->old_state,
      rr_sys_info_main_data_ptr->state,
      get_sys_info_acquisition_control_state_name,
      gas_id
    );

    rr_sys_info_main_data_ptr->old_state = rr_sys_info_main_data_ptr->state;
  }

  return rr_event_out;

} /* end rr_gprs_sys_info_acquisition_control() */

/*!
 * \brief Checks the BCCH ARFCN supplied against the BCCH ARFCN of the scell.
 *
 * \param bcch_arfcn (in)
 * \param gas_id (in)
 *
 * \return boolean - TRUE if the supplied ARFCN matches that of the serving cell
 */
boolean rr_is_this_the_scell(
  ARFCN_T bcch_arfcn,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(scell_info_ptr);

  return (scell_info_ptr->gsm.camped_on_flag &&
          ARFCNS_SAME_CHANNEL_AND_BAND(scell_info_ptr->gsm.BCCH_ARFCN, bcch_arfcn));

} /* end rr_is_this_the_scell() */

/*!
 * \brief Returns the serving cell sys info.
 *
 * \param gas_id (in)
 *
 * \return gprs_scell_info_t*
 */
gprs_scell_info_t *rr_gprs_get_scell_info(const gas_id_t gas_id)
{
  gprs_system_info_database_t *gprs_si_database_ptr = rr_get_gprs_system_info_database_ptr(gas_id);

  RR_NULL_CHECK_FATAL(gprs_si_database_ptr);

  return &gprs_si_database_ptr->scell;

} /* end rr_gprs_get_scell_info() */

/*!
 * \brief Returns the ARFCN of the serving cell.
 *
 * \param gas_id (in)
 *
 * \return ARFCN_T*
 */
ARFCN_T rr_gprs_get_scell_arfcn(const gas_id_t gas_id)
{
  return rr_scell_arfcn(gas_id);

} /* end rr_gprs_get_scell_arfcn() */

/*!
 * \brief Returns the BSIC of the serving cell.
 *
 * \param gas_id (in)
 *
 * \return BSIC_T*
 */
BSIC_T rr_gprs_get_scell_bsic(const gas_id_t gas_id)
{
  return rr_scell_bsic(gas_id);

} /* end rr_gprs_get_scell_bsic() */

/*!
 * \brief Returns the PLMN ID of the serving cell.
 *
 * \param gas_id (in)
 *
 * \return PLMN_id_T
 */
PLMN_id_T rr_gprs_get_scell_plmn_id(const gas_id_t gas_id)
{
  return rr_scell_plmn_id(gas_id);
}

/*!
 * \brief Returns the GPRS Cell Options information for the serving cell.
 *
 * \param gas_id (in)
 *
 * \return gprs_cell_options_t*
 */
gprs_cell_options_t *rr_si_get_gprs_cell_options(const gas_id_t gas_id)
{
  gprs_scell_info_t *cell_info_ptr = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(cell_info_ptr);

  return &cell_info_ptr->bcch_sys_info.gprs_cell_options;

} /* end rr_si_get_gprs_cell_option() */

/*!
 * \brief Returns the Power Control Parameters information for the serving cell.
 *
 * \param gas_id (in)
 *
 * \return global_power_control_params_t*
 */
global_power_control_params_t *rr_si_get_power_control_params(const gas_id_t gas_id)
{
  gprs_scell_info_t *cell_info_ptr = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(cell_info_ptr);

  return &cell_info_ptr->bcch_sys_info.power_control_params;

} /* end rr_si_get_power_control_params() */

/*!
 * \brief Compares the supplied GPRS Cell Options against that stored for the current cell.
 *
 * \param gprs_cell_options_ptr (in)
 * \param gas_id (in)
 *
 * \return boolean - TRUE if the supplied GPRS Cell Options is different, FALSE otherwise
 */
boolean rr_si_diff_gprs_cell_options(
  gprs_cell_options_t *gprs_cell_options_ptr,
  const gas_id_t gas_id
)
{
  gprs_cell_options_t *scell_gprs_cell_options_ptr = rr_si_get_gprs_cell_options(gas_id);

  // If there is no SI data available, indicate the data is different
  RR_NULL_CHECK_RETURN_PARAM(scell_gprs_cell_options_ptr, TRUE);

  // Sanity-check the input paramters - if none supplied, indicate the data is unchanged
  RR_NULL_CHECK_RETURN_PARAM(gprs_cell_options_ptr, FALSE);

  if (memcmp(
        gprs_cell_options_ptr,        // Buf 1
        scell_gprs_cell_options_ptr,  // Buf 2
        sizeof(gprs_cell_options_t)   // Size
      ) == 0)
  {
    // indicates they are equal
    return FALSE;
  }

  // indicates they are different
  return TRUE;

} /* end rr_si_diff_gprs_cell_options() */

/*!
 * \brief Indicates if the RR/L1 are camped on a cell in Packet Idle mode.
 *
 * \param gas_id (in)
 *
 * \return boolean - TRUE if the MS is camped on a cell in Packet Idle mode, FALSE otherwise
 */
boolean rr_gprs_camped_on_ccch(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(scell_info_ptr);

  return (scell_info_ptr->gsm.camped_on_flag &&
          (scell_info_ptr->l1_idle_mode == GRR_L1_IDLE_MODE_CCCH));

} /* end rr_gprs_camped_on_ccch() */

/*!
 * \brief Indicates if the RR/L1 are camped on a cell in GSM Idle mode.
 *
 * \param gas_id (in)
 *
 * \return boolean - TRUE if the MS is camped on a cell in GSM Idle mode, FALSE otherwise
 */
boolean rr_gsm_camped_on_ccch(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(scell_info_ptr);

  return (scell_info_ptr->gsm.camped_on_flag &&
          (scell_info_ptr->l1_idle_mode == GRR_L1_IDLE_MODE_GSM));

} /* end rr_gsm_camped_on_ccch() */

/*!
 * \brief Indicates if the serving cell supports GPRS.
 *
 * \param gas_id (in)
 *
 * \return boolean - TRUE if the serving cell supports GPRS, FALSE otherwise
 */
boolean rr_cell_supports_gprs(const gas_id_t gas_id)
{
  boolean cell_supports_gprs = FALSE;

  if (rr_get_gprs_system_info_database_valid(gas_id))
  {
    cell_supports_gprs = rr_gprs_get_scell_info(gas_id)->gsm.gprs_indicator;
  }

  return cell_supports_gprs;

} /* end grr_cell_support_gprs() */

/*!
 * \brief Indicates if the serving cell supports EGPRS.
 *
 * \param gas_id (in)
 *
 * \return boolean - TRUE if the serving cell supports EGPRS, FALSE otherwise
 */
boolean rr_cell_supports_egprs(const gas_id_t gas_id)
{
  boolean cell_supports_egprs = FALSE;

  if (rr_get_gprs_system_info_database_valid(gas_id))
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

    cell_supports_egprs =
      scell_info_ptr->gsm.gprs_indicator &&
      (scell_info_ptr->bcch_sys_info.gprs_cell_options.optional_extension_info_flag == valid) &&
      scell_info_ptr->bcch_sys_info.gprs_cell_options.optional_extension_info.egprs_supported_flag;
  }

  return cell_supports_egprs;

} /* end rr_cell_support_egprs() */

#ifdef FEATURE_GSM_DTM
/*!
 * \brief Indicates if the serving cell supports DTM.
 *
 * \param gas_id (in)
 *
 * \return boolean - TRUE if the serving cell supports DTM, FALSE otherwise
 */
boolean rr_cell_supports_dtm(const gas_id_t gas_id)
{
  boolean cell_supports_dtm = FALSE;

  if (rr_get_gprs_system_info_database_valid(gas_id))
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

    /*
    Note: Ideally the check below should include
    (scell_info_ptr->bcch_sys_info.si13_data_valid_flag || scell_info_ptr->dtm_sys_info_valid) &&
    but this will not work in the case where the DTM support is enabled implicitly via the reception of a
    PACKET ASSIGNMENT / DTM ASSIGNMENT on a cell assumed not to support DTM
    (see [rr_dtm.c] dtm_supported_in_cell_implicitly())
    */
    cell_supports_dtm =
      scell_info_ptr->gsm.gprs_indicator &&
      (scell_info_ptr->bcch_sys_info.gprs_cell_options.optional_extension_info_flag == valid) &&
      scell_info_ptr->bcch_sys_info.gprs_cell_options.optional_extension_info.dtm_support;
  }

  return cell_supports_dtm;

} /* end rr_cell_support_dtm() */
#endif // FEATURE_GSM_DTM

/*!
 * \brief Indicates if the sys info for the serving cell is valid.
 *
 * \param gas_id (in)
 *
 * \return boolean - Returns TRUE if the sys info for the cell is valid; FALSE otherwise
 */
boolean rr_cell_si_valid(const gas_id_t gas_id)
{
  boolean si_valid = FALSE;

  if (rr_get_gprs_system_info_database_valid(gas_id))
  {
    si_valid = rr_gprs_get_scell_info(gas_id)->gsm.valid_data_flag;
  }

  return (si_valid);
}

/*!
 * \brief Populates the GPRS sys info structure for message to MM (e.g. RR_SERVICE_IND).
 *
 * \param mm_gprs_sys_info_ptr (out)
 * \param gas_id (in)
 *
 * \return boolean - TRUE if there is GPRS sys info available, FALSE otherwise
 */
boolean rr_gprs_get_sys_info_for_mm(
  mm_gprs_sys_info_T *mm_gprs_sys_info_ptr,
  const gas_id_t gas_id
)
{
#define MM_SGSNR_REV_97  0
#define MM_SGSNR_REV_99  1

  boolean gprs_params_available;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;

  RR_NULL_CHECK_FATAL(scell_info_ptr);

#ifdef FEATURE_GSM_DTM
  if (rr_in_dedicated(gas_id))
  {
    gprs_params_available = rr_get_nv_dtm_enabled(as_id);

    if (gprs_params_available)
    {
      if (rr_cell_supports_dtm(gas_id))
      {
        mm_gprs_sys_info_ptr->dtm_supported = TRUE;

        /* Always assume SGSNR R99 in DTM (3GPP CR G2-050457) */
        mm_gprs_sys_info_ptr->sgsnr = MM_SGSNR_REV_99;
      }
      else
      {
        mm_gprs_sys_info_ptr->dtm_supported = FALSE;

        if (scell_info_ptr->bcch_sys_info.sgsnr == SGSNR_REV_99)
        {
          mm_gprs_sys_info_ptr->sgsnr = MM_SGSNR_REV_99;
        }
        else
        {
          mm_gprs_sys_info_ptr->sgsnr = MM_SGSNR_REV_97;
        }
      }
    }
  }
  else
#endif /* FEATURE_GSM_DTM */
  {
    /*
    SI13 is acquired if SI3 indicates GPRS is supported - therefore all the GPRS SI should be available.
    However, during cell reestablishment, only minimal SIs are received, which doesn't include SI13.
    So need to check whether the GPRS paramters are available
    */
    gprs_params_available =
      rr_cell_supports_gprs(gas_id) && scell_info_ptr->bcch_sys_info.si13_data_valid_flag;

    if (gprs_params_available)
    {
      if (scell_info_ptr->bcch_sys_info.sgsnr == SGSNR_REV_99)
      {
        mm_gprs_sys_info_ptr->sgsnr = MM_SGSNR_REV_99;
      }
      else
      {
        mm_gprs_sys_info_ptr->sgsnr = MM_SGSNR_REV_97;
      }

#ifdef FEATURE_GSM_DTM
      mm_gprs_sys_info_ptr->dtm_supported =
        rr_get_nv_dtm_enabled(as_id) && rr_cell_supports_dtm(gas_id);
#endif
    }
  }

  if (gprs_params_available)
  {
    mm_gprs_sys_info_ptr->nmo = (network_op_mode_type)scell_info_ptr->bcch_sys_info.gprs_cell_options.nmo;
    mm_gprs_sys_info_ptr->rac = (routing_area_code_T)scell_info_ptr->bcch_sys_info.routing_area_code;

    mm_gprs_sys_info_ptr->egprs_supported =
      rr_get_nv_egprs_enabled(as_id) && rr_cell_supports_egprs(gas_id);

    MSG_GERAN_MED_3_G(
      "NMO: %d, SGSNR: %d, MSCR: %d",
      (int)mm_gprs_sys_info_ptr->nmo,
      (int)mm_gprs_sys_info_ptr->sgsnr,
      (int)scell_info_ptr->gsm.control_channel_description.MSCR
    );

    {
      boolean temp_egprs_supported = FALSE;
      boolean temp_dtm_supported = FALSE;

    temp_egprs_supported = mm_gprs_sys_info_ptr->egprs_supported;
#ifdef FEATURE_GSM_DTM
    temp_dtm_supported = mm_gprs_sys_info_ptr->dtm_supported;
#endif

      MSG_GERAN_MED_2_G(
        "EGPRS: %d, DTM: %d",
        (int)temp_egprs_supported,
        (int)temp_dtm_supported
      );
    }
  }
  else
  {
    MSG_GERAN_MED_0_G("No GPRS parameters available");
  }

  return gprs_params_available;

} /* rr_gprs_get_sys_info_for_mm() */

/*!
 * \brief Returns the NMO value of the given cell.
 *
 * \param cell_info (in)
 * \param gas_id (in)
 *
 * \return network_op_mode_type - NETWORK_OP_MODE_[1|2|3]; PS_DOMAIN_SYS_INFO_UNAVAILABLE
 */
network_op_mode_type rr_si_get_nmo(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *cell_info_ptr = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(cell_info_ptr);

  if (grr_is_ps_allowed_by_service_domain(gas_id))
  {
    if (cell_info_ptr->bcch_sys_info.si13_data_valid_flag)
    {
      return (network_op_mode_type) cell_info_ptr->bcch_sys_info.gprs_cell_options.nmo;
    }
  }

  return PS_DOMAIN_SYS_INFO_UNAVAILABLE;

} /* end grr_get_nmo() */

/*!
 * \brief Returns the DRX timer values from the encoded representation in sys info.
 *
 * \param enc_drx_timer (in)
 *
 * \return uint8 - DRX timer value in ms
 */
uint8 rr_gprs_decode_drx_timer(uint8 enc_drx_timer)
{
  const uint8 drx_lookup[8] = {0,1,2,4,8,16,32,64};

  return drx_lookup[enc_drx_timer];

} /* end rr_gprs_decode_drx_timer() */

/*!
 * \brief Returns the MS non-drx timer value, which is the minimum of the DRX_TIMER_MAX value from sys info
 *        and NON_DRX_TIMER value in NV.
 *
 * \param drx_timer_max (in)
 * \param gas_id (in)
 *
 * \return uint8 - non-DRX timer value in ms
 */
uint8 rr_gprs_calc_non_drx_timer(
  uint8 drx_timer_max,
  const gas_id_t gas_id
)
{
  uint8 non_drx_timer, ms_non_drx_timer;

  non_drx_timer = rr_get_nv_non_drx_timer(RR_GAS_ID_TO_AS_ID);

  ms_non_drx_timer = MIN(non_drx_timer, drx_timer_max);

  return ms_non_drx_timer;

} /* end rr_gprs_calc_non_drx_timer() */

/*!
 * \brief Allows updates from sys info changes to all layers during a sys info refresh.
 *
 * \param gas_id (in)
 */
void rr_gprs_sys_info_allow_all_updates(const gas_id_t gas_id)
{
  rr_sys_info_main_data_t *rr_sys_info_main_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_main_data_ptr = rr_sys_info_main_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_main_data_ptr);

  rr_sys_info_main_data_ptr->sys_info_update_identities_blocked = 0;

} /* rr_gprs_sys_info_allow_all_updates() */

/*!
 * \brief Blocks updates from sys info changes to all layers during a sys info refresh.
 *
 * \param gas_id (in)
 */
void rr_gprs_sys_info_block_all_updates(const gas_id_t gas_id)
{
  rr_gprs_sys_info_block_updates(
     SYS_INFO_UPDATE_IDENTITY_L1 | SYS_INFO_UPDATE_IDENTITY_MM | SYS_INFO_UPDATE_IDENTITY_MAC,
     gas_id
   );

} /* rr_gprs_sys_info_block_all_updates() */

/*!
 * \brief Indicates if updates to L1 are allowed during a sys info refresh.
 *
 * \param gas_id (in)
 *
 * \return boolean - TRUE if updates to L1 are allowed, FALSE otherwise
 */
boolean rr_gprs_l1_sys_info_update_allowed(const gas_id_t gas_id)
{
  return
    rr_gprs_sys_info_update_allowed(
      SYS_INFO_UPDATE_IDENTITY_L1,
      gas_id
    );

} /* end rr_gprs_l1_sys_info_update_allowed() */

/*!
 * \brief Indicates if updates to MAC are allowed during a sys info refresh.
 *
 * \param gas_id (in)
 *
 * \return boolean - TRUE if updates to MAC are allowed during a sys info refresh, FALSE otherwise
 */
boolean rr_gprs_mac_sys_info_update_allowed(const gas_id_t gas_id)
{
  return
    rr_gprs_sys_info_update_allowed(
      SYS_INFO_UPDATE_IDENTITY_MAC,
      gas_id
    );

} /* end rr_gprs_mac_sys_info_update_allowed() */

/*!
 * \brief Indicates if updates to MM are allowed during a sys info refresh.
 *
 * \param gas_id
 *
 * \return boolean - TRUE if updates to MM are allowed during a sys info refresh; FALSE otherwise
 */
boolean rr_gprs_mm_sys_info_update_allowed(const gas_id_t gas_id)
{
  return
    rr_gprs_sys_info_update_allowed(
      SYS_INFO_UPDATE_IDENTITY_MM,
      gas_id
    );

} /* end rr_gprs_mm_sys_info_update_allowed() */

/*!
 * \brief Checks if the status of SI13 has changed (i.e. added or removed).
 *
 * \param gas_id
 *
 * \return grr_siev_t - {SIEV_NO_EVENT, SI13_ADDED_OR_REMOVED}
 */
grr_siev_t grr_align_sys_info_acquisition_with_cell_capabilities(const gas_id_t gas_id)
{
  grr_siev_t siev = SIEV_NO_EVENT;
  gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

  if (cell_info->gsm.gprs_indicator &&
       (cell_info->bcch_sys_info.si13_data_valid_flag == FALSE))
  {
    siev = SIEV_SI13_ADDED;
  }
  else
  if ((cell_info->gsm.gprs_indicator == FALSE) &&
       cell_info->bcch_sys_info.si13_data_valid_flag)
  {
    cell_info->bcch_sys_info.si13_data_valid_flag = FALSE;
    siev = SIEV_SI13_REMOVED;
  }

  rr_eng_mode_update_scell_config(gas_id);

  return siev;

} /* end grr_align_sys_info_acquisition_with_cell_capabilities() */

/*!
 * \brief Sends a RR_SERVICE_IND to MM using the serving cell information.
 *
 * \param gas_id
 */
void grr_update_mm_with_rr_service_ind(const gas_id_t gas_id)
{
  if (rr_gprs_mm_sys_info_update_allowed(gas_id))
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

#ifdef FEATURE_GSM_DTM
    if (rr_in_dedicated(gas_id))
    {
      rr_send_service_ind_from_ota_msg(
        MM_AS_SERVICE_AVAILABLE,
        &(scell_info_ptr->gsm.location_area_identification),
        &scell_info_ptr->gsm,
        gas_id
      );
    }
    else
#endif /* FEATURE_GSM_DTM */
    rr_send_service_ind(
      MM_AS_SERVICE_AVAILABLE,
      &(scell_info_ptr->gsm.location_area_identification),
      &scell_info_ptr->gsm,
      gas_id
    );
  }
  else
  {
    MSG_GERAN_MED_0_G("Updates to MM blocked");
  }

} /* grr_update_mm_with_rr_service_ind() */

/*!
 * \brief This function maps the internal RR band type 'rr_internal_band_T' to the type used by MM for the
 *        'MS Radio Access Capability IE', 24.008 table 10.5.146
 *
 * \param band (in)
 *
 * \return uint8 - band in Radio Access Capability IE format
 */
static uint8 map_rr_band_to_mm_rac_band(rr_internal_band_T band)
{
  uint8 rac_band = 0;

  switch(band)
  {
    case BAND_EGSM:
      rac_band = ACCESS_TECH_GSM_E;
      break;

    case BAND_PGSM:
      rac_band = ACCESS_TECH_GSM_P;
      break;

    case BAND_PCS:
      rac_band = ACCESS_TECH_GSM_1900;
      break;

    case BAND_DCS:
      rac_band = ACCESS_TECH_GSM_1800;
      break;

    case BAND_GSM_850:
      rac_band = ACCESS_TECH_GSM_850;
      break;

    default:
      MSG_GERAN_ERROR_0("RR Invalid rr band, cannot map to MM Rac type");
      break;
  }

  MSG_GERAN_HIGH_2("GRR DEBUG, rr map current band:%d, to rac_band:%d",band,rac_band);

  return rac_band;
}

/*!
 * \brief Updates the public store with the sys info data required by MAC/RLC
 *
 * \param gas_id
 */
void rr_si_update_public_store(
  const gas_id_t gas_id
)
{
  rrps_store_update_bitmap_t rrps_bitmap = 0;
  gprs_scell_info_t *scell_ptr = rr_gprs_get_scell_info(gas_id);
  rrps_store_update_data_t *rrps_store_update_ptr = rr_get_rrps_store_update(gas_id);
  if (rrps_store_update_ptr != NULL)
  {
    if (scell_ptr->bcch_sys_info.si13_data_valid_flag)
    {
      rrps_bitmap = RRPS_UPD_VALID_MA_BIT_MAP |
                    RRPS_UPD_CHANGE_MARK_SI13 |
                    RRPS_UPD_CELL_OPTIONS     |
                    RRPS_UPD_POWER_CTRL       |
                    RRPS_UPD_MS_NON_DRX_TIMER |
                    RRPS_UPD_PRIORITY_ACCESS_THR;

      rrps_store_update_ptr->ma_valid_bitmap  = scell_ptr->mobile_allocations.ma_valid_bitmap;
      rrps_store_update_ptr->change_mark_si13 = scell_ptr->mobile_allocations.si13_change_mark;

      {
        gprs_bcch_scell_info_t *gprs_ptr = &(scell_ptr->bcch_sys_info);

        rrps_store_update_ptr->gprs_cell_options    = gprs_ptr->gprs_cell_options;
        rrps_store_update_ptr->power_control_params = gprs_ptr->power_control_params;
        rrps_store_update_ptr->non_drx_timer        = gprs_ptr->non_drx_timer;
        rrps_store_update_ptr->priority_access_thr  = gprs_ptr->priority_access_thr;
      }
    }

    rrps_bitmap |= RRPS_UPD_MS_BCCH_BAND;
    rrps_store_update_ptr->ms_bcch_band = map_rr_band_to_mm_rac_band(rr_gapi_get_current_band(gas_id));


    /* load the public store with the current band indicator */
    rrps_bitmap |= RRPS_UPD_NW_BAND_IND;

    if (rr_get_current_band_indicator(TRUE, gas_id) == SI_PSI_BAND_IND_PCS1900)
    {
      rrps_store_update_ptr->nw_band_ind = ACCESS_TECH_GSM_1900;
    }
    else
    {
      rrps_store_update_ptr->nw_band_ind = ACCESS_TECH_GSM_1800;
    }

    if (rrps_bitmap != 0)
    {
      rr_write_public_store(rrps_bitmap, rrps_store_update_ptr, gas_id);
    }
    rr_free_rrps_store_update(rrps_store_update_ptr, gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rrps_store_update_ptr = NULL");
  }

} /* end rr_si_update_public_store() */

/*!
 * \brief Checks the validity of the sys info database.
 *
 * \param gas_id (in)
 *
 * \return TRUE - if valid, FALSE - otherwise
 */
boolean rr_get_gprs_system_info_database_valid(const gas_id_t gas_id)
{
  int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id); 

  return (gprs_system_info_database_ptrs[as_index] != NULL);
}

/*!
 * \brief Returns a pointer to the sys info database (and allocates memory if not allocated).
 *
 * \param gas_id (in)
 *
 * \return gprs_system_info_database_t* - pointer to the sys info database
 */
gprs_system_info_database_t *rr_get_gprs_system_info_database_ptr(const gas_id_t gas_id)
{
  int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id); 
  gprs_system_info_database_t *sidb_ptr = NULL;

  if (gprs_system_info_database_ptrs[as_index] == NULL)
  {
    // Allocate heap for an instance of gprs_system_info_database_t
    sidb_ptr = (gprs_system_info_database_t *)GPRS_MEM_CALLOC(sizeof(gprs_system_info_database_t));
    MSG_GERAN_HIGH_3_G("Allocated %d bytes for sys info database, gprs_system_info_database_ptrs[%d]:0x%x",
      sizeof(gprs_system_info_database_t),
      (int)as_index,
      sidb_ptr
    );
    if (sidb_ptr != NULL)
    {
      // Store the pointer to the heap storage for later access
      gprs_system_info_database_ptrs[as_index] = sidb_ptr;
    }
    else
    {
      ERR_GERAN_FATAL_0_G("gprs_system_info_database_ptr == NULL");
    }
  }
  else
  {
    // Storage already allocated - return a pointer to the correct instance
    sidb_ptr = gprs_system_info_database_ptrs[as_index];
  }

  return (sidb_ptr);
}

/*!
 * \brief Clears the 'GPRS' elements of the sys info database structure.
 *
 * \param cell_info (in)
 */
void rr_clear_gprs_cell_info(gprs_scell_info_t *cell_info)
{
  memset( &cell_info->l1_idle_mode, 0, sizeof( cell_info->l1_idle_mode ) );
  memset( &cell_info->bcch_sys_info, 0, sizeof( cell_info->bcch_sys_info ) );
  memset( &cell_info->dtm_sys_info_valid, 0, sizeof( cell_info->dtm_sys_info_valid ) );
  memset( &cell_info->dtm_sys_info, 0, sizeof( cell_info->dtm_sys_info ) );
  memset( &cell_info->mobile_allocations, 0, sizeof( cell_info->mobile_allocations ) );
  memset( &cell_info->nc_meas_params, 0, sizeof( cell_info->nc_meas_params ) );
  memset( &cell_info->gprs_meas_params, 0, sizeof( cell_info->gprs_meas_params ) );
  memset( &cell_info->gprs_3g_meas_params, 0, sizeof( cell_info->gprs_3g_meas_params ) );
  memset( &cell_info->gprs_bsic_description, 0, sizeof( cell_info->gprs_bsic_description ) );
}


/****************************************************************************
 *   Function name:  rr_data_free_gprs_system_info_database_data
 *   --------------------------
 *
 *   Description: De allocate the heap assigned for GPRS scell info.
 *
 *   Parameters: gas_id_t
 *   -----------
 *
 *   Returns:
 *   --------
 *
 ****************************************************************************/
void rr_data_free_gprs_system_info_database_data(const gas_id_t gas_id)
{
  int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  if (gprs_system_info_database_ptrs[as_index] != NULL)
  {
    gprs_scell_info_t    *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

    /* This database can be accessed by other tasks. So, acquire the lock before freeing the memory. */
    GRR_SYSINFO_LOCK();

    rr_data_free_scell_dedicated_mode_params(scell_info_ptr,gas_id);

    GPRS_MEM_FREE(gprs_system_info_database_ptrs[as_index]);
    gprs_system_info_database_ptrs[as_index] = NULL;

    GRR_SYSINFO_UNLOCK();
    
  }
}

/* EOF */

