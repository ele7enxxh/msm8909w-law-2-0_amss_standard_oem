#ifndef RR_SELECT_BCCH_IF_H
#define RR_SELECT_BCCH_IF_H

/*============================================================================
  rr_select_bcch_if.h

  Signalling interface for the RR-SELECT-BCCH module.
  See comments below for more detail.

                Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_select_bcch_if.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $ */

/** 
  @file rr_select_bcch_if.h
  @brief This header file specifies the RR IMSG/OMSG signalling interface for
         the RR-SELECT-BCCH module
    
  Also see the associated module header file, rr_select_bcch.h. This contains
  other external functions which are required for the module to function
  correctly, such as the initialisation function main state machine message
  input function. \n \n
    
  External functions are provided to generate the RR-SELECT-BCCH IMSGs that this
  module understands, and the RR-SELECT-BCCH OMSGs to be processed by the parent
  module.
 */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"
#include "rr_candidate_cell_db.h"
#include "geran_multi_sim.h"
#include "rr_lai.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** 
  @enum rr_select_bcch_imsg_e
  @brief The RR-SELECT-BCCH IMSGs which can be sent to rr_select_bcch_control

  RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ
  @see rr_select_bcch_imsg_pscan_select_req_t
   
  RR_SELECT_BCCH_IMSG_PSCAN_LIST_SELECT_REQ
  @see rr_select_bcch_imsg_pscan_list_select_req_t
 
  RR_SELECT_BCCH_IMSG_SELECT_REQ
  @see rr_select_bcch_imsg_select_req_t
   
  RR_SELECT_BCCH_IMSG_ACQUIRE_SI_REQ
  @see rr_select_bcch_imsg_acquire_si_req_t
   
  RR_SELECT_BCCH_IMSG_BSI_RECEIVED_IND
  @see rr_select_bcch_imsg_bsi_received_ind_t
   
  RR_SELECT_BCCH_IMSG_ABORT_REQ
  @see rr_select_bcch_imsg_abort_req_t
 */
typedef enum
{
  RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ,
  RR_SELECT_BCCH_IMSG_PSCAN_LIST_SELECT_REQ,
  RR_SELECT_BCCH_IMSG_SELECT_REQ,
  RR_SELECT_BCCH_IMSG_ACQUIRE_SI_REQ,
  RR_SELECT_BCCH_IMSG_BSI_RECEIVED_IND,
  RR_SELECT_BCCH_IMSG_ABORT_REQ,
  RR_SELECT_BCCH_IMSG_COUNT
} rr_select_bcch_imsg_e;

/** 
  @enum rr_select_bcch_early_camp_e
  @brief The available early-camp methods
 
  RR_SELECT_BCCH_EARLY_CAMP_NONE       : No early-camping required.
  RR_SELECT_BCCH_EARLY_CAMP_SI3        : Eary-camp when SI3 received.
  RR_SELECT_BCCH_EARLY_CAMP_USE_PARAMS : Eary-camp using the supplied parameters.
 */
typedef enum
{
  RR_SELECT_BCCH_EARLY_CAMP_NONE,
  RR_SELECT_BCCH_EARLY_CAMP_SI3,
  RR_SELECT_BCCH_EARLY_CAMP_USE_PARAMS
} rr_select_bcch_early_camp_e;

typedef struct
{
  uint8   bs_ag_blks_res;
  uint8   bs_cc_chans;
  uint8   bs_ccch_sdcch_comb;
  uint8   bs_pa_mfrms;
  boolean imsi_valid;
  uint16  imsi_mod_1000;
  LAI_T   lai;
  uint16  ci;
  uint16  si_messages_on_bcch_ext;
  uint16  si_messages_not_broadcast;
} rr_select_bcch_init_params_t;

/**
  @typedef rr_cell_suitable_cb_fn_t
  @brief Call-back function to check cell suitability.
 
  This is called when SI3 is received and should return TRUE if the cell is
  suitable, FALSE otherwise
 */
typedef boolean (*rr_cell_suitable_cb_fn_t)(rr_candidate_cell_T *, const gas_id_t gas_id);

/** 
  @struct rr_select_bcch_imsg_pscan_select_req_t
  @brief Definition of the RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ message
  @see rr_select_bcch_send_imsg_pscan_select_req   
 */ 
