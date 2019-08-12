#ifndef RFGSM_CORE_TEMP_COMP_H
#define RFGSM_CORE_TEMP_COMP_H

/*!
  @file
  rfgsm_core_temp_comp.h
 
  @brief
  temperature compensation header file for GSM technology.

  @details
  This file exports the definitions and declartions to be used for temp comp
  for GSM technology.
*/

/*==============================================================================

  Copyright (c) 2010 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_core_temp_comp.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/23/15   bp     Stop further processing of therm_read_rsp if temp. compensation is stopped through GSM Tx Disable.
07/01/14   jr     Enable therm read on sub-2 
06/20/14   zbz    Add feature to adjust temp comp table based on therm read during cal
02/28/14   sc     Pass core data through mc to core layers
02/17/14   sc     Clean up mDSP APIs
02/11/14   sw     Remove therm read TRITON featurisation
06/02/14   sw     Therm read to use RFFEs and not SBIs
07/12/13   av     GTA EXIT mode
06/25/13   ec     Make changes to support Therm Read Cancellation
05/30/13   tws    Correct the prototype of rfgsm_core_therm_read_trigger() 
04/09/13   tsr    Added RF APPS->RF Task MSGR framework for temp comp to make temp comp data thread safe
03/25/13   tsr    Use device driver api to extract therm read from retrieved ssbi buffer 
03/12/13   tsr    Cleanup the therm read event handle before rfgsm exit
02/07/13   tsr    Ensure that Temp comp calculation is thread safe
02/06/13   tsr    Added GSM temp comp algorithm implementation
01/14/13   tsr    Added changes for Therm read implementation  
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
  Initialize the PA temp comp. clock callback.
*/
void rfgsm_core_temp_comp_init( rfm_device_enum_type rfm_dev );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Starts the callback that calls rfgsm_core_therm_read_trigger .It is called when 
  there is Tx. 
*/
void rfgsm_core_temp_comp_start( rfm_device_enum_type rfm_dev, uint8 sub_id );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stops the callback that calls rfgsm_core_therm_read_trigger .It is called when there 
  is no transmition. 
*/
void rfgsm_core_temp_comp_stop( rfm_device_enum_type rfm_dev );


void rfgsm_core_temp_comp_suspend( rfm_device_enum_type rfm_dev );

void rfgsm_core_temp_comp_resume( rfm_device_enum_type rfm_dev );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the temperature compensation information from the 
  local structure.
*/
uint8 rfgsm_core_temp_comp_bin_index_get( rfm_device_enum_type rfm_dev );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function updates the temperature compensation information from the 
  rf apps task.
*/
void rfgsm_core_temp_comp_bin_index_update( rfgsm_core_handle_type *rfgsm_core_handle_ptr, uint8 temp_bin_16 );


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the PA temp comp value for the max power PCL.
*/
int16 rfgsm_core_temp_comp_pwr_get(rfm_device_enum_type rfm_dev);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function updates the PA temp comp value for the max power PCL.
*/
void rfgsm_core_temp_comp_pwr_update(rfgsm_core_handle_type *rfgsm_core_handle_ptr, int16 scaled_value);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the bb headroom for temperature compensation based on GSM band.
*/
int32 rfgsm_core_temp_comp_get_bb_headroom(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This funtion performs the thermistor temperature read. 
 
*/
void rfgsm_core_therm_read_handler(rf_apps_cmd_type* cmd_ptr);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This funtion performs cleanup the last therm read event handle.
 
*/
void rfgsm_core_therm_read_event_cleanup(rfm_device_enum_type rfm_dev);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This funtion performs cleanup therm read event handler memory on apps task.
*/

void rfgsm_core_therm_read_event_cleanup_handler(rf_apps_cmd_type* cmd_ptr);

/*----------------------------------------------------------------------------*/
void rfgsm_core_construct_read_therm_script_buffer(rfgsm_core_handle_type *rfgsm_core_handle_ptr, 
                                                   rf_hal_bus_rffe_type* read_buf,
                                                   uint16 num_retrieved);

void rfgsm_core_construct_read_therm_ssbi_script_buffer(rfgsm_core_handle_type *rfgsm_core_handle_ptr,
                                                        rf_hal_bus_sbi_type* read_buf_ptr,
                                                        uint16 num_retrieved);

/*----------------------------------------------------------------------------*/
void rfgsm_core_free_read_therm_script_buffer(rfgsm_core_handle_type *rfgsm_core_handle_ptr);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function will trigger the rf_app task to apply(Calculate) the tx power
  temperature compensation based on recent therm read value. 
  This can be called upon the receipt of recent therm read event or whenever the
  Tx band changes */
void rfgsm_core_temp_comp_trigger(rfgsm_core_data_type *core_data);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calculates the amount of tx power compensation 
  based on current temperature.
*/
void rfgsm_core_temp_comp_handler(rf_apps_cmd_type* cmd_ptr);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is to read the thermistor and is called evey 2seconds or whenever tx enabled. 
  This function will read the thermistor value, the read thermistor value will be used for
  tx power temperature compensation.
*/
void rfgsm_core_therm_read_trigger(uint32 t_unused, timer_cb_data_type data);

/*----------------------------------------------------------------------------*/
 boolean rfgsm_core_process_therm_read_rsp(rfgsm_core_data_type *core_data, uint8 tag, boolean therm_read_ok);

int16 rfgsm_core_temp_comp_proces_raw_therm_read(rfm_device_enum_type rfm_dev, uint16 current_therm_read);

void rfgsm_core_temp_comp_flag_set(rfgsm_core_handle_type *rfgsm_core_handle_ptr);

void rfgsm_core_temp_comp_flag_clear(rfm_device_enum_type rfm_dev);

void rfgsm_core_temp_comp_update_req(rfm_device_enum_type rfm_dev, int16 temp_comp_scaled_value, uint8 temp_bin_16, uint8 sub_id);

void rfgsm_core_temp_comp_tbl_adjust(rfm_device_enum_type rfm_dev);

#ifdef __cplusplus
}
#endif

#endif /* RFGSM_CORE_TEMP_COMP_H */
