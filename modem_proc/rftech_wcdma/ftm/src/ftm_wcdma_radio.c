/*!
  @file
  ftm_wcdma_radio.c

  @brief
  This module contains FTM WCDMA Radio Control code which is used to
  exclusively control Radio for test purposes.
*/

/*==============================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/ftm/src/ftm_wcdma_radio.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/19/15   rmb/ck  Fix for radio tune API to set the WCDMA as paramter to FTM Conc Manager.
01/06/15   sd      Add NULL check for pointer agc_rsp_type
11/13/14   vbh     Add checks for concurrency manager before tune
09/23/14   vbh     CW cleanup- Use of right argument 
09/15/14   vbh     Fixed the wrong check in tune
09/05/14   vbh     Move to the RFM Tx APis for Init/Deinit, Enable/Disable
08/22/14   vbh     Updated the call to set_tx api with device argument 
08/06/14   aro     Support to override TempComp mode of operation
06/04/14   kai     Avoid using script based tune for dual carrier
05/15/14   aro     Interface to override TxPlim override mode
05/01/14   rmb     Use RFM API for AGC logging.
04/11/14   vbh     [FR17754] Update the tune call flow
2/18/14    vbh     Initial Revision
12/12/13   aro     Support to enable/disable HDET
10/22/13   vs      Add MultiCarrier Tune and Get RxAGC for multi carrier mode
10/17/13   aro     Coding standard compatibility
10/12/13   zhh     Support set debug flag by QRCT
01/28/14   zhh     Update WCDMA Radio Test Interface 
12/12/13   aro     Support to enable/disable HDET
10/22/13   vs      Add MultiCarrier Tune and Get RxAGC for multi carrier mode
10/17/13   aro     Coding standard compatibility
10/12/13   zhh     Support set debug flag by QRCT
09/10/13   aa      Enable/Disable NBEE RXCAL on the fly
07/30/13   aro     Implementation for AGC log trigger
07/30/13   aro     Initial Release : WCDMA Radio Dispatch

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_wcdma_radio.h"
#include "ftm_msg.h"
#include "rfwcdma_core_log.h"

/*----------------------------------------------------------------------------*/
#include "ftm_wcdma_ctl.h"
#include "rfm.h"
#include "rfm_wcdma_ftm.h"
#include "rfcommon_core.h"
#include "rfdevice_wcdma_intf.h"
#include "rfcommon_math.h"
#include "rfwcdma_core_util.h"
#include "rfm_wcdma_param_types.h"
#include "rfumts.h"
#include "ftm_common_concurrency_manager.h"
/*----------------------------------------------------------------------------*/

extern boolean rx_cal_optimization;

extern ftm_lm_buf_type ftm_wcdma_rxlm_buffer[]; /*!< RXLM buffer */

extern ftm_lm_buf_type ftm_wcdma_rxlm_buffer[]; /*!< RXLM buffer */

extern ftm_rf_mode_type ftm_current_rf_mode;

#ifdef FEATURE_WCDMA
extern ftm_lm_buf_type ftm_wcdma_txlm_buffer;
extern boolean ftm_wcdma_10mhz_mode_rf;
extern rfcom_wcdma_band_type ftm_curr_wcdma_mode;
#endif


typedef PACK(struct) 
{
  uint8 cmd;
  uint8 sub_sys;
  uint16 mode;
  uint16 wcdma_cmd;
  uint16 req_len;
  uint16 rsp_len;
  boolean agc_state;
  int16 rx_agc;
  uint8 lna_state;
  uint8 lna_gain_word;
  boolean rx_pll_lock_state;
  
}ftm_wcdma_agc_resp_s;

/*============================================================================*/
/*!
  @name WCDMA Radio Control Functions

  @brief
  Functions for FTM to control the radio in WCDMA Mode of Operation
*/
/*! @{ */
/*============================================================================*/
/*!
  @name Enter WCDMA Mode

  @brief
  Enter WCDMA Mode 
*/

