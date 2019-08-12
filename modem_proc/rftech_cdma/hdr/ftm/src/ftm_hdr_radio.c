/*!
  @file
  ftm_hdr_radio.c

  @brief
  This module contains FTM HDR Radio Control code which is used to 
  exclusively control Radio for test purposes.
*/

/*==============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/ftm/src/ftm_hdr_radio.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/19/14   spa     Added FTM concurrecny manager check,done in single enable API
07/15/14   pk      Compiler warning fix
07/08/14   pk      Implementation of HDR FTM module
06/27/14   sty     KW fix to prevent array bound violations
09/04/13   APU     Brought over FTM HDR radio Tx control from Dime.
08/20/13   zhw/sty Update FTM HDR radio Tx control interface
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
03/28/13   zhw     Clean up obsoleted PA range in tx_agc_data
02/08/13   sty     KW fix
01/31/13   sty     Do not call HDR enable/disable modulator API if 
                   rfm_hdr_wakeup/sleep_tx() fails
12/17/12   sty     KW fixes
12/16/12   sty     KW fixes
05/17/12   zhw     Removed cdma debug flag: rf_hdr_wait_time_after_tx_start
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/09/12   spa     Changed ftm_cdma_data_update_band_chan arguments to cdma band     
                   and chan type
01/20/12   hdz     Changed interface of rfm_hdr_ftm_set_tx_pwr()
01/03/12   hdz     Force hdr into traffic mode in ftm_hdr_radio_wakeup_tx() 
12/21/11   aro     Added HDR Enable and disable radio functions
12/10/11   sty     Added base_rot_offset to rf_cdma_compute_carrier_info() calls
11/17/11   aro     Enabled FTM callback functions for RFM APIs
11/17/11   aro     Added new MCPM configure function with voting mechanism
11/15/11   cd      Get Tx AGC should return PA state instead of PA range
11/07/11   cd      Update FTM CDMA data in retune function with current band 
                   chan info
09/22/11   cd      Added power mode and jammer state logging in Rx AGC response
                   packet
09/12/11   cd      Updated set Tx power interface
08/24/11   aro     Updated Tx Data query function to have more Tx parameter
08/23/11   sty     Fixed KW error
08/12/11   aro     Added support for HDR LNA override clear
07/18/11   aro     Renamed RESUME agc mode to RESUME_INIT
06/21/11   aro     Moved ftm_sys_control functionality to ftm_common_control
06/17/11   Saul    Now configuring Tx carrier freq offsets and mask for MCDO
06/16/11   sar     Fixed compiler error.
06/16/11   sar     Included internal rf hdr interface.
06/14/11   Saul    Compile error fixes from previous checkin.
06/14/11   Saul    HDR Rx AGC API now returns AGC for all carriers
06/13/11   sty     Added implementation for ftm_hdr_radio_disable_diversity
06/13/11   sty     Changes needed for HDR diversity in FTM mode
06/08/11   aro     Fixed the Tx Sleep to have disable dac followed by Tx stop
06/07/11   sty     Added call to ftm_hdr_disable_modulator() in sleep Tx
06/07/11   aro     Mainlined FEATURE_RF_COMMON_LM_RFM_INTERFACE
06/06/11   sty     Revert state to sleep instead of Invalid - since invalid
                   results in a deadlock condition
05/27/11   sty     Added enter_mode to ftm_hdr_radio_enable_div()
05/17/11   sty     Added wait between Tx_Start and Tx_DAC_Start
05/05/11   sty     Implemeted ftm_hdr_radio_enable_div()
05/05/11   sty     Invoke new APIs in HDR wakeup sequence
                   Added API for enabling HDR diversity
04/25/11   aro     Renamed FTM RFMODE to FTM State
04/21/11   aro     Changed rfm_enter_mode() call to support RxLM Buffer
04/12/11   aro     Renamed Enable/Disable FW Interface
03/24/11   aro     Renamed FTM system control filename
03/24/11   aro     Unified the RxAGC Read Interface
03/24/11   aro     Renamed FTM Enter Mode function 
03/23/11   sty     Fixed warning 
03/22/11   sty     Deleted APIs that enabled and disabled clks - this is taken 
                   care of by MCPM
03/22/11   sty     Disable clks in ftm_hdr_radio_sleep_tx()
                   Deleted call to ftm_hdr_enable_tones() - not nedeed
03/21/11   sty     [1] Update ftm_cdma_data during state transitions 
                       (for ex "Rx to RxTX" etc)
                   [2] Mimic L1 call sequence in ftm_hdr_radio_wakeup_tx
03/14/11   aro     Consolidated RxAGC Data to one data structure
03/07/11   aro     Added support for HDR retune control from FTM
03/03/11   sty     Refactored variables
03/02/11   aro     Renamed FTM and FTM-NS mode enums
02/23/11   sty     Fixed warnings
02/16/11   aro     Added support to get RxLM and TxLM handle from FTM Data str
02/16/11   aro     [1] Added support to do FTM init for Radio Dispatch
                   [2] Added support to send BAD response packet
02/15/11   aro     Added support to pass LM handle to RFM interface
02/14/11   sty     Commented out call to 1x modulator in ftm_hdr_radio_wakeup_tx
02/14/11   aro     Updated HDR RxAGC Wakeup request packet
02/14/11   aro     Removed AGC Mode from Get HDR RxAGC
02/13/11   aro     Added MC support for HDR Wakeup Rx Interface
02/13/11   aro     Added Set Tx Power Interface
02/13/11   aro     [1] Updated LNA query interface
                   [2] Added support for Get TxAGC
02/12/11   aro     [1] Deleted Get LNA state interface to merge with Get RxAGC
                   [2] F3 message update
02/08/11   aro     [1] Added LaTeX for RxAGC formula
                   [2] Created QDART Doxygen group
02/08/11   aro     Documentation Update
02/08/11   aro     Moved HDR Radio Control to FTM
02/04/11   sty     Added support for HDR Rx and Tx sleep
02/03/11   sty     Added support for ftm_hwtc_hdr_radio_wakeup_tx
01/24/11   aro     Added LNA Range Query Interface
01/24/11   aro     Added code to covert Raw HDR RxAGC Value to dBm
01/24/11   aro     Compiler Warning Fix
01/23/11   aro     Added Support for HDR RxAGC Query
01/21/11   aro     Compile Error Fix
01/20/11   aro     Initial Release : Based on SVDO Design

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_cdma_packet_types.h"
#include "ftm_hdr_radio.h"
#include "ftm_cdma_control.h"
#include "ftm_cdma_data.h"
#include "ftm_hdr_control.h"
#include "rfm_hdr.h"
#include "rfm_common.h"
#include "rfm_hdr_ftm.h"
#include "ftm_msg.h"
#include "ftm_common_control.h"
#include "ftm_hdr_msm.h"
#include "rfm_internal.h"
#include "DALSys.h" /* remove when rf_cdma_debug_flags is deleted */
#include "rf_cdma_data.h"
#include "rf_cdma_utils.h"
#include "rfm_hdr_ftm.h"
#include "rfm_cdma_ftm.h"
#include "rfm_cdma_band_types.h"
#include "rf_cdma_utils_freq.h"

