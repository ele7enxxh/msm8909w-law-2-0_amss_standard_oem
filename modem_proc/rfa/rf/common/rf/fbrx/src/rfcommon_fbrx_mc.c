/*!
  @file
  rfcommon_fbrx_mc.c

  @brief
  This module contains interface and implementation of common FBRx.
  
*/

/*==============================================================================

  Copyright (c) 2013 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/fbrx/src/rfcommon_fbrx_mc.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/15/15    gh     Remove featurization for building FBRx LDO ON and OFF scripts
12/10/14    aa     Store the GPS state globally to fix FBRx slow ramp issue 
12/08/14    dr     Tabasco featurization for FBRX
11/18/14   aro     Renamed SW based ILPC variable to better suit the feature
11/10/14    ka     Store SVLTE information
10/31/14    vr     Add modem specific check to configure_on_off_scripts api
10/15/14    JJ     Add return val check in configure_on_off_scripts api
10/08/14    ka     Replace rfc with device api for retrieving device pointer
09/30/14    aa     Fix OFT compiler error 
09/29/14    aa     Round off freq_idx to the nearest integral value 
08/27/14    gh     Moved ilpc_setup from init_tx to tune_to_chan
                   Added API to check if FBRx ILPC is enabled in NV
08/18/14   aro/gh  Add support for FBRx ILPC
09/17/14    dr     Adding DPM featurization
08/20/14    ka     Added api to enable/disable fbrx 
08/04/14    pv     Added support for FBRx + GPS concurrency based scripts.
08/11/14    ka     Buffer lin update results to avoid race condition
08/08/14    aa     Changed gps_state enum type
08/06/14    aa     Fixed compiler warning
08/06/14    aa     Fixed bug in the tech type mismatch
08/06/14   aro     Initial support for FED based ILPC correction
07/31/14    aa     Fix KW error
08/01/14    aa     Added support for rsb comp during calv3 opcode based fbrx setup
07/31/14    aa     Added api to set the gps state and update it to the fbrx dm
07/31/14    aa     Added fbrx NV check prior to applying fbrx temp comp
07/31/14    php    Remove FBRx ADC config for DPM
07/22/14    JJ     Deleted an un-used api
07/21/14    JJ     Fixed a compilation warning
07/21/14    JJ     Use therm bins from each tech to get temp comp value
07/07/14    aa     Added RSB IQMC changes for mulitple gain state support
07/14/14    aa     Fix FBRx DC Cal failures due to incorrect condition buffer ID check
06/30/14    ka     Set handle to invalid value if allocation fails
06/17/14    aa     Added support to enable FBRX DTR updates  
05/15/14    aa     Update the fbrx temp comp across freq 
05/13/14    aa     Add invalid fbrx lm handle check
05/05/14    aa     Move fbrx lm buffer settings to modem component 
05/05/14    ka     Added fbrx gain freq comp
04/21/14    aa     Added api to return fbrx lm handle 
04/11/14    ka     Update FBRx run decision based on NV setting
04/10/14    ka     Ensure fbrx state is cleared when tx is initialized
04/09/14    aa     Update FBRX CCS Enable/Disable Scripts in FBRx DM
04/03/14    ka     Clear filt error and swp offset in temp comp
03/18/14    aa     Update FTM Cal mode state in FBRx DM
03/13/14    ka     Update nv control for MTPL and FTM
03/12/14    aa     Latest FBRx changes for ILPC correction
03/11/14    JJ     Pass rflm_tech to rfcommon_fbrx_msm_update_rsb_coeff()
03/10/14    hm     Hook-up of FBRX enable API enhancement
03/07/14    aa     Update Tx bw config to FBRx DM
03/04/14    aa     Fix to get correct rsb coeff data from wtr device
02/27/14    aa     Update rfcommon_fbrx_mc_tune_to_chan
02/19/14    aa     Added support for FBRx RSB correction
02/18/14    ka     Added mtpl flag
02/13/14    aa     Update fbrx temp comp
02/13/14    ka     Added single trigger api
02/11/14    ka     Remove unconditional WTR enable for power savings
02/07/14    aa     Fix off target compiler error
02/06/14    aa     Clear fbrx lin update data during temp comp
01/30/14    aa     Update the fbrx mode in fbrx dm
01/14/14    aa     Added support to read FBRx NV's during bootup 
01/10/14    aa     Backing out change due to RFCal crash
01/08/14    aa     Added support to read FBRx NV's during bootup 
12/13/13    aa     Removed FBRx NV read NVs from EFS 
12/12/13    ka     Added protection code for handle allocation
12/12/13    cd     Provide FBRx gain state data to RFLM during linearizer 
                   loading
11/25/13    aa     Updated FBRX DM with FBRX Droop Cal data
11/25/13    ka     Change default gain state to 2
11/20/13    aa     Updated FBRX DM with FBRX Cal NV data
11/19/13    aa     Added featurization to fix compiler errors in D3925
11/19/13    aa     Updated FBRX DM with FBRX_GAIN_FREQ_I NV data
11/14/13    ka     Updated temp comp, added tune to chan
11/13/13    ka     API updates
10/30/13    ka     Added rf dev apis
09/13/13    ka     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfcom.h"
#include "rfcommon_fbrx_api.h"
#include "rfcommon_fbrx_ccs.h"
#include "rxlm_intf.h"
#ifdef FEATURE_RF_FBRX
#include "rflm_api_fbrx.h"
#endif /* FEATURE_RF_FBRX */
#include "rfm_mode_types.h"
#include "rfdevice_cmn_intf.h"
#include "DALSys.h"
#include "rfc_card.h"
#include "rflm_dm_api.h"
#include "rfcommon_fbrx_nv.h"
#include "rflm_api_fbrx_fw_hal.h"
#include "stringl.h" /* for memscpy */
#include "ftm.h"
#include "rfm_internal.h" /* for rfm_get_calibration_state() */
#include "rfcommon_msg.h"
#include "rfcommon_math.h"
#include "rfcommon_msm.h"
#include "rfcommon_fbrx_ilpc.h"
#include "rfcommon_core_utils.h"
#include "math.h"

