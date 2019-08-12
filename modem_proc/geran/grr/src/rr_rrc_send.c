/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                            rr_rrc_send.c


GENERAL DESCRIPTION
   This module contains functions that send messages from rr to rrc.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2002-2014 Qualcomm Technologies, Inc.


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_rrc_send.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
02/14/03    RG      Added support for G2W blind Handover
10/03/02    Rao     Added InterRAT ( GSM to WCDMA ) Reselection Support.
08/30/02    Rao     Added InterRAT (WCDMA to GSM ) Reselection Support.
08/13/02    rmc     Added include of "customer.h" so the feature can be defined
08/13/02    atan    Moved #ifdef for inter Rat to before includes section.
07/22/02    Rao     Created the Module.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)

#include "comdef.h"
#include <string.h>
#include "rr_seg_load.h"
#include "rr_rrc_send.h"
#include "rrc_rr_types.h"
#include "gs.h"
#include "geran_msgs.h"
#include "rr_gprs_debug.h"
#include "rr_general.h"
#include "cfa.h"
#include "rr_defs.h"
#include "ms.h"
#include "rr_shared_cell_selection.h"
#include "rr_w2g_service_redirection.h"
#include "rr_candidate_cell_db.h"
#include "sys.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"


#ifdef FEATURE_WCDMA
#if defined(TEST_FRAMEWORK)
  #error code not present
#endif // TEST_FRAMEWORK

#define RRC_DS_INSERT_AS_ID(msg) {msg.as_id = geran_map_gas_id_to_nas_id(gas_id);}

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
  @brief Sends a message from RR to RRC.

  @param rrc_cmd_ptr The message to send from RR to RRC.
*/
static void rr_send_message_to_rrc(
  rrc_rr_cmd_type *rrc_cmd_ptr,
  const gas_id_t gas_id
)
{
  int32 rrc_id;

  if (rrc_cmd_ptr == NULL)
  {
    return;
  }

  rrc_id = (int32)(rrc_cmd_ptr->cmd_hdr.cmd_id);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* sanity check the active subscription's support for W+G */
  if (!geran_is_multi_mode_gas_id(gas_id))
  {
    /* message will still be sent below, but debug should indicate it */
    MSG_GERAN_ERROR_1_G("Sending signal(%d) to RRC when dual RAT disabled", rrc_id);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_show_output_message("RRC", NULL_PTR, MS_RRC_RR, rrc_id, gas_id);
  rr_debug_log_rrc_rr_omsg(rrc_cmd_ptr, gas_id);

  rrc_put_rr_cmd(rrc_cmd_ptr);
}

#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
/* function build GSM arfcn list based on rr_candidate_cell_db
 * passing GSM arfcns on which PLMNs are found during PLMN search
 * on GSM can removing W channels around these arfcns so that
 * PLMN search on WCDMA side could be faster.
 */
static void rr_build_gsm_arfcn_list(
  rr_rrc_gsm_cell_list_type *gsm_arfcn_list,
  const gas_id_t gas_id
)
{
  rr_candidate_cell_db_T *candidate_db_ptr = rr_get_candidate_cell_db_ptr(gas_id);
  uint32                  i;

  if ((gsm_arfcn_list == NULL) || (candidate_db_ptr == NULL))
  {
    MSG_GERAN_ERROR_0_G("Unable to build GSM ARFCN list");
    return;
  }

  gsm_arfcn_list->size = 0;
  for (i = 0; i < candidate_db_ptr->num_frequencies && gsm_arfcn_list->size < MAX_RR_RRC_GSM_CELL_LIST_SIZE; i++)
  {
    rr_candidate_cell_T *entry = &(candidate_db_ptr->candidate_cells[i]);

    if ((entry->type == RR_CANDIDATE_VALID_BCCH) ||
        (entry->type == RR_CANDIDATE_BCCH_WITH_DECODE_FAILURES))
    {
      MSG_GERAN_LOW_3_G("Adding cell (%d, %d) to GSM cell list, index %d",
        entry->meas.arfcn.num,
        entry->meas.arfcn.band,
        gsm_arfcn_list->size);

      gsm_arfcn_list->cells[gsm_arfcn_list->size] = entry->meas.arfcn;
      gsm_arfcn_list->size++;
    }
  }
}
#endif /* FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION */

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/**
  @brief Sends RRC_INTERRAT_RESELECTION_REQ from RR to RRC.
  @param uarfcn                 The UARFCN of the target WCDMA cell.
  @param scrambling_code        The Scrambling Code of the target WCDMA cell.
  @param network_selection_mode The network selection mode that the UE is currently operating in.
  @param serving_cell_plmn_id   The ID of the PLMN that the current serving cell belongs to.
  @param trk_lo_adj             The last trk_lo_adj value received from GSM L1.
*/
void rr_send_interrat_reselection_req(
  uint16                              uarfcn,
  uint16                              scrambling_code,
  uint16                              pn_offset,
  sys_network_selection_mode_e_type   network_selection_mode,
  PLMN_id_T                           serving_cell_plmn_id,
  uint16                              trk_lo_adj,
  sys_rat_pri_list_info_s_type      * rat_pri_list_info,
  const gas_id_t                      gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_INTERRAT_RESELECTION_REQ);
    return;
  }

  RR_NULL_CHECK_RETURN_VOID(rat_pri_list_info);

  rrc_cmd = rrc_get_rr_cmd_buf();
  if (rrc_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send RRC command");
  }

  rrc_cmd->cmd_hdr.cmd_id = RRC_INTERRAT_RESELECTION_REQ;

  rrc_cmd->cmd.interrat_reselection_req.UARFCN              = uarfcn;
  rrc_cmd->cmd.interrat_reselection_req.scrambling_code     = scrambling_code;
  rrc_cmd->cmd.interrat_reselection_req.network_select_mode = network_selection_mode;
  rrc_cmd->cmd.interrat_reselection_req.requested_PLMN      = serving_cell_plmn_id;
  rrc_cmd->cmd.interrat_reselection_req.trk_lo_adj          = trk_lo_adj;
  rrc_cmd->cmd.interrat_reselection_req.rat_pri_list_info   = *rat_pri_list_info;
  rrc_cmd->cmd.interrat_reselection_req.PN_Offset           = pn_offset;