/*============================================================================*/
/*!
  @addtogroup RF_FTM_HDR_QDART
  @image html ftm_hwtc_cdma_rsp_pkt_structure.jpg
  @image html ftm_hwtc_cdma_req_pkt_structure.jpg
  @{
*/

/*============================================================================*/
/*!
  @name HDR Radio Control Request Packets

  @brief
  This block includes all the Request Packet definitions to be used to send
  FTM commands related to CDMA HDR Radio Control
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_ENTER Command
*/
typedef ftm_cdma_req_7_type ftm_hdr_radio_enter_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_ENABLE Command
*/
typedef ftm_cdma_req_2_type ftm_hdr_radio_enable_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_DISABLE Command
*/
typedef ftm_cdma_req_7_type ftm_hdr_radio_disable_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_WAKEUP_RX Command
*/
typedef ftm_cdma_req_6_type ftm_hdr_radio_wakeup_rx_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_WAKEUP_TX Command
*/
typedef ftm_cdma_req_7_type ftm_hdr_radio_wakeup_tx_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_SLEEP_RX Command
*/
typedef ftm_cdma_req_7_type ftm_hdr_radio_sleep_rx_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_SLEEP_TX Command
*/
typedef ftm_cdma_req_7_type ftm_hdr_radio_sleep_tx_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_RETUNE Command
*/
typedef ftm_cdma_req_8_type ftm_hdr_radio_retune_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_ENABLE_DIVERSITY Command
*/
typedef ftm_cdma_req_9_type ftm_hdr_radio_enable_diversity_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_DISABLE_DIVERSITY Command
*/
typedef ftm_cdma_req_7_type ftm_hdr_radio_disable_diversity_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_SET_TX_POWER Command
*/
typedef ftm_cdma_req_5_type ftm_hdr_radio_set_tx_power_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_CLEAR_LNA_OVERRIDE Command
*/
typedef ftm_cdma_req_1_type ftm_hdr_radio_clear_lna_override_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_GET_RX_AGC Command
*/
typedef ftm_cdma_req_4_type ftm_hdr_radio_get_rx_agc_req_type;

/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_GET_TX_AGC Command
*/
typedef ftm_cdma_req_1_type ftm_hdr_radio_get_tx_agc_req_type;

/*! \} */

