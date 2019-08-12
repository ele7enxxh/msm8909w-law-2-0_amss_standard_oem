 
/*!
  @file
  ftm_calibration_v3_data.c

  @brief
  This module contains common RF calibration code for Data Opcode

  @addtogroup RF_COMMON_FTM_CALV3_DATA

  @brief
  This module provides a common framework to perform Data OpCode operation.
  This module is responsible for calling the WAN technology specific handler
  function to perform various Data operations. This module is also responsible
  for populating the Data results buffer with appropriate calibration data.
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_calibration_v3_data.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/03/16   am      Store PIN cal offset correctly in lte nv table 
09/21/15   pk      Initialize Number of selection sweep powers for Meas Pwr and Calc Pwr List in ET/EPT to 0
02/27/15   aa      Compiler warning fix
02/25/15   myu     Add sub-opcode handling for extended PIN CAL process
11/17/14   vv      Check for papm et voltage min and max only in ET mode during rfcal
11/12/14   spa     Change tx_nv_path to be per band per logical device
11/03/14   id      Add support for populating Rx PAR as part of XPT capture Results opcode
10/29/14   vb      Support for QPOET/PAPM ET voltage API
09/18/14   xsh    Fix compiler warning
09/03/14   cd      Remove legacy Tx override fields from data module
08/14/14   aj      Flushed DPD Result table post returning XPT Capture results.
                   Fixed population of ICQ and NUM_RESULTS field in the response packet.
07/16/14   aa      load DPD tables to HW during EPT and ET cal
07/10/14   cd/cri  Merge Pin Cal
05/16/14   cri     IQ capture segment reduction changes
04/24/14   zhw     Use rfcommon_mdsp_xpt_mode_enum_type for all DPD loading
03/14/14   zhh    use debug_mode_en to control some ET debug info
2/19/14    rsr     Added Mline bias option to pick Mline entry based on proximity
                   to target power.
01/08/14   cac     Add RGI delta table for EPT ET in different PA states
01/02/14   cd      Fixed compiler warning
01/02/14   cd      Load DPD Tx override fixes for xPT Cal
12/27/13   rsr     Added support for XPT capture results opcode.
12/11/13   cd      Change Tx override handlers to use RFLM Tx override for xPT 
                   cal
11/22/13   id      Load DPD changes for BOLT
10/30/13    pk     Added F3s enable/disable conditions to optimize the cal time
10/25/13   id      Changes to make Load DPD common for use across modems
10/23/13   nrk     Fix compiler warnings
09/10/13   shk     Support to pack M Line results in the response packet
08/08/13   sl/cri  Common implementation for DPD scaling
07/15/13   cri     Immediate MSM register programming for tx override
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
07/05/13   cri     Choose final m-line point if no bias is greater than max
06/27/13   kai/id  Program QPOET into APT mode before loading multiple DPD tables
06/23/13   kai     Added WCDMA 16channels xpt delay support
06/19/13    pv     Static NV loading bug fix. Impacts B28B ET.
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/21/13   jmf     [xPT] ET delay implementation for Cal V4
06/05/13   rsr     Mline opcode parameter packet change :- 
                     *  Separate EPT/ Et target powers 
                     *  Bias limiting option - Cap Bias at Vdmax or interpolat
06/03/13   rsr     Fixed Mline interpolation code to work for both EPT and ET. 
                   Earlier checkin only fixed EPT.
05/28/13   rsr     Fix Mline to save and restore delay action type from DPD map. 
05/08/13   id/kai  1. Use correct Env Scale NV for EPT and ET
                   2. Fix DPD Valid check and result buffer fill
                   3. DPD Process will now change Env Scale Action to Override
                   from NV for Sweep 3
                   4. Fixed "measured" flag in result buffer for calc powers
05/04/13    id     Add support for VdMax for EPT and ET
05/02/13    cd     Copy over IQ gain and envelope scale action types to DPD map
                   during Mline computation
05/02/13    id     Add Support to populate EPDT Results to result buffer
04/30/13    id     Add Load ISC and Load SC for Calc DPD Load and some clean up
04/23/13    id     Add Reference Channel in data payload
04/20/13    id     Bug fix to load correct DPD Tables during Swp3
04/19/13    id     1. Support for Swp2 and 3
                   2. Support for Data Opcode Actions for DPD Proc and Load
                      Calc DPD
04/18/13   kai     Changed wcdma tx agc override api
04/16/13   aro     Moved Opcode Data to HK data structure
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/11/13   cri     Use XPT mode in power lookup for uniqueness 
04/10/13    id     Do Mline Calculation on FTM Task
04/09/13    id     Fix KW Error in process_rgi_pwr_tbl_instr
04/09/13    id     Support for RGIvsPwr Tbl and Mline Calc Data Actions
04/02/13   aro     Debug message update
04/01/13   aro     Fixed unpack function signature
03/29/13   aro     Radio Config/Retune optimized opcode proc functions
03/29/13   aro     Tx Config/Meas optimized opcode proc functions
03/29/13   aro     Rx Config/Meas optimized opcode proc functions
03/27/13   aro     Data structure restructing
03/25/13    id     Store DET table in Data Module
01/01/13    id     Initial version

==============================================================================*/

#include "comdef.h"
#include "ftm_common_calibration_v3.h"
#include "ftm_calibration_v3_data.h"
#include "ftm_calibration_data_module.h"
#include "ftm_common_data.h"
#include "ftm_common_xpt.h"
#include "ftm_common_control.h"
#include "ftm_msg.h"
#include "ftmdiag.h"
#include "ftm.h"
#include "ftm_rf_cmd.h"
#include "rfcommon_math.h"
#include "modem_mem.h"
#include "rfcommon_mdsp_types.h"
#include "rfcommon_core_xpt.h"
#include "rfcommon_mc.h"
#include "rfcommon_core_utils.h"
#include "diagcmd.h"
#include "math.h"
#include "stringl.h" /* for memscpy */
#include "rfc_card.h"
#include "rfdevice_papm_intf.h"

#ifdef FEATURE_CDMA1X
#include "ftm_1x_control.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_mc.h"
#include "rfdevice_cdma_interface.h"
#endif

#ifdef FEATURE_LTE
#include "rflte_ftm_external_api.h"
#include "rflte_nv.h"
#include "rflte_msm.h"
#include "rflte_core_util.h"
#include "rflte_ftm_mc.h"
#include "rfdevice_lte_interface.h"
#endif

#ifdef FEATURE_WCDMA
#include "ftm_wcdma_ctl.h"
#include "rfwcdma_msm.h"
#include "rfnv_wcdma.h"
#include "rfdevice_wcdma_intf.h"
#endif

#ifdef FEATURE_WCDMA
extern rfcom_wcdma_band_type ftm_curr_wcdma_mode;
extern rfnv_wcdma_nv_tbl_type *rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_supported_mode_type band );
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
#include "ftm_tdscdma_ctl.h"
#include "rf_tdscdma_msm.h"
#include "rf_tdscdma_core_util.h"
#endif

#ifdef FEATURE_LTE
extern rflte_nv_tbl_type* rflte_mc_get_nv_tbl_ptr(uint32 carrier_idx);
#endif

#ifdef FEATURE_TDSCDMA
extern int32 rf_tdscdma_msm_get_rms_dbfs_x100_val( void );
#endif


/*============================================================================*/
/*!
  @name Data OpCode handling

  @brief
  This section contains functions for Data opcode handling.
*/
/*! @{ */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function for scaling detrough table
 
  @details
  Use this function to scale the detrough table based on the calculated bias.
 
  @param epdt_tbl
  Pointer to the detrough table to scale
 
  @return
  Boolean to indicate pass/fail
*/
boolean 
ftm_calibration_data_scale_detrough
(
  ftm_cal_data_det_type *epdt_tbl,
  uint16 epdt_size,
  uint16 qpoet_min_x1000,
  uint16 qpoet_max_x1000,
  uint16 et_min_x1000,
  int16 mline_bias,
  boolean debug_en
)
{
  boolean return_val = TRUE;
  uint16 scaled_val = 0;
  uint16 two_pow_15 = 32768;
  uint16 i = 0;

  if( epdt_tbl == NULL )
  {
    FTM_MSG( FTM_ERROR, "Null Pointer for Detrough Table in Scaling");
    return FALSE;
  }

  for( i = 0; i < epdt_size; i++ )
  {
    //unsigned short detScaled = (unsigned short) floor(((Vdmax_mline-Vdmin_et)/(Vdmax_qpoet-Vdmin_qpoet)*detUnityVec[i] + (Vdmin_et-Vdmin_qpoet)/(Vdmax_qpoet-Vdmin_qpoet)*POW_2_15) + 0.5);
    scaled_val = (uint16)(floor(((float)(mline_bias-et_min_x1000)/(float)(qpoet_max_x1000-qpoet_min_x1000)*(float)epdt_tbl->table[i]) + 
                                 ((float)(et_min_x1000 - qpoet_min_x1000)/(float)(qpoet_max_x1000 - qpoet_min_x1000)*(float)two_pow_15 ) + 
                                 0.5));

    //FTM_MSG_2( FTM_MED, "Detrough = %d, Scaled Detrough = %d", epdt_tbl->table[i], scaled_val);
    epdt_tbl->table[i] = scaled_val;
  }

  return return_val;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the RGI PWR Table Data Sub Opcode Instruction
 
  @details
  Process the RGI PWR Table for the Data Opcode.
  The payload includes the RGI vs Pwr table which needs to
  be saved in the data module for future processing.
 
  @param data_param
  Pointer to the data payload
 
  @return
  Boolean to indicate pass/fail
*/
boolean 
ftm_calibration_data_process_rgi_pwr_tbl_instr
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  uint32 chan,
  boolean mul_table,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{ 
  uint8 num_rgi_pwr_pairs = 0;
  uint8 lp_var = 0;
  boolean return_val = FALSE;
  ftm_cal_data_rgid_type *rgid_tbl = NULL;
  uint8 *res_write_ptr; /* results write pointer */
  uint16 written_size = 0; /* Total written size */
  uint16 free_size = 0; /* Total free size */
  uint8 res_type = FTM_CAL_OPCODE_DATA;
  uint8 res_action_type = DATA_RES_ACTION_RGI_PWR_TBL;
  uint8 version = 0;

  num_rgi_pwr_pairs = 
    data_param->data_param.rgi_pwr.num_rgi_pwr_pairs;
  if (cal_data->cal_config.debug_mode_en)
  {
   FTM_MSG_1( FTM_MED, "Num RGI Pwr Pairs = %d", num_rgi_pwr_pairs);
  }

  if( num_rgi_pwr_pairs > MAX_NUM_RGI_PWR_PAIRS )
  {
    FTM_MSG_1( FTM_ERROR, "Number of RGI Delta Values "
                          "cannot be greater than %d!", MAX_NUM_RGI_PWR_PAIRS);

    return FALSE;
  }

  rgid_tbl = (ftm_cal_data_rgid_type *)modem_mem_alloc(
             sizeof(ftm_cal_data_rgid_type), MODEM_MEM_CLIENT_RFA);

  if( rgid_tbl == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for RGI Delta Table!");
    return FALSE;
  }

  /* Debug Print */
  if( cal_data->cal_config.debug_mode_en )
  {
    FTM_MSG( FTM_MED, "=== RGI Pwr Table ===");
  }
  
  for( lp_var = 0; lp_var < num_rgi_pwr_pairs; lp_var++ )
  {
    rgid_tbl->table[lp_var].rgi = 
      (uint16)data_param->data_param.rgi_pwr.rgi_pwr_tbl[lp_var].rgi;
    rgid_tbl->table[lp_var].power = 
      (int32)data_param->data_param.rgi_pwr.rgi_pwr_tbl[lp_var].pwr_db100;

  if( cal_data->cal_config.debug_mode_en )
  {
      FTM_MSG_2( FTM_MED, "RGI, Power = %d, %d", 
                 rgid_tbl->table[lp_var].rgi,
                 rgid_tbl->table[lp_var].power);
    }
  }

  /* Fill invalid values for the rest of the table */
  for( ; lp_var < RFCOM_TXLIN_NUM_SEGMENTS; lp_var++ )
  {
    rgid_tbl->table[lp_var].rgi = RGID_INV_RGI;
    rgid_tbl->table[lp_var].power = RGID_INV_PWR;

    if( cal_data->cal_config.debug_mode_en )
    {
      FTM_MSG_2( FTM_MED, "RGI, Power = %d, %d", 
                 rgid_tbl->table[lp_var].rgi,
                 rgid_tbl->table[lp_var].power);
    }
  }

  /* Call Data Module Function here */ 
  if (mul_table) 
  {
    return_val = ftm_cal_data_push_multiple_rgid_data( chan, 
                                                     data_param->data_param.rgi_pwr.pa_state, 
                                                     data_param->data_param.rgi_pwr.xptMode,
                                                     rgid_tbl);
  }
  else
  {
  return_val = ftm_cal_data_push_rgid_data( chan, rgid_tbl );
  }
  

  

  if( return_val == TRUE )
  {
    /* Now populate the result buffer from cal_result */
    /* Initialize the free size */ 
    free_size = oc_disp->res_buff_info.buff_sz_max;

    /*==========================================================================*/
    /* Populate the results buffer */
    /*==========================================================================*/

    /* Initialize the write pointer */
    res_write_ptr = oc_disp->res_buff_info.buff_ptr;

    /*==========================================================================*/
    /* Populate Results Type */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA RGI PWR TBL.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */
	
    /*==========================================================================*/
    /* Populate version Type */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(version), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE.{0x%x}] "
                 "ResultPush : Version - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ! ( free_size >= sizeof(uint8) ) */
  
    /*==========================================================================*/
    /* Populate Device */
    /*==========================================================================*/

    if ( free_size >= sizeof(ftm_cal_data_device_type) )
    {
      memscpy( res_write_ptr, sizeof(ftm_cal_data_device_type), &(data_param->device), 
  		      sizeof(ftm_cal_data_device_type));
	  UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
	  				     sizeof(ftm_cal_data_device_type) );
	  free_size -= sizeof(ftm_cal_data_device_type);
    } /* if ( free_size >= sizeof(ftm_cal_data_device_type) ) */
    else
    {
	  FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA DPD CONSTRUCTION.{0x%x}] "
		         "ResultPush : dMask - ReqSize %d FreeSize %d",
	  		     cal_data->seg_data.cal_instance,
			     FTM_CAL_OPCODE_DATA,
			     sizeof(ftm_cal_data_device_type),
			     free_size );
    } /* if ! ( free_size >= sizeof(ftm_cal_data_device_type) ) */

    /*==========================================================================*/
    /* Populate Results Action */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_action_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA RGI PWR TBL.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_RGI_PWR_TBL,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Update Result Written Size */
    /*==========================================================================*/
    oc_disp->res_buff_info.buff_sz = written_size;

    if ( cal_data->cal_config.debug_mode_en )
    {
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "ResultPush : %d bytes",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 written_size );
    }
  }

  /* Free allocated memory */
  if( rgid_tbl != NULL )
  {
    modem_mem_free((void*)rgid_tbl, MODEM_MEM_CLIENT_RFA);
  }

  return return_val;
} /* ftm_calibration_data_process_rgi_pwr_tbl_instr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the Load DPD Data Sub Opcode Instruction
 
  @details
  Process the Load DPD Table for the Data Opcode.
  The payload includes the AMAM, AMPM tables which needs to
  be loaded in the HW for EPT Mode and AMAM, AMPM and DET
  tables to be loaded in the HW for ET mode.
 
  @param data_param
  Pointer to the data payload
 
  @param ftm_rf_tech
  Current FTM RF Technology
 
  @return
  Boolean to indicate pass/fail
*/
boolean 
ftm_calibration_data_process_load_dpd_tbl_instr
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  ftm_rf_technology_type ftm_rf_tech,
  uint8 device_ind,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  uint8 num_amam = 0;
  uint8 num_ampm = 0;
  uint8 num_det = 0;
  uint16 *amam_ptr = NULL;
  uint16 *ampm_ptr = NULL;
  uint16 *det_ptr = NULL;
  rfcommon_mdsp_xpt_mode_enum_type xpt_type = 0;
  //uint8 lp_var = 0;
  boolean return_val = TRUE;

  uint8   amam_bank_index = -1;
  uint16  amam_bank_offset = -1;
  uint8   ampm_bank_index = -1;
  uint16  ampm_bank_offset = -1;
  uint8   epdt_bank_index = -1;
  uint16  epdt_bank_offset = -1;
  ftm_xpt_txagc_override_type ovr_params;
  ftm_cal_data_unity_hwid_type* unity_hwids = NULL;
  rfcommon_core_xpt_tx_override_type curr_tx_ovr;

  uint8 *res_write_ptr; /* results write pointer */
  uint16 written_size = 0; /* Total written size */
  uint16 free_size = 0; /* Total free size */
  uint8 res_type = FTM_CAL_OPCODE_DATA;
  uint8 res_action_type = DATA_RES_ACTION_PUSH_DPD_TBL;
  uint8 version = 0;

  /* Get the XPT Type */
  if ( data_param->data_param.load_dpd.xpt_type == 0 )
  {
    xpt_type = RFCOMMON_MDSP_XPT_MODE_EPT;
  }
  else if ( data_param->data_param.load_dpd.xpt_type == 1 )
  {
    xpt_type = RFCOMMON_MDSP_XPT_MODE_ET;
  }
  else
  {
    xpt_type = 0xFF;
  }

  if ( cal_data->cal_config.debug_mode_en ) 
  {  
   FTM_MSG_1( FTM_MED, "Load DPD XPT Type = %d", xpt_type);
  }

  /* Get num AMAM, num AMPM and num DET */
  num_amam = data_param->data_param.load_dpd.num_amam;
  if (cal_data->cal_config.debug_mode_en)
  {
   FTM_MSG_1( FTM_MED, "Load DPD Num AMAM = %d", num_amam);
  }
  num_ampm = data_param->data_param.load_dpd.num_ampm;
  if (cal_data->cal_config.debug_mode_en)
  {
   FTM_MSG_1( FTM_MED, "Load DPD Num AMPM = %d", num_ampm);
  }
  num_det = data_param->data_param.load_dpd.num_det;
  if (cal_data->cal_config.debug_mode_en)
  {
   FTM_MSG_1( FTM_MED, "Load DPD Num DET = %d", num_det);
  }

  if( num_amam == 0 && num_ampm == 0 && num_det == 0 )
  {
    FTM_MSG( FTM_ERROR, "Load DPD Num AMAM, Num AMPM and Num DET "
                        "are zero ");
    return FALSE;
  }

  /* Debug Print */
  /*
  if( data_param->debug_mode_en )
  {
    FTM_MSG( FTM_MED, "Load DPD AMAM Table");
    for( lp_var = 0; lp_var < num_amam; lp_var++ )
    {
      FTM_MSG_2( FTM_MED, "AMAM[%d] = %d", lp_var, 
                 data_param->data_param.load_dpd.amam[lp_var]);
    }

    FTM_MSG( FTM_MED, "Load DPD AMPM Table");
    for( lp_var = 0; lp_var < num_ampm; lp_var++ )
    {
      FTM_MSG_2( FTM_MED, "AMPM[%d] = %d", lp_var, 
                 data_param->data_param.load_dpd.ampm[lp_var]);
    }

    if( xpt_type == 1 )
    {
      FTM_MSG( FTM_MED, "Load DPD EPDT Table");
      for( lp_var = 0; lp_var < num_det; lp_var++ )
      {
        FTM_MSG_2( FTM_MED, "EPDT[%d] = %d", lp_var, 
                   data_param->data_param.load_dpd.det[lp_var]);
      }
    }
  }
  */

  /* Program QPOET into APT mode */
  /* Get the current Tx override */
  memset( &curr_tx_ovr, 0, sizeof(rfcommon_core_xpt_tx_override_type) );
  return_val &= ftm_common_xpt_get_curr_tx_override( device_ind, 
                                                     &curr_tx_ovr );
  ovr_params.device = device_ind;
  ovr_params.xpt_mode = RFCOMMON_MDSP_XPT_MODE_APT;
  ovr_params.iq_gain_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
  ovr_params.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
  ovr_params.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
  ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID; 
  ovr_params.rgi = curr_tx_ovr.rgi;
  ovr_params.smps_bias = 2000;
  ovr_params.pa_state = curr_tx_ovr.pa_state;
  ovr_params.pa_curr = curr_tx_ovr.pa_curr;
  ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
  ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;

  switch( ftm_rf_tech )
  {
  case FTM_RF_TECH_CDMA:
      #ifdef FEATURE_CDMA1X
      return_val &= ftm_1x_set_xpt_txagc_override( &ovr_params );
      #endif
      break;
  case FTM_RF_TECH_WCDMA:
      #ifdef FEATURE_WCDMA
      return_val &= ftm_wcdma_xpt_override_rf_settings( &ovr_params );
      #endif
      break;
  case FTM_RF_TECH_LTE:
      #ifdef FEATURE_LTE
      return_val &= rflte_ftm_xpt_override_rf_settings( &ovr_params );
      #endif
      break;
  case FTM_RF_TECH_TDSCDMA:
      #ifdef FEATURE_TDSCDMA
      return_val &= ftm_tdscdma_xpt_override_rf_settings( &ovr_params );
      #endif
      break;
  default:
      break;
  }
  if (cal_data->cal_config.debug_mode_en)
  {
   FTM_MSG_1( FTM_MED, "Return Value from Tx Override = %d", return_val);
  }

  /* Load the DPD Tables to HW */
  if(return_val == TRUE)
  {
    /* Clear dpd tables of any previous data */
    return_val &= 
      rf_common_xpt_load_dpd_tbls(device_ind, 
                                  NULL, 
                                  0, 
                                  0, 
                                  &amam_bank_index, 
                                  &amam_bank_offset, 
                                  XPT_DPD_ACTION_INIT,
                                  xpt_type);
  }

  amam_bank_index = -1;
  amam_bank_offset = -1;
  /* Point to AMAM Table */
  amam_ptr = (uint16*)(&(data_param->data_param.load_dpd.amam[0]));
  if((return_val == TRUE) && (num_amam > 0) && (amam_ptr != NULL))
  {
    /* Write AMAM to table */
    return_val &=
      rf_common_xpt_load_dpd_tbls(device_ind,
                                   amam_ptr,
                                   num_amam,
                                   DPD_TYPE_AMAM,
                                   &amam_bank_index,
                                  &amam_bank_offset, 
                                  XPT_DPD_ACTION_SAVE,
                                  xpt_type); 
  }
  if (cal_data->cal_config.debug_mode_en)
  {
   FTM_MSG_1( FTM_MED, "AMAM Bank Index = %d", amam_bank_index);
   FTM_MSG_1( FTM_MED, "AMAM Bank Offset = %d", amam_bank_offset);
  }
  ampm_bank_index = -1;
  ampm_bank_offset = -1;
  /* Point to AMPM Table */
  ampm_ptr = (uint16*)(&(data_param->data_param.load_dpd.ampm[0]));
  if((return_val == TRUE) && (num_ampm > 0) && (ampm_ptr != NULL))
  {
    /* Write AMPM to table */
    return_val &=
      rf_common_xpt_load_dpd_tbls(device_ind, 
                                   ampm_ptr,
                                   num_ampm,
                                   DPD_TYPE_AMPM,
                                   &ampm_bank_index,
                                  &ampm_bank_offset, 
                                  XPT_DPD_ACTION_SAVE,
                                  xpt_type);
  }
  if (cal_data->cal_config.debug_mode_en)
  {
   FTM_MSG_1( FTM_MED, "AMPM Bank Index = %d", ampm_bank_index);
   FTM_MSG_1( FTM_MED, "AMPM Bank Offset = %d", ampm_bank_offset);
  }

  epdt_bank_index = -1;
  epdt_bank_offset = -1;
  
  if((return_val == TRUE) && (num_det > 0) && (xpt_type == RFCOMMON_MDSP_XPT_MODE_ET))
  {
    /* Point to DET Table */
    det_ptr = (uint16*)(&(data_param->data_param.load_dpd.det[0]));

    /* Write EPDT to table if not EPT */
    if( det_ptr != NULL )
    {
      return_val &=
        rf_common_xpt_load_dpd_tbls(device_ind, 
                                     det_ptr,
                                     num_det,
                                     DPD_TYPE_EPDT,
                                     &epdt_bank_index,
                                    &epdt_bank_offset, 
                                    XPT_DPD_ACTION_SAVE,
                                    xpt_type);

      if (cal_data->cal_config.debug_mode_en)
      {
       FTM_MSG_1( FTM_MED, "DET Bank Index = %d", epdt_bank_index);
       FTM_MSG_1( FTM_MED, "DET Bank Offset = %d", epdt_bank_offset);
      }
    }
  }

  if(return_val == TRUE)
  {
    /* Commit DPD tables to HW */
    return_val &= 
      rf_common_xpt_load_dpd_tbls(device_ind, 
                                  NULL, 
                                  0, 
                                  0, 
                                  &amam_bank_index, 
                                  &amam_bank_offset, 
                                  XPT_DPD_ACTION_COMMIT,
                                  xpt_type);

    /* Get the current Tx override and update only the DPD information */
    memset( &curr_tx_ovr, 0, sizeof(rfcommon_core_xpt_tx_override_type) );
    return_val &= ftm_common_xpt_get_curr_tx_override( device_ind, 
                                                       &curr_tx_ovr );
    curr_tx_ovr.amam_addr.mem_bank_id = amam_bank_index;
    curr_tx_ovr.amam_addr.start_offset = amam_bank_offset;
    curr_tx_ovr.amam_addr.tbl_size = num_amam;
    curr_tx_ovr.ampm_addr.mem_bank_id = ampm_bank_index;
    curr_tx_ovr.ampm_addr.start_offset = ampm_bank_offset;
    curr_tx_ovr.ampm_addr.tbl_size = num_ampm;
    curr_tx_ovr.detrough_addr.mem_bank_id = epdt_bank_index;
    curr_tx_ovr.detrough_addr.start_offset = epdt_bank_offset;
    curr_tx_ovr.detrough_addr.tbl_size = num_det;

    /* Update the current set of overrides to FTM common data */
    return_val &= ftm_common_xpt_set_curr_tx_override( device_ind, 
                                                       &curr_tx_ovr );
  }

  /* Save DET table in Data Module */
  if( (xpt_type == RFCOMMON_MDSP_XPT_MODE_ET) && (det_ptr != NULL) )
  {
    if (cal_data->cal_config.debug_mode_en)
    {
     FTM_MSG( FTM_MED, "Store DET Table in Data Module!");
    }
    return_val &= 
      ftm_cal_data_store_nom_epdt_table( (ftm_cal_data_det_type*)det_ptr );
  }

  /* Save the HW IDs for Unity DPD in Data Module */
  unity_hwids = (ftm_cal_data_unity_hwid_type*)modem_mem_alloc(
                sizeof(ftm_cal_data_unity_hwid_type), MODEM_MEM_CLIENT_RFA);

  if( unity_hwids == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for Unity HW IDs!");
    return_val &= FALSE;
  }
  else
  {
    unity_hwids->amam_addr.mem_bank_id = amam_bank_index;
    unity_hwids->amam_addr.start_offset = amam_bank_offset;
    unity_hwids->amam_addr.tbl_size = RFCOMMON_MDSP_DPD_SIZE_64;

    unity_hwids->ampm_addr.mem_bank_id = ampm_bank_index;
    unity_hwids->ampm_addr.start_offset = ampm_bank_offset;
    unity_hwids->ampm_addr.tbl_size = RFCOMMON_MDSP_DPD_SIZE_64;

    if( xpt_type == RFCOMMON_MDSP_XPT_MODE_ET )
    {
      unity_hwids->detrough_addr.mem_bank_id = epdt_bank_index;
      unity_hwids->detrough_addr.start_offset = epdt_bank_offset;
      unity_hwids->detrough_addr.tbl_size = RFCOMMON_MDSP_DPD_SIZE_128;
    }
    
    FTM_MSG( FTM_MED, "Store Unity HW IDs in Data Module!");
    return_val &= ftm_cal_data_set_unity_hwids( unity_hwids );
  }

  if( unity_hwids != NULL )
  {
    modem_mem_free((void*)unity_hwids, MODEM_MEM_CLIENT_RFA);
  }

  if( return_val == TRUE )
  {
    /* Now populate the result buffer from cal_result */
    /* Initialize the free size */ 
    free_size = oc_disp->res_buff_info.buff_sz_max;

    /*==========================================================================*/
    /* Populate the results buffer */
    /*==========================================================================*/

    /* Initialize the write pointer */
    res_write_ptr = oc_disp->res_buff_info.buff_ptr;

    /*==========================================================================*/
    /* Populate Results Type */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PUSH DPD TBL.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */
	
	  /*==========================================================================*/
    /* Populate version Type */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(version), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE.{0x%x}] "
                 "ResultPush : Version - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ! ( free_size >= sizeof(uint8) ) */
  
    /*==========================================================================*/
    /* Populate Device */
    /*==========================================================================*/

    if ( free_size >= sizeof(ftm_cal_data_device_type) )
    {
      memscpy( res_write_ptr, sizeof(ftm_cal_data_device_type),&(data_param->device), 
  		      sizeof(ftm_cal_data_device_type));
	  UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
	  				     sizeof(ftm_cal_data_device_type) );
	  free_size -= sizeof(ftm_cal_data_device_type);
    } /* if ( free_size >= sizeof(ftm_cal_data_device_type) ) */
    else
    {
	  FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA DPD CONSTRUCTION.{0x%x}] "
		         "ResultPush : dMask - ReqSize %d FreeSize %d",
	  		     cal_data->seg_data.cal_instance,
			     FTM_CAL_OPCODE_DATA,
			     sizeof(ftm_cal_data_device_type),
			     free_size );
    } /* if ! ( free_size >= sizeof(ftm_cal_data_device_type) ) */

    /*==========================================================================*/
    /* Populate Results Action */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_action_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PUSH DPD TBL.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PUSH_DPD_TBL,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Update Result Written Size */
    /*==========================================================================*/
    oc_disp->res_buff_info.buff_sz = written_size;

    if ( cal_data->cal_config.debug_mode_en )
    {
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "ResultPush : %d bytes",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 written_size );
    }
  }
  if (cal_data->cal_config.debug_mode_en)
  {
   FTM_MSG( FTM_MED, "Done with DPD Load Action!");
  }

  return return_val;
} /* ftm_calibration_data_process_load_dpd_tbl_instr */

