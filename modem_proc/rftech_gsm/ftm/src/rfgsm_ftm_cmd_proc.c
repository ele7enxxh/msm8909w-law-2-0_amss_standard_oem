/*!
  @file rf_gsm_ftm_cmd_proc.c 

  @brief
  This file dispatches and sends responses for any RFGSM FTM CMD that 
  the GSM RF driver receives.



*/

/*===========================================================================
  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/ftm/src/rfgsm_ftm_cmd_proc.c#1 $ 
$DateTime: 2016/12/13 07:59:51 $ 
$Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/02/14   ec      Add specific override for when SAWless Linearity override is applied to a channel
09/19/14   sb      Add IP2 cal moving averaging + pass/fail status
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
08/01/14   ec      Add code to handle global lin override for SAWless targets
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/14/14   ec      Add APIs to process new SAWless FTM commands
05/29/14   sb      IP2 cal bug fixes, updates
04/16/14   sb      Fix KW
04/16/14   sb      IP2 cal Interface and functionality changes
03/26/14   sb      Add support for IP2 cal for Dime PM
02/28/14   sc      Pass core data through mc to core layers
11/05/13   ec	   Remove empty parameters from F3 macros to save memory
08/07/13   ec      Include declaration of ftm_gsm_init_iqcapture_buffer to fix compiler
                   warning
07/30/13   ec      Move IQ capture configuration from FTM to RF Task context 
07/29/13   tws     Use RF_TASK_1 for GSM.
07/18/13   tsr     Fix FTM PA Range override bug 
05/06/13   aka     Clear PA state and RGI value when Tx pwr is set in dBm
04/22/13   sb      Add FTM hooks to override QPOET bias
04/18/13   sb      Add FTM hooks for overriding envelope gain, PA scaling and SMPS bias
03/28/13   aka     GSM frame sweep feature changes.
03/18/13   svi     Replaced memcpy() by safer memscpy().
02/15/13   sb      Added FTM hook to override tuner tune code 
12/10/12   tws     Set the path delay cal override flag.
12/07/12   tws     Fix Klocwork errors.
12/06/12   tws     Fix compiler warnings, back out previous change.
12/06/12   tws     Fix Klocwork errors.
11/27/12   tws     Add RFM device to all FTM command handlers
11/26/12   aka     Changes to use a path specific gsm handle
11/21/12   gvn     Featurize LTE for Triton 
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/11/12   ec      Added code to reset rfgsm_card override variables when we are no 
                   longer overriding parameters (i.e. frame matrix resent from host) 
11/05/12   ec      Fix compiler warning caused by unused variable (stop-gap until RF Task 2
                   is fully implemented)
10/05/12   av      Expanded rf task architecture to include more tasks. 
10/04/12   sc      Move to new RF Task dispatcher method to avoid UMID REX conflicts
08/16/12   ec      Cleanup of set pa range implementation
08/09/12   ec      Added forward declarations for rfgsm_core_set_tx_profile_update_flag 
                   and rfgsm_core_set_slot_override_flag to fix compilation warnings 
08/01/12   ec      Added processing for set path delay, set linear rgi, get pa range, 
                   set slot override flag, modified set pa range, and added debug to
                   cmd dispatcher.
07/18/12   ec      Implemented rfgsm_ftm_set_tx_pow_dbm to set tx power via MSGR from 
                   FTM task 
05/24/12   ec      Added rfgsm_ftm_set_band function and added to dispatcher 
                   (SET_BAND_CMD replaces SET_CHAN_CMD) 
05/10/12   ec/ggs  Added Set Path Delay to command dispatcher, implemented tune
				   tx, set_pow_dbm, set_slot_override_flag and linear_pa_range
				   cmd handlers and added to dispatcher
04/02/12   ggs     Added Set PA range to command dispatcher
03/12/12   ggs     Initial version based upon rf_lte_ftm_cmd_proc.h
  

===========================================================================*/

/*===========================================================================

                                  INCLUDE FILES

===========================================================================*/

/*! @todo Remove unnecessary header files */
#include "msgr.h"
#include "msg.h"
#ifdef FEATURE_LTE
#include "rf_control_msg.h"
#endif /*FEATURE_LTE*/
#include "rfa_msgr.h"
#include "rf_task.h"
#include "rf_cmd_interface.h"
#include "rf_cmd_dispatcher.h"
/* FTM_MSG_MED is defined in ftm_task. Hence needed.*/
#include "ftm_task.h"
#include "ftm_gsm_rfctl.h"
#include "rfgsm_ftm_cmd_proc.h"
#include "rfm_gsm.h"
#include "rfgsm_core.h"
#include "stringl.h"
#include "rfgsm_data_manager.h"


/*===========================================================================

                                DEFINITIONS

===========================================================================*/
/*! @brief This array holds the command list used to register
 *  the whole list with MSGR.
 */
static msgr_umid_type rfgsm_ftm_umid_cmd_list[] = 
{


/*! @todo The remainder of these commands are placeholders for possible future use */
    RFA_RF_GSM_FTM_SET_MODE_CMD,
    RFA_RF_GSM_FTM_SET_TX_ON_CMD,
    RFA_RF_GSM_FTM_SET_TX_OFF_CMD,
    RFA_RF_GSM_FTM_SET_LNA_RANGE_CMD,
    RFA_RF_GSM_FTM_SET_PDM_CMD,
    RFA_RF_GSM_FTM_SET_BAND_CMD,
    RFA_RF_GSM_FTM_SET_TRANSMIT_CONT_CMD,
    RFA_RF_GSM_FTM_SET_TRANSMIT_BURST_CMD,
    RFA_RF_GSM_FTM_SET_RX_BURST_CMD,
    RFA_RF_GSM_FTM_GET_RSSI_CMD,
    RFA_RF_GSM_FTM_SET_PA_START_DELTA_CMD,
    RFA_RF_GSM_FTM_SET_PA_STOP_DELTA_CMD,
    RFA_RF_GSM_FTM_SET_PA_DAC_INPUT_CMD,
    RFA_RF_GSM_FTM_SET_RX_CONTINUOUS_CMD,
    RFA_RF_GSM_FTM_SET_PATH_DELAY_CMD,
    RFA_RF_GSM_FTM_SET_SLOT_OVERRIDE_FLAG_CMD,
    RFA_RF_GSM_FTM_RX_GAIN_RANGE_CAL_CMD,
    RFA_RF_GSM_FTM_TX_KV_CAL_CMD,
    RFA_RF_GSM_FTM_TX_KV_CAL_V2_CMD,
    RFA_RF_GSM_FTM_SET_LINEAR_RGI_CMD,
    RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_CMD,
    RFA_RF_GSM_FTM_CAPTURE_IQ_CMD,
    RFA_RF_GSM_FTM_SET_RX_TIMING_CMD, /* Legacy ftm_gsm_extended dispatch commands */
    RFA_RF_GSM_FTM_SET_TX_TIMING_CMD,
    RFA_RF_GSM_FTM_SET_TX_GAIN_SWEEP_CMD,
    RFA_RF_GSM_FTM_TX_ENVDC_CS_SWEEP_CMD,
    RFA_RF_GSM_FTM_CFG2_AMAM_SWEEP_V2_CMD,
    RFA_RF_GSM_FTM_SET_RX_MULTISLOT_CMD,
    RFA_RF_GSM_FTM_SET_RX_BURST_FOR_EXPECTED_PWR_CMD,
    RFA_RF_GSM_FTM_GET_MEAS_PWR_CMD,
    RFA_RF_GSM_FTM_SET_TX_POW_DBM_CMD,
    RFA_RF_GSM_FTM_TUNE_TX_CMD,
    RFA_RF_GSM_FTM_TUNER_TUNE_CODE_OVERRIDE_CMD,
    RFA_RF_GSM_FTM_SET_IP2_CAL_PARAMS_CMD,
    RFA_RF_GSM_FTM_GET_IP2_CAL_PARAMS_CMD,
    RFA_RF_GSM_FTM_GET_IP2_CAL_RESULTS_CMD,
    RFA_RF_GSM_FTM_SET_SAWLESS_LIN_MODE_CMD,
    RFA_RF_GSM_FTM_GET_SAWLESS_LIN_MODE_CMD,
};



/*===========================================================================

                               Macros

===========================================================================*/
/*! @brief Definitions of the RF GSM L1 message count for the mailboxes.
*/
#define RF_GSM_FTM_UMID_CNT \
    (sizeof(rfgsm_ftm_umid_cmd_list)/sizeof(msgr_umid_type))

#define IP2_CAL_DC_SETTLING_TIME_OFFSET 20  /* Obtained from profiling the time takes to get CCS callback*/