/*============================================================================*/
/*!
  @name HDR Radio Control Response Packets

  @brief
  This block includes all the Response Packet definitions to be used to receive
  reponses for FTM commands related to CDMA HDR Radio Control
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_HDR_RADIO_ENTER Command
*/
typedef ftm_cdma_rsp_1_type ftm_hdr_radio_enter_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_HDR_RADIO_ENABLE Command
*/
typedef ftm_cdma_rsp_1_type ftm_hdr_radio_enable_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_HDR_RADIO_DISABLE Command
*/
typedef ftm_cdma_rsp_1_type ftm_hdr_radio_disable_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_HDR_RADIO_SLEEP_RX Command
*/
typedef ftm_cdma_rsp_1_type ftm_hdr_radio_sleep_rx_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_HDR_RADIO_WAKEUP_RX Command
*/
typedef ftm_cdma_rsp_1_type ftm_hdr_radio_wakeup_rx_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_HDR_RADIO_RETUNE Command
*/
typedef ftm_cdma_rsp_1_type ftm_hdr_radio_retune_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_HDR_RADIO_ENABLE_DIVERSITY Command
*/
typedef ftm_cdma_rsp_1_type ftm_hdr_radio_enable_diversity_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_HDR_RADIO_DISABLE_DIVERSITY Command
*/
typedef ftm_cdma_rsp_1_type ftm_hdr_radio_disable_diversity_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_GET_RX_AGC Command
*/
typedef ftm_cdma_rsp_4_type ftm_hdr_radio_get_rx_agc_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Request Packet for #FTM_HDR_RADIO_GET_TX_AGC Command
*/
typedef ftm_cdma_rsp_3_type ftm_hdr_radio_get_tx_agc_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_HDR_RADIO_WAKEUP_TX Command
*/
typedef ftm_cdma_rsp_1_type ftm_hdr_radio_wakeup_tx_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_HDR_RADIO_SLEEP_TX Command
*/
typedef ftm_cdma_rsp_1_type ftm_hdr_radio_sleep_tx_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_HDR_RADIO_SET_TX_POWER Command
*/
typedef ftm_cdma_rsp_1_type ftm_hdr_radio_set_tx_power_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch Response Packet for #FTM_HDR_RADIO_CLEAR_LNA_OVERRIDE Command
*/
typedef ftm_cdma_rsp_1_type ftm_hdr_radio_clear_lna_override_rsp_type;

/*! \} */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_FTM_HDR_RADIO
  @{
*/

/*============================================================================*/
/*!
  @name CDMA HDR Radio Control Functions

  @brief
  Functions for FTM to control the radio in HDR Mode of Operation
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn on the radio and tune to the given band and channel on HDR Mode.

  @details
  This function calls RFM API to turn on the radio and tune to given
  band and channel in HDR Mode.

  @param *ftm_req_data FTM Request Data
*/
ftm_rsp_pkt_type
ftm_hdr_radio_enter
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_enter_req_type* header = 
    (ftm_hdr_radio_enter_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_hdr_radio_enter_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_enter_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES  )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_radio_enter: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  rf_status = ftm_hdr_enter_mode(device, RFM_1XEVDO_MODE, NULL, NULL);

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_hdr_radio_enter_rsp_type*)ftm_rsp_data.pkt_payload;

  if ( cdma_rsp != NULL )                                                       /* what is the use of this response packet */
  {
    /* Update the Response Packet Field */
    cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_radio_enter : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Send back a BAD response packet for failures */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  FTM_MSG_1( FTM_LOW, "ftm_hdr_radio_enter : Dev %d : [Done]",  device );
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_enter */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable HDR radio

  @details
  Prepares the radio for HDR operation on an initial band and channel.
  The HDR subsystem is initialized in the Sleep state with the requested
  band and channel.

  @param *ftm_req_data
  FTM Request Data
 
  @return
  Diag Response packet
*/
ftm_rsp_pkt_type
ftm_hdr_radio_enable
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_enable_req_type* header = 
    (ftm_hdr_radio_enable_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_hdr_radio_enable_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  int32 rf_status = 0; /* Response Variable*/
  rfm_device_enum_type device; /* hols the master and slave device */  
  sys_channel_type band_chan; /* holds band_chan of master device */
  lm_handle_type rxlm_handle;
  ftm_hdr_rfm_callback_data_type cb_payload;
  ftm_cdma_data_status_type dev_status;
  boolean ret_val;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_enable_rsp_type) ); 

  /* Extract master and slave devices from input data capsule */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES  )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_radio_enable: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  band_chan.band = (sys_band_class_e_type)header->band;
  band_chan.chan_num = (sys_channel_num_type)header->chan;
  
  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );
  rxlm_handle = dev_status.rxlm_handle;

  /* Call RFM Functions to Wakeup Rx */
  cb_payload.device = device; /* Populate the CB Payload */

  ret_val = ftm_cdma_control_enable_tech( RFM_1XEVDO_MODE,
                                        device, 
                                        band_chan,
                                        rxlm_handle, 
                                        &ftm_hdr_rfm_callback, &cb_payload );

  /* Return failure if enable failed */
  if ( ret_val == FALSE )
  {
    rf_status = RFM_CDMA_ERROR_FAILURE;
  }

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_hdr_radio_enable_rsp_type*)ftm_rsp_data.pkt_payload;
  
  if ( cdma_rsp != NULL )
  {
    /* Update the Response Packet Field */
    cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_radio_enable : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  
  /* Send back a BAD response packet for failures */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_2( FTM_ERROR, "ftm_hdr_radio_enable : Failed enable HDR on" 
               "device %d : Status =%d", device, ftm_rsp_data.cmd );

  } /* if ( rf_status < 0 ) */

  FTM_MSG_1( FTM_LOW, "ftm_hdr_radio_enable : Dev %d : [Done]", 
             device );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_enable */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable HDR radio

  @details
  Deactivates the HDR portion of the CDMA RF driver.  This API should
  only be called if HDR is in the Sleep state.  If the radio was
  operating in wideband mode, it is reconfigured for narrowband mode.

  @param *ftm_req_data
  FTM Request Data
 
  @return
  Diag Response packet
