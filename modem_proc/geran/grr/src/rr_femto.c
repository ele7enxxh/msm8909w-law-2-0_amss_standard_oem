/*============================================================================
  rr_femto.c

  This file implements the RR-FEMTO module.
  See comments below for more detail.

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_femto.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/**
  @file rr_femto.c
  @brief This file implements the RR-FEMTO module.

  This module contains the functionality to support the RR Femto procedures. For
  a summary of these procedures, see the module header file.
  @see rr_femto.h

  This module makes use of the RR-SELECT-BCCH module to provide much of the
  functionality.
  @see rr_select_bcch_if.h

  The RR-FEMTO procedures are controlled by these RRC messages: \n
  RRC_RR_NBR_DISC_REQ \n
  RRC_RR_START_COSC_REQ \n
  RRC_RR_STOP_COSC_REQ \n \n

  This module uses RR IMSGs and OMSGs during the procedures. \n
  It does not use legacy RR events. \n \n

  The IMSGs which can be received by this module are: \n
  RR_FEMTO_IMSG_L1_ACTIVATED_IND \n
  RR_FEMTO_IMSG_L1_DEACTIVATED_IND
  @see rr_femto_imsg_e

  The OMSGs which can be output by this module are: \n
  RR_FEMTO_OMSG_ACTIVE_IND \n
  RR_FEMTO_OMSG_INACTIVE_IND
  @see rr_femto_omsg_e
 */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_FEMTO_GSM_NL

#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_femto_if.h"
#include "geran_msgs.h"
#include "ms.h"
#include "sys.h"
#include "sys_type.h"
#include "sys_stru.h"
#include "rrc_rr_types.h"
#include "rr_rrc_send.h"
#include "rr_mode.h"
#include "rr_gprs_debug.h"
#include "rr_pscan.h"
#include "rr_candidate_cell_db.h"
#include "rr_decode_bcch.h"
#include "rr_select_bcch.h"
#include "rr_sys_info_pcell.h"
#include "rr_general.h"
#include "rr_l1_send.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "rr_lsm_if.h"
#include "stringl/stringl.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define RR_FEMTO_SNR_START_COUNT  3

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
  @union rrc_rr_femto_msg_u
  @brief Union of RRC messages which can start a procedure

  The purpose of this union is to store the RRC message which started a
  procedure. Only one procedure can be started at one time so it can be a union.
 */
typedef union
{
  IMH_T                       header;
  rrc_rr_nbr_disc_req_type    nbr_disc_req;
  rrc_rr_start_cosc_req_type  start_cosc_req;
} rrc_rr_femto_msg_u;

/**
  @enum rr_femto_state_e
  @brief The states of the rr_femto_control state machine

  RR_FEMTO_STATE_NULL \n
  In this state the module is inactive. It is waiting for a RRC message to
  trigger a procedure. \n \n

  RR_FEMTO_STATE_ACTIVATING \n
  L1 has been activated due to a message received from RRC. Waiting for
  confirmation from L1. \n \n

  RR_FEMTO_STATE_DEACTIVATING \n
  L1 has been deactivated due to the completion of a procedure. Waiting for
  confirmation from L1. When confirmation is received, RR sends confirmation to
  RRC. Because this is a generic L1 deactivation state, the reason for the L1
  deactivation cannot be derived from this state - this is stored in
  rr_femto_data.rrc_msg_id_rsp_after_deact before deactivating. \n \n

  RR_FEMTO_STATE_DISC_PSCAN \n
  This is the start state of the Neighbour Discovery procedure. \n
  A power-scan is in progress. Waiting for the results. \n \n

  RR_FEMTO_STATE_DISC_DECODE_BCCH \n
  Parallel BCCH decodes are now being performed on the cells found in the
  power-scan. \n
  The RR-DECODE-BCCH module is used to provide this functionality. \n \n

  RR_FEMTO_STATE_DISC_SELECT_BCCH \n
  For each cell where SI3/SI4 was received during the parallel BCCH decodes, the
  cell is selected and SI3 and SI13 received. SI3 is required to obtain the Cell
  Identity (SI4 does not contain this), and SI13 is requuired to obtain the RAC.
  The RR-SELECT-BCCH module is used to select the cell and read the required
  system information. \n
  Once completed, the results are stored and L1 is deactivated. The results are
  sent to RRC once L1 deactivation has be confirmed. \n \n

  RR_FEMTO_STATE_CAMP_SELECT_BCCH \n
  This is the start state of the Camp-On-Specific-Cell procedure. \n
  The RR-SELECT-BCCH module is used to select the given cell and early-camp onto
  it. Once early-camped, confirmation is sent to RRC. \n \n

  RR_FEMTO_STATE_CAMP_CAMPED \n
  RR/L1 remain early-camped on the cell until requested to un-camp by RRC, with
  a RRC_STOP_COSC_REQ message. RR deactivates L1, and confirms to RRC once L1
  deactivation has been confirmed.
 */
typedef enum
{
  RR_FEMTO_STATE_NULL,
  RR_FEMTO_STATE_ACTIVATING,
  RR_FEMTO_STATE_DEACTIVATING,
  RR_FEMTO_STATE_DISC_PSCAN,
  RR_FEMTO_STATE_DISC_DECODE_BCCH,
  RR_FEMTO_STATE_DISC_SELECT_BCCH,
  RR_FEMTO_STATE_CAMP_SELECT_BCCH,
  RR_FEMTO_STATE_CAMP_CAMPED,
  RR_FEMTO_STATE_MAX
} rr_femto_state_e;

