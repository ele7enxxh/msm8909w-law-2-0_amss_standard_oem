/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                            rr_tds_send.c


GENERAL DESCRIPTION
   This module contains functions that send messages from rr to rrc.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2002-2014 Qualcomm Technologies, Inc.


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_tds_send.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
06/03/11    RMN     Created the Module.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "stringl/stringl.h"

/*Check featurisation combinations*/
#ifdef FEATURE_GSM_TDS
  #ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
    #ifndef FEATURE_GSM_TDS_G2T_REDIRECTION
      #error FEATURE_GSM_TDS_PSEUDO_FAST_RETURN def without FEATURE_GSM_TDS_G2T_REDIRECTION
    #endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/
  #endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

#else /*FEATURE_GSM_TDS*/

 #ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
   #error FEATURE_GSM_TDS_G2T_REDIRECTION def without FEATURE_GSM_TDS
 #endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

 #ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
   #error FEATURE_GSM_TDS_PSEUDO_FAST_RETURN def without FEATURE_GSM_TDS
 #endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_GSM_TDS

#include <string.h>
#include "rr_seg_load.h"
#include "rr_defs.h"
#include "ms.h"
#include "sys.h"
#include "rr_tds_send.h"
#include "rr_rrc_send.h"
#include "rr_gprs_debug.h"
#include "rr_multi_sim.h"
#include "rr_general.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/**
  @brief Sends a message to TDSCDMA RRC.

  This function puts a message on TDSCDMA RRCs input queue.  Before this is
  done, the message is logged in RR.

  @param tds_cmd Pointer to the message to send to TDSCDMA RRC.  This must
  have been allocated by calling tdsrrc_get_rr_cmd_buf().
*/
static void rr_send_message_to_tds_rrc(
  tdsrrc_rr_cmd_type * tds_cmd,
  const gas_id_t       gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(tds_cmd);

  rr_show_output_message(
    "TDS RRC",
    NULL_PTR,
    MS_TDSRRC_RR,
    tds_cmd->cmd_hdr.cmd_id,
    gas_id
  );

  rr_debug_log_tdsrrc_rr_omsg(tds_cmd, gas_id);

#ifdef FEATURE_TDS_SUB_ID_INTERFACE
  tds_cmd->cmd_hdr.as_id = RR_GAS_ID_TO_AS_ID;
#endif // FEATURE_DUAL_SIM

  tdsrrc_put_rr_cmd(tds_cmd);
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/**
  @brief Sends TDSRRC_INTERRAT_RESELECTION_REQ from RR to TDS-RRC.
  @param cpid                   The Cell Parameter ID of the target TDS cell.
  @param network_selection_mode The network selection mode that the UE is currently operating in.
  @param rat_pri_list_info      The RAT priority list.
  @param serving_cell_plmn_id   The ID of the PLMN that the current serving cell belongs to.
  @param trk_lo_adj             The last trk_lo_adj value received from GSM L1             .
  @param uarfcn                 The UARFCN of the target TDS cell.                                                                      .
*/
void rr_send_tds_interrat_reselection_req(
  uint16                              cpid,
  sys_network_selection_mode_e_type   network_selection_mode,
  sys_rat_pri_list_info_s_type       *rat_pri_list_info,
  PLMN_id_T                           serving_cell_plmn_id,
  uint16                              trk_lo_adj,
  uint16                              uarfcn,
  const gas_id_t                      gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRRC_INTERRAT_RESELECTION_REQ);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  MSG_GERAN_HIGH_2_G("Sent TDSRRC_INTERRAT_RESELECTION_REQ UARFCN:%d, CPID:%d",uarfcn,cpid);

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDS command");
  }

  tds_cmd->cmd_hdr.cmd_id = TDSRRC_INTERRAT_RESELECTION_REQ;

  tds_cmd->cmd.interrat_reselection_req.cpid = (uint8)cpid;
  tds_cmd->cmd.interrat_reselection_req.network_select_mode = network_selection_mode;
  tds_cmd->cmd.interrat_reselection_req.rat_pri_list_info = *rat_pri_list_info;
  tds_cmd->cmd.interrat_reselection_req.requested_PLMN = serving_cell_plmn_id;
  tds_cmd->cmd.interrat_reselection_req.trk_lo_adj = trk_lo_adj;
  tds_cmd->cmd.interrat_reselection_req.UARFCN = uarfcn;

