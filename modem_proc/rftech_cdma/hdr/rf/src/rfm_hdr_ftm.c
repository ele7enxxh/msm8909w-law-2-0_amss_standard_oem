/*! 
  @file
  rfm_hdr_ftm.c
 
  @brief
  This file defines the RFM-HDR interfaces to be used by <b>FTM module only</b>.
 
  @addtogroup RF_CDMA_HDR_RFM_FTM
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/src/rfm_hdr_ftm.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/22/14   spa     KW warning fixes
08/13/14   cd      Support for HDR FTM logging
08/01/14   cd      Add support for FTM AGC logging with FED and common tx AGC 
                   functionality
05/27/14   zhw     Check for Tx state and TXLM handle before querying TxAGC data
05/02/14   spa     Added API rfm_hdr_ftm_query_iq_sample_line_offset 
03/28/14   APU     Hook up Tx logging interfaces in SW driver to log data 
                   provided by RFLM.
03/20/14   spa     Initial support to test Rx on/off on RFLM side
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
06/18/13   JJ      Add func to register/de-register fw messages  
05/30/13   spa     Mainlined "use_new_fwrsp", use new RF dispatch implementation 
05/23/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
04/02/13   spa     Added NULL pointer check for FW housekeeping data (KW fix)
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
03/17/13   zhw     Fix APT value & PA state not being updated in get_tx_agc_data
02/21/13   spa     Add API to send pilot meas stop stream to FW (FTM IRAT)
02/07/13   spa     Added rfm_hdr_ftm_pilot_meas_cfg API 
12/26/12   shb     Updates for different physical rx_device per band
12/20/12   sty     Reworked rfm_hdr_ftm_set_tx_iq_gain to fix KW errors
12/06/12   APU     Replaced LM chains with Modem chains.
12/04/12   aro     Added HDR support for RF-FW synchronization
09/22/12   APU     Added FTM support for setting the individual pilot carrier 
                   power.
09/05/12   aro     Added RF-FW Sync callback parameter in HDR MDSP Interface
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
05/31/12   spa     Updated ftm_hdr_tear_down to use RFM HDR teardown API 
05/14/12   aro     Updated HDR FW messages to use semaphores from SemPool for
                   RF-FW synchronization
04/18/12   aro     Moved FWS transition logic in upper layer
04/18/12   aro     Added callback function as parameter to override functions
01/23/12   hdz     Fixed compiling warning 
01/20/12   hdz     Changed interface of rfm_hdr_ftm_set_tx_pwr()
01/03/12   aro     Interface to query TxLM chain for the given TxLM buffer
01/03/12   aro     Added TxLM Chain specific to 1x and HDR in logical
                   device param
12/20/11   aro     Enabled logging for APT value and TxLUT index
11/16/11   aro     Renamed FW state change function
11/10/11   aro     Upadated FW state config interface to pass semaphore to 
                   wait on for blocking calls
09/27/11   aro     Added call to CDMA HAL to update TXLM with dynamic update
                   for Tx DP IQ Gain value
09/27/11   aro     Updated Tx DP IQ Gain update function to have txlm_handle
                   and return value
09/13/11   cd      Update RFM FTM interface to override OL Tx power 
09/13/11   aro     Added carrier and cumulative Tx total power
09/07/11   shb     Reworked TX AGC Logical data to have carrier and cumulative 
                   total power
08/24/11   aro     Updated Tx Data query function to have more Tx parameter
08/12/11   aro     Added support for HDR LNA override clear
07/19/11   Saul    Combined FW enable/disable APIs into one FW config state API.
06/14/11   sty     Invoke APIs to convert AGC to requested format
                   Added format arg to agc query APIs
                   Added implementation for rfm_hdr_ftm_get_tx_agc_data
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
05/31/11   aro     Added NULL pointer check for Logical Device param
05/24/11   aro     Added Logical Device Mapping to MDSP Chain for SV support
04/20/11   sty     Added dummy values in rfm_hdr_ftm_get_tx_agc_data() - for KW
04/15/11   aro     Reverting Previous changes
04/15/11   aro     Temporary Checkin for Linker Error Fix
04/12/11   aro     Added Enable/Disable Firmware interface
03/24/11   aro     Unified the RxAGC Read Interface
03/14/11   aro     Renamed RxAGC structure
02/13/11   aro     Added Set Tx Power Interface
02/13/11   aro     [1] Updated LNA query interface
                   [2] Added support for Get TxAGC
02/12/11   aro     Changed the interface to return LNA Gain state and Gain word
01/24/11   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfm_hdr_ftm.h"
#include "rf_hdr_mdsp.h"
#include "rfdevice_cdma_interface.h"
#include "rfc_cdma.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_tx_agc.h"
#include "rfcommon_msg.h"
#include "rf_cdma_data.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_utils_freq.h"
#include "rfm_internal.h"
#include "rfcommon_core_txlin.h"
#include "rf_hdr_mc.h"

/*============================================================================*/
/*!
  @name Tx parameter Configuration

  @brief
  Functions used to configure RF parameter in FTM Mode
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets 1x Open Loop or Closed Loop Tx Power.

  @details
  This function sets Open Loop or Closed Loop Transmit Power Level depending 
  on the parameter passed. This API takes OL or CL Tx Power in dBm. The
  conversion of dBm power to corresponding MDSP units will be done in
  RF-MDSP layer.
 
  @param device
  RF Device under consideration

  @param carrier
  Current carrier of the operation

  @param tx_pwr_mode
  Defines whether to use Closed Loop Power Mode or open Loop Power Mode

  @param tx_pwr
  Open Loop or Closed Loop Power in dBm to be set.
 
  @return
  Flag indicating success or failure of the operation 
*/
boolean
rfm_hdr_ftm_set_tx_pwr
(
  rfm_device_enum_type device,
  const rfm_cdma_carrier_type carrier,
  rfm_cdma_tx_pwr_mode_type tx_pwr_mode,
  int16 tx_pwr
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean ret_val = FALSE ; /* Return Value */
  const rf_cdma_data_status_type *dev_status_r; 
  /* Device Status read Pointer */
  
  dev_status_r = rf_cdma_get_mutable_device_status( device ); 

  if ( NULL == dev_status_r )
  {
    RF_MSG( RF_ERROR, "rfm_hdr_ftm_set_tx_pwr : NULL Device Param Ptr" );
  }
  else 
  {
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL pointer check */
  if ( logical_dev != NULL )
  {
      int16 closed_loop_val = 0; /* force closed_loop val to 0 */
    if (tx_pwr_mode == RFM_CDMA_TX_PWR_OPEN_LOOP)
    {
        int16 K10 = 
          rf_cdma_get_turnaround_constant(dev_status_r->curr_band)*10;
        ret_val = rf_hdr_mdsp_tx_open_loop_override( logical_dev->tx_mdsp_path,
                                                    carrier, tx_pwr, K10,
                                                    NULL, NULL, NULL );
      ret_val &= rf_hdr_mdsp_tx_closed_loop_override( logical_dev->tx_mdsp_path,
                                                      carrier, closed_loop_val, 
                                                      NULL, NULL, NULL );
    }
    else
    {
        ret_val &= rf_hdr_mdsp_tx_closed_loop_override( logical_dev->tx_mdsp_path,
                                                        carrier, closed_loop_val, 
                                                        NULL, NULL, NULL );
      }
    }
    else
    {
      RF_MSG( RF_ERROR, "rfm_hdr_ftm_set_tx_pwr : NULL Device Param Ptr" );
    }
  }
  return ret_val ;
} /* rfm_hdr_ftm_set_tx_pwr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Clears Tx Power override.

  @details
  This function clears override so the TxAGC is closed loop again.  
 
  @param device
  RF Device under consideration

  @param carrier
  Current carrier of the operation

  @return
  Flag indicating success or failure of the operation 
*/
boolean
rfm_hdr_ftm_clear_tx_pwr_override
(
  rfm_device_enum_type device,
  const rfm_cdma_carrier_type carrier,
  rfm_cdma_tx_pwr_mode_type tx_pwr_mode 
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean ret_val = FALSE ; /* Return Value */
  /* Device Status Write Pointer */
  rf_cdma_data_status_type *dev_status_w = 
     rf_cdma_get_mutable_device_status( device ); 

  if ( NULL == dev_status_w )
  {
    RF_MSG( RF_ERROR, "rfm_hdr_ftm_set_tx_pwr : NULL Device Param Ptr" );
  }
  else
  {
    /* Get Logical Device Param */
    logical_dev = rfc_cdma_get_logical_device_params( device );
    /* NULL pointer check */
    if ( logical_dev != NULL )
    {
      if ( tx_pwr_mode == RFM_CDMA_TX_PWR_OPEN_LOOP )
      {
        ret_val = 
        rf_hdr_mdsp_tx_open_loop_clear_override ( logical_dev->tx_mdsp_path , 
                                                  carrier , NULL , 
                                                  NULL , NULL) ;
        ret_val &=
        rf_hdr_mdsp_tx_closed_loop_clear_override ( logical_dev->tx_mdsp_path , 
                                                    carrier , NULL , 
                                                    NULL, NULL ) ;
      }
      else
      {
        ret_val &=
        rf_hdr_mdsp_tx_closed_loop_clear_override ( logical_dev->tx_mdsp_path , 
                                                    carrier , NULL , 
                                                    NULL , NULL) ;
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfm_hdr_ftm_set_tx_pwr : NULL Device Param Ptr" );
  }
  }
  return ret_val ;
} /* rfm_hdr_ftm_clear_tx_pwr_override */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the Tx Datapath IQ Gain
 
  @details
  This function will override the Tx data IQ gain to adjust the DAC set point
 
  @param device
  RF device on which the Tx IQ gain is to be set.
 
  @param tx_dp_iq_gain
  Tx datapath IQ gain to be programmed on the given device
 
  @return
  Status indicating, if the IQ gain was successfully programmed or not.
*/
boolean
rfm_hdr_ftm_set_tx_iq_gain
(
  rfm_device_enum_type device,
  uint32 tx_dp_iq_gain
)
{
  boolean ret_val = TRUE; /* Return value */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Write Pointer*/
  rfm_cdma_band_class_type band;

  if ( device >= RFM_MAX_DEVICES )
  {
    RF_MSG_1( RF_ERROR,"rfm_hdr_ftm_set_tx_iq_gain: Invalid Device %d",device ); 
    ret_val = FALSE;
  }
  else 
  {
  /* Get the current mode */
  rfm_mode_enum_type curr_mode = rfm_get_current_mode(device);

  /* Check whether mode is valid */
  if ( curr_mode != RFM_1XEVDO_MODE )
  {
    RF_MSG_2( RF_ERROR, "rfm_hdr_ftm_set_tx_iq_gain: Invalid Mode %d on Dev %d",
              curr_mode, device );
      ret_val = FALSE;
  }
  } /*  if !( device >= RFM_MAX_DEVICES ) */

  if ( ret_val == TRUE )
  {
  /* Get Current Device Status for the given Device and check whether the data 
  returned is valid. */
  dev_status_r = rf_cdma_get_device_status( device );

  if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_ftm_set_tx_iq_gain: Invalid Data ptr on "
                        "Dev %d", device );
      ret_val = FALSE;
    }
    else 
    {
      band = dev_status_r->curr_band;
      if ( band >= RFM_CDMA_MAX_BAND )
      {
        RF_MSG_1 ( RF_ERROR, 
            "rfm_hdr_ftm_set_tx_iq_gain: Invalid band %d .. rf_band %d", band );
        ret_val = FALSE;
      }
  }
  } /* if ( ret_val == TRUE ) */

  /* all checks passed */
  if ( ret_val == TRUE )
  {
    const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device*/
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev != NULL )
  {
      uint32 modem_chain = 
        rfc_cdma_get_tx_modem_chain ( device , band )  ;
        ret_val &= 
          rf_cdma_msm_set_txc_gain( modem_chain , 
                   dev_status_r->txlm_handle, tx_dp_iq_gain );

    ret_val &= rf_hdr_mdsp_set_tx_iq_gain( logical_dev->rx_mdsp_path,
                                             dev_status_r->txlm_handle, 
                                             modem_chain , NULL, NULL, NULL );

    RF_MSG_2( RF_HIGH, "rfm_hdr_ftm_set_tx_iq_gain : Dev %d - TXC "
              "Gain 0x%x set", device, tx_dp_iq_gain );

  }
  else
  {
    RF_MSG( RF_ERROR, "rfm_hdr_ftm_set_tx_iq_gain : NULL Device Param Pointer" );
    ret_val = FALSE;
  }

  } /* if ( ret_val == TRUE ) */

  return ret_val;

} /* rfm_hdr_ftm_set_tx_iq_gain */