/**
  @enum rr_femto_camp_status_e
  @brief Used to record whether the cell could be camped onto.

  This is used when constructing either a RR_RRC_START_COSC_REQ or
  RR_RRC_SERVICE_LOST message to send to RRC
 */
typedef enum
{
  RR_FEMTO_CAMP_STATUS_CAMPED,
  RR_FEMTO_CAMP_STATUS_FAILED,
  RR_FEMTO_CAMP_STATUS_SERVICE_LOST
} rr_femto_camp_status_e;

/**
  @struct rr_femto_data_t
  @brief Holds the module local data
 */
typedef struct
{
  rr_femto_state_e             state;
  rr_femto_state_e             old_state;
  rrc_rr_femto_msg_u           rrc_msg;
  struct
  {
    rr_candidate_cell_T               *curr_candidate_cell_ptr;
    uint8                              num_cells;
    sys_detailed_gsm_list_info_s_type  cell_list[SYS_MAX_GSM_NBRS];
  }                            disc;
  struct
  {
    rr_femto_camp_status_e             status;
    uint8                              snr_counter;
  }                            camp;
  rrc_rr_cmd_e_type            rrc_msg_id_rsp_after_deact;
} rr_femto_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_femto_data_t  rr_femto_data;


/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/


/**
  @brief Returns a string representing a given rr_femto_state_e value

  @param state

  @return const char*
 */
static const char *rr_femto_state_name(rr_femto_state_e state)
{
  switch (state)
  {
    case RR_FEMTO_STATE_NULL:                         return "NULL";
    case RR_FEMTO_STATE_ACTIVATING:                   return "ACTIVATING";
    case RR_FEMTO_STATE_DEACTIVATING:                 return "DEACTIVATING";
    case RR_FEMTO_STATE_DISC_PSCAN:                   return "DISC_PSCAN";
    case RR_FEMTO_STATE_DISC_DECODE_BCCH:             return "DISC_DECODE_BCCH";
    case RR_FEMTO_STATE_DISC_SELECT_BCCH:             return "DISC_SELECT_BCCH";
    case RR_FEMTO_STATE_CAMP_SELECT_BCCH:             return "CAMP_SELECT_BCCH";
    case RR_FEMTO_STATE_CAMP_CAMPED:                  return "CAMP_CAMPED";

    default:
      MSG_GERAN_ERROR_1_G("Unknown state: %d", state);
      return "?";
  }

} /* rr_femto_state_name */


/**
  @brief Deactivates L1 via a request to rr_mode_control.

  Requests rr_mode_control to deactivate L1. If L1 is already deactivated then
  RR_FEMTO_IMSG_L1_DEACTIVATED_IND will be sent immediately
*/
static void rr_femto_deactivate_l1(void)
{
  rr_event_T event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL);

  if (event == EV_GSM_MODE_DEACTIVATED)
  {
    rr_femto_send_imsg_l1_deactivated_ind();
  }

} /* rr_femto_deactivate_l1 */


/**
  @brief Activates L1 via a request to rr_mode_control.

  Requests rr_mode_control to activate L1. If L1 is already activated then
  RR_FEMTO_IMSG_L1_ACTIVATED_IND will be sent immediately
*/
static void rr_femto_activate_l1(void)
{
  rr_event_T event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL);

  if (event == EV_GSM_MODE_ACTIVATED)
  {
    rr_femto_send_imsg_l1_activated_ind();
  }

} /* rr_femto_activate_l1 */


/**
  @brief Stores the initial message from RRC which causes L1 to be activated.

  When L1 confirms activation, this message is examined to determine which
  procedure to perform

  @param msg_ptr - message from RRC
 */
static void rr_femto_store_rrc_msg(rrc_rr_femto_msg_u *msg_ptr)
{
  uint8  msg_len;
  IMH_T *msg_header;

  RR_NULL_CHECK_FATAL(msg_ptr);

  msg_header = (IMH_T *)&msg_ptr->header;
  GET_IMH_LEN(msg_len, msg_header);
  memscpy(&rr_femto_data.rrc_msg,sizeof(rr_femto_data.rrc_msg),(void *)msg_ptr, msg_len + sizeof(IMH_T));  


} /* rr_femto_store_rrc_msg */


/**
  @brief Starts a foreground power-scan using the given band preference

  @param band_pref
 */
static void rr_femto_disc_start_pscan(uint32 band_pref)
{
  rr_pscan_setup();
  rr_pscan_send_start_ind(RR_SEARCH_NORMAL, band_pref);

  rr_femto_data.disc.num_cells = 0;

} /* rr_femto_disc_start_pscan */


/**
  @brief This function starts parallel BCCH decodes on the cells currently in
  the candidate cell database

  @return boolean - TRUE if there are any cells to process; FALSE otherwise
 */
static boolean rr_femto_disc_start_bcch_decodes(void)
{
  boolean any_freqs_found;

  rr_decode_bcch_list_setup();

  any_freqs_found = rr_candidate_db_send_decode_bcch_start_ind(RR_SEARCH_NORMAL, FALSE);

  return any_freqs_found;

} /* rr_femto_disc_start_bcch_decodes */


