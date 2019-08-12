#ifndef RR_GRR_IF_H
#define RR_GRR_IF_H

/*============================================================================
  @file rr_grr_if.h

  Signalling interface for the GRR module.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_grr_if.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"
#include "rr_gprs_defs.h"
#include "csn_apt.h"
#include "rr_l1.h"
#include "rr_ce_paging.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* Input messages. */
typedef enum
{
  RR_GRR_IMSG_CAMPED_ON_CELL_IND,
  RR_GRR_IMSG_PS_ACCESS_IND,
  RR_GRR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND,
  RR_GRR_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND,
  RR_GRR_IMSG_CAMP_ON_CELL_REQ,
  RR_GRR_IMSG_PS_ACCESS_REQ,
  RR_GRR_IMSG_NO_PS_ACCESS_REQ,
  RR_GRR_IMSG_GPRS_IA_IND,
  RR_GRR_IMSG_RR_EVENT_IND,
  RR_GRR_IMSG_MSC_CHANGE_IND,
  RR_GRR_IMSG_X2G_CCO_COMPLETE_IND,
  RR_GRR_IMSG_PAGE_IND,
  RR_GRR_IMSG_PAGE_MODE_IND,
  RR_GRR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND,
  RR_GRR_IMSG_L1_IDLE_MODE_CHANGE_REQUIRED,
  RR_GRR_IMSG_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ,
  RR_GRR_IMSG_SGLTE_X2G_REQ,
  RR_GRR_IMSG_SGLTE_X2G_ABORT_REQ,
  RR_GRR_IMSG_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ,
  RR_GRR_IMSG_COUNT
} rr_grr_imsg_e;

typedef struct
{
  rr_message_header_t header;
} rr_grr_imsg_camped_on_cell_ind_t;

typedef struct
{
  rr_message_header_t header;
  boolean notify_mm_service_ind;
  boolean notify_mm_plmn_select_cnf;
  boolean purge_gmm_signalling;
} rr_grr_imsg_camp_on_cell_req_t;

typedef struct
{
  rr_message_header_t header;
  boolean purge_gmm_signalling;
  rr_cb_fn_t confirm_cb_fn;
} rr_grr_imsg_ps_access_req_t;

typedef struct
{
  rr_message_header_t header;
  boolean del_all_pdus;
  rr_cb_fn_t confirm_cb_fn;
} rr_grr_imsg_no_ps_access_req_t;

typedef struct
{
  rr_message_header_t          header;
  packet_channel_description_t packet_ch_desc;
  ia_rest_octets_t             ia_rest_octets;
  frequency_list_T             frequency_list;
  byte                         timing_advance;
} rr_grr_imsg_gprs_ia_ind_t;

typedef struct
{
  rr_message_header_t header;
  rr_event_T          rr_event;
} rr_grr_imsg_rr_event_ind_t;

typedef struct
{
  rr_message_header_t header;
} rr_grr_imsg_msc_change_ind_t;

typedef struct
{
  rr_message_header_t header;
  rr_page_info_t page_info;
} rr_grr_imsg_page_ind_t;

typedef struct
{
  rr_message_header_t header;
  paging_mode_T old_page_mode;
  paging_mode_T new_page_mode;
} rr_grr_imsg_page_mode_ind_t;

typedef struct
{
  rr_message_header_t header;
  rr_cb_fn_t          cb_fn;
} rr_grr_imsg_switch_to_cs_only_gsm_only_req_t;

#ifdef FEATURE_SGLTE

typedef struct
{
  rr_message_header_t          header;
  mm_as_status_T               activation_reason;
  sys_rat_pri_list_info_s_type rat_pri_list_info;
  sglte_l2g_resel_req_ded_pri_info_t lte_dedicated_pri; 
} rr_grr_imsg_sglte_x2g_req_t;

#endif /* FEATURE_SGLTE */

typedef union
{
  rr_message_header_t                   header;
  rr_grr_imsg_camped_on_cell_ind_t      camped_on_cell_ind;
  rr_grr_imsg_camp_on_cell_req_t        camp_on_cell_req;
  rr_grr_imsg_ps_access_req_t           ps_access_req;
  rr_grr_imsg_no_ps_access_req_t        no_ps_access_req;
  rr_grr_imsg_gprs_ia_ind_t             gprs_ia_ind;
  rr_grr_imsg_rr_event_ind_t            rr_event_ind;
  rr_grr_imsg_msc_change_ind_t          msc_change_ind;
  rr_grr_imsg_page_ind_t                page_ind;
  rr_grr_imsg_page_mode_ind_t           page_mode_ind;

#ifdef FEATURE_SGLTE
  rr_grr_imsg_switch_to_cs_only_gsm_only_req_t switch_to_cs_only_gsm_only_req;
  rr_grr_imsg_sglte_x2g_req_t                  sglte_x2g_cco_req;
#endif /* FEATURE_SGLTE */
} rr_grr_imsg_u;

/* Output messages. */
typedef enum
{
  RR_GRR_OMSG_X2G_CCO_STATUS_IND,
#ifdef FEATURE_SGLTE
  RR_GRR_OMSG_SGLTE_X2G_CNF,
  RR_GRR_OMSG_SGLTE_X2G_ABORT_CNF,
  RR_GRR_OMSG_SGLTE_X2G_REJECT_IND,  
#endif /* FEATURE_SGLTE */
  RR_GRR_OMSG_COUNT
} rr_grr_omsg_e;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