*/
ftm_rsp_pkt_type
ftm_hdr_radio_disable
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_disable_req_type* header = 
    (ftm_hdr_radio_disable_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_hdr_radio_disable_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  int32 rf_status = 0; /* Response Variable*/
  rfm_device_enum_type device; /* hols the master and slave device */  
  lm_handle_type rxlm_handle;
  ftm_hdr_rfm_callback_data_type cb_payload;
  ftm_cdma_data_status_type dev_status;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_disable_rsp_type) ); 

  /* Extract master and slave devices from input data capsule */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES  )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_radio_disable: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );
  rxlm_handle = dev_status.rxlm_handle;

  /* Call RFM Functions to Wakeup Rx */
  cb_payload.device = device; /* Populate the CB Payload */
  rf_status = rfm_hdr_disable( device, rxlm_handle, &ftm_hdr_rfm_callback, 
                               &cb_payload );
  
  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_hdr_radio_disable_rsp_type*)ftm_rsp_data.pkt_payload;
  
  if ( cdma_rsp != NULL )
  {
    /* Update the Response Packet Field */
    cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_radio_disable : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  
  /* Send back a BAD response packet for failures */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_2( FTM_ERROR, "ftm_hdr_radio_disable : Failed enable HDR on" 
               "device %d : Status =%d", device, ftm_rsp_data.cmd );

  } /* if ( rf_status < 0 ) */

  FTM_MSG_1( FTM_LOW, "ftm_hdr_radio_disable : Dev %d : [Done]", 
             device );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_disable */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn on the radio and tune to the given band and channel on HDR Mode.

  @details
  This function calls RFM API to turn on the radio and tune to given
  band and channel in HDR Mode.

  @param *ftm_req_data FTM Request Data
*/
ftm_rsp_pkt_type
ftm_hdr_radio_wakeup_rx
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_wakeup_rx_req_type* header = \
    (ftm_hdr_radio_wakeup_rx_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_hdr_radio_wakeup_rx_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  uint8 band_chan_num;
  sys_channel_type band_chan[RFM_CDMA_MAX_CARRIER];
  int32 carrier_index[RFM_CDMA_MAX_CARRIER];
  rfm_hdr_agc_acq_type agc_info;
  lm_handle_type rxlm_handle;
  ftm_cdma_data_status_type dev_status;
  ftm_hdr_rfm_callback_data_type cb_payload;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_wakeup_rx_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  band_chan_num = (uint8)header->band_chan_num;
  band_chan[0].band = (sys_band_class_e_type)header->band0;
  band_chan[0].chan_num = (sys_channel_num_type)header->chan0;
  band_chan[1].band = (sys_band_class_e_type)header->band1;
  band_chan[1].chan_num = (sys_channel_num_type)header->chan1;
  band_chan[2].band = (sys_band_class_e_type)header->band2;
  band_chan[2].chan_num = (sys_channel_num_type)header->chan2;
  carrier_index[0] = (int32)header->carrier_idx0;
  carrier_index[1] = (int32)header->carrier_idx1;
  carrier_index[2] = (int32)header->carrier_idx2;

  agc_info.mode = (rfm_hdr_agc_mode_type)header->agc_mode;
  if ( agc_info.mode == RFM_HDR_AGC_MODE_FAST )
  {
    agc_info.params.fast_acq.agc_rtc_offset = header->rtc_offset;
  }
  else if ( agc_info.mode == RFM_HDR_AGC_MODE_RESUME_WITH_NEW )
  {
    agc_info.params.resume_acq.agc_rtc_offset = header->rtc_offset;
    agc_info.params.resume_acq.prev_agc = header->prev_agc;
  }

  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );
  rxlm_handle = dev_status.rxlm_handle;
  
  /* Call RFM Functions to Wakeup Rx */
  cb_payload.device = device; /* Populate the CB Payload */
  rf_status = ftm_hdr_wakeup_rx( device, rxlm_handle, band_chan_num, 
  	                             band_chan, carrier_index, &agc_info, 
                                       &ftm_hdr_rfm_callback, &cb_payload);

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_hdr_radio_wakeup_rx_rsp_type*)ftm_rsp_data.pkt_payload;

  if ( cdma_rsp != NULL )
  {
    /* Update the Response Packet Field */
    cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_radio_wakeup_rx : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Send back a BAD response packet for failures */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  FTM_MSG_2( FTM_LOW, "ftm_hdr_radio_wakeup_rx : Dev %d, RxLM %d : [Done]", 
             device, rxlm_handle );
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_wakeup_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable diversity for HDR 

  @details
  This function calls the RFM API responsible for enabling diversity. 

  @param *ftm_req_data FTM Request Data
*/
ftm_rsp_pkt_type
ftm_hdr_radio_enable_div
(
  void *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_enable_diversity_req_type* header = 
    (ftm_hdr_radio_enable_diversity_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_hdr_radio_wakeup_rx_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  /* Response Variable*/
  int32 rf_status = 0;

  /* hols the master and slave device */  
  rfm_device_enum_type device_m, device_s;

  ftm_hdr_rfm_callback_data_type cb_payload;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_enable_diversity_rsp_type) ); 

  /* Extract master and slave devices from input data capsule */
  device_m = (rfm_device_enum_type)header->device_1;
  device_s = (rfm_device_enum_type)header->device_2;

   rf_status = ftm_hdr_enable_diversity( device_m, 
                                          device_s, 
                                          &ftm_hdr_rfm_callback, &cb_payload );
    
    /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
    cdma_rsp = (ftm_hdr_radio_wakeup_rx_rsp_type*)ftm_rsp_data.pkt_payload;
    
    if ( cdma_rsp != NULL )
    {
      /* Update the Response Packet Field */
      cdma_rsp->rf_status = rf_status;
    }
    else
    {
      FTM_MSG( FTM_ERROR, "ftm_hdr_radio_enable_div : NULL cdma_rsp" );
      ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    }
    
    /* Send back a BAD response packet for failures */
    if ( rf_status < 0 )
    {
      ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  FTM_MSG_1( FTM_LOW, "ftm_hdr_radio_enable_div : Dev %d : [Done]", 
             device_s);

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_enable_div */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable receive diversity for given Master device.

  @details
  This function calls RFM API to disable the given slave device as a receive
  diversity chain for the already tuned master device.

  @param *ftm_req_data
  FTM Request Data

  @return
  Response packet for the disable diversity action
*/
ftm_rsp_pkt_type
ftm_hdr_radio_disable_div
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_disable_diversity_req_type* header = 
    (ftm_hdr_radio_disable_diversity_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_hdr_radio_disable_diversity_rsp_type *cdma_rsp; /*Pkt Payload */

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device_s;

  ftm_hdr_rfm_callback_data_type cb_payload;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_disable_diversity_rsp_type) ); 

  /* Extract Useful Data */
  device_s = (rfm_device_enum_type)header->device;
  if ( device_s >= RFM_MAX_DEVICES  )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_radio_disable_div: Unsupported device %d", 
               device_s );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* Call RFM Function to Wakeup Rx */
  cb_payload.device = device_s; /* Populate the CB Payload */
  rf_status = ftm_hdr_disable_diversity( device_s, &ftm_hdr_rfm_callback, &cb_payload );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_hdr_radio_disable_diversity_rsp_type*)ftm_rsp_data.pkt_payload;

  if ( cdma_rsp != NULL )
  {
    /* Update the Response Packet Field */
    cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_radio_disable_div : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_2( FTM_ERROR, "ftm_hdr_radio_disable_diversity: Failed on "
               "DevS %d : status %d", device_s,  
               rf_status );
  }
  
  FTM_MSG_1( FTM_LOW, "ftm_hdr_radio_disable_diversity: "
             "DevS %d: [Done]", device_s );
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_disable_diversity */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn OFF the radio and put baseband to sleep for HDR Mode

  @details
  This function calls RFM API which in turn puts the RTR and MSM to sleep for 
  the given device

  @param *ftm_req_data FTM Request Data
