#ifndef FTM_CDMA_PACKET_TYPES_H
#define FTM_CDMA_PACKET_TYPES_H

/*!
  @file
  ftm_cdma_packet_types.h

  @brief
  This module contains defintions of all Request and Response Packets
  used between Diag and FTM.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/inc/ftm_cdma_packet_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/08/14   JJ      Add a new FTM command for timed sleep rx
03/20/14   spa     Added packet for forced Rx on/off support
12/04/12   zhw     APT PA Q Current support (interface cleanup)
01/12/12   aro     Converted Tx flag and Rx flag to device flag
11/22/11   aro     Added interface to disable iC for debug purpose
11/15/11   cd      Get Tx AGC Rsp packet should return PA state instead of 
                   PA range
11/04/11   aro     Added generic payload for CDMA Driver verification cmd
10/13/11   aro     Added interface to enable and disable various debug mode for
                   various debug modules
09/22/11   cd      Added power mode and jammer state logging in Rx AGC response
                   packet
08/08/11   aro     Added command to perform calv3 config rx in FTM Task context
07/27/11   aro     Added generic FTM send command function for CDMA
07/12/11   aro     Added support to query intelliceiver power mode
07/01/11   aro     Added Debug interface to override Power Mode
06/14/11   Saul    HDR Rx AGC API now returns AGC for all carriers
06/14/11   sty     Renamed txagc_dBm100 to txagc_dBm10
06/13/11   aro     Added 1x Tx Override command
05/17/11   cd      Changed get Tx AGC implementation to return value in dBm100
05/06/11   aro     Added 1x enable diversity radio control template
03/07/11   aro     Added support for HDR retune control from FTM
03/03/11   sty     Added/renamed members in ftm_cdma_rsp_3_type
02/15/11   aro     Added support to pass LM handle to RFM interface
02/14/11   aro     Updated HDR RxAGC Wakeup request packet
02/14/11   aro     Removed AGC Mode from Get HDR RxAGC
02/13/11   aro     Added MC support for HDR Wakeup Rx Interface
02/13/11   aro     Added Set Tx Power Interface
02/13/11   aro     Added response 3 template to query TxAGC
02/12/11   aro     Updated response 2 template to include gain state and gain
                   word
02/08/11   aro     Moved the 1x Radio control to FTM
01/24/11   aro     Added LNA Range Query Packets
01/23/11   aro     Added Support for HDR RxAGC Query
01/10/11   aro     Changed PACKED to PACK(x)
01/07/11   aro     Added RXF Debug function
01/07/11   aro     Initial Revision

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_common_calibration_v3.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*!
  @name Internal Request Packet Definition

  @brief
  Request Packets used for embedded side internal CDMA commands. These commands
  dont send response back.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Radio Dispatch Request Packet to be used to configure radio for
  calibration v3 sweep
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_RFM_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  ftm_calibration_master_device_mask_type   device_master_mask;
  /*!< Master device Mask */

  ftm_calibration_slave_device_type device_slave;
  /*!< List of slave devices for each master device */

  uint8 band;
  /*!< RF band to which radio is to be tuned */

  uint16 channel;
  /*!< RF channel to which radio is to be tuned */

} ftm_cdma_cal_sweep_configure_radio_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Radio Dispatch Request Packet to be used to retune radio for
  calibration v3 sweep
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_RFM_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  ftm_calibration_master_device_mask_type   device_master_mask;
  /*!< Master device Mask */

  uint16 channel;
  /*!< RF channel to which radio is to be retuned */

} ftm_cdma_cal_sweep_retune_radio_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Radio Dispatch Request Packet to be used to configure radio for
  calibration v3 sweep
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_RFM_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  ftm_calibration_rx_action_mask_type cal_action;
  /*!<  The receive calibration actions mask. */

  uint8 power_mode;
  /*!< Intelliceiver Power Mode for calibration. 0 indicates Low Power Mode
  and 1 indicates High Power Mode */

  ftm_calibration_device_flag_mask_type device_flag;
  /*!< Rx Flag Mask indicating which device to calibrate */

} ftm_cdma_cal_sweep_configure_rx_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Radio Dispatch Request Packet to be used to tear down radio for
  calibration v3 sweep
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_RFM_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

} ftm_cdma_cal_sweep_tear_radio_req_type;

/*! @} */

