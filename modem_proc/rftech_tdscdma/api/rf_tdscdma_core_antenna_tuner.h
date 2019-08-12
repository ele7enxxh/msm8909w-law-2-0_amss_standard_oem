#ifndef RF_TDSCDMA_CORE_ANTENNA_TUNER_H
#define RF_TDSCDMA_CORE_ANTENNA_TUNER_H

#ifdef FEATURE_RF_HAS_QTUNER

/*!
  @file
  rf_tdscdma_core_antenna_tuner.h

  @brief
  Provides TDSCDMA antenna tuner functionality

  @details

*/

/*===========================================================================

Copyright (c) 2012 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rf_tdscdma_core_antenna_tuner.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
06/22/14   ych	   Add tuner_disable for both sleep/exit and Tx mode in IRAT
06/03/14   jz      Add tuner tune code override command
04/24/14   ych     Added dynamic tuner script for ifreq
02/12/14   ms      Disable tuner in X2T iRAT case (porting per CR600367)
04/23/13   jyu     Changed the interface for program_rx/tx_mode 
04/10/13   ndb     Removed the unused code 
03/21/13   ms      Tuner TDET support 
11/19/12   kai     Initial version (ported from WCDMA)

============================================================================*/
#include "rfcom.h"
#include "rfdevice_antenna_tuner_intf.h"
#include "rf_tdscdma_core_util.h"


#if 0
/*---------------------------------------------------------------------------*/
boolean rf_tdscdma_core_create_tuner_managers(void);
#endif

/*---------------------------------------------------------------------------*/
boolean rf_tdscdma_core_antenna_tuner_init
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type band,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer
);

/*---------------------------------------------------------------------------*/
boolean rf_tdscdma_core_antenna_tuner_disable
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type band,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing  
);

/*---------------------------------------------------------------------------*/
boolean rf_tdscdma_core_antenna_tuner_program_rx_mode 
( 
  rfm_device_enum_type rfm_device,
  rfcom_tdscdma_band_type band,
  uint16 chan_num,
  rf_device_execution_type rf_tdscdma_exec_type,
  rf_buffer_intf* buffer_ptr,
  int16 script_timing 
);

/*---------------------------------------------------------------------------*/
boolean rf_tdscdma_core_antenna_tuner_program_tx_mode 
( 
  rfcom_device_enum_type device,
  rfcom_tdscdma_band_type band,
  uint16 chan_num,
  rf_device_execution_type rf_tdscdma_exec_type,
  rf_buffer_intf* buffer_ptr,
  int16 script_timing 
);

/*----------------------------------------------------------------------------*/
boolean rf_tdscdma_core_antenna_tuner_tx_mode_disable 
( 
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type band, 
  uint16 chan_num,
  rf_device_execution_type rf_tdscdma_exec_type,
  rf_buffer_intf* buffer_ptr,
  int16 script_timing
);

/*---------------------------------------------------------------------------*/
uint32 rf_tdscdma_core_antenna_tuner_tune_code_override( rfm_device_enum_type device,
                                                         rfcom_tdscdma_band_type band,
                                                         uint8 override_flag,
                                                         void *data,
                                                         uint8* tuner_nv_ptr,
                                                         uint8 *tuner_id_ptr);

#if 0
/*---------------------------------------------------------------------------*/
boolean rf_tdscdma_core_antenna_tuner_disable 
( 
  rfcom_device_enum_type device
);

/*---------------------------------------------------------------------------*/
boolean rf_tdscdma_core_antenna_program_scenario_standalone
( 
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type band,
  uint16 chan,
  void *buff_obj_ptr
);

/*---------------------------------------------------------------------------*/
boolean rf_tdscdma_core_antenna_tuner_get_meas_script
( 
  boolean is_tune_away_script,
  rfcommon_core_antenna_tuner_device_info_type *device_info,
  uint8 num_of_devices_active,
  void *buf_obj_ptr
);



/******************************************************************************
	                               FTM APIs
******************************************************************************/

/*---------------------------------------------------------------------------*/
boolean rf_tdscdma_core_antenna_program_scenario_immediately(void);

/******************************************************************************
	                           WL1 INTERFACE APIs
******************************************************************************/

/*---------------------------------------------------------------------------*/
boolean rf_tdscdma_core_antenna_tuner_incremental_update ( void );

/*---------------------------------------------------------------------------*/
boolean rf_tdscdma_core_antenna_tuner_meas_cb_register( rfm_new_scenario_cb_type* new_scenario_cb);

/*---------------------------------------------------------------------------*/
void rf_tdscdma_core_antenna_tuner_notify_new_scneario (void);

/*---------------------------------------------------------------------------*/
boolean rf_tdscdma_core_antenna_tuner_meas_script_success( void );

#endif

#endif /* FEATURE_RF_HAS_QTUNER */

#endif /* RF_TDSCDMA_CORE_ANTENNA_TUNER_H */
