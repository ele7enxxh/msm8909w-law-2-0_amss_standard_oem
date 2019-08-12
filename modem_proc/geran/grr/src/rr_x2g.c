#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
#include "rr_control.h"
#include "comdef.h"
#include "rr_x2g.h"
#ifdef FEATURE_WCDMA
#include "rr_rrc_send.h"
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
#include "rr_tds_send.h"
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
#include "rr_msgr.h"
#endif /* FEATURE_LTE */

/* PUBLIC Functions declared below */
#ifdef FEATURE_WCDMA

/**
  @brief rr_x2g_map_resel_status_to_w2g_cco_status.

  This Function takes rr_x2g_resel_Status as Input and Maps it to correct
   cco status.

  @return rr_interrat_cc_order_fail_reason_type, i.e. the correct CCO Failure reason.
*/
rr_interrat_cc_order_fail_reason_type rr_x2g_map_resel_status_to_w2g_cco_status(
  rr_x2g_resel_status_e resel_status
)
{
  switch (resel_status)
  {
    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BARRED:
      return RR_GSM_CCO_CELL_BARRED;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_LOW_C1:
      return RR_GSM_CCO_CELL_LOW_C1;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_PLMN_MISMATCH:
      /* this has no direct mapping in the CCO failure enum */
      return RR_GSM_CCO_OTHER_REASON;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_FORBIDDEN:
      return RR_GSM_CCO_CELL_FORBIDDEN;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BSIC_MISMATCH:
      return RR_GSM_CCO_BSIC_NOT_FOUND;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_NOT_FOUND:
      return RR_GSM_CCO_BCCH_NOT_FOUND;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_DECODE_FAILED:
      return RR_GSM_CCO_BCCH_DECODE_FAIL;

    default:
      MSG_GERAN_ERROR_1("Unexpected resel_status: %d", resel_status);
      return RR_GSM_CCO_OTHER_REASON;
  }
}


/**
  @brief rr_x2g_map_cco_status_to_w2g_cco_status.

   This Function takes the CCO Status and Maps it to Suitable CCO Failure Reason

  @return rr_interrat_cc_order_fail_reason_type, i.e. the correct CCO Failure reason.
*/
rr_interrat_cc_order_fail_reason_type rr_x2g_map_cco_status_to_w2g_cco_status(
  rr_cco_status_e status
)
{
  switch (status)
  {
    case RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY:              return RR_GSM_CCO_T309_EXPIRED;
    case RR_CCO_STATUS_FAILURE_NO_RESPONSE:                   return RR_GSM_CCO_OTHER_REASON;
    case RR_CCO_STATUS_FAILURE_IA_REJECT:                     return RR_GSM_CCO_OTHER_REASON;
    case RR_CCO_STATUS_FAILURE_CONTENTION_RESOLUTION_FAILURE: return RR_GSM_CCO_OTHER_REASON;
    case RR_CCO_STATUS_FAILURE_DSF:                           return RR_GSM_CCO_OTHER_REASON;
    default:                                                  return RR_GSM_CCO_OTHER_REASON;
  }
}

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS

/**
  @brief rr_x2g_map_resel_status_to_t2g_cco_status.

   This Function takes rr_x2g_resel_Status as Input and Maps it to correct T2G
   cco status.

  @return rr_interrat_cc_order_fail_reason_type, i.e. the correct CCO Failure reason.
*/

tdsrr_interrat_cc_order_fail_reason_type rr_x2g_map_resel_status_to_t2g_cco_status(
  rr_x2g_resel_status_e resel_status
)
{
  switch (resel_status)
  {
    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BARRED:
      return TDSRR_GSM_CCO_CELL_BARRED;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_LOW_C1:
      return TDSRR_GSM_CCO_CELL_LOW_C1;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_FORBIDDEN:
      return TDSRR_GSM_CCO_CELL_FORBIDDEN;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_PLMN_MISMATCH:
      return TDSRR_GSM_CCO_OTHER_REASON;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BSIC_MISMATCH:
      return TDSRR_GSM_CCO_BSIC_NOT_FOUND;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_NOT_FOUND:
      return TDSRR_GSM_CCO_BCCH_NOT_FOUND;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_DECODE_FAILED:
      return TDSRR_GSM_CCO_BCCH_DECODE_FAIL;

    default:
      MSG_GERAN_ERROR_1("Unexpected resel_status: %d", resel_status);
      return TDSRR_GSM_CCO_OTHER_REASON;
  }
}