/**
  @brief This function is called when L1 indicates the parallel BCCH decodes are
  completed. Any cells where BCCH decoding failed are marked as invalid.

  @return uint16 - The number of valid cells
 */
static uint16 rr_femto_disc_process_bcch_decodes(void)
{
  rr_candidate_cell_db_T *candidate_db_ptr = rr_get_candidate_cell_db_ptr();

  RR_NULL_CHECK_FATAL(candidate_db_ptr);

  // Discard any entries with BCCH decode failures
  rr_candidate_db_mark_invalid_entries_as_processed(candidate_db_ptr);

  // Return the number of cells to be processed
  return rr_candidate_db_get_num_unprocessed_cell(candidate_db_ptr);

} /* rr_femto_disc_process_bcch_decodes */


/**
  @brief Selects the next unprocessed cell from the candidate cell database, and
  requests rr_select_bcch_control to select this cell and read SI3 + SI13

  @return boolean - TRUE if a cell was found; FALSE otherwise
 */
static boolean rr_femto_disc_select_next_cell(void)
{
  boolean cell_found;
  rr_candidate_cell_T *candidate_cell_ptr;
  rr_candidate_cell_db_T *candidate_db_ptr = rr_get_candidate_cell_db_ptr();

  RR_NULL_CHECK_FATAL(rr_get_candidate_cell_db_ptr);

  // Find the first unprocessed cell
  cell_found = rr_candidate_db_find_next(
    candidate_db_ptr,
    &candidate_cell_ptr,
    RR_CANDIDATE_STRONGEST_FIRST
  );

  if (cell_found && candidate_cell_ptr)
  {
    // Request RR-SELECT-BCCH module to select the cell, and wait for SI3 + SI13
    // Note: When SI3 is received, if the cell doesn't support GPRS, then
    // RR-SELECT-BCCH won't wait for SI13, and will finish
    rr_select_bcch_send_imsg_select_req(
      candidate_cell_ptr->meas.arfcn,
      candidate_cell_ptr->meas.RXLEV_average,
      NULL,
      RR_SELECT_BCCH_EARLY_CAMP_NONE,
      MASK_SI_NONE,
      MASK_SI_3 | MASK_SI_13
    );

    rr_femto_data.disc.curr_candidate_cell_ptr = candidate_cell_ptr;
  }
  else
  {
    rr_femto_data.disc.curr_candidate_cell_ptr = NULL;
  }

  return cell_found;

} /* rr_femto_disc_select_next_cell */


/**
  @brief Converts nas type plmn_id to rrc type plmn id

  @return sys_plmn_identity_s_type  structure
 */
static sys_plmn_identity_s_type rr_convert_nas_to_rrc_plmn_id( sys_plmn_id_s_type plmn_id )
{
   sys_plmn_identity_s_type rrc_plmn_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* ----------------------------------------
   ** PLMN octet 1 = MCC digit 2 | MCC digit 1
   ** ---------------------------------------- */
   rrc_plmn_id.mcc[0] = plmn_id.identity[0] & 0x0F;
   rrc_plmn_id.mcc[1] = (plmn_id.identity[0] & 0xF0) >> 4;

   /* ----------------------------------------
   ** PLMN octet 2 = MNC digit 3 | MCC digit 3
   ** ---------------------------------------- */
   rrc_plmn_id.mcc[2] = plmn_id.identity[1] & 0x0F;

   if ((plmn_id.identity[1] & 0xF0) == 0xF0)
   {
      rrc_plmn_id.num_mnc_digits = 2;
      rrc_plmn_id.mnc[2] = 0x0F; /* Set the 3rd digit to 0xF as per the spec */
   }
   else
   {
      rrc_plmn_id.num_mnc_digits = 3;
      rrc_plmn_id.mnc[2] = (plmn_id.identity[1] & 0xF0) >> 4;
   }

   /* ----------------------------------------
   ** PLMN octet 3 = MNC digit 2 | MNC digit 1
   ** ---------------------------------------- */
   rrc_plmn_id.mnc[0] = plmn_id.identity[2] & 0x0F;
   rrc_plmn_id.mnc[1] = (plmn_id.identity[2] & 0xF0) >> 4;

   return rrc_plmn_id;

} /* end rr_convert_nas_to_rrc_plmn_id() */


/**
  @brief Called when all the requested SIs have been read for a cell.

  The required SI data is stored and this cell is marked as processed in the
  candidate cell database.
*/
static void rr_femto_disc_all_bsi_received(void)
{
  sys_detailed_gsm_list_info_s_type *cell_info_ptr;
  gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info();

  RR_NULL_CHECK_FATAL(pcell_info_ptr);

  if (rr_femto_data.disc.num_cells > SYS_MAX_GSM_NBRS)
  {
    rr_femto_data.disc.num_cells = SYS_MAX_GSM_NBRS;
  }

  cell_info_ptr = &rr_femto_data.disc.cell_list[rr_femto_data.disc.num_cells];

  // Copy all the required SI data from the pcell into the cell info list
  cell_info_ptr->arfcn = pcell_info_ptr->gsm.BCCH_ARFCN.num;
  cell_info_ptr->plmn_id = rr_convert_nas_to_rrc_plmn_id(pcell_info_ptr->gsm.location_area_identification.plmn_id);
  cell_info_ptr->cell_id = (uint32)pcell_info_ptr->gsm.cell_identity;
  cell_info_ptr->lac[0] = (pcell_info_ptr->gsm.location_area_identification.location_area_code & 0xFF00) >> 8;
  cell_info_ptr->lac[1] = pcell_info_ptr->gsm.location_area_identification.location_area_code & 0x00FF;

  if (pcell_info_ptr->gsm.gprs_indicator)
  {
    cell_info_ptr->rac_valid = TRUE;
    cell_info_ptr->rac = pcell_info_ptr->bcch_sys_info.routing_area_code;
  }
  else
  {
    cell_info_ptr->rac_valid = FALSE;
    cell_info_ptr->rac = 0;
  }

  cell_info_ptr->signal_strength_rssi = rxlev_to_dbm(pcell_info_ptr->gsm.reselection_info.RXLEV_average);

  // Increment the cell count
  rr_femto_data.disc.num_cells++;

  // Mark this candidate cell as processed, so the next unprocessed one can be selected
  rr_femto_data.disc.curr_candidate_cell_ptr->processed = TRUE;

} /* rr_femto_disc_check_all_bsi_received */


