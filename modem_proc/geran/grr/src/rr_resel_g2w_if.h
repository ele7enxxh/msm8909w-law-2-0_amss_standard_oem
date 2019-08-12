#ifndef RR_RESEL_G2W_IF_H
#define RR_RESEL_G2W_IF_H

/** @file */

/*============================================================================
  @file rr_resel_g2w_if.h

  Signalling interface for the rr_resel_g2w module.

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_resel_g2w_if.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"
#include "rr_g2w.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** This enumeration defines all of the signal IDs that may be sent to the
rr_resel_g2w_control() state machine. */
typedef enum
{
  RR_RESEL_G2W_IMSG_RESEL_REQ,
  RR_RESEL_G2W_IMSG_L1_DEACTIVATED_IND,
  RR_RESEL_G2W_IMSG_L1_ACTIVATED_IND,
  RR_RESEL_G2W_IMSG_ABORT_IND,
  RR_RESEL_G2W_IMSG_STOP_GSM_MODE_REQ_SGLTE_RAT_CHANGE,
  RR_RESEL_G2W_IMSG_COUNT
} rr_resel_g2w_imsg_e;

/**
 * Define the types of G2W reselection that are possible.
 */
typedef enum
{
  RR_G2X_RESEL_TYPE_RESELECTION,
  RR_G2X_RESEL_TYPE_PACKET_CELL_CHANGE_ORDER
} rr_g2x_resel_type_e;

/**
 * @brief This structure defines the RR_RESEL_G2W_START_REQ signal.
 *
 * The RR_RESEL_G2W_START_REQ signal is sent to rr_resel_g2w_control()
 * in order to initiate a reselection or packet cell change order procedure
 * from the current serving GSM cell to a target WCDMA cell.
 */
typedef struct
{
  rr_message_header_t               header;
  rr_g2x_resel_type_e               type;
  rr_irat_cell_t                    target;
  sys_network_selection_mode_e_type network_selection_mode;
  PLMN_id_T                         serving_cell_plmn_id;
  uint16                            trk_lo_adj;
} rr_resel_g2w_imsg_resel_req_t;

typedef enum
{
  RR_RESEL_G2W_ABORT_CAUSE_UNDEFINED,
  RR_RESEL_G2W_ABORT_CAUSE_RR_EST_REQ,
  RR_RESEL_G2W_ABORT_CAUSE_MM_REQ,
  RR_RESEL_G2W_ABORT_CAUSE_MAX
} rr_resel_g2w_abort_cause_e;

/**
  @brief This structure defines the RR_RESEL_G2W_ABORT_IND signal.

  The RR_RESEL_G2W_ABORT_IND signal is sent to rr_resel_g2w_control()
  in order to terminate a reselection or packet cell change order procedure.

  Depending on the abort cause, this signal may be actioned or ignored by the
  rr_resel_g2w_control() state machine.
*/
typedef struct
{
  rr_message_header_t        header;
  rr_resel_g2w_abort_cause_e abort_cause;
} rr_resel_g2w_imsg_abort_ind_t;

/**
 * @brief Union of all input messages.
 *
 * This is a union of all messages that may be sent to the 
 */
typedef union
{
  rr_message_header_t           header;
  rr_resel_g2w_imsg_resel_req_t resel_req;
  rr_resel_g2w_imsg_abort_ind_t abort_ind;
} rr_resel_g2w_imsg_u;

/** Enumeration of all output signal IDs. */
typedef enum
{
  RR_RESEL_G2W_OMSG_RESEL_CNF,
  RR_RESEL_G2W_OMSG_COUNT
} rr_resel_g2w_omsg_e;

