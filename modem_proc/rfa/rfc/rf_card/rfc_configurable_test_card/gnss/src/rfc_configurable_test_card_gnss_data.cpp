/*!
   @file
   rfc_configurable_test_card_gnss_config.cpp

   @brief
   This file contains implementation the rfc_configurable_test_card_gnss_data class, which can be used to
   configure the rf-card related data as required.

*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/rf_card/rfc_configurable_test_card/gnss/src/rfc_configurable_test_card_gnss_data.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
02/09/13   sr      Added Destructor.
11/07/12   sr      Initial version.

============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_configurable_test_card_cmn_data.h" 
#include "rfc_common.h" 
#include "rfc_configurable_test_card_gnss_data.h"


static rfc_sig_info_type *rfc_configurable_test_card_gnss_sig_data = NULL;
static rfc_device_info_type *rfc_configurable_test_card_gnss_device_info = NULL;


rfc_gnss_data * rfc_configurable_test_card_gnss_data::get_instance()
{
  if (rfc_gnss_data_ptr == NULL)
  {
    rfc_gnss_data_ptr = (rfc_gnss_data *)new rfc_configurable_test_card_gnss_data();
  }
  return( (rfc_gnss_data *)rfc_gnss_data_ptr);
}

//constructor
rfc_configurable_test_card_gnss_data::rfc_configurable_test_card_gnss_data()
  :rfc_gnss_data()
{
}

// Destructor
rfc_configurable_test_card_gnss_data::~rfc_configurable_test_card_gnss_data()
{
  // nothing to do for now
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
boolean rfc_configurable_test_card_gnss_data::sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr )
{

  boolean ret_val = TRUE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  *ptr = (rfc_sig_cfg_type *)rfc_configurable_test_card_gnss_sig_data->cfg_sig_list;

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
boolean rfc_configurable_test_card_gnss_data::devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr )
{

  boolean ret_val = TRUE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  *ptr = rfc_configurable_test_card_gnss_device_info;

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the rfc_data for gnss tech based on specified configuration parameters.

  @details
  This function sets the rfc_data for gnss tech based on specified configuration parameters.
 
  @param
  'cfg' : configuration parameters for which rfc_data to be stored.
  'ptr' : ptr to the rfc_data.
 
  @retval
  TRUE - if the rfc_data is stored or FALSE if error.
*/
boolean rfc_configurable_test_card_gnss_data::sig_cfg_data_set( rfc_cfg_params_type *cfg, rfc_sig_info_type *ptr )
{

  boolean ret_val = TRUE;

  rfc_configurable_test_card_gnss_sig_data = ptr;

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the device info. table for gnss tech based on specified configuration
  parameters.

  @details
  This function sets the device info. table for gnss tech based on specified configuration
  parameters.
 
  @param
  'cfg' : configuration parameters for which device info. table to be stored.
  'ptr' : ptr to the rfc_data.
 
  @retval
  TRUE - if the device info table is stored or FALSE if error.
*/
boolean rfc_configurable_test_card_gnss_data::devices_cfg_data_set( rfc_cfg_params_type *cfg, rfc_device_info_type *ptr )
{

  boolean ret_val = TRUE;

  rfc_configurable_test_card_gnss_device_info = ptr;

  return ret_val;
}


