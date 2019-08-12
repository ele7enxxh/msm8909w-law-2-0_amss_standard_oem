/*============================================================================
  FILE:         rr_grr_if.c

  OVERVIEW:     This module provides functions to send signals to/from GRR
                (i.e. RR_GRR_IMSG, RR_GRR_OMSG).

  DEPENDENCIES: None

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_grr_if.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_grr_if.h"
#include "rr_rr.h"
#include "rr_gprs_debug.h"
#include "geran_multi_sim.h"
#include "gprs_mem.h"


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

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/

const char * rr_grr_imsg_name(const rr_grr_imsg_e imsg)
{
  switch (imsg)
  {
    case RR_GRR_IMSG_CAMPED_ON_CELL_IND:
      return "RR_GRR_IMSG_CAMPED_ON_CELL_IND";

    case RR_GRR_IMSG_PS_ACCESS_IND:
      return "RR_GRR_IMSG_PS_ACCESS_IND";

#ifdef FEATURE_LTE
    case RR_GRR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND:
      return "RR_GRR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND";

    case RR_GRR_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND:
      return "RR_GRR_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND";
#endif /* FEATURE_LTE */

    case RR_GRR_IMSG_CAMP_ON_CELL_REQ:
      return "RR_GRR_IMSG_CAMP_ON_CELL_REQ";

    case RR_GRR_IMSG_PS_ACCESS_REQ:
      return "RR_GRR_IMSG_PS_ACCESS_REQ";

    case RR_GRR_IMSG_NO_PS_ACCESS_REQ:
      return "RR_GRR_IMSG_NO_PS_ACCESS_REQ";

    case RR_GRR_IMSG_GPRS_IA_IND:
      return "RR_GRR_IMSG_GPRS_IA_IND";

    case RR_GRR_IMSG_RR_EVENT_IND:
      return "RR_GRR_IMSG_RR_EVENT_IND";

    case RR_GRR_IMSG_MSC_CHANGE_IND:
      return "RR_GRR_IMSG_MSC_CHANGE_IND";

    case RR_GRR_IMSG_X2G_CCO_COMPLETE_IND:
      return "RR_GRR_IMSG_X2G_CCO_COMPLETE_IND";

    case RR_GRR_IMSG_PAGE_IND:
      return "RR_GRR_IMSG_PAGE_IND";

    case RR_GRR_IMSG_PAGE_MODE_IND:
      return "RR_GRR_IMSG_PAGE_MODE_IND";

#ifdef FEATURE_LTE
    case RR_GRR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND:
      return "RR_GRR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND";
#endif /* FEATURE_LTE */

    case RR_GRR_IMSG_L1_IDLE_MODE_CHANGE_REQUIRED:
      return "RR_GRR_IMSG_L1_IDLE_MODE_CHANGE_REQUIRED";

#ifdef FEATURE_SGLTE
    case RR_GRR_IMSG_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ:
      return "RR_GRR_IMSG_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ";

    case RR_GRR_IMSG_SGLTE_X2G_REQ:
      return "RR_GRR_IMSG_SGLTE_X2G_REQ";

    case RR_GRR_IMSG_SGLTE_X2G_ABORT_REQ:
      return "RR_GRR_IMSG_SGLTE_X2G_ABORT_REQ";
#endif /* FEATURE_SGLTE */
    default:
      return "?";
  }
}

const char * rr_grr_omsg_name(const rr_grr_omsg_e omsg)
{
  switch (omsg)
  {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case RR_GRR_OMSG_X2G_CCO_STATUS_IND:
      return "RR_GRR_OMSG_X2G_CCO_STATUS_IND";
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_SGLTE
    case RR_GRR_OMSG_SGLTE_X2G_CNF:
      return "RR_GRR_OMSG_SGLTE_X2G_CNF";

    case RR_GRR_OMSG_SGLTE_X2G_ABORT_CNF:
      return "RR_GRR_OMSG_SGLTE_X2G_ABORT_CNF";
	  
    case RR_GRR_OMSG_SGLTE_X2G_REJECT_IND:
      return "RR_GRR_OMSG_SGLTE_X2G_REJECT_IND";		
	  	
#endif /* FEATURE_SGLTE */
    default:
      return "?";
  }
}

void rr_grr_send_imsg(
  rr_grr_imsg_e imsg,
  const gas_id_t gas_id
)
{
  rr_message_header_t header;

  memset(&header, 0, sizeof(header));

  rr_rr_send(
    &header,
    (uint8) imsg,
    sizeof(header),
    RR_GRR_IMSG,
    gas_id
  );
}

