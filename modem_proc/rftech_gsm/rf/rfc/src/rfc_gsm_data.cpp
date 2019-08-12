/*!
   @file
   rfc_gsm_data.cpp

   @brief
   This file contains implementation the rfc_gsm_data class, which will be used by RFC to retrieve
   the GSM related data.

*/

/*===========================================================================

Copyright (c) 2011-13 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:51 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/rfc/src/rfc_gsm_data.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
12/13/13   cc      Unify gpio type to rfc_sig_cfg_type for FDD and TDD tech
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
06/05/13   sd      Adding support for GSM Spur channels per RFC
03/12/13   vrb     Adding support for getting band split configuration per RFC
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/09/12   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfc_gsm_data.h"

rfc_gsm_data *rfc_gsm_data::rfc_gsm_data_ptr = NULL;

rfc_gsm_data * rfc_gsm_data::get_instance()
{
  return(rfc_gsm_data_ptr);
}

// Constructor
rfc_gsm_data::rfc_gsm_data()
{
}

// Destructor
rfc_gsm_data::~rfc_gsm_data()
{
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_signals list for GSM tech, based on specified configuration
  parameters.

  @details
  This function returns the rfc_Signals list for GSM tech, based on specified configuration
  parameters.

  @param
  'cfg' : configuration parameters for which rfc_data to be retrived.
  'ptr' : ptr to the rfc_Signals list.

  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_gsm_data::sig_cfg_data_get(rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM RFC Signals data NOT Available", 0);
  return FALSE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_device info data for GSM tech, based on specified configuration
  parameters.

  @details
  This function returns the rfc_device info data for GSM tech, based on specified configuration
  parameters.

  @param
  'cfg' : configuration parameters for which rfc_device info data to be retrived.
  'ptr' : ptr to the rfc_device info data.

  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_gsm_data::devices_cfg_data_get(rfc_cfg_params_type *cfg, rfc_device_info_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM RFC Device info data NOT available", 0);
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the band split configuration, where certain bands
  require to operate over different physical paths in the RF card design.

  @param
  'cfg' : configuration parameters for which rfc_device info data to be retrived.
  'ptr' : ptr to the rfc_device info data.

  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_gsm_data::band_split_cfg_data_get(rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "CDMA RFC Device info data NOT available", 0);
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the GSM Spur channel information

  @param
  'ptr' : ptr to the rfc_device info data.

  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_gsm_data::get_gsm_properties(rfc_gsm_properties_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM RFC Device info data NOT available", 0);
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rf-card complete data. This should be implemented in the
  rf-card specific data file. This base-class function returns NULL.

  @details
  This function returns the rf-card complete data. This should be implemented in the
  rf-card specific data file. This base-class function returns NULL.

  @retval
  NULL ptr for the device config info.
*/
boolean rfc_gsm_data::get_rfcard_data( void )
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "get_rfcard_data is failed to collect", 0);

  return FALSE;

}