typedef struct
{
  rr_message_header_t                      header;
  ARFCN_T                                  arfcn;
  rr_cell_suitable_cb_fn_t                 cell_suitable_cb_fn;
  uint16                                   notify_on_si_mask;
  uint16                                   wait_for_si_mask;
} rr_select_bcch_imsg_pscan_select_req_t;

/** 
  @struct rr_select_bcch_imsg_select_from_list_req_t
  @brief Definition of the RR_SELECT_BCCH_IMSG_PSCAN_LIST_SELECT_REQ message
  @see rr_select_bcch_send_imsg_select_from_list_req
 */ 
typedef struct
{
  rr_message_header_t                      header;
  ARFCN_T                                 *cell_list;
  uint16                                   num_of_cells;
  boolean                                  decode_si3_only;
  rr_cell_suitable_cb_fn_t                 cell_suitable_cb_fn;
  uint16                                   notify_on_si_mask;
  uint16                                   wait_for_si_mask;
  uint32                                   max_allowed_time;
} rr_select_bcch_imsg_pscan_list_select_req_t;

/** 
  @struct rr_select_bcch_imsg_select_req_t
  @brief Definition of the RR_SELECT_BCCH_IMSG_SELECT_REQ message
  @see rr_select_bcch_send_imsg_select_req   
 */ 
typedef struct
{
  rr_message_header_t                      header;
  ARFCN_T                                  arfcn;
  byte                                     rxlev_average;
  rr_cell_suitable_cb_fn_t                 cell_suitable_cb_fn;
  boolean                                  early_camp_required;
  uint16                                   notify_on_si_mask;
  uint16                                   wait_for_si_mask;
  BSIC_T                                   bsic;
} rr_select_bcch_imsg_select_req_t;

/** 
  @struct rr_select_bcch_imsg_acquire_si_req_t
  @brief Definition of the RR_SELECT_BCCH_IMSG_ACQUIRE_SI_REQ message
  @see rr_select_bcch_send_imsg_acquire_si_req   
 */ 
typedef struct
{
  rr_message_header_t                      header;
  ARFCN_T                                  arfcn;
  BSIC_T                                   bsic;
  byte                                     rxlev_average;
  rr_cell_suitable_cb_fn_t                 cell_suitable_cb_fn;
  rr_select_bcch_early_camp_e              early_camp_required;
  boolean                                  init_params_valid;
  rr_select_bcch_init_params_t             init_params;
  uint16                                   notify_on_si_mask;
  uint16                                   wait_for_si_mask;
  uint32                                   max_allowed_time_ms;
} rr_select_bcch_imsg_acquire_si_req_t;

/** 
  @struct rr_select_bcch_imsg_bsi_received_ind_t
  @brief Definition of the RR_SELECT_BCCH_IMSG_BSI_RECEIVED_IND message
  @see rr_select_bcch_send_imsg_bsi_received_ind   
 */ 
typedef struct
{
  rr_message_header_t                      header;
  rr_si_type_t                             si_received;
} rr_select_bcch_imsg_bsi_received_ind_t;

/** 
  @struct rr_select_bcch_imsg_abort_req_t 
  @brief Definition of the RR_SELECT_BCCH_IMSG_ABORT_REQ message 
 */ 
typedef struct
{
  rr_message_header_t                      header;
} rr_select_bcch_imsg_abort_req_t;

/** 
  @union rr_select_bcch_imsg_u 
  @brief Union of all the RR-SELECT-BCCH IMSGs which can be sent to 
         rr_select_bcch_control 
 */ 
typedef union
{
  rr_message_header_t                         header;
  rr_select_bcch_imsg_pscan_select_req_t      rr_select_bcch_imsg_pscan_select_req;
  rr_select_bcch_imsg_pscan_list_select_req_t rr_select_bcch_imsg_pscan_list_select_req;
  rr_select_bcch_imsg_select_req_t            rr_select_bcch_imsg_select_req;
  rr_select_bcch_imsg_acquire_si_req_t        rr_select_bcch_imsg_acquire_si_req;
  rr_select_bcch_imsg_bsi_received_ind_t      rr_select_bcch_imsg_bsi_received_ind;
  rr_select_bcch_imsg_abort_req_t             rr_select_bcch_imsg_abort_req;
} rr_select_bcch_imsg_u;