/*===========================================================================
 
                   EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_cmd_dispatch(rfgsm_ftm_msg_u* msg_ptr);

extern void rfgsm_core_set_tx_profile_update_flag( rfgsm_core_handle_type *rfgsm_core_handle_ptr );
extern void rfgsm_core_set_slot_override_flag(rfgsm_core_handle_type *rfgsm_core_handle_ptr, boolean override_val);

/*===========================================================================

  FUNCTION:  rfgsm_ftm_set_mode_cmd

===========================================================================*/
/*! 
  @todo
  This is a dummy placeholder function 
 
  @brief
  This is a dummy placeholder function 
  This function is called by rf_gsm_ftm_internal_cmd_dispatch, and calls the respective RF
  function and then sends a RSP MSG back and echos the arguments of the 
  original command.

  @return
  rfgsm_dispatch_outcome_t.
*/
/*=========================================================================*/
rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_set_mode_cmd( rfa_rf_gsm_ftm_set_mode_type_cmd_s* rfgsm_ftm_msg_ptr )
{
  rfa_rf_gsm_ftm_generic_message_type_rsp_s rsp_msg;
  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  errno_enum_type ret_status;

  rsp_msg.status = 0;

  if (1==1) /*! @todo Check return status of internal function here */
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "FTM Enter GSM mode error" );
    rsp_msg.status = 1;
  }

  rsp_msg.hdr.id = 0;
  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_MODE_RSP);
  ret_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_generic_message_type_rsp_s) );
  if (ret_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_SET_MODE_RSP, cmd_cod=%d, err=%d",
                  rsp_msg.hdr.id, ret_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_SET_MODE_RSP, cmd_cod=%d",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }
  return(cmd_rsp);
}

/*===========================================================================

  FUNCTION:  rfgsm_ftm_set_tx_pow_dbm

===========================================================================*/
/*!
  @brief
  This function is called by rf_gsm_ftm_internal_cmd_dispatch, and calls the respective RF
  function and then sends a RESP MSG back and echoes the arguments of the 
  original command.

  @return
  rfgsm_dispatch_outcome_t.
*/
/*=========================================================================*/
rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_set_tx_pow_dbm ( rfa_rf_gsm_ftm_set_tx_pow_dbm_type_cmd_s* rfgsm_ftm_msg_ptr)
{
  /* Initialise response and result variables */
  rfa_rf_gsm_ftm_set_tx_pow_dbm_type_rsp_s rsp_msg;
  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  errno_enum_type msgr_send_status = E_SUCCESS;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  if( rfgsm_ftm_msg_ptr == NULL )
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer argument is NULL in rfgsm_ftm_set_tx_pow_dbm" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfgsm_ftm_msg_ptr->rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!" );
	return cmd_rsp;
  }
  

  /* Echo CMD message parameters in RSP message. RSP message may include some updated parameter info */
  rsp_msg.hdr                      = rfgsm_ftm_msg_ptr->hdr;
  rsp_msg.rfm_dev                  = rfgsm_ftm_msg_ptr->rfm_dev;
  rsp_msg.set_tx_pow_dbm_params    = rfgsm_ftm_msg_ptr->set_tx_pow_dbm_params;
  rsp_msg.cmd_result               = RFA_RF_GSM_FTM_EXECUTION_ERROR;

  /*! @todo If required, verify payload parameters. */
  cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  
  /*update Linear PA and RGI settings to default values*/
  rfgsm_core_handle_ptr->ftm_slot_override[rfgsm_ftm_msg_ptr->set_tx_pow_dbm_params.index].pa_range = 0xFFFF;
  rfgsm_core_handle_ptr->ftm_slot_override[rfgsm_ftm_msg_ptr->set_tx_pow_dbm_params.index].linear_rgi = 0xFFFF;

  rfgsm_core_handle_ptr->ftm_slot_override[rfgsm_ftm_msg_ptr->set_tx_pow_dbm_params.index].tx_pow_dbm = rfgsm_ftm_msg_ptr->set_tx_pow_dbm_params.pow_dbm;

  /*! @todo Check return value from rfm interface */
  rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;

  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_TX_POW_DBM_RSP );
  msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_set_tx_pow_dbm_type_rsp_s) );
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_SET_TX_POW_DBM_RSP, UMID=0x%x, msgr_send_status=%d",
                  rsp_msg.hdr.id, msgr_send_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_SET_TX_POW_DBM_RSP, UMID=0x%x",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }
  return(cmd_rsp);
}
/*===========================================================================

  FUNCTION:  rfgsm_ftm_set_path_delay

===========================================================================*/
/*!
  @brief
  This function is called by rf_gsm_ftm_internal_cmd_dispatch, and calls the respective RF
  function and then sends a RESP MSG back and echos the arguments of the 
  original command.

  @return
  rfgsm_dispatch_outcome_t.
*/
/*=========================================================================*/

rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_set_path_delay( rfa_rf_gsm_ftm_set_path_delay_type_cmd_s* rfgsm_ftm_msg_ptr )
{
  /* Initialise response and result variables */
  rfa_rf_gsm_ftm_set_path_delay_type_rsp_s rsp_msg;
  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  errno_enum_type msgr_send_status = E_SUCCESS;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  if( rfgsm_ftm_msg_ptr == NULL )
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer argument is NULL in rfgsm_ftm_set_path_delay" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfgsm_ftm_msg_ptr->rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!" );
    return cmd_rsp;
  }

  /* Echo CMD message parameters in RSP message. RSP message may include some updated parameter info */
  rsp_msg.hdr                       = rfgsm_ftm_msg_ptr->hdr;
  rsp_msg.rfm_dev                   = rfgsm_ftm_msg_ptr->rfm_dev;
  rsp_msg.delay_value               = rfgsm_ftm_msg_ptr->delay_value;
  rsp_msg.cmd_result                = RFA_RF_GSM_FTM_EXECUTION_ERROR;

  /*! @todo If required, verify payload parameters. */
  cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;

  /*Update delay value*/
  /*Do not need to call RF apis to set delay now, path delay will be set from rfgsm_card inside rfgsm_core_gsmctl_set_tx_power*/
  //rfm_gsm_set_path_delay(rsp_msg.delay_value);
  rfgsm_core_handle_ptr->ftm_path_delay_override_val = rfgsm_ftm_msg_ptr->delay_value;  //set the override value

  // Set the override flag. This is cleared automatically in normal operation
  rfgsm_core_handle_ptr->ftm_path_delay_cal_override = TRUE;

  /*! @todo create wrappers for the below function in rfm & rfgsm_mc layers as part of clean-up */
  rfgsm_core_set_tx_profile_update_flag(rfgsm_core_handle_ptr);
  /*! @ec the below function is not needed, as there is no need to override the power control to update path delay */
  //rfgsm_core_set_slot_override_flag(TRUE);

  /*! @todo Check return value from rfm interface */
  rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;
  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_PATH_DELAY_RSP );
  msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_set_path_delay_type_rsp_s) );
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_SET_PATH_DELAY_RSP, UMID=0x%x, msgr_send_status=%d",
                  rsp_msg.hdr.id, msgr_send_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_SET_PATH_DELAY_RSP, UMID=0x%x",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }
  return(cmd_rsp);
}

