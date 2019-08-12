/*!
  @file
  rfcommon_fbrx_nv.c

  @brief
  This module contains interface and implementation of common FBRx NV items.
  
*/

/*==============================================================================

  Copyright (c) 2014-2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/fbrx/src/rfcommon_fbrx_nv.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/16/16   cv      [port from TH]Added LTE B66
04/06/15   par     Fix FBRX temp comp data being applied
02/25/15   ska     Fix KW error
01/28/15   ska     Delete FBRx cases for Rx only bands: B29 and B32
12/04/14    vb     Fix for array out of bounds for LTE droop NVs in valid cases
12/03/14    vb     KW fix for possible NULL memory access
11/17/14    vb     LTE band and bandwidth specific Droop cal support
11/05/14   vs      Fix linker errors in W/T compile out flavors
11/03/14   ndb     Fix the compiler error in LTE removed builds
10/29/14   pk      Compiler error fix on 1x compiled out build 
10/08/14   as      Support FBRX temp compensation compressed NVs 
09/26/14   as      Fixing non LTE config for Jolokia
09/25/14   brath   Ensure DC cal results are updated in DM structure
09/17/14    aa     Fixed compiler warning
07/31/14   ndb     Added support for extended EARFCN for LTE
07/28/14    aa     Share W B3/B9, B5/19 FBRX NVs 
06/10/14    ka     Support for RB-based freq comp 
05/15/14    aa     Update the fbrx temp comp across freq
05/05/14    ka     Added fbrx gain freq comp
04/21/14   JJ      Check band supported before retrieveing cdma fbrx nv
04/17/14   kab     Allocate memory(FBRx) for supported LTE bands only
04/10/14    ka     KW fixes
03/21/14    aa     Fix reload nv items for fbrx
03/14/14   jmf     Fix to read/update FBRx Droop Global Struct
03/07/14    aa     Latest FBRx changes for droop cal
02/27/14    aa     Use rfcom_band_type_u 
02/21/13    aa     Added support for fbrx control via nv 
02/10/14    aa     Update dc and temp comp data to fbrx DM
02/10/14    aa     Added support to read DC & temp comp cal data
02/05/14    aa     Updated params for droop cal data
01/30/14    aa     Updated gain cal params & added fbrx mode support
01/21/14   jmf     Add support to update global NV structure for Droop Filter
01/14/14    aa     Added support to read FBRx NV's during bootup
01/10/14    aa     Backing out change due to RFCal crash
01/08/14    aa     Added support to read FBRx NV's during bootup 
12/16/13    aa     Added fbrx related api's from rfcommon_nv.c file 
11/25/13    aa     Added api to update FBRX Droop Cal data
11/21/13    aa     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfcommon_fbrx_nv.h"
#include "rfcommon_nv.h"
#include "msg.h"
#include "modem_mem.h"
#include "stringl.h" /* for memscpy */
#ifdef FEATURE_WCDMA
#include "rfwcdma_core.h" /* for rfwcdma_core_is_band_supported() */
#endif
#include "rfcommon_msg.h"
#include "rf_cdma_utils_freq.h"
#include "rf_tdscdma_core_util.h"
#include "rflm_dm_api.h"
#include "zlib.h"

#ifdef FEATURE_LTE
#include "rflte_nv.h"
#include "rflte_util.h"
#endif

#ifdef FEATURE_CDMA1X
#include "rf_cdma_utils.h"  /* for rfm_cdma_is_band_supported() */
#include "rfc_cdma.h"  /* for rfc_cdma_is_tx_device() */
#endif
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Static Global Variable to store the FBRX Cal data for all techs
*/

static rfcommon_fbrx_nv_cal_data_type rfcommon_fbrx_nv_tech_cal_data = 
{
  {{NULL}, {NULL}, {NULL}, {NULL} },         /* FBRx Cal data for CDMA (Gain, SP, DC, Temp Comp ) */
  {{NULL}, {NULL}, {NULL}, {NULL} },         /* FBRx Cal data for WDMA (Gain, SP, DC, Temp Comp ) */
  {{NULL}, {NULL}, {NULL}, {NULL}, {NULL} }, /* FBRx Cal data for LTE  (Gain, SP, DC, Temp Comp ) */
  {{NULL}, {NULL}, {NULL}, {NULL} }          /* FBRx Cal data for TDS  (Gain, SP, DC, Temp Comp ) */
};

static rfcommon_fbrx_nv_droop_type *rfcommon_fbrx_nv_droop_cal_data[RFCOMMON_FBRX_NUM_TX_CONFIG_MAX] = {NULL}; 

static int32 rfcommon_fbrx_sys_control_nv_data = 0; /* FBRx control NV data */

static boolean rfcommon_fbrx_nv_droop_mem_allocated[RFCOMMON_FBRX_NUM_TX_CONFIG_MAX] = {FALSE};

static rfcommon_fbrx_nv_init_type rfcommon_fbrx_nv_init = {FALSE, FALSE, FALSE, FALSE};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_get_droop_cal_nv_id
 
  @details
  This function determines the Droop Cal item for all techs based on the BW
 
  @param tx_cfg Tech Tx BW config : Input
  @param nv_item_droop FBRx Droop NV ID : Output
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR
*/

rfcommon_fbrx_error_type 
rfcommon_fbrx_get_droop_cal_nv_id( rfcommon_fbrx_tx_cfg_type tx_cfg, 
                                   rfnv_item_id_enum_type *nv_item_droop )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

  switch (tx_cfg)
  {
    case RFCOMMON_FBRX_UMTS_DC:
      *nv_item_droop = RFNV_UMTS_DC_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_MCDO_7X:
      *nv_item_droop = RFNV_MCDO_7X_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_LTE_20:
      *nv_item_droop = RFNV_LTE_20_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_LTE_15:
      *nv_item_droop = RFNV_LTE_15_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_SB_SVDO:
      *nv_item_droop = RFNV_SB_SVDO_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_LTE_20_PLUS_20:
      *nv_item_droop = RFNV_LTE_20_PLUS_20_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_1X:
      *nv_item_droop = RFNV_CDMA_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_MCDO_3X:
      *nv_item_droop = RFNV_MCDO_3X_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_UMTS_SC:
      *nv_item_droop = RFNV_UMTS_SC_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_TDS:
      *nv_item_droop = RFNV_TDSCDMA_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_LTE_1P4:
      *nv_item_droop = RFNV_LTE_1P4_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_LTE_3:
      *nv_item_droop = RFNV_LTE_3_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_LTE_5:
      *nv_item_droop = RFNV_LTE_5_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    case RFCOMMON_FBRX_LTE_10:
      *nv_item_droop = RFNV_LTE_10_FBRX_DROOP_FIR_FILTER_COEFF_I;
      break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_get_droop_cal_nv_id: Invalid TX Config = %d", tx_cfg);
      *nv_item_droop = RFNV_UMTS_SC_FBRX_DROOP_FIR_FILTER_COEFF_I;
      ret_value = RFCOMMON_FBRX_ERROR;

  } /* switch case*/

  return ret_value;

}/* rfcommon_fbrx_get_droop_cal_nv_id */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_get_wcdma_cal_nv_id
 
  @details
  This function determines the FBRx Gain and Switchpoint NV item for WCDMA
 
  @param band WCDMA Band : Input
  @param nv_item_gain FBRx Gain NV ID : Output
  @param nv_item_sp FBRx Switchpoint NV ID : Output
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR
*/

rfcommon_fbrx_error_type rfcommon_fbrx_get_wcdma_cal_nv_id( rfcom_wcdma_band_type band, 
                                                            rfnv_item_id_enum_type *nv_item_gain, 
                                                            rfnv_item_id_enum_type *nv_item_sp,
                                                            rfnv_item_id_enum_type *nv_item_dc,
                                                            rfnv_item_id_enum_type *nv_item_temp )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

  switch (band)
  {
    case   RFCOM_BAND_IMT:
      *nv_item_gain = RFNV_WCDMA_B1_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_WCDMA_B1_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_WCDMA_B1_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_WCDMA_B1_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_1900:
      *nv_item_gain = RFNV_WCDMA_B2_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_WCDMA_B2_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_WCDMA_B2_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_WCDMA_B2_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_BC3:
    case   RFCOM_BAND_BC9:
      *nv_item_gain = RFNV_WCDMA_B3_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_WCDMA_B3_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_WCDMA_B3_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_WCDMA_B3_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_BC4:
      *nv_item_gain = RFNV_WCDMA_B4_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_WCDMA_B4_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_WCDMA_B4_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_WCDMA_B4_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_800:
	  case   RFCOM_BAND_BC19:
      *nv_item_gain = RFNV_WCDMA_B5_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_WCDMA_B5_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_WCDMA_B5_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_WCDMA_B5_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_BC8:
      *nv_item_gain = RFNV_WCDMA_B8_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_WCDMA_B8_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_WCDMA_B8_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_WCDMA_B8_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_BC11:
      *nv_item_gain = RFNV_WCDMA_B11_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_WCDMA_B11_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_WCDMA_B11_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_WCDMA_B11_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_get_wcdma_cal_nv_id: Invalid Band = %d", band);
      *nv_item_gain = RFNV_WCDMA_B1_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_WCDMA_B1_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_WCDMA_B1_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_WCDMA_B1_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      ret_value = RFCOMMON_FBRX_ERROR;

  } /* switch case*/

  return ret_value;

}/* rfcommon_fbrx_get_wcdma_cal_nv_id */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_get_wcdma_cal_compressed_nv_id
 
  @details
  This function determines the FBRx temp compressed NV item for WCDMA
 
  @param band WCDMA Band : Input
  @param nv_item_temp_z : Output
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR
*/

rfcommon_fbrx_error_type rfcommon_fbrx_get_wcdma_cal_compressed_nv_id( rfcom_wcdma_band_type band, 
                                                                       rfnv_item_id_enum_type *nv_item_temp_z )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

  switch (band)
  {
    case   RFCOM_BAND_IMT:
      *nv_item_temp_z = RFNV_WCDMA_B1_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_1900:
      *nv_item_temp_z = RFNV_WCDMA_B2_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_BC3:
    case   RFCOM_BAND_BC9:
      *nv_item_temp_z = RFNV_WCDMA_B3_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_BC4:
      *nv_item_temp_z = RFNV_WCDMA_B4_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_800:
    case   RFCOM_BAND_BC19:
      *nv_item_temp_z = RFNV_WCDMA_B5_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_BC8:
      *nv_item_temp_z = RFNV_WCDMA_B8_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_BC11:
      *nv_item_temp_z = RFNV_WCDMA_B11_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_get_wcdma_cal_compressed_nv_id: Invalid Band = %d", band);
      *nv_item_temp_z = RFNV_WCDMA_B1_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      ret_value = RFCOMMON_FBRX_ERROR;

  } /* switch case*/

  return ret_value;

}/* rfcommon_fbrx_get_wcdma_cal_compressed_nv_id */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_get_1x_cal_nv_id
 
  @details
  This function determines the FBRx Gain and Switchpoint NV item for CDMA
 
  @param band CDMA Band : Input
  @param nv_item_gain FBRx Gain NV ID : Output
  @param nv_item_sp FBRx Switchpoint NV ID : Output
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR
*/

rfcommon_fbrx_error_type rfcommon_fbrx_get_1x_cal_nv_id( rfm_cdma_band_class_type band, 
                                                         rfnv_item_id_enum_type *nv_item_gain, 
                                                         rfnv_item_id_enum_type *nv_item_sp,
                                                         rfnv_item_id_enum_type *nv_item_dc,
                                                         rfnv_item_id_enum_type *nv_item_temp )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

  switch (band)
  {
    case   RFM_CDMA_BC0:
      *nv_item_gain = RFNV_CDMA_BC0_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_CDMA_BC0_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_CDMA_BC0_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_CDMA_BC0_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFM_CDMA_BC1:
      *nv_item_gain = RFNV_CDMA_BC1_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_CDMA_BC1_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_CDMA_BC1_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_CDMA_BC1_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFM_CDMA_BC4:
      *nv_item_gain = RFNV_CDMA_BC4_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_CDMA_BC4_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_CDMA_BC4_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_CDMA_BC4_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFM_CDMA_BC5:
      *nv_item_gain = RFNV_CDMA_BC5_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_CDMA_BC5_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_CDMA_BC5_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_CDMA_BC5_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFM_CDMA_BC6:
      *nv_item_gain = RFNV_CDMA_BC6_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_CDMA_BC6_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_CDMA_BC6_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_CDMA_BC6_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFM_CDMA_BC10:
      *nv_item_gain = RFNV_CDMA_BC10_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_CDMA_BC10_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_CDMA_BC10_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_CDMA_BC10_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFM_CDMA_BC14:
      *nv_item_gain = RFNV_CDMA_BC14_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_CDMA_BC14_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_CDMA_BC14_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_CDMA_BC14_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFM_CDMA_BC15:
      *nv_item_gain = RFNV_CDMA_BC15_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_CDMA_BC15_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_CDMA_BC15_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_CDMA_BC15_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_get_1x_cal_nv_id: Invalid Band = %d", band);
      *nv_item_gain = RFNV_CDMA_BC0_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_CDMA_BC0_FBRX_GAIN_SWITCH_POINT_I ;
      *nv_item_dc = RFNV_CDMA_BC0_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_CDMA_BC0_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      ret_value = RFCOMMON_FBRX_ERROR;

  } /* switch case*/

  return ret_value;

} /* rfcommon_fbrx_get_1x_cal_nv_id */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_get_1x_cal_compressed_nv_id
 
  @details
  This function determines the temperature compensation compressed NV item for CDMA
 
  @param band CDMA Band : Input
  @param nv_item_temp_z : Output
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR
*/

rfcommon_fbrx_error_type rfcommon_fbrx_get_1x_cal_compressed_nv_id( rfm_cdma_band_class_type band, 
                                                                    rfnv_item_id_enum_type *nv_item_temp_z )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

  switch (band)
  {
    case   RFM_CDMA_BC0:
      *nv_item_temp_z = RFNV_CDMA_BC0_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFM_CDMA_BC1:
      *nv_item_temp_z = RFNV_CDMA_BC1_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFM_CDMA_BC4:
      *nv_item_temp_z = RFNV_CDMA_BC4_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFM_CDMA_BC5:
      *nv_item_temp_z = RFNV_CDMA_BC5_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFM_CDMA_BC6:
      *nv_item_temp_z = RFNV_CDMA_BC6_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFM_CDMA_BC10:
      *nv_item_temp_z = RFNV_CDMA_BC10_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFM_CDMA_BC14:
      *nv_item_temp_z = RFNV_CDMA_BC14_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFM_CDMA_BC15:
      *nv_item_temp_z = RFNV_CDMA_BC15_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_get_1x_cal_compressed_nv_id: Invalid Band = %d", band);
      *nv_item_temp_z = RFNV_CDMA_BC0_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      ret_value = RFCOMMON_FBRX_ERROR;

  } /* switch case*/

  return ret_value;

} /* rfcommon_fbrx_get_1x_cal_compressed_nv_id */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_get_tdscdma_cal_nv_id
 
  @details
  This function determines the FBRx Gain and Switchpoint NV item for TDSCDMA
 
  @param band TDSCDMA Band : Input
  @param nv_item_gain FBRx Gain NV ID : Output
  @param nv_item_sp FBRx Switchpoint NV ID : Output
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR
*/

rfcommon_fbrx_error_type rfcommon_fbrx_get_tdscdma_cal_nv_id( rfcom_tdscdma_band_type band, 
                                                              rfnv_item_id_enum_type *nv_item_gain, 
                                                              rfnv_item_id_enum_type *nv_item_sp,
                                                              rfnv_item_id_enum_type *nv_item_dc,
                                                              rfnv_item_id_enum_type *nv_item_temp  )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

  switch (band)
  {
    case   RFCOM_BAND_TDSCDMA_B34:
      *nv_item_gain = RFNV_TDSCDMA_B34_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_TDSCDMA_B34_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_TDSCDMA_B34_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_TDSCDMA_B34_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_TDSCDMA_B39:
      *nv_item_gain = RFNV_TDSCDMA_B39_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_TDSCDMA_B39_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_TDSCDMA_B39_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_TDSCDMA_B39_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;
 
    case   RFCOM_BAND_TDSCDMA_B40:
      *nv_item_gain = RFNV_TDSCDMA_B40_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_TDSCDMA_B40_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_TDSCDMA_B40_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_TDSCDMA_B40_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_TDSCDMA_B40_B:
      *nv_item_gain = RFNV_TDSCDMA_B40_B_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_TDSCDMA_B40_B_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_TDSCDMA_B40_B_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_TDSCDMA_B40_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_TDSCDMA_B34_B:
      *nv_item_gain = RFNV_TDSCDMA_B34_B_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_TDSCDMA_B34_B_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_TDSCDMA_B34_B_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_TDSCDMA_B34_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_TDSCDMA_B39_B:
      *nv_item_gain = RFNV_TDSCDMA_B39_B_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_TDSCDMA_B39_B_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_TDSCDMA_B39_B_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_TDSCDMA_B39_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_get_tdscdma_cal_nv_id: Invalid Band = %d", band);
      *nv_item_gain = RFNV_TDSCDMA_B34_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_TDSCDMA_B34_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_TDSCDMA_B34_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_TDSCDMA_B34_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      ret_value = RFCOMMON_FBRX_ERROR;

  } /* switch case*/

  return ret_value;

} /* rfcommon_fbrx_get_tdscdma_cal_nv_id */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_get_tdscdma_cal_compressed_nv_id
 
  @details
  This function determines the temperature compensation compressed NV item for TDSCDMA
 
  @param band TDSCDMA Band : Input
  @param nv_item_temp_z : Output
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR
*/

rfcommon_fbrx_error_type rfcommon_fbrx_get_tdscdma_cal_compressed_nv_id( rfcom_tdscdma_band_type band, 
                                                                         rfnv_item_id_enum_type *nv_item_temp_z  )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

  switch (band)
  {
    case   RFCOM_BAND_TDSCDMA_B34:
      *nv_item_temp_z = RFNV_TDSCDMA_B34_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_TDSCDMA_B39:
      *nv_item_temp_z = RFNV_TDSCDMA_B39_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;
 
    case   RFCOM_BAND_TDSCDMA_B40:
      *nv_item_temp_z = RFNV_TDSCDMA_B40_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_TDSCDMA_B40_B:
      *nv_item_temp_z = RFNV_TDSCDMA_B40_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_TDSCDMA_B34_B:
      *nv_item_temp_z = RFNV_TDSCDMA_B34_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_TDSCDMA_B39_B:
      *nv_item_temp_z = RFNV_TDSCDMA_B39_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_get_tdscdma_cal_compressed_nv_id: Invalid Band = %d", band);
      *nv_item_temp_z = RFNV_TDSCDMA_B34_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      ret_value = RFCOMMON_FBRX_ERROR;

  } /* switch case*/

  return ret_value;

} /* rfcommon_fbrx_get_tdscdma_cal_compressed_nv_id */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_get_lte_cal_nv_id
 
  @details
  This function determines the FBRx Gain and Switchpoint NV item for LTE
 
  @param band LTE Band : Input
  @param nv_item_gain FBRx Gain NV ID : Output
  @param nv_item_sp FBRx Switchpoint NV ID : Output
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR
*/

