/*============================================================================
  FILE:         rr_dtm.c

  OVERVIEW:     This module enables DTM functionality

                Copyright (c) 2005-2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_dtm.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#ifdef FEATURE_GSM_DTM

/**** Rest of module DTM only... ****/
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_gprs_defs.h"
#include "rr_general.h"
#include "rr_control.h"
#include "ms.h"
#include "rr_l2_send.h"
#include "rr_conn_release.h"
#include "rr_mac_send.h"
#include "rr_init.h"
#include "rr_decode_ie.h"
#include "rr_mm_send.h"
#include "gttp.h"
#include "rlc_grr.h"
#include "rr_l1_send.h"
#ifdef FEATURE_GSM_EDTM
#include "rr_gprs_edtm.h"
#endif /* FEATURE_GSM_EDTM */
#include "rr_conn_establish.h"
#include "rr_sapi3.h"
#include "rr_ded_meas_reporting.h"
#include "rr_cipher.h"
#include "rr_ds_if.h"
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
#include "rr_resel_calcs.h"   // for best_six_ncells
#include "rr_callback.h"
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
#include "rr_channel_mode.h"
#include "gprs_mem.h"
#include "rr_ded_config.h"
#include "rr_ps_access.h"
#include "rr_ps_domain.h"
#include "rr_nv.h"
#include "rr_thermal.h"
#include "rr_msc.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"
#include "timetick.h"
#include "gtmrs_g.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define MAX_LAPDM_DEFAULT_VALUE          0
#define MAX_LAPDM_OFFSET_FACTOR          5

/*
LLC PDUs are encapsulated in a GPRS INFORMATION message, sent via L2 using the GTTP protocol
The GPRS INFORMATION message is defined in 3GPP 44.018 9.6.1 GPRS Information
  octet 1-2 : header information
  octet 3-6 : TLLI
  octet   7 : LLC PDU length
  octet 8-n : LLC PDU
*/
#define MAX_GTTP_LLC_PDU_SIZE  (L3_MSG_MAX_LEN - 7)

#define RR_DTM_DEBUG_BUF_SIZE 25

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  /**
   * Initial state
   */
  DTM_NULL,

  /**
   * RR has initiated RACHing for a CS connection;
   * GPRS in GMM is suspended;
   * PS access is blocked
   */
  DTM_RR_CS_EST,

  /**
   * The cell supports DTM;
   * GPRS in GMM is resumed;
   * GMM has requested GPRS service;
   * PS access is being enabled (waiting for confirmation)
   */
  DTM_ENABLING_PS_ACCESS,

  /**
   * The cell supports DTM;
   * GPRS in GMM is resumed;
   * GMM has requested GPRS service;
   * PS access is enabled;
   * DTM is possible if an UL requested or an assignment is received
   */
  DTM_DTM_POSSIBLE,

  /**
   * PACKET ASSIGNMENT processing in progress
   */
  DTM_PROCESS_PACKET_ASSIGNMENT,

  /**
   * DTM ASSIGNMENT COMMAND processing in progress
   */
  DTM_PROCESS_ASSIGNMENT_COMMAND,

  /**
   * DTM transfer in progress
   */
  DTM_DTM_TRANSFER,

  /**
   * GTTP transfer in progress
   */
  DTM_GTTP_TRANSFER,

  /**
   * RR dedicated procedure (e.g. handover) in progress;
   * GPRS in GMM is suspended
   * PS access is blocked
   */
  DTM_DED_PROC_IN_PROGRESS,

  /**
   * The cell does not support DTM;
   * GPRS in GMM is suspended
   * PS access is blocked
   */
  DTM_CELL_DOES_NOT_SUPPORT_DTM,

  /**
   * The cell supports DTM;
   * GPRS in GMM is resumed;
   * GMM has not requested GPRS service
   * PS access is blocked
   */
  DTM_NO_PS_DOMAIN,

  /**
   * The cell supports DTM;
   * GPRS in GMM is resumed;
   * GMM has requested GPRS service
   * PS access is blocked due to DTM REJECT with T3142 specified (or no response at all);
   * DTM is possible if an assignment is received
   */
  DTM_NO_PS_ACCESS_UL_BLOCKED,

  /**
   * MSC change / Thermal Mitigation procedure in progress; 
   * PS access is blocked
   */
  DTM_MSC_CHANGE,

  /**
   * A G2X Handover is in progress;
   * GPRS in GMM is suspended;
   * PS access is blocked
   */
  DTM_G2X_HANDOVER,

  /**
   * PS access is being disabled (waiting for confirmation)
   */
  DTM_DISABLING_PS_ACCESS,

  DTM_STATE_COUNT

} dtm_control_state_t;

typedef enum
{
  DTM_PKT_ASSIGN_INACTIVE,
  DTM_PKT_ASSIGN_AWAITING_ASSIGN_CNF,
  DTM_PKT_ASSIGN_AWAITING_L1_CNF_REL_PENDING

} dtm_packet_assign_control_state_t;

typedef enum
{
  DTM_ASSIGN_CMD_INACTIVE,
  DTM_ASSIGN_CMD_SUSPENDING_LINK,
  DTM_ASSIGN_CMD_AWAITING_ASSIGN_CNF,
  DTM_ASSIGN_CMD_RESUMING_LINK,
  DTM_ASSIGN_CMD_RESUMING_LINK_PS_RELEASED,
  DTM_ASSIGN_CMD_RECONNECTING_AFTER_FAILURE,
  DTM_ASSIGN_CMD_RESUMING_LINK_AFTER_FAILURE,
  DTM_ASSIGN_CMD_RESUMING_LINK_AFTER_REJECT,
  DTM_ASSIGN_CMD_AWAITING_L1_CNF_REL_PENDING

} dtm_assign_command_control_state_t;

typedef enum
{
  DTM_CELL_DTM_NOT_SUPPORTED,
  DTM_CELL_DTM_SUPPORTED,
  DTM_CELL_HANDOVER_FROM_GSM_CELL,
  DTM_CELL_HANDOVER_FROM_DTM_CELL

} dtm_cell_capability_monitor_state_t;

typedef struct
{
  boolean                          cs_assign_present;
  struct
  {
    dtm_channel_assignment_t         channel_assignment;
    boolean                          cell_allocation_present;
    cell_channel_description_T       cell_allocation;
    boolean                          cipher_flag;
    cipher_algorithm_T               cipher_algorithm;
  }
                                   cs_assign;
  boolean                          ps_ul_assign_present;
  rr_packet_uplink_assignment_t    ps_ul_assign;
  boolean                          ps_dl_assign_present;
  rr_packet_downlink_assignment_t  ps_dl_assign;

} dtm_assignment_details_t;

typedef struct
{
  dtm_packet_assign_control_state_t   packet_assign_control;
  dtm_assign_command_control_state_t  assign_command_control;
  dtm_cell_capability_monitor_state_t cell_capability_monitor;
} rr_dtm_substate_t;

typedef struct
{
  rr_dtm_substate_t        substate;
  rr_dtm_substate_t        old_substate;
  rr_internal_band_T       old_band;
  rr_event_T               pending_action;
  uint8                    layer3_message_buffer[L3_MSG_MAX_LEN];
  mac_grr_ul_tbf_req_t     saved_ul_tbf_req;
  boolean                  saved_ul_tbf_req_valid;
  boolean                  purge_gmm_signalling;
  rr_cb_fn_t               ps_access_confirm_cb_fn;
  rr_cb_fn_t               no_ps_access_confirm_cb_fn;
  boolean                  old_dtm_support;
  struct
  {
    boolean                  l1_in_dtm;
    boolean                  ps_access_blocked;
  }                        assign_command_control_data;

} rr_dtm_data_t;

typedef enum
{
  RR_DTM_DBG_NO_EVENT,
  RR_DTM_DBG_ACTIVATE,
  RR_DTM_DBG_DEACTIVATE,
  RR_DTM_DBG_DTM_REQUEST_SENT_OTA,
  RR_DTM_DBG_DTM_ASSIGN_FAIL_SENT_OTA,
  RR_DTM_DBG_ASSIGN_COMPLETE_SENT_OTA,
  RR_DTM_DBG_GTTP_DATA_SENT_OTA,
  RR_DTM_DBG_GTTP_DATA_RCVD_OTA,
  RR_DTM_DBG_DTM_ASSIGN_CMD_RCVD,
  RR_DTM_DBG_DTM_ASSIGN_CMD_RCVD_ERR,
  RR_DTM_DBG_PKT_ASSIGN_RCVD,
  RR_DTM_DBG_PKT_ASSIGN_RCVD_ERR,
  RR_DTM_DBG_DTM_REJECT_RCVD,
  RR_DTM_DBG_DTM_REJECT_RCVD_ERR,
  RR_DTM_DBG_DTM_INFO_RCVD,
  RR_DTM_DBG_DTM_INFO_RCVD_ERR,
  RR_DTM_DBG_DTM_CHAN_ASSIGN_REQ_SENT,
  RR_DTM_DBG_DTM_CHAN_ASSIGN_CNF_RCVD,
  RR_DTM_DBG_DTM_CHAN_REL_IND_RCVD,
  RR_DTM_DBG_CS_REL_REQ_RCVD,
  RR_DTM_DBG_CS_REL_CNF_SENT,
  RR_DTM_DBG_UL_TBF_REQ,
  RR_DTM_DBG_T3148_EXPIRY,
  RR_DTM_DBG_AWAITING_L1_CNF,
  RR_DTM_DBG_NO_PS_ACCESS_REQ_RCVD,
  RR_DTM_DBG_PS_ACCESS_REQ_RCVD,
  RR_DTM_DBG_HANDOVER,
  RR_DTM_DBG_MAX_COUNT
  
} rr_dtm_debug_event_e_t;

typedef struct
{
  uint32 timestamp_fn;
  timetick_type timestamp_clk;
  rr_dtm_debug_event_e_t event;

} rr_dtm_debug_buf_entry_t;

typedef struct
{
  uint8                    index;
  rr_dtm_debug_buf_entry_t events[RR_DTM_DEBUG_BUF_SIZE];
} rr_dtm_debug_buf_t;

typedef struct
{
  dtm_control_state_t      state;
  dtm_control_state_t      old_state;
  boolean                  released_in_gttp;
  rr_dtm_data_t           *data_ptr;
  dtm_assignment_details_t dtm_assignment_details;  // Note: Used by RR-GPRS-EDTM when RR-DTM inactive
  rr_dtm_debug_buf_t       debug_buf;

} rr_dtm_static_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_dtm_static_data_t  rr_dtm_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_dtm_static_data_t*
 */
static rr_dtm_static_data_t *rr_dtm_get_data_ptr(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_dtm_data[as_index];
}

/*!
 * \brief Logs a DTM event to the debug buffer
 * 
 * \param event (in)
 * \param gas_id (in)
 */
static void log_dtm_event(rr_dtm_debug_event_e_t event, const gas_id_t gas_id)
{
  rr_dtm_debug_buf_entry_t *entry_ptr;

  rr_dtm_static_data_t *rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);
  rr_dtm_debug_buf_t *debug_buf_ptr = &rr_dtm_data_ptr->debug_buf;

  if (debug_buf_ptr->index >= RR_DTM_DEBUG_BUF_SIZE)
  {
    debug_buf_ptr->index = 0;
  }

  entry_ptr = &debug_buf_ptr->events[debug_buf_ptr->index];

  entry_ptr->timestamp_fn = GSTMR_GET_FN_GERAN(gas_id);
  entry_ptr->timestamp_clk = timetick_get();
  entry_ptr->event = event;

  ++debug_buf_ptr->index;

  return;
}

/*============================================================================

FUNCTION       start_t3148

DESCRIPTION    Starts T3148 with a duration of 4 secs
               (see 44.018 11.1.1 Timer of the Mobile Station side)

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static void start_t3148(const gas_id_t gas_id)
{
  MSG_GERAN_MED_0_G("Start T3148 (4 sec)");
  (void)rr_timer_start(T3148, GRR_TIMEOUT_1_SECOND * 4, gas_id);

} /* end start_t3148() */


/*============================================================================

FUNCTION       stop_t3148

DESCRIPTION    Stops T3148

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static void stop_t3148(const gas_id_t gas_id)
{
  if (rr_timer_cancel(T3148, gas_id))
  {
    MSG_GERAN_MED_0_G("Stopped T3148");
  }
} /* end stop_t3148() */


/*============================================================================

FUNCTION       is_t3148_running

DESCRIPTION    Queries if T3148 is running

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   TRUE if T3148 is running; FALSE otherwise

SIDE EFFECTS   None

=============================================================================*/
static boolean is_t3148_running(const gas_id_t gas_id)
{
  return rr_timer_is_running(T3148, gas_id);

} /* end is_t3148_running() */


/*============================================================================

FUNCTION       save_mac_grr_ul_tbf_req

DESCRIPTION    Save the contents of a received GRR_MAC_UL_TBF_REQ 

DEPENDENCIES   None

PARAMETERS     mac_grr_ul_tbf_req 

RETURN VALUE   None

SIDE EFFECTS   None 

=============================================================================*/
static void save_mac_grr_ul_tbf_req(mac_grr_ul_tbf_req_t *mac_grr_ul_tbf_req, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_dtm_static_data_t *rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  if (mac_grr_ul_tbf_req != NULL)
  {
    rr_dtm_data_ptr->data_ptr->saved_ul_tbf_req = (*mac_grr_ul_tbf_req);
    rr_dtm_data_ptr->data_ptr->saved_ul_tbf_req_valid = TRUE;
  }
  else
  {
    rr_dtm_data_ptr->data_ptr->saved_ul_tbf_req_valid = FALSE;
  }

} /* end save_mac_grr_ul_tbf_req() */


/*============================================================================

FUNCTION       get_mac_grr_ul_tbf_req

DESCRIPTION    Returns the contents of a previously-saved MAC_GRR_UL_TBF_REQ 
               message 

DEPENDENCIES   None

PARAMETERS     None 

RETURN VALUE   Pointer to the MAC_GRR_UL_TBF_REQ message

SIDE EFFECTS   None 

=============================================================================*/
static mac_grr_ul_tbf_req_t *get_mac_grr_ul_tbf_req(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_dtm_static_data_t *rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  return &rr_dtm_data_ptr->data_ptr->saved_ul_tbf_req;

} /* end get_mac_grr_ul_tbf_req() */


/*============================================================================

FUNCTION       get_dtm_control_state_name

DESCRIPTION    Returns a character string representing the dtm_control_state
               value indicated.

DEPENDENCIES   None

PARAMETERS     state

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static char *get_dtm_control_state_name(dtm_control_state_t state)
{
  char * s;

  switch ( state )
  {
    case DTM_NULL:
      s = "NULL";
      break;

    case DTM_RR_CS_EST:
      s = "RR_CS_EST";
      break;

    case DTM_ENABLING_PS_ACCESS:
      s = "ENABLING_PS_ACCESS";
      break;

    case DTM_DTM_POSSIBLE:
      s = "DTM_DTM_POSSIBLE";
      break;

    case DTM_PROCESS_PACKET_ASSIGNMENT:
      s = "PROCESS_PACKET_ASSIGNMENT";
      break;

    case DTM_PROCESS_ASSIGNMENT_COMMAND:
      s = "PROCESS_ASSIGNMENT_COMMAND";
      break;

    case DTM_DTM_TRANSFER:
      s = "DTM_TRANSFER";
      break;

    case DTM_GTTP_TRANSFER:
      s = "GTTP_TRANSFER";
      break;

    case DTM_DED_PROC_IN_PROGRESS:
      s = "DED_PROC_IN_PROGRESS";
      break;

    case DTM_CELL_DOES_NOT_SUPPORT_DTM:
      s = "CELL_DOES_NOT_SUPPORT_DTM";
      break;

    case DTM_NO_PS_DOMAIN:
      s = "NO_PS_DOMAIN";
      break;

    case DTM_NO_PS_ACCESS_UL_BLOCKED:
      s = "NO_PS_ACCESS_UL_BLOCKED";
      break;

    case DTM_MSC_CHANGE:
      s = "MSC_CHANGE";
      break;

    case DTM_G2X_HANDOVER:
      s = "G2X_HANDOVER";
      break;

    case DTM_DISABLING_PS_ACCESS:
      s = "DISABLING_PS_ACCESS";
      break;

    default:
      s = "?";
  }

  return s;

} /* end get_dtm_control_state_name() */


/*============================================================================

FUNCTION       get_dtm_cell_capability_monitor_state_name

DESCRIPTION    Returns a character string representing the
               dtm_cell_capability_monitor_state_t value indicated.

DEPENDENCIES   None

PARAMETERS     state

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static char *get_dtm_cell_capability_monitor_state_name(dtm_cell_capability_monitor_state_t state)
{
  char * s;

  switch ( state )
  {
    case DTM_CELL_DTM_NOT_SUPPORTED:
      s = "DTM_NOT_SUPPORTED";
      break;

    case DTM_CELL_DTM_SUPPORTED:
      s = "DTM_SUPPORTED";
      break;

    case DTM_CELL_HANDOVER_FROM_GSM_CELL:
      s = "HANDOVER_FROM_GSM_CELL";
      break;

    case DTM_CELL_HANDOVER_FROM_DTM_CELL:
      s = "HANDOVER_FROM_DTM_CELL";
      break;

    default:
      s = "?";
  }

  return s;

} /* end get_dtm_cell_capability_monitor_state_name() */


/*============================================================================

FUNCTION       get_dtm_packet_assign_control_state_name

DESCRIPTION    Returns a character string representing the
               dtm_packet_assign_control_state_t value indicated.

DEPENDENCIES   None

PARAMETERS     state

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static char *get_dtm_packet_assign_control_state_name(dtm_packet_assign_control_state_t state)
{
  char * s;

  switch ( state )
  {
    case DTM_PKT_ASSIGN_INACTIVE:
      s = "INACTIVE";
      break;

    case DTM_PKT_ASSIGN_AWAITING_ASSIGN_CNF:
      s = "AWAITING_ASSIGN_CNF";
      break;

    case DTM_PKT_ASSIGN_AWAITING_L1_CNF_REL_PENDING:
      s = "AWAITING_L1_CNF_REL_PENDING";
      break;

    default:
      s = "?";
  }

  return s;

} /* end get_dtm_packet_assign_control_state_name() */


/*============================================================================

FUNCTION       get_dtm_assign_command_control_state_name

DESCRIPTION    Returns a character string representing the
               dtm_assign_command_control_state_t value indicated.

DEPENDENCIES   None

PARAMETERS     state

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static char *get_dtm_assign_command_control_state_name(dtm_assign_command_control_state_t state)
{
  char * s;

  switch ( state )
  {
    case DTM_ASSIGN_CMD_INACTIVE:
      s = "INACTIVE";
      break;

    case DTM_ASSIGN_CMD_SUSPENDING_LINK:
      s = "SUSPENDING_LINK";
      break;

    case DTM_ASSIGN_CMD_AWAITING_ASSIGN_CNF:
      s = "AWAITING_ASSIGN_CNF";
      break;

    case DTM_ASSIGN_CMD_RESUMING_LINK:
      s = "RESUMING_LINK";
      break;

    case DTM_ASSIGN_CMD_RESUMING_LINK_PS_RELEASED:
      s = "RESUMING_LINK_PS_RELEASED";
      break;

    case DTM_ASSIGN_CMD_RECONNECTING_AFTER_FAILURE:
      s = "RECONNECTING_AFTER_FAILURE";
      break;

    case DTM_ASSIGN_CMD_RESUMING_LINK_AFTER_FAILURE:
      s = "RESUMING_LINK_AFTER_FAILURE";
      break;

    case DTM_ASSIGN_CMD_RESUMING_LINK_AFTER_REJECT:
      s = "RESUMING_LINK_AFTER_REJECT";
      break;

    case DTM_ASSIGN_CMD_AWAITING_L1_CNF_REL_PENDING:
      s = "AWAITING_L1_CNF_REL_PENDING";
      break;

    default:
      s = "?";
  }

  return s;

} /* end get_dtm_assign_command_control_state_name() */


/*============================================================================

FUNCTION       invalidate_dtm_sys_info

DESCRIPTION    Invalidates the DTM sys info data.

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static void invalidate_dtm_sys_info(const gas_id_t gas_id)
{
  gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

  cell_info->dtm_sys_info_valid = FALSE;
  cell_info->bcch_sys_info.gprs_cell_options.optional_extension_info.dtm_support = FALSE;

} /* end invalidate_dtm_sys_info() */


#ifdef FEATURE_GSM_EDTM
/*============================================================================

FUNCTION       set_default_dtm_sys_info_if_invalid

DESCRIPTION    Sets the DTM sys info to default values, if it is currently invalid.
               This is used in EDTM when DTM has been started using the enhanced
               establishment procedures

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static void set_default_dtm_sys_info_if_invalid(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_ptr = rr_gprs_get_scell_info(gas_id);

  if (scell_ptr->dtm_sys_info_valid == FALSE)
  {
    scell_ptr->dtm_sys_info_valid = TRUE;

    /* 3GPP 44.018 Table 10.5.2.35a.2 - default is zero */
    scell_ptr->dtm_sys_info.max_LAPDm = MAX_LAPDM_DEFAULT_VALUE;

    scell_ptr->dtm_sys_info.gprs_ms_txpwr_max_cch_valid = TRUE;
    scell_ptr->dtm_sys_info.gprs_ms_txpwr_max_cch =
      scell_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH;
  }
  else
  {
    MSG_GERAN_MED_3_G("DTM sys info already valid (%1d,%1d,%2d)",
            (int)scell_ptr->dtm_sys_info.max_LAPDm,
            (int)scell_ptr->dtm_sys_info.gprs_ms_txpwr_max_cch_valid,
            (int)scell_ptr->dtm_sys_info.gprs_ms_txpwr_max_cch);
  }

  return;
} /* end set_default_dtm_sys_info_if_invalid() */
#endif /* FEATURE_GSM_EDTM */


/*============================================================================

FUNCTION       get_layer3_message_buffer

DESCRIPTION    This returns a L3 message buffer, of size N201_DCCH. It is
               preloaded with the OTA message type and length set to the
               indicated values.

DEPENDENCIES   None

PARAMETERS     msg_type
               msg_len

RETURN VALUE   Pointer to the buffer

SIDE EFFECTS   None

=============================================================================*/
static uint8 *get_layer3_message_buffer(uint8 msg_type, uint8 *msg_len, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_dtm_static_data_t *rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  memset( rr_dtm_data_ptr->data_ptr->layer3_message_buffer, 0, N201_DCCH );

  rr_dtm_data_ptr->data_ptr->layer3_message_buffer[0] = RADIO_RESSOURCES;
  rr_dtm_data_ptr->data_ptr->layer3_message_buffer[1] = msg_type;
  (* msg_len) = 2;

  return rr_dtm_data_ptr->data_ptr->layer3_message_buffer;

} /* end get_layer3_message_buffer() */


/*============================================================================

FUNCTION       get_layer3_message_type

DESCRIPTION    Given a pointer to a L3 message buffer, this returns the OTA
               message type.

DEPENDENCIES   None

PARAMETERS     layer3_message

RETURN VALUE   L3 OTA message type

SIDE EFFECTS   None

=============================================================================*/
static uint8 get_layer3_message_type(byte *layer3_message)
{
  return layer3_message[1];

} /* end get_layer3_message_type() */


/*============================================================================

FUNCTION       clear_previous_assignment_details

DESCRIPTION    This function clears the information related to any previous
               assignment. Called when a new PACKET ASSIGNMENT or DTM ASSIGNMENT
               COMMAND is received

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static void clear_previous_assignment_details(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_dtm_static_data_t *rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  memset( &rr_dtm_data_ptr->dtm_assignment_details, 0, sizeof(rr_dtm_data_ptr->dtm_assignment_details) );

} /* end clear_previous_assignment_details() */


/*============================================================================

FUNCTION       send_dtm_request_ota

DESCRIPTION    Constructs and sends a DTM REQUEST message OTA

DEPENDENCIES   None

PARAMETERS     channel_req_info

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static void send_dtm_request_ota(rr_channel_req_info_t *channel_req_info, const gas_id_t gas_id)
{
  uint32 tlli32;
  csn_channel_req_desc_2_t channel_req_desc_2;
  uint8 msg_len, chan_req_desc_2_len;
  uint8 * layer3_message;

  /* Get the L3 message buffer, complete Protocol Discriminator, Skip Indicator */
  /* and Message Type */
  layer3_message = get_layer3_message_buffer(
    DTM_REQUEST,    // msg_type
    &msg_len,       // msg_len
    gas_id          // gas_id
  );

  /* Add the TLLI */
  tlli32 = rr_gmminfo_get_tlli(gas_id);
  layer3_message[msg_len++] = (byte) (tlli32 >> 24);
  layer3_message[msg_len++] = (byte) ((tlli32 >> 16) & 0xFF);
  layer3_message[msg_len++] = (byte) ((tlli32 >> 8) & 0xFF);
  layer3_message[msg_len++] = (byte) (tlli32 & 0xFF);

  /* Add the Channel Request Description 2 IE */
  channel_req_desc_2.pkt_est_cause = 0;

  /* See 04.18 10.5.2.8b Channel Request Description 2 for Packet Est Cause values */
  switch ( channel_req_info->pkt_est_cause )
  {
    case RR_PKT_EST_CAUSE_USER_DATA: channel_req_desc_2.pkt_est_cause = 0; break;
    case RR_PKT_EST_CAUSE_PAGE_RESP: channel_req_desc_2.pkt_est_cause = 1; break;
    case RR_PKT_EST_CAUSE_CELL_UPD : channel_req_desc_2.pkt_est_cause = 2; break;
    case RR_PKT_EST_CAUSE_MM       : channel_req_desc_2.pkt_est_cause = 3; break;
    case RR_PKT_EST_CAUSE_UNKNOWN  :
    default                        : channel_req_desc_2.pkt_est_cause = 0;
                                     MSG_GERAN_ERROR_1_G("Unknown pkt_est_cause %d",
                                               (int)channel_req_info->pkt_est_cause);
  }

  channel_req_desc_2.channel_req_desc.peak_thr_class = channel_req_info->peak_thr_class;
  channel_req_desc_2.channel_req_desc.radio_priority = channel_req_info->radio_priority;
  channel_req_desc_2.channel_req_desc.rlc_mode = channel_req_info->rlc_mode;
  channel_req_desc_2.channel_req_desc.llc_pdu_type = channel_req_info->llc_pdu_type;
  channel_req_desc_2.channel_req_desc.rlc_octet_cnt = channel_req_info->rlc_octet_cnt;
  channel_req_desc_2.pfi_present = channel_req_info->pfi_present;
  channel_req_desc_2.pfi = channel_req_info->pfi;
  channel_req_desc_2.mult_tbf_cap = 0;

  /* Channel Request Description 2 IE is type LV, which means the first octet contains */
  /* the length, and the remainder the IE contents */
  /* So encode the IE at offset (msg_len+1), which will determine the length, then insert */
  /* the length at offset msg_len, then add the IE content length to msg_len */
  chan_req_desc_2_len = gcsn1_rr_encoder(
    CHANNEL_REQ_DESC_2,           // message_type
    &channel_req_desc_2,          // src
    &layer3_message[msg_len+1]    // dst
  );

  layer3_message[msg_len++] = chan_req_desc_2_len;
  msg_len += chan_req_desc_2_len;

  (void) rr_send_dl_data_req(
    layer3_message,   // layer3_message
    (byte)msg_len,    // length
    SAPI0,            // SAPI
    DCCH,             // l2_channel_type
    gas_id            // gas_id
  );

  log_dtm_event(RR_DTM_DBG_DTM_REQUEST_SENT_OTA, gas_id);

} /* end send_dtm_request_ota() */


/*============================================================================

FUNCTION       send_dtm_assign_failure_ota

DESCRIPTION    Construct and send a DTM ASSIGN REJECT message OTA.
               The parameters l1_reconnected and l2_suspended affect the
               messages which are sent to L1 and L2 to achieve this

DEPENDENCIES   None

PARAMETERS     cause
               l1_reconnected
               l2_suspended

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static void send_dtm_assign_failure_ota(
  RR_cause_T cause,
  boolean l1_reconnected,
  boolean l2_suspended,
  const gas_id_t gas_id
)
{
  uint8 msg_len;
  uint8 * layer3_message;

  /* Get the L3 message buffer, complete Protocol Discriminator, Skip Indicator */
  /* and Message Type */
  layer3_message = get_layer3_message_buffer(
    DTM_ASSIGNMENT_FAILURE,   // msg_type
    &msg_len,                 // msg_len
    gas_id                    // gas_id
  );

  /* Add the RR Cause IE */
  layer3_message[msg_len++] = cause;

  if ( l1_reconnected )
  {
    rr_l2_reconnect_l2_and_send_l3_msg_ota(
      (byte *)layer3_message,   // l3_msg
      msg_len,                  // l3_msg_len
      gas_id                    // gas_id
    );
  }
  else
  if ( l2_suspended )
  {
    rr_l2_resume_l2_and_send_l3_msg_ota(
      (byte *)layer3_message,   // l3_msg
      msg_len,                  // l3_msg_len
      TRUE,                     // l1_already_connected
      gas_id                    // gas_id
    );
  }
  else
  {
    rr_l2_send_l3_msg_ota(
      (byte *)layer3_message,   // l3_msg
      msg_len,                  // l3_msg_len
      gas_id                    // gas_id
    );
  }

  log_dtm_event(RR_DTM_DBG_DTM_ASSIGN_FAIL_SENT_OTA, gas_id);

} /* end send_dtm_assign_failure_ota() */