/*============================================================================*/
/*!
  @name Request Packet Definition

  @brief
  Request Packets used with Diag interface to send commands to
  control Radio.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Request Packet Template 1
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device;
  /*!< RFM Device (rfm_device_enum_type) Under consideration */

} ftm_cdma_req_1_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Request Packet Template 2
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device;
  /*!< RFM Device (rfm_device_enum_type) Under consideration */

  uint32 lm_handle;
  /*!< LM Buffer Handle */

  uint8   band;
  /*!< RF Band */

  uint16 chan;
  /*!< RF Band */

} ftm_cdma_req_2_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Request Packet Template 3
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device;
  /*!< RFM Device (rfm_device_enum_type) Under consideration */

  uint8   carrier;
  /*!< RF Carrier */

  uint16 test_point;
  /*!< RxFrontEnd Test Point */

} ftm_cdma_req_3_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Request Packet Template 4
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device;
  /*!< RFM Device (rfm_device_enum_type) Under consideration */

} ftm_cdma_req_4_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Request Packet Template 5
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device;
  /*!< RFM Device (rfm_device_enum_type) Under consideration */

  uint8   tx_pwr_mode;
  /*!< Tx Power Mode / Open Loop or Closed Loop */

  int16   tx_pwr;
  /*!< TX Power */

} ftm_cdma_req_5_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  HWTC CDMA Dispatch Request Packet Template 6
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 10 for FTM HWTC */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_HWTC_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device;
  /*!< RFM Device (rfm_device_enum_type) Under consideration */

  uint32 lm_handle;
  /*!< LM Buffer Handle */

  uint8 band_chan_num;
  /*!< Number of Band and Channel sent */

  uint8   band0;
  /*!< RF Band 0 */

  uint16 chan0;
  /*!< RF Band 0 */

  uint8 carrier_idx0;
  /*!< Carrier Index for band_chan0 */

  uint8   band1;
  /*!< RF Band 1 */

  uint16 chan1;
  /*!< RF Band 1 */

  uint8 carrier_idx1;
  /*!< Carrier Index for band_chan1 */

  uint8   band2;
  /*!< RF Band 1 */

  uint16 chan2;
  /*!< RF Band 1 */

  uint8 carrier_idx2;
  /*!< Carrier Index for band_chan2 */

  uint8 agc_mode;
  /*!< AGC Mode to start with */

  uint32 rtc_offset;
  /*!< RTC Offset */

  int16 prev_agc;
  /*!< Previous AGC Value */

} ftm_cdma_req_6_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Request Packet Template 7
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device;
  /*!< RFM Device (rfm_device_enum_type) Under consideration */

  uint32 lm_handle;
  /*!< LM Buffer Handle */

} ftm_cdma_req_7_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  HWTC CDMA Dispatch Request Packet Template 8
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 10 for FTM HWTC */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_HWTC_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device;
  /*!< RFM Device (rfm_device_enum_type) Under consideration */

  uint32 lm_handle;
  /*!< LM Buffer Handle */

  uint8 band_chan_num;
  /*!< Number of Band and Channel sent */

  uint8   band0;
  /*!< RF Band 0 */

  uint16 chan0;
  /*!< RF Band 0 */

  uint8 carrier_idx0;
  /*!< Carrier Index for band_chan0 */

  uint8   band1;
  /*!< RF Band 1 */

  uint16 chan1;
  /*!< RF Band 1 */

  uint8 carrier_idx1;
  /*!< Carrier Index for band_chan1 */

  uint8   band2;
  /*!< RF Band 1 */

  uint16 chan2;
  /*!< RF Band 1 */

  uint8 carrier_idx2;
  /*!< Carrier Index for band_chan2 */

} ftm_cdma_req_8_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  HWTC CDMA Dispatch Request Packet Template 9
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 10 for FTM HWTC */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_HWTC_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device_1;
  /*!< RFM Master Device (rfm_device_enum_type) Under consideration */

  uint32 lm_handle_1;
  /*!< LM Buffer Handle */

  uint8   device_2;
  /*!< RFM Slave Device (rfm_device_enum_type) Under consideration */

  uint32 lm_handle_2;
  /*!< LM Buffer Handle */

} ftm_cdma_req_9_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  HWTC CDMA Dispatch Request Packet Template 10
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 10 for FTM HWTC */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_HWTC_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device;
  /*!< RFM Master Device (rfm_device_enum_type) whose Tx is to be overriden */

  uint8 pa_state;
  /*!< Override PA State value */

  uint16  avga_index;
  /*!< Override AVGA Index value */

  int16 betap_gain;
  /*!< Override BetaP Gain value */

  uint16 pa_bias;
  /*!< Override PA Bias value */

  uint16 pa_q_current;
  /*!< Override PA Q Current value */

  boolean pa_q_current_enable;
  /*!< Override PA Q Current Flag*/

} ftm_cdma_req_10_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  HWTC CDMA Dispatch Request Packet Template 11
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 10 for FTM HWTC */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_HWTC_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device;
  /*!< RFM Master Device (rfm_device_enum_type) whose Tx is to be overriden */

  uint8 power_mode;
  /*!< Power Mode */

} ftm_cdma_req_11_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA Dispatch Request Packet Template 12
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 10 for FTM HWTC */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_HWTC_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device;
  /*!< RFM Master Device */

  uint32 debug_mask;
  /*!< Debug Mask */

  uint32 debug_en;
  /*!< Debug Enable */

} ftm_cdma_req_12_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA Dispatch Request Packet Template 12
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 10 for FTM HWTC */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_HWTC_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint32 payload_0;
  /*!< Debug Payload 0 */

  uint32 payload_1;
  /*!< Debug Payload 1 */

  uint32 payload_2;
  /*!< Debug Payload 2 */

  uint32 payload_3;
  /*!< Debug Payload 3 */

  uint32 payload_4;
  /*!< Debug Payload 4 */

  uint32 payload_5;
  /*!< Debug Payload 5 */

  uint32 payload_6;
  /*!< Debug Payload 6 */

  uint32 payload_7;
  /*!< Debug Payload 7 */

} ftm_cdma_req_13_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Request Packet Template 14
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8   device;
  /*!< RFM Device (rfm_device_enum_type) Under consideration */

  uint8 disable_ic;
  /*!< Flag to indicate whether iC is to be disabled or not */

} ftm_cdma_req_14_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Request Packet Template 15
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8 device;
  /*!< The device */

  uint32   microsleep_enable;
  /*!< Enable or disable microsleep */

  uint32 rf_off_ustmr_delta;
  /*!< USTMR time relative to current ustmr, when to schedule rx off */

  uint32 rf_on_ustmr_delta;
 /*!< USTMR time relative to current ustmr, when to settle rxAGC */

  uint32 num_halfslots;
  /*!< NUmber of halfslots for which to call the API */

  uint32 sleep_mode;
  /*!< Sleep mode: micro/mili/mili_on_only/mili_off_only */
} ftm_cdma_req_15_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Request Packet Template 16
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8 device;
  /*!< The device */

  uint32 rtc_cx1_trigger_offset;
 /*!< the offset sleep rx time in rtc cx1 unit */

} ftm_cdma_req_16_type;