boolean ftm_calibration_mline_select_rows(
   uint32 chan,
   ftm_common_xpt_mline_outputs* mline_output, 
   ftm_calibration_mline_result_xpt_type* mline_result_pkt, 
   ftm_cal_data_dpd_map_row_type* mline_out_dpd_map_row,
   ftm_cal_data_parameter_type *data_param,
   uint8 num_rgi_delta,
   ftm_common_rgi_delta_data *rgi_delta_data_ptr,
   ftm_cal_data_dpd_map_row_type * saved_dpd_map_row,
   boolean is_ept, 
   boolean is_meas,
   boolean debug_on)
{
  char eptStr[4] ;
  char measStr[5];
  ftm_calibration_mline_result_selec_sweep_type* result_ptr;
  boolean interp_bias = TRUE;
  uint8 max_mline_index = 0;
  int16 lp_var, selected_ind=-1;
  int16 i, curr_power = 0;
  uint32 interped_bias = 0;
  boolean return_val = TRUE;

  /* PARAMS THAT VARY FOR ET VS EPT. */
  uint8 mline_bias_option; 
  uint32 vd_max; 
  uint8 num_target_pwrs; 
  int16* target_pwrs;
  int8 pastate;

  
  /* -------------- GET EPT/ ET or MEAS/CALC and extract params accordingly-------------*/
  if ( is_ept )
  {
    strlcpy(eptStr,"EPT",strlen("EPT")+1);
    vd_max = data_param->data_param.mline_params.vd_max_ept;
    pastate = data_param->data_param.mline_params.pa_ept;
    if ( is_meas )
    {
      strlcpy(measStr,"Meas",strlen("Meas")+1);
      result_ptr = &mline_result_pkt->selec_swp_meas_result;
      num_target_pwrs = data_param->data_param.mline_params.num_swp2_ept_meas_pwrs;
      target_pwrs = &data_param->data_param.mline_params.swp2_ept_meas_pwrs[0]; 
    }
    else
    {
       strlcpy(measStr,"Calc",strlen("Calc")+1);
       result_ptr = &mline_result_pkt->selec_swp_calc_result;
       num_target_pwrs = data_param->data_param.mline_params.num_swp2_ept_calc_pwrs;
       target_pwrs = &data_param->data_param.mline_params.swp2_ept_calc_pwrs[0]; 
    }
  }
  else
  {
    strlcpy(eptStr,"ET",strlen("ET")+1);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
    vd_max = data_param->data_param.mline_params.vd_max_et;
    pastate = data_param->data_param.mline_params.pa_et;
    if ( is_meas )
    {
      strlcpy(measStr,"Meas",strlen("Meas")+1);
      result_ptr = &mline_result_pkt->selec_swp_meas_result;
      num_target_pwrs = data_param->data_param.mline_params.num_swp2_et_meas_pwrs;
      target_pwrs = &data_param->data_param.mline_params.swp2_et_meas_pwrs[0]; 
    }
    else
    {
      FTM_MSG(FTM_ERROR, "ET CALC not supported");
      return FALSE;
    }
  }

  if ( num_target_pwrs == 0 )
  {
    return TRUE;
  }


  mline_bias_option = data_param->data_param.mline_params.mline_bias_option;

  /* ----------------GET MAX MLINE INDEX & READ MLINE BIAS OPTION ----------------------------*/
  max_mline_index = num_rgi_delta-1;
  /* Find the index in MLine Table where first time Bias > Vd Max EPT */
  switch ( mline_bias_option )
  {
    case DATA_MLINE_CALC_INTERP_BIAS:
      interp_bias = TRUE;
      for( i = 0; i < num_rgi_delta; i++ )
      {
        if( mline_output[i].bias_out > vd_max )
        {         
          max_mline_index = i ;
          break;
        }
      }
      break;
    case DATA_MLINE_CALC_CLOSEST_POWER_INTERP_BIAS:
      interp_bias = TRUE;
      for( i = 0; i < num_rgi_delta; i++ )
      {
        if( mline_output[i].bias_out > vd_max )
        {         
          max_mline_index = i ;
          break;
        }
      }
      break;
  case DATA_MLINE_CALC_CAP_BIAS_TO_VdMAX:
      interp_bias = FALSE;
      for( i = 0; i < num_rgi_delta; i++ )
      {
        if( mline_output[i].bias_out > vd_max )
        {
          mline_output[i].bias_out = vd_max;         
        }
      }
      max_mline_index = num_rgi_delta-1;
      break;
  case DATA_MLINE_CLOSEST_PWR_RET_MAX_ENTRY:
      interp_bias = FALSE;
      for( i = 0; i < num_rgi_delta; i++ )
      {
        if( mline_output[i].bias_out > vd_max )
        {         
          max_mline_index = i ;
          break;
        }
      }
      break;
    default:
      FTM_MSG_1( FTM_ERROR, "MLine bias option is wrong %d",mline_bias_option);
      return FALSE;
      break;
  } /* Switch case Mline bias options */

  /* -----------------  START PICKING ROWS FROM MLINE FOR REQUIRED TARGET POWERS---------------*/

  if( debug_on )
  {
    MSG_SPRINTF_2(MSG_SSID_FTM, MSG_LEGACY_MED,"====== Selection for Swp2 %s %s Powers ======\n",
                  eptStr,measStr);
    FTM_MSG( FTM_MED, " PA   RGI   Vcc   Icq   IQ Gain  Env Scale  Delay  "
                       "ReqPower(dB100) MLinePower(dB100) ");
    
  }
  /* Store number of EPT selection sweep calculated powers in the M Line EPT result packet pointer */
  if ( mline_result_pkt != NULL  )
  {
    result_ptr->num_selec_swp_pwrs = num_target_pwrs;
  }

  for( i = 0; i < num_target_pwrs ; i++ )
  {
    curr_power = target_pwrs[i];

    /* ---------------  CHECKS ----------------*/
    /* If requested power greater than the Mline power, then fail */
    if( (curr_power >= (int16)(mline_output[max_mline_index].pow_out*100)) &&
        (mline_bias_option != DATA_MLINE_CLOSEST_PWR_RET_MAX_ENTRY) )
    {
      MSG_SPRINTF_5( MSG_SSID_FTM, FTM_ERROR, "Cannot find %s power greater than Requested "
                             "%s Power of %d. Max Power from MLine "
                             "Outputs for VdMax(%d) is %d", eptStr, measStr, curr_power, 
                             vd_max,mline_output[max_mline_index-1].pow_out*100);

      if ( mline_result_pkt != NULL )
      {
        mline_result_pkt->mline_debug_mask |= (1 << MLINE_DEBUG_MASK_EXPPWR_GT_MAXPWR_CALC );
        result_ptr->mline_selec_swp[i].selec_swp_valid = 0;
      }
      return_val &= FALSE;
    }
    /* Check for a Mline power smaller than the requested power. If not, then fail */
    else if( curr_power < (int16)(mline_output[0].pow_out*100) )
    {
      MSG_SPRINTF_4( MSG_SSID_FTM,FTM_ERROR, "Cannot find %s power less than Requested "
                            "%s Power of %d. Min Power from MLine "
                            "Outputs is %d", eptStr, measStr,curr_power, 
                            mline_output[0].pow_out*100);

      if ( mline_result_pkt != NULL )
      {
        mline_result_pkt->mline_debug_mask |= (1 << MLINE_DEBUG_MASK_EXPPWR_LT_MAXPWR_CALC );
        result_ptr->mline_selec_swp[i].selec_swp_valid = 0;
      }
      return FALSE;
    }
    /*-----------------LOOK FOR POWER IN MLINE------------------------*/
    else
    {
      for( lp_var = max_mline_index; lp_var >= 0; lp_var-- )
      {
        if( curr_power > (int16)(mline_output[lp_var].pow_out*100) )
        {

          switch ( mline_bias_option )
          {
          case DATA_MLINE_CALC_INTERP_BIAS:
            selected_ind = lp_var+1;
            break;
          case DATA_MLINE_CALC_CAP_BIAS_TO_VdMAX:
            selected_ind = lp_var+1;
            break;
          case DATA_MLINE_CALC_CLOSEST_POWER_INTERP_BIAS:
            /* IF curr pwr is max or if its closeest to next higher then pick next higher.*/
            if ( (curr_power == target_pwrs[num_target_pwrs-1]) ||
                 (curr_power >= 
                  (int16)((mline_output[lp_var].pow_out + mline_output[lp_var+1].pow_out)*50) )
               )
            {
              selected_ind = lp_var+1;
            }
            else
            {
              /* if picking the lower power, then dno't need to interpolate the bias.*/
              selected_ind = lp_var;
              interp_bias = FALSE;
            }
            break;
          case DATA_MLINE_CLOSEST_PWR_RET_MAX_ENTRY:
            /* IF curr_power is max target power then return max mline netry */
            if (curr_power == target_pwrs[num_target_pwrs-1])
            {
              selected_ind = max_mline_index -1;
          }
          else
          {
              /* IF curr pwr is closest to next higher then pick next higher.*/
              if (curr_power >= (int16)((mline_output[lp_var].pow_out + mline_output[lp_var+1].pow_out)*50) )
              {
            selected_ind = lp_var+1;
          }
              else
              {
                /* if picking the lower power, then dno't need to interpolate the bias.*/
                selected_ind = lp_var;
              }
              if ( mline_output[selected_ind].bias_out > vd_max )
              {
                selected_ind = max_mline_index -1;
              }
            }

            break;
          default:
            break;

          }
          /* Found index break out of loop*/
          break;
        }
      }/* for lp_var = max_mline_index ... LOOP TO FIND THE RIGHT POWER. */
      if ( selected_ind < 0 )
      {
        MSG_SPRINTF_4( MSG_SSID_FTM, FTM_ERROR, "Cannot find %s Mline entry for Requested "
                      "%s Power of %d. Min Power from MLine "
                      "Outputs is %d", eptStr, measStr,curr_power, 
                      mline_output[0].pow_out*100);
        return FALSE;
      }
      /* Form the DPD row to push back into Mline.*/
      mline_out_dpd_map_row[i].curr_tx_override.pa_state = pastate;
      mline_out_dpd_map_row[i].curr_tx_override.rgi = (int16)rgi_delta_data_ptr[selected_ind].rgi;
      mline_out_dpd_map_row[i].curr_tx_override.papm_bias = (int16)mline_output[selected_ind].bias_out;
          
      /* Store Selection sweep measured results in the M Line OUTPUT Result ptr*/
      if ( mline_result_pkt != NULL )
      {
        result_ptr->mline_selec_swp[i].selec_swp_rgi = rgi_delta_data_ptr[selected_ind].rgi;
        result_ptr->mline_selec_swp[i].selec_swp_bias = (uint32)mline_output[selected_ind].bias_out;
        result_ptr->mline_selec_swp[i].selec_swp_pwr = (int16)(mline_output[selected_ind].pow_out*100);
        result_ptr->mline_selec_swp[i].selec_swp_valid = 1;
      }
      
      /* Bias Limiting check */
      if( mline_out_dpd_map_row[i].curr_tx_override.papm_bias > vd_max )
      {
        /* If bias limiting option is to interpolate bordeline failures */
        if( interp_bias)
        {
          /* Interpolation code goes here. */
          interped_bias = mline_output[selected_ind-1].bias_out + 
                          ( ( mline_output[selected_ind].bias_out - mline_output[selected_ind-1].bias_out )*
                            ( curr_power -(int16)(mline_output[selected_ind-1].pow_out*100) )
                          )/
                          ((int16)(mline_output[selected_ind].pow_out*100) - (int16)(mline_output[selected_ind-1].pow_out*100));
        
          
          if (interped_bias < vd_max ) 
          {
            if( debug_on )
            {
              MSG_SPRINTF_5( MSG_SSID_FTM,FTM_HIGH,"Interpolated Bias for %s %s Target power %d dbmx100 is %d. Capping to %d. ",
                        eptStr,measStr,
                        curr_power, 
                        interped_bias,
                        vd_max);
            }
          
            mline_out_dpd_map_row[i].curr_tx_override.papm_bias = vd_max;
            if ( mline_result_pkt != NULL )
            {
              mline_result_pkt->mline_debug_mask |= (1 << MLINE_DEBUG_MASK_INTBIAS_LT_VMAX_MEAS );
              result_ptr->mline_selec_swp[i].selec_swp_bias = vd_max;
              result_ptr->mline_selec_swp[i].selec_swp_valid = 1;
            }
          }
          else
          {
             MSG_SPRINTF_5( MSG_SSID_FTM,FTM_ERROR,"Interpolated Bias for %s %s Target power %d dbmx100 is %d. Capping to %d. ",
                       eptStr,measStr,
                       curr_power, 
                       interped_bias,
                       vd_max);
             mline_out_dpd_map_row[i].curr_tx_override.papm_bias= vd_max;


             if ( mline_result_pkt != NULL )
             {
               result_ptr->mline_selec_swp[i].selec_swp_bias =vd_max;
               result_ptr->mline_selec_swp[i].selec_swp_valid = 0;
               mline_result_pkt->mline_debug_mask |= (1 << MLINE_DEBUG_MASK_INTBIAS_GT_VMAX_MEAS );
             }
             return FALSE;
          }
        }/* if interp_bias */
      } /* End of Bias Limiting check */

      mline_out_dpd_map_row[i].power = curr_power;
      mline_out_dpd_map_row[i].measured_entry = (is_meas)? TRUE: FALSE;
      mline_out_dpd_map_row[i].curr_tx_override.xpt_mode = (is_ept)? (int8)RFCOMMON_MDSP_XPT_MODE_EPT : (int8)RFCOMMON_MDSP_XPT_MODE_ET;

      /* Fill the rest of the DPD Map row params from what was 
      saved from Swp1 */
      mline_out_dpd_map_row[i].curr_tx_override.pa_curr = saved_dpd_map_row->curr_tx_override.pa_curr;
      mline_out_dpd_map_row[i].curr_tx_override.iq_gain = saved_dpd_map_row->curr_tx_override.iq_gain;
      mline_out_dpd_map_row[i].curr_tx_override.iq_gain_action_type = saved_dpd_map_row->curr_tx_override.iq_gain_action_type;
      mline_out_dpd_map_row[i].curr_tx_override.env_scale = saved_dpd_map_row->curr_tx_override.env_scale;
      mline_out_dpd_map_row[i].curr_tx_override.env_scale_action_type = saved_dpd_map_row->curr_tx_override.env_scale_action_type;
      mline_out_dpd_map_row[i].curr_tx_override.delay_action_type = saved_dpd_map_row->curr_tx_override.delay_action_type;
      mline_out_dpd_map_row[i].curr_tx_override.et_delay = saved_dpd_map_row->curr_tx_override.et_delay;

      if( debug_on )
      {
        FTM_MSG_9( FTM_MED, " %d   %d    %d   %d    %d        %d        %d          "
                          "%d            %d", 
                 mline_out_dpd_map_row[i].curr_tx_override.pa_state,
                 mline_out_dpd_map_row[i].curr_tx_override.rgi,
                 mline_out_dpd_map_row[i].curr_tx_override.papm_bias,
                 mline_out_dpd_map_row[i].curr_tx_override.pa_curr,
                 mline_out_dpd_map_row[i].curr_tx_override.iq_gain,
                 mline_out_dpd_map_row[i].curr_tx_override.env_scale,
                 mline_out_dpd_map_row[i].curr_tx_override.et_delay,
                 mline_out_dpd_map_row[i].power,
                 (int32)(mline_output[selected_ind].pow_out*100));
      }

      return_val &= ftm_cal_data_set_row_data( chan, &(mline_out_dpd_map_row[i]));
    } /* else case of CHECKS... i.e. valid case.  */
  } /* for loop for( i = 0; i < num_target_pwrs ; i++ )*/
  return return_val;
}/* ftm_calibration_mline_select_rows */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the Mline Proc Sub Opcode Instruction
 
  @details
  Perform the MLine calculations for the Data Opcode.
  - MLine calculation takes multiple AMAM curves and a RGI Delta table as
    inputs and based on the required compression point, draws the best fit
    line/curve across those points on each curve.
  - The AMAM curves are for different RGI and Bias combinations and the line
    drawn through a fixed compression point gives a relation between Vout and
    Vin as well as Vout and Vcc (Bias).
  - Using these relationships, we can compute the inputs required for any
    Pout.
 
  @param data_param
  Pointer to the data payload
 
  @param ftm_rf_tech
  Current FTM RF Technology
 
  @return
  Boolean to indicate pass/fail
*/
ftm_rsp_pkt_type 
ftm_calibration_data_process_mline_calc_instr
( 
  void *ftm_req_data
)
{
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_cal_sweep_data_mline_proc_rsp_pkt_type *rsp_pkt;
  ftm_rf_technology_type ftm_rf_tech;
  uint32 chan = 0;
  ftm_cal_data_proc_mline_req_type* req_pkt;
  ftm_cal_data_parameter_type *data_param;
  ftm_calibration_data_type *cal_data; /* Calibration Sweep Data */

  boolean return_val = TRUE;
  uint8 num_xpt_cal = 0; /* number of alignment array elements */
  uint8 num_rgi_delta = 0; /* number of RGI delta array elements */
  ftm_common_xpt_mline_params *mline_params = NULL; 
  ftm_common_xpt_cal_data *xpt_cal_data_ptr = NULL;
  ftm_common_rgi_delta_data *rgi_delta_data_ptr = NULL;
  ftm_common_xpt_mline_outputs *mline_outputs_ptr_ept = NULL;
  ftm_common_xpt_mline_outputs *mline_outputs_ptr_et = NULL;
  ftm_cal_data_dpd_map_row_type *dpd_map_row = NULL;
  fw_dpd_cal_result_t *dpd_result = NULL;
  ftm_cal_data_rgid_type *rgid_tbl = NULL;
  ftm_cal_data_dpd_map_row_type *sel_swp2_meas_pwr_data = NULL;
  ftm_cal_data_dpd_map_row_type *sel_swp2_calc_pwr_data = NULL;
  ftm_calibration_mline_result_xpt_type *mline_result_ept_ptr = NULL;
  ftm_calibration_mline_result_xpt_type *mline_result_et_ptr = NULL;
  
  uint8 lp_var = 0;

  uint8 swp1_pa = 0;
  uint8 swp1_rgi = 0;
  uint32 swp1_bias = 0;
  uint32 swp1_icq = 0;
  int16 swp1_pwr = 0;
  uint16 num_bytes = 0;
  uint16 i = 0;
  int16 swp1_pa_curr_save = 0;
  uint32 swp1_iq_gain_save = 0;
  uint8 swp1_iq_gain_action_save = 0;
  uint32 swp1_env_scale_save = 0;
  uint8 swp1_env_scale_action_save = 0;
  uint8 swp1_delay_action_save = 0;
  uint32 swp1_delay_save = 0;

  /* Fetch NV params */
  #ifdef FEATURE_WCDMA
  rfnv_wcdma_nv_tbl_type *nv_tbl = NULL;
  #endif
  #ifdef FEATURE_LTE
  //rflte_ftm_mc_sm_type *ftm_mc_sm_ptr = NULL;
  // rflte_nv_supported_band_type rfnv_band = RFLTE_NV_INVALID_BAND;
  rflte_nv_tbl_type* rflte_nv_tbl_ptr = NULL;
  uint32 current_carrier_idx = 0;
  #endif
  #ifdef FEATURE_TDSCDMA
  rfnv_tdscdma_tx_tbl_type* tds_nv_tbl_ptr = NULL;
  #endif

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( ftm_req_data,
                                 sizeof(ftm_cal_sweep_data_mline_proc_rsp_pkt_type) ); 

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  rsp_pkt = (ftm_cal_sweep_data_mline_proc_rsp_pkt_type*)ftm_rsp_data.pkt_payload;

  /* Get Calibration Data */
  cal_data = ftm_common_data_get_cal_v3_ptr();

  if (rsp_pkt == NULL  )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_LEN;
    if( cal_data->cal_config.debug_mode_en )
    {
     FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_data_process_mline_calc_instr : Failed "
                         "during Payload malloc" );
    }
    return ftm_rsp_data;
  } /* if (rsp_pkt == NULL  ) */

  req_pkt = (ftm_cal_data_proc_mline_req_type*)ftm_req_data;
  ftm_rf_tech = req_pkt->ftm_rf_tech;
  if( cal_data->cal_config.debug_mode_en )
  {
   FTM_MSG_1( FTM_MED, "FTM RF TECH for MLine is %d", ftm_rf_tech);
  }
  chan = req_pkt->channel;
  if( cal_data->cal_config.debug_mode_en )
  {
   FTM_MSG_1( FTM_MED, "Channel for MLine is %d", chan);
  }
  data_param = (ftm_cal_data_parameter_type*)(req_pkt->data_param);

  /* Allocate required memory and check if memory is allocated */
  mline_params = (ftm_common_xpt_mline_params*)modem_mem_alloc(
                  sizeof(ftm_common_xpt_mline_params), MODEM_MEM_CLIENT_RFA);

  if( mline_params == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for MLine Params!");
    return_val &= FALSE;
  }

  num_xpt_cal = data_param->data_param.mline_params.num_swp1_meas_pwrs;
  num_bytes = sizeof(ftm_common_xpt_cal_data) * num_xpt_cal;
  /* Allocated space if num_bytes not zero */
  if (num_bytes) {
  xpt_cal_data_ptr = modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_RFA);
  }

  if( xpt_cal_data_ptr == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for XPT Cal Data Ptr!");
    return_val &= FALSE;
  }

  dpd_map_row = (ftm_cal_data_dpd_map_row_type*)modem_mem_alloc(
                sizeof(ftm_cal_data_dpd_map_row_type), MODEM_MEM_CLIENT_RFA);

  if( dpd_map_row == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for DPD Map Row!");
    return_val &= FALSE;
  }

  rgid_tbl = (ftm_cal_data_rgid_type *)modem_mem_alloc(
             sizeof(ftm_cal_data_rgid_type), MODEM_MEM_CLIENT_RFA);

  if( rgid_tbl == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for RGI Delta Table in MLine!");
    return_val &= FALSE;
  }

  mline_result_ept_ptr = (ftm_calibration_mline_result_xpt_type*)modem_mem_alloc(
                          sizeof(ftm_calibration_mline_result_xpt_type),MODEM_MEM_CLIENT_RFA );

  if ( mline_result_ept_ptr == NULL )
  {
    FTM_MSG(FTM_ERROR,"Failed to allocate memory for EPT MLine results table!");
    return_val &= FALSE;
  }
  else
  {
    /* Init the M Line debug mask and Number of selection sweep powers for Meas Pwr and Calc Pwr List */
    mline_result_ept_ptr->mline_debug_mask = 0;
    mline_result_ept_ptr->selec_swp_meas_result.num_selec_swp_pwrs = 0;
    mline_result_ept_ptr->selec_swp_calc_result.num_selec_swp_pwrs = 0;
  }

  mline_result_et_ptr = (ftm_calibration_mline_result_xpt_type*)modem_mem_alloc(
                          sizeof(ftm_calibration_mline_result_xpt_type),MODEM_MEM_CLIENT_RFA );

  if ( mline_result_et_ptr == NULL )
  {
    FTM_MSG(FTM_ERROR,"Failed to allocate memory for ET MLine results table!");
    return_val &= FALSE;
  }
  else
  {
    /* Init the M Line debug mask and Number of selection sweep powers for Meas Pwr and Calc Pwr List*/
    mline_result_et_ptr->mline_debug_mask = 0;
    mline_result_et_ptr->selec_swp_meas_result.num_selec_swp_pwrs = 0;
    mline_result_et_ptr->selec_swp_calc_result.num_selec_swp_pwrs = 0;
  }

  num_rgi_delta = RFCOM_TXLIN_NUM_SEGMENTS;
  num_bytes = sizeof(ftm_common_rgi_delta_data) * num_rgi_delta;
  rgi_delta_data_ptr = modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_RFA);
  if( rgi_delta_data_ptr == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for RGI Delta Data Ptr in MLine!");
    return_val &= FALSE;
  }

    /* Allocate for sel_swp2_meas_pwr_data */
  if( data_param->data_param.mline_params.num_swp2_ept_meas_pwrs > 
      data_param->data_param.mline_params.num_swp2_et_meas_pwrs )
  {
    num_bytes = data_param->data_param.mline_params.num_swp2_ept_meas_pwrs * 
                sizeof(ftm_cal_data_dpd_map_row_type);
  }
  else
  {
    num_bytes = data_param->data_param.mline_params.num_swp2_et_meas_pwrs * 
              sizeof(ftm_cal_data_dpd_map_row_type);
  }
 
  if (num_bytes) {
  sel_swp2_meas_pwr_data = modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_RFA);
  }
  if( sel_swp2_meas_pwr_data == NULL )
  {

    FTM_MSG_2( FTM_ERROR, "Failed to allocate memory for Swp2 Meas Row Data! EPT_Meas_Powers = %d, #ET means pwrs = %d",
               data_param->data_param.mline_params.num_swp2_ept_meas_pwrs,
               data_param->data_param.mline_params.num_swp2_et_meas_pwrs);
    return_val &= FALSE;
  }

  /* Allocate for sel_swp2_calc_pwr_data */
  if( data_param->data_param.mline_params.num_swp2_ept_calc_pwrs > 
      data_param->data_param.mline_params.num_swp2_et_calc_pwrs )
  {
    num_bytes = data_param->data_param.mline_params.num_swp2_ept_calc_pwrs * 
                sizeof(ftm_cal_data_dpd_map_row_type);
  }
  else
  {
    num_bytes = data_param->data_param.mline_params.num_swp2_et_calc_pwrs * 
              sizeof(ftm_cal_data_dpd_map_row_type);
  }
  if (num_bytes) {
  sel_swp2_calc_pwr_data = modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_RFA);
  }
  if( sel_swp2_calc_pwr_data == NULL )
  {
    if( cal_data->cal_config.debug_mode_en )
    {
     FTM_MSG_2( FTM_HIGH, "NO CALC POWERS:- Num SWP2 EPT calc powers %d, Num SWP2 Et calc power %d",
                data_param->data_param.mline_params.num_swp2_ept_calc_pwrs,
                data_param->data_param.mline_params.num_swp2_et_calc_pwrs);
    }
  }

  if(  cal_data->cal_config.debug_mode_en )
  {
   ftm_cal_data_print_dpd_map();
  } 

  if( return_val == TRUE )
  {
    /* First populate the MLine Params */
    mline_params->lin_reg_percent = (float)(data_param->data_param.mline_params.lin_reg_percent_x100) / 100.0;
    mline_params->curve_fit_type = (ftm_common_mline_curve_fit_type)data_param->data_param.mline_params.curve_fit_type;

    if(  cal_data->cal_config.debug_mode_en ) 
    {
      FTM_MSG( FTM_MED, "================ MLINE PARAMS ===============");
      FTM_MSG_1(FTM_MED, "MLine Params: lin_reg_percent x 100 = %d", mline_params->lin_reg_percent*100);
      FTM_MSG_1(FTM_MED, "MLine Params: curve_fit_type = %d", mline_params->curve_fit_type);
    }
    
    if(  cal_data->cal_config.debug_mode_en) 
    {
      FTM_MSG( FTM_MED, "============== 1st SWP XPT DATA =============");
      FTM_MSG_1(FTM_MED, "MLine Params: num_xpt_cal = %d", num_xpt_cal);
    }

    /* Now populate the MLine Inputs for 1st sweep XPT data */
    for( lp_var = 0; lp_var < num_xpt_cal; lp_var++ )
    {
      swp1_pa = 
        data_param->data_param.mline_params.swp1_meas_data[lp_var].swp1_pa;
      swp1_rgi = 
        data_param->data_param.mline_params.swp1_meas_data[lp_var].swp1_rgi;
      swp1_bias = 
        data_param->data_param.mline_params.swp1_meas_data[lp_var].swp1_bias;
      swp1_pwr = 
        data_param->data_param.mline_params.swp1_meas_data[lp_var].swp1_pwr_db100;
    
      return_val &= ftm_cal_data_get_row_data( chan, dpd_map_row, (int8)swp1_pa, 
                                             (int16)swp1_rgi, (int16)swp1_bias );
    
      /* Save first sweep data for filling up Data Map when MLine Processing 
      is done */
      swp1_pa_curr_save = dpd_map_row->curr_tx_override.pa_curr;
      swp1_iq_gain_save = dpd_map_row->curr_tx_override.iq_gain;
      swp1_iq_gain_action_save = dpd_map_row->curr_tx_override.iq_gain_action_type;
      swp1_env_scale_save = dpd_map_row->curr_tx_override.env_scale;
      swp1_env_scale_action_save = dpd_map_row->curr_tx_override.env_scale_action_type;
      swp1_delay_action_save = dpd_map_row->curr_tx_override.delay_action_type;
      swp1_delay_save = dpd_map_row->curr_tx_override.et_delay;
    
      if( cal_data->cal_config.debug_mode_en )
      {
       FTM_MSG_7( FTM_MED, "Save Icq = %d, Save IQGain = %d, Save IQGain_Action = %d,"
                           "Save EnvSc = %d, Save EnvSc_Action = %d, "
                           "Save Delay action = %d, Save Delay = %d ", 
                  swp1_pa_curr_save, 
                  swp1_iq_gain_save, 
                  swp1_iq_gain_action_save,
                  swp1_env_scale_save, 
                  swp1_env_scale_action_save,
                  swp1_delay_action_save,
                  swp1_delay_save );
      }  
      
      if( return_val == FALSE )
      {
        FTM_MSG_5( FTM_ERROR, "Failed to Get Row data from Data Module for "
                              "Chan = %d, PA = %d, RGI = %d, Bias = %d, Icq = %d", 
                   chan, swp1_pa, swp1_rgi, swp1_bias, swp1_icq);
        return_val &= FALSE;
      }
    
      if( return_val == TRUE )
      {
        xpt_cal_data_ptr[lp_var].xpt_cal_inputs.bias = swp1_bias;
        //FTM_MSG_1(FTM_LOW, "MLine Inputs: Bias = %d",
        //          xpt_cal_data_ptr[lp_var].xpt_cal_inputs.bias);
    
        xpt_cal_data_ptr[lp_var].xpt_cal_inputs.rgi = swp1_rgi;
        //FTM_MSG_1(FTM_LOW, "MLine Inputs: RGI = %d",
        //          xpt_cal_data_ptr[lp_var].xpt_cal_inputs.rgi);
    
        xpt_cal_data_ptr[lp_var].xpt_cal_outputs.pout_meas = (float)(swp1_pwr / 10.0);
        //FTM_MSG_1(FTM_LOW, "MLine Inputs: Meas Power = %d",
        //          xpt_cal_data_ptr[lp_var].xpt_cal_outputs.pout_meas * 10);
    
        /* AMAM DAC */
        for (i = 0; i < XPT_CAL_MAX_AMAM_SIZE; i++)
        {
          xpt_cal_data_ptr[lp_var].xpt_cal_amams.amam_dac[i] = i+1;
          //if( data_param->debug_mode_en )
          //{
          //  FTM_MSG_2(FTM_LOW, "MLine Inputs: amam_dac[%d] = %d",
          //          i, xpt_cal_data_ptr[lp_var].xpt_cal_amams.amam_dac[i]);
          //}
        }
    
        /* AMAM Val */
        dpd_result = (fw_dpd_cal_result_t*)(dpd_map_row->dpd);
        //FTM_MSG_1(FTM_MED, "DPD Map Row: PA State = %d",
        //          dpd_map_row->rf_orides.pa_state);
        //FTM_MSG_1(FTM_MED, "DPD Map Row: RGI = %d",
        //          dpd_map_row->rf_orides.rgi);
        //FTM_MSG_1(FTM_MED, "DPD Map Row: Bias = %d",
        //          dpd_map_row->rf_orides.smps_bias);
        //FTM_MSG_1(FTM_MED, "DPD Map Row: Cap Buff Index = %d",
        //          dpd_map_row->cap_buf_idx);
        //FTM_MSG_1(FTM_MED, "DPD Map Row: Res Buff Index = %d",
        //          dpd_map_row->res_buf_idx);
        //FTM_MSG_1(FTM_MED, "DPD Map Row: RMS = %d",
        //          dpd_result->eq_rms_mag);
    
        memscpy(&(xpt_cal_data_ptr[lp_var].xpt_cal_amams.amam_val[0]), 
               (sizeof(uint16)*XPT_CAL_MAX_AMAM_SIZE),
               dpd_result->am_dist, (sizeof(uint16)*XPT_CAL_MAX_AMAM_SIZE));
    
        //for (i = 0; i < XPT_CAL_MAX_AMAM_SIZE; i++)
        //{
        //  FTM_MSG_2(FTM_LOW, "MLine Inputs: amam_val[%d] = %d",
        //          i, xpt_cal_data_ptr[lp_var].xpt_cal_amams.amam_val[i]);
        //}
    
        /* Rx Post Eq RMS */
        xpt_cal_data_ptr[lp_var].xpt_cal_outputs.rms_amam = (float)dpd_result->eq_rms_mag / 100.0;
        //FTM_MSG_1(FTM_LOW, "Mline Inputs: rms_amam = %d",
        //          xpt_cal_data_ptr[lp_var].xpt_cal_outputs.rms_amam * 100);
      }
    }

    /*  Now populate the MLine Inputs for 1st Swp RGI Delta data */
    /*  Reset RGI Delta Counter */
    num_rgi_delta = 0;

    if( data_param->data_param.mline_params.comp_dB_ept != COMP_POINT_INV && data_param->data_param.mline_params.comp_dB_et != COMP_POINT_INV)
    {
      return_val &= ftm_cal_data_get_rgid_table( chan, 0xFF, rgid_tbl );
    }
    else if( data_param->data_param.mline_params.comp_dB_ept != COMP_POINT_INV )
    {
      return_val &= ftm_cal_data_get_rgid_table( chan, data_param->data_param.mline_params.pa_ept, rgid_tbl );
    }
    else if( data_param->data_param.mline_params.comp_dB_et != COMP_POINT_INV )
    {
      return_val &= ftm_cal_data_get_rgid_table( chan, data_param->data_param.mline_params.pa_et, rgid_tbl );
    }
    
    for( i = 0; i < RFCOM_TXLIN_NUM_SEGMENTS; i++ )
    {
      if( rgid_tbl->table[i].rgi != RGID_INV_RGI )
      {
        rgi_delta_data_ptr[i].pmeas = (float)(rgid_tbl->table[i].power) / 10.0;
        rgi_delta_data_ptr[i].rgi = rgid_tbl->table[i].rgi;
        num_rgi_delta++;
        //FTM_MSG_2(FTM_MED, "MLine Inputs: RGI Delta Sweep[%d] rgi = %d", i, rgi_delta_data_ptr[i].rgi);
        //FTM_MSG_2(FTM_MED, "MLine Inputs: RGI Delta Sweep[%d] pmeas = %d", 
        //          i, (int16)(rgi_delta_data_ptr[i].pmeas * 10));
        //FTM_MSG_2(FTM_LOW, "MLine Inputs: RGI Delta Sweep[%d] NumDelta = %d", i, num_rgi_delta);
      }
    }
    if( cal_data->cal_config.debug_mode_en )
    {
     FTM_MSG_1(FTM_LOW, "MLine Inputs: Num RGI Delta data = %d", num_rgi_delta);
    }
  } /* if( return_val == TRUE ) */

  if( return_val == TRUE )
  {
    /* If MLine for EPT */
    if( data_param->data_param.mline_params.comp_dB_ept != COMP_POINT_INV )
    {
      mline_params->compression_db = (float)(data_param->data_param.mline_params.comp_dB_ept) / 100.0;
      if( cal_data->cal_config.debug_mode_en )
      {
       FTM_MSG_1(FTM_MED, "MLine Params: EPT compression_db x 100 = %d", mline_params->compression_db*100);
      }  

      /* Need to populate the RMS dBFs and Env Scale Unity value from NV for M Line calc */
      #ifdef FEATURE_WCDMA
      if (ftm_rf_tech == FTM_RF_TECH_WCDMA)
      {
        mline_params->rms_dbfs = (float)(rfwcdma_msm_get_rms_dbfs_x100_val())/100.0;

        nv_tbl = rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_convert_band_rfcom_to_rfnv(ftm_curr_wcdma_mode) );
        mline_params->env_scale = (int32)nv_tbl->tx_ept_dpd_config_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
      }
      #endif

      #ifdef FEATURE_CDMA1X
      if (ftm_rf_tech == FTM_RF_TECH_CDMA)
      {
        mline_params->rms_dbfs = (float)(rf_cdma_msm_get_rms_dbfs_x100_val())/100.0;

        /* This needs a device input!! */
        mline_params->env_scale = 
          (int32)(rf_cdma_xpt_get_cfg_param( (rfm_device_enum_type)data_param->device, EPT_CFG_CMN_EVN_SCALE_UNITY ));
      }
      #endif

      #ifdef FEATURE_LTE
      if (ftm_rf_tech == FTM_RF_TECH_LTE)
      {
        mline_params->rms_dbfs = (float)(rflte_msm_get_rms_dbfs_x100_val())/100.0;

        current_carrier_idx = rflte_ftm_mc_sm_get_carrier_index();
        //ftm_mc_sm_ptr = rflte_ftm_mc_get_sm();
        //rfnv_band = rflte_core_convert_rfcom_band_to_rfnv_band(ftm_mc_sm_ptr->carrier[ftm_mc_sm_ptr->current_carrier_idx].tx_band);
        rflte_nv_tbl_ptr = rflte_mc_get_nv_tbl_ptr(current_carrier_idx);
        mline_params->env_scale = (int32)rflte_nv_tbl_ptr->tx_xpt_dpd_config_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
      }
      #endif

      #ifdef FEATURE_TDSCDMA
      if (ftm_rf_tech == FTM_RF_TECH_TDSCDMA)
      {
        mline_params->rms_dbfs = (float)(rf_tdscdma_msm_get_rms_dbfs_x100_val())/100.0;

        tds_nv_tbl_ptr = rf_tdscdma_core_util_init_tx_nv_table(ftm_tdscdma_current_mode);
        mline_params->env_scale = (int32)tds_nv_tbl_ptr->ept_dpd_cfg_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
      }
      #endif

      if( cal_data->cal_config.debug_mode_en)
      {
        FTM_MSG_1(FTM_MED, "MLine Params: RMS dBFs x 10000 = %d", mline_params->rms_dbfs*10000);
        FTM_MSG_1(FTM_MED, "MLine Params: EPT Env Scale = %d", mline_params->env_scale);
      }

      num_bytes = num_rgi_delta * sizeof(ftm_common_xpt_mline_outputs);
      mline_outputs_ptr_ept = modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_RFA);
      if( mline_outputs_ptr_ept == NULL )
      {
        FTM_MSG( FTM_ERROR, "Failed to allocate memory for Mline Output in MLine!");
        return_val &= FALSE;
      }

      if( return_val == TRUE )
      {
        /* Call MLine function */
        return_val &= ftm_common_xpt_mline_calc( xpt_cal_data_ptr, 
                                                 num_xpt_cal, 
                                                 rgi_delta_data_ptr, 
                                                 num_rgi_delta, 
                                                 mline_params, 
                                                 mline_outputs_ptr_ept,
                                                 mline_result_ept_ptr,
                                                 cal_data->cal_config.debug_mode_en );

        if( cal_data->cal_config.debug_mode_en )
        {
          FTM_MSG( FTM_MED, "================= MLine EPT OUTPUTS =================");
          for( i = 0; i < num_rgi_delta; i++ )
          {
            FTM_MSG_3( FTM_MED, "MLine Outputs: RGI = %d, Bias = %d, Power = %d", 
                       rgi_delta_data_ptr[i].rgi, 
                       mline_outputs_ptr_ept[i].bias_out,
                       (int32)(mline_outputs_ptr_ept[i].pow_out * 100) );
          }
        }
      }/* if( return_val == TRUE ) */
    }
    /* If MLine for ET */
    if( data_param->data_param.mline_params.comp_dB_et != COMP_POINT_INV )
    {
      mline_params->compression_db = (float)(data_param->data_param.mline_params.comp_dB_et) / 100.0;

      if( cal_data->cal_config.debug_mode_en )
      {
       FTM_MSG_1(FTM_MED, "MLine Params: ET compression_db x 100 = %d", mline_params->compression_db*100);
      }

      /* Need to populate the RMS dBFs and Env Scale Unity value from NV for M Line calc */
      #ifdef FEATURE_WCDMA
      if (ftm_rf_tech == FTM_RF_TECH_WCDMA)
      {
        mline_params->rms_dbfs = (float)(rfwcdma_msm_get_rms_dbfs_x100_val())/100.0;

        nv_tbl = rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_convert_band_rfcom_to_rfnv(ftm_curr_wcdma_mode) );
        mline_params->env_scale = (int32)nv_tbl->tx_ept_dpd_config_params[ET_CFG_CMN_ENV_SCALE_UNITY];
      }
      #endif

      #ifdef FEATURE_CDMA1X
      if (ftm_rf_tech == FTM_RF_TECH_CDMA)
      {
        mline_params->rms_dbfs = (float)(rf_cdma_msm_get_rms_dbfs_x100_val())/100.0;

        /* This needs a device input!! */
        mline_params->env_scale = 
          (int32)(rf_cdma_xpt_get_cfg_param( (rfm_device_enum_type)data_param->device, ET_CFG_CMN_ENV_SCALE_UNITY ));
      }
      #endif

      #ifdef FEATURE_LTE
      if (ftm_rf_tech == FTM_RF_TECH_LTE)
      {
        mline_params->rms_dbfs = (float)(rflte_msm_get_rms_dbfs_x100_val())/100.0;
        current_carrier_idx = rflte_ftm_mc_sm_get_carrier_index();
        //ftm_mc_sm_ptr = rflte_ftm_mc_get_sm();
        //rfnv_band = rflte_core_convert_rfcom_band_to_rfnv_band(ftm_mc_sm_ptr->carrier[ftm_mc_sm_ptr->current_carrier_idx].tx_band);
        rflte_nv_tbl_ptr = rflte_mc_get_nv_tbl_ptr(current_carrier_idx);
        mline_params->env_scale = (int32)rflte_nv_tbl_ptr->tx_xpt_dpd_config_params[ET_CFG_CMN_ENV_SCALE_UNITY];
      }
      #endif

      #ifdef FEATURE_TDSCDMA
      if (ftm_rf_tech == FTM_RF_TECH_TDSCDMA)
      {
        mline_params->rms_dbfs = (float)(rf_tdscdma_msm_get_rms_dbfs_x100_val())/100.0;

        tds_nv_tbl_ptr = rf_tdscdma_core_util_init_tx_nv_table(ftm_tdscdma_current_mode);
        mline_params->env_scale = (int32)tds_nv_tbl_ptr->ept_dpd_cfg_params[ET_CFG_CMN_ENV_SCALE_UNITY];
      }
      #endif

      if( cal_data->cal_config.debug_mode_en )
      {
        FTM_MSG_1(FTM_MED, "MLine Params: RMS dBFs x 10000 = %d", mline_params->rms_dbfs*10000);
        FTM_MSG_1(FTM_MED, "MLine Params: ET Env Scale = %d", mline_params->env_scale);
      }

      num_bytes = num_rgi_delta * sizeof(ftm_common_xpt_mline_outputs);
      mline_outputs_ptr_et = modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_RFA);
      if( mline_outputs_ptr_et == NULL )
      {
        FTM_MSG( FTM_ERROR, "Failed to allocate memory for Mline Output in MLine!");
        return_val &= FALSE;
      }

      if( return_val == TRUE )
      {
        /* Call MLine function */
        return_val &= ftm_common_xpt_mline_calc( xpt_cal_data_ptr, 
                                                 num_xpt_cal, 
                                                 rgi_delta_data_ptr, 
                                                 num_rgi_delta, 
                                                 mline_params, 
                                                 mline_outputs_ptr_et,
                                                 mline_result_et_ptr,
                                                 cal_data->cal_config.debug_mode_en );

        if( cal_data->cal_config.debug_mode_en )
        {
          FTM_MSG( FTM_MED, "================= MLine ET OUTPUTS =================");
          for( i = 0; i < num_rgi_delta; i++ )
          {
            FTM_MSG_3( FTM_MED, "MLine Outputs: RGI = %d, Bias = %d, Power = %d", 
                       rgi_delta_data_ptr[i].rgi, 
                       mline_outputs_ptr_et[i].bias_out,
                       (int32)(mline_outputs_ptr_et[i].pow_out * 100) );
          }
        }
      }/* if( return_val == TRUE ) */
    }
  }
  else
  {
    FTM_MSG( FTM_ERROR, "Failed to Clear DPD Map!");
    return_val &= FALSE;
  }

  /* If Mline Calc was a success.. */
  if( return_val == TRUE )
  {
    /* If MLine for EPT was done */
    if( data_param->data_param.mline_params.comp_dB_ept != COMP_POINT_INV )
    {
      /* Pick Mline entries for EPT Meas target powers */
      return_val &= ftm_calibration_mline_select_rows( chan,
                                                       mline_outputs_ptr_ept,
                                                       mline_result_ept_ptr, 
                                                       sel_swp2_meas_pwr_data, 
                                                       data_param, 
                                                       num_rgi_delta,
                                                       rgi_delta_data_ptr,
                                                       dpd_map_row,
                                                       TRUE,
                                                       TRUE,
                                                       cal_data->cal_config.debug_mode_en);
      /* Pick Mline entries for EPT Calc target powers */
      return_val &= ftm_calibration_mline_select_rows( chan,
                                                       mline_outputs_ptr_ept,
                                                       mline_result_ept_ptr, 
                                                       sel_swp2_calc_pwr_data, 
                                                       data_param, 
                                                       num_rgi_delta,
                                                       rgi_delta_data_ptr,
                                                       dpd_map_row,
                                                       TRUE,
                                                       FALSE,
                                                       cal_data->cal_config.debug_mode_en);

      if ( mline_result_ept_ptr != NULL )
      {
        return_val &= ftm_cal_data_store_mline_res_ptr(chan,
                                                     XPT_TYPE_EPT,
                                                     mline_result_ept_ptr);

        if(cal_data->cal_config.debug_mode_en )
        {
          if ( return_val == TRUE && data_param->data_param.mline_params.num_swp2_ept_meas_pwrs )
          {
            FTM_MSG(FTM_LOW, "EPT results copied to data module");
          }
          else
          {
            FTM_MSG(FTM_LOW,"ftm_cal_data_store_mline_res_ptr() for EPT returned FALSE.");
          }
        }
      } /* if ( mline_result_ept_ptr != NULL ) */
    }
    /* If ET MLine was done */
    if( data_param->data_param.mline_params.comp_dB_et != COMP_POINT_INV )
    {
      /* Pick Mline entries for ET Meas target powers */
      return_val &= ftm_calibration_mline_select_rows( chan,
                                                       mline_outputs_ptr_et,
                                                       mline_result_et_ptr, 
                                                       sel_swp2_meas_pwr_data, 
                                                       data_param, 
                                                       num_rgi_delta,
                                                       rgi_delta_data_ptr,
                                                       dpd_map_row,
                                                       FALSE,
                                                       TRUE,
                                                       cal_data->cal_config.debug_mode_en);

      if ( mline_result_et_ptr != NULL )
      {
        return_val &= ftm_cal_data_store_mline_res_ptr(chan,
                                                     XPT_TYPE_ET,
                                                     mline_result_et_ptr);

        if( cal_data->cal_config.debug_mode_en )
        {
          if ( return_val == TRUE )
          {
            FTM_MSG(FTM_LOW, "ET results copied to data module");
          }
          else
          {
            FTM_MSG(FTM_LOW,"ftm_cal_data_store_mline_res_ptr() for ET returned FALSE.");
          }
        }
      } /* if ( mline_result_et_ptr != NULL ) */
    } /* data_param->data_param.mline_params.comp_dB_et != COMP_POINT_INV */
  }
  else
  {
    FTM_MSG( FTM_ERROR, "Failed MLine Calc!");
    return_val &= FALSE;
  }

  if( cal_data->cal_config.debug_mode_en )
  {
   /* Print DPD Map */
   ftm_cal_data_print_dpd_map();
  }
  /* Free allocated memory */
  if( mline_params != NULL )
  {
    modem_mem_free((void*)mline_params, MODEM_MEM_CLIENT_RFA);
  }
  if( xpt_cal_data_ptr != NULL )
  {
    modem_mem_free((void*)xpt_cal_data_ptr, MODEM_MEM_CLIENT_RFA);
  }
  if( rgi_delta_data_ptr != NULL )
  {
    modem_mem_free((void*)rgi_delta_data_ptr, MODEM_MEM_CLIENT_RFA);
  }
  if( rgid_tbl != NULL )
  {
    modem_mem_free((void*)rgid_tbl, MODEM_MEM_CLIENT_RFA);
  }
  if( mline_outputs_ptr_ept != NULL )
  {
    modem_mem_free((void*)mline_outputs_ptr_ept, MODEM_MEM_CLIENT_RFA);
  }
  if( mline_outputs_ptr_et != NULL )
  {
    modem_mem_free((void*)mline_outputs_ptr_et, MODEM_MEM_CLIENT_RFA);
  }
  if ( mline_result_ept_ptr != NULL )
  {
    modem_mem_free((void*)mline_result_ept_ptr, MODEM_MEM_CLIENT_RFA);
  }
  if ( mline_result_et_ptr != NULL )
  {
    modem_mem_free((void*)mline_result_et_ptr, MODEM_MEM_CLIENT_RFA);
  }
  if( dpd_map_row != NULL )
  {
    modem_mem_free((void*)dpd_map_row, MODEM_MEM_CLIENT_RFA);
  }
  if( sel_swp2_meas_pwr_data != NULL )
  {
    modem_mem_free((void*)sel_swp2_meas_pwr_data, MODEM_MEM_CLIENT_RFA);
  }
  if( sel_swp2_calc_pwr_data != NULL )
  {
    modem_mem_free((void*)sel_swp2_calc_pwr_data, MODEM_MEM_CLIENT_RFA);
  }

  rsp_pkt->mline_proc_status = return_val;

  /* Send back a BAD response packet for failures */
  if ( return_val == FALSE )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;

    /* Throw Debug messages */
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_calibration_data_process_mline_calc_instr : Failed " );
  }
  else
  {
    /* Throw Debug messages */
    if( cal_data->cal_config.debug_mode_en )
    {
     FTM_MSG( FTM_HIGH, "Cal v3 ftm_calibration_data_process_mline_calc_instr : [Done] ");
    }
  }

  /* Return */
  return ftm_rsp_data;
} /* ftm_calibration_data_process_mline_calc_instr */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform MLine Calculations
 
  @details
  This function will be called from cal v3 common framework to perform
  mline calculations.
 
  @param data_param
  Pointer to the data structure containing data unpacked parameter
 
  @return
  Flag indicating the status of mline calc operation
