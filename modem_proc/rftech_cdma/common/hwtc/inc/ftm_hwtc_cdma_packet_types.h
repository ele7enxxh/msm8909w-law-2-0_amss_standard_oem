#ifndef FTM_HWTC_CDMA_PACKET_TYPES_H
#define FTM_HWTC_CDMA_PACKET_TYPES_H

/*!
  @file
  ftm_hwtc_cdma_packet_types.h

  @brief
  This module contains defintions of all Request and Response Packets
  used between Diag and HWTC.
*/

/*==============================================================================

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/hwtc/inc/ftm_hwtc_cdma_packet_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/04/12   adk     Added functionality to override NB and WB GDA parameters.
08/08/12   Saul    CDMA. Added code to profile writing dpd tables.
01/25/12   Saul    MCDO/FTM. Added DVGA offset dynamic configuration.
01/19/12   Saul    MCDO/FTM. Added ICIFIR configuration override.
08/30/11   aro     Added support to set Tx DP IQ Gain from FTM
01/24/11   aro     Added LNA Range Query Packets
01/23/11   aro     Added Support for HDR RxAGC Query
01/10/11   aro     Changed PACKED to PACK(x)
01/07/11   aro     Added RXF Debug function
01/07/11   aro     Initial Revision

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#ifdef FEATURE_FTM_HWTC

#ifdef __cplusplus
extern "C" {
#endif

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
  HWTC CDMA Dispatch Request Packet Template 1
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

  uint8   carrier;
  /*!< RF Carrier */

  uint16 test_point;
  /*!< RxFrontEnd Test Point */

} ftm_hwtc_cdma_req_1_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  HWTC CDMA Dispatch Request Packet Template 1
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

  uint32  tx_iq_gain;
  /*!< Tx DP IQ Gain */

} ftm_hwtc_cdma_req_2_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  HWTC CDMA Dispatch Request Packet Template 3
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

  uint8 narrow_band_index;
  /*!< The narrow-band index for which to update items */

  int32 freq_offset;
  /*!< The frequency offset for the narrow-band */

  uint8 bypass_val;
  /*!< Bypass value */

  uint8 update_iq_coeffs;
  /*!< Indicates whether or not to update iq coefficients */

  int16 i_coeff_0;
  /*!< I Coefficient 0 */

  int16 q_coeff_0;
  /*!< Q Coefficient 0 */

  int16 i_coeff_1;
  /*!< I Coefficient 1 */

  int16 q_coeff_1;
  /*!< Q Coefficient 1 */

  int16 i_coeff_2;
  /*!< I Coefficient 2 */

  int16 q_coeff_2;
  /*!< Q Coefficient 2 */

  int16 i_coeff_3;
  /*!< I Coefficient 3 */

  int16 q_coeff_3;
  /*!< Q Coefficient 3 */

  int16 i_coeff_4;
  /*!< I Coefficient 4 */

  int16 q_coeff_4;
  /*!< Q Coefficient 4 */

  int16 i_coeff_5;
  /*!< I Coefficient 5 */

  int16 q_coeff_5;
  /*!< Q Coefficient 5 */

  int16 i_coeff_6;
  /*!< I Coefficient 6 */

  int16 q_coeff_6;
  /*!< Q Coefficient 6 */

  int16 i_coeff_7;
  /*!< I Coefficient 7 */

  int16 q_coeff_7;
  /*!< Q Coefficient 7 */

  int16 i_coeff_8;
  /*!< I Coefficient 8 */

  int16 q_coeff_8;
  /*!< Q Coefficient 8 */

  int16 i_coeff_9;
  /*!< I Coefficient 9 */

  int16 q_coeff_9;
  /*!< Q Coefficient 9 */

  int16 i_coeff_10;
  /*!< I Coefficient 10 */

  int16 q_coeff_10;
  /*!< Q Coefficient 10 */

} ftm_hwtc_cdma_req_3_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  HWTC CDMA Dispatch Request Packet Template 4
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

  uint8 narrow_band_index;
  /*!< The narrow-band index for which to update items */

  int32 freq_offset;
  /*!< The frequency offset for the narrow-band */

  int32 dvga_val;
  /*!< DVGA value */

} ftm_hwtc_cdma_req_4_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  HWTC CDMA Dispatch Request Packet Template 5
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

  uint8 write_enable;
  /*!< Enables/disables writing */

  uint8 num_banks;
  /*!< The number of banks to write when "write_enable" is TRUE */

} ftm_hwtc_cdma_req_5_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  HWTC CDMA Dispatch Request Packet Template 5
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

  uint8 narrow_band_index;
  /*!< The narrow-band index for which to update items */

//  int32 freq_offset;
  /*!< The frequency offset for the narrow-band */

//  uint8 bypass_val;
  /*!< Bypass value */

  uint8 update_wbw_gda_coeffs;
  /*!< Indicates whether or not to update WB GDA coefficients */

  uint8 update_nbn_gda_coeffs;
  /*!< Indicates whether or not to update NB GDA coefficients */

  int16 wb0_gda;
  /*!< RXFE_WB0_WB_GDA */

  int16 wb1_gda;
  /*!< RXFE_WB1_WB_GDA */

  int16 wb2_gda;
  /*!< RXFE_WB2_WB_GDA */

  int16 nb0_gda;
  /*!< RXFE_NB0_GDA */

  int16 nb1_gda;
  /*!< RXFE_NB1_GDA */

  int16 nb2_gda;
  /*!< RXFE_NB2_GDA */

  int16 nb3_gda;
  /*!< RXFE_NB3_GDA */

  int16 nb4_gda;
  /*!< RXFE_NB4_GDA */

  int16 nb5_gda;
  /*!< RXFE_NB5_GDA */

  int16 nb6_gda;
  /*!< RXFE_NB6_GDA */

  int16 nb7_gda;
  /*!< RXFE_NB7_GDA */

} ftm_hwtc_cdma_req_6_type;

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
  HWTC CDMA Dispatch Response Packet 1
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 10 for FTM HWTC */

  uint16 mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_1X_C */

  uint16 cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_HWTC_CDMA_RFM_1X_WAKEUP_RX */

  uint16 req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16 rsp_len;
  /*!< Length of response packet to Diag/PC */

  int32 rf_status;
  /*!< RFM function execution status */

} ftm_hwtc_cdma_rsp_1_type;

/*! @} */


#ifdef __cplusplus
}
#endif

#endif /* FEATURE_FTM_HWTC */

#endif /*FEATURE_FACTORY_TESTMODE */

#endif /* FTM_HWTC_CDMA_PACKET_TYPES_H */