/*! @} */

/*============================================================================*/
/*!
  @name HDR Data Query Functions

  @brief
  Functions to query HDR Data
*/
/*! @{ */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Current RxAGC Data

  @details
  This function returns the current RxAGC Data
 
  @param device
  The receive device whose RxAGC data is queried
 
  @param rx_agc_data
  Container for current RxAGC data. This contains RxAGC for all carriers
  and LNA state
 
  @return
  Current TxAGC Data on given device
*/
void
rfm_hdr_ftm_get_rx_agc_data
(
  const rfm_device_enum_type device,
  rfm_cdma_ftm_rx_agc_data_type *rx_agc_data,
  const rf_cdma_agc_format_type format
)
{
  rf_hdr_mdsp_rx_agc_read_type rx_agc_read; /* RxAGC Read structure */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Get Device Status for band info */
  const rf_cdma_data_status_type* dev_status_r = 
    rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  
  /* NULL Pointer check */
  if ( ( logical_dev != NULL ) && ( dev_status_r != NULL ) )
  {
    /* Read RxAGC Data from MDSP */
    rf_hdr_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_read );

    /* Get LNA Gain word from RTR */
    rfdevice_cdma_rx_cmd_dispatch( logical_dev->rx_device[dev_status_r->curr_band],
                                   RFDEVICE_GET_LNA_STATE,
                                   &(rx_agc_data->gain_word) );
  
    /* convert RxAGC into format requested */
    rx_agc_data->rx_agc[RFM_CDMA_CARRIER_0] =
         rf_cdma_convert_rxagc(format, rx_agc_read.rx_agc[RFM_CDMA_CARRIER_0]);
    rx_agc_data->rx_agc[RFM_CDMA_CARRIER_1] =
         rf_cdma_convert_rxagc(format, rx_agc_read.rx_agc[RFM_CDMA_CARRIER_1]);
    rx_agc_data->rx_agc[RFM_CDMA_CARRIER_2] =
         rf_cdma_convert_rxagc(format, rx_agc_read.rx_agc[RFM_CDMA_CARRIER_2]);
    rx_agc_data->rx_agc[RFM_CDMA_CARRIER_CUMULATIVE] =
         rf_cdma_convert_rxagc(format, 
                               rx_agc_read.rx_agc[RFM_CDMA_CARRIER_CUMULATIVE]);
           
    /* update the LNA state */
    rx_agc_data->gain_state = rx_agc_read.lna_state;
  }
  else
  {
    rx_agc_data = NULL;
    RF_MSG( RF_ERROR, "rfm_hdr_ftm_get_rx_agc_data : NULL Device Param"
                      "Pointer" );
  }

  return;

} /* rfm_hdr_ftm_get_rx_agc_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Current TxAGC Data

  @details
  This function returns the current TxAGC Data
 
  @param device The receive device whose TxAGC data is queried
 
  @param tx_agc_data Container for current ^TxAGC data
 
  @return
  Current TxAGC Data on given device
*/
void
rfm_hdr_ftm_get_tx_agc_data
(
  const rfm_device_enum_type device,
  const rfm_cdma_carrier_type carrier,
  rfm_cdma_ftm_tx_agc_data_type *tx_agc_data,
  const rf_cdma_agc_format_type format
)
{

  rf_cdma_mdsp_tx_agc_logical_read_type tx_agc_logical; /* TxAGC Data */
  rf_cdma_mdsp_tx_agc_physical_read_type tx_agc_physical; /* TxAGC Data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  /* Linearizer logging data */
  const rfcommon_core_txlin_log_data_type* lin_log_data = NULL;

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  
  /* get read pointer to device - need this to find out current band */
  dev_status_r = rf_cdma_get_device_status( device );

  /* Get the linearizer data logging pointer */
  lin_log_data = rfcommon_core_txlin_get_log_data( RFM_1X_MODE, device );

  /* NULL Pointer check */
  if ( ( logical_dev != NULL ) && ( dev_status_r != NULL ) && 
       ( lin_log_data != NULL ) )
  {
    if ( dev_status_r->data_hdr.rf_state != RF_CDMA_STATE_RXTX )
    {
      RF_MSG_2 ( RF_ERROR ,
                 "rfm_hdr_ftm_get_tx_agc_data: Dev %d , Device State: %d "
                 "Device not in RF_CDMA_STATE_RXTX state" ,
                 device ,  dev_status_r->data_hdr.rf_state ) ;
    }
    else
    {      
      int16 K; /* turn-around const */
      rfm_pa_gain_state_type pa_state = RFM_PA_GAIN_STATE_0;

      /* Get Logical Side Data of TxAGC */
      rf_hdr_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path,
                                           rf_cdma_data_get_txlm_handle (device) ,
                                           carrier, &tx_agc_logical ) ; 
    
      /* Get Logical Side Data of TxAGC */
      rf_hdr_mdsp_get_tx_agc_physical_data( logical_dev->tx_mdsp_path,
                                          rf_cdma_data_get_txlm_handle (device) ,
                                          (rfcom_multi_carrier_id_type)carrier ,
                                          &tx_agc_physical ) ; 

      /* return txAgc in requested dBm format */
      K = rf_cdma_get_turnaround_constant(dev_status_r->curr_band);
      tx_agc_data->open_loop_power = rf_cdma_convert_txagc( format, K, 
                                                 tx_agc_logical.open_loop_power);
      tx_agc_data->closed_loop_power = tx_agc_logical.closed_loop_power;
      tx_agc_data->rate_adjust = tx_agc_logical.rate_adjust;
      tx_agc_data->total_power= tx_agc_logical.total_power;
      tx_agc_data->carrier_total_power= tx_agc_logical.total_carrier_power;
      tx_agc_data->pilot_power = tx_agc_logical.pilot_power;
      tx_agc_data->pa_state = tx_agc_logical.pa_state;
      tx_agc_data->tx_min = tx_agc_logical.tx_min;
      tx_agc_data->tx_max = tx_agc_logical.tx_max;

      /* extract PA state specific data */
      pa_state = (rfm_pa_gain_state_type)tx_agc_logical.pa_state;
      if ( pa_state < RFM_PA_GAIN_STATE_NUM )
      {
        /* Determine PA bias by looking up lin table based on FW RGI index*/
        tx_agc_data->apt_val = 
          lin_log_data->pa_state_data[pa_state].
                        rgi_data[tx_agc_physical.cal_bias_rgi_index].papm_bias;

        /* Determine the TxLUT for the given Tx AVGA index from FW */
        tx_agc_data->avga_index = 
          lin_log_data->pa_state_data[pa_state].
                        rgi_data[tx_agc_physical.full_bias_rgi_index].rgi;

        tx_agc_data->avga_val = tx_agc_physical.tx_avga;
        tx_agc_data->betap_gain = tx_agc_physical.betap;  
        tx_agc_data->xpt_mode_val = tx_agc_physical.xpt_mode;
        tx_agc_data->env_gain_val = tx_agc_physical.env_scale_gain;
        tx_agc_data->iq_gain_val =  tx_agc_physical.iq_gain;
        tx_agc_data->xpt_delay_val = tx_agc_physical.xpt_delay;

        /* Determine the pa icq for the given TX AVGA index from FW*/
        tx_agc_data->pa_q_current = 
          lin_log_data->pa_state_data[pa_state].
                        rgi_data[tx_agc_physical.cal_bias_rgi_index].pa_icq;

        /* Determine Pout compensation value */
        tx_agc_data->pout_comp =  rf_cdma_tx_agc_get_pout_comp_val( 
                                          device,
                                          tx_agc_logical.pa_state,
                                          tx_agc_physical.cal_bias_rgi_index );
        /* Determine Pin compensation value */
        tx_agc_data->pin_comp = rf_cdma_tx_agc_get_pin_comp_val( 
                                          device, tx_agc_logical.pa_state );
        /* Determine DPD index value */
        tx_agc_data->dpd_idx = rf_cdma_tx_agc_get_dpd_index( device,
                                     tx_agc_logical.pa_state,
                                     tx_agc_physical.cal_bias_rgi_index );
      }
      else
      {
        /* Invalid PA state. Set to default 0 */
        tx_agc_data->apt_val = 0;
        tx_agc_data->avga_index = 0;
        tx_agc_data->avga_val = 0;
        tx_agc_data->betap_gain = 0;  
        tx_agc_data->xpt_mode_val = 0;
        tx_agc_data->env_gain_val = 0;
        tx_agc_data->iq_gain_val = 0;
        tx_agc_data->xpt_delay_val = 0;
        tx_agc_data->pa_q_current = 0;
        tx_agc_data->pout_comp = 0;
        tx_agc_data->pin_comp = 0;
        tx_agc_data->dpd_idx = 0;
      }
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfm_hdr_ftm_get_tx_agc_data : NULL Device Param Ptr" );
    tx_agc_data = NULL;
  }

  return;

} /* rfm_hdr_ftm_get_tx_agc_data */