#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
  rr_dedicated_priority_info_get(&tds_cmd->cmd.interrat_reselection_req.dedicated_priority_info,gas_id);
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/

  rr_send_message_to_tds_rrc(tds_cmd, gas_id);
}

/**
  @brief Sends TDSRRC_INTERRAT_RESELECTION_ABORT_REQ from RR to
         TDS RRC.
*/
void rr_send_tds_interrat_reselection_abort_req(
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRRC_INTERRAT_RESELECTION_ABORT_REQ);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }

  MSG_GERAN_HIGH_0_G("Sent TDSRRC_INTERRAT_RESELECTION_ABORT_REQ");

  tds_cmd->cmd_hdr.cmd_id = TDSRRC_INTERRAT_RESELECTION_ABORT_REQ;
  rr_send_message_to_tds_rrc(tds_cmd, gas_id);
}

/**
  @brief Sends RR_INTERRAT_RESELECTION_ABORT_CNF from RR to TDS
         RRC.
*/
void rr_send_tds_interrat_reselection_abort_cnf(
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRR_INTERRAT_RESELECTION_ABORT_CNF);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }

  MSG_GERAN_HIGH_0_G("Sent TDSRR_INTERRAT_RESELECTION_ABORT_CNF");

  tds_cmd->cmd_hdr.cmd_id = TDSRR_INTERRAT_RESELECTION_ABORT_CNF;
  rr_send_message_to_tds_rrc(tds_cmd, gas_id);
}


/**
  @brief Sends TDSRRC_STOP_TDS_REQ from RR to TDS RRC.
*/
void rr_send_rrc_stop_tds_mode_req(
  tdsrrc_rr_stop_tds_cause_e_type cause,
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRRC_STOP_TDS_REQ);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Sent TDSRRC_STOP_TDS_REQ");

    tds_cmd->cmd_hdr.cmd_id = TDSRRC_STOP_TDS_REQ;
    tds_cmd->cmd.tdsrrc_rr_stop_tds_req.stop_req_cause = cause;
    rr_send_message_to_tds_rrc(tds_cmd, gas_id);
  }
}

/**
 * Send the TDSRR_INTERRAT_RESELECTION_REJ primitive to RRC
 *
 * @Failure_cause: Cause of InterRAT Reselection Failure.

 * @Time_to_reselect:  Time ( in Sec ) when the same cell should be considered

 */
void rr_send_tds_interrat_reselection_rej(
  tdsrr_interrat_reselect_fail_reason_type failure_cause,
  word time_to_reselect,
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRR_INTERRAT_RESELECTION_REJ);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }

  MSG_GERAN_HIGH_0_G("Sent TDSRR_INTERRAT_RESELECTION_REJ");

  tds_cmd->cmd_hdr.cmd_id = TDSRR_INTERRAT_RESELECTION_REJ;
  tds_cmd->cmd.interrat_reselection_rej.failure_reason = failure_cause;
  tds_cmd->cmd.interrat_reselection_rej.time_to_reselect = time_to_reselect;
  // For now Set the Expected_RXLEV_average to FF, This needs to be changed.
  tds_cmd->cmd.interrat_reselection_rej.Expected_RXLEV_average = 0xFF;
  rr_send_message_to_tds_rrc(tds_cmd, gas_id);
}

/**
 * Send the TDSRR_INTERRAT_CC_ORDER_CNF primitive to RRC
 *
 * @Status: Status of CCO
 *
 * @Failure_cause: Cause of InterRAT CC Order Failure.
 *
 */
void rr_send_tds_interrat_cc_order_cnf(
  tdsrr_interrat_cc_order_status_e_type status,
  tdsrr_interrat_cc_order_fail_reason_type failure_cause,
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRR_INTERRAT_CC_ORDER_CNF);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }

  MSG_GERAN_HIGH_2_G("Sent TDSRR_INTERRAT_CC_ORDER_CNF status:%d, failure_cause:%d",status,failure_cause);

  tds_cmd->cmd_hdr.cmd_id = TDSRR_INTERRAT_CC_ORDER_CNF;
  tds_cmd->cmd.interrat_cc_order_cnf.status = status;
  tds_cmd->cmd.interrat_cc_order_cnf.failure_reason = failure_cause;
  rr_send_message_to_tds_rrc(tds_cmd, gas_id);
}