/*============================================================================

FUNCTION       send_assignment_complete_ota

DESCRIPTION    Constructs and sends an ASSIGNMENT COMPLETE message OTA

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static void send_assignment_complete_ota(const gas_id_t gas_id)
{
  byte msg_len;
  uint8 * layer3_message;

  /* Get the L3 message buffer, complete Protocol Discriminator, Skip Indicator */
  /* and Message Type */
  layer3_message = get_layer3_message_buffer(
    ASSIGNMENT_COMPLETE,    // msg_type
    &msg_len,               // msg_len
    gas_id                  // gas_id
  );

  /* Add the RR Cause IE */
  layer3_message[msg_len++] = RR_NORMAL_EVENT;

  rr_l2_resume_l2_and_send_l3_msg_ota(
    (byte *)layer3_message,   // l3_msg
    msg_len,                  // l3_msg_len
    FALSE,                    // l1_already_connected
    gas_id                    // gas_id
  );

  log_dtm_event(RR_DTM_DBG_ASSIGN_COMPLETE_SENT_OTA, gas_id);

} /* end send_assignment_complete_ota() */


/*============================================================================

FUNCTION       send_gttp_data_ota

DESCRIPTION    Sends a LLC PDU container using GTTP via L2 (using a DL_DATA_REQ)

DEPENDENCIES   None

PARAMETERS     tlli
               llc_pdu_container

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static void send_gttp_data_ota(
  uint32                     tlli,
  gttp_llc_pdu_container_t * llc_pdu_container,
  const gas_id_t             gas_id
)
{
  byte msg_len;
  rr_dtm_static_data_t *rr_dtm_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(llc_pdu_container);

  // Obtain a pointer to the module data
  rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  if (llc_pdu_container->llc_pdu_len <= MAX_GTTP_LLC_PDU_SIZE)
  {
    memset( rr_dtm_data_ptr->data_ptr->layer3_message_buffer, 0, L3_MSG_MAX_LEN );

    rr_dtm_data_ptr->data_ptr->layer3_message_buffer[0] = GTTP;
    rr_dtm_data_ptr->data_ptr->layer3_message_buffer[1] = GPRS_INFORMATION;
    msg_len = 2;

    rr_dtm_data_ptr->data_ptr->layer3_message_buffer[msg_len++] = (uint8)((tlli >> 24) & 0xff);
    rr_dtm_data_ptr->data_ptr->layer3_message_buffer[msg_len++] = (uint8)((tlli >> 16) & 0xff);
    rr_dtm_data_ptr->data_ptr->layer3_message_buffer[msg_len++] = (uint8)((tlli >>  8) & 0xff);
    rr_dtm_data_ptr->data_ptr->layer3_message_buffer[msg_len++] = (uint8)((tlli >>  0) & 0xff);

    rr_dtm_data_ptr->data_ptr->layer3_message_buffer[msg_len++] = llc_pdu_container->llc_pdu_len;

	// Replace mecpy with memscpy
	 memscpy( &rr_dtm_data_ptr->data_ptr->layer3_message_buffer[msg_len],
			 sizeof(rr_dtm_data_ptr->data_ptr->layer3_message_buffer)-msg_len,
			 llc_pdu_container->llc_pdu,
			 llc_pdu_container->llc_pdu_len );


    msg_len += llc_pdu_container->llc_pdu_len;

    (void) rr_send_dl_data_req(
      (byte *)rr_dtm_data_ptr->data_ptr->layer3_message_buffer,   // layer3_message
      (byte)msg_len,                                              // length
      SAPI0,                                                      // SAPI
      DCCH,                                                       // l2_channel_type
      gas_id                                                      // gas_id
    );

    log_dtm_event(RR_DTM_DBG_GTTP_DATA_SENT_OTA, gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_2_G("llc_pdu_container too big for L3 message (%d, max=%d)",
              llc_pdu_container->llc_pdu_len,MAX_GTTP_LLC_PDU_SIZE);
  }

} /* end send_gttp_data_ota() */


/*============================================================================

FUNCTION       send_dtm_channel_assign_req

DESCRIPTION    Sends a GRR_MAC_DTM_CHANNEL_ASSIGN_REQ to GMAC using the
               information stored in rr_dtm_data

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static void send_dtm_channel_assign_req(const gas_id_t gas_id)
{
  gprs_l1_cell_opt_t                l1_cell_options;
  uint8                             cs_timeslot;

  frequency_information_T           dtm_freq_info = {0};
  cs_chan_desc_t                    cs_chan_desc = {0};

  cs_chan_desc_t                  * cs_chan_desc_ptr = NULL;
  rr_packet_uplink_assignment_t   * ps_ul_assign_ptr = NULL;
  rr_packet_downlink_assignment_t * ps_dl_assign_ptr = NULL;

  gprs_scell_info_t             *cell_info;
  gprs_cell_options_t           *gprs_cell_options;
  global_power_control_params_t *power_control_params;
  rr_dedicated_data_T           *dedic_ptr;
  rr_dtm_static_data_t          *rr_dtm_data_ptr;

  // Obtain a pointer to the module data
  rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  cell_info = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(cell_info);

  gprs_cell_options = rr_si_get_gprs_cell_options(gas_id);
  RR_NULL_CHECK_FATAL(gprs_cell_options);

  power_control_params = rr_si_get_power_control_params(gas_id);
  RR_NULL_CHECK_FATAL(power_control_params);

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(dedic_ptr);

  /* Setup L1 Cell Options */

  l1_cell_options.access_burst_type = gprs_cell_options->access_burst_type;
  l1_cell_options.drx_timer_max = gprs_cell_options->drx_timer_max;

  if ( gprs_cell_options->optional_extension_info_flag )
  {
    l1_cell_options.bep_period = gprs_cell_options->optional_extension_info.bep_period;
  }
  else
  {
    l1_cell_options.bep_period = 0;
  }

  l1_cell_options.alpha = power_control_params->alpha;
  l1_cell_options.n_avg_i = power_control_params->n_avg_i ;
  l1_cell_options.pc_meas_chan = power_control_params->pc_meas_chan;
  l1_cell_options.t_avg_t = power_control_params->t_avg_t;
  l1_cell_options.t_avg_w = power_control_params->t_avg_w;

  if ( cell_info->dtm_sys_info_valid && cell_info->dtm_sys_info.gprs_ms_txpwr_max_cch_valid )
  {
    l1_cell_options.gprs_ms_txpwr_max_cch =
      cell_info->dtm_sys_info.gprs_ms_txpwr_max_cch;
  }
  else
  {
    l1_cell_options.gprs_ms_txpwr_max_cch =
      cell_info->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH;
  }

  if ( rr_dtm_data_ptr->dtm_assignment_details.cs_assign_present )
  {
    cs_timeslot =
      (uint8) rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.TN;

    if ( rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode_present )
    {
      cs_chan_desc.channel_mode =
        rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode;
    }
    else
    {
      cs_chan_desc.channel_mode = dedic_ptr->curr_channel_spec.channel_mode_1;
    }

    cs_chan_desc.cs_chan_info.channel_type =
      rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.channel_type;

    cs_chan_desc.cs_chan_info.subchannel =
      rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.subchannel;

    cs_chan_desc.cs_chan_info.TN =
      rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.TN;

    cs_chan_desc.DTX_indicator = dedic_ptr->curr_channel_spec.DTX_indicator;

    /* Note: multi_rate_config contains a 'valid' flag, which will be left cleared if no */
    /* data is copied over here. L1 will then not use this data */
    if (rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.multi_rate_config_provided)
    {
		//Structure assignment 
		cs_chan_desc.multi_rate_config= dedic_ptr->next_amr_config;

    }

    cs_chan_desc.power_level =
      rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.power_level;

    cs_chan_desc.cipher_flag = rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cipher_flag;
    cs_chan_desc.cipher_algorithm = rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cipher_algorithm;

    cs_chan_desc_ptr = &cs_chan_desc;

    dtm_freq_info.frequency_list =
      rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.frequency_list;

    dtm_freq_info.hopping_flag =
      rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.hopping_flag;

    dtm_freq_info.hsn =
      rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.HSN;

    dtm_freq_info.maio =
     rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.MAIO;

    dtm_freq_info.tsc =
     rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.TSC;
  }
  else
  {
    cs_timeslot = (uint8) dedic_ptr->curr_channel_spec.channel_info_1_after.TN;
  }

  if ( rr_dtm_data_ptr->dtm_assignment_details.ps_ul_assign_present )
  {
    ps_ul_assign_ptr = &rr_dtm_data_ptr->dtm_assignment_details.ps_ul_assign;
  }

  if ( rr_dtm_data_ptr->dtm_assignment_details.ps_dl_assign_present )
  {
    ps_dl_assign_ptr = &rr_dtm_data_ptr->dtm_assignment_details.ps_dl_assign;
  }

  
  MSG_GERAN_MED_0_G("Start RR_GRR_CHN_ASSIGN_REQ_GUARD_TIMER (5 sec)");
  (void)rr_timer_start(RR_GRR_CHN_ASSIGN_REQ_GUARD_TIMER, GRR_TIMEOUT_5_SECONDS, gas_id);

  rr_send_grr_mac_dtm_channel_assign_req(
    cs_timeslot,        // cs_timeslot
    cs_chan_desc_ptr,   // cs_chan_desc
    &dtm_freq_info,     // cs_freq_info
    &l1_cell_options,   // l1_cell_options
    ps_ul_assign_ptr,   // pkt_uplink_assign
    ps_dl_assign_ptr,   // pkt_downlink_assign
    gas_id              // gas_id
  );

  log_dtm_event(RR_DTM_DBG_DTM_CHAN_ASSIGN_REQ_SENT, gas_id);

} /* send_dtm_channel_assign_req() */


#ifdef FEATURE_GSM_EDTM
/*============================================================================

FUNCTION       rr_dtm_send_edtm_assignment_to_gmac

DESCRIPTION    Validate a DTM Assignment for EDTM establishment and sends it to
               GMAC (in a GRR_MAC_DTM_CHANNEL_ASSIGN_REQ)

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   EV_DTM_ASSIGNMENT_ERROR is the CS assignment is invalid;
               EV_NO_EVENT otherwise

SIDE EFFECTS   None

=============================================================================*/
rr_event_T rr_dtm_send_edtm_assignment_to_gmac(const gas_id_t gas_id)
{
  rr_event_T return_event;
  rr_dtm_static_data_t *rr_dtm_data_ptr;

  // Obtain a pointer to the module data
  rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  if (rr_dtm_data_ptr->dtm_assignment_details.cs_assign_present == TRUE)
  {
    dtm_channel_assignment_t *cs_assign_ptr;

    /* notify the EDTM module of the CS channel type */
    cs_assign_ptr = &(rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment);
    rr_gprs_edtm_store_channel_needed(cs_assign_ptr->channel_description.channel_type, gas_id);

    /* disable system information refresh */
    rr_gprs_suspend_camped_sys_info_handling(FALSE, gas_id);

    /* send the DTM assignment to GMAC */
    send_dtm_channel_assign_req(gas_id);

    return_event = EV_NO_EVENT;
  }
  else
  {
    return_event = EV_DTM_ASSIGNMENT_ERROR;
  }

  return(return_event);
} /* rr_dtm_send_edtm_assignment_to_gmac */
#endif /* FEATURE_GSM_EDTM */


/*!
 * \brief Send GRR_MAC_PS_ACCESS_REQ. If T3142 is running, GMM signalling PDUs are purged.
 * 
 * \param gas_id 
 */
static void send_grr_mac_ps_access_req(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_dtm_static_data_t *rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  if (rr_timer_is_running(T3142, gas_id))
  {
    rr_send_grr_mac_ps_access_req(
      TRUE,     // purge_gmm_signalling
      gas_id    // gas_id
    );
  }
  else
  {
    rr_send_grr_mac_ps_access_req(
      rr_dtm_data_ptr->data_ptr->purge_gmm_signalling,    // purge_gmm_signalling
      gas_id                                              // gas_id
    );
  }

  rr_dtm_data_ptr->data_ptr->purge_gmm_signalling = FALSE;

  // Clear any saved UL request
  save_mac_grr_ul_tbf_req(NULL, gas_id);
}

/*!
 * \brief Sends RR_DTM_OMSG_CS_RELEASE_CNF
 * 
 * \param ce_rel_mode (in)
 * \param gas_id (in)
 */
static void send_omsg_cs_release_cnf(
  rr_dtm_ce_rel_mode_e ce_rel_mode,
  const gas_id_t gas_id
)
{
  rr_dtm_send_omsg_cs_release_cnf(ce_rel_mode, gas_id);

  log_dtm_event(RR_DTM_DBG_CS_REL_CNF_SENT, gas_id);
}