*/
boolean
ftm_calibration_data_process_mline
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  ftm_rf_technology_type ftm_rf_tech,
  uint32 chan,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  /* Config radio request */
  ftm_cal_data_proc_mline_req_type data_mline_req;

  uint8 *res_write_ptr; /* results write pointer */
  uint16 written_size = 0; /* Total written size */
  uint16 free_size = 0; /* Total free size */
  uint8 res_type = FTM_CAL_OPCODE_DATA;
  uint8 res_action_type = DATA_RES_ACTION_PROCESS_MLINE;
  uint8 version = 0;

  /* Parameter validation */
  if ( data_param == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_calibration_data_process_mline: NULL argument" );
    return FALSE;
  } /* if ( data_param == NULL ) */

  /* Populate the Request Packet to do mline calculations */
  data_mline_req.cmd = DIAG_SUBSYS_CMD_F;
  data_mline_req.sub_sys = FTM;
  data_mline_req.mode = FTM_COMMON_C;
  data_mline_req.cdma_cmd = FTM_INT_CALV3_DATA_PROCESS_MLINE;
  data_mline_req.ftm_rf_tech = ftm_rf_tech;
  data_mline_req.channel = chan;
  data_mline_req.data_param = (uint32)data_param;

  //{
  //  size_t data_length = sizeof(ftm_cal_data_parameter_type) ; 
  //  memscpy ( &(data_mline_req.data_param), data_length , 
  //            data_param , data_length );
  //}
  ftm_common_send_radio_command( &data_mline_req,
                                 sizeof(ftm_cal_data_proc_mline_req_type) );

  
  /* Now populate the result buffer from cal_result */
  /* Initialize the free size */ 
  free_size = oc_disp->res_buff_info.buff_sz_max;

  /*==========================================================================*/
  /* Populate the results buffer */
  /*==========================================================================*/

  /* Initialize the write pointer */
  res_write_ptr = oc_disp->res_buff_info.buff_ptr;

  /*==========================================================================*/
  /* Populate Results Type */
  /*==========================================================================*/

  if ( free_size >= sizeof(uint8) )
  {
    memscpy( res_write_ptr, sizeof(uint8),&(res_type), sizeof(uint8));
    UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
    free_size -= sizeof(uint8);
  } /* if ( free_size >= sizeof(uint8) ) */
  else
  {
    FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE.{0x%x}] "
               "ResultPush : ResType - ReqSize %d FreeSize %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_DATA,
               sizeof(uint8),
               free_size );
  } /* if ( free_size >= sizeof(uint8) ) */
  
  /*==========================================================================*/
  /* Populate version Type */
  /*==========================================================================*/

  if ( free_size >= sizeof(uint8) )
  {
    memscpy( res_write_ptr, sizeof(uint8), &(version), sizeof(uint8));
    UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
    free_size -= sizeof(uint8);
  } /* if ( free_size >= sizeof(uint8) ) */
  else
  {
    FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE.{0x%x}] "
               "ResultPush : Version - ReqSize %d FreeSize %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_DATA,
               sizeof(uint8),
               free_size );
  } /* if ! ( free_size >= sizeof(uint8) ) */
  
  /*==========================================================================*/
  /* Populate Device */
  /*==========================================================================*/

  if ( free_size >= sizeof(ftm_cal_data_device_type) )
  {
    memscpy( res_write_ptr, sizeof(ftm_cal_data_device_type),
             &(data_param->device), sizeof(ftm_cal_data_device_type));
	UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
	  				   sizeof(ftm_cal_data_device_type) );
	free_size -= sizeof(ftm_cal_data_device_type);
  } /* if ( free_size >= sizeof(ftm_cal_data_device_type) ) */
  else
  {
	FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA DPD CONSTRUCTION.{0x%x}] "
		       "ResultPush : dMask - ReqSize %d FreeSize %d",
	  		   cal_data->seg_data.cal_instance,
			   FTM_CAL_OPCODE_DATA,
			   sizeof(ftm_cal_data_device_type),
			   free_size );
  } /* if ! ( free_size >= sizeof(ftm_cal_data_device_type) ) */

  /*==========================================================================*/
  /* Populate Results Action */
  /*==========================================================================*/

  if ( free_size >= sizeof(uint8) )
  {
    memscpy( res_write_ptr, sizeof(uint8), &(res_action_type), sizeof(uint8));
    UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
    free_size -= sizeof(uint8);
  } /* if ( free_size >= sizeof(uint8) ) */
  else
  {
    FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE.{0x%x}] "
               "ResultPush : ResType - ReqSize %d FreeSize %d",
               cal_data->seg_data.cal_instance,
               DATA_RES_ACTION_PROCESS_MLINE,
               sizeof(uint8),
               free_size );
  } /* if ( free_size >= sizeof(uint8) ) */

  /*==========================================================================*/
  /* Update Result Written Size */
  /*==========================================================================*/
  oc_disp->res_buff_info.buff_sz = written_size;

  if ( cal_data->cal_config.debug_mode_en )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
               "ResultPush : %d bytes",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_DATA,
               written_size );
  }

  return TRUE;

} /* ftm_calibration_data_process_mline */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process FW processed ET path delay data to send to PC and overwrite NV table
 
  @details
  This function will be called from cal v3 common framework to perform
  ET path delay computation and to overwrite Static NV delay value
 
  @param data_param
  Pointer to the data structure containing data unpacked parameter
 
  @return
  Flag indicating the status of delay data processing operation
*/
boolean
ftm_calibration_data_process_delay
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  ftm_rf_technology_type ftm_rf_tech,
  uint32 chan,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean return_val = TRUE;
  uint8 *res_write_ptr; /* results write pointer */
  uint16 written_size = 0; /* Total written size */
  uint16 free_size = 0; /* Total free size */
  uint8 res_type = FTM_CAL_OPCODE_DATA;
  uint8 res_action_type = DATA_RES_ACTION_PROCESS_DELAY;
  uint8 version = 0;
  #ifdef FEATURE_WCDMA
  rfnv_wcdma_nv_tbl_type *nv_tbl = NULL; // must do null checks
  #endif
  #ifdef FEATURE_LTE
  uint16 i = 0;
  ftm_cal_data_delay_cap_type delay_capt_type = RF_COMMON_XPT_DELAY_CAP_NONLTE;
  rflte_ftm_mc_sm_type *ftm_mc_sm_ptr = NULL; // must do null checks
  rflte_nv_supported_band_type rfnv_band = RFLTE_NV_INVALID_BAND;
  rflte_nv_tbl_type* rflte_nv_tbl_ptr = NULL; // must do null checks
  #endif
  #ifdef FEATURE_TDSCDMA
  //rfnv_tdscdma_tx_tbl_type* tds_nv_tbl_ptr = NULL; // must do null checks
  #endif
  #ifdef FEATURE_CDMA1X
  const rfc_cdma_logical_device_params_type *cdma_logical_device_ptr = NULL;
  #endif
  int16 delay_comp_x10ns = 0;
  boolean delay_comp_done = FALSE;

  ftm_calibration_delay_results_type *cal_result = NULL; // must do null checks

  /* Param vaildation */
  if (  ( data_param == NULL )  || ( oc_disp == NULL )  )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][TX_MEASURE.{0x%x}] "
               "ResultPush : Null Argument",
               FTM_CAL_OPCODE_DATA );

    return FALSE;
  } /* if  (  ( data_param == NULL )  || ( oc_disp == NULL ) )  */

  /* Allocate required memory and check if memory is allocated */
  cal_result = (ftm_calibration_delay_results_type*)modem_mem_alloc(
               sizeof(ftm_calibration_delay_results_type), MODEM_MEM_CLIENT_RFA);

  if( cal_result == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for ET Path Delay Results!");
    return_val &= FALSE;
  }
  else
  {
    memset(cal_result, 0, sizeof(ftm_calibration_delay_results_type));
  }
	
  /* Print DPD Map */
  if (cal_data->cal_config.debug_mode_en)
  {
    ftm_cal_data_print_dpd_map();
    ftm_cal_data_print_delay_table();
  }

  if( return_val == TRUE )
  {
    cal_result->device = data_param->device;
    cal_result->et_path_delay_results.channel = chan;
    #ifdef FEATURE_WCDMA
    if (ftm_rf_tech == FTM_RF_TECH_WCDMA)
    {
      cal_result->et_path_delay_results.bandwidth = FTM_CAL_V3_INVALID_LTE_BW;
      cal_result->et_path_delay_results.delay_p1ns = ftm_cal_data_get_delay_from_table(chan,
                                                                     RF_COMMON_XPT_DELAY_CAP_NONLTE,
                                                                     cal_data->cal_config.debug_mode_en);
      delay_comp_done = FALSE;
      delay_comp_done = rfdevice_wcdma_tx_get_et_delay_compensation(RFCOM_TRANSCEIVER_0,
                                                  ftm_curr_wcdma_mode,
                                                  &delay_comp_x10ns);
      if (delay_comp_done) 
      {
        if (cal_data->cal_config.debug_mode_en)
        {
          FTM_MSG_3( FTM_HIGH, "FW measured Delay %d, "
               "Delay comp %d , NOMINAL delay %d",
               cal_result->et_path_delay_results.delay_p1ns,
               delay_comp_x10ns,
               cal_result->et_path_delay_results.delay_p1ns-delay_comp_x10ns );
        }
        cal_result->et_path_delay_results.delay_p1ns -= delay_comp_x10ns;
      }
      else
      {
        FTM_MSG( FTM_ERROR, "Could not get ET delay compensation for RC process variation. "
           "API rfdevice_wcdma_tx_get_et_delay_compensation failed" );
      }
      nv_tbl = rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_convert_band_rfcom_to_rfnv(ftm_curr_wcdma_mode) );
      if (nv_tbl == NULL)
      {
        FTM_MSG(FTM_ERROR,"ftm_calibration_data_process_delay: Could not get WCDMA NV table pointer");
        return_val &= FALSE;
      }
      else /* overwrite NV table value with computed valued */
      {
        nv_tbl->et_delay_value = cal_result->et_path_delay_results.delay_p1ns;
        nv_tbl->xpt_path_delay.num_of_active_entries = 1;
        nv_tbl->xpt_path_delay.delay_value[0] = cal_result->et_path_delay_results.delay_p1ns;
        nv_tbl->xpt_path_delay.upper_bound_tx_chan[0] = (uint16) (data_param->data_param.et_delay_params.upper_bound_tx_earfcn);
      }
    }
    #endif
    #ifdef FEATURE_CDMA1X
    if (ftm_rf_tech == FTM_RF_TECH_CDMA)
    {
      rfm_device_enum_type dev = (rfm_device_enum_type)data_param->device ;
      rfm_cdma_band_class_type band = rf_cdma_data_get_curr_band ( dev ) ;
      cal_result->et_path_delay_results.bandwidth = FTM_CAL_V3_INVALID_LTE_BW;
      cal_result->et_path_delay_results.delay_p1ns = ftm_cal_data_get_delay_from_table(chan,
                                                                     RF_COMMON_XPT_DELAY_CAP_NONLTE,
                                                                     cal_data->cal_config.debug_mode_en);
      delay_comp_done = FALSE;
      cdma_logical_device_ptr = rfc_cdma_get_logical_device_params ( dev );
      if ( (cdma_logical_device_ptr == NULL) ||
         (cdma_logical_device_ptr->tx_device[band] == NULL) )
      {
        FTM_MSG_1( FTM_ERROR, "Could not get CDMA logical device. ", dev ) ;
       delay_comp_done = FALSE;
      }
      else
      {
        delay_comp_done = FALSE;
        delay_comp_done = 
          rfdevice_cdma_tx_get_et_delay_compensation ( cdma_logical_device_ptr->tx_device[band],
                                                                  &delay_comp_x10ns );
        if (delay_comp_done)
        {
          if (cal_data->cal_config.debug_mode_en)
          {
            FTM_MSG_3( FTM_HIGH, "FW measured Delay %d, "
               "Delay comp %d , NOMINAL delay %d",
               cal_result->et_path_delay_results.delay_p1ns,
               delay_comp_x10ns,
               cal_result->et_path_delay_results.delay_p1ns-delay_comp_x10ns );
          }
          cal_result->et_path_delay_results.delay_p1ns -= delay_comp_x10ns;
        }
        else
        {
          FTM_MSG( FTM_ERROR, "Could not get ET delay compensation for RC process variation. "
             "API rfdevice_cdma_tx_get_et_delay_compensation failed" );
        }
      }
      /* This needs a device input!! */
      rf_cdma_xpt_override_et_delay_value((rfm_device_enum_type)data_param->device,
                                          cal_result->et_path_delay_results.delay_p1ns,
                                          (uint32)data_param->data_param.et_delay_params.upper_bound_tx_earfcn);
    }
    #endif
    #ifdef FEATURE_LTE
    if (ftm_rf_tech == FTM_RF_TECH_LTE)
    {
      ftm_mc_sm_ptr = rflte_ftm_mc_get_sm();
      if (ftm_mc_sm_ptr == NULL)
      {
        FTM_MSG(FTM_ERROR,"ftm_calibration_data_process_delay: Could not get LTE SM pointer");
        return_val &= FALSE;
      }
      else
      { 
        rfnv_band = rflte_core_convert_rfcom_band_to_rfnv_band(ftm_mc_sm_ptr->carrier[ftm_mc_sm_ptr->current_carrier_idx].tx_band);
        rflte_nv_tbl_ptr = rflte_nv_get_tbl_ptr(rfnv_band);
        if (rflte_nv_tbl_ptr != NULL)
        {
          cal_result->et_path_delay_results.bandwidth = ftm_mc_sm_ptr->carrier[ftm_mc_sm_ptr->current_carrier_idx].tx_bw;

          switch ((rfcom_lte_bw_type)cal_result->et_path_delay_results.bandwidth)
          {
          case RFCOM_BW_LTE_5MHz:
            delay_capt_type = RF_COMMON_XPT_DELAY_CAP_LTE_BW_5;
            break;
          case RFCOM_BW_LTE_10MHz:
            delay_capt_type = RF_COMMON_XPT_DELAY_CAP_LTE_BW_10;
            break;
          case RFCOM_BW_LTE_20MHz:
            delay_capt_type = RF_COMMON_XPT_DELAY_CAP_LTE_BW_20;
            break;
          default:
            delay_capt_type = RF_COMMON_XPT_DELAY_CAP_LTE_BW_10;
          }
          cal_result->et_path_delay_results.delay_p1ns = ftm_cal_data_get_delay_from_table(chan,
                                                                         delay_capt_type,
                                                                         cal_data->cal_config.debug_mode_en);
  
          delay_comp_done = FALSE;
          delay_comp_done = rfdevice_lte_tx_get_et_delay_compensation( RFCOM_TRANSCEIVER_0,
                                             ftm_mc_sm_ptr->carrier[ftm_mc_sm_ptr->current_carrier_idx].tx_band,
                                             cal_result->et_path_delay_results.bandwidth,
                                             &delay_comp_x10ns );
          if (delay_comp_done) 
          {
           if (cal_data->cal_config.debug_mode_en)
	   {
            FTM_MSG_3( FTM_HIGH, "FW measured Delay %d, "
               "Delay comp %d , NOMINAL delay %d",
               cal_result->et_path_delay_results.delay_p1ns,
               delay_comp_x10ns,
               cal_result->et_path_delay_results.delay_p1ns-delay_comp_x10ns );
	   }
             cal_result->et_path_delay_results.delay_p1ns -= delay_comp_x10ns;
          }
          else
          {
            FTM_MSG( FTM_ERROR, "Could not get ET delay compensation for RC process variation. "
               "API rfdevice_lte_tx_get_et_delay_compensation failed" );
          }
       
          rflte_nv_tbl_ptr->xpt_path_delay.num_of_active_entries = 1; 
          for (i=0; i<RFLTE_XPT_PATH_DELAY_ARRAY_SIZ; i++)
          {
            rflte_nv_tbl_ptr->xpt_path_delay.upper_bound_tx_earfcn[i] = 0;
            switch ((rfcom_lte_bw_type)cal_result->et_path_delay_results.bandwidth)
            {
            case RFCOM_BW_LTE_5MHz:
              rflte_nv_tbl_ptr->xpt_path_delay.upper_bound_tx_earfcn[i] = (uint16) (data_param->data_param.et_delay_params.upper_bound_tx_earfcn);
              rflte_nv_tbl_ptr->xpt_path_delay.delay_value_bw5[i] = cal_result->et_path_delay_results.delay_p1ns;
              break;
            case RFCOM_BW_LTE_10MHz:
              rflte_nv_tbl_ptr->xpt_path_delay.upper_bound_tx_earfcn[i] = (uint16) (data_param->data_param.et_delay_params.upper_bound_tx_earfcn);
              rflte_nv_tbl_ptr->xpt_path_delay.delay_value_bw10[i] = cal_result->et_path_delay_results.delay_p1ns;
              break;
            case RFCOM_BW_LTE_20MHz:
              rflte_nv_tbl_ptr->xpt_path_delay.upper_bound_tx_earfcn[i] = (uint16) (data_param->data_param.et_delay_params.upper_bound_tx_earfcn);
              rflte_nv_tbl_ptr->xpt_path_delay.delay_value_bw20[i] = cal_result->et_path_delay_results.delay_p1ns;
              break;
            default:
              rflte_nv_tbl_ptr->xpt_path_delay.upper_bound_tx_earfcn[i] = (uint16) (data_param->data_param.et_delay_params.upper_bound_tx_earfcn);
              rflte_nv_tbl_ptr->xpt_path_delay.delay_value_bw10[i] = cal_result->et_path_delay_results.delay_p1ns;
            }
          }
        }
        else
        {
          return_val &= FALSE;
          FTM_MSG(FTM_ERROR,"ftm_calibration_data_process_delay: Couldn't get LTE NV Ptr");
        }
      } /* else case, i.e. ftm_mc_sm_ptr != NULL)*/
    } /*(ftm_rf_tech == FTM_RF_TECH_LTE)*/
    #endif
    #ifdef FEATURE_TDSCDMA
    if (ftm_rf_tech == FTM_RF_TECH_TDSCDMA)
    {
      //tds_nv_tbl_ptr = rf_tdscdma_core_util_init_tx_nv_table(ftm_tdscdma_current_mode);
      /* mline_params->env_scale = (int32)tds_nv_tbl_ptr->ept_dpd_cfg_params[EPT_CFG_CMN_EVN_SCALE_UNITY]; */
      // Is ET Path delay defined in TDS NV table??
    }
    #endif
  }
  
  //if( return_val == TRUE )
  //{
    /* Now populate the result buffer from cal_result */
    /* Initialize the free size */ 
    free_size = oc_disp->res_buff_info.buff_sz_max;

    /*==========================================================================*/
    /* Populate the results buffer */
    /*==========================================================================*/

    /* Initialize the write pointer */
    res_write_ptr = oc_disp->res_buff_info.buff_ptr;

    /*==========================================================================*/
    /* Populate Results Type */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS DELAY.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate version Type */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(version), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS DELAY.{0x%x}] "
                 "ResultPush : Version - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ! ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate Device */
    /*==========================================================================*/

    if ( free_size >= sizeof(ftm_cal_data_device_type) )
    {
      memscpy( res_write_ptr, sizeof(ftm_cal_data_device_type), 
               &(cal_result->device), sizeof(ftm_cal_data_device_type));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                         sizeof(ftm_cal_data_device_type) );
      free_size -= sizeof(ftm_cal_data_device_type);
    } /* if ( free_size >= sizeof(ftm_cal_data_device_type) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS DELAY.{0x%x}] "
                 "ResultPush : dMask - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(ftm_cal_data_device_type),
                 free_size );
    } /* if ! ( free_size >= sizeof(ftm_cal_data_device_type) ) */
	
	/*==========================================================================*/
    /* Populate Results Action */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_action_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS DELAY.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_DELAY,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate Process Delay Results - Channel */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint32) )
    {
      memscpy( res_write_ptr, sizeof(uint32),
               &(cal_result->et_path_delay_results.channel), sizeof(uint32));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                         sizeof(uint32) );
      free_size -= sizeof(uint32);
    } /* if ( free_size >= sizeof(uint32) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS DELAY.{0x%x}] "
                 "ResultPush : dMask - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint32),
                 free_size );
    } /* if ! ( free_size >= sizeof(uint32) ) */

    /*==========================================================================*/
    /* Populate Process Delay Results - Bandwidth */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint32) )
    {
      memscpy( res_write_ptr, sizeof(uint32),
               &(cal_result->et_path_delay_results.bandwidth), sizeof(uint32));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                         sizeof(uint32) );
      free_size -= sizeof(uint32);
    } /* if ( free_size >= sizeof(uint32) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS DELAY.{0x%x}] "
                 "ResultPush : dMask - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint32),
                 free_size );
    } /* if ! ( free_size >= sizeof(uint32) ) */

    /*==========================================================================*/
    /* Populate Process Delay Results - Delay */
    /*==========================================================================*/


    if ( free_size >= sizeof(uint32) )
    {
      memscpy( res_write_ptr, sizeof(uint32), 
               &(cal_result->et_path_delay_results.delay_p1ns), sizeof(uint32));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                         sizeof(uint32) );
      free_size -= sizeof(uint32);
    } /* if ( free_size >= sizeof(uint32) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS DELAY.{0x%x}] "
                 "ResultPush : dMask - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint32),
                 free_size );
    } /* if ! ( free_size >= sizeof(uint32) ) */


    /*==========================================================================*/
    /* Update Result Written Size */
    /*==========================================================================*/
    oc_disp->res_buff_info.buff_sz = written_size;

    if ( cal_data->cal_config.debug_mode_en )
    {
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "ResultPush : %d bytes",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 written_size );
    }
  //}
  //else
  //{
  //  FTM_MSG( FTM_ERROR, "Aborting Result Push as DPD Construction Result "
  //                      "Row was not found!");
  //}

  /* Free allocated memory */
  if( cal_result != NULL )
  {
    modem_mem_free((void*)cal_result, MODEM_MEM_CLIENT_RFA);
  }

  return return_val;

} /* ftm_calibration_data_process_delay */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Send SW processed Pin value to PC and overwrite NV table
 
  @details
  This function will be called from cal v3 common framework to perform
  Pin computation and to overwrite Static NV delay value
 
  @param data_param
  Pointer to the data structure containing data unpacked parameter
 
  @return
  Flag indicating the status of pin data processing operation