/**
 * Send the TDSRR_INTERRAT_HANDOVER_CNF primitive to TDS-RRC
 *
 * @param handover_status: indicates handover_status.

 * @param failure_cause: indicates failure cause, valid if handover_status
   is set to FALSE.

 * @param gsm_command_list: GSM commands: this field is only filled if
   handover is failure
 */
void rr_send_tds_interrat_ho_cnf(
  tdsrrc_rr_status_e_type handover_status,
  tdsrrc_rr_failure_cause_e_type failure_cause,
  tdsrrc_rr_gsm_command_list_type *gsm_command_list,
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRR_INTERRAT_HANDOVER_CNF);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Sent TDSRR_INTERRAT_HANDOVER_CNF");

    tds_cmd->cmd_hdr.cmd_id = TDSRR_INTERRAT_HANDOVER_CNF;
    tds_cmd->cmd.interrat_handover_cnf.handover_status = handover_status;
    tds_cmd->cmd.interrat_handover_cnf.failure_cause = failure_cause;
    if (gsm_command_list != NULL)
    {
	  
      memscpy(
         &(tds_cmd->cmd.interrat_handover_cnf.gsm_command_list),
         sizeof(tds_cmd->cmd.interrat_handover_cnf.gsm_command_list),
         gsm_command_list,
         sizeof(tdsrrc_rr_gsm_command_list_type));
    }
    else
    {
      memset(
        &(tds_cmd->cmd.interrat_handover_cnf.gsm_command_list),
        0,
        sizeof(tdsrrc_rr_gsm_command_list_type)
      );
    }
    rr_send_message_to_tds_rrc(tds_cmd, gas_id);
  }
}

/**
 * Send the TDSRR_INTERRAT_HANDOVER_ABORT_CNF primitive to
 * TDSRRC
 *
 * @param abort_cause Handover Abort cause sent to RRC
 */
void rr_send_tds_interrat_ho_abort_cnf(
  tdsrrc_rr_status_e_type abort_status,
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRR_INTERRAT_HANDOVER_ABORT_CNF);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Sent TDSRR_INTERRAT_HANDOVER_ABORT_CNF");

    tds_cmd->cmd_hdr.cmd_id = TDSRR_INTERRAT_HANDOVER_ABORT_CNF;
    tds_cmd->cmd.interrat_handover_abort_cnf.abort_status = abort_status;
    rr_send_message_to_tds_rrc(tds_cmd, gas_id);
  }
}

/**
 * Send the TDSRR_INTERRAT_HANDOVER_ABORT_CNF primitive to
 * TDSRRC
 *
 * @param None
 */
void rr_send_tds_interrat_plmn_search_abort_cnf(
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRR_INTERRAT_PLMN_SRCH_ABORT_CNF);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }

  MSG_GERAN_HIGH_0_G("Sent TDSRR_INTERRAT_PLMN_SRCH_ABORT_CNF");

  tds_cmd->cmd_hdr.cmd_id = TDSRR_INTERRAT_PLMN_SRCH_ABORT_CNF;
  rr_send_message_to_tds_rrc(tds_cmd, gas_id);
}

/**
 * Send the TDSRR_INTERRAT_HANDOVER_ABORT_CNF primitive to
 * TDSRRC
 *
 * @param boolean srch_done
 * @param boolean hplmn_found
 * @param sys_detailed_plmn_list_s_type *plmn_list
 */
void rr_send_tds_interrat_plmn_search_cnf(
  boolean srch_done, boolean hplmn_found,
  sys_detailed_plmn_list_s_type *plmn_list,
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRR_INTERRAT_PLMN_SRCH_CNF);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }

  MSG_GERAN_HIGH_2_G("Sent TDSRR_INTERRAT_PLMN_SRCH_CNF, done=%d, hplmn_found=%d",\
    srch_done, hplmn_found);

  tds_cmd->cmd_hdr.cmd_id = TDSRR_INTERRAT_PLMN_SRCH_CNF;
  tds_cmd->cmd.interrat_plmn_srch_cnf.srch_done = srch_done;
  tds_cmd->cmd.interrat_plmn_srch_cnf.hplmn_found = hplmn_found;
  if (plmn_list != NULL)
  {
    tds_cmd->cmd.interrat_plmn_srch_cnf.available_plmn_list = *plmn_list;
  }
  else
  {
    memset(&tds_cmd->cmd.interrat_plmn_srch_cnf.available_plmn_list,\
           0, sizeof(sys_detailed_plmn_list_s_type));
  }

  rr_invalidate_csg_info(&tds_cmd->cmd.interrat_plmn_srch_cnf.available_plmn_list);

  rr_send_message_to_tds_rrc(tds_cmd, gas_id);
}

