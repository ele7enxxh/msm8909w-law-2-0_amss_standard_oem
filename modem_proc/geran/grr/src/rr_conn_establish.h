#ifndef RR_CONN_ESTABLISH_H
#define RR_CONN_ESTABLISH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             rr_conn_establish.h

GENERAL DESCRIPTION
   This module contains functions for dealing with rr connection establishment.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_conn_establish.h_v   1.1   08 Jul 2002 19:26:28   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_conn_establish.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
13-02-14    sjw     Defer RR_PS_ABORT_REQ if first RACH response is still pending
03-07-28    tjw     GPRS congestion indication
08/11/02    rmc     Added prototype for rr_is_page_for_mobile

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_task.h"
#include "rr_events.h"
#include "rr_ce_paging.h"
#include "geran_multi_sim.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#define RACH_TIMEOUT_SCALING_FACTOR 10

  #define IS_ENHANCED_GPRS_ACCESS_REASON(r) (((r) == GMAC_EGPRS_ONE_PHASE_ACCESS) || ((r) == GMAC_EGPRS_TWO_PHASE_ACCESS) || ((r) == GMAC_EGPRS_SIGNALLING_ACCESS))
  #define IS_STANDARD_GPRS_ACCESS_REASON(r) (((r) == GMAC_ONE_PHASE_ACCESS) || ((r) == GMAC_TWO_PHASE_ACCESS) || ((r) == GMAC_SINGLE_BLOCK_ACCESS))
  #define IS_GPRS_ACCESS_REASON(r) (IS_ENHANCED_GPRS_ACCESS_REASON(r) || IS_STANDARD_GPRS_ACCESS_REASON(r))

#define RR_CHECK_MANDATORY_IE(ptr,evt) {\
if ((ptr) == NULL)\
  return((evt));\
if ((ptr)->start_IE == NULL)\
  return((evt));\
}

#define RR_CHECK_OPTIONAL_IE(ptr,evt) {\
if ((ptr) == NULL)\
  return((evt));\
}

typedef struct
{
  boolean before_params_valid;
  boolean starting_time_valid;
  word    starting_time;
  byte    timing_advance;
} rr_ia_params_T;

typedef struct
{
  byte    t3122_rejected_cause;
  ARFCN_T t3122_serving_cell;
  BSIC_T  t3122_serving_bsic;
  boolean  nas_t3122_running;
  boolean  nas_update_required;
} t3122_info_t;

typedef enum
{
  RR_T3122_ENDED_BY_EXPIRY,
  RR_T3122_ENDED_BY_ASSIGNMENT,
  RR_T3122_ENDED_BY_CELL_CHANGE,
  RR_T3122_ENDED_BY_MODE_CHANGE
} rr_t3122_ended_T;

typedef struct
{
  /* current and previous states for the main RRCE state machine */
  rr_con_est_state_T con_est_state;
  rr_con_est_state_T old_con_est_state;

  /**
   * Number of channel_request random access messages that have been sent
   * to the network as part of the current RR connection establishment
   * attempt.
   */
  byte channel_request_counter;

  /**
   * Number of Immediate assignemnet Rej that have been sent
   * by the network as part of the current RR connection establishment
   * attempt.
   */
  byte immd_assign_rej_counter;
  
  /* Number of times the RACH request has been blocked by lower layers */
  byte failed_rach_count;

  /**
   * Holds the length in bits of the random portion of the
   * random access reference value sent in random access messages.
   * Determined based on the establishment cause and SI parameters
   */
  byte random_reference_length;

  /**
   * Holds the random access reference value used in the last
   * random access message sent to the network.
   */
  byte pending_access_reference;

  /**
   * the reason the MS is accessing the network (taken from RR_EST_REQ or
   * set to ANSWER_TO_PAGING if processing a page request)
   */
  byte access_reason;

  /**
   * Set to TRUE when the access_reason (see above) is set to MM_RR_EMERGENCY_CALL
   * This is required in case of RLF and the call is re-established, then the
   * access_reason will be MM_RR_CALL_REEST, but this flag will remain set in that
   * case
   */
  boolean emergency_call;

  /**
   * The identity used to page the mobile. Used when sending the page response
   * message
   */
  byte paged_identity[RR_MAX_IDENTITY_SIZE];

  /**
   * Stores the type of channel that is needed for an RR connection established
   * in response to a page request message (affects the establishment cause)
   */
  byte channel_needed;

  /**
   * Holds the request references used in the last 3 random access messages sent
   * to the network. Used to match ImmediateAssignment messages with the request
   * sent by mobile.
   */
  request_references_T request_references[RR_REQ_REF_ARRAY_SIZE];

  /**
   * number of times that the ImmediateAssignment procedure has been repeated
   * due to a contention resolution information field mismatch indication from
   * layer 2 in response to DL_ESTABLISH_REQ
   */
  int procedure_counter;

  /**
   * indicates whether the IA procedure should be repeated or not
   */
  boolean repeat_assignment_procedure;

  /**
   * Reason why the connection establishment is being released
   */
  byte release_reason;

  /**
   * The size in bytes of the layer 3 message sent by the MM in the
   * RR_EST_REQ message
   */
  byte cm_service_request_msg_length;

  /**
   * The contents of the L3 message sent by MM in RR_EST_REQ
   */
  byte service_request_message[N201_MAX];

  /**
   * used to determine which part of a two part msg assignment is being processed
   */
  boolean waiting_for_2nd_msg;

  /**
   * used in the EGPRS Packet Channel Request Content
   */
  byte egprs_radio_priority;

  /**
   * Holds the random access reference value used in the last
   * EGPRS Packet Channel Request message sent to the network.
   */
  word egprs_pending_access_reference;


  /**
   * Indicates if the current access is for EGPRS.
   */
  boolean egprs_access_valid;

  rr_pkt_est_cause_t pkt_est_cause;

  t3122_info_t t3122_info;

  /* This field is to store CCO status (both X2G CCO and PCCO). Normally to record failure reason of CCO procedure */
  rr_cco_status_e x2g_cco_status;

#ifdef FEATURE_LTE
  /* TRUE if a connection is being established in order to perform
  a periodic LAU, FALSE otherwise. */
  boolean is_periodic_lau;
#endif /* FEATURE_LTE */

  /* Used in some cases where different events are required to be returned after PS access blocked */
  rr_event_T return_rr_event;

  boolean starting_idle;

  boolean pcco_in_progress;

  boolean requested_by_nas;
  boolean confirmed_to_nas;
  boolean originated_on_other_rat;
  boolean t200_expired;

} rr_conn_est_info_T;

