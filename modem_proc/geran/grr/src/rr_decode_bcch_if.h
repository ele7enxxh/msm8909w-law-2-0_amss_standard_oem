#ifndef RR_DECODE_BCCH_IF_H
#define RR_DECODE_BCCH_IF_H

/*============================================================================
  @file rr_resel_if.h

  Signalling interface for the RR reselection module.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_decode_bcch_if.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"
#include "rr_gprs_defs.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define RR_ARFCN_LIST_SIZE RR_MAX_CANDIDATE_FREQ_LIST

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/* Input messages. */
typedef enum
{
  RR_DECODE_BCCH_IMSG_START_IND,
  RR_DECODE_BCCH_IMSG_ABORT_REQ,
  RR_DECODE_BCCH_IMSG_CANDIDATE_CELL_SI_IND,
  RR_DECODE_BCCH_IMSG_COUNT
} rr_decode_bcch_imsg_e;

typedef struct
{
  rr_message_header_t               header;
  rr_l1_idle_freq_list_T            bcch_freq_list;
  rr_search_mode_T                  bcch_decode_type;
  boolean                           si3_only;
} rr_decode_bcch_start_ind_t;

typedef struct
{
  rr_message_header_t          header;
} rr_decode_bcch_abort_req_t;

typedef struct
{
  rr_message_header_t          header;
} rr_decode_bcch_candidate_cell_si_ind_t;

typedef union
{
  rr_message_header_t                      header;
  rr_decode_bcch_start_ind_t               decode_bcch_start_ind;
  rr_decode_bcch_abort_req_t               decode_bcch_abort_req;
  rr_decode_bcch_candidate_cell_si_ind_t   decode_bcch_candidate_cell_si_ind;
} rr_decode_bcch_imsg_u;

/* Output messages. */
typedef enum
{
  RR_DECODE_BCCH_OMSG_FINISHED_IND,
  RR_DECODE_BCCH_OMSG_ABORT_CNF,
  RR_DECODE_BCCH_OMSG_COUNT
} rr_decode_bcch_omsg_e;

/**
  @brief Structure containing a list of ARFCNs.
*/
typedef struct
{
  uint32  count;
  ARFCN_T entries[RR_ARFCN_LIST_SIZE];
} rr_arfcn_list_t;

typedef struct
{
  rr_message_header_t header;
  rr_arfcn_list_t     detected_frequency_list;
  /*< The list of ARFCNs on which GSM service was identified. */
} rr_decode_bcch_finished_ind_t;

typedef struct
{
  rr_message_header_t          header;
} rr_decode_bcch_abort_cnf_t;

typedef union
{
  rr_message_header_t            header;
  rr_decode_bcch_finished_ind_t  decode_bcch_finished_ind;
  rr_decode_bcch_abort_cnf_t     decode_bcch_abort_cnf;  
} rr_decode_bcch_omsg_u;

/* Union of all messages. */
typedef union
{
  rr_decode_bcch_imsg_u  imsg;
  rr_decode_bcch_omsg_u  omsg;
} rr_decode_bcch_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/* Input messages. */
extern void rr_decode_bcch_send_start_ind(
  rr_decode_bcch_start_ind_t decode_bcch_start_ind,
  const gas_id_t gas_id
);
extern void rr_decode_bcch_send_abort_req(const gas_id_t gas_id);
extern void rr_decode_bcch_send_candidate_cell_si_ind(const gas_id_t gas_id);

/* Output messages. */
extern void rr_decode_bcch_send_finish_ind(
  rr_arfcn_list_t *detected_frequency_list,
  const gas_id_t gas_id
);
extern void rr_decode_bcch_send_abort_cnf(const gas_id_t gas_id);

#endif /* #ifndef RR_DECODE_BCCH_IF_H */

/* EOF */