/**
 * Send the TDSRRC_INTERRAT_PLMN_SRCH_ABORT_REQ primitive to
 * TDSRRC
 *
 * @param None
 */
void rr_send_tds_interrat_plmn_search_abort_req(
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRRC_INTERRAT_PLMN_SRCH_ABORT_REQ);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Sent TDSRRC_INTERRAT_PLMN_SRCH_ABORT_REQ");

    tds_cmd->cmd_hdr.cmd_id = TDSRRC_INTERRAT_PLMN_SRCH_ABORT_REQ;
    rr_send_message_to_tds_rrc(tds_cmd, gas_id);
  }
}

/**
 * Send the TDSRRC_INTERRAT_PLMN_SRCH_REQ primitive to TDSRRC
 *
 * @param None
 */
void rr_send_tds_interrat_plmn_search_req(
  sys_plmn_rat_list_s_type *plmn_list,
  sys_network_selection_mode_e_type network_select_mode,
  uint32 timer_val,
  boolean camped_suitably,
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRRC_INTERRAT_PLMN_SRCH_REQ);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }
  else
  {
    uint32 i;
    uint32 num_plmns = 0;

    for (i = 0; i < plmn_list->length && i < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH; i++)
    {
      if (plmn_list->info[i].rat == SYS_RAT_TDS_RADIO_ACCESS)
      {
        tds_cmd->cmd.interrat_plmn_srch_req.plmn[num_plmns] = plmn_list->info[i].plmn;
        num_plmns++;
      }
    }

    tds_cmd->cmd.interrat_plmn_srch_req.num_plmns = num_plmns;
    tds_cmd->cmd.interrat_plmn_srch_req.network_select_mode = network_select_mode;

#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
    {
      rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);

      tds_cmd->cmd.interrat_plmn_srch_req.service_search = \
        rr_plmn_list_req_info_ptr->service_search;
    }
#endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

    tds_cmd->cmd.interrat_plmn_srch_req.timer_val = timer_val;

    MSG_GERAN_HIGH_3_G("Sent TDSRRC_INTERRAT_PLMN_SRCH_REQ num_plmns:%d, nw_sel_mode:%d, timer:%d",num_plmns,network_select_mode,timer_val);
    tds_cmd->cmd_hdr.cmd_id = TDSRRC_INTERRAT_PLMN_SRCH_REQ;

    tds_cmd->cmd.interrat_plmn_srch_req.camped_suitably = camped_suitably;

    rr_send_message_to_tds_rrc(tds_cmd, gas_id);
  }
}

/**
 * Send the TDSRR_INTERRAT_REDIRECT_ABORT_CNF primitive to TDSRRC
 *
 * @param None
 */
void rr_send_tds_interrat_redirect_abort_cnf(
  tdsrrc_rr_status_e_type abort_status,
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRR_INTERRAT_REDIRECT_ABORT_CNF);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }
  else
  {
    MSG_GERAN_MED_3_G("Sent TDSRR_INTERRAT_REDIRECT_ABORT_CNF, abort_status:%d",
      abort_status,
      0,
      0);

    tds_cmd->cmd_hdr.cmd_id = TDSRR_INTERRAT_REDIRECT_ABORT_CNF;
    tds_cmd->cmd.interrat_redirect_abort_cnf.abort_status = abort_status;
    rr_send_message_to_tds_rrc(tds_cmd, gas_id);
  }
}

/**
 * Send the TDSRR_INTERRAT_REDIRECT_REJ primitive to TDSRRC
 *
 * @param None
 */