/*============================================================================

FUNCTION       process_packet_assignment

DESCRIPTION    This function decodes the contents of a PACKET ASSIGNMENT.
               See 44.018 9.1.21f Packet Assignment for the message definition

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   Returns EV_DTM_PACKET_ASSIGNMENT_CHECKED if the message contents
               are ok, EV_DTM_ASSIGNMENT_ERROR if not

SIDE EFFECTS   None

=============================================================================*/
static rr_event_T process_packet_assignment(const gas_id_t gas_id)
{
  rr_event_T  return_event;
  ie_descriptor_T * current_IE;
  boolean decoding_ok;
  gprs_broadcast_information_t  gprs_broadcast_information;
  rr_dtm_static_data_t *rr_dtm_data_ptr;

  uint8 remaining_length = 0;

  // Obtain a pointer to the module data
  rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  RR_ZERO_STRUCTURE(gprs_broadcast_information);

  clear_previous_assignment_details(gas_id);

  /* See 44.018 9.1.21f Packet Assignment for message contents and IEI values */

  /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(PACKET_ASSIGNMENT, gas_id);
  decoding_ok = FALSE;

  if ( current_IE )
  {
    /* IE mandatory: "GPRS broadcast information" */
    if ( current_IE->start_IE )
    {

      remaining_length = rr_length_to_end_of_ota_msg(
        PACKET_ASSIGNMENT,            // msg_type
        current_IE->start_IE,         // csn1_start_ie_ptr
        GPRS_BROADCAST_INFORMATION,   // csn_ie_type
        gas_id
      );

      /* The first octet is the length - skip this when decoding */

      decoding_ok = (
        gcsn1_rr_decoder(
          GPRS_BROADCAST_INFORMATION,     // message_type
          &current_IE->start_IE[1],       // csn1_ie_src
          &gprs_broadcast_information,    // dst
          remaining_length,               // len_to_end_of_buf
          gas_id
        ) == CSN_DECODE_NO_ERROR);

      if ( ! decoding_ok )
      {
        MSG_GERAN_ERROR_0_G("Error in GPRS broadcast information");
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Mandatory GPRS Broadcast Information IE missing");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("message_list parsing error");
  }

  if ( decoding_ok )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* IE optional: "RR Packet Uplink Assignment" */
      if ( current_IE->start_IE )
      {
        /* RR PACKET UPLINK ASSIGNMENT IEI = 0x22 */
        if ( current_IE->IEI == 0x22 )
        {
          MSG_GERAN_MED_0_G("RR PACKET UPLINK ASSIGNMENT IEI present");

          remaining_length = rr_length_to_end_of_ota_msg(
            PACKET_ASSIGNMENT,              // msg_type
            current_IE->start_IE,           // csn1_start_ie_ptr
            RR_PACKET_UPLINK_ASSIGNMENT,    // csn_ie_type
            gas_id
          );

          decoding_ok = (
            gcsn1_rr_decoder(
              RR_PACKET_UPLINK_ASSIGNMENT,                              // message_type
              current_IE->start_IE,                                     // csn1_ie_src
              &rr_dtm_data_ptr->dtm_assignment_details.ps_ul_assign,    // dst
              remaining_length,                                         // len_to_end_of_buf
              gas_id
            ) == CSN_DECODE_NO_ERROR);

          if ( decoding_ok )
          {
            rr_dtm_data_ptr->dtm_assignment_details.ps_ul_assign_present = TRUE;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Error in RR Packet Uplink Assign");
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("IEI (%d) not RR PACKET UPLINK ASSIGNMENT",(int)current_IE->IEI);
        }
      }
      else
      {
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if ( decoding_ok )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* IE optional: "RR Packet Downlink Assignment" */
      if ( current_IE->start_IE )
      {
        /* RR PACKET DOWNLINK ASSIGNMENT IEI = 0x23 */
        if ( current_IE->IEI == 0x23 )
        {
          MSG_GERAN_MED_0_G("RR PACKET DOWNLINK ASSIGNMENT IEI present");

          remaining_length = rr_length_to_end_of_ota_msg(
            PACKET_ASSIGNMENT,                // msg_type
            current_IE->start_IE,             // csn1_start_ie_ptr
            RR_PACKET_DOWNLINK_ASSIGNMENT,    // csn_ie_type
            gas_id
          );

          decoding_ok = (
            gcsn1_rr_decoder(
              RR_PACKET_DOWNLINK_ASSIGNMENT,                            // message_type
              current_IE->start_IE,                                     // csn1_ie_src
              &rr_dtm_data_ptr->dtm_assignment_details.ps_dl_assign,    // dst
              remaining_length,                                         // len_to_end_of_buf
              gas_id
            ) == CSN_DECODE_NO_ERROR);

          if ( decoding_ok )
          {
            rr_dtm_data_ptr->dtm_assignment_details.ps_dl_assign_present = TRUE;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Error in RR Packet Downlink Assign");
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("IEI (%d) not RR PACKET DOWNLINK ASSIGNMENT",(int)current_IE->IEI);
        }
      }
      else
      {
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if ( decoding_ok )
  {
    /* Now perform some sanity checks on the PACKET ASSIGNMENT contents */

    if ( (!rr_dtm_data_ptr->dtm_assignment_details.ps_dl_assign_present) &&
         (!rr_dtm_data_ptr->dtm_assignment_details.ps_ul_assign_present) )
    {
      MSG_GERAN_ERROR_0_G("No UL or DL PS allocation present");
      decoding_ok = FALSE;
    }
    else
    if ( rr_dtm_data_ptr->dtm_assignment_details.ps_ul_assign_present &&
         (!is_t3148_running(gas_id)) )
    {
      MSG_GERAN_ERROR_0_G("UL PS allocation present but not requested");
      decoding_ok = FALSE;
    }
  }

  if ( decoding_ok )
  {
    /* The contents of all IEs have been verified and seem reasonable */

    gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);
    rrps_store_update_data_t * rrps_store_update_ptr = rr_get_rrps_store_update(gas_id);

    MSG_GERAN_HIGH_0_G("Decoding of PACKET ASSIGNMENT successful");

    /* Update the system information database */

    cell_info->bcch_sys_info.gprs_cell_options =
      gprs_broadcast_information.gprs_cell_options;

    cell_info->bcch_sys_info.power_control_params.alpha =
      gprs_broadcast_information.gprs_power_control_params.alpha;
    cell_info->bcch_sys_info.power_control_params.n_avg_i =
      gprs_broadcast_information.gprs_power_control_params.n_avg_i;
    cell_info->bcch_sys_info.power_control_params.pc_meas_chan =
      gprs_broadcast_information.gprs_power_control_params.pc_meas_chan;
    cell_info->bcch_sys_info.power_control_params.t_avg_t =
      gprs_broadcast_information.gprs_power_control_params.t_avg_t;
    cell_info->bcch_sys_info.power_control_params.t_avg_w =
      gprs_broadcast_information.gprs_power_control_params.t_avg_w;

    /* Update the public store copy of the system information */
    if (rrps_store_update_ptr != NULL)
    {
      rrps_store_update_ptr->gprs_cell_options =
        gprs_broadcast_information.gprs_cell_options;

      rrps_store_update_ptr->power_control_params.alpha =
        gprs_broadcast_information.gprs_power_control_params.alpha;
      rrps_store_update_ptr->power_control_params.n_avg_i =
        gprs_broadcast_information.gprs_power_control_params.n_avg_i;
      rrps_store_update_ptr->power_control_params.pc_meas_chan =
        gprs_broadcast_information.gprs_power_control_params.pc_meas_chan;
      rrps_store_update_ptr->power_control_params.t_avg_t =
        gprs_broadcast_information.gprs_power_control_params.t_avg_t;
      rrps_store_update_ptr->power_control_params.t_avg_w =
        gprs_broadcast_information.gprs_power_control_params.t_avg_w;

      rr_write_public_store(
        RRPS_UPD_CELL_OPTIONS | RRPS_UPD_POWER_CTRL,
        rrps_store_update_ptr,
        gas_id
      );

      rr_free_rrps_store_update(
        rrps_store_update_ptr,
        gas_id
      );
    }
    else
    {
      MSG_GERAN_ERROR_0_G("RRPS update store NULL pointer");
    }

    log_dtm_event(RR_DTM_DBG_PKT_ASSIGN_RCVD, gas_id);

    return_event = EV_DTM_PACKET_ASSIGNMENT_CHECKED;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Decoding of PACKET ASSIGNMENT failed");

    log_dtm_event(RR_DTM_DBG_PKT_ASSIGN_RCVD_ERR, gas_id);

    return_event = EV_DTM_ASSIGNMENT_ERROR;
  }

  return return_event;

} /* end process_packet_assignment() */


/*============================================================================

FUNCTION       process_dtm_reject

DESCRIPTION    This function decodes the contents of a DTM REJECT message.
               See 44.018 9.1.12h DTM Reject for the message definition

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   EV_DTM_ACCESS_REJECT
               EV_DTM_ACCESS_REJECT_T3142_RUNNING
               EV_NO_EVENT

SIDE EFFECTS   None

=============================================================================*/
static rr_event_T process_dtm_reject(const gas_id_t gas_id)
{
  ie_descriptor_T * current_IE;

  rr_event_T return_event = EV_NO_EVENT;
  uint8 t3122_t3142_timeout_value = 0;
  boolean decoding_ok = TRUE;

  /* See 04.18 9.1.12h DTM Reject for message contents */

  /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(DTM_REJECT, gas_id);

  if ( current_IE )
  {
    /* First IE is Wait Indication, see 04.18 10.5.2.43 Wait Indication */

    if ( current_IE->start_IE )
    {
      t3122_t3142_timeout_value = (uint8) (* current_IE->start_IE);
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Error decoding WAIT INDICATION IE");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Error in DTM REJECT construction (no WAIT INDICATION IE)");
    decoding_ok = FALSE;
  }

  if ( decoding_ok )
  {
    if ( t3122_t3142_timeout_value == 0 )
    {
      /* There is no wait indicator in DTM REJECT, So send no ps access*/
      /* and ps access immediately*/
      rr_send_grr_mac_no_ps_access_ind(
        NO_DEL,   // delete_pdu
        gas_id    // gas_id
      );

      rr_send_grr_mac_ps_access_req(
        FALSE,    // purge_gmm_signalling
        gas_id    // gas_id
      );

      return_event = EV_DTM_ACCESS_REJECT;
    }
    else
    {
      grr_start_t3142(t3122_t3142_timeout_value, gas_id);

      return_event = EV_DTM_ACCESS_REJECT_T3142_RUNNING;
    }

    log_dtm_event(RR_DTM_DBG_DTM_REJECT_RCVD, gas_id);
  }
  else
  {
    log_dtm_event(RR_DTM_DBG_DTM_REJECT_RCVD_ERR, gas_id);
  }

  return return_event;

} /* end process_dtm_reject() */


/*============================================================================

FUNCTION       process_dtm_information

DESCRIPTION    This functions decodes the contents of a DTM INFORMATION message.
               See 44.018 9.1.12g DTM Information for the message definition

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   TRUE if message is decoded ok; FALSE if there was an error

SIDE EFFECTS   None

=============================================================================*/
static boolean process_dtm_information(const gas_id_t gas_id)
{
  ie_descriptor_T * current_IE;
  boolean decoding_ok;
  routing_area_id_T routing_area_id;
  dtm_information_details_t dtm_information_details;

  uint8 remaining_length = 0;

  RR_ZERO_STRUCTURE(routing_area_id);
  RR_ZERO_STRUCTURE(dtm_information_details);

  /* See 04.18 9.1.12g DTM Information for message contents */

  /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(DTM_INFORMATION, gas_id);
  decoding_ok = FALSE;

  if ( current_IE )
  {
    /* IE mandatory: "Routing Area Identification IE", see 24.008 10.5.5.15 */
    if ( current_IE->start_IE )
    {
      /* Note: The Routing Area Identifier IEI is not present as part of the */
      /* Routing Area Identifier IE when part of the DTM INFORMATION message */
      uint8 * rai_iei = &current_IE->start_IE[0];

      routing_area_id.plmn_id.identity[0] = rai_iei[0];
      routing_area_id.plmn_id.identity[1] = rai_iei[1];
      routing_area_id.plmn_id.identity[2] = rai_iei[2];

      routing_area_id.location_area_code.lac[0] = rai_iei[3];
      routing_area_id.location_area_code.lac[1] = rai_iei[4];

      routing_area_id.routing_area_code = rai_iei[5];

      /* Not possible to determine if there are any errors - so assume it's ok */
      decoding_ok = TRUE;
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Mandatory Routing Area Indentification IE missing");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("message_list parsing error");
  }

  /* Mandatory IE: "DTM Information Details", see 04.18 10.5.2.11a */
  if ( decoding_ok )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* IE mandatory: "DTM Information Details" */
      if ( current_IE->start_IE )
      {

        remaining_length = rr_length_to_end_of_ota_msg(
          DTM_INFORMATION,            // msg_type
          current_IE->start_IE,       // csn1_start_ie_ptr
          DTM_INFORMATION_DETAILS,    // csn_ie_type
          gas_id
        );

        /* The first octet is the length - skip this for the decode */
        decoding_ok = (
          gcsn1_rr_decoder(
            DTM_INFORMATION_DETAILS,    // message_type
            &current_IE->start_IE[1],   // csn1_ie_src
            &dtm_information_details,   // dst
            remaining_length,           // len_to_end_of_buf
            gas_id
          ) == CSN_DECODE_NO_ERROR);

        if ( ! decoding_ok )
        {
          MSG_GERAN_ERROR_0_G("Error in DTM Information Details");
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("Mandatory DTM Information Details IE missing");
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (decoding_ok)
  {
    gprs_scell_info_t *cell_info = rr_gprs_get_scell_info(gas_id);

    /* Note: BCCH sys info store is always used during DTM */

    cell_info->gsm.location_area_identification.plmn_id = routing_area_id.plmn_id;

    /* The LAC is 16 bits, split over 2 octets */
    /* Bit 8 of octet 0 is the MSB and bit 1 of octet 1 the LSB */
    /* (see 24.008 Table 10.5.148) */
    cell_info->gsm.location_area_identification.location_area_code =
      (routing_area_id.location_area_code.lac[0] << 8) +
      routing_area_id.location_area_code.lac[1];

    cell_info->gsm.cell_identity = (word) dtm_information_details.cell_identity;
    cell_info->gsm.cell_identity_valid_flag = TRUE;

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
    {
      best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

      rr_notify_cell_id_change(
        cell_info->gsm.cell_identity,
        *best_six_ncells_ptr,
        cell_info->gsm.location_area_identification,
        gas_id
      );
    }
#endif /*FEATURE_GSM_RR_CELL_CALLBACK*/

    cell_info->bcch_sys_info.routing_area_code = routing_area_id.routing_area_code;

    cell_info->dtm_sys_info.gprs_ms_txpwr_max_cch =
      dtm_information_details.gprs_ms_txpwr_max_cch;
    cell_info->dtm_sys_info.gprs_ms_txpwr_max_cch_valid = TRUE;
    cell_info->dtm_sys_info.max_LAPDm = dtm_information_details.max_LAPDm;

    cell_info->dtm_sys_info_valid = TRUE;

    cell_info->bcch_sys_info.gprs_cell_options.optional_extension_info_flag = valid;
    cell_info->bcch_sys_info.gprs_cell_options.optional_extension_info.dtm_support= TRUE;

    /* The fact that DTM is supported must mean the cell supports GPRS. The */
    /* gprs_indicator would normally be set from the contents of SI3, but this */
    /* won't be received during a call */
    cell_info->gsm.gprs_indicator = TRUE;

    log_dtm_event(RR_DTM_DBG_DTM_INFO_RCVD, gas_id);

    return TRUE;  // The message was decoded successfully
  }

  log_dtm_event(RR_DTM_DBG_DTM_INFO_RCVD_ERR, gas_id);

  return FALSE;  // There was an error decoding the message

} /* end process_dtm_information() */


/*============================================================================

FUNCTION       process_packet_notification

DESCRIPTION    This function decodes the contents of a PACKET NOTIFICATION
               message.
               See 44.018 9.1.21g Packet Notification for the message definition

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
static void process_packet_notification(const gas_id_t gas_id)
{
  ie_descriptor_T * current_IE;
  boolean decoding_ok;
  rr_page_info_t page_info;

  boolean page_is_for_mobile = FALSE;

  // Either P-TMSI IE or Mobile Identity IE should be present, not both.
  // Start with the assumption that Mobile Identity IE will be present.
  boolean mobile_identity_ie_expected = TRUE;

  page_info.domain = RR_PAGE_DOMAIN_PS;
  page_info.identity_type = RR_IDENTITY_TYPE_NONE;

  /* See 44.018 9.1.21g Packet Notification for message contents */

  /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(PACKET_NOTIFICATION, gas_id);
  decoding_ok = FALSE;

  if ( current_IE )
  {
    /* IE conditional: "Packet P-TMSI", see 44.018 10.5.2.42 */
    if ( current_IE->start_IE )
    {
      /* Packet TMSI IE = 0x10 */
      if ( current_IE->IEI == 0x10 )
      {
        MSG_GERAN_MED_0_G("P-TMSI IE present");

        if (rr_ce_compare_ota_ptmsi(current_IE->start_IE, gas_id))
        {
          page_info.identity_type = RR_IDENTITY_TYPE_PTMSI;
          page_is_for_mobile = TRUE;
        }

        /* Not possible to determine if there are any errors - so assume it's ok */
        decoding_ok = TRUE;

        // As the P-TMSI IE was included, the Modile Identity IE is not expected
        mobile_identity_ie_expected = FALSE;
      }
      else
      {
        MSG_GERAN_ERROR_1_G("IEI (%d) not Packet TMSI",(int)current_IE->IEI);
      }
    }
    else
    {
      decoding_ok = TRUE;
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("message_list parsing error");
  }

  if ( decoding_ok )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* Next IE conditional: "Mobile Identity" */
      if ( current_IE->start_IE )
      {
        /* Mobile Identity IEI = 0x11 */
        if ( current_IE->IEI == 0x11 )
        {
          MSG_GERAN_MED_0_G("Mobile Identity IE present");

          if (mobile_identity_ie_expected)
          {
            page_is_for_mobile = rr_ce_compare_ota_mobile_identity(
              current_IE->start_IE,
              &decoding_ok,
              &page_info.identity_type,
              gas_id
            );

            if ( decoding_ok )
            {
              if ( page_is_for_mobile )
              {
                memscpy(
                  page_info.identity,
                  sizeof(page_info.identity),
                  current_IE->start_IE,
                  sizeof(page_info.identity)
                );
              }
            }
            else
            {
              MSG_GERAN_ERROR_0_G("Error in Mobile Identity");
            }
          }
          else
          {
            MSG_GERAN_HIGH_0_G("Mobile Identity IE ignored as P-TMSI IE included");
            decoding_ok = TRUE;
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("IEI (%d) not Mobile Identity",(int)current_IE->IEI);
        }
      }
      else
      {
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (page_is_for_mobile)
  {
    rr_send_imsg_page_ind(&page_info, gas_id);
  }

} /* end process_packet_notification() */


/*============================================================================

FUNCTION       get_max_LAPDm_frames

DESCRIPTION    Returns the maximum number of LAPDm frames that can be used for
               DTM transfer via GTTP. This value is broadcast in SI6 and the DTM
               INFORMATION message

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   5..12

SIDE EFFECTS   None

=============================================================================*/
static uint8 get_max_LAPDm_frames(const gas_id_t gas_id)
{
  gprs_scell_info_t *cell_info = rr_gprs_get_scell_info(gas_id);
  uint8              max_LAPDm;

  /* The max_LAPDm value is broadcast in DTM INFORMATION and SI6 */
  if ( cell_info->dtm_sys_info_valid &&
       ( cell_info->dtm_sys_info.max_LAPDm < 8 ) )
  {
    max_LAPDm = cell_info->dtm_sys_info.max_LAPDm;
  }
  else
  {
    /* DTM sys info not received, or LAPDm value out of range, use a default */
    max_LAPDm = MAX_LAPDM_DEFAULT_VALUE;
  }

  /* Note: There is an offset between the value broadcast and value it represents */
  /* see 44.018 Table 10.5.2.35a.2: SI 6 Rest Octets information element details  */
  /* (0 = 5 frames, 1 = 6 frames, 2 = 7 frames, ..., 7 = 12 frames) */
  return (max_LAPDm + MAX_LAPDM_OFFSET_FACTOR);

} /* end get_max_LAPDm_frames() */


/*============================================================================

FUNCTION       gttp_criteria_met

DESCRIPTION    Evaluates if the requested number of octets can be transferred
               using GTTP via L2

DEPENDENCIES   None

PARAMETERS     num_of_octets

RETURN VALUE   TRUE if GTTP can be used; FALSE otherwise

SIDE EFFECTS   None

=============================================================================*/
static boolean gttp_criteria_met(uint16 num_of_octets, const gas_id_t gas_id)
{
  uint16 max_octets_per_LAPDm_frame, max_LAPDm_frames, max_num_of_octets_available;

  /*
  If sent using GTTP via L2, the data will be sent using an L2 I-frame, as it is an
  acknowledged transfer.
  The format of a frame used for L2 data exchange is either Format A, B, Bbis, or B4.
  The I-frame is Format B (see 44.006 5.1 General).
  The max num of octets in the information field of a Format B frame is 20
  (see 44.006 8.8.3 Maximum number of octets...)
  This is defined as N201_DCCH in rr_l2.h
  */

  max_octets_per_LAPDm_frame = N201_DCCH;

  /* Get the max_LAPDm value broadcast in DTM INFORMATION / SI6 */
  max_LAPDm_frames = get_max_LAPDm_frames(gas_id);

  /* Find the max num of octets available */
  max_num_of_octets_available = (uint16) (max_octets_per_LAPDm_frame * max_LAPDm_frames);

  /* Now cap this at the max which can be sent via L2 */
  if (max_num_of_octets_available > MAX_GTTP_LLC_PDU_SIZE)
  {
    MSG_GERAN_HIGH_2_G("Capping max_num_of_octets_available (%d) at MAX_GTTP_LLC_PDU_SIZE (%d)",
             max_num_of_octets_available,MAX_GTTP_LLC_PDU_SIZE);

    max_num_of_octets_available = MAX_GTTP_LLC_PDU_SIZE;
  }

  MSG_GERAN_HIGH_3_G("gttp_criteria: num_of_octets=%d max_LAPDm_frames=%d max_octets=%d",
           num_of_octets,
           max_LAPDm_frames,
           max_num_of_octets_available);

  /* Return TRUE if the message can be sent via GTTP; FALSE otherwise */
  return ( ( max_num_of_octets_available > 0 ) &&
           ( num_of_octets > 0 ) &&
           ( num_of_octets <= max_num_of_octets_available ) );

} /* end gttp_criteria_met() */


/*============================================================================

FUNCTION       dtm_packet_assign_control

DESCRIPTION    DTM module sub-state machine for processing PACKET ASSIGNMENT

DEPENDENCIES   None

PARAMETERS     event_in
               message_ptr

RETURN VALUE   EV_DTM_PACKET_ASSIGNMENT_CHECKED
               EV_DTM_ASSIGNMENT_ERROR
               EV_DTM_CSPS_EST_CNF
               EV_NO_EVENT

SIDE EFFECTS   None

=============================================================================*/
static rr_event_T dtm_packet_assign_control(rr_event_T event_in, void *message_ptr, const gas_id_t gas_id)
{
  byte message_id;
  byte message_set;

  rr_event_T  event_out = EV_NO_EVENT;
  rr_event_T  new_event = EV_NO_EVENT;

  // Obtain a pointer to the module data
  rr_dtm_static_data_t *rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  // There should be nothing passed into this state machine until the control/data structures have been setup
  RR_ASSERT_FATAL(rr_dtm_data_ptr->data_ptr != NULL);

  /* If EV_INPUT_MESSAGE is passed in, message_set & message_id will be setup from */
  /* message_ptr and EV_INPUT_MESSAGE returned. If message_ptr, message_set or */
  /* message_id are NULL, then EV_NO_EVENT will be returned */
  /* If any other event is passed in, this is simply returned */
  event_in = rr_extract_message_header_details(
    event_in,
    message_ptr,
    &message_set,
    &message_id
  );

  while ( event_in != EV_NO_EVENT )
  {
    if ( event_in == EV_RESET_SOFTWARE )
    {
      rr_dtm_data_ptr->data_ptr->substate.packet_assign_control = DTM_PKT_ASSIGN_INACTIVE;
      event_in = EV_NO_EVENT;
      continue;
    }

    switch ( rr_dtm_data_ptr->data_ptr->substate.packet_assign_control )
    {
      /*****************************************************************/
      /* DTM_PKT_ASSIGN_INACTIVE                                       */
      /*                                                               */
      /*****************************************************************/
      case DTM_PKT_ASSIGN_INACTIVE:
      {
        switch ( event_in )
        {
          case EV_DTM_ASSIGNMENT_READY:
          {
            // Construct the GRR_MAC_DTM_CHANNEL_ASSIGN_REQ and send to MAC
            send_dtm_channel_assign_req(gas_id);

            rr_dtm_data_ptr->data_ptr->substate.packet_assign_control = DTM_PKT_ASSIGN_AWAITING_ASSIGN_CNF;
            break;
          }

          default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_PKT_ASSIGN_AWAITING_ASSIGN_CNF                            */
      /*                                                               */
      /*****************************************************************/
      case DTM_PKT_ASSIGN_AWAITING_ASSIGN_CNF:
      {
        switch ( event_in )
        {
          case EV_DTM_UL_TBF_REQ:
          {
            // A MAC_GRR_UL_TBF_REQ has crossed with the GRR_MAC_DTM_CHANNEL_ASSIGN_REQ.
            // Whether the Packet Assignment is configured succesfully or not, RLC will be blocked by MAC
            // and therefore the UL will be re-requested when unblocked
            MSG_GERAN_HIGH_0_G("UL request ignored");
            break;
          }

          case EV_INPUT_MESSAGE:
          {
            if ( (message_set == MS_RR_L1) &&
                 (message_id == (byte) MPH_DTM_CHANNEL_ASSIGN_CNF) )
            {
              // Clear any saved UL request
              save_mac_grr_ul_tbf_req(NULL, gas_id);

              rr_dtm_data_ptr->data_ptr->substate.packet_assign_control = DTM_PKT_ASSIGN_INACTIVE;
              event_out = EV_DTM_CSPS_EST_CNF;
            }

            else
            if ( (message_set == MS_MAC_RR) &&
                 (message_id == (byte) MAC_GRR_DTM_CHANNEL_ASSIGN_REJ) )
            {
              // Clear any saved UL request
              save_mac_grr_ul_tbf_req(NULL, gas_id);

              rr_dtm_data_ptr->data_ptr->substate.packet_assign_control = DTM_PKT_ASSIGN_INACTIVE;
              event_out = EV_DTM_ASSIGNMENT_ERROR_PS_BLOCKED;
            }

            break;
          }

          case EV_DTM_CS_RELEASE_REQUIRED:
          {
            // Wait for L1 to confirm DTM transfer or MAC to reject the assignment
            rr_dtm_data_ptr->data_ptr->substate.packet_assign_control =
              DTM_PKT_ASSIGN_AWAITING_L1_CNF_REL_PENDING;

            log_dtm_event(RR_DTM_DBG_AWAITING_L1_CNF, gas_id);
            break;
          } /* EV_DTM_CS_RELEASE_REQUIRED */

          default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_PKT_ASSIGN_AWAITING_L1_CNF_REL_PENDING                    */
      /*                                                               */
      /*****************************************************************/
      case DTM_PKT_ASSIGN_AWAITING_L1_CNF_REL_PENDING:
      {
        switch ( event_in )
        {
          case EV_DTM_UL_TBF_REQ:
          {
            // A MAC_GRR_UL_TBF_REQ has crossed with the GRR_MAC_DTM_CHANNEL_ASSIGN_REQ.
            // Whether the Packet Assignment is configured succesfully or not, RLC will be blocked by MAC
            // and therefore the UL will be re-requested when unblocked
            MSG_GERAN_HIGH_0_G("UL request ignored");
            break;
          }

          case EV_INPUT_MESSAGE:
          {
            if ( (message_set == MS_RR_L1) &&
                 (message_id == (byte) MPH_DTM_CHANNEL_ASSIGN_CNF) )
            {
              rr_dtm_data_ptr->data_ptr->substate.packet_assign_control = DTM_PKT_ASSIGN_INACTIVE;

              // MAC is released with GRR_MAC_DTM_CHANNEL_RELEASE_IND following GRR_MAC_DTM_CHANNEL_ASSIGN_REQ
              rr_send_grr_mac_dtm_channel_release_ind(NORMAL_REL, gas_id);

              // L1 is in DTM so indicate DTM release
              event_out = EV_DTM_PERFORM_CSPS_RELEASE;
            }

            else
            if ( (message_set == MS_MAC_RR) &&
                 (message_id == (byte) MAC_GRR_DTM_CHANNEL_ASSIGN_REJ) )
            {
              rr_dtm_data_ptr->data_ptr->substate.packet_assign_control = DTM_PKT_ASSIGN_INACTIVE;

              // L1 is not in DTM so indicate GSM release
              event_out = EV_DTM_PERFORM_GSM_RELEASE;
            }

            break;
          }

          case EV_DTM_CHN_ASSIGN_REQ_GUARD_TIMER:
          {
            MSG_GERAN_FATAL_0_G("CHN_ASSIGN_REQ Guard Timer expired");
            break;
          } /* EV_DTM_CHN_ASSIGN_REQ_GUARD_TIMER */

          default: ;
        }

        break;
      }

      default:
      {
        MSG_GERAN_ERROR_1_G(
          "In unknown state (%d)",
          (int)rr_dtm_data_ptr->data_ptr->substate.packet_assign_control
        );
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((event_in != new_event) && (new_event != EV_NO_EVENT)) ||
        (rr_dtm_data_ptr->data_ptr->old_substate.packet_assign_control !=
         rr_dtm_data_ptr->data_ptr->substate.packet_assign_control))
    {
      rr_store_trace_msg_buf(
        RR_DTM_PACKET_ASSIGN_SM,
        new_event,
        (byte)rr_dtm_data_ptr->data_ptr->substate.packet_assign_control,
        message_ptr,
        gas_id
      );
    }
#endif

    if ( new_event != EV_NO_EVENT )
    {
      event_in = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event_in = EV_NO_EVENT;
    }

    if (rr_dtm_data_ptr->data_ptr->substate.packet_assign_control !=
        rr_dtm_data_ptr->data_ptr->old_substate.packet_assign_control)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_dtm_packet_assign_control",
        rr_dtm_data_ptr->data_ptr->old_substate.packet_assign_control,
        rr_dtm_data_ptr->data_ptr->substate.packet_assign_control,
        get_dtm_packet_assign_control_state_name,
        gas_id
      );

      rr_dtm_data_ptr->data_ptr->old_substate.packet_assign_control =
        rr_dtm_data_ptr->data_ptr->substate.packet_assign_control;
    }
  }

  return event_out;

} /* end dtm_packet_assign_control() */


/*============================================================================

FUNCTION       respond_to_dtm_assignment_command

DESCRIPTION    Decode & process a DTM ASSIGNMENT COMMAND. If there is an error,
               a DTM ASSIGN FAILURE is sent OTA to the network

DEPENDENCIES   None

PARAMETERS     event_in
               message_ptr

RETURN VALUE   EV_DTM_ASSIGNMENT_COMMAND_CHECKED
               EV_DTM_ASSIGNMENT_ERROR

SIDE EFFECTS   None

=============================================================================*/
static rr_event_T respond_to_dtm_assignment_command(const gas_id_t gas_id)
{
  RR_cause_T error_cause;
  sys_band_T new_band;
  rr_event_T return_event;

  error_cause = rr_dtm_process_dtm_assignment_command(
    &new_band,
    DTM_PS_ASSIGNMENT_REQUIRED,
    gas_id
  );

  if (error_cause == RR_NORMAL_EVENT)
  {
    rr_set_current_dedicated_band((rr_internal_band_T)new_band, gas_id);

    return_event = EV_DTM_ASSIGNMENT_COMMAND_CHECKED;
  }
  else
  {
    /* Send a DTM ASSIGN FAILURE back to the network */
    send_dtm_assign_failure_ota(
      error_cause,    // cause
      FALSE,          // l1_reconnected
      FALSE,          // l2_suspended
      gas_id
    );

    return_event = EV_DTM_ASSIGNMENT_ERROR;
  }

  return return_event;
} /* end respond_to_dtm_assignment_command() */

/**
 * Determines whether a msg indicates that GL2 establishment has failed
 *
 * @param msg_set
 * @param msg_id
 * @param msg_ptr
 *
 * @return rr_event_T
 */
static boolean dtm_msg_indicates_l2_establishment_failure(byte msg_set, byte msg_id, rr_cmd_bdy_type *msg_ptr)
{
  if ((MS_RR_L2 == msg_set) && (msg_ptr != NULL))
  {
    if (msg_id == (byte)DL_RELEASE_IND)
    {
      return(TRUE);
    }
    else if (msg_id == (byte)MDL_ERROR_IND)
    {
      mdl_error_ind_T *ind_ptr = (mdl_error_ind_T *)msg_ptr;

      if (UNSOLICITED_DM_RESPONSE_PAR == ind_ptr->error_code)
      {
        return(TRUE);
      }
    }
  }

  return(FALSE);
}

/**
 * DTM module sub-state machine for processing DTM ASSIGNMENT COMMAND
 *
 * @param event_in
 * @param message_ptr
 *
 * @return rr_event_T
 */
static rr_event_T dtm_assign_command_control(rr_event_T event_in, void *message_ptr, const gas_id_t gas_id)
{
  byte               message_id;
  byte               message_set;
  rr_event_T         event_out = EV_NO_EVENT;
  rr_event_T         new_event = EV_NO_EVENT;
  gprs_scell_info_t *scell_info_ptr;
  rr_dedicated_data_T *dedic_ptr;
  rr_dtm_static_data_t *rr_dtm_data_ptr;

  // Obtain a pointer to the module data
  rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_info_ptr);

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(dedic_ptr);

  // There should be nothing passed into this state machine until the control/data structures have been setup
  RR_ASSERT_FATAL(rr_dtm_data_ptr->data_ptr != NULL);

  /* If EV_INPUT_MESSAGE is passed in, message_set & message_id will be setup from */
  /* message_ptr and EV_INPUT_MESSAGE returned. If message_ptr, message_set or */
  /* message_id are NULL, then EV_NO_EVENT will be returned */
  /* If any other event is passed in, this is simply returned */
  event_in = rr_extract_message_header_details(
    event_in,
    message_ptr,
    &message_set,
    &message_id
  );

  while ( event_in != EV_NO_EVENT )
  {
    if ( event_in == EV_RESET_SOFTWARE )
    {
      rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_INACTIVE;
      rr_dtm_data_ptr->data_ptr->old_band = BAND_NONE;
      event_in = EV_NO_EVENT;
      continue;
    }

    if ((event_in == EV_INPUT_MESSAGE) && (message_ptr == NULL))
    {
      event_in = EV_NO_EVENT;
      continue;
    }

    switch ( rr_dtm_data_ptr->data_ptr->substate.assign_command_control )
    {
      /*****************************************************************/
      /* DTM_ASSIGN_CMD_INACTIVE                                       */
      /*                                                               */
      /*****************************************************************/
      case DTM_ASSIGN_CMD_INACTIVE:
      {
        switch ( event_in )
        {
          case EV_DTM_ASSIGNMENT_READY:
          case EV_DTM_ASSIGNMENT_READY_IN_DTM:
          {
            /* Save the current dedicated band - if the dedicated band does
            not change as a result of the DTM assignment command, SACCH SIs
            remain valid and do not need to be reacquired. */
            rr_dtm_data_ptr->data_ptr->old_band = rr_get_current_dedicated_band(gas_id);

            rr_dtm_data_ptr->data_ptr->assign_command_control_data.l1_in_dtm = FALSE;
            rr_dtm_data_ptr->data_ptr->assign_command_control_data.ps_access_blocked = FALSE;

            if (event_in == EV_DTM_ASSIGNMENT_READY_IN_DTM)
            {
              rr_dtm_data_ptr->data_ptr->assign_command_control_data.l1_in_dtm = TRUE;
            }

            if (rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode_present)
            {
              rr_eng_mode_update_codec_ho_info(
                dedic_ptr->curr_channel_spec.channel_mode_1,
                rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode,
                rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.channel_type,
                FALSE,
                SYS_HO_START,
                SYS_HO_TYPE_GSM_GSM,
                gas_id
              );

              rr_eng_mode_update_codec_ho_info(
                dedic_ptr->curr_channel_spec.channel_mode_1,
                rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode,
                rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.channel_type,
                FALSE,
                SYS_SPEECH_CODEC_INFO,
                SYS_HO_TYPE_GSM_GSM,
                gas_id
              );
            }

            /* SAPI3 link may be pending or established, but any action */
            /* required must be decided by the separate RR SAPI3 module */
            rr_release_sapi3(gas_id);

            /* DTM ASSIGNMENT COMMAND contents ok, so suspend L2 link */
            rr_send_dl_suspend_req(RR_L1_GSM_GSM, gas_id);

            /* Update the L2 link status tracking */
            rr_set_link_status(NO_LINK, gas_id);

            rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_SUSPENDING_LINK;
            break;
          }

          default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_ASSIGN_CMD_SUSPENDING_LINK                                */
      /*                                                               */
      /*****************************************************************/
      case DTM_ASSIGN_CMD_SUSPENDING_LINK:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            if ( (message_set == MS_RR_L1) &&
                 (message_id == (byte) MPH_DTM_CHANNEL_RELEASE_IND) )
            {
              rr_dtm_data_ptr->data_ptr->assign_command_control_data.l1_in_dtm = FALSE;
            }

            else
            if ( (message_set == MS_RR_L2) &&
                 (message_id == (byte) DL_SUSPEND_CNF) )
            {
              send_dtm_channel_assign_req(gas_id);

              /* L1 stops using DTX on receipt of a DTM CHANNEL ASSIGNMENT. */
              /* Keep the DTX indication FALSE until the procedure is complete */
              rr_dtx_rr_override_on(gas_id);

              rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_AWAITING_ASSIGN_CNF;
            }

            break;
          }

          case EV_DTM_CS_RELEASE_REQUIRED:
          {
            /* No DTM connection exists, so indicate GSM release. */
            event_out = EV_DTM_PERFORM_GSM_RELEASE;
            break;
          } /* EV_DTM_CS_RELEASE_REQUIRED */

          default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_ASSIGN_CMD_AWAITING_ASSIGN_CNF                            */
      /*                                                               */
      /*****************************************************************/
      case DTM_ASSIGN_CMD_AWAITING_ASSIGN_CNF:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            if ( (message_set == MS_RR_L1) &&
                 (message_id == (byte) MPH_DTM_CHANNEL_RELEASE_IND) )
            {
              rr_dtm_data_ptr->data_ptr->assign_command_control_data.l1_in_dtm = FALSE;
            }

            else
            if ( (message_set == MS_RR_L1) &&
                 (message_id == (byte) MPH_DTM_CHANNEL_ASSIGN_CNF) )
            {
              if (rr_timer_cancel(RR_GRR_CHN_ASSIGN_REQ_GUARD_TIMER, gas_id))
              {
                MSG_GERAN_MED_0_G("Stopped RR_GRR_CHN_ASSIGN_REQ_GUARD_TIMER");
              }

              /* Save the current channel details in case L2 resumption fails, */
              /* and we have to return to the old channel, and resume there    */
              rr_preserve_current_dedicated_config(gas_id);

              /* Update RR's channel information with the new chanel assignment data */
              /* which is now using */
              rr_dtm_start_using_new_channel_spec(gas_id);

              /* Resume L2 link and send "ASSIGNMENT COMPLETE" OTA to the network */
              send_assignment_complete_ota(gas_id);

              /* Update the L2 link status tracking */
              rr_set_link_status(SAPI0_LINK_IN_PROGRESS, gas_id);

              if (!rr_bands_equal(rr_dtm_data_ptr->data_ptr->old_band, rr_get_current_dedicated_band(gas_id)))
              {
                MSG_GERAN_MED_0_G("Frequency band changed after DTM_ASSIGNMENT_COMMAND, clearing SACCH system information");
                rr_clear_si_sacch(gas_id);
                rr_ded_set_measurement_reports_invalid(TRUE, gas_id);
              }
              else
              {
                MSG_GERAN_MED_0_G("Frequency band not changed after DTM_ASSIGNMENT_COMMAND, preserving SACCH system information");

                /*
                Need to ensure that we send an MPH_DEDICATED_UPDATE_REQ
                (based on the old values) to L1 anyway this triggers it
                to en/disable DTX in the vocoder.
                */
                rr_l1_update_dedicated_mode(&scell_info_ptr->gsm, gas_id);
              }

              rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_RESUMING_LINK;
            }

            else
            if ( (message_set == MS_MAC_RR) &&
                 (message_id == (byte) MAC_GRR_DTM_CHANNEL_ASSIGN_REJ) )
            {
              mac_grr_dtm_channel_assign_rej_t * dtm_channel_assign_rej =
                (mac_grr_dtm_channel_assign_rej_t *) message_ptr;

              if (rr_timer_cancel(RR_GRR_CHN_ASSIGN_REQ_GUARD_TIMER, gas_id))
              {
                MSG_GERAN_MED_0_G("Stopped RR_GRR_CHN_ASSIGN_REQ_GUARD_TIMER");
              }

              /* Resume L2 link and send "DTM ASSIGNMENT FAILURE" OTA to the network */
              /* (with cause indicated by MAC) */
              send_dtm_assign_failure_ota(
                (RR_cause_T) dtm_channel_assign_rej->RR_cause,    // cause
                FALSE,                                            // l1_reconnected
                TRUE,                                             // l2_suspended
                gas_id                                            // gas_id
              );

              // If not in Packet Transfer, then MAC will block RLC when rejecting a DTM Assignment
              if (FALSE == rr_dtm_data_ptr->data_ptr->assign_command_control_data.l1_in_dtm)
              {
                rr_dtm_data_ptr->data_ptr->assign_command_control_data.ps_access_blocked = TRUE;
              }

              rr_dtm_data_ptr->data_ptr->substate.assign_command_control =
                DTM_ASSIGN_CMD_RESUMING_LINK_AFTER_REJECT;
            }

            break;
          }

          case EV_DTM_CS_RELEASE_REQUIRED:
          {
            // Wait for L1 to connect the DTM assignment and then proceed with the release
            rr_dtm_data_ptr->data_ptr->substate.assign_command_control =
              DTM_ASSIGN_CMD_AWAITING_L1_CNF_REL_PENDING;

            log_dtm_event(RR_DTM_DBG_AWAITING_L1_CNF, gas_id);
            break;
          } /* EV_DTM_CS_RELEASE_REQUIRED */

          
          case EV_DTM_CHN_ASSIGN_REQ_GUARD_TIMER:
          {
            MSG_GERAN_FATAL_0_G("CHN_ASSIGN_REQ Guard Timer expired");
            break;
          } /* EV_DTM_CHN_ASSIGN_REQ_GUARD_TIMER */

          default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_ASSIGN_CMD_RESUMING_LINK                                  */
      /*                                                               */
      /*****************************************************************/
      case DTM_ASSIGN_CMD_RESUMING_LINK:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            if ( (message_set == MS_RR_L2) &&
                 (message_id == (byte) DL_RESUME_CNF) )
            {
              /* Update the L2 link status tracking */
              rr_set_link_status(SAPI0_LINK, gas_id);

              /* Reset S_counter to RADIO_LINK_TIMEOUT */
              rr_reset_S_counter(gas_id);

              /* Revert the DTX indication to match the network broadcast value */
              /* (will take effect on the next SI5/6 reception) */
              rr_dtx_rr_override_off(gas_id);

#ifdef FEATURE_GSM_BAND_AVOIDANCE
              if (rr_channel_mode_is_voice(rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode))
              {
                rr_band_avoid_ded_freq_upd_send_imsg(&rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.frequency_list, gas_id);
              }
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

              rr_eng_mode_update_codec_ho_info(
                dedic_ptr->prev_channel_spec.channel_mode_1,
                rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode,
                rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.channel_type,
                FALSE,
                SYS_HO_COMPLETE,
                SYS_HO_TYPE_GSM_GSM,
                gas_id
              );

              rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_INACTIVE;
              event_out = EV_DTM_CSPS_EST_CNF;
            }

            else if (dtm_msg_indicates_l2_establishment_failure(message_set, message_id, message_ptr))
            {
              MSG_GERAN_HIGH_1_G("GL2 message %d indicates resumption failure", (int)message_id);

              rr_send_grr_mac_dtm_channel_release_ind(NORMAL_REL, gas_id);
              rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

              /* Reconnect L1 to the previous channel */
              rr_send_mph_reconnect_channel_req(gas_id);
              rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_RECONNECTING_AFTER_FAILURE;
            }

            else
            if ( (message_set == MS_RR_L1) &&
                 (message_id == (byte) MPH_DTM_CHANNEL_RELEASE_IND) )
            {
              rr_dtm_data_ptr->data_ptr->assign_command_control_data.l1_in_dtm = FALSE;
              rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_RESUMING_LINK_PS_RELEASED;
            }

            break;
          }

          case EV_DTM_CS_RELEASE_REQUIRED:
          {
            // MAC is released with GRR_MAC_DTM_CHANNEL_RELEASE_IND following GRR_MAC_DTM_CHANNEL_ASSIGN_REQ
            rr_send_grr_mac_dtm_channel_release_ind(NORMAL_REL, gas_id);

            // L1 has responded with MPH_DTM_CHANNEL_ASSIGN_CNF so indicate DTM release
            event_out = EV_DTM_PERFORM_CSPS_RELEASE;
            break;
          } /* EV_DTM_CS_RELEASE_REQUIRED */

          default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_ASSIGN_CMD_RESUMING_LINK_PS_RELEASED                      */
      /*                                                               */
      /*****************************************************************/
      case DTM_ASSIGN_CMD_RESUMING_LINK_PS_RELEASED:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            if ( (message_set == MS_RR_L2) &&
                 (message_id == (byte) DL_RESUME_CNF) )
            {
              /* Update the L2 link status tracking */
              rr_set_link_status(SAPI0_LINK, gas_id);

              /* Revert the DTX indication to match the network broadcast value */
              /* (will take effect on the next SI5/6 reception) */
              rr_dtx_rr_override_off(gas_id);

#ifdef FEATURE_GSM_BAND_AVOIDANCE
              if (rr_channel_mode_is_voice(rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode))
              {
                rr_band_avoid_ded_freq_upd_send_imsg(&rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.frequency_list, gas_id);
              }
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

              rr_eng_mode_update_codec_ho_info(dedic_ptr->prev_channel_spec.channel_mode_1,
                                               rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode, 
                                               rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.channel_type,
                                               FALSE,
                                               SYS_HO_COMPLETE,
                                               SYS_HO_TYPE_GSM_GSM,
                                               gas_id);

              rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_INACTIVE;

              // If there is a saved UL request pending, ensure RR-DTM-CONTROL knows about it
              if (rr_dtm_data_ptr->data_ptr->saved_ul_tbf_req_valid)
              {
                MSG_GERAN_HIGH_0_G("UL request pending");
                event_out = EV_DTM_CS_EST_CNF_UL_TBF_REQ_PENDING;
              }
              else
              {
                event_out = EV_DTM_CS_EST_CNF;
              }
            }

            else if (dtm_msg_indicates_l2_establishment_failure(message_set, message_id, message_ptr))
            {
              MSG_GERAN_HIGH_1_G("GL2 message %d indicates resumption failure", (int)message_id);

              // Clear any saved UL request
              save_mac_grr_ul_tbf_req(NULL, gas_id);

              rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

              /* Reconnect L1 to the previous channel */
              rr_send_mph_reconnect_channel_req(gas_id);
              rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_RECONNECTING_AFTER_FAILURE;
            }

            break;
          }

          case EV_DTM_CS_RELEASE_REQUIRED:
          {
            // PS has been released so MAC is in Idle and L1 in Dedicated. Perform GSM release
            event_out = EV_DTM_PERFORM_GSM_RELEASE;
            break;
          } /* EV_DTM_CS_RELEASE_REQUIRED */

          case EV_DTM_UL_TBF_REQ:
          {
            MSG_GERAN_HIGH_0_G("UL request saved pending L2 resumption");
            break;
          }

           default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_ASSIGN_CMD_RECONNECTING_AFTER_FAILURE                     */
      /*                                                               */
      /*****************************************************************/
      case DTM_ASSIGN_CMD_RECONNECTING_AFTER_FAILURE:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            if ( (message_set == MS_RR_L1) &&
                 (message_id == (byte) MPH_RECONNECT_CHANNEL_CNF) )
            {
              /* Update the L2 link status tracking */
              rr_set_link_status(SAPI0_LINK_IN_PROGRESS, gas_id);

              /* Restore previously-saved channel spec and AMR config */
              rr_restore_previous_dedicated_config(gas_id);

              /* Resume L2 link and send "LOWER LAYER FAILURE" OTA to the network */
              /* (with cause RR_LOWER_LAYER_FAILURE, see */
              /* 3GPP 44.018 3.4.22.1.1.5 Abnormal cases) */
              send_dtm_assign_failure_ota(
                RR_LOWER_LAYER_FAILURE,    // cause
                TRUE,                      // l1_reconnected
                TRUE,                      // l2_suspended
                gas_id                     // gas_id
              );

              rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_RESUMING_LINK_AFTER_FAILURE;
            }

            break;
          }

          case EV_DTM_CS_RELEASE_REQUIRED:
          {
            // Wait for L1 to reconnect the previous assignment and then proceed with the release
            rr_dtm_data_ptr->data_ptr->substate.assign_command_control =
              DTM_ASSIGN_CMD_AWAITING_L1_CNF_REL_PENDING;

            break;
          } /* EV_DTM_CS_RELEASE_REQUIRED */

          default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_ASSIGN_CMD_RESUMING_LINK_AFTER_FAILURE                    */
      /*                                                               */
      /*****************************************************************/
      case DTM_ASSIGN_CMD_RESUMING_LINK_AFTER_FAILURE:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            if ( (message_set == MS_RR_L2) &&
                 (message_id == (byte) DL_RECONNECT_CNF) )
            {
              /* Update the L2 link status tracking */
              rr_set_link_status(SAPI0_LINK, gas_id);

              /* Reset S_counter to RADIO_LINK_TIMEOUT */
              rr_reset_S_counter(gas_id);

              /* Revert the DTX indication to match the network broadcast value */
              /* (This will take effect on the next reception of SI5/6) */
              rr_dtx_rr_override_off(gas_id);

              rr_eng_mode_update_codec_ho_info(
                dedic_ptr->curr_channel_spec.channel_mode_1,
                rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode,
                rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.channel_type,
                FALSE,
                SYS_HO_FAIL,
                SYS_HO_TYPE_GSM_GSM,
                gas_id
              );

              rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_INACTIVE;
              event_out = EV_DTM_ASSIGNMENT_FAILURE;
            }

            else if (dtm_msg_indicates_l2_establishment_failure(message_set, message_id, message_ptr))
            {
              MSG_GERAN_HIGH_1_G("GL2 message %d indicates reconnection failure", (int)message_id);

              /* Update the L2 link status tracking */
              rr_set_link_status(NO_LINK, gas_id);

              event_out = EV_RADIO_LINK_FAILURE;
            }

            break;
          }

          case EV_DTM_CS_RELEASE_REQUIRED:
          {
            /* No PS connection, so GSM release is performed. */
            event_out = EV_DTM_PERFORM_GSM_RELEASE;
            break;
          } /* EV_DTM_CS_RELEASE_REQUIRED */

          default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_ASSIGN_CMD_RESUMING_LINK_AFTER_REJECT                     */
      /*                                                               */
      /*****************************************************************/
      case DTM_ASSIGN_CMD_RESUMING_LINK_AFTER_REJECT:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            if ( (message_set == MS_RR_L1) &&
                 (message_id == (byte) MPH_DTM_CHANNEL_RELEASE_IND) )
            {
              rr_dtm_data_ptr->data_ptr->assign_command_control_data.l1_in_dtm = FALSE;
            }

            else
            if ( (message_set == MS_RR_L2) &&
                 (message_id == (byte) DL_RESUME_CNF) )
            {
              /* Update the L2 link status tracking */
              rr_set_link_status(SAPI0_LINK, gas_id);

              rr_eng_mode_update_codec_ho_info(
                dedic_ptr->curr_channel_spec.channel_mode_1,
                rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode,
                rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description.channel_type,
                FALSE,
                SYS_HO_FAIL,
                SYS_HO_TYPE_GSM_GSM,
                gas_id
              );

              rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_INACTIVE;

              if (rr_dtm_data_ptr->data_ptr->assign_command_control_data.l1_in_dtm)
              {
                event_out = EV_DTM_ASSIGNMENT_ERROR_IN_DTM;
              }
              else
              {
                if (rr_dtm_data_ptr->data_ptr->assign_command_control_data.ps_access_blocked)
                {
                  event_out = EV_DTM_ASSIGNMENT_ERROR_PS_BLOCKED;
                }
                else
                {
                  event_out = EV_DTM_ASSIGNMENT_ERROR_PS_ENABLED;
                }
              }
            }

            else
            if (dtm_msg_indicates_l2_establishment_failure(message_set, message_id, message_ptr))
            {
              MSG_GERAN_HIGH_1_G("GL2 message %d indicates reconnection failure", (int)message_id);

              /* Update the L2 link status tracking */
              rr_set_link_status(NO_LINK, gas_id);

              event_out = EV_RADIO_LINK_FAILURE;
            }

            break;
          }

          case EV_DTM_CS_RELEASE_REQUIRED:
          {
            if (rr_dtm_data_ptr->data_ptr->assign_command_control_data.l1_in_dtm)
            {
              // L1 has remained in DTM so indicate DTM release
              event_out = EV_DTM_PERFORM_CSPS_RELEASE;
            }
            else
            {
              // L1 was not in DTM (or has dropped out of DTM) so indicate GSM release
              event_out = EV_DTM_PERFORM_GSM_RELEASE;
            }
            break;
          } /* EV_DTM_CS_RELEASE_REQUIRED */

          default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_ASSIGN_CMD_AWAITING_L1_CNF_REL_PENDING                    */
      /*                                                               */
      /*****************************************************************/
      case DTM_ASSIGN_CMD_AWAITING_L1_CNF_REL_PENDING:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            if ( (message_set == MS_RR_L1) &&
                 (message_id == (byte) MPH_DTM_CHANNEL_ASSIGN_CNF) )
            {
              // MAC is released with GRR_MAC_DTM_CHANNEL_RELEASE_IND following GRR_MAC_DTM_CHANNEL_ASSIGN_REQ
              rr_send_grr_mac_dtm_channel_release_ind(NORMAL_REL, gas_id);

              // L1 is in DTM so indicate DTM release mode
              event_out = EV_DTM_PERFORM_CSPS_RELEASE;
            }

            else
            if ( (message_set == MS_RR_L1) &&
                 (message_id == (byte) MPH_RECONNECT_CHANNEL_CNF) )
            {
              // L1 is not in DTM so indicate GSM release mode
              event_out = EV_DTM_PERFORM_GSM_RELEASE;
            }

            else
            if ( (message_set == MS_MAC_RR) &&
                 (message_id == (byte) MAC_GRR_DTM_CHANNEL_ASSIGN_REJ) )
            {
              if (rr_dtm_data_ptr->data_ptr->assign_command_control_data.l1_in_dtm)
              {
                // L1 has remained in DTM so indicate DTM release
                event_out = EV_DTM_PERFORM_CSPS_RELEASE;
              }
              else
              {
                // L1 was not in DTM (or has dropped out of DTM) so indicate GSM release
                event_out = EV_DTM_PERFORM_GSM_RELEASE;
              }
            }

            break;
          }

          case EV_DTM_CHN_ASSIGN_REQ_GUARD_TIMER:
          {
            MSG_GERAN_FATAL_0_G("CHN_ASSIGN_REQ Guard Timer expired");
            break;
          } /* EV_DTM_CHN_ASSIGN_REQ_GUARD_TIMER */

          default: ;
        }

        break;
      }

      default:
      {
        MSG_GERAN_ERROR_1_G(
          "In unknown state (%d)",
          (int)rr_dtm_data_ptr->data_ptr->substate.assign_command_control
        );
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((event_in != new_event) && (new_event != EV_NO_EVENT)) ||
        (rr_dtm_data_ptr->data_ptr->old_substate.assign_command_control !=
         rr_dtm_data_ptr->data_ptr->substate.assign_command_control))
    {
      rr_store_trace_msg_buf(
        RR_DTM_ASSIGN_COMMAND_SM,
        new_event,
        (byte) rr_dtm_data_ptr->data_ptr->substate.assign_command_control,
        message_ptr,
        gas_id
      );
    }
#endif

    if ( new_event != EV_NO_EVENT )
    {
      event_in = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event_in = EV_NO_EVENT;
    }

    if (rr_dtm_data_ptr->data_ptr->substate.assign_command_control !=
        rr_dtm_data_ptr->data_ptr->old_substate.assign_command_control)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_dtm_assign_command_control",
        rr_dtm_data_ptr->data_ptr->old_substate.assign_command_control,
        rr_dtm_data_ptr->data_ptr->substate.assign_command_control,
        get_dtm_assign_command_control_state_name,
        gas_id
      );

      rr_dtm_data_ptr->data_ptr->old_substate.assign_command_control =
        rr_dtm_data_ptr->data_ptr->substate.assign_command_control;
    }
  }

  return event_out;

} /* end dtm_assign_command_control() */


/*============================================================================

FUNCTION       dtm_cell_capability_monitor

DESCRIPTION    Decode & process a DTM ASSIGNMENT COMMAND. If there is an error,
               a DTM ASSIGN FAILURE is sent OTA to the network

DEPENDENCIES   None

PARAMETERS     event_in

RETURN VALUE   EV_DTM_CELL_SUPPORTS_DTM
               EV_DTM_CELL_DOES_NOT_SUPPORT_DTM
               EV_NO_EVENT

SIDE EFFECTS   None

=============================================================================*/
static rr_event_T dtm_cell_capability_monitor(
  rr_event_T event_in,
  const gas_id_t gas_id
)
{
  rr_event_T  event_out = EV_NO_EVENT;
  rr_event_T  new_event = EV_NO_EVENT;

  // Obtain a pointer to the module data
  rr_dtm_static_data_t *rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  // There should be nothing passed into this state machine until the control/data structures have been setup
  RR_ASSERT_FATAL(rr_dtm_data_ptr->data_ptr != NULL);

  while ( event_in != EV_NO_EVENT )
  {
    if ( event_in == EV_RESET_SOFTWARE )
    {
      rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_NOT_SUPPORTED;
      event_in = EV_NO_EVENT;
      continue;
    }

    switch ( rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor )
    {
      /*****************************************************************/
      /* DTM_CELL_DTM_NOT_SUPPORTED                                    */
      /*                                                               */
      /*****************************************************************/
      case DTM_CELL_DTM_NOT_SUPPORTED:
      {
        switch ( event_in )
        {
          case EV_DTM_RR_EST_CNF:
          {
            boolean dtm_support = rr_cell_supports_dtm(gas_id);
            
            if (dtm_support)
            {
              MSG_GERAN_MED_0_G("CS connected - cell supports DTM");

              // Trigger a Cell Update once GMM is resumed
              grr_gprs_cell_update_required_when_gmm_resumed(gas_id);

              rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_SUPPORTED;
              event_out = EV_DTM_CELL_SUPPORTS_DTM;
            }
            else
            {
              MSG_GERAN_MED_0_G("CS connected - cell does not support DTM");
              event_out = EV_DTM_CELL_DOES_NOT_SUPPORT_DTM;
            }

            if (dtm_support != rr_dtm_data_ptr->data_ptr->old_dtm_support)
            {
              /* Send RR_SERVICE_IND_FROM_OTA_MSG to MM */
              grr_update_mm_with_rr_service_ind(gas_id);
            }

            break;
          }

          case EV_INTER_CELL_HANDOVER:
          {
            rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_HANDOVER_FROM_GSM_CELL;
            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            /* Not moving cell, so no change in DTM capability - nothing to do here */
            break;
          }

          case EV_HANDOVER_COMPLETED:
          {
            /* Intra-cell handover completed */
            event_out = EV_DTM_CELL_DOES_NOT_SUPPORT_DTM;
            break;
          }

          case EV_DTM_SI6_RECEIVED:
          {
            gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

            /* Check if the DTM capability of the cell has been enabled */
            if ( cell_info->dtm_sys_info_valid )
            {
              new_event = EV_DTM_CELL_SUPPORTS_DTM;
            }
            break;
          }

          case EV_DTM_CELL_SUPPORTS_DTM:
          {
            MSG_GERAN_HIGH_0_G("Cell now supports DTM");

            // Trigger a Cell Update once GMM is resumed
            grr_gprs_cell_update_required_when_gmm_resumed(gas_id);

            /* Send RR_SERVICE_IND_FROM_OTA_MSG to MM */
            grr_update_mm_with_rr_service_ind(gas_id);

            event_out = EV_DTM_CELL_SUPPORTS_DTM;
            rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_SUPPORTED;
            break;
          }

#ifdef FEATURE_WCDMA
          case EV_DTM_W2G_HO:
          {
            rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_HANDOVER_FROM_GSM_CELL;
            break;
          }

          case EV_DTM_W2G_HO_PS_ATTACHED:
          {
            rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_HANDOVER_FROM_DTM_CELL;
            break;
          }
#endif // FEATURE_WCDMA

#ifdef FEATURE_GSM_EDTM
          case EV_EDTM_ESTABLISHMENT_COMPLETED:
          {
            set_default_dtm_sys_info_if_invalid(gas_id);
            rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_SUPPORTED;
            break;
          }
#endif /* FEATURE_GSM_EDTM */

          default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_CELL_DTM_SUPPORTED                                        */
      /*                                                               */
      /*****************************************************************/
      case DTM_CELL_DTM_SUPPORTED:
      {
        switch ( event_in )
        {
          case EV_INTER_CELL_HANDOVER:
          {
            rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_HANDOVER_FROM_DTM_CELL;
            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            /* Not moving cell, so no change in DTM capability - nothing to do here */
            break;
          }

          case EV_HANDOVER_COMPLETED:
          {
            /* Intra-cell handover completed */
            event_out = EV_DTM_CELL_SUPPORTS_DTM;
            break;
          }

          case EV_DTM_SI6_RECEIVED:
          {
            gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

            if ( ! cell_info->dtm_sys_info_valid )
            {
              MSG_GERAN_HIGH_0_G("Cell no longer supports DTM");

              if (grr_determine_if_gprs_suspension_should_be_sent_ota(gas_id))
              {
                /* The cause value doesn't matter here - DTM_NOT_SUPPORTED will */
                /* be sent as the OTA cause */
                grr_send_gprs_suspension_ota(
                  MM_RR_MO_FULL_SPEECH,   // cause
                  gas_id                  // gas_id
                );
              }

              /* Send RR_SERVICE_IND_FROM_OTA_MSG to MM */
              grr_update_mm_with_rr_service_ind(gas_id);

              event_out = EV_DTM_CELL_DOES_NOT_SUPPORT_DTM;
              rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_NOT_SUPPORTED;
            }
            break;
          }

          default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_CELL_HANDOVER_FROM_GSM_CELL                               */
      /*                                                               */
      /*****************************************************************/
      case DTM_CELL_HANDOVER_FROM_GSM_CELL:
      {
        switch ( event_in )
        {
          case EV_HANDOVER_COMPLETED:
          {
            break;  // no action required
          }

          case EV_DTM_INFORMATION_RECEIVED:
          {
            if (process_dtm_information(gas_id))
            {
              MSG_GERAN_MED_0_G("New cell supports DTM");

              // Send RR_SERVICE_IND_FROM_OTA_MSG to MM
              grr_update_mm_with_rr_service_ind(gas_id);

              // Trigger a Cell Update once GMM is resumed
              grr_gprs_cell_update_required_when_gmm_resumed(gas_id);

              event_out = EV_DTM_CELL_SUPPORTS_DTM;
              rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_SUPPORTED;
            }
            else
            {
              MSG_GERAN_ERROR_0_G("Error decoding DTM INFORMATION - ignored");
            }
            break;
          }

          case EV_DTM_SI6_RECEIVED:
          {
            gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

            if ( cell_info->dtm_sys_info_valid )
            {
              MSG_GERAN_MED_0_G("New cell supports DTM");

              // Trigger a Cell Update once GMM is resumed
              grr_gprs_cell_update_required_when_gmm_resumed(gas_id);

              event_out = EV_DTM_CELL_SUPPORTS_DTM;
              rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_SUPPORTED;
            }
            else
            {
              MSG_GERAN_MED_0_G("New cell does not support DTM");

              event_out = EV_DTM_CELL_DOES_NOT_SUPPORT_DTM;
              rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_NOT_SUPPORTED;
            }

            // Note: MM is updated via RR_SERVICE_IND_FROM_OTA_MSG when SI6 is received
            // for the first time on a new cell in rr_handle_system_info_6 [rr_sys_info.c]
            break;
          }

          /* DTM ASSIGNMENT COMMAND / PACKET ASSIGNMENT received on new cell */
          /* DTM support assumed */
          case EV_DTM_CELL_SUPPORTS_DTM:
          {
            MSG_GERAN_MED_0_G("New cell supports DTM (assumed)");

            /* Send RR_SERVICE_IND_FROM_OTA_MSG to MM */
            grr_update_mm_with_rr_service_ind(gas_id);

            event_out = EV_DTM_CELL_SUPPORTS_DTM;
            rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_SUPPORTED;
            break;
          }

          case RR_EV_G2W_HO_FAIL:
          {
            MSG_GERAN_HIGH_0_G("Return to GSM cell after G2W HO failure");
            rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_NOT_SUPPORTED;
            event_out = EV_DTM_CELL_DOES_NOT_SUPPORT_DTM;
            break;
          }

          default: ;
        }

        break;
      }

      /*****************************************************************/
      /* DTM_CELL_HANDOVER_FROM_DTM_CELL                               */
      /*                                                               */
      /*****************************************************************/
      case DTM_CELL_HANDOVER_FROM_DTM_CELL:
      {
        switch ( event_in )
        {
          case EV_HANDOVER_COMPLETED:
          {
            invalidate_dtm_sys_info(gas_id);
            break;
          }

          case EV_DTM_INFORMATION_RECEIVED:
          {
            if (process_dtm_information(gas_id))
            {
              MSG_GERAN_MED_0_G("New cell supports DTM");

              // Send RR_SERVICE_IND_FROM_OTA_MSG to MM
              grr_update_mm_with_rr_service_ind(gas_id);

              // Trigger a Cell Update once GMM is resumed
              grr_gprs_cell_update_required_when_gmm_resumed(gas_id);

              event_out = EV_DTM_CELL_SUPPORTS_DTM;
              rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_SUPPORTED;
            }
            else
            {
              MSG_GERAN_ERROR_0_G("Error decoding DTM INFORMATION - ignored");
            }
            break;
          }

          case EV_DTM_SI6_RECEIVED:
          {
            gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

            if ( cell_info->dtm_sys_info_valid )
            {
              MSG_GERAN_MED_0_G("New cell supports DTM");

              // Trigger a Cell Update once GMM is resumed
              grr_gprs_cell_update_required_when_gmm_resumed(gas_id);

              event_out = EV_DTM_CELL_SUPPORTS_DTM;
              rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_SUPPORTED;
            }
            else
            {
              MSG_GERAN_MED_0_G("New cell does not support DTM");

              if (grr_determine_if_gprs_suspension_should_be_sent_ota(gas_id))
              {
                /* The cause value doesn't matter here - DTM_NOT_SUPPORTED will */
                /* be sent as the OTA cause */
                /* Note: This call also sets the GPRS Resumption Indicator to FALSE */
                grr_send_gprs_suspension_ota(
                  MM_RR_MO_FULL_SPEECH,   // cause
                  gas_id                  // gas_id
                );
              }

              /*
              GMM will be suspended in rr_dtm_control upon receipt of
              EV_DTM_CELL_DOES_NOT_SUPPORT_DTM
              */
              event_out = EV_DTM_CELL_DOES_NOT_SUPPORT_DTM;
              rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_NOT_SUPPORTED;
            }

            // Note: MM is updated via RR_SERVICE_IND_FROM_OTA_MSG when SI6 is received
            // for the first time on a new cell in rr_handle_system_info_6 [rr_sys_info.c]
            break;
          }

          /* DTM ASSIGNMENT COMMAND / PACKET ASSIGNMENT received on new cell */
          /* DTM support assumed */
          case EV_DTM_CELL_SUPPORTS_DTM:
          {
            MSG_GERAN_MED_0_G("New cell supports DTM (assumed)");

            /* Send RR_SERVICE_IND_FROM_OTA_MSG to MM */
            grr_update_mm_with_rr_service_ind(gas_id);

            event_out = EV_DTM_CELL_SUPPORTS_DTM;
            rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_SUPPORTED;
            break;
          }

          case RR_EV_G2W_HO_FAIL:
          {
            MSG_GERAN_HIGH_0_G("Return to DTM cell after G2W HO failure");
            rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_SUPPORTED;
            event_out = EV_DTM_CELL_SUPPORTS_DTM;
            break;
          }

          default: ;
        }

        break;
      }

      default:
      {
        MSG_GERAN_ERROR_1_G(
          "In unknown state (%d)",
          (int)rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor
        );
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((event_in != new_event) && (new_event != EV_NO_EVENT)) ||
        (rr_dtm_data_ptr->data_ptr->old_substate.cell_capability_monitor !=
         rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor))
    {
      rr_store_trace_msg_buf(
        RR_DTM_CELL_CAPABILITY_SM,
        new_event,
        (byte)rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor,
        NULL,
        gas_id
      );
    }
#endif

    if ( new_event != EV_NO_EVENT )
    {
      event_in = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event_in = EV_NO_EVENT;
    }

    if (rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor !=
        rr_dtm_data_ptr->data_ptr->old_substate.cell_capability_monitor)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_dtm_cell_capability_monitor",
        rr_dtm_data_ptr->data_ptr->old_substate.cell_capability_monitor,
        rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor,
        get_dtm_cell_capability_monitor_state_name,
        gas_id
      );

      rr_dtm_data_ptr->data_ptr->old_substate.cell_capability_monitor =
        rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor;
    }
  }

  return event_out;

} /* end dtm_cell_capability_monitor() */


/**
  @brief Indicates if a DTM PS transfer is currently in progress.
         Note: DTM GTTP transfer is not included

  @return TRUE if in DTM Transfer; FALSE otherwise
 */
static boolean is_ps_transfer_in_progress(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_dtm_static_data_t *rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  if (rr_dtm_data_ptr->state == DTM_DTM_TRANSFER)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* end is_ps_transfer_in_progress() */


/**
  @brief Determines if a UL PS assignment in a DTM ASSIGNMENT COMMAND or PACKET ASSIGNMENT is allowed

  @return TRUE if the UL PS assignment is allowed; FALSE otherwise
 */
static boolean is_ps_ul_assign_allowed(const gas_id_t gas_id)
{
  if (is_ps_transfer_in_progress(gas_id))
  {
    return TRUE;
  }
  else
  if (is_t3148_running(gas_id))
  {
    return TRUE;
  }

  return FALSE;

} /* end is_ps_ul_assign_allowed() */

/*!
 * \brief Checks if the MM GPRS sys info has changed and updates MM if so.
 * 
 * \param gas_id (in)
 */
static void check_if_mm_gprs_sys_info_changed(const gas_id_t gas_id)
{
  boolean mm_gprs_sys_info_changed;
  mm_gprs_sys_info_T mm_gprs_sys_info;

  (void)rr_gprs_get_sys_info_for_mm(&mm_gprs_sys_info, gas_id);

  mm_gprs_sys_info_changed = rr_mm_diff_gprs_sys_info(&mm_gprs_sys_info, gas_id);

  if (mm_gprs_sys_info_changed)
  {
    MSG_GERAN_HIGH_0_G("MM GPRS sys info changed");
    grr_update_mm_with_rr_service_ind(gas_id);
  }
}

/*!
 * \brief Called to reset local data and transition the state to NULL.
 * 
 * \param gas_id (in)
 */
static void rr_dtm_transition_to_null(const gas_id_t gas_id)
{
  rr_dtm_discard_data(gas_id);
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 *---------------------------------------------------------------------------*/

/**
  Allocates memory for the operational data when the module is activated and
  sets initial values
 */
void rr_dtm_activate_init(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_dtm_static_data_t *rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  RR_ASSERT_FATAL(rr_dtm_data_ptr->data_ptr == NULL);

  rr_dtm_data_ptr->data_ptr = GPRS_MEM_CALLOC(sizeof(rr_dtm_data_t));

  RR_NULL_CHECK_FATAL(rr_dtm_data_ptr->data_ptr);

  /* Setup initial substate-machine states and other initial values */
  rr_dtm_data_ptr->data_ptr->substate.assign_command_control = DTM_ASSIGN_CMD_INACTIVE;
  rr_dtm_data_ptr->data_ptr->substate.cell_capability_monitor = DTM_CELL_DTM_NOT_SUPPORTED;
  rr_dtm_data_ptr->data_ptr->substate.packet_assign_control = DTM_PKT_ASSIGN_INACTIVE;
  rr_dtm_data_ptr->data_ptr->old_band = BAND_NONE;
  rr_dtm_data_ptr->data_ptr->pending_action = EV_NO_EVENT;
  rr_dtm_data_ptr->data_ptr->old_dtm_support = rr_cell_supports_dtm(gas_id);

  log_dtm_event(RR_DTM_DBG_ACTIVATE, gas_id);

} /* end rr_dtm_activate_init() */


/**
  Discards all operational data
 */
void rr_dtm_discard_data(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_dtm_static_data_t *rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  if (rr_dtm_data_ptr->data_ptr != NULL)
  {
    GPRS_MEM_FREE(rr_dtm_data_ptr->data_ptr);
    rr_dtm_data_ptr->data_ptr = NULL;

    log_dtm_event(RR_DTM_DBG_DEACTIVATE, gas_id);
  }

  rr_dtm_data_ptr->released_in_gttp = FALSE;
  rr_dtm_data_ptr->state            = DTM_NULL;

} /* end rr_dtm_discard_data() */


/*============================================================================

FUNCTION       rr_dtm_gttp_message_received

DESCRIPTION    Called when a OTA message arrives from L2 with a GTTP Protocol
               Discriminator. The encapsulated LLC PDU container is passed to
               GLLC

DEPENDENCIES   None

PARAMETERS     dl_data_ind

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
void rr_dtm_gttp_message_received(dl_data_ind_T *dl_data_ind, const gas_id_t gas_id)
{
  /*
  Check the type of GTTP message received.
  If the MS does not recognise the message, it should be ignored (see 3GPP CR GP-050604)

  L3 message: octet 0  GTTP header
              octet 1  message IEI
  */

  /* Presently only GPRS INFORMATION is recognised */
  if ( dl_data_ind->layer3_message[1] == GPRS_INFORMATION )
  {
    /*
    GPRS INFORMATION:
    octet 0     GTTP header
    octet 1     GPRS INFORMATION IEI
    octet 2     TLLI (MSB)
    octet 3     TLLI
    octet 4     TLLI
    octet 5     TLLI (LSB)
    octet 6+    LLC PDU container
    */
    uint32 tlli;

    tlli = ((uint32)dl_data_ind->layer3_message[2] << 24) +
           ((uint32)dl_data_ind->layer3_message[3] << 16) +
           ((uint32)dl_data_ind->layer3_message[4] <<  8) +
           ((uint32)dl_data_ind->layer3_message[5] <<  0);

    MSG_GERAN_HIGH_0_G("LLC PDU sent to LLC");

    gttp_send_dl_pdu_to_llc(
      tlli,
      (gttp_llc_pdu_container_t *) &dl_data_ind->layer3_message[6],
      gas_id
    );

    log_dtm_event(RR_DTM_DBG_GTTP_DATA_RCVD_OTA, gas_id);
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Unknown GTTP message type ignored");
  }

} /* end rr_dtm_gttp_message_received() */


/*============================================================================

FUNCTION       rr_dtm_start_using_new_channel_spec

DESCRIPTION    This function copies the channel configuration information
               contained in a received DTM ASSIGNMENT COMMAND message to RR's
               internal structures, and configures the vocoder (sends
               RR_SYNC_IND)

DEPENDENCIES   None

PARAMETERS     None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
void rr_dtm_start_using_new_channel_spec(const gas_id_t gas_id)
{
  system_information_database_T *sys_info_ptr;
  rr_dedicated_data_T           *dedic_ptr;
  channel_spec_T                *rr_channel_spec_ptr;
  dtm_channel_assignment_t      *new_channel_assign_ptr;
  gprs_scell_info_t             *scell_info_ptr;
  rr_dtm_static_data_t          *rr_dtm_data_ptr;

  // Obtain a pointer to the module data
  rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_info_ptr);

  /* initialise pointers */
  sys_info_ptr           = &scell_info_ptr->gsm;
  dedic_ptr              = rr_get_dedicated_data_ptr(gas_id);
  rr_channel_spec_ptr    = rr_get_channel_spec(dedic_ptr, gas_id);
  new_channel_assign_ptr = &(rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment);

  if ( rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cell_allocation_present )
  {
    sys_info_ptr->cell_channel_description =
      rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cell_allocation;
    sys_info_ptr->valid_CA_received = RR_CA_RECEIVED;
  }

  rr_channel_spec_ptr->number_of_ded_channels = 1;
  rr_channel_spec_ptr->before_parameters_valid = FALSE;

  if ( new_channel_assign_ptr->channel_mode_present )
  {
    rr_channel_spec_ptr->channel_mode_1 = new_channel_assign_ptr->channel_mode;
  }

  rr_channel_spec_ptr->channel_info_1_after =
    new_channel_assign_ptr->channel_description;

  rr_channel_spec_ptr->cipher_flag = rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cipher_flag;
  rr_channel_spec_ptr->cipher_algorithm = rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cipher_algorithm;
  rr_ciphering_has_started(rr_channel_spec_ptr->cipher_flag, gas_id);

  /* if an AMR config was provided, switch over to it now */
  if (new_channel_assign_ptr->multi_rate_config_provided)
  {
    rr_switch_to_next_multirate_config(dedic_ptr);
  }

  if ( (rr_channel_spec_ptr->channel_info_1_after.channel_type == FULL_RATE_TRAFFIC) ||
       (rr_channel_spec_ptr->channel_info_1_after.channel_type == HALF_RATE_TRAFFIC) )
  {
    rr_send_sync_ind(
      MM_RR_RES_ASS,
      rr_channel_spec_ptr->channel_info_1_after.channel_type,
      rr_channel_spec_ptr->channel_mode_1,
      rr_channel_spec_ptr->channel_info_1_after.subchannel,
      gas_id
    );
  }

  /* log the initial channel configuration */
  (void)rr_log_channel_configuration(gas_id);

} /* end rr_dtm_start_using_new_channel_spec() */


/*============================================================================

FUNCTION       rr_dtm_process_dtm_assignment_command

DESCRIPTION    This function decodes the contents of a DTM ASSIGNMENT COMMAND
               message.
               See 44.018 9.1.12e DTM Assignment Command

DEPENDENCIES   band_ptr      - output pointer to band information from channel
                               description
               csps_required - indication of whether CS or PS allocations must be
                               present

PARAMETERS     None

RETURN VALUE   RR Cause value indicating any error (RR_NORMAL_EVENT if no error)

SIDE EFFECTS   None

=============================================================================*/
RR_cause_T rr_dtm_process_dtm_assignment_command(
  sys_band_T *band_ptr,
  dtm_validation_t csps_required,
  const gas_id_t gas_id
)
{
  gprs_broadcast_information_t  gprs_broadcast_information;
  cell_channel_description_T    cell_allocation;
  cell_channel_description_T   *cell_allocation_ptr;
  channel_information_T         channel_description;
  rr_dedicated_data_T          *dedic_ptr;
  mobile_allocation_T           mobile_allocation;
  ie_descriptor_T              *current_IE;
  channel_mode_T                channel_mode               = SIGNALLING_ONLY;
  RR_cause_T                    error_cause                = RR_NORMAL_EVENT;
  boolean                       cell_allocation_present    = FALSE;
  boolean                       channel_mode_present       = FALSE;
  boolean                       frequency_list_present     = FALSE;
  boolean                       mobile_allocation_present  = FALSE;
  boolean                       multirate_config_present   = FALSE;
  boolean                       decoding_ok                = FALSE;
  uint8                         power_level;
  boolean                       cipher_mode_setting_present = FALSE;
  boolean                       start_ciphering             = FALSE;
  cipher_algorithm_T            cipher_algorithm            = 0;
  uint8                         remaining_length            = 0;
  rr_dtm_static_data_t         *rr_dtm_data_ptr;

  // Obtain a pointer to the module data
  rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(dedic_ptr);
  RR_ZERO_STRUCTURE(gprs_broadcast_information);
  RR_ZERO_STRUCTURE(cell_allocation);
  RR_ZERO_STRUCTURE(channel_description);
  RR_ZERO_STRUCTURE(mobile_allocation);

  /* reset previous details and local structures */
  clear_previous_assignment_details(gas_id);
  rr_invalidate_multirate_config(&(dedic_ptr->next_amr_config));
  power_level = 0;   /* Not necessary, but removes compiler warning */

  /* See 44.018 9.1.12e DTM Assignment Command for message contents and IEI values */
  /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(DTM_ASSIGNMENT_COMMAND, gas_id);

  if ( current_IE )
  {
    /* IE mandatory: "Power Level" */
    if ( current_IE->start_IE )
    {
      power_level = *(current_IE->start_IE) & 0x1F;
      decoding_ok = TRUE;
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Mandatory POWER LEVEL missing");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("message_list parsing error");
  }

  if ( decoding_ok )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* IE mandatory: "Description of the CS Channel" */
      if ( current_IE->start_IE )
      {
        rr_decode_channel_description(
          current_IE->start_IE,
          &channel_description,
          &decoding_ok,
          gas_id
        );

        /* for CS allocation using a single ARFCN, check the band */
        if (channel_description.hopping_flag == FALSE)
        {
          rr_internal_band_T cs_band;

          cs_band = GET_ARFCN_BAND(channel_description.frequency_list.channel[0]);

          if (rr_gapi_is_supported_band((sys_band_T)cs_band, gas_id) == FALSE)
          {
            MSG_GERAN_MED_1_G("DTM: CS channel in unsupported band(%d)", (int)cs_band);
            decoding_ok = FALSE;
          }
        }

        if ( ! decoding_ok )
        {
          MSG_GERAN_ERROR_0_G("Error in Channel Description");
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("Mandatory CS Channel IE missing");
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if ( decoding_ok )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* IE mandatory: "GPRS broadcast information" */
      if ( current_IE->start_IE  )
      {

        remaining_length = rr_length_to_end_of_ota_msg(
          DTM_ASSIGNMENT_COMMAND,       // msg_type
          current_IE->start_IE,         // csn1_start_ie_ptr
          GPRS_BROADCAST_INFORMATION,   // csn_ie_type
          gas_id
        );

        /* The first octet is the length - skip this when decoding */

        decoding_ok = (
          gcsn1_rr_decoder(
            GPRS_BROADCAST_INFORMATION,     // message_type
            &current_IE->start_IE[1],       // csn1_ie_src
            &gprs_broadcast_information,    // dst
            remaining_length,               // len_to_end_of_buf
            gas_id
          ) == CSN_DECODE_NO_ERROR);

        if ( ! decoding_ok )
        {
          MSG_GERAN_ERROR_0_G("Error in GPRS Broadcast Information");
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("Mandatory GPRS Broadcast Information IE missing");
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if ( decoding_ok )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* IE optional: Cell Channel Description (IEI=0x10) */
      if ( current_IE->start_IE )
      {
        /* CELL CHANNEL DESCRIPTION IEI = 0x10 */
        if ( current_IE->IEI == 0x10 )
        {
          rr_decode_cell_channel_description(
            current_IE->start_IE,
            &cell_allocation,
            &decoding_ok,
            gas_id
          );

          if ( decoding_ok )
          {
            cell_allocation_present = TRUE;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Error in Cell Channel Description");
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("IEI (%d) not CELL CHANNEL DESCRIPTION",(int)current_IE->IEI);
        }
      }
      else
      {
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if ( decoding_ok  )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* IE optional: Channel Mode (IEI=0x11) */
      if ( current_IE->start_IE )
      {
        /* CHANNEL MODE IEI = 0x11 */
        if ( current_IE->IEI == 0x11 )
        {
          rr_decode_channel_mode(
            current_IE->start_IE,
            &channel_mode,
            &decoding_ok
          );

          if ( decoding_ok )
          {
            channel_mode_present = TRUE;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Error in Channel Mode");
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("IEI (%d) not RR PACKET UPLINK ASSIGNMENT",(int)current_IE->IEI);
        }
      }
      else
      {
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if ( decoding_ok  )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* IE conditional: Frequency List (IEI=12) */
      if ( current_IE->start_IE )
      {
        /* FREQUENCY LIST IEI = 0x12 */
        if ( current_IE->IEI == 0x12 )
        {
          rr_decode_frequency_list(
            current_IE->start_IE,
            rr_gprs_get_scell_arfcn(gas_id),
            &channel_description.frequency_list,
            &decoding_ok,
            gas_id
          );

          if ( decoding_ok )
          {
            frequency_list_present = TRUE;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Error in Frequency List");
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("IEI (%d) not FREQUENCY LIST",(int)current_IE->IEI);
        }
      }
      else
      {
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if ( decoding_ok  )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* Next IE conditional: Mobile Allocation (IEI=13) */
      if ( current_IE->start_IE )
      {
        /* MOBILE ALLOCATION IEI = 0x13 */
        if ( current_IE->IEI == 0x13 )
        {
          rr_decode_mobile_allocation(
            current_IE->start_IE,
            &mobile_allocation
          );

          decoding_ok = TRUE;
          mobile_allocation_present = TRUE;
        }
        else
        {
          MSG_GERAN_ERROR_1_G("IEI (%d) not MOBILE ALLOCATION",(int)current_IE->IEI);
        }
      }
      else
      {
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if ( decoding_ok )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* Next IE optional: "RR Packet Uplink Assignment" */
      if ( current_IE->start_IE )
      {
        /* RR PACKET UPLINK ASSIGNMENT IEI = 0x15 */
        if ( current_IE->IEI == 0x15 )
        {
          MSG_GERAN_MED_0_G("RR PACKET UPLINK ASSIGNMENT IEI present");

          remaining_length = rr_length_to_end_of_ota_msg(
            DTM_ASSIGNMENT_COMMAND,         // msg_type
            current_IE->start_IE,           // csn1_start_ie_ptr
            RR_PACKET_UPLINK_ASSIGNMENT,    // csn_ie_type
            gas_id
          );

          decoding_ok = (
            gcsn1_rr_decoder(
              RR_PACKET_UPLINK_ASSIGNMENT,                              // message_type
              current_IE->start_IE,                                     // csn1_ie_src
              &rr_dtm_data_ptr->dtm_assignment_details.ps_ul_assign,    // dst
              remaining_length,                                         // len_to_end_of_buf
              gas_id
            ) == CSN_DECODE_NO_ERROR);

          if ( decoding_ok )
          {
            rr_dtm_data_ptr->dtm_assignment_details.ps_ul_assign_present = TRUE;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Error in RR Packet Uplink Assign");
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("IEI (%d) not RR PACKET UPLINK ASSIGNMENT",(int)current_IE->IEI);
        }
      }
      else
      {
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if ( decoding_ok )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* Next IE optional: "RR Packet Downlink Assignment" */
      if ( current_IE->start_IE )
      {
        /* RR PACKET DOWNLINK ASSIGNMENT IEI = 0x16 */
        if ( current_IE->IEI == 0x16 )
        {
          MSG_GERAN_MED_0_G("RR PACKET DOWNLINK ASSIGNMENT IEI present");

          remaining_length = rr_length_to_end_of_ota_msg(
            DTM_ASSIGNMENT_COMMAND,           // msg_type
            current_IE->start_IE,             // csn1_start_ie_ptr
            RR_PACKET_DOWNLINK_ASSIGNMENT,    // csn_ie_type
            gas_id
          );

          decoding_ok = (
            gcsn1_rr_decoder(
              RR_PACKET_DOWNLINK_ASSIGNMENT,                            // message_type
              current_IE->start_IE,                                     // csn1_ie_src
              &rr_dtm_data_ptr->dtm_assignment_details.ps_dl_assign,    // dst
              remaining_length,                                         // len_to_end_of_buf
              gas_id
            ) == CSN_DECODE_NO_ERROR);

          if ( decoding_ok )
          {
            rr_dtm_data_ptr->dtm_assignment_details.ps_dl_assign_present = TRUE;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Error in RR Packet Downlink Assign");
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("IEI (%d) not RR PACKET DOWNLINK ASSIGNMENT",(int)current_IE->IEI);
        }
      }
      else
      {
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if ( decoding_ok )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* Next IE optional: "Multi-Rate configuration" */
      if ( current_IE->start_IE )
      {
        /* MULTIRATE CONFIGURATION IEI = 0x17 */
        if ( current_IE->IEI == 0x17 )
        {
          rr_decode_multirate_config_ie(
            current_IE->start_IE,
            &(dedic_ptr->next_amr_config),
            &decoding_ok,
            channel_mode,
            gas_id
          );

          if ( decoding_ok )
          {
            multirate_config_present = TRUE;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Error in MultiRate Configuration");
            error_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("IEI (%d) not MULTIRATE CONFIGURATION",(int)current_IE->IEI);
        }
      }
      else
      {
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if ( decoding_ok )
  {
    /* Point to next IE in message */
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if ( current_IE )
    {
      /* Next IE optional: "Ciphering Mode Setting" */
      if ( current_IE->start_IE )
      {
        /* CIPHER_MODE_SETTING_IEI = 0x90 */
        if ( current_IE->IEI == CIPHER_MODE_SETTING_IEI )
        {
          start_ciphering = *(current_IE->start_IE) & 0x01;
          cipher_algorithm = (cipher_algorithm_T) (*(current_IE->start_IE) & 0x0E) >> 1;
          cipher_mode_setting_present = TRUE;
          decoding_ok = TRUE;
        }
        else
        {
          MSG_GERAN_ERROR_1_G("IEI (%d) not CIPHER_MODE_SETTING_IEI",(int)current_IE->IEI);
        }
      }
      else
      {
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if ( decoding_ok )
  {
    gprs_scell_info_t *cell_info = rr_gprs_get_scell_info(gas_id);

    MSG_GERAN_HIGH_0_G("Decoding of DTM ASSIGNMENT COMMAND successful");

    /* Now perform some sanity checks on the DTM ASSIGNMENT COMMAND contents */

    if (rr_channel_type_supported(channel_description.channel_type, gas_id) == FALSE)
    {
      error_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
      MSG_GERAN_HIGH_1_G("Channel type is unacceptable=%d",(int)channel_description.channel_type);
    }
    else if ((channel_description.channel_type == SDCCH_4) || (channel_description.channel_type == SDCCH_8))
    {
      if (csps_required == DTM_PS_ASSIGNMENT_REQUIRED)
      {
        /*CR 95044 According to 3GPP CR GP-042768, mobile should not accept the DTM ASSIGNMENT COMMAND
        with SDCCH as CS resource*/
        error_cause = RR_PROTOCOL_ERROR_UNSPECIFIED;
        MSG_GERAN_HIGH_1_G("Channel type is unacceptable=%d",(int)channel_description.channel_type);
      }
    }

    if ( error_cause == RR_NORMAL_EVENT )
    {
      /* If the channel description indicates hopping, then either a frequency list */
      /* or a mobile allocationmust be present to provide the hopping frequencies */
      if ( channel_description.hopping_flag )
      {
        if ( ( frequency_list_present == FALSE ) &&
             ( mobile_allocation_present == FALSE ) )
        {
          error_cause = RR_CONDITIONAL_IE_ERROR;
        }
        else
        if ( frequency_list_present )
        {
          /* This returns TRUE if a frequency is out-of-bands */
          if (rr_check_frequency_list(&channel_description.frequency_list, gas_id))
          {
            error_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
          }
        }
        else
        if ( mobile_allocation_present )
        {
          /* To decode the Mobile Allocation, a Cell Allocation is required. If this */
          /* wasn't supplied, then use the existing Cell Allocation is the system */
          /* information database */
          if ( cell_allocation_present )
          {
            cell_allocation_ptr = &cell_allocation;
          }
          else
          if (( cell_info->gsm.valid_CA_received == RR_CA_RECEIVED) ||
             ( cell_info->gsm.valid_CA_received == RR_CA_RECEIVED_BUT_INVALID))
          {
            cell_allocation_ptr = &cell_info->gsm.cell_channel_description;
          }
          /* If there is neither a Cell Allocation included in the message, nor a valid */
          /* Cell Allocation previously received in a system information message, then */
          /* the DTM ASSIGNMENT COMMAND cannot be used, so is rejected */
          else
          {
            cell_allocation_ptr = NULL;
          }

          if ( cell_allocation_ptr )
          {
            if (!rr_set_channel_frequency_list_from_mobile_allocation(
                   &channel_description,
                   &mobile_allocation,
                   cell_allocation_ptr,
                   gas_id
                 )
               )
            {
              /* TODO: Check which error cause to return */
              error_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
            }
          }
          else
          {
            error_cause = RR_NO_CELL_ALLOCATION_AVAILABLE;
          }
        }
      }
    }

    if ( error_cause == RR_NORMAL_EVENT )
    {
      if (csps_required == DTM_PS_ASSIGNMENT_REQUIRED)
      {
        if ( (! rr_dtm_data_ptr->dtm_assignment_details.ps_ul_assign_present) &&
             (! rr_dtm_data_ptr->dtm_assignment_details.ps_dl_assign_present) )
        {
          MSG_GERAN_ERROR_0_G("DTM ASSIGNMENT COMMAND contains no PS assign");
          error_cause = RR_PROTOCOL_ERROR_UNSPECIFIED;
        }
        else
        if (rr_dtm_data_ptr->dtm_assignment_details.ps_ul_assign_present &&
            (!is_ps_ul_assign_allowed(gas_id)))
        {
          MSG_GERAN_ERROR_0_G("DTM ASSIGNMENT COMMAND contains UL PS assign but none expected / allowed");
          error_cause = RR_PROTOCOL_ERROR_UNSPECIFIED;
        }
      }
    }

    if ( error_cause == RR_NORMAL_EVENT )
    {
      /* check compatibility of channel mode and multirate configuration */
      if ((IS_AMR_CHANNEL_MODE(channel_mode)) &&
          (multirate_config_present == FALSE))
      {
        channel_spec_T *channel_spec_ptr = rr_get_channel_spec(dedic_ptr, gas_id);

        /* DTM Assignment has requested an AMR codec mode,   */
        /* but no multirate config was supplied in the msg:  */
        /* check the existing config to see if the multirate */
        /* information can be inherited for this assignment. */
        if ((IS_AMR_CHANNEL_MODE(channel_spec_ptr->channel_mode_1)) &&
            (dedic_ptr->curr_amr_config.valid == TRUE))
        {
          if (channel_mode == channel_spec_ptr->channel_mode_1)
          {
            /* existing channel config is using AMR, so inherit the parameters */
            rr_use_existing_multirate_config(dedic_ptr);
            multirate_config_present = TRUE;
          }
          else
          {
            /* AMR channel mode has changed but no multirate config was provided */
            error_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
            MSG_GERAN_HIGH_2_G("Current AMR type = %d New AMR type = %d",(int) channel_spec_ptr->channel_mode_1,
                      (int)channel_mode);
            MSG_GERAN_ERROR_0_G("No multirate config in DTM ASSIGNMENT COMMAND");
          }
        }
        else
        {
          /* existing channel config has no valid multirate config */
          MSG_GERAN_MED_0_G("Multirate config not available from previous assignment");
          error_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
        }
      }
    }

    if ( error_cause == RR_NORMAL_EVENT )
    {
      /* validate the multirate config against the channel type */
      if (multirate_config_present)
      {
#ifdef FEATURE_VAMOS_II
        if ((channel_mode == SPEECH_V3 || channel_mode == SPEECH_V3_VAMOS2) &&
            (dedic_ptr->next_amr_config.codec_modes & 0xC0) &&
            (channel_description.channel_type == HALF_RATE_TRAFFIC))
#else
        if ((channel_mode == SPEECH_V3)                   &&
            (dedic_ptr->next_amr_config.codec_modes & 0xC0) &&
            (channel_description.channel_type == HALF_RATE_TRAFFIC))
#endif /*FEATURE_VAMOS_II*/
        {
          MSG_GERAN_MED_0_G("Channel mode not compatible with half rate channel type");
          error_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
        }
      }
#ifdef FEATURE_GSM_AMR_WB
      /* validate channel type against channel mode*/
#ifdef FEATURE_VAMOS_II
      if ((channel_mode == SPEECH_V5 || channel_mode == SPEECH_V5_VAMOS2) &&
          (channel_description.channel_type == HALF_RATE_TRAFFIC))
#else
      if ((channel_mode == SPEECH_V5) &&
          (channel_description.channel_type == HALF_RATE_TRAFFIC))
#endif /*FEATURE_VAMOS_II*/
      {
        MSG_GERAN_MED_0_G("Channel mode not compatible with half rate channel type");
        error_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
      }
#endif /*FEATURE_GSM_AMR_WB*/
    }

    if ( error_cause == RR_NORMAL_EVENT )
    {
      if (cipher_mode_setting_present)
      {
        if (start_ciphering)
        {
          if (rr_get_ciphering_mode_command_received(gas_id) == FALSE)
          {
            MSG_GERAN_HIGH_0_G("DTM ASSIGNMENT COMMAND cipher mode start with no previous cipher mode set");
            error_cause = RR_PROTOCOL_ERROR_UNSPECIFIED;
          }

          if (rr_cipher_mode_supported(cipher_algorithm, gas_id) == FALSE)
          {
             MSG_GERAN_HIGH_1_G("DTM ASSIGNMENT COMMAND cipher algorithm:%d not supported",cipher_algorithm);
             error_cause = RR_PROTOCOL_ERROR_UNSPECIFIED;
          }
        }
      }
    }

    /* The contents of all IEs have been verified and seem reasonable */
    if ( error_cause == RR_NORMAL_EVENT )
    {
      boolean log_gprs_cell_options = FALSE;

      // If the stored SI is not valid (e.g. following a handover), log the GPRS Cell Options
      if (rr_cell_si_valid(gas_id) == FALSE)
      {
        log_gprs_cell_options = TRUE;
      }
      else
      {
        // Check for any changes in GPRS Cell Options
        if (rr_si_diff_gprs_cell_options(&gprs_broadcast_information.gprs_cell_options, gas_id))
        {
          log_gprs_cell_options = TRUE;
        }
      }

      if (log_gprs_cell_options)
      {
        (void)rr_log_gprs_cell_options(&gprs_broadcast_information.gprs_cell_options, gas_id);
      }

      /* Update the system information database */

      cell_info->bcch_sys_info.gprs_cell_options =
        gprs_broadcast_information.gprs_cell_options;

      MSG_GERAN_HIGH_3_G("access_burst_type=%d",
               gprs_broadcast_information.gprs_cell_options.access_burst_type,
               0,
               0);

      cell_info->bcch_sys_info.power_control_params.alpha =
        gprs_broadcast_information.gprs_power_control_params.alpha;
      cell_info->bcch_sys_info.power_control_params.n_avg_i =
        gprs_broadcast_information.gprs_power_control_params.n_avg_i;
      cell_info->bcch_sys_info.power_control_params.pc_meas_chan =
        gprs_broadcast_information.gprs_power_control_params.pc_meas_chan;
      cell_info->bcch_sys_info.power_control_params.t_avg_t =
        gprs_broadcast_information.gprs_power_control_params.t_avg_t;
      cell_info->bcch_sys_info.power_control_params.t_avg_w =
        gprs_broadcast_information.gprs_power_control_params.t_avg_w;

      /* Update the public store copy of the system information */
      {
        rrps_store_update_data_t *rrps_store_update_ptr = rr_get_rrps_store_update(gas_id);

        if (rrps_store_update_ptr != NULL)
        {
          rrps_store_update_ptr->gprs_cell_options =
            gprs_broadcast_information.gprs_cell_options;

          rrps_store_update_ptr->power_control_params.alpha =
            gprs_broadcast_information.gprs_power_control_params.alpha;
          rrps_store_update_ptr->power_control_params.n_avg_i =
            gprs_broadcast_information.gprs_power_control_params.n_avg_i;
          rrps_store_update_ptr->power_control_params.pc_meas_chan =
            gprs_broadcast_information.gprs_power_control_params.pc_meas_chan;
          rrps_store_update_ptr->power_control_params.t_avg_t =
            gprs_broadcast_information.gprs_power_control_params.t_avg_t;
          rrps_store_update_ptr->power_control_params.t_avg_w =
            gprs_broadcast_information.gprs_power_control_params.t_avg_w;

          rr_write_public_store(
             RRPS_UPD_CELL_OPTIONS | RRPS_UPD_POWER_CTRL,
             rrps_store_update_ptr,
             gas_id
           );

          rr_free_rrps_store_update(
            rrps_store_update_ptr,
            gas_id
          );
        }
        else
        {
          MSG_GERAN_ERROR_0_G("rrps_store_update_ptr=NULL");
        }
      }

      /* Setup new_channel_configuration with the new channel configuration and other */
      /* parameters, which are copied over to RR's internal data when L1 confirms the */
      /* new channels are being used */

      rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.power_level = power_level;
      rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_description = channel_description;

      if ( channel_mode_present )
      {
        rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode_present = TRUE;
        rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.channel_mode = channel_mode;
      }

      if ( cell_allocation_present )
      {
        rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cell_allocation_present = TRUE;
        rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cell_allocation = cell_allocation;
      }

      if ( multirate_config_present )
      {
        /* note that the actual AMR config is stored in the global dedicated data */
        rr_dtm_data_ptr->dtm_assignment_details.cs_assign.channel_assignment.multi_rate_config_provided = TRUE;
      }

      if ( cipher_mode_setting_present )
      {
        if ( start_ciphering )
        {
          rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cipher_flag = TRUE;
          rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cipher_algorithm = cipher_algorithm;
        }
        else
        {
          rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cipher_flag = FALSE;
        }
      }
      else
      {
        /* use existing cipher mode settings if the cipher mode IE is not present */
        rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cipher_flag = dedic_ptr->curr_channel_spec.cipher_flag;
        rr_dtm_data_ptr->dtm_assignment_details.cs_assign.cipher_algorithm = dedic_ptr->curr_channel_spec.cipher_algorithm;
      }

      rr_dtm_data_ptr->dtm_assignment_details.cs_assign_present = TRUE;

      log_dtm_event(RR_DTM_DBG_DTM_ASSIGN_CMD_RCVD, gas_id);
    }
    else
    {
      MSG_GERAN_ERROR_1_G("DTM ASSIGNMENT COMMAND contents not acceptable (%d)",(int)error_cause);
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Decoding of DTM ASSIGNMENT COMMAND failed");

    /* If the error cause hasn't been set to anything specific yet, so set to a */
    /* generic "message constructed incorrectly" error */
    if ( error_cause == RR_NORMAL_EVENT )
    {
      error_cause = RR_SEMANTICALLY_INCORRECT_MESSAGE;
    }

    log_dtm_event(RR_DTM_DBG_DTM_ASSIGN_CMD_RCVD_ERR, gas_id);
  }

  if (band_ptr != NULL)
  {
    *band_ptr = channel_description.frequency_list.channel[0].band;
  }

  return error_cause;
} /* end rr_dtm_process_dtm_assignment_command() */


/*============================================================================

FUNCTION       rr_dtm_control

DESCRIPTION    Main DTM module state machine

DEPENDENCIES   None

PARAMETERS     event_in
               message_ptr

RETURN VALUE   EV_RR_EST_REQ
               EV_DTM_PERFORM_GSM_RELEASE
               EV_DTM_PERFORM_CSPS_RELEASE
               EV_DTM_ASSIGNMENT_RECEIVED
               EV_DTM_ASSIGNMENT_COMPLETE
               EV_DTM_ASSIGNMENT_ERROR
               EV_RADIO_LINK_FAILURE

SIDE EFFECTS   None

=============================================================================*/
rr_event_T rr_dtm_control(rr_event_T event_in, void *message_ptr, const gas_id_t gas_id)
{
  byte message_id;
  byte message_set;

  rr_event_T  event_out = EV_NO_EVENT;
  rr_event_T  new_event = EV_NO_EVENT;

  // Obtain a pointer to the module data
  rr_dtm_static_data_t *rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  /* If EV_INPUT_MESSAGE is passed in, message_set & message_id will be setup from */
  /* message_ptr and EV_INPUT_MESSAGE returned. If message_ptr, message_set or */
  /* message_id are NULL, then EV_NO_EVENT will be returned */
  /* If any other event is passed in, this is simply returned */
  event_in = rr_extract_message_header_details(
    event_in,
    message_ptr,
    &message_set,
    &message_id
  );

  if ((event_in == EV_INPUT_MESSAGE) && (message_ptr != NULL))
  {
    if ((message_set == MS_RR_L2) &&
        ((message_id == (byte) DL_DATA_IND) || (message_id == (byte) DL_UNIT_DATA_IND)))
    {
      uint8 message_type;
      l2_channel_type_T l2_channel_type;

      if (message_id == (byte) DL_UNIT_DATA_IND)
      {
        dl_unit_data_ind_T * dl_unit_data_ind = (dl_unit_data_ind_T *)message_ptr;
        message_type = get_layer3_message_type(dl_unit_data_ind->layer3_message);
        l2_channel_type = dl_unit_data_ind->l2_channel_type;
      }
      else
      {
        dl_data_ind_T * dl_data_ind = (dl_data_ind_T *)message_ptr;
        message_type = get_layer3_message_type(dl_data_ind->layer3_message);
        l2_channel_type = dl_data_ind->l2_channel_type;
      }

      if (l2_channel_type == DCCH)
      {
        switch ( message_type )
        {
          case DTM_ASSIGNMENT_COMMAND:
          case PACKET_ASSIGNMENT:
          {
            // Default value assume the message is ok to process
            uint8 rr_cause = RR_NORMAL_EVENT;

            if (rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID))
            {
              /**
               * Only process DTM ASSIGNMENT COMMAND if
               * - TLLI is assigned
               * - service domain allows PS 
               * - PS is not blocked due to Thermal Mitigation 
               * otherwise send RRStatus(MessageTypeNotCompatibleWithProtocolState)
               */
              if (rr_ps_domain_active(gas_id) == FALSE)
              {
                MSG_GERAN_HIGH_0_G("Reject assignment (PS domain not active)");
                rr_cause = RR_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE;
              }
              else
              if (rr_thermal_ps_allowed(gas_id) == FALSE)
              {
                MSG_GERAN_HIGH_0_G("Reject assignment (Thermal Mitigation)");
                rr_cause = RR_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE;
              }
            }
            else
            {
              /**
               * If the MS does not support DTM and a DTM ASSIGNMENT COMMAND is received, then send RRStatus 
               * indicating MessageTypeNotExistent (3GPP CR 95017) 
               */
              MSG_GERAN_HIGH_0_G("Reject assignment (DTM not enabled)");
              rr_cause = RR_MESSAGE_TYPE_NON_EXISTENT;
            }

            /**
             * If it is ok to proceed, then continue and process the message according to state. Otherwise, send
             * RRStatus message to the network with cause 
             */
            if (rr_cause == RR_NORMAL_EVENT)
            {
              if (message_type == PACKET_ASSIGNMENT)
              {
                event_in = EV_DTM_PACKET_ASSIGNMENT_RECEIVED;
              }
              else
              {
                event_in = EV_DTM_ASSIGNMENT_COMMAND_RECEIVED;
              }
            }
            else
            {
              // Send RRStatus to the network with the appropriate cause
              rr_send_status(rr_cause, gas_id);

              // Don't process the message any further
              event_in = EV_NO_EVENT;
            }

            break;
          }

          case DTM_REJECT:
            event_in = EV_DTM_REJECT_RECEIVED;
            break;

          case DTM_INFORMATION:
            event_in = EV_DTM_INFORMATION_RECEIVED;
            break;

          case PACKET_NOTIFICATION:
          {
            event_in = EV_NO_EVENT;

            if (rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID))
            {
              if (rr_thermal_ps_allowed(gas_id))
              {
                process_packet_notification(gas_id);
              }
              else
              {
                // Send RRStatus{MessageTypeNotCompatibleWithProtocolState} to the network
                rr_send_status(RR_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE, gas_id);
              }
            }
            else
            {
              /**
               * If the MS does not support DTM and a DTM ASSIGNMENT COMMAND is received, then send RRStatus 
               * indicating MessageTypeNotExistent (3GPP CR 95017) 
               */
              rr_send_status(RR_MESSAGE_TYPE_NON_EXISTENT, gas_id);
            }
            break;
          }

          default: ;
        }
      }
    }
    else
    if (message_set == MS_RR_RR)
    {
      rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

      if ((RR_DTM_IMSG == rr_rr_msg_ptr->header.rr_message_set) &&
          (RR_DTM_IMSG_CS_RELEASE_REQ == rr_rr_msg_ptr->header.imh.message_id))
      {
        log_dtm_event(RR_DTM_DBG_CS_REL_REQ_RCVD, gas_id);
      }
    }
    else
    if ((message_set == MS_MAC_RR) &&
        (message_id == (byte) MAC_GRR_UL_TBF_REQ))
    {
      save_mac_grr_ul_tbf_req((mac_grr_ul_tbf_req_t *)message_ptr, gas_id);
      event_in = EV_DTM_UL_TBF_REQ;

      log_dtm_event(RR_DTM_DBG_UL_TBF_REQ, gas_id);
    }
    else
    if (message_set == MS_RR_L1)
    {
      if (message_id == (byte) MPH_DTM_CHANNEL_ASSIGN_CNF)
      {
        log_dtm_event(RR_DTM_DBG_DTM_CHAN_ASSIGN_CNF_RCVD, gas_id);
      }
      else
      if (message_id == (byte) MPH_DTM_CHANNEL_RELEASE_IND)
      {
        log_dtm_event(RR_DTM_DBG_DTM_CHAN_REL_IND_RCVD, gas_id);
      }
    }
  }
  else
  if ((EV_DTM_NO_PS_ACCESS_REQ == event_in) ||
      (EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL == event_in))
  {
    log_dtm_event(RR_DTM_DBG_NO_PS_ACCESS_REQ_RCVD, gas_id);
  }
  else
  if ((EV_DTM_PS_ACCESS_REQ == event_in) ||
      (EV_DTM_PS_ACCESS_REQ_PURGE == event_in))
  {
    log_dtm_event(RR_DTM_DBG_PS_ACCESS_REQ_RCVD, gas_id);
  }
  else
  if (EV_INTRA_CELL_HANDOVER == event_in)
  {
    log_dtm_event(RR_DTM_DBG_HANDOVER, gas_id);
  }

  while ( event_in != EV_NO_EVENT )
  {
    if ( event_in == EV_RESET_SOFTWARE )
    {
      stop_t3148(gas_id);

      if (rr_timer_cancel(RR_GRR_CHN_ASSIGN_REQ_GUARD_TIMER, gas_id))
      {
        MSG_GERAN_MED_0_G("Stopped RR_GRR_CHN_ASSIGN_REQ_GUARD_TIMER");
      }

#ifdef FEATURE_GSM_EDTM
      (void) rr_gprs_edtm_control(EV_RESET_SOFTWARE, NULL, gas_id);
#endif /* FEATURE_GSM_EDTM */

      // Reset local data and set the state to NULL
      rr_dtm_transition_to_null(gas_id);

      event_in = EV_NO_EVENT;
      continue;
    }

    if ((event_in == EV_INPUT_MESSAGE) && (message_ptr == NULL))
    {
      event_in = EV_NO_EVENT;
      continue;
    }

    switch ( rr_dtm_data_ptr->state )
    {
      case DTM_NULL:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        send_omsg_cs_release_cnf(
                          RR_DTM_CE_REL_MODE_GSM,   // ce_rel_mode
                          gas_id                    // gas_id
                        );
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }
            break;
          }

          /* RR_EST_REQ received into GRR */
          case EV_RR_EST_REQ:
          {
#ifdef FEATURE_GSM_EDTM
            if (rr_gprs_edtm_control(event_in, NULL, gas_id) == EV_EDTM_ESTABLISHMENT_STARTED)
            {
              /* EDTM module is handling this from here */
              MSG_GERAN_MED_0_G("EDTM establishment...");
            }
            else
#endif /* FEATURE_GSM_EDTM */
            {
              event_out = EV_RR_EST_REQ;
            }
            break;
          }

          /* RR has starting RACHing for CS conn est */
          case EV_RR_EST_IND:
          {
            /* Setup control/data structures for DTM operation */
            rr_dtm_activate_init(gas_id);

            rr_dtm_data_ptr->state = DTM_RR_CS_EST;
            break;
          }

#ifdef FEATURE_WCDMA
          case EV_DTM_W2G_HO:
          case EV_DTM_W2G_HO_PS_ATTACHED:
          {
            /* Setup control/data structures for DTM operation */
            rr_dtm_activate_init(gas_id);

            /*
            A W->G handover is treated like a G->G handover as far as dtm_cell_capability_monitor
            is concerned, either coming from GSM (non-DTM) cell if not PS-attached on W, or from a
            DTM cell if PS-attached from W.
            */
            (void) dtm_cell_capability_monitor(event_in, gas_id);

            // Assume no DTM support. The next SI6 received will indicate if the cell supports DTM
            rr_dtm_data_ptr->state = DTM_CELL_DOES_NOT_SUPPORT_DTM;
            break;
          }
#endif // FEATURE_WCDMA

#ifdef FEATURE_GSM_EDTM
          case EV_EDTM_IMM_ASSIGNMENT_RECEIVED:
          {
            /* Setup control/data structures for DTM operation */
            rr_dtm_activate_init(gas_id);

            /* A CS-only assignment or GMAC reject was processed by the EDTM module */
            /* so an RR_EST_REQ is about to be processed again in the normal way    */
            rr_dtm_data_ptr->state = DTM_RR_CS_EST;
            break;
          }

          case EV_EDTM_ESTABLISHMENT_COMPLETED:
          {
            /* Setup control/data structures for DTM operation */
            rr_dtm_activate_init(gas_id);

            (void) dtm_cell_capability_monitor(EV_EDTM_ESTABLISHMENT_COMPLETED, gas_id);
            rr_dtm_data_ptr->state = DTM_DTM_TRANSFER;
            break;
          }
#endif /* FEATURE_GSM_EDTM */

          case EV_DTM_NO_PS_ACCESS_REQ:
          case EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL:
          {
            // This could be due to GPRS Detach or SGLTE split when DTM disabled in NV

            // Send any confirmation required
            CALL_RR_CB_FN((rr_cb_fn_t)message_ptr, gas_id);
            break;
          }

          default: ;
        }

        break;
      } // DTM_NULL

      case DTM_RR_CS_EST:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        send_omsg_cs_release_cnf(
                          RR_DTM_CE_REL_MODE_GSM,   // ce_rel_mode
                          gas_id                    // gas_id
                        );
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }
            break;
          }

          case EV_DTM_RR_EST_CNF:
          {
            // Check if the cell supports DTM
            if (dtm_cell_capability_monitor(EV_DTM_RR_EST_CNF, gas_id) == EV_DTM_CELL_SUPPORTS_DTM)
            {
              // Resume GMM (if required)
              (void) rr_gmm_resume_gprs(gas_id);

              // Wait for GPRS service request
              rr_dtm_data_ptr->state = DTM_NO_PS_DOMAIN;
            }
            else
            {
              // The cell does not support DTM. (GPRS service may or may not have been requested)
              rr_dtm_data_ptr->state = DTM_CELL_DOES_NOT_SUPPORT_DTM;
            }
            break;
          }

          case EV_DTM_RR_EST_FAIL:
          {
            // Reset local data and transition to NULL state
            rr_dtm_transition_to_null(gas_id);
            break;
          }

#ifdef FEATURE_GSM_EDTM
          case EV_RR_EST_IND:
          {
            // Corner case where MT IA was received via EDTM, but RR_EST_REQ then received from NAS
            // RR-DTM already activated due to the MT IA processing, but now RACHing for new IA
            break;
          }
#endif // FEATURE_GSM_EDTM

          case EV_DTM_PS_ACCESS_REQ:
          case EV_DTM_PS_ACCESS_REQ_PURGE:
          {
            // An assessment of PS requirements once CS establishment is successful
            // Note: GMM is not suspended until CS establishment is successful

            // Send any confirmation required
            CALL_RR_CB_FN((rr_cb_fn_t)message_ptr, gas_id);

            if (event_in == EV_DTM_PS_ACCESS_REQ_PURGE)
            {
              // Purge GMM signalling when PS access is enabled
              rr_dtm_data_ptr->data_ptr->purge_gmm_signalling = TRUE;
            }
            break;
          }

          case EV_DTM_NO_PS_ACCESS_REQ:
          case EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL:
          {
            // An assessment of PS requirements once CS establishment is successful

            // Send any confirmation required
            CALL_RR_CB_FN((rr_cb_fn_t)message_ptr, gas_id);
            break;
          }

          default: ;
        }

        break;
      } // DTM_RR_CS_EST

      case DTM_DTM_POSSIBLE:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        (void) dtm_cell_capability_monitor(EV_RESET_SOFTWARE, gas_id);

                        // Stop T3148 if running
                        stop_t3148(gas_id);

                        // Suspend GMM
                        rr_gmm_suspend_gprs(gas_id);

                        // Block PS access (no delete)
                        rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

                        send_omsg_cs_release_cnf(
                          RR_DTM_CE_REL_MODE_GSM,   // ce_rel_mode
                          gas_id                    // gas_id
                        );

                        // Reset local data and transition to NULL state
                        rr_dtm_transition_to_null(gas_id);
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }
            break;
          }

          case EV_DTM_UL_TBF_REQ:
          {
            mac_grr_ul_tbf_req_t *ul_tbf_req = get_mac_grr_ul_tbf_req(gas_id);

            if (ul_tbf_req != NULL)
            {
              /*
              UL request received from MAC. Note that T3142 may be running, as PS is left enabled.
              This is because access via GTTP is still allowed, so the UL request must be checked
              first before deciding whether to allow/disallow it.
              */

              if (gttp_criteria_met(ul_tbf_req->gttp_gmm_sig_octets, gas_id))
              {
                rr_send_grr_mac_dtm_ul_dcch_ind(gas_id);
                rr_dtm_data_ptr->state = DTM_GTTP_TRANSFER;
              }
              else
              {
                // Perform a check if UL access is allowed (i.e. is T3142 running)
                if (rr_mac_ul_tbf_est_is_allowed(gas_id))
                {
                  mac_grr_ul_tbf_req_t *mac_grr_ul_tbf_req = get_mac_grr_ul_tbf_req(gas_id);

                  if (mac_grr_ul_tbf_req != NULL)
                  {
                    /* Send "DTM REQUEST" message OTA to the network */
                    send_dtm_request_ota(
                      &mac_grr_ul_tbf_req->channel_req_info,    // channel_req_info
                      gas_id                                    // gas_id
                    );

                    /* T3148 guards against receiving no reply */
                    start_t3148(gas_id);
                  }
                }
                else
                {
                  MSG_GERAN_HIGH_0_G("UL request blocked because access not allowed");

                  // Block PS access (no delete)
                  rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

                  rr_dtm_data_ptr->state = DTM_NO_PS_ACCESS_UL_BLOCKED;
                }
              }

              // Processing of the MAC_GRR_UL_TBF_REQ message is now complete - clear the saved message
              save_mac_grr_ul_tbf_req(NULL, gas_id);
            }
            break;
          }

          case EV_DTM_ASSIGNMENT_COMMAND_RECEIVED:
          {
            boolean ul_requested = is_t3148_running(gas_id);

            // respond_to_dtm_assignment_command() returns
            //  - EV_DTM_ASSIGNMENT_COMMAND_CHECKED if assignment ok
            //  - EV_DTM_ASSIGNMENT_ERROR if there are errors
            if (EV_DTM_ASSIGNMENT_COMMAND_CHECKED == respond_to_dtm_assignment_command(gas_id))
            {
              new_event = EV_DTM_ASSIGNMENT_COMMAND_CHECKED;
            }
            else
            {
              MSG_GERAN_HIGH_0_G("Error in DTM ASSIGNMENT COMMAND");

              // If an UL was requested, then delete the PDU and re-enable PS
              if (ul_requested)
              {
                // Reject back to MAC and delete the PDU
                rr_send_grr_mac_no_ps_access_ind(DEL_ONE, gas_id);

                // Re-enable PS access
                rr_send_grr_mac_ps_access_req(
                  FALSE,   // purge_gmm_signalling
                  gas_id   // gas_id
                );

                rr_dtm_data_ptr->state = DTM_ENABLING_PS_ACCESS;
              }
            }

            // DTM assignment received so stop T3148 (if it's running)
            stop_t3148(gas_id);
            break;
          }

          case EV_DTM_PACKET_ASSIGNMENT_RECEIVED:
          {
            boolean ul_requested = is_t3148_running(gas_id);

            // process_packet_assignment() returns
            //  - EV_DTM_PACKET_ASSIGNMENT_CHECKED if assignment ok
            //  - EV_DTM_ASSIGNMENT_ERROR if there are errors
            if (EV_DTM_PACKET_ASSIGNMENT_CHECKED == process_packet_assignment(gas_id))
            {
              new_event = EV_DTM_PACKET_ASSIGNMENT_CHECKED;
            }
            else
            {
              MSG_GERAN_HIGH_0_G("Error in PACKET ASSIGNMENT");

              // If an UL was requested, then delete the PDU and re-enable PS
              if (ul_requested)
              {
                // Reject back to MAC and delete the PDU
                rr_send_grr_mac_no_ps_access_ind(DEL_ONE, gas_id);
              
                // Re-enable PS access
                rr_send_grr_mac_ps_access_req(
                  FALSE,   // purge_gmm_signalling
                  gas_id   // gas_id
                );
              
                rr_dtm_data_ptr->state = DTM_ENABLING_PS_ACCESS;
              }
            }

            // DTM assignment received so stop T3148 (if it's running)
            stop_t3148(gas_id);
            break;
          }

          case EV_DTM_ASSIGNMENT_COMMAND_CHECKED:
          {
            /**
             * Note: EV_DTM_ASSIGNMENT_COMMAND_CHECKED is not checked-in 'in-line' with 
             * EV_DTM_ASSIGNMENT_COMMAND_RECEIVED, because EV_DTM_ASSIGNMENT_COMMAND_CHECKED may be passed-in 
             * from DTM_ENABLING_PS_ACCESS state, where it can be held pending 
             */

            // Check if the GPRS Cell Options in the DTM ASSIGNMENT COMMAND has updated any MM GPRS sys info
            check_if_mm_gprs_sys_info_changed(gas_id);

            // Start the procedure to configure MAC/L1 with the new DTM Assignment
            (void) dtm_assign_command_control(EV_DTM_ASSIGNMENT_READY, NULL, gas_id);

            // Move to DTM_PROCESS_ASSIGNMENT_COMMAND state to route the incoming messages appropriately
            rr_dtm_data_ptr->state = DTM_PROCESS_ASSIGNMENT_COMMAND;

            // Return EV_DTM_ASSIGNMENT_RECEIVED to the parent caller, which is rr_handle_procedures()
            event_out = EV_DTM_ASSIGNMENT_RECEIVED;
            break;
          }

          case EV_DTM_PACKET_ASSIGNMENT_CHECKED:
          {
            /**
             * Note: EV_DTM_PACKET_ASSIGNMENT_CHECKED is not checked-in 'in-line' with 
             * EV_DTM_PACKET_ASSIGNMENT_RECEIVED, because EV_DTM_PACKET_ASSIGNMENT_CHECKED may be passed-in 
             * from DTM_ENABLING_PS_ACCESS state, where it can be held pending 
             */

            // Check if the GPRS Cell Options in the PACKET ASSIGNMENT has updated any MM GPRS sys info
            check_if_mm_gprs_sys_info_changed(gas_id);

            // Start the procedure to configure MAC/L1 with the new DTM Assignment
            (void) dtm_packet_assign_control(EV_DTM_ASSIGNMENT_READY, NULL, gas_id);

            // Move to DTM_PROCESS_PACKET_ASSIGNMENT state to route the incoming messages appropriately
            rr_dtm_data_ptr->state = DTM_PROCESS_PACKET_ASSIGNMENT;

            // Return EV_DTM_ASSIGNMENT_RECEIVED to the parent caller, which is rr_handle_procedures()
            event_out = EV_DTM_ASSIGNMENT_RECEIVED;
            break;
          }

          case EV_DTM_REJECT_RECEIVED:
          {
            if (is_t3148_running(gas_id))
            {
              /**
               * A "DTM REJECT" message has been received OTA from the network 
               *  
               * process_dtm_reject() returns 
               *  EV_DTM_ACCESS_REJECT - DTM Reject, MAC has been informed; PS access still allowed
               *  EV_DTM_ACCESS_REJECT_T3142_RUNNING - DTM Reject and T3142 started. PS access should now be
               *  blocked
               */
              rr_event_T  process_result = process_dtm_reject(gas_id);

              if ( (process_result == EV_DTM_ACCESS_REJECT) ||
                   (process_result == EV_DTM_ACCESS_REJECT_T3142_RUNNING) )
              {
                stop_t3148(gas_id);

                if (process_result == EV_DTM_ACCESS_REJECT_T3142_RUNNING)
                {
                  // Block PS access (no delete)
                  rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

                  rr_dtm_data_ptr->state = DTM_NO_PS_ACCESS_UL_BLOCKED;
                }
              }
            }
            break;
          }

          case EV_DTM_T3148_EXPIRY:
          {
            log_dtm_event(RR_DTM_DBG_T3148_EXPIRY, gas_id);

            /**
             * T3148 has expired with no response from the network. PS is blocked & unblocked in GMAC, 
             * without deleting the PDU - this will cause GMAC to re-request an UL and another DTM REQUEST 
             * will be sent.
             */
            rr_send_grr_mac_no_ps_access_ind(
              NO_DEL,   // delete_pdu
              gas_id    // gas_id
            );

            rr_send_grr_mac_ps_access_req(
              FALSE,    // purge_gmm_signalling
              gas_id    // gas_id
            );
            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            // Stop T3148 if running
            stop_t3148(gas_id);

            /* Inform cell capability monitor that a handover is in progress */
            /* This monitors for the reception of DTM INFORMATION / SI6 on the */
            /* new cell */
            (void) dtm_cell_capability_monitor(event_in, gas_id);

            // Suspend GMM
            rr_gmm_suspend_gprs(gas_id);

            // Block PS access (no delete)
            rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

            rr_dtm_data_ptr->state = DTM_DED_PROC_IN_PROGRESS;
            break;
          }

          case EV_DTM_SI6_RECEIVED:
          {
            /* Inform cell capability monitor that SI6 has been received - this will */
            /* check if the DTM capability of the cell has changed, and return */
            /* EV_DTM_CELL_SUPPORTS_DTM or EV_DTM_CELL_DOES_NOT_SUPPORT_DTM */
            if (dtm_cell_capability_monitor(event_in, gas_id) ==
                EV_DTM_CELL_DOES_NOT_SUPPORT_DTM)
            {
              // Stop T3148 if running
              stop_t3148(gas_id);

              // Suspend GMM
              rr_gmm_suspend_gprs(gas_id);

              // Block PS access (no delete)
              rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

              // Ensure GMM signalling is purged is PS access is re-enabled on this cell
              rr_dtm_data_ptr->data_ptr->purge_gmm_signalling = TRUE;

              rr_dtm_data_ptr->state = DTM_CELL_DOES_NOT_SUPPORT_DTM;
            }
            break;
          }

          case EV_DTM_INFORMATION_RECEIVED:
          {
            /* No action necessary. */
            break;
          }

          case EV_DTM_NO_PS_ACCESS_REQ:
          case EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL:
          {
            // PS domain has been deactivated (i.e. GPRS Detach, TLLI unassignment)

            // Stop T3148 if running
            stop_t3148(gas_id);

            /**
             * The objective here is to remove the PS domain, but leave the ongoing dedicated connection 
             * intact. To this this, the TBF is released by blocking PS access in MAC/RLC via 
             * GRR_MAC_NO_PS_ACCESS_IND. 
             * Note: GRR_MAC_DTM_CHANNEL_RELEASE_IND is NOT sent here - this message indicates to MAC not to 
             * release L1, but we need the TBF released using normal Packet Transfer procedures. 
             */
            if (event_in == EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL)
            {
              rr_ps_send_imsg_no_ps_access_req(
                FALSE,     // suspend_gmm
                DEL_ALL,   // delete_pdu
                gas_id     // gas_id
              );
            }
            else
            {
              rr_ps_send_imsg_no_ps_access_req(
                FALSE,     // suspend_gmm
                NO_DEL,    // delete_pdu
                gas_id     // gas_id
              );
            }

            rr_dtm_data_ptr->data_ptr->no_ps_access_confirm_cb_fn = (rr_cb_fn_t)message_ptr;
            rr_dtm_data_ptr->state = DTM_DISABLING_PS_ACCESS;
            break;
          }

          case EV_DTM_MSC_CHANGE_IND:
          {
            // Stop T3148 if running
            stop_t3148(gas_id);

            // Request block of PS access (no purge)
            rr_ps_send_imsg_no_ps_access_req(
              FALSE,     // suspend_gmm
              NO_DEL,    // delete_pdu
              gas_id     // gas_id
            );

            // and wait for confirmation
            rr_dtm_data_ptr->state = DTM_MSC_CHANGE;
            break;
          }

          default: ;
        }

        break;
      } // DTM_DTM_POSSIBLE

      case DTM_PROCESS_PACKET_ASSIGNMENT:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        new_event = dtm_packet_assign_control(EV_DTM_CS_RELEASE_REQUIRED, NULL, gas_id);
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              case MS_MAC_RR:
              {
                switch (message_id)
                {
                  case MAC_GRR_DTM_CHANNEL_ASSIGN_REJ:
                  {
                    new_event = dtm_packet_assign_control(EV_INPUT_MESSAGE, message_ptr, gas_id);
                    break;
                  }

                  default: ;
                }
                break;
              }

              case MS_RR_L1:
              case MS_RR_L2:
              {
                /* Pass all L1/L2 messaging thru to dtm_packet_assign_control() */
                new_event = dtm_packet_assign_control(EV_INPUT_MESSAGE, message_ptr, gas_id);
                break;
              }

              default: ;
            }
            break;
          }

          case EV_DTM_UL_TBF_REQ:
          {
            // Process the UL request appropriate to the state of Packet Assignment processing
            (void) dtm_packet_assign_control(EV_DTM_UL_TBF_REQ, NULL, gas_id);
            break;
          }

          case EV_DTM_CSPS_EST_CNF:
          {
            MSG_GERAN_HIGH_0_G("Entered DTM");
            rr_dtm_data_ptr->state = DTM_DTM_TRANSFER;
            event_out = EV_DTM_ASSIGNMENT_COMPLETE;
            break;
          }

          case EV_DTM_ASSIGNMENT_ERROR_PS_BLOCKED:
          {
            /**
             * The PACKET ASSIGNMENT will not be received while in DTM Transfer (unlike DTM ASSIGNMENT COMMAND
             * which can be). 
             * MAC blocks RLC in this case when it sends MAC_GRR_DTM_CHANNEL_ASSIGN_REJ, so PS access will 
             * need to be re-enabled 
             */

            // Re-enable PS access
            rr_send_grr_mac_ps_access_req(
              FALSE,   // purge_gmm_signalling
              gas_id   // gas_id
            );

            rr_dtm_data_ptr->state = DTM_ENABLING_PS_ACCESS;
            event_out = EV_DTM_ASSIGNMENT_COMPLETE;
            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            MSG_GERAN_ERROR_0_G("Hand-over request whilst processing Packet Assign!");
            break;
          }

          case EV_DTM_PS_ACCESS_REQ:
          case EV_DTM_PS_ACCESS_REQ_PURGE:
          {
            MSG_GERAN_ERROR_0_G("Unexpected EV_DTM_PS_ACCESS in PROCESS_PACKET_ASSIGNMENT");
            break;
          }

          case EV_DTM_PERFORM_GSM_RELEASE:
          case EV_DTM_PERFORM_CSPS_RELEASE:
          {
            rr_dtm_ce_rel_mode_e ce_rel_mode = RR_DTM_CE_REL_MODE_GSM;

            if (event_in == EV_DTM_PERFORM_CSPS_RELEASE)
            {
              ce_rel_mode = RR_DTM_CE_REL_MODE_DTM;
            }

            send_omsg_cs_release_cnf(
              ce_rel_mode,   // ce_rel_mode
              gas_id         // gas_id
            );

            // Clear any saved UL request
            save_mac_grr_ul_tbf_req(NULL, gas_id);

            (void) dtm_packet_assign_control(EV_RESET_SOFTWARE, NULL, gas_id);
            (void) dtm_cell_capability_monitor(EV_RESET_SOFTWARE, gas_id);

            // Stop T3148 if running
            stop_t3148(gas_id);

            rr_gmm_suspend_gprs(gas_id);

            // Block PS access (no delete)
            rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

            // Reset local data and transition to NULL state
            rr_dtm_transition_to_null(gas_id);
            break;
          }

          /**
           * Note: Following events are blocked in this state: 
           *  
           *  EV_DTM_NO_PS_ACCESS_REQ
           *  EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL  (see rr_dtm_can_process_no_ps_access_req())
           */

          default: ;
        }

        break;
      } // DTM_PROCESS_PACKET_ASSIGNMENT

      case DTM_PROCESS_ASSIGNMENT_COMMAND:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        new_event = dtm_assign_command_control(EV_DTM_CS_RELEASE_REQUIRED, NULL, gas_id);
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              case MS_MAC_RR:
              case MS_RR_L1:
              case MS_RR_L2:
              {
                /* Pass all MAC/L1/L2 messaging thru to dtm_assign_command_control() */
                new_event = dtm_assign_command_control(EV_INPUT_MESSAGE, message_ptr, gas_id);
                break;
              }

              default: ;
            }
            break;
          }

          case EV_DTM_UL_TBF_REQ:
          {
            // Process the UL request appropriate to the state of Assignment Command processing
            (void) dtm_assign_command_control(EV_DTM_UL_TBF_REQ, NULL, gas_id);
            break;
          }

          case EV_DTM_CS_EST_CNF:
          case EV_DTM_CS_EST_CNF_UL_TBF_REQ_PENDING:
          {
            MSG_GERAN_HIGH_0_G("Assignment complete; PS released");
            rr_dtm_data_ptr->state = DTM_DTM_POSSIBLE;
            event_out = EV_DTM_ASSIGNMENT_COMPLETE;

            if (event_in == EV_DTM_CS_EST_CNF_UL_TBF_REQ_PENDING)
            {
              new_event = EV_DTM_UL_TBF_REQ;
            }
            break;
          }

          case EV_DTM_CSPS_EST_CNF:
          {
            MSG_GERAN_HIGH_0_G("Entered DTM");
            rr_dtm_data_ptr->state = DTM_DTM_TRANSFER;
            event_out = EV_DTM_ASSIGNMENT_COMPLETE;
            break;
          }

          case EV_DTM_ASSIGNMENT_ERROR_PS_ENABLED:
          {
            /**
             * The DTM ASSIGNMENT COMMAND can be received when in Dedicated or DTM Transfer. 
             * If MAC rejects the DTM Assignment content 
             * - if in Packet Transfer at the time, it will continue with Packet Transfer 
             * - if idle, it will block RLC and therefore required PS access to be re-enabled 
             *  
             * The case here is that MAC was in Packet Transfer when the DTM Assignment was received & 
             * rejected, but during the subsequent resuming of L2, Packet Transfer ended normally 
             */
            rr_dtm_data_ptr->state = DTM_DTM_POSSIBLE;
            event_out = EV_DTM_ASSIGNMENT_COMPLETE;
            break;
          }

          case EV_DTM_ASSIGNMENT_ERROR_PS_BLOCKED:
          {
            /**
             * The DTM ASSIGNMENT COMMAND can be received when in Dedicated or DTM Transfer. 
             * If MAC rejects the DTM Assignment content 
             * - if in Packet Transfer at the time, it will continue with Packet Transfer 
             * - if idle, it will block RLC and therefore required PS access to be re-enabled 
             *  
             * The case here is that MAC was idle when the DTM Assignment was received & rejected
             */
            rr_send_grr_mac_ps_access_req(
              FALSE,   // purge_gmm_signalling
              gas_id   // gas_id
            );

            rr_dtm_data_ptr->state = DTM_ENABLING_PS_ACCESS;
            event_out = EV_DTM_ASSIGNMENT_COMPLETE;
            break;
          }

          case EV_DTM_ASSIGNMENT_ERROR_IN_DTM:
          {
            /**
             * The DTM ASSIGNMENT COMMAND can be received when in Dedicated or DTM Transfer. 
             * If MAC rejects the DTM Assignment content 
             * - if in Packet Transfer at the time, it will continue with Packet Transfer 
             * - if idle, it will block RLC and therefore required PS access to be re-enabled 
             *  
             * The case here is that MAC was in Packet Transfer when the DTM Assignment was received & 
             * rejected, and that Packet Transfer is still ongoing
             */
            rr_dtm_data_ptr->state = DTM_DTM_TRANSFER;
            event_out = EV_DTM_ASSIGNMENT_COMPLETE;
            break;
          }

          case EV_DTM_ASSIGNMENT_FAILURE:
          {
            MSG_GERAN_HIGH_0_G("DTM Assignment failed");

            /*
            PS access will have been blocked during the reconnection attempt following DTM Assignment failure.
            GMM is not suspended during DTM Assignment processing, so PS access can be unblocked immediately
            */
            rr_send_grr_mac_ps_access_req(
              FALSE,   // purge_gmm_signalling
              gas_id   // gas_id
            );

            rr_dtm_data_ptr->state = DTM_DTM_POSSIBLE;
            event_out = EV_DTM_ASSIGNMENT_COMPLETE;
            break;
          }

          case EV_DTM_PERFORM_GSM_RELEASE:
          case EV_DTM_PERFORM_CSPS_RELEASE:
          {
            rr_dtm_ce_rel_mode_e ce_rel_mode = RR_DTM_CE_REL_MODE_GSM;

            if (event_in == EV_DTM_PERFORM_CSPS_RELEASE)
            {
              ce_rel_mode = RR_DTM_CE_REL_MODE_DTM;
            }

            send_omsg_cs_release_cnf(
              ce_rel_mode,   // ce_rel_mode
              gas_id         // gas_id
            );

            // Clear any saved UL request
            save_mac_grr_ul_tbf_req(NULL, gas_id);

            (void) dtm_assign_command_control(EV_RESET_SOFTWARE, NULL, gas_id);
            (void) dtm_cell_capability_monitor(EV_RESET_SOFTWARE, gas_id);

            // Stop T3148 if running
            stop_t3148(gas_id);

            // Suspend GMM
            rr_gmm_suspend_gprs(gas_id);

            // and disable PS access (no delete)
            // Note: If the DTM assignment procedure was interrupted (i.e. reconnecting after L2 failure),
            // PS access may already have been blocked
            if (rr_mac_ps_access_granted(gas_id))
            {
              rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);
            }

            // Reset local data and transition to NULL state
            rr_dtm_transition_to_null(gas_id);
            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            MSG_GERAN_ERROR_0_G("Hand-over request whilst processing DTM Assign Command!");
            break;
          }

          case EV_RADIO_LINK_FAILURE:
          {
            event_out = EV_RADIO_LINK_FAILURE;
            break;
          }

          case EV_DTM_PS_ACCESS_REQ:
          case EV_DTM_PS_ACCESS_REQ_PURGE:
          {
            MSG_GERAN_ERROR_0_G("Unexpected EV_DTM_PS_ACCESS in PROCESS_ASSIGNMENT_COMMAND");
            break;
          }

          /**
           * Note: Following events are blocked in this state: 
           *  
           *  EV_DTM_NO_PS_ACCESS_REQ
           *  EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL  (see rr_dtm_can_process_no_ps_access_req())
           */

          default: ;
        }

        break;
      } // DTM_PROCESS_ASSIGNMENT_COMMAND

      case DTM_DTM_TRANSFER:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        rr_dtm_imsg_cs_release_req_t *imsg_ptr = &rr_rr_msg_ptr->dtm.imsg.cs_release_req;

                        // In DTM transfer, so release mode will be DTM
                        rr_dtm_ce_rel_mode_e ce_rel_mode = RR_DTM_CE_REL_MODE_DTM;

#ifdef FEATURE_GSM_EDTM
                        if (imsg_ptr->edtm_cs_release_allowed &&
                            (rr_gprs_edtm_ps_release_required(gas_id) == FALSE))
                        {
                          /* PS is not being released, use the hybrid EDTM mode */
                          rr_gprs_edtm_signal_cs_release(gas_id);

                          // Change release mode to EDTM
                          ce_rel_mode = RR_DTM_CE_REL_MODE_EDTM;
                        }
                        else
#endif // FEATURE_GSM_EDTM
                        {
                          (void) dtm_cell_capability_monitor(EV_RESET_SOFTWARE, gas_id);

                          rr_send_grr_mac_dtm_channel_release_ind(NORMAL_REL, gas_id);

                          rr_gmm_suspend_gprs(gas_id);
                          rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

                          // Reset local data and transition to NULL state
                          rr_dtm_transition_to_null(gas_id);
                        }

                        send_omsg_cs_release_cnf(
                          ce_rel_mode,   // ce_rel_mode
                          gas_id         // gas_id
                        );
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_DTM_CHANNEL_RELEASE_IND:
                  {
                    mph_dtm_channel_release_ind_t * mph_dtm_channel_release_ind =
                      (mph_dtm_channel_release_ind_t *) message_ptr;

                    MSG_GERAN_HIGH_0_G("TBF released; Exited DTM");

                    rr_dtm_data_ptr->state = DTM_DTM_POSSIBLE;

                    if ( ! mph_dtm_channel_release_ind->ps_release )
                    {
                      MSG_GERAN_ERROR_0_G("Unexpected ps_release=FALSE");
                    }

                    if ( mph_dtm_channel_release_ind->cs_release )
                    {
                      MSG_GERAN_ERROR_0_G("Unexpected cs_release=TRUE");
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }
            break;
          }

          case EV_DTM_ASSIGNMENT_COMMAND_RECEIVED:
          {
            // respond_to_dtm_assignment_command() returns
            //  - EV_DTM_ASSIGNMENT_COMMAND_CHECKED if assignment ok
            //  - EV_DTM_ASSIGNMENT_ERROR if there are errors
            if (EV_DTM_ASSIGNMENT_COMMAND_CHECKED == respond_to_dtm_assignment_command(gas_id))
            {
              // Start the procedure to configure MAC/L1 with the new DTM Assignment
              (void) dtm_assign_command_control(EV_DTM_ASSIGNMENT_READY_IN_DTM, NULL, gas_id);

              // Move to DTM_PROCESS_ASSIGNMENT_COMMAND state to route the incoming messages appropriately
              rr_dtm_data_ptr->state = DTM_PROCESS_ASSIGNMENT_COMMAND;

              // Return EV_DTM_ASSIGNMENT_RECEIVED to the parent caller, which is rr_handle_procedures()
              event_out = EV_DTM_ASSIGNMENT_RECEIVED;
            }
            else
            {
              MSG_GERAN_HIGH_0_G("Error in DTM ASSIGNMENT COMMAND");
            }
            break;
          }

          case EV_DTM_PACKET_ASSIGNMENT_RECEIVED:
          {
            MSG_GERAN_ERROR_0_G("PACKET ASSIGNMENT unexpected in DTM_TRANSFER - ignored");
            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            MSG_GERAN_HIGH_0_G("Hand-over from DTM cell (in DTM transfer)");

            /* Inform cell capability monitor that a handover is in progress */
            /* This monitors for the reception of DTM INFORMATION / SI6 on the */
            /* new cell */
            (void) dtm_cell_capability_monitor(event_in, gas_id);

            rr_send_grr_mac_dtm_channel_release_ind(HANDOVER_REL, gas_id);

            rr_gmm_suspend_gprs(gas_id);
            rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

            rr_dtm_data_ptr->state = DTM_DED_PROC_IN_PROGRESS;
            break;
          }

#ifdef FEATURE_GSM_EDTM
          case EV_EDTM_ESTABLISHMENT_ABORTED:
          {
            rr_dtm_data_ptr->state = DTM_DTM_POSSIBLE;
            break;
          }

          case EV_EDTM_RELEASE_ABORTED:
          {
            (void) dtm_cell_capability_monitor(EV_RESET_SOFTWARE, gas_id);

            rr_send_grr_mac_dtm_channel_release_ind(NORMAL_REL, gas_id);

            rr_gmm_suspend_gprs(gas_id);
            rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

            // Reset local data and transition to NULL state
            rr_dtm_transition_to_null(gas_id);
            break;
          }

          case EV_DTM_CS_RELEASED:
          {
            /* Put L1 in MM non-DRX mode if this was requested my MM */
            /* This is done in case GMM was mid-procedure when the call was released */
            rr_gprs_set_l1_in_mm_non_drx(gas_id);

            // Reset local data and transition to NULL state
            rr_dtm_transition_to_null(gas_id);
            break;
          }
#endif /* FEATURE_GSM_EDTM */

          case EV_DTM_PS_ACCESS_REQ:
          case EV_DTM_PS_ACCESS_REQ_PURGE:
          {
            // A request to enable PS access received during DTM transfer is not an error.
            // This can happen due to T3142 expiry, where DTM transfer was via DL assignment
            break;
          }

          case EV_DTM_NO_PS_ACCESS_REQ:
          case EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL:
          {
            /**
             * The objective here is to remove the PS domain, but leave the ongoing dedicated connection 
             * intact. To this this, the TBF is released by blocking PS access in MAC/RLC via 
             * GRR_MAC_NO_PS_ACCESS_IND. 
             * Note: GRR_MAC_DTM_CHANNEL_RELEASE_IND is NOT sent here - this message indicates to MAC not to 
             * release L1, but we need the TBF released using normal Packet Transfer procedures. 
             */
            if (event_in == EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL)
            {
              rr_ps_send_imsg_no_ps_access_req(
                FALSE,     // suspend_gmm
                DEL_ALL,   // delete_pdu
                gas_id     // gas_id
              );
            }
            else
            {
              rr_ps_send_imsg_no_ps_access_req(
                FALSE,     // suspend_gmm
                NO_DEL,    // delete_pdu
                gas_id     // gas_id
              );
            }

            rr_dtm_data_ptr->data_ptr->no_ps_access_confirm_cb_fn = (rr_cb_fn_t)message_ptr;
            rr_dtm_data_ptr->state = DTM_DISABLING_PS_ACCESS;
            break;
          }

          case EV_DTM_MSC_CHANGE_IND:
          {
            /*
            The TBF should be released, but L1 left in Dedicated

            Note: GRR_MAC_DTM_CHANNEL_RELEASE_IND is not sent here, as this would result in the TBF being
            released locally in MAC/RLC, but leave L1 in DTM transfer (as it is assumed that L1 is about to
            receive a MPH_HANDOVER_REQ or MPH_DTM_CHANNEL_RELEASE_REQ)
            */

            // Request block of PS access (no purge)
            rr_ps_send_imsg_no_ps_access_req(
              FALSE,     // suspend_gmm
              NO_DEL,    // delete_pdu
              gas_id     // gas_id
            );

            // and wait for confirmation
            rr_dtm_data_ptr->state = DTM_MSC_CHANGE;
            break;
          }

          default: ;
        }

        break;
      } // DTM_DTM_TRANSFER

      case DTM_GTTP_TRANSFER:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        (void) dtm_cell_capability_monitor(EV_RESET_SOFTWARE, gas_id);
                        rr_dtm_data_ptr->released_in_gttp = TRUE;

                        // Note: Although not in DTM, MAC requires a DTM_CHANNEL_RELEASE_IND here
                        rr_send_grr_mac_dtm_channel_release_ind(NORMAL_REL, gas_id);

                        rr_gmm_suspend_gprs(gas_id);
                        rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

                        send_omsg_cs_release_cnf(
                          RR_DTM_CE_REL_MODE_GSM,   // ce_rel_mode
                          gas_id                    // gas_id
                        );

                        // Reset local data and transition to NULL state
                        rr_dtm_transition_to_null(gas_id);
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              case MS_RLC_RR:
              {
                switch (message_id)
                {
                  case RG_UL_DCCH_REQ:
                  {
                    rlc_grr_ul_dcch_req_t * ul_dcch_req = (rlc_grr_ul_dcch_req_t *)message_ptr;

                    send_gttp_data_ota(ul_dcch_req->tlli, &ul_dcch_req->gttp_llc_pdu_container, gas_id);
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }
            break;
          }

          case EV_DTM_L2_DATA_ACK_IND:
          {
            /* Acknowledge PDU transfer back to RLC/MAC */
            rr_send_grr_mac_dtm_ul_dcch_cnf(NO_ERROR, gas_id);
            rr_dtm_data_ptr->state = DTM_DTM_POSSIBLE;
            break;
          }

          case EV_DTM_ASSIGNMENT_COMMAND_RECEIVED:
          case EV_DTM_PACKET_ASSIGNMENT_RECEIVED:
          {
            MSG_GERAN_HIGH_0_G("Implicit L2 ack");

            /* Acknowledge PDU transfer back to RLC/MAC */
            rr_send_grr_mac_dtm_ul_dcch_cnf(NO_ERROR, gas_id);

            /* Process the DTM ASSIGNMENT COMMAND / PACKET ASSIGNMENT as if received */
            /* in DTM_POSSIBLE state */
            new_event = event_in;
            rr_dtm_data_ptr->state = DTM_DTM_POSSIBLE;
            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            MSG_GERAN_HIGH_0_G("Hand-over from DTM cell (in GTTP transfer)");

            /* Inform cell capability monitor that a handover is in progress */
            /* This monitors for the reception of DTM INFORMATION / SI6 on the */
            /* new cell */
            (void) dtm_cell_capability_monitor(event_in, gas_id);

            rr_send_grr_mac_dtm_channel_release_ind(HANDOVER_REL, gas_id);

            rr_gmm_suspend_gprs(gas_id);
            rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);

            rr_dtm_data_ptr->state = DTM_DED_PROC_IN_PROGRESS;
            break;
          }

          case EV_DTM_PS_ACCESS_REQ:
          case EV_DTM_PS_ACCESS_REQ_PURGE:
          {
            MSG_GERAN_ERROR_0_G("Unexpected EV_DTM_PS_ACCESS in GTTP_TRANSFER");
            break;
          }

          case EV_DTM_NO_PS_ACCESS_REQ:
          case EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL:
          {

#ifdef FEATURE_SGLTE  
            rr_send_grr_mac_dtm_channel_release_ind(NORMAL_REL,gas_id);
            if (event_in == EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL)
            {
            rr_send_grr_mac_no_ps_access_ind(DEL_ALL,gas_id);
            }
            else
            {
              rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);
            }
            CALL_RR_CB_FN((rr_cb_fn_t)message_ptr,gas_id);
            rr_dtm_data_ptr->state = DTM_NO_PS_DOMAIN;
#else

            MSG_GERAN_ERROR_0_G("Unhandled EV_DTM_NO_PS_ACCESS in GTTP_TRANSFER");
#endif/* FEATURE_SGLTE */
            break;
          }

          default: ;
        }

        break;
      } // DTM_GTTP_TRANSFER

      case DTM_DED_PROC_IN_PROGRESS:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        (void) dtm_cell_capability_monitor(EV_RESET_SOFTWARE, gas_id);

                        send_omsg_cs_release_cnf(
                          RR_DTM_CE_REL_MODE_GSM,   // ce_rel_mode
                          gas_id                    // gas_id
                        );

                        // Reset local data and transition to NULL state
                        rr_dtm_transition_to_null(gas_id);
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  case RR_PS_OMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        /**
                         * PS access was being disabled when a handover occurred, causing a switch to 
                         * DTM_DED_PROC_IN_PROGRESS. 
                         * The only action required is to call the confirmation call-back. 
                         */
                        CALL_RR_CB_FN(rr_dtm_data_ptr->data_ptr->no_ps_access_confirm_cb_fn, gas_id);
                        rr_dtm_data_ptr->data_ptr->no_ps_access_confirm_cb_fn = NULL;
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              case MS_MAC_RR:
              {
                switch (message_id)
                {
                  case MAC_GRR_PS_ACCESS_CNF:
                  {
                    /**
                     * Note: The scenario here is that a handover was started while PS access was being 
                     * enabled in DTM_ENABLING_PS_ACCESS state, so a MAC_GRR_PS_ACCESS_CNF is expected. 
                     */

                    // Confirm GPRS service to GMM
                    CALL_RR_CB_FN(rr_dtm_data_ptr->data_ptr->ps_access_confirm_cb_fn, gas_id);
                    rr_dtm_data_ptr->data_ptr->ps_access_confirm_cb_fn = NULL;

                    rr_gmm_suspend_gprs(gas_id);
                    rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);
                    break;
                  }

                  case MAC_GRR_MAC_STATUS:
                  {
                    rr_ps_access_control((const rr_cmd_bdy_type *)message_ptr, gas_id);
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }
            break;
          }

          case EV_G2W_HANDOVER:
          {
            /*
            When a G2W HO is started, two events are generated into this state
            machine: EV_INTRA_CELL_HANDOVER, followed by EV_G2W_HANDOVER.
            This means that all the processing relevant to an inter-cell HO is performed
            (GPRS is suspended in GMM and MAC, etc), but then as the HO is G2W, move
            to DTM_G2X_HANDOVER, and await confirmation of success or failure
            */
            rr_dtm_data_ptr->state = DTM_G2X_HANDOVER;
            break;
          }

          case EV_INTER_CELL_HANDOVER:
          {
            /*
            When a inter-cell HO is started, two events are generated into this state
            machine: EV_INTRA_CELL_HANDOVER, followed by EV_INTER_CELL_HANDOVER.
            This is because EV_INTRA_CELL_HANDOVER signals the start of a handover, but then
            when the details of the new cell are checked, EV_INTER_CELL_HANDOVER is sent if the
            cell will change
            */
            (void) dtm_cell_capability_monitor(event_in, gas_id);
            break;
          }

          case EV_HANDOVER_COMPLETED:
          {
            /*
            This will return EV_DTM_CELL_SUPPORTS_DTM / EV_DTM_CELL_DOES_NOT_SUPPORT_DTM in the case of
            intra-cell handover, because the DTM capability of the cell is known. This indication is passed
            into DTM_RR_CS_EST state, and the same action taken following initial CS conn est.
            In the case of inter-cell handover, the DTM capability of the cell is not known, and so no
            indication is returned (EV_NO_EVENT). SI6 / DTM INFORMATION must be received to know this.
            Therefore, assume no DTM support and move to DTM_CELL_DOES_NOT_SUPPORT_DTM to wait for
            SI6 / DTM INFORMATION
            */
            if (dtm_cell_capability_monitor(EV_HANDOVER_COMPLETED, gas_id) == EV_DTM_CELL_SUPPORTS_DTM)
            {
              // Resume GMM (if required)
              (void) rr_gmm_resume_gprs(gas_id);

              rr_dtm_data_ptr->state = DTM_NO_PS_DOMAIN;
            }
            else
            {
              rr_dtm_data_ptr->state = DTM_CELL_DOES_NOT_SUPPORT_DTM;
            }
            break;
          }

          case EV_DTM_PS_ACCESS_REQ:
          case EV_DTM_PS_ACCESS_REQ_PURGE:
          {
            // RR procedure in progress so PS access remains blocked

            /*
            GMM would normally be suspended during a handover. However, if GMM had not yet requested GPRS
            service (i.e. sent RR_GMM_GPRS_SERVICE_REQ), then it would not have been suspended.
            So if a GPRS service request is received during CS connection establishment (which it could be
            as GMM is not suspended), suspend GMM now.
            */
            rr_gmm_suspend_gprs(gas_id);

            // Send any confirmation required
            CALL_RR_CB_FN((rr_cb_fn_t)message_ptr, gas_id);

            if (event_in == EV_DTM_PS_ACCESS_REQ_PURGE)
            {
              rr_dtm_data_ptr->data_ptr->purge_gmm_signalling = TRUE;
            }
            break;
          }

          case EV_DTM_NO_PS_ACCESS_REQ:
          case EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL:
          {
            if (EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL == event_in)
          {
              // Ensure GMAC was blocked with DEL ALL indication (no need to wait for a response)
              rr_ps_no_ps_access_del_all(gas_id);
            }

            // RR procedure in progress so PS access already blocked
            break;
          }

          default: ;
        }

        break;
      } // DTM_DED_PROC_IN_PROGRESS

      case DTM_NO_PS_DOMAIN:
      {
        switch ( event_in )
        {
          RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        (void) dtm_cell_capability_monitor(EV_RESET_SOFTWARE, gas_id);

                        // Suspend GMM. PS access is already blocked
                        rr_gmm_suspend_gprs(gas_id);

                        send_omsg_cs_release_cnf(
                          RR_DTM_CE_REL_MODE_GSM,   // ce_rel_mode
                          gas_id                    // gas_id
                        );

                        // Reset local data and transition to NULL state
                        rr_dtm_transition_to_null(gas_id);
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }
            break;
          }

          case EV_DTM_INFORMATION_RECEIVED:
          {
            (void) dtm_cell_capability_monitor(EV_DTM_INFORMATION_RECEIVED, gas_id);
            break;
          }

          case EV_DTM_SI6_RECEIVED:
          {
            if (EV_DTM_CELL_DOES_NOT_SUPPORT_DTM == dtm_cell_capability_monitor(EV_DTM_SI6_RECEIVED, gas_id))
            {
              /* SI6 has been received on the new cell, and indicates no DTM support */

              // Suspend GMM. PS access is already blocked
              rr_gmm_suspend_gprs(gas_id);
              rr_dtm_data_ptr->state = DTM_CELL_DOES_NOT_SUPPORT_DTM;
            }
            break;
          }

          case EV_DTM_ASSIGNMENT_COMMAND_RECEIVED:
          {
            // Reject the DTM ASSIGNMENT COMMAND while waiting to enable PS access
            send_dtm_assign_failure_ota(
              RR_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,    // cause
              FALSE,                                             // l1_reconnected
              FALSE,                                             // l2_suspended
              gas_id
            );
            break;
          }

          case EV_DTM_PACKET_ASSIGNMENT_RECEIVED:
          {
            // Ignore the PACKET ASSIGNMENT while waiting to enable PS acccess
            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            /* Inform cell capability monitor that a handover is in progress */
            /* This monitors for the reception of DTM INFORMATION / SI6 on the */
            /* new cell */
            (void) dtm_cell_capability_monitor(event_in, gas_id);

            // Suspend GMM. PS access is already blocked
            rr_gmm_suspend_gprs(gas_id);

            // Wait for the procedure to finish in DTM_DED_PROC_IN_PROGRESS
            rr_dtm_data_ptr->state = DTM_DED_PROC_IN_PROGRESS;
            break;
          }

          case EV_DTM_PS_ACCESS_REQ:
          {
            // GMM is requesting GPRS service

            // Save the confirmaton call-back function (could be NULL)
            rr_dtm_data_ptr->data_ptr->ps_access_confirm_cb_fn = (rr_cb_fn_t)message_ptr;

            // Enable PS access
            // Note: This is 'no purge' in most cases, but there is a corner-case where DTM support
            // indication in SI6 changes but the cell doesn't change, then the action is to purge
            send_grr_mac_ps_access_req(gas_id);

            // Move to DTM_ENABLING_PS_ACCESS to wait for PS access confirmation
            rr_dtm_data_ptr->state = DTM_ENABLING_PS_ACCESS;
            break;
          }

          case EV_DTM_PS_ACCESS_REQ_PURGE:
          {
            MSG_GERAN_ERROR_0_G("Unexpected EV_DTM_PS_ACCESS in NO_PS_DOMAIN");
            break;
          }

          case EV_DTM_NO_PS_ACCESS_REQ:
          case EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL:
          {
            // Cell does not support DTM so PS access is already blocked

            if (EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL == event_in)
            {
              // Ensure GMAC was blocked with DEL ALL indication (no need to wait for a response)
              rr_ps_no_ps_access_del_all(gas_id);
            }

            // Send any confirmation required
            CALL_RR_CB_FN((rr_cb_fn_t)message_ptr, gas_id);
            break;
          }

          case EV_DTM_MSC_CHANGE_IND:
          {
            /*
            Update NAS as required
            RR is camped, so a RR_SERVICE_IND can be sent to MM if required
            */
            rr_msc_update(
              TRUE,    // can_send_service_ind
              gas_id   // gas_id
            );
            break;
          }

          default: ;
        }

        break;
      } // DTM_NO_PS_DOMAIN

      case DTM_CELL_DOES_NOT_SUPPORT_DTM:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        (void) dtm_cell_capability_monitor(EV_RESET_SOFTWARE, gas_id);

                        send_omsg_cs_release_cnf(
                          RR_DTM_CE_REL_MODE_GSM,   // ce_rel_mode
                          gas_id                    // gas_id
                        );

                        // Reset local data and transition to NULL state
                        rr_dtm_transition_to_null(gas_id);
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }
            break;
          }

          case EV_DTM_INFORMATION_RECEIVED:
          case EV_DTM_SI6_RECEIVED:
          {
            if (EV_DTM_CELL_SUPPORTS_DTM == dtm_cell_capability_monitor(event_in, gas_id))
            {
              // Resume GPRS in GMM
              (void) rr_gmm_resume_gprs(gas_id);

              // MM has been informed of the DTM support in the cell via a RR_SERVICE_IND_FROM_OTA_MSG.
              // Therefore a RR_GMM_GPRS_SERVICE_REQ is expected to enable PS access
              rr_dtm_data_ptr->state = DTM_NO_PS_DOMAIN;
            }
            break;
          }

          case EV_DTM_ASSIGNMENT_COMMAND_RECEIVED:
          {
            // Reject the DTM ASSIGNMENT COMMAND while waiting for DTM INFORMATION / SI6
            send_dtm_assign_failure_ota(
              RR_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,    // cause
              FALSE,                                             // l1_reconnected
              FALSE,                                             // l2_suspended
              gas_id
            );
            break;
          }

          case EV_DTM_PACKET_ASSIGNMENT_RECEIVED:
          {
            // Ignore the PACKET ASSIGNMENT while waiting for DTM INFORMATION / SI6
            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            /* Inform cell capability monitor that a handover is in progress */
            /* This monitors for the reception of DTM INFORMATION / SI6 on the */
            /* new cell */
            (void) dtm_cell_capability_monitor(event_in, gas_id);

            rr_dtm_data_ptr->state = DTM_DED_PROC_IN_PROGRESS;
            break;
          }

          case EV_DTM_PS_ACCESS_REQ:
          case EV_DTM_PS_ACCESS_REQ_PURGE:
          {
            // Cell does not support DTM so PS access remains blocked

            // Note: If this request is due to GMM requesting GPRS service(e.g. NAS merge),suspend GMM
            // as GMM is not always going to be suspended

            if (event_in == EV_DTM_PS_ACCESS_REQ_PURGE)
            {
              rr_dtm_data_ptr->data_ptr->purge_gmm_signalling = TRUE;
            }

            // Send an indication to NAS that PS is not supported by cell
            rr_gmm_suspend_gprs(gas_id);

            // Send any confirmation required
            CALL_RR_CB_FN((rr_cb_fn_t)message_ptr, gas_id);
            break;
          }

          case EV_DTM_NO_PS_ACCESS_REQ:
          case EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL:
          {
            // Cell does not support DTM so PS access already blocked

            if (EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL == event_in)
            {
              // Ensure GMAC was blocked with DEL ALL indication (no need to wait for a response)
              rr_ps_no_ps_access_del_all(gas_id);
            }

            // Send any confirmation required
            CALL_RR_CB_FN((rr_cb_fn_t)message_ptr, gas_id);
            break;
          }

          case EV_DTM_MSC_CHANGE_IND:
          {
            /*
            Update NAS as required
            RR is camped, so a RR_SERVICE_IND can be sent to MM if required
            */
            rr_msc_update(
              TRUE,    // can_send_service_ind
              gas_id   // gas_id
            );
            break;
          }

          default: ;
        }

        break;
      } // DTM_CELL_DOES_NOT_SUPPORT_DTM

      case DTM_NO_PS_ACCESS_UL_BLOCKED:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        (void) dtm_cell_capability_monitor(EV_RESET_SOFTWARE, gas_id);

                        // Suspend GMM. PS access is already blocked
                        rr_gmm_suspend_gprs(gas_id);

                        send_omsg_cs_release_cnf(
                          RR_DTM_CE_REL_MODE_GSM,   // ce_rel_mode
                          gas_id                    // gas_id
                        );

                        // Reset local data and transition to NULL state
                        rr_dtm_transition_to_null(gas_id);
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }
            break;
          }

          case EV_DTM_INFORMATION_RECEIVED:
          case EV_DTM_SI6_RECEIVED:
          {
            new_event = dtm_cell_capability_monitor(event_in, gas_id);
            break;
          }

          case EV_DTM_ASSIGNMENT_COMMAND_RECEIVED:
          case EV_DTM_PACKET_ASSIGNMENT_RECEIVED:
          {
            rr_event_T process_result;

            if (event_in == EV_DTM_ASSIGNMENT_COMMAND_RECEIVED)
            {
              process_result = respond_to_dtm_assignment_command(gas_id);
            }
            else
            {
              process_result = process_packet_assignment(gas_id);
            }

            if (process_result != EV_DTM_ASSIGNMENT_ERROR)
            {
              // Hold the new DTM Assignment pending while PS access is enabled
              new_event = process_result;
            }

            // Note: Because the network has 'pushed' the MS into a packet procedure, this implies
            // that no GMM signalling is expected. Therefore PS access is granted without co-odination
            // with GMM. Also it doesn't matter if T3142 is running

            // Enable PS access and move to DTM_ENABLING_PS_ACCESS
            rr_send_grr_mac_ps_access_req(
              TRUE,    // purge_gmm_signalling
              gas_id   // gas_id
            );

            rr_dtm_data_ptr->state =  DTM_ENABLING_PS_ACCESS;
            break;
          }

          case EV_DTM_CELL_SUPPORTS_DTM:
          {
            break;  // no action required
          }

          case EV_DTM_CELL_DOES_NOT_SUPPORT_DTM:
          {
            /* SI6 has been received on the new cell, and indicates no DTM support */

            // Suspend GMM. PS access is already blocked
            rr_gmm_suspend_gprs(gas_id);
            rr_dtm_data_ptr->state = DTM_CELL_DOES_NOT_SUPPORT_DTM;
            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            /* Inform cell capability monitor that a handover is in progress */
            /* This monitors for the reception of DTM INFORMATION / SI6 on the */
            /* new cell */
            (void) dtm_cell_capability_monitor(event_in, gas_id);

            // Suspend GMM. PS access is already blocked
            rr_gmm_suspend_gprs(gas_id);

            // Wait for the procedure to finish in DTM_DED_PROC_IN_PROGRESS
            rr_dtm_data_ptr->state = DTM_DED_PROC_IN_PROGRESS;
            break;
          }

          case EV_DTM_PS_ACCESS_REQ:
          {
            MSG_GERAN_ERROR_0_G("Unexpected EV_DTM_PS_ACCESS in PS_ACCESS_UL_BLOCKED");
            break;
          }

          case EV_DTM_PS_ACCESS_REQ_PURGE:
          {
            // T3142 has expired. PS access can be allowed again

            // Enable PS access (purge)
            rr_send_grr_mac_ps_access_req(
              TRUE,    // purge_gmm_signalling
              gas_id   // gas_id
            );

            // Move to DTM_ENABLING_PS_ACCESS to wait for PS access confirmation
            rr_dtm_data_ptr->state = DTM_ENABLING_PS_ACCESS;
            break;
          }

          case EV_DTM_NO_PS_ACCESS_REQ:
          case EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL:
          {
            // PS access is currently blocked, but now GPRS service is no longer required by GMM

            if (EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL == event_in)
            {
              // Ensure GMAC was blocked with DEL ALL indication (no need to wait for a response)
              rr_ps_no_ps_access_del_all(gas_id);
            }

            // Send any confirmation required
            CALL_RR_CB_FN((rr_cb_fn_t)message_ptr, gas_id);

            rr_dtm_data_ptr->state = DTM_NO_PS_DOMAIN;
            break;
          }

          case EV_DTM_MSC_CHANGE_IND:
          {
            /*
            Update NAS as required
            RR is camped, so a RR_SERVICE_IND can be sent to MM if required
            */
            rr_msc_update(
              TRUE,    // can_send_service_ind
              gas_id   // gas_id
            );

            // PS access is currently blocked, but now GPRS service should be re-requested by GMM
            rr_dtm_data_ptr->state = DTM_NO_PS_DOMAIN;
            break;
          }

          default: ;
        }

        break;
      } // DTM_NO_PS_ACCESS_UL_BLOCKED

      case DTM_MSC_CHANGE:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        (void) dtm_cell_capability_monitor(EV_RESET_SOFTWARE, gas_id);

                        // Suspend GMM. PS access is already blocked
                        rr_gmm_suspend_gprs(gas_id);

                        // Update NAS as required
                        rr_msc_update(
                          FALSE,   // can_send_service_ind
                          gas_id   // gas_id
                        );

                        send_omsg_cs_release_cnf(
                          RR_DTM_CE_REL_MODE_GSM,   // ce_rel_mode
                          gas_id                    // gas_id
                        );

                        // Reset local data and transition to NULL state
                        rr_dtm_transition_to_null(gas_id);
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  case RR_PS_OMSG:
                  {
                    switch (rr_rr_msg_ptr->ps.omsg.header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        /*
                        Update NAS as required
                        RR is camped, so a RR_SERVICE_IND can be sent to MM if required
                        */
                        rr_msc_update(
                          TRUE,    // can_send_service_ind
                          gas_id   // gas_id
                        );

                        rr_dtm_data_ptr->state = DTM_NO_PS_DOMAIN;
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              case MS_MAC_RR:
              {
                switch (message_id)
                {
                  case MAC_GRR_MAC_STATUS:
                  {
                    rr_ps_access_control((const rr_cmd_bdy_type *)message_ptr, gas_id);
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }
            break;
          }

          case EV_DTM_INFORMATION_RECEIVED:
          case EV_DTM_SI6_RECEIVED:
          {
            // Keep track of the cell capability - this may return EV_DTM_CELL_DOES_NOT_SUPPORT_DTM
            new_event = dtm_cell_capability_monitor(event_in, gas_id);
            break;
          }

          case EV_DTM_PACKET_ASSIGNMENT_RECEIVED:
          case EV_DTM_ASSIGNMENT_COMMAND_RECEIVED:
          {
            MSG_GERAN_HIGH_0_G("Assignment ignored during MSC change");
            break;
          }

          case EV_DTM_CELL_DOES_NOT_SUPPORT_DTM:
          {
            /* SI6 has been received on the new cell, and indicates no DTM support */

            // Suspend GMM (if not already). PS access is already blocked
            rr_gmm_suspend_gprs(gas_id);

            // Update NAS as required
            rr_msc_update(
              TRUE,    // can_send_service_ind
              gas_id   // gas_id
            );

            rr_dtm_data_ptr->state = DTM_CELL_DOES_NOT_SUPPORT_DTM;
            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            /*
            Inform cell capability monitor that a handover is in progress
            This monitors for the reception of DTM INFORMATION / SI6 on the new cell
            */
            (void) dtm_cell_capability_monitor(event_in, gas_id);

            // Suspend GMM (if not already). PS access is already blocked
            rr_gmm_suspend_gprs(gas_id);

            /*
            Update NAS as required
            RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
            This will be sent when RR/L1 enter Idle
            */
            rr_msc_update(
              TRUE,    // can_send_service_ind
              gas_id   // gas_id
            );

            // Wait for the procedure to finish in DTM_DED_PROC_IN_PROGRESS
            rr_dtm_data_ptr->state = DTM_DED_PROC_IN_PROGRESS;
            break;
          }

          case EV_DTM_PS_ACCESS_REQ:
          case EV_DTM_PS_ACCESS_REQ_PURGE:
          {
            // PS access requested - this could be due to T3142 expiring
            break;
          }

          case EV_DTM_NO_PS_ACCESS_REQ:
          case EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL:
          {
            // PS block requested - this could be due to change to CS-only

            // Send any confirmation required
            CALL_RR_CB_FN((rr_cb_fn_t)message_ptr, gas_id);
            break;
          }

          default: ;
        }

        break;
      } // DTM_MSC_CHANGE

      case DTM_ENABLING_PS_ACCESS:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_MAC_RR:
              {
                switch (message_id)
                {
                  case MAC_GRR_PS_ACCESS_CNF:
                  {
                    // Send any confirmation required
                    CALL_RR_CB_FN(rr_dtm_data_ptr->data_ptr->ps_access_confirm_cb_fn, gas_id);
                    rr_dtm_data_ptr->data_ptr->ps_access_confirm_cb_fn = NULL;

                    new_event = rr_dtm_data_ptr->data_ptr->pending_action;
                    rr_dtm_data_ptr->data_ptr->pending_action = EV_NO_EVENT;

                    rr_dtm_data_ptr->state = DTM_DTM_POSSIBLE;
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }

            /**
             * Note: Following messages are blocked in this state: 
             *  
             *  RR_DTM_IMSG_CS_RELEASE_REQ  (see rr_dtm_can_process_cs_release_req())
             */

            break;
          }

          case EV_DTM_INFORMATION_RECEIVED:
          {
            /**
             * Note: The scenario for receiving DTM INFORMATION in this state is that following a handover, 
             * SI6 has been received first which indicates DTM support. As DTM INFORMATION contains system 
             * information required for DTM, it is processed, but PS access is already in the process of being 
             * enabled so there are no state changes required. 
             */
            (void) dtm_cell_capability_monitor(event_in, gas_id);
            break;
          }

          case EV_DTM_SI6_RECEIVED:
          {
            /**
             * Note: Reception of SI6 is deliberately ignored here, in case it indicates that the cell no 
             * longer supports DTM. This is problematic, as PS can't be blocked via GRR_MAC_NO_PS_ACCESS_IND 
             * right now, as MAC_GRR_PS_ACCESS_CNF is expected and needs to be received before sending another 
             * message. 
             * So ignore SI6 - it can be acted on when it is next received. 
             */
            break;
          }

          case EV_DTM_PACKET_ASSIGNMENT_RECEIVED:
          case EV_DTM_ASSIGNMENT_COMMAND_RECEIVED:
          case EV_DTM_PACKET_ASSIGNMENT_CHECKED:
          case EV_DTM_ASSIGNMENT_COMMAND_CHECKED:
          {
            rr_dtm_data_ptr->data_ptr->pending_action = event_in;
            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            MSG_GERAN_HIGH_0_G("Handover while enabling PS access");

            /* Inform cell capability monitor that a handover is in progress */
            /* This monitors for the reception of DTM INFORMATION / SI6 on the */
            /* new cell */
            (void) dtm_cell_capability_monitor(event_in, gas_id);

            // Wait for the procedure to finish in DTM_DED_PROC_IN_PROGRESS
            rr_dtm_data_ptr->state = DTM_DED_PROC_IN_PROGRESS;

            /**
             * Note: There is a handler for the expected MAC_GRR_PS_ACCESS_CNF in DTM_DED_PROC_IN_PROGRESS
             */
            break;
          }

          case EV_DTM_PS_ACCESS_REQ:
          case EV_DTM_PS_ACCESS_REQ_PURGE:
          {
            // GMM has already requested GPRS service

            // Send any confirmation required
            CALL_RR_CB_FN((rr_cb_fn_t)message_ptr, gas_id);
            break;
          }

          /**
           * Note: Following events are blocked in this state: 
           *  
           *  EV_DTM_NO_PS_ACCESS_REQ          (see rr_dtm_can_process_no_ps_access_req())
           *  EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL  (see rr_dtm_can_process_no_ps_access_req())
           *  EV_DTM_PS_ACCESS_REQ             (see rr_dtm_can_process_ps_access_req())
           */

          default: ;
        }

        break;
      } // DTM_ENABLING_PS_ACCESS

      case DTM_DISABLING_PS_ACCESS:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = (rr_rr_msg_u *)message_ptr;

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_PS_OMSG:
                  {
                    switch (rr_rr_msg_ptr->ps.omsg.header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        // Send any confirmation required
                        CALL_RR_CB_FN(rr_dtm_data_ptr->data_ptr->no_ps_access_confirm_cb_fn, gas_id);
                        rr_dtm_data_ptr->data_ptr->no_ps_access_confirm_cb_fn = NULL;

                        new_event = rr_dtm_data_ptr->data_ptr->pending_action;
                        rr_dtm_data_ptr->data_ptr->pending_action = EV_NO_EVENT;

                        rr_dtm_data_ptr->state = DTM_NO_PS_DOMAIN;
                        break;
                      }
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              case MS_MAC_RR:
              {
                switch (message_id)
                {
                  case MAC_GRR_MAC_STATUS:
                  {
                    rr_ps_access_control((const rr_cmd_bdy_type *)message_ptr, gas_id);
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }

            /**
             * Note: Following messages are blocked in this state: 
             *  
             *  RR_DTM_IMSG_CS_RELEASE_REQ       (see rr_dtm_can_process_cs_release_req())
             */

            break;
          }

          case EV_INTRA_CELL_HANDOVER:
          {
            MSG_GERAN_HIGH_0_G("Handover while disabling PS access");

            /* Inform cell capability monitor that a handover is in progress */
            /* This monitors for the reception of DTM INFORMATION / SI6 on the */
            /* new cell */
            (void) dtm_cell_capability_monitor(event_in, gas_id);

            // Wait for the procedure to finish in DTM_DED_PROC_IN_PROGRESS
            rr_dtm_data_ptr->state = DTM_DED_PROC_IN_PROGRESS;

            /**
             * Note: There is a handler for the expected RR_PS_OMSG_NO_PS_ACCESS_CNF in 
             * DTM_DED_PROC_IN_PROGRESS 
             */
            break;
          }

          case EV_DTM_SI6_RECEIVED:
          {
            /**
             * Note: Reception of SI6 is deliberately ignored here, in case it indicates some change in DTM 
             * capability of the cell. This is problematic, as PS is in the process of being blocked. So 
             * ignore SI6 - it can be acted on when it is next received. 
             */
            break;
          }

          /**
           * Note: Following events are blocked in this state: 
           *  
           *  EV_DTM_NO_PS_ACCESS_REQ          (see rr_dtm_can_process_no_ps_access_req())
           *  EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL  (see rr_dtm_can_process_no_ps_access_req())
           *  EV_DTM_PS_ACCESS_REQ             (see rr_dtm_can_process_ps_access_req())
           */

          default: ;
        }

        break;
      } // DTM_DISABLING_PS_ACCESS

      case DTM_G2X_HANDOVER:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &(((rr_cmd_bdy_type *)message_ptr)->rr);

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_IMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_IMSG_CS_RELEASE_REQ:
                      {
                        /**
                         * RR_DTM_IMSG_CS_RELEASE_REQ can be received here if the G2X HO is aborted by MM via 
                         * a RR_ABORT_REQ. 
                         */

                        (void) dtm_cell_capability_monitor(EV_RESET_SOFTWARE, gas_id);

                        send_omsg_cs_release_cnf(
                          RR_DTM_CE_REL_MODE_GSM,   // ce_rel_mode
                          gas_id                    // gas_id
                        );

                        // Reset local data and transition to NULL state
                        rr_dtm_transition_to_null(gas_id);
                        break;
                      }

                      default: ;
                    }
                    break;
                  }

                  default: ;
                }
                break;
              }

              case MS_MAC_RR:
              {
                switch (message_id)
                {
                  case MAC_GRR_PS_ACCESS_CNF:
                  {
                    /**
                     * Note: The scenario here is that a handover was started while PS access was being 
                     * enabled in DTM_ENABLING_PS_ACCESS state, so a MAC_GRR_PS_ACCESS_CNF is expected. 
                     */

                    // Confirm GPRS service to GMM
                    CALL_RR_CB_FN(rr_dtm_data_ptr->data_ptr->ps_access_confirm_cb_fn, gas_id);
                    rr_dtm_data_ptr->data_ptr->ps_access_confirm_cb_fn = NULL;

                    rr_gmm_suspend_gprs(gas_id);
                    rr_send_grr_mac_no_ps_access_ind(NO_DEL, gas_id);
                    break;
                  }

                  default: ;
                }
                break;
              }

              default: ;
            }
            break;
          }

          case RR_EV_G2W_HO_SUCCESS:
          {
            // Reset local data and transition to NULL state
            rr_dtm_transition_to_null(gas_id);
            break;
          }

          case RR_EV_G2W_HO_FAIL:
          {
            /*
            Inform dtm_cell_capability_monitor of G2W failure. This will return to
            either DTM_CELL_DTM_SUPPORTED or DTM_CELL_DTM_NOT_SUPPORTED, depending on
            whether the cell was previously DTM-capable.
            Move to DTM_DED_PROC_IN_PROGRESS, but don't enable PS yet if DTM was supported
            (which is returned by dtm_cell_capability_monitor here, but ignored),
            because L2 is not yet resumed.
            Wait for the reconnection of L1 & L2 to complete, which will result in
            EV_HANDOVER_COMPLETED posted into rr_dtm_control from rr_procedures.
            Then enable PS if required */
            (void) dtm_cell_capability_monitor(RR_EV_G2W_HO_FAIL, gas_id);
            rr_dtm_data_ptr->released_in_gttp = FALSE;
            rr_dtm_data_ptr->state            = DTM_DED_PROC_IN_PROGRESS;
            break;
          }

          default: ;
        }

        break;
      } // DTM_G2X_HANDOVER

      default:
        MSG_GERAN_ERROR_1_G("In unknown state (%d)",(int)rr_dtm_data_ptr->state);
    }

