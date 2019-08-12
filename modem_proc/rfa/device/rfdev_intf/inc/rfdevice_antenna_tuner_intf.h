#ifndef RFDEVICE_ANTENNA_TUNER_INTF_H
#define RFDEVICE_ANTENNA_TUNER_INTF_H
/*!
   @file
   rfdevice_antenna_tuner_intf.h

   @brief
   

*/

/*===========================================================================

Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_antenna_tuner_intf.h#1 $

when       who   what, where, why
--------   ---   -------------------------------------------------------------------
09/27/13   yb    Removing tune_code_override from rfdevice_antenna_tuner_intf layer.
07/10/13   aca   RL API expansion
07/08/13   aca   RL Logging
06/26/13   aca   RL FTM API v2
06/12/13   aca   RL FTM API 
04/12/13   sr    removed legacy code which is not used.
02/04/13   ndb   Added rfdevice_antenna_tuner_tune_code_override()API
01/21/13   ndb   Added rfdevice_antenna_tuner_tune_code_override()API - To fix the compilier Error in 2.0 branch
01/23/13   aca   Added timing support
01/18/13   adk   Added APIs to retrieve chip version and slave id of antenna tuner
01/07/13   ndb   Added Tech specific Settings support in rx_init
10/05/12   gh    Added API for rf_init, rf_disable, get_provisioned_table_revision
                 and get_provisioned_table_oem_string
10/03/12   kai   Added API rfdevice_antenna_tuner_check_support
09/19/12   gh    Added intf API validating a scenario number based on mode/tech
09/17/12   gh    Added intf API for querying revision and OEM string
08/30/12   gh    Updated and added intf APIs
08/06/12   vb    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"
#include "rfdevice_antenna_tuner.h"
#ifdef __cplusplus
extern "C" {
#endif



/*----------------------------------------------------------------------------*/

boolean rfdevice_antenna_tuner_rl_setup (void *device_ptr);
/*----------------------------------------------------------------------------*/


boolean rfdevice_antenna_tuner_rl_read 
(
  void *device_ptr,
  qfe_antenna_tuner_rl_measurement_data_type *data 
);

  /*----------------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_ANTENNA_TUNER_INTF_H */


