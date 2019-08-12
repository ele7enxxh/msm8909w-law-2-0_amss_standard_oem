/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC IDLE MODE MODULE
***
***
*** DESCRIPTION
***
***	 This module administrates the signal reception and the associated actions
***  for idle mode states.  This header file is only for idle mode.  It should
***  only be included by gmacidle.c and gmacidleutil.c.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacidle.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 07/04/01   tlx	   Created
***
*****************************************************************************/

#ifndef INC_GMACIDLE_H
#define INC_GMACIDLE_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "gmacidle_if.h"
#include "gmaclog.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

typedef enum
{
  CON_RES_TRANSFER,
  CON_RES_RELEASE,
  CON_RES_RETRY,
  IGNORE_PUAN
} cont_res_success_t;

typedef enum
{
  IDLE_REALLOC_NO_ACTION,          /*default.                                                 */
                                   /*set if MAC_PH_CONNECT_IND before moving to transfer      */
                                   /*set if UL resource reallocation procedure in IDLE mode   */
                                   /*is released before procedure completes                   */
  IDLE_REALLOC_REQUESTED,          /*set on reception of RM_UL_REALLOC_REQ                    */
  IDLE_REALLOC_WAITING_TO_CONNECT, /*set when PUA has been rcvd in response to the PRR sent   */
                                   /*as part of the ul resource reallocation procedure started*/
                                   /*in UL_1P_CONTENTION_RES state in idle mode               */
} idle_ul_realloc_state_t;

typedef struct
{
   /***** pda_ul_pacch **************************************************************************
   * 
   *   Used when PDA is rcvd on the UL pacch during the release of the UL tbf during 
   *   contention resolution          
   *
   *********************************************************************************************/
   boolean pda_ul_pacch;
   
   /***** idle_ul_realloc_state ****************************************************************
   *
   *  Used to track resource reallocation initiated in IDLE mode during 1 phase contention 
   *  resolution
   *
   *********************************************************************************************/     
   idle_ul_realloc_state_t    idle_ul_realloc_state;
   
   
   boolean                    pending_realloc_req;
   


   /***** rcvd_unsolicited_pua_waiting_for_connected_ind ****************************************
   *
   *  Used to track unsolicited pua in IDLE mode during 1 phase contention 
   *  resolution
   *
   *********************************************************************************************/     
   boolean                    rcvd_unsolicited_pua_waiting_for_connected_ind;
   
}idle_flags_t;

typedef enum
{
  PUA_CONT_RES_NOT_APPLICABLE,
  PUA_CONT_RES_PASSED,
  PUA_CONT_RES_FAILED,
  PUA_CONT_RES_ADDRESS_CHECK_FAILED
} pua_cont_res_t;

typedef enum
{
  PDA_CHECK_TLLI_ONLY,
  PDA_CHECK_TFI_AND_TLLI,
  PDA_NO_ADDRESS_CHECK
}pda_address_check_t;

 typedef enum
 {
  NO_RELEASE_CAUSE,
  ACCESS_RETRY,
  PARTIAL_SYS_ACQ,
  PAR_RCVD,
  CONT_RES_COMPLETE,
  REL_DL_TBF,
  T3164_EXP,
  PDA_INTERRUPTS_UL_TBF_EST,
  FAIL_TO_SEND_PMR_PCCF,
  UL_EXCEPTION_HANDLING,
  GRR_INITIATED_NO_ACCESS,
  REL_DL_SB_TBF,
  PDA_RCVD_IN_DL_SB_TBF,
  CONT_RES_UL_RE_ALLOC_FAILURE
 }release_cause_t;

