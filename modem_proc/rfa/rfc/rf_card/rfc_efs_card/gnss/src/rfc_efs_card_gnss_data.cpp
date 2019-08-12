/*!
   @file
   rfc_efs_card_gnss_config.cpp

   @brief
   This file contains implementation the rfc_efs_card_gnss_data class, which can be used to
   configure the rf-card related data as required.

*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/rf_card/rfc_efs_card/gnss/src/rfc_efs_card_gnss_data.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
11/07/12   sr      Initial version.

============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_efs_card_cmn_data.h" 
#include "rfc_common.h" 
#include "rfc_efs_card_gnss_data.h"


static rfc_sig_info_type *rfc_efs_card_gnss_sig_data = NULL;
static rfc_device_info_type *rfc_efs_card_gnss_device_info = NULL;


rfc_gnss_data * rfc_efs_card_gnss_data::get_instance()
{
  if (rfc_gnss_data_ptr == NULL)
  {
    rfc_gnss_data_ptr = (rfc_gnss_data *)new rfc_efs_card_gnss_data();
  }
  return( (rfc_gnss_data *)rfc_gnss_data_ptr);
}

//constructor
rfc_efs_card_gnss_data::rfc_efs_card_gnss_data()
  :rfc_gnss_data()
{
  efs_data_load(); // load the rfc data from EFS
  verify_rfc_data(); // verify the rfc data revision compatibility.
}

// Destructor
rfc_efs_card_gnss_data::~rfc_efs_card_gnss_data()
{

  // free all the dynamically allocated memory in this class 

  /* GPIO/GRFC data clean-up  */
  delete rfc_efs_card_gnss_sig_data;
  rfc_efs_card_gnss_sig_data = NULL;

  /* device cfg info data clean-up  */
  delete rfc_efs_card_gnss_device_info;
  rfc_efs_card_gnss_device_info = NULL;

  rfc_gnss_data_ptr = NULL;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_data for gnss tech based on specified configuration parameters.

  @details
  This function returns the rfc_data for gnss tech based on specified configuration parameters.
 
  @param
  'cfg' : configuration parameters for which rfc_data to be retrived.
  'ptr' : ptr to the rfc_data.
 
  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_efs_card_gnss_data::sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr )
{

  boolean ret_val = TRUE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  *ptr = (rfc_sig_cfg_type *)rfc_efs_card_gnss_sig_data->cfg_sig_list;

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the device_info table for gnss tech based on specified configuration
  parameters.

  @details
  This function returns the device_info_table for gnss tech based on specified configuration
  parameters.
 
  @param
  'cfg' : configuration parameters for which rfc_data to be retrived.
  'ptr' : ptr to the device_info_table.
 
  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_efs_card_gnss_data::devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr )
{

  boolean ret_val = TRUE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  *ptr = rfc_efs_card_gnss_device_info;

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
void rfc_efs_card_gnss_data::efs_data_load(void)
{
  rfm_mode_enum_type mode = RFM_GNSS_MODE;

  rfc_efs_card_cmn_data *efs_data_ptr = (rfc_efs_card_cmn_data *)rfc_common_data::get_instance();
  if (efs_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to get rfc_common_data_efs ptr", 0);
    return;
  }

  efs_data_ptr->read_init_data(mode, RFC_CONFIG_RX, RFC_SIG_CFG_DATA, 
                               (uint8 **)&rfc_efs_card_gnss_sig_data);
  efs_data_ptr->read_init_data(mode, RFC_CONFIG_RX, RFC_DEVICE_CFG_DATA, 
                               (uint8 **)&rfc_efs_card_gnss_device_info);
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
boolean rfc_efs_card_gnss_data::verify_rfc_data(void)
{
  rfm_mode_enum_type mode = RFM_GNSS_MODE;
  boolean status = TRUE;

  rfc_efs_card_cmn_data *efs_data_ptr = (rfc_efs_card_cmn_data *)rfc_common_data::get_instance();
  if (efs_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Failed to get rfc_common_data_efs ptr", 0);
    return FALSE;
  }

  /* GPIO/GRFC info.  */
  status &= efs_data_ptr->verify_rfc_data(RFC_SIG_CFG_DATA, (uint8 *)rfc_efs_card_gnss_sig_data);
  /* device config info */
  status &= efs_data_ptr->verify_rfc_data(RFC_DEVICE_CFG_DATA,(uint8 *)rfc_efs_card_gnss_device_info);

  if (status == FALSE)
  {
    efs_data_ptr->set_rfc_init_fail_flag(mode);
  }

  return status;
}