/**
  @brief Called when a decode error is detected for a cell.

  This cell is then marked as invalid in the candidate cell database.
*/
static void rr_femto_disc_discard_curr_cell(void)
{
  rr_femto_data.disc.curr_candidate_cell_ptr->type = RR_CANDIDATE_BCCH_WITH_DECODE_FAILURES;
  rr_femto_data.disc.curr_candidate_cell_ptr->processed = TRUE;

} /* rr_femto_disc_discard_curr_cell */


/**
  @brief Called when the Camp-On-Specific-Cell procedure is started.

  This function performs initial setup, and then RR-SELECT-BCCH is requested to
  select the requested cell (including reselection power-scan)

  @param arfcn_num
 */
static void rr_femto_camp_start_select_bcch(uint16 arfcn_num)
{
  ARFCN_T arfcn;

  arfcn.num = arfcn_num;
  arfcn.band = (sys_band_T) rr_get_band(arfcn_num);

  MSG_GERAN_HIGH_2_G("Look for cell arfcn=%d band=%d",(int)arfcn.num, (int)arfcn.band);

  rr_femto_data.camp.snr_counter = RR_FEMTO_SNR_START_COUNT;

  rr_select_bcch_send_imsg_pscan_select_req(
    arfcn,
    NULL,
    MASK_SI_NONE,
    MASK_SI_3
  );

} /* rr_femto_camp_start_select_bcch */


/**
  @brief Processes a MPH_SERVING_IDLE_MEAS_IND from L1 and decides if the
  serving has become too bad

  @param idle_meas_ind

  @return boolean - TRUE if the serving cell is bad; FALSE otherwise
 */
static boolean rr_femto_camp_is_scell_bad(mph_serving_idle_meas_ind_T *idle_meas_ind)
{
  boolean scell_is_bad = FALSE;

  if (idle_meas_ind->SNR_is_bad)
  {
    if (rr_femto_data.camp.snr_counter == 0)
    {
      scell_is_bad = TRUE;
    }
    else
    {
      rr_femto_data.camp.snr_counter--;
    }
  }
  else
  {
    rr_femto_data.camp.snr_counter = RR_FEMTO_SNR_START_COUNT;
  }

  return scell_is_bad;

} /* rr_femto_camp_is_scell_bad */


/**
  @brief Called after L1 is deactivated to send a response to RRC.

  An appropriate response is sent to RRC, with the results of the procedure, a
  failure indication, etc.
*/
static void rr_femto_respond_to_rrc_after_deact(void)
{
  switch (rr_femto_data.rrc_msg.header.message_id)
  {
    case RRC_RR_NBR_DISC_REQ:
    {
      rr_send_rrc_nbr_disc_cnf(
        rr_femto_data.disc.num_cells,
        rr_femto_data.disc.cell_list
      );
      break;
    }

    case RRC_RR_START_COSC_REQ:
    {
      switch (rr_femto_data.camp.status)
      {
        // The expected case, where RR/L1 were able to camp onto the specified cell
        // In this case, the reason for deactivation would be by request from RRC, i.e.
        // RRC_RR_STOP_COSC_REQ was received
        case RR_FEMTO_CAMP_STATUS_CAMPED:
        {
          rr_send_rrc_stop_cosc_cnf();
          break;
        }

        // Here, RR/L1 were not able to camp onto the specified cell (e.g. cell not found during
        // pscan, or SI timeout)
        // In this case, RR autonomously deactivates L1, then responds to RRC
        case RR_FEMTO_CAMP_STATUS_FAILED:
        {
          uint16 dummy_cell_id = 0;
          BSIC_T dummy_bsic = {0};

          rr_send_rrc_start_cosc_cnf(
            FALSE,
            dummy_cell_id,
            dummy_bsic
          );
          break;
        }

        // Here, after initially camping successfully on the cell, the cell is lost due to
        // continual bad SNR.
        // In this case, RR autonomously deactivates L1, then responds to RRC
        case RR_FEMTO_CAMP_STATUS_SERVICE_LOST:
        {
          rr_send_rrc_service_lost_ind();
          break;
        }


        default:
        {
          MSG_GERAN_ERROR_1_G("Unexpected value for status (%d)",(int)rr_femto_data.camp.status);
        }
      }
      break;
    }

    default:
    {
      MSG_GERAN_ERROR_1_G("Unknown RRC message (%d)",(int)rr_femto_data.rrc_msg.header.message_id);
    }
  }

} /* rr_femto_respond_to_rrc_after_deact */