#ifdef DEBUG_RR_TRACE_MSG
    /* no old_state in this state machine: NOTE event conversion */
    if (((event_in != new_event) && (new_event != EV_NO_EVENT)) ||
        (rr_dtm_data_ptr->old_state != rr_dtm_data_ptr->state))
    {
      rr_store_trace_msg_buf(
        RR_DTM_SM,
        new_event,
        (byte)rr_dtm_data_ptr->state,
        message_ptr,
        gas_id
      );
    }
#endif

    if ( new_event != EV_NO_EVENT )
    {
      event_in = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event_in = EV_NO_EVENT;
    }

    if (rr_dtm_data_ptr->state != rr_dtm_data_ptr->old_state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_dtm_control",
        rr_dtm_data_ptr->old_state,
        rr_dtm_data_ptr->state,
        get_dtm_control_state_name,
        gas_id
      );

      rr_dtm_data_ptr->old_state = rr_dtm_data_ptr->state;
    }
  }

  return event_out;

} /* end rr_dtm_control() */


#ifdef FEATURE_WCDMA
/*!
 * \brief Called when a W2G handover is in progress. 
 *  
 * This functions sets up the PS data path to potentially go into PS transfer if the new cell supports DTM.
 * 
 * \param ps_attached (in)
 * \param gas_id (in)
 */