rfcommon_fbrx_error_type rfcommon_fbrx_get_lte_cal_nv_id( rfcom_lte_band_type band, 
                                                          rfnv_item_id_enum_type *nv_item_gain, 
                                                          rfnv_item_id_enum_type *nv_item_sp,
                                                          rfnv_item_id_enum_type *nv_item_dc,
                                                          rfnv_item_id_enum_type *nv_item_temp )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

  switch (band)
  {
    case   RFCOM_BAND_LTE_B1:
      *nv_item_gain = RFNV_LTE_C0_B1_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B1_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B1_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B1_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B2:
      *nv_item_gain = RFNV_LTE_C0_B2_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B2_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B2_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B2_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B3:
      *nv_item_gain = RFNV_LTE_C0_B3_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B3_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B3_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B3_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B4:
      *nv_item_gain = RFNV_LTE_C0_B4_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B4_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B4_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B4_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B5:
      *nv_item_gain = RFNV_LTE_C0_B5_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B5_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B5_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B5_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B6:
      *nv_item_gain = RFNV_LTE_C0_B6_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B6_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B6_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B6_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B7:
      *nv_item_gain = RFNV_LTE_C0_B7_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B7_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B7_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B7_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B8:
      *nv_item_gain = RFNV_LTE_C0_B8_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B8_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B8_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B8_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B9:
      *nv_item_gain = RFNV_LTE_C0_B9_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B9_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B9_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B9_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B10:
      *nv_item_gain = RFNV_LTE_C0_B10_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B10_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B10_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B10_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B11:
      *nv_item_gain = RFNV_LTE_C0_B11_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B11_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B11_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B11_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B12:
      *nv_item_gain = RFNV_LTE_C0_B12_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B12_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B12_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B12_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B13:
      *nv_item_gain = RFNV_LTE_C0_B13_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B13_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B13_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B13_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B14:
      *nv_item_gain = RFNV_LTE_C0_B14_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B14_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B14_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B14_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B17:
      *nv_item_gain = RFNV_LTE_C0_B17_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B17_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B17_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B17_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B18:
      *nv_item_gain = RFNV_LTE_C0_B18_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B18_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B18_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B18_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B19:
      *nv_item_gain = RFNV_LTE_C0_B19_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B19_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B19_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B19_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B20:
      *nv_item_gain = RFNV_LTE_C0_B20_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B20_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B20_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B20_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B21:
      *nv_item_gain = RFNV_LTE_C0_B21_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B21_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B21_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B21_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B23:
      *nv_item_gain = RFNV_LTE_C0_B23_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B23_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B23_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B23_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B25:
      *nv_item_gain = RFNV_LTE_C0_B25_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B25_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B25_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B25_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B26:
      *nv_item_gain = RFNV_LTE_C0_B26_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B26_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B26_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B26_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B27:
      *nv_item_gain = RFNV_LTE_C0_B27_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B27_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B27_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B27_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B28:
      *nv_item_gain = RFNV_LTE_C0_B28_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B28_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B28_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B28_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

   case   RFCOM_BAND_LTE_B30:
      *nv_item_gain = RFNV_LTE_C0_B30_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B30_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B30_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B30_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B31:
      *nv_item_gain = RFNV_LTE_C0_B31_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B31_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B31_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B31_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B34:
      *nv_item_gain = RFNV_LTE_C0_B34_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B34_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B34_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B34_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B38:
      *nv_item_gain = RFNV_LTE_C0_B38_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B38_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B38_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B38_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B39:
      *nv_item_gain = RFNV_LTE_C0_B39_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B39_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B39_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B39_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B40:
      *nv_item_gain = RFNV_LTE_C0_B40_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B40_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B40_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B40_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B41:
      *nv_item_gain = RFNV_LTE_C0_B41_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B41_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B41_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B41_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B42:
      *nv_item_gain = RFNV_LTE_C0_B42_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B42_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B42_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B42_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B43:
      *nv_item_gain = RFNV_LTE_C0_B43_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B43_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B43_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B43_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B44:
      *nv_item_gain = RFNV_LTE_C0_B44_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B44_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B44_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B44_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B28_B:
      *nv_item_gain = RFNV_LTE_C0_B28_B_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B28_B_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B28_B_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B28_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B40_B:
      *nv_item_gain = RFNV_LTE_C0_B40_B_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B40_B_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B40_B_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B40_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B41_B:
      *nv_item_gain = RFNV_LTE_C0_B41_B_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B41_B_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B41_B_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B41_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B41_C:
      *nv_item_gain = RFNV_LTE_C0_B41_C_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B41_C_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B41_C_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B41_C_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

    case   RFCOM_BAND_LTE_B39_B:
      *nv_item_gain = RFNV_LTE_C0_B39_B_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B39_B_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B39_B_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B39_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      break;

      case   RFCOM_BAND_LTE_B66:
        *nv_item_gain = RFNV_LTE_C0_B66_FBRX_GAIN_FREQ_I;
        *nv_item_sp   = RFNV_LTE_C0_B66_FBRX_GAIN_SWITCH_POINT_I;
        *nv_item_dc   = RFNV_LTE_C0_B66_FBRX_DC_CAL_VAL_I;
        // This band only supports the compressed gain_vs_temp_vs_freq item
        *nv_item_temp = RFNV_ITEMS_ENUM_MAX;
        break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_get_lte_cal_nv_id: Invalid Band = %d", band);
      *nv_item_gain = RFNV_LTE_C0_B1_FBRX_GAIN_FREQ_I;
      *nv_item_sp = RFNV_LTE_C0_B1_FBRX_GAIN_SWITCH_POINT_I;
      *nv_item_dc = RFNV_LTE_C0_B1_FBRX_DC_CAL_VAL_I;
      *nv_item_temp = RFNV_LTE_C0_B1_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_I;
      ret_value = RFCOMMON_FBRX_ERROR;

  } /* switch case*/

  return ret_value;

} /* rfcommon_fbrx_get_lte_cal_nv_id */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_get_lte_cal_nv_id
 
  @details
  This function determines the FBRx Gain and Switchpoint NV item for LTE
 
  @param band LTE Band : Input
  @param nv_item_temp_z NV ID : Output
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR
*/

rfcommon_fbrx_error_type rfcommon_fbrx_get_lte_cal_compressed_nv_id( rfcom_lte_band_type band, 
                                                                     rfnv_item_id_enum_type *nv_item_temp_z )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

  switch (band)
  {
    case   RFCOM_BAND_LTE_B1:
      *nv_item_temp_z = RFNV_LTE_C0_B1_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B2:
      *nv_item_temp_z = RFNV_LTE_C0_B2_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B3:
      *nv_item_temp_z = RFNV_LTE_C0_B3_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B4:
      *nv_item_temp_z = RFNV_LTE_C0_B4_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B5:
      *nv_item_temp_z = RFNV_LTE_C0_B5_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B6:
      *nv_item_temp_z = RFNV_LTE_C0_B6_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B7:
      *nv_item_temp_z = RFNV_LTE_C0_B7_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B8:
      *nv_item_temp_z = RFNV_LTE_C0_B8_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B9:
      *nv_item_temp_z = RFNV_LTE_C0_B9_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B10:
      *nv_item_temp_z = RFNV_LTE_C0_B10_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B11:
      *nv_item_temp_z = RFNV_LTE_C0_B11_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B12:
      *nv_item_temp_z = RFNV_LTE_C0_B12_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B13:
      *nv_item_temp_z = RFNV_LTE_C0_B13_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B14:
      *nv_item_temp_z = RFNV_LTE_C0_B14_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B17:
      *nv_item_temp_z = RFNV_LTE_C0_B17_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B18:
      *nv_item_temp_z = RFNV_LTE_C0_B18_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B19:
      *nv_item_temp_z = RFNV_LTE_C0_B19_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B20:
      *nv_item_temp_z = RFNV_LTE_C0_B20_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B21:
      *nv_item_temp_z = RFNV_LTE_C0_B21_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B23:
      *nv_item_temp_z = RFNV_LTE_C0_B23_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B25:
      *nv_item_temp_z = RFNV_LTE_C0_B25_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B26:
      *nv_item_temp_z = RFNV_LTE_C0_B26_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B27:
      *nv_item_temp_z = RFNV_LTE_C0_B27_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B28:
      *nv_item_temp_z = RFNV_LTE_C0_B28_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B30:
      *nv_item_temp_z = RFNV_LTE_C0_B30_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B31:
      *nv_item_temp_z = RFNV_LTE_C0_B31_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B34:
      *nv_item_temp_z = RFNV_LTE_C0_B34_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B38:
      *nv_item_temp_z = RFNV_LTE_C0_B38_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B39:
      *nv_item_temp_z = RFNV_LTE_C0_B39_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B40:
      *nv_item_temp_z = RFNV_LTE_C0_B40_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B41:
      *nv_item_temp_z = RFNV_LTE_C0_B41_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B42:
      *nv_item_temp_z = RFNV_LTE_C0_B42_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B43:
      *nv_item_temp_z = RFNV_LTE_C0_B43_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B44:
      *nv_item_temp_z = RFNV_LTE_C0_B44_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B28_B:
      *nv_item_temp_z = RFNV_LTE_C0_B28_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B40_B:
      *nv_item_temp_z = RFNV_LTE_C0_B40_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B41_B:
      *nv_item_temp_z = RFNV_LTE_C0_B41_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B41_C:
      *nv_item_temp_z = RFNV_LTE_C0_B41_C_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

    case   RFCOM_BAND_LTE_B39_B:
      *nv_item_temp_z = RFNV_LTE_C0_B39_B_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      break;

      case   RFCOM_BAND_LTE_B66:
        *nv_item_temp_z = RFNV_LTE_C0_B66_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
        break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_get_lte_cal_compressed_nv_id: Invalid Band = %d", band);
      *nv_item_temp_z = RFNV_LTE_C0_B1_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I;
      ret_value = RFCOMMON_FBRX_ERROR;

  } /* switch case*/

  return ret_value;

} /* rfcommon_fbrx_get_lte_cal_compressed_nv_id */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the LTE band specific droop cal NV items
 
  @details
  
  @param band
  Input LTE band
 
  @param nv_item_temp_z
  Droop NV item for the LTE band passed as input
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR
*/
rfcommon_fbrx_error_type rfcommon_fbrx_get_lte_band_bw_droop_compressed_nv_id( rfcom_lte_band_type band, 
                                                                               rfnv_item_id_enum_type *nv_item_temp_z )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

  switch (band)
  {
    case   RFCOM_BAND_LTE_B1:
      *nv_item_temp_z = RFNV_LTE_C0_B1_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B2:
      *nv_item_temp_z = RFNV_LTE_C0_B2_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B3:
      *nv_item_temp_z = RFNV_LTE_C0_B3_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B4:
      *nv_item_temp_z = RFNV_LTE_C0_B4_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B5:
      *nv_item_temp_z = RFNV_LTE_C0_B5_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B6:
      *nv_item_temp_z = RFNV_LTE_C0_B6_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B7:
      *nv_item_temp_z = RFNV_LTE_C0_B7_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B8:
      *nv_item_temp_z = RFNV_LTE_C0_B8_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B9:
      *nv_item_temp_z = RFNV_LTE_C0_B9_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B10:
      *nv_item_temp_z = RFNV_LTE_C0_B10_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B11:
      *nv_item_temp_z = RFNV_LTE_C0_B11_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B12:
      *nv_item_temp_z = RFNV_LTE_C0_B12_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B13:
      *nv_item_temp_z = RFNV_LTE_C0_B13_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B14:
      *nv_item_temp_z = RFNV_LTE_C0_B14_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B17:
      *nv_item_temp_z = RFNV_LTE_C0_B17_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B18:
      *nv_item_temp_z = RFNV_LTE_C0_B18_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B19:
      *nv_item_temp_z = RFNV_LTE_C0_B19_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B20:
      *nv_item_temp_z = RFNV_LTE_C0_B20_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B21:
      *nv_item_temp_z = RFNV_LTE_C0_B21_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B23:
      *nv_item_temp_z = RFNV_LTE_C0_B23_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B25:
      *nv_item_temp_z = RFNV_LTE_C0_B25_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B26:
      *nv_item_temp_z = RFNV_LTE_C0_B26_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B27:
      *nv_item_temp_z = RFNV_LTE_C0_B27_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B28:
      *nv_item_temp_z = RFNV_LTE_C0_B28_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B30:
      *nv_item_temp_z = RFNV_LTE_C0_B30_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B31:
      *nv_item_temp_z = RFNV_LTE_C0_B31_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B34:
      *nv_item_temp_z = RFNV_LTE_C0_B34_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B38:
      *nv_item_temp_z = RFNV_LTE_C0_B38_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B39:
      *nv_item_temp_z = RFNV_LTE_C0_B39_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B40:
      *nv_item_temp_z = RFNV_LTE_C0_B40_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B41:
      *nv_item_temp_z = RFNV_LTE_C0_B41_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B42:
      *nv_item_temp_z = RFNV_LTE_C0_B42_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B43:
      *nv_item_temp_z = RFNV_LTE_C0_B43_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B44:
      *nv_item_temp_z = RFNV_LTE_C0_B44_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B28_B:
      *nv_item_temp_z = RFNV_LTE_C0_B28_B_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B40_B:
      *nv_item_temp_z = RFNV_LTE_C0_B40_B_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B41_B:
      *nv_item_temp_z = RFNV_LTE_C0_B41_B_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B41_C:
      *nv_item_temp_z = RFNV_LTE_C0_B41_C_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

    case   RFCOM_BAND_LTE_B39_B:
      *nv_item_temp_z = RFNV_LTE_C0_B39_B_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      break;

      case   RFCOM_BAND_LTE_B66:
        *nv_item_temp_z = RFNV_LTE_C0_B66_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
        break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_get_lte_band_bw_droop_compressed_nv_id: Invalid Band = %d", band);
      *nv_item_temp_z = RFNV_LTE_C0_B1_FBRX_DROOP_FIR_FILTER_COEFF_Z_I;
      ret_value = RFCOMMON_FBRX_ERROR;

  } /* switch case*/

  return ret_value;

} /* rfcommon_fbrx_get_lte_band_bw_droop_compressed_nv_id */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_get_cal_nv_items: API for FBRX NV item code retrieve based on band & tech 

  @details
  The code gets the FBRx Gain & Switchpoint NV item code based on tech and band
 
  @param tech
  tech information
 
  @param band
  band of each tech
  
  @param nv_item_code_gain
  pointer to return FBRx Gain NV item code information
 
  @param nv_item_code_sp
  pointer to return FBRx Switchpoint NV item code information
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR

*/

rfcommon_fbrx_error_type rfcommon_fbrx_get_cal_nv_items
( 
  rfm_mode_enum_type tech,
  rfcom_band_type_u band,
  rfnv_item_id_enum_type *nv_item_gain,  
  rfnv_item_id_enum_type *nv_item_sp,
  rfnv_item_id_enum_type *nv_item_dc,
  rfnv_item_id_enum_type *nv_item_temp
)
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_ERROR;

  switch (tech)
  {
    case RFM_IMT_MODE:
      if (band.wcdma_band < RFCOM_NUM_WCDMA_BANDS)
      {
        ret_value = rfcommon_fbrx_get_wcdma_cal_nv_id( band.wcdma_band, 
                                                       nv_item_gain, 
                                                       nv_item_sp, 
                                                       nv_item_dc, 
                                                       nv_item_temp );
      }
      break;

    case RFM_1X_MODE:
    case RFM_1XEVDO_MODE:
    case RFM_SVDO_MODE:
      if (band.cdma_band < RFM_CDMA_MAX_BAND)
      {
        ret_value = rfcommon_fbrx_get_1x_cal_nv_id( band.cdma_band, 
                                                    nv_item_gain, 
                                                    nv_item_sp, 
                                                    nv_item_dc, 
                                                    nv_item_temp );
      }
      break;

    case RFM_LTE_MODE:
    case RFM_LTE_FTM_MODE:
      if (band.lte_band < RFCOM_NUM_LTE_BANDS)
      {
        ret_value = rfcommon_fbrx_get_lte_cal_nv_id( band.lte_band, 
                                                     nv_item_gain, 
                                                     nv_item_sp, 
                                                     nv_item_dc, 
                                                     nv_item_temp );
      }
      break;

     case RFM_TDSCDMA_MODE:
     case RFM_TDSCDMA_MODE_FTM:
      if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
      {
        ret_value = rfcommon_fbrx_get_tdscdma_cal_nv_id( band.tdscdma_band, 
                                                         nv_item_gain, 
                                                         nv_item_sp, 
                                                         nv_item_dc, 
                                                         nv_item_temp );
      }
      break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_get_cal_nv_items: Invalid Tech %d for retrieving FBRx Cal NVs item code", tech);
      *nv_item_gain = RFNV_ITEMS_ENUM_MAX;
      *nv_item_sp = RFNV_ITEMS_ENUM_MAX;
      *nv_item_dc = RFNV_ITEMS_ENUM_MAX;
      *nv_item_temp = RFNV_ITEMS_ENUM_MAX;
      ret_value = RFCOMMON_FBRX_ERROR;
      break;

  } /* switch case*/

  return ret_value;

} /* rfcommon_fbrx_get_cal_nv_items */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the FBRX Droop Cal NV data for any tech.

  @param fbrx_droop_cal_nv_data
  The NV data structure where the FBRX Droop Cal data has to be loaded into

  @param fbrx_droop_cal_item_id
  The NV item that contains the droop cal data.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  RFCOMMON_FBRX_SUCCESS if the FBRX droop cal NV data is loaded
  RFCOMMON_FBRX_ERROR if NV did not load, and some more action is required by caller
*/

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_load_droop
(
  void *fbrx_droop_cal_nv_data,
  rfnv_item_id_enum_type fbrx_droop_cal_item_id,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  /* Overall status for this API */
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  /* NV read status */
  nv_stat_enum_type  nv_read_status = NV_DONE_S;
  /* NV item size (maximum) */
  uint16 nv_item_size = sizeof(rfnv_item_type); 
  /* Pointer to NV data*/
  rfnv_item_type *nv_data_ptr = NULL;
  /* Allocate memory for raw NV data */
  nv_data_ptr = (rfnv_item_type *)modem_mem_calloc(1, sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA );

  /* Null pointer check */
  if (nv_data_ptr == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_load_droop: Failed to allocate buffer memory on heap" );
    api_status = RFCOMMON_FBRX_ERROR;
    return api_status;
  }

  nv_read_status = rfcommon_rfnv_get_item( fbrx_droop_cal_item_id,
                                           nv_data_ptr,
                                           &nv_item_size,
                                           caller_tcb_ptr,
                                           task_nv_wait_sig,
                                           task_wait_func_ptr );

  if ( nv_read_status == NV_DONE_S )
  {
    /* If NV data has been successfully retrieved proceed to populating the data structure */
    memscpy( (void*)fbrx_droop_cal_nv_data, sizeof(rfcommon_fbrx_nv_droop_type), 
             (void*)nv_data_ptr, sizeof(rfcommon_fbrx_nv_droop_type) );
  } 
  else if ( nv_read_status == NV_NOTACTIVE_S )
  {
    /* If droop NV is not-active, default to all 0's */
    memset( fbrx_droop_cal_nv_data, 0, sizeof(rfcommon_fbrx_nv_droop_type));
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_load_droop: rfcommon_rfnv_get_item NOT ACTIVE " 
              "for NV = %d : default to 0 ", fbrx_droop_cal_item_id );	
    #if 0
    /* If droop NV is not-active, default to all UMTS SC Config */
    fbrx_droop_cal_nv_data->nv_version = 0;
    fbrx_droop_cal_nv_data->group_delay = 4;
    fbrx_droop_cal_nv_data->num_filter_taps = 8;
    fbrx_droop_cal_nv_data->filter_coeff = 
      {1031,-2850,415,5932,8134,4813,-621,-471,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    #endif
  }
  else
  {
    RF_MSG_2( RF_ERROR, "rfcommon_fbrx_nv_load_droop: rfcommon_rfnv_get_item request failed " 
              "for NV = %d, status = %d ", fbrx_droop_cal_item_id, nv_read_status );	
    api_status = RFCOMMON_FBRX_ERROR;
  }

  /* We are done, do the clean-up  */
  modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
  nv_data_ptr = NULL;

  return api_status;

} /* rfcommon_fbrx_nv_load_droop() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the FBRX Droop Cal NV data to the global fbrx
  struct for Droop. Load rfcommon_fbrx_nv_droop_cal_data with FBRX
  Droop Cal data

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.
*/