typedef struct
{
  rr_message_header_t header;
  rr_cco_status_e     status;
} rr_grr_omsg_x2g_cco_status_ind_t;

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

typedef union
{
  rr_message_header_t              header;
#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  rr_grr_omsg_x2g_cco_status_ind_t x2g_cco_status_ind;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
} rr_grr_omsg_u;

/* Union of all messages. */
typedef union
{
  rr_grr_imsg_u imsg;
  rr_grr_omsg_u omsg;
} rr_grr_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern const char * rr_grr_imsg_name(
  const rr_grr_imsg_e imsg
);

extern const char * rr_grr_omsg_name(
  const rr_grr_omsg_e omsg
);

/**
  @brief Sends a message without payload to GRR.

  @param imsg The unique message identifier for the message.
 */
extern void rr_grr_send_imsg(
  rr_grr_imsg_e imsg,
  const gas_id_t gas_id
);

/**
  @brief Sends RR_GRR_IMSG_CAMPED_ON_CELL_IND to GRR.
 */
extern void rr_grr_send_imsg_camped_on_cell_ind(
  const gas_id_t gas_id
);

/**
  @brief Sends RR_GRR_IMSG_CAMP_ON_CELL_REQ to GRR.
 */
extern void rr_grr_send_imsg_camp_on_cell_req(
  boolean notify_mm_service_ind,
  boolean notify_mm_plmn_select_cnf,
  boolean purge_gmm_signalling,
  const gas_id_t gas_id
);

/**
  @brief Sends RR_GRR_IMSG_PS_ACCESS_REQ to GRR.
   
  @param purge_gmm_signalling TRUE if GMM signalling PDUs should be deleted; FALSE otherwise
  @param confirm_cb_fn Call-back function to be called upon completion (NULL if not required)
 */
extern void rr_grr_send_imsg_ps_access_req(
  boolean purge_gmm_signalling,
  rr_cb_fn_t confirm_cb_fn,
  const gas_id_t gas_id
);

/**
  @brief Sends RR_GRR_IMSG_NO_PS_ACCESS_REQ to GRR. 
   
  @param del_all_pdus TRUE if all PDUs should be deleted; FALSE otherwise
  @param confirm_cb_fn Call-back function to be called upon completion (NULL if not required)
 */
extern void rr_grr_send_imsg_no_ps_access_req(
  boolean del_all_pdus,
  rr_cb_fn_t confirm_cb_fn,
  const gas_id_t gas_id
);

/**
  @brief Sends RR_GRR_IMSG_GPRS_IA_IND to GRR.
 */
extern void rr_grr_send_imsg_gprs_ia_ind(
  packet_channel_description_t *packet_ch_desc,
  ia_rest_octets_t *ia_rest_octets,
  frequency_list_T *frequency_list,
  byte timing_advance,
  const gas_id_t gas_id
);

/**
  @brief Sends RR_GRR_IMSG_RR_EVENT_IND to GRR.
 */
extern void rr_grr_send_imsg_rr_event_ind(
  rr_event_T rr_event,
  const gas_id_t gas_id
);

/**
  @brief Sends RR_GRR_IMSG_MSC_CHANGE_IND to GRR.
 */
extern void rr_grr_send_imsg_msc_change_ind(
  const gas_id_t gas_id
);

/**
  @brief Sends RR_GRR_IMSG_PAGE_IND to RR.
 */
extern void rr_grr_send_imsg_page_ind(
  rr_page_info_t *page_info_ptr,
  const gas_id_t gas_id
);

/**
  @brief Sends RR_GRR_IMSG_PAGE_MODE_IND to RR.
 */
extern void rr_grr_send_imsg_page_mode_ind(
  paging_mode_T old_page_mode,
  paging_mode_T new_page_mode,
  const gas_id_t gas_id
);

/**
  @brief Sends RR_GRR_IMSG_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ.
*/
extern void rr_grr_send_imsg_switch_to_cs_only_gsm_only_req(
  rr_cb_fn_t cb_fn,
  const gas_id_t gas_id
);

extern void rr_grr_send_imsg_switch_to_cs_only_user_gsm_only_req(
  rr_cb_fn_t cb_fn,
  const gas_id_t gas_id
);
    

#ifdef FEATURE_SGLTE

/**
  @brief Sends RR_GRR_IMSG_SGLTE_X2G_REQ to GRR.
*/
extern void rr_grr_send_imsg_sglte_x2g_req(
  mm_as_status_T                 activation_reason,
  sys_rat_pri_list_info_s_type * rat_pri_list_info,
  sglte_l2g_resel_req_ded_pri_info_t * lte_resel_req,
  const gas_id_t gas_id
);

#endif /* FEATURE_SGLTE */

/**
  @brief Sends a message without payload to GRR.

  @param imsg The unique message identifier for the message.
 */
extern void rr_grr_send_omsg(
  rr_grr_imsg_e omsg,
  const gas_id_t gas_id
);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

/**
  @brief Sends RR_GRR_OMSG_X2G_CCO_STATUS_IND from GRR to it's caller.
 */
extern void rr_grr_send_omsg_x2g_cco_status_ind(
  rr_cco_status_e status,
  const gas_id_t gas_id
);

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#endif /* #ifndef RR_GRR_IF_H */

/* EOF */

