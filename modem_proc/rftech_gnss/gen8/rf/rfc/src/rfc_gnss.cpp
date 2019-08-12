/*!
   @file
   rfc_gnss.cpp

   @brief
   This file contains all the gnss mode specific implementations to support RFC.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2011 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:49 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/gen8/rf/rfc/src/rfc_gnss.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
05/06/13   sr/pv   Triton Support for multiple transceivers for GPS.
03/06/13   vrb     Requirement from RFC 2.13.132: populate req enum in RFC API
01/07/13   shb/dbz Moved GNSS device construction to config_mode_data
10/30/12   sr      changes create tech rfc object if the tech is supported by 
                   the specific rf-card.
07/17/12   jr       Added support for non-ELNA configuration.
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/23/12   sr       Move the manage_vregs() to base-class from rf-card specific files.
05/18/12   sr       RFC-EFS code clean-up changes.
05/08/12   sr       Added binary RFC support for GNSS.  
03/23/12   sr       RFC Clean-up.  
05/31/11   dbz      Initial Version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfc_card.h"
#include "rfc_gnss.h"
#include "rfc_common.h"
#include "rfc_vreg_mgr.h"
#include "rfc_gnss_data.h"
#include "rfcommon_msg.h"

rfc_gnss *rfc_gnss::rfc_gnss_ptr = (rfc_gnss *)NULL;

rfc_gnss * rfc_gnss::get_instance(void)
{
  return rfc_gnss_ptr;
}

// singleton create object
rfc_gnss * rfc_gnss::create_instance(void)
{
  if (rfc_gnss_ptr == NULL)
  {
    rfc_gnss_ptr = (rfc_gnss *)new rfc_gnss();
  }
  return rfc_gnss_ptr;
}

// Constructor 
rfc_gnss::rfc_gnss()
{
  uint8 dev_instance = 0;

  // initialize the gnss_trx_devices
  for (dev_instance = 0; dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND; dev_instance++)
  {
    gnss_trx_devices[dev_instance] = NULL;
  }

  init_rf_devices();

} /* rfc_gnss::rfc_gnss() */

// Destructor
rfc_gnss::~rfc_gnss()
{
  rfc_gnss_ptr = NULL;
}



rfc_gnss_attribute_type *rfc_gnss::get_mode_config(rf_path_enum_type path)
{
  return NULL; //@todo for compilation only; fix it later
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific intializations for GNSS mode

   @details
   Sets up GNSS device interface
*/
void rfc_gnss::config_mode_data(rf_path_enum_type path)
{
  //rfc_cfg_params_type cfg;
  //rfc_device_info_type *device_info = NULL;
  //rfc_gnss_data *rfc_data = rfc_gnss_data::get_instance();
  //
  //if (rfc_data == NULL)
  //{
  //  RF_MSG( RF_ERROR,"rfc_data == NULL!" );
  //  return;
  //}
  //
  //rfc_data->devices_cfg_data_get(&cfg, &device_info);
  //
  //if (device_info == NULL)
  //{
  //  RF_MSG( RF_ERROR, "rfc_gnss::config_mode_data. Null device data !");
  //  return;
  //}
  //
  //if (rf_device_factory_create_gnss_device(device_info) == FALSE)
  //{
  //  RF_MSG( RF_ERROR, "rfc_gnss::config_mode_data. Device creation failed!" );
  //}


} /* rfc_gnss::config_mode_data */


void rfc_gnss::command_dispatch(rf_path_enum_type path, int32 cmd, void* data)
{
  rfc_cfg_params_type cfg;
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_gnss_data *rfc_data = rfc_gnss_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_data == NULL!", 0);
    return;
  }

  cfg.alternate_path = 0;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);

  switch ((rfcs_gnss_cmd_type) cmd)
  {
  case RFC_GNSS_ELNA_ON:
    rfc_common_config_signal(rf_signals);
    break;

  case RFC_GNSS_ELNA_OFF:
    rfc_common_init_default(rf_signals);
    break;

  default:
    break;
  }
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures the vregs for GNSS mode

   @details

*/
void rfc_gnss::manage_vregs(rf_path_enum_type path,
                            rfcom_mode_enum_type mode,
                            rfm_path_state state)
{
  rfc_vreg_param vreg_param(path, mode);
  vreg_param.set_band(0);
  vreg_param.set_rfchip_number(RF_CHIP_0);
  vreg_param.set_rf_state(state);

  rfc_vreg_mgr *vreg_mgr_p = rfc_vreg_mgr::get_instance();
  if (vreg_mgr_p != NULL)
  {
    vreg_mgr_p->gps_manage_power(vreg_param);
  }
  else
  {
    ERR_FATAL("Cannot Create VREG_MGR",0,0,0);
  }

}
/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures external LNA control values

   @details

*/
void rfc_gnss::get_LNA_config (rfc_gnss_lna_config_type* lna)
  {
  /* Default, external LNA is Active */
  *lna=RFC_GNSS_ELNA;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the rfdevices with the rfc dependent data.

  @details
  This function initializes the rfdevices with the rfc dependent data.

*/
void rfc_gnss::init_rf_devices()
{
  rfc_cfg_params_type cfg;
  rfc_device_info_type *device_info = NULL;
  rfdevice_class *cmn_dev_obj = NULL;
  uint8 i = 0;
  uint8 local_instance = 0; 
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  uint32 instance = 0;

  rfc_intf *rfc_cmn = rfc_intf::get_instance();
  if (NULL == rfc_cmn)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC CMN Object is NOT Created", 0);
    return;
  }
  
  rfc_gnss_data *rfc_data = rfc_gnss_data::get_instance();
  if (rfc_data == NULL)
  {
    RF_MSG( RF_ERROR,"rfc_data == NULL!" );
    return;
  }
  
  cfg.alternate_path = 0;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  rfc_data->devices_cfg_data_get(&cfg, &device_info);  
  if (device_info == NULL)
  {
    RF_MSG( RF_ERROR, "rfc_gnss::config_mode_data. Null device data !");
    return;
  }

  i = 0;
  local_instance = 0;
  while (i < device_info->num_devices && local_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
  {
    dev_type = device_info->rf_asic_info[i].device_type;
    instance = device_info->rf_asic_info[i].instance;
    cmn_dev_obj = rfc_cmn->get_cmn_rf_device_object(dev_type, instance);

    if ((cmn_dev_obj == NULL) || (dev_type != RFDEVICE_TRANSCEIVER))
    {
      i++;
      continue;
    }

    gnss_trx_devices[local_instance] = cmn_dev_obj;
    local_instance++;
    i++;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the requested rf device object information.

  @details
  This function returns the requested rf device object information.

*/
rfdevice_class *rfc_gnss::get_rf_device_obj(rfdevice_type_enum_type dev_type,
                                             uint8 instance)
{
  rfdevice_class *dev_obj = NULL;

  if (dev_type != RFDEVICE_TRANSCEIVER)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gnss::get_device_obj(%d): Incorrect Parameters",
          dev_type);
    return dev_obj;
  }

  dev_obj = gnss_trx_devices[instance];

  return dev_obj;
}