boolean 
rfcommon_fbrx_nv_retrieve_droop_data( rex_tcb_type* caller_tcb_ptr,
                                      rex_sigs_type task_nv_wait_sig,
                                      void (* task_wait_func_ptr)( rex_sigs_type ))
{
  rfcommon_fbrx_tx_cfg_type tx_cfg;
  rfnv_item_id_enum_type fbrx_droop_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_ERROR;
  boolean ret_val = TRUE;

  /* Get FBRX Droop data from NV for each tech */
  for (tx_cfg = RFCOMMON_FBRX_UMTS_DC; tx_cfg<RFCOMMON_FBRX_NUM_TX_CONFIG_MAX; tx_cfg++)
  {
    /* Get the fbrx droop NV ID for current Tx config BW */
    api_status = rfcommon_fbrx_get_droop_cal_nv_id( tx_cfg, &fbrx_droop_nv_id );

    if ( api_status == RFCOMMON_FBRX_SUCCESS )
    {
      /* Allocate memory to load fbrx droop cal data */
      if ( (rfcommon_fbrx_nv_droop_cal_data[tx_cfg] == NULL) || (rfcommon_fbrx_nv_droop_mem_allocated[tx_cfg] == FALSE) )
      {
        rfcommon_fbrx_nv_droop_cal_data[tx_cfg] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_droop_type), MODEM_MEM_CLIENT_RFA);
      }

      if (rfcommon_fbrx_nv_droop_cal_data[tx_cfg] != NULL)
      {
        rfcommon_fbrx_nv_droop_mem_allocated[tx_cfg] = TRUE;

        /* Load the fbrx droop data from NV to struct for current Tx config BW */
        api_status = rfcommon_fbrx_nv_load_droop( rfcommon_fbrx_nv_droop_cal_data[tx_cfg], 
                                                  fbrx_droop_nv_id, 
                                                  caller_tcb_ptr, 
                                                  task_nv_wait_sig, 
                                                  task_wait_func_ptr );
        if (api_status == RFCOMMON_FBRX_ERROR)
        {
          modem_mem_free((void*)rfcommon_fbrx_nv_droop_cal_data[tx_cfg], MODEM_MEM_CLIENT_RFA);
          rfcommon_fbrx_nv_droop_mem_allocated[tx_cfg] = FALSE; 
          ret_val = FALSE;
          return ret_val;                     
        }
      }
      else
      {
        RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_retrieve_droop_data: "
                  "Failed to allocate buffer memory on heap for Tx config = %d", tx_cfg);
        ret_val = FALSE;
        return ret_val;
      }
      
    } /* if (api_status == RFCOMMON_FBRX_SUCCESS) */
    else
    {
      RF_MSG_2( RF_ERROR, "rfcommon_fbrx_nv_retrieve_droop_data: Invalid TX Config = %d"
                " for FBRX Droop NV ID = %d ", tx_cfg, fbrx_droop_nv_id );
    }         
  } /* for tx_cfg loop */

  return ret_val;

} /* rfcommon_fbrx_nv_retrieve_droop_data */ 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_read_current_droop_globals: API for reading current FBRx Droop Filter Coeffs 
  in Global Struct

  @details
  The code gets the current FBRx Droop Filter coeffs as were loaded from NV during bootup
 
  @param tech
  tech information
 
  @param fbrx_mode_index
  FBRx Mode Index
  
  @param droop_data
  pointer to output data
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR

*/

rfcommon_fbrx_error_type rfcommon_fbrx_read_current_droop_globals
( 
  ftm_rf_technology_type ftm_tech,
  rfcom_band_type_u band,
  rfcommon_fbrx_tx_cfg_type tx_cfg,
  rfcommon_fbrx_nv_droop_type *droop_data
)
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_ERROR;
  rfcommon_fbrx_nv_lte_band_droop_params_type *lte_droop_data;
  boolean use_common_droop_db = TRUE;
  uint8 i;

  if ( droop_data == NULL)
  {
    RF_MSG(RF_ERROR, "NV read pointer is NULL");
    return ret_value;
  }

  if (tx_cfg < RFCOMMON_FBRX_NUM_TX_CONFIG_MAX)
  {
    /* Copy FBRx Droop data to FBRx DM struct */

    /** If tech is LTE, first attempt loading from the 
    band specific droop data structures **/  
    if( ftm_tech == FTM_RF_TECH_LTE )
    {
      lte_droop_data = rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.droop_data[band.lte_band];

      if( lte_droop_data != NULL )
      {
        if( lte_droop_data->num_bw_payloads_populated <= RFCOMMON_FBRX_MAX_LTE_BW_COMBOS ) 
        {
          for ( i = 0; 
                i < (lte_droop_data->num_bw_payloads_populated); 
                i++  ) 
          {
            if ( lte_droop_data->bw_params[i].bw_cfg ==  tx_cfg ) 
            {
              /* Config found for LTE so no need to use
                 the common droop structures*/
              use_common_droop_db = FALSE;

              memscpy( (void *) droop_data, 
                       sizeof(rfcommon_fbrx_nv_droop_type), 
                       (void *)( &(lte_droop_data->bw_params[i].droop_params) ), 
                       sizeof(rfcommon_fbrx_nv_droop_type) );

              //  RF_MSG_2( RF_HIGH, "Debug: rfcommon_fbrx_read_current_droop_globals: "
              //            "reading data from the LTE band:%d spedcific droop params for FBRx config:%d",
              //             band.lte_band, tx_cfg);

            }
          }
        }
        else
        {
          RF_MSG_2(RF_ERROR, "rfcommon_fbrx_nv_update_dm: Invalid droop cal data for LTE "
                             "band:%d tx_cfg = %d", band.lte_band, tx_cfg );
        }
      }
    }

    /*** If no exceptions, Use common droop data structures ***/
    if ( (rfcommon_fbrx_nv_droop_cal_data[tx_cfg] != NULL ) &&
         ( use_common_droop_db ) ) 

    {
      memscpy((void*)droop_data, sizeof(rfcommon_fbrx_nv_droop_type), 
              (void*)(rfcommon_fbrx_nv_droop_cal_data[tx_cfg]), 
               sizeof(rfcommon_fbrx_nv_droop_type));
      ret_value = RFCOMMON_FBRX_SUCCESS;
    }
    else
    {
      RF_MSG_1(RF_ERROR, "Droop cal global NV structure pointer is NULL for Tx cfg :%d", tx_cfg);
    }
  }

  return ret_value;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_override_droop_globals: API for overwriting current FBRx 
  Droop Filter Coeffs in Global Struct

  @details
  The code gets the current FBRx Droop Filter coeffs as were loaded from 
  NV during bootup
 
  @param ftm_tech
  FTM tech 

  @param band
  band union in operation
 
  @param fbrx_mode_index
  FBRx Mode Index
  
  @param droop_data
  pointer to input data
 
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR

*/
rfcommon_fbrx_error_type rfcommon_fbrx_override_droop_globals
( 
  ftm_rf_technology_type ftm_tech,
  rfcom_band_type_u band,
  rfcommon_fbrx_tx_cfg_type tx_cfg,
  rfcommon_fbrx_nv_droop_type *droop_data
)
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_ERROR;
  rfcommon_fbrx_nv_lte_band_droop_params_type *lte_droop_data;
  uint32 handle=0;
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;
  boolean droop_index_found = FALSE;

  uint8 num_bw_payloads = 0, i;    

  if ( droop_data == NULL)
  {
    RF_MSG(RF_ERROR, "NV read pointer is NULL");
    return ret_value;
  }

  /** If tech is LTE, store the droop data in band specific droop
   *  data structures **/
  if( ftm_tech == FTM_RF_TECH_LTE )
  {
    lte_droop_data = rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.droop_data[band.lte_band];

    /* Allocate memory if NV is not available */
    if( lte_droop_data == NULL ) 
    {
      lte_droop_data = modem_mem_calloc( 1, 
                                         sizeof(rfcommon_fbrx_nv_lte_band_droop_params_type), 
                                         MODEM_MEM_CLIENT_RFA );

      if( lte_droop_data != NULL ) 
      {
        /*Memset the memory */
        memset( lte_droop_data, 0, sizeof(rfcommon_fbrx_nv_lte_band_droop_params_type));
      }
    }

    if( lte_droop_data != NULL )
    {
      num_bw_payloads = lte_droop_data->num_bw_payloads_populated;

      if( num_bw_payloads < RFCOMMON_FBRX_MAX_LTE_BW_COMBOS ) 
      { 
        /* Search for the FBRx config in the array of payloads for the current 
        LTE band, if found use that index to store the overriden data */
        for ( i = 0; i < num_bw_payloads; i++  ) 
        {
          if ( lte_droop_data->bw_params[i].bw_cfg ==  tx_cfg ) 
          {
            num_bw_payloads = i; 
            droop_index_found = TRUE;
            break;
          }
        }

        if ( !droop_index_found ) 
        {
          /* Increment the num of payloads as we are occupying the next possible available 
             payload index */
          lte_droop_data->num_bw_payloads_populated++;
        }

        /* Store the Tx config at the next available payload data structure */
        lte_droop_data->bw_params[num_bw_payloads].bw_cfg = tx_cfg;

        /*Update the droop params */
        memscpy( (void*) &(lte_droop_data->bw_params[num_bw_payloads].droop_params), 
                 sizeof(rfcommon_fbrx_nv_droop_type), 
                 (void*)droop_data, 
                 sizeof(rfcommon_fbrx_nv_droop_type) ); 


        ret_value = RFCOMMON_FBRX_SUCCESS;

        // RF_MSG_3( RF_HIGH, "Debug: rfcommon_fbrx_override_droop_globals: "
        //                    "overriding data from the LTE band:%d specific droop params " 
        //                    "for FBRx config:%d at the index:%d",
        //                     band.lte_band, tx_cfg, num_bw_payloads);

      }
      else
      {
        RF_MSG_2(RF_ERROR, "rfcommon_fbrx_override_droop_globals: Invalid droop cal data for LTE "
                           "band:%d tx_cfg = %d", band.lte_band, tx_cfg );
      }
    }
    else
    {
      RF_MSG_2(RF_ERROR, "rfcommon_fbrx_override_droop_globals: NULL memory for LTE data structures "
                         "band:%d tx_cfg = %d", band.lte_band, tx_cfg );
    }
  }
  else
  {
    /*** For non LTE techs update the common droop data structures */
    if (rfcommon_fbrx_nv_droop_cal_data[tx_cfg] !=NULL)
    {
      memscpy((void*)(rfcommon_fbrx_nv_droop_cal_data[tx_cfg]), 
              sizeof(rfcommon_fbrx_nv_droop_type), 
              (void*)droop_data, 
              sizeof(rfcommon_fbrx_nv_droop_type));
      ret_value = RFCOMMON_FBRX_SUCCESS;
    }
    else
    {
      RF_MSG_1(RF_ERROR, "Droop cal global NV structure pointer is NULL for Tx cfg :%d", tx_cfg);
    }
  }

  /*Update the DM*/
  for (handle=0;handle<RFLM_HANDLE_COUNTS;handle++)
  {
    fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(handle);

    if (fbrx_data_ptr != NULL )
    {  
        memscpy( (void *)(&(fbrx_data_ptr->droop_cal_data)), 
                 sizeof(rfcommon_fbrx_nv_droop_type), 
                 (void*)droop_data, 
                 sizeof(rfcommon_fbrx_nv_droop_type) );
    }
  }

  return ret_value;
}/*rfcommon_fbrx_override_droop_globals*/

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting FBRX gain for a given channel and gain stage
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param fbrx_gain_nv_data
  Output. Destination for the extracted data.
  
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR 
*/

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_extract_gain
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  rfcommon_fbrx_nv_gain_vs_freq_type *fbrx_gain_nv_data
)
{
  /* Overall API status */
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  /* The item to extract from NV */
  rfnv_data_fbrx_gain_type fbrx_gain_data;       
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;
  uint16 channel_index;
  uint8 gain_state_index;
  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;
  /* The number of bytes needed to extract element */
  num_bytes_needed = sizeof(fbrx_gain_data);

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_extract_gain: Not enough elements left to extract" );
    api_status = RFCOMMON_FBRX_ERROR;
  }

  if( api_status == RFCOMMON_FBRX_SUCCESS )
  {
    /* Extract the element */
    memscpy( (void*) &fbrx_gain_data, sizeof(fbrx_gain_data), nv_data_ptr, num_bytes_needed );

    /* update this variable so that caller API can know how many bytes are copied in this call */
    *extracted_element_size = num_bytes_needed;

    channel_index = fbrx_gain_data.channel_index;
    gain_state_index = fbrx_gain_data.gain_state_index;

    if ( (channel_index<RFCOMMON_FBRX_NUM_CAL_CHANNELS) && (gain_state_index<RFCOMMON_FBRX_GAIN_VS_FREQ_NUM)) 
    {
      fbrx_gain_nv_data->fbrx_gain[channel_index][gain_state_index].gain = fbrx_gain_data.fbrx_gain;
      fbrx_gain_nv_data->fbrx_gain[channel_index][gain_state_index].data_valid = TRUE;         

      //RF_MSG_3( RF_HIGH,  "rfcommon_fbrx_nv_extract_gain: ch_idx=%d, gain_state_idx=%d, gain = %d", 
      //          channel_index, gain_state_index, fbrx_gain_data.fbrx_gain);
    }
    else
    {
      RF_MSG_2( RF_ERROR, "rfcommon_fbrx_nv_extract_gain: Gain index = %d or Channel index = %d out of range",
                gain_state_index, channel_index);
      api_status = RFCOMMON_FBRX_ERROR;
    }

  } /* if( api_status == RFCOMMON_FBRX_SUCCESS) */

  return api_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting FBRX gain for a given channel and gain stage
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param fbrx_gain_nv_data
  Output. Destination for the extracted data.
  
  @return
  RFCOMMON_FBRX_SUCCESS or RFCOMMON_FBRX_ERROR 
*/

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_extract_gain_v2
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  rfcommon_fbrx_nv_gain_vs_freq_type_v2 *fbrx_gain_nv_data
)
{
  /* Overall API status */
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  /* The item to extract from NV */
  rfnv_data_fbrx_gain_type_v2 fbrx_gain_data;
       
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;
  uint16 chan_idx = 0;
  uint8 gain_idx = 0;
  uint8 mode_idx = 0;

  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* The number of bytes needed to extract element */
  num_bytes_needed = sizeof(fbrx_gain_data);

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_extract_gain_v2: Not enough elements left to extract");
    api_status = RFCOMMON_FBRX_ERROR;
  }

  if( api_status == RFCOMMON_FBRX_SUCCESS )
  {
    /* Extract the element */
    memscpy( (void*) &fbrx_gain_data, sizeof(fbrx_gain_data), nv_data_ptr, num_bytes_needed );

    /* update this variable so that caller API can know how many bytes are copied in this call */
    *extracted_element_size = num_bytes_needed;

    chan_idx = fbrx_gain_data.channel_index;
    mode_idx = fbrx_gain_data.fbrx_mode_index;
    gain_idx = fbrx_gain_data.gain_state_index;

    if ( (chan_idx < RFCOMMON_FBRX_NUM_CAL_CHANNELS) && 
         (mode_idx < RFCOMMON_FBRX_NUM_MODES) && 
         (gain_idx < RFCOMMON_FBRX_GAIN_VS_FREQ_NUM_V2) ) 
    {
      fbrx_gain_nv_data->fbrx_gain[chan_idx][mode_idx][gain_idx].gain = fbrx_gain_data.fbrx_gain;
      fbrx_gain_nv_data->fbrx_gain[chan_idx][mode_idx][gain_idx].data_valid = TRUE;         
      fbrx_gain_nv_data->fbrx_gain[chan_idx][mode_idx][gain_idx].tx_power = fbrx_gain_data.tx_power;
      fbrx_gain_nv_data->fbrx_gain[chan_idx][mode_idx][gain_idx].ubias = fbrx_gain_data.ubias;
      fbrx_gain_nv_data->fbrx_gain[chan_idx][mode_idx][gain_idx].rx_scale = fbrx_gain_data.rx_scale;
      fbrx_gain_nv_data->fbrx_gain[chan_idx][mode_idx][gain_idx].predet_index = fbrx_gain_data.predet_index;

      //RF_MSG_4( RF_HIGH,  "rfcommon_nv_extract_fbrx_gain_v2: ch_idx=%d, gain_state_idx=%d, gain = %d, tx_pwr = %d", 
      //     chan_idx, gain_idx, fbrx_gain_data.fbrx_gain, fbrx_gain_data.tx_power);
    }
    else
    {
      RF_MSG_2( RF_ERROR, "rfcommon_fbrx_nv_extract_gain_v2: Gain index = %d or Channel index = %d out of range",
                gain_idx, chan_idx);
      api_status = RFCOMMON_FBRX_ERROR;
    }

  } /* if( api_status == TRUE ) */

  return api_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the FBRX gain v.s. freq related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the FBRX gain v.s. freq data for that band.

  @param fbrx_gain_nv_data
  The fbrx_gain_nv_data NV data structure where the cal data has to be loaded into

  @param fbrx_gain_nv_id
  The NV item that contains the cal value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  RFCOMMON_FBRX_SUCCESS if the FBRX gain v.s. freq value is loaded
  RFCOMMON_FBRX_ERROR if NV did not load, and some more action is required by caller
*/

