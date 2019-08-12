/*! \file rr_sys_info_si.c

  Contains System Information acquisition, decoding & processing functionality.

                Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info_si.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"
#include "rr_seg_load.h"
#include "rr_sys_info_si.h"
#include "rr_sys_info_ncell.h"
#include "rr_sys_info_pcell.h"
#include "rr_general.h"
#include "rr_gprs_defs.h"
#include "rr_decode_ie.h"
#include "rr_init.h"
#include "rr_sys_info_i.h"
#include "rr_sys_info.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "rr_control.h"
#include "rr_l1_send.h"
#include "rr_mm_send.h"
#include "ms.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_resel_utils.h"
#include "rr_sys_info_pacch.h"
#include "rr_process_pmo_pcco.h"
#include "rr_log.h"
#ifdef FEATURE_FAKE_BTS_CELL_INFO
#include "rr_mm_send.h"
#endif /* FEATURE_FAKE_BTS_CELL_INFO*/

#include "rr_nv.h"
#ifdef FEATURE_GSM_EDTM
#include "rr_gprs_edtm.h"
#endif /* FEATURE_GSM_EDTM */

#include "rr_resel_nacc.h"

#include "geran_multi_sim.h"

#ifdef FEATURE_DUAL_SIM
#include "rr_multi_sim.h"
#include "rr_mode.h"
#endif /* FEATURE_DUAL_SIM */

#include "rr_if.h"
#include "rr_ps_domain.h"
#include "rr_l1_idle_mode.h"
#include "rr_sys_info_cache.h"

#include "rr_gprs_meas_rpt_control.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* CSN Decoder defines */
#define CSN_BCCH_NORMAL    0
#define CSN_BCCH_EXTENDED  1

#define GRR_TIMEOUT_BCCH_SI_ACQUISITION  ( GRR_TIMEOUT_1_SECOND * 55 )

/* define the amount of time it takes to receive 1 full sys info TC cycle:
 * this equates to (8 * 51) frames, which is (408 * 4.615) milliseconds
 */
#define ONE_FULL_TC_CYCLE 1883UL

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*
BCCH sys info acquisition bitmasks:

si_messages_required
This bitmask represents which SIs are required during an acquisition.
1=expected, 0=not required
As SIs are received, the appropriate bit is cleared - so when all SIs have been
received, this bitmask will be zero.

si_messages_low_priority

This bitmask represents SIs which are required, but need not be acquired in order to
declare an acquisition "complete". For example, if SI2ter was being broadcast, then
si_messages_required would contain MASK_SI_2_TER, and si_messages_low_priority would
contain MASK_SI_2_TER_REST. That means that say if GPRS was suspended in order to refresh
SI2ter, then once one instance of SI2ter was acquired, GPRS would be resumed - the sys
info handler would then continue and request SI2ter again for the rest octets.
Note: si_messages_low_priority is a subset of si_messages_required

si_messages_on_bcch_ext

This bitmask indicates which (if any) SIs are being broadcast on BCCH-Ext
1=BCCH Ext, 0=BCCH Norm

si_messages_not_broadcast

This bitmask tracks SIs which are not being broadcast by the network
1=not broadcast, 0=unknown / is broadcast

e.g. If SI3 is received and SIquater is not indicated, then the MASK_SI_2_QUATER bit
will be set in si_messages_not_broadcast.

The bitmask is used to mask off those SIs not required if the BCCH_CHANGE_MARK changes and
indicates e.g. all SI2 variants have been updated
(If a SI2 variant was added, it would be expected that a full SI refresh would be ordered
by advancing the BCCH_CHANGE_MARK by more than one)

single_l1_si_request_only

When L1 is in GPRS transfer, it can only process a request for a single SI to be read in
a MPH_READ_SYS_INFO_REQ message.
The flag is TRUE when bsi_acquisition_control is acting in this mode, and will prioritise
the requested SIs, and only set one bit in the bitmask sent to L1
*/

typedef enum
{
  BSIST_ACQ_NOT_REQUESTED,
  BSIST_ACQ_PAGING_REORG,
  BSIST_ACQ_REQUESTED,
  BSIST_ACQ_REQUESTED_SI13_ADDED,
  BSIST_ACQ_REQUESTED_SI13_REMOVED,
  BSIST_ACQ_SUSPENDED

} bsi_acquisition_state_t;

typedef struct
{
  bsi_acquisition_state_t state;
  bsi_acquisition_state_t old_state;
  si13_data_t             decoded_si13_data;
  boolean                 rr_sys_info_notified_of_ia_reject;
} rr_sys_info_si_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

#define _RR_SYS_INFO_DATA_INIT { \
                                BSIST_ACQ_NOT_REQUESTED, /*state*/ \
                                BSIST_ACQ_NOT_REQUESTED  /*old_state*/ \
                              }

static rr_sys_info_si_data_t rr_sys_info_si_data[NUM_GERAN_DATA_SPACES] =
                               { INITIAL_VALUE(_RR_SYS_INFO_DATA_INIT) };
#undef _RR_SYS_INFO_DATA_INIT

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_sys_info_si_data_t*
 */
static rr_sys_info_si_data_t *rr_sys_info_si_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_sys_info_si_data[as_index];
}

/*!
 * \brief Returns a text string representing the indicated state.
 *
 * \param state
 *
 * \return char*
 */
static char *get_bsi_acquisition_control_state_name(
  bsi_acquisition_state_t state
)
{
  char * s;

  switch ( state )
  {
    case BSIST_ACQ_NOT_REQUESTED:
      s = "NOT_REQUESTED";
      break;

    case BSIST_ACQ_PAGING_REORG:
      s = "PAGING_REORG";
      break;

    case BSIST_ACQ_REQUESTED:
      s = "REQUESTED";
      break;

    case BSIST_ACQ_REQUESTED_SI13_ADDED:
      s = "REQUESTED_SI13_ADDED";
      break;

    case BSIST_ACQ_REQUESTED_SI13_REMOVED:
      s = "REQUESTED_SI13_REMOVED";
      break;

    case BSIST_ACQ_SUSPENDED:
      s = "SUSPENDED";
      break;

    default:
      s = "?";
  }

  return s;

} /* end get_bsi_acquisition_control_state_name() */

/*!
 * \brief Determines what kind of SI acquisition, if any, should be performed.
 *
 * \param gas_id (in)
 *
 * \return grr_siev_t
 */
static grr_siev_t bsi_determine_action_after_activation(
  const gas_id_t gas_id
)
{
  grr_siev_t  siev = SIEV_NO_EVENT;
  rr_sys_info_si_data_t *rr_sys_info_si_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_si_data_ptr = rr_sys_info_si_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_si_data_ptr);

  if ( rr_ce_get_ota_page_mode(gas_id) == PAGING_REORGANIZATION )
  {
    siev = SIEV_ENTER_PAGING_REORG;
  }
  else
  {
    gprs_scell_info_t * scell_info = rr_gprs_get_scell_info(gas_id);

    if ( ! scell_info->gsm.valid_data_flag )
    {
      MSG_GERAN_HIGH_0_G("Acquiring SI (outdated)");

      scell_info->gsm.valid_data_flag = TRUE;

      siev = SIEV_SYS_INFO_FULL_REFRESH;
    }
    else
    if ( rr_sys_info_si_data_ptr->rr_sys_info_notified_of_ia_reject )
    {
      MSG_GERAN_HIGH_0_G("Acquiring SI (IA Reject)");

      rr_sys_info_si_data_ptr->rr_sys_info_notified_of_ia_reject = FALSE;

      siev = SIEV_SYS_INFO_FULL_REFRESH;
    }
    else
    if ( scell_info->bcch_sys_info.si_messages_required )
    {
      MSG_GERAN_HIGH_0_G("Acquiring SI (incomplete set)");
      siev = SIEV_CONTINUE_SYS_INFO_ACQUISITION;
    }

    /*
    If about to start a full sys info refresh, then start the 30s refresh timer from now
    (which will cancel-and-restart if currently running).
    If continuing with a previously incomplete refresh, or no refresh is required at all,
    then only start the timer if it's not already running
    */
    if ( siev == SIEV_SYS_INFO_FULL_REFRESH )
    {
      rr_gprs_start_sys_info_refresh_timer(gas_id);
    }
    else
    if ( ! rr_timer_is_running( GRR_SYS_INFO_REFRESH_TIMER, gas_id ) )
    {
      rr_gprs_start_sys_info_refresh_timer(gas_id);
    }
  }

  return siev;

} /* end bsi_determine_action_after_activation() */

/*!
 * \brief Returns a bitmask indicating which SI messages still need to be acquired.
 *
 * \param cell_info (in)
 *
 * \return uint16
 */
static uint16 bsi_determine_messages_missing_after_camping(
  gprs_scell_info_t *cell_info,
  const boolean include_3G,
  const gas_id_t gas_id
)
{
  uint16 messages_missing = MASK_SI_NONE;

  /*
  If there are any System Information messages that remain to be collected,
  make sure that L1 is told to continue to read the BCCH, and ensure that
  si_messages_expected is updated properly so that when the SIs have been
  collected, GRR will tell L1 to stop reading the BCCH.
  */

  messages_missing |= rr_gprs_nacc_additional_si_required(&cell_info->gsm, include_3G);

  if (include_3G)
  {
    if ( (cell_info->gsm.two_quater_position != SI2Q_POSITION_NONE) &&
          (! cell_info->gsm.complete_idle_3G_neighbor_list_received) )
    {
      messages_missing |= MASK_SI_2_QUATER;
    }

    if ( (cell_info->gsm.two_ti == RR_SI2TER_EXPECTED) &&
         (! cell_info->gsm.complete_idle_3G_neighbor_list_received_si2ter) )
    {
      messages_missing |= MASK_SI_2_TER_REST;
    }
  }

  /* If the cell supports GPRS but SI13 has not been acquired yet (e.g. the MS is */
  /* GSM-only so was not required to wait for SI13), acquire it now */
  if ( cell_info->gsm.gprs_indicator &&
       ( ! cell_info->bcch_sys_info.si13_data_valid_flag ) )
  {
    messages_missing |= MASK_SI_13;
  }

  /* Now mask off those SIs which we know aren't being broadcast */
  /* si_messages_not_broadcast: 1=not present, 0=present/unknown */
  messages_missing &= ( ~ cell_info->bcch_sys_info.si_messages_not_broadcast );

  return messages_missing;

} /* end bsi_determine_messages_missing_after_camping() */

/*!
 * \brief SI acqusition state machine common signal handler.
 *
 * \param message (in)
 * \param gas_id (in)
 *
 * \return grr_siev_t
 */
static grr_siev_t bsi_common_signal_handler(
  const rr_cmd_bdy_type *message,
  const gas_id_t gas_id
)
{
  grr_siev_t  event_in = SIEV_NO_EVENT;

  if ( message )
  {
    IMH_T     * message_header = (IMH_T *) message;
    byte        message_set = message_header->message_set;
    byte        message_id  = message_header->message_id;

    event_in = SIEV_INPUT_MESSAGE;

    if ( message_set == MS_RR_RR )
    {
      switch (message->rr.header.rr_message_set)
      {
        case RR_L1_IDLE_IMSG:
        {
          rr_l1_idle_control(message, gas_id);
          break;
        }

        case RR_L1_IDLE_OMSG:
        {
          switch (message->message_header.message_id)
          {
            case RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF:
            {
              event_in = SIEV_MPH_START_IDLE_MODE_CNF;
              break;
            }

            default: ;
          }
          break;
        }

        default: ; // ignore other RR-RR messages
      }
    }

    else
    if ( ( message_set == MS_RR_L1 ) &&
         ( message_id  == (byte)MPH_SELECT_SPECIFIC_BCCH_CNF ) )
    {
      mph_select_specific_bcch_cnf_T *msg_ptr;

      msg_ptr = (mph_select_specific_bcch_cnf_T *)message;
      if (msg_ptr->BCCH_found == TRUE)
      {
        event_in = SIEV_MPH_SELECT_SPECIFIC_BCCH_CNF;
      }
      else
      {
        event_in = SIEV_MPH_SELECT_SPECIFIC_BCCH_FAIL;
      }
    }

    else
    if ( ( message_set == MS_RR_L1 ) &&
         ( message_id  == (byte)MPH_START_IDLE_MODE_CNF ) )
    {
      rr_l1_idle_control(message, gas_id);
    }

    else
    if ( ( message_set == MS_RR_L1 ) &&
         ( message_id  == (byte)MPH_READ_SYS_INFO_FAILURE_IND ) )
    {
      mph_read_sys_info_failure_ind_T  * read_failure_ind = (mph_read_sys_info_failure_ind_T *) message;

      if ( read_failure_ind->in_transfer )
      {
        event_in = SIEV_READ_SYS_INFO_FAILURE_L1_IN_TRANSFER;
      }
      else
      {
        event_in = SIEV_READ_SYS_INFO_FAILURE_UNSPECIFIED;
      }
    }

    else
    /*****************************************************************/
    /* Timer Expiry                                                  */
    /* A timer expiry is translated into a SIEV event given by the   */
    /* timer ID                                                      */
    /*****************************************************************/
    if ( ( message_set == MS_TIMER ) &&
         ( message_id == MID_TIMER_EXPIRY ) )
    {
      event_in = grr_timerid_to_siev((rr_timer_id_T) ((timer_expiry_T *)message)->timer_id );
    }
  }

  return event_in;

} /* end bsi_common_signal_handler() */

/*!
 * \brief Returns a bitmask of the remaining SIs.
 *
 * \param cell_info (in)
 *
 * \return uint16
 */
static uint16 get_remaining_si_bitmask(
  gprs_scell_info_t *cell_info
)
{
  uint16 remaining_si_bitmask;

  /* This returns SIs requested but not received + SIs received but not requested */
  remaining_si_bitmask = ( cell_info->gsm.update_message_received ^
                           cell_info->bcch_sys_info.si_messages_required );

  /* This filters off the SIs received but not requested */
  remaining_si_bitmask &= cell_info->bcch_sys_info.si_messages_required;

  /* Which leaves only the SIs requested but not received */
  return remaining_si_bitmask;

} /* get_remaining_si_bitmask() */

/*!
 * \brief Maps the internal SI bitmap representation to the RR-L1 interface format.
 *
 * \param rr_si_bitmask
 * \param si_bcch_ext_bitmask
 *
 * \return rr_l1_si_type_T - returned RR-L1 format SI bitmask
 */
static rr_l1_si_type_T  map_rr_si_bitmask_to_l1_si_bitmask(
  uint16 rr_si_bitmask,
  uint16 si_bcch_ext_bitmask
)
{
  uint32 l1_si_bitmask = (uint32)RR_L1_SI_TYPE_NONE;

  if ( rr_si_bitmask & MASK_SI_1 )
  {
    l1_si_bitmask |= (uint32)RR_L1_SI_TYPE_1;
  }

  if ( rr_si_bitmask & MASK_SI_2 )
  {
    l1_si_bitmask |= (uint32)RR_L1_SI_TYPE_2;
  }

  if ( rr_si_bitmask & MASK_SI_2_BIS )
  {
    l1_si_bitmask |= (uint32)RR_L1_SI_TYPE_2_bis;
  }

  if ( ( rr_si_bitmask & MASK_SI_2_TER ) || ( rr_si_bitmask & MASK_SI_2_TER_REST ) )
  {
    l1_si_bitmask |= (uint32)RR_L1_SI_TYPE_2_ter;
  }

  if ( rr_si_bitmask & MASK_SI_2_QUATER )
  {
    if ( si_bcch_ext_bitmask & MASK_SI_2_QUATER )
    {
      l1_si_bitmask |= (uint32)RR_L1_SI_TYPE_2_quater_EXT;
    }
    else
    {
      l1_si_bitmask |= (uint32)RR_L1_SI_TYPE_2_quater_NORM;
    }
  }

  if ( rr_si_bitmask & MASK_SI_3 )
  {
    l1_si_bitmask |= (uint32)RR_L1_SI_TYPE_3;
  }

  if ( rr_si_bitmask & MASK_SI_4 )
  {
    l1_si_bitmask |= (uint32)RR_L1_SI_TYPE_4;
  }

  if ( rr_si_bitmask & MASK_SI_7 )
  {
    l1_si_bitmask |= (uint32)RR_L1_SI_TYPE_7;
  }

  if ( rr_si_bitmask & MASK_SI_8 )
  {
    l1_si_bitmask |= (uint32)RR_L1_SI_TYPE_8;
  }

  if ( rr_si_bitmask & MASK_SI_13 )
  {
    if ( si_bcch_ext_bitmask & MASK_SI_13 )
    {
      l1_si_bitmask |= (uint32)RR_L1_SI_TYPE_13_EXT;
    }
    else
    {
      l1_si_bitmask |= (uint32)RR_L1_SI_TYPE_13_NORM;
    }
  }

  return((rr_l1_si_type_T)l1_si_bitmask);

} /* end map_rr_si_bitmask_to_l1_si_bitmask() */

/*!
 * \brief Requests L1 to read the BCCH sys info indicated by the bitmask.
 *
 *  Requests L1 to read the BCCH sys info indicated by the bitmask. If the bitmask is empty (all-zero),
 *  then L1 will stop reading BCCH sys info.
 *
 * \param si_required (in) - bitmask indicating SI required (1=yes)
 * \param si_messages_on_bcch_ext (in) - bitmask indicating SIs on BCCH-Ext (1=yes)
 * \param single_l1_si_request_only (in) - TRUE if restricted to a single request, FALSE if not
 * \param gas_id (in)
 */
static void send_mph_read_sys_info_req(
  uint16 si_required,
  uint16 si_messages_on_bcch_ext,
  boolean single_l1_si_request_only,
  const gas_id_t gas_id
)
{
  rr_l1_si_type_T  l1_si_required = RR_L1_SI_TYPE_NONE;

  if (si_required != MASK_SI_NONE)
  {
    const uint16 bitmask_priority[] =
      { MASK_SI_13,
        MASK_SI_1,
        MASK_SI_3,
        MASK_SI_4,
        MASK_SI_2,
        MASK_SI_2_BIS,
        MASK_SI_2_TER | MASK_SI_2_TER_REST,
        MASK_SI_2_QUATER,
        MASK_SI_7,
        MASK_SI_8 };

    if (single_l1_si_request_only)
    {
      uint16 new_si_bitmask = MASK_SI_NONE;
      uint8 i = 0;

      while (i < ARR_SIZE(bitmask_priority))
      {
        if ((si_required & bitmask_priority[i]) != 0)
        {
          new_si_bitmask = bitmask_priority[i];
          break;
        }

        i++;
      }

      si_required = new_si_bitmask;
    }

    l1_si_required = map_rr_si_bitmask_to_l1_si_bitmask(si_required, si_messages_on_bcch_ext);
  }

  MSG_GERAN_MED_2_G("Asking for BCCH SI 0x%x (L1 mask 0x%x)",(int)si_required,(int)l1_si_required);

  rr_send_mph_read_sys_info_req(l1_si_required, gas_id);

} /* end send_mph_read_sys_info_req() */

/*!
 * \brief Requests L1 to read BCCH sys info.
 *
 * \param gas_id (in)
 */
static void continue_l1_bsi_acquisition(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t * scell_info = rr_gprs_get_scell_info(gas_id);

  /* Sent a new SI bitmask to L1, after determining which SIs still need to be acquired */
  send_mph_read_sys_info_req(
    get_remaining_si_bitmask(scell_info),
    scell_info->bcch_sys_info.si_messages_on_bcch_ext,
    scell_info->bcch_sys_info.single_l1_si_request_only,
    gas_id
  );

  /* Start the acquisition timer - 60 secs */
  /* Note: If the timer is already running, it is NOT restarted from zero */
  rr_gprs_start_sys_info_acquisition_timer(GRR_TIMEOUT_BCCH_SI_ACQUISITION, gas_id);

} /* end continue_l1_bsi_acquisition() */

/*!
 * \brief Requests L1 to stop reading BCCH sys info
 *
 * \param gas_id
 */
static void stop_l1_bsi_acquisition(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t * scell_info = rr_gprs_get_scell_info(gas_id);

  send_mph_read_sys_info_req(
    MASK_SI_NONE,   // si_required
    MASK_SI_NONE,   // si_messages_on_bcch_ext
    FALSE,          // single_l1_si_request_only
    gas_id          // gas_id
  );

  scell_info->bcch_sys_info.single_l1_si_request_only = FALSE;

} /* end stop_l1_bsi_acquisition() */

/*!
 * \brief Requests L1 to stop reading BCCH sys info.
 *
 * \param gas_id
 */
static void abort_l1_bsi_acquisition(
  const gas_id_t gas_id
)
{
  bsi_set_messages_required_for_acquisition(
    MASK_SI_NONE,   // required
    MASK_SI_NONE,   // low_priority
    TRUE,           // reset_history
    gas_id          // gas_id
  );

  stop_l1_bsi_acquisition(gas_id);

} /* end abort_l1_bsi_acquisition() */

/*!
 * \brief Called after SI acquisition has been successfully completed.
 *
 * \param gas_id
 *
 * \return grr_siev_t
 */
static grr_siev_t bsi_acquisition_complete(
  const gas_id_t gas_id
)
{
  grr_siev_t  siev;

  gprs_scell_info_t  * scell_info = rr_gprs_get_scell_info(gas_id);

  if ( scell_info->bcch_sys_info.si_messages_required == MASK_SI_NONE )
  {
    MSG_GERAN_MED_0_G("Acquisition complete");
  }
  else
  {
    MSG_GERAN_MED_2_G("Acquisition partially complete (0x%x still required)",
            scell_info->bcch_sys_info.si_messages_required, 0 );
  }

  /*
  Clean-up: Stop acquisition timer and check L1 idle mode
  This returns:
    SIEV_NO_EVENT - no change in L1 idle mode required
    SIEV_CHANGE_L1_IDLE_MODE - a new start idle should be sent to L1
  */
  siev = rr_gprs_camped_sys_info_acquisition_complete(gas_id);

  /* Remove messages from si_messages_low_priority which are present in */
  /* si_messages_required (i.e. have been received already) */
  scell_info->bcch_sys_info.si_messages_low_priority &=
    scell_info->bcch_sys_info.si_messages_required;

  if ( siev == SIEV_NO_EVENT )
  {
    /* There may still be low priority SIs left to acquire - if so, re-activate */
    if ( scell_info->bcch_sys_info.si_messages_required )
    {
      siev = SIEV_ACTIVATE_SYS_INFO_ACQUISITION;
    }
  }

  /* If not re-activating in this thread, then instruct L1 to stop reading SIs */
  if ( siev != SIEV_ACTIVATE_SYS_INFO_ACQUISITION )
  {
    /* Tell L1 to stop reading SIs */
    stop_l1_bsi_acquisition(gas_id);
  }

  return siev;

} /* bsi_acquisition_complete() */

/*!
 * \brief Called upon BCCH SI acquisition failure (i.e. acquisition timer expiry).
 *
 *  Called upon BCCH SI acquisition failure (i.e. acquisition timer expiry). L1 is requested to stop
 *  SI acquisition.
 *
 * \param gas_id
 */
static void bsi_acquisition_failed(
  const gas_id_t gas_id
)
{
  MSG_GERAN_HIGH_0_G("Acquisition failed");

  /* Tell L1 to stop reading SIs */
  abort_l1_bsi_acquisition(gas_id);

  /* Stop the refresh cycle, as the MS will now reselect */
  rr_gprs_stop_sys_info_refresh_timer(gas_id);

} /* end bsi_acquisition_failed() */

/*!
 * \brief Setup the SI bitmask for a full SI acquisition. Note: This does not send any request to L1.
 *
 * \param gas_id
 */
