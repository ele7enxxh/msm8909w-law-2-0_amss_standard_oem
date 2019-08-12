#ifndef RFGSM_CORE_VBATT_COMP_H
#define RFGSM_CORE_VBATT_COMP_H

/*!
  @file
  rfgsm_core_vbatt_comp.h
 
  @brief
  vbatt compensation header file for GSM technology.

  @details
  This file exports the definitions and declartions to be used for vbatt comp
  for GSM technology.
*/

/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_core_vbatt_comp.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/03/13   sb      Fixes for vbatt read taking too long
01/29/12   sb      Added changes to obtain vbatt reading for vbatt comp
01/08/13   sr     Initial version

==============================================================================*/

#include "rfgsm_core.h"
#include "rf_apps_task.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the vbatt comp. clock callback.
*/
void rfgsm_core_vbatt_comp_init( rfm_device_enum_type rfm_dev );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Starts the callback that calls rfgsm_core_vbatt_read_trigger .It is called when 
  there is Tx. 
*/
void rfgsm_core_vbatt_comp_start( rfm_device_enum_type rfm_dev );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stops the callback that calls rfgsm_core_vbatt_read_trigger .It is called when there 
  is no transmition. 
*/
void rfgsm_core_vbatt_comp_stop( rfm_device_enum_type rfm_dev );


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the PA temp comp value for the max power PCL.
*/
int16 rfgsm_core_vbatt_comp_pwr_get(rfm_device_enum_type rfm_dev);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function will trigger the rf_app task to apply(Calculate) the tx power
  vbatt compensation based on recent vbatt read value. 
  This can be called upon the receipt of recent therm read event or whenever the
  Tx band changes */
void rfgsm_core_vbatt_comp_trigger(int32 rfm_dev);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calculates the amount of tx power compensation 
  based on current battery voltage.
*/
void rfgsm_core_vbatt_comp_handler(rf_apps_cmd_type* cmd_ptr);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is to read the vbatt and is called evey 10 seconds or whenever tx enabled. 
  This function will read the battery voltage, the read vbatt value will be used for
  tx power compensation.
*/
void rfgsm_core_vbatt_read_trigger(uint32 t_unused, int32 rfm_dev, boolean flag);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This funtion performs the vbatt read. 
 
*/
void rfgsm_core_vbatt_read_handler(rf_apps_cmd_type* cmd_ptr);


void rfgsm_core_set_vbatt_comp_flag(uint32 t_unused, rfm_device_enum_type rfm_dev );

void rfgsm_core_override_vbatt_read(rfm_device_enum_type rfm_dev, uint32 vbatt_val);

#ifdef __cplusplus
}
#endif

#endif /* RFGSM_CORE_VBATT_COMP_H */

