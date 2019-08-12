/*!
  @file rf_tdscdma_msg_proc.c 

  @brief
  This file dispacthes and sends responses for any REQ that TDSCDMA RF driver receives.



*/

/*===========================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Technologies Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/mc/src/rf_tdscdma_msg_proc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/15   whz     AutoPin on TA: online support
10/06/15   whz     Add support to AutoPin script
03/17/15   whz     Fix TempComp on Jolokia
12/05/14   jyu     Separate RF WAKEUP_STEP0 into 0p1 and 0p2 for wakeup timeline optimization
12/05/14   jyu     Code cleanup (remove D3925 feature)
11/10/14   jyu     Add message field to indicate if use_sw_tune_in_acq and to support 
                   CMAPI type 4 (for RxD override)
08/13/14   nsh     rf_tdscdma_mc_switch_ant need to return certain value for TL1
07/30/14   qzh     Enable FTM NS new intf and remove the dependency
07/17/14   ms      Sawless RX support
07/16/14   kb      Add support for HDET trigger API and register RFLM RF task commands
06/30/14   jyu     Return rebuild_ifreq_script back to L1 for DR-DSDS
05/22/14   jyu     Mainline the support for DSDS sleep/wakeup
05/05/14   jyu     Added support for DSDS sleep/wakeup
04/24/14   ych     Added dynamic tuner script for ifreq
04/11/14   jyu     Fix OFT errors
04/04/14   ych     Fix KW warnings
03/10/14   jyu     Added support for therm read
03/09/14   jyu     Enabled RF wakeup in parallel with MCPM
02/22/14   jyu     Mainline feature RFTDS_USE_L1_DEVICE_INFO and removed it
02/22/14   jyu     Added support to allow RF wakeup in parallel with MCPM
11/20/13   qzh     Featurized FTM NS new interface to disable it on BO 0.1
10/30/13   jyu     Removed FW interface
10/23/13   qzh     Fix KW warnings
10/18/13   ych     Added support for TX_CHAIN NO
10/09/13   qzh     Update Non-signaling R4&HSUPA code with new TL1-RFSW interface and function partition
10/04/13   jps     Fixes for IQ capture in FTM calibration mode
09/18/13   zg      Compiler warning cleanup
07/26/13   ms      AsDiv support
07/17/13   ms      Support for update tune code
06/30/13   qzh     Replace memcpy() with safer memscpy().
06/25/13   ms      Added div buff index for ifreq
06/03/13   nsh     Add support for Get Multi Synth in Dime
05/25/13   qzh     Porting TDSCDMA IQ capture from NIKEL to DIME
04/24/13   qzh     Porting FTM RF code from NIKEL to DIME
04/30/13   ms      Tuner temp comp support
04/18/13   jps     Remove unused field from F3
04/18/13   zg      Removed RSB calculation from IQ capture.
03/22/13   zg      More updated on IQ capture message processing
04/04/13   jyu     Added support for IFREQ measurement on Rx1 chain
03/03/13   zg      Added an included header file
03/22/13   zg      Updated IQ capture message processing
03/07/13   zg      Added support for IQ capture via TFW
01/25/13   ms      enabled RFTDS_USE_L1_DEVICE_INFO
11/29/12   av      Using proper featurization and removing featurization that is not needed
11/20/12   gvn     Featurize LTE for Triton 
10/05/12   av      Expanded rf task architecture to include more tasks. 
09/19/12   ms      Diversity support for wakeup and BHO
09/11/12   ms      off target compile error fix 
09/11/12   ms      Added message logging for L1-RF and RF-FW messages.
09/10/12   ms      Diversity support 
08/23/12   ms      Updated interface for primary and secondary device
08/13/12   jyu     Updated interface to take rfm_device_enum_type and for both 
                   primary and secondary device  
07/17/12   zg      KW fixes. 
04/07/12   ycl     Register for State Config RSP.
06/22/11   jhe     Initial version.   

===========================================================================*/

/*===========================================================================

                  INCLUDE FILES

===========================================================================*/
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
#define FEATURE_RF_TL1_FTM_NEW_INTF
#endif

#include <stringl/stringl.h>

#include "rfa_variation.h"
#include "amssassert.h"
#include "appmgr.h"
#include "msgr.h"
#include "rf_cap.h" /*Contains FEATURE_RF_TASK */

#ifdef FEATURE_LTE
#include "rf_control_msg.h"
#endif /*FEATURE_TRITON_MODEM*/

#include "rfa_msgr.h"
#include "rf_task.h"
#include "rfcom.h"
#include "rfm.h"
#include "rfcommon_core.h"
#include "rf_cmd_interface.h"
#include "rf_cmd_dispatcher.h"
#include "rf_tdscdma_msg.h"
#include "rf_tdscdma_mc.h"
#include "rf_tdscdma_mc_meas.h"
#include "rf_tdscdma_mdsp.h"
#include "rf_task_common_services.h"
#include "rflm_tds_msg.h"
#include "rflm_msg.h"
#include "rflm_tds_mc.h"

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
#include "tdsl1_ftm_msg.h"
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
#include "DALSys.h" /* DALSYS_BusyWait() */
#include "ftm_tdscdma_ctl.h"
#include "ftm_common_msm.h"

#include "rfdevice_cmn_intf.h"

extern ftm_tdscdma_iq_capture_data_type ftm_tds_iq_capture_data;

/*===========================================================================
              typedefs
===========================================================================*/
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
typedef enum
{
  RF_TDSCDMA_DISPATCH_SUCESSFUL=1,
  RF_TDSCDMA_DISPATCH_UNSUCESSFUL=2,
  RF_TDSCDMA_DISPATCH_NULL_PTR=3,
  RF_TDSCDMA_DISPATCH_INVALID_PARAM =4,
  RF_TDSCDMA_DISPATCH_MAX
}rf_tdscdma_dispatch_outcome_t;
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
typedef union
{
  rfa_tdscdma_enter_mode_req_s            rfa_tdscdma_enter_mode;
  rfa_tdscdma_exit_mode_req_s             rfa_tdscdma_exit_mode;
  rfa_tdscdma_enable_rx_req_s             rfa_tdscdma_enable_rx;
  rfa_tdscdma_init_tx_req_s               rfa_tdscdma_init_tx;
  rfa_tdscdma_enable_tx_req_s             rfa_tdscdma_enable_tx;
  rfa_tdscdma_disable_tx_req_s            rfa_tdscdma_disable_tx;
  rfa_tdscdma_sleep_req_s                 rfa_tdscdma_sleep;
  rfa_tdscdma_wakeup_req_s                rfa_tdscdma_wakeup;
  rfa_tdscdma_set_tx_pwr_limit_req_s      rfa_tdscdma_set_tx_pwr_limit;
  rfa_tdscdma_get_rf_warmup_time_req_s    rfa_tdscdma_get_rf_warmup_time;
  rfa_tdscdma_update_bho_req_s            rfa_tdscdma_update_bho;
  rfa_tdscdma_switch_buf_req_s            rfa_tdscdma_switch_buf;
  rfa_tdscdma_update_tds_nbr_req_s        rfa_tdscdma_update_tds_nbr;
  rfa_tdscdma_update_gsm_nbr_req_s        rfa_tdscdma_update_gsm_nbr;
  rfa_tdscdma_get_max_tx_pwr_req_s        rfa_tdscdma_get_max_tx_pwr;
  rfa_tdscdma_therm_backoff_req_s         rfa_tdscdma_therm_backoff;
  rfa_tdscdma_set_coex_tx_pwr_limit_req_s rfa_tdscdma_set_coex_tx_pwr_limit;
  rfa_tdscdma_get_rxagc_val_req_s         rfa_tdscdma_get_rxagc_val;
  rfa_tdscdma_get_agc_log_req_s           rfa_tdscdma_get_agc_log;
  rfa_tdscdma_get_rf_txchain_no_req_s     rfa_tdscdma_get_rf_txchain_no;
  rfa_tdscdma_dynamic_script_req_s        rfa_tdscdma_dynamic_script;  
  rfa_tdscdma_rxd_override_req_s          rfa_tdscdma_rxd_override; 
  
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
  // NS FTM
  msgr_hdr_s             msgr_hdr;
  tdsl1_ns_cphy_start_tdscdma_mode_cnf_t  tdsl1_ns_cphy_start_tdscdma_mode;
  tdsl1_ns_cphy_stop_tdscdma_mode_cnf_t   tdsl1_ns_cphy_stop_tdscdma_mode;
  tdsl1_ns_cphy_acq_cnf_t                 tdsl1_ns_cphy_acq;
  tdsl1_ns_cphy_setup_cnf_t               tdsl1_ns_cphy_setup;
  tdsl1_ns_cphy_phychan_established_ind_t tdsl1_ns_cphy_phychan_established_ind;
  tdsl1_ns_cphy_rl_failure_ind_t          tdsl1_ns_cphy_rl_failure_ind;
  tdsl1_ns_cphy_error_ind_t               tdsl1_ns_cphy_error_ind;

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

} rf_tdscdma_msg_u;

typedef union
{
  tfw_iq_capture_rsp_t                   tfw_iq_capture;
} rf_tdscdma_mdsp_msg_u;

typedef struct
{
  uint32 l1_msg_umid;
  uint8 msg_enum;
} rfa_tdscdma_l1_log_param;

typedef struct
{
  uint32 mdsp_msg_umid;
  uint8 msg_enum;
} rfa_tdscdma_mdsp_log_param;

typedef struct
{
  uint8 idx;
  rfa_tdscdma_l1_log_param l1_log[100];
} rfa_tdscdma_l1_log_t;

typedef struct
{
  uint8 idx;
  rfa_tdscdma_mdsp_log_param mdsp_log[100];
} rfa_tdscdma_mdsp_log_t;



