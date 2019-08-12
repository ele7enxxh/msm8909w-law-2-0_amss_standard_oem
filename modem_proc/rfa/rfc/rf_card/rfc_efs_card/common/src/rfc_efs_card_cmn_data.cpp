/*!
   @file
   rfc_efs_card_cmn_data.cpp

   @brief
   This file contains implementation the rfc_efs_card_cmn_data class, which can be used to
   configure the rf-card related data as required.

*/

/*===========================================================================

Copyright (c) 2011-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/rf_card/rfc_efs_card/common/src/rfc_efs_card_cmn_data.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
08/20/14   cd      Fixed compiler warnings
07/21/14   cd      Updates to common EFS functionality to support devices
05/09/14   Saul    Fix EFS RFC
03/14/14   Saul    Added RF_HW_EFS_CARD
09/05/13   sd      Replace banned APIs
07/28/13   jr      Support for concurrency restriction, common properties and
                   GSM spur desense channels.
12/21/12   sr      klockwork error fix.
11/07/12   sr      Initial version.

============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include <stringl/stringl.h>
#include "rfc_msm_signal_info_ag.h" 
#include "rfc_efs_card_cmn_data.h" 
#include "DDITlmm.h" 
#include "rfc_common.h" 
#include "rfcommon_efs.h"
#include "rfcommon_core_sbi.h" 

#ifdef FEATURE_GSM
#include "rfc_efs_card_gsm_data.h" 
#endif 

#ifdef FEATURE_WCDMA
#include "rfc_efs_card_wcdma_data.h" 
#endif 

#ifdef FEATURE_TDSCDMA
#include "rfc_efs_card_tdscdma_data.h" 
#endif 

#ifdef FEATURE_CDMA1X
#include "rfc_efs_card_cdma_data.h" 
#endif 

#ifdef FEATURE_LTE
#include "rfc_efs_card_lte_data.h" 
#endif 

#ifdef FEATURE_CGPS
#include "rfc_efs_card_gnss_data.h" 
#endif 


extern "C" {
#include "msg.h"
#include "modem_mem.h"
#include <stdio.h>  
int snprintf(char *str,     size_t size,    const char *format, ...);

} /* extern "C" */


static const char *rfc_efs_rxtx_to_string[RFC_CONFIG_RXTX_INVALID] = {"rx", "tx"};
static const char *rfc_efs_mode_to_string[RFM_NUM_MODES] = 
{
  "park",            // RFM_PARKED_MODE
  "wcdma",           // RFM_IMT_MODE
  "gsm",             // RFM_EGSM_MODE
  "cdma",              // RFM_1X_MODE,
  "gps",             // RFM_GPS_MODE
  "1xevdo",          // RFM_1XEVDO_MODE
  "svdo",            // RFM_SVDO_MODE
  "gnss",            // RFM_GNSS_MODE
  "lte",             // RFM_LTE_MODE
  "lte_ftm",         // RFM_LTE_FTM_MODE
  "tdscdma"          // RFM_TDSCDMA_MODE
};


static rfc_phy_device_info_type *rfc_efs_card_cmn_phy_devices_list = NULL; 
static rfc_logical_device_info_type *rfc_efs_card_cmn_logical_devices_list = NULL; 
static rfc_signal_info_type *rfc_efs_card_cmn_sig_info = NULL;
static int32 rfc_efs_card_sig_info_num = 0;
static rfm_devices_configuration_type *rfc_efs_card_cmn_logical_device_properties = NULL;
static rfc_cmn_properties_type *rfc_efs_card_cmn_properties = NULL;
static rfm_concurrency_restriction_type *rfc_efs_card_cmn_restriction_table = NULL;


