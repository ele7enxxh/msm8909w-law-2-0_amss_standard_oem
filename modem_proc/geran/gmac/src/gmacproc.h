/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TRANSFER MODE MODULE
***
***
*** DESCRIPTION
***
***	 This module administrates the signal reception and the associated actions
***  for transfer mode states.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacproc.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 04/12/01   sb 	   Created
***
*****************************************************************************/

#ifndef INC_GMACPROC_H
#define INC_GMACPROC_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/

/*****************************************************************************
***
***     Public Definitions
***
*****************************************************************************/
typedef struct
{
  /********************************/
  /* GMAC messages/queues         */
  /********************************/
  q_type          mac_q;  /* MAC's signaling queue */
  mac_signals_u  *msg_ptr;      /* union of all signals sent to in MAC */
  mac_int_sig_t   mac_mac_sig; /* internal MAC message structure definition */

  /********************************/
  /* CSN                          */
  /********************************/
  /*points to the csn1 decoded DL control msg struture  */
  csn_decoder_ptrs_t     csn_decoder_result;
  csn_decoder_ptrs_t    *csn_dec_result_ptr;
  rlc_mac_cntrl_t       *csn_dec_hdr_ptr;
  uint8                 *csn_dec_data_ptr;

  /*points to the csn1 structure that's to be encoded   */
  int_data_t csn_encoder_ul_msg;

  /* buffer used to hold the data part of the rlc/mac dl control block( ie the
     logcical channel msg) and is the buffer passed to csn1*/
  uint8 dl_msg[SIZE_OF_DL_MSG];

  /********************************/
  /* GMAC global status variables */
  /********************************/
  mac_modes_t      mac_mode; /*internal mac mode state*/
  tbf_mode_T       current_tbf_mode;
  boolean          current_rcvd_dl_ctrl_ack;

  common_flags_t   c_flags;
  test_mode_T      current_test_mode;
  uint8            gmac_test_mode_ts_offset;
  boolean          gmac_test_mode_srb_mode;   /* 1 = The MS loops back blocks on the uplink using GMSK modulation only.*/
                                              /* 0 = The MS loops back blocks on the uplink using either GMSK or 8-PSK */
                                              /*     modulation following the detected received modulation.            */
  boolean          pfi_present; /*PFC Feature*/
  uint8            pfi;
  tfi_store_t      gmac_local_tfi_store; /* store uplink and downlink TFI values */
  pkt_timing_adv_t gmac_local_l1_ta_params;
  uint8            gmac_local_rrbp_valid; /* indicates if RRBP valid */
  uint8            gmac_current_mac_mode; /*current mac mode for TBF access.  This can either be Extended or
                                                Dynamic.  This variable is maintained as the current mac mode.  In
                                                DL transfer states gmac_current_mac_mode shall always be dynamic
                                                as extended does not apply. gmac_current_mac_mode is set to dynamic
                                                when exiting transfer mode.  MAC_MODE in PDA's is ignored, only
                                                UL assignment messages can alter gmac_current_mac_mode*/

  /* used to store frequency parameters currently in use by transfer */
  frequency_information_T gmac_store_freq_params_in_use;

  /* Current timeslot allocation */
  uint8 gmac_local_ul_ts_alloc;
  uint8 gmac_local_dl_ts_alloc;
  uint8 gmac_local_ul_dl_ctrl_ts;

  /* Timeslots for re-allocation */
  uint8 gmac_local_ul_realloc_ts;
  uint8 gmac_local_dl_reassign_ts;
  uint8 gmac_local_ul_realloc_dl_ctrl_ts;

  /*   indicates the current mac action, if no action always set to NONE*/
  gprs_mac_action_t mac_action;

  /*TLLI in DL CNF to RLC*/
  uint32 gmac_store_dl_ass_tlli;
  uint32 gmac_store_ul_est_tlli;

  /* indicates if TBF_EST allowed */
  boolean tbf_est_allowed_flag;

  gmac_pccn_params_t gmac_local_pccn_params;

  gmac_pss_params_t  gmac_local_pss_params;

  /* stored uplink reallocation data from RLC */
  channel_req_desc_t      gmac_local_stored_ul_chan_req;

  /* Downlink RLC_MODE */
  uint8 gmac_store_dl_rlc_mode;

  /* multislot class parameters */
  uint8 gmac_ms_class;
  uint8 gmac_ms_class_sum;
  uint8 gmac_ms_class_max_ts_tx;
  uint8 gmac_ms_class_max_ts_rx;
  uint8 gmac_ms_class_ts_tta;
  uint8 gmac_ms_class_ts_ttb;
  uint8 gmac_ms_class_ts_tra;
  uint8 gmac_ms_class_ts_trb;
   
  uint8 gmac_egprs_ms_class;
  uint8 gmac_egprs_ms_class_sum;
  uint8 gmac_egprs_ms_class_max_ts_tx;
  uint8 gmac_egprs_ms_class_max_ts_rx;
  uint8 gmac_egprs_ms_class_ts_tta;
  uint8 gmac_egprs_ms_class_ts_ttb;
  uint8 gmac_egprs_ms_class_ts_tra;
  uint8 gmac_egprs_ms_class_ts_trb;

#ifdef FEATURE_GSM_DTM
  uint8 gmac_dtm_ms_class_sum;
  uint8 gmac_dtm_ms_class_max_ts_tx;
  uint8 gmac_dtm_ms_class_max_ts_rx;
  uint8 gmac_dtm_ms_class_ts_tta;
  uint8 gmac_dtm_ms_class_ts_ttb;
  uint8 gmac_dtm_ms_class_ts_tra;
  uint8 gmac_dtm_ms_class_ts_trb;
#endif /*FEATURE_GSM_DTM*/

  uint8 gmac_local_cs_setting; /* store current coding scheme setting for transfer */

  uint8  gmac_dtm_cs_ts_alloc; /* represented in a timeslot bitmap format where
                                  MSB TS 0 -->  LSB TS 7 */

  /*******************/
  /* PSHO variables  */
  /*******************/
#ifdef FEATURE_GPRS_PS_HANDOVER
  gmac_ps_ho_t       gmac_ps_ho;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

  /*******************/
  /* EDTM variables  */
  /*******************/
#ifdef FEATURE_GSM_EDTM
  boolean gmac_edtm_enabled; /*mac global indicator for EDTM support which is set from the public store*/
  uint16  gmac_edtm_old_dl_ws;
  uint16  gmac_edtm_old_ul_ws;
  gmac_edtm_pcr_params_t gmac_local_edtm_pcr_params; /*EDTM Packet Channel Request pending on RRBP store*/
#endif /*FEATURE_GSM_EDTM*/


  /*******************/
  /* EGPRS variables */
  /*******************/
  uint16  current_egprs_dl_ws;
  uint16  current_egprs_ul_ws;
  boolean access_tech_req_present;
  uint8   access_tech_req_count; /* number of valid items in the access_tech_req array */
  uint8   access_tech_req[MAX_ACCESS_TECH_TYPE];

  /*used during 2phase access procedure. stores the allocation type ie MD or SB and the number of blocks allocated*/
  egprs_2p_sb_mb_alloc_t egprs_2p_sb_mb_alloc;


  /*********************/
  /* TBF Establishment */
  /*********************/
  gmac_pmr_params_t  gmac_local_pmr_params; /* PMR parameters saved during establishment */
  gmac_pccf_params_t gmac_local_pccf_params; /* PCCF parameters saved during establishment */

  uint8          tbf_type;
  access_type_e  access_requested;
  uint8          channel_msg; /*Holds the DL control msg received */
  boolean gmac_transfer_ul_cv_zero_ind; /* used for TBF_EST handling for UL reallocation */
  uint8 gmac_re_assign_ul_tfi;
  uint8 gmac_re_assign_dl_tfi;
  uint16 csn_msg_bit_length;  /*bit length of CSN packed msg*/
  mac_l1_single_block_config_t gmac_51_sb_params; /* store uplink single block config for two phase */
  mac_l1_dl_tbf_config_t gmac_null_store_l1_dl_config; /* downlink tbf config store */

  /* vars used when perfroming ul re-est from ul release process */
  uint32                            prr_re_est_rrbp_fn;
  uint8                             prr_re_est_rrbp_ts;
  idle_mode_ul_tbf_release_action_t idle_mode_ul_tbf_release_action;

  uint32                rlc_ul_est_tlli;
  grr_data_req_cause_t  gmac_util_data_req_cause;

  /*fields to send data*/
  mac_grr_sig_t grr_sig;
  mac_l1_sig_t l1_sig;
} gmac_global_data_t;