void rr_dtm_w2g_ho(boolean ps_attached, const gas_id_t gas_id)
{
  /* Suspend GPRS in GMM (if the PS domain is active) until SI6 / DTM INFORMATION is received */
  rr_gmm_suspend_gprs(gas_id);

  if (ps_attached)
  {
    (void) rr_dtm_control(EV_DTM_W2G_HO_PS_ATTACHED, NULL, gas_id);
  }
  else
  {
    (void) rr_dtm_control(EV_DTM_W2G_HO, NULL, gas_id);
  }

} /* end rr_dtm_w2g_ho() */
#endif // FEATURE_WCDMA


/*!
 * \brief Notifies rr_dtm_control of a CS establishment.
 * 
 * \param gas_id 
 */
void rr_dtm_send_imsg_cs_est_ind(const gas_id_t gas_id)
{
  if (rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID))
  {
    (void) rr_dtm_control(EV_RR_EST_IND, NULL, gas_id);
  }

} /* end rr_dtm_send_imsg_cs_est_ind() */


/*!
 * \brief Notifies rr_dtm_control of a CS connection established.
 * 
 * \param gas_id 
 */
void rr_dtm_send_imsg_cs_conn_ind(const gas_id_t gas_id)
{
  if (rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID))
  {
    (void) rr_dtm_control(EV_DTM_RR_EST_CNF, NULL, gas_id);
  }

} /* end rr_dtm_send_imsg_cs_conn_ind() */