/** 
  @enum rr_select_bcch_omsg_e
  @brief The RR-SELECT-BCCH OMSGs which can be sent from rr_select_bcch_control
 
  RR_SELECT_BCCH_OMSG_SELECT_CNF
  @see rr_select_bcch_omsg_select_cnf_t   
   
  RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND
  @see rr_select_bcch_omsg_select_fail_ind_t   
   
  RR_SELECT_BCCH_OMSG_SI_NOTIFY_IND
  @see rr_select_bcch_omsg_si_notify_ind_t   
 */
typedef enum
{
  RR_SELECT_BCCH_OMSG_SELECT_CNF,
  RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND,
  RR_SELECT_BCCH_OMSG_SI_NOTIFY_IND,
  RR_SELECT_BCCH_OMSG_COUNT
} rr_select_bcch_omsg_e;

/** 
  @struct rr_select_bcch_omsg_select_cnf_t
  @brief Definition of the RR_SELECT_BCCH_OMSG_SELECT_CNF message.
  @see rr_select_bcch_send_omsg_select_cnf   
 */ 
typedef struct
{
  rr_message_header_t                      header;
  ARFCN_T                                  arfcn;
  BSIC_T                                   bsic;
  boolean                                  early_camped;
  uint16                                   si_received_mask;
} rr_select_bcch_omsg_select_cnf_t;

/** 
  @enum rr_select_bcch_select_fail_reason_e
  @brief Failure reason given in RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND message.
 
  RR_SELECT_BCCH_FAIL_REASON_NONE (internal use)
  RR_SELECT_BCCH_FAIL_REASON_CELL_NOT_FOUND
  RR_SELECT_BCCH_FAIL_REASON_CELL_NOT_SUITABLE
  RR_SELECT_BCCH_FAIL_REASON_TIMEOUT
  RR_SELECT_BCCH_FAIL_REASON_ABORTED
 */
typedef enum
{
  RR_SELECT_BCCH_FAIL_REASON_NONE,
  RR_SELECT_BCCH_FAIL_REASON_CELL_NOT_FOUND,
  RR_SELECT_BCCH_FAIL_REASON_CELL_NOT_SUITABLE,
  RR_SELECT_BCCH_FAIL_REASON_TIMEOUT,
  RR_SELECT_BCCH_FAIL_REASON_ABORTED
} rr_select_bcch_fail_reason_e;

/** 
  @struct rr_select_bcch_omsg_select_fail_ind_t 
  @brief Definition of the RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND message. 
  @see rr_select_bcch_send_omsg_select_fail_ind   
 */ 
typedef struct
{
  rr_message_header_t                      header;
  ARFCN_T                                  arfcn;
  rr_select_bcch_fail_reason_e             reason;
  boolean                                  early_camped;
} rr_select_bcch_omsg_select_fail_ind_t;

/** 
  @struct rr_select_bcch_omsg_si_notify_ind_t 
  @brief Definition of the RR_SELECT_BCCH_OMSG_SI_NOTIFY_IND message. 
  @see rr_select_bcch_send_omsg_si_notify_ind   
 */ 
typedef struct
{
  rr_message_header_t                      header;
  ARFCN_T                                  arfcn;
  BSIC_T                                   bsic;
} rr_select_bcch_omsg_si_notify_ind_t;

/** 
  @union rr_select_bcch_omsg_u 
  @brief Union of all the RR-SELECT-BCCH OMSGs which can be output by 
         rr_select_bcch_control 
 */ 
typedef union
{
  rr_message_header_t                      header;
  rr_select_bcch_omsg_select_cnf_t         rr_select_bcch_omsg_select_cnf;
  rr_select_bcch_omsg_select_fail_ind_t    rr_select_bcch_omsg_select_fail_ind;
  rr_select_bcch_omsg_si_notify_ind_t      rr_select_bcch_omsg_si_notify_ind;
} rr_select_bcch_omsg_u;

/** 
  @union rr_select_bcch_msg_u 
  @brief Union of all the RR-SELECT-BCCH IMSGs and OMSGs 
 */ 