*/
ftm_rsp_pkt_type
ftm_hdr_radio_sleep_rx
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_sleep_rx_req_type* header = \
    (ftm_hdr_radio_sleep_rx_req_type*)ftm_req_data;
  
  /* Response Packets */
  /* Packet returned to Diag */
  ftm_rsp_pkt_type ftm_rsp_data; 
  /*Packet Payload for each cmd*/
  ftm_hdr_radio_sleep_rx_rsp_type *cdma_rsp; 

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  lm_handle_type rxlm_handle;
  ftm_cdma_data_status_type dev_status;
  ftm_hdr_rfm_callback_data_type cb_payload;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_sleep_rx_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES  )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_hdr_radio_sleep_rx: Unsupported device %d", 
              device);
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );                                 /* common API */
  rxlm_handle = dev_status.rxlm_handle;
  
  /* Call RFM Function to Wakeup Rx */
  cb_payload.device = device; /* Populate the CB Payload */
  rf_status = rfm_hdr_sleep_rx( device, rxlm_handle, &ftm_hdr_rfm_callback,
                                &cb_payload );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_hdr_radio_sleep_rx_rsp_type*)ftm_rsp_data.pkt_payload;

  if ( cdma_rsp != NULL )
  {
    /* Update the Response Packet Field */
    cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_radio_sleep_rx : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Send back a BAD response packet for failures */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  else
  {
    /* Update FTM Data Structure with current Tx state */
    ftm_cdma_data_update_rf_state ( device, FTM_CDMA_STATE_SLEEP );                       /* common API ends */
  }

  FTM_MSG_2( FTM_LOW, "ftm_hdr_radio_sleep_rx : Dev %d, RxLM %d : [Done]", \
             device, rxlm_handle );
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_sleep_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retune the radio to the given band and channel on HDR Mode.

  @details
  This function calls RFM API to retune the radio to the given band and channel
  on HDR Mode.

  @param *ftm_req_data FTM Request Data
*/
ftm_rsp_pkt_type
ftm_hdr_radio_retune
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_retune_req_type* header = \
    (ftm_hdr_radio_retune_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_hdr_radio_retune_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  uint8 band_chan_num;
  sys_channel_type band_chan[RFM_CDMA_MAX_CARRIER];
  int32 carrier_index[RFM_CDMA_MAX_CARRIER];
  lm_handle_type rxlm_handle;
  ftm_cdma_data_status_type dev_status;
  ftm_hdr_rfm_callback_data_type cb_payload;
  uint8 carrier_idx; /* Variable for carrier indexing */
  uint8 mask; /* Contains carrier mask for mdsp config */
  /* Device Status Read Pointer */
  const rf_cdma_data_status_type *dev_status_r; 
  rfm_cdma_carrier_en_mask_type carrier_mask;
  /* Contains frequency offsets */
  int32 freq_offset_hz[RFM_CDMA_CARRIER_NUM];

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_retune_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES  )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_hdr_radio_retune: Unsupported device %d", device);
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  dev_status_r = rf_cdma_get_device_status( device );

  /* NULL pointer check*/
  if(  dev_status_r == NULL )
  {
    FTM_MSG_1( FTM_ERROR,"ftm_hdr_radio_wakeup_tx: NULL data for device %d",
               device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  } 

  band_chan_num = (uint8)header->band_chan_num;
  band_chan[0].band = (sys_band_class_e_type)header->band0;
  band_chan[0].chan_num = (sys_channel_num_type)header->chan0;
  band_chan[1].band = (sys_band_class_e_type)header->band1;
  band_chan[1].chan_num = (sys_channel_num_type)header->chan1;
  band_chan[2].band = (sys_band_class_e_type)header->band2;
  band_chan[2].chan_num = (sys_channel_num_type)header->chan2;
  carrier_index[0] = (int32)header->carrier_idx0;
  carrier_index[1] = (int32)header->carrier_idx1;
  carrier_index[2] = (int32)header->carrier_idx2;

  if ( band_chan_num > RFM_CDMA_MAX_CARRIER )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_hdr_radio_retune: "
                         "num_band_chan = %d is not supported", band_chan_num );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );
  rxlm_handle = dev_status.rxlm_handle;

  rf_cdma_compute_carrier_info( 
                                dev_status_r->curr_band,
                                dev_status_r->curr_chans,
                                dev_status_r->num_carriers, 
                                dev_status_r->carrier_index, 
                                &carrier_mask,
                                freq_offset_hz, 
                                0 /* no need to add anything to calculated
                                     freq offset */
                              );

  mask = 0;
  for( carrier_idx = 0; carrier_idx < dev_status_r->num_carriers; 
       ++carrier_idx ) 
  {
    mask |= (carrier_mask.enable[0] << carrier_idx);
  }

  if( mask == 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, 
             "ftm_hdr_radio_wakeup_tx: Failed ..carrier mask is zero");
  }

  if ( dev_status.rf_state == FTM_CDMA_STATE_RXTX )
  {
    (void) ftm_hdr_enable_modulator( mask, TRUE /* skip_dac_toggle */ );
  }

  /* Call RFM Function to Wakeup Rx */
  cb_payload.device = device; /* Populate the CB Payload */
  rf_status = rfm_hdr_retune( device, rxlm_handle, band_chan_num, band_chan,
                              carrier_index, &ftm_hdr_rfm_callback, 
                              &cb_payload );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_hdr_radio_retune_rsp_type*)ftm_rsp_data.pkt_payload;

  if ( cdma_rsp != NULL )
  {
    /* Update the Response Packet Field */
    cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_radio_retune : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status < 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  else
  {
    /* If the retune was successful, update the FTM data with new
    band-chan info. Currently stores information only for carrier
    index 0 */
    ftm_cdma_data_update_band_chan( device, 
                       rf_cdma_sys_band_type_to_rf_band_type(band_chan[0].band),
                       (rfm_cdma_chan_type )band_chan[0].chan_num );  
  }
  
  FTM_MSG_2( FTM_LOW, "ftm_hdr_radio_retune: Dev %d, RxLM %d : [Done]", 
             device, rxlm_handle );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_retune */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable Tx for HDR

  @details
  This function calls the RFM API to enable Tx and also configures the HDR 
  modulator

  @param *ftm_req_data FTM Request Data
*/
ftm_rsp_pkt_type
ftm_hdr_radio_wakeup_tx
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_wakeup_tx_req_type* header = \
    (ftm_hdr_radio_wakeup_tx_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */

  /* Define : Response Variable*/
  int32 rf_status;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;

  ftm_hdr_rfm_callback_data_type cb_payload;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_wakeup_tx_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
    cb_payload.device = device; /* Populate the CB Payload */
      
  rf_status = ftm_hdr_wakeup_tx (device, &ftm_hdr_rfm_callback, &cb_payload	);
        
        /* If parms were good but rfm API call failed, send back a BAD
           response packet */
        if ( rf_status < 0 )
        {
          ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
        }
      
 FTM_MSG_1(FTM_LOW, "ftm_hdr_radio_wakeup_tx : Dev %d : [Done]", 
            device );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_wakeup_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn OFF the radio and put baseband to sleep for HDR Mode

  @details
  This function calls RFM API which in turn puts the RTR and MSM to sleep for 
  the given device

  @param *ftm_req_data FTM Request Data
*/
ftm_rsp_pkt_type
ftm_hdr_radio_sleep_tx
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_sleep_tx_req_type* header = \
    (ftm_hdr_radio_sleep_tx_req_type*)ftm_req_data;
  
  /* Response Packets */
  /* Packet returned to Diag */
  ftm_rsp_pkt_type ftm_rsp_data; 

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;

  ftm_hdr_rfm_callback_data_type cb_payload;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_sleep_tx_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES  )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_radio_sleep_tx: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

      /* Call RFM Function to Wakeup Rx. Make sure that DAC is disabled before
      TX_STOP is done. */
      cb_payload.device = device; /* Populate the CB Payload */
    
  rf_status = ftm_hdr_sleep_tx (device, &ftm_hdr_rfm_callback, &cb_payload	);

 /* If parms were good but rfm API call failed, send back a BAD
 response packet */
      if ( rf_status < 0 )
      {
        ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
      }

 FTM_MSG_1(FTM_LOW, "ftm_hdr_radio_wakeup_tx : Dev %d : [Done]", 
            device );
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_sleep_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the Tx Power

  @details
  This function sets Open Loop or Closed Loop Transmit Power Level depending 
  on the parameter passed. This API takes OL or CL Tx Power in dBm.

  @param *ftm_req_data FTM Request Data
 
  @return
  Response Packet for the command
*/
ftm_rsp_pkt_type
ftm_hdr_radio_set_tx_power
(
  void  *ftm_req_data
)
{
  ftm_cdma_data_status_type dev_status;  /* points to ftm device status */

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_hdr_radio_set_tx_power_rsp_type *cdma_rsp; /*Packet Payload for each cmd*/

  /* Request Packet */
  ftm_hdr_radio_set_tx_power_req_type* header = \
    (ftm_hdr_radio_set_tx_power_req_type*)ftm_req_data;

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  rfm_cdma_tx_pwr_mode_type tx_pwr_mode;
  int16 tx_pwr;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_set_tx_power_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;  
  if ( device >= RFM_MAX_DEVICES  )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_radio_set_tx_power: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  tx_pwr_mode = (rfm_cdma_tx_pwr_mode_type)header->tx_pwr_mode;
  tx_pwr = (int16)header->tx_pwr;
    
  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );

  /* Call RFM Function to set Tx power */
  rf_status = rfm_hdr_ftm_set_tx_pwr( device, RFM_CDMA_CARRIER_0, 
                                      tx_pwr_mode, tx_pwr
                                    );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_hdr_radio_set_tx_power_rsp_type*)ftm_rsp_data.pkt_payload;

  if ( cdma_rsp != NULL )
  {
    /* Update the Response Packet Field */
    cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_radio_set_tx_power : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Send back a BAD response packet for failures */
  if ( rf_status == TRUE )
  {
    FTM_MSG_3( FTM_LOW, "ftm_hdr_radio_set_tx_power: Dev %d, TxMode %d, "\
               "TxPwr %d : [Done]", device, tx_pwr_mode, tx_pwr );
  }
  else
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_3( FTM_ERROR, "ftm_hdr_radio_set_tx_power: Dev %d, TxMode %d, "\
               "TxPwr %d : [Fail]", device, tx_pwr_mode, tx_pwr );
  }
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_set_tx_power */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Clear LNA State Override

  @details
  This function clears the LNA state machine from override Mode.

  @param *ftm_req_data FTM Request Data
 
  @return
  Response Packet for the command