/*!
 * \brief Notifies rr_dtm_control of a CS release.
 * 
 * \param gas_id 
 */
void rr_dtm_send_imsg_cs_rel_ind(const gas_id_t gas_id)
{
  if (rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID))
  {
    (void) rr_dtm_control(EV_DTM_CS_RELEASED, NULL, gas_id);
  }

} /* end rr_dtm_send_imsg_cs_rel_ind() */


/*!
 * \brief Notifies rr_dtm_control of a L2 ack for a GTTP transfer.
 * 
 * \param gas_id 
 */
void rr_dtm_send_imsg_l2_gttp_ack_ind(const gas_id_t gas_id)
{
  (void) rr_dtm_control(EV_DTM_L2_DATA_ACK_IND, NULL, gas_id);

} /* end rr_dtm_send_imsg_l2_gttp_ack_ind() */


/*!
 * \brief Called when L1 confirms a DTM or Packet Assignment.
 * 
 * \param mac_status (in)
 * \param gas_id (in)
 */
void rr_dtm_dual_transfer_confirmed(rr_mac_status_t mac_status, const gas_id_t gas_id)
{
  boolean grr_controlled;

  /*  DTM has been initiated if:
   *    => RR is in state RR_GRR (i.e. this was an Enhanced Establishment)
   *   OR
   *    => GMAC is NOT already in established packet transfer (Rel4 DTM)
   */
  grr_controlled = rr_is_control_state(RR_GRR, gas_id);
  if ((grr_controlled == TRUE) || (mac_status != MAC_TRANSFER))
  {
#ifdef FEATURE_GSM_EDTM
    rr_gprs_edtm_dual_transfer_started(grr_controlled, gas_id);
#else
    MSG_GERAN_MED_1_G("Entered Dual Transfer Mode (%1d)", (int)grr_controlled);
#endif /* FEATURE_GSM_EDTM */
  }

} /* rr_dtm_dual_transfer_confirmed */