typedef union
{
  /** Input Messages */
  rr_select_bcch_imsg_u                    imsg;

  /** Output Messages */
  rr_select_bcch_omsg_u                    omsg;
} rr_select_bcch_msg_u;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/** 
  @brief Sends RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ
 
  This message instructs the RR-SELECT-BCCH module to perform a reselection
  power-scan to verify the presence of a cell, select it, and acquire the
  indicated SI messages.
   
  @param arfcn
  @param cell_suitable_cb_fn  Call-back function to be called when SI3 is
                              received. If this call-back returns FALSE, then
                              the cell is deemed not suitable and a fail
                              indication will be output.
                              Set to NULL if not required.
  @see rr_cell_suitable_cb_fn_t 
  @param notify_on_si_mask    When the indicated SIs have been received, a
                              notification indication is sent out (but this does
                              not stop the procedure).
                              Set to MASK_SI_NONE if not required.
  @param wait_for_si_mask     When the indicated SIs have been received, the
                              procedure completes. This can be set to
                              MASK_SI_NONE, but if early camping based on SI3 is
                              required, then this is internally modified to add
                              SI3.
 */
extern void rr_select_bcch_send_imsg_pscan_select_req(
  ARFCN_T arfcn,
  rr_cell_suitable_cb_fn_t cell_suitable_cb_fn,
  uint16 notify_on_si_mask,
  uint16 wait_for_si_mask,
  const gas_id_t gas_id
);


/** 
  @brief Sends RR_SELECT_BCCH_IMSG_PSCAN_LIST_SELECT_REQ
 
  This message instructs the RR-SELECT-BCCH module to perform a reselection
  power-scan over a list of supplied ARFCNs, calling the supplied cell suitability 
  call-back function to determine which cell to pick. 
  After a suitable cell is found, it is selected and the indicated SIs aquired.
   
  @param cell_list 
  @param num_of_cells 
  @param decode_si3_only      If set to TRUE, only SI3 will be decoded during the 
                              parallel list decode 
  @param cell_suitable_cb_fn  Call-back function to be called when SI3/SI4 is
                              received. If this call-back returns FALSE, then
                              the cell is deemed not suitable and the next cell in
                              the list is tried. If at the end of the list, a fail
                              indication will be output.
  @see rr_cell_suitable_cb_fn_t 
  @param notify_on_si_mask    When the indicated SIs have been received, a
                              notification indication is sent out (but this does
                              not stop the procedure).
                              Set to MASK_SI_NONE if not required.
  @param wait_for_si_mask     When the indicated SIs have been received, the
                              procedure completes.
                              Note: This can be set to MASK_SI_NONE, but it is always
                              internally modified to add SI3.
  @param max_allowed_time     Time-out value (ms)
 */
extern void rr_select_bcch_send_imsg_pscan_list_select_req(
  ARFCN_T *cell_list,
  uint16 num_of_cells,
  boolean decode_si3_only,
  rr_cell_suitable_cb_fn_t cell_suitable_cb_fn,
  uint16 notify_on_si_mask,
  uint16 wait_for_si_mask,
  uint32 max_allowed_time,
  const gas_id_t gas_id
);


/** 
  @brief Sends RR_SELECT_BCCH_IMSG_SELECT_REQ 
   
  This message instructs the RR-SELECT-BCCH module to select the specified 
  cell, and acquire the indicated SI messages. 
  Note: It is assumed that L1 is not tuned to the cell, and a 
  MPH_SELECT_SPECIFIC_BCCH_REQ will be sent to L1 to do this. If L1 is already 
  tuned to the cell, then use RR_SELECT_BCCH_IMSG_ACQUIRE_SI_REQ instead. 
  
  @param arfcn 
  @param rxlev_average 
  @param cell_suitable_cb_fn  Call-back function to be called when SI3 is 
                              received. If this call-back returns FALSE, then
                              the cell is deemed not suitable and a fail
                              indication will be output. 
                              Set to NULL if not required.
  @see rr_cell_suitable_cb_fn_t 
  @param early_camp_required  Whether early camping is required 
  @param notify_on_si_mask    When the indicated SIs have been received, a 
                              notification indication is sent out (but this
                              does not stop the procedure).
                              Set to NULL if not required.
  @param wait_for_si_mask     When the indicated SIs have been received, the 
                              procedure completes. This can be set to
                              MASK_SI_NONE, but if early camping based on SI3
                              is required, then this is internally modified to
                              add SI3
 */
extern void rr_select_bcch_send_imsg_select_req(
  ARFCN_T arfcn,
  byte rxlev_average,
  rr_cell_suitable_cb_fn_t cell_suitable_cb_fn,
  boolean early_camp_required,
  uint16 notify_on_si_mask,
  uint16 wait_for_si_mask,
  BSIC_T BSIC,
  const gas_id_t gas_id
);