/*! @} */

/*============================================================================*/
/*!
  @name Rx parameter Configuration

  @brief
  Functions used to configure RF parameter in FTM Mode
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets LNA decision.

  @details
  This function allows direct control of the LNA Decision bits in
  the HW LNA state machine. These changes will propogate to the
  LNA RANGE and MIXER RANGE pins if they have not been overriden in
  AGC_CTL2.
  
  @param device
  RF Device under consideration

  @param override_mode
  Flag indicating if the LNA Gain state is to be overriden. If True, then
  LNA State will be overriden to the following state.

  @param state
  Valid Gain States

  @return
  Status of execution completion of the function execution
*/
boolean
rfm_hdr_ftm_set_rx_gain_state
(
  rfm_device_enum_type device,
  boolean override_mode,
  rfm_lna_gain_state_type state
)
{
  /* Get the current mode */
  rfm_mode_enum_type curr_mode = rfm_get_current_mode(device);
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean ret_val = TRUE; /* Return value */

  /* Check whether mode is valid */
  if ( curr_mode != RFM_1XEVDO_MODE )
  {
    RF_MSG_2( RF_ERROR, "Set LNA State : Invalid Mode %d on Dev %d",curr_mode,\
              device );
    return FALSE;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev != NULL )
  {
    /* Based on override_mode value, clear or set the LNA state override */
    if ( override_mode == TRUE )
    {
      ret_val = rf_hdr_mdsp_lna_state_override( logical_dev->rx_mdsp_path, 
                                                state, NULL, NULL, NULL );
    }
    else
    {
      ret_val = rf_hdr_mdsp_lna_state_clear_override( logical_dev->rx_mdsp_path,
                                                      NULL, NULL, NULL );
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfm_hdr_ftm_set_rx_gain_state : NULL Device Param Ptr" );
    ret_val = FALSE;
  }

  return ret_val;
} /* rfm_hdr_ftm_set_rx_gain_state */

/*! @} */

/*============================================================================*/
/*!
  @name Firmware App Control

  @brief
  This section contains functions to control FW App.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable HDR Firmware

  @details
  This function will configure FW App while running under FTM task context by
  calling RF-MDSP Interface.
 
  @param device
  RFM device on which HDR FW state is to be configured
 
  @param fw_state
  The state in which to configure firmware
*/
boolean
rfm_hdr_ftm_config_fw_state
(
  rfm_device_enum_type device,
  rfm_cdma_fw_state_type fw_state
)
{
  rf_hdr_fw_response_data_type *fwrsp_data = NULL;
  boolean ret_val = FALSE;
  rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
  rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

  /* Get FW Response Data */
  fwrsp_hk = rf_cdma_get_fwrsp_data( device );

  /* NULL Pointer Check */
  if ( fwrsp_hk == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_hdr_ftm_config_fw_state: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  /* Get FW Response Data */
  fwrsp_data = rf_cdma_get_hdr_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_hdr_ftm_config_fw_state: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  /* Enable FW Application */
  fwrsp_hk->fws_cfg_rsp.device = device;
  fwrsp_hk->fws_cfg_rsp.sema_token = rfcommon_semaphore_pop_item();
  rsp_cb_param.rsp_handler = rf_hdr_fws_config_rsp_cb;
  rsp_cb_param.handler_data = &fwrsp_hk->fws_cfg_rsp;

  ret_val = rf_hdr_mdsp_ftm_configure_fw_state( fw_state, &rsp_cb_param,
                                                NULL, NULL );
  
  RF_MSG_2( RF_LOW, "[HDR][RF-FW Sync] [D%d] FWS_STATE_CFG : "
            "Start Wait Sem @ 0x%x", device, 
            fwrsp_hk->fws_cfg_rsp.sema_token );
  rfcommon_semaphore_wait( fwrsp_hk->fws_cfg_rsp.sema_token );
  rfcommon_semaphore_push_item( fwrsp_hk->fws_cfg_rsp.sema_token );

  return ret_val;

} /* rfm_hdr_ftm_enable_firmware */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Issue pilot measurement config message to FW

  @details
  This is a wrapper API, which calls the MDSP layer API to send message to FW
  to start pilot measurement

  @param device
  The logical device
 
  @return
  Flag indicating the function execution success
*/
boolean
rfm_hdr_ftm_pilot_meas_cfg
(
  rfm_device_enum_type device
)
{
  boolean ret = TRUE;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev != NULL )
  {
    /* Enable FW Application */
    rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
    rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

    /* Get pointer to FW Response Data */
    fwrsp_hk = rf_cdma_get_fwrsp_data( device );
    /* NULL pointer check */
    if ( fwrsp_hk != NULL )
    {
      fwrsp_hk->pilot_meas_cfg.device = device;
      rsp_cb_param.rsp_handler = rf_hdr_pilot_meas_config_rsp_cb;
      rsp_cb_param.handler_data = &fwrsp_hk->pilot_meas_cfg;
      /* send pilot meas config mssg to FW*/
      ret &= rf_hdr_mdsp_pilot_meas_cfg( logical_dev->rx_mdsp_path, 
                                        &rsp_cb_param,
                                        NULL, NULL );
    }/*if ( fwrsp_hk != NULL )*/
    else
    {
      RF_MSG_1( RF_ERROR, "rfm_hdr_ftm_pilot_meas_cfg: NULL FWRSP"
                "Housekeeping data for device: %d",device );
      ret = FALSE;
    }/*if ( fwrsp_hk == NULL )*/
  }/*if ( logical_dev != NULL )*/
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_ftm_pilot_meas_cfg: NULL data for device:%d",
              device );
    ret = FALSE;
  }

  return ret;
  
  
}/*rfm_hdr_ftm_pilot_meas_cfg*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Issue FLL XO config message to FW

  @details
  This is a wrapper API, which calls the MDSP layer API to send message to FW
  for FLL XO config

  @param device
  The logical device
 
  @return
  Flag indicating the function execution success
*/
boolean
rfm_hdr_ftm_fll_xo_cfg_msg
(
  rfm_device_enum_type device
)
{
  boolean ret = TRUE;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev != NULL )
  {
    /*Call MDSP layer API to issue for FLL XO config message*/
    ret &= rf_hdr_mdsp_fll_xo_cfg_msg( logical_dev->rx_mdsp_path,
                                       NULL, 
                                       NULL); 
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_ftm_fll_xo_cfg_msg: NULL data for device:%d",
              device );
    ret = FALSE;
  }

  return ret;
  
  
}/*rfm_hdr_ftm_fll_xo_cfg_msg*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Issue pilot measurement stop stream message to HDR FW

  @details
  This is a wrapper API, which calls the MDSP layer API to send message to FW
  to stop pilot measurement

  @param device
  The logical device
 
  @return
  Flag indicating the function execution success
*/
boolean
rfm_hdr_ftm_pilot_meas_stop_stream
(
   rfm_device_enum_type device
)
{
  rf_hdr_fw_response_data_type *fwrsp_data = NULL;
  boolean ret = TRUE;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Get FW Response Data */
  fwrsp_data = rf_cdma_get_hdr_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_hdr_ftm_pilot_meas_stop_stream: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  if ( logical_dev != NULL )
  {
    /* Enable FW Application */
    rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
    rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

    /* Get pointer to FW Response Data */
    fwrsp_hk = rf_cdma_get_fwrsp_data( device );

    /* NUll pointer check */
    if ( fwrsp_hk != NULL )
    {
      fwrsp_hk->pilot_meas_stop_stream.sema_token = rfcommon_semaphore_pop_item();
      fwrsp_hk->pilot_meas_stop_stream.device = device;
      rsp_cb_param.rsp_handler = rf_hdr_pilot_meas_stop_stream_rsp_cb;
      rsp_cb_param.handler_data = &fwrsp_hk->pilot_meas_stop_stream;
      /* send pilot meas stop stream mssg to FW*/
      ret &= rf_hdr_mdsp_pilot_meas_stop_stream( logical_dev->rx_mdsp_path, 
                                          &rsp_cb_param,
                                          NULL,
                                          NULL ); 

      RF_MSG_2( RF_LOW, "[HDR][RF-FW Sync] [D%d] PILOT_MEAS_STOP_STREAM :"
                "Start Wait Sem @ 0x%x", device, 
                fwrsp_hk->pilot_meas_stop_stream.sema_token );
      rfcommon_semaphore_wait( fwrsp_hk->pilot_meas_stop_stream.sema_token );
      rfcommon_semaphore_push_item( 
                                  fwrsp_hk->pilot_meas_stop_stream.sema_token );
    }/*if ( fwrsp_hk != NULL )*/
    else
    {
      RF_MSG_1( RF_ERROR, "rfm_hdr_ftm_pilot_meas_stop_stream: NULL FWRSP"
                "Housekeeping data for device: %d",device );
      ret = FALSE;
    }/*if ( fwrsp_hk == NULL )*/
  }/*if ( logical_dev != NULL )*/
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_hdr_ftm_pilot_meas_stop_stream: NULL data for "
                        "device:%d", device );
    ret = FALSE;
  }

  return ret;
  
}/*rfm_hdr_ftm_pilot_meas_stop_stream*/

