#ifndef RFWCDMA_CORE_TEMP_COMP_H
#define RFWCDMA_CORE_TEMP_COMP_H
/*!
  @file
  rfwcdma_core_temp_comp.h

  @brief
  Provides WCDMA Temperature Compensation functionality

  @details

*/

/*===========================================================================

Copyright (c) 2008 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_core_temp_comp.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/12/15   gh      Add support for therm read to be triggered by FED
09/15/14   vbh     Added device as a parameter to support Tx sharing
08/06/14   aro     Support to override TempComp mode of operation
06/19/14   aro/ak  Added Support for Prep/Exec Temp Comp Split
05/09/14   kai     Add rfwcdma_core_check_temp_comp_done API
02/21/14   rmb     Add global NV pointers for each of band specific static NVs. 
07/24/13   kai     Added XPT path delay offset vs temp vs freq 
05/09/13   cd      Temp Comp - Support legacy and xPT temp comp when xPT is
                   enabled
05/02/13   cd      Added API to convert raw therm read to scaled value
03/20/13   cd      XPT Temp comp support
02/04/13   ka      Added rfwcdma_core_temp_comp_set_enable
03/27/12   dw      Fix max power issue that NV limit was not initalized 
                   properly until temp comp kicks in
03/06/12   gh      16-channel Tx lin_vs_temp feature
09/30/11   dw      Nikel temp comp updates
08/11/10   ka      Update tx temp comp when enabling tx 
01/10/11   kguo    Merge mdm9k dev branch to main\latest 
03/09/09   clk     Code cleanup
11/06/08   av     Initial version.

============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rfcommon_time_profile.h"
#include "rfcommon_locks.h"
#include "rf_dispatch.h"

/*----------------------------------------------------------------------------*/

//kakash new change
/*! Enumeration listing the possible temp comp states */
typedef enum
{
  TEMP_COMP_READY,
  /*!< Indicates that Temp Comp is ready for new cycle */ 

  TEMP_COMP_PREP_COMPLETED,
  /*!< Indicates that the prep command has completed execution */

  TEMP_COMP_PENDING_EXEC
  /*!< Indicates that temp comp was prepared and waiting for execution */

} rfwcdma_temp_comp_state_type ;

/* !@brief: */
typedef struct
{
  boolean temp_comp_enable_flag;
  uint16 last_raw_temp_value;
  uint8  last_scaled_temp_value;
  uint8 therm_min;
  uint8 therm_range;

}rfwcdma_core_temp_comp_value_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the current temp comp data */
typedef struct
{
  boolean override_disable_mode;
  /*!< Flag indicating that Temp Comp needs to be overridden to disabled mode */

  rfwcdma_temp_comp_state_type tc_state;
  /*!< Current temperatature compensation state */

  uint8 last_temperature;
  /*!< Last thermistor read value */

  rf_time_tick_type prep_complete_time;
  /*!< Absolute time when Prep action was completed. */

  rf_lock_data_type *crit_section;
  /*!< Global Mutex to avoid Temp Read and RF sleep */
  
  rfm_cb_handler_type callback;
  /*!< callback function to be executed at the end of prep complete */

  void * userdata;
  /*!< opaque callback data that will be passed with callback function */

  boolean therm_enable_triggered_by_fed;
  /*!< Flag indicating that THERM Setup and Enable are to be done in FED
       Remove this in the future to mainline FED based therm_enable 
   */

} rfwcdma_temp_comp_data_type;

/*--------------------------------------------------------------------------*/
void rfwcdma_core_temp_comp_enable(boolean flag);

/*--------------------------------------------------------------------------*/
/* !@todo: IMPORTANT: Need to investigate why when temp_comp_data update is
called but linearizer isn't updated? */
//void rfwcdma_core_temp_comp_calc_data(void);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_temp_comp_calc_data(rfm_device_enum_type device,
                                      uint8 temp_index, uint8 temp_rem,
                                      uint8 scaled_temperature_read);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_temp_comp_update(rfm_device_enum_type device);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_get_txlin_temp_comp(rfm_device_enum_type device,
                                      int16 pin_comp_val[], 
                                      int16 pout_comp_val[]);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_add_comp_vs_freq_vs_temp(int16 txlin_temp_comp[], uint8 temp_index, uint8 temp_rem);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_temp_txplim_init(rfcom_wcdma_band_type band);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_temp_comp_set_enable(boolean enable);

/*--------------------------------------------------------------------------*/
uint8 rfwcdma_core_temp_comp_get_scaled_therm_read( uint16 raw_therm_read );

/*--------------------------------------------------------------------------*/
uint16 rfwcdma_core_temp_comp_get_raw_therm_read( uint8 scaled_therm );

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_temp_comp_update_algo(rfcom_wcdma_band_type band);

/*--------------------------------------------------------------------------*/
uint16* rfwcdma_core_temp_comp_get_therm_bin_array(void);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_temp_comp_data_init(void);

/*--------------------------------------------------------------------------*/
rf_lock_data_type* rfwcdma_core_temp_comp_get_lock_data(void);

/*--------------------------------------------------------------------------*/
void 
rfwcdma_temp_comp_update_handler
(
  void
);

/*--------------------------------------------------------------------------*/
boolean
rfwcdma_tempcomp_override_disable
(
   rfm_device_enum_type device
);

/*--------------------------------------------------------------------------*/
boolean
rfwcdma_tempcomp_override_enable
(
   rfm_device_enum_type device
);

/*--------------------------------------------------------------------------*/
void
rfwcdma_core_temp_comp_read_therm_handler
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

#endif /* RFWCDMA_CORE_TEMP_COMP_H */