#define RFCOMMON_FBRX_THERM_MIN 0
#define RFCOMMON_FBRX_THERM_MAX 255
#define RFCOMMON_FBRX_TEMP_TABLE_SIZ 8
#define RFCOMMON_FBRX_TEMP_BIN_SIZ (RFCOMMON_FBRX_THERM_MAX / (RFCOMMON_FBRX_TEMP_TABLE_SIZ-1))
#define RFCOMMON_FBRX_TXAGC_PA_STATE_MAX 8
#define RFCOMMON_FBRX_TXAGC_TX_LIN_APT_SIZE 64 
#define RFCOMMON_FBRX_MAX_BUFFERS_PER_TECH 1

extern rfcommon_msm_num_carr_bw 
rfcommon_fbrx_msm_convert_fbrx_tx_cfg_to_carr_bw( rfcommon_fbrx_tx_cfg_type fbrx_tx_cfg );

/*============================================================================*/
/*!
  @addtogroup RF_COMMON_FBRX
 
  @brief
  This module includes the main control functionality for FBRx.
 
  @{
*/
static uint8 fbrx_common_num_tech_buffers_allocated[RFM_NUM_MODES] = 
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static int32 rfcommon_fbrx_tech_handle[RFM_NUM_MODES] = 
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
/*============================================================================*/
/*!
  @name FBRx Main Control

  @brief
  This section contains main control functions for common FBRx.
 
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_enter_mode
 
  @details
  This function initializes common FBRx
 
  @return
  Status of LM allocation
*/
rfcommon_fbrx_error_type rfcommon_fbrx_mc_enter_mode( lm_handle_type *fbrx_lm_handle, 
                                                      rfm_mode_enum_type tech )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;
  #ifdef FEATURE_RF_FBRX
  boolean result;

  if (fbrx_common_num_tech_buffers_allocated[tech] < RFCOMMON_FBRX_MAX_BUFFERS_PER_TECH)
  {
    result = rfcommon_fbrx_msm_allocate_rxlm_buffer(fbrx_lm_handle);

    if (result != TRUE)
    {
      ret_value = RFCOMMON_FBRX_ERROR;
      *fbrx_lm_handle = -1;

      /* Print MSG */
      RF_MSG(RF_HIGH,
             "rfcommon_fbrx_mc_enter_mode: Unable to allocate buff");
    }
    else
    {
      /* update the fbrx handle allocated for current tech */
      rfcommon_fbrx_tech_handle[tech] = *fbrx_lm_handle;

      /* buffer allocation was successful, increment count */
      fbrx_common_num_tech_buffers_allocated[tech]++;
    }
  }
  else
  {
    /* Print MSG */
    RF_MSG(RF_HIGH,
           "rfcommon_fbrx_mc_enter_mode: FBRx buff already allocated");

    /* max number of buffers for this tech are already allocated */
    ret_value = RFCOMMON_FBRX_ERROR;
  }
  #endif /* FEATURE_RF_FBRX */
  return ret_value;
} /* rfcommon_fbrx_mc_enter_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_get_tech_handle
 
  @details
  This function returns the FBRx Handle for current tech

  @param tech
  The tech for which the fbrx lm handle is required
  
  @return
  FBRX LM Handle
*/
uint32 rfcommon_fbrx_mc_get_tech_handle( rfm_mode_enum_type tech )
{
  return rfcommon_fbrx_tech_handle[tech];
} /* rfcommon_fbrx_mc_get_tech_handle */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_init_tx
 
  @details
  This function prepares the FBRx LM buffer for a given Tx chain
  configuration
 
  @return
  Status of LM allocation
*/
rfcommon_fbrx_error_type rfcommon_fbrx_mc_init_tx( lm_handle_type fbrx_lm_handle, 
                                                   rfcommon_fbrx_tx_cfg_type cfg, 
                                                   rflm_handle_tx_t tx_handle )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;
  boolean update_result;
  #ifdef FEATURE_RF_FBRX
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;  

  /* Get FBRX DM data pointer */
  fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);

  if (fbrx_data_ptr != NULL)
  {
    rflm_fbrx_init_mem(fbrx_data_ptr);
    switch (cfg) 
    {
      case RFCOMMON_FBRX_MCDO_7X:
      case RFCOMMON_FBRX_SB_SVDO:
      case RFCOMMON_FBRX_1X:
      case RFCOMMON_FBRX_MCDO_3X:
        if (fbrx_common_num_tech_buffers_allocated[RFM_LTE_MODE] > 0)
        {
          fbrx_data_ptr->fbrx_ctl.c2k_is_in_svlte_mode = TRUE;
        }
        else
        {
          fbrx_data_ptr->fbrx_ctl.c2k_is_in_svlte_mode = FALSE;
        }
      break;

      default:
          fbrx_data_ptr->fbrx_ctl.c2k_is_in_svlte_mode = FALSE;
        break;
    }
  }

  update_result = rfcommon_fbrx_msm_update_static_settings(fbrx_lm_handle, cfg);

  if (update_result == TRUE )
  {
    /* Update the FBRx mode and TX Bandwidth & LM handle in FBRx DM */
    ret_value &= rfcommon_fbrx_mc_update_mode( tx_handle, cfg);

    if ( (ret_value == RFCOMMON_FBRX_SUCCESS)  && (rflm_dm_validate_handle(fbrx_lm_handle)== TRUE) )
    {
      if (fbrx_data_ptr != NULL)     
      {
        /* Update the FBRX lm handle to DM */
        fbrx_data_ptr->fbrx_ctl.fbrx_lm_handle = fbrx_lm_handle;
      }
    }
  }
 
  #endif /* FEATURE_RF_FBRX */
  return ret_value;
} /* rfcommon_fbrx_mc_init_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_exit
 
  @details
  This function de-initializes common FBRx
 
  @return
  Status of LM deallocation
*/
rfcommon_fbrx_error_type rfcommon_fbrx_mc_exit( lm_handle_type fbrx_lm_handle,
                                                rfm_mode_enum_type tech )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;
  #ifdef FEATURE_RF_FBRX
  boolean result;

  if (fbrx_common_num_tech_buffers_allocated[tech] > 0)
  {
    result = rfcommon_fbrx_msm_deallocate_rxlm_buffer(fbrx_lm_handle);

    if (result != TRUE)
    {
      ret_value = RFCOMMON_FBRX_ERROR;
    }
    else
    {
      /* de-allocation successful */
      fbrx_common_num_tech_buffers_allocated[tech]--;
    }
  }
  else
  {
    /* attempting to de-allocate without first allocating */
    ret_value = RFCOMMON_FBRX_ERROR;
  }

  #endif /* FEATURE_RF_FBRX */
  return ret_value;
} /* rfcommon_fbrx_mc_exit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_enable
 
  @details
  This function enables FBRx hw
 
  @return
  Status of LM configuration
*/
rfcommon_fbrx_error_type rfcommon_fbrx_enable( lm_handle_type fbrx_lm_handle )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;
  #ifdef FEATURE_RF_FBRX
  boolean fbrx_config_succeeded;

  fbrx_config_succeeded = rflm_fbrx_configure_chain(fbrx_lm_handle);

  if (!fbrx_config_succeeded)
  {
    ret_value = RFCOMMON_FBRX_ERROR;
  }

  #endif /* FEATURE_RF_FBRX */
  return ret_value;
} /* rfcommon_fbrx_mc_exit */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_convert_rfm_mode_to_rflm_tech()
 
  @details
  This function returns the rflm tech id based on the rfm mode enum type
 