static void setup_full_refresh_acquisition(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *cell_info     = rr_gprs_get_scell_info(gas_id);
  rr_si_type_t       last_si       = cell_info->bcch_sys_info.last_si_message_received;
  uint16             required_mask = MASK_FULL_BSI_ACQUISITION;
  uint16             low_pri;

  /*
  If this refresh was triggered by the reception of a SI (probably SI3 or SI13) as
  opposed to timer-triggered, don't include this SI in the mask of SIs required, as
  there is no point in acquiring it again
  */
  if (last_si != SI_TYPE_NONE)
  {
    uint16 not_required_mask = SI_TYPE_TO_MASK(last_si);

    required_mask &= (~not_required_mask);
  }

  if (rr_ms_3g_sys_info_mandatory(gas_id))
  {
    /* SI2ter and SI2quater are always low priority */
    low_pri = MASK_SI_2_TER | MASK_SI_2_QUATER;
  }
  else
  {
    /* this access stratum is in Dual SIM mode, but is not paired with */
    /* any WCDMA subscription, so GRR may avoid requesting 3G sys info */
    low_pri = MASK_SI_2_TER;
    required_mask &= ~(MASK_SI_2_QUATER | MASK_SI_2_TER_REST);
    MSG_GERAN_HIGH_0_G("3G system info not mandatory for refresh");
  }

  bsi_set_messages_required_for_acquisition(
    required_mask,    // required
    low_pri,          // low_priority
    TRUE,             // reset_history
    gas_id            // gas_id
  );

  return;
} /* end setup_full_refresh_acquisition() */

/*!
 * \brief Instructs L1 to read BCCH sys info.
 *
 * \param gas_id
 */
static void start_l1_full_refresh_acquisition(
  const gas_id_t gas_id
)
{
  setup_full_refresh_acquisition(gas_id);
  continue_l1_bsi_acquisition(gas_id);

} /* end start_l1_full_refresh_acquisition() */

/*!
 * \brief Setup the correct SI bitmask for a SI13 refresh. Note: This does not send any request to L1.
 *
 * \param gas_id (in)
 */
static void setup_si13_refresh_acquisition(
  const gas_id_t gas_id
)
{
  bsi_set_messages_required_for_acquisition(
    MASK_SI_13,     // required
    MASK_SI_NONE,   // low_priority
    FALSE,          // reset_history
    gas_id          // gas_id
  );

} /* end setup_si13_refresh_acquisition() */

/*!
 * \brief Setup the SI bitmasks in preparation for a SI refresh.
 *
 * \param gas_id (in)
 */
static void setup_refresh_acquisition(
  const gas_id_t gas_id
)
{
  if ( rr_cell_supports_gprs(gas_id) )
  {
    setup_si13_refresh_acquisition(gas_id);
  }
  else
  {
    setup_full_refresh_acquisition(gas_id);
  }

} /* end setup_refresh_acquisition() */

/*!
 * \brief Instructs L1 to read SI13 if the cell supports GPRS, or all the GSM SI if not.
 *
 * \param gas_id
 */
static void start_l1_refresh_acquisition(
  const gas_id_t gas_id
)
{
  setup_refresh_acquisition(gas_id);
  continue_l1_bsi_acquisition(gas_id);

} /* end start_l1_refresh_acquisition() */

/*!
 * \brief Checks the SI messages received against those expected.
 *
 * \param gas_id (in)
 *
 * \return boolean - TRUE is all required SIs have been received; FALSE otherwise
 */
static boolean check_if_bsi_acquisition_complete(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t * scell_info = rr_gprs_get_scell_info(gas_id);
  boolean  acquisition_complete = FALSE;

  /*
  Check that at all SIs required have been received.
  As SIs are received, a bit is cleared in si_messages_required. When this is
  empty, or only contains bits which represent low priority SIs, declare
  the acquisition as complete
  */
  if ( ( scell_info->bcch_sys_info.si_messages_required &
         (~ scell_info->bcch_sys_info.si_messages_low_priority) ) ==
       MASK_SI_NONE )
  {
    acquisition_complete = TRUE;

    /* If the sys info had been invalidated (e.g. after call release when a handover */
    /* in-call occurred) */
    if ( ! scell_info->gsm.valid_data_flag )
    {
      scell_info->gsm.valid_data_flag = TRUE;

      /* notify MM and L1 about any changes */
      rr_perform_MM_or_L1_notification_on_serving_SI_change(
        TRUE,     // notify_L1
        TRUE,     // notify_MM
        gas_id    // gas_id
      );
    }
  }

  return acquisition_complete;

} /* end check_if_bsi_acquisition_complete() */

/*!
 * \brief Decodes the mobile allocation in SI13.
 *
 *  This function operates in two ways:
 *  a) it is called when SI13 is received, and determines if a mobile allocation is present, and partially
 *     decodes it if so. It is assumed at this point that the cell allocation, received in SI1, has not been
 *     received, so the decoding cannot be completed ('cell_alloc' is set to NULL).
 *  b) it is called when SI1 and SI13 have both been received and processed, and completes the decoding of
 *     the mobile allocation in SI13 ('si13' is set to NULL).
 *
 * \param si13 (in)          - rest octets from SI13
 * \param cell_alloc (in)    - cell allocation from SI1
 * \param decoded_si13 (out) - decoded SI13 message
 *
 * \return boolean - TRUE if mobile alloc was either decoded for the first time, or updated; FALSE otherwise.
 */
static boolean decode_si13_mobile_alloc(
  si13_rest_octets_t *si13,
  cell_channel_description_T *cell_alloc,
  si13_data_t *decoded_si13
)
{
  boolean updated = FALSE;

  if ( si13 )
  {
    if ( si13->si13_cm_gprs_ma_flag )
    {
      decoded_si13->mobile_allocation_status = IE_PRESENT;

      rr_gprs_decode_mobile_alloc(
        &si13->gprs_mobile_alloc,
        &decoded_si13->mobile_allocation_ie,
        NULL,
        NULL
      );
    }
    else
    {
      decoded_si13->mobile_allocation_status = IE_NOT_PRESENT;
    }
  }
  else
  if ( decoded_si13->mobile_allocation_status == IE_PRESENT )
  {
    if ( cell_alloc != NULL )
    {
      rr_gprs_decode_mobile_alloc(
        NULL,
        &decoded_si13->mobile_allocation_ie,
        cell_alloc,
        &decoded_si13->mobile_allocation
      );

      decoded_si13->mobile_allocation_status = IE_DECODED;
      updated = TRUE;
    }
  }

  return updated;

} /* end decode_si13_mobile_alloc() */

/*!
 * \brief Post-processing after a SI has been received, decoded & stored.
 *
 * \param gas_id (in)
 */
static void bsi_non_camped_post_processing(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *pcell_info;
  uint16             messages_mask;
  rr_sys_info_si_data_t *rr_sys_info_si_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_si_data_ptr = rr_sys_info_si_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_si_data_ptr);

  pcell_info = rr_gprs_get_pcell_info(gas_id);
  RR_NULL_CHECK_FATAL(pcell_info);

  messages_mask = pcell_info->gsm.update_message_received;

  if ( ARE_ALL_BITS_SET( messages_mask, MASK_SI_1 | MASK_SI_13 ) &&
       pcell_info->bcch_sys_info.si13_data_valid_flag            &&
       (pcell_info->gsm.valid_CA_received == RR_CA_RECEIVED))
  {
    boolean decoded = decode_si13_mobile_alloc(
      NULL,
      &pcell_info->gsm.cell_channel_description,
      &rr_sys_info_si_data_ptr->decoded_si13_data
    );

    if ( decoded )
    {
      MSG_GERAN_MED_0_G("SI13 mobile alloc stored; MA_NUMBER=14");

      rr_gprs_store_mobile_alloc(
        MA_NUMBER_14,
        &rr_sys_info_si_data_ptr->decoded_si13_data.mobile_allocation,
        pcell_info,
        gas_id
      );
    }
  }

  /* Check if SI13 has been received and was expected */
  if ( ARE_SET( MASK_SI_13, pcell_info->gsm.update_message_received ) &&
       pcell_info->bcch_sys_info.si13_expected_flag )
  {
    /* Reset si13_expected_flag */
    pcell_info->bcch_sys_info.si13_expected_flag = FALSE;

    rr_cell_acq_send_imsg_gprs_bsi_received_ind(gas_id);

    /* If SI13 is received after GSM sys info acquisition is complete, */
    /* generate RR_EV_GPRS_SI13_RECEIVED */
    if ( pcell_info->bcch_sys_info.generate_si_received_event )
    {
      rr_send_priority_imsg_rr_event_ind(RR_EV_GPRS_SI13_RECEIVED, gas_id);
      pcell_info->bcch_sys_info.generate_si_received_event = FALSE;
    }
  }

  /* Clear all the bits for SIs received so far */
  CLEAR_BIT(pcell_info->gsm.update_message_received,
            pcell_info->bcch_sys_info.si_messages_required);

  if ( pcell_info->gsm.valid_data_flag )
  {
    pcell_info->gsm.generate_si_received_event = FALSE;
  }

} /* end bsi_non_camped_post_processing() */

/*!
 * \brief Post-processing after a SI has been received, decoded & stored.
 *
 * \param si_message_received (in)
 * \param gas_id (in)
 */
static void bsi_camped_post_processing(
  rr_si_type_t si_message_received,
  const gas_id_t gas_id
)
{
  rr_event_T              rr_event;
  gprs_scell_info_t      *scell_info;
  gprs_bcch_scell_info_t *bcch_sys_info;
  uint16                  si_messages_received;
  rr_sys_info_si_data_t  *rr_sys_info_si_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_si_data_ptr = rr_sys_info_si_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_si_data_ptr);

  scell_info = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_info);

  bcch_sys_info = &scell_info->bcch_sys_info;
  si_messages_received = scell_info->gsm.update_message_received;

  if ( ARE_ALL_BITS_SET( si_messages_received,
                         MASK_SI_1 | MASK_SI_13 ) &&
       (scell_info->gsm.valid_CA_received == RR_CA_RECEIVED))
  {
    boolean decoded = decode_si13_mobile_alloc(
      NULL,
      &scell_info->gsm.cell_channel_description,
      &rr_sys_info_si_data_ptr->decoded_si13_data
    );

    if ( decoded )
    {
      MSG_GERAN_MED_0_G("SI13 mobile alloc stored; MA_NUMBER=14");

      rr_gprs_store_mobile_alloc(
        MA_NUMBER_14,
        &rr_sys_info_si_data_ptr->decoded_si13_data.mobile_allocation,
        scell_info,
        gas_id
      );

      (void) rr_log_gprs_mobile_allocation(
        &rr_sys_info_si_data_ptr->decoded_si13_data.mobile_allocation,
        gas_id
      );
    }
  }
  /* If SI1 is indicated as received, but there is no valid CA in the sys info */
  /* database (which is required for GPRS), get SI1 again (i.e. mark it as not received) */
  else if ( scell_info->gsm.gprs_indicator &&
            ARE_ALL_BITS_SET( si_messages_received, MASK_SI_1 ) &&
            (( scell_info->gsm.valid_CA_received == RR_CA_NOT_RECEIVED) ||
             ( scell_info->gsm.valid_CA_received == RR_CA_RECEIVED_BUT_INVALID)))
  {
    MSG_GERAN_HIGH_0_G("Refreshing SI1 to get CA");

    /* Mark SI1 as required and not yet received */
    bsi_set_messages_required_for_acquisition(
      MASK_SI_1,      // required
      MASK_SI_NONE,   // low_priority
      FALSE,          // reset_history
      gas_id          // gas_id
    );
  }

  /* Make a note of the SI just processed (for use by bsi_acquisition_control) */
  bcch_sys_info->last_si_message_received = si_message_received;

  /* Clear all the bits for SIs received so far */
  CLEAR_BIT(si_messages_received, bcch_sys_info->si_messages_required);

  /*
  Inform main sys info handler about SI reception, which will pass this through to BSI
  handler
  Note: This indication is not passed directly to BSI handler, as this BSI may have been
  received during RACHing for CS, but actually, the UE is camped on PBCCH, so the BSI
  Handler is inactive
  */
  rr_event = rr_gprs_sys_info_acquisition_control(EV_SI_SYS_INFO_MESSAGE_RECEIVED, NULL, gas_id);

  /*
  SI processing complete - clear the indication of the SI just received
  Note: This is required, as some SI actions are triggered by a certain SI being
  received, whereas some due to timer expiry. last_si_message_received can be
  used to differentiate between the two
  */
  bcch_sys_info->last_si_message_received = SI_TYPE_NONE;

  /*
  If GPRS is not supported in the cell (or has just been deactivated), mark the SI13
  data as invalid
  Note: This should be done AFTER bsi_acquisition_control is notified of a SI message
  being received
  */
  if ( ! scell_info->gsm.gprs_indicator )
  {
    scell_info->bcch_sys_info.si13_data_valid_flag = FALSE;
  }

  if ( rr_event != EV_NO_EVENT )
  {
    rr_send_priority_imsg_rr_event_ind(rr_event, gas_id);
  }

} /* end bsi_camped_post_processing() */

/*!
 * \brief Decode SI13 rest octets.
 *
 * \param rest_octets (in)
 * \param decoded_msg (out)
 * \param gas_id (in)
 *
 * \return csn_decoder_error_t
 */
static csn_decoder_error_t decode_si13_ro(
  uint8 *rest_octets,
  si13_data_t *decoded_msg,
  const gas_id_t gas_id
)
{
  si13_rest_octets_t si13 = {0};
  uint8 remaining_length = 0;

  csn_decoder_error_t error_code;

  remaining_length = rr_length_to_end_of_ota_msg(
    SYSTEM_INFORMATION_TYPE_13,   // msg_type
    rest_octets,                  // csn1_start_ie_ptr
    SI13_REST_OCTETS,             // csn_ie_type
    gas_id
  );

  error_code = gcsn1_rr_decoder(
    SI13_REST_OCTETS,   // message_type
    rest_octets,        // csn1_is_src
    (void *) &si13,     // dst
    remaining_length,   // len_to_end_of_buf
    gas_id
  );

  /* If an error occurred whilst decoding the message content, abort and ignore it */
  if ( error_code != CSN_DECODE_NO_ERROR )
  {
    return error_code;
  }

  /* empty msg, ignore */
  if (si13.si13_info_included == FALSE )
  {
    return IGNORE_ERROR;
  }

  /* Perform some range checking */
  if ( si13.pbcch_present_flag == FALSE )
  {
    /* Check that the NMO isn't "reserved" */
    if ( si13.gprs_cell_options.nmo == NMO_RESERVED )
    {
      return IGNORE_ERROR;
    }
  }
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  else if (rr_multi_sim_mode_enabled())
  {
    /* PBCCH is present in the cell, ignore the entire SI13 message */
    return IGNORE_ERROR;
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  decoded_msg->bcch_change_mark = si13.bcch_chng_mrk;
  decoded_msg->si_change_field = si13.si_chng_field;
  decoded_msg->si13_change_mark = si13.si13_change_mark;

  (void)decode_si13_mobile_alloc( &si13, NULL, decoded_msg );

  decoded_msg->bcch_pbcch.bcch.rac = si13.rac;
  decoded_msg->bcch_pbcch.bcch.spgc_ccch_sup = si13.spgc_ccch_sup;
  decoded_msg->bcch_pbcch.bcch.network_control_order = si13.network_ctrl_ordr;
  decoded_msg->bcch_pbcch.bcch.priority_access_thr = si13.priority_access_thr;

  decoded_msg->bcch_pbcch.bcch.gprs_cell_options.nmo = si13.gprs_cell_options.nmo;
  decoded_msg->bcch_pbcch.bcch.gprs_cell_options.t3168 = si13.gprs_cell_options.t3168;
  decoded_msg->bcch_pbcch.bcch.gprs_cell_options.t3192 = si13.gprs_cell_options.t3192;
  decoded_msg->bcch_pbcch.bcch.gprs_cell_options.drx_timer_max =
    rr_gprs_decode_drx_timer( si13.gprs_cell_options.drx_timer_max );
  decoded_msg->bcch_pbcch.bcch.gprs_cell_options.access_burst_type =
    si13.gprs_cell_options.access_burst_type;
  decoded_msg->bcch_pbcch.bcch.gprs_cell_options.ctrl_ack_type =
    si13.gprs_cell_options.ctrl_ack_type;
  decoded_msg->bcch_pbcch.bcch.gprs_cell_options.bs_cv_max =
    si13.gprs_cell_options.bs_cv_max;

  if ( si13.gprs_cell_options.pan_flag == valid )
  {
    decoded_msg->bcch_pbcch.bcch.gprs_cell_options.pan_dec =
      si13.gprs_cell_options.pan_dec;
    decoded_msg->bcch_pbcch.bcch.gprs_cell_options.pan_inc =
      si13.gprs_cell_options.pan_inc;
    decoded_msg->bcch_pbcch.bcch.gprs_cell_options.pan_max =
      si13.gprs_cell_options.pan_max;
  }
  else
  {
    decoded_msg->bcch_pbcch.bcch.gprs_cell_options.pan_dec = 0;
    decoded_msg->bcch_pbcch.bcch.gprs_cell_options.pan_inc = 0;
    decoded_msg->bcch_pbcch.bcch.gprs_cell_options.pan_max = 0;
  }

  decoded_msg->bcch_pbcch.bcch.gprs_cell_options.optional_extension_info_flag =
    si13.gprs_cell_options.optional_extension_info_flag;

  decoded_msg->
    bcch_pbcch.bcch.gprs_cell_options.
      optional_extension_info.egprs_supported_flag =
        si13.gprs_cell_options.optional_extension_info.egprs_supported_flag;
  decoded_msg->
    bcch_pbcch.bcch.gprs_cell_options.
      optional_extension_info.egprs_packet_channel_request =
        si13.gprs_cell_options.optional_extension_info.egprs_packet_channel_request;
  decoded_msg->
    bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.bep_period =
      si13.gprs_cell_options.optional_extension_info.bep_period;

  decoded_msg->
    bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.pfc_feature_mode =
      si13.gprs_cell_options.optional_extension_info.pfc_feature_mode;

  decoded_msg->bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.ccn_active =
    si13.gprs_cell_options.optional_extension_info.ccn_active;

  decoded_msg->
    bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.nw_ext_tbf =
      si13.gprs_cell_options.optional_extension_info.nw_ext_tbf;
  decoded_msg->
    bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.ext_utbf_no_data =
      si13.gprs_cell_options.optional_extension_info.ext_utbf_no_data;

  #ifdef FEATURE_GSM_DTM
  if ( rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID) )
  {
    decoded_msg->
      bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.dtm_support =
        si13.gprs_cell_options.optional_extension_info.dtm_support;
    #ifdef FEATURE_GSM_EDTM
    decoded_msg->
      bcch_pbcch.bcch.gprs_cell_options.
        optional_extension_info.dtm_enhancements_capability =
          si13.gprs_cell_options.optional_extension_info.dtm_enhancements_capability;
    #endif /* FEATURE_GSM_EDTM */
  }
  else
  {
    decoded_msg->
      bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.dtm_support = FALSE;
    #ifdef FEATURE_GSM_EDTM
    decoded_msg->
      bcch_pbcch.bcch.gprs_cell_options.
        optional_extension_info.dtm_enhancements_capability = FALSE;
    #endif /* FEATURE_GSM_EDTM */
  }
  #endif /* FEATURE_GSM_DTM */

  decoded_msg->bcch_pbcch.bcch.power_control_params.alpha = si13.alpha;
  decoded_msg->bcch_pbcch.bcch.power_control_params.t_avg_w = si13.t_avg_w;
  decoded_msg->bcch_pbcch.bcch.power_control_params.t_avg_t = si13.t_avg_t;
  decoded_msg->bcch_pbcch.bcch.power_control_params.pc_meas_chan = si13.pc_meas_chan;
  decoded_msg->bcch_pbcch.bcch.power_control_params.n_avg_i = si13.n_avg_i;
  decoded_msg->bcch_pbcch.bcch.power_control_params.int_meas_chan_list_avail = FALSE;

  /* The SGSNR bit indicates the SGSN software rev. It has three states, depending on */
  /* a) its presence                                                                  */
  /* b) whether it is 0 or 1.                                                         */
  /* not present=R97, 0=R98, 1=R99                                                    */
  if ( si13.sgsnr_flag )
  {
    decoded_msg->sgsnr = ( si13.sgsnr ) ? SGSNR_REV_99 : SGSNR_REV_98;
  }
  else
  {
    decoded_msg->sgsnr = SGSNR_NOT_PRESENT;
  }

  decoded_msg->si_status_ind = si13.si_status_ind;

  if (si13.lb_ms_txpwr_max_cch == INVALID_LB_MS_TXPWR_MAX_CCH)
  {
    decoded_msg->LB_MS_TXPWR_MAX_CCH_valid = FALSE;
    decoded_msg->LB_MS_TXPWR_MAX_CCH = 0;
  }
  else
  {
    decoded_msg->LB_MS_TXPWR_MAX_CCH_valid = TRUE;
    decoded_msg->LB_MS_TXPWR_MAX_CCH = si13.lb_ms_txpwr_max_cch;
  }

  return error_code;

} /* end decode_si13_ro() */

/*!
 * \brief Processes the BCCH_CHANGE_MARK parameter.
 *
 * \param bcch_change_mark (in)
 * \param si_change_field (in)
 * \param cell_info (in)
 *
 * \return uint16
 */
static uint16 process_bcch_change_mark(
  uint8 bcch_change_mark,
  uint8 si_change_field,
  gprs_scell_info_t *cell_info
)
{
  uint16 stored_bcch_change_mark;
  uint16 messages_changed = 0;   /* Assume no messages are changed */
  uint16 messages_required = 0;  /* Assume no messages are changed */

  RR_NULL_CHECK_FATAL(cell_info);

  stored_bcch_change_mark = cell_info->bcch_sys_info.bcch_change_mark;

  /*
  5.5.1.3.1 Supervision of BCCH_CHANGE_MARK and update of BCCH information
  If the MS detects that the value has been incremented by one unit, compared to the
  previous value, the MS shall perform a partial acquisition of BCCH information.
  The information that shall be received is determined by the SI_CHANGE_FIELD parameter
  */
  if ( bcch_change_mark == ( ( stored_bcch_change_mark + 1 ) & 7 ) )
  {
    /*
    5.5.1.3.1 Supervision of BCCH_CHANGE_MARK and update of BCCH information
    If the SI_CHANGE_FIELD parameter indicates an update of an unspecified type or type
    of SI messages, the MS shall receive at least one message instance within each
    consistent set of SI messages on BCCH. It shall also receive all SI messages on BCCH
    not belonging to a consistent set.
    */

    /* SI_CHANGE_FIELD=0 (0000b) Update of unspecified SI message(s) */
    if ( si_change_field == 0x00 )
    {
      messages_changed = MASK_FULL_BSI_ACQUISITION_NO_SI13;
    }
    else

    /* SI_CHANGE_FIELD=1 (0001b) Update of SI1 */
    if ( si_change_field == 0x01 )
    {
      messages_changed = MASK_SI_1;
    }
    else

    /* SI_CHANGE_FIELD=2 (0010b) Update of SI2, SI2bis or SI2ter */
    if ( si_change_field == 0x02 )
    {
      messages_changed = MASK_SI_2 |
                         MASK_SI_2_BIS |
                         MASK_SI_2_TER |
                         MASK_SI_2_TER_REST |
                         MASK_SI_2_QUATER;
    }
    else

    /* SI_CHANGE_FIELD=3 (0011b) Update of SI3, SI4, SI7 or SI8 */
    /* N.B. We currently don't read SI7 or SI8 for the serving cell */
    if ( si_change_field == 0x03 )
    {
      messages_changed = MASK_SI_3 | MASK_SI_4;

      /*
      If SI2quater or SI2ter were not previously being broadcast, then if SI3 has been
      updated, this may now indicate that SI2quater or SI2ter is being broadcast.
      However, it is more probable that the network would order a full refresh by
      adding more than one to the BCCH_CHANGE_MARK. For this reason, no change to the
      status of SI2quater or SI2ter status is noted here - any change will be determined
      when SI3 is received
      */
    }
    else

    /* SI_CHANGE_FIELD=4 (0100b) Update of SI9 (not supported) */
    if ( si_change_field == 0x04 )
    {
      ;
    }
    else

    /* 5.5.1.3.1 Supervision of BCCH_CHANGE_MARK and update of BCCH information           */
    /* If the SI_CHANGE_FIELD parameter indicates an update of an unknown type of SI      */
    /* message, the MS is not required to update any BCCH information.                    */
    {
      ;
    }
  }
  else

  /* 5.5.1.3.1 Supervision of BCCH_CHANGE_MARK and update of BCCH information           */
  /* If the mobile station detects that the BCCH_CHANGE_MARK value has been incremented */
  /* by more than one unit, compared to the previous value, the MS shall perform a      */
  /* complete acquisition of BCCH messages                                              */
  {
    messages_changed = MASK_FULL_BSI_ACQUISITION_NO_SI13;

    /* Now don't know which messages are being broadcast or not */
    cell_info->bcch_sys_info.si_messages_not_broadcast = 0;
  }

  if ( messages_changed != MASK_SI_NONE )
  {
    /* Now filter out any messages which we know are not being broadcast */
    /* si_messages_not_present: 1=not present, 0=present/unknown */
    messages_required = messages_changed &
                        ( ~ cell_info->bcch_sys_info.si_messages_not_broadcast );

    MSG_GERAN_HIGH_2("SI messages changed 0x%x, require 0x%x",
             messages_changed,messages_required);
  }

  return messages_required;

} /* end process_bcch_change_mark() */