*/
boolean
ftm_calibration_data_process_pin
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  ftm_rf_technology_type ftm_rf_tech,
  uint32 ref_chan,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean return_val = TRUE;
  uint8 *res_write_ptr; /* results write pointer */
  uint16 written_size = 0; /* Total written size */
  uint16 free_size = 0; /* Total free size */
  uint8 res_type = FTM_CAL_OPCODE_DATA;
  uint8 res_action_type = DATA_RES_ACTION_PROCESS_PIN;
  uint8 version = 0;
  uint8 pa_state;
  uint32 chan;
  rfm_device_enum_type rfm_device;
  boolean calculate_ampm_scale_factor = FALSE;
  boolean calculate_amam_offset = FALSE;
  boolean ampm_scale_status = FALSE;
  boolean amam_offset_status = FALSE;
  int32 pin_compensation = 0;
  int32 dummy = 0;
  ftm_calibration_pin_results_type *cal_result = NULL; // for traditional PIN CAL, must do null checks
  ftm_calibration_ext_pin_results_type *cal_result_ext = NULL; // for extended PIN CAL, must do null checks
  
  #ifdef FEATURE_WCDMA
  rfnv_wcdma_nv_tbl_type *nv_tbl = NULL; // must do null checks
  #endif
  #ifdef FEATURE_LTE
  rflte_ftm_mc_sm_type *ftm_mc_sm_ptr = NULL; // must do null checks
  rflte_nv_supported_band_type rfnv_band = RFLTE_NV_INVALID_BAND;
  rflte_nv_tbl_type* rflte_nv_tbl_ptr = NULL; // must do null checks
  rfnv_lte_pa_state_nv_type_s *pa_state_nv_ptr = NULL;
  #endif
  #ifdef FEATURE_TDSCDMA
  //rfnv_tdscdma_tx_tbl_type* tds_nv_tbl_ptr = NULL; // must do null checks
  #endif

  FTM_MSG( FTM_MED, "Enter function ftm_calibration_data_process_pin." );


  /* Param vaildation */
  if (  ( data_param == NULL )  || ( oc_disp == NULL )  )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][TX_MEASURE.{0x%x}] "
               "ResultPush : Null Argument",
               FTM_CAL_OPCODE_DATA );

    return FALSE;
  } /* if  (  ( data_param == NULL )  || ( oc_disp == NULL ) )  */

  if ( data_param->data_param.data_action & DATA_OPCODE_MASK_PROCESS_PIN_VAL_EXT ) /* Extended PIN CAL */
  {
    res_action_type = DATA_RES_ACTION_PROCESS_PIN_EXT;

    /* Allocate required memory and check if memory is allocated */
    cal_result_ext = (ftm_calibration_ext_pin_results_type*)modem_mem_alloc(
                 sizeof(ftm_calibration_ext_pin_results_type), MODEM_MEM_CLIENT_RFA);

    if( cal_result_ext == NULL )
    {
      FTM_MSG( FTM_ERROR, "Failed to allocate memory for extended Pin Results!");
      return_val &= FALSE;
    }
    else
    {
      memset(cal_result_ext, 0, sizeof(ftm_calibration_ext_pin_results_type));
    }

    if( return_val == TRUE )
    {
      chan = data_param->data_param.pin_params.current_chan;
      cal_result_ext->device = data_param->device;
      cal_result_ext->et_pin_results.channel = chan;
      cal_result_ext->et_pin_results.pin_process_mask = data_param->data_param.pin_params.pin_process_mask;
      cal_result_ext->et_pin_results.pin_status_mask = 0; /* initialize to 0 */
      pa_state= data_param->data_param.pin_params.pa_state;

      FTM_MSG_4( FTM_MED, "Before get pin from table. process mask: %d; status mask: %d; pin_offset: %d; AMPM_scaling_factor: %d", 
               cal_result_ext->et_pin_results.pin_process_mask, 
               cal_result_ext->et_pin_results.pin_status_mask, 
               cal_result_ext->et_pin_results.pin_compensation, 
               cal_result_ext->et_pin_results.ampm_scaling_factor );
      
      if ( (data_param->data_param.pin_params.pin_process_mask & PIN_PROCESS_OPTION_AMAM) == PIN_PROCESS_OPTION_AMAM ) 
      {
        calculate_amam_offset = TRUE;
      }

      if ( (data_param->data_param.pin_params.pin_process_mask & PIN_PROCESS_OPTION_AMPM) == PIN_PROCESS_OPTION_AMPM ) 
      {
        calculate_ampm_scale_factor = TRUE;
      }
      
      pin_compensation = ftm_cal_data_get_pin_from_table(chan, 
                                                        data_param->data_param.pin_params.et_compression_point,
                                                        ref_chan,
                                                        &cal_result_ext->et_pin_results.pin_peak_compression,
                                                        &amam_offset_status,
                                                        calculate_ampm_scale_factor,
                                                        &cal_result_ext->et_pin_results.ampm_scaling_factor,
                                                        &ampm_scale_status);

      cal_result_ext->et_pin_results.pin_compensation = pin_compensation;

      if ( (TRUE == calculate_amam_offset) && (TRUE == amam_offset_status) ) 
      {
        cal_result_ext->et_pin_results.pin_status_mask |= PIN_PROCESS_OPTION_AMAM;
      }

      if ( (TRUE == calculate_ampm_scale_factor) && (TRUE == ampm_scale_status) ) 
      {
        cal_result_ext->et_pin_results.pin_status_mask |= PIN_PROCESS_OPTION_AMPM;
      }

      FTM_MSG_4( FTM_MED, "After get pin from table. process mask: %d; status mask: %d; pin_offset: %d; AMPM_scaling_factor: %d", 
               cal_result_ext->et_pin_results.pin_process_mask, 
               cal_result_ext->et_pin_results.pin_status_mask, 
               cal_result_ext->et_pin_results.pin_compensation, 
               cal_result_ext->et_pin_results.ampm_scaling_factor );
    }
  }
  else /* Traditional PIN CAL */
  {
    /* Allocate required memory and check if memory is allocated */
    cal_result = (ftm_calibration_pin_results_type*)modem_mem_alloc(
                 sizeof(ftm_calibration_pin_results_type), MODEM_MEM_CLIENT_RFA);

    if( cal_result == NULL )
    {
      FTM_MSG( FTM_ERROR, "Failed to allocate memory for ET Pin Results!");
      return_val &= FALSE;
    }
    else
    {
      memset(cal_result, 0, sizeof(ftm_calibration_pin_results_type));
    }

    if( return_val == TRUE )
    {
      chan = data_param->data_param.pin_params.current_chan;
      cal_result->device = data_param->device;
      cal_result->et_pin_results.channel = chan;
      pa_state= data_param->data_param.pin_params.pa_state;
      /*Initialize flag to false*/
      cal_result->et_pin_results.pin_cal_success = FALSE;
      pin_compensation = ftm_cal_data_get_pin_from_table(chan,
                                                        data_param->data_param.pin_params.et_compression_point,
                                                        ref_chan,
                                                        &cal_result->et_pin_results.pin_peak_compression,
                                                        &cal_result->et_pin_results.pin_cal_success,
                                                        FALSE,
                                                        &dummy,
                                                        &ampm_scale_status);

      cal_result->et_pin_results.pin_compensation = pin_compensation;
    }
  }

  if( return_val == TRUE )
  {
    /* Print Pin Map */
    ftm_cal_data_print_pin_table();
    #ifdef FEATURE_WCDMA
    if (ftm_rf_tech == FTM_RF_TECH_WCDMA)
    {           
      FTM_MSG( FTM_MED, "Begin PIN memory update for WCDMA");

      /*Get NV  memory ptr*/
      nv_tbl = rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_convert_band_rfcom_to_rfnv(ftm_curr_wcdma_mode));

      if (nv_tbl == NULL)
      {
        FTM_MSG(FTM_ERROR,"ftm_calibration_data_process_pin: Could not get WCDMA NV table pointer");
        return_val &= FALSE;
      }
	  
      rfm_device = (rfm_device_enum_type)data_param->device;
		 
      if ( rfm_device >= RFM_MAX_DEVICES )
      {
        FTM_MSG(FTM_ERROR,"ftm_calibration_data_process_pin: "
								  "Invalid RFM device ");
        return_val &= FALSE;
      }
	 
       /* overwrite NV table value with computed valued */
		   return_val = ftm_cal_data_store_pin_cal_offset(
					&(nv_tbl->lin_temp_comp_data),
					&(nv_tbl->lin_temp_comp_data.pin_cal_vs_freq),
					pa_state,
					chan,
					pin_compensation);
		  
      if(return_val == FALSE)
      {
        FTM_MSG( FTM_ERROR, "ftm_calibration_data_process_pin : Pin Cal values not saved");
      }

      FTM_MSG( FTM_MED, "Finished PIN memory update for WCDMA");

      /*To do*/
      //nv_tbl->et_delay_value = cal_result->et_path_delay_results.delay_p1ns;
      //nv_tbl->xpt_path_delay.num_of_active_entries = 1;
      //nv_tbl->xpt_path_delay.delay_value[0] = cal_result->et_path_delay_results.delay_p1ns;
      //nv_tbl->xpt_path_delay.upper_bound_tx_chan[0] = (uint16) (data_param->data_param.et_delay_params.upper_bound_tx_earfcn);
    }
    #endif
    #ifdef FEATURE_CDMA1X
    if (ftm_rf_tech == FTM_RF_TECH_CDMA)
    {
      boolean api_status = TRUE;
      rfcommon_core_txlin_pin_cal_vs_freq_type* pin_cal_nv_ptr;
      const rf_1x_config_type* rf_1x_config_ptr;
      const rfc_cdma_logical_device_params_type* logical_dev;
      const rf_cdma_data_status_type* device_r;
      rfm_device_enum_type rfm_device;

      FTM_MSG( FTM_MED, "Begin PIN memory update for CDMA");

      rfm_device = (rfm_device_enum_type)data_param->device;

      if ( rfm_device >= RFM_MAX_DEVICES )
      {
        FTM_MSG(FTM_ERROR,"ftm_calibration_data_process_pin: "
                          "Invalie RFM device ");
        api_status = FALSE;

      }

      if ( api_status == TRUE )
      {
        logical_dev = rfc_cdma_get_logical_device_params(rfm_device); 
        device_r  = rf_cdma_get_device_status( rfm_device );

        if ( logical_dev == NULL || device_r == NULL )
        {
          FTM_MSG( FTM_ERROR, "ftm_calibration_data_process_pin: "
                              "NULL CDMA data ptr" );
          api_status = FALSE;
        }
      }

      if ( api_status == TRUE )
      { 
        pin_cal_nv_ptr = rf_cdma_nv_get_mutable_tx_pin_cal(
                                   logical_dev->tx_nv_path[device_r->curr_band], 
                                   device_r->curr_band );

        rf_1x_config_ptr = rf_cdma_nv_get_1x_data( 
                                   logical_dev->tx_nv_path[device_r->curr_band], 
                                   device_r->curr_band );

        if ( pin_cal_nv_ptr == NULL || rf_1x_config_ptr == NULL )
        {
          FTM_MSG(FTM_ERROR,"ftm_calibration_data_process_pin: "
                            "NULL CDMA Pin Cal NV pointer");
          api_status = FALSE;
        }
      }

      if ( api_status == TRUE )
      { 
        return_val = ftm_cal_data_store_pin_cal_offset(
                 &(rf_1x_config_ptr->cdma_common_ptr->tx_static.lin_temp_comp_data),
                 pin_cal_nv_ptr,
                 pa_state,
                 chan,
                 pin_compensation );      
      }
      else
      {
        return_val = FALSE;
      }

      FTM_MSG( FTM_MED, "Finished PIN memory update for CDMA");
    }
    #endif
    #ifdef FEATURE_LTE
    if (ftm_rf_tech == FTM_RF_TECH_LTE)
    {
      FTM_MSG( FTM_MED, "Begin PIN memory update for LTE");

      ftm_mc_sm_ptr = rflte_ftm_mc_get_sm();
      if (ftm_mc_sm_ptr == NULL)
      {
        FTM_MSG(FTM_ERROR,"ftm_calibration_data_process_pin: Could not get LTE SM pointer");
        return_val &= FALSE;
      }
      else
      { 
        rfnv_band = rflte_core_convert_rfcom_band_to_rfnv_band(ftm_mc_sm_ptr->carrier[ftm_mc_sm_ptr->current_carrier_idx].tx_band);
        rflte_nv_tbl_ptr = rflte_nv_get_tbl_ptr(rfnv_band);
        if (rflte_nv_tbl_ptr != NULL)
        {
          rfm_device = (rfm_device_enum_type)data_param->device;
          //rflte_nv_tbl_ptr->xpt_path_delay.num_of_active_entries = 1; 
          if (RFM_DEVICE_0 == rfm_device)
          {
            pa_state_nv_ptr = &(rflte_nv_tbl_ptr->pa_state_nv_c0);
          }
          else if (RFM_DEVICE_2 == rfm_device)
          {
            pa_state_nv_ptr = &(rflte_nv_tbl_ptr->pa_state_nv_c1);
          }
          else
          {
            FTM_MSG_1(FTM_ERROR,"ftm_calibration_data_process_pin: "
                     "Invalid RFM device :%d",
                  rfm_device);
             return_val &= FALSE;
          }

           /* overwrite NV table value with computed valued */
           return_val &= ftm_cal_data_store_pin_cal_offset(&(pa_state_nv_ptr->lin_temp_comp_data),
                                                           &(pa_state_nv_ptr->lin_temp_comp_data.pin_cal_vs_freq),
                                                           pa_state,
                                                           chan,
                                                           (-1 )*pin_compensation);
		  
           if(return_val == FALSE)
           {
             FTM_MSG( FTM_ERROR, "ftm_calibration_data_process_pin : Pin Cal values not saved");
           }
        }
        else
        {
          return_val &= FALSE;
          FTM_MSG(FTM_ERROR,"ftm_calibration_data_process_pin: Couldn't get LTE NV Ptr");
        }
      } /* else case, i.e. ftm_mc_sm_ptr != NULL)*/

      FTM_MSG( FTM_MED, "Finished PIN memory update for WCDMA");
    } /*(ftm_rf_tech == FTM_RF_TECH_LTE)*/
    #endif
    #ifdef FEATURE_TDSCDMA
    if (ftm_rf_tech == FTM_RF_TECH_TDSCDMA)
    {
      /* TODO_PIN: TDS
      tds_nv_tbl_ptr = rf_tdscdma_core_util_init_tx_nv_table(ftm_tdscdma_current_mode);
      */
    }
    #endif
  } /* if( return_val == TRUE ) */
  
    /* Now populate the result buffer from cal_result */
    /* Initialize the free size */ 
    free_size = oc_disp->res_buff_info.buff_sz_max;

    /*==========================================================================*/
    /* Populate the results buffer */
    /*==========================================================================*/

    /* Initialize the write pointer */
    res_write_ptr = oc_disp->res_buff_info.buff_ptr;

    /*==========================================================================*/
    /* Populate Results Type */
    /*==========================================================================*/

    FTM_MSG( FTM_MED, "Writing result type");
    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate version Type */
    /*==========================================================================*/

    FTM_MSG( FTM_MED, "Writing version");
    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(version), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
                 "ResultPush : Version - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ! ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate Device */
    /*==========================================================================*/

    FTM_MSG( FTM_MED, "Writing device");
    if ( free_size >= sizeof(ftm_cal_data_device_type) )
    {
      memscpy( res_write_ptr, sizeof(ftm_cal_data_device_type), 
               &(data_param->device), sizeof(ftm_cal_data_device_type));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                         sizeof(ftm_cal_data_device_type) );
      free_size -= sizeof(ftm_cal_data_device_type);
    } /* if ( free_size >= sizeof(ftm_cal_data_device_type) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
                 "ResultPush : dMask - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(ftm_cal_data_device_type),
                 free_size );
    } /* if ! ( free_size >= sizeof(ftm_cal_data_device_type) ) */
	
    /*==========================================================================*/
    /* Populate Results Action */
    /*==========================================================================*/

    FTM_MSG( FTM_MED, "Writing result action type");
    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_action_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_PIN,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate Process Pin Results - Channel */
    /*==========================================================================*/

    FTM_MSG( FTM_MED, "Writing channel");
    if ( free_size >= sizeof(uint32) )
    {
      memscpy( res_write_ptr, sizeof(uint32),
               &(data_param->data_param.pin_params.current_chan), sizeof(uint32));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                         sizeof(uint32) );
      free_size -= sizeof(uint32);
    } /* if ( free_size >= sizeof(uint32) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
                 "ResultPush : dMask - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint32),
                 free_size );
    } /* if ! ( free_size >= sizeof(uint32) ) */

    if ( (data_param->data_param.data_action & DATA_OPCODE_MASK_PROCESS_PIN_VAL_EXT) == DATA_OPCODE_MASK_PROCESS_PIN_VAL_EXT ) /* Extended PIN CAL */
    {
      /*==========================================================================*/
      /* Populate Process Pin Results - process mask*/
      /*==========================================================================*/
      FTM_MSG( FTM_MED, "Writing process mask");
      if ( free_size >= sizeof(uint8) )
      {
        memscpy( res_write_ptr, sizeof(uint8), 
								 &(cal_result_ext->et_pin_results.pin_process_mask), sizeof(uint8));
        UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
													 sizeof(uint8) );
        free_size -= sizeof(uint8);
      } /* if ( free_size >= sizeof(uint8) ) */
      else
      {
        FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
									 "ResultPush : dMask - ReqSize %d FreeSize %d",
									 cal_data->seg_data.cal_instance,
									 FTM_CAL_OPCODE_DATA,
									 sizeof(uint8),
									 free_size );
      } /* if ! ( free_size >= sizeof(uint8) ) */

      FTM_MSG( FTM_MED, "Done writing process mask." );

      /*==========================================================================*/
      /* Populate Process Pin Results - status mask*/
      /*==========================================================================*/
      FTM_MSG( FTM_MED, "Writing status mask");
      if ( free_size >= sizeof(uint8) )
      {
        memscpy( res_write_ptr, sizeof(uint8), 
								 &(cal_result_ext->et_pin_results.pin_status_mask), sizeof(uint8));
        UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
													 sizeof(uint8) );
        free_size -= sizeof(uint8);
      } /* if ( free_size >= sizeof(uint8) ) */
      else
      {
        FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
									 "ResultPush : dMask - ReqSize %d FreeSize %d",
									 cal_data->seg_data.cal_instance,
									 FTM_CAL_OPCODE_DATA,
									 sizeof(uint8),
									 free_size );
      } /* if ! ( free_size >= sizeof(uint8) ) */

      FTM_MSG( FTM_MED, "Done writing status mask." );

      if ( (cal_result_ext->et_pin_results.pin_status_mask & PIN_PROCESS_OPTION_AMAM) == PIN_PROCESS_OPTION_AMAM )
      {
        /*==========================================================================*/
        /* Populate Process Pin Results - Pin_Compensation*/
        /*==========================================================================*/
        FTM_MSG( FTM_MED, "Writing PIN offset");
        if ( free_size >= sizeof(int32) )
        {
          memscpy( res_write_ptr, sizeof(uint32), 
                   &(cal_result_ext->et_pin_results.pin_compensation), sizeof(int32));
          UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                             sizeof(int32) );
          free_size -= sizeof(int32);
        } /* if ( free_size >= sizeof(int32) ) */
        else
        {
          FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
                     "ResultPush : dMask - ReqSize %d FreeSize %d",
                     cal_data->seg_data.cal_instance,
                     FTM_CAL_OPCODE_DATA,
                     sizeof(int32),
                     free_size );
        } /* if ! ( free_size >= sizeof(int32) ) */

        FTM_MSG( FTM_MED, "Done writing pin offset." );

        /*==========================================================================*/
        /* Populate Process Pin Results - Pin_Peak_Compensation*/
        /*==========================================================================*/
        FTM_MSG( FTM_MED, "Writing peak compression");
        if ( free_size >= sizeof(uint8) )
        {
          memscpy( res_write_ptr, sizeof(uint8), 
                   &(cal_result_ext->et_pin_results.pin_peak_compression), sizeof(uint8));
          UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                             sizeof(uint8) );
          free_size -= sizeof(uint8);
        } /* if ( free_size >= sizeof(uint8) ) */
        else
        {
          FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
                     "ResultPush : dMask - ReqSize %d FreeSize %d",
                     cal_data->seg_data.cal_instance,
                     FTM_CAL_OPCODE_DATA,
                     sizeof(uint8),
                     free_size );
        } /* if ! ( free_size >= sizeof(uint8) ) */

        FTM_MSG( FTM_MED, "Done writing pin peak." );
      }

      if ( (cal_result_ext->et_pin_results.pin_status_mask & PIN_PROCESS_OPTION_AMPM) == PIN_PROCESS_OPTION_AMPM ) 
      {
        /*==========================================================================*/
        /* Populate Process Pin Results - AMPM Scaling factor*/
        /*==========================================================================*/
        FTM_MSG( FTM_MED, "Writing AMPM scaling factor");
        if ( free_size >= sizeof(int32) )
        {
          memscpy( res_write_ptr, sizeof(int32), 
									 &(cal_result_ext->et_pin_results.ampm_scaling_factor), sizeof(int32));
          UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
														 sizeof(int32) );
          free_size -= sizeof(int32);
        } /* if ( free_size >= sizeof(int32) ) */
        else
        {
          FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
										 "ResultPush : dMask - ReqSize %d FreeSize %d",
										 cal_data->seg_data.cal_instance,
										 FTM_CAL_OPCODE_DATA,
										 sizeof(int32),
										 free_size );
        } /* if ! ( free_size >= sizeof(int32) ) */

        FTM_MSG( FTM_MED, "Done writing AMPM scaling factor." );
      } 
    }
    else /* Traditional PIN CAL */
    {
      /*==========================================================================*/
      /* Populate Process Pin Results - Pin_Compensation*/
      /*==========================================================================*/
      if ( free_size >= sizeof(uint32) )
      {
        memscpy( res_write_ptr, sizeof(uint32), 
                 &(cal_result->et_pin_results.pin_compensation), sizeof(uint32));
        UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                           sizeof(uint32) );
        free_size -= sizeof(uint32);
      } /* if ( free_size >= sizeof(uint32) ) */
      else
      {
        FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
                   "ResultPush : dMask - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_DATA,
                   sizeof(uint32),
                   free_size );
      } /* if ! ( free_size >= sizeof(uint32) ) */

      /*==========================================================================*/
      /* Populate Process Pin Results - Pin_Peak_Compensation*/
      /*==========================================================================*/
      if ( free_size >= sizeof(uint8) )
      {
        memscpy( res_write_ptr, sizeof(uint8), 
                 &(cal_result->et_pin_results.pin_peak_compression), sizeof(uint8));
        UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                           sizeof(uint8) );
        free_size -= sizeof(uint8);
      } /* if ( free_size >= sizeof(uint8) ) */
      else
      {
        FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
                   "ResultPush : dMask - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_DATA,
                   sizeof(uint8),
                   free_size );
      } /* if ! ( free_size >= sizeof(uint8) ) */

      /*==========================================================================*/
      /* Populate Process Pin Results - Pin_Cal_Success*/
      /*==========================================================================*/
      if ( free_size >= sizeof(boolean) )
      {
        memscpy( res_write_ptr, sizeof(boolean), 
                 &(cal_result->et_pin_results.pin_cal_success), sizeof(boolean));
        UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                           sizeof(boolean) );
        free_size -= sizeof(boolean);
      } /* if ( free_size >= sizeof(boolean) ) */
      else
      {
        FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS Pin.{0x%x}] "
                   "ResultPush : dMask - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_DATA,
                   sizeof(boolean),
                   free_size );
      } /* if ! ( free_size >= sizeof(uint32) ) */
    }

    /*==========================================================================*/
    /* Update Result Written Size */
    /*==========================================================================*/
    oc_disp->res_buff_info.buff_sz = written_size;

    if ( cal_data->cal_config.debug_mode_en )
    {
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "ResultPush : %d bytes",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 written_size );
    }

  /* Free allocated memory */
  if( cal_result != NULL )
  {
    modem_mem_free((void*)cal_result, MODEM_MEM_CLIENT_RFA);
  }
  if( cal_result_ext != NULL )
  {
    modem_mem_free((void*)cal_result_ext, MODEM_MEM_CLIENT_RFA);
  }

  return return_val;

} /* ftm_calibration_data_process_pin */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform DPD Construction Data Processing
 
  @details
  This function does the data processing for DPD Construction
 
  @param data_param
  Pointer to the data structure containing data opcode unpacked parameter
 
  @return
  Flag indicating the status of data dpd construction processing operation