*/

rflm_tech_id_t rfcommon_fbrx_mc_convert_rfm_mode_to_rflm_tech(rfm_mode_enum_type rfm_tech)
{
  rflm_tech_id_t rflm_tech;
  
  switch (rfm_tech)
  {
     case RFM_IMT_MODE:
       rflm_tech = RFLM_TECH_WCDMA;
       break;

     case RFM_1X_MODE:
       rflm_tech = RFLM_TECH_1X;
       break;

     case RFM_1XEVDO_MODE:
     case RFM_SVDO_MODE:
       rflm_tech = RFLM_TECH_HDR;
       break;

     case RFM_LTE_MODE:
     case RFM_LTE_FTM_MODE:
       rflm_tech = RFLM_TECH_LTE;
       break;

     case RFM_TDSCDMA_MODE:
     case RFM_TDSCDMA_MODE_FTM:
       rflm_tech = RFLM_TECH_TDSCDMA;
       break;

     default:
       rflm_tech = RFLM_MAX_TECH;
       break;
  }

  return rflm_tech;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_convert_rfcom_band_to_uint8
 
  @details
  This function returns the band as uint8 based on the rfcom_band_type_u and tech 
 
*/

uint8 rfcommon_fbrx_mc_convert_rfcom_band_to_uint8( rfm_mode_enum_type rfm_tech, rfcom_band_type_u rfcom_band)
{
  uint8 fbrx_band;
  
  switch (rfm_tech)
  {
     case RFM_IMT_MODE:
       fbrx_band = rfcom_band.wcdma_band;
       break;

     case RFM_1X_MODE:
     case RFM_1XEVDO_MODE:
     case RFM_SVDO_MODE:
       fbrx_band = rfcom_band.cdma_band;
       break;

     case RFM_LTE_MODE:
     case RFM_LTE_FTM_MODE:
       fbrx_band = rfcom_band.lte_band;
       break;

     case RFM_TDSCDMA_MODE:
     case RFM_TDSCDMA_MODE_FTM:
       fbrx_band = rfcom_band.tdscdma_band;
       break;

     default:
       fbrx_band = rfcom_band.gsm_band;
       break;
  }

  return fbrx_band;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_convert_uint8_to_rfcom_band
 
  @details
  This function returns the rfcom_band_type_u based on the uint8 fbrx band and tech 
 
*/

rfcom_band_type_u rfcommon_fbrx_mc_convert_uint8_to_rfcom_band( rfm_mode_enum_type rfm_tech, uint8 fbrx_band)
{
  rfcom_band_type_u rfcom_band;
  
  switch (rfm_tech)
  {
     case RFM_IMT_MODE:
       rfcom_band.wcdma_band = fbrx_band;
       break;

     case RFM_1X_MODE:
     case RFM_1XEVDO_MODE:
     case RFM_SVDO_MODE:
       rfcom_band.cdma_band = fbrx_band;
       break;

     case RFM_LTE_MODE:
     case RFM_LTE_FTM_MODE:
       rfcom_band.lte_band = fbrx_band;
       break;

     case RFM_TDSCDMA_MODE:
     case RFM_TDSCDMA_MODE_FTM:
       rfcom_band.tdscdma_band = fbrx_band;
       break;

     default:
       rfcom_band.gsm_band = fbrx_band;
       break;
  }

  return rfcom_band;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_tune_to_chan
 
  @details
  This function updates FBRx frequency-dependent values
 
*/

rfcommon_fbrx_error_type rfcommon_fbrx_mc_tune_to_chan(rflm_handle_tx_t tx_handle, 
                                                       rfm_mode_enum_type tech,
                                                       rfcom_band_type_u band,
                                                       uint16 chan_array[],                                       
                                                       uint8 num_channels,
                                                       lm_handle_type fbrx_lm_handle )
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;
  rflm_tech_id_t rflm_tech = rfcommon_fbrx_mc_convert_rfm_mode_to_rflm_tech(tech);

#if defined(FEATURE_RF_FBRX)
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;  
  boolean ilpc_retval = TRUE;

  /* Get FBRX DM data pointer */
  fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);

  if (fbrx_data_ptr != NULL)
  {
    /* update the rfm tech and band to FBRX DM */
    fbrx_data_ptr->fbrx_ctl.rfm_mode = (uint8) tech;
    fbrx_data_ptr->fbrx_ctl.fbrx_band = rfcommon_fbrx_mc_convert_rfcom_band_to_uint8(tech, band);

  #if (!(defined(FEATURE_TABASCO_MODEM)))
    /* Configure the ccs scripts to setup/disable fbrx in FBRX DM */
    ret_value &= rfcommon_fbrx_ccs_configure_on_off_scripts( RFM_DEVICE_0,
                                                             fbrx_lm_handle, 
                                                             tx_handle);
  #endif
    
    /* Enable/Disable FBRx based on FBRX Control NV settings */
    ret_value &= rfcommon_fbrx_mc_update_nv_control(tx_handle,  rflm_tech );
  
  #if (!(defined(FEATURE_DIMEPM_MODEM) || defined (FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
    /* DimePM and Jolokia do not have FBRx ADC chain */
    /* Update the RSB coefficients to RxLM */
    ret_value &= rfcommon_fbrx_mc_update_rsb_coeff( tx_handle, fbrx_lm_handle, tech, band );
  #endif

    /* Update FBRX DM struct with NV data for current tech & band */
    ret_value &= rfcommon_fbrx_nv_update_dm( fbrx_data_ptr, tech, band, chan_array, num_channels);

    /* Setup Power Meter based ILPC; if the feature is supported */
    #ifdef TEMP_FBRX_ILPC
    ilpc_retval = rfcommon_fbrx_ilpc_setup(RFM_DEVICE_0, tx_handle, fbrx_lm_handle);
    #endif
  }
  else
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_mc_tune_to_chan: "
                      "FBRX data pointer = NULL returned for current TX handle" );
    ret_value = RFCOMMON_FBRX_ERROR;
  }

  /* Setup Power Meter based ILPC if the feature is supported */
  if(rfcommon_fbrx_mc_is_ilpc_enabled(rflm_tech))
  {
    ilpc_retval = rfcommon_fbrx_ilpc_setup(RFM_DEVICE_0, tx_handle, fbrx_lm_handle);

    /* Flag as error if it API failed */
    if(!ilpc_retval)
    {
      ret_value = RFCOMMON_FBRX_ERROR;
    }
  }

