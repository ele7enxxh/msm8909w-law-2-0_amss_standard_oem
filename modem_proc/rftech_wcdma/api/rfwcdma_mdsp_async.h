#ifndef RFWCDMA_MDSP_ASYNC_H 
#define RFWCDMA_MDSP_ASYNC_H 

/*! @brief
  @file

  @brief
  This module contains the definitions of the WCDMA RF-mDSP sync interface.
*/

/*===========================================================================


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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_mdsp_async.h#1 $
  $DateTime: 2016/12/13 07:59:54 $
  $Author: mplcsds1 $

when       who     what, where, why 
--------   ---     ---------------------------------------------------------- 
07/01/15   zhh    return the mtpl_adj_db10 to SW
05/27/15   zhh    FBRX ILPC optimize
04/03/15   zhh     update PA phase comp offset at the first Tx AGC update
11/26/14   ac      Workaround to ABORT all pending Snums during disable Tx
05/01/14   dw      Added update ET delay and set freq adjust in MDSP async set data command
03/24/14   kai     Add ET delay update during temp comp 
03/13/14   rmb     Pass the IRAT flag to config chan and band API.
01/08/14    dw     Add mdsp/FED support for wakeup-tune config
11/14/13    dw     Split rfwcdma_mdsp_async_get_rx_data and rfwcdma_mdsp_async_get_tx_data
10/28/13    dw      Add LNA state logging support
10/22/13    dw     Added rfwcdma_mdsp_exit
09/24/13    dw     Add rfwcdma_mdsp_async_set_data and rfwcdma_mdsp_async_get_data
09/09/13    dw     More updates for Bolt
08/29/13    dw     Update with FED DM support
08/23/13    dw     Initial Verision
==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfwcdma_mdsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize RF-MDSP module

  @details
  This function will initialize RF-MDSP modulde and should be done only once. It
  includes the initialization of:
  1. RF-MDSP data module
  2. RF-MDSP sync interface
  3. RF-MDSP async paramaters 

  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_init_once(void);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize RF-MDSP asysnc interface paramters

  @details
  This function will initialize RF-MDSP async interface (WFW SM, FED data module)
  parameters that are band/device independent.

  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_async_init(void);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure Rx band-specific RF-MDSP Async interface

  @details
  This function will populate RF-MDSP async interface (SW-FW shared memory or
  FED data module) with band-specific Rx parameters and device scripts.
 
  Note: this function needs to be exectued before rfwcdma_mdsp_async_config_rx_chan
 
  @param cfg
  pointer to the band specific config data structure 
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_async_config_rx_band
(
  rfwcdma_mdsp_rx_cfg_bsp_type* cfg
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure Rx channel-specific RF-MDSP Async interface

  @details
  This function will populate RF-MDSP async interface (SW-FW shared memory or
  FED data module) with channel-specific Rx parameters.
 
  Note: this function needs to be exectued after rfwcdma_mdsp_async_config_rx_band
  For retune within the same band, config_rx_chan will be called multiple times
  without config_rx_band to re-config channel specific paramters only.
 
  @param cfg
  pointer to the channel specific config data structure 
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_async_config_rx_chan
(
  rfwcdma_mdsp_rx_cfg_chsp_type* cfg
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure RF-MDSP Async interface for RF wakeup config

  @details
  This function will populate RF-MDSP async interface (DM) with scripts and
  parameters neeeded for RF wakeup
 
  Note: this function created to support RF wakeup optimization feature where
  scripts are pushed to DM and executed in FW/FED context
 
  @param cfg
  pointer to the Rx wakeup data structure 
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_async_config_rx_wakeup
(
  rfwcdma_mdsp_rx_wakeup_cfg_type* cfg
);
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure Tx band-specific RF-MDSP Async interface

  @details
  This function will populate RF-MDSP async interface (SW-FW shared memory or
  FED data module) with band-specific Tx parameters and device scripts.
 
  @param cfg
  pointer to the band specific config data structure 
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_async_config_tx_band
(
  rfwcdma_mdsp_tx_cfg_bsp_type* cfg
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure Tx channel-specific RF-MDSP Async interface

  @details
  This function will populate RF-MDSP async interface (SW-FW shared memory or
  FED data module) with channel-specific Rx parameters.
 
  @param cfg
  pointer to the channel specific config data structure 
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_async_config_tx_chan
(
  rfwcdma_mdsp_tx_cfg_chsp_type* cfg
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure multi-carrier-specific RF-MDSP Async interface

  @details
  This function will populate RF-MDSP async interface (SW-FW shared memory or
  FED data module) with channel-specific Rx parameters.
 
  @param cfg
  pointer to the multi-carrier specific config data structure 
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_async_config_tx_mc
(
  rfwcdma_mdsp_tx_mc_cfg_type* cfg
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to set specific data to SM/FED DM interface based on type
  ID

  @details
  Generic function to set specific data to SM/FED DM interface based on type
  ID. Currently support following set interface data operation:
 
  RFWCDMA_MDSP_SET_VGA_GAIN_OFFSET:
  Set a specified VGA gain offset to FED DM for FTM/CAL override operation
 
  RFWCDMA_MDSP_SET_LNA_GAIN_OFFSET
  Set a specified LNA gain offset to FED DM for FTM/CAL override operation 
 
  RFWCDMA_MDSP_SET_LNA_GAIN_STATE
  Set a specified LNA gain state override to FED DM for FTM/CAL override operation
 
  RFWCDMA_MDSP_SET_FREQ_ADJUST
  Set a specified frequency error  compensation to WFW XO intf for FTM/CAL override operation
 
  Do not use this function for main RF configuration parameters that has already
  been included in the main Rx/Tx config data within RF driver API .
  Only use this API for specific parameter override required mostly in FTM Cal procedure.
 
  @param param
  pointer to data parameter including ID and data value to set 
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_async_set_data
(
  rfwcdma_mdsp_set_data_param_type* param
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to get specific data value from SM/FED DM interface based on type
  ID

  @details
  Generic function to get specific data value from SM/FED DM interface based on type
  ID. Currently support following get interface data operation:
 
  RFWCDMA_MDSP_GET_TXAGC_VAL
  Get TxAGC value

  RFWCDMA_MDSP_GET_TXAGC_PARAM
  Get current TxAGC parameters: RGI,PA bias, PA state, PA current, IQ gain, envolope scale, DPD index 

  RFWCDMA_MDSP_GET_TX_ON_STATUS
  Get FW Tx ON/PA ON status
 
  @param param
  pointer to data structure including ID and data structure to copy to
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_async_get_tx_data
(
  rfwcdma_mdsp_get_data_param_type* param
);
boolean rfwcdma_mdsp_async_get_rx_data
(
  rfwcdma_mdsp_get_data_param_type* param
);
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Funtion to do RF WCDMA MDSP module cleanup
 
  @details
  Perform all necessary cleanup related to RF WCDMA MDSP/FED module, free up
  any memory allocated, called when WCDMA stack is teared down (paired with
  rfwcdma_mdsp_init_once() function call)
 
  @return
  TRUE if cleanup is successful
*/
boolean rfwcdma_mdsp_exit
(
  void
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Funtion to abort the tuner AOL update
 
  @details
  Perform all necessary cleanup related tuner AOL operarion including the Snum 
  count
 
  @return
  None
*/
void rfwcdma_mdsp_async_abort_tuner_open_loop_update
(
uint8 txlm_handle
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Funtion to abort the pending AOL snums
 
  @return
  None
*/

void rfwcdma_mdsp_sync_abort_open_loop_pending_snums
(
 void
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Funtion to set the first txagc update flag
 
  @param rflm_handle
  txlm handle id to the current txagc handle
 
  @param first_txagc_ind
  indicate for the first txagc update
 
  @return
  None
*/
void 
rfwcdma_mdsp_async_set_first_txagc_update_flag
(
  rflm_handle_tx_t rflm_handle,
  boolean first_txagc_ind
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  set temp comp indication to rflm
 
  @param rflm_handle
  txlm handle id to the current txagc handle
  @param temp_com_ind
  temp comp indication
  
  @return
  None
*/
void 
rfwcdma_mdsp_async_set_temp_comp_ind
(
  rflm_handle_tx_t rflm_handle,
  boolean temp_comp_ind
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  update the lut_rvt and tx error accum during temp comp
 
  @param rflm_handle
  txlm handle id to the current txagc handle
 
  @return
  None
*/
void 
rfwcdma_mdsp_async_set_lut_rvt
(
  rflm_handle_tx_t rflm_handle
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  return the mtpl_adj_db10 to SW. mtpl_adj_db10 is the value when Tx LUT revert, FED
  will pass this value to FW.
 
  @param rflm_handle
  txlm handle id to the current txagc handle
 
  @return
  mtpl_adj_db10
*/
int16 rfwcdma_mdsp_async_get_mtpl_adj_db10
(
  rflm_handle_tx_t rflm_handle
);

#ifdef __cplusplus
#endif
#endif /* RFWCDMA_MDSP_ASYNC_H */
