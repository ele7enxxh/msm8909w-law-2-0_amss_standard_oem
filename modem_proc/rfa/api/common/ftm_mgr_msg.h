#ifndef FTM_MSGR_MSG_H
#define FTM_MSGR_MSG_H
/*!
  @file
  ftm_mgr_msg.h

  @brief
  Message router UMIDs header for FTM
*/

/*===========================================================================

  Copyright (c) 2009 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/ftm_mgr_msg.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/12/14   stw     Add Multiple Scell Support in NS mode
05/13/14   kab     Added Get DL BLER support for all carriers
05/13/14   kab     Added RF-ML1 interface and support for NS CA
03/25/14   bsh     Ported CL: 5493095
02/12/13   pl      remove redundant RFA_RF messages
11/20/12   gvn     Featurize LTE for Triton
07/12/12   pl      Moved FTM command processing to FTM task
                   removed obsolete RFA_RF_LTE messages
05/22/12   aca     Added FTM command to extract RX AGC data
03/19/12   php     Added LTE TDD NS support  
02/29/12   yzw     Added LTE FTM command FTM_LTE_SET_LNA_STATE_DBM
02/07/12   can     Added FTM set modulation command.
10/17/11   ars     Added FTM_RF_GET_RX_LEVEL_DBM command
10/17/11   ars     Added FTM_SET_PA_STATE and FTM_SET_TX_POWER_DBM FTM Layer
09/14/11   can     Merge MDM9K tips to NikeL.
06/27/11   ars     Added FTM_SET_TX_POWER_DBM 
06/23/11   pl      Remove obsolete interface : set_rx_on
04/20/11   pl      Remove obsolete message structures
03/31/11   sar     Removed conditional definitions for CMI-4.
03/14/11   pl      Adding Sleep and Wakeup Command for LTE FTM
02/09/11   aca     FTM dac setpoint value override support
01/31/11   aro     Renamed the Header protection feature (conflict with ftm_msh.h) 
02/08/11   aca     FTM modulation change support
12/21/10   whc     FTM Command for SMPS Override
12/07/10   mkv     LTE NS End Call implementation complete. 
10/30/10   mkv     Added support for LTE Get Rx Level       
10/29/10   mkv     Added support for LTE Reset and Get Sync BLER API       
10/26/10   mkv     Added LTE NS Tx AGC override        
10/24/10   mkv     Added LTE NS UL Grant reconfiguration 
09/28/10   whc     Added txagc oride cmd_s's to ftm_msg_u
07/27/10   pl      Remove measurement FTM command handler to Measurement module
06/24/10   pl      Adding PDCP start/stop for LTE Non-signaling support
06/07/10   pl      Added FTM support for LTE IM2 CAL
06/04/10   pl      Adding x->L IRAT FTM command  
05/17/10   pl      Adding L2 Start/Stop support
05/06/10   pl      Adding FTM_LTE_NS_IS_CONN_REQ/CNF support
03/25/10   pl      Replace lte_cphy_test_msg.h wth lte_cphy_ftm_msg.h
03/18/10   pl      Integrating with L1 Mode change message
03/16/10   pl      Merge FTM_LTE_NS_START_DL and FTM_LTE_NS_START_UL to become FTM_LTE_NS_START_DP  
03/12/10   pl      Replaced place holder with real PDCP messages 
03/10/10   pl      Remove MIB and SIB-1 related code 
03/02/10   pl      Adding LTE_CPHY_ABORT_CNF to message unions 
03/02/10   pl      Adding place holder for new PDCP UL/DL messages  
02/18/10   pl      Added Set Secondary Chain message
02/18/10   pl      Added GET LNA offset message
11/22/09   mkv     Removed PA ON message and added the FTM Generic response. 
11/18/09   pl      Replace lte_cphy_sap.h with lte_cphy_msg.h
10/23/09   pl      Added  FTM LTE NS UL data support
10/21/09   mkv     Added TX-IQ mismatch items
10/13/09   mkv     Added Calibration message types
10/06/09   mkv     Merged with Bringup Changes
08/11/09   mkv     Merge MODEM_LTE_DEV into rfa2
07/29/09   pl      Added additional LTE L1 message to support START_DL
07/29/09   pl      Added Non-signaling START_DL message types
06/11/09   pl      Added LTE L1 messages 
05/15/09   mkv	   Added RFA RF LTE FTM Calibration response types
05/01/09   pl      Added Non-signalling message types
02/25/09   mkv     Initial Check-in

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msgr.h>
#include <msgr_types.h>
#include <ftm_msgr.h>
#include <appmgr_msg.h>

#ifdef FEATURE_LTE
/*Header file for the FTM LTE NS message and structure definitions*/
#include "ftm_lte_ns_msg.h"
/*Header file for the RFA RF LTE FTM Calibration message and structure definitions*/
#include "rflte_msg.h"
#include "rf_lte_ftm_msg.h"
/*Header file for the LTE L1/L2 message and structure definitions*/
#include <lte_cphy_msg.h>
#include <lte_cphy_ftm_msg.h>
#include <lte_mac_rrc.h>
#include <lte_rlc_rrc.h>