#ifdef FEATURE_LTE
  rr_dedicated_priority_info_get(
    &rrc_cmd->cmd.interrat_reselection_req.dedicated_priority_info,
    gas_id
  );
#endif /* FEATURE_LTE */

  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_reselection_req);
  rr_send_message_to_rrc(rrc_cmd, gas_id);
}



/**
  @brief Sends RRC_INTERRAT_RESELECTION_ABORT_REQ from RR to RRC.
*/
void rr_send_interrat_reselection_abort_req(
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_INTERRAT_RESELECTION_ABORT_REQ);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("Out of memory");
  }
  rrc_cmd->cmd_hdr.cmd_id = RRC_INTERRAT_RESELECTION_ABORT_REQ;
  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_reselection_abort_req);

  rr_send_message_to_rrc(rrc_cmd, gas_id);
}

void rr_send_rrc_interrat_cc_order_abort_req(const gas_id_t gas_id)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;
    
  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_INTERRAT_CC_ORDER_ABORT_REQ);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();
  
  if (rrc_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("Out of memory");
  }
  rrc_cmd->cmd_hdr.cmd_id = RRC_INTERRAT_CC_ORDER_ABORT_REQ;
  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_cc_order_abort_req);

  rr_send_message_to_rrc(rrc_cmd, gas_id);

}


#ifdef FEATURE_INTERRAT_PCCO_GTOW

/**
  @brief Sends RRC_INTERRAT_CC_ORDER_REQ from RR to RRC.
  @param uarfcn                 The UARFCN of the target WCDMA cell.
  @param scrambling_code        The Scrambling Code of the target WCDMA cell.
  @param network_selection_mode The network selection mode that the UE is currently operating in.
  @param serving_cell_plmn_id   The ID of the PLMN that the current serving cell belongs to.
  @param t3174_timeout          The value of T3174 that should be passed to WCDMA RRC.
*/
void rr_send_interrat_pcco_req(
  uint16                            uarfcn,
  uint16                            scrambling_code,
  sys_network_selection_mode_e_type network_selection_mode,
  PLMN_id_T                         serving_cell_plmn_id,
  uint32                            t3174_value,
  sys_rat_pri_list_info_s_type      * rat_pri_list_info,
  const gas_id_t                    gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_INTERRAT_CC_ORDER_REQ);
    return;
  }

  RR_NULL_CHECK_RETURN_VOID(rat_pri_list_info);

  rrc_cmd = rrc_get_rr_cmd_buf();
  if (rrc_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send RRC command");
  }

  rrc_cmd->cmd_hdr.cmd_id = RRC_INTERRAT_CC_ORDER_REQ;

  rrc_cmd->cmd.interrat_cc_order_req.UARFCN              = uarfcn;
  rrc_cmd->cmd.interrat_cc_order_req.scrambling_code     = scrambling_code;
  rrc_cmd->cmd.interrat_cc_order_req.network_select_mode = network_selection_mode;
  rrc_cmd->cmd.interrat_cc_order_req.t3174               = t3174_value;
  rrc_cmd->cmd.interrat_cc_order_req.requested_PLMN      = serving_cell_plmn_id;
  rrc_cmd->cmd.interrat_cc_order_req.rat_pri_list_info   = *rat_pri_list_info;

#ifdef FEATURE_LTE
  rr_dedicated_priority_info_get(
    &rrc_cmd->cmd.interrat_cc_order_req.dedicated_priority_info,
    gas_id
  );
#endif /* FEATURE_LTE */

  MSG_GERAN_HIGH_3_G("Sending PCCO cmd to RRC.UARFCN %d,PSC %d N/w Select mode %d",
             rrc_cmd->cmd.interrat_cc_order_req.UARFCN,
             rrc_cmd->cmd.interrat_cc_order_req.scrambling_code,
             rrc_cmd->cmd.interrat_cc_order_req.network_select_mode);

  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_cc_order_req);
  rr_send_message_to_rrc(rrc_cmd, gas_id);
}

#endif /* FEATURE_INTERRAT_PCCO_GTOW */




/**
 * Send the RR_INTERRAT_HANDOVER_ABORT_CNF primitive to RRC
 *
 * @param abort_cause Handover Abort cause sent to RRC
 */
void rr_send_interrat_ho_abort_cnf(
  rrc_rr_status_e_type abort_status,
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_INTERRAT_HANDOVER_ABORT_CNF);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Out of memory");
  }
  else
  {
    rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_HANDOVER_ABORT_CNF;
    rrc_cmd->cmd.interrat_handover_abort_cnf.abort_status = abort_status;
    RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_handover_abort_cnf);
    rr_send_message_to_rrc(rrc_cmd, gas_id);
  }
}


/**
 * Send the RR_INTERRAT_HANDOVER_CNF primitive to RRC
 *
 * @param handover_status: indicates handover_status.

 * @param failure_cause: indicates failure cause, valid if handover_status
   is set to FALSE.

 * @param gsm_command_list: GSM commands: this field is only filled if
   handover is failure
 */
