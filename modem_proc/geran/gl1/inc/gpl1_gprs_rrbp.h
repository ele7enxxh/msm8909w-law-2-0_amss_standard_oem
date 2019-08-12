#ifndef GPL1_RRBP_H
#define GPL1_RRBP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            L1 GPRS RRBP module

GENERAL DESCRIPTION
   This module contains functions for receive and transmit RRBP messages

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---     ----------------------------------------------------------
02/03/16   sn      CR981034 Handling Packet TBF release of DL while wait for PTSR re-configuration
25/02/15   nm      CR796233 Delete RRBP if its TN is falling outside the TX window of multislot class
12/05/14   sk      CR767085 - For allowing G2T TA when clashing with EPDAN with CR & FAI set to 1
03/10/14   aga     CR732455 Changes required to tune away framework for TH 1.0 single sim build 
21/02/14   nk      CR611257 Handling for rrbp msg_type PCA_UL_FAI_MSG while adjusting starting time 
30/05/14   ws      CR657674 Data + MMS Phase 1
29/07/13   sk      CR519663 Partial QBTA bringup changes
21/06/13   cs      Major Triton TSTS Syncup
20/05/13   am      CR476324 TBF release and RRBP received in GTA gap
24/05/12   ab      CR 324793:MS sending REL_CNF immediately without sending RRBP 
                   for uplink (PTR startime comes before RRBP ). RRBP ignored when TS changes
26/03/12   jj      CR344695 rename feature name FEATURE_GSM_GPRS_MSC34 to FEATURE_GSM_GPRS_MSC33 
29/01/11   ky      CR272959:In UL+DL TBF, Adjusting the pending dl start time such
                   that PCA_UL_FAI rrbp message goes out before gl1 sends connect 
                   ind to gmac
24/11/08   og      FEATURE_GSM_GPRS_MSC33 GPL1 changes.
26/01/07   og      Implement UL ctrl msg deletion in L1.
30/06/06   og      Changes to handle high polling rate.
26/01/06   og      Neighbour cell acquisition changes to handle extended dynamic.
23/11/05   og      Update tx call back routines to provide more information.
06/09/05   og      Correctly handle Tra/Trb clashes.
05/09/05   og      Multislot class 12 support.
08/08/05   og      Restructuring PL1 before addition of DTM.
06/07/05   av      CR68673 - PCA was not sent in response to a polled PDA. PL1 was going to sleep.
02/06/05   og      Removal of header-header includes.
18/03/05   og      Fix to PDAN vs TBF suspension clash handling.
03/08/04   ws      Corrected typo
03/08/04   rm      Replaced num_dl_ab_msgs with num_ab_dl_msgs
02/08/04   rm      Reinserted missing data.
30/07/04   rm      Removed #define macros
13/05/04   rm      Changed structure alignment to reduce padding
28/04/04   rm      Initial version.
===========================================================================*/

/* Avoid including header files */

/* The RRBP queue size is calculated based on 4 timeslots polling
 * per block period, every block period and and a transmission
 *  turnaround of 26 frames.
 */
#include "geran_variation.h"
#ifdef  FEATURE_GSM_GPRS_MSC34

/* This assumes 5 timeslot polling per radio block */
#define SIZE_OF_RRBP_BUFF         (25)

#else

#define SIZE_OF_RRBP_BUFF         20

#endif

#define GPL1_INVALID_FN           L1_GPRS_INVALID_FN
#define GPL1_INVALID_TN           GPL1_INVALID
#define GPL1_FULL_RRBP_BUFF       (SIZE_OF_RRBP_BUFF+1)

/* Due to the interface design I need to add a Fn for pending Polled */
/* Assignments. The RRBP's starting time is written over and now the */
/* sleep and DRX code cause the PCA to be missed due to early sleep. */
extern uint32 gpl1_idle_pca_start_frame_number; 

typedef struct
{
  pcr_format_t  rrbp_burst_type;
  uint32        frame_num;
  uint8         timeslot;
  rrbp_msg_t    rrbp_msg_type;
#ifdef PL1_FW_SIM
  uint8         rrbp_msg[SIZE_OF_CONTROL_RADIO_BLOCK + 1];
#else
  uint8         rrbp_msg[SIZE_OF_CONTROL_RADIO_BLOCK];
#endif
  boolean       msg_valid;
  boolean       received_in_idle;
} rrbp_params_T;


