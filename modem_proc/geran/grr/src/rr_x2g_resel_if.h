#ifndef RR_X2G_RESEL_IF_H
#define RR_X2G_RESEL_IF_H

/*============================================================================
  @file rr_x2g_resel_if.h

  Signalling interface for the RR X2G reselection module module.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_x2g_resel_if.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)|| defined(FEATURE_GSM_TDS)

#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"
#include "mm_rr.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define RR_X2G_RESEL_SI_MESSAGES_MAX 10

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  RR_X2G_RESEL_STATUS_UNDEFINED = -1,
  RR_X2G_RESEL_STATUS_SUCCESS,
  RR_X2G_RESEL_STATUS_FAILURE_CELL_BARRED,
  RR_X2G_RESEL_STATUS_FAILURE_CELL_LOW_C1,
  RR_X2G_RESEL_STATUS_FAILURE_CELL_PLMN_MISMATCH,
  RR_X2G_RESEL_STATUS_FAILURE_CELL_FORBIDDEN,
  RR_X2G_RESEL_STATUS_FAILURE_CELL_BSIC_MISMATCH,
  RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_NOT_FOUND,
  RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_DECODE_FAILED,
  RR_X2G_RESEL_STATUS_MAX
} rr_x2g_resel_status_e;

/* Input messages. */
typedef enum
{
  RR_X2G_RESEL_IMSG_RESEL_REQ,
  RR_X2G_RESEL_IMSG_ABORT_REQ,
  RR_X2G_RESEL_IMSG_L1_DEACTIVATED_IND,
  RR_X2G_RESEL_IMSG_L1_ACTIVATED_IND,
  RR_X2G_RESEL_IMSG_PANIC_RESET_REQ,
  RR_X2G_RESEL_IMSG_COUNT
} rr_x2g_resel_imsg_e;

typedef struct
{
  rr_message_header_t           header;
  ARFCN_T                       arfcn;
  uint8                         rxlev;
  BSIC_T                        bsic;
  mm_as_status_T                activation_reason;
  boolean                       any_cell_reselection_mode;
  boolean                       requested_plmn_valid;
  PLMN_id_T                     requested_plmn;
  rr_x2g_system_information_t   system_information;
} rr_x2g_resel_imsg_resel_req_t;

typedef struct
{
  rr_message_header_t     header;
  rr_l1_gsm_stop_reason_T l1_stop_reason;
} rr_x2g_resel_imsg_abort_req_t;

typedef union
{
  rr_message_header_t           header;
  rr_x2g_resel_imsg_resel_req_t resel_req;
  rr_x2g_resel_imsg_abort_req_t abort_req;
} rr_x2g_resel_imsg_u;

typedef enum
{
  RR_X2G_RESEL_OMSG_RESEL_CNF,
  RR_X2G_RESEL_OMSG_ABORT_CNF,
  RR_X2G_RESEL_OMSG_PANIC_RESET_CNF,
  RR_X2G_RESEL_OMSG_COUNT
} rr_x2g_resel_omsg_e;

typedef struct
{
  rr_message_header_t   header;
  rr_x2g_resel_status_e status;
} rr_x2g_resel_omsg_resel_cnf_t;

typedef struct
{
  rr_message_header_t header;
  boolean             activation_ind_sent;
} rr_x2g_resel_omsg_abort_cnf_t;

typedef struct
{
  rr_message_header_t header;
  boolean             activation_ind_sent;
} rr_x2g_resel_omsg_panic_reset_cnf_t;

typedef union
{
  rr_message_header_t                 header;
  rr_x2g_resel_omsg_resel_cnf_t       resel_cnf;
  rr_x2g_resel_omsg_abort_cnf_t       abort_cnf;
  rr_x2g_resel_omsg_panic_reset_cnf_t panic_reset_cnf;
} rr_x2g_resel_omsg_u;

/* Union of all messages. */
typedef union
{
  rr_x2g_resel_imsg_u imsg;
  rr_x2g_resel_omsg_u omsg;
} rr_x2g_resel_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern void rr_x2g_resel_send_imsg(
  rr_x2g_resel_imsg_e imsg,
  const gas_id_t gas_id
);

extern void rr_x2g_resel_send_imsg_resel_req(
  ARFCN_T                       arfcn,
  uint8                         rxlev,
  BSIC_T                        bsic,
  mm_as_status_T                activation_reason,
  boolean                       any_cell_selection_mode,
  PLMN_id_T                   * requested_plmn,
  rr_x2g_system_information_t * system_information,
  const gas_id_t                gas_id
);

extern void rr_x2g_resel_send_imsg_abort_req(
  rr_l1_gsm_stop_reason_T l1_stop_reason,
  const gas_id_t gas_id
);

extern void rr_x2g_resel_send_omsg_resel_cnf(
  rr_x2g_resel_status_e status,
  const gas_id_t gas_id
);

extern void rr_x2g_resel_send_omsg_abort_cnf(
  boolean activation_ind_sent,
  const gas_id_t gas_id
);

extern void rr_x2g_resel_send_omsg_panic_reset_cnf(
  boolean activation_ind_sent,
  const gas_id_t gas_id
);

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

#endif /* RR_X2G_RESEL_IF_H */

/* EOF */