/* Size of Fixed part (33 bytes) and variant header (4 bytes), total 37 bytes */ 
#define RFCOMMON_FBRX_GAIN_NV_FIXED_SIZE  (33 + sizeof(rfnv_data_variant_marker_type))

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_load_gain
(
  void *fbrx_gain_nv_data,
  rfnv_item_id_enum_type fbrx_gain_nv_id,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  /* Overall status for this API */
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  /* NV read status */
  nv_stat_enum_type  fbrx_gain_nv_read_status = NV_DONE_S;
  /* NV item size (maximum) */
  uint16 nv_item_size = sizeof(rfnv_item_type);
  /* Pointer to NV data*/
  rfnv_item_type *nv_data_ptr = NULL;
  /* Helper ptr to hold moving ptr location during data copy*/
  uint8     *temp_data_ptr = NULL;
  /* actual size of memory read from the NV*/
  rfnv_item_size_type   fbrx_gain_nv_read_size = sizeof(rfnv_item_type);
  /* Contains the number of bytes for an element in NV */
  uint32 nv_element_size;
  /* Keeps track of the running total of number of bytes already parsed */
  uint32 num_bytes_parsed = 0;
  /* contains no of type_ids found in NV*/
  uint16 num_type_id = 0;
  /* contains the version number from NV*/
  uint16 nv_version = 0;
  /* Used for looping through TYPE IDs */
  uint16 type_id_idx;
  /* The TYPE ID of the current element */
  uint16 type_id;
  /* Helper */
  uint32 temp_num_bytes_left;

  /* Allocate memory for raw NV data */
  nv_data_ptr = (rfnv_item_type *)modem_mem_calloc(1, sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA );

  /* Null pointer check */
  if (nv_data_ptr == NULL)
  {
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_load_gain: Failed to allocate buffer memory on heap for %d bytes", nv_item_size);
    api_status = RFCOMMON_FBRX_ERROR;
    return api_status;
  }

  fbrx_gain_nv_read_status = rfcommon_rfnv_get_item( fbrx_gain_nv_id,
                                                     nv_data_ptr,
                                                     &fbrx_gain_nv_read_size,
                                                     caller_tcb_ptr,
                                                     task_nv_wait_sig,
                                                     task_wait_func_ptr );

  /* If NV data has been successfully retrieved proceed to populating the data structure */
  if ( fbrx_gain_nv_read_status == NV_DONE_S )
  {
    temp_data_ptr = (uint8 *)nv_data_ptr;
    /* copy data for fixed part and variant header of the NV, need to adjust size*/
    nv_element_size = RFCOMMON_FBRX_GAIN_NV_FIXED_SIZE;

    memscpy((void *)fbrx_gain_nv_data, nv_element_size, (void*)temp_data_ptr, nv_element_size );

    /* Update running total of bytes already parsed */
    num_bytes_parsed += nv_element_size;

    /* now we know no of type_ids and version present in the NV, copy it*/
    num_type_id = ((rfcommon_fbrx_nv_gain_vs_freq_type *)fbrx_gain_nv_data)->rfnv_data_variant_marker.num_elements;
    nv_version = ((rfcommon_fbrx_nv_gain_vs_freq_type *)fbrx_gain_nv_data)->rfnv_data_variant_marker.version;
    //RF_MSG_2( RF_HIGH, "rfcommon_fbrx_nv_load_gain: No. of dynamic elements=%d, version=%d",num_type_id,nv_version);

    /* copy all data nodes one by one*/
    for( type_id_idx = 0; (type_id_idx < num_type_id) ; type_id_idx++ )
    {
      /* Get type ID of the subsequent element*/
      nv_element_size = RFNV_DATA_TYPE_ID_SIZE;

      /* Ensure there is enough memory left before actually extracting the element */
      if((fbrx_gain_nv_read_size - num_bytes_parsed) < nv_element_size )
      {
        RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_load_gain: Not enough memory left " 
                  "to extract nv element of size = %d", nv_element_size );
        api_status = RFCOMMON_FBRX_ERROR;
        break;
      }
   
      /* EXTRACT ELEMENT: Type ID */
      if (num_bytes_parsed < RFNV_DATA_SIZE_MAX)
      {
      temp_data_ptr = (uint8 *)nv_data_ptr + num_bytes_parsed;
      }

      else
      {
        RF_MSG( RF_HIGH, "rfcommon_fbrx_nv_load_gain: Number of bytes parsed is greater than RFNV_DATA_SIZE_MAX ");
      }
    
      /* Check that we are not going to overflow the buffer */
      if ( (num_bytes_parsed + nv_element_size) >= RFNV_DATA_SIZE_MAX)
      {
        RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_load_gain: Memory exhausted" );
        api_status = RFCOMMON_FBRX_ERROR;
        break;
      }

      memscpy( (void*) &type_id, sizeof(type_id), (void*)temp_data_ptr, nv_element_size );

      /* Update running total of bytes parsed with num bytes from last element extracted */
      num_bytes_parsed += nv_element_size;
      
      temp_num_bytes_left = fbrx_gain_nv_read_size - num_bytes_parsed;

      if( temp_num_bytes_left <= 0 )
      {
        RF_MSG_2( RF_ERROR, "rfcommon_fbrx_nv_load_gain: No elements left to extract "
                  "item of TYPE_ID = %d for type index %d", type_id, type_id_idx);
        api_status = RFCOMMON_FBRX_ERROR;
        break;
      }

      /* Make sure we haven't overflowed the buffer */
      num_bytes_parsed = MIN(num_bytes_parsed,RFNV_DATA_SIZE_MAX);

     /* EXTRACT ELEMENT corresponding to the this type ID */
      temp_data_ptr = (uint8 *)nv_data_ptr + num_bytes_parsed;
      
      switch( type_id )
      {
        case RFNV_DATA_TYPE_FBRX_GAIN:
          if (nv_version == 0)
          {
            api_status = rfcommon_fbrx_nv_extract_gain( temp_data_ptr,
                                                        temp_num_bytes_left,
                                                        &nv_element_size,
                                                        (rfcommon_fbrx_nv_gain_vs_freq_type*)fbrx_gain_nv_data );  
          }
          else
          {
            RF_MSG_3( RF_ERROR, "rfcommon_fbrx_nv_load_gain: Element of type ID %d not supported "
                      "for type index %d for version %d", type_id, type_id_idx, nv_version);
            api_status = RFCOMMON_FBRX_ERROR;
            break;
          }
        break;

        case RFNV_DATA_TYPE_FBRX_GAIN_V2:
          if (nv_version == 1)
          {
            api_status = rfcommon_fbrx_nv_extract_gain_v2(temp_data_ptr,
                                                          temp_num_bytes_left,
                                                          &nv_element_size,
                                                          (rfcommon_fbrx_nv_gain_vs_freq_type_v2*)fbrx_gain_nv_data );  
          }
          else
          {
            RF_MSG_3( RF_ERROR, "rfcommon_fbrx_nv_load_gain: Element of type ID %d not supported "
                      "for type index %d for version %d", type_id, type_id_idx, nv_version);
            api_status = RFCOMMON_FBRX_ERROR;
            break;
          }
        break;
       
        default:
          RF_MSG_2( RF_ERROR, "rfcommon_fbrx_nv_load_gain: Element of type ID %d not supported "
                    "for type index %d", type_id, type_id_idx);
          nv_element_size = 0;
          api_status = RFCOMMON_FBRX_ERROR;
          break;
      } /* end switch case*/

      /* Update running total of bytes parsed with num bytes from last element extracted */
      num_bytes_parsed += nv_element_size;
    } /* end of for loop*/
  } /* end if (fbrx_gain_nv_read_status == NV_DONE_S ) */
  else if ( fbrx_gain_nv_read_status == NV_NOTACTIVE_S  )
  {
    memset( fbrx_gain_nv_data, 0, sizeof(rfcommon_fbrx_nv_gain_vs_freq_type_v2) );
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_load_gain: rfcommon_rfnv_get_item NOT ACTIVE " 
              "for NV = %d : default to 0", fbrx_gain_nv_id);
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_load_gain: rfcommon_rfnv_get_item failed for NV = %d", fbrx_gain_nv_id);
    api_status = RFCOMMON_FBRX_ERROR;
  }

  modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
  nv_data_ptr = NULL;

  return api_status;

} /* rfcommon_fbrx_nv_load_gain */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load the FBRX gain v.s. freq related data for any band.

  @details
  Checks that the NV item for the version, type-ID supported. 
  If all the checks look good, then droop cal data of each BW is loaded one 
  after another to the data structure pointer provided to this API.

  @param raw_nv_data_ptr
  Pointer to the raw/uncompressed droop cal data for various bandwidths for the 
  current band of interest

  @param fbrx_droop_cal_nv_read_size
  Size of the raw/uncompressed droop cal data for various bandwidths being passed

  @param fbrx_droop_nv_item_id
  The NV item ID for which the droop cal data is provided

  @param fbrx_lte_droop_nv_data
  Pointer to the data structure where the bandwidth specific droop cal data
  has to be loaded into
  
  @return
  RFCOMMON_FBRX_SUCCESS if the FBRX gain v.s. freq value is loaded
  RFCOMMON_FBRX_ERROR if NV did not load, and some more action is required by caller
*/

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_extract_droop_bw_params
(
  rfnv_item_type *raw_nv_data_ptr,
  rfnv_item_size_type  fbrx_droop_cal_nv_read_size,
  rfnv_item_id_enum_type fbrx_lte_droop_nv_item_id,
  rfcommon_fbrx_nv_lte_band_droop_params_type *fbrx_lte_droop_nv_data
)
{
  /* Overall status for this API */
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  /* NV item size (maximum) */
  uint16 nv_item_size = sizeof(rfnv_item_type);
  /* Helper ptr to hold moving ptr location during data copy*/
  uint8     *temp_data_ptr = NULL;
  /* Contains the number of bytes for an element in NV */
  uint32 nv_element_size;
  /* Keeps track of the running total of number of bytes already parsed */
  uint32 num_bytes_parsed = 0;
  /* contains no of type_ids found in NV*/
  uint16 num_type_id = 0;
  /* contains the version number from NV*/
  uint16 nv_version = 0;
  /* Used for looping through TYPE IDs */
  uint16 type_id_idx;
  /* The TYPE ID of the current element */
  uint16 type_id;
  /* Helper */
  uint32 temp_num_bytes_left;
  /*Variant marker data*/
  rfnv_data_variant_marker_type variant_marker_data;

  /* Null pointer check */
  if ( (fbrx_lte_droop_nv_data == NULL) || (raw_nv_data_ptr == NULL) )
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_load_lte_band_droop_params: NULL pointer passed! ");
    api_status = RFCOMMON_FBRX_ERROR;
    return api_status;
  }

  temp_data_ptr = (uint8 *)raw_nv_data_ptr;

  /* copy data for variant header of the NV */
  nv_element_size = sizeof(rfnv_data_variant_marker_type);

  memscpy((void *)&variant_marker_data, nv_element_size, (void*)temp_data_ptr, nv_element_size );

  /* Update running total of bytes already parsed */
  num_bytes_parsed += nv_element_size;

  /* Now we know number of type_ids and version present in the NV, copy it*/
  num_type_id = variant_marker_data.num_elements;
  nv_version = variant_marker_data.version;

  // RF_MSG_3( RF_HIGH, "Debug: rfcommon_fbrx_nv_load_lte_band_droop_params: "
  //                    "NVID:%d No. of dynamic elements=%d, version=%d",
  //                     fbrx_lte_droop_nv_item_id, num_type_id, nv_version);

  /* Sanitize the number of elements */
  if ( num_type_id > RFCOMMON_FBRX_MAX_LTE_BW_COMBOS) 
  {
      RF_MSG_2( RF_ERROR, "rfcommon_fbrx_nv_load_lte_band_droop_params"
                          "Invalid number of BWs populated:%d as the number exceeds the max allowed:%d! ", 
                          num_type_id, RFCOMMON_FBRX_MAX_LTE_BW_COMBOS );
      api_status = RFCOMMON_FBRX_ERROR;
  }

  /* Copy all LTE BW specific droop data one by one*/
  for( type_id_idx = 0; 
       (type_id_idx < num_type_id) && (api_status != RFCOMMON_FBRX_ERROR) ; 
       type_id_idx++ )
  {
    /* Get type ID of the subsequent element*/
    nv_element_size = RFNV_DATA_TYPE_ID_SIZE;

    /* Ensure there is enough memory left before actually extracting the element */
    if((fbrx_droop_cal_nv_read_size - num_bytes_parsed) < nv_element_size )
    {
      RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_load_lte_band_droop_params: "
                          "Not enough memory left to extract nv element "
                          "of size = %d", nv_element_size );
      api_status = RFCOMMON_FBRX_ERROR;
      break;
    }
 
    /* EXTRACT ELEMENT: Type ID */
    temp_data_ptr = (uint8 *)raw_nv_data_ptr + num_bytes_parsed;
  
    /* Check that we are not going to overflow the buffer */
    if ( (num_bytes_parsed + nv_element_size) >= RFNV_DATA_SIZE_MAX)
    {
      RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_load_lte_band_droop_params: Memory exhausted" );
      api_status = RFCOMMON_FBRX_ERROR;
      break;
    }

    memscpy( (void*) &type_id, sizeof(type_id), (void*)temp_data_ptr, nv_element_size );

    /* Update running total of bytes parsed with num bytes from last element extracted */
    num_bytes_parsed += nv_element_size;
    
    temp_num_bytes_left = fbrx_droop_cal_nv_read_size - num_bytes_parsed;

    if( temp_num_bytes_left <= 0 )
    {
      RF_MSG_2( RF_ERROR, "rfcommon_fbrx_nv_load_lte_band_droop_params: No elements left to extract "
                "item of TYPE_ID = %d for type index %d", type_id, type_id_idx);
      api_status = RFCOMMON_FBRX_ERROR;
      break;
    }

    /* Make sure we haven't overflowed the buffer */
    num_bytes_parsed = MIN(num_bytes_parsed,RFNV_DATA_SIZE_MAX);

   /* EXTRACT ELEMENT corresponding to the this type ID */
    temp_data_ptr = (uint8 *)raw_nv_data_ptr + num_bytes_parsed;
    
    /*Defining a switch-case for various type-IDs for future expansion*/
    switch( type_id )
    {
      case RFNV_DATA_TYPE_ID_LTE_FBRX_DROOP_FIR_FILTER_COEFF:
        if (nv_version == 0)
        {
          /* The number of bytes needed to extract element */
          nv_element_size = sizeof(rfcommon_fbrx_nv_lte_bw_droop_params_type);

          /* Copy the BW droop specific droop data in the next location of array */
          memscpy( (void*) &(fbrx_lte_droop_nv_data->bw_params[type_id_idx]), 
                   nv_element_size, 
                   (void*) temp_data_ptr, 
                   nv_element_size );
        }
        else
        {
          RF_MSG_3( RF_ERROR, "rfcommon_fbrx_nv_load_lte_band_droop_params: Element of type ID %d not supported "
                    "for type index %d for version %d", type_id, type_id_idx, nv_version);
          api_status = RFCOMMON_FBRX_ERROR;
          break;
        }
      break;

    
      default:
        RF_MSG_2( RF_ERROR, "rfcommon_fbrx_nv_load_lte_band_droop_params: Element of type ID %d not supported "
                  "for type index %d", type_id, type_id_idx);
        nv_element_size = 0;
        api_status = RFCOMMON_FBRX_ERROR;
        break;
    } /* end switch case*/

    /* Update running total of bytes parsed with num bytes from last element extracted */
    num_bytes_parsed += nv_element_size;

  } /* end of for loop*/

  /*Update the total number of bandwidths populated only if there are no issues */
  if ( api_status != RFCOMMON_FBRX_ERROR ) 
  {
    fbrx_lte_droop_nv_data->num_bw_payloads_populated = num_type_id;
  }
  else
  {
    fbrx_lte_droop_nv_data->num_bw_payloads_populated = 0;
  }

  return api_status;
} /* rfcommon_fbrx_nv_load_droop_params */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the FBRX attn switch points data for any band.


  @param fbrx_attn_switch_pts_nv_data
  The rfcommon_fbrx_nv_attn_switch_pt_type NV data structure where the cal data has to be loaded into

  @param fbrx_attn_switch_pts_nv_id
  The NV item that contains the cal value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  RFCOMMON_FBRX_SUCCESS if the FBRX attn switch points value is loaded
  RFCOMMON_FBRX_ERROR if NV did not load, and some more action is required by caller
*/

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_load_attn_switch_pts
(
  rfcommon_fbrx_nv_attn_switch_pt_type *fbrx_attn_switch_pts_nv_data,
  rfnv_item_id_enum_type fbrx_attn_switch_pts_nv_id,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  /* Overall status for this API */
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  /* NV read status */
  nv_stat_enum_type  nv_read_status = NV_DONE_S;
  /* Pointer to NV data*/
  rfnv_item_type *nv_data_ptr = NULL;
  /* actual size of memory read from the NV*/
  rfnv_item_size_type nv_read_size = sizeof(rfnv_item_type);

  int16 switch_pwr_dbm[RFCOMMON_FBRX_ATT_SWITCH_PTS_NUM] = {-260,-30,300,0,0,0,0,0,0,0};

  /* Allocate memory for raw NV data */
  nv_data_ptr = (rfnv_item_type *)modem_mem_calloc(1, sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA );

  /* Null pointer check */
  if (nv_data_ptr == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_load_attn_switch_pts: Failed to allocate buffer memory on heap" );
    api_status = RFCOMMON_FBRX_ERROR;
    return api_status;
  }

  nv_read_status = rfcommon_rfnv_get_item( fbrx_attn_switch_pts_nv_id,
                                           nv_data_ptr,
                                           &nv_read_size,
                                           caller_tcb_ptr,
                                           task_nv_wait_sig,
                                           task_wait_func_ptr );

  if ( nv_read_status == NV_DONE_S )
  {
    /* If NV data has been successfully retrieved proceed to populating the data structure */
    memscpy((void*)fbrx_attn_switch_pts_nv_data, sizeof(rfcommon_fbrx_nv_attn_switch_pt_type), 
            (void*)nv_data_ptr, sizeof(rfcommon_fbrx_nv_attn_switch_pt_type) );
  } 
  else if ( nv_read_status == NV_NOTACTIVE_S )
  {
    fbrx_attn_switch_pts_nv_data->num_switch_pts = 3;
    memscpy((void*)&(fbrx_attn_switch_pts_nv_data->switch_pwr_dbm[0]), RFCOMMON_FBRX_ATT_SWITCH_PTS_NUM*sizeof(int16), 
            (void*)&(switch_pwr_dbm[0]), RFCOMMON_FBRX_ATT_SWITCH_PTS_NUM*sizeof(int16) );
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_load_attn_switch_pts: rfcommon_rfnv_get_item NOT ACTIVE " 
              "for NV = %d : default to 3,-260,-30,300,0... ", fbrx_attn_switch_pts_nv_id);	
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_load_attn_switch_pts: rfcommon_rfnv_get_item "  
              "failed for NV = %d", fbrx_attn_switch_pts_nv_id );	
    api_status = RFCOMMON_FBRX_ERROR;
  }

  /* We are done, do the clean-up  */
  modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
  nv_data_ptr = NULL;

  return api_status;

} /* rfcommon_fbrx_nv_load_attn_switch_pts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the FBRX DC Cal NV data for any tech.

  @param fbrx_dc_cal_nv_data
  The NV data structure where the FBRX DC Cal data has to be loaded into

  @param fbrx_dc_cal_item_id
  The NV item that contains the dc cal data.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  RFCOMMON_FBRX_SUCCESS if the FBRX DC Cal NV data is loaded
  RFCOMMON_FBRX_ERROR if NV did not load, and some more action is required by caller
*/

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_load_dc
(
  void *fbrx_dc_cal_nv_data,
  rfnv_item_id_enum_type fbrx_dc_cal_item_id,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  /* Overall status for this API */
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  /* NV read status */
  nv_stat_enum_type  nv_read_status = NV_DONE_S;
  /* NV item size (maximum) */
  uint16 nv_item_size = sizeof(rfnv_item_type); 
  /* Pointer to NV data*/
  rfnv_item_type *nv_data_ptr = NULL;

  /* Allocate memory for raw NV data */
  nv_data_ptr = (rfnv_item_type *)modem_mem_calloc(1, sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA );

  /* Null pointer check */
  if (nv_data_ptr == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_load_dc: failed to allocate buffer memory on heap");
    api_status = RFCOMMON_FBRX_ERROR;
    return api_status;
  }

  nv_read_status = rfcommon_rfnv_get_item( fbrx_dc_cal_item_id,
                                           nv_data_ptr,
                                           &nv_item_size,
                                           caller_tcb_ptr,
                                           task_nv_wait_sig,
                                           task_wait_func_ptr );

  if ( nv_read_status == NV_DONE_S )
  {
    /* If NV data has been successfully retrieved proceed to populating the data structure */
    memscpy((void*)fbrx_dc_cal_nv_data, sizeof(rfcommon_fbrx_nv_dc_type), 
            (void*)nv_data_ptr, sizeof(rfcommon_fbrx_nv_dc_type) );
  }
  else if ( nv_read_status == NV_NOTACTIVE_S )
  {
    /* Non critical NV, If DC NV is not-active, default to all 0's */
    memset( fbrx_dc_cal_nv_data, 0, sizeof(rfcommon_fbrx_nv_dc_type));
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_load_dc: rfcommon_rfnv_get_item NOT ACTIVE " 
              "for NV = %d : default to 0", fbrx_dc_cal_item_id);	
  }
  else
  {    
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_load_dc: rfcommon_rfnv_get_item failed for NV=%d", fbrx_dc_cal_item_id);	
    api_status = RFCOMMON_FBRX_ERROR;
  }

  /* We are done, do the clean-up  */
  modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
  nv_data_ptr = NULL;

  return api_status;

} /* rfcommon_fbrx_nv_load_dc() */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the FBRX Temp Comp Cal NV data for any tech.

  @param fbrx_temp_comp_cal_nv_data
  The NV data structure where the FBRX DC Cal data has to be loaded into

  @param fbrx_temp_comp_cal_item_id
  The NV item that contains the dc cal data.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  RFCOMMON_FBRX_SUCCESS if the FBRX temp_comp Cal NV data is loaded
  RFCOMMON_FBRX_ERROR if NV did not load, and some more action is required by caller