/*===========================================================================


===========================================================================*/
/*!
  @brief
  This function is called by rf_gsm_ftm_internal_cmd_dispatch, and calls the respective RF
  function and then sends a RESP MSG back and echos the arguments of the 
  original command.

  @return
  rfgsm_dispatch_outcome_t.
*/
/*=========================================================================*/
rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_set_linear_rgi( rfa_rf_gsm_ftm_set_linear_rgi_type_cmd_s* rfgsm_ftm_msg_ptr )
{
  /* Initialise response and result variables */
  rfa_rf_gsm_ftm_set_linear_rgi_type_rsp_s rsp_msg;
  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  errno_enum_type msgr_send_status = E_SUCCESS;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  if( rfgsm_ftm_msg_ptr == NULL )
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer argument is NULL in rfgsm_ftm_set_linear_rgi" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfgsm_ftm_msg_ptr->rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }

  /* Echo CMD message parameters in RSP message. RSP message may include some updated parameter info */
  rsp_msg.hdr                           = rfgsm_ftm_msg_ptr->hdr;
  rsp_msg.rfm_dev                       = rfgsm_ftm_msg_ptr->rfm_dev;
  rsp_msg.linear_rgi_params             = rfgsm_ftm_msg_ptr->linear_rgi_params;
  rsp_msg.cmd_result                    = RFA_RF_GSM_FTM_EXECUTION_ERROR;

  //Update rfgsm_card
  rfgsm_core_handle_ptr->rgi_override_val = rfgsm_ftm_msg_ptr->linear_rgi_params.rgi;
  //clear the tx_pow_dbm for given slot if linear RGI is being set
  rfgsm_core_handle_ptr->ftm_slot_override[rfgsm_ftm_msg_ptr->linear_rgi_params.slot_num].tx_pow_dbm = 0xFFFF;
  rfgsm_core_handle_ptr->ftm_slot_override[rfgsm_ftm_msg_ptr->linear_rgi_params.slot_num].linear_rgi = rfgsm_ftm_msg_ptr->linear_rgi_params.rgi;
  rfgsm_core_handle_ptr->ftm_slot_override[rfgsm_ftm_msg_ptr->linear_rgi_params.slot_num].modulation = !rfgsm_ftm_msg_ptr->linear_rgi_params.mod_type_is_gsm; //this is reverse logic...mod_type: GMSK = 0; 8PSK = 1 ---> mod_type_is_gsm: 1 = GMSK; 0 = 8PSK
  rfgsm_core_handle_ptr->ftm_slot_override[rfgsm_ftm_msg_ptr->linear_rgi_params.slot_num].pa_scaling = rfgsm_ftm_msg_ptr->linear_rgi_params.pa_scaling;
  rfgsm_core_handle_ptr->ftm_slot_override[rfgsm_ftm_msg_ptr->linear_rgi_params.slot_num].env_gain  = rfgsm_ftm_msg_ptr->linear_rgi_params.env_gain;
  rfgsm_core_handle_ptr->ftm_slot_override[rfgsm_ftm_msg_ptr->linear_rgi_params.slot_num].smps_bias =  rfgsm_ftm_msg_ptr->linear_rgi_params.smps_bias;
  //Update slot override flag

  MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfgsm_ftm_set_linear_rgi, rgi:%d, pa_scaling :%d, env_gain:%d ", rfgsm_ftm_msg_ptr->linear_rgi_params.rgi, rfgsm_ftm_msg_ptr->linear_rgi_params.pa_scaling, rfgsm_ftm_msg_ptr->linear_rgi_params.env_gain);

  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfgsm_ftm_set_linear_rgi, slot_num:%d, bias:%d ", rfgsm_ftm_msg_ptr->linear_rgi_params.slot_num, rfgsm_ftm_msg_ptr->linear_rgi_params.smps_bias);
  /*! @todo create wrappers for the below functions in rfm & rfgsm_mc layers as part of clean-up */
  rfgsm_core_set_tx_profile_update_flag(rfgsm_core_handle_ptr);
  rfgsm_core_set_slot_override_flag(rfgsm_core_handle_ptr, TRUE);
  rfgsm_core_set_bias_override_flag(rfgsm_ftm_msg_ptr->rfm_dev, 1);
  

  cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;

  /*! @todo Check return value from rfm interface */
  rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;
  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_LINEAR_RGI_RSP );
  msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_set_linear_rgi_type_rsp_s) );
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_SET_LINEAR_RGI_RSP, UMID=0x%x, msgr_send_status=%d",
                  rsp_msg.hdr.id, msgr_send_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_SET_LINEAR_RGI_RSP, UMID=0x%x",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }
  return(cmd_rsp);
}
/*===========================================================================

  FUNCTION:  rfgsm_ftm_set_pa_range

===========================================================================*/
/*!
  @brief
  This function is called by rf_gsm_ftm_internal_cmd_dispatch, and calls the respective RF
  function and then sends a RESP MSG back and echos the arguments of the 
  original command.

  @return
  rfgsm_dispatch_outcome_t.
*/
/*=========================================================================*/
rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_set_pa_range( rfa_rf_gsm_ftm_set_pa_range_type_cmd_s* rfgsm_ftm_msg_ptr )
{
  /* Initialise response and result variables */
  rfa_rf_gsm_ftm_set_pa_range_type_rsp_s rsp_msg;
  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  errno_enum_type msgr_send_status = E_SUCCESS;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  if( rfgsm_ftm_msg_ptr == NULL )
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer argument is NULL in rfgsm_ftm_set_pa_range" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfgsm_ftm_msg_ptr->rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR; 
    return cmd_rsp;
  }

  /* Echo CMD message parameters in RSP message. RSP message may include some updated parameter info */
  rsp_msg.hdr                      = rfgsm_ftm_msg_ptr->hdr;
  rsp_msg.rfm_dev                  = rfgsm_ftm_msg_ptr->rfm_dev;
  rsp_msg.pa_range_params          = rfgsm_ftm_msg_ptr->pa_range_params;
  rsp_msg.cmd_result               = RFA_RF_GSM_FTM_EXECUTION_ERROR;

  /*! @todo If required, verify payload parameters. */
  cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  rfgsm_core_handle_ptr->ftm_slot_override[rsp_msg.pa_range_params.num_slot].tx_pow_dbm = 0xFFFF;
  rfgsm_core_handle_ptr->ftm_slot_override[rsp_msg.pa_range_params.num_slot].pa_range = rsp_msg.pa_range_params.pa_range;
  rfgsm_core_handle_ptr->ftm_slot_override[rsp_msg.pa_range_params.num_slot].modulation  = rsp_msg.pa_range_params.mod_type;

  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfgsm_ftm_set_pa_range, PA range:%d, Modulation (GMSK-0, 8PSK-1) :%d ", rsp_msg.pa_range_params.pa_range, rsp_msg.pa_range_params.mod_type);

  /*! @todo create wrappers for the below functions in rfm & rfgsm_mc layers as part of clean-up */                                                                                                                
  rfgsm_core_set_tx_profile_update_flag(rfgsm_core_handle_ptr);
  rfgsm_core_set_slot_override_flag(rfgsm_core_handle_ptr, TRUE);

  /*! @todo Check return value from rfm interface */
  rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;

  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_RSP );
  msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_set_pa_range_type_rsp_s) );
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_RSP, UMID=0x%x, msgr_send_status=%d",
                  rsp_msg.hdr.id, msgr_send_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_RSP, UMID=0x%x",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }
  return(cmd_rsp);
}
/*===========================================================================


===========================================================================*/
/*!
  @brief
  This function is called by rf_gsm_ftm_internal_cmd_dispatch, and calls the respective RF
  function and then sends a RESP MSG back and echos the arguments of the 
  original command.
 
  Used when setting up the FTM frame matrix and in other situations where flag needs to be cleared
  or otherwise decoupled from 

  @return
  rfgsm_dispatch_outcome_t.
*/
/*=========================================================================*/
rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_set_slot_override_flag( rfa_rf_gsm_ftm_set_slot_override_flag_type_cmd_s* rfgsm_ftm_msg_ptr )
{
  /* Initialise response and result variables */
  rfa_rf_gsm_ftm_set_slot_override_flag_type_rsp_s rsp_msg;
  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  errno_enum_type msgr_send_status = E_SUCCESS;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  if( rfgsm_ftm_msg_ptr == NULL )
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer argument is NULL in rfgsm_ftm_set_slot_override_flag" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfgsm_ftm_msg_ptr->rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }

  /* Echo CMD message parameters in RSP message. RSP message may include some updated parameter info */
  rsp_msg.hdr                           = rfgsm_ftm_msg_ptr->hdr;
  rsp_msg.rfm_dev                       = rfgsm_ftm_msg_ptr->rfm_dev;
  rsp_msg.cmd_result                    = RFA_RF_GSM_FTM_EXECUTION_ERROR;

  //Update rfgsm_card
  /*! @todo create wrappers for the below function in rfm & rfgsm_mc layers as part of clean-up */                                                                                                                
  rfgsm_core_set_slot_override_flag( rfgsm_core_handle_ptr,
                                     rfgsm_ftm_msg_ptr->override_flag );

  //Slot override flag has been cleared, so reset the following variables.
  if(rfgsm_ftm_msg_ptr->override_flag == FALSE)
  {
      rfgsm_core_handle_ptr->ftm_path_delay_override_val = 0xFFFF;
      rfgsm_core_handle_ptr->parange_override_val        = 0xFF;
      rfgsm_core_handle_ptr->rgi_override_val            = 0xFFFF;
  }

  cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;

  /*! @todo Check return value from rfm interface */
  rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;
  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_SLOT_OVERRIDE_FLAG_RSP );
  msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_set_slot_override_flag_type_rsp_s) );
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_SET_SLOT_OVERRIDE_FLAG_RSP, UMID=0x%x, msgr_send_status=%d",
                  rsp_msg.hdr.id, msgr_send_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_SET_SLOT_OVERRIDE_FLAG_RSP, UMID=0x%x",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }
  return(cmd_rsp);
}
/*===========================================================================


===========================================================================*/
/*!
  @brief
  This function is called by rf_gsm_ftm_internal_cmd_dispatch, and calls the respective RF
  function and then sends a RESP MSG back and echos the arguments of the 
  original command.
 
  Used when setting up the FTM frame matrix and in other situations where flag needs to be cleared
  or otherwise decoupled from 

  @return
  rfgsm_dispatch_outcome_t.
*/
/*=========================================================================*/
rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_iq_capture_setup( rfa_rf_gsm_ftm_capture_iq_type_cmd_s* rfgsm_ftm_msg_ptr )
{
  /* Initialise response and result variables */
  rfa_rf_gsm_ftm_capture_iq_type_rsp_s rsp_msg;
  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  errno_enum_type msgr_send_status = E_SUCCESS;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  if( rfgsm_ftm_msg_ptr == NULL )
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer argument is NULL in rfgsm_ftm_iq_capture_setup" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfgsm_ftm_msg_ptr->rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }

  /* Echo CMD message parameters in RSP message. RSP message may include some updated parameter info */
  rsp_msg.hdr                           = rfgsm_ftm_msg_ptr->hdr;
  rsp_msg.rfm_dev                       = rfgsm_ftm_msg_ptr->rfm_dev;
  rsp_msg.cmd_result                    = RFA_RF_GSM_FTM_EXECUTION_ERROR;

  if (rfgsm_core_handle_ptr->iq_capture == NULL) 
  {
      ftm_gsm_init_iqcapture_buffer(rfgsm_ftm_msg_ptr->rfm_dev);
  }

  rsp_msg.iq_capture                    = rfgsm_core_handle_ptr->iq_capture;

  if (rfgsm_ftm_msg_ptr->iq_capture_type == FTM_IQ_CAPTURE_STOP) 
  {
      rfm_set_active_iq_device( RFM_INVALID_DEVICE );
  }else
  {
      rfm_set_active_iq_device( rfgsm_ftm_msg_ptr->rfm_dev );
  }

  /* If the IQ capture buffer is NULL, something is wrong */
  if( rfgsm_core_handle_ptr->iq_capture == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "IQ capture buffer for device %d is NULL", rfgsm_ftm_msg_ptr->rfm_dev );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }

  rfgsm_core_handle_ptr->iq_capture->capture_type = rfgsm_ftm_msg_ptr->iq_capture_type;

  /* if IQ capture is stopped, the number of samples copied and the value of those samples may be
     queried, hence not reset on FTM_IQ_CAPTURE_STOP */
  if(rfgsm_ftm_msg_ptr->iq_capture_type != FTM_IQ_CAPTURE_STOP)
  {
    /* set number of samples copied into the GSM local IQ buffer to zero 
      and clear the buffer */
    rfgsm_core_handle_ptr->iq_capture->total_iq_samples_copied = 0; 

    memset( rfgsm_core_handle_ptr->iq_capture->rfgsm_iq_buffer, 
            0, 
            RFGSM_MAX_IQ_BUFFER_LEN*sizeof(int16)); 
  }

  cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;

  /*! @todo Check return value from rfm interface */
  rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;
  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_CAPTURE_IQ_RSP );
  msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_capture_iq_type_rsp_s) );

  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_CAPTURE_IQ_RSP, UMID=0x%x, msgr_send_status=%d",
                  rsp_msg.hdr.id, msgr_send_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_CAPTURE_IQ_RSP, UMID=0x%x",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }

  return(cmd_rsp);
}
/*===========================================================================


===========================================================================*/

