#ifndef RR_GPRS_DEBUG_H
#define RR_GPRS_DEBUG_H

/*============================================================================
  @file rr_gprs_debug.h

  This module provides a set of functions to return string representations of
  global RR messages and events.

                Copyright (c) 2002-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_debug.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_task.h"
#include "rr_events.h"
#include "geran_multi_sim.h"
#ifdef FEATURE_WCDMA
#include "rrcrrif.h"
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
#include "tdsrrc_rr_if.h"
#endif /* FEATURE_GSM_TDS */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define DEBUG_RR_TRACE_MSG

#define LOG_UNEXPECTED_MESSAGE(xx_message, xx_state) \
  do { \
    if (xx_message) \
    { \
      MSG_SPRINTF_2(MSG_SSID_DFLT, \
                    MSG_LEGACY_ERROR, \
                    "Unexpected message %s in state %s", \
                    rr_gprs_message_name(xx_message, xx_message->message_header.message_set, xx_message->message_header.message_id, NULL), \
                    xx_state); \
    } \
  } while (0) \

#define LOG_UNEXPECTED_EVENT(xx_event, xx_state) \
  MSG_SPRINTF_2(MSG_SSID_DFLT, \
                MSG_LEGACY_ERROR, \
                "Unexpected event %s in state %s", \
                xx_event, \
                xx_state)

#define LOG_UNEXPECTED_TIMER_EXPIRY(xx_timer, xx_state) \
  MSG_SPRINTF_2(MSG_SSID_DFLT, \
                MSG_LEGACY_ERROR, \
                "Unexpected timer %d expiry in state %s", \
                xx_timer, \
                xx_state)

#define LOG_INPUT_EVENT(xx_state_machine, xx_event, xx_state) \
  MSG_SPRINTF_3(MSG_SSID_DFLT, \
                MSG_LEGACY_MED, \
                "IEv(%s, %s, %s)", \
                xx_state_machine, \
                xx_event, \
                xx_state)

#ifdef FEATURE_DUAL_SIM
#define RR_LOG_STATE_TRANSITION(xx_state_machine, xx_old_state, xx_new_state, xx_state_name_fn, xx_gas_id) \
  MSG_SPRINTF_3(MSG_SSID_DFLT, \
                MSG_LEGACY_HIGH, \
                "gs%d:RRState: " xx_state_machine "(%s->%s)", \
                (int)(xx_gas_id+1), \
                xx_state_name_fn(xx_old_state), \
                xx_state_name_fn(xx_new_state))

#else
#define RR_LOG_STATE_TRANSITION(xx_state_machine, xx_old_state, xx_new_state, xx_state_name_fn, xx_gas_id) \
   MSG_SPRINTF_2(MSG_SSID_DFLT, \
                 MSG_LEGACY_HIGH, \
                 "RRState: " xx_state_machine "(%s->%s)", \
                 xx_state_name_fn(xx_old_state), \
                 xx_state_name_fn(xx_new_state))
#endif // FEATURE_DUAL_SIM

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

#ifdef DEBUG_RR_TRACE_MSG

