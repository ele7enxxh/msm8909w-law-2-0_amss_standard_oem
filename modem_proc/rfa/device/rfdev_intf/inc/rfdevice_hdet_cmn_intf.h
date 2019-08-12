#ifndef RFDEVICE_HDET_CMN_INTF_H
#define RFDEVICE_HDET_CMN_INTF_H
/*!
   @file
   rfdevice_hdet_cmn_intf.h

   @brief
   

*/

/*===========================================================================

Copyright (c) 2012 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_hdet_cmn_intf.h#1 $

when       who   what, where, why
--------   ---   ------------------------------------------------------------------- 
03/29/13   adk    Update temp comp (K-sensor) values for QFE TDET
03/16/13   lp     Added qfe1520_hdet_pwr_index_type
03/13/13   aca    Tuner characterization
01/29/13   adk    Added support for rfdevice_hdet_cmn_get_gated_incident_val()
10/26/12   kai    Added rfdevice_hdet_cmn_program_k0_val, removed rfdevice_hdet_cmn_do_k_sensor_cal
10/16/12   kai    Added API getting tech specific HDET object pointer
10/15/12   kai    Added APIs and implementations
09/26/12   vb     Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rfm_mode_types.h"

/*===========================================================================
                         Data Definitions
===========================================================================*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum indicating the device ID/number
*/
typedef enum
{
  RFDEVICE_HDET_DEVICE_0,
  RFDEVICE_HDET_DEVICE_1,
  RFDEVICE_HDET_DEVICE_2,
  RFDEVICE_HDET_DEVICE_3,
  RFDEVICE_HDET_DEVICE_4,
  RFDEVICE_HDET_DEVICE_5,
  RFDEVICE_HDET_DEVICE_6,
  RFDEVICE_HDET_DEVICE_7,
  RFDEVICE_HDET_DEVICE_8,
  RFDEVICE_HDET_DEVICE_9,
  RFDEVICE_HDET_DEVICE_MAX
} rfdevice_hdet_device_num_type;

/* HDET Gain State enums */
typedef enum
{
  RFDEVICE_HDET_GAIN_STATE_0,    /* Gain state 0 */
  RFDEVICE_HDET_GAIN_STATE_1,    /* Gain state 1 */
  RFDEVICE_HDET_GAIN_STATE_2,    /* Gain state 2 */
  RFDEVICE_HDET_GAIN_STATE_3,    /* Gain state 3 */
  RFDEVICE_HDET_GAIN_STATE_4,    /* Gain state 4 */
  RFDEVICE_HDET_GAIN_STATE_5,    /* Gain state 5 */
  RFDEVICE_HDET_GAIN_STATE_6,    /* Gain state 6 */
  RFDEVICE_HDET_GAIN_STATE_7,    /* Gain state 7 */
  RFDEVICE_HDET_GAIN_STATE_8,    /* Gain state 8 */
  RFDEVICE_HDET_GAIN_STATE_NUM,  /* Total number of gain states */
} rfdevice_hdet_gain_state_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration listing QFE1510 HDET paths

  @details
*/
typedef enum
{
  QFE1510_HDET_INC_PWR_INDEX = 0,
  QFE1510_HDET_REF_PWR_INDEX = 1, 
  QFE1510_HDET_PWR_MAX_INDEX
} qfe1510_hdet_pwr_index_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration listing QFE1520 HDET paths

  @details
*/
typedef enum
{
  QFE1520_HDET_INC_PWR_INDEX = 0,
  QFE1520_HDET_REF_PWR_INDEX = 1, 
  QFE1520_HDET_PWR_MAX_INDEX
} qfe1520_hdet_pwr_index_type;

/*===========================================================================
                         Prototypes
===========================================================================*/
/* ----------------------------------------------------------------------- */
/* function prototypes */

boolean rfdevice_hdet_cmn_auto_cal
(
  void *device_ptr, void *buff_obj_ptr, 
  rf_device_execution_type dev_action
);

boolean rfdevice_hdet_cmn_hkadc_set_incident_path 
(
  void *device_ptr, void *buff_obj_ptr,
  rf_device_execution_type dev_action
);

boolean rfdevice_hdet_cmn_hkadc_set_ref_path
(
  void *device_ptr, void *buff_obj_ptr,
  rf_device_execution_type dev_action
);

boolean rfdevice_hdet_cmn_get_incident_val
(
  void *device_ptr, void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint16 *hdet_inc_pwr
);

boolean rfdevice_hdet_cmn_get_ref_val
(
  void *device_ptr, void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint16 *hdet_ref_pwr
);

boolean rfdevice_hdet_cmn_do_dc_cal
( 
  void *device_ptr
);

boolean rfdevice_hdet_cmn_disable_hdet
(
  void *device_ptr, void *buff_obj_ptr,
  rf_device_execution_type dev_action
);

void* rfdevice_hdet_cmn_get_tech_object
( 
  void *device_ptr,
  rfm_mode_enum_type tech
);

/* ----------------------------------------------------------------------- */
/* K Sensor APIs */
boolean rfdevice_hdet_cmn_get_k_sensor_val
( 
  void *device_ptr,
  uint16 *k_val
);

boolean rfdevice_hdet_cmn_program_k0_val
(
  void *device_ptr,
  uint16 k0_val
);

boolean rfdevice_hdet_cmn_update_k_comp
( 
  void *device_ptr
);

int16 rfdevice_hdet_cmn_get_k_comp
( 
  void *device_ptr
);

boolean rfdevice_hdet_cmn_measure_temp_comp
(
  void *device_ptr,
  void *buff_obj_ptr,
  rf_device_execution_type dev_action
);

boolean rfdevice_hdet_cmn_measure_temp_comp_cb
(
  void *device_ptr,
  qfe_script_type* temp_comp_data_ptr
);

/* ----------------------------------------------------------------------- */
/* FTM APIs */
boolean rfdevice_hdet_cmn_set_gain_state
(
  void *device_ptr, void *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_hdet_gain_state_type hdet_gain_state,
  uint8 hdet_pid
);

boolean rfdevice_hdet_setup_power_read
(
  void *device_ptr,
  void *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  uint8 num_avg,
  uint16 wait_time
);

boolean rfdevice_hdet_cmn_get_power
(
  void *device_ptr, void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint8 hdet_pid,
  uint16 *hdet_inc_pwr
);

boolean rfdevice_hdet_cmn_get_dc_offset_val
(
  void *device_ptr,
  uint16 *hdet_inc_dc_offset_val,
  uint16 *hdet_ref_dc_offset_val
);

boolean rfdevice_hdet_cmn_get_gated_incident_val
(
  void *device_ptr, void *buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint16 *hdet_inc_pwr, int delay,
  int number_of_samples
);

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_HDET_CMN_INTF_H */