/*===========================================================================

  FUNCTION:  rf_gsm_ftm_internal_cmd_dispatcher

===========================================================================*/
/*!
  @brief
  This function is the rf gsm ftm dispatcher for the RFGSM FTM CMDs 
  the RF task receives. The dispatcher calls the function that sends a command 
  to the respective RF function. Upon completion a response is sent to the
  FTM task.

  @return
  rfgsm_dispatch_outcome_t
*/
/*=========================================================================*/
rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_internal_cmd_dispatcher(rfgsm_ftm_msg_u* msg_ptr)
{
  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  msgr_hdr_s   *msgr_hdr_ptr = (msgr_hdr_s*) msg_ptr; 

  if (msg_ptr != NULL)
  {
    switch (msgr_hdr_ptr->id)
    {
      case RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_CMD:
        MSG_1( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_CMD, UMID=0x%x", msgr_hdr_ptr->id );
        cmd_rsp = rfgsm_ftm_set_pa_range( (rfa_rf_gsm_ftm_set_pa_range_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_PATH_DELAY_CMD:
        MSG_1( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_PATH_DELAY_CMD, UMID=0x%x", msgr_hdr_ptr->id );
        cmd_rsp = rfgsm_ftm_set_path_delay( (rfa_rf_gsm_ftm_set_path_delay_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_LINEAR_RGI_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_LINEAR_RGI_CMD" );
        cmd_rsp = rfgsm_ftm_set_linear_rgi( (rfa_rf_gsm_ftm_set_linear_rgi_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_TX_POW_DBM_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_TX_POW_DBM_CMD" );
        cmd_rsp = rfgsm_ftm_set_tx_pow_dbm( (rfa_rf_gsm_ftm_set_tx_pow_dbm_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_SLOT_OVERRIDE_FLAG_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_SLOT_OVERRIDE_FLAG_CMD" );
        cmd_rsp = rfgsm_ftm_set_slot_override_flag( (rfa_rf_gsm_ftm_set_slot_override_flag_type_cmd_s*) msg_ptr);
        break;


      case RFA_RF_GSM_FTM_CAPTURE_IQ_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_CAPTURE_IQ_CMD" );
        cmd_rsp = rfgsm_ftm_iq_capture_setup( (rfa_rf_gsm_ftm_capture_iq_type_cmd_s*) msg_ptr);
        break;

/*! @todo The remainder of these commands are placeholders for possible future use */
      case RFA_RF_GSM_FTM_SET_MODE_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_MODE_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_TX_ON_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_TX_ON_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_TX_OFF_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_TX_OFF_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_LNA_RANGE_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_LNA_RANGE_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_PDM_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_PDM_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_BAND_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_BAND_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_TRANSMIT_CONT_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_TRANSMIT_CONT_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_TRANSMIT_BURST_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_TRANSMIT_BURST_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_RX_BURST_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_RX_BURST_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_GET_RSSI_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_GET_RSSI_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_PA_START_DELTA_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_PA_START_DELTA_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_PA_STOP_DELTA_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_PA_STOP_DELTA_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_PA_DAC_INPUT_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_PA_DAC_INPUT_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_RX_CONTINUOUS_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_RX_CONTINUOUS_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_RX_GAIN_RANGE_CAL_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_RX_GAIN_RANGE_CAL_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_TX_KV_CAL_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_TX_KV_CAL_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_TX_KV_CAL_V2_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_TX_KV_CAL_V2_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_RX_TIMING_CMD: /* Legacy ftm_gsm_extended dispatch commands */
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_RX_TIMING_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_TX_TIMING_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_TX_TIMING_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_TX_GAIN_SWEEP_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_TX_GAIN_SWEEP_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_TX_ENVDC_CS_SWEEP_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_TX_ENVDC_CS_SWEEP_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_CFG2_AMAM_SWEEP_V2_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_CFG2_AMAM_SWEEP_V2_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_RX_MULTISLOT_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_RX_MULTISLOT_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_SET_RX_BURST_FOR_EXPECTED_PWR_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_RX_BURST_FOR_EXPECTED_PWR_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_GET_MEAS_PWR_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_GET_MEAS_PWR_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;


      case RFA_RF_GSM_FTM_TUNE_TX_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_TUNE_TX_CMD" );
        cmd_rsp = rfgsm_ftm_set_mode_cmd( (rfa_rf_gsm_ftm_set_mode_type_cmd_s*) msg_ptr);
        break;

       case RFA_RF_GSM_FTM_TUNER_TUNE_CODE_OVERRIDE_CMD:
        MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_TUNER_TUNE_CODE_OVERRIDE_CMD" );
        cmd_rsp = rfgsm_ftm_tuner_override( (rfa_rf_gsm_ftm_tuner_override_type_cmd_s*) msg_ptr);
        break;

       case RFA_RF_GSM_FTM_SET_IP2_CAL_PARAMS_CMD:

       MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_SET_IP2_CAL_PARAMS_CMD" );

       cmd_rsp = rfgsm_ftm_set_ip2_cal_params( (rfa_rf_gsm_ftm_ip2_cal_type_cmd_s*) msg_ptr);
       break;

       case RFA_RF_GSM_FTM_GET_IP2_CAL_PARAMS_CMD:

         MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_GET_IP2_CAL_PARAMS_CMD" );

         cmd_rsp = rfgsm_ftm_get_ip2_cal_params ((rfa_rf_gsm_ftm_get_ip2_cal_info_cmd_s*) msg_ptr);

         break;

       case RFA_RF_GSM_FTM_GET_IP2_CAL_RESULTS_CMD:

         MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RFA RF GSM FTM: Message received ID: RFA_RF_GSM_FTM_GET_IP2_CAL_RESULTS_CMD" );

         cmd_rsp = rfgsm_ftm_get_ip2_cal_results ((rfa_rf_gsm_ftm_get_ip2_cal_results_cmd_s*) msg_ptr);

         break;

      case RFA_RF_GSM_FTM_SET_SAWLESS_LIN_MODE_CMD:
        cmd_rsp = rfgsm_ftm_set_sawless_lin_mode((rfa_rf_gsm_ftm_set_sawless_lin_mode_cmd_s*) msg_ptr);
        break;

      case RFA_RF_GSM_FTM_GET_SAWLESS_LIN_MODE_CMD:
        cmd_rsp = rfgsm_ftm_get_sawless_lin_mode((rfa_rf_gsm_ftm_get_sawless_lin_mode_cmd_s*) msg_ptr);
        break;

      default:
        MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Undefined RF GSM FTM CMD" );
        cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
        break;
    }
  }
  else
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer NULL" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
  }
  return(cmd_rsp);
}

/*===========================================================================

  FUNCTION:  rfgsm_ftm_cmd_dispatcher

===========================================================================*/

/*!
  @brief
  This is the common command handler for RF GSM FTM control commands.

  @param module_id
  Module ID identifies the "technology" i.e LTE, UMTS, etc. 
  in which the given command belongs.

  @param cmd
  Command ID is a unique enumeration value that identifies this interface.

  @param cmd_payload_size
  The size of the structure specific to this interface.

  @param payload
  A pointer to a structure containing the parameters specific to this interface.

  @return
  void
*/

/*=========================================================================*/

void rfgsm_ftm_cmd_dispatcher( rf_cmd_type* req_ptr )
{
  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  rfgsm_ftm_msg_u rfgsm_msgr_req;
  uint8 * cpy_dest;
  void * cpy_src;
  size_t cpy_size;

  if(req_ptr != NULL)
  {
    /*Limit memcpy message copy size to max allowable size*/
    cpy_size = MIN(sizeof(rfgsm_ftm_msg_u)-sizeof(rf_msgr_hdr_struct_type), RF_MAX_CMD_PAYLOAD_SIZE);

    cpy_src = (void *) &req_ptr->msgr_hdr;
    cpy_dest = (void *) &rfgsm_msgr_req;

    /*Copy the MSGR request packet header */
    memscpy( cpy_dest, sizeof(rfgsm_ftm_msg_u), cpy_src, sizeof(rf_msgr_hdr_struct_type));

    /* Set the destination and source pointers to the payload */
    cpy_src = (void *)&req_ptr->payload;
    cpy_dest += sizeof(rf_msgr_hdr_struct_type);

    /* Copy the payload */
    memscpy( (void *) cpy_dest, (sizeof(rfgsm_ftm_msg_u)-sizeof(rf_msgr_hdr_struct_type)), cpy_src, cpy_size );  

    if(((rfa_rf_gsm_ftm_set_pa_range_type_cmd_s *)&rfgsm_msgr_req)->rfm_dev == RFM_DEVICE_0)
    {
    cmd_rsp = rfgsm_ftm_internal_cmd_dispatcher(&rfgsm_msgr_req);

    if (cmd_rsp != RFGSM_FTM_DISPATCH_SUCCESSFUL)
    {
      MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR,
           "GSM command received from rf command dispatcher. GSM FTM Command"
           "processor could not process the command 0x%x. Exited with error code"
           "%d",req_ptr->msgr_hdr.id,cmd_rsp );

      cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
    }
  }
}
}


/*===========================================================================

  FUNCTION:  rfgsm_ftm_cmd_dispatcher

===========================================================================*/

/*!
  @brief
  This is the common command handler for RF GSM FTM control commands.

  @param module_id
  Module ID identifies the "technology" i.e LTE, UMTS, etc. 
  in which the given command belongs.

  @param cmd
  Command ID is a unique enumeration value that identifies this interface.

  @param cmd_payload_size
  The size of the structure specific to this interface.

  @param payload
  A pointer to a structure containing the parameters specific to this interface.

  @return
  void
*/

/*=========================================================================*/

void rfgsm_ftm_cmd_task_2_dispatcher( rf_cmd_type* req_ptr )
{
  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  rfgsm_ftm_msg_u rfgsm_msgr_req;
  uint8 * cpy_dest;
  void * cpy_src;
  size_t cpy_size;

  if( req_ptr != NULL )
  {
    /*Limit memcpy message copy size to max allowable size*/
    cpy_size = MIN(sizeof(rfgsm_ftm_msg_u)-sizeof(rf_msgr_hdr_struct_type), RF_MAX_CMD_PAYLOAD_SIZE);

    cpy_src = (void *) &req_ptr->msgr_hdr;
    cpy_dest = (void *) &rfgsm_msgr_req;

    /*Copy the MSGR request packet header */
    memscpy( (void *)cpy_dest, sizeof(rfgsm_ftm_msg_u), cpy_src, sizeof(rf_msgr_hdr_struct_type));

    /* Set the destination and source pointers to the payload */
    cpy_src = (void *)&req_ptr->payload;
    cpy_dest += sizeof(rf_msgr_hdr_struct_type);

    /* Copy the payload */
    memscpy((uint8 *)cpy_dest, (sizeof(rfgsm_ftm_msg_u)-sizeof(rf_msgr_hdr_struct_type)), cpy_src, cpy_size );

    if(((rfa_rf_gsm_ftm_set_pa_range_type_cmd_s *)&rfgsm_msgr_req)->rfm_dev == RFM_DEVICE_2)
    {
    cmd_rsp = rfgsm_ftm_internal_cmd_dispatcher(&rfgsm_msgr_req);

    if (cmd_rsp != RFGSM_FTM_DISPATCH_SUCCESSFUL)
    {
      MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR,
           "RF TASK 2 - GSM command received from rf command dispatcher. GSM FTM Command"
           "processor could not process the command 0x%x. Exited with error code"
           "%d",req_ptr->msgr_hdr.id, cmd_rsp );

      cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
    }
  }
}
}


/*===========================================================================

  FUNCTION:  rfgsm_ftm_msgr_register

===========================================================================*/
/*!
  @brief
  This function is used by RF Task to register for commands sent by the
  RF GSM FTM Dispatcher in the FTM task via the message router.


  @return
  TRUE if all registration were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rfgsm_ftm_msgr_register( msgr_client_t *client_id, msgr_id_t id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Register for the RF GSM FTM Commands that the RF task is interested in receiving */
  ret_status = msgr_register_block(MSGR_RFA_RF_GSM_FTM,
                                   client_id,
                                   id,
                                   rfgsm_ftm_umid_cmd_list,
                                   RF_GSM_FTM_UMID_CNT);
  if (ret_status != E_SUCCESS)
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "rfgsm_ftm_msgr_register: Unable to register GSM FTM message router register block." );
    rc = FALSE;
  }
  return(rc);
}


/*===========================================================================

  FUNCTION:  rfgsm_ftm_msgr_deregister

===========================================================================*/
/*!
  @brief
  This function is used by RF Task to de-register the messages registered earlier 
  for handling the messages sent by the RF GSM FTM processor to the message router


  @return
  TRUE if all de-registration were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rfgsm_ftm_msgr_deregister( msgr_client_t *client_id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Deregister all the RF GSM FTM message FTM task has registerd for. */
  ret_status = msgr_deregister_block(MSGR_RFA_RF_GSM_FTM,
                                     client_id,
                                     rfgsm_ftm_umid_cmd_list,
                                     RF_GSM_FTM_UMID_CNT);
  if (ret_status != E_SUCCESS)
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "rfgsm_ftm_msgr_deregister: Unable to deregister FTM message router register block." );
    rc = FALSE;
  }
  return(rc);
}



/*===========================================================================

  FUNCTION:  rfgsm_ftm_dispatch_init

===========================================================================*/
/*!
  @brief

  @details This funtion has to be called in the rfgsm_mc_init before an RF GSM FTM command is
  sent to RF Driver task. Registration with rf module dispatch table and  rf command dispatch table
  for the RF GSM FTM module.
*/
/*=========================================================================*/
void rfgsm_ftm_dispatch_init(void)
{
  /*Register with rf module dispatch table using RF RFA as the module.*/
  if(rf_cmd_dispatch_register_tech( RF_TASK_1, 
                                    MSGR_TECH_RFA, 
                                    MSGR_MODULE_RF_GSM_FTM, 
                                    rfgsm_ftm_cmd_dispatcher) != RF_CMD_REGISTRATION_SUCCESS)
  {
     MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "rfgsm_ftm_cmd_dispatcher RF TASK GSM:Failed MSGR_TECH_RFA / MSGR_MODULE_RF_GSM_FTM "
               "registration" );
     return;
  }
}