/*! @} */

/*============================================================================*/
/*!
  @name RFM teardown API

  @brief
  This section contains functions to execute radio teardown.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Teardown HDR radio

  @details
  This function will Tear Down RF to Sleep State, no matter what the current
  state is, using the rf_hdr_mc_tear_down() function.

  @param device
  The device to tear down
 
  @return
  Return if tear down was required on the device or not 
*/
boolean
rfm_hdr_ftm_tear_down_radio
( 
  rfm_device_enum_type device
)
{
  boolean was_tear_down_req = TRUE; /* Flag indicating if tear down is needed */

  was_tear_down_req = rf_hdr_mc_tear_down( device ); 

  RF_MSG_2( RF_HIGH, "rfm_hdr_ftm_tear_down_radio: Dev %d Tear down Status %d",
            device, was_tear_down_req );

  return was_tear_down_req;

} /* rfm_hdr_ftm_tear_down_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register FTM related FW MSG responses with FWRSP Task

  @details
  This function is called in ftm and it calls mdsp layer to register some MSG 
  hdr Message headers to be sent to FW.

  @return
  TRUE if registration is successful
*/
boolean
rfm_hdr_ftm_register_fw_msgs
( 
  void
)
{
  return( rf_hdr_mdsp_register_fw_msgs());
} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-register FTM related FW MSG responses with FWRSP Task

  @details
  This function is called in ftm and it calls mdsp layer to de-register some MSG 
  hdr Message headers to be sent to FW.

  @return
  TRUE if de-registration is successful