/*!
 * \brief Checks is RR-DTM-CONTROL is in a suitable state to handle EV_DTM_MSC_CHANGE_IND.
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE is the message can be handled; FALSE otherwise
 */
boolean rr_dtm_can_process_msc_change_ind(const gas_id_t gas_id)
{
  boolean process = TRUE;
  rr_dtm_static_data_t *rr_dtm_data_ptr;

  // Obtain a pointer to the module data
  rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  switch (rr_dtm_data_ptr->state)
  {
    case DTM_ENABLING_PS_ACCESS:
    case DTM_DISABLING_PS_ACCESS:
    case DTM_PROCESS_PACKET_ASSIGNMENT:
    case DTM_PROCESS_ASSIGNMENT_COMMAND:
    case DTM_GTTP_TRANSFER:
    case DTM_DED_PROC_IN_PROGRESS:
    case DTM_MSC_CHANGE:
    {
      process = FALSE;
      break;
    }

    default: ;
  }

  return process;
}


/*!
 * \brief Checks if RR-DTM-CONTROL is in a suitable state to handle EV_DTM_NO_PS_ACCESS_REQ.
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if the message can be handled, FALSE otherwise
 */
boolean rr_dtm_can_process_no_ps_access_req(const gas_id_t gas_id)
{
  boolean process = TRUE;
  rr_dtm_static_data_t *rr_dtm_data_ptr;

  // Obtain a pointer to the module data
  rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  switch (rr_dtm_data_ptr->state)
  {
    case DTM_ENABLING_PS_ACCESS:
    case DTM_DISABLING_PS_ACCESS:
    case DTM_PROCESS_PACKET_ASSIGNMENT:
    case DTM_PROCESS_ASSIGNMENT_COMMAND:
    case DTM_MSC_CHANGE:
    {
      process = FALSE;
      break;
    }

    default: ;
  }

  return process;
}