/*-----------------------------------------------------------------------------
 * Externalized Function Definitions
 * --------------------------------------------------------------------------*/


/**
  @brief Initialisation function. This sets the initial state of the state
  machine
*/
void rr_femto_init(void)
{
  rr_femto_data.old_state = RR_FEMTO_STATE_NULL;
  rr_femto_data.state = RR_FEMTO_STATE_NULL;

} /* end rr_femto_init() */


/**
  @brief This is the main RR-FEMTO state machine

  @param msg_ptr
 */
void rr_femto_control(rr_cmd_bdy_type *msg_ptr)
{
  RR_NULL_CHECK_FATAL(msg_ptr);

#ifdef DEBUG_RR_TRACE_MSG
  rr_store_trace_msg_buf(
    RR_FEMTO_SM,
    EV_INPUT_MESSAGE,
    (byte) rr_femto_data.state,
    msg_ptr
  );
#endif

  switch (rr_femto_data.state)
  {
    /*
      RR_FEMTO_STATE_NULL
      In this state the module is inactive. It is waiting for a RRC message to
      trigger a procedure.
    */
    case RR_FEMTO_STATE_NULL:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RRC_RR:
        {
          switch (msg_ptr->message_header.message_id)
          {
            // Messages from RRC which can start a procedure
            case RRC_RR_NBR_DISC_REQ:
            case RRC_RR_START_COSC_REQ:
            {
              // Store the RRC message for processing after GL1 activated
              rr_femto_store_rrc_msg((rrc_rr_femto_msg_u *)msg_ptr);

              // Move RR main state to RR_FEMTO - all further messages will then be routed
              // into rr_femto_control
              rr_femto_send_omsg_active_ind();

              // Activate L1
              rr_femto_activate_l1();

              // Wait for activation confirmation
              rr_femto_data.state = RR_FEMTO_STATE_ACTIVATING;

              break;
            }

            // During the CAMP procedure, if the given cell could not be found, or service
            // was subsequently lost, then RR will autonomously deactivate L1 and report
            // this result to RRC. RRC is then expected to request the procedure be stopped.
            case RRC_RR_STOP_COSC_REQ:
            {
              rr_send_rrc_stop_cosc_cnf();
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_femto_state_name(rr_femto_data.state)
              );
            }
          }
          break;
        } /* MS_RRC_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_femto_state_name(rr_femto_data.state)
          );
        }
      }
      break;
    } /* RR_FEMTO_STATE_NULL */

    /*
      RR_FEMTO_STATE_ACTIVATING
      L1 has been activated due to a message received from RRC. Waiting for
      confirmation from L1.
    */
    case RR_FEMTO_STATE_ACTIVATING:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_FEMTO_IMSG:
            {
              rr_femto_imsg_u *imsg_ptr;
              imsg_ptr = &msg_ptr->rr.femto.imsg;

              switch (imsg_ptr->header.imh.message_id)
              {
                // L1 has confirmed activation
                case RR_FEMTO_IMSG_L1_ACTIVATED_IND:
                {
                  // Now examine which RRC message trigger this activation, to know what procedure to perform
                  switch (rr_femto_data.rrc_msg.header.message_id)
                  {
                    // Neighbour Discovery
                    case RRC_RR_NBR_DISC_REQ:
                    {
                      rrc_rr_nbr_disc_req_type *nbr_disc_req_ptr = &rr_femto_data.rrc_msg.nbr_disc_req;

                      rr_femto_disc_start_pscan(nbr_disc_req_ptr->pref_bands);

                      // Wait for power-scan results
                      rr_femto_data.state = RR_FEMTO_STATE_DISC_PSCAN;
                      break;
                    }

                    // Camp-On-Specific-Cell
                    case RRC_RR_START_COSC_REQ:
                    {
                      rrc_rr_start_cosc_req_type *start_cosc_req_ptr = &rr_femto_data.rrc_msg.start_cosc_req;

                      // Request RR-SELECT-BCCH module to select the cell
                      rr_femto_camp_start_select_bcch(start_cosc_req_ptr->arfcn);

                      // Wait for results from RR_SELECT_BCCH module
                      rr_femto_data.state = RR_FEMTO_STATE_CAMP_SELECT_BCCH;
                      break;
                    }

                    default:
                    {
                      MSG_GERAN_ERROR_0_G("Unhandled RRC cmd");
                    }
                  }
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_femto_state_name(rr_femto_data.state)
                  );
                }
              }
              break;
            } /* RR_FEMTO_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_femto_state_name(rr_femto_data.state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_femto_state_name(rr_femto_data.state)
          );
        }
      }
      break;
    } /* RR_FEMTO_STATE_ACTIVATING */

    /*
      RR_FEMTO_STATE_DEACTIVATING
      L1 has been deactivated due to the completion of a procedure. Waiting for
      confirmation from L1.
      When confirmation is received, RR sends confirmation to RRC. Because this
      is a generic L1 deactivation state, the reason for the L1 deactivation -
      and hence the correct response to RRC - cannot be derived from this state.
      This is stored in rr_femto_data.rrc_msg_id_rsp_after_deact before
      deactivating.
    */
    case RR_FEMTO_STATE_DEACTIVATING:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_FEMTO_IMSG:
            {
              switch (msg_ptr->rr.femto.imsg.header.imh.message_id)
              {
                // L1 has confirmed deactivation
                case RR_FEMTO_IMSG_L1_DEACTIVATED_IND:
                {
                  // Send results appropriate to original request to RRC
                  rr_femto_respond_to_rrc_after_deact();

                  // Finished
                  rr_femto_send_omsg_inactive_ind();
                  rr_femto_data.state = RR_FEMTO_STATE_NULL;
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_femto_state_name(rr_femto_data.state)
                  );
                }
              }
              break;
            } /* RR_FEMTO_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_femto_state_name(rr_femto_data.state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_femto_state_name(rr_femto_data.state)
          );
        }
      }
      break;
    } /* RR_FEMTO_STATE_DISC_DEACTIVATING */

    /*
      RR_FEMTO_STATE_DISC_PSCAN
      This is the start state of the Neighbour Discovery procedure.
      A power-scan is in progress. Waiting for the results
    */
    case RR_FEMTO_STATE_DISC_PSCAN:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_PSCAN_OMSG:
            {
              switch (msg_ptr->rr.pscan.omsg.header.imh.message_id)
              {
                case RR_PSCAN_OMSG_FINISHED_IND:
                {
                  uint16 num_freqs_found = rr_pscan_process_power_scan_results(RR_NORMAL_SEARCH_ORDER);

                  if (num_freqs_found > 0)
                  {
                    // Start parallel BCCH decodes
                    (void) rr_femto_disc_start_bcch_decodes();

                    // Wait for BSI or read failure indication
                    rr_femto_data.state = RR_FEMTO_STATE_DISC_DECODE_BCCH;
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("No freqs found during pscan");

                    // Finished - deactivate L1
                    rr_femto_deactivate_l1();
                    rr_femto_data.state = RR_FEMTO_STATE_DEACTIVATING;
                  }
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_femto_state_name(rr_femto_data.state)
                  );
                }
              }
              break;
            } /* RR_PSCAN_OMSG */

            case RR_PSCAN_IMSG:
            {
              rr_pscan_control(EV_INPUT_MESSAGE, msg_ptr);
              break;
            } /*RR_PSCAN_IMSG*/

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_femto_state_name(rr_femto_data.state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_RR_L1:
        {
          rr_pscan_control(EV_INPUT_MESSAGE, msg_ptr);
          break;
        }

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_femto_state_name(rr_femto_data.state)
          );
        }
      }
      break;
    } /* RR_FEMTO_STATE_DISC_PSCAN */

    /*
      RR_FEMTO_STATE_DISC_DECODE_BCCH
      Parallel BCCH decodes are now being performed on the cells found in the
      power-scan.
      The RR-DECODE-BCCH module is used to provide this functionality.
    */
    case RR_FEMTO_STATE_DISC_DECODE_BCCH:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_DECODE_BCCH_OMSG:
            {
              switch (msg_ptr->rr.decode_bcch.imsg.header.imh.message_id)
              {
                case RR_DECODE_BCCH_OMSG_FINISHED_IND:
                {
                  uint16 num_cells = rr_femto_disc_process_bcch_decodes();

                  if (num_cells > 0)
                  {
                    MSG_GERAN_HIGH_1_G("Cells found: %d",(int)num_cells);

                    (void) rr_femto_disc_select_next_cell();
                    rr_femto_data.state = RR_FEMTO_STATE_DISC_SELECT_BCCH;
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("No valid cells found");

                    // Finished - deactivate L1
                    rr_femto_deactivate_l1();
                    rr_femto_data.state = RR_FEMTO_STATE_DEACTIVATING;
                  }
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_femto_state_name(rr_femto_data.state)
                  );
                }
              }
              break;
            } /* RR_FEMTO_IMSG */

            case RR_DECODE_BCCH_IMSG:
            {
              rr_decode_bcch_list_control(EV_INPUT_MESSAGE, msg_ptr);
              break;
            } /* RR_DECODE_BCCH_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_femto_state_name(rr_femto_data.state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_RR_L1:
        case MS_TIMER:
        {
          rr_decode_bcch_list_control(EV_INPUT_MESSAGE, msg_ptr);
          break;
        }

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_femto_state_name(rr_femto_data.state)
          );
        }
      }
      break;
    } /* RR_FEMTO_STATE_DISC_DECODE_BCCH */

    /*
      RR_FEMTO_STATE_DISC_SELECT_BCCH
      For each cell where SI3/SI4 was received during the parallel BCCH decodes,
      the cell is selected and SI3 and SI13 received. SI3 is required to obtain
      the Cell Identity (SI4 does not contain this), and SI13 is required to
      obtain the RAC.
      The RR-SELECT-BCCH module is used to select the cell and read the required
      system information.
      Once completed, the stored and L1 is deactivated. The results are sent to
      RRC once L1 deactivation has be confirmed
    */
    case RR_FEMTO_STATE_DISC_SELECT_BCCH:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_SELECT_BCCH_OMSG:
            {
              switch (msg_ptr->rr.select_bcch.imsg.header.imh.message_id)
              {
                case RR_SELECT_BCCH_OMSG_SELECT_CNF:
                {
                  rr_femto_disc_all_bsi_received();

                  if (rr_femto_disc_select_next_cell())
                  {
                    MSG_GERAN_HIGH_0_G("All SI received for this cell - select next cell");
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("All SI received for this cell - no more cells");

                    // Finished - deactivate L1
                    rr_femto_deactivate_l1();
                    rr_femto_data.state = RR_FEMTO_STATE_DEACTIVATING;
                  }
                  break;
                }

                case RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND:
                {
                  rr_femto_disc_discard_curr_cell();

                  if (rr_femto_disc_select_next_cell())
                  {
                    MSG_GERAN_HIGH_0_G("L1 couldn't sync to cell - select next cell");
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("L1 couldn't sync to cell - no more cells");

                    // Finished - deactivate L1
                    rr_femto_deactivate_l1();
                    rr_femto_data.state = RR_FEMTO_STATE_DEACTIVATING;
                  }
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_femto_state_name(rr_femto_data.state)
                  );
                }
              }
              break;
            }

            case RR_SELECT_BCCH_IMSG:
            {
              rr_select_bcch_control(msg_ptr);
              break;
            }

            case RR_DECODE_BCCH_IMSG:
            case RR_CELL_ACQ_IMSG:
            {
              break;   // Ignore - processing not required
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_femto_state_name(rr_femto_data.state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_RR_L1:
        case MS_TIMER:
        {
          rr_select_bcch_control(msg_ptr);
          break;
        }

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_femto_state_name(rr_femto_data.state)
          );
        }
      }
      break;
    } /* RR_FEMTO_STATE_DISC_SELECT_BCCH */

    /*
      RR_FEMTO_STATE_CAMP_SELECT_BCCH
      This is the start state of the Camp-On-Specific-Cell procedure.
      The RR-SELECT-BCCH module is used to select the given cell and early-camp
      onto it.
      Once early-camped, confirmation is sent to RRC.
    */
    case RR_FEMTO_STATE_CAMP_SELECT_BCCH:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_SELECT_BCCH_OMSG:
            {
              switch (msg_ptr->rr.select_bcch.omsg.header.imh.message_id)
              {
                case RR_SELECT_BCCH_OMSG_SELECT_CNF:
                {
                  gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info();

                  RR_NULL_CHECK_FATAL(pcell_info_ptr);

                  // Set the status - this determines the response to RRC after L1 has deactivated
                  // (following RRC_RR_STOP_COSC_REQ)
                  rr_femto_data.camp.status = RR_FEMTO_CAMP_STATUS_CAMPED;

                  rr_send_rrc_start_cosc_cnf(
                    TRUE,
                    pcell_info_ptr->gsm.cell_identity,
                    pcell_info_ptr->gsm.BSIC
                  );

                  rr_femto_data.state = RR_FEMTO_STATE_CAMP_CAMPED;
                  break;
                }

                case RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND:
                {
                  // Set the status - this determines the response to RRC after L1 has deactivated
                  rr_femto_data.camp.status = RR_FEMTO_CAMP_STATUS_FAILED;

                  rr_femto_deactivate_l1();
                  rr_femto_data.state = RR_FEMTO_STATE_DEACTIVATING;
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    msg_ptr,
                    rr_femto_state_name(rr_femto_data.state)
                  );
                }
              }
              break;
            }

            case RR_SELECT_BCCH_IMSG:
            case RR_PSCAN_OMSG:
            case RR_PSCAN_IMSG:
            {
              rr_select_bcch_control(msg_ptr);
              break;
            }

            case RR_DECODE_BCCH_IMSG:
            case RR_CELL_ACQ_IMSG:
            {
              break;   // Ignore - processing not required
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_femto_state_name(rr_femto_data.state)
              );
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_RR_L1:
        case MS_TIMER:
        {
          rr_select_bcch_control(msg_ptr);
          break;
        }

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_femto_state_name(rr_femto_data.state)
          );
        }
      }
      break;
    } /* RR_FEMTO_STATE_CAMP_SELECT_BCCH */

    /*
      RR_FEMTO_STATE_CAMP_CAMPED
      RR/L1 remain early-camped on the cell until requested to un-camp by RRC,
      with a RRC_STOP_COSC_REQ message. RR deactivates L1, and confirms to RRC
      once L1 deactivation has been confirmed.
    */
    case RR_FEMTO_STATE_CAMP_CAMPED:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          break;   // Ignore - processing not required
        } /* MS_RR_RR */

        case MS_RR_L1:
        {
          switch (msg_ptr->message_header.message_id)
          {
            case MPH_SERVING_IDLE_MEAS_IND:
            {
              mph_serving_idle_meas_ind_T *idle_meas_ind = &msg_ptr->mph_serving_idle_meas_ind;

              if (rr_femto_camp_is_scell_bad(idle_meas_ind))
              {
                // Set the status - this determines the response to RRC after L1 has deactivated
                rr_femto_data.camp.status = RR_FEMTO_CAMP_STATUS_SERVICE_LOST;

                rr_femto_deactivate_l1();

                rr_femto_data.state = RR_FEMTO_STATE_DEACTIVATING;
              }
              break;
            }

            case MPH_START_IDLE_MODE_CNF:
            {
              break;     // sent by L1 in response to MPH_EARLY_CAMPING_REQ
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_femto_state_name(rr_femto_data.state)
              );
            }
          }
          break;
        } /* MS_RR_L1 */

        case MS_RRC_RR:
        {
          switch (msg_ptr->message_header.message_id)
          {
            case RRC_RR_STOP_COSC_REQ:
            {
              rr_femto_deactivate_l1();

              rr_femto_data.state = RR_FEMTO_STATE_DEACTIVATING;
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                msg_ptr,
                rr_femto_state_name(rr_femto_data.state)
              );
            }
          }
          break;
        } /* MS_RRC_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(
            msg_ptr,
            rr_femto_state_name(rr_femto_data.state)
          );
        }
      }
      break;
    } /* RR_FEMTO_STATE_CAMP_CAMPED */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unknown state: %d", rr_femto_data.state);
    }
  }

  if (rr_femto_data.old_state != rr_femto_data.state)
  {
    RR_LOG_STATE_TRANSITION(
      "rr_femto_control",
      rr_femto_data.old_state,
      rr_femto_data.state,
      rr_femto_state_name
    );
    rr_femto_data.old_state = rr_femto_data.state;
  }

} /* rr_femto_control */