#endif /* defined(FEATURE_RF_FBRX) */

  return ret_value;
} /* rfcommon_fbrx_mc_tune_to_chan */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_update_temp_comp
 
  @details
  This function updates FBRx temp comp values to the FBRX DM
 

*/
rfcommon_fbrx_error_type rfcommon_fbrx_mc_update_temp_comp(rflm_handle_tx_t tx_handle, 
                                                           int16 temperature,
                                                           uint16 *tx_therm_bins)
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;

  rflm_fbrx_dm_template_t *fbrx_data_ptr;
  uint8 run_idx = 0;
  uint8 gain_idx, bin_idx;
  int16 temp_cal_data[RFCOMMON_FBRX_TEMP_TABLE_SIZ] = {0};
  int32 cur_freq = 0, min_freq = 0, max_freq = 0;
  uint8 freq_idx = 0;
  uint8 i = 0;
  double temp_freq_idx = 0;
  #ifdef FEATURE_RF_FBRX

  fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);
  
  /* Check for NULL pointer */
  if (fbrx_data_ptr != NULL)
  {
    if (fbrx_data_ptr->fbrx_ctl.fbrx_nv_ctrl_status != 0)
    {
    /* CLear all the fbrx lin update data*/
    memset(&fbrx_data_ptr->run_decision_data.fbrx_updates[0][0], 0, sizeof(fbrx_data_ptr->run_decision_data.fbrx_updates));
    for (i=0;i<RFLM_FBRX_MAX_LIN_BUFFER_SIZE;i++)
    {
      /* CLear all the fbrx lin update data*/
      memset(&fbrx_data_ptr->lin_upd_data[i].pa_state_update[0], 0, sizeof(fbrx_data_ptr->lin_upd_data[i].pa_state_update) );
      memset(&fbrx_data_ptr->lin_upd_data[i].filt_error_db10[0][0], 0, sizeof(fbrx_data_ptr->lin_upd_data[i].filt_error_db10));

    /* Clear tx lin update data */
    for (run_idx=0; run_idx< (uint8)RFLM_FBRX_MAX_RUNS_PER_SF; run_idx++)
    {
        fbrx_data_ptr->lin_upd_data[i].gain_err[run_idx] = 0;
        fbrx_data_ptr->lin_upd_data[i].apply_update[run_idx] = FALSE;
      }
      fbrx_data_ptr->lin_upd_data[i].swp_offset = 0;
    }

      /* Get the freq values from DM */
      cur_freq = fbrx_data_ptr->fbrx_ctl.cur_tx_freq;
      min_freq = fbrx_data_ptr->fbrx_ctl.min_tx_freq;
      max_freq = fbrx_data_ptr->fbrx_ctl.max_tx_freq;

      if (cur_freq >= max_freq)
      {
        freq_idx = RFCOMMON_FBRX_NUM_TEMP_FREQ - 1;
      }
      else if (cur_freq <= min_freq)
      {
        freq_idx = 0;
      }
      else if (min_freq == max_freq)
      {
        freq_idx = (RFCOMMON_FBRX_NUM_TEMP_FREQ/2)-1 ;
      }
      else
      {
        temp_freq_idx = (double)((cur_freq - min_freq)*(RFCOMMON_FBRX_NUM_TEMP_FREQ-1))/(max_freq - min_freq);
        /* Round the freq_idx to the nearest integral value */
        freq_idx = (uint8)floor( temp_freq_idx + 0.5);
      }

      /* Copy the temp data for channel idx 0 from fbrx dm */
      for (gain_idx = 0; gain_idx < RFCOMMON_FBRX_NUM_GAIN_STATES; gain_idx++)
      {
        /* copy the temp bin values for each gain state */
        for (bin_idx =0; bin_idx<RFCOMMON_FBRX_TEMP_TABLE_SIZ; bin_idx++)
        {
          temp_cal_data[bin_idx] = fbrx_data_ptr->temp_comp_data.gain_state[gain_idx].temp[bin_idx][freq_idx];
        }
        /* Update the FBRX DM with the interpolated temp comp value for each gain state */
        fbrx_data_ptr->exp_gain_data.temp_comp[gain_idx] = rf_lininterp_u16_s16( tx_therm_bins,
                                                                                temp_cal_data,
                                                                                RFCOMMON_FBRX_TEMP_TABLE_SIZ,
                                                                                temperature );
      }
    }
    else
    {
      RF_MSG( RF_MED, "rfcommon_fbrx_mc_update_temp_comp: "
                      "No FBRX temp comp applied since FBRX is disabled via NV" );
    }
  }
  else
  {
    ret_value = RFCOMMON_FBRX_ERROR;
  }

  #endif /* FEATURE_RF_FBRX */
  return ret_value;
} /* rfcommon_fbrx_mc_update_temp_comp */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_enable_rf_dev
 
  @details
  This function enables the FBRx path on the RF device (WTR).
  Intended for use in FTM. In online mode the RF FBRx will be
  turned on and off as needed by tx agc.
 
  @return TRUE if device APIs return TRUE, FALSE otherwise