*/

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_load_temp
(
  void *fbrx_temp_comp_cal_nv_data,
  rfnv_item_id_enum_type fbrx_temp_comp_cal_item_id,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  /* Overall status for this API */
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  /* NV read status */
  nv_stat_enum_type  nv_read_status = NV_DONE_S;
  /* NV item size (maximum) */
  uint16 nv_item_size = sizeof(rfnv_item_type); 
  /* Pointer to NV data*/
  rfnv_item_type *nv_data_ptr = NULL;
  /* FBRX temp comp data structure pointer */
  rfnv_item_type *nv_temp_cal_ptr = NULL;
  uint32 uncompress_size;
  /* NV decompressino status */
  int8 uncompress_status = Z_ERRNO;

  /* Allocate memory for raw NV data */
  nv_data_ptr = (rfnv_item_type *)modem_mem_calloc(1, sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA );

  /* Null pointer check */
  if (nv_data_ptr == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_load_temp: Failed to allocate buffer memory on heap");
    api_status = RFCOMMON_FBRX_ERROR;
    return api_status;
  }

  nv_read_status = rfcommon_rfnv_get_item( fbrx_temp_comp_cal_item_id,
                                           nv_data_ptr,
                                           &nv_item_size,
                                           caller_tcb_ptr,
                                           task_nv_wait_sig,
                                           task_wait_func_ptr );

  if ( nv_read_status == NV_DONE_S )
  {
     RF_MSG_2( MSG_LEGACY_HIGH, "rfcommon_fbrx_nv_load_temp: NV # %d read successful, size = %d", fbrx_temp_comp_cal_item_id, nv_item_size );

     // check for NV Id is compressed or not
     if (rfcommon_rfnv_chk_item_compressed(fbrx_temp_comp_cal_item_id))
     {
       /* if it is compressed data, need to uncompressed data */
       /* Allocate memory for NV data decompression. Allocate memeory using NV item type for temp_cal nv data type, using tmp type may cause a problem */
       nv_temp_cal_ptr = (rfnv_item_type *)modem_mem_alloc(sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA);

       if (nv_temp_cal_ptr != NULL)
       {
         /* initialize uncompress size to the maximum uncompress nv item size */
         uncompress_size = sizeof(rfnv_item_type);
		 
         /* Decompress NV data */
         uncompress_status = uncompress((uint8 *)nv_temp_cal_ptr,   
                                        &uncompress_size,
                                        (const uint8 *)nv_data_ptr, 
                                        (uint32)nv_item_size);

         if (uncompress_status != Z_OK)
         {
           RF_MSG_2( RF_ERROR, "decompression of rfcommon_fbrx_nv_load_temp NV # %d failed!status:%d", fbrx_temp_comp_cal_item_id, uncompress_status ); 
           /* Free the memory for decompressed NV data */
           modem_mem_free((void*)nv_temp_cal_ptr, MODEM_MEM_CLIENT_RFA);
           nv_read_status = NV_FAIL_S;  
         }
         else
         {
           RF_MSG_2( MSG_LEGACY_HIGH, "rfcommon_fbrx_nv_load_temp: NV # %d decompressed successful, size = %d", fbrx_temp_comp_cal_item_id, uncompress_size );
           /* Free the memory for raw NV data*/
           modem_mem_free((void*)nv_data_ptr, MODEM_MEM_CLIENT_RFA);

           /* Reuse the nv_data_ptr for the uncompress nv data */
           nv_data_ptr = (rfnv_item_type *)nv_temp_cal_ptr;
           nv_read_status = NV_DONE_S;
         }
       } /* nv_temp_cal_ptr */
       else
       {
         RF_MSG_1( RF_ERROR, "Fail to allocate memory for decompressed FBRX temp static NV %d!", fbrx_temp_comp_cal_item_id); 
         nv_read_status = NV_NOMEM_S;
       } /* nv_temp_cal_ptr - else */
     }

    if (nv_read_status == NV_DONE_S)
    {
      /* If NV data has been successfully retrieved proceed to populating the data structure */
      memscpy((void *)fbrx_temp_comp_cal_nv_data, sizeof(rfcommon_fbrx_nv_temp_type), 
              (void*)nv_data_ptr, sizeof(rfcommon_fbrx_nv_temp_type) );
    } 
    else
    {
      api_status = RFCOMMON_FBRX_ERROR;
    }	 
  } 

  else if ( nv_read_status == NV_NOTACTIVE_S )
  {
    /* Non critical NV, If Temp Comp NV is not-active, default to all 0's */
    memset( fbrx_temp_comp_cal_nv_data, 0, sizeof(rfcommon_fbrx_nv_dc_type));
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_load_temp: rfcommon_rfnv_get_item NOT ACTIVE "
                        "for NV = %d : default to 0", fbrx_temp_comp_cal_item_id);	

    /* The tech team interface reads compressed NVs first, so only set the api_status to error if compressed NV is not present and the uncompressed NVs will be read; 
       for uncompressed NVs, they can be unpresent due to FBRX not supported */
    if (rfcommon_rfnv_chk_item_compressed(fbrx_temp_comp_cal_item_id))
    {
      api_status = RFCOMMON_FBRX_ERROR;	
    }
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_load_temp: rfcommon_rfnv_get_item failed for NV=%d", fbrx_temp_comp_cal_item_id);	
  	api_status = RFCOMMON_FBRX_ERROR;
  }

  /* We are done, do the clean-up  */
  modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
  nv_data_ptr = NULL;

  return api_status;

} /* rfcommon_fbrx_nv_load_temp() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to specifically load the compressed NVs for FBRX LTE band specific
  droop cal NV data

  @param fbrx_lte_band_droop_params_nv_data
  The NV data structure where the FBRX DC Cal data has to be loaded into

  @param fbrx_temp_comp_cal_item_id
  The NV item that contains the dc cal data.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  RFCOMMON_FBRX_SUCCESS if the FBRX temp_comp Cal NV data is loaded
  RFCOMMON_FBRX_ERROR if NV did not load, and some more action is required by caller
*/

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_load_lte_band_droop_params
(
  rfcommon_fbrx_nv_lte_band_droop_params_type *fbrx_lte_band_droop_params_nv_data,
  rfnv_item_id_enum_type fbrx_droop_nv_item_id,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  /* Overall status for this API */
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  /* NV read status */
  nv_stat_enum_type  nv_read_status = NV_DONE_S;
  /* NV item size (maximum) */
  uint16 nv_item_size = sizeof(rfnv_item_type); 
  /* Pointer to NV data*/
  rfnv_item_type *nv_data_ptr = NULL;
  /* FBRX LTE band droop params data structure pointer */
  rfnv_item_type *nv_lte_band_droop_params_ptr = NULL;
  uint32 uncompress_size;
  /* NV decompressino status */
  int8 uncompress_status = Z_ERRNO;

  /* Allocate memory for raw NV data */
  nv_data_ptr = (rfnv_item_type *)modem_mem_calloc(1, sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA );

  /* Null pointer check */
  if (nv_data_ptr == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_load_lte_band_droop_params: Failed to allocate buffer memory on heap");
    api_status = RFCOMMON_FBRX_ERROR;
    return api_status;
  }

  nv_read_status = rfcommon_rfnv_get_item( fbrx_droop_nv_item_id,
                                           nv_data_ptr,
                                           &nv_item_size,
                                           caller_tcb_ptr,
                                           task_nv_wait_sig,
                                           task_wait_func_ptr );

  if ( nv_read_status == NV_DONE_S )
  {
    /*** As LTE droop NVs are compressed data, need to uncompressed data ***/

    /* Allocate memory for NV data decompression.
       Allocate memeory using NV item type for uncompression.
       Using tmp type may cause a problem */
    nv_lte_band_droop_params_ptr = (rfnv_item_type *)modem_mem_alloc(sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA);
   
    if (nv_lte_band_droop_params_ptr != NULL)
    {
      uncompress_size = sizeof(rfnv_item_type); /*Need to specify the size of the destination ptr */

      /* Decompress NV data */
      uncompress_status = uncompress((uint8 *)nv_lte_band_droop_params_ptr,   
                                     &uncompress_size,
                                     (const uint8 *)nv_data_ptr, 
                                     (uint32)nv_item_size);
   
      if (uncompress_status != Z_OK)
      {
        RF_MSG_4( RF_ERROR, "decompression of rfcommon_fbrx_nv_load_lte_band_droop_params"
                            " NV #%d failed!status:%d, uncompressed_size: %d, "
                            "NV Data ptr size: %d", fbrx_droop_nv_item_id, 
                             uncompress_status, uncompress_size, nv_item_size ); 

        nv_read_status = NV_FAIL_S;  
      }
      else
      {
        RF_MSG_2( MSG_LEGACY_HIGH, "rfcommon_fbrx_nv_load_lte_band_droop_params:NV # %d decompressed "
                                   "successful, size = %d", fbrx_droop_nv_item_id, uncompress_size );
   
        nv_read_status = NV_DONE_S;
      }
    } /* nv_temp_cal_ptr */
    else
    {
      RF_MSG_1( RF_ERROR, "Fail to allocate memory for decompressed FBRX LTE droop static NV %d!", fbrx_droop_nv_item_id); 
      api_status = RFCOMMON_FBRX_ERROR;
    } /* nv_temp_cal_ptr - else */


    if(  ( api_status == RFCOMMON_FBRX_SUCCESS ) &&
         ( nv_read_status == NV_DONE_S ) )
    {
      /*********************************************************************************** 
        NV data has been successfully decompressed, start filling up the droop
        data structures
      ***********************************************************************************/
      api_status = rfcommon_fbrx_nv_extract_droop_bw_params( nv_lte_band_droop_params_ptr,
                                                             uncompress_size,
                                                             fbrx_droop_nv_item_id,
                                                             fbrx_lte_band_droop_params_nv_data );
    }

    /* Free the memory for decompressed NV data */
    modem_mem_free((void*)nv_lte_band_droop_params_ptr, MODEM_MEM_CLIENT_RFA);
  }
  else
  {
    api_status = RFCOMMON_FBRX_ERROR;

    RF_MSG_2( RF_ERROR, "rfcommon_fbrx_nv_load_temp: rfcommon_rfnv_get_item NOT ACTIVE "
                        "for NV = %d and with NV read status of: %d hence freeing up "
                        "the memory", fbrx_droop_nv_item_id, nv_read_status);	
  }

  /* We are done, do the memory clean-up  */
  modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
  nv_data_ptr = NULL;

  return api_status;

} /* rfcommon_fbrx_nv_load_lte_band_droop_params() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to allocate memory for global fbrx
  struct to strore FBRX Cal NV data 
 
  @param tech
  Tech info for which the FBRX DM is updated.
  
  @param band
  Band info for which the FBRX DM is updated.

*/

boolean 
rfcommon_fbrx_nv_allocate_mem ( rfm_mode_enum_type tech,
                                rfcom_band_type_u band  )
{

  boolean ret_val = TRUE;

  switch (tech)
  {
    case RFM_IMT_MODE:
      if (band.wcdma_band < RFCOM_NUM_WCDMA_BANDS)
      {
        /* Allocate memory to load fbrx gain cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.gain_data[band.wcdma_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.gain_data[band.wcdma_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_gain_vs_freq_type_v2), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load fbrx sp cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.sp_data[band.wcdma_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.sp_data[band.wcdma_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_attn_switch_pt_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load fbrx dc cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band.wcdma_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band.wcdma_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_dc_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load fbrx temp comp cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.temp_comp_data[band.wcdma_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.temp_comp_data[band.wcdma_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_temp_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Check if memory was successfully allocated or not to store FBRx NV data */
        if ( (rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.gain_data[band.wcdma_band] == NULL) || 
             (rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.sp_data[band.wcdma_band] == NULL) || 
             (rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band.wcdma_band] == NULL) || 
             (rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.temp_comp_data[band.wcdma_band] == NULL) )
        {
          RF_MSG_1( RF_FATAL, "rfcommon_fbrx_nv_allocate_mem: Memory Allocation failed" 
                    "for FBRX data for WCDMA band:%d", band.wcdma_band);
          ret_val = FALSE;
        }
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_allocate_mem: Invalid WCDMA Band = %d ", band.wcdma_band);
        ret_val = FALSE;
      }
      break;

    case RFM_1X_MODE:
    case RFM_1XEVDO_MODE:
    case RFM_SVDO_MODE:
      if (band.cdma_band < RFM_CDMA_MAX_BAND)
      {
        /* Allocate memory to load fbrx gain cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.gain_data[band.cdma_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.gain_data[band.cdma_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_gain_vs_freq_type_v2), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load fbrx sp cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.sp_data[band.cdma_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.sp_data[band.cdma_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_attn_switch_pt_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load fbrx dc cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band.cdma_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band.cdma_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_dc_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load fbrx temp comp cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.temp_comp_data[band.cdma_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.temp_comp_data[band.cdma_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_temp_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Check if memory was successfully allocated or not to store FBRx NV data */
        if ( (rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.gain_data[band.cdma_band] == NULL) || 
             (rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.sp_data[band.cdma_band] == NULL) || 
             (rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band.cdma_band] == NULL) || 
             (rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.temp_comp_data[band.cdma_band] == NULL) )
        {
          RF_MSG_1( RF_FATAL, "rfcommon_fbrx_nv_allocate_mem: Memory Allocation failed" 
                    "for FBRX data for CDMA band:%d", band.cdma_band );
          ret_val = FALSE;
        }
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_allocate_mem: Invalid CDMA Band = %d ", band.cdma_band);
        ret_val = FALSE;
      }
      break;

    case RFM_LTE_MODE:
    case RFM_LTE_FTM_MODE:
      if (band.lte_band < RFCOM_NUM_LTE_BANDS)
      {
        /* Allocate memory to load fbrx gain cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.gain_data[band.lte_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.gain_data[band.lte_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_gain_vs_freq_type_v2), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load fbrx sp cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.sp_data[band.lte_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.sp_data[band.lte_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_attn_switch_pt_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load fbrx dc cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band.lte_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band.lte_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_dc_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load fbrx temp comp cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.temp_comp_data[band.lte_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.temp_comp_data[band.lte_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_temp_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load LTE <band,BW> specific droop data */
        if (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.droop_data[band.lte_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.droop_data[band.lte_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_lte_band_droop_params_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Check if memory was successfully allocated or not to store FBRx NV data */
        if ( (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.gain_data[band.lte_band] == NULL) || 
             (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.sp_data[band.lte_band] == NULL) || 
             (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band.lte_band] == NULL) || 
             (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.temp_comp_data[band.lte_band] == NULL) ||
             (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.droop_data[band.lte_band] == NULL) )
        {
          RF_MSG_1( RF_FATAL, "rfcommon_fbrx_nv_allocate_mem: Memory Allocation failed" 
                    "for FBRX data for LTE band:%d", band.lte_band );
          ret_val = FALSE;
        }
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_allocate_mem: Invalid LTE Band = %d ", band.lte_band);
        ret_val = FALSE;
      }
      break;

    case RFM_TDSCDMA_MODE:
    case RFM_TDSCDMA_MODE_FTM:
      if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
      {
        /* Allocate memory to load fbrx gain cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.gain_data[band.tdscdma_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.gain_data[band.tdscdma_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_gain_vs_freq_type_v2), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load fbrx sp cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.sp_data[band.tdscdma_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.sp_data[band.tdscdma_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_attn_switch_pt_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load fbrx dc cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band.tdscdma_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band.tdscdma_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_dc_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Allocate memory to load fbrx temp comp cal data */
        if (rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.temp_comp_data[band.tdscdma_band] == NULL)
        {
          rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.temp_comp_data[band.tdscdma_band] = 
            modem_mem_calloc(1,sizeof(rfcommon_fbrx_nv_temp_type), MODEM_MEM_CLIENT_RFA);
        }

        /* Check if memory was successfully allocated or not to store FBRx NV data */
        if ( (rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.gain_data[band.tdscdma_band] == NULL) || 
             (rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.sp_data[band.tdscdma_band] == NULL) || 
             (rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band.tdscdma_band] == NULL) || 
             (rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.temp_comp_data[band.tdscdma_band] == NULL) )
        {
          RF_MSG_1( RF_FATAL, "rfcommon_fbrx_nv_allocate_mem: Memory Allocation failed" 
                    "for FBRX data for LTE band:%d", band.tdscdma_band );
          ret_val = FALSE;
        }
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_allocate_mem: Invalid TDSCDMA Band = %d ", band.tdscdma_band);
        ret_val = FALSE;
      }
      break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_allocate_mem: Invalid Tech = %d ", tech);
      ret_val = FALSE;
      break;
  } /* switch case*/

  return ret_val;
  
} /* rfcommon_fbrx_nv_allocate_mem() */ 


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to de-allocate memory for global fbrx
  struct which strores FBRX Cal NV data 
 
  @param tech
  Tech info for which the FBRX DM is updated.
  
  @param band
  Band info for which the FBRX DM is updated.

*/