/*****************************************************************************
***
***     Public Function Prototypes
***
*****************************************************************************/
/*===========================================================================
===
===  FUNCTION    gmac_get_global_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to global data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_global_data_t*
=== 
===  SIDE EFFECTS
===
===
===========================================================================*/
gmac_global_data_t* gmac_get_global_data_ptr(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_comproc_handle_ppcta 
===
===  DESCRIPTION
===  Handles packet power control/timing advance message during UL fixed TBF.
===  Send parameters to L1 using MAC_L1_POWER_CONTROL.        
===
===  DEPENDENCIES
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data                       
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                      
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_comproc_handle_ppcta(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_normal_ul_tbf_release_proc() 
===
===  DESCRIPTION
===  Called from transfer to release the ul tbf once all rlc data blocks have
===  been successful transferred.
=== 
===  The trigger is the reception of PUAN with FAI (final acknowledgment
===  indicator) of Ack/Nack Description IE set.
===
===  note:- under normal release conditions RLC does not send RM_UL_RELEASE_IND 
===         signal to release the ul.
=== 
===  For the cases where there is more data to send, transfer must have previously
===  received and RM_UL_REALLOC_REQ containing the Channel Request Description IE.
===
===  ref 4.60, 9.3.3.3 and 9.3.2.4 rev 1999
===
===  DEPENDENCIES
===  pointer to last signal received by mac  ie  msg_ptr, this must be pointing
===  to the ph_data_ind that the PUAN was received in. This function needs  
===  access to the MAC control header .
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
idle_mode_ul_tbf_release_action_t gmac_normal_ul_tbf_release_proc(boolean tbf_est_set, 
                                     boolean dl_tbf_active,
                                     boolean est_new_ul_tbf,
                                     gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_init_mac_proc_data() 
===
===  DESCRIPTION
===  Initialises GMAC PROC data.  Used for Lower layer resets.
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
void gmac_init_mac_proc_data( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    gmac_init_common_flags() 
===
===  DESCRIPTION
===  Initialises the common flags, should only be called once at power on.
===
===  PARAMS
===   
===
===  RETURN VALUE
===
===    
===  SIDE EFFECTS
=== 
=== 
===========================================================================*/
void gmac_init_common_flags( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    gmac_init_mac_proc_data_non_zero() 
===
===  DESCRIPTION
===  Initialises non zero (including all enums) proc data.
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
void gmac_init_mac_proc_data_non_zero( gas_id_t gas_id );

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/

#endif /* INC_GMACPROC_H */
/*** EOF: don't remove! ***/
