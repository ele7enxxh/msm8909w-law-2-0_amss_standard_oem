/*****************************************************************************
***
*** TITLE
***
***  HEADER FILE FOR GPRS MAC UTILITY FUNCTIONS
***
***
*** DESCRIPTION
***
***  This header file supports that MAC utility file
***
***
*** EXTERNALIZED FUNCTIONS
***
***  None identified
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  None identified
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacutil.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 09/11/10   sjw     Updated for Dual SIM support
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 10-03-30   tjw     Explicitly include dependent headers to make SU API files
***                    stand alone
*** 04/07/01   tlx     Created
***
*****************************************************************************/

#ifndef INC_MACUTIL_H
#define INC_MACUTIL_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gmactransfer_if.h" /*ftm_egprs_ber.c forces us to have nested includes*/
#include "csn_apt.h"
#include "gprsdef.h"
#include "gprsdef_g.h"

#include "geran_eng_mode_info.h"
#include "rr_public_store.h"
#include "gmacdef.h"
#include "geran_multi_sim.h"

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/
#define MAX_NUMBER_CELL_DATA_INSTANCES 32
#define NO_INSTANCE_REQUIRED  0xFF
#define INVALID_SEGMENT       999

#define SI_MSG_TYPE_OCTET     0        /* offset into a L3 message */
#define SI_MSG_TYPE_MASK      0xFF     /* significant bits in that octet */
#define SI_MSG_TYPE_SHIFT     0        /* number of bits to right shift */

#define PSI_MSG_TYPE_OCTET    0        /* offset into a RLC/MAC ctrl message */
#define PSI_MSG_TYPE_MASK     0xFC     /* significant bits in that octet */
#define PSI_MSG_TYPE_SHIFT    2        /* number of bits to right shift */
#define PSI_INSTANCE_OFFSET   1        /* offset from msg type to instance fields */

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/
typedef  enum
{
 MAC_INITIATED,
 GRR_INITIATED
}gmac_no_access_initiator_t;

typedef enum
{
  UNBLOCKED,
  BLOCKED
}
gmac_rlc_blocked_t;

typedef enum
{
  FAIL_RESULT,
  PASS_RESULT
} gmac_result_t;

typedef enum
{
 PCCF,
 PPS,
 PSS,
 PMR,
 PCCN,
 PCR,
 PRR
} ul_chan_msg_t;

typedef enum
{
 DL_ONLY_PRESENT,
 UL_PRESENT
} existing_tbf_type_t;

typedef enum
{
  NO_PCCF_BEING_SENT,
    SENDING_VIA_SB,
    SENDING_VIA_UL_PACCH
} gmac_pccf_state_t;

#ifdef FEATURE_GPRS_PS_HANDOVER
typedef enum
{
  PSHO_UPDATE_PARAMS_REQUIRED,
  PSHO_UPDATE_PARAMS_NOT_REQUIRED
} gmac_psho_update_params_reqd_t;

#endif /*FEATURE_GPRS_PS_HANDOVER*/

typedef struct
{
  uint8  msg_type;
  uint8  msg_inst;
  uint16 segment;
} msg_type_t;

typedef struct
{
  byte       *data_ptr;           /* dynamically allocated array of octets */
  uint16      num_octets;         /* number of octets in this array */
  uint16     *segment_end_points; /* dynamically allocated array of segment end point offsets */
  uint16      num_segments;       /* number of entries in this array */
  msg_type_t *current_msgs;       /* dynamically allocated array of available msg_types */
  msg_type_t *previous_msgs;      /* dynamically allocated array of previous msg_types */
  uint16      num_previous;       /* number of previously available msgs */
  uint16      num_current;        /* number of currently available msgs */
} container_t;

typedef struct
{
  container_repetition_struct_t *instance_ptrs[MAX_NUMBER_CELL_DATA_INSTANCES];   /* indexed by CONTAINER_INDEX */
  uint32                         instances_rcvd;
  container_t                    si_container;
  container_t                    psi_container;
  boolean                        container_valid;
  ARFCN_T                        arfcn;
} cell_data_t;

typedef struct
{
  cell_data_t cell_data;
  ARFCN_T     ncell_arfcn;
  uint8       ncell_bsic;
} ncell_data_t;

typedef struct
{
  /* frequency parameters - checks new list agains list in use for differences */
  boolean identical_freq_list;
  uint8 nw_bands[MAX_ACCESS_TECH_TYPE];
  uint8 nw_bands_count;
  uint8 csn_encoded_rac_prr_msg[23]; /*used to store the csn encoded PRR msg sent as part of the RAC process*/
  uint8 csn_encoded_rac_arac_msg[23]; /*used to store the csn encoded ARAC msg sent as part of the RAC process*/
  rrps_ms_ra_cap_ie_t prr_ie;
  rrps_ms_ra_cap_ie_t arac_ie;

  cell_data_t scell_info;  /* cell data for the serving cell */

  /* PRIVATE DATA FOR NACC */
  ncell_data_t ncell_info[MAX_NUMBER_OF_NACC_NCELLS]; /* indexed by CONTAINER_ID */

  gmac_rlc_blocked_t gmac_rlc_blocked_flag;
  gmac_pcco_state_t gmac_pcco_state;
  gmac_pccf_state_t gmac_pccf_state;
  uint8 gmac_current_page_mode;
} gmac_util_data_t;

/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Function Prototypes
***
*****************************************************************************/


#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/