/*===========================================================================

              LOCAL FUNCTION PROTOTYPES

===========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_enter_mode_req( 
                                    rfa_tdscdma_enter_mode_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_exit_mode_req( 
                                    rfa_tdscdma_exit_mode_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_enable_rx_req( 
                                    rfa_tdscdma_enable_rx_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_init_tx_req( 
                                    rfa_tdscdma_init_tx_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_enable_tx_req( 
                                    rfa_tdscdma_enable_tx_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_disable_tx_req( 
                                    rfa_tdscdma_disable_tx_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_sleep_req( 
                                    rfa_tdscdma_sleep_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_wakeup_req ( 
                                    rfa_tdscdma_wakeup_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_set_tx_pwr_limit_req ( 
                                    rfa_tdscdma_set_tx_pwr_limit_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_get_rf_warmup_time_req ( 
                                    rfa_tdscdma_get_rf_warmup_time_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_mc_get_rf_txchain_no_req (
                                    rfa_tdscdma_get_rf_txchain_no_req_s* req_msg_ptr );                                    
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_update_bho_req ( 
                                    rfa_tdscdma_update_bho_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_switch_buf_req ( 
                                    rfa_tdscdma_switch_buf_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_update_tds_nbr_req ( 
                                    rfa_tdscdma_update_tds_nbr_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_update_gsm_nbr_req ( 
                                    rfa_tdscdma_update_gsm_nbr_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_get_max_tx_pwr_req ( 
                                    rfa_tdscdma_get_max_tx_pwr_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_therm_backoff_req ( 
                                    rfa_tdscdma_therm_backoff_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_set_coex_tx_pwr_limit_req ( 
                                    rfa_tdscdma_set_coex_tx_pwr_limit_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_get_rxagc_val_req ( 
                                    rfa_tdscdma_get_rxagc_val_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_get_agc_log_req ( 
                                    rfa_tdscdma_get_agc_log_req_s* req_msg_ptr );

rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_ant_sw_req ( 
                                    rfa_tdscdma_ant_sw_req_s* req_msg_ptr );
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_ant_sw_abort_ind ( 
                                    rfa_tdscdma_ant_sw_abort_ind_s* req_msg_ptr );
                                    
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_dynamic_script_req ( 
                                    rfa_tdscdma_dynamic_script_req_s* req_msg_ptr );    

rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_rxd_override_req( 
                                    rfa_tdscdma_rxd_override_req_s* req_msg_ptr );

void  rf_tdscdma_proc_rflm_hdet_trigger_rsp( void* cmd_ptr,
                                             rf_dispatch_cid_info_type *cid_info,
                                             void *cb_data );

void  rf_tdscdma_proc_rflm_therm_trigger_rsp( void* cmd_ptr,
                                             rf_dispatch_cid_info_type *cid_info,
                                             void *cb_data );

void rf_tdscdma_proc_rflm_autopin_trigger_rsp( void* cmd_ptr,
                                               rf_dispatch_cid_info_type *cid_info,
                                               void *cb_data );

rf_tdscdma_dispatch_outcome_t rf_tdscdma_req_dispatch(rf_tdscdma_msg_u* msg_ptr);

void rf_tdscdma_mdsp_msg_dispatcher( rf_cmd_type* req_ptr );

rf_tdscdma_dispatch_outcome_t rf_tdscdma_mdsp_msg_dispatch(rf_tdscdma_mdsp_msg_u* msg_ptr);

rf_tdscdma_dispatch_outcome_t rf_tdscdma_update_tuner_req( rfa_tdscdma_update_tuner_req_s* req_msg_ptr );

extern void ftm_tdscdma_store_iq_data(uint32 start_address, uint32 start_address_div, uint32 num_samples);
extern void rf_tdscdma_mc_set_blocking_cmd_rsp_status(rf_tdscdma_mdsp_msg_t ftm_rf_cmd_msg, rf_tdscdma_rfmode_cmd_rsp_status_type status);
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
extern rf_tdscdma_dispatch_outcome_t ftm_tdscdma_ber_process_l1_messages_msgr( msgr_hdr_s  *msg_ptr );

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
/*for RF Task MSGR Registration*/
/*L1 msgs*/
static msgr_umid_type rf_tdscdma_l1_umid_list[] = 
{
  RFA_RF_TDSCDMA_ENTER_MODE_REQ,
  RFA_RF_TDSCDMA_EXIT_MODE_REQ,
  RFA_RF_TDSCDMA_ENABLE_RX_REQ,
  RFA_RF_TDSCDMA_INIT_TX_REQ,
  RFA_RF_TDSCDMA_ENABLE_TX_REQ,
  RFA_RF_TDSCDMA_DISABLE_TX_REQ,
  RFA_RF_TDSCDMA_SLEEP_REQ,
  RFA_RF_TDSCDMA_WAKEUP_REQ,
  RFA_RF_TDSCDMA_SET_TX_PWR_LIMIT_REQ,
  RFA_RF_TDSCDMA_GET_RF_WARMUP_TIME_REQ,
  RFA_RF_TDSCDMA_UPDATE_BHO_REQ,
  RFA_RF_TDSCDMA_SWITCH_BUF_REQ,
  RFA_RF_TDSCDMA_UPDATE_TDS_NBR_REQ,
  RFA_RF_TDSCDMA_UPDATE_GSM_NBR_REQ,
  RFA_RF_TDSCDMA_GET_MAX_TX_PWR_REQ,
  RFA_RF_TDSCDMA_THERM_BACKOFF_REQ,
  RFA_RF_TDSCDMA_SET_COEX_TX_PWR_LIMIT_REQ,
  RFA_RF_TDSCDMA_GET_RXAGC_VAL_REQ,
  RFA_RF_TDSCDMA_GET_AGC_LOG_REQ,
  RFA_RF_TDSCDMA_UPDATE_TUNER_REQ,
  RFA_RF_TDSCDMA_ANT_SW_REQ,
  RFA_RF_TDSCDMA_ANT_SW_ABORT_IND,
  RFA_RF_TDSCDMA_GET_RF_TXCHAIN_NO_REQ,
  RFA_RF_TDSCDMA_DYNAMIC_SCRIPT_REQ,
  RFA_RF_TDSCDMA_RXD_OVERRIDE_REQ,
};

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
static msgr_umid_type rf_tdscdma_l1_ns_umid_list[] = 
{

  //NS FTM
  TDSCDMA_L1NS_CPHY_START_CNF,
  TDSCDMA_L1NS_CPHY_STOP_CNF,
  TDSCDMA_L1NS_CPHY_ACQ_CNF,
  TDSCDMA_L1NS_CPHY_SETUP_CNF,
  TDSCDMA_L1NS_CPHY_ESTABLISHED_IND,
  TDSCDMA_L1NS_CPHY_RL_FAILURE_IND,
  TDSCDMA_L1NS_CPHY_ERROR_IND,
  TDSCDMA_L1NS_DL_DATA_IND,

};

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

/*for RF Task MSGR Registration*/
/*MDSP msgs*/
static msgr_umid_type rf_tdscdma_mdsp_umid_list[] = 
{
  TDSCDMA_FW_RSP_FTM_RX_RSP,
  TDSCDMA_FW_RSP_FTM_TX_RSP,
  TDSCDMA_FW_STATE_CFG_RSP,
  TDSCDMA_FW_IQ_CAPTURE_RSP,
  TDSCDMA_FW_FTM_GET_MULTI_SYNTH_STATE_RSP
};

static rfa_tdscdma_l1_log_t l1_req_log;
static rfa_tdscdma_l1_log_t l1_cnf_log;
static rfa_tdscdma_mdsp_log_t mdsp_rsp_log;


/*===========================================================================

                               Macros

===========================================================================*/
/* ! @brief Define the RF TDSCDMA L1 message count for the mailboxes.
*/
#define RF_TDSCDMA_L1_CNT \
    (sizeof(rf_tdscdma_l1_umid_list)/sizeof(msgr_umid_type))

/* ! @brief Define the RF TDSCDMA MDSP message count for the mailboxes.
*/
#define RF_TDSCDMA_MDSP_CNT \
    (sizeof(rf_tdscdma_mdsp_umid_list)/sizeof(msgr_umid_type))
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
/* ! @brief Define the RF TDSCDMA L1 NS message count for the mailboxes.
*/
#define RF_TDSCDMA_L1_NS_CNT \
    (sizeof(rf_tdscdma_l1_ns_umid_list)/sizeof(msgr_umid_type))

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/



