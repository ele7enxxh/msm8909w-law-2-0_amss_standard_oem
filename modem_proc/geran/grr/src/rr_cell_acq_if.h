#ifndef RR_CELL_ACQ_IF_H
#define RR_CELL_ACQ_IF_H

/*============================================================================
  @file rr_cell_acq_if.h

  Signalling interface for the RR cell acquisition module.

                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_cell_acq_if.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"
#include "rr_gprs_defs.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/


/* Define the set of status values that may be indicated when cell acquisition
is completed. */
typedef enum
{
  RR_CELL_ACQ_STATUS_UNDEFINED = -1,

  /* Indicate that all BCCH system information has been acquired.  Note that
  this value implicitly indicates that early camping has not been performed. */
  RR_CELL_ACQ_STATUS_SUCCESS_BCCH,

  /* Indicate that all BCCH system information has been acquired and early
  camping has been performed. */
  RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED,

  /* Only applicable to C2 reselection - surround update of target cell failed. */
  RR_CELL_ACQ_STATUS_FAILURE_SURROUND_UPDATE_FAILED,

  /* Only applicable to C2 reselection - after surround update, target cell was
  no longer stronger than serving cell. */
  RR_CELL_ACQ_STATUS_FAILURE_CELL_NOT_BETTER,

  /* Indicate acquisition failure. */
  RR_CELL_ACQ_STATUS_FAILURE_CELL_BARRED,

  /* The target cell's C1 value is insufficient to allow camping. */
  RR_CELL_ACQ_STATUS_FAILURE_CELL_LOW_C1,

  /* The target cell's PLMN does not match the requested PLMN. */
  RR_CELL_ACQ_STATUS_FAILURE_PLMN_MISMATCH,

  /* The cell is in a forbidden LAI. */
  RR_CELL_ACQ_STATUS_FAILURE_FORBIDDEN_LAI,

  /* The BSIC of the ARFCN does not match that which is expected. */
  RR_CELL_ACQ_STATUS_FAILURE_BSIC_MISMATCH,

  /* L1 failed to sync to the specified ARFCN. */
  RR_CELL_ACQ_STATUS_FAILURE_BCCH_NOT_FOUND,

  /* Acquisition of BCCH system information did not complete before timer expiry. */
  RR_CELL_ACQ_STATUS_FAILURE_BSI_ACQUISITION_FAILURE,

  RR_CELL_ACQ_STATUS_MAX
} rr_cell_acq_status_e;


/* Define the set of actions that the RR cell acquisition state machine may
be requested to perform.  These actions determine how L1 will be moved to 
the target cell, and indicate to the RR cell acquisition state machine what
sub-actions it should perform. */
typedef enum
{
  /* The target cell should be selected using MPH_SELECT_SPECIFIC_BCCH_REQ.
  This would normally be the case if the UE is not camped on a cell at the time
  of the acquisition, e.g. following cell selection or a reselection power scan
  procedure. */
  RR_CELL_ACQ_ACTION_SELECT_BCCH,

  /* The target cell should be selected using MPH_GPRS_RESELECTION_REQ.  This
  is used in the case of immediate reselection - no surround update will be
  performed. */
  RR_CELL_ACQ_ACTION_RESELECT_BCCH,

  /* Reselection has been triggered by receiving a PCCO/PCCC with NACC parameters.
  The target cell should be selected using MPH_GPRS_RESELECTION_REQ.  Surround
  update is not performed prior to reselection.  Early camping is not performed.
  */
  RR_CELL_ACQ_ACTION_RESELECT_BCCH_NACC_PCCO_PCCC,

  /* Reselection has been triggered as a result of a failed PCCO.  The target
  cell should be selected using MPH_GPRS_RESELECTION_REQ.  Surround update is
  not performed prior to reselection.  Early camping will be performed. */
  RR_CELL_ACQ_ACTION_RESELECT_BCCH_PCCO_FAILED,

  /* Better-cell reselection has been triggered.  The RR cell acquisition state
  machine should perform a surround update procedure prior to reselection.
  Early camping is allowed. */
  RR_CELL_ACQ_ACTION_RESELECT_BCCH_AFTER_SU,

  /* Better-cell reselection has been triggered, but cell selection parameter
  for the neighbour cell have been provided by the network (e.g. in PCCO or
  PMO).  A surround update is not necessary. */
  RR_CELL_ACQ_ACTION_RESELECT_BCCH_WITH_GPRS_CELL_SELECTION_PARAMS,

  RR_CELL_ACQ_ACTION_MAX
} rr_cell_acq_action_e;