typedef struct
{
  uint8 *rrbp_txd_buff;
  gl1_defs_tn_type timeslot;
  gl1_defs_coding_type cs;
} rrbp_txd_param_T;


typedef struct
{
  uint8 idx;
  rrbp_txd_param_T rrbp_txd_buff[MAX_NUM_UL_PACCH_MSGS];
} rrbp_txd_T;

typedef struct{
uint8                       num_msgs_txd;
uint8                       num_rrbp_msgs;
uint8                       num_ul_msgs;
uint8                       num_dl_msgs;
uint8                       num_ab_dl_msgs;
rrbp_msg_t                  last_msg_type;
rrbp_txd_T                  txd_info;
rrbp_params_T               params[SIZE_OF_RRBP_BUFF];
} gpl1_gprs_rrbp_info_T;


boolean l1_prach_rrbp_start_of_block(uint8 frame_no);

uint32 gpl1_gprs_get_ul_rrbp_msg_fn(gas_id_t gas_id);

uint32 gpl1_gprs_get_dl_ab_rrbp_msg_fn(gas_id_t gas_id);


void gpl1_gprs_idle_add_rrbp_msg(
  mac_l1_pacch_rrbp_t *message_ptr
  );

void gpl1_gprs_prach_add_rrbp_msg(
  mac_l1_pacch_rrbp_t *message_ptr
  );

void gpl1_gprs_transfer_add_rrbp_msg(
  mac_l1_pacch_rrbp_t *message_ptr
  );

rrbp_msg_t  gpl1_gprs_single_tx_rrbp(
  uint8 rrbp_to_tx,
  frequency_information_T *freq_info_ptr, gas_id_t gas_id
  );

rrbp_msg_t  gpl1_gprs_transfer_tx_rrbp(
  uint8 rrbp_to_tx, gas_id_t gas_id
  );

boolean gpl1_gprs_prach_rrbp_start_of_block(
  uint8 frame_no
  );

void gpl1_gprs_send_l1_mac_rrbp_failure_ind(
  boolean pdan_crd_flag, gas_id_t gas_id
  );

void gpl1_gprs_idle_rrbp_tx_callback(
	gl1_msg_tx_scheduled_data_type  msgs_txd[ GL1_DEFS_MAX_UL_MSGS ],
	uint8                           num_msg_txd,
  gas_id_t                        gas_id
	);
void gpl1_gprs_idle_rrbp_discard_callback(
	gl1_msg_tx_discarded_data_type  msgs_discarded[ GL1_DEFS_MAX_UL_MSGS ],
	uint8                           num_msg_txd
	);

void gpl1_gprs_prach_rrbp_discard_callback(
  gl1_msg_tx_discarded_data_type  msgs_discarded[ GL1_DEFS_MAX_UL_MSGS ], 
  uint8                           num_msg_txd,
  gas_id_t                        gas_id
  );

void gpl1_gprs_prach_rrbp_tx_callback(
  gl1_msg_tx_scheduled_data_type  msgs_txd[ GL1_DEFS_MAX_UL_MSGS ], 
  uint8                           num_msg_txd,
  gas_id_t                        gas_id
  );

boolean gpl1_gprs_transfer_find_rrbp(
  uint32 fn_max,
  uint32 *starting_time, 
  gas_id_t gas_id
  );

boolean gpl1_gprs_transfer_find_rrbp_type(
  uint32 *starting_time,
  gas_id_t gas_id
  );

uint8 gpl1_gprs_get_matching_rrbp(
  uint32 fn_min, 
  uint32 fn_max,
  uint8  tn_num,
  gas_id_t gas_id
  );

uint8 gpl1_gprs_get_matching_rrbp_msg_type(
  rrbp_msg_t rrbp_msg_type, gas_id_t gas_id
  );

uint8 gpl1_gprs_get_mathching_rrbp_index(
  gas_id_t gas_id
  );
  