*/
boolean
rfm_hdr_ftm_deregister_fw_msgs
( 
  void
)
{
  return( rf_hdr_mdsp_deregister_fw_msgs());
} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Test out Rx on off control API in RFLM layer, call MDSP layer to call the RFLM
  API

  @details
  Rx on/off is called by FW for mili or micro sleep, which is basically a way
  of disabling the rx path moementarily. Differentl levels of sleep modify
  the state of rx path differently, micro sleep only disables the LNA path
  while mili will disable PLL as well

  @param device
  Device on which requested mode needs to be enabled
 
  @param rf_mode
  Requested mode
 
  @return
  Response packet for the command
*/
boolean
rfm_hdr_ftm_test_rx_on_off
( 
  uint32 force_microsleep_enable_disable,
  uint32 rf_off_ustmr_delta,
  uint32 rf_on_ustmr_delta,
  uint32 duration_half_slots,
  uint32 sleep_mode
)
{
  boolean ret_val;

  /* Send Rx on/off test mssg to FW */
  ret_val =  rf_hdr_mdsp_test_rx_on_off( force_microsleep_enable_disable,
                                        rf_off_ustmr_delta,
                                        rf_on_ustmr_delta, 
                                        duration_half_slots, 
                                        sleep_mode,
                                        NULL , NULL );

   return ret_val;
} /* rfm_hdr_ftm_test_rx_on_off */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query line offset of mempool addr for specific device and carrier
 
  @details
  This function shall be used by ftm layer to query line offset of mempool addr
  for a given device and carrier. It's a wrapping api which calls
  rf_hdr_mdsp_query_iq_sample_line_offset in mc layer. Check details of that api
  for more information.
 
  @param carrier
  The Narrowband Carrier path for which the rfm_device_enum_type is
 
  @param device
  RFM Device for which the rfm_device_enum_type is

  @param mempool_addr_line_offset
  mempool addr line offset, will be updated when api is executed successfully
  
  @return
  TRUE - if querying line_offset is successful. FALSE - if querying is unsuccessful.
