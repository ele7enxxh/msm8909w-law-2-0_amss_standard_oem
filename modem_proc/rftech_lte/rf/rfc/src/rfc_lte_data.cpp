/*!
   @file
   rfc_lte_data.cpp

   @brief
   This file contains implementation the rfc_lte_data class, which will be used by RFC to retrieve
   the LTE related data.

*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/rf/rfc/src/rfc_lte_data.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
11/10/15   fhuo    Add get_lte_properties() for LTE partial band support
09/23/14   bar     Fixed crash in Jolokia CA mode
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
04/25/13   gvn     Support for Split bands for LTE
04/01/13   sd      Adding support for SCELL Log Path Swap API
03/12/13   vrb     Adding support for getting band split configuration per RFC
10/11/12   vrb     Added interface to get generated timing information
08/28/12   pl/sbm  Added C function to retrieve device info
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/09/12   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfc_lte_data.h"

rfc_lte_data *rfc_lte_data::rfc_lte_data_ptr = NULL;

rfc_lte_data * rfc_lte_data::get_instance()
{
  return(rfc_lte_data_ptr);
}

// Constructor 
rfc_lte_data::rfc_lte_data()
{
}

// Destructor
rfc_lte_data::~rfc_lte_data()
{
}
 
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_signals list for LTE tech, based on specified configuration
  parameters.

  @details
  This function returns the rfc_Signals list for LTE tech, based on specified configuration
  parameters.
 
  @param
  'cfg' : configuration parameters for which rfc_data to be retrived.
  'ptr' : ptr to the rfc_Signals list.
 
  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_lte_data::sig_cfg_data_get(rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "LTE RFC Signals data NOT Available", 0);
  return FALSE;
}
 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_device info data for LTE tech, based on specified configuration
  parameters.

  @details
  This function returns the rfc_device info data for LTE tech, based on specified configuration
  parameters.
 
  @param
  'cfg' : configuration parameters for which rfc_device info data to be retrived.
  'ptr' : ptr to the rfc_device info data.
 
  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_lte_data::devices_cfg_data_get(rfc_cfg_params_type *cfg, rfc_device_info_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "LTE RFC Device info data NOT available", 0);
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
boolean rfc_lte_data::band_split_cfg_data_get(rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "LTE RFC band split data NOT available", 0);
  return FALSE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the SCELL log path swap data
 
  @param
  'ptr' : ptr to the rfc_band_pair_list_type data.
 
  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_lte_data::ca_scell_log_path_swap_data_get(rfc_band_pair_list_type **ptr)
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "LTE RFC Path Swap info data NOT available", 0);
  if (NULL==ptr)
  {
   return FALSE;
  }
  *ptr = NULL;
  return FALSE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the timing information for LTE Tech
 
  @details
  This function returns the timing information for LTE Tech
 
  @param
  'cfg' : configuration parameters for which rfc_device info data to be retrived.
  'ptr' : ptr to the rfc_device info data.
 
  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_lte_data::timing_cfg_data_get( rfc_cfg_params_type *cfg, rfc_timing_info_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "LTE RFC Timing info data NOT available", 0);
  return FALSE;
}
 

boolean rfc_lte_data_get_device_info(rfc_cfg_params_type *cfg, rfc_device_info_type **device_info_pptr)
{
   rfc_lte_data *lte_data_ptr = rfc_lte_data::get_instance();
    
   if(cfg == NULL)
   {
     return FALSE;
   }

   if(device_info_pptr == NULL)
   {
     return FALSE;
   }
 
   if( lte_data_ptr->devices_cfg_data_get(cfg,device_info_pptr) == FALSE)
   {
     return FALSE;
   }
   
   return TRUE;
}

boolean rfc_lte_data_get_timing_info(rfc_cfg_params_type *cfg, rfc_timing_info_type **rfc_timing_info_pptr)
{
   rfc_lte_data *lte_data_ptr = rfc_lte_data::get_instance();
    
   if(cfg == NULL)
   {
     return FALSE;
   }

   if(rfc_timing_info_pptr == NULL)
   {
     return FALSE;
   }
 
   if( lte_data_ptr->timing_cfg_data_get(cfg,rfc_timing_info_pptr) == FALSE)
   {
     return FALSE;
   }
   
   return TRUE;
}


/*!
  @brief
  This function returns the LTE downlink partial supported band information

  @param
  'ptr' : ptr to the rfc_device info data.

  @retval
  TRUE - if the partial band rfc_data is available or FALSE if error.
*/
boolean rfc_lte_data::get_lte_properties(rfc_lte_properties_type **ptr)
{
  if (NULL != ptr)
  {
    *ptr = NULL;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "LTE RFC Partial DL Band info data NOT available", 0);
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
boolean rfc_lte_data::get_rfcard_data( void )
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "get_rfcard_data is failed to collect", 0);

  return FALSE;

}