/*!
 * \brief Checks if RR-DTM-CONTROL is in a suitable state to handle EV_DTM_PS_ACCESS_REQ.
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if the message can be handled, FALSE otherwise
 */
boolean rr_dtm_can_process_ps_access_req(const gas_id_t gas_id)
{
  boolean process = TRUE;
  rr_dtm_static_data_t *rr_dtm_data_ptr;

  // Obtain a pointer to the module data
  rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  switch (rr_dtm_data_ptr->state)
  {
    case DTM_ENABLING_PS_ACCESS:
    case DTM_DISABLING_PS_ACCESS:
    case DTM_MSC_CHANGE:
    {
      process = FALSE;
      break;
    }

    default: ;
  }

  return process;
}


/*!
 * \brief Checks if RR-DTM-CONTROL is in a suitable state to handle RR_DTM_IMSG_CS_RELEASE_REQ.
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if the message can be handled, FALSE otherwise
 */
boolean rr_dtm_can_process_cs_release_req(const gas_id_t gas_id)
{
  boolean process = TRUE;
  rr_dtm_static_data_t *rr_dtm_data_ptr;

  // Obtain a pointer to the module data
  rr_dtm_data_ptr = rr_dtm_get_data_ptr(gas_id);

  switch (rr_dtm_data_ptr->state)
  {
    case DTM_ENABLING_PS_ACCESS:
    case DTM_DISABLING_PS_ACCESS:
    {
      process = FALSE;
      break;
    }

    default: ;
  }

  return process;
}


/*!
 * \brief Initialisation function
 * 
 * \param gas_id 
 */
void rr_dtm_init(const gas_id_t gas_id)
{
  (void) rr_dtm_control(EV_RESET_SOFTWARE, NULL, gas_id);

} /* end rr_dtm_init() */


/*!
 * \brief Initialisation function. Should be called once at RR task start.
 */
void rr_dtm_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(rr_dtm_data, 0, sizeof(rr_dtm_data));

} /* end rr_dtm_task_start_init() */


/* ------------------  Functions called from outside GERAN  --------------- */


/*!
 * \brief Returns the number of octets that can be transferred via GTTP. 
 *  
 * If not currently in a CS connection on a DTM-capable cell, then zero will be returned. 
 * Note: This function is called from outside GERAN* 
 * 
 * \return uint16 - number of octets
 */
uint16 rr_dtm_get_max_LAPDm_octets_per_subs(sys_modem_as_id_e_type as_id)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);
  gprs_scell_info_t *cell_info = rr_gprs_get_scell_info(gas_id);

  if (rr_in_dedicated(gas_id) && cell_info->dtm_sys_info_valid)
  {
    uint16 max_octets_per_LAPDm_frame, max_LAPDm_frames, max_num_of_octets;

    /* See gttp_criteria_met() for comments about L2 franes, etc */

    max_octets_per_LAPDm_frame = N201_DCCH;

    /* Get the max_LAPDm value broadcast in DTM INFORMATION / SI6 */
    max_LAPDm_frames = get_max_LAPDm_frames(gas_id);

    /* Find the max num of octets available */
    max_num_of_octets = (uint16) (max_octets_per_LAPDm_frame * max_LAPDm_frames);

    return max_num_of_octets;
  }
  else
  {
    return 0;
  }

} /* end rr_dtm_get_max_LAPDm_octets_per_subs() */


/*!
 * \brief Returns the number of octets that can be transferred via GTTP. 
 *  
 * If not currently in a CS connection on a DTM-capable cell, then zero will be returned. 
 * Note: This function is called from outside GERAN* 
 * 
 * \return uint16 - number of octets
 */
uint16 rr_dtm_get_max_LAPDm_octets(void)
{
  return rr_dtm_get_max_LAPDm_octets_per_subs(SYS_MODEM_AS_ID_1);
}


#else

/*
If FEATURE_GSM_DTM is undefined, a compiler warning will result because this
module will contain no code - define a dummy function here
*/
void rr_dtm_dummy_func(void) { }

#endif /* ifdef FEATURE_GSM_DTM */

/* EOF */
