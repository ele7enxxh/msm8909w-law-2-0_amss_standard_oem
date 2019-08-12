#ifndef RFWCDMA_CORE_ANTENNA_TUNER_H
#define RFWCDMA_CORE_ANTENNA_TUNER_H

#ifdef FEATURE_RF_HAS_QTUNER

/*!
  @file
  rfwcdma_core_antenna_tuner.h

  @brief
  Provides WCDMA antenna tuner functionality

  @details

*/

/*===========================================================================

Copyright (c) 2012 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_core_antenna_tuner.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
08/11/14   rmb     Add create script support for Tx Disable API.
06/04/14   vws     Support script based interface rfwcdma_core_antenna_tuner_program_tx_mode 
06/04/14   vws     port change about tuner tx disable during Tx Disable 
06/04/14   vws     WCDMA IRAT tuner_disable in X2W
03/26/14   kg      Added support for CL LM handle 
03/27/14   ac      porting the latest asd changes from dime
03/20/14   ac      asd changes ported from dime
07/24/13   vs      Pass Closed loop NV info to Tuner 
06/28/13   vs      Add WCDMA support for FTM_PROGRAM_QFE_SCENARIO
04/12/13   sr      removed legacy code which is not used.
04/10/13   ndb     Removed the unused code 
02/14/13   vb      Added Dime specific implementation of tune code override
02/13/13   vb      Support for new tuner manager in WCDMA
02/13/13   vb      Old tuner architure cleanup 
02/06/13   rmb     Added rf_buffer_intf and rf_device_execution_type params
                   to antenna_tuner APIs for wakeup optimization.
12/03/12    vb     Code to support WCDMA Tuner OL - temporary solution
11/16/12    vb     Added band argument for antenna tuner init
10/22/12   vb      Added support for Antenna Tuner's IRAT meas tune away script
10/10/12   kai/vb  Added IRAT interface APIs
10/10/12   kai/vb  Added call back function register API
10/10/12   kai     Changed interface rfwcdma_core_antenna_program_scenario_standalone
09/21/12   kai     Initial version

============================================================================*/
#include "rfcom.h"
#include "rfdevice_antenna_tuner_intf.h"
#include "rfcommon_atuner_intf.h"
#include "rfm_wcdma.h"
#include "rfcommon_mdsp_event_intf.h"
#include "rf_buffer_intf.h"
#include "rfcommon_nv.h"

/*---------------------------------------------------------------------------*/
boolean rfwcdma_core_antenna_tuner_init
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rf_buffer_intf* buffer_ptr,
  rf_device_execution_type rfwcdma_exec_type
);

/*---------------------------------------------------------------------------*/
boolean rfwcdma_core_antenna_tuner_program_rx_mode 
( 
  rfm_device_enum_type rfm_device,
  rfcom_wcdma_band_type band,
  uint16 chan_num,
  rf_device_execution_type rfwcdma_exec_type,
  rf_buffer_intf* buffer_ptr
);

/*---------------------------------------------------------------------------*/
boolean rfwcdma_core_antenna_tuner_program_tx_mode 
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint16 chan_num,
  rf_device_execution_type rfwcdma_exec_type,
  rf_buffer_intf* buffer_ptr,
  rfcommon_atuner_cl_ctrl_info_type *cl_ctrl_info
);

/*---------------------------------------------------------------------------*/
boolean rfwcdma_core_antenna_tuner_disable 
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing
);

/*---------------------------------------------------------------------------*/
boolean rfwcdma_core_antenna_tuner_tx_disable 
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint16 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing
);


/*---------------------------------------------------------------------------*/
uint32 rfwcdma_core_antenna_tuner_tune_code_override( rfcom_device_enum_type device,
                                                      rfcom_wcdma_band_type band,
                                                      uint8 override_flag,
                                                      void *data,
                                                      uint8* tuner_nv_ptr,
                                                      uint8 *tuner_id_ptr);

/******************************************************************************
	                               FTM APIs
******************************************************************************/

/*---------------------------------------------------------------------------*/
boolean rfwcdma_core_antenna_set_scenario(uint8 scenario_val);

/******************************************************************************
	                           WL1 INTERFACE APIs
******************************************************************************/

/*---------------------------------------------------------------------------*/
boolean rfwcdma_core_antenna_tuner_incremental_update ( void );

/*---------------------------------------------------------------------------*/
void rfwcdma_core_antenna_tuner_incremental_update_imm ( uint8 cb_data );

/*---------------------------------------------------------------------------*/

boolean rfwcdma_core_antenna_tuner_program_qfe_scenario ( void );

  #endif /* FEATURE_RF_HAS_QTUNER */

#endif /* RFWCDMA_CORE_ANTENNA_TUNER_H */
