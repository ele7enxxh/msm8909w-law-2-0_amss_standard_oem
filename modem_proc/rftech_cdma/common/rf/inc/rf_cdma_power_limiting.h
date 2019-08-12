#ifndef RF_CDMA_POWER_LIMITING_H
#define RF_CDMA_POWER_LIMITING_H

/*!
  @file
  rf_cdma_power_limiting.h

  @details
  This file exports the APIs needed to operate and update CDMA power limiting
  module. It also contains definitions of data structures and enums used by this 
  module.
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_power_limiting.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/15/13   APU     Added changes for Enhanced MCDO coonection setup.
05/02/13   APU     1> CCP is now a seperate module that contributes to the HDR 
                      MAX power module.
                   2> Placeholder for clamping HDR MAX power tables sent to FW when 
                      CCP values are greater than SAR values.
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/06/12   hdz     Added tx_pwr_override and tx_override_pwr_limit_dbm10
07/26/12   APU     Added Enhanced PA backoff support.
02/02/12   cd      Added API to get current minimum of max power limits from 
                   1x HDR PLIM modules 
01/27/12   hdz     Remove fcomp from max power limit module 
01/19/12   cd      Store max power linearizer limit in dBm10
11/11/11   Saul    Thermal Backoff Support.
11/10/11   Saul    SV Limiting. Initial support.
11/03/11   cd      Add a field in power limiting module to store max linearizer 
                   power 
10/13/11   aro     Added interface to enable and disable various debug mode
08/29/11   shb     Added mode and override_enabled members in plim data struct
08/24/11   shb     Appended db* to plim data names for clarity of units since 
                   some members are dbm10 while others are db640 (signed tx agc)
08/23/11   shb     Added plim_supported flag in plim_data
08/19/11   shb     Added member to track err adjusted TX power limit
08/17/11   shb     Added critical section support and other data types
08/15/11   shb     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfc_cdma.h"
#include "rfcommon_locks.h"
#include "rfm_cdma_band_types.h"
#include "rfm_hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration used to track state of power limiting module

  @details
  Power limiting module can either be "disabled" (Not accepting updates from 
  clients or sending power limit updates to firmware) or "tracking" (accepting
  updates from clients but not updating hw) or "limiting" (accepting
  updates as well as sending power limit updates to firmware).
*/
typedef enum
{
  /*! Power limiting module is completely OFF. It is not accepting 
  updates or applying updates to hardware */
  RF_CDMA_PLIM_DISABLED,

  /*! Power limiting module is full enabled. It is accepting updates from 
  clients and actively updating limit in firmware. This is the expected state
  of operation when a CDMA device is in TX state. */
  RF_CDMA_PLIM_ENABLED
} rf_cdma_plim_state_type;

/*----------------------------------------------------------------------------*/


/*! 
  @brief
  Enum to indicate if 1x MAX power or HDR MAX power without carrier imbalance
  info [legacy] or Enhanced HDR max power should be used.
*/
typedef enum
{
  RF_HDR_MAX_POWER_ERROR = -1 ,

  RF_HDR_USE_1X_MAX_POWER = 0 , /* Default */

  RF_HDR_USE_LEGACY_HDR_MAX_POWER ,
    
  RF_HDR_USE_ENHANCED_HDR_MAX_POWER 

} rf_hdr_max_power_algos ;

typedef struct
{
  /*! Indicates if 1x MAX power or HDR MAX power without carrier imbalance info 
		[legacy] or Enhanced HDR max power should be used. */
  rf_hdr_max_power_algos max_power_algo ;

  /*! If Max power needs to be overridden by some entity say L1, 
    test-harness etc */
  boolean override_max_power ; 
} rf_hdr_max_power_algo_used_type ;

/*----------------------------------------------------------------------------*/