*/
boolean
ftm_calibration_data_process_dpd_construction
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  uint32 chan,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean return_val = TRUE;
  uint8 *res_write_ptr; /* results write pointer */
  uint16 written_size = 0; /* Total written size */
  uint16 free_size = 0; /* Total free size */
  uint8 res_type = FTM_CAL_OPCODE_DATA;
  uint8 res_action_type = DATA_RES_ACTION_PROCESS_DPD;
  uint8 version = 0;
  uint16 i = 0;
  int16 curr_power = 0;
  int16 next_high_meas_power = 0;
  uint8 xpt_type = 0;
  uint8 meas_pow = 0;

  ftm_calibration_data_result_type *cal_result = NULL;
  ftm_cal_data_dpd_map_row_type *dpd_map_row = NULL;
  ftm_cal_data_dpd_map_row_type *next_high_meas_dpd_map_row = NULL;
  fw_dpd_cal_result_t *dpd_result = NULL;

  /* Param vaildation */
  if ( ( data_param == NULL ) || ( oc_disp == NULL ) )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][TX_MEASURE.{0x%x}] "
               "ResultPush : Null Argument",
               FTM_CAL_OPCODE_DATA );

    return FALSE;
  } /* if ( ( data_param == NULL ) || ( oc_disp == NULL ) ) */

  /* Allocate required memory and check if memory is allocated */
  cal_result = (ftm_calibration_data_result_type*)modem_mem_alloc(
               sizeof(ftm_calibration_data_result_type), MODEM_MEM_CLIENT_RFA);

  if( cal_result == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for DPD Construction Results!");
    return_val &= FALSE;
  }
  else
  {
    memset(cal_result, 0, sizeof(ftm_calibration_data_result_type));
  }
	
  dpd_map_row = (ftm_cal_data_dpd_map_row_type*)modem_mem_alloc(
                sizeof(ftm_cal_data_dpd_map_row_type), MODEM_MEM_CLIENT_RFA);

  if( dpd_map_row == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for DPD Map Row!");
    return_val &= FALSE;
  }

  next_high_meas_dpd_map_row = (ftm_cal_data_dpd_map_row_type*)modem_mem_alloc(
                sizeof(ftm_cal_data_dpd_map_row_type), MODEM_MEM_CLIENT_RFA);

  if( next_high_meas_dpd_map_row == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for DPD Map Row!");
    return_val &= FALSE;
  }

  /* Print DPD Map */
  ftm_cal_data_print_dpd_map();

  if( return_val == TRUE )
  {
    cal_result->device = data_param->device;

    cal_result->dpd_constr_results.num_swp2_meas_pwrs = 0;
    if( data_param->data_param.dpd_constr_params.num_swp2_meas_pwrs > MAX_NUM_MEAS_PWRS )
    {
      FTM_MSG_2( FTM_ERROR, "Number of meas powers %d in payload cannot be greater than %d", 
                 data_param->data_param.dpd_constr_params.num_swp2_meas_pwrs, MAX_NUM_MEAS_PWRS);

      return_val &= FALSE;
    }
    else
    {
      for( i = 0; i < data_param->data_param.dpd_constr_params.num_swp2_meas_pwrs; i++ )
      {
        curr_power = data_param->data_param.dpd_constr_params.swp2_meas_pwrs[i];
        xpt_type = data_param->data_param.dpd_constr_params.xpt_mode;
        return_val &= ftm_cal_data_get_row_data_by_power(chan, 
                                                         dpd_map_row, 
                                                         curr_power, 
                                                         xpt_type);
	if (cal_data->cal_config.debug_mode_en)
        {		
         FTM_MSG_1( FTM_LOW, "Meas Read DPD Map Row Index = %d", dpd_map_row->row_idx);
        }

        if( return_val == TRUE )
        {
          /* Change Env Scale Action Type for sweep 3 to NV Override */
          dpd_map_row->curr_tx_override.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_NV;
          if (cal_data->cal_config.debug_mode_en)
	  {
           FTM_MSG_1( FTM_LOW, "Meas Read DPD Map Row Env Scale Action Changed = %d", 
                     dpd_map_row->curr_tx_override.env_scale_action_type);
	  }
          /* Set Current Override so that we can push it to DPD Map and overwrite the row */
          ftm_cal_data_set_current_override_row(dpd_map_row->row_idx);
          ftm_common_xpt_set_curr_tx_override( data_param->device,
                                               &(dpd_map_row->curr_tx_override) );

          /* Push to DPD Map */
          ftm_cal_data_push_cur_override_to_dpd_map(chan, dpd_map_row->cap_buf_idx, dpd_map_row->res_buf_idx );
        }

        if( return_val == FALSE )
        {
          FTM_MSG_1( FTM_ERROR, "Cant find Meas Power %d in DPD Map in DPD Construction "
                                "Processing", curr_power);
          cal_result->dpd_constr_results.dpd_meas_res_row[i].dpd_valid = FALSE;
          FTM_MSG_1( FTM_ERROR, "Set DPD Valid Flag to %d", 
                     cal_result->dpd_constr_results.dpd_meas_res_row[i].dpd_valid);
          //break;
        }
        else
        {
          cal_result->dpd_constr_results.dpd_meas_res_row[i].dpd_valid = TRUE;
          cal_result->dpd_constr_results.dpd_meas_res_row[i].measured = TRUE;
          cal_result->dpd_constr_results.dpd_meas_res_row[i].pa_state = dpd_map_row->curr_tx_override.pa_state;
          cal_result->dpd_constr_results.dpd_meas_res_row[i].rgi = dpd_map_row->curr_tx_override.rgi;
          cal_result->dpd_constr_results.dpd_meas_res_row[i].smps_bias = dpd_map_row->curr_tx_override.papm_bias;
          cal_result->dpd_constr_results.dpd_meas_res_row[i].xpt_mode = dpd_map_row->curr_tx_override.xpt_mode;
          cal_result->dpd_constr_results.dpd_meas_res_row[i].power = curr_power;
          cal_result->dpd_constr_results.num_swp2_meas_pwrs++;
          if (cal_data->cal_config.debug_mode_en)
          {
           FTM_MSG_1( FTM_MED, "Meas Row DPD Result Curr Power = %d", curr_power);
          FTM_MSG_1( FTM_MED, "Meas Row DPD Result RGI = %d", dpd_map_row->curr_tx_override.rgi);
          FTM_MSG_1( FTM_MED, "Meas Row DPD Result SMPS = %d", dpd_map_row->curr_tx_override.papm_bias);
           FTM_MSG_1( FTM_MED, "Meas Row DPD Result Status = %d", dpd_map_row->status);
          }
          /* Save the DPD data as well */
          dpd_result = (fw_dpd_cal_result_t*)(dpd_map_row->dpd);
          if (cal_data->cal_config.debug_mode_en)
	  {
           FTM_MSG_1( FTM_MED, "Meas DPD Result AMAM[0] = %d", dpd_result->am_dpd[0]);
           FTM_MSG_1( FTM_MED, "Meas DPD Result AMPM[0] = %d", dpd_result->pm_dpd[0]);
          }
          memscpy(&(cal_result->dpd_constr_results.dpd_meas_res_row[i].amam_res[0]), 
                 (sizeof(uint16)*MAX_RES_DPD_SIZE),
                 dpd_result->am_dpd, (sizeof(uint16)*MAX_RES_DPD_SIZE));
          memscpy(&(cal_result->dpd_constr_results.dpd_meas_res_row[i].ampm_res[0]), 
                  (sizeof(int16)*MAX_RES_DPD_SIZE),
                  dpd_result->pm_dpd, (sizeof(int16)*MAX_RES_DPD_SIZE));
        }
      }
    }
    cal_result->dpd_constr_results.num_swp2_calc_pwrs = 0;
    if( data_param->data_param.dpd_constr_params.xpt_mode == 1 )
    {
      if( data_param->data_param.dpd_constr_params.num_swp2_calc_pwrs > MAX_NUM_CALC_PWRS )
      {
        FTM_MSG_2( FTM_ERROR, "Number of meas powers %d in payload cannot be greater than %d", 
                   data_param->data_param.dpd_constr_params.num_swp2_calc_pwrs, MAX_NUM_CALC_PWRS);
      
        return_val &= FALSE;
      }
      else
      {
        for( i = 0; i < data_param->data_param.dpd_constr_params.num_swp2_calc_pwrs; i++ )
        {
          curr_power = data_param->data_param.dpd_constr_params.swp2_calc_pwrs[i];
          xpt_type = data_param->data_param.dpd_constr_params.xpt_mode;
          return_val &= ftm_cal_data_get_row_data_by_power(chan, 
                                                           dpd_map_row, 
                                                           curr_power, 
                                                           xpt_type);

          if( return_val == TRUE )
          {
            /* Change Env Scale Action Type for sweep 3 to NV Override */
            dpd_map_row->curr_tx_override.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_NV;

            /* Set Current Override so that we can push it to DPD Map and overwrite the row */
            ftm_cal_data_set_current_override_row(dpd_map_row->row_idx);
            ftm_common_xpt_set_curr_tx_override( data_param->device, 
                                                 &(dpd_map_row->curr_tx_override) );

            /* Push to DPD Map */
            ftm_cal_data_push_cur_override_to_dpd_map(chan, dpd_map_row->cap_buf_idx, dpd_map_row->res_buf_idx );
          }
      
          if( return_val == FALSE )
          {
            FTM_MSG_1( FTM_ERROR, "Cant find Calc Power %d in DPD Map in DPD Construction "
                                  "Processing", curr_power);
            cal_result->dpd_constr_results.dpd_calc_res_row[i].dpd_valid = FALSE;
            FTM_MSG_1( FTM_ERROR, "Set DPD Valid Flag to %d", 
                     cal_result->dpd_constr_results.dpd_calc_res_row[i].dpd_valid);
            //break;
          }
          else
          {
            cal_result->dpd_constr_results.dpd_calc_res_row[i].dpd_valid = TRUE;
            cal_result->dpd_constr_results.dpd_calc_res_row[i].measured = FALSE;
            cal_result->dpd_constr_results.dpd_calc_res_row[i].pa_state = dpd_map_row->curr_tx_override.pa_state;
            cal_result->dpd_constr_results.dpd_calc_res_row[i].rgi = dpd_map_row->curr_tx_override.rgi;
            cal_result->dpd_constr_results.dpd_calc_res_row[i].smps_bias = dpd_map_row->curr_tx_override.papm_bias;
            cal_result->dpd_constr_results.dpd_calc_res_row[i].xpt_mode = dpd_map_row->curr_tx_override.xpt_mode;
            cal_result->dpd_constr_results.dpd_calc_res_row[i].power = curr_power;
            cal_result->dpd_constr_results.num_swp2_calc_pwrs++;
      
            /* DPD data for Calculated Powers will be same as corresponding higher measured power */
            /* If Calc Power is greater than Max Meas Power, then use Max Meas Power DPD */
            if( curr_power > 
                data_param->data_param.dpd_constr_params.swp2_meas_pwrs[data_param->data_param.dpd_constr_params.num_swp2_meas_pwrs - 1] )
            {
              next_high_meas_power = data_param->data_param.dpd_constr_params.swp2_meas_pwrs[data_param->data_param.dpd_constr_params.num_swp2_meas_pwrs - 1];
              if (cal_data->cal_config.debug_mode_en)
	      {
               FTM_MSG_2( FTM_MED, "Next Higher Meas Power for Calc Power %d = %d", 
                              curr_power, 
                              next_high_meas_power);
              }
            }
            /* Else find the next higher meas power */
            else
            {
              for( meas_pow = 0; meas_pow < data_param->data_param.dpd_constr_params.num_swp2_meas_pwrs; meas_pow++ )
              {
                if( curr_power < data_param->data_param.dpd_constr_params.swp2_meas_pwrs[meas_pow] )
                {
                  next_high_meas_power = data_param->data_param.dpd_constr_params.swp2_meas_pwrs[meas_pow];
		  if (cal_data->cal_config.debug_mode_en)
		  {
                   FTM_MSG_2( FTM_MED, "Next Higher Meas Power for Calc Power %d = %d", 
                              curr_power, 
                              next_high_meas_power);
		  }
                  break;
                }
              }
            }
            /* Once you find the next higher meas power, get the corresponding dpd row */
            return_val &= ftm_cal_data_get_row_data_by_power(chan, 
                                                             next_high_meas_dpd_map_row, 
                                                             next_high_meas_power, 
                                                             xpt_type);

            dpd_result = (fw_dpd_cal_result_t*)(next_high_meas_dpd_map_row->dpd);
	    if (cal_data->cal_config.debug_mode_en)
            {
             FTM_MSG_1( FTM_MED, "Calc DPD Result AMAM[0] = %d", dpd_result->am_dpd[0]);
             FTM_MSG_1( FTM_MED, "Calc DPD Result AMPM[0] = %d", dpd_result->pm_dpd[0]);
            }
            memscpy(&(cal_result->dpd_constr_results.dpd_calc_res_row[i].amam_res[0]), 
                    (sizeof(uint16)*MAX_RES_DPD_SIZE),
                    dpd_result->am_dpd, (sizeof(uint16)*MAX_RES_DPD_SIZE));
            memscpy(&(cal_result->dpd_constr_results.dpd_calc_res_row[i].ampm_res[0]), 
                    (sizeof(int16)*MAX_RES_DPD_SIZE),
                    dpd_result->pm_dpd, (sizeof(int16)*MAX_RES_DPD_SIZE));
          }
        }
      }
    }
  }
  
  ftm_cal_data_print_dpd_map();

  //if( return_val == TRUE )
  //{
    /* Now populate the result buffer from cal_result */
    /* Initialize the free size */ 
    free_size = oc_disp->res_buff_info.buff_sz_max;

    /*==========================================================================*/
    /* Populate the results buffer */
    /*==========================================================================*/

    /* Initialize the write pointer */
    res_write_ptr = oc_disp->res_buff_info.buff_ptr;

    /*==========================================================================*/
    /* Populate Results Type */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA DPD CONSTRUCTION.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate version Type */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(version), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA DPD CONSTRUCTION.{0x%x}] "
                 "ResultPush : Version - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ! ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate Device */
    /*==========================================================================*/

    if ( free_size >= sizeof(ftm_cal_data_device_type) )
    {
      memscpy( res_write_ptr, sizeof(ftm_cal_data_device_type), &(cal_result->device), 
              sizeof(ftm_cal_data_device_type));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                         sizeof(ftm_cal_data_device_type) );
      free_size -= sizeof(ftm_cal_data_device_type);
    } /* if ( free_size >= sizeof(ftm_cal_data_device_type) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA DPD CONSTRUCTION.{0x%x}] "
                 "ResultPush : dMask - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(ftm_cal_data_device_type),
                 free_size );
    } /* if ! ( free_size >= sizeof(ftm_cal_data_device_type) ) */
	
	  /*==========================================================================*/
    /* Populate Results Action */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_action_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS DPD.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_DPD,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate DPD Construction Num Meas Powers */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), 
               &(data_param->data_param.dpd_constr_params.num_swp2_meas_pwrs), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                         sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA DPD CONSTRUCTION.{0x%x}] "
                 "ResultPush : dMask - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ! ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate DPD Construction Num Calc Powers */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), 
               &(data_param->data_param.dpd_constr_params.num_swp2_calc_pwrs), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                         sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA DPD CONSTRUCTION.{0x%x}] "
                 "ResultPush : dMask - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ! ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate DPD Construction Meas Row */
    /*==========================================================================*/

    for( i = 0; i < data_param->data_param.dpd_constr_params.num_swp2_meas_pwrs; i++ )
    {
      if ( free_size >= sizeof(ftm_calibration_data_dpd_result_row_type) )
      {
        memscpy( res_write_ptr, sizeof(ftm_calibration_data_dpd_result_row_type), 
                 &(cal_result->dpd_constr_results.dpd_meas_res_row[i]), 
                 sizeof(ftm_calibration_data_dpd_result_row_type));
        UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                           sizeof(ftm_calibration_data_dpd_result_row_type) );
        free_size -= sizeof(ftm_calibration_data_dpd_result_row_type);
      } /* if ( free_size >= sizeof(ftm_calibration_data_dpd_result_row_type) ) */
      else
      {
        FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA DPD CONSTRUCTION.{0x%x}] "
                   "ResultPush : dMask - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_DATA,
                   sizeof(ftm_calibration_data_dpd_result_row_type),
                   free_size );
      } /* if ! ( free_size >= sizeof(ftm_calibration_data_dpd_result_row_type) ) */
    }

    /*==========================================================================*/
    /* Populate DPD Construction Calc Row */
    /*==========================================================================*/

    for( i = 0; i < data_param->data_param.dpd_constr_params.num_swp2_calc_pwrs; i++ )
    {
      if ( free_size >= sizeof(ftm_calibration_data_dpd_result_row_type) )
      {
        memscpy( res_write_ptr, sizeof(ftm_calibration_data_dpd_result_row_type), 
                 &(cal_result->dpd_constr_results.dpd_calc_res_row[i]), 
                sizeof(ftm_calibration_data_dpd_result_row_type));
        UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                           sizeof(ftm_calibration_data_dpd_result_row_type) );
        free_size -= sizeof(ftm_calibration_data_dpd_result_row_type);
      } /* if ( free_size >= sizeof(ftm_calibration_data_dpd_result_row_type) ) */
      else
      {
        FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA DPD CONSTRUCTION.{0x%x}] "
                   "ResultPush : dMask - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_DATA,
                   sizeof(ftm_calibration_data_dpd_result_row_type),
                   free_size );
      } /* if ! ( free_size >= sizeof(ftm_calibration_data_dpd_result_row_type) ) */
    }

    /*==========================================================================*/
    /* Update Result Written Size */
    /*==========================================================================*/
    oc_disp->res_buff_info.buff_sz = written_size;

    if ( cal_data->cal_config.debug_mode_en )
    {
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "ResultPush : %d bytes",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 written_size );
    }
  //}
  //else
  //{
  //  FTM_MSG( FTM_ERROR, "Aborting Result Push as DPD Construction Result "
  //                      "Row was not found!");
  //}

  /* Free allocated memory */
  if( cal_result != NULL )
  {
    modem_mem_free((void*)cal_result, MODEM_MEM_CLIENT_RFA);
  }
  if( dpd_map_row != NULL )
  {
    modem_mem_free((void*)dpd_map_row, MODEM_MEM_CLIENT_RFA);
  }
  if( next_high_meas_dpd_map_row != NULL )
  {
    modem_mem_free((void*)next_high_meas_dpd_map_row, MODEM_MEM_CLIENT_RFA);
  }

  return return_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Action to return;
  - M Line table
  - Selection sweep table
  - Compression points
  - M Line slope
  depending on the request packet
 
  @details

 
  @param data_param

 
  @return
  Flag indicating the status of M Line results return operation
*/
boolean ftm_calibration_data_process_mline_results
(
   struct ftm_calibration_data_type_t *cal_data,
   ftm_cal_data_parameter_type const *data_param,
   uint32 chan,
   ftm_cal_opcode_dispatch_info_type *oc_disp
   )
{
   boolean return_val = TRUE;
   uint8 *res_write_ptr; /* results write pointer */
   uint16 written_size = 0; /* Total written size */
   uint16 free_size = 0; /* Total free size */
   uint8 res_type = FTM_CAL_OPCODE_DATA;
   uint8 res_action_type = DATA_RES_ACTION_RETURN_MLINE_RESULTS;
   uint8 version = 0;
   uint16 i=0;
   uint8 num_ept_compr_pts = 0;
   uint8 num_et_compr_pts = 0;
   uint8 num_ept_amam_rms_vals = 0;
   uint8 num_et_amam_rms_vals = 0;
   uint8 num_ept_selec_swp_meas_pwrs = 0;
   uint8 num_ept_selec_swp_calc_pwrs = 0;
   uint8 num_et_selec_swp_meas_pwrs = 0;
   ftm_calibration_mline_results_action_type ept_mline_results_action = 0;
   ftm_calibration_mline_results_action_type et_mline_results_action = 0;
   ftm_calibration_mline_result_xpt_type *mline_ept_result_ptr = NULL; //must do NULL checks
   ftm_calibration_mline_result_xpt_type *mline_et_result_ptr = NULL; //must do NULL checks
   ftm_cal_data_module_type *module_ptr = NULL ;//must do NULL checks
   boolean selec_swp_meas_match = TRUE;
   boolean selec_swp_cal_match = TRUE;

   /* Param validation*/
   if ( (data_param == NULL) || (oc_disp == NULL) ) 
   {
      FTM_MSG_1 (FTM_ERROR, "[calv3][Segment X][TX_MEASURE.{0x%x}] "
                 "ResultPush : Null Argument",
                 FTM_CAL_OPCODE_DATA );

      return FALSE;
   }

   /* Get valid data module ptr */
   module_ptr = (ftm_cal_data_module_type*)ftm_common_data_get_data_module_ptr();

   /* Get M Line results actions */
   ept_mline_results_action = data_param->data_param.mline_results.ept_mline_results_action;
   et_mline_results_action = data_param->data_param.mline_results.et_mline_results_action;

   /* Get Number of compression points */
   num_ept_compr_pts = data_param->data_param.mline_results.ept_mline_results_compression_point.num_compression_points;
   num_et_compr_pts = data_param->data_param.mline_results.et_mline_results_compression_point.num_compression_points;

   /* Get Number of AMAM RMS values */
   num_ept_amam_rms_vals = data_param->data_param.mline_results.ept_mline_results_amam_rms.num_amam_rms_vals;
   num_et_amam_rms_vals = data_param->data_param.mline_results.et_mline_results_amam_rms.num_amam_rms_vals;

   if (cal_data->cal_config.debug_mode_en)
   {
    FTM_MSG_1(FTM_MED, "EPT M Line result action = %d", ept_mline_results_action);
    FTM_MSG_1(FTM_MED, "ET MLine result action = %d", et_mline_results_action);
   }

   /* Get pointers to EPT and ET M Line results if we can get the module pointer */
   if ( module_ptr != NULL )
   {
      mline_ept_result_ptr = &module_ptr->mline_results.ept_mline_results;
      mline_et_result_ptr = &module_ptr->mline_results.et_mline_results;

      if ( mline_ept_result_ptr == NULL || mline_et_result_ptr == NULL )
      {
        FTM_MSG(FTM_ERROR, "ftm_calibration_data_process_mline_results()."
                        "NULL value received for ept OR et ptr. Cannot access M Line results");
      }
   }
   else
   {
     FTM_MSG(FTM_ERROR, "ftm_calibration_data_process_mline_results()."
                        "NULL value received for module ptr. Cannot access M Line results");
     return FALSE;
   }

   /* Initialize the free size */ 
   free_size = oc_disp->res_buff_info.buff_sz_max;

   /*==========================================================================*/
   /* Populate the results buffer */
   /*==========================================================================*/

   /* Initialize the write pointer */
   res_write_ptr = oc_disp->res_buff_info.buff_ptr;

   /*==========================================================================*/
   /* Populate Results Type */
   /*==========================================================================*/
   if ( free_size >= sizeof(uint8) )
   {
     memscpy( res_write_ptr, sizeof(uint8), &(res_type), sizeof(uint8));
     UPDATE_SIZE_N_PTR(written_size, res_write_ptr, sizeof(uint8));
     free_size -= sizeof(uint8);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
   }/* if ( free_size >= sizeof(uint8) ) */

   /*==========================================================================*/
   /* Populate version Type */
   /*==========================================================================*/

   if ( free_size >= sizeof(uint8) )
   {
     memscpy( res_write_ptr, sizeof(uint8), &(version), sizeof(uint8));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
     free_size -= sizeof(uint8);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS.{0x%x}] "
                "ResultPush : Version - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_DATA,
                sizeof(uint8),
                free_size );
   } /* if ! ( free_size >= sizeof(uint8) ) */

   /*==========================================================================*/
   /* Populate Device */
   /*==========================================================================*/

   if ( free_size >= sizeof(ftm_cal_data_device_type) )
   {
     memscpy( res_write_ptr, sizeof(ftm_cal_data_device_type), 
             &(data_param->device), sizeof(ftm_cal_data_device_type));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
	  		sizeof(ftm_cal_data_device_type) );
     free_size -= sizeof(ftm_cal_data_device_type);
   } 
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}] "
	        "ResultPush : dMask - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
  		FTM_CAL_OPCODE_DATA,
  		sizeof(ftm_cal_data_device_type),
  		free_size );
   } /* if ! ( free_size >= sizeof(ftm_cal_data_device_type) ) */

   /*==========================================================================*/
   /* Populate Results Action */
   /*==========================================================================*/

   if ( free_size >= sizeof(uint8) )
   {
     memscpy( res_write_ptr, sizeof(uint8), &(res_action_type), sizeof(uint8));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
     free_size -= sizeof(uint8);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}] "
	        "ResultPush : ResType - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
  		DATA_RES_ACTION_PROCESS_MLINE,
  		sizeof(uint8),
  		free_size );
   }

   /*==========================================================================*/
   /* Populate EPT M Line Results Debug Mask */
   /*==========================================================================*/
   if ( free_size >= sizeof(uint32) )
   {
     memscpy(res_write_ptr, sizeof(uint32), &(mline_ept_result_ptr->mline_debug_mask),
             sizeof(uint32));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint32));
     free_size -= sizeof(uint32);
     FTM_MSG_1(FTM_LOW, " EPT Mline Results Debug Mask: %d", 
               mline_ept_result_ptr->mline_debug_mask);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                "ResultPush: ResType - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                DATA_RES_ACTION_PROCESS_MLINE,
                sizeof(uint32),
                free_size);
   }
   /*==========================================================================*/
   /* Populate ET M Line Results Debug Mask */
   /*==========================================================================*/
   if ( free_size >= sizeof(uint32) )
   {
     memscpy(res_write_ptr, sizeof(uint32), &(mline_et_result_ptr->mline_debug_mask),
             sizeof(uint32));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint32));
     free_size -= sizeof(uint32);
     FTM_MSG_1(FTM_LOW, " ET Mline Results Debug Mask: %d", 
               mline_et_result_ptr->mline_debug_mask);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                "ResultPush: ResType - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                DATA_RES_ACTION_PROCESS_MLINE,
                sizeof(uint32),
                free_size);
   }

   /*==========================================================================*/
   /* Populate EPT M Line Results Action */
   /*==========================================================================*/

   if ( free_size >= sizeof(uint16) )
   {
     memscpy( res_write_ptr, sizeof(uint16),
              &ept_mline_results_action, sizeof(uint16));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint16));
     free_size -= sizeof(uint16);
     FTM_MSG_1(FTM_LOW,"EPT Res Action: 0x%x", ept_mline_results_action);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}] "
	        "ResultPush : ResType - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
  		DATA_RES_ACTION_PROCESS_MLINE,
  		sizeof(ftm_calibration_mline_results_action_type),
  		free_size );
   }

   /*==========================================================================*/
   /* Populate ET M Line Results Action */
   /*==========================================================================*/

   if ( free_size >= sizeof(uint16) )
   {
     memscpy( res_write_ptr, sizeof(uint16),
              &et_mline_results_action, sizeof(uint16));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint16));
     free_size -= sizeof(uint16);
     FTM_MSG_1(FTM_LOW,"ET Res Action: 0x%x", et_mline_results_action);
   }
   else
   {

     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}] "
	        "ResultPush : ResType - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
  		DATA_RES_ACTION_PROCESS_MLINE,
  		sizeof(ftm_calibration_mline_results_action_type),
  		free_size );
   }

   /*==========================================================================*/
   /* Populate EPT M Line table */
   /*==========================================================================*/
   
   if ( ept_mline_results_action & RESULT_MLINE_TABLE )
   {
     if ( free_size >= sizeof(uint8) )
     {
       memscpy(res_write_ptr, sizeof(uint8), 
               &(mline_ept_result_ptr->mline_table_result.num_mline_table_rows), sizeof(uint8));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
       free_size -= sizeof(uint8);
     }
     else
     {
       FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(uint8),
                 free_size );
     }/* if ! ( free_size >= sizeof(uint8) ) */

     for (i=0; i < mline_ept_result_ptr->mline_table_result.num_mline_table_rows; i++)
     {
       if ( free_size >= sizeof(ftm_common_xpt_mline_outputs_int_data) )
       {
         memscpy(res_write_ptr, sizeof(ftm_common_xpt_mline_outputs_int_data),
                 &(mline_ept_result_ptr->mline_table_result.mline_results[i]),
                 sizeof(ftm_common_xpt_mline_outputs_int_data));
         UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(ftm_common_xpt_mline_outputs_int_data));
         free_size -= sizeof(ftm_common_xpt_mline_outputs_int_data);
       }
       else
       {
         FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(ftm_common_xpt_mline_outputs_int_data),
                 free_size );
       }/* if ( free_size >= sizeof(ftm_common_xpt_mline_outputs_int_data) ) */
     }
   }

   /*==========================================================================*/
   /* Populate EPT Selection Sweep table */
   /*==========================================================================*/
   if ( ept_mline_results_action & RESULT_SELECTION_SWEEP )
   {
     num_ept_selec_swp_meas_pwrs = 
       data_param->data_param.mline_results.ept_mline_results_selec_swp_meas_params.num_selec_swp_pwrs;
     num_ept_selec_swp_calc_pwrs = 
       data_param->data_param.mline_results.ept_mline_results_selec_swp_calc_params.num_selec_swp_pwrs;

     /* First check if requested selection sweep powers match the actual selection sweep results */
     if ( num_ept_selec_swp_meas_pwrs != mline_ept_result_ptr->selec_swp_meas_result.num_selec_swp_pwrs )
     {
       selec_swp_meas_match = FALSE;
       FTM_MSG_2(FTM_ERROR,"EPT Selection Sweep(Measured): Number of selection sweep power requested = %d"
                 " does not match number of selection sweep power response = %d",num_ept_selec_swp_meas_pwrs,
                 mline_ept_result_ptr->selec_swp_meas_result.num_selec_swp_pwrs);
     }
     else
     {
       if ( free_size >= sizeof(uint8) )
       {
         memscpy( res_write_ptr, sizeof(uint8),
                  &num_ept_selec_swp_meas_pwrs, sizeof(uint8));
         UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
         free_size -= sizeof(uint8);
       }
       else
       {
         FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                   "ResultPush : ResType - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   DATA_RES_ACTION_PROCESS_MLINE,
                   sizeof(uint8),
                   free_size );
       }/* if ( free_size >= sizeof(uint8) ) */
     }/* if ( num_ept_selec_swp_meas_pwrs != ... */

     /* First check if requested selection sweep powers match the actual selection sweep results */
     if ( num_ept_selec_swp_calc_pwrs != mline_ept_result_ptr->selec_swp_calc_result.num_selec_swp_pwrs )
     {
       selec_swp_cal_match = FALSE;
       FTM_MSG_2(FTM_ERROR,"EPT Selection Sweep(Calculated): Number of selection sweep power requested = %d"
                 " does not match number of selection sweep power response = %d",num_ept_selec_swp_calc_pwrs,
                 mline_ept_result_ptr->selec_swp_calc_result.num_selec_swp_pwrs);
     }
     else
     {
       if ( free_size >= sizeof(uint8) )
       {
         memscpy( res_write_ptr, sizeof(uint8),
                  &num_ept_selec_swp_calc_pwrs, sizeof(uint8));
         UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
         free_size -= sizeof(uint8);
       }
       else
       {
         FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                   "ResultPush : ResType - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   DATA_RES_ACTION_PROCESS_MLINE,
                   sizeof(uint8),
                   free_size );
       }/* if ( free_size >= sizeof(uint8) ) */
     }/* if ( num_ept_selec_swp_calc_pwrs != ... */

     if ( selec_swp_meas_match == TRUE )
     {
       for (i = 0; i < num_ept_selec_swp_meas_pwrs; i++)
       {
         if ( free_size >= sizeof(ftm_calibration_mline_selec_swp_op))
         {
           memscpy(res_write_ptr, sizeof(ftm_calibration_mline_selec_swp_op),
                   &(mline_ept_result_ptr->selec_swp_meas_result.mline_selec_swp[i]), sizeof(ftm_calibration_mline_selec_swp_op));
           UPDATE_SIZE_N_PTR(written_size, res_write_ptr, sizeof(ftm_calibration_mline_selec_swp_op));
           free_size -= sizeof(ftm_calibration_mline_selec_swp_op);
         }
         else
         {
           FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                   "ResultPush : ResType - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   DATA_RES_ACTION_PROCESS_MLINE,
                   sizeof(ftm_calibration_mline_selec_swp_op),
                   free_size );
         }/* if ( free_size >= sizeof(ftm_calibration_mline_selec_swp_op)) */
       } /* for (i = 0; i < num_ept_selec_swp_meas_pwrs; i++) */
     } /* if ( selec_swp_meas_match == TRUE ) */

     if ( selec_swp_cal_match == TRUE )
     {
       for (i = 0; i < num_ept_selec_swp_calc_pwrs; i++)
       {
         if ( free_size >= sizeof(ftm_calibration_mline_selec_swp_op))
         {
           memscpy(res_write_ptr, sizeof(ftm_calibration_mline_selec_swp_op),
                   &(mline_ept_result_ptr->selec_swp_calc_result.mline_selec_swp[i]), sizeof(ftm_calibration_mline_selec_swp_op));
           UPDATE_SIZE_N_PTR(written_size, res_write_ptr, sizeof(ftm_calibration_mline_selec_swp_op));
           free_size -= sizeof(ftm_calibration_mline_selec_swp_op);
         }
         else
         {
           FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                   "ResultPush : ResType - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   DATA_RES_ACTION_PROCESS_MLINE,
                   sizeof(ftm_calibration_mline_selec_swp_op),
                   free_size );
         }/* if ( free_size >= sizeof(ftm_calibration_mline_selec_swp_op)) */
       }/* for(i=0 ...*/
     }/* if ( selec_swp_cal_match == TRUE ) */    
   }/* if ( ept_mline_results_action & RESULT_SELECTION_SWEEP ) */

   /*==========================================================================*/
   /* Populate EPT Compression points*/
   /*==========================================================================*/

   if ( ept_mline_results_action & RESULT_COMPRESSION_POINTS )
   {
     if ( free_size >= sizeof(uint8) )
     {
       memscpy(res_write_ptr, sizeof(uint8), 
               &(num_ept_compr_pts),
               sizeof(uint8));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
       free_size -= sizeof(uint8);
     }
     else
     {
       FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(uint8),
                 free_size );
     }/* if ( free_size >= sizeof(uint8)) */

     for (i=0; i < num_ept_compr_pts ; i++)
     {
       if ( free_size >= sizeof(uint8))
       {
         memscpy(res_write_ptr, sizeof(uint8),
                 &(mline_ept_result_ptr->compression_point_result.compression_point_list[i]), sizeof(uint8));
         UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
         free_size -= sizeof(uint8);
       }
       else
       {
         FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(uint8),
                 free_size );
       }/* if ( free_size >= sizeof(uint8)) */
     }
   }

   /*==========================================================================*/
   /* Populate EPT Slope - Intercepts */
   /*==========================================================================*/

   if ( ept_mline_results_action & RESULT_MLINE_SLOPE_POINTS )
   {
     if ( free_size > sizeof(ftm_calibration_mline_result_slope_type))
     {
       memscpy(res_write_ptr, sizeof(ftm_calibration_mline_result_slope_type),
               &(mline_ept_result_ptr->mline_slope_result), 
               sizeof(ftm_calibration_mline_result_slope_type));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(ftm_calibration_mline_result_slope_type) );
       free_size -= sizeof(ftm_calibration_mline_result_slope_type);
     }
     else
     {
       FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(ftm_calibration_mline_result_slope_type),
                 free_size );
     }/* if ( free_size > sizeof(ftm_calibration_mline_result_slope_type)) */
   }

   /*==========================================================================*/
   /* Populate EPT AMAM RMS values */
   /*==========================================================================*/

   if ( ept_mline_results_action & RESULT_AMAM_RMS )
   {
     if ( free_size >= sizeof(uint8) )
     {
       memscpy(res_write_ptr, sizeof(uint8), 
               &(num_ept_amam_rms_vals),
               sizeof(uint8));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
       free_size -= sizeof(uint8);
     }
     else
     {
       FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(uint8),
                 free_size );
     }/* if ( free_size >= sizeof(uint8)) */

     for (i=0; i < num_ept_amam_rms_vals ; i++)
     {
       if ( free_size >= sizeof(uint32))
       {
         memscpy(res_write_ptr, sizeof(uint32),
                 &(mline_ept_result_ptr->amam_rms_result.amam_rms[i]), sizeof(uint32));
         UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint32) );
         free_size -= sizeof(uint32);
       }
       else
       {
         FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(uint32),
                 free_size );
       }/* if ( free_size >= sizeof(uint32)) */
     }
   }

   /*==========================================================================*/
   /* Populate ET M Line table */
   /*==========================================================================*/

   if ( et_mline_results_action & RESULT_MLINE_TABLE )
   {
     if ( free_size >= sizeof(uint8) )
     {
       memscpy(res_write_ptr, sizeof(uint8), 
               &(mline_et_result_ptr->mline_table_result.num_mline_table_rows), sizeof(uint8));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
       free_size -= sizeof(uint8);
     }
     else
     {
       FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(uint8),
                 free_size );
     }/* if ! ( free_size >= sizeof(uint8) ) */

     for (i=0; i < mline_et_result_ptr->mline_table_result.num_mline_table_rows; i++)
     {
       if ( free_size >= sizeof(ftm_common_xpt_mline_outputs_int_data) )
       {
         memscpy(res_write_ptr, sizeof(ftm_common_xpt_mline_outputs_int_data),
                 &(mline_et_result_ptr->mline_table_result.mline_results[i]),
                 sizeof(ftm_common_xpt_mline_outputs_int_data));
         UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(ftm_common_xpt_mline_outputs_int_data));
         free_size -= sizeof(ftm_common_xpt_mline_outputs_int_data);
       }
       else
       {
         FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(ftm_common_xpt_mline_outputs_int_data),
                 free_size );
       }/* if ( free_size >= sizeof(ftm_common_xpt_mline_outputs_int_data) ) */
     }
   }
   /*==========================================================================*/
   /* Populate ET Selection Sweep table */
   /*==========================================================================*/
   if ( et_mline_results_action & RESULT_SELECTION_SWEEP )
   {
     num_et_selec_swp_meas_pwrs =
       data_param->data_param.mline_results.et_mline_results_selec_swp_params.num_selec_swp_pwrs;

     /* First check if requested selection sweep powers match the actual selection sweep results */
     if ( num_et_selec_swp_meas_pwrs != mline_et_result_ptr->selec_swp_meas_result.num_selec_swp_pwrs )
     {
       FTM_MSG_2(FTM_ERROR,"ET Selection Sweep(Measured): Number of selection sweep power requested = %d"
                   " does not match number of selection sweep power response = %d",num_et_selec_swp_meas_pwrs,
                   mline_et_result_ptr->selec_swp_meas_result.num_selec_swp_pwrs);
     }
     else
     {
       if ( free_size >=sizeof(uint8) )
       {
         memscpy(res_write_ptr, sizeof(uint8),
                 &num_et_selec_swp_meas_pwrs, sizeof(uint8));
         UPDATE_SIZE_N_PTR(written_size, res_write_ptr, sizeof(uint8));
         free_size -=sizeof(uint8);
         FTM_MSG_1(FTM_MED,"Num ET Selection Sweep Pts = %d",num_et_selec_swp_meas_pwrs);
       }
       else
       {
         FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                     "ResultPush : ResType - ReqSize %d FreeSize %d",
                     cal_data->seg_data.cal_instance,
                     DATA_RES_ACTION_PROCESS_MLINE,
                     sizeof(uint8),
                     free_size );
       }/* if ( free_size >= sizeof(uint8) ) */
   
       for (i = 0; i < num_et_selec_swp_meas_pwrs; i++)
       {
         if ( free_size >= sizeof(ftm_calibration_mline_selec_swp_op) )
         {
           memscpy(res_write_ptr, sizeof(ftm_calibration_mline_selec_swp_op),
                   &(mline_et_result_ptr->selec_swp_meas_result.mline_selec_swp[i]),
                   sizeof(ftm_calibration_mline_selec_swp_op));
           UPDATE_SIZE_N_PTR(written_size, res_write_ptr, sizeof(ftm_calibration_mline_selec_swp_op));
           free_size -= sizeof(ftm_calibration_mline_selec_swp_op);
         }
         else
         {
           FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                     "ResultPush : ResType - ReqSize %d FreeSize %d",
                     cal_data->seg_data.cal_instance,
                     DATA_RES_ACTION_PROCESS_MLINE,
                     sizeof(ftm_calibration_mline_selec_swp_op),
                     free_size );
         }/* if ( free_size >= sizeof(ftm_calibration_mline_selec_swp_op)) */
       } /* for (i = 0; i < num_et_selec_swp_meas_pwrs; i++) */
     } /* if ( num_et_selec_swp_meas_pwrs != mline...*/
   }/* if ( et_mline_results_action & RESULT_SELECTION_SWEEP ) */
   
   /*==========================================================================*/
   /* Populate ET Compression points*/
   /*==========================================================================*/

   if ( et_mline_results_action & RESULT_COMPRESSION_POINTS )
   {
     if ( free_size >= sizeof(uint8) )
     {
       memscpy(res_write_ptr, sizeof(uint8), 
               &(num_et_compr_pts),
               sizeof(uint8));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
       free_size -= sizeof(uint8);
     }
     else
     {
       FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(ftm_common_xpt_mline_outputs_int_data),
                 free_size );
     }/* if ( free_size >= sizeof(ftm_common_xpt_mline_outputs_int_data) ) */

     for (i=0; i < num_et_compr_pts ; i++)
     {
       if ( free_size >= sizeof(uint8))
       {
         memscpy(res_write_ptr, sizeof(uint8),
                 &(mline_et_result_ptr->compression_point_result.compression_point_list[i]), sizeof(uint8));
         UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
         free_size -= sizeof(uint8);
       }
       else
       {
         FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(uint8),
                 free_size );
       }/* if ( free_size >= sizeof(uint8)) */
     }
   }

   /*==========================================================================*/
   /* Populate ET Slope - Intercepts */
   /*==========================================================================*/

   if ( et_mline_results_action & RESULT_MLINE_SLOPE_POINTS )
   {
     if ( free_size > sizeof(ftm_calibration_mline_result_slope_type))
     {
       memscpy(res_write_ptr, sizeof(ftm_calibration_mline_result_slope_type),
               &(mline_et_result_ptr->mline_slope_result), 
               sizeof(ftm_calibration_mline_result_slope_type));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(ftm_calibration_mline_result_slope_type) );
       free_size -= sizeof(ftm_calibration_mline_result_slope_type);
     }
     else
     {
       FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(ftm_calibration_mline_result_slope_type),
                 free_size );
     }/* if ( free_size > sizeof(ftm_calibration_mline_result_slope_type)) */
   }

   /*==========================================================================*/
   /* Populate ET AMAM RMS values */
   /*==========================================================================*/

   if ( et_mline_results_action & RESULT_AMAM_RMS )
   {
     if ( free_size >= sizeof(uint8) )
     {
       memscpy(res_write_ptr, sizeof(uint8), 
               &(num_et_amam_rms_vals),
               sizeof(uint8));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
       free_size -= sizeof(uint8);
     }
     else
     {
       FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(uint8),
                 free_size );
     }/* if ( free_size >= sizeof(uint8)) */

     for (i=0; i < num_et_amam_rms_vals ; i++)
     {
       if ( free_size >= sizeof(uint32))
       {
         memscpy(res_write_ptr, sizeof(uint32),
                 &(mline_et_result_ptr->amam_rms_result.amam_rms[i]), sizeof(uint32));
         UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint32) );
         free_size -= sizeof(uint32);
       }
       else
       {
         FTM_MSG_4(FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS MLINE RESULTS{0x%x}]"
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_PROCESS_MLINE,
                 sizeof(uint32),
                 free_size );
       }/* if ( free_size >= sizeof(uint32)) */
     }
   }

   /*==========================================================================*/
    /* Update Result Written Size */
    /*==========================================================================*/
    oc_disp->res_buff_info.buff_sz = written_size;

    if ( cal_data->cal_config.debug_mode_en )
    {
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "ResultPush : %d bytes",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 written_size );
    }

  return return_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Action to return;
  - M Line table
  - Selection sweep table
  - Compression points
  - M Line slope
  depending on the request packet
 
  @details

 
  @param data_param

 
  @return
  Flag indicating the status of M Line results return operation