void rr_send_interrat_ho_cnf(
  rrc_rr_status_e_type handover_status,
  rrc_rr_failure_cause_e_type failure_cause,
  rrc_rr_gsm_command_list_type *gsm_command_list,
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_INTERRAT_HANDOVER_CNF);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("There is not enough memory to send RRC command");
  }
  else
  {
    rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_HANDOVER_CNF;
    rrc_cmd->cmd.interrat_handover_cnf.handover_status = handover_status;
    rrc_cmd->cmd.interrat_handover_cnf.failure_cause = failure_cause;
    if (gsm_command_list != NULL)
    {
		//Structure assignment
		rrc_cmd->cmd.interrat_handover_cnf.gsm_command_list = *gsm_command_list;

    }
    else
    {
      memset(
        &(rrc_cmd->cmd.interrat_handover_cnf.gsm_command_list),
        0,
        sizeof(rrc_rr_gsm_command_list_type)
      );
    }

    RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_handover_cnf);
    rr_send_message_to_rrc(rrc_cmd, gas_id);
  }
}

/**
 * Send the RR_INTERRAT_RESELECTION_REJ primitive to RRC
 *
 * @Failure_cause: Cause of InterRAT Reselection Failure.

 * @Time_to_reselect:  Time ( in Sec ) when the same cell should be considered

 */
void rr_send_interrat_reselection_rej(
  rr_interrat_reselect_fail_reason_type failure_cause,
  word time_to_reselect,
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_INTERRAT_RESELECTION_REJ);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send RRC command");
  }
  rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_RESELECTION_REJ;
  rrc_cmd->cmd.interrat_reselection_rej.failure_reason = failure_cause;
  rrc_cmd->cmd.interrat_reselection_rej.time_to_reselect = time_to_reselect;
  // For now Set the Expected_RXLEV_average to FF, This needs to be changed.
  rrc_cmd->cmd.interrat_reselection_rej.Expected_RXLEV_average = 0xFF;

  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_reselection_rej);
  rr_send_message_to_rrc(rrc_cmd, gas_id);
}

void rr_send_interrat_reselection_abort_cnf(
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_INTERRAT_RESELECTION_ABORT_CNF);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("Out of memory");
  }

  rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_RESELECTION_ABORT_CNF;
  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_reselection_abort_cnf);
  rr_send_message_to_rrc(rrc_cmd, gas_id);
}

/**
 * Send the RR_INTERRAT_CC_ORDER_REJ primitive to RRC
 *
 * @Failure_cause: Cause of InterRAT CC Order Failure.

 * @Time_to_reselect:  Time ( in Sec ) when the same cell should be considered

 */
void rr_send_interrat_cc_order_rej(
  rr_interrat_cc_order_fail_reason_type failure_cause,
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_INTERRAT_CC_ORDER_REJ);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send RRC command");
  }
  rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_CC_ORDER_REJ;
  rrc_cmd->cmd.interrat_cc_order_rej.failure_reason = failure_cause;
  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_cc_order_rej);
  rr_send_message_to_rrc(rrc_cmd, gas_id);
}

/**
 * Send the RR_INTERRAT_CC_ORDER_CNF primitive to RRC
 *
 * @Failure_cause: Cause of InterRAT CC Order Failure.

 * @Time_to_reselect:  Time ( in Sec ) when the same cell should be considered

 */
void rr_send_interrat_cc_order_cnf(
  rr_interrat_cc_order_status_e_type status,
  rr_interrat_cc_order_fail_reason_type failure_cause,
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_INTERRAT_CC_ORDER_CNF);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send RRC command");
  }
  rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_CC_ORDER_CNF;
  rrc_cmd->cmd.interrat_cc_order_cnf.status = status;
  rrc_cmd->cmd.interrat_cc_order_cnf.failure_reason = failure_cause;
  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_cc_order_cnf);
  rr_send_message_to_rrc(rrc_cmd, gas_id);
}

/**
 * Send the RR_INTERRAT_CC_ORDER_CNF primitive to RRC
 *
 * @Failure_cause: Cause of InterRAT CC Order Failure.

 * @Time_to_reselect:  Time ( in Sec ) when the same cell should be considered

 */
void rr_send_interrat_cc_order_abort_cnf(
  rrc_rr_status_e_type status,
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_INTERRAT_CC_ORDER_ABORT_CNF);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    ERR_GERAN_FATAL_0_G("There is not enough memory to send RRC command");
  }
  rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_CC_ORDER_ABORT_CNF;
  rrc_cmd->cmd.interrat_cc_order_abort_cnf.abort_status = status;
  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_cc_order_abort_cnf);
  rr_send_message_to_rrc(rrc_cmd, gas_id);
}

void rr_send_interrat_handover_req(
  boolean                                 cipherOn,
  byte*                                   utran_msg,
  rr_rrc_interrat_measurement_data_type * measurements,
  sys_rat_pri_list_info_s_type          * rat_pri_list_info,
  boolean                                 csfb_call_status,
  const gas_id_t                          gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_INTERRAT_HANDOVER_REQ);
    return;
  }

  RR_NULL_CHECK_RETURN_VOID(rat_pri_list_info);

  rrc_cmd = rrc_get_rr_cmd_buf();
  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Out of memory");
    return;
  }

  rrc_cmd->cmd_hdr.cmd_id = RRC_INTERRAT_HANDOVER_REQ;
  rrc_cmd->cmd.interrat_handover_req.cipher_enabled = cipherOn;

  /* copy the measurements data */
  //Structure assignment 
	rrc_cmd->cmd.interrat_handover_req.interrat_measurement_data = *measurements;


  memset(rrc_cmd->cmd.interrat_handover_req.ho_cmd, 0, MAX_OCTETS_L3_MESSAGE);
  //Replace memcpy with memscpy
  memscpy(rrc_cmd->cmd.interrat_handover_req.ho_cmd,sizeof(rrc_cmd->cmd.interrat_handover_req.ho_cmd), utran_msg, MAX_OCTETS_L3_MESSAGE);


  rrc_cmd->cmd.interrat_handover_req.trk_lo_adj = scell_info_ptr->gsm.trk_lo_adj;

  /* to support RRC to determine which band (800 or 850) the HO is targetting to,
   * RR passes its current serving cell's PLMN-id, if feature defined
   */
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
  rrc_cmd->cmd.interrat_handover_req.plmn_id =
    scell_info_ptr->gsm.location_area_identification.plmn_id;