void rr_send_tds_interrat_redirect_rej(
  tdsrr_interrat_redirect_fail_reason_type failure_reason,
  rr_available_plmn_db_T *available_plmn_db,
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRR_INTERRAT_REDIRECT_REJ);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }
  else
  {
    tds_cmd->cmd_hdr.cmd_id = TDSRR_INTERRAT_REDIRECT_REJ;
    tds_cmd->cmd.interrat_redirect_rej.failure_reason = failure_reason;

    if ((available_plmn_db != NULL)&&(available_plmn_db->valid))
    {
      rr_get_plmn_list_for_redirection_rej(
        available_plmn_db,
        &tds_cmd->cmd.interrat_redirect_rej.num_avail_plmns,
        &tds_cmd->cmd.interrat_redirect_rej.num_available_fplmns,
        tds_cmd->cmd.interrat_redirect_rej.plmn,
        tds_cmd->cmd.interrat_redirect_rej.signal_quality,
        tds_cmd->cmd.interrat_redirect_rej.signal_strength,
        gas_id
      );
    }
    else
    {
      MSG_GERAN_MED_0_G("available_plmn_db is not valid!");
      tds_cmd->cmd.interrat_redirect_rej.num_avail_plmns = 0;
    }

    MSG_GERAN_MED_3_G("Sent TDSRR_INTERRAT_REDIRECT_REJ failure_reason:%d, num_plmns:%d",
    failure_reason,
    tds_cmd->cmd.interrat_redirect_rej.num_avail_plmns,
      0);

    rr_send_message_to_tds_rrc(tds_cmd, gas_id);
  }
}

/**
 * Send the TDSRR_STOP_GSM_CNF primitive to TDSRRC
 *
 * @param None
 */
void rr_send_tds_stop_gsm_mode_cnf(
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRR_STOP_GSM_CNF);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Failed to send TDSRR_STOP_GSM_CNF, out of memory");
  }
  else
  {
    tds_cmd->cmd_hdr.cmd_id = TDSRR_STOP_GSM_CNF;
    tds_cmd->cmd.rr_rrc_stop_gsm_cnf.stop_cnf_cause = TDSRRC_GO_TO_INACTIVE;
    MSG_GERAN_MED_1_G("Sent TDSRR_STOP_GSM_CNF cause %d",tds_cmd->cmd.rr_rrc_stop_gsm_cnf.stop_cnf_cause);
    rr_send_message_to_tds_rrc(tds_cmd, gas_id);
  }
}

/**
 * Send the TDSRR_INTERRAT_CC_ORDER_ABORT_CNF primitive to TDS-RRC
 *
 * @status: Abort Cnf cause
 */
void rr_send_tds_interrat_cc_order_abort_cnf(
  tdsrrc_rr_status_e_type status,
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRR_INTERRAT_CC_ORDER_ABORT_CNF);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDS-RRC command");
  }

  tds_cmd->cmd_hdr.cmd_id = TDSRR_INTERRAT_CC_ORDER_ABORT_CNF;
  tds_cmd->cmd.interrat_cc_order_abort_cnf.abort_status = status;
  MSG_GERAN_MED_1_G("Sent TDSRR_INTERRAT_CC_ORDER_ABORT_CNF cause %d",tds_cmd->cmd.interrat_cc_order_abort_cnf.abort_status);
  rr_send_message_to_tds_rrc(tds_cmd, gas_id);
}

