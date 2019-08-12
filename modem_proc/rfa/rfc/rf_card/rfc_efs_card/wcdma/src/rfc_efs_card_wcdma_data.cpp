/*!
   @file
   rfc_efs_card_wcdma_config.cpp

   @brief
   This file contains implementation the rfc_efs_card_wcdma_data class, which can be used to
   configure the rf-card related data as required.

*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/rf_card/rfc_efs_card/wcdma/src/rfc_efs_card_wcdma_data.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
03/11/13   sar     Updated file for APQ, GNSS only target.
03/05/13   vrb     Replacing RFC accessor param init_req boolean by req enum
11/07/12   sr      Initial version.

============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfc_efs_card_cmn_data.h" 
#include "rfc_common.h" 
#include "rfc_efs_card_wcdma_data.h"
#include "rfcom.h" 


static rfc_sig_info_type *rfc_efs_card_wcdma_init_sig_data[RFC_CONFIG_RXTX_MAX] = {0};
static rfc_sig_info_type *rfc_efs_card_wcdma_band_sig_data[RFM_MAX_WAN_DEVICES][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_WCDMA_BANDS] = {{{0}}};
static rfc_device_info_type *rfc_efs_card_wcdma_init_device_info[RFC_CONFIG_RXTX_MAX] = {0};
static rfc_device_info_type *rfc_efs_card_wcdma_band_device_info[RFM_MAX_WAN_DEVICES][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_WCDMA_BANDS] = {{{0}}};


rfc_wcdma_data * rfc_efs_card_wcdma_data::get_instance()
{
  if (rfc_wcdma_data_ptr == NULL)
  {
    rfc_wcdma_data_ptr = (rfc_wcdma_data *)new rfc_efs_card_wcdma_data();
  }
  return( (rfc_wcdma_data *)rfc_wcdma_data_ptr);
}

//constructor
rfc_efs_card_wcdma_data::rfc_efs_card_wcdma_data()
  :rfc_wcdma_data()
{
  efs_data_load(); // load the rfc data from EFS
  verify_rfc_data(); // verify the rfc data revision compatibility.
}

// Destructor
rfc_efs_card_wcdma_data::~rfc_efs_card_wcdma_data()
{
  int32 band = RFCOM_NUM_WCDMA_BANDS;
  int32 path = RFM_MAX_WAN_DEVICES;

  // free all the dynamically allocated memory in this class 

  /* GPIO/GRFC data clean-up  */
  delete rfc_efs_card_wcdma_init_sig_data[RFC_CONFIG_RX];
  rfc_efs_card_wcdma_init_sig_data[RFC_CONFIG_RX] = NULL;
  delete rfc_efs_card_wcdma_init_device_info[RFC_CONFIG_RX];
  rfc_efs_card_wcdma_init_device_info[RFC_CONFIG_RX] = NULL;

  /* device cfg info data clean-up  */
  delete rfc_efs_card_wcdma_init_sig_data[RFC_CONFIG_TX];
  rfc_efs_card_wcdma_init_sig_data[RFC_CONFIG_TX] = NULL;
  delete rfc_efs_card_wcdma_init_device_info[RFC_CONFIG_TX];
  rfc_efs_card_wcdma_init_device_info[RFC_CONFIG_TX] = NULL;

  for (path = (int32)RFM_DEVICE_0; path < (int32)RFM_MAX_WAN_DEVICES; path++)
  {
    for (band = 0; band < (int32)RFCOM_NUM_WCDMA_BANDS; band++)
    {
      /* GPIO/GRFC data clean-up  */
      delete rfc_efs_card_wcdma_band_sig_data[path][RFC_CONFIG_RX][band];
      rfc_efs_card_wcdma_band_sig_data[path][RFC_CONFIG_RX][band] = NULL;
      delete rfc_efs_card_wcdma_band_sig_data[path][RFC_CONFIG_TX][band];
      rfc_efs_card_wcdma_band_sig_data[path][RFC_CONFIG_TX][band] = NULL;

      /* device cfg info data clean-up  */
      delete rfc_efs_card_wcdma_band_device_info[path][RFC_CONFIG_RX][band];
      rfc_efs_card_wcdma_band_device_info[path][RFC_CONFIG_RX][band] = NULL;
      delete rfc_efs_card_wcdma_band_device_info[path][RFC_CONFIG_TX][band];
      rfc_efs_card_wcdma_band_device_info[path][RFC_CONFIG_TX][band] = NULL;

    }
  }

  rfc_wcdma_data_ptr = NULL;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_data for wcdma tech based on specified configuration parameters.

  @details
  This function returns the rfc_data for wcdma tech based on specified configuration parameters.
 
  @param
  'cfg' : configuration parameters for which rfc_data to be retrived.
  'ptr' : ptr to the rfc_data.
 
  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_efs_card_wcdma_data::sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr )
{

  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  *ptr = NULL;

  if (NULL==cfg)
  {
    return FALSE;
  }

  if ( ( cfg->rx_tx < RFC_CONFIG_RXTX_INVALID ) && ( cfg->req == RFC_REQ_INIT ) )
  { 
    if (rfc_efs_card_wcdma_init_sig_data[cfg->rx_tx] != NULL)
    {
      *ptr = (rfc_sig_cfg_type *)rfc_efs_card_wcdma_init_sig_data[cfg->rx_tx]->cfg_sig_list;  
      ret_val = TRUE; 
    }
  }
  else if ( (cfg->logical_device < RFM_MAX_DEVICES) && (cfg->rx_tx < RFC_CONFIG_RXTX_INVALID) && (cfg->band < RFCOM_NUM_WCDMA_BANDS) )
  {
    if (rfc_efs_card_wcdma_band_sig_data[cfg->logical_device][cfg->rx_tx][cfg->band] != NULL)
    {
      *ptr = (rfc_sig_cfg_type *)(rfc_efs_card_wcdma_band_sig_data[cfg->logical_device][cfg->rx_tx][cfg->band]->cfg_sig_list);
      ret_val = TRUE;
    }
  }

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the device_info table for wcdma tech based on specified configuration
  parameters.

  @details
  This function returns the device_info_table for wcdma tech based on specified configuration
  parameters.
 
  @param
  'cfg' : configuration parameters for which rfc_data to be retrived.
  'ptr' : ptr to the device_info_table.
 
  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_efs_card_wcdma_data::devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr )
{

  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  *ptr = NULL;

  if (NULL==cfg)
  {
    return FALSE;
  }

  if ( ( cfg->rx_tx < RFC_CONFIG_RXTX_INVALID ) && ( cfg->req == RFC_REQ_INIT ) )
  { 
    if (rfc_efs_card_wcdma_init_device_info[cfg->rx_tx] != NULL)
    {
      *ptr = rfc_efs_card_wcdma_init_device_info[cfg->rx_tx];  
      ret_val = TRUE; 
    }
  }
  else if ( (cfg->logical_device < RFM_MAX_DEVICES) && (cfg->rx_tx < RFC_CONFIG_RXTX_INVALID) && (cfg->band < RFCOM_NUM_WCDMA_BANDS) )
  {
    if (rfc_efs_card_wcdma_band_device_info[cfg->logical_device][cfg->rx_tx][cfg->band] != NULL)
    {
      *ptr = (rfc_efs_card_wcdma_band_device_info[cfg->logical_device][cfg->rx_tx][cfg->band]);
      ret_val = TRUE;
    }
  }

  return ret_val;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function loads the rfc data from efs file to local memory.

  @details
  This function loads the rfc data from efs file to local memory.
 
  @param
  None.
 
  @retval
  None.
*/ 
void rfc_efs_card_wcdma_data::efs_data_load(void)
{
  int32 band = 0;
  int32 logical_path = 0;
  int32 rxtx = 0;
  rfm_mode_enum_type mode = RFM_IMT_MODE;

  rfc_efs_card_cmn_data *efs_data_ptr = (rfc_efs_card_cmn_data *)rfc_common_data::get_instance();
  if (efs_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to get rfc_common_data_efs ptr", 0);
    return;
  }

  for (rxtx = 0; rxtx < (int32)RFC_CONFIG_RXTX_MAX; rxtx++)
  {
    /* GPIO/GRFC info.  */
    efs_data_ptr->read_init_data(mode, (rfc_rxtx_enum_type)rxtx, RFC_SIG_CFG_DATA, 
                                 (uint8 **)&rfc_efs_card_wcdma_init_sig_data[rxtx]);

    /* device config info */
    efs_data_ptr->read_init_data(mode, (rfc_rxtx_enum_type)rxtx, RFC_DEVICE_CFG_DATA,
                                 (uint8 **)&rfc_efs_card_wcdma_init_device_info[rxtx]);
  }

  for (logical_path = RFM_DEVICE_0; logical_path < (int32)RFM_MAX_WAN_DEVICES; logical_path++)
  {
    for (rxtx = 0; rxtx < (int32)RFC_CONFIG_RXTX_MAX; rxtx++)
    {
      for (band = 0; band < (int32)RFCOM_NUM_WCDMA_BANDS; band++)
      {
        /* GPIO/GRFC info.  */
        efs_data_ptr->read_band_data((rfm_device_enum_type)logical_path, mode, (rfc_rxtx_enum_type)rxtx, 
                                     band, RFC_SIG_CFG_DATA,
                                     (uint8 **)&rfc_efs_card_wcdma_band_sig_data[logical_path][rxtx][band]);

        /* device config info. */
        efs_data_ptr->read_band_data((rfm_device_enum_type)logical_path, mode, (rfc_rxtx_enum_type)rxtx, 
                                     band, RFC_DEVICE_CFG_DATA,
                                     (uint8 **)&rfc_efs_card_wcdma_band_device_info[logical_path][rxtx][band]);

      } /* band */
    } /* rxtx */
  } /* logical_path */
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function verify the rfc data revision for compatibility.

  @details
  This function verify the rfc data revision for compatibility.
 
  @param
  None.
 
  @retval
  None.
*/ 
boolean rfc_efs_card_wcdma_data::verify_rfc_data(void)
{
  int32 band = 0;
  int32 logical_path = 0;
  int32 rxtx = 0;
  rfm_mode_enum_type mode = RFM_IMT_MODE;
  boolean status = TRUE;

  rfc_efs_card_cmn_data *efs_data_ptr = (rfc_efs_card_cmn_data *)rfc_common_data::get_instance();
  if (efs_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to get rfc_common_data_efs ptr", 0);
    return FALSE;
  }

  for (rxtx = 0; rxtx < (int32)RFC_CONFIG_RXTX_MAX; rxtx++)
  {
    /* GPIO/GRFC info.  */
    status &= efs_data_ptr->verify_rfc_data(RFC_SIG_CFG_DATA, (uint8 *)rfc_efs_card_wcdma_init_sig_data[rxtx]);
    /* device config info */
    status &= efs_data_ptr->verify_rfc_data(RFC_DEVICE_CFG_DATA,(uint8 *)rfc_efs_card_wcdma_init_device_info[rxtx]);
  }

  for (logical_path = RFM_DEVICE_0; logical_path < (int32)RFM_MAX_WAN_DEVICES; logical_path++)
  {
    for (rxtx = 0; rxtx < (int32)RFC_CONFIG_RXTX_MAX; rxtx++)
    {
      for (band = 0; band < (int32)RFCOM_NUM_WCDMA_BANDS; band++)
      {
        /* GPIO/GRFC info.  */
        status = efs_data_ptr->verify_rfc_data(RFC_SIG_CFG_DATA,(uint8 *)rfc_efs_card_wcdma_band_sig_data[logical_path][rxtx][band]);
        /* device config info. */
        status = efs_data_ptr->verify_rfc_data(RFC_DEVICE_CFG_DATA,(uint8 *)rfc_efs_card_wcdma_band_device_info[logical_path][rxtx][band]);

      } /* band */
    } /* rxtx */
  } /* logical_path */

  if (status == FALSE)
  {
    efs_data_ptr->set_rfc_init_fail_flag(mode);
  }

  return status;
}

#endif /* FEATURE_WCDMA */