#endif /* FEATURE_MCC_AND_850_800_BAND_COMBINATION */

  rrc_cmd->cmd.interrat_handover_req.rat_pri_list_info = *rat_pri_list_info;

#ifdef FEATURE_LTE
  rr_dedicated_priority_info_get(
    &rrc_cmd->cmd.interrat_handover_req.dedicated_priority_info,
    gas_id
  );
#endif /* FEATURE_LTE */

  rrc_cmd->cmd.interrat_handover_req.csfb_call_status = csfb_call_status;

  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_handover_req);
  rr_send_message_to_rrc(rrc_cmd, gas_id);
}

void rr_send_interrat_handover_abort_req(
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_INTERRAT_HANDOVER_ABORT_REQ);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Out of memory");
  }
  else
  {
    rrc_cmd->cmd_hdr.cmd_id = RRC_INTERRAT_HANDOVER_ABORT_REQ;
    RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_handover_abort_req);
    rr_send_message_to_rrc(rrc_cmd, gas_id);
  }
}

void rr_send_nas_data_ind(
  SAPI_T SAPI,
  byte l3_message_length,
  byte *l3_message,
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_NAS_DATA_IND);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Out of memory");
  }
  else
  {
    rrc_cmd->cmd_hdr.cmd_id = RRC_NAS_DATA_IND;
    rrc_cmd->cmd.interrat_nas_data_ind.SAPI = SAPI;

    if (l3_message_length > MAX_OCTETS_L3_MESSAGE)
    {
      MSG_GERAN_ERROR_1_G("l3_message_length > %d, truncating", MAX_OCTETS_L3_MESSAGE);
      l3_message_length = MAX_OCTETS_L3_MESSAGE;
    }

    rrc_cmd->cmd.interrat_nas_data_ind.l3_message_length = l3_message_length;

    /* l3_message is optional - if this is the case, l3_message will be NULL and */
    /* l3_message_length zero. The message should still be sent to RRC */
    if ((l3_message != NULL) && (l3_message_length > 0))
    {
		//Replace memcpy with memscpy
		memscpy(rrc_cmd->cmd.interrat_nas_data_ind.l3_message,
				sizeof(rrc_cmd->cmd.interrat_nas_data_ind.l3_message),
				l3_message,
				l3_message_length);

    }

    RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_nas_data_ind);
    rr_send_message_to_rrc(rrc_cmd, gas_id);
  }
}

#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH

#ifdef FEATURE_LTE

void rr_send_interrat_plmn_search_req(
  sys_plmn_rat_list_s_type *plmn_list,
  sys_network_selection_mode_e_type network_select_mode,
  uint32 timer_val,
  boolean camped_suitably,
  detected_frequency_list_type * detected_frequency_list,
  const gas_id_t gas_id
)
{
  uint32 i;
  uint32 num_plmns = 0;
  rrc_rr_cmd_type *rrc_cmd = NULL;
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_INTERRAT_PLMN_SRCH_REQ);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();
  rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rrc_cmd);

  for (i = 0; i < plmn_list->length && i < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH; i++)
  {
    if (plmn_list->info[i].rat == SYS_RAT_UMTS_RADIO_ACCESS)
    {
      rrc_cmd->cmd.interrat_plmn_srch_req.plmn[num_plmns] = plmn_list->info[i].plmn;
      num_plmns++;
    }
  }

  rrc_cmd->cmd.interrat_plmn_srch_req.num_plmns = num_plmns;
  rrc_cmd->cmd.interrat_plmn_srch_req.network_select_mode = network_select_mode;

#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  {
    rrc_cmd->cmd.interrat_plmn_srch_req.service_search = rr_plmn_list_req_info_ptr->service_search;
  }
#endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

  rrc_cmd->cmd.interrat_plmn_srch_req.timer_val = timer_val;

  rrc_cmd->cmd_hdr.cmd_id = RRC_INTERRAT_PLMN_SRCH_REQ;

#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
  rr_build_gsm_arfcn_list(&(rrc_cmd->cmd.interrat_plmn_srch_req.gsm_cell_list), gas_id);
#endif /* FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION */

  rrc_cmd->cmd.interrat_plmn_srch_req.camped_suitably = camped_suitably;
  rrc_cmd->cmd.interrat_plmn_srch_req.scan_scope = rr_plmn_list_req_info_ptr->scan_scope;

  if (detected_frequency_list != NULL)
  {
    rrc_cmd->cmd.interrat_plmn_srch_req.detected_frequency_list = *detected_frequency_list;
  }

  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_plmn_srch_req);
  rr_send_message_to_rrc(rrc_cmd, gas_id);
}

#else

void rr_send_interrat_plmn_search_req(
  sys_plmn_rat_list_s_type *plmn_list,
  sys_network_selection_mode_e_type network_select_mode,
  uint32 timer_val,
  boolean camped_suitably,
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type         *rrc_cmd = NULL;
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_INTERRAT_PLMN_SRCH_REQ);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();
  rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Out of memory");
  }
  else
  {
    uint32 i;
    uint32 num_plmns = 0;

    for (i = 0; i < plmn_list->length && i < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH; i++)
    {
      if (plmn_list->info[i].rat == SYS_RAT_UMTS_RADIO_ACCESS)
      {
        rrc_cmd->cmd.interrat_plmn_srch_req.plmn[num_plmns] = plmn_list->info[i].plmn;
        num_plmns++;
      }
    }

    rrc_cmd->cmd.interrat_plmn_srch_req.num_plmns = num_plmns;
    rrc_cmd->cmd.interrat_plmn_srch_req.network_select_mode = network_select_mode;

#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
    {
      rrc_cmd->cmd.interrat_plmn_srch_req.service_search = rr_plmn_list_req_info_ptr->service_search;
    }
#endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

    rrc_cmd->cmd.interrat_plmn_srch_req.timer_val = timer_val;

    rrc_cmd->cmd_hdr.cmd_id = RRC_INTERRAT_PLMN_SRCH_REQ;

#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
    rr_build_gsm_arfcn_list(&(rrc_cmd->cmd.interrat_plmn_srch_req.gsm_cell_list), gas_id);
#endif /* FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION */

    rrc_cmd->cmd.interrat_plmn_srch_req.camped_suitably = camped_suitably;
    rrc_cmd->cmd.interrat_plmn_srch_req.scan_scope = rr_plmn_list_req_info_ptr->scan_scope;

    RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_plmn_srch_req);
    rr_send_message_to_rrc(rrc_cmd, gas_id);
  }
}