/** 
  @brief Sends RR_SELECT_BCCH_IMSG_ACQUIRE_SI_REQ 
   
  This message instructs the RR-SELECT-BCCH module to acquire the indicated SI 
  messages of the already selected cell. 
  Note: It is assumed that L1 is already tuned to the cell (i.e. L1 has just 
  left dedicated state, or a MPH_SELECT_SPECIFIC_BCCH_REQ has previously been 
  sent) and SIs are being reaed. If this is not the case, then use 
  RR_SELECT_BCCH_IMSG_SELECT_REQ instead. 
  
  @param arfcn 
  @param rxlev_average 
  @param bsic                 BSIC of the selected cell 
  @param cell_suitable_cb_fn  Call-back function to be called when SI3 is 
                              received. If this call-back returns FALSE, then
                              the cell is deemed not suitable and a fail
                              indication will be output. 
                              Set to NULL if not required.
  @see rr_cell_suitable_cb_fn_t 
  @param early_camp_required  Whether early camping is required 
  @param notify_on_si_mask    When the indicated SIs have been received, a 
                              notification indication is sent out (but this
                              does not stop the procedure).
                              Set to NULL if not required.
  @param wait_for_si_mask     When the indicated SIs have been received, the 
                              procedure completes. This can be set to
                              MASK_SI_NONE, but if early camping based on SI3
                              is required, then this is internally modified to
                              add SI3
  @param init_params_ptr      Initial paramters (optional).
                              Set to NULL if not required.
  @param max_allowed_time_ms  Maximum allowed time (in millisecs) for the acquisition. Set to 0 if not 
                              required (i.e. the caller will run a timer)
 */
extern void rr_select_bcch_send_imsg_acquire_si_req(
  ARFCN_T arfcn,
  byte rxlev_average,
  BSIC_T bsic,
  rr_cell_suitable_cb_fn_t cell_suitable_cb_fn,
  rr_select_bcch_early_camp_e early_camp_required,
  uint16 notify_on_si_mask,
  uint16 wait_for_si_mask,
  rr_select_bcch_init_params_t *init_params_ptr,
  uint32 max_allowed_time_ms,
  const gas_id_t gas_id
);


/** 
  @brief Sends RR_SELECT_BCCH_IMSG_ABORT_REQ. Called to abort the current 
         procedure
 */
extern void rr_select_bcch_send_imsg_abort_req(
  const gas_id_t gas_id
);


/** 
  @brief Called to inform RR-SELECT-BCCH module that the indicated SI message 
  has been received (and the contents decoded and stored) 
  
  @param si_received 
 */
extern void rr_select_bcch_send_imsg_bsi_received_ind(
  rr_si_type_t si_received,
  const gas_id_t gas_id
);


/** 
  @brief Call to send RR_SELECT_BCCH_OMSG_SELECT_CNF 
   
  The message indicates that the requested cell has been successfully 
  selected, and specified SIs have been acquired 
  
  @param arfcn 
  @param bsic 
  @param early_camped       TRUE is L1 is Early Camped, FALSE otherwise
  @param si_received_mask   Which SIs were acquired 
 */
extern void rr_select_bcch_send_omsg_select_cnf(
  ARFCN_T                      arfcn,
  BSIC_T                       bsic,
  boolean                      early_camped,
  uint16                       si_received_mask,
  const gas_id_t               gas_id
);


/** 
  @brief Sends RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND 
   
  This message indicates that the requested cell could not be selected, or that 
  the specified SIs could not be acquired 
   
  @param arfcn 
  @param early_camped       TRUE is L1 is Early Camped, FALSE otherwise
  @param reason 
 */
extern void rr_select_bcch_send_omsg_select_fail_ind(
  ARFCN_T arfcn,
  boolean early_camped,
  rr_select_bcch_fail_reason_e reason,
  const gas_id_t gas_id
);


/** 
  @brief Sends RR_SELECT_BCCH_IMSG_SI_NOTIFY_IND 
   
  This message indicates that the SIs specified in notify_si_mask in the 
  original request have been acquired 
   
  @param arfcn 
  @param bsic
 */
extern void rr_select_bcch_send_omsg_si_notify_ind(
  ARFCN_T arfcn,
  BSIC_T bsic,
  const gas_id_t gas_id
);


#endif // RR_SELECT_BCCH_IF_H

/* EOF */