/*! @} */

/*============================================================================*/
/*!
  @name Response Packet Definition

  @brief
  Response Packets used to send Reponse Packet to Diag.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Response Packet 1
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16 cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16 req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16 rsp_len;
  /*!< Length of response packet to Diag/PC */

  int32 rf_status;
  /*!< RFM function exexution status */

} ftm_cdma_rsp_1_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Response Packet 2
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16 cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16 req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16 rsp_len;
  /*!< Length of response packet to Diag/PC */

  int16 rx_agc;
  /*!< Rx AGC Value */

  uint8 lna_state;
  /*!< Current LNA State */

  uint8 lna_gain_word;
  /*!< Current LNA Gain word */

  uint8 ic_state;
  /*!< Current intelliceiver state */

  uint8 jammer_state;
  /*!< Current Jammer Detect state */

} ftm_cdma_rsp_2_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Response Packet 3
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16 cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16 req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16 rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint16 avga_index;
  /*!< AVGA Index */

  uint32 avga_val;
  /*!< AVGA value */

  int32 txagc_dBm10;
  /*!< Tx AGC value */

  int16 betap_gain;
  /*!< Beta Prime val */

  uint8 pa_state;
  /*!< PA state value */

  uint32 apt_val;
  /*!< APT value */

} ftm_cdma_rsp_3_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Response Packet 4
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16 cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16 req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16 rsp_len;
  /*!< Length of response packet to Diag/PC */

  int16 rx_agc_c0;
  /*!< Rx AGC Value Carrier 0*/

  int16 rx_agc_c1;
  /*!< Rx AGC Value Carrier 1*/

  int16 rx_agc_c2;
  /*!< Rx AGC Value Carrier 2*/

  int16 rx_agc_all;
  /*!< Rx AGC Value Cummulative */

  uint8 lna_state;
  /*!< Current LNA State */

  uint8 lna_gain_word;
  /*!< Current LNA Gain word */

  uint8 ic_state;
  /*!< Current intelliceiver state */

  uint8 jammer_state;
  /*!< Current Jammer Detect state */

} ftm_cdma_rsp_4_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Response Packet 5
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16 cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  uint16 req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16 rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint8 power_mode;
  /*!< Current Power Mode*/

} ftm_cdma_rsp_5_type;

/*! @} */


#ifdef __cplusplus
}
#endif

#endif /*FEATURE_FACTORY_TESTMODE */

#endif /* FTM_CDMA_PACKET_TYPES_H */