#endif /* FEATURE_LTE */

void rr_send_interrat_plmn_search_abort_req(
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_INTERRAT_PLMN_SRCH_ABORT_REQ);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Out of memory");
  }
  else
  {
    rrc_cmd->cmd_hdr.cmd_id = RRC_INTERRAT_PLMN_SRCH_ABORT_REQ;
    /* @@TODO no abort cause in this signal ? */
    RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_plmn_srch_abort_req);
    rr_send_message_to_rrc(rrc_cmd, gas_id);
  }
}

#endif /* FEATURE_GTOW_BACKGROUND_PLMN_SEARCH */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG

#ifdef FEATURE_WCDMA
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION

void rr_send_interrat_redirect_rej_service_search_success(
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Out of memory");
  }
  else
  {
    MSG_GERAN_MED_0_G("Sending RRC_INTERRAT_REDIRECT_REJ(SUCCESS) to RRC");
    rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_REDIRECT_REJ;
    rrc_cmd->cmd.interrat_redirect_rej.failure_reason = RR_RRC_REDIRECT_FAIL_OTHER_REASON;
    rrc_cmd->cmd.interrat_redirect_rej.num_avail_plmns = 1;

    RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_redirect_rej);
    rr_send_message_to_rrc(rrc_cmd, gas_id);
  }
}

void rr_send_interrat_redirect_rej_service_search_failure(
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_INTERRAT_REDIRECT_REJ);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Out of memory");
  }
  else
  {
    MSG_GERAN_MED_0_G("Sending RRC_INTERRAT_REDIRECT_REJ(FAILURE) to RRC");
    rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_REDIRECT_REJ;
    rrc_cmd->cmd.interrat_redirect_rej.failure_reason = RR_RRC_REDIRECT_FAIL_OTHER_REASON;
    rrc_cmd->cmd.interrat_redirect_rej.num_avail_plmns = 0;

#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
    rr_build_gsm_arfcn_list(&(rrc_cmd->cmd.interrat_redirect_rej.gsm_cell_list), gas_id);
#endif /* FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION */

    RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_redirect_rej);
    rr_send_message_to_rrc(rrc_cmd, gas_id);
  }
}

