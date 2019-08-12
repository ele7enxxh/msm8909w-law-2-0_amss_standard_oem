/*!
   @file
   rfc_wcdma_data.cpp

   @brief
   This file contains implementation the rfc_wcdma_data class, which will be used by RFC to retrieve
   the WCDMA related data.

*/

/*===========================================================================

Copyright (c) 2011-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/rfc/src/rfc_wcdma_data.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
07/08/13   sd      Adding support for WCDMA DBDC data per RFC
03/12/13   vrb     Adding support for getting band split configuration per RFC
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/09/12   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfc_wcdma_data.h"

rfc_wcdma_data *rfc_wcdma_data::rfc_wcdma_data_ptr = NULL;

rfc_wcdma_data * rfc_wcdma_data::get_instance()
{
  return(rfc_wcdma_data_ptr);
}

// Constructor
rfc_wcdma_data::rfc_wcdma_data()
{
}

// Destructor
rfc_wcdma_data::~rfc_wcdma_data()
{
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_signals list for wcdma tech, based on specified configuration
  parameters.

  @details
  This function returns the rfc_Signals list for wcdma tech, based on specified configuration
  parameters.

  @param
  'cfg' : configuration parameters for which rfc_data to be retrived.
  'ptr' : ptr to the rfc_Signals list.

  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_wcdma_data::sig_cfg_data_get(rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "WCDMA RFC Signals data NOT Available", 0);
  return FALSE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_device info data for wcdma tech, based on specified configuration
  parameters.

  @details
  This function returns the rfc_device info data for wcdma tech, based on specified configuration
  parameters.

  @param
  'cfg' : configuration parameters for which rfc_device info data to be retrived.
  'ptr' : ptr to the rfc_device info data.

  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_wcdma_data::devices_cfg_data_get(rfc_cfg_params_type *cfg, rfc_device_info_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "WCDMA RFC Device info data NOT available", 0);
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
boolean rfc_wcdma_data::band_split_cfg_data_get(rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr)
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
  This function returns the WCDMA DBDC information

  @param
  'ptr' : ptr to the rfc_device info data.

  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_wcdma_data::get_wcdma_properties(rfc_wcdma_properties_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "WCDMA RFC Device info data NOT available", 0);
  return FALSE;
}


boolean rfc_wcdma_data_get_device_info(rfc_cfg_params_type *cfg, rfc_device_info_type **device_info_pptr)
{
   rfc_wcdma_data *wcdma_data_ptr = rfc_wcdma_data::get_instance();

   if(cfg == NULL)
   {
     return FALSE;
   }

   if(device_info_pptr == NULL)
   {
     return FALSE;
   }

   if( wcdma_data_ptr->devices_cfg_data_get(cfg,device_info_pptr) == FALSE)
   {
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
boolean rfc_wcdma_data::get_rfcard_data( void )
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "get_rfcard_data is failed to collect", 0);

  return FALSE;

}

