#ifndef RR_RESEL_IF_H
#define RR_RESEL_IF_H

/*============================================================================
  @file rr_resel_if.h

  Signalling interface for the RR reselection module.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_resel_if.h#1 $ */

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
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Input Messages
 *--------------------------------------------------------------------------*/


/* Define the set of signals that may be sent to the RR reselection state
machine. */
typedef enum
{
  /* This signal is sent to the RR reselection state machine to indicate that
  it should immediately perform a reselection power scan. */
  RR_RESEL_IMSG_START_RPS_IND,

  /* This signal is sent to the RR reselection state machine to indicate that
  the current serving cell is no longer suitable and a reselection is
  required.  Note that the sending of this signal does not guarantee that a
  reselection has been performed and no action should be taken that assumes
  otherwise. */
  RR_RESEL_IMSG_REQUIRED_IND,

  /* This signal is sent to abort any possibly ongoing reselection.
  @see RR_RESEL_OMSG_ABORT_CNF */
  RR_RESEL_IMSG_ABORT_REQ,

  /* This signal is an indication to the rr_resel_control() state machine 
  that C1 for the serving cell is now Less Than (LT) zero.  This may for
  example be used to abandon a NACC reselection (while waiting for a response
  from the network) in favour of reselecting immediately. */
  RR_RESEL_IMSG_C1_LT_ZERO_IND,

  /* Individual Priorities have been updated.  RR should stop timers for
  reselection algorithm that is no longer required. */
  RR_RESEL_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND,

  /* A change in whether G2L reselection is allowed has occurred.  This also
  require some active timers to be stopped. */
  RR_RESEL_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND,

  /* IRAT System Information has been updated.  The reselection state machine
  should check that any actions currently in progress are still valid, e.g.
  if Priority information has now been received then rank-based reselection
  timers should be cancelled as the reselection algorithm to use has changed. */
  RR_RESEL_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND,

  RR_RESEL_IMSG_STOP_GSM_MODE_REQ_SGLTE_RAT_CHANGE,

  RR_RESEL_IMSG_MAX
} rr_resel_imsg_e;


typedef struct
{
  rr_message_header_t          header;
  ARFCN_T                      serving_cell;
  inter_task_BCCH_allocation_T ba_list;
} rr_resel_imsg_start_rps_ind_t;


/* Define the set of reasons for which an ongoing reselection may be requested
to be aborted. */
typedef enum
{
  /* Indicate that an establishment request for either CS or PS has been
  received.  If the serving cell is currently capable of maintaining service
  then any ongoing reselection will be aborted, otherwise this abort request
  will be ignored. */
  RR_RESEL_ABORT_REASON_MAC_RR_EST,

  /* A request to establish either a CS or PS connection in response to a page
  has been received.  The RR reselection state machine should abort any ongoing
  reselection if it is possible for it to remain in service on the current
  serving cell, otherwise the reselection will go ahead and the abort request
  will be ignored. */
  RR_RESEL_ABORT_REASON_PAGE_RESPONSE,

  /* A request from MM has been received and must be actioned immediately.  Any
  ongoing reselection will be aborted.*/
  RR_RESEL_ABORT_REASON_MM_REQ,
  
  /* This is an indication to the RR reselection state machine that NC2 mode
  has been entered.  Any ongoing NACC procedure must be aborted. */
  RR_RESEL_ABORT_REASON_NACC_NC2,

  RR_RESEL_ABORT_REASON_MAX
} rr_resel_abort_reason_e;


typedef struct
{
  rr_message_header_t     header;
  rr_resel_abort_reason_e reason;
} rr_resel_imsg_abort_req_t;


/* Define the set of reselection types that may be specified when the RR
reselection state machine is being asked to perform a reselection. */
typedef enum
{
  /* Request that the RR reselection state machine perform a reselection power
  scan. */
  RR_RESEL_CAUSE_RPS,

  /* Request that the RR reselection state machine perform a normal reselection,
  based on it's current neighbour cells. */
  RR_RESEL_CAUSE_RESELECTION,
  RR_RESEL_CAUSE_RESELECTION_TRM_DENIAL,

  RR_RESEL_CAUSE_MAX
} rr_resel_cause_e;


typedef struct
{
  rr_message_header_t header;
  rr_resel_cause_e    cause;
} rr_resel_imsg_required_ind_t;


typedef union
{
  rr_message_header_t           header;
  rr_resel_imsg_start_rps_ind_t start_rps_ind;
  rr_resel_imsg_required_ind_t  required_ind;
  rr_resel_imsg_abort_req_t     abort_req;
} rr_resel_imsg_u;


/*----------------------------------------------------------------------------
 * Output Messages
 *--------------------------------------------------------------------------*/


/* Define the set of signals that may be sent from the RR reselection state
machine. */
typedef enum
{
  /* Indicates that the RR reselection state machine has started to perform
  a reselection. */
  RR_RESEL_OMSG_STARTED_IND,

  /* Indicates that the RR reselection state machine has finished performing
  a reselection.  The success or failure of the reselection is indicated in the
  payload of this message. */
  RR_RESEL_OMSG_FINISHED_IND,

  /* Sent in response to RR_RESEL_IMSG_ABORT_REQ when a reselection has been
  aborted. */
  RR_RESEL_OMSG_ABORT_CNF,

  /* Sent by the RR reselection state machine to indicate to it's parent that
  MAC has been returned to idle mode.  This signal is mainly intended to help
  synchronise modules sitting above the RR reselection module.  It is unlikely
  that anything more than resumption of PS services in MAC will be required
  on receipt of this signal. */
  RR_RESEL_OMSG_MAC_IDLE_IND,

  /* Signal IDs are cast to uint8 so must not exceed this value. */
  RR_RESEL_OMSG_MAX
} rr_resel_omsg_e;