/**
  @brief Converts TDS RRC Resel Rej to internal format (same as
         WCDMA)
  @param tds_resel_rej          TDS Resel Rej message
  @param rrc_resel_rej          Converted RRC Resel Rej message
*/
void rr_convert_tds_to_rrc_resel_rej(
  tdsrrc_interrat_reselect_rej_type *tds_resel_rej,
  rrc_interrat_reselect_rej_type *rrc_resel_rej
)
{
  memscpy(rrc_resel_rej->forbidden_lac,
  	      sizeof(rrc_resel_rej->forbidden_lac), 
  	      tds_resel_rej->forbidden_lac, 
  	      sizeof tds_resel_rej->forbidden_lac
  	      );

  rrc_resel_rej->PLMN = tds_resel_rej->PLMN;
  rrc_resel_rej->squalmin = tds_resel_rej->squalmin;
  rrc_resel_rej->srxlevmin = tds_resel_rej->srxlevmin;
  rrc_resel_rej->time_to_reselect = tds_resel_rej->time_to_reselect;

  switch (tds_resel_rej->reject_reason)
  {
    case TDSRRC_UMTS_NONE:
      rrc_resel_rej->reject_reason = RRC_WCDMA_NONE;
    break;
    case TDSRRC_UMTS_INAVALID_STATE:
      rrc_resel_rej->reject_reason = RRC_WCDMA_INAVALID_STATE;
    break;
    case TDSRRC_UMTS_CELL_BARRED:
      rrc_resel_rej->reject_reason = RRC_WCDMA_CELL_BARRED;
    break;
    case TDSRRC_UMTS_UARFCN_BARRED:
      rrc_resel_rej->reject_reason = RRC_WCDMA_UARFCN_BARRED;
    break;
    case TDSRRC_UMTS_PLMN_MISMATCH:
      rrc_resel_rej->reject_reason = RRC_WCDMA_PLMN_MISMATCH;
    break;
    case TDSRRC_UMTS_LOW_S_VALUE:
      rrc_resel_rej->reject_reason = RRC_WCDMA_LOW_S_VALUE;
    break;
    case TDSRRC_UMTS_ACQUISITION_FAIL:
      rrc_resel_rej->reject_reason = RRC_WCDMA_ACQUISITION_FAIL;
    break;
    case TDSRRC_UMTS_SIBS_FAILED_LOWER_LAYERS:
      rrc_resel_rej->reject_reason = RRC_WCDMA_SIBS_FAILED_LOWER_LAYERS;
    break;
    case TDSRRC_UMTS_INVALID_SIBS:
      rrc_resel_rej->reject_reason = RRC_WCDMA_INVALID_SIBS;
    break;
    case TDSRRC_UMTS_SIBS_TIMEOUT:
      rrc_resel_rej->reject_reason = RRC_WCDMA_SIBS_TIMEOUT;
    break;
    case TDSRRC_UMTS_SIBS_FAILED_OTHER:
      rrc_resel_rej->reject_reason = RRC_WCDMA_SIBS_FAILED_OTHER;
    break;
    case TDSRRC_UMTS_FORBIDDEN_LA:
      rrc_resel_rej->reject_reason = RRC_WCDMA_FORBIDDEN_LA;
    break;
    case TDSRRC_UMTS_CELL_QUALITY_FAILURE:
      rrc_resel_rej->reject_reason = RRC_WCDMA_CELL_QUALITY_FAILURE;
    break;
    case TDSRRC_UMTS_CELL_NOT_SUITABLE:
      rrc_resel_rej->reject_reason = RRC_WCDMA_CELL_NOT_SUITABLE;
    break;
    case TDSRRC_UMTS_CELL_NOT_EVALUATED:
      rrc_resel_rej->reject_reason = RRC_WCDMA_CELL_NOT_EVALUATED;
    break;
    case TDSRRC_UMTS_CELL_CHANGE_FAILURE:
      rrc_resel_rej->reject_reason = RRC_WCDMA_CELL_CHANGE_FAILURE;
    break;
    case TDSRRC_UMTS_T3174_EXPIRED:
      rrc_resel_rej->reject_reason = RRC_WCDMA_T3174_EXPIRED;
    break;
    case TDSRRC_UMTS_MAX_RACH_ATTEMPTS:
      rrc_resel_rej->reject_reason = RRC_WCDMA_MAX_RACH_ATTEMPTS;
    break;
    case TDSRRC_UMTS_CONNECTION_REJECT:
      rrc_resel_rej->reject_reason = RRC_WCDMA_CONNECTION_REJECT;
    break;

    default:
      MSG_GERAN_ERROR_1("RR Invalid TDS RRC rej reason:%d",tds_resel_rej->reject_reason);
      rrc_resel_rej->reject_reason = RRC_WCDMA_NONE;
    break;
  }
}

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
/**
 * Send the TDSRRC_INTERRAT_REDIRECT_REQ primitive to TDSRRC
 *
 * @param None
 */
void rr_send_tds_interrat_redirection_req(
  tdsrrc_target_info_type      * tdd_target,
  sys_rat_pri_list_info_s_type * rat_pri_list_info,
  boolean                        pseudo_fr_active,
  const gas_id_t                 gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  RR_NULL_CHECK_RETURN_VOID(tdd_target);
  RR_NULL_CHECK_RETURN_VOID(rat_pri_list_info);

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRRC_INTERRAT_REDIRECT_REQ);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }

  MSG_GERAN_HIGH_1_G("Sent TDSRRC_INTERRAT_REDIRECT_REQ nw_sel_mode:%d", rr_PLMN_request_details_ptr->network_select_mode);

  tds_cmd->cmd_hdr.cmd_id = TDSRRC_INTERRAT_REDIRECT_REQ;
  tds_cmd->cmd.interrat_redirect_req.PLMN = rr_PLMN_request_details_ptr->requested_PLMN_id;
  tds_cmd->cmd.interrat_redirect_req.network_select_mode = rr_PLMN_request_details_ptr->network_select_mode;
  tds_cmd->cmd.interrat_redirect_req.tds_target_info = *tdd_target;
  tds_cmd->cmd.interrat_redirect_req.rat_pri_list_info = *rat_pri_list_info;
  tds_cmd->cmd.interrat_redirect_req.pseudo_fr_active = pseudo_fr_active;