#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */
#endif /* FEATURE_WCDMA */


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
void rr_get_plmn_list_for_redirection_rej(
  rr_available_plmn_db_T    *available_plmn_db,
  byte                      *num_avail_plmns,
  byte                      *num_available_fplmns,
  PLMN_id_T                 *plmn,
  sys_signal_quality_e_type *signal_quality,
  sys_signal_strength_type  *signal_strength,
  const gas_id_t             gas_id
)
{
  uint32 i, j;
  byte num_plmns = 0;

  if ((available_plmn_db == NULL) ||
      (num_avail_plmns == NULL) ||
      (num_available_fplmns == NULL) ||
      (plmn == NULL) ||
      (signal_quality == NULL) ||
      (signal_strength == NULL)
     )
  {
    MSG_GERAN_ERROR_0_G("Null pointer");
    return;
  }

  *num_avail_plmns = 0;
#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
  *num_available_fplmns = 0;
#endif /* FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE */

  for (i = 0; (i < available_plmn_db->num_plmn) && (i < SYS_PLMN_LIST_MAX_LENGTH); i++)
  {
    boolean plmn_lai_forbidden = FALSE;

    /* to check if a PLMN is forbidden PLMN or all LAI are forbidden
     * only applicable for OUT_OF_SERVICE_SEARCH:
     *  -- if PLMN or Location area is forbidden, RR should not report the PLMN to RRC
     */
    if (rr_w2g_oos_search_in_progress(gas_id))
    {
      for (j = 0; j < available_plmn_db->plmn_info[i].num_frequencies; j++)
      {
        inter_task_lai_T inter_task_lai =
          rr_convert_rr_lai_to_inter_task_lai(&available_plmn_db->plmn_info[i].cell_info[j].lai);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
        if (mm_rr_plmn_lai_forbidden_per_subs(inter_task_lai, geran_map_gas_id_to_nas_id(gas_id)))
#else
        if (mm_rr_plmn_lai_forbidden(inter_task_lai))
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
        {
          continue;
        }

        if (rr_is_forbidden_LAI(&available_plmn_db->plmn_info[i].cell_info[j].lai, gas_id))
        {
          continue;
        }

        break;
      }

      if (j == available_plmn_db->plmn_info[i].num_frequencies)
      {
        /* all cells in the plmn are forbidden, therefore the PLMN is forbidden */
        plmn_lai_forbidden = TRUE;
      }
    }

    /* If FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE defined:
     *    all forbidden or non-forbidden PLMN to be sent to RRC - but indicate the
     *    number of forbidden/non-forbidden PLMN seperately
     *
     * If FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE is undefined:
     *    only send non-forbidden PLMNs
     */

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
    MSG_GERAN_MED_3_G("Adding PLMN 0x%02X-0x%02X-0x%02X into redirect rej",
      available_plmn_db->plmn_info[i].PLMN_id.identity[0],
      available_plmn_db->plmn_info[i].PLMN_id.identity[1],
      available_plmn_db->plmn_info[i].PLMN_id.identity[2]);

    plmn[num_plmns] = available_plmn_db->plmn_info[i].PLMN_id;

    /* available_plmn_db's cell_info is sorted by RxLev, decreasing in signal strength
     * see rr_plmn_db_add_plmn_frequency_info()
     */
    signal_strength[i] =
      (sys_signal_strength_type) rr_convert_rxlex_to_absolute_dBm(
         available_plmn_db->plmn_info[i].cell_info[0].meas.RXLEV_average);
    if (signal_strength[num_plmns] < 85)
    {
      signal_quality[num_plmns] = SYS_SIGNAL_QUALITY_HIGH;
    }
    else
    {
      signal_quality[num_plmns] = SYS_SIGNAL_QUALITY_LOW;
    }
    MSG_GERAN_MED_2_G("Sending signal quality %d, strength %d",
      signal_quality[num_plmns],
      signal_strength[num_plmns]);

    if (plmn_lai_forbidden)
    {
      /* all cells in the plmn are forbidden, therefore the PLMN is forbidden */
      (*num_available_fplmns) ++;
    }
    else
    {
      (*num_avail_plmns) ++;
    }
    num_plmns ++;
#else
    /* if FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE not defined,
     * only non forbidden PLMNs will be send
     */
    if (!plmn_lai_forbidden)
    {
      plmn[num_plmns] = available_plmn_db->plmn_info[i].PLMN_id;
      (*num_avail_plmns) ++;
      num_plmns++;
    }
#endif

  }/* loop through all pLMNs */

}
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
void rr_send_interrat_redirect_rej(
  rr_interrat_redirect_fail_reason_type failure_reason,
  rr_available_plmn_db_T *available_plmn_db,
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_INTERRAT_REDIRECT_REJ);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Out of memory");
  }
  else
  {
    MSG_GERAN_MED_1_G("Sending RRC_INTERRAT_REDIRECT_REJ(%d) to RRC", failure_reason);
    rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_REDIRECT_REJ;
    rrc_cmd->cmd.interrat_redirect_rej.failure_reason = failure_reason;

    if ((available_plmn_db != NULL)&&(available_plmn_db->valid))
    {
      rr_get_plmn_list_for_redirection_rej(
        available_plmn_db,
        &rrc_cmd->cmd.interrat_redirect_rej.num_avail_plmns,
        &rrc_cmd->cmd.interrat_redirect_rej.num_available_fplmns,
        rrc_cmd->cmd.interrat_redirect_rej.plmn,
        rrc_cmd->cmd.interrat_redirect_rej.signal_quality,
        rrc_cmd->cmd.interrat_redirect_rej.signal_strength,
        gas_id
      );
    }
    else
    {
      MSG_GERAN_MED_0_G("available_plmn_db is not valid!");
      rrc_cmd->cmd.interrat_redirect_rej.num_avail_plmns = 0;
    }

    RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_redirect_rej);
    rr_send_message_to_rrc(rrc_cmd, gas_id);
  }
}

void rr_send_interrat_redirect_abort_cnf(
  rrc_rr_status_e_type abort_status,
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_INTERRAT_REDIRECT_ABORT_CNF);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Out of memory");
  }
  else
  {
    MSG_GERAN_MED_1_G("Sending RR_INTERRAT_REDIRECT_ABORT_CNF(%d) to RRC", abort_status);
    rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_REDIRECT_ABORT_CNF;
    rrc_cmd->cmd.interrat_redirect_abort_cnf.abort_status = abort_status;

    RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_redirect_abort_cnf);
    rr_send_message_to_rrc(rrc_cmd, gas_id);
  }
}
#endif /* FEATURE_WCDMA */

#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

#ifdef FEATURE_WCDMA
void rr_send_interrat_plmn_search_cnf(
  boolean srch_done,
  boolean hplmn_found,
  sys_detailed_plmn_list_s_type *plmn_list,
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_INTERRAT_PLMN_SRCH_CNF);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Out of memory");
    return;
  }

  MSG_GERAN_HIGH_2_G("Sending RR_INTERRAT_PLMN_SRCH_CNF to RRC, done=%d, hplmn_found=%d",\
    srch_done, hplmn_found);
  rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_PLMN_SRCH_CNF;
  rrc_cmd->cmd.interrat_plmn_srch_cnf.srch_done = srch_done;
  rrc_cmd->cmd.interrat_plmn_srch_cnf.hplmn_found = hplmn_found;
  if (plmn_list != NULL)
  {
    rrc_cmd->cmd.interrat_plmn_srch_cnf.available_plmn_list = *plmn_list;
  }
  else
  {
    memset(&rrc_cmd->cmd.interrat_plmn_srch_cnf.available_plmn_list,\
           0, sizeof(sys_detailed_plmn_list_s_type));
  }

#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
  rr_build_gsm_arfcn_list(&rrc_cmd->cmd.interrat_plmn_srch_cnf.gsm_cell_list, gas_id);
#endif /* FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION */

  rr_invalidate_csg_info(&rrc_cmd->cmd.interrat_plmn_srch_cnf.available_plmn_list);

  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_plmn_srch_cnf);
  rr_send_message_to_rrc(rrc_cmd, gas_id);
}

void rr_send_interrat_plmn_search_abort_cnf(
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_INTERRAT_PLMN_SRCH_ABORT_CNF);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Out of memory");
    return;
  }

  rrc_cmd->cmd_hdr.cmd_id = RR_INTERRAT_PLMN_SRCH_ABORT_CNF;
  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_plmn_srch_abort_cnf);
  rr_send_message_to_rrc(rrc_cmd, gas_id);
}