/*!
 * \brief Processes the BCCH_CHANGE_MARK parameter and returns an indication is SIs required.
 *
 * \param cell_info (in)
 * \param bcch_change_mark (in)
 * \param si_change_field (in)
 *
 * \return uint16 - bit-mask of SI messages changed
 */
static uint16 check_if_bcch_change_mark_changed(
  gprs_scell_info_t *cell_info,
  uint8 bcch_change_mark,
  uint8 si_change_field
)
{
  uint16 messages_required = 0;

  if ( cell_info->bcch_sys_info.bcch_change_mark != bcch_change_mark )
  {
    MSG_GERAN_HIGH_3("BCCH_CHNGE_MRK %d->%d SI_CHNGE_FLD=%d",
             cell_info->bcch_sys_info.bcch_change_mark,bcch_change_mark,si_change_field);

    messages_required =
      process_bcch_change_mark( bcch_change_mark, si_change_field, cell_info );
  }

  return messages_required;

} /* end check_if_bcch_change_mark_changed() */

/*!
 * \brief Processes the SI13 scheduling information after SI13 is received.
 *
 * \param si13_data (in)
 * \param scell_info (in)
 * \param gas_id (in)
 */
static void process_si13_scheduling_information(
  si13_data_t * si13_data,
  gprs_scell_info_t * scell_info,
  const gas_id_t gas_id
)
{
  /* Before comparing the new and old change-mark information, check that we have previously */
  /* received SI13, so there is something to compare against */
  if ( scell_info->bcch_sys_info.si13_data_valid_flag )
  {
    uint16 messages_changed;

    /* Check if the BCCH_CHANGE_MARK has changed, and work out which messages need to be */
    /* read as a result */
    messages_changed = check_if_bcch_change_mark_changed(
      scell_info,
      si13_data->bcch_change_mark,
      si13_data->si_change_field
    );

    if ( messages_changed != MASK_SI_NONE )
    {
      /* If SI1 has been changed, mark the Cell Allocation as invalid */
      if ( messages_changed & MASK_SI_1 )
      {
        scell_info->gsm.valid_CA_received = RR_CA_NOT_RECEIVED;
        scell_info->gsm.last_tc = 0;
      }

      /* Cached Sys Info should be flushed for the serving cell */
      if (scell_info->gsm.cell_identity_valid_flag)
      {
        MSG_GERAN_HIGH_0_G("SI13 changed flush SI cache");
        rr_csi_flush_data(&scell_info->gsm.location_area_identification,
                          &scell_info->gsm.cell_identity,
                          CSI_FLUSH_SI_CHANGE,
                          gas_id);
      }

      /* Mark which messages we are now expecting to receive. Don't reset receive history */
      bsi_set_messages_required_for_acquisition(
        messages_changed,   // required
        MASK_SI_NONE,       // low_priority
        FALSE,              // reset_history
        gas_id
      );

      /* attempt to send a sys info status if the UE is currently in transfer mode */
      rr_send_sysinfo_status_on_bcch_change_mark_update(scell_info, gas_id);
#ifdef FEATURE_FAKE_BTS_CELL_INFO
      if (rr_get_fake_bs_detection_enabled(gas_id) == TRUE)
      {
        scell_info->bcch_sys_info.si_refresh_ongoing = TRUE;
      }
#endif /*FEATURE_FAKE_BTS_CELL_INFO*/
    }
  }

  /* Store updated BCCH change mark */
  scell_info->bcch_sys_info.bcch_change_mark = si13_data->bcch_change_mark;

} /* end process_si13_scheduling_information() */

/*!
 * \brief This function is called to process a SI13 or PSI13 message.
 *
 * \param si13_data (in)
 * \param gas_id (in)
 */
static void process_si13_or_psi13(
  si13_data_t *si13_data,
  const gas_id_t gas_id
)
{
  rrps_store_update_data_t   *store_update_ptr;
  boolean                     update_l1 = FALSE;
  boolean                     update_mm = FALSE;
  rrps_store_update_bitmap_t  store_update_bitmap = 0;
  gprs_scell_info_t          *scell_info;
  rr_sys_info_si_data_t      *rr_sys_info_si_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(si13_data);

  // Obtain a pointer to the module data
  rr_sys_info_si_data_ptr = rr_sys_info_si_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_si_data_ptr);

  scell_info = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_info);

  store_update_ptr = rr_get_rrps_store_update(gas_id);
  RR_NULL_CHECK_FATAL(store_update_ptr);

  if ( scell_info->bcch_sys_info.si13_data_valid_flag )
  {
    /* Examine the BCCH_CHANGE_MARK to see if any SIs have been changed */
    process_si13_scheduling_information(
      si13_data,    // si13_data
      scell_info,   // cell_info
      gas_id        // gas_id
    );

    /* The mobile allocation cannot be fully decoded in this thread, because it may require */
    /* the cell allocation, which is given in SI1. Therefore, if the mobile allocation is */
    /* present, some partial decoding will have been done in decode_si13_ro(), but this */
    /* cannot be completed until SI1 has been received. If SI13_CHANGE_MARK has not */
    /* changed, then the mobile allocation is the same, so it is marked as already decoded */
    /* here, and the post-processing is not performed */
    if ( si13_data->mobile_allocation_status == IE_PRESENT )
    {
      if ( scell_info->mobile_allocations.si13_change_mark != si13_data->si13_change_mark )
      {
        MSG_GERAN_MED_0_G("SI13 mobile alloc changed");

        if ( scell_info->gsm.valid_CA_received == RR_CA_RECEIVED)
        {
          MSG_GERAN_MED_0_G("CA valid; MA_NUMBER=14 stored");

          (void) rr_gprs_process_si13_mobile_alloc(
            si13_data,
            &scell_info->gsm.cell_channel_description,
            scell_info,
            gas_id
          );

          (void) rr_log_gprs_mobile_allocation(
            &rr_sys_info_si_data_ptr->decoded_si13_data.mobile_allocation,
            gas_id
          );
        }
        else
        {
          MSG_GERAN_MED_0_G("Require CA (SI1) before MA decode");
        }

        scell_info->mobile_allocations.si13_change_mark = si13_data->si13_change_mark;

        store_update_bitmap |= RRPS_UPD_CHANGE_MARK_SI13;
        store_update_ptr->change_mark_si13 = si13_data->si13_change_mark;
      }
    }

    /* LOG: if incoming data is inconsistent with global system info database */

    /* LOG: GPRS GENERAL PARAMETERS */
    if (scell_info->bcch_sys_info.spgc_ccch_sup != si13_data->bcch_pbcch.bcch.spgc_ccch_sup ||
      scell_info->bcch_sys_info.priority_access_thr != si13_data->bcch_pbcch.bcch.priority_access_thr ||
      scell_info->bcch_sys_info.nc_meas_params_si13.network_control_order != si13_data->bcch_pbcch.bcch.network_control_order ||
      scell_info->bcch_sys_info.routing_area_code != si13_data->bcch_pbcch.bcch.rac ||
      scell_info->bcch_sys_info.sgsnr != si13_data->sgsnr ||
      scell_info->bcch_sys_info.si_status_ind != si13_data->si_status_ind)
    {
      (void) rr_log_gprs_general_parameters(
        si13_data->bcch_pbcch.bcch.spgc_ccch_sup,
        si13_data->bcch_pbcch.bcch.priority_access_thr,
        si13_data->bcch_pbcch.bcch.network_control_order,
        si13_data->bcch_pbcch.bcch.rac,
        (uint8) si13_data->sgsnr,
        gas_id
      );
    }

    /* LOG: GPRS CELL OPTIONS */
    if (rr_si_diff_gprs_cell_options(
          &si13_data->bcch_pbcch.bcch.gprs_cell_options,   // gprs_cell_options_ptr
          gas_id                                           // gas_id
        ))
    {
      (void) rr_log_gprs_cell_options(                     // gprs_cell_options
        &(si13_data->bcch_pbcch.bcch.gprs_cell_options),   // gas_id
        gas_id
      );
    }

    /* LOG: GPRS POWER CONTROL PARAMETERS */
    if (scell_info->bcch_sys_info.power_control_params.alpha != si13_data->bcch_pbcch.bcch.power_control_params.alpha ||
      scell_info->bcch_sys_info.power_control_params.t_avg_w != si13_data->bcch_pbcch.bcch.power_control_params.t_avg_w ||
      scell_info->bcch_sys_info.power_control_params.t_avg_t != si13_data->bcch_pbcch.bcch.power_control_params.t_avg_t ||
      scell_info->bcch_sys_info.power_control_params.pc_meas_chan != si13_data->bcch_pbcch.bcch.power_control_params.pc_meas_chan ||
      scell_info->bcch_sys_info.power_control_params.n_avg_i != si13_data->bcch_pbcch.bcch.power_control_params.n_avg_i)
    {
      (void) rr_log_gprs_power_control_parameters(
        si13_data->bcch_pbcch.bcch.power_control_params.alpha,
        si13_data->bcch_pbcch.bcch.power_control_params.t_avg_w,
        si13_data->bcch_pbcch.bcch.power_control_params.t_avg_t,
        si13_data->bcch_pbcch.bcch.power_control_params.pc_meas_chan,
        si13_data->bcch_pbcch.bcch.power_control_params.n_avg_i,
        gas_id
      );
    }

    if (scell_info->bcch_sys_info.sgsnr != si13_data->sgsnr)
    {
      MSG_GERAN_MED_2_G("SGSNR bit changed (%d->%d)",scell_info->bcch_sys_info.sgsnr,si13_data->sgsnr);
      update_mm = TRUE;
    }

    /* PBCCH not present - Message contains GPRS Cell Options, Power Control parameters, etc */

    /*
    GPRS Cell Options
    -----------------

    Changes in the GPRS Cell Options require the following updates:

                              L1        MAC      RLC      MM

      drx_timer_max **
      t3168                              X
      t3192                              X        X
      access_burst_type       X          X
      ctrl_ack_type                      X
      bs_cv_max                          X        X
      pan_dec                            X        X
      pan_inc                            X        X
      pan_max                            X        X
      nmo                     X                           X

       ** non_drx_timer is derived from drx_timer_max. If non_drx_timer changes, L1 & MAC must be updated
    */
    /* GPRS Cell Options: DRX_TIMER_MAX - update L1, MAC if NON_DRX_TIMER value changes */
    if (scell_info->bcch_sys_info.gprs_cell_options.drx_timer_max !=
        si13_data->bcch_pbcch.bcch.gprs_cell_options.drx_timer_max )
    {
      uint8 non_drx_timer = rr_gprs_calc_non_drx_timer(
        si13_data->bcch_pbcch.bcch.gprs_cell_options.drx_timer_max,
        gas_id
      );

      if (scell_info->bcch_sys_info.non_drx_timer != non_drx_timer)
      {
        update_l1 = TRUE;

        store_update_bitmap |= RRPS_UPD_MS_NON_DRX_TIMER;
        store_update_ptr->non_drx_timer = non_drx_timer;

        scell_info->bcch_sys_info.non_drx_timer = non_drx_timer;
      }
    }

    /* GPRS Cell Options: ACCESS_BURST - update L1, MAC */
    if (scell_info->bcch_sys_info.gprs_cell_options.access_burst_type !=
        si13_data->bcch_pbcch.bcch.gprs_cell_options.access_burst_type)
    {
      /* Update L1 */
      update_l1 = TRUE;

      /* Update public store */
      store_update_bitmap |= RRPS_UPD_CELL_OPTIONS;
      store_update_ptr->gprs_cell_options = si13_data->bcch_pbcch.bcch.gprs_cell_options;
    }
    else
    /* GPRS Cell Options: BS_CV_MAX, PAN_DEC, PAN_INC, PAN_MAX, T3192 - update RLC, MAC */
    if (    ( scell_info->bcch_sys_info.gprs_cell_options.bs_cv_max != si13_data->bcch_pbcch.bcch.gprs_cell_options.bs_cv_max )
         || ( scell_info->bcch_sys_info.gprs_cell_options.pan_dec != si13_data->bcch_pbcch.bcch.gprs_cell_options.pan_dec )
         || ( scell_info->bcch_sys_info.gprs_cell_options.pan_inc != si13_data->bcch_pbcch.bcch.gprs_cell_options.pan_inc )
         || ( scell_info->bcch_sys_info.gprs_cell_options.pan_max != si13_data->bcch_pbcch.bcch.gprs_cell_options.pan_max )
         || ( scell_info->bcch_sys_info.gprs_cell_options.t3192 != si13_data->bcch_pbcch.bcch.gprs_cell_options.t3192 ) )
    {
      store_update_bitmap |= RRPS_UPD_CELL_OPTIONS;
      store_update_ptr->gprs_cell_options = si13_data->bcch_pbcch.bcch.gprs_cell_options;
    }
    else
    /* GPRS Cell Options: CTRL_ACK_TYPE, NMO, T3168 - update MAC */
    if (    ( scell_info->bcch_sys_info.gprs_cell_options.ctrl_ack_type != si13_data->bcch_pbcch.bcch.gprs_cell_options.ctrl_ack_type )
         || ( scell_info->bcch_sys_info.gprs_cell_options.nmo != si13_data->bcch_pbcch.bcch.gprs_cell_options.nmo )
         || ( scell_info->bcch_sys_info.gprs_cell_options.t3168 != si13_data->bcch_pbcch.bcch.gprs_cell_options.t3168 ) )
    {
      store_update_bitmap |= RRPS_UPD_CELL_OPTIONS;
      store_update_ptr->gprs_cell_options = si13_data->bcch_pbcch.bcch.gprs_cell_options;
    }
      else
      /* GPRS Cell Options: egprs_supported_flag, egprs_packet_channel_request */
      if ( si13_data->bcch_pbcch.bcch.gprs_cell_options.optional_extension_info_flag == valid )
      {
        if ( ( si13_data->bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.egprs_supported_flag == valid ) &&
             ( scell_info->bcch_sys_info.gprs_cell_options.optional_extension_info.egprs_packet_channel_request !=
               si13_data->bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.egprs_packet_channel_request ) )
                     {
          store_update_bitmap |= RRPS_UPD_CELL_OPTIONS;
          store_update_ptr->gprs_cell_options = si13_data->bcch_pbcch.bcch.gprs_cell_options;
        }
        else if ( ( scell_info->bcch_sys_info.gprs_cell_options.optional_extension_info.egprs_supported_flag !=
                    si13_data->bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.egprs_supported_flag ) )
        {
          store_update_bitmap |= RRPS_UPD_CELL_OPTIONS;
          store_update_ptr->gprs_cell_options = si13_data->bcch_pbcch.bcch.gprs_cell_options;
        }

        if ( ( si13_data->bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.egprs_supported_flag == valid ) &&
             ( scell_info->bcch_sys_info.gprs_cell_options.optional_extension_info.bep_period !=
                 si13_data->bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.bep_period ) )

        {
          update_l1 = TRUE;
        }
      }
      else if ( scell_info->bcch_sys_info.gprs_cell_options.optional_extension_info.egprs_supported_flag == valid )
      {
        store_update_bitmap |= RRPS_UPD_CELL_OPTIONS;
        store_update_ptr->gprs_cell_options = si13_data->bcch_pbcch.bcch.gprs_cell_options;
      }

    if ( si13_data->bcch_pbcch.bcch.gprs_cell_options.optional_extension_info_flag == valid )
    {
      /* If NW EXT UL TBF changes, Update Public store */
      if ( scell_info->bcch_sys_info.gprs_cell_options.optional_extension_info.nw_ext_tbf !=
         si13_data->bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.nw_ext_tbf )
      {
        store_update_bitmap |= RRPS_UPD_CELL_OPTIONS;
        store_update_ptr->gprs_cell_options = si13_data->bcch_pbcch.bcch.gprs_cell_options;
      }
      /* if ext_utbf_no_data changes then update GL1*/
      if ( scell_info->bcch_sys_info.gprs_cell_options.optional_extension_info.ext_utbf_no_data !=
         si13_data->bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.ext_utbf_no_data )
      {
        update_l1 = TRUE;

        store_update_bitmap |= RRPS_UPD_CELL_OPTIONS;
        store_update_ptr->gprs_cell_options = si13_data->bcch_pbcch.bcch.gprs_cell_options;
      }
    }

    /* If NMO changes, update MM (send RR_SERVICE_IND) and L1 */
    if ( scell_info->bcch_sys_info.gprs_cell_options.nmo != si13_data->bcch_pbcch.bcch.gprs_cell_options.nmo )
    {
      update_l1 = TRUE;
      update_mm = TRUE;

      MSG_GERAN_MED_2_G("Change of NMO (%d->%d)",scell_info->bcch_sys_info.gprs_cell_options.nmo,si13_data->bcch_pbcch.bcch.gprs_cell_options.nmo);
    }
    else
    if ( scell_info->bcch_sys_info.routing_area_code != si13_data->bcch_pbcch.bcch.rac )
    {
      update_mm = TRUE;
    }

    if ( scell_info->bcch_sys_info.priority_access_thr !=
         si13_data->bcch_pbcch.bcch.priority_access_thr )
    {
      MSG_GERAN_MED_3_G("Change of PRIORITY_ACCESS_THR (%d->%d)",
              scell_info->bcch_sys_info.priority_access_thr,
              si13_data->bcch_pbcch.bcch.priority_access_thr,
              0);

      store_update_bitmap |= RRPS_UPD_PRIORITY_ACCESS_THR;
      store_update_ptr->priority_access_thr = si13_data->bcch_pbcch.bcch.priority_access_thr;
    }

    if (scell_info->bcch_sys_info.LB_MS_TXPWR_MAX_CCH_valid != si13_data->LB_MS_TXPWR_MAX_CCH_valid)
    {
      update_l1 = TRUE;
    }
    else if (si13_data->LB_MS_TXPWR_MAX_CCH_valid &&
             scell_info->bcch_sys_info.LB_MS_TXPWR_MAX_CCH != si13_data->LB_MS_TXPWR_MAX_CCH)
    {
      update_l1 = TRUE;
    }

    /* Now copy over SI13 data to the scell data store */

    scell_info->bcch_sys_info.routing_area_code = si13_data->bcch_pbcch.bcch.rac;
    scell_info->bcch_sys_info.spgc_ccch_sup = si13_data->bcch_pbcch.bcch.spgc_ccch_sup;
    scell_info->bcch_sys_info.priority_access_thr = si13_data->bcch_pbcch.bcch.priority_access_thr;
    scell_info->bcch_sys_info.nc_meas_params_si13.valid = TRUE;
    scell_info->bcch_sys_info.nc_meas_params_si13.network_control_order = si13_data->bcch_pbcch.bcch.network_control_order;
    scell_info->bcch_sys_info.gprs_cell_options = si13_data->bcch_pbcch.bcch.gprs_cell_options;
    scell_info->bcch_sys_info.power_control_params = si13_data->bcch_pbcch.bcch.power_control_params;

    /* Store the SGSNR bit status in BCCH sys info (PBCCH sys info does not include PSI13) */
    scell_info->bcch_sys_info.sgsnr = si13_data->sgsnr;
    scell_info->bcch_sys_info.si_status_ind = si13_data->si_status_ind;

    scell_info->bcch_sys_info.LB_MS_TXPWR_MAX_CCH_valid = si13_data->LB_MS_TXPWR_MAX_CCH_valid;
    scell_info->bcch_sys_info.LB_MS_TXPWR_MAX_CCH = si13_data->LB_MS_TXPWR_MAX_CCH;

  MSG_GERAN_HIGH_3_G("REL4 flags: OPT %1d CCN %1d EXT_ULTBF %1d",
           scell_info->bcch_sys_info.gprs_cell_options.optional_extension_info_flag,
           scell_info->bcch_sys_info.gprs_cell_options.optional_extension_info.ccn_active,
           scell_info->bcch_sys_info.gprs_cell_options.optional_extension_info.nw_ext_tbf);

    /* Update Public Store if necessary */
    if (store_update_bitmap)
    {
      rr_write_public_store(
        store_update_bitmap,
        store_update_ptr,
        gas_id
      );
    }

    /* Update L1 and MM with new parameters as required */
    rr_perform_MM_or_L1_notification_on_serving_SI_change(
      update_l1,
      update_mm,
      gas_id
    );

    /* Mark SI13 as received */
    SET_BIT( MASK_SI_13, scell_info->gsm.update_message_received );
  }
  else
  {
    /* Store the decoded SI13 data in the scell database. This will also decode */
    /* the mobile allocation if present in SI13 */
    /* N.B. This also marks SI13 as received and the data valid */
    rr_gprs_store_si13_or_psi13_data(
      si13_data,    // si13_data
      scell_info,   // cell_info
      NULL,         // store_update_data
      gas_id        // gas_id
    );

    if (rr_gsm_camped_on_ccch(gas_id))
    {
      MSG_GERAN_HIGH_0_G("Received SI13 after starting GSM idle mode, switching to GPRS idle mode");
      rr_send_imsg(RR_IMSG_L1_IDLE_MODE_CHANGE_REQUIRED, gas_id);
    }
  }

  {
    eng_mode_scell_info_t eng_mode_scell_info = {0};

    eng_mode_scell_info = geran_eng_mode_format_scell_info(
      scell_info,          // scell_info_ptr
      ENG_MODE_NO_CAUSE,   // cause
      gas_id               // gas_id
    );

    geran_eng_mode_data_write(
      ENG_MODE_SCELL_INFO,            // geran_eng_mode_info_type
      (void*)&eng_mode_scell_info,    // param_copy
      TRUE,                           // valid_flag
      gas_id                          // gas_id
    );
  }

  rr_eng_mode_update_scell_config(gas_id);

  rr_eng_mode_update_nw_params(gas_id);

  rr_free_rrps_store_update(
    store_update_ptr,   // ptr
    gas_id              // gas_id
  );

} /* end process_si13_or_psi13() */

#if 0  // for future optimisation - not used at present

/*!
 * \brief Calculates the time it may take to acquire all Si2quater instances.
 *
 * \param scell_ptr - pointer to current serving cell store
 *
 * \return uint32 - Acquisition time in ms for all Si2q instances, as a 32-bit integer
 */