/**
  @brief rr_x2g_map_cco_status_to_t2g_cco_status.

   This Function takes the CCO Status and Maps it to Suitable T2G CCO Failure Reason

  @return rr_interrat_cc_order_fail_reason_type, i.e. the correct T2G CCO Failure reason.
*/
tdsrr_interrat_cc_order_fail_reason_type rr_x2g_map_cco_status_to_t2g_cco_status(
  rr_cco_status_e status
)
{
  switch (status)
  {
    case RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY:              return TDSRR_GSM_CCO_T309_EXPIRED;
    case RR_CCO_STATUS_FAILURE_NO_RESPONSE:                   return TDSRR_GSM_CCO_OTHER_REASON;
    case RR_CCO_STATUS_FAILURE_IA_REJECT:                     return TDSRR_GSM_CCO_OTHER_REASON;
    case RR_CCO_STATUS_FAILURE_CONTENTION_RESOLUTION_FAILURE: return TDSRR_GSM_CCO_OTHER_REASON;
    case RR_CCO_STATUS_FAILURE_DSF:                           return TDSRR_GSM_CCO_OTHER_REASON;
    default:                                                  return TDSRR_GSM_CCO_OTHER_REASON;
  }
}

#endif /* FEATURE_GSM_TDS */

void rr_x2g_send_x2g_resel_abort_rsp(
  rr_rat_e rat,
  const gas_id_t gas_id
)
{
  switch (rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      rr_send_interrat_reselection_abort_cnf(gas_id);
      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      rr_send_tds_interrat_reselection_abort_cnf(gas_id);
      break;
    } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      msgr_hdr_struct_type lte_abort_resel_rsp;
      errno_enum_type      result;

      memset(&lte_abort_resel_rsp, 0, sizeof(lte_abort_resel_rsp));

      msgr_init_hdr(
        &lte_abort_resel_rsp,
        MSGR_GERAN_GRR,
        GERAN_GRR_LTE_ABORT_RESEL_RSP
      );
      result = rr_msgr_send(&lte_abort_resel_rsp, sizeof(lte_abort_resel_rsp), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      break;
    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rat);
    }
  }
}

void rr_x2g_send_x2g_redir_abort_rsp(
  rr_rat_e rat,
  const gas_id_t gas_id
)
{
  switch (rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      rr_send_interrat_redirect_abort_cnf(RRC_RR_SUCCESS, gas_id);
      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      rr_send_tds_interrat_redirect_abort_cnf(TDSRRC_RR_SUCCESS, gas_id);
      break;
    } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      msgr_hdr_struct_type lte_abort_redir_rsp;
      errno_enum_type      result;

      memset(&lte_abort_redir_rsp, 0, sizeof(lte_abort_redir_rsp));

      msgr_init_hdr(
        &lte_abort_redir_rsp,
        MSGR_GERAN_GRR,
        GERAN_GRR_LTE_ABORT_REDIR_RSP
      );
      result = rr_msgr_send(&lte_abort_redir_rsp, sizeof(lte_abort_redir_rsp), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      break;
    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rat);
    }
  }
}


void rr_x2g_send_x2g_cco_rsp_success(
  rr_rat_e rat,
  const gas_id_t gas_id
)
{
  switch (rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      rr_send_interrat_cc_order_cnf(
        CCO_SUCCESS,
        RR_GSM_CCO_OTHER_REASON,
        gas_id
      );
      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      rr_send_tds_interrat_cc_order_cnf(
        TDSCCO_SUCCESS,
        TDSRR_GSM_CCO_OTHER_REASON,
        gas_id
      );
      break;
    } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      geran_grr_lte_cco_rsp_t lte_cco_rsp;
      errno_enum_type         result;

      memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

      msgr_init_hdr(
        &lte_cco_rsp.hdr,
        MSGR_GERAN_GRR,
        GERAN_GRR_LTE_CCO_RSP
      );
      lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_SUCCESS;
      result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      break;
    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rat);
    }
  }
}