*/
boolean ftm_calibration_data_process_xpt_capture_results
(
   struct ftm_calibration_data_type_t *cal_data,
   ftm_cal_data_parameter_type const *data_param,
   uint32 chan,
   ftm_cal_opcode_dispatch_info_type *oc_disp
   )
{
   boolean return_val = TRUE;
   uint8 *res_write_ptr; /* results write pointer */
   uint16 written_size = 0; /* Total written size */
   uint16 free_size = 0; /* Total free size */
   uint8 res_type = FTM_CAL_OPCODE_DATA;
   uint8 res_action_type = DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS;
   uint8 version = 0;
   uint16 i=0;
   uint32 ret_option = 0;
   uint32 num_res = 0;
   int32 dpd_map_size = 0;
   boolean dpd_valid;
   fw_dpd_cal_result_t *dpd_result = NULL;
   ftm_cal_data_module_type*  module_ptr;
   uint32 temp_numresults = 0;
   uint32 temp_icq = 0;


   /* Param validation*/
   if ( (data_param == NULL) || (oc_disp == NULL) ) 
   {
      FTM_MSG_1 (FTM_ERROR, "[calv3][Segment X][DATA.{0x%x}] "
                 "ResultPush : Null Argument",
                 FTM_CAL_OPCODE_DATA );

      return FALSE;
   }

   /* Get valid data module ptr */
   module_ptr = (ftm_cal_data_module_type*)ftm_common_data_get_data_module_ptr();

   /* Get input param- result mask.  */
   ret_option = data_param->data_param.xpt_capture_results.result_mask;
   num_res = data_param->data_param.xpt_capture_results.num_results;

   /* Get pointers to EPT and ET M Line results if we can get the module pointer */
   if ( module_ptr != NULL )
   {
     dpd_map_size = module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->num_map_entries;
     if( dpd_map_size < num_res )
     {
       FTM_MSG_2(FTM_ERROR,"DPD map size  - %d, is smaller than requested number of results - %d",dpd_map_size,num_res );
       return_val =  0;
     }
   }

   /* Initialize the free size */ 
   free_size = oc_disp->res_buff_info.buff_sz_max;

   /*==========================================================================*/
   /* Populate the results buffer */
   /*==========================================================================*/

   /* Initialize the write pointer */
   res_write_ptr = oc_disp->res_buff_info.buff_ptr;

   /*==========================================================================*/
   /* Populate Results Type */
   /*==========================================================================*/
   if ( free_size >= sizeof(uint8) )
   {
     memscpy( res_write_ptr, sizeof(uint8), &(res_type), sizeof(uint8));
     UPDATE_SIZE_N_PTR(written_size, res_write_ptr, sizeof(uint8));
     free_size -= sizeof(uint8);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
   }/* if ( free_size >= sizeof(uint8) ) */

   /*==========================================================================*/
   /* Populate version Type */
   /*==========================================================================*/

   if ( free_size >= sizeof(uint8) )
   {
     memscpy( res_write_ptr, sizeof(uint8), &(version), sizeof(uint8));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
     free_size -= sizeof(uint8);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS.{0x%x}] "
                "ResultPush : Version - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_DATA,
                sizeof(uint8),
                free_size );
   } /* if ! ( free_size >= sizeof(uint8) ) */

   /*==========================================================================*/
   /* Populate Device */
   /*==========================================================================*/

   if ( free_size >= sizeof(ftm_cal_data_device_type) )
   {
     memscpy( res_write_ptr, sizeof(ftm_cal_data_device_type), 
             &(data_param->device), sizeof(ftm_cal_data_device_type));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
	  		sizeof(ftm_cal_data_device_type) );
     free_size -= sizeof(ftm_cal_data_device_type);
   } 
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
	        "ResultPush : dMask - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
  		FTM_CAL_OPCODE_DATA,
  		sizeof(ftm_cal_data_device_type),
  		free_size );
   } /* if ! ( free_size >= sizeof(ftm_cal_data_device_type) ) */

   /*==========================================================================*/
   /* Populate Results Action */
   /*==========================================================================*/

   if ( free_size >= sizeof(uint8) )
   {
     memscpy( res_write_ptr, sizeof(uint8), &(res_action_type), sizeof(uint8));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
     free_size -= sizeof(uint8);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
	        "ResultPush : ResType - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
  		DATA_RES_ACTION_PROCESS_MLINE,
  		sizeof(uint8),
  		free_size );
   }
   /*==========================================================================*/
   /* Populate Result Mask */
   /*==========================================================================*/
   if ( free_size >= sizeof(uint32) )
   {
     memscpy(res_write_ptr, sizeof(uint32), &ret_option,
             sizeof(uint32));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint32));
     free_size -= sizeof(uint32);
     FTM_MSG_1(FTM_LOW, " XPT capture Results Mask: %d", 
               ret_option);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}]"
                "ResultPush: ResType - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
                sizeof(uint32),
                free_size);
   }
   /*==========================================================================*/
   /* Populate NUM Results */
   /*==========================================================================*/
   if ( free_size >= sizeof(uint32) )
   {
	 temp_numresults = (uint32)module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->num_map_entries;
	 memscpy(res_write_ptr, sizeof(uint32), &(temp_numresults),
             sizeof(uint32));
     UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint32));
     free_size -= sizeof(uint32);
     FTM_MSG_1(FTM_LOW, " XPT capture Num Results: %d", 
               module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->num_map_entries);
   }
   else
   {
     FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}]"
                "ResultPush: ResType - ReqSize %d FreeSize %d",
                cal_data->seg_data.cal_instance,
                DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
                sizeof(uint32),
                free_size);
   }
   /*==========================================================================*/
   /* Populate XPT capture Results */
   /*==========================================================================*/
   
   for(i = 0 ; i <dpd_map_size; i++ )
   {
     dpd_valid = (module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[i].status == 
                   FTM_COMMON_XPT_IQ_PROCESSING_COMPLETE);
     /*==========================================================================*/
     /* Populate DPD valid flag */
     /*==========================================================================*/
     if ( free_size >= sizeof(uint8) )
     {
       memscpy( res_write_ptr, sizeof(uint8),
                &dpd_valid, sizeof(uint8));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
       free_size -= sizeof(uint8);
     }
     else
     {
       FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
            "ResultPush : ResType - ReqSize %d FreeSize %d",
                  cal_data->seg_data.cal_instance,
        DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
        sizeof(uint8),
        free_size );
     }
     /*==========================================================================*/
     /* Populate XPT mode flag */
     /*==========================================================================*/
     if ( free_size >= sizeof(uint8) )
     {
       memscpy( res_write_ptr, sizeof(uint8),
                &module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[i].curr_tx_override.xpt_mode, sizeof(uint8));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
       free_size -= sizeof(uint8);
     }
     else
     {
       FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
            "ResultPush : ResType - ReqSize %d FreeSize %d",
                  cal_data->seg_data.cal_instance,
        DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
        sizeof(uint8),
        free_size );
     }
     /*==========================================================================*/
     /* Populate RGI flag */
     /*==========================================================================*/
     if ( free_size >= sizeof(uint16) )
     {
       memscpy( res_write_ptr, sizeof(uint16),
                &module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[i].curr_tx_override.rgi, sizeof(uint16));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint16));
       free_size -= sizeof(uint16);
     }
     else
     {
       FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
            "ResultPush : ResType - ReqSize %d FreeSize %d",
                  cal_data->seg_data.cal_instance,
        DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
        sizeof(uint16),
        free_size );
     }
     /*==========================================================================*/
     /* Populate BIAS flag */
     /*==========================================================================*/
     if ( free_size >= sizeof(uint16) )
     {
       memscpy( res_write_ptr, sizeof(uint16),
                &module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[i].curr_tx_override.papm_bias, sizeof(uint16));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint16));
       free_size -= sizeof(uint16);
     }
     else
     {
       FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
            "ResultPush : ResType - ReqSize %d FreeSize %d",
                  cal_data->seg_data.cal_instance,
        DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
        sizeof(uint16),
        free_size );
     }
     /*==========================================================================*/
     /* Populate PA state */
     /*==========================================================================*/
     if ( free_size >= sizeof(uint8) )
     {
       memscpy( res_write_ptr, sizeof(uint8),
                &module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[i].curr_tx_override.pa_state, sizeof(uint8));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8));
       free_size -= sizeof(uint8);
     }
     else
     {
       FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
            "ResultPush : ResType - ReqSize %d FreeSize %d",
                  cal_data->seg_data.cal_instance,
        DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
        sizeof(uint8),
        free_size );
     }

     /*==========================================================================*/
     /* Populate ICQ */
     /*==========================================================================*/
     if ( free_size >= sizeof(uint32) )
     {
		temp_icq = (uint32)module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[i].curr_tx_override.pa_curr;
		memscpy(res_write_ptr, sizeof(uint32), &temp_icq, sizeof(uint32));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint32));
       free_size -= sizeof(uint32);
     }
     else
     {
       FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
            "ResultPush : ResType - ReqSize %d FreeSize %d",
                  cal_data->seg_data.cal_instance,
        DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
        sizeof(uint32),
        free_size );
     }
     /*==========================================================================*/
     /* Populate IQ GAIN  */
     /*==========================================================================*/
     if ( free_size >= sizeof(uint32) )
     {
       memscpy( res_write_ptr, sizeof(uint32),
                &module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[i].curr_tx_override.iq_gain, sizeof(uint32));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint32));
       free_size -= sizeof(uint32);
     }
     else
     {
       FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
            "ResultPush : ResType - ReqSize %d FreeSize %d",
                  cal_data->seg_data.cal_instance,
        DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
        sizeof(uint32),
        free_size );
     }
     /*==========================================================================*/
     /* Populate ENV SCALE  */
     /*==========================================================================*/
     if ( free_size >= sizeof(uint32) )
     {
       memscpy( res_write_ptr, sizeof(uint32),
                &module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[i].curr_tx_override.env_scale, sizeof(uint32));
       UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint32));
       free_size -= sizeof(uint32);
     }
     else
     {
       FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
            "ResultPush : ResType - ReqSize %d FreeSize %d",
                  cal_data->seg_data.cal_instance,
        DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
        sizeof(uint32),
        free_size );
     }

     if(ret_option & RESULT_COMPRESSION_PT)
     {
       /*==========================================================================*/
       /* Populate CompRESSION  flag */
       /*==========================================================================*/
       if ( free_size >= sizeof(uint16) )
       {
         memscpy( res_write_ptr, sizeof(uint16),
                  &module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[i].compdb10, sizeof(uint16));
         UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint16));
         free_size -= sizeof(uint16);
       }
       else
       {
         FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
              "ResultPush : ResType - ReqSize %d FreeSize %d",
                    cal_data->seg_data.cal_instance,
          DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
          sizeof(uint16),
          free_size );
       }
     }
     if( ret_option & RESULT_DPD_TABLES )
     {
       /*==========================================================================*/
       /* Populate DPD table */
       /*==========================================================================*/
       dpd_result = (fw_dpd_cal_result_t*)module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[i].dpd;
       
       if ( free_size >= sizeof(uint16)*MAX_RES_DPD_SIZE*2 )
       {
         memscpy( res_write_ptr, sizeof(uint16)*MAX_RES_DPD_SIZE,
                  &dpd_result->am_dpd[0], sizeof(uint16)*MAX_RES_DPD_SIZE);
         UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint16)*MAX_RES_DPD_SIZE);
         memscpy( res_write_ptr, sizeof(uint16)*MAX_RES_DPD_SIZE,
                  &dpd_result->pm_dpd[0], sizeof(uint16)*MAX_RES_DPD_SIZE);
         UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint16)*MAX_RES_DPD_SIZE);
         free_size -= sizeof(uint16)*MAX_RES_DPD_SIZE*2;
         FTM_MSG_3(FTM_HIGH, "DPD table 1st entry is  %d, 2'nd entry s %d, 3'rd entry is %d",dpd_result->am_dpd[0],
                   dpd_result->am_dpd[1],dpd_result->am_dpd[2] );
       }
       else
       {
         FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
              "ResultPush : ResType - ReqSize %d FreeSize %d",
                    cal_data->seg_data.cal_instance,
          DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
          sizeof(uint16)*MAX_RES_DPD_SIZE*2,
          free_size );
       }
     }
     if ( ret_option & RESULT_RX_PAR ) 
     {
       /*==========================================================================*/
       /* Populate PAR */
       /*==========================================================================*/
        dpd_result = (fw_dpd_cal_result_t*)module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[i].dpd;

        if ( free_size >= sizeof(int16) )
        {
          memscpy( res_write_ptr, sizeof(int16),
                   &dpd_result->par, sizeof(int16));
          UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(int16));

          free_size -= sizeof(int16);
          FTM_MSG_2(FTM_HIGH, "Rx PAR is %d Peak PAR %d", dpd_result->par, dpd_result->par_max );
        }
        else
        {
          FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
               "ResultPush : ResType - ReqSize %d FreeSize %d",
                     cal_data->seg_data.cal_instance,
           DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
           sizeof(int16),
           free_size );
        }
     }
     else
     {
       FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][DATA PROCESS XPT CAPTURE RESULTS{0x%x}] "
            "ResultPush : ResType - %d",
                  cal_data->seg_data.cal_instance,
        DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS,
        ret_option);

     }

   } /* for i = 0 : dpd_map_size*/

   /*==========================================================================*/
    /* Update Result Written Size */
    /*==========================================================================*/
    oc_disp->res_buff_info.buff_sz = written_size;

    if ( cal_data->seg_data.oc_payload.data.up_param.debug_mode_en == TRUE )
    {
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "ResultPush : %d bytes",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 written_size );
    }

   /*==========================================================================*/
    /* Flush DPD Result table */
    /*==========================================================================*/

	ftm_cal_data_clear_current_dpd_map(module_ptr->dpd_map_cur_chan);


  return return_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Action to Load Calculated DPDs to HW and update DPD Map
 
  @details
  This function will be called from cal v3 common framework to perform
  loading of calculated DPDs to HW.
 
  @param data_param
  Pointer to the data structure containing data unpacked parameter
 
  @return
  Flag indicating the status of load calc dpd operation