/**
  @brief Returns a character string representing the given RR_FEMOTO_IMSG or
  RR_FEMTO_OMSG

  @param msg_ptr
  @param message_set (not used)
  @param message_id (not used)

  @return char*
 */
char *rr_femto_message_name
(
  const rr_cmd_bdy_type *msg_ptr,
  const int message_set,
  const int message_id
)
{
  char * s = "?";

  if (msg_ptr != NULL)
  {
    switch (msg_ptr->rr.header.rr_message_set)
    {
      case RR_FEMTO_IMSG:
      {
        switch (msg_ptr->rr.femto.imsg.header.imh.message_id)
        {
          case RR_FEMTO_IMSG_L1_ACTIVATED_IND:
          {
            s = "RR_FEMTO_IMSG_L1_ACTIVATED_IND";
            break;
          }

          case RR_FEMTO_IMSG_L1_DEACTIVATED_IND:
          {
            s = "RR_FEMTO_IMSG_L1_DEACTIVATED_IND";
            break;
          }

          default:
          {
            MSG_GERAN_ERROR_1_G("Unknown RR_FEMTO_IMSG: 0x%02x", msg_ptr->rr.femto.imsg.header.imh.message_id);
            s = "?";
          }
        }
        break;
      } /* RR_FEMTO_IMSG */

      case RR_FEMTO_OMSG:
      {
        switch (msg_ptr->rr.femto.omsg.header.imh.message_id)
        {
          case RR_FEMTO_OMSG_ACTIVE_IND:
          {
            s = "RR_FEMTO_OMSG_ACTIVE_IND";
            break;
          }

          case RR_FEMTO_OMSG_INACTIVE_IND:
          {
            s = "RR_FEMTO_OMSG_INACTIVE_IND";
            break;
          }

          default:
          {
            MSG_GERAN_ERROR_1_G("Unknown RR_FEMTO_OMSG: 0x%02x", msg_ptr->rr.femto.omsg.header.imh.message_id);
          }
        }
        break;
      } /* RR_FEMTO_OMSG */

      default:
      {
        MSG_GERAN_ERROR_3_G_G("Unknown RR message set: %d, message ID: 0x%02x",
          msg_ptr->rr.header.rr_message_set,
          msg_ptr->rr.header.imh.message_id,
          0);
      }
    }
  }
  else
  {
    ERR_GERAN_FATAL_0_G("msg_ptr == NULL");
  }

  // message_set and message_id function parameters could be removed
  {
    int dummy;
    dummy = message_set;
    dummy = message_id;
    dummy = dummy;
  }

  return s;

} /* rr_femto_message_name */