*/
boolean rfcommon_fbrx_mc_enable_rf_dev(rfdevice_rxtx_common_class *dev_ptr, uint8 gain_state)
{
  boolean rv = TRUE;
  #if defined(FEATURE_RF_FBRX) || defined(FEATURE_RFA_D3925)
  uint8 setup_time_us = 0;

  rv &= rfdevice_cmn_get_fbrx_setup_script(dev_ptr,
                                           RFDEVICE_EXECUTE_IMMEDIATE,
                                           NULL,
                                           &setup_time_us,
                                           RFDEVICE_FBRX_LDO_ALWAYS_ON);
  DALSYS_BusyWait(setup_time_us);

  rv &= rfdevice_cmn_get_fbrx_enable_script(dev_ptr,
                                            gain_state,
                                            RFDEVICE_EXECUTE_IMMEDIATE,
                                            NULL,
                                            FALSE,
                                            0,
                                            RFM_ALL_MODES);

  #endif /* defined(FEATURE_RF_FBRX) || defined(FEATURE_RFA_D3925) */

  return rv;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_disable_rf_dev
 
  @details
  This function disables the FBRx path on the RF device (WTR).
  Intended for use in FTM. In online mode the RF FBRx will be
  turned on and off as needed by tx agc.
 
  @return TRUE if device APIs return TRUE, FALSE otherwise

*/
boolean rfcommon_fbrx_mc_disable_rf_dev(rfdevice_rxtx_common_class *dev_ptr)
{
  boolean rv = TRUE;
  #if defined(FEATURE_RF_FBRX) || defined(FEATURE_RFA_D3925)

  rv &= rfdevice_cmn_get_fbrx_disable_script(dev_ptr,
                                             RFDEVICE_EXECUTE_IMMEDIATE,
                                             NULL,
                                             RFDEVICE_FBRX_LDO_ALWAYS_ON);

  #endif /* defined(FEATURE_RF_FBRX) || defined(FEATURE_RFA_D3925) */

  return rv;
}

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  TxAGC configuration to FED - obtain FBRx gain state information
 
  @param txlm_handle_id
  Tx Handle ID used to query RFLM DM buffer
 
  @param tx_pwr_dBm10
  Tx power value in dBm10 for which FBRx gain state is to be queried
 
  @return
  Returns the FBRx gain state as obtained from FBRx module
*/
uint8
rfcommon_fbrx_mc_txagc_get_fbrx_gain_state
(
  int32 txlm_handle_id,
  int16 tx_pwr_dBm10
)
{
  uint8 fbrx_gain_state;
  rflm_err_t rflm_err_status = RFLM_ERR_NONE;
  rflm_fbrx_get_gain_state_in_t in_data;
  rflm_fbrx_get_gain_state_out_t out_data;

  /* Query FBRx gain state */
  in_data.tx_pwr_db10 = tx_pwr_dBm10;
  rflm_err_status = rflm_fbrx_get_gain_state( txlm_handle_id,
                                              &in_data,
                                              &out_data );

  if ( rflm_err_status != RFLM_ERR_NONE )
  {
    RF_MSG( RF_ERROR, "rfcommon_mdsp_txagc_get_fbrx_gain_state: "
                      "Failed query for FBRx gain state from RFLM " );
    fbrx_gain_state = 0;
  }
  else
  {
    fbrx_gain_state = out_data.fbrx_gain_state;
  }

  return fbrx_gain_state;

}/* rfcommon_fbrx_mc_txagc_get_fbrx_gain_state */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_update_mode
 
  @details
  This function updates the fbrx mode, ftm cal mode and bandwidth for current tx handle in the FBRx DM
*/
rfcommon_fbrx_error_type rfcommon_fbrx_mc_update_mode( rflm_handle_tx_t tx_handle, rfcommon_fbrx_tx_cfg_type cfg)
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;
  #ifdef FEATURE_RF_FBRX
  rflm_fbrx_dm_template_t *fbrx_data_ptr;

  fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);
  if (fbrx_data_ptr != NULL)
  {
    /* Update the TX Bandwidth in FBRx DM */
    fbrx_data_ptr->fbrx_ctl.bandwidth = cfg;
    /* Update the FBRx Mode in FBRx DM */
    fbrx_data_ptr->fbrx_ctl.fbrx_mode = rfcommon_fbrx_msm_convert_tx_cfg_to_mode(cfg);
    /* Update the FBRx Cal Mode in FBRx DM */
    fbrx_data_ptr->fbrx_ctl.ftm_cal_mode = rfm_get_calibration_state();

    RF_MSG_4( RF_HIGH, "rfcommon_fbrx_mc_update_mode: tx handle = %d, fbrx_mode = %d, bandwidth = %d, ftm_cal_mode = %d", 
              tx_handle, fbrx_data_ptr->fbrx_ctl.fbrx_mode, cfg, fbrx_data_ptr->fbrx_ctl.ftm_cal_mode );
  }/*if (fbrx_data_ptr != NULL)*/

  #endif /* FEATURE_RF_FBRX */
  return ret_value;
} /* rfcommon_fbrx_mc_update_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_single_trigger
 
  @details
  This function triggers FBRx to run one iteration using override mode
 
*/
rfcommon_fbrx_error_type rfcommon_fbrx_mc_single_trigger(rflm_handle_tx_t tx_handle)
{
  rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;

  fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);

  if (fbrx_data_ptr != NULL)
  {  
    if (IS_FTM_IN_TEST_MODE() == FALSE)
    {
      switch ( fbrx_data_ptr->fbrx_ctl.fbrx_nv_ctrl_status )
      {
        case RFCOMMON_FBRX_ENABLE_NO_UPDATES:
          /* Set override values for single trigger mode */
          fbrx_data_ptr->override_ctl.counter = 1;
          fbrx_data_ptr->override_ctl.enable = TRUE;
          fbrx_data_ptr->override_ctl.use_count = TRUE;
          fbrx_data_ptr->override_ctl.override_mode = TRUE;
          fbrx_data_ptr->fbrx_ctl.mtpl_mode_enable = FALSE;
          fbrx_data_ptr->fbrx_ctl.txlin_update_flag = FALSE;
          fbrx_data_ptr->fbrx_ctl.txfe_update_flag = FALSE;
          break;

        case RFCOMMON_FBRX_ENABLE_LIN_UPDATES:
          /* Set override values for single trigger mode */
          fbrx_data_ptr->override_ctl.counter = 1;
          fbrx_data_ptr->override_ctl.enable = TRUE;
          fbrx_data_ptr->override_ctl.use_count = TRUE;
          fbrx_data_ptr->override_ctl.override_mode = TRUE;
          fbrx_data_ptr->fbrx_ctl.mtpl_mode_enable = FALSE;
          fbrx_data_ptr->fbrx_ctl.txlin_update_flag = TRUE;
          fbrx_data_ptr->fbrx_ctl.txfe_update_flag = FALSE;
          fbrx_data_ptr->fbrx_ctl.all_rgi_update_flag = TRUE;
          break;

        case RFCOMMON_FBRX_ENABLE_MTPL:
          /* Set override values for single trigger mode */
          fbrx_data_ptr->override_ctl.counter = 1;
          fbrx_data_ptr->override_ctl.enable = TRUE;
          fbrx_data_ptr->override_ctl.use_count = TRUE;
          fbrx_data_ptr->override_ctl.override_mode = TRUE;
          fbrx_data_ptr->fbrx_ctl.mtpl_mode_enable = TRUE;
          fbrx_data_ptr->fbrx_ctl.txlin_update_flag = FALSE;
          fbrx_data_ptr->fbrx_ctl.txfe_update_flag = FALSE;
          break;

        case RFCOMMON_FBRX_ENABLE_IMM_DTR_ALWAYS:
          /* Set override values for single trigger mode */
          fbrx_data_ptr->override_ctl.counter = 1;
          fbrx_data_ptr->override_ctl.enable = TRUE;
          fbrx_data_ptr->override_ctl.use_count = TRUE;
          fbrx_data_ptr->override_ctl.override_mode = TRUE;
          fbrx_data_ptr->fbrx_ctl.mtpl_mode_enable = TRUE;
          fbrx_data_ptr->fbrx_ctl.txlin_update_flag = FALSE;
          fbrx_data_ptr->fbrx_ctl.txfe_update_flag = TRUE;
          break;

        case RFCOMMON_FBRX_ENABLE_IMM_DTR_OPT:
          /* Set override values for single trigger mode */
          fbrx_data_ptr->override_ctl.counter = 1;
          fbrx_data_ptr->override_ctl.enable = TRUE;
          fbrx_data_ptr->override_ctl.use_count = TRUE;
          fbrx_data_ptr->override_ctl.override_mode = TRUE;
          fbrx_data_ptr->fbrx_ctl.mtpl_mode_enable = TRUE;
          fbrx_data_ptr->fbrx_ctl.txlin_update_flag = FALSE;
          fbrx_data_ptr->fbrx_ctl.txfe_update_flag = TRUE;
          fbrx_data_ptr->fbrx_ctl.all_rgi_update_flag = FALSE;
          break;

        default:
          /* Set override values for single trigger mode */
          fbrx_data_ptr->override_ctl.counter = 1;
          fbrx_data_ptr->override_ctl.enable = FALSE;
          fbrx_data_ptr->override_ctl.use_count = TRUE;
          fbrx_data_ptr->override_ctl.override_mode = TRUE;
          fbrx_data_ptr->fbrx_ctl.mtpl_mode_enable = FALSE;
          fbrx_data_ptr->fbrx_ctl.txlin_update_flag = FALSE;
          fbrx_data_ptr->fbrx_ctl.txfe_update_flag = FALSE;
          break;
      } /* end switch case */
    }
  }
  else
  {
    ret_value = RFCOMMON_FBRX_ERROR;
  }

  return ret_value;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update FBRX DM buffer with the IQMC coefficients 

  @details
  RSB coefficients are obtained from device driver and updated to FBRX DM
 
  @param tx_handle: Tx Handle
  @param fbrx_lm_handle: FBRx LM Handle
  @param tech: current tech 
  @param band: current band 

  @retval True if update is succesful. False if update failed. 