/*===========================================================================

                      FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  rf_tdscdma_send_msgr_cnf()

===========================================================================*/
errno_enum_type rf_tdscdma_send_msgr_cnf(msgr_hdr_struct_type* msg_ptr, uint32 umid, uint32 size,uint8 msg_enum_cnf)
{
errno_enum_type msgr_send_status = E_SUCCESS;
 
  //static int j=0;
  if ( l1_cnf_log.idx>= 100)
  {
    /* Reset to Zero */
    l1_cnf_log.idx = 0;
  }

  
  
  if( msg_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "MSGR Send NULL Message UMID 0x%X",umid);
    return (E_BAD_ADDRESS);
  }

  msgr_init_hdr( msg_ptr, MSGR_RFA_RF_TDSCDMA, umid );
  l1_cnf_log.l1_log[l1_cnf_log.idx].l1_msg_umid=umid;
  l1_cnf_log.l1_log[l1_cnf_log.idx].msg_enum=msg_enum_cnf;
  l1_cnf_log.idx++;

    
  msgr_send_status = msgr_send( msg_ptr, size );
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "MSGR Send Unsuccessful UMID 0x%X, error_no %d",umid,msgr_send_status);
  }
  return msgr_send_status;
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_enter_mode_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_enter_mode_req( rfa_tdscdma_enter_mode_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_enter_mode_cnf_s   cnf_msg;
  errno_enum_type                msgr_send_status = E_SUCCESS;
  
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_enter_mode_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;
  cnf_msg.is_sawless = FALSE;

  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */
  mc_status = rf_tdscdma_mc_enter_mode(req_msg_ptr->device, 
                                       req_msg_ptr->sec_device,
                                       req_msg_ptr->rxlm_buf_idx,
                                       req_msg_ptr->rxlm_mode,
                                       req_msg_ptr->is_dsds);

  cnf_msg.is_sawless = rf_tdscdma_mc_is_rx_sawless();

  /* Check processing result, and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  } 
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg,
                                              RFA_RF_TDSCDMA_ENTER_MODE_CNF,
                                              sizeof(cnf_msg),
                                              RFA_TDSCDMA_MSG_ID_ENTER_MODE);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_ENTER_MODE_CNF Send Unsuccessful", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_exit_mode_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_exit_mode_req( rfa_tdscdma_exit_mode_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_exit_mode_cnf_s    cnf_msg;
  errno_enum_type                msgr_send_status = E_SUCCESS;

  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_exit_mode_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */
  mc_status = rf_tdscdma_mc_exit_mode(req_msg_ptr->device, 
                                      req_msg_ptr->sec_device,
                                      #if 0 // temp wait for L1 readiness
                                      req_msg_ptr->wait_for_fw_release
                                      #else
                                      FALSE
                                      #endif
                                      );

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send the CNF MSG back */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg,
                                              RFA_RF_TDSCDMA_EXIT_MODE_CNF,
                                              sizeof(cnf_msg),
                                              RFA_TDSCDMA_MSG_ID_EXIT_MODE);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,
          "RFA_RF_TDSCDMA_EXIT_MODE_CNF Send Unsuccessful", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }

  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_enable_rx_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_enable_rx_req( rfa_tdscdma_enable_rx_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_enable_rx_cnf_s    cnf_msg;
  errno_enum_type                msgr_send_status;
  uint32                         carrier_freq;
  uint8                          rx_buf_idx;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_enable_rx_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  if ((req_msg_ptr->sawless_rx_lin_state) > 1)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, 
          "RFA_RF_TDSCDMA_ENABLE_RX defaulting to LL (0) state %d", req_msg_ptr->sawless_rx_lin_state);
    req_msg_ptr->sawless_rx_lin_state = 0; //default to 0 if invalid value is obtain from L1
   
  }

  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */
  mc_status = rf_tdscdma_mc_enable_rx(req_msg_ptr->device, 
                                      req_msg_ptr->sec_device,
                                      req_msg_ptr->channel,
                                      req_msg_ptr->rxlm_buf_idx,
                                      req_msg_ptr->rxlm_mode,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                      req_msg_ptr->rx_path,
                                      req_msg_ptr->rxlm_buf_idx_div,
                                      req_msg_ptr->rxlm_mode_div,

#endif
                                      &carrier_freq,
                                      &rx_buf_idx,
                                      req_msg_ptr->is_dsds,
                                      req_msg_ptr->sawless_rx_lin_state,
                                      TRUE);

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.carrier_freq = carrier_freq;
    cnf_msg.rx_buf_idx   = rx_buf_idx; 
    cnf_msg.req_result   = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_ENABLE_RX_CNF, 
                                              sizeof(rfa_tdscdma_enable_rx_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_ENABLE_RX);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_ENABLE_RX_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_init_tx_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_init_tx_req( rfa_tdscdma_init_tx_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_init_tx_cnf_s      cnf_msg;
  errno_enum_type                msgr_send_status;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_init_tx_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */
  mc_status = rf_tdscdma_mc_init_tx( );

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_INIT_TX_CNF, 
                                              sizeof(rfa_tdscdma_init_tx_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_INIT_TX);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_INIT_TX_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_enable_tx_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_enable_tx_req( rfa_tdscdma_enable_tx_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_enable_tx_cnf_s    cnf_msg;
  errno_enum_type                msgr_send_status;
  uint8                          tx_buf_idx;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_enable_tx_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */
  mc_status = rf_tdscdma_mc_enable_tx(req_msg_ptr->device,        
                                      req_msg_ptr->channel,
                                      req_msg_ptr->txlm_buf_idx,
                                      &tx_buf_idx);

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.tx_buf_idx = tx_buf_idx;
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_ENABLE_TX_CNF, 
                                              sizeof(rfa_tdscdma_enable_tx_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_ENABLE_TX);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_ENABLE_TX_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_disable_tx_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_disable_tx_req( rfa_tdscdma_disable_tx_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_disable_tx_cnf_s   cnf_msg;
  errno_enum_type                msgr_send_status;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_disable_tx_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */
  mc_status = rf_tdscdma_mc_disable_tx(req_msg_ptr->device);

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_DISABLE_TX_CNF, 
                                              sizeof(rfa_tdscdma_disable_tx_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_DISABLE_TX);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_DISABLE_TX_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_sleep_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_sleep_req( rfa_tdscdma_sleep_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_sleep_cnf_s        cnf_msg;
  errno_enum_type                msgr_send_status;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_sleep_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */
  mc_status = rf_tdscdma_mc_sleep(req_msg_ptr->device, 
                                  req_msg_ptr->sec_device);

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_SLEEP_CNF, 
                                              sizeof(rfa_tdscdma_sleep_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_SLEEP);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_SLEEP_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_wakeup_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_wakeup_req( rfa_tdscdma_wakeup_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  boolean                        rebuild_ifreq_script = FALSE;
  rfa_tdscdma_wakeup_cnf_s       cnf_msg;
  errno_enum_type                msgr_send_status;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_wakeup_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */
  if (req_msg_ptr->wakeup_step == RFA_TDSCDMA_WAKEUP_STEP0)
  {
    /* Call wakeup_step0() */
    mc_status = rf_tdscdma_mc_wakeup_step0p1(req_msg_ptr->device, 
                                             req_msg_ptr->sec_device,
                                             req_msg_ptr->channel,
                                             req_msg_ptr->rxlm_buf_idx,
                                             req_msg_ptr->rxlm_mode
                                             #ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                             ,req_msg_ptr->rx_path,
                                             req_msg_ptr->rxlm_buf_idx_div,
                                             req_msg_ptr->rxlm_mode_div
                                             #endif
                                             , &rebuild_ifreq_script
                                             );
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH,
          "RFA_RF_TDSCDMA_WAKEUP_REQ: req_msg_ptr->wakeup_step= %d and step0p1() is called", 
          req_msg_ptr->wakeup_step);

    /* Check processing result,and send response */
    if (mc_status == RF_TDSCDMA_MC_SUCCESS)
    {
      cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
    }
    else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
    {
      cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
    }
    else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
    {
      cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
    }

    cnf_msg.wakeup_step = RFA_TDSCDMA_WAKEUP_STEP0P1;

#if 0 // Enable ONLY when TL1 SW dependence presents
    /* Send back the CNF MSG */
    msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                                RFA_RF_TDSCDMA_WAKEUP_CNF, 
                                                sizeof(rfa_tdscdma_wakeup_cnf_s),
                                                RFA_TDSCDMA_MSG_ID_WAKEUP);
    if (msgr_send_status != E_SUCCESS)
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
            "RFA_RF_TDSCDMA_WAKEUP_CNF Send Unsuccessful, status %d", msgr_send_status);
      ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
    }
#endif

    /* Continue with RFA_TDSCDMA_WAKEUP_STEP0P2 */
    mc_status = rf_tdscdma_mc_wakeup_step0p2(req_msg_ptr->device, 
                                             req_msg_ptr->sec_device,
                                             req_msg_ptr->channel,
                                             req_msg_ptr->rxlm_buf_idx,
                                             req_msg_ptr->rxlm_mode
                                             #ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                             ,req_msg_ptr->rx_path,
                                             req_msg_ptr->rxlm_buf_idx_div,
                                             req_msg_ptr->rxlm_mode_div
                                             #endif
                                             , &rebuild_ifreq_script
                                             );
                                             
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH,
          "RFA_RF_TDSCDMA_WAKEUP_REQ: req_msg_ptr->wakeup_step= %d and step0p2() is called", 
          req_msg_ptr->wakeup_step);

    cnf_msg.wakeup_step = RFA_TDSCDMA_WAKEUP_STEP0P2;
  }
  else if (req_msg_ptr->wakeup_step == RFA_TDSCDMA_WAKEUP_STEP1)
  {
    /* Call wakeup_step1() */
    mc_status = rf_tdscdma_mc_wakeup_step1(req_msg_ptr->device, 
                                           req_msg_ptr->sec_device,
                                           req_msg_ptr->channel,
                                           req_msg_ptr->rxlm_buf_idx,
                                           req_msg_ptr->rxlm_mode
                                           #ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                           ,req_msg_ptr->rx_path,
                                           req_msg_ptr->rxlm_buf_idx_div,
                                           req_msg_ptr->rxlm_mode_div
                                           #endif
                                           );
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH,
          "RFA_RF_TDSCDMA_WAKEUP_REQ: req_msg_ptr->wakeup_step= %d and step1() is called", 
          req_msg_ptr->wakeup_step);
  }
  else if (req_msg_ptr->wakeup_step == RFA_TDSCDMA_WAKEUP_TOGETHER)
  {
    /* Call wakeup_step0() and step1() back to back */
    mc_status = rf_tdscdma_mc_wakeup_step0p1(req_msg_ptr->device, 
                                             req_msg_ptr->sec_device,
                                             req_msg_ptr->channel,
                                             req_msg_ptr->rxlm_buf_idx,
                                             req_msg_ptr->rxlm_mode
                                             #ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                             ,req_msg_ptr->rx_path,
                                             req_msg_ptr->rxlm_buf_idx_div,
                                             req_msg_ptr->rxlm_mode_div
                                             #endif
                                             , &rebuild_ifreq_script
                                             );

        /* Call wakeup_step0() */
    mc_status = rf_tdscdma_mc_wakeup_step0p2(req_msg_ptr->device, 
                                             req_msg_ptr->sec_device,
                                             req_msg_ptr->channel,
                                             req_msg_ptr->rxlm_buf_idx,
                                             req_msg_ptr->rxlm_mode
                                             #ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                             ,req_msg_ptr->rx_path,
                                             req_msg_ptr->rxlm_buf_idx_div,
                                             req_msg_ptr->rxlm_mode_div
                                             #endif
                                             , &rebuild_ifreq_script
                                             );

    mc_status = rf_tdscdma_mc_wakeup_step1(req_msg_ptr->device, 
                                             req_msg_ptr->sec_device,
                                             req_msg_ptr->channel,
                                             req_msg_ptr->rxlm_buf_idx,
                                             req_msg_ptr->rxlm_mode
                                             #ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                             ,req_msg_ptr->rx_path,
                                             req_msg_ptr->rxlm_buf_idx_div,
                                             req_msg_ptr->rxlm_mode_div
                                             #endif
                                             );
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH,
          "RFA_RF_TDSCDMA_WAKEUP_REQ: req_msg_ptr->wakeup_step= %d and step0() and step1() is called back-to-back", 
          req_msg_ptr->wakeup_step);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,
          "RFA_RF_TDSCDMA_WAKEUP_REQ: req_msg_ptr->wakeup_step= %d and this is wrong!!", 
          req_msg_ptr->wakeup_step);
  }

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  cnf_msg.rebuild_ifreq_script = rebuild_ifreq_script;

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_WAKEUP_CNF, 
                                              sizeof(rfa_tdscdma_wakeup_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_WAKEUP);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_WAKEUP_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_set_tx_pwr_limit_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_set_tx_pwr_limit_req( rfa_tdscdma_set_tx_pwr_limit_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t         ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type             mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_set_tx_pwr_limit_cnf_s    cnf_msg;
  errno_enum_type                       msgr_send_status;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_set_tx_pwr_limit_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */
  mc_status = rf_tdscdma_mc_set_tx_pwr_limit( req_msg_ptr->tx_pwr_limit );

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_SET_TX_PWR_LIMIT_CNF, 
                                              sizeof(rfa_tdscdma_set_tx_pwr_limit_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_SET_TX_PWR_LIMIT);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_SET_TX_PWR_LIMIT_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_get_rf_warmup_time_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_get_rf_warmup_time_req( rfa_tdscdma_get_rf_warmup_time_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t         ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rfa_tdscdma_get_rf_warmup_time_cnf_s  cnf_msg;
  errno_enum_type                       msgr_send_status;
  uint16                                warmup_time; 

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_get_rf_warmup_time_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  warmup_time = rf_tdscdma_mc_get_rf_warmup_time( );

  if (warmup_time != 0xFFFF) 
  {
    cnf_msg.rf_warmup_time = warmup_time;
    cnf_msg.req_result     = RFA_TDSCDMA_SUCCESS;
  }
  else
  {
    cnf_msg.rf_warmup_time = warmup_time;
    cnf_msg.req_result     = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_GET_RF_WARMUP_TIME_CNF, 
                                              sizeof(rfa_tdscdma_get_rf_warmup_time_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_GET_RF_WARMUP_TIME);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_GET_RF_WARMUP_TIME_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}

rf_tdscdma_dispatch_outcome_t rf_tdscdma_mc_get_rf_txchain_no_req( rfa_tdscdma_get_rf_txchain_no_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t         ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rfa_tdscdma_get_rf_txchain_no_cnf_s  cnf_msg;
  
  errno_enum_type                       msgr_send_status;
  uint16                                txchain_no = 0; 

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_mc_get_rf_txchain_no_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  if (txchain_no != 0xFFFF) 
  {
    cnf_msg.rf_txchain_no = txchain_no;
    cnf_msg.req_result     = RFA_TDSCDMA_SUCCESS;
  }
  else
  {
    cnf_msg.rf_txchain_no = txchain_no;
    cnf_msg.req_result     = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_GET_RF_TXCHAIN_NO_CNF,
                                              sizeof(rfa_tdscdma_get_rf_txchain_no_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_GET_RF_TXCHAIN_NO);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_GET_RF_TXCHAIN_NO_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}
/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_update_bho_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_update_bho_req( rfa_tdscdma_update_bho_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_update_bho_cnf_s   cnf_msg;
  errno_enum_type                msgr_send_status;
  uint32                         prim_carrier_freq;
  uint32                         carrier_freq;
  uint8                          rx_buf_idx;
  uint8                          tx_buf_idx;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_update_bho_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */
  mc_status = rf_tdscdma_mc_update_bho(req_msg_ptr->device, 
                                       req_msg_ptr->sec_device,
                                       req_msg_ptr->tgt_prim_channel,
                                       req_msg_ptr->tgt_channel,
                                       req_msg_ptr->rxlm0_tgt_buf_idx,
                                       req_msg_ptr->rxlm_mode,
                                       #ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                       req_msg_ptr->rx_path,
                                       req_msg_ptr->rxlm_tgt_buf_idx_div,
                                       req_msg_ptr->rxlm_mode_div,
                                       #endif
                                       req_msg_ptr->txlm_tgt_buf_idx,
                                       &prim_carrier_freq,
                                       &carrier_freq,
                                       &rx_buf_idx,
                                       &tx_buf_idx);

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.prim_carrier_freq = prim_carrier_freq;
    cnf_msg.carrier_freq      = carrier_freq;
    cnf_msg.rx_buf_idx        = rx_buf_idx;
    cnf_msg.tx_buf_idx        = tx_buf_idx;
    cnf_msg.req_result        = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_UPDATE_BHO_CNF, 
                                              sizeof(rfa_tdscdma_update_bho_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_UPDATE_BHO);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_UPDATE_BHO_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_switch_buf_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_switch_buf_req( rfa_tdscdma_switch_buf_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_switch_buf_cnf_s   cnf_msg;
  errno_enum_type                msgr_send_status;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_switch_buf_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */
  mc_status = rf_tdscdma_mc_switch_buffer( req_msg_ptr->buf_swap,
                                           req_msg_ptr->rxlm0_wrk_buf_idx,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                           req_msg_ptr->rxlm_wrk_buf_idx_div,
#endif

                                           req_msg_ptr->txlm_wrk_buf_idx );
  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_SWITCH_BUF_CNF, 
                                              sizeof(rfa_tdscdma_switch_buf_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_SWITCH_BUF);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_SWITCH_BUF_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_update_tds_nbr_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_update_tds_nbr_req( rfa_tdscdma_update_tds_nbr_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t     ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type         mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_update_tds_nbr_cnf_s  cnf_msg;
  errno_enum_type                   msgr_send_status;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_update_tds_nbr_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */  
  mc_status = rf_tdscdma_mc_meas_update_tds_nbr( req_msg_ptr->num_nbr,
                                                 req_msg_ptr->nbr_list,
                                                 req_msg_ptr->carrier_freq_list, 
                                                 req_msg_ptr->ifreq_rxlm_buf_idx,
                                                 req_msg_ptr->ifreq_rxlm_mode, 
                                                 req_msg_ptr->rx_path,
                                                 req_msg_ptr->ifreq_rxlm_buf_idx_div,
                                                 req_msg_ptr->ifreq_rxlm_mode_div,
                                                 req_msg_ptr->div_buf_idx);

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_UPDATE_TDS_NBR_CNF, 
                                              sizeof(rfa_tdscdma_update_tds_nbr_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_UPDATE_TDS_NBR);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_UPDATE_TDS_NBR_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_update_gsm_nbr_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_update_gsm_nbr_req( rfa_tdscdma_update_gsm_nbr_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t     ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type         mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_update_gsm_nbr_cnf_s  cnf_msg;
  errno_enum_type                   msgr_send_status;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_update_gsm_nbr_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  //note: this msg is not being used
  /* Call the appropriate MC function and based on the result,
     update cnf_msg.req_result */
  //
  //mc_status = rf_tdscdma_mc_meas_update_gsm_nbr( req_msg_ptr->num_nbr,
  //                                               req_msg_ptr->nbr_list,
  //                                               req_msg_ptr->rxlm_t2g_buf_idx );

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_UPDATE_GSM_NBR_CNF, 
                                              sizeof(rfa_tdscdma_update_gsm_nbr_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_UPDATE_GSM_NBR);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_UPDATE_GSM_NBR_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_get_max_tx_pwr_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_get_max_tx_pwr_req( rfa_tdscdma_get_max_tx_pwr_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t     ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rfa_tdscdma_get_max_tx_pwr_cnf_s  cnf_msg;
  errno_enum_type                   msgr_send_status;
  int16                             max_tx_pwr; 

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_get_max_tx_pwr_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }
  
  max_tx_pwr = rf_tdscdma_mc_get_max_tx_pwr( );

  cnf_msg.max_tx_pwr  = max_tx_pwr;
  cnf_msg.req_result  = RFA_TDSCDMA_SUCCESS;

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_GET_MAX_TX_PWR_CNF, 
                                              sizeof(rfa_tdscdma_get_max_tx_pwr_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_GET_MAX_TX_PWR);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_GET_MAX_TX_PWR_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_therm_backoff_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_therm_backoff_req( rfa_tdscdma_therm_backoff_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t     ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rfa_tdscdma_therm_backoff_cnf_s   cnf_msg;
  errno_enum_type                   msgr_send_status;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_therm_backoff_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }
  
  rf_tdscdma_mc_therm_backoff(req_msg_ptr->backoff);

  cnf_msg.req_result  = RFA_TDSCDMA_SUCCESS;

  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_THERM_BACKOFF_CNF, 
                                              sizeof(rfa_tdscdma_therm_backoff_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_THERM_BACKOFF);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_THERM_BACKOFF_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_set_coex_tx_pwr_limit_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function. Note: this msg is coming from MCS, not from L1

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_set_coex_tx_pwr_limit_req( rfa_tdscdma_set_coex_tx_pwr_limit_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t         ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type             mc_status = RF_TDSCDMA_MC_SUCCESS;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_set_coex_tx_pwr_limit_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  /* Call the appropriate MC function */
  mc_status = rf_tdscdma_mc_set_coex_tx_pwr_limit( req_msg_ptr->sar_tx_pwr_limit,
                                                   req_msg_ptr->coex_tx_pwr_limit );

  if (mc_status != RF_TDSCDMA_MC_SUCCESS)
  {
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}

/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_ant_sw_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_ant_sw_req( rfa_tdscdma_ant_sw_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t     ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type         mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_ant_sw_cnf_s          cnf_msg;
  errno_enum_type                   msgr_send_status;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_ant_sw_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }
#ifdef FEATURE_RF_ASDIV 
  mc_status=rf_tdscdma_mc_switch_ant(req_msg_ptr->ant_sw_params.device,
                                       req_msg_ptr->ant_sw_params.pos,
                                       req_msg_ptr->ant_sw_params.cb_func,
                                       req_msg_ptr->ant_sw_params.cb_data,
                                       req_msg_ptr->ant_sw_params.is_immediate);
#endif

    /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
  cnf_msg.req_result  = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }


  /* Send back the CNF MSG */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg, 
                                              RFA_RF_TDSCDMA_ANT_SW_CNF, 
                                              sizeof(rfa_tdscdma_ant_sw_cnf_s),
                                              RFA_TDSCDMA_MSG_ID_ANT_SW);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RFA_RF_TDSCDMA_ANT_SW_CNF Send Unsuccessful, status %d", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
 
  return(ret_status);
}


/*===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_req_dispatch. It calls the respective MC
  function and then sends a CNF MSG back.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_ant_sw_abort_ind( rfa_tdscdma_ant_sw_abort_ind_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t     ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  //rf_tdscdma_mc_status_type         mc_status = RF_TDSCDMA_MC_SUCCESS;
  //errno_enum_type                   msgr_send_status;

  /* check input Parameter */
  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_ant_sw_abort_ind is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }
#if 0
  rf_tdscdma_mc_switch_ant_abort(req_msg_ptr->ant_sw_params.device,
                                       req_msg_ptr->ant_sw_params.pos,
                                       req_msg_ptr->ant_sw_params.cb_func,
                                       req_msg_ptr->ant_sw_params.cb_data,
                                       );

#endif

#ifdef FEATURE_RF_ASDIV 
   rf_tdscdma_mc_switch_ant_abort(RFM_DEVICE_0,
                                  0,
                                  NULL,
                                  NULL);
#endif


  return(ret_status);
}

/*=========================================================================
 
  FUNCTION:  rf_tdscdma_proc_rflm_hdet_trigger_rsp

=========================================================================*/
/*!
  @brief
  This function is called by FED RF command dispatcher. It calls the
  respective HDET trigger function.

  @return
  void
*/
/*=========================================================================*/
void rf_tdscdma_proc_rflm_hdet_trigger_rsp( void* cmd_ptr,
                                            rf_dispatch_cid_info_type *cid_info,
                                            void *cb_data )
{
  int16 txagcdb10;
  txagcdb10 = rflmTdsRfModeCtlState.pdetInfo.rflmTxagcdbm10;

  rf_tdscdma_mc_trigger_hdet(txagcdb10);

}


/*=========================================================================
 
  FUNCTION:  rf_tdscdma_proc_rflm_autopin_trigger_rsp

=========================================================================*/
/*!
  @brief
  This function is called by FED RF command dispatcher. It calls the
  respective AutoPin trigger function.

  @return
  void
*/
/*=========================================================================*/
void rf_tdscdma_mc_autopin_trigger_read();

void rf_tdscdma_proc_rflm_autopin_trigger_rsp( void* cmd_ptr,
                                               rf_dispatch_cid_info_type *cid_info,
                                               void *cb_data )
{
  rf_tdscdma_mc_autopin_trigger_read();
}


/*=========================================================================
 
  FUNCTION:  rf_tdscdma_proc_rflm_therm_trigger_rsp

=========================================================================*/
/*!
  @brief
  This function is called by FED RF command dispatcher. It calls the
  respective THERM trigger function.

  @return
  void
*/
/*=========================================================================*/
void rf_tdscdma_proc_rflm_therm_trigger_rsp( void* cmd_ptr,
                                            rf_dispatch_cid_info_type *cid_info,
                                            void *cb_data )
{
  rf_tdscdma_mc_trigger_therm();
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_mdsp_iqcapture_rsp_cal

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_proc_mdsp_iqcapture_rsp. It calls the respective
  processing function to process IQ samples.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_mdsp_iqcapture_rsp_cal( tfw_iq_capture_rsp_t* rsp_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  uint32* init_addr; //start address of the samples
  uint32  i; 
//#ifndef FEATURE_TRITON_MODEM
//  int16  rsb_a = 0, rsb_b = 0;
//#endif

  MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, " IQ capture rsp:state=%d ", rsp_msg_ptr->status); 
        
  if ( rsp_msg_ptr->status == T_IQ_CAPTURE_SUCCESS )
  {
    ftm_tds_iq_capture_data.startAddrPrx = (uint32*)rsp_msg_ptr->startAddress;
    ftm_tds_iq_capture_data.startAddrDrx = (uint32*)rsp_msg_ptr->startAddressDiv;

    MSG_3(MSG_SSID_RF, MSG_LVL_HIGH," numSamples=%d, startAddr=0x%x, startAddrDiv=0x%x ",                                                                                                                     
          rsp_msg_ptr->numSamples,
          rsp_msg_ptr->startAddress,
          rsp_msg_ptr->startAddressDiv);

    /* Printing data in debug messages */
    if ( ftm_tds_iq_capture_data.PrintData == TRUE)
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_HIGH," ============= PRX sample data =============", 0);
      init_addr = (uint32*)rsp_msg_ptr->startAddress;

      for ( i=0; i<(((ftm_tds_iq_capture_data.SampleSize *20)>>5)); i++) //loop size = SampleSize*20/32 ;
      {
        MSG_2(MSG_SSID_RF, MSG_LVL_HIGH," sample=%04d, data=0x%08X", i, (*init_addr));
        init_addr ++;
        if((i%1000) == 0)//Let diag catch-up every 1K lines
        {
          DALSYS_BusyWait(ftm_tds_iq_capture_data.IqCaptureOutputDelay);
        }
      }
      DALSYS_BusyWait(ftm_tds_iq_capture_data.IqCaptureOutputDelay*2);

      MSG_1(MSG_SSID_RF, MSG_LVL_HIGH," ============= DRX sample data =============", 0);
      init_addr = (uint32*)rsp_msg_ptr->startAddressDiv;

      for ( i=0; i<(((ftm_tds_iq_capture_data.SampleSize *20)>>5)); i++) //loop size = SampleSize*20/32 ;
      {
        MSG_2(MSG_SSID_RF, MSG_LVL_HIGH," sample=%04d, data=0x%08X", i, (*init_addr));
        init_addr ++;
        if((i%1000) == 0)//Let diag catch-up every 1K lines
        {
          DALSYS_BusyWait(ftm_tds_iq_capture_data.IqCaptureOutputDelay);
        }
      }
      DALSYS_BusyWait(ftm_tds_iq_capture_data.IqCaptureOutputDelay*2);
    } /*if ( ftm_tds_iq_capture_data.PrintData == TRUE) */

#if 0
#ifndef FEATURE_TRITON_MODEM
    //do RSB processing here:
    rfcommon_msm_compute_rsb_coeff_from_iq ( ftm_tds_iq_capture_data.startAddrPrx, 
                                             ftm_tds_iq_capture_data.SampleSize,
                                             &rsb_a,
                                             &rsb_b
                                            );
    rfcommon_msm_compute_rsb_coeff_from_iq ( ftm_tds_iq_capture_data.startAddrDrx, 
                                             ftm_tds_iq_capture_data.SampleSize,
                                             &rsb_a,
                                             &rsb_b
                                            );
#endif /*if not FEATURE_TRITON_MODEM*/
#endif /* if 0 */

    ftm_tds_iq_capture_data.IqCaptureStatus = FTM_TDSCDMA_IQ_CAPTURE_SUCCEED; //success
    ftm_tdscdma_store_iq_data(rsp_msg_ptr->startAddress, rsp_msg_ptr->startAddressDiv, rsp_msg_ptr->numSamples);
    rf_tdscdma_mc_proc_iq_capture_rsp(TRUE);
  }
  else
  {
    ftm_tds_iq_capture_data.IqCaptureStatus = FTM_TDSCDMA_IQ_CAPTURE_FAIL; //unsuccessful
    rf_tdscdma_mc_proc_iq_capture_rsp(FALSE);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;

  }

  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_mdsp_iqcapture_rsp_ftmrf

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_proc_mdsp_iqcapture_rsp. It calls the respective
  processing function to process IQ samples.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_mdsp_iqcapture_rsp_ftmrf( tfw_iq_capture_rsp_t* rsp_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t     ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_rfmode_cmd_rsp_status_type iq_capt_status = TDSCDMA_RFMODE_CMD_RSP_FAILURE;

  if (rsp_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "TDSCDMA IQ capture: NULL pointer ");
    return (RF_TDSCDMA_DISPATCH_NULL_PTR);
  }

  if (rsp_msg_ptr->status != T_IQ_CAPTURE_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "TDSCDMA IQ capture: not successful, status=%d ", rsp_msg_ptr->status );
  }
  
  if((rsp_msg_ptr->startAddress == 0) || (rsp_msg_ptr->startAddressDiv == 0))
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "TDSCDMA IQ capture: Source data pointer NULL, error! ");
  }

  MSG_5(MSG_SSID_RF, MSG_LVL_HIGH, "TDSCDMA IQ capture: data is got from FW, status=%d, startAddress=%d, startAddressDiv=%d, startTime=%d, numSamples=%d,", 
          rsp_msg_ptr->status, rsp_msg_ptr->startAddress, rsp_msg_ptr->startAddressDiv, rsp_msg_ptr->startTime, rsp_msg_ptr->numSamples );

  ftm_tdscdma_store_iq_data(rsp_msg_ptr->startAddress, rsp_msg_ptr->startAddressDiv, rsp_msg_ptr->numSamples);

  if (rsp_msg_ptr->status == T_IQ_CAPTURE_SUCCESS)
  {
    iq_capt_status = TDSCDMA_RFMODE_CMD_RSP_SUCCESS;
  }
  else if (rsp_msg_ptr->status == T_IQ_CAPTURE_FAILURE)
  {
    iq_capt_status = TDSCDMA_RFMODE_CMD_RSP_FAILURE;
  }
  else
  {          
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "The status from FW is not valid");
  }

  rf_tdscdma_mc_set_blocking_cmd_rsp_status(RF_TDSCDMA_MDSP_IQ_CAPTURE_CFG_MSG, iq_capt_status);

  return(ret_status);
}

