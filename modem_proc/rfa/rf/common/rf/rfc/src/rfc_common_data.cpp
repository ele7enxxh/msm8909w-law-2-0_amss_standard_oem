/*!
   @file
   rfc_common_data.cpp

   @brief
   This file contains implementation the rfc_common_data class, which will be used by RFC to
   retrieve the rfc common related data.

*/

/*===========================================================================

Copyright (c) 2011-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/rfc/src/rfc_common_data.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
02/24/15   am      Change IRAT alt path lookup implementation
09/29/14   Saul    New alt path selection APIs.
08/08/14   tks     Modified get alt path table function param list 
07/14/14   vrb     Support for Alternate Path Selection Table
06/16/14   vv      Added functions to get physical and logical device cfg from RFC
04/23/14   tks     Support for dynamic path selection
03/17/14   kg      Port from Dime
02/11/14   sr      Re-design asdiv_tuner to support multiple RF configurations
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
05/21/13   sd      Add new api to get cmn device properties
11/26/12   sr      Added flag to capture the rfc data init status.
09/24/12   sr      Made changes to remove hand-coded RFC files.
07/18/12   sr      Changes to create devices through device factory.
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/09/12   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfc_common_data.h"

rfc_common_data *rfc_common_data::rfc_common_data_ptr = (rfc_common_data *)NULL;

rfc_common_data * rfc_common_data::get_instance(void)
{
  return rfc_common_data_ptr;
}

// Constructor
rfc_common_data::rfc_common_data(rf_hw_type rf_hw)
  :m_rf_hw(rf_hw)
{
}

// Destructor
rfc_common_data::~rfc_common_data()
{
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc data from efs file to local memory.

  @details
  This function returns the rfc data from efs file to local memory.

  @param
  rfc_info_table : rfc_info_table ptr which is read from EFS file.

  @retval
  num_rfc_signals: number of rf signals.
*/
uint32 rfc_common_data::sig_info_table_get(rfc_signal_info_type **rfc_info_table)
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFC Signals info data NOT Available (hw_id = %d)", m_rf_hw);

  return 0;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rf-card logical device configuration info. This should be implemented in the
  rf-card specific data file. This base-class function returns NULL.

  @details
  This function returns the rf-card logical device configuration info. This should be implemented in the
  rf-card specific data file. This base-class function returns NULL.

  @retval
  NULL ptr for the device config info.
*/
rfc_logical_device_info_type* rfc_common_data::get_logical_device_cfg( void )
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFC Logical device config info data NOT Available (hw_id = %d)", m_rf_hw);

  return (rfc_logical_device_info_type *)NULL;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rf-card physical device configuration info. This should be implemented in the
  rf-card specific data file. This base-class function returns NULL.

  @details
  This function returns the rf-card physical device configuration info. This should be implemented in the
  rf-card specific data file. This base-class function returns NULL.

  @retval
  NULL ptr for the device config info.
*/
rfc_phy_device_info_type* rfc_common_data::get_phy_device_cfg( void )
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFC  Physical device config info data NOT Available (hw_id = %d)", m_rf_hw);

  return (rfc_phy_device_info_type *)NULL;

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rf card common data. This should be implemented in the
  rf-card specific data file. This base-class function returns NULL.

  @details
  This function returns the rf-card complete data. This should be implemented in the
  rf-card specific data file. This base-class function returns NULL.

  @retval
  NULL ptr for the device config info.
*/
boolean rfc_common_data::get_rfcard_data( void )
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "get_rfcard_data is available to collect (hw_id = %d)", m_rf_hw);

  return FALSE;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the logical path configuration info. This should be implemented in the
  rf-card specific data file. This base-class function returns FALSE.

  @details
  This function returns the logical path configuration info. This should be implemented in the
  rf-card specific data file. This base-class function returns FALSE.

  @retval
  FALSE to indicate requested data is NOT available.
*/
boolean rfc_common_data::get_logical_path_config( rfm_devices_configuration_type* dev_cfg )
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFC Logical Path Config info data NOT Available (hw_id = %d)", m_rf_hw);

  return FALSE;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the common property info. This should be implemented in the
  rf-card specific data file. This base-class function returns FALSE.

  @details
  This function returns the common property info. This should be implemented in the
  rf-card specific data file. This base-class function returns FALSE.

  @retval
  FALSE to indicate requested data is NOT available.
*/
boolean rfc_common_data::get_cmn_properties( rfc_cmn_properties_type **ptr )
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFC Common Property info data NOT Available (hw_id = %d)", m_rf_hw);

  return FALSE;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function set the rfc init failure flag.

  @details
  This function set the rfc init failure flag. The init_failure flag is used to indicate
  un-sucessful unitialization of the rf-card.

  @retval
  None.
*/
void rfc_common_data::set_rfc_init_fail_flag(rfm_mode_enum_type mode)
{
  rfc_init_flag |= (0x1 << mode);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc init failure flag.

  @details
  This function returns the rfc init failure flag. The init_failure flag indicates
  un-sucessful unitialization of the rf-card.

  @retval
  None.
*/
uint32 rfc_common_data::get_rfc_init_fail_flag(void)
{
  return (rfc_init_flag);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to retrieve alt path selection info specific to a rf card
 
  @details
  Function returns a pointer to the alt path selection table as well as the count
  for total no of entries and size per entry. This is used by concurrency
  manager to detect the device mapping and allocate alternate path accordingly 
 
  @param *tbl_size
  pointer to get the table size or total no of entries

  @retval rfc_alt_path_sel_type
  pointer to the path selection table specific to a rf card
 
*/
rfc_alt_path_sel_type*
rfc_common_data::get_alt_path_selection_tbl
(
  uint32 *tbl_size
)
{
  /* Since there is no-valid table, all indexes are zeroed out */
  *tbl_size  = 0; 

  return NULL ; 
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to retrieve IRAT alt path selection info specific to a rf card
 
  @details
  Function returns a pointer to the IRAT specific alternate path selection
  table as well as the count for total no of entries and size per entry.
  This is used by concurrency manager to detect the device mapping and
  allocate alternate path accordingly 
 
  @param *tbl_size
  pointer to get the table size or total no of entries

  @retval rfc_alt_path_sel_type
  pointer to the path selection table specific to a rf card
 
*/
rfc_alt_path_sel_type*
rfc_common_data::get_irat_alt_path_selection_tbl
(
  uint32 *tbl_size,
  uint32 *num_band_groups
)
{
  /* Since there is no-valid table, all indexes are zeroed out */
  *tbl_size  = 0; 
  *num_band_groups  = 0;

  return NULL ; 
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns ASDiv config info. This should be implemented in the
  rf-card specific data file. This base-class function returns FALSE.

  @details
  This function returns the ASDiv config info. This should be implemented in the
  rf-card specific data file. This base-class function returns FALSE.

  @retval
  FALSE to indicate requested data is NOT available.
*/
boolean rfc_common_data::get_asd_device_info( rfc_asd_cfg_params_type *cfg, rfc_asdiv_config_info_type **ptr )
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFC ASDiv info data NOT Available (hw_id = %d)", m_rf_hw);

  return FALSE;

}