*/
ftm_rsp_pkt_type
ftm_hdr_radio_clear_lna_override
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_clear_lna_override_req_type* header = \
    (ftm_hdr_radio_clear_lna_override_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_hdr_radio_clear_lna_override_rsp_type *cdma_rsp; /*Packet Payload*/

  /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                           sizeof(ftm_hdr_radio_clear_lna_override_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
    
  /* Call RFM Function to set Tx power */
  rf_status = rfm_hdr_ftm_set_rx_gain_state( device, FALSE, 0 );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_hdr_radio_clear_lna_override_rsp_type*)ftm_rsp_data.pkt_payload;

  if ( cdma_rsp != NULL )
  {
    /* Update the Response Packet Field */
    cdma_rsp->rf_status = rf_status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_radio_clear_lna_override : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( rf_status == 0 )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  FTM_MSG_1( FTM_LOW, "ftm_hdr_radio_clear_lna_override: Dev %d [Done]",device );

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_clear_lna_override */

/*! \} */

/*============================================================================*/
/*!
  @name CDMA HDR Radio Query Functions

  @brief
  Functions for FTM to query HDR Data
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get HDR RxAGC Data

  @details
  This function calls RFM API to query the current RxAGC on given Device.
  Following formula is used to calculated the HDR RxAGC in dBm units.
 
  \f[
    RxAGC_{dBm} = -64 + \frac{RxAGC_{raw} * 102}{65536}
  \f]
 
  Beside RxAGC, this function also return the current LNA state and current
  LNA gain word that is programmed to RTR.
 
  @param *ftm_req_data FTM Request Data
 
  @todo RxAGC Query : Formula Should be re-implemented for nikeL since RxAGC
  units may be different as HDR RxAGC has moved to FW in nikeL
*/
ftm_rsp_pkt_type
ftm_hdr_radio_get_rx_agc
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_get_rx_agc_req_type* header = \
    (ftm_hdr_radio_get_rx_agc_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_hdr_radio_get_rx_agc_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  /* Define : Response Variable*/
  rfm_cdma_ftm_rx_agc_data_type rx_agc_data;
  rfm_cdma_power_mode_type power_mode;
  rfm_cdma_power_mode_type jammer_state;

  /* Define : To Extract Useful Data */
  rfm_device_enum_type device;
  
  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_get_rx_agc_rsp_type) ); 

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_hdr_radio_get_rx_agc_rsp_type*)ftm_rsp_data.pkt_payload;
  if ( cdma_rsp != NULL )
  {
    /* Extract Useful Data */
    device = (rfm_device_enum_type)header->device;
    
    /* Call RFM Function to query LNA Data */
    rfm_hdr_ftm_get_rx_agc_data( device, &rx_agc_data,
                                 RFM_CDMA_AGC_FORMAT__DBM10 );
    /* Get Linearity Info */
    power_mode = rfm_cdma_ftm_get_power_mode(device);
    jammer_state = rfm_cdma_ftm_get_jammer_state( device );

    /* Update the Response Packet Fields */

    /* Convert the agc_value to dBm :
    --------------------------------*/
    cdma_rsp->rx_agc_c0  = rx_agc_data.rx_agc[0];
    cdma_rsp->rx_agc_c1  = rx_agc_data.rx_agc[1];
    cdma_rsp->rx_agc_c2  = rx_agc_data.rx_agc[2];
    cdma_rsp->rx_agc_all = rx_agc_data.rx_agc[3];
    
    cdma_rsp->lna_state = (uint8)rx_agc_data.gain_state;
    cdma_rsp->lna_gain_word = (uint8)rx_agc_data.gain_word;
    cdma_rsp->ic_state = (uint8)power_mode;
    cdma_rsp->jammer_state = (uint8)jammer_state;

    FTM_MSG_9( FTM_LOW, "ftm_hdr_radio_get_rx_agc: Dev %d, RxAGC0 %d, "\
               "RxAGC1 %d, RxAGC2 %d, RxAGCAll %d, LNA %d, LNA Word %d, "
               "PM %d, Jammer Det %d",
               device, cdma_rsp->rx_agc_c0, cdma_rsp->rx_agc_c1,
               cdma_rsp->rx_agc_c2, cdma_rsp->rx_agc_all,
               rx_agc_data.gain_state, rx_agc_data.gain_word, 
               power_mode, jammer_state );
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_radio_get_rx_agc : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_get_rx_agc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get HDR TxAGC Data

  @details
  This function calls RFM API to query the current TxAGC data on given Device.
 
  @param *ftm_req_data FTM Request Data
 
  @return
  Response packet for the command
*/
ftm_rsp_pkt_type
ftm_hdr_radio_get_tx_agc
(
  void  *ftm_req_data
)
{
  /* Request Packet */
  ftm_hdr_radio_get_tx_agc_req_type* header = \
    (ftm_hdr_radio_get_tx_agc_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_hdr_radio_get_tx_agc_rsp_type *cdma_rsp; /* Packet Payload for each cmd */

  /* Define : Response Variable*/
  rfm_cdma_ftm_tx_agc_data_type agc_data;

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  ftm_cdma_data_status_type dev_status;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                             sizeof(ftm_hdr_radio_get_tx_agc_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  if ( device >= RFM_MAX_DEVICES  )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_radio_get_tx_agc: Unsupported device %d", device );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* Get RF FTM Device Status */
  ftm_cdma_data_get_device_status( device, &dev_status );

  /* Call RFM Function to query TxAGC Data */
  rfm_hdr_ftm_get_tx_agc_data( device, RFM_CDMA_CARRIER_0, &agc_data,
                               RFM_CDMA_AGC_FORMAT__DBM10 );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cdma_rsp = (ftm_hdr_radio_get_tx_agc_rsp_type*)ftm_rsp_data.pkt_payload;
  if ( cdma_rsp != NULL )
  {
    /* Update the Response Packet Field */
    cdma_rsp->avga_index = agc_data.avga_index;
    cdma_rsp->avga_val = agc_data.avga_val;
    cdma_rsp->betap_gain = agc_data.betap_gain;
    cdma_rsp->pa_state = agc_data.pa_state;
    cdma_rsp->apt_val = agc_data.apt_val;
    cdma_rsp->txagc_dBm10 = agc_data.open_loop_power;

    FTM_MSG_7( FTM_LOW, "ftm_hdr_radio_get_tx_agc : Dev %d AVGA idx %d, " 
               "AVGA Val 0x%x, DVGA Val %d, PA State %d, Apt Val %u "
               "TxPwr(dBm10) %d", device, agc_data.avga_index, 
               agc_data.avga_val, agc_data.betap_gain, 
               agc_data.pa_state, agc_data.apt_val, agc_data.open_loop_power );
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_radio_get_tx_agc : NULL cdma_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_radio_get_tx_agc */

/*! \} */

#endif /*FEATURE_FACTORY_TESTMODE */
#endif /* FEATURE_CDMA1X */
/*! @} */