*/
rfcommon_fbrx_error_type 
rfcommon_fbrx_mc_update_rsb_coeff( rflm_handle_tx_t tx_handle, 
                                   lm_handle_type fbrx_lm_handle, 
                                   rfm_mode_enum_type tech,
                                   rfcom_band_type_u band )
{
  boolean api_status = FALSE;
  rfcommon_fbrx_error_type ret_val = RFCOMMON_FBRX_SUCCESS;
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;
  rfdevice_rxtx_common_class *dev_ptr = NULL;
  rfdevice_fbrx_rsb_data_type rsb_data = {0,0};
  uint8 gs_idx = 0;
  rxlm_chain_type rxlm_chain;
  rflm_tech_id_t rflm_tech = RFLM_TECH_1X;
  lm_tech_type lm_tech = LM_UMTS;
  rfcommon_msm_num_carr_bw num_carr_bw; 
  rfcommon_msm_bbrx_mode_type mode = BBRX_MODE_0;
  lm_status_type lm_status = LM_INVALID_PARAMETER;
             
  #if (!(defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)))
  rxlm_chain = RXLM_CHAIN_FBRX;
  #else
  /* added rxlm chain 0 only to pass compilation on DPM and Jolokia as they do not have a dedicated FBRX Chain */
  rxlm_chain = RXLM_CHAIN_0;   
  #endif
  
  fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);

  if (fbrx_data_ptr != NULL)
  {
    rflm_tech = rfcommon_fbrx_mc_convert_rfm_mode_to_rflm_tech(tech);
    lm_tech = rfcommon_msm_convert_rflm_tech_to_lm_tech(rflm_tech);
    num_carr_bw = rfcommon_fbrx_msm_convert_fbrx_tx_cfg_to_carr_bw(fbrx_data_ptr->fbrx_ctl.bandwidth);

    dev_ptr = rfcommon_core_get_tx_device_ptr(RFM_DEVICE_0, tech, band);

    if (dev_ptr != NULL)
    {

    lm_status = rxlm_acquire_lock(fbrx_lm_handle);

    /* Get RSB coefficients from device driver */
    for (gs_idx = 0; gs_idx < RFLM_FBRX_NUM_GAIN_STATES ;gs_idx++)
    {
      api_status = rfdevice_cmn_get_fbrx_rsb_coeff(dev_ptr, tech, band, gs_idx, &rsb_data);
      if (api_status != TRUE)
      {
        RF_MSG(RF_ERROR, "rfdevice_cmn_get_fbrx_rsb_coeff failed");
        ret_val = RFCOMMON_FBRX_ERROR;
      }

      /* Update RSB and IQMC coefficients to FBRx DM */
      fbrx_data_ptr->rsb_data[gs_idx].a_coeff = rsb_data.a_coeff;
      fbrx_data_ptr->rsb_data[gs_idx].b_coeff = rsb_data.b_coeff;
      fbrx_data_ptr->rsb_data[gs_idx].iqmc = rfcommon_msm_get_iqmc_coeffs( fbrx_lm_handle, 
                                                                           rxlm_chain, 
                                                                           lm_tech,
                                                                           num_carr_bw,
                                                                           rsb_data.a_coeff,
                                                                           rsb_data.b_coeff);
      RF_MSG_4(RF_MED, "rfcommon_fbrx_mc_update_rsb_coeff: fbrx gs idx = %d, rsb a = %d, rsb b = %d, iqmc = %d", 
                        gs_idx, rsb_data.a_coeff, rsb_data.b_coeff, fbrx_data_ptr->rsb_data[gs_idx].iqmc);
    }

    mode = rfcommon_msm_get_bbrx_mode(rxlm_chain, lm_tech, num_carr_bw);
  lm_status = rfcommon_msm_update_rxlm_by_qfuse(fbrx_lm_handle, rxlm_chain, mode);
  if (lm_status != LM_SUCCESS)
  {
    RF_MSG_1(RF_ERROR, "rfcommon_msm_update_rxlm_by_qfuse failed, status=%d", lm_status);
    ret_val = RFCOMMON_FBRX_ERROR;
  }
  lm_status = rxlm_release_lock(fbrx_lm_handle);
    }

  }

  return ret_val;
} /*rfcommon_fbrx_mc_update_rsb_coeff*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_update_nv_control
 
  @details
  This function controls the FBRx state based on NV for each tech.
  1. Either Enable or Disable FBRx
  2. Apply FBRx correction (update) 
 
  @params:
  tx_handle: tx handle
  tech: Current tech
 
*/
rfcommon_fbrx_error_type 
rfcommon_fbrx_mc_update_nv_control(rflm_handle_tx_t tx_handle,  rflm_tech_id_t tech )
{
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;
  rfcommon_fbrx_error_type api_status = RFCOMMON_FBRX_SUCCESS;

  rfcommon_fbrx_control_enum_type fbrx_ctrl_val = rfcommon_fbrx_nv_get_control_val(tech);

  fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);

  if (fbrx_data_ptr != NULL)
  {
    /* Update the fbrx control value in FBRX DM */    
    fbrx_data_ptr->fbrx_ctl.tech = tech;
    fbrx_data_ptr->fbrx_ctl.fbrx_nv_ctrl_status = fbrx_ctrl_val;
    fbrx_data_ptr->fbrx_ctl.enable_sw_based_ilpc = FALSE;

    switch (fbrx_ctrl_val)
    {
      /* Disable FBRx and do not apply any updates */
      case RFCOMMON_FBRX_DISABLE_NO_UPDATES:
        fbrx_data_ptr->override_ctl.override_mode = TRUE;
        fbrx_data_ptr->override_ctl.enable = FALSE;
        fbrx_data_ptr->fbrx_ctl.txlin_update_flag = FALSE;
        fbrx_data_ptr->fbrx_ctl.mtpl_mode_enable = FALSE;
        fbrx_data_ptr->fbrx_ctl.txfe_update_flag = FALSE;
        fbrx_data_ptr->run_decision_data.run_method = RFLM_FBRX_RUN_NEVER;
        break;

      case RFCOMMON_FBRX_ENABLE_NO_UPDATES:
        fbrx_data_ptr->fbrx_ctl.txlin_update_flag = FALSE;
        fbrx_data_ptr->fbrx_ctl.mtpl_mode_enable = FALSE;
        fbrx_data_ptr->fbrx_ctl.txfe_update_flag = FALSE;
        fbrx_data_ptr->run_decision_data.run_method = RFLM_FBRX_RUN_TIMER_ONLY;
        break;
        
      case RFCOMMON_FBRX_ENABLE_LIN_UPDATES:
        fbrx_data_ptr->fbrx_ctl.txlin_update_flag = TRUE;
        fbrx_data_ptr->fbrx_ctl.all_rgi_update_flag = TRUE; 
        fbrx_data_ptr->fbrx_ctl.mtpl_mode_enable = FALSE;
        fbrx_data_ptr->fbrx_ctl.txfe_update_flag = FALSE;
        fbrx_data_ptr->run_decision_data.run_method = RFLM_FBRX_RUN_TIMER_ONLY; //RFLM_FBRX_RUN_OPT_1;
        break;

      case RFCOMMON_FBRX_ENABLE_MTPL:
        /* Do nothing */
        break;

      case RFCOMMON_FBRX_ENABLE_IMM_DTR_ALWAYS:
        fbrx_data_ptr->fbrx_ctl.txlin_update_flag = FALSE;
        fbrx_data_ptr->fbrx_ctl.mtpl_mode_enable = FALSE;
        fbrx_data_ptr->fbrx_ctl.txfe_update_flag = TRUE;
        fbrx_data_ptr->run_decision_data.run_method = RFLM_FBRX_RUN_ALWAYS;
        break;

      case RFCOMMON_FBRX_ENABLE_IMM_DTR_OPT:
        fbrx_data_ptr->fbrx_ctl.txlin_update_flag = TRUE;
        fbrx_data_ptr->fbrx_ctl.all_rgi_update_flag = FALSE; 
        fbrx_data_ptr->fbrx_ctl.mtpl_mode_enable = FALSE;
        fbrx_data_ptr->fbrx_ctl.txfe_update_flag = TRUE;
        fbrx_data_ptr->run_decision_data.run_method = RFLM_FBRX_RUN_OPT_1;
        break;

     case RFCOMMON_FBRX_ENABLE_SW_BASED_ILPC:
       fbrx_data_ptr->fbrx_ctl.enable_sw_based_ilpc = TRUE;
       fbrx_data_ptr->run_decision_data.run_method = RFLM_FBRX_RUN_ALWAYS;
       break;
        
      default: /* Disable FBRX */
        fbrx_data_ptr->override_ctl.override_mode = TRUE;
        fbrx_data_ptr->override_ctl.enable = FALSE;
        fbrx_data_ptr->fbrx_ctl.txlin_update_flag = FALSE;
        fbrx_data_ptr->fbrx_ctl.mtpl_mode_enable = FALSE;
        fbrx_data_ptr->fbrx_ctl.txfe_update_flag = FALSE;
        fbrx_data_ptr->run_decision_data.run_method = RFLM_FBRX_RUN_NEVER;
        break;
    }
  }
  else
  {
    api_status = RFCOMMON_FBRX_ERROR;
  }

  return api_status;
} /*rfcommon_fbrx_mc_update_nv_control*/


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_set_gps_state
 
  @details
  This function updates the gps state to the FBRX DM. 
  It is to be called by common dev manager code when gps changes state.

  @params:
  gps_state: GPS state if active or not