/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter WCDMA Mode 

  @details
  Enter WCDMA Mode , this function call RFM function to Enter WCDMA Mode

  @param device
  rfm device id
 
  @param rf_mode
  value of WCDMA Mode

  @return
  Response packet for the command
*/
boolean
ftm_wcdma_radio_enter_mode
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rf_mode
)
{
  /* Response Packets */
  boolean ret_val = FALSE;
  boolean init_ftm_env = FALSE;
 //uint16 default_channel = 9750;

  if ( device >= RFM_DEVICE_4 )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_wcdma_radio_enter_mode: Unsupported device %d", 
               device );
    return FALSE;
  }

  FTM_MSG_1( FTM_HIGH, "ftm_wcdma_radio_enter_mode: device %d", device );

  if(RFM_IMT_MODE == rf_mode)
  {
    init_ftm_env = ftm_rfmode_enter( device, FTM_STATE_WCDMA );
    if(TRUE == init_ftm_env)
    {
      /* Tear Down RF to enter WCDMA mode */  
     // ftm_wcdma_tear_down(device);
      /* Enter WCDMA Mode */

      if (!ftm_wcdma_rxlm_buffer[device].is_buf_allocated)
      {
           FTM_MSG_1( FTM_HIGH, "ftm_wcdma_radio_enter_mode: RXLM buffer not allocated for %d", device );
      }

      rfm_enter_mode( device, RFCOM_WCDMA_MODE, NULL, NULL, ftm_wcdma_rxlm_buffer[device].buf_idx);

      ret_val = TRUE;
    }/* if ( TRUE ==  init_ftm_env)*/
  }
  else
  {
    ret_val = FALSE;
    FTM_MSG_1( FTM_ERROR, "ftm_wcdma_radio_enter_mode: Unsupported mode %d", 
               rf_mode );
  }
  FTM_MSG_1( FTM_HIGH, "ftm_wcdma_radio_enter_mode: ret_val=%d", 
               ret_val );
  /* Return Response Packet */
  return ret_val;

} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates RF state to tune to given channel upon wakeup. 

  @details
   This function must be called during sleep and is intended to optimize the channel
  in which to tune to during wakeup.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_pre_tune
(
  void	*ftm_req_data
)
{
  PACK(struct) ftm_wcdma_pre_tune_req_s /* Input parameter structure*/
  {
    uint8  cmd;
    uint8  sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8  device; 
    uint8  u8_reserved;
    uint16 channel;
  } *pre_tune_req_ptr;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  boolean ret_val = FALSE;
  rfm_device_enum_type device;
  rf_path_enum_type path;
  rfwcdma_mc_state_enum_type dev_status;
  uint16 chan[3];

  pre_tune_req_ptr = 
      (struct ftm_wcdma_pre_tune_req_s *)ftm_req_data;
  device = pre_tune_req_ptr->device;
  chan[0] = pre_tune_req_ptr->channel;

  path = rfcommon_core_device_to_path(device);
  dev_status = rfwcdma_mc_get_path_state(path);

  if((RFWCDMA_MC_STATE_SLEEP == dev_status) && (device < RFM_DEVICE_4))
  {
    ftm_wcdma_pretune_to_chan(device, chan);
    ret_val = TRUE;
  }
  else
  {
    ret_val = FALSE;
    if ( device >= RFM_DEVICE_4 )
    {
      FTM_MSG_1( FTM_ERROR, "ftm_wcdma_radio_pre_tune: Unsupported device %d", 
                 device );
    }
    else
    {
      FTM_MSG_1( FTM_ERROR, "ftm_wcdma_radio_pre_tune: must be called during sleep "
                     " dev_status=%d", dev_status );
    }
  }
  
  FTM_MSG_1( FTM_HIGH, "ftm_wcdma_radio_pre_tune: ret_val=%d", 
               ret_val );
  /* No response required, just return the original request. */
  if ( ret_val == TRUE )
  {
    ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  }
  else
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_CMD;
  }
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;
  
  /* Return Response Packet */
  return ftm_rsp_data;

} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tune to specific channel. 

  @details
   This function used to tune to specific channel.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_tune