void rr_grr_send_imsg_camped_on_cell_ind(const gas_id_t gas_id)
{
  rr_grr_imsg_camped_on_cell_ind_t msg;

  memset(&msg, 0, sizeof(msg));

  rr_rr_send(
    &msg.header,
    (uint8) RR_GRR_IMSG_CAMPED_ON_CELL_IND,
    sizeof(msg),
    RR_GRR_IMSG,
    gas_id
  );
}

void rr_grr_send_imsg_camp_on_cell_req(
  boolean notify_mm_service_ind,
  boolean notify_mm_plmn_select_cnf,
  boolean purge_gmm_signalling,
  const gas_id_t gas_id
)
{
  rr_grr_imsg_camp_on_cell_req_t msg;

  MSG_GERAN_HIGH_3_G("RR_GRR_IMSG_CAMP_ON_CELL_REQ: service_ind=%d plmn_select_cnf=%d purge_gmm=%d",
           notify_mm_service_ind,
           notify_mm_plmn_select_cnf,
           purge_gmm_signalling);

  msg.notify_mm_service_ind = notify_mm_service_ind;
  msg.notify_mm_plmn_select_cnf = notify_mm_plmn_select_cnf;
  msg.purge_gmm_signalling = purge_gmm_signalling;

  rr_rr_send(
    &msg.header,
    (uint8) RR_GRR_IMSG_CAMP_ON_CELL_REQ,
    sizeof(msg),
    RR_GRR_IMSG,
    gas_id
  );
}

void rr_grr_send_imsg_ps_access_req(
  boolean purge_gmm_signalling,
  rr_cb_fn_t confirm_cb_fn,
  const gas_id_t gas_id
)
{
  rr_grr_imsg_ps_access_req_t msg;

  MSG_GERAN_HIGH_1_G("RR_GRR_IMSG_PS_ACCESS_REQ: purge_gmm=%d", purge_gmm_signalling);

  msg.purge_gmm_signalling = purge_gmm_signalling;
  msg.confirm_cb_fn = confirm_cb_fn;

  rr_rr_send(
    &msg.header,
    (uint8) RR_GRR_IMSG_PS_ACCESS_REQ,
    sizeof(msg),
    RR_GRR_IMSG,
    gas_id
  );
}

void rr_grr_send_imsg_no_ps_access_req(
  boolean del_all_pdus,
  rr_cb_fn_t confirm_cb_fn,
  const gas_id_t gas_id
)
{
  rr_grr_imsg_no_ps_access_req_t msg;

  msg.del_all_pdus = del_all_pdus;
  msg.confirm_cb_fn = confirm_cb_fn;

  rr_rr_send(
    &msg.header,
    (uint8) RR_GRR_IMSG_NO_PS_ACCESS_REQ,
    sizeof(msg),
    RR_GRR_IMSG,
    gas_id
  );
}

void rr_grr_send_imsg_gprs_ia_ind(
  packet_channel_description_t *packet_ch_desc,
  ia_rest_octets_t *ia_rest_octets,
  frequency_list_T *frequency_list,
  byte timing_advance,
  const gas_id_t gas_id
)
{
  rr_grr_imsg_gprs_ia_ind_t msg;

  RR_NULL_CHECK_RETURN_VOID(packet_ch_desc);
  RR_NULL_CHECK_RETURN_VOID(ia_rest_octets);
  RR_NULL_CHECK_RETURN_VOID(frequency_list);

  msg.packet_ch_desc = (*packet_ch_desc);
  msg.ia_rest_octets = (*ia_rest_octets);
  msg.frequency_list = (*frequency_list);
  msg.timing_advance = timing_advance;

  rr_rr_send(
    &msg.header,
    (uint8) RR_GRR_IMSG_GPRS_IA_IND,
    sizeof(msg),
    RR_GRR_IMSG,
    gas_id
  );
}

void rr_grr_send_imsg_rr_event_ind(
  rr_event_T rr_event,
  const gas_id_t gas_id
)
{
  rr_grr_imsg_rr_event_ind_t message;

  MSG_SPRINTF_1(MSG_SSID_DFLT,
                MSG_LEGACY_HIGH,
                "Sending RR_GRR_IMSG_RR_EVENT_IND with %s",
                rr_event_name(rr_event));

  message.rr_event = rr_event;

  rr_rr_send(
    &message.header,
    (uint8) RR_GRR_IMSG_RR_EVENT_IND,
    sizeof(message),
    RR_GRR_IMSG,
    gas_id
  );
}