/*===========================================================================
===
===  FUNCTION      reset_tbf_parameters()
===
===  DESCRIPTION
===  Resets the current_tbf_mode for EGPRS and the current_test_mode for both
===  EGPRS and GPRS.  This function should be called after all
===  TBF's have been released.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===  void
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void reset_tbf_parameters( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      gmac_util_send_grr_reset_cnf()
===
===  DESCRIPTION
===  Send MAC_GRR_RESET_CNF to GRR to indicate that MAC has completed its
===  reset and is ready to recveive a camp on cell.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_grr_reset_cnf( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      gmac_send_data_ind_to_grr
===
===  DESCRIPTION
===  Send data block to GRR with signal MAC_GRR_DATA_IND
===
===  DEPENDENCIES
===  csn_decoder_result - global decoded CSN.1 message
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_send_data_ind_to_grr(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION   gmac_util_send_ul_reset_req_to_rlc()
===
===  DESCRIPTION
===  Send MR_UL_RESET_REQ to RLC.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_ul_reset_req_to_rlc( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION   gmac_util_send_dl_reset_req_to_rlc()
===
===  DESCRIPTION
===  Send MR_DL_RESET_REQ to RLC.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_dl_reset_req_to_rlc( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      gmac_util_send_ul_release_to_rlc
===
===  DESCRIPTION
===  Send access reject to RLC with signal MR_UL_RELEASE_IND.
===  The signal contains a cause value.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_ul_release_to_rlc(rm_tbf_release_t rel_cause,
                                      gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_send_dl_ass_to_rlc
===
===  DESCRIPTION
===  Send DL assignment indication to RLC with signal MR_DL_ASS_IND.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_dl_ass_to_rlc(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_power_control_params_ie
===
===  DESCRIPTION
===  Handles power control parameters IE
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_power_control_params_ie(pwr_ctrl_paramtrs_t *pwr_ctrl_in, 
                                  power_control_params_T *pwr_ctrl,
                                  gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_packet_ta_ie
===
===  DESCRIPTION
===  Handles packet timing advance IE
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_packet_ta_ie(pkt_timing_adv_t *pkt_ta_in, 
                          timing_advance_params_T *ta_params,
                          gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_handle_power_ctrl_TA_IE
===
===  DESCRIPTION
===  The data received in PACKET TIMING ADVANCE IE is copied
===  into the global timing advance parameter structure for uplink TA.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_util_handle_power_ctrl_TA_IE(pkt_timing_adv_t pkt_ta_in, 
                                          global_packet_ta_T *ta_params,
                                          gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_read_acknack_desc_ie
===
===  DESCRIPTION
===  Handles decoding ack/nack description IE received in PUAN
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_read_acknack_desc_ie(ack_nack_desc_t acknack_data_in, 
                               rm_acknack_desc_t *acknack_desc_ie,
                               gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_compile_acknack_desc_ie
===
===  DESCRIPTION
===  Compile and encode ACK/NACK description IE for PDAN message
===
===  DEPENDENCIES
===
===  ;
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_compile_acknack_desc_ie(ack_nack_desc_t *acknack_ie, 
                                  rm_acknack_desc_t acknack_desc_in,
                                  gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_global_power_ctrl_params_ie
===
===  DESCRIPTION
===  Handles power control parameters IE
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_global_power_ctrl_params_ie(global_pwr_ctrl_t *gl_pwr_ctrl_in, 
                                      rrps_global_power_ctrl_params_t *global_pwr_ctrl,
                                      gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_global_packet_ta_ie
===
===  DESCRIPTION
===  Handles power control parameters IE
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_global_packet_ta_ie(global_pkt_timing_t *gl_pkt_ta_in, 
                              global_packet_ta_T *global_packet_ta,
                              gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_ul_dummy_blk_to_l1
===
===  DESCRIPTION
===  Handles sending MAC_L1_DUMMY_CTRL_BLOCK to L1.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_ul_dummy_blk_to_l1(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_global_tfi
===
===  DESCRIPTION
===  Checks global TFI value against TFI values for UL or DL TBF.
===
===  DEPENDENCIES
===  dl_tfi_valid is an indication that a DL TBF is in progress
===  ul_tfi_valid is an indication that a UL TBF is in progress
===
===  RETURN VALUE
===  - TRUE if TFI match, otherwise FALSE;
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_util_global_tfi(global_tfi_t rxd_global_tfi,
                             gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_update_local_ta_params
===
===  DESCRIPTION
===  MAC stores TIMING ADVANCE parameters locally. Compare recently
===  received TA parameters with local store. If changed then update
===  local store.
===
===  DEPENDENCIES
===  gmac_local_l1_ta_params - local store for TA parameters
===
===  RETURN VALUE
===  - TRUE if parameters updated
===  - FALSE if no change
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_update_local_ta_params(pkt_timing_adv_t *pkt_ta_param_ptr,
                                    gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_utils_calculate_nts
===
===  DESCRIPTION
===  Calculate the number of allocated timeslots from the given
===  timeslot allocation.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  num_ts - number of calculated timeslots
===
===  SIDE EFFECTS
===
===
===========================================================================*/
uint8 gmac_utils_calculate_nts(uint8 ts_allocm,gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_tbf_starting_ie
===
===  DESCRIPTION
===  ref 4.60 (chapter 12.21)
===
===  DEPENDENCIES
===  Calculated the starting time according to relative or absolute frame
===  number encoding.
===
===  - tbf_exist flag: If a TBF currently exists then if the starting time
===  has passed then this is not an error (as it is if TBF does not already
===  exist), hence the input flag.
===
===  RETURN VALUE
===  - TRUE  if staring time is error free
===  - FALSE if starting time calculation has failed
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_util_tbf_starting_ie(start_frame_number_t *start_time_ptr, 
                                  uint32 *des_signal_data_ptr, 
                                  boolean tbf_exist,
                                  gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_freq_param_ie
===
===  DESCRIPTION
===  Checks the frequency parameters received in an assignment message and returns
===  the appropriate value associated with the validity of the frequency assignment
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  One of the following error causes:
===  - NO_ERRORS
===  - FREQ_ERROR_ACCESS_RETRY
===  - FREQ_ERROR_PARTIAL_SYS_ACQ
===
===  SIDE EFFECTS
===
===
===========================================================================*/
uint8 gmac_util_freq_param_ie(freq_params_t *csn_freq_param_ptr, 
                              frequency_information_T *ml_freq_config_info_ptr,
                              gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_global_dynamic_ts_ie()
===
===  DESCRIPTION
===
===  loads MAC_L1_DYNAMIC_TBF_CONFIG with
===  dl power control parmas (po and pr_mode )
===  timeslot assignment
===  USF alloc,
===  usf granularity
===  ul power control params alpha and gamma where applicable.
===
===  DEPENDENCIES
===
===  Requires the signal pointer to the MAC_L1_DYNAMIC_TBF_CONFIG signal
===  The dl msg data after its been decoded by csn.1 ===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_global_dynamic_alloc_ie(mac_dynamic_tbf_information_T *ul_tbf_msg_ptr,
                                  packet_uplink_assignment_t    *pua_ptr,
                                  gas_id_t                       gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_pkt_tmslt_reconfig_dynamic_alloc_ie()
===
===  DESCRIPTION
===
===  loads MAC_L1_TIMESLOT_RECONFIGURE with
===  dl power control parmas (po and pr_mode )
===  timeslot assignment
===  USF alloc,
===  usf granularity
===  ul power control params alpha and gamma where applicable.
===
===  DEPENDENCIES
===
===  Requires the signal pointer to the MAC_L1_TIMESLOT_RECONFIGURE signal
===  The dl msg data after its been decoded by csn.1
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_pkt_tmslt_reconfig_dynamic_alloc_ie(mac_l1_ts_reconfig_t *ts_reconfig_msg_ptr,
                                                 gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      gmac_util_dl_multislot_class_check()
===
===  DESCRIPTION
===  Performs multislot class check for a downlink timeslot allocation.
===  This check does not take into account any uplink timeslot allocation.
===
===  - Check max number of allowed UL TS
===  - Check TS 0 and 7 are not both allocated
===
===  DEPENDENCIES
===  gmac_local_dl_reassign_ts - store for new DL TS allocation
===
===  RETURN VALUE
===  - TRUE if multislot class check succeeds
===  - FALSE for multislot class violation
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_util_dl_multislot_class_check(uint8 dl_ts_alloc,
                                           gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_dyn_multislot_class_check()
===
===  DESCRIPTION
===  Performs multislot class check for combined uplink DYNAMIC and
===  downlink timeslot allocation. Note the UL downlink control
===  timeslot(s) are the same as for all UL TN for UL dynamic.
===
===  - Check max number of allowed UL TS
===  - Check max number of allowed DL TS
===  - Check SUM of number of UL TS + DL TS
===  - Check TS 0 and 7 are not both allocated for UL and DL
===  - Check UL and DL TN are different
===
===  DEPENDENCIES
===  gmac_local_ul_realloc_ts - store for new UL TS allocation
===  gmac_local_ul_realloc_dl_ctrl_ts - store for new UL downlink control TS
===  gmac_local_dl_reassign_ts - store for new DL TS allocation
===
===  RETURN VALUE
===  - TRUE if multislot class check succeeds
===  - FALSE for multislot class violation
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_util_dyn_multislot_class_check(uint8 ul_ts_alloc, 
                                            uint8 dl_ts_alloc,
                                            gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_dyn_multislot_cls12_and_eda_check()
===
===  DESCRIPTION
===  ===
===  RETURN VALUE
===  ===
===  SIDE EFFECTS
===
===
===========================================================================*/
#ifdef FEATURE_GPRS_PS_HANDOVER
boolean gmac_util_multislot_class_check(uint8   ul_ts_alloc,
                                        uint8   dl_ts_alloc,
                                        uint8   eda,
                                        uint8   cs_ts_alloc,
                                        boolean dtm_ms_class_check,
                                        gmac_psho_update_params_reqd_t psho_update_params_reqd,
                                        gas_id_t gas_id);
#else
boolean gmac_util_multislot_class_check(uint8   ul_ts_alloc,
                                        uint8   dl_ts_alloc,
                                        uint8   eda,
                                        uint8   cs_ts_alloc,
                                        boolean dtm_ms_class_check,
                                        gas_id_t gas_id);
#endif /*FEATURE_GPRS_PS_HANDOVER*/

/*===========================================================================
===
===  FUNCTION      gmac_send_prr
===
===  DESCRIPTION
===  Reallocation request from RLC results in sending a PACKET RESOURCE
===  REQUEST to the network. Compile the message, start T3168 then send
===  message to L1.
===
===  DEPENDENCIES
===  msg_ptr - received message from RLC
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_send_prr(idle_mode_ul_tbf_release_action_t re_est_action, 
                   boolean idle_realloc_req,
                   gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_send_mac_grr_abnormal_release()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_send_mac_grr_abnormal_release
(
   rr_abnormal_release_cause_t  mac_release_cause,
   gas_id_t gas_id
);

/*===========================================================================
===
===  FUNCTION      gmac_util_send_mac_l1_release()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_util_send_mac_l1_release
(
    release_tbf_t tbf_release_desc,
    gas_id_t      gas_id
);

/*===========================================================================
===
===  FUNCTION      gmac_util_send_dl_release_to_rlc
===
===  DESCRIPTION
===  Send access reject to RLC with signal MR_DL_RELEASE_IND.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_dl_release_to_rlc(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_send_grr_mac_status()
===
===  DESCRIPTION
===  Send MAC_GRR_MAC_STATUS to GRR to indicate the current MAC status.
===
===  DEPENDENCIES
===
===  global definition used by idle mode to store mac to grr signal .
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_grr_mac_status(rr_mac_status_t status,
                                   gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_frequency_param_ie()
===
===  DESCRIPTION
===  Checks the frequency parameters received in an assignment message
===  and returns the appropriate value associated with the validity of
===  the frequency assignment
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - pointer to the CSN.1 decoded logical channel message
===
===  RETURN VALUE
===  Returns the error cause and recover type (pua_data_error_t):
===  - NO_ERRORS
===  - FREQ_ERROR_ACCESS_RETRY
===  - FREQ_ERROR_PARTIAL_SYS_ACQ
===
===  SIDE EFFECTS
===
===
===========================================================================*/
pua_data_error_t gmac_util_frequency_param_ie(boolean compare_flag,
                                              boolean send_freq_config,
                                              gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_pmr_on_pacch_rrbp()
===
===  DESCRIPTION
===  Function used by transfer to send a pmr during an active downlink tbf
===  The pmr is sent in a PACCH RRBP message to the network
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_pccf_on_pacch_rrbp(uint8 ts, 
                                           uint8 rrbp, 
                                           uint32 fn,
                                           gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_pccn_on_pacch_rrbp()
===
===  DESCRIPTION
===
===  Function used by transfer to send a PACKET_CELL_CHANGE_NOTIFICATION
===  during an active downlink tbf. The pkt ms tbf_status is sent in a
===  PACCH RRBP message to the network
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_pccn_on_pacch_rrbp(uint8 ts, 
                                           uint8 rrbp, 
                                           uint32 fn,
                                           gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_pss_on_pacch_rrbp()
===
===  DESCRIPTION
===
===  Function used by transfer to send a PACKET_SI_STATUS
===  during an active downlink tbf. The pkt ms tbf_status is sent in a
===  PACCH RRBP message to the network
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_pss_on_pacch_rrbp(uint8 ts, 
                                          uint8 rrbp, 
                                          uint32 fn,
                                          gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_pps_on_pacch_rrbp()
===
===  DESCRIPTION
===
===  Function used by transfer to send a PACKET_PSI_STATUS
===  during an active downlink tbf. The pkt ms tbf_status is sent in a
===  PACCH RRBP message to the network
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_pps_on_pacch_rrbp(uint8 ts, 
                                          uint8 rrbp, 
                                          uint32 fn,
                                          gas_id_t gas_id);

#ifdef FEATURE_GSM_EDTM
/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_edtm_pcr_on_pacch_rrbp()
===
===  DESCRIPTION
===
===  Function used by transfer to send a PACKET_CS_REQUEST message
===  during an active downlink tbf.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_edtm_pcr_on_pacch_rrbp(uint8 ts, 
                                               uint8 rrbp, 
                                               uint32 fn,
                                               gas_id_t gas_id);
#endif /*FEATURE_GSM_EDTM*/

/*===========================================================================
===
===  FUNCTION    gmac_util_send_pmr_cnf_to_grr()
===
===  DESCRIPTION
===  Send measurement report confirmation to GRR. The result indicates
===  whether the measurement report was sent or not.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_pmr_cnf_to_grr(success_failure_t result,
                                   gas_id_t gas_id);



/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_pmr_on_pacch_rrbp()
===
===  DESCRIPTION
===  Function used by transfer to send a pmr during an active downlink tbf
===  The pmr is sent in a PACCH RRBP message to the network
===
===  If PMR can be sent, csn encode PMR from GRR ready to send in MAC_PH_DATA_REQ
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_pmr_on_pacch_rrbp(uint8 ts, 
                                          uint8 rrbp, 
                                          uint32 fn,
                                          gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION   gmac_util_send_cell_resel_ind_to_grr()
===
===  DESCRIPTION
===  Send MAC_GRR_CELL_RESEL_IND to GRR that that with reselection status.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_cell_resel_ind_to_grr(pcco_status_t status,
                                          gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gprs_grr_mac_acc_func
===
===  DESCRIPTION
===  Used by MAC to send messages to GRR. MAC is required to add the IMH
===  header to the message before sending it to GRR using gs_send_message
===
===  INPUT
===  Pointer to GRR message structure of the message to be sent
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gprs_grr_mac_acc_func(mac_grr_sig_t *common_msg_ptr,
                           gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gprs_l1_mac_acc_func
===
===  DESCRIPTION
===  Used by MAC to send messages to L1. MAC is required to add the IMH
===  header to the message before sending it to L1 using gs_send_message
===
===  INPUT
===  Pointer to L1 message structure of the message to be sent
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gprs_l1_mac_acc_func(mac_l1_sig_t *common_msg_ptr,
                          gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      determine_access_requested()
===
===  DESCRIPTION
===
===  Determine access type request by RLC for an ul tbf est.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  "access_req"
===
===  SIDE EFFECTS
===
===========================================================================*/
access_type_e  determine_access_requested(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_idle_pkt_polling_request_handler()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_idle_pkt_polling_request_handler(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_pmo_handler()
===
===  DESCRIPTION
===  Sends GRR PMO msg if its correctly addressed.
===
===  DEPENDENCIES
===  ptr_csn_decode_dl_msg - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_util_pmo_handler(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_pkt_paging_on_pacch()
===
===  DESCRIPTION
===  Handles the receipt of PACKET PAGING REQUEST on PACCH during
===  packet transfer. Note that this will only allow CS paging to
===  succeed. Send to GRR for actioning.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_util_pkt_paging_on_pacch(gas_id_t gas_id);

#ifdef FEATURE_GSM_EDTM
  /*===========================================================================
===
===  FUNCTION      gmac_util_check_pkt_cs_cmd_addressing()
===
===  DESCRIPTION
===
===  Checks that the Packet CS Command addresses the mobile.
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_util_check_pkt_cs_cmd_addressing(gas_id_t gas_id);
#endif /*FEATURE_GSM_EDTM*/

/*===========================================================================
===
===  FUNCTION      gmac_util_check_pcco_pmo_addressing()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_util_check_pcco_pmo_addressing(gas_id_t gas_id);




/*===========================================================================
===
===  FUNCTION      gmac_mac_control_header_tfi_checking()
===
===  DESCRIPTION
===
===  RETURN VALUE
===
===  True if TFI matched.
===  note if true then control ack value to be used is in PCA is "2"
===  SIDE EFFECTS
===
===========================================================================*/
mac_header_tfi_t gmac_mac_control_header_tfi_checking(gas_id_t gas_id);





/*===========================================================================
===
===  FUNCTION         gmac_egprs_decode_ul_window_size
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===========================================================================*/
uint16 gmac_egprs_decode_ul_window_size(uint8 encoded_ws, 
                                        uint8 usf_bitmap,
                                        gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION         gmac_egprs_decode_dl_window_size
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/

uint16 gmac_egprs_decode_dl_window_size(uint8 encoded_ws,
                                        uint8 dl_ts_alloc,
                                        gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION         gmac_egprs_fai_set()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_egprs_fai_set(packet_uplink_ack_nack_t *puan_data_ptr,
                           gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      gmac_idle_egprs_rac_procedure()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  csn_dec_data_ptr
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_idle_egprs_rac_procedure
(
boolean two_phase_access,
boolean arac_retran,
gas_id_t gas_id
);

/*===========================================================================
===
===  FUNCTION      gmac_send_rac_prr
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_send_rac_prr
(
 idle_mode_ul_tbf_release_action_t re_est_action,
 boolean send_additional_rac_info,
 boolean two_phase_access,
 gas_id_t gas_id
);

/*===========================================================================
===
===  FUNCTION      gmac_calc_pkt_resource_req_size
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
uint8 gmac_calc_pkt_resource_req_size(boolean two_phase_access, 
                                      boolean format_gprs_prr,
                                      gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION    gmac_send_add_rac_info_msg
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_send_add_rac_info_msg(idle_mode_ul_tbf_release_action_t re_est_action,
                                gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_retran_prr_msg()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_retran_prr_msg(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_retran_arac_msg()
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_retran_arac_msg(gas_id_t gas_id);

/*2p rac*/
/*===========================================================================
===
===  FUNCTION    gmac_cla_size_of_rac_ie()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===  size of rac ie in bits
===  SIDE EFFECTS
===
===========================================================================*/
uint16 gmac_cla_size_of_rac_ie(uint8 band_cnt, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_idle_egprs_rac_procedure()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  csn_dec_data_ptr
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/

void gmac_idle_egprs_rac_procedure_2
(
boolean two_phase_access,
boolean arac_retran,
boolean reformat_arac,
boolean format_gprs_rac,
gas_id_t gas_id
);


/*===========================================================================
===
===  FUNCTION    gmac_retran_arac_msg()
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_format_rac_ie(uint8 start_pos, uint8 band_count, 
                           boolean construct_arac_rac_ie,
                           gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    gmac_reset_rac_params()
===
===  DEPENDENCIES
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_reset_rac_params(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_egprs_cont_res_pua_freq_param_ie_util()
===
===  DESCRIPTION
===  Checks the frequency parameters received in an PUA assignment message,
===  rcvd as part of EGPRS 1p contetnion res, and returns the appropriate
===  value associated with the validity of the frequency assignment
===
===  DEPENDENCIES
===  NOTE this function is not dependent on
===       csn_dec_hdr_ptr - pointer to the CSN.1 decoded logical channel message
===
===  RETURN VALUE
===  Returns the error cause and recover type (pua_data_error_t):
===  - NO_ERRORS
===  - FREQ_ERROR_ACCESS_RETRY
===  - FREQ_ERROR_PARTIAL_SYS_ACQ
===
===  SIDE EFFECTS
===
===========================================================================*/
pua_data_error_t gmac_egprs_cont_res_pua_freq_param_ie_util(boolean compare_flag,

boolean send_freq_config,
freq_params_t *csn_pua_freq_param_ptr,
gas_id_t gas_id

);


/*=============================================================================

FUNCTION gmac_mac_l1_freq_config()

DESCRIPTION
   Sends MAC_L1_DTM_CHANNEL_ASSIGN_REQ to L1
PARAMS
   gmac_mac_l1_freq_config

RETURN VALUE

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_l1_freq_config(mac_l1_freq_config_t *sig_data_ptr,
                                  gas_id_t              gas_id);

#ifdef FEATURE_GSM_EDTM
/*=============================================================================

FUNCTION gmac_send_mac_grr_edtm_pkt_cs_est_rej()

DESCRIPTION
  Constructs and sends MAC_GRR_EDTM_PKT_CS_EST_REJ to GRR.

PARAMS
  cs_rej_cause

RETURN VALUE
  void

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_grr_edtm_pkt_cs_est_rej(mac_grr_edtm_cs_rej_cause_t cs_rej_cause,
                                           gas_id_t                    gas_id);

/*=============================================================================

FUNCTION gmac_send_mac_grr_edtm_cs_release_ind()

DESCRIPTION
  Constructs and sends MAC_GRR_EDTM_CS_RELEASE_IND to GRR.

PARAMS
  cs_rej_cause

RETURN VALUE
  void

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_grr_edtm_cs_release_ind(mac_grr_edtm_cs_rel_cause_t cs_rel_cause,
                                           gas_id_t                    gas_id);
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GSM_DTM
/*=============================================================================

FUNCTION gmac_send_mac_grr_dtm_channel_assign_rej()

DESCRIPTION
  Constructs and sends MAC_GRR_DTM_CHANNEL_ASSIGN_REJ to GRR. Used when a an error
  in the assignment data has caused MACC to abort the establishment.

PARAMS
  dtm_ass_status

RETURN VALUE

SIDE EFFECTS
=============================================================================*/
void gmac_send_mac_grr_dtm_channel_assign_rej(dtm_ass_status_t dtm_ass_status,
                                              gas_id_t         gas_id);


/*=============================================================================

FUNCTION gmac_mac_l1_dtm_channel_assign_req()

DESCRIPTION
  Sends MAC_L1_DTM_CHANNEL_ASSIGN_REQ to L1

PARAMS
  mac_l1_dtm_channel_assign_req

RETURN VALUE

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_l1_dtm_channel_assign_req
(
  mac_l1_dtm_channel_assign_req_t *mac_l1_dtm_chan_ass_req_sig,
  gas_id_t                         gas_id
);

/*===========================================================================

FUNCTION gmac_dtm_chan_ass_handler

DESCRIPTION
  Take the assignment data rcvd from GRR and uses it to construct mac_l1_freq_config
  destined for l1.

  Determines what pkt assigment information and what CS data is to be included
  mac_l1_dtm_chan_ass_req

PARAMS
 dtm_ass_ptr                   Rcvd signal data
 mr_ul_alloc_cnf_sig_ptr       Send signal data
 mac_l1_dtm_chan_ass_req_ptr   Send signal data
 mac_l1_freq_config_ptr        Send signal data

RETURN VALUE
 dtm_ass_status = status of assigment .

SIDE EFFECTS
  None

===========================================================================*/
dtm_ass_status_t gmac_dtm_chan_ass_handler
(
  gm_dtm_channel_assign_req_t       *dtm_ass_ptr,
  mr_ul_alloc_cnf_t                 *mr_ul_alloc_cnf_sig_ptr,
  mac_l1_dtm_channel_assign_req_t   *mac_l1_dtm_chan_ass_req_ptr,
  mac_l1_freq_config_t              *mac_l1_freq_config_ptr,
  gas_id_t                          gas_id
);

/*===========================================================================

FUNCTION gmac_dtm_ul_ass_handler

DESCRIPTION
  Take the UL assignment data rcvd from GRR and uses it to construct
  MAC_L1_DTM_CHANNEL_ASSIGN_REQ destined for l1
  MR_UL_ALLOC_CNF destined for RLC

PARAMS
  dtm_ul_ass_ptr          :-pkt ul ass rcvd from grr
  mr_ul_alloc_cnf_sig_ptr :-MR_UL_ALLOC_CNF to be send to rlc
  ul_tbf_ptr              :-MAC_L1_DTM_CHANNEL_ASSIGN_REQ to be sent to l1

RETURN VALUE
  dtm_ass_status

SIDE EFFECTS
  None

===========================================================================*/
dtm_ass_status_t gmac_dtm_ul_ass_handler
(
   rr_packet_uplink_assignment_t    *dtm_ul_ass_ptr,
   mr_ul_alloc_cnf_t                *mr_ul_alloc_cnf_sig_ptr,
   mac_dynamic_tbf_information_T    *ul_tbf_ptr,
   gas_id_t                          gas_id);

/*===========================================================================

FUNCTION gmac_dtm_ul_dyn_alloc_ie

  Take the UL dynamic assignment data rcvd from GRR and uses it to construct the UL tbf
  configuration data portion of MAC_L1_DTM_CHANNEL_ASSIGN_REQ destined for l1

PARAMS
  rr_pua_dyn_alloc_ie_ptr :- pkt ul ass data rcvd from grr
  ul_tbf_ptr              :- ul tbf config data to be sent to l1

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void gmac_dtm_ul_dyn_alloc_ie
(
  rr_pua_dynamic_alloc_t         *rr_pua_dyn_alloc_ie_ptr,  /*pkt ul ass rcvd from grr*/
  mac_dynamic_tbf_information_T  *ul_tbf_ptr,               /*ul_tbf struct sent to l1*/
  gas_id_t                        gas_id
);

/*===========================================================================

FUNCTION gmac_dtm_dl_ass_handler

  Take the DL assignment data rcvd from GRR and uses it to construct the DL tbf
  configuration data portion of MAC_L1_DTM_CHANNEL_ASSIGN_REQ destined for L1

PARAMS
  dtm_dl_ass_ptr :-  DL assigment data rcvd form GRR
  dl_tbf_ptr     :-  DL assigment data to be sent to L1

RETURN VALUE
  dtm_ass_status

SIDE EFFECTS
  None

===========================================================================*/

dtm_ass_status_t gmac_dtm_dl_ass_handler
(
  rr_packet_downlink_assignment_t  *dtm_dl_ass_ptr,
  mac_downlink_tbf_t               *dl_tbf_ptr,
  gas_id_t                          gas_id
);

/*===========================================================================

FUNCTION gmac_dtm_freq_handling

DESCRIPTION
 Takes the frequency assigment rcvd from GRR and constructs the
 mac_l1_freq_config signal

PARAMS
 freq_info_present :- is frequency data present
 dtm_freq_ass_ptr  :- freq info rcvd form GRR
 freq_config_ptr   :- freq config struct for l1
 tbf_type          :- tbf being establsihed (ul,dl or both)
RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void gmac_dtm_freq_handling
(
   boolean                       freq_info_present,
   frequency_information_T       *dtm_freq_ass_ptr,
   mac_l1_freq_config_t          *freq_config_ptr,
   dtm_tbf_type_t                dtm_tbf_type,
   gas_id_t                      gas_id
);

/*===========================================================================

FUNCTION gmac_dtm_check_for_egprs_ass_clash

DESCRIPTION
  Checks for an EGPRS assignment clash in UL and DL DTM assignments.

PARAMS
  dtm_ass_ptr - DTM Assigment.

RETURN VALUE
  dtm_ass_status_t - DTM Assignment Status.  Returns DTM_ASS_REJ_PROTOCOL_ERR if
                     there is an EGPRS clash.

SIDE EFFECTS
  None

===========================================================================*/

dtm_ass_status_t gmac_dtm_check_for_egprs_ass_clash(
                   gm_dtm_channel_assign_req_t *dtm_ass_ptr,
                   gas_id_t gas_id );

#endif /* FEATURE_GSM_DTM */

/*=============================================================================

FUNCTION gmac_send_mac_l1_sb_config()

DESCRIPTION
   Sends MAC_L1_SINGLE_BLOCK_CONFIG to L1
PARAMS
   sig_data_ptr

RETURN VALUE

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_l1_sb_config(mac_l1_single_block_config_t *sig_data_ptr,
                                gas_id_t gas_id);

/*=============================================================================

FUNCTION gmac_send_mac_l1_dyn_config()

DESCRIPTION
   Sends MAC_L1_DYNAMIC_TBF_CONFIG to L1
PARAMS
   sig_data_ptr

RETURN VALUE

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_l1_dyn_config(mac_l1_dynamic_tbf_config_t *sig_data_ptr,
                                 gas_id_t gas_id);

/*===========================================================================

FUNCTION gmac_send_mac_l1_dl_config

DESCRIPTION
  Sends mac_l1_dl_config

PARAMS
  l1_dl_tbf_cnf_ptr - DL Assignment parameters for L1.

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void gmac_send_mac_l1_dl_config(mac_l1_dl_tbf_config_t *l1_dl_tbf_cnf_ptr,
                                gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_init_util_data()
===
===  DESCRIPTION
===  Initialises Util data.  Used for Lower layer resets.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_util_data( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION    mac_complete_reset()
===
===  DESCRIPTION
===  Initialises all MAC data and flags.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_complete_reset( gas_id_t gas_id );

/*===========================================================================

FUNCTION gmac_gprs_check_frequency_params

DESCRIPTION
  This function checks for invalid frequencies not supprted by the hardware.
DEPENDENCIES
  None

RETURN VALUE
  TRUE   : if an ivalid frequency has been detected.
  FALSE  : if all frequencies are valid.

SIDE EFFECTS
  None

===========================================================================*/
boolean gmac_gprs_check_frequency_params(
                                        frequency_list_T *frequency_list_ptr,
                                        boolean hopping_flag,
                                        gas_id_t gas_id
                                        );

/*===========================================================================
===
===  FUNCTION    gmac_util_encode_ul_control_msg_handler()
===
===  DESCRIPTION
===  This function encodes the different uplink control messages
===  and puts the encoded message into a temporary storage.
===  This function returns the success or failure of the csn encoding
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  uint8 returns FAIL or PASS
===  SIDE EFFECTS
===
===
===========================================================================*/

uint8 gmac_util_encode_ul_control_msg_handler(ul_chan_msg_t msg_type,
                                              boolean dl_only_present,
                                              gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION    gmac_util_send_ul_control_msg_handler()
===
===  DESCRIPTION
===  This function sends the encoded uplink message to layer1
===  This function also switches PCCN message from
===  uplink to downlink and vice versa
===
===  DEPENDENCIES
===  Depending upon the following global structures
===  gmac_local_pccf_params, gmac_local_pps_params,
===  gmac_local_pss_params, gmac_local_pmr_params
===  gmac_local_pccn_params
===
===  RETURN VALUE
===  void
===  SIDE EFFECTS
===
===
===========================================================================*/

void gmac_util_send_ul_control_msg_handler(ul_chan_msg_t msg_type,
                                           existing_tbf_type_t existing_tbf_type,
                                           gmac_grr_cnf_needed_t grr_cnf_needed,
                                           gas_id_t gas_id);
/*cr134062*/
/*===========================================================================
===
===  FUNCTION         gmac_check_assigned_egprs_ws()
===
===  DESCRIPTION
===
===  Determine if the assigned egprs window size is with in the range supported
===  by the multislot class of the UE
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  PASS_RESULT: The egprs window size is within the allowable limits
===               dictated by the multi slot class and is greater than the
===               minimum WS that's allowable
===
===  FAIL_RESULT: The assigned gprs window greater then the allowable for the
===               Mutilslot class of the UE or is not is not greater the
===               minimum allowable size of 64.
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/

gmac_result_t gmac_check_assigned_egprs_ws(uint8 max_ts_for_ms_class, uint16 assigned_egprs_ws, gas_id_t gas_id);


#ifdef FEATURE_GSM_EDTM
/*=============================================================================

FUNCTION gmac_send_mac_l1_delete_ul_ctrl_msg()

DESCRIPTION
  Constructs and sends MAC_L1_DELETE_UL_CTRL_MSG to GRR.

PARAMS
  msg_type

RETURN VALUE
  void

SIDE EFFECTS
=============================================================================*/
void gmac_send_mac_l1_delete_ul_ctrl_msg(uint8 msg_type, gas_id_t gas_id);
#endif /*FEATURE_GSM_EDTM*/

boolean  gmac_util_packet_cell_change_failure_handler(gas_id_t gas_id);

void    gmac_nacc_respond_to_release_req(mac_signals_u *, gas_id_t gas_id);

/* FUNCTION PROTOTYPES */
void    gmac_util_init_cell_data(gas_id_t gas_id);
void    gmac_util_release_container(uint8, gas_id_t gas_id);
void    gmac_util_handle_pkt_cell_data(uint8, ARFCN_T, gas_id_t gas_id);
void    gmac_util_send_serving_si(csn_decoder_ptrs_t *, ARFCN_T, gas_id_t gas_id);
boolean gmac_util_check_tfi(uint8, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_check_rlc_blocked_flag
===
===  DESCRIPTION
===  This function checks the current state of the rlc_blocked_flag with the expected
===  state.  MSG_ERROR is called if they do not match.
===
===  PARAMETERS
===  rlc_block_flag_check - expected rlc blocked state.
===
===  DEPENDENCIES
===  None
===
===  RETURN VALUE
===  TRUE - requested value matches expected value.
===  FALSE - requested value doesn't match expected value.
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
gmac_result_t gmac_util_check_rlc_blocked_flag(gmac_rlc_blocked_t expected_rlc_block_flag,
                                               gmac_filenames_t filename,
                                               uint32           linenumber,
                                               gas_id_t         gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_is_rlc_blocked
===
===  DESCRIPTION
===  This function returns the current blocked state of RLC.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  gmac_rlc_blocked_t - current state of gmac_rlc_blocked_flag
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
gmac_rlc_blocked_t gmac_util_is_rlc_blocked(gmac_filenames_t filename,
                                            uint32           linenumber,
                                            gas_id_t         gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_unblock_rlc
===
===  DESCRIPTION
===  This function sends MR_NO_PS_ACCESS_IND to RLC and sets rlc_blocked_flag
===  to FALSE.  If RLC is already unblocked when this is called then no signal
===  is sent to RLC.
===
===  action - NO_CELL_CHANGE_NO_PURGE           -   no cell reselection has happened between no service to service .
===              CELL_CHANGE_PURGE_GMM_SIG        -  cell reselection has happened between no service to service
===              NO_CELL_CHANGE_PURGE_GMM_SIG -   no cell reselection has happened but grlc needs to delete gmm messages.
===                                                                                Special case for T3142 AND T3172 timers
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  None
===
===  SIDE EFFECTS
===  gmac_rlc_blocked_flag is set
===
===========================================================================*/
void gmac_util_unblock_rlc(action_on_ps_access_t          action,
                           gmac_filenames_t filename,
                           uint32 linenumber,
                           gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_block_rlc
===
===  DESCRIPTION
===  This function sends MR_NO_PS_ACCESS_IND to RLC and sets rlc_blocked_flag
===  flag to TRUE.  If RLC is already blocked when this is called a MR_NO_PS_ACCESS_IND
===  is still sent so that delete_pdu_cause can be updated in RLC.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  None
===
===
===  SIDE EFFECTS
===  gmac_rlc_blocked_flag is set
===
===========================================================================*/
void gmac_util_block_rlc(delete_pdu_cause_t delete_pdu_cause,
                         gmac_filenames_t   filename,
                         uint32             linenumber,
                         gas_id_t           gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_util_start_pcco_process()
===
===  DESCRIPTION
===  This function is used to set the state of the "gmac_pcco_state".
===  and is called on initiation of the PCCO procedure.
===
===  PCCO procedure can ONLY be initiated on reception of GRR_MAC_PS_ACCESS_REQ
===  with signal parameter "pcco_in_progress" set to TRUE.
===
===  DEPENDENCIES
===  gmac_pcco_state (True = PCCO is currently in progress)
===
===  RETURN VALUE None
===
===  SIDE EFFECTS None
===========================================================================*/
void gmac_util_start_pcco_process (boolean activate_pcco,
                                   gmac_filenames_t file_name,
                                   uint32 line_no,
                                   uint8 state,
                                   gas_id_t gas_id
                                  );

/*===========================================================================
===
===  FUNCTION    gmac_util_complete_pcco_process()
===
===  DESCRIPTION
===  This function is called when the sending of PCCO has deemed to be completed
===  either by a successful contention resolution OR the failure of PCCO.
===
===  For successful completion of PCCO MAC is obliged to ALWAYS send
===  MAC_GRR_CELL_RESEL_STATUS
==
===  For failure PCCO :-
===
===  If GRR initiated termination , (ie GRR sends GRR_MAC_NO_PS_ACCESS) for
===  these conditions MAC is NOT required to send MAC_GRR_CELL_RESEL_STATUS to
===  indicate to GRR the failure of PCCO.
===
===  IF MAC initiated termination , then MAC is obliged to send
===  MAC_GRR_CELL_RESEL_STATUS to GRR indicating failure reason.
===
===  DEPENDENCIES
===  gmac_pcco_state
===
===  RETURN VALUE
===  void
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_complete_pcco_process( pcco_status_t                pcco_status,
                                      gmac_no_access_initiator_t   initiated_by,
                                      gmac_filenames_t             file_name,
                                      uint32                       line_no,
                                      gas_id_t                     gas_id
                                     );
/*===========================================================================
===
===  FUNCTION      gmac_util_get_pcco_state()
===
===  DESCRIPTION
===  This function returns the current PCCF state.
===
===  DEPENDENCIES
===  gmac_pcco_state
===
===  RETURN VALUE
===
===  PCCO_ACTIVE
===  NO_PCCO_ACTIVE
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
gmac_pcco_state_t gmac_util_get_pcco_state(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_util_get_pccf_state()
===
===  DESCRIPTION
===  This function returns the current PCCF state.
===
===  DEPENDENCIES
===  gmac_pccf_state
===
===  RETURN VALUE
===
===  NO_PCCF_BEING_SENT
===  SENDING_VIA_SB
===  SENDING_VIA_UL_PACCH
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
gmac_pccf_state_t gmac_util_get_pccf_state(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_util_start_pccf_process()
===
===  DESCRIPTION
===  This function is used to set the state of the "gmac_pccf_state".
===  and is called on initiation,completion or abortion of the sending
===  of PCCF.
===
===  PCCFcan ONLY be initiated on reception of GRR_MAC_SEND_PCCF
===
===  PCCF procedure can be terminated by GRR using GRR_MAC_NO_PS_ACCESS and for
===  these conditions MAC is not required to indicate to GRR the failure of PCCF.
===
===  If MAC terminated the sending of PCCF then, it is obliged to send
===  MAC_GRR_PCCF_SENT(retry) to GRR indicating failure.
===
===  If the sending of PCCF then, MAC is  obliged to send
===  MAC_GRR_PCCF_SENT(no retry) to GRR indicating success.
===
===  True = PCCF is currently in progress of being sent
===
===  DEPENDENCIES
===
===  gmac_pccf_state
===
===  RETURN VALUE None
===
===  SIDE EFFECTS None
===========================================================================*/
void gmac_util_start_pccf_process ( gmac_pccf_state_t pccf_tx_method,
                                    gmac_filenames_t file_name,
                                    uint32 line_no,
                                    uint8 state,
                                    gas_id_t gas_id
                                   );

/*===========================================================================
===
===  FUNCTION    gmac_util_complete_pccf_process()
===
===  DESCRIPTION
===  This function is called when the sending of PCCF has deemed to be completed
===  either by a successful sending of PCCF OR the failure to send the PCCF.
===
===  PCCF procedure can be terminated by GRR using GRR_MAC_NO_PS_ACCESS and for
===  these conditions MAC is not required to indicate to GRR the failure of PCCF.
===
===  If MAC terminated the sending of PCCF then, it is obliged to send
===  MAC_GRR_PCCF_SENT(retry) to GRR indicating failure.
===
===  If the sending of PCCF then, MAC is  obliged to send
===  MAC_GRR_PCCF_SENT(no retry) to GRR indicating success.
===
===  In the case where the PCCF sending fails and GRR is sent MAC_GRR_PCCF_SENT with
===  an unsuccessful indication. This will immediately cause GRR to re-request the
===  sending PCCF. This process will be repeated until T3716 expires at which point
===  GRR will perform a re-selection
===
===  Hence it is essential that for failure to send case, L1 has been released
===  before sending MAC_GRR_PCCF_SENT to GRR.
===
===  DEPENDENCIES
===  gmac_pccf_state
===
===  RETURN VALUE
===  void
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_complete_pccf_process(pccf_status_t                pccf_status,
                                     gmac_no_access_initiator_t   initiated_by,
                                     gmac_filenames_t             file_name,
                                     uint32                       line_no,
                                     gas_id_t                     gas_id);


/*===========================================================================
===
===  FUNCTION      gmac_util_initialise_page_mode()
===
===  DESCRIPTION
===
===  On GRR_MAC_PS_ACCESS_REQ initialise gmac_current_page_mode
===
===  DEPENDENCIES
===  gmac_current_page_mode
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_initialise_page_mode(gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION    gmac_util_page_mode_handler()
===
===  DESCRIPTION
===  This function will update the page mode and if the page mode is changed
===  it will send a signal to layer1
===
===  DEPENDENCIES
===  gmac_current_page_mode
===
===  RETURN VALUE
===  void
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_page_mode_handler(gas_id_t gas_id);
#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      gmac_tran_send_grlcul_psho_start_ind()
===
===  DESCRIPTION
===  This function sends MR_UL_PSHO_START_IND signal to GRLC UL task
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_grlcul_psho_start_ind(gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION      gmac_util_send_grlcdl_psho_start_ind()
===
===  DESCRIPTION
===  This function sends MR_DL_PSHO_START_IND signal to GRLC DL task
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_grlcdl_psho_start_ind(gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION      gmac_util_ps_ho_req_handler()
===
===  DESCRIPTION
===  Handles the PS HANDOVER COMMAND message.  Performs validation of PS HANDOVER
===  COMMAND message. if validation success then it builds the MAC_L1_PSHO_REQ message
===  and send this message to L1. if validation fails then it sends PSHO STATUS with
===  FAILURE to GRR.
===
===  CTRL_ACK handling:
===  The requirements for CTRL_ACK are in 44.060 11.2.37.2.
===  If CTRL_ACK is set then MAC will check that T3192 is running, the TFI
===  specified matches and that the PCRI is received on the DL PACCH.  If this
===  is the case then the DL TBF is released.
===
===  If CTRL_ACK is not set but T3192 is running then GMAC ignores PSHANDOVER command and
===  sends failure to GRR
===
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    boolean (TRUE or FALSE)
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_util_ps_ho_req_handler(gprs_mac_transfer_substates_t mac_transfer_substate,
                                    gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION      gmac_util_send_grr_psho_cnf()
===
===  DESCRIPTION
===  This function sends MAC_GRR_PSHO_CNF signal to GRR
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_grr_psho_cnf(psho_status_t psho_status,
                                 gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION      gmac_util_initialise_psho_structure()
===
===  DESCRIPTION
===  This function initialises the gmac_ps_ho structure
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_initialise_psho_structure(gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION      gmac_util_reset_psho_structure()
===
===  DESCRIPTION
===  This function reset the gmac_ps_ho structure
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_reset_psho_structure(gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION      gmac_util_mac_control_header_pkt_phy_info_tfi_checking()
===
===  DESCRIPTION
===  This function validates the tfi checking in the header of PKT PHYSICAL
===  INFORMATION message
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  mac_header_tfi_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
mac_header_tfi_t gmac_util_mac_control_header_pkt_phy_info_tfi_checking(gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION      gmac_util_check_pkt_phy_info_global_tfi()
===
===  DESCRIPTION
===  This function validates the tfi received in the global tfi field in
===  PKT PHYSICAL INFORMATION message
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  mac_header_tfi_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_util_check_pkt_phy_info_global_tfi(global_tfi_t rxd_global_tfi,
                                                gas_id_t gas_id);
#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/
#endif /*FEATURE_GPRS_PS_HANDOVER*/

/*===========================================================================
===
===  FUNCTION  gmac_util_send_grr_mac_rach_rej
===
===  DESCRIPTION
===  Send MAC_GRR_RACH_REJ to GRR.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_grr_mac_rach_rej(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_match_old_or_new_tlli()
===
===  DESCRIPTION
===
===    Match old (RRPS_TLLI_OLD) or new (RRPS_TLLI) TLLI from the public store.
===
===  PARAMETERS
===
===    uint32 tlli  - TLLI to be matched.
===
===  RETURN VALUE
===
===    boolean - Address matches
===
===  SIDE EFFECTS
===
===    Calls rr_read_public_store().
===
===========================================================================*/
boolean gmac_match_old_or_new_tlli(uint32 tlli, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_utils_calculate_ts_alloc_ws
===
===  DESCRIPTION
===  Calculate the window size of a timeslot allocation.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  num_ts - number of calculated timeslots
===
===  SIDE EFFECTS
===
===
===========================================================================*/
uint8 gmac_utils_calculate_ts_alloc_ws( uint8 ts_alloc, gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      gmac_utils_get_lowest_ts_allocated
===
===  DESCRIPTION
===  Returns the bit position of the lowest allocated TS.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  num_ts - number of calculated timeslots
===
===  SIDE EFFECTS
===
===
===========================================================================*/
uint8 gmac_utils_get_lowest_ts_allocated(uint8 ts_alloc, gas_id_t gas_id);

#ifdef FEATURE_GSM_GPRS_MSC33
/*===========================================================================
===
===  FUNCTION      gmac_utils_get_second_lowest_ts_allocated
===
===  DESCRIPTION
===  Returns the bit position of the second lowest allocated TS.  This function
===  should only be called when there are more than 1 timeslots in ts_alloc.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  num_ts - ts number of second lowest allocated timeslot.
===
===  SIDE EFFECTS
===
===
===========================================================================*/
uint8 gmac_utils_get_second_lowest_ts_allocated(uint8 ts_alloc, gas_id_t gas_id);
#endif /*FEATURE_GSM_GPRS_MSC33*/

/*===========================================================================
===
===  FUNCTION    gmac_util_egprs_mcs_valid()
===
===  DESCRIPTION
===  Checks that an EGPRS Modulation Coding Scheme (MCS) can be supported by
===  the MS supported and cell supported options.
===
===  PARAMS
===  uint8 egprs_mcs - encoded egprs mcs value (see 44.060 12.10d)
===
===  RETURN VALUE
===  boolean - TRUE EGPRS MCS Valid
===            FALSE EGPRS MCS Invalid
===
===  SIDE EFFECTS
===  Calls to read_public_store for RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL,
===  RRPS_MS_EGPRS_8PSK_UL_ENABLED.
===
===========================================================================*/
boolean gmac_util_egprs_mcs_valid(uint8 egprs_mcs, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_eng_mode_update_tbf_info()
===
===  DESCRIPTION
===
===  Updates the eng mode when either tbf mode changes or mac mode
===
===  PARAMETERS
===
===     current_tbf_mode       = GPRS or EGPRS
===     gmac_current_mac_mode  = Dynamic or extended dynamic
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_eng_mode_update_tbf_info(
                                    uint8       update_mac_mode,
                                    tbf_mode_T  update_tbf_mode,
                                    boolean     set_to_invalid,
                                    gas_id_t    gas_id
                                  );

/*===========================================================================
===
===  FUNCTION      gmac_send_mac_grr_ps_access_cnf()
===
===  DESCRIPTION
===
===  Sends MAC_GRR_PS_ACCESS_CNF to GRR.
===
===  PARAMETERS
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
extern void gmac_send_mac_grr_ps_access_cnf(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_init_util_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) util data.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_util_data_non_zero( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      gmac_restart_ul_await_ass_guard
===
===  DESCRIPTION
===
===  Takes the starting times rcvd in IA and converts it to an absolute FN.
===  Uses this with the current FN to determine the time in milliseconds
===  to the staring of the UL tbf being established from NULL mode.  
===
===  Calculate the starting time in milliseconds.
===
===  Adjusts the starting time from mod42432 format to the absolute 
===  FN as specified in (GSM 04.18 10.5.2.38)
===
===  NOTE: THIS FUNCTION ONLY CONVERTS STARTING TIMES RCVD IN IA'S
===
===  NOTE: FOR gprs and EGPRS there will always be a valid starting time
===        present in the IA for SB 
=== 
===        This is not the case for a dynamic allocation. Where it is 
===        possible to receive an IA that does not have a staring time.
===        In this case the default is 13frame minimum reaction time.
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_restart_ul_await_ass_guard
(
boolean starting_time_present,
uint32 starting_time_fn,
gas_id_t gas_id
);

#endif /* INC_MACUTIL_H */
/*** EOF: don't remove! ***/