(
  void	*ftm_req_data
)
{
  PACK(struct) ftm_wcdma_tune_req_s /* Input parameter structure*/
  {
    uint8  cmd;
    uint8  sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8  device; 
    uint8  carrier_mask;
    uint16 channel_0;
    uint16 channel_1;
    uint16 channel_2;
  } *tune_req_ptr;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  boolean ret_val = FALSE;
  rfm_device_enum_type device;
  uint8 carrier_mask, num_channels =0;
  rfm_wcdma_rx_tune_params tune_params;
  rfc_rx_path_type rx_path; 
  rfcom_wcdma_band_type band;
  ftm_concurrency_manager_params_type wcdma_cc_params;

  tune_req_ptr = 
      (struct ftm_wcdma_tune_req_s *)ftm_req_data;
  device = tune_req_ptr->device;
  carrier_mask = tune_req_ptr->carrier_mask;
  num_channels = rf_count_one_bits((uint32)carrier_mask);
  memset(&tune_params, 0, sizeof(rfm_wcdma_rx_tune_params)); 
  
  memset(&wcdma_cc_params, 0, sizeof(ftm_concurrency_manager_params_type));

  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;

  if (device > RFM_DEVICE_3)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_wcdma_radio_tune: Invalid device - device:%d",device);

    ftm_rsp_data.cmd = FTM_RSP_BAD_CMD;
    /* Return Response Packet */
    return ftm_rsp_data;
  }

  if (device == RFM_DEVICE_0 || device == RFM_DEVICE_2)
  {
    rx_path = RFC_PRX_PATH;
  }
  else 
  {
    rx_path = RFC_DRX_PATH;
  }

  tune_params.api_params[device].is_valid = TRUE;
  tune_params.api_params[device].rxlm_handle = ftm_wcdma_rxlm_buffer[device].buf_idx;
  tune_params.api_params[device].carrier_mask = carrier_mask;

  switch (num_channels)
  {
     case 1: tune_params.api_params[device].channel[0] = tune_req_ptr->channel_0;
     break;
     
     case 2: tune_params.api_params[device].channel[0] = tune_req_ptr->channel_0;
             tune_params.api_params[device].channel[1] = tune_req_ptr->channel_1;
     break;
     
     case 3: tune_params.api_params[device].channel[0] = tune_req_ptr->channel_0;
             tune_params.api_params[device].channel[1] = tune_req_ptr->channel_1;
             tune_params.api_params[device].channel[2] = tune_req_ptr->channel_2;
     break;
     
     default: tune_params.api_params[device].channel[0] = tune_req_ptr->channel_0;
     break;
  }

  band = rfwcdma_core_convert_chan_to_band(tune_req_ptr->channel_0);

  wcdma_cc_params.prx_drx = rx_path;
  wcdma_cc_params.rfm_dev = device;
  wcdma_cc_params.band.wcdma_band = band; 
  wcdma_cc_params.tech = RFM_IMT_MODE;  

  FTM_MSG_3(FTM_HIGH, "Number of channels selected is %d, channel 0 is %d band %d", num_channels, tune_req_ptr->channel_0, band);


  if (carrier_mask == RFWCDMA_MC_SC || carrier_mask == RFWCDMA_MC_DC) 
  {
    wcdma_cc_params.carrier_index = 0;
  }
  else if (carrier_mask == 2) 
  {
    wcdma_cc_params.carrier_index = 1;
  }

  if((ftm_concurrency_manager_validate_operation(&wcdma_cc_params)))
  {
  
  if(carrier_mask == RFWCDMA_MC_SC)
  {
    /* scripts based tune does not support dual carrier */
    rfm_wcdma_rx_prep_tune(&tune_params, NULL, NULL);
   
    rfm_wcdma_rx_exec_tune(&tune_params, NULL, NULL);
  }
  else
  {
    rfm_tune_to_multi_chan_v2(device,
                              tune_params.api_params[device].channel,
                              carrier_mask,
                              RFM_NUM_TUNES,
                              NULL,
                              NULL,
                              ftm_wcdma_rxlm_buffer[device].buf_idx);
  }
    
  ret_val = TRUE;                       
  }
  else
  {
   FTM_MSG_2( FTM_ERROR, "ftm_wcdma_radio_tune: Invalid concurreny scenario - device:%d, band: %d,",
                          device,band );
   ret_val = FALSE;
  }

  
  FTM_MSG_1( FTM_HIGH, "ftm_wcdma_radio_tune: ret_val=%d", ret_val );

  /* No response required, just return the original request. */
  if ( ret_val == TRUE )
  {
    ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  }
  else
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_CMD;
  }
  
  /* Return Response Packet */
  return ftm_rsp_data;

} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wakeup Rf devices. 

  @details
   This function used to wakeup Rf devices.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_rx_wakeup