/*===========================================================================

  FUNCTION:  rfgsm_ftm_dispatch_init_2

===========================================================================*/
/*!
  @brief

  @details This funtion has to be called in the rfgsm_mc_init before an RF GSM FTM command is
  sent to RF Driver task. Registration with rf module dispatch table and  rf command dispatch table
  for the RF GSM FTM module.
*/
/*=========================================================================*/
void rfgsm_ftm_dispatch_task_2_init(void)
{
  /*Register with rf module dispatch table using RF RFA as the module.*/
  if(rf_cmd_dispatch_register_tech( RF_TASK_2, 
                                    MSGR_TECH_RFA, 
                                    MSGR_MODULE_RF_GSM_FTM, 
                                    rfgsm_ftm_cmd_task_2_dispatcher) != RF_CMD_REGISTRATION_SUCCESS)
  {
     MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "rfgsm_ftm_cmd_dispatcher RF_TASK_2:Failed MSGR_TECH_RFA / MSGR_MODULE_RF_GSM_FTM "
               "registration" );
     return;
  }
}
/*===========================================================================

  FUNCTION:  rfgsm_ftm_dispatch_deinit 

===========================================================================*/
/*!
  @brief
  This function has to be called in the whenever the RF task is killed. Frees allocated
  memory and registration tables.

  @return
  void
*/
/*=========================================================================*/