#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
  rr_dedicated_priority_info_get(&tds_cmd->cmd.interrat_redirect_req.dedicated_priority_info,gas_id);
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/

  rr_send_message_to_tds_rrc(tds_cmd, gas_id);
}

/**
 * Send the TDSRRC_INTERRAT_REDIRECT_ABORT_REQ primitive to TDSRRC
 *
 * @param None
 */
void rr_send_tds_interrat_redirection_abort_req(
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDS-RRC message %d, TDS-RRC not loaded",TDSRRC_INTERRAT_REDIRECT_ABORT_REQ);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send TDSRRC command");
  }
  MSG_GERAN_HIGH_0_G("Sent TDSRRC_INTERRAT_REDIRECT_ABORT_REQ");

  tds_cmd->cmd_hdr.cmd_id = TDSRRC_INTERRAT_REDIRECT_ABORT_REQ;
  rr_send_message_to_tds_rrc(tds_cmd, gas_id);
}

#endif /* FEATURE_GSM_TDS_G2T_REDIRECTION */

#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
/**
 * Sends the TDSRRC_CLEAR_DEDICATED_PRIORITIES_IND signal to TDSRRC
 *
 * @param None
 */
void rr_send_tds_clear_dedicated_priorities_ind(
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDSRRC message %d, TDSRRC not loaded",TDSRRC_CLEAR_DEDICATED_PRIORITIES_IND);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd)
  {
    tds_cmd->cmd_hdr.cmd_id = TDSRRC_CLEAR_DEDICATED_PRIORITIES_IND;

    MSG_GERAN_HIGH_0_G("Sent TDSRRC_CLEAR_DEDICATED_PRIORITIES_IND");
    rr_send_message_to_tds_rrc(tds_cmd,gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("tds_cmd == NULL");
  }
}

/**
 * Sends the TDSRRC_DEDICATED_PRIORITIES_REQ signal to TDSRRC
 *
 * @param None
 */
void rr_send_tds_dedicated_priorities_req(
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDSRRC message %d, TDSRRC not loaded",TDSRRC_DEDICATED_PRIORITIES_REQ);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd)
  {
    tds_cmd->cmd_hdr.cmd_id = TDSRRC_DEDICATED_PRIORITIES_REQ;

    MSG_GERAN_HIGH_0_G("Sent TDSRRC_DEDICATED_PRIORITIES_REQ");
    rr_send_message_to_tds_rrc(tds_cmd,gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("tds_cmd == NULL");
  }
}

/**
  @brief Sends TDSRR_DEDICATED_PRIORITIES_RSP to TDSCDMA RRC.
*/
void rr_send_tds_dedicated_priorities_rsp(
  const gas_id_t gas_id
)
{
  tdsrrc_rr_cmd_type *tds_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send TDSRRC message %d, TDSRRC not loaded",TDSRR_DEDICATED_PRIORITIES_RSP);
    return;
  }

  tds_cmd = tdsrrc_get_rr_cmd_buf();

  if (tds_cmd)
  {
    tds_cmd->cmd_hdr.cmd_id = TDSRR_DEDICATED_PRIORITIES_RSP;

    if (rr_individual_priorities_valid(gas_id))
    {
      tds_cmd->cmd.dedicated_priorities_rsp.dedicated_pri_valid = TRUE;
      tds_cmd->cmd.dedicated_priorities_rsp.plmn = rr_dedicated_priority_info_get_plmn(gas_id);
      rr_dedicated_priority_info_get(
        &tds_cmd->cmd.dedicated_priorities_rsp.dedicated_pri_info,
        gas_id
      );
    }
    else
    {
      tds_cmd->cmd.dedicated_priorities_rsp.dedicated_pri_valid = FALSE;
    }
    MSG_GERAN_HIGH_0_G("Sent TDSRR_DEDICATED_PRIORITIES_RSP");
    rr_send_message_to_tds_rrc(tds_cmd,gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("tds_cmd == NULL");
  }

}
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/

#endif /* FEATURE_GSM_TDS */

/* EOF */