void rr_send_cipher_sync_ind(const gas_id_t gas_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* sanity check the active subscription's multimode support */
  if (!geran_is_multi_mode_gas_id(gas_id))
  {
    MSG_GERAN_HIGH_0_G("No need for RR_CIPHER_SYNC_IND when dual RAT disabled");
  }
  else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  {
    rrc_rr_cmd_type *rrc_cmd = NULL;
  
    if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
    {
      MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_CIPHER_SYNC_IND);
      return;
    }
  
    rrc_cmd = rrc_get_rr_cmd_buf();

    if (rrc_cmd == NULL)
    {
      MSG_GERAN_ERROR_0_G("Failed to send RR_CIPHER_SYNC_IND, out of memory");
    }
    else
    {
      rrc_cmd->cmd_hdr.cmd_id = RR_CIPHER_SYNC_IND;
      RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.cipher_sync_ind);
      rr_send_message_to_rrc(rrc_cmd, gas_id);
    }
  }
}

void rr_send_rrc_stop_wcdma_mode_req(
  rrc_rr_stop_wcdma_cause_e_type cause,
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_RR_STOP_WCDMA_REQ);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Failed to send RRC_RR_STOP_WCDMA_REQ, out of memory");
  }
  else
  {
    rrc_cmd->cmd_hdr.cmd_id = RRC_RR_STOP_WCDMA_REQ;
    rrc_cmd->cmd.rrc_rr_stop_wcdma_req.stop_req_cause = cause;

    RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.rrc_rr_stop_wcdma_req);
    rr_send_message_to_rrc(rrc_cmd, gas_id);
  }
}

void rr_send_rrc_stop_gsm_mode_cnf(
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_RRC_STOP_GSM_CNF);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Failed to send RR_RRC_STOP_GSM_CNF, out of memory");
  }
  else
  {
    rrc_cmd->cmd_hdr.cmd_id = RR_RRC_STOP_GSM_CNF;
    rrc_cmd->cmd.rr_rrc_stop_gsm_cnf.stop_cnf_cause = GO_TO_INACTIVE;

    RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.rr_rrc_stop_gsm_cnf);
    rr_send_message_to_rrc(rrc_cmd, gas_id);
  }
}

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE

void rr_send_interrat_gtow_redirection_req(
  rr_rrc_fdd_target_info_type  * fdd_target,
  sys_rat_pri_list_info_s_type * rat_pri_list_info,
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_INTERRAT_REDIRECT_REQ);
    return;
  }

  RR_NULL_CHECK_RETURN_VOID(fdd_target);
  RR_NULL_CHECK_RETURN_VOID(rat_pri_list_info);

  rrc_cmd = rrc_get_rr_cmd_buf();
  RR_NULL_CHECK_RETURN_VOID(rrc_cmd);

  MSG_GERAN_HIGH_0_G("Sending GTOW_REDIRECTION_REQ to RRC");
  rrc_cmd->cmd_hdr.cmd_id = RRC_INTERRAT_REDIRECT_REQ;
  rrc_cmd->cmd.interrat_redirect_req.PLMN = rr_PLMN_request_details_ptr->requested_PLMN_id;
  rrc_cmd->cmd.interrat_redirect_req.network_select_mode = rr_PLMN_request_details_ptr->network_select_mode;
  rrc_cmd->cmd.interrat_redirect_req.fdd_target_info = *fdd_target;

#ifdef FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
  rr_build_gsm_arfcn_list(&rrc_cmd->cmd.interrat_redirect_req.gsm_cell_list, gas_id);
#endif /* FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION */

  rrc_cmd->cmd.interrat_redirect_req.rat_pri_list_info = *rat_pri_list_info;

#ifdef FEATURE_LTE
  rr_dedicated_priority_info_get(&rrc_cmd->cmd.interrat_redirect_req.dedicated_priority_info, gas_id);
#endif /* FEATURE_LTE */

  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_redirect_req);
  rr_send_message_to_rrc(rrc_cmd, gas_id);

}

void rr_send_interrat_gtow_redirection_abort_req(
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_INTERRAT_REDIRECT_ABORT_REQ);
    return;
  }

  rrc_cmd = rrc_get_rr_cmd_buf();

  if (rrc_cmd == NULL)
  {
    MSG_GERAN_ERROR_0_G("Failed to send GTOW redirection request, out of memory");
    return;
  }
  rrc_cmd->cmd_hdr.cmd_id = RRC_INTERRAT_REDIRECT_ABORT_REQ;
  RRC_DS_INSERT_AS_ID(rrc_cmd->cmd.interrat_redirect_abort_req);
  rr_send_message_to_rrc(rrc_cmd, gas_id);
}

#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */


#ifdef FEATURE_FEMTO_GSM_NL

void rr_send_rrc_nbr_disc_cnf(
  uint8 num_cells,
  sys_detailed_gsm_list_info_s_type *cell_list
)
{
  rrc_rr_cmd_type *rrc_cmd_ptr = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_RRC_NBR_DISC_CNF);
    return;
  }

  rrc_cmd_ptr = rrc_get_rr_cmd_buf();

  if (rrc_cmd_ptr)
  {
    uint8 index;
    rr_rrc_nbr_disc_cnf_type *disc_cnf_ptr = &rrc_cmd_ptr->cmd.nbr_disc_cnf;

    rrc_cmd_ptr->cmd_hdr.cmd_id = RR_RRC_NBR_DISC_CNF;

    memset(disc_cnf_ptr, 0, sizeof(rr_rrc_nbr_disc_cnf_type));

    for (index = 0; (index < num_cells) && (index < SYS_MAX_GSM_NBRS); index++)
    {
      disc_cnf_ptr->discovered_nbr_list.gsm_nbr_info[index] = cell_list[index];
    }

    disc_cnf_ptr->discovered_nbr_list.g_nbr_cnt = index;

    if (index > 0)
    {
      disc_cnf_ptr->result_incl = TRUE;
    }
    else
    {
      disc_cnf_ptr->result_incl = FALSE;
    }

    rr_send_message_to_rrc(rrc_cmd_ptr);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rrc_cmd_ptr == NULL");
  }
} /* rr_send_rrc_nbr_disc_cnf */