#endif /*FEATURE_LTE*/
/* Add more header file below as FTM messages from other modules are included */
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/****************************************************************************
                        Internal FTM MSGR messages
****************************************************************************/

/*! @brief UMIDs for FTM 
*/
enum {

  MSGR_DEFINE_UMID(FTM, CORE, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
		   msgr_spr_loopback_s),

  MSGR_DEFINE_UMID(FTM, CORE, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY, 
                   msgr_spr_loopback_reply_s),

  MSGR_DEFINE_UMID(FTM, CORE, SPR, THREAD_READY, APPMGR_ID_THREAD_READY, 
                   none),

  MSGR_DEFINE_UMID(FTM, CORE, SPR, THREAD_KILL, APPMGR_ID_THREAD_KILL, 
                   none)
};

/*! @brief Union of all msgs that FTM can receive
*/
typedef union
{
  /* Supervisor Requests */
  msgr_spr_loopback_s          msgr_spr_loopback;
  msgr_spr_loopback_reply_s    msgr_spr_loopback_reply;

#ifdef FEATURE_LTE
  /*FTM LTE Calibration Messages*/
  rfa_rf_lte_ftm_configure_tx_sweep_cmd_s lte_cal_tx_sweep_msg;
  rfa_rf_lte_ftm_set_rx_iq_mismatch_coeft_s lte_cal_set_rx_iq_mismatch_coeft_msg;
  rfa_rf_lte_ftm_get_hdet_from_sweep_s  get_hdet_from_tx_sweep;
  rfa_rf_lte_ftm_oride_txagc_output_power_cmd_s override_txagc_output_power;
  rfa_rf_lte_ftm_oride_txagc_backoff_cmd_s override_txagc_backoff;
  rfa_rf_lte_path_sel_override_update_ind_s rfa_rf_lte_path_sel_override_update;  

  /*FTM LTE Calibration response messages*/
  rfa_rf_lte_ftm_generic_message_rsp_s  lte_cal_generic_rsp;

  /* FTM LTE Non-signaling messages */
  ftm_lte_ns_start_mode_req_msg_type   start_mode_req_msg;
  ftm_lte_ns_start_mode_cnf_msg_type   start_mode_cnf_msg;
  ftm_lte_ns_stop_mode_req_msg_type    stop_mode_req_msg;
  ftm_lte_ns_stop_mode_cnf_msg_type    stop_mode_cnf_msg;
  ftm_lte_ns_acq_req_msg_type          acq_req_msg;
  ftm_lte_ns_acq_cnf_msg_type          acq_cnf_msg;
  ftm_lte_ns_start_dp_req_msg_type     start_dp_req_msg;
  ftm_lte_ns_start_dp_cnf_msg_type     start_dp_cnf_msg;
  ftm_lte_ns_idle_req_msg_type         idle_req_msg;
  ftm_lte_ns_idle_cnf_msg_type         idle_cnf_msg;
  ftm_lte_ns_dl_data_ind_msg_type      dl_data_ind_msg;
  ftm_lte_ns_is_conn_req_msg_type      is_conn_req_msg;
  ftm_lte_ns_is_conn_cnf_msg_type      is_conn_cnf_msg;
  ftm_lte_ns_config_ul_waveform_req_msg_type config_ul_waveform_req_msg;
  ftm_lte_ns_config_ul_waveform_cnf_msg_type config_ul_waveform_cnf_msg;
  ftm_lte_ns_config_ul_power_req_msg_type config_ul_power_req_msg;
  ftm_lte_ns_config_ul_power_cnf_msg_type config_ul_power_cnf_msg;
  ftm_lte_ns_get_dl_level_req_msg_type get_dl_level_req_msg;
  ftm_lte_ns_get_dl_level_cnf_msg_type get_dl_level_cnf_msg;
  ftm_lte_ns_reset_dl_bler_req_msg_type reset_dl_bler_req_msg;
  ftm_lte_ns_reset_dl_bler_cnf_msg_type reset_dl_bler_cnf_msg;
  ftm_lte_ns_get_dl_bler_req_msg_type get_dl_bler_req_msg;
  ftm_lte_ns_get_dl_bler_cnf_msg_type get_dl_bler_cnf_msg;
  ftm_lte_ns_handover_req_msg_type handover_req_msg;
  ftm_lte_ns_handover_cnf_msg_type handover_cnf_msg;
  ftm_lte_ns_tdd_config_req_msg_type tdd_config_req_msg;
  ftm_lte_ns_tdd_config_cnf_msg_type tdd_config_cnf_msg;
  ftm_lte_ns_enable_scell_req_msg_type enable_scell_req_msg;
  ftm_lte_ns_enable_scell_cnf_msg_type enable_scell_cnf_msg;
  ftm_lte_ns_disable_scell_req_msg_type disable_scell_req_msg;
  ftm_lte_ns_disable_scell_cnf_msg_type disable_scell_cnf_msg;
  ftm_lte_ns_enable_multi_scell_req_msg_type enable_multi_scell_req_msg;
  ftm_lte_ns_enable_multi_scell_cnf_msg_type enable_multi_scell_cnf_msg;
  ftm_lte_ns_disable_multi_scell_req_msg_type disable_multi_scell_req_msg;
  ftm_lte_ns_disable_multi_scell_cnf_msg_type disable_multi_scell_cnf_msg;
  ftm_lte_ns_get_all_carr_dl_bler_req_msg_type get_all_carr_dl_bler_req_msg;
  ftm_lte_ns_get_all_carr_dl_bler_cnf_msg_type get_all_carr_dl_bler_cnf_msg;

  /* LTE L1 Messages to be handled */
  lte_cphy_start_cnf_s         lte_start_cnf_msg;
  lte_cphy_stop_cnf_s          lte_stop_cnf_msg;
  lte_cphy_abort_cnf_s         lte_abort_cnf_msg;
  lte_cphy_acq_cnf_s           lte_acq_cnf_msg;
  lte_cphy_cell_select_cnf_s   lte_cell_select_cnf_msg;
  lte_cphy_tdd_cfg_cnf_s       lte_tdd_cfg_cnf_msg; 
  lte_cphy_common_cfg_cnf_s    lte_common_cfg_cnf_msg;
  lte_cphy_rach_cfg_cnf_s	   lte_rach_cfg_cnf_msg;
  lte_cphy_dedicated_cfg_cnf_s lte_dedicated_cfg_cnf_msg;
  lte_cphy_test_l1_mode_change_cnf_s lte_mode_chg_cnf_msg;
  lte_cphy_test_get_all_carr_pdsch_stats_cnf_s lte_all_carr_pdsch_stats_cnf_msg;

  /* LTE L2 Messages to be handled */
  lte_mac_cfg_cnf_msg_s          lte_mac_cfg_cnf_msg;
  lte_mac_start_cnf_msg_s        lte_mac_start_cnf_msg;
  lte_mac_stop_cnf_msg_s         lte_mac_stop_cnf_msg;
  lte_rlc_cfg_cnf_s              lte_rlc_dl_cfg_cnf_msg;
  lte_rlc_start_cnf_s            lte_rlc_dl_start_cnf_msg;
  lte_rlc_stop_cnf_s             lte_rlc_dl_stop_cnf_msg;
  msgr_hdr_s                     lte_rlc_dl_data_ind_msg;
  lte_rlc_cfg_cnf_s              lte_rlc_ul_cfg_cnf_msg;
  lte_rlc_start_cnf_s            lte_rlc_ul_start_cnf_msg;
  lte_rlc_stop_cnf_s             lte_rlc_ul_stop_cnf_msg;
  msgr_hdr_struct_type           lte_pdcp_dl_exit_ftm_cnf_msg; 
  msgr_hdr_struct_type           lte_pdcp_dl_enter_ftm_cnf_msg;
  msgr_hdr_struct_type           lte_pdcp_ul_exit_ftm_cnf_msg; 
  msgr_hdr_struct_type           lte_pdcp_ul_enter_ftm_cnf_msg;
  msgr_hdr_struct_type           lte_pdcp_dl_start_cnf_msg; 
  msgr_hdr_struct_type           lte_pdcp_dl_stop_cnf_msg;
  msgr_hdr_struct_type           lte_pdcp_ul_start_cnf_msg; 
  msgr_hdr_struct_type           lte_pdcp_ul_stop_cnf_msg;
  // Add here as new external msgs are added 
#endif /*FEATURE_LTE*/
} ftm_msgr_u;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* FTM_MSGR_MSG_H */