void rr_x2g_send_x2g_cco_rsp_rejection(
  rr_rat_e rat,
  const gas_id_t gas_id
)
{
  switch (rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      MSG_GERAN_HIGH_0_G("Rejecting W2G Cell Change Order");
      rr_send_interrat_cc_order_cnf(
        CCO_FAILURE_BEFORE_ACT_IND,
        RR_GSM_CCO_OTHER_REASON,
        gas_id
      );
      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      MSG_GERAN_HIGH_0_G("Rejecting T2G Cell Change Order");
      rr_send_tds_interrat_cc_order_cnf(
        TDSCCO_FAILURE_BEFORE_ACT_IND,
        TDSRR_GSM_CCO_OTHER_REASON,
        gas_id
      );
      break;
    } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      geran_grr_lte_cco_rsp_t lte_cco_rsp;
      errno_enum_type         result;

      MSG_GERAN_HIGH_0_G("Rejecting L2G Cell Change Order");

      memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

      msgr_init_hdr(
        &lte_cco_rsp.hdr,
        MSGR_GERAN_GRR,
        GERAN_GRR_LTE_CCO_RSP
      );
      lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_ACQUIRE;
      result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      break;
    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rat);
    }
  }
}

void rr_x2g_send_x2g_cco_rsp_aborted_by_nas(
  rr_rat_e rat,
  const gas_id_t gas_id
)
{
  switch (rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      rr_send_interrat_cc_order_cnf(
        CCO_SUCCESS,
        RR_GSM_CCO_OTHER_REASON,
        gas_id
      );
      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      rr_send_tds_interrat_cc_order_cnf(
        TDSCCO_SUCCESS,
        TDSRR_GSM_CCO_OTHER_REASON,
        gas_id
      );
      break;
    } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      geran_grr_lte_cco_rsp_t lte_cco_rsp;
      errno_enum_type         result;

      memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

      msgr_init_hdr(
        &lte_cco_rsp.hdr,
        MSGR_GERAN_GRR,
        GERAN_GRR_LTE_CCO_RSP
      );
      lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_ABORTED_DUE_TO_HIGH_PRI_REQ;
      result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      break;
    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rat);
    }
  }
}

void rr_x2g_send_x2g_cco_rsp_failure_before_act_ind(
  rr_rat_e rat,
  rr_x2g_resel_status_e x2g_resel_status,
  const gas_id_t gas_id
)
{
  switch (rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      rr_send_interrat_cc_order_cnf(
        CCO_FAILURE_BEFORE_ACT_IND,
        rr_x2g_map_resel_status_to_w2g_cco_status(x2g_resel_status),
        gas_id
      );
      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      rr_send_tds_interrat_cc_order_cnf(
        TDSCCO_FAILURE_BEFORE_ACT_IND,
        rr_x2g_map_resel_status_to_t2g_cco_status(x2g_resel_status),
        gas_id
      );
      break;
    } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      geran_grr_lte_cco_rsp_t lte_cco_rsp;
      errno_enum_type         result;

      memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

      msgr_init_hdr(
        &lte_cco_rsp.hdr,
        MSGR_GERAN_GRR,
        GERAN_GRR_LTE_CCO_RSP
      );
      lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_ACQUIRE;
      result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      break;
    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rat);
    }
  }

}