static uint32 calc_si2q_schedule(
  gprs_scell_info_t *scell_ptr
)
{
  instances_T *siq2_inst_ptr;
  uint32       result;

  si2q_inst_ptr = rr_get_instances_ptr(SI2Q_INSTANCE,gas_id);
  if (si2q_inst_ptr != NULL)
  {
    result = (uint32)(si2q_inst_ptr->num_instances);
  }
  else
  {
    result = 0UL;
  }

  /* Extract from 3GPP 45.002, section 6.3.1.3
   * System information type 2 quater is sent if needed, as determined by
   * the system operator. If sent on BCCH Norm, it shall be sent when TC = 5
   * if neither of SI2bis and SI2ter are used, otherwise it shall be sent
   * at least once within any of 4 consecutive occurrences of TC = 4.
   * If sent on BCCH Ext, it is sent at least once within any of 4 consecutive
   * occurrences of TC = 5.
   */
  if (scell_ptr->gsm.two_quater_position == SI2Q_POSITION_BCCH_Ext)
  {
    /* Si2quater on TC=5, sent at least once per 4 consecutive TC cycles */
    result *= (ONE_FULL_TC_CYCLE << 2UL);
  }
  else
  {
    if ((scell_ptr->gsm.decode_SI2bis == RR_SI_NOT_REQUIRED) &&
        (scell_ptr->gsm.decode_SI2ter == RR_SI_NOT_REQUIRED))
    {
      /* Si2quater on TC=5, sent at least once per single TC cycle */
      result *= ONE_FULL_TC_CYCLE;
    }
    else
    {
      /* Si2quater on TC=4, sent at least once per 4 consecutive TC cycles */
      result *= (ONE_FULL_TC_CYCLE << 2UL);
    }
  }

  return(result);
}

/*!
 * \brief Extends the timer to ensure that all available Si2quater instances are rcvd.
 *
 * \param scell_ptr (in) - pointer to current serving cell store
 * \param gas_id (in) - pointer to current serving cell store
 */
static void bsi_extend_acquisition_timer_for_si2q(
  gprs_scell_info_t *scell_ptr,
  const gas_id_t gas_id
)
{
  uint32 si2q_extend = calc_si2q_schedule(scell_ptr);

  /* check whether all instances can be acquired within the normal time period */
  if (si2q_extend > GRR_TIMEOUT_BCCH_SI_ACQUISITION)
  {
    /* acquisition may take longer than the current timer */
    /* setting, so calculate the ADDITIONAL time required */
    si2q_extend -= GRR_TIMEOUT_BCCH_SI_ACQUISITION;

    /* and then extend the acquisition timer by this amount */
    if (rr_extend_timer(GRR_SYS_INFO_ACQUISITION_TIMER, si2q_extend, gas_id))
    {
      MSG_GERAN_HIGH_1_G("Extended acquisition timer by %dms for SI2quater", (int)si2q_extend);
    }
  }

  return;
}

#endif /* #ifdef 0 */

/*!
 * \brief Returns a bitmask indicating the SI messages which are being broadcast on BCCH-Ext.
 * 
 * \param cell_info_ptr (in)
 * 
 * \return uint16 - SI bitmask (1=BCCH-Ext)
 */
static uint16 rr_si_messages_on_bcch_ext(gprs_scell_info_t *cell_info_ptr)
{
  RR_NULL_CHECK_RETURN_PARAM(cell_info_ptr, MASK_SI_NONE);

  return cell_info_ptr->bcch_sys_info.si_messages_on_bcch_ext;
}

/*-----------------------------------------------------------------------------
 * Externalized Function Definitions
 * --------------------------------------------------------------------------*/

/*!
 * \brief Process SI13 for the pending/serving cell.
 *
 * \param bcch_arfcn (in) - ARFCN where the message is received.
 * \param gas_id (in)
 */
void rr_process_si13_from_bcch(
  ARFCN_T              bcch_arfcn,
  dl_unit_data_ind_T  *message_ptr,
  const gas_id_t       gas_id
)
{
  ie_descriptor_T       *current_IE;
  rr_sys_info_si_data_t *rr_sys_info_si_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_si_data_ptr = rr_sys_info_si_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_si_data_ptr);

  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_13, gas_id);

  if (current_IE)
  {
    // Mandatory IE - SI13 Rest Octets
    if (current_IE->start_IE)
    {
      csn_decoder_error_t error_code;
      si13_data_t  *si13_data_ptr = &rr_sys_info_si_data_ptr->decoded_si13_data;

      /* Use the CSN Decoder to decode the contents of the message */
      error_code = decode_si13_ro((void *)current_IE->start_IE, si13_data_ptr, gas_id);

      if ( error_code == CSN_DECODE_NO_ERROR )
      {
        /* If this SI13 is from the scell, then process this in the same way as if PSI13 had */
        /* been received during transfer, as the messages are identical */
        /* Note: There is no need to indicate to the state machine that SI13 has been received when */
        /* read from BCCH. This is done, however, on reception of PSI13 */
        if ( rr_is_this_the_scell( bcch_arfcn, gas_id ) )
        {
          gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

          MSG_GERAN_HIGH_1_G("SI13 from scell (%d)",
                   (int)bcch_arfcn.num);

          process_si13_or_psi13(si13_data_ptr, gas_id);

          /* recreate the measurement params based on recently received information */
          rr_gprs_update_meas_params_from_bcch(
            bcch_arfcn,   // channel_number
            TRUE,         // notify_nc_meas_control
            gas_id        // gas_id
          );

          if (scell_info_ptr->gsm.cell_identity_valid_flag)
          {
            (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_13,
                                      bcch_arfcn,
                                      message_ptr,
                                      &scell_info_ptr->gsm.location_area_identification,
                                      &scell_info_ptr->gsm.cell_identity,
                                      gas_id);
          }
          else
          {
            (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_13, 
                                      bcch_arfcn,
                                      message_ptr,
                                      NULL,
                                      NULL,
                                      gas_id);
          }
        }
        else
        /* If this SI13 is from the pcell, then store away the contents. No other action is */
        /* required */
        if ( rr_is_this_the_pcell( bcch_arfcn, gas_id ) )
        {
          gprs_scell_info_t  * pcell_info = rr_gprs_get_pcell_info(gas_id);

          MSG_GERAN_HIGH_1_G("SI13 from pcell (%d)",
                   (int)bcch_arfcn.num);

          rr_gprs_store_si13_or_psi13_data(
            si13_data_ptr,    // si13_data
            pcell_info,       // cell_info
            NULL,             // store_update_data
            gas_id            // gas_id
          );

          /* recreate the measurement params based on recently received information */
          rr_gprs_update_meas_params_from_bcch(
            bcch_arfcn,   // channel_number
            TRUE,         // notify_nc_meas_control
            gas_id        // gas_id
          );

          if (pcell_info->gsm.cell_identity_valid_flag)
          {
            (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_13,
                                      bcch_arfcn,
                                      message_ptr,
                                      &pcell_info->gsm.location_area_identification,
                                      &pcell_info->gsm.cell_identity,
                                      gas_id);
          }
          else
          {
            (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_13, 
                                      bcch_arfcn,
                                      message_ptr,
                                      NULL,
                                      NULL,
                                      gas_id);
          }
        }
        else
        /* Check if this SI13 is from one of our neighbour cells - but this is unexpected, because */
        /* SI13 is never requested for a neighbour cell */
        if (rr_gprs_is_this_a_surround_ncell(bcch_arfcn, gas_id))
        {
          MSG_GERAN_HIGH_1_G("SI13 from ncell (%d) - ignored",(int)bcch_arfcn.num);
        }
        else
        {
          MSG_GERAN_ERROR_1_G("SI13 from unknown cell (%d)",(int)bcch_arfcn.num);
        }
      }
      else
      {
        MSG_GERAN_ERROR_1_G("SI13ro decode error %d; contents ignored",error_code);
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("SI13 Rest Octets not present");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("message_list parsing error");
  }

} /* end rr_process_si13_from_bcch */

/*!
 * \brief Resets the BCCH sys info tracking data.
 *
 * \param gas_id (in)
 */
void bsi_reset_message_tracking_data(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t * gprs_info = rr_gprs_get_scell_info(gas_id);

  gprs_info->bcch_sys_info.si_messages_required = MASK_SI_NONE;
  gprs_info->bcch_sys_info.si_messages_low_priority = MASK_SI_NONE;
  gprs_info->bcch_sys_info.si_messages_on_bcch_ext = MASK_SI_NONE;
  gprs_info->bcch_sys_info.si_messages_not_broadcast = MASK_SI_NONE;
  gprs_info->bcch_sys_info.last_si_message_received = SI_TYPE_NONE;
  gprs_info->bcch_sys_info.single_l1_si_request_only = FALSE;
  gprs_info->gsm.update_message_received = MASK_SI_NONE;
  gprs_info->gsm.last_tc = 0;
  gprs_info->gsm.two_quater_position = SI2Q_POSITION_UNKNOWN;
  gprs_info->gsm.two_ti = RR_SI2TER_UNKNOWN;
  gprs_info->gsm.decode_SI2bis = RR_SI_REQUIRE_UNKNOWN;
  gprs_info->gsm.decode_SI2ter = RR_SI_REQUIRE_UNKNOWN;

} /* end bsi_reset_message_tracking_data() */

/*!
 * \brief Initialisation function.
 *
 * \param gas_id
 */
void rr_init_sys_info_si(const gas_id_t gas_id)
{
  ;

} /* end rr_init_sys_info_si() */

/*!
 * \brief Reverts the neighbour cell list to reflect the data last received from System Information.
 *
 * \param cell_info (in)
 * \param gas_id (in)
 */
void rr_gprs_build_ba_gprs_from_si2(
  gprs_scell_info_t *cell_info,
  const gas_id_t gas_id
)
{
  rr_params_mods_t ** params_mods = rr_params_mods(gas_id);
  rr_params_mods_t ** pcco_params_mods = rr_pcco_params_mods(gas_id);

  BCCH_frequency_list_T         *ba_list = &cell_info->gsm.complete_BA_list;
  gsm_ncell_list_member_t       *ncell;
  gsm_ncell_list_member_t       temp_bcch_ncell_store[MAX_NUMBER_OF_BEST_CELLS];
  uint8                         no_of_entries = 0;
  uint8                         i;
  boolean                       tmp_bsic_desc_valid = FALSE;
  gsm_bsic_description_entry_T  tmp_bsic_desc = {0};

  best_six_ncells_t            *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  /* initialisation */
  memset(temp_bcch_ncell_store, 0, sizeof(temp_bcch_ncell_store));

  /* Save bcch sys info for the best 6 over reselections */
  for ( i = 0; i < best_six_ncells_ptr->no_of_entries; i++ )
  {
    if ( best_six_ncells_ptr->ncell_entry[i].ncell_data->bcch_or_pmo == BCCH_FORMAT &&
         best_six_ncells_ptr->ncell_entry[i].ncell_data->ncell_data.bcch.gsm.valid_data_flag )
    {
      /* Carry over this entry */
      temp_bcch_ncell_store[no_of_entries++] = *best_six_ncells_ptr->ncell_entry[i].ncell_data;
    }
  }

  /* Assume no best six until restoration after the new BA list is created */
  best_six_ncells_ptr->no_of_entries = 0;

  rr_gprs_init_gsm_neighbour_cell_list(gas_id);

  for ( i = 0; i < ba_list->no_of_entries; i++ )
  {
    rr_gprs_add_ncell_to_ba_gprs(
      ba_list->BCCH_ARFCN[i],         /* ARFCN present */
      NULL,                           /* BSIC from SI2quater ??? */
      &cell_info->gprs_bsic_description.bsics[i],
      gas_id
    );
  }

  /*
  Restore the bcch best 6 if the cell exists in the new BA list. This is done by checking
  that the saved cell still exists in the new BA list. The adding it the entry to the best
  six list again. And incrementing the no of entries.
  */
  for ( i = 0; i < no_of_entries; i++ )
  {
    ncell = rr_gprs_get_ncell_info(
      temp_bcch_ncell_store[i].arfcn,
      NULL,
      gas_id
    );

    if ( ncell != NULL )
    {
      /* Save the GPRS BSIC Description if necessary */
      if (ncell->bcch_or_pmo == BCCH_FORMAT)
      {
        tmp_bsic_desc_valid = TRUE;
        tmp_bsic_desc = ncell->ncell_data.bcch.bsic_description;
      }
      else
      {
        tmp_bsic_desc_valid = FALSE;
      }

      *ncell = temp_bcch_ncell_store[i];
      best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries++].ncell_data = ncell;

      /* Restore the GPRS BSIC Description if necessary */
      if (ncell->bcch_or_pmo == BCCH_FORMAT && tmp_bsic_desc_valid)
      {
        ncell->ncell_data.bcch.bsic_description = tmp_bsic_desc;
      }
    }
  }

  /* Apply any PCCO amendments, but only once SI2 and all variants have been */
  /* received */
  if ( cell_info->gsm.complete_idle_BA_list_received )
  {
    rr_gprs_pcco_apply_ba_gprs_deltas(gas_id);
  }

  /****************************************************************************************/
  /* merge in to the BA list the CCN_SUPPORTED settings, this may have been received in a */
  /* PCCO or PMO so this must take priority over broadcase settings                       */
  /****************************************************************************************/
  if (params_mods != NULL && *params_mods != NULL &&
      (*params_mods)->nacc_params.valid_flag && (*params_mods)->nacc_params.ccn_support_flag )
  {
    rr_gprs_merge_ccn_support_into_ba_list(
      &(*params_mods)->nacc_params.ccn_supported_desc,
      gas_id
    );
  }
  else if (pcco_params_mods != NULL && *pcco_params_mods != NULL &&
           (*pcco_params_mods)->nacc_params.valid_flag && (*pcco_params_mods)->nacc_params.ccn_support_flag)
  {
    rr_gprs_merge_ccn_support_into_ba_list(
      &(*pcco_params_mods)->nacc_params.ccn_supported_desc,
      gas_id
    );
  }
  else
  {
    instances_T *si2q_inst_ptr = rr_get_instances_ptr(SI2Q_INSTANCE, gas_id);

    if (si2q_inst_ptr == NULL)
    {
      return;
    }

    for (i = 0; i < MAX_INSTANCES; i++)
    {
      SI2q_MI_instance_T *si2quater_instance = &(si2q_inst_ptr->instance[i]);

      if (!si2quater_instance->valid_data)
      {
        continue;
      }

      if ( si2quater_instance->si2q_mi.si2q_params.ccn_support_flag )
      {
        rr_gprs_merge_ccn_support_into_ba_list(
          &si2quater_instance->si2q_mi.si2q_params.ccn_support_desc,
          gas_id
        );
      }
    }

  }
} /* rr_gprs_build_ba_gprs_from_si2() */

/*!
 * \brief Called when the GSM BA list has been changed in SI2, SI2bis or SI2ter.
 *
 * \param update_l1 (in)
 * \param gas_id (in)
 */
void rr_gprs_gsm_ba_list_updated(
  boolean update_l1,
  const gas_id_t gas_id
)
{
  /* Rebuild BA(GPRS) list based on updated GSM BA list */
  rr_gprs_build_ba_gprs_from_si2(rr_gprs_get_scell_info(gas_id), gas_id);

  /* Update L1 with new parameters (if required) */
  if ( update_l1 )
  {
    rr_gprs_update_l1_params(gas_id);
  }

} /* end rr_gprs_gsm_ba_list_updated() */

/*!
 * \brief Called when SI3 is received to check if the cell capability (i.e. gprs_indicator) has changed.
 *
 * \param gprs_indicator (in) - TRUE if the GPRS INDICATOR IE is present in SI3; FALSE otherwise.
 * \param gsm_sys_info (in) - GSM system information database.
 */
void rr_gprs_check_for_cell_capability_change(
  boolean                        gprs_indicator,
  system_information_database_T *gsm_sys_info
)
{
  /* Check for activation or deactivation of GPRS - indicated by a change in the */
  /* presence of the GPRS Indicator IE in SI3. No action is required here, the sys info */
  /* handler will handle the addition or removal of SI13 */
  if ( gsm_sys_info->gprs_indicator != gprs_indicator )
  {
    MSG_GERAN_HIGH_1("Advertised GPRS capability changed (now %d)",gprs_indicator);
  }

} /* end rr_gprs_check_for_cell_capability_change() */

/*!
 * \brief This function processes PSI13 (received on the PACCH).
 *
 * \param decoded_psi13 (in) - ptr to PSI13 message content.
 * \param scell_info (in) - ptr to serving cell database.
 * \param gas_id
 */
void rr_gprs_process_psi13_ccch_camped(
  si13_data_t *decoded_psi13,
  gprs_scell_info_t *scell_info,
  const gas_id_t gas_id
)
{
  grr_siev_t siev;

#ifdef DEBUG_GSM_GPRS_GRR_SGSNR_AGILENT8960_WORKAROUND
  /* The Agilent 8960 includes the SGSNR, set to 1, in PSI13, but omits it in SI13 */
  /* This leads to RR_SERVICE_INDs being sent to MM each time PSI13 follows SI13 and */
  /* vica versa. As a work-around, the SGSNR bit is set to the value previously received */
  /* in SI13 */
  decoded_psi13->sgsnr = scell_info->bcch_sys_info.sgsnr;
#endif

#ifdef FEATURE_GSM_DTM
  /*
  If PSI13 is received during DTM, only process the change-mark information, and ignore
  the rest of the message

  44.060 5.5.2.1.3  System information on PACCH (and other logical channels)

  When a PSI or SI message is received on PACCH during dual transfer mode, no parameters
  except those relevant for monitoring possible changes in the contents of SI or PSI
  messages (e.g. PBCCH_CHANGE_MARK, BCCH_CHANGE_MARK, PSI_CHANGED_IND) shall be applied
  by the MS for operation in dual transfer mode
  */
  #ifdef FEATURE_GSM_EDTM
  if ( rr_in_dedicated(gas_id) && (!rr_gprs_edtm_enhanced_release_started(gas_id)) )
  #else
  if ( rr_in_dedicated(gas_id) )
  #endif /* FEATURE_GSM_EDTM */
  {
    /* Check for changes in BCCH_CHANGE_MARK, and stores the new value */
    /* Note: A comparison will only be made if the SI13 data is valid */
    process_si13_scheduling_information(
      decoded_psi13,
      scell_info,
      gas_id
    );

    /*
    Note: MASK_SI_13 is not cleared in si_messages_required, as would normally be the
    case if PSI13 were received during Packet Transfer, as the message contents have
    not been stored - so it will need to be re-acquired when back in Packet Idle
    */
    MSG_GERAN_HIGH_0_G("PSI13/14 message contents ignored during DTM");
  }
  else
#endif /* FEATURE_GSM_DTM */
  {
    process_si13_or_psi13(decoded_psi13, gas_id);

    /* As PSI13 can be considered a substitute for SI13, remove SI13 from the */
    /* bitmask indicating SIs required */
    scell_info->bcch_sys_info.si_messages_required &= (~ MASK_SI_13);

    /* Indicate to the state machine that PSI13 has been received. */
    /* This resets the 30s refresh timer */
    /* Note: This must be done after the message has been processed. */
    siev = bsi_acquisition_control(SIEV_PSI13_RECEIVED, NULL, gas_id);

    if ( siev != SIEV_NO_EVENT )
    {
      rr_send_priority_imsg_rr_event_ind(grr_siev_to_rrev(siev), gas_id);
    }
  }

} /* rr_gprs_process_psi13_ccch_camped() */

/*!
 * \brief Stored SI13/PSI13 data.
 *
 * \param si13_data (in)
 * \param cell_info (in)
 * \param store_update_data (out)
 * \param gas_id (in)
 */
void rr_gprs_store_si13_or_psi13_data(
  si13_data_t *si13_data,
  gprs_scell_info_t *cell_info,
  rrps_store_update_data_t *store_update_data,
  const gas_id_t gas_id
)
{
  rrps_store_update_bitmap_t store_update_bitmap = 0;
  rr_sys_info_si_data_t *rr_sys_info_si_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_si_data_ptr = rr_sys_info_si_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_si_data_ptr);

  cell_info->bcch_sys_info.bcch_change_mark = si13_data->bcch_change_mark;

  /* The mobile allocation cannot be fully decoded in this thread, because it may require */
  /* the cell allocation, which is given in SI1. Therefore, if the mobile allocation is */
  /* present, some partial decoding will have been done in decode_si13_ro(), but this */
  /* cannot be completed until SI1 has been received. */
  if ( si13_data->mobile_allocation_status == IE_PRESENT )
  {
    MSG_GERAN_MED_0_G("SI13 mobile alloc present");

    if ( cell_info->gsm.valid_CA_received == RR_CA_RECEIVED)
    {
      MSG_GERAN_MED_0_G("CA valid; MA_NUMBER=14 stored");

      rr_gprs_decode_mobile_alloc(
        NULL,
        &rr_sys_info_si_data_ptr->decoded_si13_data.mobile_allocation_ie,
        &cell_info->gsm.cell_channel_description,
        &rr_sys_info_si_data_ptr->decoded_si13_data.mobile_allocation
      );

      rr_gprs_store_mobile_alloc(
        MA_NUMBER_14,
        &rr_sys_info_si_data_ptr->decoded_si13_data.mobile_allocation,
        cell_info,
        gas_id
      );

      si13_data->mobile_allocation_status = IE_DECODED;
    }
    else
    {
      MSG_GERAN_MED_0_G("Require CA (SI1) before MA decode");
    }

    cell_info->mobile_allocations.si13_change_mark = si13_data->si13_change_mark;

    if ( store_update_data )
    {
      store_update_bitmap |= RRPS_UPD_CHANGE_MARK_SI13;
      store_update_data->change_mark_si13 = si13_data->si13_change_mark;
    }
  }

  cell_info->bcch_sys_info.routing_area_code =
    si13_data->bcch_pbcch.bcch.rac;

  cell_info->bcch_sys_info.spgc_ccch_sup =
    si13_data->bcch_pbcch.bcch.spgc_ccch_sup;

  cell_info->bcch_sys_info.priority_access_thr =
    si13_data->bcch_pbcch.bcch.priority_access_thr;

  cell_info->bcch_sys_info.nc_meas_params_si13.valid = TRUE;
  cell_info->bcch_sys_info.nc_meas_params_si13.network_control_order =
    si13_data->bcch_pbcch.bcch.network_control_order;

  cell_info->bcch_sys_info.gprs_cell_options =
    si13_data->bcch_pbcch.bcch.gprs_cell_options;

  cell_info->bcch_sys_info.power_control_params =
    si13_data->bcch_pbcch.bcch.power_control_params;

  cell_info->bcch_sys_info.non_drx_timer =
    rr_gprs_calc_non_drx_timer(
      si13_data->bcch_pbcch.bcch.gprs_cell_options.drx_timer_max,
      gas_id
    );

  if ( store_update_data )
  {
    store_update_bitmap |= RRPS_UPD_CELL_OPTIONS;
    store_update_data->gprs_cell_options = si13_data->bcch_pbcch.bcch.gprs_cell_options;
  }

  /* Store the SGSNR and SI_STATUS_IND in BCCH sys info */
  cell_info->bcch_sys_info.sgsnr = si13_data->sgsnr;
  cell_info->bcch_sys_info.si_status_ind = si13_data->si_status_ind;

  /* Store LB_MS_TXPWR_MAX_CCH */
  cell_info->bcch_sys_info.LB_MS_TXPWR_MAX_CCH_valid = si13_data->LB_MS_TXPWR_MAX_CCH_valid;
  cell_info->bcch_sys_info.LB_MS_TXPWR_MAX_CCH = si13_data->LB_MS_TXPWR_MAX_CCH;

  /* Update MAC/RLC data if required */
  if ( store_update_data && store_update_bitmap )
  {
    rr_write_public_store(
      store_update_bitmap,
      store_update_data,
      gas_id
    );
  }

  /* Mark the SI13 data as valid */
  cell_info->bcch_sys_info.si13_data_valid_flag = TRUE;

  /* Mark SI13 as received */
  SET_BIT( MASK_SI_13, cell_info->gsm.update_message_received );

} /* end rr_gprs_store_si13_or_psi13_data() */

/*!
 * \brief This function is called after any sys info message has been received and processed.
 *
 * \param si_message_received
 * \param bcch_arfcn
 * \param gas_id
 */
