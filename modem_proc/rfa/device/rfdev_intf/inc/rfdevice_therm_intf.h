#ifndef RFDEVICE_THERM_INTF_H
#define RFDEVICE_THERM_INTF_H
/*!
   @file
   rfdevice_therm_intf.h

   @brief


*/

/*==============================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_therm_intf.h#1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------

09/09/14   vv    Initial version
==============================================================================*/

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "rfm.h"
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"
#include "rfdevice_therm.h"
#include "rfc_class.h"

#ifdef __cplusplus
extern "C" {
#endif


/*==============================================================================
                             rfdevice interface therm APIs
==============================================================================*/


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfdevice API to get the active number of thermistors
 
  @details
  rfdevice API to get the active number of thermistors

  @param   size : out parameter. Returns the number of active thermistors

*/
boolean rfdevice_get_thermistor_count(uint8* size_p);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfdevice interface API to enumerate and return the active thermistors for external use
 
  @details
  rfdevice interface API to enumerate and return the active thermistors for external use

  @param size : in+out parameter. It specifies the number of entries provide in the array by the caller, 
         and is updated to the number of thermistors that have been filled in after the call
  
  @param therm_device_list : out parameter.Array of structures containing handles and any other associated data 
                      that may be used in the future by the thermal algorithm to improve their performance

*/
boolean rfdevice_enumerate_thermistors(uint8* size_p, rfm_thermistor_type* therm_device_list_p);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfdevice API to read and return the temperature for the requested thermistor 
 
  @details
  rfdeivce API to read and return the temperature for the requested thermistor 

  @param handle : in parameter to specify the thermistor to read temperature
  @param result : out parameter which will be updated with the therm read value if the read was successfull

*/
boolean rfdevice_query_thermistor(rfm_thermistor_handle_type handle, rfm_thermistor_result_type* result_p);

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_THERM_INTF_H */