/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_mdsp_iqcapture_rsp

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_mdsp_msg_dispatch. It calls the respective
  processing function to process IQ samples.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_mdsp_iqcapture_rsp( tfw_iq_capture_rsp_t* rsp_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;

  if(rfm_get_calibration_state())
  {
    ret_status = rf_tdscdma_proc_mdsp_iqcapture_rsp_cal(rsp_msg_ptr);
  }
  else
  {
    ret_status = rf_tdscdma_proc_mdsp_iqcapture_rsp_ftmrf(rsp_msg_ptr);
  }
  return (ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_mdsp_get_multi_synth_rsp

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_mdsp_msg_dispatch. 
  It calls the Multi Synth processing function to process the PLL status from FW.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_mdsp_get_multi_synth_rsp(tfw_ftm_get_multi_synth_state_rsp_t * rsp_msg_ptr)
{
	rf_tdscdma_dispatch_outcome_t     ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;

	uint8 tx_pll_status = 0;
	uint8 rx_pll_status = 0;
	uint8 rx_pll_status_bitmask =0;
	uint8 tx_pll_status_bitmask =0;
	
	rfdevice_tdscdma_rx_cmd_dispatch(RFM_DEVICE_0, RFM_TDSCDMA_MODE,RFCOM_BAND_TDSCDMA_B34, 
									 RFDEVICE_TDSCDMA_GET_RX_PLL_STATUS_BITMASK, &rx_pll_status_bitmask);

	rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0, RFM_TDSCDMA_MODE,RFCOM_BAND_TDSCDMA_B34, 
									 RFDEVICE_TDSCDMA_GET_TX_PLL_STATUS_BITMASK, &tx_pll_status_bitmask);


	MSG_2(MSG_SSID_RF, MSG_LVL_HIGH, "Value from Tx Pll Status is 0x%x, bitmask is 0x%x ", 
          rsp_msg_ptr->tx_pll_status, tx_pll_status_bitmask);
	
	MSG_2(MSG_SSID_RF, MSG_LVL_HIGH, "Value from Rx Pll Status is 0x%x  bitmask is 0x%x ", 
          rsp_msg_ptr->rx_pll_status, rx_pll_status_bitmask);

	

	tx_pll_status = (rsp_msg_ptr->tx_pll_status) & tx_pll_status_bitmask;
	rx_pll_status = (rsp_msg_ptr->rx_pll_status) & rx_pll_status_bitmask;
	

	rf_tdscdma_mc_get_multi_synth_rsp(rsp_msg_ptr->status, rx_pll_status, 
		                              tx_pll_status, rsp_msg_ptr->is_cont_tx_valid);

	return ret_status;
}

rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_rfmode_rx_cmd_rsp(tfw_ftm_rx_cmd_rsp_t* rsp_msg_ptr)
{
  rf_tdscdma_dispatch_outcome_t     ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  tfw_ftm_rx_cmd_status_t response = rsp_msg_ptr->rsp;

  MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "TDSCDMA FTM_RX command completion response status: %d", response.status);
  rf_tdscdma_mc_proc_burst_rx_rsp(response.status);

  return ret_status;
}

rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_rfmode_tx_cmd_rsp(tfw_ftm_tx_cmd_rsp_t* rsp_msg_ptr)
{
  rf_tdscdma_dispatch_outcome_t     ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  tfw_ftm_tx_cmd_status_t response = rsp_msg_ptr->rsp;

  MSG_3(MSG_SSID_RF, MSG_LVL_LOW, "TDSCDMA FTM_TX command completion response. Status: %d, Reset: %d, subID: %d", 
        response.status, response.reset, response.subID);
  rf_tdscdma_mc_proc_burst_tx_rsp(response.status, response.reset, response.subID);

  return ret_status;

}



rf_tdscdma_dispatch_outcome_t rf_tdscdma_update_tuner_req( rfa_tdscdma_update_tuner_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  //rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  //rfa_tdscdma_update_tuner_cnf_s    cnf_msg;
//  errno_enum_type                msgr_send_status = E_SUCCESS;

  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_update_tuner_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  //cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;


  /* TODO: Call the appropriate function and based on the result,
     update cnf_msg.req_result */
  MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, 
        "tuner update tick : is_immediate =%d ",req_msg_ptr->is_immediate );
#ifdef FEATURE_RF_HAS_QTUNER
  rf_tdscdma_mc_update_tuner_code(req_msg_ptr->is_immediate);