*/
boolean
rfm_hdr_ftm_query_iq_sample_line_offset
(
  const rfm_cdma_carrier_type carrier,
  const rfm_device_enum_type device,
  uint32 *mempool_addr_line_offset
)
{
  rf_cdma_mdsp_chain_enum_type mdsp_chain;
  boolean api_status = TRUE;;

  if( ( device == RFM_DEVICE_0 ) || ( device == RFM_DEVICE_2 ) )
  {
    mdsp_chain = RF_CDMA_MDSP_CHAIN_0;
  }
  else if( ( device == RFM_DEVICE_1 ) || ( device == RFM_DEVICE_3 ) )
  {
    mdsp_chain = RF_CDMA_MDSP_CHAIN_1;
  }
  else
  {
    api_status = FALSE;
    mempool_addr_line_offset = 0;
    RF_MSG_1(RF_ERROR, "rfm_hdr_ftm_query_iq_sample_line_offset: unsupported"
                        "device %d ",device);
  }

  if ( api_status == TRUE ) 
  {
    api_status = rf_hdr_mdsp_query_iq_sample_line_offset( carrier,mdsp_chain,
                                                    mempool_addr_line_offset);
  }

  return api_status;

}/* rfm_hdr_ftm_query_iq_sample_line_offset */

#endif /* FEATURE_CDMA1X */
/*! @} */
/*! @} */