void rr_gprs_bcch_sys_info_post_processing(
  rr_si_type_t   si_message_received,
  ARFCN_T        bcch_arfcn,
  const gas_id_t gas_id
)
{
  if (rr_is_this_the_pcell(bcch_arfcn, gas_id))
  {
    bsi_non_camped_post_processing(gas_id);
  }
  else
  if ( rr_is_this_the_scell(bcch_arfcn, gas_id))
  {
    bsi_camped_post_processing(si_message_received, gas_id);
  }

} /* end rr_gprs_bcch_sys_info_post_processing() */

/*!
 * \brief External entry point to SI acquisition state machine.
 *
 * \param rr_event_in (in)
 * \param event_in_data (in)
 * \param gas_id (in)
 *
 * \return rr_event_T
 */
rr_event_T rr_gprs_bsi_acquisition_control(
  rr_event_T rr_event_in,
  void *event_in_data,
  const gas_id_t gas_id
)
{
  grr_siev_t  event_out;
  grr_siev_t  event_in = grr_rrev_to_siev(rr_event_in);

  event_out = bsi_acquisition_control(event_in, event_in_data, gas_id);

  return grr_siev_to_rrev(event_out);

} /* end rr_gprs_bsi_acquisition_control() */

/*!
 * \brief BCCH SI acquisition timer handler.
 *
 * \param timer_id (in)
 * \param gas_id (in)
 *
 * \return rr_event_T
 */