/* This enumeration defines all of the signal IDs that may be sent to the
rr_cell_acq_control() state machine. */
typedef enum
{
  /* Sent to the RR cell acquisition state machine in order to initiate an
  acquisition of a new cell. */
  RR_CELL_ACQ_IMSG_START_IND,

  /* Sent to the RR cell acquisition state machine in order to abort an
  ongoing acquisition. @see RR_CELL_ACQ_OMSG_ABORT_CNF */
  RR_CELL_ACQ_IMSG_ABORT_REQ,

  /* Sent to the RR cell acquisition state machine to indicate that an SI3
  or SI4 message for a neighbour cell has been received. */
  RR_CELL_ACQ_IMSG_SURROUNDING_SI_RECEIVED_IND,

  /* Sent to the RR cell acquisition state machine to indicate that SI3
  has been received for the pending cell, suggesting that early camping
  may be performed if required. */
  RR_CELL_ACQ_IMSG_EARLY_CAMPING_BSI_RECEIVED_IND,

  /* Indicates to the RR cell acquisition state machine that all mandatory
  BCCH system information (not including SI13) have been received. */
  RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND,

  /* Indicates to the RR cell acquisition state machine that SI13 has been
  received. */
  RR_CELL_ACQ_IMSG_GPRS_BSI_RECEIVED_IND,

  /* Indicates to the RR cell acquisition state machine that all system
  information required for NACC camping has been received.  On a BCCH cell
  this would normally be SI1, SI3 and SI13 and on a PBCCH cell this would be
  PSI1, PSI2 and PSI14. */
  RR_CELL_ACQ_IMSG_NACC_ACQ_COMPLETE_IND,

  /* Signal IDs are cast to uint8 so must not exceed this value. */
  RR_CELL_ACQ_IMSG_MAX = 255
} rr_cell_acq_imsg_e;


typedef struct
{
  rr_message_header_t            header;
  ARFCN_T                        target_cell;
  uint8                          RXLEV_average;
  boolean                        bsic_valid;
  BSIC_T                         bsic;
  boolean                        recent_reselection_flag;
  rr_cell_acq_action_e           action;
  boolean                        sync_flag;
  boolean                        clear_si_database;
  gprs_scell_info_t             *gprs_cell_info;
  boolean                        requested_plmn_valid;
  PLMN_id_T                      requested_plmn;
} rr_cell_acq_imsg_start_ind_t;


typedef union
{
  rr_message_header_t          header;
  rr_cell_acq_imsg_start_ind_t start_ind;
} rr_cell_acq_imsg_u;


/* This enumeration defines all of the signal IDs that may be sent from the
rr_cell_acq_control() state machine. */
typedef enum
{
  /* Sent from RR cell acquisition state machine to indicate that MAC is
  suspended prior to reselection.  This indication may be used by the parent
  state machine to trigger any actions required at the start of the
  reselection process. */
  RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND,

  /* Sent from RR cell acquisition state machine to indicate that L1 has
  successfully moved to the new ARFCN. */
  RR_CELL_ACQ_OMSG_RESELECTED_IND,

  /* Sent from RR cell acquisition state machine to indicate that it has
  completed it's acquisition procedure.  The success or failure of the
  acquisition is indicated in the payload of this signal. 
  @see rr_cell_acq_omsg_finished_ind_t */
  RR_CELL_ACQ_OMSG_FINISHED_IND,

  /* Sent from RR cell acquisition state machine to indicate that is has
  aborted an ongoing acquisition in response to an abort request.
  @see RR_CELL_ACQ_IMSG_ABORT_REQ */
  RR_CELL_ACQ_OMSG_ABORT_CNF,

  /* Signal IDs are cast to uint8 so must not exceed this value. */
  RR_CELL_ACQ_OMSG_MAX = 255
} rr_cell_acq_omsg_e;


typedef struct
{
  rr_message_header_t  header;
  rr_cell_acq_status_e cell_acq_status;
} rr_cell_acq_omsg_finished_ind_t;


typedef union
{
  rr_message_header_t             header;
  rr_cell_acq_omsg_finished_ind_t finished_ind;
} rr_cell_acq_omsg_u;


/* Union of all messages send to and from the RR cell acquisition state
machine. */
typedef union
{
  rr_cell_acq_imsg_u imsg;
  rr_cell_acq_omsg_u omsg;
} rr_cell_acq_msg_u;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/


