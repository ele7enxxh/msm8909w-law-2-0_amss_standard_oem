#ifndef TDSL1_MSGR_H
#define TDSL1_MSGR_H

/*===========================================================================

            W2LTE Message Router R E L A T E D   D E C L A R A T I O N S

DESCRIPTION 

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

REFERENCES

Copyright (c) 2001-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/inc/tdsl1msgr.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/11   asm     Added support for RF MSGR messages
01/25/11   cxd     Merge MSGR queues for L1 task
06/02/10   stk     Initial Version. Added support for W2LTE measurements and reselection 
                   under FEATURE_WCDMA_ABSOLUTE_PRIORITY & FEATURE_WCDMA_TO_LTE

===========================================================================*/
#include "tdscdma_variation.h"
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include <msgr.h>
#include <msgr_umid.h>  /* UMID definitions */
#include "tfw_sw_intf_msg.h"

#ifdef FEATURE_TDSCDMA_RF_SUPPORT
#include "rf_tdscdma_msg.h"
#endif

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#include "gl1_hw.h"

#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
#include "lte_cphy_irat_meas_msg.h"

#define MSGR_MODULE_L1                 0x01
#define MSGR_WCDMA_L1                  MSGR_TECH_MODULE( MSGR_TECH_WCDMA, MSGR_MODULE_L1 )

typedef lte_cphy_irat_meas_cleanup_cnf_s wl1_msgr_w2l_cleanup_cnf;
typedef lte_cphy_irat_meas_meas_cnf_s wl1_msgr_w2l_meas_cnf;

#endif /* (FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY) */

#include "tdsirat.h"

#include "wwan_coex_mgr.h"
#include "wcn_coex_mgr.h"
typedef cxm_freqid_info_ind_s tdsl1_msgr_cxm_freqid_ind;


typedef union
{
  /* Message Router Header field */
  msgr_hdr_s             msgr_hdr;
#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
  lte_cphy_irat_meas_cleanup_cnf_s cleanup_cnf;
  lte_cphy_irat_meas_search_cnf_s search_cnf;
  lte_cphy_irat_meas_meas_cnf_s meas_cnf;
    #if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
	lte_cphy_irat_meas_timed_srch_meas_cnf_s timed_meas_cnf;
	#endif
#endif /* (FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY) */
  tfw_srch_gap_detect_rsp_t gap_detect_rsp;
  tfw_srch_syncdl_midamble_detect_rsp_t syncdl_midamble_detect_rsp;
  //tdscsrchfw_abort_rsp_msg_t abort_rsp;
  tfw_measure_rscp_rsp_t rscp_rsp;
  tfw_measure_iscp_rsp_t iscp_rsp;
  tfw_measure_all_rscp_rsp_t rscp_all_rsp;
  tfw_cell_info_ind_t cell_info_ind;
  tfw_baton_handover_rsp_t fw_bho_rsp;
  tfw_srch_reacq_rsp_t srch_reacq_rsp;
  #ifdef FEATURE_JDS_OPT
  tfw_ts0_cell_rsp_t ts0_cell_rsp;
  tfw_nonts0_cell_rsp_t non_ts0_cell_rsp;
  #endif
  
#if 0  
  tfw_measure_unscheduled_stats_ind_t unscheduled_stats_ind;
#endif
  tfw_srch_power_scan_rsp_t power_scan_rsp;
  
  gsm_irat_acquisition_rsp_t gsm_acq_rsp; 
  gsm_irat_sch_burst_rsp_t gsm_sch_rsp;
  gsm_irat_meas_search_rsp_t gsm_meas_rsp;

//#ifdef FEATURE_TDSCDMA_PLT  
  tfw_state_cfg_rsp_t state_rsp;
//#endif
  tdsirat_msg_union_type irat_msg;
  
  tfw_ard_ind_t ard_ind ;
#ifdef FEATURE_TDSCDMA_FATAL_RECOVERY 
  tfw_antenna_switching_config_rsp_t asdiv_rsp;
#endif

  #ifdef FEATURE_TDSCDMA_DSDS_QTA  /*FEATURE_QTA*/
  tfw_dsds_cleanup_ind_t dsds_qta_cleanup_ind;
  #endif
  
#ifdef FEATURE_BOLT_MODEM
  tfw_rx_config_update_rsp_t rx_config_update_rsp;
#endif

  tdsl1_msgr_cxm_freqid_ind freqid_ind;
#ifdef FEATURE_TDSCDMA_BAND_AVOID 
  cxm_coex_ba_ind_s band_avoid_ind;
#endif  
  #if (defined (FEATURE_DIMEPM_MODEM) || defined (FEATURE_JOLOKIA_MODEM)) && defined (FEATURE_TDSCDMA_WLAN_COEX)
  cxm_coex_active_policy_ind_s wlan_coex_policy_ind;
  cxm_coex_metrics_req_s       wlan_coex_metrics_req;
  tfw_wlan_txfrmdnl_ind_t  tfw_txfrmdnl_ind;
  #endif
  
} tdsl1_msgr_cmd_code_type;

typedef struct
{
  q_link_type link;

  tdsl1_msgr_cmd_code_type cmd_code;

} tdsl1_msgr_cmd_type;

#endif /* TDSL1_MSGR_H */