void gpl1_gprs_calc_num_ab_dl_rrbp(
  gas_id_t gas_id
  );

uint8 gpl1_gprs_get_num_rrbp_msg(
  gas_id_t gas_id
  );

uint8 gpl1_gprs_get_dl_num_rrbp_msg(
  gas_id_t gas_id
  );

uint8 gpl1_gprs_get_dl_ab_num_rrbp_msg(
  gas_id_t gas_id
  );

uint8 gpl1_gprs_get_ul_num_rrbp_msg(
  gas_id_t gas_id
  );

uint8 gpl1_gprs_get_num_rrbp_msg_txd(
  gas_id_t gas_id
  );

void  gpl1_gprs_reset_num_rrbp_msg_txd( gas_id_t gas_id );

uint32 gpl1_gprs_get_rrbp_frame(
  uint8 rrbp_index, gas_id_t gas_id
  );

rrbp_msg_t gpl1_gprs_get_rrbp_msg_type(
  uint8 rrbp_index, gas_id_t gas_id
  );

uint32 gpl1_gprs_check_for_rrbp_msg_FN(
     rrbp_msg_t *rrbp_msg_type, pcr_format_t* rrbp_burst_type, uint8 *timeslot, gas_id_t gas_id
     );

uint8 gpl1_gprs_check_for_rrbp_msg(
  boolean valid_msg, gas_id_t gas_id
  );


gl1_defs_coding_type gpl1_gprs_set_rrbp_coding_scheme(
  uint8 rrbp_to_tx, gas_id_t gas_id
  );

void gpl1_gprs_check_for_rrbp_duplicates(
  gas_id_t gas_id
  );

void gpl1_gprs_delete_rrbp_entry(
  uint8 rrbp_idx, gas_id_t gas_id
  );

void gpl1_gprs_delete_dl_rrbp_entries(
  gas_id_t gas_id
  );

void gpl1_gprs_reset_rrbp_enteries(
  gas_id_t gas_id
  );

void gpl1_gprs_reset_rrbp_msgs_txd(
  gas_id_t gas_id
  );

void  gpl1_gprs_clean_rrbp_enteries(
  uint32    current_fn, 
  uint8     num_rx_pdch,
  uint8*    tra_trb_cancel_ts_alloc_ptr,
  uint8*    l1_cancel_ts_alloc_ptr, gas_id_t gas_id
  );

void gpl1_gprs_clean_rrbp_txd_info(
  uint8                           num_msg_txd, 
  gl1_msg_tx_scheduled_data_type  msgs_txd[ GL1_DEFS_MAX_UL_MSGS ],
  gas_id_t gas_id
  );

boolean gpl1_gprs_rrbp_to_abort_tbf_suspension(
  boolean tdscdma_client,
  uint32 fn_max,
  gas_id_t gas_id
  );

void gpl1_gprs_delete_oldest_frame_num(
  uint32 current_fn, gas_id_t gas_id
  );

void gpl1_gprs_delete_out_of_tx_window_rrbp(
  uint32 current_fn, gas_id_t gas_id
  );

boolean  gpl1_gprs_find_rrbp_allows_TA
         ( boolean tdscdma_client,
           uint32  rrbp_msg_t,
           gas_id_t gas_id);

boolean  gpl1_gprs_find_rrbp_during_suspension
         ( boolean tdscdma_client,
           uint32  start_fn,
           uint32  end_fn,
           gas_id_t gas_id );

#ifdef  FEATURE_GSM_DTM
#ifdef  FEATURE_GSM_EDTM

/* This function searches for the supplied msg type and
 * deletes it from the rrbp msg queue.
 */
void  gpl1_rrbp_delete_msg( uint8  csn_msg_type, gas_id_t gas_id );

#endif
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_G2X_TUNEAWAY
boolean gpl1_expired_rrbp_msg( uint32 current_fn , gas_id_t gas_id);
boolean gpl1_expired_rrbp_msg_in_gta( gas_id_t gas_id );
void gpl1_set_expired_rrbp_msg_in_gta( boolean expired, gas_id_t gas_id );
#endif /* FEATURE_G2X_TUNEAWAY */

#endif /* GPL1_RRBP_H */