*/
boolean
ftm_calibration_data_load_calc_dpd
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  ftm_rf_technology_type ftm_rf_tech,
  uint32 chan,
  uint8 device_ind,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  uint16 *amam_ptr = NULL;
  uint16 *ampm_ptr = NULL;
  uint16 *det_ptr = NULL;
  uint8 xpt_type = 0;
  uint8 load_action = 0;
  uint8 lp_var = 0;
  uint8 i = 0;
  int16 curr_power = 0;
  uint16 qpoet_min_volt_x1000 = 0;
  uint16 qpoet_max_volt_x1000 = 0;
  rfdevice_papm_misc_info et_voltage_info;
  void* papm_device;
  uint16 et_min_volt_x1000 = 0;
  boolean return_val = TRUE;

  uint8   amam_bank_index = -1;
  uint16  amam_bank_offset = -1;
  uint8   ampm_bank_index = -1;
  uint16  ampm_bank_offset = -1;
  uint8   epdt_bank_index = -1;
  uint16  epdt_bank_offset = -1;
  ftm_xpt_txagc_override_type ovr_params;
  ftm_cal_data_unity_hwid_type *unity_hwids_meas = NULL;
  ftm_cal_data_unity_hwid_type *unity_hwids_calc = NULL;
  ftm_cal_data_dpd_map_row_type *dpd_map_row = NULL;
  fw_dpd_cal_result_t *dpd_result = NULL;
  int16 row_bias = 0;
  ftm_cal_data_det_type *epdt_tbl = NULL;
  uint16 epdt_size = MAX_NUM_DET_VALS;
  uint16 expAMAMtable[MAX_NUM_DPD_VALS];
  int16  expAMPMtable[MAX_NUM_DPD_VALS];
  uint16 expEPDTTable[MAX_NUM_DET_VALS];
  uint16 decimateEPDTTable[MAX_RES_DPD_SIZE];
  int16 next_high_meas_power = 0;
  uint8 meas_pow = 0;
  uint16 amam_val = 0;
  rfcommon_mdsp_xpt_mode_enum_type load_dpd_xpt_type = 0;
  rfcommon_core_xpt_tx_override_type curr_tx_ovr;

  uint8 *res_write_ptr; /* results write pointer */
  uint16 written_size = 0; /* Total written size */
  uint16 free_size = 0; /* Total free size */
  uint8 res_type = FTM_CAL_OPCODE_DATA;
  uint8 res_action_type = DATA_RES_ACTION_LOAD_CALC_DPD;
  uint8 version = 0;
  ftm_calibration_data_result_type *cal_result = NULL;
  xpt_dpd_scaling_value_type dpd_scale_factor;

  /* Disable DPD scaling for calibration */
  dpd_scale_factor.valid = FALSE;

  /* Get the XPT Type */
  xpt_type = data_param->data_param.load_calc_dpd_params.xpt_mode;

  load_dpd_xpt_type = xpt_type;

  if (cal_data->cal_config.debug_mode_en)
  {
   FTM_MSG_1( FTM_MED, "Load Calc DPD XPT Type = %d", xpt_type);
  }
  /* Get Load Action */
  load_action = data_param->data_param.load_calc_dpd_params.load_action;
  if (cal_data->cal_config.debug_mode_en)
  {
   FTM_MSG_1( FTM_MED, "Load Calc DPD Action = %d", load_action);
  }

  /*Check for ET mode*/
  if ( xpt_type == 2 )
  {
    /*Get the ET voltage min and max from PAPM device*/

    //Get the PAPM device in use
    /*! Note: 
      This is a temporary code to get the PAPM device at instance-0 directly from common RFC.
      The assumption here is that there is only one PAPM device in the RF card.
      The correct implementation should come from each tech who should provide the PAPM device in use */
    papm_device = rfc_common_get_cmn_rf_device_object( RFDEVICE_PAPM, 0 );

    // Get the min-max ET_voltage from PAPM device
    return_val = rfdevice_pam_get_et_voltage_min_max( papm_device, &et_voltage_info );   

    if( !return_val ) 
    {
      FTM_MSG( FTM_ERROR, "PAPM device returned ERRROR while querying for ET_Voltage info" );
      return FALSE;
    }

    // Get QPOET Min and Max Voltage and ET min Voltage 
    qpoet_min_volt_x1000 = et_voltage_info.ET_Vmin;
    qpoet_max_volt_x1000 = et_voltage_info.ET_Vmax;

    et_min_volt_x1000 = data_param->data_param.load_calc_dpd_params.et_min_volt_x100 * 10;

    if (cal_data->cal_config.debug_mode_en)
    {
     FTM_MSG_3( FTM_MED, "QPOET Min Volt = %d, QPOET Max Volt = %d, ET Min Volt = %d", 
              qpoet_min_volt_x1000,
              qpoet_max_volt_x1000,
              et_min_volt_x1000);
    }
  }

  /* Validate number of Swp2 Meas and Calc Power */
  if( data_param->data_param.load_calc_dpd_params.num_swp2_meas_pwrs > MAX_NUM_MEAS_PWRS )
  {
    FTM_MSG_2( FTM_ERROR, "Number of Swp2 Meas Powers %d cannot be greater than %d", 
               data_param->data_param.load_calc_dpd_params.num_swp2_meas_pwrs, 
               MAX_NUM_MEAS_PWRS);
    return FALSE;
  }
  if( data_param->data_param.load_calc_dpd_params.num_swp2_calc_pwrs > MAX_NUM_CALC_PWRS )
  {
    FTM_MSG_2( FTM_ERROR, "Number of Swp2 Calc Powers %d cannot be greater than %d", 
               data_param->data_param.load_calc_dpd_params.num_swp2_calc_pwrs, 
               MAX_NUM_CALC_PWRS);
    return FALSE;
  }

  dpd_map_row = (ftm_cal_data_dpd_map_row_type*)modem_mem_alloc(
                sizeof(ftm_cal_data_dpd_map_row_type), MODEM_MEM_CLIENT_RFA);

  if( dpd_map_row == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for DPD Map Row!");
    return FALSE;
  }

  epdt_tbl = (ftm_cal_data_det_type*)modem_mem_alloc(
              sizeof(ftm_cal_data_det_type), MODEM_MEM_CLIENT_RFA);

  if( epdt_tbl == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for EPDT Table!");
    modem_mem_free((void*)dpd_map_row, MODEM_MEM_CLIENT_RFA);
    return FALSE;
  }

  /* Get the HW IDs for Unity DPD from Data Module and fill all rows */
  unity_hwids_meas = (ftm_cal_data_unity_hwid_type*)modem_mem_alloc(
      sizeof(ftm_cal_data_unity_hwid_type) * data_param->data_param.load_calc_dpd_params.num_swp2_meas_pwrs, 
      MODEM_MEM_CLIENT_RFA);

  if( unity_hwids_meas == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for Unity HW Meas IDs!");
    modem_mem_free((void*)dpd_map_row, MODEM_MEM_CLIENT_RFA);
    modem_mem_free((void*)epdt_tbl, MODEM_MEM_CLIENT_RFA);
    return FALSE;
  }

  if( data_param->data_param.load_calc_dpd_params.num_swp2_calc_pwrs != 0 )
  {
    unity_hwids_calc = (ftm_cal_data_unity_hwid_type*)modem_mem_alloc(
        sizeof(ftm_cal_data_unity_hwid_type) * data_param->data_param.load_calc_dpd_params.num_swp2_calc_pwrs, 
        MODEM_MEM_CLIENT_RFA);

    if( unity_hwids_calc == NULL )
  {
      FTM_MSG( FTM_ERROR, "Failed to allocate memory for Unity HW Meas IDs!");
    modem_mem_free((void*)dpd_map_row, MODEM_MEM_CLIENT_RFA);
    modem_mem_free((void*)epdt_tbl, MODEM_MEM_CLIENT_RFA);
      modem_mem_free((void*)unity_hwids_meas, MODEM_MEM_CLIENT_RFA);
    return FALSE;
  }
  }

  /* Allocate required memory and check if memory is allocated */
  cal_result = (ftm_calibration_data_result_type*)modem_mem_alloc(
               sizeof(ftm_calibration_data_result_type), MODEM_MEM_CLIENT_RFA);

  if( cal_result == NULL )
  {
    FTM_MSG( FTM_ERROR, "Failed to allocate memory for DPD Construction Results!");
    return_val &= FALSE;
  }

  if( load_action == LOAD_CALC_DPD_ACTION_LOAD_UNITY )
  {
    cal_result->device = data_param->device;
    cal_result->load_unity_dpd_results.num_swp2_meas_pwrs = 0;
    cal_result->load_unity_dpd_results.num_swp2_calc_pwrs = 0;
    if (cal_data->cal_config.debug_mode_en)
    {
     FTM_MSG( FTM_MED, "====== LOAD CALC DPD ACTION LOAD UNITY ======");
    }

    /* Construct Nominal AMAM and AMPM Tables and load them and save the address in DPD Map */
    for( i = 0; i < MAX_NUM_DPD_VALS; i++ )
    {
      amam_val = (i << 10) + 0x400;
      if( i == (MAX_NUM_DPD_VALS-1) )
    {
        amam_val = 0xFFFF;
      }
      expAMAMtable[i] = amam_val;
      expAMPMtable[i] = 0;
    }

    if( return_val == TRUE )
    {
      /* Program QPOET into APT mode */
      /* Get the current Tx override */
      memset( &curr_tx_ovr, 0, sizeof(rfcommon_core_xpt_tx_override_type) );
      return_val &= ftm_common_xpt_get_curr_tx_override( device_ind, 
                                                         &curr_tx_ovr );
      ovr_params.device = device_ind;
      ovr_params.xpt_mode = RFCOMMON_MDSP_XPT_MODE_APT;
      ovr_params.iq_gain_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
      ovr_params.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
      ovr_params.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
      ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID; 
      ovr_params.rgi = curr_tx_ovr.rgi;
      ovr_params.smps_bias = 2000;
      ovr_params.pa_state = curr_tx_ovr.pa_state;
      ovr_params.pa_curr = curr_tx_ovr.pa_curr;
      ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
      ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;
     
      switch( ftm_rf_tech )
      {
      case FTM_RF_TECH_CDMA:
          #ifdef FEATURE_CDMA1X
          return_val &= ftm_1x_set_xpt_txagc_override( &ovr_params );
          #endif
          break;
      case FTM_RF_TECH_WCDMA:
          #ifdef FEATURE_WCDMA
          return_val &= ftm_wcdma_xpt_override_rf_settings( &ovr_params );
          #endif
          break;
      case FTM_RF_TECH_LTE:
          #ifdef FEATURE_LTE
          return_val &= rflte_ftm_xpt_override_rf_settings( &ovr_params );
          #endif
          break;
      case FTM_RF_TECH_TDSCDMA:
          #ifdef FEATURE_TDSCDMA
          return_val &= ftm_tdscdma_xpt_override_rf_settings( &ovr_params );
          #endif
          break;
      default:
          break;
      }
      if (cal_data->cal_config.debug_mode_en)
      {
       FTM_MSG_1( FTM_MED, "Return Value from Tx Override = %d", return_val);
      }
    }

    /* Load the DPD Tables to HW */
    if(return_val == TRUE)
    {
      /* Clear dpd tables of any previous data */
      return_val &= 
        rf_common_xpt_load_dpd_tbls(device_ind, 
                                    NULL, 
                                    0, 
                                    0, 
                                    &amam_bank_index, 
                                    &amam_bank_offset, 
                                    XPT_DPD_ACTION_INIT,
                                    load_dpd_xpt_type);
    }

    if( return_val == TRUE )
    {
      if (cal_data->cal_config.debug_mode_en)
      {
       FTM_MSG_1( FTM_MED, "Num Swp2 Meas Powers for Load Calc DPD = %d", 
                  data_param->data_param.load_calc_dpd_params.num_swp2_meas_pwrs );
      }
      for( lp_var = 0; lp_var < data_param->data_param.load_calc_dpd_params.num_swp2_meas_pwrs; lp_var++ )
      {
        curr_power = data_param->data_param.load_calc_dpd_params.swp2_meas_pwrs[lp_var];
        FTM_MSG_1( FTM_MED, "Load Nominal DPD for Meas Power = %d", curr_power);

        /* Point to AMAM Table */
        amam_ptr = (uint16*)(&(expAMAMtable[0]));
        if((return_val == TRUE) && (amam_ptr != NULL))
	  {
          /* Write AMAM to table */
          return_val &=
            rf_common_xpt_load_dpd_tbls(device_ind, 
                                        amam_ptr,
                                        (uint16)MAX_NUM_DPD_VALS, 
                                        DPD_TYPE_AMAM,
                                        &amam_bank_index,
                                        &amam_bank_offset, 
                                        XPT_DPD_ACTION_SAVE,
                                        load_dpd_xpt_type);
	  }
        
        /* Save AMAM Table HWIDs */
        unity_hwids_meas[lp_var].amam_addr.mem_bank_id = amam_bank_index;
        unity_hwids_meas[lp_var].amam_addr.start_offset = amam_bank_offset;
        unity_hwids_meas[lp_var].amam_addr.tbl_size = RFCOMMON_MDSP_DPD_SIZE_64;

        /* Point to AMPM Table */
        ampm_ptr = (uint16*)(&(expAMPMtable[0]));
        if((return_val == TRUE) && (ampm_ptr != NULL))
        {
          /* Write AMPM to table */
          return_val &=
            rf_common_xpt_load_dpd_tbls(device_ind, 
                                        ampm_ptr,
                                        (uint16)MAX_NUM_DPD_VALS, 
                                        DPD_TYPE_AMPM,
                                        &ampm_bank_index,
                                        &ampm_bank_offset, 
                                        XPT_DPD_ACTION_SAVE,
                                        load_dpd_xpt_type);
        }

        /* Save AMPM Table HWIDs */
        unity_hwids_meas[lp_var].ampm_addr.mem_bank_id = ampm_bank_index;
        unity_hwids_meas[lp_var].ampm_addr.start_offset = ampm_bank_offset;
        unity_hwids_meas[lp_var].ampm_addr.tbl_size = RFCOMMON_MDSP_DPD_SIZE_64;

        /* Append HWIDs to DPD Map rows because table is already written to HW */
	return_val &= ftm_cal_data_set_hwid_by_power(chan, 
                                                     curr_power, 
                                                     xpt_type, 
                                                     unity_hwids_meas[lp_var].amam_addr,
                                                     unity_hwids_meas[lp_var].ampm_addr,
                                                     unity_hwids_meas[lp_var].detrough_addr);

        if (cal_data->cal_config.debug_mode_en)
	{
         FTM_MSG_1( FTM_MED, "Updated Unity AMAM and AMPM HW IDs for Curr Power %d", curr_power);
	}

        /* if ET, get DPD Row, read the bias, scale the nominal detrough, load and save the 
        HWID to the row */
        if( xpt_type == 2 )
        {
          /* Get DPD Map Row for Curr Meas Power */
          return_val &= ftm_cal_data_get_row_data_by_power(chan, dpd_map_row, curr_power, xpt_type);
        
          if( return_val == TRUE )
          {
            /* Read Bias from the DPD Map Row */
            row_bias = dpd_map_row->curr_tx_override.papm_bias;
            if (cal_data->cal_config.debug_mode_en)
	    {
             FTM_MSG_1( FTM_MED, "Bias from Row = %d", row_bias);
            }
            /* Get the nominal detrough table from the Data Module */
            return_val &= ftm_cal_data_get_nom_epdt_table( epdt_tbl );

	    if (cal_data->cal_config.debug_mode_en)
	    {
             FTM_MSG_1( FTM_MED, "Detrough Table [0] = %d", epdt_tbl->table[0]);
            }
        
            if( return_val == TRUE )
            {
              /* Scale the detrough based on the bias */
              return_val &= ftm_calibration_data_scale_detrough( epdt_tbl, 
                                                                 epdt_size, 
                                                                 qpoet_min_volt_x1000, 
                                                                 qpoet_max_volt_x1000, 
                                                                 et_min_volt_x1000, 
                                                                 row_bias, 
                                                                 TRUE);

              /* Store for results back to PC */
              cal_result->load_unity_dpd_results.meas_epdt_row[lp_var].det_valid = TRUE;
              cal_result->load_unity_dpd_results.meas_epdt_row[lp_var].measured = TRUE;
              for( i = 0; i < MAX_NUM_DET_VALS; i++ )
              {
                cal_result->load_unity_dpd_results.meas_epdt_row[lp_var].epdt_val[i] = epdt_tbl->table[i];
              }
              cal_result->load_unity_dpd_results.num_swp2_meas_pwrs++;

              /* After scaling, decimate to 16 entry table */
	      if (cal_data->cal_config.debug_mode_en)
              {
               FTM_MSG( FTM_MED, "====== Decimate Detrough Table ====== ");
              }
              for( i = 0; i < MAX_RES_DPD_SIZE; i++ )
              {
                decimateEPDTTable[i] = epdt_tbl->table[i*8+7];
                //FTM_MSG_2( FTM_MED, "Decimated Detrough Table [%d] = %d", 
                //           i, decimateEPDTTable[i]);
              }

              /* Now expand to 128 using the expansion algorithm before loading */
	      if (cal_data->cal_config.debug_mode_en)
              {
               FTM_MSG( FTM_MED, "====== Expand Detrough Table ====== ");
	      }
              return_val &= rf_common_xpt_expand_dpd_data(DPD_TYPE_EPDT, 
                                                          16, 
                                                          &(decimateEPDTTable[0]), 
                                                          8, 
                                                          &expEPDTTable[0],
                                                          &dpd_scale_factor);

              //for( i = 0; i < MAX_NUM_DET_VALS; i++ )
              //{
              //  FTM_MSG_2( FTM_MED, "Expanded Detrough Table [%d] = %d", 
              //             i, expEPDTTable[i]);
              //}
        
              if( return_val == TRUE )
              {
                /* Load the Scaled Detrough Table to HW */
                /* Point to DET Table */
                det_ptr = (uint16*)(&(expEPDTTable[0]));
                /* Write EPDT to table */
                if( det_ptr != NULL )
                {
                  return_val &=
                    rf_common_xpt_load_dpd_tbls(device_ind, 
                                                 det_ptr,
                                                 epdt_size,
                                                 DPD_TYPE_EPDT,
                                                 &epdt_bank_index,
                                                &epdt_bank_offset, 
                                                XPT_DPD_ACTION_SAVE,
                                                load_dpd_xpt_type);
                  if (cal_data->cal_config.debug_mode_en)
		  {
                   FTM_MSG_1( FTM_MED, "DET Bank Index = %d", epdt_bank_index);
                   FTM_MSG_1( FTM_MED, "DET Bank Offset = %d", epdt_bank_offset);
		  }
        
                  if( return_val == TRUE )
                  {
                    /* Save the HWIDs to the DPD Map Row */
                    unity_hwids_meas[lp_var].detrough_addr.mem_bank_id = epdt_bank_index;
                    unity_hwids_meas[lp_var].detrough_addr.start_offset = epdt_bank_offset;
                    unity_hwids_meas[lp_var].detrough_addr.tbl_size = RFCOMMON_MDSP_DPD_SIZE_128;

                    /* Append HWIDs to DPD Map rows because table is already written to HW */
                    return_val &= ftm_cal_data_set_hwid_by_power(chan, 
                                                                 curr_power, 
                                                                 xpt_type, 
                                                                 unity_hwids_meas[lp_var].amam_addr,
                                                                 unity_hwids_meas[lp_var].ampm_addr,
                                                                 unity_hwids_meas[lp_var].detrough_addr);

                    if (cal_data->cal_config.debug_mode_en)
		    {
                     FTM_MSG_1( FTM_MED, "Updated EPDT HW IDs for Curr Power %d", curr_power);
                    }
                  }
                }
                else
                {
                  FTM_MSG( FTM_ERROR, "Detrough Pointer is NULL");
                  return_val &= FALSE;
                }
              }
              else
              {
                FTM_MSG( FTM_ERROR, "Failed to Scale Detrough Table");
                return_val &= FALSE;
              }
            }
            else
            {
              FTM_MSG( FTM_ERROR, "Failed to get Nominal Detrough Table from Data Module");
              return_val &= FALSE;
            }
          }
          else
          {
            FTM_MSG( FTM_ERROR, "Failed to get Row data by Power");
            return_val &= FALSE;
          }
        }
      }

        if(return_val == TRUE)
        {
          /* Commit DPD tables to HW */
          return_val &= 
            rf_common_xpt_load_dpd_tbls(device_ind, 
                                        NULL, 
                                        0, 
                                        0, 
                                        &amam_bank_index, 
                                        &amam_bank_offset, 
                                        XPT_DPD_ACTION_COMMIT,
                                        load_dpd_xpt_type);
        }

    }
    else
    {
      FTM_MSG( FTM_ERROR, "Failed to get Unity HW IDs from Data Module!");
      return_val &= FALSE;
    }
  }

  if( load_action == LOAD_CALC_DPD_ACTION_LOAD_MEAS_ISC || load_action == LOAD_CALC_DPD_ACTION_LOAD_MEAS_SC )
  {
    if (cal_data->cal_config.debug_mode_en)
    {
     FTM_MSG( FTM_MED, "====== LOAD CALC DPD ACTION LOAD MEAS ======");
    }

    /* Program QPOET into APT mode */
    /* Get the current Tx override */
    memset( &curr_tx_ovr, 0, sizeof(rfcommon_core_xpt_tx_override_type) );
    return_val &= ftm_common_xpt_get_curr_tx_override( device_ind, 
                                                       &curr_tx_ovr );
    ovr_params.device = device_ind;
    ovr_params.xpt_mode = RFCOMMON_MDSP_XPT_MODE_APT;
    ovr_params.iq_gain_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
    ovr_params.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
    ovr_params.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
    ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID; 
    ovr_params.rgi = curr_tx_ovr.rgi;
    ovr_params.smps_bias = 2000;
    ovr_params.pa_state = curr_tx_ovr.pa_state;
    ovr_params.pa_curr = curr_tx_ovr.pa_curr;
    ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
    ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;

    switch( ftm_rf_tech )
    {
    case FTM_RF_TECH_CDMA:
        #ifdef FEATURE_CDMA1X
        return_val &= ftm_1x_set_xpt_txagc_override( &ovr_params );
        #endif
        break;
    case FTM_RF_TECH_WCDMA:
        #ifdef FEATURE_WCDMA
        return_val &= ftm_wcdma_xpt_override_rf_settings( &ovr_params );
        #endif
        break;
    case FTM_RF_TECH_LTE:
        #ifdef FEATURE_LTE
        return_val &= rflte_ftm_xpt_override_rf_settings( &ovr_params );
        #endif
        break;
    case FTM_RF_TECH_TDSCDMA:
        #ifdef FEATURE_TDSCDMA
        return_val &= ftm_tdscdma_xpt_override_rf_settings( &ovr_params );
        #endif
        break;
    default:
        break;
    }

    /* Do Init only if requested */
    if( load_action == LOAD_CALC_DPD_ACTION_LOAD_MEAS_ISC )
    {
      if(return_val == TRUE)
      {
        /* Clear dpd tables of any previous data */
        return_val &= 
          rf_common_xpt_load_dpd_tbls(device_ind, 
                                      NULL, 
                                      0, 
                                      0, 
                                      &amam_bank_index, 
                                      &amam_bank_offset, 
                                      XPT_DPD_ACTION_INIT,
                                      load_dpd_xpt_type);
      }
    }
  
    /* Update HW IDs for Meas Power Rows */
    for( lp_var = 0; lp_var < data_param->data_param.load_calc_dpd_params.num_swp2_meas_pwrs; lp_var++ )
    {
      curr_power = data_param->data_param.load_calc_dpd_params.swp2_meas_pwrs[lp_var];
      //FTM_MSG_1( FTM_MED, "Get Row for Meas Power = %d", curr_power);
  
      /* Get DPD Map Row for Curr Meas Power */
      return_val &= ftm_cal_data_get_row_data_by_power(chan, dpd_map_row, curr_power, xpt_type);
      //FTM_MSG_1( FTM_MED, "Meas Read DPD Map Row Index = %d", dpd_map_row->row_idx);

      if( return_val == FALSE )
      {
        FTM_MSG_1( FTM_ERROR, "Cant find Meas Power %d in Load Meas DPD "
                              "Processing", curr_power);
        break;
      }
      else
      {
        /* Get the DPD data */
        dpd_result = (fw_dpd_cal_result_t*)(dpd_map_row->dpd);
        //FTM_MSG_1( FTM_MED, "Meas DPD Result AMAM[0] = %d", dpd_result->am_dpd[0]);
        //FTM_MSG_1( FTM_MED, "Meas DPD Result AMPM[0] = %d", dpd_result->pm_dpd[0]);

        /* Expand to write to HW */
        return_val &= rf_common_xpt_expand_dpd_data(DPD_TYPE_AMAM, 
                                                    16, 
                                                    &(dpd_result->am_dpd[0]), 
                                                    4, 
                                                    &expAMAMtable[0],
                                                    &dpd_scale_factor);

        //FTM_MSG( FTM_MED, "====== Expanded AMAM Table ======");
        //for( i = 0; i < MAX_NUM_DPD_VALS; i++ )
        //{
        //  FTM_MSG_2( FTM_MED, "Expanded AMAM Table [%d] = %d", i, expAMAMtable[i] );
        //}

        return_val &= rf_common_xpt_expand_dpd_data(DPD_TYPE_AMPM, 
                                                    16, 
                                                    (uint16*)&(dpd_result->pm_dpd[0]), 
                                                    4, 
                                                    (uint16*)&expAMPMtable[0],
                                                    &dpd_scale_factor);

        //FTM_MSG( FTM_MED, "====== Expanded AMPM Table ======");
        //for( i = 0; i < MAX_NUM_DPD_VALS; i++ )
        //{
        //  FTM_MSG_2( FTM_MED, "Expanded AMPM Table [%d] = %d", i, expAMPMtable[i] );
        //}

        /* Point to AMAM Table */
        amam_ptr = (uint16*)(&(expAMAMtable[0]));
        if((return_val == TRUE) && (amam_ptr != NULL))
        {
          /* Write AMAM to table */
          return_val &=
            rf_common_xpt_load_dpd_tbls(device_ind, 
                                         amam_ptr,
                                        (uint16)MAX_NUM_DPD_VALS, 
                                         DPD_TYPE_AMAM,
                                         &amam_bank_index,
                                        &amam_bank_offset, 
                                        XPT_DPD_ACTION_SAVE,
                                        load_dpd_xpt_type);
        }

        /* Point to AMPM Table */
        ampm_ptr = (uint16*)(&(expAMPMtable[0]));
        if((return_val == TRUE) && (ampm_ptr != NULL))
        {
          /* Write AMPM to table */
          return_val &=
            rf_common_xpt_load_dpd_tbls(device_ind, 
                                         ampm_ptr,
                                        (uint16)MAX_NUM_DPD_VALS, 
                                         DPD_TYPE_AMPM,
                                         &ampm_bank_index,
                                        &ampm_bank_offset, 
                                        XPT_DPD_ACTION_SAVE,
                                        load_dpd_xpt_type);
        }

        if( xpt_type == 2 && return_val == TRUE )
        {
          /* Read Bias from the DPD Map Row */
          row_bias = dpd_map_row->curr_tx_override.papm_bias;
          //FTM_MSG_1( FTM_MED, "Bias from Row = %d", row_bias);
      
          /* Get the nominal detrough table from the Data Module */
          return_val &= ftm_cal_data_get_nom_epdt_table( epdt_tbl );
      
          if( return_val == TRUE )
          {
            /* Scale the detrough based on the bias */
            return_val &= ftm_calibration_data_scale_detrough( epdt_tbl, 
                                                               epdt_size, 
                                                               qpoet_min_volt_x1000, 
                                                               qpoet_max_volt_x1000, 
                                                               et_min_volt_x1000, 
                                                               row_bias, 
                                                               TRUE);

            /* After scaling, decimate to 16 entry table */
            //FTM_MSG( FTM_MED, "====== Decimate Detrough Table ====== ");
            for( i = 0; i < MAX_RES_DPD_SIZE; i++ )
            {
              decimateEPDTTable[i] = epdt_tbl->table[i*8+7];
              //FTM_MSG_2( FTM_MED, "Decimated Detrough Table [%d] = %d", 
              //           i, decimateEPDTTable[i]);
            }

            /* Now expand to 128 using the expansion algorithm before loading */
            //FTM_MSG( FTM_MED, "====== Expand Detrough Table ====== ");
            return_val &= rf_common_xpt_expand_dpd_data(DPD_TYPE_EPDT, 
                                                        16, 
                                                        &(decimateEPDTTable[0]), 
                                                        8, 
                                                        &expEPDTTable[0],
                                                        &dpd_scale_factor);

           // for( i = 0; i < MAX_NUM_DET_VALS; i++ )
            //{
            //  FTM_MSG_2( FTM_MED, "Expanded Detrough Table [%d] = %d", 
            //             i, expEPDTTable[i]);
            //}


            /* Point to DET Table */
            det_ptr = (uint16*)(&(expEPDTTable[0]));
            /* Write EPDT to table */
            if( det_ptr != NULL && return_val == TRUE )
            {
              return_val &=
                rf_common_xpt_load_dpd_tbls(device_ind, 
                                             det_ptr,
                                            (uint16)MAX_NUM_DET_VALS, 
                                             DPD_TYPE_EPDT,
                                             &epdt_bank_index,
                                            &epdt_bank_offset, 
                                            XPT_DPD_ACTION_SAVE,
                                            load_dpd_xpt_type);
            }
          }
        }

        if( return_val == TRUE )
        {
          /* Save HW IDs in DPD Map for the corresponding row */
          unity_hwids_meas[lp_var].amam_addr.mem_bank_id = amam_bank_index;
          unity_hwids_meas[lp_var].amam_addr.start_offset = amam_bank_offset;
          unity_hwids_meas[lp_var].amam_addr.tbl_size = RFCOMMON_MDSP_DPD_SIZE_64;
          unity_hwids_meas[lp_var].ampm_addr.mem_bank_id = ampm_bank_index;
          unity_hwids_meas[lp_var].ampm_addr.start_offset = ampm_bank_offset;
          unity_hwids_meas[lp_var].ampm_addr.tbl_size = RFCOMMON_MDSP_DPD_SIZE_64;

          //FTM_MSG_1( FTM_MED, "AMAM Bank Index = %d", unity_hwids_meas[lp_var].amam_addr.mem_bank_id);
          //FTM_MSG_1( FTM_MED, "AMAM Bank Offset = %d", unity_hwids_meas[lp_var].amam_addr.start_offset);
          //FTM_MSG_1( FTM_MED, "AMAM Bank Size = %d", unity_hwids_meas[lp_var].amam_addr.tbl_size);
          //FTM_MSG_1( FTM_MED, "AMPM Bank Index = %d", unity_hwids_meas[lp_var].ampm_addr.mem_bank_id);
          //FTM_MSG_1( FTM_MED, "AMPM Bank Offset = %d", unity_hwids_meas[lp_var].ampm_addr.start_offset);
          //FTM_MSG_1( FTM_MED, "AMPM Bank Size = %d", unity_hwids_meas[lp_var].ampm_addr.tbl_size);

          if( xpt_type == 2 )
          {
            unity_hwids_meas[lp_var].detrough_addr.mem_bank_id = epdt_bank_index;
            unity_hwids_meas[lp_var].detrough_addr.start_offset = epdt_bank_offset;
            unity_hwids_meas[lp_var].detrough_addr.tbl_size = RFCOMMON_MDSP_DPD_SIZE_128;

            //FTM_MSG_1( FTM_MED, "DET Bank Index = %d", unity_hwids_meas[lp_var].detrough_addr.mem_bank_id);
            //FTM_MSG_1( FTM_MED, "DET Bank Offset = %d", unity_hwids_meas[lp_var].detrough_addr.start_offset);
            //FTM_MSG_1( FTM_MED, "DET Bank Size = %d", unity_hwids_meas[lp_var].detrough_addr.tbl_size);
          }

          /* Append HWIDs to DPD Map rows because table is already written to HW */
          return_val &= ftm_cal_data_set_hwid_by_power(chan, 
                                                       curr_power, 
                                                       xpt_type, 
                                                       unity_hwids_meas[lp_var].amam_addr,
                                                       unity_hwids_meas[lp_var].ampm_addr,
                                                       unity_hwids_meas[lp_var].detrough_addr);
        }
      }
    }

    /* Calc Power Rows */
    for( lp_var = 0; lp_var < data_param->data_param.load_calc_dpd_params.num_swp2_calc_pwrs; lp_var++ )
    {
      curr_power = data_param->data_param.load_calc_dpd_params.swp2_calc_pwrs[lp_var];
      if (cal_data->cal_config.debug_mode_en)
      {
       FTM_MSG_1( FTM_MED, "Get Row for Calc Power = %d", curr_power);
      }

      /* DPD data for Calculated Powers will be same as corresponding higher measured power */
      /* If Calc Power is greater than Max Meas Power, then use Max Meas Power DPD */
      if( curr_power > 
          data_param->data_param.load_calc_dpd_params.swp2_meas_pwrs[data_param->data_param.load_calc_dpd_params.num_swp2_meas_pwrs - 1] )
      {
        next_high_meas_power = data_param->data_param.load_calc_dpd_params.swp2_meas_pwrs[data_param->data_param.load_calc_dpd_params.num_swp2_meas_pwrs - 1];
        if (cal_data->cal_config.debug_mode_en)
	{
         FTM_MSG_2( FTM_MED, "Next Higher Meas Power for Calc Power %d = %d", 
                        curr_power, 
                        next_high_meas_power);
        }
      }
      /* Else find the next higher meas power */
      else
      {
        for( meas_pow = 0; meas_pow < data_param->data_param.load_calc_dpd_params.num_swp2_meas_pwrs; meas_pow++ )
        {
          if( curr_power < data_param->data_param.load_calc_dpd_params.swp2_meas_pwrs[meas_pow] )
          {
            next_high_meas_power = data_param->data_param.load_calc_dpd_params.swp2_meas_pwrs[meas_pow];
            if (cal_data->cal_config.debug_mode_en)
	    {
             FTM_MSG_2( FTM_MED, "Next Higher Meas Power for Calc Power %d = %d", 
                        curr_power, 
                        next_high_meas_power);
	    }
            break;
          }
        }
      }
  
      /* Get DPD Map Row for Curr Calc Power */
      return_val &= ftm_cal_data_get_row_data_by_power(chan, dpd_map_row, next_high_meas_power, xpt_type);
      //FTM_MSG_1( FTM_MED, "Calc Read DPD Map Row Index = %d", dpd_map_row->row_idx);

      if( return_val == FALSE )
      {
        FTM_MSG_1( FTM_ERROR, "Cant find Calc Power %d in Load Meas DPD "
                              "Processing", curr_power);
        break;
      }
      else
      {
        /* Get the DPD data */
        dpd_result = (fw_dpd_cal_result_t*)(dpd_map_row->dpd);
        //FTM_MSG_1( FTM_MED, "Meas DPD Result AMAM[0] = %d", dpd_result->am_dpd[0]);
        //FTM_MSG_1( FTM_MED, "Meas DPD Result AMPM[0] = %d", dpd_result->pm_dpd[0]);

        /* Expand to write to HW */
        return_val &= rf_common_xpt_expand_dpd_data(DPD_TYPE_AMAM, 
                                                    16, 
                                                    &(dpd_result->am_dpd[0]), 
                                                    4, 
                                                    &expAMAMtable[0],
                                                    &dpd_scale_factor);

        //FTM_MSG( FTM_MED, "====== Expanded AMAM Table ======");
        //for( i = 0; i < MAX_NUM_DPD_VALS; i++ )
        //{
        //  FTM_MSG_2( FTM_MED, "Expanded AMAM Table [%d] = %d", i, expAMAMtable[i] );
        //}

        return_val &= rf_common_xpt_expand_dpd_data(DPD_TYPE_AMPM, 
                                                    16, 
                                                    (uint16*)&(dpd_result->pm_dpd[0]), 
                                                    4, 
                                                    (uint16*)&expAMPMtable[0],
                                                    &dpd_scale_factor);

        //FTM_MSG( FTM_MED, "====== Expanded AMPM Table ======");
        //for( i = 0; i < MAX_NUM_DPD_VALS; i++ )
        //{
        //  FTM_MSG_2( FTM_MED, "Expanded AMPM Table [%d] = %d", i, expAMPMtable[i] );
        //}

        /* Program QPOET into APT mode */
        /* Get the current Tx override */
        memset( &curr_tx_ovr, 0, sizeof(rfcommon_core_xpt_tx_override_type) );
        return_val &= ftm_common_xpt_get_curr_tx_override( device_ind, 
                                                           &curr_tx_ovr );
        ovr_params.device = device_ind;
        ovr_params.xpt_mode = RFCOMMON_MDSP_XPT_MODE_APT;
        ovr_params.iq_gain_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
        ovr_params.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
        ovr_params.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
        ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID; 
        ovr_params.rgi = curr_tx_ovr.rgi;
        ovr_params.smps_bias = 2000;
        ovr_params.pa_state = curr_tx_ovr.pa_state;
        ovr_params.pa_curr = curr_tx_ovr.pa_curr;
        ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
        ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;

        switch( ftm_rf_tech )
        {
        case FTM_RF_TECH_CDMA:
            #ifdef FEATURE_CDMA1X
            return_val &= ftm_1x_set_xpt_txagc_override( &ovr_params );
            #endif
            break;
        case FTM_RF_TECH_WCDMA:
            #ifdef FEATURE_WCDMA
            return_val &= ftm_wcdma_xpt_override_rf_settings( &ovr_params );
            #endif
            break;
        case FTM_RF_TECH_LTE:
            #ifdef FEATURE_LTE
            return_val &= rflte_ftm_xpt_override_rf_settings( &ovr_params );
            #endif
            break;
        case FTM_RF_TECH_TDSCDMA:
            #ifdef FEATURE_TDSCDMA
            return_val &= ftm_tdscdma_xpt_override_rf_settings( &ovr_params );
            #endif
            break;
        default:
            break;
        }

        /* Point to AMAM Table */
        amam_ptr = (uint16*)(&(expAMAMtable[0]));
        if((return_val == TRUE) && (amam_ptr != NULL))
        {
          /* Write AMAM to table */
          return_val &=
            rf_common_xpt_load_dpd_tbls(device_ind, 
                                         amam_ptr,
                                        (uint16)MAX_NUM_DPD_VALS, 
                                         DPD_TYPE_AMAM,
                                         &amam_bank_index,
                                        &amam_bank_offset, 
                                        XPT_DPD_ACTION_SAVE,
                                        load_dpd_xpt_type);
        }

        /* Point to AMPM Table */
        ampm_ptr = (uint16*)(&(expAMPMtable[0]));
        if((return_val == TRUE) && (ampm_ptr != NULL))
        {
          /* Write AMPM to table */
          return_val &=
            rf_common_xpt_load_dpd_tbls(device_ind, 
                                         ampm_ptr,
                                        (uint16)MAX_NUM_DPD_VALS, 
                                         DPD_TYPE_AMPM,
                                         &ampm_bank_index,
                                        &ampm_bank_offset, 
                                        XPT_DPD_ACTION_SAVE,
                                        load_dpd_xpt_type);
        }
      }

      if( return_val == TRUE )
      {
        /* Save HW IDs in DPD Map for the corresponding row */
        unity_hwids_calc[lp_var].amam_addr.mem_bank_id = amam_bank_index;
        unity_hwids_calc[lp_var].amam_addr.start_offset = amam_bank_offset;
        unity_hwids_calc[lp_var].amam_addr.tbl_size = RFCOMMON_MDSP_DPD_SIZE_64;
        unity_hwids_calc[lp_var].ampm_addr.mem_bank_id = ampm_bank_index;
        unity_hwids_calc[lp_var].ampm_addr.start_offset = ampm_bank_offset;
        unity_hwids_calc[lp_var].ampm_addr.tbl_size = RFCOMMON_MDSP_DPD_SIZE_64;

        //FTM_MSG_1( FTM_MED, "AMAM Bank Index = %d", unity_hwids_calc[lp_var].amam_addr.mem_bank_id);
        //FTM_MSG_1( FTM_MED, "AMAM Bank Offset = %d", unity_hwids_calc[lp_var].amam_addr.start_offset);
        //FTM_MSG_1( FTM_MED, "AMAM Bank Size = %d", unity_hwids_calc[lp_var].amam_addr.tbl_size);
        //FTM_MSG_1( FTM_MED, "AMPM Bank Index = %d", unity_hwids_calc[lp_var].ampm_addr.mem_bank_id);
        //FTM_MSG_1( FTM_MED, "AMPM Bank Offset = %d", unity_hwids_calc[lp_var].ampm_addr.start_offset);
        //FTM_MSG_1( FTM_MED, "AMPM Bank Size = %d", unity_hwids_calc[lp_var].ampm_addr.tbl_size);

        /* Append HWIDs to DPD Map rows because table is already written to HW */
        return_val &= ftm_cal_data_set_hwid_by_power(chan, 
                                                     curr_power, 
                                                     xpt_type, 
                                                     unity_hwids_calc[lp_var].amam_addr,
                                                     unity_hwids_calc[lp_var].ampm_addr,
                                                     unity_hwids_calc[lp_var].detrough_addr);
      }
    }

    if(return_val == TRUE)
    {
      /* Commit DPD tables to HW */
      return_val &= 
        rf_common_xpt_load_dpd_tbls(device_ind, 
                                    NULL, 
                                    0, 
                                    0, 
                                    &amam_bank_index, 
                                    &amam_bank_offset, 
                                    XPT_DPD_ACTION_COMMIT,
                                    load_dpd_xpt_type);
    }
  }

  /* Allocate free memory */
  if( dpd_map_row != NULL )
  {
    modem_mem_free((void*)dpd_map_row, MODEM_MEM_CLIENT_RFA);
  }
  if( epdt_tbl != NULL )
  {
    modem_mem_free((void*)epdt_tbl, MODEM_MEM_CLIENT_RFA);
  }
  if( unity_hwids_meas != NULL )
  {
    modem_mem_free((void*)unity_hwids_meas, MODEM_MEM_CLIENT_RFA);
  }
  if( unity_hwids_calc != NULL )
  {
    modem_mem_free((void*)unity_hwids_calc, MODEM_MEM_CLIENT_RFA);
  }

  if( return_val == TRUE )
  {
    /* Now populate the result buffer from cal_result */
    /* Initialize the free size */ 
    free_size = oc_disp->res_buff_info.buff_sz_max;

    /*==========================================================================*/
    /* Populate the results buffer */
    /*==========================================================================*/

    /* Initialize the write pointer */
    res_write_ptr = oc_disp->res_buff_info.buff_ptr;

    /*==========================================================================*/
    /* Populate Results Type */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(res_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA LOAD CALC DPD.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */
	
	  /*==========================================================================*/
    /* Populate version Type */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), &(version), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA LOAD CALC DPD.{0x%x}] "
                 "ResultPush : Version - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 sizeof(uint8),
                 free_size );
    } /* if ! ( free_size >= sizeof(uint8) ) */
  
    /*==========================================================================*/
    /* Populate Device */
    /*==========================================================================*/

    if ( free_size >= sizeof(ftm_cal_data_device_type) )
    {
      memscpy( res_write_ptr, sizeof(ftm_cal_data_device_type), 
               &(data_param->device), sizeof(ftm_cal_data_device_type));
	  UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
	  				     sizeof(ftm_cal_data_device_type) );
	  free_size -= sizeof(ftm_cal_data_device_type);
    } /* if ( free_size >= sizeof(ftm_cal_data_device_type) ) */
    else
    {
	  FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA LOAD CALC DPD.{0x%x}] "
		         "ResultPush : dMask - ReqSize %d FreeSize %d",
	  		     cal_data->seg_data.cal_instance,
  			     FTM_CAL_OPCODE_DATA,
  			     sizeof(ftm_cal_data_device_type),
  			     free_size );
    } /* if ! ( free_size >= sizeof(ftm_cal_data_device_type) ) */

    /*==========================================================================*/
    /* Populate Results Action */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8), 
               &(res_action_type), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA LOAD CALC DPD.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_LOAD_CALC_DPD,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate Load DPD Action */
    /*==========================================================================*/

    if ( free_size >= sizeof(uint8) )
    {
      memscpy( res_write_ptr, sizeof(uint8),
               &(load_action), sizeof(uint8));
      UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
      free_size -= sizeof(uint8);
    } /* if ( free_size >= sizeof(uint8) ) */
    else
    {
      FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA LOAD CALC DPD.{0x%x}] "
                 "ResultPush : ResType - ReqSize %d FreeSize %d",
                 cal_data->seg_data.cal_instance,
                 DATA_RES_ACTION_LOAD_CALC_DPD,
                 sizeof(uint8),
                 free_size );
    } /* if ( free_size >= sizeof(uint8) ) */

    /*==========================================================================*/
    /* Populate Load DPD Num Meas Powers if Load Action is Unity DPD */
    /*==========================================================================*/

    if( load_action == LOAD_CALC_DPD_ACTION_LOAD_UNITY )
    {
      if ( free_size >= sizeof(uint8) )
      {
        memscpy( res_write_ptr, sizeof(uint8),
                 &(cal_result->load_unity_dpd_results.num_swp2_meas_pwrs), sizeof(uint8));
        UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                           sizeof(uint8) );
        free_size -= sizeof(uint8);
      } /* if ( free_size >= sizeof(uint8) ) */
      else
      {
        FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA LOAD CALC DPD.{0x%x}] "
                   "ResultPush : dMask - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_DATA,
                   sizeof(uint8),
                   free_size );
      } /* if ! ( free_size >= sizeof(uint8) ) */
    }

    /*==========================================================================*/
    /* Populate Load DPD Num Calc Powers if Load Action is Unity DPD */
    /*==========================================================================*/

    if( load_action == LOAD_CALC_DPD_ACTION_LOAD_UNITY )
    {
      if ( free_size >= sizeof(uint8) )
      {
        memscpy( res_write_ptr, sizeof(uint8), 
                 &(cal_result->load_unity_dpd_results.num_swp2_calc_pwrs), sizeof(uint8));
        UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                           sizeof(uint8) );
        free_size -= sizeof(uint8);
      } /* if ( free_size >= sizeof(uint8) ) */
      else
      {
        FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA LOAD CALC DPD.{0x%x}] "
                   "ResultPush : dMask - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_DATA,
                   sizeof(uint8),
                   free_size );
      } /* if ! ( free_size >= sizeof(uint8) ) */
    }

    /*==========================================================================*/
    /* Populate Load Unity DPD Detrough Result Row */
    /*==========================================================================*/

    if( load_action == LOAD_CALC_DPD_ACTION_LOAD_UNITY )
    {
      for( i = 0; i < cal_result->load_unity_dpd_results.num_swp2_meas_pwrs; i++ )
      {
        if ( free_size >= sizeof(ftm_calibration_data_epdt_row_type) )
        {
          memscpy( res_write_ptr, sizeof(ftm_calibration_data_epdt_row_type),
                   &(cal_result->load_unity_dpd_results.meas_epdt_row[i]), 
                  sizeof(ftm_calibration_data_epdt_row_type));
          UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                             sizeof(ftm_calibration_data_epdt_row_type) );
          free_size -= sizeof(ftm_calibration_data_epdt_row_type);
        } /* if ( free_size >= sizeof(ftm_calibration_data_epdt_row_type) ) */
        else
        {
          FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA LOAD CALC DPD.{0x%x}] "
                     "ResultPush : dMask - ReqSize %d FreeSize %d",
                     cal_data->seg_data.cal_instance,
                     FTM_CAL_OPCODE_DATA,
                     sizeof(ftm_calibration_data_epdt_row_type),
                     free_size );
        } /* if ! ( free_size >= sizeof(ftm_calibration_data_epdt_row_type) ) */
      }
    }

    /*==========================================================================*/
    /* Populate Load Unity DPD Detrough Result Row */
    /*==========================================================================*/

    if( load_action == LOAD_CALC_DPD_ACTION_LOAD_UNITY )
    {
      for( i = 0; i < cal_result->load_unity_dpd_results.num_swp2_calc_pwrs; i++ )
      {
        if ( free_size >= sizeof(ftm_calibration_data_epdt_row_type) )
        {
          memscpy( res_write_ptr, sizeof(ftm_calibration_data_epdt_row_type), 
                   &(cal_result->load_unity_dpd_results.calc_epdt_row[i]), 
                  sizeof(ftm_calibration_data_epdt_row_type));
          UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                             sizeof(ftm_calibration_data_epdt_row_type) );
          free_size -= sizeof(ftm_calibration_data_epdt_row_type);
        } /* if ( free_size >= sizeof(ftm_calibration_data_epdt_row_type) ) */
        else
        {
          FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][DATA LOAD CALC DPD.{0x%x}] "
                     "ResultPush : dMask - ReqSize %d FreeSize %d",
                     cal_data->seg_data.cal_instance,
                     FTM_CAL_OPCODE_DATA,
                     sizeof(ftm_calibration_data_epdt_row_type),
                     free_size );
        } /* if ! ( free_size >= sizeof(ftm_calibration_data_epdt_row_type) ) */
      }
    }

    /*==========================================================================*/
    /* Update Result Written Size */
    /*==========================================================================*/
    oc_disp->res_buff_info.buff_sz = written_size;

    if ( cal_data->cal_config.debug_mode_en )
    {
      FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "ResultPush : %d bytes",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA,
                 written_size );
    }
  }

  if( cal_result != NULL )
  {
    modem_mem_free((void*)cal_result, MODEM_MEM_CLIENT_RFA);
  }

  if (cal_data->cal_config.debug_mode_en)
  {
   FTM_MSG( FTM_MED, "Done with Load Calc DPD Action!");
  }

  return return_val;

} /* ftm_calibration_data_load_calc_dpd */
/*! @} */

