/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state_offline.c_v   1.2   20 May 2002 11:48:02   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_state_offline.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/20/02   kwa     Added output_name function.
09/19/02   kwa     Added function to process the cm_act_req message.
05/21/03   kwa     Restructured code to eliminate lint warnings.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
11/19/03   kwa     Removed obsolete function for retrieving the state name.
05/17/04   kwa     Added function to process the mmr_camped_ind message.
10/16/04   kwa     Added the functions process_cm_network_list_req,
                   process_cm_service_req, and process_cm_sim_available_req
                   to generate an ERR_FATAL if CM attempts to reacquire service
                   after placing the phone in the offline state.
02/22/05   up      Updated for Cingular ENS feature.
                   Updated state table to add new function pointer for FCN IND.
09/19/05   up      Updated state table for new primitive CM_HPLMN_SEARCH_REQ.
11/07/05   ajt     Added handling of MMR_EMERGENCY_NUM_LIST_IND in offline state.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "err.h"
#include "msg.h"
#include "reg_send.h"
#include "reg_state.h"
#include "naslog_v.h"
#include "reg_timers.h"
#include "reg_nv.h"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  process_cm_network_list_req

==============================================================================*/

static reg_state_p_type process_cm_network_list_req
(
  const cm_network_list_req_s_type* msg_p
)
{
  (void) msg_p;

  MSG_FATAL_DS(REG_SUB, "=REG= CM_NETWORK_LIST_REQ - Unexpected", 0, 0, 0);

/*lint -e{527} Reviewed with the process group and recommended for suppression */
  return (&reg_state_offline);
}


/*==============================================================================

FUNCTION NAME

  process_cm_service_req

==============================================================================*/

static reg_state_p_type process_cm_service_req
(
  const cm_service_req_s_type* msg_p
)
{
  (void) msg_p;

  MSG_FATAL_DS(REG_SUB, "=REG= CM_SERVICE_REQ - Unexpected", 0, 0, 0);
/*lint -e{527} Reviewed with the process group and recommended for suppression */
  return (&reg_state_offline);
}


/*==============================================================================

FUNCTION NAME

  process_cm_sim_available_req

==============================================================================*/

static reg_state_p_type process_cm_sim_available_req
(
  cm_sim_available_req_s_type* msg_p
)
{
  (void) msg_p;

  MSG_FATAL_DS(REG_SUB, "=REG= CM_SIM_AVAILABLE_REQ - Unexpected", 0, 0, 0);

/*lint -e{527} Reviewed with the process group and recommended for suppression */
  return (&reg_state_offline);
}


/*==============================================================================

FUNCTION NAME

  process_mmr_stop_mode_cnf

==============================================================================*/

static reg_state_p_type process_mmr_stop_mode_cnf
(
  mmr_stop_mode_cnf_s_type* msg_p
)
{
  (void) msg_p;

  reg_send_cm_stop_mode_cnf
  (
    reg_state_cm_transaction_id_get()
  );

  return (&reg_state_offline);
}


/*==============================================================================

FUNCTION NAME

  process_mmr_ph_status_change_cnf

==============================================================================*/

static reg_state_p_type process_mmr_ph_status_change_cnf
(
  mmr_ph_status_change_cnf_s_type* msg_p
)
{
  (void) msg_p;
  
  reg_send_cm_ph_status_chgd_cnf
  (
    reg_state_cm_transaction_id_get(),
    SYS_PH_STATUS_CHGD_SUCCESS
  );
  
  
  return (&reg_state_offline);
}

/*==============================================================================

FUNCTION NAME

  process_cm_set_hplmn_irat_timer

==============================================================================*/
static reg_state_p_type process_cm_set_hplmn_irat_timer
(
  const cm_set_hplmn_irat_search_timer_req_s_type *msg_p
)
{
  
  uint32 timer_val = msg_p->timer_value;

  MSG_HIGH_DS_1(REG_SUB, "=REG= CM sent HPLMN IRAT timer  - %lu(milli-sec)", timer_val);   
  if(timer_val == 0)
  {
    timer_val = REG_TIMERS_MAX_TIMER_VALUE;    
  }

  reg_nv_hplmn_irat_search_timer_set(timer_val);

  return (&reg_state_offline);
}



/*==============================================================================

FUNCTION NAME

  number

==============================================================================*/

static reg_state_e_type number ( void )
{
  return REG_STATE_OFFLINE;
}


/*==============================================================================

FUNCTION NAME

  output_name

==============================================================================*/

static void output_name ( void )
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_OFFLINE");
}


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

reg_state_s_type reg_state_offline = 
{
  process_cm_network_list_req,
  process_cm_service_req,
  process_cm_sim_available_req,
  reg_state_unexpected_cm_sim_not_available_req,
  reg_state_unexpected_cm_stop_mode_req,
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
  reg_state_ignore_hplmn_search_request,
#endif
  reg_state_process_cm_ph_status_chgd_req,
#ifdef FEATURE_LTE
  reg_state_unexpected_cm_set_drx_req,
#endif 
  reg_state_unexpected_mmr_camped_ind,
  reg_state_process_mmr_attach_failed_ind,                                        
  reg_state_unexpected_mmr_mm_information_ind,
  reg_state_unexpected_mmr_plmn_search_cnf,
  reg_state_unexpected_mmr_reg_cnf,
  reg_state_unexpected_mmr_service_ind,
  reg_state_unexpected_mmr_sim_available_cnf,
  reg_state_unexpected_mmr_sim_not_available_cnf,
  process_mmr_stop_mode_cnf,
  reg_state_unexpected_hplmn_search_timer_expiration,
  reg_state_process_mmr_emergency_num_list_ind,
  reg_state_unexpected_sim_refresh_fcn_ind,
  process_mmr_ph_status_change_cnf,
  reg_state_unexpected_mmr_reg_reject_ind,
  reg_state_unexpected_hplmn_irat_search_timer_expiration,
  process_cm_set_hplmn_irat_timer,
  reg_state_t3245_exp_clean_up,
#ifdef FEATURE_LTE
  reg_state_unexpected_backoff_fplmn_timer_expiration,
  reg_state_process_cm_block_plmn_req,
  reg_state_ignore_update_lte_cap_timer_expiration,
#endif
#ifdef FEATURE_SGLTE
  reg_state_unexpected_sglte_ps_timer_expiry,
#endif
  reg_state_unexpected_lte_irat_search_timer_expiration,
#ifdef FEATURE_FEMTO_CSG
  reg_state_process_unexpected_mmr_csg_list_read_complete,
#endif 
  reg_state_unexpected_mmr_reg_req_ignored_ind,
  number,
  output_name
};

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/