#endif

  #if 0
  /* Send the CNF MSG back */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg,
                                              RFA_RF_TDSCDMA_UPDATE_TUNER_CNF,
                                              sizeof(cnf_msg),
                                              RFA_TDSCDMA_MSG_ID_UPDATE_TUNER);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,
          "RFA_RF_TDSCDMA_UPDATE_TUNER_CNF Send Unsuccessful", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }
  #endif

  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_dynamic_script_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_mdsp_msg_dispatch. 
  It builds dynamic tuner scripts for ifreq meas.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_dynamic_script_req( rfa_tdscdma_dynamic_script_req_s* req_msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_update_tuner_cnf_s    cnf_msg;
  errno_enum_type                msgr_send_status = E_SUCCESS;

  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_dynamic_script_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  /* Call MC function in mc_meas */
  mc_status = rf_tdscdma_mc_dynamic_script(req_msg_ptr->ifreq_meas_freq, req_msg_ptr->ifreq_buf_idx);

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send the CNF MSG back */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg,
                                              RFA_RF_TDSCDMA_DYNAMIC_SCRIPT_CNF,
                                              sizeof(cnf_msg),
                                              RFA_TDSCDMA_MSG_ID_DYNAMIC_SCRIPT);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,
          "RFA_RF_TDSCDMA_DYNAMIC_SCRIPT_CNF Send Unsuccessful", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }

  return(ret_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_proc_rxd_override_req

===========================================================================*/
/*!
  @brief
  This function is called by rf_tdscdma_mc_msg_dispatch. 
  It process RxD overrride cmd.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_proc_rxd_override_req
( 
  rfa_tdscdma_rxd_override_req_s* req_msg_ptr 
)
{
  rf_tdscdma_dispatch_outcome_t  ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mc_status_type      mc_status = RF_TDSCDMA_MC_SUCCESS;
  rfa_tdscdma_update_tuner_cnf_s    cnf_msg;
  errno_enum_type                msgr_send_status = E_SUCCESS;

  if (req_msg_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "Pointer argument into rf_tdscdma_proc_rxd_override_req is NULL");
    ret_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
    return ret_status;
  }

  cnf_msg.req_result = RFA_TDSCDMA_EXECUTION_ERROR;

  /* Call MC function in mc_meas */
  mc_status = rf_tdscdma_mc_rxd_override(req_msg_ptr->rxd_override);

  /* Check processing result,and mark response */
  if (mc_status == RF_TDSCDMA_MC_SUCCESS)
  {
    cnf_msg.req_result = RFA_TDSCDMA_SUCCESS;
  }
  else if (mc_status == RF_TDSCDMA_MC_SM_ERROR)
  {
    cnf_msg.req_result = RFA_TDSCDMA_STATE_MACHINE_ERROR;
  }
  else if (mc_status == RF_TDSCDMA_MC_INVALID_PARAM)
  {
    cnf_msg.req_result = RFA_TDSCDMA_INVALID_PARAM_ERROR;
  }

  /* Send the CNF MSG back */
  msgr_send_status = rf_tdscdma_send_msgr_cnf((msgr_hdr_struct_type *)&cnf_msg,
                                              RFA_RF_TDSCDMA_RXD_OVERRIDE_CNF,
                                              sizeof(cnf_msg),
                                              RFA_TDSCDMA_MSG_ID_RXD_OVERRIDE);
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,
          "RFA_RF_TDSCDMA_RXD_OVERRIDE_CNF Send Unsuccessful", msgr_send_status);
    ret_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  }

  return(ret_status);
}



#ifdef FEATURE_RF_TASK
/*===========================================================================

  FUNCTION:  rf_tdscdma_l1_msgr_register

===========================================================================*/
/*!
  @brief
  This function is used by RF Task to register for commands sent by the
  TDSCDMA L1 task via the message router.

  @return
  TRUE if all registration were successful, FALSE otherwise
*/
/*=========================================================================*/
//TODO - to change the function name
boolean rf_tdscdma_l1_msgr_register( msgr_client_t *client_id, msgr_id_t id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;
  rf_cmd_dispatch_status_type dis_res;

  /* Register for the TDSCDMA L1 Commands that the RF task is interested in receiving */
  ret_status = msgr_register_block(MSGR_RFA_RF_TDSCDMA,
                                   client_id,
                                   id,
                                   rf_tdscdma_l1_umid_list,
                                   RF_TDSCDMA_L1_CNT);
  if (ret_status != E_SUCCESS)
  {
    MSG_ERROR("rf_tdscdma_l1_msgr_register: Unable to register for TDSCDMA L1 message router register block.",0,0,0);
    rc = FALSE;
  }

  /* Register for the TDSCDMA MDSP msgs that the RF task is interested in receiving */
  ret_status = msgr_register_block(MSGR_RFA_RF_TDSCDMA,
                                   client_id,
                                   id,
                                   rf_tdscdma_mdsp_umid_list,
                                   RF_TDSCDMA_MDSP_CNT);

  if (ret_status != E_SUCCESS)
  {
    MSG_ERROR("rf_tdscdma_msgr_register: Unable to register for TDSCDMA MDSP message block.",0,0,0);
    rc = FALSE;
  }
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
  /* Register for the TDSCDMA L1 NS msgs that the RF task is interested in receiving */
  ret_status = msgr_register_block(MSGR_TDSCDMA_L1NS,
                                   client_id,
                                   id,
                                   rf_tdscdma_l1_ns_umid_list,
                                   RF_TDSCDMA_L1_NS_CNT);

  if (ret_status != E_SUCCESS)
  {
    MSG_ERROR("rf_tdscdma_msgr_register: Unable to register for TDSCDMA L1 NS message block.",0,0,0);
    rc = FALSE;
  }

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/


  /* Register handler for TDSCDMA MDSP messages */
  dis_res = rf_cmd_dispatch_register_tech( RF_TASK_MAIN, MSGR_TECH_TDSCDMA, MSGR_MODULE_RF_MAX,
                                           rf_tdscdma_mdsp_msg_dispatcher );

  if ( dis_res != RF_CMD_REGISTRATION_SUCCESS )
  {
    MSG_ERROR("rf_tdscdma_msgr_register: Failed mdsp msg handler registration", 0,0,0 );    
    rc = FALSE;
  }

  return(rc);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_l1_msgr_deregister

===========================================================================*/
/*!
  @brief
  This function is used by RF Task to de-register the messages registered earlier 
  for handling the messages sent by the TDSCDMA L1 to the message router

  @return
  TRUE if all de-registration were successful, FALSE otherwise
*/
/*=========================================================================*/
//TODO - to change the function name
boolean rf_tdscdma_l1_msgr_deregister( msgr_client_t *client_id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Deregister all the TDSCDMA L1 message RF task has registerd for. */
  ret_status = msgr_deregister_block(MSGR_RFA_RF_TDSCDMA,
                                     client_id,
                                     rf_tdscdma_l1_umid_list,
                                     RF_TDSCDMA_L1_CNT);
  if (ret_status != E_SUCCESS)
  {
    MSG_ERROR("rf_tdscdma_l1_msgr_deregister: Unable to deregister RF message router register block.",0,0,0);
    rc = FALSE;
  }

  /* Deregister all the TDSCDMA MDSP messages RF task has registerd for. */
  ret_status = msgr_deregister_block(MSGR_RFA_RF_TDSCDMA,
                                     client_id,
                                     rf_tdscdma_mdsp_umid_list,
                                     RF_TDSCDMA_MDSP_CNT);

  if (ret_status != E_SUCCESS)
  {
    MSG_ERROR("rf_tdscdma_msgr_deregister: Unable to deregister MDSP messages ",0,0,0);
    rc = FALSE;
  }
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
    /* Deregister all the TDSCDMA L1 NS message RF task has registerd for. */
  ret_status = msgr_deregister_block(MSGR_TDSCDMA_L1NS,
                                     client_id,
                                     rf_tdscdma_l1_ns_umid_list,
                                     RF_TDSCDMA_L1_NS_CNT);
  if (ret_status != E_SUCCESS)
  {
    MSG_ERROR("rf_tdscdma_l1_msgr_deregister: Unable to deregister  NS RF message router register block.",0,0,0);
    rc = FALSE;
  }

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

  /* de-register the TDSCDMA tech */
  rf_cmd_dispatch_deregister_tech( RF_TASK_MAIN, MSGR_TECH_TDSCDMA, MSGR_MODULE_RF_MAX );

  return(rc);
}

/*===========================================================================

  FUNCTION:  rf_tdscdma_msgr_rflm_register

===========================================================================*/
/*!
  @brief

  @details This funtion is used to register RFLM messages.
 
*/
boolean rf_tdscdma_msgr_rflm_register(void)
{
  boolean ret_val = TRUE; /*  return variable  */

  /*----------------------------------------------------------------------------------------------*/
  /* Register all commands that RF TDSCDMA is the recipient of. These registrations are done
  with the dispatcher module with corresponding Task handler */
  /*----------------------------------------------------------------------------------------------*/

  /* Register command to trigger HDET read */
  ret_val &= rf_dispatch_register_command( rf_task_get_dispatch_handle(),
                                           MSGR_TECH_RFLM, 
                                           RFLM_TDSCDMA_TRIGGER_PDET_CMD, 
                                           rf_tdscdma_proc_rflm_hdet_trigger_rsp,
                                           NULL,
                                           1<<RF_DISPATACH_MSGR_REG );

  /* Register command to trigger Therm read */
  ret_val &= rf_dispatch_register_command( rf_task_get_dispatch_handle(),
                                           MSGR_TECH_RFLM, 
                                           RFLM_TDSCDMA_TRIGGER_THERM_CMD, 
                                           rf_tdscdma_proc_rflm_therm_trigger_rsp,
                                           NULL,
                                           1<<RF_DISPATACH_MSGR_REG );

  /* Register command to trigger AutoPin read */
  ret_val &= rf_dispatch_register_command( rf_task_get_dispatch_handle(),
                                           MSGR_TECH_RFLM, 
                                           RFLM_TDSCDMA_TRIGGER_AUTOPIN_CMD, 
                                           rf_tdscdma_proc_rflm_autopin_trigger_rsp,
                                           NULL,
                                           1<<RF_DISPATACH_MSGR_REG );

  RF_MSG_1( RF_HIGH, "rf_tdscdma_msgr_rflm_register: [Status %d]", ret_val );

  return ret_val;
}

/*===========================================================================

  FUNCTION:  rf_tdscdma_msgr_rflm_deregister

===========================================================================*/
/*!
  @brief

  @details This funtion is used to deregister RFLM messages.
 
*/
boolean rf_tdscdma_msgr_rflm_deregister(void)
{
  boolean ret_val = TRUE; /*  return variable  */

  /*----------------------------------------------------------------------------------------------*/
  /* De-Register all commands */
  /*----------------------------------------------------------------------------------------------*/

  ret_val &= rf_dispatch_deregister_command( rf_task_get_dispatch_handle(),
                                             MSGR_TECH_RFLM, 
                                             RFLM_TDSCDMA_TRIGGER_PDET_CMD );

  RF_MSG_1( RF_HIGH, "rf_tdscdma_msgr_rflm_deregister: [Status %d]", ret_val );

  return ret_val;

}

/*===========================================================================

  FUNCTION:  rf_tdscdma_req_dispatcher

===========================================================================*/
/*!
  @brief

  @details This funtion is the rf command dispatcher function for the TDSCDMA L1 Commands.
  This function has to be registered with the rf command dispatch handler during TDSCDMA Initializtion.
*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the common command handler for rf tdscdma main control commands.

  @param module_id
  Module ID identifies the "technology" i.e TDSCDMA, UMTS, etc. 
  in which the given command belongs.

  @param req
  Command ID is a unique enumeration value that identifies this interface.

  @param req_payload_size
  The size of the structure specific to this interface.

  @param payload
  A pointer to a structure containing the parameters specific to this interface.

  @return
  void
*/

/*=========================================================================*/

void rf_tdscdma_req_dispatcher( 
 rf_module_id_type module_id, rf_cmd_id_type req_id, rf_cmd_size_type req_payload_size, rf_cmd_type* req_ptr
)
{
  rf_tdscdma_dispatch_outcome_t  disp_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_msg_u rf_tdscdma_msgr_req;
  rf_tdscdma_msg_u *rf_tdscdma_msgr_ptr = NULL;

  if(req_ptr != NULL)
  {
    /* Copy the MSGR request packet*/
    rf_tdscdma_msgr_ptr = (rf_tdscdma_msg_u *)&(req_ptr->msgr_hdr);

    memscpy( &rf_tdscdma_msgr_req, 
            sizeof(rf_tdscdma_msg_u),
            rf_tdscdma_msgr_ptr, 
            (sizeof(req_ptr->msgr_hdr)+ RF_MAX_CMD_PAYLOAD_SIZE)
          );


    disp_status = rf_tdscdma_req_dispatch(&rf_tdscdma_msgr_req);

    if (disp_status != RF_TDSCDMA_DISPATCH_SUCESSFUL)
    {
      MSG_ERROR("TDSCDMA command recieved from rf command dispatcher. TDSCDMA Command"
                "processor could not process the command. Exited with error code"
                "%d",disp_status,0,0);
    }
  } /* end of if(req_ptr != NULL) */
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_mdsp_msg_dispatcher

===========================================================================*/
/*!
  @brief

  @details This funtion is the rf command dispatcher function for TDSCDMA MDSP msgs.
  This function has to be registered with the rf task dispatcher during TDSCDMA Initializtion.
*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the common command handler for rf tdscdma mdsp msgs.

  @return
  void
*/

/*=========================================================================*/
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
void rf_tdscdma_mdsp_msg_dispatcher( rf_cmd_type* req_ptr )
{
  rf_tdscdma_dispatch_outcome_t  disp_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mdsp_msg_u  rf_tdscdma_msgr_req;
  rf_tdscdma_mdsp_msg_u  *rf_tdscdma_msgr_ptr = NULL;
  msgr_umid_type rf_uimd; /* Received UMID */

 
  rf_tdscdma_msg_u rf_tdscdma_ns_msgr_req;
  rf_tdscdma_msg_u *rf_tdscdma_ns_msgr_ptr = NULL;

  size_t tmp_size;


  /* Validate Request Pointer */
  if( req_ptr == NULL )
  {
    MSG_ERROR( "rf_tdscdma_mdsp_msg_dispatcher: NULL Request",0,0,0 );
    return;
  }

  /* Get UMID */
  rf_uimd = req_ptr->msgr_hdr.id;


  /* Check if the dispatched message is valid */
  if( !((MSGR_TECH_MODULE_VAL(rf_uimd) == MSGR_TDSCDMA_FW) || (MSGR_MODULE_VAL(rf_uimd) == MSGR_MODULE_L1NS)) )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mdsp_msg_dispatcher: not a tds mdsp or l1ns UMID 0x%x", rf_uimd );
    return;
  }

  if( MSGR_TECH_MODULE_VAL(rf_uimd) == MSGR_TDSCDMA_FW )
  {
  /*Copy the MSGR request packet*/
  rf_tdscdma_msgr_ptr = (rf_tdscdma_mdsp_msg_u *)&(req_ptr->msgr_hdr);

  memscpy( &rf_tdscdma_msgr_req, 
          sizeof(rf_tdscdma_mdsp_msg_u),
          rf_tdscdma_msgr_ptr, 
          (sizeof(req_ptr->msgr_hdr)+ RF_MAX_CMD_PAYLOAD_SIZE)
        );

  disp_status = rf_tdscdma_mdsp_msg_dispatch(&rf_tdscdma_msgr_req);

  if (disp_status != RF_TDSCDMA_DISPATCH_SUCESSFUL)
  {
    MSG_ERROR("TDSCDMA command recieved from rf command dispatcher. TDSCDMA msg"
              "processor could not process the msg. Exited with error code"
              "%d",disp_status,0,0);
  }
  }

  else if (MSGR_MODULE_VAL(rf_uimd) == MSGR_MODULE_L1NS)
  {
     /* Copy the MSGR request packet*/
    rf_tdscdma_ns_msgr_ptr = (rf_tdscdma_msg_u *)&(req_ptr->msgr_hdr);
    
    tmp_size = (sizeof(rf_tdscdma_mdsp_msg_u) <= (sizeof(req_ptr->msgr_hdr)+ RF_MAX_CMD_PAYLOAD_SIZE))? sizeof(rf_tdscdma_mdsp_msg_u) : (sizeof(req_ptr->msgr_hdr)+ RF_MAX_CMD_PAYLOAD_SIZE); //To fix kw warnings
    	
    memscpy(&rf_tdscdma_ns_msgr_req, 
            tmp_size,
            rf_tdscdma_ns_msgr_ptr, 
            tmp_size
           );

    disp_status = rf_tdscdma_req_dispatch(&rf_tdscdma_ns_msgr_req);

    if (disp_status != RF_TDSCDMA_DISPATCH_SUCESSFUL)
    {
      MSG_ERROR("TDSCDMA command recieved from rf command dispatcher. TDSCDMA Command"
                "processor could not process the command. Exited with error code"
                "%d",disp_status,0,0);
    }


  }

}
#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
void rf_tdscdma_mdsp_msg_dispatcher( rf_cmd_type* req_ptr )
{
  rf_tdscdma_dispatch_outcome_t  disp_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  rf_tdscdma_mdsp_msg_u  rf_tdscdma_msgr_req;
  rf_tdscdma_mdsp_msg_u  *rf_tdscdma_msgr_ptr = NULL;
  msgr_umid_type rf_uimd; /* Received UMID */

  /* Validate Request Pointer */
  if( req_ptr == NULL )
  {
    MSG_ERROR( "rf_tdscdma_mdsp_msg_dispatcher: NULL Request",0,0,0 );
    return;
  }

  /* Get UMID */
  rf_uimd = req_ptr->msgr_hdr.id;

  /* Check if the dispatched message is valid */
  if( MSGR_TECH_MODULE_VAL(rf_uimd) != MSGR_TDSCDMA_FW )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mdsp_msg_dispatcher: not a tds mdsp UMID 0x%x", rf_uimd );
    return;
  }

  /*Copy the MSGR request packet*/
  rf_tdscdma_msgr_ptr = (rf_tdscdma_mdsp_msg_u *)&(req_ptr->msgr_hdr);

  memscpy( &rf_tdscdma_msgr_req, 
          sizeof(rf_tdscdma_mdsp_msg_u),
          rf_tdscdma_msgr_ptr, 
          (sizeof(req_ptr->msgr_hdr)+ RF_MAX_CMD_PAYLOAD_SIZE)
        );

  disp_status = rf_tdscdma_mdsp_msg_dispatch(&rf_tdscdma_msgr_req);

  if (disp_status != RF_TDSCDMA_DISPATCH_SUCESSFUL)
  {
    MSG_ERROR("TDSCDMA command recieved from rf command dispatcher. TDSCDMA msg"
              "processor could not process the msg. Exited with error code"
              "%d",disp_status,0,0);
  }

}
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/