/*============================================================================*/
/*!
  @name Data OpCode handling

  @brief
  This section contains functions for Data opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Data OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Data OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_data_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_data_opcode_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.data);

  reg_data.unpack_fp = ftm_calibration_data_unpack;
  reg_data.process_fp = ftm_calibration_data_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;

  if ( cfg_data->res_buff == NULL )
  {
    /* Allocate memory for results buffer to store results for the current
    segment */
    cfg_data->res_buff = modem_mem_alloc( FTM_CAL_DATA_RES_BUFF_SIZE_MAX,
                                          MODEM_MEM_CLIENT_RFA );

    /* Check if the malloc was successfull */
    if ( cfg_data->res_buff != NULL )
    {
      reg_data.res_buff_info.buff_ptr = cfg_data->res_buff;
      reg_data.res_buff_info.buff_sz_max = FTM_CAL_DATA_RES_BUFF_SIZE_MAX;
      reg_data.res_buff_info.buff_sz = 0;
    } /* if ( cfg_data->res_buff != NULL ) */
    else
    {
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][DATA.{0x%x}] "
                 "Init : ResBuff malloc failed",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_DATA );
      reg_data.res_buff_info.buff_ptr = 0;
      reg_data.res_buff_info.buff_sz_max = 0;
      reg_data.res_buff_info.buff_sz = 0;
      ret_val = FALSE;
    } /* if !( cfg_data->res_buff != NULL ) */
  } /* if ( cfg_data->res_buff == NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
               "Init : ResBuff not NULL",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_DATA );
    ret_val = FALSE;
  } /* if ! ( cfg_data->res_buff == NULL ) */

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_DATA,
                                             reg_data );

  return ret_val;

} /* ftm_calibration_data_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Data OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Data OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_data_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_data_opcode_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.data);

  if ( cfg_data->res_buff != NULL )
  {
    /* De-allocate memory which was allocated for results */
    modem_mem_free( cfg_data->res_buff, MODEM_MEM_CLIENT_RFA );
    cfg_data->res_buff = NULL;
  } /* if ( cfg_data->res_buff != NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][DATA.{0x%x}] "
               "Deinit : Null ResBuff",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_DATA );
    ret_val = FALSE;
  } /* if ! ( cfg_data->res_buff != NULL ) */

  return ret_val;
} /* ftm_calibration_data_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Data OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Data OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_data_unpack
( 
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  uint16 payload_size = 0; /* Payload Size */
  uint8 *payload_ptr = NULL; /* Paylaod pointer */
  
  uint8 num_rgi_pwr_pairs = 0;
  uint8 num_amam = 0;
  uint8 num_ampm = 0;
  uint8 num_det = 0;
  
  uint8 num_swp1_meas_pwrs = 0;
  uint8 num_swp2_meas_pwrs = 0;
  uint8 num_swp2_calc_pwrs = 0;
  
  /* Mline proc params */ 
  uint8 num_swp2_ept_meas_pwrs = 0;
  uint8 num_swp2_ept_calc_pwrs = 0;
  uint8 num_swp2_et_meas_pwrs = 0;
  uint8 num_swp2_et_calc_pwrs = 0;
    
  /* MLine Result Action params*/
  uint16 results_ept_curr_action;
  uint16 results_et_curr_action;
  uint8 results_ept_num_selec_swp_target_pwrs;
  uint8 results_et_num_selec_swp_target_pwrs;
  uint8 results_ept_num_compression_points;
  uint8 results_et_num_compression_points;
    
  uint8 lp_var = 0;
  
  ftm_calibration_data_action_type curr_action;
  ftm_cal_data_parameter_type *data_unpack = NULL;
  ftm_cal_data_opcode_data_type *cfg_data;
  
  /* NULL Pointer check */
  if ( ( instr_payload == NULL ) || ( cal_data == NULL ) )
  {
    FTM_MSG_1( FTM_HIGH, "[calv3][Segment X][DATA.{0x%x}] "
               "Unpack : Null Argument",
               FTM_CAL_OPCODE_DATA );
    return 0;
  } /* if ( instr_payload == NULL ) */
  
  cfg_data = &(cal_data->seg_data.oc_payload.data); 
  
  /* Get pointer to memory to hold Data Opcode unpacked data */
  data_unpack = &(cfg_data->up_param);
  
  /* Initialize the payload pointer */
  payload_ptr = (uint8*)instr_payload;
  payload_size = 0;
  
  /* Reset all data */
  memset( data_unpack, 0x0, sizeof(ftm_cal_data_parameter_type) );
  
  /* Update the payload size and Payload pointer with version*/
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
  /* Update the payload size and Payload pointer with Debug Flag */
  memscpy( &(data_unpack->debug_mode_en), sizeof(uint8), 
           payload_ptr, sizeof(uint8) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
  /* Update the payload size and Payload pointer with Flag */
  memscpy( &(data_unpack->flag), sizeof(uint16), 
           payload_ptr, sizeof(uint16) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );
  
  /* Update the payload size and Payload pointer with Technology */
  memscpy( &(data_unpack->technology), sizeof(uint8),
           payload_ptr, sizeof(uint8) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

  /* Update the payload size and Payload pointer with Ref Channel */
  memscpy( &(data_unpack->refChan), sizeof(uint32),
          payload_ptr, sizeof(uint32) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint32) );
  
  /* Update the payload size and Payload pointer with device mask */
  memscpy( &(data_unpack->device), sizeof(ftm_cal_data_device_type),
           payload_ptr, sizeof(ftm_cal_data_device_type) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                     sizeof(ftm_cal_data_device_type) );
  
  /* Fetch the Sub Opcode */
  memscpy( &(data_unpack->data_param.data_action), sizeof(ftm_calibration_data_action_type),
          payload_ptr, sizeof(ftm_calibration_data_action_type) );
  
  /* Update the payload size and Payload pointer */
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                     sizeof(ftm_calibration_data_action_type) );
  
  curr_action = data_unpack->data_param.data_action;
  
  /*--------------------------------------------------------------------------*/
  /* 0x1 : RGI PWR Table */
  /*--------------------------------------------------------------------------*/
  if( curr_action & DATA_OPCODE_MASK_PUSH_RGI_PWR )
  {
    memscpy( &(data_unpack->data_param.rgi_pwr.num_rgi_pwr_pairs),sizeof(uint8),
            payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
    data_unpack->data_param.rgi_pwr.pa_state = 0xFF;

    data_unpack->data_param.rgi_pwr.xptMode = 0xFF;
  
    num_rgi_pwr_pairs = 
      data_unpack->data_param.rgi_pwr.num_rgi_pwr_pairs;
  
    for( lp_var = 0; lp_var < num_rgi_pwr_pairs; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.rgi_pwr.rgi_pwr_tbl[lp_var].rgi),sizeof(uint8),
              payload_ptr, sizeof(uint8) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
      memscpy( &(data_unpack->data_param.rgi_pwr.rgi_pwr_tbl[lp_var].pwr_db100),sizeof(int16),
              payload_ptr, sizeof(int16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
    }
  }
  
  /*--------------------------------------------------------------------------*/
  /* 0x2 : Load DPD Table */
  /*--------------------------------------------------------------------------*/
  if( curr_action & DATA_OPCODE_MASK_PUSH_DPD_TBL )
  {
    memscpy( &(data_unpack->data_param.load_dpd.xpt_type),sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
    memscpy( &(data_unpack->data_param.load_dpd.num_amam),sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
    num_amam = data_unpack->data_param.load_dpd.num_amam;
  
    for( lp_var = 0; lp_var < num_amam; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.load_dpd.amam[lp_var]),sizeof(uint16),
               payload_ptr, sizeof(uint16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );
    }
  
    memscpy( &(data_unpack->data_param.load_dpd.num_ampm),sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
    num_ampm = data_unpack->data_param.load_dpd.num_ampm;
  
    for( lp_var = 0; lp_var < num_ampm; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.load_dpd.ampm[lp_var]),sizeof(uint16),
               payload_ptr, sizeof(uint16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );
    }
  
    memscpy( &(data_unpack->data_param.load_dpd.num_det), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
    num_det = data_unpack->data_param.load_dpd.num_det;    
  
    for( lp_var = 0; lp_var < num_det; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.load_dpd.det[lp_var]), sizeof(uint16),
               payload_ptr, sizeof(uint16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );
    }
  }
  
  /*--------------------------------------------------------------------------*/
  /* 0x4 : MLine Calc */
  /*--------------------------------------------------------------------------*/
  if( curr_action & DATA_OPCODE_MASK_PROCESS_MLINE )
  {
    memscpy( &(data_unpack->data_param.mline_params.lin_reg_percent_x100), sizeof(uint16),
             payload_ptr, sizeof(uint16) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );
  
    memscpy( &(data_unpack->data_param.mline_params.comp_dB_ept), sizeof(uint16),
             payload_ptr, sizeof(uint16) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );
  
    memscpy( &(data_unpack->data_param.mline_params.comp_dB_et), sizeof(uint16),
             payload_ptr, sizeof(uint16) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );

    memscpy( &(data_unpack->data_param.mline_params.pa_ept), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    memscpy( &(data_unpack->data_param.mline_params.pa_et), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
    memscpy( &(data_unpack->data_param.mline_params.curve_fit_type), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    memscpy( &(data_unpack->data_param.mline_params.vd_max_ept), sizeof(uint32),
             payload_ptr, sizeof(uint32) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint32) );

    memscpy( &(data_unpack->data_param.mline_params.vd_max_et), sizeof(uint32),
            payload_ptr, sizeof(uint32) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint32) );

    memscpy( &(data_unpack->data_param.mline_params.mline_bias_option), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    
    memscpy( &(data_unpack->data_param.mline_params.num_swp1_meas_pwrs), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    num_swp1_meas_pwrs = data_unpack->data_param.mline_params.num_swp1_meas_pwrs;
  
    memscpy( &(data_unpack->data_param.mline_params.num_swp2_ept_meas_pwrs), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    num_swp2_ept_meas_pwrs = data_unpack->data_param.mline_params.num_swp2_ept_meas_pwrs;
  
    memscpy( &(data_unpack->data_param.mline_params.num_swp2_ept_calc_pwrs), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    num_swp2_ept_calc_pwrs = data_unpack->data_param.mline_params.num_swp2_ept_calc_pwrs;
  
    memscpy( &(data_unpack->data_param.mline_params.num_swp2_et_meas_pwrs), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
    num_swp2_et_meas_pwrs = data_unpack->data_param.mline_params.num_swp2_et_meas_pwrs;

    memscpy( &(data_unpack->data_param.mline_params.num_swp2_et_calc_pwrs), sizeof(uint8),
            payload_ptr, sizeof(uint8) );

    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    num_swp2_et_calc_pwrs = data_unpack->data_param.mline_params.num_swp2_et_calc_pwrs;
  
    for( lp_var = 0; lp_var < num_swp1_meas_pwrs; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.mline_params.swp1_meas_data[lp_var].swp1_pa), sizeof(uint8),
               payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
      memscpy( &(data_unpack->data_param.mline_params.swp1_meas_data[lp_var].swp1_rgi),
               sizeof(uint8),
               payload_ptr, sizeof(uint8) );

      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

      memscpy( &(data_unpack->data_param.mline_params.swp1_meas_data[lp_var].swp1_bias), sizeof(uint32),
               payload_ptr, sizeof(uint32) );

      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint32) );
  
      memscpy( &(data_unpack->data_param.mline_params.swp1_meas_data[lp_var].swp1_pwr_db100), sizeof(int16),
              payload_ptr, sizeof(int16) );

      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
    }

    for( lp_var = 0; lp_var < num_swp2_ept_meas_pwrs; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.mline_params.swp2_ept_meas_pwrs[lp_var]), sizeof(int16),
               payload_ptr, sizeof(int16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
    }
  
    for( lp_var = 0; lp_var < num_swp2_ept_calc_pwrs; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.mline_params.swp2_ept_calc_pwrs[lp_var]), sizeof(int16),
               payload_ptr, sizeof(int16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
    }
    for( lp_var = 0; lp_var < num_swp2_et_meas_pwrs; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.mline_params.swp2_et_meas_pwrs[lp_var]), sizeof(int16),
               payload_ptr, sizeof(int16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
    }
  
    for( lp_var = 0; lp_var < num_swp2_et_calc_pwrs; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.mline_params.swp2_et_calc_pwrs[lp_var]), sizeof(int16),
               payload_ptr, sizeof(int16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
    }
  }
  
  /*--------------------------------------------------------------------------*/
  /* 0x8 : Delay Val processing */
  /*--------------------------------------------------------------------------*/
  if( curr_action & DATA_OPCODE_MASK_PROCESS_DELAY_VAL )
  {
    memscpy( &(data_unpack->data_param.et_delay_params.upper_bound_tx_earfcn),
            sizeof(uint32), payload_ptr, sizeof(uint32) );
  
    /* Update the upper bound chan */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint32) );
  
  }

  /*--------------------------------------------------------------------------*/
  /* 0x100 or 0x400: Pin Val processing (traditional or extended) */
  /*--------------------------------------------------------------------------*/
  if( curr_action & DATA_OPCODE_MASK_PROCESS_PIN_VAL || curr_action & DATA_OPCODE_MASK_PROCESS_PIN_VAL_EXT )
  {
    memscpy( &(data_unpack->data_param.pin_params.et_compression_point),
            sizeof(uint8), payload_ptr, sizeof(uint8) );
  
    /* Acquire compression point */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    memscpy( &(data_unpack->data_param.pin_params.current_chan),
            sizeof(uint32), payload_ptr, sizeof(uint32) );
  
    /* Acquire compression point */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint32) );

    /* PA state */
    memscpy( &(data_unpack->data_param.pin_params.pa_state),
            sizeof(uint8), payload_ptr, sizeof(uint8) );
  
    /* Acquire PA state */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    if ( curr_action & DATA_OPCODE_MASK_PROCESS_PIN_VAL_EXT ) 
    {
      /* Acquire extra process mask */
      memscpy( &(data_unpack->data_param.pin_params.pin_process_mask),
              sizeof(uint8), payload_ptr, sizeof(uint8) );
		
      /* Update payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
    }
    else
    {
      /* For traditional PIN CAL, zero out the process mask */
      data_unpack->data_param.pin_params.pin_process_mask = 0;
    }
  }

  /*--------------------------------------------------------------------------*/
  /* 0x10 : DPD Construction Data Processing */
  /*--------------------------------------------------------------------------*/
  if( curr_action & DATA_OPCODE_MASK_PROCESS_DPD )
  {
    memscpy( &(data_unpack->data_param.dpd_constr_params.xpt_mode), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

     memscpy( &(data_unpack->data_param.dpd_constr_params.num_swp2_meas_pwrs), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    num_swp2_meas_pwrs = data_unpack->data_param.dpd_constr_params.num_swp2_meas_pwrs;
  
    memscpy( &(data_unpack->data_param.dpd_constr_params.num_swp2_calc_pwrs), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
    num_swp2_calc_pwrs = data_unpack->data_param.dpd_constr_params.num_swp2_calc_pwrs;

    for( lp_var = 0; lp_var < num_swp2_meas_pwrs; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.dpd_constr_params.swp2_meas_pwrs[lp_var]), sizeof(int16),
               payload_ptr, sizeof(int16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
    }
  
    for( lp_var = 0; lp_var < num_swp2_calc_pwrs; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.dpd_constr_params.swp2_calc_pwrs[lp_var]), sizeof(int16),
               payload_ptr, sizeof(int16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
    }
  }

  /*--------------------------------------------------------------------------*/
  /* 0x20 : Load Calc DPD */
  /*--------------------------------------------------------------------------*/
  if( curr_action & DATA_OPCODE_MASK_LOAD_CALC_DPD )
  {
    memscpy( &(data_unpack->data_param.load_calc_dpd_params.load_action), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    memscpy( &(data_unpack->data_param.load_calc_dpd_params.xpt_mode), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    memscpy( &(data_unpack->data_param.load_calc_dpd_params.qpoet_min_volt_x100), sizeof(uint16),
             payload_ptr, sizeof(uint16) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );

    memscpy( &(data_unpack->data_param.load_calc_dpd_params.qpoet_max_volt_x100), sizeof(uint16),
            payload_ptr, sizeof(uint16) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );

    memscpy( &(data_unpack->data_param.load_calc_dpd_params.et_min_volt_x100), sizeof(uint16),
             payload_ptr, sizeof(uint16) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );

    memscpy( &(data_unpack->data_param.load_calc_dpd_params.num_swp2_meas_pwrs), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    num_swp2_meas_pwrs = data_unpack->data_param.load_calc_dpd_params.num_swp2_meas_pwrs;
  
    memscpy( &(data_unpack->data_param.load_calc_dpd_params.num_swp2_calc_pwrs), sizeof(uint8),
             payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
    num_swp2_calc_pwrs = data_unpack->data_param.load_calc_dpd_params.num_swp2_calc_pwrs;

    for( lp_var = 0; lp_var < num_swp2_meas_pwrs; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.load_calc_dpd_params.swp2_meas_pwrs[lp_var]), sizeof(int16),
               payload_ptr, sizeof(int16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
    }
  
    for( lp_var = 0; lp_var < num_swp2_calc_pwrs; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.load_calc_dpd_params.swp2_calc_pwrs[lp_var]), sizeof(int16),
               payload_ptr, sizeof(int16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
    }
  }
  
  /*--------------------------------------------------------------------------*/
  /* 0x40 : Return M Line Results */
  /*--------------------------------------------------------------------------*/
  if ( curr_action & DATA_OPCODE_MASK_RETURN_MLINE_RESULTS) 
  {
     memscpy(& (data_unpack->data_param.mline_results.ept_mline_results_action), sizeof(uint16),
             payload_ptr, sizeof(uint16) );

     /* Update the payload size and Payload pointer */
     UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );

     memscpy(& (data_unpack->data_param.mline_results.et_mline_results_action), sizeof(uint16),
             payload_ptr, sizeof(uint16) );

     /* Update the payload size and Payload pointer */
     UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );

     /* Get M Line Result Action type for EPT and ET*/
     results_ept_curr_action = data_unpack->data_param.mline_results.ept_mline_results_action;
     results_et_curr_action = data_unpack->data_param.mline_results.et_mline_results_action;

     if (cal_data->cal_config.debug_mode_en)
     {
      FTM_MSG_2(FTM_LOW, "Unpack: EPT Result Action= %d. ET Result Action = %d",results_ept_curr_action,results_et_curr_action );
     }

     /* Unpack EPT Selection Sweep Payload*/
     if (results_ept_curr_action & RESULT_SELECTION_SWEEP ) 
     {
        memscpy(& (data_unpack->data_param.mline_results.ept_mline_results_selec_swp_meas_params.num_selec_swp_pwrs), sizeof(uint8),
                payload_ptr, sizeof(uint8) );
        /* Update the payload size and Payload pointer */
        UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

        memscpy(& (data_unpack->data_param.mline_results.ept_mline_results_selec_swp_calc_params.num_selec_swp_pwrs), sizeof(uint8),
                payload_ptr, sizeof(uint8) );
        /* Update the payload size and Payload pointer */
        UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

        results_ept_num_selec_swp_target_pwrs = data_unpack->data_param.mline_results.ept_mline_results_selec_swp_meas_params.num_selec_swp_pwrs;

        for ( lp_var = 0; lp_var < results_ept_num_selec_swp_target_pwrs; lp_var++) 
        {
           memscpy(& (data_unpack->data_param.mline_results.ept_mline_results_selec_swp_meas_params.selec_swp_pwrs[lp_var]), sizeof(int16),
                   payload_ptr, sizeof(int16) );

           /* Update the payload size and Payload pointer */
           UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
        }

        results_ept_num_selec_swp_target_pwrs = data_unpack->data_param.mline_results.ept_mline_results_selec_swp_calc_params.num_selec_swp_pwrs;
        for ( lp_var = 0; lp_var < results_ept_num_selec_swp_target_pwrs; lp_var++) 
        {
           memscpy(& (data_unpack->data_param.mline_results.ept_mline_results_selec_swp_calc_params.selec_swp_pwrs[lp_var]), sizeof(int16),
                   payload_ptr, sizeof(int16) );

           /* Update the payload size and Payload pointer */
           UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
        }
     }
     /* Unpack EPT Compression Point payload*/
     if (results_ept_curr_action & RESULT_COMPRESSION_POINTS ) 
     {
        memscpy(& (data_unpack->data_param.mline_results.ept_mline_results_compression_point.num_compression_points), sizeof(uint8),
                payload_ptr, sizeof(uint8) );

        /* Update the payload size and Payload pointer */
        UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

        results_ept_num_compression_points = data_unpack->data_param.mline_results.ept_mline_results_compression_point.num_compression_points;

        for (lp_var = 0; lp_var < results_ept_num_compression_points ; lp_var++ ) 
        {
           memscpy(& (data_unpack->data_param.mline_results.ept_mline_results_compression_point.compression_point_rgi[lp_var]), sizeof(uint8),
                   payload_ptr, sizeof(uint8) );

           /* Update the payload size and Payload pointer */
           UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
        }

        for (lp_var = 0; lp_var < results_ept_num_compression_points ; lp_var++ ) 
        {
           memscpy(& (data_unpack->data_param.mline_results.ept_mline_results_compression_point.compression_point_bias[lp_var]), sizeof(uint32),
                      payload_ptr, sizeof(uint32) );

           /* Update the payload size and Payload pointer */
           UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint32) );
        }
     }

     /* Unpack EPT AMAM RMS payload*/
     if (results_ept_curr_action & RESULT_AMAM_RMS ) 
     {
        memscpy(& (data_unpack->data_param.mline_results.ept_mline_results_amam_rms.num_amam_rms_vals), sizeof(uint8),
                payload_ptr, sizeof(uint8) );

        /* Update the payload size and Payload pointer */
        UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );      
     }

     /* Unpack ET Selection Sweep Payload*/
     if (results_et_curr_action & RESULT_SELECTION_SWEEP ) 
     {
        memscpy(& (data_unpack->data_param.mline_results.et_mline_results_selec_swp_params.num_selec_swp_pwrs), sizeof(uint8),
                payload_ptr, sizeof(uint8) );

        /* Update the payload size and Payload pointer */
        UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

        results_et_num_selec_swp_target_pwrs = data_unpack->data_param.mline_results.et_mline_results_selec_swp_params.num_selec_swp_pwrs;

        for ( lp_var = 0; lp_var < results_et_num_selec_swp_target_pwrs; lp_var++) 
        {
           memscpy(& (data_unpack->data_param.mline_results.et_mline_results_selec_swp_params.selec_swp_pwrs[lp_var]), sizeof(int16),
                   payload_ptr, sizeof(int16) );

           /* Update the payload size and Payload pointer */
           UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );
        }
     }
     /* Unpack ET Compression Point Payload*/
     if (results_et_curr_action & RESULT_COMPRESSION_POINTS ) 
     {
        memscpy(& (data_unpack->data_param.mline_results.et_mline_results_compression_point.num_compression_points), sizeof(uint8),
                payload_ptr, sizeof(uint8) );

        /* Update the payload size and Payload pointer */
        UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

        results_et_num_compression_points = data_unpack->data_param.mline_results.et_mline_results_compression_point.num_compression_points;

        for (lp_var = 0; lp_var < results_et_num_compression_points ; lp_var++ ) 
        {
           memscpy(& (data_unpack->data_param.mline_results.et_mline_results_compression_point.compression_point_rgi[lp_var]), sizeof(uint8),
                   payload_ptr, sizeof(uint8) );

           /* Update the payload size and Payload pointer */
           UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
        }

        for (lp_var = 0; lp_var < results_et_num_compression_points ; lp_var++ ) 
        {
           memscpy(& (data_unpack->data_param.mline_results.et_mline_results_compression_point.compression_point_bias[lp_var]), sizeof(uint32),
                      payload_ptr, sizeof(uint32) );

           /* Update the payload size and Payload pointer */
           UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint32) );
        }
     }

   /* Unpack ET AMAM RMS payload */
     if (results_et_curr_action & RESULT_AMAM_RMS ) 
     {
        memscpy(& (data_unpack->data_param.mline_results.et_mline_results_amam_rms.num_amam_rms_vals), sizeof(uint8),
                payload_ptr, sizeof(uint8) );

        /* Update the payload size and Payload pointer */
        UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );      
     }
  }
  
  /*--------------------------------------------------------------------------*/
  /* 0x80 : XPT capture Result Opcode */
  /*--------------------------------------------------------------------------*/
  if( curr_action & DATA_OPCODE_MASK_RETURN_XPT_CAPTURE_RESULTS )
  {
    memscpy( &(data_unpack->data_param.xpt_capture_results.result_mask),
            sizeof(uint32), payload_ptr, sizeof(uint32) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint32) );
  
    memscpy( &(data_unpack->data_param.xpt_capture_results.num_results),
            sizeof(uint32), payload_ptr, sizeof(uint32) );

    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint32) );
  }


   /*--------------------------------------------------------------------------*/
  /* 0x200 : RGI PWR COM Table */
  /*--------------------------------------------------------------------------*/
  if( curr_action & DATA_OPCODE_MASK_PUSH_RGI_PWR_COM )
  {


    memscpy( &(data_unpack->data_param.rgi_pwr.num_rgi_pwr_pairs),sizeof(uint8),
            payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

 
    memscpy( &(data_unpack->data_param.rgi_pwr.pa_state),sizeof(uint8),
            payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

    
    memscpy( &(data_unpack->data_param.rgi_pwr.xptMode),sizeof(uint8),
            payload_ptr, sizeof(uint8) );
  
    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );


    num_rgi_pwr_pairs = 
      data_unpack->data_param.rgi_pwr.num_rgi_pwr_pairs;
  
    for( lp_var = 0; lp_var < num_rgi_pwr_pairs; lp_var++ )
    {
      memscpy( &(data_unpack->data_param.rgi_pwr.rgi_pwr_tbl[lp_var].rgi),sizeof(uint8),
              payload_ptr, sizeof(uint8) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );
  
      memscpy( &(data_unpack->data_param.rgi_pwr.rgi_pwr_tbl[lp_var].pwr_db100),sizeof(int16),
              payload_ptr, sizeof(int16) );
  
      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );

    }
  }
  
  /* Debug message */
  if( cal_data->cal_config.debug_mode_en )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_DATA,
               payload_size );
  } /* if( data_unpack->debug_mode_en == TRUE ) */
  
  /* return Payload size */
  return payload_size;

} /* ftm_calibration_data_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Data OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Data OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_data_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  ftm_rf_technology_type ftm_rf_tech;
  ftm_calibration_data_action_type curr_action;
  boolean return_val = TRUE;
  ftm_cal_data_opcode_data_type *cfg_data;
  
  uint8 device = 0;
  
  /* NULL Pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG_1( FTM_HIGH, "[calv3][Segment X][DATA.{0x%x}] "
               "Process : Null Argument",
               FTM_CAL_OPCODE_DATA );  
    return 0;
  } /* if ( cal_data == NULL ) */
  
  cfg_data = &(cal_data->seg_data.oc_payload.data); 
  
  ftm_rf_tech = (ftm_rf_technology_type)cfg_data->up_param.technology;
  
  /* Fetch the device mask sent from tools */
  device = cfg_data->up_param.device;
  
  curr_action = cfg_data->up_param.data_param.data_action;
  
  /*--------------------------------------------------------------------------*/
  /* 0x1 : RGI PWR Table */
  /*--------------------------------------------------------------------------*/
  if( curr_action & DATA_OPCODE_MASK_PUSH_RGI_PWR )
  {
    if (cal_data->cal_config.debug_mode_en)
    {
     FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                "Process : RGI Pwr Sub Opcode Processing",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_DATA );  
    }

    ftm_calibration_data_process_rgi_pwr_tbl_instr( cal_data, &(cfg_data->up_param), 
                                                    cfg_data->up_param.refChan, 
                                                    FALSE,
                                                    oc_disp );
  
    return_val = TRUE;
  
  } /* if( curr_action & DATA_OPCODE_MASK_PUSH_RGI_PWR ) */
  
  /*--------------------------------------------------------------------------*/
  /* 0x2 : Load DPD Table */
  /*--------------------------------------------------------------------------*/
  else if( curr_action & DATA_OPCODE_MASK_PUSH_DPD_TBL )
  {
    if (cal_data->cal_config.debug_mode_en)
    {
     FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                "Process : Load DPD Action Processing",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_DATA );  
    }
  
    ftm_calibration_data_process_load_dpd_tbl_instr( cal_data, 
                                                     &(cfg_data->up_param), 
                                                     ftm_rf_tech, device, 
                                                     oc_disp );
  
    return_val = TRUE;
  
  } /* if( curr_action & DATA_OPCODE_MASK_PUSH_DPD_TBL ) */
  
  /*--------------------------------------------------------------------------*/
  /* 0x4 : MLine Calc */
  /*--------------------------------------------------------------------------*/
  else if( curr_action & DATA_OPCODE_MASK_PROCESS_MLINE )
  {
    if (cal_data->cal_config.debug_mode_en)
    {
     FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                "Process : MLine Calc Action Processing",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_DATA );  
    }
    ftm_calibration_data_process_mline( cal_data, 
                                        &(cfg_data->up_param), 
                                                   ftm_rf_tech, 
                                        cfg_data->up_param.refChan, 
                                        oc_disp );
  
    return_val = TRUE;
  
  } /* if( curr_action & DATA_OPCODE_MASK_PROCESS_MLINE ) */
  
  /*--------------------------------------------------------------------------*/
  /* 0x8 : Delay Processing */
  /*--------------------------------------------------------------------------*/
  else if( curr_action & DATA_OPCODE_MASK_PROCESS_DELAY_VAL )
  {    
    if (cal_data->cal_config.debug_mode_en)
    {
     FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                "Process : Process Delay Action Processing",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_DATA );  
    }
    ftm_calibration_data_process_delay( cal_data, 
                                        &(cfg_data->up_param), 
                                        ftm_rf_tech, 
                                        cfg_data->up_param.refChan, 
                                        oc_disp );
    return_val = TRUE;
  
  } /* if( curr_action & DATA_OPCODE_MASK_PROCESS_DELAY_VAL ) */

  /*--------------------------------------------------------------------------*/
  /* 0x100 or 0x400 : Pin Processing (traditional or extended) */
  /*--------------------------------------------------------------------------*/
  else if( curr_action & DATA_OPCODE_MASK_PROCESS_PIN_VAL || curr_action & DATA_OPCODE_MASK_PROCESS_PIN_VAL_EXT )
  {    
    if (cal_data->cal_config.debug_mode_en)
    {
      if ( curr_action & DATA_OPCODE_MASK_PROCESS_PIN_VAL_EXT ) 
      {
        FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                "Process : Process Extended Pin Action Processing",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_DATA );
      }
      else
      {
        FTM_MSG_2(FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                "Process : Process Pin Action Processing",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_DATA ); 
      }
    }

    ftm_calibration_data_process_pin( cal_data, 
                                      &(cfg_data->up_param), 
                                      ftm_rf_tech, 
                                      cfg_data->up_param.refChan, 
                                      oc_disp );
    return_val = TRUE;
  
  } /* if( curr_action & DATA_OPCODE_MASK_PROCESS_PIN_VAL || curr_action & DATA_OPCODE_MASK_PROCESS_PIN_VAL_EXT ) */

  /*--------------------------------------------------------------------------*/
  /* 0x10 : DPD Construction Processing */
  /*--------------------------------------------------------------------------*/
  else if( curr_action & DATA_OPCODE_MASK_PROCESS_DPD )
  {    
    if (cal_data->cal_config.debug_mode_en)
    {
     FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                "Process : Process DPD Construction Action Processing",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_DATA );  
    }

    ftm_calibration_data_process_dpd_construction( cal_data, 
                                                   &(cfg_data->up_param), 
                                                   cfg_data->up_param.refChan,
                                                   oc_disp ); 
    return_val = TRUE;
  
  } /* if( curr_action & DATA_OPCODE_MASK_PROCESS_DPD ) */

  /*--------------------------------------------------------------------------*/
  /* 0x20 : Load Calc DPD */
  /*--------------------------------------------------------------------------*/
  else if( curr_action & DATA_OPCODE_MASK_LOAD_CALC_DPD )
  {
    if (cal_data->cal_config.debug_mode_en)
    {
     FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                "Process : Load Calc DPD Action Processing",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_DATA );  
    }

    ftm_calibration_data_load_calc_dpd( cal_data, 
                                        &(cfg_data->up_param), 
                                        ftm_rf_tech, 
                                        cfg_data->up_param.refChan, 
                                        device, oc_disp );
  
    return_val = TRUE;
  
  } /* if( curr_action & DATA_OPCODE_MASK_LOAD_CALC_DPD ) */

  /*--------------------------------------------------------------------------*/
  /* 0x40 : Return M Line Results */
  /*--------------------------------------------------------------------------*/
  else if (curr_action & DATA_OPCODE_MASK_RETURN_MLINE_RESULTS) 
  {
     FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                "Process : Return M Line Results",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_DATA );

     ftm_calibration_data_process_mline_results( cal_data,
                                                 &(cfg_data->up_param),
                                                 cfg_data->up_param.refChan,
                                                 oc_disp);
     return_val = TRUE;

  }/* if( curr_action & DATA_OPCODE_MASK_RETURN_MLINE_RESULTS ) */
  
  /*--------------------------------------------------------------------------*/
  /* 0x80 : Return XPT Capture Results */
  /*--------------------------------------------------------------------------*/
  else if (curr_action & DATA_OPCODE_MASK_RETURN_XPT_CAPTURE_RESULTS) 
  {
     FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
                "Process : Return XPT capture Results",
                cal_data->seg_data.cal_instance,
                FTM_CAL_OPCODE_DATA );

     ftm_calibration_data_process_xpt_capture_results( cal_data,
                                                 &(cfg_data->up_param),
                                                 cfg_data->up_param.refChan,
                                                 oc_disp);
     return_val = TRUE;

  }/* if( curr_action & DATA_OPCODE_MASK_RETURN_MLINE_RESULTS ) */

  /*--------------------------------------------------------------------------*/
  /* 0x200 : RGI PWR Table COM*/
  /*--------------------------------------------------------------------------*/
  else if( curr_action & DATA_OPCODE_MASK_PUSH_RGI_PWR_COM )
  {
    FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][DATA.{0x%x}] "
               "Process : RGI Pwr PA state Sub Opcode Processing",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_DATA );  

    ftm_calibration_data_process_rgi_pwr_tbl_instr( cal_data, &(cfg_data->up_param), 
                                                    cfg_data->up_param.refChan,
                                                    TRUE, 
                                                    oc_disp );
  
    return_val = TRUE;
  
  } /* if( curr_action & DATA_OPCODE_MASK_PUSH_RGI_PWR ) */
  
  /*--------------------------------------------------------------------------*/
  /* Incorrect Sub-Opcode */
  /*--------------------------------------------------------------------------*/
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][DATA.{0x%x}] "
               "Process : Incorrect Action for Data Processing",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_DATA );  
    return_val = FALSE;
  }
  
  return return_val;
} /* ftm_calibration_data_process */

/*! @} */

/*! @} */