/* Define the types of reselection that may be indicated in
RR_RESEL_STARTED_IND. */
typedef enum
{
  /* A normal reselection has been started. */
  RR_RESEL_TYPE_NORMAL,

  /* A G2W reselection has been started. */
  RR_RESEL_TYPE_G2W,

  RR_RESEL_TYPE_MAX
} rr_resel_type_e;


typedef struct
{
  rr_message_header_t header;
  rr_resel_type_e     type;
} rr_resel_omsg_started_ind_t;


/* Define the set of status indications that may be included in an
RR_RESEL_OMSG_FINISHED_IND message. */
typedef enum
{
  /* Indicates to the receiver that the RR reselection state machine has
  completed it's activity and the UE is in service. */
  RR_RESEL_STATUS_IN_SERVICE,

  /* Indicates to the receiver that the RR reselection state machine has
  completed it's activity and the UE is not in service. */
  RR_RESEL_STATUS_OUT_OF_SERVICE,

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  /* Indicates to the receiver that the RR reselection state machine has
  performed a G2W reselection successfully. */
  RR_RESEL_STATUS_G2W_RESELECTION_SUCCESS,

  /* Indicates to the receiver that the RR reselection state machine has
  failed a G2W reselection. */

  RR_RESEL_STATUS_G2W_RESELECTION_FAILURE,

  /* Indicates to the receiver that the RR reselection state machine has
  performed a G2W PCCO successfully. */
  RR_RESEL_STATUS_G2W_PCCO_SUCCESS,
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  RR_RESEL_STATUS_MAX
} rr_resel_status_e;


typedef struct
{
  rr_message_header_t header;
  rr_resel_status_e   status;
} rr_resel_omsg_finished_ind_t;


typedef union
{
  rr_message_header_t          header;
  rr_resel_omsg_finished_ind_t finished_ind;
  rr_resel_omsg_started_ind_t  started_ind;
} rr_resel_omsg_u;


/* Union of all messages. */
typedef union
{
  rr_resel_imsg_u imsg;
  rr_resel_omsg_u omsg;
} rr_resel_msg_u;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern const char * rr_resel_imsg_name(const rr_resel_imsg_e imsg);

extern const char * rr_resel_omsg_name(const rr_resel_omsg_e omsg);

/**
  @brief Sends an RR_RESEL_IMSG_START_RPS_IND to the RR reselection state
         machine.

  @param serving_cell:  The ARFCN of the current serving cell.

  @param ba_list: The BA list on which the reselection power scan should be
  performed.
*/
extern void rr_resel_send_start_rps_ind(
  ARFCN_T                              serving_cell,
  const inter_task_BCCH_allocation_T * ba_list,
  const gas_id_t gas_id
);


/**
  @brief Sends an RR_RESEL_IMSG_REQUIRED_IND to the RR reselection state
         machine.

  @param cause: This parameter indicates the type of reselection that
  is required.  Currently this may specify either a reselection power scan
  or a normal reselection.
*/
extern void rr_resel_send_imsg_required_ind(
  rr_resel_cause_e cause,
  const gas_id_t gas_id
);


/**
  @brief Sends an RR_RESEL_IMSG_ABORT_REQ to the RR reselection state machine.

  @param reason: Specifies the reason for the reselection to be aborted.
  
  @see RR_RESEL_OMSG_ABORT_CNF
*/
extern void rr_resel_send_imsg_abort_req(
  rr_resel_abort_reason_e reason,
  const gas_id_t gas_id
);

/**
  @brief Sends a message without payload belonging to the RR_RESEL_IMSG message
  set to RR.
*/
extern void rr_resel_send_imsg(
  rr_resel_imsg_e imsg,
  const gas_id_t gas_id
);

/**
  @brief Sends an RR_RESEL_OMSG_RESEL_FINISHED_IND from the RR reselection
         state machine.

  @param status: Provides an indication of the status of the reselection,
  allowing the receiver to determine if the reselection were successful or not.
*/
extern void rr_resel_send_omsg_finished_ind(
  rr_resel_status_e status,
  const gas_id_t gas_id
);


/**
  @brief Sends an RR_RESEL_OMSG_STARTED_IND from the RR reselection state machine.

  @param type: Indicates to the receiver the type of reselection started.
*/
extern void rr_resel_send_omsg_started_ind(
  rr_resel_type_e type,
  const gas_id_t gas_id
);


/**
  @brief Sends an RR_RESEL_OMSG_ABORT_CNF from the RR reselection state machine.
*/
extern void rr_resel_send_omsg_abort_cnf(
  const gas_id_t gas_id
);


/**
  @brief Sends an RR_RESEL_OMSG_MAC_IDLE_IND from the RR reselection state machine.
*/
extern void rr_resel_send_omsg_mac_idle_ind(
  const gas_id_t gas_id
);

#endif /* #ifndef RR_RESEL_IF_H */

/* EOF */