void rr_x2g_send_x2g_cco_rsp_failure_after_act_ind(
  rr_rat_e        rat,
  rr_cco_status_e cco_status,
  const gas_id_t gas_id
)
{
  switch (rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      rr_send_interrat_cc_order_cnf(
        CCO_FAILURE_AFTER_ACT_IND,
        rr_x2g_map_cco_status_to_w2g_cco_status(cco_status),
        gas_id
      );
      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      rr_send_tds_interrat_cc_order_cnf(
        TDSCCO_FAILURE_AFTER_ACT_IND,
        rr_x2g_map_cco_status_to_t2g_cco_status(cco_status),
        gas_id
      );
      break;
    } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      geran_grr_lte_cco_rsp_t lte_cco_rsp;
      errno_enum_type         result;

      memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

      msgr_init_hdr(
        &lte_cco_rsp.hdr,
        MSGR_GERAN_GRR,
        GERAN_GRR_LTE_CCO_RSP
      );
      lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_RACH;
      result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      break;
    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rat);
    }
  }
}

void rr_x2g_send_x2g_cco_abort_rsp(
  rr_rat_e rat,
  const gas_id_t gas_id
)
{
  switch (rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      rr_send_interrat_cc_order_abort_cnf(RRC_RR_SUCCESS, gas_id);
      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      rr_send_tds_interrat_cc_order_abort_cnf(TDSRRC_RR_SUCCESS, gas_id);
      break;
    } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      msgr_hdr_struct_type header;
      errno_enum_type      result;

      memset(&header, 0, sizeof(header));

      msgr_init_hdr(
        &header,
        MSGR_GERAN_GRR,
        GERAN_GRR_LTE_ABORT_CCO_RSP
      );
      
      result = rr_msgr_send(&header, sizeof(header), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      break;
    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rat);
    }
  }
}


void rr_x2g_send_x2g_IRAT_rej(rr_x2g_resel_type_e x2g_resel_type,const gas_id_t gas_id)
{

 /* The procedure failed.  Send response to the
  appropriate calling RAT and return to INACTIVE state. */
 switch (x2g_resel_type)
 {
#ifdef FEATURE_GSM_TDS
   case RR_X2G_RESEL_TYPE_T2G_RESELECTION:
   {
       rr_send_tds_interrat_reselection_rej(
                     TDSRR_GSM_OTHER_REASON,
                     0,
                     gas_id
                     );
       break;
   }
   case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
   {
      (void)rr_timer_cancel(T3174, gas_id);
 	
       rr_send_tds_interrat_cc_order_cnf(
	                   TDSCCO_FAILURE_AFTER_ACT_IND,
	                   TDSRR_GSM_CCO_OTHER_REASON,
	                   gas_id
	                   );
                                      
       break;
   }                                              
#endif /* FEATURE_GSM_TDS */
 
#ifdef FEATURE_LTE
   case RR_X2G_RESEL_TYPE_L2G_RESELECTION:
   {
       geran_grr_lte_resel_failed_rsp_t lte_resel_failed_rsp;
       errno_enum_type                 result;
       memset(&lte_resel_failed_rsp, 0, sizeof(lte_resel_failed_rsp));
                   
       msgr_init_hdr(
              &lte_resel_failed_rsp.hdr,
              MSGR_GERAN_GRR,
              GERAN_GRR_LTE_RESEL_FAILED_RSP
              );
       lte_resel_failed_rsp.resel_fail_cause = GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_UNDEFINED;
       result = rr_msgr_send(&lte_resel_failed_rsp.hdr, sizeof(lte_resel_failed_rsp), gas_id);
 	   if (result != E_SUCCESS)
 	   {
 	       MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
 	   }
       break;
   }
   case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
   {
       geran_grr_lte_cco_rsp_t lte_cco_rsp;
 	   errno_enum_type        result;
 	
 	   memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));                  
 	   (void)rr_timer_cancel(T3174, gas_id);
 	                  
 	   msgr_init_hdr(
 	             &lte_cco_rsp.hdr,
 	             MSGR_GERAN_GRR,
 	             GERAN_GRR_LTE_CCO_RSP
 	             );
 	   lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_ABORTED_DUE_TO_HIGH_PRI_REQ;
 	   result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
 	   if (result != E_SUCCESS)
 	   {
 	       MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
 	   }
 	   break;
   }
#endif /* FEATURE_LTE */
   default:
   {
 	   MSG_GERAN_ERROR_1_G(
 	       "Unexpected x2g_resel_type: %d",
 	               x2g_resel_type
 	       );
   }
 }
}	



#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
