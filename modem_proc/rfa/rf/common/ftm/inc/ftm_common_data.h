#ifndef FTM_COMMON_DATA_H
#define FTM_COMMON_DATA_H
/*! 
  @file
  ftm_common_data.h
 
  @brief
  This file contains prototypes and definitions to be used by Centralized 
  FTM Global Data.
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_data.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/10/14   cd/cri  Merge Pin Cal
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
07/16/13   dj      Removed legacy Tx internal Cal support
03/29/13   cri     Moving the data module to rfcommon_data
02/22/12   sc      Remove q6fw handle and correct for single q6clk handle
05/07/12   aa      Changes for Code Maintainability and data access
04/10/12   id      Add support for Tx Internal Device Cal params
08/12/11   aro     Added Cal timer data structure
06/28/11   aro     Renamed header file to ftm_common_calibration_v3
06/20/11   aro     Added Calibration V3 Data structure and accessor funct
06/20/11   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "ftm_common_calibration_v3.h"
#include "ftm_common_enh_internal_device_cal.h"
#include "ftm_common_timer.h"
#include "ftm_calibration_data_module.h"
#include "npa.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to keep track of RF Calibration Data
*/
typedef struct
{

  ftm_calibration_data_type cal_v3;
  /*!< The member stores Calibration V3 Data. */

  ftm_enh_internal_device_cal_data_type enh_internal_device_cal_data;

  ftm_cal_data_module_type data_module;
  /*!< This member stores calibration data between sweeps. */

  ftm_cal_data_delay_map_type et_delay_map;
  /*!< This member stores ET path delay data for 16 channels x 3 LTE BW. */

  ftm_cal_data_pin_map_type pin_map;
  /*!< This member stores Pin data for up to 16 channels. */

} ftm_common_cal_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure that contains data related to Processor Clock Speed. Currently, this 
  structure contains NPA-related data types, since this is the module used to 
  install and uninstall timers 
*/
typedef struct
{
  boolean handle_initialised;
  /*!< Flag indicating if q6clk NPA handle is initialized. This will be set to 
  TRUE only if handle is successfully initialized. */
	
  npa_client_handle npa_handle_q6clk;
  /*!< NPA Handle for Q6CLK */
} ftm_common_proc_clock_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Data used by Common FTM. There should not
  be stray global variables. If global variable is required, it must be 
  contained within this structure. 
*/ 
typedef struct
{
  ftm_common_cal_data_type cal_data;
  /*!< The member sub-structure stores data used for RF Calibration. This
  field will be reused for different devices assuming that only one device
  will be calibrated at once. */

  ftm_enh_internal_device_cal_data_type enh_internal_device_cal_data;
  
  ftm_common_timer_type cal_timer;
  /*!< The member stores the information pertaining to calibration timer */  

  ftm_common_proc_clock_type proc_clock;
  /*!< The member sub-structure stores the Timer used by FTM. This timer 
  structure will be used when using TIMER ISR to do RF Calibration. For
  instance: Tx Sweep Calibration. */

} ftm_common_data_type;

/*----------------------------------------------------------------------------*/
ftm_calibration_data_type*
ftm_common_data_get_cal_v3_ptr
(
  void
);

/*----------------------------------------------------------------------------*/
ftm_enh_internal_device_cal_data_type*
ftm_common_data_get_enh_internal_device_cal_ptr
(
  void
);

/*----------------------------------------------------------------------------*/
ftm_common_timer_type*
ftm_common_data_get_timer_ptr
(
  void
);

/*----------------------------------------------------------------------------*/
ftm_common_proc_clock_type*
ftm_common_get_proc_clock_ptr
(
  void
);

/*----------------------------------------------------------------------------*/
ftm_cal_data_module_type*
ftm_common_data_get_data_module_ptr
(
  void
);

/*----------------------------------------------------------------------------*/
ftm_cal_data_delay_map_type*
ftm_common_data_get_delay_table_ptr
(
  void
);

/*----------------------------------------------------------------------------*/
ftm_cal_data_pin_map_type*
ftm_common_data_get_pin_table_ptr
(
  void
);
/*---------------------------------------------------------------------------*/
uint32 ftm_common_data_get_curr_channel(void);


#ifdef __cplusplus
}
#endif

#endif /* FTM_COMMON_DATA_H */