typedef enum
{
  RR_CONTROL_SM,               // 0  : rr_control
  RR_CELL_RESEL_SM,            // 1  : rr_cell_reselection
  RR_CELL_SELECTION_SM,        // 2  : rr_selection_control
  RR_CHANNEL_MODE_MODIFY_SM,   // 3  : rr_channel_mode_modify_control
  RR_CIPHER_CONTROL_SM,        // 4  : rr_cipher_control
  RR_CONN_ESTABLISH_SM,        // 5  : rr_connection_establishment_control
  RR_CONN_RELEASE_SM,          // 6  : rr_connection_release
  RR_FREQ_REDEF_SM,            // 7  : rr_freq_redef_control
  RR_G2W_HO_SM,                // 8  : rr_g2w_handover_control
  RR_HANDLE_PROC_SM,           // 9  : rr_handle_procedures
  RR_INTER_CELL_HO_SM,         // 10 : rr_inter_cell_handover_control
  RR_INTRA_CELL_HO_SM,         // 11 : rr_intra_cell_handover_control
  RR_MODE_SM,                  // 12 : rr_mode_control
  RR_PLMN_LIST_SM,             // 13 : rr_plmn_list_control
  RR_REEST_CELL_SM,            // 14 : rr_reest_cell_control
  RR_CONTROL_SAPI3_SM,         // 15 : rr_control_SAPI3_link
  RR_W2G_CC_ORDER_SM,          // 16 : rr_w2g_cc_order_control ** not used **
  RR_W2G_RESEL_SM,             // 17 : rr_w2gr_reselection_control
  GRR_CONTROL_SM,              // 18 : grr_control
  GRR_BSI_ACQ_SM,              // 19 : bsi_acquisition_control
  GRR_GMM_SUSP_SM,             // 20 : gprs_gmm_suspension_control
  GRR_MAC_SUSP_SM,             // 21 : gprs_mac_suspension_control
  GRR_NETW_SUSP_SM,            // 22 : gprs_netw_suspension_control
  GRR_CELL_RESEL_SM,           // 23 : rr_gprs_cell_reselection
  GRR_NC_MMT_SM,               // 24 : rr_gprs_nc_measurement_control
  GRR_CELL_SELECTION_SM,       // 25 : rr_gprs_selection_control
  GRR_SYS_INFO_ACQ_SM,          // 26 : rr_gprs_sys_info_acquisition_control
  RR_W2G_SERVICE_REDIRECTION_SM,// 27 : rr_w2g_service_redirection_control
  RR_DTM_SM,                    // 28 : dtm_control
  RR_DTM_TBF_EST_SM,            // 29 : dtm_tbf_est_control
  RR_DTM_PACKET_ASSIGN_SM,      // 30 : dtm_packet_assign_control
  RR_DTM_ASSIGN_COMMAND_SM,     // 31 : dtm_assign_command_control
  RR_DTM_CELL_CAPABILITY_SM,    // 32 : dtm_cell_capability_monitor
  RR_SAVED_MSG_OLD,             // 33 : log a message that is about to be overwritten
  RR_SAVED_MSG_NEW,             // 34 : log a message that will overwrite an already saved message
  RR_EDTM_SM,                   // 35 : rr_gprs_edtm_control
  RR_PSHO_SM,                   // 36 : rr_psho_control
  RR_RESEL_G2W_SM,              // 37 : rr_resel_g2w_control
  RR_CELL_ACQ_SM,               // 38 : rr_cell_acq_control
  RR_FEMTO_SM,                  // 39 : rr_femto_control
  RR_DECODE_BCCH_LIST_SM,       // 40 : rr_decode_bcch_list_control
  RR_G2X_REDIR_SM,              // 41 : rr_g2x_redir_control
  RR_L1_IDLE_SM,                // 42 : rr_l1_idle_control
  RR_PLC_GSM_SM,                // 43 : rr_plc_gsm_control
  RR_PLC_G2X_SM,                // 44 : rr_plc_g2x_control
  RR_PLC_X2G_SM,                // 45 : rr_plc_x2g_control
  RR_PS_ACCESS_SM,              // 46 : rr_ps_access_control
  RR_PSCAN_SM,                  // 47 : rr_pscan_control
  RR_RPS_SM,                    // 48 : rr_resel_control / RPS sub-state machine
  RR_SELECT_BCCH_SM,            // 49 : rr_select_bcch_control
  RR_X2G_CGI_SM,                // 50 : rr_x2g_cgi_control
  RR_X2G_RESEL_SM,              // 51 : rr_x2g_resel_control
  RR_SGLTE_BAND_COEX_SM,        // 52 : rr_sglte_band_coex_control
  RR_SGLTE_X2G_SM,              // 53 : rr_sglte_x2g_control
  RR_SGLTE_BPLMN_SM,            // 54 : rr_plc_sglte_g2x_control
  RR_SGLTE_RR_CONN_G2X_RESEL_SM, // 55: rr_sglte_rr_conn_g2x_resel_control
#ifdef FEATURE_GSM_BAND_AVOIDANCE
  RR_BAND_AVOID_SM,             // 56 : rr_band_avoidance_control
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
#ifdef FEATURE_IDLE_DRX_SCALING
  RR_IDLE_DRX_SM,               // 57 : rr_idle_drx_control
#endif /* FEATURE_IDLE_DRX_SCALING */
  RR_STATE_MACHINE_COUNT

} rr_grr_state_mc_T;

#endif /* DEBUG_RR_TRACE_MSG */


