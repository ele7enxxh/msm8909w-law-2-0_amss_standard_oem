#ifndef RR_G2X_REDIR_IF_H
#define RR_G2X_REDIR_IF_H

/*============================================================================
  @file rr_g2x_redir_if.h

  Signalling interface for the RR G2X Redirection module.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_g2x_redir_if.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS_G2T_REDIRECTION)

#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"
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
  RR_G2X_REDIR_IMSG_REDIR_REQ,
  RR_G2X_REDIR_IMSG_ABORT_REQ,
  RR_G2X_REDIR_IMSG_L1_DEACTIVATED_IND,
  RR_G2X_REDIR_IMSG_COUNT
} rr_g2x_redir_imsg_e;

typedef struct
{
  rr_message_header_t                    header;
  rr_chan_rel_cell_selection_indicator_t csi;
} rr_g2x_redir_imsg_redir_req_t;

typedef union
{
  rr_message_header_t           header;
  rr_g2x_redir_imsg_redir_req_t redir_req;
} rr_g2x_redir_imsg_u;

typedef enum
{
  RR_G2X_REDIR_OMSG_REDIR_CNF,
  RR_G2X_REDIR_OMSG_ABORT_CNF,
#ifdef FEATURE_SGLTE
  RR_G2X_REDIR_OMSG_SGLTE_ABORT_CNF,
#endif /*FEATURE_SGLTE*/
  RR_G2X_REDIR_OMSG_COUNT
} rr_g2x_redir_omsg_e;

typedef enum
{
  RR_G2X_REDIR_STATUS_UNDEFINED,
  RR_G2X_REDIR_STATUS_SUCCESS,
  RR_G2X_REDIR_STATUS_FAILURE,
#ifdef FEATURE_SGLTE
  RR_G2X_REDIR_STATUS_SGLTE_SUCCESS,
#endif /*FEATURE_SGLTE*/
  RR_G2X_REDIR_STATUS_MAX
} rr_g2x_redir_status_e;

typedef enum
{
  RR_G2X_REDIR_FAILURE_ACTION_UNDEFINED,
  RR_G2X_REDIR_FAILURE_ACTION_OOS,
  RR_G2X_REDIR_FAILURE_ACTION_CELL_SELECTION,
#ifdef FEATURE_SGLTE
  RR_G2X_REDIR_FAILURE_ACTION_SGLTE_RETURN_TO_CAMPED,
#endif /*FEATURE_SGLTE*/
  RR_G2X_REDIR_FAILURE_ACTION_MAX
} rr_g2x_redir_failure_action_e;

typedef struct
{
  rr_message_header_t           header;
  rr_g2x_redir_status_e         status;
  rr_g2x_redir_failure_action_e failure_action;
} rr_g2x_redir_omsg_redir_cnf_t;

typedef union
{
  rr_message_header_t           header;
  rr_g2x_redir_omsg_redir_cnf_t redir_cnf;
} rr_g2x_redir_omsg_u;

/* Union of all messages. */
typedef union
{
  rr_g2x_redir_imsg_u imsg;
  rr_g2x_redir_omsg_u omsg;
} rr_g2x_redir_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern const char * rr_g2x_redir_imsg_name(
  rr_g2x_redir_imsg_e imsg
);

extern const char * rr_g2x_redir_omsg_name(
  rr_g2x_redir_omsg_e omsg
);

extern void rr_g2x_redir_send_imsg(
  rr_g2x_redir_imsg_e imsg,
  const gas_id_t gas_id
);

extern void rr_g2x_redir_send_imsg_redir_req(
  rr_chan_rel_cell_selection_indicator_t * csi,
  const gas_id_t gas_id
);

extern void rr_g2x_redir_send_omsg(
  rr_g2x_redir_omsg_e omsg,
  const gas_id_t gas_id
);

extern void rr_g2x_redir_send_omsg_redir_cnf(
  rr_g2x_redir_status_e status,
  rr_g2x_redir_failure_action_e failure_action,
  const gas_id_t gas_id
);

#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION */

#endif /* RR_G2X_REDIR_IF_H */