/*! 
  @brief
  Structure to store enhanced PA back off related entries for DO Rev B
*/
typedef struct
{
  /*! Holds the information read from NV for HDR max power */
	const rf_cdma_tx_pwr_limit_data_type* hdr_max_power ;

	/*! Goes to L1.  	An additional funneling bias is suppled for each assigned 
		 carrier based on the carrier configuration */
	rfm_hdr_per_carrier_funneling_bias_type funnel_info[RFM_CDMA_CARRIER_NUM] ;

	/*! Indicates if 1x MAX power or HDR MAX power without carrier imbalance info 
		[legacy] or Enhanced HDR max power should be used. */
  rf_hdr_max_power_algo_used_type algo_used ;

	/*! 3-bit vector LUT_indicator, spread in a 3 element array, 
	    corresponding to a Hi_Backoff_LUT and Lo_Backoff_LUT, 
		  where N is the number of active carriers in the channel configuration.  */
	boolean freq_sorted_lut_indicator[RFM_CDMA_CARRIER_NUM] ;

  /*! Power difference between current power and power retrieved 
      from the NV. The power in the NV was measured at a
      reference temperature*/
  int16 temp_adjusted_power_difference_dB10 ; 

  /*! Power retrieved from the NV for current temperature*/ 
  int16 max_hdr_ccp_pwr_dB10 ; 

  /*! The table that keeps all the therm cal values for a given CCP */
  const rf_cdma_therm_properties_type* therm_cal_info ;

  /*! Turn on the messages for debugging */
  boolean debug_hdr_max_power ;

} rf_cdma_enhanced_power_limiting_type ;

/*----------------------------------------------------------------------------*/

/*! 
  @brief
  Structure to store all max power limits
*/
typedef struct
{
  
  /*! Current desired TX max power limit from SAR module in dBm10 units */
  int16 max_power_limit_sar_dbm10;

  /*! Current desired TX max power limit from SV module in dBm10 units */
  int16 max_power_limit_sv_dbm10;

  /*! Current desired TX max power limit Thermal Backoff module in dBm10 units */
  int16 max_power_limit_thermal_backoff_dbm10;

  /*! Current desired Tx max power limit from Lim vs Temp vs Freq NV in dBm10 */
  int16 max_power_limit_vs_temp_vs_freq_dbm10;

  /*! Current HDR Tx max power limit LUTs for Enhanced HDR Max power algo. */
  rf_cdma_enhanced_power_limiting_type max_power_info ; 

  /* WARNING          WARNING          WARNING
  If adding a max limit from another module, it needs to be accounted for
  in the min of all max limits calculation */

} rf_cdma_max_power_limits_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to store all relevant data for power limiting module
*/
typedef struct
{
  /*! Flag indicating if Power Limiting data has been initialized for a given 
  device */
  boolean init_done;

  /*! Flag indicating if Power Limiting is supported on a device */
  boolean plim_supported;

  /*! Flag tracking override state - whether power limiting module is actively
  tracking max/min limits or it is overriden and fixed to some externally 
  provided value */
  boolean override_enabled;

  /*! Current state of operation of power limiting module. Can be enabled 
  (actively updating power limit) or disabled */
  rf_cdma_plim_state_type state;

  /*! Current mode (technology) of operation - 1x or HDR */
  rfm_mode_enum_type mode;

  /*! Current band of operation */
  rfm_cdma_band_class_type band;

  /*! Holds the TX max power limits from all contributing modules */
  rf_cdma_max_power_limits_type max_power_limits;

  /*! Current max power limit applied to firmware after adjusting for TX error
  estimated by HDET. Units are signed dB640 (Raw FW TXAGC units) */
  int16 max_power_limit_err_adj_db640;

  /*! Current TX min power limit in dbm10 units */
  int16 min_power_limit_dbm10;

  /*! Current estimate in TX power error in dB640 computed by HDET module */
  int16 tx_err_from_hdet_db640;

  /*! Current max linearizer power in dBm10 - after frequency and 
  temperature compensation */
  int16 max_power_linearizer_limit_dbm10;

  /*! critical section protection for HDET operations */
  rf_lock_data_type crit_section;

  /*! Flag to enable debug mode. More F3s */
  boolean debugging_enabled;

  /*! The override valued of Tx power limit in dbm10*/
  int16 tx_override_pwr_limit_dbm10;


} rf_cdma_plim_data_type;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_plim_init
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_plim_data_type* plim_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_plim_configure_debug_mode
(
  rf_cdma_plim_data_type* plim_data,
  boolean debug_en
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_plim_get_curr_max_power_limit
(
  rfm_device_enum_type device,
  rfm_mode_enum_type mode
);

#ifdef __cplusplus
}
#endif

#endif  /* RF_CDMA_POWER_LIMITING_H */