/*===========================================================================

  FUNCTION:  rf_tdscdma_msg_dispatch_register

===========================================================================*/
/*!
  @brief

  @details This funtion has to be called in the rfm_init_tdscdma before a RF TDSCDMA L1 command is
  sent to RF Driver task. Registration with rf module dispatch table and rf command dispatch table
  for the RF TDSCDMA module.
*/
/*=========================================================================*/
void rf_tdscdma_msg_dispatch_register(void)
{
  uint16 num_of_req = (RFA_TDSCDMA_MSG_MAX_ID - RFA_TDSCDMA_MSG_ID_ENTER_MODE) + 1;
  uint16 min_req_id = RFA_TDSCDMA_MSG_ID_ENTER_MODE;
  uint16 max_req_id = RFA_TDSCDMA_MSG_MAX_ID;
  
  /*Register with rf module dispatch table using RF TDSCDMA as the module.*/
  rf_cmd_dispatch_register_rf_module( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, num_of_req, min_req_id, max_req_id );

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
  //NS
  rf_cmd_dispatch_register_rf_module( RF_TASK_MAIN, MSGR_MODULE_L1NS, 0x0B, 0, 0x0A );

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

  //////////////////////////// REGISTER RF TDSCDMA COMMANDS////////////////////////////
  /*Register with the rf command dispatch table for individual command handlers.*/
    
  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_ENTER_MODE, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_EXIT_MODE, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_ENABLE_RX, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_INIT_TX, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_ENABLE_TX, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_DISABLE_TX, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_SLEEP, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );
     
  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_WAKEUP, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );
         
  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_SET_TX_PWR_LIMIT, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );
         
  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_GET_RF_WARMUP_TIME, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );
         
  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_GET_RF_TXCHAIN_NO, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );     
         
  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_UPDATE_BHO, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_SWITCH_BUF, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_UPDATE_TDS_NBR, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_UPDATE_GSM_NBR, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_GET_MAX_TX_PWR, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_THERM_BACKOFF, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_SET_COEX_TX_PWR_LIMIT, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_GET_RXAGC_VAL, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_GET_AGC_LOG, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_UPDATE_TUNER, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_ANT_SW, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_ANT_SW_ABORT, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_DYNAMIC_SCRIPT, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA, 
     RFA_TDSCDMA_MSG_ID_RXD_OVERRIDE, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );
     
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
  //NS
  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_L1NS, 
     TDSL1_NS_CPHY_STOP_TDSCDMA_MODE_CNF_CODE, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_L1NS, 
     TDSL1_NS_CPHY_ACQ_CNF_CODE, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_L1NS, 
     TDSL1_NS_CPHY_SETUP_CNF_CODE, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_L1NS, 
     TDSL1_NS_CPHY_PHYCHAN_ESTABLISHED_IND_CODE, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_L1NS, 
     TDSL1_NS_CPHY_RL_FAILURE_IND_CODE, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );

  rf_cmd_dispatch_register_handler( RF_TASK_MAIN, MSGR_MODULE_L1NS, 
     TDSL1_NS_CPHY_ERROR_IND_CODE, rf_tdscdma_req_dispatcher, sizeof(rfcom_device_enum_type) );


#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_msg_dispatch_deregister

===========================================================================*/
/*!
  @brief
  This funtion has to be called whenever the RF task is killed. Frees allocated
  memory and registration tables. 

  @return
  void
*/
/*=========================================================================*/
void rf_tdscdma_msg_dispatch_deregister(void)
{
  /* Clean up the rf command dispatch table which contains all the pointers to individual command handler. 
     The rf module dispatch table is cleaned by rf_req_dispatcher when rf task is killed.*/
  rf_cmd_dispatch_deregister_rf_module( RF_TASK_MAIN, MSGR_MODULE_RF_TDSCDMA );
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
  rf_cmd_dispatch_deregister_rf_module( RF_TASK_MAIN, MSGR_MODULE_L1NS );
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
  
}
#endif /*#ifdef FEATURE_RF_TASK*/


