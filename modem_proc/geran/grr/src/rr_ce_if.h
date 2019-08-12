#ifndef RR_CE_IF_H
#define RR_CE_IF_H

/*! \file rr_ce_if.h 
 
  Signalling interface for the RR Connection Establishment module.
 
                Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_ce_if.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* Input messages. */
typedef enum
{
  RR_CE_IMSG_CS_ABORT_REQ,
  RR_CE_IMSG_COUNT
} rr_ce_imsg_e;

typedef struct
{
  rr_message_header_t  header;
  boolean              l1_in_null;
} rr_ce_imsg_cs_abort_req_t;

typedef union
{
  rr_message_header_t        header;
  rr_ce_imsg_cs_abort_req_t  cs_abort_req;
} rr_ce_imsg_u;

/* Output messages. */
typedef enum
{
  RR_CE_OMSG_X2G_CCO_STATUS_IND,
  RR_CE_OMSG_CS_ABORT_CNF,
  RR_CE_OMSG_CS_RELEASE_IND,
  RR_CE_OMSG_COUNT
} rr_ce_omsg_e;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)

typedef struct
{
  rr_message_header_t header;
  rr_cco_status_e     status;
} rr_ce_omsg_x2g_cco_status_ind_t;

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

typedef enum
{
  RR_CE_REL_STATUS_CAMP_ON_CELL,
  RR_CE_REL_STATUS_ACQUIRE_SI,
  RR_CE_REL_STATUS_RPS_REQUIRED,
  RR_CE_REL_STATUS_CAMPED,
  RR_CE_REL_STATUS_MAX
} rr_ce_rel_status_e;

typedef struct
{
  ARFCN_T arfcn;
  BSIC_T bsic;
  uint8 rxlev_average;
  inter_task_BCCH_allocation_T ba_sacch;
} rr_ce_rel_cell_info_t;

typedef struct
{
  rr_message_header_t header;
  rr_ce_rel_status_e status;
  rr_ce_rel_cell_info_t cell_info;
  boolean purge_gmm_signalling;
} rr_ce_omsg_cs_abort_cnf_t;

typedef struct
{
  rr_message_header_t header;
  rr_ce_rel_status_e status;
  rr_ce_rel_cell_info_t cell_info;
  boolean purge_gmm_signalling;
} rr_ce_omsg_cs_release_ind_t;

typedef union
{
  rr_message_header_t             header;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  rr_ce_omsg_x2g_cco_status_ind_t x2g_cco_status_ind;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  rr_ce_omsg_cs_release_ind_t     cs_release_ind;
} rr_ce_omsg_u;

/* Union of all messages. */
typedef union
{
  rr_ce_imsg_u imsg;
  rr_ce_omsg_u omsg;
} rr_ce_msg_u;

typedef struct
{
  boolean l1_in_null;
} rr_ce_rel_ded_proc_details_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns the text string for the given message
 * 
 * \param imsg (in)
 * 
 * \return const char* 
 */
extern const char *rr_ce_imsg_name(rr_ce_imsg_e imsg);

/*!
 * \brief Sends RR_CE_IMSG_CS_ABORT_REQ
 * 
 * \param ded_proc_in_progress (in)
 * \param ded_proc_details_ptr (in)
 * \param gas_id (in)
 */
extern void rr_ce_send_imsg_cs_abort_req(
  boolean ded_proc_in_progress,
  rr_ce_rel_ded_proc_details_t *ded_proc_details_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Sends RR_CE_IMSG_CS_RELEASE_IND
 * 
 * \param status (in) - release status which indicates the next action
 * \param arfcn (in) - cell ARFCN
 * \param bsic (in) - cell BSIC
 * \param rxlev_average (in) - cell RxLev (average)
 * \param ba_sacch_ptr (in) - copy of the BA-SACCH which is the cell list for the RPS (optional)
 * \param purge_gmm_signalling (in) - TRUE/FALSE whether to purge GMM signalling PDUs when camping-on
 * \param gas_id (in)
 */
extern void rr_ce_send_imsg_cs_release_ind(
  rr_ce_rel_status_e status,
  ARFCN_T arfcn,
  BSIC_T bsic,
  uint8 rxlev_average,
  inter_task_BCCH_allocation_T *ba_sacch_ptr,
  boolean purge_gmm_signalling,
  const gas_id_t gas_id
);

/*!
 * \brief Returns the text string for the given message
 * 
 * \param omsg (in)
 * 
 * \return const char* 
 */
extern const char *rr_ce_omsg_name(rr_ce_omsg_e omsg);

#if defined (FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

/*!
 * \brief Sends RR_CE_OMSG_X2G_CCO_STATUS_IND
 * 
 * \param status (in)
 * \param gas_id (in)
 */
extern void rr_ce_send_omsg_x2g_cco_status_ind(
  rr_cco_status_e status,
  const gas_id_t gas_id
);

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

/*!
 * \brief Sends RR_CE_OMSG_CS_ABORT_CNF
 * 
 * \param status (in) - release status which indicates the next action
 * \param arfcn (in) - cell ARFCN
 * \param bsic (in) - cell BSIC
 * \param rxlev_average (in) - cell RxLev (average)
 * \param ba_sacch_ptr (in) - copy of the BA-SACCH which is the cell list for the RPS (optional)
 * \param purge_gmm_signalling (in) - TRUE/FALSE whether to purge GMM signalling PDUs when camping-on
 * \param gas_id (in)
 */
extern void rr_ce_send_omsg_cs_abort_cnf(
  rr_ce_rel_status_e status,
  ARFCN_T arfcn,
  BSIC_T bsic,
  uint8 rxlev_average,
  inter_task_BCCH_allocation_T *ba_sacch_ptr,
  boolean purge_gmm_signalling,
  const gas_id_t gas_id
);

#endif /* #ifndef RR_CE_IF_H */

/* EOF */