rfc_common_data* rfc_efs_card_cmn_data::get_instance(rf_hw_type rf_hw)
{
  if (rfc_common_data_ptr == NULL && rf_hw != RF_HW_UNDEFINED)
  {
    rfc_common_data_ptr = (rfc_common_data *)new rfc_efs_card_cmn_data(rf_hw);
  }
  return( (rfc_common_data *)rfc_common_data_ptr);
}


//constructor
rfc_efs_card_cmn_data::rfc_efs_card_cmn_data(rf_hw_type rf_hw)
:rfc_common_data(rf_hw)
{
  (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), " "); 

  common_data_load();

}

// Destructor
rfc_efs_card_cmn_data::~rfc_efs_card_cmn_data()
{
  rfc_efs_card_sig_info_num = 0;
  if (rfc_efs_card_cmn_phy_devices_list != NULL)
  {
    delete rfc_efs_card_cmn_phy_devices_list;
    rfc_efs_card_cmn_phy_devices_list = NULL;
  }

  if (rfc_efs_card_cmn_logical_devices_list != NULL)
  {
    delete rfc_efs_card_cmn_logical_devices_list;
    rfc_efs_card_cmn_logical_devices_list = NULL;
  }

  if (rfc_efs_card_cmn_sig_info != NULL)
  {
    delete rfc_efs_card_cmn_sig_info;
    rfc_efs_card_cmn_sig_info = NULL;
  }

  if (rfc_efs_card_cmn_logical_device_properties != NULL)
  {
    delete rfc_efs_card_cmn_logical_device_properties;
    rfc_efs_card_cmn_logical_device_properties = NULL;
  }

  if (rfc_efs_card_cmn_properties != NULL)
  {
    delete rfc_efs_card_cmn_properties;
    rfc_efs_card_cmn_properties = NULL;
  }

  if (rfc_efs_card_cmn_restriction_table != NULL)
  {
    delete rfc_efs_card_cmn_restriction_table;
    rfc_efs_card_cmn_restriction_table = NULL;
  }
    
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the common rfc data. And creates the supported techs rfc data objects.

  @details
  This function returns the common rfc data. And creates the supported techs rfc data objects.
 
  @param
  rfc_info_table : common rfc_info_table ptr. 
 
  @retval
  num_rfc_signals: number of rf signals.
*/
uint32 rfc_efs_card_cmn_data::sig_info_table_get(rfc_signal_info_type **rfc_info_table)
{
  if (NULL == rfc_info_table)
  {
    return 0;
  }

  *rfc_info_table = rfc_efs_card_cmn_sig_info;

  #ifdef FEATURE_GSM
  // Create GSM RFC AG Data Object
  rfc_gsm_data *rfc_gsm_data = rfc_efs_card_gsm_data::get_instance(); 
  if (rfc_gsm_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_GSM */

  #ifdef FEATURE_WCDMA
  // Create WCDMA RFC AG Data Object
  rfc_wcdma_data *rfc_wcdma_data = rfc_efs_card_wcdma_data::get_instance(); 
  if (rfc_wcdma_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_WCDMA */

  #ifdef FEATURE_CDMA1X
  // Create CDMA RFC AG Data Object
  rfc_cdma_data *rfc_cdma_data = rfc_efs_card_cdma_data::get_instance(); 
  if (rfc_cdma_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC CDMA Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_CDMA */

  #ifdef FEATURE_LTE
  // Create LTE RFC AG Data Object
  rfc_lte_data *rfc_lte_data = rfc_efs_card_lte_data::get_instance(); 
  if (rfc_lte_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_LTE */

  #ifdef FEATURE_TDSCDMA
  // Create TDSCDMA RFC AG Data Object
  rfc_tdscdma_data *rfc_tdscdma_data = rfc_efs_card_tdscdma_data::get_instance(); 
  if (rfc_tdscdma_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_TDSCDMA */

  #ifdef FEATURE_CGPS
  // Create GNSS RFC AG Data Object
  rfc_gnss_data *rfc_gnss_data = rfc_efs_card_gnss_data::get_instance(); 
  if (rfc_gnss_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GNSS Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_GNSS */

  return rfc_efs_card_sig_info_num;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the device configuration table.

  @details
  This function returns the device configuration table.
 
  @param None
 
  @retval
  rfc_device_cfg_info_type: rf device configuration list.
*/
rfc_phy_device_info_type* rfc_efs_card_cmn_data::get_phy_device_cfg( void )
{
  return (&rfc_efs_card_cmn_phy_devices_list[0]);
}

rfc_logical_device_info_type* rfc_efs_card_cmn_data::get_logical_device_cfg( void )
{
  return (&rfc_efs_card_cmn_logical_devices_list[0]);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the logical path configuration table.

  @details
  This function returns the logical path configuration table.
 
  @param
  dev_cfg : ptr to the logical path configuration data to be set.
 
  @retval
  boolean: TRUE if successful, else FALSE.
*/
boolean rfc_efs_card_cmn_data::get_logical_path_config(rfm_devices_configuration_type* dev_cfg)
{
  if ( dev_cfg == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfm_get_devices_configuration: Invalid Container", 0);
    return FALSE;
  }

  if(rfc_efs_card_cmn_restriction_table != NULL)
  {
     rfc_efs_card_cmn_logical_device_properties->concurrency_restrictions.restriction_table = rfc_efs_card_cmn_restriction_table;
  }

  memscpy(dev_cfg,
          sizeof(rfm_devices_configuration_type),
          rfc_efs_card_cmn_logical_device_properties,
          sizeof(rfm_devices_configuration_type));

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the common properties.

  @details
  This function returns the common properties.
 
  @param
  dev_cfg : ptr to the logical path configuration data to be set.
 
  @retval
  boolean: TRUE if successful, else FALSE.
*/

boolean rfc_efs_card_cmn_data::get_cmn_properties(rfc_cmn_properties_type **ptr)
{
  if (NULL==ptr)
  {
    return FALSE;
  }

  if(rfc_efs_card_cmn_properties != NULL)
  {
     *ptr = rfc_efs_card_cmn_properties;
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC common Property info data NOT Available from efs", 0);
    return FALSE;
  }
  return TRUE;
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
void rfc_efs_card_cmn_data::common_data_load(void)
{
  int32 num_bytes_read = 0;
  rfm_mode_enum_type mode = RFM_PARKED_MODE;
  boolean status = TRUE;

  // load and verify the rfc_signal_info data.
  (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), "%s/%04d/common/rfc_cmn_sig_info.dat", 
                        RFC_EFS_FILE_PATH, m_rf_hw); 
  num_bytes_read = read(m_rfc_file_name, (uint8 **)&rfc_efs_card_cmn_sig_info);
  if (rfc_efs_card_cmn_sig_info == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC EFS SIG INFO Object is NULL", 0);
    status = FALSE;
    return;
  }

  if (num_bytes_read != 0) // make sure that we have sucessfully read the efs data
  {
    /* Subtract 1 to not count the last row which is usually "invalid" elements row
       to signify table end. */
    rfc_efs_card_sig_info_num = (num_bytes_read/sizeof(rfc_signal_info_type)) - 1;

    if (rfc_efs_card_sig_info_num > 0) // and we have rf signals.
    {
      // check for revision compatibility
      if ( (rfc_efs_card_cmn_sig_info[rfc_efs_card_sig_info_num].init_state & RFC_MAJOR_REV_BMSK) != RFC_MSM_SIG_INFO_REV) // major rev
      {
        status = FALSE;
      }
    }
    else
    {
      status = FALSE;
    }
  }
  else
  {
    status = FALSE;
  }


  // load and verify the rfc_phy_device_info_type data
  (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), "%s/%04d/common/rfc_cmn_phy_devices_list.dat", 
                        RFC_EFS_FILE_PATH, m_rf_hw); 

  num_bytes_read = read(m_rfc_file_name, (uint8 **)&rfc_efs_card_cmn_phy_devices_list);

  // load and verify the rfc_logical_device_info_type data
  (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), "%s/%04d/common/rfc_cmn_logical_devices_list.dat", 
                        RFC_EFS_FILE_PATH, m_rf_hw); 

  num_bytes_read = read(m_rfc_file_name, (uint8 **)&rfc_efs_card_cmn_logical_devices_list);

    // load and verify the rfm cmn properties data
  (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), "%s/%04d/common/rfc_cmn_restriction_table.dat", 
                        RFC_EFS_FILE_PATH, m_rf_hw); 

  num_bytes_read = read(m_rfc_file_name, (uint8 **)&rfc_efs_card_cmn_restriction_table);

  // load and verify the rfm device properties data
  (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), "%s/%04d/common/rfc_cmn_logical_device_properties.dat", 
                        RFC_EFS_FILE_PATH, m_rf_hw); 

  num_bytes_read = read(m_rfc_file_name, (uint8 **)&rfc_efs_card_cmn_logical_device_properties);

   // load and verify the rfm cmn properties data
  (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), "%s/%04d/common/rfc_cmn_properties.dat", 
                        RFC_EFS_FILE_PATH, m_rf_hw); 

  num_bytes_read = read(m_rfc_file_name, (uint8 **)&rfc_efs_card_cmn_properties);

  if (status == FALSE)
  {
    set_rfc_init_fail_flag(mode);
    rfc_efs_card_sig_info_num = 0; // indicates that there are no valid rf signals.
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function reads the rfc init data from efs file system.

  @details
  This function reads the rfc init data from efs file system for the specified rx_tx configuration.
 
  @param
  'rx_tx' : rx or tx configuration to read the efs data for.
  'rfc_data' : ptr to the data which has been read from efs file.
 
  @retval
  number of bytes read from efs file.
*/ 
int32 rfc_efs_card_cmn_data::read_init_data(rfm_mode_enum_type mode, rfc_rxtx_enum_type rx_tx, 
                                            rfc_data_enum_type rfc_data_type, uint8 **rfc_data)
{
  int32 num_bytes_read = 0;
  boolean status = FALSE;

  // construct the efs file name based on 'm_mode'  and 'rx_tx' and the resulting file name
  // will be in 'm_rfc_file_name'.
  status = construct_file_name(mode, rx_tx, rfc_data_type);
  if (status)
  {
    num_bytes_read = read(m_rfc_file_name, rfc_data);
  }

  return num_bytes_read;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function reads the rfc tech specific data from efs file system.

  @details
  This function reads the tech specific data from efs file system for the speicific technology
 
  @param
  'mode' : mode to read the efs data for.
  'rfc_data' : ptr to the data which has been read from efs file.
 
  @retval
  number of bytes read from efs file.
*/ 
int32 rfc_efs_card_cmn_data::read_tech_specific_data(rfm_mode_enum_type mode,rfc_data_enum_type rfc_data_type, 
                                                     uint8 **rfc_data)
{
  int32 num_bytes_read = 0;
  boolean status = FALSE;

  // construct the efs file name based on 'm_mode'  and 'rx_tx' and the resulting file name
  // will be in 'm_rfc_file_name'.
  status = construct_file_name(mode,rfc_data_type);
  if (status)
  {
    num_bytes_read = read(m_rfc_file_name, rfc_data);
  }

  return num_bytes_read;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function reads the rfc band data from efs file system.

  @details
  This function reads the rfc band data from efs file system for the specified path, rx_tx and band
  configuration.
 
  @param
  'path' : rf path
  'rx_tx' : rx or tx configuration to read the efs data for.
  'band' : rf band
  'rfc_data' : ptr to the data which has been read from efs file.
 
  @retval
  number of bytes read from efs file.
*/ 
int32 rfc_efs_card_cmn_data::read_band_data(rfm_device_enum_type path, rfm_mode_enum_type mode, 
                                            rfc_rxtx_enum_type rx_tx, int band, 
                                            rfc_data_enum_type rfc_data_type, uint8 **rfc_data)
{
  int32 num_bytes_read = 0;
  boolean status = FALSE;

  // construct the efs file name based on m_mode, path, rx_tx and band, the resulting file name
  // will be in 'm_rfc_file_name'.
  status = construct_file_name(path, mode, rx_tx, band, rfc_data_type);
  if (status)
  {
    num_bytes_read = read(m_rfc_file_name, rfc_data);
  }

  return num_bytes_read;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function reads the rfc band data from efs file system.

  @details
  This function reads the rfc band data from efs file system for the specified path, rx_tx and band
  configuration.
 
  @param
  'rfc_data_type' : type of RFC data (GPIO/GRFC data or Device Config data).
  'rfc_data' : ptr to the data which has been read from efs file.
 
  @retval
  number of bytes read from efs file.
*/ 
boolean rfc_efs_card_cmn_data::verify_rfc_data(rfc_data_enum_type rfc_data_type, uint8 *rfc_data)
{
  boolean status = TRUE;
  rfc_sig_info_type *rfc_sig_data = NULL;
  rfc_device_info_type *rfc_dev_info_data = NULL;
  rfc_gsm_properties_type *rfc_gsm_properties_data = NULL;
  rfc_band_split_info_type *rfc_split_info_data = NULL;

  // if the rfc data is NULL, we assume that it is a valid case where some
  // bands or techs may not be supported for a rf-card.
  if (rfc_data == NULL) 
  {
    return status;
  }

  // if the data is available we should make sure that the revision is compatible.
  if (rfc_data_type == RFC_SIG_CFG_DATA)
  {
    rfc_sig_data = (rfc_sig_info_type *)rfc_data;
    if ( (rfc_sig_data->rfc_revision & RFC_MAJOR_REV_BMSK) != RFC_MSM_SIG_INFO_REV) // major rev
    {
      status = FALSE;
    }
  }
  else if (rfc_data_type == RFC_DEVICE_CFG_DATA)
  {
    rfc_dev_info_data = (rfc_device_info_type *)rfc_data;
    if ( (rfc_dev_info_data->rfc_revision & RFC_MAJOR_REV_BMSK) != RFC_MSM_SIG_INFO_REV) // major rev
    {
      status = FALSE;
    }
  }
  else if (rfc_data_type == RFC_PROPERTIES_DATA)
  {
    rfc_gsm_properties_data  = (rfc_gsm_properties_type *)rfc_data;
    if ( (rfc_gsm_properties_data->rfc_revision & RFC_MAJOR_REV_BMSK) != RFC_MSM_SIG_INFO_REV) // major rev
    {
      status = FALSE;
    }
  }
  else if (rfc_data_type == RFC_SPLIT_CFG_DATA)
  {
    rfc_split_info_data = (rfc_band_split_info_type *)rfc_data;
    if ( (rfc_split_info_data->rfc_revision & RFC_MAJOR_REV_BMSK) != RFC_MSM_SIG_INFO_REV) // major rev
    {
      status = FALSE;
    }
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function efs file name for the init data.

  @details
  This function efs file name for the init data.
 
  @param
  'rx_tx' : rx or tx configuration to read the efs data for.
 
  @retval
  TRUE - efs file has contructed, FALSE otherwise.
*/
boolean rfc_efs_card_cmn_data::construct_file_name(rfm_mode_enum_type mode, rfc_rxtx_enum_type rx_tx,
                                                   rfc_data_enum_type rfc_data_type)
{
  const char RFC_SIG_CFG_DATA_FILE[] =  "init_sig_cfg" ;   // GPIO GRFC signal data
  const char RFC_DEVICE_CFG_DATA_FILE[] =  "init_device_info" ;   // Device info data

  if (mode >= RFM_NUM_MODES || rx_tx >= RFC_CONFIG_RXTX_INVALID || 
      rfc_data_type >= RFC_DATA_TYPE_MAX)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid parameters(m_mode=%d, rx_tx=%d, rfc_data=%d)", 
          mode, rx_tx, rfc_data_type);
    return FALSE;
  }

  if (RFC_SIG_CFG_DATA == rfc_data_type)
  {
    (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), "%s/%04d/%s/%s_%s_%s.dat", 
                    RFC_EFS_FILE_PATH, m_rf_hw, rfc_efs_mode_to_string[mode], 
                    rfc_efs_rxtx_to_string[rx_tx], rfc_efs_mode_to_string[mode],
                    RFC_SIG_CFG_DATA_FILE); 
  }
  else if (RFC_DEVICE_CFG_DATA == rfc_data_type)
  {
    (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), "%s/%04d/%s/%s_%s_%s.dat", 
                    RFC_EFS_FILE_PATH, m_rf_hw, rfc_efs_mode_to_string[mode], 
                    rfc_efs_rxtx_to_string[rx_tx], rfc_efs_mode_to_string[mode],
                    RFC_DEVICE_CFG_DATA_FILE); 
  }

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function efs file name for the tech based data

  @details
  This function efs file name for the tech based data.
 
  @param
  'mode' : mode to read the efs data for.
 
  @retval
  TRUE - efs file has contructed, FALSE otherwise.
*/
boolean rfc_efs_card_cmn_data::construct_file_name(rfm_mode_enum_type mode, rfc_data_enum_type rfc_data_type)
{
  const char RFC_PROPERTIES_DATA_FILE[] =  "properties" ;   // GSM PROPERTIES


  if (mode >= RFM_NUM_MODES || rfc_data_type >= RFC_DATA_TYPE_MAX)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid parameters(m_mode=%d, rfc_data=%d)", mode, rfc_data_type);
    return FALSE;
  }

  if (RFC_PROPERTIES_DATA == rfc_data_type)
  {
    (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), "%s/%04d/%s%s_%s_%s.dat", 
                    RFC_EFS_FILE_PATH, m_rf_hw, rfc_efs_mode_to_string[mode],RFC_EFS_FILE_PATH, rfc_efs_mode_to_string[mode],RFC_PROPERTIES_DATA_FILE); 

  }

  return TRUE;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function efs file name for the band data.

  @details
  This function efs file name for the band data.
 
  @param
  'path' : rf path
  'rx_tx' : rx or tx configuration to read the efs data for.
  'band' : rf band
 
  @retval
  TRUE - efs file has contructed, FALSE otherwise.
*/
boolean rfc_efs_card_cmn_data::construct_file_name(rfm_device_enum_type path, rfm_mode_enum_type mode, 
                                                   rfc_rxtx_enum_type rx_tx, int band, 
                                                   rfc_data_enum_type rfc_data_type)
{
  const char RFC_SIG_CFG_DATA_FILE[] =  "sig_cfg" ;   // GPIO GRFC signal data
  const char RFC_DEVICE_CFG_DATA_FILE[] =  "device_info" ;   // Device info data
  const char RFC_SPLIT_CFG_DATA_FILE[] =  "split_cfg" ;   // Device info data

  if (mode >= RFM_NUM_MODES || rx_tx >= RFC_CONFIG_RXTX_INVALID || 
      rfc_data_type >= RFC_DATA_TYPE_MAX)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid parameters(m_mode=%d, rx_tx=%d, rfc_data=%d)", 
          mode, rx_tx, rfc_data_type);
    return FALSE;
  }

  if (RFC_SIG_CFG_DATA == rfc_data_type)
  {
    (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), "%s/%04d/%s/%s%01d_%s_%02d_%s.dat", 
                    RFC_EFS_FILE_PATH, m_rf_hw, rfc_efs_mode_to_string[mode], 
                    rfc_efs_rxtx_to_string[rx_tx], path, 
                    rfc_efs_mode_to_string[mode], band, RFC_SIG_CFG_DATA_FILE); 
  }
  else if (RFC_DEVICE_CFG_DATA == rfc_data_type)
  {
    (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), "%s/%04d/%s/%s%01d_%s_%02d_%s.dat", 
                    RFC_EFS_FILE_PATH, m_rf_hw, rfc_efs_mode_to_string[mode], 
                    rfc_efs_rxtx_to_string[rx_tx], path, 
                    rfc_efs_mode_to_string[mode], band, RFC_DEVICE_CFG_DATA_FILE); 
  }
  else if (RFC_SPLIT_CFG_DATA == rfc_data_type)
  {
    (void) snprintf(m_rfc_file_name, sizeof(m_rfc_file_name), "%s/%04d/%s/%s%01d_%s_%02d_%s.dat", 
                    RFC_EFS_FILE_PATH, m_rf_hw, rfc_efs_mode_to_string[mode], 
                    rfc_efs_rxtx_to_string[rx_tx], path, 
                    rfc_efs_mode_to_string[mode], band, RFC_SPLIT_CFG_DATA_FILE); 
  }

  return TRUE;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function reads the specified file from EFS and copies the data to specified location.

  @details
  This function reads the specified file from EFS and copies the data to specified location.
  It dynamically allocates the memory required to store the data. And the calling function is
  responsible for de-allocating the memory when not needed.
 
  @param
  'file_name' : efs file to read
  'rfc_data' : data to be copied into.
 
  @retval
  TRUE - efs file has contructed, FALSE otherwise.
*/
int32 rfc_efs_card_cmn_data::read(char *file_name, uint8 **rfc_data)
{
  int32 data_size = 0;
  uint8* buffer = NULL;
  boolean api_status = TRUE;

  if (NULL == rfc_data)
  {
    return data_size;
  }

  /* Allocate gen purpose mem */
  buffer = (uint8*) modem_mem_alloc( RFC_EFS_MAX_DATA_SIZE, MODEM_MEM_CLIENT_RFA );

  if(buffer == NULL)
  {
    MSG_SPRINTF_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "EFS Data Read - Not able to alloc temp mem size of %d."
                  " Cannot use EFS RFC.", RFC_EFS_MAX_DATA_SIZE);
    api_status = FALSE;
    return 0;
  }

  /* read the file into buffer */
  data_size =  rfcommon_efs_get((const char*)file_name, (void*)buffer, RFC_EFS_MAX_DATA_SIZE);
  if (data_size > RFC_EFS_MAX_DATA_SIZE) // to fix the KW errors
  {
    MSG_SPRINTF_3(MSG_SSID_RF, MSG_LEGACY_ERROR, "File size is larger than the limit! file= %s, size=%d limit=%d", 
                  file_name, data_size, RFC_EFS_MAX_DATA_SIZE);
    data_size = 0;
    api_status = FALSE;
  }

  if ( (data_size != 0) && (api_status == TRUE))
  {
    *rfc_data = (uint8 *)modem_mem_alloc( data_size, MODEM_MEM_CLIENT_RFA);
    if (*rfc_data != NULL)
    {
      memscpy(*rfc_data, data_size, buffer, data_size);
    }
    else
    {
      MSG_SPRINTF_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Memory Allocation failed for rfc data! file= %s, size=%d", 
                    file_name, data_size);
      data_size = 0; /* Reset to ZERO since malloc failed */
      api_status = FALSE;
    }
  }
  else
  {
    *rfc_data = NULL;
  }

  if (buffer != NULL)
  {
    modem_mem_free( buffer, MODEM_MEM_CLIENT_RFA );
    buffer = NULL;
  }

  return data_size;
}