(
  void	*ftm_req_data
)
{
  PACK(struct) ftm_wcdma_pre_tune_req_s /* Input parameter structure*/
  {
    uint8  cmd;
    uint8  sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8  device; 
    uint8  u8_reserved;
    uint16 channel;
  } *pre_tune_req_ptr;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  boolean ret_val;
  rfm_device_enum_type device;
  rf_path_enum_type path;
  rfwcdma_mc_state_enum_type dev_status;

  pre_tune_req_ptr = 
      (struct ftm_wcdma_pre_tune_req_s *)ftm_req_data;
  device = pre_tune_req_ptr->device;

  path = rfcommon_core_device_to_path(device);
  dev_status = rfwcdma_mc_get_path_state(path);

  if((RFWCDMA_MC_STATE_SLEEP == dev_status) && (device < RFM_DEVICE_4))
  {
    ftm_wcdma_wakeup(device);
    ret_val = TRUE;
  }
  else
  {
    ret_val = FALSE;
    if(device >= RFM_DEVICE_4)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_wcdma_radio_rx_wakeup: Unsupported device %d", 
                 device );
    }
    {
      FTM_MSG_1( FTM_HIGH, "ftm_wcdma_radio_rx_wakeup: dev_status=%d", 
                 dev_status);
    }
  }

  FTM_MSG_1( FTM_HIGH, "ftm_wcdma_radio_rx_wakeup: ret_val=%d", 
                  ret_val );

  /* No response required, just return the original request. */
  if ( ret_val == TRUE )
  {
    ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  }
  else
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_CMD;
  }
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;
  
  /* Return Response Packet */
  return ftm_rsp_data;

} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sleep Rf devices. 

  @details
   This function used to sleep Rf devices.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_rx_sleep
(
  void	*ftm_req_data
)
{
  PACK(struct) ftm_wcdma_pre_tune_req_s /* Input parameter structure*/
  {
    uint8  cmd;
    uint8  sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8  device; 
    uint8  u8_reserved;
    uint16 channel;
  } *pre_tune_req_ptr;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  boolean ret_val;
  rfm_device_enum_type device;
  rf_path_enum_type path;
  rfwcdma_mc_state_enum_type dev_status;

  pre_tune_req_ptr = 
      (struct ftm_wcdma_pre_tune_req_s *)ftm_req_data;
  device = pre_tune_req_ptr->device;  

  path = rfcommon_core_device_to_path(device);
  dev_status = rfwcdma_mc_get_path_state(path);

  if((RFWCDMA_MC_STATE_SLEEP != dev_status) && (device < RFM_DEVICE_4))
  {
    ftm_wcdma_sleep(device);
    ret_val = TRUE;
  }
  else
  {
    ret_val = FALSE;
    if(device >= RFM_DEVICE_4)
    {
      FTM_MSG_1( FTM_ERROR, "ftm_wcdma_radio_rx_sleep: Unsupported device %d", 
                 device );
    }
    else
    {
      FTM_MSG_1( FTM_HIGH, "ftm_wcdma_radio_rx_sleep: dev_status=%d", 
                 dev_status );
    }
  }

  FTM_MSG_1( FTM_HIGH, "ftm_wcdma_radio_rx_sleep: ret_val=%d", 
                  ret_val );
  
  /* No response required, just return the original request. */
  if ( ret_val == TRUE )
  {
    ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  }
  else
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_CMD;
  }
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;
  
  /* Return Response Packet */
  return ftm_rsp_data;

} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Init Rf devices to Tx state. 

  @details
   This function used to enable Rf devices to Tx state.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_tx_init
