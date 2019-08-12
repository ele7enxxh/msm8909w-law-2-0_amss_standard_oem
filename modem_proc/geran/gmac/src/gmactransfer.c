/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TRANSFER MODE MODULE
***
***
*** DESCRIPTION
***
***  This module administrates the signal reception and the associated actions
***  for TRANSFER mode states.
***
*** EXTERNALIZED FUNCTIONS
***
***  xxx
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  xxx
***
*** Copyright (c) 2001,2002,2003,2004,2005-2013 Qualcomm Technologies, Inc.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmactransfer.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 16/11/01   sb      Created
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#include "grlcx.h"
#include "gmacdef.h"
#include "gmacmac.h"
#include "gmacrb.h"
#include "gmactransfer.h"
#include "gmactran_ul.h"
#include "gmactran_dl.h"
#include "gmactran_c.h"
#include "gmacutil.h"
#include "gmaclog.h"
#ifdef FEATURE_GPRS_PS_HANDOVER
#include "gmacnull.h"
#endif /*FEATURE_GPRS_PS_HANDOVER*/
#ifdef FEATURE_GSM_EDTM
#include "grlcmac.h"
#include "gmac.h"
#endif /*FEATURE_GSM*/

#ifdef PERLUTF
#error code not present
#endif
#include "gmacproc.h"

#include "geran_eng_mode_info.h"
#include "gmacds.h"
#include "stringl/stringl.h"


#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

#define GMAC_UPDATE_UL_TS_REALLOC (global_data_ptr->gmac_local_ul_ts_alloc = global_data_ptr->gmac_local_ul_realloc_ts)
#define GMAC_UPDATE_DL_TS_REASS (global_data_ptr->gmac_local_dl_ts_alloc = global_data_ptr->gmac_local_dl_reassign_ts)
#define GMAC_UPDATE_UL_DLCTRL_TS_REALLOC (global_data_ptr->gmac_local_ul_dl_ctrl_ts = global_data_ptr->gmac_local_ul_realloc_dl_ctrl_ts)

/*when PH_CONNET_IND Rcvd, update the stored RLC info          */
/*(gmac_transfer_ul_rlc_info_store) to have the correct UL_TFI */
#define GMAC_UPDATE_UL_TFI {global_data_ptr->gmac_local_tfi_store.ul_tfi = global_data_ptr->gmac_re_assign_ul_tfi; \
                            if(global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE)\
                            {\
                              transfer_data_ptr->gmac_transfer_ul_rlc_info_store.ul_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;\
                            }\
                           }

#define GMAC_UPDATE_DL_TFI { global_data_ptr->gmac_local_tfi_store.dl_tfi = global_data_ptr->gmac_re_assign_dl_tfi;}

/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/
typedef enum
{
  GMAC_L1_CNF_NOT_SET,
  GMAC_SEND_TS_RECONF_TO_L1,
  GMAC_SEND_DL_CONF_TO_L1,
  GMAC_SEND_DYN_CONF_TO_L1
} gmac_freq_cnf_l1_cnf_msg_t;

typedef enum
{
  GMAC_RLC_CNF_NOT_SET,
  GMAC_NO_RLC_ASS_MSG,
  GMAC_SEND_DL_ASS_TO_RLC
} gmac_freq_cnf_rlc_ass_msg_t;

#ifdef FEATURE_GSM_EDTM
typedef enum
{
  EDTM_FIRST_PKT_CS_REQUEST,
  EDTM_PKT_CS_REQUEST_RETRY,
  EDTM_PKT_CS_REQ_FROM_CONT_RES
} mac_edtm_pkt_cs_req_retry_t;

typedef enum
{
  EDTM_CLEAR_PENDING_TBF,
  EDTM_CLEAR_ACTIVE_TBF
} mac_edtm_tbf_clear_action_t;

typedef enum
{
  PCRI_PASS,
  PCRI_IGNORE_MSG,
  PCRI_ABNORMAL_REL
} gmac_pkt_cs_rel_result_t;
#endif /*FEATURE_GSM_EDTM*/

/*****************************************************************************
***
***     Public Data Declarations
***
***     Public data is only for transfer mode.  The externs shall be in gmactransfer.h
***     and will only be included by gmactran_ul.c gmactran_dl.c gmactran_c.c
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/
#ifdef PERLUTF
#error code not present
#else
static gmac_transfer_data_t gmac_transfer_data[NUM_GERAN_DATA_SPACES];
#endif /*PERLUTF*/

/*****************************************************************************
***
***     Public Functions Prototypes
***
*****************************************************************************/
/*===========================================================================
===
===  FUNCTION    gmac_get_transfer_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to transfer data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_transfer_data_t*
===
===  SIDE EFFECTS
===
===
===========================================================================*/
gmac_transfer_data_t* gmac_get_transfer_data_ptr(gas_id_t gas_id)
{
  gmac_transfer_data_t *transfer_data_ptr = &gmac_transfer_data[0];

  /* validate the GAS ID */
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    transfer_data_ptr = &gmac_transfer_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)];
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Transfer bad gas_id:%d",gas_id);
  }

  return transfer_data_ptr;
}

/*****************************************************************************
***
***     Private Functions Prototypes
***
*****************************************************************************/
static void gmac_transfer_send_freq_cnf_msgs( gmac_freq_cnf_rlc_ass_msg_t rlc_ass_msg,
                                              gmac_freq_cnf_l1_cnf_msg_t  l1_cnf_msg,
                                              gas_id_t                    gas_id );

#ifdef FEATURE_GPRS_PS_HANDOVER
static void gmac_tran_send_gl1_psho_cancel_pending_config(gas_id_t gas_id);
static void gmac_tran_send_gl1_psho_status(psho_status_t psho_status, gas_id_t gas_id);
static void gmac_tran_send_grlcul_psho_complete_ind(psho_status_t psho_status, gas_id_t gas_id);
static void gmac_tran_send_grlcdl_psho_complete_ind(psho_status_t psho_status, gas_id_t gas_id);
static void gmac_tran_psho_trigger_pccf_sending(gprs_mac_transfer_substates_t transfer_substate, gas_id_t gas_id);
static void gmac_tran_ps_handover_data_ind(gas_id_t gas_id);
static void gmac_tran_store_tbf_rel_cnf(gas_id_t gas_id);
static void gmac_tran_store_rlc_release_ind(uint8 rlc_mac_sig, gas_id_t gas_id);
static void gmac_tran_dl_process_rel_cnf(tbf_released_t tbf_released, gas_id_t gas_id);
static void gmac_tran_ul_dynamic_process_rel_cnf(tbf_released_t tbf_released, gas_id_t gas_id);
static void gmac_tran_concurrent_process_rel_cnf(tbf_released_t tbf_released, gas_id_t gas_id);
static void gmac_tran_psho_process_saved_msgs(gprs_mac_transfer_substates_t transfer_substate, gas_id_t gas_id);
static gprs_mac_transfer_substates_t gmac_tran_ignore_pending_tbfs(gprs_mac_transfer_substates_t transfer_substate, gas_id_t gas_id);
static void gmac_tran_psho_restart_timers(gprs_mac_transfer_substates_t gmac_transfer_substate, gas_id_t gas_id);
#endif /*FEATURE_GPRS_PS_HANDOVER*/

static void gmac_ul_transfer_init(gas_id_t gas_id);

static void gmac_transfer_tbf_rel_confirm(tbf_released_t tbf_rel, gas_id_t gas_id);


#ifdef FEATURE_GSM_DTM
static void gmac_dtm_chan_rel_handler( gm_dtm_channel_release_ind_t *chan_rel_ptr,
                                       tbf_released_t     tbf_rel,
                                       gas_id_t           gas_id );

static dtm_ass_status_t gmac_tran_dtm_chan_ass_handler(
                           gm_dtm_channel_assign_req_t     *dtm_ass_ptr,
                           mr_ul_alloc_cnf_t               *mr_ul_alloc_cnf_sig_ptr,
                           dtm_tbf_type_t                   current_tbf_assigned,
                           gas_id_t                         gas_id );

static void gmac_tran_rel_state_handle_rel_cnf( tbf_released_t tbf_released, gas_id_t gas_id );

static void gmac_tran_rel_state_handle_connect_ind( uint8 tbf_type_connected, gas_id_t gas_id );

#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
static void gmac_transfer_edtm_pkt_cs_req_handler(active_tbf_t                active_tbf,
                                                  uint8                       rr_est_cause,
                                                  mac_edtm_pkt_cs_req_retry_t cs_req_retry,
                                                  gas_id_t                    gas_id);

static void gmac_transfer_edtm_t3196_expiry_handler(gas_id_t gas_id);

static void gmac_transfer_edtm_ph_cs_connect_handler(gas_id_t gas_id);

static void gmac_transfer_edtm_clear_dl_tbf( mac_edtm_tbf_clear_action_t tbf_action, gas_id_t gas_id );

static void gmac_transfer_edtm_clear_ul_tbf( mac_edtm_tbf_clear_action_t tbf_action, gas_id_t gas_id );

static void gmac_transfer_edtm_restore_tbf_data( active_tbf_t restore_tbf_type, gas_id_t gas_id);

static void gmac_tran_edtm_ass_cmd_handler( gprs_mac_transfer_substates_t *transfer_state,
                                            dtm_tbf_type_t                 dtm_tbf_type,
                                            gas_id_t                       gas_id
                                          );

static void gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id_t gas_id);

static void gmac_tran_send_mac_l1_edtm_cs_rel_tbf_alloc_msg(  mac_l1_edtm_cs_rel_tbf_alloc_t
                                                             *mac_l1_cs_rel_msg,
                                                              gas_id_t gas_id );

static gmac_pkt_cs_rel_result_t gmac_tran_edtm_pkt_cs_rel_msg_handler(packet_cs_release_ind_t *pcri_ptr,
                                                                      active_tbf_t             active_tbfs,
                                                                      boolean                  ctrl_ack_set,
                                                                      gas_id_t                 gas_id);

static void gmac_tran_edtm_handle_coding_scheme_change(packet_cs_release_ind_t *pcri_ptr, gas_id_t gas_id);

static void gmac_tran_edtm_populate_cs_rel_config_msgs(packet_cs_release_ind_t *pcri_ptr,
                                                       boolean                  ctrl_ack_set,
                                                       gas_id_t                 gas_id);

static gmac_pkt_cs_rel_result_t gmac_tran_edtm_validate_pkt_cs_rel_msg(packet_cs_release_ind_t *pcri_ptr,
                                                                       active_tbf_t             active_tbfs,
                                                                       boolean                  ctrl_ack_set,
                                                                       gas_id_t                 gas_id);

static boolean gmac_get_cs_from_pkt_cs_rel(packet_cs_release_ind_t *pcri_ptr,
                                           uint8                   *cs,
                                           gas_id_t                 gas_id);

static boolean gmac_get_egprs_mcs_from_pkt_cs_rel(packet_cs_release_ind_t *pcri_ptr,
                                                  uint8                   *egprs_mcs,
                                                  gas_id_t                 gas_id);

static boolean gmac_get_ul_ws_from_pkt_cs_rel(packet_cs_release_ind_t *pcri_ptr,
                                              uint8                   *ul_ws,
                                              gas_id_t                 gas_id);

static boolean gmac_get_dl_ws_from_pkt_cs_rel(packet_cs_release_ind_t *pcri_ptr,
                                              uint8                   *dl_ws,
                                              gas_id_t                 gas_id);

static boolean gmac_get_usf_from_pkt_cs_rel(packet_cs_release_ind_t        *pcri_ptr,
                                            mac_l1_edtm_cs_rel_tbf_alloc_t *edtm_cs_realloc_ptr,
                                            gas_id_t                        gas_id);

static boolean gmac_get_ul_ts_alloc_from_pkt_cs_rel(packet_cs_release_ind_t *pcri_ptr,
                                                    uint8                   *ts_alloc,
                                                    gas_id_t                 gas_id);

static boolean gmac_edtm_pkt_cs_rel_tbfs_releasing(active_tbf_t active_tbfs,
                                                   gas_id_t     gas_id);

static void gmac_tran_edtm_cs_rel_handle_glob_pkt_ta(global_pkt_timing_t glob_pkt_ta,
                                                     gas_id_t            gas_id);

#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/
#endif /*FEATURE_GSM_EDTM*/
#ifdef FEATURE_GPRS_PS_HANDOVER
static void gmac_transfer_initiate_local_release_of_old_cell_dl_tbfs(gas_id_t gas_id);
#endif /*FEATURE_GPRS_PS_HANDOVER*/
static void gmac_tran_handle_unexpected_rlc_tbf_req( gas_id_t gas_id );

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_transfer_init
===
===  DESCRIPTION
===  Initialise information applicable to uplink transfer
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
static void gmac_ul_transfer_init(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  transfer_data_ptr->gmac_transfer_glob_final_ack_ind = RM_ACKNACK_FAI_TBF_NOT_COMPLETE;
  global_data_ptr->gmac_transfer_ul_cv_zero_ind = FALSE;
  transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;
  transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_NORMAL;
  global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
}

/*===========================================================================
===
===  FUNCTION    gmac_init_transfer_data()
===
===  DESCRIPTION
===  Initialises the transfer data, used for lower layer resets.
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
void gmac_init_transfer_data( gas_id_t gas_id )
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  gmac_init_transfer_data_non_zero(gas_id);

  transfer_data_ptr->gmac_transfer_glob_final_ack_ind = 0;
  transfer_data_ptr->gmac_transfer_ul_request_count = 0;

  memset( &transfer_data_ptr->gmac_transfer_store_dl_tbf_msg, 0, sizeof(transfer_data_ptr->gmac_transfer_store_dl_tbf_msg) );
  memset( &transfer_data_ptr->gmac_transfer_store_ts_reconfig, 0, sizeof(transfer_data_ptr->gmac_transfer_store_ts_reconfig) );
  memset( &transfer_data_ptr->gmac_transfer_store_l1_dyn_realloc, 0, sizeof(transfer_data_ptr->gmac_transfer_store_l1_dyn_realloc) );
  memset( &transfer_data_ptr->gmac_transfer_ul_rlc_info_store, 0, sizeof(transfer_data_ptr->gmac_transfer_ul_rlc_info_store) );
  transfer_data_ptr->pua_ptr_cs_change = 0;
  memset( &transfer_data_ptr->t_flags, 0, sizeof(transfer_data_ptr->t_flags) );
  memset( &transfer_data_ptr->gmac_local_pts_params, 0, sizeof(transfer_data_ptr->gmac_local_pts_params) );
  memset( &transfer_data_ptr->mac_l1_sig_cs_reconfig, 0, sizeof(transfer_data_ptr->mac_l1_sig_cs_reconfig) );
  transfer_data_ptr->mr_ul_alloc_cnf_ptr = NULL;
  transfer_data_ptr->mr_dl_test_mode_ind_ptr = NULL;
  transfer_data_ptr->t3168_time = 0;
  memset( &transfer_data_ptr->gmac_transfer_local_pccf_params, 0, sizeof(transfer_data_ptr->gmac_transfer_local_pccf_params) );
#ifdef FEATURE_GSM_EDTM
  transfer_data_ptr->mac_tran_substate_str = NULL;
  memset( &transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg, 0, sizeof(transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg) );
#endif /*FEATURE_GSM_EDTM*/
}

/*===========================================================================
===
===  FUNCTION    gmac_init_transfer_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) transfer data.
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
void gmac_init_transfer_data_non_zero( gas_id_t gas_id )
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  transfer_data_ptr->mac_transfer_substate = TRANSFER_INACTIVE;
  transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_NORMAL;
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_tbf_rel_confirm
===
===  DESCRIPTION
===  When releasing TBF, wait for TBF release confirmation from L1 before
===  returning to NULL/IDLE.
===
===  If T3164 has expired, need to return to IDLE to retry the access
===  attempt, otherwise leave transfer and return to normal NULL/IDLE state.
===
===  If cell reselection, return to NULL_INACTIVE state until
===  the GRR_MAC_PS_ACCESS_REQ signal is received.
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

static void gmac_transfer_tbf_rel_confirm(tbf_released_t tbf_rel, gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_MED_0_G("GMAC State tran = T_INACTIVE");
  transfer_data_ptr->mac_transfer_substate = TRANSFER_INACTIVE;

  /* Reset for cases where RLC UL normal release is interrupted by an abnormal
     release (e.g. GRR_NO_PS_ACCESS_IND or T3180 expiry) */
  transfer_data_ptr->t_flags.ul_tbf_normal_release_ind = FALSE;

  global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;

#ifdef FEATURE_GSM_DTM
  MSG_GERAN_MED_0_G("GMAC Rel State, gmac_tran_rel_state set to NO_REL");
  transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
#endif /*FEATURE_GSM_DTM*/

  /* Transfer has gone inactive so the TBF parameters can be reset*/
  reset_tbf_parameters(gas_id);
  /*Reset this flag. this flag is set when gmac received PTR/PDA in T_CON_DYN state*/
  transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = FALSE;
#ifdef FEATURE_GSM_EDTM
   transfer_data_ptr->t_flags.edtm_realloc_in_progress = FALSE;
   transfer_data_ptr->t_flags.edtm_pcri_rel_dl_tbf_for_ctrl_ack = FALSE;

   /*Reset this flag under FEATURE flag to limit side effects*/
   /*This flag is not reset properly when MAC suspends in T_DL_WITH_DYN state*/
   transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;
#endif /*FEATURE_GSM_EDTM*/

  MSG_GERAN_MED_0_G("mac info wait_for_ul_rel_confirm = FALSE");
  global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;

  /*For ul normal relase ie PUAN with FAI, ul tfi valid flag is reset when l1 has */
  /*confirmed the relase its at this point that the UL tfi is invlaid*/
  if((tbf_rel == UL_DL_TBF_RELEASE_CONF )||(tbf_rel == UL_TBF_RELEASE_CONF))
  {
    global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
  }

  gmac_stop_t3164(gas_id);
  gmac_stop_t3164_transfer(gas_id);

  gmac_stop_t3180(gas_id);

  if(tbf_rel == UL_DL_TBF_RELEASE_CONF)
  {
    transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg = FALSE;
    MSG_GERAN_MED_1_G("MAC info abnormal_ul_and_dl_rel_flg =%d",transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg);
  }


  /*re-set flag when exiting transfer mode*/
  transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
  transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release = FALSE;
  transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_dl_release = FALSE;

  /*clear all ul control msg flags*/
  global_data_ptr->gmac_local_pccf_params.pccf_pending = FALSE;

  /*cr68663*/
  if (global_data_ptr->gmac_local_pmr_params.pmr_pending == TRUE)
  {
    gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE, gas_id);
  }
  transfer_data_ptr->gmac_local_pts_params.pts_pending = FALSE;

  global_data_ptr->gmac_local_pss_params.pss_pending = FALSE;

#ifdef FEATURE_GSM_EDTM
  /* If T3196 is still running then indicate Pkt CS Est Rej to GRR*/
  if( rex_get_timer(&mac_timers_ptr->timer_t3196) != 0 )
  {
    gmac_stop_t3196(gas_id);
    gmac_stop_edtm_pkt_cs_req_retry_timer(gas_id);
    gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_TBF_RELEASE_IN_PROGRESS, gas_id);

    /* reset pending edtm pkt cs req on RRBP */
    memset(&global_data_ptr->gmac_local_edtm_pcr_params,0,sizeof(gmac_edtm_pcr_params_t));
  }
#endif /*FEATURE_GSM_EDTM*/

  global_data_ptr->gmac_local_pccn_params.pccn_pending = NO_PENDING_PCCN;
  MSG_GERAN_MED_0_G(" pccn_pending = NO_PENDING_PCCN");

  /* Release any data gathered from Packet Serving Cell Data messages during the TBF */
  /* NOTE that any complete messages from the container have already ben sent to GRR */
  gmac_util_release_container(SERVING_CELL_CONTAINER, gas_id);

  switch (transfer_data_ptr->gmac_transfer_tbf_rel_cause)
  {
    case GMAC_TRANSFER_TBF_REL_NORMAL:

#ifdef FEATURE_GSM_EDTM
      /* For EDTM we might have come from Idle -> Transfer -> dual transfer mode.  In this case if
         the PS only is released then we want to return to Null not Idle because the CS is still active.
         Note. gmac_gprs_mode is also reset here for consistency. */
      if (transfer_data_ptr->t_flags.dtm_active == TRUE)
      {
          global_data_ptr->mac_mode = NULL_MODE;
          MSG_GERAN_MED_0_G("MAC Sent MAC_TN_EDTM_52MF_TO_51MF_CS_CALL_ACTIVE");
          global_data_ptr->mac_mac_sig.sig_id = MAC_TN_EDTM_52MF_TO_51MF_CS_CALL_ACTIVE;
          global_data_ptr->mac_mac_sig.task_id = MS_MAC_MAC;
          gprs_mac_mac_acc_func(&global_data_ptr->mac_mac_sig, gas_id);
      }
      else
#endif /*FEATURE_GSM_EDTM*/
      {
        global_data_ptr->mac_mode = NULL_MODE;
      }

      MSG_GERAN_MED_1_G("MAC set mode %d",global_data_ptr->mac_mode);
      (void)mac_log_state(gas_id);

      if( rex_get_timer(&mac_timers_ptr->timer_t3172) != 0 )
      {
        MSG_GERAN_MED_0_G("GMAC T3172 running on exit from Transfer, leave RLC blocked");

        /* Check that RLC is still blocked */
        (void)gmac_util_check_rlc_blocked_flag(BLOCKED,GMAC_TRAN, __LINE__, gas_id);
      }
      else
      {
        if ( gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED )
        {
          gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
        }
      }

      gmac_util_send_grr_mac_status(MAC_IDLE, gas_id);
      break;

    case GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE:
      /* Case where GRR has initiated release via GRR_MAC_NO_PS_ACCESS_IND or GRR_MAC_DTM_CHANNEL_RELEASE_IND */
      global_data_ptr->mac_mode = NULL_MODE;
      MSG_GERAN_MED_1_G("MAC set mode %d",global_data_ptr->mac_mode);
      (void)mac_log_state(gas_id);

      gmac_util_send_grr_mac_status(MAC_IDLE, gas_id);

      MSG_GERAN_MED_0_G("MAC Sent MAC_TN_NO_SERVICE");
      global_data_ptr->mac_mac_sig.sig_id = MAC_TN_NO_SERVICE;
      global_data_ptr->mac_mac_sig.task_id = MS_MAC_MAC;
      gprs_mac_mac_acc_func(&global_data_ptr->mac_mac_sig, gas_id);
      break;

    default:
      if(geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_1_G("GMAC BAD tbf rel cause: 0x%x",transfer_data_ptr->gmac_transfer_tbf_rel_cause);
        MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
        geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
      }
      else
      {
        ERR_GERAN_FATAL_2_G("GMAC BAD tbf rel cause: 0x%x",gas_id, transfer_data_ptr->gmac_transfer_tbf_rel_cause);
      }
    break;
  }

#ifdef FEATURE_GSM_DTM
  /* MAC is leaving transfer state so DTM is no longer active*/
  transfer_data_ptr->t_flags.dtm_active = FALSE;
  transfer_data_ptr->t_flags.dtm_realloc_in_progress = FALSE;
#endif /*FEATURE_GSM_DTM*/

  global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;

  /*TLLI in*/
  global_data_ptr->gmac_store_dl_ass_tlli = (uint32)0xFFFFFFFF;

  global_data_ptr->gmac_store_ul_est_tlli = (uint32)0xFFFFFFFF;

  MSG_GERAN_HIGH_2_G("MAC info gmac_store_dl_ass_tlli %x, gmac_store_ul_est_tlli %x ",
                     global_data_ptr->gmac_store_dl_ass_tlli,global_data_ptr->gmac_store_ul_est_tlli);

  /* Reset TS to guard against the case where the UL TS is not reset before returning to IDLE.
     If it is not reset then we will get an error when following this sequence:
         state T-DL
         receive PUA move to T_DL_DYN_ALLOC
         receive PDA before PUA connects -> MS Class checking is made against the old PUA TS allocation!
  */

  /* Reset timeslot allocation */
  global_data_ptr->gmac_local_dl_ts_alloc = 0;
  global_data_ptr->gmac_local_ul_ts_alloc = 0;

  /* default release cause */
  transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_NORMAL;

  gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, global_data_ptr->current_tbf_mode, TRUE, gas_id);

}

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      gmac_init_transfer_flags
===
===  DESCRIPTION
===  Initilises the transfer flags, should only be called once at power on.
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
void gmac_init_transfer_flags( gas_id_t gas_id )
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  transfer_data_ptr->t_flags.ul_tbf_normal_release_ind = FALSE;
  transfer_data_ptr->t_flags.dl_tbf_normal_release_ind = FALSE;
  transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = FALSE;
  transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = FALSE;
  transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
  transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = FALSE;
  transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;
  transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
  transfer_data_ptr->t_flags.failed_to_send_pdan_chan_req = FALSE;
  transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg = FALSE;
  transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;
  transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
  transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release = FALSE;
  transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_dl_release = FALSE;
  transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = FALSE;
#ifdef FEATURE_GSM_EDTM
  transfer_data_ptr->t_flags.edtm_pcri_rel_dl_tbf_for_ctrl_ack = FALSE;
#endif /*FEATURE_GSM_EDTM*/
}


/*===========================================================================
===
===  FUNCTION      transfer_mode_handler
===
===  DESCRIPTION
===  Handles the signal reception and the associated actions
===  for transfer mode states.
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
void transfer_mode_handler(uint8 task_id, uint8 signal_id, gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  /*Logging*/
  if((task_id == MS_MAC_L1)
      && (signal_id == (uint8)MAC_PH_DATA_IND))
  {
    MSG_GERAN_MED_3_G("MAC info block dl msg(during ul,dl rel) =%d,%d state = %d",
                      transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release,
                      transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_dl_release,
                      transfer_data_ptr->mac_transfer_substate);
    /*TLLI in DL CNF to RLC*/
    MSG_GERAN_HIGH_2_G("MAC  gmac_store_dl_ass_tlli %x, gmac_store_ul_est_tlli %x",
                       global_data_ptr->gmac_store_dl_ass_tlli,global_data_ptr->gmac_store_ul_est_tlli);
  }

  switch(transfer_data_ptr->mac_transfer_substate)
  {
    /***********************************
    * transfer state TRANSFER_INACTIVE *
    ************************************/
    case TRANSFER_INACTIVE:
      if(task_id == MS_MAC_MAC)
      {
#ifdef FEATURE_GSM_DTM
        MSG_GERAN_MED_0_G("GMAC Rel State, gmac_tran_rel_state set to NO_REL");
        transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
#endif /*FEATURE_GSM_DTM*/

        switch (signal_id)
        {
#ifdef FEATURE_GSM_DTM
          case MAC_NT_UL_DTM_DYNAMIC:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_NT_UL_DTM_DYNAMIC");
            gmac_ul_transfer_init(gas_id);
            gmac_dl_transfer_init(gas_id);

            /* used in case t3164 expiry (reestablishment may be required) */
            transfer_data_ptr->gmac_transfer_ul_request_count++;

            transfer_data_ptr->t_flags.dtm_active = TRUE;
            MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYNAMIC");
            transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC;
            (void)mac_log_state(gas_id);
            gmac_reset_rac_params(gas_id);
            gmac_start_t3180(gas_id);

            gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, global_data_ptr->current_tbf_mode, FALSE, gas_id);

            break;

          case MAC_NT_DL_DTM_DYNAMIC:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_NT_DL_DTM_DYNAMIC");
            gmac_ul_transfer_init(gas_id);
            gmac_dl_transfer_init(gas_id);

            transfer_data_ptr->t_flags.dtm_active = TRUE;

            if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
            {
              if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED )
              {
                gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
              }
            }
            else
            {
              MSG_GERAN_HIGH_0_G("GMAC T3172 running on entry to Transfer, leave RLC blocked");
              (void)gmac_util_check_rlc_blocked_flag(BLOCKED,GMAC_TRAN, __LINE__, gas_id);
            }

            MSG_GERAN_MED_0_G("GMAC State tran = T_DL");
            transfer_data_ptr->mac_transfer_substate = TRANSFER_DL;
            (void)mac_log_state(gas_id);

            gmac_eng_mode_update_tbf_info((uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER, global_data_ptr->current_tbf_mode,FALSE, gas_id);

            break;

          case MAC_NT_UL_DL_DTM_DYNAMIC:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_NT_UL_DL_DTM_DYNAMIC");
            gmac_ul_transfer_init(gas_id);
            gmac_dl_transfer_init(gas_id);

            /* used in case t3164 expiry (reestablishment may be required) */
            transfer_data_ptr->gmac_transfer_ul_request_count++;
            transfer_data_ptr->t_flags.dtm_active = TRUE;
            MSG_GERAN_MED_0_G("GMAC State tran = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER");
            transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;
            (void)mac_log_state(gas_id);

            gmac_reset_rac_params(gas_id);
            gmac_start_t3180(gas_id);

            gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, global_data_ptr->current_tbf_mode,FALSE, gas_id);

          break;
#endif /*FEATURE_GSM_DTM*/


          case MAC_IT_UL_DYNAMIC:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_IT_UL_DYNAMIC");
            gmac_ul_transfer_init(gas_id);
            gmac_dl_transfer_init(gas_id);

            /*cr151736*/
            if(global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE)
            {
              /*As there is NO DL active, ensure that the DL TFI valid flag is correctly set*/
              MSG_GERAN_HIGH_1_G("GMAC info dl_tfi_valid = TRUE, dl_tfi = %d ",global_data_ptr->gmac_local_tfi_store.dl_tfi);
              global_data_ptr->gmac_local_tfi_store.dl_tfi_valid = FALSE;
            }

            /* if entering transfer from 2p access then t3164 will be running               */
            /* increment counter  in case t3164 expiry (reestablishment may be required)    */
            if( rex_get_timer(&mac_timers_ptr->timer_t3164) != 0 )
            {
              transfer_data_ptr->gmac_transfer_ul_request_count++;
            }
            else
            {
              /*If entering transfer from 1 phase, timer t3164 should not be running*/
              /*however we may be performing N resource reallocation request. update*/
              /*gmac_transfer_ul_request_count To reflect this so that TRANSFER can */
              /*perform the correct num of Retries if the resource re allocation    */
              /*procedure fails */
              transfer_data_ptr->gmac_transfer_ul_request_count = global_data_ptr->msg_ptr->mac_int_sig.msg.mac_it_ul_dynamic.idle_realloc_req_count;
            }

            if (global_data_ptr->msg_ptr->mac_int_sig.msg.mac_it_ul_dynamic.waiting_to_connect_realloc_flg  == TRUE)
            {
              /*vars gmac_re_assign_ul_tfi and gmac_local_ul_realloc_ts set when   */
              /*PUA was rcvd in 1 phase contention res state as part of the 1phase */
              /*ul realloc procedure                                               */
              MSG_GERAN_MED_0_G("GMAC infoWaiting for Con ind for re-alloc PUA rcvd in 1p cont res ");
              MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYNAMIC_REALLOC");
              transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC;
              (void)mac_log_state(gas_id);

              /*need to pass in MR_UL_ALOC_CNF to send in transfer*/
              transfer_data_ptr->gmac_transfer_ul_rlc_info_store
               = global_data_ptr->msg_ptr->mac_int_sig.msg.mac_it_ul_dynamic.rlc_realloc_data;


              /* waiting for ul realloc req started in idle to be connected. if there  */
              /* were a pending realloc request then set reallocation pending flag to  */
              /* be dealt with on Reception  of PH_CONNECT_IND                         */
              if (global_data_ptr->msg_ptr->mac_int_sig.msg.mac_it_ul_dynamic.pending_realloc_req == TRUE)
              {
                transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = TRUE;
              }

            }
            else
            {
              MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYNAMIC");
              transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC;

              (void)mac_log_state(gas_id);
            }
            gmac_reset_rac_params(gas_id);

#ifdef FEATURE_GSM_EDTM
            if( rex_get_timer(&mac_timers_ptr->timer_t3196) != 0 )
            {
              MSG_GERAN_MED_0_G("GMAC Handle T3196 timer running on entry to Transfer");
              gmac_transfer_edtm_pkt_cs_req_handler(
                UL_ONLY,
                global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
                EDTM_PKT_CS_REQ_FROM_CONT_RES,
                gas_id);
            }
#endif /*FEATURE_GSM_EDTM*/

            gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, global_data_ptr->current_tbf_mode, FALSE, gas_id);

            break;

          case MAC_IT_DL_DYNAMIC:
          case MAC_NT_DL_DYNAMIC:
            if(signal_id == (uint8)MAC_NT_DL_DYNAMIC)
            {
              MSG_GERAN_MED_0_G("MAC Rcvd MAC_NT_DL_DYNAMIC");
            }

            if(signal_id == (uint8)MAC_IT_DL_DYNAMIC)
            {
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_IT_DL_DYNAMIC");
            }

            gmac_ul_transfer_init(gas_id);
            gmac_dl_transfer_init(gas_id);

            /*cr151736*/
            if(global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE)
            {
              /*As there is NO UL active, ensure that the UL TFI valid flag is correctly set*/
              MSG_GERAN_HIGH_1_G("GMAC info ul_tfi_valid = TRUE, ul_tfi = %d ",global_data_ptr->gmac_local_tfi_store.ul_tfi);
              global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
            }


            if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
            {
              if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED )
              {
                gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
              }
            }
            else
            {
              MSG_GERAN_HIGH_0_G("GMAC T3172 running on entry to Transfer, leave RLC blocked");
              (void)gmac_util_check_rlc_blocked_flag(BLOCKED,GMAC_TRAN, __LINE__, gas_id);
            }

            MSG_GERAN_MED_0_G("GMAC State tran = T_DL");
            transfer_data_ptr->mac_transfer_substate = TRANSFER_DL;

            gmac_reset_rac_params(gas_id);

            (void)mac_log_state(gas_id);

            gmac_eng_mode_update_tbf_info((uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER, global_data_ptr->current_tbf_mode,FALSE, gas_id);

            break;

          case MAC_IT_UL_DYN_EGPRS_PUA_RCVD_IN_CONT_RES:

            /************************************************************************
            * case where a PUA is rcvd during egprs 1 phase cont res which contains *
            * CONTENTION_RESOLUTION_TLLI field that results in contention res being *
            * completed successful. IDLE completes cont res process and switches to *
            * transfer to handle the PUA assignment.                                *
            *                                                                       *
            * The csn1 encoded PUA rcvd in IDLE,sent via the internal sig. gmactasks*
            * runs it throught csn1 decode and transfer on recpetion of the internal*
            * signal perfroms the functionality as if the original PUA had been rcvd*
            * in transfer mode.                                                     *
            ************************************************************************/

            MSG_GERAN_MED_0_G("MAC Rcvd MAC_IT_UL_DYN_EGPRS_PUA_RCVD_IN_CONT_RES");
            gmac_ul_transfer_init(gas_id);
            gmac_dl_transfer_init(gas_id);
            (void)gmac_util_check_rlc_blocked_flag(UNBLOCKED,GMAC_TRAN, __LINE__, gas_id);

            /* if entering transfer from 2p access then t3164 will be running               */
            /* increment counter  in case t3164 expiry (reestablishment may be required)    */
            if( rex_get_timer(&mac_timers_ptr->timer_t3164) != 0 )
            {
              transfer_data_ptr->gmac_transfer_ul_request_count++;
            }
            else
            {
              /*If entering transfer from 1 phase, timer t3164 should not be running*/
              /*however we may be performing N resource reallocation request. update*/
              /*gmac_transfer_ul_request_count To reflect this so that TRANSFER can */
              /*perform the correct num of Retries if the resource re allocation    */
              /*procedure fails */
              transfer_data_ptr->gmac_transfer_ul_request_count = global_data_ptr->msg_ptr->mac_int_sig.msg.mac_it_ul_epua_rcvd.idle_realloc_req_count;
            }

            MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYNAMIC");
            transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC;
            (void)mac_log_state(gas_id);

            gmac_reset_rac_params(gas_id);

#ifdef FEATURE_GSM_EDTM
            if( rex_get_timer(&mac_timers_ptr->timer_t3196) != 0 )
            {
              MSG_GERAN_MED_0_G("GMAC Handle T3196 timer running on entry to Transfer");
              gmac_transfer_edtm_pkt_cs_req_handler(
                UL_ONLY,
                global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
                EDTM_PKT_CS_REQ_FROM_CONT_RES,
                gas_id);
            }
#endif /*FEATURE_GSM_EDTM*/

            /*Note PUA must have been addressed correctly to have got here, */
            /*when gmac_tdynamic_handle_egprs_cont_res_pua() is called there*/
            /*is no need to pefrom the address checking                     */

            if (gmac_tdynamic_handle_egprs_cont_res_pua(global_data_ptr->msg_ptr->mac_int_sig.msg.mac_it_ul_epua_rcvd.egprs_c_r_pua_ptr, gas_id))
            {
              gmac_stop_t3168(gas_id);
              MSG_GERAN_MED_0_G("MAC transfer state T_UL_DYNAMIC_REALLOC");
              transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC;


              /* set reallocation pending flag to be dealt with on receipt of PH_CONNECT_IND */
              if (global_data_ptr->msg_ptr->mac_int_sig.msg.mac_it_ul_epua_rcvd.pending_realloc_req == TRUE)
              {
                transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = TRUE;
              }

              if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
              {
                gmac_transfer_simulate_freq_cnf(gas_id);
              }
              (void)mac_log_state(gas_id);
            }
          break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_MAC signals */
      break; /* end of TRANSFER_INACTIVE */

    /*****************************
    * transfer state TRANSFER_DL *
    *****************************/
    case TRANSFER_DL:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_DATA_IND:
            if(transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_dl_release == FALSE)
            {
              gmac_transfer_dl_data_ind(gas_id);
            }
            break;

          case L1_MAC_QUAL_MEASURMENTS:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_QUAL_MEASURMENTS");
            gmac_transfer_dl_pkt_acknack(&(global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements), gas_id);
            break;

          case L1_MAC_TBF_REL_CONFIRM:
#ifdef FEATURE_GPRS_PS_HANDOVER
            gmac_tran_dl_process_rel_cnf(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released, gas_id);
#else /*FEATURE_GPRS_PS_HANDOVER*/

            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
            gmac_tran_rel_state_handle_rel_cnf( global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released, gas_id );
#endif /*FEATURE_GSM_DTM*/

            transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_dl_release = FALSE;

            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF)
            {
              gmac_transfer_tbf_rel_confirm(DL_TBF_RELEASE_CONF, gas_id);
            }
            else
            {
              /* This can only happen if PH_CONNECT_IND DL is received in T_UL_DYN_DL_ASS
                 before the REL_CONF for UL during normal UL release.  T3172 cannot be started
                 in T_UL_DYN_DL_ASS so should not be running.

                 Following pre IPA design here.  RLC should have already been released in T_UL_DYN_DL_ASS
                 when the PH_CONNECT_IND (DL) was received, but old design maintained with an additional
                 call to unblock RLC here.
              */
              gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
              global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;
            }
#endif /*FEATURE_GPRS_PS_HANDOVER*/
            break;

          case L1_MAC_RRBP_FAILED_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_RRBP_FAILED_IND");
            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.rrbp_failed_ind.rrbp_contained_chan_des == TRUE)
            {
              gmac_stop_t3168(gas_id);
              transfer_data_ptr->t_flags.failed_to_send_pdan_chan_req = TRUE;
              gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__, gas_id);
              gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN,__LINE__, gas_id);
            }
            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND");
            gmac_transfer_edtm_ph_cs_connect_handler(gas_id);
          break;

          case L1_MAC_FREQ_ASS_CNF:
            if ( global_data_ptr->gmac_edtm_enabled &&
                 transfer_data_ptr->t_flags.edtm_realloc_in_progress )
            {
              MSG_GERAN_MED_0_G("GMAC L1_MAC_FREQ_ASS_CNF rxd for EDTM in TRANSFER_DL");
            }
            else
            {
              MSG_GERAN_ERROR_0_G("GMAC L1_MAC_FREQ_ASS_CNF not expected in TRANSFER_DL");
            }
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_L1 signals */
      else if(task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_DL_PKT_ACKNACK_REQ:
            MSG_GERAN_LOW_0_G("MAC Rcvd RM_DL_PKT_ACKNACK_REQ");
            gmac_transfer_dl_pdan_handler(&(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_acknack_req),FALSE, gas_id);
            break;

          case RM_DL_RELEASE_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd RM_DL_RELEASE_IND %d",global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause);

            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause == REL_DL_NORMAL)
            {
              gmac_stop_t3168(gas_id);
              if (transfer_data_ptr->t_flags.gmac_transfer_ul_req)
              {
                MSG_GERAN_LOW_2_G("MAC info UL REQ CNT %d PEND %d",
                  transfer_data_ptr->gmac_transfer_ul_request_count,
                  transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending);

                gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,DONT_REL_RLC_DL,NO_DEL,FALSE, gas_id);

                mac_log_dl_tbf_release(MAC_LOG_DL_RELEASE_IND, gas_id);
              }
              else
              {
                gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__,gas_id);
                gmac_transfer_dl_release_clean(gas_id);
                gmac_transfer_dl_normal_release_to_idle(gas_id);
              }
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,DONT_REL_RLC_DL,NO_DEL,FALSE,gas_id);

              mac_log_dl_tbf_release(MAC_LOG_DL_RELEASE_IND,gas_id);
            }
            break;

          case RM_DL_PKT_CTRL_ACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_CTRL_ACK_REQ");
            transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = TRUE;
            gmac_rrbp_send_pca(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.frame_number,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.time_slot, TRUE,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.rrbp, PCA_DL_FAI_MSG,
                               gas_id);
            break;

          case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");


            if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__,gas_id) == BLOCKED )
            {
              /* RLC has attempted access when we have blocked it.  This might
                 be a race condition or an error in RLC.
                 Because a race condition is a possibility we have to assume that this has happened
                 in this case */
              MSG_GERAN_HIGH_0_G("GMAC RLC attempted access whilst blocked, ignore msg, assume race condition");
            }
            else if (global_data_ptr->c_flags.tbf_release_pending_flg)
            {
              /* RLC should have been blocked if we are pending a TBF release */
              MSG_GERAN_ERROR_0_G("GMAC tbf_release_pending_flg set, RLC not blocked in T_DL, Ignore RLC req");
            }
            else
            {
              /* store RLC parameters to be added to PDAN */

              /*TLLI in UL CNF to RLC*/
              global_data_ptr->gmac_store_ul_est_tlli =  global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.tlli;
              MSG_GERAN_HIGH_2_G("MAC info UL est tlli %x, gmac_store_ul_est_tlli %x",
                                 global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.tlli,global_data_ptr->gmac_store_ul_est_tlli);

              global_data_ptr->gmac_local_stored_ul_chan_req = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.chan_req_desc;

              transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = TRUE;
              transfer_data_ptr->t_flags.gmac_transfer_ul_req = TRUE;

              /*For establishing an UL TBF via DL MAC is to turn off rlc UL est guard timer */
              /*as the protocol timer t3168 expiry and 4 access retry  is the mechanism     */
              /*that is provided by the protocol to guard against no response from the N/W  */
              /*to UE's request to establish an UL                                          */
              grlc_ul_stop_await_ass_guard(gas_id);

              if(transfer_data_ptr->t_flags.failed_to_send_pdan_chan_req == TRUE)
              {
                 /*don't alter the number of attempts made to send the channel request*/
                 transfer_data_ptr->t_flags.failed_to_send_pdan_chan_req = FALSE;
              }
              else
              {
                transfer_data_ptr->gmac_transfer_ul_request_count = 0;
              }

              global_data_ptr->current_test_mode = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.test_mode;

              global_data_ptr->gmac_test_mode_ts_offset = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.ts_offset;

              global_data_ptr->gmac_test_mode_srb_mode = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.srb_mode;

              if ( global_data_ptr->current_test_mode != TEST_MODE_OFF )
              {
                MSG_GERAN_HIGH_1_G("MAC DEBUG, RLC set TEST MODE to: %d", global_data_ptr->current_test_mode);
              }

              if ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.pfi_present )
              {
                global_data_ptr->pfi_present = TRUE;
                global_data_ptr->pfi = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.pfi;
              }
              else
              {
                global_data_ptr->pfi_present = FALSE;
              }
            }
            break;

          case RM_DL_FIRST_BLOCK_IND:
               /*ignore this signal when rcvd in transfer*/
            break;

          case RM_PS_ACCESS_CNF:
             /*ignore this signal, no action required*/
          break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_RLC signals */
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {
          case MAC_TIMER_T3172:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3172");


            /*if there is tbf release already pending ignore this signal*/
            if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
            {
              if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED )
              {
                gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN_UL, __LINE__, gas_id);
              }
              else
              {
                /* RLC should have been blocked when T3172 was started, find cause of error*/
                MSG_GERAN_ERROR_0_G("GMAC T3172 expired, RLC not blocked, No action");
              }
            }
            else
            {
              MSG_GERAN_MED_0_G("GMAC releasing L1, so ignoring MAC_TIMER_T3172 signal");
            }

            break;

          case MAC_TIMER_T3168:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3168");
            if (transfer_data_ptr->gmac_transfer_ul_request_count < MAX_RETRY_ATTEMPTS)
            {
              transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = TRUE;
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE, gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_EXPIRED_T3168, gas_id);
            }
            break;

          case MAC_TIMER_T3200_1:
          case MAC_TIMER_T3200_2:
          case MAC_TIMER_T3200_3:
          case MAC_TIMER_T3200_4:
          case MAC_TIMER_T3200_5:
          case MAC_TIMER_T3200_6:
          case MAC_TIMER_T3200_7:
          case MAC_TIMER_T3200_8:
               MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
               t3200_expiry_handler((mac_mac_sig_id_t)signal_id, gas_id );
          break;

#ifdef FEATURE_GSM_EDTM
          case MAC_TIMER_PKT_CS_REQ_RETRY:
            MSG_GERAN_MED_0_G("GMAC EDTM PKT CS REQ RETRY Timer expired");
            gmac_transfer_edtm_pkt_cs_req_handler(
              DL_ONLY,
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
              EDTM_PKT_CS_REQUEST_RETRY,
              gas_id);
          break;

          case MAC_TIMER_T3196:
            MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
            gmac_transfer_edtm_t3196_expiry_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal */
            break;
        }
      } /* end of MAC_MAC signals */
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {
          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_LOW_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");
            if(transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set == TRUE)
            {
              /*When releaseing DL only PDAN's can be sent on the RRBP*/
              gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE, gas_id);
            }
            else
            {
              if(gmac_util_encode_ul_control_msg_handler(PMR,TRUE, gas_id) == PASS)
              {
                gmac_util_send_ul_control_msg_handler(PMR, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
              }

            }
            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if(gmac_util_encode_ul_control_msg_handler(PCCF,TRUE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCF, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND (del_pdu: 0x%x) in T_DL",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            gmac_tran_check_dl_tbf_status(gas_id);

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            REL_RLC_DL,
                                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                            FALSE,
                                            gas_id);

            transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
            mac_log_dl_tbf_release(MAC_LOG_DL_GPRS_SUSPENSION, gas_id);
            break;

          case GRR_MAC_SEND_PCCN_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCN");
            if(gmac_util_encode_ul_control_msg_handler(PCCN,TRUE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCN, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

          case GRR_MAC_SEND_PSS_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PSS");
            if(gmac_util_encode_ul_control_msg_handler(PSS,TRUE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PSS, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_2_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND: ps_rel:%d, ps_rel_cause:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            gmac_dtm_chan_rel_handler( &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind,
                                       DL_TBF_RELEASE_CONF,
                                       gas_id );
            break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            MSG_GERAN_MED_3_G("GMAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ: dtm_tbf_type:0x%x, cs_pres:%d, dtm_freq_pres:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.cs_present,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_freq_present );

            if( gmac_tran_dtm_chan_ass_handler(
                  &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                  &transfer_data_ptr->gmac_transfer_ul_rlc_info_store,
                  DL_TBF_PRESENT,
                  gas_id ) == NO_DTM_ASS_ERROR )
            {
#ifdef FEATURE_GSM_EDTM
              if ( global_data_ptr->gmac_edtm_enabled )
              {
                gmac_tran_edtm_ass_cmd_handler(
                     &transfer_data_ptr->mac_transfer_substate,
                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                      gas_id );

                if ( transfer_data_ptr->mac_transfer_substate != TRANSFER_DL )
                {
                    transfer_data_ptr->mac_tran_substate_str = gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate, gas_id);

                    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                }
              }
              else
#endif /*FEATURE_GSM_EDTM*/
              {
                transfer_data_ptr->mac_transfer_substate = TRANSFER_DL_REASSIGN;

                MSG_GERAN_MED_0_G("GMAC State tran = TRANSFER_DL_REASSIGN");
                (void)mac_log_state(gas_id);
              }
            }
          break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);
            gmac_transfer_edtm_pkt_cs_req_handler(
              DL_ONLY,
              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause,
              EDTM_FIRST_PKT_CS_REQUEST,
              gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
          case GRR_MAC_PSHO_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_PSHO_REQ psho_type: 0x%x",global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_psho_req.psho_type);
            if(gmac_util_ps_ho_req_handler(transfer_data_ptr->mac_transfer_substate,gas_id) == TRUE)
            {
              transfer_data_ptr->mac_transfer_substate = TRANSFER_PS_HANDOVER;
              transfer_data_ptr->mac_tran_substate_str = gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate,gas_id);
              MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
              if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_type == PSHO_GTOG)
              {
                gmac_start_t3218(gas_id);
                if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
                {
                  gmac_util_send_grlcdl_psho_start_ind(gas_id);
                }
                grlc_dl_start_t3190(gas_id);

                gmac_util_send_grlcul_psho_start_ind(gas_id);

                gmac_stop_t3168(gas_id);

                memset( &global_data_ptr->gmac_local_stored_ul_chan_req, 0, sizeof(channel_req_desc_t) );

                global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_REQ;
                transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;

                transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = FALSE;
                transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;
                transfer_data_ptr->gmac_transfer_ul_request_count = 0;
              }
            }
            else
            {
              gmac_tran_psho_trigger_pccf_sending(transfer_data_ptr->mac_transfer_substate, gas_id);
              gmac_util_reset_psho_structure(gas_id);
            }
          break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_GRR signals */
      break; /* end of TRANSFER_DL */

    /*************************************
    * transfer state TRANSFER_UL_DYNAMIC *
    *************************************/
    case TRANSFER_UL_DYNAMIC:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_DATA_IND:
            if(transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release == FALSE &&
               transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release == FALSE)
            {
              gmac_tdynamic_data_ind(gas_id);
            }
            break;

          case L1_MAC_SINGLE_BLOCK_SENT:

            MSG_GERAN_MED_1_G("L1_MAC_SINGLE_BLOCK_SENT rcvd, ul re-alloc on UL PACCH gmac_prr_on_pacch_state %d",
                              global_data_ptr->c_flags.gmac_prr_on_pacch_state);
            /*only when sending control msg on UL PACCH*/

            if (global_data_ptr->c_flags.gmac_prr_on_pacch_state == GMAC_PRR_ON_PACCH_AWAIT_USF)
            {
              transfer_data_ptr->gmac_transfer_ul_request_count++;
              rr_read_public_store(RRPS_CELL_OPTIONS_T3168,&transfer_data_ptr->t3168_time, gas_id);
              gmac_start_t3168(transfer_data_ptr->t3168_time, gas_id);
              global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_REQ;
            }
            break;


          case L1_MAC_USF_DETECTED:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_USF_DETECTED");
            gmac_tdynamic_usf_detected(gas_id);
            break;

          case L1_MAC_QUAL_MEASURMENTS:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_QUAL_MEASURMENTS");

            if (global_data_ptr->c_flags.gmac_prr_on_pacch_state == GMAC_PRR_ON_PACCH_AWAIT_QUAL)
            {
              /*preload L1 with PRR, note PRR only sent once USF has been */
              /*detected at which point timer t3168 should be stated      */
              global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_USF;
              gmac_send_prr(NO_RE_EST,FALSE, gas_id);
            }
            break;

          case L1_MAC_1ST_DATA_BLOCK_SENT:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_1ST_DATA_BLOCK_SENT");
            gmac_stop_t3164(gas_id);
            gmac_start_t3180(gas_id);
            break;

          case L1_MAC_TBF_REL_CONFIRM:
#ifdef FEATURE_GPRS_PS_HANDOVER
            gmac_tran_ul_dynamic_process_rel_cnf(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released, gas_id);
#else /*FEATURE_GPRS_PS_HANDOVER*/
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
            gmac_tran_rel_state_handle_rel_cnf( global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released, gas_id );
#endif /*FEATURE_GSM_DTM*/

            transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
            transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release = FALSE;
            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_TBF_RELEASE_CONF)
            {
              gmac_transfer_tbf_rel_confirm(UL_TBF_RELEASE_CONF, gas_id);
              global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
            }
#endif /*FEATURE_GPRS_PS_HANDOVER*/
            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND");
            gmac_transfer_edtm_ph_cs_connect_handler(gas_id);
          break;

          case L1_MAC_FREQ_ASS_CNF:
            if ( global_data_ptr->gmac_edtm_enabled &&
                 transfer_data_ptr->t_flags.edtm_realloc_in_progress )
            {
              MSG_GERAN_MED_0_G("GMAC L1_MAC_FREQ_ASS_CNF rxd for EDTM in TRANSFER_UL_DYNAMIC");
            }
            else
            {
              MSG_GERAN_ERROR_0_G("GMAC L1_MAC_FREQ_ASS_CNF not expected in TRANSFER_UL_DYNAMIC");
            }
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from L1 */
            break;
        }
      } /* end of MAC_L1 signals */
      else if(task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_UL_REALLOC_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_REALLOC_REQ");
            transfer_data_ptr->gmac_transfer_ul_request_count = 0;

            global_data_ptr->gmac_transfer_ul_cv_zero_ind = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.b_cv_zero;

            if ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.pfi_present )
            {
              global_data_ptr->pfi_present = TRUE;
              global_data_ptr->pfi = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.pfi;
            }
            else
            {
              global_data_ptr->pfi_present = FALSE;
            }

            /* store RLC parameters and request quality report for PRR */
            global_data_ptr->gmac_local_stored_ul_chan_req = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.chan_req_desc;

            global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
            gmac_transfer_qual_meas_req_to_L1(gas_id);
            break;

          case RM_UL_RELEASE_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_RELEASE_IND");
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause != REL_UL_NORMAL)
            {
              if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause == REL_UL_ABNORMAL_CELL_RESEL)
              {
                gmac_abnormal_release_procedure(RR_CELL_RESEL_REQ,
                                                NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                                NO_DEL,
                                                FALSE,
                                                gas_id);
              }
              else
              {
                gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                                NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                                NO_DEL,
                                                FALSE,
                                                gas_id);
              }
              mac_log_ul_tbf_release(MAC_LOG_UL_RELEASE_IND, gas_id);
            }
            else
            {
              /*No PUAN with FAI has been rcvd during rlc UNACK mode, and t3182 has expired under these conditions */
              /*release UL and return to IDLE or NULL mode. NOTE:- as there is no RRBP no PCA can be sent.         */
              MSG_GERAN_MED_0_G("MAC UL TBF normal release");
              gmac_transfer_ul_dynamic_release_clean(gas_id);
              global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
              gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__,gas_id);
              gmac_tran_send_mac_l1_release(UL_TBF_NORMAL,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE,gas_id);
            }
            break;

        case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");
            gmac_tran_handle_unexpected_rlc_tbf_req(gas_id);
            break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from RLC */
            break;
        }
      } /* end of MAC_RLC signals */
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {
          case MAC_TIMER_T3180:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3180");

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                            NO_DEL,
                                            FALSE,
                                            gas_id);

            mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3180, gas_id);
            break;

          case MAC_TIMER_T3164_TRANSFER:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3164_TRANSFER");
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                            NO_DEL,
                                            FALSE,
                                            gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3164,gas_id);
            break;

          case MAC_TIMER_T3168:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3168");
            if (transfer_data_ptr->gmac_transfer_ul_request_count < MAX_RETRY_ATTEMPTS)
            {
              /* request quality reports again for PRR */
              global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
              gmac_transfer_qual_meas_req_to_L1(gas_id);
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                              NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                              NO_DEL,
                                              FALSE,
                                              gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3168, gas_id);
            }
            break;

          case MAC_TIMER_T3164:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3164");

            /* retry access up to 4 times then return to packet */
            /* idle and inform upper layers of TBF failure      */
            /* retry performed in idle, so leave transfer       */
            if (transfer_data_ptr->gmac_transfer_ul_request_count < MAX_RETRY_ATTEMPTS)
            {

              /* UL TBF released so TFI no longer valid */
              global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;

              /* for safety reset global params */
              memset(&global_data_ptr->gmac_local_l1_ta_params, 0xff, sizeof(pkt_timing_adv_t));
              memset(&global_data_ptr->gmac_local_stored_ul_chan_req, 0xff, sizeof(channel_req_desc_t));

              transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = FALSE;
              transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
              transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;

              global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_REQ;
              transfer_data_ptr->t_flags.failed_to_send_pdan_chan_req = FALSE;
              global_data_ptr->current_egprs_ul_ws = WS_ALLOCATION_INVALID;

              /*stop timers*/
              gmac_stop_t3180(gas_id);
              gmac_stop_t3168(gas_id);
              gmac_stop_t3164_transfer(gas_id);

              gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__, gas_id);

              gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL, gas_id);
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                              NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                              DEL_ONE,
                                              FALSE,
                                              gas_id);
            }
            mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3164, gas_id);
            break;

          case MAC_TIMER_T3200_1:
          case MAC_TIMER_T3200_2:
          case MAC_TIMER_T3200_3:
          case MAC_TIMER_T3200_4:
          case MAC_TIMER_T3200_5:
          case MAC_TIMER_T3200_6:
          case MAC_TIMER_T3200_7:
          case MAC_TIMER_T3200_8:
               MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
               t3200_expiry_handler((mac_mac_sig_id_t)signal_id, gas_id );
          break;

#ifdef FEATURE_GSM_EDTM
          case MAC_TIMER_PKT_CS_REQ_RETRY:
            MSG_GERAN_MED_0_G("GMAC EDTM PKT CS REQ RETRY Timer expired");
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_ONLY,
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
              EDTM_PKT_CS_REQUEST_RETRY,
              gas_id);
          break;

          case MAC_TIMER_T3196:
            MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
            gmac_transfer_edtm_t3196_expiry_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_MAC signals */
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {
          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_LOW_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");
            if(gmac_util_encode_ul_control_msg_handler(PMR,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if(gmac_util_encode_ul_control_msg_handler(PCCF,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND (del_pdu: 0x%x) in T_UL_DYN",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            gmac_tran_check_dl_tbf_status(gas_id);

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                            FALSE,
                                            gas_id);

            transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
            mac_log_ul_tbf_release(MAC_LOG_UL_GPRS_SUSPENSION, gas_id);
            break;

          case GRR_MAC_SEND_PCCN_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCN");
            if(gmac_util_encode_ul_control_msg_handler(PCCN,FALSE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCN, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

          case GRR_MAC_SEND_PSS_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PSS");
            if(gmac_util_encode_ul_control_msg_handler(PSS,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PSS, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_2_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND: ps_rel:%d, ps_rel_cause:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            gmac_dtm_chan_rel_handler( &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind,
                                       UL_TBF_RELEASE_CONF,
                                       gas_id );
            break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            MSG_GERAN_MED_3_G("GMAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ: dtm_tbf_type:0x%x, cs_pres:%d, dtm_freq_pres:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.cs_present,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_freq_present );

            if( gmac_tran_dtm_chan_ass_handler(
                  &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                  &transfer_data_ptr->gmac_transfer_ul_rlc_info_store,
                  UL_TBF_PRESENT,
                  gas_id ) == NO_DTM_ASS_ERROR )
            {
#ifdef FEATURE_GSM_EDTM
              if ( global_data_ptr->gmac_edtm_enabled )
              {
                gmac_tran_edtm_ass_cmd_handler(
                     &transfer_data_ptr->mac_transfer_substate,
                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                      gas_id );

                if ( transfer_data_ptr->mac_transfer_substate != TRANSFER_UL_DYNAMIC )
                {
                    transfer_data_ptr->mac_tran_substate_str = gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate, gas_id);

                    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                }
              }
              else
#endif /*FEATURE_GSM_EDTM*/
              {
                 transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC;

                 MSG_GERAN_MED_0_G("GMAC State tran = TRANSFER_UL_DYNAMIC_REALLOC");
                 (void)mac_log_state(gas_id);
              }
            }
          break;

#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);

            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_ONLY,
              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause,
              EDTM_FIRST_PKT_CS_REQUEST,
              gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
          case GRR_MAC_PSHO_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_PSHO_REQ psho_type: 0x%x",global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_psho_req.psho_type);

            if(gmac_util_ps_ho_req_handler(transfer_data_ptr->mac_transfer_substate, gas_id) == TRUE)
            {
              transfer_data_ptr->mac_transfer_substate = TRANSFER_PS_HANDOVER;

              transfer_data_ptr->mac_tran_substate_str = gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate, gas_id);
             MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);

              if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_type == PSHO_GTOG)
              {
                gmac_start_t3218(gas_id);
                if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
                {
                  gmac_util_send_grlcdl_psho_start_ind(gas_id);
                }
                gmac_start_t3180(gas_id);

                gmac_util_send_grlcul_psho_start_ind(gas_id);

                gmac_stop_t3168(gas_id);

                memset( &global_data_ptr->gmac_local_stored_ul_chan_req, 0, sizeof(channel_req_desc_t) );
                global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_REQ;
                transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;

                transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;

                transfer_data_ptr->gmac_transfer_ul_request_count = 0;
              }
            }
            else
            {
              gmac_tran_psho_trigger_pccf_sending(transfer_data_ptr->mac_transfer_substate, gas_id);
              gmac_util_reset_psho_structure(gas_id);
            }
          break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from GRR */
            break;
        }
      } /* end of GRR_MAC signals */
      break; /* end of TRANSFER_UL_DYNAMIC */

    /**************************************
    * transfer state TRANSFER_DL_REASSIGN *
    **************************************/
  case TRANSFER_DL_REASSIGN:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_DATA_IND:
            if(transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_dl_release == FALSE)
            {
              gmac_transfer_dl_reassign_data_ind(gas_id);
            }

            break;

          case L1_MAC_QUAL_MEASURMENTS:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_QUAL_MEASURMENTS");
            gmac_transfer_dl_pkt_acknack(&(global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements), gas_id);
            break;

          case MAC_PH_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");

            if (global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
            {

  #ifdef FEATURE_GSM_DTM
              gmac_tran_rel_state_handle_connect_ind(
                global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind,
                gas_id );
  #endif /*FEATURE_GSM_DTM*/

              if (global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == DOWNLINK_TBF_ESTABLISHED)
              {
                GMAC_UPDATE_DL_TS_REASS;
                GMAC_UPDATE_DL_TFI;

                global_data_ptr->gmac_local_tfi_store.dl_tfi_valid = TRUE;

  #ifdef FEATURE_GSM_DTM
                transfer_data_ptr->t_flags.dtm_realloc_in_progress = FALSE;
  #endif /*FEATURE_GSM_DTM*/

                /*For HMSC RLC needs to know the Number of DL ts so irrespective of */
                /*EGPRS or GPRS mode always send MR_DL_ASS_IND to RLC               */
                gmac_util_send_dl_ass_to_rlc(gas_id);


                MSG_GERAN_MED_0_G("GMAC State tran = T_DL");
                transfer_data_ptr->mac_transfer_substate = TRANSFER_DL;
                transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
                (void)mac_log_state(gas_id);
                MSG_GERAN_MED_0_G("mac info wait_for_ul_rel_confirm = FALSE");
                global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;

                /*ensure that RLC is not blocked if returning to T_DL */
                if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
                {
                  if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED)
                  {
                    gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
                  }
                }
                else
                {
                  MSG_GERAN_HIGH_0_G("GMAC T3172 still running leave RLC blocked");
                }
              }
              else
              {
                if(geran_get_nv_recovery_restart_enabled(gas_id))
                {
                  MSG_GERAN_ERROR_1_G("GMAC AB Rel, Expected DL (0x01) in PH_CONNECT_IND, received: 0x%x",
                                      global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                  MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                  geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                }
                else
                {
                  ERR_GERAN_FATAL_2_G("GMAC AB Rel, Expected DL (0x01) in PH_CONNECT_IND, received: 0x%x",
                            gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                }
              }
            }
            else
            {
              MSG_GERAN_MED_0_G("GMAC releasing L1, so ignoring MAC_PH_CONNECT_IND signal");
            }

            break;

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("GMAC INFO L1 Freq Cnf ignored for freq cleanup");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }
          break;

          case L1_MAC_TBF_REL_CONFIRM:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
            gmac_tran_rel_state_handle_rel_cnf( global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released, gas_id );
#endif /*FEATURE_GSM_DTM*/

            transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_dl_release = FALSE;

            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF)
            {
              gmac_transfer_tbf_rel_confirm(DL_TBF_RELEASE_CONF, gas_id);
            }
            else
            {
              if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
              {
                if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED)
                {
                  gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
                }
              }
              else
              {
                MSG_GERAN_HIGH_0_G("GMAC T3172 still running leave RLC blocked");
              }

              global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;
            }
            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND");
            gmac_transfer_edtm_ph_cs_connect_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in state %d task id %d sig id 0x%x",
                       transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from L1 */
            break;
        }
      } /* end of MAC_L1 signals */
      else if(task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_DL_PKT_ACKNACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_ACKNACK_REQ");
            gmac_transfer_dl_pdan_handler(&(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_acknack_req),FALSE, gas_id);
            break;

          case RM_DL_RELEASE_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd RM_DL_RELEASE_IND %d",global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause);

            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause == REL_DL_NORMAL)
            {
              gmac_stop_t3168(gas_id);
              if (transfer_data_ptr->t_flags.gmac_transfer_ul_req)
              {
                MSG_GERAN_LOW_2_G("MAC info UL REQ CNT %d PEND %d",
                                  transfer_data_ptr->gmac_transfer_ul_request_count,transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending);

                gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,DONT_REL_RLC_DL,NO_DEL,FALSE, gas_id);
                mac_log_dl_tbf_release(MAC_LOG_DL_NORMAL_RELEASE, gas_id);
              }
              else
              {
                gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__, gas_id);
                gmac_transfer_dl_release_clean(gas_id);
                gmac_transfer_dl_normal_release_to_idle(gas_id);
              }
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,DONT_REL_RLC_DL,NO_DEL,FALSE, gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_RELEASE_IND, gas_id);
            }
            break;

          case RM_DL_PKT_CTRL_ACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_CTRL_ACK_REQ");
            transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = TRUE;
            gmac_rrbp_send_pca(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.frame_number,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.time_slot, TRUE,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.rrbp, PCA_DL_FAI_MSG,
                               gas_id);
            break;

        case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");
            if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED )
            {
              /* RLC has attempted access when we have blocked it.  This might
                 be a race condition or an error in RLC.
                 Because a race condition is a possibility we have to assume that this has happened
                 in this case */
              MSG_GERAN_HIGH_0_G("GMAC RLC attempted access whilst blocked, ignore msg, assume race condition");
            }
            else
            {
              if (global_data_ptr->c_flags.tbf_release_pending_flg)
              {
                /* RLC should have been blocked if we are pending a TBF release.  Flag as error.
                   Cause of this needs to be found */
                MSG_GERAN_ERROR_0_G("GMAC tbf_release_pending_flg set, RLC not blocked in T_DL_REASS, Ignore RLC req");
              }
              else
              {
                /* MAC doesn't accept access attempts in T_DL_REASS, block RLC and release on CONNECT IND */
                MSG_GERAN_HIGH_0_G("GMAC Can't handle tbf req in T_DL_REASS, block RLC until CONNECT_IND");
                gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__, gas_id);
              }
            }
            break;

          case RM_PS_ACCESS_CNF:
             /*ignore this signal, no action required*/
          break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from RLC */
            break;
        }
      } /* end of MAC_RLC signals */
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {
          case MAC_TIMER_T3172:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3172 in T_DL_REASS, No action until CONNECT_IND");
            break;

          case MAC_TIMER_T3168:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3168");
            if (transfer_data_ptr->gmac_transfer_ul_request_count < MAX_RETRY_ATTEMPTS)
            {
              transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = TRUE;
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE, gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_EXPIRED_T3168, gas_id);
            }
            break;

          case MAC_TIMER_T3200_1:
          case MAC_TIMER_T3200_2:
          case MAC_TIMER_T3200_3:
          case MAC_TIMER_T3200_4:
          case MAC_TIMER_T3200_5:
          case MAC_TIMER_T3200_6:
          case MAC_TIMER_T3200_7:
          case MAC_TIMER_T3200_8:
               MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
               t3200_expiry_handler((mac_mac_sig_id_t)signal_id, gas_id );
          break;

#ifdef FEATURE_GSM_EDTM
          case MAC_TIMER_PKT_CS_REQ_RETRY:
            MSG_GERAN_MED_0_G("GMAC EDTM PKT CS REQ RETRY Timer expired");
            gmac_transfer_edtm_pkt_cs_req_handler(
              DL_ONLY,
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
              EDTM_PKT_CS_REQUEST_RETRY,
              gas_id);
          break;

          case MAC_TIMER_T3196:
            MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
            gmac_transfer_edtm_t3196_expiry_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_MAC signals */
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {
          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_LOW_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");
            if(transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set == TRUE)
            {
              /*When releaseing DL only PDAN's can be sent on the RRBP*/
              gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE, gas_id);
            }
            else
            {
              if(gmac_util_encode_ul_control_msg_handler(PMR,TRUE,gas_id) == PASS)
              {
                gmac_util_send_ul_control_msg_handler(PMR, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
              }
            }
            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if(gmac_util_encode_ul_control_msg_handler(PCCF,TRUE,gas_id) == PASS)
            {
                gmac_util_send_ul_control_msg_handler(PCCF, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND (del_pdu: 0x%x) in T_DL_REASS",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            gmac_tran_check_dl_tbf_status(gas_id);

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            REL_RLC_DL,
                                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                            FALSE,
                                            gas_id);

            transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
            mac_log_dl_tbf_release(MAC_LOG_DL_GPRS_SUSPENSION, gas_id);
            break;

          case GRR_MAC_SEND_PCCN_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCN");
            if(gmac_util_encode_ul_control_msg_handler(PCCN,TRUE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCN, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

          case GRR_MAC_SEND_PSS_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PSS");
            if(gmac_util_encode_ul_control_msg_handler(PSS,TRUE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PSS, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_2_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND: ps_rel:%d, ps_rel_cause:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            gmac_dtm_chan_rel_handler( &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind,
                                       DL_TBF_RELEASE_CONF,
                                       gas_id );
            break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            MSG_GERAN_MED_3_G("GMAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ: dtm_tbf_type:0x%x, cs_pres:%d, dtm_freq_pres:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.cs_present,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_freq_present );

            if (gmac_tran_dtm_chan_ass_handler(
                    &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                    &transfer_data_ptr->gmac_transfer_ul_rlc_info_store,
                    DL_TBF_PRESENT,
                    gas_id) == NO_DTM_ASS_ERROR)
            {
#ifdef FEATURE_GSM_EDTM
              if ( global_data_ptr->gmac_edtm_enabled )
              {
                gmac_tran_edtm_ass_cmd_handler(
                     &transfer_data_ptr->mac_transfer_substate,
                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                      gas_id );

                if ( transfer_data_ptr->mac_transfer_substate != TRANSFER_DL_REASSIGN )
                {
                    transfer_data_ptr->mac_tran_substate_str =
                      gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate, gas_id);

                   MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                }
              }
#endif /*FEATURE_GSM_EDTM*/
            }
          break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);
            gmac_transfer_edtm_pkt_cs_req_handler(
              DL_ONLY,
              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause,
              EDTM_FIRST_PKT_CS_REQUEST,
              gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from GRR */
            break;
        }
      } /* end of MAC_GRR signals */
      break; /* end of TRANSFER_DL_REASSIGN */

    /*********************************************
    * transfer state TRANSFER_UL_DYNAMIC_REALLOC *
    *********************************************/
    case TRANSFER_UL_DYNAMIC_REALLOC:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_DATA_IND:
            if(transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release == FALSE)
            {
              gmac_tdynamic_realloc_data_ind(gas_id);
            }
            break;

          case L1_MAC_USF_DETECTED:
            gmac_tdynamic_usf_detected(gas_id);
            break;

          case MAC_PH_CONNECT_IND:
            /*while waiting for a pending UL to connect cell reselection or suspension for cs can be rcvd. */
            /*Under these conditions the rel process must complete IGNORE any attempts to connect the ul   */
            if(transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE)
            {
  #ifdef FEATURE_GSM_DTM
              gmac_tran_rel_state_handle_connect_ind(
                global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind,
                gas_id );
  #endif /*FEATURE_GSM_DTM*/
  
              MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");
              if (global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_TBF_ESTABLISHED)
              {
  #ifdef FEATURE_GSM_DTM
                transfer_data_ptr->t_flags.dtm_realloc_in_progress = FALSE;
  #endif /*FEATURE_GSM_DTM*/
  
  #ifdef FEATURE_GSM_EDTM
                if ( ( global_data_ptr->gmac_edtm_enabled ) &&
                     ( transfer_data_ptr->t_flags.dtm_active ) )
                {
                  /* If the DL is implicitly released using the DTM Ass Cmd (when EDTM is active) but the
                     UL is established then the gmac_transfer_ul_req needs to be reset.
                     If the UL has already been established then resetting the flag will have no effect*/
                  MSG_GERAN_MED_1_G("GMAC EDTM t_flags.gmac_transfer_ul_req set to FALSE, previous value: 0x%x",
                                    transfer_data_ptr->t_flags.gmac_transfer_ul_req);
  
                  transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;
                }
  
  #endif /*FEATURE_GSM_EDTM*/
  
                global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = TRUE;
  
                GMAC_UPDATE_UL_TS_REALLOC;
                GMAC_UPDATE_UL_DLCTRL_TS_REALLOC;
                GMAC_UPDATE_UL_TFI;
  
  
                if (transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag)
                {
                  /* send MAC_L1_CS_RECONFIG due to CS change */
                  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_CS_RECONFIG");
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_id = (uint8)MAC_L1_CS_RECONFIG;
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_set = MS_MAC_L1;
                  gprs_l1_mac_acc_func(&transfer_data_ptr->mac_l1_sig_cs_reconfig, gas_id);
  
                  global_data_ptr->gmac_local_cs_setting = transfer_data_ptr->pua_ptr_cs_change;
                }
  
                transfer_data_ptr->mr_ul_alloc_cnf_ptr = grlc_get_cmd_buf_mac();
                if (transfer_data_ptr->mr_ul_alloc_cnf_ptr != NULL)
                {
                  MSG_GERAN_MED_0_G("MAC Sent MR_UL_ALLOC_CNF");
                  transfer_data_ptr->mr_ul_alloc_cnf_ptr->msg.ul_alloc_cnf = transfer_data_ptr->gmac_transfer_ul_rlc_info_store;
                  gmac_rlc_mac_insert_gas_id(transfer_data_ptr->mr_ul_alloc_cnf_ptr,gas_id);
                  grlc_put_cmd_mac(gas_id,transfer_data_ptr->mr_ul_alloc_cnf_ptr);
                }
                else
                {
                  ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
                }
  
                MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYN");
  
                transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC;
                gmac_start_t3164_transfer(gas_id);
                (void)mac_log_state(gas_id);
  
                gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, global_data_ptr->current_tbf_mode, FALSE, gas_id);
  
                if ( transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending )
                {
                  /* Pending reallocation request can now be processed. Prepare to send PRR on PACCH */
  
                  transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
                  transfer_data_ptr->gmac_transfer_ul_request_count = 0;
                  global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
                  gmac_transfer_qual_meas_req_to_L1(gas_id);
                }
  
                /* Any pending PCCN to be sent on the DL RRBP is switched and sent on the UL pacch*/
                /* note this cover the case where an ul establishment initiated via the DL has not*/
                /* connected before the DL is released                                            */
  
                if(global_data_ptr->gmac_local_pccn_params.pccn_pending == PCCN_PENDING_DL)
                {
                  gmac_util_send_ul_control_msg_handler(PCCN, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
                }
  
  
                /*cr68663*/
                /*If a PMR is waiting to be sent via the DL rrbp and an UL */
                /*connects, switch the sending of the PMR to ul pacch      */
                if(global_data_ptr->gmac_local_pmr_params.pmr_pending == TRUE)
                {
                  gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
                }
  
              }
              else
              {
                if(geran_get_nv_recovery_restart_enabled(gas_id))
                {
                  MSG_GERAN_ERROR_1_G("GMAC AB Rel, Expected UL (0x00) in PH_CONNECT_IND, received: 0x%x",
                                      global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                  MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                  geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                }
                else
                {
                  ERR_GERAN_FATAL_2_G("GMAC AB Rel, Expected UL (0x00) in PH_CONNECT_IND, received: 0x%x",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                }
              }
            }
            else
            {
              MSG_GERAN_MED_0_G("MAC info ABNORMAL UL & DL rel in progress,Ignore PH_CONNECT_IND");
            }
            break;

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("GMAC INFO L1 Freq Cnf ignored for freq cleanup");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }
          break;

          case L1_MAC_1ST_DATA_BLOCK_SENT:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_1ST_DATA_BLOCK_SENT");
            gmac_stop_t3164(gas_id);
            gmac_start_t3180(gas_id);
            break;

          case L1_MAC_TBF_REL_CONFIRM:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
            gmac_tran_rel_state_handle_rel_cnf( global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released, gas_id );
#endif /*FEATURE_GSM_DTM*/

            transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_TBF_RELEASE_CONF)
            {
              gmac_transfer_tbf_rel_confirm(UL_TBF_RELEASE_CONF, gas_id);
            }
            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND");
            gmac_transfer_edtm_ph_cs_connect_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from L1 */
            break;
        }
      } /* end of MAC_L1 signals */
      else if(task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {

          case RM_UL_REALLOC_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_REALLOC_REQ");

            /* Store channel request parameters and set reallocation pending flag
               to be dealt with on receipt of PH_CONNECT_IND */

            global_data_ptr->gmac_transfer_ul_cv_zero_ind = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.b_cv_zero;
            global_data_ptr->gmac_local_stored_ul_chan_req = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.chan_req_desc;

            transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = TRUE;
            break;

          case RM_UL_RELEASE_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_RELEASE_IND");
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause != REL_UL_NORMAL)
            {
              if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause == REL_UL_ABNORMAL_CELL_RESEL)
              {
                gmac_abnormal_release_procedure(RR_CELL_RESEL_REQ,REL_RLC_DL,NO_DEL,FALSE, gas_id);
              }
              else
              {
                gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE, gas_id);
              }
              mac_log_ul_tbf_release(MAC_LOG_UL_RELEASE_IND, gas_id);
            }
            else
            {
              /*No PUAN with FAI has been rcvd during rlc UNACK mode, and t3182 has expired under these conditions */
              /*release UL and return to IDLE or NULL mode. NOTE:- as there is no RRBP no PCA can be sent.         */
              MSG_GERAN_MED_0_G("MAC UL TBF normal release");
              gmac_transfer_ul_dynamic_release_clean(gas_id);
              global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
              gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__, gas_id);
              gmac_tran_send_mac_l1_release(UL_TBF_NORMAL, gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE, gas_id);
            }
            break;

          case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");
            gmac_tran_handle_unexpected_rlc_tbf_req(gas_id);
            break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from RLC */
            break;
        }
      } /* end of MAC_RLC signals */
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {
          case MAC_TIMER_T3180:
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                            NO_DEL,
                                            FALSE,
                                            gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3180, gas_id);
            break;

          case MAC_TIMER_T3164:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3164");

            /* retry access up to 4 times then return to packet */
            /* idle and inform upper layers of TBF failure      */
            /* retry performed in idle, so leave transfer       */
            if (transfer_data_ptr->gmac_transfer_ul_request_count < MAX_RETRY_ATTEMPTS)
            {
              /* UL TBF released so TFI no longer valid */
              global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;

              /* for safety reset global params */
              memset(&global_data_ptr->gmac_local_l1_ta_params, 0xff, sizeof(pkt_timing_adv_t));
              memset(&global_data_ptr->gmac_local_stored_ul_chan_req, 0xff, sizeof(channel_req_desc_t));

              transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = FALSE;
              transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
              transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;
              global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_REQ;
              transfer_data_ptr->t_flags.failed_to_send_pdan_chan_req = FALSE;
              global_data_ptr->current_egprs_ul_ws = WS_ALLOCATION_INVALID;

              /*stop timers*/
              gmac_stop_t3180(gas_id);
              gmac_stop_t3168(gas_id);
              gmac_stop_t3164_transfer(gas_id);
              gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__,gas_id);
              gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL,gas_id);
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                              NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                              DEL_ONE,
                                              FALSE,
                                              gas_id);
            }
            mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3164,gas_id);
            break;

          case MAC_TIMER_T3200_1:
          case MAC_TIMER_T3200_2:
          case MAC_TIMER_T3200_3:
          case MAC_TIMER_T3200_4:
          case MAC_TIMER_T3200_5:
          case MAC_TIMER_T3200_6:
          case MAC_TIMER_T3200_7:
          case MAC_TIMER_T3200_8:
               MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
               t3200_expiry_handler((mac_mac_sig_id_t)signal_id, gas_id );
          break;

#ifdef FEATURE_GSM_EDTM
          case MAC_TIMER_PKT_CS_REQ_RETRY:
            MSG_GERAN_MED_0_G("GMAC EDTM PKT CS REQ RETRY Timer expired");
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_ONLY,
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
              EDTM_PKT_CS_REQUEST_RETRY,
              gas_id);
          break;

          case MAC_TIMER_T3196:
            MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
            gmac_transfer_edtm_t3196_expiry_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_MAC signals */
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {
          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_LOW_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");
            if(gmac_util_encode_ul_control_msg_handler(PMR,FALSE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if(gmac_util_encode_ul_control_msg_handler(PCCF,FALSE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND (del_pdu: 0x%x) in T_UL_DYN_REALL",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            gmac_tran_check_dl_tbf_status(gas_id);

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                            FALSE,
                                            gas_id);

            transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
            mac_log_ul_tbf_release(MAC_LOG_UL_GPRS_SUSPENSION, gas_id);
            break;

         case GRR_MAC_SEND_PCCN_REQ :
           MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCN");
           if(gmac_util_encode_ul_control_msg_handler(PCCN,FALSE, gas_id) == PASS)
           {
              gmac_util_send_ul_control_msg_handler(PCCN, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
           }
           break;

          case GRR_MAC_SEND_PSS_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PSS");
            if(gmac_util_encode_ul_control_msg_handler(PSS,FALSE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PSS, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_2_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND: ps_rel:%d, ps_rel_cause:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            gmac_dtm_chan_rel_handler( &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind,
                                       UL_TBF_RELEASE_CONF,
                                       gas_id );
            break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            MSG_GERAN_MED_3_G("GMAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ: dtm_tbf_type:0x%x, cs_pres:%d, dtm_freq_pres:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.cs_present,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_freq_present );

            if ( gmac_tran_dtm_chan_ass_handler(
                    &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                    &transfer_data_ptr->gmac_transfer_ul_rlc_info_store,
                    UL_TBF_PRESENT,
                    gas_id ) == NO_DTM_ASS_ERROR )
            {
#ifdef FEATURE_GSM_EDTM
              if ( global_data_ptr->gmac_edtm_enabled )
              {
                gmac_tran_edtm_ass_cmd_handler(
                     &transfer_data_ptr->mac_transfer_substate,
                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                      gas_id );

                if ( transfer_data_ptr->mac_transfer_substate != TRANSFER_UL_DYNAMIC_REALLOC )
                {
                    transfer_data_ptr->mac_tran_substate_str =
                      gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate, gas_id);

                    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                }
              }
#endif /*FEATURE_GSM_EDTM*/
            }
          break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);

            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_ONLY,
              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause,
              EDTM_FIRST_PKT_CS_REQUEST,
              gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from GRR */
            break;
        }
      } /* end of GRR_MAC signals */
      break; /* end of TRANSFER_UL_DYNAMIC_REALLOC */

    /************************************************
    * transfer state TRANSFER_DL_WITH_DYNAMIC_ALLOC *
    ************************************************/
    case TRANSFER_DL_WITH_DYNAMIC_ALLOC:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_DATA_IND:
            gmac_transfer_dl_dynamic_alloc_data_ind(gas_id);
            break;

          case L1_MAC_QUAL_MEASURMENTS:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_QUAL_MEASURMENTS");
            gmac_transfer_dl_pkt_acknack(&(global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements), gas_id);
            break;

          case MAC_PH_CONNECT_IND:
            /*while waiting for a pending UL to connect cell reselection or suspension for cs can be rcvd. */
            /*Under these conditions the rel process must complete IGNORE any attempts to connect the ul   */
            if(transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE)
            {
#ifdef FEATURE_GSM_DTM
              gmac_tran_rel_state_handle_connect_ind(
                global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind,
                gas_id );
#endif /*FEATURE_GSM_DTM*/

              /*L1 and MAC have to handle simultionus UL and DL config in this state*/
              if (global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_TBF_ESTABLISHED)
              {
                MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND for UL");
                gmac_ul_transfer_init(gas_id);

                global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = TRUE;

                GMAC_UPDATE_UL_TFI;
                GMAC_UPDATE_UL_TS_REALLOC;
                GMAC_UPDATE_UL_DLCTRL_TS_REALLOC;

                /*CR 96169 - start T3180 when entering concurrency from a DL state.
                  This guards against the possibility of MAC never timing out */
                gmac_start_t3180(gas_id);

                /* Any pending PCCN to be sent on the DL RRBP is switched and sent on the UL pacch*/

                if(global_data_ptr->gmac_local_pccn_params.pccn_pending == PCCN_PENDING_DL)
                {
                  gmac_util_send_ul_control_msg_handler(PCCN, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
                }

                /*cr68663*/
                if(global_data_ptr->gmac_local_pmr_params.pmr_pending == TRUE)
                {
                  gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
                }

                if (transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag)
                {
                  /* send MAC_L1_CS_RECONFIG due to CS change */
                  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_CS_RECONFIG");
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_id = (uint8)MAC_L1_CS_RECONFIG;
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_set = MS_MAC_L1;
                  gprs_l1_mac_acc_func(&transfer_data_ptr->mac_l1_sig_cs_reconfig, gas_id);

                  global_data_ptr->gmac_local_cs_setting = transfer_data_ptr->pua_ptr_cs_change;
                }

                if ( global_data_ptr->current_test_mode != TEST_MODE_OFF )
                {
                  transfer_data_ptr->mr_dl_test_mode_ind_ptr = grlc_get_cmd_buf_mac();

                  if ( transfer_data_ptr->mr_dl_test_mode_ind_ptr != NULL )
                  {
                    transfer_data_ptr->mr_dl_test_mode_ind_ptr->msg.sig = MR_DL_TEST_MODE_IND;
                    transfer_data_ptr->mr_dl_test_mode_ind_ptr->msg.dl_test_mode_ind.test_mode = global_data_ptr->current_test_mode;
                    gmac_rlc_mac_insert_gas_id(transfer_data_ptr->mr_dl_test_mode_ind_ptr,gas_id);
                    grlc_put_cmd_mac(gas_id,transfer_data_ptr->mr_dl_test_mode_ind_ptr);

                    MSG_GERAN_MED_1_G("GMAC Sent MR_DL_TEST_MODE_IND : %d", global_data_ptr->current_test_mode);
                  }
                  else
                  {
                    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
                  }
                }

                transfer_data_ptr->mr_ul_alloc_cnf_ptr = grlc_get_cmd_buf_mac();
                if (transfer_data_ptr->mr_ul_alloc_cnf_ptr != NULL)
                {
                  MSG_GERAN_MED_0_G("MAC Sent MR_UL_ALLOC_CNF");
                  transfer_data_ptr->mr_ul_alloc_cnf_ptr->msg.ul_alloc_cnf = transfer_data_ptr->gmac_transfer_ul_rlc_info_store;

                  gmac_rlc_mac_insert_gas_id(transfer_data_ptr->mr_ul_alloc_cnf_ptr,gas_id);
                  grlc_put_cmd_mac(gas_id,transfer_data_ptr->mr_ul_alloc_cnf_ptr);
                }
                else
                {
                  ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
                }

                gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, global_data_ptr->current_tbf_mode, FALSE, gas_id);

                if (transfer_data_ptr->t_flags.dl_tbf_normal_release_ind)
                {
                  gmac_transfer_dl_release_clean(gas_id);
                  gmac_transfer_dl_normal_release_to_idle(gas_id);

                  MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYN");
                  transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC;

                  (void)mac_log_state(gas_id);
                  transfer_data_ptr->t_flags.dl_tbf_normal_release_ind = FALSE;

                  gmac_start_t3164_transfer(gas_id);
                }
#ifdef FEATURE_GSM_EDTM
                else if (transfer_data_ptr->t_flags.edtm_pcri_rel_dl_tbf_for_ctrl_ack)
                {
                  gmac_transfer_dl_release_clean(gas_id);
                  gmac_util_send_dl_release_to_rlc(gas_id);
                  gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL, gas_id);
                  MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYN");
                  transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC;
                  (void)mac_log_state(gas_id);
                  transfer_data_ptr->t_flags.edtm_pcri_rel_dl_tbf_for_ctrl_ack = FALSE;
                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;
                }
#endif /*FEATURE_GSM_EDTM*/
                else
                {

                  if((transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl == FALSE) &&
                     (transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul == TRUE))
                  {
                    MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DYN");
                    transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;
                    transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;
                    (void)mac_log_state(gas_id);

                    gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, global_data_ptr->current_tbf_mode, FALSE, gas_id);

                    gmac_start_t3164_transfer(gas_id);
                  }
                  else
                  {
                    transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;
                  }

                }
              }
              else if(global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == DOWNLINK_TBF_ESTABLISHED)
              {
                MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND for DL");

                GMAC_UPDATE_DL_TFI;

                /*For HMSC RLC needs to know the Number of DL ts so irrespective of */
                /*EGPRS or GPRS mode always send MR_DL_ASS_IND to RLC               */
                gmac_util_send_dl_ass_to_rlc(gas_id);


                if((transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl == TRUE) &&
                   (transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul == FALSE))
                {
                  MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DYN");

                  if ( transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending )
                  {
                    MSG_GERAN_HIGH_0_G("MAC handling saved gmac_transfer_ul_tbf_realloc_pending");

                    /* Pending reallocation request can now be processed. Prepare to send PRR on PACCH */

                    transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
                    transfer_data_ptr->gmac_transfer_ul_request_count = 0;
                    global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
                    gmac_transfer_qual_meas_req_to_L1(gas_id);
                  }

                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
                  transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;
                  (void)mac_log_state(gas_id);

                  gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, global_data_ptr->current_tbf_mode, FALSE, gas_id);

                }
                else
                {
                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
                }
              }
              else
              {
                if(geran_get_nv_recovery_restart_enabled(gas_id))
                {
                  MSG_GERAN_ERROR_1_G("GMAC AB Rel, Expected UL (0x00) or DL (0x01) in PH_CONNECT_IND, received: 0x%x",
                                      global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                  MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                  geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                }
                else
                {
                  ERR_GERAN_FATAL_2_G("GMAC AB Rel, Expected UL (0x00) or DL (0x01) in PH_CONNECT_IND, received: 0x%x",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                }
              }
            }
            else
            {
              MSG_GERAN_MED_0_G("MAC info ABNORMAL UL & DL rel in progress,Ignore PH_CONNECT_IND");
            }
            break;

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("GMAC INFO L1 Freq Cnf ignored for freq cleanup");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }
          break;

          case L1_MAC_TBF_REL_CONFIRM:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
            gmac_tran_rel_state_handle_rel_cnf( global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released, gas_id );
#endif /*FEATURE_GSM_DTM*/

            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_DL_TBF_RELEASE_CONF)
            {
              transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;

              /* abnormal release, leave transfer */
              gmac_transfer_tbf_rel_confirm(UL_DL_TBF_RELEASE_CONF, gas_id);
            }
            else if((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF)
                     && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE))
            {

              /* wait for pending UL TBF to be established*/
              MSG_GERAN_MED_0_G("MAC wait for pending UL TBF to be connect ");

              /* note DL rel confrim can only occur if pkt_tbf_rel releasing DL(AB or NORM) has   */
              /* been rcvd (if RLC release the DL abnormal an UL+DL abnormal release is performed */
              /* if RLC release DL normal, then the DL rel to L1 will not have been sent yet      */


              /* clear 'tbf_release_pending_flg' flag */
              global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;

              /* wait for pending UL TBF to be established */

              /*if been waiting for a pending DL connection, reset flag as DL has been released*/
              if(transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl == TRUE)
              {
                 transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
              }

              /*clean up DL vars*/
              gmac_transfer_dl_release_clean(gas_id);

              /*switch state and wait for UL to connect*/
              MSG_GERAN_MED_0_G("MAC transfer state T_UL_DYNAMIC_REALLOC");
              transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC;
              (void)mac_log_state(gas_id);

            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released != UL_DL_TBF_RELEASE_CONF)
                           && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == TRUE))
            {
                /* This is the case where an abnormal release that results in both UL and DL tbf*/
                /* being terminated has been initiated while a release is already in progress and*/
                /* MAC is waiting for L1 to confirm the release. Under these conditions MAC must*/
                /* wait for an UL and DL rel confirm form L1 before progressing                 */
                MSG_GERAN_MED_1_G("ABNORMAL UL & DL rel in progress, require rel confirm for both UL and DL to continue. rel type = %d",
                                  global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);

            }
            else
            {

              if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_TBF_RELEASE_CONF)
              {
                transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;

                MSG_GERAN_MED_0_G("MAC info pending UL released before connection occured ");

                /* In cases where the release of the UL does not cause MAC to  */
                /* leave TRANSFER 'wait_for_ul_rel_confirm' flag must be reset */
                global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;

                /* note UL rel confrim can only occur if pkt_tbf_rel releasing UL (AB ) has been rcvd   */
                /* as the UL has never been connected there is no means by which RLC can release the UL */
                /* clear 'tbf_release_pending_flg' flag */

                global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;

                /*clear the pending UL PH_CONNECT_IND flg*/
                transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;

                /*clean up UL vars as pending UL never completed the connection process*/
                gmac_transfer_ul_dynamic_release_clean(gas_id);

                if (transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl == TRUE)
                {
                  /* waiting for a pending DL to connect, switch to TRANSFER_DL_REASSIGN*/
                  /* state and wait for PH_CONNECT_IND for the DL                       */

                  global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
                  MSG_GERAN_MED_0_G("GMAC State tran = T_DL_REASS");
                  transfer_data_ptr->mac_transfer_substate = TRANSFER_DL_REASSIGN;
                  (void)mac_log_state(gas_id);

                  /*reset flag ... only relevant to TRANSFER_DL_WITH_DYNAMIC_ALLOC */
                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
                }
                else
                {
                  /*NOT waiting for a pending DL to connect*/

                  if(transfer_data_ptr->t_flags.dl_tbf_normal_release_ind == TRUE)
                  {
                    /*When a DL tbf is NORMALLY released by RLC while waiting for the pending UL  */
                    /*to connect the release to L1 for the DL is put on hold till the UL connects.*/
                    /*In the case where the UL is terminated before connection the release of     */
                    /*halted DL needs to be completed                                             */

                    /*release L1 DL*/
                    gmac_transfer_dl_release_clean(gas_id);
                    gmac_transfer_dl_normal_release_to_idle(gas_id);

                  }
                  else
                  {
                    if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED)
                    {
                      gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
                    }
                  }

                  /*Return to T_DL to either continue the DL tbf OR if the DL is being released */
                  /*to handle the release confirm from L1 and complete the DL release process   */
                  global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;

                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
                  MSG_GERAN_MED_0_G("GMAC State tran = T_DL");
                  transfer_data_ptr->mac_transfer_substate = TRANSFER_DL;
                  (void)mac_log_state(gas_id);
                }
              }
              else
              {
                MSG_GERAN_HIGH_1_G("MAC Suspicious DL rel confirm, abnormal_ul_and_dl_rel_flg %d",
                                   transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg);
              }
            }

            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND");
            gmac_transfer_edtm_ph_cs_connect_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from L1 */
            break;
        }
      } /* end of MAC_L1 signals */
      else if(task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_DL_PKT_ACKNACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_ACKNACK_REQ");
            gmac_transfer_dl_pdan_handler(&(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_acknack_req),FALSE, gas_id);
            break;

          case RM_DL_PKT_CTRL_ACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_CTRL_ACK_REQ");
            transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = TRUE;
            gmac_rrbp_send_pca(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.frame_number,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.time_slot, TRUE,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.rrbp, PCA_DL_FAI_MSG,
                               gas_id);
            break;

          case RM_DL_RELEASE_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd RM_DL_RELEASE_IND %d",global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause);
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause == REL_DL_NORMAL)
            {
               MSG_GERAN_LOW_0_G("MAC hold DL until UL TBF connect");
               transfer_data_ptr->t_flags.dl_tbf_normal_release_ind = TRUE;
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,DONT_REL_RLC_DL,NO_DEL,FALSE,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_RLC_DL_RELEASE_IND,gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_RELEASE_IND,gas_id);
            }
            break;

          case RM_UL_RELEASE_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_RELEASE_IND");

            if(transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl == TRUE)
            {
              if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause != REL_UL_NORMAL)
              {
                MSG_GERAN_MED_0_G("MAC Rcvd UL abnormal release cause");
                if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause == REL_UL_ABNORMAL_CELL_RESEL)
                {
                  gmac_abnormal_release_procedure(RR_CELL_RESEL_REQ,
                                                  REL_RLC_DL,
                                                  NO_DEL,
                                                  FALSE,
                                                  gas_id);
                }
                else
                {
                  gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                                  REL_RLC_DL,
                                                  NO_DEL,
                                                  FALSE,
                                                  gas_id);
                }
                mac_log_ul_tbf_release(MAC_LOG_UL_RELEASE_IND, gas_id);
                mac_log_dl_tbf_release(MAC_LOG_DL_RLC_UL_RELEASE_IND, gas_id);
              }
              else
              {
                MSG_GERAN_ERROR_0_G("RM_UL_RELEASE_IND with normal cause not expected in this state");
              }
            }
            else
            {
               MSG_GERAN_ERROR_0_G("RM_UL_RELEASE_IND rxd when not waiting for DL con");
            }
            break;

          case RM_DL_FIRST_BLOCK_IND:
               /*ignore this signal when rcvd in transfer*/
            break;

          case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");
            gmac_tran_handle_unexpected_rlc_tbf_req(gas_id);
            break;

          case RM_UL_REALLOC_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_REALLOC_REQ");

            /* Store channel request parameters and set reallocation pending flag
               to be dealt with on completion of DL Assignment */
            if(transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl == TRUE)
            {
              MSG_GERAN_HIGH_0_G("MAC save RM_UL_REALLOC_REQ and handle after DL con");  

              global_data_ptr->gmac_transfer_ul_cv_zero_ind = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.b_cv_zero;

              if ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.pfi_present )
              {
                global_data_ptr->pfi_present = TRUE;
                global_data_ptr->pfi = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.pfi;
              }
              else
              {
                global_data_ptr->pfi_present = FALSE;
              }

              /* store RLC parameters and request quality report for PRR */
              global_data_ptr->gmac_local_stored_ul_chan_req = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.chan_req_desc;

              transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = TRUE;
            }
            else
            {
              MSG_GERAN_ERROR_0_G("RM_UL_REALLOC_REQ received when not waiting for DL con");
            }
            break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from RLC */
            break;
        }
      } /* end of MAC_RLC signals */
#ifdef FEATURE_GSM_EDTM
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {
          case MAC_TIMER_PKT_CS_REQ_RETRY:
            MSG_GERAN_MED_0_G("GMAC EDTM PKT CS REQ RETRY Timer expired");
            gmac_transfer_edtm_pkt_cs_req_handler(
              DL_ONLY,
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
              EDTM_PKT_CS_REQUEST_RETRY,
              gas_id);
          break;

          case MAC_TIMER_T3196:
            MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
            gmac_transfer_edtm_t3196_expiry_handler(gas_id);
          break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
          break;
        }
      }
#endif /*FEATURE_GSM_EDTM*/
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {
          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_LOW_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");
            if(transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set == TRUE)
            {
              /*When releaseing DL only PDAN's can be sent on the RRBP*/
              gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE, gas_id);
            }
            else
            {
              if(gmac_util_encode_ul_control_msg_handler(PMR,TRUE,gas_id) == PASS)
              {
                gmac_util_send_ul_control_msg_handler(PMR, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
              }
            }
            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if(gmac_util_encode_ul_control_msg_handler(PCCF,TRUE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCF, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND (del_pdu: 0x%x) in T_DL_WITH_DYN_ALLOC",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            gmac_tran_check_dl_tbf_status(gas_id);

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            REL_RLC_DL,
                                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                            FALSE,
                                            gas_id);

            transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
            mac_log_dl_tbf_release(MAC_LOG_DL_GPRS_SUSPENSION, gas_id);
            break;

          case GRR_MAC_SEND_PCCN_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCN");
            if(gmac_util_encode_ul_control_msg_handler(PCCN,TRUE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCN, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

          case GRR_MAC_SEND_PSS_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PSS");
            if(gmac_util_encode_ul_control_msg_handler(PSS,TRUE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PSS, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_2_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND: ps_rel:%d, ps_rel_cause:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            gmac_dtm_chan_rel_handler( &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind,
                                       UL_DL_TBF_RELEASE_CONF, gas_id);
            break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            MSG_GERAN_MED_3_G("GMAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ: dtm_tbf_type:0x%x, cs_pres:%d, dtm_freq_pres:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.cs_present,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_freq_present );

            if( gmac_tran_dtm_chan_ass_handler(
                  &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                  &transfer_data_ptr->gmac_transfer_ul_rlc_info_store,
                  UL_DL_TBF_PRESENT,
                  gas_id ) == NO_DTM_ASS_ERROR )
            {
#ifdef FEATURE_GSM_EDTM
              if ( global_data_ptr->gmac_edtm_enabled )
              {
                gmac_tran_edtm_ass_cmd_handler(
                     &transfer_data_ptr->mac_transfer_substate,
                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                      gas_id );

                if ( transfer_data_ptr->mac_transfer_substate != TRANSFER_DL_WITH_DYNAMIC_ALLOC )
                {
                    transfer_data_ptr->mac_tran_substate_str =
                      gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate,
                                                               gas_id);

                    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                }
              }
              else
#endif /*FEATURE_GSM_EDTM*/
              {
                transfer_data_ptr->mac_transfer_substate = TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC;

                MSG_GERAN_MED_0_G("GMAC State tran = TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC");
                (void)mac_log_state(gas_id);
              }
            }
          break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);

            gmac_transfer_edtm_pkt_cs_req_handler(
              DL_ONLY,
              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause,
              EDTM_FIRST_PKT_CS_REQUEST,
              gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from GRR */
            break;
        }
      } /* end of MAC_GRR signals */
      break; /* end of TRANSFER_DL_WITH_DYNAMIC_ALLOC */

    /***********************************************
    * transfer state TRANSFER_UL_DYNAMIC_DL_ASSIGN *
    ***********************************************/
    case TRANSFER_UL_DYNAMIC_DL_ASSIGN:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_DATA_IND:
            if(transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release == FALSE)
            {
             gmac_tdynamic_dl_assign_data_ind(gas_id);
            }
            break;

          case L1_MAC_USF_DETECTED:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_USF_DETECTED");
            gmac_tdynamic_usf_detected(gas_id);
            break;

          case MAC_PH_CONNECT_IND:
            /*while waiting for a pending UL to connect cell reselection or suspension for cs can be rcvd. */
            /*Under these conditions the rel process must complete IGNORE any attempts to connect the ul   */
            if(transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE)
            {
#ifdef FEATURE_GSM_DTM
              gmac_tran_rel_state_handle_connect_ind(
                global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind,
                gas_id );
#endif /*FEATURE_GSM_DTM*/

              MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");
              if (global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == DOWNLINK_TBF_ESTABLISHED)
              {
                gmac_dl_transfer_init(gas_id);

                GMAC_UPDATE_DL_TS_REASS;
                GMAC_UPDATE_DL_TFI;

                gmac_util_send_dl_ass_to_rlc(gas_id);

                global_data_ptr->gmac_local_tfi_store.dl_tfi_valid = TRUE;

                if (transfer_data_ptr->t_flags.ul_tbf_normal_release_ind)
                {
                  MSG_GERAN_MED_0_G("GMAC State tran = T_DL");
                  MSG_GERAN_MED_0_G("mac info wait_for_ul_rel_confirm = FALSE");
                  global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;

                  if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED)
                  {
                    gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
                  }

                  global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
                  transfer_data_ptr->mac_transfer_substate = TRANSFER_DL;
                  (void)mac_log_state(gas_id);
                  transfer_data_ptr->t_flags.ul_tbf_normal_release_ind = FALSE;

                  gmac_eng_mode_update_tbf_info((uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER, global_data_ptr->current_tbf_mode ,FALSE, gas_id);

                }
                else
                {
                  MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DYN");
                  transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;
                  (void)mac_log_state(gas_id);

                  if ( transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending )
                  {
                    /* Pending reallocation request can now be processed. Prepare to send PRR on PACCH */

                    transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
                    transfer_data_ptr->gmac_transfer_ul_request_count = 0;
                    global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
                    gmac_transfer_qual_meas_req_to_L1(gas_id);
                  }
                }
              }
              else
              {
                if(geran_get_nv_recovery_restart_enabled(gas_id))
                {
                  MSG_GERAN_ERROR_1_G("GMAC AB Rel, Expected DL (0x01) in PH_CONNECT_IND, received: 0x%x",
                                      global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                  MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                  geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                }
                else
                {
                  ERR_GERAN_FATAL_2_G("GMAC AB Rel, Expected DL (0x01) in PH_CONNECT_IND, received: 0x%x",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                }
              }

            }
            else
            {
              MSG_GERAN_MED_0_G("MAC info ABNORMAL UL & DL rel in progress,Ignore PH_CONNECT_IND");
            }
            break;

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("GMAC INFO L1 Freq Cnf ignored for freq cleanup");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }
          break;

          case L1_MAC_TBF_REL_CONFIRM:

            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
            gmac_tran_rel_state_handle_rel_cnf( global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released, gas_id );
#endif /*FEATURE_GSM_DTM*/

            transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_DL_TBF_RELEASE_CONF)
            {
              /* abnormal release, leave transfer */
              gmac_transfer_tbf_rel_confirm(UL_DL_TBF_RELEASE_CONF, gas_id);
            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_TBF_RELEASE_CONF)
                    && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE))
            {
              /* wait for pending DL TBF to be established */
              transfer_data_ptr->t_flags.ul_tbf_normal_release_ind = TRUE;
              global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released != UL_DL_TBF_RELEASE_CONF)
                           && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == TRUE))
            {
                /* This is the case where an abnormal release that results in both UL and DL tbf*/
                /* being terminated has been initiated while a release is already in progress and*/
                /* MAC is waiting for L1 to confirm the release. Under these conditions MAC must*/
                /* wait for an UL and DL rel confirm form L1 before progressing                 */
                MSG_GERAN_MED_1_G("ABNORMAL UL & DL rel in progress, require rel confirm for both UL and DL to continue. rel type = %d",
                                  global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);

            }
            else
            {
              if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF)
              {
                global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;

                /* Going to UL_DYNAMIC, if we are releasing the UL it will be handled in this state*/

                MSG_GERAN_MED_0_G("GMAC State tran = TRANSFER_UL_DYNAMIC");
                transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC;
              }
            }
            break;

          case L1_MAC_1ST_DATA_BLOCK_SENT:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_1ST_DATA_BLOCK_SENT");
            gmac_stop_t3164(gas_id);
            gmac_start_t3180(gas_id);
            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND");
            gmac_transfer_edtm_ph_cs_connect_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from L1 */
            break;
        }
      } /* end of MAC_L1 signals */
      else if(task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_UL_REALLOC_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_REALLOC_REQ");

            /* Store channel request parameters and set reallocation pending flag
               to be dealt with on completion of DL Assignment */

            global_data_ptr->gmac_transfer_ul_cv_zero_ind = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.b_cv_zero;

            if ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.pfi_present )
            {
              global_data_ptr->pfi_present = TRUE;
              global_data_ptr->pfi = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.pfi;
            }
            else
            {
              global_data_ptr->pfi_present = FALSE;
            }

            /* store RLC parameters and request quality report for PRR */
            global_data_ptr->gmac_local_stored_ul_chan_req = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.chan_req_desc;

            transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = TRUE;
            break;

          case RM_UL_RELEASE_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_RELEASE_IND");
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause != REL_UL_NORMAL)
            {
              MSG_GERAN_MED_0_G("MAC Rcvd UL abnormal release cause");
              if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause == REL_UL_ABNORMAL_CELL_RESEL)
              {
                gmac_abnormal_release_procedure(RR_CELL_RESEL_REQ,
                                                NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                                NO_DEL,
                                                FALSE,
                                                gas_id);
              }
              else
              {
                gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                                NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                                NO_DEL,
                                                FALSE,
                                                gas_id);
              }
              mac_log_ul_tbf_release(MAC_LOG_UL_RELEASE_IND, gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_RLC_UL_RELEASE_IND, gas_id);
            }
            else
            {
              MSG_GERAN_LOW_0_G("MAC UL TBF normal release");
              gmac_transfer_ul_dynamic_release_clean(gas_id);
              gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__, gas_id);
              gmac_tran_send_mac_l1_release(UL_TBF_NORMAL, gas_id);
              MSG_GERAN_LOW_0_G("MAC awaiting for PH_CONNECT_IND to establish DL TBF");
              transfer_data_ptr->t_flags.ul_tbf_normal_release_ind = TRUE;
              mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE, gas_id);
            }
            break;

          case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");
            gmac_tran_handle_unexpected_rlc_tbf_req(gas_id);
            break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from RLC */
            break;
        }
      } /* end of MAC_RLC signals */
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {
          case MAC_TIMER_T3180:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3180");
            /* Note if t_flags.ul_tbf_normal_release_ind is set then RLC has already been blocked (with NO_DEL).
            gmac_abnormal_release_procedure will send RLC a subsequent block (NO_DEL) which will
            be ignored by RLC */
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                            NO_DEL,
                                            FALSE,
                                            gas_id);

            mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3180, gas_id);
            mac_log_dl_tbf_release(MAC_LOG_DL_EXPIRED_T3180, gas_id);
            break;

          case MAC_TIMER_T3200_1:
          case MAC_TIMER_T3200_2:
          case MAC_TIMER_T3200_3:
          case MAC_TIMER_T3200_4:
          case MAC_TIMER_T3200_5:
          case MAC_TIMER_T3200_6:
          case MAC_TIMER_T3200_7:
          case MAC_TIMER_T3200_8:
               MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
               t3200_expiry_handler((mac_mac_sig_id_t)signal_id, gas_id );
          break;

#ifdef FEATURE_GSM_EDTM
          case MAC_TIMER_PKT_CS_REQ_RETRY:
            MSG_GERAN_MED_0_G("GMAC EDTM PKT CS REQ RETRY Timer expired");
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_ONLY,
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
              EDTM_PKT_CS_REQUEST_RETRY,
              gas_id);
          break;

          case MAC_TIMER_T3196:
            MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
            gmac_transfer_edtm_t3196_expiry_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_MAC signals */
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {
          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_LOW_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");
            if(gmac_util_encode_ul_control_msg_handler(PMR,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if(gmac_util_encode_ul_control_msg_handler(PCCF,FALSE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND (del_pdu: 0x%x) in T_UL_DYN_DL_ASS",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            gmac_tran_check_dl_tbf_status(gas_id);

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                            FALSE,
                                            gas_id);

            transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
            mac_log_ul_tbf_release(MAC_LOG_UL_GPRS_SUSPENSION, gas_id);
            break;

         case GRR_MAC_SEND_PCCN_REQ :
           MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCN");
           if(gmac_util_encode_ul_control_msg_handler(PCCN,FALSE,gas_id) == PASS)
           {
              gmac_util_send_ul_control_msg_handler(PCCN, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
           }
           break;

          case GRR_MAC_SEND_PSS_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PSS");
            if(gmac_util_encode_ul_control_msg_handler(PSS,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PSS, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_2_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND: ps_rel:%d, ps_rel_cause:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            gmac_dtm_chan_rel_handler( &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind,
                                       UL_DL_TBF_RELEASE_CONF,
                                       gas_id );
          break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            MSG_GERAN_MED_3_G("GMAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ: dtm_tbf_type:0x%x, cs_pres:%d, dtm_freq_pres:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.cs_present,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_freq_present );

            if( gmac_tran_dtm_chan_ass_handler(
                  &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                  &transfer_data_ptr->gmac_transfer_ul_rlc_info_store,
                  UL_DL_TBF_PRESENT,
                  gas_id ) == NO_DTM_ASS_ERROR )
            {
#ifdef FEATURE_GSM_EDTM
              if ( global_data_ptr->gmac_edtm_enabled )
              {
                gmac_tran_edtm_ass_cmd_handler(
                     &transfer_data_ptr->mac_transfer_substate,
                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                      gas_id );

                if ( transfer_data_ptr->mac_transfer_substate != TRANSFER_UL_DYNAMIC_DL_ASSIGN )
                {
                    transfer_data_ptr->mac_tran_substate_str =
                      gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate,
                                                               gas_id);

                    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                }
              }
              else
#endif /*FEATURE_GSM_EDTM*/
              {
                transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN;

                MSG_GERAN_MED_0_G("GMAC State tran = TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN");
                (void)mac_log_state(gas_id);
              }
            }
          break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_ONLY,
              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause,
              EDTM_FIRST_PKT_CS_REQUEST,
              gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from GRR */
            break;
        }
      } /* end of GRR_MAC signals */
      break; /* end of TRANSFER_UL_DYNAMIC_DL_ASSIGN */

    /****************************************************
    * transfer state TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC *
    ****************************************************/
    case TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_DATA_IND:
            gmac_transfer_dl_reassign_dynamic_alloc_data_ind(gas_id);
            break;

          case L1_MAC_QUAL_MEASURMENTS:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_QUAL_MEASURMENTS");
            gmac_transfer_dl_pkt_acknack(&(global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements), gas_id);
            break;

          case MAC_PH_CONNECT_IND:
            /*while waiting for a pending UL to connect cell reselection or suspension for cs can be rcvd. */
            /*Under these conditions the rel process must complete IGNORE any attempts to connect the ul   */
            if(transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE)
            {
#ifdef FEATURE_GSM_DTM
              gmac_tran_rel_state_handle_connect_ind(
                global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind,
                gas_id );
#endif /*FEATURE_GSM_DTM*/

              MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");
              if (global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_DOWNLINK_TBF_ESTABLISHED)
              {
#ifdef FEATURE_GSM_DTM
                transfer_data_ptr->t_flags.dtm_realloc_in_progress = FALSE;
#endif /*FEATURE_GSM_DTM*/

                /* DL reassign */
                transfer_data_ptr->t_flags.dl_tbf_normal_release_ind = FALSE;

                /* UL alloc */
                gmac_ul_transfer_init(gas_id);

                global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = TRUE;

                GMAC_UPDATE_UL_TS_REALLOC;
                GMAC_UPDATE_UL_DLCTRL_TS_REALLOC;
                GMAC_UPDATE_DL_TS_REASS;

                GMAC_UPDATE_UL_TFI;
                GMAC_UPDATE_DL_TFI;


                if (transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag)
                {
                  /* send MAC_L1_CS_RECONFIG due to CS change */
                  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_CS_RECONFIG");
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_id = (uint8)MAC_L1_CS_RECONFIG;
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_set = MS_MAC_L1;
                  gprs_l1_mac_acc_func(&transfer_data_ptr->mac_l1_sig_cs_reconfig, gas_id);

                  global_data_ptr->gmac_local_cs_setting = transfer_data_ptr->pua_ptr_cs_change;
                }

                transfer_data_ptr->mr_ul_alloc_cnf_ptr = grlc_get_cmd_buf_mac();
                if (transfer_data_ptr->mr_ul_alloc_cnf_ptr != NULL)
                {
                  MSG_GERAN_MED_0_G("MAC Sent MR_UL_ALLOC_CNF");

                  transfer_data_ptr->mr_ul_alloc_cnf_ptr->msg.ul_alloc_cnf
                    = transfer_data_ptr->gmac_transfer_ul_rlc_info_store;

                  gmac_rlc_mac_insert_gas_id(transfer_data_ptr->mr_ul_alloc_cnf_ptr,gas_id);
                  grlc_put_cmd_mac(gas_id,transfer_data_ptr->mr_ul_alloc_cnf_ptr);
                }
                else
                {
                  ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
                }

                /*For HMSC RLC needs to know the Number of DL ts so irrespective of */
                /*EGPRS or GPRS mode always send MR_DL_ASS_IND to RLC               */
                gmac_util_send_dl_ass_to_rlc(gas_id);


                /*CR 96169 - start T3180 when entering concurrency from a DL state.
                  This guards against the possibility of MAC never timing out */
                gmac_start_t3180(gas_id);

                MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DYN");
                transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;

                /* Any pending PCCN to be sent on the DL RRBP is switched and sent on the UL pacch*/

                if(global_data_ptr->gmac_local_pccn_params.pccn_pending == PCCN_PENDING_DL)
                {
                  gmac_util_send_ul_control_msg_handler(PCCN, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
                }

                /*cr68663*/
                if(global_data_ptr->gmac_local_pmr_params.pmr_pending == TRUE)
                {
                  gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
                }

                gmac_start_t3164_transfer(gas_id);

                (void)mac_log_state(gas_id);

                gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, global_data_ptr->current_tbf_mode, FALSE, gas_id);

              }
              else
              {
                if(geran_get_nv_recovery_restart_enabled(gas_id))
                {
                  MSG_GERAN_ERROR_1_G("GMAC AB Rel, Expected UL_DL (0x02) in PH_CONNECT_IND, received: 0x%x",
                                      global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                  MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                  geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                }
                else
                {
                  ERR_GERAN_FATAL_2_G("GMAC AB Rel, Expected UL_DL (0x02) in PH_CONNECT_IND, received: 0x%x",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                }
              }

            }
            else
            {
              MSG_GERAN_MED_0_G("MAC info ABNORMAL UL & DL rel in progress,Ignore PH_CONNECT_IND");
            }
            break;

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("GMAC INFO L1 Freq Cnf ignored for freq cleanup");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }
          break;

          case L1_MAC_TBF_REL_CONFIRM:

            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
            gmac_tran_rel_state_handle_rel_cnf( global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released, gas_id );
#endif /*FEATURE_GSM_DTM*/

            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_DL_TBF_RELEASE_CONF)
            {
              transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;

              /* abnormal release, leave transfer */
              gmac_transfer_tbf_rel_confirm(UL_DL_TBF_RELEASE_CONF, gas_id);
            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF)
                      && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE))
            {

              /* wait for pending UL TBF to be established */
              MSG_GERAN_MED_0_G("MAC wait for pending UL TBF to be connect ");

              /* note DL rel confrim can only occur if pkt_tbf_rel releasing DL(AB or NORM) has   */
              /* been rcvd (if RLC release the DL abnormal an UL+DL abnormal release is performed */
              /* if RLC release DL normal, then the DL rel to L1 will not have been sent yet      */


              /* clear 'tbf_release_pending_flg' flag */
              global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;

              /* wait for pending UL TBF to be established */

              /*if been waiting for a pending DL connection, reset flag as DL has been released*/
              if(transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl == TRUE)
              {
                 transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
              }

              /*clean up DL vars*/
              gmac_transfer_dl_release_clean(gas_id);

              /*switch state and wait for UL to connect*/
              MSG_GERAN_MED_0_G("MAC transfer state T_UL_DYNAMIC_REALLOC");
              transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC;
              (void)mac_log_state(gas_id);

            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released != UL_DL_TBF_RELEASE_CONF)
                           && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == TRUE))
            {
                /* This is the case where an abnormal release that results in both UL and DL tbf*/
                /* being terminated has been initiated while a release is already in progress and*/
                /* MAC is waiting for L1 to confirm the release. Under these conditions MAC must*/
                /* wait for an UL and DL rel confirm form L1 before progressing                 */
                MSG_GERAN_MED_1_G("ABNORMAL UL & DL rel in progress, require rel confirm for both UL and DL to continue. rel type = %d",
                                  global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);

            }
            else
            {

              if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_TBF_RELEASE_CONF)
              {
                transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;

                MSG_GERAN_MED_0_G("MAC info pending UL released before connection occured ");

                /* In cases where the release of the UL does not cause MAC to  */
                /* leave TRANSFER 'wait_for_ul_rel_confirm' flag must be reset */
                global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;

                /* note UL rel confrim can only occur if pkt_tbf_rel releasing UL (AB ) has been rcvd   */
                /* as the UL has never been connected there is no means by which RLC can release the UL */
                /* clear 'tbf_release_pending_flg' flag */

                global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;

                /*clear the pending UL PH_CONNECT_IND flg*/
                transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;

                if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED)
                {
                  gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
                }

                /*clean up UL vars as pending UL never completed the connection process*/
                gmac_transfer_ul_dynamic_release_clean(gas_id);

                global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
                MSG_GERAN_MED_0_G("GMAC State tran = T_DL_REASS");
                transfer_data_ptr->mac_transfer_substate = TRANSFER_DL_REASSIGN;
                (void)mac_log_state(gas_id);

                gmac_eng_mode_update_tbf_info((uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER, global_data_ptr->current_tbf_mode,FALSE, gas_id);

              }
              else
              {
                MSG_GERAN_HIGH_1_G("MAC Suspicious DL rel confirm, abnormal_ul_and_dl_rel_flg %d",
                                   transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg);
              }
            }
            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND");
            gmac_transfer_edtm_ph_cs_connect_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown  signal from L1 */
            break;
        }
      } /* end of MAC_L1 signals */
      else if(task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_DL_PKT_ACKNACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_ACKNACK_REQ");
            gmac_transfer_dl_pdan_handler(&(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_acknack_req),FALSE, gas_id);
            break;

          case RM_DL_PKT_CTRL_ACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_CTRL_ACK_REQ");
            transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = TRUE;
            gmac_rrbp_send_pca(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.frame_number,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.time_slot, TRUE,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.rrbp, PCA_DL_FAI_MSG, gas_id);
            break;

          case RM_DL_RELEASE_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd RM_DL_RELEASE_IND %d",global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause);
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause == REL_DL_NORMAL)
            {
               MSG_GERAN_LOW_0_G("MAC hold DL until UL TBF connect");
               transfer_data_ptr->t_flags.dl_tbf_normal_release_ind = TRUE;
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,DONT_REL_RLC_DL,NO_DEL,FALSE, gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_RLC_DL_RELEASE_IND, gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_RELEASE_IND, gas_id);
            }
          break;

          case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");
            gmac_tran_handle_unexpected_rlc_tbf_req(gas_id);
            break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from RLC */
            break;
        }
      } /* end of MAC_RLC signals */
#ifdef FEATURE_GSM_EDTM
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {
          case MAC_TIMER_PKT_CS_REQ_RETRY:
            MSG_GERAN_MED_0_G("GMAC EDTM PKT CS REQ RETRY Timer expired");
            gmac_transfer_edtm_pkt_cs_req_handler(
              DL_ONLY,
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
              EDTM_PKT_CS_REQUEST_RETRY,
              gas_id);
          break;

          case MAC_TIMER_T3196:
            MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
            gmac_transfer_edtm_t3196_expiry_handler(gas_id);
          break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
          break;
        }
      }
#endif /*FEATURE_GSM_EDTM*/
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {
          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_LOW_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");
            if(transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set == TRUE)
            {
              /*When releaseing DL only PDAN's can be sent on the RRBP*/
              gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE, gas_id);
            }
            else
            {
              if(gmac_util_encode_ul_control_msg_handler(PMR,TRUE, gas_id) == PASS)
              {
                gmac_util_send_ul_control_msg_handler(PMR, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
              }
            }
            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if(gmac_util_encode_ul_control_msg_handler(PCCF,TRUE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCF, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND (del_pdu: 0x%x) in T_DL_REASS_DYN_ALLOC",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            gmac_tran_check_dl_tbf_status(gas_id);

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            REL_RLC_DL,
                                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                            FALSE,
                                            gas_id);

            transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
            mac_log_dl_tbf_release(MAC_LOG_DL_GPRS_SUSPENSION, gas_id);
            break;

          case GRR_MAC_SEND_PCCN_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCN");
            if(gmac_util_encode_ul_control_msg_handler(PCCN,TRUE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCN, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

          case GRR_MAC_SEND_PSS_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PSS");
            if(gmac_util_encode_ul_control_msg_handler(PSS,TRUE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PSS, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_2_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND: ps_rel:%d, ps_rel_cause:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            gmac_dtm_chan_rel_handler( &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind,
                                       UL_DL_TBF_RELEASE_CONF,
                                       gas_id );
            break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            MSG_GERAN_MED_3_G("GMAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ: dtm_tbf_type:0x%x, cs_pres:%d, dtm_freq_pres:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.cs_present,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_freq_present );

            if ( gmac_tran_dtm_chan_ass_handler(
                   &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                   &transfer_data_ptr->gmac_transfer_ul_rlc_info_store,
                   UL_DL_TBF_PRESENT,
                   gas_id ) == NO_DTM_ASS_ERROR)
            {

#ifdef FEATURE_GSM_EDTM
              if ( global_data_ptr->gmac_edtm_enabled )
              {
                gmac_tran_edtm_ass_cmd_handler(
                     &transfer_data_ptr->mac_transfer_substate,
                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                      gas_id );

                if ( transfer_data_ptr->mac_transfer_substate != TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC )
                {
                    transfer_data_ptr->mac_tran_substate_str =
                      gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate, gas_id);

                    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                }
              }
#endif /*FEATURE_GSM_EDTM*/
            }
          break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);
            gmac_transfer_edtm_pkt_cs_req_handler(
              DL_ONLY,
              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause,
              EDTM_FIRST_PKT_CS_REQUEST,
              gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from GRR */
            break;
        }
      } /* end of MAC_GRR signals */
      break; /* end of TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC */

    /*******************************************************
    * transfer state TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN *
    *******************************************************/
    case TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_DATA_IND:
            if(transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release == FALSE)
            {
              gmac_tdynamic_realloc_dl_assign_data_ind(gas_id);
            }
            break;

          case L1_MAC_USF_DETECTED:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_USF_DETECTED");
            gmac_tdynamic_usf_detected(gas_id);
            break;

          case MAC_PH_CONNECT_IND:
            /*while waiting for a pending UL to connect cell reselection or suspension for cs can be rcvd. */
            /*Under these conditions the rel process must complete IGNORE any attempts to connect the ul   */
            if(transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE)
            {
#ifdef FEATURE_GSM_DTM
              gmac_tran_rel_state_handle_connect_ind(
                global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind,
                gas_id );
#endif /*FEATURE_GSM_DTM*/

              MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");
              if (global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_DOWNLINK_TBF_ESTABLISHED)
              {
                /* DL assign */
                gmac_dl_transfer_init(gas_id);

                GMAC_UPDATE_UL_TFI;
                GMAC_UPDATE_DL_TFI;

                global_data_ptr->gmac_local_tfi_store.dl_tfi_valid = TRUE;

                /* UL assign */
                transfer_data_ptr->t_flags.ul_tbf_normal_release_ind = FALSE;

#ifdef FEATURE_GSM_DTM
                transfer_data_ptr->t_flags.dtm_realloc_in_progress = FALSE;
#endif /*FEATURE_GSM_DTM*/

                GMAC_UPDATE_UL_TS_REALLOC;
                GMAC_UPDATE_UL_DLCTRL_TS_REALLOC;
                GMAC_UPDATE_DL_TS_REASS;

                gmac_util_send_dl_ass_to_rlc(gas_id);


                if (transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag)
                {
                  /* send MAC_L1_CS_RECONFIG due to CS change */
                  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_CS_RECONFIG");
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_id = (uint8)MAC_L1_CS_RECONFIG;
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_set = MS_MAC_L1;
                  gprs_l1_mac_acc_func(&transfer_data_ptr->mac_l1_sig_cs_reconfig,gas_id);

                  global_data_ptr->gmac_local_cs_setting = transfer_data_ptr->pua_ptr_cs_change;
                }

                transfer_data_ptr->mr_ul_alloc_cnf_ptr = grlc_get_cmd_buf_mac();
                if (transfer_data_ptr->mr_ul_alloc_cnf_ptr != NULL)
                {
                  MSG_GERAN_MED_0_G("MAC Sent MR_UL_ALLOC_CNF");
                  transfer_data_ptr->mr_ul_alloc_cnf_ptr->msg.ul_alloc_cnf = transfer_data_ptr->gmac_transfer_ul_rlc_info_store;
                  gmac_rlc_mac_insert_gas_id(transfer_data_ptr->mr_ul_alloc_cnf_ptr,gas_id);
                  grlc_put_cmd_mac(gas_id,transfer_data_ptr->mr_ul_alloc_cnf_ptr);
                }
                else
                {
                  ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
                }

                MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DYN");
                transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;

                gmac_start_t3164_transfer(gas_id);

                (void)mac_log_state(gas_id);

                gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, global_data_ptr->current_tbf_mode, FALSE, gas_id);

                if ( transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending )
                {
                  /* Pending reallocation request can now be processed. Prepare to send PRR on PACCH */

                  transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
                  transfer_data_ptr->gmac_transfer_ul_request_count = 0;
                  global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
                  gmac_transfer_qual_meas_req_to_L1(gas_id);
                }
              }
              else
              {
                if(geran_get_nv_recovery_restart_enabled(gas_id))
                {
                  MSG_GERAN_ERROR_1_G("GMAC AB Rel, Expected UL_DL (0x02) in PH_CONNECT_IND, received: 0x%x",
                                      global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                  MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                  geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                }
                else
                {
                  ERR_GERAN_FATAL_2_G("MAC AB Rel, Expected UL_DL (0x02) in PH_CONNECT_IND, received: 0x%x",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                }
              }

            }
            else
            {
              MSG_GERAN_MED_0_G("MAC info ABNORMAL UL & DL rel in progress,Ignore PH_CONNECT_IND");
            }
            break;

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("GMAC INFO L1 Freq Cnf ignored for freq cleanup");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }
          break;

          case L1_MAC_TBF_REL_CONFIRM:

            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
            gmac_tran_rel_state_handle_rel_cnf( global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released, gas_id );
#endif /*FEATURE_GSM_DTM*/

            transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_DL_TBF_RELEASE_CONF)
            {
              /* abnormal release, leave transfer */
              gmac_transfer_tbf_rel_confirm(UL_DL_TBF_RELEASE_CONF, gas_id);
            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_TBF_RELEASE_CONF)
                    && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE))
            {
              /* wait for pending DL TBF to be established */
              transfer_data_ptr->t_flags.ul_tbf_normal_release_ind = TRUE;
              global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;

              /*we are now waiting for the DL to connect before releasing RLC. This is best done in the
                TRANSFER_UL_DYNAMIC_DL_ASSIGN state as it is designed to handle the case were the UL has been
                released whilst waiting for the DL connection*/
              MSG_GERAN_MED_0_G("GMAC State tran = TRANSFER_UL_DYNAMIC_DL_ASSIGN");
              transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_DL_ASSIGN;

              if(global_data_ptr->gmac_local_pccn_params.pccn_pending  != NO_PENDING_PCCN)
              {
                gmac_util_send_ul_control_msg_handler(PCCN, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
              }

            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released != UL_DL_TBF_RELEASE_CONF)
                           && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == TRUE))
            {
                /* This is the case where an abnormal release that results in both UL and DL tbf*/
                /* being terminated has been initiated while a release is already in progress and*/
                /* MAC is waiting for L1 to confirm the release. Under these conditions MAC must*/
                /* wait for an UL and DL rel confirm form L1 before progressing                 */
                MSG_GERAN_MED_1_G("ABNORMAL UL & DL rel in progress, require rel confirm for both UL and DL to continue. rel type = %d",
                                  global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);

            }
            else
            {
              if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF)
              {
                /*The DL has been released before the connect ind for UL+DL so we are now waiting for the UL connect ind.*/
                /*This is best handled in the UL REALLOC state*/
                MSG_GERAN_MED_0_G("GMAC State tran = TRANSFER_UL_DYNAMIC_REALLOC");
                transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC;
              }
            }
            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND");
            gmac_transfer_edtm_ph_cs_connect_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from L1 */
            break;
        }
      } /* end of MAC_L1 signals */
      else if(task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_UL_REALLOC_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_REALLOC_REQ");

            /* Store channel request parameters and set reallocation pending flag
               to be dealt with on completion of DL Assignment */

            global_data_ptr->gmac_transfer_ul_cv_zero_ind = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.b_cv_zero;
            global_data_ptr->gmac_local_stored_ul_chan_req = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.chan_req_desc;
            transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = TRUE;
            break;

          case RM_UL_RELEASE_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_RELEASE_IND");
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause != REL_UL_NORMAL)
            {
              MSG_GERAN_MED_0_G("MAC Rcvd UL abnormal release cause");
              if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause == REL_UL_ABNORMAL_CELL_RESEL)
              {
                gmac_abnormal_release_procedure(RR_CELL_RESEL_REQ,
                                                NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                                NO_DEL,
                                                FALSE,
                                                gas_id);
              }
              else
              {
                gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                                NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                                NO_DEL,
                                                FALSE,
                                                gas_id);
              }
              mac_log_ul_tbf_release(MAC_LOG_UL_RELEASE_IND, gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_RLC_UL_RELEASE_IND, gas_id);
            }
            else
            {
              MSG_GERAN_LOW_0_G("MAC UL TBF normal release");
              gmac_transfer_ul_dynamic_release_clean(gas_id);
              gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__, gas_id);
              gmac_tran_send_mac_l1_release(UL_TBF_NORMAL, gas_id);
              MSG_GERAN_LOW_0_G("MAC awaiting for PH_CONNECT_IND to establish DL TBF");
              transfer_data_ptr->t_flags.ul_tbf_normal_release_ind = TRUE;
              mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE, gas_id);
            }
            break;

          case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");
            gmac_tran_handle_unexpected_rlc_tbf_req(gas_id);
            break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from RLC */
            break;
        }
      } /* end of MAC_RLC signals */
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {
          case MAC_TIMER_T3180:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3180");
            /* Note if t_flags.ul_tbf_normal_release_ind is set then RLC has already been blocked (with NO_DEL).
               gmac_abnormal_release_procedure will send RLC a subsequent block (NO_DEL) which will
               be ignored */
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                            NO_DEL,
                                            FALSE,
                                            gas_id);

            mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3180, gas_id);
            mac_log_dl_tbf_release(MAC_LOG_DL_EXPIRED_T3180, gas_id);
            break;

          case MAC_TIMER_T3200_1:
          case MAC_TIMER_T3200_2:
          case MAC_TIMER_T3200_3:
          case MAC_TIMER_T3200_4:
          case MAC_TIMER_T3200_5:
          case MAC_TIMER_T3200_6:
          case MAC_TIMER_T3200_7:
          case MAC_TIMER_T3200_8:
               MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
               t3200_expiry_handler((mac_mac_sig_id_t)signal_id, gas_id );
          break;

#ifdef FEATURE_GSM_EDTM
          case MAC_TIMER_PKT_CS_REQ_RETRY:
            MSG_GERAN_MED_0_G("GMAC EDTM PKT CS REQ RETRY Timer expired");
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_ONLY,
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
              EDTM_PKT_CS_REQUEST_RETRY,
              gas_id);
          break;

          case MAC_TIMER_T3196:
            MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
            gmac_transfer_edtm_t3196_expiry_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_MAC signals */
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {
          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_LOW_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");
            if(gmac_util_encode_ul_control_msg_handler(PMR,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if(gmac_util_encode_ul_control_msg_handler(PCCF,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND (del_pdu: 0x%x) in T_UL_DYN_REALLOC_DL_ASS",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            gmac_tran_check_dl_tbf_status(gas_id);

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            NO_RLC_DL_TBF_PRESENT_TO_RELEASE,
                                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                            FALSE,
                                            gas_id);

            transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
            mac_log_dl_tbf_release(MAC_LOG_DL_GPRS_SUSPENSION, gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_GPRS_SUSPENSION, gas_id);
            break;

          case GRR_MAC_SEND_PCCN_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCN");
            if(gmac_util_encode_ul_control_msg_handler(PCCN,FALSE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCN, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

          case GRR_MAC_SEND_PSS_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PSS");
            if(gmac_util_encode_ul_control_msg_handler(PSS,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PSS, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_2_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND: ps_rel:%d, ps_rel_cause:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            gmac_dtm_chan_rel_handler( &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind,
                                       UL_DL_TBF_RELEASE_CONF,
                                       gas_id );
            break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            MSG_GERAN_MED_3_G("GMAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ: dtm_tbf_type:0x%x, cs_pres:%d, dtm_freq_pres:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.cs_present,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_freq_present );

            if ( gmac_tran_dtm_chan_ass_handler(
                   &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                   &transfer_data_ptr->gmac_transfer_ul_rlc_info_store,
                   UL_DL_TBF_PRESENT,
                   gas_id ) == NO_DTM_ASS_ERROR )
            {
#ifdef FEATURE_GSM_EDTM
              if ( global_data_ptr->gmac_edtm_enabled )
              {
                gmac_tran_edtm_ass_cmd_handler(
                     &transfer_data_ptr->mac_transfer_substate,
                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                      gas_id );

                if ( transfer_data_ptr->mac_transfer_substate != TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN )
                {
                    transfer_data_ptr->mac_tran_substate_str =
                      gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate,
                                                               gas_id);

                    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                }
              }
#endif /*FEATURE_GSM_EDTM*/
            }
          break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_ONLY,
              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause,
              EDTM_FIRST_PKT_CS_REQUEST,
              gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from GRR */
            break;
        }
      } /* end of GRR_MAC signals */
      break; /* end of TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN */

    /******************************************************
    * transfer state TRANSFER_CONCURRENT_DYNAMIC_TRANSFER *
    ******************************************************/
    case TRANSFER_CONCURRENT_DYNAMIC_TRANSFER:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_DATA_IND:
            if(transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release == FALSE)
            {
              gmac_tdynamic_concurrent_data_ind(gas_id);
            }
            break;

          case L1_MAC_USF_DETECTED:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_USF_DETECTED");
            gmac_tdynamic_usf_detected(gas_id);

            if (global_data_ptr->c_flags.gmac_prr_on_pacch_state == GMAC_PRR_ON_PACCH_AWAIT_USF)
            {
              transfer_data_ptr->gmac_transfer_ul_request_count++;
              rr_read_public_store(RRPS_CELL_OPTIONS_T3168,&transfer_data_ptr->t3168_time,gas_id);
              gmac_start_t3168(transfer_data_ptr->t3168_time,gas_id);
              global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_REQ;
            }
            break;

          case L1_MAC_QUAL_MEASURMENTS:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_QUAL_MEASURMENTS");
            if (global_data_ptr->c_flags.gmac_prr_on_pacch_state == GMAC_PRR_ON_PACCH_AWAIT_QUAL)
            {
              /*preload L1 with PRR, note PRR only sent once USF has been */
              /*detected at which point timer t3168 should be started      */
              global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_USF;
              gmac_send_prr(NO_RE_EST,FALSE,gas_id);
            }
            else
            {
              gmac_transfer_dl_pkt_acknack(&(global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements), gas_id);
            }
            break;

          case MAC_PH_CONNECT_IND:
            /*while waiting for a pending UL to connect cell reselection or suspension for cs can be rcvd. */
            /*Under these conditions the rel process must complete IGNORE any attempts to connect the ul   */
            if(transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE)
            {
#ifdef FEATURE_GSM_DTM
              gmac_tran_rel_state_handle_connect_ind(
                global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind,
                gas_id );
#endif /*FEATURE_GSM_DTM*/

              MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");
              if (global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_DOWNLINK_TBF_ESTABLISHED)
              {
                GMAC_UPDATE_UL_TS_REALLOC;
                GMAC_UPDATE_UL_DLCTRL_TS_REALLOC;
                GMAC_UPDATE_DL_TS_REASS;

                GMAC_UPDATE_UL_TFI;
                GMAC_UPDATE_DL_TFI;

                if (transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag)
                {
                  /* send MAC_L1_CS_RECONFIG due to CS change */
                  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_CS_RECONFIG");
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_id = (uint8)MAC_L1_CS_RECONFIG;
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_set = MS_MAC_L1;
                  gprs_l1_mac_acc_func(&transfer_data_ptr->mac_l1_sig_cs_reconfig,
                                        gas_id);

                  global_data_ptr->gmac_local_cs_setting = transfer_data_ptr->pua_ptr_cs_change;
                }

                /*For HMSC RLC needs to know the Number of DL ts so irrespective of */
                /*EGPRS or GPRS mode always send MR_DL_ASS_IND to RLC               */
                gmac_util_send_dl_ass_to_rlc(gas_id);


                transfer_data_ptr->mr_ul_alloc_cnf_ptr = grlc_get_cmd_buf_mac();
                if (transfer_data_ptr->mr_ul_alloc_cnf_ptr != NULL)
                {
                  MSG_GERAN_MED_0_G("MAC Sent MR_UL_ALLOC_CNF");
                  transfer_data_ptr->mr_ul_alloc_cnf_ptr->msg.ul_alloc_cnf = transfer_data_ptr->gmac_transfer_ul_rlc_info_store;
                  gmac_rlc_mac_insert_gas_id(transfer_data_ptr->mr_ul_alloc_cnf_ptr,gas_id);
                  grlc_put_cmd_mac(gas_id,transfer_data_ptr->mr_ul_alloc_cnf_ptr);
                }
                else
                {
                  ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
                }

                gmac_eng_mode_update_tbf_info((uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER, global_data_ptr->current_tbf_mode,FALSE,gas_id);

              }
              else
              {
                if(geran_get_nv_recovery_restart_enabled(gas_id))
                {
                  MSG_GERAN_ERROR_1_G("GMAC AB Rel, Expected UL_DL (0x02) in PH_CONNECT_IND, received: 0x%x",
                                      global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                  MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                  geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                }
                else
                {
                  ERR_GERAN_FATAL_2_G("GMAC AB Rel, Expected UL_DL (0x02) in PH_CONNECT_IND, received: 0x%x",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                }
              }
            }
            else
            {
              MSG_GERAN_MED_0_G("MAC info ABNORMAL UL & DL rel in progress,Ignore PH_CONNECT_IND");
            }
            break;

          case L1_MAC_TBF_REL_CONFIRM:
#ifdef FEATURE_GPRS_PS_HANDOVER
            gmac_tran_concurrent_process_rel_cnf(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released,
                                                 gas_id);
#else /*FEATURE_GPRS_PS_HANDOVER*/
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
            gmac_tran_rel_state_handle_rel_cnf( global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released,
                                                gas_id );
#endif /*FEATURE_GSM_DTM*/

            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_DL_TBF_RELEASE_CONF)
            {
              /* for abnormal release case, therefore release to NULL/IDLE */
              gmac_transfer_tbf_rel_confirm(UL_DL_TBF_RELEASE_CONF, gas_id);
              transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF)
                    && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE))
            {
              MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYN");

              /* flag set when RM_DL_RELEASE_IND is released during concurrent*/
              /* tbf operation reset if L1 confirms dl before ul              */

              MSG_GERAN_MED_1_G("mac info wait_for_ul_rel_confirm = %d",global_data_ptr->c_flags.wait_for_ul_rel_confirm);
              if (global_data_ptr->c_flags.wait_for_ul_rel_confirm == TRUE)
              {
                global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
              }
              else
              {
                global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
              }

              transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC;
              (void)mac_log_state(gas_id);
            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released != UL_DL_TBF_RELEASE_CONF)
                           && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == TRUE))
            {
                /* This is the case where an abnormal release that results in both UL and DL tbf*/
                /* being terminated has been initiated while a release is already in progress and*/
                /* MAC is waiting for L1 to confirm the release. Under these conditions MAC must*/
                /* wait for an UL and DL rel confirm form L1 before progressing                 */
                MSG_GERAN_MED_1_G("ABNORMAL UL & DL rel in progress, require rel confirm for both UL and DL to continue. rel type = %d",
                                  global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);

            }
            else /* UL_TBF_RELEASE_CONF */
            {
              MSG_GERAN_MED_0_G("GMAC State tran = T_DL");
              global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
              MSG_GERAN_MED_0_G("mac info wait_for_ul_rel_confirm = FALSE");
              global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;

              /*ensure that RLC is not blocked if returning to T_DL_REASS */
              if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
              {
                if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED)
                {
                  gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
                }
              }
              else
              {
                MSG_GERAN_HIGH_0_G("GMAC T3172 still running leave RLC blocked");
              }

              transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release = FALSE;
              global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
              transfer_data_ptr->mac_transfer_substate = TRANSFER_DL;
              transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
              global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
              (void)mac_log_state(gas_id);

              gmac_eng_mode_update_tbf_info((uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER, global_data_ptr->current_tbf_mode,FALSE,gas_id);

              if(global_data_ptr->gmac_local_pccn_params.pccn_pending  != NO_PENDING_PCCN)
              {
                gmac_util_send_ul_control_msg_handler(PCCN, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
              }

            }
#endif /*FEATURE_GPRS_PS_HANDOVER*/
            break;

          case L1_MAC_1ST_DATA_BLOCK_SENT:
               /*ignore this signal when rcvd in transfer*/
            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND");
            gmac_transfer_edtm_ph_cs_connect_handler(gas_id);
          break;

          case L1_MAC_FREQ_ASS_CNF:
            if ( global_data_ptr->gmac_edtm_enabled &&
                 transfer_data_ptr->t_flags.edtm_realloc_in_progress )
            {
              MSG_GERAN_MED_0_G("GMAC L1_MAC_FREQ_ASS_CNF rxd for EDTM in TRANSFER_DL");
            }
            else
            {
              MSG_GERAN_ERROR_0_G("GMAC L1_MAC_FREQ_ASS_CNF not expected in TRANSFER_DL");
            }
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from L1 */
            break;
        }
      } /* end of MAC_L1 signals */
      else if(task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_UL_REALLOC_REQ:

            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_REALLOC_REQ");
            transfer_data_ptr->gmac_transfer_ul_request_count = 0;

            if ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.pfi_present )
            {
              global_data_ptr->pfi_present = TRUE;
              global_data_ptr->pfi = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.pfi;
            }
            else
            {
              global_data_ptr->pfi_present = FALSE;
            }

            global_data_ptr->gmac_transfer_ul_cv_zero_ind = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.b_cv_zero;

            /* store RLC parameters and request quality report for PRR */
            global_data_ptr->gmac_local_stored_ul_chan_req = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.chan_req_desc;
            global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
            gmac_transfer_qual_meas_req_to_L1(gas_id);
            break;

          case RM_UL_RELEASE_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_RELEASE_IND");
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause != REL_UL_NORMAL)
            {
              MSG_GERAN_MED_0_G("MAC Rcvd UL abnormal release cause");
              if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause == REL_UL_ABNORMAL_CELL_RESEL)
              {
                gmac_abnormal_release_procedure(RR_CELL_RESEL_REQ,REL_RLC_DL,NO_DEL,FALSE,gas_id);
              }
              else
              {
                gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
              }
              mac_log_dl_tbf_release(MAC_LOG_DL_RLC_UL_RELEASE_IND,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_RELEASE_IND,gas_id);
            }
            else
            {
              MSG_GERAN_MED_0_G("MAC Rcvd UL normal release cause");
              gmac_transfer_ul_dynamic_release_clean(gas_id);
              gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__,gas_id);
              gmac_tran_send_mac_l1_release(UL_TBF_NORMAL,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE,gas_id);
            }
            break;

          case RM_DL_PKT_ACKNACK_REQ:
            MSG_GERAN_LOW_0_G("MAC Rcvd RM_DL_PKT_ACKNACK_REQ");
            gmac_transfer_dl_pdan_handler(&(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_acknack_req),TRUE,gas_id);
            break;

          case RM_DL_PKT_CTRL_ACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_CTRL_ACK_REQ");
            transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = TRUE;
            gmac_rrbp_send_pca(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.frame_number,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.time_slot, TRUE,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.rrbp, PCA_DL_FAI_MSG,
                               gas_id);
            break;

          case RM_DL_RELEASE_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd RM_DL_RELEASE_IND %d",global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause);
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause == REL_DL_NORMAL)
            {
              gmac_transfer_dl_release_clean(gas_id);
              gmac_transfer_dl_normal_release_with_ul(gas_id);
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,DONT_REL_RLC_DL,NO_DEL,FALSE,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_RLC_DL_RELEASE_IND,gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_RELEASE_IND,gas_id);
            }
            break;

          case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");
            gmac_tran_handle_unexpected_rlc_tbf_req(gas_id);
            break;

          case RM_DL_FIRST_BLOCK_IND:
               /*ignore this signal when rcvd in transfer*/
            break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from RLC */
            break;
        }
      } /* end of MAC_RLC signals */
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {
          case MAC_TIMER_T3180:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3180");
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3180,gas_id);
            mac_log_dl_tbf_release(MAC_LOG_DL_EXPIRED_T3180,gas_id);
            break;

          case MAC_TIMER_T3164_TRANSFER:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3164_TRANSFER");
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3164,gas_id);
            break;

          case MAC_TIMER_T3168:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3168");
            if (transfer_data_ptr->gmac_transfer_ul_request_count < MAX_RETRY_ATTEMPTS)
            {
              /* request quality reports again for PRR */
              global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
              gmac_transfer_qual_meas_req_to_L1(gas_id);
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3168,gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_EXPIRED_T3168,gas_id);
            }
            break;

          case MAC_TIMER_T3200_1:
          case MAC_TIMER_T3200_2:
          case MAC_TIMER_T3200_3:
          case MAC_TIMER_T3200_4:
          case MAC_TIMER_T3200_5:
          case MAC_TIMER_T3200_6:
          case MAC_TIMER_T3200_7:
          case MAC_TIMER_T3200_8:
               MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
               t3200_expiry_handler((mac_mac_sig_id_t)signal_id,gas_id );
          break;

#ifdef FEATURE_GSM_EDTM
          case MAC_TIMER_PKT_CS_REQ_RETRY:
            MSG_GERAN_MED_0_G("GMAC EDTM PKT CS REQ RETRY Timer expired");
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_AND_DL,
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
              EDTM_PKT_CS_REQUEST_RETRY,
              gas_id);
          break;

          case MAC_TIMER_T3196:
            MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
            gmac_transfer_edtm_t3196_expiry_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_MAC signals */
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {
          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_LOW_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");
            if(gmac_util_encode_ul_control_msg_handler(PMR,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if(gmac_util_encode_ul_control_msg_handler(PCCF,FALSE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND in T_CON_DYN");

            gmac_tran_check_dl_tbf_status(gas_id);

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            REL_RLC_DL,
                                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                            FALSE,
                                            gas_id);

            transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
            mac_log_dl_tbf_release(MAC_LOG_DL_GPRS_SUSPENSION,gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_GPRS_SUSPENSION, gas_id);
            break;

          case GRR_MAC_SEND_PCCN_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCN");
            if(gmac_util_encode_ul_control_msg_handler(PCCN,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCN, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

          case GRR_MAC_SEND_PSS_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PSS");
            if(gmac_util_encode_ul_control_msg_handler(PSS,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PSS, UL_PRESENT, GRR_CNF_NEEDED,gas_id);
            }
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_2_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND: ps_rel:%d, ps_rel_cause:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            gmac_dtm_chan_rel_handler( &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind,
                                       UL_DL_TBF_RELEASE_CONF,
                                       gas_id );
            break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            MSG_GERAN_MED_3_G("GMAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ: dtm_tbf_type:0x%x, cs_pres:%d, dtm_freq_pres:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.cs_present,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_freq_present );

            if( gmac_tran_dtm_chan_ass_handler(
                  &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                  &transfer_data_ptr->gmac_transfer_ul_rlc_info_store,
                  UL_DL_TBF_PRESENT,
                  gas_id ) == NO_DTM_ASS_ERROR )
            {
#ifdef FEATURE_GSM_EDTM
              if ( global_data_ptr->gmac_edtm_enabled )
              {
                gmac_tran_edtm_ass_cmd_handler(
                     &transfer_data_ptr->mac_transfer_substate,
                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                      gas_id );

                if ( transfer_data_ptr->mac_transfer_substate != TRANSFER_CONCURRENT_DYNAMIC_TRANSFER )
                {
                    transfer_data_ptr->mac_tran_substate_str =
                      gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate, gas_id);

                    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                }
              }
              else
#endif /*FEATURE_GSM_EDTM*/
              {
                transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release = FALSE;
                transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;

                MSG_GERAN_MED_0_G("GMAC State tran = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC");
                (void)mac_log_state(gas_id);
              }
            }
          break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_AND_DL,
              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause,
              EDTM_FIRST_PKT_CS_REQUEST,
              gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
          case GRR_MAC_PSHO_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_PSHO_REQ psho_type: 0x%x",global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_psho_req.psho_type);
            if(gmac_util_ps_ho_req_handler(transfer_data_ptr->mac_transfer_substate, gas_id) == TRUE)
            {
              transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release = FALSE;
              transfer_data_ptr->mac_transfer_substate = TRANSFER_PS_HANDOVER;

              transfer_data_ptr->mac_tran_substate_str =
                gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate, gas_id);
              MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
              if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_type == PSHO_GTOG)
              {
                gmac_start_t3218(gas_id);
                if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
                {
                  gmac_util_send_grlcdl_psho_start_ind(gas_id);
                }
                gmac_start_t3180(gas_id);
                grlc_dl_start_t3190(gas_id);

                gmac_util_send_grlcul_psho_start_ind(gas_id);

                gmac_stop_t3168(gas_id);

                memset( &global_data_ptr->gmac_local_stored_ul_chan_req, 0, sizeof(channel_req_desc_t) );

                global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_REQ;
                transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;

                transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;
                transfer_data_ptr->gmac_transfer_ul_request_count = 0;
              }
            }
            else
            {
              gmac_tran_psho_trigger_pccf_sending(transfer_data_ptr->mac_transfer_substate, gas_id);
              gmac_util_reset_psho_structure(gas_id);
            }
          break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from GRR */
            break;
        }
      } /* end of GRR_MAC signals */
      break; /* end of TRANSFER_CONCURRENT_DYNAMIC_TRANSFER */

    /*********************************************************
    * transfer state TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN *
    *********************************************************/
    case TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_DATA_IND:
            if(transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release == FALSE)
            {
              gmac_tdynamic_concurrent_dl_reassign_data_ind(gas_id);
            }
            break;

          case L1_MAC_USF_DETECTED:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_USF_DETECTED");
            gmac_tdynamic_usf_detected(gas_id);
            break;

          case L1_MAC_QUAL_MEASURMENTS:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_QUAL_MEASURMENTS");
            gmac_transfer_dl_pkt_acknack(&(global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements), gas_id);
            break;

          case MAC_PH_CONNECT_IND:
            /*while waiting for a pending UL to connect cell reselection or suspension for cs can be rcvd. */
            /*Under these conditions the rel process must complete IGNORE any attempts to connect the ul   */
            if(transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE)
            {
#ifdef FEATURE_GSM_DTM
              gmac_tran_rel_state_handle_connect_ind(
                global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind,
                gas_id );
#endif /*FEATURE_GSM_DTM*/

              MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");
              if (global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == DOWNLINK_TBF_ESTABLISHED)
              {
                GMAC_UPDATE_DL_TS_REASS;
                GMAC_UPDATE_DL_TFI;

                /*Reset this flag. this flag is set when gmac received PDA in T_CON_DYN state*/
                transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = FALSE;

                /*For HMSC RLC needs to know the Number of DL ts so irrespective of */
                /*EGPRS or GPRS mode always send MR_DL_ASS_IND to RLC               */
                gmac_util_send_dl_ass_to_rlc(gas_id);


                if (transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending)
                {
                  /* Pending reallocation request can now be processed. Prepare to send PRR on PACCH */
                  transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
                  transfer_data_ptr->gmac_transfer_ul_request_count = 0;
                  global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
                  gmac_transfer_qual_meas_req_to_L1(gas_id);
                }

                MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DYN");
                transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;
                (void)mac_log_state(gas_id);
              }
              else
              {
                if(geran_get_nv_recovery_restart_enabled(gas_id))
                {
                  MSG_GERAN_ERROR_1_G("GMAC AB Rel, Expected DL (0x01) in PH_CONNECT_IND, received: 0x%x",
                                      global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                  MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                  geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                }
                else
                {
                  ERR_GERAN_FATAL_2_G("GMAC AB Rel, Expected DL (0x01) in PH_CONNECT_IND, received: 0x%x",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                }
              }
            }
            else
            {
              MSG_GERAN_MED_0_G("MAC info ABNORMAL UL & DL rel in progress,Ignore PH_CONNECT_IND");
            }
            break;

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("GMAC INFO L1 Freq Cnf ignored for freq cleanup");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }
          break;

          case L1_MAC_TBF_REL_CONFIRM:

            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
            gmac_tran_rel_state_handle_rel_cnf( global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released,
                                                gas_id );
#endif /*FEATURE_GSM_DTM*/

            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_DL_TBF_RELEASE_CONF)
            {
              /* for abnormal release case, therefore release to NULL/IDLE */
              gmac_transfer_tbf_rel_confirm(UL_DL_TBF_RELEASE_CONF, gas_id);
              transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF)
                    && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE))
            {
              /* flag set when RM_DL_RELEASE_IND is released during concurrent*/
              /* tbf operation reset if L1 confirms dl before ul              */

              MSG_GERAN_MED_1_G("mac info wait_for_ul_rel_confirm = %d",global_data_ptr->c_flags.wait_for_ul_rel_confirm);
              if (global_data_ptr->c_flags.wait_for_ul_rel_confirm == TRUE)
              {
                global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
              }
              else
              {
               global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
              }
              transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;

              if ( transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending )
              {
                /* Pending reallocation request can now be processed. Prepare to send PRR on PACCH */

                transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
                transfer_data_ptr->gmac_transfer_ul_request_count = 0;
                global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
                gmac_transfer_qual_meas_req_to_L1(gas_id);
              }
              if(transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency == TRUE)
              {
                /*This means GMAC received dl assignment from the network after
                gmac send dl release req to layer1*/
                transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_DL_ASSIGN;
                (void)mac_log_state(gas_id);
                transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = FALSE;
                MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYN_DL_ASSIGN");
              }
              else
              {
                transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC;
                (void)mac_log_state(gas_id);
                MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYN");
              }
            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released != UL_DL_TBF_RELEASE_CONF)
                           && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == TRUE))
            {
                /* This is the case where an abnormal release that results in both UL and DL tbf*/
                /* being terminated has been initiated while a release is already in progress and*/
                /* MAC is waiting for L1 to confirm the release. Under these conditions MAC must*/
                /* wait for an UL and DL rel confirm form L1 before progressing                 */
                MSG_GERAN_MED_1_G("ABNORMAL UL & DL rel in progress, require rel confirm for both UL and DL to continue. rel type = %d",
                                  global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);

            }
            else /* UL_TBF_RELEASE_CONF */
            {
              MSG_GERAN_MED_0_G("GMAC State tran = T_DL_REASS");
              global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
              MSG_GERAN_MED_0_G("mac info wait_for_ul_rel_confirm = FALSE");
              global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;

              /*ensure that RLC is not blocked if returning to T_DL_REASS */
              if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
              {
                if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED)
                {
                  gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
                }
              }
              else
              {
                MSG_GERAN_HIGH_0_G("GMAC T3172 still running leave RLC blocked");
              }

              transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release = FALSE;
              global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
              transfer_data_ptr->mac_transfer_substate = TRANSFER_DL_REASSIGN;
              global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
              transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = FALSE;
              (void)mac_log_state(gas_id);

              gmac_eng_mode_update_tbf_info((uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER, global_data_ptr->current_tbf_mode,FALSE,gas_id);

              if(global_data_ptr->gmac_local_pccn_params.pccn_pending  != NO_PENDING_PCCN)
              {
                gmac_util_send_ul_control_msg_handler(PCCN, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
              }
            }
            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND");
            gmac_transfer_edtm_ph_cs_connect_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from L1 */
            break;
        }
      } /* end of MAC_L1 signals */
      else if(task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_UL_REALLOC_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_REALLOC_REQ");

            /* Store channel request parameters and set reallocation pending flag
               to be dealt with on completion of DL Assignment */

            global_data_ptr->gmac_transfer_ul_cv_zero_ind = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.b_cv_zero;
            transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = TRUE;

            if ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.pfi_present )
            {
              global_data_ptr->pfi_present = TRUE;
              global_data_ptr->pfi = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.pfi;
            }
            else
            {
              global_data_ptr->pfi_present = FALSE;
            }

            /* store RLC parameters and request quality report for PRR */
            global_data_ptr->gmac_local_stored_ul_chan_req = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.chan_req_desc;
            break;

          case RM_UL_RELEASE_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_RELEASE_IND");
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause != REL_UL_NORMAL)
            {
              MSG_GERAN_MED_0_G("MAC Rcvd UL abnormal release cause");
              if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause == REL_UL_ABNORMAL_CELL_RESEL)
              {
                gmac_abnormal_release_procedure(RR_CELL_RESEL_REQ,REL_RLC_DL,NO_DEL,FALSE,gas_id);
              }
              else
              {
                gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
              }
              mac_log_dl_tbf_release(MAC_LOG_DL_RLC_UL_RELEASE_IND,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_RELEASE_IND,gas_id);

            }
            else
            {
              MSG_GERAN_MED_0_G("MAC Rcvd UL normal release cause");
              gmac_transfer_ul_dynamic_release_clean(gas_id);
              gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__,gas_id);
              gmac_tran_send_mac_l1_release(UL_TBF_NORMAL,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE,gas_id);
            }
            break;

          case RM_DL_PKT_ACKNACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_ACKNACK_REQ");
            gmac_transfer_dl_pdan_handler(&(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_acknack_req),TRUE,gas_id);
            break;

          case RM_DL_PKT_CTRL_ACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_CTRL_ACK_REQ");
            transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = TRUE;
            gmac_rrbp_send_pca(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.frame_number,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.time_slot, TRUE,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.rrbp, PCA_DL_FAI_MSG,
                               gas_id);
            break;

          case RM_DL_RELEASE_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd RM_DL_RELEASE_IND %d",global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause);
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause == REL_DL_NORMAL)
            {
              gmac_transfer_dl_release_clean(gas_id);
              gmac_transfer_dl_normal_release_with_ul(gas_id);
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,DONT_REL_RLC_DL,NO_DEL,FALSE,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_RLC_DL_RELEASE_IND,gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_RELEASE_IND,gas_id);
            }
            break;

          case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");
            gmac_tran_handle_unexpected_rlc_tbf_req(gas_id);
            break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from RLC */
            break;
        }
      } /* end of MAC_RLC signals */
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {
          case MAC_TIMER_T3180:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3180");
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3180,gas_id);
            mac_log_dl_tbf_release(MAC_LOG_DL_EXPIRED_T3180,gas_id);
            break;

          case MAC_TIMER_T3200_1:
          case MAC_TIMER_T3200_2:
          case MAC_TIMER_T3200_3:
          case MAC_TIMER_T3200_4:
          case MAC_TIMER_T3200_5:
          case MAC_TIMER_T3200_6:
          case MAC_TIMER_T3200_7:
          case MAC_TIMER_T3200_8:
               MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
               t3200_expiry_handler((mac_mac_sig_id_t)signal_id, gas_id );
          break;

#ifdef FEATURE_GSM_EDTM
          case MAC_TIMER_PKT_CS_REQ_RETRY:
            MSG_GERAN_MED_0_G("GMAC EDTM PKT CS REQ RETRY Timer expired");
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_AND_DL,
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
              EDTM_PKT_CS_REQUEST_RETRY,
              gas_id);
          break;

          case MAC_TIMER_T3196:
            MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
            gmac_transfer_edtm_t3196_expiry_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_MAC signals */
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {
          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_LOW_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");
            if(gmac_util_encode_ul_control_msg_handler(PMR,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if(gmac_util_encode_ul_control_msg_handler(PCCF,FALSE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND in T_CON_DYN_DL_REASS");

            gmac_tran_check_dl_tbf_status(gas_id);

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            REL_RLC_DL,
                                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                            FALSE,
                                            gas_id);

            transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
            mac_log_dl_tbf_release(MAC_LOG_DL_GPRS_SUSPENSION, gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_GPRS_SUSPENSION, gas_id);
            break;

          case GRR_MAC_SEND_PCCN_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCN");
            if(gmac_util_encode_ul_control_msg_handler(PCCN,FALSE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCN, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_SEND_PSS_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PSS");
            if(gmac_util_encode_ul_control_msg_handler(PSS,FALSE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PSS, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_2_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND: ps_rel:%d, ps_rel_cause:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            gmac_dtm_chan_rel_handler( &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind,
                                       UL_DL_TBF_RELEASE_CONF,
                                       gas_id );
            break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            MSG_GERAN_MED_3_G("GMAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ: dtm_tbf_type:0x%x, cs_pres:%d, dtm_freq_pres:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.cs_present,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_freq_present );

            if( gmac_tran_dtm_chan_ass_handler(
                  &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                  &transfer_data_ptr->gmac_transfer_ul_rlc_info_store,
                  UL_DL_TBF_PRESENT,
                  gas_id ) == NO_DTM_ASS_ERROR )
            {
#ifdef FEATURE_GSM_EDTM
              if ( global_data_ptr->gmac_edtm_enabled )
              {
                gmac_tran_edtm_ass_cmd_handler(
                     &transfer_data_ptr->mac_transfer_substate,
                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                      gas_id );

                if ( transfer_data_ptr->mac_transfer_substate != TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN )
                {
                    transfer_data_ptr->mac_tran_substate_str =
                      gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate, gas_id);

                    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                }
              }
              else
#endif /*FEATURE_GSM_EDTM*/
              {
                transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;

                MSG_GERAN_MED_0_G("GMAC State tran = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC");
                (void)mac_log_state(gas_id);
              }
            }
          break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_AND_DL,
              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause,
              EDTM_FIRST_PKT_CS_REQUEST,
              gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from GRR */
            break;
        }
      } /* end of GRR_MAC signals */
      break; /* end of TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN */

    /*****************************************************
    * transfer state TRANSFER_CONCURRENT_DYNAMIC_REALLOC *
    *****************************************************/
    case TRANSFER_CONCURRENT_DYNAMIC_REALLOC:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_DATA_IND:
            if(transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release == FALSE)
            {
              gmac_tdynamic_concurrent_realloc_data_ind(gas_id);
            }
            break;

          case L1_MAC_USF_DETECTED:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_USF_DETECTED");
            gmac_tdynamic_usf_detected(gas_id);
            break;

          case L1_MAC_QUAL_MEASURMENTS:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_QUAL_MEASURMENTS");
            gmac_transfer_dl_pkt_acknack(&(global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements), gas_id);
            break;

          case MAC_PH_CONNECT_IND:
            /*while waiting for a pending UL to connect cell reselection or suspension for cs can be rcvd. */
            /*Under these conditions the rel process must complete IGNORE any attempts to connect the ul   */
            if(transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE)
            {
#ifdef FEATURE_GSM_DTM
              gmac_tran_rel_state_handle_connect_ind(
                global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind,
                gas_id );
#endif /*FEATURE_GSM_DTM*/

              MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");
              if (global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_TBF_ESTABLISHED)
              {
                GMAC_UPDATE_UL_TS_REALLOC;
                GMAC_UPDATE_UL_DLCTRL_TS_REALLOC;

                GMAC_UPDATE_UL_TFI;

                if (transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag)
                {
                  /* send MAC_L1_CS_RECONFIG due to CS change */
                  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_CS_RECONFIG");
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_id = (uint8)MAC_L1_CS_RECONFIG;
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_set = MS_MAC_L1;
                  gprs_l1_mac_acc_func(&transfer_data_ptr->mac_l1_sig_cs_reconfig, gas_id);

                  global_data_ptr->gmac_local_cs_setting = transfer_data_ptr->pua_ptr_cs_change;
                }

                transfer_data_ptr->mr_ul_alloc_cnf_ptr = grlc_get_cmd_buf_mac();
                if (transfer_data_ptr->mr_ul_alloc_cnf_ptr != NULL)
                {
                  MSG_GERAN_MED_0_G("MAC Sent MR_UL_ALLOC_CNF");
                  transfer_data_ptr->mr_ul_alloc_cnf_ptr->msg.ul_alloc_cnf = transfer_data_ptr->gmac_transfer_ul_rlc_info_store;
                  gmac_rlc_mac_insert_gas_id(transfer_data_ptr->mr_ul_alloc_cnf_ptr,gas_id);
                  grlc_put_cmd_mac(gas_id,transfer_data_ptr->mr_ul_alloc_cnf_ptr);
                }
                else
                {
                  ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
                }

                MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DYN");
                transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;

                gmac_start_t3164_transfer(gas_id);

                (void)mac_log_state(gas_id);

                gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, global_data_ptr->current_tbf_mode,FALSE,gas_id);

                if ( transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending )
                {
                  /* Pending reallocation request can now be processed. Prepare to send PRR on PACCH */

                  transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
                  transfer_data_ptr->gmac_transfer_ul_request_count = 0;
                  global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
                  gmac_transfer_qual_meas_req_to_L1(gas_id);
                }
              }
              else
              {
                if(geran_get_nv_recovery_restart_enabled(gas_id))
                {
                  MSG_GERAN_ERROR_1_G("GMAC AB Rel, Expected UL (0x00) in PH_CONNECT_IND, received: 0x%x",
                                      global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                  MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                  geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                }
                else
                {
                  ERR_GERAN_FATAL_2_G("GMAC AB Rel, Expected UL (0x00) in PH_CONNECT_IND, received: 0x%x",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                }
              }
            }
            else
            {
              MSG_GERAN_MED_0_G("MAC info ABNORMAL UL & DL rel in progress,Ignore PH_CONNECT_IND");
            }
            break;

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("GMAC INFO L1 Freq Cnf ignored for freq cleanup");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }

          break;

          case L1_MAC_TBF_REL_CONFIRM:

            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
            gmac_tran_rel_state_handle_rel_cnf( global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released,
                                                gas_id );
#endif /*FEATURE_GSM_DTM*/

            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_DL_TBF_RELEASE_CONF)
            {
              /* for abnormal release case, therefore release to NULL/IDLE */
              gmac_transfer_tbf_rel_confirm(UL_DL_TBF_RELEASE_CONF, gas_id);
              transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF)
                    && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE))
            {
              MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYN_REALL");

              /* flag set when RM_DL_RELEASE_IND is released during concurrent*/
              /* tbf operation reset if L1 confirms dl before ul              */

              MSG_GERAN_MED_1_G("mac info wait_for_ul_rel_confirm = %d",global_data_ptr->c_flags.wait_for_ul_rel_confirm);
              if (global_data_ptr->c_flags.wait_for_ul_rel_confirm == TRUE)
              {
                global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
              }
              else
              {
                global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
              }

              transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC;
              (void)mac_log_state(gas_id);
            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released != UL_DL_TBF_RELEASE_CONF)
                           && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == TRUE))
            {
                /* This is the case where an abnormal release that results in both UL and DL tbf*/
                /* being terminated has been initiated while a release is already in progress and*/
                /* MAC is waiting for L1 to confirm the release. Under these conditions MAC must*/
                /* wait for an UL and DL rel confirm form L1 before progressing                 */
                MSG_GERAN_MED_1_G("ABNORMAL UL & DL rel in progress, require rel confirm for both UL and DL to continue. rel type = %d",
                                  global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);

            }
            else /* UL_TBF_RELEASE_CONF */
            {
              MSG_GERAN_MED_0_G("GMAC State tran = T_DL");
              global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
              MSG_GERAN_MED_0_G("mac info wait_for_ul_rel_confirm = FALSE");
              global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;

              if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED)
              {
                gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
              }

              global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
              transfer_data_ptr->mac_transfer_substate = TRANSFER_DL;
              transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
              global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
              (void)mac_log_state(gas_id);

              gmac_eng_mode_update_tbf_info((uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER, global_data_ptr->current_tbf_mode,FALSE,gas_id);

              if(global_data_ptr->gmac_local_pccn_params.pccn_pending  != NO_PENDING_PCCN)
              {
                gmac_util_send_ul_control_msg_handler(PCCN, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
              }

            }
            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND");
            gmac_transfer_edtm_ph_cs_connect_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from L1 */
            break;
        }
      } /* end of MAC_L1 signals */
      else if(task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_UL_REALLOC_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_REALLOC_REQ");

            /* Store channel request parameters and set reallocation pending flag
               to be dealt with on receipt of PH_CONNECT_IND */

            global_data_ptr->gmac_transfer_ul_cv_zero_ind = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.b_cv_zero;
            global_data_ptr->gmac_local_stored_ul_chan_req = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.chan_req_desc;
            transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = TRUE;
            break;

          case RM_UL_RELEASE_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_RELEASE_IND");
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause != REL_UL_NORMAL)
            {
              MSG_GERAN_MED_0_G("MAC Rcvd UL abnormal release cause");
              if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause == REL_UL_ABNORMAL_CELL_RESEL)
              {
                gmac_abnormal_release_procedure(RR_CELL_RESEL_REQ,REL_RLC_DL,NO_DEL,FALSE, gas_id);
              }
              else
              {
                gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE, gas_id);
              }
              mac_log_ul_tbf_release(MAC_LOG_UL_RELEASE_IND, gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_RLC_UL_RELEASE_IND, gas_id);
            }
            else
            {
              MSG_GERAN_MED_0_G("MAC Rcvd UL normal release cause");
              gmac_transfer_ul_dynamic_release_clean(gas_id);
              gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__, gas_id);
              gmac_tran_send_mac_l1_release(UL_TBF_NORMAL, gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE, gas_id);
            }
            break;

          case RM_DL_PKT_ACKNACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_ACKNACK_REQ");
            gmac_transfer_dl_pdan_handler(&(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_acknack_req),TRUE,gas_id);
            break;

          case RM_DL_PKT_CTRL_ACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_CTRL_ACK_REQ");
            transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = TRUE;
            gmac_rrbp_send_pca(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.frame_number,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.time_slot, TRUE,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.rrbp, PCA_DL_FAI_MSG,
                               gas_id);
            break;

          case RM_DL_RELEASE_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd RM_DL_RELEASE_IND %d",global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause);
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause == REL_DL_NORMAL)
            {
              gmac_transfer_dl_release_clean(gas_id);
              gmac_transfer_dl_normal_release_with_ul(gas_id);
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,DONT_REL_RLC_DL,NO_DEL,FALSE,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_RLC_DL_RELEASE_IND,gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_RELEASE_IND,gas_id);
            }
            break;

          case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");
            gmac_tran_handle_unexpected_rlc_tbf_req(gas_id);
            break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected signal RLC in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from RLC */
            break;
        }
      } /* end of MAC_RLC signals */
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {
          case MAC_TIMER_T3180:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3180");
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3180,gas_id);
            mac_log_dl_tbf_release(MAC_LOG_DL_EXPIRED_T3180,gas_id);
            break;

          case MAC_TIMER_T3200_1:
          case MAC_TIMER_T3200_2:
          case MAC_TIMER_T3200_3:
          case MAC_TIMER_T3200_4:
          case MAC_TIMER_T3200_5:
          case MAC_TIMER_T3200_6:
          case MAC_TIMER_T3200_7:
          case MAC_TIMER_T3200_8:
               MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
               t3200_expiry_handler((mac_mac_sig_id_t)signal_id,gas_id );
          break;

#ifdef FEATURE_GSM_EDTM
          case MAC_TIMER_PKT_CS_REQ_RETRY:
            MSG_GERAN_MED_0_G("GMAC EDTM PKT CS REQ RETRY Timer expired");
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_AND_DL,
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
              EDTM_PKT_CS_REQUEST_RETRY,
              gas_id);
          break;

          case MAC_TIMER_T3196:
            MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
            gmac_transfer_edtm_t3196_expiry_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_MAC signals */
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {
          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_LOW_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");
            if(gmac_util_encode_ul_control_msg_handler(PMR,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if(gmac_util_encode_ul_control_msg_handler(PCCF,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND in T_CON_DYN_REALLOC");

            gmac_tran_check_dl_tbf_status(gas_id);

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            REL_RLC_DL,
                                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                            FALSE,
                                            gas_id);

            transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
            mac_log_dl_tbf_release(MAC_LOG_DL_GPRS_SUSPENSION, gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_GPRS_SUSPENSION, gas_id);
            break;

          case GRR_MAC_SEND_PCCN_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCN");
            if(gmac_util_encode_ul_control_msg_handler(PCCN,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCN, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_SEND_PSS_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PSS");
            if(gmac_util_encode_ul_control_msg_handler(PSS,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PSS, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_2_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND: ps_rel:%d, ps_rel_cause:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            gmac_dtm_chan_rel_handler( &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind,
                                       UL_DL_TBF_RELEASE_CONF,
                                       gas_id );
            break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
             MSG_GERAN_MED_3_G("GMAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ: dtm_tbf_type:0x%x, cs_pres:%d, dtm_freq_pres:%d",
                               global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                               global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.cs_present,
                               global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_freq_present );

            if( gmac_tran_dtm_chan_ass_handler(
                  &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                  &transfer_data_ptr->gmac_transfer_ul_rlc_info_store,
                  UL_DL_TBF_PRESENT,
                  gas_id ) == NO_DTM_ASS_ERROR )
            {
#ifdef FEATURE_GSM_EDTM
              if ( global_data_ptr->gmac_edtm_enabled )
              {
                gmac_tran_edtm_ass_cmd_handler(
                     &transfer_data_ptr->mac_transfer_substate,
                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                      gas_id );

                if ( transfer_data_ptr->mac_transfer_substate != TRANSFER_CONCURRENT_DYNAMIC_REALLOC )
                {
                    transfer_data_ptr->mac_tran_substate_str =
                      gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate,
                                                               gas_id);

                    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                }
              }
              else
#endif /*FEATURE_GSM_EDTM*/
              {
                transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;

                MSG_GERAN_MED_0_G("GMAC State tran = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC");
                (void)mac_log_state(gas_id);
              }
            }
          break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_AND_DL,
              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause,
              EDTM_FIRST_PKT_CS_REQUEST,
              gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from GRR */
            break;
        }
      } /* end of GRR_MAC signals */
      break; /* end of TRANSFER_CONCURRENT_DYNAMIC_REALLOC */

    /*****************************************************************
    * transfer state TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC *
    *****************************************************************/
    case TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_DATA_IND:
            if(transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release == FALSE)
            {
              gmac_tdynamic_concurrent_realloc_dl_reassign_data_ind(gas_id);
            }
            break;

          case L1_MAC_USF_DETECTED:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_USF_DETECTED");
            gmac_tdynamic_usf_detected(gas_id);
            break;

          case L1_MAC_QUAL_MEASURMENTS:
            MSG_GERAN_LOW_0_G("MAC Rcvd L1_MAC_QUAL_MEASURMENTS");
            gmac_transfer_dl_pkt_acknack(&(global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements), gas_id);
            break;

          case MAC_PH_CONNECT_IND:
            /*while waiting for a pending UL to connect cell reselection or suspension for cs can be rcvd. */
            /*Under these conditions the rel process must complete IGNORE any attempts to connect the ul   */
            if (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE)
            {
  #ifdef FEATURE_GSM_DTM
              gmac_tran_rel_state_handle_connect_ind(
                global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind,
                gas_id );
  #endif /*FEATURE_GSM_DTM*/
  
              MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");
              if (global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_DOWNLINK_TBF_ESTABLISHED)
              {
                GMAC_UPDATE_UL_TS_REALLOC;
                GMAC_UPDATE_UL_DLCTRL_TS_REALLOC;
                GMAC_UPDATE_DL_TS_REASS;
  
                GMAC_UPDATE_UL_TFI;
                GMAC_UPDATE_DL_TFI;
                /*Reset this flag. this flag is set when gmac received PTR in T_CON_DYN state*/
                transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = FALSE;
  
                /* due to fix for cr697201. These flags will be set when a PDA is rcvd           */
                /* in TRANSFER_CONCURRENT_DYNAMIC_REALLOC while waiting for the                  */
                /* connect ind for the PUA which put MAC into T_CON_DYN_REALLOC state initially. */
                /* Reset flags as reception of this PTR  will supersede the previous assignments */
                /* and L1 will send 1 connec ind for both UL and DL                              */
                transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
                transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;
  
  
  #ifdef FEATURE_GSM_DTM
                transfer_data_ptr->t_flags.dtm_realloc_in_progress = FALSE;
  #endif /*FEATURE_GSM_DTM*/
  
                if (transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag)
                {
                  /* send MAC_L1_CS_RECONFIG due to CS change */
                  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_CS_RECONFIG");
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_id = (uint8)MAC_L1_CS_RECONFIG;
                  transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_set = MS_MAC_L1;
                  gprs_l1_mac_acc_func(&transfer_data_ptr->mac_l1_sig_cs_reconfig, gas_id);
  
                  global_data_ptr->gmac_local_cs_setting = transfer_data_ptr->pua_ptr_cs_change;
                }
  
                /*For HMSC RLC needs to know the Number of DL ts so irrespective of */
                /*EGPRS or GPRS mode always send MR_DL_ASS_IND to RLC               */
                gmac_util_send_dl_ass_to_rlc(gas_id);
  
  
                transfer_data_ptr->mr_ul_alloc_cnf_ptr = grlc_get_cmd_buf_mac();
                if (transfer_data_ptr->mr_ul_alloc_cnf_ptr != NULL)
                {
                  MSG_GERAN_MED_0_G("MAC Sent MR_UL_ALLOC_CNF");
                  transfer_data_ptr->mr_ul_alloc_cnf_ptr->msg.ul_alloc_cnf = transfer_data_ptr->gmac_transfer_ul_rlc_info_store;
                  gmac_rlc_mac_insert_gas_id(transfer_data_ptr->mr_ul_alloc_cnf_ptr,gas_id);
                  grlc_put_cmd_mac(gas_id,transfer_data_ptr->mr_ul_alloc_cnf_ptr);
                }
                else
                {
                  ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
                }
  
                MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DYN");
                transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;
  
                gmac_start_t3164_transfer(gas_id);
  
                (void)mac_log_state(gas_id);
  
                gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode,
                                              global_data_ptr->current_tbf_mode, FALSE, gas_id);
  
                if ( transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending )
                {
                  /* Pending reallocation request can now be processed. Prepare to send PRR on PACCH */
  
                  transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
                  transfer_data_ptr->gmac_transfer_ul_request_count = 0;
                  global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
                  gmac_transfer_qual_meas_req_to_L1(gas_id);
                }
              }
              else
              {
                /* cr 697201 PDA rxd while waiting for connect ind for UL in             */
                /*TRANSFER_CONCURRENT_DYNAMIC_REALLOC state. Reception of PDA is         */
                /*going to put MAC into  TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC */
  
                if((global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == DOWNLINK_TBF_ESTABLISHED)
                   && (transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl == TRUE))
                {
                  MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND for DL");
  
                  GMAC_UPDATE_DL_TFI;
  
                  /*For HMSC RLC needs to know the Number of DL ts so irrespective of */
                  /*EGPRS or GPRS mode always send MR_DL_ASS_IND to RLC               */
                  gmac_util_send_dl_ass_to_rlc(gas_id);
  
                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
                  
                  if(transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul == FALSE)
                  {
                    /*case where L1 has already sent Connect ind for UL but MAC was still*/
                    /*wating for a connect in for DL*/
  
                    MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DYN");
                    transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;
                    (void)mac_log_state(gas_id);
  
                    gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, 
                                                  global_data_ptr->current_tbf_mode, FALSE,gas_id);
                  }
                  else
                  {
                    /*wait in state for the UL to connect*/
                    MSG_GERAN_MED_0_G("GMAC waiting for UL connect ind");
                  }
                }            
                else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_TBF_ESTABLISHED)
                    && (transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul == TRUE))
  
                {
                  MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND for UL");
  
                  GMAC_UPDATE_UL_TS_REALLOC;
                  GMAC_UPDATE_UL_DLCTRL_TS_REALLOC;
  
                  GMAC_UPDATE_UL_TFI;
  
                  if (transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag)
                  {
                    /* send MAC_L1_CS_RECONFIG due to CS change */
                    MSG_GERAN_MED_0_G("MAC Sent MAC_L1_CS_RECONFIG");
                    transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_id = (uint8)MAC_L1_CS_RECONFIG;
                    transfer_data_ptr->mac_l1_sig_cs_reconfig.mac_l1_cs_reconfig.message_header.message_set = MS_MAC_L1;
                    gprs_l1_mac_acc_func(&transfer_data_ptr->mac_l1_sig_cs_reconfig,gas_id);
  
                    global_data_ptr->gmac_local_cs_setting = transfer_data_ptr->pua_ptr_cs_change;
                  }
  
                  transfer_data_ptr->mr_ul_alloc_cnf_ptr = grlc_get_cmd_buf_mac();
                  if (transfer_data_ptr->mr_ul_alloc_cnf_ptr != NULL)
                  {
                    MSG_GERAN_MED_0_G("MAC Sent MR_UL_ALLOC_CNF");
                    transfer_data_ptr->mr_ul_alloc_cnf_ptr->msg.ul_alloc_cnf = transfer_data_ptr->gmac_transfer_ul_rlc_info_store;
                    grlc_put_cmd_mac(gas_id,transfer_data_ptr->mr_ul_alloc_cnf_ptr);
                  }
                  else
                  {
                    ERR_FATAL("MAC error MALLOC failed",0,0,0);
                  }
  
                  gmac_start_t3164_transfer(gas_id);
  
                  (void)mac_log_state(gas_id);
  
                  gmac_eng_mode_update_tbf_info(global_data_ptr->gmac_current_mac_mode, 
                                                global_data_ptr->current_tbf_mode,FALSE,gas_id);
  
                  if ( transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending )
                  {
                    /* Pending reallocation request can now be processed. Prepare to send PRR on PACCH */
  
                    transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
                    transfer_data_ptr->gmac_transfer_ul_request_count = 0;
                    global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;
                    gmac_transfer_qual_meas_req_to_L1(gas_id);
                  }
  
                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;
  
                  if(transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl == FALSE)
                {
                    /*case where L1 has already sent Connect ind for DL but MAC was still*/
                    /*wating for a connect in for UL*/
  
                    MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DYN");
                    transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;
                  }
                  else
                  {
                    /*wait in state for the DL to connect*/
                    MSG_GERAN_MED_0_G("GMAC waiting for DL connect ind");                   
                  }
                }            
                else if((global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == DOWNLINK_TBF_ESTABLISHED)
                   && (transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl == FALSE))
  
                {
                  /*697201 alternative rel the tbf instead of panic reset.*/
                  MSG_GERAN_ERROR_2_G("GMAC AB Rel, unexpected DL only in PH_CONNECT_IND, received: 0x%x waiting_for_ph_con_for_dl %d",
                                      global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind,
                                      transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl);
  
                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;
  
                  gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
                }
                else if((global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_TBF_ESTABLISHED)
                   && (transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul == FALSE))
  
                {
                  /*697201 alternative rel the tbf instead of panic reset.*/
                  MSG_GERAN_ERROR_2_G("GMAC AB Rel, unexpected UL only in PH_CONNECT_IND, received: 0x%x t_flags.waiting_for_ph_con_for_ul %d",
                                       global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind,
                                       transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul);
  
                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;
  
                  gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
                }
              }
            }
            else
            {
              MSG_GERAN_MED_0_G("MAC info ABNORMAL UL & DL rel in progress,Ignore PH_CONNECT_IND");
            }
            break;

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("GMAC INFO L1 Freq Cnf ignored for freq cleanup");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }
          break;

          case L1_MAC_TBF_REL_CONFIRM:

            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
            gmac_tran_rel_state_handle_rel_cnf( global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released,
                                                gas_id );
#endif /*FEATURE_GSM_DTM*/

            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_DL_TBF_RELEASE_CONF)
            {
              /* for abnormal release case, therefore release to NULL/IDLE */
              gmac_transfer_tbf_rel_confirm(UL_DL_TBF_RELEASE_CONF, gas_id);
              transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF)
                     && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE))
            {
              /* flag set when RM_DL_RELEASE_IND is released during concurrent*/
              /* tbf operation reset if L1 confirms dl before ul              */

              MSG_GERAN_MED_1_G("mac info wait_for_ul_rel_confirm = %d",global_data_ptr->c_flags.wait_for_ul_rel_confirm);
              if (global_data_ptr->c_flags.wait_for_ul_rel_confirm == TRUE)
              {
                global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
              }
              else
              {
                global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
              }

              if(transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency == TRUE)
              {
                /*This means GMAC received PTR from the network after
                gmac send dl release req to layer1*/
                transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN;
                (void)mac_log_state(gas_id);
                transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = FALSE;
                MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYN_REALL_DL_ASSIGN");
              }
              else
              {
              transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC;
              (void)mac_log_state(gas_id);
                MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYN_REALL");
              }
            }
            else if ((global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released != UL_DL_TBF_RELEASE_CONF)
                           && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == TRUE))
            {
                /* This is the case where an abnormal release that results in both UL and DL tbf*/
                /* being terminated has been initiated while a release is already in progress and*/
                /* MAC is waiting for L1 to confirm the release. Under these conditions MAC must*/
                /* wait for an UL and DL rel confirm form L1 before progressing                 */
                MSG_GERAN_MED_1_G("ABNORMAL UL & DL rel in progress, require rel confirm for both UL and DL to continue. rel type = %d",
                                  global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);

            }
            else /* UL_TBF_RELEASE_CONF */
            {
              MSG_GERAN_MED_0_G("GMAC State tran = T_DL_REASS");
              global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
              MSG_GERAN_MED_0_G("mac info wait_for_ul_rel_confirm = FALSE");
              global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;

              if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__,gas_id) == BLOCKED)
              {
                gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__,gas_id);
              }

              global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
              transfer_data_ptr->mac_transfer_substate = TRANSFER_DL_REASSIGN;
              transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
              global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
              transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = FALSE;
              (void)mac_log_state(gas_id);

              gmac_eng_mode_update_tbf_info((uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER, global_data_ptr->current_tbf_mode,FALSE,gas_id);

              if(global_data_ptr->gmac_local_pccn_params.pccn_pending  != NO_PENDING_PCCN)
              {
                gmac_util_send_ul_control_msg_handler(PCCN, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
              }
            }
            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND");
            gmac_transfer_edtm_ph_cs_connect_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from L1 */
            break;
        }
      } /* end of MAC_L1 signals */
      else if(task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_UL_REALLOC_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_REALLOC_REQ");

            /* Store channel request parameters and set reallocation pending flag
               to be dealt with on completion of DL Assignment */

            global_data_ptr->gmac_transfer_ul_cv_zero_ind = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.b_cv_zero;
            global_data_ptr->gmac_local_stored_ul_chan_req = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.chan_req_desc;
            transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = TRUE;
            break;

          case RM_UL_RELEASE_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_RELEASE_IND");
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause != REL_UL_NORMAL)
            {
              MSG_GERAN_MED_0_G("MAC Rcvd UL abnormal release cause");
              if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause == REL_UL_ABNORMAL_CELL_RESEL)
              {
                gmac_abnormal_release_procedure(RR_CELL_RESEL_REQ,REL_RLC_DL,NO_DEL,FALSE,gas_id);
              }
              else
              {
                gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
              }
              mac_log_ul_tbf_release(MAC_LOG_UL_RELEASE_IND,gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_RLC_UL_RELEASE_IND,gas_id);
            }
            else
            {
              MSG_GERAN_MED_0_G("MAC Rcvd UL normal release cause");
              gmac_transfer_ul_dynamic_release_clean(gas_id);
              gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__,gas_id);
              /* This is missing from the original code */
              gmac_tran_send_mac_l1_release(UL_TBF_NORMAL,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE,gas_id);
            }
            break;

          case RM_DL_PKT_ACKNACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_ACKNACK_REQ");
            gmac_transfer_dl_pdan_handler(&(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_acknack_req),TRUE,gas_id);
            break;

          case RM_DL_PKT_CTRL_ACK_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_DL_PKT_CTRL_ACK_REQ");
            transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = TRUE;
            gmac_rrbp_send_pca(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.frame_number,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.time_slot, TRUE,
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_ctrl_ack_req.rrbp, PCA_DL_FAI_MSG,
                               gas_id);
            break;

          case RM_DL_RELEASE_IND:
            MSG_GERAN_MED_1_G("MAC Rcvd RM_DL_RELEASE_IND %d",global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause);
            if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause == REL_DL_NORMAL)
            {
              gmac_transfer_dl_release_clean(gas_id);
              gmac_transfer_dl_normal_release_with_ul(gas_id);
            }
            else
            {
              gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,DONT_REL_RLC_DL,NO_DEL,FALSE,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_RLC_DL_RELEASE_IND,gas_id);
              mac_log_dl_tbf_release(MAC_LOG_DL_RELEASE_IND,gas_id);
            }
            break;

          case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");
            gmac_tran_handle_unexpected_rlc_tbf_req(gas_id);
            break;

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from RLC */
            break;
        }
      } /* end of MAC_RLC signals */
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {
          case MAC_TIMER_T3180:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3180");
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3180,gas_id);
            mac_log_dl_tbf_release(MAC_LOG_DL_EXPIRED_T3180,gas_id);
            break;

          case MAC_TIMER_T3200_1:
          case MAC_TIMER_T3200_2:
          case MAC_TIMER_T3200_3:
          case MAC_TIMER_T3200_4:
          case MAC_TIMER_T3200_5:
          case MAC_TIMER_T3200_6:
          case MAC_TIMER_T3200_7:
          case MAC_TIMER_T3200_8:
               MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
               t3200_expiry_handler((mac_mac_sig_id_t)signal_id,gas_id );
          break;

#ifdef FEATURE_GSM_EDTM
          case MAC_TIMER_PKT_CS_REQ_RETRY:
            MSG_GERAN_MED_0_G("GMAC EDTM PKT CS REQ RETRY Timer expired");
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_AND_DL,
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause,
              EDTM_PKT_CS_REQUEST_RETRY,
              gas_id);
          break;

          case MAC_TIMER_T3196:
            MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
            gmac_transfer_edtm_t3196_expiry_handler(gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in state %d task id %d sig id 0x%x",
                                 transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown internal signal from MAC */
            break;
        }
      } /* end of MAC_MAC signals */
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {
          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_LOW_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");
            if(gmac_util_encode_ul_control_msg_handler(PMR,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if(gmac_util_encode_ul_control_msg_handler(PCCF,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND in T_CON_DYN_REALL_DL_REASS");

            gmac_tran_check_dl_tbf_status(gas_id);

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                            REL_RLC_DL,
                                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                            FALSE,
                                            gas_id);

            transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
            mac_log_dl_tbf_release(MAC_LOG_DL_GPRS_SUSPENSION, gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_GPRS_SUSPENSION, gas_id);
            break;

          case GRR_MAC_SEND_PCCN_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCN");
            if(gmac_util_encode_ul_control_msg_handler(PCCN,FALSE, gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PCCN, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }

            break;

          case GRR_MAC_SEND_PSS_REQ :
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PSS");
            if(gmac_util_encode_ul_control_msg_handler(PSS,FALSE,gas_id) == PASS)
            {
              gmac_util_send_ul_control_msg_handler(PSS, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
            }
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_2_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND: ps_rel:%d, ps_rel_cause:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            gmac_dtm_chan_rel_handler( &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind,
                                       UL_DL_TBF_RELEASE_CONF,
                                       gas_id );
            break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            MSG_GERAN_MED_3_G("GMAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ: dtm_tbf_type:0x%x, cs_pres:%d, dtm_freq_pres:%d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.cs_present,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_freq_present );

            if ( gmac_tran_dtm_chan_ass_handler(
                   &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                   &transfer_data_ptr->gmac_transfer_ul_rlc_info_store,
                   UL_DL_TBF_PRESENT,
                   gas_id ) == NO_DTM_ASS_ERROR )
            {
#ifdef FEATURE_GSM_EDTM
              if ( global_data_ptr->gmac_edtm_enabled )
              {
                gmac_tran_edtm_ass_cmd_handler(
                     &transfer_data_ptr->mac_transfer_substate,
                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type,
                      gas_id );

                if ( transfer_data_ptr->mac_transfer_substate != TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC )
                {
                    transfer_data_ptr->mac_tran_substate_str = gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate,gas_id);

                    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                }
              }
#endif /*FEATURE_GSM_EDTM*/


            }
          break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);
            gmac_transfer_edtm_pkt_cs_req_handler(
              UL_AND_DL,
              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause,
              EDTM_FIRST_PKT_CS_REQUEST,
              gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

         default:
           MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in state %d task id %d sig id 0x%x",
                                transfer_data_ptr->mac_transfer_substate,task_id, signal_id);
            /* unknown signal from GRR */
            break;
        }
      } /* end of GRR_MAC signals */
      break; /* end of TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC */

#ifdef FEATURE_GPRS_PS_HANDOVER
    /*****************************************************************
    * transfer state TRANSFER_PS_HANDOVER                            *
    *****************************************************************/
    case TRANSFER_PS_HANDOVER:
      switch (global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate)
      {
        case PSHO_NOT_ACTIVE:
          MSG_GERAN_ERROR_3_G( "Unexpected signal in psho substate %d task id %d sig id 0x%x",
                               global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate,task_id, signal_id);
          break;

        case PSHO_WAITING_FOR_L1_CNF:
          if(task_id == MS_MAC_L1)
          {
            switch (signal_id)
            {
              case L1_MAC_PSHO_CNF:
                MSG_GERAN_HIGH_1_G("GMAC Rcvd L1_MAC_PSHO CNF with psho status %d",
                                   global_data_ptr->msg_ptr->l1_mac_sig.msg.psho_cnf.psho_status);
                if(global_data_ptr->msg_ptr->l1_mac_sig.msg.psho_cnf.psho_status == PSHO_SUCCESS)
                {


                  if(global_data_ptr->gmac_local_pmr_params.pmr_pending == TRUE)
                  {
                    MSG_GERAN_MED_0_G("GMAC pmr_pending is true");
                    gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
                  }

                  /*GMAC Resetting PSS PENDING, PCCN PENDING, PCCF PENDING flags*/
                  global_data_ptr->gmac_local_pmr_params.pmr_pending   = FALSE;

                  global_data_ptr->gmac_local_pccn_params.pccn_pending = NO_PENDING_PCCN;

                  global_data_ptr->gmac_local_pss_params.pss_pending   = FALSE;
                  global_data_ptr->gmac_local_pccf_params.pccf_pending = FALSE;
                  global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate = PSHO_CONNECTING_TARGET_CHANNELS;
                  MSG_GERAN_MED_0_G("GMAC PSHO State = PSHO_CONNECTING_TARGET_CHANNELS ");
                }
                else
                {
                  if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
                  {
                    /*GMAC sends psho complete ind to GRLC DL task with PSHO FAILURE*/
                    gmac_tran_send_grlcdl_psho_complete_ind(PSHO_FAILURE,gas_id);
                  }

                  /*GMAC sends psho complete ind to GRLC UL task with PSHO FAILURE*/
                  gmac_tran_send_grlcul_psho_complete_ind(PSHO_FAILURE,gas_id);

                  if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_type == PSHO_GTOG)
                  {
                    /*GMAC setting mac_transfer_substate value from gmac_ps_ho structure*/
                    transfer_data_ptr->mac_transfer_substate = global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate;
                    transfer_data_ptr->mac_tran_substate_str = gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate,gas_id);
                    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                    /*Set PCCF cause to PSHO FAILURE OTHERS*/
                    global_data_ptr->gmac_ps_ho.psho_pccf_params.cause = PSHO_FAILURE_OTHERS;

                    /*GMAC process saved message any received in PSHO state*/
                    gmac_tran_psho_process_saved_msgs(transfer_data_ptr->mac_transfer_substate,gas_id);

                    if(transfer_data_ptr->mac_transfer_substate != TRANSFER_INACTIVE)
                    {
                      /*GMAC trigger pccf sending process*/
                      gmac_tran_psho_trigger_pccf_sending(transfer_data_ptr->mac_transfer_substate,gas_id);
                    }
                    gmac_tran_psho_restart_timers(transfer_data_ptr->mac_transfer_substate,gas_id);
                  }

                  /*GMAC sending PSHO CNF to GRR with PSHO FAILURE*/
                  gmac_util_send_grr_psho_cnf(PSHO_FAILURE,gas_id);

                  gmac_util_reset_psho_structure(gas_id);
                  global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate = PSHO_NOT_ACTIVE;
                  MSG_GERAN_MED_0_G("GMAC PSHO State = PSHO_NOT_ACTIVE ");
                }
                break;

              case L1_MAC_FREQ_ASS_CNF:
                break;

              case L1_MAC_TBF_REL_CONFIRM:
                MSG_GERAN_MED_0_G("GMAC rcvd L1_MAC_TBF_REL_CONFIRM. Gmac stores this signal in psho strucutre");
                gmac_tran_store_tbf_rel_cnf(gas_id);
                break;

              default:
                MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in psho substate %d task id %d sig id 0x%x",
                                     global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate,task_id, signal_id);
                /* unknown signal from L1 */
                break;
            }
          }
          else if(task_id == MS_MAC_RLC)
          {
            switch (signal_id)
            {
              case RM_UL_RELEASE_IND:
              case RM_DL_RELEASE_IND:
                gmac_tran_store_rlc_release_ind(signal_id,gas_id);
                break;

              default:
                MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in psho substate %d task id %d sig id 0x%x",
                                     global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate,task_id, signal_id);
                /* unknown signal from RLC */
                break;
            }
          } /* end of MAC_RLC signals */
          else if (task_id == MS_MAC_MAC)
          {
            switch (signal_id)
            {
              case MAC_TIMER_T3172:
                MSG_GERAN_HIGH_0_G("GMAC rcvd T3172 expiry");
                break;

              case MAC_TIMER_T3218:
                MSG_GERAN_MED_0_G("GMAC rcvd T3218 expiry");

                if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_type == PSHO_GTOG)
                {
                  /*GMAC psho type is GTOG*/

                  /*GMAC sending psho status to GL1 with PSHO FAILURE*/
                  gmac_tran_send_gl1_psho_status(PSHO_FAILURE,gas_id);

                  if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
                  {
                    /*GMAC sends psho complete ind with PSHO FAILURE to GRLC DL task*/
                    gmac_tran_send_grlcdl_psho_complete_ind(PSHO_FAILURE,gas_id);
                  }

                  /*GMAC sends psho complete ind with PSHO FAILURE to GRLC UL task*/
                  gmac_tran_send_grlcul_psho_complete_ind(PSHO_FAILURE,gas_id);

                  /*GMAC sends psho cnf with PSHO FAILURE to GRR*/
                  gmac_util_send_grr_psho_cnf(PSHO_FAILURE,gas_id);

                  /*GMAC setting mac_transfer_substate value from gmac_ps_ho structure*/
                  transfer_data_ptr->mac_transfer_substate = global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate;

                  transfer_data_ptr->mac_tran_substate_str =
                    gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate,gas_id);
                  MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);

                  global_data_ptr->gmac_ps_ho.psho_pccf_params.cause = PSHO_FAILURE_OTHERS;

                  /*GMAC process saved message any received in PSHO state*/
                  gmac_tran_psho_process_saved_msgs(transfer_data_ptr->mac_transfer_substate,gas_id);

                  if(transfer_data_ptr->mac_transfer_substate != TRANSFER_INACTIVE)
                  {
                    /*GMAC trigger pccf sending process*/
                    gmac_tran_psho_trigger_pccf_sending(transfer_data_ptr->mac_transfer_substate,gas_id);
                  }

                  gmac_util_reset_psho_structure(gas_id);

                  if(transfer_data_ptr->mac_transfer_substate == TRANSFER_DL)
                  {
                    if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
                    {
                      if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__,gas_id) == BLOCKED)
                      {
                        gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__,gas_id);
                      }
                    }
                    else
                    {
                      MSG_GERAN_HIGH_0_G("GMAC T3172 still running leave RLC blocked");
                    }
                  }
                  gmac_tran_psho_restart_timers(transfer_data_ptr->mac_transfer_substate,gas_id);

                  global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate = PSHO_NOT_ACTIVE;
                  MSG_GERAN_MED_0_G("GMAC PSHO State = PSHO_NOT_ACTIVE ");
                }
                break;

              default:
                MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in psho substate %d task id %d sig id 0x%x",
                                     global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate,task_id, signal_id);
                /* unknown signal from MAC */
                break;
            }
          }
          break;

        case PSHO_CONNECTING_TARGET_CHANNELS:
          if(task_id == MS_MAC_L1)
          {
            switch (signal_id)
            {
              case L1_MAC_USF_DETECTED:
                MSG_GERAN_HIGH_0_G("GMAC rcvd L1_MAC_USF_DETECTED");
                if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type == PSHO_NON_SYNC)
                {
                  if( rex_get_timer(&mac_timers_ptr->timer_t3216) == 0 )
                  {
                    MSG_GERAN_MED_0_G("GMAC psho sync type is PSHO_NON_SYNC");

                    MSG_GERAN_MED_0_G("GMACstops T3218 timer and starts T3216 timer");

                    gmac_stop_t3218(gas_id);
                    gmac_start_t3216(gas_id);
                  }
                }
                else if((global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type == PSHO_PRE_SYNC)
                        ||(global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type == PSHO_SYNC))
                {
                  MSG_GERAN_MED_1_G("GMAC psho_sync_type is %d ",global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type);
                  /*GMAC sends PSHO CNF with PSHO_SUCCESS to GRR and stops T3218 timer*/
                  gmac_util_send_grr_psho_cnf(PSHO_SUCCESS,gas_id);

                  gmac_stop_t3218(gas_id);/*Stop T3218*/
                  gmac_stop_t3172(gas_id);/*T3172 might be running so stop T3172*/
                  global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type = PSHO_NO_SYNC;
                }
                else if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type == PSHO_NO_SYNC)
                {
                  MSG_GERAN_MED_0_G("GMAC completed sync procedure and waiting for complete ind from GRR ");
                }
                break;

              case MAC_PH_CONNECT_IND:
                MSG_GERAN_MED_0_G("GMAC rcvd MAC_PH_CONNECT_IND ");
                /*GMAC rcvd MAC_PH_CONNECT_IND signal on the target channels. GMAC waits for USF DETECTED signal */
                if(global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_DOWNLINK_TBF_ESTABLISHED)
                {
                  if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
                  {
                    MSG_GERAN_HIGH_0_G("MAC_PH_CONNECT_IND received with UPLINK_DOWNLINK_TBF_ESTABLISHED");
                  }
                  else
                  {
                    MSG_GERAN_ERROR_1_G("Unexpected MAC_PH_CONNECT_IND received when PSHO TBF TYPE is %d ",
                                        global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type);
                  }
                }
                else if(global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_TBF_ESTABLISHED)
                {
                  if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_TBF)
                  {
                    MSG_GERAN_HIGH_0_G("MAC_PH_CONNECT_IND received with UPLINK_TBF_ESTABLISHED");
                  }
                  else
                  {
                    MSG_GERAN_ERROR_1_G("Unexpected MAC_PH_CONNECT_IND received when PSHO TBF TYPE is %d ",
                                        global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type);
                  }
                }
                else if(global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == DOWNLINK_TBF_ESTABLISHED)
                {
                  MSG_GERAN_HIGH_1_G("Unexpected MAC_PH_CONNECT_IND received in PSHO state psho_substate is %d ",
                                     global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate);
                }
                break;

              case MAC_PH_DATA_IND:
                MSG_GERAN_MED_0_G("GMAC rcvd MAC_PH_DATA_IND");
                gmac_tran_ps_handover_data_ind(gas_id);
                break;

              case L1_MAC_TBF_REL_CONFIRM:
                MSG_GERAN_MED_0_G("GMAC rcvd L1_MAC_TBF_REL_CONFIRM. Gmac stores this signal in psho strucutre");
                gmac_tran_store_tbf_rel_cnf(gas_id);
                break;

              default:
                MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in psho substate %d task id %d sig id 0x%x",
                                     global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate,task_id, signal_id);
                /* unknown signal from L1 */
                break;
            }
          }
          else if(task_id == MS_MAC_RLC)
          {
            switch (signal_id)
            {
              case RM_UL_RELEASE_IND:
              case RM_DL_RELEASE_IND:
                gmac_tran_store_rlc_release_ind(signal_id,gas_id);
                break;

              default:
                MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in psho substate %d task id %d sig id 0x%x",
                                     global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate,task_id, signal_id);
                /* unknown signal from RLC */
                break;
            }
          } /* end of MAC_RLC signals */
          else if (task_id == MS_MAC_MAC)
          {
            switch (signal_id)
            {
              case MAC_TIMER_T3172:
                MSG_GERAN_HIGH_0_G("GMAC rcvd T3172 expiry");
                break;

              case MAC_TIMER_T3218:
                MSG_GERAN_MED_0_G("GMAC rcvd MAC_TIMER_T3218 Expiry");
                if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_type == PSHO_GTOG)
                {
                  MSG_GERAN_MED_0_G("GMAC psho type is PSHO_GTOG");
                  /*GMAC sends PSHO STATUS to GL1 with PSHO FAILURE*/
                  gmac_tran_send_gl1_psho_status(PSHO_FAILURE,gas_id);

                  global_data_ptr->gmac_ps_ho.psho_pccf_params.cause = PSHO_NO_RESPONSE_ON_TARGET_CELL;

                  global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate = PSHO_RECONNECTING_OLD_CHANNELS;
                  MSG_GERAN_MED_0_G("GMAC PSHO State = PSHO_RECONNECTING_OLD_CHANNELS ");
                }
                break;

              case MAC_TIMER_T3216:
                MSG_GERAN_MED_0_G("GMAC rcvd MAC_TIMER_T3216 Expiry");
                if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_type == PSHO_GTOG)
                {
                  MSG_GERAN_MED_0_G("GMAC psho type is GTOG");
                  /*GMAC psho status is connecting target channels. so sending failure to GL1*/
                  gmac_tran_send_gl1_psho_status(PSHO_FAILURE,gas_id);

                  global_data_ptr->gmac_ps_ho.psho_pccf_params.cause = PSHO_NO_RESPONSE_ON_TARGET_CELL;

                  global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate = PSHO_RECONNECTING_OLD_CHANNELS;
                  MSG_GERAN_MED_0_G("GMAC PSHO State = PSHO_RECONNECTING_OLD_CHANNELS ");
                }
                break;
              default:
                MSG_GERAN_ERROR_3_G( "Unexpected MAC signal in psho substate %d task id %d sig id 0x%x",
                                     global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate,task_id, signal_id);
                /* unknown signal from MAC */
                break;
            }
          }
          else if(task_id == MS_MAC_RR)
          {
            switch (signal_id)
            {
              case GRR_MAC_PSHO_COMPLETE_IND:
                if(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_psho_complete_ind.psho_status == PSHO_SUCCESS)
                {
                  MSG_GERAN_MED_0_G("GMAC rcvd GRR_MAC_PSHO_COMPLETE_IND signal with PSHO SUCCESS");
                  transfer_data_ptr->t_flags.ul_tbf_normal_release_ind               = FALSE;
                  transfer_data_ptr->t_flags.dl_tbf_normal_release_ind               = FALSE;
                  transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag          = FALSE;
                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul               = FALSE;
                  transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl               = FALSE;
                  transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg              = FALSE;
                  transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
                  transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set                = FALSE;
                  global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
                  global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;

                  if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_type == PSHO_GTOG)
                  {
                    MSG_GERAN_MED_1_G("GMAC psho type is %d",global_data_ptr->gmac_ps_ho.psho_control_params.psho_type);
                    /*update ul tbf global variables*/
                    global_data_ptr->gmac_local_tfi_store.ul_tfi = global_data_ptr->gmac_ps_ho.psho_config_params.psho_assign_ul_tfi;
                    global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = TRUE;

                    global_data_ptr->gmac_local_ul_ts_alloc = global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_ts;
                    global_data_ptr->current_egprs_ul_ws = global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.ws;
                    /*if mode changes in ps handover procedure set current tbf mode correctly*/
                    if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
                    {
                      if(global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.tbf_mode == TBF_MODE_EGPRS)
                      {
                        MSG_GERAN_MED_0_G("GMAC Mode is changed from GPRS to EGPRS during PS HANDOVER");
                        global_data_ptr->current_tbf_mode = global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.tbf_mode;
                      }
                    }
                    else if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
                    {
                     if(global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.tbf_mode == TBF_MODE_GPRS)
                      {
                        MSG_GERAN_MED_0_G("GMAC Mode is changed from EGPRS to GPRS during PS HANDOVER");
                        global_data_ptr->current_tbf_mode = global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.tbf_mode;
                      }
                    }

                    /*GMAC sends complete ind with PSHO SUCCESS to GRLC UL task*/
                    gmac_tran_send_grlcul_psho_complete_ind(PSHO_SUCCESS,gas_id);

                    if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
                    {
                      /*update dl tbf global variables*/
                      /*GMAC sends complete ind with PSHO SUCCESS to GRLC DL task*/
                      gmac_tran_send_grlcdl_psho_complete_ind(PSHO_SUCCESS,gas_id);

                      global_data_ptr->gmac_local_tfi_store.dl_tfi = global_data_ptr->gmac_ps_ho.psho_config_params.psho_assign_dl_tfi;
                      global_data_ptr->gmac_local_tfi_store.dl_tfi_valid = TRUE;

                      global_data_ptr->gmac_local_dl_ts_alloc = global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_assign_ts;
                      global_data_ptr->current_egprs_dl_ws = global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_ass_info.ws;
                      global_data_ptr->gmac_store_dl_rlc_mode = (uint8)global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_rlc_mode;

                      transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;
                      transfer_data_ptr->mac_tran_substate_str = gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate,gas_id);
                      MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                      (void)mac_log_state(gas_id);
                    }
                    else if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_TBF)
                    {
                      transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC;
                      transfer_data_ptr->mac_tran_substate_str =
                        gmac_convert_transfer_substate_to_string(transfer_data_ptr->mac_transfer_substate,gas_id);
                      MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, transfer_data_ptr->mac_tran_substate_str);
                      (void)mac_log_state(gas_id);
                      /*if there is downlink tbf running on the old cell then mac should
                      release dl tbf locally*/
                      gmac_transfer_initiate_local_release_of_old_cell_dl_tbfs(gas_id);
                      gmac_transfer_dl_release_clean(gas_id);
                    }

                    gmac_tran_psho_restart_timers(transfer_data_ptr->mac_transfer_substate,gas_id);

                    /*GMAC sends psho status with PSHO SUCCESS to GL1 task*/
                    gmac_tran_send_gl1_psho_status(PSHO_SUCCESS,gas_id);

                    gmac_util_reset_psho_structure(gas_id);
                    /*if T3172 timer is expired in PS HANDOVER state we need to unblock
                      RLC after PS HANDOVER completion*/
                    if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__,gas_id) == BLOCKED)
                    {
                      gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__,gas_id);
                    }
                  }
                  global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate = PSHO_NOT_ACTIVE;
                  MSG_GERAN_MED_0_G("GMAC PSHO State = PSHO_NOT_ACTIVE ");
                }
                else if(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_psho_complete_ind.psho_status == PSHO_FAILURE)
                {
                  MSG_GERAN_MED_0_G("GMAC rcvd GRR_MAC_PSHO_COMPLETE_IND signal with PSHO FAILURE");
                }
                break;
              default:
                MSG_GERAN_ERROR_3_G( "Unexpected GRR signal in psho substate %d task id %d sig id 0x%x",
                                     global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate,task_id, signal_id);
                /* unknown signal from GRR */
                break;
            }
          }
          break;

        case PSHO_RECONNECTING_OLD_CHANNELS:
          if(task_id == MS_MAC_L1)
          {
            switch (signal_id)
            {
              case MAC_PH_CONNECT_IND:
                MSG_GERAN_MED_0_G("GMAC rcvd MAC_PH_CONNECT_IND");
                if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_type == PSHO_GTOG)
                {
                  if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
                  {
                    /*GMAC sends psho complete ind with PSHO FAILURE to GRLC DL task*/
                    gmac_tran_send_grlcdl_psho_complete_ind(PSHO_FAILURE,gas_id);
                  }
                  /*GMAC sends psho complete ind with PSHO FAILURE to GRLC UL task*/
                  gmac_tran_send_grlcul_psho_complete_ind(PSHO_FAILURE,gas_id);
                }

                /*GMAC sends psho cnf with PSHO FAILURE to GRR*/
                gmac_util_send_grr_psho_cnf(PSHO_FAILURE,gas_id);

                /*GMAC setting mac_transfer_substate value from gmac_ps_ho structure*/

                transfer_data_ptr->mac_transfer_substate = global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate;
                MSG_GERAN_MED_1_G("GMAC State Tran = %d",transfer_data_ptr->mac_transfer_substate);

                /*GMAC process saved message any received in PSHO state*/
                gmac_tran_psho_process_saved_msgs(transfer_data_ptr->mac_transfer_substate,gas_id);

                if(transfer_data_ptr->mac_transfer_substate != TRANSFER_INACTIVE)
                {
                  /*GMAC trigger pccf sending process*/
                  gmac_tran_psho_trigger_pccf_sending(transfer_data_ptr->mac_transfer_substate,gas_id);
                }
                /*Reset PSHO structure*/
                gmac_util_reset_psho_structure(gas_id);

                if(transfer_data_ptr->mac_transfer_substate == TRANSFER_DL)
                {
                  if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
                  {
                    if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__,gas_id) == BLOCKED)
                    {
                      gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__,gas_id);
                    }
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("GMAC T3172 still running leave RLC blocked");
                  }
                }
                gmac_tran_psho_restart_timers(transfer_data_ptr->mac_transfer_substate,gas_id);

                global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate = PSHO_NOT_ACTIVE;
                MSG_GERAN_MED_0_G("GMAC PSHO State = PSHO_NOT_ACTIVE ");
              break;

            case L1_MAC_TBF_REL_CONFIRM:
              MSG_GERAN_MED_0_G("GMAC rcvd L1_MAC_TBF_REL_CONFIRM. Gmac stores this signal in psho strucutre");
              gmac_tran_store_tbf_rel_cnf(gas_id);
              break;

            default:
              MSG_GERAN_ERROR_3_G( "Unexpected L1 signal in psho substate %d task id %d sig id 0x%x",
                                   global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate,task_id, signal_id);
                /* unknown signal from L1 */
              break;
            }
          }
          else if(task_id == MS_MAC_RLC)
          {
            switch (signal_id)
            {
              case RM_UL_RELEASE_IND:
              case RM_DL_RELEASE_IND:
                gmac_tran_store_rlc_release_ind(signal_id,gas_id);
                break;

              default:
                MSG_GERAN_ERROR_3_G( "Unexpected RLC signal in psho substate %d task id %d sig id 0x%x",
                                     global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate,task_id, signal_id);
                /* unknown signal from RLC */
                break;
            }
          } /* end of MAC_RLC signals */
          break;

        default:
          if(geran_get_nv_recovery_restart_enabled(gas_id))
          {
            MSG_GERAN_ERROR_1_G( "Unknown psho substate %d",global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate);
            MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
            geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
          }
          else
          {
            ERR_GERAN_FATAL_2_G("Unknown psho substate %d",gas_id, global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate);
          }
          /* unknown signal from RLC */
          break;
      }
      break; /* end of TRANSFER_PS_HANDOVER*/
#endif /*FEATURE_GPRS_PS_HANDOVER*/
    default:
      if(geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_1_G( "Unknown state %d",transfer_data_ptr->mac_transfer_substate);
        MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
        geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
      }
      else
      {
        ERR_GERAN_FATAL_2_G("Unknown state %d",gas_id, transfer_data_ptr->mac_transfer_substate);
      }
      /* unknown state in TRANSFER mode */
      break;
  }
}
#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      gmac_tran_ignore_pending_tbfs()
===
===  DESCRIPTION
===  This function ignores the pending tbfs
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  gprs_mac_transfer_substates_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static gprs_mac_transfer_substates_t gmac_tran_ignore_pending_tbfs(gprs_mac_transfer_substates_t transfer_substate,
                                                                   gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  global_data_ptr->gmac_local_ul_realloc_ts  = 0;
  global_data_ptr->gmac_local_dl_reassign_ts = 0;
  global_data_ptr->gmac_re_assign_ul_tfi     = 0xff;
  global_data_ptr->gmac_re_assign_dl_tfi     = 0xff;

  transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = FALSE;
  if(transfer_substate ==  TRANSFER_DL_REASSIGN)
  {
    MSG_GERAN_MED_0_G( "MAC ignoring pending DL TBF");
    if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
    {
      MSG_GERAN_MED_0_G("T3172 is not running if rlc is blocked GMAC sends access ind to GRLC");
      if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED)
      {
        gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_UTIL, __LINE__, gas_id);
      }
    }
    return(TRANSFER_DL);
  }
  else if((transfer_substate ==  TRANSFER_DL_WITH_DYNAMIC_ALLOC)
     ||(transfer_substate ==  TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC))
  {
    if(transfer_substate ==  TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC)
    {
      MSG_GERAN_MED_0_G( "MAC ignoring pending UL & DL TBF");
    }
    else if(transfer_substate ==  TRANSFER_DL_WITH_DYNAMIC_ALLOC)
    {
      MSG_GERAN_MED_0_G( "MAC ignoring pending DL TBF");
    }

    global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
    transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;
    transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
    if(transfer_data_ptr->t_flags.dl_tbf_normal_release_ind)
    {
      gmac_transfer_dl_release_clean(gas_id);
      gmac_util_block_rlc(NO_DEL,GMAC_UTIL, __LINE__, gas_id);
      gmac_tran_send_mac_l1_release(DL_TBF_NORMAL, gas_id);
      global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
      transfer_data_ptr->t_flags.dl_tbf_normal_release_ind = FALSE;
    }
    return(TRANSFER_DL);
  }
  else if((transfer_substate ==  TRANSFER_UL_DYNAMIC_REALLOC)
     ||(transfer_substate ==  TRANSFER_UL_DYNAMIC_DL_ASSIGN)
     ||(transfer_substate ==  TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN))
  {
    if(transfer_substate ==  TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN)
    {
      MSG_GERAN_MED_0_G( "MAC ignoring pending UL & DL TBF");
    }
    else if(transfer_substate ==  TRANSFER_UL_DYNAMIC_DL_ASSIGN)
    {
      MSG_GERAN_MED_0_G( "MAC ignoring pending DL TBF");
    }
    else if(transfer_substate ==  TRANSFER_UL_DYNAMIC_REALLOC)
    {
      MSG_GERAN_MED_0_G( "MAC ignoring pending UL TBF");
    }

    global_data_ptr->gmac_local_tfi_store.dl_tfi_valid = FALSE;
    transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;

    if(transfer_data_ptr->t_flags.ul_tbf_normal_release_ind)
    {
      /*ul tbf normal release ind is set means ul tfi is not valid
        so we cannot validate PS HO command message with tfi
        so this case should not happen*/
      MSG_GERAN_ERROR_0_G( "MAC received PS_HANDOVER_COMMAND from network when ul tbf normal release ind is set");
    }
    return(TRANSFER_UL_DYNAMIC);
  }
  else if(transfer_substate ==  TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN)
  {
    MSG_GERAN_MED_0_G( "MAC ignoring pending DL TBF");
    return(TRANSFER_CONCURRENT_DYNAMIC_TRANSFER);
  }
  else if(transfer_substate ==  TRANSFER_CONCURRENT_DYNAMIC_REALLOC)
  {
    MSG_GERAN_MED_0_G( "MAC ignoring pending UL TBF");
    return(TRANSFER_CONCURRENT_DYNAMIC_TRANSFER);
  }
  else if(transfer_substate ==  TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC)
  {
    MSG_GERAN_MED_0_G( "MAC ignoring pending UL&DL TBF");
    return(TRANSFER_CONCURRENT_DYNAMIC_TRANSFER);
  }
  return(transfer_substate);
}

static void gmac_tran_psho_restart_timers(gprs_mac_transfer_substates_t gmac_transfer_substate, gas_id_t gas_id)
{
  if(gmac_transfer_substate == TRANSFER_DL)
  {
    /*MAC re start T3190 TIMER*/
    grlc_dl_start_t3190(gas_id);
  }
  else if(gmac_transfer_substate == TRANSFER_UL_DYNAMIC)
  {
    /*MAC re start T3180 TIMER*/
    gmac_start_t3180(gas_id);
  }
  else if(gmac_transfer_substate == TRANSFER_CONCURRENT_DYNAMIC_TRANSFER)
  {
    /*MAC re start T3190 & T3180 TIMERS*/
    grlc_dl_start_t3190(gas_id);
    gmac_start_t3180(gas_id);
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_transfer_initiate_local_release_of_old_cell_dl_tbfs()
===
===  DESCRIPTION
===    This function will release dl tbf if running in the old cell when there
===    is no downlink tbf assigned in PSHO COMMAND
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    void
===
===  SIDE EFFECTS
===
===========================================================================*/
static void gmac_transfer_initiate_local_release_of_old_cell_dl_tbfs(gas_id_t gas_id)
{
  gprs_mac_transfer_substates_t old_cell_transfer_substate;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  old_cell_transfer_substate = global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate;

  if(global_data_ptr->gmac_ps_ho.psho_control_params.control_ack_set == FALSE)
  {
    if(global_data_ptr->gmac_ps_ho.psho_control_params.dl_release_ind_ptr == NULL)
    {
      if((old_cell_transfer_substate == TRANSFER_DL)
         ||(old_cell_transfer_substate == TRANSFER_CONCURRENT_DYNAMIC_TRANSFER))
      {
        /*Release old cell dl tbf locally*/
        gmac_transfer_dl_release_clean(gas_id);
        gmac_util_send_dl_release_to_rlc(gas_id);
      }
    }
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_send_gl1_psho_cancel_pending_config()
===
===  DESCRIPTION
===  This function sends MAC_L1_PSHO_CANCEL_PENDING_CONFIG signal to GL1
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
static void gmac_tran_send_gl1_psho_cancel_pending_config(gas_id_t gas_id)
{
  mac_l1_sig_t         mac_l1_sig;      /* mac to l1 signal   */

  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_PSHO_CANCEL_PENDING_CONFIG ");
  mac_l1_sig.mac_l1_psho_cancel_pending_config.message_header.message_set = MS_MAC_L1;
  mac_l1_sig.mac_l1_psho_cancel_pending_config.message_header.message_id = (uint8)MAC_L1_PSHO_CANCEL_PENDING_CONFIG;

  gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_send_gl1_psho_status()
===
===  DESCRIPTION
===  This function sends PSHO_STATUS signal to GL1
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
static void gmac_tran_send_gl1_psho_status(psho_status_t psho_status,
                                           gas_id_t gas_id)
{
  mac_l1_sig_t         mac_l1_sig;      /* mac to l1 signal   */

  MSG_GERAN_MED_1_G("MAC Sent MAC_L1_PSHO_STATUS %d", psho_status);

  mac_l1_sig.mac_l1_psho_status.psho_status = psho_status;
  mac_l1_sig.mac_l1_psho_status.message_header.message_set = MS_MAC_L1;
  mac_l1_sig.mac_l1_psho_status.message_header.message_id = (uint8)MAC_L1_PSHO_STATUS;

  gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_send_grlcul_psho_complete_ind()
===
===  DESCRIPTION
===  This function sends MR_UL_PSHO_COMPLETE_IND signal to GRLC UL task
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
static void gmac_tran_send_grlcul_psho_complete_ind(psho_status_t psho_status,
                                                    gas_id_t gas_id)
{
  mac_rlc_msg_t  *mac_rlc_sig_ptr;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  mac_rlc_sig_ptr = grlc_get_cmd_buf_mac();

  if (mac_rlc_sig_ptr != NULL)
  {
    MSG_GERAN_MED_1_G("MAC Sent MR_UL_PSHO_COMPLETE_IND %d",psho_status);
    mac_rlc_sig_ptr->msg.ul_psho_complete_ind.psho_status = psho_status;
    if(psho_status == PSHO_SUCCESS)
    {
      mac_rlc_sig_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf = global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info;
    }

    mac_rlc_sig_ptr->msg.ul_psho_complete_ind.psho_type = global_data_ptr->gmac_ps_ho.psho_control_params.psho_type;
    mac_rlc_sig_ptr->msg.ul_psho_complete_ind.reset_flag = global_data_ptr->gmac_ps_ho.psho_config_params.psho_reset_flag;

    mac_rlc_sig_ptr->msg.ul_psho_complete_ind.nas_container_present = global_data_ptr->gmac_ps_ho.psho_config_params.psho_nas_container_flag;
    mac_rlc_sig_ptr->msg.ul_psho_complete_ind.ul_rlc_mode           = (uint8)global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_rlc_mode;
    mac_rlc_sig_ptr->msg.ul_psho_complete_ind.sig = MR_UL_PSHO_COMPLETE_IND;

    gmac_rlc_mac_insert_gas_id(mac_rlc_sig_ptr,gas_id);
    grlc_put_cmd_mac(gas_id,mac_rlc_sig_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_tran_send_grlcdl_psho_complete_ind()
===
===  DESCRIPTION
===  This function sends MR_DL_PSHO_COMPLETE_IND signal to GRLC DL task
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
static void gmac_tran_send_grlcdl_psho_complete_ind(psho_status_t psho_status,
                                                    gas_id_t gas_id)
{
  mac_rlc_msg_t  *mac_rlc_sig_ptr;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  mac_rlc_sig_ptr = grlc_get_cmd_buf_mac();

  if (mac_rlc_sig_ptr != NULL)
  {
    MSG_GERAN_MED_1_G("MAC Sent MR_DL_PSHO_COMPLETE_IND %d",psho_status);
    mac_rlc_sig_ptr->msg.dl_psho_complete_ind.psho_status = psho_status;

    if(psho_status == PSHO_SUCCESS)
    {
      if(global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_ass_present == TRUE)
      {
        mac_rlc_sig_ptr->msg.dl_psho_complete_ind.dl_ass_ind = global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_ass_info;
        mac_rlc_sig_ptr->msg.dl_psho_complete_ind.dl_ass_ind_present = TRUE;
      }
      else
      {
        mac_rlc_sig_ptr->msg.dl_psho_complete_ind.dl_ass_ind_present = FALSE;
      }
    }
    else
    {
      mac_rlc_sig_ptr->msg.dl_psho_complete_ind.dl_ass_ind_present = FALSE;
    }

    mac_rlc_sig_ptr->msg.dl_psho_complete_ind.psho_type = global_data_ptr->gmac_ps_ho.psho_control_params.psho_type;
    mac_rlc_sig_ptr->msg.dl_psho_complete_ind.reset_flag = global_data_ptr->gmac_ps_ho.psho_config_params.psho_reset_flag;

    mac_rlc_sig_ptr->msg.dl_psho_complete_ind.nas_container_present = global_data_ptr->gmac_ps_ho.psho_config_params.psho_nas_container_flag;

    mac_rlc_sig_ptr->msg.dl_psho_complete_ind.sig = MR_DL_PSHO_COMPLETE_IND;

    gmac_rlc_mac_insert_gas_id(mac_rlc_sig_ptr,gas_id);
    grlc_put_cmd_mac(gas_id,mac_rlc_sig_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_psho_trigger_pccf_sending()
===
===  DESCRIPTION
===  This function trigger the PCCF sending procedure because of PSHO failure.
===  This function forms the PCCF message and call gmac_util_send_ul_control_msg_handler
===  function with appropriate parameters depending upon the mac transfer substate
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
static void gmac_tran_psho_trigger_pccf_sending(gprs_mac_transfer_substates_t transfer_substate,
                                                gas_id_t gas_id)
{
  int_data_t input_data;
  uint32 local_rrps_tlli;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);

  input_data.type.pkt_cell_chg_failure.tlli         = local_rrps_tlli;

  if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_type == PSHO_GTOG)
  {
    input_data.type.pkt_cell_chg_failure.arfcn.num  = global_data_ptr->gmac_ps_ho.psho_pccf_params.psho_arfcn.num;

    input_data.type.pkt_cell_chg_failure.arfcn.band = global_data_ptr->gmac_ps_ho.psho_pccf_params.psho_arfcn.band;
    input_data.type.pkt_cell_chg_failure.bsic       = global_data_ptr->gmac_ps_ho.psho_pccf_params.psho_bsic;

    input_data.type.pkt_cell_chg_failure.rel99      = FALSE;
  }

  input_data.type.pkt_cell_chg_failure.cause = global_data_ptr->gmac_ps_ho.psho_pccf_params.cause;

  if(gcsn1_encoder(PACKET_CELL_CHANGE_FAILURE, &(global_data_ptr->gmac_local_pccf_params.csn_encoded_pccf[1]), &input_data) == PASS)
  {
    if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
    {
      global_data_ptr->gmac_local_pccf_params.csn_encoded_pccf[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
    }
    else
    {
      global_data_ptr->gmac_local_pccf_params.csn_encoded_pccf[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
    }
    /*T_state should be T_DL or T_UL_DYN or T_CON*/
    if(transfer_substate == TRANSFER_DL)
    {
      gmac_util_send_ul_control_msg_handler(PCCF, DL_ONLY_PRESENT, GRR_CNF_NOT_NEEDED, gas_id);
    }
    else
    {
      gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NOT_NEEDED, gas_id);
    }
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_psho_handler()
===
===  DESCRIPTION
===  This function validates PSHO command message and if validation successful
===  this function forwards the message to GRR. This function also ignores the
===  pending tbf and sends MAC_L1_PSHO_CANCEL_PENDING_CONFIG to GL1
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  void
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_tran_psho_handler(gas_id_t gas_id)
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  if(gmac_tran_check_psho_addressing(gas_id) == TRUE)
  {
    MSG_GERAN_MED_0_G("MAC Rcvd PS_HANDOVER_COMMAND");
    gmac_send_data_ind_to_grr(gas_id);
    rrbp_control_block_handler_2(PCA_DL_MSG, gas_id);
    gmac_tran_send_gl1_psho_cancel_pending_config(gas_id);
    MSG_GERAN_MED_0_G("GMAC ignores the pending tbfs ");
    transfer_data_ptr->mac_transfer_substate = gmac_tran_ignore_pending_tbfs(transfer_data_ptr->mac_transfer_substate, gas_id);
    (void)mac_log_state(gas_id);
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_ps_handover_data_ind()
===
===  DESCRIPTION
===  This function process the PKT_PHYSICAL_INFOMRATION message.
===  if validation successful then gmac sends PSHO_CNF to GRR with PSHO_SUCCESS
===  otherwise it ignores the message
===  if GMAC receives any other OTA messages then it ignores the message
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_tran_ps_handover_data_ind(gas_id_t gas_id)
{
  packet_physical_information_t  *pkt_phy_info;
  mac_l1_sig_t  mac_l1_sig;
  mac_l1_pwr_ctrl_t *mac_l1_ta_msg = &(mac_l1_sig.mac_l1_power_control);
  boolean addr_match = FALSE;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);


  memset( &mac_l1_sig, 0, sizeof( mac_l1_sig_t ));

  pkt_phy_info = (packet_physical_information_t *)global_data_ptr->csn_dec_data_ptr;

  if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type == PSHO_NON_SYNC)
  {
    if(global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_PHYSICAL_INFORMATION)
    {
      if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
      {
        addr_match = gmac_util_check_pkt_phy_info_global_tfi(pkt_phy_info->global_tfi,
                                                             gas_id);
      }
      if(addr_match)
      {
        /*address matches so sending timing advance to GL1*/
        if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate == PSHO_CONNECTING_TARGET_CHANNELS)
        {
          /*psho state is connecting target channels so stop T3216 timer and stop T3172 if running*/
          gmac_stop_t3216(gas_id);
          gmac_stop_t3172(gas_id);
          /*Reset sync type */
          global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type = PSHO_NO_SYNC;
          MSG_GERAN_MED_0_G( "MAC Reset psho sync type to PSHO NO SYNC");
          /*Send RRBP to GL1 if s/p is set*/
          rrbp_control_block_handler_2(PCA_DL_MSG, gas_id);
          mac_l1_ta_msg->power_control.present                 = FALSE;

          mac_l1_ta_msg->global_packet_ta_present              = TRUE;

          mac_l1_ta_msg->global_packet_ta.tav.present          = TRUE;
          mac_l1_ta_msg->global_packet_ta.tav.ta_value         = pkt_phy_info->timing_advance_value;

          mac_l1_ta_msg->global_packet_ta.uplink_tai.present   = FALSE;
          mac_l1_ta_msg->global_packet_ta.downlink_tai.present = FALSE;

          /* send MAC_L1_POWER_CONTROL with parameters */
          MSG_GERAN_LOW_0_G( "MAC Send MAC_L1_POWER_CONTROL");
          mac_l1_sig.mac_l1_power_control.message_header.message_set = MS_MAC_L1;
          mac_l1_sig.mac_l1_power_control.message_header.message_id = (uint8)MAC_L1_POWER_CONTROL;

          gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
          /*PSHO successful so sending PSHO SUCCESS to GRR and WAIT for PSHO COMPLETE IND from GRR
            to complete PSHO procedure*/
          gmac_util_send_grr_psho_cnf(PSHO_SUCCESS, gas_id);
        }
        else
        {
          MSG_GERAN_MED_1_G( "MAC receives PKT PHY INFO message when psho status is %d",
                             global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate);
        }
      }
      else
      {
        MSG_GERAN_MED_0_G( "MAC address is not matched so ignoring PKT PHY INFO message");
      }
    }
    else
    {
      MSG_GERAN_MED_1_G( "MAC ignors the other OTA messages except PKT PHY INFO message, rcvd OTA message is %d",
                         global_data_ptr->csn_dec_hdr_ptr->message_type);
    }
  }
  else
  {
    MSG_GERAN_MED_1_G( "MAC ignors the OTA messages and the sync type is %d",
                       global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type);
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_store_tbf_rel_cnf()
===
===  DESCRIPTION
===  This function stores the tbf rel cnf signal in PSHO CONTEXT strucutre
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  void
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_tran_store_tbf_rel_cnf(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  global_data_ptr->gmac_ps_ho.psho_control_params.tbf_rel_cnf_ptr = (l1_mac_sig_t *)GPRS_MEM_MALLOC(sizeof(l1_mac_sig_t));

  if (global_data_ptr->gmac_ps_ho.psho_control_params.tbf_rel_cnf_ptr != NULL)
  {
    *global_data_ptr->gmac_ps_ho.psho_control_params.tbf_rel_cnf_ptr = global_data_ptr->msg_ptr->l1_mac_sig;
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_store_rlc_release_ind()
===
===  DESCRIPTION
===  This function stores the rlc release ind signal in PSHO CONTEXT strucutre
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  void
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_tran_store_rlc_release_ind(uint8 rlc_mac_sig,
                                            gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  if(rlc_mac_sig == (uint8)RM_UL_RELEASE_IND)
  {
    MSG_GERAN_MED_0_G("GMAC received RM UL RELEASE IND. gmac stores this signal in PSHO structure");
    global_data_ptr->gmac_ps_ho.psho_control_params.ul_release_ind_ptr = (rlc_mac_msg_t *)GPRS_MEM_MALLOC(sizeof(rlc_mac_msg_t));

    if (global_data_ptr->gmac_ps_ho.psho_control_params.ul_release_ind_ptr != NULL)
    {
      *global_data_ptr->gmac_ps_ho.psho_control_params.ul_release_ind_ptr = global_data_ptr->msg_ptr->rlc_mac_sig;
    }
    else
    {
      ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
    }
  }
  else if(rlc_mac_sig == (uint8)RM_DL_RELEASE_IND)
  {
    MSG_GERAN_MED_0_G("GMAC received RM DL RELEASE IND. gmac stores this signal in PSHO structure");
    global_data_ptr->gmac_ps_ho.psho_control_params.dl_release_ind_ptr = (rlc_mac_msg_t *)GPRS_MEM_MALLOC(sizeof(rlc_mac_msg_t));

    if (global_data_ptr->gmac_ps_ho.psho_control_params.dl_release_ind_ptr != NULL)
    {
      *global_data_ptr->gmac_ps_ho.psho_control_params.dl_release_ind_ptr = global_data_ptr->msg_ptr->rlc_mac_sig;
    }
    else
    {
      ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
    }
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_dl_process_rel_cnf()
===
===  DESCRIPTION
===  This function process the release cnf signal in T_DL state from GL1
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  void
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_tran_dl_process_rel_cnf(tbf_released_t tbf_released,
                                         gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");
#ifdef FEATURE_GSM_DTM
  gmac_tran_rel_state_handle_rel_cnf( tbf_released, gas_id );
#endif /*FEATURE_GSM_DTM*/

  if (tbf_released == DL_TBF_RELEASE_CONF)
  {
    gmac_transfer_tbf_rel_confirm(DL_TBF_RELEASE_CONF, gas_id);
  }
  else
  {
    /* This can only happen if PH_CONNECT_IND DL is received in T_UL_DYN_DL_ASS
    before the REL_CONF for UL during normal UL release.  T3172 cannot be started
    in T_UL_DYN_DL_ASS so should not be running.

    Following pre IPA design here.  RLC should have already been released in T_UL_DYN_DL_ASS
    when the PH_CONNECT_IND (DL) was received, but old design maintained with an additional
    call to unblock RLC here.
    */
    gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
    global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_ul_dynamic_process_rel_cnf()
===
===  DESCRIPTION
===  This function process the release cnf signal in T_UL_DYN state from GL1
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  void
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_tran_ul_dynamic_process_rel_cnf(tbf_released_t tbf_released,
                                                 gas_id_t       gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
  gmac_tran_rel_state_handle_rel_cnf( tbf_released, gas_id );
#endif /*FEATURE_GSM_DTM*/
  transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
  transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release = FALSE;
  if (tbf_released == UL_TBF_RELEASE_CONF)
  {
    gmac_transfer_tbf_rel_confirm(UL_TBF_RELEASE_CONF, gas_id);
    global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_concurrent_process_rel_cnf()
===
===  DESCRIPTION
===  This function process the release cnf signal in T_CON state from GL1
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  void
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_tran_concurrent_process_rel_cnf(tbf_released_t tbf_released,
                                                 gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

#ifdef FEATURE_GSM_DTM
  gmac_tran_rel_state_handle_rel_cnf( tbf_released, gas_id );
#endif /*FEATURE_GSM_DTM*/

  if (tbf_released == UL_DL_TBF_RELEASE_CONF)
  {
    /* for abnormal release case, therefore release to NULL/IDLE */
    gmac_transfer_tbf_rel_confirm(UL_DL_TBF_RELEASE_CONF, gas_id);
    transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
    transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release = FALSE;
  }
  else if ((tbf_released == DL_TBF_RELEASE_CONF)
            && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == FALSE))
  {
    MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYN");

    /* flag set when RM_DL_RELEASE_IND is released during concurrent*/
    /* tbf operation reset if L1 confirms dl before ul              */

    MSG_GERAN_MED_1_G("mac info wait_for_ul_rel_confirm = %d",global_data_ptr->c_flags.wait_for_ul_rel_confirm);
    if (global_data_ptr->c_flags.wait_for_ul_rel_confirm == TRUE)
    {
      global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
    }
    else
    {
      global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
    }

    transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC;
    (void)mac_log_state(gas_id);
  }
  else if ((tbf_released != UL_DL_TBF_RELEASE_CONF)
                && (transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg == TRUE))
  {
    /* This is the case where an abnormal release that results in both UL and DL tbf*/
    /* being terminated has been initiated while a release is already in progress and*/
    /* MAC is waiting for L1 to confirm the release. Under these conditions MAC must*/
    /* wait for an UL and DL rel confirm form L1 before progressing                 */
    MSG_GERAN_MED_1_G("ABNORMAL UL & DL rel in progress, require rel confirm for both UL and DL to continue. rel type = %d",tbf_released);

  }
  else /* UL_TBF_RELEASE_CONF */
  {
    MSG_GERAN_MED_0_G("GMAC State tran = T_DL");
    global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
    MSG_GERAN_MED_0_G("mac info wait_for_ul_rel_confirm = FALSE");
    global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;

    /*ensure that RLC is not blocked if returning to T_DL_REASS */
    if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
    {
      if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__, gas_id) == BLOCKED)
      {
        gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
      }
    }
    else
    {
      MSG_GERAN_HIGH_0_G("GMAC T3172 still running leave RLC blocked");
    }

    global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
    transfer_data_ptr->mac_transfer_substate = TRANSFER_DL;
    transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
    transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release = FALSE;
    global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
    (void)mac_log_state(gas_id);

    gmac_eng_mode_update_tbf_info((uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER, global_data_ptr->current_tbf_mode,FALSE,gas_id);

    if(global_data_ptr->gmac_local_pccn_params.pccn_pending  != NO_PENDING_PCCN)
    {
      gmac_util_send_ul_control_msg_handler(PCCN, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
    }
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_psho_process_saved_msgs()
===
===  DESCRIPTION
===  This function process the saved messages received in PSHO state
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  void
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_tran_psho_process_saved_msgs(gprs_mac_transfer_substates_t transfer_substate,
                                              gas_id_t gas_id)
{
  tbf_released_t             tbf_released;
  rm_tbf_release_t           cause;
  gmac_rlc_dl_rel_cause_t    release_rlc_dl;
  gmac_global_data_t        *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t      *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  if(global_data_ptr->gmac_ps_ho.psho_control_params.tbf_rel_cnf_ptr != NULL)
  {
    MSG_GERAN_MED_0_G("GMAC process stored TBF REL CNF message");
    tbf_released = global_data_ptr->gmac_ps_ho.psho_control_params.tbf_rel_cnf_ptr->msg.tbf_released.tbf_released;
    if(transfer_substate == TRANSFER_DL)
    {
      if(tbf_released == UL_DL_TBF_RELEASE_CONF)
      {
        /*tbf released at GL1 is DL+UL pending.
          UL pending is ignored by GMAC at the start of
          PSHO procedure. so modify tbf released flag to
          DL and call gmac_tran_dl_process_rel_cnf function*/
        tbf_released = DL_TBF_RELEASE_CONF;
        transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg = FALSE;
      }
      gmac_tran_dl_process_rel_cnf(tbf_released, gas_id);
    }
    else if(transfer_substate == TRANSFER_UL_DYNAMIC)
    {
      if(tbf_released == UL_DL_TBF_RELEASE_CONF)
      {
        /*tbf released at GL1 is UL+ DLpending.
          DL pending is ignored by GMAC at the start of
          PSHO procedure. so modify tbf released flag to
          UL and call gmac_tran_ul_dynamic_process_rel_cnf function*/
        tbf_released = UL_TBF_RELEASE_CONF;
        transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg = FALSE;
      }
      gmac_tran_ul_dynamic_process_rel_cnf(tbf_released, gas_id);
    }
    else if(transfer_substate == TRANSFER_CONCURRENT_DYNAMIC_TRANSFER)
    {
      gmac_tran_concurrent_process_rel_cnf(tbf_released, gas_id);
    }
  }
  if(global_data_ptr->gmac_ps_ho.psho_control_params.ul_release_ind_ptr != NULL)
  {
    MSG_GERAN_MED_0_G("GMAC process stored UL RELEASE IND message");
    cause = global_data_ptr->gmac_ps_ho.psho_control_params.ul_release_ind_ptr->msg.ul_release_ind.cause;
    /*Sest release_rlc_dl flag depening upon the transfer sub state*/
    if(transfer_substate == TRANSFER_UL_DYNAMIC)
    {
      release_rlc_dl = NO_RLC_DL_TBF_PRESENT_TO_RELEASE;
    }
    else
    {
      release_rlc_dl = REL_RLC_DL;
    }
    if(cause == REL_UL_NORMAL)
    {
      /*Release cause is normal. Do normal release of UL TBF*/
      MSG_GERAN_MED_0_G("MAC UL TBF normal release");
      gmac_transfer_ul_dynamic_release_clean(gas_id);
      global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
      gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__, gas_id);
      gmac_tran_send_mac_l1_release(UL_TBF_NORMAL, gas_id);
      mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE, gas_id);
    }
    else
    {
      /*Release cause is not NORMAL so call abnormal release handler*/
      if(cause == REL_UL_ABNORMAL_CELL_RESEL)
      {
        gmac_abnormal_release_procedure(RR_CELL_RESEL_REQ,
                                        release_rlc_dl,
                                        NO_DEL,
                                        FALSE,
                                        gas_id);
      }
      else
      {
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,
                                        release_rlc_dl,
                                        NO_DEL,
                                        FALSE,
                                        gas_id);
      }
      mac_log_ul_tbf_release(MAC_LOG_UL_RELEASE_IND, gas_id);

    }
  }
  if(global_data_ptr->gmac_ps_ho.psho_control_params.dl_release_ind_ptr != NULL)
  {
    MSG_GERAN_MED_0_G("GMAC process stored DL RELEASE IND message");
    cause = global_data_ptr->gmac_ps_ho.psho_control_params.dl_release_ind_ptr->msg.dl_release_ind.cause;

    if(transfer_substate == TRANSFER_DL)
    {
      if(cause == REL_DL_NORMAL)
      {
        gmac_stop_t3168(gas_id);
        gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__, gas_id);
        gmac_transfer_dl_release_clean(gas_id);
        gmac_transfer_dl_normal_release_to_idle(gas_id);
      }
      else
      {
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,DONT_REL_RLC_DL,NO_DEL,FALSE,gas_id);
        mac_log_dl_tbf_release(MAC_LOG_DL_RELEASE_IND,gas_id);
      }
    }
    else if(transfer_substate == TRANSFER_CONCURRENT_DYNAMIC_TRANSFER)
    {
      if(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_release_ind.cause == REL_DL_NORMAL)
      {
        gmac_transfer_dl_release_clean(gas_id);
        gmac_transfer_dl_normal_release_with_ul(gas_id);
      }
      else
      {
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,DONT_REL_RLC_DL,NO_DEL,FALSE,gas_id);
        mac_log_ul_tbf_release(MAC_LOG_UL_RLC_DL_RELEASE_IND,gas_id);
        mac_log_dl_tbf_release(MAC_LOG_DL_RELEASE_IND,gas_id);
      }
    }
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_tran_check_psho_addressing()
===
===  DESCRIPTION
===  Sends GRR PSHO msg if its correctly addressed.
===
===  DEPENDENCIES
===  ptr_csn_decode_dl_msg - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_tran_check_psho_addressing(gas_id_t gas_id)
{
  boolean addr_match = FALSE;

  mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  ps_handover_command_t *psho_data_ptr;
  psho_data_ptr = (ps_handover_command_t *)global_data_ptr->csn_dec_data_ptr;

  /* ADDRESS GLOBAL TFI, TLLI */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
  {
    mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);
    if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
    {
      if(psho_data_ptr->global_tfi_present == 0)
      {
        addr_match = gmac_util_global_tfi(psho_data_ptr->global_tfi, gas_id);
      }
    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
    {
      addr_match = TRUE;
    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
    {
      addr_match = FALSE;
    }
  }
  return(addr_match);
}

#endif /*FEATURE_GPRS_PS_HANDOVER*/
/*===========================================================================
===
===  FUNCTION    gmac_abnormal_release_procedure()
===
===  DESCRIPTION
===  Handles all abnormal release of UL & DL TBF.  This function handles both
===  MAC initiated abnormal releases and GRR (GRR_MAC_NO_PS_ACCESS_IND) releases.
===
===  PARAMS
===    mac_release_cause:  Indicates if GRR needs an abnormal release
===    release_rlc_dl:     Indicates if RLC DL should be released
===    rlc_del_ul_pdu:     Type of RLC UL PDU deletion used when blocking RLC
===    dtm_chan_rel        Indicates if the ab rel is from a DTM release.
===
===  RETURN VALUE
===    None
===
===  SIDE EFFECTS
===    c_flags.tbf_release_pending_flg write
===    mac_transfer_substate read
===    t_flags.abnormal_ul_and_dl_rel_flg write
===
===========================================================================*/

void gmac_abnormal_release_procedure
(
    rr_abnormal_release_cause_t  mac_release_cause,
    gmac_rlc_dl_rel_cause_t      release_rlc_dl,
    delete_pdu_cause_t           rlc_del_ul_pdu,
    boolean                      dtm_chan_rel,
    gas_id_t                     gas_id
)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  MSG_GERAN_HIGH_1_G("GMAC info AB REL (%d)",transfer_data_ptr->mac_transfer_substate);

  global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;

  switch(transfer_data_ptr->mac_transfer_substate)
  {
    case TRANSFER_INACTIVE:
      if(geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_0_G("MAC error not from T_INACTIVE");
        MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
        geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
      }
      else
      {
        ERR_GERAN_FATAL_0_G("MAC error not from T_INACTIVE");
      }
      break;

    case TRANSFER_DL:
    case TRANSFER_DL_REASSIGN:
      gmac_transfer_dl_release_clean(gas_id);
      if (release_rlc_dl == REL_RLC_DL)
      {
        gmac_util_send_dl_release_to_rlc(gas_id);
      }
      gmac_transfer_ul_tbf_abnormal_abort(rlc_del_ul_pdu,gas_id);

#ifdef FEATURE_GSM_DTM
      if ( dtm_chan_rel == FALSE )
      {
        gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL,gas_id);
      }
#else
      gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL,gas_id);
#endif /*FEATURE_GSM_DTM*/
      break;

    case TRANSFER_UL_DYNAMIC:
    case TRANSFER_UL_DYNAMIC_REALLOC:
      gmac_transfer_ul_dynamic_release_clean(gas_id);
      gmac_util_block_rlc(rlc_del_ul_pdu, GMAC_TRAN, __LINE__,gas_id);

#ifdef FEATURE_GSM_DTM
      if ( dtm_chan_rel == FALSE )
      {
        gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL,gas_id);
      }
#else
      gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL,gas_id);
#endif /*FEATURE_GSM_DTM*/
      break;

    case TRANSFER_UL_DYNAMIC_DL_ASSIGN:
    case TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN:
      gmac_transfer_dl_release_clean(gas_id);
      gmac_transfer_ul_dynamic_release_clean(gas_id);
      gmac_util_block_rlc(rlc_del_ul_pdu, GMAC_TRAN, __LINE__,gas_id);

#ifdef FEATURE_GSM_DTM
      if ( dtm_chan_rel == FALSE )
      {
        gmac_tran_send_mac_l1_release(UL_AND_DL_TBF_ABNORMAL,gas_id);
      }
#else
      gmac_tran_send_mac_l1_release(UL_AND_DL_TBF_ABNORMAL,gas_id);
#endif /*FEATURE_GSM_DTM*/

      transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg = TRUE;

      break;

    case TRANSFER_DL_WITH_DYNAMIC_ALLOC:
    case TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC:
    case TRANSFER_CONCURRENT_DYNAMIC_TRANSFER:
    case TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN:
    case TRANSFER_CONCURRENT_DYNAMIC_REALLOC:
    case TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC:
      gmac_transfer_dl_release_clean(gas_id);
      gmac_transfer_ul_dynamic_release_clean(gas_id);
      if (release_rlc_dl == REL_RLC_DL)
      {
        gmac_util_send_dl_release_to_rlc(gas_id);
      }
      gmac_util_block_rlc(rlc_del_ul_pdu, GMAC_TRAN, __LINE__,gas_id);

#ifdef FEATURE_GSM_DTM
      if ( dtm_chan_rel == FALSE )
      {
        gmac_tran_send_mac_l1_release(UL_AND_DL_TBF_ABNORMAL,gas_id);
      }
#else
      gmac_tran_send_mac_l1_release(UL_AND_DL_TBF_ABNORMAL,gas_id);
#endif /*FEATURE_GSM_DTM*/

      transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg = TRUE;

      break;

    default:
      if(geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_0_G("MAC error invalid T substate");
        MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
        geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
      }
      else
      {
        ERR_GERAN_FATAL_0_G("MAC error invalid T substate");
      }
      break;
  }

  /* In this case, cannot do any TBF until GRR has read more SI/PSI and therefore MAC will
  ** act as if a cell reselection is taking place when releasing.
  */
  if ( (mac_release_cause == RR_PART_SYS_INFO_REQ) ||
       (mac_release_cause == RR_CELL_RESEL_REQ) )
  {
    gmac_send_mac_grr_abnormal_release(mac_release_cause,gas_id);
    MSG_GERAN_HIGH_1_G("MAC info SYS INFO  %d",mac_release_cause);
  }

  MSG_GERAN_MED_1_G("MAC info abnormal_ul_and_dl_rel_flg =%d",transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg);
}


#ifdef FEATURE_GSM_DTM
/*===========================================================================
===
===  FUNCTION    gmac_dtm_chan_rel_handler()
===
===  DESCRIPTION
===  Handles the GRR_MAC_DTM_CHANNEL_RELEASE_IND signal.  If the PS is being released
===  then L1 is NOT commanded to release.  Instead the rel confirm handler is called here
===  to take MAC out of transfer.
===
===  PARAMS
===    dtm_ps_release:      Indicates whether the PS is being released
===    dtm_ps_rel_cause:    PS release cause, only valid if dtm_ps_release i TRUE
===    tbf_rel:             Indicates which TBF's are being release (UL,DL or UL+DL)
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_dtm_chan_rel_handler( gm_dtm_channel_release_ind_t *chan_rel_ptr,
                                       tbf_released_t     tbf_rel,
                                       gas_id_t           gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

#ifdef FEATURE_GSM_DTM
  global_data_ptr->gmac_dtm_cs_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
#endif /*FEATURE_GSM_DTM*/

  if ( chan_rel_ptr->dtm_ps_release == TRUE )
  {
    if ( ( chan_rel_ptr->dtm_ps_rel_cause == HANDOVER_REL ) ||
         ( chan_rel_ptr->dtm_ps_rel_cause == NORMAL_REL ) )
    {
      /* L1 is NOT released in the gmac_abnormal_release_procedure */
      gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,TRUE,gas_id);
      transfer_data_ptr->gmac_transfer_tbf_rel_cause = GMAC_TRANSFER_TBF_REL_GRR_NO_SERVICE;
    }
    else
    {
      if(geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_0_G("GMAC DTM Invalid dtm_ps_rel_cause");
        MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
        geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
      }
      else
      {
        ERR_GERAN_FATAL_0_G("GMAC DTM Invalid dtm_ps_rel_cause");
      }
    }

    /* Reset Transfer flags. There are many flags which are reset when the L1_MAC_TBF_REL_CONFIRM
       is received in transfer.  The following flags should be the total of all flags reset
       by L1_MAC_TBF_REL_CONFIRM (in all transfer states).
       There should be no harm in resetting them as we are guaranteed to exit transfer at this point */
    transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = FALSE;
    global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
    global_data_ptr->c_flags.tbf_release_pending_flg   = FALSE;
    transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl               = FALSE;
    global_data_ptr->c_flags.wait_for_ul_rel_confirm   = FALSE;
    transfer_data_ptr->t_flags.ul_tbf_normal_release_ind               = FALSE;
    transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul               = FALSE;
    transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending    = FALSE;
    transfer_data_ptr->t_flags.gmac_transfer_ul_req                    = FALSE;
    transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency  = FALSE;

    /* L1 is released by GRR for DTM.  MAC is not therefore expecting a L1_MAC_TBF_REL_CONFIRM.
       So it is necessary to simulate this by calling gmac_transfer_tbf_rel_confirm here*/
    gmac_transfer_tbf_rel_confirm(tbf_rel,gas_id);
  }
  else /* Enhanced CS release, PS remaining active*/
  {
#ifdef FEATURE_GSM_EDTM
    /* reset DTM flags */
    transfer_data_ptr->t_flags.dtm_active              = FALSE;
    transfer_data_ptr->t_flags.dtm_realloc_in_progress = FALSE;

    /* Reset DTM CS timseslot */
    global_data_ptr->gmac_dtm_cs_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;

    /* Set the current serving cell as it may have changed when in dual
       transfer mode */
#else
    if(geran_get_nv_recovery_restart_enabled(gas_id))
    {
      MSG_GERAN_ERROR_0_G("GMAC Error in DTM Chan Rel, EDTM NOT currently supported");
      MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
      geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
    }
    else
    {
      ERR_GERAN_FATAL_0_G("GMAC Error in DTM Chan Rel, EDTM NOT currently supported");
    }
#endif /*FEATURE_GSM_EDTM*/
  }
}

/*===========================================================================
===
===  FUNCTION    gmac_tran_dtm_chan_ass_handler()
===
===  DESCRIPTION
===    Handles the DTM Chan Assignment message in TRANSFER mode.
===    Checks that the assignment matches the TBF's currently active.
===    Calls the common DTM channel ass handler.
===
===  PARAMS
===    *dtm_ass_ptr                  - DTM assignment msg.
===    *mr_ul_alloc_cnf_sig_ptr      - MAC->RLC Config msg to be written to.
===    *mac_l1_dtm_chan_ass_req_ptr  - MAC->L1 DTM config msg to be written to.
===    *mac_l1_freq_config_ptr       - MAC->L1 freq config msg to be written to.
===     current_tbf_assigned         - Current active TBF's.
===
===
===  RETURN VALUE
===    dtm_ass_status_t
===
===  SIDE EFFECTS
===   sets t_flags.dtm_realloc_in_progress
===
===========================================================================*/
static dtm_ass_status_t gmac_tran_dtm_chan_ass_handler(
                           gm_dtm_channel_assign_req_t     *dtm_ass_ptr,
                           mr_ul_alloc_cnf_t               *mr_ul_alloc_cnf_sig_ptr,
                           dtm_tbf_type_t                   current_tbf_assigned,
                           gas_id_t                         gas_id )
{
  dtm_ass_status_t                ass_status = NO_DTM_ASS_ERROR;
  mac_l1_dtm_channel_assign_req_t mac_l1_dtm_chan_ass_req;
  mac_l1_freq_config_t            mac_l1_freq_config;
  uint8                           dtm_cs = 0;
  gmac_global_data_t             *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t           *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_timers_t                  *mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  /* MAC cannot handle a DTM Chan Ass when it is currently reallocating DTM */
  if ( transfer_data_ptr->t_flags.dtm_realloc_in_progress == TRUE )
  {
    MSG_GERAN_ERROR_0_G("GMAC received a DTM CHAN ASS when it is already reallocating");
    ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
  }

  if ( ass_status == NO_DTM_ASS_ERROR )
  {
    /* MAC should not receive a DTM Chan Ass when it is not in DTM */
    if ( transfer_data_ptr->t_flags.dtm_active != TRUE )
    {
#ifdef FEATURE_GSM_EDTM
      if ( global_data_ptr->gmac_edtm_enabled )
      {
        MSG_GERAN_MED_0_G("GMAC EDTM DTM Chan Ass rxd in transfer mode, CS establishing");
      }
      else
#endif /*FEATURE_GSM_EDTM*/
      {
        MSG_GERAN_ERROR_0_G("GMAC DTM not active, cannot handle DTM Chan Ass");
        ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
      }
    }
  }

  /* MAC will reject a DTM Chan Ass if it is currently releasing */
  if ( ass_status == NO_DTM_ASS_ERROR )
  {
    if ( transfer_data_ptr->t_flags.gmac_tran_rel_state != NO_REL )
    {
      ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
      MSG_GERAN_HIGH_1_G("GMAC DTM, DTM Chan Ass received whilst releasing: %d",transfer_data_ptr->t_flags.gmac_tran_rel_state);
    }
  }

  /* MAC will reject a DTM Chan Ass if the assigned TBF's don't match
     the currently active TBF's */
  if ( ass_status == NO_DTM_ASS_ERROR )
  {
#ifdef FEATURE_GSM_EDTM
    if ( global_data_ptr->gmac_edtm_enabled )
    {
      /*If EDTM is enabled(advertised to the NW) then it is ok to receive a DTM Ass Cmd with no PS*/

      /*Reset this flag for EDTM because if no TBFs are defined it will
        not be reset*/
      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = FALSE;
    }
    else
#endif /*FEATURE_GSM_EDTM*/
    {
      if ( dtm_ass_ptr->dtm_tbf_type != current_tbf_assigned )
      {
        ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
        MSG_GERAN_HIGH_2_G("GMAC DTM, TBF's do not match, dtm_tbf_type: 0x%d, curr_ass_tbf: 0x%x",
                           dtm_ass_ptr->dtm_tbf_type, current_tbf_assigned);
      }
    }
  }

#ifdef FEATURE_GSM_EDTM
  /* Check that T3168 is running if UL TBF is established from a DL state*/
  if ( global_data_ptr->gmac_edtm_enabled )
  {
    if ( dtm_ass_ptr->dtm_tbf_type == UL_TBF_PRESENT ||
         dtm_ass_ptr->dtm_tbf_type == UL_DL_TBF_PRESENT )
    {
      if ( current_tbf_assigned == DL_TBF_PRESENT )
      {
        if ( rex_get_timer(&mac_timers_ptr->timer_t3168) == 0 )
        {
          ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
          MSG_GERAN_HIGH_0_G("GMAC EDTM UL received when T3168 is not running");
        }
        else
        {
          gmac_stop_t3168(gas_id);
        }
      }
    }
  }
#endif /*FEATURE_GSM_EDTM*/


  /* Call general DTM ass handler */
  if ( ass_status == NO_DTM_ASS_ERROR)
  {
    ass_status = gmac_dtm_chan_ass_handler( dtm_ass_ptr,
                                            mr_ul_alloc_cnf_sig_ptr,
                                           &mac_l1_dtm_chan_ass_req,
                                           &mac_l1_freq_config,
                                            gas_id);
  }

  /* handle coding scheme change */
  if ( ass_status == NO_DTM_ASS_ERROR )
  {
    if ( ( dtm_ass_ptr->dtm_tbf_type == UL_TBF_PRESENT ) ||
         ( dtm_ass_ptr->dtm_tbf_type == UL_DL_TBF_PRESENT ) )
    {
      if ( dtm_ass_ptr->gm_dtm_ul_tbf_ass.egprs_flag == valid )
      {
        dtm_cs = dtm_ass_ptr->gm_dtm_ul_tbf_ass.egprs_mcs_mode;
      }
      else
      {
        dtm_cs = dtm_ass_ptr->gm_dtm_ul_tbf_ass.channel_coding_command;
      }

      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = FALSE;

      if ( global_data_ptr->gmac_local_cs_setting != dtm_cs )
      {
        transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = TRUE;
        transfer_data_ptr->pua_ptr_cs_change = dtm_cs;
      }
    }
  }


  /* Send MAC L1 Freq and DTM config messages if the ass status passed */
  if ( ass_status == NO_DTM_ASS_ERROR )
  {
    /* if the DTM Chan Ass does not contain a frequency assignment then there is
       no need to notify L1 as it is already in transfer */
    /* This behaviour is consistent with the existant GMAC code in transfer*/
    if ( dtm_ass_ptr->dtm_freq_present )
    {
      gmac_send_mac_l1_freq_config( &mac_l1_freq_config, gas_id );
    }

    gmac_send_mac_l1_dtm_channel_assign_req( &mac_l1_dtm_chan_ass_req, gas_id );

#ifdef FEATURE_GSM_EDTM
    if ( global_data_ptr->gmac_edtm_enabled )
    {
      transfer_data_ptr->t_flags.edtm_realloc_in_progress = TRUE;
    }
    else
#endif /*FEATURE_GSM_EDTM*/
    {
      transfer_data_ptr->t_flags.dtm_realloc_in_progress = TRUE;
    }
  }
  else /* failed so send a reject to GRR */
  {
    gmac_send_mac_grr_dtm_channel_assign_rej( ass_status, gas_id );
  }

  return ass_status;
}


/*===========================================================================
===
===  FUNCTION    gmac_tran_rel_state_handle_rel_cnf()
===
===  DESCRIPTION
===    Updates the GMAC rel state for the TBF Release Confirm.
===
===  PARAMS
===    tbf_released - Indication of which TBF is released by L1
===
===  RETURN VALUE
===    NONE
===
===
===  SIDE EFFECTS
===    sets t_flags.gmac_tran_rel_state
===
===
===========================================================================*/
static void gmac_tran_rel_state_handle_rel_cnf( tbf_released_t tbf_released,
                                                gas_id_t gas_id )
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  MSG_GERAN_MED_2_G("GMAC Rel State, REL CNF, tbf_rel:0x%x, original rel state:0x%x",
                    tbf_released,transfer_data_ptr->t_flags.gmac_tran_rel_state);

  switch ( transfer_data_ptr->t_flags.gmac_tran_rel_state )
  {
    case NO_REL:
      /* no change in state */
      MSG_GERAN_HIGH_0_G("GMAC Rel State, BAD TRANSITION in NO_REL state");
    break;

    case UL_NORM:
      switch ( tbf_released )
      {
        case DL_TBF_RELEASE_CONF:
          /* no change in state */
          MSG_GERAN_HIGH_0_G("GMAC Rel State, BAD TRANSITION, DL_TBF_RELEASE_CONF rcvd in UL_NORM state");
        break;

        case UL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        case UL_DL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
          MSG_GERAN_HIGH_0_G("GMAC Rel State, BAD TRANSITION, UL_DL_TBF_RELEASE_CONF rcvd in UL_NORM state");
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_released value:0x%x",tbf_released);
        break;
      }
    break;

    case DL_NORM:
      switch ( tbf_released )
      {
        case DL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        case UL_TBF_RELEASE_CONF:
          /* no change in state */
          MSG_GERAN_HIGH_0_G("GMAC Rel State, BAD TRANSITION, UL_TBF_RELEASE_CONF rcvd in DL_NORM state");
        break;

        case UL_DL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
          MSG_GERAN_HIGH_0_G("GMAC Rel State, BAD TRANSITION, UL_DL_TBF_RELEASE_CONF rcvd in DL_NORM state");
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_released value:0x%x",tbf_released);
        break;
      }
    break;

    case UL_AB:
      switch ( tbf_released )
      {
        case DL_TBF_RELEASE_CONF:
          /* no change in state */
          MSG_GERAN_HIGH_0_G("GMAC Rel State, BAD TRANSITION, DL_TBF_RELEASE_CONF rcvd in UL_AB state");
        break;

        case UL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        case UL_DL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
          MSG_GERAN_HIGH_0_G("GMAC Rel State, BAD TRANSITION, UL_DL_TBF_RELEASE_CONF rcvd in UL_AB state");
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_released value:0x%x",tbf_released);
        break;
      }
    break;

    case DL_AB:
      switch ( tbf_released )
      {
        case DL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        case UL_TBF_RELEASE_CONF:
          /* no change in state */
          MSG_GERAN_HIGH_0_G("GMAC Rel State, BAD TRANSITION, UL_TBF_RELEASE_CONF rcvd in DL_AB state");
        break;

        case UL_DL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
          MSG_GERAN_HIGH_0_G("GMAC Rel State, BAD TRANSITION, UL_DL_TBF_RELEASE_CONF rcvd in DL_AB state");
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_released value:0x%x",tbf_released);
        break;
      }
    break;

    case UL_DL_NORM:
      switch ( tbf_released )
      {
        case DL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_NORM;
        break;

        case UL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = DL_NORM;
        break;

        case UL_DL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_released value:0x%x",tbf_released);
        break;
      }
    break;

    case UL_DL_AB:
      switch ( tbf_released )
      {
        case DL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_AB;
        break;

        case UL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = DL_AB;
        break;

        case UL_DL_TBF_RELEASE_CONF:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_released value:0x%x",tbf_released);
        break;
      }
    break;

    default:
      MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD gmac_tran_rel_state value:0x%x",transfer_data_ptr->t_flags.gmac_tran_rel_state);
    break;
  }

  MSG_GERAN_MED_1_G("GMAC Rel State, rel state changed to:0x%x",transfer_data_ptr->t_flags.gmac_tran_rel_state);
}

/*===========================================================================
===
===  FUNCTION    gmac_tran_rel_state_handle_connect_ind()
===
===  DESCRIPTION
===    Updates the GMAC rel state for the PH_CONNECT_IND signal.
===
===  PARAMS
===    tbf_type_connected - Indication of which TBF was connected by L1
===
===  RETURN VALUE
===    NONE
===
===
===  SIDE EFFECTS
===    sets t_flags.gmac_tran_rel_state
===
===
===========================================================================*/
static void gmac_tran_rel_state_handle_connect_ind( uint8 tbf_type_connected,
                                                    gas_id_t gas_id )
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  MSG_GERAN_MED_2_G("GMAC Rel State, CONNECT_IND, tbf_type_connected:0x%x, original rel state:0x%x",
                    tbf_type_connected,transfer_data_ptr->t_flags.gmac_tran_rel_state);

  switch ( transfer_data_ptr->t_flags.gmac_tran_rel_state )
  {
    case NO_REL:
      /* No state transition */
    break;

    case UL_NORM:
      switch ( tbf_type_connected )
      {
        case UPLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        case DOWNLINK_TBF_ESTABLISHED:
          /* No state transition */
        break;

        case UPLINK_DOWNLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_type_connected value:0x%x",tbf_type_connected);
        break;
      }
    break;

    case DL_NORM:
      switch ( tbf_type_connected )
      {
        case UPLINK_TBF_ESTABLISHED:
          /* No state transition */
        break;

        case DOWNLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        case UPLINK_DOWNLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_type_connected value:0x%x",tbf_type_connected);
        break;
      }
    break;

    case UL_AB:
      switch ( tbf_type_connected )
      {
        case UPLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        case DOWNLINK_TBF_ESTABLISHED:
          /* No state transition */
        break;

        case UPLINK_DOWNLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_type_connected value:0x%x",tbf_type_connected);
        break;
      }
    break;

    case DL_AB:
      switch ( tbf_type_connected )
      {
        case UPLINK_TBF_ESTABLISHED:
          /* No state transition */
        break;

        case DOWNLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        case UPLINK_DOWNLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_type_connected value:0x%x",tbf_type_connected);
        break;
      }
    break;

    case UL_DL_NORM:
      switch ( tbf_type_connected )
      {
        case UPLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = DL_NORM;
        break;

        case DOWNLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_NORM;
        break;

        case UPLINK_DOWNLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_type_connected value:0x%x",tbf_type_connected);
        break;
      }
    break;

    case UL_DL_AB:
      switch ( tbf_type_connected )
      {
        case UPLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = DL_AB;
        break;

        case DOWNLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_AB;
        break;

        case UPLINK_DOWNLINK_TBF_ESTABLISHED:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = NO_REL;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_type_connected value:0x%x",tbf_type_connected);
        break;
      }
    break;

   default:
     MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD gmac_tran_rel_state value:0x%x",transfer_data_ptr->t_flags.gmac_tran_rel_state);
   break;
  }

  MSG_GERAN_MED_1_G("GMAC Rel State, CONNECT_IND, rel state changed to:0x%x",transfer_data_ptr->t_flags.gmac_tran_rel_state);
}


/*===========================================================================
===
===  FUNCTION    gmac_tran_rel_state_handle_rel_req()
===
===  DESCRIPTION
===    Updates the GMAC rel state for the MAC_L1_RELEASE signal.
===
===  PARAMS
===    tbf_rel_type - Indication of which TBF is being released
===
===  RETURN VALUE
===    NONE
===
===  SIDE EFFECTS
===    sets t_flags.gmac_tran_rel_state
===
===
===========================================================================*/
void gmac_tran_rel_state_handle_rel_req( release_tbf_t tbf_rel_type,
                                         gas_id_t gas_id )
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  MSG_GERAN_MED_2_G("GMAC Rel State, MAC_L1_RELEASE, tbf_type_rel:0x%x, original rel state:0x%x",
                    tbf_rel_type,transfer_data_ptr->t_flags.gmac_tran_rel_state);

  switch ( transfer_data_ptr->t_flags.gmac_tran_rel_state )
  {
    case NO_REL:
      switch ( tbf_rel_type )
      {
        case UL_TBF_NORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_NORM;
        break;

        case DL_TBF_NORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = DL_NORM;
        break;

        case UL_AND_DL_TBF_NORMAL:
          MSG_GERAN_ERROR_0_G("GMAC Rel State, MAC_L1_RELEASE, UL_AND_DL_TBF_NORMAL should not be used");
        break;

        case UL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_AB;
        break;

        case DL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = DL_AB;
        break;

        case UL_AND_DL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_rel_type value:0x%x",tbf_rel_type);
        break;
      }
    break;

    case UL_NORM:
      switch ( tbf_rel_type )
      {
        case UL_TBF_NORMAL:
          /* no state transition*/
        break;

        case DL_TBF_NORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_NORM;
        break;

        case UL_AND_DL_TBF_NORMAL:
          MSG_GERAN_ERROR_0_G("GMAC Rel State, MAC_L1_RELEASE, UL_AND_DL_TBF_NORMAL should not be used");
        break;

        case UL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_AB;
        break;

        case DL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        case UL_AND_DL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_rel_type value:0x%x",tbf_rel_type);
        break;
      }
    break;

    case DL_NORM:
      switch ( tbf_rel_type )
      {
        case UL_TBF_NORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_NORM;
        break;

        case DL_TBF_NORMAL:
          /* no state transition*/
        break;

        case UL_AND_DL_TBF_NORMAL:
          MSG_GERAN_ERROR_0_G("GMAC Rel State, MAC_L1_RELEASE, UL_AND_DL_TBF_NORMAL should not be used");
        break;

        case UL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        case DL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = DL_AB;
        break;

        case UL_AND_DL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_rel_type value:0x%x",tbf_rel_type);
        break;
      }
    break;

    case UL_AB:
      switch ( tbf_rel_type )
      {
        case UL_TBF_NORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_AB;
        break;

        case DL_TBF_NORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        case UL_AND_DL_TBF_NORMAL:
          MSG_GERAN_ERROR_0_G("GMAC Rel State, MAC_L1_RELEASE, UL_AND_DL_TBF_NORMAL should not be used");
        break;

        case UL_TBF_ABNORMAL:
          /* no state transition*/
        break;

        case DL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        case UL_AND_DL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_rel_type value:0x%x",tbf_rel_type);
        break;
      }
    break;

    case DL_AB:
      switch ( tbf_rel_type )
      {
        case UL_TBF_NORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        case DL_TBF_NORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = DL_AB;
        break;

        case UL_AND_DL_TBF_NORMAL:
          MSG_GERAN_ERROR_0_G("GMAC Rel State, MAC_L1_RELEASE, UL_AND_DL_TBF_NORMAL should not be used");
        break;

        case UL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        case DL_TBF_ABNORMAL:
          /* no state transition*/
        break;

        case UL_AND_DL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_rel_type value:0x%x",tbf_rel_type);
        break;
      }
    break;

    case UL_DL_NORM:
      switch ( tbf_rel_type )
      {
        case UL_TBF_NORMAL:
          /* no state transition*/
        break;

        case DL_TBF_NORMAL:
          /* no state transition*/
        break;

        case UL_AND_DL_TBF_NORMAL:
          MSG_GERAN_ERROR_0_G("GMAC Rel State, MAC_L1_RELEASE, UL_AND_DL_TBF_NORMAL should not be used");
        break;

        case UL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        case DL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        case UL_AND_DL_TBF_ABNORMAL:
          transfer_data_ptr->t_flags.gmac_tran_rel_state = UL_DL_AB;
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_rel_type value:0x%x",tbf_rel_type);
        break;
      }
    break;

    case UL_DL_AB:
      switch ( tbf_rel_type )
      {
        case UL_TBF_NORMAL:
          /* no state transition*/
        break;

        case DL_TBF_NORMAL:
          /* no state transition*/
        break;

        case UL_AND_DL_TBF_NORMAL:
          MSG_GERAN_ERROR_0_G("GMAC Rel State, MAC_L1_RELEASE, UL_AND_DL_TBF_NORMAL should not be used");
        break;

        case UL_TBF_ABNORMAL:
          /* no state transition*/
        break;

        case DL_TBF_ABNORMAL:
          /* no state transition*/
        break;

        case UL_AND_DL_TBF_ABNORMAL:
          /* no state transition*/
        break;

        default:
          MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD tbf_rel_type value:0x%x",tbf_rel_type);
        break;
      }
    break;

    default:
      MSG_GERAN_ERROR_1_G("GMAC Rel State, BAD gmac_tran_rel_state value:0x%x",transfer_data_ptr->t_flags.gmac_tran_rel_state);
    break;

  }

  MSG_GERAN_MED_1_G("GMAC Rel State, MAC_L1_RELEASE, rel state changed to:0x%x",transfer_data_ptr->t_flags.gmac_tran_rel_state);

}


#endif /* FEATURE_GSM_DTM */


/*===========================================================================
===
===  FUNCTION      gmac_tran_pcco_handler()
===
===  DESCRIPTION
===  Sends GRR PCCO msg if its correctly addressed.
===
===  DEPENDENCIES
===  ptr_csn_decode_dl_msg - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_tran_pcco_handler(gprs_mac_transfer_substates_t mac_tran_substate,
                            gas_id_t gas_id)
{
  boolean addr_match = FALSE;

  mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  packet_cell_change_order_t *pcco_data_ptr;
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  pcco_data_ptr = (packet_cell_change_order_t *)global_data_ptr->csn_dec_data_ptr;

  NOTUSED(mac_tran_substate);

  /* ADDRESS GLOBAL TFI, TLLI */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
  {

    mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);
    if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
    {

        switch (pcco_data_ptr->tlli_gtfi_pkt_ref_valid)
        {
          case tlli_valid:
            addr_match = gmac_match_old_or_new_tlli(pcco_data_ptr->tlli, gas_id);
            break;

          case global_tfi_valid:
            /* if stored TFI = received TFI then match else ignore message */
            addr_match = gmac_util_global_tfi(pcco_data_ptr->global_tfi, gas_id);
            break;

          default:
            break;
        }

    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
    {
      addr_match = TRUE;
    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
    {
      addr_match = FALSE;
    }
  }

  if (addr_match)
  {
#ifdef FEATURE_GSM_DTM
    if ( transfer_data_ptr->t_flags.dtm_active )
    {
      MSG_GERAN_HIGH_0_G("GMAC ignored PCCO because MS is in DTM");
      return;
    }
#endif /*FEATURE_GSM_DTM*/

    MSG_GERAN_MED_0_G("MAC Rcvd PACKET_CELL_CHANGE_ORDER");
    gmac_send_data_ind_to_grr(gas_id);
    rrbp_control_block_handler_2(PCA_DL_MSG, gas_id);
  }

}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_send_freq_cnf_msgs
===
===  DESCRIPTION
===
===  Called when simulating frequency confirm.  Sends L1 and RLC config messages.
===
===  DEPENDENCIES
===  gmac_transfer_store_l1_dyn_realloc - Uses this global to send a message.
===  t_flags.gmac_transfer_dl_fai_set - Sets this global flag.
===
===  PARAMETERS
===  rlc_ass_msg - Configure which RLC message is to be sent.
===  l1_cnf_msg  - Configure which L1 message is to be sent.
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_transfer_send_freq_cnf_msgs( gmac_freq_cnf_rlc_ass_msg_t rlc_ass_msg,
                                              gmac_freq_cnf_l1_cnf_msg_t  l1_cnf_msg,
                                              gas_id_t gas_id )
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  switch (l1_cnf_msg)
  {
    case GMAC_SEND_TS_RECONF_TO_L1:
      gmac_transfer_send_stored_ts_reconfig_to_l1(gas_id);
    break;

    case GMAC_SEND_DL_CONF_TO_L1:
      gmac_transfer_send_stored_dl_config_to_l1(gas_id);
    break;

    case GMAC_SEND_DYN_CONF_TO_L1:
      /* send MAC_L1_DYNAMIC_TBF_CONFIG with parameters */
      MSG_GERAN_MED_0_G("MAC Sent MAC_L1_DYNAMIC_TBF_CONFIG");
      transfer_data_ptr->gmac_transfer_store_l1_dyn_realloc.mac_l1_dynamic_tbf_config.message_header.message_set = MS_MAC_L1;
      transfer_data_ptr->gmac_transfer_store_l1_dyn_realloc.mac_l1_dynamic_tbf_config.message_header.message_id = (uint8)MAC_L1_DYNAMIC_TBF_CONFIG;
      gprs_l1_mac_acc_func(&transfer_data_ptr->gmac_transfer_store_l1_dyn_realloc,gas_id);
    break;

    default:
      MSG_GERAN_ERROR_1_G("GMAC INFO BAD l1_cnf_msg: 0x%x",l1_cnf_msg);
    break;
  }

  switch (rlc_ass_msg)
  {
    case GMAC_NO_RLC_ASS_MSG:
      /*do nothing*/
    break;

    case GMAC_SEND_DL_ASS_TO_RLC:
      if (transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set)
      {
        gmac_util_send_dl_ass_to_rlc(gas_id);
        transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = FALSE;
      }
    break;

    default:
      MSG_GERAN_ERROR_1_G("GMAC INFO BAD rlc_ass_msg: 0x%x",rlc_ass_msg);
    break;
  }
}


/*===========================================================================
===
===  FUNCTION      gmac_transfer_simulate_freq_cnf
===
===  DESCRIPTION
===
===  Called when simulating frequency confirm.  Determines which config messages
===  should be sent depending on which transfer state MAC is in.
===
===  DEPENDENCIES
===  mac_transfer_substate - Determines which config messages should be sent.
===
===  PARAMETERS
===    none
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_simulate_freq_cnf( gas_id_t gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  switch ( transfer_data_ptr->mac_transfer_substate )
  {
    case TRANSFER_DL_REASSIGN:
    case TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN:
      gmac_transfer_send_freq_cnf_msgs(GMAC_SEND_DL_ASS_TO_RLC, GMAC_SEND_DL_CONF_TO_L1,gas_id);
    break;

    case TRANSFER_UL_DYNAMIC_DL_ASSIGN:
      gmac_transfer_send_freq_cnf_msgs(GMAC_NO_RLC_ASS_MSG, GMAC_SEND_DL_CONF_TO_L1,gas_id);
    break;

    case TRANSFER_DL_WITH_DYNAMIC_ALLOC:
      if (global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_UPLINK_ASSIGNMENT)
      {
        gmac_transfer_send_freq_cnf_msgs(GMAC_NO_RLC_ASS_MSG, GMAC_SEND_DYN_CONF_TO_L1,gas_id);
      }
      else if (global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_DOWNLINK_ASSIGNMENT)
      {
        gmac_transfer_send_freq_cnf_msgs(GMAC_SEND_DL_ASS_TO_RLC, GMAC_SEND_DL_CONF_TO_L1,gas_id);
      }
      else
      {
        if(geran_get_nv_recovery_restart_enabled(gas_id))
        {
          MSG_GERAN_ERROR_2_G("GMAC unknown msg_type: 0x%x in transfer substate: 0x%x",
                   global_data_ptr->csn_dec_hdr_ptr->message_type,transfer_data_ptr->mac_transfer_substate);
          MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
          geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
        }
        else
        {
          ERR_GERAN_FATAL_3_G("GMAC unknown msg_type: 0x%x in transfer substate: 0x%x",gas_id, global_data_ptr->csn_dec_hdr_ptr->message_type,transfer_data_ptr->mac_transfer_substate);
        }
      }
    break;

    case TRANSFER_UL_DYNAMIC_REALLOC:
    case TRANSFER_CONCURRENT_DYNAMIC_REALLOC:
      gmac_transfer_send_freq_cnf_msgs(GMAC_NO_RLC_ASS_MSG, GMAC_SEND_DYN_CONF_TO_L1,gas_id);
    break;

    case TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC:
      gmac_transfer_send_freq_cnf_msgs(GMAC_SEND_DL_ASS_TO_RLC, GMAC_SEND_TS_RECONF_TO_L1,gas_id);
    break;

case TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC:
      /*changes introduced in cr697201 to handle PDA in TRANSFER_CONCURRENT_DYNAMIC_REALLOC which results */
      /* in a transition to TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC. IF PDA has no frequency      */
      /* parameters than sending of MAC_L1_DL_TBF_CONFIG is done inline while processing the PDA, if freq */
      /* params are present then the config is deferred to after freq config has been sent to L1.hence the*/
      /* need to determine OTA msg being processed here in order to send the correct config to L1.        */ 
           
      if (global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_DOWNLINK_ASSIGNMENT)
      {
        gmac_transfer_send_freq_cnf_msgs(GMAC_SEND_DL_ASS_TO_RLC, GMAC_SEND_DL_CONF_TO_L1,gas_id);
      }
      else if (global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_TIMESLOT_RECONFIGURE)
      {
        gmac_transfer_send_freq_cnf_msgs(GMAC_SEND_DL_ASS_TO_RLC, GMAC_SEND_TS_RECONF_TO_L1,gas_id);
      }
      else
      {
        if(geran_get_nv_recovery_restart_enabled(gas_id))
        {
          MSG_GERAN_ERROR_2_G("GMAC unknown msg_type: 0x%x in transfer substate: 0x%x",
                   global_data_ptr->csn_dec_hdr_ptr->message_type,transfer_data_ptr->mac_transfer_substate);
          MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
          geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
        }
        else
        {
          ERR_FATAL("ds%d: GMAC unknown msg_type: 0x%x in transfer substate: 0x%x",gas_id, global_data_ptr->csn_dec_hdr_ptr->message_type,transfer_data_ptr->mac_transfer_substate);
        }
      }

    break;

    case TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN:
      gmac_transfer_send_freq_cnf_msgs(GMAC_NO_RLC_ASS_MSG, GMAC_SEND_TS_RECONF_TO_L1,gas_id);
    break;

    case TRANSFER_INACTIVE:
    case TRANSFER_DL:
    case TRANSFER_UL_DYNAMIC:
    case TRANSFER_CONCURRENT_DYNAMIC_TRANSFER:
      if(geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_1_G("GMAC simulate freq cnf called in wrong state: 0x%x",
                  transfer_data_ptr->mac_transfer_substate);
        MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
        geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
      }
      else
      {
        ERR_GERAN_FATAL_2_G("GMAC simulate freq cnf called in wrong state: 0x%x",gas_id, transfer_data_ptr->mac_transfer_substate);
      }
    break;

    default:
      if(geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_1_G("GMAC unknown mac transfer substate: 0x%x",transfer_data_ptr->mac_transfer_substate);
        MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
        geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
      }
      else
      {
        ERR_GERAN_FATAL_2_G("GMAC unknown mac transfer substate: 0x%x",gas_id, transfer_data_ptr->mac_transfer_substate);
      }
    break;
  }
}

#ifdef FEATURE_GSM_EDTM
/*===========================================================================
===
===  FUNCTION    gmac_transfer_edtm_ph_cs_connect_handler()
===
===  DESCRIPTION
===
===  Handles the MAC_PH_CS_CONNECT_IND signal
===
===  DEPENDENCIES
===    gmac_edtm_enabled
===    t_flags.dtm_active
===    t_flags.edtm_realloc_in_progress
===
===  RETURN VALUE
===    NONE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_transfer_edtm_ph_cs_connect_handler( gas_id_t gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  if (global_data_ptr->gmac_edtm_enabled)
  {
    transfer_data_ptr->t_flags.dtm_active = TRUE;
    transfer_data_ptr->t_flags.edtm_realloc_in_progress = FALSE;
  }
}

/*===========================================================================
===
===  FUNCTION    gmac_transfer_edtm_t3196_expiry_handler()
===
===  DESCRIPTION
===
===  Handles EDTM T3196 expiry.  Sends a reject signal to RR.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    NONE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_transfer_edtm_t3196_expiry_handler( gas_id_t gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /* reset pending edtm pkt cs req on RRBP */
  memset(&global_data_ptr->gmac_local_edtm_pcr_params,0,sizeof(gmac_edtm_pcr_params_t));

  /* Ensure that all pending Pkt CS Req messages are deleted from L1's UL PACCH and RRBP queues*/
  gmac_send_mac_l1_delete_ul_ctrl_msg(PACKET_CS_REQUEST, gas_id);

  gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_T3196_EXPIRY, gas_id);
}


/*===========================================================================
===
===  FUNCTION    gmac_transfer_edtm_pkt_cs_req_handler()
===
===  DESCRIPTION
===
===  Format EDTM PACKET CS REQUEST message.  If the PACCH for an UL TBF is
===  available send the message in the MAC_PH_DATA_REQ signal.  Overwise indicate
===  that the Packet CS Req is pending on the PACCH for the DL TBF.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    NONE
===
===  SIDE EFFECTS
===    Read and write gmac_local_edtm_pcr_params.
===    Read c_flags.mac_ctrl_header_r_bit.
===
===
===========================================================================*/
static void gmac_transfer_edtm_pkt_cs_req_handler(active_tbf_t                active_tbf,
                                                  uint8                       rr_est_cause,
                                                  mac_edtm_pkt_cs_req_retry_t cs_req_retry,
                                                  gas_id_t                    gas_id)
{
  uint8                         pkt_cs_req_encoding = FAIL;
  boolean                       send_edtm_pcr = TRUE;
  mac_grr_edtm_cs_rej_cause_t   edtm_pcr_rej_cause = EDTM_CS_REJ_CAUSE_UNSPECIFIED;
  gmac_global_data_t           *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t         *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  /* Check that conditions are right for EDTM Packet CS Request to be sent */
  if (active_tbf == DL_ONLY)
  {
    /* Check DL is not releasing */
    if ( transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set == TRUE ||
         transfer_data_ptr->t_flags.gmac_tran_rel_state == DL_NORM ||
         transfer_data_ptr->t_flags.gmac_tran_rel_state == DL_AB )
    {
      send_edtm_pcr = FALSE;
      edtm_pcr_rej_cause = EDTM_CS_REJ_TBF_RELEASE_IN_PROGRESS;
      MSG_GERAN_MED_0_G("GMAC EDTM DL releasing can't send Pkt CS Req on DL");
    }
  }
  else /*UL or UL_DL */
  {
    /* Check if UL is releasing */
    if ( transfer_data_ptr->t_flags.gmac_tran_rel_state == UL_NORM ||
         transfer_data_ptr->t_flags.gmac_tran_rel_state == UL_AB ||
         transfer_data_ptr->t_flags.gmac_tran_rel_state == UL_DL_NORM ||
         transfer_data_ptr->t_flags.gmac_tran_rel_state == UL_DL_AB )
    {
      send_edtm_pcr = FALSE;
      edtm_pcr_rej_cause = EDTM_CS_REJ_TBF_RELEASE_IN_PROGRESS;
      MSG_GERAN_MED_0_G("GMAC EDTM UL releasing can't send Pkt CS Req on UL");
    }
    /* if UL is not releasing check CV is not active */
    else if ( grlc_ul_is_cv_active(gas_id) == TRUE )
    {
      send_edtm_pcr = FALSE;
      edtm_pcr_rej_cause = EDTM_CS_REJ_COUNTDOWN_IN_PROGRESS;
      MSG_GERAN_MED_0_G("GMAC EDTM UL counting down can't send Pkt CS Req on UL");
    }

    /* if unable to send on UL attempt to send on DL */
    if ( active_tbf    == UL_AND_DL &&
         send_edtm_pcr == FALSE )
    {
      if ( transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set == FALSE &&
           transfer_data_ptr->t_flags.gmac_tran_rel_state != DL_NORM &&
           transfer_data_ptr->t_flags.gmac_tran_rel_state != DL_AB &&
           transfer_data_ptr->t_flags.gmac_tran_rel_state != UL_DL_NORM &&
           transfer_data_ptr->t_flags.gmac_tran_rel_state != UL_DL_AB )
      {
        /* DL is not releasing so switch to DL */
        send_edtm_pcr = TRUE;
        active_tbf = DL_ONLY;
        MSG_GERAN_MED_0_G("GMAC EDTM Pkt CS Req OK to send on DL RRBP");
      }
    }
  }

  if (send_edtm_pcr)
  {
    if (cs_req_retry == EDTM_FIRST_PKT_CS_REQUEST)
    {
      /*Do not start the timers when doing a CS request retry*/
      gmac_start_t3196(gas_id);
      gmac_start_edtm_pkt_cs_req_retry_timer(gas_id);
      global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause = rr_est_cause;
    }
    else if (cs_req_retry == EDTM_PKT_CS_REQ_FROM_CONT_RES)
    {
      /* T3196 was already started in Idle Contention Res */
      gmac_start_edtm_pkt_cs_req_retry_timer(gas_id);
    }

    if(active_tbf == DL_ONLY)
    {
      pkt_cs_req_encoding = gmac_util_encode_ul_control_msg_handler(PCR,TRUE,gas_id);
    }
    else
    {
      pkt_cs_req_encoding = gmac_util_encode_ul_control_msg_handler(PCR,FALSE,gas_id);
    }


    if ( active_tbf == UL_ONLY ||
         active_tbf == UL_AND_DL )
    {
      if (pkt_cs_req_encoding == PASS)
      {
        gmac_util_send_ul_control_msg_handler(PCR, UL_PRESENT, GRR_CNF_NEEDED, gas_id);
      }
      else
      {
        if(geran_get_nv_recovery_restart_enabled(gas_id))
        {
          MSG_GERAN_ERROR_0_G("MAC error CSN.1 encode EDTM PACKET CS REQ during Transfer");
          MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
          geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("MAC error CSN.1 encode EDTM PACKET CS REQ during Transfer");
        }
      }
    }
    else
    {
      MSG_GERAN_MED_1_G("GMAC EDTM pend Packet CS Request for RRBP, rr_est_cause: 0x%x",rr_est_cause);
      gmac_util_send_ul_control_msg_handler(PCR, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
    }
  }
  else
  {
    gmac_send_mac_grr_edtm_pkt_cs_est_rej(edtm_pcr_rej_cause, gas_id);
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_edtm_pkt_cs_cmd_handler()
===
===  DESCRIPTION
===  Sends GRR Packet CS Command msg if it's correctly addressed.
===
===  DEPENDENCIES
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_transfer_edtm_pkt_cs_cmd_handler(gas_id_t gas_id)
{
  boolean addr_match = FALSE;

  mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_timers_t *mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  packet_cs_command_t *pkt_cs_cmd_ptr;
  pkt_cs_cmd_ptr = (packet_cs_command_t *)global_data_ptr->csn_dec_data_ptr;

  /* ADDRESS GLOBAL TFI, TLLI */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
  {
    mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);

    if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
    {
      addr_match = gmac_util_global_tfi(pkt_cs_cmd_ptr->global_tfi, gas_id);
    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
    {
      addr_match = TRUE;
    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
    {
      addr_match = FALSE;
    }
  }

  if (addr_match)
  {
    MSG_GERAN_MED_0_G("MAC Rcvd EDTM PACKET CS COMMAND");
    gmac_send_data_ind_to_grr(gas_id);
    rrbp_control_block_handler_2(PCA_DL_MSG, gas_id);

    if( rex_get_timer(&mac_timers_ptr->timer_t3196) != 0 )
    {
      gmac_stop_t3196(gas_id);
      gmac_stop_edtm_pkt_cs_req_retry_timer(gas_id);

      /*Pkt CS Command has been received, ensure that all pending Pkt CS Req
        messages are deleted from L1's UL PACCH and RRBP queues*/
      gmac_send_mac_l1_delete_ul_ctrl_msg(PACKET_CS_REQUEST, gas_id);

      /* reset pending edtm pkt cs req on RRBP */
      memset(&global_data_ptr->gmac_local_edtm_pcr_params,0,sizeof(gmac_edtm_pcr_params_t));
    }
  }
}


/*===========================================================================
===
===  FUNCTION      gmac_transfer_edtm_clear_dl_tbf()
===
===  DESCRIPTION
===  Clears a DL TBF by resetting the local DL TBF parameters and sending
===  a release to RLC.  This function is used when implicitly releasing
===  a DL TBF in a DTM Ass Command received when EDTM is active.  Note
===  that no L1 MAC TBF release is required to be sent, and therefore no
===  MAC L1 TBF Release confirm is expected.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void gmac_transfer_edtm_clear_dl_tbf( mac_edtm_tbf_clear_action_t tbf_action,
                                             gas_id_t gas_id )
{
  MSG_GERAN_MED_1_G("GMAC EDTM Clear DL TBF, tbf_action: 0x%x",tbf_action);

  gmac_transfer_dl_release_clean(gas_id);

  if ( tbf_action == EDTM_CLEAR_ACTIVE_TBF )
  {
    gmac_util_send_dl_release_to_rlc(gas_id);
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_edtm_clear_ul_tbf()
===
===  DESCRIPTION
===  Clears a UL TBF by resetting the local UL TBF parameters and sending
===  a release to RLC.  This function is used when implicitly releasing
===  an UL TBF in a DTM Ass Command received when EDTM is active.  Note
===  that no L1 MAC TBF release is required to be sent, and therefore no
===  MAC L1 TBF Release confirm is expected.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void gmac_transfer_edtm_clear_ul_tbf( mac_edtm_tbf_clear_action_t tbf_action,
                                             gas_id_t                    gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  MSG_GERAN_MED_1_G("GMAC EDTM Clear UL TBF, tbf_action:0x%x",tbf_action);

  switch ( tbf_action )
  {
    case EDTM_CLEAR_PENDING_TBF:
      /* The outstanding request for UL TBF establishment is not cleared for EDTM.
         The MS waits on the new channels for the UL assignment */
      global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
      transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;
    break;

    case EDTM_CLEAR_ACTIVE_TBF:
      gmac_transfer_ul_dynamic_release_clean(gas_id);
      /* Where DL_TBF is present in DTM Ass Cmd then MAC will move to DL_REASS.  RLC
         will be unblocked when the DL connects.

         Where No TBF is present in DTM Ass Cmd then MAC will move to T_DL.  RLC will
         be unblocked after this function is called for T_DL_WITH_DYN_ALLOC and
         T_DL_REASS_DYN_ALLOC states */
      gmac_util_block_rlc(NO_DEL, GMAC_TRAN, __LINE__, gas_id);
    break;

    default:
      if(geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_1_G("GMAC bad tbf_action: 0x%x",tbf_action);
        MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
        geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
      }
      else
      {
        ERR_GERAN_FATAL_2_G("GMAC bad tbf_action: 0x%x",gas_id, tbf_action);
      }
    break;
  }
}


/*===========================================================================
===
===  FUNCTION      gmac_transfer_edtm_restore_tbf_data()
===
===  DESCRIPTION
===  Restores the TBF data.  If MAC is pending an assignment when the DTM
===  Ass Command is received (with no PS defined) then the current tbf data will remain
===  set to the value in the pending assignment.  This is because the DTM Ass Command does
===  not contain any PS and therefore won't set the TBF data.   We need to restore the value prior
===  to the pending assignment.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void gmac_transfer_edtm_restore_tbf_data( active_tbf_t restore_tbf_type,
                                                 gas_id_t     gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /* Current only window size requires restoring */

  switch ( restore_tbf_type )
  {
    case UL_ONLY:
      global_data_ptr->current_egprs_ul_ws = global_data_ptr->gmac_edtm_old_ul_ws;
    break;

    case DL_ONLY:
      global_data_ptr->current_egprs_dl_ws = global_data_ptr->gmac_edtm_old_dl_ws;
    break;

    case UL_AND_DL:
      global_data_ptr->current_egprs_ul_ws = global_data_ptr->gmac_edtm_old_ul_ws;
      global_data_ptr->current_egprs_dl_ws = global_data_ptr->gmac_edtm_old_dl_ws;
    break;

    default:
      if(geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_1_G("GMAC bad TBF type: %d",restore_tbf_type);
        MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
        geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
      }
      else
      {
        ERR_GERAN_FATAL_2_G("GMAC bad TBF type: %d",gas_id, restore_tbf_type);
      }
    break;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_tran_edtm_ass_cmd_handler()
===
===  DESCRIPTION
===  Handles the GRR_MAC_DTM_CHANNEL_ASSIGN_REQ if it is received when establishing
===  CS during transfer, or when reallocating DTM when EDTM is active.
===  If no TBF's are defined in the DTM Ass Command then the current TBF's are maintained.
===  Any pending assignments are dropped.
===
===  If one or more TBF's are defined then the following applies:
===     Any active TBF's not defined in the DTM Ass Cmd are implicitly released.  Note that
===     the normal L1->MAC release procedure is not followed.  L1 will silently release the
===     relevant TBF's.
===
===     TBF's defined in the DTM Ass Cmd that are not currently active will be established.
===
===     TBF's defined in the DTM Ass Cmd that are currently active will be reassigned.
===
===  DEPENDENCIES
===    Writes to: gmac_local_tfi_store.ul_tfi_valid
===               gmac_local_tfi_store.dl_tfi_valid
===               t_flags.waiting_for_ph_con_for_dl
===
===
===  RETURN VALUE
===    NONE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void gmac_tran_edtm_ass_cmd_handler( gprs_mac_transfer_substates_t *transfer_state,
                                            dtm_tbf_type_t                 dtm_tbf_type,
                                            gas_id_t                       gas_id
                                          )
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  switch(*transfer_state)
  {
    case TRANSFER_DL:
      switch ( dtm_tbf_type )
      {
        case NO_TBF_PRESENT:
          /*Stay in TRANSFER_DL*/
        break;

        case UL_TBF_PRESENT:
          gmac_transfer_edtm_clear_dl_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC;
        break;

        case DL_TBF_PRESENT:
          grlc_dl_start_t3190(gas_id);
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
          *transfer_state = TRANSFER_DL_REASSIGN;
        break;

        case UL_DL_TBF_PRESENT:
          grlc_dl_start_t3190(gas_id);
          *transfer_state = TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC;
        break;
        default:
           if(geran_get_nv_recovery_restart_enabled(gas_id))
          {
            MSG_GERAN_ERROR_1_G("GMAC EDTM bad tbf type: %d",dtm_tbf_type);
            MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
            geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
          }
          else
          {
            ERR_GERAN_FATAL_2_G("GMAC EDTM bad tbf type: %d",gas_id, dtm_tbf_type);
          }
        break;
      }
      break;

    case TRANSFER_UL_DYNAMIC:
      switch ( dtm_tbf_type )
      {
          case NO_TBF_PRESENT:
            /*Stay in TRANSFER_UL_DYNAMIC*/
          break;

          case UL_TBF_PRESENT:
            *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC;
          break;

          case DL_TBF_PRESENT:
            gmac_transfer_edtm_clear_ul_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
            grlc_dl_start_t3190(gas_id);
            transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
            *transfer_state = TRANSFER_DL_REASSIGN;
          break;

          case UL_DL_TBF_PRESENT:
            grlc_dl_start_t3190(gas_id);
            *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN;
          break;

          default:
            if(geran_get_nv_recovery_restart_enabled(gas_id))
            {
              MSG_GERAN_ERROR_1_G("GMAC EDTM bad tbf type: %d",dtm_tbf_type);
              MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
              geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
            }
            else
            {
              ERR_GERAN_FATAL_2_G("GMAC EDTM bad tbf type: %d",gas_id, dtm_tbf_type);
            }
          break;
      }
      break;

    case TRANSFER_DL_REASSIGN:
      switch ( dtm_tbf_type )
      {
          case NO_TBF_PRESENT:
            gmac_transfer_edtm_restore_tbf_data(DL_ONLY, gas_id);
            transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
            *transfer_state = TRANSFER_DL;
          break;

          case UL_TBF_PRESENT:
            gmac_transfer_edtm_clear_dl_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
            *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC;
          break;

          case DL_TBF_PRESENT:
            grlc_dl_start_t3190(gas_id);
            transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
            /* Stay in state */
          break;

          case UL_DL_TBF_PRESENT:
            grlc_dl_start_t3190(gas_id);
            *transfer_state = TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC;
          break;

          default:
             if(geran_get_nv_recovery_restart_enabled(gas_id))
             {
               MSG_GERAN_ERROR_1_G("GMAC EDTM bad tbf type: %d",dtm_tbf_type);
               MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
               geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
             }
             else
             {
               ERR_GERAN_FATAL_2_G("GMAC EDTM bad tbf type: %d",gas_id, dtm_tbf_type);
             }
          break;
      }
      break;

    case TRANSFER_UL_DYNAMIC_REALLOC:
      switch ( dtm_tbf_type )
      {
        case NO_TBF_PRESENT:
          gmac_transfer_edtm_restore_tbf_data(UL_ONLY, gas_id);
          *transfer_state = TRANSFER_UL_DYNAMIC;
        break;

        case UL_TBF_PRESENT:
          /* stay in state */
        break;

        case DL_TBF_PRESENT:
          gmac_transfer_edtm_clear_ul_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          grlc_dl_start_t3190(gas_id);
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
          *transfer_state = TRANSFER_DL_REASSIGN;
        break;

        case UL_DL_TBF_PRESENT:
          grlc_dl_start_t3190(gas_id);
          *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN;
        break;
        default:
           if(geran_get_nv_recovery_restart_enabled(gas_id))
           {
             MSG_GERAN_ERROR_1_G("GMAC EDTM bad tbf type: %d",dtm_tbf_type);
             MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
             geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
           }
           else
           {
             ERR_GERAN_FATAL_2_G("GMAC EDTM bad tbf type: %d",gas_id, dtm_tbf_type);
           }
        break;
      }
      break;

    case TRANSFER_DL_WITH_DYNAMIC_ALLOC:
      switch ( dtm_tbf_type )
      {
        case NO_TBF_PRESENT:
          gmac_transfer_edtm_clear_ul_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          /* MAC is moving to T_DL state so need to unblock RLC here as there
             is no other trigger */
          gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
          transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
          *transfer_state = TRANSFER_DL;
        break;

        case UL_TBF_PRESENT:
          gmac_transfer_edtm_clear_dl_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC;
        break;

        case DL_TBF_PRESENT:
          gmac_transfer_edtm_clear_ul_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;
          grlc_dl_start_t3190(gas_id);
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
          *transfer_state = TRANSFER_DL_REASSIGN;
        break;

        case UL_DL_TBF_PRESENT:
          grlc_dl_start_t3190(gas_id);
          *transfer_state = TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC;
        break;
        default:
           if(geran_get_nv_recovery_restart_enabled(gas_id))
           {
             MSG_GERAN_ERROR_1_G("GMAC EDTM bad tbf type: %d",dtm_tbf_type);
             MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
             geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
           }
           else
           {
             ERR_GERAN_FATAL_2_G("GMAC EDTM bad tbf type: %d",gas_id, dtm_tbf_type);
           }
        break;
      }
      break;

    case TRANSFER_UL_DYNAMIC_DL_ASSIGN:
      switch ( dtm_tbf_type )
      {
          case NO_TBF_PRESENT:
            gmac_transfer_edtm_clear_dl_tbf(EDTM_CLEAR_PENDING_TBF, gas_id);
            *transfer_state = TRANSFER_UL_DYNAMIC;
          break;

          case UL_TBF_PRESENT:
            gmac_transfer_edtm_clear_dl_tbf(EDTM_CLEAR_PENDING_TBF, gas_id);
            *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC;
          break;

          case DL_TBF_PRESENT:
            gmac_transfer_edtm_clear_ul_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
            grlc_dl_start_t3190(gas_id);
            transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
            *transfer_state = TRANSFER_DL_REASSIGN;
          break;

          case UL_DL_TBF_PRESENT:
            grlc_dl_start_t3190(gas_id);
            *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN;
          break;
          default:
             if(geran_get_nv_recovery_restart_enabled(gas_id))
             {
               MSG_GERAN_ERROR_1_G("GMAC EDTM bad tbf type: %d",dtm_tbf_type);
               MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
               geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
             }
             else
             {
               ERR_GERAN_FATAL_2_G("GMAC EDTM bad tbf type: %d",gas_id, dtm_tbf_type);
             }
          break;
      }
      break;

    case TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC:
      switch ( dtm_tbf_type )
      {
        case NO_TBF_PRESENT:
          gmac_transfer_edtm_restore_tbf_data(DL_ONLY, gas_id);
          gmac_transfer_edtm_clear_ul_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          /* MAC is moving to T_DL state so need to unblock RLC here as there
             is no other trigger */
          gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN, __LINE__, gas_id);
          transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
          *transfer_state = TRANSFER_DL;
        break;

        case UL_TBF_PRESENT:
          gmac_transfer_edtm_clear_dl_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC;
        break;

        case DL_TBF_PRESENT:
          gmac_transfer_edtm_clear_ul_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          grlc_dl_start_t3190(gas_id);
          transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
          *transfer_state = TRANSFER_DL_REASSIGN;
        break;

        case UL_DL_TBF_PRESENT:
          grlc_dl_start_t3190(gas_id);
          /*stay in state*/
        break;
        default:
           if(geran_get_nv_recovery_restart_enabled(gas_id))
           {
             MSG_GERAN_ERROR_1_G("GMAC EDTM bad tbf type: %d",dtm_tbf_type);
             MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
             geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
           }
           else
           {
             ERR_GERAN_FATAL_2_G("GMAC EDTM bad tbf type: %d",gas_id, dtm_tbf_type);
           }
        break;
      }
      break;

    case TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN:
      switch ( dtm_tbf_type )
      {
        case NO_TBF_PRESENT:
          gmac_transfer_edtm_clear_dl_tbf(EDTM_CLEAR_PENDING_TBF, gas_id);
          gmac_transfer_edtm_restore_tbf_data(UL_ONLY,gas_id);
          *transfer_state = TRANSFER_UL_DYNAMIC;
        break;

        case UL_TBF_PRESENT:
          gmac_transfer_edtm_clear_dl_tbf(EDTM_CLEAR_PENDING_TBF, gas_id);
          *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC;
        break;

        case DL_TBF_PRESENT:
          gmac_transfer_edtm_clear_ul_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          grlc_dl_start_t3190(gas_id);
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
          *transfer_state = TRANSFER_DL_REASSIGN;
        break;

        case UL_DL_TBF_PRESENT:
          grlc_dl_start_t3190(gas_id);
          /*stay in state*/
        break;

        default:
          if(geran_get_nv_recovery_restart_enabled(gas_id))
          {
            MSG_GERAN_ERROR_1_G("GMAC EDTM bad tbf type: %d",dtm_tbf_type);
            MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
            geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
          }
          else
          {
            ERR_GERAN_FATAL_2_G("GMAC EDTM bad tbf type: %d",gas_id, dtm_tbf_type);
          }
        break;
      }
      break;

    case TRANSFER_CONCURRENT_DYNAMIC_TRANSFER:
      switch ( dtm_tbf_type )
      {
        case NO_TBF_PRESENT:
          /* stay in state */
        break;

        case UL_TBF_PRESENT:
          gmac_transfer_edtm_clear_dl_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC;
        break;

        case DL_TBF_PRESENT:
          gmac_transfer_edtm_clear_ul_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          grlc_dl_start_t3190(gas_id);
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
          *transfer_state = TRANSFER_DL_REASSIGN;
        break;

        case UL_DL_TBF_PRESENT:
          grlc_dl_start_t3190(gas_id);
          *transfer_state = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;
        break;

        default:
          if(geran_get_nv_recovery_restart_enabled(gas_id))
          {
            MSG_GERAN_ERROR_1_G("GMAC EDTM bad tbf type: %d",dtm_tbf_type);
            MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
            geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
          }
          else
          {
            ERR_GERAN_FATAL_2_G("GMAC EDTM bad tbf type: %d",gas_id, dtm_tbf_type);
          }
        break;
      }
      break;

    case TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN:
      switch ( dtm_tbf_type )
      {
        case NO_TBF_PRESENT:
          gmac_transfer_edtm_restore_tbf_data(DL_ONLY, gas_id);
          *transfer_state = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;
        break;

        case UL_TBF_PRESENT:
          gmac_transfer_edtm_clear_dl_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC;
        break;

        case DL_TBF_PRESENT:
          gmac_transfer_edtm_clear_ul_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          grlc_dl_start_t3190(gas_id);
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
          *transfer_state = TRANSFER_DL_REASSIGN;
        break;

        case UL_DL_TBF_PRESENT:
          grlc_dl_start_t3190(gas_id);
          *transfer_state = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;
        break;

        default:
           if(geran_get_nv_recovery_restart_enabled(gas_id))
           {
             MSG_GERAN_ERROR_1_G("GMAC EDTM bad tbf type: %d",dtm_tbf_type);
             MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
             geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
           }
           else
           {
             ERR_GERAN_FATAL_2_G("GMAC EDTM bad tbf type: %d",gas_id, dtm_tbf_type);
           }
        break;
      }
      break;

    case TRANSFER_CONCURRENT_DYNAMIC_REALLOC:
      switch ( dtm_tbf_type )
      {
          case NO_TBF_PRESENT:
            gmac_transfer_edtm_restore_tbf_data(UL_ONLY, gas_id);
            *transfer_state = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;
          break;

          case UL_TBF_PRESENT:
            gmac_transfer_edtm_clear_dl_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
            *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC;
          break;

          case DL_TBF_PRESENT:
            gmac_transfer_edtm_clear_ul_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
            grlc_dl_start_t3190(gas_id);
            transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
            *transfer_state = TRANSFER_DL_REASSIGN;
          break;

          case UL_DL_TBF_PRESENT:
            grlc_dl_start_t3190(gas_id);
            *transfer_state = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;
          break;

          default:
             if(geran_get_nv_recovery_restart_enabled(gas_id))
             {
               MSG_GERAN_ERROR_1_G("GMAC EDTM bad tbf type: %d",dtm_tbf_type);
               MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
               geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
             }
             else
             {
               ERR_GERAN_FATAL_2_G("GMAC EDTM bad tbf type: %d",gas_id, dtm_tbf_type);
             }
          break;
      }
      break;

    case TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC:
      switch ( dtm_tbf_type )
      {
        case NO_TBF_PRESENT:
          gmac_transfer_edtm_restore_tbf_data(UL_AND_DL, gas_id);
          *transfer_state = TRANSFER_CONCURRENT_DYNAMIC_TRANSFER;
        break;

        case UL_TBF_PRESENT:
          gmac_transfer_edtm_clear_dl_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          *transfer_state = TRANSFER_UL_DYNAMIC_REALLOC;
        break;

        case DL_TBF_PRESENT:
          gmac_transfer_edtm_clear_ul_tbf(EDTM_CLEAR_ACTIVE_TBF, gas_id);
          grlc_dl_start_t3190(gas_id);
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
          *transfer_state = TRANSFER_DL_REASSIGN;
        break;

        case UL_DL_TBF_PRESENT:
          grlc_dl_start_t3190(gas_id);
          /* stay in state */
        break;

        default:
           if(geran_get_nv_recovery_restart_enabled(gas_id))
           {
             MSG_GERAN_ERROR_1_G("GMAC EDTM bad tbf type: %d",dtm_tbf_type);
             MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
             geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
           }
           else
           {
             ERR_GERAN_FATAL_2_G("GMAC EDTM bad tbf type: %d",gas_id, dtm_tbf_type);
           }
        break;
      }
      break;

    default:
      if(geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_1_G("EDTM handler called in wrong state: 0x%x",*transfer_state);
        MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
        geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
      }
      else
      {
        ERR_GERAN_FATAL_2_G("EDTM handler called in wrong state: 0x%x",gas_id, *transfer_state);
      }
    break;
  }
}

#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/

/*===========================================================================
===
===  FUNCTION      gmac_tran_edtm_cs_rel_ind_handler()
===
===  DESCRIPTION
===  Handles the Packet CS Release Ind message.  Performs address checking
===  and handles the PCRI based on the current transfer state.
===
===  CTRL_ACK handling:
===  The requirements for CTRL_ACK are in 44.060 11.2.37.2.
===  If CTRL_ACK is set then MAC will check that T3192 is running, the TFI
===  specified matches and that the PCRI is received on the DL PACCH.  If this
===  is the case then the DL TBF is released.
===
===  If CTRL_ACK is not set but T3192 is running then this is an abnormal case.
===
===
===  DEPENDENCIES
===  csn_dec_data_ptr
===  gmac_local_tfi_store.dl_tfi_valid
===  t_flags.edtm_cs_rel_freq_set
===  mac_l1_edtm_cs_rel_alloc_msg
===
===  RETURN VALUE
===    New transfer state.
===
===  SIDE EFFECTS
===
===========================================================================*/
gprs_mac_transfer_substates_t gmac_tran_edtm_cs_rel_ind_handler(
                                       gprs_mac_transfer_substates_t transfer_state,
                                       active_tbf_t                  active_tbfs,
                                       gas_id_t                      gas_id )
{
  /* Local Data */
  packet_cs_release_ind_t      *pcri_ptr              = NULL;
  boolean                       addr_match            = FALSE;
  dtm_tbf_type_t                pcri_tbf_type         = NO_TBF_PRESENT;
  gmac_pkt_cs_rel_result_t      result                = PCRI_PASS;
  char                         *mac_tran_str          = NULL;
  mac_header_tfi_t              mac_header_tfi_check  = MAC_HEADER_TFI_NOT_VALID;
  boolean                       ctrl_ack_set          = FALSE;
  gmac_global_data_t           *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t         *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  pcri_ptr = (packet_cs_release_ind_t*)global_data_ptr->csn_dec_data_ptr;

  transfer_data_ptr->t_flags.edtm_pcri_rel_dl_tbf_for_ctrl_ack = FALSE;

  mac_tran_str = gmac_convert_transfer_substate_to_string(transfer_state, gas_id);

  /* Address Checking using GLOBAL_TFI */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
  {
    mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);

    if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
    {
      addr_match = gmac_util_global_tfi(pcri_ptr->global_tfi,
                                        gas_id);
    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
    {
      addr_match = TRUE;
    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
    {
      addr_match = FALSE;
    }
  }

  if (addr_match)
  {
#ifdef PERLUTF
    #error code not present
#endif

    MSG_GERAN_MED_0_G("MAC Rcvd PACKET_CS_RELEASE_INDICATION");

    if (global_data_ptr->csn_dec_hdr_ptr->error_code == MESSAGE_ESCAPE_ERROR)
    {
      gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_MESS_ESCAPE_CAUSE, PACKET_CS_RELEASE, gas_id);
      return transfer_state;
    }

    /* work out the TBF's present */
    if ( (pcri_ptr->num_mult_ul_assignments > 0) &&
         (pcri_ptr->num_mult_dl_assignments > 0) )
    {
      pcri_tbf_type = UL_DL_TBF_PRESENT;
    }
    else if (pcri_ptr->num_mult_ul_assignments > 0)
    {
      pcri_tbf_type = UL_TBF_PRESENT;
    }
    else if (pcri_ptr->num_mult_dl_assignments > 0)
    {
      pcri_tbf_type = DL_TBF_PRESENT;
    }
    else
    {
      pcri_tbf_type = NO_TBF_PRESENT;
    }


    /* Determine if CTRL_ACK is set */
    if ( (pcri_ptr->num_mult_dl_assignments > 0) &&
         (pcri_ptr->multiple_dl_assignments[0].num_dl_tbf_assignments > 0) &&
         (pcri_ptr->multiple_dl_assignments[0].dl_tbf_assignments[0].control_ack == 1) )
    {
      ctrl_ack_set = TRUE;
    }
    else
    {
      ctrl_ack_set = FALSE;
    }


    switch (pcri_tbf_type)
    {
      case NO_TBF_PRESENT:
        switch(transfer_state)
        {
          case TRANSFER_INACTIVE:
            MSG_GERAN_ERROR_0_G("MAC Packet CS Rel Ind received in TRANSFER_INACTIVE, Ignore MSG");
          break;

          case TRANSFER_DL:
          case TRANSFER_DL_REASSIGN:
          case TRANSFER_DL_WITH_DYNAMIC_ALLOC:
          case TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC:
          case TRANSFER_UL_DYNAMIC:
          case TRANSFER_UL_DYNAMIC_REALLOC:
          case TRANSFER_UL_DYNAMIC_DL_ASSIGN:
          case TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN:
          case TRANSFER_CONCURRENT_DYNAMIC_TRANSFER:
          case TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN:
          case TRANSFER_CONCURRENT_DYNAMIC_REALLOC:
          case TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC:
            result = gmac_tran_edtm_pkt_cs_rel_msg_handler(pcri_ptr,active_tbfs,ctrl_ack_set, gas_id);

            if (result == PCRI_PASS)
            {
              gmac_tran_edtm_cs_rel_handle_glob_pkt_ta(pcri_ptr->global_pkt_timing, gas_id);
              gmac_send_mac_grr_edtm_cs_release_ind(EDTM_CS_REL_ENHANCED, gas_id);
            }
            else if (result == PCRI_ABNORMAL_REL)
            {
              gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
            }
          break;

          default:
            MSG_GERAN_ERROR_1_G("MAC Packet CS Rel Ind received in Bad state:0x%x",transfer_state);
          break;
        }
      break;

      case UL_TBF_PRESENT:
        switch(transfer_state)
        {
          case TRANSFER_INACTIVE:
            MSG_GERAN_ERROR_0_G("MAC Packet CS Rel Ind received in TRANSFER_INACTIVE, Ignore MSG");
          break;

          case TRANSFER_DL:
          case TRANSFER_DL_REASSIGN:
            MSG_SPRINTF_2( MSG_SSID_DFLT,
                           MSG_LEGACY_MED,
                           "ds%d: GMAC, PCRI Expected DL TBF, received UL TBF in %s, AB Release",
                           gas_id,
                            mac_tran_str);
            gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
          break;

          case TRANSFER_UL_DYNAMIC:
          case TRANSFER_UL_DYNAMIC_REALLOC:
            result = gmac_tran_edtm_pkt_cs_rel_msg_handler(pcri_ptr,active_tbfs,ctrl_ack_set, gas_id);

            if (result == PCRI_PASS)
            {
              if (pcri_ptr->freq_params_present)
              {
                /* Send MAC_L1_FREQ_CONFIG */
                (void)gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);
              }
              gmac_send_mac_grr_edtm_cs_release_ind(EDTM_CS_REL_ENHANCED,gas_id);
              gmac_tran_send_mac_l1_edtm_cs_rel_tbf_alloc_msg(&transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg,gas_id);
              transfer_state = TRANSFER_UL_DYNAMIC_REALLOC;
            }
            else if (result == PCRI_ABNORMAL_REL)
            {
              gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
            }
          break;

          case TRANSFER_DL_WITH_DYNAMIC_ALLOC:
          case TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC:
          case TRANSFER_UL_DYNAMIC_DL_ASSIGN:
          case TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN:
          case TRANSFER_CONCURRENT_DYNAMIC_TRANSFER:
          case TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN:
          case TRANSFER_CONCURRENT_DYNAMIC_REALLOC:
          case TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC:
            MSG_SPRINTF_2( MSG_SSID_DFLT,
                           MSG_LEGACY_MED,
                           "ds%d: GMAC, PCRI Expected UL+DL TBF, received UL TBF in %s, AB Release",
                           gas_id,
                            mac_tran_str);
            gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
          break;

          default:
            MSG_GERAN_ERROR_1_G("MAC Packet CS Rel Ind received in Bad state:0x%x",transfer_state);
          break;
        }
      break;

      case DL_TBF_PRESENT:
        switch(transfer_state)
        {
          case TRANSFER_INACTIVE:
            MSG_GERAN_ERROR_0_G("MAC Packet CS Rel Ind received in TRANSFER_INACTIVE, Ignore MSG");
          break;

          case TRANSFER_DL:
          case TRANSFER_DL_REASSIGN:
            result = gmac_tran_edtm_pkt_cs_rel_msg_handler(pcri_ptr,active_tbfs,ctrl_ack_set,gas_id);

            if (result == PCRI_PASS)
            {
              if (ctrl_ack_set == TRUE)
              {
                /* DL only is active and Ctrl Ack has released the DL TBF, so ab release*/
                gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
              }
              else
              {
                if (pcri_ptr->freq_params_present)
                {
                  /* Send MAC_L1_FREQ_CONFIG */
                  (void)gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);
                }
                grlc_dl_start_t3190(gas_id);
                gmac_send_mac_grr_edtm_cs_release_ind(EDTM_CS_REL_ENHANCED,gas_id);
                gmac_tran_send_mac_l1_edtm_cs_rel_tbf_alloc_msg(&transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg,gas_id);
                transfer_state = TRANSFER_DL_REASSIGN;
              }
            }
            else if (result == PCRI_ABNORMAL_REL)
            {
              gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
            }
          break;

          case TRANSFER_UL_DYNAMIC:
          case TRANSFER_UL_DYNAMIC_REALLOC:
            MSG_SPRINTF_2( MSG_SSID_DFLT,
                           MSG_LEGACY_MED,
                           "ds%d: GMAC, PCRI Expected UL TBF, received DL TBF in %s, AB Release",
                           gas_id,
                            mac_tran_str);
            gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
          break;

          case TRANSFER_DL_WITH_DYNAMIC_ALLOC:
            if (ctrl_ack_set == TRUE)
            {
              result = gmac_tran_edtm_pkt_cs_rel_msg_handler(pcri_ptr,active_tbfs,ctrl_ack_set,gas_id);

              /* defer release of DL until UL connects */
              if (result == PCRI_PASS)
              {
                gmac_tran_edtm_cs_rel_handle_glob_pkt_ta(pcri_ptr->global_pkt_timing,gas_id);
                gmac_send_mac_grr_edtm_cs_release_ind(EDTM_CS_REL_ENHANCED,gas_id);
                transfer_data_ptr->t_flags.edtm_pcri_rel_dl_tbf_for_ctrl_ack = TRUE;
                MSG_GERAN_MED_0_G("GMAC PCRI CTRL_ACK set, DL to be released when UL connects");
              }
              else if (result == PCRI_ABNORMAL_REL)
              {
                gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
              }
            }
            else
            {
              MSG_SPRINTF_2( MSG_SSID_DFLT,
                           MSG_LEGACY_MED,
                           "ds%d: GMAC, PCRI Expected UL+DL TBF, received DL TBF in %s, AB Release",
                            gas_id,
                            mac_tran_str);
              gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
            }
          break;

          case TRANSFER_UL_DYNAMIC_DL_ASSIGN:
          case TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN:
          case TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC:
            MSG_SPRINTF_2( MSG_SSID_DFLT,
                           MSG_LEGACY_MED,
                           "ds%d: GMAC, PCRI Expected UL+DL TBF, received DL TBF in %s, AB Release",
                            gas_id,
                            mac_tran_str);
            gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
          break;

          case TRANSFER_CONCURRENT_DYNAMIC_TRANSFER:
          case TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN:
          case TRANSFER_CONCURRENT_DYNAMIC_REALLOC:
          case TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC:
            if (ctrl_ack_set == TRUE)
            {
              result = gmac_tran_edtm_pkt_cs_rel_msg_handler(pcri_ptr,active_tbfs,ctrl_ack_set,gas_id);

              if (result == PCRI_PASS)
              {
                gmac_tran_edtm_cs_rel_handle_glob_pkt_ta(pcri_ptr->global_pkt_timing,gas_id);
                gmac_send_mac_grr_edtm_cs_release_ind(EDTM_CS_REL_ENHANCED,gas_id);

                /*release DL TBF because CTRL_ACK is set*/
                gmac_util_send_dl_release_to_rlc(gas_id);
                gmac_transfer_dl_release_clean(gas_id);
                gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL,gas_id);
                mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE,gas_id);
              }
              else if (result == PCRI_ABNORMAL_REL)
              {
                gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
              }
            }
            else
            {
              MSG_SPRINTF_2( MSG_SSID_DFLT,
                             MSG_LEGACY_MED,
                             "ds%d: GMAC, PCRI Expected UL+DL TBF, received DL TBF in %s, AB Release",
                              gas_id,
                              mac_tran_str);
              gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
            }
          break;

          default:
          MSG_GERAN_ERROR_1_G("MAC Packet CS Rel Ind received in Bad state:0x%x",transfer_state);
          break;
        }
      break;

      case UL_DL_TBF_PRESENT:
        switch(transfer_state)
        {
          case TRANSFER_INACTIVE:
            MSG_GERAN_ERROR_0_G("MAC Packet CS Rel Ind received in TRANSFER_INACTIVE, Ignore MSG");
          break;

          case TRANSFER_DL:
          case TRANSFER_DL_REASSIGN:
            MSG_SPRINTF_2( MSG_SSID_DFLT,
                           MSG_LEGACY_MED,
                           "ds%d: GMAC, PCRI Expected DL TBF, received UL+DL TBF in %s, AB Release",
                            gas_id,
                            mac_tran_str);
            gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
          break;

          case TRANSFER_DL_WITH_DYNAMIC_ALLOC:
            result = gmac_tran_edtm_pkt_cs_rel_msg_handler(pcri_ptr,active_tbfs,ctrl_ack_set,gas_id);

            if (result == PCRI_PASS)
            {
              if (pcri_ptr->freq_params_present)
              {
                /* Send MAC_L1_FREQ_CONFIG */
                (void)gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);
              }
              gmac_send_mac_grr_edtm_cs_release_ind(EDTM_CS_REL_ENHANCED,gas_id);
              gmac_tran_send_mac_l1_edtm_cs_rel_tbf_alloc_msg(&transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg,gas_id);

              grlc_dl_start_t3190(gas_id);

              if (ctrl_ack_set == TRUE)
              {
                /*Ctrl Ack releases DL so only UL is being redefined,
                  do not change state and set flag to indicate that the DL is to be released
                  when the UL connects */
                MSG_GERAN_MED_0_G("GMAC PCRI CTRL_ACK set, DL to be released when UL connects");
                transfer_data_ptr->t_flags.edtm_pcri_rel_dl_tbf_for_ctrl_ack = TRUE;
              }
              else
              {
                transfer_state = TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC;
              }
            }
            else if (result == PCRI_ABNORMAL_REL)
            {
              gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
            }
          break;

          case TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC:

            if (ctrl_ack_set == TRUE)
            {
              MSG_SPRINTF_2( MSG_SSID_DFLT,
                           MSG_LEGACY_MED,
                           "ds%d: GMAC,PCRI Ctrl Ack removes DL, Cannot realloc UL in state: %s, AB Release",
                            gas_id,
                            mac_tran_str);
              gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
            }
            else
            {
              result = gmac_tran_edtm_pkt_cs_rel_msg_handler(pcri_ptr,active_tbfs,ctrl_ack_set,gas_id);

              if (result == PCRI_PASS)
              {
                if (pcri_ptr->freq_params_present)
                {
                  /* Send MAC_L1_FREQ_CONFIG */
                  (void)gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);
                }
                grlc_dl_start_t3190(gas_id);
                gmac_send_mac_grr_edtm_cs_release_ind(EDTM_CS_REL_ENHANCED,gas_id);
                gmac_tran_send_mac_l1_edtm_cs_rel_tbf_alloc_msg(&transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg,gas_id);

                transfer_state = TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC;
              }
              else if (result == PCRI_ABNORMAL_REL)
              {
                gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
              }
            }
          break;

          case TRANSFER_UL_DYNAMIC:
          case TRANSFER_UL_DYNAMIC_REALLOC:
              /*Establishing DL TBF*/
              result = gmac_tran_edtm_pkt_cs_rel_msg_handler(pcri_ptr,active_tbfs,ctrl_ack_set,gas_id);

              if (result == PCRI_PASS)
              {
                /* We should not have passed the Ctrl Ack checks if the DL is not connected */
                if (ctrl_ack_set == TRUE)
                {
                  MSG_GERAN_ERROR_0_G("MAC PCRI Ctrl Ack Set, Internal error: Pass Result in UL only State, Ab Release ");
                  gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
                }
                else
                {
                  if (pcri_ptr->freq_params_present)
                  {
                    /* Send MAC_L1_FREQ_CONFIG */
                    (void)gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);
                  }
                  grlc_dl_start_t3190(gas_id);
                  gmac_send_mac_grr_edtm_cs_release_ind(EDTM_CS_REL_ENHANCED,gas_id);
                  gmac_tran_send_mac_l1_edtm_cs_rel_tbf_alloc_msg(&transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg,gas_id);
                  transfer_state = TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN;
                }
              }
              else if (result == PCRI_ABNORMAL_REL)
              {
                gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
              }
            break;

          case TRANSFER_UL_DYNAMIC_DL_ASSIGN:
          case TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN:
            /*Establishing DL TBF*/
            result = gmac_tran_edtm_pkt_cs_rel_msg_handler(pcri_ptr,active_tbfs,ctrl_ack_set,gas_id);

            if (result == PCRI_PASS)
            {
              if (ctrl_ack_set == TRUE)
              {
                /* We should not have passed the Ctrl Ack checks if the DL has not been connected yet */
                MSG_GERAN_ERROR_0_G("MAC PCRI Ctrl Ack Set, Internal error: Pass Result in UL with DL Ass State, Ab Release ");
                gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
              }
              else
              {
                if (pcri_ptr->freq_params_present)
                {
                  /* Send MAC_L1_FREQ_CONFIG */
                  (void)gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);
                }
                grlc_dl_start_t3190(gas_id);
                gmac_send_mac_grr_edtm_cs_release_ind(EDTM_CS_REL_ENHANCED,gas_id);
                gmac_tran_send_mac_l1_edtm_cs_rel_tbf_alloc_msg(&transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg,gas_id);
                transfer_state = TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN;
              }
            }
            else if (result == PCRI_ABNORMAL_REL)
            {
              gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
            }
          break;


          case TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN:
          case TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC:
            if (ctrl_ack_set == TRUE)
            {
              gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
              MSG_SPRINTF_2( MSG_SSID_DFLT,
                           MSG_LEGACY_MED,
                             "ds%d: GMAC,PCRI Ctrl Ack removes DL, Cannot realloc UL in state: %s, AB Release",
                             gas_id,
                            mac_tran_str);
            }
            else
            {
              /*Establishing DL TBF*/
              result = gmac_tran_edtm_pkt_cs_rel_msg_handler(pcri_ptr,active_tbfs,ctrl_ack_set,gas_id);

              if (result == PCRI_PASS)
              {
                if (pcri_ptr->freq_params_present)
                {
                  /* Send MAC_L1_FREQ_CONFIG */
                  (void)gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);
                }
                grlc_dl_start_t3190(gas_id);
                gmac_send_mac_grr_edtm_cs_release_ind(EDTM_CS_REL_ENHANCED,gas_id);
                gmac_tran_send_mac_l1_edtm_cs_rel_tbf_alloc_msg(&transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg,gas_id);

                transfer_state = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;
              }
              else if (result == PCRI_ABNORMAL_REL)
              {
                gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
              }
            }
          break;

          case TRANSFER_CONCURRENT_DYNAMIC_TRANSFER:
          case TRANSFER_CONCURRENT_DYNAMIC_REALLOC:
            /*Establishing DL TBF*/
            result = gmac_tran_edtm_pkt_cs_rel_msg_handler(pcri_ptr,active_tbfs,ctrl_ack_set,gas_id);

            if (result == PCRI_PASS)
            {
              if (pcri_ptr->freq_params_present)
              {
                /* Send MAC_L1_FREQ_CONFIG */
                (void)gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);
              }
              gmac_send_mac_grr_edtm_cs_release_ind(EDTM_CS_REL_ENHANCED,gas_id);
              gmac_tran_send_mac_l1_edtm_cs_rel_tbf_alloc_msg(&transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg,gas_id);

              grlc_dl_start_t3190(gas_id);

              if (ctrl_ack_set == TRUE)
              {
                gmac_util_send_dl_release_to_rlc(gas_id);
                gmac_transfer_dl_release_clean(gas_id);
                gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL,gas_id);
                mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE,gas_id);

                /* The DL is being released */
                transfer_state = TRANSFER_CONCURRENT_DYNAMIC_REALLOC;
              }
              else
              {
                transfer_state = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;
              }
            }
            else if (result == PCRI_ABNORMAL_REL)
            {
              gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id);
            }
          break;

          default:
            MSG_GERAN_ERROR_1_G("MAC Packet CS Rel Ind received in Bad state:0x%x",transfer_state);
          break;
        }
      break;

      default:
        MSG_GERAN_ERROR_1_G("GMAC PCRI handler, bad tbf_type: 0x%x",pcri_tbf_type);
      break;
    }
  }

  return transfer_state;
}

/*===========================================================================
===
===  FUNCTION      gmac_tran_edtm_cs_rel_handle_glob_pkt_ta
===
===  DESCRIPTION
===  Handles the global pkt ta, updates the local store and sends a MAC_L1_POWER_CONTROL message
===  if TA is present.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  None
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_tran_edtm_cs_rel_handle_glob_pkt_ta(global_pkt_timing_t glob_pkt_ta,
                                                     gas_id_t            gas_id)
{
  /*The global store gmac_local_l1_ta_params will be left unchanged.
    This is consistent with the way that the PTR code deals with Global Pkt TA*/
  mac_l1_sig_t mac_l1_sig;

  if ( (glob_pkt_ta.ta_flag == valid) ||
       (glob_pkt_ta.uplink_ta_flag == valid) ||
       (glob_pkt_ta.dwnlink_ta_flag == valid) )
  {
    /*populate MAC_L1_POWER_CONTROL message*/
    mac_l1_sig.mac_l1_power_control.power_control.present = FALSE;
    mac_l1_sig.mac_l1_power_control.global_packet_ta_present = TRUE;

    /*TA Value*/
    if (glob_pkt_ta.ta_flag == valid)
    {
      mac_l1_sig.mac_l1_power_control.global_packet_ta.tav.present = TRUE;
      mac_l1_sig.mac_l1_power_control.global_packet_ta.tav.ta_value = glob_pkt_ta.ta_value;
    }
    else
    {
      mac_l1_sig.mac_l1_power_control.global_packet_ta.tav.present = FALSE;
    }

    /*Uplink TAI Index*/
    if (glob_pkt_ta.uplink_ta_flag == valid)
    {
      mac_l1_sig.mac_l1_power_control.global_packet_ta.uplink_tai.present = TRUE;
      mac_l1_sig.mac_l1_power_control.global_packet_ta.uplink_tai.ta_timeslot = glob_pkt_ta.uplink_ta_ts_num;
      mac_l1_sig.mac_l1_power_control.global_packet_ta.uplink_tai.tai = glob_pkt_ta.uplink_ta_index;
    }
    else
    {
      mac_l1_sig.mac_l1_power_control.global_packet_ta.uplink_tai.present = FALSE;
    }

    /*Downlink TAI Index*/
    if (glob_pkt_ta.dwnlink_ta_flag == valid)
    {
      mac_l1_sig.mac_l1_power_control.global_packet_ta.downlink_tai.present = TRUE;
      mac_l1_sig.mac_l1_power_control.global_packet_ta.downlink_tai.ta_timeslot = glob_pkt_ta.dwnlink_ta_ts_num;
      mac_l1_sig.mac_l1_power_control.global_packet_ta.downlink_tai.tai = glob_pkt_ta.dwnlink_ta_index;
    }
    else
    {
      mac_l1_sig.mac_l1_power_control.global_packet_ta.downlink_tai.present = FALSE;
    }

    /* send MAC_L1_POWER_CONTROL with parameters */
    MSG_GERAN_MED_0_G("GMAC Sent MAC_L1_POWER_CONTROL");
    mac_l1_sig.mac_l1_power_control.message_header.message_set = MS_MAC_L1;
    mac_l1_sig.mac_l1_power_control.message_header.message_id = (uint8)MAC_L1_POWER_CONTROL;
    gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
  }
}


/*===========================================================================
===
===  FUNCTION      gmac_tran_edtm_cs_rel_handle_abnormal_rel
===
===  DESCRIPTION
===  Handles an abnormal release as a result of an error in the Packet
===  CS Release Ind.
===
===  DEPENDENCIES
===  gmac_transfer_tbf_rel_cause
===
===  RETURN VALUE
===  None
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_tran_edtm_cs_rel_handle_abnormal_rel(gas_id_t gas_id)
{
  /* After sending this, GRR will respond with GRR_MAC_DTM_CHANNEL_RELEASE_IND ps rel=1 */
  gmac_send_mac_grr_edtm_cs_release_ind(EDTM_CS_REL_ABNORMAL, gas_id);
}


/*===========================================================================
===
===  FUNCTION      gmac_tran_send_mac_l1_edtm_cs_rel_tbf_alloc_msg
===
===  DESCRIPTION
===  Send MAC_L1_EDTM_CS_REL_TBF_ALLOC to L1
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
static void gmac_tran_send_mac_l1_edtm_cs_rel_tbf_alloc_msg(  mac_l1_edtm_cs_rel_tbf_alloc_t
                                                             *mac_l1_cs_rel_msg,
                                                              gas_id_t gas_id )
{
  mac_l1_sig_t mac_l1_sig;

  mac_l1_sig.mac_l1_edtm_cs_rel_tbf_alloc = *mac_l1_cs_rel_msg;

  /* send MAC_L1_EDTM_CS_REL_TBF_ALLOC with parameters */
  MSG_GERAN_MED_1_G("GMAC Sent MAC_L1_EDTM_CS_REL_TBF_ALLOC tbf_type: 0x%x",mac_l1_cs_rel_msg->tbf_type);
  mac_l1_sig.mac_l1_edtm_cs_rel_tbf_alloc.message_header.message_set = MS_MAC_L1;
  mac_l1_sig.mac_l1_edtm_cs_rel_tbf_alloc.message_header.message_id =
      (uint8)MAC_L1_EDTM_CS_REL_TBF_ALLOC;
  gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
}

/*===========================================================================
===
===  FUNCTION      gmac_tran_edtm_pkt_cs_rel_msg_handler()
===
===  DESCRIPTION
===  Handles the Pkt CS Release Ind message content.  Validates the PCRI and
===  populates the config messages.  Also checks for coding scheme changes and calls
===  the RRBP handler.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    gmac_pkt_cs_rel_result_t - PCRI_PASS         - Passed validation, config msgs populated.
===                               PCRI_IGNORE_MSG   - Ignore PCRI and remain in dual transfer mode.
===                               PCRI_ABNORMAL_REL - Error in PCRI, perform Abnormal Release.
===
===  SIDE EFFECTS
===
===========================================================================*/
static gmac_pkt_cs_rel_result_t gmac_tran_edtm_pkt_cs_rel_msg_handler(packet_cs_release_ind_t *pcri_ptr,
                                                                      active_tbf_t             active_tbfs,
                                                                      boolean                  ctrl_ack_set,
                                                                      gas_id_t                 gas_id)
{
  gmac_pkt_cs_rel_result_t msg_validation_status;

  msg_validation_status = gmac_tran_edtm_validate_pkt_cs_rel_msg(pcri_ptr,active_tbfs,ctrl_ack_set,gas_id);

  if ( msg_validation_status == PCRI_PASS )
  {
    /* if at least one assignment in the PCRI */
    if ( (pcri_ptr->num_mult_ul_assignments > 0) ||
         ( ( pcri_ptr->num_mult_dl_assignments > 0) &&
             ctrl_ack_set == FALSE) )
    {
      gmac_tran_edtm_populate_cs_rel_config_msgs(pcri_ptr,ctrl_ack_set,gas_id);

      gmac_tran_edtm_handle_coding_scheme_change(pcri_ptr,gas_id);
    }

    rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);
  }

  return msg_validation_status;
}

/*===========================================================================
===
===  FUNCTION      gmac_tran_edtm_handle_coding_scheme_change()
===
===  DESCRIPTION
===  Sets the pending coding scheme change flag (t_flags.gmac_transfer_cs_reconfig_flag)
===  and the pending coding scheme (pua_ptr_cs_change) when the coding scheme in the PCRI
===  differs from the current (gmac_local_cs_setting).
===
===  DEPENDENCIES
===  Writes: t_flags.gmac_transfer_cs_reconfig_flag
===          pua_ptr_cs_change
===
===  Reads:  gmac_local_cs_setting
===
===
===  RETURN VALUE
===    NONE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void gmac_tran_edtm_handle_coding_scheme_change(packet_cs_release_ind_t *pcri_ptr,
                                                       gas_id_t                 gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
  {
    uint8 pcri_mcs = 0;

    (void)gmac_get_egprs_mcs_from_pkt_cs_rel(pcri_ptr,&pcri_mcs,gas_id);

    MSG_GERAN_MED_3_G("t_cs_reconfig_flg %d ,l_cs_setting %d, PCRI mcs %d",
                      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag,
                      global_data_ptr->gmac_local_cs_setting,pcri_mcs);

    if (global_data_ptr->gmac_local_cs_setting != pcri_mcs)
    {
      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = TRUE;
      transfer_data_ptr->pua_ptr_cs_change = pcri_mcs;
    }
    else
    {
      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = FALSE;
    }
  }
  else if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
  {
    uint8 pcri_cs = 0;

    (void)gmac_get_cs_from_pkt_cs_rel(pcri_ptr,&pcri_cs,gas_id);

    MSG_GERAN_MED_3_G("t_cs_reconfig_flg %d ,l_cs_setting %d, PCRI cs %d",
                      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag,
                      global_data_ptr->gmac_local_cs_setting,pcri_cs);

    if (global_data_ptr->gmac_local_cs_setting != pcri_cs)
    {
      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = TRUE;
      transfer_data_ptr->pua_ptr_cs_change = pcri_cs;
    }
    else
    {
      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = FALSE;
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GMAC PCRI, Bad TBF Mode: 0x%x",global_data_ptr->current_tbf_mode);
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_tran_edtm_populate_cs_rel_config_msgs()
===
===  DESCRIPTION
===  Populates messages associated with the Packet CS Release Ind procedure.
===  Messages populated: MAC_L1_EDTM_CS_RELEASE_ALLOC (mac_l1_edtm_cs_rel_alloc_msg)
===                      MR_UL_ALLOC_CNF (gmac_transfer_ul_rlc_info_store)
===
===  Updates Global data:
===    gmac_re_assign_ul_tfi
===    gmac_re_assign_dl_tfi
===    gmac_current_mac_mode
===    gmac_edtm_old_ul_ws
===    gmac_edtm_old_dl_ws
===    current_egprs_ul_ws
===    current_egprs_dl_ws
===    gmac_store_dl_rlc_mode
===
===  RETURN VALUE
===    NONE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void gmac_tran_edtm_populate_cs_rel_config_msgs(packet_cs_release_ind_t *pcri_ptr,
                                                       boolean                  ctrl_ack_set,
                                                       gas_id_t                 gas_id)
{
  ul_tbf_assign_t     *pcri_ul_tbf_ass_ptr = &pcri_ptr->multiple_ul_assignment.ul_tbf_assignments[0];
  dl_tbf_assign_t     *pcri_dl_tbf_ass_ptr = &pcri_ptr->multiple_dl_assignments[0].dl_tbf_assignments[0];
  pwr_ctrl_paramtrs_t *pcri_pwr_cntl_ptr   = NULL;
  uint8                ul_egprs_ws         = 0;
  uint8                ul_ts_alloc         = 0;
  uint16               decoded_ul_ws       = 0;
  uint8                coding_scheme       = 0;
  uint8                dl_egprs_ws         = 0;
  gmac_global_data_t  *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  memset(&transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg,
          0,
          sizeof(transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg));

  /*************************************************************/
  /***** Populate the MAC_L1_EDTM_CS_RELEASE_TBF_ALLOC message *****/
  /*************************************************************/

  /**** tbf_type ****/
  if ( (pcri_ptr->num_mult_ul_assignments > 0) &&
       (pcri_ptr->num_mult_dl_assignments > 0) &&
       (ctrl_ack_set == FALSE) )
  {
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type = UL_DL_TBF_PRESENT;
  }
  else if (pcri_ptr->num_mult_ul_assignments > 0)
  {
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type = UL_TBF_PRESENT;
  }
  else if ( (pcri_ptr->num_mult_dl_assignments > 0) &&
            (ctrl_ack_set == FALSE) )
  {
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type = DL_TBF_PRESENT;
  }
  else
  {
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type = NO_TBF_PRESENT;
    MSG_GERAN_ERROR_3_G("GMAC EDTM Internal error, TBF type is NO_TBF_PRESENT, num_mult_ul_ass:0x%x, num_mult_dl_ass:0x%x, ctrl_ack:%d",
                        pcri_ptr->num_mult_ul_assignments ,pcri_ptr->num_mult_dl_assignments,ctrl_ack_set);
  }

  /**** pkt_ext_ta_present ****/
  if (pcri_ptr->extended_timing_advance_present)
  {
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.pkt_ext_ta.pkt_ext_ta_present = TRUE;
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.pkt_ext_ta.pkt_ext_ta = pcri_ptr->extended_timing_advance;
  }
  else
  {
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.pkt_ext_ta.pkt_ext_ta_present = FALSE;
  }

  /**************************************/
  /**** Set up the DL TBF parameters ****/
  /**************************************/

  if ( ( transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type == UL_DL_TBF_PRESENT )||
       ( transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type == DL_TBF_PRESENT ) )
  {
    /**** mac_mode ****/
    if ( pcri_ptr->num_mult_ul_assignments == 1 )
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.mac_mode = pcri_ptr->multiple_ul_assignment.extended_dynamic_allocation;
    }
    else
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
    }

    /**** ts_allocation ****/
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.ts_allocation = pcri_ptr->multiple_dl_assignments[0].timeslot_allocation;

    /**** downlink_pwr ****/
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.downlink_pwr.params_valid = FALSE; /* Not present in mult DL assignment */

    /**** tbf_start_time_valid ****/
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.tbf_start_time_valid = FALSE; /* No start time in Pkt CS Rel Ind */

    /**** ta_params ****/
    if (pcri_ptr->global_pkt_timing.ta_flag == valid)
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.ta_params.ta_value_ie.present = TRUE;
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.ta_params.ta_value_ie.ta_value = pcri_ptr->global_pkt_timing.ta_value;
    }
    else
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.ta_params.ta_value_ie.present = FALSE;
    }

    if (pcri_ptr->global_pkt_timing.dwnlink_ta_flag == valid)
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.ta_params.cont_ta_ie.present = TRUE;
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.ta_params.cont_ta_ie.ta_timeslot = pcri_ptr->global_pkt_timing.dwnlink_ta_ts_num;
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.ta_params.cont_ta_ie.tai = pcri_ptr->global_pkt_timing.dwnlink_ta_index;
    }
    else
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.ta_params.cont_ta_ie.present = FALSE;
    }

    /**** power_control ****/
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.power_control.present = FALSE; /* power control not in mult DL assignment */

    /**** dl_tfi ****/
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.dl_tfi_present = TRUE;
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.dl_tfi = pcri_dl_tbf_ass_ptr->tfi_assignment;

    /* Set up EGPRS fields */
    if (pcri_ptr->egprs_mode_tbfs == 1)
    {
      /**** bep_period2 ****/
      if (pcri_ptr->bep_period2_present)
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.bep_period2.present = TRUE;
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.bep_period2.bep_period2 = pcri_ptr->bep_period2;
      }
      else
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.bep_period2.present = FALSE;
      }

      /**** tbf_mode ****/
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.tbf_mode = TBF_MODE_EGPRS;

      /***** link_qual_meas_mode ****/
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.link_qual_meas_mode = pcri_ptr->link_quality_measurement_mode;

      /**** rlc_mode ****/
      if (pcri_dl_tbf_ass_ptr->downlink_rlc_mode == 0)
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.rlc_mode = RLC_ACK;
      }
      else
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.rlc_mode = RLC_UNACK;
      }
    }
    else
    {
      /**** tbf_mode ****/
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.dl_tbf.tbf_mode = TBF_MODE_GPRS;
    }
  }

  /**************************************/
  /**** Set up the UL TBF parameters ****/
  /**************************************/

  if ( ( transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type == UL_DL_TBF_PRESENT )||
       ( transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type == UL_TBF_PRESENT ) )
  {
    /**** usf ****/
    /* return already checked in validate funciton*/
    (void)gmac_get_usf_from_pkt_cs_rel(pcri_ptr,
                                      &transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg,
                                       gas_id);

    /**** power_control_params ****/
    if (pcri_ptr->multiple_ul_assignment.timeslot_description.power_control_included == TRUE)
    {
      pcri_pwr_cntl_ptr = &pcri_ptr->multiple_ul_assignment.timeslot_description.power_control_params;

      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.present   = TRUE;

      if ( ( pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation & 0x80 ) != 0)
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN0.present = TRUE;
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN0.gamma = pcri_pwr_cntl_ptr->gamma_tn0;
      }

      if ( ( pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation & 0x40 ) != 0)
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN1.present = TRUE;
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN1.gamma = pcri_pwr_cntl_ptr->gamma_tn1;
      }

      if ( ( pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation & 0x20 ) != 0)
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN2.present = TRUE;
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN2.gamma = pcri_pwr_cntl_ptr->gamma_tn2;
      }

      if ( ( pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation & 0x10 ) != 0)
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN3.present = TRUE;
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN3.gamma = pcri_pwr_cntl_ptr->gamma_tn3;
      }

      if ( ( pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation & 0x08 ) != 0)
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN4.present = TRUE;
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN4.gamma = pcri_pwr_cntl_ptr->gamma_tn4;
      }

      if ( ( pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation & 0x04 ) != 0)
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN5.present = TRUE;
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN5.gamma = pcri_pwr_cntl_ptr->gamma_tn5;
      }

      if ( ( pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation & 0x02 ) != 0)
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN6.present = TRUE;
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN6.gamma = pcri_pwr_cntl_ptr->gamma_tn6;
      }

      if ( ( pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation & 0x01 ) != 0)
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN7.present = TRUE;
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.gamma_TN7.gamma = pcri_pwr_cntl_ptr->gamma_tn7;
      }

      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.alpha = pcri_pwr_cntl_ptr->alpha;
    }
    else
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.power_control_params.present = FALSE;
    }

    /**** timing_advance_params ****/
    if (pcri_ptr->global_pkt_timing.ta_flag == valid)
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.timing_advance_params.ta_value_ie.present = TRUE;
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.timing_advance_params.ta_value_ie.ta_value = pcri_ptr->global_pkt_timing.ta_value;
    }
    else
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.timing_advance_params.ta_value_ie.present = FALSE;
    }

    if (pcri_ptr->global_pkt_timing.uplink_ta_flag == valid)
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.timing_advance_params.cont_ta_ie.present = TRUE;
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.timing_advance_params.cont_ta_ie.ta_timeslot = pcri_ptr->global_pkt_timing.uplink_ta_ts_num;
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.timing_advance_params.cont_ta_ie.tai = pcri_ptr->global_pkt_timing.uplink_ta_index;
    }
    else
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.timing_advance_params.cont_ta_ie.present = FALSE;
    }

    /**** usf granularity ****/
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.usf_granularity =  pcri_ul_tbf_ass_ptr->usf_granularity;

    /**** tbf_start_time_valid ****/
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.tbf_start_time_valid = FALSE; /* No starting time in PCRI msg */

    /**** dl_ctrl_param ****/
    if ( pcri_ptr->multiple_ul_assignment.p0_present == TRUE )
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.dl_ctrl_param.params_valid = TRUE;
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.dl_ctrl_param.po = pcri_ptr->multiple_ul_assignment.p0;
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.dl_ctrl_param.pr_mode = pcri_ptr->multiple_ul_assignment.pr_mode;
    }
    else
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.dl_ctrl_param.params_valid = FALSE;
    }

    /* Set up EGPRS fields */
    if (pcri_ptr->egprs_mode_tbfs == 1)
    {
      /**** bep_period2 ****/
      if (pcri_ptr->bep_period2_present)
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.bep_period2.present = TRUE;
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.bep_period2.bep_period2 = pcri_ptr->bep_period2;
      }
      else
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.bep_period2.present = FALSE;
      }

      /**** tbf_mode ****/
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.tbf_mode = TBF_MODE_EGPRS;

      /**** rlc_mode ****/
      if (global_data_ptr->gmac_local_stored_ul_chan_req.rlc_mode == GRLC_MODE_ACK)
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.rlc_mode = RLC_ACK;
      }
      else
      {
        transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.rlc_mode = RLC_UNACK;
      }
    }
    else
    {
      transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.tbf_mode = TBF_MODE_GPRS;
    }

    /**** test_mode ****/
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.test_mode = global_data_ptr->current_test_mode;

    /**** ts_offest ****/
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.ts_offset = global_data_ptr->gmac_test_mode_ts_offset;

    /**** srb mode ****/
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.srb_mode = global_data_ptr->gmac_test_mode_srb_mode;

    /**** mac_mode ****/
    transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.ul_tbf.mac_mode = pcri_ptr->multiple_ul_assignment.extended_dynamic_allocation;
  }


  /*************************************************************/
  /*********** Populate the MR_UL_ALLOC_CNF message ************/
  /*************************************************************/
  if ( ( transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type == UL_DL_TBF_PRESENT )||
       ( transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type == UL_TBF_PRESENT ) )
  {
    memset(&transfer_data_ptr->gmac_transfer_ul_rlc_info_store,0,sizeof(transfer_data_ptr->gmac_transfer_ul_rlc_info_store));

    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.sig = MR_UL_ALLOC_CNF;

    /**** tbf_mode ****/
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_mode = global_data_ptr->current_tbf_mode;

    /**** dtm_access ****/
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.dtm_access = FALSE;

    /* Get the UL timeslot alloc for use later*/
    (void)gmac_get_ul_ts_alloc_from_pkt_cs_rel(pcri_ptr, &ul_ts_alloc,gas_id);

    /* Set up EGPRS parameters */
    if (pcri_ptr->egprs_mode_tbfs == 1)
    {
      /**** mcs ****/
      /* return already checked in validate funciton*/
      (void)gmac_get_egprs_mcs_from_pkt_cs_rel(pcri_ptr,&transfer_data_ptr->gmac_transfer_ul_rlc_info_store.mcs,gas_id);

      /**** resegment ****/
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.resegment = pcri_ptr->resegment;

      /**** ws ****/
      /* return already checked in validate funciton*/
      (void)gmac_get_ul_ws_from_pkt_cs_rel(pcri_ptr, &ul_egprs_ws,gas_id);

      decoded_ul_ws = gmac_egprs_decode_ul_window_size(ul_egprs_ws, ul_ts_alloc,gas_id);
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.ws = decoded_ul_ws;
    }

    /**** access_mode ****/
    if ( pcri_ptr->multiple_ul_assignment.extended_dynamic_allocation == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC )
    {
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_mode = RM_EXT_DYNAMIC_ALLOC;
    }
    else
    {
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_mode = RM_DYNAMIC_ALLOC;
    }

    /**** access_phase ****/
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_phase = GMAC_NO_PHASE_ACCESS;

    /**** tbf_type ****/
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_type = RM_OPEN_ENDED_TBF;
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.blocks_granted = 0;

    /**** cs ****/
    if (pcri_ptr->egprs_mode_tbfs == 0)
    {
      /* return already checked in validate funciton*/
      (void)gmac_get_cs_from_pkt_cs_rel(pcri_ptr,&coding_scheme,gas_id);
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.cs = coding_scheme;
    }

    /**** ti ****/
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.ti = 0; /* Unused */

    /**** tlli ****/
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tlli = 0; /*Unused*/

    /**** tlli_block_cs ****/
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tlli_block_cs = RM_TLLI_USE_CS_COMMANDED;

    /**** retry ****/
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.retry = 0; /* Unused */

    /**** ul_tfi ****/
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.ul_tfi = pcri_ul_tbf_ass_ptr->tfi_assignment;

    /**** num_timeslots ****/
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.num_timeslots = gmac_utils_calculate_nts(ul_ts_alloc,gas_id);

    /**** final_alloc ****/
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.final_alloc = 0; /* Fixed alloc only */

    /**** usf_granularity ****/
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.usf_granularity = pcri_ul_tbf_ass_ptr->usf_granularity;
  }

  /*************************************************************/
  /**************** Populate Global Data ***********************/
  /*************************************************************/

  if ( ( transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type == UL_DL_TBF_PRESENT )||
       ( transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type == UL_TBF_PRESENT ) )
  {
    /**** gmac_re_assign_ul_tfi ****/
    global_data_ptr->gmac_re_assign_ul_tfi = pcri_ul_tbf_ass_ptr->tfi_assignment;

    /**** gmac_current_mac_mode ****/
    global_data_ptr->gmac_current_mac_mode = pcri_ptr->multiple_ul_assignment.extended_dynamic_allocation;

    /**** gmac_edtm_old_ul_ws ****/
    global_data_ptr->gmac_edtm_old_ul_ws = global_data_ptr->current_egprs_ul_ws; /* store before current_egprs_ul_ws is written
                                                  to in the next statement */

    /**** current_egprs_ul_ws ****/
    global_data_ptr->current_egprs_ul_ws = decoded_ul_ws;
  }

  if ( transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type == DL_TBF_PRESENT )
  {
    /**** gmac_current_mac_mode ****/
    global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
  }

  if ( ( transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type == UL_DL_TBF_PRESENT )||
       ( transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg.tbf_type == DL_TBF_PRESENT ) )
  {
    /**** gmac_re_assign_dl_tfi ****/
    global_data_ptr->gmac_re_assign_dl_tfi = pcri_dl_tbf_ass_ptr->tfi_assignment;

    /**** gmac_edtm_old_ul_ws ****/
    global_data_ptr->gmac_edtm_old_dl_ws = global_data_ptr->current_egprs_dl_ws; /* store before current_egprs_dl_ws is written
                                                  to in the next statement */

    /**** current_egprs_ul_ws ****/
    /* return already checked in validate funciton*/
    (void)gmac_get_dl_ws_from_pkt_cs_rel(pcri_ptr, &dl_egprs_ws,gas_id);

    global_data_ptr->current_egprs_dl_ws = gmac_egprs_decode_dl_window_size(dl_egprs_ws,
                                                                            pcri_ptr->multiple_dl_assignments[0].timeslot_allocation,
                                                                            gas_id);

    /**** gmac_store_dl_rlc_mode ****/
    global_data_ptr->gmac_store_dl_rlc_mode = pcri_dl_tbf_ass_ptr->downlink_rlc_mode;
  }
}



/*===========================================================================
===
===  FUNCTION      gmac_tran_edtm_validate_pkt_cs_rel_msg()
===
===  DESCRIPTION
===  Validates the content of the Packet CS Release message.
===  Performs multislot and frequency checking.
===
===  DEPENDENCIES
===    t_flags.dtm_active
===    gmac_edtm_enabled
===    gmac_current_mac_mode
===    current_tbf_mode
===
===  RETURN VALUE
===    gmac_pkt_cs_rel_result_t - indicates status of checking
===
===  SIDE EFFECTS
===  Frequency checking function updates: gmac_store_freq_params_in_use
===  Multislot class routine updates: gmac_local_ul_realloc_ts
===                                   gmac_local_ul_realloc_dl_ctrl_ts
===                                   gmac_local_dl_reassign_ts
===
===
===========================================================================*/
static gmac_pkt_cs_rel_result_t gmac_tran_edtm_validate_pkt_cs_rel_msg(packet_cs_release_ind_t *pcri_ptr,
                                                                       active_tbf_t             active_tbfs,
                                                                       boolean                  ctrl_ack_set,
                                                                       gas_id_t                 gas_id)
{
  /*local data*/
  gmac_pkt_cs_rel_result_t result = PCRI_PASS;
  uint8 ul_ts_alloc               = 0;
  uint8 dl_ts_alloc               = 0;
  uint8 local_eda                 = 0;
  uint8 ul_egprs_ws               = 0;
  uint8 dl_egprs_ws               = 0;
  uint8 coding_scheme             = 0;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  /*Check for enhanced CS Release indication*/
  if (pcri_ptr->enhanced_dtm_cs_release_ind == 0)
  {
    MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, ENHANCED_DTM_CS_REL_IND set to 0, Ignore msg");
    return PCRI_IGNORE_MSG;
  }

  /*Check that the Packet CS Release Indication is received in dual transfer mode*/
  if (transfer_data_ptr->t_flags.dtm_active == FALSE)
  {
    MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel received when MS not in dual transfer mode, Ignore msg");
    return PCRI_IGNORE_MSG;
  }

  /*Check that EDTM is supported by the MS*/
  if (global_data_ptr->gmac_edtm_enabled == FALSE)
  {
    MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel received when EDTM not supported by MS, Ignore msg");
    return PCRI_IGNORE_MSG;
  }

  /* Check if currently active TBFs are being released */
  if ( gmac_edtm_pkt_cs_rel_tbfs_releasing(active_tbfs, gas_id) == TRUE )
  {
    MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel received when TBFs releasing, AB release");
    return PCRI_ABNORMAL_REL;
  }

  /* Check that the Pkt CS Rel Ind assignment type is in range*/
  if ( (pcri_ptr->edtm_realloc_type == EDTM_MAINTAIN_EXISTING_TBFS) ||
       (pcri_ptr->edtm_realloc_type == EDTM_RESERVED_TREAT_AS_00) )
  {
    MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, Maintain existing TBFs");
    return PCRI_PASS;
  }
  else if (pcri_ptr->edtm_realloc_type == EDTM_CONVERT_TO_FULL_RATE)
  {
    MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, type 01 - convert to full rate, NOT supported, AB release");
    return PCRI_ABNORMAL_REL;
  }
  else if (pcri_ptr->edtm_realloc_type != EDTM_USE_ALLOCATION_PROVIDED)
  {
    MSG_GERAN_ERROR_1_G("GMAC Pkt CS rel, BAD edtm_realloc_type: 0x%x",pcri_ptr->edtm_realloc_type);
    return PCRI_IGNORE_MSG;
  }

  /*Check that no more than 1 multiple UL TBF structure is assigned*/
  if (pcri_ptr->num_mult_ul_assignments > 1)
  {
    MSG_GERAN_HIGH_1_G("GMAC Pkt CS Rel, %d mult UL TBFs assigned, AB release ",pcri_ptr->num_mult_ul_assignments);
    return PCRI_ABNORMAL_REL;
  }

  /*Check that only 1 UL TBF is assigned in the multiple structure*/
  if ( (pcri_ptr->num_mult_ul_assignments == 1) &&
       (pcri_ptr->multiple_ul_assignment.num_ul_tbf_assignments != 1) )
  {
    MSG_GERAN_HIGH_1_G("GMAC Pkt CS Rel, %d UL TBFs assigned, AB release ",pcri_ptr->multiple_ul_assignment.num_ul_tbf_assignments);
    return PCRI_ABNORMAL_REL;
  }

  /*Check that no more than 1 multiple DL TBF structure is assigned*/
  if (pcri_ptr->num_mult_dl_assignments > 1)
  {
    MSG_GERAN_HIGH_1_G("GMAC Pkt CS Rel, %d mult DL TBFs assigned, AB release ",pcri_ptr->num_mult_dl_assignments);
    return PCRI_ABNORMAL_REL;
  }

   /*Check that only 1 DL TBF is assigned in the multiple structure*/
  if ( (pcri_ptr->num_mult_dl_assignments == 1) &&
       (pcri_ptr->multiple_dl_assignments[0].num_dl_tbf_assignments != 1) )
  {
    MSG_GERAN_HIGH_1_G("GMAC Pkt CS Rel, %d DL TBFs assigned, AB release ", pcri_ptr->multiple_dl_assignments[0].num_dl_tbf_assignments);
    return PCRI_ABNORMAL_REL;
  }

  /*Ctrl Ack checking */
  global_data_ptr->current_rcvd_dl_ctrl_ack = ctrl_ack_set;

  if (pcri_ptr->num_mult_dl_assignments == 1)
  {
    if (ctrl_ack_set == TRUE)
    {
      /*T3192 must be running if CTRL_ACK is set*/
      if (grlc_dl_t3192_status(gas_id) != TMR_ACTIVE)
      {
        MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, DL TBF CTRL ACK set, T3192 NOT running, Ab Rel ");
        return PCRI_ABNORMAL_REL;
      }

      if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == FALSE)
      {
        MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, DL TBF CTRL ACK set, Current DL TFI NOT valid, Ab Rel");
        return PCRI_ABNORMAL_REL;
      }

      /*PCRI TFI must match the current active TFI*/
      if (pcri_ptr->multiple_dl_assignments[0].dl_tbf_assignments[0].tfi_assignment != global_data_ptr->gmac_local_tfi_store.dl_tfi)
      {
        MSG_GERAN_HIGH_2_G("GMAC Pkt CS Rel, DL TBF CTRL ACK set, TFIs do NOT match, PCRI DL TFI:0x%x, Current DL TFI:0x%x, Ab Rel ",
                           pcri_ptr->multiple_dl_assignments[0].dl_tbf_assignments[0].tfi_assignment,
                           global_data_ptr->gmac_local_tfi_store.dl_tfi);
        return PCRI_ABNORMAL_REL;
      }

      /*The DL TFI must be valid on the PACCH that the PCRI was received on*/
      if ( ( ((uint8)(0x80 >> global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot)) & global_data_ptr->gmac_local_dl_ts_alloc ) == 0 )
      {
        /* PCRI was NOT received on a valid DL timeslot */
        MSG_GERAN_HIGH_3_G("GMAC PCRI Ctrl Ack DL TFI on TS NOT in DL TBF, rxd TS:%d, DL TFI:%d, DL TS curr bmap:0x%x, Ab Rel",
                           global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot,
                           pcri_ptr->multiple_dl_assignments[0].dl_tbf_assignments[0].tfi_assignment,
                           global_data_ptr->gmac_local_dl_ts_alloc);

        return PCRI_ABNORMAL_REL;
      }
    }
    else
    {
      if (grlc_dl_t3192_status(gas_id) == TMR_ACTIVE)
      {
        MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, DL TBF CTRL ACK NOT set, T3192 running, AB release ");
        return PCRI_ABNORMAL_REL;
      }
    }
  }


  if (pcri_ptr->num_mult_ul_assignments == 1)
  {
    /* Check that the UL timeslots have been allocated correctly */
    if ( gmac_get_ul_ts_alloc_from_pkt_cs_rel(pcri_ptr,&ul_ts_alloc,gas_id) == FALSE )
    {
       MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, error in UL TS alloc, AB release ");
       return PCRI_ABNORMAL_REL;
    }

    /* Check that the USF can be retrieved from the PCRI msg */
    if (gmac_get_usf_from_pkt_cs_rel(pcri_ptr,
                                    &transfer_data_ptr->mac_l1_edtm_cs_rel_alloc_msg,
                                     gas_id) == FALSE )
    {
      MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, error getting USF from msg, AB release ");
      return PCRI_ABNORMAL_REL;
    }
  }


  /* Multislot class checking */

  /* Get the downlink timeslots*/
  if ( (pcri_ptr->num_mult_dl_assignments == 1) &&
       (ctrl_ack_set == FALSE) )
  {
    dl_ts_alloc = pcri_ptr->multiple_dl_assignments[0].timeslot_allocation;
  }
  else
  {
    dl_ts_alloc = 0;
  }

  /* Get the EDA mode */
  if (pcri_ptr->num_mult_ul_assignments == 1)
  {
    local_eda = pcri_ptr->multiple_ul_assignment.extended_dynamic_allocation;
  }
  else
  {
    local_eda = GMAC_MAC_MODE_DYNAMIC_ALLOC;
  }

  /* DTM MS class checking is set to FALSE as the TBF assignments in the PCRI will be connected
     when the CS TS is released */
#ifdef FEATURE_GPRS_PS_HANDOVER
  if ( gmac_util_multislot_class_check(ul_ts_alloc,
                                       dl_ts_alloc,
                                       local_eda,
                                       0,
                                       FALSE, PSHO_UPDATE_PARAMS_NOT_REQUIRED,
                                       gas_id ) == FALSE )
#else
  if ( gmac_util_multislot_class_check(ul_ts_alloc,
                                       dl_ts_alloc,
                                       local_eda,
                                       0,
                                       FALSE,
                                       gas_id ) == FALSE )
#endif /*FEATURE_GPRS_PS_HANDOVER*/
  {
    MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, error in multislot class, AB release ");
    return PCRI_ABNORMAL_REL;
  }

  /* Check for EGPRS mode change, EGPRS to GPRS */
  if ( ( pcri_ptr->egprs_mode_tbfs == 0 ) &&
       ( global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS ) )
  {
    MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, change in TBF mode, Current: EGPRS, Pkt CS Rel: GPRS, AB release ");
    return PCRI_ABNORMAL_REL;
  }

  /* Check for EGPRS mode change, GPRS to EGPRS */
  if ( ( pcri_ptr->egprs_mode_tbfs == 1 ) &&
       ( global_data_ptr->current_tbf_mode == TBF_MODE_GPRS ) )
  {
    MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, change in TBF mode, Current: GPRS, Pkt CS Rel: EGPRS, AB release ");
    return PCRI_ABNORMAL_REL;
  }

  /* DL EGPRS window size checking */
  if ( (pcri_ptr->num_mult_dl_assignments == 1) &&
       (ctrl_ack_set == FALSE ) &&
       (pcri_ptr->egprs_mode_tbfs == 1) )
  {
    if ( gmac_get_dl_ws_from_pkt_cs_rel(pcri_ptr, &dl_egprs_ws, gas_id) == FALSE )
    {
      MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, DL EGPRS window size not present, AB release ");
      return PCRI_ABNORMAL_REL;
    }

    if ( gmac_egprs_decode_dl_window_size(dl_egprs_ws,dl_ts_alloc, gas_id) == WS_ALLOCATION_INVALID )
    {
      MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, DL EGPRS window size invalid, AB release ");
      return PCRI_ABNORMAL_REL;
    }
  }


  if ( (pcri_ptr->num_mult_ul_assignments == 1) &&
       (pcri_ptr->egprs_mode_tbfs == 1) )
  {
    /* UL EGPRS window size checking */
    if ( gmac_get_ul_ws_from_pkt_cs_rel(pcri_ptr, &ul_egprs_ws, gas_id) == FALSE )
    {
      MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, UL EGPRS window size not present, AB release ");
      return PCRI_ABNORMAL_REL;
    }

    if ( gmac_egprs_decode_ul_window_size(ul_egprs_ws, ul_ts_alloc, gas_id) == WS_ALLOCATION_INVALID )
    {
      MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, UL EGPRS window size invalid, AB release ");
      return PCRI_ABNORMAL_REL;
    }

    /* UL EGPRS MCS checking */
    if (gmac_get_egprs_mcs_from_pkt_cs_rel(pcri_ptr,&coding_scheme, gas_id) == FALSE)
    {
      MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, UL EGPRS MCS missing, AB release ");
      return PCRI_ABNORMAL_REL;
    }
  }


  /* Check that coding scheme is present if the PCRI is not for EGPRS */
  if ((pcri_ptr->num_mult_ul_assignments == 1) &&
      (pcri_ptr->egprs_mode_tbfs == 0))
  {
    if (gmac_get_cs_from_pkt_cs_rel(pcri_ptr,&coding_scheme, gas_id) == FALSE)
    {
      MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, error in coding scheme, AB release");
      return PCRI_ABNORMAL_REL;
    }
  }

  /* Check the freq params if present */
  if (pcri_ptr->freq_params_present)
  {
    if (gmac_util_frequency_param_ie(FALSE,FALSE, gas_id) != NO_ERRORS )
    {
      MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, error in freq params, AB release");
      return PCRI_ABNORMAL_REL;
    }
  }

  return result;
}


/*===========================================================================
===
===  FUNCTION      gmac_get_cs_from_pkt_cs_rel()
===
===  DESCRIPTION
===    Gets the coding scheme (cs) from the Pkt CS Rel Ind message.  If the CS is
===    present in the Multiple Uplink Assignment Struct then this is taken in preference
===    to the CS in the main body of the Pkt CS Rel Ind msg.
===    This function gets the CS if only 1 TBF is defined.
===    The following must be checked before calling this function (not checked here
===    to avoid repeated checking):
===      pcri_ptr->edtm_realloc_type == EDTM_USE_ALLOCATION_PROVIDED
===      pcri_ptr->num_mult_ul_assignments == 1
===      pcri_ptr->multiple_ul_assignment.num_ul_tbf_assignments == 1
===      pcri_ptr->egprs_mode_tbfs == 0
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE - PASS, FALSE - FAIL
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_get_cs_from_pkt_cs_rel(packet_cs_release_ind_t *pcri_ptr,
                                           uint8                   *cs,
                                           gas_id_t                 gas_id)
{
  boolean status = TRUE;

  if (pcri_ptr->multiple_ul_assignment.ul_tbf_assignments[0].channel_coding_present)
  {
    *cs = pcri_ptr->multiple_ul_assignment.ul_tbf_assignments[0].channel_coding_command;
  }
  else if (pcri_ptr->channel_coding_present)
  {
    *cs = pcri_ptr->channel_coding_command;
  }
  else
  {
    status = FALSE;
  }

  return status;
}


/*===========================================================================
===
===  FUNCTION      gmac_get_egprs_mcs_from_pkt_cs_rel()
===
===  DESCRIPTION
===    Gets the EGPRS MCS from the Pkt CS Rel Ind message.  If the MCS is
===    present in the Multiple Uplink Assignment Struct then this is taken in preference
===    to the MCS in the main body of the Pkt CS Rel Ind msg.
===    This function gets the MCS if only 1 TBF is defined.
===    The following must be checked before calling this function (not checked here
===    to avoid repeated checking):
===      pcri_ptr->edtm_realloc_type == EDTM_USE_ALLOCATION_PROVIDED
===      pcri_ptr->num_mult_ul_assignments == 1
===      pcri_ptr->multiple_ul_assignment.num_ul_tbf_assignments == 1
===      pcri_ptr->egprs_mode_tbfs == 1
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE - PASS, FALSE - FAIL
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_get_egprs_mcs_from_pkt_cs_rel(packet_cs_release_ind_t *pcri_ptr,
                                                  uint8                   *egprs_mcs,
                                                  gas_id_t                 gas_id)
{
  boolean status = TRUE;

  if (pcri_ptr->multiple_ul_assignment.ul_tbf_assignments[0].egprs_channel_coding_present)
  {
    *egprs_mcs = pcri_ptr->multiple_ul_assignment.ul_tbf_assignments[0].egprs_channel_coding_command;
  }
  else if (pcri_ptr->egprs_channel_coding_present)
  {
    *egprs_mcs = pcri_ptr->egprs_channel_coding_command;
  }
  else
  {
    status = FALSE;
  }

  if (status == TRUE)
  {
    if ( gmac_util_egprs_mcs_valid(*egprs_mcs, gas_id) == FALSE )
    {
      MSG_GERAN_ERROR_0_G("GMAC info PKT CS REL EGPRS MCS invalid");
      status = FALSE;
    }
  }

  return status;
}

/*===========================================================================
===
===  FUNCTION      gmac_get_ul_ws_from_pkt_cs_rel()
===
===  DESCRIPTION
===    Gets the UL EGPRS window size from the Pkt CS Rel Ind message.  If the WS is
===    present in the Multiple Uplink Assignment Struct then this is taken in preference
===    to the window size in the main body of the Pkt CS Rel Ind msg.
===    This function gets the window size if only 1 TBF is defined.
===    The following must be checked before calling this function (not checked here
===    to avoid repeated checking):
===      pcri_ptr->edtm_realloc_type == EDTM_USE_ALLOCATION_PROVIDED
===      pcri_ptr->num_mult_ul_assignments == 1
===      pcri_ptr->multiple_ul_assignment.num_ul_tbf_assignments == 1
===      pcri_ptr->egprs_mode_tbfs == 1
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE - PASS, FALSE - FAIL
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_get_ul_ws_from_pkt_cs_rel(packet_cs_release_ind_t *pcri_ptr,
                                              uint8                   *ul_ws,
                                              gas_id_t                 gas_id)
{
  boolean status = TRUE;

  if (pcri_ptr->multiple_ul_assignment.ul_tbf_assignments[0].ul_egprs_window_size_present)
  {
    *ul_ws = pcri_ptr->multiple_ul_assignment.ul_tbf_assignments[0].ul_egprs_window_size;
  }
  else if (pcri_ptr->ul_egprs_window_size_present)
  {
    *ul_ws = pcri_ptr->ul_egprs_window_size;
  }
  else
  {
    status = FALSE;
  }

  return status;
}


/*===========================================================================
===
===  FUNCTION      gmac_get_dl_ws_from_pkt_cs_rel()
===
===  DESCRIPTION
===    Gets the DL EGPRS window size from the Pkt CS Rel Ind message.  If the WS is
===    present in the Multiple Dplink Assignment Struct then this is taken in preference
===    to the window size in the main body of the Pkt CS Rel Ind msg.
===    This function gets the window size if only 1 TBF is defined.
===    The following must be checked before calling this function (not checked here
===    to avoid repeated checking):
===      pcri_ptr->edtm_realloc_type == EDTM_USE_ALLOCATION_PROVIDED
===      pcri_ptr->num_mult_dl_assignments == 1
===      pcri_ptr->multiple_dl_assignment.num_dl_tbf_assignments == 1
===      pcri_ptr->egprs_mode_tbfs == 1
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE - PASS, FALSE - FAIL
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_get_dl_ws_from_pkt_cs_rel(packet_cs_release_ind_t *pcri_ptr,
                                              uint8                   *dl_ws,
                                              gas_id_t                 gas_id)
{
  boolean status = TRUE;

  if (pcri_ptr->multiple_dl_assignments[0].dl_tbf_assignments[0].dl_egprs_window_size_present)
  {
    *dl_ws = pcri_ptr->multiple_dl_assignments[0].dl_tbf_assignments[0].dl_egprs_window_size;
  }
  else if (pcri_ptr->dl_egprs_window_size_present)
  {
    *dl_ws = pcri_ptr->dl_egprs_window_size;
  }
  else
  {
    status = FALSE;
  }

  return status;
}


/*===========================================================================
===
===  FUNCTION      gmac_get_usf_from_pkt_cs_rel()
===
===  DESCRIPTION
===    Gets the usf from the Pkt CS Rel Multiple UL Assignment
===    structure.  The usf_allocation decoded by CSN is mapped into the
===    mac_l1_edtm_cs_rel_alloc_t usf array using the
===    assigned timeslots (see Table 11.2.37.2 44.060).  The usf_allocation can
===    either be contain different USFs or the same USF for all timeslots.
===    The following must be checked before calling this function (not checked here
===    to avoid repeated checking):
===      pcri_ptr->edtm_realloc_type == EDTM_USE_ALLOCATION_PROVIDED
===      pcri_ptr->num_mult_ul_assignments == 1
===      pcri_ptr->multiple_ul_assignment.num_ul_tbf_assignments == 1
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE - PASS, FALSE - FAIL
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_get_usf_from_pkt_cs_rel(packet_cs_release_ind_t        *pcri_ptr,
                                            mac_l1_edtm_cs_rel_tbf_alloc_t *edtm_cs_realloc_ptr,
                                            gas_id_t                        gas_id)
{
  uint8            ul_ts_alloc     = 0;
  ul_tbf_assign_t *pcri_ul_tbf_ptr = &pcri_ptr->multiple_ul_assignment.ul_tbf_assignments[0];
  uint8            num_ts_alloc    = 0;
  uint8            usf_count       = 0;
  boolean          finding_next_ts = FALSE;
  uint8            ts_offset       = 0;

  /* Get the timeslot allocation from the PCRI msg */
  if ( gmac_get_ul_ts_alloc_from_pkt_cs_rel(pcri_ptr, &ul_ts_alloc, gas_id) == FALSE )
  {
    MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, Error getting UL TS");
    return FALSE;
  }

  /* Check that the number of timeslots in the allocation is consistent with the number of USF allocations */
  if ( pcri_ul_tbf_ptr->usf_allocation_per_slot == 1)
  {
    num_ts_alloc = gmac_utils_calculate_nts(ul_ts_alloc, gas_id);

    if ( num_ts_alloc != pcri_ul_tbf_ptr->num_usf_allocations )
    {
      MSG_GERAN_HIGH_2_G("GMAC Pkt CS Rel, Num of UL TS(%d) != Num of USF alloc(%d)",num_ts_alloc,pcri_ul_tbf_ptr->num_usf_allocations);
      return FALSE;
    }
  }
  else
  {
    num_ts_alloc = gmac_utils_calculate_nts(ul_ts_alloc, gas_id);
  }

  memset(edtm_cs_realloc_ptr->ul_tbf.usf,0,sizeof(edtm_cs_realloc_ptr->ul_tbf.usf));

  /* Start offset at TS 0 */
  ts_offset = 0;

  /* count through the number of timeslots allocated */
  for ( usf_count=0; usf_count < num_ts_alloc; usf_count++)
  {
    finding_next_ts = TRUE;

    /* find the next allocated timeslot */  
    while ((finding_next_ts) && (ts_offset <= 7))
    {
      if ( (ul_ts_alloc & (0x80 >> ts_offset) ) != 0 )
      {
        if (pcri_ul_tbf_ptr->usf_allocation_per_slot == 1 )
        {
          /* Different USF assigned to each allocated timeslots */
          edtm_cs_realloc_ptr->ul_tbf.usf[ts_offset].usf_value = pcri_ul_tbf_ptr->usf_allocation[usf_count];
        }
        else
        {
          /* Same USF is assigned to all allocated timeslots */
          edtm_cs_realloc_ptr->ul_tbf.usf[ts_offset].usf_value = pcri_ul_tbf_ptr->usf_allocation[0];
        }

        edtm_cs_realloc_ptr->ul_tbf.usf[ts_offset].present = TRUE;

        /* exit while loop and assign next usf value */
        finding_next_ts = FALSE;
      }

      /* Reached last offset at TS 7 */
      if (ts_offset == 7)
      {
        /* Reached the last timeslot, check that the usf_count for loop is going to exit */
        if ( (usf_count+1) != num_ts_alloc)
        {
          MSG_GERAN_ERROR_2_G("GMAC Pkt CS Rel, Internal failure in usf routine, usf_count:%d, num_ts_alloc:%d",usf_count,num_ts_alloc);
          return FALSE;
        }
        finding_next_ts = FALSE;
      }
      else
      {
        /* increment through TS 0 to TS 7 */
        ts_offset++;
      }
    }
  }

  return TRUE;
}


/*===========================================================================
===
===  FUNCTION      gmac_get_ul_ts_alloc_from_pkt_cs_rel()
===
===  DESCRIPTION
===    Gets the timeslot allocation from the Pkt CS Rel Multiple UL Assignment
===    structure.  This function gets the timeslot if only 1 TBF is
===    defined.
===    The following must be checked before calling this function (not checked here
===    to avoid repeated checking):
===      pcri_ptr->edtm_realloc_type == EDTM_USE_ALLOCATION_PROVIDED
===      pcri_ptr->num_mult_ul_assignments == 1
===      pcri_ptr->multiple_ul_assignment.num_ul_tbf_assignments == 1
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE - PASS, FALSE - FAIL
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_get_ul_ts_alloc_from_pkt_cs_rel(packet_cs_release_ind_t *pcri_ptr,
                                                    uint8                   *ts_alloc,
                                                    gas_id_t                 gas_id)
{
  boolean local_ts_set                  = FALSE;
  boolean finding_next_global_ts  = FALSE;
  uint8 ts_mask                   = 0;
  uint8 num_ts_counter            = 0;
  uint8 global_ts_offset          = 0;
  ul_tbf_assign_t *ul_tbf_ass_ptr = &pcri_ptr->multiple_ul_assignment.ul_tbf_assignments[0];

  *ts_alloc = 0;

  /***** validate variables ******/

  /* Check that Global Pkt TS description for UL is present*/
  if (pcri_ptr->multiple_ul_assignment.timeslot_description_present == FALSE)
  {
    MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, Global TS desc is not present");
    return FALSE;
  }

  /* Check that at least one TS has been assigned */
  if (pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation == 0)
  {
    MSG_GERAN_HIGH_0_G("GMAC Pkt CS Rel, Global TS desc contains no timeslots");
    return FALSE;
  }

  /* Check that the number of TS in the Global TS Alloc is consistent with num_timeslots from CSN */
  if ( gmac_utils_calculate_nts(pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation,gas_id)
       != ul_tbf_ass_ptr->num_timeslots )
  {
    MSG_GERAN_HIGH_2_G("GMAC Pkt CS Rel, num of TS in glob != num_timeslots, num_timeslots:0x%x, global_ts_alloc:0x%x",
                       ul_tbf_ass_ptr->num_timeslots,pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation);
    return FALSE;
  }

  /*TBF_TIMESLOT_ALLOCTION in Uplink_TBF_Assignment_struct is present*/
  if (ul_tbf_ass_ptr->timeslot_allocation_present == TRUE)
  {
    global_ts_offset = 7;

    /* set the start position for the TBF_TIMESLOT_ALLOCTION based on the number of timeslots*/
    ts_mask = (uint8)(0x01 << (ul_tbf_ass_ptr->num_timeslots - 1));

    /* Loop around the number of TS in the TBF_TIMESLOT_ALLOCTION */
    for (num_ts_counter = 0; num_ts_counter < ul_tbf_ass_ptr->num_timeslots; num_ts_counter++)
    {
      if ( (ul_tbf_ass_ptr->timeslot_allocation & ts_mask) == 0 )
      {
        local_ts_set = 0;
      }
      else
      {
        local_ts_set = 1;
      }

      ts_mask = ts_mask >> 1;

      finding_next_global_ts = TRUE;

      while (finding_next_global_ts)
      {
        /* found next global ts descr allocated timeslot */
        if ( (pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation &
             (0x01 << global_ts_offset)) != 0 )
        {
          /* Set the time slot from the global ts descr. */
          *ts_alloc |= (uint8)(local_ts_set << (global_ts_offset));
          finding_next_global_ts = FALSE;
        }

        if ( global_ts_offset > 0 )
        {
          global_ts_offset--;
        }
        else
        {
          finding_next_global_ts = FALSE;
        }
      }
    }
  }
  else
  {
    /* set TS from global time slot descr*/
    *ts_alloc = pcri_ptr->multiple_ul_assignment.timeslot_description.ms_timeslot_allocation;
  }

  return TRUE;
}


/*===========================================================================
===
===  FUNCTION      gmac_edtm_pkt_cs_rel_tbfs_releasing()
===
===  DESCRIPTION
===    Determines if the currently active TBFs are being released.
===
===  DEPENDENCIES
===    t_flags.gmac_tran_rel_state
===
===  RETURN VALUE
===    boolean - TRUE - TBFs are being released
===              FALSE - TBFs are not being released
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_edtm_pkt_cs_rel_tbfs_releasing(active_tbf_t active_tbfs,
                                                   gas_id_t     gas_id)
{
  boolean tbfs_releasing = FALSE;
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  /* If the UL is abnormally releasing then always abnormally release the Pkt CS Rel Request.
     This is consistent with approach taken in gmactransfer where all PACCH assignments
     are blocked when the UL is abnormally releasing */
  if ( transfer_data_ptr->t_flags.gmac_tran_rel_state == UL_AB )
  {
    MSG_GERAN_HIGH_2_G("GMAC EDTM UL AB releasing, AB CS rel, rel_state:0x%x, active_tbfs:0x%x",
                       transfer_data_ptr->t_flags.gmac_tran_rel_state,active_tbfs);
    tbfs_releasing = TRUE;
  }
  else if ( active_tbfs == UL_AND_DL )
  {
    if ( ( transfer_data_ptr->t_flags.gmac_tran_rel_state == UL_DL_NORM ) ||
         ( transfer_data_ptr->t_flags.gmac_tran_rel_state == UL_DL_AB ) )
    {
      MSG_GERAN_HIGH_2_G("GMAC EDTM UL+DL releasing, AB CS rel, rel_state:0x%x, active_tbfs:0x%x",
                         transfer_data_ptr->t_flags.gmac_tran_rel_state,active_tbfs);
      tbfs_releasing = TRUE;
    }
  }
  else if ( active_tbfs == UL_ONLY )
  { /* UL_AB case already caught above*/
    if ( transfer_data_ptr->t_flags.gmac_tran_rel_state == UL_NORM )
    {
      MSG_GERAN_HIGH_2_G("GMAC EDTM UL releasing, AB CS rel, rel_state:0x%x, active_tbfs:0x%x",
                         transfer_data_ptr->t_flags.gmac_tran_rel_state,active_tbfs);
      tbfs_releasing = TRUE;
    }
  }
  else if ( active_tbfs == DL_ONLY )
  {
    if ( ( transfer_data_ptr->t_flags.gmac_tran_rel_state == DL_NORM ) ||
         ( transfer_data_ptr->t_flags.gmac_tran_rel_state == DL_AB ) )
    {
      MSG_GERAN_HIGH_2_G("GMAC EDTM DL releasing, AB CS rel, rel_state:0x%x, active_tbfs:0x%x",
                         transfer_data_ptr->t_flags.gmac_tran_rel_state,active_tbfs);
      tbfs_releasing = TRUE;
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GMAC bad active TBFs value: 0x%x",active_tbfs);
  }

  return tbfs_releasing;
}

#endif /*FEATURE_GSM_EDTM*/


/*===========================================================================
===
===  FUNCTION      gmac_convert_transfer_substate_to_string()
===
===  DESCRIPTION
===  Converts the supplied transfer state (of type gprs_mac_transfer_substates_t) into a string.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    char *  - transfer substate string
===
===  SIDE EFFECTS
===
===========================================================================*/
char * gmac_convert_transfer_substate_to_string( gprs_mac_transfer_substates_t transfer_state,
                                                 gas_id_t                      gas_id )
{
  char *s = "";

  switch ( transfer_state )
  {
    case TRANSFER_INACTIVE:
      s = "TRANSFER_INACTIVE";
    break;

    case TRANSFER_DL:
      s = "TRANSFER_DL";
    break;

    case TRANSFER_DL_REASSIGN:
      s = "TRANSFER_DL_REASSIGN";
    break;

    case TRANSFER_DL_WITH_DYNAMIC_ALLOC:
      s = "TRANSFER_DL_WITH_DYNAMIC_ALLOC";
    break;

    case TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC:
      s = "TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC";
    break;

    case TRANSFER_UL_DYNAMIC:
      s = "TRANSFER_UL_DYNAMIC";
    break;

    case TRANSFER_UL_DYNAMIC_REALLOC:
      s = "TRANSFER_UL_DYNAMIC_REALLOC";
    break;

    case TRANSFER_UL_DYNAMIC_DL_ASSIGN:
      s = "TRANSFER_UL_DYNAMIC_DL_ASSIGN";
    break;

    case TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN:
      s = "TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN";
    break;

    case TRANSFER_CONCURRENT_DYNAMIC_TRANSFER:
      s = "TRANSFER_CONCURRENT_DYNAMIC_TRANSFER";
    break;

    case TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN:
      s = "TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN";
    break;

    case TRANSFER_CONCURRENT_DYNAMIC_REALLOC:
      s = "TRANSFER_CONCURRENT_DYNAMIC_REALLOC";
    break;

    case TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC:
      s = "TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC";
    break;

#ifdef FEATURE_GPRS_PS_HANDOVER
    case TRANSFER_PS_HANDOVER:
      s = "TRANSFER_PS_HANDOVER";
    break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

    default:
      MSG_GERAN_ERROR_1_G("GMAC Transfer substate invalid: 0x%x",transfer_state);
    break;
  }

  return s;
}

/*===========================================================================
===
===  FUNCTION      gmac_tran_get_tbfs_active()
===
===  DESCRIPTION
===   This function can be called externally to transfer in
===   order to determine which TBFs are active.  Note: A TBF is only
===   active when L1 has indicated that it has been connected.
===   This function should only be called when in Transfer
===   mode.  It will send an ERROR F3 if not.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    active_tbf_t
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
active_tbf_t gmac_tran_get_tbfs_active( gas_id_t gas_id )
{
  active_tbf_t active_tbf = DL_ONLY;
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  switch (transfer_data_ptr->mac_transfer_substate)
  {
    case TRANSFER_DL:
    case TRANSFER_DL_REASSIGN:
    case TRANSFER_DL_WITH_DYNAMIC_ALLOC:
    case TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC:
      active_tbf = DL_ONLY;
    break;

    case TRANSFER_UL_DYNAMIC:
    case TRANSFER_UL_DYNAMIC_REALLOC:
    case TRANSFER_UL_DYNAMIC_DL_ASSIGN:
    case TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN:
      active_tbf = UL_ONLY;
    break;

    case TRANSFER_CONCURRENT_DYNAMIC_TRANSFER:
    case TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN:
    case TRANSFER_CONCURRENT_DYNAMIC_REALLOC:
    case TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC:
      active_tbf = UL_AND_DL;
    break;

    default:
      MSG_GERAN_ERROR_1_G("GMAC gmac_tran_tbfs_active called in bad state:0x%x",transfer_data_ptr->mac_transfer_substate);
    break;
  }

  return active_tbf;
}

/*===========================================================================
===
===  FUNCTION      gmac_tran_handle_unexpected_rlc_tbf_req()
===
===  DESCRIPTION
===  Handles a RM_UL_EST_TBF_REQ signal in states where an UL request is not expected.
===  This function only displays error messages, no action is performed.
===
===  DEPENDENCIES
===    c_flags.tbf_release_pending_flg

===  RETURN VALUE
===    void
===
===  SIDE EFFECTS
===
===========================================================================*/
static void gmac_tran_handle_unexpected_rlc_tbf_req( gas_id_t gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  if (gmac_util_is_rlc_blocked(GMAC_TRAN, __LINE__,gas_id) == BLOCKED )
  {
    /* RLC has attempted access when we have blocked it.  This might
       be a race condition or an error in RLC.
       Because a race condition is a possibility we have to assume that this has happened
       in this case */
    MSG_GERAN_HIGH_0_G("GMAC RLC attempted access whilst blocked, ignore msg, assume race condition");
  }
  else
  {
    if (global_data_ptr->c_flags.tbf_release_pending_flg)
    {
      /* RLC should have been blocked if we are pending a TBF release.  Flag as error.
         Cause of this needs to be found */
      MSG_GERAN_ERROR_0_G("GMAC tbf_release_pending_flg set, RLC not blocked, Ignore RLC req");
    }
    else
    {
      /* RLC is already accessing or has access, so we shouldn't receive another UL est req */
      MSG_GERAN_ERROR_0_G("GMAC RLC sent second RM_UL_EST_TBF_REQ whilst unblocked, ignore msg");
    }
  }
}


/*===========================================================================
===
===  FUNCTION    gmac_tran_get_transfer_state()
===
===  DESCRIPTION
===  This is an access function for external entities to get the mac_transfer_substate.
===
===  PARAMS
===
===
===  RETURN VALUE
===  gprs_mac_transfer_substates_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
gprs_mac_transfer_substates_t gmac_tran_get_transfer_state(gas_id_t gas_id)
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  return transfer_data_ptr->mac_transfer_substate;
}

/*===========================================================================
===
===  FUNCTION    gmac_tran_get_t_flags()
===
===  DESCRIPTION
===  This is an access function for external entities to get t_flags.
===
===  PARAMS
===
===
===  RETURN VALUE
===  transfer_flags_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
transfer_flags_t gmac_tran_get_t_flags(gas_id_t gas_id)
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  return transfer_data_ptr->t_flags;
}

/*===========================================================================
===
===  FUNCTION      gmac_tran_handle_pdch_release
===
===  DESCRIPTION
===  Send PDCH release to L1 with signal MAC_L1_PDCH_REL_TS.
===
===  DEPENDENCIES
===  timeslot_available - timeslot bitmap
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_tran_handle_pdch_release(uint8 timeslot_available,
                                   active_tbf_t active_tbf,
                                   gas_id_t     gas_id)
{
  mac_l1_sig_t         mac_l1_sig;      /* mac to l1 signal   */
  boolean              send_pdch_rel = FALSE;
  boolean              releasing_l1 = FALSE;

#ifdef FEATURE_GSM_GPRS_MSC33
  uint8                second_lowest_ts = 0;
  uint8                lowest_ts_bitmap = 0;
  uint8                lowest_ts = 0;
#endif /*FEATURE_GSM_GPRS_MSC33*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  MSG_GERAN_MED_3_G("MAC Rcvd PACKET_PDCH_RELEASE ts_avail 0x%x ul ts 0x%x dl ts 0x%0x",
                    timeslot_available,global_data_ptr->gmac_local_ul_ts_alloc,global_data_ptr->gmac_local_dl_ts_alloc);

#ifdef FEATURE_GSM_GPRS_MSC33
  if ( gmac_utils_calculate_ts_alloc_ws(global_data_ptr->gmac_local_ul_ts_alloc,gas_id) == GMAC_SHIFTED_USF_NUM_OF_UL_TS)
  {
    /* An UL window size of 5 means Shifted USF is being used */

    second_lowest_ts = gmac_utils_get_second_lowest_ts_allocated(global_data_ptr->gmac_local_ul_ts_alloc,gas_id);

    /* If the second lowest allocated TS is released by the Pkt PDCH Rel then the lowest
       allocated TS is also implicitly released */
    if (((0x80 >> second_lowest_ts) & timeslot_available) == 0)
    {
      lowest_ts = gmac_utils_get_lowest_ts_allocated(global_data_ptr->gmac_local_ul_ts_alloc,gas_id);

      MSG_GERAN_MED_2_G("GMAC Shifted USF monitor released TS: %d, also releasing lowest TS: %d",second_lowest_ts,lowest_ts);

      lowest_ts_bitmap = 0x80 >> lowest_ts;

      /* Remove the lowest allocated TS */
      timeslot_available = timeslot_available & (~lowest_ts_bitmap);
    }
  }
#endif /*FEATURE_GSM_GPRS_MSC33*/

  /* Note UL_DL cause is set when this function is called from T_DL_WITH_DYN and T_DL_REASS_DYN_ALLOC.
     In these states the UL has not yet been connected and as a result the gmac_local_ul_ts_alloc will
     be 0.  Therefore the UL will always be released in these states regardless of whether the TS is
     still available.
     The reason behind this is that L1 cannot handle the MAC_L1_PDCH_REL_TS signal when it is waiting to connect
     the UL TBF.  So MAC only has the option to release the UL TBF.  Fixing this might be problematic
     for L1 because of L1 ISR/TASK context issues.
   */

  if(active_tbf == UL_AND_DL)
  {
   if(
     ((global_data_ptr->gmac_local_ul_ts_alloc & timeslot_available) == 0)
     && ((global_data_ptr->gmac_local_dl_ts_alloc & timeslot_available) == 0)
     )
   {
     /*release UL and DL*/
      gmac_transfer_dl_release_clean(gas_id);
      gmac_transfer_ul_dynamic_release_clean(gas_id);

      gmac_util_block_rlc(NO_DEL, GMAC_UTIL, __LINE__,gas_id);
      gmac_util_send_dl_release_to_rlc(gas_id);
      gmac_tran_send_mac_l1_release(UL_AND_DL_TBF_ABNORMAL,gas_id);

      /*update TS avalible*/
      global_data_ptr->gmac_local_ul_ts_alloc = 0;
      global_data_ptr->gmac_local_dl_ts_alloc = 0;

      releasing_l1 = TRUE;

      /*set flag*/
      transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg = TRUE;
   }
   else
   {
     /*Not all allocated TS are being removed, determine IF ul or DL is being removed*/
     if(
         ( (global_data_ptr->gmac_local_ul_ts_alloc != 0) && (global_data_ptr->gmac_local_ul_ts_alloc & timeslot_available) == 0)
       )
     {
       /*Ul being removed*/
       gmac_transfer_ul_dynamic_release_clean(gas_id);

       gmac_util_block_rlc(NO_DEL, GMAC_UTIL, __LINE__, gas_id);
       gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL, gas_id);

       /*update TS avalible*/
       global_data_ptr->gmac_local_ul_ts_alloc = 0;
       global_data_ptr->gmac_local_dl_ts_alloc = global_data_ptr->gmac_local_dl_ts_alloc & timeslot_available;

       send_pdch_rel = TRUE;
       releasing_l1 = TRUE;

     }
     else if(
              ( (global_data_ptr->gmac_local_dl_ts_alloc !=0) && (global_data_ptr->gmac_local_dl_ts_alloc & timeslot_available) == 0)
            )
     {
       /*DL being removed*/
       gmac_transfer_dl_release_clean(gas_id);
       gmac_util_send_dl_release_to_rlc(gas_id);
       gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL, gas_id);

       /*update TS avalible*/
       global_data_ptr->gmac_local_dl_ts_alloc = 0;
       global_data_ptr->gmac_local_ul_ts_alloc = global_data_ptr->gmac_local_ul_ts_alloc & timeslot_available;

       send_pdch_rel = TRUE;
       releasing_l1 = TRUE;
     }
     else
     {
       /*removed TS does not cause UL or DL to be released*/
       global_data_ptr->gmac_local_dl_ts_alloc = global_data_ptr->gmac_local_dl_ts_alloc & timeslot_available;
       global_data_ptr->gmac_local_ul_ts_alloc = global_data_ptr->gmac_local_ul_ts_alloc & timeslot_available;

       send_pdch_rel = TRUE;
     }
   }
  }
  else if(active_tbf == UL_ONLY)
  {
     if((global_data_ptr->gmac_local_ul_ts_alloc & timeslot_available) == 0)
     {
        gmac_transfer_ul_dynamic_release_clean(gas_id);
        gmac_util_block_rlc(NO_DEL, GMAC_UTIL, __LINE__, gas_id);
        gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL, gas_id);

        /*update TS avalible*/
        global_data_ptr->gmac_local_ul_ts_alloc = 0;
        releasing_l1 = TRUE;
     }
     else
     {
        /*update ul ts in use*/
        global_data_ptr->gmac_local_ul_ts_alloc = global_data_ptr->gmac_local_ul_ts_alloc & timeslot_available;
        send_pdch_rel = TRUE;
     }
  }
  else if(active_tbf == DL_ONLY)
  {
     if((global_data_ptr->gmac_local_dl_ts_alloc & timeslot_available) == 0)
     {
       gmac_transfer_dl_release_clean(gas_id);
       gmac_util_send_dl_release_to_rlc(gas_id);

       /*If RLC started UL est (via PDAN) and is waiting for */
       /*PRR then need to release RLC ul                     */


       gmac_transfer_ul_tbf_abnormal_abort(NO_DEL, gas_id);

       gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL, gas_id);

       global_data_ptr->gmac_local_dl_ts_alloc = 0;
       releasing_l1 = TRUE;
     }
     else
     {
       /*update dl ts in use*/
       global_data_ptr->gmac_local_dl_ts_alloc = global_data_ptr->gmac_local_dl_ts_alloc & timeslot_available;
       send_pdch_rel = TRUE;
     }
  }


  if(releasing_l1 == TRUE)
  {
    /* missing from 94182 */
    global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
    global_data_ptr->mac_action = NONE;

    if(
       (active_tbf == UL_AND_DL )
       && (global_data_ptr->gmac_local_ul_ts_alloc == 0)
       && (global_data_ptr->gmac_local_dl_ts_alloc == 0 )
      )
    {
      transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg = TRUE;
    }
  }

  MSG_GERAN_MED_2_G("MAC info PDCH rel abnormal_ul_and_dl_rel_flg =%d, tbf_release_pending_flg %d",
                    transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg,
                    global_data_ptr->c_flags.tbf_release_pending_flg);



  if(send_pdch_rel == TRUE)
  {
    MSG_GERAN_MED_1_G("MAC Sent MAC_L1_PDCH_REL_TS, ts_available = 0x%x",timeslot_available);
    mac_l1_sig.mac_l1_pdch_rel_ts.ts_allocation = timeslot_available;
    mac_l1_sig.mac_l1_pdch_rel_ts.message_header.message_set = MS_MAC_L1;
    mac_l1_sig.mac_l1_pdch_rel_ts.message_header.message_id = (uint8)MAC_L1_PDCH_REL_TS;
    gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
  }

}

/*===========================================================================
===
===  FUNCTION    gmac_tran_send_mac_l1_release()
===
===  DESCRIPTION
===  Send MAC_L1_RELEASE to L1 to release its physical resources
===
===  PARAMS
===
===    tbf_release_desc: UL or|and DL TBF release indicator
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_tran_send_mac_l1_release
(
    release_tbf_t                tbf_release_desc,
    gas_id_t                     gas_id
)
{
  mac_l1_sig_t         mac_l1_sig;      /* mac to l1 signal   */
  gmac_global_data_t   *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

#ifdef FEATURE_GSM_DTM
  if ( global_data_ptr->mac_mode == TRANSFER_MODE)
  {
    gmac_tran_rel_state_handle_rel_req( tbf_release_desc, gas_id );
  }
#endif /*FEATURE_GSM_DTM*/

  if( (tbf_release_desc == DL_TBF_ABNORMAL) || (tbf_release_desc == UL_AND_DL_TBF_ABNORMAL))
  {
    switch(transfer_data_ptr->mac_transfer_substate)
    {
      /*restrict to DL only state*/
      case TRANSFER_DL:
      case TRANSFER_DL_REASSIGN:
        transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_dl_release = TRUE;
        MSG_GERAN_MED_2_G("MAC info block dl msg (dl ab) =%d rel cause = %d",
          transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_dl_release,tbf_release_desc);
      break;

      default:
        /*no action*/
      break;
    }
  }

  if(tbf_release_desc == UL_TBF_NORMAL)
  {
    switch(transfer_data_ptr->mac_transfer_substate)
    {
      /*restrict to UL only state*/
      case TRANSFER_UL_DYNAMIC:
      case TRANSFER_CONCURRENT_DYNAMIC_TRANSFER:
        transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release = TRUE;
        MSG_GERAN_MED_2_G("MAC info block dl msg (ul norm) =%d rel cause = %d",
          transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release,tbf_release_desc);
      break;

      default:
        /*no action*/
      break;
    }
  }

  if( (tbf_release_desc == UL_TBF_ABNORMAL) || (tbf_release_desc == UL_AND_DL_TBF_ABNORMAL))
  {
   transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release = TRUE;
   MSG_GERAN_MED_2_G("MAC info block dl msg(ul ab) =%d rel casue = %d",
                     transfer_data_ptr->t_flags.block_dl_msg_during_abnormal_ul_release,tbf_release_desc);
  }

  if((tbf_release_desc == UL_TBF_ABNORMAL) || (tbf_release_desc == UL_TBF_NORMAL))
  {
    /* when in concurrency the is an opertunity for a Dl release will occur before the UL has been confirmed*/
    /* under these conditions the release pending flag must no be reset */
    MSG_GERAN_MED_1_G("mac info wait_for_ul_rel_confirm = %d",global_data_ptr->c_flags.wait_for_ul_rel_confirm);
    global_data_ptr->c_flags.wait_for_ul_rel_confirm = TRUE;
  }
/*event logging changes*/
  mac_log_packet_tbf_rel_event( tbf_release_desc, gas_id );

  MSG_GERAN_MED_1_G("MAC Sent MAC_L1_RELEASE %d", tbf_release_desc);
  mac_l1_sig.mac_l1_release.release_tbf = tbf_release_desc;
  mac_l1_sig.mac_l1_release.message_header.message_set = MS_MAC_L1;
  mac_l1_sig.mac_l1_release.message_header.message_id = (uint8)MAC_L1_RELEASE;
  gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
}

/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_pts_on_pacch_rrbp()
===
===  DESCRIPTION
===  Function used by transfer to send a pkt ms tbf_status during an active
===  downlink tbf. The pkt ms tbf_status is sent in a PACCH RRBP message to
===  the network
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
void gmac_transfer_send_pts_on_pacch_rrbp(uint8 ts, uint8 rrbp, uint32 fn,
                                          gas_id_t gas_id)
{
  mac_l1_sig_t        ml_rrbp_sig;

  mac_l1_pacch_rrbp_t *rrbp_sig_ptr = &(ml_rrbp_sig.mac_l1_pacch_rrbp);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
/*tlx pemr logging changes for idle cases */
  MSG_GERAN_ERROR_0_G("MAC Sent MAC_L1_PACCH_RRBP");

  rrbp_sig_ptr->rrbp_msg = PMR_MSG;
  rrbp_sig_ptr->ts_of_dl_msg = ts;
  rrbp_sig_ptr->absolute_fn = gmac_rrbp_absolute_fn(fn, rrbp, gas_id);
  rrbp_sig_ptr->msg_burst_type = NB;

  /* copy the CSN.1 encoded PKT_TBF_STATUS into the PACCH_RRBP message */
  memscpy(rrbp_sig_ptr->pacch_ab_data,sizeof(rrbp_sig_ptr->pacch_ab_data),
          transfer_data_ptr->gmac_local_pts_params.csn_encoded_pts,
          SIZE_OF_CONTROL_RADIO_BLOCK);

#ifdef FEATURE_GSM_EDTM
  ml_rrbp_sig.mac_l1_pacch_rrbp.message_type = PACKET_MOBILE_TBF_STATUS;
#endif /*FEATURE_GSM_EDTM*/

  ml_rrbp_sig.mac_l1_pacch_rrbp.message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
  ml_rrbp_sig.mac_l1_pacch_rrbp.message_header.message_set = MS_MAC_L1;
  gprs_l1_mac_acc_func(&ml_rrbp_sig, gas_id);

  transfer_data_ptr->gmac_local_pts_params.pts_pending = FALSE;

  /* call log-on-demand function */
  (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                   PACKET_MOBILE_TBF_STATUS,
                                   (byte)(sizeof(packet_mobile_tbf_status_t)),
                                   &(rrbp_sig_ptr->pacch_ab_data[1]),
                                    gas_id);
}

/*===========================================================================
===
===  FUNCTION    gmac_transfer_get_transfer_state()
===
===  DESCRIPTION
===  This is an access function for external entities to get the mac_transfer_substate.
===
===  PARAMS
===
===
===  RETURN VALUE
===  gprs_mac_transfer_substates_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
gprs_mac_transfer_substates_t gmac_transfer_get_transfer_state(gas_id_t gas_id)
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  return transfer_data_ptr->mac_transfer_substate;
}
/*** EOF: don't remove! ***/
