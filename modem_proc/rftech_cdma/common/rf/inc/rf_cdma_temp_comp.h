#ifndef RF_CDMA_TEMP_COMP_H
#define RF_CDMA_TEMP_COMP_H

/*!
  @file
  rf_cdma_temp_comp.h

  @details
  This file exports the definitions and declarations to be used by 1x and HDR
  temperature compensation algorithms.
*/

/*==============================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_temp_comp.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/16/14   cd      Cleanup unused MDSP interfaces and Tx AGC functionality
05/07/13   cd      Fixed API header inclusion
05/07/13   cd      Temp Comp - Support legacy and xPT temp comp algorithms
12/05/12   hdz     Removed rf_cdma_temp_comp_set_status()
                   rf_cdma_temp_comp_clear_status() and
                   rf_cdma_temp_comp_get_status() due to restructure of temp comp
12/03/12   APU     Added a one time check for therm cal  monotonicity. 
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/07/12   hdz     Added rf_cdma_temp_comp_get_therm_index_remainder()
04/18/12   hdz     Changed interface of rf_cdma_temp_comp_read_handler() to
                   handle cmd from rf_apps_task.
03/23/12   hdz     Added rf_cdma_temp_comp_set_status()
                   rf_cdma_temp_comp_clear_status()
                   rf_cdma_temp_comp_get_status()
03/08/16   hdz     Renamed temp_comp.is_combined_with_freq_comp to 
                   temp_comp.init_trigger 
01/23/12   hdz     Added API rf_cdma_get_therm_reading()
01/05/12   hdz     Added temp_comp_status 
01/04/12   hdz     Added rf_cdma_obtain_hkadc_lock()
12/27/11   bmg     Changed mdsp_poll_timeout to signed integer since it is used
                   as a count-down variable.
                   Documentation improvements.
12/22/11   hdz     Add mdsp_poll_timeout in temp comp data type
12/02/11   hdz     Moved 1xhdr temp comp common functions to here
01/03/11   sty     Initial Release

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfcommon_locks.h"
#include "rfc_cdma.h"
#include "timer.h"
#include "rf_apps_task.h"
#include "rfm_gain_state_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Interval between two consecutive Temp sensor read in ms*/
#define RF_CDMA_TEMP_SENSOR_MDSP_POLLING_PERIOD_S          10000

/*! Interval between polling timer start and the first expiry in ms*/
#define RF_CDMA_TEMP_SENSOR_MDSP_FIRST_DELAY_S             10000

/*! timer id for 10s timer*/
#define TIMER_ID_10S  0

/*! timer id for 3ms timer*/
#define TIMER_ID_3MS  1

/*! union of a generic data address in 32 bits system and an array of 4 uint8 */
typedef union
{
  /*! address of generic data in uint32 */
  uint32 funcpt_handler;  

  /*! address of generic data in array of uint8 */
  uint8  addr_array[4];

} rf_cdma_temp_comp_funcpt_handler_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data structure containing all state information specific to the temperature
  compensation module.
*/
typedef struct
{
  /*! Flag indicating if Temp Comp initialization has been done by calling 
      rf_cdma_1x_temp_comp_init() for a device */
  boolean init_done;

  /*! Current technology of operation (RFM Mode). Can be 1x or HDR */
  rfm_mode_enum_type mode;

  /*! Device which this data corresponds to */
  rfm_device_enum_type device;
  
  /*! Pointer to logical device Temp Comp is currently operating ON. We store this
   in temp_comp_data to make it accessible in callback functions when needed */
  const rfc_cdma_logical_device_params_type* logical_device;
  
  /*! Most recent Temp Sensor reading recorded for debugging and logging */
  uint16 last_temp_sensor_read;

  /*! Valid or invalid of last thermistor reading */
  boolean is_last_reading_valid;

  /*! Indicate if the them_comp is combined with freq_comq or not*/
  boolean init_trigger;
 
  /*! Timer handle used to trigger Temp sensor reads periodically thru MDSP in 
   acquisition as well as tracking mode */
  timer_type temp_sensor_read_timer;

  /*! Timer used to poll mdsp_count in mdsp share memory */
  timer_type mdsp_polling_timer;
   
  /*! critical section protection for Temp Comp operations */
  rf_lock_data_type crit_section;

  /*! Flag to enable debug mode. More F3s */
  boolean debugging_enabled;

  /*!
    @brief
    Firmware HKADC transaction counter from before the current therm read
    was requested.

    @details
    The firmware HKADC transaction counter must increment from this value
    before it is safe to read the HKADC result from the RTR.  Until the
    transaction counter increments, the firmware has not executed the
    HKADC transaction.
  */
  uint32 prev_fw_read_count;

  /*!
    @brief
    Counts the number of times that temp comp has aborted due to the HKADC
    being busy in the 10s update.

    @details
    If this count exceeds SLOW_POLLING_LIMIT, temp comp will go into 3ms
    polling mode, to insure a recent temp reading can be made.
  */
  uint8 slow_poll_skip_count;

  /*!
    @brief
    Counts the number of times that temp comp has aborted due to the HKADC
    being busy in the 3ms fast updated.

    @details
    If this count exceeds QUICK_POLLING_TIMEOUT, temp comp will abort fast
    polling mode and will again wait until the next 10s timer to try again.
  */
  int8 quick_poll_countdown_timeout;

  /* temp_comp_status = TRUE means temp comp algorithm is running;
     temp_comp_status = TURE will be set only in rf_temp_comp_start(); 
     temp_comp_status = FALSE means temp comp algorithm is sleeping  ;
     temp_comp_status = FALSE will be set in rf_temp_comp_stop() and
                                             rf_temp_comp_init();
 */
  boolean temp_comp_status;

  /* Current temp comp algorithm in effect */
  rfcom_txlin_temp_comp_algo_type temp_comp_algo;

} rf_cdma_temp_comp_data_type;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_temp_comp_init
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_temp_comp_data_type *temp_comp_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_temp_comp_start
(
  rf_cdma_temp_comp_data_type *temp_comp_data,
  rfm_mode_enum_type mode,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_temp_comp_stop
(
  rf_cdma_temp_comp_data_type *temp_comp_data
);

/*----------------------------------------------------------------------------*/
void 
rf_cdma_temp_comp_read_handler
( 
  rf_apps_cmd_type* cmd_ptr 
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_enable_temp_comp
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_disable_temp_comp
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_obtain_hkadc_lock
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_release_hkadc_lock
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
uint16
rf_cdma_get_therm_reading
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
uint8 
rf_cdma_temp_comp_get_therm_index_remainder
(
  rfm_device_enum_type device,
  uint8 *remainder,
  uint16 *therm_raw
);


/*----------------------------------------------------------------------------*/
boolean
rf_cdma_temp_comp_check_monotonicity
(
  uint16    items[] ,
  uint16    size
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_temp_comp_update_algo
(
  rf_cdma_temp_comp_data_type *temp_comp_data,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rfm_cdma_band_class_type band
);

#ifdef __cplusplus
}
#endif

#endif  /* RF_CDMA_TEMP_COMP_H */