(
  void	*ftm_req_data
)
{
  PACK(struct) ftm_wcdma_pre_tune_req_s /* Input parameter structure*/
  {
    uint8  cmd;
    uint8  sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8  device; 
    uint8  u8_reserved;
    uint16 channel;
  } *pre_tune_req_ptr;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */

  pre_tune_req_ptr = 
      (struct ftm_wcdma_pre_tune_req_s *)ftm_req_data;  
  FTM_MSG_1(FTM_HIGH,"ftm_wcdma_radio_tx_init called on device - %d", pre_tune_req_ptr->device);
  
  ftm_wcdma_set_tx(pre_tune_req_ptr->device, TRUE);

  FTM_MSG( FTM_HIGH, "ftm_wcdma_radio_tx_init: Done");
  
  /* No response required, just return the original request. */
  ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;
  
  /* Return Response Packet */
  return ftm_rsp_data;

} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable Tx. 

  @details
   This function used to enable Tx.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_tx_enable
(
  void	*ftm_req_data
)
{
  PACK(struct) ftm_wcdma_pre_tune_req_s /* Input parameter structure*/
  {
    uint8  cmd;
    uint8  sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8  device; 
    uint8  u8_reserved;
    uint16 channel;
  } *pre_tune_req_ptr;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  rfm_device_enum_type device; 
  boolean ret_val;
  rfm_wcdma_tx_enable_params tx_params;

  memset(&tx_params, 0, sizeof(rfm_wcdma_tx_enable_params)); 

  pre_tune_req_ptr = 
      (struct ftm_wcdma_pre_tune_req_s *)ftm_req_data;
  device = pre_tune_req_ptr->device;

  if(device < RFM_DEVICE_4)
  {
    tx_params.device = device; 
    rfm_wcdma_tx_enable(&tx_params, NULL, NULL);
    ret_val = TRUE;
  }
  else
  {
    ret_val = TRUE;
    FTM_MSG_1( FTM_ERROR, "ftm_wcdma_radio_tx_enable: Unsupported device %d", 
                 device );
  }

  FTM_MSG_1( FTM_HIGH, "ftm_wcdma_radio_tx_enable: ret_val=%d", 
                  ret_val );
  
  /* No response required, just return the original request. */
  if(TRUE == ret_val)
  {
    ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  }
  else
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_CMD;
  }
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;
  
  /* Return Response Packet */
  return ftm_rsp_data;

} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable tx. 

  @details
   This function used to disable Tx.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_tx_disable
(
  void	*ftm_req_data
)
{
  PACK(struct) ftm_wcdma_pre_tune_req_s /* Input parameter structure*/
  {
    uint8  cmd;
    uint8  sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8  device; 
    uint8  u8_reserved;
    uint16 channel;
  } *pre_tune_req_ptr;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  rfm_device_enum_type device;
  boolean ret_val;
  rfm_wcdma_tx_disable_params tx_params;

  memset(&tx_params, 0, sizeof(rfm_wcdma_tx_disable_params)); 

  pre_tune_req_ptr = 
      (struct ftm_wcdma_pre_tune_req_s *)ftm_req_data;
  device = pre_tune_req_ptr->device;
  if(device < RFM_DEVICE_4)
  { 
    tx_params.device = device; 
    rfm_wcdma_tx_disable(&tx_params, NULL, NULL);
    ret_val = TRUE;
  }
  else
  {
    ret_val = FALSE;
    FTM_MSG_1( FTM_ERROR, "ftm_wcdma_radio_tx_disable: Unsupported device %d", 
                 device );
  }

  FTM_MSG_1( FTM_HIGH, "ftm_wcdma_radio_tx_disable: ret_val=%d", 
                  ret_val );
  
  /* No response required, just return the original request. */
  ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;
  
  /* Return Response Packet */
  return ftm_rsp_data;

} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  wakeup Tx. 

  @details
   This function used to wakeup Tx.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_tx_wakeup