rr_event_T rr_gprs_bsi_acquisition_timer_handler(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  grr_siev_t  event_in;
  grr_siev_t  event_out = SIEV_NO_EVENT;

  event_in = grr_timerid_to_siev( timer_id );

  if ( event_in != SIEV_NO_EVENT )
  {
    event_out = bsi_acquisition_control(event_in, NULL, gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_1_G("No SIEV for timer %d",timer_id);
  }

  return grr_siev_to_rrev( event_out );

} /* end rr_gprs_bsi_acquisition_timer_handler() */

/*!
 * \brief Determines whether paging reorg is compatible with the current mode.
 *
 * \return boolean - TRUE if paging reorganisation should be allowed, FALSE otherwise
 */
static boolean bsi_paging_reorg_allowed(const gas_id_t gas_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (rr_ms_any_other_active_gas_id(gas_id))
  {
    /* Multi SIM mode is enabled and another GSM stack is active, so */
    /* paging reorganisation mode in the Sys Info handler is blocked */
    MSG_GERAN_HIGH_0_G("BCCH sys info reorganisation not allowed");
    return(FALSE);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(TRUE);
} /* end bsi_paging_reorg_allowed() */

/*!
 * \brief BCCH SI must be read continuously during Paging Reorganisation.
 *
 * \param gas_id
 */
void bsi_enter_paging_reorg(
  const gas_id_t gas_id
)
{
  /* change state if allowed to do so */
  if (bsi_paging_reorg_allowed(gas_id))
  {
    rr_gprs_stop_sys_info_refresh_timer(gas_id);

    /* Indicate Paging Reorg to the state machine, which will stop doing controlled acquisitions */
    /* and instruct L1 to read all SI */
    (void) bsi_acquisition_control(SIEV_ENTER_PAGING_REORG, NULL, gas_id);
  }

  return;
} /* bsi_enter_paging_reorg() */

/*!
 * \brief BCCH SI must be read continuously during Paging Reorganisation.
 *
 * \param gas_id
 */
void bsi_exit_paging_reorg(
  const gas_id_t gas_id
)
{
  (void) bsi_acquisition_control(SIEV_EXIT_PAGING_REORG, NULL, gas_id);

} /* bsi_exit_paging_reorg() */

/*!
 * \brief Decodes and stores a mobile allocation in SI13.
 *
 * \param psi13_data (in)      - PSI13 message. If this is omitted (NULL), the last SI13 data is substituted.
 * \param cell_allocation (in) - Cell allocation used to decode the mobile allocation
 * \param cell_info (in)
 * \param gas_id (in)
 *
 * \return boolean - TRUE if SI13/PSI13 contained a valid mobile allocation; FALSE otherwise.
 */
boolean rr_gprs_process_si13_mobile_alloc(
  si13_data_t *psi13_data,
  cell_channel_description_T *cell_allocation,
  gprs_scell_info_t *cell_info,
  const gas_id_t gas_id
)
{
  rr_sys_info_si_data_t *rr_sys_info_si_data_ptr;
  si13_data_t *si13_data;
  boolean updated = FALSE;

  // Obtain a pointer to the module data
  rr_sys_info_si_data_ptr = rr_sys_info_si_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_si_data_ptr);

  // Assume that PSI13 data is supplied
  si13_data = psi13_data;

  // If not supplied, substitute the existing SI13 data
  if (si13_data == NULL)
  {
    si13_data = &rr_sys_info_si_data_ptr->decoded_si13_data;
  }

  if (si13_data->mobile_allocation_status == IE_PRESENT)
  {
    RR_NULL_CHECK_RETURN_PARAM(cell_allocation, FALSE);

    rr_gprs_decode_mobile_alloc(
      NULL,
      &si13_data->mobile_allocation_ie,
      cell_allocation,
      &si13_data->mobile_allocation
    );

    si13_data->mobile_allocation_status = IE_DECODED;

    rr_gprs_store_mobile_alloc(
      MA_NUMBER_14,
      &si13_data->mobile_allocation,
      cell_info,
      gas_id
    );

    updated = TRUE;
  }

  return updated;

} /* end rr_gprs_process_si13_mobile_alloc() */

/*!
 * \brief Called after camping-on a cell, starts the 30s refresh period.
 *
 *  Called after camping-on a cell, starts the 30s refresh period. Also determines if there are missing SIs
 *  which have not yet been acquired (but which were not necessary for camping), and instructs L1 to acquire
 *  them.
 *
 * \param gas_id
 */
void bsi_activate_camped_sys_info_handling(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info = rr_gprs_get_scell_info(gas_id);
  boolean            require_3G = rr_ms_3g_sys_info_mandatory(gas_id);

  /*
  Setup which BSIs are missing. There are two reasons why some BSIs are still
  outstanding:
  a) GRR has just camped on a new cell, and some SIs are not initially required
   for camping (e.g. SI2quater), so need to be acquired now. These will be
   indicated in the mask returned from bsi_determine_message_missing_after_camping()
   (In this case, si_messages_expected should be all-zero).
  b) A previous BSI acquisition was interrupted due to suspension, and sys info
   handling has now been re-activated. si_messages_expected will already contain the
   mask
  */

  bsi_set_messages_required_for_acquisition(
    bsi_determine_messages_missing_after_camping(scell_info, require_3G, gas_id),// required
    MASK_SI_NONE,                                                                // low_priority
    FALSE,                                                                       // reset_history
    gas_id                                                                       // gas_id
  );

  /* now apply changes for optional 3G sys info */
  if (FALSE == require_3G)
  {
    /* the access stratum is in multi-SIM mode, but is not paired with */
    /* any WCDMA subscription, so GRR may avoid requesting 3G sys info */
    scell_info->bcch_sys_info.si_messages_required &= ~(MASK_SI_2_QUATER | MASK_SI_2_TER_REST);
    MSG_GERAN_HIGH_0_G("3G system info not mandatory after camping");
  }

  (void) bsi_acquisition_control(SIEV_ACTIVATE_SYS_INFO_ACQUISITION, NULL, gas_id);

  return;
} /* bsi_activate_camped_sys_info_handling() */

/*!
 * \brief Called when there is a change detected in which SI messages are being broadcast.
 *
 *  Called when there is a change detected in which SI messages are being broadcast, or when a SI message is
 *  noted as being on BCCH-Extended.
 *
 * \param cell_info_ptr (in)
 * \param added_bcch_norm (in)
 * \param added_bcch_ext (in)
 * \param removed (in)
 */
void bsi_update_messages_being_broadcast(
  gprs_scell_info_t *cell_info_ptr,
  uint16             added_bcch_norm,
  uint16             added_bcch_ext,
  uint16             removed
)
{
  if ( added_bcch_norm & MASK_SI_2_TER )
  {
    added_bcch_norm |= MASK_SI_2_TER_REST;
  }

  if ( added_bcch_ext & MASK_SI_2_TER )
  {
    added_bcch_ext |= MASK_SI_2_TER_REST;
  }

  if ( removed & MASK_SI_2_TER )
  {
    removed |= MASK_SI_2_TER_REST;
  }

  cell_info_ptr->bcch_sys_info.si_messages_not_broadcast |= removed;
  cell_info_ptr->bcch_sys_info.si_messages_required &= (~ removed);
  cell_info_ptr->bcch_sys_info.si_messages_low_priority &= (~ removed);
  cell_info_ptr->bcch_sys_info.si_messages_on_bcch_ext &= (~ removed);

  /* Also mark the SI as being received */
  cell_info_ptr->gsm.update_message_received |= removed;

  cell_info_ptr->
    bcch_sys_info.si_messages_not_broadcast &= (~ (added_bcch_norm | added_bcch_ext));

  cell_info_ptr->bcch_sys_info.si_messages_on_bcch_ext |= added_bcch_ext;

} /* end bsi_update_messages_being_broadcast() */

/*!
 * \brief Set-up SI message masks in preparation for an acquisition.
 *
 * \param required (in)
 * \param low_priority (in)
 * \param reset_history (in)
 * \param gas_id (in)
 */
void bsi_set_messages_required_for_acquisition(
  uint16 required,
  uint16 low_priority,
  boolean reset_history,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t * scell_info;
  system_information_database_T *system_information_database_ptr;
  uint16 si_messages_required;

  if (rr_is_camped(gas_id))
  {
    scell_info = rr_gprs_get_scell_info(gas_id);
  }
  else
  {
    scell_info = rr_gprs_get_pcell_info(gas_id);
  }

  system_information_database_ptr = &scell_info->gsm;

  if ( required & MASK_SI_1 )
  {
    system_information_database_ptr->last_tc = 0;
    system_information_database_ptr->valid_CA_received = RR_CA_NOT_RECEIVED;
  }

  /* Ensure the 'low priority' SIs are also part of the 'required' set, or they */
  /* won't be cleared-off when received */
  required |= low_priority;

  if ( reset_history )
  {
    system_information_database_ptr->update_message_received = 0;
    scell_info->bcch_sys_info.single_l1_si_request_only = FALSE;
    scell_info->bcch_sys_info.si_messages_required = required;
    scell_info->bcch_sys_info.si_messages_low_priority = low_priority;
  }
  else
  {
    system_information_database_ptr->update_message_received &= (~ required);
    scell_info->bcch_sys_info.si_messages_required |= required;
    scell_info->bcch_sys_info.si_messages_low_priority |= low_priority;
  }

  si_messages_required = scell_info->bcch_sys_info.si_messages_required;

  /* Now filter out those which aren't being broadcast */

  scell_info->bcch_sys_info.si_messages_required &=
    (~ scell_info->bcch_sys_info.si_messages_not_broadcast );

  scell_info->bcch_sys_info.si_messages_low_priority &=
    (~ scell_info->bcch_sys_info.si_messages_not_broadcast );

  MSG_GERAN_HIGH_2_G("si_messages_required 0x%x (0x%x)",
           scell_info->bcch_sys_info.si_messages_required, si_messages_required);

} /* end bsi_set_messages_required_for_acquisition() */

/*!
 * \brief SI acquisition state machine.
 *
 * \param event_in
 * \param event_in_data
 * \param gas_id
 *
 * \return grr_siev_t
 */
grr_siev_t bsi_acquisition_control(
  grr_siev_t event_in,
  const void *event_in_data,
  const gas_id_t gas_id
)
{
  grr_siev_t  event_out = SIEV_NO_EVENT;
  grr_siev_t  new_event = SIEV_NO_EVENT;
  rr_sys_info_si_data_t *rr_sys_info_si_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_si_data_ptr = rr_sys_info_si_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_si_data_ptr);

  /*****************************************************************/
  /* Perform state-independent common processing on incoming event */
  /*****************************************************************/

  /*****************************************************************/
  /* SIEV_INPUT_MESSAGE                                            */
  /*****************************************************************/
  if ( event_in == SIEV_INPUT_MESSAGE )
  {
    /* Translate input message into an event to be processed below */
    event_in = bsi_common_signal_handler(event_in_data, gas_id);
  }

  while ( event_in != SIEV_NO_EVENT )
  {
    /*****************************************************************/
    /* Now perform processing according to state                     */
    /*****************************************************************/

    switch ( rr_sys_info_si_data_ptr->state )
    {
      case BSIST_ACQ_NOT_REQUESTED:
      {
        /*****************************************************************/
        /* SIEV_SYS_INFO_REFRESH_TIMER_EXPIRY                            */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_REFRESH_TIMER_EXPIRY )
        {
          MSG_GERAN_MED_0_G("Acquiring SI (for periodic refresh)");

          /* Restart the refresh timer */
          rr_gprs_start_sys_info_refresh_timer(gas_id);

          new_event = SIEV_SYS_INFO_PART_REFRESH;
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_PART_REFRESH                                    */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_PART_REFRESH )
        {
          gprs_scell_info_t *cell_info;
          rr_ca_status_t    rr_ca_status;

          cell_info = rr_gprs_get_scell_info(gas_id);
          rr_ca_status = cell_info->gsm.valid_CA_received;
          /*
          Refresh system information - if SI13 is being broadcast, this will be read, and
          the BCCH_CHANGE_MARK checked to see if any other SIs have changed.
          If SI13 is not being broadcast, all the GSM SIs will be read
          */
          start_l1_refresh_acquisition(gas_id);

          /*if SI1 is required and we already received CA then modify valid_CA_received to
            RR_CA_RECEIVED_BUT_INVALID*/
          if (( cell_info->bcch_sys_info.si_messages_required & MASK_SI_1 ) &&
              ( rr_ca_status == RR_CA_RECEIVED ))
          {
             cell_info->gsm.valid_CA_received = RR_CA_RECEIVED_BUT_INVALID;
          }

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED;
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_FULL_REFRESH                                    */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_FULL_REFRESH )
        {
          /* Ask L1 to read all SIs */
          start_l1_full_refresh_acquisition(gas_id);

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED;
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_MESSAGE_RECEIVED                                */
        /* A sys info message has been received, perhaps due to L1 being */
        /* in MM non-DRX                                                 */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_MESSAGE_RECEIVED )
        {
          /* Check for PBCCH activation, or SI13 addition/removal */
          /* This will return SIEV_NO_EVENT, SIEV_PBCCH_ACTIVATED or */
          /* SIEV_SI13_ADDED_OR_REMOVED */
          new_event = grr_align_sys_info_acquisition_with_cell_capabilities(gas_id);

          /* SI13 status unchanged - no special action is required */
          if (new_event == SIEV_NO_EVENT)
          {
            gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);
            gprs_bcch_scell_info_t * bcch_sys_info = &cell_info->bcch_sys_info;

            /* Check if the SI just received was SI13 - if so, use this to restart */
            /* the 30sec refresh timer */
            if ( bcch_sys_info->last_si_message_received == SI_TYPE_SI_13 )
            {
              rr_gprs_start_sys_info_refresh_timer(gas_id);

              /* SI13 may have contained an updated BCCH_CHANGE_MARK, which means that */
              /* other SIs need to be acquired */
              if ( bcch_sys_info->si_messages_required )
              {
                rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED;
                new_event = SIEV_SYS_INFO_MESSAGE_RECEIVED;
              }
            }
          }
        }

        else
        /*****************************************************************/
        /* SIEV_PSI13_RECEIVED                                           */
        /*****************************************************************/
        if ( event_in == SIEV_PSI13_RECEIVED )
        {
          gprs_scell_info_t * scell_info = rr_gprs_get_scell_info(gas_id);

          MSG_GERAN_MED_0_G("PSI13 received from PACCH");

          if (scell_info != NULL)
          {
            if (scell_info->gsm.valid_data_flag)
            {
              /* Restart the refresh timer */
              rr_gprs_start_sys_info_refresh_timer(gas_id);

              /* PSI13 may have contained an updated BCCH_CHANGE_MARK, which means that */
              /* other SIs need to be acquired */
              if ( scell_info->bcch_sys_info.si_messages_required )
              {
                rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED;
                new_event = SIEV_SYS_INFO_MESSAGE_RECEIVED;
              }
            }
          }
        }

        else
        /*****************************************************************/
        /* SIEV_SI13_ADDED                                               */
        /*****************************************************************/
        if ( event_in == SIEV_SI13_ADDED )
        {
          MSG_GERAN_HIGH_0_G("Acquiring SI (SI13 added in cell)");

          /*
          Setup SI masks to perform full SI refresh
          Note: previous reception history is NOT cleared
          */
          setup_full_refresh_acquisition(gas_id);

          new_event = SIEV_SYS_INFO_MESSAGE_RECEIVED;
          rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED_SI13_ADDED;
        }

        else
        /*****************************************************************/
        /* SIEV_SI13_REMOVED                                             */
        /*****************************************************************/
        if ( event_in == SIEV_SI13_REMOVED )
        {
          MSG_GERAN_HIGH_0_G("Acquiring SI (SI13 removed from cell)");

          /*
          Setup SI masks to perform full SI refresh
          Note: previous reception history is NOT cleared
          */
          setup_full_refresh_acquisition(gas_id);

          event_out = SIEV_GPRS_SUSPEND_NO_SI13;
          rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED_SI13_REMOVED;
        }

        else
        /*****************************************************************/
        /* SIEV_ACTIVATE_SYS_INFO_ACQUISITION                            */
        /*****************************************************************/
        if ( event_in == SIEV_ACTIVATE_SYS_INFO_ACQUISITION )
        {
          new_event = bsi_determine_action_after_activation(gas_id);
        }

        else
        /*****************************************************************/
        /* SIEV_CONTINUE_SYS_INFO_ACQUISITION                            */
        /* This event is generated when BSI handling is activated and    */
        /* there is an incomplete set of SI                              */
        /*****************************************************************/
        if ( event_in == SIEV_CONTINUE_SYS_INFO_ACQUISITION )
        {
          /* Ask L1 to read the SIs from BCCH */
          continue_l1_bsi_acquisition(gas_id);

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED;
        }

        else
        /*****************************************************************/
        /* SIEV_ENTER_PAGING_REORG                                       */
        /*****************************************************************/
        if ( event_in == SIEV_ENTER_PAGING_REORG )
        {
          uint32 rr_l1_mask = (uint32)RR_L1_SI_TYPE_ALL_NORM |
                              (uint32)RR_L1_SI_TYPE_ALL_EXT;

          MSG_GERAN_HIGH_0_G("Acquiring SI (Paging Reorg)");

          /*
          Ask L1 to read all BCCH SI (see 04.18 3.3.2.1.1), but stay in this
          "not requested" state. This means there will be no feedback to L1 as messages
          are received, so it will keep reading everything
          */
          rr_send_mph_read_sys_info_req((rr_l1_si_type_T)rr_l1_mask, gas_id);

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_PAGING_REORG;
        }

        else
        /*****************************************************************/
        /* SIEV_SUSPEND_SYS_INFO_ACQUISITION                             */
        /* Some action (such as a CS call establishment) needs to        */
        /* interrupt timed sys info acquisition. No acquisition is in    */
        /* progress at the present time, but move to the suspended state.*/
        /* The refresh timer is left running - when sys info handling is */
        /* resumed, if the timer has expired, a refresh will be started  */
        /*****************************************************************/
        if ( event_in == SIEV_SUSPEND_SYS_INFO_ACQUISITION )
        {
          MSG_GERAN_HIGH_0_G("Sys info acquisition suspended (idle)");

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_SUSPENDED;
        }

        break;
      }

      case BSIST_ACQ_PAGING_REORG:
      {
        /*****************************************************************/
        /* SIEV_EXIT_PAGING_REORG                                        */
        /*****************************************************************/
        if ( event_in == SIEV_EXIT_PAGING_REORG )
        {
          MSG_GERAN_HIGH_0_G("Paging Reorg SI acquisition stopped");

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_NOT_REQUESTED;
          new_event = bsi_determine_action_after_activation(gas_id);

          if ( new_event == SIEV_NO_EVENT )
          {
            stop_l1_bsi_acquisition(gas_id);
          }
        }

        else
        /*****************************************************************/
        /* SIEV_SUSPEND_SYS_INFO_ACQUISITION                             */
        /* Some action (such as a CS call establishment) needs to        */
        /* interrupt. Move to the suspended state. The refresh timer is  */
        /* started - when sys info handling is resumed, if the timer has */
        /* expired, a refresh will be started                            */
        /*****************************************************************/
        if ( event_in == SIEV_SUSPEND_SYS_INFO_ACQUISITION )
        {
          MSG_GERAN_HIGH_0_G("Sys info acquisition suspended (paging reorg)");

          stop_l1_bsi_acquisition(gas_id);

          /* Restart the refresh timer - this will time the period of suspension */
          rr_gprs_start_sys_info_refresh_timer(gas_id);

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_SUSPENDED;
        }

        else
        /*****************************************************************/
        /* SIEV_PSI13_RECEIVED                                           */
        /*****************************************************************/
        if ( event_in == SIEV_PSI13_RECEIVED )
        {
          MSG_GERAN_MED_0_G("PSI13 received from PACCH");
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_MESSAGE_RECEIVED                                */
        /* A sys info message has been received                          */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_MESSAGE_RECEIVED )
        {
          ;
        }

        else
        /*****************************************************************/
        /* SIEV_STOP_AND_RESET_SOFTWARE                                  */
        /* Instruct L1 to stop reading BCCH SI                           */
        /*****************************************************************/
        if ( event_in == SIEV_STOP_AND_RESET_SOFTWARE )
        {
          stop_l1_bsi_acquisition(gas_id);
        }

        break;
      }

      case BSIST_ACQ_REQUESTED:
      {
        /*****************************************************************/
        /* SIEV_SYS_INFO_MESSAGE_RECEIVED                                */
        /* A sys info message (any) has been received                    */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_MESSAGE_RECEIVED )
        {
          /* Check for PBCCH activation, or SI13 addition/removal */
          /* This will return SIEV_NO_EVENT, SIEV_PBCCH_ACTIVATED or SIEV_SI13_ADDED_OR_REMOVED */
          new_event = grr_align_sys_info_acquisition_with_cell_capabilities(gas_id);

          if ( new_event == SIEV_NO_EVENT )
          {
            if ( check_if_bsi_acquisition_complete(gas_id) )
            {
              new_event = SIEV_SYS_INFO_ACQUISITION_COMPLETE;
            }
            else
            {
              continue_l1_bsi_acquisition(gas_id);
            }
          }
        }

        else
        /*****************************************************************/
        /* SIEV_PSI13_RECEIVED                                           */
        /* PSI13 has been receieved.  An SI acquisition is in progress.  */
        /*****************************************************************/
        if ( event_in == SIEV_PSI13_RECEIVED )
        {
          MSG_GERAN_MED_0_G("PSI13 received from PACCH");

          /* Restart the refresh timer */
          rr_gprs_start_sys_info_refresh_timer(gas_id);

          if ( check_if_bsi_acquisition_complete(gas_id) )
          {
            new_event = SIEV_SYS_INFO_ACQUISITION_COMPLETE;
          }
          else
          {
            continue_l1_bsi_acquisition(gas_id);
          }
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_ACQUISITION_COMPLETE                            */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_ACQUISITION_COMPLETE )
        {
          /*
          Clean-up, stop the acquisition timer, resume MAC, check L1 idle mode
          This call can return:
            SIEV_NO_EVENT - no change in L1 idle mode required
            SIEV_CHANGE_L1_IDLE_MODE - a new start idle should be sent to L1
            SIEV_ACTIVATE_SYS_INFO_ACQUISITION - continue to acquire SIs
          */
          grr_siev_t siev = bsi_acquisition_complete(gas_id);

          if ( (siev == SIEV_NO_EVENT) ||
               (siev == SIEV_ACTIVATE_SYS_INFO_ACQUISITION) )
          {
            new_event = siev;
          }
          else
          {
            event_out = siev;
          }

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_NOT_REQUESTED;
        }

        else
        /*****************************************************************/
        /* SIEV_SUSPEND_SYS_INFO_ACQUISITION                             */
        /*****************************************************************/
        if ( event_in == SIEV_SUSPEND_SYS_INFO_ACQUISITION )
        {
          MSG_GERAN_HIGH_0_G("Sys info acquisition suspended (in progress)");

          stop_l1_bsi_acquisition(gas_id);

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_SUSPENDED;
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_REFRESH_TIMER_EXPIRY                            */
        /* 30 secs have passed since the acquisition was started,        */
        /* but some SIs still remain unacquired. Reasons for this:       */
        /* a) The acquisition is failing because of decode failures      */
        /* b) SI13 no longer exists (e.g. GPRS has been deactivated)     */
        /* c) There are many instances of SI2quater                      */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_REFRESH_TIMER_EXPIRY )
        {
          gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);
          uint16 remaining_si = get_remaining_si_bitmask( cell_info );

          if ( remaining_si & MASK_SI_13 )
          {
            MSG_GERAN_HIGH_0_G("Giving up on SI13 - perform GSM SI refresh");

            /* Assume the cell no longer supports GPRS - as if SI3 had been received */
            /* without a GPRS Indicator IE */
            cell_info->gsm.gprs_indicator = FALSE;
            cell_info->bcch_sys_info.si13_data_valid_flag = FALSE;

            /*
            Reset all message tracking history, which SIs are being broadcast, positions
            of SI2ter, SI2quater, etc
            Setup SI masks to perform full SI refresh
            */
            bsi_reset_message_tracking_data(gas_id);
            setup_full_refresh_acquisition(gas_id);

            rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED_SI13_REMOVED;
            event_out = SIEV_GPRS_SUSPEND_NO_SI13;
          }
          else
          {
            new_event = SIEV_SYS_INFO_ACQUISITION_TIMER_EXPIRY;
          }

          /* If there are still instances of SI2quater or SI2ter(ro) outstanding, suspend GPRS */
          /* and then continue acquisition. GPRS will be resumed when this completes */
          if ( ( remaining_si & MASK_SI_2_QUATER ) || ( remaining_si & MASK_SI_2_TER_REST ) )
          {
            MSG_GERAN_HIGH_1_G("Still waiting for SI2quater/SI2ter(ro) (0x%x)",(int)remaining_si);

            if (rr_gsm_only(gas_id))
            {
              continue_l1_bsi_acquisition(gas_id);
            }
            else
            {
              event_out = SIEV_GPRS_SUSPEND;
            }

            /* As the 30s refresh timer has expired, add in SI13 mask */
            /* in addition to SI2quater / SI2ter */
            bsi_set_messages_required_for_acquisition(
              MASK_SI_13,     // required
              MASK_SI_NONE,   // low_priority
              FALSE,          // reset_history
              gas_id          // gas_id
            );

            new_event = SIEV_NO_EVENT;
          }

          /* If we are not about to reselect, restart the 30s refresh timer */
          if ( new_event != SIEV_SYS_INFO_ACQUISITION_TIMER_EXPIRY )
          {
            rr_gprs_start_sys_info_refresh_timer(gas_id);
          }
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_ACQUISITION_TIMER_EXPIRY                        */
        /* Acquisition has failed. The MS must now reselect.             */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_ACQUISITION_TIMER_EXPIRY )
        {
          /* Stop reading BCCH SI, stop refresh timer */
          bsi_acquisition_failed(gas_id);

          /* Indicate acquisition failure - will cause a reselection */
          event_out = SIEV_SYS_INFO_ACQUISITION_FAILURE;

          /* Reset the state machine */
          rr_sys_info_si_data_ptr->state = BSIST_ACQ_NOT_REQUESTED;
        }

        else
        /*****************************************************************/
        /* SIEV_READ_SYS_INFO_FAILURE_L1_IN_TRANSFER                     */
        /*****************************************************************/
        if ( event_in == SIEV_READ_SYS_INFO_FAILURE_L1_IN_TRANSFER )
        {
          gprs_scell_info_t  * scell_info = rr_gprs_get_scell_info(gas_id);

          scell_info->bcch_sys_info.single_l1_si_request_only = TRUE;

          continue_l1_bsi_acquisition(gas_id);
        }

        else
        /*****************************************************************/
        /* SIEV_READ_SYS_INFO_FAILURE_UNSPECIFIED                        */
        /*****************************************************************/
        if ( event_in == SIEV_READ_SYS_INFO_FAILURE_UNSPECIFIED )
        {
          rr_gprs_start_sys_info_retry_timer(GRR_TIMEOUT_1_SECOND, gas_id);
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_RETRY_TIMER_EXPIRY                              */
        /* A previously failed sys info read can now be retried          */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_RETRY_TIMER_EXPIRY )
        {
          continue_l1_bsi_acquisition(gas_id);
        }

        else
        /*****************************************************************/
        /* SIEV_STOP_AND_RESET_SOFTWARE                                  */
        /* Abort the acquisition in progress                             */
        /*****************************************************************/
        if ( event_in == SIEV_STOP_AND_RESET_SOFTWARE )
        {
          abort_l1_bsi_acquisition(gas_id);
        }

        else
        /*****************************************************************/
        /* SIEV_ENTER_PAGING_REORG                                       */
        /*****************************************************************/
        if ( event_in == SIEV_ENTER_PAGING_REORG )
        {
          MSG_GERAN_HIGH_0_G("Cancel controlled SI acquisition");

          rr_gprs_stop_sys_info_acquisition_timer(gas_id);

          new_event = SIEV_ENTER_PAGING_REORG;
          rr_sys_info_si_data_ptr->state = BSIST_ACQ_NOT_REQUESTED;
        }

        else
        /*****************************************************************/
        /* SIEV_SI13_ADDED                                               */
        /*****************************************************************/
        if ( event_in == SIEV_SI13_ADDED )
        {
          MSG_GERAN_HIGH_0_G("Acquiring SI (SI13 added in cell)");

          /*
          Setup SI masks to perform full SI refresh
          Note: previous reception history is NOT cleared
          */
          setup_full_refresh_acquisition(gas_id);

          new_event = SIEV_SYS_INFO_MESSAGE_RECEIVED;
          rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED_SI13_ADDED;
        }

        else
        /*****************************************************************/
        /* SIEV_SI13_REMOVED                                             */
        /*****************************************************************/
        if ( event_in == SIEV_SI13_REMOVED )
        {
          MSG_GERAN_HIGH_0_G("Acquiring SI (SI13 removed from cell)");

          /*
          Setup SI masks to perform full SI refresh
          Note: previous reception history is NOT cleared
          */
          setup_full_refresh_acquisition(gas_id);

          event_out = SIEV_GPRS_SUSPEND_NO_SI13;
          rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED_SI13_REMOVED;
        }

        else
        /*****************************************************************/
        /* SIEV_GPRS_SUSPENDED                                           */
        /* GPRS has been suspended. Continue with the SI acquisition     */
        /*****************************************************************/
        if ( event_in == SIEV_GPRS_SUSPENDED )
        {
          continue_l1_bsi_acquisition(gas_id);
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_FULL_REFRESH                                    */
        /* The occurrence of this event in this state implies that the   */
        /* full SI refresh is required due to some abnormal conditions,  */
        /* such as the reception of an ABNORMAL_RELEASE signal from GMAC */
        /* Note that this can also happen if the NACC code has forced a  */
        /* sys info refresh due to an incomplete BA list.                */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_FULL_REFRESH )
        {
          /* restart the sys info acquisition timer */
          rr_gprs_stop_sys_info_acquisition_timer(gas_id);
          rr_gprs_start_sys_info_acquisition_timer(GRR_TIMEOUT_BCCH_SI_ACQUISITION, gas_id);

          /*
          Reset all message tracking history, which SIs are being broadcast, positions
          of SI2ter, SI2quater, etc
          Setup SI masks to perform full SI refresh
          */
          bsi_reset_message_tracking_data(gas_id);
          setup_full_refresh_acquisition(gas_id);
        }

        break;
      }

      case BSIST_ACQ_REQUESTED_SI13_ADDED:
      {
        /*****************************************************************/
        /* SIEV_SYS_INFO_MESSAGE_RECEIVED                                */
        /* A sys info message (any) has been received                    */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_MESSAGE_RECEIVED )
        {
          gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

          if ( ! scell_info_ptr->gsm.gprs_indicator )
          {
            MSG_GERAN_HIGH_0_G("SI13 removed from cell");

            /*
            Setup SI masks to perform full SI refresh
            Note: previous reception history is NOT cleared
            */
            setup_full_refresh_acquisition(gas_id);

            rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED_SI13_REMOVED;
            new_event = SIEV_SYS_INFO_MESSAGE_RECEIVED;
          }
          else
          if (check_if_bsi_acquisition_complete(gas_id))
          {
            new_event = SIEV_SYS_INFO_ACQUISITION_COMPLETE;
          }
          else
          {
            continue_l1_bsi_acquisition(gas_id);
          }
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_ACQUISITION_COMPLETE                            */
        /* All the requested BCCH SIs have been received.                */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_ACQUISITION_COMPLETE )
        {
          /*
          Clean-up, stop the acquisition timer, resume MAC, check L1 idle mode
          This call can return:
            SIEV_NO_EVENT - no change in L1 idle mode required
            SIEV_CHANGE_L1_IDLE_MODE - a new start idle should be sent to L1
            SIEV_ACTIVATE_SYS_INFO_ACQUISITION - continue to acquire SIs
          */
          grr_siev_t siev = bsi_acquisition_complete(gas_id);

          if ( (siev == SIEV_NO_EVENT) ||
               (siev == SIEV_ACTIVATE_SYS_INFO_ACQUISITION) )
          {
            /* As there is no change to L1 idle mode, update MM with the new cell capabiltiy */
            grr_update_mm_with_rr_service_ind(gas_id);

            /* Restart the 30s refresh timer */
            rr_gprs_start_sys_info_refresh_timer(gas_id);

            new_event = siev;
          }
          else
          {
            event_out = siev;
          }

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_NOT_REQUESTED;
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_ACQUISITION_TIMER_EXPIRY                        */
        /* Acquisition has failed. The MS must now reselect.             */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_ACQUISITION_TIMER_EXPIRY )
        {
          /* Stop reading BCCH SI, stop refresh timer */
          bsi_acquisition_failed(gas_id);

          /* Indicate acquisition failure - will cause a reselection */
          event_out = SIEV_SYS_INFO_ACQUISITION_FAILURE;

          /* Reset the state machine */
          rr_sys_info_si_data_ptr->state = BSIST_ACQ_NOT_REQUESTED;
        }

        else
        /*****************************************************************/
        /* SIEV_SUSPEND_SYS_INFO_ACQUISITION                             */
        /*****************************************************************/
        if ( event_in == SIEV_SUSPEND_SYS_INFO_ACQUISITION )
        {
          MSG_GERAN_HIGH_0_G("Sys info acquisition suspended (SI13 added)");

          stop_l1_bsi_acquisition(gas_id);

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_SUSPENDED;
        }

        else
        /*****************************************************************/
        /* SIEV_READ_SYS_INFO_FAILURE_UNSPECIFIED                        */
        /*****************************************************************/
        if ( event_in == SIEV_READ_SYS_INFO_FAILURE_UNSPECIFIED )
        {
          rr_gprs_start_sys_info_retry_timer(GRR_TIMEOUT_1_SECOND, gas_id);
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_RETRY_TIMER_EXPIRY                              */
        /* A previously failed sys info read can now be retried          */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_RETRY_TIMER_EXPIRY )
        {
          continue_l1_bsi_acquisition(gas_id);
        }

        else
        /*****************************************************************/
        /* SIEV_STOP_AND_RESET_SOFTWARE                                  */
        /* Abort the acquisition in progress                             */
        /*****************************************************************/
        if ( event_in == SIEV_STOP_AND_RESET_SOFTWARE )
        {
          abort_l1_bsi_acquisition(gas_id);
        }

        break;
      }

      /*
      Routes to BSIST_ACQ_REQUESTED_SI13_REMOVED state:
       - SI3 indicates no GPRS
       - SI3 indicates no GPRS (30sec refresh)
       - SI13 not received (30sec refresh)
       - SI3 indicates no GPRS after SI13 added
       - SI3 indicates no GPRS (RACHing for TBF)
      */
      case BSIST_ACQ_REQUESTED_SI13_REMOVED:
      {
        /*****************************************************************/
        /* SIEV_GPRS_SUSPENDED                                           */
        /* GPRS has been suspended. Now start the acquisition            */
        /*****************************************************************/
        if ( event_in == SIEV_GPRS_SUSPENDED )
        {
          gprs_scell_info_t * scell_info = rr_gprs_get_scell_info(gas_id);

          /*
          Check if this is the case where SI3 is received indicating no GPRS support while RACHing for TBF.
          If so, the RACHing is aborted and GPRS suspended, but L1 will left in RACH state - this is
          indicated by the l1_idle_mode being GRR_L1_IDLE_MODE_NONE.
          L1 can be put in Early Camping straight from RACH state
          */
          if ( scell_info->l1_idle_mode == GRR_L1_IDLE_MODE_NONE )
          {
            /*
            Put L1 in Early Camping.
            Note: Because this case is SI3 received during RACHing, the CCD is available and therefore
            Early Camping (with NORMAL page mode) is possible.
            When L1 processes MPH_EARLY_CAMPING_REQ, L1's SI required mask is not
            modified - therefore a specific request must be sent
            */
            rr_l1_idle_send_imsg_start_early_camp_req(
              scell_info->gsm.BCCH_ARFCN,
              scell_info->gsm.BSIC,
              scell_info->gsm.location_area_identification,
              scell_info->gsm.cell_identity,
              scell_info->gsm.control_channel_description,
              scell_info->gsm.control_channel_description_valid,
              gas_id
            );
          }
          else
          {
            continue_l1_bsi_acquisition(gas_id);
          }
        }

        else
        /*****************************************************************/
        /* SIEV_MPH_START_IDLE_MODE_CNF                                  */
        /* This is generated when a MPH_START_IDLE_MODE_CNF is received  */
        /*****************************************************************/
        if ( event_in == SIEV_MPH_START_IDLE_MODE_CNF )
        {
          continue_l1_bsi_acquisition(gas_id);
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_MESSAGE_RECEIVED                                */
        /* A sys info message (any) has been received                    */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_MESSAGE_RECEIVED )
        {
          gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

          if ( scell_info_ptr->gsm.gprs_indicator )
          {
            MSG_GERAN_HIGH_0_G("SI13 added to cell");

            /*
            Setup SI masks to perform full SI refresh
            Note: previous reception history is NOT cleared
            */
            setup_full_refresh_acquisition(gas_id);

            rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED_SI13_ADDED;
            new_event = SIEV_SYS_INFO_MESSAGE_RECEIVED;
          }
          else
          if ( check_if_bsi_acquisition_complete(gas_id) )
          {
            new_event = SIEV_SYS_INFO_ACQUISITION_COMPLETE;
          }
          else
          {
            continue_l1_bsi_acquisition(gas_id);
          }
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_ACQUISITION_COMPLETE                            */
        /* All the requested BCCH SIs have been received.                */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_ACQUISITION_COMPLETE )
        {
          /*
          Clean-up, stop the acquisition timer, resume MAC, check L1 idle mode
          This call can return:
            SIEV_NO_EVENT - no change in L1 idle mode required
            SIEV_CHANGE_L1_IDLE_MODE - a new start idle should be sent to L1
            SIEV_ACTIVATE_SYS_INFO_ACQUISITION - continue to acquire SIs
          */
          grr_siev_t siev = bsi_acquisition_complete(gas_id);

          if ( (siev == SIEV_NO_EVENT) ||
               (siev == SIEV_ACTIVATE_SYS_INFO_ACQUISITION) )
          {
            /* As there is no change to L1 idle mode, update MM with the new cell capabiltiy */
            grr_update_mm_with_rr_service_ind(gas_id);

            /* Restart the 30s refresh timer */
            rr_gprs_start_sys_info_refresh_timer(gas_id);

            new_event = siev;
          }
          else
          {
            event_out = siev;
          }

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_NOT_REQUESTED;
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_ACQUISITION_TIMER_EXPIRY                        */
        /* Acquisition has failed. The MS must now reselect.             */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_ACQUISITION_TIMER_EXPIRY )
        {
          /* Stop reading BCCH SI, stop refresh timer */
          bsi_acquisition_failed(gas_id);

          /* Indicate acquisition failure - will cause a reselection */
          event_out = SIEV_SYS_INFO_ACQUISITION_FAILURE;

          /* Reset the state machine */
          rr_sys_info_si_data_ptr->state = BSIST_ACQ_NOT_REQUESTED;
        }

        else
        /*****************************************************************/
        /* SIEV_SUSPEND_SYS_INFO_ACQUISITION                             */
        /*****************************************************************/
        if ( event_in == SIEV_SUSPEND_SYS_INFO_ACQUISITION )
        {
          MSG_GERAN_HIGH_0_G("Sys info acquisition suspended (SI13 removed)");

          stop_l1_bsi_acquisition(gas_id);

          rr_sys_info_si_data_ptr->state = BSIST_ACQ_SUSPENDED;
        }

        else
        /*****************************************************************/
        /* SIEV_READ_SYS_INFO_FAILURE_UNSPECIFIED                        */
        /*****************************************************************/
        if ( event_in == SIEV_READ_SYS_INFO_FAILURE_UNSPECIFIED )
        {
          rr_gprs_start_sys_info_retry_timer(GRR_TIMEOUT_1_SECOND, gas_id);
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_RETRY_TIMER_EXPIRY                              */
        /* A previously failed sys info read can now be retried          */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_RETRY_TIMER_EXPIRY )
        {
          continue_l1_bsi_acquisition(gas_id);
        }

        else
        /*****************************************************************/
        /* SIEV_STOP_AND_RESET_SOFTWARE                                  */
        /* Abort the acquisition in progress                             */
        /*****************************************************************/
        if ( event_in == SIEV_STOP_AND_RESET_SOFTWARE )
        {
          abort_l1_bsi_acquisition(gas_id);
        }

        break;
      }

      case BSIST_ACQ_SUSPENDED:
      {
        /*****************************************************************/
        /* SIEV_ACTIVATE_SYS_INFO_ACQUISITION                            */
        /*****************************************************************/
        if ( event_in == SIEV_ACTIVATE_SYS_INFO_ACQUISITION )
        {
          /* Move back to the idle state, and examine if it's necessary to restart acquisition */
          rr_sys_info_si_data_ptr->state = BSIST_ACQ_NOT_REQUESTED;
          new_event = bsi_determine_action_after_activation(gas_id);
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_RETRY_TIMER_EXPIRY                              */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_RETRY_TIMER_EXPIRY )
        {
          /* Move back to the idle state, and examine if it's necessary to restart acquisition */
          rr_sys_info_si_data_ptr->state = BSIST_ACQ_NOT_REQUESTED;

          new_event = bsi_determine_action_after_activation(gas_id);

          /* If there is SI that needs to be acquired, cancel any ncell SI acquisition in progress */
          if ( new_event != SIEV_NO_EVENT )
          {
            rr_gprs_nsi_notify_scell_si_acquisition_required(gas_id);
          }
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_REFRESH_TIMER_EXPIRY                            */
        /* The 30sec refresh timer has expired whilst the BSI handler is */
        /* suspended. Setup the SI masks so a refresh is started once    */
        /* the handler is resumed                                        */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_REFRESH_TIMER_EXPIRY )
        {
          gprs_scell_info_t * cell_info;
          rr_ca_status_t  rr_ca_status;

          cell_info = rr_gprs_get_scell_info(gas_id);

          rr_ca_status = cell_info->gsm.valid_CA_received;
          /*valid_CA_received could be changed in the below function*/
          setup_refresh_acquisition(gas_id);

          /*if SI1 is required and we already received CA then modify valid_CA_received to
           RR_CA_RECEIVED_BUT_INVALID*/
          if((cell_info->bcch_sys_info.si_messages_required & MASK_SI_1) &&
            (rr_ca_status == RR_CA_RECEIVED))
          {
            cell_info->gsm.valid_CA_received = RR_CA_RECEIVED_BUT_INVALID;
          }
        }

        else
        /*****************************************************************/
        /* SIEV_SYS_INFO_MESSAGE_RECEIVED                                */
        /* A sys info message has been received, perhaps due to RACHing  */
        /* for CS or PS establishment                                    */
        /*****************************************************************/
        if ( event_in == SIEV_SYS_INFO_MESSAGE_RECEIVED )
        {
          /* Check for PBCCH activation, or SI13 addition/removal */
          /* This will return SIEV_NO_EVENT, SIEV_PBCCH_ACTIVATED or SIEV_SI13_ADDED_OR_REMOVED */
          new_event = grr_align_sys_info_acquisition_with_cell_capabilities(gas_id);

          /* SI13 status unchanged - no special action is required */
          if (new_event == SIEV_NO_EVENT)
          {
            gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);
            gprs_bcch_scell_info_t * bcch_sys_info = &cell_info->bcch_sys_info;

            /* so check if the SI just received was SI13 - and if so, use this */
            /* reception of SI13 during RACHing to restart the 30sec refresh timer */
            if ( bcch_sys_info->last_si_message_received == SI_TYPE_SI_13 )
            {
              rr_gprs_start_sys_info_refresh_timer(gas_id);
            }
          }
        }

        else
        if ( (event_in == SIEV_SI13_ADDED) ||
             (event_in == SIEV_SI13_REMOVED) )
        {
          /* If suspended due to CS connection (so SIs received due to RACHing), invalidate */
          /* the sys info, so that when the connection is released, a full sys info refresh will */
          /* be done */
          if ( grr_cs_conn_est_in_progress(gas_id) )
          {
            rr_gprs_invalidate_sys_info_during_cs_conn(RR_SI_INCONSISTENT, gas_id);
          }
          else
          /* If suspended for TBF establishment then handle this the same as if SI13 was */
          /* removed (SI13 added doesn't apply here, as GPRS would not be enabled) */
          /* can't during an acquisition, i.e. suspend GPRS and start a full refresh */
          if ( grr_tbf_est_in_progress(gas_id) )
          {
            gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

            MSG_GERAN_HIGH_0_G("Acquiring SI (SI13 removed from cell)");

            /*
            As L1 is currently in RACH state, set l1_idle_mode to indicate NONE, as this
            will ensure that after GPRS is suspended, a MPH_SELECT_SPECIFIC_BCCH_REQ will
            be sent to L1 to move it our of RACH state.
            This also ensures that when the SI acquisition completes, a change in L1 idle
            mode is detected
            */
            cell_info->l1_idle_mode = GRR_L1_IDLE_MODE_NONE;

            /*
            Setup SI masks to perform full SI refresh
            Note: previous reception history is NOT cleared
            */
            setup_full_refresh_acquisition(gas_id);

            /*
            Instruct grr_control to suspend GPRS
            Move to BSIST_ACQ_REQUESTED_SI13_REMOVED to wait for the confirmation that
            GPRS is suspended. Then continue the SI refresh
            */
            rr_sys_info_si_data_ptr->state = BSIST_ACQ_REQUESTED_SI13_REMOVED;
            event_out = SIEV_GPRS_SUSPEND_NO_SI13;
          }
        }

        break;
      }

      default:

        MSG_GERAN_ERROR_1_G("In unknown state (%d)",rr_sys_info_si_data_ptr->state);
        break;
    }

#ifdef DEBUG_RR_TRACE_MSG
    /* no old_state in this state machine */
    if ((event_in != new_event) && (new_event != SIEV_NO_EVENT))
    {
      /* this state machine uses its own event representation, so convert here */
      rr_store_trace_msg_buf(
        GRR_BSI_ACQ_SM,
        (rr_event_T)new_event,
        (byte)rr_sys_info_si_data_ptr->state,
        (void *)event_in_data,
        gas_id
      );
    }