/**
  @brief API for GPS

  @param p_BsInfo
 */
void rr_get_current_bs_info( current_bs_info_T *p_BsInfo )
{
  if (rr_femto_data.camp.status == RR_FEMTO_CAMP_STATUS_CAMPED)
  {
    gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info();

    p_BsInfo->camped_on_flag = TRUE;
    p_BsInfo->cell_identity = pcell_info_ptr->gsm.cell_identity;
    p_BsInfo->location_area_identification.plmn_id = pcell_info_ptr->gsm.location_area_identification.plmn_id;
    p_BsInfo->arfcn = pcell_info_ptr->gsm.BCCH_ARFCN;
    p_BsInfo->bsic = (uint8)((pcell_info_ptr->gsm.BSIC.PLMN_colour_code << 3) +
                             (pcell_info_ptr->gsm.BSIC.BS_colour_code));
  }
  else
  {
    // Zero result (incl. camped_on_flag)
    memset(p_BsInfo, 0, sizeof(current_bs_info_T));

    // Set non-zero values
    p_BsInfo->location_area_identification.plmn_id.identity[0] = 0xFF;
    p_BsInfo->location_area_identification.plmn_id.identity[1] = 0xFF;
    p_BsInfo->location_area_identification.plmn_id.identity[2] = 0xFF;
    p_BsInfo->location_area_identification.location_area_code = 0xFFFF;
    p_BsInfo->arfcn.num = 0xFFFF;
    p_BsInfo->bsic = 0xFF;
  }

} /* rr_get_current_bs_info */


#else /* FEATURE_FEMTO_GSM_NL */

/**
  @brief Dummy function to avoid compilation issues if FEATURE_FEMTO_GSM_NL is
  not defined
*/
void rr_femto_dummy_func(void)
{
  ;
}

#endif /* FEATURE_FEMTO_GSM_NL */

/* EOF */