*/
void rfcommon_fbrx_mc_set_gps_state( rflm_fbrx_gps_state_type gps_state )
{

  /* Set the GPS state globally */
  rflm_fbrx_set_gps_state(gps_state);

  RF_MSG_1(RF_HIGH, "rfcommon_fbrx_mc_set_gps_state: GPS state rflm_fbrx_gps_state_info set to = %d ", gps_state);

} /*rfcommon_fbrx_mc_set_gps_state*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_enable_fbrx
 
  @details
  This function enables or disables fbrx operation

  @params:
  enable: TRUE to enable fbrx operation, FALSE to disable
*/
void rfcommon_fbrx_mc_enable_fbrx( boolean enable )
{
  uint32 handle = 0;
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;

  for (handle=0;handle<RFLM_HANDLE_COUNTS;handle++)
  {
    fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(handle);
    if (fbrx_data_ptr != NULL)
    {
      if (enable)
      {
        /* Set override values to enable operation */
        fbrx_data_ptr->override_ctl.enable = TRUE;
        fbrx_data_ptr->override_ctl.use_count = FALSE;
        fbrx_data_ptr->override_ctl.override_mode = FALSE;
      }
      else
      {
        /* Set override values to stop operation */
        fbrx_data_ptr->override_ctl.enable = FALSE;
        fbrx_data_ptr->override_ctl.use_count = FALSE;
        fbrx_data_ptr->override_ctl.override_mode = TRUE;
      }
    }
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_mc_reconfigure_rxlm
 
  @details
  This function reconfigures rxlm for FBRx operation. It is intended for use
  to restore FBRx operation after rxlm has been configured for a different
  operation such as XPT capture.

*/
rfcommon_fbrx_error_type rfcommon_fbrx_mc_reconfigure_rxlm
( 
  rfm_mode_enum_type tech, 
  rfcommon_fbrx_tx_cfg_type cfg,
  rflm_handle_tx_t tx_handle,
  rfcom_band_type_u band
)
{
   rfcommon_fbrx_error_type ret_value = RFCOMMON_FBRX_SUCCESS;
   lm_handle_type fbrx_lm_handle;

   fbrx_lm_handle = rfcommon_fbrx_mc_get_tech_handle( tech );

   ret_value = rfcommon_fbrx_msm_update_static_settings(fbrx_lm_handle, cfg);

   if (ret_value == RFCOMMON_FBRX_SUCCESS)
   {
     rfcommon_fbrx_mc_update_rsb_coeff(tx_handle, fbrx_lm_handle, tech, band);
   }

   if (ret_value == RFCOMMON_FBRX_SUCCESS) 
   {
      ret_value = rfcommon_fbrx_enable( fbrx_lm_handle );

      if (ret_value == RFCOMMON_FBRX_SUCCESS) 
      {
         RF_MSG(RF_HIGH,"rfcommon_fbrx_mc_reconfigure_rxlm re-enabled FBRX");
      }
   }

   return ret_value;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Check if FBRx ILPC is enabled or not based on NV
 
  @details
  This function checks if FBRx is enabled or not through fbrx NV.

  @params:
  tech: Current tech
*/
boolean rfcommon_fbrx_mc_is_ilpc_enabled(rflm_tech_id_t tech )
{
  boolean ret_val = FALSE;
  rfcommon_fbrx_control_enum_type control_val = RFCOMMON_FBRX_DISABLE_NO_UPDATES;

  /* Get data from NV */
  control_val = rfcommon_fbrx_nv_get_control_val(tech);

  /* Check if FBRx is enabled */
  ret_val = (control_val == RFCOMMON_FBRX_ENABLE_SW_BASED_ILPC);

  /* Print MSG */
  RF_MSG_1(RF_LOW, "ILPC enabled %d", ret_val);

  return ret_val;
} /* rfcommon_fbrx_mc_is_ilpc_enabled */


/*! @} */

/*! @} */