typedef struct
{
 /*********************************************************************
  *     use of the global mac_l1_sig signal struture                   *
  * Mac_l1_sig is a global signal structure is used to store primarily *
  * data for  MAC_L1_xxxx_CONFIG type signals but can be used for any  *
  * mac to l1 signal. BUT if at anytime that the uses of this global   *
  * signal definition would cause corruption of previously store l1    *
  * config data then it is imperative that a local signal structure is *
  * defined and used to send the mac to l1 sig. This must be done so   *
  * that the integrity of the config signal is not corrupted.          *
  *********************************************************************/
  mac_l1_sig_t  mac_l1_sig;    /* mac to l1 signal   */
  mac_grr_sig_t mac_grr_sig;   /* mac to grr signals */

  /*number of ul tbf establishment access attempts for a single establishment request */
  uint8  access_attempts_count;
  uint8  encoded_ul_msg[MAX_SIZE_OF_CONTROL_BLOCK_DATA];

  access_type_e   access_granted;

  idle_flags_t i_flags;

  /*********************************************************************
  *                   signal sending from MAC                          *
  *********************************************************************/

  mac_rlc_msg_t    *mac_rlc_sig_ptr;        /* mac to rlc signals */
  mac_int_sig_t     mac_int_sig;            /* mac to mac signals */
  mr_ul_alloc_cnf_t mr_ul_alloc_cnf_sig;    /*store for mr_ul_alloc_cnf sig data*/
  mr_dl_ass_ind_t   dl_ass_ind_sig;         /*store fo mr_dl_ass_ind sig data*/

  /***********************************************************************
  *     info received in signals that need to be stored for later use    *
  ************************************************************************/

  /*         indicates the currently idle mode sub state                 */
   gprs_mac_idle_substates_t mac_idle_substate;

   /*   the result of checking data in  PUA msg                           */
   pua_data_error_t  pua_data_result;

   /*   the result of checking data in  PDA msg                           */
   pua_data_error_t  pda_data_result;

   /*************************************************************************
   *                       queued ul est vars                               *
   **************************************************************************/

   /*************************************************************************
   *             MS addressing  vars                                        *
   **************************************************************************/
   uint32  rrps_tlli;    /*holds tlli read form public store*/

   /*************************************************************************
   *           contention resolution vars                                   *
   *************************************************************************/
   cont_res_success_t contention_result;

   /*************************************************************************
   *           tbf release vars                                             *
   *************************************************************************/

   /*indicates the release process to be imitated once L1_MAC_TBF_REL_CONFIRM*/
   /*has been received*/
   release_cause_t release_cause;

   /*************************************************************************
   *   used with cell change order to track ul data establishments          *
   *************************************************************************/

   /* used to hold T3168 time period read from GRR */
   uint8 t3168_timeout;

   /*store PDA rcvd in encode_ csn.1 format, needed to preserve data */
   /*integrity as a result of new MAC/L1 tbf release procedure*/

   uint8 csn1_encoded_pda_msg[SIZE_OF_DL_MSG];

   /*Use to store FN PDA is rcvd on when PDA interrupts UL est and we   */
   /*have to wait for the UL rel confirm from L1 before proceeding with */
   /*with the PDA est, used for relative starting time calculations     */
   uint32 gmac_store_rcvd_fn_of_pda;

   release_tbf_t tbf_rel;

   pua_cont_res_t egprs_pua_contention_result;
   packet_uplink_assignment_t egprs_cont_res_pua;

   /* temp_egprs_2p_sb_mb_alloc is the type of allocation originally used to TX the PRR.
     This temp variable is set here because egprs_2p_sb_mb_alloc is over-written in
     gmac_idle_build_rlc_alloc_and_l1_config_sigs()

      This is to handle ARAC retransmission after having sent the previous PRR from a single
      block or Multiblock (1 block). see 04.60 7.1.3.2.1 */
   egprs_2p_sb_mb_alloc_t temp_egprs_2p_sb_mb_alloc;

   /* Used during contention resolution to count the number ul relocation request attempts */
   /* that have made                                                                       */
   /* var is RESET when RM_UL_REALLOC_REQ is rcvd and incremented when PRR has been        */
   /* physically TX-ed ie when  L1_MAC_SINGLE_BLOCK_SENT is rcvd                           */
   uint8 idle_realloc_attempts_count;
} gmac_idle_data_t;

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Data References For Idle and Idleutil only
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_idle_puan_handler() 
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
cont_res_success_t gmac_idle_puan_handler(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_idle_pda_handler() 
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
pua_data_error_t gmac_idle_pda_handler(mr_dl_ass_ind_t *dl_ass_ind_sig_ptr,
                                       boolean send_frq_config,
                                       pda_address_check_t address_check,
                                       gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_ul_est_access_granted() 
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
access_type_e gmac_ul_est_access_granted(access_type_e rlc_access_requested, 
                                         gprs_mac_action_t action,
                                         gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_dl_msg_data_valid() 
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
boolean gmac_dl_msg_data_valid(gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      gmac_util_send_mac_grr_mac_status() 
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
void gmac_util_send_mac_grr_mac_status(mac_grr_sig_t *grr_sig,
                                       rr_mac_status_t status,
                                       gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      gmac_idle_send_tbf_config_sigs() 
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
void gmac_idle_send_tbf_config_sigs
(
mac_l1_sig_t *l1_sig_ptr,
mac_rlc_msg_t *rlc_sig_ptr, 
mac_l1_sig_id_t l1_sig_id,
mr_ul_alloc_cnf_t *mr_ul_alloc_cnf_sig_ptr,
gas_id_t gas_id
);

/*===========================================================================
===
===  FUNCTION      gmac_idle_send_mr_ul_alloc_cnf() 
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
void gmac_idle_send_mr_ul_alloc_cnf
(
mac_rlc_msg_t *rlc_sig_ptr, 
mr_ul_alloc_cnf_t *mr_ul_alloc_cnf_sig_ptr,
gas_id_t gas_id
);

/*===========================================================================
===
===  FUNCTION      gmac_idle_send_mac_internal_sig() 
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
void gmac_idle_send_mac_internal_sig
(
mac_int_sig_t *mac_sig,
mac_mac_sig_id_t mac_sig_id,
gas_id_t gas_id
);

/*===========================================================================
===
===  FUNCTION      gmac_idle_send_l1_release_sig() 
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
void gmac_idle_send_l1_release_sig(mac_l1_sig_t *l1_sig_ptr,
                                   release_tbf_t release_type,
                                   mac_log_ul_tbf_release_t ul_failure_reason,
                                   mac_log_dl_tbf_release_t dl_failure_reason,
                                   gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_idle_check_pua_addressing() 
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
boolean gmac_idle_check_pua_addressing
(
  idle_mode_ul_tbf_release_action_t rel_action,
  gprs_mac_idle_substates_t current_idle_substate,
  gas_id_t gas_id
);

/*===========================================================================
===
===  FUNCTION      check_idle_par_addressing() 
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
boolean check_idle_par_addressing(access_type_e access_type,gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION      gmac_idle_build_rlc_alloc_and_l1_config_sigs() 
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
pua_data_error_t gmac_idle_build_rlc_alloc_and_l1_config_sigs
(
uint8 ul_tbf_type,
gprs_mac_action_t current_action,
mr_ul_alloc_cnf_t *mr_ul_alloc_cnf_sig_ptr,
gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_idle_reset_parameters() 
===
===  DESCRIPTION
===
===  While mac_action is not NONE, timers and stored parameters need to be cleared 
===  when mac_action is reset. 
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
=== 
===========================================================================*/
void gmac_idle_reset_parameters(gprs_mac_idle_substates_t current_idle_substate, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_idle_send_pmr_cnf_to_grr() 
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
void gmac_idle_send_pmr_cnf_to_grr(success_failure_t result, gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION      gmac_idle_packet_tbf_release() 
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===========================================================================*/
release_tbf_t gmac_idle_packet_tbf_release (boolean int_ul_tbf, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_idle_abnormal_release_handler() 
===
===  DESCRIPTION 
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===========================================================================*/
void gmac_idle_abnormal_release_handler
(
gprs_mac_action_t  current_mac_action,
gprs_mac_idle_substates_t current_mac_state,
gas_id_t gas_id
);

/******************************************************************************
*                               EGPRS FUNCTIONS                               *
*******************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_idle_pua_contention_res_checking() 
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
pua_cont_res_t gmac_idle_egprs_pua_contention_res_checking(gprs_mac_idle_substates_t idle_substate,
                                                           gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_idle_send_no_access_to_rlc() 
===
===  DESCRIPTION
===  This functin will block RLC and also sends NO_PS_ACCESS IND with delete/no_delete.
===  Delete/no_delete will be determined depending upon number of access attemps or if PCCO 
===  procedure is on GMAC always sends Delete
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
void gmac_idle_send_no_access_to_rlc(gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION    gmac_idle_stop_access_timers() 
===
===  DESCRIPTION
===  This function will stop the access timers depending upon the mac idle substate
===
===  PARAMS
===   mac_idle_substate --- This is the current mac idle sub state
===
===  RETURN VALUE
===
===    
===  SIDE EFFECTS
=== 
=== 
===========================================================================*/
void gmac_idle_stop_access_timers(gprs_mac_idle_substates_t mac_idle_substate,
                                  gas_id_t gas_id);
/*===========================================================================
===
===  FUNCTION    gmac_idle_send_del_one_or_no_del_to_rlc() 
===
===  DESCRIPTION
===  This functin will send del or no del to rlc depending upon pcco active or not.
===  if pcco is active and establishment cause is GMM CELL UPDATE, gmac will send
===  del one to RLC. For all other establishment causes gmac will send NO DEL to GRLC.
===  if pcco is not active, gmac will always sends DEL ONE to GRLC
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
void gmac_idle_send_del_one_or_no_del_to_rlc(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_get_idle_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to idle data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_idle_data_t*
=== 
===  SIDE EFFECTS
===
===
===========================================================================*/
gmac_idle_data_t* gmac_get_idle_data_ptr(gas_id_t gas_id);

#endif /* INC_GMACIDLE_H */
/*** EOF: don't remove! ***/
