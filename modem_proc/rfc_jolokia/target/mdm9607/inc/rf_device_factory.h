#ifndef RF_DEVICE_FACTORY_H
#define RF_DEVICE_FACTORY_H
/*!
  @file
  rf_device_factory.h

  @brief

*/

/*==============================================================================
Copyright (c) 2011 - 2014 by QUALCOMM Technologies Inc.  All Rights Reserved.

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/target/mdm9607/inc/rf_device_factory.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/02/14   vv      Wrapper api to create 3rd party logical devices.
06/09/14   vv      Update physical device creation with new physical device structure for WTR,QFE15xx and QFE25xx
04/24/14   lp      Ported rf_device_factory_create_asd_tuner_device changes
04/23/14   sd      Initial Revision

==============================================================================*/
#include "rfc_common.h"
#include "rfdevice_class.h"
#include "rfdevice_physical_device.h"
#include "rfcommon_atuner_manager_factory.h"

rfdevice_class* 
rf_device_factory_create_tuner_manager
(
  rfdevice_class* device_obj,
   rfcommon_atuner_manager_factory_type manager_type
);

rfdevice_class* 
rf_device_factory_create_device
(
  rfc_device_cfg_info_type* cfg
);

rfdevice_physical_device*
rf_device_factory_create_phys_device
(
   rfc_phy_device_info_type* phy_device_cfg
);

boolean
rf_device_factory_create_gnss_device
( 
  rfc_device_info_type* cfg
);

boolean rf_device_factory_program_device_id
(
   rfc_phy_device_info_type* cfg
);

rfdevice_logical_component* create_gen_device_object(
                                                        rfdevice_physical_device     *rfdevice_physical_third_party_p,
                                                        rfc_logical_device_info_type      *logical_device_cfg
                                                    ); 

#ifdef FEATURE_RF_ASDIV
rfdevice_class* 
rf_device_factory_create_asd_tuner_device 
( 
  void
);
#endif
#endif