boolean 
rfcommon_fbrx_nv_deallocate_mem ( rfm_mode_enum_type tech,
                                  rfcom_band_type_u band  )
{

  boolean ret_val = TRUE;

  switch (tech)
  {
    case RFM_IMT_MODE:
      if (band.wcdma_band < RFCOM_NUM_WCDMA_BANDS)
      {
        /* Do clean up */
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.gain_data[band.wcdma_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.gain_data[band.wcdma_band] = NULL;
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.sp_data[band.wcdma_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.sp_data[band.wcdma_band] = NULL;
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band.wcdma_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band.wcdma_band] = NULL;
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.temp_comp_data[band.wcdma_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.temp_comp_data[band.wcdma_band] = NULL;
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_deallocate_mem: Invalid WCDMA Band = %d ", band.wcdma_band);
        ret_val = FALSE;
      }
      break;

    case RFM_1X_MODE:
    case RFM_1XEVDO_MODE:
    case RFM_SVDO_MODE:
      if (band.cdma_band < RFM_CDMA_MAX_BAND)
      {
        /* Do clean up */
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.gain_data[band.cdma_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.gain_data[band.cdma_band] = NULL;
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.sp_data[band.cdma_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.sp_data[band.cdma_band] = NULL;
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band.cdma_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band.cdma_band] = NULL;
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.temp_comp_data[band.cdma_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.temp_comp_data[band.cdma_band] = NULL;
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_deallocate_mem: Invalid CDMA Band = %d ", band.cdma_band);
        ret_val = FALSE;
      }
      break;

    case RFM_LTE_MODE:
    case RFM_LTE_FTM_MODE:
      if (band.lte_band < RFCOM_NUM_LTE_BANDS)
      {
        /* Do clean up */
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.gain_data[band.lte_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.gain_data[band.lte_band] = NULL;
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.sp_data[band.lte_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.sp_data[band.lte_band] = NULL;
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band.lte_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band.lte_band] = NULL;
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.temp_comp_data[band.lte_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.temp_comp_data[band.lte_band] = NULL;

        if(rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.droop_data[band.lte_band] != NULL) 
        {
          modem_mem_free((void *)rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.droop_data[band.lte_band], MODEM_MEM_CLIENT_RFA); 
          rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.droop_data[band.lte_band] = NULL;
        }
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_deallocate_mem: Invalid LTE Band = %d ", band.lte_band);
        ret_val = FALSE;
      }
      break;

    case RFM_TDSCDMA_MODE:
    case RFM_TDSCDMA_MODE_FTM:
      if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
      {
        /* Do clean up */
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.gain_data[band.tdscdma_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.gain_data[band.tdscdma_band] = NULL;
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.sp_data[band.tdscdma_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.sp_data[band.tdscdma_band] = NULL;
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band.tdscdma_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band.tdscdma_band] = NULL;
        modem_mem_free((void*)rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.temp_comp_data[band.tdscdma_band], MODEM_MEM_CLIENT_RFA);
        rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.temp_comp_data[band.tdscdma_band] = NULL;
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_deallocate_mem: Invalid TDSCDMA Band = %d ", band.tdscdma_band);
        ret_val = FALSE;
      }
      break;

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_deallocate_mem: Invalid Tech = %d ", tech);
      ret_val = FALSE;
      break;
  } /* switch case*/

  return ret_val;
  
} /* rfcommon_fbrx_nv_deallocate_mem() */ 


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the FBRX Cal NV data to the global fbrx
  struct for WCDMA. Load rfcommon_fbrx_nv_tech_cal_data with FBRX
  NV data for Gain Cal, Switch Point Cal and Droop Cal data

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.
*/

boolean 
rfcommon_fbrx_nv_retrieve_wcdma_data( rex_tcb_type* caller_tcb_ptr,
                                      rex_sigs_type task_nv_wait_sig,
                                      void (* task_wait_func_ptr)( rex_sigs_type ) )
{
  rfcom_wcdma_band_type band;
  rfcom_band_type_u rfcom_band;
  rfnv_item_id_enum_type fbrx_gain_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_sp_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_dc_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_temp_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_temp_nv_id_z = RFNV_ITEMS_ENUM_MAX;
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  boolean ret_val = TRUE;

#ifdef FEATURE_WCDMA
  /* Get FBRX Gain & Switch point, DC data from NV for each supported band */
  for (band = RFCOM_BAND_IMT; band<RFCOM_NUM_WCDMA_BANDS; band++)
  {
    rfcom_band.wcdma_band = band;

    if (rfwcdma_core_is_band_supported(band,RF_PATH_0))
    {
      /* Get the fbrx gain, switch point, DC & temp comp NV ID for current WCDMA Band */
      if ( rfcommon_fbrx_get_wcdma_cal_nv_id( band, &fbrx_gain_nv_id, &fbrx_sp_nv_id, &fbrx_dc_nv_id, &fbrx_temp_nv_id ) )
      {
        if ((api_status = rfcommon_fbrx_get_wcdma_cal_compressed_nv_id( band, &fbrx_temp_nv_id_z )) != RFCOMMON_FBRX_SUCCESS) 
        {
          /* It should never come here */
          RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_wcdma_data: Load fbrx compressed NV failed for WCDMA band:%d", band);
          ret_val = FALSE;
          return ret_val;
        }

        if (rfcommon_fbrx_nv_init.wcdma_init == TRUE)
        {
          /* Do clean up 1st */
          ret_val = rfcommon_fbrx_nv_deallocate_mem(RFCOM_WCDMA_MODE, rfcom_band);
        }

        /* If memory was successfully allocated to store FBRx NV data */
        if ( rfcommon_fbrx_nv_allocate_mem(RFM_IMT_MODE, rfcom_band) )
        {
          /* Load the fbrx gain data from NV to struct for current WCDMA Band */
          api_status = rfcommon_fbrx_nv_load_gain( rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.gain_data[band], 
                                                   fbrx_gain_nv_id, 
                                                   caller_tcb_ptr, 
                                                   task_nv_wait_sig, 
                                                   task_wait_func_ptr );

          if (api_status != RFCOMMON_FBRX_SUCCESS )
          {
            RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_wcdma_data: Load fbrx gain data from NV failed for WCDMA band:%d", band);
            ret_val = FALSE;
            return ret_val;
          }

          /* Load the fbrx switch point data from NV to struct for current WCDMA Band */
          api_status = rfcommon_fbrx_nv_load_attn_switch_pts( rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.sp_data[band], 
                                                              fbrx_sp_nv_id, 
                                                              caller_tcb_ptr, 
                                                              task_nv_wait_sig, 
                                                              task_wait_func_ptr );
          if (api_status != RFCOMMON_FBRX_SUCCESS )
          {
            RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_wcdma_data: Load fbrx switchpoint data from NV failed for WCDMA band:%d", band);
            ret_val = FALSE;
            return ret_val;
          }

          /* Load the fbrx dc data from NV to struct for current WCDMA Band */
          api_status = rfcommon_fbrx_nv_load_dc( rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band], 
                                                 fbrx_dc_nv_id, 
                                                 caller_tcb_ptr, 
                                                 task_nv_wait_sig, 
                                                 task_wait_func_ptr );

          if (api_status != RFCOMMON_FBRX_SUCCESS )
          {
            RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_wcdma_data: Load fbrx dc data from NV failed for WCDMA band:%d", band);
            ret_val = FALSE;
            return ret_val;
          }

          /* Load the fbrx temp comp data from NV to struct for current WCDMA Band */
          api_status = rfcommon_fbrx_nv_load_temp( rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.temp_comp_data[band], 
                                                   fbrx_temp_nv_id_z, 
                                                   caller_tcb_ptr, 
                                                   task_nv_wait_sig, 
                                                   task_wait_func_ptr );
          if ( api_status != RFCOMMON_FBRX_SUCCESS ) 
          {
             /* Load the fbrx temp comp data from NV to struct for current WCDMA Band */
             api_status = rfcommon_fbrx_nv_load_temp( rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.temp_comp_data[band], 
                                                      fbrx_temp_nv_id, 
                                                      caller_tcb_ptr, 
                                                      task_nv_wait_sig, 
                                                      task_wait_func_ptr );

          }
          if (api_status != RFCOMMON_FBRX_SUCCESS )
          {
            RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_wcdma_data: Load fbrx temp comp data from NV failed for WCDMA band:%d", band);
            ret_val = FALSE;
            return ret_val;
          }
        } /* if (rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data != NULL) */
        else
        {
          RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_wcdma_data: Memory Allocation failed for FBRX data for WCDMA band:%d", band);
          ret_val = FALSE;
          return ret_val;
        }
      } /* if (api_status == RFCOMMON_FBRX_SUCCESS)  */
      else
      {
        RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_wcdma_data: FBRX Cal NV not supported for WCDMA band:%d", band); 
      } 
          
    }/* if (rfwcdma_core_is_band_supported(band,RF_PATH_0)) */ 
  } /* for (band = RFCOM_BAND_IMT; band<RFCOM_NUM_WCDMA_BANDS; band++) */

  rfcommon_fbrx_nv_init.wcdma_init = TRUE;
#endif /* FEATURE_WCDMA */

  return ret_val;
  
} /* rfcommon_fbrx_nv_retrieve_wcdma_data() */ 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the FBRX Cal NV data to the global fbrx
  struct for CDMA. Load rfcommon_fbrx_nv_tech_cal_data with FBRX
  NV data for Gain Cal, Switch Point Cal and Droop Cal data

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.
*/
#ifdef FEATURE_CDMA1X
boolean 
rfcommon_fbrx_nv_retrieve_1x_data( rex_tcb_type* caller_tcb_ptr,
                                   rex_sigs_type task_nv_wait_sig,
                                   void (* task_wait_func_ptr)( rex_sigs_type ))
{
  rfm_cdma_band_class_type band;
  rfm_device_enum_type device;
  rfcom_band_type_u rfcom_band;
  rfnv_item_id_enum_type fbrx_gain_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_sp_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_dc_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_temp_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_temp_nv_id_z = RFNV_ITEMS_ENUM_MAX;
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  boolean ret_val = TRUE;

  /* Get FBRX Gain & Switch point, DC & temp comp data from NV for each supported band */
  for (device = RFM_DEVICE_0; device< RFM_MAX_DEVICES; device++) 
  {
    for (band = RFM_CDMA_BC0; band<RFM_CDMA_MAX_BAND; band++)
    {
      if ( TRUE == rfc_cdma_is_tx_device (device) ) 
      {
        if (rf_cdma_is_band_supported(device,band)) 
        {
          rfcom_band.cdma_band = band;

          /* Get the fbrx gain, switch point, DC & temp comp NV ID for current CDMA Band */
          if ( rfcommon_fbrx_get_1x_cal_nv_id( band, &fbrx_gain_nv_id, &fbrx_sp_nv_id, &fbrx_dc_nv_id, &fbrx_temp_nv_id ) )
          {
             if ( (api_status = rfcommon_fbrx_get_1x_cal_compressed_nv_id( band, &fbrx_temp_nv_id_z )) != RFCOMMON_FBRX_SUCCESS) 
             {
               /* It should never come here */
               RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_1x_data: Load fbrx compressed NV failed for CDMA band:%d", band);
               ret_val = FALSE;
               return ret_val;
             }

             if (rfcommon_fbrx_nv_init.cdma_init == TRUE)
            {
              /* Do clean up 1st */
              ret_val = rfcommon_fbrx_nv_deallocate_mem(RFCOM_1X_MODE, rfcom_band);
            }
     
            /* If memory was successfully allocated to store FBRx NV data */
            if ( rfcommon_fbrx_nv_allocate_mem(RFCOM_1X_MODE, rfcom_band) )
            {
              /* Load the fbrx gain data from NV to struct for current CDMA Band */
              api_status = rfcommon_fbrx_nv_load_gain( rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.gain_data[band], 
                                                       fbrx_gain_nv_id, 
                                                       caller_tcb_ptr, 
                                                       task_nv_wait_sig, 
                                                       task_wait_func_ptr );
     
              if (api_status != RFCOMMON_FBRX_SUCCESS )
              {
                RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_retrieve_1x_data: Load fbrx gain data from NV failed for CDMA band:%d", band);
                ret_val = FALSE;
                return ret_val;
              }
     
              /* Load the fbrx switch point data from NV to struct for current CDMA Band */
              api_status = rfcommon_fbrx_nv_load_attn_switch_pts( rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.sp_data[band], 
                                                                  fbrx_sp_nv_id, 
                                                                  caller_tcb_ptr, 
                                                                  task_nv_wait_sig, 
                                                                  task_wait_func_ptr );
              if (api_status != RFCOMMON_FBRX_SUCCESS )
              {
                RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_retrieve_1x_data: Load fbrx switchpoint data from NV failed for CDMA band:%d", band);
                ret_val = FALSE;
                return ret_val;
              }
              
              /* Load the fbrx dc data from NV to struct for current CDMA Band */
              api_status = rfcommon_fbrx_nv_load_dc( rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band], 
                                                     fbrx_dc_nv_id, 
                                                     caller_tcb_ptr, 
                                                     task_nv_wait_sig, 
                                                     task_wait_func_ptr );
     
              if (api_status != RFCOMMON_FBRX_SUCCESS )
              {
                RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_retrieve_1x_data: Load fbrx dc data from NV failed for CDMA band:%d", band);
                ret_val = FALSE;
                return ret_val;
              }
     
              /* Load the fbrx temp comp data from compressed NV to struct for current CDMA Band */
              api_status = rfcommon_fbrx_nv_load_temp( rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.temp_comp_data[band], 
                                                       fbrx_temp_nv_id_z, 
                                                       caller_tcb_ptr, 
                                                       task_nv_wait_sig, 
                                                       task_wait_func_ptr );

              if (api_status != RFCOMMON_FBRX_SUCCESS) 
              {              
                /* Load the fbrx temp comp data from NV to struct for current CDMA Band */
                api_status = rfcommon_fbrx_nv_load_temp( rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.temp_comp_data[band], 
                                                         fbrx_temp_nv_id, 
                                                         caller_tcb_ptr, 
                                                         task_nv_wait_sig, 
                                                         task_wait_func_ptr );
              }
              if (api_status != RFCOMMON_FBRX_SUCCESS )
              {
                RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_retrieve_1x_data: Load fbrx temp comp data from NV failed for CDMA band:%d", band);
                ret_val = FALSE;
                return ret_val;
              }
            } /* if (rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data != NULL) */
            else
            {
              RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_retrieve_1x_data: Memory Allocation failed for FBRX data for CDMA band:%d", band);
              ret_val = FALSE;
              return ret_val;
            } 
          } /* if (api_status == RFCOMMON_FBRX_SUCCESS) */
          else
          {
            RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_retrieve_1x_data: FBRX Cal NV not supported for CDMA band:%d", band);
          }
              
        }/* if band supported */ 
      } /* if ( TRUE == rfc_cdma_is_tx_device (device) ) */
    } /* for (band = RFM_CDMA_BC0; band<RFM_CDMA_MAX_BAND; band++) */
  }/* for (device = RFM_DEVICE_0; device< RFM_MAX_DEVICES; device++)*/
  
  rfcommon_fbrx_nv_init.cdma_init = TRUE;
  return ret_val;

} /* rfcommon_fbrx_nv_retrieve_1x_data() */ 
#endif /* FEATURE_CDMA1X */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the FBRX Cal NV data to the global fbrx
  struct for LTE. Load rfcommon_fbrx_nv_tech_cal_data with FBRX
  NV data for Gain Cal, Switch Point Cal and Droop Cal data

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.
*/

boolean
rfcommon_fbrx_nv_retrieve_lte_data( rex_tcb_type* caller_tcb_ptr,
                                    rex_sigs_type task_nv_wait_sig,
                                    void (* task_wait_func_ptr)( rex_sigs_type ))
{
  rfcom_lte_band_type band;
  rfcom_band_type_u rfcom_band;
  rfnv_item_id_enum_type fbrx_gain_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_sp_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_dc_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_temp_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_temp_nv_id_z = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_droop_nv_id_z = RFNV_ITEMS_ENUM_MAX;
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  boolean ret_val = TRUE;

  /* Get FBRX Gain & Switch point, DC & temp comp data from NV for each supported band */
  for (band = RFCOM_BAND_LTE_B1; band<RFCOM_NUM_LTE_BANDS; band++)
  {
    rfcom_band.lte_band = band;

    if (rflte_nv_get_tx_band_support(band) == TRUE) 
    {

      /************************************************************************************* 
        Step 1: Get the fbrx gain & switch point, DC, Temp Comp NV ID for current LTE Band
       *************************************************************************************/
      api_status = rfcommon_fbrx_get_lte_cal_nv_id( band, 
                                                    &fbrx_gain_nv_id, 
                                                    &fbrx_sp_nv_id,
                                                    &fbrx_dc_nv_id, 
                                                    &fbrx_temp_nv_id);
      if ( api_status != RFCOMMON_FBRX_ERROR )
      {
        api_status = rfcommon_fbrx_get_lte_cal_compressed_nv_id( band, &fbrx_temp_nv_id_z );
        if ( api_status != RFCOMMON_FBRX_SUCCESS) 
        {
          /* It should never come here */
          RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_lte_data: Load fbrx compressed NV "
                             "failed for LTE band:%d", band);
          ret_val = FALSE;
          return ret_val;
        }

        if (rfcommon_fbrx_nv_init.lte_init == TRUE)
        {
          ret_val = rfcommon_fbrx_nv_deallocate_mem(RFCOM_LTE_MODE, rfcom_band);
        }
        /* If memory was successfully allocated to store FBRx NV data */
        if ( rfcommon_fbrx_nv_allocate_mem(RFCOM_LTE_MODE, rfcom_band) )
        {
          /* Load the fbrx gain data from NV to struct for current LTE Band */
          api_status = rfcommon_fbrx_nv_load_gain( rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.gain_data[band], 
                                                   fbrx_gain_nv_id, 
                                                   caller_tcb_ptr, 
                                                   task_nv_wait_sig, 
                                                   task_wait_func_ptr );

          if (api_status != RFCOMMON_FBRX_SUCCESS )
          {
            RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_lte_data: Load fbrx gain data from NV failed for LTE band:%d", band);
            ret_val = FALSE;
            return ret_val;
          }

          /* Load the fbrx switch point data from NV to struct for current LTE Band */
          api_status = rfcommon_fbrx_nv_load_attn_switch_pts( rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.sp_data[band], 
                                                              fbrx_sp_nv_id, 
                                                              caller_tcb_ptr, 
                                                              task_nv_wait_sig, 
                                                              task_wait_func_ptr );
          if (api_status != RFCOMMON_FBRX_SUCCESS )
          {
            RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_lte_data: Load fbrx switchpoint data from NV failed for LTE band:%d", band);
            ret_val = FALSE;
            return ret_val;
          }

          /* Load the fbrx dc data from NV to struct for current LTE Band */
          api_status = rfcommon_fbrx_nv_load_dc( rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band], 
                                                 fbrx_dc_nv_id, 
                                                 caller_tcb_ptr, 
                                                 task_nv_wait_sig, 
                                                 task_wait_func_ptr );

          if (api_status != RFCOMMON_FBRX_SUCCESS )
          {
            RF_MSG_1(RF_FATAL, "Load fbrx dc data from NV failed for LTE band:%d", band);
            ret_val = FALSE;
            return ret_val;
          }

          /* Load the fbrx temp comp data from compressed NV to struct for current LTE Band */
          api_status = rfcommon_fbrx_nv_load_temp( rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.temp_comp_data[band], 
                                                   fbrx_temp_nv_id_z, 
                                                   caller_tcb_ptr, 
                                                   task_nv_wait_sig, 
                                                   task_wait_func_ptr );

          if (api_status != RFCOMMON_FBRX_SUCCESS) 
          {
            /* Load the fbrx temp comp data from NV to struct for current LTE Band */
            api_status = rfcommon_fbrx_nv_load_temp( rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.temp_comp_data[band], 
                                                     fbrx_temp_nv_id, 
                                                     caller_tcb_ptr, 
                                                     task_nv_wait_sig, 
                                                     task_wait_func_ptr );
          }

          if (api_status != RFCOMMON_FBRX_SUCCESS )
          {
            RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_lte_data: Load fbrx temp comp data from NV failed for LTE band:%d", band);
            ret_val = FALSE;
            return ret_val;
          }
        } /* if (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data != NULL) */
        else
        {
          RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_lte_data: Memory Allocation failed for FBRX data for LTE band:%d", band);
          ret_val = FALSE;
          return ret_val;
        }
      } /* if (api_status == RFCOMMON_FBRX_SUCCESS) */
      else
      {
        RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_lte_data: FBRX Cal NV not supported for LTE band:%d", band);
        ret_val = FALSE;
        return ret_val;
      }

      /************************************************************************************* 
              Step 2: Get the LTE Band/BW specific droop cal data
      *************************************************************************************/

      api_status = rfcommon_fbrx_get_lte_band_bw_droop_compressed_nv_id( band, 
                                                                         &fbrx_droop_nv_id_z );

      if ( api_status != RFCOMMON_FBRX_SUCCESS) 
      {
        /* It should never come here */
        RF_MSG_1(RF_FATAL, "rfcommon_fbrx_get_lte_band_bw_droop_compressed_nv_id: Load fbrx LTE "
                           "droop compressed NV failed for LTE band:%d", band);
        ret_val = FALSE;
        return ret_val;
      }

      /*We can proceed to the NV memory retrival. We wouldnot have reached here if the memory
      allocation in Step-1 failed */

      /* Load the fbrx lte band specific droop data from compressed NV to struct for current LTE Band */
      api_status = rfcommon_fbrx_nv_load_lte_band_droop_params( rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.droop_data[band], 
                                                                fbrx_droop_nv_id_z, 
                                                                caller_tcb_ptr, 
                                                                task_nv_wait_sig, 
                                                                task_wait_func_ptr );

      if (api_status != RFCOMMON_FBRX_SUCCESS )
      {
        RF_MSG_1(RF_HIGH, "rfcommon_fbrx_nv_retrieve_lte_data: Load fbrx LTE band specific droop params from"
                           " NV failed for LTE band:%d", band);

        /* [1] Not finding LTE band specific droop data doesnot qualify as a fatal error as we can still
               fall back to common droop data structures 
           [2] We can free up the memory as the droop lte band data structure is not available */
       modem_mem_free( (void*)rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.droop_data[band], MODEM_MEM_CLIENT_RFA);
       rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.droop_data[band] = NULL;
      }
         
    }/* if band supported */ 
  } /* for (band = RFCOM_BAND_LTE_B1; band<RFCOM_NUM_LTE_BANDS; band++) */

  rfcommon_fbrx_nv_init.lte_init = TRUE;
  return ret_val;

} /* rfcommon_fbrx_nv_retrieve_lte_data() */ 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the FBRX Cal NV data to the global fbrx
  struct for TDSCDMA. Load rfcommon_fbrx_nv_tech_cal_data with FBRX
  NV data for Gain Cal, Switch Point Cal and Droop Cal data

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.
*/

boolean 
rfcommon_fbrx_nv_retrieve_tdscdma_data( rex_tcb_type* caller_tcb_ptr,
                                        rex_sigs_type task_nv_wait_sig,
                                        void (* task_wait_func_ptr)( rex_sigs_type ))
{
  rfcom_tdscdma_band_type band;
  rfcom_band_type_u rfcom_band;
  rfnv_item_id_enum_type fbrx_gain_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_sp_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_dc_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_temp_nv_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_item_id_enum_type fbrx_temp_nv_id_z = RFNV_ITEMS_ENUM_MAX;
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  boolean ret_val = TRUE;

  /* Get FBRX Gain & Switch point, DC & temp comp data from NV for each supported band */
  for (band = RFCOM_BAND_TDSCDMA_B34; band<RFCOM_NUM_TDSCDMA_BANDS; band++)
  {
    rfcom_band.tdscdma_band = band;

    if (1) /* @todo: check for band support */
    {
      /* Get the fbrx gain & switch point, DC, Temp Comp NV ID for current TDSCDMA Band */
      if ( rfcommon_fbrx_get_tdscdma_cal_nv_id( band, &fbrx_gain_nv_id, &fbrx_sp_nv_id, &fbrx_dc_nv_id, &fbrx_temp_nv_id) )
      {
        if ( (api_status = rfcommon_fbrx_get_tdscdma_cal_compressed_nv_id( band, &fbrx_temp_nv_id_z )) != RFCOMMON_FBRX_SUCCESS) 
        {
          /* It should never come here */
          RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_tdscdma_data: Load fbrx compressed NV failed for TDSCDMA band:%d", band);
          ret_val = FALSE;
          return ret_val;
        }
        if (rfcommon_fbrx_nv_init.tds_init == TRUE)
        {
          ret_val = rfcommon_fbrx_nv_deallocate_mem(RFCOM_TDSCDMA_MODE, rfcom_band);
        }
        /* If memory was successfully allocated to store FBRx NV data */
        if ( rfcommon_fbrx_nv_allocate_mem(RFCOM_TDSCDMA_MODE, rfcom_band) )
        {
          /* Load the fbrx gain data from NV to struct for current TDSCDMA Band */
          api_status = rfcommon_fbrx_nv_load_gain( rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.gain_data[band], 
                                                   fbrx_gain_nv_id, 
                                                   caller_tcb_ptr, 
                                                   task_nv_wait_sig, 
                                                   task_wait_func_ptr );

          if (api_status != RFCOMMON_FBRX_SUCCESS )
          {
            RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_tdscdma_data: Load fbrx gain data from NV failed for TDS band:%d", band);
            ret_val = FALSE;
            return ret_val;
          }

          /* Load the fbrx switch point data from NV to struct for current TDSCDMA Band */
          api_status = rfcommon_fbrx_nv_load_attn_switch_pts( rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.sp_data[band], 
                                                              fbrx_sp_nv_id, 
                                                              caller_tcb_ptr, 
                                                              task_nv_wait_sig, 
                                                              task_wait_func_ptr );
          if (api_status != RFCOMMON_FBRX_SUCCESS )
          {
            RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_tdscdma_data: Load fbrx switchpoint data from NV failed for TDS band:%d", band);
            ret_val = FALSE;
            return ret_val;
          }
      
          /* Load the fbrx dc data from NV to struct for current TDSCDMA Band */
          api_status = rfcommon_fbrx_nv_load_dc( rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band], 
                                                 fbrx_dc_nv_id, 
                                                 caller_tcb_ptr, 
                                                 task_nv_wait_sig, 
                                                 task_wait_func_ptr );

          if (api_status != RFCOMMON_FBRX_SUCCESS )
          {
            RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_tdscdma_data: Load fbrx dc data from NV failed for TDS band:%d", band);
            ret_val = FALSE;
            return ret_val;
          }

          /* Load the fbrx temp comp data from NV to struct for current TDSCDMA Band */
          api_status = rfcommon_fbrx_nv_load_temp( rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.temp_comp_data[band], 
                                                   fbrx_temp_nv_id_z, 
                                                   caller_tcb_ptr, 
                                                   task_nv_wait_sig, 
                                                   task_wait_func_ptr );
          if (api_status != RFCOMMON_FBRX_SUCCESS) 
          {
            /* Load the fbrx temp comp data from NV to struct for current TDSCDMA Band */
            api_status = rfcommon_fbrx_nv_load_temp( rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.temp_comp_data[band], 
                                                     fbrx_temp_nv_id, 
                                                     caller_tcb_ptr, 
                                                     task_nv_wait_sig, 
                                                     task_wait_func_ptr );
          }
          if (api_status != RFCOMMON_FBRX_SUCCESS )
          {
            RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_tdscdma_data: Load fbrx temp comp data from NV failed for TDS band:%d", band);
            ret_val = FALSE;
            return ret_val;
          }                   
        } /* if (rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data != NULL) */
        else
        {
          RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_tdscdma_data: Memory Allocation failed for FBRX data for TDS band:%d", band);
          ret_val = FALSE;
          return ret_val;
        }
      } /* if (api_status == RFCOMMON_FBRX_SUCCESS) */
      else
      {
        RF_MSG_1(RF_FATAL, "rfcommon_fbrx_nv_retrieve_tdscdma_data: FBRX Cal NV not supported for TDSCDMA band:%d", band);        
      } 
      
    }/* if band supported */ 
  } /* for (band = RFCOM_BAND_TDSCDMA_B34; band<RFCOM_NUM_TDSCDMA_BANDS; band++) */

  rfcommon_fbrx_nv_init.tds_init = TRUE;
  return ret_val;

} /* rfcommon_fbrx_nv_retrieve_tdscdma_data() */ 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the common FBRX Cal NV data to the global 
  fbrx control variable. 

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.
*/

boolean 
rfcommon_fbrx_nv_retrieve_control_data( rex_tcb_type* caller_tcb_ptr,
                                         rex_sigs_type task_nv_wait_sig,
                                         void (* task_wait_func_ptr)( rex_sigs_type ))
{
  rfnv_item_id_enum_type fbrx_control_nv_id = RFNV_COMMON_FBRX_SYSTEM_CONTROL_I;

  /* NV read status */
  nv_stat_enum_type  fbrx_nv_read_status = NV_DONE_S;
  /* NV item size (maximum) */
  uint16 nv_item_size = sizeof(rfnv_item_type); 
  /* Pointer to NV data*/
  rfnv_item_type *nv_data_ptr = NULL;
  boolean ret_val = TRUE;

  /* Allocate memory for raw NV data */
  nv_data_ptr = (rfnv_item_type *)modem_mem_calloc(1, sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA );
  
  /* Null pointer check */
  if (nv_data_ptr == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_retrieve_control_data: Failed to allocate buffer memory on heap");
    ret_val = FALSE;
    return ret_val;
  }

  fbrx_nv_read_status = rfcommon_rfnv_get_item( fbrx_control_nv_id,
                                                nv_data_ptr,
                                                &nv_item_size,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );

  if ( fbrx_nv_read_status == NV_DONE_S )
  {
    memscpy( &rfcommon_fbrx_sys_control_nv_data, sizeof(int32), (void*)nv_data_ptr, sizeof(int32) );
  } 
  else if ( fbrx_nv_read_status == NV_NOTACTIVE_S )
  {
    /* Default NV data to 0 */
    rfcommon_fbrx_sys_control_nv_data = 0;
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_retrieve_control_data: rfcommon_rfnv_get_item NOT ACTIVE "
                        "for NV = %d", fbrx_control_nv_id);	
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_nv_retrieve_control_data: rfcommon_rfnv_get_item failed for NV=%d", fbrx_control_nv_id);	
    ret_val = FALSE;
  }

  modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
  return ret_val;

} /* rfcommon_fbrx_nv_retrieve_control_data */ 

/*----------------------------------------------------------------------------*/

/*!
  @brief
  Generic function to load the common FBRX Cal NV data to the global 
  fbrx variable. 

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.
*/

boolean 
rfcommon_fbrx_nv_retrieve_common_data( rex_tcb_type* caller_tcb_ptr,
                                       rex_sigs_type task_nv_wait_sig,
                                       void (* task_wait_func_ptr)( rex_sigs_type ))
{
  boolean ret_val = TRUE;
  /* Copy the common FBRx Contol NV data  */
  ret_val &= rfcommon_fbrx_nv_retrieve_control_data( caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );

  if (ret_val == FALSE)
  {
     RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_retrieve_common_data: rfcommon_fbrx_nv_retrieve_control_data failed");	
  }

  /* Copy the common FBRx Droop NV data for all tech/bw */
  ret_val &= rfcommon_fbrx_nv_retrieve_droop_data( caller_tcb_ptr,
                                        task_nv_wait_sig,
                                        task_wait_func_ptr);
  if (ret_val == FALSE)
  {
     RF_MSG( RF_ERROR, "rfcommon_fbrx_nv_retrieve_common_data: rfcommon_fbrx_nv_retrieve_control_data failed");	
  }

  return ret_val;
} /* rfcommon_fbrx_nv_retrieve_common_data */ 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Computes the interpolated gain value based on current frequency and cal data. 
  
  @param tech
  Tech info for which the FBRX DM is updated.
  
  @param band
  Band info for which the FBRX DM is updated.
 
  @param *gain_ptr
  Pointer to the gain cal data.
 
  @param tx_freq
  Current tx frequency.
 
  @param mode_idx
  Current fbrx mode.
 
  @param gs_idx
  FBRx gain state.
 
*/
rfcommon_fbrx_nv_gain_freq_comp_type rfcommon_fbrx_nv_get_freq_comp_gain
(
  rfm_mode_enum_type tech,
  rfcom_band_type_u band,
  rfcommon_fbrx_nv_gain_vs_freq_type_v2* gain_ptr,
  int32 tx_freq,
  uint8  mode_idx,
  uint8  gs_idx,
  rflm_fbrx_dm_template_t* fbrx_dm_ptr
)
{
  rfcommon_fbrx_nv_gain_freq_comp_type result = {0};
  uint8 i = 0;
  uint8 lower_bin = 0, upper_bin = 0;
  int32 tx_cal_freq_list[RFLM_FBRX_NUM_CAL_CHANNELS] = {0};
  int32 lower_bin_distance = 0x7FFFFFFF, upper_bin_distance = 0x7FFFFFFF;
  int32 min_freq = 0x7FFFFFFF, max_freq = 0;
  uint8 min_freq_idx = 0, max_freq_idx = 0;

  /* Get list of calibrated frequencies */
  switch (tech)
  {
    #ifdef FEATURE_WCDMA
    case RFM_IMT_MODE:
      for (i=0;i<gain_ptr->fbrx_gain_chan_size;i++)
      {
        tx_cal_freq_list[i] = rfwcdma_core_get_freq_from_ul_arfcn(gain_ptr->fbrx_gain_chan[i]);
      }
      break;
    #endif /* FEATURE_WCDMA */

    #ifdef FEATURE_CDMA 
    case RFM_1X_MODE:
    case RFM_1XEVDO_MODE:
    case RFM_SVDO_MODE:
      for (i=0;i<gain_ptr->fbrx_gain_chan_size;i++)
      {
        tx_cal_freq_list[i] = rf_cdma_get_tx_carrier_freq(band.cdma_band, gain_ptr->fbrx_gain_chan[i]);
      }
      break;
    #endif 

    #ifdef FEATURE_LTE	
    case RFM_LTE_MODE:
    case RFM_LTE_FTM_MODE:
      for (i=0;i<gain_ptr->fbrx_gain_chan_size;i++)
      {
        tx_cal_freq_list[i] = rflte_util_get_tx_freq_from_uarfcn(gain_ptr->fbrx_gain_chan[i], band.lte_band);
      }
      break;
    #endif

    #ifdef FEATURE_TDSCDMA
    case RFM_TDSCDMA_MODE:
    case RFM_TDSCDMA_MODE_FTM:
      for (i=0;i<gain_ptr->fbrx_gain_chan_size;i++)
      {
        tx_cal_freq_list[i] = rf_tdscdma_core_util_get_freq_from_chan(gain_ptr->fbrx_gain_chan[i]);
      }
      break;
    #endif /* FEATURE_TDSCDMA */

    default:
      RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_get_freq_comp_gain: Invalid Tech = %d ", tech);
      break;
  } /* switch case*/

  /* Find lower bin - the closest cal point lower than or equal to the current freq, and find min and max freq */
  for (i=0;i<gain_ptr->fbrx_gain_chan_size;i++)
  {
    if ((tx_freq - tx_cal_freq_list[i]) < lower_bin_distance && (tx_freq - tx_cal_freq_list[i]) >= 0)
    {
      lower_bin = i;
      lower_bin_distance = tx_freq - tx_cal_freq_list[i];
    }
    if (tx_cal_freq_list[i] < min_freq)
    {
      min_freq_idx = i;
      min_freq = tx_cal_freq_list[i];
    }
    if (tx_cal_freq_list[i] > max_freq)
    {
      max_freq_idx = i;
      max_freq = tx_cal_freq_list[i];
    }
  }

  if (fbrx_dm_ptr != NULL)
  {
     fbrx_dm_ptr->fbrx_ctl.min_tx_freq = min_freq;
     fbrx_dm_ptr->fbrx_ctl.max_tx_freq = max_freq;
     fbrx_dm_ptr->fbrx_ctl.cur_tx_freq = tx_freq;     
  }

  /* Find upper bin - the closest cal point higher than the current frequency */
  for (i=0;i<gain_ptr->fbrx_gain_chan_size;i++)
  {
    if ((tx_cal_freq_list[i] - tx_cal_freq_list[lower_bin]) < upper_bin_distance && 
        (tx_cal_freq_list[i] - tx_cal_freq_list[lower_bin]) > 0)
    {
      upper_bin = i;
      upper_bin_distance = tx_cal_freq_list[i] - tx_cal_freq_list[lower_bin];
    }
  }
  if (tx_freq < min_freq)
  {
    /* Use the lowest cal point */
    result.gain = gain_ptr->fbrx_gain[min_freq_idx][mode_idx][gs_idx].gain;
    result.predet_index = gain_ptr->fbrx_gain[min_freq_idx][mode_idx][gs_idx].predet_index;
    result.tx_pwr = gain_ptr->fbrx_gain[min_freq_idx][mode_idx][gs_idx].tx_power;
  }
  else if (tx_freq > max_freq)
  {
    /* Use the highest cal point */
    result.gain = gain_ptr->fbrx_gain[max_freq_idx][mode_idx][gs_idx].gain;
    result.predet_index = gain_ptr->fbrx_gain[max_freq_idx][mode_idx][gs_idx].predet_index;
    result.tx_pwr = gain_ptr->fbrx_gain[max_freq_idx][mode_idx][gs_idx].tx_power;
  }
  else
  {
    /* Interpolate between the two cal points  */
    if (tx_cal_freq_list[upper_bin] - tx_cal_freq_list[lower_bin] != 0)
    {
      result.gain = ((gain_ptr->fbrx_gain[upper_bin][mode_idx][gs_idx].gain - gain_ptr->fbrx_gain[lower_bin][mode_idx][gs_idx].gain) *
                     (tx_freq - tx_cal_freq_list[lower_bin])) /
                       (tx_cal_freq_list[upper_bin] - tx_cal_freq_list[lower_bin]) +
                        gain_ptr->fbrx_gain[lower_bin][mode_idx][gs_idx].gain;
      result.tx_pwr = ((gain_ptr->fbrx_gain[upper_bin][mode_idx][gs_idx].tx_power - gain_ptr->fbrx_gain[lower_bin][mode_idx][gs_idx].tx_power) *
                       (tx_freq - tx_cal_freq_list[lower_bin])) /
                         (tx_cal_freq_list[upper_bin] - tx_cal_freq_list[lower_bin]) +
                          gain_ptr->fbrx_gain[lower_bin][mode_idx][gs_idx].tx_power;
    }
    else
    {
      result.gain = gain_ptr->fbrx_gain[lower_bin][mode_idx][gs_idx].gain;
      result.tx_pwr = gain_ptr->fbrx_gain[lower_bin][mode_idx][gs_idx].tx_power;
    }
    result.predet_index = gain_ptr->fbrx_gain[lower_bin][mode_idx][gs_idx].predet_index;
  }

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to copy the FBRX Cal NV data from the global fbrx
  var to the FBRX DM struct. 

  @param *fbrx_dm_ptr
  The fbrx DM template type DM pointer that needs to be updated.
  
  @param tech
  Tech info for which the FBRX DM is updated.
  
  @param band
  Band info for which the FBRX DM is updated.
*/

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_update_dm( rflm_fbrx_dm_template_t* fbrx_dm_ptr, 
                            rfm_mode_enum_type tech,
                            rfcom_band_type_u band,
                            uint16 chan_array[],                                       
                            uint8 num_channels)
{
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;
  rfcommon_fbrx_nv_gain_vs_freq_type_v2 *gain_data_ptr = NULL;
  rfcommon_fbrx_nv_attn_switch_pt_type *sp_data_ptr = NULL;
  rfcommon_fbrx_dc_num_gain_type *dc_data_ptr = NULL;
  rfcommon_fbrx_temp_num_gain_type *temp_comp_data_ptr = NULL;

  uint8 sp_idx = 0;
  uint8 gain_idx = 0;
  uint8 mode_idx = 0;
  uint8 cur_fbrx_mode_idx = 0;
  rfcommon_fbrx_tx_cfg_type tx_cfg = 0;
  uint32 tx_freq = 0;
  uint8 i;
  rfcommon_fbrx_nv_gain_freq_comp_type freq_comp_data;
  rfcommon_fbrx_nv_lte_band_droop_params_type *lte_droop_data;
  boolean use_common_droop_db = TRUE;

  if (fbrx_dm_ptr!= NULL && num_channels != 0)
  {
    /* Get the current FBRx mode */
    cur_fbrx_mode_idx = fbrx_dm_ptr->fbrx_ctl.fbrx_mode;
    /* Get the current Tx Config */
    tx_cfg = fbrx_dm_ptr->fbrx_ctl.bandwidth;

    if (tx_cfg < RFCOMMON_FBRX_NUM_TX_CONFIG_MAX)
    {
      /* Copy FBRx Droop data to FBRx DM struct */

      /** If tech is LTE, first attempt loading from the 
      band specific droop data structures **/  
      if( ( tech == RFM_LTE_MODE ) || ( tech == RFM_LTE_FTM_MODE ) )
      {
        lte_droop_data = rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.droop_data[band.lte_band];

        if( lte_droop_data != NULL )
        {
          if( lte_droop_data->num_bw_payloads_populated <= RFCOMMON_FBRX_MAX_LTE_BW_COMBOS ) 
          {
            for ( i = 0; 
                  i < (lte_droop_data->num_bw_payloads_populated); 
                  i++  ) 
            {
              if ( lte_droop_data->bw_params[i].bw_cfg ==  tx_cfg ) 
              {
                /* Config found for LTE so no need to use
                   the common droop structures*/
                use_common_droop_db = FALSE;

                memscpy( (void *)( &(fbrx_dm_ptr->droop_cal_data) ), 
                         sizeof(rfcommon_fbrx_nv_droop_type), 
                         (void *)( &(lte_droop_data->bw_params[i].droop_params) ), 
                         sizeof(rfcommon_fbrx_nv_droop_type) );
              }
            }
          }
          else
          {
            RF_MSG_2(RF_ERROR, "rfcommon_fbrx_nv_update_dm: Invalid droop cal data for LTE "
                               "band:%d tx_cfg = %d", band.lte_band, tx_cfg );
          }
        }
      }

      /*** If no exceptions, Use common droop data structures ***/
      if ( use_common_droop_db ) 
      {
        if (rfcommon_fbrx_nv_droop_cal_data[tx_cfg] != NULL) 

        {
          memscpy( (void *)(&(fbrx_dm_ptr->droop_cal_data)), 
                   sizeof(rfcommon_fbrx_nv_droop_type), 
                   (void*)(rfcommon_fbrx_nv_droop_cal_data[tx_cfg]), 
                   sizeof(rfcommon_fbrx_nv_droop_type) );
        }
        else
        {
          RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_update_dm: FBRX Droop Data is NULL for tx_cfg = %d", tx_cfg );
          api_status = RFCOMMON_FBRX_ERROR;
        }
      }

    }

    switch (tech)
    {
     #ifdef FEATURE_WCDMA
      case RFM_IMT_MODE:
        if (band.wcdma_band < RFCOM_NUM_WCDMA_BANDS)
        {
          gain_data_ptr = (rfcommon_fbrx_nv_gain_vs_freq_type_v2*) 
                           rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.gain_data[band.wcdma_band];
          sp_data_ptr = (rfcommon_fbrx_nv_attn_switch_pt_type*) 
                         rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.sp_data[band.wcdma_band];
          dc_data_ptr = (rfcommon_fbrx_dc_num_gain_type*)
               &(rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band.wcdma_band]->fbrx_mode[cur_fbrx_mode_idx]);
          temp_comp_data_ptr = (rfcommon_fbrx_temp_num_gain_type*)
               &(rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.temp_comp_data[band.wcdma_band]->fbrx_mode[cur_fbrx_mode_idx]);
          /* Compute mid-point frequency */
          for (i=0;i<num_channels;i++)
          {
            tx_freq += rfwcdma_core_get_freq_from_ul_arfcn(chan_array[i]);
          }
          tx_freq /= num_channels;
        }
        else
        {
          RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_update_dm: Invalid WCDMA Band = %d ", band.wcdma_band);
          api_status = RFCOMMON_FBRX_ERROR;
        }
        break;
        #endif /* FEATURE_WCDMA */

      #ifdef FEATURE_CDMA
      case RFM_1X_MODE:
      case RFM_1XEVDO_MODE:
      case RFM_SVDO_MODE:
        if (band.cdma_band < RFM_CDMA_MAX_BAND)
        {
          gain_data_ptr = (rfcommon_fbrx_nv_gain_vs_freq_type_v2*) 
                           rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.gain_data[band.cdma_band];
          sp_data_ptr = (rfcommon_fbrx_nv_attn_switch_pt_type*) 
                         rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.sp_data[band.cdma_band];
          dc_data_ptr = (rfcommon_fbrx_dc_num_gain_type*)
               &(rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band.cdma_band]->fbrx_mode[cur_fbrx_mode_idx]);
          temp_comp_data_ptr = (rfcommon_fbrx_temp_num_gain_type*)
               &(rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.temp_comp_data[band.cdma_band]->fbrx_mode[cur_fbrx_mode_idx]);
          /* Compute mid-point frequency */
          for (i=0;i<num_channels;i++)
          {
            tx_freq += rf_cdma_get_tx_carrier_freq(band.cdma_band, chan_array[i]);
          }
          tx_freq /= num_channels;
        }
        else
        {
          RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_update_dm: Invalid CDMA Band = %d ", band.cdma_band);
          api_status = RFCOMMON_FBRX_ERROR;
        }
        break;
      #endif

      #ifdef FEATURE_LTE
      case RFM_LTE_MODE:
      case RFM_LTE_FTM_MODE:
        if (band.lte_band < RFCOM_NUM_LTE_BANDS)
        {
          gain_data_ptr = (rfcommon_fbrx_nv_gain_vs_freq_type_v2*) 
                           rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.gain_data[band.lte_band];
          sp_data_ptr = (rfcommon_fbrx_nv_attn_switch_pt_type*) 
                          rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.sp_data[band.lte_band];
          dc_data_ptr = (rfcommon_fbrx_dc_num_gain_type*)
               &(rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band.lte_band]->fbrx_mode[cur_fbrx_mode_idx]);
          temp_comp_data_ptr = (rfcommon_fbrx_temp_num_gain_type*)
               &(rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.temp_comp_data[band.lte_band]->fbrx_mode[cur_fbrx_mode_idx]);
          /* Compute mid-point frequency */
          for (i=0;i<num_channels;i++)
          {
            tx_freq += rflte_util_get_tx_freq_from_uarfcn((rfcom_lte_earfcn_type)chan_array[i], band.lte_band);
          }
          tx_freq /= num_channels;
        }
        else
        {
          RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_update_dm: Invalid LTE Band = %d ", band.lte_band);
          api_status = RFCOMMON_FBRX_ERROR;
        }
        break;
      #endif /* FEATURE_LTE */

      #ifdef FEATURE_TDSCDMA
      case RFM_TDSCDMA_MODE:
      case RFM_TDSCDMA_MODE_FTM:
        if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
        {
          gain_data_ptr = (rfcommon_fbrx_nv_gain_vs_freq_type_v2*) 
                           rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.gain_data[band.tdscdma_band];
          sp_data_ptr = (rfcommon_fbrx_nv_attn_switch_pt_type*) 
                           rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.sp_data[band.tdscdma_band];
          dc_data_ptr = (rfcommon_fbrx_dc_num_gain_type*)
               &(rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band.tdscdma_band]->fbrx_mode[cur_fbrx_mode_idx]);
          temp_comp_data_ptr = (rfcommon_fbrx_temp_num_gain_type*)
               &(rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.temp_comp_data[band.tdscdma_band]->fbrx_mode[cur_fbrx_mode_idx]);
          /* Compute mid-point frequency */
          for (i=0;i<num_channels;i++)
          {
            tx_freq += rf_tdscdma_core_util_get_freq_from_chan(chan_array[i]);
          }
          tx_freq /= num_channels;
        }
        else
        {
          RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_update_dm: Invalid TDSCDMA Band = %d ", band.tdscdma_band);
          api_status = RFCOMMON_FBRX_ERROR;
        }
        break;
        #endif /* FEATURE_TDSCDMA */

      default:
        RF_MSG_1(RF_ERROR, "rfcommon_fbrx_nv_update_dm: Invalid Tech = %d ", tech);
        api_status = RFCOMMON_FBRX_ERROR;
        break;
    } /* switch case*/

    if (api_status == RFCOMMON_FBRX_SUCCESS)
    {
      /* Copy FBRx switch point data to FBRx DM struct */
      if (sp_data_ptr != NULL )
      {
        for(sp_idx = 0; sp_idx < MIN(sp_data_ptr->num_switch_pts, RFLM_FBRX_NUM_GAIN_STATES) ; sp_idx++)
        {
          fbrx_dm_ptr->fbrx_swp[sp_idx] = sp_data_ptr->switch_pwr_dbm[sp_idx];
        }
      }
      else
      {
        RF_MSG(RF_ERROR, "rfcommon_fbrx_nv_update_dm: FBRX Switch Point Data is NULL" );
        api_status = RFCOMMON_FBRX_ERROR;
      }
      
      /* Copy FBRx Gain Cal data to FBRx DM struct */
      if (gain_data_ptr != NULL)
      {
        for (mode_idx = 0; mode_idx < RFCOMMON_FBRX_NUM_MODES ; mode_idx++)
        {
          for(gain_idx = 0; gain_idx < RFCOMMON_FBRX_NUM_GAIN_STATES ; gain_idx++)
          {
            if(gain_data_ptr->fbrx_gain[0][mode_idx][gain_idx].data_valid == TRUE)
            {
              freq_comp_data = rfcommon_fbrx_nv_get_freq_comp_gain(tech,
                                                                   band,
                                                                   gain_data_ptr, 
                                                                   tx_freq, 
                                                                   mode_idx, 
                                                                   gain_idx,
                                                                   fbrx_dm_ptr);

              fbrx_dm_ptr->exp_gain_data.cal_params[0].fbrx_mode[mode_idx].gain_state[gain_idx].gain = 
                              freq_comp_data.gain;
              fbrx_dm_ptr->exp_gain_data.cal_params[0].fbrx_mode[mode_idx].gain_state[gain_idx].tx_pwr = 
                              freq_comp_data.tx_pwr;
              fbrx_dm_ptr->exp_gain_data.cal_params[0].fbrx_mode[mode_idx].gain_state[gain_idx].ubias = 
                              gain_data_ptr->fbrx_gain[0][mode_idx][gain_idx].ubias;
              fbrx_dm_ptr->exp_gain_data.cal_params[0].fbrx_mode[mode_idx].gain_state[gain_idx].rx_scale  = 
                              gain_data_ptr->fbrx_gain[0][mode_idx][gain_idx].rx_scale;
              fbrx_dm_ptr->exp_gain_data.cal_params[0].fbrx_mode[mode_idx].gain_state[gain_idx].predet_index  = 
                              freq_comp_data.predet_index;
            }
          } /* for gain_idx = 0 */
        }/* for mode_idx = 0 */
      }
      else
      {
        RF_MSG(RF_ERROR, "rfcommon_fbrx_nv_update_dm: FBRX Gain Cal Data is NULL" );
        api_status = RFCOMMON_FBRX_ERROR;
      }

      /* Copy FBRx DC Cal data to FBRx DM struct */  
      if (dc_data_ptr != NULL )
      {
        memscpy((void *)(&(fbrx_dm_ptr->dc_cal_data)), sizeof(rfcommon_fbrx_dc_num_gain_type), 
                (void*)dc_data_ptr, sizeof(rfcommon_fbrx_dc_num_gain_type) );
      }
      else
      {
        RF_MSG(RF_ERROR, "rfcommon_fbrx_nv_update_dm: FBRX DC Cal Data is NULL" );
        api_status = RFCOMMON_FBRX_ERROR;
      }

      /* Copy FBRx Temp Comp Cal data to FBRx DM struct */
      if (temp_comp_data_ptr != NULL )
      {
        memscpy((void *)(&(fbrx_dm_ptr->temp_comp_data)), sizeof(rfcommon_fbrx_temp_num_gain_type), 
                (void*)temp_comp_data_ptr, sizeof(rfcommon_fbrx_temp_num_gain_type) );
      }
      else
      {
        RF_MSG(RF_ERROR, "rfcommon_fbrx_nv_update_dm: FBRX Temp Comp Data is NULL" );
        api_status = RFCOMMON_FBRX_ERROR;
      }

    } /* end if (api_status == TRUE) */
  }
  else
  {
    RF_MSG(RF_ERROR, "rfcommon_fbrx_nv_update_dm: fbrx_dm_ptr = NULL");
    api_status = RFCOMMON_FBRX_ERROR;
  }

  return api_status;
} /* rfcommon_fbrx_nv_update_dm() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper API to return the bit masked FBRx Control data per tech 

  @param tech
  Tech info for which the FBRX control data is returned.
*/

