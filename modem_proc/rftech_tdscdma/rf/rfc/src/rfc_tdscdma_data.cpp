/*!
   @file
   rfc_tdscdma_data.cpp

   @brief
   This file contains implementation the rfc_tdscdma_data class, which will be used by RFC to retrieve
   the TDSCDMA related data.

*/

/*===========================================================================

Copyright (c) 2011-13 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/rfc/src/rfc_tdscdma_data.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
12/13/13   cc      Unify gpio type to rfc_sig_cfg_type for FDD and TDD tech
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
05/20/13   al      Fixed some typos during porting
03/12/13   vrb     Adding support for getting band split configuration per RFC
09/27/12   jyu     Added support to query rfdevice_info_ptr
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/09/12   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfc_tdscdma_data.h"

rfc_tdscdma_data *rfc_tdscdma_data::rfc_tdscdma_data_ptr = NULL;

rfc_tdscdma_data * rfc_tdscdma_data::get_instance()
{
  return(rfc_tdscdma_data_ptr);
}

// Constructor
rfc_tdscdma_data::rfc_tdscdma_data()
{
}

// Destructor
rfc_tdscdma_data::~rfc_tdscdma_data()
{
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_signals list for TDSCDMA tech, based on specified configuration
  parameters.

  @details
  This function returns the rfc_Signals list for TDSCDMA tech, based on specified configuration
  parameters.

  @param
  'cfg' : configuration parameters for which rfc_data to be retrived.
  'ptr' : ptr to the rfc_Signals list.

  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_tdscdma_data::sig_cfg_data_get(rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "TDSCDMA RFC Signals data NOT Available", 0);
  return FALSE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_device info data for TDSCDMA tech, based on specified configuration
  parameters.

  @details
  This function returns the rfc_device info data for TDSCDMA tech, based on specified configuration
  parameters.

  @param
  'cfg' : configuration parameters for which rfc_device info data to be retrived.
  'ptr' : ptr to the rfc_device info data.

  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_tdscdma_data::devices_cfg_data_get(rfc_cfg_params_type *cfg, rfc_device_info_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "TDSCDMA RFC Device info data NOT available", 0);
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
boolean rfc_tdscdma_data::band_split_cfg_data_get(rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "TDSCDMA RFC Device info data NOT available", 0);
  return FALSE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_device info data for TDSCDMA tech, based on specified configuration
  parameters.

  @details
  This function returns the rfc_device info data for TDSCDMA tech, based on specified configuration
  parameters.

  @param
  'cfg' : configuration parameters for which rfc_device info data to be retrived.
  'ptr' : ptr to the rfc_device info data.

  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_tdscdma_get_devices_info_data(rfc_cfg_params_type *cfg, rfc_device_info_type **ptr)
{
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();
  boolean result;

  if(cfg == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Can't get devices_info_data because cfg is NULL", 0);
    return FALSE;
  }

  if(ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Can't get devices_info_data because ptr is NULL", 0);
    return FALSE;
  }

  if(rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Can't get devices_info_data because rfc_data is NULL", 0);
    return FALSE;
  }

  result = rfc_data->devices_cfg_data_get(cfg, ptr);

  if( result == FALSE)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Can't get devices_info_data because return failure", 0);
    return FALSE;
  }

  return TRUE;
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
boolean rfc_tdscdma_data::get_rfcard_data( void )
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "get_rfcard_data is failed to collect", 0);

  return FALSE;

}