#endif

    if ( ( event_in == SIEV_STOP_AND_RESET_SOFTWARE ) ||
         ( event_in == SIEV_RESET_SOFTWARE ) )
    {
      rr_sys_info_si_data_ptr->state = BSIST_ACQ_NOT_REQUESTED;
    }

    if ( new_event != SIEV_NO_EVENT )
    {
      event_in = new_event;
      new_event = SIEV_NO_EVENT;
    }
    else
    {
      event_in = SIEV_NO_EVENT;
    }

    if ( rr_sys_info_si_data_ptr->state != rr_sys_info_si_data_ptr->old_state )
    {
      RR_LOG_STATE_TRANSITION(
        "bsi_acquisition_control",
        rr_sys_info_si_data_ptr->old_state,
        rr_sys_info_si_data_ptr->state,
        get_bsi_acquisition_control_state_name,
        gas_id
      );

      rr_sys_info_si_data_ptr->old_state = rr_sys_info_si_data_ptr->state;
    }
  }

  return event_out;

} /* end bsi_acquisition_control() */

/*-----------------------------------------------------------------------------
 * Externalized Function Definitions
 * --------------------------------------------------------------------------*/

/*!
 * \brief Functions to merge measurement parameter modifications.
 *
 * \param dst (out)
 * \param mods (in)
 * \param gas_id (in)
 */
void rr_nc_meas_params_merge(
  nc_measurement_params_t *dst,
  nc_meas_params_mods_t *mods,
  const gas_id_t gas_id
)
{
  if (mods->valid)
  {
    dst->network_control_order = mods->network_control_order;
    if (mods->nc_period_valid)
    {
      dst->nc_non_drx_period = mods->nc_non_drx_period;
      dst->nc_reporting_period_i = mods->nc_reporting_period_i;
      dst->nc_reporting_period_t = mods->nc_reporting_period_t;
    }
    rr_eng_mode_update_nw_params(gas_id);
  }
}

/*!
 * \brief Functions to merge measurement parameter modifications.
 *
 * \param dst (out)
 * \param mods (in)
 */
void rr_gprs_meas_params_merge(
  gprs_meas_params_t *dst,
  gprs_meas_params_mods_t *mods
)
{
  if (mods->valid)
  {
    if (mods->pmo_ind_valid)
    {
      dst->pmo_ind_valid = mods->pmo_ind_valid;
      dst->pmo_ind = mods->pmo_ind;
    }
    if (mods->report_type_valid)
    {
      dst->report_type = mods->report_type;
    }
    if (mods->reporting_rate_valid)
    {
      dst->reporting_rate = mods->reporting_rate;
    }
    if (mods->invalid_bsic_reporting_valid)
    {
      dst->invalid_bsic_reporting = mods->invalid_bsic_reporting;
    }
    if (mods->multiband_reporting_valid)
    {
      dst->multiband_reporting = mods->multiband_reporting;
    }
    if (mods->serving_band_reporting_valid)
    {
      dst->serving_band_reporting = mods->serving_band_reporting;
    }
    if (mods->scale_ord_valid)
    {
      dst->scale_ord = mods->scale_ord;
    }
    if (mods->reporting_900_valid)
    {
      dst->reporting_900_offset = mods->reporting_900_offset;
      dst->reporting_900_threshold = mods->reporting_900_threshold;
    }
    if (mods->reporting_1800_valid)
    {
      dst->reporting_1800_offset = mods->reporting_1800_offset;
      dst->reporting_1800_threshold = mods->reporting_1800_threshold;
    }
    if (mods->reporting_450_valid)
    {
      dst->reporting_450_offset = mods->reporting_450_offset;
      dst->reporting_450_threshold = mods->reporting_450_threshold;
    }
    if (mods->reporting_1900_valid)
    {
      dst->reporting_1900_offset = mods->reporting_1900_offset;
      dst->reporting_1900_threshold = mods->reporting_1900_threshold;
    }
    if (mods->reporting_850_valid)
    {
      dst->reporting_850_offset = mods->reporting_850_offset;
      dst->reporting_850_threshold = mods->reporting_850_threshold;
    }
  }
}

/*!
 * \brief Functions to merge 3G measurement parameter modifications.
 *
 * \param dst (out)
 * \param mods (in)
 */
void rr_gprs_3g_meas_params_merge(
  gprs_3g_meas_params_t *dst,
  gprs_3g_meas_params_mods_t *mods,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  if (mods->valid)
  {
    if (mods->Qsearch_P_valid)
    {
      dst->Qsearch_P = mods->Qsearch_P;
    }
    if (mods->_3G_SEARCH_PRIO_valid)
    {
      dst->_3G_SEARCH_PRIO = mods->_3G_SEARCH_PRIO;
    }
    if (mods->FDD_valid)
    {
      dst->FDD_GPRS_Qoffset = mods->FDD_GPRS_Qoffset;
      dst->FDD_Qmin = mods->FDD_Qmin;
    }
    if (mods->TDD_valid)
    {
      dst->TDD_GPRS_Qoffset = mods->TDD_GPRS_Qoffset;
    }
    if (mods->FDD_reporting_valid)
    {
      dst->FDD_REP_QUANT = mods->FDD_REP_QUANT;
      dst->FDD_MULTIRAT_REPORTING = mods->FDD_MULTIRAT_REPORTING;
    }
    if (mods->FDD_off_thresh_valid)
    {
      dst->FDD_REPORTING_OFFSET = mods->FDD_REPORTING_OFFSET;
      dst->FDD_REPORTING_THRESHOLD = mods->FDD_REPORTING_THRESHOLD;
    }
    if (mods->TDD_reporting_valid)
    {
      dst->TDD_MULTIRAT_REPORTING = mods->TDD_MULTIRAT_REPORTING;
    }
    else
    {
      dst->TDD_MULTIRAT_REPORTING = scell_info_ptr->gsm.utran_neighbor_list.si2q_params.TDD_MULTIRAT_REPORTING;
    }
    if (mods->TDD_off_thresh_valid)
    {
      dst->TDD_REPORTING_OFFSET = mods->TDD_REPORTING_OFFSET;
      dst->TDD_REPORTING_THRESHOLD = mods->TDD_REPORTING_THRESHOLD;
    }
    if (mods->CDMA2000_reporting_valid)
    {
      dst->CDMA2000_MULTIRAT_REPORTING = mods->CDMA2000_MULTIRAT_REPORTING;
    }
    if (mods->CDMA2000_off_thresh_valid)
    {
      dst->CDMA2000_REPORTING_OFFSET = mods->CDMA2000_REPORTING_OFFSET;
      dst->CDMA2000_REPORTING_THRESHOLD = mods->CDMA2000_REPORTING_THRESHOLD;
    }
    if (mods->FDD_additional_MPs_valid)
    {
      dst->FDD_additional_MPs_valid = TRUE;
      dst->FDD_Qmin_offset = mods->FDD_Qmin_offset;
      dst->FDD_RSCPmin = mods->FDD_RSCPmin;
    }
    else
    {
      dst->FDD_additional_MPs_valid = FALSE;
    }
  }
  else
  {
    dst->TDD_MULTIRAT_REPORTING = scell_info_ptr->gsm.utran_neighbor_list.si2q_params.TDD_MULTIRAT_REPORTING;
  }
}

#ifdef FEATURE_LTE
void rr_csn_pcid_group_ie_to_rr_pcid_group_ie(
  rr_pcid_group_ie_t * dst,
  csn_pcid_group_ie_t * src
)
{
  uint32 pcid;
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(dst);
  RR_NULL_CHECK_RETURN_VOID(src);

  for (pcid = 0; pcid <= CSN_PCID_GROUP_IE_MAX_PCID && pcid <= RR_LTE_PCID_MAX; ++pcid)
  {
    if (CSN_PCID_GROUP_IE_GET(src->bitmap, pcid))
    {
      RR_PCID_GROUP_IE_SET(dst->bitmap, pcid);
    }
  }

  for (i = 0;
       i < src->pcid_pattern_count &&
       i < CSN_PCID_GROUP_IE_MAX_PATTERNS;
       ++i)
  {
    uint32 length = src->pcid_patterns[i].length;
    uint32 value = src->pcid_patterns[i].value;
    uint32 sense = src->pcid_patterns[i].sense;
    uint32 pcid;

    /* Max PCID length is 9 bits.  We have n bits of pattern.  We need to shift this
    left. */
    uint32 mask = value << (9 - length);

    /* Check all possible PCIDs (0 - 503 inclusive) to find those
    that match the PCID pattern, and set them appropriately. */
    for (pcid = 0; pcid <= RR_LTE_PCID_MAX; ++pcid)
    {
      if (sense == 0)
      {
        if ((pcid & mask) == mask)
        {
          RR_PCID_GROUP_IE_SET(dst->bitmap, pcid);
        }
      }
      else
      {
        if ((pcid & mask) != mask)
        {
          RR_PCID_GROUP_IE_SET(dst->bitmap, pcid);
        }
      }
    }
  }
}

boolean rr_find_earfcn_index_in_lte_neighbour_list(
  rr_lte_neighbour_list_t * lte_neighbour_list,
  uint16                    earfcn,
  uint32                  * index
)
{
  uint32  i;
  boolean found = FALSE;

  RR_NULL_CHECK_RETURN_PARAM(lte_neighbour_list, FALSE);

  for (i = 0; i < lte_neighbour_list->count; i++)
  {
    if (lte_neighbour_list->entries[i].frequency.earfcn == earfcn)
    {
      *index = i;
      found = TRUE;
      break;
    }
  }

  return found;
}
#endif /* FEATURE_LTE */

/*!
 * \brief Applies modifications to measurement parameters from PMO or PCCO contents.
 *
 * \param channel_number
 * \param gas_id
 */
void rr_gprs_update_meas_params_with_modifications(
  ARFCN_T channel_number,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *cell = NULL;

  if (rr_is_this_the_scell(channel_number,gas_id))
  {
    cell = rr_gprs_get_scell_info(gas_id);
  }
  else if (rr_is_this_the_pcell(channel_number,gas_id))
  {
    cell = rr_gprs_get_pcell_info(gas_id);
  }

  if (cell)
  {
    rr_params_mods_t        ** params_mods = rr_params_mods(gas_id);
    rr_params_mods_t        ** pcco_params_mods = rr_pcco_params_mods(gas_id);
    nc_measurement_params_t  * nc_meas_params = &cell->nc_meas_params;
    gprs_meas_params_t       * gprs_meas_params = &cell->gprs_meas_params;
#ifdef FEATURE_WCDMA
    gprs_3g_meas_params_t    * gprs_3g_meas_params = &cell->gprs_3g_meas_params;
#endif /* FEATURE_WCDMA */
    gprs_cell_options_t      * gprs_cell_options = &cell->bcch_sys_info.gprs_cell_options;

    /* Merge PMO parameters in, if present. */
    if (params_mods != NULL && *params_mods != NULL)
    {
      rr_nc_meas_params_merge(nc_meas_params, &(*params_mods)->nc_meas_params,gas_id);
      rr_gprs_meas_params_merge(gprs_meas_params, &(*params_mods)->gprs_meas_params);
#ifdef FEATURE_WCDMA
      rr_gprs_3g_meas_params_merge(gprs_3g_meas_params,  &(*params_mods)->gprs_3g_meas_params,gas_id);
#endif /* FEATURE_WCDMA */
      rr_gprs_merge_nc_nacc_into_cell_nacc_params(gprs_cell_options, &(*params_mods)->nacc_params,gas_id);

      if ((*params_mods)->serving_cell_priority_params_valid)
      {
        MSG_GERAN_HIGH_0_G("Using Serving Cell Priority Parameters from PMO");
        cell->serving_cell_priority_params_valid = TRUE;
        cell->serving_cell_priority_params = (*params_mods)->serving_cell_priority_params;
      }

      if ((*params_mods)->utran_priority_params_valid)
      {
        MSG_GERAN_HIGH_0_G("Using UTRAN Priority Parameters from PMO");
        rr_utran_priority_params_merge_mods(
          &cell->utran_priority_params,
          &(*params_mods)->utran_priority_params
        );
      }

#ifdef FEATURE_LTE
      if ((*params_mods)->eutran_params_valid)
      {
        uint32 i;

        MSG_GERAN_HIGH_0_G("Using E-UTRAN Priority Parameters from PMO");

        rr_eutran_params_merge_csn_mods(
          &cell->eutran_params,
          &(*params_mods)->eutran_params
        );

        /* Append any LTE neighbour frequencies from PMO to the neighbour list. */
        for (i = 0;
             i < cell->eutran_params.neighbour_cells_count &&
             i < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS;
             ++i)
        {
          rr_eutran_neighbour_cells_t * src = &cell->eutran_params.neighbour_cells[i];
          uint32 j;

          for (j = 0;
               j < src->eutran_frequency_list_count &&
               j < MAX_EUTRAN_NEIGHBOUR_CELLS_EARFCNS;
               ++j)
          {
            if (cell->lte_neighbour_list.count < MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE)
            {
              rr_lte_neighbour_list_entry_t * dst;
              uint32 index;

              /* If same earfcn is already present in neighbour list, just update it.
               * Otherwise create a new entry. */
              if (rr_find_earfcn_index_in_lte_neighbour_list(&cell->lte_neighbour_list, src->eutran_frequency_list[j].earfcn, &index))
              {
                MSG_GERAN_HIGH_1_G("Updating LTE EARFCN %d to neighbour list (PMO)", src->eutran_frequency_list[j].earfcn);
              }
              else
              {
                MSG_GERAN_HIGH_1_G("Adding LTE EARFCN %d to neighbour list (PMO)", src->eutran_frequency_list[j].earfcn);
                index = cell->lte_neighbour_list.count;
                ++cell->lte_neighbour_list.count;
              }

              dst = &cell->lte_neighbour_list.entries[index];

              memset(dst, 0, sizeof(rr_lte_neighbour_list_entry_t));

              dst->frequency = src->eutran_frequency_list[j];
              if (src->eutran_priority_valid)
              {
                dst->priority = src->eutran_priority;
              }
              else
              {
                MSG_GERAN_ERROR_1_G("No priority received for EARFCN %d", src->eutran_frequency_list[j].earfcn);
                dst->priority = RR_RESEL_PRIORITY_UNDEFINED;
              }
              dst->thresh_high = src->thresh_eutran_high;
              if (src->thresh_eutran_low_valid)
              {
                dst->thresh_low = src->thresh_eutran_low;
              }
              else
              {
                dst->thresh_low = src->thresh_eutran_high;
              }
              if (src->eutran_qrxlevmin_valid)
              {
                dst->qrxlevmin = src->eutran_qrxlevmin;
              }
              else
              {
                dst->qrxlevmin = RR_DEFAULT_EUTRAN_QRXLEVMIN;
              }
            }
          }
        }

        /* Now merge in the Not Allowed Cells data structures. */
        for (i = 0;
             i < cell->eutran_params.not_allowed_cells_count &&
             i < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS;
             ++i)
        {
          csn_eutran_not_allowed_cells_t * src = &cell->eutran_params.not_allowed_cells[i];
          uint32 j;

          /* Update the LTE neighbour cell list for each index that is set in
          the frequency index bitmap (from explicitly encoded PCIDs and from
          the PCID_BITMAP_GROUP field). */
          for (j = 0; j < CSN_EUTRAN_FREQUENCY_INDICES_BITMAP_LEN; ++j)
          {
            if (IS_BIT_NUM_SET(src->eutran_frequency_indices_bitmap, j) &&
                j <= cell->lte_neighbour_list.count)
            {
              rr_csn_pcid_group_ie_to_rr_pcid_group_ie(
                &cell->lte_neighbour_list.entries[j].not_allowed_cells,
                &src->pcid_group
              );
            }
          }
        }
      }
#endif /* FEATURE_LTE */
    }

    /* Merge PCCO parameters in, if present. */
    if (pcco_params_mods != NULL && *pcco_params_mods != NULL)
    {
      rr_nc_meas_params_merge(nc_meas_params, &(*pcco_params_mods)->nc_meas_params,gas_id);
      rr_gprs_meas_params_merge(gprs_meas_params, &(*pcco_params_mods)->gprs_meas_params);
#ifdef FEATURE_WCDMA
      rr_gprs_3g_meas_params_merge(gprs_3g_meas_params,  &(*pcco_params_mods)->gprs_3g_meas_params,gas_id);
#endif /* FEATURE_WCDMA */
      rr_gprs_merge_nc_nacc_into_cell_nacc_params(gprs_cell_options, &(*pcco_params_mods)->nacc_params,gas_id);
    }
  }
}

static void rr_eutran_params_merge_si2quater_mods(
  rr_eutran_params_t                   * dst,
  rr_si2quater_ro_eutran_params_desc_t * si2quater_mods,
  gas_id_t                              gas_id
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(dst);
  RR_NULL_CHECK_RETURN_VOID(si2quater_mods);

  dst->ccn_active = si2quater_mods->eutran_ccn_active;

  if (rr_si2quater_all_instances_received(rr_get_instances_ptr(SI2Q_INSTANCE,gas_id)))
  { /* E-UTRAN Measurement Parameters Description */
    if (si2quater_mods->eutran_meas_params != NULL)
    {
      dst->meas_params = *si2quater_mods->eutran_meas_params;
    }

    /* GPRS E-UTRAN Measurement Parameters Description */
    if (si2quater_mods->gprs_eutran_meas_params != NULL)
    {
      dst->gprs_meas_params = *si2quater_mods->gprs_eutran_meas_params;
    }

    if ( (si2quater_mods->eutran_meas_params != NULL) && (si2quater_mods->gprs_eutran_meas_params != NULL))
    {
	  MSG_GERAN_HIGH_0("E-UTRAN and GPRS E-UTRAN Measurement Parameters present"); 
    }
    else if ( (si2quater_mods->eutran_meas_params != NULL) && (si2quater_mods->gprs_eutran_meas_params == NULL))
    {
	  MSG_GERAN_HIGH_0("E-UTRAN Measurement Parameters present"); 
    }
    else if ( (si2quater_mods->eutran_meas_params == NULL) && (si2quater_mods->gprs_eutran_meas_params != NULL))
    {
	  MSG_GERAN_HIGH_0("GPRS E-UTRAN Measurement Parameters present"); 
    }
  }
  /* Repeated E-UTRAN Neighbour Cells */
  for (i = 0;
       i < si2quater_mods->eutran_neighbour_cells_count &&
       i < MAX_REPEATED_EUTRAN_NEIGHBOUR_CELLS;
       ++i)
  {
    if (dst->neighbour_cells_count < RR_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS)
    {
      dst->neighbour_cells[dst->neighbour_cells_count] = *(si2quater_mods->eutran_neighbour_cells[i]);
      ++dst->neighbour_cells_count;
    }
    else
    {
      MSG_GERAN_ERROR_1("Overflow merging E-UTRAN Neighbour Cells (%d)", i);
    }
  }

  /* Repeated E-UTRAN Not Allowed Cells */
  for (i = 0;
       i < si2quater_mods->eutran_not_allowed_cells_count &&
       i < MAX_REPEATED_EUTRAN_NOT_ALLOWED_CELLS;
       ++i)
  {
    if (dst->not_allowed_cells_count < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS)
    {
      //dst->not_allowed_cells[dst->not_allowed_cells_count] = si2quater_mods->not_allowed_cells[i];
      //++dst->not_allowed_cells_count;
    }
    else
    {
      MSG_GERAN_ERROR_1("Overflow merging E-UTRAN Not Allowed Cells (%d)", i);
    }
  }

  /* Repeated E-UTRAN PCID to TA mapping */
  for (i = 0;
       i < si2quater_mods->eutran_pcid_to_ta_mapping_count &&
       i < MAX_REPEATED_EUTRAN_PCID_TO_TA_MAPPING;
       ++i)
  {
    //if (dst->not_allowed_cells_count < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS)
    //{
      //dst->not_allowed_cells[dst->not_allowed_cells_count] = si2quater_mods->not_allowed_cells[i];
      //++dst->not_allowed_cells_count;
    //}
    //else
    //{
     // MSG_GERAN_ERROR_1("Overflow merging E-UTRAN Not Allowed Cells (%d)", i);
    //}
  }
}

/*!
 * \brief Revert to broadcast measurement paramters.
 *
 * \param cell (in)
 * \param si_db_ptr (in)
 * \param gas_id (in)
 */