rfcommon_fbrx_control_enum_type
rfcommon_fbrx_nv_get_control_val( rflm_tech_id_t tech )
{
  rfcommon_fbrx_control_enum_type fbrx_ctrl_val = 0;

  /* Update the fbrx control value in FBRX DM */    
  switch (tech)
  {
    case RFLM_TECH_WCDMA:
      fbrx_ctrl_val = RFCOMMON_FBRX_CONTROL_BITMASK & rfcommon_fbrx_sys_control_nv_data;        
      break;

    case RFLM_TECH_LTE:
      fbrx_ctrl_val = RFCOMMON_FBRX_CONTROL_BITMASK & (rfcommon_fbrx_sys_control_nv_data >> 4);
      break;

    case RFLM_TECH_TDSCDMA:
      fbrx_ctrl_val = RFCOMMON_FBRX_CONTROL_BITMASK & (rfcommon_fbrx_sys_control_nv_data >> 8);
      break;

    case RFLM_TECH_1X:
      fbrx_ctrl_val = RFCOMMON_FBRX_CONTROL_BITMASK & (rfcommon_fbrx_sys_control_nv_data >> 12);
      break;
      
    case RFLM_TECH_HDR:
      fbrx_ctrl_val = RFCOMMON_FBRX_CONTROL_BITMASK & (rfcommon_fbrx_sys_control_nv_data >> 16);        
      break;

    default:
      /* Disable FBRx */
      fbrx_ctrl_val = 0;
      break;
  }
  return fbrx_ctrl_val;
} /*rfcommon_fbrx_nv_get_control_val*/