(
  void	*ftm_req_data
)
{
  PACK(struct) ftm_wcdma_pre_tune_req_s /* Input parameter structure*/
  {
    uint8  cmd;
    uint8  sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8  device; 
    uint8  u8_reserved;
    uint16 channel;
  } *pre_tune_req_ptr;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */

  pre_tune_req_ptr = 
      (struct ftm_wcdma_pre_tune_req_s *)ftm_req_data;

  FTM_MSG_1(FTM_HIGH,"ftm_wcdma_radio_tx_wakeup called on device - %d", pre_tune_req_ptr->device);

  rfm_wcdma_wakeup_tx();

  FTM_MSG( FTM_HIGH, "ftm_wcdma_radio_tx_wakeup: Done" );
  
  /* No response required, just return the original request. */
  ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_wcdma_radio_tx_wakeup */

/*! @} */

/*============================================================================*/
/*!
  @name Debug

  @brief
  Functions used for WCDMA debugging
*/
/*! @{ */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure CDMA debug Mode

  @details
  Depending on the mask, this function will set various CDMA modules to debug
  mode.

  @param *ftm_req_data
  FTM Request Data to configure CDMA Debug mode

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_trigger_agc_log
(
  void  *ftm_req_data
)
{
  PACK(struct) ftm_wcdma_trigger_agc_logreq_s /* Input parameter structure*/
  {
    uint8 cmd;
    uint8 sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8 immediate_log; /* Indicates if log is immediate or buffered */
    uint16 num_of_log_sample; /* Number of samples for buffered log */
    uint16 delay_bet_samples; /* Delay between samples for buffered log */
  } *trig_agc_log_req_ptr;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */

  boolean immediate_log;
  uint16 num_of_log_sample;
  uint16 delay_bet_samples;
  uint16 sample_idx;

  /* Request Packet */
  trig_agc_log_req_ptr = 
      (struct ftm_wcdma_trigger_agc_logreq_s *)ftm_req_data;

  immediate_log = (boolean)trig_agc_log_req_ptr->immediate_log;
  num_of_log_sample = (uint16)trig_agc_log_req_ptr->num_of_log_sample;
  delay_bet_samples = (uint16)trig_agc_log_req_ptr->delay_bet_samples;

  /* Check if immediate logging is to be done */
  if ( immediate_log == TRUE )
  {
    rfm_log_single_agc_data(RFM_DEVICE_0);
  } /* if ( immediate_log == TRUE ) */
  else
  {
    /* Continue only is more than 0 samples of AGC is to be done */
    if ( num_of_log_sample > 0 )
    {
      /* Call Logging num_of_log_sample times */
      for ( sample_idx = 0; sample_idx < num_of_log_sample; sample_idx++ )
      {
        rfm_log_agc(RFM_DEVICE_0);
        DALSYS_BusyWait(delay_bet_samples);
      } /*for ( sample_idx = 0; sample_idx < num_of_log_sample; sample_idx++ )*/
    } /* if ( num_of_log_sample > 0 ) */
    else
    {
      rfm_log_agc(RFM_DEVICE_0);
    }
  } /* if ! ( immediate_log == TRUE ) */

  FTM_MSG_3( FTM_HIGH, "ftm_wcdma_radio_trigger_agc_log: "
             "IMM %d, Num %d, Delay IMM %d : [Done]", 
             immediate_log,
             num_of_log_sample, 
             delay_bet_samples );

  /* No response required, just return the original request. */
  ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_wcdma_radio_trigger_agc_log */

/*! @} */


/*!
  @brief
  Enables or Disabled RX CAL optimization (using NBEE)

  @details
  This function is called by host to enable/disable RX calibration optimization
 
  @param on_off: TRUE/FALSE
  
*/
ftm_rsp_pkt_type ftm_wcdma_set_rxcal_optimization( void  *ftm_req_data )
{

PACK(struct) ftm_wcdma_trigger_agc_logreq_s /* Input parameter structure*/
  {
    uint8 cmd;
    uint8 sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8 nbee_enabled; 
  } *trig_agc_log_req_ptr;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */

  boolean nbee_enabled;

  /* Request Packet */
  trig_agc_log_req_ptr = 
      (struct ftm_wcdma_trigger_agc_logreq_s *)ftm_req_data;

  nbee_enabled = (boolean)trig_agc_log_req_ptr->nbee_enabled;


  /* Check if immediate logging is to be done */
  if ( nbee_enabled == 1 )
  {
    rx_cal_optimization = TRUE;
  } /* if ( nbee_enabled == TRUE ) */
  else
  {
    rx_cal_optimization = FALSE;
  } /* if ! ( nbee_enabled == TRUE ) */

  FTM_MSG_2( FTM_HIGH, "ftm_wcdma_set_rxcal_optimization: "
             "nbee_enabled = %d, rxcal flag = %d: [Done]", 
             nbee_enabled,
             rx_cal_optimization
             );

  /* No response required, just return the original request. */
  ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;
  
  /* Return Response Packet */
  return ftm_rsp_data;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tune to specific channel. 

  @details
   This function used to tune to specific channel.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_get_rxagc
(
  void	*ftm_req_data
)
{
  PACK(struct) ftm_wcdma_agc_req_s /* Input parameter structure*/
  {
    uint8  cmd;
    uint8  sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8  device; 
    uint8  multi_carrier_idx;
    uint8  agc_type;
  } *agc_req_ptr;
 
 uint8 device =0, carrier =0, carrier_id =0, pll_status =0;
 rfm_wcdma_ftm_rxagc_info ftm_wcdma_agc_state; 
 ftm_wcdma_agc_resp_s *agc_rsp_type;
 ftm_rsp_pkt_type rsp_pkt;
 boolean status, rx_agc_enabled;
 rf_path_enum_type path;

 agc_req_ptr = (struct ftm_wcdma_agc_req_s *)ftm_req_data;
 
 rsp_pkt = ftm_common_malloc_rsp_pkt( agc_req_ptr, sizeof(ftm_wcdma_agc_resp_s)); 

 agc_rsp_type = (ftm_wcdma_agc_resp_s*)rsp_pkt.pkt_payload;

 if(agc_rsp_type == NULL)
 {
   FTM_MSG(FTM_ERROR, "ftm_wcdma_radio: ftm_common_malloc_rsp_pkt returned NULL");
   return rsp_pkt;
 }
 device = agc_req_ptr->device; 

 carrier = agc_req_ptr->multi_carrier_idx; 

 if (rf_count_one_bits((uint32)carrier)> 1)
  {
      carrier_id = RFCOM_MULTI_CARRIER_ID__CUMULATIVE;
  }
 else 
 {
     switch (carrier)
     {
        case 1: carrier_id =0;
        break;
        
        case 2:carrier_id =1;
        break;
        
        case 4: carrier_id =2;
        break;
        
        default: carrier_id =0; 
     }
 }

 FTM_MSG_1(FTM_HIGH ,"ftm_wcdma_radio: Computed RFCOM multi-carrier index is :%d",carrier_id);

 path = rfcommon_core_device_to_path(device);

 rx_agc_enabled = rfwcdma_core_util_is_rxagc_enabled((rfagc_receive_chain_type)path, carrier_id);

 pll_status = rfdevice_wcdma_rx_cmd_dispatch(device,ftm_curr_wcdma_mode,RFDEVICE_WCDMA_GET_PRX_SIG_PATH_STATUS, &pll_status);

 status = rfm_wcdma_ftm_get_rxagc_info(device,carrier_id,RFCOM_RXAGC_IN_DB10,&ftm_wcdma_agc_state);

 if (status == TRUE)
 {
     agc_rsp_type->rx_agc= ftm_wcdma_agc_state.rxagc_val;
     agc_rsp_type->lna_state = ftm_wcdma_agc_state.lna_state;
     agc_rsp_type->lna_gain_word =0;
     agc_rsp_type->rx_pll_lock_state = pll_status;
     agc_rsp_type->agc_state= rx_agc_enabled ;

     FTM_MSG_5( FTM_MED, "ftm_wcdma_radio_get_rxagc: Dev %d, RxAGC %d," 
                          "LNA %d, LNA Word %d, PLL lock: %d", 
                          device, agc_rsp_type->rx_agc, 
                          agc_rsp_type->lna_state, 
                          agc_rsp_type->lna_gain_word, 
                          agc_rsp_type->rx_pll_lock_state);
 }

 else 
    FTM_MSG_1(FTM_MED, "Get RxAGC info failed",0); 
  
   
  return rsp_pkt;
 
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override PLIM disable mode. 

  @details
  This function used to Override PLIM disable mode.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_override_plim
(
  void *ftm_req_data
)
{
  PACK(struct) ftm_wcdma_override_plim_req_s /* Input parameter structure*/
  {
    uint8  cmd;
    uint8  sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8  device; 
    uint8  plim_enable;
  } *override_plim_req_ptr;
 
  rfm_device_enum_type device;
  boolean plim_enable;
  boolean ret_val;
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  
  override_plim_req_ptr = (struct ftm_wcdma_override_plim_req_s *)ftm_req_data;
    
  device = override_plim_req_ptr->device; 
  plim_enable = override_plim_req_ptr->plim_enable; 

  ret_val = rfm_wcdma_ftm_plim_override_mode(device,plim_enable);

  if (ret_val == TRUE)
  {
    FTM_MSG_2( FTM_MED, "ftm_wcdma_radio_override_plim: Dev %d, Enable %d",
               device, plim_enable);
  }
  else 
  {
    FTM_MSG_2( FTM_ERROR, "ftm_wcdma_radio_override_plim: Dev %d, Enable %d : Failed",
               device, plim_enable);
  }

    /* No response required, just return the original request. */
  ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;

  return ftm_rsp_data;
 
} /* ftm_wcdma_radio_override_plim */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override TempComp disable mode. 

  @details
  This function used to Override TempComp disable mode.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_wcdma_radio_override_tempcomp
(
  void *ftm_req_data
)
{
  PACK(struct) ftm_wcdma_override_tempcomp_req_s /* Input parameter structure*/
  {
    uint8  cmd;
    uint8  sub_sys;
    uint16 mode;
    uint16 wcdma_cmd;
    uint16 req_len;
    uint16 rsp_len;
    uint8  device; 
    uint8  tempcomp_enable;
  } *override_tempcomp_req_ptr;
 
  rfm_device_enum_type device;
  boolean tempcomp_enable;
  boolean ret_val;
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  
  override_tempcomp_req_ptr = (struct ftm_wcdma_override_tempcomp_req_s *)ftm_req_data;
    
  device = override_tempcomp_req_ptr->device; 
  tempcomp_enable = override_tempcomp_req_ptr->tempcomp_enable; 

  ret_val = rfm_wcdma_ftm_temp_comp_override(device,tempcomp_enable);

  if (ret_val == TRUE)
  {
    FTM_MSG_2( FTM_MED, "ftm_wcdma_radio_override_tempcomp: Dev %d, Enable %d",
               device, tempcomp_enable);
  }
  else 
  {
    FTM_MSG_2( FTM_ERROR, "ftm_wcdma_radio_override_tempcomp: Dev %d, Enable %d : Failed",
               device, tempcomp_enable);
  }

    /* No response required, just return the original request. */
  ftm_rsp_data.cmd = FTM_RSP_DO_LEGACY;
  ftm_rsp_data.pkt_len = 0;
  ftm_rsp_data.pkt_payload = NULL;
  ftm_rsp_data.delete_payload = FALSE;

  return ftm_rsp_data;
 
} /* ftm_wcdma_radio_override_tempcomp */


#endif /*FEATURE_FACTORY_TESTMODE */