/**
  @brief Send an RR_CELL_ACQ_IMSG_START_IND to the RR cell acquisition state
         machine.

  @param target_cell:   The ARFCN of the cell to acquire.

  @param RXLEV_average: The RXLEV of the target cell, used to calculate C1 in
  the case of non-immediate reselection.

  @param recent_reselection_flag: Boolean value indicating if a reselection has
  recently occurred.  Used to apply hysteresis when determining cell
  suitability for non-immediate reselection.

  @param rr_cell_acq_cause: Indicates the method of selection or reselection
  that should be used when acquiring the new cell.

  @param sync_flag: Indicates whether the target cell is in the 'sync cell'
  list.  TRUE if it is (but only if the cell acq cause is for a non-immediate
  reselection), FALSE otherwise.

  @param clear_si_database: Flag to indicate whether or not the RR cell
  acquisition state machine should clear the system information database it
  is told to use.
 
  @param gprs_cell_info: Pointer to the gprs system information
  database that the RR cell acquisition database should
  reference when performing it's acquisition.

  @param requested_plmn Pointer to the current requested PLMN.  May be set
         to NULL if not available.
* 
*/
extern void rr_cell_acq_send_imsg_start_ind(
  ARFCN_T                        target_cell,
  uint8                          RXLEV_average,
  BSIC_T                       * bsic,
  boolean                        recent_reselection_flag,
  rr_cell_acq_action_e           action,
  boolean                        sync_flag,
  boolean                        clear_si_database,
  gprs_scell_info_t            * gprs_cell_info,
  PLMN_id_T                    * requested_plmn,
  const gas_id_t                 gas_id
);


/**
  @brief Send an RR_CELL_ACQ_IMSG_ABORT_REQ to the RR cell acquisition state
         machine.
*/
extern void rr_cell_acq_send_imsg_abort_req(const gas_id_t gas_id);


/**
  @brief Send an RR_CELL_ACQ_IMSG_SURROUNDING_SI_RECEIVED_IND to the RR cell
         acquisition state machine.
*/
extern void rr_cell_acq_send_imsg_surrounding_si_received_ind(const gas_id_t gas_id);


/**
  @brief Send an RR_CELL_ACQ_IMSG_EARLY_CAMPING_BSI_RECEIVED_IND to the RR
         cell acquisition state machine.
*/
extern void rr_cell_acq_send_imsg_early_camping_bsi_received_ind(const gas_id_t gas_id);


/**
  @brief Send an RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND to the RR cell
         acquisition state machine.
*/
extern void rr_cell_acq_send_imsg_gsm_bsi_received_ind(const gas_id_t gas_id);


/**
  @brief Send an RR_CELL_ACQ_IMSG_GPRS_BSI_RECEIVED_IND to the RR cell
         acquisition state machine.
*/
extern void rr_cell_acq_send_imsg_gprs_bsi_received_ind(const gas_id_t gas_id);


/**
  @brief Send an RR_CELL_ACQ_IMSG_NACC_ACQ_COMPLETE_IND to the RR cell
         acquisition state machine.
*/
extern void rr_cell_acq_send_imsg_nacc_acq_complete_ind(const gas_id_t gas_id);


/**
  @brief Send an RR_CELL_ACQ_OMSG_MAC_SUSPENDED_IND from the RR cell
         acquisition state machine.
*/
extern void rr_cell_acq_send_omsg_mac_suspended_ind(const gas_id_t gas_id);


/**
  @brief Send an RR_CELL_ACQ_OMSG_RESELECTED_IND from the RR cell acquisition
         state machine.
*/
extern void rr_cell_acq_send_omsg_reselected_ind(const gas_id_t gas_id);


/**
  @brief Send an RR_CELL_ACQ_OMSG_FINISHED_IND from the RR cell acquisition
         state machine.

  @param acq_status: Indicates whether the acquisition was successful or not.
  If not successful, this parameter will indicate the nature of the
  acquisition failure.
*/
extern void rr_cell_acq_send_omsg_finished_ind(rr_cell_acq_status_e acq_status, const gas_id_t gas_id);


/**
  @brief Send an RR_CELL_ACQ_OMSG_ABORT_CNF from the RR cell acquisition state
         machine.
*/
extern void rr_cell_acq_send_omsg_abort_cnf(const gas_id_t gas_id);


#endif /* #ifndef RR_RESEL_IF_H */

/* EOF */

