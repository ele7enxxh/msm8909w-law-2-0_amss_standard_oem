/*!
   @file
   rfc_card_gnss.cpp

   @brief
   This file contains all the gnss mode specific implementations to support RFC.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2008-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:49 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/gen8/rf/rfc/src/rfc_card_gnss.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
10/31/13   shb     Converted TRX common device interface to C++
05/06/13   pv      Triton Support for multiple transceivers for GPS.
07/17/12   jr      Added support for non-ELNA configuration
07/10/12   rmb     Removed critical section enter/exit and added these in rfc_common.c 
                   for wakeup/sleep function.
02/28/12   sr      made changes to put all the GPIOs into low power mode during sleep.
11/05/11    aak     Enable NPA calls from RFSW for power management on 9x15  
08/25/11   aak     Enable RF NPA voting only for MSM8960 
07/15/11   gy      Enabled command dispatch
05/24/11   dbz     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfc_card.h"
#include "rfc_card_gnss.h"
#include "rfm_path_types.h"
}

#include "rfdevice_rxtx_common_class.h"
#include "rfc_gnss.h"

/* ----------------------------------------------------------------------- */
/*!
  @brief
  Sets the specified path to the technology mode of operation.

  @details
  This funtion will call the card specific version of the set mode API.

  @parameter
  path RF signal path

*/
void rfc_gnss_mode_config( rf_path_enum_type path )
{
  rfc_gnss *rfc_obj = rfc_gnss::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"gnss RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->config_mode_data(path);

  return;
} /* rfc_gnss_mode_config() */

void rfc_gnss_manage_vregs
( 
  rf_path_enum_type path, 
  rfcom_mode_enum_type mode, 
  rfm_path_state state
)
{
  rfc_gnss *rfc_obj = rfc_gnss::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"gnss RFC Obj is NOT created. ",0);
    return;
  }

  if (  state == RF_PATH_SLEEP_STATE)
  {
    /* disable the rf signals, put the signals into low power mode */
    rfc_common_sleep(path, RFM_GNSS_MODE);
  }
  else
  {
    /* enable the rf signals, if the signals are in low power mode */
    rfc_common_wakeup(path, RFM_GNSS_MODE);
  }

  rfc_obj->manage_vregs(path, mode, state);

  return;
}

int32 rfc_gnss_command_dispatch
( 
  rf_path_enum_type path, 
  rfcs_gnss_cmd_type cmd, 
  void *data
) 
{
  rfc_gnss *rfc_obj = rfc_gnss::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"gnss RFC Obj is NOT created. ",0);
    return 0;
  }

  rfc_obj->command_dispatch(path, cmd, data);

  return 1;

}
/* ----------------------------------------------------------------------- */
/*!
  @brief
  gets LNA configuration

  @details
  This funtion will call the card to get LNA configuration.

  @parameter
  lna config type

*/

void rfc_gnss_get_LNA_config
( 
  rfc_gnss_lna_config_type *lna
) 
{
  rfc_gnss *rfc_obj = rfc_gnss::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"gnss RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->get_LNA_config(lna);

  return;

}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  gets All the device configuration needed to operate on GNSS.

  @details
  This funtion will call the card to get device configuration for a specific type of device.

  @parameter
  rfdevice_type_enum_type
 
  @return
  array of pointers to the devices.

*/

rfdevice_rxtx_common_class* rfc_gnss_get_dev_obj
( 
  rfdevice_type_enum_type dev_type, uint8 instance
) 
{
  rfc_gnss *rfc_obj = rfc_gnss::get_instance();
  rfdevice_rxtx_common_class *temp_gnss_dev_obj = NULL;
  
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"gnss RFC Obj is NOT created. ",0);
    return temp_gnss_dev_obj;
  }

  temp_gnss_dev_obj = (rfdevice_rxtx_common_class*)(rfc_obj->get_rf_device_obj(dev_type, instance));

  return temp_gnss_dev_obj;
}