typedef enum
{
  RR_RESEL_G2W_STATUS_UNDEFINED,

  /* A G2W reselection procedure completed successfully. */
  RR_RESEL_G2W_STATUS_RESELECTION_SUCCESS,

  /* A G2W reselection procedure has failed (reject received from RRC) */
  RR_RESEL_G2W_STATUS_RESELECTION_FAILURE,

  /* A G2W PCCO procedure has completed successfully. */
  RR_RESEL_G2W_STATUS_PCCO_SUCCESS,

  /* A G2W PCCO procedure has failed, 'FAILURE_NO_RESPONSE' should be sent in PCCF. */
  RR_RESEL_G2W_STATUS_PCCO_FAILURE_NO_RESPONSE,

  /* A G2W PCCO procedure has failed, 'ASSIGNMENT_REJECT' should be sent in PCCF. */
  RR_RESEL_G2W_STATUS_PCCO_FAILURE_ASSIGNMENT_REJECT,

  /* A G2W procedure (Reselection or PCCO) has been aborted, RR/L1 are still camped (although MAC may be suspended). */
  RR_RESEL_G2W_STATUS_ABORTED_IN_SERVICE,

  /* A G2W procedure (Reselection or PCCO) has been aborted, RR/L1 are not camped (although L1 is activated). */
  RR_RESEL_G2W_STATUS_ABORTED_OUT_OF_SERVICE,

  RR_RESEL_G2W_STATUS_MAX
} rr_resel_g2w_status_e;

typedef struct
{
  rr_message_header_t   header;
  rr_resel_g2w_status_e status;
} rr_resel_g2w_omsg_resel_cnf_t;

/** Union of all output messages. */
typedef union
{
  rr_message_header_t           header;
  rr_resel_g2w_omsg_resel_cnf_t resel_cnf;
} rr_resel_g2w_omsg_u;

/** Union of all input and output messages belonging to this interface. */
typedef union
{
  /** Input Messages */
  rr_resel_g2w_imsg_u imsg;

  /** Output Messages */
  rr_resel_g2w_omsg_u omsg;
} rr_resel_g2w_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Send a message of type RR_RESEL_G2W_IMSG_RESEL_REQ to the
         rr_resel_g2w_control() state machine.

  @param target Details of the reselection target.
  @param type Indication of the type of reselection to be performed.
  @param network_selection_mode The current network selection mode that the
  UE is operating in.
  @param serving_cell_plmn_id The PLMN ID of the current serving GSM cell.
*/
void rr_resel_g2w_send_imsg_resel_req(
  rr_irat_cell_t                    target,
  rr_g2x_resel_type_e               type,
  sys_network_selection_mode_e_type network_selection_mode,
  PLMN_id_T                         serving_cell_plmn_id,
  uint16                            trk_lo_adj,
  const gas_id_t                    gas_id
);

/**
  @brief Send a message of type RR_RESEL_G2W_IMSG_L1_DEACTIVATED_IND to the
         rr_resel_g2w_control() state machine
*/
void rr_resel_g2w_send_imsg_l1_deactivated_ind(
  const gas_id_t gas_id
);

/**
  @brief Send a message of type RR_RESEL_G2W_IMSG_L1_ACTIVATED_IND to the
         rr_resel_g2w_control() state machine.
*/
void rr_resel_g2w_send_imsg_l1_activated_ind(
  const gas_id_t gas_id
);

/**
  @brief Send a message of type RR_RESEL_G2W_IMSG_ABORT_REQ to the
         rr_resel_g2w_control() state machine.
*/
void rr_resel_g2w_send_imsg_abort_ind(
  rr_resel_g2w_abort_cause_e abort_cause,
  const gas_id_t gas_id
);

/**
  @brief Send a message of type RR_RESEL_G2W_OMSG_RESEL_CNF from the
         rr_resel_g2w_control() state machine.
*/
void rr_resel_g2w_send_omsg_resel_cnf(
  rr_resel_g2w_status_e status,
  const gas_id_t gas_id
);

/**
  @brief Sends a message without payload belonging to the RR_RESEL_G2W_IMSG
         message set to RR.
*/
extern void rr_resel_g2w_send_imsg(
  rr_resel_g2w_imsg_e imsg,
  const gas_id_t gas_id
);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#endif /* RR_RESEL_G2W_IF_H */

/* EOF */