typedef enum
{
  RR_MESSAGE_TYPE_UNDEFINED,
  RR_MESSAGE_TYPE_NEW,
  RR_MESSAGE_TYPE_SAVED,
  RR_MESSAGE_TYPE_PENDED,
  RR_MESSAGE_TYPE_MAX
} rr_message_type_e;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Initialises the RR debug module.
*/
extern void rr_gprs_debug_init(
  const gas_id_t gas_id
);

#ifdef DEBUG_RR_TRACE_MSG

extern void rr_store_trace_msg_buf(
  const rr_grr_state_mc_T sm_id,
  const rr_event_T        event,
  const byte              state,
  const void *            msg_ptr,
  const gas_id_t          gas_id
);

#endif


/**
  @brief Returns a string representation of the signal specified.

  This function is used to get a string representation of a signal that may
  be sent or received by RR.  Additionally the string returned will, in some
  cases, contain some extra information regarding the contents of the signal.
  This is only possible if the caller provides the msg_ptr and the message_set
  and message_id values.

  @param msg_ptr:     This is an optional parameter.  If it is not required to
                      be included then it should be set to NULL.
  @param message_set: This value represents the signalling interface that
                      the signal belongs to, as defined in ms.h.  For an IMH_T
                      signal this corresponds to the message_header.message_set
                      field.
  @param message_id:  This value represents the unique identifier of the
                      signal.  For an IMH_T signal, this corresponds to the
                      message_header.message_id field.
  @param level:       Optional parameter - if passed in this will be set to
                      the level that the message should be logged at.
  @return:            A string (char *) is returned, containing a string
                      representation of the signal.
*/
extern const char * rr_gprs_message_name
(
  const rr_cmd_bdy_type * msg_ptr,
  const uint8             message_set,
  const uint32            message_id,
  uint32                * level
);

/**
  @brief This function returns a string representation of an RR event.

  @param event: The event that the caller requires a string representation of.
  @return:      This function will return a string (char *) containing a string
                representation of the event provided by the caller.
*/
extern char *rr_event_name(const rr_event_T event);

extern void rr_show_input_message(
  const rr_message_type_e   type,
  const rr_cmd_bdy_type   * message_ptr,
  const int                 message_set,
  const byte                message_id,
  const char              * state_name,
  const uint32              state,
  const gas_id_t            gas_id
);

extern void rr_show_output_message(
  const char     * destination,
  const IMH_T    * message_ptr,
  const uint8      message_set,
  const uint32     message_id,
  const gas_id_t   gas_id
);

extern const char *rr_gprs_l2_channel_name(
  const l2_channel_type_T l2_channel
);

#ifdef FEATURE_LTE

/**
  @brief Returns the name of a UMID.
*/
extern const char * rr_gprs_debug_msgr_umid_name(
  msgr_umid_type umid
);

#endif /* FEATURE_LTE */
/**
  @brief Add an input message to the appropriate trace buffer.
*/
extern void rr_debug_log_imsg(
  uint8          message_set,
  uint32         message_id,
  uint32         pended_count,
  const gas_id_t gas_id

);

/**
  @brief Add an IMH-style output message to the appropriate trace buffer.
*/
extern void rr_debug_log_omsg(
  IMH_T * message_header,
  const gas_id_t gas_id
);

#ifdef FEATURE_WCDMA

/**
  @brief Logs a message sent to WCDMA RRC on the MS_RRC_RR interface.
*/
extern void rr_debug_log_rrc_rr_omsg(
  rrc_rr_cmd_type * rrc_rr_cmd,
  const gas_id_t    gas_id
);

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS

/**
  @brief Logs a message sent to TDS RRC on the MS_TDSRRC_RR interface.
*/
extern void rr_debug_log_tdsrrc_rr_omsg(
  tdsrrc_rr_cmd_type * tdsrrc_rr_cmd,
  const gas_id_t       gas_id
);

#endif /* FEATURE_GSM_TDS */

/**
  @brief Logs a message sent to via Message Router.
*/
extern void rr_debug_log_msgr_rr_omsg(
  msgr_hdr_struct_type * lte_rrc_cmd,
  const gas_id_t         gas_id
);

/**
  @brief Logs a message sent to MAC.
*/
extern void rr_debug_log_rr_mac_omsg(grr_mac_sig_id_t msg_id, const gas_id_t gas_id);

/**
  @brief Logs an occurence of panic reset.
*/
extern void rr_debug_log_panic_reset(
  const gas_id_t gas_id
);

#endif /* #ifndef RR_GPRS_DEBUG_H */

/* EOF */