/*===========================================================================

  FUNCTION:  rf_tdscdma_req_dispatch

===========================================================================*/
/*!
  @brief
  This function is the main dispatch function for all the TDSCDMA REQs the RF
  task will receive. Once it receives a command, it basically dispatches
  it to the appropriate function again in this file. That function will
  basically send the command to the repective MC function and then once that 
  is executed, it will send back a the reponse. That function will also echo 
  the arguments of the original command.

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_req_dispatch(rf_tdscdma_msg_u* msg_ptr)
{
  rf_tdscdma_dispatch_outcome_t  disp_status;
  msgr_hdr_s  *msgr_hdr_ptr = (msgr_hdr_s*) msg_ptr; 
 
   if ( l1_req_log.idx>= 100)
  {
    /* Reset to Zero */
    l1_req_log.idx = 0;
  }

 
  if (msg_ptr != NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "RF TDSCDMA req dispatch, MSG ID=%d", msgr_hdr_ptr->id);

    switch ( msgr_hdr_ptr->id )
    {
    case RFA_RF_TDSCDMA_ENTER_MODE_REQ:
      disp_status = rf_tdscdma_proc_enter_mode_req((rfa_tdscdma_enter_mode_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_ENTER_MODE;
      break;

    case RFA_RF_TDSCDMA_EXIT_MODE_REQ:  
      disp_status = rf_tdscdma_proc_exit_mode_req((rfa_tdscdma_exit_mode_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_EXIT_MODE;
      break;

    case RFA_RF_TDSCDMA_ENABLE_RX_REQ:
      disp_status = rf_tdscdma_proc_enable_rx_req((rfa_tdscdma_enable_rx_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_ENABLE_RX;
      break;   

    case RFA_RF_TDSCDMA_INIT_TX_REQ:
      disp_status = rf_tdscdma_proc_init_tx_req((rfa_tdscdma_init_tx_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_INIT_TX;
      break;

    case RFA_RF_TDSCDMA_ENABLE_TX_REQ:
      disp_status = rf_tdscdma_proc_enable_tx_req((rfa_tdscdma_enable_tx_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_ENABLE_TX;
      break;

    case RFA_RF_TDSCDMA_DISABLE_TX_REQ:
      disp_status = rf_tdscdma_proc_disable_tx_req((rfa_tdscdma_disable_tx_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_DISABLE_TX;
      break;

    case RFA_RF_TDSCDMA_SLEEP_REQ:
      disp_status = rf_tdscdma_proc_sleep_req((rfa_tdscdma_sleep_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_SLEEP;
      break;

    case RFA_RF_TDSCDMA_WAKEUP_REQ:
      disp_status = rf_tdscdma_proc_wakeup_req((rfa_tdscdma_wakeup_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_WAKEUP;
      break;

    case RFA_RF_TDSCDMA_SET_TX_PWR_LIMIT_REQ:
      disp_status = rf_tdscdma_proc_set_tx_pwr_limit_req((rfa_tdscdma_set_tx_pwr_limit_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_SET_TX_PWR_LIMIT;
      break;

    case RFA_RF_TDSCDMA_GET_RF_WARMUP_TIME_REQ:
      disp_status = rf_tdscdma_proc_get_rf_warmup_time_req((rfa_tdscdma_get_rf_warmup_time_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_GET_RF_WARMUP_TIME;
      break;

    case RFA_RF_TDSCDMA_GET_RF_TXCHAIN_NO_REQ:
      disp_status = rf_tdscdma_mc_get_rf_txchain_no_req((rfa_tdscdma_get_rf_txchain_no_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_GET_RF_TXCHAIN_NO;
      break;

    case RFA_RF_TDSCDMA_UPDATE_BHO_REQ:
      disp_status = rf_tdscdma_proc_update_bho_req((rfa_tdscdma_update_bho_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_UPDATE_BHO;
      break;

    case RFA_RF_TDSCDMA_SWITCH_BUF_REQ:
      disp_status = rf_tdscdma_proc_switch_buf_req((rfa_tdscdma_switch_buf_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_SWITCH_BUF;
      break;

    case RFA_RF_TDSCDMA_UPDATE_TDS_NBR_REQ:
      disp_status = rf_tdscdma_proc_update_tds_nbr_req((rfa_tdscdma_update_tds_nbr_req_s*) msg_ptr);
     l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_UPDATE_TDS_NBR;
      break;

    case RFA_RF_TDSCDMA_UPDATE_GSM_NBR_REQ:
      disp_status = rf_tdscdma_proc_update_gsm_nbr_req((rfa_tdscdma_update_gsm_nbr_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_UPDATE_GSM_NBR;
      break;

    case RFA_RF_TDSCDMA_GET_MAX_TX_PWR_REQ:
      disp_status = rf_tdscdma_proc_get_max_tx_pwr_req((rfa_tdscdma_get_max_tx_pwr_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_GET_MAX_TX_PWR;
      break;

    case RFA_RF_TDSCDMA_THERM_BACKOFF_REQ:
      disp_status = rf_tdscdma_proc_therm_backoff_req((rfa_tdscdma_therm_backoff_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_THERM_BACKOFF;
      break;

    case RFA_RF_TDSCDMA_SET_COEX_TX_PWR_LIMIT_REQ:
      disp_status = rf_tdscdma_proc_set_coex_tx_pwr_limit_req((rfa_tdscdma_set_coex_tx_pwr_limit_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_SET_COEX_TX_PWR_LIMIT;
      break;

   case RFA_RF_TDSCDMA_UPDATE_TUNER_REQ:
      disp_status = rf_tdscdma_update_tuner_req((rfa_tdscdma_update_tuner_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_UPDATE_TUNER;
      break;

   case RFA_RF_TDSCDMA_ANT_SW_REQ:
      disp_status = rf_tdscdma_proc_ant_sw_req((rfa_tdscdma_ant_sw_req_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_ANT_SW;
      break;

   case RFA_RF_TDSCDMA_ANT_SW_ABORT_IND:
      disp_status = rf_tdscdma_proc_ant_sw_abort_ind((rfa_tdscdma_ant_sw_abort_ind_s*) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_ANT_SW_ABORT;
      break;
      
   case RFA_RF_TDSCDMA_DYNAMIC_SCRIPT_REQ:
      disp_status = rf_tdscdma_proc_dynamic_script_req((rfa_tdscdma_dynamic_script_req_s *) msg_ptr);
      l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_DYNAMIC_SCRIPT;
      break;    

    case RFA_RF_TDSCDMA_RXD_OVERRIDE_REQ:
       disp_status = rf_tdscdma_proc_rxd_override_req((rfa_tdscdma_rxd_override_req_s *) msg_ptr);
       l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_RXD_OVERRIDE;
       break;  

    //case RFA_RF_TDSCDMA_GET_RXAGC_VAL_REQ:
    //  disp_status = rf_tdscdma_proc_get_rxagc_val_req((rfa_tdscdma_get_rxagc_val_req_s*) msg_ptr);
    // l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_GET_RXAGC_VAL;
    //  break;

    //case RFA_RF_TDSCDMA_GET_AGC_LOG_REQ:
    //  disp_status = rf_tdscdma_proc_get_agc_log_req((rfa_tdscdma_get_agc_log_req_s*) msg_ptr);
    // l1_req_log.l1_log[l1_req_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_GET_AGC_LOG;
    //  break;
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
    case TDSCDMA_L1NS_CPHY_START_CNF:
    case TDSCDMA_L1NS_CPHY_STOP_CNF:
    case TDSCDMA_L1NS_CPHY_ACQ_CNF:
    case TDSCDMA_L1NS_CPHY_SETUP_CNF:
    case TDSCDMA_L1NS_CPHY_ESTABLISHED_IND:
    case TDSCDMA_L1NS_CPHY_RL_FAILURE_IND:
    case TDSCDMA_L1NS_CPHY_ERROR_IND:
    case TDSCDMA_L1NS_DL_DATA_IND:
      disp_status = ftm_tdscdma_ber_process_l1_messages_msgr(msgr_hdr_ptr );

      break;
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/


    default :
      MSG_ERROR("Undefined TDSCDMA REQ",0,0,0);
      disp_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
      break;
    }
   l1_req_log.l1_log[l1_req_log.idx].l1_msg_umid=msgr_hdr_ptr->id;
   l1_req_log.idx++;
  } 
  else
  {
    MSG_ERROR("Pointer NULL",0,0,0);
    disp_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
  }

  return(disp_status);
}


/*===========================================================================

  FUNCTION:  rf_tdscdma_mdsp_msg_dispatch

===========================================================================*/
/*!
  @brief
  This function is the main dispatch function for all the TDSCDMA MDSP msgs

  @return
  rf_tdscdma_dispatch_outcome_t.
*/
/*=========================================================================*/
rf_tdscdma_dispatch_outcome_t rf_tdscdma_mdsp_msg_dispatch(rf_tdscdma_mdsp_msg_u* msg_ptr)
{
  rf_tdscdma_dispatch_outcome_t  disp_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
  msgr_hdr_s  *msgr_hdr_ptr = (msgr_hdr_s*) msg_ptr; 
  tfw_state_cfg_rsp_t* state_cfg_msg_ptr;

  if ( mdsp_rsp_log.idx>= 100)
  {
    /* Reset to Zero */
    mdsp_rsp_log.idx = 0;
  }

  if (msg_ptr != NULL)
  {
    switch ( msgr_hdr_ptr->id )
    {
    case TDSCDMA_FW_STATE_CFG_RSP:
      disp_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
      mdsp_rsp_log.mdsp_log[mdsp_rsp_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_FW_STATE_CFG_RSP;
      state_cfg_msg_ptr = (tfw_state_cfg_rsp_t*) msg_ptr;
      MSG_MED("TDSCDMA MDSP STATE CONFIG RSP msg, tfw_state = %d",state_cfg_msg_ptr->state,0,0);
      //Do nothing for now, need to wait for response
      break;

    case TDSCDMA_FW_RSP_FTM_RX_RSP:
      mdsp_rsp_log.mdsp_log[mdsp_rsp_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_FW_FTM_RX_RSP; 
      disp_status = rf_tdscdma_proc_rfmode_rx_cmd_rsp((tfw_ftm_rx_cmd_rsp_t*)msg_ptr);
      break;

    case TDSCDMA_FW_RSP_FTM_TX_RSP:
      mdsp_rsp_log.mdsp_log[mdsp_rsp_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_FW_FTM_TX_RSP;
      disp_status = rf_tdscdma_proc_rfmode_tx_cmd_rsp((tfw_ftm_tx_cmd_rsp_t*)msg_ptr);
      break;

    case TDSCDMA_FW_IQ_CAPTURE_RSP:
      disp_status = rf_tdscdma_proc_mdsp_iqcapture_rsp((tfw_iq_capture_rsp_t*) msg_ptr);
      mdsp_rsp_log.mdsp_log[mdsp_rsp_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_FW_IQ_CAPTURE_RSP;
      break;

    case TDSCDMA_FW_FTM_GET_MULTI_SYNTH_STATE_RSP:
	  disp_status = rf_tdscdma_proc_mdsp_get_multi_synth_rsp((tfw_ftm_get_multi_synth_state_rsp_t*) msg_ptr);
	  mdsp_rsp_log.mdsp_log[mdsp_rsp_log.idx].msg_enum=RFA_TDSCDMA_MSG_ID_FW_GET_MULTI_SYNTH_RSP;
	  break;

    default :
      MSG_ERROR("Undefined TDSCDMA mdsp msg",0,0,0);
      disp_status = RF_TDSCDMA_DISPATCH_UNSUCESSFUL;
      break;
    }
    mdsp_rsp_log.mdsp_log[mdsp_rsp_log.idx].mdsp_msg_umid=msgr_hdr_ptr->id;
    mdsp_rsp_log.idx++;
  } 
  else
  {
    MSG_ERROR("Pointer NULL",0,0,0);
    disp_status = RF_TDSCDMA_DISPATCH_NULL_PTR;
  }

  return(disp_status);
}