void rr_initialise_connection_establishment(const gas_id_t gas_id);

rr_event_T rr_connection_establishment_control(
  rr_event_T con_est_event,
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
);
word rr_compute_rach_timeout_value(system_information_database_T *);
void rr_ce_store_cs_page_info(byte *identity, byte channel_needed, const gas_id_t gas_id);
#ifdef FEATURE_GSM_EDTM
void rr_ce_edtm_set_paged_identity(byte *id_ptr, const gas_id_t gas_id);
#endif // FEATURE_GSM_EDTM
void rr_ce_set_access_reason(byte access_reason, const gas_id_t gas_id);
byte rr_ce_access_reason(const gas_id_t gas_id);
void rr_reset_rach_references(const gas_id_t gas_id);
boolean rr_ce_for_gprs_access(const gas_id_t gas_id);
boolean rr_ce_for_emergency_call(const gas_id_t gas_id);
rr_event_T rr_ce_process_est_req(rr_est_req_T *, byte *, const gas_id_t gas_id);
byte rr_get_rr_est_req_L3MsgSize(rr_est_req_T *messagePtr);
void rr_ce_set_pkt_est_cause( rr_pkt_est_cause_t pkt_est_cause, const gas_id_t gas_id);
rr_pkt_est_cause_t rr_ce_get_pkt_est_cause(const gas_id_t gas_id);
void rr_ce_set_egprs_radio_priority(uint8 egprs_radio_priority, const gas_id_t gas_id);
byte rr_ce_egprs_radio_priority(const gas_id_t gas_id);
boolean rr_ce_egprs_access_valid(const gas_id_t gas_id);
void rr_ce_reset_egprs_access(const gas_id_t gas_id);
boolean rr_ce_allow_cs_connection(byte new_est_cause, const gas_id_t gas_id);
byte    rr_ce_generate_access_reference(byte, byte *, const gas_id_t gas_id);
void    rr_cs_access_rejected_on_cell(byte access_reason, const gas_id_t gas_id);
void rr_stop_t3122(const gas_id_t gas_id);
void rr_start_t3122(byte t3122_timeout_secs, byte access_reason, const gas_id_t gas_id);
boolean rr_get_t3122_running_for_nas(const gas_id_t gas_id);
void rr_check_nas_t3122_status(const gas_id_t gas_id);
rr_event_T rr_respond_to_immediate_assignment(
  ie_descriptor_T       *,
  channel_information_T *,
  channel_information_T *,
  rr_ia_params_T        *,
  boolean,
  uint8*,
  const gas_id_t gas_id
);
#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
void rr_ce_set_edtm_access_parameters(byte, const gas_id_t gas_id);
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

boolean rr_match_IA_request_reference(
  request_references_T  *request_references_array,
  byte                  *request_reference_IE,
  int                   channel_requests_sent
);

boolean rr_match_IA_extended_request_reference(
  request_references_T  *request_references_array,
  int                   channel_requests_sent,
  word                  extended_ra,
  word                  frame_number
);

#ifdef FEATURE_LTE
extern boolean rr_ce_is_periodic_lau(const gas_id_t gas_id);
#endif /* FEATURE_LTE */

/**
  @brief Checks whether Connection Establishment state machine is in a state
         suitable for processing NAS requested.

  This function will return FALSE if the connection establishment state machine
  is waiting for an abort to complete (i.e. waiting for L1 to start idle, etc.)
  and otherwise will return TRUE.  This allows the processing of further NAS
  requests to be held off until AS procedures are complete.
*/
extern boolean rr_ce_can_process_mm_message(const gas_id_t gas_id);
#ifdef FEATURE_DUAL_SIM
extern boolean rr_ce_can_process_ps_abort(const gas_id_t gas_id);
#endif /* FEATURE_DUAL_SIM */

extern boolean rr_ce_can_process_rr_grr_gprs_ia_ind(const gas_id_t gas_id);

extern void rr_ce_init_conn_info(
  boolean requested_by_nas,
  const gas_id_t gas_id
);

extern void rr_ce_init_conn_info_after_x2g_ho(const gas_id_t gas_id);

extern boolean rr_ce_conn_is_requested_by_nas(const gas_id_t gas_id);

extern boolean rr_ce_should_confirm_conn_rel_to_nas(const gas_id_t gas_id);

extern boolean rr_ce_conn_originated_on_other_rat(const gas_id_t gas_id);

extern void rr_ce_init_access_reason(const gas_id_t gas_id);

#endif /* RR_CONN_ESTABLISH_H */

/* EOF */