/*----------------------------------------------------------------------------*/




rfcommon_fbrx_error_type
rfcommon_fbrx_nv_read_dc_globals( rfm_mode_enum_type tech,
                            uint16 band,
                                  uint8 fbrx_mode_index,
                                  uint8 gain_state_index,
                                  int32 * dc_i,
                                  int32 * dc_q)
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_ERROR;

  if (( dc_i == NULL) || (dc_q == NULL))
  {
    RF_MSG(RF_ERROR, "return pointers are NULL");
    return ret_value;
  }

  switch (tech)
  {
  case RFM_IMT_MODE:
    if (rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band] != NULL)
    {
      *dc_i = rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_i;
      *dc_q = rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_q;
      ret_value = RFCOMMON_FBRX_SUCCESS;
    }
    else
    {
      RF_MSG(RF_ERROR, "WCDMA dc cal global NV structure pointer is NULL");
    }
    break;
  case RFM_1X_MODE:
  case RFM_1XEVDO_MODE:
  case RFM_SVDO_MODE:
    if (rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band] != NULL)
    {
      *dc_i = rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_i;
      *dc_q = rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_q;
      ret_value = RFCOMMON_FBRX_SUCCESS;
    }
    else
    {
      RF_MSG(RF_ERROR, "CDMA dc cal global NV structure pointer is NULL");
    }
    break;
  case RFM_LTE_MODE:
    if (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band] != NULL)
    {
      *dc_i = rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_i;
      *dc_q = rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_q;
      ret_value = RFCOMMON_FBRX_SUCCESS;
    }
    else
    {
      RF_MSG(RF_ERROR, "LTE dc cal global NV structure pointer is NULL");
    }
    break;
  case RFM_TDSCDMA_MODE:
    if (rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band] != NULL)
    {
      *dc_i = rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_i;
      *dc_q = rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_q;
      ret_value = RFCOMMON_FBRX_SUCCESS;
    }
    else
    {
      RF_MSG(RF_ERROR, "TDSCDMA dc cal global NV structure pointer is NULL");
    }
    break;
  default:
    RF_MSG(RF_ERROR, "RF Mode not supported for this NV");
  }

  return ret_value;

}

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_update_dc_globals( rfm_mode_enum_type tech,
                            uint16 band,
                                    uint8 fbrx_mode_index,
                                    uint8 gain_state_index,
                                    int32 dc_i,
                                    int32 dc_q)
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_ERROR;
  uint32 handle=0;
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;

  switch (tech)
  {
  case RFM_IMT_MODE:
    if (rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band] != NULL)
    {
      rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_i = dc_i;
      rfcommon_fbrx_nv_tech_cal_data.wcdma_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_q = dc_q;
      ret_value = RFCOMMON_FBRX_SUCCESS;
    }
    else
    {
      RF_MSG(RF_ERROR, "WCDMA droop cal global NV structure pointer is NULL");
    }
    break;
  case RFM_1X_MODE:
  case RFM_1XEVDO_MODE:
  case RFM_SVDO_MODE:
    if (rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band] != NULL)
    {
      rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_i = dc_i;
      rfcommon_fbrx_nv_tech_cal_data.cdma_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_q = dc_q;
      ret_value = RFCOMMON_FBRX_SUCCESS;
    }
    else
    {
      RF_MSG(RF_ERROR, "CDMA dc cal global NV structure pointer is NULL");
    }
    break;
  case RFM_LTE_MODE:
    if (rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band] != NULL)
    {
      rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_i = dc_i;
      rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_q = dc_q;
      ret_value = RFCOMMON_FBRX_SUCCESS;
    }
    else
    {
      RF_MSG(RF_ERROR, "LTE dc cal global NV structure pointer is NULL");
    }
    break;
  case RFM_TDSCDMA_MODE:
    if (rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band] != NULL)
    {
      rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_i = dc_i;
      rfcommon_fbrx_nv_tech_cal_data.tdscdma_cal_data.dc_data[band]->fbrx_mode[fbrx_mode_index].gain_state[gain_state_index].dc_q = dc_q;
      ret_value = RFCOMMON_FBRX_SUCCESS;
    }
    else
    {
      RF_MSG(RF_ERROR, "TDSCDMA dc cal global NV structure pointer is NULL");
    }
    break;
  default:
    RF_MSG(RF_ERROR, "RF Mode not supported for this NV");
  }

  for (handle=0;handle<RFLM_HANDLE_COUNTS;handle++)
  {
    fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(handle);

    if (fbrx_data_ptr != NULL && gain_state_index < RFLM_FBRX_NUM_GAIN_STATES)
    {  
      fbrx_data_ptr->dc_cal_data.gain_state[gain_state_index].dc_i = dc_i;
      fbrx_data_ptr->dc_cal_data.gain_state[gain_state_index].dc_q = dc_q;
    }
  }

  return ret_value;

}

const rfcommon_fbrx_nv_gain_vs_freq_type_v2* rfcommon_fbrx_nv_get_gain_cal_data
(
  rfm_mode_enum_type tech,
  rfcom_band_type_u band
)
{
  const rfcommon_fbrx_nv_gain_vs_freq_type_v2 *gain_data_ptr = NULL;

  switch (tech)
  {
    case RFM_LTE_MODE:
    case RFM_LTE_FTM_MODE:
      if (band.lte_band < RFCOM_NUM_LTE_BANDS)
      {
        gain_data_ptr = (rfcommon_fbrx_nv_gain_vs_freq_type_v2*) 
                         rfcommon_fbrx_nv_tech_cal_data.lte_cal_data.gain_data[band.lte_band];
      }
      break;

    default:
      break;
  }
  return gain_data_ptr;
}