void rfgsm_ftm_dispatch_deinit(void)
{
  /* Clean up the rf command dispatch table which contains all the pointers to individual command handler. 
    The rf module dispatch table is cleaned by rf_cmd_dispatcher when rf task is killed.*/
  rf_cmd_dispatch_deregister_tech( RF_TASK_1, MSGR_TECH_RFA, MSGR_MODULE_RF_GSM_FTM);
}

/*===========================================================================

  FUNCTION:  rfgsm_ftm_dispatch_deinit_2

===========================================================================*/
/*!
  @brief
  This function has to be called in the whenever the RF task is killed. Frees allocated
  memory and registration tables.

  @return
  void
*/
/*=========================================================================*/

void rfgsm_ftm_dispatch_deinit_2(void)
{
  /* Clean up the rf command dispatch table which contains all the pointers to individual command handler. 
    The rf module dispatch table is cleaned by rf_cmd_dispatcher when rf task is killed.*/
  rf_cmd_dispatch_deregister_tech( RF_TASK_2, MSGR_TECH_RFA, MSGR_MODULE_RF_GSM_FTM);
}


rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_tuner_override ( rfa_rf_gsm_ftm_tuner_override_type_cmd_s* rfgsm_ftm_msg_ptr)
{
  /* Initialise response and result variables */
  rfa_rf_gsm_ftm_tuner_override_type_rsp_s rsp_msg;
  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  errno_enum_type msgr_send_status = E_SUCCESS;
  uint32 error_status = 0xFFFFFFFF;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  if( rfgsm_ftm_msg_ptr == NULL )
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer argument is NULL in rfgsm_ftm_tuner_override" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfgsm_ftm_msg_ptr->rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!" );
	return cmd_rsp;
  }
  

  /* Echo CMD message parameters in RSP message. RSP message may include some updated parameter info */
  rsp_msg.hdr                      = rfgsm_ftm_msg_ptr->hdr;
  rsp_msg.rfm_dev                  = rfgsm_ftm_msg_ptr->rfm_dev;
  rsp_msg.tuner_params             = rfgsm_ftm_msg_ptr->tuner_params;
  rsp_msg.cmd_result               = RFA_RF_GSM_FTM_EXECUTION_ERROR;

  /*! @todo If required, verify payload parameters. */
  cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  

  //execution goes here
  error_status = rfgsm_core_set_tuner_tune_code_override( rsp_msg.rfm_dev, 
  							  rsp_msg.tuner_params.override_flag, 
  						          rsp_msg.tuner_params.data_ptr, 
  						          rsp_msg.tuner_params.tuner_nv_ptr,
  							  rsp_msg.tuner_params.tuner_id_ptr);

  rsp_msg.tuner_params.error_status = error_status;

  /*! @todo Check return value from rfm interface */
  rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;

  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_TUNER_TUNE_CODE_OVERRIDE_RSP );
  msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_tuner_override_type_rsp_s) );
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_TUNER_TUNE_CODE_OVERRIDE_RSP, UMID=0x%x, msgr_send_status=%d",
                  rsp_msg.hdr.id, msgr_send_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_TUNER_TUNE_CODE_OVERRIDE_RSP, UMID=0x%x",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }
  return(cmd_rsp);
}




/* Receive params from FTM and send response*/
rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_set_ip2_cal_params ( rfa_rf_gsm_ftm_ip2_cal_type_cmd_s* rfgsm_ftm_msg_ptr)
{

    /* Initialise response and result variables */
  rfa_rf_gsm_ftm_ip2_cal_type_rsp_s rsp_msg;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  errno_enum_type msgr_send_status = E_SUCCESS;

  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;


  if( rfgsm_ftm_msg_ptr == NULL )
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer argument is NULL in rfgsm_ftm_tuner_override" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
  }
  else
  {
      /* Echo CMD message parameters in RSP message. RSP message may include some updated parameter info */
      rsp_msg.hdr                      = rfgsm_ftm_msg_ptr->hdr;
      rsp_msg.rfm_dev                  = rfgsm_ftm_msg_ptr->rfm_dev;
      rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfgsm_ftm_msg_ptr->rfm_dev);
  }

  rsp_msg.cmd_result = RFA_RF_GSM_FTM_EXECUTION_ERROR;

  
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!" );
     
  }
  else
  {
      /* Consider Critical section lock ??*/
  
      /* Update params in rfgsm core handles*/

      rfgsm_core_handle_ptr->ip2_cal_params.alg_params.override_params = !rfgsm_ftm_msg_ptr->ip2_cal_params.use_default_params;

      if (! (rfgsm_ftm_msg_ptr->ip2_cal_params.use_default_params))
      {
          rfgsm_core_handle_ptr->ip2_cal_params.rfcom_band = (rfcom_gsm_band_type)rfgsm_ftm_msg_ptr->ip2_cal_params.rfcom_band;
          rfgsm_core_handle_ptr->ip2_cal_params.arfcn     = rfgsm_ftm_msg_ptr->ip2_cal_params.arfcn;
          rfgsm_core_handle_ptr->ip2_cal_params.lna_range = rfgsm_ftm_msg_ptr->ip2_cal_params.lna_range;
          rfgsm_core_handle_ptr->ip2_cal_params.alg_params.num_cal_points = ((2*rfgsm_ftm_msg_ptr->ip2_cal_params.width)+1)*((2*rfgsm_ftm_msg_ptr->ip2_cal_params.width)+1);
          rfgsm_core_handle_ptr->ip2_cal_params.alg_params.start_i_code = rfgsm_ftm_msg_ptr->ip2_cal_params.start_i_code;
          rfgsm_core_handle_ptr->ip2_cal_params.alg_params.start_q_code = rfgsm_ftm_msg_ptr->ip2_cal_params.start_q_code;
          rfgsm_core_handle_ptr->ip2_cal_params.alg_params.unit_step_size = rfgsm_ftm_msg_ptr->ip2_cal_params.unit_step_size;
          rfgsm_core_handle_ptr->ip2_cal_params.alg_params.width  = rfgsm_ftm_msg_ptr->ip2_cal_params.width;
          rfgsm_core_handle_ptr->ip2_cal_params.tx_rgi = rfgsm_ftm_msg_ptr->ip2_cal_params.tx_rgi;
          rfgsm_core_handle_ptr->ip2_cal_params.step_duration_qs = rfgsm_ftm_msg_ptr->ip2_cal_params.step_duration_qs;
          rfgsm_core_handle_ptr->ip2_cal_params.dc_settle_time_qs = rfgsm_ftm_msg_ptr->ip2_cal_params.dc_settling_time_qs;

          /*Since this API is called to set up the IP2 cal params, first IP2 cal iteration flag needs to be set*/
          rfgsm_core_handle_ptr->ip2_cal_first_iter = TRUE;
          rfgsm_core_handle_ptr->ip2_cal_params.ip2_cal_supported = rfgsm_core_get_sawless_status(rfgsm_ftm_msg_ptr->rfm_dev);

            /*Set result in response msg to success*/
          rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;

//          MSG_8( MSG_SSID_FTM, MSG_LEGACY_HIGH, "rfgsm_ftm_set_ip2_cal_params, i code: %d, q code:%d, unit_step_size:%d, width:%d, arfcn:%d, band:%d, TX rgi:%d, lna_range:%d",
//                 rfgsm_core_handle_ptr->ip2_cal_params.alg_params.start_i_code,
//                 rfgsm_core_handle_ptr->ip2_cal_params.alg_params.start_q_code,
//                 rfgsm_core_handle_ptr->ip2_cal_params.alg_params.unit_step_size,
//                 rfgsm_core_handle_ptr->ip2_cal_params.alg_params.width,
//                 rfgsm_core_handle_ptr->ip2_cal_params.arfcn,
//                 rfgsm_core_handle_ptr->ip2_cal_params.rfcom_band,
//                 rfgsm_core_handle_ptr->ip2_cal_params.tx_rgi,
//                 rfgsm_core_handle_ptr->ip2_cal_params.lna_range);
//
//          MSG_3( MSG_SSID_FTM, MSG_LEGACY_HIGH, "rfgsm_ftm_set_ip2_cal_params, step duration (qs): %d, dc settling time:%d, use default params:%d",
//                 rfgsm_ftm_msg_ptr->ip2_cal_params.step_duration_qs, rfgsm_ftm_msg_ptr->ip2_cal_params.dc_settling_time_qs, rfgsm_ftm_msg_ptr->ip2_cal_params.use_default_params  );


          MSG_1( MSG_SSID_FTM,MSG_LEGACY_HIGH, "rfgsm_ftm_set_ip2_cal_params, sub id:%d",rfgsm_ftm_msg_ptr->sub_id );

          rfgsm_core_handle_ptr->ip2_cal_params.dc_settle_time_qs+=IP2_CAL_DC_SETTLING_TIME_OFFSET;// 20 is the worst case time between trigger and ccs callback received

          if(!rfgsm_core_dm_set_ip2_cal_info(rfgsm_ftm_msg_ptr->sub_id, 
                                             rfgsm_core_handle_ptr->ip2_cal_params.step_duration_qs, 
                                             rfgsm_core_handle_ptr->ip2_cal_params.dc_settle_time_qs, 
                                             rfgsm_core_handle_ptr->ip2_cal_alpha,
                                             rfgsm_core_handle_ptr->ip2_cal_params.alg_params.num_cal_points))
          {
            MSG( MSG_SSID_FTM,MSG_LEGACY_ERROR, "rfgsm_ftm_set_ip2_cal_params, Failed to set IP2 cal info in DM" );
            return FALSE;
          }

      }
      else
      {

          MSG_1( MSG_SSID_FTM, MSG_LEGACY_HIGH, "rfgsm_ftm_set_ip2_cal_params, use default params: %d",rfgsm_core_handle_ptr->ip2_cal_params.alg_params.override_params );
      }

  }

  /*Send the response*/

  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_IP2_CAL_PARAMS_RSP );
  msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_ip2_cal_type_rsp_s) );
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_SET_IP2_CAL_PARAMS_RSP, UMID=0x%x, msgr_send_status=%d",
                  rsp_msg.hdr.id, msgr_send_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_SET_IP2_CAL_PARAMS_RSP, UMID=0x%x",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }

  return cmd_rsp;

}


rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_get_ip2_cal_params ( rfa_rf_gsm_ftm_get_ip2_cal_info_cmd_s* rfgsm_ftm_msg_ptr)
{

    /* Initialise response and result variables */
  rfa_rf_gsm_ftm_get_ip2_cal_info_rsp_s rsp_msg;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  errno_enum_type msgr_send_status = E_SUCCESS;

  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;

  if( rfgsm_ftm_msg_ptr == NULL )
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer argument is NULL in rfgsm_ftm_get_ip2_cal_params" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
  }
  else
  {
      /* Echo CMD message parameters in RSP message. RSP message may include some updated parameter info */
      rsp_msg.hdr                      = rfgsm_ftm_msg_ptr->hdr;
      rsp_msg.rfm_dev                  = rfgsm_ftm_msg_ptr->rfm_dev;
      rfgsm_core_handle_ptr            = rfgsm_core_handle_get(rfgsm_ftm_msg_ptr->rfm_dev);


  }

  rsp_msg.cmd_result = RFA_RF_GSM_FTM_EXECUTION_ERROR;

  
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!" );
     
  }
  else
  {

    /*load default values in core handles from the device driver*/
    if(rfgsm_core_load_default_ip2_cal_params(rfgsm_ftm_msg_ptr->rfm_dev,rfgsm_ftm_msg_ptr->rfcom_band ))
    {
      /* Prepare the response*/
      rsp_msg.width = rfgsm_core_handle_ptr->ip2_cal_params.alg_params.width;
      rsp_msg.num_frames = rfgsm_core_handle_ptr->ip2_cal_params.alg_params.num_iter; 
      rsp_msg.step_duration_qs = rfgsm_core_handle_ptr->ip2_cal_params.step_duration_qs;
      rsp_msg.dc_settling_time_qs = rfgsm_core_handle_ptr->ip2_cal_params.dc_settle_time_qs;

      /*Set the rfcom band*/
      rfgsm_core_handle_ptr->ip2_cal_params.rfcom_band = rfgsm_ftm_msg_ptr->rfcom_band;

      /* Since this API sets up the default IP2 cal params before cal, the first cal iteration flag needs to be set*/
      rfgsm_core_handle_ptr->ip2_cal_first_iter = TRUE;

       /*Set result in response msg to success*/
      rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;

       MSG_4( MSG_SSID_FTM, MSG_LEGACY_HIGH, "rfgsm_ftm_get_ip2_cal_params, width:%d, num_frames:%d, step_duration:%d, dc settling time:%d",
                   rsp_msg.width,
                   rsp_msg.num_frames,
                   rsp_msg.step_duration_qs,
                   rsp_msg.dc_settling_time_qs);


       rfgsm_core_handle_ptr->ip2_cal_params.dc_settle_time_qs+=IP2_CAL_DC_SETTLING_TIME_OFFSET; // 20 is the worst case time between trigger and ccs callback received

       if(!rfgsm_core_dm_set_ip2_cal_info(rfgsm_ftm_msg_ptr->sub_id, 
                                          rfgsm_core_handle_ptr->ip2_cal_params.step_duration_qs, 
                                          rfgsm_core_handle_ptr->ip2_cal_params.dc_settle_time_qs, 
                                          rfgsm_core_handle_ptr->ip2_cal_alpha,
                                          rfgsm_core_handle_ptr->ip2_cal_params.alg_params.num_cal_points))
       {
         MSG( MSG_SSID_FTM,MSG_LEGACY_ERROR, "rfgsm_ftm_get_ip2_cal_params, Failed to set IP2 cal info in DM" );
         return FALSE;
       }


    }
    else
    {

      MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "rfgsm_ftm_get_ip2_cal_params, load default params failed ");
    }
  }


  /*Send the response*/

  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_GET_IP2_CAL_PARAMS_RSP );
  msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_get_ip2_cal_info_rsp_s) );
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_GET_IP2_CAL_PARAMS_RSP, UMID=0x%x, msgr_send_status=%d",
                  rsp_msg.hdr.id, msgr_send_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_GET_IP2_CAL_PARAMS_RSP, UMID=0x%x",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }

  return cmd_rsp;

}




rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_get_ip2_cal_results( rfa_rf_gsm_ftm_get_ip2_cal_results_cmd_s* rfgsm_ftm_msg_ptr)
{

  rfgsm_ip2_cal_params_type ip2_cal_params;

  uint8 flag_mask=0;

  boolean cal_end = FALSE;

  /* Initialise response and result variables */
  rfa_rf_gsm_ftm_get_ip2_cal_results_rsp_s rsp_msg;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  errno_enum_type msgr_send_status = E_SUCCESS;

  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;


  if( rfgsm_ftm_msg_ptr == NULL )
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer argument is NULL in rfgsm_ftm_get_ip2_cal_results" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
  }
  else
  {
      /* Echo CMD message parameters in RSP message. RSP message may include some updated parameter info */
      rsp_msg.hdr                      = rfgsm_ftm_msg_ptr->hdr;
      rsp_msg.rfm_dev                  = rfgsm_ftm_msg_ptr->rfm_dev;
      flag_mask                        = rfgsm_ftm_msg_ptr->flag_mask;
      rfgsm_core_handle_ptr            = rfgsm_core_handle_get(rfgsm_ftm_msg_ptr->rfm_dev);
      cal_end                          = rfgsm_ftm_msg_ptr->ip2_cal_end;                     

  }

  rsp_msg.cmd_result = RFA_RF_GSM_FTM_EXECUTION_ERROR;

  
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!" );
     
  }
  else
  {
    /* Fetch the cal results based upon the flag mask*/


    if ( rfgsm_core_use_ip2_cal_results(rfgsm_ftm_msg_ptr->rfm_dev, 
                                        rfgsm_ftm_msg_ptr->rfcom_band, 
                                        rfgsm_ftm_msg_ptr->sub_id, 
                                        &ip2_cal_params, 
                                        flag_mask,
                                        cal_end) )
    {
      rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;
    }


     
    rsp_msg.final_i_code = ip2_cal_params.alg_params.start_i_code;
    rsp_msg.final_q_code = ip2_cal_params.alg_params.start_q_code;
    rsp_msg.min_im2_val = ip2_cal_params.alg_params.min_im2_val;
    rsp_msg.status = ip2_cal_params.status;

    MSG_5( MSG_SSID_FTM, MSG_LEGACY_HIGH, "rfgsm_ftm_get_ip2_cal_results, band passed:%d, final i code:%d, final q code:%d, min im2 value:%d, cal end:%d%",
             rfgsm_ftm_msg_ptr->rfcom_band,
             rsp_msg.final_i_code,
             rsp_msg.final_q_code,
             rsp_msg.min_im2_val,
             cal_end);

     MSG_1( MSG_SSID_FTM, MSG_LEGACY_HIGH, "rfgsm_ftm_get_ip2_cal_results, IP2 cal pass status:%d",rsp_msg.status);

  }


  /*Send the response*/

  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_GET_IP2_CAL_RESULTS_RSP );
  msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_get_ip2_cal_results_rsp_s) );
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_GET_IP2_CAL_RESULTS_RSP, UMID=0x%x, msgr_send_status=%d",
                  rsp_msg.hdr.id, msgr_send_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_GET_IP2_CAL_RESULTS_RSP, UMID=0x%x",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }

  return cmd_rsp;

}
/*=========================================================================*/
/*!
  @brief
  This function overrides the Rx Linearity mode of a SAWLess target.

  @details
 
*/

rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_set_sawless_lin_mode( rfa_rf_gsm_ftm_set_sawless_lin_mode_cmd_s* rfgsm_ftm_msg_ptr)
{
  /* Initialise response and result variables */
  rfa_rf_gsm_ftm_set_sawless_lin_mode_rsp_s rsp_msg;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  errno_enum_type msgr_send_status = E_SUCCESS;

  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;


  if( rfgsm_ftm_msg_ptr == NULL )
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer argument is NULL in rfgsm_ftm_set_sawless_lin_mode" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }
  else
  {
      /* Echo CMD message parameters in RSP message. RSP message may include some updated parameter info */
      rsp_msg.hdr                      = rfgsm_ftm_msg_ptr->hdr;
      rsp_msg.rfm_dev                  = rfgsm_ftm_msg_ptr->rfm_dev;
      rfgsm_core_handle_ptr            = rfgsm_core_handle_get(rfgsm_ftm_msg_ptr->rfm_dev);
  }

  rsp_msg.cmd_result = RFA_RF_GSM_FTM_EXECUTION_ERROR;

  
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!" );
     
  }
  else
  {
    if ( rfgsm_core_get_sawless_status(rfgsm_ftm_msg_ptr->rfm_dev) )
    {
      /* If the band is 0xFF, override globally to HL or LL */
      if ( rfgsm_ftm_msg_ptr->band == 0xFF )
      {
        /* Set the High Lin/Low Lin decision based on setting in cmd */
        if ( rfgsm_ftm_msg_ptr->use_high_lin_mode == TRUE )
        {
          rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_HL_OVERRIDE; 
        }
        else
        {
          rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_LL_OVERRIDE; 
        }
      }
      else /* Override the specific band and channel to HL or LL */
      {
        /* Set the High Lin/Low Lin decision based on setting in cmd */
        if ( rfgsm_ftm_msg_ptr->use_high_lin_mode == TRUE )
        {
          if ( rfgsm_core_set_arfcn_to_high_lin(rfgsm_ftm_msg_ptr->rfm_dev,
                                                rfgsm_ftm_msg_ptr->arfcn,
                                                rfgsm_ftm_msg_ptr->band ) == TRUE)
          {
            rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;

            /* If we are setting an override on a channel, we do not want any algorithm
            updates to happen, so set the override flag to indicate channel specific override */
            rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_CHAN_OVERRIDE;
          }
        }
        else
        {
          if ( rfgsm_core_set_arfcn_to_low_lin(rfgsm_ftm_msg_ptr->rfm_dev,
                                               rfgsm_ftm_msg_ptr->arfcn,
                                               rfgsm_ftm_msg_ptr->band ) == TRUE)
          {
            rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;

            /* If we are setting an override on a channel, we do not want any algorithm
            updates to happen, so set the override flag to channel specific override */
            rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override = RFGSM_SAWLESS_CHAN_OVERRIDE;
          }
        }
      }

      

      MSG_3( MSG_SSID_FTM, MSG_LEGACY_HIGH, "rfgsm_ftm_set_sawless_lin_mode, band: %d, ARFCN: %d, setting: %d",
             rfgsm_ftm_msg_ptr->band,
             rfgsm_ftm_msg_ptr->arfcn,
             rfgsm_ftm_msg_ptr->use_high_lin_mode);
    }
  
  }


  /*Send the response*/
  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_SET_SAWLESS_LIN_MODE_RSP );
  msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_set_sawless_lin_mode_rsp_s) );
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_SET_SAWLESS_LIN_MODE_RSP, UMID=0x%x, msgr_send_status=%d",
                  rsp_msg.hdr.id, msgr_send_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_SET_SAWLESS_LIN_MODE_RSP, UMID=0x%x",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }

  return cmd_rsp;

}
/*=========================================================================*/
/*!
  @brief
  This function reports the Rx Linearity mode of a SAWLess target.

  @details
 
*/
rfgsm_ftm_dispatch_outcome_t rfgsm_ftm_get_sawless_lin_mode( rfa_rf_gsm_ftm_get_sawless_lin_mode_cmd_s* rfgsm_ftm_msg_ptr)
{
  /* Initialise response and result variables */
  rfa_rf_gsm_ftm_get_sawless_lin_mode_rsp_s rsp_msg;

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  errno_enum_type msgr_send_status = E_SUCCESS;

  rfgsm_ftm_dispatch_outcome_t cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;


  if( rfgsm_ftm_msg_ptr == NULL )
  {
    MSG( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Pointer argument is NULL in rfgsm_ftm_get_sawless_lin_mode" );
    cmd_rsp = RFGSM_FTM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }
  else
  {
      /* Echo CMD message parameters in RSP message. RSP message may include some updated parameter info */
      rsp_msg.hdr                      = rfgsm_ftm_msg_ptr->hdr;
      rsp_msg.rfm_dev                  = rfgsm_ftm_msg_ptr->rfm_dev;
      rfgsm_core_handle_ptr            = rfgsm_core_handle_get(rfgsm_ftm_msg_ptr->rfm_dev);
      rsp_msg.band                     = rfgsm_ftm_msg_ptr->band;
      rsp_msg.arfcn                    = rfgsm_ftm_msg_ptr->arfcn;
      rsp_msg.is_in_high_lin           = FALSE;
  }

  rsp_msg.cmd_result = RFA_RF_GSM_FTM_EXECUTION_ERROR;

  
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!" );
     
  }
  else
  {
    if ( rfgsm_core_get_sawless_status(rfgsm_ftm_msg_ptr->rfm_dev) )
    {
      /* If the band is 0xFF, check global override */
      if ( rfgsm_ftm_msg_ptr->band == 0xFF )
      {
        /* Retrieve the High Lin/Low Lin decision based on setting in core handle */
        if ( rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override == RFGSM_SAWLESS_HL_OVERRIDE )
        {
          rsp_msg.is_in_high_lin = TRUE; 
        }
        else
        {
          rsp_msg.is_in_high_lin = FALSE; 
        }
      }
      else /* Check the specific band and channel */
      {
        if ( rfgsm_core_is_arfcn_in_high_lin( rfgsm_ftm_msg_ptr->rfm_dev,
                                              rfgsm_ftm_msg_ptr->arfcn, 
                                              rfgsm_ftm_msg_ptr->band,
                                              &(rsp_msg.is_in_high_lin)) == TRUE )
        {
          rsp_msg.cmd_result = RFA_RF_GSM_FTM_SUCCESS;
        }
      }
    }
  }

  MSG_3( MSG_SSID_FTM, MSG_LEGACY_HIGH, "rfgsm_ftm_get_sawless_lin_mode, band: %d, ARFCN: %d, setting: %d",
           rfgsm_ftm_msg_ptr->band,
           rfgsm_ftm_msg_ptr->arfcn,
           rsp_msg.is_in_high_lin );

  /*Send the response*/
  msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_GSM_FTM, RFA_RF_GSM_FTM_GET_SAWLESS_LIN_MODE_RSP );
  msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfa_rf_gsm_ftm_get_sawless_lin_mode_rsp_s) );
  if (msgr_send_status != E_SUCCESS)
  {
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_ERROR, "RF GSM FTM Command Processor cannot send RFA_RF_GSM_FTM_GET_SAWLESS_LIN_MODE_RSP, UMID=0x%x, msgr_send_status=%d",
                  rsp_msg.hdr.id, msgr_send_status );
    cmd_rsp = RFGSM_FTM_DISPATCH_UNSUCCESSFUL;
  }
  else
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_LOW, "RF GSM FTM Command Processor successfully sent RFA_RF_GSM_FTM_GET_SAWLESS_LIN_MODE_RSP, UMID=0x%x",
                rsp_msg.hdr.id );
    cmd_rsp = RFGSM_FTM_DISPATCH_SUCCESSFUL;
  }

  return cmd_rsp;

}