void rr_gprs_revert_to_broadcast_meas_params_51(
  gprs_scell_info_t *cell,
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
)
{
  nc_measurement_params_t *nc_meas_params      = &cell->nc_meas_params;
  gprs_meas_params_t      *gprs_meas_params    = &cell->gprs_meas_params;
  SI2q_MI_instance_T      *si2quater_instance;
  instances_T             *si2q_inst_ptr;
  int                      i;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
  gprs_3g_meas_params_t   *gprs_3g_meas_params = &cell->gprs_3g_meas_params;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/
  boolean scell_params = FALSE;
  boolean utran_params = FALSE;
  boolean eutran_params = FALSE;

  /* clear the current parameters (setting to defaults other than 0 where appropriate) */
  memset(nc_meas_params, 0, sizeof(nc_measurement_params_t));
  nc_meas_params->nc_non_drx_period = DEFAULT_NON_DRX_PERIOD;
  nc_meas_params->nc_reporting_period_i = DEFAULT_NC_REPORTING_PERIOD_I;

  nc_meas_params->nc_reporting_period_t = DEFAULT_NC_REPORTING_PERIOD_T;
  //wasnc_meas_params->nc_reporting_period_t = MAX_NC_REPORTING_PERIOD_INDEX;

  memset(gprs_meas_params, 0, sizeof(gprs_meas_params_t));
  gprs_meas_params->report_type = REPORT_TYPE_N0RMAL;

  cell->serving_cell_priority_params_valid = FALSE;
  memset(&cell->serving_cell_priority_params, 0, sizeof(csn_gsm_priority_params_ie_t));
  memset(&cell->utran_priority_params, 0, sizeof(csn_utran_priority_params_ie_t));

  memset(&cell->eutran_params, 0, sizeof(rr_eutran_params_t));
  cell->eutran_params.meas_params.qsearch_c = RR_DEFAULT_QSEARCH_C_EUTRAN;
  cell->eutran_params.gprs_meas_params.qsearch_p = RR_DEFAULT_QSEARCH_P_EUTRAN;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
  {
    si2ter_instances_T *si2ter_inst_ptr;

    memset(gprs_3g_meas_params, 0, sizeof(gprs_3g_meas_params_t));
    gprs_3g_meas_params->FDD_GPRS_Qoffset = DEFAULT_FDD_GPRS_QOFFSET;
    gprs_3g_meas_params->FDD_Qmin         = DEFAULT_FDD_QMIN;
    gprs_3g_meas_params->Qsearch_P        = DEFAULT_QSEARCH_P;
    gprs_3g_meas_params->FDD_Qmin_offset  = DEFAULT_FDD_QMIN_OFFSET;
    gprs_3g_meas_params->FDD_RSCPmin      = DEFAULT_FDD_RSCPMIN;
    gprs_3g_meas_params->TDD_GPRS_Qoffset = DEFAULT_TDD_GPRS_QOFFSET;

    si2ter_inst_ptr = rr_get_SI2ter_instances_ptr(gas_id);
    if (si2ter_inst_ptr != NULL)
    {
      si2ter_rest_T *si2ter_ptr;

      /* merge in changes in SI2ter instances */
      for (i = 0; i < MAX_SI2TER_INSTANCES; i++)
      {
        si2ter_ptr = &(si2ter_inst_ptr->instance[i]);

        if (!si2ter_ptr->valid_data)
        {
          continue;
        }

        rr_gprs_3g_meas_params_merge(
          gprs_3g_meas_params,
          &(si2ter_ptr->gprs_3g_meas_params),
          gas_id
        );
      }
    }
  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

  /* merge in changes in SI13 */
  rr_nc_meas_params_merge(
    nc_meas_params,
    &cell->bcch_sys_info.nc_meas_params_si13,
    gas_id
  );

  /* multiband_reporting is received in SI2ter, merge it in here manually */
  cell->gprs_meas_params.multiband_reporting = si_db_ptr->multiband_reporting;

  /* merge in changes in SI2quater instances */
  si2q_inst_ptr = rr_get_instances_ptr(SI2Q_INSTANCE, gas_id);

  if (si2q_inst_ptr != NULL)
  {
    for (i = 0; i < MAX_INSTANCES; i++)
    {
      si2quater_instance = &(si2q_inst_ptr->instance[i]);

      if (!si2quater_instance->valid_data)
      {
        continue;
      }

      rr_nc_meas_params_merge(
        nc_meas_params,
        &si2quater_instance->si2q_mi.si2q_params.nc_meas_params,
        gas_id
      );

      rr_gprs_meas_params_merge(
        gprs_meas_params,
        &si2quater_instance->si2q_mi.si2q_params.gprs_meas_params
      );

#ifdef FEATURE_WCDMA
      rr_gprs_3g_meas_params_merge(
        gprs_3g_meas_params,
        &si2quater_instance->si2q_mi.si2q_params.gprs_3g_meas_params,
    gas_id
      );

      /* If RR is camped on a BCCH, FDD_Qmin must be taken from the 3G
         Measurement Parameters in SI2quater as it is not contained within the
         GPRS 3G Measurement Paramters. */
      if (si2quater_instance->si2q_mi.si2q_params.FDD_Info_PI)
      {
        gprs_3g_meas_params->FDD_Qmin = si2quater_instance->si2q_mi.si2q_params.FDD_Qmin;
      }
#endif /* FEATURE_WCDMA */

      if ( si2quater_instance->si2q_mi.si2q_params.ccn_support_flag && rr_gprs_camped_on_ccch(gas_id) )
      {
        rr_gprs_merge_ccn_support_into_ba_list(
          &si2quater_instance->si2q_mi.si2q_params.ccn_support_desc,
          gas_id
        );
      }

      if (si2quater_instance->si2q_mi.si2q_params.priority_and_eutran_params_desc != NULL)
      {
        if (si2quater_instance->si2q_mi.si2q_params.priority_and_eutran_params_desc->serving_cell_priority_params != NULL)
        {
          scell_params = TRUE;
          /* No merging to do with the Serving Cell Priority Parameters
          Description, new values just overwrite the old values. */
          cell->serving_cell_priority_params_valid = TRUE;
          cell->serving_cell_priority_params = *si2quater_instance->si2q_mi.si2q_params.priority_and_eutran_params_desc->serving_cell_priority_params;
        }

        if (si2quater_instance->si2q_mi.si2q_params.priority_and_eutran_params_desc->utran_priority_params != NULL)
        {
          utran_params = TRUE;
          rr_utran_priority_params_merge_mods(
            &cell->utran_priority_params,
            si2quater_instance->si2q_mi.si2q_params.priority_and_eutran_params_desc->utran_priority_params
          );
        }

        if (si2quater_instance->si2q_mi.si2q_params.priority_and_eutran_params_desc->eutran_params_desc != NULL)
        {
          eutran_params = TRUE;
          rr_eutran_params_merge_si2quater_mods(
            &cell->eutran_params,
            si2quater_instance->si2q_mi.si2q_params.priority_and_eutran_params_desc->eutran_params_desc,
			gas_id
          );
        }
		
	  }
          
      /*update int_pwr_red info rcvd in si2q*/
      if(si2quater_instance->si2q_mi.si2q_params.int_pwr_red_present == TRUE)
      {
        cell->pwr_reduction_valid = si2quater_instance->si2q_mi.si2q_params.int_pwr_red_present;
        cell->int_pwr_red_value = si2quater_instance->si2q_mi.si2q_params.int_pwr_red;
        cell->nc2_csg_pccn_permitted = si2quater_instance->si2q_mi.si2q_params.nc2_csg_pccn_permitted;
      }

    } /* end for */
	 MSG_GERAN_HIGH_3_G("Merging Serving Cell %d, UTRAN %d, EUTRAN %d Priority Parameters from SI2quater to SCELL", scell_params, utran_params, eutran_params);
  }

#ifdef FEATURE_LTE
 if ( rr_create_lte_neighbour_list(
    &cell->gsm.lte_neighbour_list,
    si2q_inst_ptr ))
    {
      cell->lte_neighbour_list = cell->gsm.lte_neighbour_list;

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
      /* Previously GERAN was using Pseudo SI2Q data, Time to clean Up Now as the
        Real ONE has arrived */
      rr_set_rr_using_pseudo_si2q_data(FALSE, gas_id);
#endif /* FEATURE_FAST_G2L_PSEUDO_SI2Q */
    }

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
    if ( rr_is_pseudo_si2q_data_used(gas_id) && cell->lte_neighbour_list.count > 0 )
    {
      cell->gsm.lte_neighbour_list = cell->lte_neighbour_list;
      MSG_GERAN_HIGH_0_G("Restoring serving cell priority for PSEUDO SI2q ");
      /* Restore back the Defaults as they are reset at the begining of the function */
      rr_restore_default_serv_utran_pseudo_si2q_params(cell, gas_id);  
    }
#endif /* FEATURE_FAST_G2L_PSEUDO_SI2Q */

#endif /* FEATURE_LTE */

  /* sanity check the network control order here, make sure it is valid
   * we may receive a NCO with value 3 in PMO (and maybe in PCCO) but never
   * in broadcast system information.
   */
  if ((nc_meas_params->network_control_order != NC_MEAS_NC0) &&
      (nc_meas_params->network_control_order != NC_MEAS_NC1) &&
      (nc_meas_params->network_control_order != NC_MEAS_NC2))
  {
    MSG_GERAN_HIGH_1_G("Invalid broadcast network control order %d, override to 0",
             nc_meas_params->network_control_order);
    nc_meas_params->network_control_order = NC_MEAS_NC0;
    rr_eng_mode_update_nw_params(gas_id);
  }
}

/*!
 * \brief This function should be called whenever SI13, SI2ter and SI2quater are received.
 *
 * \param channel_number (in)
 * \param update_nc_meas_control (in)
 * \param gas_id (in)
 */
void rr_gprs_update_meas_params_from_bcch(
  ARFCN_T channel_number,
  boolean update_nc_meas_control,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *cell = NULL;
  system_information_database_T *system_information_database_ptr = NULL;

  if (rr_is_this_the_scell(channel_number, gas_id))
  {
    cell = rr_gprs_get_scell_info(gas_id);
    system_information_database_ptr = &cell->gsm;
  }
  else if (rr_is_this_the_pcell(channel_number, gas_id))
  {
    cell = rr_gprs_get_pcell_info(gas_id);
    system_information_database_ptr = &cell->gsm;
  }

  if (cell)
  {
    rr_gprs_revert_to_broadcast_meas_params_51(
      cell,
      system_information_database_ptr,
      gas_id
    );
  }

  /* finally, merge in any changes received in PMO or PCCO messages */
  rr_gprs_update_meas_params_with_modifications(
    channel_number,
    gas_id
  );

  if (rr_gprs_camped_on_ccch(gas_id) && update_nc_meas_control)
  {
    rr_send_priority_imsg_rr_event_ind(RR_EV_MEASUREMENT_PARAMS_UPDATED, gas_id);
  }
}

/*!
 * \brief Update measurement parameters.
 *
 * \param gas_id (in)
 */
void rr_gprs_update_meas_params(
  const gas_id_t gas_id
)
{
  ARFCN_T arfcn = rr_gprs_get_scell_arfcn(gas_id);

  if (rr_gprs_camped_on_ccch(gas_id))
  {
    rr_gprs_update_meas_params_from_bcch(
      arfcn,    // channel_number
      FALSE,    // update_nc_meas_control
      gas_id    // gas_id
    );
  }
}

/*!
 * \brief Update BSIC information received from SI2quater.
 *
 * \param channel_number (in)
 * \param gas_id (in)
 */
void rr_gprs_update_gprs_bsic_description_from_si2quater_instances(
  ARFCN_T channel_number,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t             *cell;
  instances_T                   *si2q_inst_ptr;
  system_information_database_T *system_information_database_ptr = NULL;

  if (rr_is_this_the_scell(channel_number, gas_id))
  {
    cell = rr_gprs_get_scell_info(gas_id);
    system_information_database_ptr = &cell->gsm;
  }
  else if (rr_is_this_the_pcell(channel_number, gas_id))
  {
    cell = rr_gprs_get_pcell_info(gas_id);
    system_information_database_ptr = &cell->gsm;
  }
  else
  {
    cell = NULL;
  }

  /* obtain pointer to global SI2q instances store */
  si2q_inst_ptr = rr_get_instances_ptr(SI2Q_INSTANCE, gas_id);
  if ((cell != NULL) && (si2q_inst_ptr != NULL))
  {
    uint32 ba_index_start_bsic;
    uint32 instance_frequency_index;
    int    i;

    memset(&cell->gprs_bsic_description, 0, sizeof(rr_gsm_bsic_description_T));

    /* Each instance of SI2quater specifies BA_INDEX_START_BSIC, the index at which
    to start filling in BSICs. */

    /* Iterate over GPRS BSIC Description structures received in SI2quater instances
    to populate the GPRS BSIC Description structure for cell. */

    for (i = 0; i < MAX_INSTANCES; i++)
    {
#ifdef FEATURE_LTE
      gprs_bsic_description_instance_t *instance = si2q_inst_ptr->instance[i].si2q_mi.si2q_params.gprs_bsic_description;
#else
      gprs_bsic_description_instance_t *instance = &(si2q_inst_ptr->instance[i].si2q_mi.si2q_params.gprs_bsic_description);
#endif /* FEATURE_LTE */

      if (!si2q_inst_ptr->instance[i].valid_data)
      {
        continue;
      }

#ifdef FEATURE_LTE
      if (instance == NULL)
      {
        continue;
      }
#endif /* FEATURE_LTE */

      ba_index_start_bsic = instance->ba_index_start_bsic;

      MSG_GERAN_MED_3_G("SI2QUATER_INDEX: %d, BA_INDEX_START_BSIC: %d",
              i,
              ba_index_start_bsic,
              0);

      for (instance_frequency_index = 0; instance_frequency_index < instance->entry_count; instance_frequency_index++)
      {
        gprs_bsic_description_instance_entry_t *entry = &instance->entries[instance_frequency_index];

        uint32 ba_index = ba_index_start_bsic + instance_frequency_index;
        uint32 bsic_index;

        if (ba_index < MAX_SIZE_OF_GSM_NEIGHBOUR_CELL_LIST)
        {
          for (bsic_index = 0; bsic_index < entry->size && bsic_index < MAX_BSIC_PER_FREQUENCY && bsic_index < GSM_BSIC_DESCRIPTION_MAX_BSICS_PER_ENTRY; ++bsic_index)
          {
            CONVERT_TO_BSIC_T(cell->gprs_bsic_description.bsics[ba_index].bsic[bsic_index],
                              entry->bsic[bsic_index]);
          }

          cell->gprs_bsic_description.bsics[ba_index].num_of_bsics = (byte) bsic_index;
        }
        else
        {
          MSG_GERAN_ERROR_1_G("ba_index %d out of range", ba_index);
        }
      }
    }

    /* Copy the GPRS BSIC Description to the GSM BSIC Description to be used
    in a dedicated call until Measurement Information is received. */
    system_information_database_ptr->gsm_bsic_description = cell->gprs_bsic_description;


    for (i = 0; i < MAX_SIZE_OF_GSM_NEIGHBOUR_CELL_LIST; i++)
    {
      int j;

      if (cell->gprs_bsic_description.bsics[i].num_of_bsics > 0)
      {
        MSG_GERAN_MED_1_G("GPRS BSIC Description for index: %d", i);
      }

      for (j = 0; j < cell->gprs_bsic_description.bsics[i].num_of_bsics; j++)
      {
        MSG_GERAN_MED_3_G("  (%d, %d)",
                cell->gprs_bsic_description.bsics[i].bsic[j].BS_colour_code,
                cell->gprs_bsic_description.bsics[i].bsic[j].PLMN_colour_code,
                0);
      }
    }

    if (system_information_database_ptr->camped_on_flag)
    {
      rr_gprs_build_ba_gprs_from_si2(cell, gas_id);
    }
  }
}

/*!
 * \brief This function applies the CCN parameters provided by the network to the BA list.
 *
 *  This function gets the CCN parameters that have been provided by the network in SI2quater and applies
 *  them to the BA list. Each ncell will then have a CCN_SUPPORTED flag.
 *
 * \param ccn_support_desc (in)
 * \param gas_id (in)
 */
void rr_gprs_merge_ccn_support_into_ba_list(
  csn_ccn_support_desc_t *ccn_support_desc,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t            *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  BCCH_frequency_list_T         *ba_list;
  gsm_ncell_list_member_t       *ncell;
  boolean                        local_ccn_supported;
  boolean                        local_ccn_valid;
  uint8                          array_byte_location = 0;
  uint8                          bit_location        = 0;
  uint8                          index;

  ba_list    = &(scell_info_ptr->gsm.complete_BA_list);

  /* number of cells in the CCN Support Description must not exceed number in BA list */
  if ( ccn_support_desc->number_cells > ba_list->no_of_entries )
  {
    MSG_GERAN_MED_2_G("CCN support (%d cells) cannot be matched with BA list (%d cells)",
            ccn_support_desc->number_cells, ba_list->no_of_entries);
    return;
  }

  /* If the CCN Support Description bitmap has LESS entries than the BA List
   * then neighbour cells with indices outside the range of the bitmap will be
   * updated to indicate that the CCN support flag is NOT valid. See 3GPP 44.060 $8.8.2a.
   * NACC availability for that neighbour will therefore be based on the serving cell only
   */
  for ( index = 0; index < ba_list->no_of_entries; index++ )
  {
    ncell = rr_gprs_get_ncell_info(ba_list->BCCH_ARFCN[index], NULL, gas_id);

    if ( (ncell != NULL) && (index < (CCN_SUPPORT_BITMAP_LENGTH << 3)) )
    {
      if ( ccn_support_desc->ccn_supported[array_byte_location] & (1 << bit_location) )
      {
        /* deliberate reversed logic here: if the bit is 1, CCN is NOT enabled */
        local_ccn_supported = FALSE;
      }
      else
      {
        /* if the bit is zero, CCN is enabled */
        local_ccn_supported = TRUE;
      }

      /* if the index is covered by the CCN Support bitmap, the local flag is valid */
      if ( index < ccn_support_desc->number_cells )
      {
        local_ccn_valid = TRUE;
      }
      else
      {
        local_ccn_valid = FALSE;
      }

      if ( ncell->bcch_or_pmo == BCCH_FORMAT )
      {
        ncell->ncell_data.bcch.gprs.ccn_supported_valid = local_ccn_valid;
        ncell->ncell_data.bcch.gprs.ccn_supported = local_ccn_supported;
      }
      else
      {
        ncell->ncell_data.pmo_pcco.gprs_cell_selection_params.ccn_supported_valid = local_ccn_valid;
        ncell->ncell_data.pmo_pcco.gprs_cell_selection_params.ccn_supported = local_ccn_supported;
      }
    }
    else
    {
      MSG_GERAN_ERROR_2_G("CCN Support: missing ncell %d at index %d", ba_list->BCCH_ARFCN[index].num,index);
    }

    /* update variables for extraction of next bit */
    if ( bit_location == 7 )
    {
      bit_location = 0;
      array_byte_location++;
    }
    else
    {
      bit_location++;
    }
  }
}

/*!
 * \brief Processes a PSI14 messages if it is was a SI13 message.
 *
 * \param psi14_ptr (in)
 * \param cell_info (in)
 * \param gas_id (in)
 */
void rr_gprs_process_psi14_as_si13(
  packet_psi14_t *psi14_ptr,
  gprs_scell_info_t *cell_info,
  const gas_id_t gas_id
)
{
  si13_data_t *si13_data_ptr;
  rr_sys_info_si_data_t *rr_sys_info_si_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_si_data_ptr = rr_sys_info_si_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_si_data_ptr);

  si13_data_ptr = &rr_sys_info_si_data_ptr->decoded_si13_data;

  if (rr_gprs_convert_psi14_to_si13_data(
        psi14_ptr,       // psi14_ptr
        si13_data_ptr    // si13_ptr
      ))
  {
    cell_info->bcch_sys_info.spgc_ccch_sup =
      si13_data_ptr->bcch_pbcch.bcch.spgc_ccch_sup;

    cell_info->bcch_sys_info.priority_access_thr =
      si13_data_ptr->bcch_pbcch.bcch.priority_access_thr;

    cell_info->bcch_sys_info.nc_meas_params_si13.valid = TRUE;
    cell_info->bcch_sys_info.nc_meas_params_si13.network_control_order =
      si13_data_ptr->bcch_pbcch.bcch.network_control_order;

    cell_info->bcch_sys_info.gprs_cell_options =
      si13_data_ptr->bcch_pbcch.bcch.gprs_cell_options;

    cell_info->bcch_sys_info.power_control_params =
      si13_data_ptr->bcch_pbcch.bcch.power_control_params;

    cell_info->bcch_sys_info.non_drx_timer = rr_gprs_calc_non_drx_timer(
      si13_data_ptr->bcch_pbcch.bcch.gprs_cell_options.drx_timer_max,
      gas_id
    );

    cell_info->bcch_sys_info.sgsnr = si13_data_ptr->sgsnr;

    if (psi14_ptr->dtm_additions_flag == valid)
    {
      cell_info->bcch_sys_info.routing_area_code = si13_data_ptr->bcch_pbcch.bcch.rac;
      cell_info->bcch_sys_info.si_status_ind = si13_data_ptr->si_status_ind;
    }

    /* GPRS Mobile Allocation */
    if (si13_data_ptr->mobile_allocation_status == IE_PRESENT)
    {
      cell_info->mobile_allocations.si13_change_mark = si13_data_ptr->si13_change_mark;

      if (cell_info->gsm.valid_CA_received == RR_CA_RECEIVED)
      {
        rr_gprs_decode_mobile_alloc(
          NULL,
          &si13_data_ptr->mobile_allocation_ie,
          &cell_info->gsm.cell_channel_description,
          &si13_data_ptr->mobile_allocation
        );

        si13_data_ptr->mobile_allocation_status = IE_DECODED;
      }
      else
      {
        MSG_GERAN_HIGH_0_G("SI13 MA (from PSI14) requires CA");
      }
    }

    /* if a complete GPRS MA is available, store it */
    if (si13_data_ptr->mobile_allocation_status == IE_DECODED)
    {
      rr_gprs_store_mobile_alloc(
        MA_NUMBER_14,
        &si13_data_ptr->mobile_allocation,
        cell_info,
        gas_id
      );
    }

    /* PSI14 is treated as a substitute SI13, so mark the SI13 data as valid */
    cell_info->bcch_sys_info.si13_data_valid_flag = TRUE;

    /* mark SI13 and PSI14 as received */
    SET_BIT(MASK_SI_13,  cell_info->gsm.update_message_received);
  }

  return;
}

/*!
 * \brief Sets the 'ia_rejected' flag to indicate that a full sys info refresh is required.
 *
 * \param ia_rejected
 * \param gas_id
 */
void rr_gprs_notify_sys_info_ia_reject(
  boolean ia_rejected,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_sys_info_si_data_t *rr_sys_info_si_data_ptr = rr_sys_info_si_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_si_data_ptr);

  rr_sys_info_si_data_ptr->rr_sys_info_notified_of_ia_reject = ia_rejected;
}

/*!
 * \brief Requests L1 to read the BCCH sys info indicated by the bitmask.
 *
 *  Requests L1 to read the BCCH sys info indicated by the bitmask. If the bitmask is empty (all-zero),
 *  then L1 will stop reading BCCH sys info.
 *
 * \param si_required (in) - bitmask indicating SI required (1=yes)
 * \param gas_id (in)
 */
void rr_si_send_l1_read_sys_info_req(
  uint16 si_required,
  const gas_id_t gas_id
)
{
  rr_l1_si_type_T l1_si_required = RR_L1_SI_TYPE_NONE; 
  uint16 si_messages_on_bcch_ext = rr_si_messages_on_bcch_ext(rr_gprs_get_pcell_info(gas_id));

  // Remove any messages not being broadcast
  if (si_required != MASK_SI_NONE)
  {
    l1_si_required = map_rr_si_bitmask_to_l1_si_bitmask(si_required, si_messages_on_bcch_ext);
  }

  MSG_GERAN_MED_2_G("Asking for BCCH SI 0x%x (L1 mask 0x%x)",(int)si_required,(int)l1_si_required);
  
  rr_send_mph_read_sys_info_req(l1_si_required, gas_id);

} /* end rr_si_send_l1_read_sys_info_req() */

/*!
 * \brief Returns a bitmask indicating the SI messages which are not being broadcast.
 * 
 * \param cell_info_ptr (in)
 * 
 * \return uint16 - SI bitmask (1=not broadcast)
 */
uint16 rr_si_messages_not_broadcast(gprs_scell_info_t *cell_info_ptr)
{
  RR_NULL_CHECK_RETURN_PARAM(cell_info_ptr, MASK_SI_NONE);

  return cell_info_ptr->bcch_sys_info.si_messages_not_broadcast;
}

/*!
 * \brief Prints the SI bitmask to diag.
 * 
 * \param si_bitmask (in)
 */
void rr_si_print_bitmask(char *descr_text, uint16 si_bitmask)
{
//#ifdef PERLUTF
#if 1
  if (descr_text != NULL)
  {
    MSG_SPRINTF_1(
      MSG_SSID_DFLT,
      MSG_LEGACY_HIGH,
      "%s:",
      descr_text
    );
  }
  if (si_bitmask == MASK_SI_NONE)
  {
    MSG_GERAN_HIGH_0("  (none)");
  }
  else
  {
    if (si_bitmask & MASK_SI_1)
    {
      MSG_GERAN_HIGH_0("  SI1");
    }
    if (si_bitmask & MASK_SI_2)
    {
      MSG_GERAN_HIGH_0("  SI2");
    }
    if (si_bitmask & MASK_SI_2_BIS)
    {
      MSG_GERAN_HIGH_0("  SI2bis");
    }
    if (si_bitmask & MASK_SI_2_TER)
    {
      MSG_GERAN_HIGH_0("  SI2ter");
    }
    if (si_bitmask & MASK_SI_2_QUATER)
    {
      MSG_GERAN_HIGH_0("  SI2quater");
    }
    if (si_bitmask & MASK_SI_3)
    {
      MSG_GERAN_HIGH_0("  SI3");
    }
    if (si_bitmask & MASK_SI_4)
    {
      MSG_GERAN_HIGH_0("  SI4");
    }
    if (si_bitmask & MASK_SI_13)
    {
      MSG_GERAN_HIGH_0("  SI13");
    }
  }
#else
  NOTUSED(descr_text);
  NOTUSED(si_bitmask);
#endif // PERLUTF
}

/*!
 * \brief Calculates the time required to acquire to indicated SIs.
 * 
 * \param si_required (in)
 * \param si_on_bcch_ext (in)
 * 
 * \return uint32 - time required (millisecs)
 */
uint32 rr_si_calc_sys_info_acquisition_time(uint16 si_required, uint16 si_on_bcch_ext)
{
  uint32 time_required_ms = RR_GSM_SYS_INFO_TIMEOUT;

  // Extend the timeout if SI13 is required and is on BCCH-Norm
  if (((si_required    & MASK_SI_13) != 0) &&
      ((si_on_bcch_ext & MASK_SI_13) == 0))
  {
    time_required_ms += RR_GPRS_SYS_INFO_TIMEOUT;
  }

  return time_required_ms;
}

/* EOF */