void rr_grr_send_imsg_msc_change_ind(
  const gas_id_t gas_id
)
{
  rr_grr_imsg_msc_change_ind_t msg;

  rr_rr_send(
    &msg.header,
    (uint8) RR_GRR_IMSG_MSC_CHANGE_IND,
    sizeof(msg),
    RR_GRR_IMSG,
    gas_id
  );
}

void rr_grr_send_imsg_page_ind(
  rr_page_info_t *page_info_ptr,
  const gas_id_t gas_id
)
{
  rr_grr_imsg_page_ind_t msg;

  RR_NULL_CHECK_RETURN_VOID(page_info_ptr);

  msg.page_info = (*page_info_ptr);

  rr_rr_send(
    &msg.header,
    (uint8) RR_GRR_IMSG_PAGE_IND,
    sizeof(msg),
    RR_GRR_IMSG,
    gas_id
  );
}

void rr_grr_send_imsg_page_mode_ind(
  paging_mode_T old_page_mode,
  paging_mode_T new_page_mode,
  const gas_id_t gas_id
)
{
  rr_grr_imsg_page_mode_ind_t msg;

  msg.old_page_mode = old_page_mode;
  msg.new_page_mode = new_page_mode;

  rr_rr_send(
    &msg.header,
    (uint8) RR_GRR_IMSG_PAGE_MODE_IND,
    sizeof(msg),
    RR_GRR_IMSG,
    gas_id
  );
}


void rr_grr_send_imsg_switch_to_cs_only_gsm_only_req(
  rr_cb_fn_t cb_fn,
  const gas_id_t gas_id
)
{
  rr_grr_imsg_switch_to_cs_only_gsm_only_req_t msg;

  msg.cb_fn = cb_fn;

  rr_rr_send(&msg.header,
             (uint8) RR_GRR_IMSG_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ,
             sizeof(msg),
             RR_GRR_IMSG,
             gas_id);
}

void rr_grr_send_imsg_switch_to_cs_only_user_gsm_only_req(
  rr_cb_fn_t cb_fn,
  const gas_id_t gas_id
)
{
  rr_grr_imsg_switch_to_cs_only_gsm_only_req_t msg;

  msg.cb_fn = cb_fn;

  rr_rr_send(&msg.header,
             (uint8) RR_GRR_IMSG_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ,
             sizeof(msg),
             RR_GRR_IMSG,
             gas_id);
}


#ifdef FEATURE_SGLTE

void rr_grr_send_imsg_sglte_x2g_req(
  mm_as_status_T                 activation_reason,
  sys_rat_pri_list_info_s_type * rat_pri_list_info,
  sglte_l2g_resel_req_ded_pri_info_t * lte_dedicated_pri,
  const gas_id_t gas_id 
)
{
  rr_grr_imsg_sglte_x2g_req_t *msg; 
  
  msg = (rr_grr_imsg_sglte_x2g_req_t *)GPRS_MEM_CALLOC(sizeof(rr_grr_imsg_sglte_x2g_req_t));

  RR_NULL_CHECK_RETURN_VOID(msg);

  msg->activation_reason = activation_reason;
  if (rat_pri_list_info != NULL)
  {
    msg->rat_pri_list_info = *rat_pri_list_info;
  }

  if (lte_dedicated_pri != NULL)
  {
    msg->lte_dedicated_pri = *lte_dedicated_pri;
  }

  rr_rr_send(&msg->header,
             (uint8) RR_GRR_IMSG_SGLTE_X2G_REQ,
             sizeof(rr_grr_imsg_sglte_x2g_req_t),
             RR_GRR_IMSG,
             gas_id);

 GPRS_MEM_FREE(msg);    
}

#endif /* FEATURE_SGLTE */

void rr_grr_send_omsg(
  rr_grr_imsg_e omsg,
  const gas_id_t gas_id
)
{
  rr_message_header_t header;

  memset(&header, 0, sizeof(header));

  rr_rr_send(&header,
             (uint8) omsg,
             sizeof(header),
             RR_GRR_OMSG,
             gas_id);
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)

void rr_grr_send_omsg_x2g_cco_status_ind(
  rr_cco_status_e status,
  const gas_id_t gas_id
)
{
  rr_grr_omsg_x2g_cco_status_ind_t msg;

  msg.status = status;

  rr_rr_send(&msg.header,
             (uint8) RR_GRR_OMSG_X2G_CCO_STATUS_IND,
             sizeof(msg),
             RR_GRR_OMSG,
             gas_id);
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */



/* EOF */

