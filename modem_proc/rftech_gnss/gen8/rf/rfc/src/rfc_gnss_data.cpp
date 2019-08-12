/*!
   @file
   rfc_gnss_data.cpp

   @brief
   This file contains implementation the rfc_gnss_data class, which will be used by RFC to retrieve
   the GNSS related data.

*/

/*===========================================================================

Copyright (c) 2011-13 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:49 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/gen8/rf/rfc/src/rfc_gnss_data.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
03/12/13   vrb     Adding support for getting band split configuration per RFC
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/09/12   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfc_gnss_data.h"

rfc_gnss_data *rfc_gnss_data::rfc_gnss_data_ptr = NULL;

rfc_gnss_data * rfc_gnss_data::get_instance()
{
  return(rfc_gnss_data_ptr);
}

// Constructor
rfc_gnss_data::rfc_gnss_data()
{
}

// Destructor
rfc_gnss_data::~rfc_gnss_data()
{
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_signals list for gnss tech, based on specified configuration
  parameters.

  @details
  This function returns the rfc_Signals list for gnss tech, based on specified configuration
  parameters.

  @param
  'cfg' : configuration parameters for which rfc_data to be retrived.
  'ptr' : ptr to the rfc_Signals list.

  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_gnss_data::sig_cfg_data_get(rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "GNSS RFC Signals data NOT Available", 0);
  return FALSE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_device info data for gnss tech, based on specified configuration
  parameters.

  @details
  This function returns the rfc_device info data for gnss tech, based on specified configuration
  parameters.

  @param
  'cfg' : configuration parameters for which rfc_device info data to be retrived.
  'ptr' : ptr to the rfc_device info data.

  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_gnss_data::devices_cfg_data_get(rfc_cfg_params_type *cfg, rfc_device_info_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "GNSS RFC Device info data NOT available", 0);
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
boolean rfc_gnss_data::band_split_cfg_data_get(rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr)
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
  This function returns the rf-card complete data. This should be implemented in the
  rf-card specific data file. This base-class function returns NULL.

  @details
  This function returns the rf-card complete data. This should be implemented in the
  rf-card specific data file. This base-class function returns NULL.

  @retval
  NULL ptr for the device config info.
*/
boolean rfc_gnss_data::get_rfcard_data( void )
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "get_rfcard_data is failed to collect", 0);

  return FALSE;

}