void rr_send_rrc_start_cosc_cnf(
  boolean camped,
  uint16 cell_id,
  BSIC_T bsic
)
{
  rrc_rr_cmd_type *rrc_cmd_ptr = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_RRC_START_COSC_CNF);
    return;
  }

  rrc_cmd_ptr = rrc_get_rr_cmd_buf();

  if (rrc_cmd_ptr)
  {
    rr_rrc_start_cosc_cnf_type *cosc_cnf_ptr = &rrc_cmd_ptr->cmd.start_cosc_cnf;

    rrc_cmd_ptr->cmd_hdr.cmd_id = RR_RRC_START_COSC_CNF;

    if (camped)
    {
      cosc_cnf_ptr->service_status = COSC_SERVICE_AVAILABLE;
      cosc_cnf_ptr->acq_cell_id = (uint32) cell_id;

      // acq_cell_bsic:
      // The format used is given in 3GPP 23.003 4.3.2
      //  7  6  5  4  3  2  1  0
      //        |     |  |     |
      //        |     |    BCC
      //          NCC
      cosc_cnf_ptr->acq_cell_bsic = ((bsic.PLMN_colour_code & 0x07) << 3) | (bsic.BS_colour_code & 0x07);
    }
    else
    {
      cosc_cnf_ptr->service_status = COSC_NO_SERVICE;
    }

    rr_send_message_to_rrc(rrc_cmd_ptr);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rrc_cmd_ptr == NULL");
  }
} /* rr_send_rrc_start_cosc_cnf */


void rr_send_rrc_stop_cosc_cnf(
  void
)
{
  rrc_rr_cmd_type *rrc_cmd_ptr = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_RRC_STOP_COSC_CNF);
    return;
  }

  rrc_cmd_ptr = rrc_get_rr_cmd_buf();

  if (rrc_cmd_ptr)
  {
    rrc_cmd_ptr->cmd_hdr.cmd_id = RR_RRC_STOP_COSC_CNF;

    rr_send_message_to_rrc(rrc_cmd_ptr);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rrc_cmd_ptr == NULL");
  }
} /* rr_send_rrc_stop_cosc_cnf */


void rr_send_rrc_service_lost_ind(
  void
)
{
  rrc_rr_cmd_type *rrc_cmd_ptr = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_RRC_SERVICE_LOST_IND);
    return;
  }

  rrc_cmd_ptr = rrc_get_rr_cmd_buf();

  if (rrc_cmd_ptr)
  {
    rrc_cmd_ptr->cmd_hdr.cmd_id = RR_RRC_SERVICE_LOST_IND;

    rr_send_message_to_rrc(rrc_cmd_ptr);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rrc_cmd_ptr == NULL");
  }
} /* rr_send_rrc_service_lost_ind */

#endif /* FEATURE_FEMTO_GSM_NL */

#ifdef FEATURE_LTE

void rr_send_rrc_clear_dedicated_priorities_ind(
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd_ptr = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_CLEAR_DEDICATED_PRIORITIES_IND);
    return;
  }

  rrc_cmd_ptr = rrc_get_rr_cmd_buf();

  if (rrc_cmd_ptr)
  {
    rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_CLEAR_DEDICATED_PRIORITIES_IND;

    RRC_DS_INSERT_AS_ID(rrc_cmd_ptr->cmd.clear_dedicated_priorities_ind);
    rr_send_message_to_rrc(rrc_cmd_ptr, gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rrc_cmd_ptr == NULL");
  }
}

void rr_send_rrc_dedicated_priorities_req(
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd_ptr = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RRC_DEDICATED_PRIORITIES_REQ);
    return;
  }

  rrc_cmd_ptr = rrc_get_rr_cmd_buf();

  if (rrc_cmd_ptr)
  {
    rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_DEDICATED_PRIORITIES_REQ;

    RRC_DS_INSERT_AS_ID(rrc_cmd_ptr->cmd.dedicated_priorities_req);
    rr_send_message_to_rrc(rrc_cmd_ptr, gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rrc_cmd_ptr == NULL");
  }
}

void rr_send_rrc_dedicated_priorities_rsp(
  const gas_id_t gas_id
)
{
  rrc_rr_cmd_type *rrc_cmd_ptr = NULL;

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_ERROR_1_G("RR Can't send RRC message %d, RRC not loaded",RR_DEDICATED_PRIORITIES_RSP);
    return;
  }

  rrc_cmd_ptr = rrc_get_rr_cmd_buf();

  if (rrc_cmd_ptr)
  {
    rrc_cmd_ptr->cmd_hdr.cmd_id = RR_DEDICATED_PRIORITIES_RSP;

    if (rr_individual_priorities_valid(gas_id))
    {
      rrc_cmd_ptr->cmd.dedicated_priorities_rsp.dedicated_pri_valid = TRUE;
      rrc_cmd_ptr->cmd.dedicated_priorities_rsp.plmn = rr_dedicated_priority_info_get_plmn(gas_id);
      rr_dedicated_priority_info_get(
        &rrc_cmd_ptr->cmd.dedicated_priorities_rsp.dedicated_pri_info,
        gas_id
      );
    }
    else
    {
      rrc_cmd_ptr->cmd.dedicated_priorities_rsp.dedicated_pri_valid = FALSE;
    }

    RRC_DS_INSERT_AS_ID(rrc_cmd_ptr->cmd.dedicated_priorities_rsp);
    rr_send_message_to_rrc(rrc_cmd_ptr, gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rrc_cmd_ptr == NULL");
  }
}

#endif /* FEATURE_LTE */
#endif /* FEATURE_WCDMA */

#else
/* no WCDMA support in this build */
int rr_rrc_messages_supported(void)
{
  return(0);
}
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

/* EOF */

